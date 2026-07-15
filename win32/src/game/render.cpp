// Packed-sprite UI and text drawing.
// renderPackedSprite draws the menu/dialog layouts (state 20). renderFontString
// draws text from the `ca[]` font atlas. u() resolves per-slot sub-sprites.
#include "game_methods.h"

#define RGB3(r, g, b) (((r) << 16) | ((g) << 8) | (b))
static int imin(int a, int b) { return a < b ? a : b; }
static int imax(int a, int b) { return a > b ? a : b; }

// public void renderFontString(Graphics graphics, int n, int n2, byte[] byArray, boolean bl)
void gc_renderFontString(GameCanvas *gc, Surface *g, int32_t n, int32_t n2,
                        int8_t *byArray, bool bl) {
    int n3 = n;
    midp_set_color(g, 0);
    int len = midp_arr_len(byArray);
    for (int i = 0; i < len; ++i) {
        int8_t by = byArray[i];
        if (bl) {
            midp_set_clip(g, gc->clipX, gc->clipY, gc->clipWidth, gc->clipHeight);
            midp_clip_rect(g, n3, n2 - 4, 5, 12);
        } else {
            midp_set_clip(g, n3, n2 - 4, 5, 12);
        }
        if (by == 0) { n3 += 6; continue; }
        if (by < 32) {
            midp_draw_image(g, gc->font_images[(by - 1) / 16].get(), n3 - 5 * ((by - 1) % 16), n2, 20);
            n3 += 6;
            continue;
        }
        if (by < 40) {
            int8_t by2 = (int8_t)(byArray[i + 1] >= 40 && byArray[i + 1] <= 65 ? 1 : 0);
            switch (by) {
                case 32:
                    midp_draw_line(g, n3 + 2, n2 - 2 + by2, n3 + 2, n2 - 2 + by2);
                    break;
                case 35:
                    midp_draw_line(g, n3 + 2, n2 - 4 + by2, n3 + 2, n2 - 4 + by2);
                    /* fall through */
                case 34:
                    midp_draw_line(g, n3 + 2, n2 - 2 + by2, n3 + 1, n2 - 3 + by2);
                    /* fall through */
                case 33:
                    midp_draw_line(g, n3 + 2, n2 - 2 + by2, n3 + 3, n2 - 3 + by2);
                    break;
                case 37:
                    midp_draw_line(g, n3 + 1, n2 - 2 + by2, n3 + 1, n2 - 2 + by2);
                    midp_draw_line(g, n3 + 3, n2 - 2 + by2, n3 + 3, n2 - 2 + by2);
                    break;
            }
            continue;
        }
        if (by < 70) {
            midp_draw_image(g, gc->font_images[2 + (by - 40) / 16].get(), n3 - 5 * ((by - 40) % 16), n2, 20);
            n3 += 6;
            continue;
        }
        if (by >= 100) continue;
        midp_draw_image(g, gc->font_images[4 + (by - 70) / 16].get(), n3 - 5 * ((by - 70) % 16), n2, 20);
        n3 += 6;
    }
    if (bl) {
        midp_set_clip(g, gc->clipX, gc->clipY, gc->clipWidth, gc->clipHeight);
    } else {
        midp_set_clip(g, 0, 0, gcs_screenWidth, gcs_hs);
    }
}

