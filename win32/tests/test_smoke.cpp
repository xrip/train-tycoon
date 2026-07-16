#include "doctest/doctest.h"

#include "game.h"
#include "click_map.h"
#include "midp_input.h"
#include "stable_digest.h"
#include "support/input_queue.h"
#include "support/resource_reader.h"
#include "support/record_store.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>

namespace filesystem = std::filesystem;

namespace {

constexpr int kScreenWidth = 240;
constexpr int kScreenHeight = 320;
constexpr std::int64_t kFixedRandomSeed = 0x5eed1234;
std::uint32_t framebuffer[kScreenWidth * kScreenHeight];

Game* make_game() {
    auto* game = new Game{};
    game_fields_init(game);
    game->music_output = std::make_unique<SilentMusicOutput>();
    game->rng.set_seed(kFixedRandomSeed);
    return game;
}

std::uint64_t map_digest(const Game* game) {
    StableDigest digest;
    digest.add_i32(game->scenario.activeScenario);
    digest.add_i32(game->world.mapWidth);
    digest.add_i32(game->world.mapHeight);
    digest.add_i32(game->economy.money);
    digest.add_i32(game->economy.loanCeiling);
    digest.add_i32(game->economy.loanBalance);
    for (int x = 0; x < game->world.mapWidth; ++x) {
        digest.add_bytes(game->world.tileMap[x].data(), static_cast<std::size_t>(game->world.mapHeight));
        digest.add_bytes(game->world.trackMap[x].data(), static_cast<std::size_t>(game->world.mapHeight));
        digest.add_bytes(game->world.tileElevation[x].data(), static_cast<std::size_t>(game->world.mapHeight));
    }
    for (int x = 0; x < game->world.mapWidth + 2; ++x) {
        digest.add_bytes(game->world.heightMap[x].data(), static_cast<std::size_t>(game->world.mapHeight + 2));
    }
    return digest.value();
}

void generate_scenario(Game* game, int scenario) {
    game->scenario.activeScenario = scenario;
    game_setState(game, 25);
    for (int step = 0; game->gameState == 25 && step < 200000; ++step) {
        game_update(game);
    }
    REQUIRE(game->gameState == 30);
    REQUIRE(!game->world.tileMap.empty());
    REQUIRE(!game->world.trackMap.empty());
}

} // namespace

TEST_CASE("fast: Java random values") {
    JavaRandom random{};
    random.set_seed(0);
    CHECK(random.next_int() == -1155484576);
    CHECK(random.next_int() == -723955400);
    CHECK(random.next_int(1000) == 29);
    CHECK(random.next_int(16) == 9);

    random.set_seed(0);
    CHECK(random.next_int() == -1155484576);
}

TEST_CASE("fast: byte buffer owns low-byte output") {
    ByteBuffer buffer;
    CHECK(buffer.bytes().empty());

    buffer.write(0x123);
    buffer.write(-1);
    const auto bytes = buffer.bytes();
    REQUIRE(bytes.size() == 2);
    CHECK(bytes[0] == 0x23);
    CHECK(bytes[1] == 0xff);

    buffer.clear();
    CHECK(buffer.bytes().empty());
}

TEST_CASE("fast: input queue maps, orders, and debounces keys") {
    InputQueue queue;
    KeyEvent event;
    CHECK_FALSE(queue.poll(event));

    queue.on_virtual_key('6', true);
    queue.on_virtual_key('6', true);
    queue.on_virtual_key('6', false);

    REQUIRE(queue.poll(event));
    CHECK(event.key_code == MIDP_KEY_NUM0 + 6);
    CHECK(event.pressed);
    CHECK(midp_get_game_action(event.key_code) == MIDP_RIGHT_ACTION);
    REQUIRE(queue.poll(event));
    CHECK(event.key_code == MIDP_KEY_NUM0 + 6);
    CHECK_FALSE(event.pressed);
    CHECK_FALSE(queue.poll(event));

    queue.on_virtual_key(0x26, true); // VK_UP
    REQUIRE(queue.poll(event));
    CHECK(midp_get_game_action(event.key_code) == MIDP_UP);

    queue.clear();
    queue.on_virtual_key(0x26, true);
    REQUIRE(queue.poll(event));
    CHECK(event.pressed);
}

