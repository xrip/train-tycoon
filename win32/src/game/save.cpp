// Save-buffer reading, writing, and compression.
#include "game.h"
#include "support/byte_buffer.h"
#include "support/numeric.h"
#include "support/record_store.h"

#include <algorithm>
#include <stdlib.h>
#include <string.h>

// Read one byte from the save buffer as an unsigned value (0..255).
int32_t SaveCodec::read_unsigned_byte() {
    return saveBuffer[saveReadOffset] >= 0
               ? saveBuffer[saveReadOffset++]
               : 256 + saveBuffer[saveReadOffset++];
}

bool SaveCodec::io_bool(bool value) {
    return io_byte((int8_t)(value ? 1 : 0)) == 1;
}

int8_t SaveCodec::io_byte(int8_t value) {
    if (savePendingMode == save::MODE_SKIP) return 0;
    int8_t result = value;
    if (savePendingMode == save::MODE_WRITE) {
        ensure_capacity(1);
        saveBuffer[saveReadOffset++] = value;
    } else if (snapshotLoaded) {
        result = saveBuffer[saveReadOffset++];
    }
    return result;
}

int32_t SaveCodec::io_int(int32_t value) {
    if (savePendingMode == save::MODE_SKIP) return 0;
    int32_t result = value;
    if (savePendingMode == save::MODE_WRITE) {
        ensure_capacity(4);
        saveBuffer[saveReadOffset++] = static_cast<int8_t>(value & 0xFF);
        saveBuffer[saveReadOffset++] = static_cast<int8_t>(value >> 8 & 0xFF);
        saveBuffer[saveReadOffset++] = static_cast<int8_t>(value >> 16 & 0xFF);
        saveBuffer[saveReadOffset++] = static_cast<int8_t>(value >> 24 & 0xFF);
    } else if (snapshotLoaded) {
        uint32_t bits = (uint32_t)read_unsigned_byte();
        const uint32_t byte1 = (uint32_t)read_unsigned_byte();
        const uint32_t byte2 = (uint32_t)read_unsigned_byte();
        const uint32_t byte3 = (uint32_t)read_unsigned_byte();
        bits |= byte1 << 8;
        bits |= byte2 << 16;
        bits |= byte3 << 24;
        memcpy(&result, &bits, sizeof(result));
    }
    return result;
}

void SaveCodec::load_slot(int32_t slot, int32_t pending_mode) {
    saveSlot = slot;
    savePendingMode = pending_mode;
    saveReadOffset = 0;
    snapshotSize = 0;
    snapshotLoaded = false;
    if (savePendingMode == save::MODE_READ) {
        std::unique_ptr<RecordStore> store = RecordStore::open("TTStore", true);
        if (store) {
            const int record_count = store->record_count();
            for (int record_index = 0; record_index < record_count;
                 ++record_index) {
                const int record_id = store->record_id_at(record_index);
                const auto record = store->record(record_id);
                if (!record) continue;
                const int record_length = static_cast<int>(record->size());
                if (record_length > 0 && (int8_t)(*record)[0] == slot) {
                    saveBuffer.assign(
                        static_cast<std::size_t>(record_length - 1), 0);
                    memcpy(saveBuffer.data(), record->data() + 1,
                           static_cast<std::size_t>(record_length - 1));
                    snapshotSize = record_length - 1;
                    snapshotLoaded = true;
                }
            }
        }
    } else {
        snapshotLoaded = true;
    }
}

void SaveCodec::persist() const {
    if (savePendingMode != save::MODE_WRITE) return;
    std::unique_ptr<RecordStore> store = RecordStore::open("TTStore", true);
    if (!store) return;
    for (int record_index = store->record_count() - 1;
         record_index >= 0; --record_index) {
        const int record_id = store->record_id_at(record_index);
        const auto record = store->record(record_id);
        if (!record) continue;
        bool same_slot = !record->empty() && (int8_t)(*record)[0] == saveSlot;
        if (same_slot) store->delete_record(record_id);
    }
    std::vector<uint8_t> record(static_cast<std::size_t>(snapshotSize) + 1);
    record[0] = (uint8_t)saveSlot;
    memcpy(record.data() + 1, saveBuffer.data(), (size_t)snapshotSize);
    store->add_record(record);
}

