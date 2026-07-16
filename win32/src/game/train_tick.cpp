// Train movement, loading, and delivery.
#include "game_internal.h"
#include <cstdlib>

int32_t game_accumulateTrainTurns(Game *game, int32_t car) {
    if (car == -1) return 0;
    int tile = game->world.tileMap[unsignedByte(game->fleet.trainX[car])][unsignedByte(game->fleet.trainY[car])];
    int turn = 0;
    if (tile == 77) turn = game->fleet.trainDir[car] == 0 ? -1 : 1;
    if (tile == 79) turn = game->fleet.trainDir[car] == 0 ? 1 : -1;
    if (tile == 78) turn = game->fleet.trainDir[car] == 2 ? 1 : -1;
    if (tile == 80) turn = game->fleet.trainDir[car] == 2 ? -1 : 1;
    return game_accumulateTrainTurns(game, game->fleet.nextCar[car]) + turn;
}

int32_t game_findServiceVehicleAt(Game *game, int32_t x, int32_t y) {
    for (int vehicle = 0; vehicle < 250; ++vehicle) {
        if ((game->fleet.vehicleFlags[vehicle] & 0x80) != 128) continue;
        int vehicle_x = unsignedByte(game->fleet.serviceVehicleX[vehicle]);
        int vehicle_y = unsignedByte(game->fleet.serviceVehicleY[vehicle]);
        if (vehicle_x == x && vehicle_y == y) return vehicle;
        if ((vehicle_x == x - 1 || vehicle_x == x + 1) && vehicle_y == y
            && (game->fleet.vehicleFlags[vehicle] & 0xE0) == 224)
            return vehicle;
        if ((vehicle_y == y - 1 || vehicle_y == y + 1) && vehicle_x == x
            && (game->fleet.vehicleFlags[vehicle] & 0xE0) == 160)
            return vehicle;
    }
    return 5000;
}

int32_t game_spawnTrainCar(Game *game, int32_t previous_car, int32_t car_type) {
    int new_car = 500;
    for (int car = 0; car < 500; ++car) {
        if ((game->fleet.trainFlags[car] & 0x80) == 0) {
            new_car = car;
            break;
        }
    }
    if (new_car != 500) {
        game->fleet.nextCar[previous_car] = (int16_t)new_car;
        game->fleet.trainFlags[new_car] = (int8_t)(0x80 | car_type);
        game->fleet.nextCar[new_car] = -1;
        game->fleet.trainX[new_car] = game->fleet.trainX[previous_car];
        game->fleet.trainY[new_car] = game->fleet.trainY[previous_car];
        game->fleet.trainDir[new_car] = game->fleet.trainDir[previous_car];
        game->fleet.trainPhase[new_car] = (int8_t)(game->fleet.trainPhase[previous_car] - 8);
        game->fleet.trainEnergyOrCount[new_car] = 0;
        game->fleet.wagonEnergyRemaining[new_car] = 0;
        game->fleet.lastServiceVehicle[new_car] = -1;
    }
    game->renderer.lastViewportCenterX = -1;
    return new_car;
}

void game_resetTrailingCars(Game *game, int32_t head, int32_t car, int32_t phase) {
    if (car == -1) return;
    game_resetTrailingCars(game, head, game->fleet.nextCar[car], phase - 8);
    game->fleet.trainX[car] = game->fleet.trainX[head];
    game->fleet.trainY[car] = game->fleet.trainY[head];
    game->fleet.trainDir[car] = game->fleet.trainDir[head];
    game->fleet.trainPhase[car] = (int8_t)(phase - 8);
}

int32_t game_takeServiceResource(Game *game, int32_t vehicle,
                               int32_t resource, int32_t wanted) {
    int taken = game->fleet.serviceResourceStock[vehicle][resource] > wanted
                ? wanted : game->fleet.serviceResourceStock[vehicle][resource];
    game->fleet.serviceResourceStock[vehicle][resource] =
        (int16_t)(game->fleet.serviceResourceStock[vehicle][resource] - taken);
    return taken;
}

