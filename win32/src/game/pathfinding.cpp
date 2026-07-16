// Build-path search, validation, and preview tiles.
#include "game_methods.h"

#include <limits.h>

int16_t gc_pathDistanceMin(GameCanvas *gc, int32_t x, int32_t y,
                           int32_t current_min) {
    int16_t result = (int16_t)current_min;
    if (x >= 0 && x < gc->world.mapWidth && y >= 0 && y < gc->world.mapHeight
        && gc->pathfinder.distanceGrid[x][y] >= 20
        && gc->pathfinder.distanceGrid[x][y] < result)
        result = gc->pathfinder.distanceGrid[x][y];
    return result;
}

int32_t gc_relaxPathCell(GameCanvas *gc, int32_t x, int32_t y) {
    bool changed_or_blocked = false;
    int16_t cell = gc->pathfinder.distanceGrid[x][y];
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
                if (gc->world.tileMap[x][y] == 7) step_y = -1;
                if (gc->world.tileMap[x][y] == 8) step_x = -1;
                if (gc->world.tileMap[x][y] == 9) step_x = 1;
                if (gc->world.tileMap[x][y] == 10) step_y = 1;
                if (step_x != 0 || step_y != 0) {
                    int tunnel_distance = distance;
                    do {
                        tunnel_distance = (int16_t)(tunnel_distance + 1);
                        tunnel_x += step_x;
                        tunnel_y += step_y;
                    } while (gc->pathfinder.distanceGrid[tunnel_x][tunnel_y] == 3);
                    gc->pathfinder.distanceGrid[tunnel_x][tunnel_y] =
                        (int16_t)tunnel_distance;
                    changed_or_blocked = true;
                }
            }
            gc->pathfinder.distanceGrid[x][y] = (int16_t)distance;
        }
    } else {
        changed_or_blocked = true;
    }
    if (gc->pathfinder.distanceGrid[gc->pathfinder.targetX][gc->pathfinder.targetY] >= 20) return 2;
    return changed_or_blocked ? 0 : 1;
}

void gc_floodFillPath(GameCanvas *gc) {
    bool keep_searching = true;
    bool destination_found = false;
    for (int pass = 50;
         keep_searching && !destination_found && pass > 0; --pass) {
        keep_searching = false;
        for (int x = gc->pathfinder.anchorX; x >= 0; --x) {
            for (int y = gc->pathfinder.anchorY; y >= 0; --y) {
                int result = gc_relaxPathCell(gc, x, y);
                keep_searching |= result == 0;
                destination_found |= result == 2;
            }
        }
        for (int x = gc->pathfinder.anchorX; x < gc->world.mapWidth; ++x) {
            for (int y = gc->pathfinder.anchorY; y < gc->world.mapHeight; ++y) {
                int result = gc_relaxPathCell(gc, x, y);
                keep_searching |= result == 0;
                destination_found |= result == 2;
            }
        }
        for (int x = gc->pathfinder.anchorX; x >= 0; --x) {
            for (int y = gc->pathfinder.anchorY; y < gc->world.mapHeight; ++y) {
                int result = gc_relaxPathCell(gc, x, y);
                keep_searching |= result == 0;
                destination_found |= result == 2;
            }
        }
        for (int x = gc->pathfinder.anchorX; x < gc->world.mapWidth; ++x) {
            for (int y = gc->pathfinder.anchorY; y >= 0; --y) {
                int result = gc_relaxPathCell(gc, x, y);
                keep_searching |= result == 0;
                destination_found |= result == 2;
            }
        }
    }
}

bool gc_findPathStep(GameCanvas *gc, int32_t x, int32_t y,
                     int32_t current_distance) {
    if (!gc->world.in_bounds(x, y)
        || gc->pathfinder.distanceGrid[x][y] < 20
        || gc->pathfinder.distanceGrid[x][y] >= current_distance)
        return false;
    gc->pathfinder.nextX = x;
    gc->pathfinder.nextY = y;
    return true;
}

