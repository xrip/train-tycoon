// Terrain and map generation.
// Driven by loadState25Setup across mapGenStage 0..10 (raise/lower terrain,
// place patches and features, scatter industry). The random process is exact.
#include "game_internal.h"
#include <stdlib.h> // abs
#include <vector>

static int nextRngInt(Game *game) { return game->rng.next_int(); }

// Game::mapGenStage steps through the incremental generator in
// game_runMapGenerationStep: init, raise then lower terrain, place patches, six
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

int8_t game_randomCityTile(Game *game) {
    int n = abs(nextRngInt(game) % 22);
    if (n < 6) return 34;
    if (n < 11) return 35;
    if (n < 15) return 36;
    if (n < 18) return 37;
    if (n < 20) return 38;
    if (n < 21) return 39;
    if (n < 22) return 40;
    return 0;
}

int8_t game_addCityEntry(Game *game, int32_t n) {
    game->world.cities[game->ui.cityListIndex].stationDirection = (int8_t)n;
    game->world.cities[game->ui.cityListIndex].population = (int8_t)(50 + nextRngInt(game) % 20);
    game->world.cities[game->ui.cityListIndex].zoneTileCount = 0;
    game->ui.cityListIndex = (int8_t)(game->ui.cityListIndex + 1);
    return (int8_t)(game->ui.cityListIndex - 1);
}

int8_t game_registerTrain(Game *game, int8_t by, int32_t n, int32_t n2) {
    game->world.constructionTimers[game->ui.numSelectedTrains] = 50;
    game->world.zonePopulation[game->ui.numSelectedTrains] = by;
    game->ui.trainSelectionY[game->ui.numSelectedTrains] = (int16_t)n;
    game->ui.trainSelectionX[game->ui.numSelectedTrains] = (int16_t)n2;
    int16_t s = 0;
    bool bl;
    do {
        int8_t by2 = (int8_t)abs(nextRngInt(game) % 6);
        int8_t by3 = (int8_t)abs(nextRngInt(game) % 14);
        s = (int16_t)(256 * by2 + by3);
        bl = true;
        for (int i = 0; i < game->ui.numSelectedTrains; ++i)
            if (s == game->ui.trainSelectionIndex[i]) bl = false;
    } while (!bl);
    game->ui.trainSelectionIndex[game->ui.numSelectedTrains] = s;
    game->ui.numSelectedTrains = (int8_t)(game->ui.numSelectedTrains + 1);
    return (int8_t)(game->ui.numSelectedTrains - 1);
}

