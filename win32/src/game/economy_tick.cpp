// Monthly economy and offer processing.
#include "game_methods.h"
#include <cstdlib>

// void placeTrainAt(int,int)
void gc_placeTrainAt(GameCanvas *gc, int32_t duration_units, int32_t row) {
    int delay_days = duration_units * 60 + 20 + gc->rng.next_int() % 20;
    int due_day = gc->ui.placeTrainDay[row] + delay_days;
    int due_month = gc->ui.placeTrainMonth[row];
    int due_year = gc->ui.placeTrainYear[row];
    while (due_day > 28) {
        due_day -= 28;
        ++due_month;
    }
    while (due_month > 12) {
        due_month -= 12;
        ++due_year;
    }
    if (due_day == 1) ++due_day;
    gc->ui.menuRowParamA[row] = (int8_t)due_day;
    gc->ui.menuRowParamB[row] = (int8_t)due_month;
    gc->ui.menuRowParamC[row] = (int8_t)due_year;
}

static void copy_menu_row_to_slot5(GameCanvas *gc, int row) {
    gc->scenario.offerTypeTransitionCode = gc->ui.menuRowType[row] + 40;
    gc->scenario.offerEffectTransitionCode = gc->ui.menuRowAltType[row] + 60;
    gc->scenario.offerRowIndex = 5;
    gc->ui.menuRowType[5] = gc->ui.menuRowType[row];
    gc->ui.menuRowCost[5] = gc->ui.menuRowCost[row];
    gc->ui.menuRowValue[5] = gc->ui.menuRowValue[row];
    gc->ui.menuRowAltType[5] = gc->ui.menuRowAltType[row];
    gc->ui.menuRowEra[5] = gc->ui.menuRowEra[row];
    gc->ui.menuRowRevenue[5] = gc->ui.menuRowRevenue[row];
    gc->ui.placeTrainDay[5] = gc->ui.placeTrainDay[row];
    gc->ui.placeTrainMonth[5] = gc->ui.placeTrainMonth[row];
    gc->ui.placeTrainYear[5] = gc->ui.placeTrainYear[row];
    gc->ui.menuRowParamA[5] = gc->ui.menuRowParamA[row];
    gc->ui.menuRowParamB[5] = gc->ui.menuRowParamB[row];
    gc->ui.menuRowParamC[5] = gc->ui.menuRowParamC[row];
    gc->ui.menuRowType[row] = 0;
    gc->ui.menuRowCost[row] = 0;
    gc->ui.menuRowValue[row] = 0;
    gc->ui.menuRowAltType[row] = 0;
    gc->ui.menuRowEra[row] = 0;
    gc->ui.menuRowRevenue[row] = 0;
    gc->ready = false;
}

// void applyMenuRowEffect(int)
void gc_applyMenuRowEffect(GameCanvas *gc, int32_t row) {
    if (gc->ui.menuRowAltType[row] == 1 || gc->ui.menuRowAltType[row] == 2) {
        gc->economy.money += gc->ui.menuRowRevenue[row];
        gc_pushMoneyHistory(gc, gc->ui.menuRowRevenue[row]);
    }
    if (gc->ui.menuRowAltType[row] == 3) gc->economy.loanCeiling += gc->ui.menuRowRevenue[row];
    if (gc->ui.menuRowAltType[row] == 4 && gc->era < 5) ++gc->era;
    if (gc->ui.menuRowAltType[row] >= 5 && gc->ui.menuRowAltType[row] <= 10) {
        int resource = gc->ui.menuRowAltType[row] - 5;
        gc->scenario.routeResourceFlags[resource] =
            (int8_t)(gc->scenario.routeResourceFlags[resource] + gc->ui.menuRowEra[row]);
    }
    if (gc->ui.menuRowAltType[row] == 11) {
        gc->economy.loanBalance -= gc->economy.loanBalance * gc->ui.menuRowEra[row] / 100;
        if (gc->economy.loanBalance < 0) gc->economy.loanBalance = 0;
        if (gc->economy.loanBalance % 20000 != 0) {
            if (gc->economy.loanBalance % 20000 < 10000)
                gc->economy.loanBalance -= gc->economy.loanBalance % 20000;
            else
                gc->economy.loanBalance += 20000 - gc->economy.loanBalance % 20000;
        }
    }
    if (gc->ui.menuRowAltType[row] == 12 && gc->economy.interestRate > 1)
        --gc->economy.interestRate;
    copy_menu_row_to_slot5(gc, row);
    gc->scenario.duplicateRowState = 3;
    gc->transitionCode = 9;
    gc_setState(gc, +GameState::MenuOverlay);
}

