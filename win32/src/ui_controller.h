// UiController: the menu, dialog, and text-entry state that drives the on-screen
// interface (see game/menu_*.cpp, game/dialog_data.cpp, game/gameplay_ui.cpp).
// It holds UI state; drawing lives in the renderer.
#ifndef UI_CONTROLLER_H
#define UI_CONTROLLER_H
#include "game_constants.h"
#include "support/grid.h"
#include <cstdint>

struct Economy;
struct Scenario;

// UiController owns interface selection and screen-flow state. Saved offer
// records live in OfferBoard.
struct UiController {
    int32_t advance_key_repeat();
    int32_t report_graph_value(int32_t sprite_id, int32_t index) const;
    void set_year_graph_mode(const Economy& economy, int32_t mode,
                             bool alternate);
    void reset_key_state(int32_t key);
    void cycle_build_menu_index(bool forward);
    void init_build_menu_row(int32_t row, int32_t count);
    void add_build_menu_item(int32_t text_sprite_code, int32_t menu_item_id,
                             int32_t confirm_result_code, bool wide_item,
                             bool item_enabled, int32_t cursor_action_code);
    void init_build_menu(Scenario& scenario, int32_t scenario_bits);

    bool monthlyReportEnabled = true;
    bool annualReportEnabled = true;

    // Build-menu navigation and content state. The grids hold the current menu's
    // item ids, result codes, item/text/transition codes, and per-row enabled
    // flags; the scalar cursors and mode indices track the selection; the
    // enabled-row list and count drive row navigation. All runtime-only (not
    // saved).
    int32_t buildMenuItemIndex = 0;
    int32_t buildMenuMode = 0;
    Grid<int8_t> buildMenuItems;
    Grid<int8_t> buildMenuResultCodes;
    Grid<int8_t> menuItemCode;
    Grid<int8_t> menuTextIndex;
    Grid<int8_t> menuTransitionCode;
    Grid<bool> menuRowEnabled;
    int32_t buildMenuItemCursor = 0;
    int32_t buildMenuModeIndex = 0;
    int32_t buildMenuItemCode = 0;
    bool blockBuildMenu = false;
    bool menuDirty = true;
    int32_t selectedMenuRowX = 0;
    int32_t menuRowEnabledCount = 0;
    int8_t enabledMenuRowIndexes[kEnabledMenuRowCount]{};

    // Confirm-dialog and screen-transition/overlay state. The confirm fields
    // drive the yes/no bar; transitionCode / pendingTransitionCode / the debounce
    // pick the overlay sprite and defer it; screenParam parameterises the current
    // screen; the flags track the pause menu, default overlay selection, and
    // whether a snapshot was loaded. All runtime-only (not saved).
    int32_t confirmToggle = 0;
    int32_t confirmCounter = 0;
    int32_t confirmTargetState = -1;
    int32_t pendingTransitionCode = menu_transition::NoPending;
    int32_t transitionCode = menu_transition::Initial;
    int8_t transitionDebounce = 0;
    int32_t screenParam = 0;
    bool pauseMenuOpen = false;
    bool useDefaultOverlaySelection = true;
    bool snapshotLoadedFlag = false;

    // Report-graph display state and the money ticker. graphSeries and
    // graphUsesDecadeData pick the data set; graphYearLabels and
    // graphPeriodLabels hold the axis labels; graphBarHeights holds the plotted
    // bars; moneyTickerFrames animates the header money change. All runtime-only
    // (not saved).
    int32_t graphSeries = 0;
    bool graphUsesDecadeData = false;
    int8_t graphYearLabels[kHistoryCount]{};
    int8_t graphPeriodLabels[kHistoryCount]{};
    int8_t graphBarHeights[kHistoryCount]{};
    int8_t moneyTickerFrames[money_ticker::kSlotCount]{};

    // On-map train/vehicle/city selection state. numSelectedTrains and the
    // trainSelection screen-position lists track the trains picked under the
    // cursor; cityListIndex is the highlighted city; the two menu-select flags
    // pick the vehicle/train sub-menu. cityListIndex, numSelectedTrains, and the
    // three trainSelection lists are saved and keep their exact byte order.
    int8_t numSelectedTrains = 0;
    int16_t trainSelectionIndex[kTrainSelectionCount]{};
    int16_t trainSelectionY[kTrainSelectionCount]{};
    int16_t trainSelectionX[kTrainSelectionCount]{};
    int8_t cityListIndex = 0;
    bool vehicleMenuSelectIndex = false;
    bool trainMenuSelectIndex = false;

    // Cursor state: zoom scale, drag origin, current selection cell, whether the
    // cursor is over the UI or may zoom out, the screen pixel position, and the
    // map tile under the cursor. All runtime-only.
    int32_t cursorScale = 0;
    int32_t cursorDragX = -1;
    int32_t cursorDragY = -1;
    int32_t cursorSelectX = -1;
    int32_t cursorSelectY = -1;
    bool cursorOverUI = false;
    bool cursorAllowZoomOut = false;
    int32_t cursorScreenX = 0;
    int32_t cursorScreenY = 0;
    int32_t tileCursorX = 0;
    int32_t tileCursorY = 0;
    // Pointer (mouse) state: true while the mouse hover sits over a valid tile on
    // the gameplay field this frame. Gates the mouse cursor arrow draw and tells
    // pointerPressed that a click lands on the map rather than a control element.
    bool mouseOverField = false;

    // Key input state: the pending/pause flags, the key-repeat timing, the held
    // and current keys, and the input/key sub-state used by the dispatcher. All
    // runtime-only.
    bool inputPending = false;
    bool pauseFlag = false;
    int32_t keyRepeatDelay = 0;
    int32_t heldKey = input_key::NoKey;
    int32_t inputSubState = 0;
    int32_t currentKey = input_key::NoKey;
    int32_t keySubState = 0;
    int32_t lastKeyCode = input_key::NoKey;
    int32_t keyRepeatStep = 3;
};

#endif  // UI_CONTROLLER_H
