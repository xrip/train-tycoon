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
    int32_t at;
    int32_t cB;
    int32_t ar;
    int32_t D;
    int32_t viewportHalfWidth;
    int32_t viewportHalfHeight;
    int32_t initialObstacleCount;
    int32_t terrainRaiseStepCount;
    int32_t terrainLowerStepCount;
    int32_t mapGenColumns;
    int32_t mapGenRows;
    int32_t terrainPatchBaseSize;
    int32_t terrainFeatureStage7Count;
    int32_t terrainFeatureStage6Count;
    int32_t weightAccel;
    int32_t weightAccel2;
    int32_t weightAccel3;
    int32_t weightAccel4;
    int32_t moneyBaseA;
    int32_t moneyBaseB;
    int32_t baseTrainSpeed;
    int32_t accel1;
    int32_t brake1;
    int32_t baseTrainSpeed2;
    int32_t accel2;
    int32_t costPower;
    int32_t costStruct;
    int32_t costOverhead;
    int32_t costMaintenance;
    int32_t costCrew;
    int32_t gWagonCost;
    int32_t gWagonCost2;
    int32_t gWagonCost3;
    int32_t gWagonCost4;
    int32_t gWagonCost5;
    int32_t gWagonCost6;
    int32_t costGroupC1;
    int32_t costGroupC2;
    int32_t costGroupC3;
    int32_t costGroupC4;
    void* gameThread;
    int32_t simSpeed;
    int64_t frameStartTime;
    int64_t sleepStart;
    int32_t sleepMillis;
    int32_t frameCount;
    int32_t frameIndex;
    bool inputPending;
    bool pauseFlag;
    bool paused;
    int64_t frameTime;
    bool ce;
    int64_t bT;
    int32_t keyRepeatDelay;
    int32_t heldKey;
    bool displayInitialized;
    int32_t inputSubState;
    int32_t currentKey;
    bool ee;
    int32_t dp;
    ResourceReader sprite_stream;
    std::array<std::unique_ptr<Image>, 6> font_images;
    std::vector<int16_t> spriteOffsetTable;
    std::vector<int8_t> spriteData;
    int32_t cursorScale;
    int32_t cursorDragY;
    int32_t cursorSelectY;
    int32_t cursorSelectX;
    bool cursorOverUI;
    bool cursorAllowZoomOut;
    int32_t cursorDragX;
    bool mapRedrawPending;
    bool useDefaultOverlaySelection;
    bool pauseMenuOpen;
    int32_t dialogContentHeight;
    bool needsRepaint;
    int32_t clipX;
    int32_t clipY;
    int32_t clipWidth;
    int32_t clipHeight;
    int32_t currentSpriteId;
    int32_t lastRenderedSpriteId;
    int32_t spriteJustChanged;
    bool initialIndustryPlaced;
    int32_t mapGenStage;
    int32_t tilesGeneratedInStage;
    int32_t totalTilesToGenerate;
    int32_t mapGenTilesDone;
    int32_t viewportBaseX;
    int32_t viewportBaseY;
    int32_t mapScaleX;
    int32_t mapScaleY;
    int32_t keySubState;
    int32_t lastKeyCode;
    int32_t keyRepeatStep;
    int32_t buildMenuItemIndex;
    int32_t cursorScreenX;
    int32_t cursorScreenY;
    int32_t tileCursorX;
    int32_t tileCursorY;
    bool vehicleMenuSelectIndex;
    bool trainMenuSelectIndex;
    bool blockBuildMenu;
    bool menuDirty;
    int32_t buildMenuMode;
    int32_t selectedMenuRowX;
    Grid<int8_t> buildMenuItems;
    Grid<int8_t> buildMenuResultCodes;
    Grid<int8_t> menuItemCode;
    Grid<int8_t> menuTextIndex;
    Grid<int8_t> menuTransitionCode;
    Grid<bool> menuRowEnabled;
    int32_t buildMenuItemCursor;
    int32_t buildMenuModeIndex;
    int32_t buildMenuItemCode;
    int32_t confirmToggle;
    std::array<int8_t, 3> moneyTickerFrames{};
    int32_t confirmCounter;
    int32_t confirmTargetState;
    bool flagsA;
    int8_t junctionDirCursor;
    int8_t numSelectedTrains;
    std::array<int16_t, 64> trainSelectionIndex{};
    std::array<int16_t, 64> trainSelectionY{};
    std::array<int16_t, 64> trainSelectionX{};
    int8_t stationDirCursor;
    int8_t cityListIndex;
    int32_t pendingTransitionCode;
    int8_t spriteCycleCursor;
    bool snapshotLoadedFlag;
    bool cH;
    int32_t screenParam;
    Pathfinder pathfinder;
    // Endpoints of the path being built. pathAnchor is the reference endpoint
    // (the city station in AI gen, the first tile the player picks): it is the
    // reverse-BFS seed (distance 20) and the back-walk terminus. pathTarget is
    // the endpoint the track reaches toward (the industry station / second
    // pick): the flood-fill must reach it and the back-walk starts there.
    int32_t pathAnchorX;
    int32_t pathAnchorY;
    int32_t pathTargetX;
    int32_t pathTargetY;
    int32_t serviceVehicleAtCursor;
    Fleet fleet;
    int32_t trainAtCursor;
    int32_t era;
    int32_t dI;
    std::array<std::array<int16_t, 8>, 8> trainSpriteDirectionTable{};
    std::vector<int8_t> saveBuffer;
    int32_t saveReadOffset;
    int32_t saveSlot;
    int32_t savePendingMode;
    int32_t snapshotSize;
    bool snapshotLoaded;
    int32_t bytesUncompressed;
    int32_t bytesCompressed;
    bool compareToPrevious;
    int32_t yearGraphMode;
    bool yearGraphAlt;
    bool monthRolloverFlag;
    std::array<int8_t, 12> yearGraphOffsets{};
    std::array<int8_t, 12> altYearArray{};
    std::array<int8_t, 12> yearGraphValues{};
    int32_t industryCategory;
    bool industrySiteFound;
    int32_t cityStationX;
    int32_t cityStationY;
    int32_t industryStationX;
    int32_t industryTileY;
    std::array<int8_t, 6> menuRowType{};
    std::array<int8_t, 6> placeTrainDay{};
    std::array<int8_t, 6> menuRowParamA{};
    std::array<int8_t, 6> placeTrainMonth{};
    std::array<int8_t, 6> menuRowParamB{};
    std::array<int8_t, 6> placeTrainYear{};
    std::array<int8_t, 6> menuRowParamC{};
    std::array<int32_t, 6> menuRowCost{};
    std::array<int32_t, 6> menuRowValue{};
    std::array<int8_t, 6> menuRowAltType{};
    std::array<int8_t, 6> menuRowEra{};
    std::array<int32_t, 6> menuRowRevenue{};
    bool flagsB;
    int32_t savedLegacyMetricBq;
    int32_t menuRowEnabledCount;
    std::array<int8_t, 5> enabledMenuRowIndexes{};
    bool music_playing;
    bool music_available;
    bool music_auto_start;
    bool music_enabled;
    int32_t music_volume_level;
    std::unique_ptr<MusicOutput> music_output;
    std::vector<int8_t> tileOffsetX;
    std::vector<int8_t> tileOffsetY;
    int32_t numActiveTileAnimations;
    int32_t gameState;
    World world;  // owns tileMap, trackMap, tileElevation, heightMap
    int32_t viewportCenterX;
    int32_t viewportCenterY;
    Economy economy;
    Scenario scenario;
    BuildController build;
    int32_t cachedHeaderMoney;
    int32_t cachedHeaderDay;
    int32_t cachedHeaderMonth;
    int32_t cachedHeaderYear;
    int32_t counterA;
    JavaRandom rng;
    std::unique_ptr<Image> backbuffer;
    Surface* backbuffer_surface;
    std::vector<std::unique_ptr<Image>> tile_sprites;
    std::array<int8_t, 255> tileSpriteIndex{};
    std::unique_ptr<Image> cycle_flag;
    int64_t loadingStartTime;
    bool loadingTimeout;
    int32_t spriteLoadCounter;
    int32_t transitionCode;
    bool ready;
    int32_t loadStep;
    std::vector<int8_t> spriteIndexTable;
    Grid<int8_t> spriteBitmapTable;
    int32_t timerA1;
    int32_t timerA2;
    int32_t timerB1;
    int32_t timerB2;
    int32_t timerC1;
    int32_t timerC2;
    std::array<int32_t, 9> dc{};
    int32_t bx;
    int32_t fR;
    int8_t transitionDebounce;
    int32_t eD;
    int32_t aJ;
    int32_t hx;
    int32_t loadingTimer;
    int32_t dH;
    int64_t tickStart;
    int64_t tickEnd;
    int64_t gw;
    std::array<int8_t, 26> tileSpriteYOffsetByCode{};
    int32_t lastViewportCenterX;
    int32_t viewportCenterYCached;
    std::array<int16_t, 500> visibleTrains{};
    int32_t visibleTrainCount;
    int32_t viewportX1;
    int32_t viewportY1;
    int32_t viewportX0;
    int32_t viewportY0;
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
