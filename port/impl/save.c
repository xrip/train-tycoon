// port/impl/save.c - save-buffer read/write primitives (hand-ported).
#include "game_methods.h"

// private int readShortFromBuffer()
int32_t gc_readShortFromBuffer(GameCanvas *gc) {
    return gc->saveBuffer[gc->saveReadOffset] >= 0
               ? gc->saveBuffer[gc->saveReadOffset++]
               : 256 + gc->saveBuffer[gc->saveReadOffset++];
}

// boolean saveOrLoadBool(boolean bl)
bool gc_saveOrLoadBool(GameCanvas *gc, bool bl) {
    return gc_saveOrLoadByte(gc, (int8_t)(bl ? 1 : 0)) == 1;
}

// byte saveOrLoadByte(byte by)
int8_t gc_saveOrLoadByte(GameCanvas *gc, int8_t by) {
    if (gc->savePendingMode == 2) return 0;
    int8_t by2 = by;
    if (gc->savePendingMode == 0) {
        gc_ensureSaveBufferCapacity(gc, 1);
        gc->saveBuffer[gc->saveReadOffset++] = by;
    } else if (gc->snapshotLoaded) {
        by2 = gc->saveBuffer[gc->saveReadOffset++];
    }
    return by2;
}

// int saveIntToBuffer(int n)
int32_t gc_saveIntToBuffer(GameCanvas *gc, int32_t n) {
    if (gc->savePendingMode == 2) return 0;
    int32_t n2 = n;
    if (gc->savePendingMode == 0) {
        gc_ensureSaveBufferCapacity(gc, 4);
        gc->saveBuffer[gc->saveReadOffset++] = (int8_t)(n & 0xFF);
        gc->saveBuffer[gc->saveReadOffset++] = (int8_t)(n >> 8 & 0xFF);
        gc->saveBuffer[gc->saveReadOffset++] = (int8_t)(n >> 16 & 0xFF);
        gc->saveBuffer[gc->saveReadOffset++] = (int8_t)(n >> 24 & 0xFF);
    } else if (gc->snapshotLoaded) {
        n2 = gc_readShortFromBuffer(gc) + 256 * gc_readShortFromBuffer(gc)
             + 65536 * gc_readShortFromBuffer(gc)
             + 0x1000000 * gc_readShortFromBuffer(gc);
    }
    return n2;
}