// Image u(int n, int n2)
Image *gc_u(GameCanvas *gc, int32_t n, int32_t n2) {
    if (n == 23) {
        if (n2 == 0 && gc->wagonType >= 0 && gc->wagonType < 3)
            return gc->tile_sprites[gc_byteToUnsignedInt(gc, gc->tileSpriteIndex[136])].get();
        if (n2 == 0 && gc->wagonType >= 3 && gc->wagonType < 6)
            return gc->tile_sprites[gc_byteToUnsignedInt(gc, gc->tileSpriteIndex[146])].get();
        if (n2 > 0) {
            switch (gc->wagonOptionSlots[n2 - 1]) {
                case -1: return NULL;
                case 0: return gc->tile_sprites[gc_byteToUnsignedInt(gc, gc->tileSpriteIndex[162])].get();
                case 1: return gc->tile_sprites[gc_byteToUnsignedInt(gc, gc->tileSpriteIndex[167])].get();
                case 2: return gc->tile_sprites[gc_byteToUnsignedInt(gc, gc->tileSpriteIndex[172])].get();
                case 3: return gc->tile_sprites[gc_byteToUnsignedInt(gc, gc->tileSpriteIndex[182])].get();
            }
        }
    }
    if (n == 26 && gc_countCarsInTrain(gc, gc->trainAtCursor) > n2) {
        int n4 = gc->trainAtCursor;
        for (int i = 0; i < n2; ++i) n4 = gc->nextCar[n4];
        int n3 = gc->trainFlags[n4] & 0xF;
        switch (n3) {
            case -1: return NULL;
            case 0: return gc->tile_sprites[gc_byteToUnsignedInt(gc, gc->tileSpriteIndex[136])].get();
            case 1: return gc->tile_sprites[gc_byteToUnsignedInt(gc, gc->tileSpriteIndex[146])].get();
            case 2: return gc->tile_sprites[gc_byteToUnsignedInt(gc, gc->tileSpriteIndex[162])].get();
            case 3: return gc->tile_sprites[gc_byteToUnsignedInt(gc, gc->tileSpriteIndex[167])].get();
            case 4: return gc->tile_sprites[gc_byteToUnsignedInt(gc, gc->tileSpriteIndex[172])].get();
            case 5: return gc->tile_sprites[gc_byteToUnsignedInt(gc, gc->tileSpriteIndex[177])].get();
            case 6: return gc->tile_sprites[gc_byteToUnsignedInt(gc, gc->tileSpriteIndex[182])].get();
            case 7: return gc->tile_sprites[gc_byteToUnsignedInt(gc, gc->tileSpriteIndex[187])].get();
        }
    }
    return NULL;
}

// void drawTextBox(Graphics graphics, int n, int n2, int n3, int n4, byte[] byArray)
void gc_drawTextBox(GameCanvas *gc, Surface *g, int32_t n, int32_t n2,
                    int32_t n3, int32_t n4, int8_t *byArray) {
    int n5 = 0;
    int len = midp_arr_len(byArray);
    for (int i = 0; i < len; ++i) {
        if (byArray[i] >= 32 && byArray[i] <= 39) continue;
        ++n5;
    }
    n5 *= 6;
    if ((n4 & 1) == 1) n -= n5 / 2 + 3;
    if ((n4 & 2) == 2) n2 -= 5;
    if ((n4 & 4) == 4) n -= n5 + 6;
    if (n4 == 37) { n = 0; n2 = 15 + gcs_gameplayViewportHeight - 15; }
    midp_set_color(g, gc->dc[3 * n3]);
    midp_fill_rect(g, n, n2, n5 + 5, 13);
    midp_set_color(g, gc->dc[3 * n3 + 1]);
    midp_draw_rect(g, n, n2, n5 + 5, 13);
    midp_set_color(g, gc->dc[3 * n3 + 2]);
    midp_draw_rect(g, n, n2 + 13, n5 + 5, 0);
    midp_draw_rect(g, n + n5 + 5, n2, 0, 13);
    gc_renderFontString(gc, g, n + 4, n2 + 5, byArray, false);
}

