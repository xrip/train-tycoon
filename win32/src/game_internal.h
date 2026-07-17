// Calls shared by game-core translation units and behavior tests.
#ifndef GAME_INTERNAL_H
#define GAME_INTERNAL_H
#include "game.h"
#include "support/byte_buffer.h"

#include <optional>
#include <span>
#include <vector>

constexpr int32_t unsignedByte(int8_t value) {
    return static_cast<uint8_t>(value);
}
void game_drawSpriteQuad(Game *game, Surface* graphics, int32_t n, int32_t n2, int32_t n3, int32_t n4);
void game_handleKey(Game *game, int32_t n);
void game_resetKeyState(Game *game, int32_t n);
void game_openSpriteStream(Game *game, const char* resource_name);
std::unique_ptr<Image> game_decodePngFromSpriteStream(Game *game);
void game_resetOverlayRenderState(Game *game);
void game_loadPackedSpriteData(Game *game, const char* resource_name);
std::vector<int8_t> game_readPackedTextCodes(Game *game, int32_t offset);
void game_renderPackedSprite(Game *game, Surface* graphics, int32_t n);
void game_renderFontString(Game *game, Surface* graphics, int32_t n, int32_t n2,
                         std::span<const int8_t> codes, bool bl);
void game_registerCityEntry(Game *game, int8_t by);
bool game_isAdjacentTileEmpty(Game *game, int32_t n, int32_t n2, int32_t n3, bool bl);
void game_smoothTileAt(Game *game, int32_t n, int32_t n2, bool bl);
void game_raiseTerrainArea(Game *game, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5);
void game_lowerTerrainArea(Game *game, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5);
void game_placeTerrainPatch(Game *game, int32_t n, int32_t n2);
void game_trackBuildStep(Game *game, int32_t n, int32_t n2, int32_t n3, int32_t n4);
void game_placeTerrainFeature(Game *game, int32_t n);
void game_viewportPanKey(Game *game);
void game_viewportZoomKey(Game *game);
int32_t game_crossProduct2D(int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5, int32_t n6);
void game_computeIsoCorners(Game *game, int32_t n, int32_t n2, int32_t* nArray, int32_t* nArray2, bool bl);
// Resolve the tile under a map-screen-space point (anchorX/Y) and refresh the
// tile cursor, its screen position, and the selection diamond. anchorX/Y are in
// the same space as viewportBaseX/Y (absolute map-pixel coords, not view-relative).
// The keyboard zoom path passes viewportBase (screen centre); the pointer path
// passes the mouse position translated into map space.
void game_computeViewportBounds(Game *game, int32_t anchorX, int32_t anchorY);
void game_drawSelectionQuad(Game *game, Surface* graphics, int32_t* nArray, int32_t* nArray2);
void game_drawBuildMenuCursor(Game *game, Surface* graphics);
void game_drawBuildMenuRow(Game *game, Surface* graphics);
void game_demolishAtCursor(Game *game);
void game_pickTrainsAtCursor(Game *game);
void game_buildPlaceTrackPiece(Game *game, int32_t n);
void game_openBuildMenu(Game *game);
bool game_spendMoney(Game *game, int32_t n);
void game_computeTrackBuild(Game *game, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5, int32_t n6, int32_t n7, int32_t n8);
void game_revertTrackTiles(Game *game);
void game_placeTrackAtCursor(Game *game, bool bl, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5, int32_t n6, bool bl2, int32_t n7);
void game_queueTileChange(Game *game, int32_t n, int32_t n2, bool bl, bool bl2);
void game_applyBackgroundCost(Game *game);
void game_initBuildMenuRow(Game *game, int32_t n, int32_t n2);
void game_addMenuItem(Game *game, int32_t n, int32_t n2, int32_t n3, bool bl, bool bl2, int32_t n4);
void game_initBuildMenuForScenario(Game *game, int32_t n);
void game_cycleBuildMenuIndex(Game *game, bool bl);
int32_t game_drawMenuRow(Game *game, Surface* graphics, int32_t n, int32_t n2, int32_t n3, bool bl);
void game_drawConfirmBar(Game *game, Surface* graphics);
void game_drawHeaderBar(Game *game, Surface* graphics);
void game_paintMain(Game *game, Surface* graphics);
void game_pushMoneyHistory(Game *game, int32_t n);
int32_t game_drawNumber(Game *game, Surface* graphics, int32_t n, int32_t n2);
void game_drawMoneyTicker(Game *game, Surface* graphics);
void game_drawConfirmDialog(Game *game, Surface* graphics);
void game_placeCityTile(Game *game, int32_t n, int32_t n2, int32_t n3);
bool game_pickValidForestTile(Game *game);
std::vector<int8_t> game_formatCurrency(int32_t value);
std::vector<int8_t> game_formatNumberWithSuffix(int32_t value, int32_t suffix);
std::vector<int8_t> game_encodeDisplayText(const char* text);
Image* game_wagonCarSprite(Game *game, int32_t n, int32_t n2);
bool game_isResourceTile(Game *game, int32_t n, int32_t n2);
std::optional<std::vector<int8_t>> game_resolveDialogValue(Game *game, int32_t n, int32_t n2);
void game_handleBuildMenuState(Game *game, int32_t n, int32_t n2);
bool game_tryBuildPath(Game *game);
void game_validatePathStep(Game *game);
bool game_isTileMatchForPath(Game *game, bool bl, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5, int32_t n6, int32_t n7, int32_t n8, int32_t n9, int32_t n10, bool bl2);
void game_normalizeTrackBits(Game *game);
void game_scanTrackTiles(Game *game);
void game_setTrackOwnerCode(Game *game, int32_t n);
void game_clearPathDistanceGrid(Game *game);
void game_addSelectedOption(Game *game, int32_t n);
void game_popSelectedOption(Game *game);
bool game_buyTrainAtCursor(Game *game);
void game_tickInputOrAi(Game *game, bool bl);
void game_simulationTick(Game *game);
void game_updateTrain(Game *game, int32_t n);
void game_drawSpriteSingle(Game *game, Surface* graphics, int32_t n);
int32_t game_unloadTrainRevenue(Game *game, int32_t n, int32_t n2);
void game_loadTrainAtServiceVehicle(Game *game, int32_t n, int32_t n2, bool bl);
void game_decrementTrainCargoOptionCounts(Game *game, int32_t n);
bool game_applyTrainUpgrade(Game *game, int32_t n, int32_t n2, int32_t* nArray);
void game_persistSnapshotMode0(Game *game);
void game_persistSnapshotMode1(Game *game);
void game_everyTenTick(Game *game);
void game_newYearNotify(Game *game);
int32_t game_scanMapForMetric(Game *game);
void game_setYearGraphMode(Game *game, int32_t n, bool bl);
int32_t game_computeMilestoneScore(Game *game);
void game_checkScenarioMilestone(Game *game);
void game_rebuildMenuTypeIndexes(Game *game);
void game_incrementCargoOptionCount(Game *game, int32_t n);
void game_decrementCargoOptionCount(Game *game, int32_t n);
int32_t game_computeMapTrainMetric(Game *game, int32_t n);
bool game_syncHighScores(Game *game, bool bl);
void game_appendTileDrawOffset(ByteBuffer& x_offsets,
                             ByteBuffer& y_offsets, int32_t x, int32_t y);
void game_buildTileDrawOrder(Game *game, int32_t n, int32_t n2);
void game_drawTextBox(Game *game, Surface* graphics, int32_t n, int32_t n2,
                    int32_t n3, int32_t n4, std::span<const int8_t> codes);
void game_enterOutOfMoneyState(Game *game);
void game_drawSelectedTrainLabel(Game *game, Surface* graphics, int32_t x, int32_t y, int8_t selectionIndex);
void game_setCanvasSize(Game *game, int32_t n, int32_t n2);
void game_setState(Game *game, int32_t n);
void game_renderMap(Game *game, Surface* graphics);

#endif  // GAME_INTERNAL_H