// Rebuild slope tiles from the height map.
void game_rebuildTerrainTiles(Game *game, bool bl) {
    int n, n2, n3, n4;
    if (bl) {
        game_commitTerrainEdit(game);
        game->build.terrainEditBuffer.clear();
        n4 = game->build.selectionStartX; n3 = game->build.selectionStartY;
        n2 = game->build.selectionStartX + 10; n = game->build.selectionStartY + 10;
    } else {
        n4 = 0; n3 = 0; n2 = game->world.terrain.mapWidth; n = game->world.terrain.mapHeight;
    }
    for (int i = n4; i < n2; ++i) {
        for (int j = n3; j < n; ++j) {
            int8_t by = game->world.terrain.heightMap[i][j];
            int8_t by2 = game->world.terrain.heightMap[i][j + 1];
            int8_t by3 = game->world.terrain.heightMap[i + 1][j];
            int8_t by4 = game->world.terrain.heightMap[i + 1][j + 1];
            if (game->world.terrain.tileMap[i][j] >= tile::BUILT_FIRST) continue;
            if (by == by2 && by == by3 && by == by4) {
                game->world.terrain.tileElevation[i][j] = by;
                if (game->world.terrain.tileMap[i][j] <= 28) game->world.terrain.tileMap[i][j] = 0;
            } else if (by == by2 && by == by3 && by4 < by) {
                game->world.terrain.tileElevation[i][j] = by; game->world.terrain.tileMap[i][j] = 11;
            }
            if (by == by3 && by == by4 && by2 < by) { game->world.terrain.tileElevation[i][j] = by; game->world.terrain.tileMap[i][j] = 13; }
            if (by == by2 && by == by4 && by3 < by) { game->world.terrain.tileElevation[i][j] = by; game->world.terrain.tileMap[i][j] = 14; }
            if (by2 == by3 && by2 == by4 && by < by2) { game->world.terrain.tileElevation[i][j] = by2; game->world.terrain.tileMap[i][j] = 12; }
            if (by == by4 && by2 == by3 && by2 < by) { game->world.terrain.tileElevation[i][j] = by; game->world.terrain.tileMap[i][j] = 16; }
            if (by == by4 && by2 == by3 && by < by2) { game->world.terrain.tileElevation[i][j] = by2; game->world.terrain.tileMap[i][j] = 15; }
            if (by == by2 && by3 == by4 && by3 < by) {
                game->world.terrain.tileElevation[i][j] = by;
                game->world.terrain.tileMap[i][j] = (int8_t)(by3 == -1 && i + 2 <= game->world.terrain.mapWidth && (game->world.terrain.heightMap[i + 2][j] == -1 || game->world.terrain.heightMap[i + 2][j + 1] == -1) ? 9 : 24);
            }
            if (by == by2 && by3 == by4 && by < by3) {
                game->world.terrain.tileElevation[i][j] = by3;
                game->world.terrain.tileMap[i][j] = (int8_t)(by == -1 && i - 1 >= 0 && (game->world.terrain.heightMap[i - 1][j] == -1 || game->world.terrain.heightMap[i - 1][j + 1] == -1) ? 8 : 22);
            }
            if (by == by3 && by2 == by4 && by2 < by) {
                game->world.terrain.tileElevation[i][j] = by;
                game->world.terrain.tileMap[i][j] = (int8_t)(by2 == -1 && j + 2 <= game->world.terrain.mapHeight && (game->world.terrain.heightMap[i][j + 2] == -1 || game->world.terrain.heightMap[i + 1][j + 2] == -1) ? 10 : 21);
            }
            if (by == by3 && by2 == by4 && by < by2) {
                game->world.terrain.tileElevation[i][j] = by2;
                game->world.terrain.tileMap[i][j] = (int8_t)(by == -1 && j - 1 >= 0 && (game->world.terrain.heightMap[i][j - 1] == -1 || game->world.terrain.heightMap[i + 1][j - 1] == -1) ? 7 : 23);
            }
            if (by == by2 && by == by3 && by < by4) { game->world.terrain.tileElevation[i][j] = by; game->world.terrain.tileMap[i][j] = (int8_t)(by == -1 ? 3 : 19); }
            if (by == by2 && by == by4 && by < by3) { game->world.terrain.tileElevation[i][j] = by; game->world.terrain.tileMap[i][j] = (int8_t)(by == -1 ? 6 : 18); }
            if (by == by3 && by == by4 && by < by2) { game->world.terrain.tileElevation[i][j] = by; game->world.terrain.tileMap[i][j] = (int8_t)(by == -1 ? 5 : 20); }
            if (by2 == by3 && by2 == by4 && by4 < by) { game->world.terrain.tileElevation[i][j] = by4; game->world.terrain.tileMap[i][j] = (int8_t)(by4 == -1 ? 4 : 17); }
            if (by == by4 && by2 < by && by3 > by) { game->world.terrain.tileElevation[i][j] = by; game->world.terrain.tileMap[i][j] = 25; }
            if (by3 == by2 && by < by3 && by4 > by3) { game->world.terrain.tileElevation[i][j] = by3; game->world.terrain.tileMap[i][j] = 26; }
            if (by == by4 && by2 > by && by3 < by) { game->world.terrain.tileElevation[i][j] = by; game->world.terrain.tileMap[i][j] = 27; }
            if (by3 != by2 || by <= by3 || by4 >= by3) continue;
            game->world.terrain.tileElevation[i][j] = by3; game->world.terrain.tileMap[i][j] = 28;
        }
    }
}

void game_smoothTileAt(Game *game, int32_t n, int32_t n2, bool bl) {
    if ((bl && game->world.terrain.heightMap[n][n2] < 3) || (!bl && game->world.terrain.heightMap[n][n2] > -1)) {
        game_setSelectionStart(game, n, n2);
        if (game_isAdjacentTileEmpty(game, n, n2, game->world.terrain.heightMap[n][n2] + (bl ? 1 : -1), bl)) {
            if (game->world.initialIndustryPlaced) {
                if (game->build.terrainSmoothCost <= game->economy.money) {
                    game->economy.money -= game->build.terrainSmoothCost;
                    game_pushMoneyHistory(game, -game->build.terrainSmoothCost);
                    game->economy.subsidyCost += game->build.terrainSmoothCost;
                    game_rebuildTerrainTiles(game, true);
                } else {
                    game_enterOutOfMoneyState(game);
                }
            } else {
                game_rebuildTerrainTiles(game, true);
            }
        }
    }
    game->renderer.lastViewportCenterX = -1;
}

void game_raiseTerrainArea(Game *game, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5) {
    for (int i = n; i < n + n3; ++i) {
        for (int j = n2; j < n2 + n4; ++j) {
            if (i <= 0 || j <= 0 || i > game->world.terrain.mapWidth || j > game->world.terrain.mapHeight) continue;
            game_setSelectionStart(game, i, j);
            game_isAdjacentTileEmpty(game, i, j, n5, true);
            if (nextRngInt(game) % 2 < 0 && i > 0) game_isAdjacentTileEmpty(game, i - 1, j, n5, true);
            if (nextRngInt(game) % 2 < 0 && j > 0) game_isAdjacentTileEmpty(game, i, j - 1, n5, true);
            if (nextRngInt(game) % 2 < 0 && i < game->world.terrain.mapWidth) game_isAdjacentTileEmpty(game, i + 1, j, n5, true);
            if (nextRngInt(game) % 2 < 0 && j < game->world.terrain.mapHeight) game_isAdjacentTileEmpty(game, i, j + 1, n5, true);
            game_commitTerrainEdit(game);
        }
    }
}

