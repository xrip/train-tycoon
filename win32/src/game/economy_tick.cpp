// Monthly economy and offer processing.
#include "game.h"
#include "support/numeric.h"
#include <cstdlib>

namespace {

namespace offer_timing {
constexpr int32_t kDaysPerDurationUnit = 60;
constexpr int32_t kBaseDelayDays = 20;
constexpr int32_t kRandomDelayDays = 20;
constexpr int32_t kDaysPerMonth = 28;
constexpr int32_t kMonthsPerYear = 12;
constexpr int32_t kFirstDayOfMonth = 1;
constexpr int32_t kMonthDateStride = 32;
constexpr int32_t kYearDateStride = 370;
constexpr int32_t kMinimumOfferInterval = 150;
constexpr int32_t kIntervalRandomRange = 20;
}  // namespace offer_timing

namespace offer_finance {
constexpr int32_t kLoanRoundingUnit = 20000;
constexpr int32_t kLoanRoundingHalf = 10000;
}  // namespace offer_finance

namespace offer_generation {
constexpr int32_t kDurationBase = 4;
constexpr int32_t kDurationRandomRange = 12;
constexpr int32_t kObjectiveRandomRange = 16;
constexpr int32_t kEngineCountOffset = 2;
constexpr int32_t kCargoCountMultiplier = 2;
constexpr int32_t kCargoCountOffset = 5;
constexpr int32_t kPassengerTargetUnit = 25;
constexpr int32_t kIndustryTargetRandomRange = 3;
constexpr int32_t kPassengerTargetRandomRange = 3;
constexpr int32_t kMailTargetUnit = 18;
constexpr int32_t kPassengerTargetUnitSmall = 10;
constexpr int32_t kSmallObjectiveBase = 3;
constexpr int32_t kSmallObjectiveRandomRange = 8;
constexpr int32_t kRevenueTargetBase = 13324;
constexpr int32_t kRevenueTargetRandomRange = 5000;
constexpr int32_t kLoanTargetMinimum = 40000;
constexpr int32_t kLoanTargetPercentBase = 75;
constexpr int32_t kLoanTargetPercentRange = 15;
constexpr int32_t kCashEffectBase = 30;
constexpr int32_t kCashEffectRandomRange = 50;
constexpr int32_t kFixedCashUnit = 5000;
constexpr int32_t kFixedCashBase = 6;
constexpr int32_t kFixedCashRandomRange = 17;
constexpr int32_t kLoanCeilingUnit = 20000;
constexpr int32_t kLoanCeilingBase = 3;
constexpr int32_t kLoanCeilingRandomRange = 3;
constexpr int32_t kResourceEffectBase = 25;
constexpr int32_t kResourceEffectRandomRange = 45;
constexpr int32_t kLoanReductionBase = 15;
constexpr int32_t kLoanReductionRandomRange = 30;
constexpr int32_t kMaximumLoanCeiling = 1000000;
}  // namespace offer_generation

}  // namespace

int32_t Economy::train_cost_for_era(int32_t era) const {
    switch (era) {
        case train_era::First: return trainCostFirstEra;
        case train_era::Second: return trainCostSecondEra;
        case train_era::Third: return trainCostThirdEra;
        case train_era::Fourth: return trainCostFourthEra;
        case train_era::Fifth: return trainCostFifthEra;
        case train_era::Sixth: return trainCostSixthEra;
        default: return 0;
    }
}

void OfferBoard::schedule_due_date(int32_t row, int32_t duration_units,
                                   JavaRandom& random) {
    int delay_days = duration_units * offer_timing::kDaysPerDurationUnit
                     + offer_timing::kBaseDelayDays
                     + random.next_int() % offer_timing::kRandomDelayDays;
    int due_day = offers[row].issuedDay + delay_days;
    int due_month = offers[row].issuedMonth;
    int due_year = offers[row].issuedYear;
    while (due_day > offer_timing::kDaysPerMonth) {
        due_day -= offer_timing::kDaysPerMonth;
        ++due_month;
    }
    while (due_month > offer_timing::kMonthsPerYear) {
        due_month -= offer_timing::kMonthsPerYear;
        ++due_year;
    }
    if (due_day == offer_timing::kFirstDayOfMonth) ++due_day;
    offers[row].dueDay = (int8_t)due_day;
    offers[row].dueMonth = (int8_t)due_month;
    offers[row].dueYear = (int8_t)due_year;
}

