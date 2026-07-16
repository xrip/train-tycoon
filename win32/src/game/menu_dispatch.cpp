// Build-menu action dispatch.
// Routes a (menu id, option) selection to state transitions / economy actions.
// Some actions use `goto hbms_end` to reach the common tail.
#include "game_methods.h"

// void handleBuildMenuState(int n, int n2)
void gc_handleBuildMenuState(GameCanvas *gc, int32_t n, int32_t n2) {
    switch (n) {
        case 1:
            switch (n2) {
                case 1:
                    if (gc->ready) { gc->transitionCode = 4; gc_resetOverlayRenderState(gc); break; }
                    gc->pendingTransitionCode = 4; gc_setState(gc, +GameState::Playing); break;
                case 2:
                    gc->screenParam = 1;
                    if (gc->ready) { gc->transitionCode = 14; gc_resetOverlayRenderState(gc); break; }
                    gc->pendingTransitionCode = 14; gc_setState(gc, +GameState::Playing); break;
                case 3:
                    gc->screenParam = 0;
                    if (gc->ready) { gc->transitionCode = 14; gc_resetOverlayRenderState(gc); break; }
                    gc->pendingTransitionCode = 14; gc_setState(gc, +GameState::Playing); break;
                case 4:
                    if (gc->ready) { gc->transitionCode = 10; gc_resetOverlayRenderState(gc); break; }
                    gc->pendingTransitionCode = 10; gc_setState(gc, +GameState::Playing); break;
                case 5:
                    if (gc->ready) { gc->transitionCode = 2; gc_resetOverlayRenderState(gc); break; }
                    gc->pendingTransitionCode = 2; gc_setState(gc, +GameState::Playing); break;
                case 6:
                    if (gc->ready) { gc->transitionCode = 3; gc_resetOverlayRenderState(gc); break; }
                    gc->pendingTransitionCode = 3; gc_setState(gc, +GameState::Playing); break;
                case 7:
                    gc_setState(gc, +GameState::Playing); break;
                case 8:
                    if (gc->ready) { gc->transitionCode = 5; gc_resetOverlayRenderState(gc); break; }
                    gc->pendingTransitionCode = 5; gc_setState(gc, +GameState::Playing); break;
                case 9:
                    if (gc->ready) { gc->transitionCode = 28; gc_resetOverlayRenderState(gc); break; }
                    gc->pendingTransitionCode = 28; gc_setState(gc, +GameState::Playing); break;
                case 99:
                    gc->screenParam = 2;
                    if (gc->ready) { gc->transitionCode = 14; gc_resetOverlayRenderState(gc); break; }
                    gc->pendingTransitionCode = 14; gc_setState(gc, +GameState::Playing); break;
            }
            break;
        case 14:
            if (n2 == 2) {
                if (gc->screenParam == 4) { gc->pendingTransitionCode = 26; gc_setState(gc, +GameState::Playing); break; }
                if (gc->screenParam == 3) { gc_setState(gc, +GameState::LoadingResources); break; }
                if (gc->ready) { gc->transitionCode = 1; gc_resetOverlayRenderState(gc); break; }
                gc->pendingTransitionCode = 1; gc_setState(gc, +GameState::Playing); break;
            }
            switch (gc->screenParam) {
                case 0: gc_setState(gc, +GameState::SavingGame); break;
                case 1: gc_setState(gc, +GameState::LoadingGame); break;
                case 2: gc->stopRequested = true; break;
                case 3: gc->music_auto_start = true; gc->music_enabled = true; gc_setState(gc, +GameState::LoadingResources); break;
                case 4: {
                    gc->economy.money += gc->currentTrainCostSnapshot * 70 / 100;
                    gc->economy.aiMoneySpent -= gc->currentTrainCostSnapshot * 70 / 100;
                    if (gc->currentTrainCostSnapshot != 0)
                        gc_pushMoneyHistory(gc, gc->currentTrainCostSnapshot * 70 / 100);
                    int16_t s = gc->fleet.nextCar[gc->trainAtCursor];
                    gc_decrementTrainCargoOptionCounts(gc, s);
                    gc->fleet.trainFlags[gc->trainAtCursor] = 0;
                    while (s != -1) { gc->fleet.trainFlags[s] = 0; s = gc->fleet.nextCar[s]; }
                    gc_setState(gc, +GameState::Playing);
                    break;
                }
            }
            break;
        case 36:
            gc_setState(gc, +GameState::MapOverview); break;
        case 17:
            if (n2 == 1) gc->pendingTransitionCode = transition::SCENARIO_NOTICE_BASE + gc->scenarioNoticeId;
            gc_setState(gc, +GameState::Playing); break;
        case 2:
        case 3:
            if (gc->ready) { gc->transitionCode = 1; gc_resetOverlayRenderState(gc); break; }
            gc->pendingTransitionCode = 1; gc_setState(gc, +GameState::Playing); break;
        case 6:
            gc_setState(gc, +GameState::Playing); break;
        case 7:
            gc->ready = false; gc->scenario.aiEnabled = true; gc->transitionCode = 1;
            gc_resetOverlayRenderState(gc); gc_setState(gc, +GameState::MenuOverlay); break;
        case 5:
            if (n2 >= 2) { gc->activeScenario = n2; gc_setState(gc, +GameState::GeneratingMap); break; }
            if (gc->ready) { gc->transitionCode = 1; gc_resetOverlayRenderState(gc); break; }
            gc->pendingTransitionCode = 1; gc_setState(gc, +GameState::Playing); break;
        case 4:
            switch (n2) {
                case 1:
                    if (gc->scenario.scenarioMilestone == 0) { gc->scenario.scenarioMilestone = 1; goto hbms_end; }
                    gc->scenario.scenarioMilestone = 0; goto hbms_end;
                case 2:
                    if (gc->difficultyA == 0) { gc->difficultyA = 1; goto hbms_end; }
                    gc->difficultyA = 0; goto hbms_end;
                case 3:
                    if (gc->difficultySlot == 0) { gc->difficultySlot = 1; goto hbms_end; }
                    gc->difficultySlot = 0; goto hbms_end;
                case 4:
                    if (gc->difficultyB == 0) { gc->difficultyB = 1; goto hbms_end; }
                    gc->difficultyB = 0; goto hbms_end;
                case 5:
                    if (gc->difficultyC == 0) { gc->difficultyC = 1; goto hbms_end; }
                    gc->difficultyC = 0; goto hbms_end;
                case 6:
                    gc->difficultyCycle = (int8_t)(gc->difficultyCycle + 1);
                    if (gc->difficultyCycle != 4) goto hbms_end;
                    gc->difficultyCycle = 1; goto hbms_end;
                case 10:
                    gc->activeScenario = gc->scenario.scenarioMilestone; gc_setState(gc, +GameState::GeneratingMap); goto hbms_end;
            }
            if (gc->ready) { gc->transitionCode = 1; gc_resetOverlayRenderState(gc); break; }
            gc->pendingTransitionCode = 1; gc_setState(gc, +GameState::Playing); break;
        case 20:
            gc_setState(gc, +GameState::Playing); break;
        case 21:
            gc_setState(gc, +GameState::Playing); break;
        case 22:
            if (n2 == 1) {
                gc->wagonType = 0;
                gc->currentTrainCostSnapshot = 0;
                gc->wagonDiscountType = gc->wagonType;
                for (int n3 = 0; n3 < 5; ++n3) gc->wagonOptionSlots[n3] = -1;
                for (int n3 = 0; n3 < 5; ++n3) gc->wagonCountMatch[n3] = -1;
                gc->difficultyPct = 100;
                gc->discountApplied = false;
                gc_computeTrainCost(gc);
                gc->pendingTransitionCode = 23; gc_setState(gc, +GameState::Playing); goto hbms_end;
            }
            gc_setState(gc, +GameState::Playing); break;
        case 23:
            if (n2 == 0) { gc->pendingTransitionCode = 24; gc_setState(gc, +GameState::Playing); break; }
            if (n2 == 1) { gc->pendingTransitionCode = 25; gc_setState(gc, +GameState::Playing); break; }
            if (n2 == 2) {
                if (!gc->discountApplied) {
                    if (!gc_buyTrainAtCursor(gc)) break;
                    gc_setState(gc, +GameState::Playing); break;
                }
                if (!gc_applyTrainUpgradeInlined(gc, gc->trainAtCursor, gc->wagonType, gc->wagonOptionSlots.data())) break;
                gc->pendingTransitionCode = 26; gc_setState(gc, +GameState::Playing); break;
            }
            gc->pendingTransitionCode = !gc->discountApplied ? 22 : 26;
            gc_setState(gc, +GameState::Playing); break;
        case 24:
            if (n2 >= 0 && n2 < 6) { gc->wagonType = n2; gc_computeTrainCost(gc); }
            gc->pendingTransitionCode = 23; gc_setState(gc, +GameState::Playing); break;
        case 25:
            if (n2 == 3) gc_addSelectedOption(gc, 0);
            else if (n2 == 1) gc_addSelectedOption(gc, 1);
            else if (n2 == 0) gc_addSelectedOption(gc, 2);
            else if (n2 == 2) gc_addSelectedOption(gc, 3);
            else if (n2 == 9) gc_popSelectedOption(gc);
            gc_computeTrainCost(gc);
            gc->pendingTransitionCode = 23; gc_setState(gc, +GameState::Playing); break;
        case 26:
            if (n2 == 1) {
                gc->discountApplied = true;
                gc->pendingTransitionCode = 23; gc_setState(gc, +GameState::Playing);
            } else if (n2 == 2) {
                int n4 = gc->baseTrainCostSnapshot - gc->currentTrainCostSnapshot;
                if (n4 <= gc->economy.money) {
                    gc->fleet.trainAgeOrTimer[gc->trainAtCursor] = 0;
                    gc->economy.money -= n4;
                    gc_pushMoneyHistory(gc, -n4);
                    gc->economy.aiMoneySpent += n4;
                    gc->difficultyPct = 100 - 3 * gc->fleet.trainAgeOrTimer[gc->trainAtCursor];
                    gc_computeTrainCost(gc);
                    gc->currentTrainCostSnapshot = gc->unitCost;
                    gc->pendingTransitionCode = 26; gc_setState(gc, +GameState::Playing);
                } else {
                    gc_enterOutOfMoneyState(gc);
                }
            } else if (n2 == 3) {
                gc->screenParam = 4;
                gc->pendingTransitionCode = 14; gc_setState(gc, +GameState::Playing);
            }
            if (n2 == 4) {
                gc->fleet.trainAiOverride[gc->trainAtCursor] = (int8_t)(gc->fleet.trainAiOverride[gc->trainAtCursor] == 0 ? 1 : 0);
                break;
            }
            gc_setState(gc, +GameState::Playing); break;
        case 10:
            switch (n2) {
                case 1: gcs_animationsDisabled = !gcs_animationsDisabled; goto hbms_end;
                case 2: gcs_selectionHidden = !gcs_selectionHidden; goto hbms_end;
                case 3: gcs_monthlyReportEnabled = !gcs_monthlyReportEnabled; goto hbms_end;
                case 4: gcs_annualReportEnabled = !gcs_annualReportEnabled; goto hbms_end;
                case 5:
                    gc->music_enabled = !gc->music_enabled;
                    if (gc->music_enabled) { gc_playMusic(gc); goto hbms_end; }
                    gc_stopMusic(gc); goto hbms_end;
                case 6:
                    ++gc->music_volume_level;
                    if (gc->music_volume_level == 6) gc->music_volume_level = 1;
                    gc_setMusicVolume(gc, gc->music_volume_level); goto hbms_end;
            }
            gc_persistSnapshotMode0(gc);
            if (gc->ready) { gc->transitionCode = 1; gc_resetOverlayRenderState(gc); break; }
            gc->pendingTransitionCode = 1; gc_setState(gc, +GameState::Playing); break;
        case 18:
        case 19:
            if (gc->ready) { gc->transitionCode = 1; gc_resetOverlayRenderState(gc); break; }
            gc->pendingTransitionCode = 1; gc_setState(gc, +GameState::Playing); break;
        case 8:
        case 9:
            if (n2 == 1) { gc->pendingTransitionCode = gc->offerTypeTransitionCode; gc_setState(gc, +GameState::Playing); break; }
            if (n2 == 2) { gc->pendingTransitionCode = gc->offerEffectTransitionCode; gc_setState(gc, +GameState::Playing); break; }
            gc_setState(gc, +GameState::Playing); break;
        case 11: case 12: case 13: case 16:
            gc_setState(gc, +GameState::Playing); break;
        case 15:
            if (n2 == 1) gc->music_auto_start = true;
            gc_setState(gc, +GameState::LoadingResources); break;
        case 30:
            switch (n2) {
                case 1:
                    if (gc->economy.loanCeiling - 20000 < gc->economy.loanBalance) break;
                    gc->economy.loanBalance += 20000; gc->economy.money += 20000;
                    gc_incrementRowValueType14(gc, -20000);
                    break;
                case 2:
                    if (gc->economy.loanBalance < 20000 || gc->economy.money < 20000) break;
                    gc->economy.loanBalance -= 20000; gc->economy.money -= 20000;
                    gc_incrementRowValueType14(gc, 20000);
                    break;
                default:
                    gc_setState(gc, +GameState::Playing); gc->menuDirty = true;
            }
            gc->mapRedrawPending = true;
            break;
        case 31:
            gc->monthRolloverFlag = false;
            switch (n2) {
                case 0: gc->compareToPrevious = false; gc_archiveOrRecall(gc); gc->pendingTransitionCode = 32; gc_setState(gc, +GameState::Playing); goto hbms_end;
                case 1: gc->compareToPrevious = true; gc->pendingTransitionCode = 32; gc_setState(gc, +GameState::Playing); goto hbms_end;
                case 2: gc_setYearGraphMode(gc, 0, false); gc->pendingTransitionCode = 33; gc_setState(gc, +GameState::Playing); goto hbms_end;
                case 3: gc_setYearGraphMode(gc, 0, false); gc->pendingTransitionCode = 34; gc_setState(gc, +GameState::Playing); goto hbms_end;
                case 4: gc_setYearGraphMode(gc, 1, false); gc->pendingTransitionCode = 33; gc_setState(gc, +GameState::Playing); goto hbms_end;
                case 5: gc_setYearGraphMode(gc, 1, false); gc->pendingTransitionCode = 34; gc_setState(gc, +GameState::Playing); goto hbms_end;
                case 6: gc_setYearGraphMode(gc, 2, false); gc->pendingTransitionCode = 33; gc_setState(gc, +GameState::Playing); goto hbms_end;
                case 7: gc_setYearGraphMode(gc, 2, false); gc->pendingTransitionCode = 34; gc_setState(gc, +GameState::Playing); goto hbms_end;
                case 8: gc_setYearGraphMode(gc, 0, true); gc->pendingTransitionCode = 33; gc_setState(gc, +GameState::Playing); goto hbms_end;
                case 9: gc_setYearGraphMode(gc, 0, true); gc->pendingTransitionCode = 34; gc_setState(gc, +GameState::Playing); goto hbms_end;
                case 10: gc_setYearGraphMode(gc, 1, true); gc->pendingTransitionCode = 33; gc_setState(gc, +GameState::Playing); goto hbms_end;
                case 11: gc_setYearGraphMode(gc, 1, true); gc->pendingTransitionCode = 34; gc_setState(gc, +GameState::Playing); goto hbms_end;
                case 12: gc_setYearGraphMode(gc, 2, true); gc->pendingTransitionCode = 33; gc_setState(gc, +GameState::Playing); goto hbms_end;
                case 13: gc_setYearGraphMode(gc, 2, true); gc->pendingTransitionCode = 34; gc_setState(gc, +GameState::Playing); goto hbms_end;
                case 14: gc->compareToPrevious = false; gc_archiveOrRecall(gc); gc->pendingTransitionCode = 35; gc_setState(gc, +GameState::Playing); goto hbms_end;
                case 15: gc->compareToPrevious = true; gc->pendingTransitionCode = 35; gc_setState(gc, +GameState::Playing); goto hbms_end;
            }
            gc_setState(gc, +GameState::Playing); gc->menuDirty = true;
            break;
        case 32:
        case 35:
            if (!gc->monthRolloverFlag) { gc->pendingTransitionCode = 31; gc_setState(gc, +GameState::Playing); break; }
            gc_setState(gc, +GameState::Playing); gc->menuDirty = true; break;
        case 33:
        case 34:
            gc->pendingTransitionCode = 31; gc_setState(gc, +GameState::Playing); break;
        case 40:
            switch (n2) {
                case 1:
                    gc->pendingTransitionCode = gc->menuRowType[gc->offerRowIndex] + transition::OFFER_TYPE_BASE;
                    gc->duplicateRowState = 2; break;
                case 2:
                    gc->pendingTransitionCode = gc->menuRowAltType[gc->offerRowIndex] + transition::OFFER_ALT_BASE;
                    gc->duplicateRowState = 2; break;
            }
            gc_setState(gc, +GameState::Playing); break;
        case 41: case 42: case 44: case 45: case 46: case 47: case 48:
        case 49: case 50: case 51: case 52: case 53: case 54:
            if (gc->duplicateRowState == 1) gc->pendingTransitionCode = 60;
            if (gc->duplicateRowState == 2) gc->pendingTransitionCode = 40;
            if (gc->duplicateRowState == 3) gc->pendingTransitionCode = 9;
            if (gc->duplicateRowState == 4) gc->pendingTransitionCode = 8;
            gc_setState(gc, +GameState::Playing); break;
        case 60:
            if (n2 % 2 == 1) {
                gc->offerRowIndex = gc->enabledMenuRowIndexes[n2 / 2];
                gc->pendingTransitionCode = gc->menuRowType[gc->offerRowIndex] + transition::OFFER_TYPE_BASE;
            }
            if (n2 % 2 == 0) {
                gc->offerRowIndex = gc->enabledMenuRowIndexes[(n2 - 1) / 2];
                gc->pendingTransitionCode = gc->menuRowAltType[gc->offerRowIndex] + transition::OFFER_ALT_BASE;
            }
            gc->duplicateRowState = 1;
            gc_setState(gc, +GameState::Playing); break;
        case 61: case 62: case 63: case 64: case 65: case 66:
        case 67: case 68: case 69: case 70: case 71: case 72:
            if (gc->duplicateRowState == 1) gc->pendingTransitionCode = 60;
            if (gc->duplicateRowState == 2) gc->pendingTransitionCode = 40;
            if (gc->duplicateRowState == 3) gc->pendingTransitionCode = 9;
            if (gc->duplicateRowState == 4) gc->pendingTransitionCode = 8;
            gc_setState(gc, +GameState::Playing); break;
        case 28:
            if (n2 == 41) { gc->highScoreUploadRequested = false; gc_setState(gc, +GameState::HighScoreSync); break; }
            if (gc->ready) { gc->transitionCode = n2 == 40 ? 27 : 1; gc_resetOverlayRenderState(gc); break; }
            gc->pendingTransitionCode = n2 == 40 ? 27 : 1; gc_setState(gc, +GameState::Playing); break;
        case 27:
            if (n2 == 40) { gc->highScoreUploadRequested = true; gc_setState(gc, +GameState::HighScoreSync); break; }
            if (n2 == 0) gc->routeSlotIndex = (int8_t)(gc->routeSlotIndex - 1);
            if (n2 == 1) gc->routeSlotIndex = (int8_t)(gc->routeSlotIndex + 1);
            if (gc->routeSlotIndex < 0) gc->routeSlotIndex = 9;
            if (gc->routeSlotIndex == 10) gc->routeSlotIndex = 0;
            gc->spriteCycleCursor = gc->playerNameCodes[gc->routeSlotIndex];
            if (n2 == 2) {
                gc->spriteCycleCursor = (int8_t)(gc->spriteCycleCursor - 1);
                if (gc->spriteCycleCursor < 0) gc->spriteCycleCursor = 95;
                if (gc->spriteCycleCursor == 69) gc->spriteCycleCursor = 65;
                if (gc->spriteCycleCursor == 39) gc->spriteCycleCursor = 17;
                gc->playerNameCodes[gc->routeSlotIndex] = gc->spriteCycleCursor;
            }
            if (n2 == 3) {
                gc->spriteCycleCursor = (int8_t)(gc->spriteCycleCursor + 1);
                if (gc->spriteCycleCursor == 18) gc->spriteCycleCursor = 40;
                if (gc->spriteCycleCursor == 66) gc->spriteCycleCursor = 70;
                if (gc->spriteCycleCursor == 96) gc->spriteCycleCursor = 0;
                gc->playerNameCodes[gc->routeSlotIndex] = gc->spriteCycleCursor;
            }
            if (n2 >= 0) break;
            gc_persistSnapshotMode0(gc);
            if (gc->ready) { gc->transitionCode = 28; gc_resetOverlayRenderState(gc); break; }
            gc->pendingTransitionCode = 28; gc_setState(gc, +GameState::Playing); break;
    }
hbms_end:
    if (n > 80 && n < 100) gc_setState(gc, +GameState::Playing);
}