void game_lowerTerrainArea(Game *game, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5) {
    (void)n5;
    for (int i = n; i < n + n3; ++i) {
        for (int j = n2; j < n2 + n4; ++j) {
            if (i <= 0 || j <= 0 || i > game->world.terrain.mapWidth || j > game->world.terrain.mapHeight) continue;
            for (int k = 0; k < 3; ++k) game_smoothTileAt(game, i, j, false);
            if (nextRngInt(game) % 2 < 0 && i > 0) game_smoothTileAt(game, i - 1, j, false);
            if (nextRngInt(game) % 2 < 0 && j > 0) game_smoothTileAt(game, i, j - 1, false);
            if (nextRngInt(game) % 2 < 0 && i < game->world.terrain.mapWidth) game_smoothTileAt(game, i + 1, j, false);
            if (nextRngInt(game) % 2 >= 0 || j >= game->world.terrain.mapHeight) continue;
            game_smoothTileAt(game, i, j + 1, false);
        }
    }
}

void game_trackBuildStep(Game *game, int32_t n, int32_t n2, int32_t n3, int32_t n4) {
    int8_t by = -2;
    for (int i = n; i < n + n3; ++i) {
        for (int j = n2; j < n2 + n4; ++j) {
            if (i >= game->world.terrain.mapWidth || j >= game->world.terrain.mapHeight) continue;
            if (by == -2 && (by = game->world.terrain.heightMap[i][j]) < 0) by = 0;
            while (by != game->world.terrain.heightMap[i][j]) game_smoothTileAt(game, i, j, by > game->world.terrain.heightMap[i][j]);
            while (by != game->world.terrain.heightMap[i + 1][j]) game_smoothTileAt(game, i + 1, j, by > game->world.terrain.heightMap[i + 1][j]);
            while (by != game->world.terrain.heightMap[i][j + 1]) game_smoothTileAt(game, i, j + 1, by > game->world.terrain.heightMap[i][j + 1]);
            while (by != game->world.terrain.heightMap[i + 1][j + 1]) game_smoothTileAt(game, i + 1, j + 1, by > game->world.terrain.heightMap[i + 1][j + 1]);
        }
    }
}

void game_placeTerrainPatch(Game *game, int32_t n, int32_t n2) {
    int n3, n4, n5, n6;
    int8_t by, by2, by3, by4;
    int n7 = abs(nextRngInt(game) % 5) + game->world.terrainPatchBaseSize;
    int8_t by5 = (int8_t)(abs(nextRngInt(game) % 5) + game->world.terrainPatchBaseSize);
    std::vector<std::vector<int8_t>> patch_tiles(
        static_cast<size_t>(n7),
        std::vector<int8_t>(static_cast<size_t>(by5)));
    bool bl = false;
    int n8 = abs(nextRngInt(game) % (n7 - 2)) + 1;
    for (n6 = abs(nextRngInt(game) % 3); n6 < by5 - abs(nextRngInt(game) % 3); ++n6) patch_tiles[n8][n6] = 1;
    if (n7 >= 7) {
        do {
            n8 = abs(nextRngInt(game) % (n7 - 2)) + 1;
            bl = false;
            for (n6 = 0; n6 < by5; ++n6)
                if (patch_tiles[n8 - 1][n6] == 1 || patch_tiles[n8 + 1][n6] == 1) bl = true;
        } while (bl);
        for (n6 = abs(nextRngInt(game) % 3); n6 < by5 - abs(nextRngInt(game) % 3); ++n6) patch_tiles[n8][n6] = 1;
    }
    n6 = abs(nextRngInt(game) % (by5 - 2)) + 1;
    for (n5 = abs(nextRngInt(game) % 3); n5 < n7 - abs(nextRngInt(game) % 3); ++n5) patch_tiles[n5][n6] = 2;
    if (by5 >= 7) {
        do {
            n6 = abs(nextRngInt(game) % (by5 - 2)) + 1;
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
            patch_tiles[n5][by4] = game_randomCityTile(game);
        }
    }
    n5 = -2;
    for (by4 = 0; by4 < n7; ++by4) {
        for (by3 = 0; by3 < by5; ++by3) {
            if (n + by4 >= game->world.terrain.mapWidth || n2 + by3 >= game->world.terrain.mapHeight
                || (patch_tiles[by4][by3] == 0 && (patch_tiles[by4][by3] != 0 || nextRngInt(game) % 3 >= 0))) continue;
            if (n5 == -2 && (n5 = game->world.terrain.heightMap[n + by4][n2 + by3]) < 0) n5 = 0;
            int64_t l = midp_current_time_millis();
            while (n5 != game->world.terrain.heightMap[n + by4][n2 + by3]) {
                game_smoothTileAt(game, n + by4, n2 + by3, n5 > game->world.terrain.heightMap[n + by4][n2 + by3]);
                if (midp_current_time_millis() - l <= 1000) continue;
                game->build.trackBuildInProgress = true; return;
            }
            l = midp_current_time_millis();
            while (n5 != game->world.terrain.heightMap[n + by4 + 1][n2 + by3]) {
                game_smoothTileAt(game, n + by4 + 1, n2 + by3, n5 > game->world.terrain.heightMap[n + by4 + 1][n2 + by3]);
                if (midp_current_time_millis() - l <= 1000) continue;
                game->build.trackBuildInProgress = true; return;
            }
            l = midp_current_time_millis();
            while (n5 != game->world.terrain.heightMap[n + by4][n2 + by3 + 1]) {
                game_smoothTileAt(game, n + by4, n2 + by3 + 1, n5 > game->world.terrain.heightMap[n + by4][n2 + by3 + 1]);
                if (midp_current_time_millis() - l <= 1000) continue;
                game->build.trackBuildInProgress = true; return;
            }
            l = midp_current_time_millis();
            while (n5 != game->world.terrain.heightMap[n + by4 + 1][n2 + by3 + 1]) {
                game_smoothTileAt(game, n + by4 + 1, n2 + by3 + 1, n5 > game->world.terrain.heightMap[n + by4 + 1][n2 + by3 + 1]);
                if (midp_current_time_millis() - l <= 1000) continue;
                game->build.trackBuildInProgress = true; return;
            }
        }
    }
    by4 = 0;
    for (by3 = 0; by3 < n7; ++by3)
        for (int8_t by6 = 0; by6 < by5; ++by6)
            if (!(patch_tiles[by3][by6] > 40 || patch_tiles[by3][by6] < 34)) by4 = (int8_t)(by4 + 1);
    by3 = game_registerTrain(game, by4, n + n7 / 2, n2 + by5 / 2);
    for (int i = 0; i < n7; ++i) {
        for (n4 = 0; n4 < by5; ++n4) {
            if (n + i >= game->world.terrain.mapWidth || n2 + n4 >= game->world.terrain.mapHeight || patch_tiles[i][n4] == 0) continue;
            game->world.terrain.tileMap[n + i][n2 + n4] = patch_tiles[i][n4];
            game->world.trackMap[n + i][n2 + n4] = (int8_t)(0x80 | by3);
        }
    }
}