void Game::move_offer_to_summary(int32_t row) {
    Game* game = this;
    game->scenario.offerTypeTransitionCode =
        game->offerBoard.offers[row].objectiveType + transition::OFFER_TYPE_BASE;
    game->scenario.offerEffectTransitionCode =
        game->offerBoard.offers[row].effectType + transition::OFFER_ALT_BASE;
    game->scenario.offerRowIndex = kOfferSummaryRow;
    game->offerBoard.move_to_summary(row);
    game->ready = false;
}

void OfferBoard::move_to_summary(int32_t row) {
    offers[kOfferSummaryRow].objectiveType = offers[row].objectiveType;
    offers[kOfferSummaryRow].target = offers[row].target;
    offers[kOfferSummaryRow].progress = offers[row].progress;
    offers[kOfferSummaryRow].effectType = offers[row].effectType;
    offers[kOfferSummaryRow].effectValue = offers[row].effectValue;
    offers[kOfferSummaryRow].effectAmount = offers[row].effectAmount;
    offers[kOfferSummaryRow].issuedDay = offers[row].issuedDay;
    offers[kOfferSummaryRow].issuedMonth = offers[row].issuedMonth;
    offers[kOfferSummaryRow].issuedYear = offers[row].issuedYear;
    offers[kOfferSummaryRow].dueDay = offers[row].dueDay;
    offers[kOfferSummaryRow].dueMonth = offers[row].dueMonth;
    offers[kOfferSummaryRow].dueYear = offers[row].dueYear;
    offers[row].objectiveType = offer_objective::None;
    offers[row].target = 0;
    offers[row].progress = 0;
    offers[row].effectType = offer_effect::None;
    offers[row].effectValue = 0;
    offers[row].effectAmount = 0;
}

void Game::apply_offer_effect(int32_t row) {
    Game* game = this;
    if (game->offerBoard.offers[row].effectType == offer_effect::CashPercentOfTarget
        || game->offerBoard.offers[row].effectType == offer_effect::FixedCash) {
        game->economy.money += game->offerBoard.offers[row].effectAmount;
        game->record_money_change(game->offerBoard.offers[row].effectAmount);
    }
    if (game->offerBoard.offers[row].effectType == offer_effect::LoanCeilingIncrease)
        game->economy.loanCeiling += game->offerBoard.offers[row].effectAmount;
    if (game->offerBoard.offers[row].effectType == offer_effect::EraAdvance
        && game->scenario.currentEra < train_era::Sixth)
        ++game->scenario.currentEra;
    if (game->offerBoard.offers[row].effectType >= offer_effect::kFirstResourceFlag
        && game->offerBoard.offers[row].effectType <= offer_effect::kLastResourceFlag) {
        int resource = game->offerBoard.offers[row].effectType - offer_effect::kFirstResourceFlag;
        game->scenario.routeResourceFlags[resource] =
            (int8_t)(game->scenario.routeResourceFlags[resource] + game->offerBoard.offers[row].effectValue);
    }
    if (game->offerBoard.offers[row].effectType == offer_effect::LoanBalanceReduction) {
        game->economy.loanBalance -=
            game->economy.loanBalance * game->offerBoard.offers[row].effectValue
            / percentage::kBase;
        if (game->economy.loanBalance < 0) game->economy.loanBalance = 0;
        if (game->economy.loanBalance % offer_finance::kLoanRoundingUnit != 0) {
            if (game->economy.loanBalance % offer_finance::kLoanRoundingUnit
                < offer_finance::kLoanRoundingHalf)
                game->economy.loanBalance -=
                    game->economy.loanBalance % offer_finance::kLoanRoundingUnit;
            else
                game->economy.loanBalance +=
                    offer_finance::kLoanRoundingUnit
                    - game->economy.loanBalance % offer_finance::kLoanRoundingUnit;
        }
    }
    if (game->offerBoard.offers[row].effectType == offer_effect::InterestRateReduction
        && game->economy.interestRate > 1)
        --game->economy.interestRate;
    move_offer_to_summary(row);
    game->scenario.offerFlowState = offer_flow::EffectChoice;
    game->ui.transitionCode = offer_screen::EffectChoice;
    game->set_state(GameState::MenuOverlay);
}

