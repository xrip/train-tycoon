// Train movement, loading, and delivery.
#include "game.h"
#include "support/numeric.h"
#include <array>
#include <cstdlib>

namespace {

namespace train_service {
constexpr int32_t kPassengerCapacity = 25;
constexpr int32_t kIndustryGroup1Capacity = kPassengerCapacity;
constexpr int32_t kMailCapacity = 15;
constexpr int32_t kIndustryCapacity = 30;
constexpr int32_t kRefitThreshold = 10;
constexpr int32_t kNearbySiteRadius = 3;
}  // namespace train_service

namespace train_movement {
constexpr int32_t kPhaseStep = 8;
constexpr int32_t kServiceCheckPhase = 4;
constexpr int32_t kServiceLoadPhase = 3;
constexpr int32_t kCargoAgeScaleDivisor = 15;
constexpr int32_t kCargoAgeMaximum = 200;
constexpr int32_t kEnergyPerCar = 40;
constexpr int32_t kEnergyDecayPerUpdate = 3;
constexpr int32_t kBaseEngineEnergy = 8;
}  // namespace train_movement

namespace train_revenue {
constexpr int32_t kCargoAgeUnitValue = 3;
constexpr int32_t kMailUnitValue = 8;
constexpr int32_t kIndustryGroup1UnitValue = 10;
constexpr int32_t kIndustryGroup2UnitValue = 12;
constexpr int32_t kIndustryGroup3UnitValue = 13;
constexpr int32_t kScenarioBonusMultiplier = 2;
}  // namespace train_revenue

struct TrackVariantAlias {
    int sourceVariant;
    int normalizedVariant;
};

// These are legacy tile-shape aliases. Their exact visual names are not yet
// proven, so the table records only the normalization relation used by train
// movement.
constexpr std::array<TrackVariantAlias, 14> kTrackVariantAliases = {{
    {7, 0}, {9, 0}, {12, 0}, {13, 0}, {16, 0}, {17, 0}, {19, 0},
    {8, 1}, {10, 1}, {11, 1}, {14, 1}, {15, 1}, {18, 1}, {20, 1},
}};

}  // namespace

int32_t Fleet::accumulate_turns(const World& world, int32_t car) const {
    if (car == train_chain::kEnd) return 0;
    const int tile_code = world.terrain.tileMap[unsigned_byte(trainCars[car].x)]
                                               [unsigned_byte(trainCars[car].y)];
    int turn_offset = 0;
    if (tile_code == track_turn_tile::VariantA)
        turn_offset = trainCars[car].direction == 0 ? -1 : 1;
    if (tile_code == track_turn_tile::VariantC)
        turn_offset = trainCars[car].direction == 0 ? 1 : -1;
    if (tile_code == track_turn_tile::VariantB)
        turn_offset = trainCars[car].direction == 2 ? 1 : -1;
    if (tile_code == track_turn_tile::VariantD)
        turn_offset = trainCars[car].direction == 2 ? -1 : 1;
    return accumulate_turns(world, trainCars[car].nextCar) + turn_offset;
}

int32_t Fleet::find_service_site_at(int32_t x, int32_t y) const {
    for (int site_slot = 0; site_slot < kServiceVehicleSlotCount; ++site_slot) {
        if ((serviceSites[site_slot].flags & service_site::USED_SLOT) == 0) continue;
        const int site_x = unsigned_byte(serviceSites[site_slot].x);
        const int site_y = unsigned_byte(serviceSites[site_slot].y);
        if (site_x == x && site_y == y) return site_slot;
        if ((site_x == x - 1 || site_x == x + 1) && site_y == y
            && (serviceSites[site_slot].flags & (service_site::USED_SLOT | service_site::HORIZONTAL | service_site::EXTENDED))
                   == (service_site::USED_SLOT | service_site::HORIZONTAL | service_site::EXTENDED))
            return site_slot;
        if ((site_y == y - 1 || site_y == y + 1) && site_x == x
            && (serviceSites[site_slot].flags & (service_site::USED_SLOT | service_site::HORIZONTAL | service_site::EXTENDED))
                   == (service_site::USED_SLOT | service_site::EXTENDED))
            return site_slot;
    }
    return kNoFleetSlot;
}