void game_placeTerrainFeature(Game *game, int32_t n) {
    int n2, n3, n4, n5, n6, n7;
    bool bl;
    if (game->build.trackBuildInProgress) return;
    int64_t l = midp_current_time_millis();
    do {
        bl = true;
        n7 = abs(nextRngInt(game) % (game->world.terrain.mapWidth - 8)) + 4;
        n6 = abs(nextRngInt(game) % (game->world.terrain.mapHeight - 8)) + 4;
        n5 = nextRngInt(game) % 2 <= 0 ? 3 : 2;
        n4 = nextRngInt(game) % 2 <= 0 ? 3 : 2;
        if (n == 2) { n5 = 2; n4 = 2; }
        for (n3 = n7 - 3; n3 < n7 + n5 + 3; ++n3) {
            for (n2 = n6 - 3; n2 < n6 + n4 + 3; ++n2) {
                if (game->world.terrain.tileMap[n3][n2] >= tile::BUILT_FIRST) bl = false;
                if (game->world.terrain.tileElevation[n3][n2] != -1) continue;
                bl = false;
            }
        }
        if (midp_current_time_millis() - l <= 1500) continue;
        game->build.trackBuildInProgress = true;
        break;
    } while (!bl);
    if (game->build.trackBuildInProgress) return;
    game_trackBuildStep(game, n7, n6, n5, n4);
    n3 = game_addCityEntry(game, n);
    switch (n) {
        case 0:
            if (n5 == 2 && n4 == 2) {
                game_placeTrackTile(game, n7, n6, 52, n3); game_placeTrackTile(game, n7, n6 + 1, 52, n3);
                game_placeTrackTile(game, n7 + 1, n6, 51, n3); game_placeTrackTile(game, n7 + 1, n6 + 1, 50, n3);
            }
            if ((n5 == 3 && n4 == 2) || (n5 == 3 && n4 == 3)) {
                game_placeTrackTile(game, n7, n6, 52, n3); game_placeTrackTile(game, n7, n6 + 1, 52, n3);
                game_placeTrackTile(game, n7 + 1, n6, 51, n3); game_placeTrackTile(game, n7 + 2, n6, 51, n3);
                game_placeTrackTile(game, n7 + 1, n6 + 1, 50, n3); game_placeTrackTile(game, n7 + 2, n6 + 1, 50, n3);
            }
            if (n5 != 2 || n4 != 3) break;
            game_placeTrackTile(game, n7, n6, 52, n3); game_placeTrackTile(game, n7 + 1, n6, 52, n3);
            game_placeTrackTile(game, n7, n6 + 1, 50, n3); game_placeTrackTile(game, n7 + 1, n6 + 1, 51, n3);
            game_placeTrackTile(game, n7, n6 + 2, 50, n3); game_placeTrackTile(game, n7 + 1, n6 + 2, 51, n3);
            break;
        case 1:
            if (n5 == 2 && n4 == 2) {
                game_placeTrackTile(game, n7, n6, 53, n3); game_placeTrackTile(game, n7, n6 + 1, 53, n3);
                game_placeTrackTile(game, n7 + 1, n6, 55, n3); game_placeTrackTile(game, n7 + 1, n6 + 1, 54, n3);
            }
            if ((n5 == 3 && n4 == 2) || (n5 == 3 && n4 == 3)) {
                game_placeTrackTile(game, n7, n6, 53, n3); game_placeTrackTile(game, n7, n6 + 1, 53, n3);
                game_placeTrackTile(game, n7 + 1, n6, 55, n3); game_placeTrackTile(game, n7 + 2, n6, 55, n3);
                game_placeTrackTile(game, n7 + 1, n6 + 1, 54, n3); game_placeTrackTile(game, n7 + 2, n6 + 1, 54, n3);
            }
            if (n5 != 2 || n4 != 3) break;
            game_placeTrackTile(game, n7, n6, 53, n3); game_placeTrackTile(game, n7 + 1, n6, 53, n3);
            game_placeTrackTile(game, n7, n6 + 1, 55, n3); game_placeTrackTile(game, n7 + 1, n6 + 1, 55, n3);
            game_placeTrackTile(game, n7, n6 + 2, 54, n3); game_placeTrackTile(game, n7 + 1, n6 + 2, 54, n3);
            break;
        case 2:
            if (n5 != 2 || n4 != 2) break;
            game_placeTrackTile(game, n7, n6, 57 + abs(nextRngInt(game) % 2), n3);
            game_placeTrackTile(game, n7, n6 + 1, 56, n3);
            game_placeTrackTile(game, n7 + 1, n6, 57 + abs(nextRngInt(game) % 2), n3);
            game_placeTrackTile(game, n7 + 1, n6 + 1, 57 + abs(nextRngInt(game) % 2), n3);
            break;
        case 3:
            for (n2 = n7; n2 < n7 + n5; ++n2)
                for (int i = n6; i < n6 + n4; ++i)
                    game_placeTrackTile(game, n2, i, 59 + abs(nextRngInt(game) % 3), n3);
            break;
        case 4:
            if (n5 == 2 && n4 == 2) {
                game_placeTrackTile(game, n7, n6, 62, n3); game_placeTrackTile(game, n7, n6 + 1, 62, n3);
                game_placeTrackTile(game, n7 + 1, n6, 64, n3); game_placeTrackTile(game, n7 + 1, n6 + 1, 63, n3);
            }
            if ((n5 == 3 && n4 == 2) || (n5 == 3 && n4 == 3)) {
                game_placeTrackTile(game, n7, n6, 62, n3); game_placeTrackTile(game, n7, n6 + 1, 62, n3);
                game_placeTrackTile(game, n7 + 1, n6, 64, n3); game_placeTrackTile(game, n7 + 2, n6, 64, n3);
                game_placeTrackTile(game, n7 + 1, n6 + 1, 62, n3); game_placeTrackTile(game, n7 + 2, n6 + 1, 63, n3);
            }
            if (n5 != 2 || n4 != 3) break;
            game_placeTrackTile(game, n7, n6, 62, n3); game_placeTrackTile(game, n7 + 1, n6, 63, n3);
            game_placeTrackTile(game, n7, n6 + 1, 62, n3); game_placeTrackTile(game, n7 + 1, n6 + 1, 64, n3);
            game_placeTrackTile(game, n7, n6 + 2, 62, n3); game_placeTrackTile(game, n7 + 1, n6 + 2, 64, n3);
            break;
        case 5:
            if (n5 == 2 && n4 == 2) {
                game_placeTrackTile(game, n7, n6, 65, n3); game_placeTrackTile(game, n7 + 1, n6, 66, n3);
                game_placeTrackTile(game, n7, n6 + 1, 67, n3);
            }
            if ((n5 == 3 && n4 == 2) || (n5 == 3 && n4 == 3)) {
                game_placeTrackTile(game, n7, n6, 65, n3); game_placeTrackTile(game, n7 + 1, n6, 66, n3);
                game_placeTrackTile(game, n7 + 2, n6, 67, n3); game_placeTrackTile(game, n7, n6 + 1, 68, n3);
            }
            if (n5 != 2 || n4 != 3) break;
            game_placeTrackTile(game, n7, n6, 65, n3); game_placeTrackTile(game, n7, n6 + 1, 66, n3);
            game_placeTrackTile(game, n7, n6 + 2, 67, n3); game_placeTrackTile(game, n7 + 1, n6, 68, n3);
            break;
    }
}

