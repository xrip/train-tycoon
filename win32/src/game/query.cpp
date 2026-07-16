// Map and game-state queries.
#include "game_internal.h"
#include <stdlib.h> // abs

int32_t game_computeKeyRepeatDelay(Game *game) {
    if (game->ui.keySubState < 30) game->ui.keySubState += game->ui.keyRepeatStep;
    game->ui.keySubState = (int8_t)game->ui.keySubState;
    return game->ui.keySubState / 3;
}

void game_pushMoneyHistory(Game *game, int32_t n) {
    game->economy.moneyHistory[2] = game->economy.moneyHistory[1];
    game->economy.moneyHistory[1] = game->economy.moneyHistory[0];
    game->economy.moneyHistory[0] = n;
    game->ui.moneyTickerFrames[2] = game->ui.moneyTickerFrames[1];
    game->ui.moneyTickerFrames[1] = game->ui.moneyTickerFrames[0];
    game->ui.moneyTickerFrames[0] = 80;
}

bool game_isTrackStepInlined(Game *game, int32_t n, int32_t n2, int32_t n3,
                          int32_t n4, int32_t n5) {
    bool bl = false;
    while ((n += n3) >= 0 && (n2 += n4) >= 0 && n < game->world.mapWidth && n2 < game->world.mapHeight) {
        if (game->world.tileMap[n][n2] == n5) bl = true;
        if (game->world.tileMap[n][n2] == 0 && game->world.trackMap[n][n2] == 0) continue;
        break;
    }
    return bl;
}

int32_t game_neighboursMask(Game *game, int32_t n, int32_t n2) {
    if (n == 34) {
        if (game->ui.yearGraphMode == 0 || game->ui.yearGraphMode == 2) {
            if (n2 == 65) return 65;
            if (n2 >= 20) return 64 + game->ui.yearGraphValues[n2 - 20];
            return 65 + game->ui.yearGraphValues[n2];
        }
        if (n2 == 65) return 35;
        if (n2 >= 20) return 34 + game->ui.yearGraphValues[n2 - 20];
        return 35 + game->ui.yearGraphValues[n2];
    }
    return 0;
}

bool game_isAdjacentTileEmpty(Game *game, int32_t n, int32_t n2, int32_t n3, bool bl) {
    bool bl2 = true;
    if (n > 0) {
        if (game->world.tileMap[n - 1][n2] >= tile::BUILT_FIRST) bl2 = false;
        if (n2 > 0 && game->world.tileMap[n - 1][n2 - 1] >= tile::BUILT_FIRST) bl2 = false;
    }
    if (n2 > 0 && game->world.tileMap[n][n2 - 1] >= tile::BUILT_FIRST) bl2 = false;
    if (game->world.tileMap[n][n2] >= tile::BUILT_FIRST) bl2 = false;
    int32_t n4 = n - game->build.selectionStartX;
    int32_t n5 = n2 - game->build.selectionStartY;
    if (bl2) {
        game->build.terrainEditBuffer[n4][n5] = (int8_t)n3;
        int step = bl ? 1 : -1;
        if (n4 > 0 && (bl ? game->build.terrainEditBuffer[n4 - 1][n5] < n3 - 1
                          : game->build.terrainEditBuffer[n4 - 1][n5] > n3 + 1)
            && !game_isAdjacentTileEmpty(game, n - 1, n2, n3 - step, bl)) bl2 = false;
        if (n5 > 0 && (bl ? game->build.terrainEditBuffer[n4][n5 - 1] < n3 - 1
                          : game->build.terrainEditBuffer[n4][n5 - 1] > n3 + 1)
            && !game_isAdjacentTileEmpty(game, n, n2 - 1, n3 - step, bl)) bl2 = false;
        if (n4 < 9 && (bl ? game->build.terrainEditBuffer[n4 + 1][n5] < n3 - 1
                          : game->build.terrainEditBuffer[n4 + 1][n5] > n3 + 1)
            && !game_isAdjacentTileEmpty(game, n + 1, n2, n3 - step, bl)) bl2 = false;
        if (n5 < 9 && (bl ? game->build.terrainEditBuffer[n4][n5 + 1] < n3 - 1
                          : game->build.terrainEditBuffer[n4][n5 + 1] > n3 + 1)
            && !game_isAdjacentTileEmpty(game, n, n2 + 1, n3 - step, bl)) bl2 = false;
    }
    game->build.terrainSmoothCost += game->economy.costOverhead
        + (n3 == -1 ? game->economy.costMaintenance : abs(n3) * game->economy.costCrew);
    return bl2;
}

