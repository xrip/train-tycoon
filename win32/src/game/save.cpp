// Save-buffer reading, writing, and compression.
#include "game_internal.h"
#include "support/record_store.h"

#include <stdlib.h>
#include <string.h>

// Read one byte from the save buffer as an unsigned value (0..255).
int32_t game_readSaveByteUnsigned(Game *game) {
    return game->saveCodec.saveBuffer[game->saveCodec.saveReadOffset] >= 0
               ? game->saveCodec.saveBuffer[game->saveCodec.saveReadOffset++]
               : 256 + game->saveCodec.saveBuffer[game->saveCodec.saveReadOffset++];
}

bool game_saveOrLoadBool(Game *game, bool bl) {
    return game_saveOrLoadByte(game, (int8_t)(bl ? 1 : 0)) == 1;
}

int8_t game_saveOrLoadByte(Game *game, int8_t by) {
    if (game->saveCodec.savePendingMode == save::MODE_SKIP) return 0;
    int8_t by2 = by;
    if (game->saveCodec.savePendingMode == save::MODE_WRITE) {
        game_ensureSaveBufferCapacity(game, 1);
        game->saveCodec.saveBuffer[game->saveCodec.saveReadOffset++] = by;
    } else if (game->saveCodec.snapshotLoaded) {
        by2 = game->saveCodec.saveBuffer[game->saveCodec.saveReadOffset++];
    }
    return by2;
}

int32_t game_saveIntToBuffer(Game *game, int32_t n) {
    if (game->saveCodec.savePendingMode == save::MODE_SKIP) return 0;
    int32_t n2 = n;
    if (game->saveCodec.savePendingMode == save::MODE_WRITE) {
        game_ensureSaveBufferCapacity(game, 4);
        game->saveCodec.saveBuffer[game->saveCodec.saveReadOffset++] = (int8_t)(n & 0xFF);
        game->saveCodec.saveBuffer[game->saveCodec.saveReadOffset++] = (int8_t)(n >> 8 & 0xFF);
        game->saveCodec.saveBuffer[game->saveCodec.saveReadOffset++] = (int8_t)(n >> 16 & 0xFF);
        game->saveCodec.saveBuffer[game->saveCodec.saveReadOffset++] = (int8_t)(n >> 24 & 0xFF);
    } else if (game->saveCodec.snapshotLoaded) {
        uint32_t bits = (uint32_t)game_readSaveByteUnsigned(game);
        bits |= (uint32_t)game_readSaveByteUnsigned(game) << 8;
        bits |= (uint32_t)game_readSaveByteUnsigned(game) << 16;
        bits |= (uint32_t)game_readSaveByteUnsigned(game) << 24;
        memcpy(&n2, &bits, sizeof(n2));
    }
    return n2;
}

void game_saveGame(Game *game) {
    if (game->saveCodec.savePendingMode != 0) return;
    std::unique_ptr<RecordStore> store = RecordStore::open("TTStore", true);
    if (!store) return;
    for (int index = store->record_count() - 1; index >= 0; --index) {
        int id = store->record_id_at(index);
        const auto record = store->record(id);
        if (!record) continue;
        bool same_slot = !record->empty() && (int8_t)(*record)[0] == game->saveCodec.saveSlot;
        if (same_slot) store->delete_record(id);
    }
    std::vector<uint8_t> record(static_cast<std::size_t>(game->saveCodec.snapshotSize) + 1);
    record[0] = (uint8_t)game->saveCodec.saveSlot;
    memcpy(record.data() + 1, game->saveCodec.saveBuffer.data(), (size_t)game->saveCodec.snapshotSize);
    store->add_record(record);
}

