// Game-map header, bottom bar, and overlays.
#include "game_methods.h"

#include <algorithm>
#include <stdio.h>
#include <string.h>

static std::vector<int8_t> make_font_bytes(const char *text,
                                           bool money_prefix) {
    int text_len = (int)strlen(text);
    int offset = money_prefix ? 1 : 0;
    std::vector<int8_t> bytes(static_cast<size_t>(text_len + offset));
    if (money_prefix) bytes[0] = 24;
    for (int i = 0; i < text_len; ++i) {
        char ch = text[i];
        if (ch >= '0' && ch <= '9') bytes[i + offset] = (int8_t)(ch - 47);
        else if (ch == '.' || ch == ':') bytes[i + offset] = 11;
        else if (ch == '-') bytes[i + offset] = 22;
    }
    return bytes;
}

static void format_money(int32_t value, char *out, int out_size) {
    int64_t magnitude = value < 0 ? -(int64_t)value : value;
    char digits[24];
    snprintf(digits, sizeof(digits), "%lld", (long long)magnitude);
    int digit_count = (int)strlen(digits);
    int pos = 0;
    if (value < 0 && pos < out_size - 1) out[pos++] = '-';
    for (int i = 0; i < digit_count && pos < out_size - 1; ++i) {
        if (i > 0 && (digit_count - i) % 3 == 0 && pos < out_size - 1)
            out[pos++] = ':';
        out[pos++] = digits[i];
    }
    out[pos] = '\0';
}

// int drawMenuRow(Graphics,int,int,int,boolean)
int32_t gc_drawMenuRow(GameCanvas *gc, Surface *graphics, int32_t sprite,
                       int32_t item_index, int32_t x, bool spaced) {
    gc_drawSpriteQuad(gc, graphics, sprite, x, gcs_bottomBarY, 20);
    if (gc->buildMenuItemIndex == item_index) gc->selectedMenuRowX = x;
    return x + 15 + (spaced ? 3 : 0);
}

// void drawHeaderBar(Graphics)
void gc_drawHeaderBar(GameCanvas *gc, Surface *graphics) {
    if (gc->cachedHeaderDay == gc->economy.dayOfMonth
        && gc->cachedHeaderMonth == gc->economy.monthOfYear
        && gc->cachedHeaderYear == gc->economy.year
        && gc->cachedHeaderMoney == gc->economy.money && !gc->menuDirty)
        return;

    midp_set_color(graphics, 0x909090);
    midp_fill_rect(graphics, 0, 0, gcs_screenWidth, 14);
    midp_set_color(graphics, 0xBBBBBB);
    midp_draw_rect(graphics, 0, 0, gcs_screenWidth, 14);
    midp_set_color(graphics, 0x222222);
    midp_draw_line(graphics, 1, 14, gcs_screenWidth, 14);
    midp_draw_line(graphics, gcs_screenWidth - 1, 1, gcs_screenWidth - 1, 14);

    char money_text[32];
    format_money(gc->economy.money, money_text, sizeof(money_text));
    const auto money_bytes = make_font_bytes(money_text, true);
    gc_renderFontString(gc, graphics, 1, 5, money_bytes, false);

    char date_text[24];
    if (gcs_screenWidth < 140)
        snprintf(date_text, sizeof(date_text), "%02d.%02d.%d",
                 gc->economy.dayOfMonth, gc->economy.monthOfYear, gc->economy.year - 1900);
    else
        snprintf(date_text, sizeof(date_text), "%02d.%02d.%d",
                 gc->economy.dayOfMonth, gc->economy.monthOfYear, gc->economy.year);
    const auto date_bytes = make_font_bytes(date_text, false);
    gc_renderFontString(gc, graphics, gcs_screenWidth - (gcs_screenWidth < 140 ? 49 : 61),
                        5, date_bytes, false);

    gc->cachedHeaderMoney = gc->economy.money;
    gc->cachedHeaderDay = gc->economy.dayOfMonth;
    gc->cachedHeaderMonth = gc->economy.monthOfYear;
    gc->cachedHeaderYear = gc->economy.year;
}

