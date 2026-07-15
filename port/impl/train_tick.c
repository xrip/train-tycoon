// port/impl/train_tick.c - train movement, loading, and delivery.
#include "game_methods.h"

// int accumulateTrainTurns(int)
int32_t gc_accumulateTrainTurns(GameCanvas *gc, int32_t car) {
    if (car == -1) return 0;
    int tile = gc->tileMap[gc_byteToUnsignedInt(gc, gc->trainX[car])][gc_byteToUnsignedInt(gc, gc->trainY[car])];
    int turn = 0;
    if (tile == 77) turn = gc->trainDir[car] == 0 ? -1 : 1;
    if (tile == 79) turn = gc->trainDir[car] == 0 ? 1 : -1;
    if (tile == 78) turn = gc->trainDir[car] == 2 ? 1 : -1;
    if (tile == 80) turn = gc->trainDir[car] == 2 ? -1 : 1;
    return gc_accumulateTrainTurns(gc, gc->nextCar[car]) + turn;
}

// int findServiceVehicleAt(int,int)
int32_t gc_findServiceVehicleAt(GameCanvas *gc, int32_t x, int32_t y) {
    for (int vehicle = 0; vehicle < 250; ++vehicle) {
        if ((gc->vehicleFlags[vehicle] & 0x80) != 128) continue;
        int vehicle_x = gc_byteToUnsignedInt(gc, gc->serviceVehicleX[vehicle]);
        int vehicle_y = gc_byteToUnsignedInt(gc, gc->serviceVehicleY[vehicle]);
        if (vehicle_x == x && vehicle_y == y) return vehicle;
        if ((vehicle_x == x - 1 || vehicle_x == x + 1) && vehicle_y == y
            && (gc->vehicleFlags[vehicle] & 0xE0) == 224)
            return vehicle;
        if ((vehicle_y == y - 1 || vehicle_y == y + 1) && vehicle_x == x
            && (gc->vehicleFlags[vehicle] & 0xE0) == 160)
            return vehicle;
    }
    return 5000;
}

// int spawnTrainCar(int,int)
int32_t gc_spawnTrainCar(GameCanvas *gc, int32_t previous_car, int32_t car_type) {
    int new_car = 500;
    for (int car = 0; car < 500; ++car) {
        if ((gc->trainFlags[car] & 0x80) == 0) {
            new_car = car;
            break;
        }
    }
    if (new_car != 500) {
        gc->nextCar[previous_car] = (int16_t)new_car;
        gc->trainFlags[new_car] = (int8_t)(0x80 | car_type);
        gc->nextCar[new_car] = -1;
        gc->trainX[new_car] = gc->trainX[previous_car];
        gc->trainY[new_car] = gc->trainY[previous_car];
        gc->trainDir[new_car] = gc->trainDir[previous_car];
        gc->trainPhase[new_car] = (int8_t)(gc->trainPhase[previous_car] - 8);
        gc->trainEnergyOrCount[new_car] = 0;
        gc->wagonEnergyRemaining[new_car] = 0;
        gc->lastServiceVehicle[new_car] = -1;
    }
    gc->lastViewportCenterX = -1;
    return new_car;
}

// void resetTrailingCars(int,int,int)
void gc_resetTrailingCars(GameCanvas *gc, int32_t head, int32_t car, int32_t phase) {
    if (car == -1) return;
    gc_resetTrailingCars(gc, head, gc->nextCar[car], phase - 8);
    gc->trainX[car] = gc->trainX[head];
    gc->trainY[car] = gc->trainY[head];
    gc->trainDir[car] = gc->trainDir[head];
    gc->trainPhase[car] = (int8_t)(phase - 8);
}

// int takeServiceResource(int,int,int)
int32_t gc_takeServiceResource(GameCanvas *gc, int32_t vehicle,
                               int32_t resource, int32_t wanted) {
    int taken = gc->serviceResourceStock[vehicle][resource] > wanted
                ? wanted : gc->serviceResourceStock[vehicle][resource];
    gc->serviceResourceStock[vehicle][resource] =
        (int16_t)(gc->serviceResourceStock[vehicle][resource] - taken);
    return taken;
}

