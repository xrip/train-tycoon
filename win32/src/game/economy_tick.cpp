// Monthly economy and offer processing.
#include "game_methods.h"
#include <cstdlib>

// void placeTrainAt(int,int)
void gc_placeTrainAt(GameCanvas *gc, int32_t duration_units, int32_t row) {
    int delay_days = duration_units * 60 + 20 + gc->rng.next_int() % 20;
    int due_day = gc->placeTrainDay[row] + delay_days;
    int due_month = gc->placeTrainMonth[row];
    int due_year = gc->placeTrainYear[row];
    while (due_day > 28) {
        due_day -= 28;
        ++due_month;
    }
    while (due_month > 12) {
        due_month -= 12;
        ++due_year;
    }
    if (due_day == 1) ++due_day;
    gc->menuRowParamA[row] = (int8_t)due_day;
    gc->menuRowParamB[row] = (int8_t)due_month;
    gc->menuRowParamC[row] = (int8_t)due_year;
}

static void copy_menu_row_to_slot5(GameCanvas *gc, int row) {
    gc->offerTypeTransitionCode = gc->menuRowType[row] + 40;
    gc->offerEffectTransitionCode = gc->menuRowAltType[row] + 60;
    gc->offerRowIndex = 5;
    gc->menuRowType[5] = gc->menuRowType[row];
    gc->menuRowCost[5] = gc->menuRowCost[row];
    gc->menuRowValue[5] = gc->menuRowValue[row];
    gc->menuRowAltType[5] = gc->menuRowAltType[row];
    gc->menuRowEra[5] = gc->menuRowEra[row];
    gc->menuRowRevenue[5] = gc->menuRowRevenue[row];
    gc->placeTrainDay[5] = gc->placeTrainDay[row];
    gc->placeTrainMonth[5] = gc->placeTrainMonth[row];
    gc->placeTrainYear[5] = gc->placeTrainYear[row];
    gc->menuRowParamA[5] = gc->menuRowParamA[row];
    gc->menuRowParamB[5] = gc->menuRowParamB[row];
    gc->menuRowParamC[5] = gc->menuRowParamC[row];
    gc->menuRowType[row] = 0;
    gc->menuRowCost[row] = 0;
    gc->menuRowValue[row] = 0;
    gc->menuRowAltType[row] = 0;
    gc->menuRowEra[row] = 0;
    gc->menuRowRevenue[row] = 0;
    gc->ready = false;
}

// void applyMenuRowEffect(int)
void gc_applyMenuRowEffect(GameCanvas *gc, int32_t row) {
    if (gc->menuRowAltType[row] == 1 || gc->menuRowAltType[row] == 2) {
        gc->money += gc->menuRowRevenue[row];
        gc_pushMoneyHistory(gc, gc->menuRowRevenue[row]);
    }
    if (gc->menuRowAltType[row] == 3) gc->loanCeiling += gc->menuRowRevenue[row];
    if (gc->menuRowAltType[row] == 4 && gc->era < 5) ++gc->era;
    if (gc->menuRowAltType[row] >= 5 && gc->menuRowAltType[row] <= 10) {
        int resource = gc->menuRowAltType[row] - 5;
        gc->routeResourceFlags[resource] =
            (int8_t)(gc->routeResourceFlags[resource] + gc->menuRowEra[row]);
    }
    if (gc->menuRowAltType[row] == 11) {
        gc->loanBalance -= gc->loanBalance * gc->menuRowEra[row] / 100;
        if (gc->loanBalance < 0) gc->loanBalance = 0;
        if (gc->loanBalance % 20000 != 0) {
            if (gc->loanBalance % 20000 < 10000)
                gc->loanBalance -= gc->loanBalance % 20000;
            else
                gc->loanBalance += 20000 - gc->loanBalance % 20000;
        }
    }
    if (gc->menuRowAltType[row] == 12 && gc->interestRate > 1)
        --gc->interestRate;
    copy_menu_row_to_slot5(gc, row);
    gc->duplicateRowState = 3;
    gc->transitionCode = 9;
    gc_setState(gc, +GameState::MenuOverlay);
}

// void duplicateMenuRow(int)
void gc_duplicateMenuRow(GameCanvas *gc, int32_t row) {
    copy_menu_row_to_slot5(gc, row);
    gc->duplicateRowState = 4;
    gc->transitionCode = 8;
    gc_setState(gc, +GameState::MenuOverlay);
}

