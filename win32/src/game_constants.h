// Named game constants recovered during the model stage (MODEL-01).
// Values are unchanged from the original numeric literals; only names are added.
#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H
#include <cstdint>

inline constexpr int kTrainSlotCount = 500;
inline constexpr int kServiceVehicleSlotCount = 250;
inline constexpr int kCitySlotCount = 48;
inline constexpr int kZoneCount = 64;
inline constexpr int kTileQueueCount = 50;
inline constexpr int kResourceTypeCount = 5;
// Service stock uses slots 0..4. Unload progress keeps a separate legacy
// numbering with one reserved slot at index 0; these names follow the proven
// train-car mapping and do not claim richer commodity names.
namespace delivery_progress_slot {
enum Id : int32_t {
    Reserved = 0,
    IndustryGroup3 = 1,
    IndustryGroup2 = 2,
    IndustryGroup1 = 3,
    Passengers = 4,
    Mail = 5,
    AfterLast = 6,
};

inline constexpr int32_t First = IndustryGroup3;
}  // namespace delivery_progress_slot

inline constexpr int kDeliveredResourceCount = delivery_progress_slot::AfterLast;
inline constexpr int kMenuRowCount = 6;
inline constexpr int kOfferActiveRowCount = kMenuRowCount - 1;
inline constexpr int kOfferSummaryRow = kOfferActiveRowCount;
inline constexpr int kEnabledMenuRowCount = 5;
inline constexpr int kHistoryCount = 12;
inline constexpr int kTrainSelectionCount = 64;
inline constexpr int kWagonOptionCount = 5;
inline constexpr int kSelectionCornerCount = 4;
inline constexpr int kTerrainEditGridSide = 10;
inline constexpr int kTileSpriteCodeCount = 255;
inline constexpr int kTileYOffsetCount = 26;
inline constexpr int kTrainSpriteTypeCount = 8;
inline constexpr int kTrainSpriteDirectionCount = 8;
inline constexpr int kFontPageCount = 6;
inline constexpr int kTrackDirectionCount = 6;
inline constexpr int kTrackStepValueCount = 12;
inline constexpr int kTrackTransitionCount = kTrackStepValueCount / 2 - 2;
inline constexpr int kScenarioCount = 13;
inline constexpr int kLeaderboardScoreCount = 14;
inline constexpr int kOfferEffectCount = 12;
inline constexpr int kOfferScheduleFieldCount = 2;
inline constexpr int kPlayerNameCodeCount = 10;
inline constexpr int kGraphColorCount = 9;
inline constexpr int kNoFleetSlot = 5000;
inline constexpr int kNoTrainSlot = kTrainSlotCount;

// Integer percentages in the legacy model use a whole-percent base rather
// than a floating-point fraction.
namespace percentage {
inline constexpr int32_t kBase = 100;
}  // namespace percentage

// Free Game setup levels. These are runtime-only menu values; the first two
// map records in scenarios.bin still hold the small and large base maps.
namespace free_game_map_size {
enum Id : int8_t {
    Small = 0,
    Medium = 1,
    Large = 2,
    AfterLast = 3,
};
}  // namespace free_game_map_size

namespace free_game_town_level {
enum Id : int8_t {
    Few = 0,
    Medium = 1,
    Lots = 2,
    Megapolis = 3,
    AfterLast = 4,
};
}  // namespace free_game_town_level

namespace free_game_industry_level {
enum Id : int8_t {
    Few = 0,
    Medium = 1,
    Lots = 2,
    AfterLast = 3,
};
}  // namespace free_game_industry_level

// Key-state sentinels used by the host input loop. Zero means no pending key;
// the negative value marks a release event for the next frame.
namespace input_key {
inline constexpr int32_t NoKey = 0;
inline constexpr int32_t Released = -999;
}  // namespace input_key

// The game receives ASCII numeric-keypad codes after the host input adapter.
// Keep these separate from MIDP action values below: both protocols are used
// by the same input path.
namespace keypad_key {
inline constexpr int32_t kUpLeft = '1';
inline constexpr int32_t kUp = '2';
inline constexpr int32_t kUpRight = '3';
inline constexpr int32_t kLeft = '4';
inline constexpr int32_t kSelect = '5';
inline constexpr int32_t kRight = '6';
inline constexpr int32_t kDownLeft = '7';
inline constexpr int32_t kDown = '8';
inline constexpr int32_t kDownRight = '9';
}  // namespace keypad_key