// void duplicateMenuRow(int)
void gc_duplicateMenuRow(GameCanvas *gc, int32_t row) {
    copy_menu_row_to_slot5(gc, row);
    gc->scenario.duplicateRowState = 4;
    gc->transitionCode = 8;
    gc_setState(gc, +GameState::MenuOverlay);
}

// void everyMonthlyTick()
void gc_everyMonthlyTick(GameCanvas *gc) {
    if (!gc->flagsB) return;
    for (int resource = 0; resource < 6; ++resource) {
        if (gc->scenario.routeResourceFlags[resource] > 0) --gc->scenario.routeResourceFlags[resource];
    }
    for (int row = 0; row < 5; ++row) {
        if ((gc->ui.menuRowType[row] == 1
             || (gc->ui.menuRowType[row] >= 4 && gc->ui.menuRowType[row] <= 14))
            && gc->ui.menuRowCost[row] <= gc->ui.menuRowValue[row])
            gc_applyMenuRowEffect(gc, row);
        if (gc->ui.menuRowParamA[row] != gc->economy.dayOfMonth
            || gc->ui.menuRowParamB[row] != gc->economy.monthOfYear
            || gc->ui.menuRowParamC[row] != gc->economy.year - 1950)
            continue;
        if (gc->ui.menuRowType[row] == 2 && gc->ui.menuRowCost[row] <= gc->economy.money)
            gc_applyMenuRowEffect(gc, row);
        if (gc->ui.menuRowType[row] != 0) gc_duplicateMenuRow(gc, row);
    }

    int date = gc->economy.dayOfMonth + 32 * gc->economy.monthOfYear + 370 * (gc->economy.year - 1950);
    if (date - gc->scenario.lastOfferDateCode <= 150 + gc->rng.next_int() % 20)
        return;
    int free_row = -1;
    for (int row = 0; row < 5; ++row) {
        if (gc->ui.menuRowType[row] == 0) {
            free_row = row;
            break;
        }
    }
    if (free_row > -1) {
        gc->ui.placeTrainDay[free_row] = (int8_t)gc->economy.dayOfMonth;
        gc->ui.placeTrainMonth[free_row] = (int8_t)gc->economy.monthOfYear;
        gc->ui.placeTrainYear[free_row] = (int8_t)(gc->economy.year - 1950);
        int offer_scale = 4 + std::abs(gc->rng.next_int() % 12);
        gc_placeTrainAt(gc, offer_scale, free_row);
        int offer_type = std::abs(gc->rng.next_int() % 16);
        int engine_count = 0;
        int active_train_count = 0;
        int type4_count = 0; // Current game logic keeps this value at zero.
        int type45_count = 0;
        int type3_count = 0;
        int type267_count = 0;
        for (int train = 0; train < 500; ++train) {
            if ((gc->fleet.trainFlags[train] & 0x80) != 128
                || gc->world.trackMap[gc_byteToUnsignedInt(gc, gc->fleet.trainX[train])]
                               [gc_byteToUnsignedInt(gc, gc->fleet.trainY[train])] != 65)
                continue;
            ++active_train_count;
            if ((gc->fleet.trainFlags[train] & 0x40) == 64) {
                ++engine_count;
                continue;
            }
            switch (gc->fleet.trainFlags[train] & 7) {
                case 2: ++type267_count; break;
                case 3: ++type3_count; break;
                case 4: case 5: ++type45_count; break;
                case 6: case 7: ++type267_count; break;
            }
        }
        (void)active_train_count;
        gc->ui.menuRowType[free_row] =
            offer_type > 0 && offer_type < 15 ? (int8_t)offer_type : 0;
        if (offer_type == 1)
            gc->ui.menuRowCost[free_row] = offer_scale * (engine_count + 2)
                                         * (3000 + gc->rng.next_int() % 1500);
        if (offer_type == 2)
            gc->ui.menuRowCost[free_row] = gc->economy.money + offer_scale * (engine_count + 2)
                                         * (2000 + gc->rng.next_int() % 1000);
        if (offer_type == 3) gc->ui.menuRowType[free_row] = 0;
        if (offer_type == 4)
            gc->ui.menuRowCost[free_row] = offer_scale * (type4_count * 2 + 5)
                                         * (25 + gc->rng.next_int() % 3);
        if (offer_type == 5)
            gc->ui.menuRowCost[free_row] = offer_scale * (type45_count * 2 + 5)
                                         * (25 + gc->rng.next_int() % 3);
        if (offer_type == 6)
            gc->ui.menuRowCost[free_row] = offer_scale * (type3_count * 2 + 5)
                                         * (25 + gc->rng.next_int() % 3);
        if (offer_type == 7)
            gc->ui.menuRowCost[free_row] = offer_scale * (type267_count * 2 + 5)
                                         * (18 + gc->rng.next_int() % 3);
        if (offer_type == 8)
            gc->ui.menuRowCost[free_row] = offer_scale * (type267_count * 2 + 5)
                                         * (10 + gc->rng.next_int() % 3);
        if (offer_type >= 9 && offer_type <= 12)
            gc->ui.menuRowCost[free_row] = 3 + std::abs(gc->rng.next_int() % 8);
        if (offer_type == 13)
            gc->ui.menuRowCost[free_row] =
                (13324 + std::abs(gc->rng.next_int() % 5000)) * offer_scale;
        if (offer_type == 14) {
            if (gc->economy.loanBalance > 40000)
                gc->ui.menuRowCost[free_row] = gc->economy.loanBalance
                    * (75 + std::abs(gc->rng.next_int() % 15)) / 100;
            else
                gc->ui.menuRowType[free_row] = 0;
        }
        if (offer_type != 0) {
            int effect_code = 0;
            while (effect_code == 0) {
                effect_code = 1 + std::abs(gc->rng.next_int() % 12);
                const std::vector<int8_t> &allowed_types = gc->scenario.offerTypesByEffect[effect_code - 1];
                bool valid = false;
                for (int i = 0; i < static_cast<int>(allowed_types.size()); ++i) {
                    if (allowed_types[i] == offer_type) valid = true;
                }
                if (effect_code == 12 && gc->economy.interestRate <= 1) valid = false;
                if (effect_code == 3 && gc->economy.loanCeiling >= 1000000) valid = false;
                if (effect_code == 4 && gc->era >= 5) valid = false;
                if (gc->scenario.offerGenerationCount
                    < gc->scenario.offerEffectSchedule[effect_code - 1][0])
                    valid = false;
                if (gc->scenario.offerGenerationCount < gc->scenario.offerEffectNextCount[effect_code - 1])
                    valid = false;
                if (!valid) effect_code = 0;
            }
            int effect_index = effect_code - 1;
            gc->scenario.offerEffectNextCount[effect_index] =
                (int16_t)(gc->scenario.offerEffectNextCount[effect_index]
                          + gc_byteToUnsignedInt(gc, gc->scenario.offerEffectSchedule[effect_index][1]));
            if (effect_code == 1) {
                gc->ui.menuRowEra[free_row] =
                    (int8_t)(30 + std::abs(gc->rng.next_int() % 50));
                gc->ui.menuRowRevenue[free_row] =
                    gc->ui.menuRowCost[free_row] * gc->ui.menuRowEra[free_row] / 100;
            }
            if (effect_code == 2)
                gc->ui.menuRowRevenue[free_row] =
                    5000 * (6 + std::abs(gc->rng.next_int() % 17));
            if (effect_code == 3)
                gc->ui.menuRowRevenue[free_row] =
                    20000 * (3 + std::abs(gc->rng.next_int() % 3));
            if (effect_code >= 5 && effect_code <= 10)
                gc->ui.menuRowEra[free_row] =
                    (int8_t)(25 + std::abs(gc->rng.next_int() % 45));
            if (effect_code == 11)
                gc->ui.menuRowEra[free_row] =
                    (int8_t)(15 + std::abs(gc->rng.next_int() % 30));
            gc->ui.menuRowAltType[free_row] = (int8_t)effect_code;
        }
        if (gc->ui.menuRowType[free_row] != 0) {
            gc->ready = false;
            gc->transitionCode = 40;
            gc->scenario.offerRowIndex = (int8_t)free_row;
            gc_setState(gc, +GameState::MenuOverlay);
            ++gc->scenario.offerGenerationCount;
        }
    }
    gc->scenario.lastOfferDateCode = date;
}
