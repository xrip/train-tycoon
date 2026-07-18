// Game start, resources, sprites, state changes, and save start.
#include "game.h"
#include "support/byte_buffer.h"
#include "text/text_catalog.h"
#include <stdlib.h>
#include <string.h>
#include <array>
#include <utility>
#include <vector>

namespace {

namespace resource_format {
constexpr int32_t kIndexedImage = 1;
constexpr int32_t kPngStream = 2;
}  // namespace resource_format

namespace png_format {
constexpr int32_t kPaletteBytesPerColor = 3;
constexpr int32_t kIndexedImageFixedByteCount = 69;
constexpr int32_t kTransparencyChunkByteCount = 13;
constexpr int32_t kSingleTransparencyEntry = 1;
constexpr int32_t kIndexedColorType = 3;
constexpr int32_t kByteMask = 0xFF;

constexpr std::array<int8_t, 4> kSignaturePrefix = {
    static_cast<int8_t>(0x89), 'P', 'N', 'G',
};
constexpr std::array<int8_t, 8> kSignature = {
    static_cast<int8_t>(0x89), 'P', 'N', 'G', '\r', '\n', '\x1A', '\n',
};
constexpr std::array<int8_t, 4> kIhdrChunkType = {'I', 'H', 'D', 'R'};
constexpr std::array<int8_t, 4> kPlteChunkType = {'P', 'L', 'T', 'E'};
constexpr std::array<int8_t, 4> kTrnsChunkType = {'t', 'R', 'N', 'S'};
constexpr std::array<int8_t, 4> kIdatChunkType = {'I', 'D', 'A', 'T'};
constexpr std::array<int8_t, 4> kIendChunkType = {'I', 'E', 'N', 'D'};
constexpr std::array<int8_t, 4> kIendCrc = {-82, 66, 96, -126};

namespace indexed_offset {
constexpr std::size_t kIhdrLengthLastByte = 11;
constexpr std::size_t kIhdrType = 12;
constexpr std::size_t kIhdrWidthLowByte = 18;
constexpr std::size_t kIhdrHeightLowByte = 22;
constexpr std::size_t kIhdrBitDepth = 24;
constexpr std::size_t kIhdrColorType = 25;
constexpr std::size_t kIhdrCrc = 29;
constexpr std::size_t kPlteLengthHighByte = 35;
constexpr std::size_t kPlteLengthLowByte = 36;
constexpr std::size_t kPlteType = 37;
constexpr std::size_t kPlteData = 41;
}  // namespace indexed_offset

template <std::size_t N>
void write_bytes(std::vector<int8_t>& output,
                 std::size_t offset,
                 const std::array<int8_t, N>& bytes) {
    for (std::size_t byte_index = 0; byte_index < bytes.size(); ++byte_index)
        output[offset + byte_index] = bytes[byte_index];
}
}  // namespace png_format

namespace state_work {
constexpr int32_t kMapGenerationStart = 0;
constexpr int32_t kDeferredOperationStart = 4;
}  // namespace state_work

namespace tile_sprite_alias {
// These ids are a compatibility alias in tile_sprite_ids.bin. Their visual
// meaning is not proven, so the names describe the operation only.
constexpr int32_t kAliasCode = 68;
constexpr int32_t kAliasSourceCode = 66;
constexpr int32_t kScenarioResourceHeaderOffset = 8;
}  // namespace tile_sprite_alias

namespace tile_draw_order {
constexpr int32_t kHeightAlignment = 8;
constexpr int32_t kTopStripHeight = 30;
constexpr int32_t kRowsAfterViewport = 6;
constexpr int32_t kWidthDivisor = 32;
constexpr int32_t kColumnsAfterViewport = 2;
constexpr int32_t kFirstColumn = -1;
constexpr int32_t kRowHalf = 2;
constexpr int32_t kOddRowOffset = -1;
}  // namespace tile_draw_order

}  // namespace

static int read_u16_be(ResourceReader& resource) {
    const int high_byte = resource.read_byte();
    const int low_byte = resource.read_byte();
    return high_byte * 256 + low_byte;
}

void Game::key_pressed(int32_t key_code) {
    Game* game = this;
    if (game->ui.inputPending) { game->ui.inputPending = false; return; }
    game->ui.currentKey = key_code;
}