namespace game_action {
inline constexpr int32_t kUp = 1;
inline constexpr int32_t kLeft = 2;
inline constexpr int32_t kRight = 5;
inline constexpr int32_t kDown = 6;
inline constexpr int32_t kSelect = 8;
}  // namespace game_action

// Isometric map dimensions use these fixed pixel factors in both initial
// state construction and scenario setup.
namespace map_geometry {
inline constexpr int32_t kTileHalfWidthPixels = 8;
inline constexpr int32_t kTileWidthPixels = 16;
inline constexpr int32_t kTileHeightPixels = 8;
inline constexpr int32_t kElevationPixelScale = 4;
inline constexpr int32_t kViewportScaleFactor = 2;
}  // namespace map_geometry

namespace game_calendar {
inline constexpr int32_t kFirstDay = 1;
inline constexpr int32_t kFirstMonth = 1;
inline constexpr int32_t kBaseYear = 1950;
}  // namespace game_calendar

// Header money-change ticker slots. Newest changes are drawn last so they
// appear closest to the header; the three slots age independently.
namespace money_ticker {
enum Slot : int32_t {
    Oldest = 2,
    Middle = 1,
    Newest = 0,
};
inline constexpr int32_t kSlotCount = 3;
inline constexpr int32_t kFrameDuration = 80;
}  // namespace money_ticker

// Train engine eras used by the saved TrainCar::era field and the six wagon
// price columns in Economy. The labels describe order only; the source data
// does not prove historical era names.
namespace train_era {
enum Id : int32_t {
    First = 0,
    Second = 1,
    Third = 2,
    Fourth = 3,
    Fifth = 4,
    Sixth = 5,
};
}  // namespace train_era

// Cargo option and car-type ids used by the build menu, map setup, and train
// load/unload code. The group labels are deliberately neutral: the source
// data gives stable groups, while the resource names live in service_resource.
namespace wagon {
enum OptionSlotId : int32_t {
    NoOption = -1,
    PassengerMail = 0,
    IndustryGroup1 = 1,
    IndustryGroup2 = 2,
    IndustryGroup3 = 3,
};

enum CargoCarTypeId : int32_t {
    NoCar = -1,
    EngineTypeA = 0,
    EngineTypeB = 1,
    PassengerMailCar = 2,
    IndustryGroup1Car = 3,
    IndustryGroup2CarA = 4,
    IndustryGroup2CarB = 5,
    IndustryGroup3CarA = 6,
    IndustryGroup3CarB = 7,
};
}  // namespace wagon

// Packed train state-code fields. The byte layout is saved legacy data: three
// bits per wagon option after the three-bit engine field.
namespace train_state_code {
inline constexpr int32_t PackedFieldWidth = 3;
inline constexpr int32_t PackedFieldMask = 7;
inline constexpr int32_t FirstWagonFieldShift = 3;
inline constexpr int32_t EmptyWagonCode = 7;
inline constexpr int32_t WagonTypeBase = 2;
inline constexpr int32_t SkippedCarType = 5;
}  // namespace train_state_code

// Train direction values used by the renderer. The save data proves twelve
// stable slots, but not their full compass or sprite meaning, so the labels
// stay ordinal until the legacy direction map is recovered.
namespace train_direction_slot {
enum Id : int32_t {
    Slot0 = 0,
    Slot1 = 1,
    Slot2 = 2,
    Slot3 = 3,
    Slot4 = 4,
    Slot5 = 5,
    Slot6 = 6,
    Slot7 = 7,
    Slot8 = 8,
    Slot9 = 9,
    Slot10 = 10,
    Slot11 = 11,
};
}  // namespace train_direction_slot

// Track-step lookup values are also saved as twelve neutral slots. They are
// separate from the renderer direction labels because their tile-variant
// meaning is not fully decoded.
namespace track_variant_slot {
enum Id : int32_t {
    Variant0 = 0,
    Variant1 = 1,
    Variant2 = 2,
    Variant3 = 3,
    Variant4 = 4,
    Variant5 = 5,
    Variant6 = 6,
    Variant7 = 7,
    Variant8 = 8,
    Variant9 = 9,
    Variant10 = 10,
    Variant11 = 11,
};
}  // namespace track_variant_slot

