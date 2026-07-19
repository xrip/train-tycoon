// Simulation tick functions.
#include "game.h"
#include "support/numeric.h"
#include <cstdlib>

namespace {

namespace metric {
constexpr int32_t kTrackTilePoints = 3;
constexpr int32_t kTrainHeadPoints = 400;
constexpr int32_t kTrainCarPoints = 55;
constexpr int32_t kRailTileFirst = tile::TRACK_FIRST;
constexpr int32_t kRailTileLast = track_turn_tile::VariantD;
constexpr int32_t kRailTileBonus = 94;
constexpr int32_t kRailTileBasePoints = 196;
constexpr int32_t kCrossingPoints = 168;
constexpr int32_t kConstructionTileFirst = construction_tile::kFirst;
constexpr int32_t kConstructionTileLast = construction_tile::kLast;
constexpr int32_t kTrainCarCostDivisor = 4;
constexpr int32_t kDaysPerMonth = 30;
constexpr int32_t kDaysPerYear = 365;
constexpr int32_t kFirstScenarioScoreSlot = 0;
}  // namespace metric

namespace construction {
constexpr int32_t kUpdateRadius = 5;
constexpr int32_t kTimerMaximum = 100;
}  // namespace construction

namespace population {
constexpr int32_t kRandomGrowthRange = 8;
constexpr int32_t kMinimum = 10;
constexpr int32_t kMaximum = 90;
}  // namespace population

namespace running_cost {
constexpr int32_t kPerCar = 2;
constexpr int32_t kEngine = 3;
constexpr int32_t kWagon = 7;
}  // namespace running_cost

namespace calendar {
enum MonthId : int32_t {
    January = 1,
    February = 2,
    March = 3,
    April = 4,
    May = 5,
    June = 6,
    July = 7,
    August = 8,
    September = 9,
    October = 10,
    November = 11,
    December = 12,
    YearEnd = 13,
};

constexpr int32_t kLongMonthRolloverDay = 32;
constexpr int32_t kFebruaryRolloverDay = 29;
constexpr int32_t kLeapFebruaryRolloverDay = 30;
constexpr int32_t kShortMonthRolloverDay = 31;
}  // namespace calendar

enum MonthlyEconomicEvent : int32_t {
    MoneyBaseIncrease = 0,
    EarlyTrainPerformanceIncrease = 1,
    LateTrainPerformanceIncrease = 2,
    TrackCostIncrease = 3,
    OperatingCostIncrease = 4,
    WagonCostIncrease = 5,
    CargoCostIncrease = 6,
};

namespace monthly_event {
constexpr int32_t kCount = 7;
constexpr int32_t kMoneyBaseStep = 9;
constexpr int32_t kEarlyTrainAccelerationStep = 3;
constexpr int32_t kLateTrainAccelerationStep = 2;
constexpr int32_t kPowerCostStep = 47;
constexpr int32_t kStructureCostStep = 98;
constexpr int32_t kOverheadCostStep = 3;
constexpr int32_t kMaintenanceCostStep = 59;
constexpr int32_t kCrewCostStep = 4;
constexpr int32_t kWagonCostStep1 = 378;
constexpr int32_t kWagonCostStep2 = 432;
constexpr int32_t kWagonCostStep3 = 518;
constexpr int32_t kWagonCostStep4 = 601;
constexpr int32_t kWagonCostStep5 = 714;
constexpr int32_t kWagonCostStep6 = 826;
constexpr int32_t kCargoCostStep1 = 45;
constexpr int32_t kCargoCostStep2 = 51;
constexpr int32_t kCargoCostStep3 = 43;
constexpr int32_t kCargoCostStep4 = 50;
}  // namespace monthly_event

namespace service_tick {
constexpr int32_t kNeighborhoodRadius = 3;
constexpr int32_t kPopulationGrowthFactor = 4;
constexpr int32_t kPopulationMonthDivisor = 30;
constexpr int32_t kRandomGrowthRange = 40;
constexpr int32_t kPopulationGrowthDivisor = 170;
constexpr int32_t kPassengerSourceThreshold = 3;
constexpr int32_t kMailSourceThreshold = 7;
constexpr int32_t kLargePassengerCapacity = 250;
constexpr int32_t kLargeIndustryCapacity = 450;
constexpr int32_t kLargeMailCapacity = 325;
constexpr int32_t kSmallPassengerCapacity = 100;
constexpr int32_t kSmallIndustryCapacity = 200;
constexpr int32_t kSmallMailCapacity = 150;

constexpr int32_t kIndustryGroup1ProductionFirst = industry_tile::Group1ProductionFirst;
constexpr int32_t kIndustryGroup1ProductionLast = industry_tile::Group1ProductionLast;
constexpr int32_t kIndustryGroup2ProductionFirst = industry_tile::Group2ProductionFirst;
constexpr int32_t kIndustryGroup2ProductionLast = industry_tile::Group2ProductionLast;
constexpr int32_t kIndustryGroup3ProductionFirst = industry_tile::Group3ProductionFirst;
constexpr int32_t kIndustryGroup3ProductionLast = industry_tile::Group3ProductionLast;
constexpr int32_t kIndustryGroup1DestinationFirst = industry_tile::Group1DestinationFirst;
constexpr int32_t kIndustryGroup1DestinationLast = industry_tile::Group1DestinationLast;
constexpr int32_t kIndustryGroup2DestinationFirst = industry_tile::Group2DestinationFirst;
constexpr int32_t kIndustryGroup2DestinationLast = industry_tile::Group2DestinationLast;
constexpr int32_t kIndustryGroup3DestinationFirst = industry_tile::Group3DestinationFirst;
constexpr int32_t kIndustryGroup3DestinationLast = industry_tile::Group3DestinationLast;
}  // namespace service_tick

namespace tile_change {
// A queued edit stores the original tile code with a high-bit marker until it
// is committed. The values below are legacy queue states, not visual tile ids.
constexpr int32_t kPendingBit = 128;
constexpr int32_t kQueueStartCountdown = 14;
constexpr int32_t kCommitCountdown = 7;
constexpr int32_t kEmptyTileCode = -128;
constexpr int32_t kTrackBeforeCrossingLast = tile::CROSSING - 1;
constexpr int32_t kTrackAfterCrossingFirst = tile::CROSSING + 1;
constexpr int32_t kTrackAfterCrossingLast = track_turn_tile::VariantD;
constexpr int32_t kConstructionClearFirst = construction_tile::HorizontalA;
constexpr int32_t kConstructionClearLast = construction_tile::VerticalA;
constexpr int32_t kConstructionRestoreFirst = construction_tile::VerticalB;
constexpr int32_t kConstructionRestoreLast = construction_tile::kLast;
constexpr int32_t kTrackRestoreOffset = track_build_tile::CornerVariantA;
constexpr int32_t kConstructionRestoreOffset = 52;
constexpr int32_t kCrossingReplacementCodeA = -57;
constexpr int32_t kCrossingReplacementCodeB = -58;
constexpr int32_t kEmptyTrackCode = 0;
}  // namespace tile_change

namespace tile_animation {
constexpr int32_t kFastPhasePeriod = 2;
constexpr int32_t kFrameCount = 3;
constexpr int32_t kSlowPhasePeriod = 3;
constexpr int32_t kWoodAgePeriod = 150;
constexpr int32_t kWoodTileFirst = 59;
constexpr int32_t kWoodTileLast = 61;
constexpr int32_t kWoodTileCycleEnd = 62;
constexpr int32_t kWoodRandomDivisor = 3;
constexpr int32_t kMapPhasePeriod = 10;
}  // namespace tile_animation

}  // namespace

void World::decay_construction_timers_quarterly(int32_t month) {
    if (month % 4 == 0) {
        for (int zone_index = 0; zone_index < kZoneCount; ++zone_index) {
            if (constructionTimers[zone_index] > 0)
                --constructionTimers[zone_index];
        }
    }
}

