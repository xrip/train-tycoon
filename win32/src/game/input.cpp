// Game input state and actions.
#include "game_methods.h"

#include <stdlib.h>

void gc_keyReleased(Game *gc, int32_t n) {
    (void)n;
    gc->ui.currentKey = -999;
}

void gc_pointerPressed(Game *gc, int32_t n, int32_t n2) {
    (void)n; (void)n2;
    if (gc->ui.inputPending) return;
}

void gc_pointerReleased(Game *gc, int32_t n, int32_t n2) {
    (void)n; (void)n2;
    if (gc->ui.inputPending) {
        gc->ui.inputPending = false;
        return;
    }
}

void gc_pointerDragged(Game *gc, int32_t n, int32_t n2) {
    (void)n; (void)n2;
    if (gc->ui.inputPending) return;
}

void gc_resetKeyState(Game *gc, int32_t n) {
    (void)n;
    gc->ui.currentKey = 0;
    gc->ui.lastKeyCode = 0;
    gc->ui.keySubState = 0;
    gc->ui.keyRepeatDelay = 0;
    gc->ui.heldKey = 0;
    gc->ui.inputSubState = 0;
}

void gc_cycleBuildMenuIndex(Game *gc, bool forward) {
    gc->ui.buildMenuItemIndex += forward ? 1 : -1;
    int item_count = static_cast<int>(gc->ui.buildMenuItems[gc->ui.buildMenuMode].size());
    if (gc->ui.buildMenuItemIndex < 0) gc->ui.buildMenuItemIndex = item_count - 1;
    if (gc->ui.buildMenuItemIndex == item_count) gc->ui.buildMenuItemIndex = 0;
    gc->ui.confirmTargetState =
        gc->ui.buildMenuResultCodes[gc->ui.buildMenuMode][gc->ui.buildMenuItemIndex];
    gc->ui.menuDirty = true;
    gc->ui.confirmCounter = 20;
}

void gc_viewportPanKey(Game *gc) {
    int key = gc->ui.lastKeyCode;
    if (key == 56 || key == 6) gc->renderer.viewportCenterY += 30;
    if (key == 50 || key == 1) gc->renderer.viewportCenterY -= 30;
    if (key == 54 || key == 5) gc->renderer.viewportCenterX += 60;
    if (key == 52 || key == 2) gc->renderer.viewportCenterX -= 60;
    if (key == 49) {
        gc->renderer.viewportCenterY -= 30;
        gc->renderer.viewportCenterX -= 60;
    }
    if (key == 51) {
        gc->renderer.viewportCenterY -= 30;
        gc->renderer.viewportCenterX += 60;
    }
    if (key == 55) {
        gc->renderer.viewportCenterY += 30;
        gc->renderer.viewportCenterX -= 60;
    }
    if (key == 57) {
        gc->renderer.viewportCenterY += 30;
        gc->renderer.viewportCenterX += 60;
    }
    if (gc->renderer.viewportCenterX < 0) gc->renderer.viewportCenterX = 0;
    if (gc->renderer.viewportCenterY < 0) gc->renderer.viewportCenterY = 0;
    if (gc->renderer.viewportCenterX > gc->world.mapScreenWidth - gcs_screenWidth)
        gc->renderer.viewportCenterX = gc->world.mapScreenWidth - gcs_screenWidth;
    if (gc->renderer.viewportCenterY > gc->world.mapScreenHeight - gcs_gameplayViewportHeight)
        gc->renderer.viewportCenterY = gc->world.mapScreenHeight - gcs_gameplayViewportHeight;
    gc->renderer.viewportBaseX = gc->renderer.viewportCenterX + gcs_screenWidth / 2;
    gc->renderer.viewportBaseY =
        gc->renderer.viewportCenterY + gcs_gameplayViewportHeight / 2;
    gc->renderer.mapScaleX = gc->renderer.viewportBaseX * 2;
    gc->renderer.mapScaleY = gc->renderer.viewportBaseY * 2;
}

