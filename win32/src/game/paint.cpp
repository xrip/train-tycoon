// Game paint and update state machines.
#include "game_internal.h"
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

namespace {

void paintMapOverview(Game *game, Surface *g) {
    midp_set_clip(g, 0, 15, canvasWidth, gameplayViewportHeight);
    midp_set_color(g, 0);
    midp_fill_rect(g, 0, 15, canvasWidth, gameplayViewportHeight);
    int viewportMapX = game->renderer.viewportCenterX / 16;
    int viewportMapY = game->renderer.viewportCenterY / 8;
    int overviewSize = game->world.mapWidth + game->world.mapHeight;
    int drawX = canvasWidth / 2 - viewportMapX - canvasWidth / 32;
    if (canvasWidth - drawX > overviewSize) drawX = canvasWidth - overviewSize;
    if (drawX > 0) drawX = 0;
    int drawY = gameplayViewportHeight / 2 - viewportMapY - gameplayViewportHeight / 16;
    if (gameplayViewportHeight - drawY > overviewSize) drawY = gameplayViewportHeight - overviewSize;
    if (drawY > 0) drawY = 0;
    int centerX = overviewSize < canvasWidth ? (canvasWidth - overviewSize) / 2 : 0;
    int centerY = overviewSize < gameplayViewportHeight ? (gameplayViewportHeight - overviewSize) / 2 : 0;
    midp_draw_image(g, game->renderer.backbuffer.get(), drawX + centerX, 15 + drawY + centerY, 20);
    int mapX = game->renderer.viewportCenterX - 2 * game->renderer.viewportCenterY + 16 + 2 * game->world.mapHalfWidth;
    int mapY = game->renderer.viewportCenterX + 2 * game->renderer.viewportCenterY - 16 - 2 * game->world.mapHalfWidth;
    int boxY = game->world.mapWidth - (mapX /= 32) + (mapY /= 32);
    int boxX = mapX + mapY;
    midp_set_color(g, 0xFFFFFF);
    midp_draw_rect(g, boxX + drawX + centerX, 15 + boxY + drawY + centerY,
                   canvasWidth / 16, gameplayViewportHeight / 8);
    midp_set_clip(g, 0, 0, canvasWidth, canvasHeight);
    game_drawConfirmBar(game, g);
    game_drawConfirmDialog(game, g);
    if (game->confirmCloseRequested) {
        game->ready = false;
        game->ui.transitionCode = 36;
        game_setState(game, GameState::MenuOverlay);
        game->confirmCloseRequested = false;
    }
    if (game->ui.pendingTransitionCode <= -1) return;
    game->ui.transitionCode = game->ui.pendingTransitionCode;
    game_resetOverlayRenderState(game);
    game->ui.pendingTransitionCode = -1;
    game_renderPackedSprite(game, g, game->ui.transitionCode);
    game_setState(game, GameState::MenuOverlay);
}

} // namespace

