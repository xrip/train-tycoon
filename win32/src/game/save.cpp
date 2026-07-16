// Save-buffer reading, writing, and compression.
#include "game_methods.h"
#include "support/record_store.h"

#include <stdlib.h>
#include <string.h>

// Read one byte from the save buffer as an unsigned value (0..255).
int32_t gc_readSaveByteUnsigned(GameCanvas *gc) {
    return gc->saveBuffer[gc->saveReadOffset] >= 0
               ? gc->saveBuffer[gc->saveReadOffset++]
               : 256 + gc->saveBuffer[gc->saveReadOffset++];
}

// boolean saveOrLoadBool(boolean bl)
bool gc_saveOrLoadBool(GameCanvas *gc, bool bl) {
    return gc_saveOrLoadByte(gc, (int8_t)(bl ? 1 : 0)) == 1;
}

// byte saveOrLoadByte(byte by)
int8_t gc_saveOrLoadByte(GameCanvas *gc, int8_t by) {
    if (gc->savePendingMode == save::MODE_SKIP) return 0;
    int8_t by2 = by;
    if (gc->savePendingMode == save::MODE_WRITE) {
        gc_ensureSaveBufferCapacity(gc, 1);
        gc->saveBuffer[gc->saveReadOffset++] = by;
    } else if (gc->snapshotLoaded) {
        by2 = gc->saveBuffer[gc->saveReadOffset++];
    }
    return by2;
}

// int saveIntToBuffer(int n)
int32_t gc_saveIntToBuffer(GameCanvas *gc, int32_t n) {
    if (gc->savePendingMode == save::MODE_SKIP) return 0;
    int32_t n2 = n;
    if (gc->savePendingMode == save::MODE_WRITE) {
        gc_ensureSaveBufferCapacity(gc, 4);
        gc->saveBuffer[gc->saveReadOffset++] = (int8_t)(n & 0xFF);
        gc->saveBuffer[gc->saveReadOffset++] = (int8_t)(n >> 8 & 0xFF);
        gc->saveBuffer[gc->saveReadOffset++] = (int8_t)(n >> 16 & 0xFF);
        gc->saveBuffer[gc->saveReadOffset++] = (int8_t)(n >> 24 & 0xFF);
    } else if (gc->snapshotLoaded) {
        uint32_t bits = (uint32_t)gc_readSaveByteUnsigned(gc);
        bits |= (uint32_t)gc_readSaveByteUnsigned(gc) << 8;
        bits |= (uint32_t)gc_readSaveByteUnsigned(gc) << 16;
        bits |= (uint32_t)gc_readSaveByteUnsigned(gc) << 24;
        memcpy(&n2, &bits, sizeof(n2));
    }
    return n2;
}

void gc_saveGame(GameCanvas *gc) {
    if (gc->savePendingMode != 0) return;
    std::unique_ptr<RecordStore> store = RecordStore::open("TTStore", true);
    if (!store) return;
    for (int index = store->record_count() - 1; index >= 0; --index) {
        int id = store->record_id_at(index);
        const auto record = store->record(id);
        if (!record) continue;
        bool same_slot = !record->empty() && (int8_t)(*record)[0] == gc->saveSlot;
        if (same_slot) store->delete_record(id);
    }
    std::vector<uint8_t> record(static_cast<std::size_t>(gc->snapshotSize) + 1);
    record[0] = (uint8_t)gc->saveSlot;
    memcpy(record.data() + 1, gc->saveBuffer.data(), (size_t)gc->snapshotSize);
    store->add_record(record);
}

