// Monthly economy and offer processing.
#include "game.h"
#include <cstdlib>

void game_placeTrainAt(Game *game, int32_t duration_units, int32_t row) {
    int delay_days = duration_units * 60 + 20 + game->rng.next_int() % 20;
    int due_day = game->ui.placeTrainDay[row] + delay_days;
    int due_month = game->ui.placeTrainMonth[row];
    int due_year = game->ui.placeTrainYear[row];
    while (due_day > 28) {
        due_day -= 28;
        ++due_month;
    }
    while (due_month > 12) {
        due_month -= 12;
        ++due_year;
    }
    if (due_day == 1) ++due_day;
    game->ui.menuRowParamA[row] = (int8_t)due_day;
    game->ui.menuRowParamB[row] = (int8_t)due_month;
    game->ui.menuRowParamC[row] = (int8_t)due_year;
}

static void copy_menu_row_to_slot5(Game *game, int row) {
    game->scenario.offerTypeTransitionCode = game->ui.menuRowType[row] + 40;
    game->scenario.offerEffectTransitionCode = game->ui.menuRowAltType[row] + 60;
    game->scenario.offerRowIndex = 5;
    game->ui.menuRowType[5] = game->ui.menuRowType[row];
    game->ui.menuRowCost[5] = game->ui.menuRowCost[row];
    game->ui.menuRowValue[5] = game->ui.menuRowValue[row];
    game->ui.menuRowAltType[5] = game->ui.menuRowAltType[row];
    game->ui.menuRowEra[5] = game->ui.menuRowEra[row];
    game->ui.menuRowRevenue[5] = game->ui.menuRowRevenue[row];
    game->ui.placeTrainDay[5] = game->ui.placeTrainDay[row];
    game->ui.placeTrainMonth[5] = game->ui.placeTrainMonth[row];
    game->ui.placeTrainYear[5] = game->ui.placeTrainYear[row];
    game->ui.menuRowParamA[5] = game->ui.menuRowParamA[row];
    game->ui.menuRowParamB[5] = game->ui.menuRowParamB[row];
    game->ui.menuRowParamC[5] = game->ui.menuRowParamC[row];
    game->ui.menuRowType[row] = 0;
    game->ui.menuRowCost[row] = 0;
    game->ui.menuRowValue[row] = 0;
    game->ui.menuRowAltType[row] = 0;
    game->ui.menuRowEra[row] = 0;
    game->ui.menuRowRevenue[row] = 0;
    game->ready = false;
}

void game_applyMenuRowEffect(Game *game, int32_t row) {
    if (game->ui.menuRowAltType[row] == 1 || game->ui.menuRowAltType[row] == 2) {
        game->economy.money += game->ui.menuRowRevenue[row];
        game_pushMoneyHistory(game, game->ui.menuRowRevenue[row]);
    }
    if (game->ui.menuRowAltType[row] == 3) game->economy.loanCeiling += game->ui.menuRowRevenue[row];
    if (game->ui.menuRowAltType[row] == 4 && game->era < 5) ++game->era;
    if (game->ui.menuRowAltType[row] >= 5 && game->ui.menuRowAltType[row] <= 10) {
        int resource = game->ui.menuRowAltType[row] - 5;
        game->scenario.routeResourceFlags[resource] =
            (int8_t)(game->scenario.routeResourceFlags[resource] + game->ui.menuRowEra[row]);
    }
    if (game->ui.menuRowAltType[row] == 11) {
        game->economy.loanBalance -= game->economy.loanBalance * game->ui.menuRowEra[row] / 100;
        if (game->economy.loanBalance < 0) game->economy.loanBalance = 0;
        if (game->economy.loanBalance % 20000 != 0) {
            if (game->economy.loanBalance % 20000 < 10000)
                game->economy.loanBalance -= game->economy.loanBalance % 20000;
            else
                game->economy.loanBalance += 20000 - game->economy.loanBalance % 20000;
        }
    }
    if (game->ui.menuRowAltType[row] == 12 && game->economy.interestRate > 1)
        --game->economy.interestRate;
    copy_menu_row_to_slot5(game, row);
    game->scenario.duplicateRowState = 3;
    game->ui.transitionCode = 9;
    game_setState(game, +GameState::MenuOverlay);
}

void game_duplicateMenuRow(Game *game, int32_t row) {
    copy_menu_row_to_slot5(game, row);
    game->scenario.duplicateRowState = 4;
    game->ui.transitionCode = 8;
    game_setState(game, +GameState::MenuOverlay);
}

