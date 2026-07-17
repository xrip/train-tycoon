// Train movement, loading, and delivery.
#include "game_internal.h"
#include <cstdlib>

int32_t Fleet::accumulate_turns(const World& world, int32_t car) const {
    if (car == -1) return 0;
    int tile = world.terrain.tileMap[unsignedByte(trainCars[car].x)]
                                    [unsignedByte(trainCars[car].y)];
    int turn = 0;
    if (tile == 77) turn = trainCars[car].direction == 0 ? -1 : 1;
    if (tile == 79) turn = trainCars[car].direction == 0 ? 1 : -1;
    if (tile == 78) turn = trainCars[car].direction == 2 ? 1 : -1;
    if (tile == 80) turn = trainCars[car].direction == 2 ? -1 : 1;
    return accumulate_turns(world, trainCars[car].nextCar) + turn;
}

int32_t Fleet::find_service_site_at(int32_t x, int32_t y) const {
    for (int vehicle = 0; vehicle < 250; ++vehicle) {
        if ((serviceSites[vehicle].flags & 0x80) != 128) continue;
        int vehicle_x = unsignedByte(serviceSites[vehicle].x);
        int vehicle_y = unsignedByte(serviceSites[vehicle].y);
        if (vehicle_x == x && vehicle_y == y) return vehicle;
        if ((vehicle_x == x - 1 || vehicle_x == x + 1) && vehicle_y == y
            && (serviceSites[vehicle].flags & 0xE0) == 224)
            return vehicle;
        if ((vehicle_y == y - 1 || vehicle_y == y + 1) && vehicle_x == x
            && (serviceSites[vehicle].flags & 0xE0) == 160)
            return vehicle;
    }
    return 5000;
}

int32_t Fleet::append_car(int32_t previous_car, int32_t car_type) {
    int new_car = 500;
    for (int car = 0; car < 500; ++car) {
        if (!train_flag::isUsed(trainCars[car].flags)) {
            new_car = car;
            break;
        }
    }
    if (new_car != 500) {
        trainCars[previous_car].nextCar = (int16_t)new_car;
        trainCars[new_car].flags = static_cast<int8_t>(train_flag::USED_SLOT | car_type);
        trainCars[new_car].nextCar = -1;
        trainCars[new_car].x = trainCars[previous_car].x;
        trainCars[new_car].y = trainCars[previous_car].y;
        trainCars[new_car].direction = trainCars[previous_car].direction;
        trainCars[new_car].phase = (int8_t)(trainCars[previous_car].phase - 8);
        trainCars[new_car].energyCargoOrRemovalTicks = 0;
        trainCars[new_car].secondaryCargoOrBaseEnergy = 0;
        trainCars[new_car].lastServiceSite = -1;
    }
    return new_car;
}

void Fleet::reset_trailing_cars(int32_t head, int32_t car, int32_t phase) {
    if (car == -1) return;
    reset_trailing_cars(head, trainCars[car].nextCar, phase - 8);
    trainCars[car].x = trainCars[head].x;
    trainCars[car].y = trainCars[head].y;
    trainCars[car].direction = trainCars[head].direction;
    trainCars[car].phase = (int8_t)(phase - 8);
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
    for (int other = 0; other < 250; ++other) {
        if (site == other || (serviceSites[other].flags & 0x80) != 128
            || (serviceSites[other].flags & 7) != (serviceSites[site].flags & 7)
            || std::abs(serviceSites[site].x - serviceSites[other].x) >= 3
            || std::abs(serviceSites[site].y - serviceSites[other].y) >= 3)
            continue;
        total += take_service_resource(other, resource, wanted - total);
    }
    return total;
}

