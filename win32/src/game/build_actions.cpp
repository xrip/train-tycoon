// Direct game-map build and selection actions.
#include "game_methods.h"
#include <vector>

void gc_enterOutOfMoneyState(GameCanvas *gc) {
    gc->ready = false;
    gc->transitionCode = 11;
    gc_setState(gc, +GameState::MenuOverlay);
}

void gc_incrementRowValueType13(GameCanvas *gc, int32_t amount) {
    for (int row = 0; row < 5; ++row)
        if (gc->menuRowType[row] == 13) gc->menuRowValue[row] += amount;
}

bool gc_isAnyTrainCarAt(GameCanvas *gc, int32_t car, int32_t x, int32_t y) {
    if (car == -1) return false;
    if (gc_byteToUnsignedInt(gc, gc->fleet.trainX[car]) == x
        && gc_byteToUnsignedInt(gc, gc->fleet.trainY[car]) == y)
        return true;
    return gc_isAnyTrainCarAt(gc, gc->fleet.nextCar[car], x, y);
}

int32_t gc_findTrainHeadAt(GameCanvas *gc, int32_t x, int32_t y) {
    for (int train = 0; train < 500; ++train) {
        if ((gc->fleet.trainFlags[train] & 0x80) == 128
            && (gc->fleet.trainFlags[train] & 0x40) == 64
            && gc_isAnyTrainCarAt(gc, train, x, y))
            return train;
    }
    return 5000;
}

int32_t gc_allocateServiceVehicleSlot(GameCanvas *gc, int32_t type,
                                      int32_t x, int32_t y,
                                      bool horizontal, bool extended) {
    int slot = 5000;
    for (int vehicle = 0; vehicle < 250; ++vehicle) {
        if ((gc->fleet.vehicleFlags[vehicle] & 0x80) == 0) {
            slot = vehicle;
            break;
        }
    }
    if (slot == 5000) return slot;

    gc->fleet.vehicleFlags[slot] = (int8_t)(0x80 | (horizontal ? 0x40 : 0)
                                      | (extended ? 0x20 : 0) | type);
    gc->fleet.serviceVehicleX[slot] = (int8_t)x;
    gc->fleet.serviceVehicleY[slot] = (int8_t)y;
    gc->fleet.serviceUnloadFlags[slot] = 0;
    for (int resource = 0; resource < 5; ++resource)
        gc->fleet.serviceResourceStock[slot][resource] = 0;
    return slot;
}

void gc_computeTrainCost(GameCanvas *gc) {
    switch (gc->build.wagonType) {
        case 0: gc->build.unitCost = gc->gWagonCost; break;
        case 1: gc->build.unitCost = gc->gWagonCost2; break;
        case 2: gc->build.unitCost = gc->gWagonCost3; break;
        case 3: gc->build.unitCost = gc->gWagonCost4; break;
        case 4: gc->build.unitCost = gc->gWagonCost5; break;
        case 5: gc->build.unitCost = gc->gWagonCost6; break;
    }
    if (gc->build.wagonType == gc->build.wagonDiscountType)
        gc->build.unitCost = gc->build.unitCost * gc->scenario.difficultyPct / 100;

    for (int option = 0; option < 5; ++option) {
        int option_cost;
        switch (gc->build.wagonOptionSlots[option]) {
            case 0: option_cost = gc->costGroupC1; break;
            case 1: option_cost = gc->costGroupC2; break;
            case 2: option_cost = gc->costGroupC3; break;
            case 3: option_cost = gc->costGroupC4; break;
            default: continue;
        }
        if (gc->build.wagonOptionSlots[option] == gc->build.wagonCountMatch[option])
            option_cost = option_cost * gc->scenario.difficultyPct / 100;
        gc->build.unitCost += option_cost;
    }
}

void gc_rebuildCityList(GameCanvas *gc) {
    for (int city = 0; city < 48; ++city) gc->world.cityIndex[city] = 0;
    const int seen_count = 64 * gc->world.mapWidth * gc->world.mapHeight;
    std::vector<uint8_t> seen_city_tiles(
        static_cast<std::size_t>(seen_count));
    for (int station_x = 0; station_x < gc->world.mapWidth; ++station_x) {
        for (int station_y = 0; station_y < gc->world.mapHeight; ++station_y) {
            int track = gc->world.trackMap[station_x][station_y];
            if ((track & 0xC0) != 192) continue;
            int city = track & 0x3F;
            int min_x = station_x - 3;
            int min_y = station_y - 3;
            int max_x = station_x + 3;
            int max_y = station_y + 3;
            if (min_x < 0) min_x = 0;
            if (min_y < 0) min_y = 0;
            if (max_x >= gc->world.mapWidth) max_x = gc->world.mapWidth - 1;
            if (max_y >= gc->world.mapHeight) max_y = gc->world.mapHeight - 1;
            for (int x = min_x; x <= max_x; ++x) {
                for (int y = min_y; y <= max_y; ++y) {
                    if (gc->world.tileMap[x][y] != 87 && gc->world.tileMap[x][y] != 88)
                        continue;
                    int seen_index = (city * gc->world.mapWidth + x) * gc->world.mapHeight + y;
                    if (seen_city_tiles[seen_index]) continue;
                    seen_city_tiles[seen_index] = 1;
                    gc->world.cityIndex[city] = (int8_t)(gc->world.cityIndex[city] + 1);
                }
            }
        }
    }
}

