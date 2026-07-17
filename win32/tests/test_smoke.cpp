#include "doctest/doctest.h"

#include "game_internal.h"
#include "click_map.h"
#include "midp_input.h"
#include "scenario_definition.h"
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

std::uint64_t save_state_digest(Game* game) {
    game->saveCodec.walk(*game, 4);
    StableDigest digest;
    digest.add_bytes(game->saveCodec.saveBuffer.data(),
                     static_cast<std::size_t>(game->saveCodec.snapshotSize));
    return digest.value();
}

std::uint64_t paint_digest(Game* game, Surface* screen) {
    std::memset(framebuffer, 0, sizeof(framebuffer));
    game_paint(game, screen);
    StableDigest digest;
    digest.add_pixels(framebuffer, std::size(framebuffer));
    return digest.value();
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

void load_game_resources(Game* game) {
    game_start(game);
    game->loadingStartTime = 0;
    game_update(game);
    game->loadingStartTime = 0;
    game_update(game);
    game_update(game);
    REQUIRE(game->gameState == GameState::MenuOverlay);

    game_setState(game, GameState::LoadingResources);
    for (int step = 0; game->gameState == GameState::LoadingResources && step < 1000; ++step) {
        game_update(game);
    }
    REQUIRE(game->gameState == GameState::MenuOverlay);
}

} // namespace

TEST_CASE("fast: game initialization restores all non-zero defaults") {
    Game defaults{};
    CHECK(defaults.simSpeed == 1);
    CHECK(defaults.economy.money == 200000);
    CHECK(defaults.ui.cursorDragX == -1);
    CHECK(defaults.renderer.lastRenderedSpriteId == -1);

    Game* game = make_game();

    CHECK(game->simSpeed == 1);
    CHECK(game->economy.money == 200000);
    CHECK(game->economy.loanCeiling == 600000);
    CHECK(game->economy.dayOfMonth == 1);
    CHECK(game->economy.monthOfYear == 1);
    CHECK(game->economy.year == 1950);
    CHECK(game->scenario.difficultyPct == 100);
    CHECK(game->scenario.difficultyCycle == 1);
    CHECK(game->scenario.aiFrequency == 15);
    CHECK(game->scenario.aiThreshold == 12);
    CHECK(game->build.nextTrackPieceCode == 66);
    CHECK(game->dayTickThreshold == 50);
    CHECK(game->lastCanvasWidth == -1);
    CHECK(game->lastCanvasHeight == -1);
    CHECK(game->fleet.forwardTrackSteps[0][1] == 1);
    CHECK(game->fleet.reverseTrackSteps[5][11] == 1);
    CHECK(game->assets.trainSpriteDirectionTable[7][7] == 188);
    CHECK(game->scenario.offerTypesByEffect[0] == std::vector<int8_t>{1, 3, 13});
    CHECK(game->scenario.offerEffectSchedule[3][1] == -56);
    CHECK(game->graphColors[8] == 51200);
    CHECK(game->assets.tileSpriteYOffsetByCode[24] == -4);

    game->economy.money = 1;
    game->fleet.forwardTrackSteps[0][1] = 99;
    game->scenario.offerTypesByEffect[0].clear();
    game_fields_init(game);

    CHECK(game->economy.money == 200000);
    CHECK(game->fleet.forwardTrackSteps[0][1] == 1);
    CHECK(game->scenario.offerTypesByEffect[0] == std::vector<int8_t>{1, 3, 13});
    CHECK(game->renderer.backbuffer != nullptr);
    delete game;
}

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

