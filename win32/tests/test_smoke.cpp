#include "doctest/doctest.h"

#include "game.h"
#include "game_test_access.h"
#include "support/byte_buffer.h"
#include "support/numeric.h"
#include "click_map.h"
#include "midp_input.h"
#include "scenario_definition.h"
#include "stable_digest.h"
#include "support/input_queue.h"
#include "support/resource_reader.h"
#include "support/record_store.h"
#include "text/text_catalog.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>

namespace filesystem = std::filesystem;

namespace {

constexpr int kScreenWidth = MIDP_SCREEN_W;
constexpr int kScreenHeight = MIDP_SCREEN_H;
constexpr std::int64_t kFixedRandomSeed = 0x5eed1234;
std::uint32_t framebuffer[kScreenWidth * kScreenHeight];

Game* make_game() {
    return new Game(std::make_unique<SilentMusicOutput>(), kFixedRandomSeed);
}

std::uint64_t save_state_digest(Game* game) {
    GameTestAccess::save_codec(*game).walk(*game, save::OP_MEMORY_WRITE);
    StableDigest digest;
    digest.add_bytes(GameTestAccess::save_codec(*game).saveBuffer.data(),
                     static_cast<std::size_t>(GameTestAccess::save_codec(*game).snapshotSize));
    return digest.value();
}

std::uint64_t paint_digest(Game* game, Surface* screen) {
    std::memset(framebuffer, 0, sizeof(framebuffer));
    game->render(*screen);
    StableDigest digest;
    digest.add_pixels(framebuffer, std::size(framebuffer));
    return digest.value();
}

std::uint64_t map_digest(const Game* game) {
    StableDigest digest;
    digest.add_i32(GameTestAccess::scenario(*game).activeScenario);
    digest.add_i32(GameTestAccess::world(*game).terrain.mapWidth);
    digest.add_i32(GameTestAccess::world(*game).terrain.mapHeight);
    digest.add_i32(GameTestAccess::economy(*game).money);
    digest.add_i32(GameTestAccess::economy(*game).loanCeiling);
    digest.add_i32(GameTestAccess::economy(*game).loanBalance);
    for (int x = 0; x < GameTestAccess::world(*game).terrain.mapWidth; ++x) {
        digest.add_bytes(GameTestAccess::world(*game).terrain.tileMap[x].data(), static_cast<std::size_t>(GameTestAccess::world(*game).terrain.mapHeight));
        digest.add_bytes(GameTestAccess::world(*game).trackMap[x].data(), static_cast<std::size_t>(GameTestAccess::world(*game).terrain.mapHeight));
        digest.add_bytes(GameTestAccess::world(*game).terrain.tileElevation[x].data(), static_cast<std::size_t>(GameTestAccess::world(*game).terrain.mapHeight));
    }
    for (int x = 0; x < GameTestAccess::world(*game).terrain.mapWidth + 2; ++x) {
        digest.add_bytes(GameTestAccess::world(*game).terrain.heightMap[x].data(), static_cast<std::size_t>(GameTestAccess::world(*game).terrain.mapHeight + 2));
    }
    return digest.value();
}

void generate_scenario(Game* game, int scenario) {
    GameTestAccess::scenario(*game).activeScenario = scenario;
    GameTestAccess::set_state(*game, 25);
    for (int step = 0; GameTestAccess::state(*game) == 25 && step < 200000; ++step) {
        GameTestAccess::update_state_machine(*game);
    }
    REQUIRE(GameTestAccess::state(*game) == 30);
    REQUIRE(!GameTestAccess::world(*game).terrain.tileMap.empty());
    REQUIRE(!GameTestAccess::world(*game).trackMap.empty());
}

void load_game_resources(Game* game) {
    game->start();
    GameTestAccess::loading_start_time(*game) = 0;
    GameTestAccess::update_state_machine(*game);
    GameTestAccess::loading_start_time(*game) = 0;
    GameTestAccess::update_state_machine(*game);
    GameTestAccess::update_state_machine(*game);
    REQUIRE(GameTestAccess::state(*game) == GameState::MenuOverlay);

    GameTestAccess::set_state(*game, GameState::LoadingResources);
    for (int step = 0; GameTestAccess::state(*game) == GameState::LoadingResources && step < 1000; ++step) {
        GameTestAccess::update_state_machine(*game);
    }
    REQUIRE(GameTestAccess::state(*game) == GameState::MenuOverlay);
}

} // namespace

TEST_CASE("fast: game initialization restores all non-zero defaults") {
    Game defaults(std::make_unique<SilentMusicOutput>(), kFixedRandomSeed);
    CHECK(GameTestAccess::simulation_speed(defaults) == 1);
    CHECK(GameTestAccess::economy(defaults).money == 200000);
    CHECK(GameTestAccess::ui(defaults).cursorDragX == -1);
    CHECK(GameTestAccess::renderer(defaults).lastRenderedSpriteId == -1);

    Game* game = make_game();

    CHECK(GameTestAccess::simulation_speed(*game) == 1);
    CHECK(GameTestAccess::economy(*game).money == 200000);
    CHECK(GameTestAccess::economy(*game).loanCeiling == 600000);
    CHECK(GameTestAccess::economy(*game).dayOfMonth == 1);
    CHECK(GameTestAccess::economy(*game).monthOfYear == 1);
    CHECK(GameTestAccess::economy(*game).year == 1950);
    CHECK(GameTestAccess::scenario(*game).difficultyPct == 100);
    CHECK(GameTestAccess::scenario(*game).difficultyCycle == 1);
    CHECK(GameTestAccess::scenario(*game).aiFrequency == 15);
    CHECK(GameTestAccess::scenario(*game).aiThreshold == 12);
    CHECK(GameTestAccess::build(*game).nextTrackPieceCode == 66);
    CHECK(GameTestAccess::day_tick_threshold(*game) == 50);
    CHECK(GameTestAccess::last_canvas_width(*game) == -1);
    CHECK(GameTestAccess::last_canvas_height(*game) == -1);
    CHECK(GameTestAccess::fleet(*game).forwardTrackSteps[0].fallbackDirection == 1);
    CHECK(GameTestAccess::fleet(*game).reverseTrackSteps[5]
              .transitions[kTrackTransitionCount - 1]
              .nextDirection
          == 1);
    CHECK(GameTestAccess::assets(*game).trainSpriteDirectionTable[7][7] == 188);
    CHECK(GameTestAccess::scenario(*game).offerTypesByEffect[0] == std::vector<int8_t>{1, 3, 13});
    CHECK(GameTestAccess::scenario(*game).offerEffectSchedule[3][1] == -56);
    CHECK(GameTestAccess::renderer(*game).graphColors[8] == 51200);
    CHECK(GameTestAccess::assets(*game).tileSpriteYOffsetByCode[24] == -4);

    delete game;
    game = make_game();

    CHECK(GameTestAccess::economy(*game).money == 200000);
    CHECK(GameTestAccess::fleet(*game).forwardTrackSteps[0].fallbackDirection == 1);
    CHECK(GameTestAccess::scenario(*game).offerTypesByEffect[0] == std::vector<int8_t>{1, 3, 13});
    CHECK(GameTestAccess::renderer(*game).backbuffer != nullptr);
    delete game;
}

TEST_CASE("fast: Free Game setup cycles all new size and count levels") {
    Game* game = make_game();
    Scenario& scenario = GameTestAccess::scenario(*game);

    CHECK(scenario.freeGameMapSize == free_game_map_size::Small);
    GameTestAccess::handle_build_menu_state(
        *game, menu_screen::ScenarioSettings, 1);
    CHECK(scenario.freeGameMapSize == free_game_map_size::Medium);
    const auto medium_map_text =
        GameTestAccess::resolve_dialog_value(*game, 4, 1);
    REQUIRE(medium_map_text.has_value());
    CHECK(*medium_map_text == catalog_codes(Text::medium));
    GameTestAccess::handle_build_menu_state(
        *game, menu_screen::ScenarioSettings, 1);
    CHECK(scenario.freeGameMapSize == free_game_map_size::Large);
    GameTestAccess::handle_build_menu_state(
        *game, menu_screen::ScenarioSettings, 1);
    CHECK(scenario.freeGameMapSize == free_game_map_size::Small);

    GameTestAccess::handle_build_menu_state(
        *game, menu_screen::ScenarioSettings, 4);
    CHECK(scenario.freeGameTownLevel == free_game_town_level::Medium);
    GameTestAccess::handle_build_menu_state(
        *game, menu_screen::ScenarioSettings, 4);
    CHECK(scenario.freeGameTownLevel == free_game_town_level::Lots);
    GameTestAccess::handle_build_menu_state(
        *game, menu_screen::ScenarioSettings, 4);
    CHECK(scenario.freeGameTownLevel == free_game_town_level::Megapolis);
    const auto megapolis_text =
        GameTestAccess::resolve_dialog_value(*game, 4, 4);
    REQUIRE(megapolis_text.has_value());
    CHECK(*megapolis_text == catalog_codes(Text::megapolis));
    GameTestAccess::handle_build_menu_state(
        *game, menu_screen::ScenarioSettings, 4);
    CHECK(scenario.freeGameTownLevel == free_game_town_level::Few);

    GameTestAccess::handle_build_menu_state(
        *game, menu_screen::ScenarioSettings, 5);
    CHECK(scenario.freeGameIndustryLevel == free_game_industry_level::Medium);
    const auto medium_industry_text =
        GameTestAccess::resolve_dialog_value(*game, 4, 5);
    REQUIRE(medium_industry_text.has_value());
    CHECK(*medium_industry_text == catalog_codes(Text::medium));
    GameTestAccess::handle_build_menu_state(
        *game, menu_screen::ScenarioSettings, 5);
    CHECK(scenario.freeGameIndustryLevel == free_game_industry_level::Lots);
    GameTestAccess::handle_build_menu_state(
        *game, menu_screen::ScenarioSettings, 5);
    CHECK(scenario.freeGameIndustryLevel == free_game_industry_level::Few);

    scenario.freeGameMapSize = free_game_map_size::Medium;
    GameTestAccess::handle_build_menu_state(
        *game, menu_screen::ScenarioSettings, 10);
    CHECK(scenario.activeScenario == 1);
    CHECK(GameTestAccess::state(*game) == GameState::GeneratingMap);

    delete game;
}

TEST_CASE("full: Free Game medium settings use middle generation values") {
    Game* game = make_game();
    Scenario& scenario = GameTestAccess::scenario(*game);
    scenario.activeScenario = 1;
    scenario.freeGameMapSize = free_game_map_size::Medium;
    scenario.freeGameTownLevel = free_game_town_level::Medium;
    scenario.freeGameIndustryLevel = free_game_industry_level::Medium;

    GameTestAccess::begin_map_generation(*game);

    const World& world = GameTestAccess::world(*game);
    CHECK(world.terrain.mapWidth == 105);
    CHECK(world.terrain.mapHeight == 105);
    CHECK(world.terrainRaiseStepCount == 40);
    CHECK(world.terrainLowerStepCount == 20);
    CHECK(world.initialObstacleCount == 2750);
    CHECK(world.terrainPatchBaseSize == 8);
    CHECK(world.mapGenColumns == 3);
    CHECK(world.mapGenRows == 3);
    CHECK(world.featureStage0Count == 3);
    CHECK(world.featureStage1Count == 3);
    CHECK(world.featureStage2Count == 3);
    CHECK(world.featureStage3Count == 3);
    CHECK(world.featureStage4Count == 3);
    CHECK(world.featureStage5Count == 3);

    delete game;
}

TEST_CASE("full: Free Game megapolis makes four large towns safely") {
    Game* game = make_game();
    Scenario& scenario = GameTestAccess::scenario(*game);
    scenario.freeGameMapSize = free_game_map_size::Large;
    scenario.freeGameTownLevel = free_game_town_level::Megapolis;
    scenario.freeGameIndustryLevel = free_game_industry_level::Lots;

    generate_scenario(game, 1);

    const World& world = GameTestAccess::world(*game);
    const UiController& ui = GameTestAccess::ui(*game);
    CHECK(world.terrain.mapWidth == 130);
    CHECK(world.terrainPatchBaseSize == 39);
    CHECK(world.mapGenColumns == 2);
    CHECK(world.mapGenRows == 2);
    CHECK(ui.numSelectedTrains == 4);
    const int cell_width = world.terrain.mapWidth / 2;
    const int cell_height = world.terrain.mapHeight / 2;
    for (int town = 0; town < ui.numSelectedTrains; ++town) {
        CHECK(world.zonePopulation[town] > 0);
        int minimum_x = world.terrain.mapWidth;
        int minimum_y = world.terrain.mapHeight;
        int maximum_x = -1;
        int maximum_y = -1;
        for (int x = 0; x < world.terrain.mapWidth; ++x) {
            for (int y = 0; y < world.terrain.mapHeight; ++y) {
                if ((world.trackMap[x][y] & track_overlay::MASK)
                        != track_overlay::JUNCTION
                    || (world.trackMap[x][y] & track::OWNER_MASK) != town)
                    continue;
                if (x < minimum_x) minimum_x = x;
                if (y < minimum_y) minimum_y = y;
                if (x > maximum_x) maximum_x = x;
                if (y > maximum_y) maximum_y = y;
            }
        }
        REQUIRE(maximum_x >= minimum_x);
        REQUIRE(maximum_y >= minimum_y);
        CHECK(maximum_x - minimum_x + 1 >= 35);
        CHECK(maximum_y - minimum_y + 1 >= 35);
        const int town_column = town / 2;
        const int town_row = town % 2;
        CHECK(minimum_x >= town_column * cell_width);
        CHECK(minimum_y >= town_row * cell_height);
        CHECK(maximum_x < (town_column + 1) * cell_width);
        CHECK(maximum_y < (town_row + 1) * cell_height);
    }

    delete game;
}