void World::advance_construction_timers(int32_t center_x, int32_t center_y, int32_t construction_delta) {
    bool seen[kZoneCount] = {false};
    for (int tile_x = center_x - construction::kUpdateRadius; tile_x <= center_x + construction::kUpdateRadius; ++tile_x) {
        for (int tile_y = center_y - construction::kUpdateRadius; tile_y <= center_y + construction::kUpdateRadius; ++tile_y) {
            if (!in_bounds(tile_x, tile_y)
                || (trackMap[tile_x][tile_y] & track_overlay::MASK)
                   != track_overlay::JUNCTION)
                continue;
            int owner_code = trackMap[tile_x][tile_y] & track::OWNER_MASK;
            if (seen[owner_code]) continue;
            seen[owner_code] = true;
            int timer_change;
            if (construction_delta < 0) {
                timer_change = constructionTimers[owner_code] * construction_delta * 3 / 200 - 1;
                if (timer_change < construction_delta) construction_delta = timer_change;
            } else {
                timer_change = (100 - constructionTimers[owner_code]) / 30 * construction_delta / 3 + 1;
                if (timer_change > construction_delta) timer_change = construction_delta;
            }
            constructionTimers[owner_code] = (int8_t)(constructionTimers[owner_code] + timer_change);
            if (constructionTimers[owner_code] < 0) constructionTimers[owner_code] = 0;
            if (constructionTimers[owner_code] > construction::kTimerMaximum) constructionTimers[owner_code] = construction::kTimerMaximum;
        }
    }
}

void World::recount_zone_population() {
    for (int zone_index = 0; zone_index < kZoneCount; ++zone_index) zonePopulation[zone_index] = 0;
    for (int tile_x = 0; tile_x < terrain.mapWidth; ++tile_x) {
        for (int tile_y = 0; tile_y < terrain.mapHeight; ++tile_y) {
            if (terrain.tileMap[tile_x][tile_y] < tile::BUILT_FIRST
                || terrain.tileMap[tile_x][tile_y] > tile::CITY_ZONE_LAST)
                continue;
            int zone_index = trackMap[tile_x][tile_y] & track::OWNER_MASK;
            ++zonePopulation[zone_index];
        }
    }
}

void World::update_city_population(JavaRandom& random) {
    for (int city_index = 0; city_index < kCitySlotCount; ++city_index) {
        cities[city_index].population += (int8_t)(random.next_int() % population::kRandomGrowthRange);
        if (cities[city_index].population < population::kMinimum) cities[city_index].population = population::kMinimum;
        if (cities[city_index].population > population::kMaximum) cities[city_index].population = population::kMaximum;
    }
}

void Fleet::age_one_year() {
    for (TrainCar& car : trainCars) ++car.ageYears;
}

void Fleet::roll_revenue_year() {
    for (TrainCar& car : trainCars) {
        car.previousRevenue = car.revenue;
        car.revenue = 0;
    }
}

void World::roll_station_revenue_year() {
    for (City& city : cities) {
        city.previousStationRevenue = city.stationRevenue;
        city.stationRevenue = 0;
    }
}

void Game::charge_train_running_costs() {
    Game* game = this;
    if (game->scenario.routeResourceFlags[0] != 0) return;
    for (int train_slot = 0; train_slot < kTrainSlotCount; ++train_slot) {
        if (!train_flag::isUsed(game->fleet.trainCars[train_slot].flags) || !train_flag::isHead(game->fleet.trainCars[train_slot].flags))
            continue;
        int train_x = unsigned_byte(game->fleet.trainCars[train_slot].x);
        int train_y = unsigned_byte(game->fleet.trainCars[train_slot].y);
        if (game->world.trackMap[train_x][train_y] != tile::SERVICE_TRACK) continue;
        int running_cost = game->fleet.count_cars(train_slot) * running_cost::kPerCar;
        if (train_flag::type(game->fleet.trainCars[train_slot].flags)
            == wagon::EngineTypeA)
            running_cost += running_cost::kEngine;
        else if (train_flag::type(game->fleet.trainCars[train_slot].flags)
                 == wagon::EngineTypeB)
            running_cost += running_cost::kWagon;
        game->fleet.trainCars[train_slot].revenue -= running_cost;
        game->economy.money -= running_cost;
        game->economy.fuelCostAccrued += running_cost;
    }
}

int32_t Fleet::count_cars(int32_t car) const {
    return car != train_chain::kEnd
               ? count_cars(trainCars[car].nextCar) + 1
               : 0;
}

void Game::remove_train_from_map(int32_t car, bool queue_tile) {
    Game* game = this;
    if (car == train_chain::kEnd) return;
    remove_train_from_map(game->fleet.trainCars[car].nextCar, queue_tile);
    game->fleet.trainCars[car].flags = static_cast<int8_t>(
        game->fleet.trainCars[car].flags
        | train_flag::USED_SLOT
        | train_flag::HEAD
        | train_flag::REMOVAL_PENDING);
    game->fleet.trainCars[car].nextCar = train_chain::kEnd;
    game->fleet.trainCars[car].energyCargoOrRemovalTicks = 7;
    if (queue_tile)
        game->build.queue_tile_change(game->world.terrain, game->economy, unsigned_byte(game->fleet.trainCars[car].x),
                                    unsigned_byte(game->fleet.trainCars[car].y), false, true);
    game->renderer.lastViewportCenterX = -1;
    game->ready = false;
    game->ui.transitionCode = menu_screen::GenericOverlayJ;
    game->set_state(GameState::MenuOverlay);
}

void Game::merge_train_at(int32_t train_head) {
    Game* game = this;
    for (int other_train = 0; other_train < kTrainSlotCount; ++other_train) {
        if (train_head == other_train || !train_flag::isUsed(game->fleet.trainCars[other_train].flags)
            || !train_flag::isHead(game->fleet.trainCars[other_train].flags)
            || !game->fleet.has_car_at(other_train, unsigned_byte(game->fleet.trainCars[train_head].x),
                                    unsigned_byte(game->fleet.trainCars[train_head].y)))
            continue;
        remove_train_from_map(other_train, true);
        remove_train_from_map(train_head, true);
    }
}

void Game::maybe_take_ai_turn() {
    Game* game = this;
    if (game->ui.buildMenuMode != build_menu::TrackBuildConfirm && game->ui.buildMenuMode != build_menu::PathTarget
        && game->rng.next_int() % game->scenario.aiFrequency > game->scenario.aiThreshold)
        game->set_state(GameState::AiTurn);
}

void Game::upgrade_one_ai_train_for_era() {
    Game* game = this;
    for (int train_slot = 0; train_slot < kTrainSlotCount; ++train_slot) {
        if (!train_flag::isUsed(game->fleet.trainCars[train_slot].flags) || !train_flag::isHead(game->fleet.trainCars[train_slot].flags))
            continue;
        int train_x = unsigned_byte(game->fleet.trainCars[train_slot].x);
        int train_y = unsigned_byte(game->fleet.trainCars[train_slot].y);
        if ((game->world.trackMap[train_x][train_y] < track::AI_PIECE_FIRST
             || game->world.trackMap[train_x][train_y] >= track::AI_PIECE_WRAP)
            || game->fleet.trainCars[train_slot].era >= game->scenario.currentEra)
            continue;
        game->fleet.trainCars[train_slot].era = (int8_t)game->scenario.currentEra;
        game->fleet.trainCars[train_slot].flags = static_cast<int8_t>(
            train_flag::USED_SLOT | train_flag::HEAD
            | game->scenario.currentEra / 3);
        game->fleet.trainCars[train_slot].secondaryCargoOrBaseEnergy =
            (int8_t)(8 - game->scenario.currentEra);
        break;
    }
}

void Game::reset_ai_depot_train_ages() {
    Game* game = this;
    for (int train_slot = 0; train_slot < kTrainSlotCount; ++train_slot) {
        if (!train_flag::isUsed(game->fleet.trainCars[train_slot].flags)) continue;
        int train_x = unsigned_byte(game->fleet.trainCars[train_slot].x);
        int train_y = unsigned_byte(game->fleet.trainCars[train_slot].y);
        if (game->world.trackMap[train_x][train_y] >= track::AI_PIECE_FIRST
            && game->world.trackMap[train_x][train_y] < track::AI_PIECE_WRAP)
            game->fleet.trainCars[train_slot].ageYears = 0;
    }
}

