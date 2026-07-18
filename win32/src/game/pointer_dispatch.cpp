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
#include "game.h"
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
    // Overlay menus (start menu, dialogs): make the row highlight follow the
    // mouse so it is clear which row/button a click will hit. This mirrors
    // keyboard navigation -- cursorDragY is the selected row and the overlay
    // redraws its highlight from it every frame. We only move the cursor; the
    // action still fires on click (pointer_pressed). Not applied to the Playing
    // bottom bar or the MapOverview confirm bar, where "selecting" arms a tool
    // or toggles a mode rather than merely highlighting.
    if (game->gameState == GameState::MenuOverlay) {
        const int hx = static_cast<int>(x), hy = static_cast<int>(y);
        int row_index = 0, option_code = 0;
        if (game->clickMap.hit_menu_row(hx, hy, &row_index, &option_code))
            game->ui.cursorDragY = row_index;
        else if (game->clickMap.hit_close(hx, hy))
            game->ui.cursorDragY = -1;  // -1 selects the top-right close (X)
        return;
    }
    if (game->gameState != GameState::Playing) return;
    if (y < 15 || y >= game->renderer.bottomBarY) return;  // top status strip / bottom bar = UI chrome
    // render_map draws the map shifted down by the 15px top status strip
    // (drawY = ... - viewportCenterY + 15). Undo that here so the resolved tile
    // sits under the cursor instead of 15px below it.
    int32_t anchor_x = x + game->renderer.camera.viewportCenterX;
    int32_t anchor_y = y - 15 + game->renderer.camera.viewportCenterY;
    game->compute_viewport_bounds(anchor_x, anchor_y);
    game->ui.mouseOverField = !game->ui.blockBuildMenu;
}

// Right-drag map panning, Transport Tycoon style: while the right mouse button
// is held, moving the mouse drags the world under the cursor. (dx, dy) is the
// mouse motion since the last frame, in screen pixels. We grab the map -- the
// camera moves opposite the drag, so the point under the cursor follows the
// mouse (like Google Maps / a touchscreen). To keep the keyboard cursor's screen
// position steady we advance the camera (viewportCenter) and the cursor anchor
// (mapScale; viewportBase = mapScale/2) together. game_viewportZoomKey -- run
// next, inside game_run -- re-derives viewportBase from mapScale and clamps to
// the map; with viewportBase - viewportCenter unchanged its camera-follow does
// not fight us. Gated to match the arrow path: Playing only, and only while a
// build-menu row is enabled (game_viewportZoomKey is skipped otherwise, so the
// advance would never be clamped/processed).
void Game::pointer_drag_pan(int32_t dx, int32_t dy) {
    Game* game = this;
    if (game->gameState != GameState::Playing) return;
    if (!game->ui.menuRowEnabled[game->ui.buildMenuMode][game->ui.buildMenuItemIndex]) return;
    if (dx == 0 && dy == 0) return;

    const int32_t camDx = -dx;  // grab-the-map: camera moves against the drag
    const int32_t camDy = -dy;
    game->renderer.camera.viewportCenterX += camDx;
    game->renderer.camera.viewportCenterY += camDy;
    game->renderer.camera.mapScaleX += camDx * 2;  // keep viewportBase (= mapScale/2) locked to the camera
    game->renderer.camera.mapScaleY += camDy * 2;
}

// Hide the OS cursor only where the game draws its own arrow at the mouse: in
// Playing, over a valid field tile (mouseOverField), and with a build row enabled
// (draw_build_menu_row draws sprite 200 at cursorScreenX/Y only then). Over UI
// chrome, water/out-of-bounds, or menus the OS arrow stays, so the pointer is
// never invisible.
bool Game::should_hide_os_cursor() const {
    const Game* game = this;
    if (game->gameState != GameState::Playing) return false;
    if (!game->ui.mouseOverField) return false;
    return game->ui.menuRowEnabled[game->ui.buildMenuMode][game->ui.buildMenuItemIndex];
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
                int32_t anchor_y = y - 15 + game->renderer.camera.viewportCenterY;  // undo the 15px top-strip draw shift
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