// Scenario milestone ids used by progress, completion, and dialog display.
// Combined entries keep their legacy threshold pairings; the names describe
// only the proven relationship between the saved counters.
namespace scenario_milestone {
enum Id : int32_t {
    Threshold1 = 1,
    Threshold2 = 2,
    Threshold3 = 3,
    Threshold4 = 4,
    Threshold5 = 5,
    CombinedThreshold1And2 = 6,
    CombinedThreshold1And3 = 7,
    CombinedThreshold1And4 = 8,
    CombinedThreshold2And3 = 9,
    CombinedThreshold2And4 = 10,
    CombinedThreshold4And5 = 11,
    LifetimeRevenue = 12,
    LoanCleared = 13,
    TargetYear = 14,
    WagonsBought = 15,
};
}  // namespace scenario_milestone

namespace scenario_flag {
inline constexpr int32_t Enabled = 0x01;
inline constexpr int32_t AiExpansion = 0x02;
inline constexpr int32_t MonthlyOffers = 0x04;
}  // namespace scenario_flag

namespace offer_schedule {
enum Field : int32_t {
    FirstDelay = 0,
    RepeatDelay = 1,
};
}  // namespace offer_schedule

// Offer effect ids. These names describe the operations observed in the
// economy update; the original data does not expose a richer domain model.
namespace offer_effect {
enum Id : int32_t {
    None = 0,
    CashPercentOfTarget = 1,
    FixedCash = 2,
    LoanCeilingIncrease = 3,
    EraAdvance = 4,
    ResourceFlag1 = 5,
    ResourceFlag2 = 6,
    ResourceFlag3 = 7,
    ResourceFlag4 = 8,
    ResourceFlag5 = 9,
    ResourceFlag6 = 10,
    LoanBalanceReduction = 11,
    InterestRateReduction = 12,
};

inline constexpr int32_t kFirstResourceFlag = ResourceFlag1;
inline constexpr int32_t kLastResourceFlag = ResourceFlag6;
}  // namespace offer_effect

// Objective ids carried by Offer::objectiveType. The native data proves the
// stable ids and their progress ranges, but not richer resource names, so the
// individual labels stay deliberately neutral until the offer assets are
// decoded.
namespace offer_objective {
enum Id : int32_t {
    None = 0,
    Objective1 = 1,
    Objective2 = 2,
    Objective3 = 3,
    Objective4 = 4,
    Objective5 = 5,
    Objective6 = 6,
    Objective7 = 7,
    Objective8 = 8,
    Objective9 = 9,
    Objective10 = 10,
    Objective11 = 11,
    Objective12 = 12,
    Objective13 = 13,
    Objective14 = 14,
    Objective15 = 15,
};

inline constexpr int32_t kFirstGenerated = Objective1;
inline constexpr int32_t kLastGenerated = Objective14;
inline constexpr int32_t kFirstProgress = Objective4;
inline constexpr int32_t kLastProgress = Objective14;
inline constexpr int32_t kFirstCargoCount = Objective9;
inline constexpr int32_t kLastCargoCount = Objective12;
}  // namespace offer_objective

// Runtime offer-navigation states. They are saved only indirectly through
// the currently visible overlay; the names describe which offer screen is
// shown when a response dialog closes.
namespace offer_flow {
enum State : int32_t {
    Selection = 1,
    Summary = 2,
    EffectChoice = 3,
    ObjectiveChoice = 4,
};
}  // namespace offer_flow

// Report-series selectors used by the yearly and decade graph buffers.
namespace report_series {
enum Id : int32_t {
    Profit = 0,
    Revenue = 1,
    CompanyValue = 2,
};
}  // namespace report_series

