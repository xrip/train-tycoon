// Game input state and actions.
#include "game_methods.h"

#include <stdlib.h>

void gc_keyReleased(GameCanvas *gc, int32_t n) {
    (void)n;
    gc->currentKey = -999;
}

void gc_pointerPressed(GameCanvas *gc, int32_t n, int32_t n2) {
    (void)n; (void)n2;
    if (gc->inputPending) return;
}

void gc_pointerReleased(GameCanvas *gc, int32_t n, int32_t n2) {
    (void)n; (void)n2;
    if (gc->inputPending) {
        gc->inputPending = false;
        return;
    }
}

void gc_pointerDragged(GameCanvas *gc, int32_t n, int32_t n2) {
    (void)n; (void)n2;
    if (gc->inputPending) return;
}

void gc_resetKeyState(GameCanvas *gc, int32_t n) {
    (void)n;
    gc->currentKey = 0;
    gc->lastKeyCode = 0;
    gc->keySubState = 0;
    gc->keyRepeatDelay = 0;
    gc->heldKey = 0;
    gc->inputSubState = 0;
}

// void cycleBuildMenuIndex(boolean forward)
void gc_cycleBuildMenuIndex(GameCanvas *gc, bool forward) {
    gc->ui.buildMenuItemIndex += forward ? 1 : -1;
    int item_count = static_cast<int>(gc->ui.buildMenuItems[gc->ui.buildMenuMode].size());
    if (gc->ui.buildMenuItemIndex < 0) gc->ui.buildMenuItemIndex = item_count - 1;
    if (gc->ui.buildMenuItemIndex == item_count) gc->ui.buildMenuItemIndex = 0;
    gc->ui.confirmTargetState =
        gc->ui.buildMenuResultCodes[gc->ui.buildMenuMode][gc->ui.buildMenuItemIndex];
    gc->ui.menuDirty = true;
    gc->ui.confirmCounter = 20;
}

// void viewportPanKey()
void gc_viewportPanKey(GameCanvas *gc) {
    int key = gc->lastKeyCode;
    if (key == 56 || key == 6) gc->viewportCenterY += 30;
    if (key == 50 || key == 1) gc->viewportCenterY -= 30;
    if (key == 54 || key == 5) gc->viewportCenterX += 60;
    if (key == 52 || key == 2) gc->viewportCenterX -= 60;
    if (key == 49) {
        gc->viewportCenterY -= 30;
        gc->viewportCenterX -= 60;
    }
    if (key == 51) {
        gc->viewportCenterY -= 30;
        gc->viewportCenterX += 60;
    }
    if (key == 55) {
        gc->viewportCenterY += 30;
        gc->viewportCenterX -= 60;
    }
    if (key == 57) {
        gc->viewportCenterY += 30;
        gc->viewportCenterX += 60;
    }
    if (gc->viewportCenterX < 0) gc->viewportCenterX = 0;
    if (gc->viewportCenterY < 0) gc->viewportCenterY = 0;
    if (gc->viewportCenterX > gc->world.mapScreenWidth - gcs_screenWidth)
        gc->viewportCenterX = gc->world.mapScreenWidth - gcs_screenWidth;
    if (gc->viewportCenterY > gc->world.mapScreenHeight - gcs_gameplayViewportHeight)
        gc->viewportCenterY = gc->world.mapScreenHeight - gcs_gameplayViewportHeight;
    gc->viewportBaseX = gc->viewportCenterX + gcs_screenWidth / 2;
    gc->viewportBaseY =
        gc->viewportCenterY + gcs_gameplayViewportHeight / 2;
    gc->mapScaleX = gc->viewportBaseX * 2;
    gc->mapScaleY = gc->viewportBaseY * 2;
}

