// Map and game-state queries.
#include "game.h"
#include <array>
#include <stdlib.h> // abs

namespace {

namespace query_sprite {
constexpr int32_t Graph = 34;
constexpr int32_t Snapshot = 1;
constexpr int32_t ScenarioProgress = 5;
constexpr int32_t StationDirection = 21;
constexpr int32_t ServiceSite = 22;
constexpr int32_t EraAvailability = 24;
constexpr int32_t TrainInfo = 26;
constexpr int32_t MenuRows = 60;
}  // namespace query_sprite

namespace query_condition {
constexpr int32_t NoCondition = 0;
constexpr int32_t SnapshotLoaded = 1;
constexpr int32_t StationEvenDirection = 0;
constexpr int32_t StationOddDirectionA = 1;
constexpr int32_t StationOddDirectionB = 2;
constexpr int32_t FirstStock = 1;
constexpr int32_t LastStock = 5;
constexpr int32_t NoUnloadFlags = 6;
constexpr int32_t FirstUnloadFlag = 7;
constexpr int32_t LastUnloadFlag = 11;
constexpr int32_t TrainRunningState = 6;
constexpr int32_t TrainStoppedState = 7;
constexpr int32_t TrainHasAge = 8;
constexpr int32_t TrainStateCodeForCondition = 7;
}  // namespace query_condition

namespace query_graph {
constexpr int32_t CursorMarker = 65;
constexpr int32_t HistoryFieldBase = 20;
constexpr int32_t ProfitValueOffset = 64;
constexpr int32_t ProfitStartOffset = 65;
constexpr int32_t OtherValueOffset = 34;
constexpr int32_t OtherStartOffset = 35;
}  // namespace query_graph

// The five unload conditions use the legacy flag order, which differs from the
// resource-stock order. Keep the table explicit so the cross-map stays visible.
constexpr std::array<uint8_t, 5> kUnloadConditionFlags = {
    service_resource::PASSENGER_FLAG,
    service_resource::INDUSTRY_GROUP3_FLAG,
    service_resource::INDUSTRY_GROUP1_FLAG,
    service_resource::INDUSTRY_GROUP2_FLAG,
    service_resource::MAIL_FLAG,
};

}  // namespace

int32_t UiController::advance_key_repeat() {
    if (keySubState < 30) keySubState += keyRepeatStep;
    keySubState = (int8_t)keySubState;
    return keySubState / 3;
}

void Game::record_money_change(int32_t amount) {
    Game* game = this;
    game->economy.push_money_history(amount);
    game->ui.moneyTickerFrames[money_ticker::Oldest] =
        game->ui.moneyTickerFrames[money_ticker::Middle];
    game->ui.moneyTickerFrames[money_ticker::Middle] =
        game->ui.moneyTickerFrames[money_ticker::Newest];
    game->ui.moneyTickerFrames[money_ticker::Newest] =
        money_ticker::kFrameDuration;
}

void Economy::push_money_history(int32_t amount) {
    moneyHistory[money_ticker::Oldest] = moneyHistory[money_ticker::Middle];
    moneyHistory[money_ticker::Middle] = moneyHistory[money_ticker::Newest];
    moneyHistory[money_ticker::Newest] = amount;
}

bool World::has_track_step(int32_t start_x, int32_t start_y,
                           int32_t step_x, int32_t step_y,
                           int32_t target_tile) const {
    bool found_target = false;
    while ((start_x += step_x) >= 0 && (start_y += step_y) >= 0
           && start_x < terrain.mapWidth && start_y < terrain.mapHeight) {
        if (terrain.tileMap[start_x][start_y] == target_tile)
            found_target = true;
        if (terrain.tileMap[start_x][start_y] == 0
            && trackMap[start_x][start_y] == 0)
            continue;
        break;
    }
    return found_target;
}

int32_t UiController::report_graph_value(int32_t sprite_id,
                                         int32_t index) const {
    if (sprite_id == query_sprite::Graph) {
        if (graphSeries == report_series::Profit
            || graphSeries == report_series::CompanyValue) {
            if (index == query_graph::CursorMarker)
                return query_graph::CursorMarker;
            if (index >= query_graph::HistoryFieldBase)
                return query_graph::ProfitValueOffset
                    + graphBarHeights[index - query_graph::HistoryFieldBase];
            return query_graph::ProfitStartOffset + graphBarHeights[index];
        }
        if (index == query_graph::CursorMarker)
            return query_graph::OtherStartOffset;
        if (index >= query_graph::HistoryFieldBase)
            return query_graph::OtherValueOffset
                + graphBarHeights[index - query_graph::HistoryFieldBase];
        return query_graph::OtherStartOffset + graphBarHeights[index];
    }
    return 0;
}

