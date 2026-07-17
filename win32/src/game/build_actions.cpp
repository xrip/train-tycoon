// Direct game-map build and selection actions.
#include "game_internal.h"
#include <vector>

void game_enterOutOfMoneyState(Game *game) {
    game->ready = false;
    game->ui.transitionCode = 11;
    game_setState(game, GameState::MenuOverlay);
}

void game_incrementRowValueType13(Game *game, int32_t amount) {
    for (int row = 0; row < 5; ++row)
        if (game->ui.menuRowType[row] == 13) game->ui.menuRowValue[row] += amount;
}

bool game_isAnyTrainCarAt(Game *game, int32_t car, int32_t x, int32_t y) {
    if (car == -1) return false;
    if (unsignedByte(game->fleet.trainCars[car].x) == x
        && unsignedByte(game->fleet.trainCars[car].y) == y)
        return true;
    return game_isAnyTrainCarAt(game, game->fleet.trainCars[car].nextCar, x, y);
}

int32_t game_findTrainHeadAt(Game *game, int32_t x, int32_t y) {
    for (int train = 0; train < 500; ++train) {
        if (train_flag::isUsed(game->fleet.trainCars[train].flags)
            && train_flag::isHead(game->fleet.trainCars[train].flags)
            && game_isAnyTrainCarAt(game, train, x, y))
            return train;
    }
    return 5000;
}

int32_t game_allocateServiceVehicleSlot(Game *game, int32_t type,
                                      int32_t x, int32_t y,
                                      bool horizontal, bool extended) {
    int slot = 5000;
    for (int vehicle = 0; vehicle < 250; ++vehicle) {
        if ((game->fleet.serviceSites[vehicle].flags & 0x80) == 0) {
            slot = vehicle;
            break;
        }
    }
    if (slot == 5000) return slot;

    game->fleet.serviceSites[slot].flags = (int8_t)(0x80 | (horizontal ? 0x40 : 0)
                                      | (extended ? 0x20 : 0) | type);
    game->fleet.serviceSites[slot].x = (int8_t)x;
    game->fleet.serviceSites[slot].y = (int8_t)y;
    game->fleet.serviceSites[slot].unloadFlags = 0;
    for (int resource = 0; resource < 5; ++resource)
        game->fleet.serviceSites[slot].resourceStock[resource] = 0;
    return slot;
}

void game_computeTrainCost(Game *game) {
    switch (game->build.wagonType) {
        case 0: game->build.unitCost = game->economy.gWagonCost; break;
        case 1: game->build.unitCost = game->economy.gWagonCost2; break;
        case 2: game->build.unitCost = game->economy.gWagonCost3; break;
        case 3: game->build.unitCost = game->economy.gWagonCost4; break;
        case 4: game->build.unitCost = game->economy.gWagonCost5; break;
        case 5: game->build.unitCost = game->economy.gWagonCost6; break;
    }
    if (game->build.wagonType == game->build.wagonDiscountType)
        game->build.unitCost = game->build.unitCost * game->scenario.difficultyPct / 100;

    for (int option = 0; option < 5; ++option) {
        int option_cost;
        switch (game->build.wagonOptionSlots[option]) {
            case 0: option_cost = game->economy.costGroupC1; break;
            case 1: option_cost = game->economy.costGroupC2; break;
            case 2: option_cost = game->economy.costGroupC3; break;
            case 3: option_cost = game->economy.costGroupC4; break;
            default: continue;
        }
        if (game->build.wagonOptionSlots[option] == game->build.wagonCountMatch[option])
            option_cost = option_cost * game->scenario.difficultyPct / 100;
        game->build.unitCost += option_cost;
    }
}

void game_rebuildCityList(Game *game) {
    for (int city = 0; city < 48; ++city) game->world.cityIndex[city] = 0;
    const int seen_count = 64 * game->world.mapWidth * game->world.mapHeight;
    std::vector<uint8_t> seen_city_tiles(
        static_cast<std::size_t>(seen_count));
    for (int station_x = 0; station_x < game->world.mapWidth; ++station_x) {
        for (int station_y = 0; station_y < game->world.mapHeight; ++station_y) {
            int track = game->world.trackMap[station_x][station_y];
            if ((track & 0xC0) != 192) continue;
            int city = track & 0x3F;
            int min_x = station_x - 3;
            int min_y = station_y - 3;
            int max_x = station_x + 3;
            int max_y = station_y + 3;
            if (min_x < 0) min_x = 0;
            if (min_y < 0) min_y = 0;
            if (max_x >= game->world.mapWidth) max_x = game->world.mapWidth - 1;
            if (max_y >= game->world.mapHeight) max_y = game->world.mapHeight - 1;
            for (int x = min_x; x <= max_x; ++x) {
                for (int y = min_y; y <= max_y; ++y) {
                    if (game->world.tileMap[x][y] != 87 && game->world.tileMap[x][y] != 88)
                        continue;
                    int seen_index = (city * game->world.mapWidth + x) * game->world.mapHeight + y;
                    if (seen_city_tiles[seen_index]) continue;
                    seen_city_tiles[seen_index] = 1;
                    game->world.cityIndex[city] = (int8_t)(game->world.cityIndex[city] + 1);
                }
            }
        }
    }
}

