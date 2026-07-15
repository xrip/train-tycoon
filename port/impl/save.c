// port/impl/save.c - save-buffer read/write primitives (hand-ported).
#include "game_methods.h"

#include <stdlib.h>
#include <string.h>

// private int readShortFromBuffer()
int32_t gc_readShortFromBuffer(GameCanvas *gc) {
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
    if (gc->savePendingMode == 2) return 0;
    int8_t by2 = by;
    if (gc->savePendingMode == 0) {
        gc_ensureSaveBufferCapacity(gc, 1);
        gc->saveBuffer[gc->saveReadOffset++] = by;
    } else if (gc->snapshotLoaded) {
        by2 = gc->saveBuffer[gc->saveReadOffset++];
    }
    return by2;
}

// int saveIntToBuffer(int n)
int32_t gc_saveIntToBuffer(GameCanvas *gc, int32_t n) {
    if (gc->savePendingMode == 2) return 0;
    int32_t n2 = n;
    if (gc->savePendingMode == 0) {
        gc_ensureSaveBufferCapacity(gc, 4);
        gc->saveBuffer[gc->saveReadOffset++] = (int8_t)(n & 0xFF);
        gc->saveBuffer[gc->saveReadOffset++] = (int8_t)(n >> 8 & 0xFF);
        gc->saveBuffer[gc->saveReadOffset++] = (int8_t)(n >> 16 & 0xFF);
        gc->saveBuffer[gc->saveReadOffset++] = (int8_t)(n >> 24 & 0xFF);
    } else if (gc->snapshotLoaded) {
        n2 = gc_readShortFromBuffer(gc) + 256 * gc_readShortFromBuffer(gc)
             + 65536 * gc_readShortFromBuffer(gc)
             + 0x1000000 * gc_readShortFromBuffer(gc);
    }
    return n2;
}

void gc_saveGame(GameCanvas *gc) {
    if (gc->savePendingMode != 0) return;
    MidpRecordStore *store = midp_rms_open("TTStore", true);
    if (!store) return;
    for (int index = midp_rms_num_records(store) - 1; index >= 0; --index) {
        int id = midp_rms_record_id_at(store, index);
        uint8_t *record = NULL;
        int length = 0;
        if (!midp_rms_get_record(store, id, &record, &length)) continue;
        bool same_slot = length > 0 && (int8_t)record[0] == gc->saveSlot;
        free(record);
        if (same_slot) midp_rms_delete_record(store, id);
    }
    uint8_t *record = (uint8_t *)malloc((size_t)gc->snapshotSize + 1);
    if (record) {
        record[0] = (uint8_t)gc->saveSlot;
        memcpy(record + 1, gc->saveBuffer, (size_t)gc->snapshotSize);
        midp_rms_add_record(store, record, 0, gc->snapshotSize + 1);
        free(record);
    }
    midp_rms_close(store);
}

void gc_writeRlePairInlined(GameCanvas *gc, MidpByteArray *output,
                            int32_t value, int32_t count) {
    (void)gc;
    if (count == 1) {
        if (value == -1) {
            midp_bytearray_write(output, 255);
            midp_bytearray_write(output, 1);
        }
        midp_bytearray_write(output, value);
    } else {
        midp_bytearray_write(output, 255);
        midp_bytearray_write(output, count);
        midp_bytearray_write(output, value);
    }
}

void gc_checkSaveByteSum(GameCanvas *gc, char *label) {
    (void)label;
    uint64_t sum = 0;
    for (int i = 0; i < gc->snapshotSize; ++i)
        sum += (uint64_t)gc_byteToUnsignedInt(gc, gc->saveBuffer[i]);
    (void)sum;
}

