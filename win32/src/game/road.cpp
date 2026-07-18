// Road-transport proof of concept -- see road.h. Opt-in via the
// TRAIN_TYCOON_ROAD_POC env var (set in main.cpp); a no-op otherwise, so the
// shipped game and its fixed-seed golden behavior are untouched.
#include "game.h"
#include "road.h"
#include "support/numeric.h"

#include <algorithm>
#include <climits>

// ---- RoadVehicle -----------------------------------------------------------

RoadPoint RoadVehicle::from_tile() const {
    if (!active()) return RoadPoint{};
    return forward ? route[segment] : route[segment + 1];
}

RoadPoint RoadVehicle::to_tile() const {
    if (!active()) return RoadPoint{};
    return forward ? route[segment + 1] : route[segment];
}

void RoadVehicle::step() {
    if (!active()) return;
    const RoadPoint f = from_tile();
    const RoadPoint t = to_tile();
    const int ddx = t.x - f.x;
    const int ddy = t.y - f.y;
    if (ddx > 0) direction = 0;
    else if (ddx < 0) direction = 1;
    else if (ddy > 0) direction = 2;
    else if (ddy < 0) direction = 3;

    if (++phase < kStepsPerTile) return;
    phase = 0;
    const int last_segment = static_cast<int>(route.size()) - 2;
    if (forward) {
        if (++segment > last_segment) { segment = last_segment; forward = false; }
    } else {
        if (--segment < 0) { segment = 0; forward = true; }
    }
}

// ---- RoadNetwork -----------------------------------------------------------

std::vector<RoadPoint> RoadNetwork::plan_longest_route(
    int width, int height,
    const std::function<bool(int, int)>& passable, RoadPoint start) const {
    std::vector<RoadPoint> path;
    if (width <= 0 || height <= 0) return path;
    if (!passable(start.x, start.y)) return path;

    const int cell_count = width * height;
    std::vector<int> parent(static_cast<std::size_t>(cell_count), -1);
    std::vector<uint8_t> seen(static_cast<std::size_t>(cell_count), 0);
    std::vector<int> queue;
    queue.reserve(static_cast<std::size_t>(cell_count));

    const auto index_of = [width](int x, int y) { return y * width + x; };
    const int start_index = index_of(start.x, start.y);
    seen[static_cast<std::size_t>(start_index)] = 1;
    parent[static_cast<std::size_t>(start_index)] = start_index;
    queue.push_back(start_index);

    // BFS explores in non-decreasing distance order, so the last cell dequeued
    // is a farthest reachable cell -- the far end of our demo road.
    int farthest = start_index;
    const int step_x[4] = {1, -1, 0, 0};
    const int step_y[4] = {0, 0, 1, -1};
    for (std::size_t head = 0; head < queue.size(); ++head) {
        const int current = queue[head];
        farthest = current;
        const int cx = current % width;
        const int cy = current / width;
        for (int d = 0; d < 4; ++d) {
            const int nx = cx + step_x[d];
            const int ny = cy + step_y[d];
            if (nx < 0 || ny < 0 || nx >= width || ny >= height) continue;
            const int ni = index_of(nx, ny);
            if (seen[static_cast<std::size_t>(ni)]) continue;
            if (!passable(nx, ny)) continue;
            seen[static_cast<std::size_t>(ni)] = 1;
            parent[static_cast<std::size_t>(ni)] = current;
            queue.push_back(ni);
        }
    }

    for (int current = farthest;; current = parent[static_cast<std::size_t>(current)]) {
        path.push_back(RoadPoint{current % width, current / width});
        if (current == start_index) break;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

void RoadNetwork::mark(const std::vector<RoadPoint>& path) {
    if (roadMap.empty()) return;
    for (const RoadPoint& p : path) roadMap[p.x][p.y] = 1;
}

// ---- Game hooks (all gated on roadPocEnabled by the callers) ---------------

void Game::road_poc_init() {
    Game* game = this;
    game->roadPocInitDone = true;
    const int width = game->world.terrain.mapWidth;
    const int height = game->world.terrain.mapHeight;
    if (width <= 0 || height <= 0) return;
    game->roadNet.resize(width, height);

    // Drivable ground for the demo: flat grass (0), scenery (29..33), and city
    // zone tiles (34..40); never water (elevation -1). This is the passability
    // seam a real road layer would own.
    const auto passable = [game, width, height](int x, int y) -> bool {
        if (x < 0 || y < 0 || x >= width || y >= height) return false;
        if (game->world.terrain.tileElevation[x][y] == -1) return false;
        const int t = unsigned_byte(game->world.terrain.tileMap[x][y]);
        return t == 0 || (t >= 29 && t <= 40);
    };

    // Seed at the drivable tile nearest the map centre, then lay the longest
    // road reachable from it and put one car on it.
    RoadPoint seed{-1, -1};
    int best = INT_MAX;
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            if (!passable(x, y)) continue;
            const int dx = x - width / 2;
            const int dy = y - height / 2;
            const int d = dx * dx + dy * dy;
            if (d < best) { best = d; seed = RoadPoint{x, y}; }
        }
    }
    if (seed.x < 0) return;  // no drivable ground on this map -- nothing to show

    std::vector<RoadPoint> route =
        game->roadNet.plan_longest_route(width, height, passable, seed);
    if (route.size() < 2) return;
    game->roadNet.mark(route);
    RoadVehicle car;
    car.route = std::move(route);
    game->roadCars.push_back(std::move(car));
}

