// Train movement, loading, and delivery.
#include "game_methods.h"
#include <cstdlib>

// int accumulateTrainTurns(int)
int32_t gc_accumulateTrainTurns(GameCanvas *gc, int32_t car) {
    if (car == -1) return 0;
    int tile = gc->world.tileMap[gc_byteToUnsignedInt(gc, gc->fleet.trainX[car])][gc_byteToUnsignedInt(gc, gc->fleet.trainY[car])];
    int turn = 0;
    if (tile == 77) turn = gc->fleet.trainDir[car] == 0 ? -1 : 1;
    if (tile == 79) turn = gc->fleet.trainDir[car] == 0 ? 1 : -1;
    if (tile == 78) turn = gc->fleet.trainDir[car] == 2 ? 1 : -1;
    if (tile == 80) turn = gc->fleet.trainDir[car] == 2 ? -1 : 1;
    return gc_accumulateTrainTurns(gc, gc->fleet.nextCar[car]) + turn;
}

// int findServiceVehicleAt(int,int)
int32_t gc_findServiceVehicleAt(GameCanvas *gc, int32_t x, int32_t y) {
    for (int vehicle = 0; vehicle < 250; ++vehicle) {
        if ((gc->fleet.vehicleFlags[vehicle] & 0x80) != 128) continue;
        int vehicle_x = gc_byteToUnsignedInt(gc, gc->fleet.serviceVehicleX[vehicle]);
        int vehicle_y = gc_byteToUnsignedInt(gc, gc->fleet.serviceVehicleY[vehicle]);
        if (vehicle_x == x && vehicle_y == y) return vehicle;
        if ((vehicle_x == x - 1 || vehicle_x == x + 1) && vehicle_y == y
            && (gc->fleet.vehicleFlags[vehicle] & 0xE0) == 224)
            return vehicle;
        if ((vehicle_y == y - 1 || vehicle_y == y + 1) && vehicle_x == x
            && (gc->fleet.vehicleFlags[vehicle] & 0xE0) == 160)
            return vehicle;
    }
    return 5000;
}

// int spawnTrainCar(int,int)
int32_t gc_spawnTrainCar(GameCanvas *gc, int32_t previous_car, int32_t car_type) {
    int new_car = 500;
    for (int car = 0; car < 500; ++car) {
        if ((gc->fleet.trainFlags[car] & 0x80) == 0) {
            new_car = car;
            break;
        }
    }
    if (new_car != 500) {
        gc->fleet.nextCar[previous_car] = (int16_t)new_car;
        gc->fleet.trainFlags[new_car] = (int8_t)(0x80 | car_type);
        gc->fleet.nextCar[new_car] = -1;
        gc->fleet.trainX[new_car] = gc->fleet.trainX[previous_car];
        gc->fleet.trainY[new_car] = gc->fleet.trainY[previous_car];
        gc->fleet.trainDir[new_car] = gc->fleet.trainDir[previous_car];
        gc->fleet.trainPhase[new_car] = (int8_t)(gc->fleet.trainPhase[previous_car] - 8);
        gc->fleet.trainEnergyOrCount[new_car] = 0;
        gc->fleet.wagonEnergyRemaining[new_car] = 0;
        gc->fleet.lastServiceVehicle[new_car] = -1;
    }
    gc->lastViewportCenterX = -1;
    return new_car;
}

// void resetTrailingCars(int,int,int)
void gc_resetTrailingCars(GameCanvas *gc, int32_t head, int32_t car, int32_t phase) {
    if (car == -1) return;
    gc_resetTrailingCars(gc, head, gc->fleet.nextCar[car], phase - 8);
    gc->fleet.trainX[car] = gc->fleet.trainX[head];
    gc->fleet.trainY[car] = gc->fleet.trainY[head];
    gc->fleet.trainDir[car] = gc->fleet.trainDir[head];
    gc->fleet.trainPhase[car] = (int8_t)(phase - 8);
}

// int takeServiceResource(int,int,int)
int32_t gc_takeServiceResource(GameCanvas *gc, int32_t vehicle,
                               int32_t resource, int32_t wanted) {
    int taken = gc->fleet.serviceResourceStock[vehicle][resource] > wanted
                ? wanted : gc->fleet.serviceResourceStock[vehicle][resource];
    gc->fleet.serviceResourceStock[vehicle][resource] =
        (int16_t)(gc->fleet.serviceResourceStock[vehicle][resource] - taken);
    return taken;
}

