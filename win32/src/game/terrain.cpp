// Terrain and tile queries and changes.
#include "game_internal.h"
#include <stdlib.h>
#include <string.h>

bool game_isPlaceableTerrain(Game *game, int32_t n, int32_t n2) {
    return game->world.tileElevation[n][n2] >= 0
           && (game->world.tileMap[n][n2] == 0
               || (game->world.tileMap[n][n2] >= tile::SCENERY_FIRST && game->world.tileMap[n][n2] <= tile::SCENERY_LAST))
           && game->world.trackMap[n][n2] == 0;
}

void game_placeTrackTile(Game *game, int32_t n, int32_t n2, int32_t n3, int32_t n4) {
    game->world.tileMap[n][n2] = (int8_t)n3;
    game->world.trackMap[n][n2] = (int8_t)(0xC0 | n4);
}

void game_clearBuildPathTile(Game *game, int32_t n, int32_t n2) {
    if (n > 0 && n < game->world.mapWidth && n2 > 0 && n2 < game->world.mapHeight
        && ((game->world.tileMap[n][n2] >= 7 && game->world.tileMap[n][n2] <= 10)
            || (game->world.tileMap[n][n2] >= 21 && game->world.tileMap[n][n2] <= 24)
            || game->world.tileMap[n][n2] == 70 || game->world.tileMap[n][n2] == 71)
        && game->pathfinder.distanceGrid[n][n2] > 0) {
        game->pathfinder.distanceGrid[n][n2] = 0;
    }
}

bool game_isCurveTile(Game *game, int32_t n, int32_t n2) {
    int8_t by = game->world.tileMap[n][n2];
    if (by < 7 || by > 10) return false;
    bool bl = false;
    if (by == 7)  bl |= game_isTrackStep(game, n, n2, 0, -1, 10);
    if (by == 10) bl |= game_isTrackStep(game, n, n2, 0, 1, 7);
    if (by == 8)  bl |= game_isTrackStep(game, n, n2, -1, 0, 9);
    if (by == 9)  bl |= game_isTrackStep(game, n, n2, 1, 0, 8);
    return bl;
}

bool game_isCityOrStationTile(Game *game, int32_t n, int32_t n2) {
    if ((game->world.tileMap[n][n2] >= tile::SCENERY_FIRST && game->world.tileMap[n][n2] <= tile::SCENERY_LAST)
        || (game->world.tileMap[n][n2] >= 70 && game->world.tileMap[n][n2] <= 80 && game->world.trackMap[n][n2] == 65)
        || (game->world.tileMap[n][n2] == tile::CROSSING && (game->world.trackMap[n][n2] & 0x47) == 65)
        || (game->world.tileMap[n][n2] == tile::CROSSING && (game->world.trackMap[n][n2] & 0x78) == 72)
        || ((game->world.tileMap[n][n2] == 87 || game->world.tileMap[n][n2] == 88
             || game->world.tileMap[n][n2] == 89 || game->world.tileMap[n][n2] == 90)
            && game->world.trackMap[n][n2] == 65)) {
        int32_t n3 = game_findServiceVehicleAt(game, n, n2);
        for (int i = 0; i < 500; ++i) {
            if (!train_flag::isUsed(game->fleet.trainFlags[i])) continue;
            int32_t n4 = unsignedByte(game->fleet.trainX[i]);
            int32_t n5 = unsignedByte(game->fleet.trainY[i]);
            if (n == n4 && n2 == n5) return false;
            if (n3 == 5000 || n3 != game_findServiceVehicleAt(game, n4, n5)) continue;
            return false;
        }
        return true;
    }
    if (game->world.tileMap[n][n2] >= 81 && game->world.tileMap[n][n2] <= 86 && game->world.trackMap[n][n2] == 65) {
        for (int i = 0; i < 500; ++i) {
            int32_t n6;
            if (!train_flag::isUsed(game->fleet.trainFlags[i])) continue;
            if (game->world.tileMap[n][n2] == 81 || game->world.tileMap[n][n2] == 84 || game->world.tileMap[n][n2] == 85) {
                for (n6 = n; n6 < game->world.mapWidth
                     && (game->world.tileMap[n6][n2] == 81 || game->world.tileMap[n6][n2] == 84
                         || game->world.tileMap[n6][n2] == 85); ++n6) {
                    if (unsignedByte(game->fleet.trainX[i]) != n6 || unsignedByte(game->fleet.trainY[i]) != n2) continue;
                    return false;
                }
                for (n6 = n - 1; n6 >= 0
                     && (game->world.tileMap[n6][n2] == 81 || game->world.tileMap[n6][n2] == 84
                         || game->world.tileMap[n6][n2] == 85); --n6) {
                    if (unsignedByte(game->fleet.trainX[i]) != n6 || unsignedByte(game->fleet.trainY[i]) != n2) continue;
                    return false;
                }
                continue;
            }
            for (n6 = n2; n6 < game->world.mapHeight
                 && (game->world.tileMap[n][n6] == 82 || game->world.tileMap[n][n6] == 83
                     || game->world.tileMap[n][n6] == 86); ++n6) {
                if (unsignedByte(game->fleet.trainX[i]) != n || unsignedByte(game->fleet.trainY[i]) != n6) continue;
                return false;
            }
            for (n6 = n2 - 1; n6 >= 0
                 && (game->world.tileMap[n][n6] == 82 || game->world.tileMap[n][n6] == 83
                     || game->world.tileMap[n][n6] == 86); --n6) {
                if (unsignedByte(game->fleet.trainX[i]) != n || unsignedByte(game->fleet.trainY[i]) != n6) continue;
                return false;
            }
        }
        return true;
    }
    return false;
}

