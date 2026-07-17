// Save-buffer reading, writing, and compression.
#include "game_internal.h"
#include "support/record_store.h"

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

int8_t SaveCodec::io_byte(int8_t by) {
    if (savePendingMode == save::MODE_SKIP) return 0;
    int8_t by2 = by;
    if (savePendingMode == save::MODE_WRITE) {
        ensure_capacity(1);
        saveBuffer[saveReadOffset++] = by;
    } else if (snapshotLoaded) {
        by2 = saveBuffer[saveReadOffset++];
    }
    return by2;
}

int32_t SaveCodec::io_int(int32_t n) {
    if (savePendingMode == save::MODE_SKIP) return 0;
    int32_t n2 = n;
    if (savePendingMode == save::MODE_WRITE) {
        ensure_capacity(4);
        saveBuffer[saveReadOffset++] = (int8_t)(n & 0xFF);
        saveBuffer[saveReadOffset++] = (int8_t)(n >> 8 & 0xFF);
        saveBuffer[saveReadOffset++] = (int8_t)(n >> 16 & 0xFF);
        saveBuffer[saveReadOffset++] = (int8_t)(n >> 24 & 0xFF);
    } else if (snapshotLoaded) {
        uint32_t bits = (uint32_t)read_unsigned_byte();
        const uint32_t byte1 = (uint32_t)read_unsigned_byte();
        const uint32_t byte2 = (uint32_t)read_unsigned_byte();
        const uint32_t byte3 = (uint32_t)read_unsigned_byte();
        bits |= byte1 << 8;
        bits |= byte2 << 16;
        bits |= byte3 << 24;
        memcpy(&n2, &bits, sizeof(n2));
    }
    return n2;
}

void SaveCodec::load_slot(int32_t slot, int32_t mode) {
    saveSlot = slot;
    savePendingMode = mode;
    saveReadOffset = 0;
    snapshotSize = 0;
    snapshotLoaded = false;
    if (savePendingMode == save::MODE_READ) {
        std::unique_ptr<RecordStore> store = RecordStore::open("TTStore", true);
        if (store) {
            const int count = store->record_count();
            for (int i = 0; i < count; ++i) {
                const int id = store->record_id_at(i);
                const auto record = store->record(id);
                if (!record) continue;
                const int length = static_cast<int>(record->size());
                if (length > 0 && (int8_t)(*record)[0] == slot) {
                    saveBuffer.assign(static_cast<std::size_t>(length - 1), 0);
                    memcpy(saveBuffer.data(), record->data() + 1,
                           (size_t)(length - 1));
                    snapshotSize = length - 1;
                    snapshotLoaded = true;
                }
            }
        }
    } else {
        snapshotLoaded = true;
    }
}

void SaveCodec::persist() const {
    if (savePendingMode != 0) return;
    std::unique_ptr<RecordStore> store = RecordStore::open("TTStore", true);
    if (!store) return;
    for (int index = store->record_count() - 1; index >= 0; --index) {
        int id = store->record_id_at(index);
        const auto record = store->record(id);
        if (!record) continue;
        bool same_slot = !record->empty() && (int8_t)(*record)[0] == saveSlot;
        if (same_slot) store->delete_record(id);
    }
    std::vector<uint8_t> record(static_cast<std::size_t>(snapshotSize) + 1);
    record[0] = (uint8_t)saveSlot;
    memcpy(record.data() + 1, saveBuffer.data(), (size_t)snapshotSize);
    store->add_record(record);
}

namespace {

void write_rle_run(ByteBuffer& output, int32_t value, int32_t count) {
    if (count == 1) {
        if (value == -1) {
            output.write(255);
            output.write(1);
        }
        output.write(value);
    } else {
        output.write(255);
        output.write(count);
        output.write(value);
    }
}

}  // namespace

void SaveCodec::compress() {
    ByteBuffer output;
    int value = saveBuffer[0];
    int count = 1;
    for (int i = 1; i < snapshotSize; ++i) {
        if (value != saveBuffer[i] || count == 254) {
            write_rle_run(output, value, count);
            value = saveBuffer[i];
            count = 1;
        } else {
            ++count;
        }
    }
    write_rle_run(output, value, count);
    bytesUncompressed += snapshotSize;
    const auto compressed = output.bytes();
    snapshotSize = static_cast<int>(compressed.size());
    saveBuffer.assign(static_cast<std::size_t>(snapshotSize), 0);
    memcpy(saveBuffer.data(), compressed.data(), (size_t)snapshotSize);
    bytesCompressed += snapshotSize;
}

void SaveCodec::decompress() {
    ByteBuffer output;
    for (int i = 0; i < snapshotSize; ++i) {
        if (saveBuffer[i] == -1) {
            int count = unsignedByte(saveBuffer[i + 1]);
            for (int j = 0; j < count; ++j)
                output.write(saveBuffer[i + 2]);
            i += 2;
        } else {
            output.write(saveBuffer[i]);
        }
    }
    bytesCompressed += snapshotSize;
    const auto decompressed = output.bytes();
    snapshotSize = static_cast<int>(decompressed.size());
    saveBuffer.assign(static_cast<std::size_t>(snapshotSize), 0);
    memcpy(saveBuffer.data(), decompressed.data(), (size_t)snapshotSize);
    bytesUncompressed += snapshotSize;
}