bool BuildController::adjust_terrain_point(const Terrain& terrain,
                                           const Economy& economy,
                                           int32_t x, int32_t y,
                                           int32_t target_elevation,
                                           bool raise) {
    bool terrain_can_change = true;
    if (x > 0) {
        if (terrain.tileMap[x - 1][y] >= tile::BUILT_FIRST)
            terrain_can_change = false;
        if (y > 0 && terrain.tileMap[x - 1][y - 1] >= tile::BUILT_FIRST)
            terrain_can_change = false;
    }
    if (y > 0 && terrain.tileMap[x][y - 1] >= tile::BUILT_FIRST)
        terrain_can_change = false;
    if (terrain.tileMap[x][y] >= tile::BUILT_FIRST)
        terrain_can_change = false;
    const int32_t local_x = x - selectionStartX;
    const int32_t local_y = y - selectionStartY;
    if (terrain_can_change) {
        terrainEditBuffer[local_x][local_y] =
            static_cast<int8_t>(target_elevation);
        const int elevation_step = raise ? 1 : -1;
        if (local_x > 0
            && (raise ? terrainEditBuffer[local_x - 1][local_y]
                         < target_elevation - 1
                      : terrainEditBuffer[local_x - 1][local_y]
                         > target_elevation + 1)
            && !adjust_terrain_point(terrain, economy, x - 1, y,
                                     target_elevation - elevation_step, raise))
            terrain_can_change = false;
        if (local_y > 0
            && (raise ? terrainEditBuffer[local_x][local_y - 1]
                         < target_elevation - 1
                      : terrainEditBuffer[local_x][local_y - 1]
                         > target_elevation + 1)
            && !adjust_terrain_point(terrain, economy, x, y - 1,
                                     target_elevation - elevation_step, raise))
            terrain_can_change = false;
        if (local_x < kTerrainEditGridSide - 1
            && (raise ? terrainEditBuffer[local_x + 1][local_y]
                         < target_elevation - 1
                      : terrainEditBuffer[local_x + 1][local_y]
                         > target_elevation + 1)
            && !adjust_terrain_point(terrain, economy, x + 1, y,
                                     target_elevation - elevation_step, raise))
            terrain_can_change = false;
        if (local_y < kTerrainEditGridSide - 1
            && (raise ? terrainEditBuffer[local_x][local_y + 1]
                         < target_elevation - 1
                      : terrainEditBuffer[local_x][local_y + 1]
                         > target_elevation + 1)
            && !adjust_terrain_point(terrain, economy, x, y + 1,
                                     target_elevation - elevation_step, raise))
            terrain_can_change = false;
    }
    terrainSmoothCost += economy.overheadCost
        + (target_elevation == -1
               ? economy.maintenanceCost
               : abs(target_elevation) * economy.crewCost);
    return terrain_can_change;
}

bool Game::matches_resource_condition(int32_t sprite_id,
                                      int32_t condition_id) {
    Game* game = this;
    if (sprite_id != query_sprite::Snapshot)
        game->ui.snapshotLoadedFlag = false;
    if (sprite_id == query_sprite::Snapshot) {
        if (!game->ui.snapshotLoadedFlag) {
            game->snapshotLoadSucceeded = game->saveCodec.load_main_snapshot();
            game->ui.snapshotLoadedFlag = true;
        }
        if (condition_id == query_condition::NoCondition
            && game->scenario.aiEnabled)
            return false;
        if (condition_id == query_condition::NoCondition && !game->ready)
            return true;
        if (condition_id == query_condition::SnapshotLoaded
            && game->snapshotLoadSucceeded)
            return true;
    }
    if (sprite_id == query_sprite::ScenarioProgress
        && game->scenario.scenarioStepCompare > condition_id)
        return true;
    if (sprite_id == query_sprite::StationDirection) {
        const int station_direction =
            game->world.cities[game->build.stationDirCursor].stationDirection;
        if (condition_id == query_condition::StationEvenDirection
            && station_direction % 2 == 0)
            return true;
        if ((condition_id == query_condition::StationOddDirectionA
             || condition_id == query_condition::StationOddDirectionB)
            && station_direction % 2 == 1)
            return true;
    }
    if (sprite_id == query_sprite::ServiceSite) {
        const int site_slot = game->serviceVehicleAtCursor;
        const ServiceSite& service_site = game->fleet.serviceSites[site_slot];
        if (condition_id == query_condition::NoCondition
            && service_site.resourceStock[service_resource::Passengers] == 0
            && service_site.resourceStock[service_resource::IndustryGroup1] == 0
            && service_site.resourceStock[service_resource::IndustryGroup2] == 0
            && service_site.resourceStock[service_resource::IndustryGroup3] == 0)
            return true;
        if (condition_id >= query_condition::FirstStock
            && condition_id <= query_condition::LastStock
            && service_site.resourceStock[condition_id - query_condition::FirstStock]
                   > 0)
            return true;
        if (condition_id == query_condition::NoUnloadFlags
            && service_site.unloadFlags == 0)
            return true;
        if (condition_id >= query_condition::FirstUnloadFlag
            && condition_id <= query_condition::LastUnloadFlag) {
            const uint8_t unload_flag = kUnloadConditionFlags[
                condition_id - query_condition::FirstUnloadFlag];
            if ((service_site.unloadFlags & unload_flag) != 0) return true;
        }
    }
    if (sprite_id == query_sprite::EraAvailability
        && condition_id <= game->scenario.currentEra)
        return true;
    if (sprite_id == query_sprite::TrainInfo) {
        if (condition_id < query_condition::TrainRunningState
            && game->fleet.count_cars(game->trainAtCursor) > condition_id)
            return true;
        const TrainCar& train = game->fleet.trainCars[game->trainAtCursor];
        if (condition_id == query_condition::TrainRunningState
            && train.stateCode == query_condition::TrainStateCodeForCondition)
            return true;
        if (condition_id == query_condition::TrainStoppedState
            && train.stateCode != query_condition::TrainStateCodeForCondition)
            return true;
        if (condition_id == query_condition::TrainHasAge
            && train.ageYears > 0)
            return true;
    }
    return sprite_id == query_sprite::MenuRows
        && (condition_id != query_condition::NoCondition
                ? condition_id <= game->ui.menuRowEnabledCount
                              : game->ui.menuRowEnabledCount == 0);
}
