#include "game_internal.h"
#include "scenario_definition.h"

namespace {

void applyScenarioDefinition(Game *game, const ScenarioDefinition& definition) {
    game->scenario.scenarioStep = definition.scenarioIndex;
    game->world.terrain.mapWidth = game->world.terrain.mapHeight = definition.mapSize;
    game->world.terrain.mapHalfWidth = (game->world.terrain.mapWidth - 1) * 8;
    game->world.terrain.mapScreenWidth = (game->world.terrain.mapWidth + game->world.terrain.mapHeight) * 16;
    game->world.terrain.mapScreenHeight = (game->world.terrain.mapWidth + game->world.terrain.mapHeight) * 8;
    game->renderer.camera.viewportHalfWidth = game->world.terrain.mapScreenWidth / 2;
    game->renderer.camera.viewportHalfHeight = game->world.terrain.mapScreenHeight / 2;
    game->renderer.camera.viewportCenterX = game->renderer.camera.viewportHalfWidth;
    game->renderer.camera.viewportCenterY = game->renderer.camera.viewportHalfHeight;
    game->renderer.camera.viewportBaseX = game->renderer.camera.viewportCenterX;
    game->renderer.camera.viewportBaseY = game->renderer.camera.viewportCenterY;
    game->renderer.camera.mapScaleX = game->renderer.camera.viewportBaseX * 2;
    game->renderer.camera.mapScaleY = game->renderer.camera.viewportBaseY * 2;
    game->world.initialObstacleCount = definition.obstacleCount;
    game->world.terrainRaiseStepCount = definition.terrainRaiseSteps;
    game->world.terrainLowerStepCount = definition.terrainLowerSteps;
    game->world.terrainPatchBaseSize = definition.terrainPatchSize;
    game->world.mapGenColumns = definition.mapGenColumns;
    game->world.mapGenRows = definition.mapGenRows;
    game->world.terrainFeatureStage7Count = definition.stage7FeatureCount;
    game->world.terrainFeatureStage6Count = definition.stage6FeatureCount;
    game->world.weightAccel = definition.weightAccel1;
    game->world.weightAccel2 = definition.weightAccel2;
    game->world.weightAccel3 = definition.weightAccel3;
    game->world.weightAccel4 = definition.weightAccel4;
    game->economy.money = definition.startingMoney;
    game->economy.loanCeiling = definition.loanCeiling;
    game->economy.loanBalance = definition.startingLoan;
    game->economy.interestRate = static_cast<int8_t>(definition.interestRate);
    game->scenario.currentEra = definition.eraNumber - 1;
    game->scenario.scenarioMilestoneIndex = static_cast<int8_t>(definition.milestoneIndex);
    game->scenario.currentMilestoneScore = definition.milestoneScore;
    game->scenario.milestoneCoThreshold2 = definition.milestoneThreshold;
    game_initBuildMenuForScenario(game, definition.buildMenuScenario);
    game->scenario.aiExpansionEnabled = (definition.scenarioFlags & 2) == 2;
    game->scenario.monthlyOffersEnabled = (definition.scenarioFlags & 4) == 4;
    if ((definition.scenarioFlags & 1) != 1) {
        game->scenario.aiFrequency = 25;
        game->scenario.aiThreshold = 100;
    } else {
        game->scenario.aiFrequency = 32;
        game->scenario.aiThreshold = 29;
    }
    game->scenario.scenarioNoticeId = definition.noticeId;
    if (definition.noticeId != 0) {
        game->ui.transitionDebounce = 5;
        game->scenario.scenarioNoticeCode = static_cast<int8_t>(definition.noticeId);
    } else {
        game->ui.transitionDebounce = 0;
    }
}

void beginMapGeneration(Game *game) {
    game->saveCodec.walk(*game, save::OP_RESET_SKIP);
    game->economy.dayOfMonth = 1; game->economy.monthOfYear = 1; game->economy.year = 1950;
    game->scenario.milestonesEnabled = false; game->ui.pauseFlag = false; game->scenario.aiEnabled = false;
    game->economy.moneyBaseA = 974; game->economy.moneyBaseB = 974;
    game->economy.baseTrainSpeed = 173; game->economy.accel1 = 342; game->economy.brake1 = 311;
    game->economy.baseTrainSpeed2 = 173; game->economy.accel2 = 234;
    game->economy.costPower = 4759; game->economy.costStruct = 9845; game->economy.costOverhead = 347;
    game->economy.costMaintenance = 5982; game->economy.costCrew = 481;
    game->economy.gWagonCost = 37894; game->economy.gWagonCost2 = 43247; game->economy.gWagonCost3 = 51872;
    game->economy.gWagonCost4 = 60149; game->economy.gWagonCost5 = 71423; game->economy.gWagonCost6 = 82675;
    game->economy.costGroupC1 = 4578; game->economy.costGroupC2 = 5196; game->economy.costGroupC3 = 4314; game->economy.costGroupC4 = 5072;

    ResourceReader resource = resources::open("scenarios.bin");
    if (resource) {
        const int recordCount = resource.read_byte();
        for (int recordIndex = 0; recordIndex < recordCount; ++recordIndex) {
            ScenarioDefinition definition;
            if (!readScenarioDefinition(resource, definition)) break;
            if (definition.scenarioIndex == game->scenario.activeScenario)
                applyScenarioDefinition(game, definition);
        }
    }
    if (game->scenario.activeScenario <= 1) {
        if (game->scenario.difficultySlot == 0) { game->scenario.aiFrequency = 32; game->scenario.aiThreshold = 29; }
        if (game->scenario.difficultySlot == 1) { game->scenario.aiFrequency = 25; game->scenario.aiThreshold = 22; }
        if (game->scenario.difficultyA == 0) { game->economy.interestRate = 4; game->economy.enterpriseTaxPct = 2; }
        if (game->scenario.difficultyA == 1) { game->economy.interestRate = 8; game->economy.enterpriseTaxPct = 5; }
        if (game->scenario.difficultyB == 0) { game->world.mapGenColumns /= 2; game->world.mapGenRows /= 2; }
        if (game->scenario.difficultyC == 0) {
            game->world.terrainFeatureStage7Count /= 2; game->world.terrainFeatureStage6Count /= 2;
            game->world.weightAccel /= 2; game->world.weightAccel2 /= 2; game->world.weightAccel3 /= 2; game->world.weightAccel4 /= 2;
        }
    }
}

void updateBootSplash(Game *game) {
    if (midp_current_time_millis() - game->loadingStartTime > 5000L)
        game->loadingTimeout = true;
    ++game->loadingAnimationFrame;
    if (!game->loadingTimeout) return;
    if (game->gameState == GameState::BootSplash1) {
        for (int i = 0; i < 6; ++i)
            game->assets.font_images[i] = game_decodePngFromSpriteStream(game);
        game_setState(game, GameState::IntroReady);
        return;
    }
    game_setState(game, game->gameState + 1);
}

void updateIntroReady(Game *game) {
    game->ready = true;
    game->ui.screenParam = 3;
    game->ui.transitionCode = 14;
    game_resetOverlayRenderState(game);
    game_setState(game, GameState::MenuOverlay);
}

void updateLoadingResources(Game *game) {
    game->assets.tile_sprites[game->assets.spriteLoadCounter] = game_decodePngFromSpriteStream(game);
    ++game->assets.spriteLoadCounter;
    if (game->assets.spriteLoadCounter == game->scenario.scenarioResourceCount / 2) {
        game->load_music();
        if (game->music.autoStart) { game->play_music(); game->music.enabled = true; }
        else game->music.enabled = false;
    }
    if (game->assets.spriteLoadCounter != game->scenario.scenarioResourceCount) return;
    game->ready = true;
    game->ui.transitionCode = 1;
    game_setState(game, GameState::MenuOverlay);
}

void updateGeneratingMap(Game *game) {
    ++game->stateWorkStep;
    if (game->stateWorkStep == 2) beginMapGeneration(game);
    if (game->stateWorkStep == 3) {
        game->ui.buildMenuItemIndex = 0;
        game->ui.buildMenuMode = 0;
        game->run_map_generation_step(0);
    }
    if (game->stateWorkStep <= 3) return;
    game->run_map_generation_step(1);
    if (game->world.mapGenTilesDone != game->world.totalTilesToGenerate) return;
    game->world.mapGenTilesDone = 0;
    game_setState(game, GameState::Playing);
}

void updatePlaying(Game *game) {
    game_simulationTick(game);
}

void setMinimapColor(Game *game, int kind, int x, int y) {
    Surface *surface = game->renderer.backbuffer_surface;
    switch (kind) {
        case 1:
            if (game->world.terrain.tileElevation[x][y] == -1) midp_set_color(surface, 3167360);
            if (game->world.terrain.tileElevation[x][y] == 0) midp_set_color(surface, 5274640);
            if (game->world.terrain.tileElevation[x][y] == 1) midp_set_color(surface, 6327328);
            if (game->world.terrain.tileElevation[x][y] == 2) midp_set_color(surface, 7380016);
            if (game->world.terrain.tileElevation[x][y] == 3) midp_set_color(surface, 8432704);
            break;
        case 2: midp_set_color(surface, 6327312); break;
        case 3: midp_set_color(surface, 0x909040); break;
        case 4: midp_set_color(surface, 0x606060); break;
        case 5: midp_set_color(surface, 0x404040); break;
        case 6: midp_set_color(surface, 0xFC0000); break;
        case 7: midp_set_color(surface, 0x101010); break;
        case 8: midp_set_color(surface, 0xFC9C00); break;
        case 9: midp_set_color(surface, 0x78DC1E); break;
        case 10: midp_set_color(surface, 0xFCFC00); break;
        case 11: midp_set_color(surface, 0x80C4FC); break;
        case 12: midp_set_color(surface, 0); break;
        case 13: midp_set_color(surface, 14287063); break;
        case 14: midp_set_color(surface, 16738557); break;
        case 15: midp_set_color(surface, 16040947); break;
    }
}

void buildMinimapOverview(Game *game) {
    const int size = game->world.terrain.mapHeight + game->world.terrain.mapWidth;
    game->renderer.backbuffer = Image::create_blank(size, size);
    game->renderer.backbuffer_surface = game->renderer.backbuffer->drawing_surface();
    midp_set_color(game->renderer.backbuffer_surface, 0);
    midp_fill_rect(game->renderer.backbuffer_surface, 0, 0, size, size);
    int previousKind = -1;
    int kind = 0;
    for (int x = 0; x < game->world.terrain.mapWidth; ++x) {
        for (int y = 0; y < game->world.terrain.mapHeight; ++y) {
            const int drawY = game->world.terrain.mapWidth - x + y;
            const int drawX = x + y;
            const int tileCode = game->world.terrain.tileMap[x][y];
            const int trackCode = game->world.trackMap[x][y];
            if (tileCode == 0) kind = 1;
            if (tileCode >= 3 && tileCode <= 28) kind = 2;
            if (tileCode >= tile::SCENERY_FIRST && tileCode <= tile::SCENERY_LAST) kind = 3;
            if (tileCode >= 34 && tileCode <= 41) kind = 4;
            if (tileCode >= 42 && tileCode <= 48) kind = 5;
            if (tileCode >= 50 && tileCode <= 52) kind = 6;
            if (tileCode >= 53 && tileCode <= 55) kind = 7;
            if (tileCode >= 56 && tileCode <= 58) kind = 8;
            if (tileCode >= 59 && tileCode <= 61) kind = 9;
            if (tileCode >= 62 && tileCode <= 64) kind = 10;
            if (tileCode >= 65 && tileCode <= 68) kind = 11;
            if (tileCode >= 70 && tileCode <= 90) {
                if (trackCode == track::PRESENT || trackCode == track::PRESENT + 1) kind = 12;
                if (trackCode == track::PRESENT + 2) kind = 13;
                if (trackCode == track::PRESENT + 3) kind = 14;
                if (trackCode == track::PRESENT + 4) kind = 15;
            }
            if (kind != previousKind) setMinimapColor(game, kind, x, y);
            previousKind = kind;
            midp_draw_line(game->renderer.backbuffer_surface, drawX, drawY, drawX + 1, drawY);
        }
        previousKind = -1;
    }
    game_setState(game, GameState::MapOverview);
}

void updateBuildingMinimap(Game *game) {
    if (game->stateWorkStep > 0) --game->stateWorkStep;
    if (game->stateWorkStep == 2) buildMinimapOverview(game);
}

void updateMapOverview(Game *game) {
    game->renderer.camera.pan(
        game->ui.lastKeyCode, game->world.terrain.mapScreenWidth,
        game->world.terrain.mapScreenHeight, game->renderer.canvasWidth,
        game->renderer.gameplayViewportHeight);
}

void updateBuildingPath(Game *game) {
    if (game->stateWorkStep > 0) --game->stateWorkStep;
    if (game->stateWorkStep != 2) return;
    if (game_tryBuildPath(game)) {
        game_setState(game, GameState::Playing);
        game->ui.buildMenuItemIndex = 0;
        game->build.buildCancelled = true;
        game->ui.buildMenuMode = 2;
        game->ui.menuDirty = true;
        return;
    }
    game->ui.buildMenuItemIndex = 0;
    game->build.buildCancelled = true;
    game->ui.buildMenuMode = 1;
    game->ui.menuDirty = true;
    game->ready = false;
    game->ui.transitionCode = 12;
    game_setState(game, GameState::MenuOverlay);
}

void updateSavingGame(Game *game) {
    if (game->stateWorkStep > 0) --game->stateWorkStep;
    if (game->stateWorkStep != 2) return;
    game->saveCodec.save_all(*game);
    game->ui.buildMenuItemIndex = 0;
    game->ui.menuDirty = true;
    game_setState(game, GameState::Playing);
}

void updateLoadingGame(Game *game) {
    if (game->stateWorkStep > 0) --game->stateWorkStep;
    if (game->stateWorkStep != 2) return;
    game->saveCodec.restore_all(*game);
    game->ui.pauseFlag = false;
    game->ui.buildMenuItemIndex = 0;
    game->ui.menuDirty = true;
    game_setState(game, GameState::Playing);
}

void updateAiTurn(Game *game) {
    if (game->stateWorkStep > 0) --game->stateWorkStep;
    if (game->stateWorkStep != 2) return;
    if (game->build.nextTrackPieceCode - track::AI_PIECE_FIRST < game->scenario.difficultyCycle) {
        game->place_cities_and_industry();
        game_setState(game, GameState::Playing);
        return;
    }
    ++game->build.nextTrackPieceCode;
    if (game->build.nextTrackPieceCode == track::AI_PIECE_WRAP)
        game->build.nextTrackPieceCode = track::AI_PIECE_FIRST;
    game_setState(game, GameState::Playing);
}

void updateHighScoreSync(Game *game) {
    if (game->stateWorkStep > 0) --game->stateWorkStep;
    if (game->stateWorkStep != 2) return;
    game->ui.transitionCode = game->sync_high_scores(game->scenario.highScoreUploadRequested)
                              ? 19 : 18;
    game_setState(game, GameState::MenuOverlay);
}

} // namespace

void Game::update_state_machine() {
    Game* game = this;
    switch (game->gameState) {
        case GameState::BootSplash0:
        case GameState::BootSplash1:
        case GameState::BootSplash2: updateBootSplash(game); break;
        case GameState::IntroReady: updateIntroReady(game); break;
        case GameState::LoadingResources: updateLoadingResources(game); break;
        case GameState::GeneratingMap: updateGeneratingMap(game); break;
        case GameState::Playing: updatePlaying(game); break;
        case GameState::BuildingMinimap: updateBuildingMinimap(game); break;
        case GameState::MapOverview: updateMapOverview(game); break;
        case GameState::BuildingPath: updateBuildingPath(game); break;
        case GameState::SavingGame: updateSavingGame(game); break;
        case GameState::LoadingGame: updateLoadingGame(game); break;
        case GameState::AiTurn: updateAiTurn(game); break;
        case GameState::HighScoreSync: updateHighScoreSync(game); break;
    }
}
