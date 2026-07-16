// Complete Train Tycoon game state and shared values.
#ifndef GAME_STATE_H
#define GAME_STATE_H
#include "game_constants.h"
#include "world.h"
#include "pathfinder.h"
#include "fleet.h"
#include "economy.h"
#include "scenario.h"
#include "build_controller.h"
#include "assets.h"
#include "renderer.h"
#include "ui_controller.h"
#include "save_codec.h"
#include "click_map.h"
#include "midp.h"
#include "support/grid.h"
#include "support/java_random.h"
#include "support/music_output.h"
#include "support/resource_reader.h"
#include <memory>
#include <vector>

typedef struct Game {
    bool stopRequested;
    int32_t simSpeed = 1;
    int32_t frameIndex;
    bool displayInitialized;
    bool confirmCloseRequested;  // one-shot: close confirm dialog to the menu overlay
    bool snapshotLoadSucceeded;
    Pathfinder pathfinder;
    int32_t serviceVehicleAtCursor;
    Fleet fleet;
    int32_t trainAtCursor;
    int32_t era = 1;
    bool compareToPrevious;
    bool monthRolloverFlag;
    int32_t savedLegacyMetricBq;
    bool music_playing;
    bool music_available;
    bool music_auto_start;
    bool music_enabled;
    int32_t music_volume_level = 2;
    std::unique_ptr<MusicOutput> music_output;
    int32_t gameState;
    World world;  // owns tileMap, trackMap, tileElevation, heightMap
    Economy economy;
    Scenario scenario;
    BuildController build;
    Assets assets;
    Renderer renderer;
    UiController ui;
    SaveCodec saveCodec;
    ClickMap clickMap;  // captured overlay menu-row hit-rects, filled in game_paint
    int32_t cachedHeaderMoney;
    int32_t cachedHeaderDay;
    int32_t cachedHeaderMonth;
    int32_t cachedHeaderYear;
    int32_t dayFrameCounter;
    JavaRandom rng;
    int64_t loadingStartTime;
    bool loadingTimeout;
    bool ready = true;
    int32_t stateWorkStep;
    int32_t tile50AnimationPhase;
    int32_t tile50AnimationFrame;
    int32_t tile64AnimationPhase;
    int32_t tile64AnimationFrame;
    int32_t tile55AnimationPhase;
    int32_t tile55AnimationFrame;
    int32_t graphColors[kGraphColorCount]{};  // 3 series x 3 colors for report graphs
    int32_t lastCanvasWidth = -1;   // last size passed to game_setCanvasSize
    int32_t lastCanvasHeight = -1;
    int32_t savedGameState;  // game state stashed for return after an overlay
    int32_t loadingAnimationFrame;
    int32_t dayTickThreshold = 50;  // frames per game-day (dayFrameCounter counts up to this)
} Game;

extern int32_t canvasWidth;
extern int32_t canvasHeight;
extern int32_t gameplayViewportHeight;
extern int32_t bottomBarY;
extern bool animationsDisabled;
extern bool selectionHidden;
extern bool monthlyReportEnabled;
extern bool annualReportEnabled;

void game_fields_init(Game *game);

#endif