namespace {

namespace save_compression {
constexpr int32_t kEscapeByte = 255;
constexpr int32_t kEscapeMarker = -1;
constexpr int32_t kSingleValueRunLength = 1;
constexpr int32_t kMaximumRunLength = 254;
}  // namespace save_compression

namespace road_vehicle_save {
constexpr int32_t kMagic = 0x52565431;  // "RVT1"
constexpr int32_t kLegacyVersion = 1;
constexpr int32_t kVersion = 2;  // mixed cargo counters
constexpr int32_t kHeaderBytes = 12;
constexpr int32_t kLegacyMinimumVehicleBytes = 34;
constexpr int32_t kMinimumVehicleBytes = 50;
}  // namespace road_vehicle_save

void write_rle_run(ByteBuffer& output, int32_t byte_value,
                   int32_t run_length) {
    if (run_length == save_compression::kSingleValueRunLength) {
        if (byte_value == save_compression::kEscapeMarker) {
            output.write(save_compression::kEscapeByte);
            output.write(save_compression::kSingleValueRunLength);
        }
        output.write(byte_value);
    } else {
        output.write(save_compression::kEscapeByte);
        output.write(run_length);
        output.write(byte_value);
    }
}

}  // namespace

void SaveCodec::compress() {
    ByteBuffer output;
    int byte_value = saveBuffer[0];
    int run_length = save_compression::kSingleValueRunLength;
    for (int buffer_index = save_compression::kSingleValueRunLength;
         buffer_index < snapshotSize; ++buffer_index) {
        if (byte_value != saveBuffer[buffer_index]
            || run_length == save_compression::kMaximumRunLength) {
            write_rle_run(output, byte_value, run_length);
            byte_value = saveBuffer[buffer_index];
            run_length = save_compression::kSingleValueRunLength;
        } else {
            ++run_length;
        }
    }
    write_rle_run(output, byte_value, run_length);
    bytesUncompressed += snapshotSize;
    const auto compressed = output.bytes();
    snapshotSize = static_cast<int>(compressed.size());
    saveBuffer.assign(static_cast<std::size_t>(snapshotSize), 0);
    memcpy(saveBuffer.data(), compressed.data(), (size_t)snapshotSize);
    bytesCompressed += snapshotSize;
}

void SaveCodec::decompress() {
    ByteBuffer output;
    for (int buffer_index = 0; buffer_index < snapshotSize; ++buffer_index) {
        if (saveBuffer[buffer_index] == save_compression::kEscapeMarker) {
            const int run_length =
                unsigned_byte(saveBuffer[buffer_index + 1]);
            for (int repeat_index = 0; repeat_index < run_length;
                 ++repeat_index)
                output.write(saveBuffer[buffer_index + 2]);
            buffer_index += 2;
        } else {
            output.write(saveBuffer[buffer_index]);
        }
    }
    bytesCompressed += snapshotSize;
    const auto decompressed = output.bytes();
    snapshotSize = static_cast<int>(decompressed.size());
    saveBuffer.assign(static_cast<std::size_t>(snapshotSize), 0);
    memcpy(saveBuffer.data(), decompressed.data(), (size_t)snapshotSize);
    bytesUncompressed += snapshotSize;
}

