// Build-path search, validation, and preview tiles.
#include "game.h"

#include <algorithm>
#include <array>
#include <limits.h>

namespace {

namespace path_distance {
constexpr int16_t kUnvisited = 0;
constexpr int16_t kPassable = 1;
constexpr int16_t kTunnelEndpoint = 2;
constexpr int16_t kTunnelInterior = 3;
constexpr int16_t kReserved = 7;
constexpr int16_t kVisited = 9;
constexpr int16_t kSearchSeed = 20;
constexpr int32_t kFloodFillPassCount = 50;
constexpr int32_t kSearchLimit = 10;
}  // namespace path_distance

namespace path_tile {
// These names describe the step used by path search, not a visual tile type.
constexpr int32_t kStepUp = tile::CURVE_UP;
constexpr int32_t kStepLeft = tile::CURVE_LEFT;
constexpr int32_t kStepRight = tile::CURVE_RIGHT;
constexpr int32_t kStepDown = tile::CURVE_DOWN;
// The source proves these stable path/build bytes, but not richer sprite names.
constexpr int32_t kOpenGroundVariantA = track_build_tile::OpenGroundVariantA;
constexpr int32_t kOpenGroundVariantB = track_build_tile::OpenGroundVariantB;
constexpr int32_t kOpenGroundVariantC = track_build_tile::OpenGroundVariantC;
constexpr int32_t kOpenGroundVariantD = track_build_tile::OpenGroundVariantD;
constexpr int32_t kAlongY = track_build_tile::AlongY;
constexpr int32_t kAlongX = track_build_tile::AlongX;
constexpr int32_t kTrackCornerVariantA = track_build_tile::CornerVariantA;
constexpr int32_t kTrackCornerVariantB = track_build_tile::CornerVariantB;
constexpr int32_t kTrackCornerVariantC = track_build_tile::CornerVariantC;
constexpr int32_t kTrackCornerVariantD = track_build_tile::CornerVariantD;
}  // namespace path_tile

struct GridStep {
    int32_t x;
    int32_t y;
};

constexpr GridStep path_step_for_tile(int32_t tile_code) {
    switch (tile_code) {
        case path_tile::kStepUp: return {0, -1};
        case path_tile::kStepLeft: return {-1, 0};
        case path_tile::kStepRight: return {1, 0};
        case path_tile::kStepDown: return {0, 1};
        default: return {0, 0};
    }
}

constexpr std::array<int32_t, 10> kBuildPathTileCodes = {
    path_tile::kStepUp, path_tile::kStepLeft, path_tile::kStepRight,
    path_tile::kStepDown, path_tile::kOpenGroundVariantA,
    path_tile::kOpenGroundVariantB, path_tile::kOpenGroundVariantC,
    path_tile::kOpenGroundVariantD,
    path_tile::kAlongY, path_tile::kAlongX,
};

constexpr std::array<int32_t, 5> kHorizontalPathNeighborCodes = {
    path_tile::kStepUp, path_tile::kStepDown,
    path_tile::kOpenGroundVariantA, path_tile::kOpenGroundVariantC,
    path_tile::kAlongX,
};

constexpr std::array<int32_t, 5> kVerticalPathNeighborCodes = {
    path_tile::kStepLeft, path_tile::kStepRight,
    path_tile::kOpenGroundVariantB, path_tile::kOpenGroundVariantD,
    path_tile::kAlongY,
};

constexpr std::array<int32_t, 8> kLeftNeighborTrackCodes = {
    path_tile::kAlongX, path_tile::kTrackCornerVariantA,
    path_tile::kTrackCornerVariantC, track_turn_tile::VariantB,
    track_turn_tile::VariantD, construction_tile::HorizontalB,
    tile::HORIZONTAL_STATION, tile::HORIZONTAL_TERMINAL,
};

constexpr std::array<int32_t, 8> kRightNeighborTrackCodes = {
    path_tile::kAlongX, path_tile::kTrackCornerVariantB,
    path_tile::kTrackCornerVariantD, track_turn_tile::VariantB,
    track_turn_tile::VariantD, construction_tile::HorizontalC,
    tile::HORIZONTAL_STATION, tile::HORIZONTAL_TERMINAL,
};

constexpr std::array<int32_t, 8> kUpNeighborTrackCodes = {
    path_tile::kAlongY, path_tile::kTrackCornerVariantB,
    path_tile::kTrackCornerVariantC, track_turn_tile::VariantA,
    track_turn_tile::VariantC, construction_tile::VerticalB,
    tile::VERTICAL_STATION, tile::VERTICAL_TERMINAL,
};

constexpr std::array<int32_t, 8> kDownNeighborTrackCodes = {
    path_tile::kAlongY, path_tile::kTrackCornerVariantA,
    path_tile::kTrackCornerVariantD, track_turn_tile::VariantA,
    track_turn_tile::VariantC, construction_tile::VerticalC,
    tile::VERTICAL_STATION, tile::VERTICAL_TERMINAL,
};

template <typename T, std::size_t N>
constexpr bool contains_code(const std::array<T, N>& codes, T code) {
    return std::find(codes.begin(), codes.end(), code) != codes.end();
}

bool contains_code(std::span<const int32_t> codes, int32_t code) {
    return std::find(codes.begin(), codes.end(), code) != codes.end();
}

bool is_open_ground_tile(int32_t tile_code) {
    return tile_code == 0
        || (tile_code >= path_tile::kOpenGroundVariantA
            && tile_code <= path_tile::kOpenGroundVariantD)
        || (tile_code >= tile::SCENERY_FIRST && tile_code <= tile::SCENERY_LAST);
}

}  // namespace