void game_applyBackgroundCost(Game *game) {
    if (game->build.demolitionAffordable) {
        game->economy.subsidyCost += game->build.backgroundCost;
        game->economy.money -= game->build.backgroundCost;
        game_pushMoneyHistory(game, -game->build.backgroundCost);
    } else {
        game_enterOutOfMoneyState(game);
    }
}

void game_demolishAtCursor(Game *game) {
    int cursor_x = game->ui.tileCursorX;
    int cursor_y = game->ui.tileCursorY;
    if (game->ui.blockBuildMenu || !game_isCityOrStationTile(game, cursor_x, cursor_y))
        return;

    game->build.backgroundCost = 0;
    int tile = game->world.tileMap[cursor_x][cursor_y];
    if (tile >= 81 && tile <= 86) {
        game_advanceConstructionTimers(game, cursor_x, cursor_y, -3);
        tile = game->world.tileMap[cursor_x][cursor_y];
        if (tile == 81 || tile == 84 || tile == 85) {
            int x;
            for (x = cursor_x;
                 x < game->world.mapWidth
                 && (game->world.tileMap[x][cursor_y] == 81
                     || game->world.tileMap[x][cursor_y] == 84
                     || game->world.tileMap[x][cursor_y] == 85);
                 ++x)
                game_queueTileChange(game, x, cursor_y, true, false);
            for (x = cursor_x - 1;
                 x >= 0
                 && (game->world.tileMap[x][cursor_y] == 81
                     || game->world.tileMap[x][cursor_y] == 84
                     || game->world.tileMap[x][cursor_y] == 85);
                 --x)
                game_queueTileChange(game, x, cursor_y, true, false);
        } else {
            int y;
            for (y = cursor_y;
                 y < game->world.mapHeight
                 && (game->world.tileMap[cursor_x][y] == 82
                     || game->world.tileMap[cursor_x][y] == 83
                     || game->world.tileMap[cursor_x][y] == 86);
                 ++y)
                game_queueTileChange(game, cursor_x, y, true, false);
            for (y = cursor_y - 1;
                 y >= 0
                 && (game->world.tileMap[cursor_x][y] == 82
                     || game->world.tileMap[cursor_x][y] == 83
                     || game->world.tileMap[cursor_x][y] == 86);
                 --y)
                game_queueTileChange(game, cursor_x, y, true, false);
        }
    } else if (tile >= 87 && tile <= 90) {
        int vehicle = game_findServiceVehicleAt(game, cursor_x, cursor_y);
        if (vehicle != 5000) {
            int x = unsignedByte(game->fleet.serviceSites[vehicle].x);
            int y = unsignedByte(game->fleet.serviceSites[vehicle].y);
            if ((game->fleet.serviceSites[vehicle].flags & 0x20) == 32) {
                if ((game->fleet.serviceSites[vehicle].flags & 0x40) == 0) {
                    game->world.trackMap[x][y - 1] = 0;
                    game->world.trackMap[x][y + 1] = 0;
                    game_queueTileChange(game, x, y - 1, true, false);
                    game_queueTileChange(game, x, y + 1, true, false);
                } else {
                    game->world.trackMap[x - 1][y] = 0;
                    game->world.trackMap[x + 1][y] = 0;
                    game_queueTileChange(game, x - 1, y, true, false);
                    game_queueTileChange(game, x + 1, y, true, false);
                }
            }
            game_queueTileChange(game, x, y, true, false);
            game_advanceConstructionTimers(game, x, y, -6);
            game->world.trackMap[x][y] = 0;
            game->fleet.serviceSites[vehicle].flags = 0;
        }
    } else {
        game_queueTileChange(game, cursor_x, cursor_y, true, false);
        game_advanceConstructionTimers(game, cursor_x, cursor_y, -6);
    }
    game_applyBackgroundCost(game);
}

