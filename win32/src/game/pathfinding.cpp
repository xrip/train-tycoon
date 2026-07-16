// Build-path search, validation, and preview tiles.
#include "game_internal.h"

#include <limits.h>

int16_t game_pathDistanceMin(Game *game, int32_t x, int32_t y,
                           int32_t current_min) {
    int16_t result = (int16_t)current_min;
    if (x >= 0 && x < game->world.mapWidth && y >= 0 && y < game->world.mapHeight
        && game->pathfinder.distanceGrid[x][y] >= 20
        && game->pathfinder.distanceGrid[x][y] < result)
        result = game->pathfinder.distanceGrid[x][y];
    return result;
}

int32_t game_relaxPathCell(Game *game, int32_t x, int32_t y) {
    bool changed_or_blocked = false;
    int16_t cell = game->pathfinder.distanceGrid[x][y];
    if (cell > 0 && cell != 3 && cell != 7) {
        int previous = cell < 20 ? SHRT_MAX : cell;
        int distance = game_pathDistanceMin(game, x - 1, y, previous);
        distance = game_pathDistanceMin(game, x + 1, y, distance);
        distance = game_pathDistanceMin(game, x, y - 1, distance);
        distance = game_pathDistanceMin(game, x, y + 1, distance);
        if (distance < previous) {
            distance = (int16_t)(distance + 1);
            if (cell == 2) {
                int step_x = 0;
                int step_y = 0;
                int tunnel_x = x;
                int tunnel_y = y;
                if (game->world.tileMap[x][y] == 7) step_y = -1;
                if (game->world.tileMap[x][y] == 8) step_x = -1;
                if (game->world.tileMap[x][y] == 9) step_x = 1;
                if (game->world.tileMap[x][y] == 10) step_y = 1;
                if (step_x != 0 || step_y != 0) {
                    int tunnel_distance = distance;
                    do {
                        tunnel_distance = (int16_t)(tunnel_distance + 1);
                        tunnel_x += step_x;
                        tunnel_y += step_y;
                    } while (game->pathfinder.distanceGrid[tunnel_x][tunnel_y] == 3);
                    game->pathfinder.distanceGrid[tunnel_x][tunnel_y] =
                        (int16_t)tunnel_distance;
                    changed_or_blocked = true;
                }
            }
            game->pathfinder.distanceGrid[x][y] = (int16_t)distance;
        }
    } else {
        changed_or_blocked = true;
    }
    if (game->pathfinder.distanceGrid[game->pathfinder.targetX][game->pathfinder.targetY] >= 20) return 2;
    return changed_or_blocked ? 0 : 1;
}

void game_floodFillPath(Game *game) {
    bool keep_searching = true;
    bool destination_found = false;
    for (int pass = 50;
         keep_searching && !destination_found && pass > 0; --pass) {
        keep_searching = false;
        for (int x = game->pathfinder.anchorX; x >= 0; --x) {
            for (int y = game->pathfinder.anchorY; y >= 0; --y) {
                int result = game_relaxPathCell(game, x, y);
                keep_searching |= result == 0;
                destination_found |= result == 2;
            }
        }
        for (int x = game->pathfinder.anchorX; x < game->world.mapWidth; ++x) {
            for (int y = game->pathfinder.anchorY; y < game->world.mapHeight; ++y) {
                int result = game_relaxPathCell(game, x, y);
                keep_searching |= result == 0;
                destination_found |= result == 2;
            }
        }
        for (int x = game->pathfinder.anchorX; x >= 0; --x) {
            for (int y = game->pathfinder.anchorY; y < game->world.mapHeight; ++y) {
                int result = game_relaxPathCell(game, x, y);
                keep_searching |= result == 0;
                destination_found |= result == 2;
            }
        }
        for (int x = game->pathfinder.anchorX; x < game->world.mapWidth; ++x) {
            for (int y = game->pathfinder.anchorY; y >= 0; --y) {
                int result = game_relaxPathCell(game, x, y);
                keep_searching |= result == 0;
                destination_found |= result == 2;
            }
        }
    }
}

bool game_findPathStep(Game *game, int32_t x, int32_t y,
                     int32_t current_distance) {
    if (!game->world.in_bounds(x, y)
        || game->pathfinder.distanceGrid[x][y] < 20
        || game->pathfinder.distanceGrid[x][y] >= current_distance)
        return false;
    game->pathfinder.nextX = x;
    game->pathfinder.nextY = y;
    return true;
}

