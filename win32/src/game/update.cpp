#include "game.h"
#include "scenario_definition.h"

namespace {

namespace minimap_kind {
enum Id : int32_t {
    None = 0,
    FlatTerrain = 1,
    SlopedTerrain = 2,
    Scenery = 3,
    CityZone = 4,
    Road = 5,
    IndustryGroup1Destination = 6,
    IndustryGroup2Production = 7,
    IndustryGroup2Destination = 8,
    IndustryGroup3Production = 9,
    IndustryGroup3Destination = 10,
    IndustryGroup1Production = 11,
    Rail = 12,
    RailOverlayFirst = 13,
    RailOverlaySecond = 14,
    RailOverlayThird = 15,
};
}  // namespace minimap_kind

namespace minimap_elevation {
// These are stored tile-elevation bytes, not visual color ids.
enum Level : int32_t {
    BelowZero = -1,
    LevelZero = 0,
    LevelOne = 1,
    LevelTwo = 2,
    LevelThree = 3,
};
}  // namespace minimap_elevation

namespace minimap_state {
constexpr int32_t kNoPreviousKind = -1;
}  // namespace minimap_state

namespace minimap_color {
constexpr int32_t kElevationMinusOne = 3167360;
constexpr int32_t kElevationZero = 5274640;
constexpr int32_t kElevationOne = 6327328;
constexpr int32_t kElevationTwo = 7380016;
constexpr int32_t kElevationThree = 8432704;
constexpr int32_t kSlopedTerrain = 6327312;
constexpr int32_t kScenery = 0x909040;
constexpr int32_t kCityZone = 0x606060;
constexpr int32_t kRoad = 0x404040;
constexpr int32_t kIndustryGroup1Destination = 0xFC0000;
constexpr int32_t kIndustryGroup2Production = 0x101010;
constexpr int32_t kIndustryGroup2Destination = 0xFC9C00;
constexpr int32_t kIndustryGroup3Production = 0x78DC1E;
constexpr int32_t kIndustryGroup3Destination = 0xFCFC00;
constexpr int32_t kIndustryGroup1Production = 0x80C4FC;
constexpr int32_t kRail = 0;
constexpr int32_t kRailOverlayFirst = 14287063;
constexpr int32_t kRailOverlaySecond = 16738557;
constexpr int32_t kRailOverlayThird = 16040947;
}  // namespace minimap_color

namespace minimap_tile_code {
constexpr int32_t kEmpty = 0;
// The minimap groups the built city range through the legacy code 41. The
// native city-zone systems use the narrower proven range 34..40 elsewhere.
constexpr int32_t kCityZoneFirst = tile::BUILT_FIRST;
constexpr int32_t kCityZoneLast = 41;
}  // namespace minimap_tile_code

struct EconomyGenerationDefaults {
    int32_t moneyBasePrimary;
    int32_t moneyBaseSecondary;
    int32_t baseTrainSpeedEarly;
    int32_t accelerationEarly;
    int32_t brakingEarly;
    int32_t baseTrainSpeedLate;
    int32_t accelerationLate;
    int32_t powerCost;
    int32_t structureCost;
    int32_t overheadCost;
    int32_t maintenanceCost;
    int32_t crewCost;
    int32_t trainCostFirstEra;
    int32_t trainCostSecondEra;
    int32_t trainCostThirdEra;
    int32_t trainCostFourthEra;
    int32_t trainCostFifthEra;
    int32_t trainCostSixthEra;
    int32_t wagonOptionCostPassengerMail;
    int32_t wagonOptionCostIndustryGroup1;
    int32_t wagonOptionCostIndustryGroup2;
    int32_t wagonOptionCostIndustryGroup3;
};

constexpr EconomyGenerationDefaults kEconomyGenerationDefaults = {
    974, 974, 173, 342, 311, 173, 234,
    4759, 9845, 347, 5982, 481,
    37894, 43247, 51872, 60149, 71423, 82675,
    4578, 5196, 4314, 5072,
};

constexpr int32_t kMegapolisPatchMapScaleNumerator = 3;
constexpr int32_t kMegapolisPatchMapScaleDenominator = 10;

int32_t middle_value(int32_t small, int32_t large) {
    return small + (large - small) / 2;
}

ScenarioDefinition make_medium_free_game_definition(
    const ScenarioDefinition& small, const ScenarioDefinition& large) {
    ScenarioDefinition medium = large;
    medium.mapSize = middle_value(small.mapSize, large.mapSize);
    medium.terrainRaiseSteps = middle_value(
        small.terrainRaiseSteps, large.terrainRaiseSteps);
    medium.terrainLowerSteps = middle_value(
        small.terrainLowerSteps, large.terrainLowerSteps);
    medium.obstacleCount = middle_value(small.obstacleCount, large.obstacleCount);
    medium.terrainPatchSize = middle_value(
        small.terrainPatchSize, large.terrainPatchSize);
    medium.mapGenColumns = middle_value(
        small.mapGenColumns, large.mapGenColumns);
    medium.mapGenRows = middle_value(small.mapGenRows, large.mapGenRows);
    medium.featureStage0Count = middle_value(
        small.featureStage0Count, large.featureStage0Count);
    medium.featureStage1Count = middle_value(
        small.featureStage1Count, large.featureStage1Count);
    medium.featureStage2Count = middle_value(
        small.featureStage2Count, large.featureStage2Count);
    medium.featureStage3Count = middle_value(
        small.featureStage3Count, large.featureStage3Count);
    medium.featureStage4Count = middle_value(
        small.featureStage4Count, large.featureStage4Count);
    medium.featureStage5Count = middle_value(
        small.featureStage5Count, large.featureStage5Count);
    return medium;
}

int32_t middle_count(int32_t full_count) {
    return (full_count + full_count / 2) / 2;
}

void set_middle_industry_counts(World& world) {
    world.featureStage0Count = middle_count(world.featureStage0Count);
    world.featureStage1Count = middle_count(world.featureStage1Count);
    world.featureStage2Count = middle_count(world.featureStage2Count);
    world.featureStage3Count = middle_count(world.featureStage3Count);
    world.featureStage4Count = middle_count(world.featureStage4Count);
    world.featureStage5Count = middle_count(world.featureStage5Count);
}

void halve_industry_counts(World& world) {
    world.featureStage0Count /= 2;
    world.featureStage1Count /= 2;
    world.featureStage2Count /= 2;
    world.featureStage3Count /= 2;
    world.featureStage4Count /= 2;
    world.featureStage5Count /= 2;
}

}  // namespace

