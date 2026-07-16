// Build-menu tables for a scenario.
#include "game_methods.h"

void gc_initBuildMenuRow(Game *gc, int32_t n, int32_t n2) {
    const auto row_len = static_cast<std::size_t>(n2);
    gc->ui.buildMenuItems[n].assign(row_len, 0);
    gc->ui.buildMenuResultCodes[n].assign(row_len, 0);
    gc->ui.menuItemCode[n].assign(row_len, 0);
    gc->ui.menuTextIndex[n].assign(row_len, 0);
    gc->ui.menuTransitionCode[n].assign(row_len, 0);
    gc->ui.menuRowEnabled[n].assign(row_len, false);
    gc->ui.buildMenuItemCursor = 0;
    gc->ui.buildMenuModeIndex = n;
    gc->ui.buildMenuItemCode = 0;
}

void gc_addMenuItemInlined(Game *gc, int32_t n, int32_t n2, int32_t n3,
                          bool bl, bool bl2, int32_t n4) {
    int mi = gc->ui.buildMenuModeIndex, c = gc->ui.buildMenuItemCursor;
    gc->ui.buildMenuItems[mi][c] = (int8_t)n2;
    gc->ui.buildMenuResultCodes[mi][c] = (int8_t)n3;
    gc->ui.menuItemCode[mi][c] = (int8_t)gc->ui.buildMenuItemCode;
    gc->ui.menuTextIndex[mi][c] = (int8_t)n;
    gc->ui.menuTransitionCode[mi][c] = (int8_t)n4;
    gc->ui.menuRowEnabled[mi][c] = bl2;
    gc->ui.buildMenuItemCode += 15 + (bl ? 2 : 0);
    ++gc->ui.buildMenuItemCursor;
}

void gc_initBuildMenuForScenario(Game *gc, int32_t n) {
    if (n != -1) gc->scenario.scenarioBits = (int8_t)n;
    gc->ui.buildMenuItems.resize(6, 0);
    gc->ui.buildMenuResultCodes.resize(6, 0);
    gc->ui.menuItemCode.resize(6, 0);
    gc->ui.menuTextIndex.resize(6, 0);
    gc->ui.menuTransitionCode.resize(6, 0);
    gc->ui.menuRowEnabled.resize(6, 0);
    int n2 = 5;
    if ((gc->scenario.scenarioBits & 1) == 1) ++n2;
    if ((gc->scenario.scenarioBits & 2) == 2) ++n2;
    if ((gc->scenario.scenarioBits & 4) == 4) ++n2;
    if (gc->scenario.scenarioStep >= 2) ++n2;
    gc_initBuildMenuRow(gc, 0, n2);
    gc_addMenuItemInlined(gc, 202, 0, 0, true, true, 10);
    gc_addMenuItemInlined(gc, 205, 1, 3, true, false, 0);
    if ((gc->scenario.scenarioBits & 1) == 1) gc_addMenuItemInlined(gc, 208, 2, 25, false, false, 0);
    if ((gc->scenario.scenarioBits & 2) == 2) gc_addMenuItemInlined(gc, 218, 3, 26, false, false, 0);
    if (gc->scenario.scenarioStep >= 2) gc_addMenuItemInlined(gc, 207, 5, 28, (gc->scenario.scenarioBits & 4) != 4, false, 0);
    if ((gc->scenario.scenarioBits & 4) == 4) gc_addMenuItemInlined(gc, 206, 4, 27, true, false, 0);
    gc_addMenuItemInlined(gc, 236, 11, 31, false, false, 0);
    gc_addMenuItemInlined(gc, 234, 8, 29, true, false, 0);
    gc_addMenuItemInlined(gc, 210, 9, 5, true, false, 0);
    n2 = 4;
    if ((gc->scenario.scenarioBits & 8) == 8) ++n2;
    if ((gc->scenario.scenarioBits & 0x10) == 16) ++n2;
    if ((gc->scenario.scenarioBits & 0x20) == 32) n2 += 2;
    gc_initBuildMenuRow(gc, 1, n2);
    if ((gc->scenario.scenarioBits & 8) == 8) gc_addMenuItemInlined(gc, 211, 0, 8, true, true, 8);
    gc_addMenuItemInlined(gc, 205, 1, 3, false, false, 0);
    if ((gc->scenario.scenarioBits & 0x10) == 16) gc_addMenuItemInlined(gc, 216, 2, 13, false, true, 11);
    gc_addMenuItemInlined(gc, 217, 3, 15, true, false, 0);
    gc_addMenuItemInlined(gc, 219, 4, 14, true, true, 9);
    if ((gc->scenario.scenarioBits & 0x20) == 32) gc_addMenuItemInlined(gc, 203, 5, 2, false, true, 0);
    if ((gc->scenario.scenarioBits & 0x20) == 32) gc_addMenuItemInlined(gc, 204, 6, 1, true, true, 0);
    gc_addMenuItemInlined(gc, 201, 9, 17, true, false, 0);
    gc_initBuildMenuRow(gc, 2, 2);
    gc_addMenuItemInlined(gc, 225, 0, 18, true, true, 0);
    gc_addMenuItemInlined(gc, 226, 9, 19, true, true, 0);
    n2 = 3;
    if ((gc->scenario.scenarioBits & 0x40) == 64) n2 += 2;
    gc_initBuildMenuRow(gc, 3, n2);
    gc_addMenuItemInlined(gc, 230, 0, 20, false, true, 1);
    gc_addMenuItemInlined(gc, 231, 1, 21, true, true, 1);
    if ((gc->scenario.scenarioBits & 0x40) == 64) gc_addMenuItemInlined(gc, 232, 2, 22, false, true, 6);
    if ((gc->scenario.scenarioBits & 0x40) == 64) gc_addMenuItemInlined(gc, 233, 3, 23, true, true, 7);
    gc_addMenuItemInlined(gc, 201, 9, 17, true, false, 0);
    gc_initBuildMenuRow(gc, 4, 2);
    gc_addMenuItemInlined(gc, 211, 0, 24, true, true, 8);
    gc_addMenuItemInlined(gc, 226, 9, 19, true, false, 0);
    gc_initBuildMenuRow(gc, 5, 5);
    gc_addMenuItemInlined(gc, 212, 0, 9, false, true, 2);
    gc_addMenuItemInlined(gc, 213, 1, 10, true, true, 3);
    gc_addMenuItemInlined(gc, 214, 2, 11, false, true, 4);
    gc_addMenuItemInlined(gc, 215, 3, 12, true, true, 5);
    gc_addMenuItemInlined(gc, 201, 9, 17, true, false, 0);
}
