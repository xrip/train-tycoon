#include "doctest/doctest.h"

#include "game_methods.h"
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

GameCanvas* make_game() {
    auto* game = new GameCanvas{};
    gc_fields_init(game);
    game->music_output = std::make_unique<SilentMusicOutput>();
    game->rng.set_seed(kFixedRandomSeed);
    return game;
}

std::uint64_t map_digest(const GameCanvas* game) {
    StableDigest digest;
    digest.add_i32(game->activeScenario);
    digest.add_i32(game->mapWidth);
    digest.add_i32(game->mapHeight);
    digest.add_i32(game->money);
    digest.add_i32(game->loanCeiling);
    digest.add_i32(game->loanBalance);
    for (int x = 0; x < game->mapWidth; ++x) {
        digest.add_bytes(game->tileMap[x], static_cast<std::size_t>(game->mapHeight));
        digest.add_bytes(game->trackMap[x], static_cast<std::size_t>(game->mapHeight));
        digest.add_bytes(game->tileElevation[x], static_cast<std::size_t>(game->mapHeight));
    }
    for (int x = 0; x < game->mapWidth + 2; ++x) {
        digest.add_bytes(game->heightMap[x], static_cast<std::size_t>(game->mapHeight + 2));
    }
    return digest.value();
}

void generate_scenario(GameCanvas* game, int scenario) {
    game->activeScenario = scenario;
    gc_setState(game, 25);
    for (int step = 0; game->gameState == 25 && step < 200000; ++step) {
        gc_update(game);
    }
    REQUIRE(game->gameState == 30);
    REQUIRE(game->tileMap != nullptr);
    REQUIRE(game->trackMap != nullptr);
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
    GameCanvas* game = make_game();
    game->mapWidth = 5;
    game->mapHeight = 5;
    game->tileMap = GC_NEW(int8_t*, game->mapWidth);
    game->trackMap = GC_NEW(int8_t*, game->mapWidth);
    game->tileElevation = GC_NEW(int8_t*, game->mapWidth);
    for (int x = 0; x < game->mapWidth; ++x) {
        game->tileMap[x] = GC_NEW(int8_t, game->mapHeight);
        game->trackMap[x] = GC_NEW(int8_t, game->mapHeight);
        game->tileElevation[x] = GC_NEW(int8_t, game->mapHeight);
    }
    game->placementX = 1;
    game->placementY = 2;
    game->demolishX = 3;
    game->demolishY = 2;
    game->accel1 = 10;

    CHECK(gc_tryBuildPath(game));
    CHECK(game->trackTileCount > 0);
    CHECK(game->selectionCost > 0);
    delete game;
}

TEST_CASE("fast: save buffer bytes and round trip") {
    GameCanvas* game = make_game();
    game->money = 123456;
    game->dayOfMonth = 17;
    game->monthOfYear = 8;
    game->year = 1973;
    game->loanBalance = -7654321;
    game->visibleTrains[0] = -1234;
    gc_initSaveOrLoad(game, 4);

    const int plain_size = game->snapshotSize;
    REQUIRE(plain_size > 0);
    StableDigest save_digest;
    save_digest.add_bytes(game->saveBuffer, static_cast<std::size_t>(plain_size));
    CHECK(plain_size == 19242);
    CHECK(save_digest.value() == 16273675477449773753ULL);
    auto* plain = static_cast<int8_t*>(std::malloc(static_cast<std::size_t>(plain_size)));
    REQUIRE(plain != nullptr);
    std::memcpy(plain, game->saveBuffer, static_cast<std::size_t>(plain_size));

    gc_compressSaveBuffer(game);
    gc_decompressSaveBuffer(game);
    CHECK(game->snapshotSize == plain_size);
    CHECK(std::memcmp(game->saveBuffer, plain, static_cast<std::size_t>(plain_size)) == 0);

    game->money = 0;
    game->dayOfMonth = 0;
    game->monthOfYear = 0;
    game->year = 0;
    game->loanBalance = 0;
    game->visibleTrains[0] = 0;
    gc_initSaveOrLoad(game, 5);
    CHECK(game->money == 123456);
    CHECK(game->dayOfMonth == 17);
    CHECK(game->monthOfYear == 8);
    CHECK(game->year == 1973);
    CHECK(game->loanBalance == -7654321);
    CHECK(game->visibleTrains[0] == -1234);

    std::free(plain);
    delete game;
}

TEST_CASE("fast: MIDI resource is valid") {
    const auto midi = resources::read_all("music.mid");
    REQUIRE(midi);
    CHECK(is_valid_midi(*midi));
}

TEST_CASE("fast: game music rules use a silent output") {
    GameCanvas* game = make_game();
    auto* output = static_cast<SilentMusicOutput*>(game->music_output.get());

    gc_loadMusic(game);
    CHECK(game->music_available);
    CHECK_FALSE(game->music_playing);
    CHECK(output->volume() == 40);

    gc_playMusic(game);
    CHECK(output->play_count() == 0);
    game->music_enabled = true;
    gc_playMusic(game);
    CHECK(game->music_playing);
    CHECK(output->play_count() == 1);
    gc_playMusic(game);
    CHECK(output->play_count() == 1);

    gc_setMusicVolume(game, 5);
    CHECK(output->volume() == 100);
    gc_stopMusic(game);
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
        GameCanvas* game = make_game();
        generate_scenario(game, scenario);
        INFO(scenario);
        CHECK(map_digest(game) == expected[scenario]);
        delete game;
    }
}

TEST_CASE("full: scenario zero input and frames have fixed digests") {
    GameCanvas* game = make_game();
    gc_start(game, nullptr);

    game->loadingStartTime = 0;
    gc_update(game);
    game->loadingStartTime = 0;
    gc_update(game);
    gc_update(game);
    REQUIRE(game->gameState == 20);

    gc_setState(game, 10);
    for (int step = 0; game->gameState == 10 && step < 1000; ++step) {
        gc_update(game);
    }
    REQUIRE(game->gameState == 20);

    generate_scenario(game, 0);

    std::memset(framebuffer, 0, sizeof(framebuffer));
    Surface screen;
    midp_gfx_init_screen(&screen, framebuffer, kScreenWidth, kScreenHeight);
    gc_paint(game, &screen);
    StableDigest first_frame;
    first_frame.add_pixels(framebuffer, std::size(framebuffer));
    CHECK(first_frame.value() == 13699798000239068498ULL);

    const int old_viewport_x = game->viewportCenterX;
    gc_keyPressed(game, 54);
    gc_run(game);
    gc_paint(game, &screen);
    CHECK(game->gameState == 30);
    CHECK(game->backbuffer != nullptr);
    CHECK(game->viewportCenterX != old_viewport_x);
    StableDigest moved_frame;
    moved_frame.add_pixels(framebuffer, std::size(framebuffer));
    CHECK(moved_frame.value() == 2749731419861486662ULL);

    delete game;
}
