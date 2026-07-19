// Road-transport subsystem -- see road.h. Roads are tileMap codes 42..48 (laid
// at map generation for city streets and by the build tool for player roads),
// so they persist and render through the normal map path; only routing and the
// bus/truck fleet live here.
#include "game.h"
#include "road.h"
#include "terrain.h"
#include "support/numeric.h"

#include <algorithm>
#include <array>
#include <climits>

namespace {

constexpr std::array<RoadPoint, 4> kOrthogonalOffsets = {{
    {1, 0}, {-1, 0}, {0, 1}, {0, -1}
}};
constexpr std::array<RoadPoint, 5> kCellAndOrthogonalOffsets = {{
    {0, 0}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}
}};

namespace road_sprite_slot {
constexpr int PositiveX = 3;
constexpr int NegativeX = 7;
constexpr int PositiveY = 1;
constexpr int NegativeY = 5;
}  // namespace road_sprite_slot

constexpr std::array<int, 4> kTrainSpriteSlotByRoadDirection = {{
    road_sprite_slot::PositiveY,
    road_sprite_slot::NegativeY,
    road_sprite_slot::PositiveX,
    road_sprite_slot::NegativeX,
}};

// The train images are anchored around the tile centre, not at its draw origin.
// These are the same fixed offsets used by Renderer::draw_train for straight
// movement in each map direction.
constexpr std::array<RoadPoint, 4> kSpriteAnchorByRoadDirection = {{
    {-8, -3},
    {8, -11},
    {-8, -11},
    {8, -3},
}};

int road_direction_between(RoadPoint from_point, RoadPoint to_point) {
    const int delta_x = to_point.x - from_point.x;
    const int delta_y = to_point.y - from_point.y;
    if (delta_x == 1 && delta_y == 0) return road_direction::PositiveX;
    if (delta_x == -1 && delta_y == 0) return road_direction::NegativeX;
    if (delta_x == 0 && delta_y == 1) return road_direction::PositiveY;
    if (delta_x == 0 && delta_y == -1) return road_direction::NegativeY;
    return -1;
}

}  // namespace

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
    if (phase < 0 || phase >= kStepsPerTile) phase = 0;
    const RoadPoint from_point = from_tile();
    const RoadPoint to_point = to_tile();
    const int next_direction = road_direction_between(from_point, to_point);
    if (next_direction < 0) return;
    direction = next_direction;

    if (++phase < kStepsPerTile) return;
    phase = 0;
    const int last_segment = static_cast<int>(route.size()) - 2;
    if (loop) {
        // Closed loop: the route's last leg returns to the first tile, so simply
        // wrap back to segment 0 and keep circling.
        if (++segment > last_segment) segment = 0;
        return;
    }
    if (forward) {
        if (++segment > last_segment) { segment = last_segment; forward = false; }
    } else {
        if (--segment < 0) { segment = 0; forward = true; }
    }
}

// ---- RoadNetwork -----------------------------------------------------------

void RoadNetwork::rebuild_from_tiles(const Terrain& terrain) {
    resize(terrain.mapWidth, terrain.mapHeight);
    if (width <= 0 || height <= 0) return;
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            const int tile_code = unsigned_byte(terrain.tileMap[x][y]);
            roadMap[x][y] = (tile_code >= road_tile::kFirstShape
                              && tile_code <= road_tile::kLastShape)
                                 ? 1
                                 : 0;
            roadComponent[x][y] = kNoRoadComponent;
        }
    }
    recompute_components();
}

void RoadNetwork::recompute_components() {
    if (roadMap.empty()) return;
    int next_id = 0;
    std::vector<RoadPoint> stack;
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            if (roadMap[x][y] == 0
                || roadComponent[x][y] != kNoRoadComponent)
                continue;
            const int id = next_id++;
            stack.clear();
            stack.push_back(RoadPoint{x, y});
            roadComponent[x][y] = static_cast<int16_t>(id);
            while (!stack.empty()) {
                const RoadPoint p = stack.back();
                stack.pop_back();
                for (const RoadPoint offset : kOrthogonalOffsets) {
                    const int neighbor_x = p.x + offset.x;
                    const int neighbor_y = p.y + offset.y;
                    if (neighbor_x < 0 || neighbor_y < 0
                        || neighbor_x >= width || neighbor_y >= height) continue;
                    if (roadMap[neighbor_x][neighbor_y] == 0
                        || roadComponent[neighbor_x][neighbor_y] != kNoRoadComponent)
                        continue;
                    roadComponent[neighbor_x][neighbor_y] = static_cast<int16_t>(id);
                    stack.push_back(RoadPoint{neighbor_x, neighbor_y});
                }
            }
        }
    }
}

