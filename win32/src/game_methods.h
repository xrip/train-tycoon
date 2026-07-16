// Train Tycoon game function declarations.
#ifndef GAME_METHODS_H
#define GAME_METHODS_H
#include "game_state.h"
#include "support/byte_buffer.h"

#include <optional>
#include <span>
#include <vector>

// Owned code rows carry their own length.
inline std::span<const int8_t> gc_tracked_codes(const std::vector<int8_t>& codes) {
    return {codes.data(), codes.size()};
}

int32_t gc_byteToUnsignedInt(Game *gc, int8_t n);
void gc_start(Game *gc, void* gameMIDlet);
void gc_run(Game *gc);
void gc_pointerPressed(Game *gc, int32_t n, int32_t n2);
void gc_pointerReleased(Game *gc, int32_t n, int32_t n2);
void gc_pointerDragged(Game *gc, int32_t n, int32_t n2);
void gc_keyPressed(Game *gc, int32_t n);
void gc_keyReleased(Game *gc, int32_t n);
void gc_drawSpriteQuad(Game *gc, Surface* graphics, int32_t n, int32_t n2, int32_t n3, int32_t n4);
void gc_handleKey(Game *gc, int32_t n);
void gc_resetKeyState(Game *gc, int32_t n);
void gc_openSpriteStream(Game *gc, const char* resource_name);
std::unique_ptr<Image> gc_decodePngFromSpriteStream(Game *gc);
void gc_resetOverlayRenderState(Game *gc);
void gc_loadPackedSpriteData(Game *gc, const char* resource_name);
std::vector<int8_t> gc_readPackedTextCodes(Game *gc, int32_t offset);
void gc_renderPackedSprite(Game *gc, Surface* graphics, int32_t n);
void gc_renderFontString(Game *gc, Surface* graphics, int32_t n, int32_t n2,
                         std::span<const int8_t> codes, bool bl);
