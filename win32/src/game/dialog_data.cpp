// Dynamic values used by data-driven dialogs.
#include "game.h"
#include "support/numeric.h"
#include "text/text_catalog.h"

#include <stdio.h>
#include <string.h>

namespace {

namespace display_code {
constexpr int32_t kAsciiDigitOffset = 47;
constexpr int32_t kMinusSign = 22;
constexpr int32_t kPeriod = 11;
constexpr int32_t kDateSeparator = 20;
constexpr int32_t kPercent = 29;
constexpr int32_t kCurrencyPrefix = 24;
constexpr int32_t kNoSuffix = 0;
constexpr int32_t kThousandsGroupSize = 3;
}  // namespace display_code

namespace dialog_value {
constexpr int32_t kScenarioChoiceTextBase = 11;
constexpr int32_t kFinancialDifficultyTextBase = 30;
constexpr int32_t kAiDifficultyTextBase = 35;
constexpr int32_t kScenarioChoiceCount = 5;
constexpr int32_t kJunctionEngineTextBase = 80;
constexpr int32_t kJunctionWagonTextBase = 86;
constexpr int32_t kPackedSelectionByteMask = 0xff;
constexpr int32_t kPopulationScale = 13;
constexpr int32_t kStationDirectionTextBase = 100;
constexpr int32_t kStationOrientationTextBase = 106;
constexpr int32_t kStationOrientationDivisor = 2;
constexpr int32_t kEnabledTextCode = 20;
constexpr int32_t kDisabledTextCode = 21;
constexpr int32_t kTrainEraTextBase = 60;
constexpr int32_t kCurrentCostPercent = 70;
constexpr int32_t kGraphModeField = 65;
constexpr int32_t kYearGraphModeTextBase = 57;
constexpr int32_t kDecadeGraphModeTextBase = 54;
constexpr int32_t kGraphHistoryFieldBase = 40;
constexpr int32_t kGraphYearFieldBase = 20;
constexpr int32_t kDecadeYearOffset = 40;
constexpr int32_t kLegacyYearBase = 1940;
constexpr int32_t kOfferObjectiveTextBase = 160;
constexpr int32_t kOfferFieldPairWidth = 2;
constexpr int32_t kFirstOfferProgressField = 1;
constexpr int32_t kNoAiOverride = 0;
constexpr int32_t kInitialCarSlot = 0;
constexpr int32_t kPlayerNameCursorCode = 28;
}  // namespace dialog_value

template <std::size_t N>
std::optional<std::vector<int8_t>> free_game_level_text(
    int32_t level, const Text (&keys)[N]) {
    if (level < 0 || level >= static_cast<int32_t>(N)) return std::nullopt;
    return catalog_codes(keys[level]);
}

constexpr Text kFreeGameMapSizeText[] = {
    Text::small, Text::medium, Text::large,
};
constexpr Text kFreeGameTownLevelText[] = {
    Text::few, Text::medium, Text::lots, Text::megapolis,
};
constexpr Text kFreeGameIndustryLevelText[] = {
    Text::few, Text::medium, Text::lots,
};

enum DialogScreenId : int32_t {
    CurrentScenarioScore = 6,
    ScenarioSetup = 4,
    Settings = 10,
    ScenarioChoice = 14,
    MilestoneProgress = 17,
    JunctionInfo = 20,
    StationInfo = 21,
    ServiceSiteInfo = 22,
    TrainCostDelta = 23,
    WagonPrices = 24,
    TrainInfo = 26,
    PlayerNameEntry = 27,
    HighScoreMenu = 28,
    LoanInfo = 30,
    AnnualReport = 32,
    YearGraph = 33,
    DecadeGraph = 34,
    DecadeReport = 35,
    OfferResponse41 = 41,
    OfferResponse42 = 42,
    OfferResponse44 = 44,
    OfferResponse45 = 45,
    OfferResponse46 = 46,
    OfferResponse47 = 47,
    OfferResponse48 = 48,
    OfferResponse49 = 49,
    OfferResponse50 = 50,
    OfferResponse51 = 51,
    OfferResponse52 = 52,
    OfferResponse53 = 53,
    OfferResponse54 = 54,
    OfferSelection = 60,
    OfferResponse61 = 61,
    OfferResponse62 = 62,
    OfferResponse63 = 63,
    OfferResponse64 = 64,
    OfferResponse65 = 65,
    OfferResponse66 = 66,
    OfferResponse67 = 67,
    OfferResponse68 = 68,
    OfferResponse69 = 69,
    OfferResponse70 = 70,
    OfferResponse71 = 71,
    OfferResponse72 = 72,
};

namespace dialog_field {
namespace scenario_setup {
enum Id : int32_t { MapSize = 1, FinancialDifficulty = 2,
                     AiDifficulty = 3, TownLevel = 4,
                     IndustryLevel = 5, DifficultyCycle = 6 };
}  // namespace scenario_setup

namespace junction_info {
enum Id : int32_t { TrainSelection = 0, ZonePopulation = 1, ConstructionTimer = 2 };
}  // namespace junction_info

namespace station_info {
enum Id : int32_t { Direction = 0, Orientation = 1, PreviousRevenue = 2 };
}  // namespace station_info

namespace settings {
enum Id : int32_t { Animations = 1, Selection = 2, MonthlyReport = 3,
                    AnnualReport = 4, Music = 5, Volume = 6 };
}  // namespace settings

namespace wagon_prices {
enum Id : int32_t { First = 0, Second = 1, Third = 2, Fourth = 3,
                    Fifth = 4, Sixth = 5 };
}  // namespace wagon_prices

namespace train_info {
enum Id : int32_t {
    Era = 0,
    FirstCar = 1,
    LastCar = 5,
    Revenue = 6,
    PreviousRevenue = 7,
    AgeYears = 8,
    CurrentCost = 9,
    CostDelta = 10,
    AiControl = 20,
};
}  // namespace train_info

namespace loan_info {
enum Id : int32_t { Balance = 0, Ceiling = 1, InterestRate = 2 };
}  // namespace loan_info

namespace offer_response {
enum Id : int32_t { DueDate = 1, Target = 2, Progress = 3, TargetValue = 4 };
}  // namespace offer_response

namespace offer_effect_response {
enum Id : int32_t { FirstValue = 1, Amount = 2, DisplayValue = 3 };
}  // namespace offer_effect_response

namespace player_name {
enum Id : int32_t { CursorMarker = 0, Name = 1 };
}  // namespace player_name

namespace high_score {
enum Id : int32_t {
    Best = 0,
    ScenarioFirst = 1,
    ScenarioLast = 13,
    LeaderboardFirst = 20,
    LeaderboardLast = 33,
};
}  // namespace high_score
}  // namespace dialog_field

namespace report_field {
enum Id : int32_t {
    Construction = 0,
    Trains = 1,
    RunningCosts = 2,
    Other = 3,
    Interest = 4,
    Taxes = 5,
    Income = 6,
    Balance = 7,
};
constexpr int32_t kCount = 8;
}  // namespace report_field

static_assert(report_field::Balance + 1 == report_field::kCount);

}  // namespace

