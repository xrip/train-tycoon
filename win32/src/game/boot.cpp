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
    if (gc->inputPending) { gc->inputPending = false; return; }
    gc->currentKey = n;
}

// public void resetOverlayRenderState()
void gc_resetOverlayRenderState(GameCanvas *gc) {
    gc->cursorScale = 0;
    gc->cursorDragY = -1;
    gc->cursorSelectY = -1;
    gc->cursorSelectX = -1;
    gc->cursorOverUI = false;
    gc->cursorAllowZoomOut = false;
    gc->dialogContentHeight = 0;
    gc->needsRepaint = false;
}

void gc_loadPackedSpriteData(GameCanvas *gc, const char *resource_name) {
    ResourceReader in = resources::open(resource_name);
    if (!in) return;
    int n = in.read_byte();
    int n2 = in.read_byte();
    int n3 = read_u16_be(in);
    int n4 = 0;
    gc->spriteOffsetTable.assign(static_cast<std::size_t>(n2 + 1), 0);
    gc->spriteData.assign(static_cast<std::size_t>(n3), 0);
    for (int i = 0; i < n; ++i) {
        int n5 = in.read_byte();
        int n6 = read_u16_be(in);
        gc->spriteOffsetTable[n5] = (int16_t)n4;
        for (int j = 0; j < n6; ++j) {
            gc->spriteData[n4] = (int8_t)in.read_byte();
            ++n4;
        }
    }
}

