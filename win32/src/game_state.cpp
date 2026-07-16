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
    gc->renderer.viewportHalfWidth = gc->world.mapScreenWidth / 2;
    gc->renderer.viewportHalfHeight = gc->world.mapScreenHeight / 2;
    gc->simSpeed = 1;
    gc->frameCount = 0;
    gc->frameIndex = 0;
    gc->ui.inputPending = false;
    gc->ui.pauseFlag = false;
    gc->ui.paused = false;
    gc->paintSuspended = false;
    gc->ui.keyRepeatDelay = 0;
    gc->ui.heldKey = 0;
    gc->displayInitialized = false;
    gc->ui.inputSubState = 0;
    gc->ui.currentKey = 0;
    gc->confirmCloseRequested = false;
    gc->ui.cursorScale = 0;
    gc->ui.cursorDragY = -1;
    gc->ui.cursorSelectY = -1;
    gc->ui.cursorSelectX = -1;
    gc->ui.cursorDragX = -1;
    gc->renderer.mapRedrawPending = false;
    gc->ui.useDefaultOverlaySelection = true;
    gc->ui.pauseMenuOpen = false;
    gc->renderer.needsRepaint = false;
    gc->renderer.lastRenderedSpriteId = -1;
    gc->renderer.spriteJustChanged = 0;
    gc->build.terrainSmoothCost = 0;
    gc->world.initialIndustryPlaced = true;
    gc->build.trackBuildInProgress = false;
    gc->world.mapGenStage = 0;
    gc->world.tilesGeneratedInStage = 0;
    gc->world.totalTilesToGenerate = 0;
    gc->world.mapGenTilesDone = 0;
    gc->renderer.viewportBaseX = gc->renderer.viewportHalfWidth;
    gc->renderer.viewportBaseY = gc->renderer.viewportHalfHeight;
    gc->renderer.mapScaleX = gc->renderer.viewportHalfWidth * 2;
    gc->renderer.mapScaleY = gc->renderer.viewportHalfHeight * 2;
    gc->ui.keySubState = 0;
    gc->ui.lastKeyCode = 0;
    gc->ui.keyRepeatStep = 3;
    gc->ui.buildMenuItemIndex = 0;
    gc->ui.blockBuildMenu = false;
    gc->ui.menuDirty = true;
    gc->ui.buildMenuMode = 0;
    gc->ui.selectedMenuRowX = 0;
    gc->ui.confirmToggle = 0;
    gc->ui.confirmCounter = 0;
    gc->ui.confirmTargetState = -1;
    gc->scenario.aiExpansionEnabled = true;
    gc->build.junctionDirCursor = 0;
    gc->ui.numSelectedTrains = 0;
    gc->build.stationDirCursor = 0;
    gc->ui.cityListIndex = 0;
    gc->ui.pendingTransitionCode = -1;
    gc->build.discountApplied = false;
    gc->build.baseTrainCostSnapshot = 0;
    gc->scenario.scenarioMilestone = 0;
    gc->scenario.difficultyA = 0;
    gc->scenario.difficultySlot = 0;
    gc->scenario.difficultyB = 0;
    gc->scenario.difficultyC = 0;
    gc->scenario.offerRowIndex = 0;
    gc->scenario.duplicateRowState = 0;
    gc->scenario.highScoreUploadRequested = false;
    gc->scenario.routeSlotIndex = 0;
    gc->assets.spriteCycleCursor = 0;
    gc->ui.snapshotLoadedFlag = false;
    gc->snapshotLoadSucceeded = false;
    gc->scenario.aiEnabled = false;
    gc->pathfinder.tieBreakCounter = 0;
    gc->build.previewTrackOwnerCode = 0;
    gc->build.existingTrackOwnerCode = 1;
    gc->build.selectionCost = 0;
    gc->build.buildCancelled = true;
    gc->build.trackTileCount = 0;
    gc->serviceVehicleAtCursor = 0;
    gc->build.wagonType = 0;
    gc->build.wagonDiscountType = 0;
    gc->build.unitCost = 0;
    gc->build.currentTrainCostSnapshot = 0;
    gc->scenario.difficultyPct = 100;
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
    gc->assets.trainSpriteDirectionTable = {{
        {{130, 131, 132, 133, 134, 135, 136, 137}},
        {{140, 141, 142, 143, 144, 145, 146, 147}},
        {{160, 161, 162, 163, 160, 161, 162, 163}},
        {{165, 166, 167, 168, 165, 166, 167, 168}},
        {{170, 171, 172, 173, 170, 171, 172, 173}},
        {{175, 176, 177, 178, 175, 176, 177, 178}},
        {{180, 181, 182, 183, 180, 181, 182, 183}},
        {{185, 186, 187, 188, 185, 186, 187, 188}},
    }};
    gc->saveCodec.saveBuffer.clear();
    gc->saveCodec.saveReadOffset = 0;
    gc->saveCodec.saveSlot = 0;
    gc->saveCodec.savePendingMode = save::MODE_WRITE;
    gc->saveCodec.snapshotSize = 0;
    gc->saveCodec.snapshotLoaded = false;
    gc->saveCodec.bytesUncompressed = 0;
    gc->saveCodec.bytesCompressed = 0;
    gc->compareToPrevious = false;
    gc->ui.yearGraphMode = 0;
    gc->ui.yearGraphAlt = false;
    gc->monthRolloverFlag = false;
    gc->economy.totalSpent = 0;
    gc->economy.aiMoneySpent = 0;
    gc->economy.fuelCostAccrued = 0;
    gc->economy.subsidyCost = 0;
    gc->economy.totalRevenue = 0;
    gc->economy.terrainTotal = 0;
    gc->economy.decadeEnterpriseTax = 0;
    gc->economy.netProfit = 0;
    gc->economy.decadeSpentWithCurrent = 0;
    gc->economy.decadeAiSpentWithCurrent = 0;
    gc->economy.decadeFuelWithCurrent = 0;
    gc->economy.decadeSubsidyWithCurrent = 0;
    gc->economy.decadeRevenueWithCurrent = 0;
    gc->economy.legacyDecadeMetric = 0;
    gc->economy.decadeEnterpriseWithCurrent = 0;
    gc->economy.decadeProfitWithCurrent = 0;
    gc->economy.prevTotalSpent = 0;
    gc->economy.prevAiMoneySpent = 0;
    gc->economy.prevFuelCost = 0;
    gc->economy.prevSubsidyCost = 0;
    gc->economy.prevTotalRevenue = 0;
    gc->economy.prevTerrainTotal = 0;
    gc->economy.prevEnterpriseTotal = 0;
    gc->economy.previousYearProfit = 0;
    gc->economy.prevDecadeSpent = 0;
    gc->economy.prevDecadeAiSpent = 0;
    gc->economy.prevDecadeFuel = 0;
    gc->economy.prevDecadeSubsidy = 0;
    gc->economy.prevDecadeRevenue = 0;
    gc->economy.previousLegacyDecadeMetric = 0;
    gc->economy.previousDecadeEnterpriseTax = 0;
    gc->economy.previousDecadeProfit = 0;
    gc->economy.decadeCostTotal = 0;
    gc->economy.decadeSpentTotal = 0;
    gc->economy.decadeFuelCost = 0;
    gc->economy.decadeSubsidyTotal = 0;
    gc->economy.decadeRevenueTotal = 0;
    gc->economy.savedLegacyMetricEb = 0;
    gc->economy.decadeEnterpriseTotal = 0;
    gc->economy.decadeProfitTotal = 0;
    gc->economy.revenueTotal = 0;
    gc->economy.decadeProfitGraphTotal = 0;
    gc->scenario.difficultyCycle = 1;
    gc->build.nextTrackPieceCode = 66;
    gc->scenario.aiFrequency = 15;
    gc->scenario.aiThreshold = 12;
    gc->scenario.offerTypesByEffect = {{
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
    gc->scenario.offerEffectSchedule = {{
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
    gc->scenario.offerGenerationCount = 0;
    gc->scenario.lastOfferDateCode = 0;
    gc->ui.menuRowEnabledCount = 0;
    gc->economy.lifetimeTrainRevenue = 0;
    gc->scenario.milestoneThreshold1 = 0;
    gc->scenario.milestoneThreshold2 = 0;
    gc->scenario.milestoneThreshold3 = 0;
    gc->scenario.milestoneThreshold4 = 0;
    gc->scenario.milestoneThreshold5 = 0;
    gc->scenario.aiWagonsBought = 0;
    gc->music_playing = false;
    gc->music_available = false;
    gc->music_auto_start = false;
    gc->music_enabled = false;
    gc->music_volume_level = 2;
    gc->economy.trainRevenueScore = 0;
    gc->scenario.bestScenarioScore = 0;
    gc->assets.numActiveTileAnimations = 0;
    gc->renderer.viewportCenterX = 0;
    gc->renderer.viewportCenterY = 0;
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
    gc->scenario.scenarioStepCompare = 1;
    gc->scenario.scenarioStep = 0;
    gc->scenario.scenarioNoticeId = 0;
    gc->rng.set_seed(midp_current_time_millis());
    gc->renderer.backbuffer = Image::create_blank(gcs_screenWidth, gcs_hs);
    gc->renderer.backbuffer_surface = gc->renderer.backbuffer->drawing_surface();
    gc->scenario.scenarioResourceCount = 0;
    gc->ui.transitionCode = 0;
    gc->ready = true;
    gc->loadStep = 0;
    gc->timerA1 = 0;
    gc->timerA2 = 0;
    gc->timerB1 = 0;
    gc->timerB2 = 0;
    gc->timerC1 = 0;
    gc->timerC2 = 0;
    gc->world.woodAgeTimer = 0;
    gc->world.phaseTimer = 0;
    gc->graphColors = {16756834, 16765610, 0xC08000, 0xFF9696, 0xFFC8C8, 0xC80000, 0xA0FFA0, 0xC8FFC8, 51200};
    gc->lastCanvasWidth = -1;
    gc->lastCanvasHeight = -1;
    gc->savedGameState = 0;
    gc->loadingTimer = 0;
    gc->dayTickThreshold = 50;
    gc->assets.tileSpriteYOffsetByCode = {0, 0, -4, 0, 0, 4, 0, 4, 0, 0, 4, 0, 0, 4, 0, 0, 0, -4, 4, 4, 0, 0, 4, 0, -4, 0};
    gc->renderer.lastViewportCenterX = -1;
    gc->renderer.viewportCenterYCached = -1;
    gc->renderer.visibleTrainCount = 0;
}
