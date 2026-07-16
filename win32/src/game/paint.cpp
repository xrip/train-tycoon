// Game paint and update state machines.
#include "game_methods.h"
#include <cstdlib>

static int read_u16_le(ResourceReader& resource) {
    int low = resource.read_byte();
    int high = resource.read_byte();
    return low + high * 256;
}

static int read_u24_le(ResourceReader& resource) {
    int low = resource.read_byte();
    int middle = resource.read_byte();
    int high = resource.read_byte();
    return low + middle * 256 + high * 65536;
}

#define RGB3(r, g, b) (((r) << 16) | ((g) << 8) | (b))
#define WIDTH  MIDP_SCREEN_W   // this.getWidth()
#define HEIGHT MIDP_SCREEN_H   // this.getHeight()

// public void paint(Graphics graphics)
void gc_paint(GameCanvas *gc, Surface *g) {
    // setFullScreenMode(true) is a no-op on this backend.
    int n = WIDTH, n2 = HEIGHT;
    if (gc->ce) return;
    gc_setCanvasSize(gc, n, n2);
    if (gc->gameState != +GameState::MapOverview && gc->gameState != +GameState::BuildingMinimap && gc->gameState != +GameState::BuildingPath
        && gc->gameState != +GameState::SavingGame && gc->gameState != +GameState::LoadingGame && gc->gameState != +GameState::AiTurn
        && gc->gameState != +GameState::HighScoreSync && (gc->gameState != +GameState::MenuOverlay || gc->ready)) {
        midp_set_color(g, 0);
        midp_fill_rect(g, 0, 0, n, n2);
    }
    midp_set_clip(g, 0, 0, gcs_screenWidth, gcs_hs);
    switch (gc->gameState) {
        case +GameState::BootSplash0: case +GameState::BootSplash1: case +GameState::BootSplash2: case +GameState::BootSplash3: {
            midp_set_color(g, 0xFFFFFF);
            if (gc->gameState == +GameState::BootSplash0) midp_set_color(g, 0);
            midp_fill_rect(g, 0, 0, gcs_screenWidth, gcs_hs);
            int n3 = gcs_screenWidth / 2;
            int n4 = gcs_hs / 2;
            int n5 = gc->cycle_flag->width() / 2;
            int n6 = gc->cycle_flag->height();
            int n7 = n6 / 10 + 1;
            for (int i = 0; i < 10; ++i) {
                int n8 = (n3 + n5 + 30) * (gc->loadingTimer - 5 * (i / 2)) / 10;
                if (n8 < 0) n8 = 0;
                if (n8 > n3 + n5 + 30) n8 = n3 + n5 + 30;
                int n9 = n3 - (n3 + n5 + 30 - n8);
                if (i % 2 == 1) n9 = n3 + (n3 + n5 + 30 - n8);
                midp_set_clip(g, n9 - n5, n4 - n6 / 2 + i * n7, n5 * 2, n7);
                midp_draw_image(g, gc->cycle_flag.get(), n9, n4, 3);
            }
            midp_set_clip(g, 0, 0, gcs_screenWidth, gcs_hs);
            break;
        }
        case +GameState::LoadingResources: {
            midp_set_color(g, 0xFFFFFF);
            midp_fill_rect(g, 0, 0, gcs_screenWidth, gcs_hs);
            midp_draw_image(g, gc->cycle_flag.get(), gcs_screenWidth / 2, gcs_hs / 2, 3);
            midp_set_color(g, RGB3(215, 222, 238));
            midp_fill_rect(g, 10, gcs_hs - 13, gcs_screenWidth - 21, 9);
            int n10 = gc->spriteLoadCounter * (gcs_screenWidth - 20) / (gc->scenarioResourceCount - 1);
            midp_set_color(g, RGB3(60, 105, 144));
            midp_fill_rect(g, 10, gcs_hs - 13, n10, 9);
            midp_set_color(g, RGB3(49, 64, 90));
            midp_draw_rect(g, 10, gcs_hs - 13, gcs_screenWidth - 21, 9);
            break;
        }
        case +GameState::MenuOverlay: {
            if (gc->ready) {
                midp_set_color(g, 0xFFFFFF);
                midp_fill_rect(g, 0, 0, gcs_screenWidth, gcs_hs);
                if (gc->cycle_flag != NULL)
                    midp_draw_image(g, gc->cycle_flag.get(), gcs_screenWidth / 2, gcs_hs / 2, 3);
            }
            if (gc->transitionCode == 36) {
                gc->gameState = +GameState::MapOverview;
                gc_paint(gc, g);
                gc->gameState = +GameState::MenuOverlay;
            }
            gc_renderPackedSprite(gc, g, gc->transitionCode);
            break;
        }
        case +GameState::GeneratingMap: {
            midp_set_color(g, 0);
            midp_fill_rect(g, 0, 0, gcs_screenWidth, gcs_hs);
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc_tracked_codes(gc->spriteBitmapTable[gc->spriteIndexTable[0]]));
            midp_set_color(g, RGB3(215, 222, 238));
            midp_fill_rect(g, 10, gcs_hs - 23, gcs_screenWidth - 21, 9);
            int n11 = gc->totalTilesToGenerate < 2 ? 2 : gc->totalTilesToGenerate;
            int n12 = gc->mapGenTilesDone * (gcs_screenWidth - 20) / (n11 - 1);
            midp_set_color(g, RGB3(60, 105, 144));
            midp_fill_rect(g, 10, gcs_hs - 23, n12, 9);
            midp_set_color(g, 0xFFFFFF);
            midp_draw_rect(g, 10, gcs_hs - 23, gcs_screenWidth - 21, 9);
            break;
        }
        case +GameState::Playing: {
            gc->tickStart = midp_current_time_millis();
            gc_renderMap(gc, g);
            gc->tickEnd = midp_current_time_millis();
            if (gc->inputPending) {
                int n13;
                midp_set_color(g, 0);
                midp_set_clip(g, 0, 15, gcs_screenWidth, gcs_bottomBarY - 15);
                for (n13 = 0; n13 < gcs_screenWidth; ++n13)
                    midp_draw_line(g, n13 * 2, 0, gcs_hs + n13 * 2, gcs_hs);
                for (n13 = 0; n13 < gcs_hs; ++n13)
                    midp_draw_line(g, 0, n13 * 2, gcs_screenWidth, gcs_screenWidth + n13 * 2);
                midp_set_clip(g, 0, 0, gcs_screenWidth, gcs_hs);
                gc_drawSpriteQuad(gc, g, 234, 0, 15, 20);
            }
            gc_drawMoneyTicker(gc, g);
            if (gc->pauseFlag) gc_drawSpriteQuad(gc, g, 236, 0, 15, 0);
            if (gc->pendingTransitionCode > -1) {
                gc->transitionCode = gc->pendingTransitionCode;
                gc_resetOverlayRenderState(gc);
                gc->pendingTransitionCode = -1;
                gc_renderPackedSprite(gc, g, gc->transitionCode);
                gc_setState(gc, +GameState::MenuOverlay);
            }
            if (gc->transitionDebounce <= 0) break;
            gc->transitionDebounce = (int8_t)(gc->transitionDebounce - 1);
            if (gc->transitionDebounce != 0) break;
            gc->transitionCode = transition::SCENARIO_NOTICE_BASE + gc->scenarioNoticeCode;
            gc_resetOverlayRenderState(gc);
            gc->ready = false;
            gc_renderPackedSprite(gc, g, gc->transitionCode);
            gc_setState(gc, +GameState::MenuOverlay);
            break;
        }
        case +GameState::BuildingMinimap:
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc_tracked_codes(gc->spriteBitmapTable[gc->spriteIndexTable[50]]));
            break;
        case +GameState::MapOverview: {
            int n14;
            midp_set_clip(g, 0, 15, gcs_screenWidth, gcs_gameplayViewportHeight);
            midp_set_color(g, 0);
            midp_fill_rect(g, 0, 15, gcs_screenWidth, gcs_gameplayViewportHeight);
            int n15 = gc->viewportCenterX / 16;
            int n16 = gc->viewportCenterY / 8;
            int n17 = gc->world.mapWidth + gc->world.mapHeight;
            int n18 = gcs_screenWidth / 2 - n15 - gcs_screenWidth / 32;
            if (gcs_screenWidth - n18 > n17) n18 = gcs_screenWidth - n17;
            if (n18 > 0) n18 = 0;
            if (gcs_gameplayViewportHeight - (n14 = gcs_gameplayViewportHeight / 2 - n16 - gcs_gameplayViewportHeight / 16) > n17) n14 = gcs_gameplayViewportHeight - n17;
            if (n14 > 0) n14 = 0;
            int n19 = n17 < gcs_screenWidth ? (gcs_screenWidth - n17) / 2 : 0;
            int n20 = n17 < gcs_gameplayViewportHeight ? (gcs_gameplayViewportHeight - n17) / 2 : 0;
            midp_draw_image(g, gc->backbuffer.get(), n18 + n19, 15 + n14 + n20, 20);
            int n21 = gc->viewportCenterX - 2 * gc->viewportCenterY + 16 + 2 * gc->world.mapHalfWidth;
            int n22 = gc->viewportCenterX + 2 * gc->viewportCenterY - 16 - 2 * gc->world.mapHalfWidth;
            int n23 = gc->world.mapWidth - (n21 /= 32) + (n22 /= 32);
            int n24 = n21 + n22;
            midp_set_color(g, 0xFFFFFF);
            midp_draw_rect(g, n24 + n18 + n19, 15 + n23 + n14 + n20, gcs_screenWidth / 16, gcs_gameplayViewportHeight / 8);
            midp_set_clip(g, 0, 0, gcs_screenWidth, gcs_hs);
            gc_drawConfirmBar(gc, g);
            gc_drawConfirmDialog(gc, g);
            if (gc->ee) {
                gc->ready = false;
                gc->transitionCode = 36;
                gc_setState(gc, +GameState::MenuOverlay);
                gc->ee = false;
            }
            if (gc->pendingTransitionCode <= -1) break;
            gc->transitionCode = gc->pendingTransitionCode;
            gc_resetOverlayRenderState(gc);
            gc->pendingTransitionCode = -1;
            gc_renderPackedSprite(gc, g, gc->transitionCode);
            gc_setState(gc, +GameState::MenuOverlay);
            break;
        }
        case +GameState::BuildingPath:
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc_tracked_codes(gc->spriteBitmapTable[gc->spriteIndexTable[51]]));
            break;
        case +GameState::SavingGame:
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc_tracked_codes(gc->spriteBitmapTable[gc->spriteIndexTable[52]]));
            break;
        case +GameState::LoadingGame:
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc_tracked_codes(gc->spriteBitmapTable[gc->spriteIndexTable[53]]));
            break;
        case +GameState::AiTurn:
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc_tracked_codes(gc->spriteBitmapTable[gc->spriteIndexTable[49]]));
            break;
        case +GameState::HighScoreSync:
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc_tracked_codes(gc->spriteBitmapTable[gc->spriteIndexTable[66]]));
            break;
    }
}