void Economy::calculate_period_profit() {
    terrainTotal = loanBalance * interestRate / 1200;
    decadeEnterpriseTax = totalRevenue * enterpriseTaxPct / percentage::kBase;
    netProfit = totalRevenue - terrainTotal - totalSpent - aiMoneySpent
                - fuelCostAccrued - subsidyCost - decadeEnterpriseTax;
}

void Economy::compute_year_end_metrics() {
    calculate_period_profit();
    yearProfitHistory[0] = netProfit;
    yearRevenueHistory[0] = totalRevenue;
    decadeProfitHistory[0] = decadeProfitGraphTotal + netProfit;
    altDecadeRevenueTotal[0] = revenueTotal + totalRevenue;
    decadeSpentWithCurrent = decadeCostTotal + totalSpent;
    decadeAiSpentWithCurrent = decadeSpentTotal + aiMoneySpent;
    decadeFuelWithCurrent = decadeFuelCost + fuelCostAccrued;
    decadeSubsidyWithCurrent = decadeSubsidyTotal + subsidyCost;
    decadeRevenueWithCurrent = decadeRevenueTotal + totalRevenue;
    decadeProfitWithCurrent = decadeProfitTotal + netProfit;
    decadeEnterpriseWithCurrent = decadeEnterpriseTotal + decadeEnterpriseTax;
}

void BuildController::queue_tile_change(Terrain& terrain,
                                        const Economy& economy,
                                        int32_t x, int32_t y,
                                        bool charge, bool visual_only) {
    bool can_apply_change = !charge;
    int change_cost;
    if (charge) {
        change_cost = terrain.tileMap[x][y] >= tile::SCENERY_FIRST && terrain.tileMap[x][y] <= tile::SCENERY_LAST
             ? economy.baseTrainSpeedLate : -economy.accelerationLate;
        if (economy.money >= change_cost || economy.money < 0) {
            backgroundCost += change_cost;
            can_apply_change = true;
            demolitionAffordable = true;
        } else {
            demolitionAffordable = false;
        }
    }
    if (!can_apply_change) return;
    terrain.queue_change(x, y, visual_only);
}

bool Terrain::queue_change(int32_t x, int32_t y, bool visual_only) {
    for (int index = kTileQueueCount - 1; index >= 0; --index) {
        if (tileQueue[index].countdown != 0
            && unsigned_byte(tileQueue[index].x) == x
            && unsigned_byte(tileQueue[index].y) == y)
            return false;
    }
    if (unsigned_byte(tileMap[x][y]) < tile_change::kPendingBit)
        tileMap[x][y] = static_cast<int8_t>(
            tileMap[x][y] + tile_change::kPendingBit);
    int slot = 0;
    for (int index = kTileQueueCount - 1; index >= 0; --index) {
        if (tileQueue[index].countdown == 0) {
            slot = index;
            break;
        }
    }
    tileQueue[slot].countdown = tile_change::kQueueStartCountdown;
    tileQueueVisualOnly[slot] = visual_only;
    tileQueue[slot].x = static_cast<int8_t>(x);
    tileQueue[slot].y = static_cast<int8_t>(y);
    return true;
}

void World::place_city_tile(JavaRandom& random, int32_t tile_x, int32_t tile_y,
                            int32_t track_owner) {
    if (terrain.tileMap[tile_x][tile_y] == 0
        || (terrain.tileMap[tile_x][tile_y] >= tile::SCENERY_FIRST && terrain.tileMap[tile_x][tile_y] <= tile::SCENERY_LAST)) {
        terrain.tileMap[tile_x][tile_y] =
            random_city_tile(random);
        trackMap[tile_x][tile_y] = (int8_t)track_owner;
    }
}

bool World::grow_city_street(JavaRandom& random) {
    int tile_x = std::abs(random.next_int() % (terrain.mapWidth - 2)) + 1;
    int tile_y = std::abs(random.next_int() % (terrain.mapHeight - 2)) + 1;
    if (terrain.tileMap[tile_x][tile_y] != 0
        && (terrain.tileMap[tile_x][tile_y] < tile::SCENERY_FIRST || terrain.tileMap[tile_x][tile_y] > tile::SCENERY_LAST))
        return false;

    int8_t left_tile = terrain.tileMap[tile_x - 1][tile_y];
    int8_t right_tile = terrain.tileMap[tile_x + 1][tile_y];
    int8_t up_tile = terrain.tileMap[tile_x][tile_y - 1];
    int8_t down_tile = terrain.tileMap[tile_x][tile_y + 1];
    bool has_left_road = left_tile == road_tile::kStraightShape43 || left_tile == road_tile::kJunctionShape44 || left_tile == road_tile::kCornerShape45 || left_tile == road_tile::kCornerShape46;
    bool has_right_road = right_tile == road_tile::kStraightShape43 || right_tile == road_tile::kJunctionShape44 || right_tile == road_tile::kCornerShape47 || right_tile == road_tile::kCornerShape48;
    bool has_up_road = up_tile == road_tile::kStraightShape42 || up_tile == road_tile::kJunctionShape44 || up_tile == road_tile::kCornerShape46 || up_tile == road_tile::kCornerShape47;
    bool has_down_road = down_tile == road_tile::kStraightShape42 || down_tile == road_tile::kJunctionShape44 || down_tile == road_tile::kCornerShape45 || down_tile == road_tile::kCornerShape48;
    if (!has_left_road && !has_right_road && !has_up_road && !has_down_road) return false;

    int track_owner_code;
    if (has_left_road) track_owner_code = trackMap[tile_x - 1][tile_y];
    else if (has_right_road) track_owner_code = trackMap[tile_x + 1][tile_y];
    else if (has_up_road) track_owner_code = trackMap[tile_x][tile_y - 1];
    else track_owner_code = trackMap[tile_x][tile_y + 1];
    if ((track_owner_code & track_overlay::MASK) != track_overlay::JUNCTION)
        return false;  // player roads have no city owner and must not grow zones
    int owner_code = track_owner_code & track::OWNER_MASK;
    if (owner_code < 0 || owner_code >= kCitySlotCount) return false;

    const int road_neighbor_count = (has_left_road ? 1 : 0)
        + (has_right_road ? 1 : 0) + (has_up_road ? 1 : 0)
        + (has_down_road ? 1 : 0);
    const int32_t deterministic_shape = road_shape_from_neighbors(
        has_left_road, has_right_road, has_up_road, has_down_road);
    if (road_neighbor_count > 2
        || (deterministic_shape >= road_tile::kCornerShape45
            && deterministic_shape <= road_tile::kCornerShape48)) {
        // Junctions and bends follow the shared shape rule; single and
        // straight links get a random chance of a corner/junction instead,
        // so grown cities look organic rather than gridded.
        terrain.tileMap[tile_x][tile_y] =
            static_cast<int8_t>(deterministic_shape);
    } else {
        int shape_roll = std::abs(random.next_int() % 13);
        if (shape_roll == 12) {
            terrain.tileMap[tile_x][tile_y] = road_tile::kJunctionShape44;
        } else if (shape_roll == 11) {
            if (has_left_road) terrain.tileMap[tile_x][tile_y] = road_tile::kCornerShape47;
            if (has_right_road) terrain.tileMap[tile_x][tile_y] = road_tile::kCornerShape46;
            if (has_up_road) terrain.tileMap[tile_x][tile_y] = road_tile::kCornerShape45;
            if (has_down_road) terrain.tileMap[tile_x][tile_y] = road_tile::kCornerShape46;
        } else if (shape_roll == 10) {
            if (has_left_road) terrain.tileMap[tile_x][tile_y] = road_tile::kCornerShape48;
            if (has_right_road) terrain.tileMap[tile_x][tile_y] = road_tile::kCornerShape45;
            if (has_up_road) terrain.tileMap[tile_x][tile_y] = road_tile::kCornerShape48;
            if (has_down_road) terrain.tileMap[tile_x][tile_y] = road_tile::kCornerShape47;
        } else {
            if (has_left_road || has_right_road) terrain.tileMap[tile_x][tile_y] = road_tile::kStraightShape43;
            if (has_up_road || has_down_road) terrain.tileMap[tile_x][tile_y] = road_tile::kStraightShape42;
        }
    }

    switch (terrain.tileMap[tile_x][tile_y]) {
        case road_tile::kStraightShape42:
            place_city_tile(random, tile_x - 1, tile_y, track_owner_code);
            place_city_tile(random, tile_x + 1, tile_y, track_owner_code);
            break;
        case road_tile::kStraightShape43:
            place_city_tile(random, tile_x, tile_y - 1, track_owner_code);
            place_city_tile(random, tile_x, tile_y + 1, track_owner_code);
            break;
        case road_tile::kCornerShape45:
            place_city_tile(random, tile_x - 1, tile_y, track_owner_code);
            place_city_tile(random, tile_x, tile_y + 1, track_owner_code);
            place_city_tile(random, tile_x - 1, tile_y + 1, track_owner_code);
            break;
        case road_tile::kCornerShape46:
            place_city_tile(random, tile_x - 1, tile_y, track_owner_code);
            place_city_tile(random, tile_x, tile_y - 1, track_owner_code);
            place_city_tile(random, tile_x - 1, tile_y - 1, track_owner_code);
            break;
        case road_tile::kCornerShape47:
            place_city_tile(random, tile_x + 1, tile_y, track_owner_code);
            place_city_tile(random, tile_x, tile_y - 1, track_owner_code);
            place_city_tile(random, tile_x + 1, tile_y - 1, track_owner_code);
            break;
        case road_tile::kCornerShape48:
            place_city_tile(random, tile_x + 1, tile_y, track_owner_code);
            place_city_tile(random, tile_x, tile_y + 1, track_owner_code);
            place_city_tile(random, tile_x + 1, tile_y + 1, track_owner_code);
            break;
    }
    trackMap[tile_x][tile_y] = (int8_t)track_owner_code;
    // Re-orient the streets this cell joins: a straight that gained a branch
    // becomes a corner or junction. The new cell keeps its growth shape (it may
    // be a random junction); only the pre-existing neighbours are updated, with
    // the same shared rule the road editor and map generator use.
    const auto is_street = [this](int x, int y) {
        if (x < 0 || y < 0 || x >= terrain.mapWidth || y >= terrain.mapHeight)
            return false;
        const int code = unsigned_byte(terrain.tileMap[x][y]);
        return code >= road_tile::kFirstShape && code <= road_tile::kLastShape;
    };
    const int neighbor_x[4] = {tile_x - 1, tile_x + 1, tile_x, tile_x};
    const int neighbor_y[4] = {tile_y, tile_y, tile_y - 1, tile_y + 1};
    for (int direction = 0; direction < 4; ++direction) {
        const int cell_x = neighbor_x[direction];
        const int cell_y = neighbor_y[direction];
        if (!is_street(cell_x, cell_y)) continue;
        terrain.tileMap[cell_x][cell_y] = static_cast<int8_t>(
            road_shape_from_neighbors(is_street(cell_x - 1, cell_y),
                                      is_street(cell_x + 1, cell_y),
                                      is_street(cell_x, cell_y - 1),
                                      is_street(cell_x, cell_y + 1)));
    }
    return true;
}