// void drawConfirmBar(Graphics)
void gc_drawConfirmBar(GameCanvas *gc, Surface *graphics) {
    gc_drawHeaderBar(gc, graphics);
    midp_set_color(graphics, 0x606060);
    midp_fill_rect(graphics, 0, gcs_bottomBarY, gcs_screenWidth, 15);
    gc_drawMenuRow(gc, graphics, 201, 10, 0, false);
    gc_drawMenuRow(gc, graphics, 235, 10, 18, false);
    midp_set_color(graphics, 0xFFFFFF);
    midp_draw_rect(graphics, 18 * gc->confirmToggle, gcs_bottomBarY, 14, 14);
    midp_draw_rect(graphics, 18 * gc->confirmToggle + 1, gcs_bottomBarY + 1, 12, 12);
}

// void paintMain(Graphics)
void gc_paintMain(GameCanvas *gc, Surface *graphics) {
    (void)graphics;
    gc_drawHeaderBar(gc, gc->backbuffer_surface);
    if (!gc->menuDirty) return;
    midp_set_color(gc->backbuffer_surface, 0x606060);
    midp_fill_rect(gc->backbuffer_surface, 0, gcs_bottomBarY, gcs_screenWidth, 15);
    int selected_x = gc_byteToUnsignedInt(gc, gc->menuItemCode[gc->buildMenuMode]
                                               [gc->buildMenuItemIndex]);
    int scroll = selected_x + 15 - gcs_screenWidth;
    if (scroll < 0) scroll = 0;
    int item_count = static_cast<int>(gc->buildMenuItems[gc->buildMenuMode].size());
    for (int item = 0; item < item_count; ++item) {
        int x = gc_byteToUnsignedInt(gc, gc->menuItemCode[gc->buildMenuMode][item]) - scroll;
        int sprite = gc_byteToUnsignedInt(gc, gc->menuTextIndex[gc->buildMenuMode][item]);
        gc_drawSpriteQuad(gc, gc->backbuffer_surface, sprite, x, gcs_bottomBarY, 20);
        if (item != gc->buildMenuItemIndex) continue;
        midp_set_color(gc->backbuffer_surface, 0xFFFFFF);
        midp_draw_rect(gc->backbuffer_surface, x, gcs_bottomBarY, 14, 14);
        midp_draw_rect(gc->backbuffer_surface, x + 1, gcs_bottomBarY + 1, 12, 12);
    }
    if (gc->buildMenuMode == 2) {
        char cost_text[24];
        int64_t magnitude = gc->build.selectionCost < 0
                            ? -(int64_t)gc->build.selectionCost : gc->build.selectionCost;
        snprintf(cost_text, sizeof(cost_text), "%lld", (long long)magnitude);
        int text_len = (int)strlen(cost_text);
        const auto currency_codes = gc_tracked_codes(
            gc->assets.spriteBitmapTable[gc->assets.spriteIndexTable[10]]);
        std::vector<int8_t> bytes(
            static_cast<size_t>(text_len + 1) + currency_codes.size());
        bytes[0] = 24;
        for (int i = 0; i < text_len; ++i) bytes[i + 1] = (int8_t)(cost_text[i] - 47);
        std::copy(currency_codes.begin(), currency_codes.end(),
                  bytes.begin() + text_len + 1);
        gc_renderFontString(gc, gc->backbuffer_surface, 35, gcs_bottomBarY + 5, bytes, false);
    }
    gc->menuDirty = false;
}

