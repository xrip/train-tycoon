// Map and game-state queries.
#include "game_methods.h"
#include <stdlib.h> // abs

// int computeKeyRepeatDelay()
int32_t gc_computeKeyRepeatDelay(GameCanvas *gc) {
    if (gc->keySubState < 30) gc->keySubState += gc->keyRepeatStep;
    gc->keySubState = (int8_t)gc->keySubState;
    return gc->keySubState / 3;
}

// void pushMoneyHistory(int n)
void gc_pushMoneyHistory(GameCanvas *gc, int32_t n) {
    gc->moneyHistory[2] = gc->moneyHistory[1];
    gc->moneyHistory[1] = gc->moneyHistory[0];
    gc->moneyHistory[0] = n;
    gc->moneyTickerFrames[2] = gc->moneyTickerFrames[1];
    gc->moneyTickerFrames[1] = gc->moneyTickerFrames[0];
    gc->moneyTickerFrames[0] = 80;
}

// boolean isTrackStepInlined(int n, int n2, int n3, int n4, int n5)
bool gc_isTrackStepInlined(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3,
                          int32_t n4, int32_t n5) {
    bool bl = false;
    while ((n += n3) >= 0 && (n2 += n4) >= 0 && n < gc->world.mapWidth && n2 < gc->world.mapHeight) {
        if (gc->world.tileMap[n][n2] == n5) bl = true;
        if (gc->world.tileMap[n][n2] == 0 && gc->world.trackMap[n][n2] == 0) continue;
        break;
    }
    return bl;
}

// int neighboursMask(int n, int n2)
int32_t gc_neighboursMask(GameCanvas *gc, int32_t n, int32_t n2) {
    if (n == 34) {
        if (gc->yearGraphMode == 0 || gc->yearGraphMode == 2) {
            if (n2 == 65) return 65;
            if (n2 >= 20) return 64 + gc->yearGraphValues[n2 - 20];
            return 65 + gc->yearGraphValues[n2];
        }
        if (n2 == 65) return 35;
        if (n2 >= 20) return 34 + gc->yearGraphValues[n2 - 20];
        return 35 + gc->yearGraphValues[n2];
    }
    return 0;
}

// boolean isAdjacentTileEmpty(int n, int n2, int n3, boolean bl)  (recursive smooth)
bool gc_isAdjacentTileEmpty(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3, bool bl) {
    bool bl2 = true;
    if (n > 0) {
        if (gc->world.tileMap[n - 1][n2] >= tile::BUILT_FIRST) bl2 = false;
        if (n2 > 0 && gc->world.tileMap[n - 1][n2 - 1] >= tile::BUILT_FIRST) bl2 = false;
    }
    if (n2 > 0 && gc->world.tileMap[n][n2 - 1] >= tile::BUILT_FIRST) bl2 = false;
    if (gc->world.tileMap[n][n2] >= tile::BUILT_FIRST) bl2 = false;
    int32_t n4 = n - gc->selectionStartX;
    int32_t n5 = n2 - gc->selectionStartY;
    if (bl2) {
        gc->terrainEditBuffer[n4][n5] = (int8_t)n3;
        int step = bl ? 1 : -1;
        if (n4 > 0 && (bl ? gc->terrainEditBuffer[n4 - 1][n5] < n3 - 1
                          : gc->terrainEditBuffer[n4 - 1][n5] > n3 + 1)
            && !gc_isAdjacentTileEmpty(gc, n - 1, n2, n3 - step, bl)) bl2 = false;
        if (n5 > 0 && (bl ? gc->terrainEditBuffer[n4][n5 - 1] < n3 - 1
                          : gc->terrainEditBuffer[n4][n5 - 1] > n3 + 1)
            && !gc_isAdjacentTileEmpty(gc, n, n2 - 1, n3 - step, bl)) bl2 = false;
        if (n4 < 9 && (bl ? gc->terrainEditBuffer[n4 + 1][n5] < n3 - 1
                          : gc->terrainEditBuffer[n4 + 1][n5] > n3 + 1)
            && !gc_isAdjacentTileEmpty(gc, n + 1, n2, n3 - step, bl)) bl2 = false;
        if (n5 < 9 && (bl ? gc->terrainEditBuffer[n4][n5 + 1] < n3 - 1
                          : gc->terrainEditBuffer[n4][n5 + 1] > n3 + 1)
            && !gc_isAdjacentTileEmpty(gc, n, n2 + 1, n3 - step, bl)) bl2 = false;
    }
    gc->terrainSmoothCost += gc->costOverhead
        + (n3 == -1 ? gc->costMaintenance : abs(n3) * gc->costCrew);
    return bl2;
}