static std::vector<int8_t> format_number_with_suffix(int32_t value,
                                                      int32_t suffix_code) {
    char formatted_text[24];
    snprintf(formatted_text, sizeof(formatted_text), "%d", value);
    const int text_length = (int)strlen(formatted_text);
    std::vector<int8_t> result(static_cast<size_t>(text_length + 1));
    for (int character_index = 0; character_index < text_length; ++character_index)
        result[character_index] = formatted_text[character_index] == '-'
            ? display_code::kMinusSign
            : (int8_t)(formatted_text[character_index]
                       - display_code::kAsciiDigitOffset);
    result[text_length] = (int8_t)suffix_code;
    return result;
}

static std::vector<int8_t> encode_display_text(const char *text) {
    const int text_length = (int)strlen(text);
    std::vector<int8_t> result(static_cast<size_t>(text_length));
    for (int character_index = 0; character_index < text_length; ++character_index) {
        const char character = text[character_index];
        result[character_index] = character == '-' ? display_code::kMinusSign
                    : character == '.' ? display_code::kPeriod
                    : character == '/' ? display_code::kDateSeparator
                    : character == '%' ? display_code::kPercent
                    : (int8_t)(character - display_code::kAsciiDigitOffset);
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
    const int digit_count = (int)strlen(digits);
    int separator_count = (digit_count - 1) / display_code::kThousandsGroupSize;
    int text_count = digit_count + separator_count + (value < 0 ? 1 : 0);
    std::vector<int8_t> result(static_cast<size_t>(text_count + 1));
    int output_index = 0;
    result[output_index++] = display_code::kCurrencyPrefix;
    if (value < 0) result[output_index++] = display_code::kMinusSign;
    for (int digit_index = 0; digit_index < digit_count; ++digit_index) {
        result[output_index++] = (int8_t)(digits[digit_index] - display_code::kAsciiDigitOffset);
        int remaining_digits = digit_count - digit_index - 1;
        if (remaining_digits > 0
            && remaining_digits % display_code::kThousandsGroupSize == 0)
            result[output_index++] = display_code::kPeriod;
    }
    return result;
}

struct ReportValues {
    int construction;
    int trains;
    int running_costs;
    int other;
    int interest;
    int taxes;
    int income;
    int balance;
};

static std::optional<std::vector<int8_t>> format_report_value(
    const ReportValues& values, int32_t field_id) {
    switch (field_id) {
        case report_field::Construction: return format_currency(values.construction);
        case report_field::Trains: return format_currency(values.trains);
        case report_field::RunningCosts: return format_currency(values.running_costs);
        case report_field::Other: return format_currency(values.other);
        case report_field::Interest: return format_currency(values.interest);
        case report_field::Taxes: return format_currency(values.taxes);
        case report_field::Income: return format_currency(values.income);
        case report_field::Balance: return format_currency(values.balance);
        default: return std::nullopt;
    }
}

std::optional<std::vector<int8_t>> Game::resolve_dialog_value(
    int32_t dialog_screen, int32_t field_id) {
    switch (dialog_screen) {
        case DialogScreenId::ScenarioChoice:
            if (this->ui.screenParam >= 0
                && this->ui.screenParam < dialog_value::kScenarioChoiceCount)
                return this->assets.index_text_codes(
                    dialog_value::kScenarioChoiceTextBase + this->ui.screenParam);
            break;

        case DialogScreenId::ScenarioSetup:
            switch (field_id) {
                case dialog_field::scenario_setup::MapSize:
                    return free_game_level_text(
                        this->scenario.freeGameMapSize, kFreeGameMapSizeText);
                case dialog_field::scenario_setup::FinancialDifficulty:
                    return this->assets.index_text_codes(
                        dialog_value::kFinancialDifficultyTextBase
                        + this->scenario.financialDifficulty);
                case dialog_field::scenario_setup::AiDifficulty:
                    return this->assets.index_text_codes(
                        dialog_value::kAiDifficultyTextBase
                        + this->scenario.aiDifficulty);
                case dialog_field::scenario_setup::TownLevel:
                    return free_game_level_text(
                        this->scenario.freeGameTownLevel,
                        kFreeGameTownLevelText);
                case dialog_field::scenario_setup::IndustryLevel:
                    return free_game_level_text(
                        this->scenario.freeGameIndustryLevel,
                        kFreeGameIndustryLevelText);
                case dialog_field::scenario_setup::DifficultyCycle:
                    return format_number_with_suffix(
                        this->scenario.difficultyCycle, display_code::kNoSuffix);
            }
            break;

        case DialogScreenId::JunctionInfo:
            switch (field_id) {
                case dialog_field::junction_info::TrainSelection: {
                    const int selection_code =
                        this->ui.trainSelectionIndex[this->build.junctionDirCursor];
                    const std::vector<int8_t> engine_name_codes =
                        this->assets.index_text_codes(
                            dialog_value::kJunctionEngineTextBase
                            + ((selection_code >> 8)
                               & dialog_value::kPackedSelectionByteMask));
                    const std::vector<int8_t> wagon_name_codes =
                        this->assets.index_text_codes(
                            dialog_value::kJunctionWagonTextBase
                            + (selection_code
                               & dialog_value::kPackedSelectionByteMask));
                    return join_codes(wagon_name_codes, engine_name_codes);
                }
                case dialog_field::junction_info::ZonePopulation:
                    return format_number_with_suffix(
                        this->world.zonePopulation[this->build.junctionDirCursor]
                            * dialog_value::kPopulationScale,
                        display_code::kNoSuffix);
                case dialog_field::junction_info::ConstructionTimer:
                    return format_number_with_suffix(
                        this->world.constructionTimers[this->build.junctionDirCursor],
                        display_code::kPercent);
            }
            break;

        case DialogScreenId::StationInfo:
            switch (field_id) {
                case dialog_field::station_info::Direction:
                    return this->assets.index_text_codes(
                        dialog_value::kStationDirectionTextBase
                        + this->world.cities[this->build.stationDirCursor]
                              .stationDirection);
                case dialog_field::station_info::Orientation:
                    return this->assets.index_text_codes(
                        dialog_value::kStationOrientationTextBase
                        + this->world.cities[this->build.stationDirCursor]
                              .stationDirection
                              / dialog_value::kStationOrientationDivisor);
                case dialog_field::station_info::PreviousRevenue:
                    return format_number_with_suffix(
                        this->world.cities[this->build.stationDirCursor]
                            .previousStationRevenue,
                        display_code::kNoSuffix);
            }
            break;

        case DialogScreenId::MilestoneProgress:
            return format_number_with_suffix(
                this->scenario.milestone_progress(this->economy),
                display_code::kPercent);

        case DialogScreenId::Settings:
            switch (field_id) {
                case dialog_field::settings::Animations:
                    return this->assets.index_text_codes(
                        this->settings.animationsDisabled
                            ? dialog_value::kDisabledTextCode
                            : dialog_value::kEnabledTextCode);
                case dialog_field::settings::Selection:
                    return this->assets.index_text_codes(
                        this->settings.selectionHidden
                            ? dialog_value::kDisabledTextCode
                            : dialog_value::kEnabledTextCode);
                case dialog_field::settings::MonthlyReport:
                    return this->assets.index_text_codes(
                        this->ui.monthlyReportEnabled
                            ? dialog_value::kEnabledTextCode
                            : dialog_value::kDisabledTextCode);
                case dialog_field::settings::AnnualReport:
                    return this->assets.index_text_codes(
                        this->ui.annualReportEnabled
                            ? dialog_value::kEnabledTextCode
                            : dialog_value::kDisabledTextCode);
                case dialog_field::settings::Music:
                    return this->assets.index_text_codes(
                        this->music.enabled ? dialog_value::kEnabledTextCode
                                            : dialog_value::kDisabledTextCode);
                case dialog_field::settings::Volume:
                    return format_number_with_suffix(
                        this->music.volumeLevel, display_code::kNoSuffix);
            }
            break;

        case DialogScreenId::WagonPrices:
            switch (field_id) {
                case dialog_field::wagon_prices::First: return format_currency(this->economy.trainCostFirstEra);
                case dialog_field::wagon_prices::Second: return format_currency(this->economy.trainCostSecondEra);
                case dialog_field::wagon_prices::Third: return format_currency(this->economy.trainCostThirdEra);
                case dialog_field::wagon_prices::Fourth: return format_currency(this->economy.trainCostFourthEra);
                case dialog_field::wagon_prices::Fifth: return format_currency(this->economy.trainCostFifthEra);
                case dialog_field::wagon_prices::Sixth: return format_currency(this->economy.trainCostSixthEra);
            }
            break;

        case DialogScreenId::TrainCostDelta:
            return format_currency(this->build.unitCost - this->build.currentTrainCostSnapshot);

        case DialogScreenId::ServiceSiteInfo:
            return format_number_with_suffix(
                this->fleet.serviceSites[this->serviceVehicleAtCursor]
                    .resourceStock[field_id],
                display_code::kNoSuffix);

        case DialogScreenId::TrainInfo: {
            if (field_id == dialog_field::train_info::Era)
                return this->assets.index_text_codes(
                    dialog_value::kTrainEraTextBase
                    + this->fleet.trainCars[this->trainAtCursor].era);
            if (field_id == dialog_field::train_info::Revenue) return format_currency(this->fleet.trainCars[this->trainAtCursor].revenue);
            if (field_id == dialog_field::train_info::PreviousRevenue) return format_currency(this->fleet.trainCars[this->trainAtCursor].previousRevenue);
            if (field_id == dialog_field::train_info::AgeYears)
                return format_number_with_suffix(
                    this->fleet.trainCars[this->trainAtCursor].ageYears,
                    display_code::kNoSuffix);
            if (field_id == dialog_field::train_info::CurrentCost)
                return format_currency(
                    this->build.currentTrainCostSnapshot
                    * dialog_value::kCurrentCostPercent
                    / percentage::kBase);
            if (field_id == dialog_field::train_info::CostDelta) return format_currency(this->build.baseTrainCostSnapshot - this->build.currentTrainCostSnapshot);
            if (field_id == dialog_field::train_info::AiControl)
                return this->assets.index_text_codes(
                    this->fleet.trainCars[this->trainAtCursor].aiOverride
                            == dialog_value::kNoAiOverride
                        ? dialog_value::kDisabledTextCode
                        : dialog_value::kEnabledTextCode);
            if (field_id < dialog_field::train_info::FirstCar
                || field_id > dialog_field::train_info::LastCar)
                break;
            int display_car_slot = dialog_value::kInitialCarSlot;
            if (this->fleet.count_cars(this->trainAtCursor) > field_id) {
                display_car_slot = this->trainAtCursor;
                for (int car_offset = 0; car_offset < field_id; ++car_offset)
                    display_car_slot =
                        this->fleet.trainCars[display_car_slot].nextCar;
            }
            const int car_type = train_flag::type(
                this->fleet.trainCars[display_car_slot].flags);
            char display_text[32];
            if (car_type == wagon::PassengerMailCar)
                snprintf(display_text, sizeof(display_text), "%d/%d",
                         this->fleet.trainCars[display_car_slot]
                             .energyCargoOrRemovalTicks,
                         this->fleet.trainCars[display_car_slot]
                             .secondaryCargoOrBaseEnergy);
            else
                snprintf(display_text, sizeof(display_text), "%d",
                         this->fleet.trainCars[display_car_slot]
                             .energyCargoOrRemovalTicks);
            return encode_display_text(display_text);
        }

        case DialogScreenId::LoanInfo:
            switch (field_id) {
                case dialog_field::loan_info::Balance: return format_currency(this->economy.loanBalance);
                case dialog_field::loan_info::Ceiling: return format_currency(this->economy.loanCeiling);
                case dialog_field::loan_info::InterestRate:
                    return format_number_with_suffix(
                        this->economy.interestRate, display_code::kPercent);
            }
            break;

        case DialogScreenId::AnnualReport: {
            const ReportValues values = {
                .construction = this->compareToPrevious
                    ? this->economy.prevTotalSpent : this->economy.totalSpent,
                .trains = this->compareToPrevious
                    ? this->economy.prevAiMoneySpent : this->economy.aiMoneySpent,
                .running_costs = this->compareToPrevious
                    ? this->economy.prevFuelCost : this->economy.fuelCostAccrued,
                .other = this->compareToPrevious
                    ? this->economy.prevSubsidyCost : this->economy.subsidyCost,
                .interest = this->compareToPrevious
                    ? this->economy.prevTerrainTotal : this->economy.terrainTotal,
                .taxes = this->compareToPrevious
                    ? this->economy.prevEnterpriseTotal
                    : this->economy.decadeEnterpriseTax,
                .income = this->compareToPrevious
                    ? this->economy.prevTotalRevenue : this->economy.totalRevenue,
                .balance = this->compareToPrevious
                    ? this->economy.previousYearProfit : this->economy.netProfit,
            };
            return format_report_value(values, field_id);
        }

        case DialogScreenId::DecadeReport: {
            const ReportValues values = {
                .construction = this->compareToPrevious
                    ? this->economy.prevDecadeSpent
                    : this->economy.decadeSpentWithCurrent,
                .trains = this->compareToPrevious
                    ? this->economy.prevDecadeAiSpent
                    : this->economy.decadeAiSpentWithCurrent,
                .running_costs = this->compareToPrevious
                    ? this->economy.prevDecadeFuel
                    : this->economy.decadeFuelWithCurrent,
                .other = this->compareToPrevious
                    ? this->economy.prevDecadeSubsidy
                    : this->economy.decadeSubsidyWithCurrent,
                .interest = this->compareToPrevious
                    ? this->economy.previousLegacyDecadeMetric
                    : this->economy.legacyDecadeMetric,
                .taxes = this->compareToPrevious
                    ? this->economy.previousDecadeEnterpriseTax
                    : this->economy.decadeEnterpriseWithCurrent,
                .income = this->compareToPrevious
                    ? this->economy.prevDecadeRevenue
                    : this->economy.decadeRevenueWithCurrent,
                .balance = this->compareToPrevious
                    ? this->economy.previousDecadeProfit
                    : this->economy.decadeProfitWithCurrent,
            };
            return format_report_value(values, field_id);
        }

        case DialogScreenId::YearGraph:
            if (field_id == dialog_value::kGraphModeField)
                return this->assets.index_text_codes(
                    dialog_value::kYearGraphModeTextBase
                    + this->ui.graphSeries);
            if (field_id >= dialog_value::kGraphHistoryFieldBase) {
                const int history_index =
                    field_id - dialog_value::kGraphHistoryFieldBase;
                if (!this->ui.graphUsesDecadeData) {
                    if (this->ui.graphSeries == report_series::Profit)
                        return format_currency(
                            this->economy.yearProfitHistory[history_index]);
                    if (this->ui.graphSeries == report_series::Revenue)
                        return format_currency(
                            this->economy.yearRevenueHistory[history_index]);
                    if (this->ui.graphSeries == report_series::CompanyValue)
                        return format_currency(
                            this->economy.yearCompanyValueHistory[history_index]);
                } else {
                    if (this->ui.graphSeries == report_series::Profit)
                        return format_currency(
                            this->economy.decadeProfitHistory[history_index]);
                    if (this->ui.graphSeries == report_series::Revenue)
                        return format_currency(
                            this->economy.altDecadeRevenueTotal[history_index]);
                    if (this->ui.graphSeries == report_series::CompanyValue)
                        return format_currency(
                            this->economy.altDecadeTotalCost[history_index]);
                }
            } else {
                if (field_id >= dialog_value::kGraphYearFieldBase) {
                    if (!this->ui.graphUsesDecadeData)
                        return format_number_with_suffix(
                            this->ui.graphPeriodLabels[
                                field_id - dialog_value::kGraphYearFieldBase],
                            display_code::kNoSuffix);
                    return std::nullopt;
                }
                return !this->ui.graphUsesDecadeData
                    ? format_number_with_suffix(
                        this->ui.graphYearLabels[field_id]
                            + dialog_value::kLegacyYearBase,
                        display_code::kDateSeparator)
                    : format_number_with_suffix(
                        this->ui.graphPeriodLabels[field_id]
                            + dialog_value::kLegacyYearBase,
                        display_code::kNoSuffix);
            }
            // The original falls through when the graph mode is invalid.
        case DialogScreenId::DecadeGraph:
            if (field_id == dialog_value::kGraphModeField)
                return this->assets.index_text_codes(
                    dialog_value::kDecadeGraphModeTextBase
                    + this->ui.graphSeries);
            if (field_id >= dialog_value::kGraphYearFieldBase) {
                if (!this->ui.graphUsesDecadeData)
                    return format_number_with_suffix(
                        this->ui.graphPeriodLabels[
                            field_id - dialog_value::kGraphYearFieldBase],
                        display_code::kNoSuffix);
                return std::nullopt;
            }
            return !this->ui.graphUsesDecadeData
                ? format_number_with_suffix(
                    this->ui.graphYearLabels[field_id]
                        + dialog_value::kDecadeYearOffset,
                    display_code::kDateSeparator)
                : format_number_with_suffix(
                    this->ui.graphPeriodLabels[field_id]
                        + dialog_value::kLegacyYearBase,
                    display_code::kNoSuffix);

        case DialogScreenId::OfferResponse41: case DialogScreenId::OfferResponse42:
        case DialogScreenId::OfferResponse44: case DialogScreenId::OfferResponse45:
        case DialogScreenId::OfferResponse46: case DialogScreenId::OfferResponse47:
        case DialogScreenId::OfferResponse48: case DialogScreenId::OfferResponse49:
        case DialogScreenId::OfferResponse50: case DialogScreenId::OfferResponse51:
        case DialogScreenId::OfferResponse52: case DialogScreenId::OfferResponse53:
        case DialogScreenId::OfferResponse54:
            if (field_id == dialog_field::offer_response::DueDate) {
                const auto day_codes = format_number_with_suffix(
                    this->offerBoard.offers[this->scenario.offerRowIndex].dueDay,
                    display_code::kPeriod);
                const auto month_codes = format_number_with_suffix(
                    this->offerBoard.offers[this->scenario.offerRowIndex].dueMonth,
                    display_code::kPeriod);
                const auto year_codes = format_number_with_suffix(
                    this->offerBoard.offers[this->scenario.offerRowIndex].dueYear
                        + game_calendar::kBaseYear,
                    display_code::kNoSuffix);
                return join_three_codes(day_codes, month_codes, year_codes);
            }
            if (field_id == dialog_field::offer_response::Target) return format_currency(this->offerBoard.offers[this->scenario.offerRowIndex].target);
            if (field_id == dialog_field::offer_response::Progress) return format_number_with_suffix(
                this->offerBoard.display_progress(this->scenario.offerRowIndex,
                                                  this->economy),
                display_code::kPercent);
            if (field_id == dialog_field::offer_response::TargetValue)
                return format_number_with_suffix(
                    this->offerBoard.offers[this->scenario.offerRowIndex].target,
                    display_code::kNoSuffix);
            break;

        case DialogScreenId::OfferSelection:
            if (field_id % dialog_value::kOfferFieldPairWidth
                == dialog_value::kFirstOfferProgressField) {
                const int offer_index =
                    field_id / dialog_value::kOfferFieldPairWidth;
                return this->assets.index_text_codes(
                    this->offerBoard.offers[
                        this->ui.enabledMenuRowIndexes[offer_index]].objectiveType
                    + dialog_value::kOfferObjectiveTextBase);
            } else {
                const int offer_index =
                    (field_id - dialog_value::kFirstOfferProgressField)
                    / dialog_value::kOfferFieldPairWidth;
                return format_number_with_suffix(
                    this->offerBoard.display_progress(
                        this->ui.enabledMenuRowIndexes[offer_index], this->economy),
                    display_code::kPercent);
            }

        case DialogScreenId::OfferResponse61: case DialogScreenId::OfferResponse62:
        case DialogScreenId::OfferResponse63: case DialogScreenId::OfferResponse64:
        case DialogScreenId::OfferResponse65: case DialogScreenId::OfferResponse66:
        case DialogScreenId::OfferResponse67: case DialogScreenId::OfferResponse68:
        case DialogScreenId::OfferResponse69: case DialogScreenId::OfferResponse70:
        case DialogScreenId::OfferResponse71: case DialogScreenId::OfferResponse72:
            if (field_id == dialog_field::offer_effect_response::FirstValue)
                return format_number_with_suffix(
                    this->offerBoard.offers[this->scenario.offerRowIndex]
                        .effectValue,
                    display_code::kPercent);
            if (field_id == dialog_field::offer_effect_response::Amount) return format_currency(this->offerBoard.offers[this->scenario.offerRowIndex].effectAmount);
            if (field_id == dialog_field::offer_effect_response::DisplayValue)
                return format_number_with_suffix(
                    this->offerBoard.offers[this->scenario.offerRowIndex]
                        .effectValue,
                    display_code::kNoSuffix);
            break;

        case DialogScreenId::PlayerNameEntry:
            if (field_id == dialog_field::player_name::Name)
                return copy_codes(this->scenario.playerNameCodes, kPlayerNameCodeCount);
            if (field_id == dialog_field::player_name::CursorMarker) {
                memset(this->scenario.playerNameCursorMarker, 0,
                       sizeof(this->scenario.playerNameCursorMarker));
                this->scenario.playerNameCursorMarker[this->scenario.playerNameCursor]
                    = dialog_value::kPlayerNameCursorCode;
                return copy_codes(this->scenario.playerNameCursorMarker, kPlayerNameCodeCount);
            }
            break;

        case DialogScreenId::HighScoreMenu:
            if (field_id == dialog_field::high_score::Best)
                return format_number_with_suffix(
                    this->scenario.bestScenarioScore, display_code::kNoSuffix);
            if (field_id >= dialog_field::high_score::ScenarioFirst
                && field_id <= dialog_field::high_score::ScenarioLast)
                return format_number_with_suffix(
                    this->scenario.scenarioScores[
                        field_id - dialog_field::high_score::ScenarioFirst],
                    display_code::kNoSuffix);
            if (field_id >= dialog_field::high_score::LeaderboardFirst
                && field_id <= dialog_field::high_score::LeaderboardLast)
                return format_number_with_suffix(
                    this->scenario.leaderboardScores[
                        field_id - dialog_field::high_score::LeaderboardFirst],
                    display_code::kNoSuffix);
            break;

        case DialogScreenId::CurrentScenarioScore:
            return format_number_with_suffix(
                this->scenario.currentScenarioScore, display_code::kNoSuffix);
    }
    return std::nullopt;
}