void SaveCodec::walk(Game& game_value, int32_t operation) {
    Game* game = &game_value;
    if (operation == save::OP_SAVE_MAIN)
        load_slot(save::slot::Main, save::MODE_WRITE);
    if (operation == save::OP_LOAD_MAIN)
        load_slot(save::slot::Main, save::MODE_READ);
    if (operation == save::OP_RESET_SKIP) savePendingMode = save::MODE_SKIP;
    if (operation == save::OP_MEMORY_WRITE) {
        snapshotSize = 0;
        saveBuffer.clear();
        savePendingMode = save::MODE_WRITE;
    }
    if (operation == save::OP_MEMORY_READ) {
        saveReadOffset = 0;
        savePendingMode = save::MODE_READ;
        snapshotLoaded = true;
    }
    if (operation == save::OP_LOAD_MAIN) decompress();

#define IO_INT(field) \
    (game->field = io_int(game->field))
#define IO_BYTE(field) \
    (game->field = io_byte(game->field))
#define IO_BOOL(field) \
    (game->field = io_bool(game->field))
#define FIELD_COUNT(field) static_cast<int>(sizeof(game->field) / sizeof(game->field[0]))
#define IO_BYTES(field) io_bytes(game->field, FIELD_COUNT(field))
#define IO_SHORTS(field) io_shorts(game->field, FIELD_COUNT(field))
#define IO_INTS(field) io_ints(game->field, FIELD_COUNT(field))
#define IO_CAR_BYTE(member) \
    for (TrainCar& car : game->fleet.trainCars) car.member = io_byte(car.member)
#define IO_CAR_SHORT(member) \
    for (TrainCar& car : game->fleet.trainCars) io_shorts(&car.member, 1)
#define IO_CAR_INT(member) \
    for (TrainCar& car : game->fleet.trainCars) car.member = io_int(car.member)
#define IO_SITE_BYTE(member) \
    for (ServiceSite& site : game->fleet.serviceSites) site.member = io_byte(site.member)
#define IO_CITY_BYTE(member) \
    for (City& city : game->world.cities) city.member = io_byte(city.member)
#define IO_CITY_SHORT(member) \
    for (City& city : game->world.cities) io_shorts(&city.member, 1)
#define IO_OFFER_BYTE(member) \
    for (Offer& offer : game->offerBoard.offers) offer.member = io_byte(offer.member)
#define IO_OFFER_INT(member) \
    for (Offer& offer : game->offerBoard.offers) offer.member = io_int(offer.member)

    IO_INT(world.terrain.mapWidth);
    IO_INT(world.terrain.mapHeight);
    IO_INT(renderer.camera.viewportCenterX);
    IO_INT(renderer.camera.viewportCenterY);
    IO_INT(economy.money);
    IO_INT(economy.dayOfMonth);
    IO_INT(economy.monthOfYear);
    IO_INT(economy.year);
    IO_INT(dayFrameCounter);
    IO_INT(renderer.camera.viewportBaseX);
    IO_INT(renderer.camera.viewportBaseY);
    IO_INT(renderer.camera.mapScaleX);
    IO_INT(renderer.camera.mapScaleY);
    IO_SHORTS(renderer.visibleTrains);
    IO_INT(renderer.visibleTrainCount);
    IO_INT(renderer.viewportX0);
    IO_INT(renderer.viewportY0);
    IO_INT(renderer.viewportX1);
    IO_INT(renderer.viewportY1);
    IO_INT(economy.loanBalance);
    IO_INT(economy.loanCeiling);
    IO_BYTE(economy.interestRate);
    IO_INT(economy.enterpriseTaxPct);
    IO_INTS(economy.yearProfitHistory);
    IO_INTS(economy.yearRevenueHistory);
    IO_INTS(economy.yearCompanyValueHistory);
    IO_INTS(economy.decadeProfitHistory);
    IO_INTS(economy.altDecadeRevenueTotal);
    IO_INTS(economy.altDecadeTotalCost);
    IO_CAR_INT(revenue);
    IO_CAR_INT(previousRevenue);
    IO_CITY_SHORT(stationRevenue);
    IO_CITY_SHORT(previousStationRevenue);
    IO_INT(economy.revenueTotal);
    IO_INT(economy.decadeProfitGraphTotal);
    IO_INT(economy.totalSpent);
    IO_INT(economy.aiMoneySpent);
    IO_INT(economy.fuelCostAccrued);
    IO_INT(economy.subsidyCost);
    IO_INT(economy.totalRevenue);
    IO_INT(economy.terrainTotal);
    IO_INT(economy.decadeEnterpriseTax);
    IO_INT(economy.netProfit);
    IO_INT(economy.prevTotalSpent);
    IO_INT(economy.prevAiMoneySpent);
    IO_INT(economy.prevFuelCost);
    IO_INT(economy.prevSubsidyCost);
    IO_INT(economy.prevTotalRevenue);
    IO_INT(economy.prevTerrainTotal);
    IO_INT(economy.prevEnterpriseTotal);
    IO_INT(economy.previousYearProfit);
    IO_INT(economy.prevDecadeSpent);
    IO_INT(economy.prevDecadeAiSpent);
    IO_INT(economy.prevDecadeFuel);
    IO_INT(economy.prevDecadeSubsidy);
    IO_INT(economy.prevDecadeRevenue);
    IO_INT(economy.previousLegacyDecadeMetric);
    IO_INT(economy.previousDecadeEnterpriseTax);
    IO_INT(economy.previousDecadeProfit);
    IO_INT(economy.decadeCostTotal);
    IO_INT(economy.decadeSpentTotal);
    IO_INT(economy.decadeFuelCost);
    IO_INT(economy.decadeSubsidyTotal);
    IO_INT(economy.decadeRevenueTotal);
    IO_INT(economy.savedLegacyMetricEb);
    IO_INT(economy.decadeEnterpriseTotal);
    IO_INT(economy.decadeProfitTotal);
    IO_BYTE(ui.cityListIndex);
    IO_CITY_BYTE(stationDirection);
    IO_CITY_BYTE(population);
    IO_CITY_BYTE(zoneTileCount);
    IO_SITE_BYTE(flags);
    IO_SITE_BYTE(x);
    IO_SITE_BYTE(y);
    IO_SITE_BYTE(unloadFlags);
    for (ServiceSite& site : game->fleet.serviceSites)
        io_shorts(site.resourceStock, kResourceTypeCount);
    IO_BYTE(ui.numSelectedTrains);
    IO_BOOL(scenario.aiExpansionEnabled);
    IO_BYTES(world.constructionTimers);
    IO_SHORTS(world.zonePopulation);
    IO_SHORTS(ui.trainSelectionIndex);
    IO_SHORTS(ui.trainSelectionY);
    IO_SHORTS(ui.trainSelectionX);
    IO_INT(scenario.currentEra);
    IO_CAR_BYTE(x);
    IO_CAR_BYTE(y);
    IO_CAR_BYTE(direction);
    IO_CAR_BYTE(phase);
    IO_CAR_BYTE(flags);
    IO_CAR_BYTE(secondaryCargoOrBaseEnergy);
    IO_CAR_BYTE(energyCargoOrRemovalTicks);
    IO_CAR_BYTE(lastServiceSite);
    IO_CAR_BYTE(cargoAge);
    IO_CAR_SHORT(nextCar);
    IO_CAR_BYTE(era);
    IO_CAR_BYTE(ageYears);
    IO_CAR_INT(stateCode);
    IO_CAR_BYTE(aiOverride);
    IO_INT(world.industryCategory);
    IO_BOOL(world.industrySiteFound);
    IO_INT(build.nextTrackPieceCode);
    if (game->build.nextTrackPieceCode == 0)
        game->build.nextTrackPieceCode = track::AI_PIECE_FIRST;
    IO_BYTE(scenario.aiFrequency);
    IO_BYTE(scenario.aiThreshold);
    if (operation != save::OP_RESET_SKIP) IO_BYTE(scenario.difficultyCycle);
    IO_OFFER_BYTE(objectiveType);
    IO_OFFER_BYTE(issuedDay);
    IO_OFFER_BYTE(dueDay);
    IO_OFFER_BYTE(issuedMonth);
    IO_OFFER_BYTE(dueMonth);
    IO_OFFER_BYTE(issuedYear);
    IO_OFFER_BYTE(dueYear);
    IO_OFFER_INT(target);
    IO_OFFER_INT(progress);
    IO_OFFER_BYTE(effectType);
    IO_OFFER_BYTE(effectValue);
    IO_OFFER_INT(effectAmount);
    IO_SHORTS(scenario.offerEffectNextCount);
    IO_INT(scenario.offerGenerationCount);
    IO_INT(savedLegacyMetricBq);
    IO_INT(scenario.lastOfferDateCode);
    IO_BOOL(scenario.monthlyOffersEnabled);
    IO_BYTE(scenario.scenarioBits);
    IO_BYTE(scenario.scenarioMilestoneIndex);
    IO_BOOL(scenario.milestonesEnabled);
    IO_INT(scenario.currentMilestoneScore);
    IO_INT(scenario.milestoneCoThreshold2);
    IO_INT(economy.lifetimeTrainRevenue);
    IO_INT(scenario.milestoneThreshold1);
    IO_INT(scenario.milestoneThreshold2);
    IO_INT(scenario.milestoneThreshold3);
    IO_INT(scenario.milestoneThreshold4);
    IO_INT(scenario.milestoneThreshold5);
    IO_INT(scenario.aiWagonsBought);
    IO_INT(economy.trainRevenueScore);
    IO_INT(scenario.scenarioStep);
    IO_INT(scenario.scenarioNoticeId);
    IO_INT(economy.moneyBasePrimary);
    IO_INT(economy.moneyBaseSecondary);
    IO_INT(economy.baseTrainSpeedEarly);
    IO_INT(economy.accelerationEarly);
    IO_INT(economy.brakingEarly);
    IO_INT(economy.baseTrainSpeedLate);
    IO_INT(economy.accelerationLate);
    IO_INT(economy.powerCost);
    IO_INT(economy.structureCost);
    IO_INT(economy.overheadCost);
    IO_INT(economy.maintenanceCost);
    IO_INT(economy.crewCost);
    IO_INT(economy.trainCostFirstEra);
    IO_INT(economy.trainCostSecondEra);
    IO_INT(economy.trainCostThirdEra);
    IO_INT(economy.trainCostFourthEra);
    IO_INT(economy.trainCostFifthEra);
    IO_INT(economy.trainCostSixthEra);
    IO_INT(economy.wagonOptionCostPassengerMail);
    IO_INT(economy.wagonOptionCostIndustryGroup1);
    IO_INT(economy.wagonOptionCostIndustryGroup2);
    IO_INT(economy.wagonOptionCostIndustryGroup3);

#undef IO_INT
#undef IO_BYTE
#undef IO_BOOL
#undef IO_BYTES
#undef IO_SHORTS
#undef IO_INTS
#undef IO_CAR_BYTE
#undef IO_CAR_SHORT
#undef IO_CAR_INT
#undef IO_SITE_BYTE
#undef IO_CITY_BYTE
#undef IO_CITY_SHORT
#undef IO_OFFER_BYTE
#undef IO_OFFER_INT
#undef FIELD_COUNT

    if (operation == save::OP_SAVE_MAIN) {
        compress();
        persist();
    }
}

