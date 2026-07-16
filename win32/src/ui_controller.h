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
};

#endif  // UI_CONTROLLER_H