namespace {

void beginMapGeneration(Game *game) {
        game->world.resize(game->world.terrain.mapWidth, game->world.terrain.mapHeight);
        game->world.initialIndustryPlaced = false;
        game->world.mapGenStage = mapgen::STAGE_INIT;
        game->world.tilesGeneratedInStage = 0;
        game->world.totalTilesToGenerate = game->world.terrainRaiseStepCount + game->world.terrainLowerStepCount + game->world.mapGenColumns * (game->world.mapGenColumns - 1)
            + game->world.mapGenRows + game->world.weightAccel2 + game->world.weightAccel + game->world.terrainFeatureStage6Count
            + game->world.terrainFeatureStage7Count + game->world.weightAccel4 + game->world.weightAccel3 + 1;
        game->world.mapGenTilesDone = 0;
}

void advanceMapGenerationStage(Game *game) {
        if (game->world.mapGenStage == mapgen::STAGE_INIT) { game->world.mapGenStage = mapgen::STAGE_RAISE_TERRAIN; game->world.tilesGeneratedInStage = 0; }
        if (game->world.mapGenStage == mapgen::STAGE_RAISE_TERRAIN && game->world.tilesGeneratedInStage == game->world.terrainRaiseStepCount) { game->world.mapGenStage = mapgen::STAGE_LOWER_TERRAIN; game->world.tilesGeneratedInStage = 0; game_rebuildTerrainTiles(game, false); }
        if (game->world.mapGenStage == mapgen::STAGE_LOWER_TERRAIN && game->world.tilesGeneratedInStage == game->world.terrainLowerStepCount) { game->world.mapGenStage = mapgen::STAGE_PLACE_PATCHES; game->world.tilesGeneratedInStage = 0; game_rebuildTerrainTiles(game, false); }
        if (game->world.mapGenStage == mapgen::STAGE_PLACE_PATCHES && game->world.tilesGeneratedInStage == game->world.mapGenColumns * (game->world.mapGenColumns - 1) + game->world.mapGenRows) { game->world.mapGenStage = mapgen::STAGE_FEATURE_0; game->world.tilesGeneratedInStage = 0; }
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_0 && game->world.tilesGeneratedInStage == game->world.weightAccel2) { game->world.mapGenStage = mapgen::STAGE_FEATURE_1; game->world.tilesGeneratedInStage = 0; }
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_1 && game->world.tilesGeneratedInStage == game->world.weightAccel) { game->world.mapGenStage = mapgen::STAGE_FEATURE_2; game->world.tilesGeneratedInStage = 0; }
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_2 && game->world.tilesGeneratedInStage == game->world.terrainFeatureStage6Count) { game->world.mapGenStage = mapgen::STAGE_FEATURE_3; game->world.tilesGeneratedInStage = 0; }
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_3 && game->world.tilesGeneratedInStage == game->world.terrainFeatureStage7Count) { game->world.mapGenStage = mapgen::STAGE_FEATURE_4; game->world.tilesGeneratedInStage = 0; }
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_4 && game->world.tilesGeneratedInStage == game->world.weightAccel4) { game->world.mapGenStage = mapgen::STAGE_FEATURE_5; game->world.tilesGeneratedInStage = 0; }
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_5 && game->world.tilesGeneratedInStage == game->world.weightAccel3) { game->world.mapGenStage = mapgen::STAGE_SCATTER_OBSTACLES; game->world.tilesGeneratedInStage = 0; }
}

