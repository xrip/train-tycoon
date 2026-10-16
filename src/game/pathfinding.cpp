// Build-path search, validation, and preview tiles.
#include "game.h"

#include <limits.h>

int16_t Pathfinder::distance_min(const World& world, int32_t x, int32_t y,
                                 int32_t current_min) const {
    int16_t result = (int16_t)current_min;
    if (x >= 0 && x < world.terrain.mapWidth
        && y >= 0 && y < world.terrain.mapHeight
        && distanceGrid[x][y] >= 20 && distanceGrid[x][y] < result)
        result = distanceGrid[x][y];
    return result;
}

int32_t Pathfinder::relax_cell(const World& world, int32_t x, int32_t y) {
    bool changed_or_blocked = false;
    int16_t cell = distanceGrid[x][y];
    if (cell > 0 && cell != 3 && cell != 7) {
        int previous = cell < 20 ? SHRT_MAX : cell;
        int distance = distance_min(world, x - 1, y, previous);
        distance = distance_min(world, x + 1, y, distance);
        distance = distance_min(world, x, y - 1, distance);
        distance = distance_min(world, x, y + 1, distance);
        if (distance < previous) {
            distance = (int16_t)(distance + 1);
            if (cell == 2) {
                int step_x = 0;
                int step_y = 0;
                int tunnel_x = x;
                int tunnel_y = y;
                if (world.terrain.tileMap[x][y] == 7) step_y = -1;
                if (world.terrain.tileMap[x][y] == 8) step_x = -1;
                if (world.terrain.tileMap[x][y] == 9) step_x = 1;
                if (world.terrain.tileMap[x][y] == 10) step_y = 1;
                if (step_x != 0 || step_y != 0) {
                    int tunnel_distance = distance;
                    do {
                        tunnel_distance = (int16_t)(tunnel_distance + 1);
                        tunnel_x += step_x;
                        tunnel_y += step_y;
                    } while (distanceGrid[tunnel_x][tunnel_y] == 3);
                    distanceGrid[tunnel_x][tunnel_y] = (int16_t)tunnel_distance;
                    changed_or_blocked = true;
                }
            }
            distanceGrid[x][y] = (int16_t)distance;
        }
    } else {
        changed_or_blocked = true;
    }
    if (distanceGrid[targetX][targetY] >= 20) return 2;
    return changed_or_blocked ? 0 : 1;
}

void Pathfinder::flood_fill(const World& world) {
    bool keep_searching = true;
    bool destination_found = false;
    for (int pass = 50;
         keep_searching && !destination_found && pass > 0; --pass) {
        keep_searching = false;
        for (int x = anchorX; x >= 0; --x) {
            for (int y = anchorY; y >= 0; --y) {
                int result = relax_cell(world, x, y);
                keep_searching |= result == 0;
                destination_found |= result == 2;
            }
        }
        for (int x = anchorX; x < world.terrain.mapWidth; ++x) {
            for (int y = anchorY; y < world.terrain.mapHeight; ++y) {
                int result = relax_cell(world, x, y);
                keep_searching |= result == 0;
                destination_found |= result == 2;
            }
        }
        for (int x = anchorX; x >= 0; --x) {
            for (int y = anchorY; y < world.terrain.mapHeight; ++y) {
                int result = relax_cell(world, x, y);
                keep_searching |= result == 0;
                destination_found |= result == 2;
            }
        }
        for (int x = anchorX; x < world.terrain.mapWidth; ++x) {
            for (int y = anchorY; y >= 0; --y) {
                int result = relax_cell(world, x, y);
                keep_searching |= result == 0;
                destination_found |= result == 2;
            }
        }
    }
}

bool Pathfinder::find_step(const World& world, int32_t x, int32_t y,
                           int32_t current_distance) {
    if (!world.in_bounds(x, y) || distanceGrid[x][y] < 20
        || distanceGrid[x][y] >= current_distance)
        return false;
    nextX = x;
    nextY = y;
    return true;
}

void Pathfinder::clear_build_path_tile(const World& world, int32_t x,
                                       int32_t y) {
    if (x > 0 && x < world.terrain.mapWidth
        && y > 0 && y < world.terrain.mapHeight
        && ((world.terrain.tileMap[x][y] >= 7
             && world.terrain.tileMap[x][y] <= 10)
            || (world.terrain.tileMap[x][y] >= 21
                && world.terrain.tileMap[x][y] <= 24)
            || world.terrain.tileMap[x][y] == 70
            || world.terrain.tileMap[x][y] == 71)
        && distanceGrid[x][y] > 0) {
        distanceGrid[x][y] = 0;
    }
}