// int collectNearbyServiceResource(int,int,int)
int32_t gc_collectNearbyServiceResource(GameCanvas *gc, int32_t vehicle,
                                        int32_t resource, int32_t wanted) {
    int total = gc_takeServiceResource(gc, vehicle, resource, wanted);
    for (int other = 0; other < 250; ++other) {
        if (vehicle == other || (gc->fleet.vehicleFlags[other] & 0x80) != 128
            || (gc->fleet.vehicleFlags[other] & 7) != (gc->fleet.vehicleFlags[vehicle] & 7)
            || std::abs(gc->fleet.serviceVehicleX[vehicle] - gc->fleet.serviceVehicleX[other]) >= 3
            || std::abs(gc->fleet.serviceVehicleY[vehicle] - gc->fleet.serviceVehicleY[other]) >= 3)
            continue;
        total += gc_takeServiceResource(gc, other, resource, wanted - total);
    }
    return total;
}

// boolean trainCanLoadAtServiceVehicle(int,int)
bool gc_trainCanLoadAtServiceVehicle(GameCanvas *gc, int32_t head,
                                     int32_t vehicle) {
    int car = gc->fleet.nextCar[head];
    if (car == -1) return false;
    if ((gc->fleet.trainFlags[car] & 0x40) == 0) {
        switch (gc->fleet.trainFlags[car] & 0xF) {
            case 2:
                if ((gc->fleet.serviceLoadFlags[vehicle] & 1) == 1
                    && gc->fleet.trainEnergyOrCount[car] < 25)
                    return true;
                if ((gc->fleet.serviceLoadFlags[vehicle] & 0x10) == 0x10
                    && gc->fleet.wagonEnergyRemaining[car] < 15)
                    return true;
                break;
            case 3:
                if ((gc->fleet.serviceLoadFlags[vehicle] & 4) == 4
                    && gc->fleet.trainEnergyOrCount[car] < 25)
                    return true;
                break;
            case 4: case 5:
                if ((gc->fleet.serviceLoadFlags[vehicle] & 8) == 8
                    && gc->fleet.trainEnergyOrCount[car] < 30)
                    return true;
                break;
            case 6: case 7:
                if ((gc->fleet.serviceLoadFlags[vehicle] & 2) == 2
                    && gc->fleet.trainEnergyOrCount[car] < 30)
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
    if ((gc->fleet.trainFlags[car] & 0x40) == 0) {
        int car_type = gc->fleet.trainFlags[car] & 0xF;
        switch (car_type) {
            case 2:
                gc->fleet.trainEnergyOrCount[car] =
                    (int8_t)(gc->fleet.trainEnergyOrCount[car]
                             + gc_collectNearbyServiceResource(
                                 gc, vehicle, 0, 25 - gc->fleet.trainEnergyOrCount[car]));
                gc->fleet.wagonEnergyRemaining[car] =
                    (int8_t)(gc->fleet.wagonEnergyRemaining[car]
                             + gc_collectNearbyServiceResource(
                                 gc, vehicle, 4, 15 - gc->fleet.wagonEnergyRemaining[car]));
                gc->fleet.cargoAge[car] = 0;
                if (vehicle != gc->fleet.lastServiceVehicle[car] && !construction_done) {
                    gc_advanceConstructionTimers(gc, gc_byteToUnsignedInt(gc, gc->fleet.trainX[car]),
                                                 gc_byteToUnsignedInt(gc, gc->fleet.trainY[car]), 4);
                    construction_done = true;
                }
                break;
            case 3:
                gc->fleet.trainEnergyOrCount[car] =
                    (int8_t)(gc->fleet.trainEnergyOrCount[car]
                             + gc_collectNearbyServiceResource(
                                 gc, vehicle, 1, 25 - gc->fleet.trainEnergyOrCount[car]));
                gc->fleet.cargoAge[car] = 0;
                break;
            case 4: case 5:
                gc->fleet.trainEnergyOrCount[car] =
                    (int8_t)(gc->fleet.trainEnergyOrCount[car]
                             + gc_collectNearbyServiceResource(
                                 gc, vehicle, 2, 30 - gc->fleet.trainEnergyOrCount[car]));
                gc->fleet.cargoAge[car] = 0;
                break;
            case 6: case 7:
                gc->fleet.trainEnergyOrCount[car] =
                    (int8_t)(gc->fleet.trainEnergyOrCount[car]
                             + gc_collectNearbyServiceResource(
                                 gc, vehicle, 3, 30 - gc->fleet.trainEnergyOrCount[car]));
                gc->fleet.cargoAge[car] = 0;
                break;
        }
        if (car_type == 4 || car_type == 5)
            gc->fleet.trainFlags[car] = (int8_t)((gc->fleet.trainFlags[car] & 0xF0)
                                  | (gc->fleet.trainEnergyOrCount[car] >= 10 ? 5 : 4));
        if (car_type == 6 || car_type == 7)
            gc->fleet.trainFlags[car] = (int8_t)((gc->fleet.trainFlags[car] & 0xF0)
                                  | (gc->fleet.trainEnergyOrCount[car] >= 10 ? 7 : 6));
        gc->fleet.lastServiceVehicle[car] = (int8_t)vehicle;
    } else {
        gc->fleet.cargoAge[car] = 0;
    }
    gc_loadTrainAtServiceVehicle(gc, gc->fleet.nextCar[car], vehicle, construction_done);
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
    if ((gc->fleet.trainFlags[car] & 0x40) == 0
        && gc->fleet.lastServiceVehicle[car] != vehicle
        && (gc->fleet.trainEnergyOrCount[car] > 0 || gc->fleet.wagonEnergyRemaining[car] > 0)) {
        int age_scale = gc_byteToUnsignedInt(gc, gc->fleet.cargoAge[car]) / 15 + 1;
        switch (gc->fleet.trainFlags[car] & 0xF) {
            case 2:
                if ((gc->fleet.serviceUnloadFlags[vehicle] & 1) == 1) {
                    int value = gc->fleet.trainEnergyOrCount[car] * 3 * age_scale;
                    if (gc->routeResourceFlags[4] > 0) value *= 2;
                    revenue += value;
                    gc_recordDeliveredResource(gc, 4, gc->fleet.trainEnergyOrCount[car]);
                    gc->fleet.trainEnergyOrCount[car] = 0;
                }
                if ((gc->fleet.serviceUnloadFlags[vehicle] & 0x10) == 0x10) {
                    int value = gc->fleet.wagonEnergyRemaining[car] * 8 * age_scale;
                    if (gc->routeResourceFlags[5] > 0) value *= 2;
                    revenue += value;
                    gc_recordDeliveredResource(gc, 5, gc->fleet.wagonEnergyRemaining[car]);
                    gc->fleet.wagonEnergyRemaining[car] = 0;
                }
                break;
            case 3:
                if ((gc->fleet.serviceUnloadFlags[vehicle] & 2) == 2) {
                    int value = gc->fleet.trainEnergyOrCount[car] * 10 * age_scale;
                    if (gc->routeResourceFlags[3] > 0) value *= 2;
                    revenue += value;
                    gc_recordDeliveredResource(gc, 3, gc->fleet.trainEnergyOrCount[car]);
                    gc->fleet.trainEnergyOrCount[car] = 0;
                }
                break;
            case 4: case 5:
                if ((gc->fleet.serviceUnloadFlags[vehicle] & 4) == 4) {
                    int value = gc->fleet.trainEnergyOrCount[car] * 12 * age_scale;
                    if (gc->routeResourceFlags[2] > 0) value *= 2;
                    revenue += value;
                    gc_recordDeliveredResource(gc, 2, gc->fleet.trainEnergyOrCount[car]);
                    gc->fleet.trainEnergyOrCount[car] = 0;
                }
                break;
            case 6: case 7:
                if ((gc->fleet.serviceUnloadFlags[vehicle] & 8) == 8) {
                    int value = gc->fleet.trainEnergyOrCount[car] * 13 * age_scale;
                    if (gc->routeResourceFlags[1] > 0) value *= 2;
                    revenue += value;
                    gc_recordDeliveredResource(gc, 1, gc->fleet.trainEnergyOrCount[car]);
                    gc->fleet.trainEnergyOrCount[car] = 0;
                }
                break;
        }
    }
    return gc_unloadTrainRevenue(gc, gc->fleet.nextCar[car], vehicle) + revenue;
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
    if ((gc->fleet.trainFlags[head] & 0x40) == 0
        || gc_byteToUnsignedInt(gc, gc->fleet.trainEnergyOrCount[head]) <= 0) {
        gc->fleet.trainFlags[head] = (int8_t)(gc->fleet.trainFlags[head] & 0xEF);
        if ((gc->fleet.trainFlags[head] & 0x40) == 64) {
            gc->fleet.trainEnergyOrCount[head] =
                (int8_t)(gc->fleet.wagonEnergyRemaining[head]
                         + gc_accumulateTrainTurns(gc, head) / 2
                         + gc_countCarsInTrain(gc, head) / 2
                         + gc->fleet.trainAgeOrTimer[head] / 3);
        }

        int old_x = gc_byteToUnsignedInt(gc, gc->fleet.trainX[head]);
        int old_y = gc_byteToUnsignedInt(gc, gc->fleet.trainY[head]);
        bool at_service_tile = (gc->world.tileMap[old_x][old_y] == 87
                                || gc->world.tileMap[old_x][old_y] == 88);
        if ((gc->fleet.trainFlags[head] & 0x40) == 64 && gc->fleet.trainPhase[head] == 4
            && at_service_tile && gc->world.trackMap[old_x][old_y] == 65) {
            int service_vehicle = gc_findServiceVehicleAt(gc, old_x, old_y);
            int revenue = 0;
            if (service_vehicle != gc->fleet.lastServiceVehicle[head])
                revenue = gc_unloadTrainRevenue(gc, head, service_vehicle);

            if (gc->fleet.trainStateCode[head] != 7) {
                int engine_type = gc->fleet.trainStateCode[head] & 7;
                gc->fleet.trainFlags[head] = (int8_t)(0xC0 | (engine_type >= 3 ? 1 : 0));
                gc->fleet.wagonEnergyRemaining[head] = (int8_t)(8 - engine_type);
                gc->fleet.trainEraOrStage[head] = (int8_t)engine_type;
                gc->fleet.trainAgeOrTimer[head] = 0;
                int car = gc->fleet.nextCar[head];
                while (car != -1) {
                    gc->fleet.trainFlags[car] = 0;
                    car = gc->fleet.nextCar[car];
                }
                int previous_car = head;
                for (int slot = 0; slot < 5; ++slot) {
                    int car_code = (gc->fleet.trainStateCode[head] >> (3 + 3 * slot)) & 7;
                    if (car_code >= 7) continue;
                    int car_type = 2 + car_code;
                    if (car_type == 5) ++car_type;
                    previous_car = gc_spawnTrainCar(gc, previous_car, car_type);
                }
                gc->fleet.trainStateCode[head] = 7;
                gc->lastViewportCenterX = -1;
            }

            gc_loadTrainAtServiceVehicle(gc, head, service_vehicle, false);
            if (gc->fleet.trainAiOverride[head] == 1
                && gc_trainCanLoadAtServiceVehicle(gc, head, service_vehicle)) {
                gc->fleet.trainPhase[head] = 3;
                stop_chain = 1;
            }
            if (service_vehicle != gc->fleet.lastServiceVehicle[head]) {
                if (revenue > 0) {
                    gc->economy.money += revenue;
                    gc_pushMoneyHistory(gc, revenue);
                    gc->totalRevenue += revenue;
                    gc->fleet.trainRevenue[head] += revenue;
                    gc_recordTrainRevenue(gc, revenue);
                }
                gc->fleet.trainEnergyOrCount[head] =
                    (int8_t)(gc_countCarsInTrain(gc, head) * 40);
            }
            gc->fleet.lastServiceVehicle[head] = (int8_t)service_vehicle;
            gc->fleet.trainFlags[head] = (int8_t)(gc->fleet.trainFlags[head] | 0x10);
        }
        if ((gc->fleet.trainFlags[head] & 0x40) == 64 && gc->fleet.trainPhase[head] == 4
            && at_service_tile && gc->world.trackMap[old_x][old_y] != 65) {
            gc->fleet.trainEnergyOrCount[head] =
                (int8_t)(gc_countCarsInTrain(gc, head) * 40);
        }

        int collision = 0;
        if (gc->fleet.trainPhase[head] < 8) {
            ++gc->fleet.trainPhase[head];
        } else {
            if (gc_byteToUnsignedInt(gc, gc->fleet.cargoAge[head]) < 200) ++gc->fleet.cargoAge[head];
            old_x = gc->fleet.trainX[head];
            old_y = gc->fleet.trainY[head];
            int old_dir = gc->fleet.trainDir[head];
            gc->fleet.trainPhase[head] = 0;

            int next_x = gc_byteToUnsignedInt(gc, gc->fleet.trainX[head]);
            int next_y = gc_byteToUnsignedInt(gc, gc->fleet.trainY[head]);
            int current_track = gc->world.trackMap[next_x][next_y];
            int current_variant = gc->world.tileMap[next_x][next_y] - 70;
            if (current_variant == 6 && (gc->fleet.trainDir[head] == 0
                                         || gc->fleet.trainDir[head] == 1)) {
                current_track = 0x40 | (current_track & 7);
                current_variant = 0;
            }
            if (current_variant == 6 && (gc->fleet.trainDir[head] == 2
                                         || gc->fleet.trainDir[head] == 3)) {
                current_track = 0x40 | ((current_track & 0x38) >> 3);
                current_variant = 1;
            }
            current_variant = normalize_track_variant(current_variant);
            const std::array<int8_t, 12> &step =
                gc->fleet.forwardTrackSteps[current_variant][0] == gc->fleet.trainDir[head]
                ? gc->fleet.forwardTrackSteps[current_variant]
                : gc->fleet.reverseTrackSteps[current_variant];
            next_x += step[2];
            next_y += step[3];
            if (next_x >= 0 && next_y >= 0
                && next_x < gc->world.mapWidth && next_y < gc->world.mapHeight) {
                bool moved = false;
                int next_track = gc->world.trackMap[next_x][next_y];
                int next_variant = normalize_track_variant(gc->world.tileMap[next_x][next_y] - 70);
                for (int pair = 2; pair < static_cast<int>(step.size()) / 2; ++pair) {
                    if (step[2 * pair] != next_variant) continue;
                    int next_dir = step[2 * pair + 1];
                    if (next_variant == 6) {
                        if (next_dir == 0 || next_dir == 1)
                            next_track = 0x40 | (next_track & 7);
                        if (next_dir == 2 || next_dir == 3)
                            next_track = 0x40 | ((next_track & 0x38) >> 3);
                    }
                    if (current_track != next_track) break;
                    gc->fleet.trainX[head] = (int8_t)next_x;
                    gc->fleet.trainY[head] = (int8_t)next_y;
                    gc->fleet.trainDir[head] = (int8_t)next_dir;
                    moved = true;
                }
                if (!moved) {
                    gc->fleet.trainDir[head] = step[1];
                    gc_resetTrailingCars(gc, head, gc->fleet.nextCar[head], 0);
                }
                if (moved && (gc->fleet.trainFlags[head] & 0x40) == 64
                    && next_variant == 6) {
                    bool occupied = false;
                    for (int other = 0; other < 500; ++other) {
                        if ((gc->fleet.trainFlags[other] & 0x80) == 128 && other != head
                            && gc->fleet.trainX[other] == gc->fleet.trainX[head]
                            && gc->fleet.trainY[other] == gc->fleet.trainY[head]) {
                            occupied = true;
                            break;
                        }
                    }
                    if (occupied) {
                        gc->fleet.trainX[head] = (int8_t)old_x;
                        gc->fleet.trainY[head] = (int8_t)old_y;
                        gc->fleet.trainDir[head] = (int8_t)old_dir;
                        gc->fleet.trainPhase[head] = 8;
                        collision = 1;
                    }
                }
            } else {
                if (gc->fleet.forwardTrackSteps[current_variant][0] == gc->fleet.trainDir[head])
                    gc->fleet.trainDir[head] = gc->fleet.forwardTrackSteps[current_variant][1];
                else if (gc->fleet.reverseTrackSteps[current_variant][0] == gc->fleet.trainDir[head])
                    gc->fleet.trainDir[head] = gc->fleet.reverseTrackSteps[current_variant][1];
                gc_resetTrailingCars(gc, head, gc->fleet.nextCar[head], 0);
            }
        }
        if (stop_chain == 0 && collision == 0 && gc->fleet.nextCar[head] != -1)
            gc_updateTrain(gc, gc->fleet.nextCar[head]);
    }
    if ((gc->fleet.trainFlags[head] & 0x40) == 64) {
        int energy = gc_byteToUnsignedInt(gc, gc->fleet.trainEnergyOrCount[head]) - 3;
        if (energy < 0) energy = 0;
        gc->fleet.trainEnergyOrCount[head] = (int8_t)energy;
    }
}