int32_t Fleet::append_car(int32_t previous_car, int32_t car_type) {
    int new_car = kTrainSlotCount;
    for (int car_slot = 0; car_slot < kTrainSlotCount; ++car_slot) {
        if (!train_flag::isUsed(trainCars[car_slot].flags)) {
            new_car = car_slot;
            break;
        }
    }
    if (new_car != kTrainSlotCount) {
        trainCars[previous_car].nextCar = (int16_t)new_car;
        trainCars[new_car].flags = static_cast<int8_t>(train_flag::USED_SLOT | car_type);
        trainCars[new_car].nextCar = train_chain::kEnd;
        trainCars[new_car].x = trainCars[previous_car].x;
        trainCars[new_car].y = trainCars[previous_car].y;
        trainCars[new_car].direction = trainCars[previous_car].direction;
        trainCars[new_car].phase =
            (int8_t)(trainCars[previous_car].phase - train_movement::kPhaseStep);
        trainCars[new_car].energyCargoOrRemovalTicks = 0;
        trainCars[new_car].secondaryCargoOrBaseEnergy = 0;
        trainCars[new_car].lastServiceSite = -1;
    }
    return new_car;
}

void Fleet::reset_trailing_cars(int32_t head, int32_t car, int32_t phase) {
    if (car == train_chain::kEnd) return;
    reset_trailing_cars(head, trainCars[car].nextCar,
                        phase - train_movement::kPhaseStep);
    trainCars[car].x = trainCars[head].x;
    trainCars[car].y = trainCars[head].y;
    trainCars[car].direction = trainCars[head].direction;
    trainCars[car].phase = (int8_t)(phase - train_movement::kPhaseStep);
}

int32_t Fleet::take_service_resource(int32_t site, int32_t resource,
                                     int32_t wanted) {
    int taken = serviceSites[site].resourceStock[resource] > wanted
                ? wanted : serviceSites[site].resourceStock[resource];
    serviceSites[site].resourceStock[resource] =
        (int16_t)(serviceSites[site].resourceStock[resource] - taken);
    return taken;
}

int32_t Fleet::collect_nearby_service_resource(int32_t site, int32_t resource,
                                                int32_t wanted) {
    int total = take_service_resource(site, resource, wanted);
    for (int other_site = 0; other_site < kServiceVehicleSlotCount; ++other_site) {
        if (site == other_site || (serviceSites[other_site].flags & service_site::USED_SLOT) == 0
            || (serviceSites[other_site].flags & service_site::TYPE_MASK)
                   != (serviceSites[site].flags & service_site::TYPE_MASK)
            || std::abs(serviceSites[site].x - serviceSites[other_site].x) >= train_service::kNearbySiteRadius
            || std::abs(serviceSites[site].y - serviceSites[other_site].y) >= train_service::kNearbySiteRadius)
            continue;
        total += take_service_resource(other_site, resource, wanted - total);
    }
    return total;
}

bool Fleet::can_load_at_service_site(int32_t head, int32_t site) const {
    int car = trainCars[head].nextCar;
    if (car == train_chain::kEnd) return false;
    if (!train_flag::isHead(trainCars[car].flags)) {
        switch (train_flag::type(trainCars[car].flags)) {
            case wagon::PassengerMailCar:
                if ((serviceSites[site].loadFlags & service_resource::PASSENGER_FLAG) != 0
                    && trainCars[car].energyCargoOrRemovalTicks < train_service::kPassengerCapacity)
                    return true;
                if ((serviceSites[site].loadFlags & service_resource::MAIL_FLAG) != 0
                    && trainCars[car].secondaryCargoOrBaseEnergy < train_service::kMailCapacity)
                    return true;
                break;
            case wagon::IndustryGroup1Car:
                if ((serviceSites[site].loadFlags & service_resource::INDUSTRY_GROUP1_FLAG) != 0
                    && trainCars[car].energyCargoOrRemovalTicks < train_service::kIndustryGroup1Capacity)
                    return true;
                break;
            case wagon::IndustryGroup2CarA: case wagon::IndustryGroup2CarB:
                if ((serviceSites[site].loadFlags & service_resource::INDUSTRY_GROUP2_FLAG) != 0
                    && trainCars[car].energyCargoOrRemovalTicks < train_service::kIndustryCapacity)
                    return true;
                break;
            case wagon::IndustryGroup3CarA: case wagon::IndustryGroup3CarB:
                if ((serviceSites[site].loadFlags & service_resource::INDUSTRY_GROUP3_FLAG) != 0
                    && trainCars[car].energyCargoOrRemovalTicks < train_service::kIndustryCapacity)
                    return true;
                break;
        }
    }
    return can_load_at_service_site(car, site);
}

