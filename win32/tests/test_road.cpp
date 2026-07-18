// Road-transport proof-of-concept unit tests: the route search and the vehicle
// stepping, exercised without a full Game so they stay fast and deterministic.
#include "doctest/doctest.h"

#include "road.h"

#include <vector>

TEST_CASE("fast: road route search finds the longest path on open ground") {
    RoadNetwork net;
    net.resize(8, 8);
    // Whole 8x8 grid is drivable.
    const auto passable = [](int, int) { return true; };

    std::vector<RoadPoint> route =
        net.plan_longest_route(8, 8, passable, RoadPoint{0, 0});

    REQUIRE(route.size() >= 2);
    CHECK(route.front().x == 0);
    CHECK(route.front().y == 0);
    // Farthest reachable corner from (0,0) on a 4-connected grid is (7,7),
    // Manhattan distance 14 -> 15 tiles along the path.
    CHECK(route.size() == 15);

    // Every hop is to a 4-connected neighbour, and stays in bounds.
    for (std::size_t i = 1; i < route.size(); ++i) {
        const int dx = route[i].x - route[i - 1].x;
        const int dy = route[i].y - route[i - 1].y;
        CHECK(std::abs(dx) + std::abs(dy) == 1);
        CHECK(route[i].x >= 0);
        CHECK(route[i].x < 8);
        CHECK(route[i].y >= 0);
        CHECK(route[i].y < 8);
    }
}

TEST_CASE("fast: road route search respects blocked (water) cells") {
    RoadNetwork net;
    net.resize(5, 5);
    // A wall down column x==2 splits the grid; only the left strip is reachable.
    const auto passable = [](int x, int) { return x != 2; };

    std::vector<RoadPoint> route =
        net.plan_longest_route(5, 5, passable, RoadPoint{0, 0});

    REQUIRE(route.size() >= 2);
    for (const RoadPoint& p : route) CHECK(p.x < 2);  // never crosses the wall
}

TEST_CASE("fast: road route search returns nothing from a blocked seed") {
    RoadNetwork net;
    net.resize(4, 4);
    const auto passable = [](int, int) { return false; };
    CHECK(net.plan_longest_route(4, 4, passable, RoadPoint{1, 1}).empty());
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