TEST_CASE("fast: full input queue keeps its first 63 events") {
    InputQueue queue;
    for (int index = 0; index < 70; ++index) {
        queue.on_virtual_key('1', index % 2 == 0);
    }

    KeyEvent event;
    int count = 0;
    while (queue.poll(event)) {
        CHECK(event.key_code == MIDP_KEY_NUM0 + 1);
        ++count;
    }
    CHECK(count == 63);
}

TEST_CASE("fast: plain resources have fixed digests") {
    struct ExpectedResource {
        const char* name;
        std::uint64_t digest;
    };
    constexpr std::array expected_resources{
        ExpectedResource{"sprite_images.bin", 7703144764765851560ULL},
        ExpectedResource{"tile_sprite_ids.bin", 11149815025433152475ULL},
        ExpectedResource{"sprite_image_map.bin", 13405639703315080894ULL},
        ExpectedResource{"packed_sprites.bin", 10914531412671063909ULL},
        ExpectedResource{"scenarios.bin", 17851964682521031332ULL},
        ExpectedResource{"music.mid", 6784549803277222961ULL},
        ExpectedResource{"icon.png", 5309835248505004148ULL},
    };

    for (const auto& resource : expected_resources) {
        const auto bytes = resources::read_all(resource.name);
        REQUIRE(bytes);
        StableDigest digest;
        digest.add_bytes(bytes->data(), bytes->size());
        INFO("resource: ", resource.name);
        CHECK(digest.value() == resource.digest);
    }
}

TEST_CASE("fast: resource reader reads plain bytes and reaches EOF") {
    const filesystem::path path = resources::path_for("scenarios.bin");
    CHECK(path.filename() == "scenarios.bin");
    CHECK(path.parent_path().filename() == "resources");
    CHECK(!resources::open("missing.bin"));

    ResourceReader resource = resources::open("scenarios.bin");
    REQUIRE(resource);
    CHECK(resource.read_byte() == 15);
    CHECK(filesystem::file_size(path) == 1 + 15 * 36);
    CHECK(resource.read_byte() == 0);
    CHECK(resource.read_byte() == 80);
    CHECK(resource.read_byte() == 30);

    std::uintmax_t byte_count = 4;
    while (resource.read_byte() >= 0) {
        ++byte_count;
    }
    CHECK(byte_count == filesystem::file_size(path));
    CHECK(resource.read_byte() == -1);
}

TEST_CASE("fast: images own pixels and expose only valid drawing targets") {
    CHECK(Image::create_blank(0, 2) == nullptr);

    std::unique_ptr<Image> blank = Image::create_blank(2, 2);
    REQUIRE(blank != nullptr);
    CHECK(blank->width() == 2);
    CHECK(blank->height() == 2);
    REQUIRE(blank->drawing_surface() != nullptr);
    for (int index = 0; index < 4; ++index) {
        CHECK(blank->pixels()[index] == 0xFFFFFFFFu);
    }

    Surface* surface = blank->drawing_surface();
    midp_set_color(surface, 0x123456);
    midp_fill_rect(surface, 0, 0, 1, 1);
    CHECK(blank->pixels()[0] == 0xFF123456u);
    CHECK(blank->pixels()[1] == 0xFFFFFFFFu);

    const auto source = Image::create_blank(1, 1);
    REQUIRE(source != nullptr);
    midp_draw_image(surface, source.get(), -10, -10, MIDP_LEFT | MIDP_TOP);
    CHECK(blank->pixels()[0] == 0xFF123456u);
    CHECK(blank->pixels()[1] == 0xFFFFFFFFu);

    const auto png = resources::read_all("icon.png");
    REQUIRE(png);
    std::unique_ptr<Image> icon = Image::decode_png(*png);
    REQUIRE(icon != nullptr);
    CHECK(icon->width() == 50);
    CHECK(icon->height() == 50);
    CHECK(icon->drawing_surface() == nullptr);
}