void SaveCodec::walk_settings(Game& game) {
    constexpr int kExpectedBytes = 4 + 3 * 4
        + kScenarioCount * 4 + kLeaderboardScoreCount * 4
        + kPlayerNameCodeCount;
    if (savePendingMode == save::MODE_READ
        && (!snapshotLoaded || snapshotSize != kExpectedBytes))
        return;
    game.settings.animationsDisabled = io_bool(game.settings.animationsDisabled);
    game.settings.selectionHidden = io_bool(game.settings.selectionHidden);
    game.ui.monthlyReportEnabled = io_bool(game.ui.monthlyReportEnabled);
    game.ui.annualReportEnabled = io_bool(game.ui.annualReportEnabled);
    game.scenario.scenarioStepCompare = io_int(game.scenario.scenarioStepCompare);
    game.scenario.bestScenarioScore = io_int(game.scenario.bestScenarioScore);
    game.music.volumeLevel = io_int(game.music.volumeLevel);
    io_ints(game.scenario.scenarioScores, kScenarioCount);
    io_ints(game.scenario.leaderboardScores, kLeaderboardScoreCount);
    io_bytes(game.scenario.playerNameCodes, kPlayerNameCodeCount);
}

void SaveCodec::walk_road_vehicles(Game& game) {
    // Roads and stops already persist (tiles in slot 1, sites via walk()); only
    // the vehicles and their route definition need saving. The tile path is a
    // derived cache, so we save just the ordered stop-site list and rebuild the
    // route with expand_road_route on load.
    if (savePendingMode == save::MODE_READ
        && (!snapshotLoaded
            || snapshotSize < road_vehicle_save::kHeaderBytes)) {
        game.roadVehicles.clear();
        return;
    }

    const int magic = io_int(road_vehicle_save::kMagic);
    const int version = io_int(road_vehicle_save::kVersion);
    if (savePendingMode == save::MODE_READ
        && (magic != road_vehicle_save::kMagic
            || (version != road_vehicle_save::kLegacyVersion
                && version != road_vehicle_save::kVersion))) {
        game.roadVehicles.clear();
        return;
    }
    const int minimum_vehicle_bytes =
        version == road_vehicle_save::kLegacyVersion
            ? road_vehicle_save::kLegacyMinimumVehicleBytes
            : road_vehicle_save::kMinimumVehicleBytes;

    int vehicle_count = static_cast<int>(game.roadVehicles.size());
    vehicle_count = io_int(vehicle_count);
    if (savePendingMode == save::MODE_READ) {
        const int remaining_bytes = snapshotSize - saveReadOffset;
        if (vehicle_count < 0
            || vehicle_count > remaining_bytes / minimum_vehicle_bytes) {
            game.roadVehicles.clear();
            return;
        }
        game.roadVehicles.assign(static_cast<std::size_t>(vehicle_count),
                                 RoadVehicle{});
    }
    const int available_vehicle_count =
        static_cast<int>(game.roadVehicles.size());
    for (int vehicle_index = 0;
         vehicle_index < vehicle_count
         && vehicle_index < available_vehicle_count; ++vehicle_index) {
        if (savePendingMode == save::MODE_READ
            && snapshotSize - saveReadOffset
                   < minimum_vehicle_bytes) {
            game.roadVehicles.clear();
            return;
        }
        RoadVehicle& vehicle =
            game.roadVehicles[static_cast<std::size_t>(vehicle_index)];
        vehicle.carType = io_int(vehicle.carType);
        vehicle.capacity = io_int(vehicle.capacity);
        if (version == road_vehicle_save::kLegacyVersion) {
            const int legacy_cargo = io_int(0);
            int legacy_resource = -1;
            if (vehicle.carType == road_vehicle_type::Bus) {
                legacy_resource = service_resource::Passengers;
            } else if (vehicle.carType == wagon::IndustryGroup1Car) {
                legacy_resource = service_resource::IndustryGroup1;
                vehicle.carType = road_vehicle_type::Truck;
            } else if (vehicle.carType == wagon::IndustryGroup2CarA) {
                legacy_resource = service_resource::IndustryGroup2;
                vehicle.carType = road_vehicle_type::Truck;
            } else if (vehicle.carType == wagon::IndustryGroup3CarA) {
                legacy_resource = service_resource::IndustryGroup3;
                vehicle.carType = road_vehicle_type::Truck;
            }
            vehicle.cargo.fill(0);
            if (legacy_resource >= 0)
                vehicle.cargo[legacy_resource] = legacy_cargo;
        } else {
            for (int resource = 0; resource < kResourceTypeCount; ++resource)
                vehicle.cargo[resource] = io_int(vehicle.cargo[resource]);
        }
        vehicle.revenue = io_int(vehicle.revenue);
        vehicle.segment = io_int(vehicle.segment);
        vehicle.phase = io_int(vehicle.phase);
        vehicle.direction = io_int(vehicle.direction);
        vehicle.forward = io_bool(vehicle.forward);
        vehicle.loop = io_bool(vehicle.loop);
        int stop_count = static_cast<int>(vehicle.stopSites.size());
        stop_count = io_int(stop_count);
        if (savePendingMode == save::MODE_READ) {
            if (stop_count < 0 || stop_count > kServiceVehicleSlotCount
                || stop_count > (snapshotSize - saveReadOffset) / 4) {
                game.roadVehicles.clear();
                return;
            }
            vehicle.stopSites.assign(static_cast<std::size_t>(stop_count), 0);
        }
        const int available_stop_count =
            static_cast<int>(vehicle.stopSites.size());
        for (int stop_index = 0;
             stop_index < stop_count && stop_index < available_stop_count;
             ++stop_index) {
            vehicle.stopSites[static_cast<std::size_t>(stop_index)] =
                io_int(vehicle.stopSites[static_cast<std::size_t>(stop_index)]);
        }
    }
    if (savePendingMode == save::MODE_READ) {
        for (RoadVehicle& vehicle : game.roadVehicles) {
            const bool is_bus = vehicle.carType == road_vehicle_type::Bus;
            const bool is_truck = vehicle.carType == road_vehicle_type::Truck;
            bool valid = (is_bus || is_truck)
                && vehicle.capacity == (is_bus ? kBusCapacity : kTruckCapacity)
                && vehicle.phase >= 0
                && vehicle.phase < RoadVehicle::kStepsPerTile
                && vehicle.direction >= road_direction::PositiveX
                && vehicle.direction <= road_direction::NegativeY
                && vehicle.stopSites.size() >= 2;
            int cargo_total = 0;
            if (valid) {
                for (int resource = 0; resource < kResourceTypeCount; ++resource) {
                    const int amount = vehicle.cargo[resource];
                    if (amount < 0 || amount > vehicle.capacity
                        || (!vehicle.accepts_resource(resource) && amount != 0)
                        || cargo_total > vehicle.capacity - amount) {
                        valid = false;
                        break;
                    }
                    cargo_total += amount;
                }
            }
            for (int stop_slot : vehicle.stopSites) {
                if (stop_slot < 0 || stop_slot >= kServiceVehicleSlotCount) {
                    valid = false;
                    break;
                }
                const ServiceSite& site = game.fleet.serviceSites[stop_slot];
                if ((site.flags & service_site::USED_SLOT) == 0
                    || (site.flags & service_site::TYPE_MASK)
                           != kRoadStopSiteType) {
                    valid = false;
                    break;
                }
            }
            if (valid)
                vehicle.route =
                    game.expand_road_route(vehicle.stopSites, vehicle.loop);
            if (!valid || !vehicle.active()) vehicle.route.clear();
        }
        game.roadVehicles.erase(
            std::remove_if(game.roadVehicles.begin(), game.roadVehicles.end(),
                           [](const RoadVehicle& vehicle) {
                               return !vehicle.active();
                           }),
            game.roadVehicles.end());
    }
}