void Fleet::load_at_service_site(World& world, int32_t car,
                                 int32_t vehicle, bool construction_done) {
    if (car == train_chain::kEnd) return;
    if (!train_flag::isHead(trainCars[car].flags)) {
        const int car_type = train_flag::type(trainCars[car].flags);
        switch (car_type) {
            case wagon::PassengerMailCar:
                trainCars[car].energyCargoOrRemovalTicks =
                    (int8_t)(trainCars[car].energyCargoOrRemovalTicks
                             + collect_nearby_service_resource(
                                 vehicle, service_resource::Passengers,
                                 train_service::kPassengerCapacity
                                     - trainCars[car].energyCargoOrRemovalTicks));
                trainCars[car].secondaryCargoOrBaseEnergy =
                    (int8_t)(trainCars[car].secondaryCargoOrBaseEnergy
                             + collect_nearby_service_resource(
                                 vehicle, service_resource::Mail,
                                 train_service::kMailCapacity
                                     - trainCars[car].secondaryCargoOrBaseEnergy));
                trainCars[car].cargoAge = 0;
                if (vehicle != trainCars[car].lastServiceSite && !construction_done) {
                    world.advance_construction_timers(
                        unsigned_byte(trainCars[car].x),
                        unsigned_byte(trainCars[car].y), 4);
                    construction_done = true;
                }
                break;
            case wagon::IndustryGroup1Car:
                trainCars[car].energyCargoOrRemovalTicks =
                    (int8_t)(trainCars[car].energyCargoOrRemovalTicks
                             + collect_nearby_service_resource(
                                 vehicle, service_resource::IndustryGroup1,
                                 train_service::kIndustryGroup1Capacity
                                     - trainCars[car].energyCargoOrRemovalTicks));
                trainCars[car].cargoAge = 0;
                break;
            case wagon::IndustryGroup2CarA: case wagon::IndustryGroup2CarB:
                trainCars[car].energyCargoOrRemovalTicks =
                    (int8_t)(trainCars[car].energyCargoOrRemovalTicks
                             + collect_nearby_service_resource(
                                 vehicle, service_resource::IndustryGroup2,
                                 train_service::kIndustryCapacity
                                     - trainCars[car].energyCargoOrRemovalTicks));
                trainCars[car].cargoAge = 0;
                break;
            case wagon::IndustryGroup3CarA: case wagon::IndustryGroup3CarB:
                trainCars[car].energyCargoOrRemovalTicks =
                    (int8_t)(trainCars[car].energyCargoOrRemovalTicks
                             + collect_nearby_service_resource(
                                 vehicle, service_resource::IndustryGroup3,
                                 train_service::kIndustryCapacity
                                     - trainCars[car].energyCargoOrRemovalTicks));
                trainCars[car].cargoAge = 0;
                break;
        }
        if (car_type == wagon::IndustryGroup2CarA || car_type == wagon::IndustryGroup2CarB)
            trainCars[car].flags = (int8_t)((trainCars[car].flags & train_flag::NON_TYPE_MASK)
                                  | (trainCars[car].energyCargoOrRemovalTicks >= train_service::kRefitThreshold
                                         ? wagon::IndustryGroup2CarB : wagon::IndustryGroup2CarA));
        if (car_type == wagon::IndustryGroup3CarA || car_type == wagon::IndustryGroup3CarB)
            trainCars[car].flags = (int8_t)((trainCars[car].flags & train_flag::NON_TYPE_MASK)
                                  | (trainCars[car].energyCargoOrRemovalTicks >= train_service::kRefitThreshold
                                         ? wagon::IndustryGroup3CarB : wagon::IndustryGroup3CarA));
        trainCars[car].lastServiceSite = (int8_t)vehicle;
    } else {
        trainCars[car].cargoAge = 0;
    }
    load_at_service_site(world, trainCars[car].nextCar, vehicle,
                         construction_done);
}

