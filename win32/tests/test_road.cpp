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
        CHECK_FALSE(world.grow_city_street(random));

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

TEST_CASE("fast: shared road shape rule follows the linked neighbours") {
    CHECK(road_shape_from_neighbors(true, true, false, false)
          == road_tile::kStraightShape43);
    CHECK(road_shape_from_neighbors(false, false, true, true)
          == road_tile::kStraightShape42);
    CHECK(road_shape_from_neighbors(true, false, false, false)
          == road_tile::kStraightShape43);  // stub along x
    CHECK(road_shape_from_neighbors(false, false, true, false)
          == road_tile::kStraightShape42);  // stub along y
    CHECK(road_shape_from_neighbors(false, false, false, false)
          == road_tile::kStraightShape42);
    CHECK(road_shape_from_neighbors(false, true, true, false)
          == road_tile::kCornerShape45);
    CHECK(road_shape_from_neighbors(false, true, false, true)
          == road_tile::kCornerShape46);
    CHECK(road_shape_from_neighbors(true, false, false, true)
          == road_tile::kCornerShape47);
    CHECK(road_shape_from_neighbors(true, false, true, false)
          == road_tile::kCornerShape48);
    CHECK(road_shape_from_neighbors(true, true, true, false)
          == road_tile::kJunctionShape44);
    CHECK(road_shape_from_neighbors(true, true, true, true)
          == road_tile::kJunctionShape44);
}

TEST_CASE("fast: city growth re-orients the streets it joins") {
    World world;
    world.resize(7, 7);
    const int8_t city_owned =
        static_cast<int8_t>(track_overlay::JUNCTION | 0);
    // Two horizontal streets linked by junctions. The only cells a new street
    // can grow into are the gaps in the middle row, and each gap touches the
    // straight above and below -- so a growth must turn those straights into
    // junctions.
    for (int x = 1; x <= 5; ++x) {
        world.terrain.tileMap[x][2] = road_tile::kStraightShape43;
        world.terrain.tileMap[x][4] = road_tile::kStraightShape43;
        world.trackMap[x][2] = city_owned;
        world.trackMap[x][4] = city_owned;
    }
    for (int x = 1; x <= 5; x += 2) {
        world.terrain.tileMap[x][3] = road_tile::kJunctionShape44;
        world.trackMap[x][3] = city_owned;
    }
    JavaRandom random;
    random.set_seed(1234);

    bool grew = false;
    for (int attempt = 0; attempt < 500 && !grew; ++attempt) {
        int8_t before[7][7];
        for (int x = 0; x < 7; ++x)
            for (int y = 0; y < 7; ++y)
                before[x][y] = world.terrain.tileMap[x][y];
        if (!world.grow_city_street(random)) continue;
        grew = true;

        const auto is_road = [&world](int x, int y) {
            if (x < 0 || y < 0 || x >= 7 || y >= 7) return false;
            const int code = world.terrain.tileMap[x][y];
            return code >= road_tile::kFirstShape
                   && code <= road_tile::kLastShape;
        };
        // Find the new street cell (was not a road before the growth).
        int new_x = -1;
        int new_y = -1;
        for (int x = 0; x < 7 && new_x < 0; ++x)
            for (int y = 0; y < 7 && new_x < 0; ++y) {
                const int old_code = before[x][y];
                const bool was_road = old_code >= road_tile::kFirstShape
                                      && old_code <= road_tile::kLastShape;
                if (is_road(x, y) && !was_road) { new_x = x; new_y = y; }
            }
        REQUIRE(new_x >= 0);

        // Every street the new cell touches now follows the shared shape
        // rule, and at least one of them actually changed shape.
        int reoriented = 0;
        const int neighbor_x[4] = {new_x - 1, new_x + 1, new_x, new_x};
        const int neighbor_y[4] = {new_y, new_y, new_y - 1, new_y + 1};
        for (int direction = 0; direction < 4; ++direction) {
            const int cell_x = neighbor_x[direction];
            const int cell_y = neighbor_y[direction];
            if (!is_road(cell_x, cell_y)) continue;
            CHECK(world.terrain.tileMap[cell_x][cell_y]
                  == road_shape_from_neighbors(
                         is_road(cell_x - 1, cell_y),
                         is_road(cell_x + 1, cell_y),
                         is_road(cell_x, cell_y - 1),
                         is_road(cell_x, cell_y + 1)));
            if (world.terrain.tileMap[cell_x][cell_y] != before[cell_x][cell_y])
                ++reoriented;
        }
        CHECK(reoriented > 0);
    }
    REQUIRE(grew);
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
