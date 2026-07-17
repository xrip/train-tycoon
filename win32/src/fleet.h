// Per-slot train records (see game/train_tick.cpp).
#ifndef FLEET_H
#define FLEET_H
#include "game_constants.h"
#include <cstdint>

namespace train_flag {
// Bit 0x10 is used by movement code, but its exact meaning is still open.
// Service-vehicle flags are a separate encoding and stay unnamed.
inline constexpr uint8_t USED_SLOT = 0x80;
inline constexpr uint8_t HEAD = 0x40;
inline constexpr uint8_t REMOVAL_PENDING = 0x20;
inline constexpr uint8_t TYPE_MASK = 0x0F;

constexpr bool isUsed(int8_t flags) {
    return (static_cast<uint8_t>(flags) & USED_SLOT) != 0;
}

constexpr bool isHead(int8_t flags) {
    return (static_cast<uint8_t>(flags) & HEAD) != 0;
}

constexpr bool isRemovalPending(int8_t flags) {
    return (static_cast<uint8_t>(flags) & REMOVAL_PENDING) != 0;
}

constexpr int type(int8_t flags) {
    return static_cast<uint8_t>(flags) & TYPE_MASK;
}
} // namespace train_flag

// One stable train/car slot. Zero-filled defaults match the old arrays; in
// particular, nextCar stays 0 until allocation sets the -1 chain terminator.
struct TrainCar {
    int8_t x = 0;
    int8_t y = 0;
    int8_t direction = 0;
    int8_t phase = 0;
    int8_t flags = 0;
    int8_t era = 0;
    int8_t ageYears = 0;
    int8_t secondaryCargoOrBaseEnergy = 0;
    int8_t energyCargoOrRemovalTicks = 0;
    int8_t lastServiceSite = 0;
    int8_t cargoAge = 0;
    int16_t nextCar = 0;
    int32_t stateCode = 0;
    int8_t aiOverride = 0;
    int32_t revenue = 0;
    int32_t previousRevenue = 0;
};

// Fleet owns the per-train-slot records. Each slot id is one train/car; a train
// is a linked list of cars via nextCar (-1 terminator). Slot ids are stable
// across the save and the simulation, and the save byte order is unchanged.
struct Fleet {
    TrainCar trainCars[kTrainSlotCount]{};

    // Service-vehicle records (250-slot): the depots/stations where trains load
    // and unload. serviceResourceStock[site][5] is the per-resource stock;
    // serviceLoad/UnloadFlags are bitmasks of which resources to load/unload
    // (serviceLoadFlags is runtime-only, recomputed on load, so not saved).
    int8_t vehicleFlags[kServiceVehicleSlotCount]{};
    int8_t serviceVehicleX[kServiceVehicleSlotCount]{};
    int8_t serviceVehicleY[kServiceVehicleSlotCount]{};
    int16_t serviceResourceStock[kServiceVehicleSlotCount][kResourceTypeCount]{};
    int8_t serviceUnloadFlags[kServiceVehicleSlotCount]{};
    int8_t serviceLoadFlags[kServiceVehicleSlotCount]{};

    // Movement-rule lookup tables: for each track-shape variant, the [0] entry is
    // the entering train direction and [1] the exiting direction (forward/reverse
    // travel). Used by game_updateTrain to step trains along track. Constant,
    // populated at init, not saved.
    int8_t forwardTrackSteps[kTrackDirectionCount][kTrackStepValueCount]{};
    int8_t reverseTrackSteps[kTrackDirectionCount][kTrackStepValueCount]{};
};

#endif  // FLEET_H