void gc_applyBackgroundCost(GameCanvas *gc) {
    if (gc->build.demolitionAffordable) {
        gc->economy.subsidyCost += gc->build.backgroundCost;
        gc->economy.money -= gc->build.backgroundCost;
        gc_pushMoneyHistory(gc, -gc->build.backgroundCost);
    } else {
        gc_enterOutOfMoneyState(gc);
    }
}

void gc_demolishAtCursor(GameCanvas *gc) {
    int cursor_x = gc->tileCursorX;
    int cursor_y = gc->tileCursorY;
    if (gc->blockBuildMenu || !gc_isCityOrStationTile(gc, cursor_x, cursor_y))
        return;

    gc->build.backgroundCost = 0;
    int tile = gc->world.tileMap[cursor_x][cursor_y];
    if (tile >= 81 && tile <= 86) {
        gc_advanceConstructionTimers(gc, cursor_x, cursor_y, -3);
        tile = gc->world.tileMap[cursor_x][cursor_y];
        if (tile == 81 || tile == 84 || tile == 85) {
            int x;
            for (x = cursor_x;
                 x < gc->world.mapWidth
                 && (gc->world.tileMap[x][cursor_y] == 81
                     || gc->world.tileMap[x][cursor_y] == 84
                     || gc->world.tileMap[x][cursor_y] == 85);
                 ++x)
                gc_handleCursorClickInlined(gc, x, cursor_y, true, false);
            for (x = cursor_x - 1;
                 x >= 0
                 && (gc->world.tileMap[x][cursor_y] == 81
                     || gc->world.tileMap[x][cursor_y] == 84
                     || gc->world.tileMap[x][cursor_y] == 85);
                 --x)
                gc_handleCursorClickInlined(gc, x, cursor_y, true, false);
        } else {
            int y;
            for (y = cursor_y;
                 y < gc->world.mapHeight
                 && (gc->world.tileMap[cursor_x][y] == 82
                     || gc->world.tileMap[cursor_x][y] == 83
                     || gc->world.tileMap[cursor_x][y] == 86);
                 ++y)
                gc_handleCursorClickInlined(gc, cursor_x, y, true, false);
            for (y = cursor_y - 1;
                 y >= 0
                 && (gc->world.tileMap[cursor_x][y] == 82
                     || gc->world.tileMap[cursor_x][y] == 83
                     || gc->world.tileMap[cursor_x][y] == 86);
                 --y)
                gc_handleCursorClickInlined(gc, cursor_x, y, true, false);
        }
    } else if (tile >= 87 && tile <= 90) {
        int vehicle = gc_findServiceVehicleAt(gc, cursor_x, cursor_y);
        if (vehicle != 5000) {
            int x = gc_byteToUnsignedInt(gc, gc->fleet.serviceVehicleX[vehicle]);
            int y = gc_byteToUnsignedInt(gc, gc->fleet.serviceVehicleY[vehicle]);
            if ((gc->fleet.vehicleFlags[vehicle] & 0x20) == 32) {
                if ((gc->fleet.vehicleFlags[vehicle] & 0x40) == 0) {
                    gc->world.trackMap[x][y - 1] = 0;
                    gc->world.trackMap[x][y + 1] = 0;
                    gc_handleCursorClickInlined(gc, x, y - 1, true, false);
                    gc_handleCursorClickInlined(gc, x, y + 1, true, false);
                } else {
                    gc->world.trackMap[x - 1][y] = 0;
                    gc->world.trackMap[x + 1][y] = 0;
                    gc_handleCursorClickInlined(gc, x - 1, y, true, false);
                    gc_handleCursorClickInlined(gc, x + 1, y, true, false);
                }
            }
            gc_handleCursorClickInlined(gc, x, y, true, false);
            gc_advanceConstructionTimers(gc, x, y, -6);
            gc->world.trackMap[x][y] = 0;
            gc->fleet.vehicleFlags[vehicle] = 0;
        }
    } else {
        gc_handleCursorClickInlined(gc, cursor_x, cursor_y, true, false);
        gc_advanceConstructionTimers(gc, cursor_x, cursor_y, -6);
    }
    gc_applyBackgroundCost(gc);
}