void Game::apply_scenario_definition(const ScenarioDefinition& definition) {
    Game *game = this;
    game->scenario.scenarioStep = definition.scenarioIndex;
    game->world.terrain.mapWidth = game->world.terrain.mapHeight = definition.mapSize;
    game->world.terrain.mapHalfWidth =
        (game->world.terrain.mapWidth - 1) * map_geometry::kTileHalfWidthPixels;
    game->world.terrain.mapScreenWidth =
        (game->world.terrain.mapWidth + game->world.terrain.mapHeight)
        * map_geometry::kTileWidthPixels;
    game->world.terrain.mapScreenHeight =
        (game->world.terrain.mapWidth + game->world.terrain.mapHeight)
        * map_geometry::kTileHeightPixels;
    game->renderer.camera.viewportHalfWidth =
        game->world.terrain.mapScreenWidth / map_geometry::kViewportScaleFactor;
    game->renderer.camera.viewportHalfHeight =
        game->world.terrain.mapScreenHeight / map_geometry::kViewportScaleFactor;
    game->renderer.camera.viewportCenterX = game->renderer.camera.viewportHalfWidth;
    game->renderer.camera.viewportCenterY = game->renderer.camera.viewportHalfHeight;
    game->renderer.camera.viewportBaseX = game->renderer.camera.viewportCenterX;
    game->renderer.camera.viewportBaseY = game->renderer.camera.viewportCenterY;
    game->renderer.camera.mapScaleX =
        game->renderer.camera.viewportBaseX * map_geometry::kViewportScaleFactor;
    game->renderer.camera.mapScaleY =
        game->renderer.camera.viewportBaseY * map_geometry::kViewportScaleFactor;
    game->world.initialObstacleCount = definition.obstacleCount;
    game->world.terrainRaiseStepCount = definition.terrainRaiseSteps;
    game->world.terrainLowerStepCount = definition.terrainLowerSteps;
    game->world.terrainPatchBaseSize = definition.terrainPatchSize;
    game->world.mapGenColumns = definition.mapGenColumns;
    game->world.mapGenRows = definition.mapGenRows;
    game->world.featureStage3Count = definition.featureStage3Count;
    game->world.featureStage2Count = definition.featureStage2Count;
    game->world.featureStage1Count = definition.featureStage1Count;
    game->world.featureStage0Count = definition.featureStage0Count;
    game->world.featureStage5Count = definition.featureStage5Count;
    game->world.featureStage4Count = definition.featureStage4Count;
    game->economy.money = definition.startingMoney;
    game->economy.loanCeiling = definition.loanCeiling;
    game->economy.loanBalance = definition.startingLoan;
    game->economy.interestRate = static_cast<int8_t>(definition.interestRate);
    game->scenario.currentEra = definition.eraNumber - 1;
    game->scenario.scenarioMilestoneIndex = static_cast<int8_t>(definition.milestoneIndex);
    game->scenario.currentMilestoneScore = definition.milestoneScore;
    game->scenario.milestoneCoThreshold2 = definition.milestoneThreshold;
    game->ui.init_build_menu(game->scenario, definition.buildMenuScenario);
    game->scenario.aiExpansionEnabled =
        (definition.scenarioFlags & scenario_flag::AiExpansion) != 0;
    game->scenario.monthlyOffersEnabled =
        (definition.scenarioFlags & scenario_flag::MonthlyOffers) != 0;
    if ((definition.scenarioFlags & scenario_flag::Enabled) == 0) {
        game->scenario.aiFrequency = 25;
        game->scenario.aiThreshold = 100;
    } else {
        game->scenario.aiFrequency = 32;
        game->scenario.aiThreshold = 29;
    }
    game->scenario.scenarioNoticeId = definition.noticeId;
    if (definition.noticeId != 0) {
        game->ui.transitionDebounce = 5;
        game->scenario.scenarioNoticeCode = static_cast<int8_t>(definition.noticeId);
    } else {
        game->ui.transitionDebounce = 0;
    }
}

