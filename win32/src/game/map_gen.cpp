// Terrain and map generation.
// Driven by loadState25Setup across mapGenStage 0..10 (raise/lower terrain,
// place patches and features, scatter industry). The random process is exact.
#include "game_methods.h"
#include <stdlib.h> // abs
#include <vector>

static int nextRngInt(GameCanvas *gc) { return gc->rng.next_int(); }

// GameCanvas::mapGenStage steps through the incremental generator in
// gc_loadState25Setup: init, raise then lower terrain, place patches, six
// terrain-feature passes, then scatter starting obstacles.
namespace mapgen {
inline constexpr int32_t STAGE_INIT = 0;
inline constexpr int32_t STAGE_RAISE_TERRAIN = 1;
inline constexpr int32_t STAGE_LOWER_TERRAIN = 2;
inline constexpr int32_t STAGE_PLACE_PATCHES = 3;
inline constexpr int32_t STAGE_FEATURE_0 = 4;
inline constexpr int32_t STAGE_FEATURE_1 = 5;
inline constexpr int32_t STAGE_FEATURE_2 = 6;
inline constexpr int32_t STAGE_FEATURE_3 = 7;
inline constexpr int32_t STAGE_FEATURE_4 = 8;
inline constexpr int32_t STAGE_FEATURE_5 = 9;
inline constexpr int32_t STAGE_SCATTER_OBSTACLES = 10;
}  // namespace mapgen

// byte randomCityTile()
int8_t gc_randomCityTile(GameCanvas *gc) {
    int n = abs(nextRngInt(gc) % 22);
    if (n < 6) return 34;
    if (n < 11) return 35;
    if (n < 15) return 36;
    if (n < 18) return 37;
    if (n < 20) return 38;
    if (n < 21) return 39;
    if (n < 22) return 40;
    return 0;
}

// byte addCityEntry(int n)
int8_t gc_addCityEntry(GameCanvas *gc, int32_t n) {
    gc->stationDirectionCode[gc->cityListIndex] = (int8_t)n;
    gc->cityPopulation[gc->cityListIndex] = (int8_t)(50 + nextRngInt(gc) % 20);
    gc->cityIndex[gc->cityListIndex] = 0;
    gc->cityListIndex = (int8_t)(gc->cityListIndex + 1);
    return (int8_t)(gc->cityListIndex - 1);
}

// byte registerTrain(byte by, int n, int n2)
int8_t gc_registerTrain(GameCanvas *gc, int8_t by, int32_t n, int32_t n2) {
    gc->constructionTimers[gc->numSelectedTrains] = 50;
    gc->zonePopulation[gc->numSelectedTrains] = by;
    gc->trainSelectionY[gc->numSelectedTrains] = (int16_t)n;
    gc->trainSelectionX[gc->numSelectedTrains] = (int16_t)n2;
    int16_t s = 0;
    bool bl;
    do {
        int8_t by2 = (int8_t)abs(nextRngInt(gc) % 6);
        int8_t by3 = (int8_t)abs(nextRngInt(gc) % 14);
        s = (int16_t)(256 * by2 + by3);
        bl = true;
        for (int i = 0; i < gc->numSelectedTrains; ++i)
            if (s == gc->trainSelectionIndex[i]) bl = false;
    } while (!bl);
    gc->trainSelectionIndex[gc->numSelectedTrains] = s;
    gc->numSelectedTrains = (int8_t)(gc->numSelectedTrains + 1);
    return (int8_t)(gc->numSelectedTrains - 1);
}

