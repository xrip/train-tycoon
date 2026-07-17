// Game input state and actions.
#include "game_internal.h"

#include <stdlib.h>

void Game::key_released(int32_t n) {
    Game* game = this;
    (void)n;
    game->ui.currentKey = -999;
}

void UiController::reset_key_state(int32_t n) {
    (void)n;
    currentKey = 0;
    lastKeyCode = 0;
    keySubState = 0;
    keyRepeatDelay = 0;
    heldKey = 0;
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
    confirmCounter = 20;
}

void Camera::pan(int32_t key, int32_t map_screen_width,
                 int32_t map_screen_height, int32_t canvas_width,
                 int32_t viewport_height) {
    if (key == 56 || key == 6) viewportCenterY += 30;
    if (key == 50 || key == 1) viewportCenterY -= 30;
    if (key == 54 || key == 5) viewportCenterX += 60;
    if (key == 52 || key == 2) viewportCenterX -= 60;
    if (key == 49) {
        viewportCenterY -= 30;
        viewportCenterX -= 60;
    }
    if (key == 51) {
        viewportCenterY -= 30;
        viewportCenterX += 60;
    }
    if (key == 55) {
        viewportCenterY += 30;
        viewportCenterX -= 60;
    }
    if (key == 57) {
        viewportCenterY += 30;
        viewportCenterX += 60;
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
    int step = game->ui.advance_key_repeat();
    int key = game->ui.lastKeyCode;
    if (key == 56 || key == 6) game->renderer.camera.mapScaleY += step * 2;
    if (key == 50 || key == 1) game->renderer.camera.mapScaleY -= step * 2;
    if (key == 54 || key == 5) game->renderer.camera.mapScaleX += step * 2;
    if (key == 52 || key == 2) game->renderer.camera.mapScaleX -= step * 2;
    if (key == 49) {
        game->renderer.camera.mapScaleY -= step;
        game->renderer.camera.mapScaleX -= step * 2;
    }
    if (key == 51) {
        game->renderer.camera.mapScaleY -= step;
        game->renderer.camera.mapScaleX += step * 2;
    }
    if (key == 55) {
        game->renderer.camera.mapScaleY += step;
        game->renderer.camera.mapScaleX -= step * 2;
    }
    if (key == 57) {
        game->renderer.camera.mapScaleY += step;
        game->renderer.camera.mapScaleX += step * 2;
    }

    game->renderer.camera.viewportBaseX = game->renderer.camera.mapScaleX / 2;
    game->renderer.camera.viewportBaseY = game->renderer.camera.mapScaleY / 2;
    if (game->renderer.camera.viewportBaseX > game->world.terrain.mapScreenWidth - 8)
        game->renderer.camera.viewportBaseX = game->world.terrain.mapScreenWidth - 8;
    if (game->renderer.camera.viewportBaseY > game->world.terrain.mapScreenHeight - 4)
        game->renderer.camera.viewportBaseY = game->world.terrain.mapScreenHeight - 4;

    bool moved_view = false;
    if (game->renderer.camera.viewportCenterX > 0
        && game->renderer.camera.viewportBaseX - game->renderer.camera.viewportCenterX <= 10) {
        moved_view = true;
        game->renderer.camera.viewportCenterX = game->renderer.camera.viewportBaseX - 11;
    }
    if (game->renderer.camera.viewportCenterY > 0
        && game->renderer.camera.viewportBaseY - game->renderer.camera.viewportCenterY <= 10) {
        moved_view = true;
        game->renderer.camera.viewportCenterY = game->renderer.camera.viewportBaseY - 11;
    }
    if (game->renderer.camera.viewportBaseX - game->renderer.camera.viewportCenterX >= canvasWidth - 20) {
        moved_view = true;
        game->renderer.camera.viewportCenterX = game->renderer.camera.viewportBaseX - canvasWidth + 21;
    }
    if (game->renderer.camera.viewportBaseY - game->renderer.camera.viewportCenterY
        >= gameplayViewportHeight - 20) {
        moved_view = true;
        game->renderer.camera.viewportCenterY =
            game->renderer.camera.viewportBaseY - gameplayViewportHeight + 21;
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
    if (game->renderer.camera.viewportCenterX > game->world.terrain.mapScreenWidth - canvasWidth) {
        game->renderer.camera.viewportCenterX = game->world.terrain.mapScreenWidth - canvasWidth;
        moved_view = false;
    }
    if (game->renderer.camera.viewportCenterY
        > game->world.terrain.mapScreenHeight - gameplayViewportHeight) {
        game->renderer.camera.viewportCenterY =
            game->world.terrain.mapScreenHeight - gameplayViewportHeight;
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
            game_computeIsoCorners(game, x, y, game->build.selectionTileX,
                                        game->build.selectionTileY, false);
            int opposite_edges =
                game_crossProduct2D(anchorX, anchorY,
                                  game->build.selectionTileX[0], game->build.selectionTileY[0],
                                  game->build.selectionTileX[1], game->build.selectionTileY[1])
                * game_crossProduct2D(anchorX, anchorY,
                                    game->build.selectionTileX[2], game->build.selectionTileY[2],
                                    game->build.selectionTileX[3], game->build.selectionTileY[3]);
            int other_edges =
                game_crossProduct2D(anchorX, anchorY,
                                  game->build.selectionTileX[0], game->build.selectionTileY[0],
                                  game->build.selectionTileX[2], game->build.selectionTileY[2])
                * game_crossProduct2D(anchorX, anchorY,
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
        int dx = anchorX - game->build.selectionTileX[corner];
        int dy = anchorY - game->build.selectionTileY[corner];
        corner_distance[corner] = abs(dx * dx + dy * dy);
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