bool Pathfinder::is_tile_match(const World& world,
                                  const BuildController& build,
                                  bool connected,
                                  int32_t x, int32_t y,
                                  int32_t tile_a, int32_t tile_b,
                                  int32_t tile_c, int32_t tile_d,
                                  int32_t tile_e, int32_t tile_f,
                                  int32_t tile_g, int32_t tile_h,
                                  bool high_owner) const {
    if (connected || !world.in_bounds(x, y))
        return connected;
    int tile = world.terrain.tileMap[x][y];
    int owner = world.trackMap[x][y] & track::OWNER_MASK;
    owner = tile == tile::CROSSING && high_owner
                ? owner >> track::DIR_OWNER_SHIFT
                : owner & track::DIR_OWNER_MASK;
    if (owner == build.existingTrackOwnerCode
        && (tile == tile_a || tile == tile_b || tile == tile_c
            || tile == tile_d || tile == tile_e || tile == tile_f
            || tile == tile_g || tile == tile_h || tile == 76))
        return true;
    return false;
}

void Pathfinder::validate_step(World& world, BuildController& build,
                               const Economy& economy) {
    int x = targetX;
    int y = targetY;
    bool path_complete = true;
    bool walking = true;
    do {
        walking = true;
        nextX = x;
        nextY = y;
        int tile = world.terrain.tileMap[x][y];
        if (tile == 7 || tile == 10 || tile == 21 || tile == 23 || tile == 71) {
            clear_build_path_tile(world, x - 1, y);
            clear_build_path_tile(world, x + 1, y);
        }
        if (tile == 8 || tile == 9 || tile == 22 || tile == 24 || tile == 70) {
            clear_build_path_tile(world, x, y - 1);
            clear_build_path_tile(world, x, y + 1);
        }
        if (x == anchorX && y == anchorY) {
            walking = false;
        } else {
            int distance = distanceGrid[x][y];
            bool found;
            if (tieBreakCounter % 2 == 0) {
                found = find_step(world, x + 1, y, distance)
                        || find_step(world, x - 1, y, distance)
                        || find_step(world, x, y + 1, distance)
                        || find_step(world, x, y - 1, distance);
            } else {
                found = find_step(world, x, y + 1, distance)
                        || find_step(world, x, y - 1, distance)
                        || find_step(world, x + 1, y, distance)
                        || find_step(world, x - 1, y, distance);
            }
            if (!found) {
                walking = false;
                path_complete = false;
            }
        }
        ++tieBreakCounter;
        distanceGrid[x][y] = 9;
        x = nextX;
        y = nextY;

        int step_x = 0;
        int step_y = 0;
        if (world.terrain.tileMap[x][y] == 7) step_y = -1;
        if (world.terrain.tileMap[x][y] == 8) step_x = -1;
        if (world.terrain.tileMap[x][y] == 9) step_x = 1;
        if (world.terrain.tileMap[x][y] == 10) step_y = 1;
        if (step_x != 0 || step_y != 0) {
            do {
                distanceGrid[x][y] = 9;
                x += step_x;
                y += step_y;
            } while (distanceGrid[x][y] == 3);
        }
    } while (walking);

    for (int map_x = 0; map_x < world.terrain.mapWidth; ++map_x)
        for (int map_y = 0; map_y < world.terrain.mapHeight; ++map_y)
            if (distanceGrid[map_x][map_y] != 9)
                distanceGrid[map_x][map_y] = 0;
    if (!path_complete) return;

    for (int map_x = 0; map_x < world.terrain.mapWidth; ++map_x) {
        for (int map_y = 0; map_y < world.terrain.mapHeight; ++map_y) {
            if (distanceGrid[map_x][map_y] != 9) continue;
            bool left = map_x > 0
                        && distanceGrid[map_x - 1][map_y] == 9;
            bool right = map_x < world.terrain.mapWidth - 1
                         && distanceGrid[map_x + 1][map_y] == 9;
            bool up = map_y > 0
                      && distanceGrid[map_x][map_y - 1] == 9;
            bool down = map_y < world.terrain.mapHeight - 1
                        && distanceGrid[map_x][map_y + 1] == 9;
            if ((left ? 1 : 0) + (right ? 1 : 0)
                + (up ? 1 : 0) + (down ? 1 : 0) == 1) {
                left = is_tile_match(
                    world, build, left, map_x - 1, map_y,
                    71, 72, 74, 78, 80, 84, 88, 90, true);
                right = is_tile_match(
                    world, build, right, map_x + 1, map_y,
                    71, 73, 75, 78, 80, 85, 88, 90, true);
                up = is_tile_match(
                    world, build, up, map_x, map_y - 1,
                    70, 73, 74, 77, 79, 83, 87, 89, false);
                down = is_tile_match(
                    world, build, down, map_x, map_y + 1,
                    70, 72, 75, 77, 79, 86, 87, 89, false);
            }

            int old_tile = world.terrain.tileMap[map_x][map_y];
            bool joined_track = false;
            if (old_tile != 0 && old_tile > 28)
                build.selectionCost += economy.baseTrainSpeed;
            int cost_before_shape = build.selectionCost;
            if (left && right) {
                if (old_tile == 22) world.terrain.tileMap[map_x][map_y] = 78;
                else if (old_tile == 24) world.terrain.tileMap[map_x][map_y] = 80;
                else if (old_tile == 9) {
                    world.terrain.tileMap[map_x][map_y] = 85;
                    build.selectionCost += economy.moneyBaseA;
                } else if (old_tile == 8) {
                    world.terrain.tileMap[map_x][map_y] = 84;
                    build.selectionCost += economy.moneyBaseA;
                } else if (old_tile == 70) {
                    world.terrain.tileMap[map_x][map_y] = tile::CROSSING;
                    joined_track = true;
                    world.trackMap[map_x][map_y] = (int8_t)(
                        track::PRESENT | (world.trackMap[map_x][map_y] & track::OWNER_MASK)
                        | build.previewTrackOwnerCode << track::DIR_OWNER_SHIFT);
                } else if (world.terrain.tileElevation[map_x][map_y] == -1) {
                    world.terrain.tileMap[map_x][map_y] = 81;
                    build.selectionCost += economy.moneyBaseB;
                } else {
                    world.terrain.tileMap[map_x][map_y] = 71;
                }
            } else if (up && down) {
                if (old_tile == 21) world.terrain.tileMap[map_x][map_y] = 77;
                else if (old_tile == 23) world.terrain.tileMap[map_x][map_y] = 79;
                else if (old_tile == 7) {
                    world.terrain.tileMap[map_x][map_y] = 83;
                    build.selectionCost += economy.moneyBaseA;
                } else if (old_tile == 10) {
                    world.terrain.tileMap[map_x][map_y] = 86;
                    build.selectionCost += economy.moneyBaseA;
                } else if (old_tile == 71) {
                    world.terrain.tileMap[map_x][map_y] = tile::CROSSING;
                    joined_track = true;
                    world.trackMap[map_x][map_y] = (int8_t)(
                        track::PRESENT | (world.trackMap[map_x][map_y] & track::OWNER_MASK) << track::DIR_OWNER_SHIFT
                        | build.previewTrackOwnerCode);
                } else if (world.terrain.tileElevation[map_x][map_y] == -1) {
                    world.terrain.tileMap[map_x][map_y] = 82;
                    build.selectionCost += economy.moneyBaseB;
                } else {
                    world.terrain.tileMap[map_x][map_y] = 70;
                }
            } else if (left && up) world.terrain.tileMap[map_x][map_y] = 75;
            else if (right && up) world.terrain.tileMap[map_x][map_y] = 72;
            else if (left && down) world.terrain.tileMap[map_x][map_y] = 73;
            else if (right && down) world.terrain.tileMap[map_x][map_y] = 74;

            if (!joined_track && old_tile != world.terrain.tileMap[map_x][map_y]) {
                world.trackMap[map_x][map_y] =
                    (int8_t)(track::PRESENT | build.previewTrackOwnerCode);
                if (cost_before_shape == build.selectionCost)
                    build.selectionCost += economy.accel1;
            }
            if (joined_track)
                build.selectionCost +=
                    economy.brake1 + economy.accel1 + economy.baseTrainSpeed;
        }
    }
}