bool game_isTileMatchForPath(Game *game, bool connected,
                                  int32_t x, int32_t y,
                                  int32_t tile_a, int32_t tile_b,
                                  int32_t tile_c, int32_t tile_d,
                                  int32_t tile_e, int32_t tile_f,
                                  int32_t tile_g, int32_t tile_h,
                                  bool high_owner) {
    if (connected || !game->world.in_bounds(x, y))
        return connected;
    int tile = game->world.tileMap[x][y];
    int owner = game->world.trackMap[x][y] & track::OWNER_MASK;
    owner = tile == tile::CROSSING && high_owner
                ? owner >> track::DIR_OWNER_SHIFT
                : owner & track::DIR_OWNER_MASK;
    if (owner == game->build.existingTrackOwnerCode
        && (tile == tile_a || tile == tile_b || tile == tile_c
            || tile == tile_d || tile == tile_e || tile == tile_f
            || tile == tile_g || tile == tile_h || tile == 76))
        return true;
    return false;
}

void game_validatePathStep(Game *game) {
    int x = game->pathfinder.targetX;
    int y = game->pathfinder.targetY;
    bool path_complete = true;
    bool walking = true;
    do {
        walking = true;
        game->pathfinder.nextX = x;
        game->pathfinder.nextY = y;
        int tile = game->world.tileMap[x][y];
        if (tile == 7 || tile == 10 || tile == 21 || tile == 23 || tile == 71) {
            game_clearBuildPathTile(game, x - 1, y);
            game_clearBuildPathTile(game, x + 1, y);
        }
        if (tile == 8 || tile == 9 || tile == 22 || tile == 24 || tile == 70) {
            game_clearBuildPathTile(game, x, y - 1);
            game_clearBuildPathTile(game, x, y + 1);
        }
        if (x == game->pathfinder.anchorX && y == game->pathfinder.anchorY) {
            walking = false;
        } else {
            int distance = game->pathfinder.distanceGrid[x][y];
            bool found;
            if (game->pathfinder.tieBreakCounter % 2 == 0) {
                found = game_findPathStep(game, x + 1, y, distance)
                        || game_findPathStep(game, x - 1, y, distance)
                        || game_findPathStep(game, x, y + 1, distance)
                        || game_findPathStep(game, x, y - 1, distance);
            } else {
                found = game_findPathStep(game, x, y + 1, distance)
                        || game_findPathStep(game, x, y - 1, distance)
                        || game_findPathStep(game, x + 1, y, distance)
                        || game_findPathStep(game, x - 1, y, distance);
            }
            if (!found) {
                walking = false;
                path_complete = false;
            }
        }
        ++game->pathfinder.tieBreakCounter;
        game->pathfinder.distanceGrid[x][y] = 9;
        x = game->pathfinder.nextX;
        y = game->pathfinder.nextY;

        int step_x = 0;
        int step_y = 0;
        if (game->world.tileMap[x][y] == 7) step_y = -1;
        if (game->world.tileMap[x][y] == 8) step_x = -1;
        if (game->world.tileMap[x][y] == 9) step_x = 1;
        if (game->world.tileMap[x][y] == 10) step_y = 1;
        if (step_x != 0 || step_y != 0) {
            do {
                game->pathfinder.distanceGrid[x][y] = 9;
                x += step_x;
                y += step_y;
            } while (game->pathfinder.distanceGrid[x][y] == 3);
        }
    } while (walking);

    for (int map_x = 0; map_x < game->world.mapWidth; ++map_x)
        for (int map_y = 0; map_y < game->world.mapHeight; ++map_y)
            if (game->pathfinder.distanceGrid[map_x][map_y] != 9)
                game->pathfinder.distanceGrid[map_x][map_y] = 0;
    if (!path_complete) return;

    for (int map_x = 0; map_x < game->world.mapWidth; ++map_x) {
        for (int map_y = 0; map_y < game->world.mapHeight; ++map_y) {
            if (game->pathfinder.distanceGrid[map_x][map_y] != 9) continue;
            bool left = map_x > 0
                        && game->pathfinder.distanceGrid[map_x - 1][map_y] == 9;
            bool right = map_x < game->world.mapWidth - 1
                         && game->pathfinder.distanceGrid[map_x + 1][map_y] == 9;
            bool up = map_y > 0
                      && game->pathfinder.distanceGrid[map_x][map_y - 1] == 9;
            bool down = map_y < game->world.mapHeight - 1
                        && game->pathfinder.distanceGrid[map_x][map_y + 1] == 9;
            if ((left ? 1 : 0) + (right ? 1 : 0)
                + (up ? 1 : 0) + (down ? 1 : 0) == 1) {
                left = game_isTileMatchForPath(
                    game, left, map_x - 1, map_y,
                    71, 72, 74, 78, 80, 84, 88, 90, true);
                right = game_isTileMatchForPath(
                    game, right, map_x + 1, map_y,
                    71, 73, 75, 78, 80, 85, 88, 90, true);
                up = game_isTileMatchForPath(
                    game, up, map_x, map_y - 1,
                    70, 73, 74, 77, 79, 83, 87, 89, false);
                down = game_isTileMatchForPath(
                    game, down, map_x, map_y + 1,
                    70, 72, 75, 77, 79, 86, 87, 89, false);
            }

            int old_tile = game->world.tileMap[map_x][map_y];
            bool joined_track = false;
            if (old_tile != 0 && old_tile > 28)
                game->build.selectionCost += game->economy.baseTrainSpeed;
            int cost_before_shape = game->build.selectionCost;
            if (left && right) {
                if (old_tile == 22) game->world.tileMap[map_x][map_y] = 78;
                else if (old_tile == 24) game->world.tileMap[map_x][map_y] = 80;
                else if (old_tile == 9) {
                    game->world.tileMap[map_x][map_y] = 85;
                    game->build.selectionCost += game->economy.moneyBaseA;
                } else if (old_tile == 8) {
                    game->world.tileMap[map_x][map_y] = 84;
                    game->build.selectionCost += game->economy.moneyBaseA;
                } else if (old_tile == 70) {
                    game->world.tileMap[map_x][map_y] = tile::CROSSING;
                    joined_track = true;
                    game->world.trackMap[map_x][map_y] = (int8_t)(
                        track::PRESENT | (game->world.trackMap[map_x][map_y] & track::OWNER_MASK)
                        | game->build.previewTrackOwnerCode << track::DIR_OWNER_SHIFT);
                } else if (game->world.tileElevation[map_x][map_y] == -1) {
                    game->world.tileMap[map_x][map_y] = 81;
                    game->build.selectionCost += game->economy.moneyBaseB;
                } else {
                    game->world.tileMap[map_x][map_y] = 71;
                }
            } else if (up && down) {
                if (old_tile == 21) game->world.tileMap[map_x][map_y] = 77;
                else if (old_tile == 23) game->world.tileMap[map_x][map_y] = 79;
                else if (old_tile == 7) {
                    game->world.tileMap[map_x][map_y] = 83;
                    game->build.selectionCost += game->economy.moneyBaseA;
                } else if (old_tile == 10) {
                    game->world.tileMap[map_x][map_y] = 86;
                    game->build.selectionCost += game->economy.moneyBaseA;
                } else if (old_tile == 71) {
                    game->world.tileMap[map_x][map_y] = tile::CROSSING;
                    joined_track = true;
                    game->world.trackMap[map_x][map_y] = (int8_t)(
                        track::PRESENT | (game->world.trackMap[map_x][map_y] & track::OWNER_MASK) << track::DIR_OWNER_SHIFT
                        | game->build.previewTrackOwnerCode);
                } else if (game->world.tileElevation[map_x][map_y] == -1) {
                    game->world.tileMap[map_x][map_y] = 82;
                    game->build.selectionCost += game->economy.moneyBaseB;
                } else {
                    game->world.tileMap[map_x][map_y] = 70;
                }
            } else if (left && up) game->world.tileMap[map_x][map_y] = 75;
            else if (right && up) game->world.tileMap[map_x][map_y] = 72;
            else if (left && down) game->world.tileMap[map_x][map_y] = 73;
            else if (right && down) game->world.tileMap[map_x][map_y] = 74;

            if (!joined_track && old_tile != game->world.tileMap[map_x][map_y]) {
                game->world.trackMap[map_x][map_y] =
                    (int8_t)(track::PRESENT | game->build.previewTrackOwnerCode);
                if (cost_before_shape == game->build.selectionCost)
                    game->build.selectionCost += game->economy.accel1;
            }
            if (joined_track)
                game->build.selectionCost +=
                    game->economy.brake1 + game->economy.accel1 + game->economy.baseTrainSpeed;
        }
    }
}

