// Per-state key dispatch and
// registerCityEntryInlined (menu cursor move / select).
#include "game_methods.h"
#include "midp_input.h"

// public void registerCityEntryInlined(byte by)  -- 1=down/right 2=up/left 3=select
void gc_registerCityEntryInlined(Game *gc, int8_t by) {
    switch (by) {
        case 1:
            if (gc->ui.cursorSelectX != -2) { gc->ui.cursorDragY = gc->ui.cursorSelectX; break; }
            if (!gc->ui.cursorAllowZoomOut) break;
            gc->ui.cursorScale += 10;
            break;
        case 2:
            if (gc->ui.cursorSelectY > -1) gc->ui.cursorDragY = gc->ui.cursorSelectY;
            else if (gc->ui.cursorOverUI) gc->ui.cursorScale -= 10;
            else gc->ui.cursorDragY = -1;
            if (gc->ui.cursorScale >= 0) break;
            gc->ui.cursorScale = 0;
            break;
        case 3:
            gc_handleBuildMenuState(gc, gc->renderer.currentSpriteId, gc->ui.cursorDragX);
            break;
    }
}

// void handleKey(int n)
void gc_handleKey(Game *gc, int32_t n) {
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
    gc->ui.currentKey = 0;
    gc->ui.keyRepeatDelay = 0;
    gc->ui.heldKey = 0;
    switch (gc->gameState) {
        case +GameState::BootSplash0: case +GameState::BootSplash1: case +GameState::BootSplash2: case +GameState::BootSplash3:
            gc->loadingTimeout = true;
            break;
        case +GameState::MenuOverlay:
            if (n == 56 || n == 54) {
                gc_registerCityEntryInlined(gc, 1);
                gc->ui.keyRepeatDelay = 4;
                gc->ui.heldKey = n;
            }
            if (n == 50 || n == 52) {
                gc_registerCityEntryInlined(gc, 2);
                gc->ui.keyRepeatDelay = 4;
                gc->ui.heldKey = n;
            }
            if (n == 53) gc_registerCityEntryInlined(gc, 3);
            if (!gc->ui.pauseMenuOpen || n != 42) break;
            gc->ui.cursorDragY = -3;
            gc->ui.cursorDragX = -1;
            gc_registerCityEntryInlined(gc, 3);
            break;
        case +GameState::Playing:
            gc->ui.lastKeyCode = n;
            gc->ui.keySubState = 0;
            if (gc->ui.inputSubState == 0) {
                if (n == 42) { gc_cycleBuildMenuIndex(gc, false); gc->ui.inputSubState = 7; gc->ui.currentKey = n; }
                if (n == 35) { gc_cycleBuildMenuIndex(gc, true); gc->ui.inputSubState = 7; gc->ui.currentKey = n; }
                if (n == 53) gc_openBuildMenu(gc);
                if (n != 48 || gc->gameState != +GameState::Playing) break;
                gc_setState(gc, +GameState::BuildingMinimap);
                break;
            }
            if (gc->ui.inputSubState == 1) {
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
            gc->ui.currentKey = n;
            break;
        case +GameState::MapOverview:
            gc->ui.lastKeyCode = n;
            gc->ui.keySubState = 40;
            if (n == 53) {
                if (gc->ui.confirmToggle == 0) {
                    gc_setState(gc, +GameState::Playing);
                    gc->ui.buildMenuItemIndex = 0;
                    gc->ui.menuDirty = true;
                } else {
                    gc->ready = false;
                    gc->ui.transitionCode = 36;
                    gc_setState(gc, +GameState::MenuOverlay);
                }
            }
            if (n != 42 && n != 35) break;
            if (gc->ui.confirmToggle == 0) {
                gc->ui.confirmToggle = 1;
                gc->ui.confirmTargetState = 30;
            } else {
                gc->ui.confirmToggle = 0;
                gc->ui.confirmTargetState = 17;
            }
            gc->ui.menuDirty = true;
            gc->ui.confirmCounter = 20;
            break;
    }
}