void Game::record_delivered_resource(int32_t resource, int32_t amount) {
    Game* game = this;
    game->offerBoard.add_progress(offer_objective::Objective3 + resource, amount);
    switch (resource) {
        case delivery_progress_slot::IndustryGroup3: game->scenario.milestoneThreshold1 += amount; break;
        case delivery_progress_slot::IndustryGroup2: game->scenario.milestoneThreshold2 += amount; break;
        case delivery_progress_slot::IndustryGroup1: game->scenario.milestoneThreshold3 += amount; break;
        case delivery_progress_slot::Passengers: game->scenario.milestoneThreshold4 += amount; break;
        case delivery_progress_slot::Mail: game->scenario.milestoneThreshold5 += amount; break;
    }
}

void Game::record_train_revenue(int32_t amount) {
    Game* game = this;
    game->offerBoard.add_progress(offer_objective::Objective1, amount);
    game->economy.lifetimeTrainRevenue += amount;
    game->economy.trainRevenueScore += amount / 73;
}

UnloadResult Fleet::unload_at_service_site(const Scenario& scenario,
                                           int32_t car, int32_t vehicle) {
    if (car == train_chain::kEnd) return {};
    UnloadResult result;
    if (!train_flag::isHead(trainCars[car].flags)
        && trainCars[car].lastServiceSite != vehicle
        && (trainCars[car].energyCargoOrRemovalTicks > 0 || trainCars[car].secondaryCargoOrBaseEnergy > 0)) {
        const int age_scale = unsigned_byte(trainCars[car].cargoAge)
                            / train_movement::kCargoAgeScaleDivisor + 1;
        switch (train_flag::type(trainCars[car].flags)) {
            case wagon::PassengerMailCar:
                if ((serviceSites[vehicle].unloadFlags & service_resource::PASSENGER_FLAG) != 0) {
                    int revenue_value = trainCars[car].energyCargoOrRemovalTicks
                                      * train_revenue::kCargoAgeUnitValue * age_scale;
                    if (scenario.routeResourceFlags[delivery_progress_slot::Passengers] > 0)
                        revenue_value *= train_revenue::kScenarioBonusMultiplier;
                    result.revenue += revenue_value;
                    result.delivered[delivery_progress_slot::Passengers] +=
                        trainCars[car].energyCargoOrRemovalTicks;
                    trainCars[car].energyCargoOrRemovalTicks = 0;
                }
                if ((serviceSites[vehicle].unloadFlags & service_resource::MAIL_FLAG) != 0) {
                    int revenue_value = trainCars[car].secondaryCargoOrBaseEnergy
                                      * train_revenue::kMailUnitValue * age_scale;
                    if (scenario.routeResourceFlags[delivery_progress_slot::Mail] > 0)
                        revenue_value *= train_revenue::kScenarioBonusMultiplier;
                    result.revenue += revenue_value;
                    result.delivered[delivery_progress_slot::Mail] +=
                        trainCars[car].secondaryCargoOrBaseEnergy;
                    trainCars[car].secondaryCargoOrBaseEnergy = 0;
                }
                break;
            case wagon::IndustryGroup1Car:
                if ((serviceSites[vehicle].unloadFlags & service_resource::INDUSTRY_GROUP3_FLAG) != 0) {
                    int revenue_value = trainCars[car].energyCargoOrRemovalTicks
                                      * train_revenue::kIndustryGroup1UnitValue * age_scale;
                    if (scenario.routeResourceFlags[delivery_progress_slot::IndustryGroup1] > 0)
                        revenue_value *= train_revenue::kScenarioBonusMultiplier;
                    result.revenue += revenue_value;
                    result.delivered[delivery_progress_slot::IndustryGroup1] +=
                        trainCars[car].energyCargoOrRemovalTicks;
                    trainCars[car].energyCargoOrRemovalTicks = 0;
                }
                break;
            case wagon::IndustryGroup2CarA: case wagon::IndustryGroup2CarB:
                if ((serviceSites[vehicle].unloadFlags & service_resource::INDUSTRY_GROUP1_FLAG) != 0) {
                    int revenue_value = trainCars[car].energyCargoOrRemovalTicks
                                      * train_revenue::kIndustryGroup2UnitValue * age_scale;
                    if (scenario.routeResourceFlags[delivery_progress_slot::IndustryGroup2] > 0)
                        revenue_value *= train_revenue::kScenarioBonusMultiplier;
                    result.revenue += revenue_value;
                    result.delivered[delivery_progress_slot::IndustryGroup2] +=
                        trainCars[car].energyCargoOrRemovalTicks;
                    trainCars[car].energyCargoOrRemovalTicks = 0;
                }
                break;
            case wagon::IndustryGroup3CarA: case wagon::IndustryGroup3CarB:
                if ((serviceSites[vehicle].unloadFlags & service_resource::INDUSTRY_GROUP2_FLAG) != 0) {
                    int revenue_value = trainCars[car].energyCargoOrRemovalTicks
                                      * train_revenue::kIndustryGroup3UnitValue * age_scale;
                    if (scenario.routeResourceFlags[delivery_progress_slot::IndustryGroup3] > 0)
                        revenue_value *= train_revenue::kScenarioBonusMultiplier;
                    result.revenue += revenue_value;
                    result.delivered[delivery_progress_slot::IndustryGroup3] +=
                        trainCars[car].energyCargoOrRemovalTicks;
                    trainCars[car].energyCargoOrRemovalTicks = 0;
                }
                break;
        }
    }
    UnloadResult tail = unload_at_service_site(scenario, trainCars[car].nextCar,
                                               vehicle);
    tail.revenue += result.revenue;
    for (int resource = delivery_progress_slot::First;
         resource < delivery_progress_slot::AfterLast; ++resource)
        tail.delivered[resource] += result.delivered[resource];
    return tail;
}