void gc_pickTrainsAtCursor(GameCanvas *gc) {
    if (gc->blockBuildMenu) return;
    int track = gc->world.trackMap[gc->tileCursorX][gc->tileCursorY];
    if ((track & 0xC0) == 128) {
        gc->junctionDirCursor = (int8_t)(track & 0x3F);
        gc->ready = false;
        gc->transitionCode = 20;
        gc_setState(gc, +GameState::MenuOverlay);
    }
    if ((track & 0xC0) == 192) {
        gc->stationDirCursor = (int8_t)(track & 0x3F);
        gc->ready = false;
        gc->transitionCode = 21;
        gc_setState(gc, +GameState::MenuOverlay);
    }
    if (track != 65) return;

    gc->trainAtCursor =
        gc_findTrainHeadAt(gc, gc->tileCursorX, gc->tileCursorY);
    if (gc->trainAtCursor == 5000) {
        gc->serviceVehicleAtCursor =
            gc_findServiceVehicleAt(gc, gc->tileCursorX, gc->tileCursorY);
        if (gc->serviceVehicleAtCursor != 5000) {
            gc->ready = false;
            gc->transitionCode = 22;
            gc_setState(gc, +GameState::MenuOverlay);
        }
        return;
    }

    gc->build.wagonType = gc->fleet.trainEraOrStage[gc->trainAtCursor];
    for (int option = 0; option < 5; ++option) gc->build.wagonOptionSlots[option] = -1;
    int car = gc->fleet.nextCar[gc->trainAtCursor];
    for (int option = 0; option < 5 && car != -1; ++option) {
        switch (gc->fleet.trainFlags[car] & 0xF) {
            case 2: gc->build.wagonOptionSlots[option] = 0; break;
            case 3: gc->build.wagonOptionSlots[option] = 1; break;
            case 4: case 5: gc->build.wagonOptionSlots[option] = 2; break;
            case 6: case 7: gc->build.wagonOptionSlots[option] = 3; break;
        }
        car = gc->fleet.nextCar[car];
    }
    for (int option = 0; option < 5; ++option)
        gc->build.wagonCountMatch[option] = gc->build.wagonOptionSlots[option];
    gc->build.wagonDiscountType = gc->build.wagonType;
    gc->scenario.difficultyPct = 100;
    gc_computeTrainCost(gc);
    gc->build.baseTrainCostSnapshot = gc->build.unitCost;
    gc->scenario.difficultyPct = 100 - 3 * gc->fleet.trainAgeOrTimer[gc->trainAtCursor];
    gc_computeTrainCost(gc);
    gc->build.currentTrainCostSnapshot = gc->build.unitCost;
    gc->ready = false;
    gc->transitionCode = 26;
    gc_setState(gc, +GameState::MenuOverlay);
}