void game_paint(Game *game, Surface *g) {
    // setFullScreenMode(true) is a no-op on this backend.
    int n = WIDTH, n2 = HEIGHT;
    game->clickMap.clear();  // rebuild overlay hit-rects fresh each frame
    game_setCanvasSize(game, n, n2);
    if (game->gameState != GameState::MapOverview && game->gameState != GameState::BuildingMinimap && game->gameState != GameState::BuildingPath
        && game->gameState != GameState::SavingGame && game->gameState != GameState::LoadingGame && game->gameState != GameState::AiTurn
        && game->gameState != GameState::HighScoreSync && (game->gameState != GameState::MenuOverlay || game->ready)) {
        midp_set_color(g, 0);
        midp_fill_rect(g, 0, 0, n, n2);
    }
    midp_set_clip(g, 0, 0, canvasWidth, canvasHeight);
    switch (game->gameState) {
        case GameState::BootSplash0: case GameState::BootSplash1: case GameState::BootSplash2: case GameState::BootSplash3: {
            midp_set_color(g, 0xFFFFFF);
            if (game->gameState == GameState::BootSplash0) midp_set_color(g, 0);
            midp_fill_rect(g, 0, 0, canvasWidth, canvasHeight);
            int n3 = canvasWidth / 2;
            int n4 = canvasHeight / 2;
            int n5 = game->assets.cycle_flag->width() / 2;
            int n6 = game->assets.cycle_flag->height();
            int n7 = n6 / 10 + 1;
            for (int i = 0; i < 10; ++i) {
                int n8 = (n3 + n5 + 30) * (game->loadingAnimationFrame - 5 * (i / 2)) / 10;
                if (n8 < 0) n8 = 0;
                if (n8 > n3 + n5 + 30) n8 = n3 + n5 + 30;
                int n9 = n3 - (n3 + n5 + 30 - n8);
                if (i % 2 == 1) n9 = n3 + (n3 + n5 + 30 - n8);
                midp_set_clip(g, n9 - n5, n4 - n6 / 2 + i * n7, n5 * 2, n7);
                midp_draw_image(g, game->assets.cycle_flag.get(), n9, n4, 3);
            }
            midp_set_clip(g, 0, 0, canvasWidth, canvasHeight);
            break;
        }
        case GameState::LoadingResources: {
            midp_set_color(g, 0xFFFFFF);
            midp_fill_rect(g, 0, 0, canvasWidth, canvasHeight);
            midp_draw_image(g, game->assets.cycle_flag.get(), canvasWidth / 2, canvasHeight / 2, 3);
            midp_set_color(g, RGB3(215, 222, 238));
            midp_fill_rect(g, 10, canvasHeight - 13, canvasWidth - 21, 9);
            int n10 = game->assets.spriteLoadCounter * (canvasWidth - 20) / (game->scenario.scenarioResourceCount - 1);
            midp_set_color(g, RGB3(60, 105, 144));
            midp_fill_rect(g, 10, canvasHeight - 13, n10, 9);
            midp_set_color(g, RGB3(49, 64, 90));
            midp_draw_rect(g, 10, canvasHeight - 13, canvasWidth - 21, 9);
            break;
        }
        case GameState::MenuOverlay: {
            if (game->ready) {
                midp_set_color(g, 0xFFFFFF);
                midp_fill_rect(g, 0, 0, canvasWidth, canvasHeight);
                if (game->assets.cycle_flag != NULL)
                    midp_draw_image(g, game->assets.cycle_flag.get(), canvasWidth / 2, canvasHeight / 2, 3);
            }
            if (game->ui.transitionCode == 36) {
                game->gameState = GameState::MapOverview;
                game_paint(game, g);
                game->gameState = GameState::MenuOverlay;
            }
            game_renderPackedSprite(game, g, game->ui.transitionCode);
            break;
        }
        case GameState::GeneratingMap: {
            midp_set_color(g, 0);
            midp_fill_rect(g, 0, 0, canvasWidth, canvasHeight);
            game_drawTextBox(game, g, canvasWidth / 2, canvasHeight / 2, 0, 3,
                           std::span<const int8_t>(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[0]]));
            midp_set_color(g, RGB3(215, 222, 238));
            midp_fill_rect(g, 10, canvasHeight - 23, canvasWidth - 21, 9);
            int n11 = game->world.totalTilesToGenerate < 2 ? 2 : game->world.totalTilesToGenerate;
            int n12 = game->world.mapGenTilesDone * (canvasWidth - 20) / (n11 - 1);
            midp_set_color(g, RGB3(60, 105, 144));
            midp_fill_rect(g, 10, canvasHeight - 23, n12, 9);
            midp_set_color(g, 0xFFFFFF);
            midp_draw_rect(g, 10, canvasHeight - 23, canvasWidth - 21, 9);
            break;
        }
        case GameState::Playing: {
            game_renderMap(game, g);
            if (game->ui.inputPending) {
                int n13;
                midp_set_color(g, 0);
                midp_set_clip(g, 0, 15, canvasWidth, bottomBarY - 15);
                for (n13 = 0; n13 < canvasWidth; ++n13)
                    midp_draw_line(g, n13 * 2, 0, canvasHeight + n13 * 2, canvasHeight);
                for (n13 = 0; n13 < canvasHeight; ++n13)
                    midp_draw_line(g, 0, n13 * 2, canvasWidth, canvasWidth + n13 * 2);
                midp_set_clip(g, 0, 0, canvasWidth, canvasHeight);
                game_drawSpriteQuad(game, g, 234, 0, 15, 20);
            }
            game_drawMoneyTicker(game, g);
            if (game->ui.pauseFlag) game_drawSpriteQuad(game, g, 236, 0, 15, 0);
            if (game->ui.pendingTransitionCode > -1) {
                game->ui.transitionCode = game->ui.pendingTransitionCode;
                game_resetOverlayRenderState(game);
                game->ui.pendingTransitionCode = -1;
                game_renderPackedSprite(game, g, game->ui.transitionCode);
                game_setState(game, GameState::MenuOverlay);
            }
            if (game->ui.transitionDebounce <= 0) break;
            game->ui.transitionDebounce = (int8_t)(game->ui.transitionDebounce - 1);
            if (game->ui.transitionDebounce != 0) break;
            game->ui.transitionCode = transition::SCENARIO_NOTICE_BASE + game->scenario.scenarioNoticeCode;
            game_resetOverlayRenderState(game);
            game->ready = false;
            game_renderPackedSprite(game, g, game->ui.transitionCode);
            game_setState(game, GameState::MenuOverlay);
            break;
        }
        case GameState::BuildingMinimap:
            game_drawTextBox(game, g, canvasWidth / 2, canvasHeight / 2, 0, 3,
                           std::span<const int8_t>(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[50]]));
            break;
        case GameState::MapOverview:
            paintMapOverview(game, g);
            break;
        case GameState::BuildingPath:
            game_drawTextBox(game, g, canvasWidth / 2, canvasHeight / 2, 0, 3,
                           std::span<const int8_t>(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[51]]));
            break;
        case GameState::SavingGame:
            game_drawTextBox(game, g, canvasWidth / 2, canvasHeight / 2, 0, 3,
                           std::span<const int8_t>(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[52]]));
            break;
        case GameState::LoadingGame:
            game_drawTextBox(game, g, canvasWidth / 2, canvasHeight / 2, 0, 3,
                           std::span<const int8_t>(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[53]]));
            break;
        case GameState::AiTurn:
            game_drawTextBox(game, g, canvasWidth / 2, canvasHeight / 2, 0, 3,
                           std::span<const int8_t>(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[49]]));
            break;
        case GameState::HighScoreSync:
            game_drawTextBox(game, g, canvasWidth / 2, canvasHeight / 2, 0, 3,
                           std::span<const int8_t>(game->assets.spriteBitmapTable[game->assets.spriteIndexTable[66]]));
            break;
    }
}

