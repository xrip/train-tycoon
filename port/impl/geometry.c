// port/impl/geometry.c - hand-ported GameCanvas methods (you own and edit this).
//
// Each function here is a faithful port of the same-named Java method. When a
// method is ported, set its status to "ported" in tools/port_manifest.json
// (or `python tools/port.py mark <name>`), then `gen` stops emitting its stub.
#include "game_methods.h"

// public int byteToUnsignedInt(byte n)
int32_t gc_byteToUnsignedInt(GameCanvas *gc, int8_t n) {
    (void)gc;
    return n < 0 ? 256 + n : n;
}

// int crossProduct2D(int,int,int,int,int,int) -- 2D cross product for iso picking
int32_t gc_crossProduct2D(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3,
                          int32_t n4, int32_t n5, int32_t n6) {
    (void)gc;
    return (n3 - n) * (n2 - n6) - (n4 - n2) * (n - n5);
}