void Game::show_due_offer(int32_t row) {
    Game* game = this;
    move_offer_to_summary(row);
    game->scenario.offerFlowState = offer_flow::ObjectiveChoice;
    game->ui.transitionCode = offer_screen::ObjectiveChoice;
    game->set_state(GameState::MenuOverlay);
}

void Game::monthly_offer_tick() {
    Game* game = this;
    if (!game->scenario.monthlyOffersEnabled) return;
    const int resource_flag_count =
        offer_effect::kLastResourceFlag - offer_effect::kFirstResourceFlag + 1;
    for (int resource = 0; resource < resource_flag_count; ++resource) {
        if (game->scenario.routeResourceFlags[resource] > 0) --game->scenario.routeResourceFlags[resource];
    }
    for (int row = 0; row < kOfferActiveRowCount; ++row) {
        if ((game->offerBoard.offers[row].objectiveType == offer_objective::Objective1
             || (game->offerBoard.offers[row].objectiveType >= offer_objective::kFirstProgress
                 && game->offerBoard.offers[row].objectiveType <= offer_objective::kLastProgress))
            && game->offerBoard.offers[row].target <= game->offerBoard.offers[row].progress)
            apply_offer_effect(row);
        if (game->offerBoard.offers[row].dueDay != game->economy.dayOfMonth
            || game->offerBoard.offers[row].dueMonth != game->economy.monthOfYear
            || game->offerBoard.offers[row].dueYear
               != game->economy.year - game_calendar::kBaseYear)
            continue;
        if (game->offerBoard.offers[row].objectiveType == offer_objective::Objective2
            && game->offerBoard.offers[row].target <= game->economy.money)
            apply_offer_effect(row);
        if (game->offerBoard.offers[row].objectiveType != offer_objective::None)
            show_due_offer(row);
    }

    int date = game->economy.dayOfMonth
               + offer_timing::kMonthDateStride * game->economy.monthOfYear
               + offer_timing::kYearDateStride
                 * (game->economy.year - game_calendar::kBaseYear);
    if (date - game->scenario.lastOfferDateCode
        <= offer_timing::kMinimumOfferInterval
           + game->rng.next_int() % offer_timing::kIntervalRandomRange)
        return;
    int free_row = -1;
    for (int row = 0; row < kOfferActiveRowCount; ++row) {
        if (game->offerBoard.offers[row].objectiveType == offer_objective::None) {
            free_row = row;
            break;
        }
    }
    if (free_row > -1) {
        game->offerBoard.offers[free_row].issuedDay = (int8_t)game->economy.dayOfMonth;
        game->offerBoard.offers[free_row].issuedMonth = (int8_t)game->economy.monthOfYear;
        game->offerBoard.offers[free_row].issuedYear =
            (int8_t)(game->economy.year - game_calendar::kBaseYear);
        const int offer_scale = offer_generation::kDurationBase
                              + std::abs(game->rng.next_int()
                                         % offer_generation::kDurationRandomRange);
        game->offerBoard.schedule_due_date(free_row, offer_scale, game->rng);
        const int offer_type = std::abs(game->rng.next_int()
                                        % offer_generation::kObjectiveRandomRange);
        int engine_count = 0;
        int industry_group2_car_a_count = 0;
        // No current wagon branch increments this legacy objective counter;
        // preserve its zero value until the old objective data is decoded.
        int industry_group2_car_count = 0;
        int industry_group1_car_count = 0;
        int passenger_or_industry_group3_car_count = 0;
        for (int train_slot = 0; train_slot < kTrainSlotCount; ++train_slot) {
            if (!train_flag::isUsed(game->fleet.trainCars[train_slot].flags)
                || game->world.trackMap[unsigned_byte(game->fleet.trainCars[train_slot].x)]
                               [unsigned_byte(game->fleet.trainCars[train_slot].y)]
                               != tile::SERVICE_TRACK)
                continue;
            if (train_flag::isHead(game->fleet.trainCars[train_slot].flags)) {
                ++engine_count;
                continue;
            }
            switch (train_flag::type(game->fleet.trainCars[train_slot].flags)) {
                case wagon::PassengerMailCar:
                    ++passenger_or_industry_group3_car_count;
                    break;
                case wagon::IndustryGroup1Car:
                    ++industry_group1_car_count;
                    break;
                case wagon::IndustryGroup2CarA: case wagon::IndustryGroup2CarB:
                    ++industry_group2_car_count;
                    break;
                case wagon::IndustryGroup3CarA: case wagon::IndustryGroup3CarB:
                    ++passenger_or_industry_group3_car_count;
                    break;
            }
        }
        game->offerBoard.offers[free_row].objectiveType =
            offer_type > offer_objective::None
                && offer_type < offer_objective::Objective15
                ? (int8_t)offer_type : offer_objective::None;
        if (offer_type == offer_objective::Objective1)
            game->offerBoard.offers[free_row].target =
                offer_scale * (engine_count + offer_generation::kEngineCountOffset)
                * (3000 + game->rng.next_int() % 1500);
        if (offer_type == offer_objective::Objective2)
            game->offerBoard.offers[free_row].target =
                game->economy.money
                + offer_scale * (engine_count + offer_generation::kEngineCountOffset)
                  * (2000 + game->rng.next_int() % 1000);
        if (offer_type == offer_objective::Objective3)
            game->offerBoard.offers[free_row].objectiveType = offer_objective::None;
        if (offer_type == offer_objective::Objective4)
            game->offerBoard.offers[free_row].target =
                offer_scale * (industry_group2_car_a_count
                               * offer_generation::kCargoCountMultiplier
                               + offer_generation::kCargoCountOffset)
                * (offer_generation::kPassengerTargetUnit
                   + game->rng.next_int() % offer_generation::kIndustryTargetRandomRange);
        if (offer_type == offer_objective::Objective5)
            game->offerBoard.offers[free_row].target =
                offer_scale * (industry_group2_car_count
                               * offer_generation::kCargoCountMultiplier
                               + offer_generation::kCargoCountOffset)
                * (offer_generation::kPassengerTargetUnit
                   + game->rng.next_int() % offer_generation::kIndustryTargetRandomRange);
        if (offer_type == offer_objective::Objective6)
            game->offerBoard.offers[free_row].target =
                offer_scale * (industry_group1_car_count
                               * offer_generation::kCargoCountMultiplier
                               + offer_generation::kCargoCountOffset)
                * (offer_generation::kPassengerTargetUnit
                   + game->rng.next_int() % offer_generation::kIndustryTargetRandomRange);
        if (offer_type == offer_objective::Objective7)
            game->offerBoard.offers[free_row].target =
                offer_scale * (passenger_or_industry_group3_car_count
                               * offer_generation::kCargoCountMultiplier
                               + offer_generation::kCargoCountOffset)
                * (offer_generation::kMailTargetUnit
                   + game->rng.next_int() % offer_generation::kPassengerTargetRandomRange);
        if (offer_type == offer_objective::Objective8)
            game->offerBoard.offers[free_row].target =
                offer_scale * (passenger_or_industry_group3_car_count
                               * offer_generation::kCargoCountMultiplier
                               + offer_generation::kCargoCountOffset)
                * (offer_generation::kPassengerTargetUnitSmall
                   + game->rng.next_int() % offer_generation::kPassengerTargetRandomRange);
        if (offer_type >= offer_objective::kFirstCargoCount
            && offer_type <= offer_objective::kLastCargoCount)
            game->offerBoard.offers[free_row].target =
                offer_generation::kSmallObjectiveBase
                + std::abs(game->rng.next_int()
                           % offer_generation::kSmallObjectiveRandomRange);
        if (offer_type == offer_objective::Objective13)
            game->offerBoard.offers[free_row].target =
                (offer_generation::kRevenueTargetBase
                 + std::abs(game->rng.next_int()
                            % offer_generation::kRevenueTargetRandomRange))
                * offer_scale;
        if (offer_type == offer_objective::Objective14) {
            if (game->economy.loanBalance > offer_generation::kLoanTargetMinimum)
                game->offerBoard.offers[free_row].target = game->economy.loanBalance
                    * (offer_generation::kLoanTargetPercentBase
                       + std::abs(game->rng.next_int()
                                  % offer_generation::kLoanTargetPercentRange))
                    / percentage::kBase;
            else
                game->offerBoard.offers[free_row].objectiveType = offer_objective::None;
        }
        if (offer_type != offer_objective::None) {
            int effect_code = offer_effect::None;
            while (effect_code == offer_effect::None) {
                effect_code = offer_effect::CashPercentOfTarget
                              + std::abs(game->rng.next_int() % kOfferEffectCount);
                const std::vector<int8_t>& allowed_types =
                    game->scenario.offerTypesByEffect[
                        effect_code - offer_effect::CashPercentOfTarget];
                bool valid = false;
                for (int allowed_type_index = 0;
                     allowed_type_index < static_cast<int>(allowed_types.size());
                     ++allowed_type_index) {
                    if (allowed_types[allowed_type_index] == offer_type)
                        valid = true;
                }
                if (effect_code == offer_effect::InterestRateReduction
                    && game->economy.interestRate <= 1)
                    valid = false;
                if (effect_code == offer_effect::LoanCeilingIncrease
                    && game->economy.loanCeiling >= offer_generation::kMaximumLoanCeiling)
                    valid = false;
                if (effect_code == offer_effect::EraAdvance
                    && game->scenario.currentEra >= train_era::Sixth)
                    valid = false;
                if (game->scenario.offerGenerationCount
                    < game->scenario.offerEffectSchedule[
                          effect_code - offer_effect::CashPercentOfTarget]
                          [offer_schedule::FirstDelay])
                    valid = false;
                if (game->scenario.offerGenerationCount
                    < game->scenario.offerEffectNextCount[
                          effect_code - offer_effect::CashPercentOfTarget])
                    valid = false;
                if (!valid) effect_code = offer_effect::None;
            }
            int effect_index = effect_code - offer_effect::CashPercentOfTarget;
            game->scenario.offerEffectNextCount[effect_index] =
                (int16_t)(game->scenario.offerEffectNextCount[effect_index]
                          + unsigned_byte(game->scenario.offerEffectSchedule[effect_index][offer_schedule::RepeatDelay]));
            if (effect_code == offer_effect::CashPercentOfTarget) {
                game->offerBoard.offers[free_row].effectValue =
                    (int8_t)(offer_generation::kCashEffectBase
                             + std::abs(game->rng.next_int()
                                        % offer_generation::kCashEffectRandomRange));
                    game->offerBoard.offers[free_row].effectAmount =
                    game->offerBoard.offers[free_row].target
                    * game->offerBoard.offers[free_row].effectValue
                    / percentage::kBase;
            }
            if (effect_code == offer_effect::FixedCash)
                game->offerBoard.offers[free_row].effectAmount =
                    offer_generation::kFixedCashUnit
                    * (offer_generation::kFixedCashBase
                       + std::abs(game->rng.next_int()
                                  % offer_generation::kFixedCashRandomRange));
            if (effect_code == offer_effect::LoanCeilingIncrease)
                game->offerBoard.offers[free_row].effectAmount =
                    offer_generation::kLoanCeilingUnit
                    * (offer_generation::kLoanCeilingBase
                       + std::abs(game->rng.next_int()
                                  % offer_generation::kLoanCeilingRandomRange));
            if (effect_code >= offer_effect::kFirstResourceFlag
                && effect_code <= offer_effect::kLastResourceFlag)
                game->offerBoard.offers[free_row].effectValue =
                    (int8_t)(offer_generation::kResourceEffectBase
                             + std::abs(game->rng.next_int()
                                        % offer_generation::kResourceEffectRandomRange));
            if (effect_code == offer_effect::LoanBalanceReduction)
                game->offerBoard.offers[free_row].effectValue =
                    (int8_t)(offer_generation::kLoanReductionBase
                             + std::abs(game->rng.next_int()
                                        % offer_generation::kLoanReductionRandomRange));
            game->offerBoard.offers[free_row].effectType = (int8_t)effect_code;
        }
        if (game->offerBoard.offers[free_row].objectiveType != offer_objective::None) {
            game->ready = false;
            game->ui.transitionCode = offer_screen::Selection;
            game->scenario.offerRowIndex = (int8_t)free_row;
            game->set_state(GameState::MenuOverlay);
            ++game->scenario.offerGenerationCount;
        }
    }
    game->scenario.lastOfferDateCode = date;
}