void game_pickTrainsAtCursor(Game *game) {
    if (game->ui.blockBuildMenu) return;
    int track = game->world.trackMap[game->ui.tileCursorX][game->ui.tileCursorY];
    if ((track & 0xC0) == 128) {
        game->build.junctionDirCursor = (int8_t)(track & 0x3F);
        game->ready = false;
        game->ui.transitionCode = 20;
        game_setState(game, GameState::MenuOverlay);
    }
    if ((track & 0xC0) == 192) {
        game->build.stationDirCursor = (int8_t)(track & 0x3F);
        game->ready = false;
        game->ui.transitionCode = 21;
        game_setState(game, GameState::MenuOverlay);
    }
    if (track != 65) return;

    game->trainAtCursor =
        game_findTrainHeadAt(game, game->ui.tileCursorX, game->ui.tileCursorY);
    if (game->trainAtCursor == 5000) {
        game->serviceVehicleAtCursor =
            game_findServiceVehicleAt(game, game->ui.tileCursorX, game->ui.tileCursorY);
        if (game->serviceVehicleAtCursor != 5000) {
            game->ready = false;
            game->ui.transitionCode = 22;
            game_setState(game, GameState::MenuOverlay);
        }
        return;
    }

    game->build.wagonType = game->fleet.trainCars[game->trainAtCursor].era;
    for (int option = 0; option < 5; ++option) game->build.wagonOptionSlots[option] = -1;
    int car = game->fleet.trainCars[game->trainAtCursor].nextCar;
    for (int option = 0; option < 5 && car != -1; ++option) {
        switch (train_flag::type(game->fleet.trainCars[car].flags)) {
            case 2: game->build.wagonOptionSlots[option] = 0; break;
            case 3: game->build.wagonOptionSlots[option] = 1; break;
            case 4: case 5: game->build.wagonOptionSlots[option] = 2; break;
            case 6: case 7: game->build.wagonOptionSlots[option] = 3; break;
        }
        car = game->fleet.trainCars[car].nextCar;
    }
    for (int option = 0; option < 5; ++option)
        game->build.wagonCountMatch[option] = game->build.wagonOptionSlots[option];
    game->build.wagonDiscountType = game->build.wagonType;
    game->scenario.difficultyPct = 100;
    game_computeTrainCost(game);
    game->build.baseTrainCostSnapshot = game->build.unitCost;
    game->scenario.difficultyPct = 100 - 3 * game->fleet.trainCars[game->trainAtCursor].ageYears;
    game_computeTrainCost(game);
    game->build.currentTrainCostSnapshot = game->build.unitCost;
    game->ready = false;
    game->ui.transitionCode = 26;
    game_setState(game, GameState::MenuOverlay);
}

void game_buildPlaceTrackPiece(Game *game, int32_t piece) {
    int slot;
    switch (piece) {
        case 0:
            if (!game_isTileBuildable(game, game->ui.tileCursorX, game->ui.tileCursorY, 0)
                || (slot = game_allocateServiceVehicleSlot(
                        game, 1, game->ui.tileCursorX, game->ui.tileCursorY, false, false))
                       == 5000
                || !game_spendMoney(game, game->economy.costPower))
                break;
            game->world.tileMap[game->ui.tileCursorX][game->ui.tileCursorY] = 87;
            game->world.trackMap[game->ui.tileCursorX][game->ui.tileCursorY] = 65;
            game_advanceConstructionTimers(game, game->ui.tileCursorX, game->ui.tileCursorY, -8);
            game_incrementRowValueType13(game, game->economy.costPower);
            break;
        case 1:
            if (!game_isTileBuildable(game, game->ui.tileCursorX, game->ui.tileCursorY, 0)
                || (slot = game_allocateServiceVehicleSlot(
                        game, 1, game->ui.tileCursorX, game->ui.tileCursorY, true, false))
                       == 5000
                || !game_spendMoney(game, game->economy.costPower))
                break;
            game->world.tileMap[game->ui.tileCursorX][game->ui.tileCursorY] = 88;
            game->world.trackMap[game->ui.tileCursorX][game->ui.tileCursorY] = 65;
            game_advanceConstructionTimers(game, game->ui.tileCursorX, game->ui.tileCursorY, -8);
            game_incrementRowValueType13(game, game->economy.costPower);
            break;
        case 2:
            if (!game_isTileBuildable(game, game->ui.tileCursorX, game->ui.tileCursorY, 5)
                || (slot = game_allocateServiceVehicleSlot(
                        game, 1, game->ui.tileCursorX, game->ui.tileCursorY, false, true))
                       == 5000
                || !game_spendMoney(game, game->economy.costStruct))
                break;
            game->world.tileMap[game->ui.tileCursorX][game->ui.tileCursorY] = 87;
            game->world.tileMap[game->ui.tileCursorX][game->ui.tileCursorY - 1] = 89;
            game->world.tileMap[game->ui.tileCursorX][game->ui.tileCursorY + 1] = 89;
            game->world.trackMap[game->ui.tileCursorX][game->ui.tileCursorY] = 65;
            game->world.trackMap[game->ui.tileCursorX][game->ui.tileCursorY - 1] = 65;
            game->world.trackMap[game->ui.tileCursorX][game->ui.tileCursorY + 1] = 65;
            game_advanceConstructionTimers(game, game->ui.tileCursorX, game->ui.tileCursorY, -11);
            game_incrementRowValueType13(game, game->economy.costStruct);
            break;
        case 3:
            if (!game_isTileBuildable(game, game->ui.tileCursorX, game->ui.tileCursorY, 6)
                || (slot = game_allocateServiceVehicleSlot(
                        game, 1, game->ui.tileCursorX, game->ui.tileCursorY, true, true))
                       == 5000
                || !game_spendMoney(game, game->economy.costStruct))
                break;
            game->world.tileMap[game->ui.tileCursorX][game->ui.tileCursorY] = 88;
            game->world.tileMap[game->ui.tileCursorX - 1][game->ui.tileCursorY] = 90;
            game->world.tileMap[game->ui.tileCursorX + 1][game->ui.tileCursorY] = 90;
            game->world.trackMap[game->ui.tileCursorX][game->ui.tileCursorY] = 65;
            game->world.trackMap[game->ui.tileCursorX - 1][game->ui.tileCursorY] = 65;
            game->world.trackMap[game->ui.tileCursorX + 1][game->ui.tileCursorY] = 65;
            game_advanceConstructionTimers(game, game->ui.tileCursorX, game->ui.tileCursorY, -11);
            game_incrementRowValueType13(game, game->economy.costStruct);
            break;
    }
    game_tickInputOrAi(game, true);
    game_rebuildCityList(game);
}

