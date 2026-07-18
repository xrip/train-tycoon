// Direct game-map build and selection actions.
#include "game.h"
#include "support/numeric.h"
#include <array>
#include <vector>

namespace {

struct ConstructionTileRestoreRule {
    int constructionTileCode;
    int restoredTileCode;
};

namespace track_piece {
// These rules keep the source tile codes in one place. The names describe the
// placement operation; the asset-level names of the individual sprites remain
// intentionally open (see docs/refactor/DECISIONS.md).
constexpr TrackPlacementSpec kVertical = {
    .firstTileCode = track_build_tile::CornerVariantD,
    .secondTileCode = track_build_tile::CornerVariantC,
    .firstMatchTileCode = -1,
    .secondMatchTileCode = -1,
    .firstResultTileCode = -1,
    .secondResultTileCode = -1,
    .joinsExistingTrack = false,
    .joinedTrackTileCode = 0,
};
constexpr TrackPlacementSpec kHorizontal = {
    .firstTileCode = track_build_tile::CornerVariantA,
    .secondTileCode = track_build_tile::CornerVariantB,
    .firstMatchTileCode = -1,
    .secondMatchTileCode = -1,
    .firstResultTileCode = -1,
    .secondResultTileCode = -1,
    .joinsExistingTrack = false,
    .joinedTrackTileCode = 0,
};
constexpr TrackPlacementSpec kDiagonalA = {
    .firstTileCode = track_build_tile::AlongY,
    .secondTileCode = 0,
    .firstMatchTileCode = track_build_tile::OpenGroundVariantA,
    .secondMatchTileCode = track_build_tile::OpenGroundVariantC,
    .firstResultTileCode = track_turn_tile::VariantA,
    .secondResultTileCode = track_turn_tile::VariantC,
    .joinsExistingTrack = true,
    .joinedTrackTileCode = track_build_tile::AlongX,
};
constexpr TrackPlacementSpec kDiagonalB = {
    .firstTileCode = track_build_tile::AlongX,
    .secondTileCode = 0,
    .firstMatchTileCode = track_build_tile::OpenGroundVariantB,
    .secondMatchTileCode = track_build_tile::OpenGroundVariantD,
    .firstResultTileCode = track_turn_tile::VariantB,
    .secondResultTileCode = track_turn_tile::VariantD,
    .joinsExistingTrack = true,
    .joinedTrackTileCode = track_build_tile::AlongY,
};
}  // namespace track_piece

// Construction overlays are restored to their original terrain tile when a
// build is cancelled. The source data does not prove visual names for these
// tile ids, so this table records only the byte-to-byte restoration.
constexpr std::array<ConstructionTileRestoreRule, 8> kConstructionTileRestoreRules = {{
    {.constructionTileCode = track_turn_tile::VariantB,
     .restoredTileCode = track_build_tile::OpenGroundVariantB},
    {.constructionTileCode = track_turn_tile::VariantD,
     .restoredTileCode = track_build_tile::OpenGroundVariantD},
    {.constructionTileCode = construction_tile::HorizontalC,
     .restoredTileCode = tile::CURVE_RIGHT},
    {.constructionTileCode = construction_tile::HorizontalB,
     .restoredTileCode = tile::CURVE_LEFT},
    {.constructionTileCode = track_turn_tile::VariantA,
     .restoredTileCode = track_build_tile::OpenGroundVariantA},
    {.constructionTileCode = track_turn_tile::VariantC,
     .restoredTileCode = track_build_tile::OpenGroundVariantC},
    {.constructionTileCode = construction_tile::VerticalB,
     .restoredTileCode = tile::CURVE_UP},
    {.constructionTileCode = construction_tile::VerticalC,
     .restoredTileCode = tile::CURVE_DOWN},
}};

int restored_construction_tile(int construction_tile) {
    for (const ConstructionTileRestoreRule& rule : kConstructionTileRestoreRules)
        if (rule.constructionTileCode == construction_tile)
            return rule.restoredTileCode;
    return 0;
}

}  // namespace

void Game::enter_out_of_money_state() {
    Game *game = this;
    game->ready = false;
    game->ui.transitionCode = menu_screen::GenericOverlayG;
    game->set_state(GameState::MenuOverlay);
}

void OfferBoard::add_progress(int32_t objective_type, int32_t amount) {
    for (int row = 0; row < kOfferActiveRowCount; ++row)
        if (offers[row].objectiveType == objective_type)
            offers[row].progress += amount;
}

bool Fleet::has_car_at(int32_t car, int32_t x, int32_t y) const {
    if (car == train_chain::kEnd) return false;
    if (unsigned_byte(trainCars[car].x) == x
        && unsigned_byte(trainCars[car].y) == y)
        return true;
    return has_car_at(trainCars[car].nextCar, x, y);
}

int32_t Fleet::find_train_head_at(int32_t x, int32_t y) const {
    for (int train_slot = 0; train_slot < kTrainSlotCount; ++train_slot) {
        if (train_flag::isUsed(trainCars[train_slot].flags)
            && train_flag::isHead(trainCars[train_slot].flags)
            && has_car_at(train_slot, x, y))
            return train_slot;
    }
    return kNoFleetSlot;
}

int32_t Fleet::allocate_service_site(int32_t type, int32_t x, int32_t y,
                                     bool horizontal, bool extended) {
    int service_site_slot = kNoFleetSlot;
    for (int candidate_slot = 0; candidate_slot < kServiceVehicleSlotCount; ++candidate_slot) {
        if ((serviceSites[candidate_slot].flags & service_site::USED_SLOT) == 0) {
            service_site_slot = candidate_slot;
            break;
        }
    }
    if (service_site_slot == kNoFleetSlot) return service_site_slot;

    serviceSites[service_site_slot].flags = (int8_t)(service_site::USED_SLOT
                                                    | (horizontal ? service_site::HORIZONTAL : 0)
                                                    | (extended ? service_site::EXTENDED : 0) | type);
    serviceSites[service_site_slot].x = (int8_t)x;
    serviceSites[service_site_slot].y = (int8_t)y;
    serviceSites[service_site_slot].unloadFlags = 0;
    serviceSites[service_site_slot].loadFlags = 0;
    for (int resource_index = 0; resource_index < kResourceTypeCount; ++resource_index)
        serviceSites[service_site_slot].resourceStock[resource_index] = 0;
    return service_site_slot;
}

