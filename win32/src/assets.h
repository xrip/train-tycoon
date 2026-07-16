// Game assets: loaded images, the font, and packed sprite data tables
// (see game/render*.cpp and game/boot.cpp). Assets owns the data; drawing that
// reads it stays in the renderer.
#ifndef ASSETS_H
#define ASSETS_H
#include "support/grid.h"
#include <array>
#include <cstdint>
#include <vector>

// Assets owns the packed sprite data loaded from resources. Packed-data leaf:
// the sprite offset/data tables, the sprite index and bitmap tables, the tile
// sprite index and per-code Y offsets, the train sprite direction table, and
// the tile-animation offset lists. All runtime-only (not saved). Values and the
// resource read order are unchanged.
struct Assets {
    std::vector<int16_t> spriteOffsetTable;
    std::vector<int8_t> spriteData;
    std::vector<int8_t> spriteIndexTable;
    Grid<int8_t> spriteBitmapTable;
    std::array<int8_t, 255> tileSpriteIndex{};
    std::array<int8_t, 26> tileSpriteYOffsetByCode{};
    std::array<std::array<int16_t, 8>, 8> trainSpriteDirectionTable{};
    std::vector<int8_t> tileOffsetX;
    std::vector<int8_t> tileOffsetY;
    int32_t numActiveTileAnimations = 0;
};

#endif  // ASSETS_H