// public void update()
void gc_update(GameCanvas *gc) {
    switch (gc->gameState) {
        case +GameState::BootSplash0: case +GameState::BootSplash1: case +GameState::BootSplash2: {
            if (midp_current_time_millis() - gc->loadingStartTime > 5000L)
                gc->loadingTimeout = true;
            ++gc->loadingTimer;
            if (!gc->loadingTimeout) break;
            if (gc->gameState == +GameState::BootSplash1) {
                for (int i = 0; i < 6; ++i) gc->font_images[i] = gc_decodePngFromSpriteStream(gc);
                gc_setState(gc, +GameState::IntroReady);
                break;
            }
            gc_setState(gc, gc->gameState + 1);
            break;
        }
        case +GameState::IntroReady:
            gc->ready = true;
            gc->screenParam = 3;
            gc->transitionCode = 14;
            gc_resetOverlayRenderState(gc);
            gc_setState(gc, +GameState::MenuOverlay);
            break;
        case +GameState::LoadingResources:
            gc->tile_sprites[gc->spriteLoadCounter] = gc_decodePngFromSpriteStream(gc);
            ++gc->spriteLoadCounter;
            if (gc->spriteLoadCounter == gc->scenarioResourceCount / 2) {
                gc_loadMusic(gc);
                if (gc->music_auto_start) { gc_playMusic(gc); gc->music_enabled = true; }
                else gc->music_enabled = false;
            }
            if (gc->spriteLoadCounter != gc->scenarioResourceCount) break;
            gc->ready = true;
            gc->transitionCode = 1;
            gc_setState(gc, +GameState::MenuOverlay);
            break;
        case +GameState::GeneratingMap: {
            ++gc->loadStep;
            if (gc->loadStep == 2) {
                gc_initSaveOrLoad(gc, save::OP_RESET_SKIP);
                gc->dayOfMonth = 1; gc->monthOfYear = 1; gc->year = 1950;
                gc->milestonesEnabled = false; gc->pauseFlag = false; gc->aiEnabled = false;
                gc->moneyBaseA = 974; gc->moneyBaseB = 974;
                gc->baseTrainSpeed = 173; gc->accel1 = 342; gc->brake1 = 311;
                gc->baseTrainSpeed2 = 173; gc->accel2 = 234;
                gc->costPower = 4759; gc->costStruct = 9845; gc->costOverhead = 347;
                gc->costMaintenance = 5982; gc->costCrew = 481;
                gc->gWagonCost = 37894; gc->gWagonCost2 = 43247; gc->gWagonCost3 = 51872;
                gc->gWagonCost4 = 60149; gc->gWagonCost5 = 71423; gc->gWagonCost6 = 82675;
                gc->costGroupC1 = 4578; gc->costGroupC2 = 5196; gc->costGroupC3 = 4314; gc->costGroupC4 = 5072;
                ResourceReader in = resources::open("scenarios.bin");
                if (in) {
                    const int record_count = in.read_byte();
                    for (int record_index = 0; record_index < record_count;
                         ++record_index) {
                        const int scenario_index = in.read_byte();
                        const int map_size = in.read_byte();
                        const int terrain_raise_steps = in.read_byte();
                        const int terrain_lower_steps = in.read_byte();
                        const int obstacle_count = read_u16_le(in);
                        const int terrain_patch_size = in.read_byte();
                        const int map_gen_columns = in.read_byte();
                        const int map_gen_rows = in.read_byte();
                        const int stage7_feature_count = in.read_byte();
                        const int stage6_feature_count = in.read_byte();
                        const int weight_accel_1 = in.read_byte();
                        const int weight_accel_2 = in.read_byte();
                        const int weight_accel_3 = in.read_byte();
                        const int weight_accel_4 = in.read_byte();
                        const int starting_money = read_u24_le(in);
                        const int loan_ceiling = read_u24_le(in);
                        const int starting_loan = read_u24_le(in);
                        const int interest_rate = in.read_byte();
                        const int era_number = in.read_byte();
                        const int notice_id = in.read_byte();
                        const int milestone_index = in.read_byte();
                        const int milestone_score = read_u24_le(in);
                        const int milestone_threshold = read_u24_le(in);
                        const int build_menu_scenario = in.read_byte();
                        const int scenario_flags = in.read_byte();
                        if (scenario_index != gc->activeScenario) continue;
                        gc->scenarioStep = scenario_index;
                        gc->world.mapWidth = gc->world.mapHeight = map_size;
                        gc->world.mapHalfWidth = (gc->world.mapWidth - 1) * 8;
                        gc->world.mapScreenWidth = (gc->world.mapWidth + gc->world.mapHeight) * 16;
                        gc->world.mapScreenHeight = (gc->world.mapWidth + gc->world.mapHeight) * 8;
                        gc->viewportHalfWidth = gc->world.mapScreenWidth / 2;
                        gc->viewportHalfHeight = gc->world.mapScreenHeight / 2;
                        gc->viewportCenterX = gc->viewportHalfWidth;
                        gc->viewportCenterY = gc->viewportHalfHeight;
                        gc->viewportBaseX = gc->viewportCenterX;
                        gc->viewportBaseY = gc->viewportCenterY;
                        gc->mapScaleX = gc->viewportBaseX * 2;
                        gc->mapScaleY = gc->viewportBaseY * 2;
                        gc->initialObstacleCount = obstacle_count; gc->terrainRaiseStepCount = terrain_raise_steps; gc->terrainLowerStepCount = terrain_lower_steps; gc->terrainPatchBaseSize = terrain_patch_size;
                        gc->mapGenColumns = map_gen_columns; gc->mapGenRows = map_gen_rows;
                        gc->terrainFeatureStage7Count = stage7_feature_count; gc->terrainFeatureStage6Count = stage6_feature_count;
                        gc->weightAccel = weight_accel_1; gc->weightAccel2 = weight_accel_2; gc->weightAccel3 = weight_accel_3; gc->weightAccel4 = weight_accel_4;
                        gc->money = starting_money; gc->loanCeiling = loan_ceiling; gc->loanBalance = starting_loan;
                        gc->interestRate = (int8_t)interest_rate; gc->era = era_number - 1;
                        gc->scenarioMilestoneIndex = (int8_t)milestone_index;
                        gc->currentMilestoneScore = milestone_score; gc->milestoneCoThreshold2 = milestone_threshold;
                        gc_initBuildMenuForScenario(gc, build_menu_scenario);
                        gc->flagsA = (scenario_flags & 2) == 2;
                        gc->flagsB = (scenario_flags & 4) == 4;
                        if ((scenario_flags & 1) != 1) { gc->aiFrequency = 25; gc->aiThreshold = 100; }
                        else { gc->aiFrequency = 32; gc->aiThreshold = 29; }
                        gc->scenarioNoticeId = notice_id;
                        if (notice_id != 0) { gc->transitionDebounce = 5; gc->scenarioNoticeCode = (int8_t)notice_id; continue; }
                        gc->transitionDebounce = 0;
                    }
                }
                if (gc->activeScenario <= 1) {
                    if (gc->difficultySlot == 0) { gc->aiFrequency = 32; gc->aiThreshold = 29; }
                    if (gc->difficultySlot == 1) { gc->aiFrequency = 25; gc->aiThreshold = 22; }
                    if (gc->difficultyA == 0) { gc->interestRate = 4; gc->enterpriseTaxPct = 2; }
                    if (gc->difficultyA == 1) { gc->interestRate = 8; gc->enterpriseTaxPct = 5; }
                    if (gc->difficultyB == 0) { gc->mapGenColumns /= 2; gc->mapGenRows /= 2; }
                    if (gc->difficultyC == 0) {
                        gc->terrainFeatureStage7Count /= 2; gc->terrainFeatureStage6Count /= 2;
                        gc->weightAccel /= 2; gc->weightAccel2 /= 2; gc->weightAccel3 /= 2; gc->weightAccel4 /= 2;
                    }
                }
            }
            if (gc->loadStep == 3) {
                gc->buildMenuItemIndex = 0;
                gc->buildMenuMode = 0;
                gc_loadState25Setup(gc, 0);
            }
            if (gc->loadStep <= 3) break;
            gc_loadState25Setup(gc, 1);
            if (gc->mapGenTilesDone != gc->totalTilesToGenerate) break;
            gc->mapGenTilesDone = 0;
            gc_setState(gc, +GameState::Playing);
            break;
        }
        case +GameState::Playing: {
            gc->tickStart = midp_current_time_millis();
            if (++gc->timerA1 == 2) {
                gc->timerA1 = 0;
                if (++gc->timerA2 == 3) gc->timerA2 = 0;
            }
            if (++gc->timerB1 == 2) {
                gc->timerB1 = 0;
                if (++gc->timerB2 == 3) gc->timerB2 = 0;
            }
            if (++gc->timerC1 == 3) {
                gc->timerC1 = 0;
                if (++gc->timerC2 == 3) gc->timerC2 = 0;
            }
            if (++gc->woodAgeTimer == 150) {
                for (int n27 = 0; n27 < gc->world.mapWidth; ++n27) {
                    for (int n = 0; n < gc->world.mapHeight; ++n) {
                        if (gc->world.tileMap[n27][n] < 59 || gc->world.tileMap[n27][n] > 61
                            || gc->rng.next_int() % 3 != 0)
                            continue;
                        ++gc->world.tileMap[n27][n];
                        if (gc->world.tileMap[n27][n] == 62) gc->world.tileMap[n27][n] = 59;
                    }
                }
                gc->woodAgeTimer = 0;
            }
            if (++gc->phaseTimer == 10) gc->phaseTimer = 0;

            for (int n27 = 0; n27 < 50; ++n27) {
                if (gc->tileQueue[n27][0] <= 0) continue;
                --gc->tileQueue[n27][0];
                int n = gc_byteToUnsignedInt(gc, gc->tileQueue[n27][1]);
                int n29 = gc_byteToUnsignedInt(gc, gc->tileQueue[n27][2]);
                if (gc->tileQueue[n27][0] == 0 && n >= 0 && n29 >= 0
                    && n < gc->world.mapWidth && n29 < gc->world.mapHeight)
                    gc->world.tileMap[n][n29] = (int8_t)(gc_byteToUnsignedInt(gc, gc->world.tileMap[n][n29]) - 128);
                if (gc->tileQueue[n27][0] != 7 || n < 0 || n29 < 0
                    || n >= gc->world.mapWidth || n29 >= gc->world.mapHeight)
                    continue;
                int n30 = gc_byteToUnsignedInt(gc, gc->world.tileMap[n][n29]) - 128;
                if (gc->tileQueueVisualOnly[n27]) continue;
                if (n30 >= 29 && n30 <= 33) gc->world.tileMap[n][n29] = -128;
                if ((n30 >= 70 && n30 <= 75) || (n30 >= 87 && n30 <= 90)) {
                    gc->world.trackMap[n][n29] = 0;
                    gc->world.tileMap[n][n29] = -128;
                    gc->lastViewportCenterX = -1;
                }
                if (n30 >= 77 && n30 <= 80) {
                    gc->world.tileMap[n][n29] = (int8_t)(128 + n30 - 56);
                    gc->world.trackMap[n][n29] = 0;
                    gc->lastViewportCenterX = -1;
                }
                if (n30 == 81 || n30 == 82) {
                    gc->world.tileMap[n][n29] = -128;
                    gc->world.trackMap[n][n29] = 0;
                    gc->lastViewportCenterX = -1;
                }
                if (n30 >= 83 && n30 <= 86) {
                    gc->world.tileMap[n][n29] = (int8_t)(128 + n30 - 76);
                    gc->world.trackMap[n][n29] = 0;
                    gc->lastViewportCenterX = -1;
                }
                if (n30 == 76) {
                    int n31 = gc->world.trackMap[n][n29] & 7;
                    int n32 = (gc->world.trackMap[n][n29] & 0x3F) >> 3;
                    if (n31 == 1 && n32 == 1) {
                        gc->world.tileMap[n][n29] = -128;
                        gc->world.trackMap[n][n29] = 0;
                    } else if (n31 == 1) {
                        gc->world.tileMap[n][n29] = -57;
                        gc->world.trackMap[n][n29] = (int8_t)(0x40 | n32);
                    } else if (n32 == 1) {
                        gc->world.tileMap[n][n29] = -58;
                        gc->world.trackMap[n][n29] = (int8_t)(0x40 | n31);
                    }
                    gc->lastViewportCenterX = -1;
                }
            }

            for (int n27 = 0; n27 < 500; ++n27) {
                if ((gc->trainFlags[n27] & 0x80) != 128
                    || (gc->trainFlags[n27] & 0x40) != 64)
                    continue;
                if ((gc->trainFlags[n27] & 0x20) == 32) {
                    if (gc->trainEnergyOrCount[n27] > 0) {
                        --gc->trainEnergyOrCount[n27];
                        continue;
                    }
                    gc->trainPhase[n27] = 0;
                    gc->trainFlags[n27] = 0;
                    continue;
                }
                gc_updateTrain(gc, n27);
                gc_updateTrain(gc, n27);
                if (gc->trainPhase[n27] == 0) gc_mergeTrainAt(gc, n27);
            }

            if (gc->counterA >= gc->dH) {
                gc_computeYearEndMetrics(gc);
                if (gc->frameIndex > 0) --gc->frameIndex;
                gc->counterA = 0;
                ++gc->dayOfMonth;
                gc_everyMonthlyTick(gc);
                if (gc->dayOfMonth % 4 == 0) gc_maybeAiTakeTurn(gc);
                if (gc->dayOfMonth % 2 == 0) {
                    if (gc->dayOfMonth % 10 == 0) gc_everyTenTick(gc);
                    gc_everyTwoTick(gc);
                    gc_everyTwoTickZ(gc);
                    if (gc->flagsA) {
                        int found = 0;
                        for (int n = 0; n < 30; ++n)
                            found = found || gc_pickValidForestTile(gc);
                        if (found) gc_maybeAITick(gc);
                    }
                    gc_tickInputOrAi(gc, false);
                }
                int oldMonth = gc->monthOfYear;
                switch (gc->monthOfYear) {
                    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
                        if (gc->dayOfMonth == 32) ++gc->monthOfYear;
                        break;
                    case 2:
                        if ((gc->year % 4 != 0 && gc->dayOfMonth == 29)
                            || (gc->year % 4 == 0 && gc->dayOfMonth == 30))
                            ++gc->monthOfYear;
                        break;
                    case 4: case 6: case 9: case 11:
                        if (gc->dayOfMonth == 31) ++gc->monthOfYear;
                        break;
                }
                if (oldMonth != gc->monthOfYear) {
                    gc->dayOfMonth = 1;
                    gc_newYearNotify(gc);
                    gc_dayResetP(gc);
                    gc_dayResetAg(gc);
                    gc_dayResetAd(gc);
                    switch (std::abs(gc->rng.next_int() % 7)) {
                        case 0: gc->moneyBaseA += 9; gc->moneyBaseB += 9; break;
                        case 1: ++gc->baseTrainSpeed; gc->accel1 += 3; gc->brake1 += 3; break;
                        case 2: ++gc->baseTrainSpeed2; gc->accel2 += 2; break;
                        case 3: gc->costPower += 47; gc->costStruct += 98; break;
                        case 4:
                            gc->costOverhead += 3; gc->costMaintenance += 59; gc->costCrew += 4;
                            break;
                        case 5:
                            gc->gWagonCost += 378; gc->gWagonCost2 += 432;
                            gc->gWagonCost3 += 518; gc->gWagonCost4 += 601;
                            gc->gWagonCost5 += 714; gc->gWagonCost6 += 826;
                            break;
                        case 6:
                            gc->costGroupC1 += 45; gc->costGroupC2 += 51;
                            gc->costGroupC3 += 43; gc->costGroupC4 += 50;
                            break;
                    }
                }
                if (gc->monthOfYear == 13) {
                    gc_revenueOrYear(gc);
                    gc_postRevenueReset(gc);
                    gc->monthOfYear = 1;
                    ++gc->year;
                    if (gc->money < 0) {
                        gc->ready = false;
                        gc->transitionCode = 7;
                        gc_setState(gc, +GameState::MenuOverlay);
                    }
                }
                int oldEra = gc->era;
                if (gc->dayOfMonth == 5 && gc->monthOfYear == 3 && gc->year == 1951 && gc->era < 2)
                    gc->era = 2;
                if (gc->dayOfMonth == 17 && gc->monthOfYear == 7 && gc->year == 1953 && gc->era < 3)
                    gc->era = 3;
                if (gc->dayOfMonth == 4 && gc->monthOfYear == 12 && gc->year == 1957 && gc->era < 4)
                    gc->era = 4;
                if (gc->dayOfMonth == 26 && gc->monthOfYear == 2 && gc->year == 1962 && gc->era < 5)
                    gc->era = 5;
                if (oldEra != gc->era) {
                    gc->ready = false;
                    gc->transitionCode = 13;
                    gc_setState(gc, +GameState::MenuOverlay);
                }
            } else {
                ++gc->counterA;
            }
            gc->tickEnd = midp_current_time_millis();
            break;
        }
        case +GameState::BuildingMinimap: {
            if (gc->loadStep > 0) --gc->loadStep;
            if (gc->loadStep != 2) break;
            int size = gc->world.mapHeight + gc->world.mapWidth;
            gc->backbuffer = Image::create_blank(size, size);
            gc->backbuffer_surface = gc->backbuffer->drawing_surface();
            midp_set_color(gc->backbuffer_surface, 0);
            midp_fill_rect(gc->backbuffer_surface, 0, 0, size, size);
            int previous_kind = -1;
            int kind = 0;
            for (int x = 0; x < gc->world.mapWidth; ++x) {
                for (int y = 0; y < gc->world.mapHeight; ++y) {
                    int draw_y = gc->world.mapWidth - x + y;
                    int draw_x = x + y;
                    int tile = gc->world.tileMap[x][y];
                    int track = gc->world.trackMap[x][y];
                    if (tile == 0) kind = 1;
                    if (tile >= 3 && tile <= 28) kind = 2;
                    if (tile >= tile::SCENERY_FIRST && tile <= tile::SCENERY_LAST) kind = 3;
                    if (tile >= 34 && tile <= 41) kind = 4;
                    if (tile >= 42 && tile <= 48) kind = 5;
                    if (tile >= 50 && tile <= 52) kind = 6;
                    if (tile >= 53 && tile <= 55) kind = 7;
                    if (tile >= 56 && tile <= 58) kind = 8;
                    if (tile >= 59 && tile <= 61) kind = 9;
                    if (tile >= 62 && tile <= 64) kind = 10;
                    if (tile >= 65 && tile <= 68) kind = 11;
                    if (tile >= 70 && tile <= 90) {
                        if (track == 64 || track == 65) kind = 12;
                        if (track == 66) kind = 13;
                        if (track == 67) kind = 14;
                        if (track == 68) kind = 15;
                    }
                    if (kind != previous_kind) {
                        switch (kind) {
                            case 1:
                                if (gc->world.tileElevation[x][y] == -1) midp_set_color(gc->backbuffer_surface, 3167360);
                                if (gc->world.tileElevation[x][y] == 0) midp_set_color(gc->backbuffer_surface, 5274640);
                                if (gc->world.tileElevation[x][y] == 1) midp_set_color(gc->backbuffer_surface, 6327328);
                                if (gc->world.tileElevation[x][y] == 2) midp_set_color(gc->backbuffer_surface, 7380016);
                                if (gc->world.tileElevation[x][y] == 3) midp_set_color(gc->backbuffer_surface, 8432704);
                                break;
                            case 2: midp_set_color(gc->backbuffer_surface, 6327312); break;
                            case 3: midp_set_color(gc->backbuffer_surface, 0x909040); break;
                            case 4: midp_set_color(gc->backbuffer_surface, 0x606060); break;
                            case 5: midp_set_color(gc->backbuffer_surface, 0x404040); break;
                            case 6: midp_set_color(gc->backbuffer_surface, 0xFC0000); break;
                            case 7: midp_set_color(gc->backbuffer_surface, 0x101010); break;
                            case 8: midp_set_color(gc->backbuffer_surface, 0xFC9C00); break;
                            case 9: midp_set_color(gc->backbuffer_surface, 0x78DC1E); break;
                            case 10: midp_set_color(gc->backbuffer_surface, 0xFCFC00); break;
                            case 11: midp_set_color(gc->backbuffer_surface, 0x80C4FC); break;
                            case 12: midp_set_color(gc->backbuffer_surface, 0); break;
                            case 13: midp_set_color(gc->backbuffer_surface, 14287063); break;
                            case 14: midp_set_color(gc->backbuffer_surface, 16738557); break;
                            case 15: midp_set_color(gc->backbuffer_surface, 16040947); break;
                        }
                    }
                    previous_kind = kind;
                    midp_draw_line(gc->backbuffer_surface, draw_x, draw_y,
                                   draw_x + 1, draw_y);
                }
                previous_kind = -1;
            }
            gc_setState(gc, +GameState::MapOverview);
            break;
        }
        case +GameState::MapOverview:
            gc_viewportPanKey(gc);
            break;
        case +GameState::BuildingPath:
            if (gc->loadStep > 0) --gc->loadStep;
            if (gc->loadStep != 2) break;
            if (gc_tryBuildPath(gc)) {
                gc_setState(gc, +GameState::Playing);
                gc->buildMenuItemIndex = 0;
                gc->buildCancelled = true;
                gc->buildMenuMode = 2;
                gc->menuDirty = true;
                break;
            }
            gc->buildMenuItemIndex = 0;
            gc->buildCancelled = true;
            gc->buildMenuMode = 1;
            gc->menuDirty = true;
            gc->ready = false;
            gc->transitionCode = 12;
            gc_setState(gc, +GameState::MenuOverlay);
            break;
        case +GameState::SavingGame:
            if (gc->loadStep > 0) --gc->loadStep;
            if (gc->loadStep != 2) break;
            gc_saveAllMapArrays(gc);
            gc->buildMenuItemIndex = 0;
            gc->menuDirty = true;
            gc_setState(gc, +GameState::Playing);
            break;
        case +GameState::LoadingGame:
            if (gc->loadStep > 0) --gc->loadStep;
            if (gc->loadStep != 2) break;
            gc_restoreAllMapArrays(gc);
            gc->pauseFlag = false;
            gc->buildMenuItemIndex = 0;
            gc->menuDirty = true;
            gc_setState(gc, +GameState::Playing);
            break;
        case +GameState::AiTurn:
            if (gc->loadStep > 0) --gc->loadStep;
            if (gc->loadStep != 2) break;
            if (gc->nextTrackPieceCode - track::AI_PIECE_FIRST < gc->difficultyCycle) {
                gc_placeCitiesAndIndustry(gc);
                gc_setState(gc, +GameState::Playing);
                break;
            }
            ++gc->nextTrackPieceCode;
            if (gc->nextTrackPieceCode == track::AI_PIECE_WRAP) gc->nextTrackPieceCode = track::AI_PIECE_FIRST;
            gc_setState(gc, +GameState::Playing);
            break;
        case +GameState::HighScoreSync:
            if (gc->loadStep > 0) --gc->loadStep;
            if (gc->loadStep != 2) break;
            gc->transitionCode = gc_syncHighScores(gc, gc->highScoreUploadRequested)
                                 ? 19 : 18;
            gc_setState(gc, +GameState::MenuOverlay);
            break;
    }
}
