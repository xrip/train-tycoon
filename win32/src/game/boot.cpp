// Game start, resources, sprites, state changes, and save start.
#include "game_methods.h"
#include "support/record_store.h"
#include <stdlib.h>
#include <string.h>
#include <vector>

static int read_u16_be(ResourceReader& resource) {
    int high = resource.read_byte();
    int low = resource.read_byte();
    return high * 256 + low;
}

// public void keyPressed(int n)
void gc_keyPressed(GameCanvas *gc, int32_t n) {
    if (gc->ui.inputPending) { gc->ui.inputPending = false; return; }
    gc->ui.currentKey = n;
}

// public void resetOverlayRenderState()
void gc_resetOverlayRenderState(GameCanvas *gc) {
    gc->ui.cursorScale = 0;
    gc->ui.cursorDragY = -1;
    gc->ui.cursorSelectY = -1;
    gc->ui.cursorSelectX = -1;
    gc->ui.cursorOverUI = false;
    gc->ui.cursorAllowZoomOut = false;
    gc->renderer.dialogContentHeight = 0;
    gc->renderer.needsRepaint = false;
}

void gc_loadPackedSpriteData(GameCanvas *gc, const char *resource_name) {
    ResourceReader in = resources::open(resource_name);
    if (!in) return;
    int n = in.read_byte();
    int n2 = in.read_byte();
    int n3 = read_u16_be(in);
    int n4 = 0;
    gc->assets.spriteOffsetTable.assign(static_cast<std::size_t>(n2 + 1), 0);
    gc->assets.spriteData.assign(static_cast<std::size_t>(n3), 0);
    for (int i = 0; i < n; ++i) {
        int n5 = in.read_byte();
        int n6 = read_u16_be(in);
        gc->assets.spriteOffsetTable[n5] = (int16_t)n4;
        for (int j = 0; j < n6; ++j) {
            gc->assets.spriteData[n4] = (int8_t)in.read_byte();
            ++n4;
        }
    }
}