// void drawRectFill(Graphics graphics, int n, int n2, byte by)
void gc_drawRectFill(GameCanvas *gc, Surface *g, int32_t n, int32_t n2, int8_t by) {
    int8_t *byArray = gc->spriteBitmapTable[gc->spriteIndexTable[80 + (gc->trainSelectionIndex[by] >> 8 & 0xFF)]];
    int8_t *byArray2 = gc->spriteBitmapTable[gc->spriteIndexTable[86 + (gc->trainSelectionIndex[by] & 0xFF)]];
    int la = midp_arr_len(byArray), lb = midp_arr_len(byArray2);
    int8_t *byArray3 = GC_NEW(int8_t, lb + la);
    for (int i = 0; i < lb; ++i) byArray3[i] = byArray2[i];
    for (int i = 0; i < la; ++i) byArray3[lb + i] = byArray[i];
    int n3 = 0;
    for (int i = 0; i < lb + la; ++i) {
        if (byArray3[i] >= 32 && byArray3[i] <= 39) continue;
        ++n3;
    }
    midp_set_color(g, RGB3(255, 176, 98));
    midp_fill_rect(g, n -= (n3 *= 6) / 2 + 3, n2 -= 5, n3 + 5, 11);
    midp_set_color(g, RGB3(255, 210, 170));
    midp_draw_rect(g, n, n2, n3 + 5, 11);
    midp_set_color(g, RGB3(255, 130, 0));
    midp_draw_rect(g, n, n2 + 11, n3 + 5, 0);
    midp_draw_rect(g, n + n3 + 5, n2, 0, 11);
    gc->clipX = 0;
    gc->clipY = 15;
    gc->clipWidth = gcs_screenWidth;
    gc->clipHeight = gcs_gameplayViewportHeight;
    gc_renderFontString(gc, g, n + 4, n2 + 3, byArray3, true);
}