void game_computeTrackBuild(Game *game, int32_t start_x,
                                 int32_t start_y, int32_t step_x,
                                 int32_t step_y, int32_t target_tile,
                                 int32_t start_tile, int32_t end_tile,
                                 int32_t middle_tile) {
    int end_x = start_x;
    int end_y = start_y;
    bool found_target = false;
    int cost = game->economy.moneyBaseA;
    game->build.trackStartX = start_x;
    game->build.trackStartY = start_y;
    game->build.trackStepX = step_x;
    game->build.trackStepY = step_y;
    while ((end_x += step_x) >= 0 && (end_y += step_y) >= 0
           && end_x < game->world.mapWidth && end_y < game->world.mapHeight) {
        cost += game->economy.moneyBaseB;
        if (game->world.tileMap[end_x][end_y] == target_tile) found_target = true;
        if (game->world.tileMap[end_x][end_y] != 0 || game->world.trackMap[end_x][end_y] != 0)
            break;
    }
    if (!found_target) return;

    game->build.selectionCost = cost;
    game->ui.buildMenuMode = 2;
    game->ui.menuDirty = true;
    game->ui.buildMenuItemIndex = 0;
    game->build.buildCancelled = false;
    game->world.tileMap[end_x][end_y] = (int8_t)end_tile;
    game->world.trackMap[end_x][end_y] = 65;
    game->world.tileMap[start_x][start_y] = (int8_t)start_tile;
    game->world.trackMap[start_x][start_y] = 65;
    while ((start_x += step_x) != end_x || (start_y += step_y) != end_y) {
        game->world.tileMap[start_x][start_y] = (int8_t)middle_tile;
        game_advanceConstructionTimers(game, start_x, start_y, -4);
        game->world.trackMap[start_x][start_y] = 65;
    }
    game->renderer.lastViewportCenterX = -1;
}

void game_revertTrackTiles(Game *game) {
    int x = game->build.trackStartX;
    int y = game->build.trackStartY;
    for (;;) {
        int tile = game->world.tileMap[x][y];
        if (tile < 81 || tile > 86) break;
        if (tile == 83) game->world.tileMap[x][y] = 7;
        if (tile == 86) game->world.tileMap[x][y] = 10;
        if (tile == 84) game->world.tileMap[x][y] = 8;
        if (tile == 85) game->world.tileMap[x][y] = 9;
        if (tile == 81 || tile == 82) game->world.tileMap[x][y] = 0;
        game->world.trackMap[x][y] = 0;
        x += game->build.trackStepX;
        y += game->build.trackStepY;
        if (!game->world.in_bounds(x, y)) break;
    }
}