void BuildController::compute_train_cost(const Economy& economy,
                                         const Scenario& scenario) {
    unitCost = economy.train_cost_for_era(wagonType);
    if (wagonType == wagonDiscountType)
        unitCost = unitCost * scenario.difficultyPct / percentage::kBase;

    for (int option = 0; option < kWagonOptionCount; ++option) {
        int option_cost;
        switch (wagonOptionSlots[option]) {
            case wagon::PassengerMail:
                option_cost = economy.wagonOptionCostPassengerMail;
                break;
            case wagon::IndustryGroup1:
                option_cost = economy.wagonOptionCostIndustryGroup1;
                break;
            case wagon::IndustryGroup2:
                option_cost = economy.wagonOptionCostIndustryGroup2;
                break;
            case wagon::IndustryGroup3:
                option_cost = economy.wagonOptionCostIndustryGroup3;
                break;
            default: continue;
        }
        if (wagonOptionSlots[option] == matchingWagonOptions[option])
            option_cost = option_cost * scenario.difficultyPct / percentage::kBase;
        unitCost += option_cost;
    }
}

void World::recount_city_zone_tiles() {
    for (City& city : cities) city.zoneTileCount = 0;
    const int seen_count = kZoneCount * terrain.mapWidth * terrain.mapHeight;
    std::vector<uint8_t> seen_city_tiles(
        static_cast<std::size_t>(seen_count));
    for (int station_x = 0; station_x < terrain.mapWidth; ++station_x) {
        for (int station_y = 0; station_y < terrain.mapHeight; ++station_y) {
            const int track_code = trackMap[station_x][station_y];
            if ((track_code & track_overlay::MASK) != track_overlay::STATION)
                continue;
            const int city_id = track_code & track::OWNER_MASK;
            int min_map_x = station_x - 3;
            int min_map_y = station_y - 3;
            int max_map_x = station_x + 3;
            int max_map_y = station_y + 3;
            if (min_map_x < 0) min_map_x = 0;
            if (min_map_y < 0) min_map_y = 0;
            if (max_map_x >= terrain.mapWidth) max_map_x = terrain.mapWidth - 1;
            if (max_map_y >= terrain.mapHeight) max_map_y = terrain.mapHeight - 1;
            for (int map_x = min_map_x; map_x <= max_map_x; ++map_x) {
                for (int map_y = min_map_y; map_y <= max_map_y; ++map_y) {
                    if (terrain.tileMap[map_x][map_y] != tile::VERTICAL_STATION
                        && terrain.tileMap[map_x][map_y] != tile::HORIZONTAL_STATION)
                        continue;
                    const int city_tile_index =
                        (city_id * terrain.mapWidth + map_x) * terrain.mapHeight
                        + map_y;
                    if (seen_city_tiles[city_tile_index]) continue;
                    seen_city_tiles[city_tile_index] = 1;
                    cities[city_id].zoneTileCount =
                        (int8_t)(cities[city_id].zoneTileCount + 1);
                }
            }
        }
    }
}

void Game::apply_background_cost() {
    Game *game = this;
    if (game->build.demolitionAffordable) {
        game->economy.subsidyCost += game->build.backgroundCost;
        game->economy.money -= game->build.backgroundCost;
        game->record_money_change(-game->build.backgroundCost);
    } else {
        game->enter_out_of_money_state();
    }
}

void Game::demolish_at_cursor() {
    Game *game = this;
    int cursor_x = game->ui.tileCursorX;
    int cursor_y = game->ui.tileCursorY;
    if (game->ui.blockBuildMenu
        || !game->world.is_city_or_station_tile(game->fleet, cursor_x,
                                                cursor_y))
        return;

    game->build.backgroundCost = 0;
    const int road_stop_slot = game->road_stop_at(cursor_x, cursor_y);
    if (road_stop_slot != kNoFleetSlot) {
        game->build.queue_tile_change(game->world.terrain, game->economy,
                                      cursor_x, cursor_y, true, false);
        game->world.advance_construction_timers(cursor_x, cursor_y, -6);
        game->world.trackMap[cursor_x][cursor_y] = 0;
        game->remove_road_stop(road_stop_slot);
        game->apply_background_cost();
        return;
    }
    int tile_code = game->world.terrain.tileMap[cursor_x][cursor_y];
    if (tile_code >= construction_tile::kFirst
        && tile_code <= construction_tile::kLast) {
        game->world.advance_construction_timers(cursor_x, cursor_y, -3);
        tile_code = game->world.terrain.tileMap[cursor_x][cursor_y];
        if (tile_code == construction_tile::HorizontalA
            || tile_code == construction_tile::HorizontalB
            || tile_code == construction_tile::HorizontalC) {
            int scan_x;
            for (scan_x = cursor_x;
                 scan_x < game->world.terrain.mapWidth
                 && (game->world.terrain.tileMap[scan_x][cursor_y] == construction_tile::HorizontalA
                     || game->world.terrain.tileMap[scan_x][cursor_y] == construction_tile::HorizontalB
                     || game->world.terrain.tileMap[scan_x][cursor_y] == construction_tile::HorizontalC);
                 ++scan_x)
                game->build.queue_tile_change(game->world.terrain, game->economy, scan_x, cursor_y, true, false);
            for (scan_x = cursor_x - 1;
                 scan_x >= 0
                 && (game->world.terrain.tileMap[scan_x][cursor_y] == construction_tile::HorizontalA
                     || game->world.terrain.tileMap[scan_x][cursor_y] == construction_tile::HorizontalB
                     || game->world.terrain.tileMap[scan_x][cursor_y] == construction_tile::HorizontalC);
                 --scan_x)
                game->build.queue_tile_change(game->world.terrain, game->economy, scan_x, cursor_y, true, false);
        } else {
            int scan_y;
            for (scan_y = cursor_y;
                 scan_y < game->world.terrain.mapHeight
                 && (game->world.terrain.tileMap[cursor_x][scan_y] == construction_tile::VerticalA
                     || game->world.terrain.tileMap[cursor_x][scan_y] == construction_tile::VerticalB
                     || game->world.terrain.tileMap[cursor_x][scan_y] == construction_tile::VerticalC);
                 ++scan_y)
                game->build.queue_tile_change(game->world.terrain, game->economy, cursor_x, scan_y, true, false);
            for (scan_y = cursor_y - 1;
                 scan_y >= 0
                 && (game->world.terrain.tileMap[cursor_x][scan_y] == construction_tile::VerticalA
                     || game->world.terrain.tileMap[cursor_x][scan_y] == construction_tile::VerticalB
                     || game->world.terrain.tileMap[cursor_x][scan_y] == construction_tile::VerticalC);
                 --scan_y)
                game->build.queue_tile_change(game->world.terrain, game->economy, cursor_x, scan_y, true, false);
        }
    } else if (tile_code >= tile::VERTICAL_STATION
               && tile_code <= tile::HORIZONTAL_TERMINAL) {
        const int service_site_slot = game->fleet.find_service_site_at(cursor_x, cursor_y);
        if (service_site_slot != kNoFleetSlot) {
            const int service_x = unsigned_byte(game->fleet.serviceSites[service_site_slot].x);
            const int service_y = unsigned_byte(game->fleet.serviceSites[service_site_slot].y);
            if ((game->fleet.serviceSites[service_site_slot].flags & service_site::EXTENDED) != 0) {
                if ((game->fleet.serviceSites[service_site_slot].flags & service_site::HORIZONTAL) == 0) {
                    game->world.trackMap[service_x][service_y - 1] = 0;
                    game->world.trackMap[service_x][service_y + 1] = 0;
                    game->build.queue_tile_change(game->world.terrain, game->economy, service_x, service_y - 1, true, false);
                    game->build.queue_tile_change(game->world.terrain, game->economy, service_x, service_y + 1, true, false);
                } else {
                    game->world.trackMap[service_x - 1][service_y] = 0;
                    game->world.trackMap[service_x + 1][service_y] = 0;
                    game->build.queue_tile_change(game->world.terrain, game->economy, service_x - 1, service_y, true, false);
                    game->build.queue_tile_change(game->world.terrain, game->economy, service_x + 1, service_y, true, false);
                }
            }
            game->build.queue_tile_change(game->world.terrain, game->economy, service_x, service_y, true, false);
            game->world.advance_construction_timers(service_x, service_y, -6);
            game->world.trackMap[service_x][service_y] = 0;
            game->fleet.serviceSites[service_site_slot].flags = 0;
        }
    } else {
        game->build.queue_tile_change(game->world.terrain, game->economy, cursor_x, cursor_y, true, false);
        game->world.advance_construction_timers(cursor_x, cursor_y, -6);
    }
    game->apply_background_cost();
}