// Values for Game::gameState and its saved previous-state copy. The field stays
// int32_t because it is part of the save stream.
//
// Flow: BootSplash0 -> BootSplash1 (loads fonts) -> IntroReady -> MenuOverlay.
// Splash states 2 and 3 are defined by the switches but unreached in the
// current boot path (BootSplash1 jumps straight to IntroReady).
enum GameState : int32_t {
    BootSplash0      = 0,    // first splash: opens the sprite stream; black frame
    BootSplash1      = 1,    // second splash: decodes fonts, then -> IntroReady
    BootSplash2      = 2,    // defined by the switches; unreached in current flow
    BootSplash3      = 3,    // defined by the switches; unreached in current flow
    IntroReady       = 4,    // fonts ready: arm the main menu, then -> MenuOverlay
    LoadingResources = 10,   // load scenario sprites and music (progress bar)
    MenuOverlay      = 20,   // packed-sprite menu/dialog overlay and its dispatch
    GeneratingMap    = 25,   // map generation progress screen
    Playing          = 30,   // in-game map, money ticker, and input
    BuildingMinimap  = 35,   // build the minimap image, then -> MapOverview
    MapOverview      = 40,   // minimap / overview with pan
    BuildingPath     = 45,   // compute a train route/path
    SavingGame       = 55,   // write the map arrays to the save
    LoadingGame      = 65,   // restore the map arrays from the save
    AiTurn           = 105,  // AI turn: place cities/industry or advance
    HighScoreSync    = 120,  // upload/sync high scores (network)
};

// Track-map byte encoding (Game::trackMap). A byte packs a "track present"
// flag in bit 6 with a 6-bit owner field in the low bits. At the crossing tile
// (tile::CROSSING) the owner field holds two 3-bit sub-owners, one per crossing
// direction. The high overlay values are named below after their junction and
// station uses were confirmed at the read/write sites.
namespace track {
inline constexpr int32_t PRESENT = 0x40;        // bit 6: a track piece is on this tile
inline constexpr int32_t OWNER_MASK = 0x3F;     // low 6 bits: the owner field
inline constexpr int32_t DIR_OWNER_MASK = 7;    // one crossing direction's 3-bit owner
inline constexpr int32_t DIR_OWNER_SHIFT = 3;   // shift to the other direction's owner
inline constexpr int32_t PRESENT_AND_LOW_DIRECTION_MASK =
    PRESENT | DIR_OWNER_MASK;
inline constexpr int32_t PRESENT_AND_UPPER_DIRECTION_MASK =
    PRESENT | (DIR_OWNER_MASK << DIR_OWNER_SHIFT);
inline constexpr int32_t PRESENT_WITH_LOW_OWNER_ONE = PRESENT | 1;
inline constexpr int32_t PRESENT_WITH_UPPER_OWNER_ONE =
    PRESENT | (1 << DIR_OWNER_SHIFT);
inline constexpr int32_t AI_PIECE_FIRST = 66;   // first AI route piece code (owner 2)
inline constexpr int32_t AI_PIECE_WRAP = 69;    // one past the last AI piece code
}  // namespace track

// High track-map bits mark junction and station overlays. Their exact binary
// form is stable at every read/write site: 0x80 is a junction and 0xC0 is a
// station.
namespace track_overlay {
inline constexpr int32_t MASK = 0xC0;
inline constexpr int32_t JUNCTION = 0x80;
inline constexpr int32_t STATION = 0xC0;
}  // namespace track_overlay

// Tile-map sentinels (Game::tileMap). Most tile ids are track, bridge, and
// road shape codes recorded in DECISIONS; only the strongly-evidenced sentinel
// is named here.
namespace tile {
inline constexpr int32_t CROSSING = 76;      // two tracks cross; trackMap holds two owners
inline constexpr int32_t SLOPE_FIRST = 3;    // generated terrain slope sprites
inline constexpr int32_t SLOPE_LAST = 28;
inline constexpr int32_t SCENERY_FIRST = 29; // passable scenery/obstacle tiles (5 variants)
inline constexpr int32_t SCENERY_LAST = 33;
inline constexpr int32_t BUILT_FIRST = 34;   // first non-terrain (built) tile; city zone starts here
inline constexpr int32_t CITY_ZONE_LAST = 40; // last city population zone tile
inline constexpr int32_t TRACK_FIRST = 70;   // track, bridge, and station shapes
inline constexpr int32_t TRACK_LAST = 90;
inline constexpr int32_t CROSSING_VARIANT = CROSSING - TRACK_FIRST;
inline constexpr int32_t SERVICE_TRACK = 65; // station/service track owner code
inline constexpr int32_t PATH_ANCHOR_MARKER = 127; // temporary path-menu marker
inline constexpr int32_t VERTICAL_STATION = 87;
inline constexpr int32_t HORIZONTAL_STATION = 88;
inline constexpr int32_t VERTICAL_TERMINAL = 89;
inline constexpr int32_t HORIZONTAL_TERMINAL = 90;
// Four curve/step tiles used by path finding and the cursor preview. The
// names describe their proven neighboring step, not a sprite orientation.
inline constexpr int32_t CURVE_UP = 7;
inline constexpr int32_t CURVE_LEFT = 8;
inline constexpr int32_t CURVE_RIGHT = 9;
inline constexpr int32_t CURVE_DOWN = 10;
}  // namespace tile