void Game::persist_settings_snapshot() {
    Game *game = this;
    game->saveCodec.load_slot(save::slot::Auxiliary, save::MODE_WRITE);
    game->saveCodec.walk_settings(*game);
    game->saveCodec.persist();
}

int32_t Game::compute_map_train_metric(int32_t score_slot) {
    Game *game = this;
    int metric = game->economy.trainRevenueScore;
    for (int tile_x = 0; tile_x < game->world.terrain.mapWidth; ++tile_x) {
        for (int tile_y = 0; tile_y < game->world.terrain.mapHeight; ++tile_y) {
            if (game->world.trackMap[tile_x][tile_y] != tile::SERVICE_TRACK) continue;
            metric += metric::kTrackTilePoints;
            for (int train_slot = 0; train_slot < kTrainSlotCount; ++train_slot) {
                if (!train_flag::isUsed(game->fleet.trainCars[train_slot].flags)
                    || unsigned_byte(game->fleet.trainCars[train_slot].x) != tile_x || unsigned_byte(game->fleet.trainCars[train_slot].y) != tile_y)
                    continue;
                metric += train_flag::isHead(game->fleet.trainCars[train_slot].flags)
                              ? metric::kTrainHeadPoints
                              : metric::kTrainCarPoints;
            }
        }
    }
    metric += game->economy.dayOfMonth
              + metric::kDaysPerMonth * game->economy.monthOfYear
              + metric::kDaysPerYear
                * (game->economy.year - game_calendar::kBaseYear);
    if (score_slot < metric::kFirstScenarioScoreSlot) {
        if (metric > game->scenario.bestScenarioScore) game->scenario.bestScenarioScore = metric;
    } else {
        game->scenario.scenarioScores[score_slot] = metric;
    }
    game->persist_settings_snapshot();
    return metric;
}

bool Scenario::complete_milestone_if_met(const Economy& economy) {
    bool milestone_met = false;
    if (!milestonesEnabled) {
        switch (scenarioMilestoneIndex) {
            case scenario_milestone::Threshold1: milestone_met = currentMilestoneScore <= milestoneThreshold1; break;
            case scenario_milestone::Threshold2: milestone_met = currentMilestoneScore <= milestoneThreshold2; break;
            case scenario_milestone::Threshold3: milestone_met = currentMilestoneScore <= milestoneThreshold3; break;
            case scenario_milestone::Threshold4: milestone_met = currentMilestoneScore <= milestoneThreshold4; break;
            case scenario_milestone::Threshold5: milestone_met = currentMilestoneScore <= milestoneThreshold5; break;
            case scenario_milestone::CombinedThreshold1And2:
                milestone_met = currentMilestoneScore <= milestoneThreshold1
                     && milestoneCoThreshold2 <= milestoneThreshold2;
                break;
            case scenario_milestone::CombinedThreshold1And3:
                milestone_met = currentMilestoneScore <= milestoneThreshold1
                     && milestoneCoThreshold2 <= milestoneThreshold3;
                break;
            case scenario_milestone::CombinedThreshold1And4:
                milestone_met = currentMilestoneScore <= milestoneThreshold1
                     && milestoneCoThreshold2 <= milestoneThreshold4;
                break;
            case scenario_milestone::CombinedThreshold2And3:
                milestone_met = currentMilestoneScore <= milestoneThreshold2
                     && milestoneCoThreshold2 <= milestoneThreshold3;
                break;
            case scenario_milestone::CombinedThreshold2And4:
                milestone_met = currentMilestoneScore <= milestoneThreshold2
                     && milestoneCoThreshold2 <= milestoneThreshold4;
                break;
            case scenario_milestone::CombinedThreshold4And5:
                milestone_met = currentMilestoneScore <= milestoneThreshold4
                     && milestoneCoThreshold2 <= milestoneThreshold5;
                break;
            case scenario_milestone::LifetimeRevenue: milestone_met = currentMilestoneScore <= economy.lifetimeTrainRevenue; break;
            case scenario_milestone::LoanCleared: milestone_met = economy.loanBalance == 0; break;
            case scenario_milestone::TargetYear: milestone_met = economy.year == currentMilestoneScore; break;
            case scenario_milestone::WagonsBought: milestone_met = currentMilestoneScore <= aiWagonsBought; break;
        }
    }
    if (milestone_met) milestonesEnabled = true;
    return milestone_met;
}

void Game::check_scenario_milestone() {
    Game *game = this;
    if (!game->scenario.complete_milestone_if_met(game->economy)) return;
    game->ready = false;
    game->ui.transitionCode = menu_screen::ContinueGame;
    if (game->scenario.scenarioStep >= 2) {
        if (game->scenario.scenarioStepCompare < game->scenario.scenarioStep)
            game->scenario.scenarioStepCompare = game->scenario.scenarioStep;
        game->persist_settings_snapshot();
        game->scenario.currentScenarioScore = game->compute_map_train_metric(game->scenario.scenarioStep - 2);
    }
    game->set_state(GameState::MenuOverlay);
}

