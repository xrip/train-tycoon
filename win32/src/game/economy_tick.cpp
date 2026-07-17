// Monthly economy and offer processing.
#include "game_internal.h"
#include <cstdlib>

void OfferBoard::schedule_due_date(int32_t row, int32_t duration_units,
                                   JavaRandom& random) {
    int delay_days = duration_units * 60 + 20 + random.next_int() % 20;
    int due_day = offers[row].issuedDay + delay_days;
    int due_month = offers[row].issuedMonth;
    int due_year = offers[row].issuedYear;
    while (due_day > 28) {
        due_day -= 28;
        ++due_month;
    }
    while (due_month > 12) {
        due_month -= 12;
        ++due_year;
    }
    if (due_day == 1) ++due_day;
    offers[row].dueDay = (int8_t)due_day;
    offers[row].dueMonth = (int8_t)due_month;
    offers[row].dueYear = (int8_t)due_year;
}

static void copy_menu_row_to_slot5(Game *game, int row) {
    game->scenario.offerTypeTransitionCode = game->offerBoard.offers[row].objectiveType + 40;
    game->scenario.offerEffectTransitionCode = game->offerBoard.offers[row].effectType + 60;
    game->scenario.offerRowIndex = 5;
    game->offerBoard.copy_to_summary_and_clear(row);
    game->ready = false;
}

void OfferBoard::copy_to_summary_and_clear(int32_t row) {
    offers[5].objectiveType = offers[row].objectiveType;
    offers[5].target = offers[row].target;
    offers[5].progress = offers[row].progress;
    offers[5].effectType = offers[row].effectType;
    offers[5].effectValue = offers[row].effectValue;
    offers[5].effectAmount = offers[row].effectAmount;
    offers[5].issuedDay = offers[row].issuedDay;
    offers[5].issuedMonth = offers[row].issuedMonth;
    offers[5].issuedYear = offers[row].issuedYear;
    offers[5].dueDay = offers[row].dueDay;
    offers[5].dueMonth = offers[row].dueMonth;
    offers[5].dueYear = offers[row].dueYear;
    offers[row].objectiveType = 0;
    offers[row].target = 0;
    offers[row].progress = 0;
    offers[row].effectType = 0;
    offers[row].effectValue = 0;
    offers[row].effectAmount = 0;
}

void game_applyMenuRowEffect(Game *game, int32_t row) {
    if (game->offerBoard.offers[row].effectType == 1 || game->offerBoard.offers[row].effectType == 2) {
        game->economy.money += game->offerBoard.offers[row].effectAmount;
        game_pushMoneyHistory(game, game->offerBoard.offers[row].effectAmount);
    }
    if (game->offerBoard.offers[row].effectType == 3) game->economy.loanCeiling += game->offerBoard.offers[row].effectAmount;
    if (game->offerBoard.offers[row].effectType == 4 && game->era < 5) ++game->era;
    if (game->offerBoard.offers[row].effectType >= 5 && game->offerBoard.offers[row].effectType <= 10) {
        int resource = game->offerBoard.offers[row].effectType - 5;
        game->scenario.routeResourceFlags[resource] =
            (int8_t)(game->scenario.routeResourceFlags[resource] + game->offerBoard.offers[row].effectValue);
    }
    if (game->offerBoard.offers[row].effectType == 11) {
        game->economy.loanBalance -= game->economy.loanBalance * game->offerBoard.offers[row].effectValue / 100;
        if (game->economy.loanBalance < 0) game->economy.loanBalance = 0;
        if (game->economy.loanBalance % 20000 != 0) {
            if (game->economy.loanBalance % 20000 < 10000)
                game->economy.loanBalance -= game->economy.loanBalance % 20000;
            else
                game->economy.loanBalance += 20000 - game->economy.loanBalance % 20000;
        }
    }
    if (game->offerBoard.offers[row].effectType == 12 && game->economy.interestRate > 1)
        --game->economy.interestRate;
    copy_menu_row_to_slot5(game, row);
    game->scenario.duplicateRowState = 3;
    game->ui.transitionCode = 9;
    game_setState(game, GameState::MenuOverlay);
}