void gc_buildPlaceTrackPiece(GameCanvas *gc, int32_t piece) {
    int slot;
    switch (piece) {
        case 0:
            if (!gc_isTileBuildable(gc, gc->tileCursorX, gc->tileCursorY, 0)
                || (slot = gc_allocateServiceVehicleSlot(
                        gc, 1, gc->tileCursorX, gc->tileCursorY, false, false))
                       == 5000
                || !gc_spendMoney(gc, gc->costPower))
                break;
            gc->world.tileMap[gc->tileCursorX][gc->tileCursorY] = 87;
            gc->world.trackMap[gc->tileCursorX][gc->tileCursorY] = 65;
            gc_advanceConstructionTimers(gc, gc->tileCursorX, gc->tileCursorY, -8);
            gc_incrementRowValueType13(gc, gc->costPower);
            break;
        case 1:
            if (!gc_isTileBuildable(gc, gc->tileCursorX, gc->tileCursorY, 0)
                || (slot = gc_allocateServiceVehicleSlot(
                        gc, 1, gc->tileCursorX, gc->tileCursorY, true, false))
                       == 5000
                || !gc_spendMoney(gc, gc->costPower))
                break;
            gc->world.tileMap[gc->tileCursorX][gc->tileCursorY] = 88;
            gc->world.trackMap[gc->tileCursorX][gc->tileCursorY] = 65;
            gc_advanceConstructionTimers(gc, gc->tileCursorX, gc->tileCursorY, -8);
            gc_incrementRowValueType13(gc, gc->costPower);
            break;
        case 2:
            if (!gc_isTileBuildable(gc, gc->tileCursorX, gc->tileCursorY, 5)
                || (slot = gc_allocateServiceVehicleSlot(
                        gc, 1, gc->tileCursorX, gc->tileCursorY, false, true))
                       == 5000
                || !gc_spendMoney(gc, gc->costStruct))
                break;
            gc->world.tileMap[gc->tileCursorX][gc->tileCursorY] = 87;
            gc->world.tileMap[gc->tileCursorX][gc->tileCursorY - 1] = 89;
            gc->world.tileMap[gc->tileCursorX][gc->tileCursorY + 1] = 89;
            gc->world.trackMap[gc->tileCursorX][gc->tileCursorY] = 65;
            gc->world.trackMap[gc->tileCursorX][gc->tileCursorY - 1] = 65;
            gc->world.trackMap[gc->tileCursorX][gc->tileCursorY + 1] = 65;
            gc_advanceConstructionTimers(gc, gc->tileCursorX, gc->tileCursorY, -11);
            gc_incrementRowValueType13(gc, gc->costStruct);
            break;
        case 3:
            if (!gc_isTileBuildable(gc, gc->tileCursorX, gc->tileCursorY, 6)
                || (slot = gc_allocateServiceVehicleSlot(
                        gc, 1, gc->tileCursorX, gc->tileCursorY, true, true))
                       == 5000
                || !gc_spendMoney(gc, gc->costStruct))
                break;
            gc->world.tileMap[gc->tileCursorX][gc->tileCursorY] = 88;
            gc->world.tileMap[gc->tileCursorX - 1][gc->tileCursorY] = 90;
            gc->world.tileMap[gc->tileCursorX + 1][gc->tileCursorY] = 90;
            gc->world.trackMap[gc->tileCursorX][gc->tileCursorY] = 65;
            gc->world.trackMap[gc->tileCursorX - 1][gc->tileCursorY] = 65;
            gc->world.trackMap[gc->tileCursorX + 1][gc->tileCursorY] = 65;
            gc_advanceConstructionTimers(gc, gc->tileCursorX, gc->tileCursorY, -11);
            gc_incrementRowValueType13(gc, gc->costStruct);
            break;
    }
    gc_tickInputOrAi(gc, true);
    gc_rebuildCityList(gc);
}

void gc_computeTrackBuildInlined(GameCanvas *gc, int32_t start_x,
                                 int32_t start_y, int32_t step_x,
                                 int32_t step_y, int32_t target_tile,
                                 int32_t start_tile, int32_t end_tile,
                                 int32_t middle_tile) {
    int end_x = start_x;
    int end_y = start_y;
    bool found_target = false;
    int cost = gc->moneyBaseA;
    gc->build.trackStartX = start_x;
    gc->build.trackStartY = start_y;
    gc->build.trackStepX = step_x;
    gc->build.trackStepY = step_y;
    while ((end_x += step_x) >= 0 && (end_y += step_y) >= 0
           && end_x < gc->world.mapWidth && end_y < gc->world.mapHeight) {
        cost += gc->moneyBaseB;
        if (gc->world.tileMap[end_x][end_y] == target_tile) found_target = true;
        if (gc->world.tileMap[end_x][end_y] != 0 || gc->world.trackMap[end_x][end_y] != 0)
            break;
    }
    if (!found_target) return;

    gc->build.selectionCost = cost;
    gc->buildMenuMode = 2;
    gc->menuDirty = true;
    gc->buildMenuItemIndex = 0;
    gc->build.buildCancelled = false;
    gc->world.tileMap[end_x][end_y] = (int8_t)end_tile;
    gc->world.trackMap[end_x][end_y] = 65;
    gc->world.tileMap[start_x][start_y] = (int8_t)start_tile;
    gc->world.trackMap[start_x][start_y] = 65;
    while ((start_x += step_x) != end_x || (start_y += step_y) != end_y) {
        gc->world.tileMap[start_x][start_y] = (int8_t)middle_tile;
        gc_advanceConstructionTimers(gc, start_x, start_y, -4);
        gc->world.trackMap[start_x][start_y] = 65;
    }
    gc->lastViewportCenterX = -1;
}

void gc_revertTrackTiles(GameCanvas *gc) {
    int x = gc->build.trackStartX;
    int y = gc->build.trackStartY;
    for (;;) {
        int tile = gc->world.tileMap[x][y];
        if (tile < 81 || tile > 86) break;
        if (tile == 83) gc->world.tileMap[x][y] = 7;
        if (tile == 86) gc->world.tileMap[x][y] = 10;
        if (tile == 84) gc->world.tileMap[x][y] = 8;
        if (tile == 85) gc->world.tileMap[x][y] = 9;
        if (tile == 81 || tile == 82) gc->world.tileMap[x][y] = 0;
        gc->world.trackMap[x][y] = 0;
        x += gc->build.trackStepX;
        y += gc->build.trackStepY;
        if (!gc->world.in_bounds(x, y)) break;
    }
}

