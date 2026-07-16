// Game-map header, bottom bar, and overlays.
#include "game_internal.h"

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

int32_t game_drawMenuRow(Game *game, Surface *graphics, int32_t sprite,
                       int32_t item_index, int32_t x, bool spaced) {
    game_drawSpriteQuad(game, graphics, sprite, x, bottomBarY, 20);
    if (game->ui.buildMenuItemIndex == item_index) game->ui.selectedMenuRowX = x;
    return x + 15 + (spaced ? 3 : 0);
}

void game_drawHeaderBar(Game *game, Surface *graphics) {
    if (game->cachedHeaderDay == game->economy.dayOfMonth
        && game->cachedHeaderMonth == game->economy.monthOfYear
        && game->cachedHeaderYear == game->economy.year
        && game->cachedHeaderMoney == game->economy.money && !game->ui.menuDirty)
        return;

    midp_set_color(graphics, 0x909090);
    midp_fill_rect(graphics, 0, 0, canvasWidth, 14);
    midp_set_color(graphics, 0xBBBBBB);
    midp_draw_rect(graphics, 0, 0, canvasWidth, 14);
    midp_set_color(graphics, 0x222222);
    midp_draw_line(graphics, 1, 14, canvasWidth, 14);
    midp_draw_line(graphics, canvasWidth - 1, 1, canvasWidth - 1, 14);

    char money_text[32];
    format_money(game->economy.money, money_text, sizeof(money_text));
    const auto money_bytes = make_font_bytes(money_text, true);
    game_renderFontString(game, graphics, 1, 5, money_bytes, false);

    char date_text[24];
    if (canvasWidth < 140)
        snprintf(date_text, sizeof(date_text), "%02d.%02d.%d",
                 game->economy.dayOfMonth, game->economy.monthOfYear, game->economy.year - 1900);
    else
        snprintf(date_text, sizeof(date_text), "%02d.%02d.%d",
                 game->economy.dayOfMonth, game->economy.monthOfYear, game->economy.year);
    const auto date_bytes = make_font_bytes(date_text, false);
    game_renderFontString(game, graphics, canvasWidth - (canvasWidth < 140 ? 49 : 61),
                        5, date_bytes, false);

    game->cachedHeaderMoney = game->economy.money;
    game->cachedHeaderDay = game->economy.dayOfMonth;
    game->cachedHeaderMonth = game->economy.monthOfYear;
    game->cachedHeaderYear = game->economy.year;
}

void game_drawConfirmBar(Game *game, Surface *graphics) {
    game_drawHeaderBar(game, graphics);
    midp_set_color(graphics, 0x606060);
    midp_fill_rect(graphics, 0, bottomBarY, canvasWidth, 15);
    game_drawMenuRow(game, graphics, 201, 10, 0, false);
    game_drawMenuRow(game, graphics, 235, 10, 18, false);
    midp_set_color(graphics, 0xFFFFFF);
    midp_draw_rect(graphics, 18 * game->ui.confirmToggle, bottomBarY, 14, 14);
    midp_draw_rect(graphics, 18 * game->ui.confirmToggle + 1, bottomBarY + 1, 12, 12);
}

