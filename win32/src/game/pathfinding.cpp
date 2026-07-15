// Build-path search, validation, and preview tiles.
#include "game_methods.h"

#include <limits.h>

int16_t gc_pathDistanceMin(GameCanvas *gc, int32_t x, int32_t y,
                           int32_t current_min) {
    int16_t result = (int16_t)current_min;
    if (x >= 0 && x < gc->mapWidth && y >= 0 && y < gc->mapHeight
        && gc->pathDistanceGrid[x][y] >= 20
        && gc->pathDistanceGrid[x][y] < result)
        result = gc->pathDistanceGrid[x][y];
    return result;
}

int32_t gc_relaxPathCell(GameCanvas *gc, int32_t x, int32_t y) {
    bool changed_or_blocked = false;
    int16_t cell = gc->pathDistanceGrid[x][y];
    if (cell > 0 && cell != 3 && cell != 7) {
        int previous = cell < 20 ? SHRT_MAX : cell;
        int distance = gc_pathDistanceMin(gc, x - 1, y, previous);
        distance = gc_pathDistanceMin(gc, x + 1, y, distance);
        distance = gc_pathDistanceMin(gc, x, y - 1, distance);
        distance = gc_pathDistanceMin(gc, x, y + 1, distance);
        if (distance < previous) {
            distance = (int16_t)(distance + 1);
            if (cell == 2) {
                int step_x = 0;
                int step_y = 0;
                int tunnel_x = x;
                int tunnel_y = y;
                if (gc->tileMap[x][y] == 7) step_y = -1;
                if (gc->tileMap[x][y] == 8) step_x = -1;
                if (gc->tileMap[x][y] == 9) step_x = 1;
                if (gc->tileMap[x][y] == 10) step_y = 1;
                if (step_x != 0 || step_y != 0) {
                    int tunnel_distance = distance;
                    do {
                        tunnel_distance = (int16_t)(tunnel_distance + 1);
                        tunnel_x += step_x;
                        tunnel_y += step_y;
                    } while (gc->pathDistanceGrid[tunnel_x][tunnel_y] == 3);
                    gc->pathDistanceGrid[tunnel_x][tunnel_y] =
                        (int16_t)tunnel_distance;
                    changed_or_blocked = true;
                }
            }
            gc->pathDistanceGrid[x][y] = (int16_t)distance;
        }
    } else {
        changed_or_blocked = true;
    }
    if (gc->pathDistanceGrid[gc->demolishX][gc->demolishY] >= 20) return 2;
    return changed_or_blocked ? 0 : 1;
}

void gc_floodFillPath(GameCanvas *gc) {
    bool keep_searching = true;
    bool destination_found = false;
    for (int pass = 50;
         keep_searching && !destination_found && pass > 0; --pass) {
        keep_searching = false;
        for (int x = gc->placementX; x >= 0; --x) {
            for (int y = gc->placementY; y >= 0; --y) {
                int result = gc_relaxPathCell(gc, x, y);
                keep_searching |= result == 0;
                destination_found |= result == 2;
            }
        }
        for (int x = gc->placementX; x < gc->mapWidth; ++x) {
            for (int y = gc->placementY; y < gc->mapHeight; ++y) {
                int result = gc_relaxPathCell(gc, x, y);
                keep_searching |= result == 0;
                destination_found |= result == 2;
            }
        }
        for (int x = gc->placementX; x >= 0; --x) {
            for (int y = gc->placementY; y < gc->mapHeight; ++y) {
                int result = gc_relaxPathCell(gc, x, y);
                keep_searching |= result == 0;
                destination_found |= result == 2;
            }
        }
        for (int x = gc->placementX; x < gc->mapWidth; ++x) {
            for (int y = gc->placementY; y >= 0; --y) {
                int result = gc_relaxPathCell(gc, x, y);
                keep_searching |= result == 0;
                destination_found |= result == 2;
            }
        }
    }
}

bool gc_findPathStep(GameCanvas *gc, int32_t x, int32_t y,
                     int32_t current_distance) {
    if (x < 0 || x >= gc->mapWidth || y < 0 || y >= gc->mapHeight
        || gc->pathDistanceGrid[x][y] < 20
        || gc->pathDistanceGrid[x][y] >= current_distance)
        return false;
    gc->nextPathX = x;
    gc->nextPathY = y;
    return true;
}