TEST_CASE("fast: scenario definition reads one packed record") {
    ResourceReader resource = resources::open("scenarios.bin");
    REQUIRE(resource);
    CHECK(resource.read_byte() == 15);

    ScenarioDefinition definition;
    REQUIRE(readScenarioDefinition(resource, definition));
    CHECK(definition.scenarioIndex == 0);
    CHECK(definition.mapSize == 80);
    CHECK(definition.terrainRaiseSteps == 30);
    CHECK(definition.terrainLowerSteps == 15);
    CHECK(definition.obstacleCount == 1500);
    CHECK(definition.terrainPatchSize == 7);
    CHECK(definition.mapGenColumns == 4);
    CHECK(definition.mapGenRows == 4);
    CHECK(definition.startingMoney == 200000);
    CHECK(definition.loanCeiling == 600000);
    CHECK(definition.startingLoan == 0);
    CHECK(definition.interestRate == 8);
    CHECK(definition.eraNumber == 2);
    CHECK(definition.noticeId == 0);
    CHECK(definition.buildMenuScenario == 127);
    CHECK(definition.scenarioFlags == 7);
}

TEST_CASE("fast: proven train flag queries") {
    constexpr int8_t flags = static_cast<int8_t>(
        train_flag::USED_SLOT | train_flag::HEAD |
        train_flag::REMOVAL_PENDING | 7);
    static_assert(train_flag::isUsed(flags));
    static_assert(train_flag::isHead(flags));
    static_assert(train_flag::isRemovalPending(flags));
    static_assert(train_flag::type(flags) == 7);

    CHECK_FALSE(train_flag::isUsed(0));
    CHECK_FALSE(train_flag::isHead(0));
    CHECK_FALSE(train_flag::isRemovalPending(0));
    CHECK(train_flag::type(static_cast<int8_t>(0xF3)) == 3);
}

TEST_CASE("fast: train car records keep defaults, slots, and save fields") {
    Game* game = make_game();
    CHECK(game->fleet.trainCars[0].nextCar == 0);
    CHECK(game->fleet.trainCars[kTrainSlotCount - 1].flags == 0);

    TrainCar& first = game->fleet.trainCars[3];
    first.x = 11;
    first.direction = 2;
    first.secondaryCargoOrBaseEnergy = 17;
    first.nextCar = 321;
    first.revenue = 1234567;
    TrainCar& last = game->fleet.trainCars[kTrainSlotCount - 1];
    last.y = 22;
    last.flags = static_cast<int8_t>(train_flag::USED_SLOT | 5);
    last.ageYears = 9;
    last.stateCode = -7654321;
    last.previousRevenue = 456789;

    game->saveCodec.walk(*game, 4);
    first = TrainCar{};
    last = TrainCar{};
    game->saveCodec.walk(*game, 5);

    CHECK(first.x == 11);
    CHECK(first.direction == 2);
    CHECK(first.secondaryCargoOrBaseEnergy == 17);
    CHECK(first.nextCar == 321);
    CHECK(first.revenue == 1234567);
    CHECK(last.y == 22);
    CHECK(train_flag::type(last.flags) == 5);
    CHECK(last.ageYears == 9);
    CHECK(last.stateCode == -7654321);
    CHECK(last.previousRevenue == 456789);

    const int slot = game_allocateTrainSlot(game, 1, 2, 3, 4, 5, 6);
    CHECK(slot == 0);
    CHECK(game->fleet.trainCars[slot].x == 1);
    CHECK(game->fleet.trainCars[slot].y == 2);
    CHECK(game->fleet.trainCars[slot].nextCar == -1);

    for (TrainCar& car : game->fleet.trainCars)
        car.flags = static_cast<int8_t>(train_flag::USED_SLOT);
    CHECK(game_allocateTrainSlot(game, 1, 2, 3, 4, 5, 6) == kTrainSlotCount);
    delete game;
}

