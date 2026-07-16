// Save and load of game arrays and matrices.
#include "game_methods.h"
#include <string.h>
#include <vector>

void gc_ensureSaveBufferCapacity(Game *gc, int32_t n) {
    // Grow by n, preserving existing bytes and zeroing the new tail.
    gc->saveCodec.saveBuffer.resize(static_cast<std::size_t>(gc->saveCodec.snapshotSize + n));
    gc->saveCodec.snapshotSize += n;
}

// Byte array save/load over an explicit length. The std::array and Grid
// overloads forward here so the byte order stays identical.
void gc_saveOrLoadByteSpan(Game *gc, int8_t *byArray, int len) {
    if (gc->saveCodec.savePendingMode == save::MODE_SKIP) {
        for (int i = 0; i < len; ++i) byArray[i] = 0;
    } else if (gc->saveCodec.savePendingMode == save::MODE_WRITE) {
        gc_ensureSaveBufferCapacity(gc, len);
        memcpy(gc->saveCodec.saveBuffer.data() + gc->saveCodec.saveReadOffset, byArray, (size_t)len);
        gc->saveCodec.saveReadOffset += len;
    } else if (gc->saveCodec.snapshotLoaded) {
        memcpy(byArray, gc->saveCodec.saveBuffer.data() + gc->saveCodec.saveReadOffset, (size_t)len);
        gc->saveCodec.saveReadOffset += len;
    }
}

// Short array save/load over an explicit length.
void gc_saveOrLoadShortSpan(Game *gc, int16_t *sArray, int len) {
    if (gc->saveCodec.savePendingMode == save::MODE_SKIP) {
        for (int i = 0; i < len; ++i) sArray[i] = 0;
    } else if (gc->saveCodec.savePendingMode == save::MODE_WRITE) {
        gc_ensureSaveBufferCapacity(gc, 2 * len);
        for (int i = 0; i < len; ++i) {
            gc->saveCodec.saveBuffer[gc->saveCodec.saveReadOffset++] = (int8_t)(sArray[i] & 0xFF);
            gc->saveCodec.saveBuffer[gc->saveCodec.saveReadOffset++] = (int8_t)(sArray[i] >> 8 & 0xFF);
        }
    } else if (gc->saveCodec.snapshotLoaded) {
        for (int i = 0; i < len; ++i) {
            uint16_t bits = (uint16_t)gc_readSaveByteUnsigned(gc);
            bits |= (uint16_t)((uint16_t)gc_readSaveByteUnsigned(gc) << 8);
            memcpy(&sArray[i], &bits, sizeof(sArray[i]));
        }
    }
}

// Int array save/load over an explicit length.
void gc_saveOrLoadIntSpan(Game *gc, int32_t *nArray, int len) {
    if (gc->saveCodec.savePendingMode == save::MODE_SKIP) {
        for (int i = 0; i < len; ++i) nArray[i] = 0;
    } else if (gc->saveCodec.savePendingMode == save::MODE_WRITE) {
        gc_ensureSaveBufferCapacity(gc, 4 * len);
        for (int i = 0; i < len; ++i) {
            gc->saveCodec.saveBuffer[gc->saveCodec.saveReadOffset++] = (int8_t)(nArray[i] & 0xFF);
            gc->saveCodec.saveBuffer[gc->saveCodec.saveReadOffset++] = (int8_t)(nArray[i] >> 8 & 0xFF);
            gc->saveCodec.saveBuffer[gc->saveCodec.saveReadOffset++] = (int8_t)(nArray[i] >> 16 & 0xFF);
            gc->saveCodec.saveBuffer[gc->saveCodec.saveReadOffset++] = (int8_t)(nArray[i] >> 24 & 0xFF);
        }
    } else if (gc->saveCodec.snapshotLoaded) {
        for (int i = 0; i < len; ++i) {
            uint32_t bits = (uint32_t)gc_readSaveByteUnsigned(gc);
            bits |= (uint32_t)gc_readSaveByteUnsigned(gc) << 8;
            bits |= (uint32_t)gc_readSaveByteUnsigned(gc) << 16;
            bits |= (uint32_t)gc_readSaveByteUnsigned(gc) << 24;
            memcpy(&nArray[i], &bits, sizeof(nArray[i]));
        }
    }
}

bool gc_loadSnapshotAndReturn(Game *gc) {
    gc_loadGame(gc, 0, save::MODE_READ);
    return gc->saveCodec.snapshotLoaded;
}