void game_duplicateMenuRow(Game *game, int32_t row) {
    copy_menu_row_to_slot5(game, row);
    game->scenario.duplicateRowState = 4;
    game->ui.transitionCode = 8;
    game_setState(game, GameState::MenuOverlay);
}

void game_everyMonthlyTick(Game *game) {
    if (!game->scenario.monthlyOffersEnabled) return;
    for (int resource = 0; resource < 6; ++resource) {
        if (game->scenario.routeResourceFlags[resource] > 0) --game->scenario.routeResourceFlags[resource];
    }
    for (int row = 0; row < 5; ++row) {
        if ((game->offerBoard.offers[row].objectiveType == 1
             || (game->offerBoard.offers[row].objectiveType >= 4 && game->offerBoard.offers[row].objectiveType <= 14))
            && game->offerBoard.offers[row].target <= game->offerBoard.offers[row].progress)
            game_applyMenuRowEffect(game, row);
        if (game->offerBoard.offers[row].dueDay != game->economy.dayOfMonth
            || game->offerBoard.offers[row].dueMonth != game->economy.monthOfYear
            || game->offerBoard.offers[row].dueYear != game->economy.year - 1950)
            continue;
        if (game->offerBoard.offers[row].objectiveType == 2 && game->offerBoard.offers[row].target <= game->economy.money)
            game_applyMenuRowEffect(game, row);
        if (game->offerBoard.offers[row].objectiveType != 0) game_duplicateMenuRow(game, row);
    }

    int date = game->economy.dayOfMonth + 32 * game->economy.monthOfYear + 370 * (game->economy.year - 1950);
    if (date - game->scenario.lastOfferDateCode <= 150 + game->rng.next_int() % 20)
        return;
    int free_row = -1;
    for (int row = 0; row < 5; ++row) {
        if (game->offerBoard.offers[row].objectiveType == 0) {
            free_row = row;
            break;
        }
    }
    if (free_row > -1) {
        game->offerBoard.offers[free_row].issuedDay = (int8_t)game->economy.dayOfMonth;
        game->offerBoard.offers[free_row].issuedMonth = (int8_t)game->economy.monthOfYear;
        game->offerBoard.offers[free_row].issuedYear = (int8_t)(game->economy.year - 1950);
        int offer_scale = 4 + std::abs(game->rng.next_int() % 12);
        game->offerBoard.schedule_due_date(free_row, offer_scale, game->rng);
        int offer_type = std::abs(game->rng.next_int() % 16);
        int engine_count = 0;
        int active_train_count = 0;
        int type4_count = 0; // Current game logic keeps this value at zero.
        int type45_count = 0;
        int type3_count = 0;
        int type267_count = 0;
        for (int train = 0; train < 500; ++train) {
            if (!train_flag::isUsed(game->fleet.trainCars[train].flags)
                || game->world.trackMap[unsignedByte(game->fleet.trainCars[train].x)]
                               [unsignedByte(game->fleet.trainCars[train].y)] != 65)
                continue;
            ++active_train_count;
            if (train_flag::isHead(game->fleet.trainCars[train].flags)) {
                ++engine_count;
                continue;
            }
            switch (game->fleet.trainCars[train].flags & 7) {
                case 2: ++type267_count; break;
                case 3: ++type3_count; break;
                case 4: case 5: ++type45_count; break;
                case 6: case 7: ++type267_count; break;
            }
        }
        (void)active_train_count;
        game->offerBoard.offers[free_row].objectiveType =
            offer_type > 0 && offer_type < 15 ? (int8_t)offer_type : 0;
        if (offer_type == 1)
            game->offerBoard.offers[free_row].target = offer_scale * (engine_count + 2)
                                         * (3000 + game->rng.next_int() % 1500);
        if (offer_type == 2)
            game->offerBoard.offers[free_row].target = game->economy.money + offer_scale * (engine_count + 2)
                                         * (2000 + game->rng.next_int() % 1000);
        if (offer_type == 3) game->offerBoard.offers[free_row].objectiveType = 0;
        if (offer_type == 4)
            game->offerBoard.offers[free_row].target = offer_scale * (type4_count * 2 + 5)
                                         * (25 + game->rng.next_int() % 3);
        if (offer_type == 5)
            game->offerBoard.offers[free_row].target = offer_scale * (type45_count * 2 + 5)
                                         * (25 + game->rng.next_int() % 3);
        if (offer_type == 6)
            game->offerBoard.offers[free_row].target = offer_scale * (type3_count * 2 + 5)
                                         * (25 + game->rng.next_int() % 3);
        if (offer_type == 7)
            game->offerBoard.offers[free_row].target = offer_scale * (type267_count * 2 + 5)
                                         * (18 + game->rng.next_int() % 3);
        if (offer_type == 8)
            game->offerBoard.offers[free_row].target = offer_scale * (type267_count * 2 + 5)
                                         * (10 + game->rng.next_int() % 3);
        if (offer_type >= 9 && offer_type <= 12)
            game->offerBoard.offers[free_row].target = 3 + std::abs(game->rng.next_int() % 8);
        if (offer_type == 13)
            game->offerBoard.offers[free_row].target =
                (13324 + std::abs(game->rng.next_int() % 5000)) * offer_scale;
        if (offer_type == 14) {
            if (game->economy.loanBalance > 40000)
                game->offerBoard.offers[free_row].target = game->economy.loanBalance
                    * (75 + std::abs(game->rng.next_int() % 15)) / 100;
            else
                game->offerBoard.offers[free_row].objectiveType = 0;
        }
        if (offer_type != 0) {
            int effect_code = 0;
            while (effect_code == 0) {
                effect_code = 1 + std::abs(game->rng.next_int() % 12);
                const std::vector<int8_t> &allowed_types = game->scenario.offerTypesByEffect[effect_code - 1];
                bool valid = false;
                for (int i = 0; i < static_cast<int>(allowed_types.size()); ++i) {
                    if (allowed_types[i] == offer_type) valid = true;
                }
                if (effect_code == 12 && game->economy.interestRate <= 1) valid = false;
                if (effect_code == 3 && game->economy.loanCeiling >= 1000000) valid = false;
                if (effect_code == 4 && game->era >= 5) valid = false;
                if (game->scenario.offerGenerationCount
                    < game->scenario.offerEffectSchedule[effect_code - 1][0])
                    valid = false;
                if (game->scenario.offerGenerationCount < game->scenario.offerEffectNextCount[effect_code - 1])
                    valid = false;
                if (!valid) effect_code = 0;
            }
            int effect_index = effect_code - 1;
            game->scenario.offerEffectNextCount[effect_index] =
                (int16_t)(game->scenario.offerEffectNextCount[effect_index]
                          + unsignedByte(game->scenario.offerEffectSchedule[effect_index][1]));
            if (effect_code == 1) {
                game->offerBoard.offers[free_row].effectValue =
                    (int8_t)(30 + std::abs(game->rng.next_int() % 50));
                game->offerBoard.offers[free_row].effectAmount =
                    game->offerBoard.offers[free_row].target * game->offerBoard.offers[free_row].effectValue / 100;
            }
            if (effect_code == 2)
                game->offerBoard.offers[free_row].effectAmount =
                    5000 * (6 + std::abs(game->rng.next_int() % 17));
            if (effect_code == 3)
                game->offerBoard.offers[free_row].effectAmount =
                    20000 * (3 + std::abs(game->rng.next_int() % 3));
            if (effect_code >= 5 && effect_code <= 10)
                game->offerBoard.offers[free_row].effectValue =
                    (int8_t)(25 + std::abs(game->rng.next_int() % 45));
            if (effect_code == 11)
                game->offerBoard.offers[free_row].effectValue =
                    (int8_t)(15 + std::abs(game->rng.next_int() % 30));
            game->offerBoard.offers[free_row].effectType = (int8_t)effect_code;
        }
        if (game->offerBoard.offers[free_row].objectiveType != 0) {
            game->ready = false;
            game->ui.transitionCode = 40;
            game->scenario.offerRowIndex = (int8_t)free_row;
            game_setState(game, GameState::MenuOverlay);
            ++game->scenario.offerGenerationCount;
        }
    }
    game->scenario.lastOfferDateCode = date;
}