void Game::begin_map_generation() {
    Game *game = this;
    game->saveCodec.walk(*game, save::OP_RESET_SKIP);
    game->roadVehicles.clear();
    game->roadNet.clear();
    game->build.roadStopCursor = -1;
    game->build.roadAutoAnchorX = -1;
    game->build.roadAutoAnchorY = -1;
    game->economy.dayOfMonth = game_calendar::kFirstDay;
    game->economy.monthOfYear = game_calendar::kFirstMonth;
    game->economy.year = game_calendar::kBaseYear;
    game->scenario.milestonesEnabled = false; game->ui.pauseFlag = false; game->scenario.aiEnabled = false;
    const EconomyGenerationDefaults& defaults = kEconomyGenerationDefaults;
    game->economy.moneyBasePrimary = defaults.moneyBasePrimary;
    game->economy.moneyBaseSecondary = defaults.moneyBaseSecondary;
    game->economy.baseTrainSpeedEarly = defaults.baseTrainSpeedEarly;
    game->economy.accelerationEarly = defaults.accelerationEarly;
    game->economy.brakingEarly = defaults.brakingEarly;
    game->economy.baseTrainSpeedLate = defaults.baseTrainSpeedLate;
    game->economy.accelerationLate = defaults.accelerationLate;
    game->economy.powerCost = defaults.powerCost;
    game->economy.structureCost = defaults.structureCost;
    game->economy.overheadCost = defaults.overheadCost;
    game->economy.maintenanceCost = defaults.maintenanceCost;
    game->economy.crewCost = defaults.crewCost;
    game->economy.trainCostFirstEra = defaults.trainCostFirstEra;
    game->economy.trainCostSecondEra = defaults.trainCostSecondEra;
    game->economy.trainCostThirdEra = defaults.trainCostThirdEra;
    game->economy.trainCostFourthEra = defaults.trainCostFourthEra;
    game->economy.trainCostFifthEra = defaults.trainCostFifthEra;
    game->economy.trainCostSixthEra = defaults.trainCostSixthEra;
    game->economy.wagonOptionCostPassengerMail = defaults.wagonOptionCostPassengerMail;
    game->economy.wagonOptionCostIndustryGroup1 = defaults.wagonOptionCostIndustryGroup1;
    game->economy.wagonOptionCostIndustryGroup2 = defaults.wagonOptionCostIndustryGroup2;
    game->economy.wagonOptionCostIndustryGroup3 = defaults.wagonOptionCostIndustryGroup3;

    ScenarioDefinition selected_definition;
    ScenarioDefinition small_free_game_definition;
    ScenarioDefinition large_free_game_definition;
    bool selected_definition_found = false;
    bool small_free_game_definition_found = false;
    bool large_free_game_definition_found = false;
    ResourceReader resource = resources::open("scenarios.bin");
    if (resource) {
        const int recordCount = resource.read_byte();
        for (int recordIndex = 0; recordIndex < recordCount; ++recordIndex) {
            ScenarioDefinition definition;
            if (!readScenarioDefinition(resource, definition)) break;
            if (definition.scenarioIndex == 0) {
                small_free_game_definition = definition;
                small_free_game_definition_found = true;
            }
            if (definition.scenarioIndex == 1) {
                large_free_game_definition = definition;
                large_free_game_definition_found = true;
            }
            if (definition.scenarioIndex == game->scenario.activeScenario) {
                selected_definition = definition;
                selected_definition_found = true;
            }
        }
    }
    if (game->scenario.activeScenario <= 1
        && game->scenario.freeGameMapSize == free_game_map_size::Medium
        && small_free_game_definition_found
        && large_free_game_definition_found) {
        selected_definition = make_medium_free_game_definition(
            small_free_game_definition, large_free_game_definition);
        selected_definition_found = true;
    }
    if (selected_definition_found)
        game->apply_scenario_definition(selected_definition);

    if (game->scenario.activeScenario <= 1) {
        if (game->scenario.aiDifficulty == 0) { game->scenario.aiFrequency = 32; game->scenario.aiThreshold = 29; }
        if (game->scenario.aiDifficulty == 1) { game->scenario.aiFrequency = 25; game->scenario.aiThreshold = 22; }
        if (game->scenario.financialDifficulty == 0) { game->economy.interestRate = 4; game->economy.enterpriseTaxPct = 2; }
        if (game->scenario.financialDifficulty == 1) { game->economy.interestRate = 8; game->economy.enterpriseTaxPct = 5; }
        if (game->scenario.freeGameTownLevel == free_game_town_level::Few) {
            game->world.mapGenColumns /= 2;
            game->world.mapGenRows /= 2;
        }
        if (game->scenario.freeGameTownLevel == free_game_town_level::Medium) {
            game->world.mapGenColumns = middle_count(game->world.mapGenColumns);
            game->world.mapGenRows = middle_count(game->world.mapGenRows);
        }
        if (game->scenario.freeGameTownLevel == free_game_town_level::Megapolis) {
            game->world.mapGenColumns = 2;
            game->world.mapGenRows = 2;
            game->world.terrainPatchBaseSize =
                game->world.terrain.mapWidth
                * kMegapolisPatchMapScaleNumerator
                / kMegapolisPatchMapScaleDenominator;
        }
        if (game->scenario.freeGameIndustryLevel
            == free_game_industry_level::Few)
            halve_industry_counts(game->world);
        if (game->scenario.freeGameIndustryLevel
            == free_game_industry_level::Medium)
            set_middle_industry_counts(game->world);
    }
}

