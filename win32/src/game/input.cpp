// Game input state and actions.
#include "game.h"

#include <stdlib.h>

namespace {

namespace camera_motion {
constexpr int32_t kPanVerticalPixels = 30;
constexpr int32_t kPanHorizontalPixels = 60;
constexpr int32_t kZoomEdgeDistance = 10;
constexpr int32_t kTopViewportMargin = 8;
constexpr int32_t kSideViewportMargin = 4;
constexpr int32_t kViewportEdgeMargin = 20;
constexpr int32_t kViewportCorrection = 11;
constexpr int32_t kViewportCorrectionWithCanvas = 21;
}  // namespace camera_motion

namespace ui_timing {
constexpr int32_t kConfirmCounterStart = 20;
}  // namespace ui_timing

int32_t cross_product_2d(int32_t origin_x, int32_t origin_y,
                         int32_t point_a_x, int32_t point_a_y,
                         int32_t point_b_x, int32_t point_b_y) {
    return (point_a_x - origin_x) * (origin_y - point_b_y)
         - (point_a_y - origin_y) * (origin_x - point_b_x);
}

}  // namespace

void Game::key_released(int32_t key_code) {
    Game* game = this;
    (void)key_code;
    game->ui.currentKey = input_key::Released;
}

void UiController::reset_key_state(int32_t key_code) {
    (void)key_code;
    currentKey = input_key::NoKey;
    lastKeyCode = input_key::NoKey;
    keySubState = 0;
    keyRepeatDelay = 0;
    heldKey = input_key::NoKey;
    inputSubState = 0;
}

void UiController::cycle_build_menu_index(bool forward) {
    buildMenuItemIndex += forward ? 1 : -1;
    int item_count = static_cast<int>(buildMenuItems[buildMenuMode].size());
    if (buildMenuItemIndex < 0) buildMenuItemIndex = item_count - 1;
    if (buildMenuItemIndex == item_count) buildMenuItemIndex = 0;
    confirmTargetState =
        buildMenuResultCodes[buildMenuMode][buildMenuItemIndex];
    menuDirty = true;
    confirmCounter = ui_timing::kConfirmCounterStart;
}

void Camera::pan(int32_t input_key, int32_t map_screen_width,
                 int32_t map_screen_height, int32_t canvas_width,
                 int32_t viewport_height) {
    if (input_key == keypad_key::kDown || input_key == game_action::kDown)
        viewportCenterY += camera_motion::kPanVerticalPixels;
    if (input_key == keypad_key::kUp || input_key == game_action::kUp)
        viewportCenterY -= camera_motion::kPanVerticalPixels;
    if (input_key == keypad_key::kRight || input_key == game_action::kRight)
        viewportCenterX += camera_motion::kPanHorizontalPixels;
    if (input_key == keypad_key::kLeft || input_key == game_action::kLeft)
        viewportCenterX -= camera_motion::kPanHorizontalPixels;
    if (input_key == keypad_key::kUpLeft) {
        viewportCenterY -= camera_motion::kPanVerticalPixels;
        viewportCenterX -= camera_motion::kPanHorizontalPixels;
    }
    if (input_key == keypad_key::kUpRight) {
        viewportCenterY -= camera_motion::kPanVerticalPixels;
        viewportCenterX += camera_motion::kPanHorizontalPixels;
    }
    if (input_key == keypad_key::kDownLeft) {
        viewportCenterY += camera_motion::kPanVerticalPixels;
        viewportCenterX -= camera_motion::kPanHorizontalPixels;
    }
    if (input_key == keypad_key::kDownRight) {
        viewportCenterY += camera_motion::kPanVerticalPixels;
        viewportCenterX += camera_motion::kPanHorizontalPixels;
    }
    if (viewportCenterX < 0) viewportCenterX = 0;
    if (viewportCenterY < 0) viewportCenterY = 0;
    if (viewportCenterX > map_screen_width - canvas_width)
        viewportCenterX = map_screen_width - canvas_width;
    if (viewportCenterY > map_screen_height - viewport_height)
        viewportCenterY = map_screen_height - viewport_height;
    viewportBaseX = viewportCenterX + canvas_width / 2;
    viewportBaseY = viewportCenterY + viewport_height / 2;
    mapScaleX = viewportBaseX * 2;
    mapScaleY = viewportBaseY * 2;
}

