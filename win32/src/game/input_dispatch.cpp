// Per-state key dispatch and
// Menu cursor movement and selection.
#include "game_internal.h"
#include "midp_input.h"

// Direction: 1=down/right, 2=up/left, 3=select.
void Game::handle_dialog_input(int8_t by) {
    Game *game = this;
    switch (by) {
        case 1:
            if (game->ui.cursorSelectX != -2) { game->ui.cursorDragY = game->ui.cursorSelectX; break; }
            if (!game->ui.cursorAllowZoomOut) break;
            game->ui.cursorScale += 10;
            break;
        case 2:
            if (game->ui.cursorSelectY > -1) game->ui.cursorDragY = game->ui.cursorSelectY;
            else if (game->ui.cursorOverUI) game->ui.cursorScale -= 10;
            else game->ui.cursorDragY = -1;
            if (game->ui.cursorScale >= 0) break;
            game->ui.cursorScale = 0;
            break;
        case 3:
            game_handleBuildMenuState(game, game->renderer.currentSpriteId, game->ui.cursorDragX);
            break;
    }
}

void Game::handle_key(int32_t n) {
    Game *game = this;
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
    game->ui.currentKey = 0;
    game->ui.keyRepeatDelay = 0;
    game->ui.heldKey = 0;
    switch (game->gameState) {
        case GameState::BootSplash0: case GameState::BootSplash1: case GameState::BootSplash2: case GameState::BootSplash3:
            game->loadingTimeout = true;
            break;
        case GameState::MenuOverlay:
            if (n == 56 || n == 54) {
                game->handle_dialog_input(1);
                game->ui.keyRepeatDelay = 4;
                game->ui.heldKey = n;
            }
            if (n == 50 || n == 52) {
                game->handle_dialog_input(2);
                game->ui.keyRepeatDelay = 4;
                game->ui.heldKey = n;
            }
            if (n == 53) game->handle_dialog_input(3);
            if (!game->ui.pauseMenuOpen || n != 42) break;
            game->ui.cursorDragY = -3;
            game->ui.cursorDragX = -1;
            game->handle_dialog_input(3);
            break;
        case GameState::Playing:
            game->ui.lastKeyCode = n;
            game->ui.keySubState = 0;
            if (game->ui.inputSubState == 0) {
                if (n == 42) { game->ui.cycle_build_menu_index(false); game->ui.inputSubState = 7; game->ui.currentKey = n; }
                if (n == 35) { game->ui.cycle_build_menu_index(true); game->ui.inputSubState = 7; game->ui.currentKey = n; }
                if (n == 53) game->open_build_menu();
                if (n != 48 || game->gameState != GameState::Playing) break;
                game->set_state(GameState::BuildingMinimap);
                break;
            }
            if (game->ui.inputSubState == 1) {
                if (n == 42 && game->ui.buildMenuMode != 4 && game->ui.buildMenuMode != 2) {
                    game->ui.buildMenuMode = 0;
                    game->ui.buildMenuItemIndex = 0;
                    game->ui.menuDirty = true;
                }
                if (n != 35) break;
                if (game->ui.buildMenuMode == 1) { game->ui.buildMenuMode = 0; game->ui.buildMenuItemIndex = 1; }
                if (game->ui.buildMenuMode == 3) { game->ui.buildMenuMode = 1; game->ui.buildMenuItemIndex = 3; }
                if (game->ui.buildMenuMode == 5) { game->ui.buildMenuMode = 1; game->ui.buildMenuItemIndex = 1; }
                game->ui.menuDirty = true;
                break;
            }
            if (n != 42 && n != 35) break;
            game->ui.currentKey = n;
            break;
        case GameState::MapOverview:
            game->ui.lastKeyCode = n;
            game->ui.keySubState = 40;
            if (n == 53) {
                if (game->ui.confirmToggle == 0) {
                    game->set_state(GameState::Playing);
                    game->ui.buildMenuItemIndex = 0;
                    game->ui.menuDirty = true;
                } else {
                    game->ready = false;
                    game->ui.transitionCode = 36;
                    game->set_state(GameState::MenuOverlay);
                }
            }
            if (n != 42 && n != 35) break;
            if (game->ui.confirmToggle == 0) {
                game->ui.confirmToggle = 1;
                game->ui.confirmTargetState = 30;
            } else {
                game->ui.confirmToggle = 0;
                game->ui.confirmTargetState = 17;
            }
            game->ui.menuDirty = true;
            game->ui.confirmCounter = 20;
            break;
    }
}