// void toggleFlagInlined(boolean bl) -- recompute slope tiles from heightMap
void gc_toggleFlagInlined(GameCanvas *gc, bool bl) {
    int n, n2, n3, n4;
    if (bl) {
        gc_commitTerrainEdit(gc);
        gc->terrainEditBuffer.clear();
        n4 = gc->selectionStartX; n3 = gc->selectionStartY;
        n2 = gc->selectionStartX + 10; n = gc->selectionStartY + 10;
    } else {
        n4 = 0; n3 = 0; n2 = gc->world.mapWidth; n = gc->world.mapHeight;
    }
    for (int i = n4; i < n2; ++i) {
        for (int j = n3; j < n; ++j) {
            int8_t by = gc->world.heightMap[i][j];
            int8_t by2 = gc->world.heightMap[i][j + 1];
            int8_t by3 = gc->world.heightMap[i + 1][j];
            int8_t by4 = gc->world.heightMap[i + 1][j + 1];
            if (gc->world.tileMap[i][j] >= tile::BUILT_FIRST) continue;
            if (by == by2 && by == by3 && by == by4) {
                gc->world.tileElevation[i][j] = by;
                if (gc->world.tileMap[i][j] <= 28) gc->world.tileMap[i][j] = 0;
            } else if (by == by2 && by == by3 && by4 < by) {
                gc->world.tileElevation[i][j] = by; gc->world.tileMap[i][j] = 11;
            }
            if (by == by3 && by == by4 && by2 < by) { gc->world.tileElevation[i][j] = by; gc->world.tileMap[i][j] = 13; }
            if (by == by2 && by == by4 && by3 < by) { gc->world.tileElevation[i][j] = by; gc->world.tileMap[i][j] = 14; }
            if (by2 == by3 && by2 == by4 && by < by2) { gc->world.tileElevation[i][j] = by2; gc->world.tileMap[i][j] = 12; }
            if (by == by4 && by2 == by3 && by2 < by) { gc->world.tileElevation[i][j] = by; gc->world.tileMap[i][j] = 16; }
            if (by == by4 && by2 == by3 && by < by2) { gc->world.tileElevation[i][j] = by2; gc->world.tileMap[i][j] = 15; }
            if (by == by2 && by3 == by4 && by3 < by) {
                gc->world.tileElevation[i][j] = by;
                gc->world.tileMap[i][j] = (int8_t)(by3 == -1 && i + 2 <= gc->world.mapWidth && (gc->world.heightMap[i + 2][j] == -1 || gc->world.heightMap[i + 2][j + 1] == -1) ? 9 : 24);
            }
            if (by == by2 && by3 == by4 && by < by3) {
                gc->world.tileElevation[i][j] = by3;
                gc->world.tileMap[i][j] = (int8_t)(by == -1 && i - 1 >= 0 && (gc->world.heightMap[i - 1][j] == -1 || gc->world.heightMap[i - 1][j + 1] == -1) ? 8 : 22);
            }
            if (by == by3 && by2 == by4 && by2 < by) {
                gc->world.tileElevation[i][j] = by;
                gc->world.tileMap[i][j] = (int8_t)(by2 == -1 && j + 2 <= gc->world.mapHeight && (gc->world.heightMap[i][j + 2] == -1 || gc->world.heightMap[i + 1][j + 2] == -1) ? 10 : 21);
            }
            if (by == by3 && by2 == by4 && by < by2) {
                gc->world.tileElevation[i][j] = by2;
                gc->world.tileMap[i][j] = (int8_t)(by == -1 && j - 1 >= 0 && (gc->world.heightMap[i][j - 1] == -1 || gc->world.heightMap[i + 1][j - 1] == -1) ? 7 : 23);
            }
            if (by == by2 && by == by3 && by < by4) { gc->world.tileElevation[i][j] = by; gc->world.tileMap[i][j] = (int8_t)(by == -1 ? 3 : 19); }
            if (by == by2 && by == by4 && by < by3) { gc->world.tileElevation[i][j] = by; gc->world.tileMap[i][j] = (int8_t)(by == -1 ? 6 : 18); }
            if (by == by3 && by == by4 && by < by2) { gc->world.tileElevation[i][j] = by; gc->world.tileMap[i][j] = (int8_t)(by == -1 ? 5 : 20); }
            if (by2 == by3 && by2 == by4 && by4 < by) { gc->world.tileElevation[i][j] = by4; gc->world.tileMap[i][j] = (int8_t)(by4 == -1 ? 4 : 17); }
            if (by == by4 && by2 < by && by3 > by) { gc->world.tileElevation[i][j] = by; gc->world.tileMap[i][j] = 25; }
            if (by3 == by2 && by < by3 && by4 > by3) { gc->world.tileElevation[i][j] = by3; gc->world.tileMap[i][j] = 26; }
            if (by == by4 && by2 > by && by3 < by) { gc->world.tileElevation[i][j] = by; gc->world.tileMap[i][j] = 27; }
            if (by3 != by2 || by <= by3 || by4 >= by3) continue;
            gc->world.tileElevation[i][j] = by3; gc->world.tileMap[i][j] = 28;
        }
    }
}

// void smoothTileAtInlined(int n, int n2, boolean bl)
void gc_smoothTileAtInlined(GameCanvas *gc, int32_t n, int32_t n2, bool bl) {
    if ((bl && gc->world.heightMap[n][n2] < 3) || (!bl && gc->world.heightMap[n][n2] > -1)) {
        gc_setSelectionStart(gc, n, n2);
        if (gc_isAdjacentTileEmpty(gc, n, n2, gc->world.heightMap[n][n2] + (bl ? 1 : -1), bl)) {
            if (gc->initialIndustryPlaced) {
                if (gc->terrainSmoothCost <= gc->money) {
                    gc->money -= gc->terrainSmoothCost;
                    gc_pushMoneyHistory(gc, -gc->terrainSmoothCost);
                    gc->subsidyCost += gc->terrainSmoothCost;
                    gc_toggleFlagInlined(gc, true);
                } else {
                    gc_enterOutOfMoneyState(gc);
                }
            } else {
                gc_toggleFlagInlined(gc, true);
            }
        }
    }
    gc->lastViewportCenterX = -1;
}

// void raiseTerrainArea(int n, int n2, int n3, int n4, int n5)
void gc_raiseTerrainArea(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5) {
    for (int i = n; i < n + n3; ++i) {
        for (int j = n2; j < n2 + n4; ++j) {
            if (i <= 0 || j <= 0 || i > gc->world.mapWidth || j > gc->world.mapHeight) continue;
            gc_setSelectionStart(gc, i, j);
            gc_isAdjacentTileEmpty(gc, i, j, n5, true);
            if (nextRngInt(gc) % 2 < 0 && i > 0) gc_isAdjacentTileEmpty(gc, i - 1, j, n5, true);
            if (nextRngInt(gc) % 2 < 0 && j > 0) gc_isAdjacentTileEmpty(gc, i, j - 1, n5, true);
            if (nextRngInt(gc) % 2 < 0 && i < gc->world.mapWidth) gc_isAdjacentTileEmpty(gc, i + 1, j, n5, true);
            if (nextRngInt(gc) % 2 < 0 && j < gc->world.mapHeight) gc_isAdjacentTileEmpty(gc, i, j + 1, n5, true);
            gc_commitTerrainEdit(gc);
        }
    }
}

