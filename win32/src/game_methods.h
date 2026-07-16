// Train Tycoon game function declarations.
#ifndef GAME_METHODS_H
#define GAME_METHODS_H
#include "game_state.h"
#include "support/byte_buffer.h"

#include <optional>
#include <span>
#include <vector>

// Owned code rows carry their own length.
inline std::span<const int8_t> game_tracked_codes(const std::vector<int8_t>& codes) {
    return {codes.data(), codes.size()};
}

int32_t game_byteToUnsignedInt(Game *game, int8_t n);
void game_start(Game *game, void* gameMIDlet);
void game_run(Game *game);
void game_pointerPressed(Game *game, int32_t n, int32_t n2);
void game_pointerReleased(Game *game, int32_t n, int32_t n2);
void game_pointerDragged(Game *game, int32_t n, int32_t n2);
void game_keyPressed(Game *game, int32_t n);
void game_keyReleased(Game *game, int32_t n);
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
void game_registerCityEntryInlined(Game *game, int8_t by);
void game_setSelectionStart(Game *game, int32_t n, int32_t n2);
bool game_isAdjacentTileEmpty(Game *game, int32_t n, int32_t n2, int32_t n3, bool bl);
void game_commitTerrainEdit(Game *game);
void game_toggleFlagInlined(Game *game, bool bl);
void game_smoothTileAtInlined(Game *game, int32_t n, int32_t n2, bool bl);
void game_raiseTerrainArea(Game *game, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5);
void game_lowerTerrainArea(Game *game, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5);
void game_loadState25Setup(Game *game, int32_t n);
int8_t game_randomCityTile(Game *game);
void game_placeTerrainPatch(Game *game, int32_t n, int32_t n2);
void game_trackBuildStepInlined(Game *game, int32_t n, int32_t n2, int32_t n3, int32_t n4);
void game_placeTrackTile(Game *game, int32_t n, int32_t n2, int32_t n3, int32_t n4);
void game_placeTerrainFeature(Game *game, int32_t n);
int32_t game_computeKeyRepeatDelay(Game *game);
void game_viewportPanKey(Game *game);
void game_viewportZoomKey(Game *game);
int32_t game_crossProduct2D(Game *game, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5, int32_t n6);
void game_computeIsoCornersInlined(Game *game, int32_t n, int32_t n2, int32_t* nArray, int32_t* nArray2, bool bl);
void game_computeViewportBounds(Game *game);
void game_drawSelectionQuad(Game *game, Surface* graphics, int32_t* nArray, int32_t* nArray2);
void game_drawBuildMenuCursor(Game *game, Surface* graphics);
void game_drawBuildMenuRow(Game *game, Surface* graphics);
void game_demolishAtCursor(Game *game);
void game_pickTrainsAtCursor(Game *game);
void game_buildPlaceTrackPiece(Game *game, int32_t n);
void game_openBuildMenu(Game *game);
bool game_spendMoney(Game *game, int32_t n);
void game_computeTrackBuildInlined(Game *game, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5, int32_t n6, int32_t n7, int32_t n8);
void game_revertTrackTiles(Game *game);
void game_placeTrackAtCursorInlined(Game *game, bool bl, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5, int32_t n6, bool bl2, int32_t n7);
void game_handleCursorClickInlined(Game *game, int32_t n, int32_t n2, bool bl, bool bl2);
void game_applyBackgroundCost(Game *game);
void game_initBuildMenuRow(Game *game, int32_t n, int32_t n2);
void game_addMenuItemInlined(Game *game, int32_t n, int32_t n2, int32_t n3, bool bl, bool bl2, int32_t n4);
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
int8_t game_registerTrain(Game *game, int8_t by, int32_t n, int32_t n2);
void game_dayResetP(Game *game);
void game_advanceConstructionTimers(Game *game, int32_t n, int32_t n2, int32_t n3);
void game_maybeAITick(Game *game);
void game_placeCityTileInlined(Game *game, int32_t n, int32_t n2, int32_t n3);
bool game_pickValidForestTile(Game *game);
int8_t game_addCityEntry(Game *game, int32_t n);
void game_dayResetAg(Game *game);
void game_rebuildCityList(Game *game);
std::vector<int8_t> game_formatCurrency(Game *game, int32_t n);
std::vector<int8_t> game_formatNumberWithSuffix(Game *game, int32_t n, int32_t n2);
std::vector<int8_t> game_encodeDisplayText(Game *game, const char* string);
Image* game_wagonCarSprite(Game *game, int32_t n, int32_t n2);
bool game_isResourceTile(Game *game, int32_t n, int32_t n2);
int32_t game_neighboursMask(Game *game, int32_t n, int32_t n2);
std::optional<std::vector<int8_t>> game_resolveDialogValue(Game *game, int32_t n, int32_t n2);
void game_handleBuildMenuState(Game *game, int32_t n, int32_t n2);
bool game_tryBuildPath(Game *game);
void game_validatePathStep(Game *game);
bool game_isTileMatchForPathInlined(Game *game, bool bl, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5, int32_t n6, int32_t n7, int32_t n8, int32_t n9, int32_t n10, bool bl2);
bool game_findPathStep(Game *game, int32_t n, int32_t n2, int32_t n3);
void game_clearBuildPathTile(Game *game, int32_t n, int32_t n2);
void game_floodFillPath(Game *game);
int32_t game_relaxPathCell(Game *game, int32_t n, int32_t n2);
int16_t game_pathDistanceMin(Game *game, int32_t n, int32_t n2, int32_t n3);
void game_normalizeTrackBits(Game *game);
void game_scanTrackTiles(Game *game);
void game_setTrackOwnerCode(Game *game, int32_t n);
void game_clearPathDistanceGrid(Game *game);
bool game_isTrackStepInlined(Game *game, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5);
bool game_isCurveTile(Game *game, int32_t n, int32_t n2);
bool game_isPlaceableTerrain(Game *game, int32_t n, int32_t n2);
bool game_isCityOrStationTile(Game *game, int32_t n, int32_t n2);
bool game_isTileBuildable(Game *game, int32_t n, int32_t n2, int32_t n3);
int32_t game_findServiceVehicleAt(Game *game, int32_t n, int32_t n2);
int32_t game_allocateServiceVehicleSlot(Game *game, int32_t n, int32_t n2, int32_t n3, bool bl, bool bl2);
void game_computeTrainCost(Game *game);
void game_addSelectedOption(Game *game, int32_t n);
void game_popSelectedOption(Game *game);
bool game_buyTrainAtCursor(Game *game);
void game_tickInputOrAi(Game *game, bool bl);
int32_t game_spawnTrainCar(Game *game, int32_t n, int32_t n2);
int32_t game_allocateTrainSlot(Game *game, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5, int32_t n6);
void game_revenueOrYear(Game *game);
void game_simulationTick(Game *game);
void game_everyTwoTick(Game *game);
bool game_trainCanLoadAtServiceVehicle(Game *game, int32_t n, int32_t n2);
void game_updateTrain(Game *game, int32_t n);
void game_resetTrailingCars(Game *game, int32_t n, int32_t n2, int32_t n3);
int32_t game_countCarsInTrain(Game *game, int32_t n);
int32_t game_accumulateTrainTurns(Game *game, int32_t n);
void game_removeTrainFromMapInlined(Game *game, int32_t n, bool bl);
void game_drawSpriteSingle(Game *game, Surface* graphics, int32_t n);
bool game_isTrainHeadCarAt(Game *game, int32_t n, int32_t n2, int32_t n3);
void game_mergeTrainAt(Game *game, int32_t n);
bool game_isAnyTrainCarAt(Game *game, int32_t n, int32_t n2, int32_t n3);
int32_t game_findTrainHeadAt(Game *game, int32_t n, int32_t n2);
int32_t game_unloadTrainRevenue(Game *game, int32_t n, int32_t n2);
int32_t game_takeServiceResource(Game *game, int32_t n, int32_t n2, int32_t n3);
int32_t game_collectNearbyServiceResource(Game *game, int32_t n, int32_t n2, int32_t n3);
void game_loadTrainAtServiceVehicle(Game *game, int32_t n, int32_t n2, bool bl);
void game_decrementTrainCargoOptionCounts(Game *game, int32_t n);
bool game_applyTrainUpgradeInlined(Game *game, int32_t n, int32_t n2, int32_t* nArray);
void game_loadGame(Game *game, int32_t slot, int32_t mode);
void game_ensureSaveBufferCapacity(Game *game, int32_t n);
int32_t game_readSaveByteUnsigned(Game *game);
void game_saveGame(Game *game);
void game_writeRleRun(Game *game, ByteBuffer& output, int32_t value, int32_t count);
void game_checkSaveByteSum(Game *game, const char* string);
void game_compressSaveBuffer(Game *game);
void game_decompressSaveBuffer(Game *game);
bool game_saveOrLoadBool(Game *game, bool bl);
int8_t game_saveOrLoadByte(Game *game, int8_t by);
int32_t game_saveIntToBuffer(Game *game, int32_t n);
// Explicit-length save/load cores. The std::array, matrix, and Grid overloads
// below forward here; the owned containers carry their own length.
void game_saveOrLoadByteSpan(Game *game, int8_t* byArray, int len);
void game_saveOrLoadShortSpan(Game *game, int16_t* sArray, int len);
void game_saveOrLoadIntSpan(Game *game, int32_t* nArray, int len);
template <std::size_t N>
inline void game_saveOrLoadByteArrayInlined(Game *game, std::array<int8_t, N> &a) {
    game_saveOrLoadByteSpan(game, a.data(), static_cast<int>(N));
}
template <std::size_t N>
inline void game_saveOrLoadShortArrayInlined(Game *game, std::array<int16_t, N> &a) {
    game_saveOrLoadShortSpan(game, a.data(), static_cast<int>(N));
}
template <std::size_t N>
inline void game_saveOrLoadIntArrayInlined(Game *game, std::array<int32_t, N> &a) {
    game_saveOrLoadIntSpan(game, a.data(), static_cast<int>(N));
}
template <std::size_t Rows, std::size_t Cols>
inline void game_saveOrLoadByteMatrixInlined(Game *game, std::array<std::array<int8_t, Cols>, Rows> &m) {
    for (auto &row : m) game_saveOrLoadByteSpan(game, row.data(), static_cast<int>(Cols));
}
template <std::size_t Rows, std::size_t Cols>
inline void game_saveOrLoadShortMatrixInlined(Game *game, std::array<std::array<int16_t, Cols>, Rows> &m) {
    for (auto &row : m) game_saveOrLoadShortSpan(game, row.data(), static_cast<int>(Cols));
}
inline void game_saveOrLoadByteMatrixInlined(Game *game, Grid<int8_t> &m) {
    for (auto &row : m.storage())
        game_saveOrLoadByteSpan(game, row.data(), static_cast<int>(row.size()));
}
void game_initSaveOrLoad(Game *game, int32_t n);
void game_writeSaveSnapshot(Game *game);
void game_persistSnapshotMode0(Game *game);
void game_persistSnapshotMode1(Game *game);
void game_saveAllMapArrays(Game *game);
bool game_loadSnapshotAndReturn(Game *game);
void game_restoreAllMapArrays(Game *game);
void game_everyTenTick(Game *game);
void game_computeYearEndMetrics(Game *game);
void game_newYearNotify(Game *game);
void game_archiveOrRecall(Game *game);
int32_t game_scanMapForMetric(Game *game);
void game_setYearGraphMode(Game *game, int32_t n, bool bl);
void game_maybeAiTakeTurn(Game *game);
void game_placeCitiesAndIndustry(Game *game);
void game_dayResetAd(Game *game);
void game_postRevenueReset(Game *game);
void game_spawnTrainAt(Game *game, int32_t n, int32_t n2);
void game_autoExtendTrackStation(Game *game, int32_t n, int32_t n2);
void game_pickCitySite(Game *game);
void game_pickIndustrySite(Game *game);
int32_t game_computeMilestoneScore(Game *game);
void game_everyTwoTickZ(Game *game);
void game_rebuildMenuTypeIndexes(Game *game);
int8_t game_computeMenuRowDisplayValue(Game *game, int32_t n);
void game_placeTrainAt(Game *game, int32_t n, int32_t n2);
void game_applyMenuRowEffect(Game *game, int32_t n);
void game_duplicateMenuRow(Game *game, int32_t n);
void game_everyMonthlyTick(Game *game);
void game_recordTrainRevenue(Game *game, int32_t n);
void game_recordDeliveredResource(Game *game, int32_t n, int32_t n2);
void game_incrementCargoOptionCount(Game *game, int32_t n);
void game_decrementCargoOptionCount(Game *game, int32_t n);
void game_incrementRowValueType13(Game *game, int32_t n);
void game_incrementRowValueType14(Game *game, int32_t n);
void game_loadMusic(Game *game);
void game_playMusic(Game *game);
void game_setMusicVolume(Game *game, int32_t n);
void game_stopMusic(Game *game);
int32_t game_computeMapTrainMetric(Game *game, int32_t n);
char* game_decodeString(Game *game, int8_t* byArray);
char* game_buildPlayerNameString(Game *game);
bool game_syncHighScores(Game *game, bool bl);
void game_appendTileDrawOffset(Game *game, ByteBuffer& x_offsets,
                             ByteBuffer& y_offsets, int32_t x, int32_t y);
void game_k(Game *game, int32_t n, int32_t n2);
void game_drawTextBox(Game *game, Surface* graphics, int32_t n, int32_t n2,
                    int32_t n3, int32_t n4, std::span<const int8_t> codes);
void game_enterOutOfMoneyState(Game *game);
void game_drawRectFill(Game *game, Surface* graphics, int32_t n, int32_t n2, int8_t by);
void game_setCanvasSize(Game *game, int32_t n, int32_t n2);
void game_paint(Game *game, Surface* graphics);
void game_setState(Game *game, int32_t n);
void game_update(Game *game);
void game_renderMap(Game *game, Surface* graphics);
void game_hideNotify(Game *game);
void game_showNotify(Game *game);

#endif