void Game::pick_trains_at_cursor() {
    Game *game = this;
    if (game->ui.blockBuildMenu) return;
    const int track_code =
        game->world.trackMap[game->ui.tileCursorX][game->ui.tileCursorY];
    if ((track_code & track_overlay::MASK) == track_overlay::JUNCTION) {
        game->build.junctionDirCursor =
            (int8_t)(track_code & track::OWNER_MASK);
        game->ready = false;
        game->ui.transitionCode = menu_screen::GenericOverlayC;
        game->set_state(GameState::MenuOverlay);
    }
    if ((track_code & track_overlay::MASK) == track_overlay::STATION) {
        game->build.stationDirCursor =
            (int8_t)(track_code & track::OWNER_MASK);
        game->ready = false;
        game->ui.transitionCode = menu_screen::GenericOverlayD;
        game->set_state(GameState::MenuOverlay);
    }
    if (track_code != tile::SERVICE_TRACK) return;

    game->trainAtCursor =
        game->fleet.find_train_head_at(game->ui.tileCursorX, game->ui.tileCursorY);
    if (game->trainAtCursor == kNoFleetSlot) {
        game->serviceVehicleAtCursor =
            game->fleet.find_service_site_at(game->ui.tileCursorX, game->ui.tileCursorY);
        if (game->serviceVehicleAtCursor != kNoFleetSlot) {
            game->ready = false;
        game->ui.transitionCode = menu_screen::TrainBuildReset;
            game->set_state(GameState::MenuOverlay);
        }
        return;
    }

    game->build.wagonType = game->fleet.trainCars[game->trainAtCursor].era;
    for (int option = 0; option < kWagonOptionCount; ++option)
        game->build.wagonOptionSlots[option] = wagon::NoOption;
    int car_slot = game->fleet.trainCars[game->trainAtCursor].nextCar;
    for (int option_slot = 0;
         option_slot < kWagonOptionCount && car_slot != train_chain::kEnd; ++option_slot) {
        switch (train_flag::type(game->fleet.trainCars[car_slot].flags)) {
            case wagon::PassengerMailCar:
                game->build.wagonOptionSlots[option_slot] = wagon::PassengerMail;
                break;
            case wagon::IndustryGroup1Car:
                game->build.wagonOptionSlots[option_slot] = wagon::IndustryGroup1;
                break;
            case wagon::IndustryGroup2CarA: case wagon::IndustryGroup2CarB:
                game->build.wagonOptionSlots[option_slot] = wagon::IndustryGroup2;
                break;
            case wagon::IndustryGroup3CarA: case wagon::IndustryGroup3CarB:
                game->build.wagonOptionSlots[option_slot] = wagon::IndustryGroup3;
                break;
        }
        car_slot = game->fleet.trainCars[car_slot].nextCar;
    }
    for (int option = 0; option < kWagonOptionCount; ++option)
        game->build.matchingWagonOptions[option] =
            game->build.wagonOptionSlots[option];
    game->build.wagonDiscountType = game->build.wagonType;
    game->scenario.difficultyPct = percentage::kBase;
    game->build.compute_train_cost(game->economy, game->scenario);
    game->build.baseTrainCostSnapshot = game->build.unitCost;
    game->scenario.difficultyPct = percentage::kBase
        - 3 * game->fleet.trainCars[game->trainAtCursor].ageYears;
    game->build.compute_train_cost(game->economy, game->scenario);
    game->build.currentTrainCostSnapshot = game->build.unitCost;
    game->ready = false;
    game->ui.transitionCode = menu_screen::TrainUpgrade;
    game->set_state(GameState::MenuOverlay);
}