void game_placeTrackAtCursor(Game *game, bool first_direction,
                                  int32_t first_tile, int32_t second_tile,
                                  int32_t match_tile_a, int32_t match_tile_b,
                                  int32_t result_tile_a, int32_t result_tile_b,
                                  bool join_track, int32_t join_tile) {
    int x = game->ui.tileCursorX;
    int y = game->ui.tileCursorY;
    int tile = game->world.tileMap[x][y];
    if (game->ui.blockBuildMenu || game->world.tileElevation[x][y] < 0
        || (game->world.trackMap[x][y] != 0 && !(join_track && tile == join_tile))
        || !(tile == join_tile || tile == match_tile_a || tile == match_tile_b
             || tile == 0 || (tile >= tile::SCENERY_FIRST && tile <= tile::SCENERY_LAST)))
        return;

    int cost = tile == 0 ? 0 : game->economy.baseTrainSpeed;
    cost += game->economy.accel1;
    if (join_track && tile == join_tile) {
        cost += game->economy.brake1;
        if (cost > game->economy.money) {
            game_enterOutOfMoneyState(game);
            return;
        }
        game->world.tileMap[x][y] = 76;
        int track = game->world.trackMap[x][y] & 0x3F;
        track = join_tile == 70 ? 0x48 | track : 0x41 | track << 3;
        game->world.trackMap[x][y] = (int8_t)track;
    } else {
        if (cost > game->economy.money) {
            game_enterOutOfMoneyState(game);
            return;
        }
        game->world.tileMap[x][y] =
            tile == match_tile_a ? (int8_t)result_tile_a
            : tile == match_tile_b ? (int8_t)result_tile_b
            : (int8_t)(first_direction ? first_tile : second_tile);
        game->world.trackMap[x][y] = 65;
    }
    game->economy.money -= cost;
    game->economy.totalSpent += cost;
    game->renderer.lastViewportCenterX = -1;
    game_pushMoneyHistory(game, -cost);
    game_incrementRowValueType13(game, cost);
    game_advanceConstructionTimers(game, x, y, -4);
}

void game_normalizeTrackBits(Game *game) {
    for (int x = 0; x < game->world.mapWidth; ++x) {
        for (int y = 0; y < game->world.mapHeight; ++y) {
            if (game->world.trackMap[x][y] == track::PRESENT) {
                int tile = game->world.tileMap[x][y];
                game->world.tileMap[x][y] =
                    tile == 78 ? 22 : tile == 80 ? 24 : tile == 85 ? 9
                    : tile == 84 ? 8 : tile == 77 ? 21 : tile == 79 ? 23
                    : tile == 83 ? 7 : tile == 86 ? 10 : 0;
                game->world.trackMap[x][y] = 0;
            }
            if (game->world.tileMap[x][y] != 76) continue;
            int low_owner = game->world.trackMap[x][y] & 7;
            int high_owner = (game->world.trackMap[x][y] & 0x3F) >> 3;
            if (low_owner == 0 && high_owner == 0) {
                game->world.tileMap[x][y] = 0;
                game->world.trackMap[x][y] = 0;
            } else if (low_owner == 0) {
                game->world.tileMap[x][y] = 71;
                game->world.trackMap[x][y] = static_cast<int8_t>(track::PRESENT | high_owner);
            } else if (high_owner == 0) {
                game->world.tileMap[x][y] = 70;
                game->world.trackMap[x][y] = static_cast<int8_t>(track::PRESENT | low_owner);
            }
        }
    }
}

void game_setTrackOwnerCode(Game *game, int32_t owner) {
    for (int x = 0; x < game->world.mapWidth; ++x) {
        for (int y = 0; y < game->world.mapHeight; ++y) {
            if (game->world.trackMap[x][y] == track::PRESENT) {
                if (owner == 1) game_advanceConstructionTimers(game, x, y, -4);
                game->world.trackMap[x][y] = static_cast<int8_t>(track::PRESENT | owner);
            }
            if (game->world.tileMap[x][y] != 76) continue;
            int low_owner = game->world.trackMap[x][y] & 7;
            int high_owner = (game->world.trackMap[x][y] & 0x3F) >> 3;
            if (low_owner == 0) low_owner = owner;
            if (high_owner == 0) high_owner = owner;
            game->world.trackMap[x][y] =
                static_cast<int8_t>(track::PRESENT | low_owner | high_owner << track::DIR_OWNER_SHIFT);
        }
    }
}

void game_rebuildMenuTypeIndexes(Game *game) {
    game->ui.menuRowEnabledCount = 0;
    for (int row = 0; row < 5; ++row) {
        if (game->ui.menuRowType[row] == 0) continue;
        game->ui.enabledMenuRowIndexes[game->ui.menuRowEnabledCount++] = (int8_t)row;
    }
}

// Build-menu items that act on the tile under the cursor (place a depot/track/
// terrain tile, demolish, smooth, or set an auto-route end). A bottom-bar click
// carries no map tile -- the cursor re-anchors to the screen centre every tick
// -- so these must only arm from the bar; the player then clicks the map to act
// on the tile they point at. Mirrors the place-at-cursor branches of
// game_openBuildMenu below; keep the two in sync.
bool game_buildToolActsOnCursorTile(int32_t mode, int32_t item) {
    switch (mode) {
        case 0: return item == 0;                                 // select/inspect at cursor
        case 1: return item == 0 || item == 2                     // route anchor, bridge/tunnel
                     || item == 4 || item == 5 || item == 6;      // demolish, smooth up/down
        case 3: return item != 9;                                 // place depot (9 = back)
        case 4: return item == 0;                                 // route target
        case 5: return item != 9;                                 // place track piece (9 = back)
        default: return false;                                    // navigation, confirm/cancel
    }
}

