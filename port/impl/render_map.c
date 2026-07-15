// port/impl/render_map.c - renderMap: the isometric world renderer.
// Draws terrain into the offscreen backbuffer (only when the viewport moved),
// blits it, then overlays dynamic tiles, trains, selections and the build menu.
#include "game_methods.h"
#include <stdlib.h>

// public void renderMap(Graphics graphics)
void gc_renderMap(GameCanvas *gc, MidpGraphics *g) {
    MidpGraphics *bb = gc->backbufferG;
    int n, n2, n3, n4, n5, n6, n7;
    int n9 = gc->viewportCenterX - 2 * gc->viewportCenterY + 16 + 2 * gc->mapHalfWidth;
    int n10 = gc->viewportCenterX + 2 * gc->viewportCenterY - 16 - 2 * gc->mapHalfWidth;
    n9 /= 32;
    n10 /= 32;
    if (gc->viewportCenterX != gc->lastViewportCenterX || gc->viewportCenterY != gc->viewportCenterYCached) {
        midp_set_clip(bb, 0, 15, gcs_screenWidth, gcs_eN);
        midp_set_color(bb, 0);
        midp_fill_rect(bb, 0, 15, gcs_screenWidth, gcs_eN);
        gc->visibleTrainCount = 0;
        gc->viewportX0 = 2 * gc->mapWidth;
        gc->viewportY0 = 2 * gc->mapHeight;
        gc->viewportX1 = -gc->mapWidth;
        gc->viewportY1 = -gc->mapWidth;
        for (int n8 = 0; n8 < gc->numActiveTileAnimations; ++n8) {
            n7 = n9 + gc->tileOffsetX[n8];
            int n11 = n10 + gc->tileOffsetY[n8];
            if (n7 >= 0 && n7 < gc->mapWidth && n11 >= 0 && n11 < gc->mapHeight) {
                n6 = gc_c(gc, gc->tileMap[n7][n11]);
                if (n6 >= 128) n6 -= 128;
                if (!((n6 != 0 && n6 <= 28) || (n6 >= 77 && n6 <= 86))) {
                    n5 = n11 * 16 + n7 * 16 + 16 - gc->viewportCenterX;
                    n4 = gc->mapHalfWidth - n7 * 8 + n11 * 8 + 16 - gc->viewportCenterY - 4 * gc->tileElevation[n7][n11] + 15;
                    if (gc->tileElevation[n7][n11] > -1) {
                        if ((n6 >= 42 && n6 <= 48) || (n6 >= 70 && n6 <= 76))
                            midp_draw_image(bb, gc->tileSprites[gc->tileSpriteIndex[n6]], n5, n4, 33);
                        else
                            midp_draw_image(bb, gc->tileSprites[gc->tileSpriteIndex[1]], n5, n4, 33);
                    } else {
                        midp_draw_image(bb, gc->tileSprites[gc->tileSpriteIndex[2]], n5, n4, 33);
                    }
                }
                if (n6 >= 3 && n6 <= 28) {
                    n5 = n11 * 16 + n7 * 16 + 16 - gc->viewportCenterX;
                    n4 = gc->mapHalfWidth - n7 * 8 + n11 * 8 + 16 - gc->viewportCenterY - 4 * gc->tileElevation[n7][n11] + 15;
                    midp_draw_image(bb, gc->tileSprites[gc->tileSpriteIndex[n6]], n5, n4 + gc->tileSpriteYOffsetByCode[n6 - 3], 33);
                }
                if (n6 >= 77 && n6 <= 80) {
                    n5 = n11 * 16 + n7 * 16 + 16 - gc->viewportCenterX;
                    n4 = gc->mapHalfWidth - n7 * 8 + n11 * 8 + 16 - gc->viewportCenterY - 4 * gc->tileElevation[n7][n11] + 15;
                    midp_draw_image(bb, gc->tileSprites[gc->tileSpriteIndex[n6]], n5, n4 + gc->tileSpriteYOffsetByCode[n6 - 56 - 3], 33);
                }
                if (n6 >= 80 && n6 <= 86) {
                    n5 = n11 * 16 + n7 * 16 + 16 - gc->viewportCenterX;
                    n4 = gc->mapHalfWidth - n7 * 8 + n11 * 8 + 16 - gc->viewportCenterY - 4 * gc->tileElevation[n7][n11] + 15;
                    n3 = 0;
                    switch (n6) { case 84: n3 = 4; break; case 86: n3 = 4; break; }
                    midp_draw_image(bb, gc->tileSprites[gc->tileSpriteIndex[n6]], n5, n4 + n3, 33);
                }
            }
            if (n7 > gc->viewportX1) gc->viewportX1 = n7;
            if (n11 > gc->viewportY1) gc->viewportY1 = n11;
            if (n7 < gc->viewportX0) gc->viewportX0 = n7;
            if (n11 >= gc->viewportY0) continue;
            gc->viewportY0 = n11;
        }
        if (gc->viewportX0 < 0) gc->viewportX0 = 0;
        if (gc->viewportY0 < 0) gc->viewportY0 = 0;
        if (gc->viewportX1 >= gc->mapWidth) gc->viewportX1 = gc->mapWidth - 1;
        if (gc->viewportY1 >= gc->mapHeight) gc->viewportY1 = gc->mapHeight - 1;
        gc->lastViewportCenterX = gc->viewportCenterX;
        gc->viewportCenterYCached = gc->viewportCenterY;
        midp_set_clip(bb, 0, 0, gcs_screenWidth, gcs_hs);
    }
    gc_paintMain(gc, g);
    midp_draw_image(g, gc->backbuffer, 0, 0, 20);
    midp_set_clip(g, 0, 15, gcs_screenWidth, gcs_eN);
    gc_drawBuildMenuCursor(gc, g);
    int n8w = gc->viewportX1 - gc->viewportX0 + 1;
    int n7h = gc->viewportY1 - gc->viewportY0 + 1;
    if (n8w < 1) n8w = 1;
    if (n7h < 1) n7h = 1;
    bool *blArray = (bool *)calloc((size_t)n8w * n7h, sizeof(bool));
#define BLA(a, b) blArray[(a) * n7h + (b)]
    gc->visibleTrainCount = 0;
    for (n6 = 0; n6 < 500; ++n6) {
        if ((gc->trainFlags[n6] & 0x80) != 128) continue;
        n5 = gc_c(gc, gc->trainX[n6]);
        n4 = gc_c(gc, gc->trainY[n6]);
        if (n5 < gc->viewportX0 || n4 < gc->viewportY0 || n5 > gc->viewportX1 || n4 > gc->viewportY1) continue;
        gc->visibleTrains[gc->visibleTrainCount++] = (int16_t)n6;
        BLA(n5 - gc->viewportX0, n4 - gc->viewportY0) = true;
    }
    for (n6 = 0; n6 < gc->numActiveTileAnimations; ++n6) {
        int n12;
        n5 = n9 + gc->tileOffsetX[n6];
        n4 = n10 + gc->tileOffsetY[n6];
        if (n5 < 0 || n5 >= gc->mapWidth || n4 < 0 || n4 >= gc->mapHeight) continue;
        n3 = gc_c(gc, gc->tileMap[n5][n4]);
        n2 = n3;
        if (n2 >= 128) n2 -= 128;
        if (n2 > 28 && n2 < 128 && (((n2 < 42 || n2 > 48) && n2 < 70) || n2 > 86)) {
            n = n4 * 16 + n5 * 16 + 16 - gc->viewportCenterX;
            n12 = gc->mapHalfWidth - n5 * 8 + n4 * 8 + 16 - gc->viewportCenterY - 4 * gc->tileElevation[n5][n4] + 15;
            if (!gcs_ch) {
                if (n2 == 54 || n2 == 55)
                    midp_draw_image(g, gc->tileSprites[gc->tileSpriteIndex[53]], n, n12, 33);
                if (gc->phaseTimer == 1 && n2 >= 65 && n2 <= 68) {
                    int8_t *row = gc->tileMap[n5];
                    row[n4] = (int8_t)(row[n4] + 1);
                    if (gc->tileMap[n5][n4] == 69) gc->tileMap[n5][n4] = 65;
                }
                if (n2 != 55) midp_draw_image(g, gc->tileSprites[gc->tileSpriteIndex[n2]], n, n12, 33);
                else midp_draw_image(g, gc->tileSprites[gc->tileSpriteIndex[243 + gc->timerC2]], n, n12, 33);
                if (n2 == 50) midp_draw_image(g, gc->tileSprites[gc->tileSpriteIndex[240 + gc->timerA2]], n + 12, n12 - 28, 33);
                if (n2 == 64) midp_draw_image(g, gc->tileSprites[gc->tileSpriteIndex[246 + gc->timerB2]], n + 3, n12 - 39, 33);
            } else {
                midp_draw_image(g, gc->tileSprites[gc->tileSpriteIndex[41]], n, n12, 33);
            }
        }
        if (BLA(n5 - gc->viewportX0, n4 - gc->viewportY0)) {
            for (n = 0; n < gc->visibleTrainCount; ++n) {
                n12 = gc->visibleTrains[n];
                if ((gc->trainFlags[n12] & 0x80) != 128 || n5 != gc_c(gc, gc->trainX[n12]) || n4 != gc_c(gc, gc->trainY[n12])) continue;
                gc_drawSpriteSingle(gc, g, n12);
            }
        }
        if (n3 < 128) continue;
        n = n4 * 16 + n5 * 16 + 16 - gc->viewportCenterX;
        n12 = gc->mapHalfWidth - n5 * 8 + n4 * 8 + 16 - gc->viewportCenterY - 4 * gc->tileElevation[n5][n4] + 15;
        int n14 = -1;
        for (int i = 0; i < 50; ++i) {
            if (gc_c(gc, gc->tileQueue[i][1]) != n5 || gc_c(gc, gc->tileQueue[i][2]) != n4) continue;
            n14 = gc->tileQueue[i][0];
        }
        if (n14 <= 0) continue;
        midp_draw_image(g, gc->tileSprites[gc_c(gc, gc->tileSpriteIndex[250 + n14 / 3])], n, n12 - 3, 33);
    }
    if (!gcs_selectionHidden) {
        for (n6 = 0; n6 < gc->numActiveTileAnimations; ++n6) {
            n5 = n9 + gc->tileOffsetX[n6];
            n4 = n10 + gc->tileOffsetY[n6];
            for (n3 = 0; n3 < gc->numSelectedTrains; ++n3) {
                if (gc->trainSelectionY[n3] != n5 || gc->trainSelectionX[n3] != n4) continue;
                n2 = n4 * 16 + n5 * 16 + 16 - gc->viewportCenterX;
                n = gc->mapHalfWidth - n5 * 8 + n4 * 8 + 16 - gc->viewportCenterY - 4 * gc->tileElevation[n5][n4] + 15;
                gc_drawRectFill(gc, g, n2, n - 16, (int8_t)n3);
            }
        }
    }
    gc_drawBuildMenuRow(gc, g);
    midp_set_clip(g, 0, 0, gcs_screenWidth, gcs_hs);
    gc_drawConfirmDialog(gc, g);
    free(blArray);
#undef BLA
}