// public Image decodePngFromSpriteStream() -- rebuild a PNG from the stream
std::unique_ptr<Image> gc_decodePngFromSpriteStream(GameCanvas *gc) {
    std::vector<int8_t> png_bytes;
    int png_size = 0;
    int image_format = gc->assets.sprite_stream.read_byte();
    if (image_format == 2) {
        png_size = read_u16_be(gc->assets.sprite_stream);
        png_bytes.resize(static_cast<std::size_t>(png_size));
        png_bytes[0] = -119; png_bytes[1] = 80; png_bytes[2] = 78; png_bytes[3] = 71;
        for (int index = 4; index < png_size; ++index)
            png_bytes[index] = (int8_t)gc->assets.sprite_stream.read_byte();
    }
    if (image_format == 1) {
        int image_data_size = read_u16_be(gc->assets.sprite_stream);
        int palette_byte_count = gc->assets.sprite_stream.read_byte() * 3;
        int transparency_count = gc->assets.sprite_stream.read_byte();
        png_size = image_data_size + palette_byte_count + 69 +
                   transparency_count * 13;
        png_bytes.resize(static_cast<std::size_t>(png_size));
        png_bytes[0] = -119; png_bytes[1] = 80; png_bytes[2] = 78; png_bytes[3] = 71;
        png_bytes[4] = 13; png_bytes[5] = 10; png_bytes[6] = 26; png_bytes[7] = 10;
        png_bytes[11] = 13; png_bytes[12] = 73; png_bytes[13] = 72; png_bytes[14] = 68; png_bytes[15] = 82;
        png_bytes[18] = (int8_t)gc->assets.sprite_stream.read_byte();
        png_bytes[19] = (int8_t)gc->assets.sprite_stream.read_byte();
        png_bytes[22] = (int8_t)gc->assets.sprite_stream.read_byte();
        png_bytes[23] = (int8_t)gc->assets.sprite_stream.read_byte();
        png_bytes[24] = (int8_t)gc->assets.sprite_stream.read_byte();
        png_bytes[25] = 3;
        png_bytes[29] = (int8_t)gc->assets.sprite_stream.read_byte();
        png_bytes[30] = (int8_t)gc->assets.sprite_stream.read_byte();
        png_bytes[31] = (int8_t)gc->assets.sprite_stream.read_byte();
        png_bytes[32] = (int8_t)gc->assets.sprite_stream.read_byte();
        png_bytes[35] = (int8_t)(palette_byte_count >> 8 & 0xFF);
        png_bytes[36] = (int8_t)(palette_byte_count & 0xFF);
        png_bytes[37] = 80; png_bytes[38] = 76; png_bytes[39] = 84; png_bytes[40] = 69;
        int output_index;
        for (output_index = 0; output_index < palette_byte_count + 4;
             ++output_index) {
            png_bytes[41 + output_index] =
                (int8_t)gc->assets.sprite_stream.read_byte();
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
            png_bytes[output_index++] = (int8_t)gc->assets.sprite_stream.read_byte();
        output_index += 4;
        png_bytes[output_index++] = 73; png_bytes[output_index++] = 69; png_bytes[output_index++] = 78; png_bytes[output_index++] = 68;
        png_bytes[output_index++] = -82; png_bytes[output_index++] = 66; png_bytes[output_index++] = 96; png_bytes[output_index++] = -126;
    }
    if (png_bytes.empty()) return nullptr;
    return Image::decode_png(std::span<const uint8_t>(
        reinterpret_cast<const uint8_t*>(png_bytes.data()), png_bytes.size()));
}

// byte[] m(int n)
std::vector<int8_t> gc_readPackedTextCodes(GameCanvas *gc, int32_t offset) {
    const int code_count = gc->assets.spriteData[offset];
    return {gc->assets.spriteData.data() + offset + 1,
            gc->assets.spriteData.data() + offset + 1 + code_count};
}

// void loadGame(int n, int n2)
void gc_loadGame(GameCanvas *gc, int32_t slot, int32_t mode) {
    gc->saveCodec.saveSlot = slot;
    gc->saveCodec.savePendingMode = mode;
    gc->saveCodec.saveReadOffset = 0;
    gc->saveCodec.snapshotSize = 0;
    gc->saveCodec.snapshotLoaded = false;
    if (gc->saveCodec.savePendingMode == save::MODE_READ) {
        std::unique_ptr<RecordStore> store = RecordStore::open("TTStore", true);
        if (store) {
            int nr = store->record_count();
            for (int i = 0; i < nr; ++i) {
                int id = store->record_id_at(i);
                const auto record = store->record(id);
                if (!record) continue;
                const int len = static_cast<int>(record->size());
                if (len > 0 && (int8_t)(*record)[0] == slot) {
                    gc->saveCodec.saveBuffer.assign(static_cast<std::size_t>(len - 1), 0);
                    memcpy(gc->saveCodec.saveBuffer.data(), record->data() + 1,
                           (size_t)(len - 1));
                    gc->saveCodec.snapshotSize = len - 1;
                    gc->saveCodec.snapshotLoaded = true;
                }
            }
        }
    } else {
        gc->saveCodec.snapshotLoaded = true;
    }
}

// void writeSaveSnapshot()
void gc_writeSaveSnapshot(GameCanvas *gc) {
    gcs_animationsDisabled = gc_saveOrLoadBool(gc, gcs_animationsDisabled);
    gcs_selectionHidden = gc_saveOrLoadBool(gc, gcs_selectionHidden);
    gcs_monthlyReportEnabled = gc_saveOrLoadBool(gc, gcs_monthlyReportEnabled);
    gcs_annualReportEnabled = gc_saveOrLoadBool(gc, gcs_annualReportEnabled);
    gc->scenario.scenarioStepCompare = gc_saveIntToBuffer(gc, gc->scenario.scenarioStepCompare);
    gc->scenario.bestScenarioScore = gc_saveIntToBuffer(gc, gc->scenario.bestScenarioScore);
    gc->music_volume_level = gc_saveIntToBuffer(gc, gc->music_volume_level);
    gc_saveOrLoadIntArrayInlined(gc, gc->scenario.scenarioScores);
    gc_saveOrLoadIntArrayInlined(gc, gc->scenario.leaderboardScores);
    gc_saveOrLoadByteArrayInlined(gc, gc->scenario.playerNameCodes);
}

// void persistSnapshotMode1()
void gc_persistSnapshotMode1(GameCanvas *gc) {
    gc_loadGame(gc, 5, save::MODE_READ);
    gc_writeSaveSnapshot(gc);
    gc_setMusicVolume(gc, gc->music_volume_level);
}

// Add one isometric tile-draw offset pair.
void gc_appendTileDrawOffset(GameCanvas *gc, ByteBuffer& x_offsets,
                             ByteBuffer& y_offsets, int32_t x, int32_t y) {
    (void)gc;
    x_offsets.write(x);
    y_offsets.write(y);
}

// public void k(int n, int n2) -- build+sort the iso tile-draw order tables
void gc_k(GameCanvas *gc, int32_t n, int32_t n2) {
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
            gc_appendTileDrawOffset(gc, x_offsets, y_offsets, n3, n8);
        }
    }
    const auto x_offset_bytes = x_offsets.bytes();
    const auto y_offset_bytes = y_offsets.bytes();
    int sz = static_cast<int>(x_offset_bytes.size());
    gc->assets.tileOffsetX.resize(static_cast<std::size_t>(sz));
    memcpy(gc->assets.tileOffsetX.data(), x_offset_bytes.data(), (size_t)sz);
    gc->assets.tileOffsetY.resize(static_cast<std::size_t>(sz));
    memcpy(gc->assets.tileOffsetY.data(), y_offset_bytes.data(), (size_t)sz);
    gc->assets.numActiveTileAnimations = sz;
    do {
        n5 = 0;
        for (n4 = 0; n4 < gc->assets.numActiveTileAnimations - 1; ++n4) {
            if (gc->assets.tileOffsetY[n4] <= gc->assets.tileOffsetY[n4 + 1]) continue;
            n3 = gc->assets.tileOffsetY[n4]; gc->assets.tileOffsetY[n4] = gc->assets.tileOffsetY[n4 + 1]; gc->assets.tileOffsetY[n4 + 1] = (int8_t)n3;
            n3 = gc->assets.tileOffsetX[n4]; gc->assets.tileOffsetX[n4] = gc->assets.tileOffsetX[n4 + 1]; gc->assets.tileOffsetX[n4 + 1] = (int8_t)n3;
            n5 = 1;
        }
    } while (n5 != 0);
    do {
        n5 = 0;
        for (n4 = 1; n4 < gc->assets.numActiveTileAnimations; ++n4) {
            if (gc->assets.tileOffsetX[n4] <= gc->assets.tileOffsetX[n4 - 1] || gc->assets.tileOffsetY[n4] != gc->assets.tileOffsetY[n4 - 1]) continue;
            n3 = gc->assets.tileOffsetY[n4]; gc->assets.tileOffsetY[n4] = gc->assets.tileOffsetY[n4 - 1]; gc->assets.tileOffsetY[n4 - 1] = (int8_t)n3;
            n3 = gc->assets.tileOffsetX[n4]; gc->assets.tileOffsetX[n4] = gc->assets.tileOffsetX[n4 - 1]; gc->assets.tileOffsetX[n4 - 1] = (int8_t)n3;
            n5 = 1;
        }
    } while (n5 != 0);
}