// The seven generated city-zone tile variants. Their art names are not
// decoded, so the labels remain ordinal.
namespace city_zone_tile {
enum Variant : int32_t {
    VariantA = tile::BUILT_FIRST,
    VariantB = VariantA + 1,
    VariantC = VariantA + 2,
    VariantD = VariantA + 3,
    VariantE = VariantA + 4,
    VariantF = VariantA + 5,
    VariantG = tile::CITY_ZONE_LAST,
};
inline constexpr int32_t First = VariantA;
inline constexpr int32_t Last = VariantG;
}  // namespace city_zone_tile

// Construction overlays are grouped by the axis scanned by demolition and
// terrain queries. The individual sprite meaning is intentionally neutral.
namespace construction_tile {
enum Id : int32_t {
    HorizontalA = 81,
    VerticalA = 82,
    VerticalB = 83,
    HorizontalB = 84,
    HorizontalC = 85,
    VerticalC = 86,
};
inline constexpr int32_t kFirst = HorizontalA;
inline constexpr int32_t kLast = VerticalC;
}  // namespace construction_tile

// Four turn/bridge tile variants used by movement and rendering. Their full
// visual direction map is not proven, so the names stay ordinal.
namespace track_turn_tile {
enum Id : int32_t {
    VariantA = 77,
    VariantB = 78,
    VariantC = 79,
    VariantD = 80,
};
}  // namespace track_turn_tile

// Neutral tile roles used by path finding and the track-piece builder. The
// source proves the byte relationships and neighbor roles, but not richer
// sprite names.
namespace track_build_tile {
enum Id : int32_t {
    OpenGroundVariantA = 21,
    OpenGroundVariantB = 22,
    OpenGroundVariantC = 23,
    OpenGroundVariantD = 24,
    AlongY = 70,
    AlongX = 71,
    CornerVariantA = 72,
    CornerVariantB = 73,
    CornerVariantC = 74,
    CornerVariantD = 75,
};
}  // namespace track_build_tile

// Build-type values passed to World::is_tile_buildable. The menu cursor uses
// the same ordinal protocol after its first build-type action.
namespace terrain_build_type {
inline constexpr int32_t SingleTile = 0;
inline constexpr int32_t DiagonalA = 3;
inline constexpr int32_t DiagonalB = 4;
inline constexpr int32_t VerticalTerminal = 5;
inline constexpr int32_t HorizontalTerminal = 6;
}  // namespace terrain_build_type

// Service-site type nibbles. Rail sites use the original value; road sites
// use the separate value introduced by the road subsystem.
namespace service_site_type {
enum Id : int32_t {
    Rail = 1,
    Road = 2,
};
}  // namespace service_site_type

// Service-site flags and resource-stock indexes. Passenger and mail slots are
// known from the load path; the three middle slots stay neutral because the
// legacy source/destination groups do not prove commodity names.
namespace service_site {
inline constexpr uint8_t USED_SLOT = 0x80;
inline constexpr uint8_t HORIZONTAL = 0x40;
inline constexpr uint8_t EXTENDED = 0x20;
inline constexpr uint8_t TYPE_MASK = 0x07;
}  // namespace service_site

namespace service_resource {
enum Id : int32_t {
    Passengers = 0,
    IndustryGroup1 = 1,
    IndustryGroup2 = 2,
    IndustryGroup3 = 3,
    Mail = 4,
};

inline constexpr uint8_t PASSENGER_FLAG = 0x01;
inline constexpr uint8_t INDUSTRY_GROUP1_FLAG = 0x04;
inline constexpr uint8_t INDUSTRY_GROUP2_FLAG = 0x08;
inline constexpr uint8_t INDUSTRY_GROUP3_FLAG = 0x02;
inline constexpr uint8_t MAIL_FLAG = 0x10;
}  // namespace service_resource