bool game_isTileBuildable(Game *game, int32_t x, int32_t y, int32_t type) {
    if (type == 5)
        return game_isTileBuildable(game, x, y, 0)
               && game_isTileBuildable(game, x, y - 1, 0)
               && game_isTileBuildable(game, x, y + 1, 0);
    if (type == 6)
        return game_isTileBuildable(game, x, y, 0)
               && game_isTileBuildable(game, x - 1, y, 0)
               && game_isTileBuildable(game, x + 1, y, 0);
    if (!game->world.in_bounds(x, y)) return false;
    int tile = game->world.tileMap[x][y];
    bool buildable = game->world.tileElevation[x][y] >= 0
                     && (tile == 0 || (tile >= tile::SCENERY_FIRST && tile <= tile::SCENERY_LAST))
                     && game->world.trackMap[x][y] == 0;
    if (type == 3 && (tile == 71 || tile == 21 || tile == 23)) buildable = true;
    if (type == 4 && (tile == 70 || tile == 22 || tile == 24)) buildable = true;
    return buildable;
}

// void commitTerrainEdit() -- copy the 10x10 edit buffer back into heightMap
void game_commitTerrainEdit(Game *game) {
    for (int i = 0; i < 10; ++i) {
        memcpy(game->world.heightMap[game->build.selectionStartX + i].data() + game->build.selectionStartY,
               game->build.terrainEditBuffer[i].data(), 10);
    }
}

void game_setSelectionStart(Game *game, int32_t n, int32_t n2) {
    game->build.selectionStartX = n - 5;
    game->build.selectionStartY = n2 - 5;
    if (game->build.selectionStartX < 0) game->build.selectionStartX = 0;
    if (game->build.selectionStartY < 0) game->build.selectionStartY = 0;
    if (game->build.selectionStartX + 10 >= game->world.mapWidth)  game->build.selectionStartX = game->world.mapWidth - 9;
    if (game->build.selectionStartY + 10 >= game->world.mapHeight) game->build.selectionStartY = game->world.mapHeight - 9;
    // this.terrainEditBuffer = new byte[10][10];
    game->build.terrainEditBuffer.resize(10, 10);
    for (int i = 0; i < 10; ++i) {
        memcpy(game->build.terrainEditBuffer[i].data(),
               game->world.heightMap[game->build.selectionStartX + i].data() + game->build.selectionStartY, 10);
    }
    game->build.terrainSmoothCost = 0;
}