void gc_compressSaveBuffer(GameCanvas *gc) {
    gc_checkSaveByteSum(gc, "Compress pre");
    MidpByteArray *output = midp_bytearray_new();
    int value = gc->saveBuffer[0];
    int count = 1;
    for (int i = 1; i < gc->snapshotSize; ++i) {
        if (value != gc->saveBuffer[i] || count == 254) {
            gc_writeRlePairInlined(gc, output, value, count);
            value = gc->saveBuffer[i];
            count = 1;
        } else {
            ++count;
        }
    }
    gc_writeRlePairInlined(gc, output, value, count);
    gc->bytesUncompressed += gc->snapshotSize;
    gc->snapshotSize = midp_bytearray_size(output);
    gc->saveBuffer = GC_NEW(int8_t, gc->snapshotSize);
    memcpy(gc->saveBuffer, midp_bytearray_data(output), (size_t)gc->snapshotSize);
    gc->bytesCompressed += gc->snapshotSize;
    midp_bytearray_free(output);
    gc_checkSaveByteSum(gc, "Compress post");
}

void gc_decompressSaveBuffer(GameCanvas *gc) {
    gc_checkSaveByteSum(gc, "Decompress pre");
    MidpByteArray *output = midp_bytearray_new();
    for (int i = 0; i < gc->snapshotSize; ++i) {
        if (gc->saveBuffer[i] == -1) {
            int count = gc_byteToUnsignedInt(gc, gc->saveBuffer[i + 1]);
            for (int j = 0; j < count; ++j)
                midp_bytearray_write(output, gc->saveBuffer[i + 2]);
            i += 2;
        } else {
            midp_bytearray_write(output, gc->saveBuffer[i]);
        }
    }
    gc->bytesCompressed += gc->snapshotSize;
    gc->snapshotSize = midp_bytearray_size(output);
    gc->saveBuffer = GC_NEW(int8_t, gc->snapshotSize);
    memcpy(gc->saveBuffer, midp_bytearray_data(output), (size_t)gc->snapshotSize);
    gc->bytesUncompressed += gc->snapshotSize;
    midp_bytearray_free(output);
    gc_checkSaveByteSum(gc, "Decompress post");
}