void gc_writeRleRun(GameCanvas *gc, ByteBuffer& output,
                    int32_t value, int32_t count) {
    (void)gc;
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

void gc_checkSaveByteSum(GameCanvas *gc, const char *label) {
    (void)label;
    uint64_t sum = 0;
    for (int i = 0; i < gc->snapshotSize; ++i)
        sum += (uint64_t)gc_byteToUnsignedInt(gc, gc->saveBuffer[i]);
    (void)sum;
}

void gc_compressSaveBuffer(GameCanvas *gc) {
    gc_checkSaveByteSum(gc, "Compress pre");
    ByteBuffer output;
    int value = gc->saveBuffer[0];
    int count = 1;
    for (int i = 1; i < gc->snapshotSize; ++i) {
        if (value != gc->saveBuffer[i] || count == 254) {
            gc_writeRleRun(gc, output, value, count);
            value = gc->saveBuffer[i];
            count = 1;
        } else {
            ++count;
        }
    }
    gc_writeRleRun(gc, output, value, count);
    gc->bytesUncompressed += gc->snapshotSize;
    const auto compressed = output.bytes();
    gc->snapshotSize = static_cast<int>(compressed.size());
    gc->saveBuffer.assign(static_cast<std::size_t>(gc->snapshotSize), 0);
    memcpy(gc->saveBuffer.data(), compressed.data(), (size_t)gc->snapshotSize);
    gc->bytesCompressed += gc->snapshotSize;
    gc_checkSaveByteSum(gc, "Compress post");
}

void gc_decompressSaveBuffer(GameCanvas *gc) {
    gc_checkSaveByteSum(gc, "Decompress pre");
    ByteBuffer output;
    for (int i = 0; i < gc->snapshotSize; ++i) {
        if (gc->saveBuffer[i] == -1) {
            int count = gc_byteToUnsignedInt(gc, gc->saveBuffer[i + 1]);
            for (int j = 0; j < count; ++j)
                output.write(gc->saveBuffer[i + 2]);
            i += 2;
        } else {
            output.write(gc->saveBuffer[i]);
        }
    }
    gc->bytesCompressed += gc->snapshotSize;
    const auto decompressed = output.bytes();
    gc->snapshotSize = static_cast<int>(decompressed.size());
    gc->saveBuffer.assign(static_cast<std::size_t>(gc->snapshotSize), 0);
    memcpy(gc->saveBuffer.data(), decompressed.data(), (size_t)gc->snapshotSize);
    gc->bytesUncompressed += gc->snapshotSize;
    gc_checkSaveByteSum(gc, "Decompress post");
}

void gc_initSaveOrLoad(GameCanvas *gc, int32_t mode) {
    if (mode == save::OP_SAVE_MAIN) gc_loadGame(gc, 0, save::MODE_WRITE);
    if (mode == save::OP_LOAD_MAIN) gc_loadGame(gc, 0, save::MODE_READ);
    if (mode == save::OP_RESET_SKIP) gc->savePendingMode = save::MODE_SKIP;
    if (mode == 4) {
        gc->snapshotSize = 0;
        gc->saveBuffer.clear();
        gc->savePendingMode = save::MODE_WRITE;
    }
    if (mode == 5) {
        gc->saveReadOffset = 0;
        gc->savePendingMode = save::MODE_READ;
        gc->snapshotLoaded = true;
    }
    if (mode == save::OP_LOAD_MAIN) gc_decompressSaveBuffer(gc);

#define IO_INT(field) \
    (gc->field = gc_saveIntToBuffer(gc, gc->field))
#define IO_BYTE(field) \
    (gc->field = gc_saveOrLoadByte(gc, gc->field))
#define IO_BOOL(field) \
    (gc->field = gc_saveOrLoadBool(gc, gc->field))
#define IO_BYTES(field) gc_saveOrLoadByteArrayInlined(gc, gc->field)
#define IO_SHORTS(field) gc_saveOrLoadShortArrayInlined(gc, gc->field)
#define IO_INTS(field) gc_saveOrLoadIntArrayInlined(gc, gc->field)

    IO_INT(world.mapWidth);
    IO_INT(world.mapHeight);
    IO_INT(viewportCenterX);
    IO_INT(viewportCenterY);
    IO_INT(economy.money);
    IO_INT(economy.dayOfMonth);
    IO_INT(economy.monthOfYear);
    IO_INT(economy.year);
    IO_INT(counterA);
    IO_INT(viewportBaseX);
    IO_INT(viewportBaseY);
    IO_INT(mapScaleX);
    IO_INT(mapScaleY);
    IO_SHORTS(visibleTrains);
    IO_INT(visibleTrainCount);
    IO_INT(viewportX0);
    IO_INT(viewportY0);
    IO_INT(viewportX1);
    IO_INT(viewportY1);
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
    IO_BYTE(cityListIndex);
    IO_BYTES(world.stationDirectionCode);
    IO_BYTES(world.cityPopulation);
    IO_BYTES(world.cityIndex);
    IO_BYTES(fleet.vehicleFlags);
    IO_BYTES(fleet.serviceVehicleX);
    IO_BYTES(fleet.serviceVehicleY);
    IO_BYTES(fleet.serviceUnloadFlags);
    gc_saveOrLoadShortMatrixInlined(gc, gc->fleet.serviceResourceStock);
    IO_BYTE(numSelectedTrains);
    IO_BOOL(flagsA);
    IO_BYTES(world.constructionTimers);
    IO_SHORTS(world.zonePopulation);
    IO_SHORTS(trainSelectionIndex);
    IO_SHORTS(trainSelectionY);
    IO_SHORTS(trainSelectionX);
    IO_INT(era);
    IO_BYTES(fleet.trainX);
    IO_BYTES(fleet.trainY);
    IO_BYTES(fleet.trainDir);
    IO_BYTES(fleet.trainPhase);
    IO_BYTES(fleet.trainFlags);
    IO_BYTES(fleet.wagonEnergyRemaining);
    IO_BYTES(fleet.trainEnergyOrCount);
    IO_BYTES(fleet.lastServiceVehicle);
    IO_BYTES(fleet.cargoAge);
    IO_SHORTS(fleet.nextCar);
    IO_BYTES(fleet.trainEraOrStage);
    IO_BYTES(fleet.trainAgeOrTimer);
    IO_INTS(fleet.trainStateCode);
    IO_BYTES(fleet.trainAiOverride);
    IO_INT(industryCategory);
    IO_BOOL(industrySiteFound);
    IO_INT(build.nextTrackPieceCode);
    if (gc->build.nextTrackPieceCode == 0) gc->build.nextTrackPieceCode = 66;
    IO_BYTE(scenario.aiFrequency);
    IO_BYTE(scenario.aiThreshold);
    if (mode != save::OP_RESET_SKIP) IO_BYTE(scenario.difficultyCycle);
    IO_BYTES(menuRowType);
    IO_BYTES(placeTrainDay);
    IO_BYTES(menuRowParamA);
    IO_BYTES(placeTrainMonth);
    IO_BYTES(menuRowParamB);
    IO_BYTES(placeTrainYear);
    IO_BYTES(menuRowParamC);
    IO_INTS(menuRowCost);
    IO_INTS(menuRowValue);
    IO_BYTES(menuRowAltType);
    IO_BYTES(menuRowEra);
    IO_INTS(menuRowRevenue);
    IO_SHORTS(scenario.offerEffectNextCount);
    IO_INT(scenario.offerGenerationCount);
    IO_INT(savedLegacyMetricBq);
    IO_INT(scenario.lastOfferDateCode);
    IO_BOOL(flagsB);
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
    IO_INT(moneyBaseA);
    IO_INT(moneyBaseB);
    IO_INT(baseTrainSpeed);
    IO_INT(accel1);
    IO_INT(brake1);
    IO_INT(baseTrainSpeed2);
    IO_INT(accel2);
    IO_INT(costPower);
    IO_INT(costStruct);
    IO_INT(costOverhead);
    IO_INT(costMaintenance);
    IO_INT(costCrew);
    IO_INT(gWagonCost);
    IO_INT(gWagonCost2);
    IO_INT(gWagonCost3);
    IO_INT(gWagonCost4);
    IO_INT(gWagonCost5);
    IO_INT(gWagonCost6);
    IO_INT(costGroupC1);
    IO_INT(costGroupC2);
    IO_INT(costGroupC3);
    IO_INT(costGroupC4);

#undef IO_INT
#undef IO_BYTE
#undef IO_BOOL
#undef IO_BYTES
#undef IO_SHORTS
#undef IO_INTS

    if (mode == save::OP_SAVE_MAIN) {
        gc_compressSaveBuffer(gc);
        gc_saveGame(gc);
    }
}

void gc_saveAllMapArrays(GameCanvas *gc) {
    gc->bytesUncompressed = 0;
    gc->bytesCompressed = 0;
    gc_initSaveOrLoad(gc, save::OP_SAVE_MAIN);
    gc_loadGame(gc, 1, save::MODE_WRITE);
    gc_saveOrLoadByteMatrixInlined(gc, gc->world.tileMap);
    gc_compressSaveBuffer(gc);
    gc_saveGame(gc);
    gc_loadGame(gc, 2, save::MODE_WRITE);
    gc_saveOrLoadByteMatrixInlined(gc, gc->world.trackMap);
    gc_compressSaveBuffer(gc);
    gc_saveGame(gc);
    gc_loadGame(gc, 3, save::MODE_WRITE);
    gc_saveOrLoadByteMatrixInlined(gc, gc->world.tileElevation);
    gc_compressSaveBuffer(gc);
    gc_saveGame(gc);
    gc_loadGame(gc, 4, save::MODE_WRITE);
    gc_saveOrLoadByteMatrixInlined(gc, gc->world.heightMap);
    gc_compressSaveBuffer(gc);
    gc_saveGame(gc);
}

void gc_restoreAllMapArrays(GameCanvas *gc) {
    gc->bytesUncompressed = 0;
    gc->bytesCompressed = 0;
    gc_initSaveOrLoad(gc, save::OP_LOAD_MAIN);
    gc->world.tileMap.resize(gc->world.mapWidth + 1, gc->world.mapHeight + 1);
    gc->world.trackMap.resize(gc->world.mapWidth + 1, gc->world.mapHeight + 1);
    gc->world.tileElevation.resize(gc->world.mapWidth + 1, gc->world.mapHeight + 1);
    gc->world.heightMap.resize(gc->world.mapWidth + 2, gc->world.mapHeight + 2);
    gc->world.mapHalfWidth = (gc->world.mapWidth - 1) * 8;
    gc->world.mapScreenWidth = (gc->world.mapWidth + gc->world.mapHeight) * 16;
    gc->world.mapScreenHeight = (gc->world.mapWidth + gc->world.mapHeight) * 8;
    gc->viewportHalfWidth = gc->world.mapScreenWidth / 2;
    gc->viewportHalfHeight = gc->world.mapScreenHeight / 2;
    gc_initBuildMenuForScenario(gc, -1);
    gc_loadGame(gc, 1, save::MODE_READ);
    gc_decompressSaveBuffer(gc);
    gc_saveOrLoadByteMatrixInlined(gc, gc->world.tileMap);
    gc_loadGame(gc, 2, save::MODE_READ);
    gc_decompressSaveBuffer(gc);
    gc_saveOrLoadByteMatrixInlined(gc, gc->world.trackMap);
    gc_loadGame(gc, 3, save::MODE_READ);
    gc_decompressSaveBuffer(gc);
    gc_saveOrLoadByteMatrixInlined(gc, gc->world.tileElevation);
    gc_loadGame(gc, 4, save::MODE_READ);
    gc_decompressSaveBuffer(gc);
    gc_saveOrLoadByteMatrixInlined(gc, gc->world.heightMap);
    gc->lastViewportCenterX = -1;
}
