// Game paint state machine.
#include "game_internal.h"

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
    int overviewSize = game->world.terrain.mapWidth + game->world.terrain.mapHeight;
    int drawX = canvasWidth / 2 - viewportMapX - canvasWidth / 32;
    if (canvasWidth - drawX > overviewSize) drawX = canvasWidth - overviewSize;
    if (drawX > 0) drawX = 0;
    int drawY = gameplayViewportHeight / 2 - viewportMapY - gameplayViewportHeight / 16;
    if (gameplayViewportHeight - drawY > overviewSize) drawY = gameplayViewportHeight - overviewSize;
    if (drawY > 0) drawY = 0;
    int centerX = overviewSize < canvasWidth ? (canvasWidth - overviewSize) / 2 : 0;
    int centerY = overviewSize < gameplayViewportHeight ? (gameplayViewportHeight - overviewSize) / 2 : 0;
    midp_draw_image(g, game->renderer.backbuffer.get(), drawX + centerX, 15 + drawY + centerY, 20);
    int mapX = game->renderer.viewportCenterX - 2 * game->renderer.viewportCenterY + 16 + 2 * game->world.terrain.mapHalfWidth;
    int mapY = game->renderer.viewportCenterX + 2 * game->renderer.viewportCenterY - 16 - 2 * game->world.terrain.mapHalfWidth;
    int boxY = game->world.terrain.mapWidth - (mapX /= 32) + (mapY /= 32);
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
