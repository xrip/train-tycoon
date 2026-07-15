// port/impl/input.c - input-related GameCanvas methods (hand-ported).
#include "game_methods.h"

void gc_keyReleased(GameCanvas *gc, int32_t n) {
    (void)n;
    gc->currentKey = -999;
}

void gc_pointerPressed(GameCanvas *gc, int32_t n, int32_t n2) {
    (void)n; (void)n2;
    if (gc->inputPending) return;
}

void gc_pointerReleased(GameCanvas *gc, int32_t n, int32_t n2) {
    (void)n; (void)n2;
    if (gc->inputPending) {
        gc->inputPending = false;
        return;
    }
}

void gc_pointerDragged(GameCanvas *gc, int32_t n, int32_t n2) {
    (void)n; (void)n2;
    if (gc->inputPending) return;
}

void gc_resetKeyState(GameCanvas *gc, int32_t n) {
    (void)n;
    gc->currentKey = 0;
    gc->lastKeyCode = 0;
    gc->keySubState = 0;
    gc->keyRepeatDelay = 0;
    gc->heldKey = 0;
    gc->inputSubState = 0;
}