bool Fleet::can_load_at_service_site(int32_t head, int32_t site) const {
    int car = trainCars[head].nextCar;
    if (car == -1) return false;
    if (!train_flag::isHead(trainCars[car].flags)) {
        switch (train_flag::type(trainCars[car].flags)) {
            case 2:
                if ((serviceSites[site].loadFlags & 1) == 1
                    && trainCars[car].energyCargoOrRemovalTicks < 25)
                    return true;
                if ((serviceSites[site].loadFlags & 0x10) == 0x10
                    && trainCars[car].secondaryCargoOrBaseEnergy < 15)
                    return true;
                break;
            case 3:
                if ((serviceSites[site].loadFlags & 4) == 4
                    && trainCars[car].energyCargoOrRemovalTicks < 25)
                    return true;
                break;
            case 4: case 5:
                if ((serviceSites[site].loadFlags & 8) == 8
                    && trainCars[car].energyCargoOrRemovalTicks < 30)
                    return true;
                break;
            case 6: case 7:
                if ((serviceSites[site].loadFlags & 2) == 2
                    && trainCars[car].energyCargoOrRemovalTicks < 30)
                    return true;
                break;
        }
    }
    return can_load_at_service_site(car, site);
}

void game_loadTrainAtServiceVehicle(Game *game, int32_t car,
                                  int32_t vehicle, bool construction_done) {
    if (car == -1) return;
    if (!train_flag::isHead(game->fleet.trainCars[car].flags)) {
        int car_type = train_flag::type(game->fleet.trainCars[car].flags);
        switch (car_type) {
            case 2:
                game->fleet.trainCars[car].energyCargoOrRemovalTicks =
                    (int8_t)(game->fleet.trainCars[car].energyCargoOrRemovalTicks
                             + game->fleet.collect_nearby_service_resource(
                                 vehicle, 0, 25 - game->fleet.trainCars[car].energyCargoOrRemovalTicks));
                game->fleet.trainCars[car].secondaryCargoOrBaseEnergy =
                    (int8_t)(game->fleet.trainCars[car].secondaryCargoOrBaseEnergy
                             + game->fleet.collect_nearby_service_resource(
                                 vehicle, 4, 15 - game->fleet.trainCars[car].secondaryCargoOrBaseEnergy));
                game->fleet.trainCars[car].cargoAge = 0;
                if (vehicle != game->fleet.trainCars[car].lastServiceSite && !construction_done) {
                    game->world.advance_construction_timers(
                        unsignedByte(game->fleet.trainCars[car].x),
                        unsignedByte(game->fleet.trainCars[car].y), 4);
                    construction_done = true;
                }
                break;
            case 3:
                game->fleet.trainCars[car].energyCargoOrRemovalTicks =
                    (int8_t)(game->fleet.trainCars[car].energyCargoOrRemovalTicks
                             + game->fleet.collect_nearby_service_resource(
                                 vehicle, 1, 25 - game->fleet.trainCars[car].energyCargoOrRemovalTicks));
                game->fleet.trainCars[car].cargoAge = 0;
                break;
            case 4: case 5:
                game->fleet.trainCars[car].energyCargoOrRemovalTicks =
                    (int8_t)(game->fleet.trainCars[car].energyCargoOrRemovalTicks
                             + game->fleet.collect_nearby_service_resource(
                                 vehicle, 2, 30 - game->fleet.trainCars[car].energyCargoOrRemovalTicks));
                game->fleet.trainCars[car].cargoAge = 0;
                break;
            case 6: case 7:
                game->fleet.trainCars[car].energyCargoOrRemovalTicks =
                    (int8_t)(game->fleet.trainCars[car].energyCargoOrRemovalTicks
                             + game->fleet.collect_nearby_service_resource(
                                 vehicle, 3, 30 - game->fleet.trainCars[car].energyCargoOrRemovalTicks));
                game->fleet.trainCars[car].cargoAge = 0;
                break;
        }
        if (car_type == 4 || car_type == 5)
            game->fleet.trainCars[car].flags = (int8_t)((game->fleet.trainCars[car].flags & 0xF0)
                                  | (game->fleet.trainCars[car].energyCargoOrRemovalTicks >= 10 ? 5 : 4));
        if (car_type == 6 || car_type == 7)
            game->fleet.trainCars[car].flags = (int8_t)((game->fleet.trainCars[car].flags & 0xF0)
                                  | (game->fleet.trainCars[car].energyCargoOrRemovalTicks >= 10 ? 7 : 6));
        game->fleet.trainCars[car].lastServiceSite = (int8_t)vehicle;
    } else {
        game->fleet.trainCars[car].cargoAge = 0;
    }
    game_loadTrainAtServiceVehicle(game, game->fleet.trainCars[car].nextCar, vehicle, construction_done);
}