bool gc_isTileMatchForPathInlined(GameCanvas *gc, bool connected,
                                  int32_t x, int32_t y,
                                  int32_t tile_a, int32_t tile_b,
                                  int32_t tile_c, int32_t tile_d,
                                  int32_t tile_e, int32_t tile_f,
                                  int32_t tile_g, int32_t tile_h,
                                  bool high_owner) {
    if (connected || x < 0 || x >= gc->mapWidth
        || y < 0 || y >= gc->mapHeight)
        return connected;
    int tile = gc->tileMap[x][y];
    int owner = gc->trackMap[x][y] & 0x3F;
    owner = tile == 76 && high_owner ? owner >> 3 : owner & 7;
    if (owner == gc->existingTrackOwnerCode
        && (tile == tile_a || tile == tile_b || tile == tile_c
            || tile == tile_d || tile == tile_e || tile == tile_f
            || tile == tile_g || tile == tile_h || tile == 76))
        return true;
    return false;
}

void gc_validatePathStep(GameCanvas *gc) {
    int x = gc->demolishX;
    int y = gc->demolishY;
    bool path_complete = true;
    bool walking = true;
    do {
        walking = true;
        gc->nextPathX = x;
        gc->nextPathY = y;
        int tile = gc->tileMap[x][y];
        if (tile == 7 || tile == 10 || tile == 21 || tile == 23 || tile == 71) {
            gc_clearBuildPathTile(gc, x - 1, y);
            gc_clearBuildPathTile(gc, x + 1, y);
        }
        if (tile == 8 || tile == 9 || tile == 22 || tile == 24 || tile == 70) {
            gc_clearBuildPathTile(gc, x, y - 1);
            gc_clearBuildPathTile(gc, x, y + 1);
        }
        if (x == gc->placementX && y == gc->placementY) {
            walking = false;
        } else {
            int distance = gc->pathDistanceGrid[x][y];
            bool found;
            if (gc->pathTieBreakCounter % 2 == 0) {
                found = gc_findPathStep(gc, x + 1, y, distance)
                        || gc_findPathStep(gc, x - 1, y, distance)
                        || gc_findPathStep(gc, x, y + 1, distance)
                        || gc_findPathStep(gc, x, y - 1, distance);
            } else {
                found = gc_findPathStep(gc, x, y + 1, distance)
                        || gc_findPathStep(gc, x, y - 1, distance)
                        || gc_findPathStep(gc, x + 1, y, distance)
                        || gc_findPathStep(gc, x - 1, y, distance);
            }
            if (!found) {
                walking = false;
                path_complete = false;
            }
        }
        ++gc->pathTieBreakCounter;
        gc->pathDistanceGrid[x][y] = 9;
        x = gc->nextPathX;
        y = gc->nextPathY;

        int step_x = 0;
        int step_y = 0;
        if (gc->tileMap[x][y] == 7) step_y = -1;
        if (gc->tileMap[x][y] == 8) step_x = -1;
        if (gc->tileMap[x][y] == 9) step_x = 1;
        if (gc->tileMap[x][y] == 10) step_y = 1;
        if (step_x != 0 || step_y != 0) {
            do {
                gc->pathDistanceGrid[x][y] = 9;
                x += step_x;
                y += step_y;
            } while (gc->pathDistanceGrid[x][y] == 3);
        }
    } while (walking);

    for (int map_x = 0; map_x < gc->mapWidth; ++map_x)
        for (int map_y = 0; map_y < gc->mapHeight; ++map_y)
            if (gc->pathDistanceGrid[map_x][map_y] != 9)
                gc->pathDistanceGrid[map_x][map_y] = 0;
    if (!path_complete) return;

    for (int map_x = 0; map_x < gc->mapWidth; ++map_x) {
        for (int map_y = 0; map_y < gc->mapHeight; ++map_y) {
            if (gc->pathDistanceGrid[map_x][map_y] != 9) continue;
            bool left = map_x > 0
                        && gc->pathDistanceGrid[map_x - 1][map_y] == 9;
            bool right = map_x < gc->mapWidth - 1
                         && gc->pathDistanceGrid[map_x + 1][map_y] == 9;
            bool up = map_y > 0
                      && gc->pathDistanceGrid[map_x][map_y - 1] == 9;
            bool down = map_y < gc->mapHeight - 1
                        && gc->pathDistanceGrid[map_x][map_y + 1] == 9;
            if ((left ? 1 : 0) + (right ? 1 : 0)
                + (up ? 1 : 0) + (down ? 1 : 0) == 1) {
                left = gc_isTileMatchForPathInlined(
                    gc, left, map_x - 1, map_y,
                    71, 72, 74, 78, 80, 84, 88, 90, true);
                right = gc_isTileMatchForPathInlined(
                    gc, right, map_x + 1, map_y,
                    71, 73, 75, 78, 80, 85, 88, 90, true);
                up = gc_isTileMatchForPathInlined(
                    gc, up, map_x, map_y - 1,
                    70, 73, 74, 77, 79, 83, 87, 89, false);
                down = gc_isTileMatchForPathInlined(
                    gc, down, map_x, map_y + 1,
                    70, 72, 75, 77, 79, 86, 87, 89, false);
            }

            int old_tile = gc->tileMap[map_x][map_y];
            bool joined_track = false;
            if (old_tile != 0 && old_tile > 28)
                gc->selectionCost += gc->baseTrainSpeed;
            int cost_before_shape = gc->selectionCost;
            if (left && right) {
                if (old_tile == 22) gc->tileMap[map_x][map_y] = 78;
                else if (old_tile == 24) gc->tileMap[map_x][map_y] = 80;
                else if (old_tile == 9) {
                    gc->tileMap[map_x][map_y] = 85;
                    gc->selectionCost += gc->moneyBaseA;
                } else if (old_tile == 8) {
                    gc->tileMap[map_x][map_y] = 84;
                    gc->selectionCost += gc->moneyBaseA;
                } else if (old_tile == 70) {
                    gc->tileMap[map_x][map_y] = 76;
                    joined_track = true;
                    gc->trackMap[map_x][map_y] = (int8_t)(
                        0x40 | (gc->trackMap[map_x][map_y] & 0x3F)
                        | gc->previewTrackOwnerCode << 3);
                } else if (gc->tileElevation[map_x][map_y] == -1) {
                    gc->tileMap[map_x][map_y] = 81;
                    gc->selectionCost += gc->moneyBaseB;
                } else {
                    gc->tileMap[map_x][map_y] = 71;
                }
            } else if (up && down) {
                if (old_tile == 21) gc->tileMap[map_x][map_y] = 77;
                else if (old_tile == 23) gc->tileMap[map_x][map_y] = 79;
                else if (old_tile == 7) {
                    gc->tileMap[map_x][map_y] = 83;
                    gc->selectionCost += gc->moneyBaseA;
                } else if (old_tile == 10) {
                    gc->tileMap[map_x][map_y] = 86;
                    gc->selectionCost += gc->moneyBaseA;
                } else if (old_tile == 71) {
                    gc->tileMap[map_x][map_y] = 76;
                    joined_track = true;
                    gc->trackMap[map_x][map_y] = (int8_t)(
                        0x40 | (gc->trackMap[map_x][map_y] & 0x3F) << 3
                        | gc->previewTrackOwnerCode);
                } else if (gc->tileElevation[map_x][map_y] == -1) {
                    gc->tileMap[map_x][map_y] = 82;
                    gc->selectionCost += gc->moneyBaseB;
                } else {
                    gc->tileMap[map_x][map_y] = 70;
                }
            } else if (left && up) gc->tileMap[map_x][map_y] = 75;
            else if (right && up) gc->tileMap[map_x][map_y] = 72;
            else if (left && down) gc->tileMap[map_x][map_y] = 73;
            else if (right && down) gc->tileMap[map_x][map_y] = 74;

            if (!joined_track && old_tile != gc->tileMap[map_x][map_y]) {
                gc->trackMap[map_x][map_y] =
                    (int8_t)(0x40 | gc->previewTrackOwnerCode);
                if (cost_before_shape == gc->selectionCost)
                    gc->selectionCost += gc->accel1;
            }
            if (joined_track)
                gc->selectionCost +=
                    gc->brake1 + gc->accel1 + gc->baseTrainSpeed;
        }
    }
}