// int drawNumber(Graphics,int,int)
int32_t gc_drawNumber(GameCanvas *gc, Surface *graphics,
                      int32_t value, int32_t y) {
    int64_t magnitude = value < 0 ? -(int64_t)value : value;
    char text[24];
    snprintf(text, sizeof(text), "%lld", (long long)magnitude);
    const auto bytes = make_font_bytes(text, true);
    gc_drawTextBox(gc, graphics, gcs_screenWidth, 15 + y, value < 0 ? 1 : 2, 4, bytes);
    return y + 15;
}

// void drawMoneyTicker(Graphics)
void gc_drawMoneyTicker(GameCanvas *gc, Surface *graphics) {
    midp_set_color(graphics, 0xFF0000);
    int y = 0;
    if (gc->moneyTickerFrames[2] > 0) {
        --gc->moneyTickerFrames[2];
        y = gc_drawNumber(gc, graphics, gc->economy.moneyHistory[2], y);
    }
    if (gc->moneyTickerFrames[1] > 0) {
        --gc->moneyTickerFrames[1];
        y = gc_drawNumber(gc, graphics, gc->economy.moneyHistory[1], y);
    }
    if (gc->moneyTickerFrames[0] > 0) {
        --gc->moneyTickerFrames[0];
        gc_drawNumber(gc, graphics, gc->economy.moneyHistory[0], y);
    }
}

// void drawConfirmDialog(Graphics)
void gc_drawConfirmDialog(GameCanvas *gc, Surface *graphics) {
    if (gc->confirmCounter <= 0) return;
    if (gc->confirmTargetState >= 0) {
        gc_drawTextBox(gc, graphics, 10, 10, 0, 37,
                       gc_tracked_codes(gc->assets.spriteBitmapTable[
                           gc->assets.spriteIndexTable[120 + gc->confirmTargetState]]));
    }
    --gc->confirmCounter;
}

// void computeIsoCornersInlined(int,int,int[],int[],boolean)
void gc_computeIsoCornersInlined(GameCanvas *gc, int32_t x, int32_t y,
                                 int32_t *screen_x, int32_t *screen_y,
                                 bool viewport_relative) {
    int center_x = x * 16 + y * 16 + 16;
    int center_y = gc->world.mapHalfWidth - x * 8 + y * 8 + 8;
    screen_x[0] = center_x - 16;
    screen_x[1] = center_x;
    screen_x[2] = center_x;
    screen_x[3] = center_x + 16;
    screen_y[0] = center_y - 4 * gc->world.heightMap[x][y];
    screen_y[1] = center_y - 8 - 4 * gc->world.heightMap[x + 1][y];
    screen_y[2] = center_y + 8 - 4 * gc->world.heightMap[x][y + 1];
    screen_y[3] = center_y - 4 * gc->world.heightMap[x + 1][y + 1];
    if (viewport_relative) {
        for (int corner = 0; corner < 4; ++corner) {
            screen_x[corner] -= gc->viewportCenterX;
            screen_y[corner] -= gc->viewportCenterY;
        }
    }
}

// void drawSelectionQuad(Graphics,int[],int[])
void gc_drawSelectionQuad(GameCanvas *gc, Surface *graphics,
                          int32_t *screen_x, int32_t *screen_y) {
    (void)gc;
    midp_draw_line(graphics, screen_x[0], screen_y[0] + 15,
                   screen_x[1], screen_y[1] + 15);
    midp_draw_line(graphics, screen_x[0] + 1, screen_y[0] + 15,
                   screen_x[1] + 1, screen_y[1] + 15);
    midp_draw_line(graphics, screen_x[0], screen_y[0] + 15,
                   screen_x[2], screen_y[2] + 15);
    midp_draw_line(graphics, screen_x[0] + 1, screen_y[0] + 15,
                   screen_x[2] + 1, screen_y[2] + 15);
    midp_draw_line(graphics, screen_x[1], screen_y[1] + 15,
                   screen_x[3], screen_y[3] + 15);
    midp_draw_line(graphics, screen_x[1] + 1, screen_y[1] + 15,
                   screen_x[3] + 1, screen_y[3] + 15);
    midp_draw_line(graphics, screen_x[2], screen_y[2] + 15,
                   screen_x[3], screen_y[3] + 15);
    midp_draw_line(graphics, screen_x[2] + 1, screen_y[2] + 15,
                   screen_x[3] + 1, screen_y[3] + 15);
}

