// Train Tycoon game-state initial values.
#include "game_state.h"
#include <string.h>
#include <utility>

int32_t canvasWidth;
int32_t canvasHeight;
int32_t gameplayViewportHeight;
int32_t bottomBarY;
bool animationsDisabled;
bool selectionHidden;

namespace {

constexpr int8_t kForwardTrackSteps[kTrackDirectionCount][kTrackStepValueCount] = {
    {0, 1, 0, 1, 0, 0, 2, 4, 5, 10, 6, 0},
    {2, 3, 1, 0, 1, 2, 3, 6, 5, 11, 6, 2},
    {4, 5, 1, 0, 1, 2, 3, 6, 5, 11, 6, 2},
    {6, 7, 0, 1, 0, 0, 2, 4, 5, 10, 6, 0},
    {8, 9, 0, 1, 0, 0, 2, 4, 5, 10, 6, 0},
    {10, 11, -1, 0, 1, 3, 2, 5, 4, 8, 6, 3},
};

constexpr int8_t kReverseTrackSteps[kTrackDirectionCount][kTrackStepValueCount] = {
    {1, 0, 0, -1, 0, 1, 3, 7, 4, 9, 6, 1},
    {3, 2, -1, 0, 1, 3, 2, 5, 4, 8, 6, 3},
    {5, 4, 0, -1, 0, 1, 3, 7, 4, 9, 6, 1},
    {7, 6, -1, 0, 1, 3, 2, 5, 4, 8, 6, 3},
    {9, 8, 1, 0, 1, 2, 3, 6, 5, 11, 6, 2},
    {11, 10, 0, -1, 0, 1, 3, 7, 4, 9, 6, 1},
};

constexpr int16_t kTrainSpriteDirections[kTrainSpriteTypeCount][kTrainSpriteDirectionCount] = {
    {130, 131, 132, 133, 134, 135, 136, 137},
    {140, 141, 142, 143, 144, 145, 146, 147},
    {160, 161, 162, 163, 160, 161, 162, 163},
    {165, 166, 167, 168, 165, 166, 167, 168},
    {170, 171, 172, 173, 170, 171, 172, 173},
    {175, 176, 177, 178, 175, 176, 177, 178},
    {180, 181, 182, 183, 180, 181, 182, 183},
    {185, 186, 187, 188, 185, 186, 187, 188},
};

constexpr int8_t kOfferEffectSchedule[kOfferEffectCount][kOfferScheduleFieldCount] = {
    {0, 0}, {0, 0}, {5, 20}, {10, -56}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {8, 100},
};

constexpr int32_t kGraphColors[kGraphColorCount] = {
    16756834, 16765610, 0xC08000, 0xFF9696, 0xFFC8C8,
    0xC80000, 0xA0FFA0, 0xC8FFC8, 51200,
};

constexpr int8_t kTileSpriteYOffsets[kTileYOffsetCount] = {
    0, 0, -4, 0, 0, 4, 0, 4, 0, 0, 4, 0, 0,
    4, 0, 0, 0, -4, 4, 4, 0, 0, 4, 0, -4, 0,
};

}  // namespace

Game::Game(std::unique_ptr<MusicOutput> music_output, int64_t random_seed) {
    canvasWidth = 1;
    canvasHeight = 1;
    gameplayViewportHeight = canvasHeight - 30;
    bottomBarY = canvasHeight - 15;
    animationsDisabled = false;
    selectionHidden = false;
    world.terrain.mapHalfWidth = (world.terrain.mapWidth - 1) * 8;
    world.terrain.mapScreenWidth = (world.terrain.mapWidth + world.terrain.mapHeight) * 16;
    world.terrain.mapScreenHeight = (world.terrain.mapWidth + world.terrain.mapHeight) * 8;
    renderer.camera.viewportHalfWidth = world.terrain.mapScreenWidth / 2;
    renderer.camera.viewportHalfHeight = world.terrain.mapScreenHeight / 2;
    renderer.camera.viewportBaseX = renderer.camera.viewportHalfWidth;
    renderer.camera.viewportBaseY = renderer.camera.viewportHalfHeight;
    renderer.camera.mapScaleX = renderer.camera.viewportHalfWidth * 2;
    renderer.camera.mapScaleY = renderer.camera.viewportHalfHeight * 2;
    memcpy(fleet.forwardTrackSteps, kForwardTrackSteps, sizeof(kForwardTrackSteps));
    memcpy(fleet.reverseTrackSteps, kReverseTrackSteps, sizeof(kReverseTrackSteps));
    memcpy(assets.trainSpriteDirectionTable, kTrainSpriteDirections,
           sizeof(kTrainSpriteDirections));
    scenario.offerTypesByEffect[0] = {1, 3, 13};
    scenario.offerTypesByEffect[1] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    scenario.offerTypesByEffect[2] = {1, 2, 3, 13, 14, 15};
    scenario.offerTypesByEffect[3] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
    scenario.offerTypesByEffect[4] = {1, 2, 3, 13, 14, 15};
    scenario.offerTypesByEffect[5] = {4, 9};
    scenario.offerTypesByEffect[6] = {5, 10};
    scenario.offerTypesByEffect[7] = {6, 11};
    scenario.offerTypesByEffect[8] = {7, 12};
    scenario.offerTypesByEffect[9] = {8, 12};
    scenario.offerTypesByEffect[10] = {1, 2, 3, 13};
    scenario.offerTypesByEffect[11] = {1, 2, 3, 14};
    memcpy(scenario.offerEffectSchedule, kOfferEffectSchedule,
           sizeof(kOfferEffectSchedule));
    rng.set_seed(random_seed);
    music.output = std::move(music_output);
    renderer.backbuffer = Image::create_blank(canvasWidth, canvasHeight);
    renderer.backbuffer_surface = renderer.backbuffer->drawing_surface();
    memcpy(graphColors, kGraphColors, sizeof(kGraphColors));
    memcpy(assets.tileSpriteYOffsetByCode, kTileSpriteYOffsets,
           sizeof(kTileSpriteYOffsets));
}
