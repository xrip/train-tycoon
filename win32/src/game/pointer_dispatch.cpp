// Pointer (mouse) dispatch: a universal layer that classifies a click by the
// current screen and routes it to the existing key-action for the hit element.
//
// This is new functionality. The shipped J2ME game had empty pointer stubs (no
// touch); this port adds the mouse path. The layer never touches the keyboard,
// simulation, or draw paths, so the fixed-seed golden behavior is unaffected --
// each click resolves to the same action the equivalent keypress would produce.
//
// Three target kinds, per the current gameState:
//   - control element: Playing bottom-bar item icons, MapOverview confirm buttons
//   - menu element:    MenuOverlay selectable rows (geometry captured in render)
//   - game field:      the Playing map / MapOverview minimap (deferred: needs the
//                      isometric screen->tile inverse; tracked as the next step)
#include "game_internal.h"
#include "support/numeric.h"
#include "game_constants.h"

// Playing bottom bar: re-derive each item icon's x (the same math paint_main
// uses to draw them) and activate the clicked one. Selecting + opening in one
// click mirrors "cycle to item, then press 5/fire" -- open_build_menu reads
// buildMenuItemIndex and dispatches the item's action.
void Game::click_bottom_bar(int32_t x) {
    Game *game = this;
    const int mode = game->ui.buildMenuMode;
    const int item_count = static_cast<int>(game->ui.menuItemCode[mode].size());
    if (item_count <= 0) return;
    const int selected_x = unsigned_byte(game->ui.menuItemCode[mode][game->ui.buildMenuItemIndex]);
    int scroll = selected_x + 15 - game->renderer.canvasWidth;
    if (scroll < 0) scroll = 0;
    for (int item = 0; item < item_count; ++item) {
        const int ix = unsigned_byte(game->ui.menuItemCode[mode][item]) - scroll;
        if (x >= ix && x < ix + 14) {
            game->ui.buildMenuItemIndex = item;
            game->ui.menuDirty = true;
            // A bottom-bar click has no map-tile target. Tools that act on the
            // tile under the cursor only arm here; the player then clicks the
            // map to act on the tile they point at. Navigation/confirm items
            // still fire immediately.
            const int item_value = unsigned_byte(game->ui.buildMenuItems[mode][item]);
            if (!BuildController::tool_acts_on_cursor_tile(mode, item_value))
                game->open_build_menu();
            return;
        }
    }
}

// Mouse hover over the gameplay field. Translates the view-space mouse position
// into the map-screen space game_computeViewportBounds expects (absolute map
// pixels = mouse + viewport scroll offset) and resolves the tile under it, so the
// tile cursor, the selection diamond, and the cursor arrow all track the mouse.
// mouseOverField is cleared up front and only set again when the hover lands on a
// valid in-map tile; main clears it when the mouse leaves the window.
void Game::pointer_moved(int32_t x, int32_t y) {
    Game* game = this;
    game->ui.mouseOverField = false;
    if (game->gameState != GameState::Playing) return;
    if (y < 15 || y >= game->renderer.bottomBarY) return;  // top status strip / bottom bar = UI chrome
    int32_t anchor_x = x + game->renderer.camera.viewportCenterX;
    int32_t anchor_y = y + game->renderer.camera.viewportCenterY;
    game->compute_viewport_bounds(anchor_x, anchor_y);
    game->ui.mouseOverField = !game->ui.blockBuildMenu;
}

