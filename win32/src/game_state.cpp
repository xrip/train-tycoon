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

void game_fields_init(Game *game) {
    *game = Game{};
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
    game->stopRequested = false;
    game->world.mapHalfWidth = (game->world.mapWidth - 1) * 8;
    game->world.mapScreenWidth = (game->world.mapWidth + game->world.mapHeight) * 16;
    game->world.mapScreenHeight = (game->world.mapWidth + game->world.mapHeight) * 8;
    game->renderer.viewportHalfWidth = game->world.mapScreenWidth / 2;
    game->renderer.viewportHalfHeight = game->world.mapScreenHeight / 2;
    game->simSpeed = 1;
    game->frameIndex = 0;
    game->ui.inputPending = false;
    game->ui.pauseFlag = false;
    game->ui.keyRepeatDelay = 0;
    game->ui.heldKey = 0;
    game->displayInitialized = false;
    game->ui.inputSubState = 0;
    game->ui.currentKey = 0;
    game->confirmCloseRequested = false;
    game->ui.cursorScale = 0;
    game->ui.cursorDragY = -1;
    game->ui.cursorSelectY = -1;
    game->ui.cursorSelectX = -1;
    game->ui.cursorDragX = -1;
    game->renderer.mapRedrawPending = false;
    game->ui.useDefaultOverlaySelection = true;
    game->ui.pauseMenuOpen = false;
    game->renderer.needsRepaint = false;
    game->renderer.lastRenderedSpriteId = -1;
    game->renderer.spriteJustChanged = 0;
    game->build.terrainSmoothCost = 0;
    game->world.initialIndustryPlaced = true;
    game->build.trackBuildInProgress = false;
    game->world.mapGenStage = 0;
    game->world.tilesGeneratedInStage = 0;
    game->world.totalTilesToGenerate = 0;
    game->world.mapGenTilesDone = 0;
    game->renderer.viewportBaseX = game->renderer.viewportHalfWidth;
    game->renderer.viewportBaseY = game->renderer.viewportHalfHeight;
    game->renderer.mapScaleX = game->renderer.viewportHalfWidth * 2;
    game->renderer.mapScaleY = game->renderer.viewportHalfHeight * 2;
    game->ui.keySubState = 0;
    game->ui.lastKeyCode = 0;
    game->ui.keyRepeatStep = 3;
    game->ui.buildMenuItemIndex = 0;
    game->ui.blockBuildMenu = false;
    game->ui.menuDirty = true;
    game->ui.buildMenuMode = 0;
    game->ui.selectedMenuRowX = 0;
    game->ui.confirmToggle = 0;
    game->ui.confirmCounter = 0;
    game->ui.confirmTargetState = -1;
    game->scenario.aiExpansionEnabled = true;
    game->build.junctionDirCursor = 0;
    game->ui.numSelectedTrains = 0;
    game->build.stationDirCursor = 0;
    game->ui.cityListIndex = 0;
    game->ui.pendingTransitionCode = -1;
    game->build.discountApplied = false;
    game->build.baseTrainCostSnapshot = 0;
    game->scenario.scenarioMilestone = 0;
    game->scenario.difficultyA = 0;
    game->scenario.difficultySlot = 0;
    game->scenario.difficultyB = 0;
    game->scenario.difficultyC = 0;
    game->scenario.offerRowIndex = 0;
    game->scenario.duplicateRowState = 0;
    game->scenario.highScoreUploadRequested = false;
    game->scenario.routeSlotIndex = 0;
    game->assets.spriteCycleCursor = 0;
    game->ui.snapshotLoadedFlag = false;
    game->snapshotLoadSucceeded = false;
    game->scenario.aiEnabled = false;
    game->pathfinder.tieBreakCounter = 0;
    game->build.previewTrackOwnerCode = 0;
    game->build.existingTrackOwnerCode = 1;
    game->build.selectionCost = 0;
    game->build.buildCancelled = true;
    game->build.trackTileCount = 0;
    game->serviceVehicleAtCursor = 0;
    game->build.wagonType = 0;
    game->build.wagonDiscountType = 0;
    game->build.unitCost = 0;
    game->build.currentTrainCostSnapshot = 0;
    game->scenario.difficultyPct = 100;
    game->era = 1;
    game->fleet.forwardTrackSteps = {{
        {{0, 1, 0, 1, 0, 0, 2, 4, 5, 10, 6, 0}},
        {{2, 3, 1, 0, 1, 2, 3, 6, 5, 11, 6, 2}},
        {{4, 5, 1, 0, 1, 2, 3, 6, 5, 11, 6, 2}},
        {{6, 7, 0, 1, 0, 0, 2, 4, 5, 10, 6, 0}},
        {{8, 9, 0, 1, 0, 0, 2, 4, 5, 10, 6, 0}},
        {{10, 11, -1, 0, 1, 3, 2, 5, 4, 8, 6, 3}},
    }};
    game->fleet.reverseTrackSteps = {{
        {{1, 0, 0, -1, 0, 1, 3, 7, 4, 9, 6, 1}},
        {{3, 2, -1, 0, 1, 3, 2, 5, 4, 8, 6, 3}},
        {{5, 4, 0, -1, 0, 1, 3, 7, 4, 9, 6, 1}},
        {{7, 6, -1, 0, 1, 3, 2, 5, 4, 8, 6, 3}},
        {{9, 8, 1, 0, 1, 2, 3, 6, 5, 11, 6, 2}},
        {{11, 10, 0, -1, 0, 1, 3, 7, 4, 9, 6, 1}},
    }};
    game->assets.trainSpriteDirectionTable = {{
        {{130, 131, 132, 133, 134, 135, 136, 137}},
        {{140, 141, 142, 143, 144, 145, 146, 147}},
        {{160, 161, 162, 163, 160, 161, 162, 163}},
        {{165, 166, 167, 168, 165, 166, 167, 168}},
        {{170, 171, 172, 173, 170, 171, 172, 173}},
        {{175, 176, 177, 178, 175, 176, 177, 178}},
        {{180, 181, 182, 183, 180, 181, 182, 183}},
        {{185, 186, 187, 188, 185, 186, 187, 188}},
    }};
    game->saveCodec.saveBuffer.clear();
    game->saveCodec.saveReadOffset = 0;
    game->saveCodec.saveSlot = 0;
    game->saveCodec.savePendingMode = save::MODE_WRITE;
    game->saveCodec.snapshotSize = 0;
    game->saveCodec.snapshotLoaded = false;
    game->saveCodec.bytesUncompressed = 0;
    game->saveCodec.bytesCompressed = 0;
    game->compareToPrevious = false;
    game->ui.yearGraphMode = 0;
    game->ui.yearGraphAlt = false;
    game->monthRolloverFlag = false;
    game->economy.totalSpent = 0;
    game->economy.aiMoneySpent = 0;
    game->economy.fuelCostAccrued = 0;
    game->economy.subsidyCost = 0;
    game->economy.totalRevenue = 0;
    game->economy.terrainTotal = 0;
    game->economy.decadeEnterpriseTax = 0;
    game->economy.netProfit = 0;
    game->economy.decadeSpentWithCurrent = 0;
    game->economy.decadeAiSpentWithCurrent = 0;
    game->economy.decadeFuelWithCurrent = 0;
    game->economy.decadeSubsidyWithCurrent = 0;
    game->economy.decadeRevenueWithCurrent = 0;
    game->economy.legacyDecadeMetric = 0;
    game->economy.decadeEnterpriseWithCurrent = 0;
    game->economy.decadeProfitWithCurrent = 0;
    game->economy.prevTotalSpent = 0;
    game->economy.prevAiMoneySpent = 0;
    game->economy.prevFuelCost = 0;
    game->economy.prevSubsidyCost = 0;
    game->economy.prevTotalRevenue = 0;
    game->economy.prevTerrainTotal = 0;
    game->economy.prevEnterpriseTotal = 0;
    game->economy.previousYearProfit = 0;
    game->economy.prevDecadeSpent = 0;
    game->economy.prevDecadeAiSpent = 0;
    game->economy.prevDecadeFuel = 0;
    game->economy.prevDecadeSubsidy = 0;
    game->economy.prevDecadeRevenue = 0;
    game->economy.previousLegacyDecadeMetric = 0;
    game->economy.previousDecadeEnterpriseTax = 0;
    game->economy.previousDecadeProfit = 0;
    game->economy.decadeCostTotal = 0;
    game->economy.decadeSpentTotal = 0;
    game->economy.decadeFuelCost = 0;
    game->economy.decadeSubsidyTotal = 0;
    game->economy.decadeRevenueTotal = 0;
    game->economy.savedLegacyMetricEb = 0;
    game->economy.decadeEnterpriseTotal = 0;
    game->economy.decadeProfitTotal = 0;
    game->economy.revenueTotal = 0;
    game->economy.decadeProfitGraphTotal = 0;
    game->scenario.difficultyCycle = 1;
    game->build.nextTrackPieceCode = 66;
    game->scenario.aiFrequency = 15;
    game->scenario.aiThreshold = 12;
    game->scenario.offerTypesByEffect = {{
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
    game->scenario.offerEffectSchedule = {{
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
    game->scenario.offerGenerationCount = 0;
    game->scenario.lastOfferDateCode = 0;
    game->ui.menuRowEnabledCount = 0;
    game->economy.lifetimeTrainRevenue = 0;
    game->scenario.milestoneThreshold1 = 0;
    game->scenario.milestoneThreshold2 = 0;
    game->scenario.milestoneThreshold3 = 0;
    game->scenario.milestoneThreshold4 = 0;
    game->scenario.milestoneThreshold5 = 0;
    game->scenario.aiWagonsBought = 0;
    game->music_playing = false;
    game->music_available = false;
    game->music_auto_start = false;
    game->music_enabled = false;
    game->music_volume_level = 2;
    game->economy.trainRevenueScore = 0;
    game->scenario.bestScenarioScore = 0;
    game->assets.numActiveTileAnimations = 0;
    game->renderer.viewportCenterX = 0;
    game->renderer.viewportCenterY = 0;
    game->economy.money = 200000;
    game->cachedHeaderMoney = 0;
    game->economy.loanBalance = 0;
    game->economy.loanCeiling = 600000;
    game->economy.interestRate = 4;
    game->economy.dayOfMonth = 1;
    game->economy.monthOfYear = 1;
    game->economy.year = 1950;
    game->cachedHeaderDay = 0;
    game->cachedHeaderMonth = 0;
    game->cachedHeaderYear = 0;
    game->counterA = 0;
    game->economy.enterpriseTaxPct = 2;
    game->scenario.scenarioStepCompare = 1;
    game->scenario.scenarioStep = 0;
    game->scenario.scenarioNoticeId = 0;
    game->rng.set_seed(midp_current_time_millis());
    game->renderer.backbuffer = Image::create_blank(gcs_screenWidth, gcs_hs);
    game->renderer.backbuffer_surface = game->renderer.backbuffer->drawing_surface();
    game->scenario.scenarioResourceCount = 0;
    game->ui.transitionCode = 0;
    game->ready = true;
    game->loadStep = 0;
    game->timerA1 = 0;
    game->timerA2 = 0;
    game->timerB1 = 0;
    game->timerB2 = 0;
    game->timerC1 = 0;
    game->timerC2 = 0;
    game->world.woodAgeTimer = 0;
    game->world.phaseTimer = 0;
    game->graphColors = {16756834, 16765610, 0xC08000, 0xFF9696, 0xFFC8C8, 0xC80000, 0xA0FFA0, 0xC8FFC8, 51200};
    game->lastCanvasWidth = -1;
    game->lastCanvasHeight = -1;
    game->savedGameState = 0;
    game->loadingTimer = 0;
    game->dayTickThreshold = 50;
    game->assets.tileSpriteYOffsetByCode = {0, 0, -4, 0, 0, 4, 0, 4, 0, 0, 4, 0, 0, 4, 0, 0, 0, -4, 4, 4, 0, 0, 4, 0, -4, 0};
    game->renderer.lastViewportCenterX = -1;
    game->renderer.viewportCenterYCached = -1;
    game->renderer.visibleTrainCount = 0;
}
