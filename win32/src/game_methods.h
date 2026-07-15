// Train Tycoon game function declarations.
#ifndef GAME_METHODS_H
#define GAME_METHODS_H
#include "game_state.h"
#include "support/byte_buffer.h"

#include <optional>
#include <span>
#include <vector>

inline std::span<const int8_t> gc_tracked_codes(const int8_t* codes) {
    return {codes, static_cast<size_t>(midp_arr_len(codes))};
}
// Owned code rows carry their own length.
inline std::span<const int8_t> gc_tracked_codes(const std::vector<int8_t>& codes) {
    return {codes.data(), codes.size()};
}

int32_t gc_byteToUnsignedInt(GameCanvas *gc, int8_t n);
void gc_start(GameCanvas *gc, void* gameMIDlet);
void gc_run(GameCanvas *gc);
void gc_pointerPressed(GameCanvas *gc, int32_t n, int32_t n2);
void gc_pointerReleased(GameCanvas *gc, int32_t n, int32_t n2);
void gc_pointerDragged(GameCanvas *gc, int32_t n, int32_t n2);
void gc_keyPressed(GameCanvas *gc, int32_t n);
void gc_keyReleased(GameCanvas *gc, int32_t n);
void gc_drawSpriteQuad(GameCanvas *gc, Surface* graphics, int32_t n, int32_t n2, int32_t n3, int32_t n4);
void gc_handleKey(GameCanvas *gc, int32_t n);
void gc_resetKeyState(GameCanvas *gc, int32_t n);
void gc_openSpriteStream(GameCanvas *gc, const char* resource_name);
std::unique_ptr<Image> gc_decodePngFromSpriteStream(GameCanvas *gc);
void gc_resetOverlayRenderState(GameCanvas *gc);
void gc_loadPackedSpriteData(GameCanvas *gc, const char* resource_name);
std::vector<int8_t> gc_readPackedTextCodes(GameCanvas *gc, int32_t offset);
void gc_renderPackedSprite(GameCanvas *gc, Surface* graphics, int32_t n);
void gc_renderFontString(GameCanvas *gc, Surface* graphics, int32_t n, int32_t n2,
                         std::span<const int8_t> codes, bool bl);