namespace {

void setMinimapColor(Game *game, int kind, int x, int y) {
    Surface *surface = game->renderer.backbuffer_surface;
    switch (kind) {
        case 1:
            if (game->world.tileElevation[x][y] == -1) midp_set_color(surface, 3167360);
            if (game->world.tileElevation[x][y] == 0) midp_set_color(surface, 5274640);
            if (game->world.tileElevation[x][y] == 1) midp_set_color(surface, 6327328);
            if (game->world.tileElevation[x][y] == 2) midp_set_color(surface, 7380016);
            if (game->world.tileElevation[x][y] == 3) midp_set_color(surface, 8432704);
            break;
        case 2: midp_set_color(surface, 6327312); break;
        case 3: midp_set_color(surface, 0x909040); break;
        case 4: midp_set_color(surface, 0x606060); break;
        case 5: midp_set_color(surface, 0x404040); break;
        case 6: midp_set_color(surface, 0xFC0000); break;
        case 7: midp_set_color(surface, 0x101010); break;
        case 8: midp_set_color(surface, 0xFC9C00); break;
        case 9: midp_set_color(surface, 0x78DC1E); break;
        case 10: midp_set_color(surface, 0xFCFC00); break;
        case 11: midp_set_color(surface, 0x80C4FC); break;
        case 12: midp_set_color(surface, 0); break;
        case 13: midp_set_color(surface, 14287063); break;
        case 14: midp_set_color(surface, 16738557); break;
        case 15: midp_set_color(surface, 16040947); break;
    }
}

void buildMinimapOverview(Game *game) {
    int size = game->world.mapHeight + game->world.mapWidth;
    game->renderer.backbuffer = Image::create_blank(size, size);
    game->renderer.backbuffer_surface = game->renderer.backbuffer->drawing_surface();
    midp_set_color(game->renderer.backbuffer_surface, 0);
    midp_fill_rect(game->renderer.backbuffer_surface, 0, 0, size, size);
    int previousKind = -1;
    int kind = 0;
    for (int x = 0; x < game->world.mapWidth; ++x) {
        for (int y = 0; y < game->world.mapHeight; ++y) {
            int drawY = game->world.mapWidth - x + y;
            int drawX = x + y;
            int tileCode = game->world.tileMap[x][y];
            int trackCode = game->world.trackMap[x][y];
            if (tileCode == 0) kind = 1;
            if (tileCode >= 3 && tileCode <= 28) kind = 2;
            if (tileCode >= tile::SCENERY_FIRST && tileCode <= tile::SCENERY_LAST) kind = 3;
            if (tileCode >= 34 && tileCode <= 41) kind = 4;
            if (tileCode >= 42 && tileCode <= 48) kind = 5;
            if (tileCode >= 50 && tileCode <= 52) kind = 6;
            if (tileCode >= 53 && tileCode <= 55) kind = 7;
            if (tileCode >= 56 && tileCode <= 58) kind = 8;
            if (tileCode >= 59 && tileCode <= 61) kind = 9;
            if (tileCode >= 62 && tileCode <= 64) kind = 10;
            if (tileCode >= 65 && tileCode <= 68) kind = 11;
            if (tileCode >= 70 && tileCode <= 90) {
                if (trackCode == 64 || trackCode == 65) kind = 12;
                if (trackCode == 66) kind = 13;
                if (trackCode == 67) kind = 14;
                if (trackCode == 68) kind = 15;
            }
            if (kind != previousKind) setMinimapColor(game, kind, x, y);
            previousKind = kind;
            midp_draw_line(game->renderer.backbuffer_surface, drawX, drawY, drawX + 1, drawY);
        }
        previousKind = -1;
    }
    game_setState(game, GameState::MapOverview);
}

} // namespace

