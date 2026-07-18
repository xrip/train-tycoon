// Train Tycoon game-state initial values.
#include "game.h"
#include <array>
#include <string.h>
#include <utility>

namespace {

constexpr TrackStepTransition make_track_transition(
    int nextTileVariant, int nextDirection) {
    return {static_cast<int8_t>(nextTileVariant),
            static_cast<int8_t>(nextDirection)};
}

constexpr TrackStepEntry make_track_step(
    int enteringDirection, int fallbackDirection, int stepX, int stepY,
    TrackStepTransition firstTransition,
    TrackStepTransition secondTransition,
    TrackStepTransition thirdTransition,
    TrackStepTransition fourthTransition) {
    return {static_cast<int8_t>(enteringDirection),
            static_cast<int8_t>(fallbackDirection),
            static_cast<int8_t>(stepX), static_cast<int8_t>(stepY),
            {firstTransition, secondTransition, thirdTransition,
             fourthTransition}};
}

constexpr TrackStepEntry kForwardTrackSteps[kTrackDirectionCount] = {
    /* variant 0 */ make_track_step(
        train_direction_slot::Slot0, train_direction_slot::Slot1, 0, 1,
        make_track_transition(track_variant_slot::Variant0,
                              train_direction_slot::Slot0),
        make_track_transition(track_variant_slot::Variant2,
                              train_direction_slot::Slot4),
        make_track_transition(track_variant_slot::Variant5,
                              train_direction_slot::Slot10),
        make_track_transition(track_variant_slot::Variant6,
                              train_direction_slot::Slot0)),
    /* variant 1 */ make_track_step(
        train_direction_slot::Slot2, train_direction_slot::Slot3, 1, 0,
        make_track_transition(track_variant_slot::Variant1,
                              train_direction_slot::Slot2),
        make_track_transition(track_variant_slot::Variant3,
                              train_direction_slot::Slot6),
        make_track_transition(track_variant_slot::Variant5,
                              train_direction_slot::Slot11),
        make_track_transition(track_variant_slot::Variant6,
                              train_direction_slot::Slot2)),
    /* variant 2 */ make_track_step(
        train_direction_slot::Slot4, train_direction_slot::Slot5, 1, 0,
        make_track_transition(track_variant_slot::Variant1,
                              train_direction_slot::Slot2),
        make_track_transition(track_variant_slot::Variant3,
                              train_direction_slot::Slot6),
        make_track_transition(track_variant_slot::Variant5,
                              train_direction_slot::Slot11),
        make_track_transition(track_variant_slot::Variant6,
                              train_direction_slot::Slot2)),
    /* variant 3 */ make_track_step(
        train_direction_slot::Slot6, train_direction_slot::Slot7, 0, 1,
        make_track_transition(track_variant_slot::Variant0,
                              train_direction_slot::Slot0),
        make_track_transition(track_variant_slot::Variant2,
                              train_direction_slot::Slot4),
        make_track_transition(track_variant_slot::Variant5,
                              train_direction_slot::Slot10),
        make_track_transition(track_variant_slot::Variant6,
                              train_direction_slot::Slot0)),
    /* variant 4 */ make_track_step(
        train_direction_slot::Slot8, train_direction_slot::Slot9, 0, 1,
        make_track_transition(track_variant_slot::Variant0,
                              train_direction_slot::Slot0),
        make_track_transition(track_variant_slot::Variant2,
                              train_direction_slot::Slot4),
        make_track_transition(track_variant_slot::Variant5,
                              train_direction_slot::Slot10),
        make_track_transition(track_variant_slot::Variant6,
                              train_direction_slot::Slot0)),
    /* variant 5 */ make_track_step(
        train_direction_slot::Slot10, train_direction_slot::Slot11, -1, 0,
        make_track_transition(track_variant_slot::Variant1,
                              train_direction_slot::Slot3),
        make_track_transition(track_variant_slot::Variant2,
                              train_direction_slot::Slot5),
        make_track_transition(track_variant_slot::Variant4,
                              train_direction_slot::Slot8),
        make_track_transition(track_variant_slot::Variant6,
                              train_direction_slot::Slot3)),
};

constexpr TrackStepEntry kReverseTrackSteps[kTrackDirectionCount] = {
    /* variant 0 */ make_track_step(
        train_direction_slot::Slot1, train_direction_slot::Slot0, 0, -1,
        make_track_transition(track_variant_slot::Variant0,
                              train_direction_slot::Slot1),
        make_track_transition(track_variant_slot::Variant3,
                              train_direction_slot::Slot7),
        make_track_transition(track_variant_slot::Variant4,
                              train_direction_slot::Slot9),
        make_track_transition(track_variant_slot::Variant6,
                              train_direction_slot::Slot1)),
    /* variant 1 */ make_track_step(
        train_direction_slot::Slot3, train_direction_slot::Slot2, -1, 0,
        make_track_transition(track_variant_slot::Variant1,
                              train_direction_slot::Slot3),
        make_track_transition(track_variant_slot::Variant2,
                              train_direction_slot::Slot5),
        make_track_transition(track_variant_slot::Variant4,
                              train_direction_slot::Slot8),
        make_track_transition(track_variant_slot::Variant6,
                              train_direction_slot::Slot3)),
    /* variant 2 */ make_track_step(
        train_direction_slot::Slot5, train_direction_slot::Slot4, 0, -1,
        make_track_transition(track_variant_slot::Variant0,
                              train_direction_slot::Slot1),
        make_track_transition(track_variant_slot::Variant3,
                              train_direction_slot::Slot7),
        make_track_transition(track_variant_slot::Variant4,
                              train_direction_slot::Slot9),
        make_track_transition(track_variant_slot::Variant6,
                              train_direction_slot::Slot1)),
    /* variant 3 */ make_track_step(
        train_direction_slot::Slot7, train_direction_slot::Slot6, -1, 0,
        make_track_transition(track_variant_slot::Variant1,
                              train_direction_slot::Slot3),
        make_track_transition(track_variant_slot::Variant2,
                              train_direction_slot::Slot5),
        make_track_transition(track_variant_slot::Variant4,
                              train_direction_slot::Slot8),
        make_track_transition(track_variant_slot::Variant6,
                              train_direction_slot::Slot3)),
    /* variant 4 */ make_track_step(
        train_direction_slot::Slot9, train_direction_slot::Slot8, 1, 0,
        make_track_transition(track_variant_slot::Variant1,
                              train_direction_slot::Slot2),
        make_track_transition(track_variant_slot::Variant3,
                              train_direction_slot::Slot6),
        make_track_transition(track_variant_slot::Variant5,
                              train_direction_slot::Slot11),
        make_track_transition(track_variant_slot::Variant6,
                              train_direction_slot::Slot2)),
    /* variant 5 */ make_track_step(
        train_direction_slot::Slot11, train_direction_slot::Slot10, 0, -1,
        make_track_transition(track_variant_slot::Variant0,
                              train_direction_slot::Slot1),
        make_track_transition(track_variant_slot::Variant3,
                              train_direction_slot::Slot7),
        make_track_transition(track_variant_slot::Variant4,
                              train_direction_slot::Slot9),
        make_track_transition(track_variant_slot::Variant6,
                              train_direction_slot::Slot1)),
};

// Sprite rows are indexed by car type; each field is the sprite for one of
// the eight saved direction slots.  The direction numbers stay neutral until
// the complete legacy direction map is recovered.
struct TrainSpriteDirectionSet {
    int16_t spriteForDirection0;
    int16_t spriteForDirection1;
    int16_t spriteForDirection2;
    int16_t spriteForDirection3;
    int16_t spriteForDirection4;
    int16_t spriteForDirection5;
    int16_t spriteForDirection6;
    int16_t spriteForDirection7;
};

constexpr TrainSpriteDirectionSet kTrainSpriteDirections[kTrainSpriteTypeCount] = {
    /* wagon::EngineTypeA */      {130, 131, 132, 133, 134, 135, 136, 137},
    /* wagon::EngineTypeB */      {140, 141, 142, 143, 144, 145, 146, 147},
    /* wagon::PassengerMailCar */ {160, 161, 162, 163, 160, 161, 162, 163},
    /* wagon::IndustryGroup1Car */ {165, 166, 167, 168, 165, 166, 167, 168},
    /* wagon::IndustryGroup2CarA */ {170, 171, 172, 173, 170, 171, 172, 173},
    /* wagon::IndustryGroup2CarB */ {175, 176, 177, 178, 175, 176, 177, 178},
    /* wagon::IndustryGroup3CarA */ {180, 181, 182, 183, 180, 181, 182, 183},
    /* wagon::IndustryGroup3CarB */ {185, 186, 187, 188, 185, 186, 187, 188},
};

static_assert(sizeof(TrainSpriteDirectionSet)
              == sizeof(int16_t) * kTrainSpriteDirectionCount);

struct OfferEffectScheduleEntry {
    int8_t firstDelay;
    int8_t repeatDelay;
};

constexpr OfferEffectScheduleEntry kOfferEffectSchedules[kOfferEffectCount] = {
    /* CashPercentOfTarget */ {0, 0},
    /* FixedCash */ {0, 0},
    /* LoanCeilingIncrease */ {5, 20},
    /* EraAdvance */ {10, -56},
    /* ResourceFlag1 */ {0, 0},
    /* ResourceFlag2 */ {0, 0},
    /* ResourceFlag3 */ {0, 0},
    /* ResourceFlag4 */ {0, 0},
    /* ResourceFlag5 */ {0, 0},
    /* ResourceFlag6 */ {0, 0},
    /* LoanBalanceReduction */ {0, 0},
    /* InterestRateReduction */ {8, 100},
};

struct GraphColorSet {
    int32_t fillColor;
    int32_t outlineColor;
    int32_t lowerOutlineColor;
};

namespace graph_color {
constexpr int32_t ProfitFill = 0xFFB062;
constexpr int32_t ProfitOutline = 0xFFD2AA;
constexpr int32_t ProfitLowerOutline = 0xC08000;
constexpr int32_t RevenueFill = 0xFF9696;
constexpr int32_t RevenueOutline = 0xFFC8C8;
constexpr int32_t RevenueLowerOutline = 0xC80000;
constexpr int32_t CompanyValueFill = 0xA0FFA0;
constexpr int32_t CompanyValueOutline = 0xC8FFC8;
constexpr int32_t CompanyValueLowerOutline = 0xC800;
}  // namespace graph_color

// Offer generation uses one stable objective pool per effect. Keeping the
// table together makes the constructor data easy to audit while preserving
// the original vector ownership and effect-index order.
struct OfferTypePool {
    offer_effect::Id effect;
    std::array<int32_t, 15> objectiveIds;
    std::size_t objectiveCount;
};

constexpr OfferTypePool kOfferTypePools[] = {
    {offer_effect::CashPercentOfTarget,
     {offer_objective::Objective1, offer_objective::Objective3,
      offer_objective::Objective13}, 3},
    {offer_effect::FixedCash,
     {offer_objective::Objective1, offer_objective::Objective2,
      offer_objective::Objective3, offer_objective::Objective4,
      offer_objective::Objective5, offer_objective::Objective6,
      offer_objective::Objective7, offer_objective::Objective8,
      offer_objective::Objective9, offer_objective::Objective10,
      offer_objective::Objective11, offer_objective::Objective12,
      offer_objective::Objective13, offer_objective::Objective14,
      offer_objective::Objective15}, 15},
    {offer_effect::LoanCeilingIncrease,
     {offer_objective::Objective1, offer_objective::Objective2,
      offer_objective::Objective3, offer_objective::Objective13,
      offer_objective::Objective14, offer_objective::Objective15}, 6},
    {offer_effect::EraAdvance,
     {offer_objective::Objective4, offer_objective::Objective5,
      offer_objective::Objective6, offer_objective::Objective7,
      offer_objective::Objective8, offer_objective::Objective9,
      offer_objective::Objective10, offer_objective::Objective11,
      offer_objective::Objective12, offer_objective::Objective13,
      offer_objective::Objective14}, 11},
    {offer_effect::ResourceFlag1,
     {offer_objective::Objective1, offer_objective::Objective2,
      offer_objective::Objective3, offer_objective::Objective13,
      offer_objective::Objective14, offer_objective::Objective15}, 6},
    {offer_effect::ResourceFlag2,
     {offer_objective::Objective4, offer_objective::Objective9}, 2},
    {offer_effect::ResourceFlag3,
     {offer_objective::Objective5, offer_objective::Objective10}, 2},
    {offer_effect::ResourceFlag4,
     {offer_objective::Objective6, offer_objective::Objective11}, 2},
    {offer_effect::ResourceFlag5,
     {offer_objective::Objective7, offer_objective::Objective12}, 2},
    {offer_effect::ResourceFlag6,
     {offer_objective::Objective8, offer_objective::Objective12}, 2},
    {offer_effect::LoanBalanceReduction,
     {offer_objective::Objective1, offer_objective::Objective2,
      offer_objective::Objective3, offer_objective::Objective13}, 4},
    {offer_effect::InterestRateReduction,
     {offer_objective::Objective1, offer_objective::Objective2,
      offer_objective::Objective3, offer_objective::Objective14}, 4},
};

namespace startup_canvas {
constexpr int32_t Width = 1;
constexpr int32_t Height = 1;
}  // namespace startup_canvas

namespace tile_sprite_offset {
// The asset table proves only these three vertical byte values. Their visual
// direction is left open until the complete sprite map is recovered.
enum Value : int8_t {
    NoVerticalOffset = 0,
    PositiveFourPixels = 4,
    NegativeFourPixels = -4,
};
}  // namespace tile_sprite_offset

constexpr GraphColorSet kGraphColorSets[kGraphColorCount / 3] = {
    /* report_series::Profit */
    {graph_color::ProfitFill, graph_color::ProfitOutline,
     graph_color::ProfitLowerOutline},
    /* report_series::Revenue */
    {graph_color::RevenueFill, graph_color::RevenueOutline,
     graph_color::RevenueLowerOutline},
    /* report_series::CompanyValue */
    {graph_color::CompanyValueFill, graph_color::CompanyValueOutline,
     graph_color::CompanyValueLowerOutline},
};

static_assert(sizeof(kOfferEffectSchedules)
              == sizeof(int8_t) * kOfferEffectCount * kOfferScheduleFieldCount);
static_assert(sizeof(kGraphColorSets) == sizeof(int32_t) * kGraphColorCount);

constexpr int8_t kTileSpriteYOffsets[kTileYOffsetCount] = {
    tile_sprite_offset::NoVerticalOffset,
    tile_sprite_offset::NoVerticalOffset,
    tile_sprite_offset::NegativeFourPixels,
    tile_sprite_offset::NoVerticalOffset,
    tile_sprite_offset::NoVerticalOffset,
    tile_sprite_offset::PositiveFourPixels,
    tile_sprite_offset::NoVerticalOffset,
    tile_sprite_offset::PositiveFourPixels,
    tile_sprite_offset::NoVerticalOffset,
    tile_sprite_offset::NoVerticalOffset,
    tile_sprite_offset::PositiveFourPixels,
    tile_sprite_offset::NoVerticalOffset,
    tile_sprite_offset::NoVerticalOffset,
    tile_sprite_offset::PositiveFourPixels,
    tile_sprite_offset::NoVerticalOffset,
    tile_sprite_offset::NoVerticalOffset,
    tile_sprite_offset::NoVerticalOffset,
    tile_sprite_offset::NegativeFourPixels,
    tile_sprite_offset::PositiveFourPixels,
    tile_sprite_offset::PositiveFourPixels,
    tile_sprite_offset::NoVerticalOffset,
    tile_sprite_offset::NoVerticalOffset,
    tile_sprite_offset::PositiveFourPixels,
    tile_sprite_offset::NoVerticalOffset,
    tile_sprite_offset::NegativeFourPixels,
    tile_sprite_offset::NoVerticalOffset,
};

void initialize_offer_type_pools(Scenario& scenario) {
    for (const OfferTypePool& pool : kOfferTypePools)
        scenario.offerTypesByEffect[pool.effect - 1].assign(
            pool.objectiveIds.begin(),
            pool.objectiveIds.begin() + pool.objectiveCount);
}

}  // namespace