void generateTerrainHeightStep(Game *game) {
        if (game->world.mapGenStage == mapgen::STAGE_RAISE_TERRAIN) {
            int center_x = abs(nextRngInt(game) % game->world.terrain.mapWidth);
            int center_y = abs(nextRngInt(game) % game->world.terrain.mapHeight);
            int width = abs(nextRngInt(game) % 20) + 1;
            int height = abs(nextRngInt(game) % 20) + 1;
            int elevation_step = abs(nextRngInt(game) % 2) + 1;
            game_raiseTerrainArea(game, center_x, center_y, width, height,
                                elevation_step);
        }
        if (game->world.mapGenStage == mapgen::STAGE_LOWER_TERRAIN) {
            int center_x = abs(nextRngInt(game) % game->world.terrain.mapWidth);
            int center_y = abs(nextRngInt(game) % game->world.terrain.mapHeight);
            int width = abs(nextRngInt(game) % 20) + 1;
            int height = abs(nextRngInt(game) % 20) + 1;
            game_lowerTerrainArea(game, center_x, center_y, width, height, -1);
        }
}

void generateTerrainPatchStep(Game *game) {
        game->build.trackBuildInProgress = false;
        if (game->world.mapGenStage == mapgen::STAGE_PLACE_PATCHES) {
            for (int column = 0; column < game->world.mapGenColumns; ++column) {
                bool done = false;
                for (int row = 0; row < game->world.mapGenRows && !done; ++row) {
                    if (game->world.tilesGeneratedInStage != game->world.mapGenColumns * column + row) continue;
                    int cellWidth = game->world.terrain.mapWidth / game->world.mapGenColumns;
                    int cellHeight = game->world.terrain.mapHeight / game->world.mapGenRows;
                    int patchX = column * cellWidth + abs(nextRngInt(game) % (cellWidth / 2));
                    int patchY = row * cellHeight + abs(nextRngInt(game) % (cellHeight / 2));
                    game_placeTerrainPatch(game, patchX, patchY);
                    done = true;
                }
                if (done) break;
            }
        }
}

void generateTerrainFeatureStep(Game *game) {
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_0) game_placeTerrainFeature(game, 0);
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_1) game_placeTerrainFeature(game, 1);
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_2) game_placeTerrainFeature(game, 2);
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_3) game_placeTerrainFeature(game, 3);
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_4) game_placeTerrainFeature(game, 4);
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_5) game_placeTerrainFeature(game, 5);
        if (game->build.trackBuildInProgress) game->stateWorkStep = 0;
}