void gc_placeTrackAtCursorInlined(GameCanvas *gc, bool first_direction,
                                  int32_t first_tile, int32_t second_tile,
                                  int32_t match_tile_a, int32_t match_tile_b,
                                  int32_t result_tile_a, int32_t result_tile_b,
                                  bool join_track, int32_t join_tile) {
    int x = gc->tileCursorX;
    int y = gc->tileCursorY;
    int tile = gc->world.tileMap[x][y];
    if (gc->blockBuildMenu || gc->world.tileElevation[x][y] < 0
        || (gc->world.trackMap[x][y] != 0 && !(join_track && tile == join_tile))
        || !(tile == join_tile || tile == match_tile_a || tile == match_tile_b
             || tile == 0 || (tile >= tile::SCENERY_FIRST && tile <= tile::SCENERY_LAST)))
        return;

    int cost = tile == 0 ? 0 : gc->baseTrainSpeed;
    cost += gc->accel1;
    if (join_track && tile == join_tile) {
        cost += gc->brake1;
        if (cost > gc->economy.money) {
            gc_enterOutOfMoneyState(gc);
            return;
        }
        gc->world.tileMap[x][y] = 76;
        int track = gc->world.trackMap[x][y] & 0x3F;
        track = join_tile == 70 ? 0x48 | track : 0x41 | track << 3;
        gc->world.trackMap[x][y] = (int8_t)track;
    } else {
        if (cost > gc->economy.money) {
            gc_enterOutOfMoneyState(gc);
            return;
        }
        gc->world.tileMap[x][y] =
            tile == match_tile_a ? (int8_t)result_tile_a
            : tile == match_tile_b ? (int8_t)result_tile_b
            : (int8_t)(first_direction ? first_tile : second_tile);
        gc->world.trackMap[x][y] = 65;
    }
    gc->economy.money -= cost;
    gc->economy.totalSpent += cost;
    gc->lastViewportCenterX = -1;
    gc_pushMoneyHistory(gc, -cost);
    gc_incrementRowValueType13(gc, cost);
    gc_advanceConstructionTimers(gc, x, y, -4);
}

void gc_normalizeTrackBits(GameCanvas *gc) {
    for (int x = 0; x < gc->world.mapWidth; ++x) {
        for (int y = 0; y < gc->world.mapHeight; ++y) {
            if (gc->world.trackMap[x][y] == 64) {
                int tile = gc->world.tileMap[x][y];
                gc->world.tileMap[x][y] =
                    tile == 78 ? 22 : tile == 80 ? 24 : tile == 85 ? 9
                    : tile == 84 ? 8 : tile == 77 ? 21 : tile == 79 ? 23
                    : tile == 83 ? 7 : tile == 86 ? 10 : 0;
                gc->world.trackMap[x][y] = 0;
            }
            if (gc->world.tileMap[x][y] != 76) continue;
            int low_owner = gc->world.trackMap[x][y] & 7;
            int high_owner = (gc->world.trackMap[x][y] & 0x3F) >> 3;
            if (low_owner == 0 && high_owner == 0) {
                gc->world.tileMap[x][y] = 0;
                gc->world.trackMap[x][y] = 0;
            } else if (low_owner == 0) {
                gc->world.tileMap[x][y] = 71;
                gc->world.trackMap[x][y] = (int8_t)(0x40 | high_owner);
            } else if (high_owner == 0) {
                gc->world.tileMap[x][y] = 70;
                gc->world.trackMap[x][y] = (int8_t)(0x40 | low_owner);
            }
        }
    }
}

void gc_setTrackOwnerCode(GameCanvas *gc, int32_t owner) {
    for (int x = 0; x < gc->world.mapWidth; ++x) {
        for (int y = 0; y < gc->world.mapHeight; ++y) {
            if (gc->world.trackMap[x][y] == 64) {
                if (owner == 1) gc_advanceConstructionTimers(gc, x, y, -4);
                gc->world.trackMap[x][y] = (int8_t)(0x40 | owner);
            }
            if (gc->world.tileMap[x][y] != 76) continue;
            int low_owner = gc->world.trackMap[x][y] & 7;
            int high_owner = (gc->world.trackMap[x][y] & 0x3F) >> 3;
            if (low_owner == 0) low_owner = owner;
            if (high_owner == 0) high_owner = owner;
            gc->world.trackMap[x][y] =
                (int8_t)(0x40 | low_owner | high_owner << 3);
        }
    }
}