int32_t Game::unload_train_revenue(int32_t car, int32_t service_site) {
    UnloadResult result = fleet.unload_at_service_site(
        scenario, car, service_site);
    for (int resource = delivery_progress_slot::First;
         resource < delivery_progress_slot::AfterLast; ++resource) {
        if (result.delivered[resource] != 0)
            record_delivered_resource(resource, result.delivered[resource]);
    }
    return result.revenue;
}

static int normalize_track_variant(int variant) {
    for (const TrackVariantAlias& alias : kTrackVariantAliases)
        if (alias.sourceVariant == variant) return alias.normalizedVariant;
    return variant;
}

void Game::update_train(int32_t head) {
    Game *game = this;
    bool stop_chain_update = false;
    if (!train_flag::isHead(game->fleet.trainCars[head].flags)
        || unsigned_byte(game->fleet.trainCars[head].energyCargoOrRemovalTicks) <= 0) {
        game->fleet.trainCars[head].flags = static_cast<int8_t>(
            game->fleet.trainCars[head].flags & train_flag::CLEAR_MOVEMENT_STATE_MASK);
        if (train_flag::isHead(game->fleet.trainCars[head].flags)) {
            game->fleet.trainCars[head].energyCargoOrRemovalTicks =
                (int8_t)(game->fleet.trainCars[head].secondaryCargoOrBaseEnergy
                         + game->fleet.accumulate_turns(game->world, head) / 2
                         + game->fleet.count_cars(head) / 2
                         + game->fleet.trainCars[head].ageYears / 3);
        }

        int old_x = unsigned_byte(game->fleet.trainCars[head].x);
        int old_y = unsigned_byte(game->fleet.trainCars[head].y);
        const bool at_service_tile =
            (game->world.terrain.tileMap[old_x][old_y] == tile::VERTICAL_STATION
             || game->world.terrain.tileMap[old_x][old_y] == tile::HORIZONTAL_STATION);
        if (train_flag::isHead(game->fleet.trainCars[head].flags)
            && game->fleet.trainCars[head].phase == train_movement::kServiceCheckPhase
            && at_service_tile
            && game->world.trackMap[old_x][old_y] == tile::SERVICE_TRACK) {
            int service_vehicle = game->fleet.find_service_site_at(old_x, old_y);
            int revenue = 0;
            if (service_vehicle != game->fleet.trainCars[head].lastServiceSite)
                revenue = game->unload_train_revenue(head, service_vehicle);

            if (game->fleet.trainCars[head].stateCode != train_state_code::EmptyWagonCode) {
                int engine_type = game->fleet.trainCars[head].stateCode
                                & train_state_code::PackedFieldMask;
                game->fleet.trainCars[head].flags = static_cast<int8_t>(
                    train_flag::USED_SLOT | train_flag::HEAD | (engine_type >= 3 ? 1 : 0));
                game->fleet.trainCars[head].secondaryCargoOrBaseEnergy =
                    (int8_t)(train_movement::kBaseEngineEnergy - engine_type);
                game->fleet.trainCars[head].era = (int8_t)engine_type;
                game->fleet.trainCars[head].ageYears = 0;
                int car_slot = game->fleet.trainCars[head].nextCar;
                while (car_slot != train_chain::kEnd) {
                    game->fleet.trainCars[car_slot].flags = 0;
                    car_slot = game->fleet.trainCars[car_slot].nextCar;
                }
                int previous_car = head;
                for (int slot = 0; slot < kWagonOptionCount; ++slot) {
                    const int car_code =
                        (game->fleet.trainCars[head].stateCode
                         >> (train_state_code::FirstWagonFieldShift
                             + train_state_code::PackedFieldWidth * slot))
                        & train_state_code::PackedFieldMask;
                    if (car_code >= train_state_code::EmptyWagonCode) continue;
                    int car_type = train_state_code::WagonTypeBase + car_code;
                    if (car_type == train_state_code::SkippedCarType) ++car_type;
                    previous_car = game->fleet.append_car(previous_car, car_type);
                    game->renderer.lastViewportCenterX = -1;
                }
                game->fleet.trainCars[head].stateCode = train_state_code::EmptyWagonCode;
                game->renderer.lastViewportCenterX = -1;
            }

            game->fleet.load_at_service_site(game->world, head,
                                             service_vehicle, false);
            if (game->fleet.trainCars[head].aiOverride == 1
                && game->fleet.can_load_at_service_site(head, service_vehicle)) {
                game->fleet.trainCars[head].phase = train_movement::kServiceLoadPhase;
                stop_chain_update = true;
            }
            if (service_vehicle != game->fleet.trainCars[head].lastServiceSite) {
                if (revenue > 0) {
                    game->economy.money += revenue;
                    game->record_money_change(revenue);
                    game->economy.totalRevenue += revenue;
                    game->fleet.trainCars[head].revenue += revenue;
                    game->record_train_revenue(revenue);
                }
                game->fleet.trainCars[head].energyCargoOrRemovalTicks =
                    (int8_t)(game->fleet.count_cars(head) * train_movement::kEnergyPerCar);
            }
            game->fleet.trainCars[head].lastServiceSite = (int8_t)service_vehicle;
            game->fleet.trainCars[head].flags = static_cast<int8_t>(
                game->fleet.trainCars[head].flags | train_flag::MOVEMENT_STATE);
        }
        if (train_flag::isHead(game->fleet.trainCars[head].flags)
            && game->fleet.trainCars[head].phase == train_movement::kServiceCheckPhase
            && at_service_tile
            && game->world.trackMap[old_x][old_y] != tile::SERVICE_TRACK) {
            game->fleet.trainCars[head].energyCargoOrRemovalTicks =
                (int8_t)(game->fleet.count_cars(head) * train_movement::kEnergyPerCar);
        }

        bool collision_detected = false;
        if (game->fleet.trainCars[head].phase < train_movement::kPhaseStep) {
            ++game->fleet.trainCars[head].phase;
        } else {
            if (unsigned_byte(game->fleet.trainCars[head].cargoAge)
                < train_movement::kCargoAgeMaximum)
                ++game->fleet.trainCars[head].cargoAge;
            old_x = game->fleet.trainCars[head].x;
            old_y = game->fleet.trainCars[head].y;
            int old_dir = game->fleet.trainCars[head].direction;
            game->fleet.trainCars[head].phase = 0;

            int next_x = unsigned_byte(game->fleet.trainCars[head].x);
            int next_y = unsigned_byte(game->fleet.trainCars[head].y);
            int current_track = game->world.trackMap[next_x][next_y];
            int current_variant = game->world.terrain.tileMap[next_x][next_y]
                                 - tile::TRACK_FIRST;
            if (current_variant == tile::CROSSING_VARIANT
                && (game->fleet.trainCars[head].direction == 0
                                         || game->fleet.trainCars[head].direction == 1)) {
                current_track = track::PRESENT | (current_track & track::DIR_OWNER_MASK);
                current_variant = 0;
            }
            if (current_variant == tile::CROSSING_VARIANT
                && (game->fleet.trainCars[head].direction == 2
                                         || game->fleet.trainCars[head].direction == 3)) {
                current_track = track::PRESENT
                    | ((current_track & (track::DIR_OWNER_MASK << track::DIR_OWNER_SHIFT))
                       >> track::DIR_OWNER_SHIFT);
                current_variant = 1;
            }
            current_variant = normalize_track_variant(current_variant);
            const TrackStepEntry& step =
                game->fleet.forwardTrackSteps[current_variant]
                    .enteringDirection == game->fleet.trainCars[head].direction
                ? game->fleet.forwardTrackSteps[current_variant]
                : game->fleet.reverseTrackSteps[current_variant];
            next_x += step.stepX;
            next_y += step.stepY;
            if (next_x >= 0 && next_y >= 0
                && next_x < game->world.terrain.mapWidth && next_y < game->world.terrain.mapHeight) {
                bool moved = false;
                int next_track = game->world.trackMap[next_x][next_y];
                int next_variant = normalize_track_variant(
                    game->world.terrain.tileMap[next_x][next_y] - tile::TRACK_FIRST);
                for (int transition_index = 0;
                     transition_index < kTrackTransitionCount;
                     ++transition_index) {
                    const TrackStepTransition& transition =
                        step.transitions[transition_index];
                    if (transition.nextTileVariant != next_variant) continue;
                    const int next_dir = transition.nextDirection;
                    if (next_variant == tile::CROSSING_VARIANT) {
                        if (next_dir == 0 || next_dir == 1)
                            next_track = track::PRESENT | (next_track & track::DIR_OWNER_MASK);
                        if (next_dir == 2 || next_dir == 3)
                            next_track = track::PRESENT
                                | ((next_track & (track::DIR_OWNER_MASK << track::DIR_OWNER_SHIFT))
                                   >> track::DIR_OWNER_SHIFT);
                    }
                    if (current_track != next_track) break;
                    game->fleet.trainCars[head].x = (int8_t)next_x;
                    game->fleet.trainCars[head].y = (int8_t)next_y;
                    game->fleet.trainCars[head].direction = (int8_t)next_dir;
                    moved = true;
                }
                if (!moved) {
                    game->fleet.trainCars[head].direction = step.fallbackDirection;
                    game->fleet.reset_trailing_cars(
                        head, game->fleet.trainCars[head].nextCar, 0);
                }
                if (moved && train_flag::isHead(game->fleet.trainCars[head].flags)
                    && next_variant == tile::CROSSING_VARIANT) {
                    bool occupied = false;
                    for (int other = 0; other < kTrainSlotCount; ++other) {
                        if (train_flag::isUsed(game->fleet.trainCars[other].flags) && other != head
                            && game->fleet.trainCars[other].x == game->fleet.trainCars[head].x
                            && game->fleet.trainCars[other].y == game->fleet.trainCars[head].y) {
                            occupied = true;
                            break;
                        }
                    }
                    if (occupied) {
                        game->fleet.trainCars[head].x = (int8_t)old_x;
                        game->fleet.trainCars[head].y = (int8_t)old_y;
                        game->fleet.trainCars[head].direction = (int8_t)old_dir;
                        game->fleet.trainCars[head].phase = train_movement::kPhaseStep;
                        collision_detected = true;
                    }
                }
            } else {
                if (game->fleet.forwardTrackSteps[current_variant]
                        .enteringDirection
                    == game->fleet.trainCars[head].direction)
                    game->fleet.trainCars[head].direction =
                        game->fleet.forwardTrackSteps[current_variant]
                            .fallbackDirection;
                else if (game->fleet.reverseTrackSteps[current_variant]
                             .enteringDirection
                         == game->fleet.trainCars[head].direction)
                    game->fleet.trainCars[head].direction =
                        game->fleet.reverseTrackSteps[current_variant]
                            .fallbackDirection;
                game->fleet.reset_trailing_cars(
                    head, game->fleet.trainCars[head].nextCar, 0);
            }
        }
        if (!stop_chain_update && !collision_detected
            && game->fleet.trainCars[head].nextCar != train_chain::kEnd)
            game->update_train(game->fleet.trainCars[head].nextCar);
    }
    if (train_flag::isHead(game->fleet.trainCars[head].flags)) {
        int energy = unsigned_byte(game->fleet.trainCars[head].energyCargoOrRemovalTicks)
                   - train_movement::kEnergyDecayPerUpdate;
        if (energy < 0) energy = 0;
        game->fleet.trainCars[head].energyCargoOrRemovalTicks = (int8_t)energy;
    }
}