void Game::build_place_track_piece(int32_t track_piece_type) {
    Game *game = this;
    int service_site_slot;
    switch (track_piece_type) {
        case build_menu::station_item::VerticalStation:
            if (!game->world.is_tile_buildable(game->ui.tileCursorX, game->ui.tileCursorY,
                                               terrain_build_type::SingleTile)
                || (service_site_slot = game->fleet.allocate_service_site(
                        service_site_type::Rail, game->ui.tileCursorX,
                        game->ui.tileCursorY, false, false))
                       == kNoFleetSlot
                || !game->spend_money(game->economy.powerCost))
                break;
            game->world.terrain.tileMap[game->ui.tileCursorX][game->ui.tileCursorY] = tile::VERTICAL_STATION;
            game->world.trackMap[game->ui.tileCursorX][game->ui.tileCursorY] = tile::SERVICE_TRACK;
            game->world.advance_construction_timers(
                game->ui.tileCursorX, game->ui.tileCursorY, -8);
            game->offerBoard.add_progress(offer_objective::Objective13, game->economy.powerCost);
            break;
        case build_menu::station_item::HorizontalStation:
            if (!game->world.is_tile_buildable(game->ui.tileCursorX, game->ui.tileCursorY,
                                               terrain_build_type::SingleTile)
                || (service_site_slot = game->fleet.allocate_service_site(
                        service_site_type::Rail, game->ui.tileCursorX,
                        game->ui.tileCursorY, true, false))
                       == kNoFleetSlot
                || !game->spend_money(game->economy.powerCost))
                break;
            game->world.terrain.tileMap[game->ui.tileCursorX][game->ui.tileCursorY] = tile::HORIZONTAL_STATION;
            game->world.trackMap[game->ui.tileCursorX][game->ui.tileCursorY] = tile::SERVICE_TRACK;
            game->world.advance_construction_timers(
                game->ui.tileCursorX, game->ui.tileCursorY, -8);
            game->offerBoard.add_progress(offer_objective::Objective13, game->economy.powerCost);
            break;
        case build_menu::station_item::VerticalTerminal:
            if (!game->world.is_tile_buildable(game->ui.tileCursorX, game->ui.tileCursorY,
                                               terrain_build_type::VerticalTerminal)
                || (service_site_slot = game->fleet.allocate_service_site(
                        service_site_type::Rail, game->ui.tileCursorX,
                        game->ui.tileCursorY, false, true))
                       == kNoFleetSlot
                || !game->spend_money(game->economy.structureCost))
                break;
            game->world.terrain.tileMap[game->ui.tileCursorX][game->ui.tileCursorY] = tile::VERTICAL_STATION;
            game->world.terrain.tileMap[game->ui.tileCursorX][game->ui.tileCursorY - 1] = tile::VERTICAL_TERMINAL;
            game->world.terrain.tileMap[game->ui.tileCursorX][game->ui.tileCursorY + 1] = tile::VERTICAL_TERMINAL;
            game->world.trackMap[game->ui.tileCursorX][game->ui.tileCursorY] = tile::SERVICE_TRACK;
            game->world.trackMap[game->ui.tileCursorX][game->ui.tileCursorY - 1] = tile::SERVICE_TRACK;
            game->world.trackMap[game->ui.tileCursorX][game->ui.tileCursorY + 1] = tile::SERVICE_TRACK;
            game->world.advance_construction_timers(
                game->ui.tileCursorX, game->ui.tileCursorY, -11);
            game->offerBoard.add_progress(offer_objective::Objective13, game->economy.structureCost);
            break;
        case build_menu::station_item::HorizontalTerminal:
            if (!game->world.is_tile_buildable(game->ui.tileCursorX, game->ui.tileCursorY,
                                               terrain_build_type::HorizontalTerminal)
                || (service_site_slot = game->fleet.allocate_service_site(
                        service_site_type::Rail, game->ui.tileCursorX,
                        game->ui.tileCursorY, true, true))
                       == kNoFleetSlot
                || !game->spend_money(game->economy.structureCost))
                break;
            game->world.terrain.tileMap[game->ui.tileCursorX][game->ui.tileCursorY] = tile::HORIZONTAL_STATION;
            game->world.terrain.tileMap[game->ui.tileCursorX - 1][game->ui.tileCursorY] = tile::HORIZONTAL_TERMINAL;
            game->world.terrain.tileMap[game->ui.tileCursorX + 1][game->ui.tileCursorY] = tile::HORIZONTAL_TERMINAL;
            game->world.trackMap[game->ui.tileCursorX][game->ui.tileCursorY] = tile::SERVICE_TRACK;
            game->world.trackMap[game->ui.tileCursorX - 1][game->ui.tileCursorY] = tile::SERVICE_TRACK;
            game->world.trackMap[game->ui.tileCursorX + 1][game->ui.tileCursorY] = tile::SERVICE_TRACK;
            game->world.advance_construction_timers(
                game->ui.tileCursorX, game->ui.tileCursorY, -11);
            game->offerBoard.add_progress(offer_objective::Objective13, game->economy.structureCost);
            break;
    }
    game->tick_service_sites(true);
    game->world.recount_city_zone_tiles();
}

void Game::compute_track_build(int32_t start_x,
                                 int32_t start_y, int32_t step_x,
                                 int32_t step_y, int32_t target_tile,
                                 int32_t start_tile, int32_t end_tile,
                                 int32_t middle_tile) {
    Game *game = this;
    int end_x = start_x;
    int end_y = start_y;
    bool found_target = false;
    int build_cost = game->economy.moneyBasePrimary;
    game->build.trackStartX = start_x;
    game->build.trackStartY = start_y;
    game->build.trackStepX = step_x;
    game->build.trackStepY = step_y;
    while ((end_x += step_x) >= 0 && (end_y += step_y) >= 0
           && end_x < game->world.terrain.mapWidth && end_y < game->world.terrain.mapHeight) {
        build_cost += game->economy.moneyBaseSecondary;
        if (game->world.terrain.tileMap[end_x][end_y] == target_tile) found_target = true;
        if (game->world.terrain.tileMap[end_x][end_y] != 0 || game->world.trackMap[end_x][end_y] != 0)
            break;
    }
    if (!found_target) return;

    game->build.selectionCost = build_cost;
    game->ui.buildMenuMode = build_menu::TrackBuildConfirm;
    game->ui.menuDirty = true;
    game->ui.buildMenuItemIndex = 0;
    game->build.buildCancelled = false;
    game->world.terrain.tileMap[end_x][end_y] = (int8_t)end_tile;
    game->world.trackMap[end_x][end_y] = tile::SERVICE_TRACK;
    game->world.terrain.tileMap[start_x][start_y] = (int8_t)start_tile;
    game->world.trackMap[start_x][start_y] = tile::SERVICE_TRACK;
    while ((start_x += step_x) != end_x || (start_y += step_y) != end_y) {
        game->world.terrain.tileMap[start_x][start_y] = (int8_t)middle_tile;
        game->world.advance_construction_timers(start_x, start_y, -4);
        game->world.trackMap[start_x][start_y] = tile::SERVICE_TRACK;
    }
    game->renderer.lastViewportCenterX = -1;
}

void Game::revert_track_tiles() {
    Game *game = this;
    int current_x = game->build.trackStartX;
    int current_y = game->build.trackStartY;
    for (;;) {
        const int tile_code = game->world.terrain.tileMap[current_x][current_y];
        if (tile_code < construction_tile::kFirst
            || tile_code > construction_tile::kLast) break;
        game->world.terrain.tileMap[current_x][current_y] =
            (int8_t)restored_construction_tile(tile_code);
        game->world.trackMap[current_x][current_y] = 0;
        current_x += game->build.trackStepX;
        current_y += game->build.trackStepY;
        if (!game->world.in_bounds(current_x, current_y)) break;
    }
}