// void lowerTerrainArea(int n, int n2, int n3, int n4, int n5)
void gc_lowerTerrainArea(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5) {
    (void)n5;
    for (int i = n; i < n + n3; ++i) {
        for (int j = n2; j < n2 + n4; ++j) {
            if (i <= 0 || j <= 0 || i > gc->world.mapWidth || j > gc->world.mapHeight) continue;
            for (int k = 0; k < 3; ++k) gc_smoothTileAtInlined(gc, i, j, false);
            if (nextRngInt(gc) % 2 < 0 && i > 0) gc_smoothTileAtInlined(gc, i - 1, j, false);
            if (nextRngInt(gc) % 2 < 0 && j > 0) gc_smoothTileAtInlined(gc, i, j - 1, false);
            if (nextRngInt(gc) % 2 < 0 && i < gc->world.mapWidth) gc_smoothTileAtInlined(gc, i + 1, j, false);
            if (nextRngInt(gc) % 2 >= 0 || j >= gc->world.mapHeight) continue;
            gc_smoothTileAtInlined(gc, i, j + 1, false);
        }
    }
}

// void trackBuildStepInlined(int n, int n2, int n3, int n4)
void gc_trackBuildStepInlined(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4) {
    int8_t by = -2;
    for (int i = n; i < n + n3; ++i) {
        for (int j = n2; j < n2 + n4; ++j) {
            if (i >= gc->world.mapWidth || j >= gc->world.mapHeight) continue;
            if (by == -2 && (by = gc->world.heightMap[i][j]) < 0) by = 0;
            while (by != gc->world.heightMap[i][j]) gc_smoothTileAtInlined(gc, i, j, by > gc->world.heightMap[i][j]);
            while (by != gc->world.heightMap[i + 1][j]) gc_smoothTileAtInlined(gc, i + 1, j, by > gc->world.heightMap[i + 1][j]);
            while (by != gc->world.heightMap[i][j + 1]) gc_smoothTileAtInlined(gc, i, j + 1, by > gc->world.heightMap[i][j + 1]);
            while (by != gc->world.heightMap[i + 1][j + 1]) gc_smoothTileAtInlined(gc, i + 1, j + 1, by > gc->world.heightMap[i + 1][j + 1]);
        }
    }
}