void Game::update_boot_splash() {
    Game *game = this;
    if (midp_current_time_millis() - game->loadingStartTime > 5000L)
        game->loadingTimeout = true;
    ++game->loadingAnimationFrame;
    if (!game->loadingTimeout) return;
    if (game->gameState == GameState::BootSplash1) {
        for (int font_page_index = 0; font_page_index < kFontPageCount;
             ++font_page_index)
            game->assets.font_images[font_page_index] =
                game->assets.decode_png_from_sprite_stream();
        game->set_state(GameState::IntroReady);
        return;
    }
    game->set_state(game->gameState + 1);
}

void Game::update_intro_ready() {
    Game *game = this;
    game->ready = true;
    game->ui.screenParam = save_load_mode::LoadResources;
    game->ui.transitionCode = menu_screen::SaveLoadDialog;
    game->reset_overlay_render_state();
    game->set_state(GameState::MenuOverlay);
}

void Game::update_loading_resources() {
    Game *game = this;
    game->assets.tile_sprites[game->assets.spriteLoadCounter] =
        game->assets.decode_png_from_sprite_stream();
    ++game->assets.spriteLoadCounter;
    if (game->assets.spriteLoadCounter == game->scenario.scenarioResourceCount / 2) {
        game->load_music();
        if (game->music.autoStart) { game->play_music(); game->music.enabled = true; }
        else game->music.enabled = false;
    }
    if (game->assets.spriteLoadCounter != game->scenario.scenarioResourceCount) return;
    game->ready = true;
    game->ui.transitionCode = menu_screen::MainMenu;
    game->set_state(GameState::MenuOverlay);
}

