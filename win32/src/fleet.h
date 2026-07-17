// Per-slot train records (see game/train_tick.cpp).
#ifndef FLEET_H
#define FLEET_H
#include "game_constants.h"
#include <cstdint>

struct World;
struct Scenario;

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
    int32_t delivered[kResourceTypeCount]{};
};

// Fleet owns the per-train-slot records. Each slot id is one train/car; a train
// is a linked list of cars via nextCar (-1 terminator). Slot ids are stable
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

    // Movement-rule lookup tables: for each track-shape variant, the [0] entry is
    // the entering train direction and [1] the exiting direction (forward/reverse
    // travel). Used by Game::update_train to step trains along track. Constant,
    // populated at init, not saved.
    int8_t forwardTrackSteps[kTrackDirectionCount][kTrackStepValueCount]{};
    int8_t reverseTrackSteps[kTrackDirectionCount][kTrackStepValueCount]{};
};

#endif  // FLEET_H