void Game::place_track_at_cursor(bool first_direction,
                                  const TrackPlacementSpec& placement) {
    Game *game = this;
    const int cursor_x = game->ui.tileCursorX;
    const int cursor_y = game->ui.tileCursorY;
    const int tile_code = game->world.terrain.tileMap[cursor_x][cursor_y];
    if (game->ui.blockBuildMenu
        || game->world.terrain.tileElevation[cursor_x][cursor_y] < 0
        || (game->world.trackMap[cursor_x][cursor_y] != 0
            && !(placement.joinsExistingTrack
                 && tile_code == placement.joinedTrackTileCode))
        || !(tile_code == placement.joinedTrackTileCode
             || tile_code == placement.firstMatchTileCode
             || tile_code == placement.secondMatchTileCode || tile_code == 0
             || (tile_code >= tile::SCENERY_FIRST && tile_code <= tile::SCENERY_LAST)))
        return;

    int build_cost = tile_code == 0 ? 0 : game->economy.baseTrainSpeedEarly;
    build_cost += game->economy.accelerationEarly;
    if (placement.joinsExistingTrack
        && tile_code == placement.joinedTrackTileCode) {
        build_cost += game->economy.brakingEarly;
        if (build_cost > game->economy.money) {
            game->enter_out_of_money_state();
            return;
        }
        game->world.terrain.tileMap[cursor_x][cursor_y] = tile::CROSSING;
        int track_owner = game->world.trackMap[cursor_x][cursor_y] & track::OWNER_MASK;
        track_owner = placement.joinedTrackTileCode == tile::TRACK_FIRST
                          ? track::PRESENT_WITH_UPPER_OWNER_ONE | track_owner
                          : track::PRESENT_WITH_LOW_OWNER_ONE
                                | track_owner << track::DIR_OWNER_SHIFT;
        game->world.trackMap[cursor_x][cursor_y] = (int8_t)track_owner;
    } else {
        if (build_cost > game->economy.money) {
            game->enter_out_of_money_state();
            return;
        }
        game->world.terrain.tileMap[cursor_x][cursor_y] =
            tile_code == placement.firstMatchTileCode
                ? (int8_t)placement.firstResultTileCode
            : tile_code == placement.secondMatchTileCode
                ? (int8_t)placement.secondResultTileCode
            : (int8_t)(first_direction ? placement.firstTileCode
                                        : placement.secondTileCode);
        game->world.trackMap[cursor_x][cursor_y] = tile::SERVICE_TRACK;
    }
    game->economy.money -= build_cost;
    game->economy.totalSpent += build_cost;
    game->renderer.lastViewportCenterX = -1;
    game->record_money_change(-build_cost);
    game->offerBoard.add_progress(offer_objective::Objective13, build_cost);
    game->world.advance_construction_timers(cursor_x, cursor_y, -4);
}

void Game::normalize_track_bits() {
    Game *game = this;
    for (int map_x = 0; map_x < game->world.terrain.mapWidth; ++map_x) {
        for (int map_y = 0; map_y < game->world.terrain.mapHeight; ++map_y) {
            if (game->world.trackMap[map_x][map_y] == track::PRESENT) {
                const int tile_code = game->world.terrain.tileMap[map_x][map_y];
                game->world.terrain.tileMap[map_x][map_y] =
                    (int8_t)restored_construction_tile(tile_code);
                game->world.trackMap[map_x][map_y] = 0;
            }
            if (game->world.terrain.tileMap[map_x][map_y] != tile::CROSSING) continue;
            int low_owner = game->world.trackMap[map_x][map_y] & track::DIR_OWNER_MASK;
            int high_owner = (game->world.trackMap[map_x][map_y] & track::OWNER_MASK)
                            >> track::DIR_OWNER_SHIFT;
            if (low_owner == 0 && high_owner == 0) {
                game->world.terrain.tileMap[map_x][map_y] = 0;
                game->world.trackMap[map_x][map_y] = 0;
            } else if (low_owner == 0) {
                game->world.terrain.tileMap[map_x][map_y] = tile::TRACK_FIRST + 1;
                game->world.trackMap[map_x][map_y] = static_cast<int8_t>(track::PRESENT | high_owner);
            } else if (high_owner == 0) {
                game->world.terrain.tileMap[map_x][map_y] = tile::TRACK_FIRST;
                game->world.trackMap[map_x][map_y] = static_cast<int8_t>(track::PRESENT | low_owner);
            }
        }
    }
}

void Game::set_track_owner_code(int32_t owner) {
    Game *game = this;
    for (int map_x = 0; map_x < game->world.terrain.mapWidth; ++map_x) {
        for (int map_y = 0; map_y < game->world.terrain.mapHeight; ++map_y) {
            if (game->world.trackMap[map_x][map_y] == track::PRESENT) {
                if (owner == 1)
                    game->world.advance_construction_timers(map_x, map_y, -4);
                game->world.trackMap[map_x][map_y] = static_cast<int8_t>(track::PRESENT | owner);
            }
            if (game->world.terrain.tileMap[map_x][map_y] != tile::CROSSING) continue;
            int low_owner = game->world.trackMap[map_x][map_y] & track::DIR_OWNER_MASK;
            int high_owner = (game->world.trackMap[map_x][map_y] & track::OWNER_MASK)
                            >> track::DIR_OWNER_SHIFT;
            if (low_owner == 0) low_owner = owner;
            if (high_owner == 0) high_owner = owner;
            game->world.trackMap[map_x][map_y] =
                static_cast<int8_t>(track::PRESENT | low_owner | high_owner << track::DIR_OWNER_SHIFT);
        }
    }
}

void Game::rebuild_menu_type_indexes() {
    Game *game = this;
    game->ui.menuRowEnabledCount = 0;
    for (int row = 0; row < kOfferActiveRowCount; ++row) {
        if (game->offerBoard.offers[row].objectiveType == offer_objective::None) continue;
        game->ui.enabledMenuRowIndexes[game->ui.menuRowEnabledCount++] = (int8_t)row;
    }
}

// Build-menu items that act on the tile under the cursor (place a depot/track/
// terrain tile, demolish, smooth, or set an auto-route end). A bottom-bar click
// carries no map tile -- the cursor re-anchors to the screen centre every tick
// -- so these must only arm from the bar; the player then clicks the map to act
// on the tile they point at. Mirrors the place-at-cursor branches of
// open_build_menu below; keep the two in sync.
bool BuildController::tool_acts_on_cursor_tile(int32_t menu_mode, int32_t menu_item_id) {
    switch (menu_mode) {
        case build_menu::Toolbar: return menu_item_id == build_menu::toolbar_item::SelectTrain; // select/inspect at cursor
        case build_menu::RailTools: return menu_item_id == build_menu::rail_item::RouteAnchor
                     || menu_item_id == build_menu::rail_item::BridgeOrTunnel
                     || menu_item_id == build_menu::rail_item::Demolish
                     || menu_item_id == build_menu::rail_item::SmoothDown
                     || menu_item_id == build_menu::rail_item::SmoothUp;
        case build_menu::StationTools: return menu_item_id != build_menu::kBackItem;                                 // place depot (9 = back)
        case build_menu::PathTarget: return menu_item_id == build_menu::rail_item::RouteAnchor; // route target
        case build_menu::TrackPieceSelection: return menu_item_id != build_menu::kBackItem;                                 // place track piece (9 = back)
        case build_menu::RoadTools: return menu_item_id != build_menu::kBackItem;                                 // road tools (9 = back to toolbar)
        default: return false;                                    // navigation, confirm/cancel
    }
}