void game_update(Game *game) {
    switch (game->gameState) {
        case GameState::BootSplash0: case GameState::BootSplash1: case GameState::BootSplash2: {
            if (midp_current_time_millis() - game->loadingStartTime > 5000L)
                game->loadingTimeout = true;
            ++game->loadingAnimationFrame;
            if (!game->loadingTimeout) break;
            if (game->gameState == GameState::BootSplash1) {
                for (int i = 0; i < 6; ++i) game->assets.font_images[i] = game_decodePngFromSpriteStream(game);
                game_setState(game, GameState::IntroReady);
                break;
            }
            game_setState(game, game->gameState + 1);
            break;
        }
        case GameState::IntroReady:
            game->ready = true;
            game->ui.screenParam = 3;
            game->ui.transitionCode = 14;
            game_resetOverlayRenderState(game);
            game_setState(game, GameState::MenuOverlay);
            break;
        case GameState::LoadingResources:
            game->assets.tile_sprites[game->assets.spriteLoadCounter] = game_decodePngFromSpriteStream(game);
            ++game->assets.spriteLoadCounter;
            if (game->assets.spriteLoadCounter == game->scenario.scenarioResourceCount / 2) {
                game_loadMusic(game);
                if (game->music_auto_start) { game_playMusic(game); game->music_enabled = true; }
                else game->music_enabled = false;
            }
            if (game->assets.spriteLoadCounter != game->scenario.scenarioResourceCount) break;
            game->ready = true;
            game->ui.transitionCode = 1;
            game_setState(game, GameState::MenuOverlay);
            break;
        case GameState::GeneratingMap: {
            ++game->stateWorkStep;
            if (game->stateWorkStep == 2) {
                game_initSaveOrLoad(game, save::OP_RESET_SKIP);
                game->economy.dayOfMonth = 1; game->economy.monthOfYear = 1; game->economy.year = 1950;
                game->scenario.milestonesEnabled = false; game->ui.pauseFlag = false; game->scenario.aiEnabled = false;
                game->economy.moneyBaseA = 974; game->economy.moneyBaseB = 974;
                game->economy.baseTrainSpeed = 173; game->economy.accel1 = 342; game->economy.brake1 = 311;
                game->economy.baseTrainSpeed2 = 173; game->economy.accel2 = 234;
                game->economy.costPower = 4759; game->economy.costStruct = 9845; game->economy.costOverhead = 347;
                game->economy.costMaintenance = 5982; game->economy.costCrew = 481;
                game->economy.gWagonCost = 37894; game->economy.gWagonCost2 = 43247; game->economy.gWagonCost3 = 51872;
                game->economy.gWagonCost4 = 60149; game->economy.gWagonCost5 = 71423; game->economy.gWagonCost6 = 82675;
                game->economy.costGroupC1 = 4578; game->economy.costGroupC2 = 5196; game->economy.costGroupC3 = 4314; game->economy.costGroupC4 = 5072;
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
                        if (scenario_index != game->scenario.activeScenario) continue;
                        game->scenario.scenarioStep = scenario_index;
                        game->world.mapWidth = game->world.mapHeight = map_size;
                        game->world.mapHalfWidth = (game->world.mapWidth - 1) * 8;
                        game->world.mapScreenWidth = (game->world.mapWidth + game->world.mapHeight) * 16;
                        game->world.mapScreenHeight = (game->world.mapWidth + game->world.mapHeight) * 8;
                        game->renderer.viewportHalfWidth = game->world.mapScreenWidth / 2;
                        game->renderer.viewportHalfHeight = game->world.mapScreenHeight / 2;
                        game->renderer.viewportCenterX = game->renderer.viewportHalfWidth;
                        game->renderer.viewportCenterY = game->renderer.viewportHalfHeight;
                        game->renderer.viewportBaseX = game->renderer.viewportCenterX;
                        game->renderer.viewportBaseY = game->renderer.viewportCenterY;
                        game->renderer.mapScaleX = game->renderer.viewportBaseX * 2;
                        game->renderer.mapScaleY = game->renderer.viewportBaseY * 2;
                        game->world.initialObstacleCount = obstacle_count; game->world.terrainRaiseStepCount = terrain_raise_steps; game->world.terrainLowerStepCount = terrain_lower_steps; game->world.terrainPatchBaseSize = terrain_patch_size;
                        game->world.mapGenColumns = map_gen_columns; game->world.mapGenRows = map_gen_rows;
                        game->world.terrainFeatureStage7Count = stage7_feature_count; game->world.terrainFeatureStage6Count = stage6_feature_count;
                        game->world.weightAccel = weight_accel_1; game->world.weightAccel2 = weight_accel_2; game->world.weightAccel3 = weight_accel_3; game->world.weightAccel4 = weight_accel_4;
                        game->economy.money = starting_money; game->economy.loanCeiling = loan_ceiling; game->economy.loanBalance = starting_loan;
                        game->economy.interestRate = (int8_t)interest_rate; game->era = era_number - 1;
                        game->scenario.scenarioMilestoneIndex = (int8_t)milestone_index;
                        game->scenario.currentMilestoneScore = milestone_score; game->scenario.milestoneCoThreshold2 = milestone_threshold;
                        game_initBuildMenuForScenario(game, build_menu_scenario);
                        game->scenario.aiExpansionEnabled = (scenario_flags & 2) == 2;
                        game->scenario.monthlyOffersEnabled = (scenario_flags & 4) == 4;
                        if ((scenario_flags & 1) != 1) { game->scenario.aiFrequency = 25; game->scenario.aiThreshold = 100; }
                        else { game->scenario.aiFrequency = 32; game->scenario.aiThreshold = 29; }
                        game->scenario.scenarioNoticeId = notice_id;
                        if (notice_id != 0) { game->ui.transitionDebounce = 5; game->scenario.scenarioNoticeCode = (int8_t)notice_id; continue; }
                        game->ui.transitionDebounce = 0;
                    }
                }
                if (game->scenario.activeScenario <= 1) {
                    if (game->scenario.difficultySlot == 0) { game->scenario.aiFrequency = 32; game->scenario.aiThreshold = 29; }
                    if (game->scenario.difficultySlot == 1) { game->scenario.aiFrequency = 25; game->scenario.aiThreshold = 22; }
                    if (game->scenario.difficultyA == 0) { game->economy.interestRate = 4; game->economy.enterpriseTaxPct = 2; }
                    if (game->scenario.difficultyA == 1) { game->economy.interestRate = 8; game->economy.enterpriseTaxPct = 5; }
                    if (game->scenario.difficultyB == 0) { game->world.mapGenColumns /= 2; game->world.mapGenRows /= 2; }
                    if (game->scenario.difficultyC == 0) {
                        game->world.terrainFeatureStage7Count /= 2; game->world.terrainFeatureStage6Count /= 2;
                        game->world.weightAccel /= 2; game->world.weightAccel2 /= 2; game->world.weightAccel3 /= 2; game->world.weightAccel4 /= 2;
                    }
                }
            }
            if (game->stateWorkStep == 3) {
                game->ui.buildMenuItemIndex = 0;
                game->ui.buildMenuMode = 0;
                game_loadState25Setup(game, 0);
            }
            if (game->stateWorkStep <= 3) break;
            game_loadState25Setup(game, 1);
            if (game->world.mapGenTilesDone != game->world.totalTilesToGenerate) break;
            game->world.mapGenTilesDone = 0;
            game_setState(game, GameState::Playing);
            break;
        }
        case GameState::Playing: {
            game_simulationTick(game);
            break;
        }
        case GameState::BuildingMinimap: {
            if (game->stateWorkStep > 0) --game->stateWorkStep;
            if (game->stateWorkStep != 2) break;
            buildMinimapOverview(game);
            break;
        }
        case GameState::MapOverview:
            game_viewportPanKey(game);
            break;
        case GameState::BuildingPath:
            if (game->stateWorkStep > 0) --game->stateWorkStep;
            if (game->stateWorkStep != 2) break;
            if (game_tryBuildPath(game)) {
                game_setState(game, GameState::Playing);
                game->ui.buildMenuItemIndex = 0;
                game->build.buildCancelled = true;
                game->ui.buildMenuMode = 2;
                game->ui.menuDirty = true;
                break;
            }
            game->ui.buildMenuItemIndex = 0;
            game->build.buildCancelled = true;
            game->ui.buildMenuMode = 1;
            game->ui.menuDirty = true;
            game->ready = false;
            game->ui.transitionCode = 12;
            game_setState(game, GameState::MenuOverlay);
            break;
        case GameState::SavingGame:
            if (game->stateWorkStep > 0) --game->stateWorkStep;
            if (game->stateWorkStep != 2) break;
            game_saveAllMapArrays(game);
            game->ui.buildMenuItemIndex = 0;
            game->ui.menuDirty = true;
            game_setState(game, GameState::Playing);
            break;
        case GameState::LoadingGame:
            if (game->stateWorkStep > 0) --game->stateWorkStep;
            if (game->stateWorkStep != 2) break;
            game_restoreAllMapArrays(game);
            game->ui.pauseFlag = false;
            game->ui.buildMenuItemIndex = 0;
            game->ui.menuDirty = true;
            game_setState(game, GameState::Playing);
            break;
        case GameState::AiTurn:
            if (game->stateWorkStep > 0) --game->stateWorkStep;
            if (game->stateWorkStep != 2) break;
            if (game->build.nextTrackPieceCode - track::AI_PIECE_FIRST < game->scenario.difficultyCycle) {
                game_placeCitiesAndIndustry(game);
                game_setState(game, GameState::Playing);
                break;
            }
            ++game->build.nextTrackPieceCode;
            if (game->build.nextTrackPieceCode == track::AI_PIECE_WRAP) game->build.nextTrackPieceCode = track::AI_PIECE_FIRST;
            game_setState(game, GameState::Playing);
            break;
        case GameState::HighScoreSync:
            if (game->stateWorkStep > 0) --game->stateWorkStep;
            if (game->stateWorkStep != 2) break;
            game->ui.transitionCode = game_syncHighScores(game, game->scenario.highScoreUploadRequested)
                                 ? 19 : 18;
            game_setState(game, GameState::MenuOverlay);
            break;
    }
}