void game_scanTrackTiles(Game *game) {
    game->build.trackTileCount = 0;
    for (int x = 0; x < game->world.mapWidth; ++x) {
        for (int y = 0; y < game->world.mapHeight; ++y) {
            if (game->world.trackMap[x][y] == track::PRESENT) ++game->build.trackTileCount;
            if (game->world.tileMap[x][y] != tile::CROSSING) continue;
            int low_owner = game->world.trackMap[x][y] & track::DIR_OWNER_MASK;
            int high_owner = (game->world.trackMap[x][y] & track::OWNER_MASK) >> track::DIR_OWNER_SHIFT;
            if (low_owner == 0) ++game->build.trackTileCount;
            if (high_owner == 0) ++game->build.trackTileCount;
        }
    }
}

void game_clearPathDistanceGrid(Game *game) {
    for (int x = 0; x < game->world.mapWidth; ++x)
        for (int y = 0; y < game->world.mapHeight; ++y)
            game->pathfinder.distanceGrid[x][y] = 0;

    for (int x = 0; x < game->world.mapWidth; ++x) {
        for (int y = 0; y < game->world.mapHeight; ++y) {
            if (game->pathfinder.distanceGrid[x][y] != 0) continue;
            int16_t passable = game->world.trackMap[x][y] == 0 ? 1 : 0;
            int tile = game->world.tileMap[x][y];
            if (passable == 1) {
                if (game->world.tileElevation[x][y] == -1) passable = 0;
                if (tile != 0 && tile != 21 && tile != 22
                    && tile != 23 && tile != 24
                    && (tile < tile::SCENERY_FIRST || tile > tile::SCENERY_LAST))
                    passable = 0;
            }
            if (tile == 71 && x > 0 && x < game->world.mapWidth - 1
                && game->world.trackMap[x][y] == game->world.trackMap[x - 1][y]
                && game->world.trackMap[x][y] == game->world.trackMap[x + 1][y])
                passable = 1;
            if (tile == 70 && y > 0 && y < game->world.mapHeight - 1
                && game->world.trackMap[x][y] == game->world.trackMap[x][y - 1]
                && game->world.trackMap[x][y] == game->world.trackMap[x][y + 1])
                passable = 1;
            game->pathfinder.distanceGrid[x][y] = passable;

            if (tile == 7) {
                int other_y = y;
                bool found_exit = false;
                bool stop;
                do {
                    stop = false;
                    --other_y;
                    if (game->world.tileElevation[x][other_y] != -1
                        || game->world.tileMap[x][other_y] != 0
                        || game->world.trackMap[x][other_y] != 0)
                        stop = true;
                    if (other_y == 0) stop = true;
                    if (game->world.tileMap[x][other_y] == 10) {
                        stop = true;
                        found_exit = true;
                    }
                } while (!stop);
                if (found_exit) {
                    for (int tunnel_y = other_y; tunnel_y <= y; ++tunnel_y)
                        game->pathfinder.distanceGrid[x][tunnel_y] = 3;
                    game->pathfinder.distanceGrid[x][other_y] = 2;
                    game->pathfinder.distanceGrid[x][y] = 2;
                }
            }
            if (tile != 8) continue;
            int other_x = x;
            bool found_exit = false;
            bool stop;
            do {
                stop = false;
                --other_x;
                if (game->world.tileElevation[other_x][y] != -1
                    || game->world.tileMap[other_x][y] != 0
                    || game->world.trackMap[other_x][y] != 0)
                    stop = true;
                if (other_x == 0) stop = true;
                if (game->world.tileMap[other_x][y] == 9) {
                    stop = true;
                    found_exit = true;
                }
            } while (!stop);
            if (!found_exit) continue;
            for (int tunnel_x = other_x; tunnel_x <= x; ++tunnel_x)
                game->pathfinder.distanceGrid[tunnel_x][y] = 3;
            game->pathfinder.distanceGrid[other_x][y] = 2;
            game->pathfinder.distanceGrid[x][y] = 2;
        }
    }
}

bool game_tryBuildPath(Game *game) {
    game->pathfinder.distanceGrid.resize(game->world.mapWidth, game->world.mapHeight);
    game->pathfinder.searchLimit = 10;
    game->build.selectionCost = 0;
    game_clearPathDistanceGrid(game);
    game->pathfinder.distanceGrid[game->pathfinder.anchorX][game->pathfinder.anchorY] = 20;
    game_floodFillPath(game);
    if (game->pathfinder.distanceGrid[game->pathfinder.targetX][game->pathfinder.targetY] < 20) {
        game->pathfinder.distanceGrid.clear();
        return false;
    }
    game_validatePathStep(game);
    game->pathfinder.distanceGrid.clear();
    game_scanTrackTiles(game);
    if (game->build.trackTileCount == 0) return false;
    return game->build.selectionCost != 0;
}
