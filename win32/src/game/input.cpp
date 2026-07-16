// Game input state and actions.
#include "game_internal.h"

#include <stdlib.h>

void game_keyReleased(Game *game, int32_t n) {
    (void)n;
    game->ui.currentKey = -999;
}

void game_resetKeyState(Game *game, int32_t n) {
    (void)n;
    game->ui.currentKey = 0;
    game->ui.lastKeyCode = 0;
    game->ui.keySubState = 0;
    game->ui.keyRepeatDelay = 0;
    game->ui.heldKey = 0;
    game->ui.inputSubState = 0;
}

void game_cycleBuildMenuIndex(Game *game, bool forward) {
    game->ui.buildMenuItemIndex += forward ? 1 : -1;
    int item_count = static_cast<int>(game->ui.buildMenuItems[game->ui.buildMenuMode].size());
    if (game->ui.buildMenuItemIndex < 0) game->ui.buildMenuItemIndex = item_count - 1;
    if (game->ui.buildMenuItemIndex == item_count) game->ui.buildMenuItemIndex = 0;
    game->ui.confirmTargetState =
        game->ui.buildMenuResultCodes[game->ui.buildMenuMode][game->ui.buildMenuItemIndex];
    game->ui.menuDirty = true;
    game->ui.confirmCounter = 20;
}

void game_viewportPanKey(Game *game) {
    int key = game->ui.lastKeyCode;
    if (key == 56 || key == 6) game->renderer.viewportCenterY += 30;
    if (key == 50 || key == 1) game->renderer.viewportCenterY -= 30;
    if (key == 54 || key == 5) game->renderer.viewportCenterX += 60;
    if (key == 52 || key == 2) game->renderer.viewportCenterX -= 60;
    if (key == 49) {
        game->renderer.viewportCenterY -= 30;
        game->renderer.viewportCenterX -= 60;
    }
    if (key == 51) {
        game->renderer.viewportCenterY -= 30;
        game->renderer.viewportCenterX += 60;
    }
    if (key == 55) {
        game->renderer.viewportCenterY += 30;
        game->renderer.viewportCenterX -= 60;
    }
    if (key == 57) {
        game->renderer.viewportCenterY += 30;
        game->renderer.viewportCenterX += 60;
    }
    if (game->renderer.viewportCenterX < 0) game->renderer.viewportCenterX = 0;
    if (game->renderer.viewportCenterY < 0) game->renderer.viewportCenterY = 0;
    if (game->renderer.viewportCenterX > game->world.mapScreenWidth - gcs_screenWidth)
        game->renderer.viewportCenterX = game->world.mapScreenWidth - gcs_screenWidth;
    if (game->renderer.viewportCenterY > game->world.mapScreenHeight - gcs_gameplayViewportHeight)
        game->renderer.viewportCenterY = game->world.mapScreenHeight - gcs_gameplayViewportHeight;
    game->renderer.viewportBaseX = game->renderer.viewportCenterX + gcs_screenWidth / 2;
    game->renderer.viewportBaseY =
        game->renderer.viewportCenterY + gcs_gameplayViewportHeight / 2;
    game->renderer.mapScaleX = game->renderer.viewportBaseX * 2;
    game->renderer.mapScaleY = game->renderer.viewportBaseY * 2;
}

