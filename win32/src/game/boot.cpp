// Game start, resources, sprites, state changes, and save start.
#include "game_methods.h"
#include <stdlib.h>
#include <string.h>

static int read_u16_be(MidpResource *resource) {
    int high = midp_resource_read_byte(resource);
    int low = midp_resource_read_byte(resource);
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

// public void b(String string) -- load packed sprite offset/data tables
void gc_b(GameCanvas *gc, char *string) {
    MidpResource *in = midp_resource_open(string);
    if (!in) return;
    int n = midp_resource_read_byte(in);
    int n2 = midp_resource_read_byte(in);
    int n3 = read_u16_be(in);
    int n4 = 0;
    gc->spriteOffsetTable = GC_NEW(int16_t, n2 + 1);
    gc->spriteData = GC_NEW(int8_t, n3);
    for (int i = 0; i < n; ++i) {
        int n5 = midp_resource_read_byte(in);
        int n6 = read_u16_be(in);
        gc->spriteOffsetTable[n5] = (int16_t)n4;
        for (int j = 0; j < n6; ++j) {
            gc->spriteData[n4] = (int8_t)midp_resource_read_byte(in);
            ++n4;
        }
    }
    midp_resource_close(in);
}

// public Image decodePngFromSpriteStream() -- rebuild a PNG from the stream
MidpImage *gc_decodePngFromSpriteStream(GameCanvas *gc) {
    int8_t *byArray = NULL;
    int n = 0;
    int n3 = midp_resource_read_byte(gc->spriteStream);
    if (n3 == 2) {
        n = read_u16_be(gc->spriteStream);
        byArray = GC_NEW(int8_t, n);
        byArray[0] = -119; byArray[1] = 80; byArray[2] = 78; byArray[3] = 71;
        for (int n2 = 4; n2 < n; ++n2)
            byArray[n2] = (int8_t)midp_resource_read_byte(gc->spriteStream);
    }
    if (n3 == 1) {
        int n2 = read_u16_be(gc->spriteStream);
        int n5 = midp_resource_read_byte(gc->spriteStream) * 3;
        int n6 = midp_resource_read_byte(gc->spriteStream);
        n = n2 + n5 + 69 + n6 * 13;
        byArray = GC_NEW(int8_t, n);
        byArray[0] = -119; byArray[1] = 80; byArray[2] = 78; byArray[3] = 71;
        byArray[4] = 13; byArray[5] = 10; byArray[6] = 26; byArray[7] = 10;
        byArray[11] = 13; byArray[12] = 73; byArray[13] = 72; byArray[14] = 68; byArray[15] = 82;
        byArray[18] = (int8_t)midp_resource_read_byte(gc->spriteStream);
        byArray[19] = (int8_t)midp_resource_read_byte(gc->spriteStream);
        byArray[22] = (int8_t)midp_resource_read_byte(gc->spriteStream);
        byArray[23] = (int8_t)midp_resource_read_byte(gc->spriteStream);
        byArray[24] = (int8_t)midp_resource_read_byte(gc->spriteStream);
        byArray[25] = 3;
        byArray[29] = (int8_t)midp_resource_read_byte(gc->spriteStream);
        byArray[30] = (int8_t)midp_resource_read_byte(gc->spriteStream);
        byArray[31] = (int8_t)midp_resource_read_byte(gc->spriteStream);
        byArray[32] = (int8_t)midp_resource_read_byte(gc->spriteStream);
        byArray[35] = (int8_t)(n5 >> 8 & 0xFF);
        byArray[36] = (int8_t)(n5 & 0xFF);
        byArray[37] = 80; byArray[38] = 76; byArray[39] = 84; byArray[40] = 69;
        int n4;
        for (n4 = 0; n4 < n5 + 4; ++n4)
            byArray[41 + n4] = (int8_t)midp_resource_read_byte(gc->spriteStream);
        n4 = 41 + n5 + 4;
        if (n6 == 1) {
            n4 += 3;
            byArray[n4++] = 1;
            byArray[n4++] = 116; byArray[n4++] = 82; byArray[n4++] = 78; byArray[n4++] = 83;
            byArray[++n4] = 64;
            byArray[++n4] = -26;
            byArray[++n4] = -40;
            int n10 = ++n4;
            ++n4;
            byArray[n10] = 102;
        }
        n4 += 2;
        byArray[n4++] = (int8_t)(n2 >> 8 & 0xFF);
        byArray[n4++] = (int8_t)(n2 & 0xFF);
        byArray[n4++] = 73; byArray[n4++] = 68; byArray[n4++] = 65; byArray[n4++] = 84;
        for (int i = 0; i < n2 + 4; ++i)
            byArray[n4++] = (int8_t)midp_resource_read_byte(gc->spriteStream);
        n4 += 4;
        byArray[n4++] = 73; byArray[n4++] = 69; byArray[n4++] = 78; byArray[n4++] = 68;
        byArray[n4++] = -82; byArray[n4++] = 66; byArray[n4++] = 96; byArray[n4++] = -126;
    }
    if (!byArray) return NULL;
    return midp_image_create_from_png((const uint8_t *)byArray, 0, n);
}

// byte[] m(int n)
int8_t *gc_m(GameCanvas *gc, int32_t n) {
    int8_t by = gc->spriteData[n];
    int8_t *byArray = GC_NEW(int8_t, by);
    memcpy(byArray, gc->spriteData + n + 1, (size_t)by);
    return byArray;
}

// void loadGame(int n, int n2)
void gc_loadGame(GameCanvas *gc, int32_t n, int32_t n2) {
    gc->saveSlot = n;
    gc->savePendingMode = n2;
    gc->saveReadOffset = 0;
    gc->snapshotSize = 0;
    gc->snapshotLoaded = false;
    if (gc->savePendingMode == 1) {
        MidpRecordStore *rs = midp_rms_open("TTStore", true);
        if (rs) {
            int nr = midp_rms_num_records(rs);
            for (int i = 0; i < nr; ++i) {
                int id = midp_rms_record_id_at(rs, i);
                uint8_t *rec; int len;
                if (!midp_rms_get_record(rs, id, &rec, &len)) continue;
                if (len > 0 && (int8_t)rec[0] == n) {
                    gc->saveBuffer = GC_NEW(int8_t, len - 1);
                    memcpy(gc->saveBuffer, rec + 1, (size_t)(len - 1));
                    gc->snapshotSize = len - 1;
                    gc->snapshotLoaded = true;
                }
                free(rec);
            }
            midp_rms_close(rs);
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
    gc->gameStateCounter = gc_saveIntToBuffer(gc, gc->gameStateCounter);
    gc_saveOrLoadIntArrayInlined(gc, gc->scenarioScores);
    gc_saveOrLoadIntArrayInlined(gc, gc->leaderboardScores);
    gc_saveOrLoadByteArrayInlined(gc, gc->playerNameCodes);
}

// void persistSnapshotMode1()
void gc_persistSnapshotMode1(GameCanvas *gc) {
    gc_loadGame(gc, 5, 1);
    gc_writeSaveSnapshot(gc);
    gc_setMusicVolume(gc, gc->gameStateCounter);
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
    gc->tileOffsetX = GC_NEW(int8_t, sz);
    memcpy(gc->tileOffsetX, x_offset_bytes.data(), (size_t)sz);
    gc->tileOffsetY = GC_NEW(int8_t, sz);
    memcpy(gc->tileOffsetY, y_offset_bytes.data(), (size_t)sz);
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
        gc->backbuffer = midp_image_create_blank(gcs_screenWidth, gcs_hs);
        gc->backbufferG = midp_image_get_graphics(gc->backbuffer);
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
    if (!gc->ready) gc->cycleFlag = NULL;
    gc->snapshotLoadedFlag = false;
    gc->menuDirty = true;
    switch (n) {
        case 0:
            gc_loadSpriteStream(gc, (char *)"/1");
            gc_b(gc, (char *)"/4");
            gc->loadingTimer = 0;
            /* fall through */
        case 1:
        case 2:
            gc->cycleFlag = gc_decodePngFromSpriteStream(gc);
            gc->loadingStartTime = midp_current_time_millis();
            gc->loadingTimeout = false;
            gc->loadingTimer = 0;
            break;
        case 4: {
            MidpResource *in = midp_resource_open("/3");
            if (in) {
                int n2 = midp_resource_read_byte(in);
                int n3 = midp_resource_read_byte(in);
                gc->spriteBitmapTable = (int8_t **)midp_alloc_arr(sizeof(int8_t *), n2);
                gc->spriteIndexTable = GC_NEW(int8_t, n3 + 1);
                for (int i = 0; i < n2; ++i) {
                    int n4 = midp_resource_read_byte(in);
                    int n5 = midp_resource_read_byte(in);
                    gc->spriteBitmapTable[i] = GC_NEW(int8_t, n5);
                    gc->spriteIndexTable[n4] = (int8_t)i;
                    for (int j = 0; j < n5; ++j)
                        gc->spriteBitmapTable[i][j] = (int8_t)midp_resource_read_byte(in);
                }
                midp_resource_close(in);
            }
            break;
        }
        case 10: {
            gc->spriteLoadCounter = 0;
            MidpResource *in = midp_resource_open("/2");
            if (in) {
                gc->scenarioResourceCount = midp_resource_read_byte(in);
                for (int i = 0; i < gc->scenarioResourceCount; ++i) {
                    int n6 = midp_resource_read_byte(in);
                    gc->tileSpriteIndex[n6] = (int8_t)(i - 8);
                }
                midp_resource_close(in);
            }
            gc->scenarioResourceCount -= 8;
            gc->tileSprites = (MidpImage **)midp_alloc_arr(sizeof(MidpImage *), gc->scenarioResourceCount);
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
            gc->backbuffer = midp_image_create_blank(gcs_screenWidth, gcs_hs);
            gc->backbufferG = midp_image_get_graphics(gc->backbuffer);
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
