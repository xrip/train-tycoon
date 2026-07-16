// Isometric world drawing.
// Draws terrain into the offscreen backbuffer (only when the viewport moved),
// blits it, then overlays dynamic tiles, trains, selections and the build menu.
#include "game_methods.h"
#include <vector>

// public void renderMap(Graphics graphics)
void gc_renderMap(GameCanvas *gc, Surface *g) {
    Surface *bb = gc->renderer.backbuffer_surface;
    int n, n2, n3, n4, n5, n6, n7;
    int n9 = gc->viewportCenterX - 2 * gc->viewportCenterY + 16 + 2 * gc->world.mapHalfWidth;
    int n10 = gc->viewportCenterX + 2 * gc->viewportCenterY - 16 - 2 * gc->world.mapHalfWidth;
    n9 /= 32;
    n10 /= 32;
    if (gc->viewportCenterX != gc->renderer.lastViewportCenterX || gc->viewportCenterY != gc->renderer.viewportCenterYCached) {
        midp_set_clip(bb, 0, 15, gcs_screenWidth, gcs_gameplayViewportHeight);
        midp_set_color(bb, 0);
        midp_fill_rect(bb, 0, 15, gcs_screenWidth, gcs_gameplayViewportHeight);
        gc->renderer.visibleTrainCount = 0;
        gc->renderer.viewportX0 = 2 * gc->world.mapWidth;
        gc->renderer.viewportY0 = 2 * gc->world.mapHeight;
        gc->renderer.viewportX1 = -gc->world.mapWidth;
        gc->renderer.viewportY1 = -gc->world.mapWidth;
        for (int n8 = 0; n8 < gc->assets.numActiveTileAnimations; ++n8) {
            n7 = n9 + gc->assets.tileOffsetX[n8];
            int n11 = n10 + gc->assets.tileOffsetY[n8];
            if (n7 >= 0 && n7 < gc->world.mapWidth && n11 >= 0 && n11 < gc->world.mapHeight) {
                n6 = gc_byteToUnsignedInt(gc, gc->world.tileMap[n7][n11]);
                if (n6 >= 128) n6 -= 128;
                if (!((n6 != 0 && n6 <= 28) || (n6 >= 77 && n6 <= 86))) {
                    n5 = n11 * 16 + n7 * 16 + 16 - gc->viewportCenterX;
                    n4 = gc->world.mapHalfWidth - n7 * 8 + n11 * 8 + 16 - gc->viewportCenterY - 4 * gc->world.tileElevation[n7][n11] + 15;
                    if (gc->world.tileElevation[n7][n11] > -1) {
                        if ((n6 >= 42 && n6 <= 48) || (n6 >= 70 && n6 <= 76))
                            midp_draw_image(bb, gc->assets.tile_sprites[gc->assets.tileSpriteIndex[n6]].get(), n5, n4, 33);
                        else
                            midp_draw_image(bb, gc->assets.tile_sprites[gc->assets.tileSpriteIndex[1]].get(), n5, n4, 33);
                    } else {
                        midp_draw_image(bb, gc->assets.tile_sprites[gc->assets.tileSpriteIndex[2]].get(), n5, n4, 33);
                    }
                }
                if (n6 >= 3 && n6 <= 28) {
                    n5 = n11 * 16 + n7 * 16 + 16 - gc->viewportCenterX;
                    n4 = gc->world.mapHalfWidth - n7 * 8 + n11 * 8 + 16 - gc->viewportCenterY - 4 * gc->world.tileElevation[n7][n11] + 15;
                    midp_draw_image(bb, gc->assets.tile_sprites[gc->assets.tileSpriteIndex[n6]].get(), n5, n4 + gc->assets.tileSpriteYOffsetByCode[n6 - 3], 33);
                }
                if (n6 >= 77 && n6 <= 80) {
                    n5 = n11 * 16 + n7 * 16 + 16 - gc->viewportCenterX;
                    n4 = gc->world.mapHalfWidth - n7 * 8 + n11 * 8 + 16 - gc->viewportCenterY - 4 * gc->world.tileElevation[n7][n11] + 15;
                    midp_draw_image(bb, gc->assets.tile_sprites[gc->assets.tileSpriteIndex[n6]].get(), n5, n4 + gc->assets.tileSpriteYOffsetByCode[n6 - 56 - 3], 33);
                }
                if (n6 >= 80 && n6 <= 86) {
                    n5 = n11 * 16 + n7 * 16 + 16 - gc->viewportCenterX;
                    n4 = gc->world.mapHalfWidth - n7 * 8 + n11 * 8 + 16 - gc->viewportCenterY - 4 * gc->world.tileElevation[n7][n11] + 15;
                    n3 = 0;
                    switch (n6) { case 84: n3 = 4; break; case 86: n3 = 4; break; }
                    midp_draw_image(bb, gc->assets.tile_sprites[gc->assets.tileSpriteIndex[n6]].get(), n5, n4 + n3, 33);
                }
            }
            if (n7 > gc->renderer.viewportX1) gc->renderer.viewportX1 = n7;
            if (n11 > gc->renderer.viewportY1) gc->renderer.viewportY1 = n11;
            if (n7 < gc->renderer.viewportX0) gc->renderer.viewportX0 = n7;
            if (n11 >= gc->renderer.viewportY0) continue;
            gc->renderer.viewportY0 = n11;
        }
        if (gc->renderer.viewportX0 < 0) gc->renderer.viewportX0 = 0;
        if (gc->renderer.viewportY0 < 0) gc->renderer.viewportY0 = 0;
        if (gc->renderer.viewportX1 >= gc->world.mapWidth) gc->renderer.viewportX1 = gc->world.mapWidth - 1;
        if (gc->renderer.viewportY1 >= gc->world.mapHeight) gc->renderer.viewportY1 = gc->world.mapHeight - 1;
        gc->renderer.lastViewportCenterX = gc->viewportCenterX;
        gc->renderer.viewportCenterYCached = gc->viewportCenterY;
        midp_set_clip(bb, 0, 0, gcs_screenWidth, gcs_hs);
    }
    gc_paintMain(gc, g);
    midp_draw_image(g, gc->renderer.backbuffer.get(), 0, 0, 20);
    midp_set_clip(g, 0, 15, gcs_screenWidth, gcs_gameplayViewportHeight);
    gc_drawBuildMenuCursor(gc, g);
    int visible_width = gc->renderer.viewportX1 - gc->renderer.viewportX0 + 1;
    int visible_height = gc->renderer.viewportY1 - gc->renderer.viewportY0 + 1;
    if (visible_width < 1) visible_width = 1;
    if (visible_height < 1) visible_height = 1;
    std::vector<uint8_t> visible_train_tiles(
        static_cast<std::size_t>(visible_width * visible_height));
    gc->renderer.visibleTrainCount = 0;
    for (n6 = 0; n6 < 500; ++n6) {
        if ((gc->fleet.trainFlags[n6] & 0x80) != 128) continue;
        n5 = gc_byteToUnsignedInt(gc, gc->fleet.trainX[n6]);
        n4 = gc_byteToUnsignedInt(gc, gc->fleet.trainY[n6]);
        if (n5 < gc->renderer.viewportX0 || n4 < gc->renderer.viewportY0 || n5 > gc->renderer.viewportX1 || n4 > gc->renderer.viewportY1) continue;
        gc->renderer.visibleTrains[gc->renderer.visibleTrainCount++] = (int16_t)n6;
        visible_train_tiles[(n5 - gc->renderer.viewportX0) * visible_height +
                            n4 - gc->renderer.viewportY0] = 1;
    }
    for (n6 = 0; n6 < gc->assets.numActiveTileAnimations; ++n6) {
        int n12;
        n5 = n9 + gc->assets.tileOffsetX[n6];
        n4 = n10 + gc->assets.tileOffsetY[n6];
        if (n5 < 0 || n5 >= gc->world.mapWidth || n4 < 0 || n4 >= gc->world.mapHeight) continue;
        n3 = gc_byteToUnsignedInt(gc, gc->world.tileMap[n5][n4]);
        n2 = n3;
        if (n2 >= 128) n2 -= 128;
        if (n2 > 28 && n2 < 128 && (((n2 < 42 || n2 > 48) && n2 < 70) || n2 > 86)) {
            n = n4 * 16 + n5 * 16 + 16 - gc->viewportCenterX;
            n12 = gc->world.mapHalfWidth - n5 * 8 + n4 * 8 + 16 - gc->viewportCenterY - 4 * gc->world.tileElevation[n5][n4] + 15;
            if (!gcs_animationsDisabled) {
                if (n2 == 54 || n2 == 55)
                    midp_draw_image(g, gc->assets.tile_sprites[gc->assets.tileSpriteIndex[53]].get(), n, n12, 33);
                if (gc->world.phaseTimer == 1 && n2 >= 65 && n2 <= 68) {
                    int8_t *row = gc->world.tileMap[n5].data();
                    row[n4] = (int8_t)(row[n4] + 1);
                    if (gc->world.tileMap[n5][n4] == 69) gc->world.tileMap[n5][n4] = 65;
                }
                if (n2 != 55) midp_draw_image(g, gc->assets.tile_sprites[gc->assets.tileSpriteIndex[n2]].get(), n, n12, 33);
                else midp_draw_image(g, gc->assets.tile_sprites[gc->assets.tileSpriteIndex[243 + gc->timerC2]].get(), n, n12, 33);
                if (n2 == 50) midp_draw_image(g, gc->assets.tile_sprites[gc->assets.tileSpriteIndex[240 + gc->timerA2]].get(), n + 12, n12 - 28, 33);
                if (n2 == 64) midp_draw_image(g, gc->assets.tile_sprites[gc->assets.tileSpriteIndex[246 + gc->timerB2]].get(), n + 3, n12 - 39, 33);
            } else {
                midp_draw_image(g, gc->assets.tile_sprites[gc->assets.tileSpriteIndex[41]].get(), n, n12, 33);
            }
        }
        if (visible_train_tiles[(n5 - gc->renderer.viewportX0) * visible_height +
                                n4 - gc->renderer.viewportY0]) {
            for (n = 0; n < gc->renderer.visibleTrainCount; ++n) {
                n12 = gc->renderer.visibleTrains[n];
                if ((gc->fleet.trainFlags[n12] & 0x80) != 128 || n5 != gc_byteToUnsignedInt(gc, gc->fleet.trainX[n12]) || n4 != gc_byteToUnsignedInt(gc, gc->fleet.trainY[n12])) continue;
                gc_drawSpriteSingle(gc, g, n12);
            }
        }
        if (n3 < 128) continue;
        n = n4 * 16 + n5 * 16 + 16 - gc->viewportCenterX;
        n12 = gc->world.mapHalfWidth - n5 * 8 + n4 * 8 + 16 - gc->viewportCenterY - 4 * gc->world.tileElevation[n5][n4] + 15;
        int n14 = -1;
        for (int i = 0; i < 50; ++i) {
            if (gc_byteToUnsignedInt(gc, gc->world.tileQueue[i][1]) != n5 || gc_byteToUnsignedInt(gc, gc->world.tileQueue[i][2]) != n4) continue;
            n14 = gc->world.tileQueue[i][0];
        }
        if (n14 <= 0) continue;
        midp_draw_image(g, gc->assets.tile_sprites[gc_byteToUnsignedInt(gc, gc->assets.tileSpriteIndex[250 + n14 / 3])].get(), n, n12 - 3, 33);
    }
    if (!gcs_selectionHidden) {
        for (n6 = 0; n6 < gc->assets.numActiveTileAnimations; ++n6) {
            n5 = n9 + gc->assets.tileOffsetX[n6];
            n4 = n10 + gc->assets.tileOffsetY[n6];
            for (n3 = 0; n3 < gc->numSelectedTrains; ++n3) {
                if (gc->trainSelectionY[n3] != n5 || gc->trainSelectionX[n3] != n4) continue;
                n2 = n4 * 16 + n5 * 16 + 16 - gc->viewportCenterX;
                n = gc->world.mapHalfWidth - n5 * 8 + n4 * 8 + 16 - gc->viewportCenterY - 4 * gc->world.tileElevation[n5][n4] + 15;
                gc_drawRectFill(gc, g, n2, n - 16, (int8_t)n3);
            }
        }
    }
    gc_drawBuildMenuRow(gc, g);
    midp_set_clip(g, 0, 0, gcs_screenWidth, gcs_hs);
    gc_drawConfirmDialog(gc, g);
}