TEST_CASE("fast: service site records keep defaults, slots, and save fields") {
    Game* game = make_game();
    CHECK(game->fleet.serviceSites[0].flags == 0);
    CHECK(game->fleet.serviceSites[kServiceVehicleSlotCount - 1].resourceStock[4] == 0);

    ServiceSite& first = game->fleet.serviceSites[3];
    first.flags = static_cast<int8_t>(0x83);
    first.x = 14;
    first.unloadFlags = 9;
    first.resourceStock[0] = 1234;
    first.resourceStock[4] = -2345;
    first.loadFlags = 7;
    ServiceSite& last = game->fleet.serviceSites[kServiceVehicleSlotCount - 1];
    last.y = 27;
    last.resourceStock[2] = 30000;

    game->saveCodec.walk(*game, 4);
    first = ServiceSite{};
    last = ServiceSite{};
    game->saveCodec.walk(*game, 5);

    CHECK(static_cast<uint8_t>(first.flags) == 0x83);
    CHECK(first.x == 14);
    CHECK(first.unloadFlags == 9);
    CHECK(first.resourceStock[0] == 1234);
    CHECK(first.resourceStock[4] == -2345);
    CHECK(first.loadFlags == 0);
    CHECK(last.y == 27);
    CHECK(last.resourceStock[2] == 30000);

    const int slot = game_allocateServiceVehicleSlot(game, 2, 4, 5, true, false);
    CHECK(slot == 0);
    CHECK(game->fleet.serviceSites[slot].x == 4);
    CHECK(game->fleet.serviceSites[slot].y == 5);
    for (ServiceSite& site : game->fleet.serviceSites)
        site.flags = static_cast<int8_t>(0x80);
    CHECK(game_allocateServiceVehicleSlot(game, 2, 4, 5, true, false) == 5000);
    delete game;
}

TEST_CASE("fast: city records keep defaults and save field positions") {
    Game* game = make_game();
    CHECK(game->world.cities[0].stationDirection == 0);
    CHECK(game->world.cities[kCitySlotCount - 1].stationRevenue == 0);

    City& first = game->world.cities[4];
    first.stationDirection = 5;
    first.population = 71;
    first.zoneTileCount = 12;
    first.stationRevenue = 12345;
    first.previousStationRevenue = -23456;
    City& last = game->world.cities[kCitySlotCount - 1];
    last.stationDirection = 2;
    last.population = 44;
    last.zoneTileCount = 8;
    last.stationRevenue = -30000;
    last.previousStationRevenue = 30000;

    game->saveCodec.walk(*game, 4);
    first = City{};
    last = City{};
    game->saveCodec.walk(*game, 5);

    CHECK(first.stationDirection == 5);
    CHECK(first.population == 71);
    CHECK(first.zoneTileCount == 12);
    CHECK(first.stationRevenue == 12345);
    CHECK(first.previousStationRevenue == -23456);
    CHECK(last.stationDirection == 2);
    CHECK(last.population == 44);
    CHECK(last.zoneTileCount == 8);
    CHECK(last.stationRevenue == -30000);
    CHECK(last.previousStationRevenue == 30000);
    delete game;
}

TEST_CASE("fast: offer records keep defaults and save field positions") {
    Game* game = make_game();
    CHECK(game->offerBoard.offers[0].objectiveType == 0);
    CHECK(game->offerBoard.offers[kMenuRowCount - 1].effectAmount == 0);

    Offer& first = game->offerBoard.offers[1];
    first.objectiveType = 2;
    first.issuedDay = 3;
    first.dueDay = 4;
    first.issuedMonth = 5;
    first.dueMonth = 6;
    first.issuedYear = 7;
    first.dueYear = 8;
    first.target = 123456;
    first.progress = -234567;
    first.effectType = 9;
    first.effectValue = 10;
    first.effectAmount = 345678;
    Offer& last = game->offerBoard.offers[kMenuRowCount - 1];
    last.objectiveType = 14;
    last.target = -7654321;
    last.effectAmount = 7654321;

    game->saveCodec.walk(*game, 4);
    first = Offer{};
    last = Offer{};
    game->saveCodec.walk(*game, 5);

    CHECK(first.objectiveType == 2);
    CHECK(first.issuedDay == 3);
    CHECK(first.dueDay == 4);
    CHECK(first.issuedMonth == 5);
    CHECK(first.dueMonth == 6);
    CHECK(first.issuedYear == 7);
    CHECK(first.dueYear == 8);
    CHECK(first.target == 123456);
    CHECK(first.progress == -234567);
    CHECK(first.effectType == 9);
    CHECK(first.effectValue == 10);
    CHECK(first.effectAmount == 345678);
    CHECK(last.objectiveType == 14);
    CHECK(last.target == -7654321);
    CHECK(last.effectAmount == 7654321);
    delete game;
}

