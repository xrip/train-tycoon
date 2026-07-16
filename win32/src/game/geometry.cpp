// Small geometry and input functions.
#include "game_methods.h"

// public int byteToUnsignedInt(byte n)
int32_t gc_byteToUnsignedInt(Game *gc, int8_t n) {
    (void)gc;
    return n < 0 ? 256 + n : n;
}

// int crossProduct2D(int,int,int,int,int,int) -- 2D cross product for iso picking
int32_t gc_crossProduct2D(Game *gc, int32_t n, int32_t n2, int32_t n3,
                          int32_t n4, int32_t n5, int32_t n6) {
    (void)gc;
    return (n3 - n) * (n2 - n6) - (n4 - n2) * (n - n5);
}
