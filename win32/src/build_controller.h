// Build actions: laying track, editing terrain, removal, and buying trains
// (see game/build_actions.cpp). Only the action-in-progress state lives here;
// the build/menu display rows stay on Game for UI-01.
#ifndef BUILD_CONTROLLER_H
#define BUILD_CONTROLLER_H
#include "game_constants.h"
#include "support/grid.h"
#include <cstdint>

// BuildController owns the transient state of a build action. Track-laying leaf:
// the walk start/step, the in-progress flag and tile count, the preview/existing
// track owner codes (deferred from PATH-01), the next track piece code, the
// cancel flag, and the running selection cost. nextTrackPieceCode is saved; the
// rest are runtime-only. Save byte order is unchanged.
struct BuildController {
    int32_t trackStartX = 0;
    int32_t trackStartY = 0;
    int32_t trackStepX = 0;
    int32_t trackStepY = 0;
    bool trackBuildInProgress = false;
    int32_t trackTileCount = 0;
    int8_t previewTrackOwnerCode = 0;
    int8_t existingTrackOwnerCode = 1;
    int32_t nextTrackPieceCode = 66;
    bool buildCancelled = true;
    int32_t selectionCost = 0;

    // Train-buy dialog state and the terrain/removal action costs. All
    // runtime-only. wagonOptionSlots/wagonCountMatch hold the offered wagon
    // choices; the *CostSnapshot and unitCost fields track the priced train;
    // terrainSmoothCost, backgroundCost, and demolitionAffordable price the
    // terrain and removal actions. Save byte order is unchanged.
    int32_t wagonType = 0;
    int32_t wagonDiscountType = 0;
    int32_t wagonOptionSlots[kWagonOptionCount]{};
    int32_t wagonCountMatch[kWagonOptionCount]{};
    int32_t unitCost = 0;
    int32_t currentTrainCostSnapshot = 0;
    int32_t baseTrainCostSnapshot = 0;
    bool discountApplied = false;
    bool demolitionAffordable = false;
    int32_t terrainSmoothCost = 0;
    int32_t backgroundCost = 0;

    // Terrain-edit and rectangle-selection state. terrainEditBuffer is the
    // 10x10 scratch grid used while smoothing terrain; selectionStart and
    // selectionTile track the picked rectangle corners; swapMap holds the tile
    // being swapped. All runtime-only. Save byte order is unchanged.
    Grid<int8_t> terrainEditBuffer;
    int32_t selectionStartX = 0;
    int32_t selectionStartY = 0;
    int32_t selectionTileX[kSelectionCornerCount]{};
    int32_t selectionTileY[kSelectionCornerCount]{};
    int32_t swapMapX = 0;
    int32_t swapMapY = 0;

    // Placement direction cursors: which way a junction or station being placed
    // faces. Both runtime-only. Save byte order is unchanged.
    int8_t junctionDirCursor = 0;
    int8_t stationDirCursor = 0;
};

#endif  // BUILD_CONTROLLER_H