TEST_CASE("fast: record store keeps record bytes and ids") {
    constexpr std::array<std::uint8_t, 5> record{3, 1, 4, 1, 5};
    CHECK(RecordStore::open("MissingRulesStore", false) == nullptr);
    {
        std::unique_ptr<RecordStore> store = RecordStore::open("RulesStore", true);
        REQUIRE(store != nullptr);
        CHECK(store->record_count() == 0);
        CHECK(store->add_record(record) == 1);
    }
    CHECK(filesystem::is_regular_file(
        RecordStore::path_for("RulesStore")));
    constexpr std::array<std::uint8_t, 21> expected_file{
        1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 5, 0, 0, 0,
        3, 1, 4, 1, 5,
    };
    std::ifstream file(RecordStore::path_for("RulesStore"), std::ios::binary);
    std::vector<std::uint8_t> file_bytes(expected_file.size());
    REQUIRE(file.read(reinterpret_cast<char*>(file_bytes.data()),
                      static_cast<std::streamsize>(file_bytes.size())));
    CHECK(file_bytes == std::vector<std::uint8_t>(expected_file.begin(),
                                                   expected_file.end()));

    std::unique_ptr<RecordStore> store = RecordStore::open("RulesStore", false);
    REQUIRE(store != nullptr);
    CHECK(store->record_count() == 1);
    CHECK(store->first_record_id() == 1);
    CHECK(store->record_size(1) == static_cast<int>(record.size()));
    const auto restored = store->record(1);
    REQUIRE(restored);
    CHECK(*restored == std::vector<std::uint8_t>(record.begin(), record.end()));
    store->delete_record(1);
    store.reset();

    store = RecordStore::open("RulesStore", false);
    REQUIRE(store != nullptr);
    CHECK(store->record_count() == 0);
}

TEST_CASE("fast: small path search") {
    Game* game = make_game();
    game->world.mapWidth = 5;
    game->world.mapHeight = 5;
    game->world.tileMap.resize(game->world.mapWidth, game->world.mapHeight);
    game->world.trackMap.resize(game->world.mapWidth, game->world.mapHeight);
    game->world.tileElevation.resize(game->world.mapWidth, game->world.mapHeight);
    game->pathfinder.anchorX = 1;
    game->pathfinder.anchorY = 2;
    game->pathfinder.targetX = 3;
    game->pathfinder.targetY = 2;
    game->economy.accel1 = 10;

    CHECK(game_tryBuildPath(game));
    CHECK(game->build.trackTileCount > 0);
    CHECK(game->build.selectionCost > 0);
    delete game;
}

TEST_CASE("fast: save buffer bytes and round trip") {
    Game* game = make_game();
    game->economy.money = 123456;
    game->economy.dayOfMonth = 17;
    game->economy.monthOfYear = 8;
    game->economy.year = 1973;
    game->economy.loanBalance = -7654321;
    game->renderer.visibleTrains[0] = -1234;
    game_initSaveOrLoad(game, 4);

    const int plain_size = game->saveCodec.snapshotSize;
    REQUIRE(plain_size > 0);
    StableDigest save_digest;
    save_digest.add_bytes(game->saveCodec.saveBuffer.data(), static_cast<std::size_t>(plain_size));
    CHECK(plain_size == 19242);
    CHECK(save_digest.value() == 16273675477449773753ULL);
    auto* plain = static_cast<int8_t*>(std::malloc(static_cast<std::size_t>(plain_size)));
    REQUIRE(plain != nullptr);
    std::memcpy(plain, game->saveCodec.saveBuffer.data(), static_cast<std::size_t>(plain_size));

    game_compressSaveBuffer(game);
    game_decompressSaveBuffer(game);
    CHECK(game->saveCodec.snapshotSize == plain_size);
    CHECK(std::memcmp(game->saveCodec.saveBuffer.data(), plain, static_cast<std::size_t>(plain_size)) == 0);

    game->economy.money = 0;
    game->economy.dayOfMonth = 0;
    game->economy.monthOfYear = 0;
    game->economy.year = 0;
    game->economy.loanBalance = 0;
    game->renderer.visibleTrains[0] = 0;
    game_initSaveOrLoad(game, 5);
    CHECK(game->economy.money == 123456);
    CHECK(game->economy.dayOfMonth == 17);
    CHECK(game->economy.monthOfYear == 8);
    CHECK(game->economy.year == 1973);
    CHECK(game->economy.loanBalance == -7654321);
    CHECK(game->renderer.visibleTrains[0] == -1234);

    std::free(plain);
    delete game;
}

TEST_CASE("fast: MIDI resource is valid") {
    const auto midi = resources::read_all("music.mid");
    REQUIRE(midi);
    CHECK(is_valid_midi(*midi));
}

TEST_CASE("fast: game music rules use a silent output") {
    Game* game = make_game();
    auto* output = static_cast<SilentMusicOutput*>(game->music_output.get());

    game_loadMusic(game);
    CHECK(game->music_available);
    CHECK_FALSE(game->music_playing);
    CHECK(output->volume() == 40);

    game_playMusic(game);
    CHECK(output->play_count() == 0);
    game->music_enabled = true;
    game_playMusic(game);
    CHECK(game->music_playing);
    CHECK(output->play_count() == 1);
    game_playMusic(game);
    CHECK(output->play_count() == 1);

    game_setMusicVolume(game, 5);
    CHECK(output->volume() == 100);
    game_stopMusic(game);
    CHECK_FALSE(game->music_playing);
    CHECK(output->stop_count() == 1);

    delete game;
}