int32_t game_collectNearbyServiceResource(Game *game, int32_t vehicle,
                                        int32_t resource, int32_t wanted) {
    int total = game_takeServiceResource(game, vehicle, resource, wanted);
    for (int other = 0; other < 250; ++other) {
        if (vehicle == other || (game->fleet.vehicleFlags[other] & 0x80) != 128
            || (game->fleet.vehicleFlags[other] & 7) != (game->fleet.vehicleFlags[vehicle] & 7)
            || std::abs(game->fleet.serviceVehicleX[vehicle] - game->fleet.serviceVehicleX[other]) >= 3
            || std::abs(game->fleet.serviceVehicleY[vehicle] - game->fleet.serviceVehicleY[other]) >= 3)
            continue;
        total += game_takeServiceResource(game, other, resource, wanted - total);
    }
    return total;
}

bool game_trainCanLoadAtServiceVehicle(Game *game, int32_t head,
                                     int32_t vehicle) {
    int car = game->fleet.nextCar[head];
    if (car == -1) return false;
    if ((game->fleet.trainFlags[car] & 0x40) == 0) {
        switch (game->fleet.trainFlags[car] & 0xF) {
            case 2:
                if ((game->fleet.serviceLoadFlags[vehicle] & 1) == 1
                    && game->fleet.trainEnergyOrCount[car] < 25)
                    return true;
                if ((game->fleet.serviceLoadFlags[vehicle] & 0x10) == 0x10
                    && game->fleet.wagonEnergyRemaining[car] < 15)
                    return true;
                break;
            case 3:
                if ((game->fleet.serviceLoadFlags[vehicle] & 4) == 4
                    && game->fleet.trainEnergyOrCount[car] < 25)
                    return true;
                break;
            case 4: case 5:
                if ((game->fleet.serviceLoadFlags[vehicle] & 8) == 8
                    && game->fleet.trainEnergyOrCount[car] < 30)
                    return true;
                break;
            case 6: case 7:
                if ((game->fleet.serviceLoadFlags[vehicle] & 2) == 2
                    && game->fleet.trainEnergyOrCount[car] < 30)
                    return true;
                break;
        }
    }
    return game_trainCanLoadAtServiceVehicle(game, car, vehicle);
}

void game_loadTrainAtServiceVehicle(Game *game, int32_t car,
                                  int32_t vehicle, bool construction_done) {
    if (car == -1) return;
    if ((game->fleet.trainFlags[car] & 0x40) == 0) {
        int car_type = game->fleet.trainFlags[car] & 0xF;
        switch (car_type) {
            case 2:
                game->fleet.trainEnergyOrCount[car] =
                    (int8_t)(game->fleet.trainEnergyOrCount[car]
                             + game_collectNearbyServiceResource(
                                 game, vehicle, 0, 25 - game->fleet.trainEnergyOrCount[car]));
                game->fleet.wagonEnergyRemaining[car] =
                    (int8_t)(game->fleet.wagonEnergyRemaining[car]
                             + game_collectNearbyServiceResource(
                                 game, vehicle, 4, 15 - game->fleet.wagonEnergyRemaining[car]));
                game->fleet.cargoAge[car] = 0;
                if (vehicle != game->fleet.lastServiceVehicle[car] && !construction_done) {
                    game_advanceConstructionTimers(game, unsignedByte(game->fleet.trainX[car]),
                                                 unsignedByte(game->fleet.trainY[car]), 4);
                    construction_done = true;
                }
                break;
            case 3:
                game->fleet.trainEnergyOrCount[car] =
                    (int8_t)(game->fleet.trainEnergyOrCount[car]
                             + game_collectNearbyServiceResource(
                                 game, vehicle, 1, 25 - game->fleet.trainEnergyOrCount[car]));
                game->fleet.cargoAge[car] = 0;
                break;
            case 4: case 5:
                game->fleet.trainEnergyOrCount[car] =
                    (int8_t)(game->fleet.trainEnergyOrCount[car]
                             + game_collectNearbyServiceResource(
                                 game, vehicle, 2, 30 - game->fleet.trainEnergyOrCount[car]));
                game->fleet.cargoAge[car] = 0;
                break;
            case 6: case 7:
                game->fleet.trainEnergyOrCount[car] =
                    (int8_t)(game->fleet.trainEnergyOrCount[car]
                             + game_collectNearbyServiceResource(
                                 game, vehicle, 3, 30 - game->fleet.trainEnergyOrCount[car]));
                game->fleet.cargoAge[car] = 0;
                break;
        }
        if (car_type == 4 || car_type == 5)
            game->fleet.trainFlags[car] = (int8_t)((game->fleet.trainFlags[car] & 0xF0)
                                  | (game->fleet.trainEnergyOrCount[car] >= 10 ? 5 : 4));
        if (car_type == 6 || car_type == 7)
            game->fleet.trainFlags[car] = (int8_t)((game->fleet.trainFlags[car] & 0xF0)
                                  | (game->fleet.trainEnergyOrCount[car] >= 10 ? 7 : 6));
        game->fleet.lastServiceVehicle[car] = (int8_t)vehicle;
    } else {
        game->fleet.cargoAge[car] = 0;
    }
    game_loadTrainAtServiceVehicle(game, game->fleet.nextCar[car], vehicle, construction_done);
}