bool gc_isTileMatchForPathInlined(GameCanvas *gc, bool connected,
                                  int32_t x, int32_t y,
                                  int32_t tile_a, int32_t tile_b,
                                  int32_t tile_c, int32_t tile_d,
                                  int32_t tile_e, int32_t tile_f,
                                  int32_t tile_g, int32_t tile_h,
                                  bool high_owner) {
    if (connected || !gc->world.in_bounds(x, y))
        return connected;
    int tile = gc->world.tileMap[x][y];
    int owner = gc->world.trackMap[x][y] & track::OWNER_MASK;
    owner = tile == tile::CROSSING && high_owner
                ? owner >> track::DIR_OWNER_SHIFT
                : owner & track::DIR_OWNER_MASK;
    if (owner == gc->build.existingTrackOwnerCode
        && (tile == tile_a || tile == tile_b || tile == tile_c
            || tile == tile_d || tile == tile_e || tile == tile_f
            || tile == tile_g || tile == tile_h || tile == 76))
        return true;
    return false;
}

void gc_validatePathStep(GameCanvas *gc) {
    int x = gc->pathfinder.targetX;
    int y = gc->pathfinder.targetY;
    bool path_complete = true;
    bool walking = true;
    do {
        walking = true;
        gc->pathfinder.nextX = x;
        gc->pathfinder.nextY = y;
        int tile = gc->world.tileMap[x][y];
        if (tile == 7 || tile == 10 || tile == 21 || tile == 23 || tile == 71) {
            gc_clearBuildPathTile(gc, x - 1, y);
            gc_clearBuildPathTile(gc, x + 1, y);
        }
        if (tile == 8 || tile == 9 || tile == 22 || tile == 24 || tile == 70) {
            gc_clearBuildPathTile(gc, x, y - 1);
            gc_clearBuildPathTile(gc, x, y + 1);
        }
        if (x == gc->pathfinder.anchorX && y == gc->pathfinder.anchorY) {
            walking = false;
        } else {
            int distance = gc->pathfinder.distanceGrid[x][y];
            bool found;
            if (gc->pathfinder.tieBreakCounter % 2 == 0) {
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
        ++gc->pathfinder.tieBreakCounter;
        gc->pathfinder.distanceGrid[x][y] = 9;
        x = gc->pathfinder.nextX;
        y = gc->pathfinder.nextY;

        int step_x = 0;
        int step_y = 0;
        if (gc->world.tileMap[x][y] == 7) step_y = -1;
        if (gc->world.tileMap[x][y] == 8) step_x = -1;
        if (gc->world.tileMap[x][y] == 9) step_x = 1;
        if (gc->world.tileMap[x][y] == 10) step_y = 1;
        if (step_x != 0 || step_y != 0) {
            do {
                gc->pathfinder.distanceGrid[x][y] = 9;
                x += step_x;
                y += step_y;
            } while (gc->pathfinder.distanceGrid[x][y] == 3);
        }
    } while (walking);

    for (int map_x = 0; map_x < gc->world.mapWidth; ++map_x)
        for (int map_y = 0; map_y < gc->world.mapHeight; ++map_y)
            if (gc->pathfinder.distanceGrid[map_x][map_y] != 9)
                gc->pathfinder.distanceGrid[map_x][map_y] = 0;
    if (!path_complete) return;

    for (int map_x = 0; map_x < gc->world.mapWidth; ++map_x) {
        for (int map_y = 0; map_y < gc->world.mapHeight; ++map_y) {
            if (gc->pathfinder.distanceGrid[map_x][map_y] != 9) continue;
            bool left = map_x > 0
                        && gc->pathfinder.distanceGrid[map_x - 1][map_y] == 9;
            bool right = map_x < gc->world.mapWidth - 1
                         && gc->pathfinder.distanceGrid[map_x + 1][map_y] == 9;
            bool up = map_y > 0
                      && gc->pathfinder.distanceGrid[map_x][map_y - 1] == 9;
            bool down = map_y < gc->world.mapHeight - 1
                        && gc->pathfinder.distanceGrid[map_x][map_y + 1] == 9;
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

            int old_tile = gc->world.tileMap[map_x][map_y];
            bool joined_track = false;
            if (old_tile != 0 && old_tile > 28)
                gc->build.selectionCost += gc->baseTrainSpeed;
            int cost_before_shape = gc->build.selectionCost;
            if (left && right) {
                if (old_tile == 22) gc->world.tileMap[map_x][map_y] = 78;
                else if (old_tile == 24) gc->world.tileMap[map_x][map_y] = 80;
                else if (old_tile == 9) {
                    gc->world.tileMap[map_x][map_y] = 85;
                    gc->build.selectionCost += gc->moneyBaseA;
                } else if (old_tile == 8) {
                    gc->world.tileMap[map_x][map_y] = 84;
                    gc->build.selectionCost += gc->moneyBaseA;
                } else if (old_tile == 70) {
                    gc->world.tileMap[map_x][map_y] = tile::CROSSING;
                    joined_track = true;
                    gc->world.trackMap[map_x][map_y] = (int8_t)(
                        track::PRESENT | (gc->world.trackMap[map_x][map_y] & track::OWNER_MASK)
                        | gc->build.previewTrackOwnerCode << track::DIR_OWNER_SHIFT);
                } else if (gc->world.tileElevation[map_x][map_y] == -1) {
                    gc->world.tileMap[map_x][map_y] = 81;
                    gc->build.selectionCost += gc->moneyBaseB;
                } else {
                    gc->world.tileMap[map_x][map_y] = 71;
                }
            } else if (up && down) {
                if (old_tile == 21) gc->world.tileMap[map_x][map_y] = 77;
                else if (old_tile == 23) gc->world.tileMap[map_x][map_y] = 79;
                else if (old_tile == 7) {
                    gc->world.tileMap[map_x][map_y] = 83;
                    gc->build.selectionCost += gc->moneyBaseA;
                } else if (old_tile == 10) {
                    gc->world.tileMap[map_x][map_y] = 86;
                    gc->build.selectionCost += gc->moneyBaseA;
                } else if (old_tile == 71) {
                    gc->world.tileMap[map_x][map_y] = tile::CROSSING;
                    joined_track = true;
                    gc->world.trackMap[map_x][map_y] = (int8_t)(
                        track::PRESENT | (gc->world.trackMap[map_x][map_y] & track::OWNER_MASK) << track::DIR_OWNER_SHIFT
                        | gc->build.previewTrackOwnerCode);
                } else if (gc->world.tileElevation[map_x][map_y] == -1) {
                    gc->world.tileMap[map_x][map_y] = 82;
                    gc->build.selectionCost += gc->moneyBaseB;
                } else {
                    gc->world.tileMap[map_x][map_y] = 70;
                }
            } else if (left && up) gc->world.tileMap[map_x][map_y] = 75;
            else if (right && up) gc->world.tileMap[map_x][map_y] = 72;
            else if (left && down) gc->world.tileMap[map_x][map_y] = 73;
            else if (right && down) gc->world.tileMap[map_x][map_y] = 74;

            if (!joined_track && old_tile != gc->world.tileMap[map_x][map_y]) {
                gc->world.trackMap[map_x][map_y] =
                    (int8_t)(track::PRESENT | gc->build.previewTrackOwnerCode);
                if (cost_before_shape == gc->build.selectionCost)
                    gc->build.selectionCost += gc->accel1;
            }
            if (joined_track)
                gc->build.selectionCost +=
                    gc->brake1 + gc->accel1 + gc->baseTrainSpeed;
        }
    }
}

void gc_scanTrackTiles(GameCanvas *gc) {
    gc->build.trackTileCount = 0;
    for (int x = 0; x < gc->world.mapWidth; ++x) {
        for (int y = 0; y < gc->world.mapHeight; ++y) {
            if (gc->world.trackMap[x][y] == track::PRESENT) ++gc->build.trackTileCount;
            if (gc->world.tileMap[x][y] != tile::CROSSING) continue;
            int low_owner = gc->world.trackMap[x][y] & track::DIR_OWNER_MASK;
            int high_owner = (gc->world.trackMap[x][y] & track::OWNER_MASK) >> track::DIR_OWNER_SHIFT;
            if (low_owner == 0) ++gc->build.trackTileCount;
            if (high_owner == 0) ++gc->build.trackTileCount;
        }
    }
}

void gc_clearPathDistanceGrid(GameCanvas *gc) {
    for (int x = 0; x < gc->world.mapWidth; ++x)
        for (int y = 0; y < gc->world.mapHeight; ++y)
            gc->pathfinder.distanceGrid[x][y] = 0;

    for (int x = 0; x < gc->world.mapWidth; ++x) {
        for (int y = 0; y < gc->world.mapHeight; ++y) {
            if (gc->pathfinder.distanceGrid[x][y] != 0) continue;
            int16_t passable = gc->world.trackMap[x][y] == 0 ? 1 : 0;
            int tile = gc->world.tileMap[x][y];
            if (passable == 1) {
                if (gc->world.tileElevation[x][y] == -1) passable = 0;
                if (tile != 0 && tile != 21 && tile != 22
                    && tile != 23 && tile != 24
                    && (tile < tile::SCENERY_FIRST || tile > tile::SCENERY_LAST))
                    passable = 0;
            }
            if (tile == 71 && x > 0 && x < gc->world.mapWidth - 1
                && gc->world.trackMap[x][y] == gc->world.trackMap[x - 1][y]
                && gc->world.trackMap[x][y] == gc->world.trackMap[x + 1][y])
                passable = 1;
            if (tile == 70 && y > 0 && y < gc->world.mapHeight - 1
                && gc->world.trackMap[x][y] == gc->world.trackMap[x][y - 1]
                && gc->world.trackMap[x][y] == gc->world.trackMap[x][y + 1])
                passable = 1;
            gc->pathfinder.distanceGrid[x][y] = passable;

            if (tile == 7) {
                int other_y = y;
                bool found_exit = false;
                bool stop;
                do {
                    stop = false;
                    --other_y;
                    if (gc->world.tileElevation[x][other_y] != -1
                        || gc->world.tileMap[x][other_y] != 0
                        || gc->world.trackMap[x][other_y] != 0)
                        stop = true;
                    if (other_y == 0) stop = true;
                    if (gc->world.tileMap[x][other_y] == 10) {
                        stop = true;
                        found_exit = true;
                    }
                } while (!stop);
                if (found_exit) {
                    for (int tunnel_y = other_y; tunnel_y <= y; ++tunnel_y)
                        gc->pathfinder.distanceGrid[x][tunnel_y] = 3;
                    gc->pathfinder.distanceGrid[x][other_y] = 2;
                    gc->pathfinder.distanceGrid[x][y] = 2;
                }
            }
            if (tile != 8) continue;
            int other_x = x;
            bool found_exit = false;
            bool stop;
            do {
                stop = false;
                --other_x;
                if (gc->world.tileElevation[other_x][y] != -1
                    || gc->world.tileMap[other_x][y] != 0
                    || gc->world.trackMap[other_x][y] != 0)
                    stop = true;
                if (other_x == 0) stop = true;
                if (gc->world.tileMap[other_x][y] == 9) {
                    stop = true;
                    found_exit = true;
                }
            } while (!stop);
            if (!found_exit) continue;
            for (int tunnel_x = other_x; tunnel_x <= x; ++tunnel_x)
                gc->pathfinder.distanceGrid[tunnel_x][y] = 3;
            gc->pathfinder.distanceGrid[other_x][y] = 2;
            gc->pathfinder.distanceGrid[x][y] = 2;
        }
    }
}

bool gc_tryBuildPath(GameCanvas *gc) {
    gc->pathfinder.distanceGrid.resize(gc->world.mapWidth, gc->world.mapHeight);
    gc->pathfinder.searchLimit = 10;
    gc->build.selectionCost = 0;
    gc_clearPathDistanceGrid(gc);
    gc->pathfinder.distanceGrid[gc->pathfinder.anchorX][gc->pathfinder.anchorY] = 20;
    gc_floodFillPath(gc);
    if (gc->pathfinder.distanceGrid[gc->pathfinder.targetX][gc->pathfinder.targetY] < 20) {
        gc->pathfinder.distanceGrid.clear();
        return false;
    }
    gc_validatePathStep(gc);
    gc->pathfinder.distanceGrid.clear();
    gc_scanTrackTiles(gc);
    if (gc->build.trackTileCount == 0) return false;
    return gc->build.selectionCost != 0;
}