int32_t Game::scan_map_metric() const {
    const Game *game = this;
    int32_t metric = 0;
    for (int tile_x = 0; tile_x < game->world.terrain.mapWidth; ++tile_x) {
        for (int tile_y = 0; tile_y < game->world.terrain.mapHeight; ++tile_y) {
            if (game->world.trackMap[tile_x][tile_y] != tile::SERVICE_TRACK) continue;
            const int tile_code = game->world.terrain.tileMap[tile_x][tile_y];
            if (tile_code >= metric::kRailTileFirst && tile_code <= metric::kRailTileLast) {
                if (tile_code > tile::CROSSING) metric += metric::kRailTileBonus;
                metric += metric::kRailTileBasePoints;
            }
            if (tile_code == tile::CROSSING) metric += metric::kCrossingPoints;
            if (tile_code >= metric::kConstructionTileFirst && tile_code <= metric::kConstructionTileLast)
                metric += game->economy.accelerationEarly;
            if (tile_code >= tile::VERTICAL_STATION
                && tile_code <= tile::HORIZONTAL_TERMINAL) {
                if (tile_code < tile::VERTICAL_TERMINAL)
                metric += game->economy.powerCost;
                else metric += game->economy.powerCost / 2;
            }
        }
    }
    for (int train_slot = 0; train_slot < kTrainSlotCount; ++train_slot) {
        if (!train_flag::isUsed(game->fleet.trainCars[train_slot].flags)) continue;
        const int train_x = unsigned_byte(game->fleet.trainCars[train_slot].x);
        const int train_y = unsigned_byte(game->fleet.trainCars[train_slot].y);
        if (game->world.trackMap[train_x][train_y] != tile::SERVICE_TRACK) continue;
        if (train_flag::isHead(game->fleet.trainCars[train_slot].flags)) {
            metric += game->economy.train_cost_for_era(
                game->fleet.trainCars[train_slot].era);
        } else {
            metric += (game->economy.wagonOptionCostPassengerMail
                       + game->economy.wagonOptionCostIndustryGroup1
                       + game->economy.wagonOptionCostIndustryGroup2
                       + game->economy.wagonOptionCostIndustryGroup3)
                      / metric::kTrainCarCostDivisor;
        }
    }
    return metric - game->economy.loanBalance;
}

void Game::new_year_notify() {
    Game *game = this;
    game->fleet.roll_revenue_year();
    game->world.roll_station_revenue_year();
    game->economy.calculate_period_profit();
    game->economy.prevTotalSpent = game->economy.totalSpent;
    game->economy.prevAiMoneySpent = game->economy.aiMoneySpent;
    game->economy.prevFuelCost = game->economy.fuelCostAccrued;
    game->economy.prevSubsidyCost = game->economy.subsidyCost;
    game->economy.prevTotalRevenue = game->economy.totalRevenue;
    game->economy.prevTerrainTotal = game->economy.terrainTotal;
    game->economy.prevEnterpriseTotal = game->economy.decadeEnterpriseTax;
    game->economy.previousYearProfit = game->economy.netProfit;
    for (int history_index = kHistoryCount - 2; history_index >= 0; --history_index) {
        game->economy.yearProfitHistory[history_index + 1] = game->economy.yearProfitHistory[history_index];
        game->economy.yearRevenueHistory[history_index + 1] = game->economy.yearRevenueHistory[history_index];
        game->economy.yearCompanyValueHistory[history_index + 1] = game->economy.yearCompanyValueHistory[history_index];
    }
    game->economy.yearProfitHistory[0] = game->economy.yearRevenueHistory[0] = game->economy.yearCompanyValueHistory[0] = 0;
    game->economy.decadeCostTotal += game->economy.totalSpent;
    game->economy.decadeSpentTotal += game->economy.aiMoneySpent;
    game->economy.decadeFuelCost += game->economy.fuelCostAccrued;
    game->economy.decadeSubsidyTotal += game->economy.subsidyCost;
    game->economy.decadeRevenueTotal += game->economy.totalRevenue;
    game->economy.decadeProfitTotal += game->economy.netProfit;
    game->economy.decadeEnterpriseTotal += game->economy.decadeEnterpriseTax;
    game->economy.decadeProfitGraphTotal += game->economy.netProfit;
    game->economy.revenueTotal += game->economy.totalRevenue;
    if (game->economy.monthOfYear == calendar::YearEnd) {
        game->economy.prevDecadeSpent = game->economy.decadeSpentWithCurrent;
        game->economy.prevDecadeAiSpent = game->economy.decadeAiSpentWithCurrent;
        game->economy.prevDecadeFuel = game->economy.decadeFuelWithCurrent;
        game->economy.prevDecadeSubsidy = game->economy.decadeSubsidyWithCurrent;
        game->economy.prevDecadeRevenue = game->economy.decadeRevenueWithCurrent;
        game->economy.previousLegacyDecadeMetric = game->economy.legacyDecadeMetric;
        game->economy.previousDecadeEnterpriseTax = game->economy.decadeEnterpriseWithCurrent;
        game->economy.previousDecadeProfit = game->economy.decadeProfitWithCurrent;
        game->economy.decadeCostTotal = game->economy.decadeSpentTotal = game->economy.decadeFuelCost = 0;
        game->economy.decadeSubsidyTotal = game->economy.decadeRevenueTotal = game->economy.decadeProfitTotal = 0;
        game->economy.decadeEnterpriseTotal = 0;
        for (int history_index = kHistoryCount - 2; history_index >= 0; --history_index) {
            game->economy.decadeProfitHistory[history_index + 1] = game->economy.decadeProfitHistory[history_index];
            game->economy.altDecadeRevenueTotal[history_index + 1] = game->economy.altDecadeRevenueTotal[history_index];
            game->economy.altDecadeTotalCost[history_index + 1] = game->economy.altDecadeTotalCost[history_index];
        }
        game->economy.decadeProfitGraphTotal = game->economy.revenueTotal = 0;
        game->economy.decadeProfitHistory[0] = game->economy.altDecadeRevenueTotal[0] = 0;
        game->economy.altDecadeTotalCost[0] = 0;
    }
    game->economy.altDecadeTotalCost[0] = game->scan_map_metric();
    game->economy.money -= game->economy.terrainTotal;
    if (game->economy.terrainTotal > 0)
        game->record_money_change(-game->economy.terrainTotal);
    game->economy.money -= game->economy.decadeEnterpriseTax;
    if (game->economy.decadeEnterpriseTax > 0)
        game->record_money_change(-game->economy.decadeEnterpriseTax);
    game->economy.totalSpent = game->economy.aiMoneySpent = game->economy.fuelCostAccrued = 0;
    game->economy.subsidyCost = game->economy.totalRevenue = game->economy.netProfit = 0;
    game->economy.decadeEnterpriseTax = 0;
    game->renderer.mapRedrawPending = true;
    game->compareToPrevious = true;
    game->monthRolloverFlag = true;
    if (game->economy.monthOfYear == calendar::YearEnd) {
        if (game->ui.annualReportEnabled) {
            game->ready = false;
            game->ui.transitionCode = menu_screen::AnnualReport;
            game->set_state(GameState::MenuOverlay);
        }
    } else if (game->ui.monthlyReportEnabled) {
        game->ready = false;
        game->ui.transitionCode = menu_screen::ReportComparison;
        game->set_state(GameState::MenuOverlay);
    }
}