void gc_rebuildMenuTypeIndexes(GameCanvas *gc) {
    gc->menuRowEnabledCount = 0;
    for (int row = 0; row < 5; ++row) {
        if (gc->menuRowType[row] == 0) continue;
        gc->enabledMenuRowIndexes[gc->menuRowEnabledCount++] = (int8_t)row;
    }
}

void gc_openBuildMenu(GameCanvas *gc) {
    int item = gc->buildMenuItems[gc->buildMenuMode][gc->buildMenuItemIndex];
    bool allowed_while_paused =
        (gc->buildMenuMode == 0 && (item == 11 || item == 1 || item == 9))
        || (gc->buildMenuMode == 1 && (item == 1 || item == 3 || item == 9))
        || (gc->buildMenuMode == 3 && item == 9)
        || (gc->buildMenuMode == 5 && item == 9);
    if (gc->pauseFlag && !allowed_while_paused) return;

    switch (gc->buildMenuMode) {
        case 0:
            switch (item) {
                case 0: gc_pickTrainsAtCursor(gc); break;
                case 1:
                    gc->buildMenuMode = 1;
                    gc->menuDirty = true;
                    gc->buildMenuItemIndex = 0;
                    break;
                case 2:
                    gc->ready = false; gc->transitionCode = 30;
                    gc_setState(gc, +GameState::MenuOverlay); break;
                case 3:
                    gc->ready = false; gc->transitionCode = 31;
                    gc_setState(gc, +GameState::MenuOverlay); break;
                case 4:
                    gc_rebuildMenuTypeIndexes(gc);
                    gc->ready = false; gc->transitionCode = 60;
                    gc_setState(gc, +GameState::MenuOverlay); break;
                case 5:
                    gc->ready = false; gc->transitionCode = 17;
                    gc_setState(gc, +GameState::MenuOverlay); break;
                case 8: gc->inputPending = true; break;
                case 9:
                    gc_computeMapTrainMetric(gc, gc->scenario.scenarioStep - 2);
                    gc->useDefaultOverlaySelection = false;
                    gc->ready = false; gc->transitionCode = 1;
                    gc_setState(gc, +GameState::MenuOverlay); break;
                case 11: gc->pauseFlag = !gc->pauseFlag; break;
            }
            break;
        case 1:
            switch (item) {
                case 0:
                    if (gc->blockBuildMenu
                        || !gc_isPlaceableTerrain(gc, gc->tileCursorX,
                                                  gc->tileCursorY))
                        break;
                    gc->pathAnchorX = gc->tileCursorX;
                    gc->pathAnchorY = gc->tileCursorY;
                    gc->world.tileMap[gc->pathAnchorX][gc->pathAnchorY] = 127;
                    gc->buildMenuMode = 4;
                    gc->menuDirty = true;
                    gc->buildMenuItemIndex = 0;
                    break;
                case 1:
                    gc->buildMenuMode = 5;
                    gc->menuDirty = true;
                    gc->buildMenuItemIndex = 0;
                    break;
                case 2:
                    if (gc->blockBuildMenu) break;
                    if (gc->world.tileMap[gc->tileCursorX][gc->tileCursorY] == 7)
                        gc_computeTrackBuildInlined(
                            gc, gc->tileCursorX, gc->tileCursorY,
                            0, -1, 10, 83, 86, 82);
                    if (gc->world.tileMap[gc->tileCursorX][gc->tileCursorY] == 10)
                        gc_computeTrackBuildInlined(
                            gc, gc->tileCursorX, gc->tileCursorY,
                            0, 1, 7, 86, 83, 82);
                    if (gc->world.tileMap[gc->tileCursorX][gc->tileCursorY] == 8)
                        gc_computeTrackBuildInlined(
                            gc, gc->tileCursorX, gc->tileCursorY,
                            -1, 0, 9, 84, 85, 81);
                    if (gc->world.tileMap[gc->tileCursorX][gc->tileCursorY] == 9)
                        gc_computeTrackBuildInlined(
                            gc, gc->tileCursorX, gc->tileCursorY,
                            1, 0, 8, 85, 84, 81);
                    break;
                case 3:
                    gc->buildMenuMode = 3;
                    gc->menuDirty = true;
                    gc->buildMenuItemIndex = 0;
                    break;
                case 4: gc_demolishAtCursor(gc); break;
                case 5:
                    if (!gc->blockBuildMenu)
                        gc_smoothTileAtInlined(gc, gc->swapMapX, gc->swapMapY,
                                               false);
                    break;
                case 6:
                    if (!gc->blockBuildMenu)
                        gc_smoothTileAtInlined(gc, gc->swapMapX, gc->swapMapY,
                                               true);
                    break;
                case 9:
                    gc->buildMenuMode = 0;
                    gc->menuDirty = true;
                    gc->buildMenuItemIndex = 1;
                    break;
            }
            break;
        case 2:
            if (item == 0) {
                if (gc->build.selectionCost <= gc->economy.money) {
                    gc_setTrackOwnerCode(gc, 1);
                    gc->economy.money -= gc->build.selectionCost;
                    gc->economy.totalSpent += gc->build.selectionCost;
                    gc_pushMoneyHistory(gc, -gc->build.selectionCost);
                    gc_incrementRowValueType13(gc, gc->build.selectionCost);
                } else {
                    gc_normalizeTrackBits(gc);
                    gc_enterOutOfMoneyState(gc);
                }
            } else if (item == 9) {
                if (gc->build.buildCancelled) gc_normalizeTrackBits(gc);
                else gc_revertTrackTiles(gc);
            }
            gc->buildMenuMode = 1;
            gc->buildMenuItemIndex = gc->build.buildCancelled ? 0 : 2;
            gc->menuDirty = true;
            gc->lastViewportCenterX = -1;
            break;
        case 3:
            if (item != 9) {
                gc_buildPlaceTrackPiece(gc, item);
                return;
            }
            gc->buildMenuMode = 1;
            gc->menuDirty = true;
            gc->buildMenuItemIndex = 3;
            break;
        case 4:
            if (item == 0) {
                if (gc->blockBuildMenu
                    || !gc_isPlaceableTerrain(gc, gc->tileCursorX,
                                              gc->tileCursorY))
                    break;
                gc->pathTargetX = gc->tileCursorX;
                gc->pathTargetY = gc->tileCursorY;
                gc->world.tileMap[gc->pathAnchorX][gc->pathAnchorY] = 0;
                gc_setState(gc, +GameState::BuildingPath);
            } else if (item == 9) {
                gc->world.tileMap[gc->pathAnchorX][gc->pathAnchorY] = 0;
                gc->buildMenuMode = 1;
                gc->menuDirty = true;
                gc->buildMenuItemIndex = 0;
            }
            break;
        case 5:
            switch (item) {
                case 0:
                    gc_placeTrackAtCursorInlined(
                        gc, gc->vehicleMenuSelectIndex, 75, 74,
                        -1, -1, -1, -1, false, 0);
                    return;
                case 1:
                    gc_placeTrackAtCursorInlined(
                        gc, gc->trainMenuSelectIndex, 72, 73,
                        -1, -1, -1, -1, false, 0);
                    return;
                case 2:
                    gc_placeTrackAtCursorInlined(
                        gc, true, 70, 0, 21, 23, 77, 79, true, 71);
                    return;
                case 3:
                    gc_placeTrackAtCursorInlined(
                        gc, true, 71, 0, 22, 24, 78, 80, true, 70);
                    return;
                case 9:
                    gc->buildMenuMode = 1;
                    gc->menuDirty = true;
                    gc->buildMenuItemIndex = 1;
                    break;
            }
            break;
    }
}

