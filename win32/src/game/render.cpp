// Packed-sprite UI and text drawing.
// renderPackedSprite draws the menu/dialog layouts (state 20). renderFontString
// draws text from the `ca[]` font atlas. u() resolves per-slot sub-sprites.
#include "game_methods.h"

#define RGB3(r, g, b) (((r) << 16) | ((g) << 8) | (b))
static int imin(int a, int b) { return a < b ? a : b; }
static int imax(int a, int b) { return a > b ? a : b; }

void game_renderFontString(Game *game, Surface *g, int32_t n, int32_t n2,
                        std::span<const int8_t> codes, bool bl) {
    int n3 = n;
    midp_set_color(g, 0);
    const int len = static_cast<int>(codes.size());
    for (int i = 0; i < len; ++i) {
        int8_t by = codes[i];
        if (bl) {
            midp_set_clip(g, game->renderer.clipX, game->renderer.clipY, game->renderer.clipWidth, game->renderer.clipHeight);
            midp_clip_rect(g, n3, n2 - 4, 5, 12);
        } else {
            midp_set_clip(g, n3, n2 - 4, 5, 12);
        }
        if (by == 0) { n3 += 6; continue; }
        if (by < 32) {
            midp_draw_image(g, game->assets.font_images[(by - 1) / 16].get(), n3 - 5 * ((by - 1) % 16), n2, 20);
            n3 += 6;
            continue;
        }
        if (by < 40) {
            int8_t by2 = (int8_t)(codes[i + 1] >= 40 && codes[i + 1] <= 65 ? 1 : 0);
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
            midp_draw_image(g, game->assets.font_images[2 + (by - 40) / 16].get(), n3 - 5 * ((by - 40) % 16), n2, 20);
            n3 += 6;
            continue;
        }
        if (by >= 100) continue;
        midp_draw_image(g, game->assets.font_images[4 + (by - 70) / 16].get(), n3 - 5 * ((by - 70) % 16), n2, 20);
        n3 += 6;
    }
    if (bl) {
        midp_set_clip(g, game->renderer.clipX, game->renderer.clipY, game->renderer.clipWidth, game->renderer.clipHeight);
    } else {
        midp_set_clip(g, 0, 0, gcs_screenWidth, gcs_hs);
    }
}

// Returns the tile sprite image for a wagon/car slot. contextCode 23 is the
// build-menu wagon preview (slotIndex 0 = the engine chosen by wagonType, higher
// slots = the offered wagon in wagonOptionSlots); contextCode 26 is the
// slotIndex-th car of the train under the cursor. Returns NULL when the slot has
// no sprite.
Image *game_wagonCarSprite(Game *game, int32_t contextCode, int32_t slotIndex) {
    if (contextCode == 23) {
        if (slotIndex == 0 && game->build.wagonType >= 0 && game->build.wagonType < 3)
            return game->assets.tile_sprites[game_byteToUnsignedInt(game, game->assets.tileSpriteIndex[136])].get();
        if (slotIndex == 0 && game->build.wagonType >= 3 && game->build.wagonType < 6)
            return game->assets.tile_sprites[game_byteToUnsignedInt(game, game->assets.tileSpriteIndex[146])].get();
        if (slotIndex > 0) {
            switch (game->build.wagonOptionSlots[slotIndex - 1]) {
                case -1: return NULL;
                case 0: return game->assets.tile_sprites[game_byteToUnsignedInt(game, game->assets.tileSpriteIndex[162])].get();
                case 1: return game->assets.tile_sprites[game_byteToUnsignedInt(game, game->assets.tileSpriteIndex[167])].get();
                case 2: return game->assets.tile_sprites[game_byteToUnsignedInt(game, game->assets.tileSpriteIndex[172])].get();
                case 3: return game->assets.tile_sprites[game_byteToUnsignedInt(game, game->assets.tileSpriteIndex[182])].get();
            }
        }
    }
    if (contextCode == 26 && game_countCarsInTrain(game, game->trainAtCursor) > slotIndex) {
        int carSlot = game->trainAtCursor;
        for (int i = 0; i < slotIndex; ++i) carSlot = game->fleet.nextCar[carSlot];
        int carType = game->fleet.trainFlags[carSlot] & 0xF;
        switch (carType) {
            case -1: return NULL;
            case 0: return game->assets.tile_sprites[game_byteToUnsignedInt(game, game->assets.tileSpriteIndex[136])].get();
            case 1: return game->assets.tile_sprites[game_byteToUnsignedInt(game, game->assets.tileSpriteIndex[146])].get();
            case 2: return game->assets.tile_sprites[game_byteToUnsignedInt(game, game->assets.tileSpriteIndex[162])].get();
            case 3: return game->assets.tile_sprites[game_byteToUnsignedInt(game, game->assets.tileSpriteIndex[167])].get();
            case 4: return game->assets.tile_sprites[game_byteToUnsignedInt(game, game->assets.tileSpriteIndex[172])].get();
            case 5: return game->assets.tile_sprites[game_byteToUnsignedInt(game, game->assets.tileSpriteIndex[177])].get();
            case 6: return game->assets.tile_sprites[game_byteToUnsignedInt(game, game->assets.tileSpriteIndex[182])].get();
            case 7: return game->assets.tile_sprites[game_byteToUnsignedInt(game, game->assets.tileSpriteIndex[187])].get();
        }
    }
    return NULL;
}