TEST_CASE("full: all scenario maps have fixed digests") {
    constexpr std::array<std::uint64_t, 13> expected{
        4307122233410706396ULL,
        7410989599457244056ULL,
        14660043004649095592ULL,
        1298331539406995531ULL,
        13704524898869083524ULL,
        2397133757916587733ULL,
        2733903835998681015ULL,
        2254306706094717035ULL,
        12476545952634235763ULL,
        5618243249801725703ULL,
        9569871328048813870ULL,
        5566282860187776653ULL,
        46185107338736850ULL,
    };
    for (int scenario = 0; scenario < static_cast<int>(expected.size()); ++scenario) {
        Game* game = make_game();
        generate_scenario(game, scenario);
        INFO(scenario);
        CHECK(map_digest(game) == expected[scenario]);
        delete game;
    }
}

TEST_CASE("full: scenario zero input and frames have fixed digests") {
    Game* game = make_game();
    game_start(game, nullptr);

    game->loadingStartTime = 0;
    game_update(game);
    game->loadingStartTime = 0;
    game_update(game);
    game_update(game);
    REQUIRE(game->gameState == 20);

    game_setState(game, 10);
    for (int step = 0; game->gameState == 10 && step < 1000; ++step) {
        game_update(game);
    }
    REQUIRE(game->gameState == 20);

    generate_scenario(game, 0);

    std::memset(framebuffer, 0, sizeof(framebuffer));
    Surface screen;
    midp_gfx_init_screen(&screen, framebuffer, kScreenWidth, kScreenHeight);
    game_paint(game, &screen);
    StableDigest first_frame;
    first_frame.add_pixels(framebuffer, std::size(framebuffer));
    CHECK(first_frame.value() == 13699798000239068498ULL);

    const int old_viewport_x = game->renderer.viewportCenterX;
    game_keyPressed(game, 54);
    game_run(game);
    game_paint(game, &screen);
    CHECK(game->gameState == 30);
    CHECK(game->renderer.backbuffer != nullptr);
    CHECK(game->renderer.viewportCenterX != old_viewport_x);
    StableDigest moved_frame;
    moved_frame.add_pixels(framebuffer, std::size(framebuffer));
    CHECK(moved_frame.value() == 2749731419861486662ULL);

    delete game;
}

TEST_CASE("fast: click map records and hit-tests overlay elements") {
    ClickMap map;
    CHECK(map.count == 0);
    CHECK_FALSE(map.hit_menu_row(50, 10, nullptr, nullptr));
    CHECK_FALSE(map.hit_close(50, 10));

    map.add_menu_row(0, 0, 200, 20, 0, 11);
    map.add_menu_row(0, 20, 200, 20, 1, 22);
    map.add_close(190, 2, 7, 7);   // top-right X, overlapping row 0's band
    CHECK(map.count == 3);

    int row = -1, code = -1;
    // A click on row 0's text area misses the X and hits the row.
    CHECK(map.hit_menu_row(10, 10, &row, &code));
    CHECK(row == 0);
    CHECK(code == 11);
    CHECK_FALSE(map.hit_close(10, 10));

    // The X wins on its own pixels even though row 0's band covers them.
    CHECK(map.hit_close(193, 5));
    CHECK(map.hit_menu_row(193, 5, &row, &code));   // row also spans it; caller checks X first
    CHECK(row == 0);

    CHECK(map.hit_menu_row(10, 30, &row, &code));
    CHECK(row == 1);
    CHECK(code == 22);

    map.clear();
    CHECK(map.count == 0);
    CHECK_FALSE(map.hit_close(193, 5));
}