// Menu/dialog transition-code bases (Game::transitionCode and
// pendingTransitionCode). The per-screen menu ids are UI navigation and are
// named with the UI controller in UI-01; only the arithmetic bases that build a
// code from model data are named here. See DECISIONS "Recovered in MODEL-01".
namespace transition {
inline constexpr int32_t OFFER_TYPE_BASE = 40;      // objectiveType + this
inline constexpr int32_t OFFER_ALT_BASE = 60;       // effectType + this
inline constexpr int32_t SCENARIO_NOTICE_BASE = 80; // scenarioNotice id   + this
}  // namespace transition

// Offer screens that are entered by both the economy tick and the menu
// dispatcher. The other packed-sprite screens stay local to their dispatcher
// until their shared protocol is proved.
namespace offer_screen {
enum Id : int32_t {
    ObjectiveChoice = 8,
    EffectChoice = 9,
    Summary = 40,
    Selection = 60,
};
}  // namespace offer_screen

// Packed-sprite menu screen ids shared by the state tick, input, painting,
// and menu dispatcher. GenericOverlay* and OfferResponseNN stay neutral where
// the resource data does not prove a narrower screen meaning.
namespace menu_screen {
enum Id : int32_t {
    MainMenu = 1,
    GenericOverlayA = 2,
    GenericOverlayB = 3,
    ScenarioSettings = 4,
    ScenarioSelection = 5,
    ContinueGame = 6,
    AiScenarioSetup = 7,
    OfferTypeChoice = 8,
    OfferEffectChoice = 9,
    Settings = 10,
    GenericOverlayG = 11,
    GenericOverlayH = 12,
    GenericOverlayI = 13,
    SaveLoadDialog = 14,
    ResourceLoading = 15,
    GenericOverlayJ = 16,
    ScenarioNotice = 17,
    GenericOverlayE = 18,
    GenericOverlayF = 19,
    GenericOverlayC = 20,
    GenericOverlayD = 21,
    TrainBuildReset = 22,
    TrainPurchase = 23,
    TrainTypeSelection = 24,
    WagonOptionSelection = 25,
    TrainUpgrade = 26,
    HighScoreEntry = 27,
    HighScoreMenu = 28,
    LoanDialog = 30,
    MonthlyReport = 31,
    ReportComparison = 32,
    ReportGraph = 33,
    AlternateReportGraph = 34,
    AnnualReport = 35,
    MapOverview = 36,
    OfferSummary = 40,
    OfferResponse41 = 41,
    OfferResponse42 = 42,
    OfferResponse44 = 44,
    OfferResponse45 = 45,
    OfferResponse46 = 46,
    OfferResponse47 = 47,
    OfferResponse48 = 48,
    OfferResponse49 = 49,
    OfferResponse50 = 50,
    OfferResponse51 = 51,
    OfferResponse52 = 52,
    OfferResponse53 = 53,
    OfferResponse54 = 54,
    OfferSelection = 60,
    OfferResponse61 = 61,
    OfferResponse62 = 62,
    OfferResponse63 = 63,
    OfferResponse64 = 64,
    OfferResponse65 = 65,
    OfferResponse66 = 66,
    OfferResponse67 = 67,
    OfferResponse68 = 68,
    OfferResponse69 = 69,
    OfferResponse70 = 70,
    OfferResponse71 = 71,
    OfferResponse72 = 72,
    RoadVehicleDialog = 100,
};
}  // namespace menu_screen

namespace menu_transition {
inline constexpr int32_t NoPending = -1;
inline constexpr int32_t Initial = 0;
}  // namespace menu_transition

namespace save_load_mode {
enum Id : int32_t {
    SaveGame = 0,
    LoadGame = 1,
    ExitGame = 2,
    LoadResources = 3,
    CancelTrainPurchase = 4,
};
}  // namespace save_load_mode

