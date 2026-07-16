// Train Tycoon game-state initial values.
#include "game_state.h"
#include <string.h>

int32_t gcs_screenWidth;
int32_t gcs_hs;
int32_t gcs_gameplayViewportHeight;
int32_t gcs_bottomBarY;
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
    gcs_animationsDisabled = false;
    gcs_selectionHidden = false;
    gcs_monthlyReportEnabled = true;
    gcs_annualReportEnabled = true;
    // --- instance fields (declaration order) ---
    gc->stopRequested = false;
    gc->world.mapHalfWidth = (gc->world.mapWidth - 1) * 8;
    gc->world.mapScreenWidth = (gc->world.mapWidth + gc->world.mapHeight) * 16;
    gc->world.mapScreenHeight = (gc->world.mapWidth + gc->world.mapHeight) * 8;
    gc->viewportHalfWidth = gc->world.mapScreenWidth / 2;
    gc->viewportHalfHeight = gc->world.mapScreenHeight / 2;
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
    gc->routeSlotIndex = 0;
    gc->spriteCycleCursor = 0;
    gc->snapshotLoadedFlag = false;
    gc->cH = false;
    gc->aiEnabled = false;
    gc->pathfinder.tieBreakCounter = 0;
    gc->previewTrackOwnerCode = 0;
    gc->existingTrackOwnerCode = 1;
    gc->selectionCost = 0;
    gc->buildCancelled = true;
    gc->trackTileCount = 0;
    gc->serviceVehicleAtCursor = 0;
    gc->wagonType = 0;
    gc->wagonDiscountType = 0;
    gc->unitCost = 0;
    gc->currentTrainCostSnapshot = 0;
    gc->difficultyPct = 100;
    gc->era = 1;
    gc->fleet.forwardTrackSteps = {{
        {{0, 1, 0, 1, 0, 0, 2, 4, 5, 10, 6, 0}},
        {{2, 3, 1, 0, 1, 2, 3, 6, 5, 11, 6, 2}},
        {{4, 5, 1, 0, 1, 2, 3, 6, 5, 11, 6, 2}},
        {{6, 7, 0, 1, 0, 0, 2, 4, 5, 10, 6, 0}},
        {{8, 9, 0, 1, 0, 0, 2, 4, 5, 10, 6, 0}},
        {{10, 11, -1, 0, 1, 3, 2, 5, 4, 8, 6, 3}},
    }};
    gc->fleet.reverseTrackSteps = {{
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
    gc->saveBuffer.clear();
    gc->saveReadOffset = 0;
    gc->saveSlot = 0;
    gc->savePendingMode = save::MODE_WRITE;
    gc->snapshotSize = 0;
    gc->snapshotLoaded = false;
    gc->bytesUncompressed = 0;
    gc->bytesCompressed = 0;
    gc->compareToPrevious = false;
    gc->yearGraphMode = 0;
    gc->yearGraphAlt = false;
    gc->monthRolloverFlag = false;
    gc->economy.totalSpent = 0;
    gc->economy.aiMoneySpent = 0;
    gc->fuelCostAccrued = 0;
    gc->subsidyCost = 0;
    gc->economy.totalRevenue = 0;
    gc->terrainTotal = 0;
    gc->decadeEnterpriseTax = 0;
    gc->netProfit = 0;
    gc->decadeSpentWithCurrent = 0;
    gc->decadeAiSpentWithCurrent = 0;
    gc->decadeFuelWithCurrent = 0;
    gc->decadeSubsidyWithCurrent = 0;
    gc->economy.decadeRevenueWithCurrent = 0;
    gc->legacyDecadeMetric = 0;
    gc->decadeEnterpriseWithCurrent = 0;
    gc->decadeProfitWithCurrent = 0;
    gc->prevTotalSpent = 0;
    gc->economy.prevAiMoneySpent = 0;
    gc->prevFuelCost = 0;
    gc->prevSubsidyCost = 0;
    gc->economy.prevTotalRevenue = 0;
    gc->prevTerrainTotal = 0;
    gc->prevEnterpriseTotal = 0;
    gc->economy.previousYearProfit = 0;
    gc->prevDecadeSpent = 0;
    gc->prevDecadeAiSpent = 0;
    gc->prevDecadeFuel = 0;
    gc->prevDecadeSubsidy = 0;
    gc->economy.prevDecadeRevenue = 0;
    gc->previousLegacyDecadeMetric = 0;
    gc->previousDecadeEnterpriseTax = 0;
    gc->previousDecadeProfit = 0;
    gc->decadeCostTotal = 0;
    gc->decadeSpentTotal = 0;
    gc->decadeFuelCost = 0;
    gc->decadeSubsidyTotal = 0;
    gc->economy.decadeRevenueTotal = 0;
    gc->savedLegacyMetricEb = 0;
    gc->decadeEnterpriseTotal = 0;
    gc->decadeProfitTotal = 0;
    gc->economy.revenueTotal = 0;
    gc->economy.decadeProfitGraphTotal = 0;
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
    gc->offerGenerationCount = 0;
    gc->lastOfferDateCode = 0;
    gc->menuRowEnabledCount = 0;
    gc->economy.lifetimeTrainRevenue = 0;
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
    gc->economy.trainRevenueScore = 0;
    gc->bestScenarioScore = 0;
    gc->numActiveTileAnimations = 0;
    gc->viewportCenterX = 0;
    gc->viewportCenterY = 0;
    gc->economy.money = 200000;
    gc->cachedHeaderMoney = 0;
    gc->economy.loanBalance = 0;
    gc->economy.loanCeiling = 600000;
    gc->economy.interestRate = 4;
    gc->economy.dayOfMonth = 1;
    gc->economy.monthOfYear = 1;
    gc->economy.year = 1950;
    gc->cachedHeaderDay = 0;
    gc->cachedHeaderMonth = 0;
    gc->cachedHeaderYear = 0;
    gc->counterA = 0;
    gc->economy.enterpriseTaxPct = 2;
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
    gc->visibleTrainCount = 0;
}