TEST_CASE("full: a bottom-bar click selects and activates the hit item") {
    Game* game = make_game();
    generate_scenario(game, 0);
    REQUIRE(game->gameState == 30);
    REQUIRE(game->ui.buildMenuMode == 0);

    // generate_scenario never paints, so set the screen-size globals that the
    // first game_paint would normally establish (the live loop always has these).
    gcs_screenWidth = kScreenWidth;
    gcs_hs = kScreenHeight;
    gcs_bottomBarY = gcs_hs - 15;

    // Mode-0 index 1 is always the "build" entry (item id 1); activating it
    // switches buildMenuMode to 1 and resets the item index to 0 (openBuildMenu
    // mode-0 case 1). Click that icon's x on the bottom bar.
    const int item1_x = static_cast<int>(static_cast<std::uint8_t>(game->ui.menuItemCode[0][1]));
    REQUIRE(item1_x > 0);

    game_pointerPressed(game, item1_x, gcs_bottomBarY);

    CHECK(game->ui.buildMenuMode == 1);
    CHECK(game->ui.buildMenuItemIndex == 0);
    delete game;
}

TEST_CASE("full: pointer hover resolves the tile under the mouse") {
    Game* game = make_game();
    generate_scenario(game, 0);
    REQUIRE(game->gameState == 30);

    gcs_screenWidth = kScreenWidth;
    gcs_hs = kScreenHeight;
    gcs_bottomBarY = gcs_hs - 15;

    // Hover the middle of the gameplay viewport. The viewport always shows the
    // map after generation, so its centre sits on a valid tile.
    const int hover_x = kScreenWidth / 2;
    const int hover_y = (15 + gcs_bottomBarY) / 2;

    game_pointerMoved(game, hover_x, hover_y);
    REQUIRE(game->ui.mouseOverField);

    const int tx = game->ui.tileCursorX;
    const int ty = game->ui.tileCursorY;
    CHECK(tx >= 0);
    CHECK(tx < game->world.mapWidth);
    CHECK(ty >= 0);
    CHECK(ty < game->world.mapHeight);

    // Round-trip: the resolved tile's screen centre (the flat iso forward
    // transform the resolver mirrors) must sit within one tile of the hover.
    const int tile_screen_x =
        ty * 16 + tx * 16 + 16 - game->renderer.viewportCenterX;
    const int tile_screen_y =
        game->world.mapHalfWidth - tx * 8 + ty * 8 + 8 - game->renderer.viewportCenterY;
    const int dx = tile_screen_x - hover_x;
    const int dy = tile_screen_y - hover_y;
    CHECK(dx >= -16);
    CHECK(dx <= 16);
    CHECK(dy >= -8);
    CHECK(dy <= 8);

    // Hovering UI chrome (the bottom bar) must not claim a field tile.
    game_pointerMoved(game, hover_x, gcs_bottomBarY);
    CHECK_FALSE(game->ui.mouseOverField);
    delete game;
}

TEST_CASE("full: pointer edge-scroll pans the viewport toward the edge") {
    Game* game = make_game();
    generate_scenario(game, 0);
    REQUIRE(game->gameState == 30);

    // generate_scenario never paints, so set the screen-size globals that the
    // first game_paint would normally establish (the live loop always has these).
    gcs_screenWidth = kScreenWidth;
    gcs_hs = kScreenHeight;
    gcs_bottomBarY = gcs_hs - 15;

    const int mid_x = kScreenWidth / 2;
    const int mid_y = (15 + gcs_bottomBarY) / 2;
    const int cx0 = game->renderer.viewportCenterX;
    const int cy0 = game->renderer.viewportCenterY;
    const int msx0 = game->renderer.mapScaleX;
    const int msy0 = game->renderer.mapScaleY;

    // Middle of the viewport never arms a pan.
    game_pointerEdgeScroll(game, mid_x, mid_y);
    CHECK(game->renderer.viewportCenterX == cx0);
    CHECK(game->renderer.viewportCenterY == cy0);

    // Right edge pans right; mapScale (= 2*viewportBase) moves 2x the camera
    // delta so the run loop's camera-follow sees an unchanged cursor offset and
    // doesn't fight the pan. A cardinal edge only moves one axis.
    game_pointerEdgeScroll(game, kScreenWidth - 1, mid_y);
    const int right_dx = game->renderer.viewportCenterX - cx0;
    CHECK(right_dx > 0);
    CHECK(game->renderer.mapScaleX - msx0 == 2 * right_dx);
    CHECK(game->renderer.viewportCenterY == cy0);

    // Top-right corner pans along both axes (right + up).
    game_pointerEdgeScroll(game, kScreenWidth - 1, 15);
    const int dx = game->renderer.viewportCenterX - (cx0 + right_dx);
    const int dy = game->renderer.viewportCenterY - cy0;
    CHECK(dx > 0);
    CHECK(dy < 0);
    CHECK(game->renderer.mapScaleY - msy0 == 2 * dy);

    delete game;
}