// Build-menu row ids and stable item ids. These values are shared by the
// menu table, input, and build-action dispatch.
namespace build_menu {
enum Mode : int32_t {
    Toolbar = 0,
    RailTools = 1,
    TrackBuildConfirm = 2,
    StationTools = 3,
    PathTarget = 4,
    TrackPieceSelection = 5,
    RoadTools = 6,
};

inline constexpr int32_t kModeCount = 7;
inline constexpr int32_t kBackItem = 9;
inline constexpr int32_t kNoScenarioOverride = -1;

// Features packed in Scenario::scenarioBits for the build-menu layout. This
// byte is separate from scenario_flag, which describes scenario definitions.
namespace scenario_feature {
inline constexpr int32_t Bank = 0x01;
inline constexpr int32_t Graphs = 0x02;
inline constexpr int32_t Tasks = 0x04;
inline constexpr int32_t RouteAnchor = 0x08;
inline constexpr int32_t BridgeOrTunnel = 0x10;
inline constexpr int32_t TerrainSmoothing = 0x20;
inline constexpr int32_t Terminals = 0x40;
}  // namespace scenario_feature

namespace toolbar_item {
enum Id : int32_t {
    SelectTrain = 0,
    RailToolsEntry = 1,
    Bank = 2,
    GraphsAndStats = 3,
    Tasks = 4,
    CurrentMission = 5,
    TimeCompression = 8,
    MainMenu = 9,
    Pause = 11,
    RoadToolsEntry = 12,
};
}  // namespace toolbar_item

namespace rail_item {
enum Id : int32_t {
    RouteAnchor = 0,
    TrackPieceMenu = 1,
    BridgeOrTunnel = 2,
    StationMenu = 3,
    Demolish = 4,
    SmoothDown = 5,
    SmoothUp = 6,
};
}  // namespace rail_item

namespace station_item {
enum Id : int32_t {
    VerticalStation = 0,
    HorizontalStation = 1,
    VerticalTerminal = 2,
    HorizontalTerminal = 3,
};
}  // namespace station_item

namespace track_piece_item {
enum Id : int32_t {
    VerticalTrack = 0,
    HorizontalTrack = 1,
    DiagonalTrackA = 2,
    DiagonalTrackB = 3,
};
}  // namespace track_piece_item

namespace road_item {
enum Id : int32_t {
    LayTile = 0,
    AutoRoute = 1,
    PlaceStop = 2,
    BuyVehicle = 3,
};
}  // namespace road_item

namespace confirm_item {
inline constexpr int32_t Confirm = 0;
}  // namespace confirm_item

// Cursor-action codes stored beside each build-menu item. The renderer uses
// these values to decide which preview or selection colour to draw; the names
// describe that UI protocol, not the underlying asset ids.
namespace cursor_action {
inline constexpr int32_t kNone = 0;
inline constexpr int32_t kBuildTypeFirst = 1;
inline constexpr int32_t kBuildTypeSecond = 2;
inline constexpr int32_t kBuildTypeThird = 3;
inline constexpr int32_t kBuildTypeFourth = 4;
inline constexpr int32_t kBuildTypeFifth = 5;
inline constexpr int32_t kVerticalTerminal = 6;
inline constexpr int32_t kHorizontalTerminal = 7;
inline constexpr int32_t kPlaceableTerrain = 8;
inline constexpr int32_t kCityOrStation = 9;
inline constexpr int32_t kNeutral = 10;
inline constexpr int32_t kCurve = 11;
inline constexpr int32_t kRoadBuildable = 12;
inline constexpr int32_t kRoadTile = 13;
inline constexpr int32_t kRoadStopBuildable = 14;
inline constexpr int32_t kRoadStop = 15;
}  // namespace cursor_action

// Confirmation-result codes shown by the build-menu bar. The table stores
// these as byte values and the dialog layer resolves them to text; names are
// based on the menu action that supplies each code.
namespace result_code {
enum Id : int32_t {
    None = 0,
    SmoothUp = 1,
    SmoothDown = 2,
    RailToolsEntry = 3,
    MainMenu = 5,
    RouteAnchor = 8,
    VerticalTrack = 9,
    HorizontalTrack = 10,
    DiagonalTrackA = 11,
    DiagonalTrackB = 12,
    BridgeOrTunnel = 13,
    Demolish = 14,
    StationMenu = 15,
    Back = 17,
    Confirm = 18,
    Cancel = 19,
    VerticalStation = 20,
    HorizontalStation = 21,
    VerticalTerminal = 22,
    HorizontalTerminal = 23,
    RouteTarget = 24,
    Bank = 25,
    GraphsAndStats = 26,
    Tasks = 27,
    CurrentMission = 28,
    TimeCompression = 29,
    Pause = 31,
};
}  // namespace result_code
}  // namespace build_menu