void gc_registerCityEntryInlined(GameCanvas *gc, int8_t by);
void gc_setSelectionStart(GameCanvas *gc, int32_t n, int32_t n2);
bool gc_isAdjacentTileEmpty(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3, bool bl);
void gc_commitTerrainEdit(GameCanvas *gc);
void gc_toggleFlagInlined(GameCanvas *gc, bool bl);
void gc_smoothTileAtInlined(GameCanvas *gc, int32_t n, int32_t n2, bool bl);
void gc_raiseTerrainArea(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5);
void gc_lowerTerrainArea(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5);
void gc_loadState25Setup(GameCanvas *gc, int32_t n);
int8_t gc_randomCityTile(GameCanvas *gc);
void gc_placeTerrainPatch(GameCanvas *gc, int32_t n, int32_t n2);
void gc_trackBuildStepInlined(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4);
void gc_placeTrackTile(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4);
void gc_placeTerrainFeature(GameCanvas *gc, int32_t n);
int32_t gc_computeKeyRepeatDelay(GameCanvas *gc);
void gc_viewportPanKey(GameCanvas *gc);
void gc_viewportZoomKey(GameCanvas *gc);
int32_t gc_crossProduct2D(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5, int32_t n6);
void gc_computeIsoCornersInlined(GameCanvas *gc, int32_t n, int32_t n2, int32_t* nArray, int32_t* nArray2, bool bl);
void gc_computeViewportBounds(GameCanvas *gc);
void gc_drawSelectionQuad(GameCanvas *gc, Surface* graphics, int32_t* nArray, int32_t* nArray2);
void gc_drawBuildMenuCursor(GameCanvas *gc, Surface* graphics);
void gc_drawBuildMenuRow(GameCanvas *gc, Surface* graphics);
void gc_demolishAtCursor(GameCanvas *gc);
void gc_pickTrainsAtCursor(GameCanvas *gc);
void gc_buildPlaceTrackPiece(GameCanvas *gc, int32_t n);
void gc_openBuildMenu(GameCanvas *gc);
bool gc_spendMoney(GameCanvas *gc, int32_t n);
void gc_computeTrackBuildInlined(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5, int32_t n6, int32_t n7, int32_t n8);
void gc_revertTrackTiles(GameCanvas *gc);
void gc_placeTrackAtCursorInlined(GameCanvas *gc, bool bl, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5, int32_t n6, bool bl2, int32_t n7);
void gc_handleCursorClickInlined(GameCanvas *gc, int32_t n, int32_t n2, bool bl, bool bl2);
void gc_applyBackgroundCost(GameCanvas *gc);
void gc_initBuildMenuRow(GameCanvas *gc, int32_t n, int32_t n2);
void gc_addMenuItemInlined(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3, bool bl, bool bl2, int32_t n4);
void gc_initBuildMenuForScenario(GameCanvas *gc, int32_t n);
void gc_cycleBuildMenuIndex(GameCanvas *gc, bool bl);
int32_t gc_drawMenuRow(GameCanvas *gc, Surface* graphics, int32_t n, int32_t n2, int32_t n3, bool bl);
void gc_drawConfirmBar(GameCanvas *gc, Surface* graphics);
void gc_drawHeaderBar(GameCanvas *gc, Surface* graphics);
void gc_paintMain(GameCanvas *gc, Surface* graphics);
void gc_pushMoneyHistory(GameCanvas *gc, int32_t n);
int32_t gc_drawNumber(GameCanvas *gc, Surface* graphics, int32_t n, int32_t n2);
void gc_drawMoneyTicker(GameCanvas *gc, Surface* graphics);
void gc_drawConfirmDialog(GameCanvas *gc, Surface* graphics);
int8_t gc_registerTrain(GameCanvas *gc, int8_t by, int32_t n, int32_t n2);
void gc_dayResetP(GameCanvas *gc);
void gc_advanceConstructionTimers(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3);
void gc_maybeAITick(GameCanvas *gc);
void gc_placeCityTileInlined(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3);
bool gc_pickValidForestTile(GameCanvas *gc);
int8_t gc_addCityEntry(GameCanvas *gc, int32_t n);
void gc_dayResetAg(GameCanvas *gc);
void gc_rebuildCityList(GameCanvas *gc);
std::vector<int8_t> gc_formatCurrency(GameCanvas *gc, int32_t n);
std::vector<int8_t> gc_formatNumberWithSuffix(GameCanvas *gc, int32_t n, int32_t n2);
std::vector<int8_t> gc_encodeDisplayText(GameCanvas *gc, const char* string);
Image* gc_u(GameCanvas *gc, int32_t n, int32_t n2);
bool gc_isResourceTile(GameCanvas *gc, int32_t n, int32_t n2);
int32_t gc_neighboursMask(GameCanvas *gc, int32_t n, int32_t n2);
std::optional<std::vector<int8_t>> gc_resolveDialogValue(GameCanvas *gc, int32_t n, int32_t n2);
void gc_handleBuildMenuState(GameCanvas *gc, int32_t n, int32_t n2);
bool gc_tryBuildPath(GameCanvas *gc);
void gc_validatePathStep(GameCanvas *gc);
bool gc_isTileMatchForPathInlined(GameCanvas *gc, bool bl, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5, int32_t n6, int32_t n7, int32_t n8, int32_t n9, int32_t n10, bool bl2);
bool gc_findPathStep(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3);
void gc_clearBuildPathTile(GameCanvas *gc, int32_t n, int32_t n2);
void gc_floodFillPath(GameCanvas *gc);
int32_t gc_relaxPathCell(GameCanvas *gc, int32_t n, int32_t n2);
int16_t gc_pathDistanceMin(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3);
void gc_normalizeTrackBits(GameCanvas *gc);
void gc_scanTrackTiles(GameCanvas *gc);
void gc_setTrackOwnerCode(GameCanvas *gc, int32_t n);
void gc_clearPathDistanceGrid(GameCanvas *gc);
bool gc_isTrackStepInlined(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5);
bool gc_isCurveTile(GameCanvas *gc, int32_t n, int32_t n2);
bool gc_isPlaceableTerrain(GameCanvas *gc, int32_t n, int32_t n2);
bool gc_isCityOrStationTile(GameCanvas *gc, int32_t n, int32_t n2);
bool gc_isTileBuildable(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3);
int32_t gc_findServiceVehicleAt(GameCanvas *gc, int32_t n, int32_t n2);
int32_t gc_allocateServiceVehicleSlot(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3, bool bl, bool bl2);
void gc_computeTrainCost(GameCanvas *gc);
void gc_addSelectedOption(GameCanvas *gc, int32_t n);
void gc_popSelectedOption(GameCanvas *gc);
bool gc_buyTrainAtCursor(GameCanvas *gc);
void gc_tickInputOrAi(GameCanvas *gc, bool bl);
int32_t gc_spawnTrainCar(GameCanvas *gc, int32_t n, int32_t n2);
int32_t gc_allocateTrainSlot(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3, int32_t n4, int32_t n5, int32_t n6);
void gc_revenueOrYear(GameCanvas *gc);
void gc_everyTwoTick(GameCanvas *gc);
bool gc_trainCanLoadAtServiceVehicle(GameCanvas *gc, int32_t n, int32_t n2);
void gc_updateTrain(GameCanvas *gc, int32_t n);
void gc_resetTrailingCars(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3);
int32_t gc_countCarsInTrain(GameCanvas *gc, int32_t n);
int32_t gc_accumulateTrainTurns(GameCanvas *gc, int32_t n);
void gc_removeTrainFromMapInlined(GameCanvas *gc, int32_t n, bool bl);
void gc_drawSpriteSingle(GameCanvas *gc, Surface* graphics, int32_t n);
bool gc_isTrainHeadCarAt(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3);
void gc_mergeTrainAt(GameCanvas *gc, int32_t n);
bool gc_isAnyTrainCarAt(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3);
int32_t gc_findTrainHeadAt(GameCanvas *gc, int32_t n, int32_t n2);
int32_t gc_unloadTrainRevenue(GameCanvas *gc, int32_t n, int32_t n2);
int32_t gc_takeServiceResource(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3);
int32_t gc_collectNearbyServiceResource(GameCanvas *gc, int32_t n, int32_t n2, int32_t n3);
void gc_loadTrainAtServiceVehicle(GameCanvas *gc, int32_t n, int32_t n2, bool bl);
void gc_decrementTrainCargoOptionCounts(GameCanvas *gc, int32_t n);
bool gc_applyTrainUpgradeInlined(GameCanvas *gc, int32_t n, int32_t n2, int32_t* nArray);
void gc_loadGame(GameCanvas *gc, int32_t n, int32_t n2);
void gc_ensureSaveBufferCapacity(GameCanvas *gc, int32_t n);
int32_t gc_readShortFromBuffer(GameCanvas *gc);
void gc_saveGame(GameCanvas *gc);
void gc_writeRleRun(GameCanvas *gc, ByteBuffer& output, int32_t value, int32_t count);
void gc_checkSaveByteSum(GameCanvas *gc, const char* string);
void gc_compressSaveBuffer(GameCanvas *gc);
void gc_decompressSaveBuffer(GameCanvas *gc);
bool gc_saveOrLoadBool(GameCanvas *gc, bool bl);
int8_t gc_saveOrLoadByte(GameCanvas *gc, int8_t by);
int32_t gc_saveIntToBuffer(GameCanvas *gc, int32_t n);
void gc_saveOrLoadByteMatrixInlined(GameCanvas *gc, int8_t** byArray);
void gc_saveOrLoadShortMatrixInlined(GameCanvas *gc, int16_t** sArray);
void gc_saveOrLoadByteArrayInlined(GameCanvas *gc, int8_t* byArray);
void gc_saveOrLoadShortArrayInlined(GameCanvas *gc, int16_t* sArray);
void gc_saveOrLoadIntArrayInlined(GameCanvas *gc, int32_t* nArray);
// Explicit-length save/load: used for owned std::array state that has no
// entry in the array-length registry. Same byte order as the raw versions.
void gc_saveOrLoadByteSpan(GameCanvas *gc, int8_t* byArray, int len);
void gc_saveOrLoadShortSpan(GameCanvas *gc, int16_t* sArray, int len);
void gc_saveOrLoadIntSpan(GameCanvas *gc, int32_t* nArray, int len);
template <std::size_t N>
inline void gc_saveOrLoadByteArrayInlined(GameCanvas *gc, std::array<int8_t, N> &a) {
    gc_saveOrLoadByteSpan(gc, a.data(), static_cast<int>(N));
}
template <std::size_t N>
inline void gc_saveOrLoadShortArrayInlined(GameCanvas *gc, std::array<int16_t, N> &a) {
    gc_saveOrLoadShortSpan(gc, a.data(), static_cast<int>(N));
}
template <std::size_t N>
inline void gc_saveOrLoadIntArrayInlined(GameCanvas *gc, std::array<int32_t, N> &a) {
    gc_saveOrLoadIntSpan(gc, a.data(), static_cast<int>(N));
}
template <std::size_t Rows, std::size_t Cols>
inline void gc_saveOrLoadByteMatrixInlined(GameCanvas *gc, std::array<std::array<int8_t, Cols>, Rows> &m) {
    for (auto &row : m) gc_saveOrLoadByteSpan(gc, row.data(), static_cast<int>(Cols));
}
template <std::size_t Rows, std::size_t Cols>
inline void gc_saveOrLoadShortMatrixInlined(GameCanvas *gc, std::array<std::array<int16_t, Cols>, Rows> &m) {
    for (auto &row : m) gc_saveOrLoadShortSpan(gc, row.data(), static_cast<int>(Cols));
}
inline void gc_saveOrLoadByteMatrixInlined(GameCanvas *gc, Grid<int8_t> &m) {
    for (auto &row : m.storage())
        gc_saveOrLoadByteSpan(gc, row.data(), static_cast<int>(row.size()));
}
void gc_initSaveOrLoad(GameCanvas *gc, int32_t n);
void gc_writeSaveSnapshot(GameCanvas *gc);
void gc_persistSnapshotMode0(GameCanvas *gc);
void gc_persistSnapshotMode1(GameCanvas *gc);
void gc_saveAllMapArrays(GameCanvas *gc);
bool gc_loadSnapshotAndReturn(GameCanvas *gc);
void gc_restoreAllMapArrays(GameCanvas *gc);
void gc_everyTenTick(GameCanvas *gc);
void gc_computeYearEndMetrics(GameCanvas *gc);
void gc_newYearNotify(GameCanvas *gc);
void gc_archiveOrRecall(GameCanvas *gc);
int32_t gc_scanMapForMetric(GameCanvas *gc);
void gc_setYearGraphMode(GameCanvas *gc, int32_t n, bool bl);
void gc_maybeAiTakeTurn(GameCanvas *gc);
void gc_placeCitiesAndIndustry(GameCanvas *gc);
void gc_dayResetAd(GameCanvas *gc);
void gc_postRevenueReset(GameCanvas *gc);
void gc_spawnTrainAt(GameCanvas *gc, int32_t n, int32_t n2);
void gc_autoExtendTrackStation(GameCanvas *gc, int32_t n, int32_t n2);
void gc_pickCitySite(GameCanvas *gc);
void gc_pickIndustrySite(GameCanvas *gc);
int32_t gc_computeMilestoneScore(GameCanvas *gc);
void gc_everyTwoTickZ(GameCanvas *gc);
void gc_rebuildMenuTypeIndexes(GameCanvas *gc);
int8_t gc_computeMenuRowDisplayValue(GameCanvas *gc, int32_t n);
void gc_placeTrainAt(GameCanvas *gc, int32_t n, int32_t n2);
void gc_applyMenuRowEffect(GameCanvas *gc, int32_t n);
void gc_duplicateMenuRow(GameCanvas *gc, int32_t n);
void gc_everyMonthlyTick(GameCanvas *gc);
void gc_recordTrainRevenue(GameCanvas *gc, int32_t n);
void gc_recordDeliveredResource(GameCanvas *gc, int32_t n, int32_t n2);
void gc_incrementCargoOptionCount(GameCanvas *gc, int32_t n);
void gc_decrementCargoOptionCount(GameCanvas *gc, int32_t n);
void gc_incrementRowValueType13(GameCanvas *gc, int32_t n);
void gc_incrementRowValueType14(GameCanvas *gc, int32_t n);
void gc_loadMusic(GameCanvas *gc);
void gc_playMusic(GameCanvas *gc);
void gc_setMusicVolume(GameCanvas *gc, int32_t n);
void gc_stopMusic(GameCanvas *gc);
int32_t gc_computeMapTrainMetric(GameCanvas *gc, int32_t n);
char* gc_decodeString(GameCanvas *gc, int8_t* byArray);
char* gc_buildPlayerNameString(GameCanvas *gc);
bool gc_syncHighScores(GameCanvas *gc, bool bl);
void gc_appendTileDrawOffset(GameCanvas *gc, ByteBuffer& x_offsets,
                             ByteBuffer& y_offsets, int32_t x, int32_t y);
void gc_k(GameCanvas *gc, int32_t n, int32_t n2);
void gc_drawTextBox(GameCanvas *gc, Surface* graphics, int32_t n, int32_t n2,
                    int32_t n3, int32_t n4, std::span<const int8_t> codes);
void gc_enterOutOfMoneyState(GameCanvas *gc);
void gc_drawRectFill(GameCanvas *gc, Surface* graphics, int32_t n, int32_t n2, int8_t by);
void gc_setCanvasSize(GameCanvas *gc, int32_t n, int32_t n2);
void gc_paint(GameCanvas *gc, Surface* graphics);
void gc_setState(GameCanvas *gc, int32_t n);
void gc_update(GameCanvas *gc);
void gc_renderMap(GameCanvas *gc, Surface* graphics);
void gc_hideNotify(GameCanvas *gc);
void gc_showNotify(GameCanvas *gc);

#endif
