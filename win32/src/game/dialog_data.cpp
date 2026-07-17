// Dynamic values used by data-driven dialogs.
#include "game.h"
#include "support/numeric.h"

#include <stdio.h>
#include <string.h>

static std::vector<int8_t> format_number_with_suffix(int32_t value,
                                                      int32_t suffix) {
    char text[24];
    snprintf(text, sizeof(text), "%d", value);
    int length = (int)strlen(text);
    std::vector<int8_t> result(static_cast<size_t>(length + 1));
    for (int i = 0; i < length; ++i)
        result[i] = text[i] == '-' ? 22 : (int8_t)(text[i] - 47);
    result[length] = (int8_t)suffix;
    return result;
}

static std::vector<int8_t> encode_display_text(const char *text) {
    int length = (int)strlen(text);
    std::vector<int8_t> result(static_cast<size_t>(length));
    for (int i = 0; i < length; ++i) {
        char ch = text[i];
        result[i] = ch == '-' ? 22 : ch == '.' ? 11 : ch == '/' ? 20
                    : ch == '%' ? 29 : (int8_t)(ch - 47);
    }
    return result;
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

static std::vector<int8_t> format_currency(int32_t value) {
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

std::optional<std::vector<int8_t>> Game::resolve_dialog_value(
    int32_t screen, int32_t field) {
    switch (screen) {
        case 14:
            if (this->ui.screenParam >= 0 && this->ui.screenParam <= 4)
                return this->assets.index_text_codes(11 + this->ui.screenParam);
            break;

        case 4:
            switch (field) {
                case 1: return this->assets.index_text_codes(25 + this->scenario.scenarioMilestone);
                case 2: return this->assets.index_text_codes(30 + this->scenario.difficultyA);
                case 3: return this->assets.index_text_codes(35 + this->scenario.difficultySlot);
                case 4: return this->assets.index_text_codes(40 + this->scenario.difficultyB);
                case 5: return this->assets.index_text_codes(40 + this->scenario.difficultyC);
                case 6: return format_number_with_suffix(this->scenario.difficultyCycle, 0);
            }
            break;

        case 20:
            switch (field) {
                case 0: {
                    int selection = this->ui.trainSelectionIndex[this->build.junctionDirCursor];
                    const std::vector<int8_t> engine =
                        this->assets.index_text_codes(80 + ((selection >> 8) & 0xff));
                    const std::vector<int8_t> wagon =
                        this->assets.index_text_codes(86 + (selection & 0xff));
                    return join_codes(wagon, engine);
                }
                case 1: return format_number_with_suffix(this->world.zonePopulation[this->build.junctionDirCursor] * 13, 0);
                case 2: return format_number_with_suffix(this->world.constructionTimers[this->build.junctionDirCursor], 29);
            }
            break;

        case 21:
            switch (field) {
                case 0: return this->assets.index_text_codes(100 + this->world.cities[this->build.stationDirCursor].stationDirection);
                case 1: return this->assets.index_text_codes(106 + this->world.cities[this->build.stationDirCursor].stationDirection / 2);
                case 2: return format_number_with_suffix(this->world.cities[this->build.stationDirCursor].previousStationRevenue, 0);
            }
            break;

        case 17:
            return format_number_with_suffix(
                this->scenario.milestone_progress(this->economy), 29);

        case 10:
            switch (field) {
                case 1: return this->assets.index_text_codes(this->settings.animationsDisabled ? 21 : 20);
                case 2: return this->assets.index_text_codes(this->settings.selectionHidden ? 21 : 20);
                case 3: return this->assets.index_text_codes(this->ui.monthlyReportEnabled ? 20 : 21);
                case 4: return this->assets.index_text_codes(this->ui.annualReportEnabled ? 20 : 21);
                case 5: return this->assets.index_text_codes(this->music.enabled ? 20 : 21);
                case 6: return format_number_with_suffix(this->music.volumeLevel, 0);
            }
            break;

        case 24:
            switch (field) {
                case 0: return format_currency(this->economy.gWagonCost);
                case 1: return format_currency(this->economy.gWagonCost2);
                case 2: return format_currency(this->economy.gWagonCost3);
                case 3: return format_currency(this->economy.gWagonCost4);
                case 4: return format_currency(this->economy.gWagonCost5);
                case 5: return format_currency(this->economy.gWagonCost6);
            }
            break;

        case 23:
            return format_currency(this->build.unitCost - this->build.currentTrainCostSnapshot);

        case 22:
            return format_number_with_suffix(
                this->fleet.serviceSites[this->serviceVehicleAtCursor].resourceStock[field], 0);

        case 26: {
            if (field == 0)
                return this->assets.index_text_codes(60 + this->fleet.trainCars[this->trainAtCursor].era);
            if (field == 6) return format_currency(this->fleet.trainCars[this->trainAtCursor].revenue);
            if (field == 7) return format_currency(this->fleet.trainCars[this->trainAtCursor].previousRevenue);
            if (field == 8) return format_number_with_suffix(this->fleet.trainCars[this->trainAtCursor].ageYears, 0);
            if (field == 9) return format_currency(this->build.currentTrainCostSnapshot * 70 / 100);
            if (field == 10) return format_currency(this->build.baseTrainCostSnapshot - this->build.currentTrainCostSnapshot);
            if (field == 20)
                return this->assets.index_text_codes(
                    this->fleet.trainCars[this->trainAtCursor].aiOverride == 0 ? 21 : 20);
            if (field <= 0 || field >= 6) break;
            int car = 0;
            if (this->fleet.count_cars(this->trainAtCursor) > field) {
                car = this->trainAtCursor;
                for (int i = 0; i < field; ++i) car = this->fleet.trainCars[car].nextCar;
            }
            int type = train_flag::type(this->fleet.trainCars[car].flags);
            char text[32];
            if (type == 2)
                snprintf(text, sizeof(text), "%d/%d", this->fleet.trainCars[car].energyCargoOrRemovalTicks,
                         this->fleet.trainCars[car].secondaryCargoOrBaseEnergy);
            else
                snprintf(text, sizeof(text), "%d", this->fleet.trainCars[car].energyCargoOrRemovalTicks);
            return encode_display_text(text);
        }

        case 30:
            switch (field) {
                case 0: return format_currency(this->economy.loanBalance);
                case 1: return format_currency(this->economy.loanCeiling);
                case 2: return format_number_with_suffix(this->economy.interestRate, 29);
            }
            break;

        case 32: {
            int values[8] = {
                this->compareToPrevious ? this->economy.prevTotalSpent : this->economy.totalSpent,
                this->compareToPrevious ? this->economy.prevAiMoneySpent : this->economy.aiMoneySpent,
                this->compareToPrevious ? this->economy.prevFuelCost : this->economy.fuelCostAccrued,
                this->compareToPrevious ? this->economy.prevSubsidyCost : this->economy.subsidyCost,
                this->compareToPrevious ? this->economy.prevTerrainTotal : this->economy.terrainTotal,
                this->compareToPrevious ? this->economy.prevEnterpriseTotal : this->economy.decadeEnterpriseTax,
                this->compareToPrevious ? this->economy.prevTotalRevenue : this->economy.totalRevenue,
                this->compareToPrevious ? this->economy.previousYearProfit : this->economy.netProfit
            };
            if (field >= 0 && field < 8) return format_currency(values[field]);
            break;
        }

        case 35: {
            int values[8] = {
                this->compareToPrevious ? this->economy.prevDecadeSpent : this->economy.decadeSpentWithCurrent,
                this->compareToPrevious ? this->economy.prevDecadeAiSpent : this->economy.decadeAiSpentWithCurrent,
                this->compareToPrevious ? this->economy.prevDecadeFuel : this->economy.decadeFuelWithCurrent,
                this->compareToPrevious ? this->economy.prevDecadeSubsidy : this->economy.decadeSubsidyWithCurrent,
                this->compareToPrevious ? this->economy.previousLegacyDecadeMetric : this->economy.legacyDecadeMetric,
                this->compareToPrevious ? this->economy.previousDecadeEnterpriseTax : this->economy.decadeEnterpriseWithCurrent,
                this->compareToPrevious ? this->economy.prevDecadeRevenue : this->economy.decadeRevenueWithCurrent,
                this->compareToPrevious ? this->economy.previousDecadeProfit : this->economy.decadeProfitWithCurrent
            };
            if (field >= 0 && field < 8) return format_currency(values[field]);
            break;
        }

        case 33:
            if (field == 65)
                return this->assets.index_text_codes(57 + this->ui.yearGraphMode);
            if (field >= 40) {
                int index = field - 40;
                if (!this->ui.yearGraphAlt) {
                    if (this->ui.yearGraphMode == 0) return format_currency(this->economy.yearProfitHistory[index]);
                    if (this->ui.yearGraphMode == 1) return format_currency(this->economy.yearRevenueHistory[index]);
                    if (this->ui.yearGraphMode == 2) return format_currency(this->economy.yearCompanyValueHistory[index]);
                } else {
                    if (this->ui.yearGraphMode == 0) return format_currency(this->economy.decadeProfitHistory[index]);
                    if (this->ui.yearGraphMode == 1) return format_currency(this->economy.altDecadeRevenueTotal[index]);
                    if (this->ui.yearGraphMode == 2) return format_currency(this->economy.altDecadeTotalCost[index]);
                }
            } else {
                if (field >= 20) {
                    if (!this->ui.yearGraphAlt)
                        return format_number_with_suffix(this->ui.altYearArray[field - 20], 0);
                    return std::nullopt;
                }
                return !this->ui.yearGraphAlt
                    ? format_number_with_suffix(this->ui.yearGraphOffsets[field] + 1940, 20)
                    : format_number_with_suffix(this->ui.altYearArray[field] + 1940, 0);
            }
            // The original falls through when the graph mode is invalid.
        case 34:
            if (field == 65)
                return this->assets.index_text_codes(54 + this->ui.yearGraphMode);
            if (field >= 20) {
                if (!this->ui.yearGraphAlt)
                    return format_number_with_suffix(this->ui.altYearArray[field - 20], 0);
                return std::nullopt;
            }
            return !this->ui.yearGraphAlt
                ? format_number_with_suffix(this->ui.yearGraphOffsets[field] + 40, 20)
                : format_number_with_suffix(this->ui.altYearArray[field] + 1940, 0);

        case 41: case 42: case 44: case 45: case 46: case 47: case 48:
        case 49: case 50: case 51: case 52: case 53: case 54:
            if (field == 1) {
                const auto day = format_number_with_suffix(this->offerBoard.offers[this->scenario.offerRowIndex].dueDay, 11);
                const auto month = format_number_with_suffix(this->offerBoard.offers[this->scenario.offerRowIndex].dueMonth, 11);
                const auto year = format_number_with_suffix(this->offerBoard.offers[this->scenario.offerRowIndex].dueYear + 1950, 0);
                return join_three_codes(day, month, year);
            }
            if (field == 2) return format_currency(this->offerBoard.offers[this->scenario.offerRowIndex].target);
            if (field == 3) return format_number_with_suffix(
                this->offerBoard.display_progress(this->scenario.offerRowIndex,
                                                  this->economy), 29);
            if (field == 4) return format_number_with_suffix(this->offerBoard.offers[this->scenario.offerRowIndex].target, 0);
            break;

        case 60:
            if (field % 2 == 1) {
                int index = field / 2;
                return this->assets.index_text_codes(
                    this->offerBoard.offers[
                        this->ui.enabledMenuRowIndexes[index]].objectiveType + 160);
            } else {
                int index = (field - 1) / 2;
                return format_number_with_suffix(
                    this->offerBoard.display_progress(
                        this->ui.enabledMenuRowIndexes[index], this->economy), 29);
            }

        case 61: case 62: case 63: case 64: case 65: case 66:
        case 67: case 68: case 69: case 70: case 71: case 72:
            if (field == 1) return format_number_with_suffix(this->offerBoard.offers[this->scenario.offerRowIndex].effectValue, 29);
            if (field == 2) return format_currency(this->offerBoard.offers[this->scenario.offerRowIndex].effectAmount);
            if (field == 3) return format_number_with_suffix(this->offerBoard.offers[this->scenario.offerRowIndex].effectValue, 0);
            break;

        case 27:
            if (field == 1)
                return copy_codes(this->scenario.playerNameCodes, kPlayerNameCodeCount);
            if (field == 0) {
                memset(this->scenario.playerNameCursorMarker, 0,
                       sizeof(this->scenario.playerNameCursorMarker));
                this->scenario.playerNameCursorMarker[this->scenario.playerNameCursor] = 28;
                return copy_codes(this->scenario.playerNameCursorMarker, kPlayerNameCodeCount);
            }
            break;

        case 28:
            if (field == 0) return format_number_with_suffix(this->scenario.bestScenarioScore, 0);
            if (field >= 1 && field <= 13)
                return format_number_with_suffix(this->scenario.scenarioScores[field - 1], 0);
            if (field >= 20 && field <= 33)
                return format_number_with_suffix(this->scenario.leaderboardScores[field - 20], 0);
            break;

        case 6:
            return format_number_with_suffix(this->scenario.currentScenarioScore, 0);
    }
    return std::nullopt;
}