void Game::update_generating_map() {
    Game *game = this;
    ++game->stateWorkStep;
    if (game->stateWorkStep == 2) game->begin_map_generation();
    if (game->stateWorkStep == 3) {
        game->ui.buildMenuItemIndex = 0;
        game->ui.buildMenuMode = 0;
        game->run_map_generation_step(0);
    }
    if (game->stateWorkStep <= 3) return;
    game->run_map_generation_step(1);
    if (game->world.mapGenTilesDone != game->world.totalTilesToGenerate) return;
    game->world.mapGenTilesDone = 0;
    // Derive the road layer (and its per-town connected components) from the
    // freshly laid city street tiles (42..48) so buses can route the moment the
    // map is playable. Read-only over the caches, so fixed-seed goldens are
    // untouched.
    game->roadNet.rebuild_from_tiles(game->world.terrain);
    game->set_state(GameState::Playing);
}

void Game::update_playing() {
    Game *game = this;
    game->simulation_tick();
}

void Game::set_minimap_color(int kind, int x, int y) {
    Game *game = this;
    Surface *surface = game->renderer.backbuffer_surface;
    switch (kind) {
        case minimap_kind::FlatTerrain:
            if (game->world.terrain.tileElevation[x][y]
                == minimap_elevation::BelowZero)
                midp_set_color(surface, minimap_color::kElevationMinusOne);
            if (game->world.terrain.tileElevation[x][y]
                == minimap_elevation::LevelZero)
                midp_set_color(surface, minimap_color::kElevationZero);
            if (game->world.terrain.tileElevation[x][y]
                == minimap_elevation::LevelOne)
                midp_set_color(surface, minimap_color::kElevationOne);
            if (game->world.terrain.tileElevation[x][y]
                == minimap_elevation::LevelTwo)
                midp_set_color(surface, minimap_color::kElevationTwo);
            if (game->world.terrain.tileElevation[x][y]
                == minimap_elevation::LevelThree)
                midp_set_color(surface, minimap_color::kElevationThree);
            break;
        case minimap_kind::SlopedTerrain:
            midp_set_color(surface, minimap_color::kSlopedTerrain);
            break;
        case minimap_kind::Scenery:
            midp_set_color(surface, minimap_color::kScenery);
            break;
        case minimap_kind::CityZone:
            midp_set_color(surface, minimap_color::kCityZone);
            break;
        case minimap_kind::Road:
            midp_set_color(surface, minimap_color::kRoad);
            break;
        case minimap_kind::IndustryGroup1Destination:
            midp_set_color(surface, minimap_color::kIndustryGroup1Destination);
            break;
        case minimap_kind::IndustryGroup2Production:
            midp_set_color(surface, minimap_color::kIndustryGroup2Production);
            break;
        case minimap_kind::IndustryGroup2Destination:
            midp_set_color(surface, minimap_color::kIndustryGroup2Destination);
            break;
        case minimap_kind::IndustryGroup3Production:
            midp_set_color(surface, minimap_color::kIndustryGroup3Production);
            break;
        case minimap_kind::IndustryGroup3Destination:
            midp_set_color(surface, minimap_color::kIndustryGroup3Destination);
            break;
        case minimap_kind::IndustryGroup1Production:
            midp_set_color(surface, minimap_color::kIndustryGroup1Production);
            break;
        case minimap_kind::Rail:
            midp_set_color(surface, minimap_color::kRail);
            break;
        case minimap_kind::RailOverlayFirst:
            midp_set_color(surface, minimap_color::kRailOverlayFirst);
            break;
        case minimap_kind::RailOverlaySecond:
            midp_set_color(surface, minimap_color::kRailOverlaySecond);
            break;
        case minimap_kind::RailOverlayThird:
            midp_set_color(surface, minimap_color::kRailOverlayThird);
            break;
    }
}