TEST_CASE("full: buying a train places it on the selected depot, not the drifted cursor") {
    Game* game = make_game();
    generate_scenario(game, 0);

    // Find a buildable, empty interior tile for a depot.
    int ax = -1, ay = -1;
    for (int y = 8; y < game->world.mapHeight - 8 && ay < 0; ++y) {
        for (int x = 8; x < game->world.mapWidth - 8; ++x) {
            if (game->world.tileMap[x][y] == 0 && game->world.trackMap[x][y] == 0
                && game->world.tileElevation[x][y] == 0
                && game_isTileBuildable(game, x, y, 0)) {
                ax = x; ay = y; break;
            }
        }
    }
    REQUIRE(ax >= 0);

    // Place a 1-cell depot at the chosen tile via the real build action.
    game->economy.money = 1000000;
    game->ui.tileCursorX = ax;
    game->ui.tileCursorY = ay;
    game_buildPlaceTrackPiece(game, 0);
    REQUIRE(static_cast<uint8_t>(game->world.trackMap[ax][ay]) == 65);  // depot on player track

    // Open the buy dialog on the depot (captures serviceVehicleAtCursor).
    game->ui.tileCursorX = ax;
    game->ui.tileCursorY = ay;
    game_pickTrainsAtCursor(game);
    REQUIRE(game->serviceVehicleAtCursor != 5000);

    // Choose an engine with two wood wagons, as the buy menu would.
    game->build.wagonType = 0;
    game->build.unitCost = 1;
    game->economy.money = 1000000;
    for (int i = 0; i < 5; ++i) game->build.wagonOptionSlots[i] = -1;
    game->build.wagonOptionSlots[0] = 3;  // wood
    game->build.wagonOptionSlots[1] = 3;  // wood

    // The Playing loop re-anchors tileCursor to the screen-center tile every
    // tick; by the time "buy" fires it has drifted off the depot. Simulate that
    // drift with an empty, non-track tile.
    int bx = ax + 3, by = ay;
    REQUIRE(static_cast<uint8_t>(game->world.trackMap[bx][by]) != 65);
    game->ui.tileCursorX = bx;
    game->ui.tileCursorY = by;

    REQUIRE(game_buyTrainAtCursor(game));

    // The freshly-bought engine must sit on the depot we selected, on player
    // track (65) so it can actually run -- not on the drifted screen-center tile.
    int head = game_findTrainHeadAt(game, ax, ay);
    CHECK(head != 5000);
    CHECK(game_findTrainHeadAt(game, bx, by) == 5000);
    if (head != 5000) {
        CHECK(static_cast<uint8_t>(game->world.trackMap[
            static_cast<uint8_t>(game->fleet.trainCars[head].x)][
            static_cast<uint8_t>(game->fleet.trainCars[head].y)]) == 65);
    }
    delete game;
}

TEST_CASE("fast: build tools that act on the cursor tile are classified") {
    // Place-at-cursor tools (must wait for a map click).
    CHECK(game_buildToolActsOnCursorTile(0, 0));   // select/inspect
    CHECK(game_buildToolActsOnCursorTile(1, 0));   // route anchor
    CHECK(game_buildToolActsOnCursorTile(1, 4));   // demolish
    CHECK(game_buildToolActsOnCursorTile(3, 0));   // place depot
    CHECK(game_buildToolActsOnCursorTile(4, 0));   // route target
    CHECK(game_buildToolActsOnCursorTile(5, 2));   // place track piece
    // Navigation / confirm items (fire on a bar click).
    CHECK_FALSE(game_buildToolActsOnCursorTile(0, 1));   // open build submenu
    CHECK_FALSE(game_buildToolActsOnCursorTile(1, 3));   // open station submenu
    CHECK_FALSE(game_buildToolActsOnCursorTile(3, 9));   // back
    CHECK_FALSE(game_buildToolActsOnCursorTile(2, 0));   // confirm auto-route
}

