// Dynamic values used by data-driven dialogs.
#include "game_methods.h"

#include <stdio.h>
#include <string.h>

static std::span<const int8_t> tracked_codes(const std::vector<int8_t> &codes) {
    return {codes.data(), codes.size()};
}

static std::vector<int8_t> copy_codes(const std::vector<int8_t> &codes) {
    return codes;
}

// Owned code arrays carry their own length, so copy the whole array.
template <std::size_t N>
static std::vector<int8_t> copy_codes(const std::array<int8_t, N> &codes) {
    return {codes.begin(), codes.end()};
}

static std::vector<int8_t> join_codes(std::span<const int8_t> first,
                                      std::span<const int8_t> second) {
    std::vector<int8_t> result;
    result.reserve(first.size() + second.size());
    result.insert(result.end(), first.begin(), first.end());
    result.insert(result.end(), second.begin(), second.end());
    return result;
}

static std::vector<int8_t> join_three_codes(std::span<const int8_t> first,
                                            std::span<const int8_t> second,
                                            std::span<const int8_t> third) {
    std::vector<int8_t> result;
    result.reserve(first.size() + second.size() + third.size());
    result.insert(result.end(), first.begin(), first.end());
    result.insert(result.end(), second.begin(), second.end());
    result.insert(result.end(), third.begin(), third.end());
    return result;
}

std::vector<int8_t> gc_formatCurrency(GameCanvas *gc, int32_t value) {
    (void)gc;
    uint32_t magnitude = value < 0 ? 0u - (uint32_t)value : (uint32_t)value;
    char digits[16];
    snprintf(digits, sizeof(digits), "%u", magnitude);
    int digit_count = (int)strlen(digits);
    int separator_count = (digit_count - 1) / 3;
    int text_count = digit_count + separator_count + (value < 0 ? 1 : 0);
    std::vector<int8_t> result(static_cast<size_t>(text_count + 1));
    int out = 0;
    result[out++] = 24;
    if (value < 0) result[out++] = 22;
    for (int i = 0; i < digit_count; ++i) {
        result[out++] = (int8_t)(digits[i] - 47);
        int remaining = digit_count - i - 1;
        if (remaining > 0 && remaining % 3 == 0) result[out++] = 11;
    }
    return result;
}