int16_t Pathfinder::distance_min(const World& world, int32_t x, int32_t y,
                                 int32_t current_min) const {
    int16_t result = (int16_t)current_min;
    if (x >= 0 && x < world.terrain.mapWidth
        && y >= 0 && y < world.terrain.mapHeight
        && distanceGrid[x][y] >= path_distance::kSearchSeed
        && distanceGrid[x][y] < result)
        result = distanceGrid[x][y];
    return result;
}

Pathfinder::RelaxationResult Pathfinder::relax_cell(const World& world,
                                                    int32_t x, int32_t y) {
    bool changed_or_blocked = false;
    const int16_t distance_cell = distanceGrid[x][y];
    if (distance_cell > path_distance::kUnvisited
        && distance_cell != path_distance::kTunnelInterior
        && distance_cell != path_distance::kReserved) {
        const int previous_distance = distance_cell < path_distance::kSearchSeed
            ? SHRT_MAX
            : distance_cell;
        int candidate_distance = distance_min(world, x - 1, y, previous_distance);
        candidate_distance = distance_min(world, x + 1, y, candidate_distance);
        candidate_distance = distance_min(world, x, y - 1, candidate_distance);
        candidate_distance = distance_min(world, x, y + 1, candidate_distance);
        if (candidate_distance < previous_distance) {
            candidate_distance =
                static_cast<int16_t>(candidate_distance + 1);
            if (distance_cell == path_distance::kTunnelEndpoint) {
                int tunnel_x = x;
                int tunnel_y = y;
                const GridStep tunnel_step = path_step_for_tile(
                    world.terrain.tileMap[x][y]);
                if (tunnel_step.x != 0 || tunnel_step.y != 0) {
                    int tunnel_distance = candidate_distance;
                    do {
                        tunnel_distance = (int16_t)(tunnel_distance + 1);
                        tunnel_x += tunnel_step.x;
                        tunnel_y += tunnel_step.y;
                    } while (distanceGrid[tunnel_x][tunnel_y]
                             == path_distance::kTunnelInterior);
                    distanceGrid[tunnel_x][tunnel_y] = (int16_t)tunnel_distance;
                    changed_or_blocked = true;
                }
            }
            distanceGrid[x][y] = (int16_t)candidate_distance;
        }
    } else {
        changed_or_blocked = true;
    }
    if (distanceGrid[targetX][targetY] >= path_distance::kSearchSeed)
        return RelaxationResult::DestinationFound;
    return changed_or_blocked ? RelaxationResult::ChangedOrBlocked
                              : RelaxationResult::Stable;
}