// void placeTerrainPatch(int n, int n2)
void gc_placeTerrainPatch(GameCanvas *gc, int32_t n, int32_t n2) {
    int n3, n4, n5, n6;
    int8_t by, by2, by3, by4;
    int n7 = abs(nextRngInt(gc) % 5) + gc->terrainPatchBaseSize;
    int8_t by5 = (int8_t)(abs(nextRngInt(gc) % 5) + gc->terrainPatchBaseSize);
    std::vector<std::vector<int8_t>> patch_tiles(
        static_cast<size_t>(n7),
        std::vector<int8_t>(static_cast<size_t>(by5)));
    bool bl = false;
    int n8 = abs(nextRngInt(gc) % (n7 - 2)) + 1;
    for (n6 = abs(nextRngInt(gc) % 3); n6 < by5 - abs(nextRngInt(gc) % 3); ++n6) patch_tiles[n8][n6] = 1;
    if (n7 >= 7) {
        do {
            n8 = abs(nextRngInt(gc) % (n7 - 2)) + 1;
            bl = false;
            for (n6 = 0; n6 < by5; ++n6)
                if (patch_tiles[n8 - 1][n6] == 1 || patch_tiles[n8 + 1][n6] == 1) bl = true;
        } while (bl);
        for (n6 = abs(nextRngInt(gc) % 3); n6 < by5 - abs(nextRngInt(gc) % 3); ++n6) patch_tiles[n8][n6] = 1;
    }
    n6 = abs(nextRngInt(gc) % (by5 - 2)) + 1;
    for (n5 = abs(nextRngInt(gc) % 3); n5 < n7 - abs(nextRngInt(gc) % 3); ++n5) patch_tiles[n5][n6] = 2;
    if (by5 >= 7) {
        do {
            n6 = abs(nextRngInt(gc) % (by5 - 2)) + 1;
            bl = false;
            for (n5 = 0; n5 < n7; ++n5)
                if (patch_tiles[n5][n6 - 1] == 2 || patch_tiles[n5][n6 + 1] == 2) bl = true;
        } while (bl);
    }
    for (n5 = 0; n5 < n7; ++n5) {
        for (by4 = 0; by4 < by5; ++by4) {
            if (patch_tiles[n5][by4] != 1 && patch_tiles[n5][by4] != 2) continue;
            by3 = (int8_t)(n5 > 0 && patch_tiles[n5 - 1][by4] > 0 ? 1 : 0);
            by2 = (int8_t)(n5 < n7 - 1 && patch_tiles[n5 + 1][by4] > 0 ? 1 : 0);
            by = (int8_t)(by4 < by5 - 1 && patch_tiles[n5][by4 + 1] > 0 ? 1 : 0);
            n4 = by4 > 0 && patch_tiles[n5][by4 - 1] > 0 ? 1 : 0;
            n3 = (by3 != 0 ? 1 : 0) + (by2 != 0 ? 1 : 0) + (by != 0 ? 1 : 0) + (n4 != 0 ? 1 : 0);
            if (n3 > 2) { patch_tiles[n5][by4] = 44; continue; }
            if (by2 != 0 && n4 != 0) { patch_tiles[n5][by4] = 45; continue; }
            if (by2 != 0 && by != 0) { patch_tiles[n5][by4] = 46; continue; }
            if (by3 != 0 && by != 0) { patch_tiles[n5][by4] = 47; continue; }
            if (by3 != 0 && n4 != 0) { patch_tiles[n5][by4] = 48; continue; }
            if ((by3 != 0 || by2 != 0) && by == 0 && n4 == 0) { patch_tiles[n5][by4] = 43; continue; }
            if ((n4 == 0 && by == 0) || by3 != 0 || by2 != 0) continue;
            patch_tiles[n5][by4] = 42;
        }
    }
    for (n5 = 0; n5 < n7; ++n5) {
        for (by4 = 0; by4 < by5; ++by4) {
            if (patch_tiles[n5][by4] != 0) continue;
            by3 = 1;
            by2 = (int8_t)(n5 > 0 ? patch_tiles[n5 - 1][by4] : 0);
            n4 = n5 < n7 - 1 ? patch_tiles[n5 + 1][by4] : 0;
            by = (int8_t)(by4 > 0 ? patch_tiles[n5][by4 - 1] : 0);
            n3 = by4 < by5 - 1 ? patch_tiles[n5][by4 + 1] : 0;
            if (by2 < 42 && n4 < 42 && by < 42 && n3 < 42) by3 = 0;
            if (by2 == 44 || n4 == 44 || by == 44 || n3 == 44) by3 = 0;
            if (by2 == 45 || by2 == 46 || by2 == 43) by3 = 0;
            if (n4 == 47 || n4 == 48 || n4 == 43) by3 = 0;
            if (by == 46 || by == 47 || by == 42) by3 = 0;
            if (n3 == 45 || n3 == 48 || n3 == 42) by3 = 0;
            if (by2 == 44) patch_tiles[n5][by4] = 43;
            if (n4 == 44) patch_tiles[n5][by4] = 43;
            if (by == 44) patch_tiles[n5][by4] = 42;
            if (n3 == 44) patch_tiles[n5][by4] = 42;
            if (by3 == 0) continue;
            patch_tiles[n5][by4] = gc_randomCityTile(gc);
        }
    }
    n5 = -2;
    for (by4 = 0; by4 < n7; ++by4) {
        for (by3 = 0; by3 < by5; ++by3) {
            if (n + by4 >= gc->world.mapWidth || n2 + by3 >= gc->world.mapHeight
                || (patch_tiles[by4][by3] == 0 && (patch_tiles[by4][by3] != 0 || nextRngInt(gc) % 3 >= 0))) continue;
            if (n5 == -2 && (n5 = gc->world.heightMap[n + by4][n2 + by3]) < 0) n5 = 0;
            int64_t l = midp_current_time_millis();
            while (n5 != gc->world.heightMap[n + by4][n2 + by3]) {
                gc_smoothTileAtInlined(gc, n + by4, n2 + by3, n5 > gc->world.heightMap[n + by4][n2 + by3]);
                if (midp_current_time_millis() - l <= 1000) continue;
                gc->trackBuildInProgress = true; return;
            }
            l = midp_current_time_millis();
            while (n5 != gc->world.heightMap[n + by4 + 1][n2 + by3]) {
                gc_smoothTileAtInlined(gc, n + by4 + 1, n2 + by3, n5 > gc->world.heightMap[n + by4 + 1][n2 + by3]);
                if (midp_current_time_millis() - l <= 1000) continue;
                gc->trackBuildInProgress = true; return;
            }
            l = midp_current_time_millis();
            while (n5 != gc->world.heightMap[n + by4][n2 + by3 + 1]) {
                gc_smoothTileAtInlined(gc, n + by4, n2 + by3 + 1, n5 > gc->world.heightMap[n + by4][n2 + by3 + 1]);
                if (midp_current_time_millis() - l <= 1000) continue;
                gc->trackBuildInProgress = true; return;
            }
            l = midp_current_time_millis();
            while (n5 != gc->world.heightMap[n + by4 + 1][n2 + by3 + 1]) {
                gc_smoothTileAtInlined(gc, n + by4 + 1, n2 + by3 + 1, n5 > gc->world.heightMap[n + by4 + 1][n2 + by3 + 1]);
                if (midp_current_time_millis() - l <= 1000) continue;
                gc->trackBuildInProgress = true; return;
            }
        }
    }
    by4 = 0;
    for (by3 = 0; by3 < n7; ++by3)
        for (int8_t by6 = 0; by6 < by5; ++by6)
            if (!(patch_tiles[by3][by6] > 40 || patch_tiles[by3][by6] < 34)) by4 = (int8_t)(by4 + 1);
    by3 = gc_registerTrain(gc, by4, n + n7 / 2, n2 + by5 / 2);
    for (int i = 0; i < n7; ++i) {
        for (n4 = 0; n4 < by5; ++n4) {
            if (n + i >= gc->world.mapWidth || n2 + n4 >= gc->world.mapHeight || patch_tiles[i][n4] == 0) continue;
            gc->world.tileMap[n + i][n2 + n4] = patch_tiles[i][n4];
            gc->world.trackMap[n + i][n2 + n4] = (int8_t)(0x80 | by3);
        }
    }
}

