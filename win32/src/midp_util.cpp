// midp_util.cpp - java.util.Vector, array lengths, and timing.
#include "midp.h"
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////
// java.util.Vector (pointer elements; the vector owns its backing array only).

MidpVector *midp_vector_new(void) {
    MidpVector *vector = (MidpVector *)calloc(1, sizeof(MidpVector));
    return vector;
}

void midp_vector_add(MidpVector *vector, void *element) {
    if (vector->size == vector->capacity) {
        int new_capacity = vector->capacity ? vector->capacity * 2 : 8;
        void **new_data = (void **)realloc(
            vector->data, (size_t)new_capacity * sizeof(void *));
        if (!new_data) return;
        vector->data = new_data;
        vector->capacity = new_capacity;
    }
    vector->data[vector->size++] = element;
}

void *midp_vector_get(const MidpVector *vector, int index) {
    if (index < 0 || index >= vector->size) return NULL;
    return vector->data[index];
}

int midp_vector_size(const MidpVector *vector) { return vector->size; }

void midp_vector_remove_at(MidpVector *vector, int index) {
    if (index < 0 || index >= vector->size) return;
    for (int element_index = index;
         element_index < vector->size - 1; ++element_index)
        vector->data[element_index] = vector->data[element_index + 1];
    vector->size--;
}

void midp_vector_free(MidpVector *vector) {
    if (!vector) return;
    free(vector->data);
    free(vector);
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