void scatterInitialObstacles(Game *game) {
        if (game->world.mapGenStage == mapgen::STAGE_SCATTER_OBSTACLES) {
            for (int obstacle = 0; obstacle < game->world.initialObstacleCount; ++obstacle) {
                int mapX = abs(nextRngInt(game) % game->world.terrain.mapWidth);
                int mapY = abs(nextRngInt(game) % game->world.terrain.mapHeight);
                if (game->world.terrain.tileMap[mapX][mapY] != 0 || game->world.terrain.tileElevation[mapX][mapY] < 0) continue;
                game->world.terrain.tileMap[mapX][mapY] = (int8_t)(tile::SCENERY_FIRST + abs(nextRngInt(game) % 5));
            }
            game->world.initialIndustryPlaced = true;
        }
}

void generateOneMapStep(Game *game) {
        advanceMapGenerationStage(game);
        generateTerrainHeightStep(game);
        generateTerrainPatchStep(game);
        generateTerrainFeatureStep(game);
        scatterInitialObstacles(game);
        ++game->world.tilesGeneratedInStage;
        ++game->world.mapGenTilesDone;
}

} // namespace

// Set up the map generator (mode 0), or run one ordered generation step (mode 1).
void game_runMapGenerationStep(Game *game, int32_t mode) {
    if (mode == 0) beginMapGeneration(game);
    if (mode == 1) generateOneMapStep(game);
}

void game_pickCitySite(Game *game) {
    int x = abs(nextRngInt(game) % game->world.terrain.mapWidth);
    int y = abs(nextRngInt(game) % game->world.terrain.mapHeight);
    game->world.industryCategory = -1;
    int tile = game->world.terrain.tileMap[x][y];
    if (tile != 0 && (tile < tile::SCENERY_FIRST || tile > tile::SCENERY_LAST)) return;
    int min_x = x - 2;
    int min_y = y - 2;
    int max_x = x + 2;
    int max_y = y + 2;
    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    if (max_x >= game->world.terrain.mapWidth) max_x = game->world.terrain.mapWidth - 1;
    if (max_y >= game->world.terrain.mapHeight) max_y = game->world.terrain.mapHeight - 1;
    for (int near_x = min_x;
         near_x <= max_x && game->world.industryCategory <= 0; ++near_x) {
        for (int near_y = min_y;
             near_y <= max_y && game->world.industryCategory <= 0; ++near_y) {
            tile = game->world.terrain.tileMap[near_x][near_y];
            if (tile >= 34 && tile <= 40) game->world.industryCategory = 0;
            if (tile >= 65 && tile <= 68) game->world.industryCategory = 1;
            if (tile >= 53 && tile <= 55) game->world.industryCategory = 2;
            if (tile >= 59 && tile <= 61) game->world.industryCategory = 3;
        }
    }
    if (game->world.industryCategory >= 0) {
        game->world.cityStationX = x;
        game->world.cityStationY = y;
    }
}

void game_pickIndustrySite(Game *game) {
    int x = abs(nextRngInt(game) % game->world.terrain.mapWidth);
    int y = abs(nextRngInt(game) % game->world.terrain.mapHeight);
    game->world.industrySiteFound = false;
    int tile = game->world.terrain.tileMap[x][y];
    if (tile != 0 && (tile < tile::SCENERY_FIRST || tile > tile::SCENERY_LAST)) return;
    int min_x = x - 2;
    int min_y = y - 2;
    int max_x = x + 2;
    int max_y = y + 2;
    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    if (max_x >= game->world.terrain.mapWidth) max_x = game->world.terrain.mapWidth - 1;
    if (max_y >= game->world.terrain.mapHeight) max_y = game->world.terrain.mapHeight - 1;
    for (int near_x = min_x;
         near_x <= max_x && !game->world.industrySiteFound; ++near_x) {
        for (int near_y = min_y;
             near_y <= max_y && !game->world.industrySiteFound; ++near_y) {
            tile = game->world.terrain.tileMap[near_x][near_y];
            if (tile >= 34 && tile <= 40 && game->world.industryCategory == 0)
                game->world.industrySiteFound = true;
            if (tile >= 62 && tile <= 64 && game->world.industryCategory == 1)
                game->world.industrySiteFound = true;
            if (tile >= 50 && tile <= 52 && game->world.industryCategory == 2)
                game->world.industrySiteFound = true;
            if (tile >= 56 && tile <= 58 && game->world.industryCategory == 3)
                game->world.industrySiteFound = true;
        }
    }
    if (game->world.industrySiteFound) {
        game->world.industryStationX = x;
        game->world.industryTileY = y;
    }
}

void game_spawnTrainAt(Game *game, int32_t x, int32_t y) {
    int engine_type = game->era / 3;
    int wagon_count = game->build.selectionCost / 8000 + 1;
    if (wagon_count > 5) wagon_count = 5;
    int cargo_type = 2;
    if (game->world.industryCategory == 1) cargo_type = 3;
    if (game->world.industryCategory == 2) cargo_type = 4;
    if (game->world.industryCategory == 3) cargo_type = 6;
    int train = game_allocateTrainSlot(
        game, x, y, engine_type, game->era,
        game->world.terrain.tileMap[x][y] == 87 ? 0 : 2, 8 - game->era);
    for (int wagon = 0; wagon < wagon_count; ++wagon)
        train = game_spawnTrainCar(game, train, cargo_type);
}