int RoadNetwork::component_at(int x, int y) const {
    if (roadComponent.empty()) return kNoRoadComponent;
    if (x < 0 || y < 0 || x >= width || y >= height)
        return kNoRoadComponent;
    if (roadMap[x][y] == 0) return kNoRoadComponent;
    return roadComponent[x][y];
}

bool RoadNetwork::same_component(RoadPoint first_point,
                                  RoadPoint second_point) const {
    const int first_component =
        component_at(first_point.x, first_point.y);
    return first_component != kNoRoadComponent
        && first_component == component_at(second_point.x, second_point.y);
}

std::vector<RoadPoint> RoadNetwork::plan_route(
    RoadPoint start, RoadPoint target,
    const std::function<bool(int, int)>& passable) const {
    std::vector<RoadPoint> path;
    if (width <= 0 || height <= 0) return path;
    if (start.x < 0 || start.y < 0 || start.x >= width || start.y >= height
        || target.x < 0 || target.y < 0
        || target.x >= width || target.y >= height)
        return path;
    if (!passable(start.x, start.y) || !passable(target.x, target.y)) return path;

    const int cell_count = width * height;
    std::vector<int> parent(static_cast<std::size_t>(cell_count), -1);
    std::vector<uint8_t> seen(static_cast<std::size_t>(cell_count), 0);
    std::vector<int> queue;
    queue.reserve(static_cast<std::size_t>(cell_count));

    const auto index_of = [this](int x, int y) { return y * width + x; };
    const int start_index = index_of(start.x, start.y);
    const int target_index = index_of(target.x, target.y);
    seen[static_cast<std::size_t>(start_index)] = 1;
    parent[static_cast<std::size_t>(start_index)] = start_index;
    queue.push_back(start_index);

    bool reached = start_index == target_index;
    for (std::size_t queue_index = 0;
         queue_index < queue.size() && !reached; ++queue_index) {
        const int current_cell_index = queue[queue_index];
        const int current_x = current_cell_index % width;
        const int current_y = current_cell_index / width;
        for (const RoadPoint offset : kOrthogonalOffsets) {
            const int neighbor_x = current_x + offset.x;
            const int neighbor_y = current_y + offset.y;
            if (neighbor_x < 0 || neighbor_y < 0
                || neighbor_x >= width || neighbor_y >= height) continue;
            const int neighbor_index = index_of(neighbor_x, neighbor_y);
            if (seen[static_cast<std::size_t>(neighbor_index)]) continue;
            if (!passable(neighbor_x, neighbor_y)) continue;
            seen[static_cast<std::size_t>(neighbor_index)] = 1;
            parent[static_cast<std::size_t>(neighbor_index)] = current_cell_index;
            if (neighbor_index == target_index) { reached = true; break; }
            queue.push_back(neighbor_index);
        }
    }

    if (!reached) return path;
    for (int current = target_index;; current = parent[static_cast<std::size_t>(current)]) {
        path.push_back(RoadPoint{current % width, current / width});
        if (current == start_index) break;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

// ---- Road building ---------------------------------------------------------

bool Game::is_road_tile(int32_t x, int32_t y) const {
    if (!world.in_bounds(x, y)) return false;
    const int tile_code = unsigned_byte(world.terrain.tileMap[x][y]);
    return tile_code >= road_tile::kFirstShape
           && tile_code <= road_tile::kLastShape;
}

// A new road tile may be laid on land: flat grass (0), a generated slope
// (3..28), or clearable scenery (29..33). The tile keeps its elevation, so road
// vehicles interpolate up or down to the next cell.
bool Game::road_buildable(int32_t x, int32_t y) const {
    if (!world.in_bounds(x, y)) return false;
    if (world.terrain.tileElevation[x][y] < 0) return false;  // water
    if (world.trackMap[x][y] != 0) return false;              // track / station
    if (fleet.find_service_site_at(x, y) != kNoFleetSlot) return false;
    // A stop's access is derived from its one adjacent road. Building under it
    // or on another side would move that access and leave existing routes stale.
    for (const RoadPoint offset : kOrthogonalOffsets)
        if (road_stop_at(x + offset.x, y + offset.y) != kNoFleetSlot)
            return false;
    const int tile_code = unsigned_byte(world.terrain.tileMap[x][y]);
    return tile_code == 0
        || (tile_code >= tile::SLOPE_FIRST
            && tile_code <= tile::SLOPE_LAST)
        || (tile_code >= tile::SCENERY_FIRST
            && tile_code <= tile::SCENERY_LAST);
}

// Pick the oriented street shape (42..48) for a road cell from its 4 road
// neighbours -- the shared rule (road_shape_from_neighbors) that map-gen and
// city growth also use, written into tileMap so the normal renderer shows it.
int32_t Game::road_shape_code(int32_t x, int32_t y) const {
    return road_shape_from_neighbors(is_road_tile(x - 1, y),
                                     is_road_tile(x + 1, y),
                                     is_road_tile(x, y - 1),
                                     is_road_tile(x, y + 1));
}

// Re-orient the touched road cells and their road neighbours (so a newly-laid
// tile and the street it joins both become the right corner/junction), then
// refresh the derived road caches. Only local cells are touched, so existing
// city streets keep their generated orientation.
void Game::renormalize_road_cells(const std::vector<RoadPoint>& touched) {
    for (const RoadPoint& p : touched) {
        for (const RoadPoint offset : kCellAndOrthogonalOffsets) {
            const int cell_x = p.x + offset.x;
            const int cell_y = p.y + offset.y;
            if (is_road_tile(cell_x, cell_y))
                world.terrain.tileMap[cell_x][cell_y] =
                    static_cast<int8_t>(road_shape_code(cell_x, cell_y));
        }
    }
    roadNet.rebuild_from_tiles(world.terrain);
    renderer.lastViewportCenterX = -1;  // force the static map to redraw
}

bool Game::place_road_tile_at_cursor() {
    const int x = ui.tileCursorX;
    const int y = ui.tileCursorY;
    if (ui.blockBuildMenu || is_road_tile(x, y) || !road_buildable(x, y)) return false;
    if (!spend_money(kRoadTileCost)) return false;
    world.terrain.tileMap[x][y] = road_tile::kStraightShape42;  // provisional; renormalized below
    offerBoard.add_progress(offer_objective::Objective13, kRoadTileCost);
    renormalize_road_cells({RoadPoint{x, y}});
    return true;
}

// Route a would-be road over drivable ground and any existing road, so the
// path threads through a town's streets for free and only new tiles cost.
// Shared by auto_route_road (the build) and update_road_route_preview (the
// shown-before-paying path).
std::vector<RoadPoint> Game::plan_road_build_route(
    int32_t anchor_x, int32_t anchor_y, int32_t target_x,
    int32_t target_y) const {
    const auto passable = [this](int x, int y) {
        return is_road_tile(x, y) || road_buildable(x, y);
    };
    return roadNet.plan_route(RoadPoint{anchor_x, anchor_y},
                              RoadPoint{target_x, target_y}, passable);
}

int32_t Game::road_build_route_cost(const std::vector<RoadPoint>& path) const {
    int new_tiles = 0;
    for (const RoadPoint& p : path)
        if (!is_road_tile(p.x, p.y)) ++new_tiles;
    return new_tiles * kRoadTileCost;
}

bool Game::auto_route_road(int32_t anchor_x, int32_t anchor_y,
                           int32_t target_x, int32_t target_y) {
    const std::vector<RoadPoint> path =
        plan_road_build_route(anchor_x, anchor_y, target_x, target_y);
    if (path.size() < 2) return false;

    const int cost = road_build_route_cost(path);
    if (!spend_money(cost)) return false;
    for (const RoadPoint& p : path)
        if (!is_road_tile(p.x, p.y))
            world.terrain.tileMap[p.x][p.y] = road_tile::kStraightShape42;
    offerBoard.add_progress(offer_objective::Objective13, cost);
    renormalize_road_cells(path);
    return true;
}

// Recompute the cached auto-route preview (path + cost) when the anchor is
// armed and the tile cursor moved. draw_build_menu_cursor shows the path and
// the bottom bar shows the cost, so the second click never pays blind.
void Game::update_road_route_preview() {
    if (build.roadAutoAnchorX < 0) {
        if (!build.roadPreviewPath.empty()) {
            build.roadPreviewPath.clear();
            build.roadPreviewCost = 0;
            build.roadPreviewCursorX = -1;
            build.roadPreviewCursorY = -1;
            ui.menuDirty = true;  // drop the cost text from the bottom bar
        }
        return;
    }
    if (build.roadPreviewCursorX == ui.tileCursorX
        && build.roadPreviewCursorY == ui.tileCursorY)
        return;  // cache still fresh
    build.roadPreviewCursorX = ui.tileCursorX;
    build.roadPreviewCursorY = ui.tileCursorY;
    build.roadPreviewPath =
        plan_road_build_route(build.roadAutoAnchorX, build.roadAutoAnchorY,
                              ui.tileCursorX, ui.tileCursorY);
    if (build.roadPreviewPath.size() < 2) {
        build.roadPreviewPath.clear();
        build.roadPreviewCost = 0;
    } else {
        build.roadPreviewCost = road_build_route_cost(build.roadPreviewPath);
    }
    ui.menuDirty = true;  // bottom-bar cost text follows the cursor
}

// The adjacent road cell a stop at (x, y) binds to: the first of the four
// orthogonal neighbours that is a road tile, or {-1, -1} if the cell touches no
// road. A stop's town (road component), its route waypoint, and where a vehicle
// services it are all this access cell -- the shelter sits beside the road, the
// vehicle drives past on the road. Derived, never stored, so the save format is
// unchanged.
RoadPoint Game::road_access_cell(int32_t x, int32_t y) const {
    // Keep old on-road stops readable, but new stops are only placed beside a
    // street by road_stop_placeable.
    if (is_road_tile(x, y)) return RoadPoint{x, y};
    for (const RoadPoint offset : kOrthogonalOffsets) {
        const int neighbor_x = x + offset.x;
        const int neighbor_y = y + offset.y;
        if (is_road_tile(neighbor_x, neighbor_y))
            return RoadPoint{neighbor_x, neighbor_y};
    }
    return RoadPoint{-1, -1};
}

int32_t Game::road_stop_at(int32_t x, int32_t y) const {
    for (int service_slot = 0; service_slot < kServiceVehicleSlotCount;
         ++service_slot) {
        const ServiceSite& site = fleet.serviceSites[service_slot];
        if ((site.flags & service_site::USED_SLOT) == 0
            || (site.flags & service_site::TYPE_MASK) != kRoadStopSiteType)
            continue;
        if (unsigned_byte(site.x) == x && unsigned_byte(site.y) == y)
            return service_slot;
    }
    return kNoFleetSlot;
}

// A player-laid road cell may be demolished; a city street may not (its
// trackMap carries the city owner code, player roads leave it 0). Cells
// touching a road stop stay locked -- the same guard road_buildable uses --
// so a stop's access cell can never move out from under its routes.
bool Game::road_demolishable(int32_t x, int32_t y) const {
    if (!is_road_tile(x, y)) return false;
    if (world.trackMap[x][y] != 0) return false;  // city-owned street
    for (const RoadPoint offset : kOrthogonalOffsets)
        if (road_stop_at(x + offset.x, y + offset.y) != kNoFleetSlot)
            return false;
    return true;
}

bool Game::road_stop_placeable(int32_t x, int32_t y) const {
    if (!world.in_bounds(x, y) || !world.is_placeable_terrain(x, y))
        return false;
    if (fleet.find_service_site_at(x, y) != kNoFleetSlot) return false;
    const RoadPoint access = road_access_cell(x, y);
    if (access.x < 0) return false;

    // One access cell can service one shelter. Without this check, two stops on
    // opposite sides of one road cell make a zero-length route and one stop can
    // never be reached by the service loop.
    for (int service_slot = 0; service_slot < kServiceVehicleSlotCount;
         ++service_slot) {
        const ServiceSite& site = fleet.serviceSites[service_slot];
        if ((site.flags & service_site::USED_SLOT) == 0
            || (site.flags & service_site::TYPE_MASK) != kRoadStopSiteType)
            continue;
        const RoadPoint other_access = road_access_cell(
            unsigned_byte(site.x), unsigned_byte(site.y));
        if (other_access.x == access.x && other_access.y == access.y)
            return false;
    }
    return true;
}

// A road stop is a ServiceSite (type kRoadStopSiteType) placed on cleared ground
// that touches a road -- like a rail station beside its track, NOT on the road
// itself. It binds to the adjacent road cell (road_access_cell); a bus/truck
// services it while passing that cell. The existing per-tick site loop
// (tick_service_sites) fills its resourceStock from nearby city and industry
// exactly as it does for rail stations, so no cargo code is added.
bool Game::place_road_stop_at_cursor() {
    const int x = ui.tileCursorX;
    const int y = ui.tileCursorY;
    if (ui.blockBuildMenu || !road_stop_placeable(x, y)) return false;
    const int slot =
        fleet.allocate_service_site(kRoadStopSiteType, x, y, false, false);
    if (slot == kNoFleetSlot) return false;
    if (!spend_money(kRoadStopCost)) {
        fleet.serviceSites[slot].flags = 0;  // roll back the reservation
        return false;
    }
    // The shelter uses the station image code but no rail overlay. This gives it
    // normal map depth and makes all standard build checks treat the cell as
    // occupied without making it a rail station.
    world.terrain.tileMap[x][y] = tile::VERTICAL_STATION;
    offerBoard.add_progress(offer_objective::Objective13, kRoadStopCost);
    renderer.lastViewportCenterX = -1;
    return true;
}

void Game::remove_road_stop(int32_t stop_slot) {
    if (stop_slot < 0 || stop_slot >= kServiceVehicleSlotCount) return;
    ServiceSite& site = fleet.serviceSites[stop_slot];
    if ((site.flags & service_site::USED_SLOT) == 0
        || (site.flags & service_site::TYPE_MASK) != kRoadStopSiteType)
        return;
    site.flags = 0;

    for (RoadVehicle& vehicle : roadVehicles)
        vehicle.stopSites.erase(
            std::remove(vehicle.stopSites.begin(), vehicle.stopSites.end(),
                        stop_slot),
            vehicle.stopSites.end());
    rebuild_road_vehicle_routes();
}

// Re-expand every vehicle's tile route from its stop list after the road
// layer changed (a stop removed, a road tile demolished). Vehicles whose
// expanded route is unchanged keep their position; vehicles that can no
// longer run (a stop lost its road or the network split) are dropped.
void Game::rebuild_road_vehicle_routes() {
    for (RoadVehicle& vehicle : roadVehicles) {
        std::vector<RoadPoint> route =
            expand_road_route(vehicle.stopSites, vehicle.loop);
        if (route == vehicle.route) continue;  // unaffected by the edit
        vehicle.route = std::move(route);
        vehicle.segment = 0;
        vehicle.phase = 0;
        vehicle.forward = true;
    }
    roadVehicles.erase(
        std::remove_if(roadVehicles.begin(), roadVehicles.end(),
                       [](const RoadVehicle& vehicle) {
                           return !vehicle.active();
                       }),
        roadVehicles.end());
    renderer.lastViewportCenterX = -1;
}

// ---- Road vehicles (buses / trucks) ----------------------------------------

namespace {
// How a road vehicle's carType maps onto the shared cargo model: which
// resourceStock channel it carries, which unloadFlags bit a stop must have to
// accept it, and the per-unit fare. Mirrors the rail load/unload constants
// (train_tick.cpp) so buses/trucks earn like the equivalent wagons -- but kept
// standalone to avoid that code's delivered[]-index footguns.
struct RoadCargoSpec {
    int unload_flag;
    int fare_per_unit;
};

namespace road_cargo {
constexpr int kPassengerFare = 3;
constexpr int kMailFare = 8;
constexpr int kIndustryGroup1Fare = 10;
constexpr int kIndustryGroup2Fare = 12;
constexpr int kIndustryGroup3Fare = 13;
}  // namespace road_cargo

RoadCargoSpec road_cargo_spec(int resource) {
    switch (resource) {
        case service_resource::Passengers:
            return {service_resource::PASSENGER_FLAG,
                    road_cargo::kPassengerFare};
        case service_resource::Mail:
            return {service_resource::MAIL_FLAG, road_cargo::kMailFare};
        case service_resource::IndustryGroup1:
            // The unload flag follows the legacy rail cross-map.
            return {service_resource::INDUSTRY_GROUP3_FLAG,
                    road_cargo::kIndustryGroup1Fare};
        case service_resource::IndustryGroup2:
            return {service_resource::INDUSTRY_GROUP1_FLAG,
                    road_cargo::kIndustryGroup2Fare};
        case service_resource::IndustryGroup3:
            return {service_resource::INDUSTRY_GROUP2_FLAG,
                    road_cargo::kIndustryGroup3Fare};
        default:
            return {0, 0};
    }
}

}  // namespace

std::vector<int32_t> Game::road_stops_in_component(int32_t component,
                                                   int32_t first_stop) const {
    std::vector<int32_t> slots;
    if (component < 0) return slots;
    for (int service_slot = 0; service_slot < kServiceVehicleSlotCount;
         ++service_slot) {
        const ServiceSite& site = fleet.serviceSites[service_slot];
        if ((site.flags & service_site::USED_SLOT) == 0
            || (site.flags & service_site::TYPE_MASK) != kRoadStopSiteType)
            continue;
        // A stop belongs to the town of the road it sits beside.
        const RoadPoint access = road_access_cell(unsigned_byte(site.x),
                                                  unsigned_byte(site.y));
        if (roadNet.component_at(access.x, access.y) != component) continue;
        slots.push_back(service_slot);
    }
    const auto first = std::find(slots.begin(), slots.end(), first_stop);
    if (first == slots.end()) return {};
    std::iter_swap(slots.begin(), first);
    // Greedy nearest-neighbour order so the loop visits stops in a sensible ring.
    for (std::size_t slot_index = 1; slot_index < slots.size(); ++slot_index) {
        int best_slot_index = static_cast<int>(slot_index);
        int best_distance_squared = INT_MAX;
        const int previous_x =
            unsigned_byte(fleet.serviceSites[slots[slot_index - 1]].x);
        const int previous_y =
            unsigned_byte(fleet.serviceSites[slots[slot_index - 1]].y);
        for (std::size_t candidate_index = slot_index;
             candidate_index < slots.size(); ++candidate_index) {
            const int delta_x =
                unsigned_byte(fleet.serviceSites[slots[candidate_index]].x)
                - previous_x;
            const int delta_y =
                unsigned_byte(fleet.serviceSites[slots[candidate_index]].y)
                - previous_y;
            const int distance_squared = delta_x * delta_x + delta_y * delta_y;
            if (distance_squared < best_distance_squared) {
                best_distance_squared = distance_squared;
                best_slot_index = static_cast<int>(candidate_index);
            }
        }
        std::swap(slots[slot_index],
                  slots[static_cast<std::size_t>(best_slot_index)]);
    }
    return slots;
}

std::vector<RoadPoint> Game::expand_road_route(
    const std::vector<int32_t>& stop_sites, bool loop) const {
    std::vector<RoadPoint> route;
    if (stop_sites.size() < 2) return route;
    const auto is_road = [this](int x, int y) { return is_road_tile(x, y); };
    // A stop's waypoint is the road cell it sits beside, so legs are planned over
    // road tiles and the vehicle stays on the road, servicing the shelter as it
    // passes its access cell.
    const auto site_point = [this](int service_site_slot) {
        return road_access_cell(
            unsigned_byte(fleet.serviceSites[service_site_slot].x),
            unsigned_byte(fleet.serviceSites[service_site_slot].y));
    };
    const int stop_count = static_cast<int>(stop_sites.size());
    const int leg_count = loop ? stop_count : stop_count - 1;
    for (int leg_index = 0; leg_index < leg_count; ++leg_index) {
        const RoadPoint start = site_point(stop_sites[leg_index]);
        const RoadPoint destination =
            site_point(stop_sites[(leg_index + 1) % stop_count]);
        if (start.x < 0 || destination.x < 0) return {};  // a stop lost its road
        const std::vector<RoadPoint> leg =
            roadNet.plan_route(start, destination, is_road);
        if (leg.empty()) return {};  // a stop is unreachable over roads -> no route
        for (std::size_t leg_point_index = route.empty() ? 0 : 1;
             leg_point_index < leg.size(); ++leg_point_index)
            route.push_back(leg[leg_point_index]);
    }
    return route;
}

bool Game::buy_road_vehicle(int32_t car_type, int32_t first_stop) {
    if (car_type != road_vehicle_type::Bus
        && car_type != road_vehicle_type::Truck)
        return false;
    if (first_stop < 0 || first_stop >= kServiceVehicleSlotCount)
        return false;
    const ServiceSite& first_site = fleet.serviceSites[first_stop];
    if ((first_site.flags & service_site::USED_SLOT) == 0
        || (first_site.flags & service_site::TYPE_MASK) != kRoadStopSiteType)
        return false;
    const RoadPoint first_access = road_access_cell(
        unsigned_byte(first_site.x), unsigned_byte(first_site.y));
    const int component = roadNet.component_at(first_access.x, first_access.y);
    std::vector<int32_t> stops =
        road_stops_in_component(component, first_stop);
    if (stops.size() < 2) return false;  // a route needs at least two stops

    std::vector<RoadPoint> route = expand_road_route(stops, true);
    if (route.size() < 2) return false;

    const int price = car_type == road_vehicle_type::Bus ? kBusCost : kTruckCost;
    if (!spend_money(price)) return false;
    RoadVehicle vehicle;
    vehicle.carType = car_type;
    vehicle.capacity = car_type == road_vehicle_type::Bus
                           ? kBusCapacity
                           : kTruckCapacity;
    vehicle.loop = true;
    vehicle.route = std::move(route);
    vehicle.stopSites = stops;
    roadVehicles.push_back(std::move(vehicle));
    return true;
}

void Game::road_load_unload(RoadVehicle& vehicle,
                            int32_t service_site_slot) {
    ServiceSite& service_site_record = fleet.serviceSites[service_site_slot];
    for (int resource = 0; resource < kResourceTypeCount; ++resource) {
        const RoadCargoSpec spec = road_cargo_spec(resource);
        if (vehicle.cargo[resource] <= 0
            || (service_site_record.unloadFlags & spec.unload_flag) == 0)
            continue;
        // Deliver every carried resource this stop accepts.
        const int revenue = vehicle.cargo[resource] * spec.fare_per_unit;
        economy.money += revenue;
        economy.totalRevenue += revenue;
        record_money_change(revenue);
        vehicle.revenue += revenue;
        vehicle.cargo[resource] = 0;
    }

    // Take the allowed resources in turns. This gives every available cargo
    // type space while keeping one shared capacity for the whole vehicle.
    int remaining = vehicle.capacity - vehicle.total_cargo();
    bool took_cargo = true;
    while (remaining > 0 && took_cargo) {
        took_cargo = false;
        for (int resource = 0;
             resource < kResourceTypeCount && remaining > 0; ++resource) {
            if (!vehicle.accepts_resource(resource)
                || service_site_record.resourceStock[resource] <= 0)
                continue;
            ++vehicle.cargo[resource];
            --service_site_record.resourceStock[resource];
            --remaining;
            took_cargo = true;
        }
    }
    // Cheap upkeep each time the vehicle services a stop.
    economy.money -= kRoadRunCost;
    economy.fuelCostAccrued += kRoadRunCost;
    vehicle.revenue -= kRoadRunCost;
}

void Game::update_road_vehicles() {
    for (RoadVehicle& vehicle : roadVehicles) {
        if (!vehicle.active()) continue;
        const RoadPoint arrival = vehicle.to_tile();
        vehicle.step();
        if (vehicle.phase != 0) continue;  // only act on a tile boundary
        // The vehicle is on a road cell; service the stop (if any) whose shelter
        // sits beside this exact cell -- its access cell is a route waypoint.
        const RoadPoint at = arrival;
        for (int service_slot = 0;
             service_slot < kServiceVehicleSlotCount; ++service_slot) {
            const ServiceSite& site = fleet.serviceSites[service_slot];
            if ((site.flags & service_site::USED_SLOT) == 0
                || (site.flags & service_site::TYPE_MASK) != kRoadStopSiteType)
                continue;
            const RoadPoint access = road_access_cell(unsigned_byte(site.x),
                                                      unsigned_byte(site.y));
            if (access.x == at.x && access.y == at.y) {
                road_load_unload(vehicle, service_slot);
                break;
            }
        }
    }
}

// Screen position of a road tile's centre, matching how Renderer::draw_train
// anchors train sprites (viewport-relative).
void Game::road_tile_screen(int32_t tile_x, int32_t tile_y,
                            int32_t& screen_x, int32_t& screen_y) const {
    screen_x = tile_y * 16 + tile_x * 16 + 16
               - renderer.camera.viewportCenterX;
    screen_y = world.terrain.mapHalfWidth - tile_x * 8 + tile_y * 8 + 16
               - renderer.camera.viewportCenterY
               - 4 * world.terrain.tileElevation[tile_x][tile_y] + 15;
}

// Legacy shelter blit for old road-stop saves whose shelter cell was left
// empty (new stops use tile code 87 and are drawn in normal map depth).
// render_map calls this from the tile sweep at the shelter's own tile, so
// city buildings south of the shelter cover it like any other building.
void Game::draw_road_stop_shelter(Surface& surface, int32_t stop_x,
                                  int32_t stop_y) const {
    int stop_screen_x = 0;
    int stop_screen_y = 0;
    road_tile_screen(stop_x, stop_y, stop_screen_x, stop_screen_y);
    const int tile_sprite_index =
        unsigned_byte(assets.tileSpriteIndex[tile::VERTICAL_STATION]);
    if (tile_sprite_index < static_cast<int>(assets.tile_sprites.size())
        && assets.tile_sprites[tile_sprite_index])
        midp_draw_image(&surface,
                        assets.tile_sprites[tile_sprite_index].get(),
                        stop_screen_x, stop_screen_y, 33);
}

// One bus/truck: reuse the rail wagon sprites -- pick the carType's direction
// row and blit the facing sprite at the phase-interpolated position between
// the two tiles it is between. render_map calls this from the tile sweep at
// the vehicle's depth_tile(), so buildings south of the vehicle cover it
// instead of the vehicle driving over them (the same depth rule trains get).
void Game::draw_road_vehicle(Surface& surface,
                             const RoadVehicle& vehicle) const {
    if (!vehicle.active()) return;
    const RoadPoint from_point = vehicle.from_tile();
    const RoadPoint to_point = vehicle.to_tile();
    if (!world.in_bounds(from_point.x, from_point.y)
        || !world.in_bounds(to_point.x, to_point.y)
        || vehicle.phase < 0
        || vehicle.phase >= RoadVehicle::kStepsPerTile)
        return;
    const int road_direction =
        road_direction_between(from_point, to_point);
    if (road_direction < 0
        || vehicle.carType < 0
        || vehicle.carType >= kTrainSpriteTypeCount)
        return;
    int from_screen_x = 0;
    int from_screen_y = 0;
    int to_screen_x = 0;
    int to_screen_y = 0;
    road_tile_screen(from_point.x, from_point.y, from_screen_x,
                     from_screen_y);
    road_tile_screen(to_point.x, to_point.y, to_screen_x, to_screen_y);
    const int screen_x = from_screen_x
                         + (to_screen_x - from_screen_x) * vehicle.phase
                               / RoadVehicle::kStepsPerTile;
    const int screen_y = from_screen_y
                         + (to_screen_y - from_screen_y) * vehicle.phase
                               / RoadVehicle::kStepsPerTile;
    // trainSpriteDirectionTable has 8 sprite slots per car; a train picks
    // slots 1/5/3/7 for travel in the +x/-x/+y/-y map directions
    // (see Renderer::draw_train). Reuse the same slots so a bus/truck faces
    // the way it moves instead of showing an unrelated (vertical) sprite.
    const int sprite_code = assets.trainSpriteDirectionTable
                                [vehicle.carType]
                                [kTrainSpriteSlotByRoadDirection[road_direction]];
    if (sprite_code < 0 || sprite_code >= kTileSpriteCodeCount) return;
    const int sprite_index =
        unsigned_byte(assets.tileSpriteIndex[sprite_code]);
    if (sprite_index >= static_cast<int>(assets.tile_sprites.size())
        || !assets.tile_sprites[sprite_index])
        return;
    const RoadPoint anchor = kSpriteAnchorByRoadDirection[road_direction];
    midp_draw_image(&surface, assets.tile_sprites[sprite_index].get(),
                    screen_x + anchor.x, screen_y + anchor.y, 33);
}