void Game::reset_overlay_render_state() {
    Game *game = this;
    game->ui.cursorScale = 0;
    game->ui.cursorDragY = -1;
    game->ui.cursorSelectY = -1;
    game->ui.cursorSelectX = -1;
    game->ui.cursorOverUI = false;
    game->ui.cursorAllowZoomOut = false;
    game->renderer.dialogContentHeight = 0;
    game->renderer.needsRepaint = false;
}


// public Image decodePngFromSpriteStream() -- rebuild a PNG from the stream
std::unique_ptr<Image> Assets::decode_png_from_sprite_stream() {
    std::vector<int8_t> png_bytes;
    int png_byte_count = 0;
    const int resource_format_code = sprite_stream.read_byte();
    if (resource_format_code == resource_format::kPngStream) {
        png_byte_count = read_u16_be(sprite_stream);
        png_bytes.resize(static_cast<std::size_t>(png_byte_count));
        png_format::write_bytes(png_bytes, 0, png_format::kSignaturePrefix);
        for (int png_byte_index =
                 static_cast<int>(png_format::kSignaturePrefix.size());
             png_byte_index < png_byte_count; ++png_byte_index)
            png_bytes[png_byte_index] =
                (int8_t)sprite_stream.read_byte();
    }
    if (resource_format_code == resource_format::kIndexedImage) {
        const int image_data_byte_count = read_u16_be(sprite_stream);
        const int palette_byte_count = sprite_stream.read_byte()
            * png_format::kPaletteBytesPerColor;
        const int transparency_entry_count = sprite_stream.read_byte();
        png_byte_count = image_data_byte_count + palette_byte_count
            + png_format::kIndexedImageFixedByteCount
            + transparency_entry_count * png_format::kTransparencyChunkByteCount;
        png_bytes.resize(static_cast<std::size_t>(png_byte_count));
        png_format::write_bytes(png_bytes, 0, png_format::kSignature);
        png_format::write_bytes(
            png_bytes, png_format::indexed_offset::kIhdrType,
            png_format::kIhdrChunkType);
        png_bytes[png_format::indexed_offset::kIhdrLengthLastByte] = 13;
        png_bytes[png_format::indexed_offset::kIhdrWidthLowByte] =
            (int8_t)sprite_stream.read_byte();
        png_bytes[png_format::indexed_offset::kIhdrWidthLowByte + 1] =
            (int8_t)sprite_stream.read_byte();
        png_bytes[png_format::indexed_offset::kIhdrHeightLowByte] =
            (int8_t)sprite_stream.read_byte();
        png_bytes[png_format::indexed_offset::kIhdrHeightLowByte + 1] =
            (int8_t)sprite_stream.read_byte();
        png_bytes[png_format::indexed_offset::kIhdrBitDepth] =
            (int8_t)sprite_stream.read_byte();
        png_bytes[png_format::indexed_offset::kIhdrColorType] =
            png_format::kIndexedColorType;
        for (std::size_t crc_byte_offset = 0; crc_byte_offset < 4;
             ++crc_byte_offset)
            png_bytes[png_format::indexed_offset::kIhdrCrc + crc_byte_offset] =
                (int8_t)sprite_stream.read_byte();
        png_bytes[png_format::indexed_offset::kPlteLengthHighByte] =
            (int8_t)(palette_byte_count >> 8 & png_format::kByteMask);
        png_bytes[png_format::indexed_offset::kPlteLengthLowByte] =
            (int8_t)(palette_byte_count & png_format::kByteMask);
        png_format::write_bytes(
            png_bytes, png_format::indexed_offset::kPlteType,
            png_format::kPlteChunkType);
        int write_index;
        for (write_index = 0;
             write_index < palette_byte_count + 4; ++write_index) {
            png_bytes[png_format::indexed_offset::kPlteData + write_index] =
                (int8_t)sprite_stream.read_byte();
        }
        write_index = static_cast<int>(
            png_format::indexed_offset::kPlteData + palette_byte_count + 4);
        if (transparency_entry_count == png_format::kSingleTransparencyEntry) {
            write_index += 3;
            png_bytes[write_index++] = 1;
            png_format::write_bytes(
                png_bytes, write_index, png_format::kTrnsChunkType);
            write_index += static_cast<int>(png_format::kTrnsChunkType.size());
            png_bytes[++write_index] = 64;
            png_bytes[++write_index] = -26;
            png_bytes[++write_index] = -40;
            const int transparency_crc_offset = ++write_index;
            ++write_index;
            png_bytes[transparency_crc_offset] = 102;
        }
        write_index += 2;
        png_bytes[write_index++] =
            (int8_t)(image_data_byte_count >> 8 & png_format::kByteMask);
        png_bytes[write_index++] =
            (int8_t)(image_data_byte_count & png_format::kByteMask);
        png_format::write_bytes(png_bytes, write_index,
                                png_format::kIdatChunkType);
        write_index += static_cast<int>(png_format::kIdatChunkType.size());
        for (int image_data_index = 0;
             image_data_index < image_data_byte_count + 4;
             ++image_data_index)
            png_bytes[write_index++] =
                (int8_t)sprite_stream.read_byte();
        write_index += 4;
        png_format::write_bytes(png_bytes, write_index,
                                png_format::kIendChunkType);
        write_index += static_cast<int>(png_format::kIendChunkType.size());
        png_format::write_bytes(png_bytes, write_index,
                                png_format::kIendCrc);
    }
    if (png_bytes.empty()) return nullptr;
    return Image::decode_png(std::span<const uint8_t>(
        reinterpret_cast<const uint8_t*>(png_bytes.data()), png_bytes.size()));
}