void Game::viewport_zoom_key() {
    Game* game = this;
    const int zoom_step = game->ui.advance_key_repeat();
    const int input_key = game->ui.lastKeyCode;
    if (input_key == keypad_key::kDown || input_key == game_action::kDown)
        game->renderer.camera.mapScaleY += zoom_step * 2;
    if (input_key == keypad_key::kUp || input_key == game_action::kUp)
        game->renderer.camera.mapScaleY -= zoom_step * 2;
    if (input_key == keypad_key::kRight || input_key == game_action::kRight)
        game->renderer.camera.mapScaleX += zoom_step * 2;
    if (input_key == keypad_key::kLeft || input_key == game_action::kLeft)
        game->renderer.camera.mapScaleX -= zoom_step * 2;
    if (input_key == keypad_key::kUpLeft) {
        game->renderer.camera.mapScaleY -= zoom_step;
        game->renderer.camera.mapScaleX -= zoom_step * 2;
    }
    if (input_key == keypad_key::kUpRight) {
        game->renderer.camera.mapScaleY -= zoom_step;
        game->renderer.camera.mapScaleX += zoom_step * 2;
    }
    if (input_key == keypad_key::kDownLeft) {
        game->renderer.camera.mapScaleY += zoom_step;
        game->renderer.camera.mapScaleX -= zoom_step * 2;
    }
    if (input_key == keypad_key::kDownRight) {
        game->renderer.camera.mapScaleY += zoom_step;
        game->renderer.camera.mapScaleX += zoom_step * 2;
    }

    game->renderer.camera.viewportBaseX = game->renderer.camera.mapScaleX / 2;
    game->renderer.camera.viewportBaseY = game->renderer.camera.mapScaleY / 2;
    if (game->renderer.camera.viewportBaseX
        > game->world.terrain.mapScreenWidth - camera_motion::kTopViewportMargin)
        game->renderer.camera.viewportBaseX =
            game->world.terrain.mapScreenWidth - camera_motion::kTopViewportMargin;
    if (game->renderer.camera.viewportBaseY
        > game->world.terrain.mapScreenHeight - camera_motion::kSideViewportMargin)
        game->renderer.camera.viewportBaseY =
            game->world.terrain.mapScreenHeight - camera_motion::kSideViewportMargin;

    bool moved_view = false;
    if (game->renderer.camera.viewportCenterX > 0
        && game->renderer.camera.viewportBaseX - game->renderer.camera.viewportCenterX
               <= camera_motion::kZoomEdgeDistance) {
        moved_view = true;
        game->renderer.camera.viewportCenterX =
            game->renderer.camera.viewportBaseX - camera_motion::kViewportCorrection;
    }
    if (game->renderer.camera.viewportCenterY > 0
        && game->renderer.camera.viewportBaseY - game->renderer.camera.viewportCenterY
               <= camera_motion::kZoomEdgeDistance) {
        moved_view = true;
        game->renderer.camera.viewportCenterY =
            game->renderer.camera.viewportBaseY - camera_motion::kViewportCorrection;
    }
    if (game->renderer.camera.viewportBaseX - game->renderer.camera.viewportCenterX
        >= game->renderer.canvasWidth - camera_motion::kViewportEdgeMargin) {
        moved_view = true;
        game->renderer.camera.viewportCenterX = game->renderer.camera.viewportBaseX
            - game->renderer.canvasWidth
            + camera_motion::kViewportCorrectionWithCanvas;
    }
    if (game->renderer.camera.viewportBaseY - game->renderer.camera.viewportCenterY
        >= game->renderer.gameplayViewportHeight - camera_motion::kViewportEdgeMargin) {
        moved_view = true;
        game->renderer.camera.viewportCenterY =
            game->renderer.camera.viewportBaseY
            - game->renderer.gameplayViewportHeight
            + camera_motion::kViewportCorrectionWithCanvas;
    }
    if (game->renderer.camera.viewportBaseX < 0) game->renderer.camera.viewportBaseX = 0;
    if (game->renderer.camera.viewportBaseY < 0) game->renderer.camera.viewportBaseY = 0;
    if (game->renderer.camera.viewportCenterX < 0) {
        game->renderer.camera.viewportCenterX = 0;
        moved_view = false;
    }
    if (game->renderer.camera.viewportCenterY < 0) {
        game->renderer.camera.viewportCenterY = 0;
        moved_view = false;
    }
    if (game->renderer.camera.viewportCenterX > game->world.terrain.mapScreenWidth - game->renderer.canvasWidth) {
        game->renderer.camera.viewportCenterX = game->world.terrain.mapScreenWidth - game->renderer.canvasWidth;
        moved_view = false;
    }
    if (game->renderer.camera.viewportCenterY
        > game->world.terrain.mapScreenHeight - game->renderer.gameplayViewportHeight) {
        game->renderer.camera.viewportCenterY =
            game->world.terrain.mapScreenHeight - game->renderer.gameplayViewportHeight;
        moved_view = false;
    }
    if (moved_view) game->ui.keySubState = 80;
    compute_viewport_bounds(game->renderer.camera.viewportBaseX,
                            game->renderer.camera.viewportBaseY);
    game->renderer.camera.mapScaleX = game->renderer.camera.viewportBaseX * 2;
    game->renderer.camera.mapScaleY = game->renderer.camera.viewportBaseY * 2;
}