// boolean isResourceTile(int n, int n2)
bool gc_isResourceTile(GameCanvas *gc, int32_t n, int32_t n2) {
    if (n != 1) gc->snapshotLoadedFlag = false;
    if (n == 1) {
        if (!gc->snapshotLoadedFlag) {
            gc->cH = gc_loadSnapshotAndReturn(gc);
            gc->snapshotLoadedFlag = true;
        }
        if (n2 == 0 && gc->aiEnabled) return false;
        if (n2 == 0 && !gc->ready) return true;
        if (n2 == 1 && gc->cH) return true;
    }
    if (n == 5 && gc->scenarioStepCompare > n2) return true;
    if (n == 21) {
        if (n2 == 0 && gc->stationDirectionCode[gc->stationDirCursor] % 2 == 0) return true;
        if (n2 == 1 && gc->stationDirectionCode[gc->stationDirCursor] % 2 == 1) return true;
        if (n2 == 2 && gc->stationDirectionCode[gc->stationDirCursor] % 2 == 1) return true;
    }
    if (n == 22) {
        int slot = gc->serviceVehicleAtCursor;
        if (n2 == 0 && gc->fleet.serviceResourceStock[slot][0] == 0 && gc->fleet.serviceResourceStock[slot][1] == 0
            && gc->fleet.serviceResourceStock[slot][2] == 0 && gc->fleet.serviceResourceStock[slot][3] == 0) return true;
        if (n2 == 1 && gc->fleet.serviceResourceStock[slot][0] > 0) return true;
        if (n2 == 2 && gc->fleet.serviceResourceStock[slot][1] > 0) return true;
        if (n2 == 3 && gc->fleet.serviceResourceStock[slot][2] > 0) return true;
        if (n2 == 4 && gc->fleet.serviceResourceStock[slot][3] > 0) return true;
        if (n2 == 5 && gc->fleet.serviceResourceStock[slot][4] > 0) return true;
        if (n2 == 6 && gc->fleet.serviceUnloadFlags[slot] == 0) return true;
        if (n2 == 7 && (gc->fleet.serviceUnloadFlags[slot] & 1) == 1) return true;
        if (n2 == 8 && (gc->fleet.serviceUnloadFlags[slot] & 2) == 2) return true;
        if (n2 == 9 && (gc->fleet.serviceUnloadFlags[slot] & 4) == 4) return true;
        if (n2 == 10 && (gc->fleet.serviceUnloadFlags[slot] & 8) == 8) return true;
        if (n2 == 11 && (gc->fleet.serviceUnloadFlags[slot] & 0x10) == 16) return true;
    }
    if (n == 24 && n2 <= gc->era) return true;
    if (n == 26) {
        if (n2 < 6 && gc_countCarsInTrain(gc, gc->trainAtCursor) > n2) return true;
        if (n2 == 6 && gc->fleet.trainStateCode[gc->trainAtCursor] == 7) return true;
        if (n2 == 7 && gc->fleet.trainStateCode[gc->trainAtCursor] != 7) return true;
        if (n2 == 8 && gc->fleet.trainAgeOrTimer[gc->trainAtCursor] > 0) return true;
    }
    return n == 60 && (n2 != 0 ? n2 <= gc->menuRowEnabledCount : gc->menuRowEnabledCount == 0);
}