void gc_viewportZoomKey(Game *gc) {
    int step = gc_computeKeyRepeatDelay(gc);
    int key = gc->ui.lastKeyCode;
    if (key == 56 || key == 6) gc->renderer.mapScaleY += step * 2;
    if (key == 50 || key == 1) gc->renderer.mapScaleY -= step * 2;
    if (key == 54 || key == 5) gc->renderer.mapScaleX += step * 2;
    if (key == 52 || key == 2) gc->renderer.mapScaleX -= step * 2;
    if (key == 49) {
        gc->renderer.mapScaleY -= step;
        gc->renderer.mapScaleX -= step * 2;
    }
    if (key == 51) {
        gc->renderer.mapScaleY -= step;
        gc->renderer.mapScaleX += step * 2;
    }
    if (key == 55) {
        gc->renderer.mapScaleY += step;
        gc->renderer.mapScaleX -= step * 2;
    }
    if (key == 57) {
        gc->renderer.mapScaleY += step;
        gc->renderer.mapScaleX += step * 2;
    }

    gc->renderer.viewportBaseX = gc->renderer.mapScaleX / 2;
    gc->renderer.viewportBaseY = gc->renderer.mapScaleY / 2;
    if (gc->renderer.viewportBaseX > gc->world.mapScreenWidth - 8)
        gc->renderer.viewportBaseX = gc->world.mapScreenWidth - 8;
    if (gc->renderer.viewportBaseY > gc->world.mapScreenHeight - 4)
        gc->renderer.viewportBaseY = gc->world.mapScreenHeight - 4;

    bool moved_view = false;
    if (gc->renderer.viewportCenterX > 0
        && gc->renderer.viewportBaseX - gc->renderer.viewportCenterX <= 10) {
        moved_view = true;
        gc->renderer.viewportCenterX = gc->renderer.viewportBaseX - 11;
    }
    if (gc->renderer.viewportCenterY > 0
        && gc->renderer.viewportBaseY - gc->renderer.viewportCenterY <= 10) {
        moved_view = true;
        gc->renderer.viewportCenterY = gc->renderer.viewportBaseY - 11;
    }
    if (gc->renderer.viewportBaseX - gc->renderer.viewportCenterX >= gcs_screenWidth - 20) {
        moved_view = true;
        gc->renderer.viewportCenterX = gc->renderer.viewportBaseX - gcs_screenWidth + 21;
    }
    if (gc->renderer.viewportBaseY - gc->renderer.viewportCenterY
        >= gcs_gameplayViewportHeight - 20) {
        moved_view = true;
        gc->renderer.viewportCenterY =
            gc->renderer.viewportBaseY - gcs_gameplayViewportHeight + 21;
    }
    if (gc->renderer.viewportBaseX < 0) gc->renderer.viewportBaseX = 0;
    if (gc->renderer.viewportBaseY < 0) gc->renderer.viewportBaseY = 0;
    if (gc->renderer.viewportCenterX < 0) {
        gc->renderer.viewportCenterX = 0;
        moved_view = false;
    }
    if (gc->renderer.viewportCenterY < 0) {
        gc->renderer.viewportCenterY = 0;
        moved_view = false;
    }
    if (gc->renderer.viewportCenterX > gc->world.mapScreenWidth - gcs_screenWidth) {
        gc->renderer.viewportCenterX = gc->world.mapScreenWidth - gcs_screenWidth;
        moved_view = false;
    }
    if (gc->renderer.viewportCenterY
        > gc->world.mapScreenHeight - gcs_gameplayViewportHeight) {
        gc->renderer.viewportCenterY =
            gc->world.mapScreenHeight - gcs_gameplayViewportHeight;
        moved_view = false;
    }
    if (moved_view) gc->ui.keySubState = 80;
    gc_computeViewportBounds(gc);
    gc->renderer.mapScaleX = gc->renderer.viewportBaseX * 2;
    gc->renderer.mapScaleY = gc->renderer.viewportBaseY * 2;
}