void game_openBuildMenu(Game *game) {
    int item = game->ui.buildMenuItems[game->ui.buildMenuMode][game->ui.buildMenuItemIndex];
    bool allowed_while_paused =
        (game->ui.buildMenuMode == 0 && (item == 11 || item == 1 || item == 9))
        || (game->ui.buildMenuMode == 1 && (item == 1 || item == 3 || item == 9))
        || (game->ui.buildMenuMode == 3 && item == 9)
        || (game->ui.buildMenuMode == 5 && item == 9);
    if (game->ui.pauseFlag && !allowed_while_paused) return;

    switch (game->ui.buildMenuMode) {
        case 0:
            switch (item) {
                case 0: game_pickTrainsAtCursor(game); break;
                case 1:
                    game->ui.buildMenuMode = 1;
                    game->ui.menuDirty = true;
                    game->ui.buildMenuItemIndex = 0;
                    break;
                case 2:
                    game->ready = false; game->ui.transitionCode = 30;
                    game_setState(game, GameState::MenuOverlay); break;
                case 3:
                    game->ready = false; game->ui.transitionCode = 31;
                    game_setState(game, GameState::MenuOverlay); break;
                case 4:
                    game_rebuildMenuTypeIndexes(game);
                    game->ready = false; game->ui.transitionCode = 60;
                    game_setState(game, GameState::MenuOverlay); break;
                case 5:
                    game->ready = false; game->ui.transitionCode = 17;
                    game_setState(game, GameState::MenuOverlay); break;
                case 8: game->ui.inputPending = true; break;
                case 9:
                    game_computeMapTrainMetric(game, game->scenario.scenarioStep - 2);
                    game->ui.useDefaultOverlaySelection = false;
                    game->ready = false; game->ui.transitionCode = 1;
                    game_setState(game, GameState::MenuOverlay); break;
                case 11: game->ui.pauseFlag = !game->ui.pauseFlag; break;
            }
            break;
        case 1:
            switch (item) {
                case 0:
                    if (game->ui.blockBuildMenu
                        || !game_isPlaceableTerrain(game, game->ui.tileCursorX,
                                                  game->ui.tileCursorY))
                        break;
                    game->pathfinder.anchorX = game->ui.tileCursorX;
                    game->pathfinder.anchorY = game->ui.tileCursorY;
                    game->world.tileMap[game->pathfinder.anchorX][game->pathfinder.anchorY] = 127;
                    game->ui.buildMenuMode = 4;
                    game->ui.menuDirty = true;
                    game->ui.buildMenuItemIndex = 0;
                    break;
                case 1:
                    game->ui.buildMenuMode = 5;
                    game->ui.menuDirty = true;
                    game->ui.buildMenuItemIndex = 0;
                    break;
                case 2:
                    if (game->ui.blockBuildMenu) break;
                    if (game->world.tileMap[game->ui.tileCursorX][game->ui.tileCursorY] == 7)
                        game_computeTrackBuild(
                            game, game->ui.tileCursorX, game->ui.tileCursorY,
                            0, -1, 10, 83, 86, 82);
                    if (game->world.tileMap[game->ui.tileCursorX][game->ui.tileCursorY] == 10)
                        game_computeTrackBuild(
                            game, game->ui.tileCursorX, game->ui.tileCursorY,
                            0, 1, 7, 86, 83, 82);
                    if (game->world.tileMap[game->ui.tileCursorX][game->ui.tileCursorY] == 8)
                        game_computeTrackBuild(
                            game, game->ui.tileCursorX, game->ui.tileCursorY,
                            -1, 0, 9, 84, 85, 81);
                    if (game->world.tileMap[game->ui.tileCursorX][game->ui.tileCursorY] == 9)
                        game_computeTrackBuild(
                            game, game->ui.tileCursorX, game->ui.tileCursorY,
                            1, 0, 8, 85, 84, 81);
                    break;
                case 3:
                    game->ui.buildMenuMode = 3;
                    game->ui.menuDirty = true;
                    game->ui.buildMenuItemIndex = 0;
                    break;
                case 4: game_demolishAtCursor(game); break;
                case 5:
                    if (!game->ui.blockBuildMenu)
                        game_smoothTileAt(game, game->build.swapMapX, game->build.swapMapY,
                                               false);
                    break;
                case 6:
                    if (!game->ui.blockBuildMenu)
                        game_smoothTileAt(game, game->build.swapMapX, game->build.swapMapY,
                                               true);
                    break;
                case 9:
                    game->ui.buildMenuMode = 0;
                    game->ui.menuDirty = true;
                    game->ui.buildMenuItemIndex = 1;
                    break;
            }
            break;
        case 2:
            if (item == 0) {
                if (game->build.selectionCost <= game->economy.money) {
                    game_setTrackOwnerCode(game, 1);
                    game->economy.money -= game->build.selectionCost;
                    game->economy.totalSpent += game->build.selectionCost;
                    game_pushMoneyHistory(game, -game->build.selectionCost);
                    game_incrementRowValueType13(game, game->build.selectionCost);
                } else {
                    game_normalizeTrackBits(game);
                    game_enterOutOfMoneyState(game);
                }
            } else if (item == 9) {
                if (game->build.buildCancelled) game_normalizeTrackBits(game);
                else game_revertTrackTiles(game);
            }
            game->ui.buildMenuMode = 1;
            game->ui.buildMenuItemIndex = game->build.buildCancelled ? 0 : 2;
            game->ui.menuDirty = true;
            game->renderer.lastViewportCenterX = -1;
            break;
        case 3:
            if (item != 9) {
                game_buildPlaceTrackPiece(game, item);
                return;
            }
            game->ui.buildMenuMode = 1;
            game->ui.menuDirty = true;
            game->ui.buildMenuItemIndex = 3;
            break;
        case 4:
            if (item == 0) {
                if (game->ui.blockBuildMenu
                    || !game_isPlaceableTerrain(game, game->ui.tileCursorX,
                                              game->ui.tileCursorY))
                    break;
                game->pathfinder.targetX = game->ui.tileCursorX;
                game->pathfinder.targetY = game->ui.tileCursorY;
                game->world.tileMap[game->pathfinder.anchorX][game->pathfinder.anchorY] = 0;
                game_setState(game, GameState::BuildingPath);
            } else if (item == 9) {
                game->world.tileMap[game->pathfinder.anchorX][game->pathfinder.anchorY] = 0;
                game->ui.buildMenuMode = 1;
                game->ui.menuDirty = true;
                game->ui.buildMenuItemIndex = 0;
            }
            break;
        case 5:
            switch (item) {
                case 0:
                    game_placeTrackAtCursor(
                        game, game->ui.vehicleMenuSelectIndex, 75, 74,
                        -1, -1, -1, -1, false, 0);
                    return;
                case 1:
                    game_placeTrackAtCursor(
                        game, game->ui.trainMenuSelectIndex, 72, 73,
                        -1, -1, -1, -1, false, 0);
                    return;
                case 2:
                    game_placeTrackAtCursor(
                        game, true, 70, 0, 21, 23, 77, 79, true, 71);
                    return;
                case 3:
                    game_placeTrackAtCursor(
                        game, true, 71, 0, 22, 24, 78, 80, true, 70);
                    return;
                case 9:
                    game->ui.buildMenuMode = 1;
                    game->ui.menuDirty = true;
                    game->ui.buildMenuItemIndex = 1;
                    break;
            }
            break;
    }
}

