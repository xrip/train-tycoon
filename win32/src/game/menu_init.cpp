// Build-menu tables for a scenario.
#include "game_internal.h"

void UiController::init_build_menu_row(int32_t n, int32_t n2) {
    const auto row_len = static_cast<std::size_t>(n2);
    buildMenuItems[n].assign(row_len, 0);
    buildMenuResultCodes[n].assign(row_len, 0);
    menuItemCode[n].assign(row_len, 0);
    menuTextIndex[n].assign(row_len, 0);
    menuTransitionCode[n].assign(row_len, 0);
    menuRowEnabled[n].assign(row_len, false);
    buildMenuItemCursor = 0;
    buildMenuModeIndex = n;
    buildMenuItemCode = 0;
}

void UiController::add_build_menu_item(int32_t n, int32_t n2, int32_t n3,
                                       bool bl, bool bl2, int32_t n4) {
    int mi = buildMenuModeIndex, c = buildMenuItemCursor;
    buildMenuItems[mi][c] = (int8_t)n2;
    buildMenuResultCodes[mi][c] = (int8_t)n3;
    menuItemCode[mi][c] = (int8_t)buildMenuItemCode;
    menuTextIndex[mi][c] = (int8_t)n;
    menuTransitionCode[mi][c] = (int8_t)n4;
    menuRowEnabled[mi][c] = bl2;
    buildMenuItemCode += 15 + (bl ? 2 : 0);
    ++buildMenuItemCursor;
}

void UiController::init_build_menu(Scenario& scenario, int32_t n) {
    if (n != -1) scenario.scenarioBits = (int8_t)n;
    buildMenuItems.resize(6, 0);
    buildMenuResultCodes.resize(6, 0);
    menuItemCode.resize(6, 0);
    menuTextIndex.resize(6, 0);
    menuTransitionCode.resize(6, 0);
    menuRowEnabled.resize(6, 0);
    int n2 = 5;
    if ((scenario.scenarioBits & 1) == 1) ++n2;
    if ((scenario.scenarioBits & 2) == 2) ++n2;
    if ((scenario.scenarioBits & 4) == 4) ++n2;
    if (scenario.scenarioStep >= 2) ++n2;
    init_build_menu_row(0, n2);
    add_build_menu_item(202, 0, 0, true, true, 10);
    add_build_menu_item(205, 1, 3, true, false, 0);
    if ((scenario.scenarioBits & 1) == 1) add_build_menu_item(208, 2, 25, false, false, 0);
    if ((scenario.scenarioBits & 2) == 2) add_build_menu_item(218, 3, 26, false, false, 0);
    if (scenario.scenarioStep >= 2) add_build_menu_item(207, 5, 28, (scenario.scenarioBits & 4) != 4, false, 0);
    if ((scenario.scenarioBits & 4) == 4) add_build_menu_item(206, 4, 27, true, false, 0);
    add_build_menu_item(236, 11, 31, false, false, 0);
    add_build_menu_item(234, 8, 29, true, false, 0);
    add_build_menu_item(210, 9, 5, true, false, 0);
    n2 = 4;
    if ((scenario.scenarioBits & 8) == 8) ++n2;
    if ((scenario.scenarioBits & 0x10) == 16) ++n2;
    if ((scenario.scenarioBits & 0x20) == 32) n2 += 2;
    init_build_menu_row(1, n2);
    if ((scenario.scenarioBits & 8) == 8) add_build_menu_item(211, 0, 8, true, true, 8);
    add_build_menu_item(205, 1, 3, false, false, 0);
    if ((scenario.scenarioBits & 0x10) == 16) add_build_menu_item(216, 2, 13, false, true, 11);
    add_build_menu_item(217, 3, 15, true, false, 0);
    add_build_menu_item(219, 4, 14, true, true, 9);
    if ((scenario.scenarioBits & 0x20) == 32) add_build_menu_item(203, 5, 2, false, true, 0);
    if ((scenario.scenarioBits & 0x20) == 32) add_build_menu_item(204, 6, 1, true, true, 0);
    add_build_menu_item(201, 9, 17, true, false, 0);
    init_build_menu_row(2, 2);
    add_build_menu_item(225, 0, 18, true, true, 0);
    add_build_menu_item(226, 9, 19, true, true, 0);
    n2 = 3;
    if ((scenario.scenarioBits & 0x40) == 64) n2 += 2;
    init_build_menu_row(3, n2);
    add_build_menu_item(230, 0, 20, false, true, 1);
    add_build_menu_item(231, 1, 21, true, true, 1);
    if ((scenario.scenarioBits & 0x40) == 64) add_build_menu_item(232, 2, 22, false, true, 6);
    if ((scenario.scenarioBits & 0x40) == 64) add_build_menu_item(233, 3, 23, true, true, 7);
    add_build_menu_item(201, 9, 17, true, false, 0);
    init_build_menu_row(4, 2);
    add_build_menu_item(211, 0, 24, true, true, 8);
    add_build_menu_item(226, 9, 19, true, false, 0);
    init_build_menu_row(5, 5);
    add_build_menu_item(212, 0, 9, false, true, 2);
    add_build_menu_item(213, 1, 10, true, true, 3);
    add_build_menu_item(214, 2, 11, false, true, 4);
    add_build_menu_item(215, 3, 12, true, true, 5);
    add_build_menu_item(201, 9, 17, true, false, 0);
}