// void setCanvasSize(int n, int n2)
void gc_setCanvasSize(GameCanvas *gc, int32_t n, int32_t n2) {
    if (gc->lastCanvasWidth != n || gc->lastCanvasHeight != n2) {
        gcs_screenWidth = n;
        gcs_hs = n2;
        gc->renderer.backbuffer = Image::create_blank(gcs_screenWidth, gcs_hs);
        gc->renderer.backbuffer_surface = gc->renderer.backbuffer->drawing_surface();
        gcs_gameplayViewportHeight = gcs_hs - 30;
        gcs_bottomBarY = gcs_hs - 15;
        gc_k(gc, n, n2);
        gc->lastCanvasWidth = n;
        gc->lastCanvasHeight = n2;
        gc->ui.menuDirty = true;
        gc->renderer.lastViewportCenterX = -1;
        if (gc->gameState == +GameState::MenuOverlay && !gc->ready) {
            if (gc->savedGameState == +GameState::Playing) { gc->ui.pendingTransitionCode = gc->ui.transitionCode; gc->gameState = +GameState::Playing; }
            if (gc->savedGameState == +GameState::MapOverview) { gc->ui.pendingTransitionCode = gc->ui.transitionCode; gc->gameState = +GameState::MapOverview; }
        }
    }
}

