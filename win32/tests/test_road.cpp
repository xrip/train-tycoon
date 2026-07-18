// Road-transport unit tests: the point-to-point route search and the vehicle
// stepping, exercised without a full Game so they stay fast and deterministic.
#include "doctest/doctest.h"

#include "road.h"
#include "support/java_random.h"
#include "terrain.h"
#include "world.h"

#include <climits>
#include <vector>

namespace {

// Lay a horizontal run of road tiles (code 42) into a terrain's tileMap.
void lay_road_row(Terrain& terrain, int y, int x0, int x1) {
    for (int x = x0; x <= x1; ++x) terrain.tileMap[x][y] = 42;
}

}  // namespace

TEST_CASE("fast: road layer is derived from road tile codes 42..48") {
    Terrain terrain;
    terrain.resize(8, 8);
    // One road cell of each shape code, plus a non-road neighbour (grass 0).
    for (int code = 42; code <= 48; ++code) terrain.tileMap[code - 42][0] = (int8_t)code;
    terrain.tileMap[7][7] = 0;  // grass: not road

    RoadNetwork net;
    net.rebuild_from_tiles(terrain);

    for (int code = 42; code <= 48; ++code)
        CHECK(net.component_at(code - 42, 0) != kNoRoadComponent);
    CHECK(net.component_at(7, 7) == kNoRoadComponent);  // grass is not road
    CHECK(net.component_at(3, 3) == kNoRoadComponent);  // empty cell is not road
}

TEST_CASE("fast: city growth does not claim unowned player roads") {
    World world;
    world.resize(7, 7);
    world.terrain.tileMap[3][3] = road_tile::kJunctionShape44;
    world.trackMap[3][3] = 0;  // player road, not a city street
    JavaRandom random;
    random.set_seed(1234);

    for (int attempt = 0; attempt < 200; ++attempt)
        CHECK_FALSE(world.pick_valid_forest_tile(random));

    for (int x = 0; x < 7; ++x)
        for (int y = 0; y < 7; ++y)
            if (x != 3 || y != 3) CHECK(world.terrain.tileMap[x][y] == 0);
}

TEST_CASE("fast: separate town road networks get different components") {
    Terrain terrain;
    terrain.resize(10, 10);
    lay_road_row(terrain, 1, 0, 3);  // "town A" streets
    lay_road_row(terrain, 8, 6, 9);  // "town B" streets, disconnected

    RoadNetwork net;
    net.rebuild_from_tiles(terrain);

    // Cells within a town share a component; cells across towns do not.
    CHECK(net.same_component(RoadPoint{0, 1}, RoadPoint{3, 1}));
    CHECK(net.same_component(RoadPoint{6, 8}, RoadPoint{9, 8}));
    CHECK_FALSE(net.same_component(RoadPoint{0, 1}, RoadPoint{6, 8}));
}

TEST_CASE("fast: point-to-point route follows roads and merges on connect") {
    Terrain terrain;
    terrain.resize(10, 10);
    lay_road_row(terrain, 1, 0, 3);
    lay_road_row(terrain, 8, 6, 9);

    RoadNetwork net;
    net.rebuild_from_tiles(terrain);

    const auto on_road = [&net](int x, int y) {
        return net.component_at(x, y) != kNoRoadComponent;
    };

    // No road between the two towns yet, so no route exists over roads.
    CHECK(net.plan_route(RoadPoint{0, 1}, RoadPoint{9, 8}, on_road).empty());

    // A same-town route is found and stays on that town's road.
    std::vector<RoadPoint> local = net.plan_route(RoadPoint{0, 1}, RoadPoint{3, 1}, on_road);
    REQUIRE(local.size() == 4);
    for (const RoadPoint& point : local)
        CHECK(net.component_at(point.x, point.y) != kNoRoadComponent);

    // Connect the towns with a road corridor, rebuild: now one component + a route.
    for (int y = 1; y <= 8; ++y) terrain.tileMap[3][y] = 42;
    lay_road_row(terrain, 8, 3, 6);
    net.rebuild_from_tiles(terrain);
    CHECK(net.same_component(RoadPoint{0, 1}, RoadPoint{9, 8}));
    CHECK_FALSE(net.plan_route(RoadPoint{0, 1}, RoadPoint{9, 8}, on_road).empty());
}

TEST_CASE("fast: point-to-point route rejects off-map endpoints") {
    RoadNetwork net;
    net.resize(4, 4);
    const auto passable = [](int, int) { return true; };

    CHECK(net.plan_route(RoadPoint{-1, 0}, RoadPoint{1, 0}, passable).empty());
    CHECK(net.plan_route(RoadPoint{0, 0}, RoadPoint{4, 0}, passable).empty());
}

TEST_CASE("fast: road vehicle steps tile-to-tile and ping-pongs") {
    RoadVehicle car;
    car.route = {RoadPoint{0, 0}, RoadPoint{1, 0}, RoadPoint{2, 0}};
    REQUIRE(car.active());

    // One full tile takes kStepsPerTile steps; start on segment 0.
    CHECK(car.segment == 0);
    CHECK(car.from_tile().x == 0);
    CHECK(car.to_tile().x == 1);

    for (int i = 0; i < RoadVehicle::kStepsPerTile; ++i) car.step();
    CHECK(car.segment == 1);          // advanced to the next segment
    CHECK(car.from_tile().x == 1);
    CHECK(car.to_tile().x == 2);

    // Reaching the far end reverses direction (ping-pong), no out-of-range.
    for (int i = 0; i < RoadVehicle::kStepsPerTile; ++i) car.step();
    CHECK(car.forward == false);
    CHECK(car.segment == 1);
    CHECK(car.from_tile().x == 2);    // now heading back toward the start
    CHECK(car.to_tile().x == 1);
}

TEST_CASE("fast: road vehicle rejects an invalid saved segment") {
    RoadVehicle car;
    car.route = {RoadPoint{0, 0}, RoadPoint{1, 0}};
    car.segment = 1;

    CHECK_FALSE(car.active());
    car.step();
    CHECK(car.segment == 1);

    car.segment = INT_MAX;
    CHECK_FALSE(car.active());
}