void Game::record_delivered_resource(int32_t resource, int32_t amount) {
    Game* game = this;
    game->offerBoard.add_progress(3 + resource, amount);
    switch (resource) {
        case 1: game->scenario.milestoneThreshold1 += amount; break;
        case 2: game->scenario.milestoneThreshold2 += amount; break;
        case 3: game->scenario.milestoneThreshold3 += amount; break;
        case 4: game->scenario.milestoneThreshold4 += amount; break;
        case 5: game->scenario.milestoneThreshold5 += amount; break;
    }
}

void Game::record_train_revenue(int32_t amount) {
    Game* game = this;
    game->offerBoard.add_progress(1, amount);
    game->economy.lifetimeTrainRevenue += amount;
    game->economy.trainRevenueScore += amount / 73;
}

int32_t game_unloadTrainRevenue(Game *game, int32_t car, int32_t vehicle) {
    if (car == -1) return 0;
    int revenue = 0;
    if (!train_flag::isHead(game->fleet.trainCars[car].flags)
        && game->fleet.trainCars[car].lastServiceSite != vehicle
        && (game->fleet.trainCars[car].energyCargoOrRemovalTicks > 0 || game->fleet.trainCars[car].secondaryCargoOrBaseEnergy > 0)) {
        int age_scale = unsignedByte(game->fleet.trainCars[car].cargoAge) / 15 + 1;
        switch (train_flag::type(game->fleet.trainCars[car].flags)) {
            case 2:
                if ((game->fleet.serviceSites[vehicle].unloadFlags & 1) == 1) {
                    int value = game->fleet.trainCars[car].energyCargoOrRemovalTicks * 3 * age_scale;
                    if (game->scenario.routeResourceFlags[4] > 0) value *= 2;
                    revenue += value;
                    game->record_delivered_resource(
                        4, game->fleet.trainCars[car].energyCargoOrRemovalTicks);
                    game->fleet.trainCars[car].energyCargoOrRemovalTicks = 0;
                }
                if ((game->fleet.serviceSites[vehicle].unloadFlags & 0x10) == 0x10) {
                    int value = game->fleet.trainCars[car].secondaryCargoOrBaseEnergy * 8 * age_scale;
                    if (game->scenario.routeResourceFlags[5] > 0) value *= 2;
                    revenue += value;
                    game->record_delivered_resource(
                        5, game->fleet.trainCars[car].secondaryCargoOrBaseEnergy);
                    game->fleet.trainCars[car].secondaryCargoOrBaseEnergy = 0;
                }
                break;
            case 3:
                if ((game->fleet.serviceSites[vehicle].unloadFlags & 2) == 2) {
                    int value = game->fleet.trainCars[car].energyCargoOrRemovalTicks * 10 * age_scale;
                    if (game->scenario.routeResourceFlags[3] > 0) value *= 2;
                    revenue += value;
                    game->record_delivered_resource(
                        3, game->fleet.trainCars[car].energyCargoOrRemovalTicks);
                    game->fleet.trainCars[car].energyCargoOrRemovalTicks = 0;
                }
                break;
            case 4: case 5:
                if ((game->fleet.serviceSites[vehicle].unloadFlags & 4) == 4) {
                    int value = game->fleet.trainCars[car].energyCargoOrRemovalTicks * 12 * age_scale;
                    if (game->scenario.routeResourceFlags[2] > 0) value *= 2;
                    revenue += value;
                    game->record_delivered_resource(
                        2, game->fleet.trainCars[car].energyCargoOrRemovalTicks);
                    game->fleet.trainCars[car].energyCargoOrRemovalTicks = 0;
                }
                break;
            case 6: case 7:
                if ((game->fleet.serviceSites[vehicle].unloadFlags & 8) == 8) {
                    int value = game->fleet.trainCars[car].energyCargoOrRemovalTicks * 13 * age_scale;
                    if (game->scenario.routeResourceFlags[1] > 0) value *= 2;
                    revenue += value;
                    game->record_delivered_resource(
                        1, game->fleet.trainCars[car].energyCargoOrRemovalTicks);
                    game->fleet.trainCars[car].energyCargoOrRemovalTicks = 0;
                }
                break;
        }
    }
    return game_unloadTrainRevenue(game, game->fleet.trainCars[car].nextCar, vehicle) + revenue;
}