void game_paintMain(Game *game, Surface *graphics) {
    (void)graphics;
    game_drawHeaderBar(game, game->renderer.backbuffer_surface);
    if (!game->ui.menuDirty) return;
    midp_set_color(game->renderer.backbuffer_surface, 0x606060);
    midp_fill_rect(game->renderer.backbuffer_surface, 0, bottomBarY, canvasWidth, 15);
    int selected_x = unsignedByte(game->ui.menuItemCode[game->ui.buildMenuMode]
                                               [game->ui.buildMenuItemIndex]);
    int scroll = selected_x + 15 - canvasWidth;
    if (scroll < 0) scroll = 0;
    int item_count = static_cast<int>(game->ui.buildMenuItems[game->ui.buildMenuMode].size());
    for (int item = 0; item < item_count; ++item) {
        int x = unsignedByte(game->ui.menuItemCode[game->ui.buildMenuMode][item]) - scroll;
        int sprite = unsignedByte(game->ui.menuTextIndex[game->ui.buildMenuMode][item]);
        game_drawSpriteQuad(game, game->renderer.backbuffer_surface, sprite, x, bottomBarY, 20);
        if (item != game->ui.buildMenuItemIndex) continue;
        midp_set_color(game->renderer.backbuffer_surface, 0xFFFFFF);
        midp_draw_rect(game->renderer.backbuffer_surface, x, bottomBarY, 14, 14);
        midp_draw_rect(game->renderer.backbuffer_surface, x + 1, bottomBarY + 1, 12, 12);
    }
    if (game->ui.buildMenuMode == 2) {
        char cost_text[24];
        int64_t magnitude = game->build.selectionCost < 0
                            ? -(int64_t)game->build.selectionCost : game->build.selectionCost;
        snprintf(cost_text, sizeof(cost_text), "%lld", (long long)magnitude);
        int text_len = (int)strlen(cost_text);
        const auto currency_codes = std::span<const int8_t>(
            game->assets.spriteBitmapTable[game->assets.spriteIndexTable[10]]);
        std::vector<int8_t> bytes(
            static_cast<size_t>(text_len + 1) + currency_codes.size());
        bytes[0] = 24;
        for (int i = 0; i < text_len; ++i) bytes[i + 1] = (int8_t)(cost_text[i] - 47);
        std::copy(currency_codes.begin(), currency_codes.end(),
                  bytes.begin() + text_len + 1);
        game_renderFontString(game, game->renderer.backbuffer_surface, 35, bottomBarY + 5, bytes, false);
    }
    game->ui.menuDirty = false;
}

int32_t game_drawNumber(Game *game, Surface *graphics,
                      int32_t value, int32_t y) {
    int64_t magnitude = value < 0 ? -(int64_t)value : value;
    char text[24];
    snprintf(text, sizeof(text), "%lld", (long long)magnitude);
    const auto bytes = make_font_bytes(text, true);
    game_drawTextBox(game, graphics, canvasWidth, 15 + y, value < 0 ? 1 : 2, 4, bytes);
    return y + 15;
}

void game_drawMoneyTicker(Game *game, Surface *graphics) {
    midp_set_color(graphics, 0xFF0000);
    int y = 0;
    if (game->ui.moneyTickerFrames[2] > 0) {
        --game->ui.moneyTickerFrames[2];
        y = game_drawNumber(game, graphics, game->economy.moneyHistory[2], y);
    }
    if (game->ui.moneyTickerFrames[1] > 0) {
        --game->ui.moneyTickerFrames[1];
        y = game_drawNumber(game, graphics, game->economy.moneyHistory[1], y);
    }
    if (game->ui.moneyTickerFrames[0] > 0) {
        --game->ui.moneyTickerFrames[0];
        game_drawNumber(game, graphics, game->economy.moneyHistory[0], y);
    }
}

void game_drawConfirmDialog(Game *game, Surface *graphics) {
    if (game->ui.confirmCounter <= 0) return;
    if (game->ui.confirmTargetState >= 0) {
        game_drawTextBox(game, graphics, 10, 10, 0, 37,
                       std::span<const int8_t>(game->assets.spriteBitmapTable[
                           game->assets.spriteIndexTable[120 + game->ui.confirmTargetState]]));
    }
    --game->ui.confirmCounter;
}

void game_computeIsoCorners(Game *game, int32_t x, int32_t y,
                                 int32_t *screen_x, int32_t *screen_y,
                                 bool viewport_relative) {
    int center_x = x * 16 + y * 16 + 16;
    int center_y = game->world.mapHalfWidth - x * 8 + y * 8 + 8;
    screen_x[0] = center_x - 16;
    screen_x[1] = center_x;
    screen_x[2] = center_x;
    screen_x[3] = center_x + 16;
    screen_y[0] = center_y - 4 * game->world.heightMap[x][y];
    screen_y[1] = center_y - 8 - 4 * game->world.heightMap[x + 1][y];
    screen_y[2] = center_y + 8 - 4 * game->world.heightMap[x][y + 1];
    screen_y[3] = center_y - 4 * game->world.heightMap[x + 1][y + 1];
    if (viewport_relative) {
        for (int corner = 0; corner < 4; ++corner) {
            screen_x[corner] -= game->renderer.viewportCenterX;
            screen_y[corner] -= game->renderer.viewportCenterY;
        }
    }
}