void Pathfinder::scan_track_tiles(const World& world,
                                  BuildController& build) const {
    build.trackTileCount = 0;
    for (int x = 0; x < world.terrain.mapWidth; ++x) {
        for (int y = 0; y < world.terrain.mapHeight; ++y) {
            if (world.trackMap[x][y] == track::PRESENT) ++build.trackTileCount;
            if (world.terrain.tileMap[x][y] != tile::CROSSING) continue;
            int low_owner = world.trackMap[x][y] & track::DIR_OWNER_MASK;
            int high_owner = (world.trackMap[x][y] & track::OWNER_MASK) >> track::DIR_OWNER_SHIFT;
            if (low_owner == 0) ++build.trackTileCount;
            if (high_owner == 0) ++build.trackTileCount;
        }
    }
}

void Pathfinder::clear_distance_grid(const World& world) {
    for (int x = 0; x < world.terrain.mapWidth; ++x)
        for (int y = 0; y < world.terrain.mapHeight; ++y)
            distanceGrid[x][y] = 0;

    for (int x = 0; x < world.terrain.mapWidth; ++x) {
        for (int y = 0; y < world.terrain.mapHeight; ++y) {
            if (distanceGrid[x][y] != 0) continue;
            int16_t passable = world.trackMap[x][y] == 0 ? 1 : 0;
            int tile = world.terrain.tileMap[x][y];
            if (passable == 1) {
                if (world.terrain.tileElevation[x][y] == -1) passable = 0;
                if (tile != 0 && tile != 21 && tile != 22
                    && tile != 23 && tile != 24
                    && (tile < tile::SCENERY_FIRST || tile > tile::SCENERY_LAST))
                    passable = 0;
            }
            if (tile == 71 && x > 0 && x < world.terrain.mapWidth - 1
                && world.trackMap[x][y] == world.trackMap[x - 1][y]
                && world.trackMap[x][y] == world.trackMap[x + 1][y])
                passable = 1;
            if (tile == 70 && y > 0 && y < world.terrain.mapHeight - 1
                && world.trackMap[x][y] == world.trackMap[x][y - 1]
                && world.trackMap[x][y] == world.trackMap[x][y + 1])
                passable = 1;
            distanceGrid[x][y] = passable;

            if (tile == 7) {
                int other_y = y;
                bool found_exit = false;
                bool stop;
                do {
                    stop = false;
                    --other_y;
                    if (world.terrain.tileElevation[x][other_y] != -1
                        || world.terrain.tileMap[x][other_y] != 0
                        || world.trackMap[x][other_y] != 0)
                        stop = true;
                    if (other_y == 0) stop = true;
                    if (world.terrain.tileMap[x][other_y] == 10) {
                        stop = true;
                        found_exit = true;
                    }
                } while (!stop);
                if (found_exit) {
                    for (int tunnel_y = other_y; tunnel_y <= y; ++tunnel_y)
                        distanceGrid[x][tunnel_y] = 3;
                    distanceGrid[x][other_y] = 2;
                    distanceGrid[x][y] = 2;
                }
            }
            if (tile != 8) continue;
            int other_x = x;
            bool found_exit = false;
            bool stop;
            do {
                stop = false;
                --other_x;
                if (world.terrain.tileElevation[other_x][y] != -1
                    || world.terrain.tileMap[other_x][y] != 0
                    || world.trackMap[other_x][y] != 0)
                    stop = true;
                if (other_x == 0) stop = true;
                if (world.terrain.tileMap[other_x][y] == 9) {
                    stop = true;
                    found_exit = true;
                }
            } while (!stop);
            if (!found_exit) continue;
            for (int tunnel_x = other_x; tunnel_x <= x; ++tunnel_x)
                distanceGrid[tunnel_x][y] = 3;
            distanceGrid[other_x][y] = 2;
            distanceGrid[x][y] = 2;
        }
    }
}

bool Pathfinder::try_build_path(World& world, BuildController& build,
                                const Economy& economy) {
    distanceGrid.resize(world.terrain.mapWidth, world.terrain.mapHeight);
    searchLimit = 10;
    build.selectionCost = 0;
    clear_distance_grid(world);
    distanceGrid[anchorX][anchorY] = 20;
    flood_fill(world);
    if (distanceGrid[targetX][targetY] < 20) {
        distanceGrid.clear();
        return false;
    }
    validate_step(world, build, economy);
    distanceGrid.clear();
    scan_track_tiles(world, build);
    if (build.trackTileCount == 0) return false;
    return build.selectionCost != 0;
}