// void placeTerrainFeature(int n)
void gc_placeTerrainFeature(GameCanvas *gc, int32_t n) {
    int n2, n3, n4, n5, n6, n7;
    bool bl;
    if (gc->trackBuildInProgress) return;
    int64_t l = midp_current_time_millis();
    do {
        bl = true;
        n7 = abs(nextRngInt(gc) % (gc->world.mapWidth - 8)) + 4;
        n6 = abs(nextRngInt(gc) % (gc->world.mapHeight - 8)) + 4;
        n5 = nextRngInt(gc) % 2 <= 0 ? 3 : 2;
        n4 = nextRngInt(gc) % 2 <= 0 ? 3 : 2;
        if (n == 2) { n5 = 2; n4 = 2; }
        for (n3 = n7 - 3; n3 < n7 + n5 + 3; ++n3) {
            for (n2 = n6 - 3; n2 < n6 + n4 + 3; ++n2) {
                if (gc->world.tileMap[n3][n2] >= tile::BUILT_FIRST) bl = false;
                if (gc->world.tileElevation[n3][n2] != -1) continue;
                bl = false;
            }
        }
        if (midp_current_time_millis() - l <= 1500) continue;
        gc->trackBuildInProgress = true;
        break;
    } while (!bl);
    if (gc->trackBuildInProgress) return;
    gc_trackBuildStepInlined(gc, n7, n6, n5, n4);
    n3 = gc_addCityEntry(gc, n);
    switch (n) {
        case 0:
            if (n5 == 2 && n4 == 2) {
                gc_placeTrackTile(gc, n7, n6, 52, n3); gc_placeTrackTile(gc, n7, n6 + 1, 52, n3);
                gc_placeTrackTile(gc, n7 + 1, n6, 51, n3); gc_placeTrackTile(gc, n7 + 1, n6 + 1, 50, n3);
            }
            if ((n5 == 3 && n4 == 2) || (n5 == 3 && n4 == 3)) {
                gc_placeTrackTile(gc, n7, n6, 52, n3); gc_placeTrackTile(gc, n7, n6 + 1, 52, n3);
                gc_placeTrackTile(gc, n7 + 1, n6, 51, n3); gc_placeTrackTile(gc, n7 + 2, n6, 51, n3);
                gc_placeTrackTile(gc, n7 + 1, n6 + 1, 50, n3); gc_placeTrackTile(gc, n7 + 2, n6 + 1, 50, n3);
            }
            if (n5 != 2 || n4 != 3) break;
            gc_placeTrackTile(gc, n7, n6, 52, n3); gc_placeTrackTile(gc, n7 + 1, n6, 52, n3);
            gc_placeTrackTile(gc, n7, n6 + 1, 50, n3); gc_placeTrackTile(gc, n7 + 1, n6 + 1, 51, n3);
            gc_placeTrackTile(gc, n7, n6 + 2, 50, n3); gc_placeTrackTile(gc, n7 + 1, n6 + 2, 51, n3);
            break;
        case 1:
            if (n5 == 2 && n4 == 2) {
                gc_placeTrackTile(gc, n7, n6, 53, n3); gc_placeTrackTile(gc, n7, n6 + 1, 53, n3);
                gc_placeTrackTile(gc, n7 + 1, n6, 55, n3); gc_placeTrackTile(gc, n7 + 1, n6 + 1, 54, n3);
            }
            if ((n5 == 3 && n4 == 2) || (n5 == 3 && n4 == 3)) {
                gc_placeTrackTile(gc, n7, n6, 53, n3); gc_placeTrackTile(gc, n7, n6 + 1, 53, n3);
                gc_placeTrackTile(gc, n7 + 1, n6, 55, n3); gc_placeTrackTile(gc, n7 + 2, n6, 55, n3);
                gc_placeTrackTile(gc, n7 + 1, n6 + 1, 54, n3); gc_placeTrackTile(gc, n7 + 2, n6 + 1, 54, n3);
            }
            if (n5 != 2 || n4 != 3) break;
            gc_placeTrackTile(gc, n7, n6, 53, n3); gc_placeTrackTile(gc, n7 + 1, n6, 53, n3);
            gc_placeTrackTile(gc, n7, n6 + 1, 55, n3); gc_placeTrackTile(gc, n7 + 1, n6 + 1, 55, n3);
            gc_placeTrackTile(gc, n7, n6 + 2, 54, n3); gc_placeTrackTile(gc, n7 + 1, n6 + 2, 54, n3);
            break;
        case 2:
            if (n5 != 2 || n4 != 2) break;
            gc_placeTrackTile(gc, n7, n6, 57 + abs(nextRngInt(gc) % 2), n3);
            gc_placeTrackTile(gc, n7, n6 + 1, 56, n3);
            gc_placeTrackTile(gc, n7 + 1, n6, 57 + abs(nextRngInt(gc) % 2), n3);
            gc_placeTrackTile(gc, n7 + 1, n6 + 1, 57 + abs(nextRngInt(gc) % 2), n3);
            break;
        case 3:
            for (n2 = n7; n2 < n7 + n5; ++n2)
                for (int i = n6; i < n6 + n4; ++i)
                    gc_placeTrackTile(gc, n2, i, 59 + abs(nextRngInt(gc) % 3), n3);
            break;
        case 4:
            if (n5 == 2 && n4 == 2) {
                gc_placeTrackTile(gc, n7, n6, 62, n3); gc_placeTrackTile(gc, n7, n6 + 1, 62, n3);
                gc_placeTrackTile(gc, n7 + 1, n6, 64, n3); gc_placeTrackTile(gc, n7 + 1, n6 + 1, 63, n3);
            }
            if ((n5 == 3 && n4 == 2) || (n5 == 3 && n4 == 3)) {
                gc_placeTrackTile(gc, n7, n6, 62, n3); gc_placeTrackTile(gc, n7, n6 + 1, 62, n3);
                gc_placeTrackTile(gc, n7 + 1, n6, 64, n3); gc_placeTrackTile(gc, n7 + 2, n6, 64, n3);
                gc_placeTrackTile(gc, n7 + 1, n6 + 1, 62, n3); gc_placeTrackTile(gc, n7 + 2, n6 + 1, 63, n3);
            }
            if (n5 != 2 || n4 != 3) break;
            gc_placeTrackTile(gc, n7, n6, 62, n3); gc_placeTrackTile(gc, n7 + 1, n6, 63, n3);
            gc_placeTrackTile(gc, n7, n6 + 1, 62, n3); gc_placeTrackTile(gc, n7 + 1, n6 + 1, 64, n3);
            gc_placeTrackTile(gc, n7, n6 + 2, 62, n3); gc_placeTrackTile(gc, n7 + 1, n6 + 2, 64, n3);
            break;
        case 5:
            if (n5 == 2 && n4 == 2) {
                gc_placeTrackTile(gc, n7, n6, 65, n3); gc_placeTrackTile(gc, n7 + 1, n6, 66, n3);
                gc_placeTrackTile(gc, n7, n6 + 1, 67, n3);
            }
            if ((n5 == 3 && n4 == 2) || (n5 == 3 && n4 == 3)) {
                gc_placeTrackTile(gc, n7, n6, 65, n3); gc_placeTrackTile(gc, n7 + 1, n6, 66, n3);
                gc_placeTrackTile(gc, n7 + 2, n6, 67, n3); gc_placeTrackTile(gc, n7, n6 + 1, 68, n3);
            }
            if (n5 != 2 || n4 != 3) break;
            gc_placeTrackTile(gc, n7, n6, 65, n3); gc_placeTrackTile(gc, n7, n6 + 1, 66, n3);
            gc_placeTrackTile(gc, n7, n6 + 2, 67, n3); gc_placeTrackTile(gc, n7 + 1, n6, 68, n3);
            break;
    }
}