// void drawBuildMenuCursor(Graphics)
void gc_drawBuildMenuCursor(GameCanvas *gc, Surface *graphics) {
    if (gc->blockBuildMenu) return;
    int action = gc->menuTransitionCode[gc->buildMenuMode][gc->buildMenuItemIndex];
    if (action == 0) return;
    if (action >= 1 && action <= 7) {
        bool buildable = gc_isTileBuildable(gc, gc->tileCursorX, gc->tileCursorY,
                                            action - 1);
        midp_set_color(graphics, buildable ? 0x00FF00 : 0xFF0000);
    }
    if (action == 6 || action == 7) {
        int32_t quad_x[4];
        int32_t quad_y[4];
        if (action == 6) {
            if (gc->tileCursorY > 0) {
                gc_computeIsoCornersInlined(gc, gc->tileCursorX, gc->tileCursorY - 1,
                                            quad_x, quad_y, true);
                gc_drawSelectionQuad(gc, graphics, quad_x, quad_y);
            }
            if (gc->tileCursorY < gc->world.mapWidth - 1) {
                gc_computeIsoCornersInlined(gc, gc->tileCursorX, gc->tileCursorY + 1,
                                            quad_x, quad_y, true);
                gc_drawSelectionQuad(gc, graphics, quad_x, quad_y);
            }
        } else {
            if (gc->tileCursorX > 0) {
                gc_computeIsoCornersInlined(gc, gc->tileCursorX - 1, gc->tileCursorY,
                                            quad_x, quad_y, true);
                gc_drawSelectionQuad(gc, graphics, quad_x, quad_y);
            }
            if (gc->tileCursorX < gc->world.mapWidth - 1) {
                gc_computeIsoCornersInlined(gc, gc->tileCursorX + 1, gc->tileCursorY,
                                            quad_x, quad_y, true);
                gc_drawSelectionQuad(gc, graphics, quad_x, quad_y);
            }
        }
    }
    if (action == 8)
        midp_set_color(graphics,
                       gc_isPlaceableTerrain(gc, gc->tileCursorX, gc->tileCursorY)
                       ? 0x00FF00 : 0xFF0000);
    if (action == 9)
        midp_set_color(graphics,
                       gc_isCityOrStationTile(gc, gc->tileCursorX, gc->tileCursorY)
                       ? 0x00FF00 : 0xFF0000);
    if (action == 10) midp_set_color(graphics, 0xFFFFFF);
    if (action == 11) {
        if (gc_isCurveTile(gc, gc->tileCursorX, gc->tileCursorY)) {
            int x = gc->tileCursorX;
            int y = gc->tileCursorY;
            int dx = 0;
            int dy = 0;
            if (gc->world.tileMap[x][y] == 7) dy = -1;
            if (gc->world.tileMap[x][y] == 10) dy = 1;
            if (gc->world.tileMap[x][y] == 8) dx = -1;
            if (gc->world.tileMap[x][y] == 9) dx = 1;
            midp_set_color(graphics, 0x00FF00);
            int32_t quad_x[4];
            int32_t quad_y[4];
            while ((x += dx) >= 0 && (y += dy) >= 0
                   && x < gc->world.mapWidth && y < gc->world.mapHeight) {
                gc_computeIsoCornersInlined(gc, x, y, quad_x, quad_y, true);
                gc_drawSelectionQuad(gc, graphics, quad_x, quad_y);
                if (gc->world.tileMap[x][y] != 0 || gc->world.trackMap[x][y] != 0) break;
            }
        } else {
            midp_set_color(graphics, 0xFF0000);
        }
    }
    gc_drawSelectionQuad(gc, graphics, gc->build.selectionTileX.data(),
                         gc->build.selectionTileY.data());
}

