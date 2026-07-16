// Terrain and tile queries and changes.
#include "game_methods.h"
#include <stdlib.h>
#include <string.h>

// boolean isPlaceableTerrain(int n, int n2)
bool gc_isPlaceableTerrain(GameCanvas *gc, int32_t n, int32_t n2) {
    return gc->world.tileElevation[n][n2] >= 0
           && (gc->world.tileMap[n][n2] == 0
               || (gc->world.tileMap[n][n2] >= 29 && gc->world.tileMap[n][n2] <= 33))
           && gc->world.trackMap[n][n2] == 0;
}

// void placeTrackTile(int n, int n2, int n3, int n4)
void gc_placeTrackTile(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4) {
    gc->world.tileMap[n][n2] = (int8_t)n3;
    gc->world.trackMap[n][n2] = (int8_t)(0xC0 | n4);
}

// void clearBuildPathTile(int n, int n2)
void gc_clearBuildPathTile(GameCanvas *gc, int32_t n, int32_t n2) {
    if (n > 0 && n < gc->world.mapWidth && n2 > 0 && n2 < gc->world.mapHeight
        && ((gc->world.tileMap[n][n2] >= 7 && gc->world.tileMap[n][n2] <= 10)
            || (gc->world.tileMap[n][n2] >= 21 && gc->world.tileMap[n][n2] <= 24)
            || gc->world.tileMap[n][n2] == 70 || gc->world.tileMap[n][n2] == 71)
        && gc->pathDistanceGrid[n][n2] > 0) {
        gc->pathDistanceGrid[n][n2] = 0;
    }
}

// boolean isCurveTile(int n, int n2)
bool gc_isCurveTile(GameCanvas *gc, int32_t n, int32_t n2) {
    int8_t by = gc->world.tileMap[n][n2];
    if (by < 7 || by > 10) return false;
    bool bl = false;
    if (by == 7)  bl |= gc_isTrackStepInlined(gc, n, n2, 0, -1, 10);
    if (by == 10) bl |= gc_isTrackStepInlined(gc, n, n2, 0, 1, 7);
    if (by == 8)  bl |= gc_isTrackStepInlined(gc, n, n2, -1, 0, 9);
    if (by == 9)  bl |= gc_isTrackStepInlined(gc, n, n2, 1, 0, 8);
    return bl;
}

// boolean isCityOrStationTile(int n, int n2)
bool gc_isCityOrStationTile(GameCanvas *gc, int32_t n, int32_t n2) {
    if ((gc->world.tileMap[n][n2] >= 29 && gc->world.tileMap[n][n2] <= 33)
        || (gc->world.tileMap[n][n2] >= 70 && gc->world.tileMap[n][n2] <= 80 && gc->world.trackMap[n][n2] == 65)
        || (gc->world.tileMap[n][n2] == tile::CROSSING && (gc->world.trackMap[n][n2] & 0x47) == 65)
        || (gc->world.tileMap[n][n2] == tile::CROSSING && (gc->world.trackMap[n][n2] & 0x78) == 72)
        || ((gc->world.tileMap[n][n2] == 87 || gc->world.tileMap[n][n2] == 88
             || gc->world.tileMap[n][n2] == 89 || gc->world.tileMap[n][n2] == 90)
            && gc->world.trackMap[n][n2] == 65)) {
        int32_t n3 = gc_findServiceVehicleAt(gc, n, n2);
        for (int i = 0; i < 500; ++i) {
            if ((gc->trainFlags[i] & 0x80) != 128) continue;
            int32_t n4 = gc_byteToUnsignedInt(gc, gc->trainX[i]);
            int32_t n5 = gc_byteToUnsignedInt(gc, gc->trainY[i]);
            if (n == n4 && n2 == n5) return false;
            if (n3 == 5000 || n3 != gc_findServiceVehicleAt(gc, n4, n5)) continue;
            return false;
        }
        return true;
    }
    if (gc->world.tileMap[n][n2] >= 81 && gc->world.tileMap[n][n2] <= 86 && gc->world.trackMap[n][n2] == 65) {
        for (int i = 0; i < 500; ++i) {
            int32_t n6;
            if ((gc->trainFlags[i] & 0x80) != 128) continue;
            if (gc->world.tileMap[n][n2] == 81 || gc->world.tileMap[n][n2] == 84 || gc->world.tileMap[n][n2] == 85) {
                for (n6 = n; n6 < gc->world.mapWidth
                     && (gc->world.tileMap[n6][n2] == 81 || gc->world.tileMap[n6][n2] == 84
                         || gc->world.tileMap[n6][n2] == 85); ++n6) {
                    if (gc_byteToUnsignedInt(gc, gc->trainX[i]) != n6 || gc_byteToUnsignedInt(gc, gc->trainY[i]) != n2) continue;
                    return false;
                }
                for (n6 = n - 1; n6 >= 0
                     && (gc->world.tileMap[n6][n2] == 81 || gc->world.tileMap[n6][n2] == 84
                         || gc->world.tileMap[n6][n2] == 85); --n6) {
                    if (gc_byteToUnsignedInt(gc, gc->trainX[i]) != n6 || gc_byteToUnsignedInt(gc, gc->trainY[i]) != n2) continue;
                    return false;
                }
                continue;
            }
            for (n6 = n2; n6 < gc->world.mapHeight
                 && (gc->world.tileMap[n][n6] == 82 || gc->world.tileMap[n][n6] == 83
                     || gc->world.tileMap[n][n6] == 86); ++n6) {
                if (gc_byteToUnsignedInt(gc, gc->trainX[i]) != n || gc_byteToUnsignedInt(gc, gc->trainY[i]) != n6) continue;
                return false;
            }
            for (n6 = n2 - 1; n6 >= 0
                 && (gc->world.tileMap[n][n6] == 82 || gc->world.tileMap[n][n6] == 83
                     || gc->world.tileMap[n][n6] == 86); --n6) {
                if (gc_byteToUnsignedInt(gc, gc->trainX[i]) != n || gc_byteToUnsignedInt(gc, gc->trainY[i]) != n6) continue;
                return false;
            }
        }
        return true;
    }
    return false;
}