void gc_initSaveOrLoad(GameCanvas *gc, int32_t mode) {
    if (mode == 0) gc_loadGame(gc, 0, 0);
    if (mode == 1) gc_loadGame(gc, 0, 1);
    if (mode == 2) gc->savePendingMode = 2;
    if (mode == 4) {
        gc->snapshotSize = 0;
        gc->saveBuffer = NULL;
        gc->savePendingMode = 0;
    }
    if (mode == 5) {
        gc->saveReadOffset = 0;
        gc->savePendingMode = 1;
        gc->snapshotLoaded = true;
    }
    if (mode == 1) gc_decompressSaveBuffer(gc);

#define IO_INT(field) \
    (gc->field = gc_saveIntToBuffer(gc, gc->field))
#define IO_BYTE(field) \
    (gc->field = gc_saveOrLoadByte(gc, gc->field))
#define IO_BOOL(field) \
    (gc->field = gc_saveOrLoadBool(gc, gc->field))
#define IO_BYTES(field) gc_saveOrLoadByteArrayInlined(gc, gc->field)
#define IO_SHORTS(field) gc_saveOrLoadShortArrayInlined(gc, gc->field)
#define IO_INTS(field) gc_saveOrLoadIntArrayInlined(gc, gc->field)

    IO_INT(mapWidth);
    IO_INT(mapHeight);
    IO_INT(viewportCenterX);
    IO_INT(viewportCenterY);
    IO_INT(money);
    IO_INT(dayOfMonth);
    IO_INT(monthOfYear);
    IO_INT(year);
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
    IO_INT(loanBalance);
    IO_INT(loanCeiling);
    IO_BYTE(interestRate);
    IO_INT(enterpriseTaxPct);
    IO_INTS(yearProfitHistory);
    IO_INTS(yearRevenueHistory);
    IO_INTS(yearCompanyValueHistory);
    IO_INTS(decadeProfitHistory);
    IO_INTS(altDecadeRevenueTotal);
    IO_INTS(altDecadeTotalCost);
    IO_INTS(trainRevenue);
    IO_INTS(prevTrainRevenue);
    IO_SHORTS(stationRevenue);
    IO_SHORTS(prevStationRevenue);
    IO_INT(revenueTotal);
    IO_INT(decadeProfitGraphTotal);
    IO_INT(totalSpent);
    IO_INT(aiMoneySpent);
    IO_INT(fuelCostAccrued);
    IO_INT(subsidyCost);
    IO_INT(totalRevenue);
    IO_INT(terrainTotal);
    IO_INT(decadeEnterpriseTax);
    IO_INT(netProfit);
    IO_INT(prevTotalSpent);
    IO_INT(prevAiMoneySpent);
    IO_INT(prevFuelCost);
    IO_INT(prevSubsidyCost);
    IO_INT(prevTotalRevenue);
    IO_INT(prevTerrainTotal);
    IO_INT(prevEnterpriseTotal);
    IO_INT(previousYearProfit);
    IO_INT(prevDecadeSpent);
    IO_INT(prevDecadeAiSpent);
    IO_INT(prevDecadeFuel);
    IO_INT(prevDecadeSubsidy);
    IO_INT(prevDecadeRevenue);
    IO_INT(previousLegacyDecadeMetric);
    IO_INT(previousDecadeEnterpriseTax);
    IO_INT(previousDecadeProfit);
    IO_INT(decadeCostTotal);
    IO_INT(decadeSpentTotal);
    IO_INT(decadeFuelCost);
    IO_INT(decadeSubsidyTotal);
    IO_INT(decadeRevenueTotal);
    IO_INT(savedLegacyMetricEb);
    IO_INT(decadeEnterpriseTotal);
    IO_INT(decadeProfitTotal);
    IO_BYTE(cityListIndex);
    IO_BYTES(stationDirectionCode);
    IO_BYTES(cityPopulation);
    IO_BYTES(cityIndex);
    IO_BYTES(vehicleFlags);
    IO_BYTES(serviceVehicleX);
    IO_BYTES(serviceVehicleY);
    IO_BYTES(serviceUnloadFlags);
    gc_saveOrLoadShortMatrixInlined(gc, gc->serviceResourceStock);
    IO_BYTE(numSelectedTrains);
    IO_BOOL(flagsA);
    IO_BYTES(constructionTimers);
    IO_SHORTS(zonePopulation);
    IO_SHORTS(trainSelectionIndex);
    IO_SHORTS(trainSelectionY);
    IO_SHORTS(trainSelectionX);
    IO_INT(era);
    IO_BYTES(trainX);
    IO_BYTES(trainY);
    IO_BYTES(trainDir);
    IO_BYTES(trainPhase);
    IO_BYTES(trainFlags);
    IO_BYTES(wagonEnergyRemaining);
    IO_BYTES(trainEnergyOrCount);
    IO_BYTES(lastServiceVehicle);
    IO_BYTES(cargoAge);
    IO_SHORTS(nextCar);
    IO_BYTES(trainEraOrStage);
    IO_BYTES(trainAgeOrTimer);
    IO_INTS(trainStateCode);
    IO_BYTES(trainAiOverride);
    IO_INT(industryCategory);
    IO_BOOL(industrySiteFound);
    IO_INT(nextTrackPieceCode);
    if (gc->nextTrackPieceCode == 0) gc->nextTrackPieceCode = 66;
    IO_BYTE(aiFrequency);
    IO_BYTE(aiThreshold);
    if (mode != 2) IO_BYTE(difficultyCycle);
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
    IO_SHORTS(offerEffectNextCount);
    IO_INT(offerGenerationCount);
    IO_INT(savedLegacyMetricBq);
    IO_INT(lastOfferDateCode);
    IO_BOOL(flagsB);
    IO_BYTE(scenarioBits);
    IO_BYTE(scenarioMilestoneIndex);
    IO_BOOL(milestonesEnabled);
    IO_INT(currentMilestoneScore);
    IO_INT(milestoneCoThreshold2);
    IO_INT(lifetimeTrainRevenue);
    IO_INT(milestoneThreshold1);
    IO_INT(milestoneThreshold2);
    IO_INT(milestoneThreshold3);
    IO_INT(milestoneThreshold4);
    IO_INT(milestoneThreshold5);
    IO_INT(aiWagonsBought);
    IO_INT(trainRevenueScore);
    IO_INT(scenarioStep);
    IO_INT(scenarioNoticeId);
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

    if (mode == 0) {
        gc_compressSaveBuffer(gc);
        gc_saveGame(gc);
    }
}