void game_drawTextBox(Game *game, Surface *g, int32_t n, int32_t n2,
                    int32_t n3, int32_t n4,
                    std::span<const int8_t> codes) {
    int n5 = 0;
    const int len = static_cast<int>(codes.size());
    for (int i = 0; i < len; ++i) {
        if (codes[i] >= 32 && codes[i] <= 39) continue;
        ++n5;
    }
    n5 *= 6;
    if ((n4 & 1) == 1) n -= n5 / 2 + 3;
    if ((n4 & 2) == 2) n2 -= 5;
    if ((n4 & 4) == 4) n -= n5 + 6;
    if (n4 == 37) { n = 0; n2 = 15 + gcs_gameplayViewportHeight - 15; }
    midp_set_color(g, game->graphColors[3 * n3]);
    midp_fill_rect(g, n, n2, n5 + 5, 13);
    midp_set_color(g, game->graphColors[3 * n3 + 1]);
    midp_draw_rect(g, n, n2, n5 + 5, 13);
    midp_set_color(g, game->graphColors[3 * n3 + 2]);
    midp_draw_rect(g, n, n2 + 13, n5 + 5, 0);
    midp_draw_rect(g, n + n5 + 5, n2, 0, 13);
    game_renderFontString(game, g, n + 4, n2 + 5, codes, false);
}

void game_drawRectFill(Game *game, Surface *g, int32_t n, int32_t n2, int8_t by) {
    const auto engine_codes = game_tracked_codes(game->assets.spriteBitmapTable[
        game->assets.spriteIndexTable[80 + (game->ui.trainSelectionIndex[by] >> 8 & 0xFF)]]);
    const auto wagon_codes = game_tracked_codes(game->assets.spriteBitmapTable[
        game->assets.spriteIndexTable[86 + (game->ui.trainSelectionIndex[by] & 0xFF)]]);
    std::vector<int8_t> train_codes;
    train_codes.reserve(wagon_codes.size() + engine_codes.size());
    train_codes.insert(train_codes.end(), wagon_codes.begin(), wagon_codes.end());
    train_codes.insert(train_codes.end(), engine_codes.begin(), engine_codes.end());
    int n3 = 0;
    for (const int8_t code : train_codes) {
        if (code >= 32 && code <= 39) continue;
        ++n3;
    }
    midp_set_color(g, RGB3(255, 176, 98));
    midp_fill_rect(g, n -= (n3 *= 6) / 2 + 3, n2 -= 5, n3 + 5, 11);
    midp_set_color(g, RGB3(255, 210, 170));
    midp_draw_rect(g, n, n2, n3 + 5, 11);
    midp_set_color(g, RGB3(255, 130, 0));
    midp_draw_rect(g, n, n2 + 11, n3 + 5, 0);
    midp_draw_rect(g, n + n3 + 5, n2, 0, 11);
    game->renderer.clipX = 0;
    game->renderer.clipY = 15;
    game->renderer.clipWidth = gcs_screenWidth;
    game->renderer.clipHeight = gcs_gameplayViewportHeight;
    game_renderFontString(game, g, n + 4, n2 + 3, train_codes, true);
}