std::vector<int8_t> Assets::index_text_codes(int32_t semantic_index) const {
    const Text key = key_for_index(semantic_index);
    if (key != Text::none) {
        const std::vector<int8_t>& codes = catalog_codes(key);
        return {codes.begin(), codes.end()};
    }
    // Unbound index: fall back to the original sprite_image_map row.
    if (semantic_index >= 0
        && semantic_index < static_cast<int32_t>(spriteIndexTable.size())) {
        const std::vector<int8_t>& row =
            spriteBitmapTable[spriteIndexTable[semantic_index]];
        return {row.begin(), row.end()};
    }
    return {};
}

void Game::load_settings_snapshot() {
    Game *game = this;
    game->saveCodec.load_slot(save::slot::Auxiliary, save::MODE_READ);
    game->saveCodec.walk_settings(*game);
    game->set_music_volume(game->music.volumeLevel);
}

// Add one isometric tile-draw offset pair.
static void append_tile_draw_offset(ByteBuffer& x_offsets,
                                    ByteBuffer& y_offsets,
                                    int32_t x, int32_t y) {
    x_offsets.write(x);
    y_offsets.write(y);
}

// Build and sort the isometric tile-draw order tables.
void Assets::build_tile_draw_order(int32_t canvas_width, int32_t canvas_height) {
    ByteBuffer x_offsets;
    ByteBuffer y_offsets;
    const int32_t aligned_height = canvas_height
        + canvas_height % tile_draw_order::kHeightAlignment;
    const int32_t draw_row_count =
        (aligned_height - tile_draw_order::kTopStripHeight)
            / tile_draw_order::kHeightAlignment
        + tile_draw_order::kRowsAfterViewport;
    const int32_t draw_column_count = canvas_width
        / tile_draw_order::kWidthDivisor
        + tile_draw_order::kColumnsAfterViewport;
    for (int32_t row = 0; row < draw_row_count; ++row) {
        for (int32_t column = tile_draw_order::kFirstColumn;
             column < draw_column_count; ++column) {
            const int32_t x_offset = column - row / tile_draw_order::kRowHalf;
            const int32_t y_offset = row % 2 == 0
                ? column + row / tile_draw_order::kRowHalf
                : tile_draw_order::kOddRowOffset
                    + column + row / tile_draw_order::kRowHalf;
            append_tile_draw_offset(x_offsets, y_offsets, x_offset, y_offset);
        }
    }
    const auto x_offset_bytes = x_offsets.bytes();
    const auto y_offset_bytes = y_offsets.bytes();
    const int active_offset_count = static_cast<int>(x_offset_bytes.size());
    tileOffsetX.resize(static_cast<std::size_t>(active_offset_count));
    memcpy(tileOffsetX.data(), x_offset_bytes.data(),
           static_cast<std::size_t>(active_offset_count));
    tileOffsetY.resize(static_cast<std::size_t>(active_offset_count));
    memcpy(tileOffsetY.data(), y_offset_bytes.data(),
           static_cast<std::size_t>(active_offset_count));
    numActiveTileAnimations = active_offset_count;
    bool reordered;
    do {
        reordered = false;
        for (int offset_index = 0;
             offset_index < numActiveTileAnimations - 1; ++offset_index) {
            if (tileOffsetY[offset_index] <= tileOffsetY[offset_index + 1])
                continue;
            std::swap(tileOffsetY[offset_index], tileOffsetY[offset_index + 1]);
            std::swap(tileOffsetX[offset_index], tileOffsetX[offset_index + 1]);
            reordered = true;
        }
    } while (reordered);
    do {
        reordered = false;
        for (int offset_index = 1;
             offset_index < numActiveTileAnimations; ++offset_index) {
            if (tileOffsetX[offset_index] <= tileOffsetX[offset_index - 1]
                || tileOffsetY[offset_index] != tileOffsetY[offset_index - 1])
                continue;
            std::swap(tileOffsetY[offset_index], tileOffsetY[offset_index - 1]);
            std::swap(tileOffsetX[offset_index], tileOffsetX[offset_index - 1]);
            reordered = true;
        }
    } while (reordered);
}