void Pathfinder::flood_fill(const World& world) {
    bool keep_searching = true;
    bool destination_found = false;
    for (int pass = path_distance::kFloodFillPassCount;
         keep_searching && !destination_found && pass > 0; --pass) {
        keep_searching = false;
        for (int x = anchorX; x >= 0; --x) {
            for (int y = anchorY; y >= 0; --y) {
                const RelaxationResult relaxation_result =
                    relax_cell(world, x, y);
                keep_searching |= relaxation_result
                                  == RelaxationResult::ChangedOrBlocked;
                destination_found |= relaxation_result
                                     == RelaxationResult::DestinationFound;
            }
        }
        for (int x = anchorX; x < world.terrain.mapWidth; ++x) {
            for (int y = anchorY; y < world.terrain.mapHeight; ++y) {
                const RelaxationResult relaxation_result =
                    relax_cell(world, x, y);
                keep_searching |= relaxation_result
                                  == RelaxationResult::ChangedOrBlocked;
                destination_found |= relaxation_result
                                     == RelaxationResult::DestinationFound;
            }
        }
        for (int x = anchorX; x >= 0; --x) {
            for (int y = anchorY; y < world.terrain.mapHeight; ++y) {
                const RelaxationResult relaxation_result =
                    relax_cell(world, x, y);
                keep_searching |= relaxation_result
                                  == RelaxationResult::ChangedOrBlocked;
                destination_found |= relaxation_result
                                     == RelaxationResult::DestinationFound;
            }
        }
        for (int x = anchorX; x < world.terrain.mapWidth; ++x) {
            for (int y = anchorY; y >= 0; --y) {
                const RelaxationResult relaxation_result =
                    relax_cell(world, x, y);
                keep_searching |= relaxation_result
                                  == RelaxationResult::ChangedOrBlocked;
                destination_found |= relaxation_result
                                     == RelaxationResult::DestinationFound;
            }
        }
    }
}

bool Pathfinder::find_step(const World& world, int32_t x, int32_t y,
                           int32_t current_distance) {
    if (!world.in_bounds(x, y)
        || distanceGrid[x][y] < path_distance::kSearchSeed
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
        && contains_code(kBuildPathTileCodes, world.terrain.tileMap[x][y])
        && distanceGrid[x][y] > path_distance::kUnvisited) {
        distanceGrid[x][y] = path_distance::kUnvisited;
    }
}

