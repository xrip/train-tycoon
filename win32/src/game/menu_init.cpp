// Build-menu tables for a scenario.
#include "game_methods.h"

void game_initBuildMenuRow(Game *game, int32_t n, int32_t n2) {
    const auto row_len = static_cast<std::size_t>(n2);
    game->ui.buildMenuItems[n].assign(row_len, 0);
    game->ui.buildMenuResultCodes[n].assign(row_len, 0);
    game->ui.menuItemCode[n].assign(row_len, 0);
    game->ui.menuTextIndex[n].assign(row_len, 0);
    game->ui.menuTransitionCode[n].assign(row_len, 0);
    game->ui.menuRowEnabled[n].assign(row_len, false);
    game->ui.buildMenuItemCursor = 0;
    game->ui.buildMenuModeIndex = n;
    game->ui.buildMenuItemCode = 0;
}

void game_addMenuItemInlined(Game *game, int32_t n, int32_t n2, int32_t n3,
                          bool bl, bool bl2, int32_t n4) {
    int mi = game->ui.buildMenuModeIndex, c = game->ui.buildMenuItemCursor;
    game->ui.buildMenuItems[mi][c] = (int8_t)n2;
    game->ui.buildMenuResultCodes[mi][c] = (int8_t)n3;
    game->ui.menuItemCode[mi][c] = (int8_t)game->ui.buildMenuItemCode;
    game->ui.menuTextIndex[mi][c] = (int8_t)n;
    game->ui.menuTransitionCode[mi][c] = (int8_t)n4;
    game->ui.menuRowEnabled[mi][c] = bl2;
    game->ui.buildMenuItemCode += 15 + (bl ? 2 : 0);
    ++game->ui.buildMenuItemCursor;
}

void game_initBuildMenuForScenario(Game *game, int32_t n) {
    if (n != -1) game->scenario.scenarioBits = (int8_t)n;
    game->ui.buildMenuItems.resize(6, 0);
    game->ui.buildMenuResultCodes.resize(6, 0);
    game->ui.menuItemCode.resize(6, 0);
    game->ui.menuTextIndex.resize(6, 0);
    game->ui.menuTransitionCode.resize(6, 0);
    game->ui.menuRowEnabled.resize(6, 0);
    int n2 = 5;
    if ((game->scenario.scenarioBits & 1) == 1) ++n2;
    if ((game->scenario.scenarioBits & 2) == 2) ++n2;
    if ((game->scenario.scenarioBits & 4) == 4) ++n2;
    if (game->scenario.scenarioStep >= 2) ++n2;
    game_initBuildMenuRow(game, 0, n2);
    game_addMenuItemInlined(game, 202, 0, 0, true, true, 10);
    game_addMenuItemInlined(game, 205, 1, 3, true, false, 0);
    if ((game->scenario.scenarioBits & 1) == 1) game_addMenuItemInlined(game, 208, 2, 25, false, false, 0);
    if ((game->scenario.scenarioBits & 2) == 2) game_addMenuItemInlined(game, 218, 3, 26, false, false, 0);
    if (game->scenario.scenarioStep >= 2) game_addMenuItemInlined(game, 207, 5, 28, (game->scenario.scenarioBits & 4) != 4, false, 0);
    if ((game->scenario.scenarioBits & 4) == 4) game_addMenuItemInlined(game, 206, 4, 27, true, false, 0);
    game_addMenuItemInlined(game, 236, 11, 31, false, false, 0);
    game_addMenuItemInlined(game, 234, 8, 29, true, false, 0);
    game_addMenuItemInlined(game, 210, 9, 5, true, false, 0);
    n2 = 4;
    if ((game->scenario.scenarioBits & 8) == 8) ++n2;
    if ((game->scenario.scenarioBits & 0x10) == 16) ++n2;
    if ((game->scenario.scenarioBits & 0x20) == 32) n2 += 2;
    game_initBuildMenuRow(game, 1, n2);
    if ((game->scenario.scenarioBits & 8) == 8) game_addMenuItemInlined(game, 211, 0, 8, true, true, 8);
    game_addMenuItemInlined(game, 205, 1, 3, false, false, 0);
    if ((game->scenario.scenarioBits & 0x10) == 16) game_addMenuItemInlined(game, 216, 2, 13, false, true, 11);
    game_addMenuItemInlined(game, 217, 3, 15, true, false, 0);
    game_addMenuItemInlined(game, 219, 4, 14, true, true, 9);
    if ((game->scenario.scenarioBits & 0x20) == 32) game_addMenuItemInlined(game, 203, 5, 2, false, true, 0);
    if ((game->scenario.scenarioBits & 0x20) == 32) game_addMenuItemInlined(game, 204, 6, 1, true, true, 0);
    game_addMenuItemInlined(game, 201, 9, 17, true, false, 0);
    game_initBuildMenuRow(game, 2, 2);
    game_addMenuItemInlined(game, 225, 0, 18, true, true, 0);
    game_addMenuItemInlined(game, 226, 9, 19, true, true, 0);
    n2 = 3;
    if ((game->scenario.scenarioBits & 0x40) == 64) n2 += 2;
    game_initBuildMenuRow(game, 3, n2);
    game_addMenuItemInlined(game, 230, 0, 20, false, true, 1);
    game_addMenuItemInlined(game, 231, 1, 21, true, true, 1);
    if ((game->scenario.scenarioBits & 0x40) == 64) game_addMenuItemInlined(game, 232, 2, 22, false, true, 6);
    if ((game->scenario.scenarioBits & 0x40) == 64) game_addMenuItemInlined(game, 233, 3, 23, true, true, 7);
    game_addMenuItemInlined(game, 201, 9, 17, true, false, 0);
    game_initBuildMenuRow(game, 4, 2);
    game_addMenuItemInlined(game, 211, 0, 24, true, true, 8);
    game_addMenuItemInlined(game, 226, 9, 19, true, false, 0);
    game_initBuildMenuRow(game, 5, 5);
    game_addMenuItemInlined(game, 212, 0, 9, false, true, 2);
    game_addMenuItemInlined(game, 213, 1, 10, true, true, 3);
    game_addMenuItemInlined(game, 214, 2, 11, false, true, 4);
    game_addMenuItemInlined(game, 215, 3, 12, true, true, 5);
    game_addMenuItemInlined(game, 201, 9, 17, true, false, 0);
}
