// Game paint state machine.
#include "game.h"

#define RGB3(red, green, blue) (((red) << 16) | ((green) << 8) | (blue))

namespace {

namespace paint_layout {
constexpr int32_t kTopStripHeight = 15;
constexpr int32_t kProgressBarX = 10;
constexpr int32_t kProgressBarHeight = 9;
constexpr int32_t kProgressBarSidePadding = 20;
constexpr int32_t kProgressBarBottomPadding = 13;
constexpr int32_t kMapProgressBarBottomPadding = 23;
constexpr int32_t kSplashFrameCount = 10;
constexpr int32_t kSplashFrameSpacing = 5;
constexpr int32_t kSplashFrameDivisor = 10;
constexpr int32_t kSplashFramePadding = 30;
}  // namespace paint_layout

namespace paint_canvas {
constexpr int32_t Width = MIDP_SCREEN_W;
constexpr int32_t Height = MIDP_SCREEN_H;
}  // namespace paint_canvas

namespace paint_sprite {
constexpr int32_t kInputPendingOverlay = 234;
constexpr int32_t kPausedOverlay = 236;
}  // namespace paint_sprite

namespace paint_text {
constexpr int32_t kGeneratingMap = 0;
constexpr int32_t kAiTurn = 49;
constexpr int32_t kBuildingMinimap = 50;
constexpr int32_t kBuildingPath = 51;
constexpr int32_t kSavingGame = 52;
constexpr int32_t kLoadingGame = 53;
constexpr int32_t kHighScoreSync = 66;
}  // namespace paint_text

}  // namespace

void Game::paint_map_overview(Surface& surface) {
    Game *game = this;
    Surface *graphics = &surface;
    midp_set_clip(graphics, 0, 15, game->renderer.canvasWidth, game->renderer.gameplayViewportHeight);
    midp_set_color(graphics, 0);
    midp_fill_rect(graphics, 0, 15, game->renderer.canvasWidth, game->renderer.gameplayViewportHeight);
    const int viewport_map_x = game->renderer.camera.viewportCenterX / 16;
    const int viewport_map_y = game->renderer.camera.viewportCenterY / 8;
    const int overview_size = game->world.terrain.mapWidth
                            + game->world.terrain.mapHeight;
    int draw_x = game->renderer.canvasWidth / 2 - viewport_map_x
               - game->renderer.canvasWidth / 32;
    if (game->renderer.canvasWidth - draw_x > overview_size)
        draw_x = game->renderer.canvasWidth - overview_size;
    if (draw_x > 0) draw_x = 0;
    int draw_y = game->renderer.gameplayViewportHeight / 2 - viewport_map_y
               - game->renderer.gameplayViewportHeight / 16;
    if (game->renderer.gameplayViewportHeight - draw_y > overview_size)
        draw_y = game->renderer.gameplayViewportHeight - overview_size;
    if (draw_y > 0) draw_y = 0;
    const int center_x = overview_size < game->renderer.canvasWidth
        ? (game->renderer.canvasWidth - overview_size) / 2 : 0;
    const int center_y = overview_size < game->renderer.gameplayViewportHeight
        ? (game->renderer.gameplayViewportHeight - overview_size) / 2 : 0;
    midp_draw_image(graphics, game->renderer.backbuffer.get(), draw_x + center_x,
                    paint_layout::kTopStripHeight + draw_y + center_y, 20);
    int map_x = game->renderer.camera.viewportCenterX
              - 2 * game->renderer.camera.viewportCenterY + 16
              + 2 * game->world.terrain.mapHalfWidth;
    int map_y = game->renderer.camera.viewportCenterX
              + 2 * game->renderer.camera.viewportCenterY - 16
              - 2 * game->world.terrain.mapHalfWidth;
    const int box_y = game->world.terrain.mapWidth
        - (map_x /= 32) + (map_y /= 32);
    const int box_x = map_x + map_y;
    midp_set_color(graphics, 0xFFFFFF);
    midp_draw_rect(graphics, box_x + draw_x + center_x,
                   paint_layout::kTopStripHeight + box_y + draw_y + center_y,
                   game->renderer.canvasWidth / 16, game->renderer.gameplayViewportHeight / 8);
    midp_set_clip(graphics, 0, 0, game->renderer.canvasWidth, game->renderer.canvasHeight);
    game->draw_confirm_bar(*graphics);
    game->draw_confirm_dialog(*graphics);
    if (game->confirmCloseRequested) {
        game->ready = false;
        game->ui.transitionCode = menu_screen::MapOverview;
        game->set_state(GameState::MenuOverlay);
        game->confirmCloseRequested = false;
    }
    if (game->ui.pendingTransitionCode <= menu_transition::NoPending) return;
    game->ui.transitionCode = game->ui.pendingTransitionCode;
    game->reset_overlay_render_state();
    game->ui.pendingTransitionCode = menu_transition::NoPending;
    game->render_packed_sprite(*graphics, game->ui.transitionCode);
    game->set_state(GameState::MenuOverlay);
}