// void loadState25Setup(int n) -- the incremental map generator
void gc_loadState25Setup(GameCanvas *gc, int32_t n) {
    if (n == 0) {
        gc->world.tileMap.resize(gc->world.mapWidth + 1, gc->world.mapHeight + 1);
        gc->world.trackMap.resize(gc->world.mapWidth + 1, gc->world.mapHeight + 1);
        gc->world.tileElevation.resize(gc->world.mapWidth + 1, gc->world.mapHeight + 1);
        gc->world.heightMap.resize(gc->world.mapWidth + 2, gc->world.mapHeight + 2);
        gc->initialIndustryPlaced = false;
        gc->mapGenStage = mapgen::STAGE_INIT;
        gc->tilesGeneratedInStage = 0;
        gc->totalTilesToGenerate = gc->terrainRaiseStepCount + gc->terrainLowerStepCount + gc->mapGenColumns * (gc->mapGenColumns - 1)
            + gc->mapGenRows + gc->weightAccel2 + gc->weightAccel + gc->terrainFeatureStage6Count
            + gc->terrainFeatureStage7Count + gc->weightAccel4 + gc->weightAccel3 + 1;
        gc->mapGenTilesDone = 0;
    }
    if (n == 1) {
        int n2 = 0, n3, n4;
        if (gc->mapGenStage == mapgen::STAGE_INIT) { gc->mapGenStage = mapgen::STAGE_RAISE_TERRAIN; gc->tilesGeneratedInStage = 0; }
        if (gc->mapGenStage == mapgen::STAGE_RAISE_TERRAIN && gc->tilesGeneratedInStage == gc->terrainRaiseStepCount) { gc->mapGenStage = mapgen::STAGE_LOWER_TERRAIN; gc->tilesGeneratedInStage = 0; gc_toggleFlagInlined(gc, false); }
        if (gc->mapGenStage == mapgen::STAGE_LOWER_TERRAIN && gc->tilesGeneratedInStage == gc->terrainLowerStepCount) { gc->mapGenStage = mapgen::STAGE_PLACE_PATCHES; gc->tilesGeneratedInStage = 0; gc_toggleFlagInlined(gc, false); }
        if (gc->mapGenStage == mapgen::STAGE_PLACE_PATCHES && gc->tilesGeneratedInStage == gc->mapGenColumns * (gc->mapGenColumns - 1) + gc->mapGenRows) { gc->mapGenStage = mapgen::STAGE_FEATURE_0; gc->tilesGeneratedInStage = 0; }
        if (gc->mapGenStage == mapgen::STAGE_FEATURE_0 && gc->tilesGeneratedInStage == gc->weightAccel2) { gc->mapGenStage = mapgen::STAGE_FEATURE_1; gc->tilesGeneratedInStage = 0; }
        if (gc->mapGenStage == mapgen::STAGE_FEATURE_1 && gc->tilesGeneratedInStage == gc->weightAccel) { gc->mapGenStage = mapgen::STAGE_FEATURE_2; gc->tilesGeneratedInStage = 0; }
        if (gc->mapGenStage == mapgen::STAGE_FEATURE_2 && gc->tilesGeneratedInStage == gc->terrainFeatureStage6Count) { gc->mapGenStage = mapgen::STAGE_FEATURE_3; gc->tilesGeneratedInStage = 0; }
        if (gc->mapGenStage == mapgen::STAGE_FEATURE_3 && gc->tilesGeneratedInStage == gc->terrainFeatureStage7Count) { gc->mapGenStage = mapgen::STAGE_FEATURE_4; gc->tilesGeneratedInStage = 0; }
        if (gc->mapGenStage == mapgen::STAGE_FEATURE_4 && gc->tilesGeneratedInStage == gc->weightAccel4) { gc->mapGenStage = mapgen::STAGE_FEATURE_5; gc->tilesGeneratedInStage = 0; }
        if (gc->mapGenStage == mapgen::STAGE_FEATURE_5 && gc->tilesGeneratedInStage == gc->weightAccel3) { gc->mapGenStage = mapgen::STAGE_SCATTER_OBSTACLES; gc->tilesGeneratedInStage = 0; }
        if (gc->mapGenStage == mapgen::STAGE_RAISE_TERRAIN) {
            int center_x = abs(nextRngInt(gc) % gc->world.mapWidth);
            int center_y = abs(nextRngInt(gc) % gc->world.mapHeight);
            int width = abs(nextRngInt(gc) % 20) + 1;
            int height = abs(nextRngInt(gc) % 20) + 1;
            int elevation_step = abs(nextRngInt(gc) % 2) + 1;
            gc_raiseTerrainArea(gc, center_x, center_y, width, height,
                                elevation_step);
        }
        if (gc->mapGenStage == mapgen::STAGE_LOWER_TERRAIN) {
            int center_x = abs(nextRngInt(gc) % gc->world.mapWidth);
            int center_y = abs(nextRngInt(gc) % gc->world.mapHeight);
            int width = abs(nextRngInt(gc) % 20) + 1;
            int height = abs(nextRngInt(gc) % 20) + 1;
            gc_lowerTerrainArea(gc, center_x, center_y, width, height, -1);
        }
        gc->trackBuildInProgress = false;
        if (gc->mapGenStage == mapgen::STAGE_PLACE_PATCHES) {
            for (n4 = 0; n4 < gc->mapGenColumns; ++n4) {
                bool done = false;
                for (n3 = 0; n3 < gc->mapGenRows && !done; ++n3) {
                    if (gc->tilesGeneratedInStage != gc->mapGenColumns * n4 + n3) continue;
                    n2 = gc->world.mapWidth / gc->mapGenColumns;
                    int n5 = gc->world.mapHeight / gc->mapGenRows;
                    int n6 = n4 * n2 + abs(nextRngInt(gc) % (n2 / 2));
                    int n7 = n3 * n5 + abs(nextRngInt(gc) % (n5 / 2));
                    gc_placeTerrainPatch(gc, n6, n7);
                    done = true;
                }
                if (done) break;
            }
        }
        if (gc->mapGenStage == mapgen::STAGE_FEATURE_0) gc_placeTerrainFeature(gc, 0);
        if (gc->mapGenStage == mapgen::STAGE_FEATURE_1) gc_placeTerrainFeature(gc, 1);
        if (gc->mapGenStage == mapgen::STAGE_FEATURE_2) gc_placeTerrainFeature(gc, 2);
        if (gc->mapGenStage == mapgen::STAGE_FEATURE_3) gc_placeTerrainFeature(gc, 3);
        if (gc->mapGenStage == mapgen::STAGE_FEATURE_4) gc_placeTerrainFeature(gc, 4);
        if (gc->mapGenStage == mapgen::STAGE_FEATURE_5) gc_placeTerrainFeature(gc, 5);
        if (gc->trackBuildInProgress) gc->loadStep = 0;
        if (gc->mapGenStage == mapgen::STAGE_SCATTER_OBSTACLES) {
            for (n4 = 0; n4 < gc->initialObstacleCount; ++n4) {
                n3 = abs(nextRngInt(gc) % gc->world.mapWidth);
                n2 = abs(nextRngInt(gc) % gc->world.mapHeight);
                if (gc->world.tileMap[n3][n2] != 0 || gc->world.tileElevation[n3][n2] < 0) continue;
                gc->world.tileMap[n3][n2] = (int8_t)(tile::SCENERY_FIRST + abs(nextRngInt(gc) % 5));
            }
            gc->initialIndustryPlaced = true;
        }
        ++gc->tilesGeneratedInStage;
        ++gc->mapGenTilesDone;
    }
}

