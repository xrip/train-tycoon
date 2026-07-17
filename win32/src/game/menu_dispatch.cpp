// Build-menu action dispatch.
// Routes a (menu id, option) selection to state transitions / economy actions.
// Some actions use `goto hbms_end` to reach the common tail.
#include "game_internal.h"

void Game::handle_build_menu_state(int32_t n, int32_t n2) {
    Game *game = this;
    switch (n) {
        case 1:
            switch (n2) {
                case 1:
                    if (game->ready) { game->ui.transitionCode = 4; game->reset_overlay_render_state(); break; }
                    game->ui.pendingTransitionCode = 4; game->set_state(GameState::Playing); break;
                case 2:
                    game->ui.screenParam = 1;
                    if (game->ready) { game->ui.transitionCode = 14; game->reset_overlay_render_state(); break; }
                    game->ui.pendingTransitionCode = 14; game->set_state(GameState::Playing); break;
                case 3:
                    game->ui.screenParam = 0;
                    if (game->ready) { game->ui.transitionCode = 14; game->reset_overlay_render_state(); break; }
                    game->ui.pendingTransitionCode = 14; game->set_state(GameState::Playing); break;
                case 4:
                    if (game->ready) { game->ui.transitionCode = 10; game->reset_overlay_render_state(); break; }
                    game->ui.pendingTransitionCode = 10; game->set_state(GameState::Playing); break;
                case 5:
                    if (game->ready) { game->ui.transitionCode = 2; game->reset_overlay_render_state(); break; }
                    game->ui.pendingTransitionCode = 2; game->set_state(GameState::Playing); break;
                case 6:
                    if (game->ready) { game->ui.transitionCode = 3; game->reset_overlay_render_state(); break; }
                    game->ui.pendingTransitionCode = 3; game->set_state(GameState::Playing); break;
                case 7:
                    game->set_state(GameState::Playing); break;
                case 8:
                    if (game->ready) { game->ui.transitionCode = 5; game->reset_overlay_render_state(); break; }
                    game->ui.pendingTransitionCode = 5; game->set_state(GameState::Playing); break;
                case 9:
                    if (game->ready) { game->ui.transitionCode = 28; game->reset_overlay_render_state(); break; }
                    game->ui.pendingTransitionCode = 28; game->set_state(GameState::Playing); break;
                case 99:
                    game->ui.screenParam = 2;
                    if (game->ready) { game->ui.transitionCode = 14; game->reset_overlay_render_state(); break; }
                    game->ui.pendingTransitionCode = 14; game->set_state(GameState::Playing); break;
            }
            break;
        case 14:
            if (n2 == 2) {
                if (game->ui.screenParam == 4) { game->ui.pendingTransitionCode = 26; game->set_state(GameState::Playing); break; }
                if (game->ui.screenParam == 3) { game->set_state(GameState::LoadingResources); break; }
                if (game->ready) { game->ui.transitionCode = 1; game->reset_overlay_render_state(); break; }
                game->ui.pendingTransitionCode = 1; game->set_state(GameState::Playing); break;
            }
            switch (game->ui.screenParam) {
                case 0: game->set_state(GameState::SavingGame); break;
                case 1: game->set_state(GameState::LoadingGame); break;
                case 2: game->stopRequested = true; break;
                case 3: game->music.autoStart = true; game->music.enabled = true; game->set_state(GameState::LoadingResources); break;
                case 4: {
                    game->economy.money += game->build.currentTrainCostSnapshot * 70 / 100;
                    game->economy.aiMoneySpent -= game->build.currentTrainCostSnapshot * 70 / 100;
                    if (game->build.currentTrainCostSnapshot != 0)
                        game->record_money_change(
                            game->build.currentTrainCostSnapshot * 70 / 100);
                    int16_t s = game->fleet.trainCars[game->trainAtCursor].nextCar;
                    game->decrement_train_cargo_option_counts(s);
                    game->fleet.trainCars[game->trainAtCursor].flags = 0;
                    while (s != -1) { game->fleet.trainCars[s].flags = 0; s = game->fleet.trainCars[s].nextCar; }
                    game->set_state(GameState::Playing);
                    break;
                }
            }
            break;
        case 36:
            game->set_state(GameState::MapOverview); break;
        case 17:
            if (n2 == 1) game->ui.pendingTransitionCode = transition::SCENARIO_NOTICE_BASE + game->scenario.scenarioNoticeId;
            game->set_state(GameState::Playing); break;
        case 2:
        case 3:
            if (game->ready) { game->ui.transitionCode = 1; game->reset_overlay_render_state(); break; }
            game->ui.pendingTransitionCode = 1; game->set_state(GameState::Playing); break;
        case 6:
            game->set_state(GameState::Playing); break;
        case 7:
            game->ready = false; game->scenario.aiEnabled = true; game->ui.transitionCode = 1;
            game->reset_overlay_render_state(); game->set_state(GameState::MenuOverlay); break;
        case 5:
            if (n2 >= 2) { game->scenario.activeScenario = n2; game->set_state(GameState::GeneratingMap); break; }
            if (game->ready) { game->ui.transitionCode = 1; game->reset_overlay_render_state(); break; }
            game->ui.pendingTransitionCode = 1; game->set_state(GameState::Playing); break;
        case 4:
            switch (n2) {
                case 1:
                    if (game->scenario.scenarioMilestone == 0) { game->scenario.scenarioMilestone = 1; goto hbms_end; }
                    game->scenario.scenarioMilestone = 0; goto hbms_end;
                case 2:
                    if (game->scenario.difficultyA == 0) { game->scenario.difficultyA = 1; goto hbms_end; }
                    game->scenario.difficultyA = 0; goto hbms_end;
                case 3:
                    if (game->scenario.difficultySlot == 0) { game->scenario.difficultySlot = 1; goto hbms_end; }
                    game->scenario.difficultySlot = 0; goto hbms_end;
                case 4:
                    if (game->scenario.difficultyB == 0) { game->scenario.difficultyB = 1; goto hbms_end; }
                    game->scenario.difficultyB = 0; goto hbms_end;
                case 5:
                    if (game->scenario.difficultyC == 0) { game->scenario.difficultyC = 1; goto hbms_end; }
                    game->scenario.difficultyC = 0; goto hbms_end;
                case 6:
                    game->scenario.difficultyCycle = (int8_t)(game->scenario.difficultyCycle + 1);
                    if (game->scenario.difficultyCycle != 4) goto hbms_end;
                    game->scenario.difficultyCycle = 1; goto hbms_end;
                case 10:
                    game->scenario.activeScenario = game->scenario.scenarioMilestone; game->set_state(GameState::GeneratingMap); goto hbms_end;
            }
            if (game->ready) { game->ui.transitionCode = 1; game->reset_overlay_render_state(); break; }
            game->ui.pendingTransitionCode = 1; game->set_state(GameState::Playing); break;
        case 20:
            game->set_state(GameState::Playing); break;
        case 21:
            game->set_state(GameState::Playing); break;
        case 22:
            if (n2 == 1) {
                game->build.wagonType = 0;
                game->build.currentTrainCostSnapshot = 0;
                game->build.wagonDiscountType = game->build.wagonType;
                for (int n3 = 0; n3 < 5; ++n3) game->build.wagonOptionSlots[n3] = -1;
                for (int n3 = 0; n3 < 5; ++n3) game->build.wagonCountMatch[n3] = -1;
                game->scenario.difficultyPct = 100;
                game->build.discountApplied = false;
                game->build.compute_train_cost(game->economy, game->scenario);
                game->ui.pendingTransitionCode = 23; game->set_state(GameState::Playing); goto hbms_end;
            }
            game->set_state(GameState::Playing); break;
        case 23:
            if (n2 == 0) { game->ui.pendingTransitionCode = 24; game->set_state(GameState::Playing); break; }
            if (n2 == 1) { game->ui.pendingTransitionCode = 25; game->set_state(GameState::Playing); break; }
            if (n2 == 2) {
                if (!game->build.discountApplied) {
                    if (!game->buy_train_at_cursor()) break;
                    game->set_state(GameState::Playing); break;
                }
                if (!game->apply_train_upgrade(game->trainAtCursor, game->build.wagonType, game->build.wagonOptionSlots)) break;
                game->ui.pendingTransitionCode = 26; game->set_state(GameState::Playing); break;
            }
            game->ui.pendingTransitionCode = !game->build.discountApplied ? 22 : 26;
            game->set_state(GameState::Playing); break;
        case 24:
            if (n2 >= 0 && n2 < 6) { game->build.wagonType = n2; game->build.compute_train_cost(game->economy, game->scenario); }
            game->ui.pendingTransitionCode = 23; game->set_state(GameState::Playing); break;
        case 25:
            if (n2 == 3) game->add_selected_option(0);
            else if (n2 == 1) game->add_selected_option(1);
            else if (n2 == 0) game->add_selected_option(2);
            else if (n2 == 2) game->add_selected_option(3);
            else if (n2 == 9) game->pop_selected_option();
            game->build.compute_train_cost(game->economy, game->scenario);
            game->ui.pendingTransitionCode = 23; game->set_state(GameState::Playing); break;
        case 26:
            if (n2 == 1) {
                game->build.discountApplied = true;
                game->ui.pendingTransitionCode = 23; game->set_state(GameState::Playing);
            } else if (n2 == 2) {
                int n4 = game->build.baseTrainCostSnapshot - game->build.currentTrainCostSnapshot;
                if (n4 <= game->economy.money) {
                    game->fleet.trainCars[game->trainAtCursor].ageYears = 0;
                    game->economy.money -= n4;
                    game->record_money_change(-n4);
                    game->economy.aiMoneySpent += n4;
                    game->scenario.difficultyPct = 100 - 3 * game->fleet.trainCars[game->trainAtCursor].ageYears;
                    game->build.compute_train_cost(game->economy, game->scenario);
                    game->build.currentTrainCostSnapshot = game->build.unitCost;
                    game->ui.pendingTransitionCode = 26; game->set_state(GameState::Playing);
                } else {
                    game->enter_out_of_money_state();
                }
            } else if (n2 == 3) {
                game->ui.screenParam = 4;
                game->ui.pendingTransitionCode = 14; game->set_state(GameState::Playing);
            }
            if (n2 == 4) {
                game->fleet.trainCars[game->trainAtCursor].aiOverride = (int8_t)(game->fleet.trainCars[game->trainAtCursor].aiOverride == 0 ? 1 : 0);
                break;
            }
            game->set_state(GameState::Playing); break;
        case 10:
            switch (n2) {
                case 1: game->settings.animationsDisabled = !game->settings.animationsDisabled; goto hbms_end;
                case 2: game->settings.selectionHidden = !game->settings.selectionHidden; goto hbms_end;
                case 3: game->ui.monthlyReportEnabled = !game->ui.monthlyReportEnabled; goto hbms_end;
                case 4: game->ui.annualReportEnabled = !game->ui.annualReportEnabled; goto hbms_end;
                case 5:
                    game->music.enabled = !game->music.enabled;
                    if (game->music.enabled) { game->play_music(); goto hbms_end; }
                    game->stop_music(); goto hbms_end;
                case 6:
                    ++game->music.volumeLevel;
                    if (game->music.volumeLevel == 6) game->music.volumeLevel = 1;
                    game->set_music_volume(game->music.volumeLevel); goto hbms_end;
            }
            game->persist_settings_snapshot();
            if (game->ready) { game->ui.transitionCode = 1; game->reset_overlay_render_state(); break; }
            game->ui.pendingTransitionCode = 1; game->set_state(GameState::Playing); break;
        case 18:
        case 19:
            if (game->ready) { game->ui.transitionCode = 1; game->reset_overlay_render_state(); break; }
            game->ui.pendingTransitionCode = 1; game->set_state(GameState::Playing); break;
        case 8:
        case 9:
            if (n2 == 1) { game->ui.pendingTransitionCode = game->scenario.offerTypeTransitionCode; game->set_state(GameState::Playing); break; }
            if (n2 == 2) { game->ui.pendingTransitionCode = game->scenario.offerEffectTransitionCode; game->set_state(GameState::Playing); break; }
            game->set_state(GameState::Playing); break;
        case 11: case 12: case 13: case 16:
            game->set_state(GameState::Playing); break;
        case 15:
            if (n2 == 1) game->music.autoStart = true;
            game->set_state(GameState::LoadingResources); break;
        case 30:
            switch (n2) {
                case 1:
                    if (game->economy.loanCeiling - 20000 < game->economy.loanBalance) break;
                    game->economy.loanBalance += 20000; game->economy.money += 20000;
                    game->offerBoard.add_progress(14, -20000);
                    break;
                case 2:
                    if (game->economy.loanBalance < 20000 || game->economy.money < 20000) break;
                    game->economy.loanBalance -= 20000; game->economy.money -= 20000;
                    game->offerBoard.add_progress(14, 20000);
                    break;
                default:
                    game->set_state(GameState::Playing); game->ui.menuDirty = true;
            }
            game->renderer.mapRedrawPending = true;
            break;
        case 31:
            game->monthRolloverFlag = false;
            switch (n2) {
                case 0: game->compareToPrevious = false; game->economy.calculate_period_profit(); game->ui.pendingTransitionCode = 32; game->set_state(GameState::Playing); goto hbms_end;
                case 1: game->compareToPrevious = true; game->ui.pendingTransitionCode = 32; game->set_state(GameState::Playing); goto hbms_end;
                case 2: game->ui.set_year_graph_mode(game->economy, 0, false); game->ui.pendingTransitionCode = 33; game->set_state(GameState::Playing); goto hbms_end;
                case 3: game->ui.set_year_graph_mode(game->economy, 0, false); game->ui.pendingTransitionCode = 34; game->set_state(GameState::Playing); goto hbms_end;
                case 4: game->ui.set_year_graph_mode(game->economy, 1, false); game->ui.pendingTransitionCode = 33; game->set_state(GameState::Playing); goto hbms_end;
                case 5: game->ui.set_year_graph_mode(game->economy, 1, false); game->ui.pendingTransitionCode = 34; game->set_state(GameState::Playing); goto hbms_end;
                case 6: game->ui.set_year_graph_mode(game->economy, 2, false); game->ui.pendingTransitionCode = 33; game->set_state(GameState::Playing); goto hbms_end;
                case 7: game->ui.set_year_graph_mode(game->economy, 2, false); game->ui.pendingTransitionCode = 34; game->set_state(GameState::Playing); goto hbms_end;
                case 8: game->ui.set_year_graph_mode(game->economy, 0, true); game->ui.pendingTransitionCode = 33; game->set_state(GameState::Playing); goto hbms_end;
                case 9: game->ui.set_year_graph_mode(game->economy, 0, true); game->ui.pendingTransitionCode = 34; game->set_state(GameState::Playing); goto hbms_end;
                case 10: game->ui.set_year_graph_mode(game->economy, 1, true); game->ui.pendingTransitionCode = 33; game->set_state(GameState::Playing); goto hbms_end;
                case 11: game->ui.set_year_graph_mode(game->economy, 1, true); game->ui.pendingTransitionCode = 34; game->set_state(GameState::Playing); goto hbms_end;
                case 12: game->ui.set_year_graph_mode(game->economy, 2, true); game->ui.pendingTransitionCode = 33; game->set_state(GameState::Playing); goto hbms_end;
                case 13: game->ui.set_year_graph_mode(game->economy, 2, true); game->ui.pendingTransitionCode = 34; game->set_state(GameState::Playing); goto hbms_end;
                case 14: game->compareToPrevious = false; game->economy.calculate_period_profit(); game->ui.pendingTransitionCode = 35; game->set_state(GameState::Playing); goto hbms_end;
                case 15: game->compareToPrevious = true; game->ui.pendingTransitionCode = 35; game->set_state(GameState::Playing); goto hbms_end;
            }
            game->set_state(GameState::Playing); game->ui.menuDirty = true;
            break;
        case 32:
        case 35:
            if (!game->monthRolloverFlag) { game->ui.pendingTransitionCode = 31; game->set_state(GameState::Playing); break; }
            game->set_state(GameState::Playing); game->ui.menuDirty = true; break;
        case 33:
        case 34:
            game->ui.pendingTransitionCode = 31; game->set_state(GameState::Playing); break;
        case 40:
            switch (n2) {
                case 1:
                    game->ui.pendingTransitionCode = game->offerBoard.offers[game->scenario.offerRowIndex].objectiveType + transition::OFFER_TYPE_BASE;
                    game->scenario.duplicateRowState = 2; break;
                case 2:
                    game->ui.pendingTransitionCode = game->offerBoard.offers[game->scenario.offerRowIndex].effectType + transition::OFFER_ALT_BASE;
                    game->scenario.duplicateRowState = 2; break;
            }
            game->set_state(GameState::Playing); break;
        case 41: case 42: case 44: case 45: case 46: case 47: case 48:
        case 49: case 50: case 51: case 52: case 53: case 54:
            if (game->scenario.duplicateRowState == 1) game->ui.pendingTransitionCode = 60;
            if (game->scenario.duplicateRowState == 2) game->ui.pendingTransitionCode = 40;
            if (game->scenario.duplicateRowState == 3) game->ui.pendingTransitionCode = 9;
            if (game->scenario.duplicateRowState == 4) game->ui.pendingTransitionCode = 8;
            game->set_state(GameState::Playing); break;
        case 60:
            if (n2 % 2 == 1) {
                game->scenario.offerRowIndex = game->ui.enabledMenuRowIndexes[n2 / 2];
                game->ui.pendingTransitionCode = game->offerBoard.offers[game->scenario.offerRowIndex].objectiveType + transition::OFFER_TYPE_BASE;
            }
            if (n2 % 2 == 0) {
                game->scenario.offerRowIndex = game->ui.enabledMenuRowIndexes[(n2 - 1) / 2];
                game->ui.pendingTransitionCode = game->offerBoard.offers[game->scenario.offerRowIndex].effectType + transition::OFFER_ALT_BASE;
            }
            game->scenario.duplicateRowState = 1;
            game->set_state(GameState::Playing); break;
        case 61: case 62: case 63: case 64: case 65: case 66:
        case 67: case 68: case 69: case 70: case 71: case 72:
            if (game->scenario.duplicateRowState == 1) game->ui.pendingTransitionCode = 60;
            if (game->scenario.duplicateRowState == 2) game->ui.pendingTransitionCode = 40;
            if (game->scenario.duplicateRowState == 3) game->ui.pendingTransitionCode = 9;
            if (game->scenario.duplicateRowState == 4) game->ui.pendingTransitionCode = 8;
            game->set_state(GameState::Playing); break;
        case 28:
            if (n2 == 41) { game->scenario.highScoreUploadRequested = false; game->set_state(GameState::HighScoreSync); break; }
            if (game->ready) { game->ui.transitionCode = n2 == 40 ? 27 : 1; game->reset_overlay_render_state(); break; }
            game->ui.pendingTransitionCode = n2 == 40 ? 27 : 1; game->set_state(GameState::Playing); break;
        case 27:
            if (n2 == 40) { game->scenario.highScoreUploadRequested = true; game->set_state(GameState::HighScoreSync); break; }
            if (n2 == 0) game->scenario.playerNameCursor = (int8_t)(game->scenario.playerNameCursor - 1);
            if (n2 == 1) game->scenario.playerNameCursor = (int8_t)(game->scenario.playerNameCursor + 1);
            if (game->scenario.playerNameCursor < 0) game->scenario.playerNameCursor = 9;
            if (game->scenario.playerNameCursor == 10) game->scenario.playerNameCursor = 0;
            game->assets.spriteCycleCursor = game->scenario.playerNameCodes[game->scenario.playerNameCursor];
            if (n2 == 2) {
                game->assets.spriteCycleCursor = (int8_t)(game->assets.spriteCycleCursor - 1);
                if (game->assets.spriteCycleCursor < 0) game->assets.spriteCycleCursor = 95;
                if (game->assets.spriteCycleCursor == 69) game->assets.spriteCycleCursor = 65;
                if (game->assets.spriteCycleCursor == 39) game->assets.spriteCycleCursor = 17;
                game->scenario.playerNameCodes[game->scenario.playerNameCursor] = game->assets.spriteCycleCursor;
            }
            if (n2 == 3) {
                game->assets.spriteCycleCursor = (int8_t)(game->assets.spriteCycleCursor + 1);
                if (game->assets.spriteCycleCursor == 18) game->assets.spriteCycleCursor = 40;
                if (game->assets.spriteCycleCursor == 66) game->assets.spriteCycleCursor = 70;
                if (game->assets.spriteCycleCursor == 96) game->assets.spriteCycleCursor = 0;
                game->scenario.playerNameCodes[game->scenario.playerNameCursor] = game->assets.spriteCycleCursor;
            }
            if (n2 >= 0) break;
            game->persist_settings_snapshot();
            if (game->ready) { game->ui.transitionCode = 28; game->reset_overlay_render_state(); break; }
            game->ui.pendingTransitionCode = 28; game->set_state(GameState::Playing); break;
    }
hbms_end:
    if (n > 80 && n < 100) game->set_state(GameState::Playing);
}