void game_drawSelectionQuad(Game *game, Surface *graphics,
                          int32_t *screen_x, int32_t *screen_y) {
    (void)game;
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

void game_drawBuildMenuCursor(Game *game, Surface *graphics) {
    if (game->ui.blockBuildMenu) return;
    int action = game->ui.menuTransitionCode[game->ui.buildMenuMode][game->ui.buildMenuItemIndex];
    if (action == 0) return;
    if (action >= 1 && action <= 7) {
        bool buildable = game_isTileBuildable(game, game->ui.tileCursorX, game->ui.tileCursorY,
                                            action - 1);
        midp_set_color(graphics, buildable ? 0x00FF00 : 0xFF0000);
    }
    if (action == 6 || action == 7) {
        int32_t quad_x[4];
        int32_t quad_y[4];
        if (action == 6) {
            if (game->ui.tileCursorY > 0) {
                game_computeIsoCorners(game, game->ui.tileCursorX, game->ui.tileCursorY - 1,
                                            quad_x, quad_y, true);
                game_drawSelectionQuad(game, graphics, quad_x, quad_y);
            }
            if (game->ui.tileCursorY < game->world.mapWidth - 1) {
                game_computeIsoCorners(game, game->ui.tileCursorX, game->ui.tileCursorY + 1,
                                            quad_x, quad_y, true);
                game_drawSelectionQuad(game, graphics, quad_x, quad_y);
            }
        } else {
            if (game->ui.tileCursorX > 0) {
                game_computeIsoCorners(game, game->ui.tileCursorX - 1, game->ui.tileCursorY,
                                            quad_x, quad_y, true);
                game_drawSelectionQuad(game, graphics, quad_x, quad_y);
            }
            if (game->ui.tileCursorX < game->world.mapWidth - 1) {
                game_computeIsoCorners(game, game->ui.tileCursorX + 1, game->ui.tileCursorY,
                                            quad_x, quad_y, true);
                game_drawSelectionQuad(game, graphics, quad_x, quad_y);
            }
        }
    }
    if (action == 8)
        midp_set_color(graphics,
                       game_isPlaceableTerrain(game, game->ui.tileCursorX, game->ui.tileCursorY)
                       ? 0x00FF00 : 0xFF0000);
    if (action == 9)
        midp_set_color(graphics,
                       game_isCityOrStationTile(game, game->ui.tileCursorX, game->ui.tileCursorY)
                       ? 0x00FF00 : 0xFF0000);
    if (action == 10) midp_set_color(graphics, 0xFFFFFF);
    if (action == 11) {
        if (game_isCurveTile(game, game->ui.tileCursorX, game->ui.tileCursorY)) {
            int x = game->ui.tileCursorX;
            int y = game->ui.tileCursorY;
            int dx = 0;
            int dy = 0;
            if (game->world.tileMap[x][y] == 7) dy = -1;
            if (game->world.tileMap[x][y] == 10) dy = 1;
            if (game->world.tileMap[x][y] == 8) dx = -1;
            if (game->world.tileMap[x][y] == 9) dx = 1;
            midp_set_color(graphics, 0x00FF00);
            int32_t quad_x[4];
            int32_t quad_y[4];
            while ((x += dx) >= 0 && (y += dy) >= 0
                   && x < game->world.mapWidth && y < game->world.mapHeight) {
                game_computeIsoCorners(game, x, y, quad_x, quad_y, true);
                game_drawSelectionQuad(game, graphics, quad_x, quad_y);
                if (game->world.tileMap[x][y] != 0 || game->world.trackMap[x][y] != 0) break;
            }
        } else {
            midp_set_color(graphics, 0xFF0000);
        }
    }
    game_drawSelectionQuad(game, graphics, game->build.selectionTileX,
                         game->build.selectionTileY);
}

void game_drawBuildMenuRow(Game *game, Surface *graphics) {
    if (!game->ui.menuRowEnabled[game->ui.buildMenuMode][game->ui.buildMenuItemIndex]) return;
    int menu_x = game->renderer.viewportBaseX - game->renderer.viewportCenterX;
    int menu_y = game->renderer.viewportBaseY - game->renderer.viewportCenterY;
    // The keyboard cursor is anchored at the screen centre (viewportBase); the
    // run loop re-anchors it there every tick. When the mouse drives the cursor,
    // game_pointerMoved has already resolved the hovered tile and written its
    // screen position to cursorScreenX/Y, so re-anchor the whole cursor widget
    // (sprite 200 + tool icon) there too. Otherwise the arrow stays pinned at
    // the centre while the selection diamond (drawn from selectionTileX/Y)
    // tracks the mouse. Keyboard frames have mouseOverField == false, so they
    // keep the centre anchor and stay byte-identical.
    if (game->ui.mouseOverField) {
        menu_x = game->ui.cursorScreenX;
        menu_y = game->ui.cursorScreenY;
    }
    if (game->ui.blockBuildMenu) {
        game_drawSpriteQuad(game, graphics, 200, menu_x, menu_y + 15, 20);
        midp_set_color(graphics, 0xFF0000);
        midp_draw_arc(graphics, menu_x + 5, menu_y + 20, 7, 7, 0, 360);
        midp_draw_line(graphics, menu_x + 7, menu_y + 25,
                       menu_x + 10, menu_y + 22);
        return;
    }
    if (game->ui.buildMenuMode == 1
        && (game->ui.buildMenuItemIndex == 5 || game->ui.buildMenuItemIndex == 6))
        game_drawSpriteQuad(game, graphics, 249, game->ui.cursorScreenX - 2,
                          game->ui.cursorScreenY + 13, 20);
    game_drawSpriteQuad(game, graphics, 200, menu_x, menu_y + 15, 20);
    if (game->ui.buildMenuMode != 0 || game->ui.buildMenuItemIndex != 0) {
        int sprite = unsignedByte(game->ui.menuTextIndex[game->ui.buildMenuMode]
                                               [game->ui.buildMenuItemIndex]);
        game_drawSpriteQuad(game, graphics, sprite, menu_x + 5, menu_y + 19, 20);
    }
    if (game->ui.buildMenuMode != 3 || game->ui.buildMenuItemIndex >= 4) return;
    game_drawSpriteQuad(game, graphics, game->ui.buildMenuItemIndex + 230,
                      menu_x + 5, menu_y + 19, 20);

    int min_x = game->ui.tileCursorX - 3;
    int min_y = game->ui.tileCursorY - 3;
    int max_x = game->ui.tileCursorX + 3;
    int max_y = game->ui.tileCursorY + 3;
    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    if (max_x >= game->world.mapWidth) max_x = game->world.mapWidth - 1;
    if (max_y >= game->world.mapHeight) max_y = game->world.mapHeight - 1;
    int population_tiles = 0;
    bool has_type_50_55 = false;
    bool has_type_56_61 = false;
    bool has_type_62_68 = false;
    for (int x = min_x; x <= max_x; ++x) {
        for (int y = min_y; y <= max_y; ++y) {
            int tile = game->world.tileMap[x][y];
            if (tile >= 50 && tile <= 55) has_type_50_55 = true;
            if (tile >= 56 && tile <= 61) has_type_56_61 = true;
            if (tile >= 62 && tile <= 68) has_type_62_68 = true;
            if (tile >= 34 && tile <= 40) ++population_tiles;
        }
    }
    int icon_x = 0;
    if (population_tiles / 3 > 0) {
        game_drawSpriteQuad(game, graphics, 190, icon_x, 15, 20);
        icon_x += 15;
    }
    if (population_tiles / 7 > 0) {
        game_drawSpriteQuad(game, graphics, 191, icon_x, 15, 20);
        icon_x += 15;
    }
    if (has_type_62_68) {
        game_drawSpriteQuad(game, graphics, 192, icon_x, 15, 20);
        icon_x += 15;
    }
    if (has_type_56_61) {
        game_drawSpriteQuad(game, graphics, 194, icon_x, 15, 20);
        icon_x += 15;
    }
    if (has_type_50_55)
        game_drawSpriteQuad(game, graphics, 193, icon_x, 15, 20);
}
