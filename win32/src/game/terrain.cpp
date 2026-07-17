// Terrain and tile queries and changes.
#include "game_internal.h"
#include <stdlib.h>
#include <string.h>

bool World::is_placeable_terrain(int32_t x, int32_t y) const {
    return terrain.tileElevation[x][y] >= 0
           && (terrain.tileMap[x][y] == 0
               || (terrain.tileMap[x][y] >= tile::SCENERY_FIRST
                   && terrain.tileMap[x][y] <= tile::SCENERY_LAST))
           && trackMap[x][y] == 0;
}

void World::place_track_tile(int32_t x, int32_t y, int32_t tile_code,
                             int32_t track_owner) {
    terrain.tileMap[x][y] = (int8_t)tile_code;
    trackMap[x][y] = (int8_t)(0xC0 | track_owner);
}

void game_clearBuildPathTile(Game *game, int32_t n, int32_t n2) {
    if (n > 0 && n < game->world.terrain.mapWidth && n2 > 0 && n2 < game->world.terrain.mapHeight
        && ((game->world.terrain.tileMap[n][n2] >= 7 && game->world.terrain.tileMap[n][n2] <= 10)
            || (game->world.terrain.tileMap[n][n2] >= 21 && game->world.terrain.tileMap[n][n2] <= 24)
            || game->world.terrain.tileMap[n][n2] == 70 || game->world.terrain.tileMap[n][n2] == 71)
        && game->pathfinder.distanceGrid[n][n2] > 0) {
        game->pathfinder.distanceGrid[n][n2] = 0;
    }
}

bool game_isCurveTile(Game *game, int32_t n, int32_t n2) {
    int8_t by = game->world.terrain.tileMap[n][n2];
    if (by < 7 || by > 10) return false;
    bool bl = false;
    if (by == 7)  bl |= game_isTrackStep(game, n, n2, 0, -1, 10);
    if (by == 10) bl |= game_isTrackStep(game, n, n2, 0, 1, 7);
    if (by == 8)  bl |= game_isTrackStep(game, n, n2, -1, 0, 9);
    if (by == 9)  bl |= game_isTrackStep(game, n, n2, 1, 0, 8);
    return bl;
}