// public Image decodePngFromSpriteStream() -- rebuild a PNG from the stream
std::unique_ptr<Image> gc_decodePngFromSpriteStream(GameCanvas *gc) {
    std::vector<int8_t> png_bytes;
    int png_size = 0;
    int image_format = gc->sprite_stream.read_byte();
    if (image_format == 2) {
        png_size = read_u16_be(gc->sprite_stream);
        png_bytes.resize(static_cast<std::size_t>(png_size));
        png_bytes[0] = -119; png_bytes[1] = 80; png_bytes[2] = 78; png_bytes[3] = 71;
        for (int index = 4; index < png_size; ++index)
            png_bytes[index] = (int8_t)gc->sprite_stream.read_byte();
    }
    if (image_format == 1) {
        int image_data_size = read_u16_be(gc->sprite_stream);
        int palette_byte_count = gc->sprite_stream.read_byte() * 3;
        int transparency_count = gc->sprite_stream.read_byte();
        png_size = image_data_size + palette_byte_count + 69 +
                   transparency_count * 13;
        png_bytes.resize(static_cast<std::size_t>(png_size));
        png_bytes[0] = -119; png_bytes[1] = 80; png_bytes[2] = 78; png_bytes[3] = 71;
        png_bytes[4] = 13; png_bytes[5] = 10; png_bytes[6] = 26; png_bytes[7] = 10;
        png_bytes[11] = 13; png_bytes[12] = 73; png_bytes[13] = 72; png_bytes[14] = 68; png_bytes[15] = 82;
        png_bytes[18] = (int8_t)gc->sprite_stream.read_byte();
        png_bytes[19] = (int8_t)gc->sprite_stream.read_byte();
        png_bytes[22] = (int8_t)gc->sprite_stream.read_byte();
        png_bytes[23] = (int8_t)gc->sprite_stream.read_byte();
        png_bytes[24] = (int8_t)gc->sprite_stream.read_byte();
        png_bytes[25] = 3;
        png_bytes[29] = (int8_t)gc->sprite_stream.read_byte();
        png_bytes[30] = (int8_t)gc->sprite_stream.read_byte();
        png_bytes[31] = (int8_t)gc->sprite_stream.read_byte();
        png_bytes[32] = (int8_t)gc->sprite_stream.read_byte();
        png_bytes[35] = (int8_t)(palette_byte_count >> 8 & 0xFF);
        png_bytes[36] = (int8_t)(palette_byte_count & 0xFF);
        png_bytes[37] = 80; png_bytes[38] = 76; png_bytes[39] = 84; png_bytes[40] = 69;
        int output_index;
        for (output_index = 0; output_index < palette_byte_count + 4;
             ++output_index) {
            png_bytes[41 + output_index] =
                (int8_t)gc->sprite_stream.read_byte();
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
            png_bytes[output_index++] = (int8_t)gc->sprite_stream.read_byte();
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
    const int code_count = gc->spriteData[offset];
    return {gc->spriteData.data() + offset + 1,
            gc->spriteData.data() + offset + 1 + code_count};
}

// void loadGame(int n, int n2)
void gc_loadGame(GameCanvas *gc, int32_t n, int32_t n2) {
    gc->saveSlot = n;
    gc->savePendingMode = n2;
    gc->saveReadOffset = 0;
    gc->snapshotSize = 0;
    gc->snapshotLoaded = false;
    if (gc->savePendingMode == 1) {
        std::unique_ptr<RecordStore> store = RecordStore::open("TTStore", true);
        if (store) {
            int nr = store->record_count();
            for (int i = 0; i < nr; ++i) {
                int id = store->record_id_at(i);
                const auto record = store->record(id);
                if (!record) continue;
                const int len = static_cast<int>(record->size());
                if (len > 0 && (int8_t)(*record)[0] == n) {
                    gc->saveBuffer = GC_NEW(int8_t, len - 1);
                    memcpy(gc->saveBuffer, record->data() + 1,
                           (size_t)(len - 1));
                    gc->snapshotSize = len - 1;
                    gc->snapshotLoaded = true;
                }
            }
        }
    } else {
        gc->snapshotLoaded = true;
    }
}

// void writeSaveSnapshot()
void gc_writeSaveSnapshot(GameCanvas *gc) {
    gcs_animationsDisabled = gc_saveOrLoadBool(gc, gcs_animationsDisabled);
    gcs_selectionHidden = gc_saveOrLoadBool(gc, gcs_selectionHidden);
    gcs_monthlyReportEnabled = gc_saveOrLoadBool(gc, gcs_monthlyReportEnabled);
    gcs_annualReportEnabled = gc_saveOrLoadBool(gc, gcs_annualReportEnabled);
    gc->scenarioStepCompare = gc_saveIntToBuffer(gc, gc->scenarioStepCompare);
    gc->bestScenarioScore = gc_saveIntToBuffer(gc, gc->bestScenarioScore);
    gc->music_volume_level = gc_saveIntToBuffer(gc, gc->music_volume_level);
    gc_saveOrLoadIntArrayInlined(gc, gc->scenarioScores);
    gc_saveOrLoadIntArrayInlined(gc, gc->leaderboardScores);
    gc_saveOrLoadByteArrayInlined(gc, gc->playerNameCodes);
}

// void persistSnapshotMode1()
void gc_persistSnapshotMode1(GameCanvas *gc) {
    gc_loadGame(gc, 5, 1);
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
    gc->tileOffsetX.resize(static_cast<std::size_t>(sz));
    memcpy(gc->tileOffsetX.data(), x_offset_bytes.data(), (size_t)sz);
    gc->tileOffsetY.resize(static_cast<std::size_t>(sz));
    memcpy(gc->tileOffsetY.data(), y_offset_bytes.data(), (size_t)sz);
    gc->numActiveTileAnimations = sz;
    do {
        n5 = 0;
        for (n4 = 0; n4 < gc->numActiveTileAnimations - 1; ++n4) {
            if (gc->tileOffsetY[n4] <= gc->tileOffsetY[n4 + 1]) continue;
            n3 = gc->tileOffsetY[n4]; gc->tileOffsetY[n4] = gc->tileOffsetY[n4 + 1]; gc->tileOffsetY[n4 + 1] = (int8_t)n3;
            n3 = gc->tileOffsetX[n4]; gc->tileOffsetX[n4] = gc->tileOffsetX[n4 + 1]; gc->tileOffsetX[n4 + 1] = (int8_t)n3;
            n5 = 1;
        }
    } while (n5 != 0);
    do {
        n5 = 0;
        for (n4 = 1; n4 < gc->numActiveTileAnimations; ++n4) {
            if (gc->tileOffsetX[n4] <= gc->tileOffsetX[n4 - 1] || gc->tileOffsetY[n4] != gc->tileOffsetY[n4 - 1]) continue;
            n3 = gc->tileOffsetY[n4]; gc->tileOffsetY[n4] = gc->tileOffsetY[n4 - 1]; gc->tileOffsetY[n4 - 1] = (int8_t)n3;
            n3 = gc->tileOffsetX[n4]; gc->tileOffsetX[n4] = gc->tileOffsetX[n4 - 1]; gc->tileOffsetX[n4 - 1] = (int8_t)n3;
            n5 = 1;
        }
    } while (n5 != 0);
}

// void setCanvasSize(int n, int n2)
void gc_setCanvasSize(GameCanvas *gc, int32_t n, int32_t n2) {
    if (gc->eD != n || gc->aJ != n2) {
        gcs_screenWidth = n;
        gcs_hs = n2;
        gc->backbuffer = Image::create_blank(gcs_screenWidth, gcs_hs);
        gc->backbuffer_surface = gc->backbuffer->drawing_surface();
        gcs_gameplayViewportHeight = gcs_hs - 30;
        gcs_bottomBarY = gcs_hs - 15;
        gc_k(gc, n, n2);
        gc->eD = n;
        gc->aJ = n2;
        gc->menuDirty = true;
        gc->lastViewportCenterX = -1;
        if (gc->gameState == 20 && !gc->ready) {
            if (gc->hx == 30) { gc->pendingTransitionCode = gc->transitionCode; gc->gameState = 30; }
            if (gc->hx == 40) { gc->pendingTransitionCode = gc->transitionCode; gc->gameState = 40; }
        }
    }
}

// public void setState(int n)
void gc_setState(GameCanvas *gc, int32_t n) {
    gc->hx = gc->gameState;
    if (!gc->ready) gc->cycle_flag = NULL;
    gc->snapshotLoadedFlag = false;
    gc->menuDirty = true;
    switch (n) {
        case 0:
            gc_openSpriteStream(gc, "sprite_images.bin");
            gc_loadPackedSpriteData(gc, "packed_sprites.bin");
            gc->loadingTimer = 0;
            /* fall through */
        case 1:
        case 2:
            gc->cycle_flag = gc_decodePngFromSpriteStream(gc);
            gc->loadingStartTime = midp_current_time_millis();
            gc->loadingTimeout = false;
            gc->loadingTimer = 0;
            break;
        case 4: {
            ResourceReader in = resources::open("sprite_image_map.bin");
            if (in) {
                int n2 = in.read_byte();
                int n3 = in.read_byte();
                gc->spriteBitmapTable.resize(n2, 0);
                gc->spriteIndexTable.assign(static_cast<std::size_t>(n3 + 1), 0);
                for (int i = 0; i < n2; ++i) {
                    int n4 = in.read_byte();
                    int n5 = in.read_byte();
                    gc->spriteBitmapTable[i].assign(static_cast<std::size_t>(n5), 0);
                    gc->spriteIndexTable[n4] = (int8_t)i;
                    for (int j = 0; j < n5; ++j)
                        gc->spriteBitmapTable[i][j] = (int8_t)in.read_byte();
                }
            }
            break;
        }
        case 10: {
            gc->spriteLoadCounter = 0;
            ResourceReader in = resources::open("tile_sprite_ids.bin");
            if (in) {
                gc->scenarioResourceCount = in.read_byte();
                for (int i = 0; i < gc->scenarioResourceCount; ++i) {
                    int n6 = in.read_byte();
                    gc->tileSpriteIndex[n6] = (int8_t)(i - 8);
                }
            }
            gc->scenarioResourceCount -= 8;
            gc->tile_sprites.clear();
            gc->tile_sprites.resize(
                static_cast<std::size_t>(gc->scenarioResourceCount));
            gc->tileSpriteIndex[68] = gc->tileSpriteIndex[66];
            break;
        }
        case 20:
            gc_resetOverlayRenderState(gc);
            if (!gc->useDefaultOverlaySelection) gc->cursorDragY = 2;
            gc->useDefaultOverlaySelection = true;
            break;
        case 25:
            gc->loadStep = 0;
            break;
        case 30:
            gc->backbuffer = NULL;
            gc->backbuffer = Image::create_blank(gcs_screenWidth, gcs_hs);
            gc->backbuffer_surface = gc->backbuffer->drawing_surface();
            gc->lastViewportCenterX = -1;
            gc->menuDirty = true;
            break;
        case 35: gc->loadStep = 4; gc->confirmToggle = 0; break;
        case 45: gc->loadStep = 4; break;
        case 55: gc->loadStep = 4; break;
        case 65: gc->loadStep = 4; break;
        case 105: gc->loadStep = 4; break;
        case 120: gc->loadStep = 4; break;
    }
    gc->gameState = n;
}

// public void start(GameMIDlet gameMIDlet) -- host owns the loop, no thread
void gc_start(GameCanvas *gc, void *gameMIDlet) {
    gc->midlet = gameMIDlet;
    gc_persistSnapshotMode1(gc);
    gc_setState(gc, 0);
    // Display.setCurrent + Thread(this).start() are replaced by the host loop.
}

// public void run() -- ONE iteration of the original game-thread loop.
// The host (main.c) calls this each frame; timing/repaint/sleep are host-owned.
void gc_run(GameCanvas *gc) {
    if (gc->stopRequested || gc->paused) return;
    gc->frameStartTime = midp_current_time_millis();
    if (gc->keyRepeatDelay == 0) {
        if (gc->heldKey != 0) gc->currentKey = gc->heldKey;
    } else {
        --gc->keyRepeatDelay;
    }
    if (gc->currentKey != 0 && gc->currentKey != -999) gc_handleKey(gc, gc->currentKey);
    if (gc->currentKey == -999) gc_resetKeyState(gc, gc->currentKey);
    if (gc->inputSubState > 0) --gc->inputSubState;
    ++gc->frameCount;
    if (gc->inputPending) {
        gc->frameIndex = 1;
        while (gc->frameIndex > 0) {
            gc_update(gc);
            if (gc->gameState != 20 && gc->gameState != 40) continue;
            gc->inputPending = false;
            break;
        }
        gc->frameIndex = 0;
    }
    if (gc->gameState == 10) {
        for (int n = 0; n < 5; ++n) gc_update(gc);
    }
    if (gc->gameState == 30) {
        for (int n = 0; n < gc->simSpeed; ++n) {
            if (!gc->pauseFlag) gc_update(gc);
            if (!gc->menuRowEnabled[gc->buildMenuMode][gc->buildMenuItemIndex]) continue;
            gc_viewportZoomKey(gc);
        }
    } else {
        gc_update(gc);
    }
    gc->frameTime = midp_current_time_millis() - gc->frameStartTime;
}