void game_addSelectedOption(Game *game, int32_t option) {
    for (int slot = 0; slot < 5; ++slot) {
        if (game->build.wagonOptionSlots[slot] != -1) continue;
        game->build.wagonOptionSlots[slot] = option;
        break;
    }
}

void game_popSelectedOption(Game *game) {
    for (int slot = 4; slot >= 0; --slot) {
        if (game->build.wagonOptionSlots[slot] == -1) continue;
        game->build.wagonOptionSlots[slot] = -1;
        break;
    }
}

int32_t game_allocateTrainSlot(Game *game, int32_t x, int32_t y,
                             int32_t engine_type, int32_t era,
                             int32_t direction, int32_t energy) {
    int train = 500;
    for (int slot = 0; slot < 500; ++slot) {
        if (!train_flag::isUsed(game->fleet.trainCars[slot].flags)) {
            train = slot;
            break;
        }
    }
    if (train != 500) {
        game->fleet.trainCars[train].flags = static_cast<int8_t>(
            train_flag::USED_SLOT | train_flag::HEAD | engine_type);
        game->fleet.trainCars[train].nextCar = -1;
        game->fleet.trainCars[train].x = (int8_t)x;
        game->fleet.trainCars[train].y = (int8_t)y;
        game->fleet.trainCars[train].direction = (int8_t)direction;
        game->fleet.trainCars[train].secondaryCargoOrBaseEnergy = (int8_t)energy;
        game->fleet.trainCars[train].phase = 0;
        game->fleet.trainCars[train].energyCargoOrRemovalTicks = 1;
        game->fleet.trainCars[train].lastServiceSite = -1;
        game->fleet.trainCars[train].era = (int8_t)era;
        game->fleet.trainCars[train].ageYears = 0;
        game->fleet.trainCars[train].stateCode = 7;
        game->fleet.trainCars[train].aiOverride = 0;
    }
    game->renderer.lastViewportCenterX = -1;
    return train;
}