void Game::open_build_menu() {
    Game *game = this;
    int menu_item_id = game->ui.buildMenuItems[game->ui.buildMenuMode][game->ui.buildMenuItemIndex];
    bool allowed_while_paused =
        (game->ui.buildMenuMode == build_menu::Toolbar && (menu_item_id == build_menu::toolbar_item::Pause
                                                            || menu_item_id == build_menu::toolbar_item::RailToolsEntry
                                                            || menu_item_id == build_menu::toolbar_item::MainMenu
                                                            || menu_item_id == build_menu::toolbar_item::RoadToolsEntry))
        || (game->ui.buildMenuMode == build_menu::RailTools
            && (menu_item_id == build_menu::rail_item::TrackPieceMenu
                || menu_item_id == build_menu::rail_item::StationMenu
                || menu_item_id == build_menu::kBackItem))
        || (game->ui.buildMenuMode == build_menu::StationTools && menu_item_id == build_menu::kBackItem)
        || (game->ui.buildMenuMode == build_menu::TrackPieceSelection && menu_item_id == build_menu::kBackItem)
        || (game->ui.buildMenuMode == build_menu::RoadTools && menu_item_id == build_menu::kBackItem);
    if (game->ui.pauseFlag && !allowed_while_paused) return;

    switch (game->ui.buildMenuMode) {
        case build_menu::Toolbar:
            switch (menu_item_id) {
                case build_menu::toolbar_item::SelectTrain: game->pick_trains_at_cursor(); break;
                case build_menu::toolbar_item::RailToolsEntry:
                    game->ui.buildMenuMode = build_menu::RailTools;
                    game->ui.menuDirty = true;
                    game->ui.buildMenuItemIndex = 0;
                    break;
                case build_menu::toolbar_item::Bank:
                    game->ready = false; game->ui.transitionCode = menu_screen::LoanDialog;
                    game->set_state(GameState::MenuOverlay); break;
                case build_menu::toolbar_item::GraphsAndStats:
                    game->ready = false; game->ui.transitionCode = menu_screen::MonthlyReport;
                    game->set_state(GameState::MenuOverlay); break;
                case build_menu::toolbar_item::Tasks:
                    game->rebuild_menu_type_indexes();
                    game->ready = false; game->ui.transitionCode = menu_screen::OfferSelection;
                    game->set_state(GameState::MenuOverlay); break;
                case build_menu::toolbar_item::CurrentMission:
                    game->ready = false; game->ui.transitionCode = menu_screen::ScenarioNotice;
                    game->set_state(GameState::MenuOverlay); break;
                case build_menu::toolbar_item::TimeCompression:
                    game->ui.inputPending = true;
                    break;
                case build_menu::toolbar_item::MainMenu:
                    game->compute_map_train_metric(game->scenario.scenarioStep - 2);
                    game->ui.useDefaultOverlaySelection = false;
                    game->ready = false; game->ui.transitionCode = menu_screen::MainMenu;
                    game->set_state(GameState::MenuOverlay); break;
                case build_menu::toolbar_item::Pause: game->ui.pauseFlag = !game->ui.pauseFlag; break;
                case build_menu::toolbar_item::RoadToolsEntry:  // roads toolbar entry -> open the road submenu
                    game->ui.buildMenuMode = build_menu::RoadTools;
                    game->ui.menuDirty = true;
                    game->ui.buildMenuItemIndex = 0;
                    break;
            }
            break;
        case build_menu::RailTools:
            switch (menu_item_id) {
                case build_menu::rail_item::RouteAnchor:
                    if (game->ui.blockBuildMenu
                        || !game->world.is_placeable_terrain(game->ui.tileCursorX,
                                                  game->ui.tileCursorY))
                        break;
                    game->pathfinder.anchorX = game->ui.tileCursorX;
                    game->pathfinder.anchorY = game->ui.tileCursorY;
                    game->world.terrain.tileMap[game->pathfinder.anchorX][game->pathfinder.anchorY] =
                        tile::PATH_ANCHOR_MARKER;
                    game->ui.buildMenuMode = build_menu::PathTarget;
                    game->ui.menuDirty = true;
                    game->ui.buildMenuItemIndex = 0;
                    break;
                case build_menu::rail_item::TrackPieceMenu:
                    game->ui.buildMenuMode = build_menu::TrackPieceSelection;
                    game->ui.menuDirty = true;
                    game->ui.buildMenuItemIndex = 0;
                    break;
                case build_menu::rail_item::BridgeOrTunnel:
                    if (game->ui.blockBuildMenu) break;
                    if (game->world.terrain.tileMap[game->ui.tileCursorX][game->ui.tileCursorY] == tile::CURVE_UP)
                        game->compute_track_build(
                            game->ui.tileCursorX, game->ui.tileCursorY,
                            0, -1, tile::CURVE_DOWN, construction_tile::VerticalB,
                            construction_tile::VerticalC, construction_tile::VerticalA);
                    if (game->world.terrain.tileMap[game->ui.tileCursorX][game->ui.tileCursorY] == tile::CURVE_DOWN)
                        game->compute_track_build(
                            game->ui.tileCursorX, game->ui.tileCursorY,
                            0, 1, tile::CURVE_UP, construction_tile::VerticalC,
                            construction_tile::VerticalB, construction_tile::VerticalA);
                    if (game->world.terrain.tileMap[game->ui.tileCursorX][game->ui.tileCursorY] == tile::CURVE_LEFT)
                        game->compute_track_build(
                            game->ui.tileCursorX, game->ui.tileCursorY,
                            -1, 0, tile::CURVE_RIGHT, construction_tile::HorizontalB,
                            construction_tile::HorizontalC, construction_tile::HorizontalA);
                    if (game->world.terrain.tileMap[game->ui.tileCursorX][game->ui.tileCursorY] == tile::CURVE_RIGHT)
                        game->compute_track_build(
                            game->ui.tileCursorX, game->ui.tileCursorY,
                            1, 0, tile::CURVE_LEFT, construction_tile::HorizontalC,
                            construction_tile::HorizontalB, construction_tile::HorizontalA);
                    break;
                case build_menu::rail_item::StationMenu:
                    game->ui.buildMenuMode = build_menu::StationTools;
                    game->ui.menuDirty = true;
                    game->ui.buildMenuItemIndex = 0;
                    break;
                case build_menu::rail_item::Demolish: game->demolish_at_cursor(); break;
                case build_menu::rail_item::SmoothDown:
                    if (!game->ui.blockBuildMenu)
                        game->smooth_tile_at(game->build.swapMapX, game->build.swapMapY,
                                               false);
                    break;
                case build_menu::rail_item::SmoothUp:
                    if (!game->ui.blockBuildMenu)
                        game->smooth_tile_at(game->build.swapMapX, game->build.swapMapY,
                                               true);
                    break;
                case build_menu::kBackItem:
                    game->ui.buildMenuMode = build_menu::Toolbar;
                    game->ui.menuDirty = true;
                    game->ui.buildMenuItemIndex = 1;
                    break;
            }
            break;
        case build_menu::RoadTools:
            // Road submenu. Selecting any tool other than auto-route (auto-route item 1)
            // cancels a pending auto-route anchor.
            if (menu_item_id != build_menu::road_item::AutoRoute
                && game->build.roadAutoAnchorX >= 0) {
                game->build.roadAutoAnchorX = -1;
                game->build.roadAutoAnchorY = -1;
            }
            switch (menu_item_id) {
                case build_menu::road_item::LayTile:  // lay one road tile on the drivable ground under the cursor
                    game->place_road_tile_at_cursor();
                    break;
                case build_menu::road_item::AutoRoute:  // auto-route road: first click anchors, second click routes
                    if (game->build.roadAutoAnchorX < 0) {
                        game->build.roadAutoAnchorX = game->ui.tileCursorX;
                        game->build.roadAutoAnchorY = game->ui.tileCursorY;
                    } else {
                        game->auto_route_road(game->build.roadAutoAnchorX,
                                              game->build.roadAutoAnchorY,
                                              game->ui.tileCursorX, game->ui.tileCursorY);
                        game->build.roadAutoAnchorX = -1;
                        game->build.roadAutoAnchorY = -1;
                    }
                    break;
                case build_menu::road_item::PlaceStop:  // drop a bus/truck stop beside the road
                    game->place_road_stop_at_cursor();
                    break;
                case build_menu::road_item::BuyVehicle: {  // open the buy dialog for the stop under the cursor
                    // The cursor is the freshly-clicked tile here (pointer_pressed
                    // just resolved it), but it drifts to the screen centre while
                    // the dialog is up, so capture the stop now and let the
                    // dialog's dispatch spend it.
                    const int road_stop = game->road_stop_at(
                        game->ui.tileCursorX, game->ui.tileCursorY);
                    if (road_stop == kNoFleetSlot) break;
                    game->build.roadStopCursor = road_stop;
                    game->ready = false;
                    game->ui.transitionCode = menu_screen::RoadVehicleDialog;
                    game->set_state(GameState::MenuOverlay);
                    break;
                }
                case build_menu::kBackItem:  // back to the main toolbar (cursor on the roads button)
                    game->ui.buildMenuMode = build_menu::Toolbar;
                    game->ui.menuDirty = true;
                    game->ui.buildMenuItemIndex = 2;
                    break;
            }
            break;
        case build_menu::TrackBuildConfirm:
            if (menu_item_id == build_menu::confirm_item::Confirm) {
                if (game->build.selectionCost <= game->economy.money) {
                    game->set_track_owner_code(1);
                    game->economy.money -= game->build.selectionCost;
                    game->economy.totalSpent += game->build.selectionCost;
                    game->record_money_change(-game->build.selectionCost);
                    game->offerBoard.add_progress(offer_objective::Objective13, game->build.selectionCost);
                } else {
                    game->normalize_track_bits();
                    game->enter_out_of_money_state();
                }
            } else if (menu_item_id == build_menu::kBackItem) {
                if (game->build.buildCancelled) game->normalize_track_bits();
                else game->revert_track_tiles();
            }
            game->ui.buildMenuMode = build_menu::RailTools;
            game->ui.buildMenuItemIndex = game->build.buildCancelled ? 0 : 2;
            game->ui.menuDirty = true;
            game->renderer.lastViewportCenterX = -1;
            break;
        case build_menu::StationTools:
            if (menu_item_id != build_menu::kBackItem) {
                game->build_place_track_piece(menu_item_id);
                return;
            }
            game->ui.buildMenuMode = build_menu::RailTools;
            game->ui.menuDirty = true;
            game->ui.buildMenuItemIndex = 3;
            break;
        case build_menu::PathTarget:
            if (menu_item_id == build_menu::rail_item::RouteAnchor) {
                if (game->ui.blockBuildMenu
                    || !game->world.is_placeable_terrain(game->ui.tileCursorX,
                                              game->ui.tileCursorY))
                    break;
                game->pathfinder.targetX = game->ui.tileCursorX;
                game->pathfinder.targetY = game->ui.tileCursorY;
                game->world.terrain.tileMap[game->pathfinder.anchorX][game->pathfinder.anchorY] = 0;
                game->set_state(GameState::BuildingPath);
            } else if (menu_item_id == build_menu::kBackItem) {
                game->world.terrain.tileMap[game->pathfinder.anchorX][game->pathfinder.anchorY] = 0;
                game->ui.buildMenuMode = build_menu::RailTools;
                game->ui.menuDirty = true;
                game->ui.buildMenuItemIndex = 0;
            }
            break;
        case build_menu::TrackPieceSelection:
            switch (menu_item_id) {
                case build_menu::track_piece_item::VerticalTrack:
                    game->place_track_at_cursor(game->ui.vehicleMenuSelectIndex,
                                                track_piece::kVertical);
                    return;
                case build_menu::track_piece_item::HorizontalTrack:
                    game->place_track_at_cursor(game->ui.trainMenuSelectIndex,
                                                track_piece::kHorizontal);
                    return;
                case build_menu::track_piece_item::DiagonalTrackA:
                    game->place_track_at_cursor(true, track_piece::kDiagonalA);
                    return;
                case build_menu::track_piece_item::DiagonalTrackB:
                    game->place_track_at_cursor(true, track_piece::kDiagonalB);
                    return;
                case build_menu::kBackItem:
                    game->ui.buildMenuMode = build_menu::RailTools;
                    game->ui.menuDirty = true;
                    game->ui.buildMenuItemIndex = 1;
                    break;
            }
            break;
    }
}