void gc_addSelectedOption(GameCanvas *gc, int32_t option) {
    for (int slot = 0; slot < 5; ++slot) {
        if (gc->build.wagonOptionSlots[slot] != -1) continue;
        gc->build.wagonOptionSlots[slot] = option;
        break;
    }
}

void gc_popSelectedOption(GameCanvas *gc) {
    for (int slot = 4; slot >= 0; --slot) {
        if (gc->build.wagonOptionSlots[slot] == -1) continue;
        gc->build.wagonOptionSlots[slot] = -1;
        break;
    }
}

int32_t gc_allocateTrainSlot(GameCanvas *gc, int32_t x, int32_t y,
                             int32_t engine_type, int32_t era,
                             int32_t direction, int32_t energy) {
    int train = 500;
    for (int slot = 0; slot < 500; ++slot) {
        if ((gc->fleet.trainFlags[slot] & 0x80) == 0) {
            train = slot;
            break;
        }
    }
    if (train != 500) {
        gc->fleet.trainFlags[train] = (int8_t)(0xC0 | engine_type);
        gc->fleet.nextCar[train] = -1;
        gc->fleet.trainX[train] = (int8_t)x;
        gc->fleet.trainY[train] = (int8_t)y;
        gc->fleet.trainDir[train] = (int8_t)direction;
        gc->fleet.wagonEnergyRemaining[train] = (int8_t)energy;
        gc->fleet.trainPhase[train] = 0;
        gc->fleet.trainEnergyOrCount[train] = 1;
        gc->fleet.lastServiceVehicle[train] = -1;
        gc->fleet.trainEraOrStage[train] = (int8_t)era;
        gc->fleet.trainAgeOrTimer[train] = 0;
        gc->fleet.trainStateCode[train] = 7;
        gc->fleet.trainAiOverride[train] = 0;
    }
    gc->lastViewportCenterX = -1;
    return train;
}