void game_renderPackedSprite(Game *game, Surface *g, int32_t n) {
    int n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13;
    int8_t by;
    if (game->renderer.lastRenderedSpriteId != n) {
        game->renderer.spriteJustChanged = 1;
        game->renderer.lastRenderedSpriteId = n;
    }
    if (game->renderer.spriteJustChanged != 0) game->renderer.needsRepaint = false;
    if (game->renderer.mapRedrawPending) {
        game->ui.menuDirty = true;
        game_renderMap(game, g);
        game->renderer.mapRedrawPending = false;
    }
    int16_t s = game->assets.spriteOffsetTable[n];
    int n14 = 65536 * game_byteToUnsignedInt(game, game->assets.spriteData[s + 1]) + 256 * game_byteToUnsignedInt(game, game->assets.spriteData[s + 2]) + game_byteToUnsignedInt(game, game->assets.spriteData[s + 3]);
    int n15 = 65536 * game_byteToUnsignedInt(game, game->assets.spriteData[s + 4]) + 256 * game_byteToUnsignedInt(game, game->assets.spriteData[s + 5]) + game_byteToUnsignedInt(game, game->assets.spriteData[s + 6]);
    int n16 = 65536 * game_byteToUnsignedInt(game, game->assets.spriteData[s + 7]) + 256 * game_byteToUnsignedInt(game, game->assets.spriteData[s + 8]) + game_byteToUnsignedInt(game, game->assets.spriteData[s + 9]);
    int n17 = 65536 * game_byteToUnsignedInt(game, game->assets.spriteData[s + 10]) + 256 * game_byteToUnsignedInt(game, game->assets.spriteData[s + 11]) + game_byteToUnsignedInt(game, game->assets.spriteData[s + 12]);
    int n18 = game_byteToUnsignedInt(game, game->assets.spriteData[s + 13]);
    int n19 = game_byteToUnsignedInt(game, game->assets.spriteData[s + 14]);
    if (game->renderer.dialogContentHeight < n19) game->renderer.dialogContentHeight = n19;
    else n19 = game->renderer.dialogContentHeight;
    if (game->ui.cursorOverUI || game->ui.cursorAllowZoomOut) n18 += 6;
    game->renderer.currentSpriteId = game->assets.spriteData[s + 15];
    int n20 = (gcs_screenWidth - n18) / 2;
    int n21 = (gcs_hs - n19) / 2;
    int n22 = 6, n23 = 6, n24 = 6;
    if (game->renderer.needsRepaint) {
        midp_set_color(g, n14); midp_fill_rect(g, n20, n21, n18, n19);
        midp_set_color(g, n16); midp_draw_rect(g, n20, n21, n18, n19);
        midp_set_color(g, n15); midp_draw_rect(g, n20, n21, 0, n19);
        midp_draw_rect(g, n20, n21, n18, 0);
    }
    game->ui.pauseMenuOpen = false;
    if ((game->assets.spriteData[s] & 1) == 1) {
        if (game->renderer.needsRepaint && game->ui.cursorDragY == -1) {
            midp_set_color(g, n17);
            midp_fill_rect(g, n20 + n18 - 10, n21 + 3, 7, 7);
        }
        if (game->renderer.needsRepaint) {
            midp_set_color(g, n16);
            midp_draw_rect(g, n20 + n18 - 10, n21 + 3, 7, 7);
            midp_draw_line(g, n20 + n18 - 8, n21 + 5, n20 + n18 - 5, n21 + 8);
            midp_draw_line(g, n20 + n18 - 8, n21 + 8, n20 + n18 - 5, n21 + 5);
        }
        game->ui.pauseMenuOpen = true;
        n24 = 19;
    } else if (game->ui.cursorDragY == -1) {
        game->ui.cursorDragY = -3;
    }
    int n25 = s + 16;
    if ((game->assets.spriteData[s] & 2) == 2) {
        if (game->renderer.needsRepaint) midp_set_color(g, n16);
        if (game->renderer.needsRepaint) midp_draw_rect(g, n20 + 2, n21 + 13, n18 - 4, 0);
        game->renderer.clipX = n20; game->renderer.clipY = n21; game->renderer.clipWidth = n18; game->renderer.clipHeight = n19;
        if (game->renderer.needsRepaint)
            game_renderFontString(game, g, n20 + 3, n21 + 5,
                                game_readPackedTextCodes(game, s + 16), true);
        n25 += 1 + game->assets.spriteData[s + 16];
        n22 = 19; n23 = 19; n24 = 19;
    }
    int n26 = n20 + 4;
    game->renderer.clipX = n20 + 1;
    game->renderer.clipY = n21 + n22 - 4;
    game->renderer.clipWidth = n18 - 2;
    game->renderer.clipHeight = n19 - n22 + 3;
    midp_set_clip(g, game->renderer.clipX, game->renderer.clipY, game->renderer.clipWidth, game->renderer.clipHeight);
    if (game->renderer.needsRepaint && game->ui.cursorOverUI) {
        midp_set_color(g, n16);
        midp_draw_rect(g, n20 + n18 - 6, n21 + n24 - 1, 4, 0);
        midp_draw_rect(g, n20 + n18 - 5, n21 + n24 - 2, 2, 0);
        midp_draw_rect(g, n20 + n18 - 4, n21 + n24 - 3, 0, 0);
    }
    if (game->renderer.needsRepaint && game->ui.cursorAllowZoomOut) {
        midp_set_color(g, n16);
        midp_draw_rect(g, n20 + n18 - 6, n21 + n19 - 5, 4, 0);
        midp_draw_rect(g, n20 + n18 - 5, n21 + n19 - 4, 2, 0);
        midp_draw_rect(g, n20 + n18 - 4, n21 + n19 - 3, 0, 0);
    }
    game->ui.cursorSelectY = -2;
    game->ui.cursorSelectX = -2;
    game->ui.cursorOverUI = false;
    game->ui.cursorAllowZoomOut = false;
    game->ui.cursorDragX = -1;
    int n27 = game->assets.spriteData[n25++];
    for (n13 = 0; n13 < n27; ++n13) {
        n12 = n21 + n22 - game->ui.cursorScale;
        if (((by = game->assets.spriteData[n25++]) & 0x80) == 0) {
            int n28;
            n11 = (by & 4) == 4 ? 0 : game->assets.spriteData[n25++];
            n10 = (by & 2) == 2 ? 0 : game->assets.spriteData[n25++];
            n9 = (by & 0x40) == 64 ? 1 : 0;
            n8 = (by & 0x20) == 32 ? 1 : 0;
            n7 = n9 != 0 ? game->assets.spriteData[n25++] : 0;
            n6 = n8 != 0 ? game->assets.spriteData[n25++] : 0;
            n5 = (by & 0x10) == 16 ? game->assets.spriteData[n25++] : 0;
            n4 = (by & 0x10) == 16 ? game->assets.spriteData[n25++] : 0;
            n3 = (by & 0x10) == 16 ? game->assets.spriteData[n25++] : 0;
            n2 = (by & 0x10) == 16 ? game->assets.spriteData[n25++] : 0;
            int8_t by2 = (by & 0x10) == 16 ? game->assets.spriteData[n25++] : (int8_t)0;
            std::optional<std::vector<int8_t>> text_codes = n8 != 0
                ? game_resolveDialogValue(game, game->renderer.currentSpriteId, n6)
                : std::optional<std::vector<int8_t>>(
                    game_readPackedTextCodes(game, n25));
            int n29 = 0;
            if (!text_codes) continue;
            const int blen = static_cast<int>(text_codes->size());
            for (n28 = 0; n28 < blen; ++n28) {
                if ((*text_codes)[n28] >= 32 && (*text_codes)[n28] <= 39) continue;
                ++n29;
            }
            if (n8 == 0) n25 = n25 + game->assets.spriteData[n25] + 1;
            if (n12 + n10 + 6 + by2 >= game->renderer.clipY + game->renderer.clipHeight) {
                n28 = n19 + (n12 + n10 + 6 + by2 + n3 + 3 - (game->renderer.clipHeight + game->renderer.clipY));
                if (game->renderer.dialogContentHeight < n28) game->renderer.dialogContentHeight = n28;
                if (game->renderer.dialogContentHeight > gcs_hs - 20) game->renderer.dialogContentHeight = gcs_hs - 20;
            }
            if (game->renderer.needsRepaint && n12 + n10 + 6 + by2 >= game->renderer.clipY + game->renderer.clipHeight)
                game->ui.cursorAllowZoomOut = true;
            if (n9 != 0 && (n9 == 0 || !game_isResourceTile(game, game->renderer.currentSpriteId, n7))) continue;
            if (n12 + n10 - n3 - 3 <= game->renderer.clipY + game->renderer.clipHeight && n12 + n10 + 8 + by2 >= game->renderer.clipY) {
                if ((by & 0x10) == 16) {
                    if (game->ui.cursorDragY == -3) game->ui.cursorDragY = n13;
                    if (n12 + n10 + 8 + by2 <= game->renderer.clipY + game->renderer.clipHeight && n12 + n10 - 3 - n3 >= game->renderer.clipY) {
                        if (n13 < game->ui.cursorDragY) game->ui.cursorSelectY = n13;
                        if (game->ui.cursorSelectX == -2 && n13 > game->ui.cursorDragY) game->ui.cursorSelectX = n13;
                    }
                    if (game->renderer.needsRepaint && game->ui.cursorDragY == n13) {
                        midp_set_color(g, n17);
                        midp_fill_rect(g, n26 + n11 - n4, n12 + n10 - n3 - 3, 6 * n29 + n4 + n2 - 1, 10 + n3 + by2);
                        game->ui.cursorDragX = n5;
                    }
                    if (game->renderer.needsRepaint && (by & 8) == 8) {
                        midp_set_color(g, n16);
                        midp_draw_rect(g, n26 + n11 - n4, n12 + n10 - n3 - 3, 6 * n29 + n4 + n2 - 1, 10 + n3 + by2);
                        midp_set_color(g, n15);
                        midp_draw_rect(g, n26 + n11 - n4, n12 + n10 - n3 - 3, 0, 10 + n3 + by2);
                        midp_draw_rect(g, n26 + n11 - n4, n12 + n10 - n3 - 3, 6 * n29 + n4 + n2 - 1, 0);
                    }
                }
                if (game->renderer.needsRepaint)
                    game_renderFontString(game, g, n26 + n11, n12 + n10,
                                        *text_codes, true);
            }
            n22 += 11 + n10;
            continue;
        }
        n11 = (by & 0x40) == 64 ? 1 : 0;
        n10 = game->assets.spriteData[n25++];
        n9 = (by & 0x20) == 32 ? 0 : game->assets.spriteData[n25++];
        n8 = (by & 0x10) == 16 ? 0 : game->assets.spriteData[n25++];
        n7 = n11 != 0 ? game->assets.spriteData[n25++] : 0;
        Image *image = game_wagonCarSprite(game, game->renderer.currentSpriteId, n10);
        if (image == NULL) continue;
        n5 = image->width();
        n4 = image->height();
        if (n11 != 0 && (n11 == 0 || !game_isResourceTile(game, game->renderer.currentSpriteId, n7))) continue;
        if (game->renderer.needsRepaint && n12 + n8 <= game->renderer.clipY + game->renderer.clipHeight && n12 + n8 + n4 >= game->renderer.clipY)
            midp_draw_image(g, image, n26 + n9, n12 + n8, 20);
        if (game->renderer.needsRepaint && n12 + n8 + n4 >= game->renderer.clipY + game->renderer.clipHeight)
            game->ui.cursorAllowZoomOut = true;
        n22 += n4 + n8;
    }
    n27 = game->assets.spriteData[n25++];
    for (n13 = 0; n13 < n27; ++n13) {
        n12 = n21 + n23 - game->ui.cursorScale;
        by = game->assets.spriteData[n25++];
        n11 = by & 7;
        n10 = 65536 * game_byteToUnsignedInt(game, game->assets.spriteData[n25++]) + 256 * game_byteToUnsignedInt(game, game->assets.spriteData[n25++]) + game_byteToUnsignedInt(game, game->assets.spriteData[n25++]);
        n9 = game_byteToUnsignedInt(game, game->assets.spriteData[n25++]) + 256 * game_byteToUnsignedInt(game, game->assets.spriteData[n25++]);
        n8 = game_byteToUnsignedInt(game, game->assets.spriteData[n25++]) + 256 * game_byteToUnsignedInt(game, game->assets.spriteData[n25++]);
        n7 = game_byteToUnsignedInt(game, game->assets.spriteData[n25++]) + 256 * game_byteToUnsignedInt(game, game->assets.spriteData[n25++]);
        n6 = game_byteToUnsignedInt(game, game->assets.spriteData[n25++]) + 256 * game_byteToUnsignedInt(game, game->assets.spriteData[n25++]);
        n9 = (int16_t)((by & 8) == 8 ? game_neighboursMask(game, game->renderer.currentSpriteId, n9) : n9);
        n8 = (int16_t)((by & 0x10) == 16 ? game_neighboursMask(game, game->renderer.currentSpriteId, n8) : n8);
        n7 = (int16_t)((by & 0x20) == 32 ? game_neighboursMask(game, game->renderer.currentSpriteId, n7) : n7);
        n6 = (int16_t)((by & 0x40) == 64 ? game_neighboursMask(game, game->renderer.currentSpriteId, n6) : n6);
        n5 = (by & 0x80) == 128 ? 1 : 0;
        n4 = n5 != 0 ? game->assets.spriteData[n25++] : 0;
        n3 = n12 + n8;
        n2 = n3 + n6;
        if (!game->renderer.needsRepaint || (n5 != 0 && (n5 == 0 || !game_isResourceTile(game, game->renderer.currentSpriteId, n4)))) continue;
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
        if (!game->renderer.needsRepaint || n2 < game->renderer.clipY + game->renderer.clipHeight) continue;
        game->ui.cursorAllowZoomOut = true;
    }
    if (game->renderer.needsRepaint) {
        if (game->ui.cursorScale == 0 && game->ui.cursorSelectY == -2) game->ui.cursorSelectY = -1;
        if (game->ui.cursorScale > 0) game->ui.cursorOverUI = true;
    }
    game->renderer.needsRepaint = true;
    if (game->renderer.spriteJustChanged > 0) --game->renderer.spriteJustChanged;
}