void Game::add_selected_option(int32_t option) {
    Game *game = this;
    for (int slot = 0; slot < 5; ++slot) {
        if (game->build.wagonOptionSlots[slot] != wagon::NoOption) continue;
        game->build.wagonOptionSlots[slot] = option;
        break;
    }
}

void Game::pop_selected_option() {
    Game *game = this;
    for (int slot = 4; slot >= 0; --slot) {
        if (game->build.wagonOptionSlots[slot] == wagon::NoOption) continue;
        game->build.wagonOptionSlots[slot] = wagon::NoOption;
        break;
    }
}

int32_t Fleet::allocate_train_car(int32_t x, int32_t y,
                                  int32_t engine_type, int32_t era,
                                  int32_t direction, int32_t energy) {
    int train = kNoTrainSlot;
    for (int slot = 0; slot < kTrainSlotCount; ++slot) {
        if (!train_flag::isUsed(trainCars[slot].flags)) {
            train = slot;
            break;
        }
    }
    if (train != kNoTrainSlot) {
        trainCars[train].flags = static_cast<int8_t>(
            train_flag::USED_SLOT | train_flag::HEAD | engine_type);
        trainCars[train].nextCar = train_chain::kEnd;
        trainCars[train].x = (int8_t)x;
        trainCars[train].y = (int8_t)y;
        trainCars[train].direction = (int8_t)direction;
        trainCars[train].secondaryCargoOrBaseEnergy = (int8_t)energy;
        trainCars[train].phase = 0;
        trainCars[train].energyCargoOrRemovalTicks = 1;
        trainCars[train].lastServiceSite = -1;
        trainCars[train].era = (int8_t)era;
        trainCars[train].ageYears = 0;
        trainCars[train].stateCode = 7;
        trainCars[train].aiOverride = 0;
    }
    return train;
}