static bool can_extend_station_over(Game *game, int x, int y) {
    int tile = game->world.terrain.tileMap[x][y];
    return tile == 0 || (tile >= tile::SCENERY_FIRST && tile <= tile::SCENERY_LAST);
}

void game_autoExtendTrackStation(Game *game, int32_t x, int32_t y) {
    bool track_left = x > 0 && game->world.trackMap[x - 1][y] == track::PRESENT;
    bool track_right = x < game->world.terrain.mapWidth - 1 && game->world.trackMap[x + 1][y] == track::PRESENT;
    bool track_up = y > 0 && game->world.trackMap[x][y - 1] == track::PRESENT;
    bool track_down = y < game->world.terrain.mapHeight - 1 && game->world.trackMap[x][y + 1] == track::PRESENT;
    if (track_left || track_right) {
        game->world.terrain.tileMap[x][y] = 88;
        game->world.trackMap[x][y] = (int8_t)game->build.nextTrackPieceCode;
        if (track_right && x > 1
            && can_extend_station_over(game, x - 1, y)
            && can_extend_station_over(game, x - 2, y)) {
            game->world.terrain.tileMap[x][y] = 90;
            game->world.terrain.tileMap[x - 1][y] = 88;
            game->world.terrain.tileMap[x - 2][y] = 90;
            game->world.trackMap[x - 1][y] = (int8_t)game->build.nextTrackPieceCode;
            game->world.trackMap[x - 2][y] = (int8_t)game->build.nextTrackPieceCode;
        }
        if (track_left && x < game->world.terrain.mapWidth - 2
            && can_extend_station_over(game, x + 1, y)
            && can_extend_station_over(game, x + 2, y)) {
            game->world.terrain.tileMap[x][y] = 90;
            game->world.terrain.tileMap[x + 1][y] = 88;
            game->world.terrain.tileMap[x + 2][y] = 90;
            game->world.trackMap[x + 1][y] = (int8_t)game->build.nextTrackPieceCode;
            game->world.trackMap[x + 2][y] = (int8_t)game->build.nextTrackPieceCode;
        }
    }
    if (track_up || track_down) {
        game->world.terrain.tileMap[x][y] = 87;
        game->world.trackMap[x][y] = (int8_t)game->build.nextTrackPieceCode;
        if (track_up && y > 1
            && can_extend_station_over(game, x, y - 1)
            && can_extend_station_over(game, x, y - 2)) {
            game->world.terrain.tileMap[x][y] = 89;
            game->world.terrain.tileMap[x][y - 1] = 87;
            game->world.terrain.tileMap[x][y - 2] = 89;
            game->world.trackMap[x][y - 1] = (int8_t)game->build.nextTrackPieceCode;
            game->world.trackMap[x][y - 2] = (int8_t)game->build.nextTrackPieceCode;
        }
        // The original bytecode tests track_up here too (not track_down).
        if (track_up && y < game->world.terrain.mapHeight - 2
            && can_extend_station_over(game, x, y + 1)
            && can_extend_station_over(game, x, y + 2)) {
            game->world.terrain.tileMap[x][y] = 89;
            game->world.terrain.tileMap[x][y + 1] = 87;
            game->world.terrain.tileMap[x][y + 2] = 89;
            game->world.trackMap[x][y + 1] = (int8_t)game->build.nextTrackPieceCode;
            game->world.trackMap[x][y + 2] = (int8_t)game->build.nextTrackPieceCode;
        }
    }
}

void game_placeCitiesAndIndustry(Game *game) {
    int city_attempts = 0;
    int industry_attempts = 0;
    do {
        game_pickCitySite(game);
    } while (++city_attempts != 50 && game->world.industryCategory == -1);
    if (city_attempts >= 50) return;
    do {
        game_pickIndustrySite(game);
    } while (++industry_attempts != 50 && !game->world.industrySiteFound);
    if (industry_attempts >= 50) return;

    game->pathfinder.anchorX = game->world.cityStationX;
    game->pathfinder.anchorY = game->world.cityStationY;
    game->pathfinder.targetX = game->world.industryStationX;
    game->pathfinder.targetY = game->world.industryTileY;
    if (!game_tryBuildPath(game)) return;
    game_autoExtendTrackStation(game, game->world.cityStationX, game->world.cityStationY);
    game_autoExtendTrackStation(game, game->world.industryStationX, game->world.industryTileY);
    game_spawnTrainAt(game, game->world.cityStationX, game->world.cityStationY);
    game_setTrackOwnerCode(game, game->build.nextTrackPieceCode - track::PRESENT);
    ++game->build.nextTrackPieceCode;
    if (game->build.nextTrackPieceCode == track::AI_PIECE_WRAP) game->build.nextTrackPieceCode = track::AI_PIECE_FIRST;
}
