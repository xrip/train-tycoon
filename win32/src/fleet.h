// Per-slot train records (see game/train_tick.cpp).
#ifndef FLEET_H
#define FLEET_H
#include <array>
#include <cstdint>

// Fleet owns the per-train-slot records. Each slot id is one train/car; a train
// is a linked list of cars via nextCar (-1 terminator). Slot ids are stable
// across the save and the simulation, and the save byte order is unchanged.
//
// Field meanings are inherited as-is from the prior GameCanvas layout; several
// (e.g. trainEraOrStage, trainAgeOrTimer) carry recorded uncertainty and are
// recovered in a later FLEET-01 sub-leaf once the tick code is re-read.
struct Fleet {
    std::array<int8_t, 500> trainX{};
    std::array<int8_t, 500> trainY{};
    std::array<int8_t, 500> trainDir{};
    std::array<int8_t, 500> trainPhase{};
    std::array<int8_t, 500> trainFlags{};
    std::array<int8_t, 500> trainEraOrStage{};
    std::array<int8_t, 500> trainAgeOrTimer{};
    std::array<int8_t, 500> wagonEnergyRemaining{};
    std::array<int8_t, 500> trainEnergyOrCount{};
    std::array<int8_t, 500> lastServiceVehicle{};
    std::array<int8_t, 500> cargoAge{};
    std::array<int16_t, 500> nextCar{};
    std::array<int32_t, 500> trainStateCode{};
    std::array<int8_t, 500> trainAiOverride{};
    std::array<int32_t, 500> trainRevenue{};
    std::array<int32_t, 500> prevTrainRevenue{};

    // Service-vehicle records (250-slot): the depots/stations where trains load
    // and unload. serviceResourceStock[site][5] is the per-resource stock;
    // serviceLoad/UnloadFlags are bitmasks of which resources to load/unload
    // (serviceLoadFlags is runtime-only, recomputed on load, so not saved).
    std::array<int8_t, 250> vehicleFlags{};
    std::array<int8_t, 250> serviceVehicleX{};
    std::array<int8_t, 250> serviceVehicleY{};
    std::array<std::array<int16_t, 5>, 250> serviceResourceStock{};
    std::array<int8_t, 250> serviceUnloadFlags{};
    std::array<int8_t, 250> serviceLoadFlags{};

    // Movement-rule lookup tables: for each track-shape variant, the [0] entry is
    // the entering train direction and [1] the exiting direction (forward/reverse
    // travel). Used by gc_updateTrain to step trains along track. Constant,
    // populated at init, not saved.
    std::array<std::array<int8_t, 12>, 6> forwardTrackSteps{};
    std::array<std::array<int8_t, 12>, 6> reverseTrackSteps{};
};

#endif  // FLEET_H