void Game::build_minimap_overview() {
    Game *game = this;
    const int overview_size =
        game->world.terrain.mapHeight + game->world.terrain.mapWidth;
    game->renderer.backbuffer =
        Image::create_blank(overview_size, overview_size);
    game->renderer.backbuffer_surface = game->renderer.backbuffer->drawing_surface();
    midp_set_color(game->renderer.backbuffer_surface, 0);
    midp_fill_rect(game->renderer.backbuffer_surface, 0, 0,
                   overview_size, overview_size);
    int previous_kind = minimap_state::kNoPreviousKind;
    int minimap_kind_id = minimap_kind::None;
    for (int x = 0; x < game->world.terrain.mapWidth; ++x) {
        for (int y = 0; y < game->world.terrain.mapHeight; ++y) {
            const int draw_y = game->world.terrain.mapWidth - x + y;
            const int draw_x = x + y;
            const int tile_code = game->world.terrain.tileMap[x][y];
            const int track_code = game->world.trackMap[x][y];
            if (tile_code == minimap_tile_code::kEmpty)
                minimap_kind_id = minimap_kind::FlatTerrain;
            if (tile_code >= tile::SLOPE_FIRST
                && tile_code <= tile::SLOPE_LAST)
                minimap_kind_id = minimap_kind::SlopedTerrain;
            if (tile_code >= tile::SCENERY_FIRST
                && tile_code <= tile::SCENERY_LAST)
                minimap_kind_id = minimap_kind::Scenery;
            if (tile_code >= minimap_tile_code::kCityZoneFirst
                && tile_code <= minimap_tile_code::kCityZoneLast)
                minimap_kind_id = minimap_kind::CityZone;
            if (tile_code >= road_tile::kFirstShape
                && tile_code <= road_tile::kLastShape)
                minimap_kind_id = minimap_kind::Road;
            if (tile_code >= industry_tile::Group1DestinationFirst
                && tile_code <= industry_tile::Group1DestinationLast)
                minimap_kind_id = minimap_kind::IndustryGroup1Destination;
            if (tile_code >= industry_tile::Group2ProductionFirst
                && tile_code <= industry_tile::Group2ProductionLast)
                minimap_kind_id = minimap_kind::IndustryGroup2Production;
            if (tile_code >= industry_tile::Group2DestinationFirst
                && tile_code <= industry_tile::Group2DestinationLast)
                minimap_kind_id = minimap_kind::IndustryGroup2Destination;
            if (tile_code >= industry_tile::Group3ProductionFirst
                && tile_code <= industry_tile::Group3ProductionLast)
                minimap_kind_id = minimap_kind::IndustryGroup3Production;
            if (tile_code >= industry_tile::Group3DestinationFirst
                && tile_code <= industry_tile::Group3DestinationLast)
                minimap_kind_id = minimap_kind::IndustryGroup3Destination;
            if (tile_code >= industry_tile::Group1ProductionFirst
                && tile_code <= industry_tile::Group1ProductionLast)
                minimap_kind_id = minimap_kind::IndustryGroup1Production;
            if (tile_code >= tile::TRACK_FIRST
                && tile_code <= tile::TRACK_LAST) {
                if (track_code == track::PRESENT
                    || track_code == tile::SERVICE_TRACK)
                    minimap_kind_id = minimap_kind::Rail;
                if (track_code == track::AI_PIECE_FIRST)
                    minimap_kind_id = minimap_kind::RailOverlayFirst;
                if (track_code == track::AI_PIECE_FIRST + 1)
                    minimap_kind_id = minimap_kind::RailOverlaySecond;
                if (track_code == track::AI_PIECE_FIRST + 2)
                    minimap_kind_id = minimap_kind::RailOverlayThird;
            }
            if (minimap_kind_id != previous_kind)
                game->set_minimap_color(minimap_kind_id, x, y);
            previous_kind = minimap_kind_id;
            midp_draw_line(game->renderer.backbuffer_surface, draw_x, draw_y,
                           draw_x + 1, draw_y);
        }
        previous_kind = minimap_state::kNoPreviousKind;
    }
    game->set_state(GameState::MapOverview);
}

