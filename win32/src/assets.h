// Game assets: loaded images, the font, and packed sprite data tables
// (see game/render*.cpp and game/boot.cpp). Assets owns the data; drawing that
// reads it stays in the renderer.
#ifndef ASSETS_H
#define ASSETS_H
#include "game_constants.h"
#include "midp.h"  // Image
#include "support/grid.h"
#include "support/resource_reader.h"
#include <cstdint>
#include <memory>
#include <vector>

// Assets owns the packed sprite data loaded from resources. Packed-data leaf:
// the sprite offset/data tables, the sprite index and bitmap tables, the tile
// sprite index and per-code Y offsets, the train sprite direction table, and
// the tile-animation offset lists. All runtime-only (not saved). Values and the
// resource read order are unchanged.
struct Assets {
    void open_sprite_stream(const char* resource_name);
    void load_packed_sprite_data(const char* resource_name);
    std::unique_ptr<Image> decode_png_from_sprite_stream();
    std::vector<int8_t> read_packed_text_codes(int32_t offset) const;

    std::vector<int16_t> spriteOffsetTable;
    std::vector<int8_t> spriteData;
    std::vector<int8_t> spriteIndexTable;
    Grid<int8_t> spriteBitmapTable;
    int8_t tileSpriteIndex[kTileSpriteCodeCount]{};
    int8_t tileSpriteYOffsetByCode[kTileYOffsetCount]{};
    int16_t trainSpriteDirectionTable[kTrainSpriteTypeCount][kTrainSpriteDirectionCount]{};
    std::vector<int8_t> tileOffsetX;
    std::vector<int8_t> tileOffsetY;
    int32_t numActiveTileAnimations = 0;

    // Loaded images and the sprite resource stream. font_images holds the six
    // font pages; tile_sprites the decoded tile images; cycle_flag the animated
    // cycle sprite. sprite_stream is the open packed-sprite reader; the load
    // counter and cycle cursor track decoding/animation progress. All
    // runtime-only. Resource read order is unchanged.
    ResourceReader sprite_stream;
    std::unique_ptr<Image> font_images[kFontPageCount];
    std::vector<std::unique_ptr<Image>> tile_sprites;
    std::unique_ptr<Image> cycle_flag;
    int32_t spriteLoadCounter = 0;
    int8_t spriteCycleCursor = 0;
};

#endif  // ASSETS_H
