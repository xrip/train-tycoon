// midp_util.c - java.util.Random (exact 48-bit LCG), java.util.Vector, timing.
//
// The map generator draws from Random.nextInt(n) in a fixed order, so the
// generator must reproduce Sun's algorithm bit-for-bit; that is why this is a
// faithful reimplementation, not rand().
#include "midp.h"
#include <stdlib.h>

#define RND_MULT 0x5DEECE66DULL
#define RND_ADD  0xBULL
#define RND_MASK ((1ULL << 48) - 1)

static int32_t rnd_next(MidpRandom *r, int bits) {
    r->seed = (r->seed * RND_MULT + RND_ADD) & RND_MASK;
    return (int32_t)(r->seed >> (48 - bits));
}

void midp_random_set_seed(MidpRandom *r, int64_t seed) {
    r->seed = ((uint64_t)seed ^ RND_MULT) & RND_MASK;
}

void midp_random_init(MidpRandom *r, int64_t seed) {
    midp_random_set_seed(r, seed);
}

int32_t midp_random_next_int(MidpRandom *r) {
    return rnd_next(r, 32);
}

int32_t midp_random_next_int_bound(MidpRandom *r, int32_t n) {
    if (n <= 0) return 0; // Java throws; game never passes n<=0
    if ((n & -n) == n)    // n is a power of two
        return (int32_t)(((int64_t)n * (int64_t)rnd_next(r, 31)) >> 31);
    int32_t bits, val;
    do {
        bits = rnd_next(r, 31);
        val = bits % n;
    } while (bits - val + (n - 1) < 0);
    return val;
}

///////////////////////////////////////////////////////////////////////////////
// java.util.Vector (pointer elements; the vector owns its backing array only).

MidpVector *midp_vector_new(void) {
    MidpVector *v = (MidpVector *)calloc(1, sizeof(MidpVector));
    return v;
}

void midp_vector_add(MidpVector *v, void *element) {
    if (v->size == v->cap) {
        int nc = v->cap ? v->cap * 2 : 8;
        void **nd = (void **)realloc(v->data, (size_t)nc * sizeof(void *));
        if (!nd) return;
        v->data = nd;
        v->cap = nc;
    }
    v->data[v->size++] = element;
}

void *midp_vector_get(const MidpVector *v, int index) {
    if (index < 0 || index >= v->size) return NULL;
    return v->data[index];
}

int midp_vector_size(const MidpVector *v) { return v->size; }

void midp_vector_remove_at(MidpVector *v, int index) {
    if (index < 0 || index >= v->size) return;
    for (int i = index; i < v->size - 1; ++i) v->data[i] = v->data[i + 1];
    v->size--;
}

void midp_vector_free(MidpVector *v) {
    if (!v) return;
    free(v->data);
    free(v);
}

///////////////////////////////////////////////////////////////////////////////
// Array length registry (open-addressed hash: pointer -> element count).

typedef struct { const void *key; int len; } ArrEntry;
static ArrEntry *g_arr;
static int g_arr_cap;
static int g_arr_count;

static size_t arr_hash(const void *p, int cap) {
    uintptr_t x = (uintptr_t)p;
    x = (x >> 4) ^ (x >> 16);
    return (size_t)(x & (uintptr_t)(cap - 1));
}

static void arr_grow(void) {
    int nc = g_arr_cap ? g_arr_cap * 2 : 1024;
    ArrEntry *na = (ArrEntry *)calloc((size_t)nc, sizeof(ArrEntry));
    for (int i = 0; i < g_arr_cap; ++i) {
        if (!g_arr[i].key) continue;
        size_t h = arr_hash(g_arr[i].key, nc);
        while (na[h].key) h = (h + 1) & (size_t)(nc - 1);
        na[h] = g_arr[i];
    }
    free(g_arr);
    g_arr = na;
    g_arr_cap = nc;
}

void midp_arr_set_len(const void *ptr, int count) {
    if (!ptr) return;
    if (g_arr_count * 2 >= g_arr_cap) arr_grow();
    size_t h = arr_hash(ptr, g_arr_cap);
    while (g_arr[h].key && g_arr[h].key != ptr) h = (h + 1) & (size_t)(g_arr_cap - 1);
    if (!g_arr[h].key) g_arr_count++;
    g_arr[h].key = ptr;
    g_arr[h].len = count;
}

int midp_arr_len(const void *ptr) {
    if (!ptr || !g_arr_cap) return 0;
    size_t h = arr_hash(ptr, g_arr_cap);
    while (g_arr[h].key) {
        if (g_arr[h].key == ptr) return g_arr[h].len;
        h = (h + 1) & (size_t)(g_arr_cap - 1);
    }
    return 0;
}

void midp_arr_forget(const void *ptr) {
    if (!ptr || !g_arr_cap) return;
    size_t h = arr_hash(ptr, g_arr_cap);
    while (g_arr[h].key) {
        if (g_arr[h].key == ptr) { g_arr[h].len = 0; return; }
        h = (h + 1) & (size_t)(g_arr_cap - 1);
    }
}

void *midp_alloc_arr(size_t elem_size, int count) {
    void *p = calloc((size_t)count, elem_size);
    midp_arr_set_len(p, count);
    return p;
}

///////////////////////////////////////////////////////////////////////////////
// java.io.ByteArrayOutputStream.

MidpByteArray *midp_bytearray_new(void) {
    return (MidpByteArray *)calloc(1, sizeof(MidpByteArray));
}

void midp_bytearray_write(MidpByteArray *b, int byte_value) {
    if (b->size == b->cap) {
        int nc = b->cap ? b->cap * 2 : 64;
        uint8_t *nd = (uint8_t *)realloc(b->data, (size_t)nc);
        if (!nd) return;
        b->data = nd;
        b->cap = nc;
    }
    b->data[b->size++] = (uint8_t)(byte_value & 0xFF);
}

int midp_bytearray_size(const MidpByteArray *b) { return b->size; }
uint8_t *midp_bytearray_data(MidpByteArray *b) { return b->data; }
void midp_bytearray_reset(MidpByteArray *b) { b->size = 0; }

void midp_bytearray_free(MidpByteArray *b) {
    if (!b) return;
    free(b->data);
    free(b);
}

///////////////////////////////////////////////////////////////////////////////
// Timing (Win32).
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

int64_t midp_current_time_millis(void) {
    return (int64_t)GetTickCount64();
}

void midp_sleep(int millis) {
    if (millis > 0) Sleep((DWORD)millis);
}
