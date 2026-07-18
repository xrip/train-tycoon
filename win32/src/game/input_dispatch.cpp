// Per-state key dispatch and
// Menu cursor movement and selection.
#include "game.h"
#include "midp_input.h"

namespace {

namespace input_direction {
enum Id : int8_t {
    DownOrRight = 1,
    UpOrLeft = 2,
    Select = 3,
};
}  // namespace input_direction

namespace key_code {
constexpr int32_t kPreviousOrBack = 42;
constexpr int32_t kNext = 35;
constexpr int32_t kMapOverview = 48;
}  // namespace key_code

namespace key_alias {
constexpr int32_t kOverviewUpper = 'A';
constexpr int32_t kOverviewLower = 'a';
constexpr int32_t kPreviousUpper = 'Y';
constexpr int32_t kPreviousLower = 'y';
constexpr int32_t kPreviousAlternate = 'Z';
constexpr int32_t kPreviousAlternateLower = 'z';
constexpr int32_t kPreviousSoftKey = -6;
constexpr int32_t kPreviousLegacySoftKey = -202;
constexpr int32_t kPreviousGameAction = 21;
constexpr int32_t kPreviousNegativeGameAction = -21;
constexpr int32_t kNextUpper = 'X';
constexpr int32_t kNextLower = 'x';
constexpr int32_t kNextSoftKey = -7;
constexpr int32_t kNextLegacySoftKey = -203;
constexpr int32_t kNextGameAction = 22;
constexpr int32_t kNextNegativeGameAction = -22;
}  // namespace key_alias

constexpr int32_t kDialogRepeatDelay = 4;
constexpr int32_t kBuildMenuInputSubState = 1;
constexpr int32_t kInitialInputSubState = 0;
constexpr int32_t kConfirmInputSubState = 40;
constexpr int32_t kConfirmCounterStart = 20;
constexpr int32_t kConfirmTargetPlaying = 30;
constexpr int32_t kConfirmTargetMenu = 17;

}  // namespace

void Game::handle_dialog_input(int8_t direction) {
    Game *game = this;
    switch (direction) {
        case input_direction::DownOrRight:
            if (game->ui.cursorSelectX != -2) { game->ui.cursorDragY = game->ui.cursorSelectX; break; }
            if (!game->ui.cursorAllowZoomOut) break;
            game->ui.cursorScale += 10;
            break;
        case input_direction::UpOrLeft:
            if (game->ui.cursorSelectY > -1) game->ui.cursorDragY = game->ui.cursorSelectY;
            else if (game->ui.cursorOverUI) game->ui.cursorScale -= 10;
            else game->ui.cursorDragY = -1;
            if (game->ui.cursorScale >= 0) break;
            game->ui.cursorScale = 0;
            break;
        case input_direction::Select:
            game->handle_build_menu_state(game->renderer.currentSpriteId, game->ui.cursorDragX);
            break;
    }
}