static int normalize_track_variant(int variant) {
    switch (variant) {
        case 7: case 9: case 12: case 13: case 16: case 17: case 19:
            return 0;
        case 8: case 10: case 11: case 14: case 15: case 18: case 20:
            return 1;
        default:
            return variant;
    }
}

void game_updateTrain(Game *game, int32_t head) {
    int stop_chain = 0;
    if (!train_flag::isHead(game->fleet.trainCars[head].flags)
        || unsignedByte(game->fleet.trainCars[head].energyCargoOrRemovalTicks) <= 0) {
        game->fleet.trainCars[head].flags = (int8_t)(game->fleet.trainCars[head].flags & 0xEF);
        if (train_flag::isHead(game->fleet.trainCars[head].flags)) {
            game->fleet.trainCars[head].energyCargoOrRemovalTicks =
                (int8_t)(game->fleet.trainCars[head].secondaryCargoOrBaseEnergy
                         + game->fleet.accumulate_turns(game->world, head) / 2
                         + game->fleet.count_cars(head) / 2
                         + game->fleet.trainCars[head].ageYears / 3);
        }

        int old_x = unsignedByte(game->fleet.trainCars[head].x);
        int old_y = unsignedByte(game->fleet.trainCars[head].y);
        bool at_service_tile = (game->world.terrain.tileMap[old_x][old_y] == 87
                                || game->world.terrain.tileMap[old_x][old_y] == 88);
        if (train_flag::isHead(game->fleet.trainCars[head].flags) && game->fleet.trainCars[head].phase == 4
            && at_service_tile && game->world.trackMap[old_x][old_y] == 65) {
            int service_vehicle = game->fleet.find_service_site_at(old_x, old_y);
            int revenue = 0;
            if (service_vehicle != game->fleet.trainCars[head].lastServiceSite)
                revenue = game_unloadTrainRevenue(game, head, service_vehicle);

            if (game->fleet.trainCars[head].stateCode != 7) {
                int engine_type = game->fleet.trainCars[head].stateCode & 7;
                game->fleet.trainCars[head].flags = static_cast<int8_t>(
                    train_flag::USED_SLOT | train_flag::HEAD | (engine_type >= 3 ? 1 : 0));
                game->fleet.trainCars[head].secondaryCargoOrBaseEnergy = (int8_t)(8 - engine_type);
                game->fleet.trainCars[head].era = (int8_t)engine_type;
                game->fleet.trainCars[head].ageYears = 0;
                int car = game->fleet.trainCars[head].nextCar;
                while (car != -1) {
                    game->fleet.trainCars[car].flags = 0;
                    car = game->fleet.trainCars[car].nextCar;
                }
                int previous_car = head;
                for (int slot = 0; slot < 5; ++slot) {
                    int car_code = (game->fleet.trainCars[head].stateCode >> (3 + 3 * slot)) & 7;
                    if (car_code >= 7) continue;
                    int car_type = 2 + car_code;
                    if (car_type == 5) ++car_type;
                    previous_car = game->fleet.append_car(previous_car, car_type);
                    game->renderer.lastViewportCenterX = -1;
                }
                game->fleet.trainCars[head].stateCode = 7;
                game->renderer.lastViewportCenterX = -1;
            }

            game_loadTrainAtServiceVehicle(game, head, service_vehicle, false);
            if (game->fleet.trainCars[head].aiOverride == 1
                && game->fleet.can_load_at_service_site(head, service_vehicle)) {
                game->fleet.trainCars[head].phase = 3;
                stop_chain = 1;
            }
            if (service_vehicle != game->fleet.trainCars[head].lastServiceSite) {
                if (revenue > 0) {
                    game->economy.money += revenue;
                    game_pushMoneyHistory(game, revenue);
                    game->economy.totalRevenue += revenue;
                    game->fleet.trainCars[head].revenue += revenue;
                    game->record_train_revenue(revenue);
                }
                game->fleet.trainCars[head].energyCargoOrRemovalTicks =
                    (int8_t)(game->fleet.count_cars(head) * 40);
            }
            game->fleet.trainCars[head].lastServiceSite = (int8_t)service_vehicle;
            game->fleet.trainCars[head].flags = (int8_t)(game->fleet.trainCars[head].flags | 0x10);
        }
        if (train_flag::isHead(game->fleet.trainCars[head].flags) && game->fleet.trainCars[head].phase == 4
            && at_service_tile && game->world.trackMap[old_x][old_y] != 65) {
            game->fleet.trainCars[head].energyCargoOrRemovalTicks =
                (int8_t)(game->fleet.count_cars(head) * 40);
        }

        int collision = 0;
        if (game->fleet.trainCars[head].phase < 8) {
            ++game->fleet.trainCars[head].phase;
        } else {
            if (unsignedByte(game->fleet.trainCars[head].cargoAge) < 200) ++game->fleet.trainCars[head].cargoAge;
            old_x = game->fleet.trainCars[head].x;
            old_y = game->fleet.trainCars[head].y;
            int old_dir = game->fleet.trainCars[head].direction;
            game->fleet.trainCars[head].phase = 0;

            int next_x = unsignedByte(game->fleet.trainCars[head].x);
            int next_y = unsignedByte(game->fleet.trainCars[head].y);
            int current_track = game->world.trackMap[next_x][next_y];
            int current_variant = game->world.terrain.tileMap[next_x][next_y] - 70;
            if (current_variant == 6 && (game->fleet.trainCars[head].direction == 0
                                         || game->fleet.trainCars[head].direction == 1)) {
                current_track = track::PRESENT | (current_track & track::DIR_OWNER_MASK);
                current_variant = 0;
            }
            if (current_variant == 6 && (game->fleet.trainCars[head].direction == 2
                                         || game->fleet.trainCars[head].direction == 3)) {
                current_track = track::PRESENT
                    | ((current_track & (track::DIR_OWNER_MASK << track::DIR_OWNER_SHIFT))
                       >> track::DIR_OWNER_SHIFT);
                current_variant = 1;
            }
            current_variant = normalize_track_variant(current_variant);
            const int8_t* step =
                game->fleet.forwardTrackSteps[current_variant][0] == game->fleet.trainCars[head].direction
                ? game->fleet.forwardTrackSteps[current_variant]
                : game->fleet.reverseTrackSteps[current_variant];
            next_x += step[2];
            next_y += step[3];
            if (next_x >= 0 && next_y >= 0
                && next_x < game->world.terrain.mapWidth && next_y < game->world.terrain.mapHeight) {
                bool moved = false;
                int next_track = game->world.trackMap[next_x][next_y];
                int next_variant = normalize_track_variant(game->world.terrain.tileMap[next_x][next_y] - 70);
                for (int pair = 2; pair < kTrackStepValueCount / 2; ++pair) {
                    if (step[2 * pair] != next_variant) continue;
                    int next_dir = step[2 * pair + 1];
                    if (next_variant == 6) {
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
                    game->fleet.trainCars[head].direction = step[1];
                    game->fleet.reset_trailing_cars(
                        head, game->fleet.trainCars[head].nextCar, 0);
                }
                if (moved && train_flag::isHead(game->fleet.trainCars[head].flags)
                    && next_variant == 6) {
                    bool occupied = false;
                    for (int other = 0; other < 500; ++other) {
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
                        game->fleet.trainCars[head].phase = 8;
                        collision = 1;
                    }
                }
            } else {
                if (game->fleet.forwardTrackSteps[current_variant][0] == game->fleet.trainCars[head].direction)
                    game->fleet.trainCars[head].direction = game->fleet.forwardTrackSteps[current_variant][1];
                else if (game->fleet.reverseTrackSteps[current_variant][0] == game->fleet.trainCars[head].direction)
                    game->fleet.trainCars[head].direction = game->fleet.reverseTrackSteps[current_variant][1];
                game->fleet.reset_trailing_cars(
                    head, game->fleet.trainCars[head].nextCar, 0);
            }
        }
        if (stop_chain == 0 && collision == 0 && game->fleet.trainCars[head].nextCar != -1)
            game_updateTrain(game, game->fleet.trainCars[head].nextCar);
    }
    if (train_flag::isHead(game->fleet.trainCars[head].flags)) {
        int energy = unsignedByte(game->fleet.trainCars[head].energyCargoOrRemovalTicks) - 3;
        if (energy < 0) energy = 0;
        game->fleet.trainCars[head].energyCargoOrRemovalTicks = (int8_t)energy;
    }
}
