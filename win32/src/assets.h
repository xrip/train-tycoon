// Game assets: loaded images, the font, and packed sprite data tables
// (see game/render*.cpp and game/boot.cpp). Assets owns the data; drawing that
// reads it stays in the renderer.
#ifndef ASSETS_H
#define ASSETS_H
#include "midp.h"  // Image
#include "support/grid.h"
#include "support/resource_reader.h"
#include <array>
#include <cstdint>
#include <memory>
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

    // Loaded images and the sprite resource stream. font_images holds the six
    // font pages; tile_sprites the decoded tile images; cycle_flag the animated
    // cycle sprite. sprite_stream is the open packed-sprite reader; the load
    // counter and cycle cursor track decoding/animation progress. All
    // runtime-only. Resource read order is unchanged.
    ResourceReader sprite_stream;
    std::array<std::unique_ptr<Image>, 6> font_images;
    std::vector<std::unique_ptr<Image>> tile_sprites;
    std::unique_ptr<Image> cycle_flag;
    int32_t spriteLoadCounter = 0;
    int8_t spriteCycleCursor = 0;
};

#endif  // ASSETS_H