bool game_isCityOrStationTile(Game *game, int32_t n, int32_t n2) {
    if ((game->world.terrain.tileMap[n][n2] >= tile::SCENERY_FIRST && game->world.terrain.tileMap[n][n2] <= tile::SCENERY_LAST)
        || (game->world.terrain.tileMap[n][n2] >= 70 && game->world.terrain.tileMap[n][n2] <= 80 && game->world.trackMap[n][n2] == 65)
        || (game->world.terrain.tileMap[n][n2] == tile::CROSSING && (game->world.trackMap[n][n2] & 0x47) == 65)
        || (game->world.terrain.tileMap[n][n2] == tile::CROSSING && (game->world.trackMap[n][n2] & 0x78) == 72)
        || ((game->world.terrain.tileMap[n][n2] == 87 || game->world.terrain.tileMap[n][n2] == 88
             || game->world.terrain.tileMap[n][n2] == 89 || game->world.terrain.tileMap[n][n2] == 90)
            && game->world.trackMap[n][n2] == 65)) {
        int32_t n3 = game_findServiceVehicleAt(game, n, n2);
        for (int i = 0; i < 500; ++i) {
            if (!train_flag::isUsed(game->fleet.trainCars[i].flags)) continue;
            int32_t n4 = unsignedByte(game->fleet.trainCars[i].x);
            int32_t n5 = unsignedByte(game->fleet.trainCars[i].y);
            if (n == n4 && n2 == n5) return false;
            if (n3 == 5000 || n3 != game_findServiceVehicleAt(game, n4, n5)) continue;
            return false;
        }
        return true;
    }
    if (game->world.terrain.tileMap[n][n2] >= 81 && game->world.terrain.tileMap[n][n2] <= 86 && game->world.trackMap[n][n2] == 65) {
        for (int i = 0; i < 500; ++i) {
            int32_t n6;
            if (!train_flag::isUsed(game->fleet.trainCars[i].flags)) continue;
            if (game->world.terrain.tileMap[n][n2] == 81 || game->world.terrain.tileMap[n][n2] == 84 || game->world.terrain.tileMap[n][n2] == 85) {
                for (n6 = n; n6 < game->world.terrain.mapWidth
                     && (game->world.terrain.tileMap[n6][n2] == 81 || game->world.terrain.tileMap[n6][n2] == 84
                         || game->world.terrain.tileMap[n6][n2] == 85); ++n6) {
                    if (unsignedByte(game->fleet.trainCars[i].x) != n6 || unsignedByte(game->fleet.trainCars[i].y) != n2) continue;
                    return false;
                }
                for (n6 = n - 1; n6 >= 0
                     && (game->world.terrain.tileMap[n6][n2] == 81 || game->world.terrain.tileMap[n6][n2] == 84
                         || game->world.terrain.tileMap[n6][n2] == 85); --n6) {
                    if (unsignedByte(game->fleet.trainCars[i].x) != n6 || unsignedByte(game->fleet.trainCars[i].y) != n2) continue;
                    return false;
                }
                continue;
            }
            for (n6 = n2; n6 < game->world.terrain.mapHeight
                 && (game->world.terrain.tileMap[n][n6] == 82 || game->world.terrain.tileMap[n][n6] == 83
                     || game->world.terrain.tileMap[n][n6] == 86); ++n6) {
                if (unsignedByte(game->fleet.trainCars[i].x) != n || unsignedByte(game->fleet.trainCars[i].y) != n6) continue;
                return false;
            }
            for (n6 = n2 - 1; n6 >= 0
                 && (game->world.terrain.tileMap[n][n6] == 82 || game->world.terrain.tileMap[n][n6] == 83
                     || game->world.terrain.tileMap[n][n6] == 86); --n6) {
                if (unsignedByte(game->fleet.trainCars[i].x) != n || unsignedByte(game->fleet.trainCars[i].y) != n6) continue;
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
    int tile = game->world.terrain.tileMap[x][y];
    bool buildable = game->world.terrain.tileElevation[x][y] >= 0
                     && (tile == 0 || (tile >= tile::SCENERY_FIRST && tile <= tile::SCENERY_LAST))
                     && game->world.trackMap[x][y] == 0;
    if (type == 3 && (tile == 71 || tile == 21 || tile == 23)) buildable = true;
    if (type == 4 && (tile == 70 || tile == 22 || tile == 24)) buildable = true;
    return buildable;
}

// void commitTerrainEdit() -- copy the 10x10 edit buffer back into heightMap
void Terrain::commit_edit(const BuildController& build) {
    for (int i = 0; i < 10; ++i) {
        memcpy(heightMap[build.selectionStartX + i].data() + build.selectionStartY,
               build.terrainEditBuffer[i].data(), 10);
    }
}

void game_setSelectionStart(Game *game, int32_t n, int32_t n2) {
    game->build.selectionStartX = n - 5;
    game->build.selectionStartY = n2 - 5;
    if (game->build.selectionStartX < 0) game->build.selectionStartX = 0;
    if (game->build.selectionStartY < 0) game->build.selectionStartY = 0;
    if (game->build.selectionStartX + 10 >= game->world.terrain.mapWidth)  game->build.selectionStartX = game->world.terrain.mapWidth - 9;
    if (game->build.selectionStartY + 10 >= game->world.terrain.mapHeight) game->build.selectionStartY = game->world.terrain.mapHeight - 9;
    // this.terrainEditBuffer = new byte[10][10];
    game->build.terrainEditBuffer.resize(10, 10);
    for (int i = 0; i < 10; ++i) {
        memcpy(game->build.terrainEditBuffer[i].data(),
               game->world.terrain.heightMap[game->build.selectionStartX + i].data() + game->build.selectionStartY, 10);
    }
    game->build.terrainSmoothCost = 0;
}
