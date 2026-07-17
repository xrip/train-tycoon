// Map and game-state queries.
#include "game_internal.h"
#include <stdlib.h> // abs

int32_t UiController::advance_key_repeat() {
    if (keySubState < 30) keySubState += keyRepeatStep;
    keySubState = (int8_t)keySubState;
    return keySubState / 3;
}

void game_pushMoneyHistory(Game *game, int32_t n) {
    game->economy.push_money_history(n);
    game->ui.moneyTickerFrames[2] = game->ui.moneyTickerFrames[1];
    game->ui.moneyTickerFrames[1] = game->ui.moneyTickerFrames[0];
    game->ui.moneyTickerFrames[0] = 80;
}

void Economy::push_money_history(int32_t amount) {
    moneyHistory[2] = moneyHistory[1];
    moneyHistory[1] = moneyHistory[0];
    moneyHistory[0] = amount;
}

bool World::has_track_step(int32_t n, int32_t n2, int32_t n3,
                           int32_t n4, int32_t n5) const {
    bool bl = false;
    while ((n += n3) >= 0 && (n2 += n4) >= 0
           && n < terrain.mapWidth && n2 < terrain.mapHeight) {
        if (terrain.tileMap[n][n2] == n5) bl = true;
        if (terrain.tileMap[n][n2] == 0 && trackMap[n][n2] == 0) continue;
        break;
    }
    return bl;
}

int32_t UiController::report_graph_value(int32_t sprite_id,
                                         int32_t index) const {
    if (sprite_id == 34) {
        if (yearGraphMode == 0 || yearGraphMode == 2) {
            if (index == 65) return 65;
            if (index >= 20) return 64 + yearGraphValues[index - 20];
            return 65 + yearGraphValues[index];
        }
        if (index == 65) return 35;
        if (index >= 20) return 34 + yearGraphValues[index - 20];
        return 35 + yearGraphValues[index];
    }
    return 0;
}

bool BuildController::adjust_terrain_point(const Terrain& terrain,
                                           const Economy& economy,
                                           int32_t n, int32_t n2,
                                           int32_t n3, bool bl) {
    bool bl2 = true;
    if (n > 0) {
        if (terrain.tileMap[n - 1][n2] >= tile::BUILT_FIRST) bl2 = false;
        if (n2 > 0 && terrain.tileMap[n - 1][n2 - 1] >= tile::BUILT_FIRST) bl2 = false;
    }
    if (n2 > 0 && terrain.tileMap[n][n2 - 1] >= tile::BUILT_FIRST) bl2 = false;
    if (terrain.tileMap[n][n2] >= tile::BUILT_FIRST) bl2 = false;
    int32_t n4 = n - selectionStartX;
    int32_t n5 = n2 - selectionStartY;
    if (bl2) {
        terrainEditBuffer[n4][n5] = (int8_t)n3;
        int step = bl ? 1 : -1;
        if (n4 > 0 && (bl ? terrainEditBuffer[n4 - 1][n5] < n3 - 1
                          : terrainEditBuffer[n4 - 1][n5] > n3 + 1)
            && !adjust_terrain_point(terrain, economy, n - 1, n2,
                                     n3 - step, bl)) bl2 = false;
        if (n5 > 0 && (bl ? terrainEditBuffer[n4][n5 - 1] < n3 - 1
                          : terrainEditBuffer[n4][n5 - 1] > n3 + 1)
            && !adjust_terrain_point(terrain, economy, n, n2 - 1,
                                     n3 - step, bl)) bl2 = false;
        if (n4 < 9 && (bl ? terrainEditBuffer[n4 + 1][n5] < n3 - 1
                          : terrainEditBuffer[n4 + 1][n5] > n3 + 1)
            && !adjust_terrain_point(terrain, economy, n + 1, n2,
                                     n3 - step, bl)) bl2 = false;
        if (n5 < 9 && (bl ? terrainEditBuffer[n4][n5 + 1] < n3 - 1
                          : terrainEditBuffer[n4][n5 + 1] > n3 + 1)
            && !adjust_terrain_point(terrain, economy, n, n2 + 1,
                                     n3 - step, bl)) bl2 = false;
    }
    terrainSmoothCost += economy.costOverhead
        + (n3 == -1 ? economy.costMaintenance : abs(n3) * economy.costCrew);
    return bl2;
}

bool game_isResourceTile(Game *game, int32_t n, int32_t n2) {
    if (n != 1) game->ui.snapshotLoadedFlag = false;
    if (n == 1) {
        if (!game->ui.snapshotLoadedFlag) {
            game->snapshotLoadSucceeded = game->saveCodec.load_main_snapshot();
            game->ui.snapshotLoadedFlag = true;
        }
        if (n2 == 0 && game->scenario.aiEnabled) return false;
        if (n2 == 0 && !game->ready) return true;
        if (n2 == 1 && game->snapshotLoadSucceeded) return true;
    }
    if (n == 5 && game->scenario.scenarioStepCompare > n2) return true;
    if (n == 21) {
        if (n2 == 0 && game->world.cities[game->build.stationDirCursor].stationDirection % 2 == 0) return true;
        if (n2 == 1 && game->world.cities[game->build.stationDirCursor].stationDirection % 2 == 1) return true;
        if (n2 == 2 && game->world.cities[game->build.stationDirCursor].stationDirection % 2 == 1) return true;
    }
    if (n == 22) {
        int slot = game->serviceVehicleAtCursor;
        if (n2 == 0 && game->fleet.serviceSites[slot].resourceStock[0] == 0 && game->fleet.serviceSites[slot].resourceStock[1] == 0
            && game->fleet.serviceSites[slot].resourceStock[2] == 0 && game->fleet.serviceSites[slot].resourceStock[3] == 0) return true;
        if (n2 == 1 && game->fleet.serviceSites[slot].resourceStock[0] > 0) return true;
        if (n2 == 2 && game->fleet.serviceSites[slot].resourceStock[1] > 0) return true;
        if (n2 == 3 && game->fleet.serviceSites[slot].resourceStock[2] > 0) return true;
        if (n2 == 4 && game->fleet.serviceSites[slot].resourceStock[3] > 0) return true;
        if (n2 == 5 && game->fleet.serviceSites[slot].resourceStock[4] > 0) return true;
        if (n2 == 6 && game->fleet.serviceSites[slot].unloadFlags == 0) return true;
        if (n2 == 7 && (game->fleet.serviceSites[slot].unloadFlags & 1) == 1) return true;
        if (n2 == 8 && (game->fleet.serviceSites[slot].unloadFlags & 2) == 2) return true;
        if (n2 == 9 && (game->fleet.serviceSites[slot].unloadFlags & 4) == 4) return true;
        if (n2 == 10 && (game->fleet.serviceSites[slot].unloadFlags & 8) == 8) return true;
        if (n2 == 11 && (game->fleet.serviceSites[slot].unloadFlags & 0x10) == 16) return true;
    }
    if (n == 24 && n2 <= game->scenario.currentEra) return true;
    if (n == 26) {
        if (n2 < 6 && game->fleet.count_cars(game->trainAtCursor) > n2) return true;
        if (n2 == 6 && game->fleet.trainCars[game->trainAtCursor].stateCode == 7) return true;
        if (n2 == 7 && game->fleet.trainCars[game->trainAtCursor].stateCode != 7) return true;
        if (n2 == 8 && game->fleet.trainCars[game->trainAtCursor].ageYears > 0) return true;
    }
    return n == 60 && (n2 != 0 ? n2 <= game->ui.menuRowEnabledCount : game->ui.menuRowEnabledCount == 0);
}