void Game::tick_service_sites(bool player_action) {
    Game *game = this;
    for (int site_slot = 0; site_slot < kServiceVehicleSlotCount; ++site_slot) {
        if ((game->fleet.serviceSites[site_slot].flags & service_site::USED_SLOT) == 0) continue;
        const int site_x = unsigned_byte(game->fleet.serviceSites[site_slot].x);
        const int site_y = unsigned_byte(game->fleet.serviceSites[site_slot].y);
        int min_x = site_x - service_tick::kNeighborhoodRadius;
        int min_y = site_y - service_tick::kNeighborhoodRadius;
        int max_x = site_x + service_tick::kNeighborhoodRadius;
        int max_y = site_y + service_tick::kNeighborhoodRadius;
        if (min_x < 0) min_x = 0;
        if (min_y < 0) min_y = 0;
        if (max_x >= game->world.terrain.mapWidth) max_x = game->world.terrain.mapWidth - 1;
        if (max_y >= game->world.terrain.mapHeight) max_y = game->world.terrain.mapHeight - 1;
        int nearby_city_tile_count = 0;
        bool seen_zone_ids[kZoneCount] = {false};
        bool seen_industry_group1[kZoneCount] = {false};
        bool seen_industry_group2[kZoneCount] = {false};
        bool seen_industry_group3[kZoneCount] = {false};
        for (int tile_x = min_x; tile_x <= max_x; ++tile_x) {
            for (int tile_y = min_y; tile_y <= max_y; ++tile_y) {
                const int tile_code = game->world.terrain.tileMap[tile_x][tile_y];
                const int zone_or_city_id = game->world.trackMap[tile_x][tile_y] & track::OWNER_MASK;
                if (tile_code >= tile::BUILT_FIRST && tile_code <= tile::CITY_ZONE_LAST) {
                    if ((game->world.trackMap[tile_x][tile_y]
                         & track_overlay::MASK) == track_overlay::JUNCTION
                        && !player_action
                        && !seen_zone_ids[zone_or_city_id]) {
                        seen_zone_ids[zone_or_city_id] = true;
                        if ((game->fleet.serviceSites[site_slot].unloadFlags & service_resource::PASSENGER_FLAG) != 0) {
                            const int stock_increase = game->world.zonePopulation[zone_or_city_id]
                                      * (game->world.constructionTimers[zone_or_city_id] + 10
                                         + game->world.zonePopulation[zone_or_city_id] / 10
                                         + game->rng.next_int() % service_tick::kRandomGrowthRange)
                                      / service_tick::kPopulationGrowthDivisor;
                            game->fleet.serviceSites[site_slot].resourceStock[service_resource::Passengers] =
                                (int16_t)(game->fleet.serviceSites[site_slot].resourceStock[service_resource::Passengers] + stock_increase);
                        }
                        if ((game->fleet.serviceSites[site_slot].unloadFlags & service_resource::MAIL_FLAG) != 0) {
                            const int stock_increase = game->world.zonePopulation[zone_or_city_id] / 2
                                      * (game->world.constructionTimers[zone_or_city_id] + 10
                                         + game->world.zonePopulation[zone_or_city_id] / 10
                                         + std::abs(game->rng.next_int() % service_tick::kRandomGrowthRange))
                                      / service_tick::kPopulationGrowthDivisor;
                            game->fleet.serviceSites[site_slot].resourceStock[service_resource::Mail] =
                                (int16_t)(game->fleet.serviceSites[site_slot].resourceStock[service_resource::Mail] + stock_increase);
                        }
                        game->world.zonePopulation[zone_or_city_id] =
                            (int16_t)(game->world.zonePopulation[zone_or_city_id]
                                      + game->rng.next_int() % 2);
                    }
                    ++nearby_city_tile_count;
                }
                if (tile_code >= service_tick::kIndustryGroup1ProductionFirst
                    && tile_code <= service_tick::kIndustryGroup1ProductionLast && !player_action
                    && (game->world.trackMap[tile_x][tile_y]
                        & track_overlay::MASK) == track_overlay::STATION
                    && zone_or_city_id < kCitySlotCount
                    && !seen_industry_group1[zone_or_city_id]) {
                    seen_industry_group1[zone_or_city_id] = true;
                    const int zone_tile_count =
                        game->world.cities[zone_or_city_id].zoneTileCount > 0
                            ? game->world.cities[zone_or_city_id].zoneTileCount : 1;
                    int stock_increase = game->world.cities[zone_or_city_id].population
                                         * service_tick::kPopulationGrowthFactor
                                         / service_tick::kPopulationMonthDivisor
                                         / zone_tile_count;
                    if (stock_increase == 0) stock_increase = 1;
                    game->fleet.serviceSites[site_slot].resourceStock[service_resource::IndustryGroup1] =
                        (int16_t)(game->fleet.serviceSites[site_slot].resourceStock[service_resource::IndustryGroup1] + stock_increase);
                    game->world.cities[zone_or_city_id].stationRevenue =
                        (int16_t)(game->world.cities[zone_or_city_id].stationRevenue + stock_increase);
                    game->fleet.serviceSites[site_slot].loadFlags =
                        (int8_t)(game->fleet.serviceSites[site_slot].loadFlags | service_resource::INDUSTRY_GROUP1_FLAG);
                }
                if (tile_code >= service_tick::kIndustryGroup2ProductionFirst
                    && tile_code <= service_tick::kIndustryGroup2ProductionLast && !player_action
                    && (game->world.trackMap[tile_x][tile_y]
                        & track_overlay::MASK) == track_overlay::STATION
                    && zone_or_city_id < kCitySlotCount
                    && !seen_industry_group2[zone_or_city_id]) {
                    seen_industry_group2[zone_or_city_id] = true;
                    const int zone_tile_count =
                        game->world.cities[zone_or_city_id].zoneTileCount > 0
                            ? game->world.cities[zone_or_city_id].zoneTileCount : 1;
                    int stock_increase = game->world.cities[zone_or_city_id].population
                                         * service_tick::kPopulationGrowthFactor
                                         / service_tick::kPopulationMonthDivisor
                                         / zone_tile_count;
                    if (stock_increase == 0) stock_increase = 1;
                    game->fleet.serviceSites[site_slot].resourceStock[service_resource::IndustryGroup2] =
                        (int16_t)(game->fleet.serviceSites[site_slot].resourceStock[service_resource::IndustryGroup2] + stock_increase);
                    game->world.cities[zone_or_city_id].stationRevenue =
                        (int16_t)(game->world.cities[zone_or_city_id].stationRevenue + stock_increase);
                    game->fleet.serviceSites[site_slot].loadFlags =
                        (int8_t)(game->fleet.serviceSites[site_slot].loadFlags | service_resource::INDUSTRY_GROUP2_FLAG);
                }
                if (tile_code >= service_tick::kIndustryGroup3ProductionFirst
                    && tile_code <= service_tick::kIndustryGroup3ProductionLast && !player_action
                    && (game->world.trackMap[tile_x][tile_y]
                        & track_overlay::MASK) == track_overlay::STATION
                    && zone_or_city_id < kCitySlotCount
                    && !seen_industry_group3[zone_or_city_id]) {
                    seen_industry_group3[zone_or_city_id] = true;
                    const int zone_tile_count =
                        game->world.cities[zone_or_city_id].zoneTileCount > 0
                            ? game->world.cities[zone_or_city_id].zoneTileCount : 1;
                    int stock_increase = game->world.cities[zone_or_city_id].population
                                         * service_tick::kPopulationGrowthFactor
                                         / service_tick::kPopulationMonthDivisor
                                         / zone_tile_count;
                    if (stock_increase == 0) stock_increase = 1;
                    game->fleet.serviceSites[site_slot].resourceStock[service_resource::IndustryGroup3] =
                        (int16_t)(game->fleet.serviceSites[site_slot].resourceStock[service_resource::IndustryGroup3] + stock_increase);
                    game->world.cities[zone_or_city_id].stationRevenue =
                        (int16_t)(game->world.cities[zone_or_city_id].stationRevenue + stock_increase);
                    game->fleet.serviceSites[site_slot].loadFlags =
                        (int8_t)(game->fleet.serviceSites[site_slot].loadFlags | service_resource::INDUSTRY_GROUP3_FLAG);
                }
                if (tile_code >= service_tick::kIndustryGroup1DestinationFirst
                    && tile_code <= service_tick::kIndustryGroup1DestinationLast)
                    game->fleet.serviceSites[site_slot].unloadFlags =
                        (int8_t)(game->fleet.serviceSites[site_slot].unloadFlags | service_resource::INDUSTRY_GROUP1_FLAG);
                if (tile_code >= service_tick::kIndustryGroup2DestinationFirst
                    && tile_code <= service_tick::kIndustryGroup2DestinationLast)
                    game->fleet.serviceSites[site_slot].unloadFlags =
                        (int8_t)(game->fleet.serviceSites[site_slot].unloadFlags | service_resource::INDUSTRY_GROUP2_FLAG);
                if (tile_code >= service_tick::kIndustryGroup3DestinationFirst
                    && tile_code <= service_tick::kIndustryGroup3DestinationLast)
                    game->fleet.serviceSites[site_slot].unloadFlags =
                        (int8_t)(game->fleet.serviceSites[site_slot].unloadFlags | service_resource::INDUSTRY_GROUP3_FLAG);
            }
        }
        if (nearby_city_tile_count / service_tick::kPassengerSourceThreshold > 0) {
            game->fleet.serviceSites[site_slot].unloadFlags =
                (int8_t)(game->fleet.serviceSites[site_slot].unloadFlags | service_resource::PASSENGER_FLAG);
            game->fleet.serviceSites[site_slot].loadFlags =
                (int8_t)(game->fleet.serviceSites[site_slot].loadFlags | service_resource::PASSENGER_FLAG);
        }
        if (nearby_city_tile_count / service_tick::kMailSourceThreshold > 0) {
            game->fleet.serviceSites[site_slot].unloadFlags =
                (int8_t)(game->fleet.serviceSites[site_slot].unloadFlags | service_resource::MAIL_FLAG);
            game->fleet.serviceSites[site_slot].loadFlags =
                (int8_t)(game->fleet.serviceSites[site_slot].loadFlags | service_resource::MAIL_FLAG);
        }
        const bool is_extended_site = (game->fleet.serviceSites[site_slot].flags & service_site::EXTENDED) != 0;
        for (int resource_index = 0; resource_index < kResourceTypeCount; ++resource_index) {
            int limit;
            if (is_extended_site) {
                limit = resource_index == service_resource::Passengers
                            ? service_tick::kLargePassengerCapacity
                            : (resource_index == service_resource::Mail
                                   ? service_tick::kLargeMailCapacity
                                   : service_tick::kLargeIndustryCapacity);
            } else {
                limit = resource_index == service_resource::Passengers
                            ? service_tick::kSmallPassengerCapacity
                            : (resource_index == service_resource::Mail
                                   ? service_tick::kSmallMailCapacity
                                   : service_tick::kSmallIndustryCapacity);
            }
            if (game->fleet.serviceSites[site_slot].resourceStock[resource_index] > limit)
                game->fleet.serviceSites[site_slot].resourceStock[resource_index] = (int16_t)limit;
        }
    }
}

