// Scratch state for the track path search (see game/pathfinding.cpp).
#ifndef PATHFINDER_H
#define PATHFINDER_H
#include "support/grid.h"
#include <cstdint>

struct World;
struct BuildController;
struct Economy;

// Pathfinder owns the breadth-first search scratch: a per-cell distance grid
// filled from the goal, plus the back-walk cursor and tie-break counter. The
// search result and tie-break order are unchanged from the legacy code.
struct Pathfinder {
    Grid<int16_t> distanceGrid;  // BFS distance fill (was pathDistanceGrid)
    int32_t searchLimit = 0;     // fill bound (was pathSearchLimit)
    int32_t nextX = 0;           // back-walk cursor x (was nextPathX)
    int32_t nextY = 0;           // back-walk cursor y (was nextPathY)
    int32_t tieBreakCounter = 0; // alternates step order (was pathTieBreakCounter)

    // Endpoints of the path being built. anchor is the reference endpoint (the
    // city station in AI gen, the first tile the player picks): it is the
    // reverse-BFS seed (distance 20) and the back-walk terminus. target is the
    // endpoint the track reaches toward (the industry station / second pick):
    // the flood-fill must reach it and the back-walk starts there.
    int32_t anchorX = 0;
    int32_t anchorY = 0;
    int32_t targetX = 0;
    int32_t targetY = 0;

    int16_t distance_min(const World& world, int32_t x, int32_t y,
                         int32_t current_min) const;
    int32_t relax_cell(const World& world, int32_t x, int32_t y);
    void flood_fill(const World& world);
    bool find_step(const World& world, int32_t x, int32_t y,
                   int32_t current_distance);
    void clear_build_path_tile(const World& world, int32_t x, int32_t y);
    bool is_tile_match(const World& world, const BuildController& build,
                       bool connected, int32_t x, int32_t y,
                       int32_t tile_a, int32_t tile_b, int32_t tile_c,
                       int32_t tile_d, int32_t tile_e, int32_t tile_f,
                       int32_t tile_g, int32_t tile_h,
                       bool high_owner) const;
    void validate_step(World& world, BuildController& build,
                       const Economy& economy);
    void scan_track_tiles(const World& world, BuildController& build) const;
    void clear_distance_grid(const World& world);
    bool try_build_path(World& world, BuildController& build,
                        const Economy& economy);
};

#endif  // PATHFINDER_H