void SaveCodec::save_all(Game& game) {
    bytesUncompressed = 0;
    bytesCompressed = 0;
    walk(game, save::OP_SAVE_MAIN);
    load_slot(save::slot::TileMap, save::MODE_WRITE);
    io_byte_grid(game.world.terrain.tileMap);
    compress();
    persist();
    load_slot(save::slot::TrackMap, save::MODE_WRITE);
    io_byte_grid(game.world.trackMap);
    compress();
    persist();
    load_slot(save::slot::TileElevation, save::MODE_WRITE);
    io_byte_grid(game.world.terrain.tileElevation);
    compress();
    persist();
    load_slot(save::slot::HeightMap, save::MODE_WRITE);
    io_byte_grid(game.world.terrain.heightMap);
    compress();
    persist();
    load_slot(save::slot::RoadVehicles, save::MODE_WRITE);
    walk_road_vehicles(game);
    compress();
    persist();
}

void SaveCodec::restore_all(Game& game) {
    bytesUncompressed = 0;
    bytesCompressed = 0;
    walk(game, save::OP_LOAD_MAIN);
    game.world.resize(game.world.terrain.mapWidth, game.world.terrain.mapHeight);
    game.renderer.camera.viewportHalfWidth = game.world.terrain.mapScreenWidth / 2;
    game.renderer.camera.viewportHalfHeight = game.world.terrain.mapScreenHeight / 2;
    game.ui.init_build_menu(game.scenario, build_menu::kNoScenarioOverride);
    load_slot(save::slot::TileMap, save::MODE_READ);
    decompress();
    io_byte_grid(game.world.terrain.tileMap);
    load_slot(save::slot::TrackMap, save::MODE_READ);
    decompress();
    io_byte_grid(game.world.trackMap);
    load_slot(save::slot::TileElevation, save::MODE_READ);
    decompress();
    io_byte_grid(game.world.terrain.tileElevation);
    load_slot(save::slot::HeightMap, save::MODE_READ);
    decompress();
    io_byte_grid(game.world.terrain.heightMap);
    // Roads live in tileMap (codes 42..48, restored above), so rebuild the
    // derived road layer + components after the tiles are back.
    game.roadNet.rebuild_from_tiles(game.world.terrain);
    // Road vehicles restore last: their routes are rebuilt from the stop list
    // over the now-current road layer and service sites.
    load_slot(save::slot::RoadVehicles, save::MODE_READ);
    decompress();
    walk_road_vehicles(game);
    game.renderer.lastViewportCenterX = -1;
}