// public void renderPackedSprite(Graphics graphics, int n)
void gc_renderPackedSprite(GameCanvas *gc, Surface *g, int32_t n) {
    int n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13;
    int8_t by;
    if (gc->lastRenderedSpriteId != n) {
        gc->spriteJustChanged = 1;
        gc->lastRenderedSpriteId = n;
    }
    if (gc->spriteJustChanged != 0) gc->needsRepaint = false;
    if (gc->mapRedrawPending) {
        gc->menuDirty = true;
        gc_renderMap(gc, g);
        gc->mapRedrawPending = false;
    }
    int16_t s = gc->spriteOffsetTable[n];
    int n14 = 65536 * gc_byteToUnsignedInt(gc, gc->spriteData[s + 1]) + 256 * gc_byteToUnsignedInt(gc, gc->spriteData[s + 2]) + gc_byteToUnsignedInt(gc, gc->spriteData[s + 3]);
    int n15 = 65536 * gc_byteToUnsignedInt(gc, gc->spriteData[s + 4]) + 256 * gc_byteToUnsignedInt(gc, gc->spriteData[s + 5]) + gc_byteToUnsignedInt(gc, gc->spriteData[s + 6]);
    int n16 = 65536 * gc_byteToUnsignedInt(gc, gc->spriteData[s + 7]) + 256 * gc_byteToUnsignedInt(gc, gc->spriteData[s + 8]) + gc_byteToUnsignedInt(gc, gc->spriteData[s + 9]);
    int n17 = 65536 * gc_byteToUnsignedInt(gc, gc->spriteData[s + 10]) + 256 * gc_byteToUnsignedInt(gc, gc->spriteData[s + 11]) + gc_byteToUnsignedInt(gc, gc->spriteData[s + 12]);
    int n18 = gc_byteToUnsignedInt(gc, gc->spriteData[s + 13]);
    int n19 = gc_byteToUnsignedInt(gc, gc->spriteData[s + 14]);
    if (gc->dialogContentHeight < n19) gc->dialogContentHeight = n19;
    else n19 = gc->dialogContentHeight;
    if (gc->cursorOverUI || gc->cursorAllowZoomOut) n18 += 6;
    gc->currentSpriteId = gc->spriteData[s + 15];
    int n20 = (gcs_screenWidth - n18) / 2;
    int n21 = (gcs_hs - n19) / 2;
    int n22 = 6, n23 = 6, n24 = 6;
    if (gc->needsRepaint) {
        midp_set_color(g, n14); midp_fill_rect(g, n20, n21, n18, n19);
        midp_set_color(g, n16); midp_draw_rect(g, n20, n21, n18, n19);
        midp_set_color(g, n15); midp_draw_rect(g, n20, n21, 0, n19);
        midp_draw_rect(g, n20, n21, n18, 0);
    }
    gc->pauseMenuOpen = false;
    if ((gc->spriteData[s] & 1) == 1) {
        if (gc->needsRepaint && gc->cursorDragY == -1) {
            midp_set_color(g, n17);
            midp_fill_rect(g, n20 + n18 - 10, n21 + 3, 7, 7);
        }
        if (gc->needsRepaint) {
            midp_set_color(g, n16);
            midp_draw_rect(g, n20 + n18 - 10, n21 + 3, 7, 7);
            midp_draw_line(g, n20 + n18 - 8, n21 + 5, n20 + n18 - 5, n21 + 8);
            midp_draw_line(g, n20 + n18 - 8, n21 + 8, n20 + n18 - 5, n21 + 5);
        }
        gc->pauseMenuOpen = true;
        n24 = 19;
    } else if (gc->cursorDragY == -1) {
        gc->cursorDragY = -3;
    }
    int n25 = s + 16;
    if ((gc->spriteData[s] & 2) == 2) {
        if (gc->needsRepaint) midp_set_color(g, n16);
        if (gc->needsRepaint) midp_draw_rect(g, n20 + 2, n21 + 13, n18 - 4, 0);
        gc->clipX = n20; gc->clipY = n21; gc->clipWidth = n18; gc->clipHeight = n19;
        if (gc->needsRepaint)
            gc_renderFontString(gc, g, n20 + 3, n21 + 5, gc_m(gc, s + 16), true);
        n25 += 1 + gc->spriteData[s + 16];
        n22 = 19; n23 = 19; n24 = 19;
    }
    int n26 = n20 + 4;
    gc->clipX = n20 + 1;
    gc->clipY = n21 + n22 - 4;
    gc->clipWidth = n18 - 2;
    gc->clipHeight = n19 - n22 + 3;
    midp_set_clip(g, gc->clipX, gc->clipY, gc->clipWidth, gc->clipHeight);
    if (gc->needsRepaint && gc->cursorOverUI) {
        midp_set_color(g, n16);
        midp_draw_rect(g, n20 + n18 - 6, n21 + n24 - 1, 4, 0);
        midp_draw_rect(g, n20 + n18 - 5, n21 + n24 - 2, 2, 0);
        midp_draw_rect(g, n20 + n18 - 4, n21 + n24 - 3, 0, 0);
    }
    if (gc->needsRepaint && gc->cursorAllowZoomOut) {
        midp_set_color(g, n16);
        midp_draw_rect(g, n20 + n18 - 6, n21 + n19 - 5, 4, 0);
        midp_draw_rect(g, n20 + n18 - 5, n21 + n19 - 4, 2, 0);
        midp_draw_rect(g, n20 + n18 - 4, n21 + n19 - 3, 0, 0);
    }
    gc->cursorSelectY = -2;
    gc->cursorSelectX = -2;
    gc->cursorOverUI = false;
    gc->cursorAllowZoomOut = false;
    gc->cursorDragX = -1;
    int n27 = gc->spriteData[n25++];
    for (n13 = 0; n13 < n27; ++n13) {
        n12 = n21 + n22 - gc->cursorScale;
        if (((by = gc->spriteData[n25++]) & 0x80) == 0) {
            int n28;
            n11 = (by & 4) == 4 ? 0 : gc->spriteData[n25++];
            n10 = (by & 2) == 2 ? 0 : gc->spriteData[n25++];
            n9 = (by & 0x40) == 64 ? 1 : 0;
            n8 = (by & 0x20) == 32 ? 1 : 0;
            n7 = n9 != 0 ? gc->spriteData[n25++] : 0;
            n6 = n8 != 0 ? gc->spriteData[n25++] : 0;
            n5 = (by & 0x10) == 16 ? gc->spriteData[n25++] : 0;
            n4 = (by & 0x10) == 16 ? gc->spriteData[n25++] : 0;
            n3 = (by & 0x10) == 16 ? gc->spriteData[n25++] : 0;
            n2 = (by & 0x10) == 16 ? gc->spriteData[n25++] : 0;
            int8_t by2 = (by & 0x10) == 16 ? gc->spriteData[n25++] : (int8_t)0;
            int8_t *byArray = n8 != 0 ? gc_resolveDialogValue(gc, gc->currentSpriteId, n6) : gc_m(gc, n25);
            int n29 = 0;
            if (byArray == NULL) continue;
            int blen = midp_arr_len(byArray);
            for (n28 = 0; n28 < blen; ++n28) {
                if (byArray[n28] >= 32 && byArray[n28] <= 39) continue;
                ++n29;
            }
            if (n8 == 0) n25 = n25 + gc->spriteData[n25] + 1;
            if (n12 + n10 + 6 + by2 >= gc->clipY + gc->clipHeight) {
                n28 = n19 + (n12 + n10 + 6 + by2 + n3 + 3 - (gc->clipHeight + gc->clipY));
                if (gc->dialogContentHeight < n28) gc->dialogContentHeight = n28;
                if (gc->dialogContentHeight > gcs_hs - 20) gc->dialogContentHeight = gcs_hs - 20;
            }
            if (gc->needsRepaint && n12 + n10 + 6 + by2 >= gc->clipY + gc->clipHeight)
                gc->cursorAllowZoomOut = true;
            if (n9 != 0 && (n9 == 0 || !gc_isResourceTile(gc, gc->currentSpriteId, n7))) continue;
            if (n12 + n10 - n3 - 3 <= gc->clipY + gc->clipHeight && n12 + n10 + 8 + by2 >= gc->clipY) {
                if ((by & 0x10) == 16) {
                    if (gc->cursorDragY == -3) gc->cursorDragY = n13;
                    if (n12 + n10 + 8 + by2 <= gc->clipY + gc->clipHeight && n12 + n10 - 3 - n3 >= gc->clipY) {
                        if (n13 < gc->cursorDragY) gc->cursorSelectY = n13;
                        if (gc->cursorSelectX == -2 && n13 > gc->cursorDragY) gc->cursorSelectX = n13;
                    }
                    if (gc->needsRepaint && gc->cursorDragY == n13) {
                        midp_set_color(g, n17);
                        midp_fill_rect(g, n26 + n11 - n4, n12 + n10 - n3 - 3, 6 * n29 + n4 + n2 - 1, 10 + n3 + by2);
                        gc->cursorDragX = n5;
                    }
                    if (gc->needsRepaint && (by & 8) == 8) {
                        midp_set_color(g, n16);
                        midp_draw_rect(g, n26 + n11 - n4, n12 + n10 - n3 - 3, 6 * n29 + n4 + n2 - 1, 10 + n3 + by2);
                        midp_set_color(g, n15);
                        midp_draw_rect(g, n26 + n11 - n4, n12 + n10 - n3 - 3, 0, 10 + n3 + by2);
                        midp_draw_rect(g, n26 + n11 - n4, n12 + n10 - n3 - 3, 6 * n29 + n4 + n2 - 1, 0);
                    }
                }
                if (gc->needsRepaint)
                    gc_renderFontString(gc, g, n26 + n11, n12 + n10, byArray, true);
            }
            n22 += 11 + n10;
            continue;
        }
        n11 = (by & 0x40) == 64 ? 1 : 0;
        n10 = gc->spriteData[n25++];
        n9 = (by & 0x20) == 32 ? 0 : gc->spriteData[n25++];
        n8 = (by & 0x10) == 16 ? 0 : gc->spriteData[n25++];
        n7 = n11 != 0 ? gc->spriteData[n25++] : 0;
        Image *image = gc_u(gc, gc->currentSpriteId, n10);
        if (image == NULL) continue;
        n5 = image->width();
        n4 = image->height();
        if (n11 != 0 && (n11 == 0 || !gc_isResourceTile(gc, gc->currentSpriteId, n7))) continue;
        if (gc->needsRepaint && n12 + n8 <= gc->clipY + gc->clipHeight && n12 + n8 + n4 >= gc->clipY)
            midp_draw_image(g, image, n26 + n9, n12 + n8, 20);
        if (gc->needsRepaint && n12 + n8 + n4 >= gc->clipY + gc->clipHeight)
            gc->cursorAllowZoomOut = true;
        n22 += n4 + n8;
    }
    n27 = gc->spriteData[n25++];
    for (n13 = 0; n13 < n27; ++n13) {
        n12 = n21 + n23 - gc->cursorScale;
        by = gc->spriteData[n25++];
        n11 = by & 7;
        n10 = 65536 * gc_byteToUnsignedInt(gc, gc->spriteData[n25++]) + 256 * gc_byteToUnsignedInt(gc, gc->spriteData[n25++]) + gc_byteToUnsignedInt(gc, gc->spriteData[n25++]);
        n9 = gc_byteToUnsignedInt(gc, gc->spriteData[n25++]) + 256 * gc_byteToUnsignedInt(gc, gc->spriteData[n25++]);
        n8 = gc_byteToUnsignedInt(gc, gc->spriteData[n25++]) + 256 * gc_byteToUnsignedInt(gc, gc->spriteData[n25++]);
        n7 = gc_byteToUnsignedInt(gc, gc->spriteData[n25++]) + 256 * gc_byteToUnsignedInt(gc, gc->spriteData[n25++]);
        n6 = gc_byteToUnsignedInt(gc, gc->spriteData[n25++]) + 256 * gc_byteToUnsignedInt(gc, gc->spriteData[n25++]);
        n9 = (int16_t)((by & 8) == 8 ? gc_neighboursMask(gc, gc->currentSpriteId, n9) : n9);
        n8 = (int16_t)((by & 0x10) == 16 ? gc_neighboursMask(gc, gc->currentSpriteId, n8) : n8);
        n7 = (int16_t)((by & 0x20) == 32 ? gc_neighboursMask(gc, gc->currentSpriteId, n7) : n7);
        n6 = (int16_t)((by & 0x40) == 64 ? gc_neighboursMask(gc, gc->currentSpriteId, n6) : n6);
        n5 = (by & 0x80) == 128 ? 1 : 0;
        n4 = n5 != 0 ? gc->spriteData[n25++] : 0;
        n3 = n12 + n8;
        n2 = n3 + n6;
        if (!gc->needsRepaint || (n5 != 0 && (n5 == 0 || !gc_isResourceTile(gc, gc->currentSpriteId, n4)))) continue;
        switch (n11) {
            case 0:
                midp_set_color(g, n10);
                midp_draw_line(g, n26 + n9, n12 + n8, n26 + n7, n12 + n6);
                n3 = imin(n12 + n8, n12 + n6);
                n2 = imax(n12 + n8, n12 + n6);
                break;
            case 1:
                midp_set_color(g, n10);
                midp_draw_rect(g, n26 + n9, n12 + n8, n7, n6);
                break;
            case 2:
                midp_set_color(g, n10);
                midp_fill_rect(g, n26 + n9, n12 + n8, n7, n6);
                break;
            case 3:
                midp_set_color(g, n10);
                midp_draw_arc(g, n26 + n9, n12 + n8, n7, n6, 0, 360);
                break;
            case 4:
                midp_set_color(g, n10);
                midp_fill_arc(g, n26 + n9, n12 + n8, n7, n6, 0, 360);
                break;
        }
        if (!gc->needsRepaint || n2 < gc->clipY + gc->clipHeight) continue;
        gc->cursorAllowZoomOut = true;
    }
    if (gc->needsRepaint) {
        if (gc->cursorScale == 0 && gc->cursorSelectY == -2) gc->cursorSelectY = -1;
        if (gc->cursorScale > 0) gc->cursorOverUI = true;
    }
    gc->needsRepaint = true;
    if (gc->spriteJustChanged > 0) --gc->spriteJustChanged;
}