void gc_saveAllMapArrays(GameCanvas *gc) {
    gc->bytesUncompressed = 0;
    gc->bytesCompressed = 0;
    gc_initSaveOrLoad(gc, 0);
    gc_loadGame(gc, 1, 0);
    gc_saveOrLoadByteMatrixInlined(gc, gc->tileMap);
    gc_compressSaveBuffer(gc);
    gc_saveGame(gc);
    gc_loadGame(gc, 2, 0);
    gc_saveOrLoadByteMatrixInlined(gc, gc->trackMap);
    gc_compressSaveBuffer(gc);
    gc_saveGame(gc);
    gc_loadGame(gc, 3, 0);
    gc_saveOrLoadByteMatrixInlined(gc, gc->tileElevation);
    gc_compressSaveBuffer(gc);
    gc_saveGame(gc);
    gc_loadGame(gc, 4, 0);
    gc_saveOrLoadByteMatrixInlined(gc, gc->heightMap);
    gc_compressSaveBuffer(gc);
    gc_saveGame(gc);
}

void gc_restoreAllMapArrays(GameCanvas *gc) {
    gc->bytesUncompressed = 0;
    gc->bytesCompressed = 0;
    gc_initSaveOrLoad(gc, 1);
    gc->tileMap = GC_NEW(int8_t *, gc->mapWidth + 1);
    gc->trackMap = GC_NEW(int8_t *, gc->mapWidth + 1);
    gc->tileElevation = GC_NEW(int8_t *, gc->mapWidth + 1);
    for (int x = 0; x < gc->mapWidth + 1; ++x) {
        gc->tileMap[x] = GC_NEW(int8_t, gc->mapHeight + 1);
        gc->trackMap[x] = GC_NEW(int8_t, gc->mapHeight + 1);
        gc->tileElevation[x] = GC_NEW(int8_t, gc->mapHeight + 1);
    }
    gc->heightMap = GC_NEW(int8_t *, gc->mapWidth + 2);
    for (int x = 0; x < gc->mapWidth + 2; ++x)
        gc->heightMap[x] = GC_NEW(int8_t, gc->mapHeight + 2);
    gc->mapHalfWidth = (gc->mapWidth - 1) * 8;
    gc->mapScreenWidth = (gc->mapWidth + gc->mapHeight) * 16;
    gc->mapScreenHeight = (gc->mapWidth + gc->mapHeight) * 8;
    gc->viewportHalfWidth = gc->mapScreenWidth / 2;
    gc->viewportHalfHeight = gc->mapScreenHeight / 2;
    gc_initBuildMenuForScenario(gc, -1);
    gc_loadGame(gc, 1, 1);
    gc_decompressSaveBuffer(gc);
    gc_saveOrLoadByteMatrixInlined(gc, gc->tileMap);
    gc_loadGame(gc, 2, 1);
    gc_decompressSaveBuffer(gc);
    gc_saveOrLoadByteMatrixInlined(gc, gc->trackMap);
    gc_loadGame(gc, 3, 1);
    gc_decompressSaveBuffer(gc);
    gc_saveOrLoadByteMatrixInlined(gc, gc->tileElevation);
    gc_loadGame(gc, 4, 1);
    gc_decompressSaveBuffer(gc);
    gc_saveOrLoadByteMatrixInlined(gc, gc->heightMap);
    gc->lastViewportCenterX = -1;
}