void game_recordDeliveredResource(Game *game, int32_t resource, int32_t amount) {
    for (int row = 0; row < 5; ++row) {
        if (game->ui.menuRowType[row] == 3 + resource)
            game->ui.menuRowValue[row] += amount;
    }
    switch (resource) {
        case 1: game->scenario.milestoneThreshold1 += amount; break;
        case 2: game->scenario.milestoneThreshold2 += amount; break;
        case 3: game->scenario.milestoneThreshold3 += amount; break;
        case 4: game->scenario.milestoneThreshold4 += amount; break;
        case 5: game->scenario.milestoneThreshold5 += amount; break;
    }
}

void game_recordTrainRevenue(Game *game, int32_t amount) {
    for (int row = 0; row < 5; ++row) {
        if (game->ui.menuRowType[row] == 1) game->ui.menuRowValue[row] += amount;
    }
    game->economy.lifetimeTrainRevenue += amount;
    game->economy.trainRevenueScore += amount / 73;
}

int32_t game_unloadTrainRevenue(Game *game, int32_t car, int32_t vehicle) {
    if (car == -1) return 0;
    int revenue = 0;
    if ((game->fleet.trainFlags[car] & 0x40) == 0
        && game->fleet.lastServiceVehicle[car] != vehicle
        && (game->fleet.trainEnergyOrCount[car] > 0 || game->fleet.wagonEnergyRemaining[car] > 0)) {
        int age_scale = unsignedByte(game->fleet.cargoAge[car]) / 15 + 1;
        switch (game->fleet.trainFlags[car] & 0xF) {
            case 2:
                if ((game->fleet.serviceUnloadFlags[vehicle] & 1) == 1) {
                    int value = game->fleet.trainEnergyOrCount[car] * 3 * age_scale;
                    if (game->scenario.routeResourceFlags[4] > 0) value *= 2;
                    revenue += value;
                    game_recordDeliveredResource(game, 4, game->fleet.trainEnergyOrCount[car]);
                    game->fleet.trainEnergyOrCount[car] = 0;
                }
                if ((game->fleet.serviceUnloadFlags[vehicle] & 0x10) == 0x10) {
                    int value = game->fleet.wagonEnergyRemaining[car] * 8 * age_scale;
                    if (game->scenario.routeResourceFlags[5] > 0) value *= 2;
                    revenue += value;
                    game_recordDeliveredResource(game, 5, game->fleet.wagonEnergyRemaining[car]);
                    game->fleet.wagonEnergyRemaining[car] = 0;
                }
                break;
            case 3:
                if ((game->fleet.serviceUnloadFlags[vehicle] & 2) == 2) {
                    int value = game->fleet.trainEnergyOrCount[car] * 10 * age_scale;
                    if (game->scenario.routeResourceFlags[3] > 0) value *= 2;
                    revenue += value;
                    game_recordDeliveredResource(game, 3, game->fleet.trainEnergyOrCount[car]);
                    game->fleet.trainEnergyOrCount[car] = 0;
                }
                break;
            case 4: case 5:
                if ((game->fleet.serviceUnloadFlags[vehicle] & 4) == 4) {
                    int value = game->fleet.trainEnergyOrCount[car] * 12 * age_scale;
                    if (game->scenario.routeResourceFlags[2] > 0) value *= 2;
                    revenue += value;
                    game_recordDeliveredResource(game, 2, game->fleet.trainEnergyOrCount[car]);
                    game->fleet.trainEnergyOrCount[car] = 0;
                }
                break;
            case 6: case 7:
                if ((game->fleet.serviceUnloadFlags[vehicle] & 8) == 8) {
                    int value = game->fleet.trainEnergyOrCount[car] * 13 * age_scale;
                    if (game->scenario.routeResourceFlags[1] > 0) value *= 2;
                    revenue += value;
                    game_recordDeliveredResource(game, 1, game->fleet.trainEnergyOrCount[car]);
                    game->fleet.trainEnergyOrCount[car] = 0;
                }
                break;
        }
    }
    return game_unloadTrainRevenue(game, game->fleet.nextCar[car], vehicle) + revenue;
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
    if ((game->fleet.trainFlags[head] & 0x40) == 0
        || unsignedByte(game->fleet.trainEnergyOrCount[head]) <= 0) {
        game->fleet.trainFlags[head] = (int8_t)(game->fleet.trainFlags[head] & 0xEF);
        if ((game->fleet.trainFlags[head] & 0x40) == 64) {
            game->fleet.trainEnergyOrCount[head] =
                (int8_t)(game->fleet.wagonEnergyRemaining[head]
                         + game_accumulateTrainTurns(game, head) / 2
                         + game_countCarsInTrain(game, head) / 2
                         + game->fleet.trainAgeOrTimer[head] / 3);
        }

        int old_x = unsignedByte(game->fleet.trainX[head]);
        int old_y = unsignedByte(game->fleet.trainY[head]);
        bool at_service_tile = (game->world.tileMap[old_x][old_y] == 87
                                || game->world.tileMap[old_x][old_y] == 88);
        if ((game->fleet.trainFlags[head] & 0x40) == 64 && game->fleet.trainPhase[head] == 4
            && at_service_tile && game->world.trackMap[old_x][old_y] == 65) {
            int service_vehicle = game_findServiceVehicleAt(game, old_x, old_y);
            int revenue = 0;
            if (service_vehicle != game->fleet.lastServiceVehicle[head])
                revenue = game_unloadTrainRevenue(game, head, service_vehicle);

            if (game->fleet.trainStateCode[head] != 7) {
                int engine_type = game->fleet.trainStateCode[head] & 7;
                game->fleet.trainFlags[head] = (int8_t)(0xC0 | (engine_type >= 3 ? 1 : 0));
                game->fleet.wagonEnergyRemaining[head] = (int8_t)(8 - engine_type);
                game->fleet.trainEraOrStage[head] = (int8_t)engine_type;
                game->fleet.trainAgeOrTimer[head] = 0;
                int car = game->fleet.nextCar[head];
                while (car != -1) {
                    game->fleet.trainFlags[car] = 0;
                    car = game->fleet.nextCar[car];
                }
                int previous_car = head;
                for (int slot = 0; slot < 5; ++slot) {
                    int car_code = (game->fleet.trainStateCode[head] >> (3 + 3 * slot)) & 7;
                    if (car_code >= 7) continue;
                    int car_type = 2 + car_code;
                    if (car_type == 5) ++car_type;
                    previous_car = game_spawnTrainCar(game, previous_car, car_type);
                }
                game->fleet.trainStateCode[head] = 7;
                game->renderer.lastViewportCenterX = -1;
            }

            game_loadTrainAtServiceVehicle(game, head, service_vehicle, false);
            if (game->fleet.trainAiOverride[head] == 1
                && game_trainCanLoadAtServiceVehicle(game, head, service_vehicle)) {
                game->fleet.trainPhase[head] = 3;
                stop_chain = 1;
            }
            if (service_vehicle != game->fleet.lastServiceVehicle[head]) {
                if (revenue > 0) {
                    game->economy.money += revenue;
                    game_pushMoneyHistory(game, revenue);
                    game->economy.totalRevenue += revenue;
                    game->fleet.trainRevenue[head] += revenue;
                    game_recordTrainRevenue(game, revenue);
                }
                game->fleet.trainEnergyOrCount[head] =
                    (int8_t)(game_countCarsInTrain(game, head) * 40);
            }
            game->fleet.lastServiceVehicle[head] = (int8_t)service_vehicle;
            game->fleet.trainFlags[head] = (int8_t)(game->fleet.trainFlags[head] | 0x10);
        }
        if ((game->fleet.trainFlags[head] & 0x40) == 64 && game->fleet.trainPhase[head] == 4
            && at_service_tile && game->world.trackMap[old_x][old_y] != 65) {
            game->fleet.trainEnergyOrCount[head] =
                (int8_t)(game_countCarsInTrain(game, head) * 40);
        }

        int collision = 0;
        if (game->fleet.trainPhase[head] < 8) {
            ++game->fleet.trainPhase[head];
        } else {
            if (unsignedByte(game->fleet.cargoAge[head]) < 200) ++game->fleet.cargoAge[head];
            old_x = game->fleet.trainX[head];
            old_y = game->fleet.trainY[head];
            int old_dir = game->fleet.trainDir[head];
            game->fleet.trainPhase[head] = 0;

            int next_x = unsignedByte(game->fleet.trainX[head]);
            int next_y = unsignedByte(game->fleet.trainY[head]);
            int current_track = game->world.trackMap[next_x][next_y];
            int current_variant = game->world.tileMap[next_x][next_y] - 70;
            if (current_variant == 6 && (game->fleet.trainDir[head] == 0
                                         || game->fleet.trainDir[head] == 1)) {
                current_track = 0x40 | (current_track & 7);
                current_variant = 0;
            }
            if (current_variant == 6 && (game->fleet.trainDir[head] == 2
                                         || game->fleet.trainDir[head] == 3)) {
                current_track = 0x40 | ((current_track & 0x38) >> 3);
                current_variant = 1;
            }
            current_variant = normalize_track_variant(current_variant);
            const int8_t* step =
                game->fleet.forwardTrackSteps[current_variant][0] == game->fleet.trainDir[head]
                ? game->fleet.forwardTrackSteps[current_variant]
                : game->fleet.reverseTrackSteps[current_variant];
            next_x += step[2];
            next_y += step[3];
            if (next_x >= 0 && next_y >= 0
                && next_x < game->world.mapWidth && next_y < game->world.mapHeight) {
                bool moved = false;
                int next_track = game->world.trackMap[next_x][next_y];
                int next_variant = normalize_track_variant(game->world.tileMap[next_x][next_y] - 70);
                for (int pair = 2; pair < kTrackStepValueCount / 2; ++pair) {
                    if (step[2 * pair] != next_variant) continue;
                    int next_dir = step[2 * pair + 1];
                    if (next_variant == 6) {
                        if (next_dir == 0 || next_dir == 1)
                            next_track = 0x40 | (next_track & 7);
                        if (next_dir == 2 || next_dir == 3)
                            next_track = 0x40 | ((next_track & 0x38) >> 3);
                    }
                    if (current_track != next_track) break;
                    game->fleet.trainX[head] = (int8_t)next_x;
                    game->fleet.trainY[head] = (int8_t)next_y;
                    game->fleet.trainDir[head] = (int8_t)next_dir;
                    moved = true;
                }
                if (!moved) {
                    game->fleet.trainDir[head] = step[1];
                    game_resetTrailingCars(game, head, game->fleet.nextCar[head], 0);
                }
                if (moved && (game->fleet.trainFlags[head] & 0x40) == 64
                    && next_variant == 6) {
                    bool occupied = false;
                    for (int other = 0; other < 500; ++other) {
                        if ((game->fleet.trainFlags[other] & 0x80) == 128 && other != head
                            && game->fleet.trainX[other] == game->fleet.trainX[head]
                            && game->fleet.trainY[other] == game->fleet.trainY[head]) {
                            occupied = true;
                            break;
                        }
                    }
                    if (occupied) {
                        game->fleet.trainX[head] = (int8_t)old_x;
                        game->fleet.trainY[head] = (int8_t)old_y;
                        game->fleet.trainDir[head] = (int8_t)old_dir;
                        game->fleet.trainPhase[head] = 8;
                        collision = 1;
                    }
                }
            } else {
                if (game->fleet.forwardTrackSteps[current_variant][0] == game->fleet.trainDir[head])
                    game->fleet.trainDir[head] = game->fleet.forwardTrackSteps[current_variant][1];
                else if (game->fleet.reverseTrackSteps[current_variant][0] == game->fleet.trainDir[head])
                    game->fleet.trainDir[head] = game->fleet.reverseTrackSteps[current_variant][1];
                game_resetTrailingCars(game, head, game->fleet.nextCar[head], 0);
            }
        }
        if (stop_chain == 0 && collision == 0 && game->fleet.nextCar[head] != -1)
            game_updateTrain(game, game->fleet.nextCar[head]);
    }
    if ((game->fleet.trainFlags[head] & 0x40) == 64) {
        int energy = unsignedByte(game->fleet.trainEnergyOrCount[head]) - 3;
        if (energy < 0) energy = 0;
        game->fleet.trainEnergyOrCount[head] = (int8_t)energy;
    }
}