bool Pathfinder::is_tile_match(const World& world,
                                  const BuildController& build,
                                  bool connected,
                                  int32_t x, int32_t y,
                                  std::span<const int32_t> allowed_tile_codes,
                                  bool high_owner) const {
    if (connected || !world.in_bounds(x, y))
        return connected;
    const int tile_code = world.terrain.tileMap[x][y];
    int owner_code = world.trackMap[x][y] & track::OWNER_MASK;
    owner_code = tile_code == tile::CROSSING && high_owner
        ? owner_code >> track::DIR_OWNER_SHIFT
        : owner_code & track::DIR_OWNER_MASK;
    if (owner_code == build.existingTrackOwnerCode
        && (contains_code(allowed_tile_codes, tile_code)
            || tile_code == tile::CROSSING))
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
        const int tile_code = world.terrain.tileMap[x][y];
        if (contains_code(kHorizontalPathNeighborCodes, tile_code)) {
            clear_build_path_tile(world, x - 1, y);
            clear_build_path_tile(world, x + 1, y);
        }
        if (contains_code(kVerticalPathNeighborCodes, tile_code)) {
            clear_build_path_tile(world, x, y - 1);
            clear_build_path_tile(world, x, y + 1);
        }
        if (x == anchorX && y == anchorY) {
            walking = false;
        } else {
            const int current_distance = distanceGrid[x][y];
            bool step_found;
            if (tieBreakCounter % 2 == 0) {
                step_found = find_step(world, x + 1, y, current_distance)
                        || find_step(world, x - 1, y, current_distance)
                        || find_step(world, x, y + 1, current_distance)
                        || find_step(world, x, y - 1, current_distance);
            } else {
                step_found = find_step(world, x, y + 1, current_distance)
                        || find_step(world, x, y - 1, current_distance)
                        || find_step(world, x + 1, y, current_distance)
                        || find_step(world, x - 1, y, current_distance);
            }
            if (!step_found) {
                walking = false;
                path_complete = false;
            }
        }
        ++tieBreakCounter;
        distanceGrid[x][y] = path_distance::kVisited;
        x = nextX;
        y = nextY;

        const GridStep tunnel_step = path_step_for_tile(
            world.terrain.tileMap[x][y]);
        if (tunnel_step.x != 0 || tunnel_step.y != 0) {
            do {
                distanceGrid[x][y] = path_distance::kVisited;
                x += tunnel_step.x;
                y += tunnel_step.y;
            } while (distanceGrid[x][y] == path_distance::kTunnelInterior);
        }
    } while (walking);

    for (int map_x = 0; map_x < world.terrain.mapWidth; ++map_x)
        for (int map_y = 0; map_y < world.terrain.mapHeight; ++map_y)
            if (distanceGrid[map_x][map_y] != path_distance::kVisited)
                distanceGrid[map_x][map_y] = path_distance::kUnvisited;
    if (!path_complete) return;

    for (int map_x = 0; map_x < world.terrain.mapWidth; ++map_x) {
        for (int map_y = 0; map_y < world.terrain.mapHeight; ++map_y) {
            if (distanceGrid[map_x][map_y] != path_distance::kVisited)
                continue;
            bool has_left_neighbor = map_x > 0
                                     && distanceGrid[map_x - 1][map_y]
                                        == path_distance::kVisited;
            bool has_right_neighbor = map_x < world.terrain.mapWidth - 1
                                      && distanceGrid[map_x + 1][map_y]
                                         == path_distance::kVisited;
            bool has_upper_neighbor = map_y > 0
                                      && distanceGrid[map_x][map_y - 1]
                                         == path_distance::kVisited;
            bool has_lower_neighbor = map_y < world.terrain.mapHeight - 1
                                      && distanceGrid[map_x][map_y + 1]
                                         == path_distance::kVisited;
            if ((has_left_neighbor ? 1 : 0) + (has_right_neighbor ? 1 : 0)
                + (has_upper_neighbor ? 1 : 0)
                + (has_lower_neighbor ? 1 : 0) == 1) {
                has_left_neighbor = is_tile_match(
                    world, build, has_left_neighbor, map_x - 1, map_y,
                    kLeftNeighborTrackCodes, true);
                has_right_neighbor = is_tile_match(
                    world, build, has_right_neighbor, map_x + 1, map_y,
                    kRightNeighborTrackCodes, true);
                has_upper_neighbor = is_tile_match(
                    world, build, has_upper_neighbor, map_x, map_y - 1,
                    kUpNeighborTrackCodes, false);
                has_lower_neighbor = is_tile_match(
                    world, build, has_lower_neighbor, map_x, map_y + 1,
                    kDownNeighborTrackCodes, false);
            }

            const int previous_tile_code =
                world.terrain.tileMap[map_x][map_y];
            bool joined_track = false;
            if (previous_tile_code != 0 && previous_tile_code > 28)
                build.selectionCost += economy.baseTrainSpeedEarly;
            const int cost_before_shape = build.selectionCost;
            if (has_left_neighbor && has_right_neighbor) {
                if (previous_tile_code == path_tile::kOpenGroundVariantB)
                    world.terrain.tileMap[map_x][map_y] = track_turn_tile::VariantB;
                else if (previous_tile_code == path_tile::kOpenGroundVariantD)
                    world.terrain.tileMap[map_x][map_y] = track_turn_tile::VariantD;
                else if (previous_tile_code == path_tile::kStepRight) {
                    world.terrain.tileMap[map_x][map_y] = construction_tile::HorizontalC;
                    build.selectionCost += economy.moneyBasePrimary;
                } else if (previous_tile_code == path_tile::kStepLeft) {
                    world.terrain.tileMap[map_x][map_y] = construction_tile::HorizontalB;
                    build.selectionCost += economy.moneyBasePrimary;
                } else if (previous_tile_code == path_tile::kAlongY) {
                    world.terrain.tileMap[map_x][map_y] = tile::CROSSING;
                    joined_track = true;
                    world.trackMap[map_x][map_y] = (int8_t)(
                        track::PRESENT | (world.trackMap[map_x][map_y] & track::OWNER_MASK)
                        | build.previewTrackOwnerCode << track::DIR_OWNER_SHIFT);
                } else if (world.terrain.tileElevation[map_x][map_y] == -1) {
                    world.terrain.tileMap[map_x][map_y] = construction_tile::HorizontalA;
                    build.selectionCost += economy.moneyBaseSecondary;
                } else {
                    world.terrain.tileMap[map_x][map_y] = path_tile::kAlongX;
                }
            } else if (has_upper_neighbor && has_lower_neighbor) {
                if (previous_tile_code == path_tile::kOpenGroundVariantA)
                    world.terrain.tileMap[map_x][map_y] = track_turn_tile::VariantA;
                else if (previous_tile_code == path_tile::kOpenGroundVariantC)
                    world.terrain.tileMap[map_x][map_y] = track_turn_tile::VariantC;
                else if (previous_tile_code == path_tile::kStepUp) {
                    world.terrain.tileMap[map_x][map_y] = construction_tile::VerticalB;
                    build.selectionCost += economy.moneyBasePrimary;
                } else if (previous_tile_code == path_tile::kStepDown) {
                    world.terrain.tileMap[map_x][map_y] = construction_tile::VerticalC;
                    build.selectionCost += economy.moneyBasePrimary;
                } else if (previous_tile_code == path_tile::kAlongX) {
                    world.terrain.tileMap[map_x][map_y] = tile::CROSSING;
                    joined_track = true;
                    world.trackMap[map_x][map_y] = (int8_t)(
                        track::PRESENT | (world.trackMap[map_x][map_y] & track::OWNER_MASK) << track::DIR_OWNER_SHIFT
                        | build.previewTrackOwnerCode);
                } else if (world.terrain.tileElevation[map_x][map_y] == -1) {
                    world.terrain.tileMap[map_x][map_y] = construction_tile::VerticalA;
                    build.selectionCost += economy.moneyBaseSecondary;
                } else {
                    world.terrain.tileMap[map_x][map_y] = path_tile::kAlongY;
                }
            } else if (has_left_neighbor && has_upper_neighbor)
                world.terrain.tileMap[map_x][map_y] = path_tile::kTrackCornerVariantD;
            else if (has_right_neighbor && has_upper_neighbor)
                world.terrain.tileMap[map_x][map_y] = path_tile::kTrackCornerVariantA;
            else if (has_left_neighbor && has_lower_neighbor)
                world.terrain.tileMap[map_x][map_y] = path_tile::kTrackCornerVariantB;
            else if (has_right_neighbor && has_lower_neighbor)
                world.terrain.tileMap[map_x][map_y] = path_tile::kTrackCornerVariantC;

            if (!joined_track
                && previous_tile_code != world.terrain.tileMap[map_x][map_y]) {
                world.trackMap[map_x][map_y] =
                    (int8_t)(track::PRESENT | build.previewTrackOwnerCode);
                if (cost_before_shape == build.selectionCost)
                    build.selectionCost += economy.accelerationEarly;
            }
            if (joined_track)
                build.selectionCost +=
                    economy.brakingEarly + economy.accelerationEarly
                    + economy.baseTrainSpeedEarly;
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
            distanceGrid[x][y] = path_distance::kUnvisited;

    for (int x = 0; x < world.terrain.mapWidth; ++x) {
        for (int y = 0; y < world.terrain.mapHeight; ++y) {
            if (distanceGrid[x][y] != path_distance::kUnvisited)
                continue;
            int16_t passable = world.trackMap[x][y] == 0
                ? path_distance::kPassable
                : path_distance::kUnvisited;
            const int tile_code = world.terrain.tileMap[x][y];
            if (passable == path_distance::kPassable) {
                if (world.terrain.tileElevation[x][y] == -1)
                    passable = path_distance::kUnvisited;
                if (!is_open_ground_tile(tile_code))
                    passable = path_distance::kUnvisited;
            }
            if (tile_code == path_tile::kAlongX
                && x > 0 && x < world.terrain.mapWidth - 1
                && world.trackMap[x][y] == world.trackMap[x - 1][y]
                && world.trackMap[x][y] == world.trackMap[x + 1][y])
                passable = path_distance::kPassable;
            if (tile_code == path_tile::kAlongY
                && y > 0 && y < world.terrain.mapHeight - 1
                && world.trackMap[x][y] == world.trackMap[x][y - 1]
                && world.trackMap[x][y] == world.trackMap[x][y + 1])
                passable = path_distance::kPassable;
            distanceGrid[x][y] = passable;

            if (tile_code == path_tile::kStepUp) {
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
                    if (world.terrain.tileMap[x][other_y] == tile::CURVE_DOWN) {
                        stop = true;
                        found_exit = true;
                    }
                } while (!stop);
                if (found_exit) {
                    for (int tunnel_y = other_y; tunnel_y <= y; ++tunnel_y)
                        distanceGrid[x][tunnel_y] = path_distance::kTunnelInterior;
                    distanceGrid[x][other_y] = path_distance::kTunnelEndpoint;
                    distanceGrid[x][y] = path_distance::kTunnelEndpoint;
                }
            }
            if (tile_code != path_tile::kStepLeft) continue;
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
                if (world.terrain.tileMap[other_x][y] == tile::CURVE_RIGHT) {
                    stop = true;
                    found_exit = true;
                }
            } while (!stop);
            if (!found_exit) continue;
            for (int tunnel_x = other_x; tunnel_x <= x; ++tunnel_x)
                distanceGrid[tunnel_x][y] = path_distance::kTunnelInterior;
            distanceGrid[other_x][y] = path_distance::kTunnelEndpoint;
            distanceGrid[x][y] = path_distance::kTunnelEndpoint;
        }
    }
}

bool Pathfinder::try_build_path(World& world, BuildController& build,
                                const Economy& economy) {
    distanceGrid.resize(world.terrain.mapWidth, world.terrain.mapHeight);
    searchLimit = path_distance::kSearchLimit;
    build.selectionCost = 0;
    clear_distance_grid(world);
    distanceGrid[anchorX][anchorY] = path_distance::kSearchSeed;
    flood_fill(world);
    if (distanceGrid[targetX][targetY] < path_distance::kSearchSeed) {
        distanceGrid.clear();
        return false;
    }
    validate_step(world, build, economy);
    distanceGrid.clear();
    scan_track_tiles(world, build);
    if (build.trackTileCount == 0) return false;
    return build.selectionCost != 0;
}
