// Terrain and tile queries and changes.
#include "game_internal.h"
#include "support/numeric.h"
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

bool World::is_curve_tile(int32_t n, int32_t n2) const {
    int8_t by = terrain.tileMap[n][n2];
    if (by < 7 || by > 10) return false;
    bool bl = false;
    if (by == 7)  bl |= has_track_step(n, n2, 0, -1, 10);
    if (by == 10) bl |= has_track_step(n, n2, 0, 1, 7);
    if (by == 8)  bl |= has_track_step(n, n2, -1, 0, 9);
    if (by == 9)  bl |= has_track_step(n, n2, 1, 0, 8);
    return bl;
}

bool World::is_city_or_station_tile(const Fleet& fleet, int32_t n,
                                    int32_t n2) const {
    if ((terrain.tileMap[n][n2] >= tile::SCENERY_FIRST && terrain.tileMap[n][n2] <= tile::SCENERY_LAST)
        || (terrain.tileMap[n][n2] >= 70 && terrain.tileMap[n][n2] <= 80 && trackMap[n][n2] == 65)
        || (terrain.tileMap[n][n2] == tile::CROSSING && (trackMap[n][n2] & 0x47) == 65)
        || (terrain.tileMap[n][n2] == tile::CROSSING && (trackMap[n][n2] & 0x78) == 72)
        || ((terrain.tileMap[n][n2] == 87 || terrain.tileMap[n][n2] == 88
             || terrain.tileMap[n][n2] == 89 || terrain.tileMap[n][n2] == 90)
            && trackMap[n][n2] == 65)) {
        int32_t n3 = fleet.find_service_site_at(n, n2);
        for (int i = 0; i < 500; ++i) {
            if (!train_flag::isUsed(fleet.trainCars[i].flags)) continue;
            int32_t n4 = unsigned_byte(fleet.trainCars[i].x);
            int32_t n5 = unsigned_byte(fleet.trainCars[i].y);
            if (n == n4 && n2 == n5) return false;
            if (n3 == 5000 || n3 != fleet.find_service_site_at(n4, n5)) continue;
            return false;
        }
        return true;
    }
    if (terrain.tileMap[n][n2] >= 81 && terrain.tileMap[n][n2] <= 86 && trackMap[n][n2] == 65) {
        for (int i = 0; i < 500; ++i) {
            int32_t n6;
            if (!train_flag::isUsed(fleet.trainCars[i].flags)) continue;
            if (terrain.tileMap[n][n2] == 81 || terrain.tileMap[n][n2] == 84 || terrain.tileMap[n][n2] == 85) {
                for (n6 = n; n6 < terrain.mapWidth
                     && (terrain.tileMap[n6][n2] == 81 || terrain.tileMap[n6][n2] == 84
                         || terrain.tileMap[n6][n2] == 85); ++n6) {
                    if (unsigned_byte(fleet.trainCars[i].x) != n6 || unsigned_byte(fleet.trainCars[i].y) != n2) continue;
                    return false;
                }
                for (n6 = n - 1; n6 >= 0
                     && (terrain.tileMap[n6][n2] == 81 || terrain.tileMap[n6][n2] == 84
                         || terrain.tileMap[n6][n2] == 85); --n6) {
                    if (unsigned_byte(fleet.trainCars[i].x) != n6 || unsigned_byte(fleet.trainCars[i].y) != n2) continue;
                    return false;
                }
                continue;
            }
            for (n6 = n2; n6 < terrain.mapHeight
                 && (terrain.tileMap[n][n6] == 82 || terrain.tileMap[n][n6] == 83
                     || terrain.tileMap[n][n6] == 86); ++n6) {
                if (unsigned_byte(fleet.trainCars[i].x) != n || unsigned_byte(fleet.trainCars[i].y) != n6) continue;
                return false;
            }
            for (n6 = n2 - 1; n6 >= 0
                 && (terrain.tileMap[n][n6] == 82 || terrain.tileMap[n][n6] == 83
                     || terrain.tileMap[n][n6] == 86); --n6) {
                if (unsigned_byte(fleet.trainCars[i].x) != n || unsigned_byte(fleet.trainCars[i].y) != n6) continue;
                return false;
            }
        }
        return true;
    }
    return false;
}

bool World::is_tile_buildable(int32_t x, int32_t y, int32_t type) const {
    if (type == 5)
        return is_tile_buildable(x, y, 0)
               && is_tile_buildable(x, y - 1, 0)
               && is_tile_buildable(x, y + 1, 0);
    if (type == 6)
        return is_tile_buildable(x, y, 0)
               && is_tile_buildable(x - 1, y, 0)
               && is_tile_buildable(x + 1, y, 0);
    if (!in_bounds(x, y)) return false;
    int tile = terrain.tileMap[x][y];
    bool buildable = terrain.tileElevation[x][y] >= 0
                     && (tile == 0 || (tile >= tile::SCENERY_FIRST && tile <= tile::SCENERY_LAST))
                     && trackMap[x][y] == 0;
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

void BuildController::set_terrain_selection(const Terrain& terrain, int32_t n,
                                            int32_t n2) {
    selectionStartX = n - 5;
    selectionStartY = n2 - 5;
    if (selectionStartX < 0) selectionStartX = 0;
    if (selectionStartY < 0) selectionStartY = 0;
    if (selectionStartX + 10 >= terrain.mapWidth) selectionStartX = terrain.mapWidth - 9;
    if (selectionStartY + 10 >= terrain.mapHeight) selectionStartY = terrain.mapHeight - 9;
    // this.terrainEditBuffer = new byte[10][10];
    terrainEditBuffer.resize(10, 10);
    for (int i = 0; i < 10; ++i) {
        memcpy(terrainEditBuffer[i].data(),
               terrain.heightMap[selectionStartX + i].data() + selectionStartY,
               10);
    }
    terrainSmoothCost = 0;
}
