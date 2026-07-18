// Game start, resources, sprites, state changes, and save start.
#include "game.h"
#include "support/byte_buffer.h"
#include "text/text_catalog.h"
#include <stdlib.h>
#include <string.h>
#include <vector>

static int read_u16_be(ResourceReader& resource) {
    int high = resource.read_byte();
    int low = resource.read_byte();
    return high * 256 + low;
}

void Game::key_pressed(int32_t n) {
    Game* game = this;
    if (game->ui.inputPending) { game->ui.inputPending = false; return; }
    game->ui.currentKey = n;
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
    int png_size = 0;
    int image_format = sprite_stream.read_byte();
    if (image_format == 2) {
        png_size = read_u16_be(sprite_stream);
        png_bytes.resize(static_cast<std::size_t>(png_size));
        png_bytes[0] = -119; png_bytes[1] = 80; png_bytes[2] = 78; png_bytes[3] = 71;
        for (int index = 4; index < png_size; ++index)
            png_bytes[index] = (int8_t)sprite_stream.read_byte();
    }
    if (image_format == 1) {
        int image_data_size = read_u16_be(sprite_stream);
        int palette_byte_count = sprite_stream.read_byte() * 3;
        int transparency_count = sprite_stream.read_byte();
        png_size = image_data_size + palette_byte_count + 69 +
                   transparency_count * 13;
        png_bytes.resize(static_cast<std::size_t>(png_size));
        png_bytes[0] = -119; png_bytes[1] = 80; png_bytes[2] = 78; png_bytes[3] = 71;
        png_bytes[4] = 13; png_bytes[5] = 10; png_bytes[6] = 26; png_bytes[7] = 10;
        png_bytes[11] = 13; png_bytes[12] = 73; png_bytes[13] = 72; png_bytes[14] = 68; png_bytes[15] = 82;
        png_bytes[18] = (int8_t)sprite_stream.read_byte();
        png_bytes[19] = (int8_t)sprite_stream.read_byte();
        png_bytes[22] = (int8_t)sprite_stream.read_byte();
        png_bytes[23] = (int8_t)sprite_stream.read_byte();
        png_bytes[24] = (int8_t)sprite_stream.read_byte();
        png_bytes[25] = 3;
        png_bytes[29] = (int8_t)sprite_stream.read_byte();
        png_bytes[30] = (int8_t)sprite_stream.read_byte();
        png_bytes[31] = (int8_t)sprite_stream.read_byte();
        png_bytes[32] = (int8_t)sprite_stream.read_byte();
        png_bytes[35] = (int8_t)(palette_byte_count >> 8 & 0xFF);
        png_bytes[36] = (int8_t)(palette_byte_count & 0xFF);
        png_bytes[37] = 80; png_bytes[38] = 76; png_bytes[39] = 84; png_bytes[40] = 69;
        int output_index;
        for (output_index = 0; output_index < palette_byte_count + 4;
             ++output_index) {
            png_bytes[41 + output_index] =
                (int8_t)sprite_stream.read_byte();
        }
        output_index = 41 + palette_byte_count + 4;
        if (transparency_count == 1) {
            output_index += 3;
            png_bytes[output_index++] = 1;
            png_bytes[output_index++] = 116; png_bytes[output_index++] = 82; png_bytes[output_index++] = 78; png_bytes[output_index++] = 83;
            png_bytes[++output_index] = 64;
            png_bytes[++output_index] = -26;
            png_bytes[++output_index] = -40;
            int transparency_crc_index = ++output_index;
            ++output_index;
            png_bytes[transparency_crc_index] = 102;
        }
        output_index += 2;
        png_bytes[output_index++] = (int8_t)(image_data_size >> 8 & 0xFF);
        png_bytes[output_index++] = (int8_t)(image_data_size & 0xFF);
        png_bytes[output_index++] = 73; png_bytes[output_index++] = 68; png_bytes[output_index++] = 65; png_bytes[output_index++] = 84;
        for (int index = 0; index < image_data_size + 4; ++index)
            png_bytes[output_index++] = (int8_t)sprite_stream.read_byte();
        output_index += 4;
        png_bytes[output_index++] = 73; png_bytes[output_index++] = 69; png_bytes[output_index++] = 78; png_bytes[output_index++] = 68;
        png_bytes[output_index++] = -82; png_bytes[output_index++] = 66; png_bytes[output_index++] = 96; png_bytes[output_index++] = -126;
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
    game->saveCodec.load_slot(5, save::MODE_READ);
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

// public void k(int n, int n2) -- build+sort the iso tile-draw order tables
void Assets::build_tile_draw_order(int32_t n, int32_t n2) {
    ByteBuffer x_offsets;
    ByteBuffer y_offsets;
    int n3, n4, n5;
    n2 += n2 % 8;
    int n6 = (n2 -= 30) / 8 + 6;
    int n7 = n / 32 + 2;
    for (n5 = 0; n5 < n6; ++n5) {
        for (n4 = -1; n4 < n7; ++n4) {
            n3 = n4 - n5 / 2;
            int n8 = n5 % 2 == 0 ? 0 + n4 + n5 / 2 : -1 + n4 + n5 / 2;
            append_tile_draw_offset(x_offsets, y_offsets, n3, n8);
        }
    }
    const auto x_offset_bytes = x_offsets.bytes();
    const auto y_offset_bytes = y_offsets.bytes();
    int sz = static_cast<int>(x_offset_bytes.size());
    tileOffsetX.resize(static_cast<std::size_t>(sz));
    memcpy(tileOffsetX.data(), x_offset_bytes.data(), (size_t)sz);
    tileOffsetY.resize(static_cast<std::size_t>(sz));
    memcpy(tileOffsetY.data(), y_offset_bytes.data(), (size_t)sz);
    numActiveTileAnimations = sz;
    do {
        n5 = 0;
        for (n4 = 0; n4 < numActiveTileAnimations - 1; ++n4) {
            if (tileOffsetY[n4] <= tileOffsetY[n4 + 1]) continue;
            n3 = tileOffsetY[n4]; tileOffsetY[n4] = tileOffsetY[n4 + 1]; tileOffsetY[n4 + 1] = (int8_t)n3;
            n3 = tileOffsetX[n4]; tileOffsetX[n4] = tileOffsetX[n4 + 1]; tileOffsetX[n4 + 1] = (int8_t)n3;
            n5 = 1;
        }
    } while (n5 != 0);
    do {
        n5 = 0;
        for (n4 = 1; n4 < numActiveTileAnimations; ++n4) {
            if (tileOffsetX[n4] <= tileOffsetX[n4 - 1] || tileOffsetY[n4] != tileOffsetY[n4 - 1]) continue;
            n3 = tileOffsetY[n4]; tileOffsetY[n4] = tileOffsetY[n4 - 1]; tileOffsetY[n4 - 1] = (int8_t)n3;
            n3 = tileOffsetX[n4]; tileOffsetX[n4] = tileOffsetX[n4 - 1]; tileOffsetX[n4 - 1] = (int8_t)n3;
            n5 = 1;
        }
    } while (n5 != 0);
}

void Game::set_canvas_size(int32_t n, int32_t n2) {
    Game *game = this;
    if (game->lastCanvasWidth != n || game->lastCanvasHeight != n2) {
        game->renderer.resize_canvas(n, n2);
        game->assets.build_tile_draw_order(n, n2);
        game->lastCanvasWidth = n;
        game->lastCanvasHeight = n2;
        game->ui.menuDirty = true;
        game->renderer.lastViewportCenterX = -1;
        if (game->gameState == GameState::MenuOverlay && !game->ready) {
            if (game->savedGameState == GameState::Playing) { game->ui.pendingTransitionCode = game->ui.transitionCode; game->gameState = GameState::Playing; }
            if (game->savedGameState == GameState::MapOverview) { game->ui.pendingTransitionCode = game->ui.transitionCode; game->gameState = GameState::MapOverview; }
        }
    }
}

void Game::set_state(int32_t n) {
    Game *game = this;
    game->savedGameState = game->gameState;
    if (!game->ready) game->assets.cycle_flag = NULL;
    game->ui.snapshotLoadedFlag = false;
    game->ui.menuDirty = true;
    switch (n) {
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
                int n2 = in.read_byte();
                int n3 = in.read_byte();
                game->assets.spriteBitmapTable.resize(n2, 0);
                game->assets.spriteIndexTable.assign(static_cast<std::size_t>(n3 + 1), 0);
                for (int i = 0; i < n2; ++i) {
                    int n4 = in.read_byte();
                    int n5 = in.read_byte();
                    game->assets.spriteBitmapTable[i].assign(static_cast<std::size_t>(n5), 0);
                    game->assets.spriteIndexTable[n4] = (int8_t)i;
                    for (int j = 0; j < n5; ++j)
                        game->assets.spriteBitmapTable[i][j] = (int8_t)in.read_byte();
                }
            }
            break;
        }
        case GameState::LoadingResources: {
            game->assets.spriteLoadCounter = 0;
            ResourceReader in = resources::open("tile_sprite_ids.bin");
            if (in) {
                game->scenario.scenarioResourceCount = in.read_byte();
                for (int i = 0; i < game->scenario.scenarioResourceCount; ++i) {
                    int n6 = in.read_byte();
                    game->assets.tileSpriteIndex[n6] = (int8_t)(i - 8);
                }
            }
            game->scenario.scenarioResourceCount -= 8;
            game->assets.tile_sprites.clear();
            game->assets.tile_sprites.resize(
                static_cast<std::size_t>(game->scenario.scenarioResourceCount));
            game->assets.tileSpriteIndex[68] = game->assets.tileSpriteIndex[66];
            break;
        }
        case GameState::MenuOverlay:
            game->reset_overlay_render_state();
            if (!game->ui.useDefaultOverlaySelection) game->ui.cursorDragY = 2;
            game->ui.useDefaultOverlaySelection = true;
            break;
        case GameState::GeneratingMap:
            game->stateWorkStep = 0;
            break;
        case GameState::Playing:
            game->renderer.backbuffer = NULL;
            game->renderer.resize_canvas(game->renderer.canvasWidth,
                                         game->renderer.canvasHeight);
            game->renderer.lastViewportCenterX = -1;
            game->ui.menuDirty = true;
            break;
        case GameState::BuildingMinimap: game->stateWorkStep = 4; game->ui.confirmToggle = 0; break;
        case GameState::BuildingPath: game->stateWorkStep = 4; break;
        case GameState::SavingGame: game->stateWorkStep = 4; break;
        case GameState::LoadingGame: game->stateWorkStep = 4; break;
        case GameState::AiTurn: game->stateWorkStep = 4; break;
        case GameState::HighScoreSync: game->stateWorkStep = 4; break;
    }
    game->gameState = n;
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
        if (game->ui.heldKey != 0) game->ui.currentKey = game->ui.heldKey;
    } else {
        --game->ui.keyRepeatDelay;
    }
    if (game->ui.currentKey != 0 && game->ui.currentKey != -999)
        game->handle_key(game->ui.currentKey);
    if (game->ui.currentKey == -999)
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
        for (int n = 0; n < 5; ++n) game->update_state_machine();
    }
    if (game->gameState == GameState::Playing) {
        for (int n = 0; n < game->simSpeed; ++n) {
            if (!game->ui.pauseFlag) game->update_state_machine();
            if (!game->ui.menuRowEnabled[game->ui.buildMenuMode][game->ui.buildMenuItemIndex]) continue;
            game->viewport_zoom_key();
        }
        if (game->roadPocEnabled) game->road_poc_tick();
    } else {
        game->update_state_machine();
    }
}
