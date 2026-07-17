// Game paint state machine.
#include "game.h"

#define RGB3(r, g, b) (((r) << 16) | ((g) << 8) | (b))
#define WIDTH  MIDP_SCREEN_W   // this.getWidth()
#define HEIGHT MIDP_SCREEN_H   // this.getHeight()

void Game::paint_map_overview(Surface& surface) {
    Game *game = this;
    Surface *g = &surface;
    midp_set_clip(g, 0, 15, game->renderer.canvasWidth, game->renderer.gameplayViewportHeight);
    midp_set_color(g, 0);
    midp_fill_rect(g, 0, 15, game->renderer.canvasWidth, game->renderer.gameplayViewportHeight);
    int viewportMapX = game->renderer.camera.viewportCenterX / 16;
    int viewportMapY = game->renderer.camera.viewportCenterY / 8;
    int overviewSize = game->world.terrain.mapWidth + game->world.terrain.mapHeight;
    int drawX = game->renderer.canvasWidth / 2 - viewportMapX - game->renderer.canvasWidth / 32;
    if (game->renderer.canvasWidth - drawX > overviewSize) drawX = game->renderer.canvasWidth - overviewSize;
    if (drawX > 0) drawX = 0;
    int drawY = game->renderer.gameplayViewportHeight / 2 - viewportMapY - game->renderer.gameplayViewportHeight / 16;
    if (game->renderer.gameplayViewportHeight - drawY > overviewSize) drawY = game->renderer.gameplayViewportHeight - overviewSize;
    if (drawY > 0) drawY = 0;
    int centerX = overviewSize < game->renderer.canvasWidth ? (game->renderer.canvasWidth - overviewSize) / 2 : 0;
    int centerY = overviewSize < game->renderer.gameplayViewportHeight ? (game->renderer.gameplayViewportHeight - overviewSize) / 2 : 0;
    midp_draw_image(g, game->renderer.backbuffer.get(), drawX + centerX, 15 + drawY + centerY, 20);
    int mapX = game->renderer.camera.viewportCenterX - 2 * game->renderer.camera.viewportCenterY + 16 + 2 * game->world.terrain.mapHalfWidth;
    int mapY = game->renderer.camera.viewportCenterX + 2 * game->renderer.camera.viewportCenterY - 16 - 2 * game->world.terrain.mapHalfWidth;
    int boxY = game->world.terrain.mapWidth - (mapX /= 32) + (mapY /= 32);
    int boxX = mapX + mapY;
    midp_set_color(g, 0xFFFFFF);
    midp_draw_rect(g, boxX + drawX + centerX, 15 + boxY + drawY + centerY,
                   game->renderer.canvasWidth / 16, game->renderer.gameplayViewportHeight / 8);
    midp_set_clip(g, 0, 0, game->renderer.canvasWidth, game->renderer.canvasHeight);
    game->draw_confirm_bar(*g);
    game->draw_confirm_dialog(*g);
    if (game->confirmCloseRequested) {
        game->ready = false;
        game->ui.transitionCode = 36;
        game->set_state(GameState::MenuOverlay);
        game->confirmCloseRequested = false;
    }
    if (game->ui.pendingTransitionCode <= -1) return;
    game->ui.transitionCode = game->ui.pendingTransitionCode;
    game->reset_overlay_render_state();
    game->ui.pendingTransitionCode = -1;
    game->render_packed_sprite(*g, game->ui.transitionCode);
    game->set_state(GameState::MenuOverlay);
}