void game_incrementCargoOptionCount(Game *game, int32_t car_type) {
    for (int row = 0; row < 5; ++row) {
        if (car_type == 6 && game->ui.menuRowType[row] == 9)
            ++game->ui.menuRowValue[row];
        if (car_type == 4 && game->ui.menuRowType[row] == 10)
            ++game->ui.menuRowValue[row];
        if (car_type == 3 && game->ui.menuRowType[row] == 11)
            ++game->ui.menuRowValue[row];
        if (car_type == 2 && game->ui.menuRowType[row] == 12)
            ++game->ui.menuRowValue[row];
    }
}

void game_decrementCargoOptionCount(Game *game, int32_t car_type) {
    for (int row = 0; row < 5; ++row) {
        if ((car_type == 7 || car_type == 6) && game->ui.menuRowType[row] == 9)
            --game->ui.menuRowValue[row];
        if ((car_type == 5 || car_type == 4) && game->ui.menuRowType[row] == 10)
            --game->ui.menuRowValue[row];
        if (car_type == 3 && game->ui.menuRowType[row] == 11)
            --game->ui.menuRowValue[row];
        if (car_type == 2 && game->ui.menuRowType[row] == 12)
            --game->ui.menuRowValue[row];
    }
}

void game_decrementTrainCargoOptionCounts(Game *game, int32_t car) {
    if (car == -1) return;
    game_decrementCargoOptionCount(game, game->fleet.trainCars[car].flags & 7);
    game_decrementTrainCargoOptionCounts(game, game->fleet.trainCars[car].nextCar);
}

void game_incrementRowValueType14(Game *game, int32_t amount) {
    for (int row = 0; row < 5; ++row)
        if (game->ui.menuRowType[row] == 14) game->ui.menuRowValue[row] += amount;
}

bool game_buyTrainAtCursor(Game *game) {
    if (game->economy.money < game->build.unitCost) {
        game_enterOutOfMoneyState(game);
        return false;
    }
    int direction =
        (game->fleet.serviceSites[game->serviceVehicleAtCursor].flags & 0x40) == 64 ? 2 : 0;
    // Spawn on the depot that opened this dialog, taken from the captured
    // serviceVehicleAtCursor. The live tile cursor re-anchors to the screen
    // centre every Playing tick, so by the time "buy" fires it has drifted off
    // the depot; using it here would strand the engine on an empty tile.
    int station_x = unsignedByte(game->fleet.serviceSites[game->serviceVehicleAtCursor].x);
    int station_y = unsignedByte(game->fleet.serviceSites[game->serviceVehicleAtCursor].y);
    int train = game_allocateTrainSlot(
        game, station_x, station_y,
        game->build.wagonType < 3 ? 0 : 1, game->build.wagonType,
        direction, 8 - game->build.wagonType);
    for (int option = 0; option < 5; ++option) {
        if (game->build.wagonOptionSlots[option] < 0) continue;
        int car_type = 2 + game->build.wagonOptionSlots[option];
        if (car_type == 5) ++car_type;
        train = game_spawnTrainCar(game, train, car_type);
        game_incrementCargoOptionCount(game, car_type);
    }
    game->economy.money -= game->build.unitCost;
    game->economy.aiMoneySpent += game->build.unitCost;
    game_pushMoneyHistory(game, -game->build.unitCost);
    ++game->scenario.aiWagonsBought;
    return true;
}

bool game_applyTrainUpgrade(Game *game, int32_t train,
                                 int32_t state_code, int32_t *options) {
    int cost = game->build.unitCost - game->build.currentTrainCostSnapshot;
    if (game->economy.money < cost) {
        game_enterOutOfMoneyState(game);
        return false;
    }
    game->fleet.trainCars[train].stateCode = (int16_t)state_code;
    game_decrementTrainCargoOptionCounts(game, game->fleet.trainCars[train].nextCar);
    for (int option = 0; option < 5; ++option) {
        if (options[option] > -1) {
            int car_type = options[option] + 2;
            if (car_type == 5) ++car_type;
            game_incrementCargoOptionCount(game, car_type);
        }
        game->fleet.trainCars[train].stateCode = (int16_t)(
            game->fleet.trainCars[train].stateCode
            | (options[option] & 7) << (3 + 3 * option));
    }
    game->economy.money -= cost;
    game->economy.aiMoneySpent += cost;
    if (cost != 0) game_pushMoneyHistory(game, -cost);
    return true;
}