void Game::compute_viewport_bounds(int32_t anchorX, int32_t anchorY) {
    Game* game = this;
    int tile_center_x = 0;
    int tile_center_y = 0;
    int tile_x = anchorX - 2 * anchorY
                 + 16 + 2 * game->world.terrain.mapHalfWidth;
    int tile_y = anchorX + 2 * anchorY
                 - 16 - 2 * game->world.terrain.mapHalfWidth;
    tile_x /= 32;
    tile_y /= 32;
    game->ui.blockBuildMenu = true;

    for (int x = tile_x - 1; x <= tile_x + 1 && game->ui.blockBuildMenu; ++x) {
        for (int y = tile_y - 1; y <= tile_y + 1; ++y) {
            if (!game->world.in_bounds(x, y))
                continue;
            tile_center_x = y * 16 + x * 16 + 16;
            tile_center_y = game->world.terrain.mapHalfWidth - x * 8 + y * 8 + 8;
            game->compute_iso_corners(x, y, game->build.selectionTileX,
                                        game->build.selectionTileY, false);
            int opposite_edges =
                cross_product_2d(anchorX, anchorY,
                                  game->build.selectionTileX[0], game->build.selectionTileY[0],
                                  game->build.selectionTileX[1], game->build.selectionTileY[1])
                * cross_product_2d(anchorX, anchorY,
                                    game->build.selectionTileX[2], game->build.selectionTileY[2],
                                    game->build.selectionTileX[3], game->build.selectionTileY[3]);
            int other_edges =
                cross_product_2d(anchorX, anchorY,
                                  game->build.selectionTileX[0], game->build.selectionTileY[0],
                                  game->build.selectionTileX[2], game->build.selectionTileY[2])
                * cross_product_2d(anchorX, anchorY,
                                    game->build.selectionTileX[1], game->build.selectionTileY[1],
                                    game->build.selectionTileX[3], game->build.selectionTileY[3]);
            if (opposite_edges > 0 || other_edges > 0) continue;
            tile_x = x;
            tile_y = y;
            game->ui.blockBuildMenu = false;
            break;
        }
    }
    if (game->ui.blockBuildMenu) return;

    game->build.swapMapX = tile_x;
    game->build.swapMapY = tile_y;
    game->ui.tileCursorX = tile_x;
    game->ui.tileCursorY = tile_y;
    int corner_distance[4];
    for (int corner = 0; corner < 4; ++corner) {
        const int distance_x = anchorX - game->build.selectionTileX[corner];
        const int distance_y = anchorY - game->build.selectionTileY[corner];
        corner_distance[corner] = abs(distance_x * distance_x
                                      + distance_y * distance_y);
    }
    int nearest = corner_distance[0];
    for (int corner = 1; corner < 4; ++corner)
        if (corner_distance[corner] < nearest) nearest = corner_distance[corner];
    game->ui.vehicleMenuSelectIndex = corner_distance[0] < corner_distance[3];
    game->ui.trainMenuSelectIndex = corner_distance[1] < corner_distance[2];
    if (nearest == corner_distance[0]) {
        tile_center_x -= 16;
    } else if (nearest == corner_distance[1]) {
        tile_center_y -= 8;
        ++game->build.swapMapX;
    } else if (nearest == corner_distance[2]) {
        tile_center_y += 8;
        ++game->build.swapMapY;
    } else {
        tile_center_x += 16;
        ++game->build.swapMapX;
        ++game->build.swapMapY;
    }
    tile_center_y -= game->renderer.camera.viewportCenterY
                     + 4 * game->world.terrain.heightMap[game->build.swapMapX][game->build.swapMapY];
    game->ui.cursorScreenX = tile_center_x - game->renderer.camera.viewportCenterX;
    game->ui.cursorScreenY = tile_center_y;
    for (int corner = 0; corner < 4; ++corner) {
        game->build.selectionTileX[corner] -= game->renderer.camera.viewportCenterX;
        game->build.selectionTileY[corner] -= game->renderer.camera.viewportCenterY;
    }
}