void game_everyMonthlyTick(Game *game) {
    if (!game->scenario.monthlyOffersEnabled) return;
    for (int resource = 0; resource < 6; ++resource) {
        if (game->scenario.routeResourceFlags[resource] > 0) --game->scenario.routeResourceFlags[resource];
    }
    for (int row = 0; row < 5; ++row) {
        if ((game->ui.menuRowType[row] == 1
             || (game->ui.menuRowType[row] >= 4 && game->ui.menuRowType[row] <= 14))
            && game->ui.menuRowCost[row] <= game->ui.menuRowValue[row])
            game_applyMenuRowEffect(game, row);
        if (game->ui.menuRowParamA[row] != game->economy.dayOfMonth
            || game->ui.menuRowParamB[row] != game->economy.monthOfYear
            || game->ui.menuRowParamC[row] != game->economy.year - 1950)
            continue;
        if (game->ui.menuRowType[row] == 2 && game->ui.menuRowCost[row] <= game->economy.money)
            game_applyMenuRowEffect(game, row);
        if (game->ui.menuRowType[row] != 0) game_duplicateMenuRow(game, row);
    }

    int date = game->economy.dayOfMonth + 32 * game->economy.monthOfYear + 370 * (game->economy.year - 1950);
    if (date - game->scenario.lastOfferDateCode <= 150 + game->rng.next_int() % 20)
        return;
    int free_row = -1;
    for (int row = 0; row < 5; ++row) {
        if (game->ui.menuRowType[row] == 0) {
            free_row = row;
            break;
        }
    }
    if (free_row > -1) {
        game->ui.placeTrainDay[free_row] = (int8_t)game->economy.dayOfMonth;
        game->ui.placeTrainMonth[free_row] = (int8_t)game->economy.monthOfYear;
        game->ui.placeTrainYear[free_row] = (int8_t)(game->economy.year - 1950);
        int offer_scale = 4 + std::abs(game->rng.next_int() % 12);
        game_placeTrainAt(game, offer_scale, free_row);
        int offer_type = std::abs(game->rng.next_int() % 16);
        int engine_count = 0;
        int active_train_count = 0;
        int type4_count = 0; // Current game logic keeps this value at zero.
        int type45_count = 0;
        int type3_count = 0;
        int type267_count = 0;
        for (int train = 0; train < 500; ++train) {
            if ((game->fleet.trainFlags[train] & 0x80) != 128
                || game->world.trackMap[game_byteToUnsignedInt(game, game->fleet.trainX[train])]
                               [game_byteToUnsignedInt(game, game->fleet.trainY[train])] != 65)
                continue;
            ++active_train_count;
            if ((game->fleet.trainFlags[train] & 0x40) == 64) {
                ++engine_count;
                continue;
            }
            switch (game->fleet.trainFlags[train] & 7) {
                case 2: ++type267_count; break;
                case 3: ++type3_count; break;
                case 4: case 5: ++type45_count; break;
                case 6: case 7: ++type267_count; break;
            }
        }
        (void)active_train_count;
        game->ui.menuRowType[free_row] =
            offer_type > 0 && offer_type < 15 ? (int8_t)offer_type : 0;
        if (offer_type == 1)
            game->ui.menuRowCost[free_row] = offer_scale * (engine_count + 2)
                                         * (3000 + game->rng.next_int() % 1500);
        if (offer_type == 2)
            game->ui.menuRowCost[free_row] = game->economy.money + offer_scale * (engine_count + 2)
                                         * (2000 + game->rng.next_int() % 1000);
        if (offer_type == 3) game->ui.menuRowType[free_row] = 0;
        if (offer_type == 4)
            game->ui.menuRowCost[free_row] = offer_scale * (type4_count * 2 + 5)
                                         * (25 + game->rng.next_int() % 3);
        if (offer_type == 5)
            game->ui.menuRowCost[free_row] = offer_scale * (type45_count * 2 + 5)
                                         * (25 + game->rng.next_int() % 3);
        if (offer_type == 6)
            game->ui.menuRowCost[free_row] = offer_scale * (type3_count * 2 + 5)
                                         * (25 + game->rng.next_int() % 3);
        if (offer_type == 7)
            game->ui.menuRowCost[free_row] = offer_scale * (type267_count * 2 + 5)
                                         * (18 + game->rng.next_int() % 3);
        if (offer_type == 8)
            game->ui.menuRowCost[free_row] = offer_scale * (type267_count * 2 + 5)
                                         * (10 + game->rng.next_int() % 3);
        if (offer_type >= 9 && offer_type <= 12)
            game->ui.menuRowCost[free_row] = 3 + std::abs(game->rng.next_int() % 8);
        if (offer_type == 13)
            game->ui.menuRowCost[free_row] =
                (13324 + std::abs(game->rng.next_int() % 5000)) * offer_scale;
        if (offer_type == 14) {
            if (game->economy.loanBalance > 40000)
                game->ui.menuRowCost[free_row] = game->economy.loanBalance
                    * (75 + std::abs(game->rng.next_int() % 15)) / 100;
            else
                game->ui.menuRowType[free_row] = 0;
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
                          + game_byteToUnsignedInt(game, game->scenario.offerEffectSchedule[effect_index][1]));
            if (effect_code == 1) {
                game->ui.menuRowEra[free_row] =
                    (int8_t)(30 + std::abs(game->rng.next_int() % 50));
                game->ui.menuRowRevenue[free_row] =
                    game->ui.menuRowCost[free_row] * game->ui.menuRowEra[free_row] / 100;
            }
            if (effect_code == 2)
                game->ui.menuRowRevenue[free_row] =
                    5000 * (6 + std::abs(game->rng.next_int() % 17));
            if (effect_code == 3)
                game->ui.menuRowRevenue[free_row] =
                    20000 * (3 + std::abs(game->rng.next_int() % 3));
            if (effect_code >= 5 && effect_code <= 10)
                game->ui.menuRowEra[free_row] =
                    (int8_t)(25 + std::abs(game->rng.next_int() % 45));
            if (effect_code == 11)
                game->ui.menuRowEra[free_row] =
                    (int8_t)(15 + std::abs(game->rng.next_int() % 30));
            game->ui.menuRowAltType[free_row] = (int8_t)effect_code;
        }
        if (game->ui.menuRowType[free_row] != 0) {
            game->ready = false;
            game->ui.transitionCode = 40;
            game->scenario.offerRowIndex = (int8_t)free_row;
            game_setState(game, +GameState::MenuOverlay);
            ++game->scenario.offerGenerationCount;
        }
    }
    game->scenario.lastOfferDateCode = date;
}