void gc_incrementCargoOptionCount(GameCanvas *gc, int32_t car_type) {
    for (int row = 0; row < 5; ++row) {
        if (car_type == 6 && gc->menuRowType[row] == 9)
            ++gc->menuRowValue[row];
        if (car_type == 4 && gc->menuRowType[row] == 10)
            ++gc->menuRowValue[row];
        if (car_type == 3 && gc->menuRowType[row] == 11)
            ++gc->menuRowValue[row];
        if (car_type == 2 && gc->menuRowType[row] == 12)
            ++gc->menuRowValue[row];
    }
}

void gc_decrementCargoOptionCount(GameCanvas *gc, int32_t car_type) {
    for (int row = 0; row < 5; ++row) {
        if ((car_type == 7 || car_type == 6) && gc->menuRowType[row] == 9)
            --gc->menuRowValue[row];
        if ((car_type == 5 || car_type == 4) && gc->menuRowType[row] == 10)
            --gc->menuRowValue[row];
        if (car_type == 3 && gc->menuRowType[row] == 11)
            --gc->menuRowValue[row];
        if (car_type == 2 && gc->menuRowType[row] == 12)
            --gc->menuRowValue[row];
    }
}

void gc_decrementTrainCargoOptionCounts(GameCanvas *gc, int32_t car) {
    if (car == -1) return;
    gc_decrementCargoOptionCount(gc, gc->fleet.trainFlags[car] & 7);
    gc_decrementTrainCargoOptionCounts(gc, gc->fleet.nextCar[car]);
}

void gc_incrementRowValueType14(GameCanvas *gc, int32_t amount) {
    for (int row = 0; row < 5; ++row)
        if (gc->menuRowType[row] == 14) gc->menuRowValue[row] += amount;
}

bool gc_buyTrainAtCursor(GameCanvas *gc) {
    if (gc->economy.money < gc->build.unitCost) {
        gc_enterOutOfMoneyState(gc);
        return false;
    }
    int direction =
        (gc->fleet.vehicleFlags[gc->serviceVehicleAtCursor] & 0x40) == 64 ? 2 : 0;
    int train = gc_allocateTrainSlot(
        gc, gc->tileCursorX, gc->tileCursorY,
        gc->build.wagonType < 3 ? 0 : 1, gc->build.wagonType,
        direction, 8 - gc->build.wagonType);
    for (int option = 0; option < 5; ++option) {
        if (gc->build.wagonOptionSlots[option] < 0) continue;
        int car_type = 2 + gc->build.wagonOptionSlots[option];
        if (car_type == 5) ++car_type;
        train = gc_spawnTrainCar(gc, train, car_type);
        gc_incrementCargoOptionCount(gc, car_type);
    }
    gc->economy.money -= gc->build.unitCost;
    gc->economy.aiMoneySpent += gc->build.unitCost;
    gc_pushMoneyHistory(gc, -gc->build.unitCost);
    ++gc->scenario.aiWagonsBought;
    return true;
}

bool gc_applyTrainUpgradeInlined(GameCanvas *gc, int32_t train,
                                 int32_t state_code, int32_t *options) {
    int cost = gc->build.unitCost - gc->build.currentTrainCostSnapshot;
    if (gc->economy.money < cost) {
        gc_enterOutOfMoneyState(gc);
        return false;
    }
    gc->fleet.trainStateCode[train] = (int16_t)state_code;
    gc_decrementTrainCargoOptionCounts(gc, gc->fleet.nextCar[train]);
    for (int option = 0; option < 5; ++option) {
        if (options[option] > -1) {
            int car_type = options[option] + 2;
            if (car_type == 5) ++car_type;
            gc_incrementCargoOptionCount(gc, car_type);
        }
        gc->fleet.trainStateCode[train] = (int16_t)(
            gc->fleet.trainStateCode[train]
            | (options[option] & 7) << (3 + 3 * option));
    }
    gc->economy.money -= cost;
    gc->economy.aiMoneySpent += cost;
    if (cost != 0) gc_pushMoneyHistory(gc, -cost);
    return true;
}