// int collectNearbyServiceResource(int,int,int)
int32_t gc_collectNearbyServiceResource(GameCanvas *gc, int32_t vehicle,
                                        int32_t resource, int32_t wanted) {
    int total = gc_takeServiceResource(gc, vehicle, resource, wanted);
    for (int other = 0; other < 250; ++other) {
        if (vehicle == other || (gc->vehicleFlags[other] & 0x80) != 128
            || (gc->vehicleFlags[other] & 7) != (gc->vehicleFlags[vehicle] & 7)
            || abs(gc->serviceVehicleX[vehicle] - gc->serviceVehicleX[other]) >= 3
            || abs(gc->serviceVehicleY[vehicle] - gc->serviceVehicleY[other]) >= 3)
            continue;
        total += gc_takeServiceResource(gc, other, resource, wanted - total);
    }
    return total;
}

// boolean trainCanLoadAtServiceVehicle(int,int)
bool gc_trainCanLoadAtServiceVehicle(GameCanvas *gc, int32_t head,
                                     int32_t vehicle) {
    int car = gc->nextCar[head];
    if (car == -1) return false;
    if ((gc->trainFlags[car] & 0x40) == 0) {
        switch (gc->trainFlags[car] & 0xF) {
            case 2:
                if ((gc->serviceLoadFlags[vehicle] & 1) == 1
                    && gc->trainEnergyOrCount[car] < 25)
                    return true;
                if ((gc->serviceLoadFlags[vehicle] & 0x10) == 0x10
                    && gc->wagonEnergyRemaining[car] < 15)
                    return true;
                break;
            case 3:
                if ((gc->serviceLoadFlags[vehicle] & 4) == 4
                    && gc->trainEnergyOrCount[car] < 25)
                    return true;
                break;
            case 4: case 5:
                if ((gc->serviceLoadFlags[vehicle] & 8) == 8
                    && gc->trainEnergyOrCount[car] < 30)
                    return true;
                break;
            case 6: case 7:
                if ((gc->serviceLoadFlags[vehicle] & 2) == 2
                    && gc->trainEnergyOrCount[car] < 30)
                    return true;
                break;
        }
    }
    return gc_trainCanLoadAtServiceVehicle(gc, car, vehicle);
}

// void loadTrainAtServiceVehicle(int,int,boolean)
void gc_loadTrainAtServiceVehicle(GameCanvas *gc, int32_t car,
                                  int32_t vehicle, bool construction_done) {
    if (car == -1) return;
    if ((gc->trainFlags[car] & 0x40) == 0) {
        int car_type = gc->trainFlags[car] & 0xF;
        switch (car_type) {
            case 2:
                gc->trainEnergyOrCount[car] =
                    (int8_t)(gc->trainEnergyOrCount[car]
                             + gc_collectNearbyServiceResource(
                                 gc, vehicle, 0, 25 - gc->trainEnergyOrCount[car]));
                gc->wagonEnergyRemaining[car] =
                    (int8_t)(gc->wagonEnergyRemaining[car]
                             + gc_collectNearbyServiceResource(
                                 gc, vehicle, 4, 15 - gc->wagonEnergyRemaining[car]));
                gc->cargoAge[car] = 0;
                if (vehicle != gc->lastServiceVehicle[car] && !construction_done) {
                    gc_advanceConstructionTimers(gc, gc_byteToUnsignedInt(gc, gc->trainX[car]),
                                                 gc_byteToUnsignedInt(gc, gc->trainY[car]), 4);
                    construction_done = true;
                }
                break;
            case 3:
                gc->trainEnergyOrCount[car] =
                    (int8_t)(gc->trainEnergyOrCount[car]
                             + gc_collectNearbyServiceResource(
                                 gc, vehicle, 1, 25 - gc->trainEnergyOrCount[car]));
                gc->cargoAge[car] = 0;
                break;
            case 4: case 5:
                gc->trainEnergyOrCount[car] =
                    (int8_t)(gc->trainEnergyOrCount[car]
                             + gc_collectNearbyServiceResource(
                                 gc, vehicle, 2, 30 - gc->trainEnergyOrCount[car]));
                gc->cargoAge[car] = 0;
                break;
            case 6: case 7:
                gc->trainEnergyOrCount[car] =
                    (int8_t)(gc->trainEnergyOrCount[car]
                             + gc_collectNearbyServiceResource(
                                 gc, vehicle, 3, 30 - gc->trainEnergyOrCount[car]));
                gc->cargoAge[car] = 0;
                break;
        }
        if (car_type == 4 || car_type == 5)
            gc->trainFlags[car] = (int8_t)((gc->trainFlags[car] & 0xF0)
                                  | (gc->trainEnergyOrCount[car] >= 10 ? 5 : 4));
        if (car_type == 6 || car_type == 7)
            gc->trainFlags[car] = (int8_t)((gc->trainFlags[car] & 0xF0)
                                  | (gc->trainEnergyOrCount[car] >= 10 ? 7 : 6));
        gc->lastServiceVehicle[car] = (int8_t)vehicle;
    } else {
        gc->cargoAge[car] = 0;
    }
    gc_loadTrainAtServiceVehicle(gc, gc->nextCar[car], vehicle, construction_done);
}