// boolean isTileBuildable(int,int,int)
bool gc_isTileBuildable(GameCanvas *gc, int32_t x, int32_t y, int32_t type) {
    if (type == 5)
        return gc_isTileBuildable(gc, x, y, 0)
               && gc_isTileBuildable(gc, x, y - 1, 0)
               && gc_isTileBuildable(gc, x, y + 1, 0);
    if (type == 6)
        return gc_isTileBuildable(gc, x, y, 0)
               && gc_isTileBuildable(gc, x - 1, y, 0)
               && gc_isTileBuildable(gc, x + 1, y, 0);
    if (x < 0 || y < 0 || x >= gc->world.mapWidth || y >= gc->world.mapHeight) return false;
    int tile = gc->world.tileMap[x][y];
    bool buildable = gc->world.tileElevation[x][y] >= 0
                     && (tile == 0 || (tile >= 29 && tile <= 33))
                     && gc->world.trackMap[x][y] == 0;
    if (type == 3 && (tile == 71 || tile == 21 || tile == 23)) buildable = true;
    if (type == 4 && (tile == 70 || tile == 22 || tile == 24)) buildable = true;
    return buildable;
}

// void commitTerrainEdit() -- copy the 10x10 edit buffer back into heightMap
void gc_commitTerrainEdit(GameCanvas *gc) {
    for (int i = 0; i < 10; ++i) {
        memcpy(gc->world.heightMap[gc->selectionStartX + i].data() + gc->selectionStartY,
               gc->terrainEditBuffer[i].data(), 10);
    }
}

// void setSelectionStart(int n, int n2)
void gc_setSelectionStart(GameCanvas *gc, int32_t n, int32_t n2) {
    gc->selectionStartX = n - 5;
    gc->selectionStartY = n2 - 5;
    if (gc->selectionStartX < 0) gc->selectionStartX = 0;
    if (gc->selectionStartY < 0) gc->selectionStartY = 0;
    if (gc->selectionStartX + 10 >= gc->world.mapWidth)  gc->selectionStartX = gc->world.mapWidth - 9;
    if (gc->selectionStartY + 10 >= gc->world.mapHeight) gc->selectionStartY = gc->world.mapHeight - 9;
    // this.terrainEditBuffer = new byte[10][10];
    gc->terrainEditBuffer.resize(10, 10);
    for (int i = 0; i < 10; ++i) {
        memcpy(gc->terrainEditBuffer[i].data(),
               gc->world.heightMap[gc->selectionStartX + i].data() + gc->selectionStartY, 10);
    }
    gc->terrainSmoothCost = 0;
}