void Game::handle_key(int32_t key_code_value) {
    Game *game = this;
    int32_t action = 0;
    if (key_code_value == key_alias::kOverviewUpper
        || key_code_value == key_alias::kOverviewLower)
        key_code_value = key_code::kMapOverview;
    if (key_code_value == key_alias::kPreviousUpper
        || key_code_value == key_alias::kPreviousLower
        || key_code_value == key_alias::kPreviousAlternate
        || key_code_value == key_alias::kPreviousAlternateLower
        || key_code_value == key_alias::kPreviousSoftKey
        || key_code_value == key_alias::kPreviousLegacySoftKey
        || key_code_value == key_alias::kPreviousGameAction
        || key_code_value == key_alias::kPreviousNegativeGameAction)
        key_code_value = key_code::kPreviousOrBack;
    if (key_code_value == key_alias::kNextUpper
        || key_code_value == key_alias::kNextLower
        || key_code_value == key_alias::kNextSoftKey
        || key_code_value == key_alias::kNextLegacySoftKey
        || key_code_value == key_alias::kNextGameAction
        || key_code_value == key_alias::kNextNegativeGameAction)
        key_code_value = key_code::kNext;
    if ((action = midp_get_game_action(key_code_value)) == game_action::kLeft)
        key_code_value = keypad_key::kLeft;
    if (action == game_action::kRight) key_code_value = keypad_key::kRight;
    if (action == game_action::kUp) key_code_value = keypad_key::kUp;
    if (action == game_action::kDown) key_code_value = keypad_key::kDown;
    if (action == game_action::kSelect) key_code_value = keypad_key::kSelect;
    game->ui.currentKey = input_key::NoKey;
    game->ui.keyRepeatDelay = 0;
    game->ui.heldKey = input_key::NoKey;
    switch (game->gameState) {
        case GameState::BootSplash0: case GameState::BootSplash1: case GameState::BootSplash2: case GameState::BootSplash3:
            game->loadingTimeout = true;
            break;
        case GameState::MenuOverlay:
            if (key_code_value == keypad_key::kDown
                || key_code_value == keypad_key::kRight) {
                game->handle_dialog_input(input_direction::DownOrRight);
                game->ui.keyRepeatDelay = kDialogRepeatDelay;
                game->ui.heldKey = key_code_value;
            }
            if (key_code_value == keypad_key::kUp
                || key_code_value == keypad_key::kLeft) {
                game->handle_dialog_input(input_direction::UpOrLeft);
                game->ui.keyRepeatDelay = kDialogRepeatDelay;
                game->ui.heldKey = key_code_value;
            }
            if (key_code_value == keypad_key::kSelect)
                game->handle_dialog_input(input_direction::Select);
            if (!game->ui.pauseMenuOpen
                || key_code_value != key_code::kPreviousOrBack)
                break;
            game->ui.cursorDragY = -3;
            game->ui.cursorDragX = -1;
            game->handle_dialog_input(input_direction::Select);
            break;
        case GameState::Playing:
            game->ui.lastKeyCode = key_code_value;
            game->ui.keySubState = 0;
            if (game->ui.inputSubState == kInitialInputSubState) {
                if (key_code_value == key_code::kPreviousOrBack) {
                    game->ui.cycle_build_menu_index(false);
                    game->ui.inputSubState = 7;
                    game->ui.currentKey = key_code_value;
                }
                if (key_code_value == key_code::kNext) {
                    game->ui.cycle_build_menu_index(true);
                    game->ui.inputSubState = 7;
                    game->ui.currentKey = key_code_value;
                }
                if (key_code_value == keypad_key::kSelect) game->open_build_menu();
                if (key_code_value != key_code::kMapOverview
                    || game->gameState != GameState::Playing)
                    break;
                game->set_state(GameState::BuildingMinimap);
                break;
            }
            if (game->ui.inputSubState == kBuildMenuInputSubState) {
                if (key_code_value == key_code::kPreviousOrBack
                    && game->ui.buildMenuMode != build_menu::PathTarget
                    && game->ui.buildMenuMode != build_menu::TrackBuildConfirm) {
                    game->ui.buildMenuMode = build_menu::Toolbar;
                    game->ui.buildMenuItemIndex = 0;
                    game->ui.menuDirty = true;
                }
                if (key_code_value != key_code::kNext) break;
                if (game->ui.buildMenuMode == build_menu::RailTools) {
                    game->ui.buildMenuMode = build_menu::Toolbar;
                    game->ui.buildMenuItemIndex = 1;
                }
                if (game->ui.buildMenuMode == build_menu::StationTools) {
                    game->ui.buildMenuMode = build_menu::RailTools;
                    game->ui.buildMenuItemIndex = 3;
                }
                if (game->ui.buildMenuMode == build_menu::TrackPieceSelection) {
                    game->ui.buildMenuMode = build_menu::RailTools;
                    game->ui.buildMenuItemIndex = 1;
                }
                if (game->ui.buildMenuMode == build_menu::RoadTools) {
                    game->ui.buildMenuMode = build_menu::Toolbar;
                    game->ui.buildMenuItemIndex = 2;
                }
                game->ui.menuDirty = true;
                break;
            }
            if (key_code_value != key_code::kPreviousOrBack
                && key_code_value != key_code::kNext)
                break;
            game->ui.currentKey = key_code_value;
            break;
        case GameState::MapOverview:
            game->ui.lastKeyCode = key_code_value;
            game->ui.keySubState = kConfirmInputSubState;
            if (key_code_value == keypad_key::kSelect) {
                if (game->ui.confirmToggle == 0) {
                    game->set_state(GameState::Playing);
                    game->ui.buildMenuItemIndex = 0;
                    game->ui.menuDirty = true;
                } else {
                    game->ready = false;
                    game->ui.transitionCode = menu_screen::MapOverview;
                    game->set_state(GameState::MenuOverlay);
                }
            }
            if (key_code_value != key_code::kPreviousOrBack
                && key_code_value != key_code::kNext)
                break;
            if (game->ui.confirmToggle == 0) {
                game->ui.confirmToggle = 1;
                game->ui.confirmTargetState = kConfirmTargetPlaying;
            } else {
                game->ui.confirmToggle = 0;
                game->ui.confirmTargetState = kConfirmTargetMenu;
            }
            game->ui.menuDirty = true;
            game->ui.confirmCounter = kConfirmCounterStart;
            break;
    }
}