void gc_registerCityEntryInlined(Game *gc, int8_t by);
void gc_setSelectionStart(Game *gc, int32_t n, int32_t n2);
bool gc_isAdjacentTileEmpty(Game *gc, int32_t n, int32_t n2, int32_t n3, bool bl);
void gc_commitTerrainEdit(Game *gc);
void gc_toggleFlagInlined(Game *gc, bool bl);
void gc_smoothTileAtInlined(Game *gc, int32_t n, int32_t n2, bool bl);
void gc_raiseTerrainArea(Game *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5);
void gc_lowerTerrainArea(Game *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5);
void gc_loadState25Setup(Game *gc, int32_t n);
int8_t gc_randomCityTile(Game *gc);
void gc_placeTerrainPatch(Game *gc, int32_t n, int32_t n2);
void gc_trackBuildStepInlined(Game *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4);
void gc_placeTrackTile(Game *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4);
void gc_placeTerrainFeature(Game *gc, int32_t n);
int32_t gc_computeKeyRepeatDelay(Game *gc);
void gc_viewportPanKey(Game *gc);
void gc_viewportZoomKey(Game *gc);
int32_t gc_crossProduct2D(Game *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5, int32_t n6);
void gc_computeIsoCornersInlined(Game *gc, int32_t n, int32_t n2, int32_t* nArray, int32_t* nArray2, bool bl);
void gc_computeViewportBounds(Game *gc);
void gc_drawSelectionQuad(Game *gc, Surface* graphics, int32_t* nArray, int32_t* nArray2);
void gc_drawBuildMenuCursor(Game *gc, Surface* graphics);
void gc_drawBuildMenuRow(Game *gc, Surface* graphics);
void gc_demolishAtCursor(Game *gc);
void gc_pickTrainsAtCursor(Game *gc);
void gc_buildPlaceTrackPiece(Game *gc, int32_t n);
void gc_openBuildMenu(Game *gc);
bool gc_spendMoney(Game *gc, int32_t n);
void gc_computeTrackBuildInlined(Game *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5, int32_t n6, int32_t n7, int32_t n8);
void gc_revertTrackTiles(Game *gc);
void gc_placeTrackAtCursorInlined(Game *gc, bool bl, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5, int32_t n6, bool bl2, int32_t n7);
void gc_handleCursorClickInlined(Game *gc, int32_t n, int32_t n2, bool bl, bool bl2);
void gc_applyBackgroundCost(Game *gc);
void gc_initBuildMenuRow(Game *gc, int32_t n, int32_t n2);
void gc_addMenuItemInlined(Game *gc, int32_t n, int32_t n2, int32_t n3, bool bl, bool bl2, int32_t n4);
void gc_initBuildMenuForScenario(Game *gc, int32_t n);
void gc_cycleBuildMenuIndex(Game *gc, bool bl);
int32_t gc_drawMenuRow(Game *gc, Surface* graphics, int32_t n, int32_t n2, int32_t n3, bool bl);
void gc_drawConfirmBar(Game *gc, Surface* graphics);
void gc_drawHeaderBar(Game *gc, Surface* graphics);
void gc_paintMain(Game *gc, Surface* graphics);
void gc_pushMoneyHistory(Game *gc, int32_t n);
int32_t gc_drawNumber(Game *gc, Surface* graphics, int32_t n, int32_t n2);
void gc_drawMoneyTicker(Game *gc, Surface* graphics);
void gc_drawConfirmDialog(Game *gc, Surface* graphics);
int8_t gc_registerTrain(Game *gc, int8_t by, int32_t n, int32_t n2);
void gc_dayResetP(Game *gc);
void gc_advanceConstructionTimers(Game *gc, int32_t n, int32_t n2, int32_t n3);
void gc_maybeAITick(Game *gc);
void gc_placeCityTileInlined(Game *gc, int32_t n, int32_t n2, int32_t n3);
bool gc_pickValidForestTile(Game *gc);
int8_t gc_addCityEntry(Game *gc, int32_t n);
void gc_dayResetAg(Game *gc);
void gc_rebuildCityList(Game *gc);
std::vector<int8_t> gc_formatCurrency(Game *gc, int32_t n);
std::vector<int8_t> gc_formatNumberWithSuffix(Game *gc, int32_t n, int32_t n2);
std::vector<int8_t> gc_encodeDisplayText(Game *gc, const char* string);
Image* gc_wagonCarSprite(Game *gc, int32_t n, int32_t n2);
bool gc_isResourceTile(Game *gc, int32_t n, int32_t n2);
int32_t gc_neighboursMask(Game *gc, int32_t n, int32_t n2);
std::optional<std::vector<int8_t>> gc_resolveDialogValue(Game *gc, int32_t n, int32_t n2);
void gc_handleBuildMenuState(Game *gc, int32_t n, int32_t n2);
bool gc_tryBuildPath(Game *gc);
void gc_validatePathStep(Game *gc);
bool gc_isTileMatchForPathInlined(Game *gc, bool bl, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5, int32_t n6, int32_t n7, int32_t n8, int32_t n9, int32_t n10, bool bl2);
bool gc_findPathStep(Game *gc, int32_t n, int32_t n2, int32_t n3);
void gc_clearBuildPathTile(Game *gc, int32_t n, int32_t n2);
void gc_floodFillPath(Game *gc);
int32_t gc_relaxPathCell(Game *gc, int32_t n, int32_t n2);
int16_t gc_pathDistanceMin(Game *gc, int32_t n, int32_t n2, int32_t n3);
void gc_normalizeTrackBits(Game *gc);
void gc_scanTrackTiles(Game *gc);
void gc_setTrackOwnerCode(Game *gc, int32_t n);
void gc_clearPathDistanceGrid(Game *gc);
bool gc_isTrackStepInlined(Game *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5);
bool gc_isCurveTile(Game *gc, int32_t n, int32_t n2);
bool gc_isPlaceableTerrain(Game *gc, int32_t n, int32_t n2);
bool gc_isCityOrStationTile(Game *gc, int32_t n, int32_t n2);
bool gc_isTileBuildable(Game *gc, int32_t n, int32_t n2, int32_t n3);
int32_t gc_findServiceVehicleAt(Game *gc, int32_t n, int32_t n2);
int32_t gc_allocateServiceVehicleSlot(Game *gc, int32_t n, int32_t n2, int32_t n3, bool bl, bool bl2);
void gc_computeTrainCost(Game *gc);
void gc_addSelectedOption(Game *gc, int32_t n);
void gc_popSelectedOption(Game *gc);
bool gc_buyTrainAtCursor(Game *gc);
void gc_tickInputOrAi(Game *gc, bool bl);
int32_t gc_spawnTrainCar(Game *gc, int32_t n, int32_t n2);
int32_t gc_allocateTrainSlot(Game *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5, int32_t n6);
void gc_revenueOrYear(Game *gc);
void gc_simulationTick(Game *gc);
void gc_everyTwoTick(Game *gc);
bool gc_trainCanLoadAtServiceVehicle(Game *gc, int32_t n, int32_t n2);
void gc_updateTrain(Game *gc, int32_t n);
void gc_resetTrailingCars(Game *gc, int32_t n, int32_t n2, int32_t n3);
int32_t gc_countCarsInTrain(Game *gc, int32_t n);
int32_t gc_accumulateTrainTurns(Game *gc, int32_t n);
void gc_removeTrainFromMapInlined(Game *gc, int32_t n, bool bl);
void gc_drawSpriteSingle(Game *gc, Surface* graphics, int32_t n);
bool gc_isTrainHeadCarAt(Game *gc, int32_t n, int32_t n2, int32_t n3);
void gc_mergeTrainAt(Game *gc, int32_t n);
bool gc_isAnyTrainCarAt(Game *gc, int32_t n, int32_t n2, int32_t n3);
int32_t gc_findTrainHeadAt(Game *gc, int32_t n, int32_t n2);
int32_t gc_unloadTrainRevenue(Game *gc, int32_t n, int32_t n2);
int32_t gc_takeServiceResource(Game *gc, int32_t n, int32_t n2, int32_t n3);
int32_t gc_collectNearbyServiceResource(Game *gc, int32_t n, int32_t n2, int32_t n3);
void gc_loadTrainAtServiceVehicle(Game *gc, int32_t n, int32_t n2, bool bl);
void gc_decrementTrainCargoOptionCounts(Game *gc, int32_t n);
bool gc_applyTrainUpgradeInlined(Game *gc, int32_t n, int32_t n2, int32_t* nArray);
void gc_loadGame(Game *gc, int32_t slot, int32_t mode);
void gc_ensureSaveBufferCapacity(Game *gc, int32_t n);
int32_t gc_readSaveByteUnsigned(Game *gc);
void gc_saveGame(Game *gc);
void gc_writeRleRun(Game *gc, ByteBuffer& output, int32_t value, int32_t count);
void gc_checkSaveByteSum(Game *gc, const char* string);
void gc_compressSaveBuffer(Game *gc);
void gc_decompressSaveBuffer(Game *gc);
bool gc_saveOrLoadBool(Game *gc, bool bl);
int8_t gc_saveOrLoadByte(Game *gc, int8_t by);
int32_t gc_saveIntToBuffer(Game *gc, int32_t n);
// Explicit-length save/load cores. The std::array, matrix, and Grid overloads
// below forward here; the owned containers carry their own length.
void gc_saveOrLoadByteSpan(Game *gc, int8_t* byArray, int len);
void gc_saveOrLoadShortSpan(Game *gc, int16_t* sArray, int len);
void gc_saveOrLoadIntSpan(Game *gc, int32_t* nArray, int len);
template <std::size_t N>
inline void gc_saveOrLoadByteArrayInlined(Game *gc, std::array<int8_t, N> &a) {
    gc_saveOrLoadByteSpan(gc, a.data(), static_cast<int>(N));
}
template <std::size_t N>
inline void gc_saveOrLoadShortArrayInlined(Game *gc, std::array<int16_t, N> &a) {
    gc_saveOrLoadShortSpan(gc, a.data(), static_cast<int>(N));
}
template <std::size_t N>
inline void gc_saveOrLoadIntArrayInlined(Game *gc, std::array<int32_t, N> &a) {
    gc_saveOrLoadIntSpan(gc, a.data(), static_cast<int>(N));
}
template <std::size_t Rows, std::size_t Cols>
inline void gc_saveOrLoadByteMatrixInlined(Game *gc, std::array<std::array<int8_t, Cols>, Rows> &m) {
    for (auto &row : m) gc_saveOrLoadByteSpan(gc, row.data(), static_cast<int>(Cols));
}
template <std::size_t Rows, std::size_t Cols>
inline void gc_saveOrLoadShortMatrixInlined(Game *gc, std::array<std::array<int16_t, Cols>, Rows> &m) {
    for (auto &row : m) gc_saveOrLoadShortSpan(gc, row.data(), static_cast<int>(Cols));
}
inline void gc_saveOrLoadByteMatrixInlined(Game *gc, Grid<int8_t> &m) {
    for (auto &row : m.storage())
        gc_saveOrLoadByteSpan(gc, row.data(), static_cast<int>(row.size()));
}
void gc_initSaveOrLoad(Game *gc, int32_t n);
void gc_writeSaveSnapshot(Game *gc);
void gc_persistSnapshotMode0(Game *gc);
void gc_persistSnapshotMode1(Game *gc);
void gc_saveAllMapArrays(Game *gc);
bool gc_loadSnapshotAndReturn(Game *gc);
void gc_restoreAllMapArrays(Game *gc);
void gc_everyTenTick(Game *gc);
void gc_computeYearEndMetrics(Game *gc);
void gc_newYearNotify(Game *gc);
void gc_archiveOrRecall(Game *gc);
int32_t gc_scanMapForMetric(Game *gc);
void gc_setYearGraphMode(Game *gc, int32_t n, bool bl);
void gc_maybeAiTakeTurn(Game *gc);
void gc_placeCitiesAndIndustry(Game *gc);
void gc_dayResetAd(Game *gc);
void gc_postRevenueReset(Game *gc);
void gc_spawnTrainAt(Game *gc, int32_t n, int32_t n2);
void gc_autoExtendTrackStation(Game *gc, int32_t n, int32_t n2);
void gc_pickCitySite(Game *gc);
void gc_pickIndustrySite(Game *gc);
int32_t gc_computeMilestoneScore(Game *gc);
void gc_everyTwoTickZ(Game *gc);
void gc_rebuildMenuTypeIndexes(Game *gc);
int8_t gc_computeMenuRowDisplayValue(Game *gc, int32_t n);
void gc_placeTrainAt(Game *gc, int32_t n, int32_t n2);
void gc_applyMenuRowEffect(Game *gc, int32_t n);
void gc_duplicateMenuRow(Game *gc, int32_t n);
void gc_everyMonthlyTick(Game *gc);
void gc_recordTrainRevenue(Game *gc, int32_t n);
void gc_recordDeliveredResource(Game *gc, int32_t n, int32_t n2);
void gc_incrementCargoOptionCount(Game *gc, int32_t n);
void gc_decrementCargoOptionCount(Game *gc, int32_t n);
void gc_incrementRowValueType13(Game *gc, int32_t n);
void gc_incrementRowValueType14(Game *gc, int32_t n);
void gc_loadMusic(Game *gc);
void gc_playMusic(Game *gc);
void gc_setMusicVolume(Game *gc, int32_t n);
void gc_stopMusic(Game *gc);
int32_t gc_computeMapTrainMetric(Game *gc, int32_t n);
char* gc_decodeString(Game *gc, int8_t* byArray);
char* gc_buildPlayerNameString(Game *gc);
bool gc_syncHighScores(Game *gc, bool bl);
void gc_appendTileDrawOffset(Game *gc, ByteBuffer& x_offsets,
                             ByteBuffer& y_offsets, int32_t x, int32_t y);
void gc_k(Game *gc, int32_t n, int32_t n2);
void gc_drawTextBox(Game *gc, Surface* graphics, int32_t n, int32_t n2,
                    int32_t n3, int32_t n4, std::span<const int8_t> codes);
void gc_enterOutOfMoneyState(Game *gc);
void gc_drawRectFill(Game *gc, Surface* graphics, int32_t n, int32_t n2, int8_t by);
void gc_setCanvasSize(Game *gc, int32_t n, int32_t n2);
void gc_paint(Game *gc, Surface* graphics);
void gc_setState(Game *gc, int32_t n);
void gc_update(Game *gc);
void gc_renderMap(Game *gc, Surface* graphics);
void gc_hideNotify(Game *gc);
void gc_showNotify(Game *gc);

#endif
