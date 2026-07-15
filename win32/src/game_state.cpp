// Train Tycoon game-state initial values.
#include "game_state.h"
#include <string.h>

int32_t gcs_screenWidth;
int32_t gcs_hs;
int32_t gcs_gameplayViewportHeight;
int32_t gcs_bottomBarY;
int8_t* gcs_decodeHeaderName1;
int8_t* gcs_decodeHeaderName2;
int8_t* gcs_decodeHeaderName3;
int8_t* gcs_decodeHeaderName4;
int8_t* gcs_decodeHeaderName6;
int8_t* gcs_decodeHeaderName7;
int8_t* gcs_decodeHeaderValue1;
int8_t* gcs_viewportZoomStep;
int8_t* gcs_decodeHeaderName5;
int8_t* gcs_decodeUrl;
bool gcs_animationsDisabled;
bool gcs_selectionHidden;
bool gcs_monthlyReportEnabled;
bool gcs_annualReportEnabled;

void gc_fields_init(GameCanvas *gc) {
    *gc = GameCanvas{};
    // --- constructor finals (GameCanvas()) ---
    gc->at = 3; gc->cB = 33; gc->ar = 20; gc->D = 240;
    // --- static fields (declaration order) ---
    gcs_screenWidth = 1;
    gcs_hs = 1;
    gcs_gameplayViewportHeight = gcs_hs - 30;
    gcs_bottomBarY = gcs_hs - 15;
    static const int8_t decodeHeaderName1__d[] = {-15, 12, -41, -41, 37, 109, -35, 61, -10, 28, -25, 49, -31, 51, 15, 104, -32, 54, 51, 77, 42, 76};
    gcs_decodeHeaderName1 = (int8_t*)midp_alloc_arr(sizeof(int8_t), (int)(sizeof(decodeHeaderName1__d)/sizeof(int8_t)));
    memcpy(gcs_decodeHeaderName1, decodeHeaderName1__d, sizeof(decodeHeaderName1__d));
    static const int8_t decodeHeaderName2__d[] = {-11, 0, 17, 17, 53, 125, 28, 99, -49, 37, -4, 0, 40, 124, 68, 68, -49, -41, -26, 59, 41, 76};
    gcs_decodeHeaderName2 = (int8_t*)midp_alloc_arr(sizeof(int8_t), (int)(sizeof(decodeHeaderName2__d)/sizeof(int8_t)));
    memcpy(gcs_decodeHeaderName2, decodeHeaderName2__d, sizeof(decodeHeaderName2__d));
    static const int8_t decodeHeaderName3__d[] = {-46, 45, -20, 60, 60, 116, 60, 71, -24, 14, -17, 8, 44, 76, 40, 122, -8, 30, 29, 125, 57, 91};
    gcs_decodeHeaderName3 = (int8_t*)midp_alloc_arr(sizeof(int8_t), (int)(sizeof(decodeHeaderName3__d)/sizeof(int8_t)));
    memcpy(gcs_decodeHeaderName3, decodeHeaderName3__d, sizeof(decodeHeaderName3__d));
    static const int8_t decodeHeaderName4__d[] = {49, 76, -59, -59, -34, -42, 8, 17, -33, 61, 41, 77, -22, 54, -59, -34, -9, 29, -41, 33, 0, 18};
    gcs_decodeHeaderName4 = (int8_t*)midp_alloc_arr(sizeof(int8_t), (int)(sizeof(decodeHeaderName4__d)/sizeof(int8_t)));
    memcpy(gcs_decodeHeaderName4, decodeHeaderName4__d, sizeof(decodeHeaderName4__d));
    static const int8_t decodeHeaderName6__d[] = {-21, 54, -29, 35, -30, 42, -60, -40, 40, 76, 28, 100, -23, 62, -42, 56, -59, -39};
    gcs_decodeHeaderName6 = (int8_t*)midp_alloc_arr(sizeof(int8_t), (int)(sizeof(decodeHeaderName6__d)/sizeof(int8_t)));
    memcpy(gcs_decodeHeaderName6, decodeHeaderName6__d, sizeof(decodeHeaderName6__d));
    static const int8_t decodeHeaderName7__d[] = {23, 98, -57, -57, -11, 61, -29, 55, 51, 72, -2, 18};
    gcs_decodeHeaderName7 = (int8_t*)midp_alloc_arr(sizeof(int8_t), (int)(sizeof(decodeHeaderName7__d)/sizeof(int8_t)));
    memcpy(gcs_decodeHeaderName7, decodeHeaderName7__d, sizeof(decodeHeaderName7__d));
    static const int8_t decodeHeaderValue1__d[] = {40, 123, 39, 28, 9, 15, 65, 127, -35, -58, 3, 6, -59, -29, 18, 6, -21, -45, 41, 17, 31, 24};
    gcs_decodeHeaderValue1 = (int8_t*)midp_alloc_arr(sizeof(int8_t), (int)(sizeof(decodeHeaderValue1__d)/sizeof(int8_t)));
    memcpy(gcs_decodeHeaderValue1, decodeHeaderValue1__d, sizeof(decodeHeaderValue1__d));
    static const int8_t viewportZoomStep__d[] = {-53, -63, -55, 57, 13, 124, -53, -50, -65, 45, 26, 29};
    gcs_viewportZoomStep = (int8_t*)midp_alloc_arr(sizeof(int8_t), (int)(sizeof(viewportZoomStep__d)/sizeof(int8_t)));
    memcpy(gcs_viewportZoomStep, viewportZoomStep__d, sizeof(viewportZoomStep__d));
    static const int8_t decodeHeaderName5__d[] = {-30, 61, -6, 10, -3, 53, 20, 122, 57, 93, 44, 127, -2, 12, -31, 56, -58, 40};
    gcs_decodeHeaderName5 = (int8_t*)midp_alloc_arr(sizeof(int8_t), (int)(sizeof(decodeHeaderName5__d)/sizeof(int8_t)));
    memcpy(gcs_decodeHeaderName5, decodeHeaderName5__d, sizeof(decodeHeaderName5__d));
    static const int8_t decodeUrl__d[] = {18, 1, -28, -57, -10, 41, -37, -62, -40, 53, 12, 116, 53, -83, 36, 19, 38, 20, -33, -33, 36, 19, 68, 120, -25, 43, 6, 62, 29, 2, -25, 41, -68, 37, 66, 125, 0, 62, 62, 127, -54, -52, 17, 49, 17, 3, -59, -54, -30, 28, 5, 51, 24, 1, -65, -30, -8, 58, -6, 47, 11, 1, 17, 5, 47, 18, 53, 99, 42, 27, 57, 123, -9, 96, -54, -50, 17, 9, 34, 28, -41, 15, 3, 39, 46, 100, 64, 97, -44, -58, -19, 22, -38, -63, 18, 1, 8, 63};
    gcs_decodeUrl = (int8_t*)midp_alloc_arr(sizeof(int8_t), (int)(sizeof(decodeUrl__d)/sizeof(int8_t)));
    memcpy(gcs_decodeUrl, decodeUrl__d, sizeof(decodeUrl__d));
    gcs_animationsDisabled = false;
    gcs_selectionHidden = false;
    gcs_monthlyReportEnabled = true;
    gcs_annualReportEnabled = true;
    // --- instance fields (declaration order) ---
    gc->stopRequested = false;
    gc->mapHalfWidth = (gc->mapWidth - 1) * 8;
    gc->mapScreenWidth = (gc->mapWidth + gc->mapHeight) * 16;
    gc->mapScreenHeight = (gc->mapWidth + gc->mapHeight) * 8;
    gc->viewportHalfWidth = gc->mapScreenWidth / 2;
    gc->viewportHalfHeight = gc->mapScreenHeight / 2;
    gc->simSpeed = 1;
    gc->frameCount = 0;
    gc->frameIndex = 0;
    gc->inputPending = false;
    gc->pauseFlag = false;
    gc->paused = false;
    gc->ce = false;
    gc->bT = midp_current_time_millis();
    gc->keyRepeatDelay = 0;
    gc->heldKey = 0;
    gc->displayInitialized = false;
    gc->inputSubState = 0;
    gc->currentKey = 0;
    gc->ee = false;
    gc->dp = 255;
    gc->cursorScale = 0;
    gc->cursorDragY = -1;
    gc->cursorSelectY = -1;
    gc->cursorSelectX = -1;
    gc->cursorDragX = -1;
    gc->mapRedrawPending = false;
    gc->useDefaultOverlaySelection = true;
    gc->pauseMenuOpen = false;
    gc->needsRepaint = false;
    gc->lastRenderedSpriteId = -1;
    gc->spriteJustChanged = 0;
    gc->terrainSmoothCost = 0;
    gc->initialIndustryPlaced = true;
    gc->trackBuildInProgress = false;
    gc->mapGenStage = 0;
    gc->tilesGeneratedInStage = 0;
    gc->totalTilesToGenerate = 0;
    gc->mapGenTilesDone = 0;
    gc->viewportBaseX = gc->viewportHalfWidth;
    gc->viewportBaseY = gc->viewportHalfHeight;
    gc->mapScaleX = gc->viewportHalfWidth * 2;
    gc->mapScaleY = gc->viewportHalfHeight * 2;
    gc->keySubState = 0;
    gc->lastKeyCode = 0;
    gc->keyRepeatStep = 3;
    gc->buildMenuItemIndex = 0;
    gc->blockBuildMenu = false;
    gc->menuDirty = true;
    gc->buildMenuMode = 0;
    gc->selectedMenuRowX = 0;
    gc->confirmToggle = 0;
    gc->confirmCounter = 0;
    gc->confirmTargetState = -1;
    gc->flagsA = true;
    gc->junctionDirCursor = 0;
    gc->numSelectedTrains = 0;
    gc->stationDirCursor = 0;
    gc->cityListIndex = 0;
    gc->pendingTransitionCode = -1;
    gc->discountApplied = false;
    gc->baseTrainCostSnapshot = 0;
    gc->scenarioMilestone = 0;
    gc->difficultyA = 0;
    gc->difficultySlot = 0;
    gc->difficultyB = 0;
    gc->difficultyC = 0;
    gc->offerRowIndex = 0;
    gc->duplicateRowState = 0;
    gc->highScoreUploadRequested = false;
    gc->playerNameCodes = (int8_t*)midp_alloc_arr(sizeof(int8_t), 10);
    gc->playerNameCursorMarker = (int8_t*)midp_alloc_arr(sizeof(int8_t), 10);
    gc->routeSlotIndex = 0;
    gc->spriteCycleCursor = 0;
    gc->snapshotLoadedFlag = false;
    gc->cH = false;
    gc->aiEnabled = false;
    gc->pathTieBreakCounter = 0;
    gc->previewTrackOwnerCode = 0;
    gc->existingTrackOwnerCode = 1;
    gc->selectionCost = 0;
    gc->buildCancelled = true;
    gc->trackTileCount = 0;
    static const int32_t dk__d[] = {-1, 1, 0, 0, -1, 1, 0};
    gc->dk = (int32_t*)midp_alloc_arr(sizeof(int32_t), (int)(sizeof(dk__d)/sizeof(int32_t)));
    memcpy(gc->dk, dk__d, sizeof(dk__d));
    static const int32_t j__d[] = {0, 0, -1, 1, 0, 0, -1};
    gc->j = (int32_t*)midp_alloc_arr(sizeof(int32_t), (int)(sizeof(j__d)/sizeof(int32_t)));
    memcpy(gc->j, j__d, sizeof(j__d));
    gc->serviceVehicleAtCursor = 0;
    gc->wagonType = 0;
    gc->wagonDiscountType = 0;
    gc->unitCost = 0;
    gc->currentTrainCostSnapshot = 0;
    gc->difficultyPct = 100;
    gc->serviceResourceStock = (int16_t**)midp_alloc_arr(sizeof(int16_t*), 250);
    for (int _i = 0; _i < 250; ++_i) gc->serviceResourceStock[_i] = (int16_t*)midp_alloc_arr(sizeof(int16_t), 5);
    gc->era = 1;
    gc->forwardTrackSteps = {{
        {{0, 1, 0, 1, 0, 0, 2, 4, 5, 10, 6, 0}},
        {{2, 3, 1, 0, 1, 2, 3, 6, 5, 11, 6, 2}},
        {{4, 5, 1, 0, 1, 2, 3, 6, 5, 11, 6, 2}},
        {{6, 7, 0, 1, 0, 0, 2, 4, 5, 10, 6, 0}},
        {{8, 9, 0, 1, 0, 0, 2, 4, 5, 10, 6, 0}},
        {{10, 11, -1, 0, 1, 3, 2, 5, 4, 8, 6, 3}},
    }};
    gc->reverseTrackSteps = {{
        {{1, 0, 0, -1, 0, 1, 3, 7, 4, 9, 6, 1}},
        {{3, 2, -1, 0, 1, 3, 2, 5, 4, 8, 6, 3}},
        {{5, 4, 0, -1, 0, 1, 3, 7, 4, 9, 6, 1}},
        {{7, 6, -1, 0, 1, 3, 2, 5, 4, 8, 6, 3}},
        {{9, 8, 1, 0, 1, 2, 3, 6, 5, 11, 6, 2}},
        {{11, 10, 0, -1, 0, 1, 3, 7, 4, 9, 6, 1}},
    }};
    gc->dI = 5;
    gc->trainSpriteDirectionTable = {{
        {{130, 131, 132, 133, 134, 135, 136, 137}},
        {{140, 141, 142, 143, 144, 145, 146, 147}},
        {{160, 161, 162, 163, 160, 161, 162, 163}},
        {{165, 166, 167, 168, 165, 166, 167, 168}},
        {{170, 171, 172, 173, 170, 171, 172, 173}},
        {{175, 176, 177, 178, 175, 176, 177, 178}},
        {{180, 181, 182, 183, 180, 181, 182, 183}},
        {{185, 186, 187, 188, 185, 186, 187, 188}},
    }};
    gc->saveBuffer = NULL;
    gc->saveReadOffset = 0;
    gc->saveSlot = 0;
    gc->savePendingMode = 0;
    gc->snapshotSize = 0;
    gc->snapshotLoaded = false;
    gc->bytesUncompressed = 0;
    gc->bytesCompressed = 0;
    gc->compareToPrevious = false;
    gc->yearGraphMode = 0;
    gc->yearGraphAlt = false;
    gc->monthRolloverFlag = false;
    gc->totalSpent = 0;
    gc->aiMoneySpent = 0;
    gc->fuelCostAccrued = 0;
    gc->subsidyCost = 0;
    gc->totalRevenue = 0;
    gc->terrainTotal = 0;
    gc->decadeEnterpriseTax = 0;
    gc->netProfit = 0;
    gc->decadeSpentWithCurrent = 0;
    gc->decadeAiSpentWithCurrent = 0;
    gc->decadeFuelWithCurrent = 0;
    gc->decadeSubsidyWithCurrent = 0;
    gc->decadeRevenueWithCurrent = 0;
    gc->legacyDecadeMetric = 0;
    gc->decadeEnterpriseWithCurrent = 0;
    gc->decadeProfitWithCurrent = 0;
    gc->prevTotalSpent = 0;
    gc->prevAiMoneySpent = 0;
    gc->prevFuelCost = 0;
    gc->prevSubsidyCost = 0;
    gc->prevTotalRevenue = 0;
    gc->prevTerrainTotal = 0;
    gc->prevEnterpriseTotal = 0;
    gc->previousYearProfit = 0;
    gc->prevDecadeSpent = 0;
    gc->prevDecadeAiSpent = 0;
    gc->prevDecadeFuel = 0;
    gc->prevDecadeSubsidy = 0;
    gc->prevDecadeRevenue = 0;
    gc->previousLegacyDecadeMetric = 0;
    gc->previousDecadeEnterpriseTax = 0;
    gc->previousDecadeProfit = 0;
    gc->decadeCostTotal = 0;
    gc->decadeSpentTotal = 0;
    gc->decadeFuelCost = 0;
    gc->decadeSubsidyTotal = 0;
    gc->decadeRevenueTotal = 0;
    gc->savedLegacyMetricEb = 0;
    gc->decadeEnterpriseTotal = 0;
    gc->decadeProfitTotal = 0;
    gc->revenueTotal = 0;
    gc->decadeProfitGraphTotal = 0;
    gc->difficultyCycle = 1;
    gc->nextTrackPieceCode = 66;
    gc->aiFrequency = 15;
    gc->aiThreshold = 12;
    gc->offerTypesByEffect = {{
        {1, 3, 13},
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
        {1, 2, 3, 13, 14, 15},
        {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
        {1, 2, 3, 13, 14, 15},
        {4, 9},
        {5, 10},
        {6, 11},
        {7, 12},
        {8, 12},
        {1, 2, 3, 13},
        {1, 2, 3, 14},
    }};
    gc->offerEffectSchedule = {{
        {{0, 0}},
        {{0, 0}},
        {{5, 20}},
        {{10, -56}},
        {{0, 0}},
        {{0, 0}},
        {{0, 0}},
        {{0, 0}},
        {{0, 0}},
        {{0, 0}},
        {{0, 0}},
        {{8, 100}},
    }};
    gc->menuRowType = (int8_t*)midp_alloc_arr(sizeof(int8_t), 6);
    gc->placeTrainDay = (int8_t*)midp_alloc_arr(sizeof(int8_t), 6);
    gc->menuRowParamA = (int8_t*)midp_alloc_arr(sizeof(int8_t), 6);
    gc->placeTrainMonth = (int8_t*)midp_alloc_arr(sizeof(int8_t), 6);
    gc->menuRowParamB = (int8_t*)midp_alloc_arr(sizeof(int8_t), 6);
    gc->placeTrainYear = (int8_t*)midp_alloc_arr(sizeof(int8_t), 6);
    gc->menuRowParamC = (int8_t*)midp_alloc_arr(sizeof(int8_t), 6);
    gc->menuRowCost = (int32_t*)midp_alloc_arr(sizeof(int32_t), 6);
    gc->menuRowValue = (int32_t*)midp_alloc_arr(sizeof(int32_t), 6);
    gc->menuRowAltType = (int8_t*)midp_alloc_arr(sizeof(int8_t), 6);
    gc->menuRowEra = (int8_t*)midp_alloc_arr(sizeof(int8_t), 6);
    gc->menuRowRevenue = (int32_t*)midp_alloc_arr(sizeof(int32_t), 6);
    gc->offerGenerationCount = 0;
    gc->offerEffectNextCount = (int16_t*)midp_alloc_arr(sizeof(int16_t), 12);
    gc->lastOfferDateCode = 0;
    gc->menuRowEnabledCount = 0;
    gc->lifetimeTrainRevenue = 0;
    gc->milestoneThreshold1 = 0;
    gc->milestoneThreshold2 = 0;
    gc->milestoneThreshold3 = 0;
    gc->milestoneThreshold4 = 0;
    gc->milestoneThreshold5 = 0;
    gc->aiWagonsBought = 0;
    gc->music_playing = false;
    gc->music_available = false;
    gc->music_auto_start = false;
    gc->music_enabled = false;
    gc->music_volume_level = 2;
    gc->trainRevenueScore = 0;
    gc->bestScenarioScore = 0;
    gc->scenarioScores = (int32_t*)midp_alloc_arr(sizeof(int32_t), 13);
    gc->leaderboardScores = (int32_t*)midp_alloc_arr(sizeof(int32_t), 14);
    gc->tileOffsetX = (int8_t*)midp_alloc_arr(sizeof(int8_t), 1);
    gc->tileOffsetY = (int8_t*)midp_alloc_arr(sizeof(int8_t), 1);
    gc->numActiveTileAnimations = 0;
    gc->viewportCenterX = 0;
    gc->viewportCenterY = 0;
    gc->money = 200000;
    gc->cachedHeaderMoney = 0;
    gc->loanBalance = 0;
    gc->loanCeiling = 600000;
    gc->interestRate = 4;
    gc->dayOfMonth = 1;
    gc->monthOfYear = 1;
    gc->year = 1950;
    gc->cachedHeaderDay = 0;
    gc->cachedHeaderMonth = 0;
    gc->cachedHeaderYear = 0;
    gc->counterA = 0;
    gc->enterpriseTaxPct = 2;
    gc->scenarioStepCompare = 1;
    gc->scenarioStep = 0;
    gc->scenarioNoticeId = 0;
    gc->rng.set_seed(midp_current_time_millis());
    gc->backbuffer = Image::create_blank(gcs_screenWidth, gcs_hs);
    gc->backbuffer_surface = gc->backbuffer->drawing_surface();
    gc->scenarioResourceCount = 0;
    gc->transitionCode = 0;
    gc->ready = true;
    gc->loadStep = 0;
    gc->timerA1 = 0;
    gc->timerA2 = 0;
    gc->timerB1 = 0;
    gc->timerB2 = 0;
    gc->timerC1 = 0;
    gc->timerC2 = 0;
    gc->woodAgeTimer = 0;
    gc->phaseTimer = 0;
    gc->tileQueue = (int8_t**)midp_alloc_arr(sizeof(int8_t*), 50);
    for (int _i = 0; _i < 50; ++_i) gc->tileQueue[_i] = (int8_t*)midp_alloc_arr(sizeof(int8_t), 3);
    gc->dc = {16756834, 16765610, 0xC08000, 0xFF9696, 0xFFC8C8, 0xC80000, 0xA0FFA0, 0xC8FFC8, 51200};
    gc->bx = 0;
    gc->fR = 0;
    gc->eD = -1;
    gc->aJ = -1;
    gc->hx = 0;
    gc->loadingTimer = 0;
    gc->dH = 50;
    gc->gw = 0;
    gc->tileSpriteYOffsetByCode = {0, 0, -4, 0, 0, 4, 0, 4, 0, 0, 4, 0, 0, 4, 0, 0, 0, -4, 4, 4, 0, 0, 4, 0, -4, 0};
    gc->lastViewportCenterX = -1;
    gc->viewportCenterYCached = -1;
    gc->visibleTrains = (int16_t*)midp_alloc_arr(sizeof(int16_t), 500);
    gc->visibleTrainCount = 0;
}
