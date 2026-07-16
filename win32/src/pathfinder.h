// Scratch state for the track path search (see game/pathfinding.cpp).
#ifndef PATHFINDER_H
#define PATHFINDER_H
#include "support/grid.h"
#include <cstdint>

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
};

#endif  // PATHFINDER_H