TEST_CASE("full: a bottom-bar depot click arms the tool; the map click places it") {
    Game* game = make_game();
    generate_scenario(game, 0);

    auto buildable = [&](int x, int y) {
        return game->world.tileMap[x][y] == 0 && game->world.trackMap[x][y] == 0
               && game->world.tileElevation[x][y] == 0
               && game_isTileBuildable(game, x, y, 0);
    };
    int ax = -1, ay = -1, bx = -1, by = -1;
    for (int y = 8; y < game->world.mapHeight - 8 && ay < 0; ++y)
        for (int x = 8; x < game->world.mapWidth - 8; ++x)
            if (buildable(x, y)) { ax = x; ay = y; break; }
    for (int y = ay + 4; y < game->world.mapHeight - 8 && by < 0; ++y)
        for (int x = 8; x < game->world.mapWidth - 8; ++x)
            if (buildable(x, y)) { bx = x; by = y; break; }
    REQUIRE(ax >= 0); REQUIRE(bx >= 0);

    canvasWidth = 240;
    bottomBarY = 300;
    game->economy.money = 1000000;
    game->ui.pauseFlag = false;
    game->ui.inputPending = false;
    game->ui.buildMenuMode = 3;          // station sub-menu
    game->ui.buildMenuItemIndex = 0;     // vertical depot

    // The cursor has drifted to the screen-center tile B.
    game->ui.tileCursorX = bx;
    game->ui.tileCursorY = by;

    // Click the depot icon in the bottom bar (compute its x the way the handler
    // does). This must ARM the tool, not place a depot at the drifted tile.
    const int mode = 3;
    const int selected_x = static_cast<uint8_t>(game->ui.menuItemCode[mode][0]);
    int scroll = selected_x + 15 - canvasWidth;
    if (scroll < 0) scroll = 0;
    const int icon_x = static_cast<uint8_t>(game->ui.menuItemCode[mode][0]) - scroll;
    game_pointerPressed(game, icon_x, bottomBarY);

    CHECK(static_cast<uint8_t>(game->world.trackMap[bx][by]) != 65);  // nothing placed at drift
    CHECK(game->ui.buildMenuItemIndex == 0);                          // tool stays armed

    // Now the player clicks the map on tile A: openBuildMenu runs with the tile
    // cursor on the clicked tile, and the armed depot tool places there.
    game->ui.tileCursorX = ax;
    game->ui.tileCursorY = ay;
    game_openBuildMenu(game);
    CHECK(static_cast<uint8_t>(game->world.trackMap[ax][ay]) == 65);  // depot placed where clicked
    delete game;
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
    game->saveCodec.walk(*game, 4);

    const int plain_size = game->saveCodec.snapshotSize;
    REQUIRE(plain_size > 0);
    StableDigest save_digest;
    save_digest.add_bytes(game->saveCodec.saveBuffer.data(), static_cast<std::size_t>(plain_size));
    CHECK(plain_size == 19242);
    CHECK(save_digest.value() == 16273675477449773753ULL);
    auto* plain = static_cast<int8_t*>(std::malloc(static_cast<std::size_t>(plain_size)));
    REQUIRE(plain != nullptr);
    std::memcpy(plain, game->saveCodec.saveBuffer.data(), static_cast<std::size_t>(plain_size));

    game->saveCodec.compress();
    game->saveCodec.decompress();
    CHECK(game->saveCodec.snapshotSize == plain_size);
    CHECK(std::memcmp(game->saveCodec.saveBuffer.data(), plain, static_cast<std::size_t>(plain_size)) == 0);

    game->economy.money = 0;
    game->economy.dayOfMonth = 0;
    game->economy.monthOfYear = 0;
    game->economy.year = 0;
    game->economy.loanBalance = 0;
    game->renderer.visibleTrains[0] = 0;
    game->saveCodec.walk(*game, 5);
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
    load_game_resources(game);

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

TEST_CASE("full: year rollover has a fixed saved-state digest") {
    Game* game = make_game();
    generate_scenario(game, 0);
    game->rng.set_seed(kFixedRandomSeed);
    monthlyReportEnabled = false;
    annualReportEnabled = false;
    game->economy.dayOfMonth = 31;
    game->economy.monthOfYear = 12;
    game->economy.year = 1950;
    game->dayFrameCounter = game->dayTickThreshold;

    game_simulationTick(game);

    CHECK(game->economy.dayOfMonth == 1);
    CHECK(game->economy.monthOfYear == 1);
    CHECK(game->economy.year == 1951);
    CHECK(game->dayFrameCounter == 0);
    CHECK(save_state_digest(game) == 8296142532964329289ULL);
    delete game;
}

TEST_CASE("full: report and graph dialogs have fixed frame digests") {
    Game* game = make_game();
    load_game_resources(game);
    generate_scenario(game, 0);
    Surface screen;
    midp_gfx_init_screen(&screen, framebuffer, kScreenWidth, kScreenHeight);

    // Set the canvas size while the game is still in Playing. An overlay entered
    // before the first paint is sent back to Playing by the resize path.
    paint_digest(game, &screen);

    game->ready = false;
    game->ui.transitionCode = 32;
    game_setState(game, GameState::MenuOverlay);
    game_paint(game, &screen);
    CHECK(paint_digest(game, &screen) == 3874743700717669836ULL);

    game_setYearGraphMode(game, 0, false);
    game->ui.transitionCode = 33;
    game_setState(game, GameState::MenuOverlay);
    game_paint(game, &screen);
    CHECK(paint_digest(game, &screen) == 1138514185145564824ULL);
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
    canvasWidth = kScreenWidth;
    canvasHeight = kScreenHeight;
    bottomBarY = canvasHeight - 15;

    // Mode-0 index 1 is always the "build" entry (item id 1); activating it
    // switches buildMenuMode to 1 and resets the item index to 0 (openBuildMenu
    // mode-0 case 1). Click that icon's x on the bottom bar.
    const int item1_x = static_cast<int>(static_cast<std::uint8_t>(game->ui.menuItemCode[0][1]));
    REQUIRE(item1_x > 0);

    game_pointerPressed(game, item1_x, bottomBarY);

    CHECK(game->ui.buildMenuMode == 1);
    CHECK(game->ui.buildMenuItemIndex == 0);
    delete game;
}

TEST_CASE("full: pointer hover resolves the tile under the mouse") {
    Game* game = make_game();
    generate_scenario(game, 0);
    REQUIRE(game->gameState == 30);

    canvasWidth = kScreenWidth;
    canvasHeight = kScreenHeight;
    bottomBarY = canvasHeight - 15;

    // Hover the middle of the gameplay viewport. The viewport always shows the
    // map after generation, so its centre sits on a valid tile.
    const int hover_x = kScreenWidth / 2;
    const int hover_y = (15 + bottomBarY) / 2;

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
    game_pointerMoved(game, hover_x, bottomBarY);
    CHECK_FALSE(game->ui.mouseOverField);
    delete game;
}

TEST_CASE("full: pointer edge-scroll pans the viewport toward the edge") {
    Game* game = make_game();
    generate_scenario(game, 0);
    REQUIRE(game->gameState == 30);

    // generate_scenario never paints, so set the screen-size globals that the
    // first game_paint would normally establish (the live loop always has these).
    canvasWidth = kScreenWidth;
    canvasHeight = kScreenHeight;
    bottomBarY = canvasHeight - 15;

    const int mid_x = kScreenWidth / 2;
    const int mid_y = (15 + bottomBarY) / 2;
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
