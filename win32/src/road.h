// Road-transport subsystem: buses and trucks over a road tile layer.
//
// Roads are tileMap codes 42..48 (the same shapes map-gen lays for city
// streets), so they persist (save slot 1) and render through the normal map
// path for free. `RoadNetwork::roadMap`/`roadComponent` are derived caches
// rebuilt from tileMap; routing is an on-demand BFS over those tiles, so
// joining a built road to a town's streets is "live" with no stored graph to
// keep in sync. `RoadVehicle` steps tile-to-tile with `phase` interpolation the
// same way trains do, which lets the renderer reuse the wagon sprites.
#ifndef ROAD_H
#define ROAD_H
#include "game_constants.h"
#include "support/grid.h"
#include <array>
#include <cstdint>
#include <functional>
#include <vector>

class Terrain;

// Cost to lay one new road tile. Deliberately cheap next to rail (a rail tile is
// hundreds-to-thousands via economy.baseTrainSpeedEarly/accelerationEarly) so a short road + a few
// trucks undercuts a rail line for close towns. Tuned further in the balance pass.
constexpr int kRoadTileCost = 200;

// A road stop is a ServiceSite whose type nibble is this (rail depots/stations
// use type 1). It sits on clear ground beside a road, generates/accepts cargo
// from nearby city/industry through the same site loop stations use, and is
// where buses and trucks load, unload, and are bought.
constexpr int kRoadStopSiteType = service_site_type::Road;
constexpr int kRoadStopCost = 500;

// Vehicle prices and per-stop running cost. Cheap on purpose: the road system's
// whole point is that buying several buses/trucks beats a rail line for short,
// close routes. Balanced further in the balance pass.
constexpr int kBusCost = 8000;
constexpr int kTruckCost = 6000;
constexpr int kRoadRunCost = 4;  // charged each time a vehicle services a stop
constexpr int kBusCapacity = 40;
constexpr int kTruckCapacity = 30;

namespace road_vehicle_type {
enum Id : int32_t {
    Bus = 2,
    Truck = 4,
};
}  // namespace road_vehicle_type

namespace road_direction {
enum Id : int32_t {
    PositiveX = 0,
    NegativeX = 1,
    PositiveY = 2,
    NegativeY = 3,
};
}  // namespace road_direction

struct RoadPoint {
    int x = 0;
    int y = 0;

    bool operator==(const RoadPoint&) const = default;
};

inline constexpr int kNoRoadComponent = -1;

// One road vehicle. Like a TrainCar it lives on a tile with a sub-tile `phase`;
// here it follows a precomputed ordered tile route and ping-pongs along it. This
// is the "different graphics, same movement" seam: the route + phase is all a
// car needs; the renderer interpolates between the two tiles it is between.
struct RoadVehicle {
    static constexpr int kStepsPerTile = 8;  // sub-tile frames, matching trains

    std::vector<RoadPoint> route;  // derived tile path (rebuilt from stopSites on load)
    int segment = 0;      // index of route[segment] -> route[segment+1]
    int phase = 0;        // 0..kStepsPerTile along the current segment
    bool forward = true;  // ping-pong direction along the route
    int direction = road_direction::PositiveX;  // derived from the current segment
    bool loop = false;    // true = closed loop (wrap), false = ping-pong

    // carType selects the shared train sprite row: 2 = bus, 4 = truck. Cargo has
    // one counter per service resource so a bus can carry passengers and mail
    // together, and a truck can carry all three freight groups together. Their
    // sum may not exceed capacity.
    int carType = 2;
    int capacity = 30;
    std::array<int, kResourceTypeCount> cargo{};
    int revenue = 0;

    // The ordered road-stop service-site slots this vehicle serves. This is the
    // saved definition of the route; `route` (the tile path) is expanded from it.
    std::vector<int> stopSites;

    bool active() const {
        return route.size() >= 2 && segment >= 0
               && static_cast<std::size_t>(segment) < route.size() - 1;
    }
    int total_cargo() const {
        int total = 0;
        for (int amount : cargo) total += amount;
        return total;
    }
    bool accepts_resource(int resource) const {
        if (carType == road_vehicle_type::Bus)
            return resource == service_resource::Passengers
                || resource == service_resource::Mail;
        if (carType == road_vehicle_type::Truck)
            return resource >= service_resource::IndustryGroup1
                && resource <= service_resource::IndustryGroup3;
        return false;
    }

    // Advance one sub-tile step; steps to the next tile at phase wrap. A ping-pong
    // vehicle reverses at either end; a loop vehicle wraps back to the start.
    void step();

    // The tile the car is leaving / heading toward this segment.
    RoadPoint from_tile() const;
    RoadPoint to_tile() const;

    // The tile whose map-draw slot the sprite is drawn at: the tile the car is
    // closest to. Drawing at that tile's slot (like trains) lets buildings
    // south of the car cover it instead of the car driving over them.
    RoadPoint depth_tile() const {
        return phase >= kStepsPerTile / 2 ? to_tile() : from_tile();
    }
};

// The road layer: one byte per cell (0 = none, 1 = road), exactly the shape of
// `trackMap`. Disconnected roads are just isolated 1s; a route is searched on
// demand, never stored as an explicit graph -- same as the rail system.
//
// `roadComponent` tags each road cell with a connected-component id
// (`kNoRoadComponent` means not a road). At map generation every town's
// streets form one isolated component, so
// a component id is effectively "which town's road network this cell belongs
// to"; two stops share a town iff they share a component. Building a road that
// joins two towns merges their components on the next `rebuild_from_tiles`, so
// connectivity is always live -- there is no stored graph to keep in sync.
struct RoadNetwork {
    Grid<uint8_t> roadMap;          // 0 = none, 1 = road
    Grid<int16_t> roadComponent;    // connected-component id per road cell;
                                    // kNoRoadComponent means none
    int width = 0;
    int height = 0;

    void resize(int map_width, int map_height) {
        width = map_width;
        height = map_height;
        roadMap.resize(map_width, map_height);
        roadComponent.resize(map_width, map_height);
    }
    void clear() {
        width = 0;
        height = 0;
        roadMap.clear();
        roadComponent.clear();
    }

    // Rebuild the road layer from the map tiles: a cell is road iff its tileMap
    // code is a road/street shape (42..48, the same codes map-gen lays for city
    // streets and the build tool writes). Then recompute connected components.
    // This makes the shipped `tileMap` the single source of truth for roads and
    // `roadMap`/`roadComponent` pure derived caches (rebuilt on load and after
    // each road edit), so nothing extra needs saving.
    void rebuild_from_tiles(const Terrain& terrain);

    // Connected-component id at (x, y), or kNoRoadComponent if the cell is not
    // road / off map.
    int component_at(int x, int y) const;
    // True when both cells are road and in the same connected component.
    bool same_component(RoadPoint first_point, RoadPoint second_point) const;

    // Breadth-first search from `start` to `target` over passable cells,
    // returning the shortest path (ordered start..target) or empty if `target`
    // is unreachable. This is the point-to-point route used both to auto-build a
    // road between two tiles and to expand a vehicle's stop list into a concrete
    // tile path. Uses this network's own `width`/`height`.
    std::vector<RoadPoint> plan_route(
        RoadPoint start, RoadPoint target,
        const std::function<bool(int, int)>& passable) const;

private:
    // Flood-fill road cells into components, filling roadComponent.
    void recompute_components();
};

#endif  // ROAD_H