void gc_scanTrackTiles(GameCanvas *gc) {
    gc->trackTileCount = 0;
    for (int x = 0; x < gc->mapWidth; ++x) {
        for (int y = 0; y < gc->mapHeight; ++y) {
            if (gc->trackMap[x][y] == 64) ++gc->trackTileCount;
            if (gc->tileMap[x][y] != 76) continue;
            int low_owner = gc->trackMap[x][y] & 7;
            int high_owner = (gc->trackMap[x][y] & 0x3F) >> 3;
            if (low_owner == 0) ++gc->trackTileCount;
            if (high_owner == 0) ++gc->trackTileCount;
        }
    }
}

void gc_clearPathDistanceGrid(GameCanvas *gc) {
    for (int x = 0; x < gc->mapWidth; ++x)
        for (int y = 0; y < gc->mapHeight; ++y)
            gc->pathDistanceGrid[x][y] = 0;

    for (int x = 0; x < gc->mapWidth; ++x) {
        for (int y = 0; y < gc->mapHeight; ++y) {
            if (gc->pathDistanceGrid[x][y] != 0) continue;
            int16_t passable = gc->trackMap[x][y] == 0 ? 1 : 0;
            int tile = gc->tileMap[x][y];
            if (passable == 1) {
                if (gc->tileElevation[x][y] == -1) passable = 0;
                if (tile != 0 && tile != 21 && tile != 22
                    && tile != 23 && tile != 24
                    && (tile < 29 || tile > 33))
                    passable = 0;
            }
            if (tile == 71 && x > 0 && x < gc->mapWidth - 1
                && gc->trackMap[x][y] == gc->trackMap[x - 1][y]
                && gc->trackMap[x][y] == gc->trackMap[x + 1][y])
                passable = 1;
            if (tile == 70 && y > 0 && y < gc->mapHeight - 1
                && gc->trackMap[x][y] == gc->trackMap[x][y - 1]
                && gc->trackMap[x][y] == gc->trackMap[x][y + 1])
                passable = 1;
            gc->pathDistanceGrid[x][y] = passable;

            if (tile == 7) {
                int other_y = y;
                bool found_exit = false;
                bool stop;
                do {
                    stop = false;
                    --other_y;
                    if (gc->tileElevation[x][other_y] != -1
                        || gc->tileMap[x][other_y] != 0
                        || gc->trackMap[x][other_y] != 0)
                        stop = true;
                    if (other_y == 0) stop = true;
                    if (gc->tileMap[x][other_y] == 10) {
                        stop = true;
                        found_exit = true;
                    }
                } while (!stop);
                if (found_exit) {
                    for (int tunnel_y = other_y; tunnel_y <= y; ++tunnel_y)
                        gc->pathDistanceGrid[x][tunnel_y] = 3;
                    gc->pathDistanceGrid[x][other_y] = 2;
                    gc->pathDistanceGrid[x][y] = 2;
                }
            }
            if (tile != 8) continue;
            int other_x = x;
            bool found_exit = false;
            bool stop;
            do {
                stop = false;
                --other_x;
                if (gc->tileElevation[other_x][y] != -1
                    || gc->tileMap[other_x][y] != 0
                    || gc->trackMap[other_x][y] != 0)
                    stop = true;
                if (other_x == 0) stop = true;
                if (gc->tileMap[other_x][y] == 9) {
                    stop = true;
                    found_exit = true;
                }
            } while (!stop);
            if (!found_exit) continue;
            for (int tunnel_x = other_x; tunnel_x <= x; ++tunnel_x)
                gc->pathDistanceGrid[tunnel_x][y] = 3;
            gc->pathDistanceGrid[other_x][y] = 2;
            gc->pathDistanceGrid[x][y] = 2;
        }
    }
}

bool gc_tryBuildPath(GameCanvas *gc) {
    gc->pathDistanceGrid.resize(gc->mapWidth, gc->mapHeight);
    gc->pathSearchLimit = 10;
    gc->selectionCost = 0;
    gc_clearPathDistanceGrid(gc);
    gc->pathDistanceGrid[gc->placementX][gc->placementY] = 20;
    gc_floodFillPath(gc);
    if (gc->pathDistanceGrid[gc->demolishX][gc->demolishY] < 20) {
        gc->pathDistanceGrid.clear();
        return false;
    }
    gc_validatePathStep(gc);
    gc->pathDistanceGrid.clear();
    gc_scanTrackTiles(gc);
    if (gc->trackTileCount == 0) return false;
    return gc->selectionCost != 0;
}
