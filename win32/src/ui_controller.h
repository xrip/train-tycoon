// UiController: the menu, dialog, and text-entry state that drives the on-screen
// interface (see game/menu_*.cpp, game/dialog_data.cpp, game/gameplay_ui.cpp).
// It holds UI state; drawing lives in the renderer.
#ifndef UI_CONTROLLER_H
#define UI_CONTROLLER_H
#include "support/grid.h"
#include <array>
#include <cstdint>

// UiController owns the interface state. Menu-row leaf: the up-to-six rows of the
// build/offer menu. Each row carries a type, three params, a cost/value/revenue,
// an alt type, an era, and the placement date (placeTrain day/month/year). All
// twelve arrays are saved; their exact save byte order is unchanged.
struct UiController {
    std::array<int8_t, 6> menuRowType{};
    std::array<int8_t, 6> placeTrainDay{};
    std::array<int8_t, 6> menuRowParamA{};
    std::array<int8_t, 6> placeTrainMonth{};
    std::array<int8_t, 6> menuRowParamB{};
    std::array<int8_t, 6> placeTrainYear{};
    std::array<int8_t, 6> menuRowParamC{};
    std::array<int32_t, 6> menuRowCost{};
    std::array<int32_t, 6> menuRowValue{};
    std::array<int8_t, 6> menuRowAltType{};
    std::array<int8_t, 6> menuRowEra{};
    std::array<int32_t, 6> menuRowRevenue{};

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
    bool menuDirty = false;
    int32_t selectedMenuRowX = 0;
    int32_t menuRowEnabledCount = 0;
    std::array<int8_t, 5> enabledMenuRowIndexes{};

    // Confirm-dialog and screen-transition/overlay state. The confirm fields
    // drive the yes/no bar; transitionCode / pendingTransitionCode / the debounce
    // pick the overlay sprite and defer it; screenParam parameterises the current
    // screen; the flags track the pause menu, default overlay selection, and
    // whether a snapshot was loaded. All runtime-only (not saved).
    int32_t confirmToggle = 0;
    int32_t confirmCounter = 0;
    int32_t confirmTargetState = 0;
    int32_t pendingTransitionCode = 0;
    int32_t transitionCode = 0;
    int8_t transitionDebounce = 0;
    int32_t screenParam = 0;
    bool pauseMenuOpen = false;
    bool useDefaultOverlaySelection = false;
    bool snapshotLoadedFlag = false;

    // Report-graph display state and the money ticker. yearGraphMode/Alt pick the
    // graph series; yearGraphOffsets/altYearArray/yearGraphValues are the plotted
    // bar geometry; moneyTickerFrames animates the header money change. All
    // runtime-only (not saved).
    int32_t yearGraphMode = 0;
    bool yearGraphAlt = false;
    std::array<int8_t, 12> yearGraphOffsets{};
    std::array<int8_t, 12> altYearArray{};
    std::array<int8_t, 12> yearGraphValues{};
    std::array<int8_t, 3> moneyTickerFrames{};

    // On-map train/vehicle/city selection state. numSelectedTrains and the
    // trainSelection screen-position lists track the trains picked under the
    // cursor; cityListIndex is the highlighted city; the two menu-select flags
    // pick the vehicle/train sub-menu. cityListIndex, numSelectedTrains, and the
    // three trainSelection lists are saved and keep their exact byte order.
    int8_t numSelectedTrains = 0;
    std::array<int16_t, 64> trainSelectionIndex{};
    std::array<int16_t, 64> trainSelectionY{};
    std::array<int16_t, 64> trainSelectionX{};
    int8_t cityListIndex = 0;
    bool vehicleMenuSelectIndex = false;
    bool trainMenuSelectIndex = false;
};

#endif  // UI_CONTROLLER_H