void Game::set_canvas_size(int32_t canvas_width, int32_t canvas_height) {
    Game *game = this;
    if (game->lastCanvasWidth != canvas_width
        || game->lastCanvasHeight != canvas_height) {
        game->renderer.resize_canvas(canvas_width, canvas_height);
        game->assets.build_tile_draw_order(canvas_width, canvas_height);
        game->lastCanvasWidth = canvas_width;
        game->lastCanvasHeight = canvas_height;
        game->ui.menuDirty = true;
        game->renderer.lastViewportCenterX = -1;
        if (game->gameState == GameState::MenuOverlay && !game->ready) {
            if (game->savedGameState == GameState::Playing) { game->ui.pendingTransitionCode = game->ui.transitionCode; game->gameState = GameState::Playing; }
            if (game->savedGameState == GameState::MapOverview) { game->ui.pendingTransitionCode = game->ui.transitionCode; game->gameState = GameState::MapOverview; }
        }
    }
}

void Game::set_state(int32_t state) {
    Game *game = this;
    game->savedGameState = game->gameState;
    if (!game->ready) game->assets.cycle_flag = NULL;
    game->ui.snapshotLoadedFlag = false;
    game->ui.menuDirty = true;
    switch (state) {
        case GameState::BootSplash0:
            game->assets.open_sprite_stream("sprite_images.bin");
            // Dialog layout now lives in code (game/dialog_table.cpp); the old
            // packed_sprites.bin load is gone.
            game->loadingAnimationFrame = 0;
            /* fall through */
        case GameState::BootSplash1:
        case GameState::BootSplash2:
            game->assets.cycle_flag = game->assets.decode_png_from_sprite_stream();
            game->loadingStartTime = midp_current_time_millis();
            game->loadingTimeout = false;
            game->loadingAnimationFrame = 0;
            break;
        case GameState::IntroReady: {
            ResourceReader in = resources::open("sprite_image_map.bin");
            if (in) {
                const int sprite_bitmap_count = in.read_byte();
                const int highest_sprite_index = in.read_byte();
                game->assets.spriteBitmapTable.resize(sprite_bitmap_count, 0);
                game->assets.spriteIndexTable.assign(
                    static_cast<std::size_t>(highest_sprite_index + 1), 0);
                for (int bitmap_index = 0;
                     bitmap_index < sprite_bitmap_count; ++bitmap_index) {
                    const int sprite_index = in.read_byte();
                    const int bitmap_byte_count = in.read_byte();
                    game->assets.spriteBitmapTable[bitmap_index].assign(
                        static_cast<std::size_t>(bitmap_byte_count), 0);
                    game->assets.spriteIndexTable[sprite_index] =
                        static_cast<int8_t>(bitmap_index);
                    for (int byte_index = 0;
                         byte_index < bitmap_byte_count; ++byte_index)
                        game->assets.spriteBitmapTable[bitmap_index][byte_index] =
                            static_cast<int8_t>(in.read_byte());
                }
            }
            break;
        }
        case GameState::LoadingResources: {
            game->assets.spriteLoadCounter = 0;
            ResourceReader in = resources::open("tile_sprite_ids.bin");
            if (in) {
                game->scenario.scenarioResourceCount = in.read_byte();
                for (int resource_index = 0;
                     resource_index < game->scenario.scenarioResourceCount;
                     ++resource_index) {
                    const int tile_sprite_code = in.read_byte();
                    game->assets.tileSpriteIndex[tile_sprite_code] =
                        static_cast<int8_t>(resource_index
                                            - tile_sprite_alias::kScenarioResourceHeaderOffset);
                }
            }
            game->scenario.scenarioResourceCount -=
                tile_sprite_alias::kScenarioResourceHeaderOffset;
            game->assets.tile_sprites.clear();
            game->assets.tile_sprites.resize(
                static_cast<std::size_t>(game->scenario.scenarioResourceCount));
            game->assets.tileSpriteIndex[tile_sprite_alias::kAliasCode] =
                game->assets.tileSpriteIndex[tile_sprite_alias::kAliasSourceCode];
            break;
        }
        case GameState::MenuOverlay:
            game->reset_overlay_render_state();
            if (!game->ui.useDefaultOverlaySelection) game->ui.cursorDragY = 2;
            game->ui.useDefaultOverlaySelection = true;
            break;
        case GameState::GeneratingMap:
            game->stateWorkStep = state_work::kMapGenerationStart;
            break;
        case GameState::Playing:
            game->renderer.backbuffer = NULL;
            game->renderer.resize_canvas(game->renderer.canvasWidth,
                                         game->renderer.canvasHeight);
            game->renderer.lastViewportCenterX = -1;
            game->ui.menuDirty = true;
            break;
        case GameState::BuildingMinimap:
            game->stateWorkStep = state_work::kDeferredOperationStart;
            game->ui.confirmToggle = 0;
            break;
        case GameState::BuildingPath:
            game->stateWorkStep = state_work::kDeferredOperationStart;
            break;
        case GameState::SavingGame:
            game->stateWorkStep = state_work::kDeferredOperationStart;
            break;
        case GameState::LoadingGame:
            game->stateWorkStep = state_work::kDeferredOperationStart;
            break;
        case GameState::AiTurn:
            game->stateWorkStep = state_work::kDeferredOperationStart;
            break;
        case GameState::HighScoreSync:
            game->stateWorkStep = state_work::kDeferredOperationStart;
            break;
    }
    game->gameState = state;
}