void Game::advance_tile_animation_frames() {
    Game *game = this;
    if (++game->tile50AnimationPhase == tile_animation::kFastPhasePeriod) {
        game->tile50AnimationPhase = 0;
        if (++game->tile50AnimationFrame == tile_animation::kFrameCount)
            game->tile50AnimationFrame = 0;
    }
    if (++game->tile64AnimationPhase == tile_animation::kFastPhasePeriod) {
        game->tile64AnimationPhase = 0;
        if (++game->tile64AnimationFrame == tile_animation::kFrameCount)
            game->tile64AnimationFrame = 0;
    }
    if (++game->tile55AnimationPhase == tile_animation::kSlowPhasePeriod) {
        game->tile55AnimationPhase = 0;
        if (++game->tile55AnimationFrame == tile_animation::kFrameCount)
            game->tile55AnimationFrame = 0;
    }
    if (++game->world.terrain.woodAgeTimer == tile_animation::kWoodAgePeriod) {
        for (int map_x = 0; map_x < game->world.terrain.mapWidth; ++map_x) {
            for (int map_y = 0; map_y < game->world.terrain.mapHeight; ++map_y) {
                if (game->world.terrain.tileMap[map_x][map_y]
                        < tile_animation::kWoodTileFirst
                    || game->world.terrain.tileMap[map_x][map_y]
                        > tile_animation::kWoodTileLast
                    || game->rng.next_int() % tile_animation::kWoodRandomDivisor != 0)
                    continue;
                ++game->world.terrain.tileMap[map_x][map_y];
                if (game->world.terrain.tileMap[map_x][map_y]
                    == tile_animation::kWoodTileCycleEnd)
                    game->world.terrain.tileMap[map_x][map_y] =
                        tile_animation::kWoodTileFirst;
            }
        }
        game->world.terrain.woodAgeTimer = 0;
    }
    if (++game->world.terrain.phaseTimer == tile_animation::kMapPhasePeriod)
        game->world.terrain.phaseTimer = 0;
}

void Game::apply_queued_tile_changes() {
    Game *game = this;
    for (int queue_slot = 0; queue_slot < kTileQueueCount; ++queue_slot) {
        TileChange& change = game->world.terrain.tileQueue[queue_slot];
        if (change.countdown <= 0) continue;
        --change.countdown;
        const int tile_x = unsigned_byte(change.x);
        const int tile_y = unsigned_byte(change.y);
        if (change.countdown == 0 && tile_x >= 0 && tile_y >= 0
            && tile_x < game->world.terrain.mapWidth && tile_y < game->world.terrain.mapHeight)
            game->world.terrain.tileMap[tile_x][tile_y] = static_cast<int8_t>(
                unsigned_byte(game->world.terrain.tileMap[tile_x][tile_y])
                - tile_change::kPendingBit);
        if (change.countdown != tile_change::kCommitCountdown || tile_x < 0 || tile_y < 0
            || tile_x >= game->world.terrain.mapWidth || tile_y >= game->world.terrain.mapHeight)
            continue;
        const int tile_code = unsigned_byte(
            game->world.terrain.tileMap[tile_x][tile_y])
            - tile_change::kPendingBit;
        if (game->world.terrain.tileQueueVisualOnly[queue_slot]) continue;
        if (tile_code >= tile::SCENERY_FIRST && tile_code <= tile::SCENERY_LAST)
            game->world.terrain.tileMap[tile_x][tile_y] = tile_change::kEmptyTileCode;
        // Player road tiles clear through the same queue (city streets never
        // reach it: demolish_at_cursor gates them out via road_demolishable).
        if (tile_code >= road_tile::kFirstShape && tile_code <= road_tile::kLastShape) {
            game->world.terrain.tileMap[tile_x][tile_y] = tile_change::kEmptyTileCode;
            game->renderer.lastViewportCenterX = -1;
        }
        if ((tile_code >= tile::TRACK_FIRST
             && tile_code <= tile_change::kTrackBeforeCrossingLast)
            || (tile_code >= tile::VERTICAL_STATION
                && tile_code <= tile::HORIZONTAL_TERMINAL)) {
            game->world.trackMap[tile_x][tile_y] = tile_change::kEmptyTrackCode;
            game->world.terrain.tileMap[tile_x][tile_y] = tile_change::kEmptyTileCode;
            game->renderer.lastViewportCenterX = -1;
        }
        if (tile_code >= tile_change::kTrackAfterCrossingFirst
            && tile_code <= tile_change::kTrackAfterCrossingLast) {
            game->world.terrain.tileMap[tile_x][tile_y] = static_cast<int8_t>(
                tile_change::kTrackRestoreOffset + tile_code);
            game->world.trackMap[tile_x][tile_y] = tile_change::kEmptyTrackCode;
            game->renderer.lastViewportCenterX = -1;
        }
        if (tile_code >= tile_change::kConstructionClearFirst
            && tile_code <= tile_change::kConstructionClearLast) {
            game->world.terrain.tileMap[tile_x][tile_y] = tile_change::kEmptyTileCode;
            game->world.trackMap[tile_x][tile_y] = tile_change::kEmptyTrackCode;
            game->renderer.lastViewportCenterX = -1;
        }
        if (tile_code >= tile_change::kConstructionRestoreFirst
            && tile_code <= tile_change::kConstructionRestoreLast) {
            game->world.terrain.tileMap[tile_x][tile_y] = static_cast<int8_t>(
                tile_change::kConstructionRestoreOffset + tile_code);
            game->world.trackMap[tile_x][tile_y] = tile_change::kEmptyTrackCode;
            game->renderer.lastViewportCenterX = -1;
        }
        if (tile_code == tile::CROSSING) {
            const int first_track_owner =
                game->world.trackMap[tile_x][tile_y] & track::DIR_OWNER_MASK;
            const int second_track_owner =
                (game->world.trackMap[tile_x][tile_y] & track::OWNER_MASK)
                >> track::DIR_OWNER_SHIFT;
            if (first_track_owner == 1 && second_track_owner == 1) {
                game->world.terrain.tileMap[tile_x][tile_y] = tile_change::kEmptyTileCode;
                game->world.trackMap[tile_x][tile_y] = tile_change::kEmptyTrackCode;
            } else if (first_track_owner == 1) {
                game->world.terrain.tileMap[tile_x][tile_y] =
                    tile_change::kCrossingReplacementCodeA;
                game->world.trackMap[tile_x][tile_y] = static_cast<int8_t>(
                    track::PRESENT | second_track_owner);
            } else if (second_track_owner == 1) {
                game->world.terrain.tileMap[tile_x][tile_y] =
                    tile_change::kCrossingReplacementCodeB;
                game->world.trackMap[tile_x][tile_y] = static_cast<int8_t>(
                    track::PRESENT | first_track_owner);
            }
            game->renderer.lastViewportCenterX = -1;
        }
    }
}