// Road tile shapes written to World::terrain.tileMap. The two straight and
// four corner orientations stay category-based: the old map generator and the
// newer road editor use different axis words for the same numeric codes.
namespace road_tile {
inline constexpr int32_t kFirstShape = 42;
inline constexpr int32_t kStraightShape42 = 42;
inline constexpr int32_t kStraightShape43 = 43;
inline constexpr int32_t kJunctionShape44 = 44;
inline constexpr int32_t kCornerShape45 = 45;
inline constexpr int32_t kCornerShape46 = 46;
inline constexpr int32_t kCornerShape47 = 47;
inline constexpr int32_t kCornerShape48 = 48;
inline constexpr int32_t kLastShape = kCornerShape48;
}  // namespace road_tile

// The oriented street shape (42..48) for a road cell from its 4 road
// neighbours: a junction for 3+ links, a corner for a bend, a straight along
// the linked axis, and 42 for a stub. This is the single rule shared by map
// generation, city growth, and the player road tools.
inline constexpr int32_t road_shape_from_neighbors(bool left, bool right,
                                                   bool up, bool down) {
    const int neighbor_count = (left ? 1 : 0) + (right ? 1 : 0)
        + (up ? 1 : 0) + (down ? 1 : 0);
    if (neighbor_count > 2) return road_tile::kJunctionShape44;
    if (right && up) return road_tile::kCornerShape45;
    if (right && down) return road_tile::kCornerShape46;
    if (left && down) return road_tile::kCornerShape47;
    if (left && up) return road_tile::kCornerShape48;
    if ((left || right) && !up && !down) return road_tile::kStraightShape43;
    return road_tile::kStraightShape42;
}

// Neutral tile ranges used by the six generated industry groups. The numeric
// data proves the groups and their source/destination ranges, but not real
// commodity names.
namespace industry_tile {
inline constexpr int32_t Group1DestinationFirst = 50;
inline constexpr int32_t Group1DestinationLast = 52;
inline constexpr int32_t Group2ProductionFirst = 53;
inline constexpr int32_t Group2ProductionLast = 55;
inline constexpr int32_t Group2DestinationFirst = 56;
inline constexpr int32_t Group2DestinationLast = 58;
inline constexpr int32_t Group3ProductionFirst = 59;
inline constexpr int32_t Group3ProductionLast = 61;
inline constexpr int32_t Group3DestinationFirst = 62;
inline constexpr int32_t Group3DestinationLast = 64;
inline constexpr int32_t Group1ProductionFirst = 65;
inline constexpr int32_t Group1ProductionLast = 68;
}  // namespace industry_tile

// Save-codec direction (SaveCodec::savePendingMode, also the mode argument of
// SaveCodec::load_slot). One IO_* field walk serves save and load; the mode decides which
// way bytes move. See save.cpp / save_arrays.cpp.
namespace save {
inline constexpr int32_t MODE_WRITE = 0;  // serialize fields into the buffer
inline constexpr int32_t MODE_READ = 1;   // deserialize fields from the buffer
inline constexpr int32_t MODE_SKIP = 2;   // touch nothing: return 0 and zero arrays

// SaveCodec::walk operation selector. The memory operations are used by the
// in-memory save tests and keep the legacy buffer order.
inline constexpr int32_t OP_SAVE_MAIN = 0;   // write the main snapshot to slot 0
inline constexpr int32_t OP_LOAD_MAIN = 1;   // read the main snapshot from slot 0
inline constexpr int32_t OP_RESET_SKIP = 2;  // reset the codec in skip mode (no I/O)
inline constexpr int32_t OP_MEMORY_WRITE = 4;  // clear the buffer and write
inline constexpr int32_t OP_MEMORY_READ = 5;   // read the current buffer

// Record slots used by SaveCodec::save_all/restore_all. Settings and road
// vehicles need separate slots so saving either record cannot erase the other.
namespace slot {
inline constexpr int32_t Main = 0;
inline constexpr int32_t TileMap = 1;
inline constexpr int32_t TrackMap = 2;
inline constexpr int32_t TileElevation = 3;
inline constexpr int32_t HeightMap = 4;
inline constexpr int32_t Auxiliary = 5;
inline constexpr int32_t RoadVehicles = 6;
}  // namespace slot
}  // namespace save

#endif  // GAME_CONSTANTS_H