void gc_pickCitySite(GameCanvas *gc) {
    int x = abs(nextRngInt(gc) % gc->world.mapWidth);
    int y = abs(nextRngInt(gc) % gc->world.mapHeight);
    gc->industryCategory = -1;
    int tile = gc->world.tileMap[x][y];
    if (tile != 0 && (tile < tile::SCENERY_FIRST || tile > tile::SCENERY_LAST)) return;
    int min_x = x - 2;
    int min_y = y - 2;
    int max_x = x + 2;
    int max_y = y + 2;
    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    if (max_x >= gc->world.mapWidth) max_x = gc->world.mapWidth - 1;
    if (max_y >= gc->world.mapHeight) max_y = gc->world.mapHeight - 1;
    for (int near_x = min_x;
         near_x <= max_x && gc->industryCategory <= 0; ++near_x) {
        for (int near_y = min_y;
             near_y <= max_y && gc->industryCategory <= 0; ++near_y) {
            tile = gc->world.tileMap[near_x][near_y];
            if (tile >= 34 && tile <= 40) gc->industryCategory = 0;
            if (tile >= 65 && tile <= 68) gc->industryCategory = 1;
            if (tile >= 53 && tile <= 55) gc->industryCategory = 2;
            if (tile >= 59 && tile <= 61) gc->industryCategory = 3;
        }
    }
    if (gc->industryCategory >= 0) {
        gc->cityStationX = x;
        gc->cityStationY = y;
    }
}

void gc_pickIndustrySite(GameCanvas *gc) {
    int x = abs(nextRngInt(gc) % gc->world.mapWidth);
    int y = abs(nextRngInt(gc) % gc->world.mapHeight);
    gc->industrySiteFound = false;
    int tile = gc->world.tileMap[x][y];
    if (tile != 0 && (tile < tile::SCENERY_FIRST || tile > tile::SCENERY_LAST)) return;
    int min_x = x - 2;
    int min_y = y - 2;
    int max_x = x + 2;
    int max_y = y + 2;
    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    if (max_x >= gc->world.mapWidth) max_x = gc->world.mapWidth - 1;
    if (max_y >= gc->world.mapHeight) max_y = gc->world.mapHeight - 1;
    for (int near_x = min_x;
         near_x <= max_x && !gc->industrySiteFound; ++near_x) {
        for (int near_y = min_y;
             near_y <= max_y && !gc->industrySiteFound; ++near_y) {
            tile = gc->world.tileMap[near_x][near_y];
            if (tile >= 34 && tile <= 40 && gc->industryCategory == 0)
                gc->industrySiteFound = true;
            if (tile >= 62 && tile <= 64 && gc->industryCategory == 1)
                gc->industrySiteFound = true;
            if (tile >= 50 && tile <= 52 && gc->industryCategory == 2)
                gc->industrySiteFound = true;
            if (tile >= 56 && tile <= 58 && gc->industryCategory == 3)
                gc->industrySiteFound = true;
        }
    }
    if (gc->industrySiteFound) {
        gc->industryStationX = x;
        gc->industryTileY = y;
    }
}