// public void setState(int n)
void gc_setState(GameCanvas *gc, int32_t n) {
    gc->savedGameState = gc->gameState;
    if (!gc->ready) gc->assets.cycle_flag = NULL;
    gc->ui.snapshotLoadedFlag = false;
    gc->ui.menuDirty = true;
    switch (n) {
        case +GameState::BootSplash0:
            gc_openSpriteStream(gc, "sprite_images.bin");
            gc_loadPackedSpriteData(gc, "packed_sprites.bin");
            gc->loadingTimer = 0;
            /* fall through */
        case +GameState::BootSplash1:
        case +GameState::BootSplash2:
            gc->assets.cycle_flag = gc_decodePngFromSpriteStream(gc);
            gc->loadingStartTime = midp_current_time_millis();
            gc->loadingTimeout = false;
            gc->loadingTimer = 0;
            break;
        case +GameState::IntroReady: {
            ResourceReader in = resources::open("sprite_image_map.bin");
            if (in) {
                int n2 = in.read_byte();
                int n3 = in.read_byte();
                gc->assets.spriteBitmapTable.resize(n2, 0);
                gc->assets.spriteIndexTable.assign(static_cast<std::size_t>(n3 + 1), 0);
                for (int i = 0; i < n2; ++i) {
                    int n4 = in.read_byte();
                    int n5 = in.read_byte();
                    gc->assets.spriteBitmapTable[i].assign(static_cast<std::size_t>(n5), 0);
                    gc->assets.spriteIndexTable[n4] = (int8_t)i;
                    for (int j = 0; j < n5; ++j)
                        gc->assets.spriteBitmapTable[i][j] = (int8_t)in.read_byte();
                }
            }
            break;
        }
        case +GameState::LoadingResources: {
            gc->assets.spriteLoadCounter = 0;
            ResourceReader in = resources::open("tile_sprite_ids.bin");
            if (in) {
                gc->scenario.scenarioResourceCount = in.read_byte();
                for (int i = 0; i < gc->scenario.scenarioResourceCount; ++i) {
                    int n6 = in.read_byte();
                    gc->assets.tileSpriteIndex[n6] = (int8_t)(i - 8);
                }
            }
            gc->scenario.scenarioResourceCount -= 8;
            gc->assets.tile_sprites.clear();
            gc->assets.tile_sprites.resize(
                static_cast<std::size_t>(gc->scenario.scenarioResourceCount));
            gc->assets.tileSpriteIndex[68] = gc->assets.tileSpriteIndex[66];
            break;
        }
        case +GameState::MenuOverlay:
            gc_resetOverlayRenderState(gc);
            if (!gc->ui.useDefaultOverlaySelection) gc->ui.cursorDragY = 2;
            gc->ui.useDefaultOverlaySelection = true;
            break;
        case +GameState::GeneratingMap:
            gc->loadStep = 0;
            break;
        case +GameState::Playing:
            gc->renderer.backbuffer = NULL;
            gc->renderer.backbuffer = Image::create_blank(gcs_screenWidth, gcs_hs);
            gc->renderer.backbuffer_surface = gc->renderer.backbuffer->drawing_surface();
            gc->renderer.lastViewportCenterX = -1;
            gc->ui.menuDirty = true;
            break;
        case +GameState::BuildingMinimap: gc->loadStep = 4; gc->ui.confirmToggle = 0; break;
        case +GameState::BuildingPath: gc->loadStep = 4; break;
        case +GameState::SavingGame: gc->loadStep = 4; break;
        case +GameState::LoadingGame: gc->loadStep = 4; break;
        case +GameState::AiTurn: gc->loadStep = 4; break;
        case +GameState::HighScoreSync: gc->loadStep = 4; break;
    }
    gc->gameState = n;
}

// public void start(GameMIDlet gameMIDlet) -- host owns the loop, no thread
void gc_start(GameCanvas *gc, void *gameMIDlet) {
    gc->midlet = gameMIDlet;
    gc_persistSnapshotMode1(gc);
    gc_setState(gc, +GameState::BootSplash0);
    // Display.setCurrent + Thread(this).start() are replaced by the host loop.
}

// public void run() -- ONE iteration of the original game-thread loop.
// The host (main.c) calls this each frame; timing/repaint/sleep are host-owned.
void gc_run(GameCanvas *gc) {
    if (gc->stopRequested || gc->ui.paused) return;
    gc->frameStartTime = midp_current_time_millis();
    if (gc->ui.keyRepeatDelay == 0) {
        if (gc->ui.heldKey != 0) gc->ui.currentKey = gc->ui.heldKey;
    } else {
        --gc->ui.keyRepeatDelay;
    }
    if (gc->ui.currentKey != 0 && gc->ui.currentKey != -999) gc_handleKey(gc, gc->ui.currentKey);
    if (gc->ui.currentKey == -999) gc_resetKeyState(gc, gc->ui.currentKey);
    if (gc->ui.inputSubState > 0) --gc->ui.inputSubState;
    ++gc->frameCount;
    if (gc->ui.inputPending) {
        gc->frameIndex = 1;
        while (gc->frameIndex > 0) {
            gc_update(gc);
            if (gc->gameState != +GameState::MenuOverlay && gc->gameState != +GameState::MapOverview) continue;
            gc->ui.inputPending = false;
            break;
        }
        gc->frameIndex = 0;
    }
    if (gc->gameState == +GameState::LoadingResources) {
        for (int n = 0; n < 5; ++n) gc_update(gc);
    }
    if (gc->gameState == +GameState::Playing) {
        for (int n = 0; n < gc->simSpeed; ++n) {
            if (!gc->ui.pauseFlag) gc_update(gc);
            if (!gc->ui.menuRowEnabled[gc->ui.buildMenuMode][gc->ui.buildMenuItemIndex]) continue;
            gc_viewportZoomKey(gc);
        }
    } else {
        gc_update(gc);
    }
    gc->frameTime = midp_current_time_millis() - gc->frameStartTime;
}