void Game::road_poc_tick() {
    Game* game = this;
    if (!game->roadPocInitDone) game->road_poc_init();
    for (RoadVehicle& car : game->roadCars) car.step();
}

void Game::road_poc_render(Surface& surface) {
    Game* game = this;
    Surface* g = &surface;
    if (game->roadNet.roadMap.empty()) return;
    const int width = game->world.terrain.mapWidth;
    const int height = game->world.terrain.mapHeight;

    const auto tile_screen = [game](int tx, int ty, int& sx, int& sy) {
        sx = ty * 16 + tx * 16 + 16 - game->renderer.camera.viewportCenterX;
        sy = game->world.terrain.mapHalfWidth - tx * 8 + ty * 8 + 16
             - game->renderer.camera.viewportCenterY
             - 4 * game->world.terrain.tileElevation[tx][ty] + 15;
    };

    // Road tiles: reuse the city road sprites (codes 42..48). Each tile's road
    // shape is picked from its neighbours by the same rule map_gen uses to lay
    // city streets (place_terrain_patch): 42/43 are the two straights, 44 the
    // crossing, 45..48 the four corners. This auto-orients straights, bends and
    // junctions without storing direction per tile.
    const auto road_present = [&game, width, height](int x, int y) -> bool {
        if (x < 0 || y < 0 || x >= width || y >= height) return false;
        return game->roadNet.roadMap[x][y] != 0;
    };
    const auto road_code = [&road_present](int x, int y) -> int {
        const bool xm = road_present(x - 1, y), xp = road_present(x + 1, y);
        const bool ym = road_present(x, y - 1), yp = road_present(x, y + 1);
        const int neighbours = (xm ? 1 : 0) + (xp ? 1 : 0)
                             + (ym ? 1 : 0) + (yp ? 1 : 0);
        if (neighbours > 2) return 44;          // junction / crossing
        if (xp && ym) return 45;                // corner: +x, -y
        if (xp && yp) return 46;                // corner: +x, +y
        if (xm && yp) return 47;                // corner: -x, +y
        if (xm && ym) return 48;                // corner: -x, -y
        if ((xm || xp) && !ym && !yp) return 43; // straight along x
        return 42;                               // straight along y / stub
    };
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            if (game->roadNet.roadMap[x][y] == 0) continue;
            int sx = 0, sy = 0;
            tile_screen(x, y, sx, sy);
            const int idx = game->assets.tileSpriteIndex[road_code(x, y)];
            if (idx < 0) continue;
            midp_draw_image(g, game->assets.tile_sprites[idx].get(), sx, sy, 33);
        }
    }

    // Cars (amber) -- position interpolated between the two tiles they span,
    // the same phase blend trains use. Swap this box for the loco sprite later.
    for (const RoadVehicle& car : game->roadCars) {
        if (!car.active()) continue;
        const RoadPoint f = car.from_tile();
        const RoadPoint t = car.to_tile();
        int fx = 0, fy = 0, tx = 0, ty = 0;
        tile_screen(f.x, f.y, fx, fy);
        tile_screen(t.x, t.y, tx, ty);
        const int cx = fx + (tx - fx) * car.phase / RoadVehicle::kStepsPerTile;
        const int cy = fy + (ty - fy) * car.phase / RoadVehicle::kStepsPerTile;
        midp_set_color(g, 0xFFC000);
        midp_fill_rect(g, cx - 3, cy - 8, 7, 8);
        midp_set_color(g, 0x000000);
        midp_draw_rect(g, cx - 3, cy - 8, 7, 8);
    }
}
