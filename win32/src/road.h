// Road-transport proof of concept (isolated, opt-in via TRAIN_TYCOON_ROAD_POC).
//
// Goal: prove the architecture we discussed -- a road layer that mirrors the
// rail `trackMap` (a grid, so disconnected roads are free), a route search that
// plays the same role as `Pathfinder` on that layer, and a vehicle that steps
// tile-to-tile with `phase` interpolation the same way trains do, so the train
// animation can later be swapped in for the graphics.
//
// Nothing here is wired into the shipped game unless the PoC flag is set, and no
// state is saved, so the fixed-seed golden behavior is untouched.
#ifndef ROAD_H
#define ROAD_H
#include "support/grid.h"
#include <cstdint>
#include <functional>
#include <vector>

struct RoadPoint {
    int x = 0;
    int y = 0;
};

// One road vehicle. Like a TrainCar it lives on a tile with a sub-tile `phase`;
// here it follows a precomputed ordered tile route and ping-pongs along it. This
// is the "different graphics, same movement" seam: the route + phase is all a
// car needs; the renderer interpolates between the two tiles it is between.
struct RoadVehicle {
    static constexpr int kStepsPerTile = 8;  // sub-tile frames, matching trains

    std::vector<RoadPoint> route;
    int segment = 0;      // index of route[segment] -> route[segment+1]
    int phase = 0;        // 0..kStepsPerTile along the current segment
    bool forward = true;  // ping-pong direction along the route
    int direction = 0;    // 0..3 facing, derived from the current segment

    bool active() const { return route.size() >= 2; }

    // Advance one sub-tile step; steps to the next tile at phase wrap and
    // reverses at either end so the demo car loops forever.
    void step();

    // The tile the car is leaving / heading toward this segment.
    RoadPoint from_tile() const;
    RoadPoint to_tile() const;
};

// The road layer: one byte per cell (0 = none, 1 = road), exactly the shape of
// `trackMap`. Disconnected roads are just isolated 1s; a route is searched on
// demand, never stored as an explicit graph -- same as the rail system.
struct RoadNetwork {
    Grid<uint8_t> roadMap;

    void resize(int width, int height) { roadMap.resize(width, height); }
    void clear() { roadMap.clear(); }

    // Breadth-first search from `start` over passable cells, returning the path
    // to the farthest reachable cell (ordered start..farthest). This is the road
    // analogue of Pathfinder::flood_fill + back-walk: unit cost, 4-connected,
    // elevation used only via `passable`. Swap this for weighted Dijkstra later
    // if roads should prefer terrain -- the callers do not change.
    std::vector<RoadPoint> plan_longest_route(
        int width, int height,
        const std::function<bool(int, int)>& passable, RoadPoint start) const;

    // Mark every tile of `path` as road in roadMap.
    void mark(const std::vector<RoadPoint>& path);
};

#endif  // ROAD_H