void gc_spawnTrainAt(GameCanvas *gc, int32_t x, int32_t y) {
    int engine_type = gc->era / 3;
    int wagon_count = gc->selectionCost / 8000 + 1;
    if (wagon_count > 5) wagon_count = 5;
    int cargo_type = 2;
    if (gc->industryCategory == 1) cargo_type = 3;
    if (gc->industryCategory == 2) cargo_type = 4;
    if (gc->industryCategory == 3) cargo_type = 6;
    int train = gc_allocateTrainSlot(
        gc, x, y, engine_type, gc->era,
        gc->world.tileMap[x][y] == 87 ? 0 : 2, 8 - gc->era);
    for (int wagon = 0; wagon < wagon_count; ++wagon)
        train = gc_spawnTrainCar(gc, train, cargo_type);
}

static bool can_extend_station_over(GameCanvas *gc, int x, int y) {
    int tile = gc->world.tileMap[x][y];
    return tile == 0 || (tile >= tile::SCENERY_FIRST && tile <= tile::SCENERY_LAST);
}

void gc_autoExtendTrackStation(GameCanvas *gc, int32_t x, int32_t y) {
    bool track_left = x > 0 && gc->world.trackMap[x - 1][y] == track::PRESENT;
    bool track_right = x < gc->world.mapWidth - 1 && gc->world.trackMap[x + 1][y] == track::PRESENT;
    bool track_up = y > 0 && gc->world.trackMap[x][y - 1] == track::PRESENT;
    bool track_down = y < gc->world.mapHeight - 1 && gc->world.trackMap[x][y + 1] == track::PRESENT;
    if (track_left || track_right) {
        gc->world.tileMap[x][y] = 88;
        gc->world.trackMap[x][y] = (int8_t)gc->nextTrackPieceCode;
        if (track_right && x > 1
            && can_extend_station_over(gc, x - 1, y)
            && can_extend_station_over(gc, x - 2, y)) {
            gc->world.tileMap[x][y] = 90;
            gc->world.tileMap[x - 1][y] = 88;
            gc->world.tileMap[x - 2][y] = 90;
            gc->world.trackMap[x - 1][y] = (int8_t)gc->nextTrackPieceCode;
            gc->world.trackMap[x - 2][y] = (int8_t)gc->nextTrackPieceCode;
        }
        if (track_left && x < gc->world.mapWidth - 2
            && can_extend_station_over(gc, x + 1, y)
            && can_extend_station_over(gc, x + 2, y)) {
            gc->world.tileMap[x][y] = 90;
            gc->world.tileMap[x + 1][y] = 88;
            gc->world.tileMap[x + 2][y] = 90;
            gc->world.trackMap[x + 1][y] = (int8_t)gc->nextTrackPieceCode;
            gc->world.trackMap[x + 2][y] = (int8_t)gc->nextTrackPieceCode;
        }
    }
    if (track_up || track_down) {
        gc->world.tileMap[x][y] = 87;
        gc->world.trackMap[x][y] = (int8_t)gc->nextTrackPieceCode;
        if (track_up && y > 1
            && can_extend_station_over(gc, x, y - 1)
            && can_extend_station_over(gc, x, y - 2)) {
            gc->world.tileMap[x][y] = 89;
            gc->world.tileMap[x][y - 1] = 87;
            gc->world.tileMap[x][y - 2] = 89;
            gc->world.trackMap[x][y - 1] = (int8_t)gc->nextTrackPieceCode;
            gc->world.trackMap[x][y - 2] = (int8_t)gc->nextTrackPieceCode;
        }
        // The original bytecode tests track_up here too (not track_down).
        if (track_up && y < gc->world.mapHeight - 2
            && can_extend_station_over(gc, x, y + 1)
            && can_extend_station_over(gc, x, y + 2)) {
            gc->world.tileMap[x][y] = 89;
            gc->world.tileMap[x][y + 1] = 87;
            gc->world.tileMap[x][y + 2] = 89;
            gc->world.trackMap[x][y + 1] = (int8_t)gc->nextTrackPieceCode;
            gc->world.trackMap[x][y + 2] = (int8_t)gc->nextTrackPieceCode;
        }
    }
}

void gc_placeCitiesAndIndustry(GameCanvas *gc) {
    int city_attempts = 0;
    int industry_attempts = 0;
    do {
        gc_pickCitySite(gc);
    } while (++city_attempts != 50 && gc->industryCategory == -1);
    if (city_attempts >= 50) return;
    do {
        gc_pickIndustrySite(gc);
    } while (++industry_attempts != 50 && !gc->industrySiteFound);
    if (industry_attempts >= 50) return;

    gc->placementX = gc->cityStationX;
    gc->placementY = gc->cityStationY;
    gc->demolishX = gc->industryStationX;
    gc->demolishY = gc->industryTileY;
    if (!gc_tryBuildPath(gc)) return;
    gc_autoExtendTrackStation(gc, gc->cityStationX, gc->cityStationY);
    gc_autoExtendTrackStation(gc, gc->industryStationX, gc->industryTileY);
    gc_spawnTrainAt(gc, gc->cityStationX, gc->cityStationY);
    gc_setTrackOwnerCode(gc, gc->nextTrackPieceCode - track::PRESENT);
    ++gc->nextTrackPieceCode;
    if (gc->nextTrackPieceCode == track::AI_PIECE_WRAP) gc->nextTrackPieceCode = track::AI_PIECE_FIRST;
}