void SaveCodec::walk(Game& game_value, int32_t mode) {
    Game* game = &game_value;
    if (mode == save::OP_SAVE_MAIN) load_slot(0, save::MODE_WRITE);
    if (mode == save::OP_LOAD_MAIN) load_slot(0, save::MODE_READ);
    if (mode == save::OP_RESET_SKIP) savePendingMode = save::MODE_SKIP;
    if (mode == 4) {
        snapshotSize = 0;
        saveBuffer.clear();
        savePendingMode = save::MODE_WRITE;
    }
    if (mode == 5) {
        saveReadOffset = 0;
        savePendingMode = save::MODE_READ;
        snapshotLoaded = true;
    }
    if (mode == save::OP_LOAD_MAIN) decompress();

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

    IO_INT(world.mapWidth);
    IO_INT(world.mapHeight);
    IO_INT(renderer.viewportCenterX);
    IO_INT(renderer.viewportCenterY);
    IO_INT(economy.money);
    IO_INT(economy.dayOfMonth);
    IO_INT(economy.monthOfYear);
    IO_INT(economy.year);
    IO_INT(dayFrameCounter);
    IO_INT(renderer.viewportBaseX);
    IO_INT(renderer.viewportBaseY);
    IO_INT(renderer.mapScaleX);
    IO_INT(renderer.mapScaleY);
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
    IO_INT(era);
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
    if (game->build.nextTrackPieceCode == 0) game->build.nextTrackPieceCode = 66;
    IO_BYTE(scenario.aiFrequency);
    IO_BYTE(scenario.aiThreshold);
    if (mode != save::OP_RESET_SKIP) IO_BYTE(scenario.difficultyCycle);
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
    IO_INT(economy.moneyBaseA);
    IO_INT(economy.moneyBaseB);
    IO_INT(economy.baseTrainSpeed);
    IO_INT(economy.accel1);
    IO_INT(economy.brake1);
    IO_INT(economy.baseTrainSpeed2);
    IO_INT(economy.accel2);
    IO_INT(economy.costPower);
    IO_INT(economy.costStruct);
    IO_INT(economy.costOverhead);
    IO_INT(economy.costMaintenance);
    IO_INT(economy.costCrew);
    IO_INT(economy.gWagonCost);
    IO_INT(economy.gWagonCost2);
    IO_INT(economy.gWagonCost3);
    IO_INT(economy.gWagonCost4);
    IO_INT(economy.gWagonCost5);
    IO_INT(economy.gWagonCost6);
    IO_INT(economy.costGroupC1);
    IO_INT(economy.costGroupC2);
    IO_INT(economy.costGroupC3);
    IO_INT(economy.costGroupC4);

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

    if (mode == save::OP_SAVE_MAIN) {
        compress();
        persist();
    }
}

void SaveCodec::walk_settings(Game& game) {
    animationsDisabled = io_bool(animationsDisabled);
    selectionHidden = io_bool(selectionHidden);
    monthlyReportEnabled = io_bool(monthlyReportEnabled);
    annualReportEnabled = io_bool(annualReportEnabled);
    game.scenario.scenarioStepCompare = io_int(game.scenario.scenarioStepCompare);
    game.scenario.bestScenarioScore = io_int(game.scenario.bestScenarioScore);
    game.music_volume_level = io_int(game.music_volume_level);
    io_ints(game.scenario.scenarioScores, kScenarioCount);
    io_ints(game.scenario.leaderboardScores, kLeaderboardScoreCount);
    io_bytes(game.scenario.playerNameCodes, kPlayerNameCodeCount);
}

void SaveCodec::save_all(Game& game) {
    bytesUncompressed = 0;
    bytesCompressed = 0;
    walk(game, save::OP_SAVE_MAIN);
    load_slot(1, save::MODE_WRITE);
    io_byte_grid(game.world.tileMap);
    compress();
    persist();
    load_slot(2, save::MODE_WRITE);
    io_byte_grid(game.world.trackMap);
    compress();
    persist();
    load_slot(3, save::MODE_WRITE);
    io_byte_grid(game.world.tileElevation);
    compress();
    persist();
    load_slot(4, save::MODE_WRITE);
    io_byte_grid(game.world.heightMap);
    compress();
    persist();
}

void SaveCodec::restore_all(Game& game) {
    bytesUncompressed = 0;
    bytesCompressed = 0;
    walk(game, save::OP_LOAD_MAIN);
    game.world.tileMap.resize(game.world.mapWidth + 1, game.world.mapHeight + 1);
    game.world.trackMap.resize(game.world.mapWidth + 1, game.world.mapHeight + 1);
    game.world.tileElevation.resize(game.world.mapWidth + 1, game.world.mapHeight + 1);
    game.world.heightMap.resize(game.world.mapWidth + 2, game.world.mapHeight + 2);
    game.world.mapHalfWidth = (game.world.mapWidth - 1) * 8;
    game.world.mapScreenWidth = (game.world.mapWidth + game.world.mapHeight) * 16;
    game.world.mapScreenHeight = (game.world.mapWidth + game.world.mapHeight) * 8;
    game.renderer.viewportHalfWidth = game.world.mapScreenWidth / 2;
    game.renderer.viewportHalfHeight = game.world.mapScreenHeight / 2;
    game_initBuildMenuForScenario(&game, -1);
    load_slot(1, save::MODE_READ);
    decompress();
    io_byte_grid(game.world.tileMap);
    load_slot(2, save::MODE_READ);
    decompress();
    io_byte_grid(game.world.trackMap);
    load_slot(3, save::MODE_READ);
    decompress();
    io_byte_grid(game.world.tileElevation);
    load_slot(4, save::MODE_READ);
    decompress();
    io_byte_grid(game.world.heightMap);
    game.renderer.lastViewportCenterX = -1;
}
