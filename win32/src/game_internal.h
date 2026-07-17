// Calls shared by game-core translation units and behavior tests.
#ifndef GAME_INTERNAL_H
#define GAME_INTERNAL_H
#include "game.h"
#include "support/byte_buffer.h"

#include <optional>
#include <span>
#include <vector>

constexpr int32_t unsignedByte(int8_t value) {
    return static_cast<uint8_t>(value);
}
int32_t cross_product_2d(int32_t n, int32_t n2, int32_t n3,
                         int32_t n4, int32_t n5, int32_t n6);
void game_computeIsoCorners(Game *game, int32_t n, int32_t n2, int32_t* nArray, int32_t* nArray2, bool bl);
// Resolve the tile under a map-screen-space point (anchorX/Y) and refresh the
// tile cursor, its screen position, and the selection diamond. anchorX/Y are in
// the same space as viewportBaseX/Y (absolute map-pixel coords, not view-relative).
// The keyboard zoom path passes viewportBase (screen centre); the pointer path
// passes the mouse position translated into map space.
void game_drawSelectionQuad(Game *game, Surface* graphics, int32_t* nArray, int32_t* nArray2);
void game_drawBuildMenuCursor(Game *game, Surface* graphics);
void game_drawBuildMenuRow(Game *game, Surface* graphics);
int32_t game_drawMenuRow(Game *game, Surface* graphics, int32_t n, int32_t n2, int32_t n3, bool bl);
void game_drawConfirmBar(Game *game, Surface* graphics);
void game_drawHeaderBar(Game *game, Surface* graphics);
void game_paintMain(Game *game, Surface* graphics);
int32_t game_drawNumber(Game *game, Surface* graphics, int32_t n, int32_t n2);
void game_drawMoneyTicker(Game *game, Surface* graphics);
void game_drawConfirmDialog(Game *game, Surface* graphics);
std::vector<int8_t> format_number_with_suffix(int32_t value, int32_t suffix);
std::vector<int8_t> encode_display_text(const char* text);
Image* game_wagonCarSprite(Game *game, int32_t n, int32_t n2);
void game_handleBuildMenuState(Game *game, int32_t n, int32_t n2);

#endif  // GAME_INTERNAL_H