Game::Game(std::unique_ptr<MusicOutput> music_output, int64_t random_seed) {
    world.terrain.mapHalfWidth =
        (world.terrain.mapWidth - 1) * map_geometry::kTileHalfWidthPixels;
    world.terrain.mapScreenWidth =
        (world.terrain.mapWidth + world.terrain.mapHeight)
        * map_geometry::kTileWidthPixels;
    world.terrain.mapScreenHeight =
        (world.terrain.mapWidth + world.terrain.mapHeight)
        * map_geometry::kTileHeightPixels;
    renderer.camera.viewportHalfWidth =
        world.terrain.mapScreenWidth / map_geometry::kViewportScaleFactor;
    renderer.camera.viewportHalfHeight =
        world.terrain.mapScreenHeight / map_geometry::kViewportScaleFactor;
    renderer.camera.viewportBaseX = renderer.camera.viewportHalfWidth;
    renderer.camera.viewportBaseY = renderer.camera.viewportHalfHeight;
    renderer.camera.mapScaleX =
        renderer.camera.viewportHalfWidth * map_geometry::kViewportScaleFactor;
    renderer.camera.mapScaleY =
        renderer.camera.viewportHalfHeight * map_geometry::kViewportScaleFactor;
    memcpy(fleet.forwardTrackSteps, kForwardTrackSteps, sizeof(kForwardTrackSteps));
    memcpy(fleet.reverseTrackSteps, kReverseTrackSteps, sizeof(kReverseTrackSteps));
    memcpy(assets.trainSpriteDirectionTable, kTrainSpriteDirections,
           sizeof(kTrainSpriteDirections));
    initialize_offer_type_pools(scenario);
    memcpy(scenario.offerEffectSchedule, kOfferEffectSchedules,
           sizeof(kOfferEffectSchedules));
    rng.set_seed(random_seed);
    music.output = std::move(music_output);
    renderer.resize_canvas(startup_canvas::Width, startup_canvas::Height);
    memcpy(renderer.graphColors, kGraphColorSets, sizeof(kGraphColorSets));
    memcpy(assets.tileSpriteYOffsetByCode, kTileSpriteYOffsets,
           sizeof(kTileSpriteYOffsets));
}