TEST_CASE("full: Free Game megapolis size follows map size") {
    struct MegapolisMapCase {
        int8_t map_level;
        int32_t active_scenario;
        int32_t map_size;
        int32_t patch_size;
    };
    constexpr MegapolisMapCase cases[] = {
        {free_game_map_size::Small, 0, 80, 24},
        {free_game_map_size::Medium, 1, 105, 31},
        {free_game_map_size::Large, 1, 130, 39},
    };

    for (const MegapolisMapCase& test_case : cases) {
        Game* game = make_game();
        Scenario& scenario = GameTestAccess::scenario(*game);
        scenario.activeScenario = test_case.active_scenario;
        scenario.freeGameMapSize = test_case.map_level;
        scenario.freeGameTownLevel = free_game_town_level::Megapolis;
        scenario.freeGameIndustryLevel = free_game_industry_level::Lots;

        GameTestAccess::begin_map_generation(*game);

        const World& world = GameTestAccess::world(*game);
        CHECK(world.terrain.mapWidth == test_case.map_size);
        CHECK(world.terrainPatchBaseSize == test_case.patch_size);
        delete game;
    }
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
    CHECK(GameTestAccess::fleet(*game).trainCars[0].nextCar == 0);
    CHECK(GameTestAccess::fleet(*game).trainCars[kTrainSlotCount - 1].flags == 0);

    TrainCar& first = GameTestAccess::fleet(*game).trainCars[3];
    first.x = 11;
    first.direction = 2;
    first.secondaryCargoOrBaseEnergy = 17;
    first.nextCar = 321;
    first.revenue = 1234567;
    TrainCar& last = GameTestAccess::fleet(*game).trainCars[kTrainSlotCount - 1];
    last.y = 22;
    last.flags = static_cast<int8_t>(train_flag::USED_SLOT | 5);
    last.ageYears = 9;
    last.stateCode = -7654321;
    last.previousRevenue = 456789;

    GameTestAccess::save_codec(*game).walk(*game, save::OP_MEMORY_WRITE);
    first = TrainCar{};
    last = TrainCar{};
    GameTestAccess::save_codec(*game).walk(*game, save::OP_MEMORY_READ);

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

    const int slot = GameTestAccess::fleet(*game).allocate_train_car(1, 2, 3, 4, 5, 6);
    CHECK(slot == 0);
    CHECK(GameTestAccess::fleet(*game).trainCars[slot].x == 1);
    CHECK(GameTestAccess::fleet(*game).trainCars[slot].y == 2);
    CHECK(GameTestAccess::fleet(*game).trainCars[slot].nextCar == -1);

    for (TrainCar& car : GameTestAccess::fleet(*game).trainCars)
        car.flags = static_cast<int8_t>(train_flag::USED_SLOT);
    CHECK(GameTestAccess::fleet(*game).allocate_train_car(1, 2, 3, 4, 5, 6) == kTrainSlotCount);
    delete game;
}

TEST_CASE("fast: service site records keep defaults, slots, and save fields") {
    Game* game = make_game();
    CHECK(GameTestAccess::fleet(*game).serviceSites[0].flags == 0);
    CHECK(GameTestAccess::fleet(*game).serviceSites[kServiceVehicleSlotCount - 1].resourceStock[4] == 0);

    ServiceSite& first = GameTestAccess::fleet(*game).serviceSites[3];
    first.flags = static_cast<int8_t>(0x83);
    first.x = 14;
    first.unloadFlags = 9;
    first.resourceStock[0] = 1234;
    first.resourceStock[4] = -2345;
    first.loadFlags = 7;
    ServiceSite& last = GameTestAccess::fleet(*game).serviceSites[kServiceVehicleSlotCount - 1];
    last.y = 27;
    last.resourceStock[2] = 30000;

    GameTestAccess::save_codec(*game).walk(*game, save::OP_MEMORY_WRITE);
    first = ServiceSite{};
    last = ServiceSite{};
    GameTestAccess::save_codec(*game).walk(*game, save::OP_MEMORY_READ);

    CHECK(static_cast<uint8_t>(first.flags) == 0x83);
    CHECK(first.x == 14);
    CHECK(first.unloadFlags == 9);
    CHECK(first.resourceStock[0] == 1234);
    CHECK(first.resourceStock[4] == -2345);
    CHECK(first.loadFlags == 0);
    CHECK(last.y == 27);
    CHECK(last.resourceStock[2] == 30000);

    const int slot = GameTestAccess::fleet(*game).allocate_service_site(2, 4, 5, true, false);
    CHECK(slot == 0);
    CHECK(GameTestAccess::fleet(*game).serviceSites[slot].x == 4);
    CHECK(GameTestAccess::fleet(*game).serviceSites[slot].y == 5);
    for (ServiceSite& site : GameTestAccess::fleet(*game).serviceSites)
        site.flags = static_cast<int8_t>(0x80);
    CHECK(GameTestAccess::fleet(*game).allocate_service_site(2, 4, 5, true, false) == 5000);
    delete game;
}

TEST_CASE("fast: fleet owns service resource collection and load checks") {
    Fleet fleet;
    ServiceSite& first = fleet.serviceSites[3];
    first.flags = static_cast<int8_t>(0x82);
    first.x = 10;
    first.y = 10;
    first.resourceStock[0] = 7;
    first.loadFlags = 1;
    ServiceSite& near = fleet.serviceSites[4];
    near.flags = static_cast<int8_t>(0x82);
    near.x = 12;
    near.y = 11;
    near.resourceStock[0] = 9;
    ServiceSite& other_type = fleet.serviceSites[5];
    other_type.flags = static_cast<int8_t>(0x83);
    other_type.x = 11;
    other_type.y = 10;
    other_type.resourceStock[0] = 20;

    CHECK(fleet.collect_nearby_service_resource(3, 0, 12) == 12);
    CHECK(first.resourceStock[0] == 0);
    CHECK(near.resourceStock[0] == 4);
    CHECK(other_type.resourceStock[0] == 20);

    fleet.trainCars[0].nextCar = 1;
    fleet.trainCars[1].flags = static_cast<int8_t>(train_flag::USED_SLOT | 2);
    fleet.trainCars[1].nextCar = -1;
    fleet.trainCars[1].energyCargoOrRemovalTicks = 24;
    CHECK(fleet.can_load_at_service_site(0, 3));
    fleet.trainCars[1].energyCargoOrRemovalTicks = 25;
    CHECK_FALSE(fleet.can_load_at_service_site(0, 3));
}

TEST_CASE("fast: fleet loading and unloading keeps fixed cargo values") {
    Fleet fleet;
    World world;
    world.resize(8, 8);
    const int head = fleet.allocate_train_car(3, 3, 0, 0, 0, 8);
    const int wagon = fleet.append_car(head, 3);
    const int source = fleet.allocate_service_site(2, 3, 3, false, false);
    fleet.serviceSites[source].resourceStock[1] = 17;
    fleet.serviceSites[source].loadFlags = 4;

    fleet.load_at_service_site(world, head, source, false);
    CHECK(fleet.trainCars[wagon].energyCargoOrRemovalTicks == 17);
    CHECK(fleet.trainCars[wagon].cargoAge == 0);
    CHECK(fleet.trainCars[wagon].lastServiceSite == source);
    CHECK(fleet.serviceSites[source].resourceStock[1] == 0);

    const int destination = fleet.allocate_service_site(2, 5, 5, false, false);
    fleet.serviceSites[destination].unloadFlags = 2;
    fleet.trainCars[wagon].cargoAge = 30;
    Scenario scenario;
    scenario.routeResourceFlags[delivery_progress_slot::IndustryGroup1] = 1;
    const UnloadResult result = fleet.unload_at_service_site(
        scenario, head, destination);
    CHECK(result.revenue == 1020);
    CHECK(result.delivered[delivery_progress_slot::IndustryGroup1] == 17);
    CHECK(fleet.trainCars[wagon].energyCargoOrRemovalTicks == 0);
}

TEST_CASE("fast: passenger and mail unload fit the legacy result slots") {
    Fleet fleet;
    World world;
    world.resize(8, 8);
    const int head = fleet.allocate_train_car(3, 3, 0, 0, 0, 8);
    const int wagon = fleet.append_car(head, wagon::PassengerMailCar);
    const int source = fleet.allocate_service_site(2, 3, 3, false, false);
    fleet.serviceSites[source].resourceStock[service_resource::Passengers] = 7;
    fleet.serviceSites[source].resourceStock[service_resource::Mail] = 9;
    fleet.serviceSites[source].loadFlags = static_cast<int8_t>(
        service_resource::PASSENGER_FLAG | service_resource::MAIL_FLAG);
    fleet.load_at_service_site(world, head, source, false);

    const int destination = fleet.allocate_service_site(2, 5, 5, false, false);
    fleet.serviceSites[destination].unloadFlags = static_cast<int8_t>(
        service_resource::PASSENGER_FLAG | service_resource::MAIL_FLAG);
    Scenario scenario;
    scenario.routeResourceFlags[delivery_progress_slot::Passengers] = 1;
    scenario.routeResourceFlags[delivery_progress_slot::Mail] = 1;
    const UnloadResult result = fleet.unload_at_service_site(
        scenario, head, destination);
    CHECK(result.delivered[delivery_progress_slot::Passengers] == 7);
    CHECK(result.delivered[delivery_progress_slot::Mail] == 9);
    CHECK(fleet.trainCars[wagon].energyCargoOrRemovalTicks == 0);
    CHECK(fleet.trainCars[wagon].secondaryCargoOrBaseEnergy == 0);
}

TEST_CASE("fast: train movement follows the fixed straight-track step") {
    Game game(std::make_unique<SilentMusicOutput>(), kFixedRandomSeed);
    World& world = GameTestAccess::world(game);
    Fleet& fleet = GameTestAccess::fleet(game);
    world.resize(8, 8);
    world.terrain.tileMap[2][2] = 70;
    world.terrain.tileMap[2][3] = 70;
    world.trackMap[2][2] = 65;
    world.trackMap[2][3] = 65;
    const int head = fleet.allocate_train_car(2, 2, 0, 0, 0, 8);
    fleet.trainCars[head].phase = 8;
    fleet.trainCars[head].energyCargoOrRemovalTicks = 0;

    GameTestAccess::update_train(game, head);
    CHECK(fleet.trainCars[head].x == 2);
    CHECK(fleet.trainCars[head].y == 3);
    CHECK(fleet.trainCars[head].direction == 0);
    CHECK(fleet.trainCars[head].phase == 0);
    CHECK(fleet.trainCars[head].energyCargoOrRemovalTicks == 5);
    CHECK(fleet.trainCars[head].cargoAge == 1);
}

TEST_CASE("full: renderer train drawing does not change domain state") {
    Game game(std::make_unique<SilentMusicOutput>(), kFixedRandomSeed);
    generate_scenario(&game, 0);
    World& world = GameTestAccess::world(game);
    Fleet& fleet = GameTestAccess::fleet(game);
    Assets& assets = GameTestAccess::assets(game);
    Renderer& renderer = GameTestAccess::renderer(game);
    world.terrain.tileMap[2][2] = 70;
    world.trackMap[2][2] = 65;
    const int head = fleet.allocate_train_car(2, 2, 0, 0, 0, 8);
    fleet.trainCars[head].phase = -1;
    Surface screen;
    midp_gfx_init_screen(&screen, framebuffer, kScreenWidth, kScreenHeight);
    auto model_digest = [&world, &fleet]() {
        StableDigest digest;
        digest.add_i32(world.terrain.mapWidth);
        digest.add_i32(world.terrain.mapHeight);
        for (int x = 0; x < world.terrain.mapWidth; ++x) {
            digest.add_bytes(world.terrain.tileMap[x].data(),
                             static_cast<std::size_t>(world.terrain.mapHeight));
            digest.add_bytes(world.trackMap[x].data(),
                             static_cast<std::size_t>(world.terrain.mapHeight));
            digest.add_bytes(world.terrain.tileElevation[x].data(),
                             static_cast<std::size_t>(world.terrain.mapHeight));
        }
        for (const TrainCar& car : fleet.trainCars) {
            digest.add_i32(car.x);
            digest.add_i32(car.y);
            digest.add_i32(car.direction);
            digest.add_i32(car.phase);
            digest.add_i32(car.flags);
            digest.add_i32(car.nextCar);
            digest.add_i32(car.stateCode);
            digest.add_i32(car.revenue);
        }
        for (const ServiceSite& site : fleet.serviceSites) {
            digest.add_i32(site.flags);
            digest.add_i32(site.x);
            digest.add_i32(site.y);
            for (int stock : site.resourceStock) digest.add_i32(stock);
            digest.add_i32(site.unloadFlags);
            digest.add_i32(site.loadFlags);
        }
        return digest.value();
    };
    const std::uint64_t before = model_digest();

    renderer.draw_train(screen, assets, world, fleet, head);

    CHECK(model_digest() == before);
}

TEST_CASE("fast: fleet and world roll yearly revenue under their records") {
    Fleet fleet;
    fleet.trainCars[2].revenue = 123;
    fleet.trainCars[2].previousRevenue = 9;
    fleet.roll_revenue_year();
    CHECK(fleet.trainCars[2].revenue == 0);
    CHECK(fleet.trainCars[2].previousRevenue == 123);

    World world;
    world.cities[3].stationRevenue = 456;
    world.cities[3].previousStationRevenue = 8;
    world.roll_station_revenue_year();
    CHECK(world.cities[3].stationRevenue == 0);
    CHECK(world.cities[3].previousStationRevenue == 456);
}