void Game::update_active_trains() {
    Game *game = this;
    for (int trainIndex = 0; trainIndex < kTrainSlotCount; ++trainIndex) {
        if (!train_flag::isUsed(game->fleet.trainCars[trainIndex].flags)
            || !train_flag::isHead(game->fleet.trainCars[trainIndex].flags))
            continue;
        if (train_flag::isRemovalPending(game->fleet.trainCars[trainIndex].flags)) {
            if (game->fleet.trainCars[trainIndex].energyCargoOrRemovalTicks > 0) {
                --game->fleet.trainCars[trainIndex].energyCargoOrRemovalTicks;
                continue;
            }
            game->fleet.trainCars[trainIndex].phase = 0;
            game->fleet.trainCars[trainIndex].flags = 0;
            continue;
        }
        game->update_train(trainIndex);
        game->update_train(trainIndex);
        if (game->fleet.trainCars[trainIndex].phase == 0)
            game->merge_train_at(trainIndex);
    }
}

void Game::advance_month_if_day_ended() {
    Game *game = this;
    switch (game->economy.monthOfYear) {
        case calendar::January: case calendar::March: case calendar::May:
        case calendar::July: case calendar::August: case calendar::October: case calendar::December:
            if (game->economy.dayOfMonth == calendar::kLongMonthRolloverDay) ++game->economy.monthOfYear;
            break;
        case calendar::February:
            if ((game->economy.year % 4 != 0 && game->economy.dayOfMonth == calendar::kFebruaryRolloverDay)
                || (game->economy.year % 4 == 0 && game->economy.dayOfMonth == calendar::kLeapFebruaryRolloverDay))
                ++game->economy.monthOfYear;
            break;
        case calendar::April: case calendar::June: case calendar::September: case calendar::November:
            if (game->economy.dayOfMonth == calendar::kShortMonthRolloverDay) ++game->economy.monthOfYear;
            break;
    }
}

void Game::apply_new_month_changes() {
    Game *game = this;
    game->economy.dayOfMonth = game_calendar::kFirstDay;
    game->new_year_notify();
    game->world.decay_construction_timers_quarterly(game->economy.monthOfYear);
    game->world.update_city_population(game->rng);
    game->upgrade_one_ai_train_for_era();
    switch (std::abs(game->rng.next_int() % monthly_event::kCount)) {
        case MoneyBaseIncrease:
            game->economy.moneyBasePrimary += monthly_event::kMoneyBaseStep;
            game->economy.moneyBaseSecondary += monthly_event::kMoneyBaseStep;
            break;
        case EarlyTrainPerformanceIncrease:
            ++game->economy.baseTrainSpeedEarly;
            game->economy.accelerationEarly += monthly_event::kEarlyTrainAccelerationStep;
            game->economy.brakingEarly += monthly_event::kEarlyTrainAccelerationStep;
            break;
        case LateTrainPerformanceIncrease:
            ++game->economy.baseTrainSpeedLate;
            game->economy.accelerationLate += monthly_event::kLateTrainAccelerationStep;
            break;
        case TrackCostIncrease:
            game->economy.powerCost += monthly_event::kPowerCostStep;
            game->economy.structureCost += monthly_event::kStructureCostStep;
            break;
        case OperatingCostIncrease:
            game->economy.overheadCost += monthly_event::kOverheadCostStep;
            game->economy.maintenanceCost += monthly_event::kMaintenanceCostStep;
            game->economy.crewCost += monthly_event::kCrewCostStep;
            break;
        case WagonCostIncrease:
            game->economy.trainCostFirstEra += monthly_event::kWagonCostStep1;
            game->economy.trainCostSecondEra += monthly_event::kWagonCostStep2;
            game->economy.trainCostThirdEra += monthly_event::kWagonCostStep3;
            game->economy.trainCostFourthEra += monthly_event::kWagonCostStep4;
            game->economy.trainCostFifthEra += monthly_event::kWagonCostStep5;
            game->economy.trainCostSixthEra += monthly_event::kWagonCostStep6;
            break;
        case CargoCostIncrease:
            game->economy.wagonOptionCostPassengerMail += monthly_event::kCargoCostStep1;
            game->economy.wagonOptionCostIndustryGroup1 += monthly_event::kCargoCostStep2;
            game->economy.wagonOptionCostIndustryGroup2 += monthly_event::kCargoCostStep3;
            game->economy.wagonOptionCostIndustryGroup3 += monthly_event::kCargoCostStep4;
            break;
    }
}

void Game::advance_year_if_needed() {
    Game *game = this;
    if (game->economy.monthOfYear == calendar::YearEnd) {
        game->fleet.age_one_year();
        game->reset_ai_depot_train_ages();
        game->economy.monthOfYear = calendar::January;
        ++game->economy.year;
        if (game->economy.money < 0) {
            game->ready = false;
            game->ui.transitionCode = menu_screen::AiScenarioSetup;
            game->set_state(GameState::MenuOverlay);
        }
    }
}

void Game::advance_era_if_needed() {
    Game *game = this;
    int previousEra = game->scenario.currentEra;
    if (game->economy.dayOfMonth == 5 && game->economy.monthOfYear == calendar::March
        && game->economy.year == 1951 && game->scenario.currentEra < 2)
        game->scenario.currentEra = 2;
    if (game->economy.dayOfMonth == 17 && game->economy.monthOfYear == calendar::July
        && game->economy.year == 1953 && game->scenario.currentEra < 3)
        game->scenario.currentEra = 3;
    if (game->economy.dayOfMonth == 4 && game->economy.monthOfYear == calendar::December
        && game->economy.year == 1957 && game->scenario.currentEra < 4)
        game->scenario.currentEra = 4;
    if (game->economy.dayOfMonth == 26 && game->economy.monthOfYear == calendar::February
        && game->economy.year == 1962 && game->scenario.currentEra < 5)
        game->scenario.currentEra = 5;
    if (previousEra != game->scenario.currentEra) {
        game->ready = false;
        game->ui.transitionCode = menu_screen::GenericOverlayI;
        game->set_state(GameState::MenuOverlay);
    }
}

void Game::advance_game_day() {
    Game *game = this;
    game->economy.compute_year_end_metrics();
    if (game->frameIndex > 0) --game->frameIndex;
    game->dayFrameCounter = 0;
    ++game->economy.dayOfMonth;
    game->monthly_offer_tick();
    if (game->economy.dayOfMonth % 4 == 0) game->maybe_take_ai_turn();
    if (game->economy.dayOfMonth % 2 == 0) {
        if (game->economy.dayOfMonth % 10 == 0)
            game->update_ten_day_metric();
        game->charge_train_running_costs();
        game->check_scenario_milestone();
        if (game->scenario.aiExpansionEnabled) {
            int streetGrew = 0;
            for (int attempt = 0; attempt < 30; ++attempt)
                streetGrew = streetGrew || game->world.grow_city_street(game->rng);
            if (streetGrew) {
                game->world.recount_zone_population();
                game->roadNet.rebuild_from_tiles(game->world.terrain);
                game->renderer.lastViewportCenterX = -1;
            }
        }
        game->tick_service_sites(false);
    }

    int previousMonth = game->economy.monthOfYear;
    game->advance_month_if_day_ended();
    if (previousMonth != game->economy.monthOfYear) game->apply_new_month_changes();
    game->advance_year_if_needed();
    game->advance_era_if_needed();
}

// Advance frame animation, queued map work, trains, and the calendar in that order.
void Game::simulation_tick() {
    Game *game = this;
    game->advance_tile_animation_frames();
    game->apply_queued_tile_changes();
    game->update_active_trains();
    game->update_road_vehicles();
    if (game->dayFrameCounter >= game->dayTickThreshold) {
        game->advance_game_day();
    } else {
        ++game->dayFrameCounter;
    }
}
