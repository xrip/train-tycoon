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
    if (gc->paintSuspended) return;
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
            int n5 = gc->assets.cycle_flag->width() / 2;
            int n6 = gc->assets.cycle_flag->height();
            int n7 = n6 / 10 + 1;
            for (int i = 0; i < 10; ++i) {
                int n8 = (n3 + n5 + 30) * (gc->loadingTimer - 5 * (i / 2)) / 10;
                if (n8 < 0) n8 = 0;
                if (n8 > n3 + n5 + 30) n8 = n3 + n5 + 30;
                int n9 = n3 - (n3 + n5 + 30 - n8);
                if (i % 2 == 1) n9 = n3 + (n3 + n5 + 30 - n8);
                midp_set_clip(g, n9 - n5, n4 - n6 / 2 + i * n7, n5 * 2, n7);
                midp_draw_image(g, gc->assets.cycle_flag.get(), n9, n4, 3);
            }
            midp_set_clip(g, 0, 0, gcs_screenWidth, gcs_hs);
            break;
        }
        case +GameState::LoadingResources: {
            midp_set_color(g, 0xFFFFFF);
            midp_fill_rect(g, 0, 0, gcs_screenWidth, gcs_hs);
            midp_draw_image(g, gc->assets.cycle_flag.get(), gcs_screenWidth / 2, gcs_hs / 2, 3);
            midp_set_color(g, RGB3(215, 222, 238));
            midp_fill_rect(g, 10, gcs_hs - 13, gcs_screenWidth - 21, 9);
            int n10 = gc->assets.spriteLoadCounter * (gcs_screenWidth - 20) / (gc->scenario.scenarioResourceCount - 1);
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
                if (gc->assets.cycle_flag != NULL)
                    midp_draw_image(g, gc->assets.cycle_flag.get(), gcs_screenWidth / 2, gcs_hs / 2, 3);
            }
            if (gc->ui.transitionCode == 36) {
                gc->gameState = +GameState::MapOverview;
                gc_paint(gc, g);
                gc->gameState = +GameState::MenuOverlay;
            }
            gc_renderPackedSprite(gc, g, gc->ui.transitionCode);
            break;
        }
        case +GameState::GeneratingMap: {
            midp_set_color(g, 0);
            midp_fill_rect(g, 0, 0, gcs_screenWidth, gcs_hs);
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc_tracked_codes(gc->assets.spriteBitmapTable[gc->assets.spriteIndexTable[0]]));
            midp_set_color(g, RGB3(215, 222, 238));
            midp_fill_rect(g, 10, gcs_hs - 23, gcs_screenWidth - 21, 9);
            int n11 = gc->world.totalTilesToGenerate < 2 ? 2 : gc->world.totalTilesToGenerate;
            int n12 = gc->world.mapGenTilesDone * (gcs_screenWidth - 20) / (n11 - 1);
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
            if (gc->ui.inputPending) {
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
            if (gc->ui.pauseFlag) gc_drawSpriteQuad(gc, g, 236, 0, 15, 0);
            if (gc->ui.pendingTransitionCode > -1) {
                gc->ui.transitionCode = gc->ui.pendingTransitionCode;
                gc_resetOverlayRenderState(gc);
                gc->ui.pendingTransitionCode = -1;
                gc_renderPackedSprite(gc, g, gc->ui.transitionCode);
                gc_setState(gc, +GameState::MenuOverlay);
            }
            if (gc->ui.transitionDebounce <= 0) break;
            gc->ui.transitionDebounce = (int8_t)(gc->ui.transitionDebounce - 1);
            if (gc->ui.transitionDebounce != 0) break;
            gc->ui.transitionCode = transition::SCENARIO_NOTICE_BASE + gc->scenario.scenarioNoticeCode;
            gc_resetOverlayRenderState(gc);
            gc->ready = false;
            gc_renderPackedSprite(gc, g, gc->ui.transitionCode);
            gc_setState(gc, +GameState::MenuOverlay);
            break;
        }
        case +GameState::BuildingMinimap:
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc_tracked_codes(gc->assets.spriteBitmapTable[gc->assets.spriteIndexTable[50]]));
            break;
        case +GameState::MapOverview: {
            int n14;
            midp_set_clip(g, 0, 15, gcs_screenWidth, gcs_gameplayViewportHeight);
            midp_set_color(g, 0);
            midp_fill_rect(g, 0, 15, gcs_screenWidth, gcs_gameplayViewportHeight);
            int n15 = gc->renderer.viewportCenterX / 16;
            int n16 = gc->renderer.viewportCenterY / 8;
            int n17 = gc->world.mapWidth + gc->world.mapHeight;
            int n18 = gcs_screenWidth / 2 - n15 - gcs_screenWidth / 32;
            if (gcs_screenWidth - n18 > n17) n18 = gcs_screenWidth - n17;
            if (n18 > 0) n18 = 0;
            if (gcs_gameplayViewportHeight - (n14 = gcs_gameplayViewportHeight / 2 - n16 - gcs_gameplayViewportHeight / 16) > n17) n14 = gcs_gameplayViewportHeight - n17;
            if (n14 > 0) n14 = 0;
            int n19 = n17 < gcs_screenWidth ? (gcs_screenWidth - n17) / 2 : 0;
            int n20 = n17 < gcs_gameplayViewportHeight ? (gcs_gameplayViewportHeight - n17) / 2 : 0;
            midp_draw_image(g, gc->renderer.backbuffer.get(), n18 + n19, 15 + n14 + n20, 20);
            int n21 = gc->renderer.viewportCenterX - 2 * gc->renderer.viewportCenterY + 16 + 2 * gc->world.mapHalfWidth;
            int n22 = gc->renderer.viewportCenterX + 2 * gc->renderer.viewportCenterY - 16 - 2 * gc->world.mapHalfWidth;
            int n23 = gc->world.mapWidth - (n21 /= 32) + (n22 /= 32);
            int n24 = n21 + n22;
            midp_set_color(g, 0xFFFFFF);
            midp_draw_rect(g, n24 + n18 + n19, 15 + n23 + n14 + n20, gcs_screenWidth / 16, gcs_gameplayViewportHeight / 8);
            midp_set_clip(g, 0, 0, gcs_screenWidth, gcs_hs);
            gc_drawConfirmBar(gc, g);
            gc_drawConfirmDialog(gc, g);
            if (gc->confirmCloseRequested) {
                gc->ready = false;
                gc->ui.transitionCode = 36;
                gc_setState(gc, +GameState::MenuOverlay);
                gc->confirmCloseRequested = false;
            }
            if (gc->ui.pendingTransitionCode <= -1) break;
            gc->ui.transitionCode = gc->ui.pendingTransitionCode;
            gc_resetOverlayRenderState(gc);
            gc->ui.pendingTransitionCode = -1;
            gc_renderPackedSprite(gc, g, gc->ui.transitionCode);
            gc_setState(gc, +GameState::MenuOverlay);
            break;
        }
        case +GameState::BuildingPath:
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc_tracked_codes(gc->assets.spriteBitmapTable[gc->assets.spriteIndexTable[51]]));
            break;
        case +GameState::SavingGame:
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc_tracked_codes(gc->assets.spriteBitmapTable[gc->assets.spriteIndexTable[52]]));
            break;
        case +GameState::LoadingGame:
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc_tracked_codes(gc->assets.spriteBitmapTable[gc->assets.spriteIndexTable[53]]));
            break;
        case +GameState::AiTurn:
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc_tracked_codes(gc->assets.spriteBitmapTable[gc->assets.spriteIndexTable[49]]));
            break;
        case +GameState::HighScoreSync:
            gc_drawTextBox(gc, g, gcs_screenWidth / 2, gcs_hs / 2, 0, 3,
                           gc_tracked_codes(gc->assets.spriteBitmapTable[gc->assets.spriteIndexTable[66]]));
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
                for (int i = 0; i < 6; ++i) gc->assets.font_images[i] = gc_decodePngFromSpriteStream(gc);
                gc_setState(gc, +GameState::IntroReady);
                break;
            }
            gc_setState(gc, gc->gameState + 1);
            break;
        }
        case +GameState::IntroReady:
            gc->ready = true;
            gc->ui.screenParam = 3;
            gc->ui.transitionCode = 14;
            gc_resetOverlayRenderState(gc);
            gc_setState(gc, +GameState::MenuOverlay);
            break;
        case +GameState::LoadingResources:
            gc->assets.tile_sprites[gc->assets.spriteLoadCounter] = gc_decodePngFromSpriteStream(gc);
            ++gc->assets.spriteLoadCounter;
            if (gc->assets.spriteLoadCounter == gc->scenario.scenarioResourceCount / 2) {
                gc_loadMusic(gc);
                if (gc->music_auto_start) { gc_playMusic(gc); gc->music_enabled = true; }
                else gc->music_enabled = false;
            }
            if (gc->assets.spriteLoadCounter != gc->scenario.scenarioResourceCount) break;
            gc->ready = true;
            gc->ui.transitionCode = 1;
            gc_setState(gc, +GameState::MenuOverlay);
            break;
        case +GameState::GeneratingMap: {
            ++gc->loadStep;
            if (gc->loadStep == 2) {
                gc_initSaveOrLoad(gc, save::OP_RESET_SKIP);
                gc->economy.dayOfMonth = 1; gc->economy.monthOfYear = 1; gc->economy.year = 1950;
                gc->scenario.milestonesEnabled = false; gc->ui.pauseFlag = false; gc->scenario.aiEnabled = false;
                gc->economy.moneyBaseA = 974; gc->economy.moneyBaseB = 974;
                gc->economy.baseTrainSpeed = 173; gc->economy.accel1 = 342; gc->economy.brake1 = 311;
                gc->economy.baseTrainSpeed2 = 173; gc->economy.accel2 = 234;
                gc->economy.costPower = 4759; gc->economy.costStruct = 9845; gc->economy.costOverhead = 347;
                gc->economy.costMaintenance = 5982; gc->economy.costCrew = 481;
                gc->economy.gWagonCost = 37894; gc->economy.gWagonCost2 = 43247; gc->economy.gWagonCost3 = 51872;
                gc->economy.gWagonCost4 = 60149; gc->economy.gWagonCost5 = 71423; gc->economy.gWagonCost6 = 82675;
                gc->economy.costGroupC1 = 4578; gc->economy.costGroupC2 = 5196; gc->economy.costGroupC3 = 4314; gc->economy.costGroupC4 = 5072;
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
                        if (scenario_index != gc->scenario.activeScenario) continue;
                        gc->scenario.scenarioStep = scenario_index;
                        gc->world.mapWidth = gc->world.mapHeight = map_size;
                        gc->world.mapHalfWidth = (gc->world.mapWidth - 1) * 8;
                        gc->world.mapScreenWidth = (gc->world.mapWidth + gc->world.mapHeight) * 16;
                        gc->world.mapScreenHeight = (gc->world.mapWidth + gc->world.mapHeight) * 8;
                        gc->renderer.viewportHalfWidth = gc->world.mapScreenWidth / 2;
                        gc->renderer.viewportHalfHeight = gc->world.mapScreenHeight / 2;
                        gc->renderer.viewportCenterX = gc->renderer.viewportHalfWidth;
                        gc->renderer.viewportCenterY = gc->renderer.viewportHalfHeight;
                        gc->renderer.viewportBaseX = gc->renderer.viewportCenterX;
                        gc->renderer.viewportBaseY = gc->renderer.viewportCenterY;
                        gc->renderer.mapScaleX = gc->renderer.viewportBaseX * 2;
                        gc->renderer.mapScaleY = gc->renderer.viewportBaseY * 2;
                        gc->world.initialObstacleCount = obstacle_count; gc->world.terrainRaiseStepCount = terrain_raise_steps; gc->world.terrainLowerStepCount = terrain_lower_steps; gc->world.terrainPatchBaseSize = terrain_patch_size;
                        gc->world.mapGenColumns = map_gen_columns; gc->world.mapGenRows = map_gen_rows;
                        gc->world.terrainFeatureStage7Count = stage7_feature_count; gc->world.terrainFeatureStage6Count = stage6_feature_count;
                        gc->world.weightAccel = weight_accel_1; gc->world.weightAccel2 = weight_accel_2; gc->world.weightAccel3 = weight_accel_3; gc->world.weightAccel4 = weight_accel_4;
                        gc->economy.money = starting_money; gc->economy.loanCeiling = loan_ceiling; gc->economy.loanBalance = starting_loan;
                        gc->economy.interestRate = (int8_t)interest_rate; gc->era = era_number - 1;
                        gc->scenario.scenarioMilestoneIndex = (int8_t)milestone_index;
                        gc->scenario.currentMilestoneScore = milestone_score; gc->scenario.milestoneCoThreshold2 = milestone_threshold;
                        gc_initBuildMenuForScenario(gc, build_menu_scenario);
                        gc->scenario.aiExpansionEnabled = (scenario_flags & 2) == 2;
                        gc->scenario.monthlyOffersEnabled = (scenario_flags & 4) == 4;
                        if ((scenario_flags & 1) != 1) { gc->scenario.aiFrequency = 25; gc->scenario.aiThreshold = 100; }
                        else { gc->scenario.aiFrequency = 32; gc->scenario.aiThreshold = 29; }
                        gc->scenario.scenarioNoticeId = notice_id;
                        if (notice_id != 0) { gc->ui.transitionDebounce = 5; gc->scenario.scenarioNoticeCode = (int8_t)notice_id; continue; }
                        gc->ui.transitionDebounce = 0;
                    }
                }
                if (gc->scenario.activeScenario <= 1) {
                    if (gc->scenario.difficultySlot == 0) { gc->scenario.aiFrequency = 32; gc->scenario.aiThreshold = 29; }
                    if (gc->scenario.difficultySlot == 1) { gc->scenario.aiFrequency = 25; gc->scenario.aiThreshold = 22; }
                    if (gc->scenario.difficultyA == 0) { gc->economy.interestRate = 4; gc->economy.enterpriseTaxPct = 2; }
                    if (gc->scenario.difficultyA == 1) { gc->economy.interestRate = 8; gc->economy.enterpriseTaxPct = 5; }
                    if (gc->scenario.difficultyB == 0) { gc->world.mapGenColumns /= 2; gc->world.mapGenRows /= 2; }
                    if (gc->scenario.difficultyC == 0) {
                        gc->world.terrainFeatureStage7Count /= 2; gc->world.terrainFeatureStage6Count /= 2;
                        gc->world.weightAccel /= 2; gc->world.weightAccel2 /= 2; gc->world.weightAccel3 /= 2; gc->world.weightAccel4 /= 2;
                    }
                }
            }
            if (gc->loadStep == 3) {
                gc->ui.buildMenuItemIndex = 0;
                gc->ui.buildMenuMode = 0;
                gc_loadState25Setup(gc, 0);
            }
            if (gc->loadStep <= 3) break;
            gc_loadState25Setup(gc, 1);
            if (gc->world.mapGenTilesDone != gc->world.totalTilesToGenerate) break;
            gc->world.mapGenTilesDone = 0;
            gc_setState(gc, +GameState::Playing);
            break;
        }
        case +GameState::Playing: {
            gc_simulationTick(gc);
            break;
        }
        case +GameState::BuildingMinimap: {
            if (gc->loadStep > 0) --gc->loadStep;
            if (gc->loadStep != 2) break;
            int size = gc->world.mapHeight + gc->world.mapWidth;
            gc->renderer.backbuffer = Image::create_blank(size, size);
            gc->renderer.backbuffer_surface = gc->renderer.backbuffer->drawing_surface();
            midp_set_color(gc->renderer.backbuffer_surface, 0);
            midp_fill_rect(gc->renderer.backbuffer_surface, 0, 0, size, size);
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
                                if (gc->world.tileElevation[x][y] == -1) midp_set_color(gc->renderer.backbuffer_surface, 3167360);
                                if (gc->world.tileElevation[x][y] == 0) midp_set_color(gc->renderer.backbuffer_surface, 5274640);
                                if (gc->world.tileElevation[x][y] == 1) midp_set_color(gc->renderer.backbuffer_surface, 6327328);
                                if (gc->world.tileElevation[x][y] == 2) midp_set_color(gc->renderer.backbuffer_surface, 7380016);
                                if (gc->world.tileElevation[x][y] == 3) midp_set_color(gc->renderer.backbuffer_surface, 8432704);
                                break;
                            case 2: midp_set_color(gc->renderer.backbuffer_surface, 6327312); break;
                            case 3: midp_set_color(gc->renderer.backbuffer_surface, 0x909040); break;
                            case 4: midp_set_color(gc->renderer.backbuffer_surface, 0x606060); break;
                            case 5: midp_set_color(gc->renderer.backbuffer_surface, 0x404040); break;
                            case 6: midp_set_color(gc->renderer.backbuffer_surface, 0xFC0000); break;
                            case 7: midp_set_color(gc->renderer.backbuffer_surface, 0x101010); break;
                            case 8: midp_set_color(gc->renderer.backbuffer_surface, 0xFC9C00); break;
                            case 9: midp_set_color(gc->renderer.backbuffer_surface, 0x78DC1E); break;
                            case 10: midp_set_color(gc->renderer.backbuffer_surface, 0xFCFC00); break;
                            case 11: midp_set_color(gc->renderer.backbuffer_surface, 0x80C4FC); break;
                            case 12: midp_set_color(gc->renderer.backbuffer_surface, 0); break;
                            case 13: midp_set_color(gc->renderer.backbuffer_surface, 14287063); break;
                            case 14: midp_set_color(gc->renderer.backbuffer_surface, 16738557); break;
                            case 15: midp_set_color(gc->renderer.backbuffer_surface, 16040947); break;
                        }
                    }
                    previous_kind = kind;
                    midp_draw_line(gc->renderer.backbuffer_surface, draw_x, draw_y,
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
                gc->ui.buildMenuItemIndex = 0;
                gc->build.buildCancelled = true;
                gc->ui.buildMenuMode = 2;
                gc->ui.menuDirty = true;
                break;
            }
            gc->ui.buildMenuItemIndex = 0;
            gc->build.buildCancelled = true;
            gc->ui.buildMenuMode = 1;
            gc->ui.menuDirty = true;
            gc->ready = false;
            gc->ui.transitionCode = 12;
            gc_setState(gc, +GameState::MenuOverlay);
            break;
        case +GameState::SavingGame:
            if (gc->loadStep > 0) --gc->loadStep;
            if (gc->loadStep != 2) break;
            gc_saveAllMapArrays(gc);
            gc->ui.buildMenuItemIndex = 0;
            gc->ui.menuDirty = true;
            gc_setState(gc, +GameState::Playing);
            break;
        case +GameState::LoadingGame:
            if (gc->loadStep > 0) --gc->loadStep;
            if (gc->loadStep != 2) break;
            gc_restoreAllMapArrays(gc);
            gc->ui.pauseFlag = false;
            gc->ui.buildMenuItemIndex = 0;
            gc->ui.menuDirty = true;
            gc_setState(gc, +GameState::Playing);
            break;
        case +GameState::AiTurn:
            if (gc->loadStep > 0) --gc->loadStep;
            if (gc->loadStep != 2) break;
            if (gc->build.nextTrackPieceCode - track::AI_PIECE_FIRST < gc->scenario.difficultyCycle) {
                gc_placeCitiesAndIndustry(gc);
                gc_setState(gc, +GameState::Playing);
                break;
            }
            ++gc->build.nextTrackPieceCode;
            if (gc->build.nextTrackPieceCode == track::AI_PIECE_WRAP) gc->build.nextTrackPieceCode = track::AI_PIECE_FIRST;
            gc_setState(gc, +GameState::Playing);
            break;
        case +GameState::HighScoreSync:
            if (gc->loadStep > 0) --gc->loadStep;
            if (gc->loadStep != 2) break;
            gc->ui.transitionCode = gc_syncHighScores(gc, gc->scenario.highScoreUploadRequested)
                                 ? 19 : 18;
            gc_setState(gc, +GameState::MenuOverlay);
            break;
    }
}