// The host owns the loop; start only loads saved settings and enters boot.
void Game::start() {
    Game* game = this;
    game->load_settings_snapshot();
    game->set_state(GameState::BootSplash0);
    // Display.setCurrent + Thread(this).start() are replaced by the host loop.
}

// public void run() -- ONE iteration of the original game-thread loop.
// The host (main.c) calls this each frame; timing/repaint/sleep are host-owned.
void Game::advance_frame() {
    Game* game = this;
    if (game->stopRequested) return;
    if (game->ui.keyRepeatDelay == 0) {
        if (game->ui.heldKey != input_key::NoKey)
            game->ui.currentKey = game->ui.heldKey;
    } else {
        --game->ui.keyRepeatDelay;
    }
    if (game->ui.currentKey != input_key::NoKey
        && game->ui.currentKey != input_key::Released)
        game->handle_key(game->ui.currentKey);
    if (game->ui.currentKey == input_key::Released)
        game->ui.reset_key_state(game->ui.currentKey);
    if (game->ui.inputSubState > 0) --game->ui.inputSubState;
    if (game->ui.inputPending) {
        game->frameIndex = 1;
        while (game->frameIndex > 0) {
            game->update_state_machine();
            if (game->gameState != GameState::MenuOverlay && game->gameState != GameState::MapOverview) continue;
            game->ui.inputPending = false;
            break;
        }
        game->frameIndex = 0;
    }
    if (game->gameState == GameState::LoadingResources) {
        for (int loading_step = 0; loading_step < 5; ++loading_step)
            game->update_state_machine();
    }
    if (game->gameState == GameState::Playing) {
        for (int simulation_step = 0;
             simulation_step < game->simSpeed; ++simulation_step) {
            if (!game->ui.pauseFlag) game->update_state_machine();
            if (!game->ui.menuRowEnabled[game->ui.buildMenuMode][game->ui.buildMenuItemIndex]) continue;
            game->viewport_zoom_key();
        }
    } else {
        game->update_state_machine();
    }
}
