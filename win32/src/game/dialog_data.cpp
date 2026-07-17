// Dynamic values used by data-driven dialogs.
#include "game_internal.h"

#include <stdio.h>
#include <string.h>

static std::vector<int8_t> copy_codes(const std::vector<int8_t> &codes) {
    return codes;
}

// Owned code arrays carry their own length, so copy the whole array.
static std::vector<int8_t> copy_codes(const int8_t* codes, int count) {
    return {codes, codes + count};
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

std::vector<int8_t> game_formatCurrency(int32_t value) {
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

std::optional<std::vector<int8_t>> game_resolveDialogValue(
    Game *game, int32_t screen, int32_t field) {
    switch (screen) {
        case 14:
            if (game->ui.screenParam >= 0 && game->ui.screenParam <= 4)
                return copy_codes(game->assets.spriteBitmapTable[
                    game->assets.spriteIndexTable[11 + game->ui.screenParam]]);
            break;

        case 4:
            switch (field) {
                case 1: return copy_codes(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[25 + game->scenario.scenarioMilestone]]);
                case 2: return copy_codes(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[30 + game->scenario.difficultyA]]);
                case 3: return copy_codes(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[35 + game->scenario.difficultySlot]]);
                case 4: return copy_codes(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[40 + game->scenario.difficultyB]]);
                case 5: return copy_codes(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[40 + game->scenario.difficultyC]]);
                case 6: return game_formatNumberWithSuffix(game->scenario.difficultyCycle, 0);
            }
            break;

        case 20:
            switch (field) {
                case 0: {
                    int selection = game->ui.trainSelectionIndex[game->build.junctionDirCursor];
                    const std::vector<int8_t> &engine = game->assets.spriteBitmapTable[
                        game->assets.spriteIndexTable[80 + ((selection >> 8) & 0xff)]];
                    const std::vector<int8_t> &wagon = game->assets.spriteBitmapTable[
                        game->assets.spriteIndexTable[86 + (selection & 0xff)]];
                    return join_codes(wagon, engine);
                }
                case 1: return game_formatNumberWithSuffix(game->world.zonePopulation[game->build.junctionDirCursor] * 13, 0);
                case 2: return game_formatNumberWithSuffix(game->world.constructionTimers[game->build.junctionDirCursor], 29);
            }
            break;

        case 21:
            switch (field) {
                case 0: return copy_codes(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[100 + game->world.cities[game->build.stationDirCursor].stationDirection]]);
                case 1: return copy_codes(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[106 + game->world.cities[game->build.stationDirCursor].stationDirection / 2]]);
                case 2: return game_formatNumberWithSuffix(game->world.cities[game->build.stationDirCursor].previousStationRevenue, 0);
            }
            break;

        case 17:
            return game_formatNumberWithSuffix(game_computeMilestoneScore(game), 29);

        case 10:
            switch (field) {
                case 1: return copy_codes(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[animationsDisabled ? 21 : 20]]);
                case 2: return copy_codes(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[selectionHidden ? 21 : 20]]);
                case 3: return copy_codes(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[monthlyReportEnabled ? 20 : 21]]);
                case 4: return copy_codes(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[annualReportEnabled ? 20 : 21]]);
                case 5: return copy_codes(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[game->music_enabled ? 20 : 21]]);
                case 6: return game_formatNumberWithSuffix(game->music_volume_level, 0);
            }
            break;

        case 24:
            switch (field) {
                case 0: return game_formatCurrency(game->economy.gWagonCost);
                case 1: return game_formatCurrency(game->economy.gWagonCost2);
                case 2: return game_formatCurrency(game->economy.gWagonCost3);
                case 3: return game_formatCurrency(game->economy.gWagonCost4);
                case 4: return game_formatCurrency(game->economy.gWagonCost5);
                case 5: return game_formatCurrency(game->economy.gWagonCost6);
            }
            break;

        case 23:
            return game_formatCurrency(game->build.unitCost - game->build.currentTrainCostSnapshot);

        case 22:
            return game_formatNumberWithSuffix(
                game->fleet.serviceSites[game->serviceVehicleAtCursor].resourceStock[field], 0);

        case 26: {
            if (field == 0)
                return copy_codes(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[60 + game->fleet.trainCars[game->trainAtCursor].era]]);
            if (field == 6) return game_formatCurrency(game->fleet.trainCars[game->trainAtCursor].revenue);
            if (field == 7) return game_formatCurrency(game->fleet.trainCars[game->trainAtCursor].previousRevenue);
            if (field == 8) return game_formatNumberWithSuffix(game->fleet.trainCars[game->trainAtCursor].ageYears, 0);
            if (field == 9) return game_formatCurrency(game->build.currentTrainCostSnapshot * 70 / 100);
            if (field == 10) return game_formatCurrency(game->build.baseTrainCostSnapshot - game->build.currentTrainCostSnapshot);
            if (field == 20)
                return copy_codes(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[
                    game->fleet.trainCars[game->trainAtCursor].aiOverride == 0 ? 21 : 20]]);
            if (field <= 0 || field >= 6) break;
            int car = 0;
            if (game->fleet.count_cars(game->trainAtCursor) > field) {
                car = game->trainAtCursor;
                for (int i = 0; i < field; ++i) car = game->fleet.trainCars[car].nextCar;
            }
            int type = train_flag::type(game->fleet.trainCars[car].flags);
            char text[32];
            if (type == 2)
                snprintf(text, sizeof(text), "%d/%d", game->fleet.trainCars[car].energyCargoOrRemovalTicks,
                         game->fleet.trainCars[car].secondaryCargoOrBaseEnergy);
            else
                snprintf(text, sizeof(text), "%d", game->fleet.trainCars[car].energyCargoOrRemovalTicks);
            return game_encodeDisplayText(text);
        }

        case 30:
            switch (field) {
                case 0: return game_formatCurrency(game->economy.loanBalance);
                case 1: return game_formatCurrency(game->economy.loanCeiling);
                case 2: return game_formatNumberWithSuffix(game->economy.interestRate, 29);
            }
            break;

        case 32: {
            int values[8] = {
                game->compareToPrevious ? game->economy.prevTotalSpent : game->economy.totalSpent,
                game->compareToPrevious ? game->economy.prevAiMoneySpent : game->economy.aiMoneySpent,
                game->compareToPrevious ? game->economy.prevFuelCost : game->economy.fuelCostAccrued,
                game->compareToPrevious ? game->economy.prevSubsidyCost : game->economy.subsidyCost,
                game->compareToPrevious ? game->economy.prevTerrainTotal : game->economy.terrainTotal,
                game->compareToPrevious ? game->economy.prevEnterpriseTotal : game->economy.decadeEnterpriseTax,
                game->compareToPrevious ? game->economy.prevTotalRevenue : game->economy.totalRevenue,
                game->compareToPrevious ? game->economy.previousYearProfit : game->economy.netProfit
            };
            if (field >= 0 && field < 8) return game_formatCurrency(values[field]);
            break;
        }

        case 35: {
            int values[8] = {
                game->compareToPrevious ? game->economy.prevDecadeSpent : game->economy.decadeSpentWithCurrent,
                game->compareToPrevious ? game->economy.prevDecadeAiSpent : game->economy.decadeAiSpentWithCurrent,
                game->compareToPrevious ? game->economy.prevDecadeFuel : game->economy.decadeFuelWithCurrent,
                game->compareToPrevious ? game->economy.prevDecadeSubsidy : game->economy.decadeSubsidyWithCurrent,
                game->compareToPrevious ? game->economy.previousLegacyDecadeMetric : game->economy.legacyDecadeMetric,
                game->compareToPrevious ? game->economy.previousDecadeEnterpriseTax : game->economy.decadeEnterpriseWithCurrent,
                game->compareToPrevious ? game->economy.prevDecadeRevenue : game->economy.decadeRevenueWithCurrent,
                game->compareToPrevious ? game->economy.previousDecadeProfit : game->economy.decadeProfitWithCurrent
            };
            if (field >= 0 && field < 8) return game_formatCurrency(values[field]);
            break;
        }

        case 33:
            if (field == 65)
                return copy_codes(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[57 + game->ui.yearGraphMode]]);
            if (field >= 40) {
                int index = field - 40;
                if (!game->ui.yearGraphAlt) {
                    if (game->ui.yearGraphMode == 0) return game_formatCurrency(game->economy.yearProfitHistory[index]);
                    if (game->ui.yearGraphMode == 1) return game_formatCurrency(game->economy.yearRevenueHistory[index]);
                    if (game->ui.yearGraphMode == 2) return game_formatCurrency(game->economy.yearCompanyValueHistory[index]);
                } else {
                    if (game->ui.yearGraphMode == 0) return game_formatCurrency(game->economy.decadeProfitHistory[index]);
                    if (game->ui.yearGraphMode == 1) return game_formatCurrency(game->economy.altDecadeRevenueTotal[index]);
                    if (game->ui.yearGraphMode == 2) return game_formatCurrency(game->economy.altDecadeTotalCost[index]);
                }
            } else {
                if (field >= 20) {
                    if (!game->ui.yearGraphAlt)
                        return game_formatNumberWithSuffix(game->ui.altYearArray[field - 20], 0);
                    return std::nullopt;
                }
                return !game->ui.yearGraphAlt
                    ? game_formatNumberWithSuffix(game->ui.yearGraphOffsets[field] + 1940, 20)
                    : game_formatNumberWithSuffix(game->ui.altYearArray[field] + 1940, 0);
            }
            // The original falls through when the graph mode is invalid.
        case 34:
            if (field == 65)
                return copy_codes(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[54 + game->ui.yearGraphMode]]);
            if (field >= 20) {
                if (!game->ui.yearGraphAlt)
                    return game_formatNumberWithSuffix(game->ui.altYearArray[field - 20], 0);
                return std::nullopt;
            }
            return !game->ui.yearGraphAlt
                ? game_formatNumberWithSuffix(game->ui.yearGraphOffsets[field] + 40, 20)
                : game_formatNumberWithSuffix(game->ui.altYearArray[field] + 1940, 0);

        case 41: case 42: case 44: case 45: case 46: case 47: case 48:
        case 49: case 50: case 51: case 52: case 53: case 54:
            if (field == 1) {
                const auto day = game_formatNumberWithSuffix(game->offerBoard.offers[game->scenario.offerRowIndex].dueDay, 11);
                const auto month = game_formatNumberWithSuffix(game->offerBoard.offers[game->scenario.offerRowIndex].dueMonth, 11);
                const auto year = game_formatNumberWithSuffix(game->offerBoard.offers[game->scenario.offerRowIndex].dueYear + 1950, 0);
                return join_three_codes(day, month, year);
            }
            if (field == 2) return game_formatCurrency(game->offerBoard.offers[game->scenario.offerRowIndex].target);
            if (field == 3) return game_formatNumberWithSuffix(
                game->offerBoard.display_progress(game->scenario.offerRowIndex,
                                                  game->economy), 29);
            if (field == 4) return game_formatNumberWithSuffix(game->offerBoard.offers[game->scenario.offerRowIndex].target, 0);
            break;

        case 60:
            if (field % 2 == 1) {
                int index = field / 2;
                return copy_codes(game->assets.spriteBitmapTable[unsignedByte(
                    game->assets.spriteIndexTable[game->offerBoard.offers[
                        game->ui.enabledMenuRowIndexes[index]].objectiveType + 160])]);
            } else {
                int index = (field - 1) / 2;
                return game_formatNumberWithSuffix(
                    game->offerBoard.display_progress(
                        game->ui.enabledMenuRowIndexes[index], game->economy), 29);
            }

        case 61: case 62: case 63: case 64: case 65: case 66:
        case 67: case 68: case 69: case 70: case 71: case 72:
            if (field == 1) return game_formatNumberWithSuffix(game->offerBoard.offers[game->scenario.offerRowIndex].effectValue, 29);
            if (field == 2) return game_formatCurrency(game->offerBoard.offers[game->scenario.offerRowIndex].effectAmount);
            if (field == 3) return game_formatNumberWithSuffix(game->offerBoard.offers[game->scenario.offerRowIndex].effectValue, 0);
            break;

        case 27:
            if (field == 1)
                return copy_codes(game->scenario.playerNameCodes, kPlayerNameCodeCount);
            if (field == 0) {
                memset(game->scenario.playerNameCursorMarker, 0,
                       sizeof(game->scenario.playerNameCursorMarker));
                game->scenario.playerNameCursorMarker[game->scenario.routeSlotIndex] = 28;
                return copy_codes(game->scenario.playerNameCursorMarker, kPlayerNameCodeCount);
            }
            break;

        case 28:
            if (field == 0) return game_formatNumberWithSuffix(game->scenario.bestScenarioScore, 0);
            if (field >= 1 && field <= 13)
                return game_formatNumberWithSuffix(game->scenario.scenarioScores[field - 1], 0);
            if (field >= 20 && field <= 33)
                return game_formatNumberWithSuffix(game->scenario.leaderboardScores[field - 20], 0);
            break;

        case 6:
            return game_formatNumberWithSuffix(game->scenario.currentScenarioScore, 0);
    }
    return std::nullopt;
}