void Game::increment_cargo_option_count(int32_t car_type) {
    Game *game = this;
    for (int row = 0; row < kOfferActiveRowCount; ++row) {
        if (car_type == wagon::IndustryGroup3CarA
            && game->offerBoard.offers[row].objectiveType == offer_objective::Objective9)
            ++game->offerBoard.offers[row].progress;
        if (car_type == wagon::IndustryGroup2CarA
            && game->offerBoard.offers[row].objectiveType == offer_objective::Objective10)
            ++game->offerBoard.offers[row].progress;
        if (car_type == wagon::IndustryGroup1Car
            && game->offerBoard.offers[row].objectiveType == offer_objective::Objective11)
            ++game->offerBoard.offers[row].progress;
        if (car_type == wagon::PassengerMailCar
            && game->offerBoard.offers[row].objectiveType == offer_objective::Objective12)
            ++game->offerBoard.offers[row].progress;
    }
}

void Game::decrement_cargo_option_count(int32_t car_type) {
    Game *game = this;
    for (int row = 0; row < kOfferActiveRowCount; ++row) {
        if ((car_type == wagon::IndustryGroup3CarB || car_type == wagon::IndustryGroup3CarA)
            && game->offerBoard.offers[row].objectiveType == offer_objective::Objective9)
            --game->offerBoard.offers[row].progress;
        if ((car_type == wagon::IndustryGroup2CarB || car_type == wagon::IndustryGroup2CarA)
            && game->offerBoard.offers[row].objectiveType == offer_objective::Objective10)
            --game->offerBoard.offers[row].progress;
        if (car_type == wagon::IndustryGroup1Car
            && game->offerBoard.offers[row].objectiveType == offer_objective::Objective11)
            --game->offerBoard.offers[row].progress;
        if (car_type == wagon::PassengerMailCar
            && game->offerBoard.offers[row].objectiveType == offer_objective::Objective12)
            --game->offerBoard.offers[row].progress;
    }
}

void Game::decrement_train_cargo_option_counts(int32_t car) {
    Game *game = this;
    if (car == train_chain::kEnd) return;
    game->decrement_cargo_option_count(
        train_flag::type(game->fleet.trainCars[car].flags));
    game->decrement_train_cargo_option_counts(game->fleet.trainCars[car].nextCar);
}

bool Game::buy_train_at_cursor() {
    Game *game = this;
    if (game->economy.money < game->build.unitCost) {
        game->enter_out_of_money_state();
        return false;
    }
    int direction =
        (game->fleet.serviceSites[game->serviceVehicleAtCursor].flags
         & service_site::HORIZONTAL) == service_site::HORIZONTAL ? 2 : 0;
    // Spawn on the depot that opened this dialog, taken from the captured
    // serviceVehicleAtCursor. The live tile cursor re-anchors to the screen
    // centre every Playing tick, so by the time "buy" fires it has drifted off
    // the depot; using it here would strand the engine on an empty tile.
    int station_x = unsigned_byte(game->fleet.serviceSites[game->serviceVehicleAtCursor].x);
    int station_y = unsigned_byte(game->fleet.serviceSites[game->serviceVehicleAtCursor].y);
    int train = game->fleet.allocate_train_car(
        station_x, station_y,
        game->build.wagonType < 3 ? 0 : 1, game->build.wagonType,
        direction, 8 - game->build.wagonType);
    game->renderer.lastViewportCenterX = -1;
    for (int option = 0; option < kWagonOptionCount; ++option) {
        if (game->build.wagonOptionSlots[option] == wagon::NoOption) continue;
        int car_type = train_state_code::WagonTypeBase
            + game->build.wagonOptionSlots[option];
        if (car_type == train_state_code::SkippedCarType) ++car_type;
        train = game->fleet.append_car(train, car_type);
        game->renderer.lastViewportCenterX = -1;
        game->increment_cargo_option_count(car_type);
    }
    game->economy.money -= game->build.unitCost;
    game->economy.aiMoneySpent += game->build.unitCost;
    game->record_money_change(-game->build.unitCost);
    ++game->scenario.aiWagonsBought;
    return true;
}

bool Game::apply_train_upgrade(int32_t train,
                                 int32_t state_code, int32_t *options) {
    Game *game = this;
    int cost = game->build.unitCost - game->build.currentTrainCostSnapshot;
    if (game->economy.money < cost) {
        game->enter_out_of_money_state();
        return false;
    }
    game->fleet.trainCars[train].stateCode = (int16_t)state_code;
    game->decrement_train_cargo_option_counts(game->fleet.trainCars[train].nextCar);
    for (int option = 0; option < kWagonOptionCount; ++option) {
        if (options[option] != wagon::NoOption) {
            int car_type = options[option] + train_state_code::WagonTypeBase;
            if (car_type == train_state_code::SkippedCarType) ++car_type;
            game->increment_cargo_option_count(car_type);
        }
        game->fleet.trainCars[train].stateCode = (int16_t)(
            game->fleet.trainCars[train].stateCode
            | (options[option] & train_state_code::PackedFieldMask)
                  << (train_state_code::FirstWagonFieldShift
                      + train_state_code::PackedFieldWidth * option));
    }
    game->economy.money -= cost;
    game->economy.aiMoneySpent += cost;
    if (cost != 0) game->record_money_change(-cost);
    return true;
}