// void drawBuildMenuRow(Graphics)
void gc_drawBuildMenuRow(GameCanvas *gc, Surface *graphics) {
    if (!gc->menuRowEnabled[gc->buildMenuMode][gc->buildMenuItemIndex]) return;
    int menu_x = gc->viewportBaseX - gc->viewportCenterX;
    int menu_y = gc->viewportBaseY - gc->viewportCenterY;
    if (gc->blockBuildMenu) {
        gc_drawSpriteQuad(gc, graphics, 200, menu_x, menu_y + 15, 20);
        midp_set_color(graphics, 0xFF0000);
        midp_draw_arc(graphics, menu_x + 5, menu_y + 20, 7, 7, 0, 360);
        midp_draw_line(graphics, menu_x + 7, menu_y + 25,
                       menu_x + 10, menu_y + 22);
        return;
    }
    if (gc->buildMenuMode == 1
        && (gc->buildMenuItemIndex == 5 || gc->buildMenuItemIndex == 6))
        gc_drawSpriteQuad(gc, graphics, 249, gc->cursorScreenX - 2,
                          gc->cursorScreenY + 13, 20);
    gc_drawSpriteQuad(gc, graphics, 200, menu_x, menu_y + 15, 20);
    if (gc->buildMenuMode != 0 || gc->buildMenuItemIndex != 0) {
        int sprite = gc_byteToUnsignedInt(gc, gc->menuTextIndex[gc->buildMenuMode]
                                               [gc->buildMenuItemIndex]);
        gc_drawSpriteQuad(gc, graphics, sprite, menu_x + 5, menu_y + 19, 20);
    }
    if (gc->buildMenuMode != 3 || gc->buildMenuItemIndex >= 4) return;
    gc_drawSpriteQuad(gc, graphics, gc->buildMenuItemIndex + 230,
                      menu_x + 5, menu_y + 19, 20);

    int min_x = gc->tileCursorX - 3;
    int min_y = gc->tileCursorY - 3;
    int max_x = gc->tileCursorX + 3;
    int max_y = gc->tileCursorY + 3;
    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    if (max_x >= gc->world.mapWidth) max_x = gc->world.mapWidth - 1;
    if (max_y >= gc->world.mapHeight) max_y = gc->world.mapHeight - 1;
    int population_tiles = 0;
    bool has_type_50_55 = false;
    bool has_type_56_61 = false;
    bool has_type_62_68 = false;
    for (int x = min_x; x <= max_x; ++x) {
        for (int y = min_y; y <= max_y; ++y) {
            int tile = gc->world.tileMap[x][y];
            if (tile >= 50 && tile <= 55) has_type_50_55 = true;
            if (tile >= 56 && tile <= 61) has_type_56_61 = true;
            if (tile >= 62 && tile <= 68) has_type_62_68 = true;
            if (tile >= 34 && tile <= 40) ++population_tiles;
        }
    }
    int icon_x = 0;
    if (population_tiles / 3 > 0) {
        gc_drawSpriteQuad(gc, graphics, 190, icon_x, 15, 20);
        icon_x += 15;
    }
    if (population_tiles / 7 > 0) {
        gc_drawSpriteQuad(gc, graphics, 191, icon_x, 15, 20);
        icon_x += 15;
    }
    if (has_type_62_68) {
        gc_drawSpriteQuad(gc, graphics, 192, icon_x, 15, 20);
        icon_x += 15;
    }
    if (has_type_56_61) {
        gc_drawSpriteQuad(gc, graphics, 194, icon_x, 15, 20);
        icon_x += 15;
    }
    if (has_type_50_55)
        gc_drawSpriteQuad(gc, graphics, 193, icon_x, 15, 20);
}
