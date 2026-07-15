// port/impl/terrain.c - terrain / tile query + edit methods (hand-ported).
#include "game_methods.h"
#include <stdlib.h>
#include <string.h>

// boolean isPlaceableTerrain(int n, int n2)
bool gc_isPlaceableTerrain(GameCanvas *gc, int32_t n, int32_t n2) {
    return gc->tileElevation[n][n2] >= 0
           && (gc->tileMap[n][n2] == 0
               || (gc->tileMap[n][n2] >= 29 && gc->tileMap[n][n2] <= 33))
           && gc->trackMap[n][n2] == 0;
}

// void placeTrackTile(int n, int n2, int n3, int n4)
void gc_placeTrackTile(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4) {
    gc->tileMap[n][n2] = (int8_t)n3;
    gc->trackMap[n][n2] = (int8_t)(0xC0 | n4);
}

// void clearBuildPathTile(int n, int n2)
void gc_clearBuildPathTile(GameCanvas *gc, int32_t n, int32_t n2) {
    if (n > 0 && n < gc->mapWidth && n2 > 0 && n2 < gc->mapHeight
        && ((gc->tileMap[n][n2] >= 7 && gc->tileMap[n][n2] <= 10)
            || (gc->tileMap[n][n2] >= 21 && gc->tileMap[n][n2] <= 24)
            || gc->tileMap[n][n2] == 70 || gc->tileMap[n][n2] == 71)
        && gc->pathDistanceGrid[n][n2] > 0) {
        gc->pathDistanceGrid[n][n2] = 0;
    }
}

// boolean isCurveTile(int n, int n2)
bool gc_isCurveTile(GameCanvas *gc, int32_t n, int32_t n2) {
    int8_t by = gc->tileMap[n][n2];
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
    if ((gc->tileMap[n][n2] >= 29 && gc->tileMap[n][n2] <= 33)
        || (gc->tileMap[n][n2] >= 70 && gc->tileMap[n][n2] <= 80 && gc->trackMap[n][n2] == 65)
        || (gc->tileMap[n][n2] == 76 && (gc->trackMap[n][n2] & 0x47) == 65)
        || (gc->tileMap[n][n2] == 76 && (gc->trackMap[n][n2] & 0x78) == 72)
        || ((gc->tileMap[n][n2] == 87 || gc->tileMap[n][n2] == 88
             || gc->tileMap[n][n2] == 89 || gc->tileMap[n][n2] == 90)
            && gc->trackMap[n][n2] == 65)) {
        int32_t n3 = gc_findServiceVehicleAt(gc, n, n2);
        for (int i = 0; i < 500; ++i) {
            if ((gc->trainFlags[i] & 0x80) != 128) continue;
            int32_t n4 = gc_c(gc, gc->trainX[i]);
            int32_t n5 = gc_c(gc, gc->trainY[i]);
            if (n == n4 && n2 == n5) return false;
            if (n3 == 5000 || n3 != gc_findServiceVehicleAt(gc, n4, n5)) continue;
            return false;
        }
        return true;
    }
    if (gc->tileMap[n][n2] >= 81 && gc->tileMap[n][n2] <= 86 && gc->trackMap[n][n2] == 65) {
        for (int i = 0; i < 500; ++i) {
            int32_t n6;
            if ((gc->trainFlags[i] & 0x80) != 128) continue;
            if (gc->tileMap[n][n2] == 81 || gc->tileMap[n][n2] == 84 || gc->tileMap[n][n2] == 85) {
                for (n6 = n; n6 < gc->mapWidth
                     && (gc->tileMap[n6][n2] == 81 || gc->tileMap[n6][n2] == 84
                         || gc->tileMap[n6][n2] == 85); ++n6) {
                    if (gc_c(gc, gc->trainX[i]) != n6 || gc_c(gc, gc->trainY[i]) != n2) continue;
                    return false;
                }
                for (n6 = n - 1; n6 >= 0
                     && (gc->tileMap[n6][n2] == 81 || gc->tileMap[n6][n2] == 84
                         || gc->tileMap[n6][n2] == 85); --n6) {
                    if (gc_c(gc, gc->trainX[i]) != n6 || gc_c(gc, gc->trainY[i]) != n2) continue;
                    return false;
                }
                continue;
            }
            for (n6 = n2; n6 < gc->mapHeight
                 && (gc->tileMap[n][n6] == 82 || gc->tileMap[n][n6] == 83
                     || gc->tileMap[n][n6] == 86); ++n6) {
                if (gc_c(gc, gc->trainX[i]) != n || gc_c(gc, gc->trainY[i]) != n6) continue;
                return false;
            }
            for (n6 = n2 - 1; n6 >= 0
                 && (gc->tileMap[n][n6] == 82 || gc->tileMap[n][n6] == 83
                     || gc->tileMap[n][n6] == 86); --n6) {
                if (gc_c(gc, gc->trainX[i]) != n || gc_c(gc, gc->trainY[i]) != n6) continue;
                return false;
            }
        }
        return true;
    }
    return false;
}

// void commitTerrainEdit() -- copy the 10x10 edit buffer back into heightMap
void gc_commitTerrainEdit(GameCanvas *gc) {
    for (int i = 0; i < 10; ++i) {
        memcpy(gc->heightMap[gc->selectionStartX + i] + gc->selectionStartY,
               gc->terrainEditBuffer[i], 10);
    }
}

// void setSelectionStart(int n, int n2)
void gc_setSelectionStart(GameCanvas *gc, int32_t n, int32_t n2) {
    gc->selectionStartX = n - 5;
    gc->selectionStartY = n2 - 5;
    if (gc->selectionStartX < 0) gc->selectionStartX = 0;
    if (gc->selectionStartY < 0) gc->selectionStartY = 0;
    if (gc->selectionStartX + 10 >= gc->mapWidth)  gc->selectionStartX = gc->mapWidth - 9;
    if (gc->selectionStartY + 10 >= gc->mapHeight) gc->selectionStartY = gc->mapHeight - 9;
    // this.terrainEditBuffer = new byte[10][10];
    if (gc->terrainEditBuffer) {
        for (int i = 0; i < 10; ++i) free(gc->terrainEditBuffer[i]);
        free(gc->terrainEditBuffer);
    }
    gc->terrainEditBuffer = (int8_t **)malloc(10 * sizeof(int8_t *));
    for (int i = 0; i < 10; ++i)
        gc->terrainEditBuffer[i] = (int8_t *)calloc(10, sizeof(int8_t));
    for (int i = 0; i < 10; ++i) {
        memcpy(gc->terrainEditBuffer[i],
               gc->heightMap[gc->selectionStartX + i] + gc->selectionStartY, 10);
    }
    gc->terrainSmoothCost = 0;
}