TEST_CASE("fast: city records keep defaults and save field positions") {
    Game* game = make_game();
    CHECK(GameTestAccess::world(*game).cities[0].stationDirection == 0);
    CHECK(GameTestAccess::world(*game).cities[kCitySlotCount - 1].stationRevenue == 0);

    City& first = GameTestAccess::world(*game).cities[4];
    first.stationDirection = 5;
    first.population = 71;
    first.zoneTileCount = 12;
    first.stationRevenue = 12345;
    first.previousStationRevenue = -23456;
    City& last = GameTestAccess::world(*game).cities[kCitySlotCount - 1];
    last.stationDirection = 2;
    last.population = 44;
    last.zoneTileCount = 8;
    last.stationRevenue = -30000;
    last.previousStationRevenue = 30000;

    GameTestAccess::save_codec(*game).walk(*game, save::OP_MEMORY_WRITE);
    first = City{};
    last = City{};
    GameTestAccess::save_codec(*game).walk(*game, save::OP_MEMORY_READ);

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

TEST_CASE("fast: world owns zone timers, population, and city growth") {
    World world;
    world.resize(12, 12);
    world.constructionTimers[3] = 60;
    world.trackMap[5][5] = static_cast<int8_t>(0x80 | 3);
    world.trackMap[6][5] = static_cast<int8_t>(0x80 | 3);
    world.advance_construction_timers(5, 5, -4);
    CHECK(world.constructionTimers[3] == 56);
    world.decay_construction_timers_quarterly(3);
    CHECK(world.constructionTimers[3] == 56);
    world.decay_construction_timers_quarterly(4);
    CHECK(world.constructionTimers[3] == 55);

    world.terrain.tileMap[2][2] = tile::BUILT_FIRST;
    world.trackMap[2][2] = 3;
    world.terrain.tileMap[3][2] = tile::CITY_ZONE_LAST;
    world.trackMap[3][2] = 3;
    world.terrain.tileMap[4][2] = tile::CITY_ZONE_LAST + 1;
    world.trackMap[4][2] = 3;
    world.recount_zone_population();
    CHECK(world.zonePopulation[3] == 2);

    JavaRandom actual_random{};
    JavaRandom expected_random{};
    actual_random.set_seed(2468);
    expected_random.set_seed(2468);
    world.cities[0].population = 50;
    int expected_first = 50 + expected_random.next_int() % 8;
    for (int city = 1; city < kCitySlotCount; ++city)
        (void)expected_random.next_int();
    world.update_city_population(actual_random);
    CHECK(world.cities[0].population == expected_first);
    CHECK(actual_random.next_int() == expected_random.next_int());
}

TEST_CASE("fast: world recounts unique city zone tiles") {
    World world;
    world.resize(8, 8);
    world.trackMap[2][2] = static_cast<int8_t>(0xC0 | 3);
    world.trackMap[3][2] = static_cast<int8_t>(0xC0 | 3);
    world.terrain.tileMap[2][3] = 87;
    world.terrain.tileMap[3][3] = 88;
    world.recount_city_zone_tiles();
    CHECK(world.cities[3].zoneTileCount == 2);
}

TEST_CASE("fast: offer records keep defaults and save field positions") {
    Game* game = make_game();
    CHECK(GameTestAccess::offer_board(*game).offers[0].objectiveType == 0);
    CHECK(GameTestAccess::offer_board(*game).offers[kMenuRowCount - 1].effectAmount == 0);

    Offer& first = GameTestAccess::offer_board(*game).offers[1];
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
    Offer& last = GameTestAccess::offer_board(*game).offers[kMenuRowCount - 1];
    last.objectiveType = 14;
    last.target = -7654321;
    last.effectAmount = 7654321;

    GameTestAccess::save_codec(*game).walk(*game, save::OP_MEMORY_WRITE);
    first = Offer{};
    last = Offer{};
    GameTestAccess::save_codec(*game).walk(*game, save::OP_MEMORY_READ);

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

TEST_CASE("fast: type two offer progress handles zero money") {
    Game* game = make_game();
    Offer& offer = GameTestAccess::offer_board(*game).offers[0];
    offer.objectiveType = 2;
    offer.target = 1000;

    GameTestAccess::economy(*game).money = 500;
    CHECK(GameTestAccess::offer_board(*game).display_progress(0, GameTestAccess::economy(*game)) == 50);
    GameTestAccess::economy(*game).money = -500;
    CHECK(GameTestAccess::offer_board(*game).display_progress(0, GameTestAccess::economy(*game)) == 0);
    GameTestAccess::economy(*game).money = 0;
    CHECK(GameTestAccess::offer_board(*game).display_progress(0, GameTestAccess::economy(*game)) == 0);
    delete game;
}

TEST_CASE("fast: offer board schedules due dates with fixed random order") {
    OfferBoard board;
    Offer& offer = board.offers[2];
    offer.issuedDay = 27;
    offer.issuedMonth = 12;
    offer.issuedYear = 4;
    JavaRandom actual_random{};
    JavaRandom expected_random{};
    actual_random.set_seed(12345);
    expected_random.set_seed(12345);
    int due_day = offer.issuedDay + 2 * 60 + 20
                  + expected_random.next_int() % 20;
    int due_month = offer.issuedMonth;
    int due_year = offer.issuedYear;
    while (due_day > 28) {
        due_day -= 28;
        ++due_month;
    }
    while (due_month > 12) {
        due_month -= 12;
        ++due_year;
    }
    if (due_day == 1) ++due_day;

    board.schedule_due_date(2, 2, actual_random);
    CHECK(offer.dueDay == due_day);
    CHECK(offer.dueMonth == due_month);
    CHECK(offer.dueYear == due_year);
    CHECK(actual_random.next_int() == expected_random.next_int());
}

TEST_CASE("fast: offer board moves the summary slot and clears active data") {
    OfferBoard board;
    Offer& active = board.offers[2];
    active.objectiveType = 7;
    active.issuedDay = 8;
    active.dueDay = 9;
    active.issuedMonth = 10;
    active.dueMonth = 11;
    active.issuedYear = 12;
    active.dueYear = 13;
    active.target = 1400;
    active.progress = 1500;
    active.effectType = 6;
    active.effectValue = 17;
    active.effectAmount = 1800;

    board.move_to_summary(2);
    const Offer& summary = board.offers[kOfferSummaryRow];
    CHECK(summary.objectiveType == 7);
    CHECK(summary.issuedDay == 8);
    CHECK(summary.dueDay == 9);
    CHECK(summary.issuedMonth == 10);
    CHECK(summary.dueMonth == 11);
    CHECK(summary.issuedYear == 12);
    CHECK(summary.dueYear == 13);
    CHECK(summary.target == 1400);
    CHECK(summary.progress == 1500);
    CHECK(summary.effectType == 6);
    CHECK(summary.effectValue == 17);
    CHECK(summary.effectAmount == 1800);
    CHECK(active.objectiveType == 0);
    CHECK(active.target == 0);
    CHECK(active.progress == 0);
    CHECK(active.effectType == 0);
    CHECK(active.effectValue == 0);
    CHECK(active.effectAmount == 0);
    CHECK(active.issuedDay == 8);
    CHECK(active.dueDay == 9);
}

TEST_CASE("fast: offer board adds progress only to matching active rows") {
    OfferBoard board;
    board.offers[0].objectiveType = 13;
    board.offers[0].progress = 10;
    board.offers[1].objectiveType = 14;
    board.offers[1].progress = 20;
    board.offers[kOfferSummaryRow].objectiveType = 13;
    board.offers[kOfferSummaryRow].progress = 30;
    board.add_progress(13, 7);
    CHECK(board.offers[0].progress == 17);
    CHECK(board.offers[1].progress == 20);
    CHECK(board.offers[kOfferSummaryRow].progress == 30);
}

TEST_CASE("fast: economy spending and money history are owned rules") {
    Economy economy;
    economy.money = 100;
    economy.totalSpent = 10;
    CHECK(economy.spend(60));
    CHECK(economy.money == 40);
    CHECK(economy.totalSpent == 70);
    CHECK_FALSE(economy.spend(41));
    CHECK(economy.money == 40);
    CHECK(economy.totalSpent == 70);

    economy.push_money_history(5);
    economy.push_money_history(-7);
    economy.push_money_history(9);
    CHECK(economy.moneyHistory[0] == 9);
    CHECK(economy.moneyHistory[1] == -7);
    CHECK(economy.moneyHistory[2] == 5);
}

TEST_CASE("fast: UI controller owns key repeat timing") {
    UiController ui;
    ui.keyRepeatStep = 3;
    CHECK(ui.advance_key_repeat() == 1);
    CHECK(ui.keySubState == 3);
    ui.keySubState = 30;
    CHECK(ui.advance_key_repeat() == 10);
    CHECK(ui.keySubState == 30);
}

TEST_CASE("fast: UI controller maps report graph values") {
    UiController ui;
    ui.graphBarHeights[2] = 7;
    ui.graphSeries = 0;
    CHECK(ui.report_graph_value(34, 2) == 72);
    CHECK(ui.report_graph_value(34, 22) == 71);
    CHECK(ui.report_graph_value(34, 65) == 65);
    ui.graphSeries = 1;
    CHECK(ui.report_graph_value(34, 2) == 42);
    CHECK(ui.report_graph_value(34, 22) == 41);
    CHECK(ui.report_graph_value(34, 65) == 35);
    CHECK(ui.report_graph_value(33, 2) == 0);
}

TEST_CASE("fast: economy computes period and year-end report values") {
    Economy economy;
    economy.loanBalance = 120000;
    economy.interestRate = 5;
    economy.totalRevenue = 100000;
    economy.enterpriseTaxPct = 3;
    economy.totalSpent = 20000;
    economy.aiMoneySpent = 4000;
    economy.fuelCostAccrued = 5000;
    economy.subsidyCost = 6000;
    economy.decadeProfitGraphTotal = 7000;
    economy.revenueTotal = 8000;
    economy.decadeCostTotal = 9000;
    economy.decadeSpentTotal = 10000;
    economy.decadeFuelCost = 11000;
    economy.decadeSubsidyTotal = 12000;
    economy.decadeRevenueTotal = 13000;
    economy.decadeProfitTotal = 14000;
    economy.decadeEnterpriseTotal = 15000;

    economy.compute_year_end_metrics();
    CHECK(economy.terrainTotal == 500);
    CHECK(economy.decadeEnterpriseTax == 3000);
    CHECK(economy.netProfit == 61500);
    CHECK(economy.yearProfitHistory[0] == 61500);
    CHECK(economy.yearRevenueHistory[0] == 100000);
    CHECK(economy.decadeProfitHistory[0] == 68500);
    CHECK(economy.altDecadeRevenueTotal[0] == 108000);
    CHECK(economy.decadeSpentWithCurrent == 29000);
    CHECK(economy.decadeAiSpentWithCurrent == 14000);
    CHECK(economy.decadeFuelWithCurrent == 16000);
    CHECK(economy.decadeSubsidyWithCurrent == 18000);
    CHECK(economy.decadeRevenueWithCurrent == 113000);
    CHECK(economy.decadeProfitWithCurrent == 75500);
    CHECK(economy.decadeEnterpriseWithCurrent == 18000);
}

TEST_CASE("fast: scenario owns milestone completion rules") {
    Economy economy;
    Scenario scenario;
    scenario.scenarioMilestoneIndex = 6;
    scenario.currentMilestoneScore = 40;
    scenario.milestoneCoThreshold2 = 60;
    scenario.milestoneThreshold1 = 39;
    scenario.milestoneThreshold2 = 60;
    CHECK_FALSE(scenario.complete_milestone_if_met(economy));
    CHECK_FALSE(scenario.milestonesEnabled);
    scenario.milestoneThreshold1 = 40;
    CHECK(scenario.complete_milestone_if_met(economy));
    CHECK(scenario.milestonesEnabled);

    Scenario loan_scenario;
    loan_scenario.scenarioMilestoneIndex = 13;
    economy.loanBalance = 1;
    CHECK_FALSE(loan_scenario.complete_milestone_if_met(economy));
    economy.loanBalance = 0;
    CHECK(loan_scenario.complete_milestone_if_met(economy));
}

TEST_CASE("fast: fleet lookup and chain count use stable slots") {
    Fleet fleet;
    const int head = fleet.allocate_train_car(2, 3, 0, 1, 0, 7);
    const int car = fleet.append_car(head, 3);
    CHECK(car == 1);
    fleet.trainCars[car].x = 4;
    CHECK(fleet.count_cars(head) == 2);
    CHECK(fleet.has_car_at(head, 4, 3));
    CHECK(fleet.find_train_head_at(4, 3) == head);
    CHECK(fleet.find_train_head_at(1, 1) == 5000);
    fleet.reset_trailing_cars(head, car, 0);
    CHECK(fleet.trainCars[car].x == fleet.trainCars[head].x);
    CHECK(fleet.trainCars[car].y == fleet.trainCars[head].y);
    CHECK(fleet.trainCars[car].phase == -8);
    fleet.age_one_year();
    CHECK(fleet.trainCars[head].ageYears == 1);
    CHECK(fleet.trainCars[kTrainSlotCount - 1].ageYears == 1);

    const int site = fleet.allocate_service_site(1, 6, 7, true, true);
    CHECK(fleet.find_service_site_at(6, 7) == site);
    CHECK(fleet.find_service_site_at(5, 7) == site);
    CHECK(fleet.find_service_site_at(6, 6) == 5000);
}

TEST_CASE("full: buying a train places it on the selected depot, not the drifted cursor") {
    Game* game = make_game();
    generate_scenario(game, 0);

    // Find a buildable, empty interior tile for a depot.
    int ax = -1, ay = -1;
    for (int y = 8; y < GameTestAccess::world(*game).terrain.mapHeight - 8 && ay < 0; ++y) {
        for (int x = 8; x < GameTestAccess::world(*game).terrain.mapWidth - 8; ++x) {
            if (GameTestAccess::world(*game).terrain.tileMap[x][y] == 0 && GameTestAccess::world(*game).trackMap[x][y] == 0
                && GameTestAccess::world(*game).terrain.tileElevation[x][y] == 0
                && GameTestAccess::world(*game).is_tile_buildable(x, y, 0)) {
                ax = x; ay = y; break;
            }
        }
    }
    REQUIRE(ax >= 0);

    // Place a 1-cell depot at the chosen tile via the real build action.
    GameTestAccess::economy(*game).money = 1000000;
    GameTestAccess::ui(*game).tileCursorX = ax;
    GameTestAccess::ui(*game).tileCursorY = ay;
    GameTestAccess::build_place_track_piece(*game, 0);
    REQUIRE(static_cast<uint8_t>(GameTestAccess::world(*game).trackMap[ax][ay]) == 65);  // depot on player track

    // Open the buy dialog on the depot (captures serviceVehicleAtCursor).
    GameTestAccess::ui(*game).tileCursorX = ax;
    GameTestAccess::ui(*game).tileCursorY = ay;
    GameTestAccess::pick_trains_at_cursor(*game);
    REQUIRE(GameTestAccess::service_site_at_cursor(*game) != 5000);

    // Choose an engine with two wood wagons, as the buy menu would.
    GameTestAccess::build(*game).wagonType = 0;
    GameTestAccess::build(*game).unitCost = 1;
    GameTestAccess::economy(*game).money = 1000000;
    for (int i = 0; i < 5; ++i) GameTestAccess::build(*game).wagonOptionSlots[i] = -1;
    GameTestAccess::build(*game).wagonOptionSlots[0] = 3;  // wood
    GameTestAccess::build(*game).wagonOptionSlots[1] = 3;  // wood

    // The Playing loop re-anchors tileCursor to the screen-center tile every
    // tick; by the time "buy" fires it has drifted off the depot. Simulate that
    // drift with an empty, non-track tile.
    int bx = ax + 3, by = ay;
    REQUIRE(static_cast<uint8_t>(GameTestAccess::world(*game).trackMap[bx][by]) != 65);
    GameTestAccess::ui(*game).tileCursorX = bx;
    GameTestAccess::ui(*game).tileCursorY = by;

    REQUIRE(GameTestAccess::buy_train_at_cursor(*game));

    // The freshly-bought engine must sit on the depot we selected, on player
    // track (65) so it can actually run -- not on the drifted screen-center tile.
    int head = GameTestAccess::fleet(*game).find_train_head_at(ax, ay);
    CHECK(head != 5000);
    CHECK(GameTestAccess::fleet(*game).find_train_head_at(bx, by) == 5000);
    if (head != 5000) {
        CHECK(static_cast<uint8_t>(GameTestAccess::world(*game).trackMap[
            static_cast<uint8_t>(GameTestAccess::fleet(*game).trainCars[head].x)][
            static_cast<uint8_t>(GameTestAccess::fleet(*game).trainCars[head].y)]) == 65);
    }
    delete game;
}

TEST_CASE("fast: build tools that act on the cursor tile are classified") {
    // Place-at-cursor tools (must wait for a map click).
    CHECK(BuildController::tool_acts_on_cursor_tile(0, 0));   // select/inspect
    CHECK(BuildController::tool_acts_on_cursor_tile(1, 0));   // route anchor
    CHECK(BuildController::tool_acts_on_cursor_tile(1, 4));   // demolish
    CHECK(BuildController::tool_acts_on_cursor_tile(3, 0));   // place depot
    CHECK(BuildController::tool_acts_on_cursor_tile(4, 0));   // route target
    CHECK(BuildController::tool_acts_on_cursor_tile(5, 2));   // place track piece
    // Navigation / confirm items (fire on a bar click).
    CHECK_FALSE(BuildController::tool_acts_on_cursor_tile(0, 1));   // open build submenu
    CHECK_FALSE(BuildController::tool_acts_on_cursor_tile(1, 3));   // open station submenu
    CHECK_FALSE(BuildController::tool_acts_on_cursor_tile(3, 9));   // back
    CHECK_FALSE(BuildController::tool_acts_on_cursor_tile(2, 0));   // confirm auto-route
}

TEST_CASE("fast: build controller owns train cost calculation") {
    BuildController build;
    Economy economy;
    Scenario scenario;
    economy.trainCostSecondEra = 100;
    economy.wagonOptionCostIndustryGroup2 = 30;
    scenario.difficultyPct = 50;
    build.wagonType = 1;
    build.wagonDiscountType = 1;
    for (int option = 0; option < kWagonOptionCount; ++option) {
        build.wagonOptionSlots[option] = -1;
        build.matchingWagonOptions[option] = -2;
    }
    build.wagonOptionSlots[0] = 2;
    build.matchingWagonOptions[0] = 2;
    build.compute_train_cost(economy, scenario);
    CHECK(build.unitCost == 65);
}

TEST_CASE("full: a bottom-bar depot click arms the tool; the map click places it") {
    Game* game = make_game();
    generate_scenario(game, 0);

    auto buildable = [&](int x, int y) {
        return GameTestAccess::world(*game).terrain.tileMap[x][y] == 0 && GameTestAccess::world(*game).trackMap[x][y] == 0
               && GameTestAccess::world(*game).terrain.tileElevation[x][y] == 0
               && GameTestAccess::world(*game).is_tile_buildable(x, y, 0);
    };
    int ax = -1, ay = -1, bx = -1, by = -1;
    for (int y = 8; y < GameTestAccess::world(*game).terrain.mapHeight - 8 && ay < 0; ++y)
        for (int x = 8; x < GameTestAccess::world(*game).terrain.mapWidth - 8; ++x)
            if (buildable(x, y)) { ax = x; ay = y; break; }
    for (int y = ay + 4; y < GameTestAccess::world(*game).terrain.mapHeight - 8 && by < 0; ++y)
        for (int x = 8; x < GameTestAccess::world(*game).terrain.mapWidth - 8; ++x)
            if (buildable(x, y)) { bx = x; by = y; break; }
    REQUIRE(ax >= 0); REQUIRE(bx >= 0);

    GameTestAccess::renderer(*game).canvasWidth = 240;
    GameTestAccess::renderer(*game).bottomBarY = 300;
    GameTestAccess::economy(*game).money = 1000000;
    GameTestAccess::ui(*game).pauseFlag = false;
    GameTestAccess::ui(*game).inputPending = false;
    GameTestAccess::ui(*game).buildMenuMode = 3;          // station sub-menu
    GameTestAccess::ui(*game).buildMenuItemIndex = 0;     // vertical depot

    // The cursor has drifted to the screen-center tile B.
    GameTestAccess::ui(*game).tileCursorX = bx;
    GameTestAccess::ui(*game).tileCursorY = by;

    // Click the depot icon in the bottom bar (compute its x the way the handler
    // does). This must ARM the tool, not place a depot at the drifted tile.
    const int mode = 3;
    const int selected_x = static_cast<uint8_t>(GameTestAccess::ui(*game).menuItemCode[mode][0]);
    int scroll = selected_x + 15 - GameTestAccess::renderer(*game).canvasWidth;
    if (scroll < 0) scroll = 0;
    const int icon_x = static_cast<uint8_t>(GameTestAccess::ui(*game).menuItemCode[mode][0]) - scroll;
    game->pointer_pressed(icon_x, GameTestAccess::renderer(*game).bottomBarY);

    CHECK(static_cast<uint8_t>(GameTestAccess::world(*game).trackMap[bx][by]) != 65);  // nothing placed at drift
    CHECK(GameTestAccess::ui(*game).buildMenuItemIndex == 0);                          // tool stays armed

    // Now the player clicks the map on tile A: openBuildMenu runs with the tile
    // cursor on the clicked tile, and the armed depot tool places there.
    GameTestAccess::ui(*game).tileCursorX = ax;
    GameTestAccess::ui(*game).tileCursorY = ay;
    GameTestAccess::open_build_menu(*game);
    CHECK(static_cast<uint8_t>(GameTestAccess::world(*game).trackMap[ax][ay]) == 65);  // depot placed where clicked
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

TEST_CASE("fast: record store rejects a corrupt size header") {
    // The header claims one record of 1 GiB with no payload behind it. load()
    // must bound the size by the real file size and skip the record, not try
    // the allocation.
    constexpr std::array<std::uint8_t, 16> corrupt_file{
        1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0x00, 0x00, 0x00, 0x40,
    };
    {
        std::ofstream file(RecordStore::path_for("CorruptStore"),
                           std::ios::binary | std::ios::trunc);
        file.write(reinterpret_cast<const char*>(corrupt_file.data()),
                   static_cast<std::streamsize>(corrupt_file.size()));
    }
    std::unique_ptr<RecordStore> store = RecordStore::open("CorruptStore", false);
    REQUIRE(store != nullptr);
    CHECK(store->record_count() == 0);
}

TEST_CASE("fast: small path search") {
    Game* game = make_game();
    GameTestAccess::world(*game).resize(5, 5);
    GameTestAccess::pathfinder(*game).anchorX = 1;
    GameTestAccess::pathfinder(*game).anchorY = 2;
    GameTestAccess::pathfinder(*game).targetX = 3;
    GameTestAccess::pathfinder(*game).targetY = 2;
    GameTestAccess::economy(*game).accelerationEarly = 10;

    CHECK(GameTestAccess::pathfinder(*game).try_build_path(GameTestAccess::world(*game), GameTestAccess::build(*game),
                                          GameTestAccess::economy(*game)));
    CHECK(GameTestAccess::build(*game).trackTileCount > 0);
    CHECK(GameTestAccess::build(*game).selectionCost > 0);
    delete game;
}

TEST_CASE("fast: world resize keeps terrain and track grids in step") {
    World world;
    world.resize(7, 9);
    CHECK(world.terrain.mapWidth == 7);
    CHECK(world.terrain.mapHeight == 9);
    CHECK(world.terrain.tileMap.rows() == 8);
    CHECK(world.terrain.tileMap[0].size() == 10);
    CHECK(world.trackMap.rows() == 8);
    CHECK(world.trackMap[0].size() == 10);
    CHECK(world.terrain.tileElevation.rows() == 8);
    CHECK(world.terrain.tileElevation[0].size() == 10);
    CHECK(world.terrain.heightMap.rows() == 9);
    CHECK(world.terrain.heightMap[0].size() == 11);
}

TEST_CASE("fast: world place check reads terrain and track") {
    World world;
    world.resize(4, 4);
    CHECK(world.is_placeable_terrain(1, 1));
    world.terrain.tileElevation[1][1] = -1;
    CHECK_FALSE(world.is_placeable_terrain(1, 1));
    world.terrain.tileElevation[1][1] = 0;
    world.terrain.tileMap[1][1] = tile::SCENERY_FIRST;
    CHECK(world.is_placeable_terrain(1, 1));
    world.trackMap[1][1] = 1;
    CHECK_FALSE(world.is_placeable_terrain(1, 1));
}

TEST_CASE("fast: world track tile placement changes both grids") {
    World world;
    world.resize(4, 4);
    world.place_track_tile(1, 2, 53, 3);
    CHECK(world.terrain.tileMap[1][2] == 53);
    CHECK(static_cast<uint8_t>(world.trackMap[1][2]) == 0xC3);
}

TEST_CASE("fast: world buildable query checks bounds and full footprint") {
    World world;
    world.resize(5, 5);
    CHECK(world.is_tile_buildable(2, 2, 0));
    CHECK(world.is_tile_buildable(2, 2, 5));
    CHECK_FALSE(world.is_tile_buildable(-1, 2, 0));
    world.terrain.tileMap[2][3] = tile::BUILT_FIRST;
    CHECK_FALSE(world.is_tile_buildable(2, 2, 5));
}

TEST_CASE("fast: world track-step query stops at the first occupied tile") {
    World world;
    world.resize(5, 5);
    world.terrain.tileMap[3][2] = 9;
    CHECK(world.has_track_step(1, 2, 1, 0, 9));
    world.terrain.tileMap[2][2] = 4;
    CHECK_FALSE(world.has_track_step(1, 2, 1, 0, 9));

    world.terrain.tileMap[1][2] = 9;
    world.terrain.tileMap[2][2] = 0;
    world.terrain.tileMap[3][2] = 8;
    CHECK(world.is_curve_tile(1, 2));
}

TEST_CASE("fast: world station tile query uses fleet occupancy") {
    World world;
    Fleet fleet;
    world.resize(6, 6);
    world.terrain.tileMap[2][2] = 70;
    world.trackMap[2][2] = 65;
    CHECK(world.is_city_or_station_tile(fleet, 2, 2));
    fleet.trainCars[0].flags = static_cast<int8_t>(train_flag::USED_SLOT);
    fleet.trainCars[0].x = 2;
    fleet.trainCars[0].y = 2;
    CHECK_FALSE(world.is_city_or_station_tile(fleet, 2, 2));
}

TEST_CASE("fast: terrain commits the build edit buffer") {
    World world;
    world.resize(20, 20);
    BuildController build;
    build.selectionStartX = 3;
    build.selectionStartY = 4;
    build.terrainEditBuffer.resize(kTerrainEditGridSide, kTerrainEditGridSide);
    build.terrainEditBuffer[0][0] = 11;
    build.terrainEditBuffer[5][6] = -7;
    build.terrainEditBuffer[kTerrainEditGridSide - 1][kTerrainEditGridSide - 1] = 23;

    world.terrain.commit_edit(build);

    CHECK(world.terrain.heightMap[3][4] == 11);
    CHECK(world.terrain.heightMap[8][10] == -7);
    CHECK(world.terrain.heightMap[12][13] == 23);
}

TEST_CASE("fast: build controller takes a bounded terrain selection") {
    Terrain terrain;
    terrain.resize(20, 20);
    terrain.heightMap[11][12] = 7;
    BuildController build;
    build.terrainSmoothCost = 99;
    build.set_terrain_selection(terrain, 16, 17);
    CHECK(build.selectionStartX == 11);
    CHECK(build.selectionStartY == 11);
    CHECK(build.terrainEditBuffer[0][1] == 7);
    CHECK(build.terrainSmoothCost == 0);

    Economy economy;
    economy.overheadCost = 3;
    economy.crewCost = 4;
    CHECK(build.adjust_terrain_point(terrain, economy, 16, 17, 1, true));
    CHECK(build.terrainEditBuffer[5][6] == 1);
    CHECK(build.terrainSmoothCost == 7);
}

TEST_CASE("fast: terrain rebuild makes a flat tile from equal heights") {
    World world;
    world.resize(6, 6);
    BuildController build;
    world.terrain.tileMap[2][3] = 11;
    world.terrain.heightMap[2][3] = 2;
    world.terrain.heightMap[2][4] = 2;
    world.terrain.heightMap[3][3] = 2;
    world.terrain.heightMap[3][4] = 2;

    world.terrain.rebuild(build, false);

    CHECK(world.terrain.tileMap[2][3] == 0);
    CHECK(world.terrain.tileElevation[2][3] == 2);
}

TEST_CASE("fast: terrain queue keeps slot order and rejects a duplicate") {
    Terrain terrain;
    terrain.resize(6, 6);
    terrain.tileMap[2][3] = 10;

    CHECK(terrain.queue_change(2, 3, true));
    CHECK_FALSE(terrain.queue_change(2, 3, false));
    CHECK(unsigned_byte(terrain.tileMap[2][3]) == 138);
    CHECK(terrain.tileQueue[kTileQueueCount - 1].countdown == 14);
    CHECK(terrain.tileQueue[kTileQueueCount - 1].x == 2);
    CHECK(terrain.tileQueue[kTileQueueCount - 1].y == 3);
    CHECK(terrain.tileQueueVisualOnly[kTileQueueCount - 1]);
    CHECK(terrain.tileQueue[kTileQueueCount - 2].countdown == 0);
}

TEST_CASE("fast: save buffer bytes and round trip") {
    Game* game = make_game();
    GameTestAccess::economy(*game).money = 123456;
    GameTestAccess::economy(*game).dayOfMonth = 17;
    GameTestAccess::economy(*game).monthOfYear = 8;
    GameTestAccess::economy(*game).year = 1973;
    GameTestAccess::economy(*game).loanBalance = -7654321;
    GameTestAccess::renderer(*game).visibleTrains[0] = -1234;
    GameTestAccess::save_codec(*game).walk(*game, save::OP_MEMORY_WRITE);

    const int plain_size = GameTestAccess::save_codec(*game).snapshotSize;
    REQUIRE(plain_size > 0);
    StableDigest save_digest;
    save_digest.add_bytes(GameTestAccess::save_codec(*game).saveBuffer.data(), static_cast<std::size_t>(plain_size));
    CHECK(plain_size == 19242);
    CHECK(save_digest.value() == 16273675477449773753ULL);
    auto* plain = static_cast<int8_t*>(std::malloc(static_cast<std::size_t>(plain_size)));
    REQUIRE(plain != nullptr);
    std::memcpy(plain, GameTestAccess::save_codec(*game).saveBuffer.data(), static_cast<std::size_t>(plain_size));

    GameTestAccess::save_codec(*game).compress();
    GameTestAccess::save_codec(*game).decompress();
    CHECK(GameTestAccess::save_codec(*game).snapshotSize == plain_size);
    CHECK(std::memcmp(GameTestAccess::save_codec(*game).saveBuffer.data(), plain, static_cast<std::size_t>(plain_size)) == 0);

    GameTestAccess::economy(*game).money = 0;
    GameTestAccess::economy(*game).dayOfMonth = 0;
    GameTestAccess::economy(*game).monthOfYear = 0;
    GameTestAccess::economy(*game).year = 0;
    GameTestAccess::economy(*game).loanBalance = 0;
    GameTestAccess::renderer(*game).visibleTrains[0] = 0;
    GameTestAccess::save_codec(*game).walk(*game, save::OP_MEMORY_READ);
    CHECK(GameTestAccess::economy(*game).money == 123456);
    CHECK(GameTestAccess::economy(*game).dayOfMonth == 17);
    CHECK(GameTestAccess::economy(*game).monthOfYear == 8);
    CHECK(GameTestAccess::economy(*game).year == 1973);
    CHECK(GameTestAccess::economy(*game).loanBalance == -7654321);
    CHECK(GameTestAccess::renderer(*game).visibleTrains[0] == -1234);

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
    auto* output = static_cast<SilentMusicOutput*>(GameTestAccess::music(*game).output.get());

    game->load_music();
    CHECK(GameTestAccess::music(*game).available);
    CHECK_FALSE(GameTestAccess::music(*game).playing);
    CHECK(output->volume() == 40);

    game->play_music();
    CHECK(output->play_count() == 0);
    GameTestAccess::music(*game).enabled = true;
    game->play_music();
    CHECK(GameTestAccess::music(*game).playing);
    CHECK(output->play_count() == 1);
    game->play_music();
    CHECK(output->play_count() == 1);

    game->set_music_volume(5);
    CHECK(output->volume() == 100);
    game->stop_music();
    CHECK_FALSE(GameTestAccess::music(*game).playing);
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
    game->render(screen);
    StableDigest first_frame;
    first_frame.add_pixels(framebuffer, std::size(framebuffer));
    CHECK(first_frame.value() == 3551171988763127126ULL);

    const int old_viewport_x = GameTestAccess::renderer(*game).camera.viewportCenterX;
    game->key_pressed(54);
    game->advance_frame();
    game->render(screen);
    CHECK(GameTestAccess::state(*game) == 30);
    CHECK(GameTestAccess::renderer(*game).backbuffer != nullptr);
    CHECK(GameTestAccess::renderer(*game).camera.viewportCenterX != old_viewport_x);
    StableDigest moved_frame;
    moved_frame.add_pixels(framebuffer, std::size(framebuffer));
    CHECK(moved_frame.value() == 10927439313109183097ULL);

    delete game;
}

TEST_CASE("full: year rollover has a fixed saved-state digest") {
    Game* game = make_game();
    generate_scenario(game, 0);
    GameTestAccess::random(*game).set_seed(kFixedRandomSeed);
    GameTestAccess::ui(*game).monthlyReportEnabled = false;
    GameTestAccess::ui(*game).annualReportEnabled = false;
    GameTestAccess::economy(*game).dayOfMonth = 31;
    GameTestAccess::economy(*game).monthOfYear = 12;
    GameTestAccess::economy(*game).year = 1950;
    GameTestAccess::day_frame_counter(*game) = GameTestAccess::day_tick_threshold(*game);

    GameTestAccess::simulation_tick(*game);

    CHECK(GameTestAccess::economy(*game).dayOfMonth == 1);
    CHECK(GameTestAccess::economy(*game).monthOfYear == 1);
    CHECK(GameTestAccess::economy(*game).year == 1951);
    CHECK(GameTestAccess::day_frame_counter(*game) == 0);
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

    GameTestAccess::ready(*game) = false;
    GameTestAccess::ui(*game).transitionCode = 32;
    GameTestAccess::set_state(*game, GameState::MenuOverlay);
    game->render(screen);
    CHECK(paint_digest(game, &screen) == 611837288114446412ULL);

    GameTestAccess::ui(*game).set_year_graph_mode(GameTestAccess::economy(*game), 0, false);
    GameTestAccess::ui(*game).transitionCode = 33;
    GameTestAccess::set_state(*game, GameState::MenuOverlay);
    game->render(screen);
    CHECK(paint_digest(game, &screen) == 5091032400029738936ULL);
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
    REQUIRE(GameTestAccess::state(*game) == 30);
    REQUIRE(GameTestAccess::ui(*game).buildMenuMode == 0);

    // generate_scenario never paints, so set the render layout that the
    // first game_paint would normally establish (the live loop always has these).
    GameTestAccess::renderer(*game).canvasWidth = kScreenWidth;
    GameTestAccess::renderer(*game).canvasHeight = kScreenHeight;
    GameTestAccess::renderer(*game).bottomBarY = GameTestAccess::renderer(*game).canvasHeight - 15;

    // Mode-0 index 1 is always the "build" entry (item id 1); activating it
    // switches buildMenuMode to 1 and resets the item index to 0 (openBuildMenu
    // mode-0 case 1). Click that icon's x on the bottom bar.
    const int item1_x = static_cast<int>(static_cast<std::uint8_t>(GameTestAccess::ui(*game).menuItemCode[0][1]));
    REQUIRE(item1_x > 0);

    game->pointer_pressed(item1_x, GameTestAccess::renderer(*game).bottomBarY);

    CHECK(GameTestAccess::ui(*game).buildMenuMode == 1);
    CHECK(GameTestAccess::ui(*game).buildMenuItemIndex == 0);
    delete game;
}

TEST_CASE("full: pointer hover resolves the tile under the mouse") {
    Game* game = make_game();
    generate_scenario(game, 0);
    REQUIRE(GameTestAccess::state(*game) == 30);

    GameTestAccess::renderer(*game).canvasWidth = kScreenWidth;
    GameTestAccess::renderer(*game).canvasHeight = kScreenHeight;
    GameTestAccess::renderer(*game).bottomBarY = GameTestAccess::renderer(*game).canvasHeight - 15;

    // Hover the middle of the gameplay viewport. The viewport always shows the
    // map after generation, so its centre sits on a valid tile.
    const int hover_x = kScreenWidth / 2;
    const int hover_y = (15 + GameTestAccess::renderer(*game).bottomBarY) / 2;

    game->pointer_moved(hover_x, hover_y);
    REQUIRE(GameTestAccess::ui(*game).mouseOverField);

    const int tx = GameTestAccess::ui(*game).tileCursorX;
    const int ty = GameTestAccess::ui(*game).tileCursorY;
    CHECK(tx >= 0);
    CHECK(tx < GameTestAccess::world(*game).terrain.mapWidth);
    CHECK(ty >= 0);
    CHECK(ty < GameTestAccess::world(*game).terrain.mapHeight);

    // Round-trip: the resolved tile's drawn screen centre (the flat iso forward
    // transform the resolver mirrors, including the +15 top-strip draw shift)
    // must sit within one tile of the hover.
    const int tile_screen_x =
        ty * 16 + tx * 16 + 16 - GameTestAccess::renderer(*game).camera.viewportCenterX;
    const int tile_screen_y =
        GameTestAccess::world(*game).terrain.mapHalfWidth - tx * 8 + ty * 8 + 8 - GameTestAccess::renderer(*game).camera.viewportCenterY + 15;
    const int dx = tile_screen_x - hover_x;
    const int dy = tile_screen_y - hover_y;
    CHECK(dx >= -16);
    CHECK(dx <= 16);
    CHECK(dy >= -8);
    CHECK(dy <= 8);

    // Hovering UI chrome (the bottom bar) must not claim a field tile.
    game->pointer_moved(hover_x, GameTestAccess::renderer(*game).bottomBarY);
    CHECK_FALSE(GameTestAccess::ui(*game).mouseOverField);
    delete game;
}

TEST_CASE("full: pointer right-drag pans the viewport (grab-the-map)") {
    Game* game = make_game();
    generate_scenario(game, 0);
    REQUIRE(GameTestAccess::state(*game) == 30);

    // generate_scenario never paints, so set the render layout that the
    // first game_paint would normally establish (the live loop always has these).
    GameTestAccess::renderer(*game).canvasWidth = kScreenWidth;
    GameTestAccess::renderer(*game).canvasHeight = kScreenHeight;
    GameTestAccess::renderer(*game).bottomBarY = GameTestAccess::renderer(*game).canvasHeight - 15;

    const int cx0 = GameTestAccess::renderer(*game).camera.viewportCenterX;
    const int cy0 = GameTestAccess::renderer(*game).camera.viewportCenterY;
    const int msx0 = GameTestAccess::renderer(*game).camera.mapScaleX;
    const int msy0 = GameTestAccess::renderer(*game).camera.mapScaleY;

    // A zero motion delta never moves the camera.
    game->pointer_drag_pan(0, 0);
    CHECK(GameTestAccess::renderer(*game).camera.viewportCenterX == cx0);
    CHECK(GameTestAccess::renderer(*game).camera.viewportCenterY == cy0);

    // Grab-the-map: dragging right (dx > 0) moves the camera left -- opposite the
    // drag -- so the point under the cursor follows the mouse. mapScale
    // (= 2*viewportBase) moves 2x the camera delta so the run loop's
    // camera-follow sees an unchanged cursor offset and doesn't fight the pan. A
    // single-axis drag moves only that axis.
    const int drag = 20;
    game->pointer_drag_pan(drag, 0);
    CHECK(GameTestAccess::renderer(*game).camera.viewportCenterX - cx0 == -drag);
    CHECK(GameTestAccess::renderer(*game).camera.mapScaleX - msx0 == 2 * -drag);
    CHECK(GameTestAccess::renderer(*game).camera.viewportCenterY == cy0);

    // A downward drag moves only the Y axis, also opposite the drag.
    game->pointer_drag_pan(0, drag);
    CHECK(GameTestAccess::renderer(*game).camera.viewportCenterY - cy0 == -drag);
    CHECK(GameTestAccess::renderer(*game).camera.mapScaleY - msy0 == 2 * -drag);
    CHECK(GameTestAccess::renderer(*game).camera.viewportCenterX - cx0 == -drag);

    delete game;
}

TEST_CASE("full: generated map derives a road layer from its city streets") {
    Game* game = make_game();
    generate_scenario(game, 0);
    REQUIRE(GameTestAccess::state(*game) == 30);

    const World& world = GameTestAccess::world(*game);
    const RoadNetwork& net = GameTestAccess::road_net(*game);

    // The road layer mirrors exactly the street tiles (codes 42..48) the map
    // generator laid, and every road cell has a connected-component id.
    int street_tiles = 0;
    int road_cells = 0;
    for (int x = 0; x < world.terrain.mapWidth; ++x) {
        for (int y = 0; y < world.terrain.mapHeight; ++y) {
            const int code = unsigned_byte(world.terrain.tileMap[x][y]);
            const bool is_street = code >= 42 && code <= 48;
            if (is_street) ++street_tiles;
            const bool is_road = net.component_at(x, y) != kNoRoadComponent;
            CHECK(is_road == is_street);  // derived layer == street tiles
            if (is_road) ++road_cells;
        }
    }
    CHECK(street_tiles > 0);            // towns really do have streets
    CHECK(road_cells == street_tiles);

    delete game;
}

TEST_CASE("full: manual road placement writes a road tile and charges for it") {
    Game* game = make_game();
    generate_scenario(game, 0);
    const World& world = GameTestAccess::world(*game);

    // Find any buildable ground tile away from the map edges.
    int bx = -1, by = -1;
    for (int x = 1; x < world.terrain.mapWidth - 1 && bx < 0; ++x)
        for (int y = 1; y < world.terrain.mapHeight - 1 && bx < 0; ++y)
            if (GameTestAccess::road_buildable(*game, x, y)) { bx = x; by = y; }
    REQUIRE(bx >= 0);

    const int money_before = GameTestAccess::economy(*game).money;
    GameTestAccess::ui(*game).tileCursorX = (int8_t)bx;
    GameTestAccess::ui(*game).tileCursorY = (int8_t)by;
    REQUIRE(GameTestAccess::place_road_tile_at_cursor(*game));

    CHECK(GameTestAccess::is_road_tile(*game, bx, by));                 // became a road tile
    CHECK(GameTestAccess::road_net(*game).component_at(bx, by)
          != kNoRoadComponent);  // and joined the road graph
    CHECK(GameTestAccess::economy(*game).money == money_before - kRoadTileCost);

    delete game;
}

TEST_CASE("full: auto-route lays a connected road and charges per new tile") {
    Game* game = make_game();
    generate_scenario(game, 0);
    const World& world = GameTestAccess::world(*game);

    // Find a straight run of three buildable ground cells to route across.
    int ax = -1, ay = -1;
    for (int x = 1; x < world.terrain.mapWidth - 3 && ax < 0; ++x)
        for (int y = 1; y < world.terrain.mapHeight - 1 && ax < 0; ++y)
            if (GameTestAccess::road_buildable(*game, x, y)
                && GameTestAccess::road_buildable(*game, x + 1, y)
                && GameTestAccess::road_buildable(*game, x + 2, y)) { ax = x; ay = y; }
    REQUIRE(ax >= 0);

    const int money_before = GameTestAccess::economy(*game).money;
    REQUIRE(GameTestAccess::auto_route_road(*game, ax, ay, ax + 2, ay));

    // All three cells are now one connected road component.
    CHECK(GameTestAccess::is_road_tile(*game, ax, ay));
    CHECK(GameTestAccess::is_road_tile(*game, ax + 1, ay));
    CHECK(GameTestAccess::is_road_tile(*game, ax + 2, ay));
    CHECK(GameTestAccess::road_net(*game).same_component(RoadPoint{ax, ay},
                                                         RoadPoint{ax + 2, ay}));
    CHECK(GameTestAccess::economy(*game).money == money_before - 3 * kRoadTileCost);

    delete game;
}

// --- Road-stop test helpers -------------------------------------------------
// Road stops sit on cleared ground beside a street (not on it), so a test stop
// is a buildable cell that touches a road. These helpers find such cells and
// their bound town (the access road cell's connected component).
namespace {
struct RoadStopCell { int sx, sy, rx, ry, comp; };

int road_stop_zone_neighbours(const World& world, int center_x, int center_y) {
    int zone_neighbour_count = 0;
    for (int tile_x = center_x - 3; tile_x <= center_x + 3; ++tile_x)
        for (int tile_y = center_y - 3; tile_y <= center_y + 3; ++tile_y) {
            if (!world.in_bounds(tile_x, tile_y)) continue;
            const int tile_code =
                unsigned_byte(world.terrain.tileMap[tile_x][tile_y]);
            if (tile_code >= tile::BUILT_FIRST
                && tile_code <= tile::CITY_ZONE_LAST
                && (world.trackMap[tile_x][tile_y] & track_overlay::MASK)
                       == track_overlay::JUNCTION)
                ++zone_neighbour_count;
        }
    return zone_neighbour_count;
}

// Candidate stop cells: buildable ground touching a road. When min_zones > 0 the
// cell must also have that many populated-zone neighbours so it can serve cargo.
std::vector<RoadStopCell> road_stop_candidates(Game& game, int min_zones) {
    const World& world = GameTestAccess::world(game);
    std::vector<RoadStopCell> out;
    for (int x = 0; x < world.terrain.mapWidth; ++x)
        for (int y = 0; y < world.terrain.mapHeight; ++y) {
            if (!GameTestAccess::road_stop_placeable(game, x, y)) continue;
            const RoadPoint a = GameTestAccess::road_access_cell(game, x, y);
            if (a.x < 0) continue;
            if (min_zones > 0 && road_stop_zone_neighbours(world, x, y) < min_zones)
                continue;
            out.push_back({x, y, a.x, a.y,
                           GameTestAccess::road_net(game).component_at(a.x, a.y)});
        }
    return out;
}

// Populate the zones around a stop cell so it produces passengers.
void road_stop_seed_zones(Game& game, int cx, int cy, int pop) {
    World& world = GameTestAccess::world(game);
    for (int x = cx - 3; x <= cx + 3; ++x)
        for (int y = cy - 3; y <= cy + 3; ++y) {
            if (!world.in_bounds(x, y)) continue;
            const int t = unsigned_byte(world.terrain.tileMap[x][y]);
            if (t >= 34 && t <= 40)
                world.zonePopulation[world.trackMap[x][y] & 0x3F] = (int16_t)pop;
        }
}

// Pick two stop candidates in the SAME town whose access road cells differ and
// whose stop cells are a few tiles apart, so a bus has a real loop to run.
bool road_two_same_town_stops(Game& game, int min_zones,
                              RoadStopCell& a, RoadStopCell& b) {
    const std::vector<RoadStopCell> cands = road_stop_candidates(game, min_zones);
    for (std::size_t i = 0; i < cands.size(); ++i)
        for (std::size_t j = i + 1; j < cands.size(); ++j) {
            if (cands[i].comp < 0 || cands[i].comp != cands[j].comp) continue;
            if (cands[i].rx == cands[j].rx && cands[i].ry == cands[j].ry) continue;
            if (std::abs(cands[i].sx - cands[j].sx)
                    + std::abs(cands[i].sy - cands[j].sy) < 3)
                continue;
            a = cands[i];
            b = cands[j];
            return true;
        }
    return false;
}
}  // namespace

TEST_CASE("full: buses and trucks take mixed cargo up to one shared capacity") {
    Game* game = make_game();
    ServiceSite& site = GameTestAccess::fleet(*game).serviceSites[0];
    site.flags = static_cast<int8_t>(service_site::USED_SLOT
                                     | kRoadStopSiteType);

    RoadVehicle bus;
    bus.carType = road_vehicle_type::Bus;
    bus.capacity = kBusCapacity;
    site.resourceStock[service_resource::Passengers] = 30;
    site.resourceStock[service_resource::Mail] = 30;
    site.resourceStock[service_resource::IndustryGroup1] = 30;
    GameTestAccess::road_load_unload(*game, bus, 0);
    CHECK(bus.total_cargo() == kBusCapacity);
    CHECK(bus.cargo[service_resource::Passengers] == 20);
    CHECK(bus.cargo[service_resource::Mail] == 20);
    CHECK(bus.cargo[service_resource::IndustryGroup1] == 0);

    for (int resource = 0; resource < kResourceTypeCount; ++resource)
        site.resourceStock[resource] = 0;
    site.unloadFlags = static_cast<int8_t>(service_resource::PASSENGER_FLAG
                                           | service_resource::MAIL_FLAG);
    const int bus_money_before = GameTestAccess::economy(*game).money;
    GameTestAccess::road_load_unload(*game, bus, 0);
    CHECK(bus.total_cargo() == 0);
    CHECK(GameTestAccess::economy(*game).money
          == bus_money_before + 20 * 3 + 20 * 8 - kRoadRunCost);

    RoadVehicle truck;
    truck.carType = road_vehicle_type::Truck;
    truck.capacity = kTruckCapacity;
    site.unloadFlags = 0;
    site.resourceStock[service_resource::Passengers] = 30;
    site.resourceStock[service_resource::Mail] = 30;
    site.resourceStock[service_resource::IndustryGroup1] = 30;
    site.resourceStock[service_resource::IndustryGroup2] = 30;
    site.resourceStock[service_resource::IndustryGroup3] = 30;
    GameTestAccess::road_load_unload(*game, truck, 0);
    CHECK(truck.total_cargo() == kTruckCapacity);
    CHECK(truck.cargo[service_resource::Passengers] == 0);
    CHECK(truck.cargo[service_resource::Mail] == 0);
    CHECK(truck.cargo[service_resource::IndustryGroup1] == 10);
    CHECK(truck.cargo[service_resource::IndustryGroup2] == 10);
    CHECK(truck.cargo[service_resource::IndustryGroup3] == 10);

    delete game;
}

TEST_CASE("full: road stops use unique ground cells beside a road") {
    Game* game = make_game();
    generate_scenario(game, 0);
    const World& world = GameTestAccess::world(*game);

    // Find an isolated open 5x3 area. The centre becomes one road cell; the two
    // side cells would both bind to it.
    int road_x = -1;
    int road_y = -1;
    for (int x = 2; x < world.terrain.mapWidth - 2 && road_x < 0; ++x) {
        for (int y = 1; y < world.terrain.mapHeight - 1 && road_x < 0; ++y) {
            bool open = true;
            for (int scan_x = x - 2; scan_x <= x + 2; ++scan_x)
                for (int scan_y = y - 1; scan_y <= y + 1; ++scan_y)
                    open = open
                        && world.is_placeable_terrain(scan_x, scan_y);
            if (open) { road_x = x; road_y = y; }
        }
    }
    REQUIRE(road_x >= 0);

    UiController& ui = GameTestAccess::ui(*game);
    ui.tileCursorX = static_cast<int8_t>(road_x);
    ui.tileCursorY = static_cast<int8_t>(road_y);
    REQUIRE(GameTestAccess::place_road_tile_at_cursor(*game));
    CHECK_FALSE(GameTestAccess::road_stop_placeable(*game, road_x, road_y));

    ui.tileCursorX = static_cast<int8_t>(road_x + 1);
    REQUIRE(GameTestAccess::place_road_stop_at_cursor(*game));
    CHECK(GameTestAccess::road_stop_at(*game, road_x + 1, road_y)
          != kNoFleetSlot);
    CHECK(unsigned_byte(GameTestAccess::world(*game).terrain.tileMap
                            [road_x + 1][road_y])
          == tile::VERTICAL_STATION);

    // The other side has the same road access cell, so a second shelter there
    // is refused instead of making a zero-length route.
    ui.tileCursorX = static_cast<int8_t>(road_x - 1);
    CHECK_FALSE(GameTestAccess::road_stop_placeable(*game, road_x - 1,
                                                     road_y));
    const int money_before = GameTestAccess::economy(*game).money;
    CHECK_FALSE(GameTestAccess::place_road_stop_at_cursor(*game));
    CHECK(GameTestAccess::economy(*game).money == money_before);

    // A new road cannot be put under or on another side of this stop, because
    // that would change the access cell used by its saved routes.
    ui.tileCursorX = static_cast<int8_t>(road_x + 2);
    CHECK_FALSE(GameTestAccess::road_buildable(*game, road_x + 2, road_y));
    CHECK_FALSE(GameTestAccess::place_road_tile_at_cursor(*game));
    ui.tileCursorX = static_cast<int8_t>(road_x + 1);
    CHECK_FALSE(GameTestAccess::place_road_tile_at_cursor(*game));

    delete game;
}

TEST_CASE("full: player roads can be demolished, city streets cannot") {
    Game* game = make_game();
    generate_scenario(game, 0);
    World& world = GameTestAccess::world(*game);
    UiController& ui = GameTestAccess::ui(*game);

    // Find an open 3x1 strip (with one cell of margin) and lay three road
    // tiles in a row.
    int road_x = -1;
    int road_y = -1;
    for (int x = 1; x < world.terrain.mapWidth - 3 && road_x < 0; ++x) {
        for (int y = 1; y < world.terrain.mapHeight - 1 && road_x < 0; ++y) {
            bool open = true;
            for (int scan_x = x - 1; scan_x <= x + 3; ++scan_x)
                for (int scan_y = y - 1; scan_y <= y + 1; ++scan_y)
                    open = open
                        && world.is_placeable_terrain(scan_x, scan_y);
            if (open) { road_x = x; road_y = y; }
        }
    }
    REQUIRE(road_x >= 0);
    for (int dx = 0; dx < 3; ++dx) {
        ui.tileCursorX = static_cast<int8_t>(road_x + dx);
        ui.tileCursorY = static_cast<int8_t>(road_y);
        REQUIRE(GameTestAccess::place_road_tile_at_cursor(*game));
    }
    const RoadNetwork& net = GameTestAccess::road_net(*game);
    REQUIRE(net.component_at(road_x, road_y)
            == net.component_at(road_x + 2, road_y));

    // Demolish the middle tile: the pending bit takes it out of the road
    // layer at once, so the row splits into two components immediately.
    REQUIRE(GameTestAccess::road_demolishable(*game, road_x + 1, road_y));
    ui.tileCursorX = static_cast<int8_t>(road_x + 1);
    GameTestAccess::demolish_at_cursor(*game);
    CHECK_FALSE(GameTestAccess::is_road_tile(*game, road_x + 1, road_y));
    CHECK(net.component_at(road_x, road_y)
          != net.component_at(road_x + 2, road_y));

    // A city street (trackMap carries the city owner) is not demolishable.
    int city_x = -1;
    int city_y = -1;
    for (int x = 0; x < world.terrain.mapWidth && city_x < 0; ++x)
        for (int y = 0; y < world.terrain.mapHeight && city_x < 0; ++y)
            if (GameTestAccess::is_road_tile(*game, x, y)
                && world.trackMap[x][y] != 0) { city_x = x; city_y = y; }
    REQUIRE(city_x >= 0);
    CHECK_FALSE(GameTestAccess::road_demolishable(*game, city_x, city_y));
    ui.tileCursorX = static_cast<int8_t>(city_x);
    ui.tileCursorY = static_cast<int8_t>(city_y);
    GameTestAccess::demolish_at_cursor(*game);
    CHECK(GameTestAccess::is_road_tile(*game, city_x, city_y));

    delete game;
}

TEST_CASE("full: road auto-route preview shows the path and cost before building") {
    Game* game = make_game();
    generate_scenario(game, 0);
    World& world = GameTestAccess::world(*game);
    UiController& ui = GameTestAccess::ui(*game);
    BuildController& build = GameTestAccess::build(*game);

    // Find an open 4x1 strip (with one cell of margin) for a straight route.
    int road_x = -1;
    int road_y = -1;
    for (int x = 1; x < world.terrain.mapWidth - 4 && road_x < 0; ++x) {
        for (int y = 1; y < world.terrain.mapHeight - 1 && road_x < 0; ++y) {
            bool open = true;
            for (int scan_x = x - 1; scan_x <= x + 4; ++scan_x)
                for (int scan_y = y - 1; scan_y <= y + 1; ++scan_y)
                    open = open
                        && world.is_placeable_terrain(scan_x, scan_y);
            if (open) { road_x = x; road_y = y; }
        }
    }
    REQUIRE(road_x >= 0);

    // Arm the anchor at one end and point the cursor at the other: the
    // preview reports exactly what the second click would build.
    build.roadAutoAnchorX = road_x;
    build.roadAutoAnchorY = road_y;
    ui.tileCursorX = static_cast<int8_t>(road_x + 3);
    ui.tileCursorY = static_cast<int8_t>(road_y);
    GameTestAccess::update_road_route_preview(*game);
    REQUIRE(build.roadPreviewPath.size() == 4);
    CHECK(build.roadPreviewCost == 4 * kRoadTileCost);

    // The build charges what the preview showed.
    const int money_before = GameTestAccess::economy(*game).money;
    REQUIRE(GameTestAccess::auto_route_road(*game, road_x, road_y,
                                            road_x + 3, road_y));
    CHECK(GameTestAccess::economy(*game).money
          == money_before - build.roadPreviewCost);

    // Clearing the anchor clears the preview.
    build.roadAutoAnchorX = -1;
    build.roadAutoAnchorY = -1;
    GameTestAccess::update_road_route_preview(*game);
    CHECK(build.roadPreviewPath.empty());

    delete game;
}

TEST_CASE("full: a road stop beside a zero-zone factory does not divide by zero") {
    Game* game = make_game();
    generate_scenario(game, 0);
    World& world = GameTestAccess::world(*game);

    int factory_x = -1;
    int factory_y = -1;
    int stop_x = -1;
    int stop_y = -1;
    int road_x = -1;
    int road_y = -1;
    int stock_index = -1;
    constexpr RoadPoint offsets[] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    for (int fx = 0; fx < world.terrain.mapWidth && factory_x < 0; ++fx) {
        for (int fy = 0; fy < world.terrain.mapHeight && factory_x < 0; ++fy) {
            const int tile_code = unsigned_byte(world.terrain.tileMap[fx][fy]);
            int candidate_stock = -1;
            if (tile_code >= industry_tile::Group1ProductionFirst
                && tile_code <= industry_tile::Group1ProductionLast)
                candidate_stock = service_resource::IndustryGroup1;
            if (tile_code >= industry_tile::Group2ProductionFirst
                && tile_code <= industry_tile::Group2ProductionLast)
                candidate_stock = service_resource::IndustryGroup2;
            if (tile_code >= industry_tile::Group3ProductionFirst
                && tile_code <= industry_tile::Group3ProductionLast)
                candidate_stock = service_resource::IndustryGroup3;
            if (candidate_stock < 0) continue;

            for (int sx = fx - 3; sx <= fx + 3 && factory_x < 0; ++sx) {
                for (int sy = fy - 3; sy <= fy + 3 && factory_x < 0; ++sy) {
                    if (!world.in_bounds(sx, sy)
                        || !world.is_placeable_terrain(sx, sy))
                        continue;
                    for (RoadPoint offset : offsets) {
                        const int rx = sx + offset.x;
                        const int ry = sy + offset.y;
                        if (!world.in_bounds(rx, ry)
                            || !world.is_placeable_terrain(rx, ry))
                            continue;
                        factory_x = fx; factory_y = fy;
                        stop_x = sx; stop_y = sy;
                        road_x = rx; road_y = ry;
                        stock_index = candidate_stock;
                        break;
                    }
                }
            }
        }
    }
    REQUIRE(factory_x >= 0);

    const int city_id = world.trackMap[factory_x][factory_y]
                        & track::OWNER_MASK;
    REQUIRE(city_id < kCitySlotCount);
    world.cities[city_id].zoneTileCount = 0;
    world.cities[city_id].population = 60;

    UiController& ui = GameTestAccess::ui(*game);
    ui.tileCursorX = static_cast<int8_t>(road_x);
    ui.tileCursorY = static_cast<int8_t>(road_y);
    REQUIRE(GameTestAccess::place_road_tile_at_cursor(*game));
    ui.tileCursorX = static_cast<int8_t>(stop_x);
    ui.tileCursorY = static_cast<int8_t>(stop_y);
    REQUIRE(GameTestAccess::place_road_stop_at_cursor(*game));
    const int stop_slot = GameTestAccess::road_stop_at(*game, stop_x, stop_y);
    REQUIRE(stop_slot != kNoFleetSlot);

    GameTestAccess::generate_site_cargo(*game);
    CHECK(GameTestAccess::fleet(*game).serviceSites[stop_slot]
              .resourceStock[stock_index] > 0);

    delete game;
}

TEST_CASE("full: a road stop is a service site and collects city passengers") {
    Game* game = make_game();
    generate_scenario(game, 0);
    Fleet& fleet = GameTestAccess::fleet(*game);

    // A stop sits on cleared ground beside a town street, with several zones in
    // pickup range (3) so passengers are produced.
    const std::vector<RoadStopCell> cands = road_stop_candidates(*game, 3);
    REQUIRE(!cands.empty());
    const RoadStopCell stop = cands.front();
    const int sx = stop.sx, sy = stop.sy;
    road_stop_seed_zones(*game, sx, sy, 50);

    const int money_before = GameTestAccess::economy(*game).money;
    GameTestAccess::ui(*game).tileCursorX = (int8_t)sx;
    GameTestAccess::ui(*game).tileCursorY = (int8_t)sy;
    REQUIRE(GameTestAccess::place_road_stop_at_cursor(*game));
    CHECK(GameTestAccess::economy(*game).money == money_before - kRoadStopCost);

    const int slot = fleet.find_service_site_at(sx, sy);
    REQUIRE(slot != 5000);
    CHECK((fleet.serviceSites[slot].flags & 7) == kRoadStopSiteType);  // typed as a road stop
    CHECK((fleet.serviceSites[slot].flags & 0x80) != 0);               // and in use

    for (int i = 0; i < 5; ++i) GameTestAccess::generate_site_cargo(*game);
    CHECK((fleet.serviceSites[slot].unloadFlags & 1) != 0);   // accepts/serves passengers
    CHECK(fleet.serviceSites[slot].resourceStock[0] > 0);     // and has collected some

    delete game;
}

TEST_CASE("full: a city bus loops two stops and earns passenger fares") {
    Game* game = make_game();
    generate_scenario(game, 0);

    // Two stops on cleared ground beside streets of the SAME town, a few tiles
    // apart, each near enough zones to serve passengers.
    RoadStopCell a{}, b{};
    REQUIRE(road_two_same_town_stops(*game, 3, a, b));
    road_stop_seed_zones(*game, a.sx, a.sy, 60);
    road_stop_seed_zones(*game, b.sx, b.sy, 60);

    GameTestAccess::ui(*game).tileCursorX = (int8_t)a.sx;
    GameTestAccess::ui(*game).tileCursorY = (int8_t)a.sy;
    REQUIRE(GameTestAccess::place_road_stop_at_cursor(*game));
    GameTestAccess::ui(*game).tileCursorX = (int8_t)b.sx;
    GameTestAccess::ui(*game).tileCursorY = (int8_t)b.sy;
    REQUIRE(GameTestAccess::place_road_stop_at_cursor(*game));
    const int first_stop = GameTestAccess::road_stop_at(*game, b.sx, b.sy);
    REQUIRE(first_stop != kNoFleetSlot);

    // Fill both stops with waiting passengers.
    for (int i = 0; i < 8; ++i) GameTestAccess::generate_site_cargo(*game);

    REQUIRE(GameTestAccess::buy_road_vehicle(*game, 2, first_stop));
    std::vector<RoadVehicle>& buses = GameTestAccess::road_vehicles(*game);
    REQUIRE(buses.size() == 1);
    REQUIRE(buses[0].stopSites.size() == 2);
    REQUIRE(buses[0].route.size() >= 2);
    CHECK(buses[0].stopSites.front() == first_stop);
    CHECK(buses[0].route.front().x == b.rx);
    CHECK(buses[0].route.front().y == b.ry);

    const int money_after_buy = GameTestAccess::economy(*game).money;
    // Drive many sub-tile steps; refill the stops periodically the way the live
    // sim does, so the route keeps carrying passengers rather than draining once.
    for (int i = 0; i < 3000; ++i) {
        if (i % 40 == 0) GameTestAccess::generate_site_cargo(*game);
        GameTestAccess::update_road_vehicles(*game);
    }

    CHECK(buses[0].revenue > 0);                                  // it earned net of upkeep
    CHECK(GameTestAccess::economy(*game).money > money_after_buy);  // fares reached the bank

    // Removing a stop also removes a bus that can no longer have a route.
    GameTestAccess::ui(*game).tileCursorX = (int8_t)b.sx;
    GameTestAccess::ui(*game).tileCursorY = (int8_t)b.sy;
    GameTestAccess::demolish_at_cursor(*game);
    CHECK(GameTestAccess::road_stop_at(*game, b.sx, b.sy) == kNoFleetSlot);
    CHECK(buses.empty());

    delete game;
}

TEST_CASE("full: build-menu road tool lays a road on the cursor tile") {
    Game* game = make_game();
    generate_scenario(game, 0);
    const World& world = GameTestAccess::world(*game);
    UiController& ui = GameTestAccess::ui(*game);

    // The build submenu (mode 1) must carry the "lay road" tool (item id 7).
    REQUIRE(ui.buildMenuItems.rows() > 1);
    int road_index = -1;
    for (int i = 0; i < (int)ui.buildMenuItems[6].size(); ++i)
        if (ui.buildMenuItems[6][i] == 0) road_index = i;
    REQUIRE(road_index >= 0);

    int bx = -1, by = -1;
    for (int x = 1; x < world.terrain.mapWidth - 1 && bx < 0; ++x)
        for (int y = 1; y < world.terrain.mapHeight - 1 && bx < 0; ++y)
            if (GameTestAccess::road_buildable(*game, x, y)) { bx = x; by = y; }
    REQUIRE(bx >= 0);

    ui.buildMenuMode = 6;
    ui.buildMenuItemIndex = (int8_t)road_index;
    ui.blockBuildMenu = false;
    ui.tileCursorX = (int8_t)bx;
    ui.tileCursorY = (int8_t)by;
    const int money_before = GameTestAccess::economy(*game).money;

    GameTestAccess::open_build_menu(*game);  // same call the map click makes

    CHECK(GameTestAccess::is_road_tile(*game, bx, by));
    CHECK(GameTestAccess::economy(*game).money == money_before - kRoadTileCost);

    delete game;
}

TEST_CASE("full: road auto-route builds through a land slope") {
    Game* game = make_game();
    World& world = GameTestAccess::world(*game);
    world.resize(5, 5);
    GameTestAccess::road_net(*game).rebuild_from_tiles(world.terrain);
    GameTestAccess::economy(*game).money = 1000000;

    world.terrain.tileMap[2][2] = static_cast<int8_t>(tile::SLOPE_FIRST);
    world.terrain.tileElevation[2][2] = 1;
    REQUIRE(GameTestAccess::road_buildable(*game, 2, 2));
    REQUIRE(GameTestAccess::auto_route_road(*game, 1, 2, 3, 2));

    CHECK(GameTestAccess::is_road_tile(*game, 1, 2));
    CHECK(GameTestAccess::is_road_tile(*game, 2, 2));
    CHECK(GameTestAccess::is_road_tile(*game, 3, 2));
    CHECK(world.terrain.tileElevation[2][2] == 1);

    delete game;
}

TEST_CASE("full: road-stop buy dialog buys a bus or truck for the cursor network") {
    Game* game = make_game();
    generate_scenario(game, 0);
    UiController& ui = GameTestAccess::ui(*game);

    // Two stops beside streets of the same town, a few tiles apart.
    RoadStopCell a{}, b{};
    REQUIRE(road_two_same_town_stops(*game, 0, a, b));
    ui.tileCursorX = (int8_t)a.sx;
    ui.tileCursorY = (int8_t)a.sy;
    REQUIRE(GameTestAccess::place_road_stop_at_cursor(*game));
    ui.tileCursorX = (int8_t)b.sx;
    ui.tileCursorY = (int8_t)b.sy;
    REQUIRE(GameTestAccess::place_road_stop_at_cursor(*game));
    const int selected_stop = GameTestAccess::road_stop_at(*game, b.sx, b.sy);
    REQUIRE(selected_stop != kNoFleetSlot);

    // The road submenu (mode 6) must carry the "buy bus" tool (item id 3).
    int buy_index = -1;
    for (int i = 0; i < (int)ui.buildMenuItems[6].size(); ++i)
        if (ui.buildMenuItems[6][i] == 3) buy_index = i;
    REQUIRE(buy_index >= 0);

    // Arming the tool and clicking a stop opens the buy dialog (sprite 100)
    // with that exact stop captured for the dialog dispatch.
    ui.buildMenuMode = 6;
    ui.buildMenuItemIndex = (int8_t)buy_index;
    ui.blockBuildMenu = false;

    // A plain road cell is not a buy point.
    ui.tileCursorX = (int8_t)a.rx;
    ui.tileCursorY = (int8_t)a.ry;
    GameTestAccess::open_build_menu(*game);
    CHECK(GameTestAccess::state(*game) == GameState::Playing);
    CHECK(GameTestAccess::road_vehicles(*game).empty());

    ui.tileCursorX = (int8_t)b.sx;
    ui.tileCursorY = (int8_t)b.sy;
    GameTestAccess::open_build_menu(*game);

    REQUIRE(GameTestAccess::ui(*game).transitionCode == 100);
    REQUIRE(GameTestAccess::state(*game) == GameState::MenuOverlay);
    REQUIRE(GameTestAccess::build(*game).roadStopCursor == selected_stop);

    // Action 1 = buy bus (passengers, carType 2); the dialog then closes.
    const int money_before_bus = GameTestAccess::economy(*game).money;
    GameTestAccess::handle_build_menu_state(*game, 100, 1);
    REQUIRE(GameTestAccess::road_vehicles(*game).size() == 1);
    CHECK(GameTestAccess::road_vehicles(*game)[0].carType == 2);
    CHECK(GameTestAccess::road_vehicles(*game)[0].stopSites.front()
          == selected_stop);
    CHECK(GameTestAccess::economy(*game).money == money_before_bus - kBusCost);
    CHECK(GameTestAccess::state(*game) == GameState::Playing);

    // Reopen and choose action 2 = buy truck (freight, carType 4).
    GameTestAccess::open_build_menu(*game);
    REQUIRE(GameTestAccess::ui(*game).transitionCode == 100);
    const int money_before_truck = GameTestAccess::economy(*game).money;
    GameTestAccess::handle_build_menu_state(*game, 100, 2);
    REQUIRE(GameTestAccess::road_vehicles(*game).size() == 2);
    CHECK(GameTestAccess::road_vehicles(*game)[1].carType == 4);
    CHECK(GameTestAccess::economy(*game).money == money_before_truck - kTruckCost);

    // Closing the dialog (the close X sends option -1) buys nothing.
    GameTestAccess::open_build_menu(*game);
    const std::size_t owned = GameTestAccess::road_vehicles(*game).size();
    GameTestAccess::handle_build_menu_state(*game, 100, -1);
    CHECK(GameTestAccess::road_vehicles(*game).size() == owned);
    CHECK(GameTestAccess::state(*game) == GameState::Playing);

    delete game;
}

TEST_CASE("full: road vehicles survive a save/load round-trip") {
    Game* game = make_game();
    generate_scenario(game, 0);

    // Two stops beside streets of the same town, a few tiles apart.
    RoadStopCell a{}, b{};
    REQUIRE(road_two_same_town_stops(*game, 0, a, b));
    GameTestAccess::ui(*game).tileCursorX = (int8_t)a.sx;
    GameTestAccess::ui(*game).tileCursorY = (int8_t)a.sy;
    REQUIRE(GameTestAccess::place_road_stop_at_cursor(*game));
    GameTestAccess::ui(*game).tileCursorX = (int8_t)b.sx;
    GameTestAccess::ui(*game).tileCursorY = (int8_t)b.sy;
    REQUIRE(GameTestAccess::place_road_stop_at_cursor(*game));
    const int first_stop = GameTestAccess::road_stop_at(*game, a.sx, a.sy);
    REQUIRE(first_stop != kNoFleetSlot);
    REQUIRE(GameTestAccess::buy_road_vehicle(*game, 2, first_stop));

    const std::vector<int32_t> expected_stops =
        GameTestAccess::road_vehicles(*game)[0].stopSites;
    const int expected_type = GameTestAccess::road_vehicles(*game)[0].carType;
    GameTestAccess::road_vehicles(*game)[0]
        .cargo[service_resource::Passengers] = 11;
    GameTestAccess::road_vehicles(*game)[0].cargo[service_resource::Mail] = 9;
    const std::array<int, kResourceTypeCount> expected_cargo =
        GameTestAccess::road_vehicles(*game)[0].cargo;

    GameTestAccess::settings(*game).animationsDisabled = true;
    GameTestAccess::settings(*game).selectionHidden = true;
    GameTestAccess::save_codec(*game).load_slot(save::slot::Auxiliary,
                                                 save::MODE_WRITE);
    GameTestAccess::save_codec(*game).walk_settings(*game);
    GameTestAccess::save_codec(*game).persist();
    GameTestAccess::save_codec(*game).save_all(*game);
    GameTestAccess::road_vehicles(*game).clear();  // wipe the in-memory fleet
    GameTestAccess::settings(*game).animationsDisabled = false;
    GameTestAccess::settings(*game).selectionHidden = false;
    GameTestAccess::save_codec(*game).restore_all(*game);

    // Saving road vehicles must not erase the settings record.
    GameTestAccess::save_codec(*game).load_slot(save::slot::Auxiliary,
                                                 save::MODE_READ);
    GameTestAccess::save_codec(*game).walk_settings(*game);
    CHECK(GameTestAccess::settings(*game).animationsDisabled);
    CHECK(GameTestAccess::settings(*game).selectionHidden);

    std::vector<RoadVehicle>& restored = GameTestAccess::road_vehicles(*game);
    REQUIRE(restored.size() == 1);
    CHECK(restored[0].carType == expected_type);
    CHECK(restored[0].cargo == expected_cargo);
    CHECK(restored[0].stopSites == expected_stops);
    CHECK(restored[0].route.size() >= 2);           // route rebuilt from the stop list

    // Version 1 stored one cargo amount and used the freight car type as its
    // resource. Read it into the matching mixed-cargo counter.
    SaveCodec& codec = GameTestAccess::save_codec(*game);
    codec.saveBuffer.clear();
    const auto write_int = [&codec](int32_t value) {
        const uint32_t bits = static_cast<uint32_t>(value);
        for (int shift = 0; shift < 32; shift += 8)
            codec.saveBuffer.push_back(static_cast<int8_t>(bits >> shift));
    };
    write_int(0x52565431);                         // magic
    write_int(1);                                  // old version
    write_int(1);                                  // one vehicle
    write_int(wagon::IndustryGroup2CarA);          // old truck/resource type
    write_int(kTruckCapacity);
    write_int(17);                                 // old single cargo amount
    write_int(0);                                  // revenue
    write_int(0);                                  // segment
    write_int(0);                                  // phase
    write_int(road_direction::PositiveX);
    codec.saveBuffer.push_back(1);                 // forward
    codec.saveBuffer.push_back(1);                 // loop
    write_int(static_cast<int32_t>(expected_stops.size()));
    for (int stop : expected_stops) write_int(stop);
    codec.savePendingMode = save::MODE_READ;
    codec.saveReadOffset = 0;
    codec.snapshotSize = static_cast<int32_t>(codec.saveBuffer.size());
    codec.snapshotLoaded = true;
    codec.walk_road_vehicles(*game);

    REQUIRE(GameTestAccess::road_vehicles(*game).size() == 1);
    const RoadVehicle& migrated = GameTestAccess::road_vehicles(*game)[0];
    CHECK(migrated.carType == road_vehicle_type::Truck);
    CHECK(migrated.total_cargo() == 17);
    CHECK(migrated.cargo[service_resource::IndustryGroup2] == 17);

    delete game;
}

TEST_CASE("full: starting a new map clears old road vehicles") {
    Game* game = make_game();
    RoadVehicle vehicle;
    vehicle.route = {RoadPoint{1, 1}, RoadPoint{2, 1}};
    GameTestAccess::road_vehicles(*game).push_back(vehicle);
    GameTestAccess::build(*game).roadStopCursor = 7;
    GameTestAccess::build(*game).roadAutoAnchorX = 4;
    GameTestAccess::build(*game).roadAutoAnchorY = 5;

    GameTestAccess::begin_map_generation(*game);

    CHECK(GameTestAccess::road_vehicles(*game).empty());
    CHECK(GameTestAccess::road_net(*game).width == 0);
    CHECK(GameTestAccess::build(*game).roadStopCursor == -1);
    CHECK(GameTestAccess::build(*game).roadAutoAnchorX == -1);
    CHECK(GameTestAccess::build(*game).roadAutoAnchorY == -1);

    delete game;
}

TEST_CASE("full: a built road links two towns and one bus serves both") {
    Game* game = make_game();
    generate_scenario(game, 0);
    const RoadNetwork& net = GameTestAccess::road_net(*game);

    // Stop cells beside streets, near passengers, tagged with their town. Among
    // cross-town pairs (nearest first) pick one whose access road cells are
    // reachable over land, so a road can connect them. Reachability is a dry-run
    // BFS over existing road plus buildable ground -- what auto_route_road uses.
    const std::vector<RoadStopCell> cells = road_stop_candidates(*game, 3);
    const auto drivable = [&](int x, int y) {
        return GameTestAccess::is_road_tile(*game, x, y)
               || GameTestAccess::road_buildable(*game, x, y);
    };
    struct Pair { int i, j, d; };
    std::vector<Pair> pairs;
    for (std::size_t i = 0; i < cells.size(); ++i)
        for (std::size_t j = i + 1; j < cells.size(); ++j)
            if (cells[i].comp >= 0 && cells[i].comp != cells[j].comp)
                pairs.push_back({(int)i, (int)j,
                                 std::abs(cells[i].sx - cells[j].sx)
                                     + std::abs(cells[i].sy - cells[j].sy)});
    std::sort(pairs.begin(), pairs.end(),
              [](const Pair& l, const Pair& r) { return l.d < r.d; });
    int ia = -1, ib = -1;
    for (const Pair& p : pairs) {
        if (!net.plan_route(RoadPoint{cells[p.i].rx, cells[p.i].ry},
                            RoadPoint{cells[p.j].rx, cells[p.j].ry}, drivable).empty()) {
            ia = p.i; ib = p.j; break;
        }
    }
    REQUIRE(ia >= 0);                                   // two towns joinable by road
    const RoadStopCell a = cells[ia], b = cells[ib];
    REQUIRE(a.comp != b.comp);                          // distinct towns before we build

    // Drop a stop beside each town's street, then build a road joining the two
    // access cells; the towns must merge into one network.
    road_stop_seed_zones(*game, a.sx, a.sy, 60);
    road_stop_seed_zones(*game, b.sx, b.sy, 60);
    GameTestAccess::ui(*game).tileCursorX = (int8_t)a.sx;
    GameTestAccess::ui(*game).tileCursorY = (int8_t)a.sy;
    REQUIRE(GameTestAccess::place_road_stop_at_cursor(*game));
    GameTestAccess::ui(*game).tileCursorX = (int8_t)b.sx;
    GameTestAccess::ui(*game).tileCursorY = (int8_t)b.sy;
    REQUIRE(GameTestAccess::place_road_stop_at_cursor(*game));

    REQUIRE(GameTestAccess::auto_route_road(*game, a.rx, a.ry, b.rx, b.ry));
    REQUIRE(GameTestAccess::road_net(*game).same_component(RoadPoint{a.rx, a.ry},
                                                           RoadPoint{b.rx, b.ry}));
    for (int i = 0; i < 8; ++i) GameTestAccess::generate_site_cargo(*game);

    const int first_stop = GameTestAccess::road_stop_at(*game, a.sx, a.sy);
    REQUIRE(first_stop != kNoFleetSlot);
    REQUIRE(GameTestAccess::buy_road_vehicle(*game, 2, first_stop));
    std::vector<RoadVehicle>& buses = GameTestAccess::road_vehicles(*game);
    REQUIRE(buses[0].stopSites.size() == 2);            // serves a stop in each town

    const int money_after_buy = GameTestAccess::economy(*game).money;
    for (int i = 0; i < 6000; ++i) {
        if (i % 40 == 0) GameTestAccess::generate_site_cargo(*game);
        GameTestAccess::update_road_vehicles(*game);
    }
    CHECK(buses[0].revenue > 0);
    CHECK(GameTestAccess::economy(*game).money > money_after_buy);

    delete game;
}