// Edge-scroll: with the mouse held near a gameplay-viewport edge, pan the map
// the same way holding an arrow key does. We advance the camera (viewportCenter)
// and the cursor anchor (mapScale; viewportBase = mapScale/2) together so they
// stay in lockstep. game_viewportZoomKey -- run next, inside game_run --
// re-derives viewportBase from mapScale and clamps to the map; with
// viewportBase - viewportCenter unchanged its camera-follow does not fight us,
// and the logical cursor doesn't drift off to a map edge. The visible cursor
// stays on the mouse (game_pointerMoved runs after run), so the world scrolls
// under it. Gated to match the arrow path: Playing only, and only while a
// build-menu row is enabled (game_viewportZoomKey is skipped otherwise, so the
// advance would never be clamped/processed). margin/step are tuning knobs.
void Game::pointer_edge_scroll(int32_t x, int32_t y) {
    Game* game = this;
    if (game->gameState != GameState::Playing) return;
    if (!game->ui.menuRowEnabled[game->ui.buildMenuMode][game->ui.buildMenuItemIndex]) return;
    if (y < 15 || y >= game->renderer.bottomBarY) return;  // only inside the gameplay viewport

    constexpr int32_t margin = 12;  // px from the edge that arms a pan
    constexpr int32_t step = 8;     // px/frame the camera moves (screen units)

    int32_t dx = 0;
    if (x <= margin) dx = -step;
    else if (x >= game->renderer.canvasWidth - 1 - margin) dx = step;
    int32_t dy = 0;
    const int32_t top = 15 + margin;
    const int32_t bottom = game->renderer.bottomBarY - 1 - margin;
    if (y <= top) dy = -step;
    else if (y >= bottom) dy = step;
    if (dx == 0 && dy == 0) return;

    game->renderer.camera.viewportCenterX += dx;
    game->renderer.camera.viewportCenterY += dy;
    game->renderer.camera.mapScaleX += dx * 2;  // keep viewportBase (= mapScale/2) locked to the camera
    game->renderer.camera.mapScaleY += dy * 2;
}

void Game::pointer_pressed(int32_t x, int32_t y) {
    Game* game = this;
    if (game->ui.inputPending) { game->ui.inputPending = false; return; }
    switch (game->gameState) {
        case GameState::BootSplash0: case GameState::BootSplash1:
        case GameState::BootSplash2: case GameState::BootSplash3:
        case GameState::LoadingResources:
        case GameState::GeneratingMap:
            game->loadingTimeout = true;
            return;
        case GameState::MenuOverlay: {
            const int hx = static_cast<int>(x);
            const int hy = static_cast<int>(y);
            // Control element: the top-right X closes the overlay. Same as the
            // soft-left key (*): clear the row cursor, then back out.
            if (game->clickMap.hit_close(hx, hy)) {
                game->ui.cursorDragY = -3;
                game->ui.cursorDragX = -1;
                game->handle_build_menu_state(game->renderer.currentSpriteId, -1);
                return;
            }
            // Menu element: a captured overlay row. Set the cursor to it and fire
            // its action -- identical to navigating there and pressing 5/fire.
            int row_index = 0, option_code = 0;
            if (game->clickMap.hit_menu_row(hx, hy, &row_index, &option_code)) {
                game->ui.cursorDragY = row_index;
                game->handle_build_menu_state(game->renderer.currentSpriteId, option_code);
            }
            return;
        }
        case GameState::Playing:
            // Control element: the bottom build-menu bar.
            if (y >= game->renderer.bottomBarY) {
                game->click_bottom_bar(x);
                return;
            }
            // Game field: resolve the tile under the click and fire the active
            // build tool -- the same action as pressing 5/fire on that tile.
            // (game_pointerMoved usually already resolved the hover tile; recompute
            // so a click without a prior hover still lands on the right tile.)
            if (y >= 15) {
                int32_t anchor_x = x + game->renderer.camera.viewportCenterX;
                int32_t anchor_y = y + game->renderer.camera.viewportCenterY;
                game->compute_viewport_bounds(anchor_x, anchor_y);
                if (!game->ui.blockBuildMenu) game->open_build_menu();
            }
            return;
        case GameState::MapOverview:
            // Control element: the confirm bar's two 14px buttons -- x=0 accepts
            // (return to play), x=18 exits (back to menu). Same as the 5/fire path.
            if (y >= game->renderer.bottomBarY) {
                if (x < 14) game->ui.confirmToggle = 0;
                else if (x >= 18 && x < 32) game->ui.confirmToggle = 1;
                else return;
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
            // Minimap pan-on-click is deferred with the screen->map inverse.
            return;
        default:
            return;  // Building*/Saving/Loading/Ai/Sync: transient, ignore.
    }
}