void Game::update_building_minimap() {
    Game *game = this;
    if (game->stateWorkStep > 0) --game->stateWorkStep;
    if (game->stateWorkStep == 2) game->build_minimap_overview();
}

void Game::update_map_overview() {
    Game *game = this;
    game->renderer.camera.pan(
        game->ui.lastKeyCode, game->world.terrain.mapScreenWidth,
        game->world.terrain.mapScreenHeight, game->renderer.canvasWidth,
        game->renderer.gameplayViewportHeight);
}

void Game::update_building_path() {
    Game *game = this;
    if (game->stateWorkStep > 0) --game->stateWorkStep;
    if (game->stateWorkStep != 2) return;
    if (game->pathfinder.try_build_path(game->world, game->build,
                                        game->economy)) {
        game->set_state(GameState::Playing);
        game->ui.buildMenuItemIndex = 0;
        game->build.buildCancelled = true;
        game->ui.buildMenuMode = 2;
        game->ui.menuDirty = true;
        return;
    }
    game->ui.buildMenuItemIndex = 0;
    game->build.buildCancelled = true;
    game->ui.buildMenuMode = 1;
    game->ui.menuDirty = true;
    game->ready = false;
    game->ui.transitionCode = menu_screen::GenericOverlayH;
    game->set_state(GameState::MenuOverlay);
}

void Game::update_saving_game() {
    Game *game = this;
    if (game->stateWorkStep > 0) --game->stateWorkStep;
    if (game->stateWorkStep != 2) return;
    game->saveCodec.save_all(*game);
    game->ui.buildMenuItemIndex = 0;
    game->ui.menuDirty = true;
    game->set_state(GameState::Playing);
}

void Game::update_loading_game() {
    Game *game = this;
    if (game->stateWorkStep > 0) --game->stateWorkStep;
    if (game->stateWorkStep != 2) return;
    game->saveCodec.restore_all(*game);
    game->ui.pauseFlag = false;
    game->ui.buildMenuItemIndex = 0;
    game->ui.menuDirty = true;
    game->set_state(GameState::Playing);
}

void Game::update_ai_turn() {
    Game *game = this;
    if (game->stateWorkStep > 0) --game->stateWorkStep;
    if (game->stateWorkStep != 2) return;
    if (game->build.nextTrackPieceCode - track::AI_PIECE_FIRST < game->scenario.difficultyCycle) {
        game->place_cities_and_industry();
        game->set_state(GameState::Playing);
        return;
    }
    ++game->build.nextTrackPieceCode;
    if (game->build.nextTrackPieceCode == track::AI_PIECE_WRAP)
        game->build.nextTrackPieceCode = track::AI_PIECE_FIRST;
    game->set_state(GameState::Playing);
}

void Game::update_high_score_sync() {
    Game *game = this;
    if (game->stateWorkStep > 0) --game->stateWorkStep;
    if (game->stateWorkStep != 2) return;
    game->ui.transitionCode = game->sync_high_scores(game->scenario.highScoreUploadRequested)
                              ? 19 : 18;
    game->set_state(GameState::MenuOverlay);
}

void Game::update_state_machine() {
    Game* game = this;
    switch (game->gameState) {
        case GameState::BootSplash0:
        case GameState::BootSplash1:
        case GameState::BootSplash2: game->update_boot_splash(); break;
        case GameState::IntroReady: game->update_intro_ready(); break;
        case GameState::LoadingResources: game->update_loading_resources(); break;
        case GameState::GeneratingMap: game->update_generating_map(); break;
        case GameState::Playing: game->update_playing(); break;
        case GameState::BuildingMinimap: game->update_building_minimap(); break;
        case GameState::MapOverview: game->update_map_overview(); break;
        case GameState::BuildingPath: game->update_building_path(); break;
        case GameState::SavingGame: game->update_saving_game(); break;
        case GameState::LoadingGame: game->update_loading_game(); break;
        case GameState::AiTurn: game->update_ai_turn(); break;
        case GameState::HighScoreSync: game->update_high_score_sync(); break;
    }
}