// void viewportZoomKey()
void gc_viewportZoomKey(GameCanvas *gc) {
    int step = gc_computeKeyRepeatDelay(gc);
    int key = gc->lastKeyCode;
    if (key == 56 || key == 6) gc->mapScaleY += step * 2;
    if (key == 50 || key == 1) gc->mapScaleY -= step * 2;
    if (key == 54 || key == 5) gc->mapScaleX += step * 2;
    if (key == 52 || key == 2) gc->mapScaleX -= step * 2;
    if (key == 49) {
        gc->mapScaleY -= step;
        gc->mapScaleX -= step * 2;
    }
    if (key == 51) {
        gc->mapScaleY -= step;
        gc->mapScaleX += step * 2;
    }
    if (key == 55) {
        gc->mapScaleY += step;
        gc->mapScaleX -= step * 2;
    }
    if (key == 57) {
        gc->mapScaleY += step;
        gc->mapScaleX += step * 2;
    }

    gc->viewportBaseX = gc->mapScaleX / 2;
    gc->viewportBaseY = gc->mapScaleY / 2;
    if (gc->viewportBaseX > gc->world.mapScreenWidth - 8)
        gc->viewportBaseX = gc->world.mapScreenWidth - 8;
    if (gc->viewportBaseY > gc->world.mapScreenHeight - 4)
        gc->viewportBaseY = gc->world.mapScreenHeight - 4;

    bool moved_view = false;
    if (gc->viewportCenterX > 0
        && gc->viewportBaseX - gc->viewportCenterX <= 10) {
        moved_view = true;
        gc->viewportCenterX = gc->viewportBaseX - 11;
    }
    if (gc->viewportCenterY > 0
        && gc->viewportBaseY - gc->viewportCenterY <= 10) {
        moved_view = true;
        gc->viewportCenterY = gc->viewportBaseY - 11;
    }
    if (gc->viewportBaseX - gc->viewportCenterX >= gcs_screenWidth - 20) {
        moved_view = true;
        gc->viewportCenterX = gc->viewportBaseX - gcs_screenWidth + 21;
    }
    if (gc->viewportBaseY - gc->viewportCenterY
        >= gcs_gameplayViewportHeight - 20) {
        moved_view = true;
        gc->viewportCenterY =
            gc->viewportBaseY - gcs_gameplayViewportHeight + 21;
    }
    if (gc->viewportBaseX < 0) gc->viewportBaseX = 0;
    if (gc->viewportBaseY < 0) gc->viewportBaseY = 0;
    if (gc->viewportCenterX < 0) {
        gc->viewportCenterX = 0;
        moved_view = false;
    }
    if (gc->viewportCenterY < 0) {
        gc->viewportCenterY = 0;
        moved_view = false;
    }
    if (gc->viewportCenterX > gc->world.mapScreenWidth - gcs_screenWidth) {
        gc->viewportCenterX = gc->world.mapScreenWidth - gcs_screenWidth;
        moved_view = false;
    }
    if (gc->viewportCenterY
        > gc->world.mapScreenHeight - gcs_gameplayViewportHeight) {
        gc->viewportCenterY =
            gc->world.mapScreenHeight - gcs_gameplayViewportHeight;
        moved_view = false;
    }
    if (moved_view) gc->keySubState = 80;
    gc_computeViewportBounds(gc);
    gc->mapScaleX = gc->viewportBaseX * 2;
    gc->mapScaleY = gc->viewportBaseY * 2;
}

// void computeViewportBounds()
void gc_computeViewportBounds(GameCanvas *gc) {
    int tile_center_x = 0;
    int tile_center_y = 0;
    int tile_x = gc->viewportBaseX - 2 * gc->viewportBaseY
                 + 16 + 2 * gc->world.mapHalfWidth;
    int tile_y = gc->viewportBaseX + 2 * gc->viewportBaseY
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
                gc_crossProduct2D(gc, gc->viewportBaseX, gc->viewportBaseY,
                                  gc->build.selectionTileX[0], gc->build.selectionTileY[0],
                                  gc->build.selectionTileX[1], gc->build.selectionTileY[1])
                * gc_crossProduct2D(gc, gc->viewportBaseX, gc->viewportBaseY,
                                    gc->build.selectionTileX[2], gc->build.selectionTileY[2],
                                    gc->build.selectionTileX[3], gc->build.selectionTileY[3]);
            int other_edges =
                gc_crossProduct2D(gc, gc->viewportBaseX, gc->viewportBaseY,
                                  gc->build.selectionTileX[0], gc->build.selectionTileY[0],
                                  gc->build.selectionTileX[2], gc->build.selectionTileY[2])
                * gc_crossProduct2D(gc, gc->viewportBaseX, gc->viewportBaseY,
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
    gc->tileCursorX = tile_x;
    gc->tileCursorY = tile_y;
    int corner_distance[4];
    for (int corner = 0; corner < 4; ++corner) {
        int dx = gc->viewportBaseX - gc->build.selectionTileX[corner];
        int dy = gc->viewportBaseY - gc->build.selectionTileY[corner];
        corner_distance[corner] = abs(dx * dx + dy * dy);
    }
    int nearest = corner_distance[0];
    for (int corner = 1; corner < 4; ++corner)
        if (corner_distance[corner] < nearest) nearest = corner_distance[corner];
    gc->vehicleMenuSelectIndex = corner_distance[0] < corner_distance[3];
    gc->trainMenuSelectIndex = corner_distance[1] < corner_distance[2];
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
    tile_center_y -= gc->viewportCenterY
                     + 4 * gc->world.heightMap[gc->build.swapMapX][gc->build.swapMapY];
    gc->cursorScreenX = tile_center_x - gc->viewportCenterX;
    gc->cursorScreenY = tile_center_y;
    for (int corner = 0; corner < 4; ++corner) {
        gc->build.selectionTileX[corner] -= gc->viewportCenterX;
        gc->build.selectionTileY[corner] -= gc->viewportCenterY;
    }
}