// void recordDeliveredResource(int,int)
void gc_recordDeliveredResource(GameCanvas *gc, int32_t resource, int32_t amount) {
    for (int row = 0; row < 5; ++row) {
        if (gc->menuRowType[row] == 3 + resource)
            gc->menuRowValue[row] += amount;
    }
    switch (resource) {
        case 1: gc->milestoneThreshold1 += amount; break;
        case 2: gc->milestoneThreshold2 += amount; break;
        case 3: gc->milestoneThreshold3 += amount; break;
        case 4: gc->milestoneThreshold4 += amount; break;
        case 5: gc->milestoneThreshold5 += amount; break;
    }
}

// void recordTrainRevenue(int)
void gc_recordTrainRevenue(GameCanvas *gc, int32_t amount) {
    for (int row = 0; row < 5; ++row) {
        if (gc->menuRowType[row] == 1) gc->menuRowValue[row] += amount;
    }
    gc->lifetimeTrainRevenue += amount;
    gc->trainRevenueScore += amount / 73;
}

// int unloadTrainRevenue(int,int)
int32_t gc_unloadTrainRevenue(GameCanvas *gc, int32_t car, int32_t vehicle) {
    if (car == -1) return 0;
    int revenue = 0;
    if ((gc->trainFlags[car] & 0x40) == 0
        && gc->lastServiceVehicle[car] != vehicle
        && (gc->trainEnergyOrCount[car] > 0 || gc->wagonEnergyRemaining[car] > 0)) {
        int age_scale = gc_byteToUnsignedInt(gc, gc->cargoAge[car]) / 15 + 1;
        switch (gc->trainFlags[car] & 0xF) {
            case 2:
                if ((gc->serviceUnloadFlags[vehicle] & 1) == 1) {
                    int value = gc->trainEnergyOrCount[car] * 3 * age_scale;
                    if (gc->routeResourceFlags[4] > 0) value *= 2;
                    revenue += value;
                    gc_recordDeliveredResource(gc, 4, gc->trainEnergyOrCount[car]);
                    gc->trainEnergyOrCount[car] = 0;
                }
                if ((gc->serviceUnloadFlags[vehicle] & 0x10) == 0x10) {
                    int value = gc->wagonEnergyRemaining[car] * 8 * age_scale;
                    if (gc->routeResourceFlags[5] > 0) value *= 2;
                    revenue += value;
                    gc_recordDeliveredResource(gc, 5, gc->wagonEnergyRemaining[car]);
                    gc->wagonEnergyRemaining[car] = 0;
                }
                break;
            case 3:
                if ((gc->serviceUnloadFlags[vehicle] & 2) == 2) {
                    int value = gc->trainEnergyOrCount[car] * 10 * age_scale;
                    if (gc->routeResourceFlags[3] > 0) value *= 2;
                    revenue += value;
                    gc_recordDeliveredResource(gc, 3, gc->trainEnergyOrCount[car]);
                    gc->trainEnergyOrCount[car] = 0;
                }
                break;
            case 4: case 5:
                if ((gc->serviceUnloadFlags[vehicle] & 4) == 4) {
                    int value = gc->trainEnergyOrCount[car] * 12 * age_scale;
                    if (gc->routeResourceFlags[2] > 0) value *= 2;
                    revenue += value;
                    gc_recordDeliveredResource(gc, 2, gc->trainEnergyOrCount[car]);
                    gc->trainEnergyOrCount[car] = 0;
                }
                break;
            case 6: case 7:
                if ((gc->serviceUnloadFlags[vehicle] & 8) == 8) {
                    int value = gc->trainEnergyOrCount[car] * 13 * age_scale;
                    if (gc->routeResourceFlags[1] > 0) value *= 2;
                    revenue += value;
                    gc_recordDeliveredResource(gc, 1, gc->trainEnergyOrCount[car]);
                    gc->trainEnergyOrCount[car] = 0;
                }
                break;
        }
    }
    return gc_unloadTrainRevenue(gc, gc->nextCar[car], vehicle) + revenue;
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

// void updateTrain(int)
void gc_updateTrain(GameCanvas *gc, int32_t head) {
    int stop_chain = 0;
    if ((gc->trainFlags[head] & 0x40) == 0
        || gc_byteToUnsignedInt(gc, gc->trainEnergyOrCount[head]) <= 0) {
        gc->trainFlags[head] = (int8_t)(gc->trainFlags[head] & 0xEF);
        if ((gc->trainFlags[head] & 0x40) == 64) {
            gc->trainEnergyOrCount[head] =
                (int8_t)(gc->wagonEnergyRemaining[head]
                         + gc_accumulateTrainTurns(gc, head) / 2
                         + gc_countCarsInTrain(gc, head) / 2
                         + gc->trainAgeOrTimer[head] / 3);
        }

        int old_x = gc_byteToUnsignedInt(gc, gc->trainX[head]);
        int old_y = gc_byteToUnsignedInt(gc, gc->trainY[head]);
        bool at_service_tile = (gc->tileMap[old_x][old_y] == 87
                                || gc->tileMap[old_x][old_y] == 88);
        if ((gc->trainFlags[head] & 0x40) == 64 && gc->trainPhase[head] == 4
            && at_service_tile && gc->trackMap[old_x][old_y] == 65) {
            int service_vehicle = gc_findServiceVehicleAt(gc, old_x, old_y);
            int revenue = 0;
            if (service_vehicle != gc->lastServiceVehicle[head])
                revenue = gc_unloadTrainRevenue(gc, head, service_vehicle);

            if (gc->trainStateCode[head] != 7) {
                int engine_type = gc->trainStateCode[head] & 7;
                gc->trainFlags[head] = (int8_t)(0xC0 | (engine_type >= 3 ? 1 : 0));
                gc->wagonEnergyRemaining[head] = (int8_t)(8 - engine_type);
                gc->trainEraOrStage[head] = (int8_t)engine_type;
                gc->trainAgeOrTimer[head] = 0;
                int car = gc->nextCar[head];
                while (car != -1) {
                    gc->trainFlags[car] = 0;
                    car = gc->nextCar[car];
                }
                int previous_car = head;
                for (int slot = 0; slot < 5; ++slot) {
                    int car_code = (gc->trainStateCode[head] >> (3 + 3 * slot)) & 7;
                    if (car_code >= 7) continue;
                    int car_type = 2 + car_code;
                    if (car_type == 5) ++car_type;
                    previous_car = gc_spawnTrainCar(gc, previous_car, car_type);
                }
                gc->trainStateCode[head] = 7;
                gc->lastViewportCenterX = -1;
            }

            gc_loadTrainAtServiceVehicle(gc, head, service_vehicle, false);
            if (gc->trainAiOverride[head] == 1
                && gc_trainCanLoadAtServiceVehicle(gc, head, service_vehicle)) {
                gc->trainPhase[head] = 3;
                stop_chain = 1;
            }
            if (service_vehicle != gc->lastServiceVehicle[head]) {
                if (revenue > 0) {
                    gc->money += revenue;
                    gc_pushMoneyHistory(gc, revenue);
                    gc->totalRevenue += revenue;
                    gc->trainRevenue[head] += revenue;
                    gc_recordTrainRevenue(gc, revenue);
                }
                gc->trainEnergyOrCount[head] =
                    (int8_t)(gc_countCarsInTrain(gc, head) * 40);
            }
            gc->lastServiceVehicle[head] = (int8_t)service_vehicle;
            gc->trainFlags[head] = (int8_t)(gc->trainFlags[head] | 0x10);
        }
        if ((gc->trainFlags[head] & 0x40) == 64 && gc->trainPhase[head] == 4
            && at_service_tile && gc->trackMap[old_x][old_y] != 65) {
            gc->trainEnergyOrCount[head] =
                (int8_t)(gc_countCarsInTrain(gc, head) * 40);
        }

        int collision = 0;
        if (gc->trainPhase[head] < 8) {
            ++gc->trainPhase[head];
        } else {
            if (gc_byteToUnsignedInt(gc, gc->cargoAge[head]) < 200) ++gc->cargoAge[head];
            old_x = gc->trainX[head];
            old_y = gc->trainY[head];
            int old_dir = gc->trainDir[head];
            gc->trainPhase[head] = 0;

            int next_x = gc_byteToUnsignedInt(gc, gc->trainX[head]);
            int next_y = gc_byteToUnsignedInt(gc, gc->trainY[head]);
            int current_track = gc->trackMap[next_x][next_y];
            int current_variant = gc->tileMap[next_x][next_y] - 70;
            if (current_variant == 6 && (gc->trainDir[head] == 0
                                         || gc->trainDir[head] == 1)) {
                current_track = 0x40 | (current_track & 7);
                current_variant = 0;
            }
            if (current_variant == 6 && (gc->trainDir[head] == 2
                                         || gc->trainDir[head] == 3)) {
                current_track = 0x40 | ((current_track & 0x38) >> 3);
                current_variant = 1;
            }
            current_variant = normalize_track_variant(current_variant);
            int8_t *step =
                gc->forwardTrackSteps[current_variant][0] == gc->trainDir[head]
                ? gc->forwardTrackSteps[current_variant]
                : gc->reverseTrackSteps[current_variant];
            next_x += step[2];
            next_y += step[3];
            if (next_x >= 0 && next_y >= 0
                && next_x < gc->mapWidth && next_y < gc->mapHeight) {
                bool moved = false;
                int next_track = gc->trackMap[next_x][next_y];
                int next_variant = normalize_track_variant(gc->tileMap[next_x][next_y] - 70);
                for (int pair = 2; pair < midp_arr_len(step) / 2; ++pair) {
                    if (step[2 * pair] != next_variant) continue;
                    int next_dir = step[2 * pair + 1];
                    if (next_variant == 6) {
                        if (next_dir == 0 || next_dir == 1)
                            next_track = 0x40 | (next_track & 7);
                        if (next_dir == 2 || next_dir == 3)
                            next_track = 0x40 | ((next_track & 0x38) >> 3);
                    }
                    if (current_track != next_track) break;
                    gc->trainX[head] = (int8_t)next_x;
                    gc->trainY[head] = (int8_t)next_y;
                    gc->trainDir[head] = (int8_t)next_dir;
                    moved = true;
                }
                if (!moved) {
                    gc->trainDir[head] = step[1];
                    gc_resetTrailingCars(gc, head, gc->nextCar[head], 0);
                }
                if (moved && (gc->trainFlags[head] & 0x40) == 64
                    && next_variant == 6) {
                    bool occupied = false;
                    for (int other = 0; other < 500; ++other) {
                        if ((gc->trainFlags[other] & 0x80) == 128 && other != head
                            && gc->trainX[other] == gc->trainX[head]
                            && gc->trainY[other] == gc->trainY[head]) {
                            occupied = true;
                            break;
                        }
                    }
                    if (occupied) {
                        gc->trainX[head] = (int8_t)old_x;
                        gc->trainY[head] = (int8_t)old_y;
                        gc->trainDir[head] = (int8_t)old_dir;
                        gc->trainPhase[head] = 8;
                        collision = 1;
                    }
                }
            } else {
                if (gc->forwardTrackSteps[current_variant][0] == gc->trainDir[head])
                    gc->trainDir[head] = gc->forwardTrackSteps[current_variant][1];
                else if (gc->reverseTrackSteps[current_variant][0] == gc->trainDir[head])
                    gc->trainDir[head] = gc->reverseTrackSteps[current_variant][1];
                gc_resetTrailingCars(gc, head, gc->nextCar[head], 0);
            }
        }
        if (stop_chain == 0 && collision == 0 && gc->nextCar[head] != -1)
            gc_updateTrain(gc, gc->nextCar[head]);
    }
    if ((gc->trainFlags[head] & 0x40) == 64) {
        int energy = gc_byteToUnsignedInt(gc, gc->trainEnergyOrCount[head]) - 3;
        if (energy < 0) energy = 0;
        gc->trainEnergyOrCount[head] = (int8_t)energy;
    }
}
