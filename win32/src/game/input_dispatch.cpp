// Per-state key dispatch and
// registerCityEntryInlined (menu cursor move / select).
#include "game_methods.h"
#include "midp_input.h"

// public void registerCityEntryInlined(byte by)  -- 1=down/right 2=up/left 3=select
void gc_registerCityEntryInlined(GameCanvas *gc, int8_t by) {
    switch (by) {
        case 1:
            if (gc->cursorSelectX != -2) { gc->cursorDragY = gc->cursorSelectX; break; }
            if (!gc->cursorAllowZoomOut) break;
            gc->cursorScale += 10;
            break;
        case 2:
            if (gc->cursorSelectY > -1) gc->cursorDragY = gc->cursorSelectY;
            else if (gc->cursorOverUI) gc->cursorScale -= 10;
            else gc->cursorDragY = -1;
            if (gc->cursorScale >= 0) break;
            gc->cursorScale = 0;
            break;
        case 3:
            gc_handleBuildMenuState(gc, gc->renderer.currentSpriteId, gc->cursorDragX);
            break;
    }
}

// void handleKey(int n)
void gc_handleKey(GameCanvas *gc, int32_t n) {
    int n2;
    if (n == 65 || n == 97) n = 48;
    if (n == 89 || n == 121) n = 42;
    if (n == 90 || n == 122) n = 42;
    if (n == 88 || n == 120) n = 35;
    if (n == -6) n = 42;
    if (n == -7) n = 35;
    if (n == -202) n = 42;
    if (n == -203) n = 35;
    if (n == 21) n = 42;
    if (n == 22) n = 35;
    if (n == -21) n = 42;
    if (n == -22) n = 35;
    if ((n2 = midp_get_game_action(n)) == 2) n = 52;
    if (n2 == 5) n = 54;
    if (n2 == 1) n = 50;
    if (n2 == 6) n = 56;
    if (n2 == 8) n = 53;
    gc->currentKey = 0;
    gc->keyRepeatDelay = 0;
    gc->heldKey = 0;
    switch (gc->gameState) {
        case +GameState::BootSplash0: case +GameState::BootSplash1: case +GameState::BootSplash2: case +GameState::BootSplash3:
            gc->loadingTimeout = true;
            break;
        case +GameState::MenuOverlay:
            if (n == 56 || n == 54) {
                gc_registerCityEntryInlined(gc, 1);
                gc->keyRepeatDelay = 4;
                gc->heldKey = n;
            }
            if (n == 50 || n == 52) {
                gc_registerCityEntryInlined(gc, 2);
                gc->keyRepeatDelay = 4;
                gc->heldKey = n;
            }
            if (n == 53) gc_registerCityEntryInlined(gc, 3);
            if (!gc->pauseMenuOpen || n != 42) break;
            gc->cursorDragY = -3;
            gc->cursorDragX = -1;
            gc_registerCityEntryInlined(gc, 3);
            break;
        case +GameState::Playing:
            gc->lastKeyCode = n;
            gc->keySubState = 0;
            if (gc->inputSubState == 0) {
                if (n == 42) { gc_cycleBuildMenuIndex(gc, false); gc->inputSubState = 7; gc->currentKey = n; }
                if (n == 35) { gc_cycleBuildMenuIndex(gc, true); gc->inputSubState = 7; gc->currentKey = n; }
                if (n == 53) gc_openBuildMenu(gc);
                if (n != 48 || gc->gameState != +GameState::Playing) break;
                gc_setState(gc, +GameState::BuildingMinimap);
                break;
            }
            if (gc->inputSubState == 1) {
                if (n == 42 && gc->ui.buildMenuMode != 4 && gc->ui.buildMenuMode != 2) {
                    gc->ui.buildMenuMode = 0;
                    gc->ui.buildMenuItemIndex = 0;
                    gc->ui.menuDirty = true;
                }
                if (n != 35) break;
                if (gc->ui.buildMenuMode == 1) { gc->ui.buildMenuMode = 0; gc->ui.buildMenuItemIndex = 1; }
                if (gc->ui.buildMenuMode == 3) { gc->ui.buildMenuMode = 1; gc->ui.buildMenuItemIndex = 3; }
                if (gc->ui.buildMenuMode == 5) { gc->ui.buildMenuMode = 1; gc->ui.buildMenuItemIndex = 1; }
                gc->ui.menuDirty = true;
                break;
            }
            if (n != 42 && n != 35) break;
            gc->currentKey = n;
            break;
        case +GameState::MapOverview:
            gc->lastKeyCode = n;
            gc->keySubState = 40;
            if (n == 53) {
                if (gc->confirmToggle == 0) {
                    gc_setState(gc, +GameState::Playing);
                    gc->ui.buildMenuItemIndex = 0;
                    gc->ui.menuDirty = true;
                } else {
                    gc->ready = false;
                    gc->transitionCode = 36;
                    gc_setState(gc, +GameState::MenuOverlay);
                }
            }
            if (n != 42 && n != 35) break;
            if (gc->confirmToggle == 0) {
                gc->confirmToggle = 1;
                gc->confirmTargetState = 30;
            } else {
                gc->confirmToggle = 0;
                gc->confirmTargetState = 17;
            }
            gc->ui.menuDirty = true;
            gc->confirmCounter = 20;
            break;
    }
}
