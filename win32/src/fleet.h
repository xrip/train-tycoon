// Per-slot train records (see game/train_tick.cpp).
#ifndef FLEET_H
#define FLEET_H
#include "game_constants.h"
#include <cstdint>

struct World;
struct Scenario;

namespace train_flag {
// MOVEMENT_STATE is used by movement code, but its exact meaning is still open.
// Service-vehicle flags are a separate encoding and stay unnamed.
inline constexpr uint8_t USED_SLOT = 0x80;
inline constexpr uint8_t HEAD = 0x40;
inline constexpr uint8_t REMOVAL_PENDING = 0x20;
inline constexpr uint8_t MOVEMENT_STATE = 0x10;
inline constexpr uint8_t TYPE_MASK = 0x0F;
inline constexpr uint8_t NON_TYPE_MASK = 0xF0;
inline constexpr uint8_t CLEAR_MOVEMENT_STATE_MASK = 0xEF;

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

namespace train_chain {
// A train is a linked list of car slots. This sentinel is stored in nextCar
// and is not a valid slot id.
inline constexpr int32_t kEnd = -1;
} // namespace train_chain

// One stable train/car slot. Zero-filled defaults match the old arrays; in
// particular, nextCar stays 0 until allocation sets the train_chain::kEnd
// terminator.
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

// One fixed depot or station slot. loadFlags is rebuilt after load and is not
// saved; every member keeps the old zero-filled default.
struct ServiceSite {
    int8_t flags = 0;
    int8_t x = 0;
    int8_t y = 0;
    int16_t resourceStock[kResourceTypeCount]{};
    int8_t unloadFlags = 0;
    int8_t loadFlags = 0;
};

struct UnloadResult {
    int32_t revenue = 0;
    int32_t delivered[kDeliveredResourceCount]{};
};

struct TrackStepTransition {
    int8_t nextTileVariant = 0;
    int8_t nextDirection = 0;
};

// One movement row for a normalized track variant. The first two values are
// the entering and fallback directions, followed by the tile step and four
// possible next-variant transitions. This is runtime data, not save data.
struct TrackStepEntry {
    int8_t enteringDirection = 0;
    int8_t fallbackDirection = 0;
    int8_t stepX = 0;
    int8_t stepY = 0;
    TrackStepTransition transitions[kTrackTransitionCount]{};
};

static_assert(sizeof(TrackStepEntry)
              == sizeof(int8_t) * kTrackStepValueCount);

// Fleet owns the per-train-slot records. Each slot id is one train/car; a train
// is a linked list of cars via nextCar (train_chain::kEnd terminator). Slot ids are stable
// across the save and the simulation, and the save byte order is unchanged.
struct Fleet {
    int32_t allocate_service_site(int32_t type, int32_t x, int32_t y,
                                  bool horizontal, bool extended);
    int32_t allocate_train_car(int32_t x, int32_t y, int32_t engine_type,
                               int32_t era, int32_t direction, int32_t energy);
    bool has_car_at(int32_t car, int32_t x, int32_t y) const;
    int32_t find_train_head_at(int32_t x, int32_t y) const;
    int32_t find_service_site_at(int32_t x, int32_t y) const;
    int32_t count_cars(int32_t car) const;
    int32_t append_car(int32_t previous_car, int32_t car_type);
    void age_one_year();
    void roll_revenue_year();
    void reset_trailing_cars(int32_t head, int32_t car, int32_t phase);
    int32_t take_service_resource(int32_t site, int32_t resource,
                                  int32_t wanted);
    int32_t collect_nearby_service_resource(int32_t site, int32_t resource,
                                             int32_t wanted);
    bool can_load_at_service_site(int32_t head, int32_t site) const;
    int32_t accumulate_turns(const World& world, int32_t car) const;
    void load_at_service_site(World& world, int32_t car, int32_t site,
                              bool construction_done);
    UnloadResult unload_at_service_site(const Scenario& scenario,
                                        int32_t car, int32_t site);

    TrainCar trainCars[kTrainSlotCount]{};

    // Service-vehicle records (250-slot): the depots/stations where trains load
    // and unload. serviceResourceStock[site][5] is the per-resource stock;
    // serviceLoad/UnloadFlags are bitmasks of which resources to load/unload
    // (serviceLoadFlags is runtime-only, recomputed on load, so not saved).
    ServiceSite serviceSites[kServiceVehicleSlotCount]{};

    // Movement-rule lookup tables for forward/reverse travel. Constant,
    // populated at init, not saved.
    TrackStepEntry forwardTrackSteps[kTrackDirectionCount]{};
    TrackStepEntry reverseTrackSteps[kTrackDirectionCount]{};
};

#endif  // FLEET_H
