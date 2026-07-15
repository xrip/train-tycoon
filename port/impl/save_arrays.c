// port/impl/save_arrays.c - save/load of whole arrays + matrices (hand-ported).
#include "game_methods.h"
#include <string.h>

// void ensureSaveBufferCapacity(int n)
void gc_ensureSaveBufferCapacity(GameCanvas *gc, int32_t n) {
    if (gc->snapshotSize > 0) {
        int8_t *byArray = GC_NEW(int8_t, gc->snapshotSize + n);
        memcpy(byArray, gc->saveBuffer, (size_t)gc->snapshotSize);
        gc->snapshotSize += n;
        gc->saveBuffer = GC_NEW(int8_t, gc->snapshotSize);
        memcpy(gc->saveBuffer, byArray, (size_t)gc->snapshotSize);
    } else {
        gc->saveBuffer = GC_NEW(int8_t, n);
        gc->snapshotSize += n;
    }
}

// void saveOrLoadByteArrayInlined(byte[] byArray)
void gc_saveOrLoadByteArrayInlined(GameCanvas *gc, int8_t *byArray) {
    int len = midp_arr_len(byArray);
    if (gc->savePendingMode == 2) {
        for (int i = 0; i < len; ++i) byArray[i] = 0;
    } else if (gc->savePendingMode == 0) {
        gc_ensureSaveBufferCapacity(gc, len);
        memcpy(gc->saveBuffer + gc->saveReadOffset, byArray, (size_t)len);
        gc->saveReadOffset += len;
    } else if (gc->snapshotLoaded) {
        memcpy(byArray, gc->saveBuffer + gc->saveReadOffset, (size_t)len);
        gc->saveReadOffset += len;
    }
}

// void saveOrLoadByteMatrixInlined(byte[][] byArray)
void gc_saveOrLoadByteMatrixInlined(GameCanvas *gc, int8_t **byArray) {
    int rows = midp_arr_len(byArray);
    for (int i = 0; i < rows; ++i) gc_saveOrLoadByteArrayInlined(gc, byArray[i]);
}

// void saveOrLoadShortArrayInlined(short[] sArray)
void gc_saveOrLoadShortArrayInlined(GameCanvas *gc, int16_t *sArray) {
    int len = midp_arr_len(sArray);
    if (gc->savePendingMode == 2) {
        for (int i = 0; i < len; ++i) sArray[i] = 0;
    } else if (gc->savePendingMode == 0) {
        gc_ensureSaveBufferCapacity(gc, 2 * len);
        for (int i = 0; i < len; ++i) {
            gc->saveBuffer[gc->saveReadOffset++] = (int8_t)(sArray[i] & 0xFF);
            gc->saveBuffer[gc->saveReadOffset++] = (int8_t)(sArray[i] >> 8 & 0xFF);
        }
    } else if (gc->snapshotLoaded) {
        for (int i = 0; i < len; ++i)
            sArray[i] = (int16_t)(gc_readShortFromBuffer(gc) + 256 * gc_readShortFromBuffer(gc));
    }
}

// void saveOrLoadShortMatrixInlined(short[][] sArray)
void gc_saveOrLoadShortMatrixInlined(GameCanvas *gc, int16_t **sArray) {
    int rows = midp_arr_len(sArray);
    for (int i = 0; i < rows; ++i) gc_saveOrLoadShortArrayInlined(gc, sArray[i]);
}

// void saveOrLoadIntArrayInlined(int[] nArray)
void gc_saveOrLoadIntArrayInlined(GameCanvas *gc, int32_t *nArray) {
    int len = midp_arr_len(nArray);
    if (gc->savePendingMode == 2) {
        for (int i = 0; i < len; ++i) nArray[i] = 0;
    } else if (gc->savePendingMode == 0) {
        gc_ensureSaveBufferCapacity(gc, 4 * len);
        for (int i = 0; i < len; ++i) {
            gc->saveBuffer[gc->saveReadOffset++] = (int8_t)(nArray[i] & 0xFF);
            gc->saveBuffer[gc->saveReadOffset++] = (int8_t)(nArray[i] >> 8 & 0xFF);
            gc->saveBuffer[gc->saveReadOffset++] = (int8_t)(nArray[i] >> 16 & 0xFF);
            gc->saveBuffer[gc->saveReadOffset++] = (int8_t)(nArray[i] >> 24 & 0xFF);
        }
    } else if (gc->snapshotLoaded) {
        for (int i = 0; i < len; ++i)
            nArray[i] = gc_readShortFromBuffer(gc) + 256 * gc_readShortFromBuffer(gc)
                        + 65536 * gc_readShortFromBuffer(gc)
                        + 0x1000000 * gc_readShortFromBuffer(gc);
    }
}

// boolean loadSnapshotAndReturn()
bool gc_loadSnapshotAndReturn(GameCanvas *gc) {
    gc_loadGame(gc, 0, 1);
    return gc->snapshotLoaded;
}
