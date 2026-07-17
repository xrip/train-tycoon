// Train Tycoon game-state initial values.
#include "game_state.h"
#include <string.h>

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

void game_fields_init(Game *game) {
    *game = Game{};
    canvasWidth = 1;
    canvasHeight = 1;
    gameplayViewportHeight = canvasHeight - 30;
    bottomBarY = canvasHeight - 15;
    animationsDisabled = false;
    selectionHidden = false;
    game->world.terrain.mapHalfWidth = (game->world.terrain.mapWidth - 1) * 8;
    game->world.terrain.mapScreenWidth = (game->world.terrain.mapWidth + game->world.terrain.mapHeight) * 16;
    game->world.terrain.mapScreenHeight = (game->world.terrain.mapWidth + game->world.terrain.mapHeight) * 8;
    game->renderer.viewportHalfWidth = game->world.terrain.mapScreenWidth / 2;
    game->renderer.viewportHalfHeight = game->world.terrain.mapScreenHeight / 2;
    game->renderer.viewportBaseX = game->renderer.viewportHalfWidth;
    game->renderer.viewportBaseY = game->renderer.viewportHalfHeight;
    game->renderer.mapScaleX = game->renderer.viewportHalfWidth * 2;
    game->renderer.mapScaleY = game->renderer.viewportHalfHeight * 2;
    memcpy(game->fleet.forwardTrackSteps, kForwardTrackSteps, sizeof(kForwardTrackSteps));
    memcpy(game->fleet.reverseTrackSteps, kReverseTrackSteps, sizeof(kReverseTrackSteps));
    memcpy(game->assets.trainSpriteDirectionTable, kTrainSpriteDirections,
           sizeof(kTrainSpriteDirections));
    game->scenario.offerTypesByEffect[0] = {1, 3, 13};
    game->scenario.offerTypesByEffect[1] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    game->scenario.offerTypesByEffect[2] = {1, 2, 3, 13, 14, 15};
    game->scenario.offerTypesByEffect[3] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
    game->scenario.offerTypesByEffect[4] = {1, 2, 3, 13, 14, 15};
    game->scenario.offerTypesByEffect[5] = {4, 9};
    game->scenario.offerTypesByEffect[6] = {5, 10};
    game->scenario.offerTypesByEffect[7] = {6, 11};
    game->scenario.offerTypesByEffect[8] = {7, 12};
    game->scenario.offerTypesByEffect[9] = {8, 12};
    game->scenario.offerTypesByEffect[10] = {1, 2, 3, 13};
    game->scenario.offerTypesByEffect[11] = {1, 2, 3, 14};
    memcpy(game->scenario.offerEffectSchedule, kOfferEffectSchedule,
           sizeof(kOfferEffectSchedule));
    game->rng.set_seed(midp_current_time_millis());
    game->renderer.backbuffer = Image::create_blank(canvasWidth, canvasHeight);
    game->renderer.backbuffer_surface = game->renderer.backbuffer->drawing_surface();
    memcpy(game->graphColors, kGraphColors, sizeof(kGraphColors));
    memcpy(game->assets.tileSpriteYOffsetByCode, kTileSpriteYOffsets,
           sizeof(kTileSpriteYOffsets));
}