bool game_isResourceTile(Game *game, int32_t n, int32_t n2) {
    if (n != 1) game->ui.snapshotLoadedFlag = false;
    if (n == 1) {
        if (!game->ui.snapshotLoadedFlag) {
            game->snapshotLoadSucceeded = game_loadSnapshotAndReturn(game);
            game->ui.snapshotLoadedFlag = true;
        }
        if (n2 == 0 && game->scenario.aiEnabled) return false;
        if (n2 == 0 && !game->ready) return true;
        if (n2 == 1 && game->snapshotLoadSucceeded) return true;
    }
    if (n == 5 && game->scenario.scenarioStepCompare > n2) return true;
    if (n == 21) {
        if (n2 == 0 && game->world.stationDirectionCode[game->build.stationDirCursor] % 2 == 0) return true;
        if (n2 == 1 && game->world.stationDirectionCode[game->build.stationDirCursor] % 2 == 1) return true;
        if (n2 == 2 && game->world.stationDirectionCode[game->build.stationDirCursor] % 2 == 1) return true;
    }
    if (n == 22) {
        int slot = game->serviceVehicleAtCursor;
        if (n2 == 0 && game->fleet.serviceResourceStock[slot][0] == 0 && game->fleet.serviceResourceStock[slot][1] == 0
            && game->fleet.serviceResourceStock[slot][2] == 0 && game->fleet.serviceResourceStock[slot][3] == 0) return true;
        if (n2 == 1 && game->fleet.serviceResourceStock[slot][0] > 0) return true;
        if (n2 == 2 && game->fleet.serviceResourceStock[slot][1] > 0) return true;
        if (n2 == 3 && game->fleet.serviceResourceStock[slot][2] > 0) return true;
        if (n2 == 4 && game->fleet.serviceResourceStock[slot][3] > 0) return true;
        if (n2 == 5 && game->fleet.serviceResourceStock[slot][4] > 0) return true;
        if (n2 == 6 && game->fleet.serviceUnloadFlags[slot] == 0) return true;
        if (n2 == 7 && (game->fleet.serviceUnloadFlags[slot] & 1) == 1) return true;
        if (n2 == 8 && (game->fleet.serviceUnloadFlags[slot] & 2) == 2) return true;
        if (n2 == 9 && (game->fleet.serviceUnloadFlags[slot] & 4) == 4) return true;
        if (n2 == 10 && (game->fleet.serviceUnloadFlags[slot] & 8) == 8) return true;
        if (n2 == 11 && (game->fleet.serviceUnloadFlags[slot] & 0x10) == 16) return true;
    }
    if (n == 24 && n2 <= game->era) return true;
    if (n == 26) {
        if (n2 < 6 && game_countCarsInTrain(game, game->trainAtCursor) > n2) return true;
        if (n2 == 6 && game->fleet.trainStateCode[game->trainAtCursor] == 7) return true;
        if (n2 == 7 && game->fleet.trainStateCode[game->trainAtCursor] != 7) return true;
        if (n2 == 8 && game->fleet.trainAgeOrTimer[game->trainAtCursor] > 0) return true;
    }
    return n == 60 && (n2 != 0 ? n2 <= game->ui.menuRowEnabledCount : game->ui.menuRowEnabledCount == 0);
}