void game_viewportZoomKey(Game *game) {
    int step = game_computeKeyRepeatDelay(game);
    int key = game->ui.lastKeyCode;
    if (key == 56 || key == 6) game->renderer.mapScaleY += step * 2;
    if (key == 50 || key == 1) game->renderer.mapScaleY -= step * 2;
    if (key == 54 || key == 5) game->renderer.mapScaleX += step * 2;
    if (key == 52 || key == 2) game->renderer.mapScaleX -= step * 2;
    if (key == 49) {
        game->renderer.mapScaleY -= step;
        game->renderer.mapScaleX -= step * 2;
    }
    if (key == 51) {
        game->renderer.mapScaleY -= step;
        game->renderer.mapScaleX += step * 2;
    }
    if (key == 55) {
        game->renderer.mapScaleY += step;
        game->renderer.mapScaleX -= step * 2;
    }
    if (key == 57) {
        game->renderer.mapScaleY += step;
        game->renderer.mapScaleX += step * 2;
    }

    game->renderer.viewportBaseX = game->renderer.mapScaleX / 2;
    game->renderer.viewportBaseY = game->renderer.mapScaleY / 2;
    if (game->renderer.viewportBaseX > game->world.mapScreenWidth - 8)
        game->renderer.viewportBaseX = game->world.mapScreenWidth - 8;
    if (game->renderer.viewportBaseY > game->world.mapScreenHeight - 4)
        game->renderer.viewportBaseY = game->world.mapScreenHeight - 4;

    bool moved_view = false;
    if (game->renderer.viewportCenterX > 0
        && game->renderer.viewportBaseX - game->renderer.viewportCenterX <= 10) {
        moved_view = true;
        game->renderer.viewportCenterX = game->renderer.viewportBaseX - 11;
    }
    if (game->renderer.viewportCenterY > 0
        && game->renderer.viewportBaseY - game->renderer.viewportCenterY <= 10) {
        moved_view = true;
        game->renderer.viewportCenterY = game->renderer.viewportBaseY - 11;
    }
    if (game->renderer.viewportBaseX - game->renderer.viewportCenterX >= gcs_screenWidth - 20) {
        moved_view = true;
        game->renderer.viewportCenterX = game->renderer.viewportBaseX - gcs_screenWidth + 21;
    }
    if (game->renderer.viewportBaseY - game->renderer.viewportCenterY
        >= gcs_gameplayViewportHeight - 20) {
        moved_view = true;
        game->renderer.viewportCenterY =
            game->renderer.viewportBaseY - gcs_gameplayViewportHeight + 21;
    }
    if (game->renderer.viewportBaseX < 0) game->renderer.viewportBaseX = 0;
    if (game->renderer.viewportBaseY < 0) game->renderer.viewportBaseY = 0;
    if (game->renderer.viewportCenterX < 0) {
        game->renderer.viewportCenterX = 0;
        moved_view = false;
    }
    if (game->renderer.viewportCenterY < 0) {
        game->renderer.viewportCenterY = 0;
        moved_view = false;
    }
    if (game->renderer.viewportCenterX > game->world.mapScreenWidth - gcs_screenWidth) {
        game->renderer.viewportCenterX = game->world.mapScreenWidth - gcs_screenWidth;
        moved_view = false;
    }
    if (game->renderer.viewportCenterY
        > game->world.mapScreenHeight - gcs_gameplayViewportHeight) {
        game->renderer.viewportCenterY =
            game->world.mapScreenHeight - gcs_gameplayViewportHeight;
        moved_view = false;
    }
    if (moved_view) game->ui.keySubState = 80;
    game_computeViewportBounds(game, game->renderer.viewportBaseX, game->renderer.viewportBaseY);
    game->renderer.mapScaleX = game->renderer.viewportBaseX * 2;
    game->renderer.mapScaleY = game->renderer.viewportBaseY * 2;
}

void game_computeViewportBounds(Game *game, int32_t anchorX, int32_t anchorY) {
    int tile_center_x = 0;
    int tile_center_y = 0;
    int tile_x = anchorX - 2 * anchorY
                 + 16 + 2 * game->world.mapHalfWidth;
    int tile_y = anchorX + 2 * anchorY
                 - 16 - 2 * game->world.mapHalfWidth;
    tile_x /= 32;
    tile_y /= 32;
    game->ui.blockBuildMenu = true;

    for (int x = tile_x - 1; x <= tile_x + 1 && game->ui.blockBuildMenu; ++x) {
        for (int y = tile_y - 1; y <= tile_y + 1; ++y) {
            if (!game->world.in_bounds(x, y))
                continue;
            tile_center_x = y * 16 + x * 16 + 16;
            tile_center_y = game->world.mapHalfWidth - x * 8 + y * 8 + 8;
            game_computeIsoCornersInlined(game, x, y, game->build.selectionTileX,
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
    tile_center_y -= game->renderer.viewportCenterY
                     + 4 * game->world.heightMap[game->build.swapMapX][game->build.swapMapY];
    game->ui.cursorScreenX = tile_center_x - game->renderer.viewportCenterX;
    game->ui.cursorScreenY = tile_center_y;
    for (int corner = 0; corner < 4; ++corner) {
        game->build.selectionTileX[corner] -= game->renderer.viewportCenterX;
        game->build.selectionTileY[corner] -= game->renderer.viewportCenterY;
    }
}