void game_writeRleRun(ByteBuffer& output, int32_t value, int32_t count) {
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

void game_compressSaveBuffer(Game *game) {
    ByteBuffer output;
    int value = game->saveCodec.saveBuffer[0];
    int count = 1;
    for (int i = 1; i < game->saveCodec.snapshotSize; ++i) {
        if (value != game->saveCodec.saveBuffer[i] || count == 254) {
            game_writeRleRun(output, value, count);
            value = game->saveCodec.saveBuffer[i];
            count = 1;
        } else {
            ++count;
        }
    }
    game_writeRleRun(output, value, count);
    game->saveCodec.bytesUncompressed += game->saveCodec.snapshotSize;
    const auto compressed = output.bytes();
    game->saveCodec.snapshotSize = static_cast<int>(compressed.size());
    game->saveCodec.saveBuffer.assign(static_cast<std::size_t>(game->saveCodec.snapshotSize), 0);
    memcpy(game->saveCodec.saveBuffer.data(), compressed.data(), (size_t)game->saveCodec.snapshotSize);
    game->saveCodec.bytesCompressed += game->saveCodec.snapshotSize;
}

void game_decompressSaveBuffer(Game *game) {
    ByteBuffer output;
    for (int i = 0; i < game->saveCodec.snapshotSize; ++i) {
        if (game->saveCodec.saveBuffer[i] == -1) {
            int count = unsignedByte(game->saveCodec.saveBuffer[i + 1]);
            for (int j = 0; j < count; ++j)
                output.write(game->saveCodec.saveBuffer[i + 2]);
            i += 2;
        } else {
            output.write(game->saveCodec.saveBuffer[i]);
        }
    }
    game->saveCodec.bytesCompressed += game->saveCodec.snapshotSize;
    const auto decompressed = output.bytes();
    game->saveCodec.snapshotSize = static_cast<int>(decompressed.size());
    game->saveCodec.saveBuffer.assign(static_cast<std::size_t>(game->saveCodec.snapshotSize), 0);
    memcpy(game->saveCodec.saveBuffer.data(), decompressed.data(), (size_t)game->saveCodec.snapshotSize);
    game->saveCodec.bytesUncompressed += game->saveCodec.snapshotSize;
}

void game_initSaveOrLoad(Game *game, int32_t mode) {
    if (mode == save::OP_SAVE_MAIN) game_loadGame(game, 0, save::MODE_WRITE);
    if (mode == save::OP_LOAD_MAIN) game_loadGame(game, 0, save::MODE_READ);
    if (mode == save::OP_RESET_SKIP) game->saveCodec.savePendingMode = save::MODE_SKIP;
    if (mode == 4) {
        game->saveCodec.snapshotSize = 0;
        game->saveCodec.saveBuffer.clear();
        game->saveCodec.savePendingMode = save::MODE_WRITE;
    }
    if (mode == 5) {
        game->saveCodec.saveReadOffset = 0;
        game->saveCodec.savePendingMode = save::MODE_READ;
        game->saveCodec.snapshotLoaded = true;
    }
    if (mode == save::OP_LOAD_MAIN) game_decompressSaveBuffer(game);

#define IO_INT(field) \
    (game->field = game_saveIntToBuffer(game, game->field))
#define IO_BYTE(field) \
    (game->field = game_saveOrLoadByte(game, game->field))
#define IO_BOOL(field) \
    (game->field = game_saveOrLoadBool(game, game->field))
#define FIELD_COUNT(field) static_cast<int>(sizeof(game->field) / sizeof(game->field[0]))
#define IO_BYTES(field) game_saveOrLoadBytes(game, game->field, FIELD_COUNT(field))
#define IO_SHORTS(field) game_saveOrLoadShorts(game, game->field, FIELD_COUNT(field))
#define IO_INTS(field) game_saveOrLoadInts(game, game->field, FIELD_COUNT(field))

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
    IO_INTS(fleet.trainRevenue);
    IO_INTS(fleet.prevTrainRevenue);
    IO_SHORTS(economy.stationRevenue);
    IO_SHORTS(economy.prevStationRevenue);
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
    IO_BYTES(world.stationDirectionCode);
    IO_BYTES(world.cityPopulation);
    IO_BYTES(world.cityIndex);
    IO_BYTES(fleet.vehicleFlags);
    IO_BYTES(fleet.serviceVehicleX);
    IO_BYTES(fleet.serviceVehicleY);
    IO_BYTES(fleet.serviceUnloadFlags);
    game_saveOrLoadShorts(game, &game->fleet.serviceResourceStock[0][0],
                          kServiceVehicleSlotCount * kResourceTypeCount);
    IO_BYTE(ui.numSelectedTrains);
    IO_BOOL(scenario.aiExpansionEnabled);
    IO_BYTES(world.constructionTimers);
    IO_SHORTS(world.zonePopulation);
    IO_SHORTS(ui.trainSelectionIndex);
    IO_SHORTS(ui.trainSelectionY);
    IO_SHORTS(ui.trainSelectionX);
    IO_INT(era);
    IO_BYTES(fleet.trainX);
    IO_BYTES(fleet.trainY);
    IO_BYTES(fleet.trainDir);
    IO_BYTES(fleet.trainPhase);
    IO_BYTES(fleet.trainFlags);
    IO_BYTES(fleet.secondaryCargoOrBaseEnergy);
    IO_BYTES(fleet.energyCargoOrRemovalTicks);
    IO_BYTES(fleet.lastServiceVehicle);
    IO_BYTES(fleet.cargoAge);
    IO_SHORTS(fleet.nextCar);
    IO_BYTES(fleet.trainEra);
    IO_BYTES(fleet.trainAgeYears);
    IO_INTS(fleet.trainStateCode);
    IO_BYTES(fleet.trainAiOverride);
    IO_INT(world.industryCategory);
    IO_BOOL(world.industrySiteFound);
    IO_INT(build.nextTrackPieceCode);
    if (game->build.nextTrackPieceCode == 0) game->build.nextTrackPieceCode = 66;
    IO_BYTE(scenario.aiFrequency);
    IO_BYTE(scenario.aiThreshold);
    if (mode != save::OP_RESET_SKIP) IO_BYTE(scenario.difficultyCycle);
    IO_BYTES(ui.menuRowType);
    IO_BYTES(ui.placeTrainDay);
    IO_BYTES(ui.menuRowParamA);
    IO_BYTES(ui.placeTrainMonth);
    IO_BYTES(ui.menuRowParamB);
    IO_BYTES(ui.placeTrainYear);
    IO_BYTES(ui.menuRowParamC);
    IO_INTS(ui.menuRowCost);
    IO_INTS(ui.menuRowValue);
    IO_BYTES(ui.menuRowAltType);
    IO_BYTES(ui.menuRowEra);
    IO_INTS(ui.menuRowRevenue);
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
#undef FIELD_COUNT

    if (mode == save::OP_SAVE_MAIN) {
        game_compressSaveBuffer(game);
        game_saveGame(game);
    }
}

void game_saveAllMapArrays(Game *game) {
    game->saveCodec.bytesUncompressed = 0;
    game->saveCodec.bytesCompressed = 0;
    game_initSaveOrLoad(game, save::OP_SAVE_MAIN);
    game_loadGame(game, 1, save::MODE_WRITE);
    game_saveOrLoadByteGrid(game, game->world.tileMap);
    game_compressSaveBuffer(game);
    game_saveGame(game);
    game_loadGame(game, 2, save::MODE_WRITE);
    game_saveOrLoadByteGrid(game, game->world.trackMap);
    game_compressSaveBuffer(game);
    game_saveGame(game);
    game_loadGame(game, 3, save::MODE_WRITE);
    game_saveOrLoadByteGrid(game, game->world.tileElevation);
    game_compressSaveBuffer(game);
    game_saveGame(game);
    game_loadGame(game, 4, save::MODE_WRITE);
    game_saveOrLoadByteGrid(game, game->world.heightMap);
    game_compressSaveBuffer(game);
    game_saveGame(game);
}

void game_restoreAllMapArrays(Game *game) {
    game->saveCodec.bytesUncompressed = 0;
    game->saveCodec.bytesCompressed = 0;
    game_initSaveOrLoad(game, save::OP_LOAD_MAIN);
    game->world.tileMap.resize(game->world.mapWidth + 1, game->world.mapHeight + 1);
    game->world.trackMap.resize(game->world.mapWidth + 1, game->world.mapHeight + 1);
    game->world.tileElevation.resize(game->world.mapWidth + 1, game->world.mapHeight + 1);
    game->world.heightMap.resize(game->world.mapWidth + 2, game->world.mapHeight + 2);
    game->world.mapHalfWidth = (game->world.mapWidth - 1) * 8;
    game->world.mapScreenWidth = (game->world.mapWidth + game->world.mapHeight) * 16;
    game->world.mapScreenHeight = (game->world.mapWidth + game->world.mapHeight) * 8;
    game->renderer.viewportHalfWidth = game->world.mapScreenWidth / 2;
    game->renderer.viewportHalfHeight = game->world.mapScreenHeight / 2;
    game_initBuildMenuForScenario(game, -1);
    game_loadGame(game, 1, save::MODE_READ);
    game_decompressSaveBuffer(game);
    game_saveOrLoadByteGrid(game, game->world.tileMap);
    game_loadGame(game, 2, save::MODE_READ);
    game_decompressSaveBuffer(game);
    game_saveOrLoadByteGrid(game, game->world.trackMap);
    game_loadGame(game, 3, save::MODE_READ);
    game_decompressSaveBuffer(game);
    game_saveOrLoadByteGrid(game, game->world.tileElevation);
    game_loadGame(game, 4, save::MODE_READ);
    game_decompressSaveBuffer(game);
    game_saveOrLoadByteGrid(game, game->world.heightMap);
    game->renderer.lastViewportCenterX = -1;
}
