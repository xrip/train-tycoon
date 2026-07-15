// midp_util.cpp - java.util.Vector, array lengths, and timing.
#include "midp.h"
#include <stdlib.h>

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
