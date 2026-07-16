// Per-slot train records (see game/train_tick.cpp).
#ifndef FLEET_H
#define FLEET_H
#include "game_constants.h"
#include <cstdint>

// Fleet owns the per-train-slot records. Each slot id is one train/car; a train
// is a linked list of cars via nextCar (-1 terminator). Slot ids are stable
// across the save and the simulation, and the save byte order is unchanged.
//
// Field meanings are inherited as-is from the prior Game layout; several
// (e.g. trainEra, trainAgeYears) carry recorded uncertainty and are
// recovered in a later FLEET-01 sub-leaf once the tick code is re-read.
struct Fleet {
    int8_t trainX[kTrainSlotCount]{};
    int8_t trainY[kTrainSlotCount]{};
    int8_t trainDir[kTrainSlotCount]{};
    int8_t trainPhase[kTrainSlotCount]{};
    int8_t trainFlags[kTrainSlotCount]{};
    int8_t trainEra[kTrainSlotCount]{};
    int8_t trainAgeYears[kTrainSlotCount]{};
    int8_t secondaryCargoOrBaseEnergy[kTrainSlotCount]{};
    int8_t energyCargoOrRemovalTicks[kTrainSlotCount]{};
    int8_t lastServiceVehicle[kTrainSlotCount]{};
    int8_t cargoAge[kTrainSlotCount]{};
    int16_t nextCar[kTrainSlotCount]{};
    int32_t trainStateCode[kTrainSlotCount]{};
    int8_t trainAiOverride[kTrainSlotCount]{};
    int32_t trainRevenue[kTrainSlotCount]{};
    int32_t prevTrainRevenue[kTrainSlotCount]{};

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