void Game::render(Surface& surface) {
    Game* game = this;
    Surface* g = &surface;
    // setFullScreenMode(true) is a no-op on this backend.
    int n = WIDTH, n2 = HEIGHT;
    game->clickMap.clear();  // rebuild overlay hit-rects fresh each frame
    game->set_canvas_size(n, n2);
    if (game->gameState != GameState::MapOverview && game->gameState != GameState::BuildingMinimap && game->gameState != GameState::BuildingPath
        && game->gameState != GameState::SavingGame && game->gameState != GameState::LoadingGame && game->gameState != GameState::AiTurn
        && game->gameState != GameState::HighScoreSync && (game->gameState != GameState::MenuOverlay || game->ready)) {
        midp_set_color(g, 0);
        midp_fill_rect(g, 0, 0, n, n2);
    }
    midp_set_clip(g, 0, 0, game->renderer.canvasWidth, game->renderer.canvasHeight);
    switch (game->gameState) {
        case GameState::BootSplash0: case GameState::BootSplash1: case GameState::BootSplash2: case GameState::BootSplash3: {
            midp_set_color(g, 0xFFFFFF);
            if (game->gameState == GameState::BootSplash0) midp_set_color(g, 0);
            midp_fill_rect(g, 0, 0, game->renderer.canvasWidth, game->renderer.canvasHeight);
            int n3 = game->renderer.canvasWidth / 2;
            int n4 = game->renderer.canvasHeight / 2;
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
            midp_set_clip(g, 0, 0, game->renderer.canvasWidth, game->renderer.canvasHeight);
            break;
        }
        case GameState::LoadingResources: {
            midp_set_color(g, 0xFFFFFF);
            midp_fill_rect(g, 0, 0, game->renderer.canvasWidth, game->renderer.canvasHeight);
            midp_draw_image(g, game->assets.cycle_flag.get(), game->renderer.canvasWidth / 2, game->renderer.canvasHeight / 2, 3);
            midp_set_color(g, RGB3(215, 222, 238));
            midp_fill_rect(g, 10, game->renderer.canvasHeight - 13, game->renderer.canvasWidth - 21, 9);
            int n10 = game->assets.spriteLoadCounter * (game->renderer.canvasWidth - 20) / (game->scenario.scenarioResourceCount - 1);
            midp_set_color(g, RGB3(60, 105, 144));
            midp_fill_rect(g, 10, game->renderer.canvasHeight - 13, n10, 9);
            midp_set_color(g, RGB3(49, 64, 90));
            midp_draw_rect(g, 10, game->renderer.canvasHeight - 13, game->renderer.canvasWidth - 21, 9);
            break;
        }
        case GameState::MenuOverlay: {
            if (game->ready) {
                midp_set_color(g, 0xFFFFFF);
                midp_fill_rect(g, 0, 0, game->renderer.canvasWidth, game->renderer.canvasHeight);
                if (game->assets.cycle_flag != NULL)
                    midp_draw_image(g, game->assets.cycle_flag.get(), game->renderer.canvasWidth / 2, game->renderer.canvasHeight / 2, 3);
            }
            if (game->ui.transitionCode == 36) {
                game->gameState = GameState::MapOverview;
                game->render(*g);
                game->gameState = GameState::MenuOverlay;
            }
            game->render_packed_sprite(*g, game->ui.transitionCode);
            break;
        }
        case GameState::GeneratingMap: {
            midp_set_color(g, 0);
            midp_fill_rect(g, 0, 0, game->renderer.canvasWidth, game->renderer.canvasHeight);
            game->renderer.draw_text_box(*g, game->assets, game->renderer.canvasWidth / 2, game->renderer.canvasHeight / 2, 0, 3,
                           game->assets.index_text_codes(0));
            midp_set_color(g, RGB3(215, 222, 238));
            midp_fill_rect(g, 10, game->renderer.canvasHeight - 23, game->renderer.canvasWidth - 21, 9);
            int n11 = game->world.totalTilesToGenerate < 2 ? 2 : game->world.totalTilesToGenerate;
            int n12 = game->world.mapGenTilesDone * (game->renderer.canvasWidth - 20) / (n11 - 1);
            midp_set_color(g, RGB3(60, 105, 144));
            midp_fill_rect(g, 10, game->renderer.canvasHeight - 23, n12, 9);
            midp_set_color(g, 0xFFFFFF);
            midp_draw_rect(g, 10, game->renderer.canvasHeight - 23, game->renderer.canvasWidth - 21, 9);
            break;
        }
        case GameState::Playing: {
            game->render_map(*g);
            if (game->ui.inputPending) {
                int n13;
                midp_set_color(g, 0);
                midp_set_clip(g, 0, 15, game->renderer.canvasWidth, game->renderer.bottomBarY - 15);
                for (n13 = 0; n13 < game->renderer.canvasWidth; ++n13)
                    midp_draw_line(g, n13 * 2, 0, game->renderer.canvasHeight + n13 * 2, game->renderer.canvasHeight);
                for (n13 = 0; n13 < game->renderer.canvasHeight; ++n13)
                    midp_draw_line(g, 0, n13 * 2, game->renderer.canvasWidth, game->renderer.canvasWidth + n13 * 2);
                midp_set_clip(g, 0, 0, game->renderer.canvasWidth, game->renderer.canvasHeight);
                game->renderer.draw_sprite_quad(*g, game->assets, 234, 0, 15, 20);
            }
            game->draw_money_ticker(*g);
            if (game->ui.pauseFlag) game->renderer.draw_sprite_quad(*g, game->assets, 236, 0, 15, 0);
            if (game->ui.pendingTransitionCode > -1) {
                game->ui.transitionCode = game->ui.pendingTransitionCode;
                game->reset_overlay_render_state();
                game->ui.pendingTransitionCode = -1;
                game->render_packed_sprite(*g, game->ui.transitionCode);
                game->set_state(GameState::MenuOverlay);
            }
            if (game->ui.transitionDebounce <= 0) break;
            game->ui.transitionDebounce = (int8_t)(game->ui.transitionDebounce - 1);
            if (game->ui.transitionDebounce != 0) break;
            game->ui.transitionCode = transition::SCENARIO_NOTICE_BASE + game->scenario.scenarioNoticeCode;
            game->reset_overlay_render_state();
            game->ready = false;
            game->render_packed_sprite(*g, game->ui.transitionCode);
            game->set_state(GameState::MenuOverlay);
            break;
        }
        case GameState::BuildingMinimap:
            game->renderer.draw_text_box(*g, game->assets, game->renderer.canvasWidth / 2, game->renderer.canvasHeight / 2, 0, 3,
                           game->assets.index_text_codes(50));
            break;
        case GameState::MapOverview:
            game->paint_map_overview(*g);
            break;
        case GameState::BuildingPath:
            game->renderer.draw_text_box(*g, game->assets, game->renderer.canvasWidth / 2, game->renderer.canvasHeight / 2, 0, 3,
                           game->assets.index_text_codes(51));
            break;
        case GameState::SavingGame:
            game->renderer.draw_text_box(*g, game->assets, game->renderer.canvasWidth / 2, game->renderer.canvasHeight / 2, 0, 3,
                           game->assets.index_text_codes(52));
            break;
        case GameState::LoadingGame:
            game->renderer.draw_text_box(*g, game->assets, game->renderer.canvasWidth / 2, game->renderer.canvasHeight / 2, 0, 3,
                           game->assets.index_text_codes(53));
            break;
        case GameState::AiTurn:
            game->renderer.draw_text_box(*g, game->assets, game->renderer.canvasWidth / 2, game->renderer.canvasHeight / 2, 0, 3,
                           game->assets.index_text_codes(49));
            break;
        case GameState::HighScoreSync:
            game->renderer.draw_text_box(*g, game->assets, game->renderer.canvasWidth / 2, game->renderer.canvasHeight / 2, 0, 3,
                           game->assets.index_text_codes(66));
            break;
    }
}