void gc_computeViewportBounds(Game *gc) {
    int tile_center_x = 0;
    int tile_center_y = 0;
    int tile_x = gc->renderer.viewportBaseX - 2 * gc->renderer.viewportBaseY
                 + 16 + 2 * gc->world.mapHalfWidth;
    int tile_y = gc->renderer.viewportBaseX + 2 * gc->renderer.viewportBaseY
                 - 16 - 2 * gc->world.mapHalfWidth;
    tile_x /= 32;
    tile_y /= 32;
    gc->ui.blockBuildMenu = true;

    for (int x = tile_x - 1; x <= tile_x + 1 && gc->ui.blockBuildMenu; ++x) {
        for (int y = tile_y - 1; y <= tile_y + 1; ++y) {
            if (!gc->world.in_bounds(x, y))
                continue;
            tile_center_x = y * 16 + x * 16 + 16;
            tile_center_y = gc->world.mapHalfWidth - x * 8 + y * 8 + 8;
            gc_computeIsoCornersInlined(gc, x, y, gc->build.selectionTileX.data(),
                                        gc->build.selectionTileY.data(), false);
            int opposite_edges =
                gc_crossProduct2D(gc, gc->renderer.viewportBaseX, gc->renderer.viewportBaseY,
                                  gc->build.selectionTileX[0], gc->build.selectionTileY[0],
                                  gc->build.selectionTileX[1], gc->build.selectionTileY[1])
                * gc_crossProduct2D(gc, gc->renderer.viewportBaseX, gc->renderer.viewportBaseY,
                                    gc->build.selectionTileX[2], gc->build.selectionTileY[2],
                                    gc->build.selectionTileX[3], gc->build.selectionTileY[3]);
            int other_edges =
                gc_crossProduct2D(gc, gc->renderer.viewportBaseX, gc->renderer.viewportBaseY,
                                  gc->build.selectionTileX[0], gc->build.selectionTileY[0],
                                  gc->build.selectionTileX[2], gc->build.selectionTileY[2])
                * gc_crossProduct2D(gc, gc->renderer.viewportBaseX, gc->renderer.viewportBaseY,
                                    gc->build.selectionTileX[1], gc->build.selectionTileY[1],
                                    gc->build.selectionTileX[3], gc->build.selectionTileY[3]);
            if (opposite_edges > 0 || other_edges > 0) continue;
            tile_x = x;
            tile_y = y;
            gc->ui.blockBuildMenu = false;
            break;
        }
    }
    if (gc->ui.blockBuildMenu) return;

    gc->build.swapMapX = tile_x;
    gc->build.swapMapY = tile_y;
    gc->ui.tileCursorX = tile_x;
    gc->ui.tileCursorY = tile_y;
    int corner_distance[4];
    for (int corner = 0; corner < 4; ++corner) {
        int dx = gc->renderer.viewportBaseX - gc->build.selectionTileX[corner];
        int dy = gc->renderer.viewportBaseY - gc->build.selectionTileY[corner];
        corner_distance[corner] = abs(dx * dx + dy * dy);
    }
    int nearest = corner_distance[0];
    for (int corner = 1; corner < 4; ++corner)
        if (corner_distance[corner] < nearest) nearest = corner_distance[corner];
    gc->ui.vehicleMenuSelectIndex = corner_distance[0] < corner_distance[3];
    gc->ui.trainMenuSelectIndex = corner_distance[1] < corner_distance[2];
    if (nearest == corner_distance[0]) {
        tile_center_x -= 16;
    } else if (nearest == corner_distance[1]) {
        tile_center_y -= 8;
        ++gc->build.swapMapX;
    } else if (nearest == corner_distance[2]) {
        tile_center_y += 8;
        ++gc->build.swapMapY;
    } else {
        tile_center_x += 16;
        ++gc->build.swapMapX;
        ++gc->build.swapMapY;
    }
    tile_center_y -= gc->renderer.viewportCenterY
                     + 4 * gc->world.heightMap[gc->build.swapMapX][gc->build.swapMapY];
    gc->ui.cursorScreenX = tile_center_x - gc->renderer.viewportCenterX;
    gc->ui.cursorScreenY = tile_center_y;
    for (int corner = 0; corner < 4; ++corner) {
        gc->build.selectionTileX[corner] -= gc->renderer.viewportCenterX;
        gc->build.selectionTileY[corner] -= gc->renderer.viewportCenterY;
    }
}
