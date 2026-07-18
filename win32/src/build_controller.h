// Build actions: laying track, editing terrain, removal, and buying trains
// (see game/build_actions.cpp). Only the action-in-progress state lives here;
// the build/menu display rows stay on Game for UI-01.
#ifndef BUILD_CONTROLLER_H
#define BUILD_CONTROLLER_H
#include "game_constants.h"
#include "support/grid.h"
#include <cstdint>

struct Economy;
struct Scenario;
class Terrain;

// One track-piece placement rule. These values describe the legacy tile
// transitions used by the build tool; they are runtime UI data, not save data.
struct TrackPlacementSpec {
    int32_t firstTileCode;
    int32_t secondTileCode;
    int32_t firstMatchTileCode;
    int32_t secondMatchTileCode;
    int32_t firstResultTileCode;
    int32_t secondResultTileCode;
    bool joinsExistingTrack;
    int32_t joinedTrackTileCode;
};

// BuildController owns the transient state of a build action. Track-laying leaf:
// the walk start/step, the in-progress flag and tile count, the preview/existing
// track owner codes (deferred from PATH-01), the next track piece code, the
// cancel flag, and the running selection cost. nextTrackPieceCode is saved; the
// rest are runtime-only. Save byte order is unchanged.
struct BuildController {
    static bool tool_acts_on_cursor_tile(int32_t menu_mode,
                                         int32_t menu_item_id);
    void compute_train_cost(const Economy& economy, const Scenario& scenario);
    void set_terrain_selection(const Terrain& terrain, int32_t center_x,
                               int32_t center_y);
    bool adjust_terrain_point(const Terrain& terrain, const Economy& economy,
                              int32_t x, int32_t y, int32_t height,
                              bool raising);
    void queue_tile_change(Terrain& terrain, const Economy& economy,
                           int32_t x, int32_t y, bool charge,
                           bool visual_only);

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
    // runtime-only. wagonOptionSlots holds the offered wagon choices and
    // matchingWagonOptions holds the prior choices used for discount matching;
    // the *CostSnapshot and unitCost fields track the priced train;
    // terrainSmoothCost, backgroundCost, and demolitionAffordable price the
    // terrain and removal actions. Save byte order is unchanged.
    int32_t wagonType = 0;
    int32_t wagonDiscountType = 0;
    int32_t wagonOptionSlots[kWagonOptionCount]{};
    int32_t matchingWagonOptions[kWagonOptionCount]{};
    int32_t unitCost = 0;
    int32_t currentTrainCostSnapshot = 0;
    int32_t baseTrainCostSnapshot = 0;
    bool discountApplied = false;
    bool demolitionAffordable = false;
    int32_t terrainSmoothCost = 0;
    int32_t backgroundCost = 0;

    // Terrain-edit and rectangle-selection state. terrainEditBuffer is the
    // fixed-size scratch grid used while smoothing terrain; selectionStart and
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

    // Road-stop buy dialog: the exact road-stop slot under the cursor when the
    // dialog was opened. Captured at open time because the tile cursor
    // drifts back to the screen centre while the dialog is up (the MenuOverlay
    // bounces through Playing between ticks). Runtime-only; save byte order is
    // unchanged.
    int32_t roadStopCursor = -1;

    // Road auto-route tool: the anchor tile captured on the first map click
    // (-1 = no anchor yet). The second click calls auto_route_road(anchor,
    // target). Reset whenever any other build tool is chosen, which cancels the
    // pending route. Runtime-only; save byte order is unchanged.
    int32_t roadAutoAnchorX = -1;
    int32_t roadAutoAnchorY = -1;
};

#endif  // BUILD_CONTROLLER_H
