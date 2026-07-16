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
#include "midp.h"
#include "support/grid.h"
#include "support/java_random.h"
#include "support/music_output.h"
#include "support/resource_reader.h"
#include <array>
#include <memory>
#include <vector>

typedef struct GameCanvas {
    void* midlet;
    bool stopRequested;
    void* gameThread;
    int32_t simSpeed;
    int64_t frameStartTime;
    int64_t sleepStart;
    int32_t sleepMillis;
    int32_t frameCount;
    int32_t frameIndex;
    int64_t frameTime;
    bool paintSuspended;   // when true, gc_paint returns without drawing
    bool displayInitialized;
    bool confirmCloseRequested;  // one-shot: close confirm dialog to the menu overlay
    bool flagsA;
    bool snapshotLoadSucceeded;
    Pathfinder pathfinder;
    int32_t serviceVehicleAtCursor;
    Fleet fleet;
    int32_t trainAtCursor;
    int32_t era;
    bool compareToPrevious;
    bool monthRolloverFlag;
    bool flagsB;
    int32_t savedLegacyMetricBq;
    bool music_playing;
    bool music_available;
    bool music_auto_start;
    bool music_enabled;
    int32_t music_volume_level;
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
    int32_t cachedHeaderMoney;
    int32_t cachedHeaderDay;
    int32_t cachedHeaderMonth;
    int32_t cachedHeaderYear;
    int32_t counterA;
    JavaRandom rng;
    int64_t loadingStartTime;
    bool loadingTimeout;
    bool ready;
    int32_t loadStep;
    int32_t timerA1;
    int32_t timerA2;
    int32_t timerB1;
    int32_t timerB2;
    int32_t timerC1;
    int32_t timerC2;
    std::array<int32_t, 9> graphColors{};  // 3 series x 3 ARGB colors for report graphs
    int32_t lastCanvasWidth;   // last size passed to gc_setCanvasSize
    int32_t lastCanvasHeight;
    int32_t savedGameState;  // game state stashed for return after an overlay
    int32_t loadingTimer;
    int32_t dayTickThreshold;  // frames per game-day (counterA counts up to this)
    int64_t tickStart;
    int64_t tickEnd;
} GameCanvas;

extern int32_t gcs_screenWidth;
extern int32_t gcs_hs;
extern int32_t gcs_gameplayViewportHeight;
extern int32_t gcs_bottomBarY;
extern bool gcs_animationsDisabled;
extern bool gcs_selectionHidden;
extern bool gcs_monthlyReportEnabled;
extern bool gcs_annualReportEnabled;

void gc_fields_init(GameCanvas *gc);

#endif
