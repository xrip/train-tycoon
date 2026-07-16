// UiController: the menu, dialog, and text-entry state that drives the on-screen
// interface (see game/menu_*.cpp, game/dialog_data.cpp, game/gameplay_ui.cpp).
// It holds UI state; drawing lives in the renderer.
#ifndef UI_CONTROLLER_H
#define UI_CONTROLLER_H
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
};

#endif  // UI_CONTROLLER_H