void Game::render(Surface& surface) {
    Game* game = this;
    Surface* graphics = &surface;
    // setFullScreenMode(true) is a no-op on this backend.
    const int canvas_width = paint_canvas::Width;
    const int canvas_height = paint_canvas::Height;
    game->clickMap.clear();  // rebuild overlay hit-rects fresh each frame
    game->set_canvas_size(canvas_width, canvas_height);
    if (game->gameState != GameState::MapOverview && game->gameState != GameState::BuildingMinimap && game->gameState != GameState::BuildingPath
        && game->gameState != GameState::SavingGame && game->gameState != GameState::LoadingGame && game->gameState != GameState::AiTurn
        && game->gameState != GameState::HighScoreSync && (game->gameState != GameState::MenuOverlay || game->ready)) {
        midp_set_color(graphics, 0);
        midp_fill_rect(graphics, 0, 0, canvas_width, canvas_height);
    }
    midp_set_clip(graphics, 0, 0, game->renderer.canvasWidth, game->renderer.canvasHeight);
    switch (game->gameState) {
        case GameState::BootSplash0: case GameState::BootSplash1: case GameState::BootSplash2: case GameState::BootSplash3: {
            midp_set_color(graphics, 0xFFFFFF);
            if (game->gameState == GameState::BootSplash0) midp_set_color(graphics, 0);
            midp_fill_rect(graphics, 0, 0, game->renderer.canvasWidth, game->renderer.canvasHeight);
            const int splash_center_x = game->renderer.canvasWidth / 2;
            const int splash_center_y = game->renderer.canvasHeight / 2;
            const int flag_half_width = game->assets.cycle_flag->width() / 2;
            const int flag_height = game->assets.cycle_flag->height();
            const int stripe_height = flag_height / paint_layout::kSplashFrameCount + 1;
            const int splash_travel = splash_center_x + flag_half_width
                                    + paint_layout::kSplashFramePadding;
            for (int stripe_index = 0;
                 stripe_index < paint_layout::kSplashFrameCount; ++stripe_index) {
                int stripe_offset = splash_travel
                    * (game->loadingAnimationFrame
                       - paint_layout::kSplashFrameSpacing * (stripe_index / 2))
                    / paint_layout::kSplashFrameDivisor;
                if (stripe_offset < 0) stripe_offset = 0;
                if (stripe_offset > splash_travel) stripe_offset = splash_travel;
                int stripe_center_x = splash_center_x - (splash_travel - stripe_offset);
                if (stripe_index % 2 == 1)
                    stripe_center_x = splash_center_x + (splash_travel - stripe_offset);
                midp_set_clip(graphics, stripe_center_x - flag_half_width,
                              splash_center_y - flag_height / 2
                                  + stripe_index * stripe_height,
                              flag_half_width * 2, stripe_height);
                midp_draw_image(graphics, game->assets.cycle_flag.get(),
                                stripe_center_x, splash_center_y, 3);
            }
            midp_set_clip(graphics, 0, 0, game->renderer.canvasWidth, game->renderer.canvasHeight);
            break;
        }
        case GameState::LoadingResources: {
            midp_set_color(graphics, 0xFFFFFF);
            midp_fill_rect(graphics, 0, 0, game->renderer.canvasWidth, game->renderer.canvasHeight);
            midp_draw_image(graphics, game->assets.cycle_flag.get(), game->renderer.canvasWidth / 2, game->renderer.canvasHeight / 2, 3);
            midp_set_color(graphics, RGB3(215, 222, 238));
            midp_fill_rect(graphics, paint_layout::kProgressBarX,
                           game->renderer.canvasHeight
                               - paint_layout::kProgressBarBottomPadding,
                           game->renderer.canvasWidth - 21,
                           paint_layout::kProgressBarHeight);
            const int resource_progress_width = game->assets.spriteLoadCounter
                * (game->renderer.canvasWidth - paint_layout::kProgressBarSidePadding)
                / (game->scenario.scenarioResourceCount - 1);
            midp_set_color(graphics, RGB3(60, 105, 144));
            midp_fill_rect(graphics, paint_layout::kProgressBarX,
                           game->renderer.canvasHeight
                               - paint_layout::kProgressBarBottomPadding,
                           resource_progress_width,
                           paint_layout::kProgressBarHeight);
            midp_set_color(graphics, RGB3(49, 64, 90));
            midp_draw_rect(graphics, paint_layout::kProgressBarX,
                           game->renderer.canvasHeight
                               - paint_layout::kProgressBarBottomPadding,
                           game->renderer.canvasWidth - 21,
                           paint_layout::kProgressBarHeight);
            break;
        }
        case GameState::MenuOverlay: {
            if (game->ready) {
                midp_set_color(graphics, 0xFFFFFF);
                midp_fill_rect(graphics, 0, 0, game->renderer.canvasWidth, game->renderer.canvasHeight);
                if (game->assets.cycle_flag != NULL)
                    midp_draw_image(graphics, game->assets.cycle_flag.get(), game->renderer.canvasWidth / 2, game->renderer.canvasHeight / 2, 3);
            }
            if (game->ui.transitionCode == menu_screen::MapOverview) {
                game->gameState = GameState::MapOverview;
                game->render(*graphics);
                game->gameState = GameState::MenuOverlay;
            }
            game->render_packed_sprite(*graphics, game->ui.transitionCode);
            break;
        }
        case GameState::GeneratingMap: {
            midp_set_color(graphics, 0);
            midp_fill_rect(graphics, 0, 0, game->renderer.canvasWidth, game->renderer.canvasHeight);
            game->renderer.draw_text_box(*graphics, game->assets,
                           game->renderer.canvasWidth / 2,
                           game->renderer.canvasHeight / 2, 0, 3,
                           game->assets.index_text_codes(paint_text::kGeneratingMap));
            midp_set_color(graphics, RGB3(215, 222, 238));
            midp_fill_rect(graphics, paint_layout::kProgressBarX,
                           game->renderer.canvasHeight
                               - paint_layout::kMapProgressBarBottomPadding,
                           game->renderer.canvasWidth - 21,
                           paint_layout::kProgressBarHeight);
            const int total_generation_tiles =
                game->world.totalTilesToGenerate < 2
                    ? 2 : game->world.totalTilesToGenerate;
            const int generation_progress_width = game->world.mapGenTilesDone
                * (game->renderer.canvasWidth - paint_layout::kProgressBarSidePadding)
                / (total_generation_tiles - 1);
            midp_set_color(graphics, RGB3(60, 105, 144));
            midp_fill_rect(graphics, paint_layout::kProgressBarX,
                           game->renderer.canvasHeight
                               - paint_layout::kMapProgressBarBottomPadding,
                           generation_progress_width,
                           paint_layout::kProgressBarHeight);
            midp_set_color(graphics, 0xFFFFFF);
            midp_draw_rect(graphics, paint_layout::kProgressBarX,
                           game->renderer.canvasHeight
                               - paint_layout::kMapProgressBarBottomPadding,
                           game->renderer.canvasWidth - 21,
                           paint_layout::kProgressBarHeight);
            break;
        }
        case GameState::Playing: {
            game->render_map(*graphics);
            if (game->ui.inputPending) {
                int stripe_coordinate;
                midp_set_color(graphics, 0);
                midp_set_clip(graphics, 0, 15, game->renderer.canvasWidth, game->renderer.bottomBarY - 15);
                for (stripe_coordinate = 0;
                     stripe_coordinate < game->renderer.canvasWidth;
                     ++stripe_coordinate)
                    midp_draw_line(graphics, stripe_coordinate * 2, 0,
                                   game->renderer.canvasHeight
                                       + stripe_coordinate * 2,
                                   game->renderer.canvasHeight);
                for (stripe_coordinate = 0;
                     stripe_coordinate < game->renderer.canvasHeight;
                     ++stripe_coordinate)
                    midp_draw_line(graphics, 0, stripe_coordinate * 2,
                                   game->renderer.canvasWidth,
                                   game->renderer.canvasWidth
                                       + stripe_coordinate * 2);
                midp_set_clip(graphics, 0, 0, game->renderer.canvasWidth, game->renderer.canvasHeight);
                game->renderer.draw_sprite_quad(*graphics, game->assets,
                                                paint_sprite::kInputPendingOverlay,
                                                0, 15, 20);
            }
            game->draw_money_ticker(*graphics);
            if (game->ui.pauseFlag)
                game->renderer.draw_sprite_quad(*graphics, game->assets,
                                                paint_sprite::kPausedOverlay,
                                                0, 15, 0);
            if (game->ui.pendingTransitionCode > menu_transition::NoPending) {
                game->ui.transitionCode = game->ui.pendingTransitionCode;
                game->reset_overlay_render_state();
                game->ui.pendingTransitionCode = menu_transition::NoPending;
                game->render_packed_sprite(*graphics, game->ui.transitionCode);
                game->set_state(GameState::MenuOverlay);
            }
            if (game->ui.transitionDebounce <= 0) break;
            game->ui.transitionDebounce = (int8_t)(game->ui.transitionDebounce - 1);
            if (game->ui.transitionDebounce != 0) break;
            game->ui.transitionCode = transition::SCENARIO_NOTICE_BASE + game->scenario.scenarioNoticeCode;
            game->reset_overlay_render_state();
            game->ready = false;
            game->render_packed_sprite(*graphics, game->ui.transitionCode);
            game->set_state(GameState::MenuOverlay);
            break;
        }
        case GameState::BuildingMinimap:
            game->renderer.draw_text_box(*graphics, game->assets, game->renderer.canvasWidth / 2, game->renderer.canvasHeight / 2, 0, 3,
                           game->assets.index_text_codes(paint_text::kBuildingMinimap));
            break;
        case GameState::MapOverview:
            game->paint_map_overview(*graphics);
            break;
        case GameState::BuildingPath:
            game->renderer.draw_text_box(*graphics, game->assets, game->renderer.canvasWidth / 2, game->renderer.canvasHeight / 2, 0, 3,
                           game->assets.index_text_codes(paint_text::kBuildingPath));
            break;
        case GameState::SavingGame:
            game->renderer.draw_text_box(*graphics, game->assets, game->renderer.canvasWidth / 2, game->renderer.canvasHeight / 2, 0, 3,
                           game->assets.index_text_codes(paint_text::kSavingGame));
            break;
        case GameState::LoadingGame:
            game->renderer.draw_text_box(*graphics, game->assets, game->renderer.canvasWidth / 2, game->renderer.canvasHeight / 2, 0, 3,
                           game->assets.index_text_codes(paint_text::kLoadingGame));
            break;
        case GameState::AiTurn:
            game->renderer.draw_text_box(*graphics, game->assets, game->renderer.canvasWidth / 2, game->renderer.canvasHeight / 2, 0, 3,
                           game->assets.index_text_codes(paint_text::kAiTurn));
            break;
        case GameState::HighScoreSync:
            game->renderer.draw_text_box(*graphics, game->assets, game->renderer.canvasWidth / 2, game->renderer.canvasHeight / 2, 0, 3,
                           game->assets.index_text_codes(paint_text::kHighScoreSync));
            break;
    }
}