// void everyMonthlyTick()
void gc_everyMonthlyTick(GameCanvas *gc) {
    if (!gc->flagsB) return;
    for (int resource = 0; resource < 6; ++resource) {
        if (gc->routeResourceFlags[resource] > 0) --gc->routeResourceFlags[resource];
    }
    for (int row = 0; row < 5; ++row) {
        if ((gc->menuRowType[row] == 1
             || (gc->menuRowType[row] >= 4 && gc->menuRowType[row] <= 14))
            && gc->menuRowCost[row] <= gc->menuRowValue[row])
            gc_applyMenuRowEffect(gc, row);
        if (gc->menuRowParamA[row] != gc->dayOfMonth
            || gc->menuRowParamB[row] != gc->monthOfYear
            || gc->menuRowParamC[row] != gc->year - 1950)
            continue;
        if (gc->menuRowType[row] == 2 && gc->menuRowCost[row] <= gc->money)
            gc_applyMenuRowEffect(gc, row);
        if (gc->menuRowType[row] != 0) gc_duplicateMenuRow(gc, row);
    }

    int date = gc->dayOfMonth + 32 * gc->monthOfYear + 370 * (gc->year - 1950);
    if (date - gc->lastOfferDateCode <= 150 + gc->rng.next_int() % 20)
        return;
    int free_row = -1;
    for (int row = 0; row < 5; ++row) {
        if (gc->menuRowType[row] == 0) {
            free_row = row;
            break;
        }
    }
    if (free_row > -1) {
        gc->placeTrainDay[free_row] = (int8_t)gc->dayOfMonth;
        gc->placeTrainMonth[free_row] = (int8_t)gc->monthOfYear;
        gc->placeTrainYear[free_row] = (int8_t)(gc->year - 1950);
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
        gc->menuRowType[free_row] =
            offer_type > 0 && offer_type < 15 ? (int8_t)offer_type : 0;
        if (offer_type == 1)
            gc->menuRowCost[free_row] = offer_scale * (engine_count + 2)
                                         * (3000 + gc->rng.next_int() % 1500);
        if (offer_type == 2)
            gc->menuRowCost[free_row] = gc->money + offer_scale * (engine_count + 2)
                                         * (2000 + gc->rng.next_int() % 1000);
        if (offer_type == 3) gc->menuRowType[free_row] = 0;
        if (offer_type == 4)
            gc->menuRowCost[free_row] = offer_scale * (type4_count * 2 + 5)
                                         * (25 + gc->rng.next_int() % 3);
        if (offer_type == 5)
            gc->menuRowCost[free_row] = offer_scale * (type45_count * 2 + 5)
                                         * (25 + gc->rng.next_int() % 3);
        if (offer_type == 6)
            gc->menuRowCost[free_row] = offer_scale * (type3_count * 2 + 5)
                                         * (25 + gc->rng.next_int() % 3);
        if (offer_type == 7)
            gc->menuRowCost[free_row] = offer_scale * (type267_count * 2 + 5)
                                         * (18 + gc->rng.next_int() % 3);
        if (offer_type == 8)
            gc->menuRowCost[free_row] = offer_scale * (type267_count * 2 + 5)
                                         * (10 + gc->rng.next_int() % 3);
        if (offer_type >= 9 && offer_type <= 12)
            gc->menuRowCost[free_row] = 3 + std::abs(gc->rng.next_int() % 8);
        if (offer_type == 13)
            gc->menuRowCost[free_row] =
                (13324 + std::abs(gc->rng.next_int() % 5000)) * offer_scale;
        if (offer_type == 14) {
            if (gc->loanBalance > 40000)
                gc->menuRowCost[free_row] = gc->loanBalance
                    * (75 + std::abs(gc->rng.next_int() % 15)) / 100;
            else
                gc->menuRowType[free_row] = 0;
        }
        if (offer_type != 0) {
            int effect_code = 0;
            while (effect_code == 0) {
                effect_code = 1 + std::abs(gc->rng.next_int() % 12);
                const std::vector<int8_t> &allowed_types = gc->offerTypesByEffect[effect_code - 1];
                bool valid = false;
                for (int i = 0; i < static_cast<int>(allowed_types.size()); ++i) {
                    if (allowed_types[i] == offer_type) valid = true;
                }
                if (effect_code == 12 && gc->interestRate <= 1) valid = false;
                if (effect_code == 3 && gc->loanCeiling >= 1000000) valid = false;
                if (effect_code == 4 && gc->era >= 5) valid = false;
                if (gc->offerGenerationCount
                    < gc->offerEffectSchedule[effect_code - 1][0])
                    valid = false;
                if (gc->offerGenerationCount < gc->offerEffectNextCount[effect_code - 1])
                    valid = false;
                if (!valid) effect_code = 0;
            }
            int effect_index = effect_code - 1;
            gc->offerEffectNextCount[effect_index] =
                (int16_t)(gc->offerEffectNextCount[effect_index]
                          + gc_byteToUnsignedInt(gc, gc->offerEffectSchedule[effect_index][1]));
            if (effect_code == 1) {
                gc->menuRowEra[free_row] =
                    (int8_t)(30 + std::abs(gc->rng.next_int() % 50));
                gc->menuRowRevenue[free_row] =
                    gc->menuRowCost[free_row] * gc->menuRowEra[free_row] / 100;
            }
            if (effect_code == 2)
                gc->menuRowRevenue[free_row] =
                    5000 * (6 + std::abs(gc->rng.next_int() % 17));
            if (effect_code == 3)
                gc->menuRowRevenue[free_row] =
                    20000 * (3 + std::abs(gc->rng.next_int() % 3));
            if (effect_code >= 5 && effect_code <= 10)
                gc->menuRowEra[free_row] =
                    (int8_t)(25 + std::abs(gc->rng.next_int() % 45));
            if (effect_code == 11)
                gc->menuRowEra[free_row] =
                    (int8_t)(15 + std::abs(gc->rng.next_int() % 30));
            gc->menuRowAltType[free_row] = (int8_t)effect_code;
        }
        if (gc->menuRowType[free_row] != 0) {
            gc->ready = false;
            gc->transitionCode = 40;
            gc->offerRowIndex = (int8_t)free_row;
            gc_setState(gc, +GameState::MenuOverlay);
            ++gc->offerGenerationCount;
        }
    }
    gc->lastOfferDateCode = date;
}
