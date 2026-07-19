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
    std::unique_ptr<Image> decode_png_from_sprite_stream();
    // Decode one loose sprite PNG (resources/sprites/<index>.png). Stage 1a of
    // the sprite_images.bin migration: sprites now load from loose PNG files
    // instead of the legacy encoded stream.
    std::unique_ptr<Image> load_sprite_png(int32_t index);
    void build_tile_draw_order(int32_t width, int32_t height);

    // Glyph codes for a sprite_image_map semantic index, from the text catalog
    // (falls back to the loaded sprite_image_map row if the index is unbound).
    std::vector<int8_t> index_text_codes(int32_t semantic_index) const;

    // Y offset for the raised-track pieces (codes 77..80), from the slots they
    // share with the slope codes in tileSpriteYOffsetByCode.
    int8_t raised_track_y_offset(int32_t tile_code) const {
        return tileSpriteYOffsetByCode[track_turn_tile::kFirstYOffsetSlot
                                       + tile_code - track_turn_tile::VariantA];
    }

    std::vector<int8_t> spriteIndexTable;
    Grid<int8_t> spriteBitmapTable;
    int8_t tileSpriteIndex[kTileSpriteCodeCount]{};
    int8_t tileSpriteYOffsetByCode[kTileYOffsetCount]{};
    int16_t trainSpriteDirectionTable[kTrainSpriteTypeCount][kTrainSpriteDirectionCount]{};
    std::vector<int8_t> tileOffsetX;
    std::vector<int8_t> tileOffsetY;
    int32_t numActiveTileAnimations = 0;

    // Loaded images. font_images holds the six font pages; tile_sprites the
    // decoded tile images; cycle_flag the animated cycle sprite. Sprites load
    // from loose PNG files (resources/sprites/<index>.png); sprite_load_index
    // is the consume counter that preserves the original stream read order.
    // All runtime-only.
    int32_t sprite_load_index = 0;
    std::unique_ptr<Image> font_images[kFontPageCount];
    std::vector<std::unique_ptr<Image>> tile_sprites;
    std::unique_ptr<Image> cycle_flag;
    int32_t spriteLoadCounter = 0;
    int8_t spriteCycleCursor = 0;
};

#endif  // ASSETS_H