std::optional<std::vector<int8_t>> gc_resolveDialogValue(
    GameCanvas *gc, int32_t screen, int32_t field) {
    switch (screen) {
        case 14:
            if (gc->screenParam >= 0 && gc->screenParam <= 4)
                return copy_codes(gc->spriteBitmapTable[
                    gc->spriteIndexTable[11 + gc->screenParam]]);
            break;

        case 4:
            switch (field) {
                case 1: return copy_codes(gc->spriteBitmapTable[gc->spriteIndexTable[25 + gc->scenarioMilestone]]);
                case 2: return copy_codes(gc->spriteBitmapTable[gc->spriteIndexTable[30 + gc->difficultyA]]);
                case 3: return copy_codes(gc->spriteBitmapTable[gc->spriteIndexTable[35 + gc->difficultySlot]]);
                case 4: return copy_codes(gc->spriteBitmapTable[gc->spriteIndexTable[40 + gc->difficultyB]]);
                case 5: return copy_codes(gc->spriteBitmapTable[gc->spriteIndexTable[40 + gc->difficultyC]]);
                case 6: return gc_formatNumberWithSuffix(gc, gc->difficultyCycle, 0);
            }
            break;

        case 20:
            switch (field) {
                case 0: {
                    int selection = gc->trainSelectionIndex[gc->junctionDirCursor];
                    const std::vector<int8_t> &engine = gc->spriteBitmapTable[
                        gc->spriteIndexTable[80 + ((selection >> 8) & 0xff)]];
                    const std::vector<int8_t> &wagon = gc->spriteBitmapTable[
                        gc->spriteIndexTable[86 + (selection & 0xff)]];
                    return join_codes(tracked_codes(wagon), tracked_codes(engine));
                }
                case 1: return gc_formatNumberWithSuffix(gc, gc->zonePopulation[gc->junctionDirCursor] * 13, 0);
                case 2: return gc_formatNumberWithSuffix(gc, gc->constructionTimers[gc->junctionDirCursor], 29);
            }
            break;

        case 21:
            switch (field) {
                case 0: return copy_codes(gc->spriteBitmapTable[gc->spriteIndexTable[100 + gc->world.stationDirectionCode[gc->stationDirCursor]]]);
                case 1: return copy_codes(gc->spriteBitmapTable[gc->spriteIndexTable[106 + gc->world.stationDirectionCode[gc->stationDirCursor] / 2]]);
                case 2: return gc_formatNumberWithSuffix(gc, gc->prevStationRevenue[gc->stationDirCursor], 0);
            }
            break;

        case 17:
            return gc_formatNumberWithSuffix(gc, gc_computeMilestoneScore(gc), 29);

        case 10:
            switch (field) {
                case 1: return copy_codes(gc->spriteBitmapTable[gc->spriteIndexTable[gcs_animationsDisabled ? 21 : 20]]);
                case 2: return copy_codes(gc->spriteBitmapTable[gc->spriteIndexTable[gcs_selectionHidden ? 21 : 20]]);
                case 3: return copy_codes(gc->spriteBitmapTable[gc->spriteIndexTable[gcs_monthlyReportEnabled ? 20 : 21]]);
                case 4: return copy_codes(gc->spriteBitmapTable[gc->spriteIndexTable[gcs_annualReportEnabled ? 20 : 21]]);
                case 5: return copy_codes(gc->spriteBitmapTable[gc->spriteIndexTable[gc->music_enabled ? 20 : 21]]);
                case 6: return gc_formatNumberWithSuffix(gc, gc->music_volume_level, 0);
            }
            break;

        case 24:
            switch (field) {
                case 0: return gc_formatCurrency(gc, gc->gWagonCost);
                case 1: return gc_formatCurrency(gc, gc->gWagonCost2);
                case 2: return gc_formatCurrency(gc, gc->gWagonCost3);
                case 3: return gc_formatCurrency(gc, gc->gWagonCost4);
                case 4: return gc_formatCurrency(gc, gc->gWagonCost5);
                case 5: return gc_formatCurrency(gc, gc->gWagonCost6);
            }
            break;

        case 23:
            return gc_formatCurrency(gc, gc->unitCost - gc->currentTrainCostSnapshot);

        case 22:
            return gc_formatNumberWithSuffix(
                gc, gc->fleet.serviceResourceStock[gc->serviceVehicleAtCursor][field], 0);

        case 26: {
            if (field == 0)
                return copy_codes(gc->spriteBitmapTable[gc->spriteIndexTable[60 + gc->fleet.trainEraOrStage[gc->trainAtCursor]]]);
            if (field == 6) return gc_formatCurrency(gc, gc->fleet.trainRevenue[gc->trainAtCursor]);
            if (field == 7) return gc_formatCurrency(gc, gc->fleet.prevTrainRevenue[gc->trainAtCursor]);
            if (field == 8) return gc_formatNumberWithSuffix(gc, gc->fleet.trainAgeOrTimer[gc->trainAtCursor], 0);
            if (field == 9) return gc_formatCurrency(gc, gc->currentTrainCostSnapshot * 70 / 100);
            if (field == 10) return gc_formatCurrency(gc, gc->baseTrainCostSnapshot - gc->currentTrainCostSnapshot);
            if (field == 20)
                return copy_codes(gc->spriteBitmapTable[gc->spriteIndexTable[
                    gc->fleet.trainAiOverride[gc->trainAtCursor] == 0 ? 21 : 20]]);
            if (field <= 0 || field >= 6) break;
            int car = 0;
            if (gc_countCarsInTrain(gc, gc->trainAtCursor) > field) {
                car = gc->trainAtCursor;
                for (int i = 0; i < field; ++i) car = gc->fleet.nextCar[car];
            }
            int type = gc->fleet.trainFlags[car] & 0x0f;
            char text[32];
            if (type == 2)
                snprintf(text, sizeof(text), "%d/%d", gc->fleet.trainEnergyOrCount[car],
                         gc->fleet.wagonEnergyRemaining[car]);
            else
                snprintf(text, sizeof(text), "%d", gc->fleet.trainEnergyOrCount[car]);
            return gc_encodeDisplayText(gc, text);
        }

        case 30:
            switch (field) {
                case 0: return gc_formatCurrency(gc, gc->loanBalance);
                case 1: return gc_formatCurrency(gc, gc->loanCeiling);
                case 2: return gc_formatNumberWithSuffix(gc, gc->interestRate, 29);
            }
            break;

        case 32: {
            int values[8] = {
                gc->compareToPrevious ? gc->prevTotalSpent : gc->totalSpent,
                gc->compareToPrevious ? gc->prevAiMoneySpent : gc->aiMoneySpent,
                gc->compareToPrevious ? gc->prevFuelCost : gc->fuelCostAccrued,
                gc->compareToPrevious ? gc->prevSubsidyCost : gc->subsidyCost,
                gc->compareToPrevious ? gc->prevTerrainTotal : gc->terrainTotal,
                gc->compareToPrevious ? gc->prevEnterpriseTotal : gc->decadeEnterpriseTax,
                gc->compareToPrevious ? gc->prevTotalRevenue : gc->totalRevenue,
                gc->compareToPrevious ? gc->previousYearProfit : gc->netProfit
            };
            if (field >= 0 && field < 8) return gc_formatCurrency(gc, values[field]);
            break;
        }

        case 35: {
            int values[8] = {
                gc->compareToPrevious ? gc->prevDecadeSpent : gc->decadeSpentWithCurrent,
                gc->compareToPrevious ? gc->prevDecadeAiSpent : gc->decadeAiSpentWithCurrent,
                gc->compareToPrevious ? gc->prevDecadeFuel : gc->decadeFuelWithCurrent,
                gc->compareToPrevious ? gc->prevDecadeSubsidy : gc->decadeSubsidyWithCurrent,
                gc->compareToPrevious ? gc->previousLegacyDecadeMetric : gc->legacyDecadeMetric,
                gc->compareToPrevious ? gc->previousDecadeEnterpriseTax : gc->decadeEnterpriseWithCurrent,
                gc->compareToPrevious ? gc->prevDecadeRevenue : gc->decadeRevenueWithCurrent,
                gc->compareToPrevious ? gc->previousDecadeProfit : gc->decadeProfitWithCurrent
            };
            if (field >= 0 && field < 8) return gc_formatCurrency(gc, values[field]);
            break;
        }

        case 33:
            if (field == 65)
                return copy_codes(gc->spriteBitmapTable[gc->spriteIndexTable[57 + gc->yearGraphMode]]);
            if (field >= 40) {
                int index = field - 40;
                if (!gc->yearGraphAlt) {
                    if (gc->yearGraphMode == 0) return gc_formatCurrency(gc, gc->yearProfitHistory[index]);
                    if (gc->yearGraphMode == 1) return gc_formatCurrency(gc, gc->yearRevenueHistory[index]);
                    if (gc->yearGraphMode == 2) return gc_formatCurrency(gc, gc->yearCompanyValueHistory[index]);
                } else {
                    if (gc->yearGraphMode == 0) return gc_formatCurrency(gc, gc->decadeProfitHistory[index]);
                    if (gc->yearGraphMode == 1) return gc_formatCurrency(gc, gc->altDecadeRevenueTotal[index]);
                    if (gc->yearGraphMode == 2) return gc_formatCurrency(gc, gc->altDecadeTotalCost[index]);
                }
            } else {
                if (field >= 20) {
                    if (!gc->yearGraphAlt)
                        return gc_formatNumberWithSuffix(gc, gc->altYearArray[field - 20], 0);
                    return std::nullopt;
                }
                return !gc->yearGraphAlt
                    ? gc_formatNumberWithSuffix(gc, gc->yearGraphOffsets[field] + 1940, 20)
                    : gc_formatNumberWithSuffix(gc, gc->altYearArray[field] + 1940, 0);
            }
            // The original falls through when the graph mode is invalid.
        case 34:
            if (field == 65)
                return copy_codes(gc->spriteBitmapTable[gc->spriteIndexTable[54 + gc->yearGraphMode]]);
            if (field >= 20) {
                if (!gc->yearGraphAlt)
                    return gc_formatNumberWithSuffix(gc, gc->altYearArray[field - 20], 0);
                return std::nullopt;
            }
            return !gc->yearGraphAlt
                ? gc_formatNumberWithSuffix(gc, gc->yearGraphOffsets[field] + 40, 20)
                : gc_formatNumberWithSuffix(gc, gc->altYearArray[field] + 1940, 0);

        case 41: case 42: case 44: case 45: case 46: case 47: case 48:
        case 49: case 50: case 51: case 52: case 53: case 54:
            if (field == 1) {
                const auto day = gc_formatNumberWithSuffix(gc, gc->menuRowParamA[gc->offerRowIndex], 11);
                const auto month = gc_formatNumberWithSuffix(gc, gc->menuRowParamB[gc->offerRowIndex], 11);
                const auto year = gc_formatNumberWithSuffix(gc, gc->menuRowParamC[gc->offerRowIndex] + 1950, 0);
                return join_three_codes(day, month, year);
            }
            if (field == 2) return gc_formatCurrency(gc, gc->menuRowCost[gc->offerRowIndex]);
            if (field == 3) return gc_formatNumberWithSuffix(gc, gc_computeMenuRowDisplayValue(gc, gc->offerRowIndex), 29);
            if (field == 4) return gc_formatNumberWithSuffix(gc, gc->menuRowCost[gc->offerRowIndex], 0);
            break;

        case 60:
            if (field % 2 == 1) {
                int index = field / 2;
                return copy_codes(gc->spriteBitmapTable[gc_byteToUnsignedInt(
                    gc, gc->spriteIndexTable[gc->menuRowType[gc->enabledMenuRowIndexes[index]] + 160])]);
            } else {
                int index = (field - 1) / 2;
                return gc_formatNumberWithSuffix(gc,
                    gc_computeMenuRowDisplayValue(gc, gc->enabledMenuRowIndexes[index]), 29);
            }

        case 61: case 62: case 63: case 64: case 65: case 66:
        case 67: case 68: case 69: case 70: case 71: case 72:
            if (field == 1) return gc_formatNumberWithSuffix(gc, gc->menuRowEra[gc->offerRowIndex], 29);
            if (field == 2) return gc_formatCurrency(gc, gc->menuRowRevenue[gc->offerRowIndex]);
            if (field == 3) return gc_formatNumberWithSuffix(gc, gc->menuRowEra[gc->offerRowIndex], 0);
            break;

        case 27:
            if (field == 1) return copy_codes(gc->playerNameCodes);
            if (field == 0) {
                gc->playerNameCursorMarker.fill(0);
                gc->playerNameCursorMarker[gc->routeSlotIndex] = 28;
                return copy_codes(gc->playerNameCursorMarker);
            }
            break;

        case 28:
            if (field == 0) return gc_formatNumberWithSuffix(gc, gc->bestScenarioScore, 0);
            if (field >= 1 && field <= 13)
                return gc_formatNumberWithSuffix(gc, gc->scenarioScores[field - 1], 0);
            if (field >= 20 && field <= 33)
                return gc_formatNumberWithSuffix(gc, gc->leaderboardScores[field - 20], 0);
            break;

        case 6:
            return gc_formatNumberWithSuffix(gc, gc->currentScenarioScore, 0);
    }
    return std::nullopt;
}
