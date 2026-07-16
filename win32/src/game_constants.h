// Named game constants recovered during the model stage (MODEL-01).
// Values are unchanged from the original numeric literals; only names are added.
#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H
#include <cstdint>

// Values for GameCanvas::gameState (and its saved previous-state copy `hx`).
// The field stays int32_t for save and arithmetic safety; these names replace
// the raw state literals at read and write sites. Use unary `+` to get the
// int32_t value, e.g. gc_setState(gc, +GameState::Playing).
//
// Flow: BootSplash0 -> BootSplash1 (loads fonts) -> IntroReady -> MenuOverlay.
// Splash states 2 and 3 are defined by the switches but unreached in the
// current boot path (BootSplash1 jumps straight to IntroReady).
enum class GameState : int32_t {
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

// Unary plus yields the underlying int32_t so the value fits the int32_t
// gameState field, switch subjects, and case labels without a verbose cast.
constexpr int32_t operator+(GameState state) noexcept {
    return static_cast<int32_t>(state);
}

// Track-map byte encoding (GameCanvas::trackMap). A byte packs a "track present"
// flag in bit 6 with a 6-bit owner field in the low bits. At the crossing tile
// (tile::CROSSING) the owner field holds two 3-bit sub-owners, one per crossing
// direction. Bits 0x80/0xC0 mark other trackMap overlays whose exact meaning is
// not yet recovered; see DECISIONS "Recovered in MODEL-01".
namespace track {
inline constexpr int32_t PRESENT = 0x40;        // bit 6: a track piece is on this tile
inline constexpr int32_t OWNER_MASK = 0x3F;     // low 6 bits: the owner field
inline constexpr int32_t DIR_OWNER_MASK = 7;    // one crossing direction's 3-bit owner
inline constexpr int32_t DIR_OWNER_SHIFT = 3;   // shift to the other direction's owner
inline constexpr int32_t AI_PIECE_FIRST = 66;   // first AI route piece code (owner 2)
inline constexpr int32_t AI_PIECE_WRAP = 69;    // one past the last AI piece code
}  // namespace track

// Tile-map sentinels (GameCanvas::tileMap). Most tile ids are track, bridge, and
// road shape codes recorded in DECISIONS; only the strongly-evidenced sentinel
// is named here.
namespace tile {
inline constexpr int32_t CROSSING = 76;      // two tracks cross; trackMap holds two owners
inline constexpr int32_t SCENERY_FIRST = 29; // passable scenery/obstacle tiles (5 variants)
inline constexpr int32_t SCENERY_LAST = 33;
inline constexpr int32_t BUILT_FIRST = 34;   // first non-terrain (built) tile; city zone starts here
inline constexpr int32_t CITY_ZONE_LAST = 40; // last city population zone tile
}  // namespace tile

// Menu/dialog transition-code bases (GameCanvas::transitionCode and
// pendingTransitionCode). The per-screen menu ids are UI navigation and are
// named with the UI controller in UI-01; only the arithmetic bases that build a
// code from model data are named here. See DECISIONS "Recovered in MODEL-01".
namespace transition {
inline constexpr int32_t OFFER_TYPE_BASE = 40;      // menuRowType[row]    + this
inline constexpr int32_t OFFER_ALT_BASE = 60;       // menuRowAltType[row] + this
inline constexpr int32_t SCENARIO_NOTICE_BASE = 80; // scenarioNotice id   + this
}  // namespace transition

// Save-codec direction (GameCanvas::savePendingMode, also the mode argument of
// gc_loadGame). One IO_* field walk serves save and load; the mode decides which
// way bytes move. See save.cpp / save_arrays.cpp.
namespace save {
inline constexpr int32_t MODE_WRITE = 0;  // serialize fields into the buffer
inline constexpr int32_t MODE_READ = 1;   // deserialize fields from the buffer
inline constexpr int32_t MODE_SKIP = 2;   // touch nothing: return 0 and zero arrays

// gc_initSaveOrLoad operation selector. Only these three values are reached.
// Selector values 4 and 5 exist in gc_initSaveOrLoad but are never called; that
// dead branch is a smell recorded for a separate cleanup, not removed here.
inline constexpr int32_t OP_SAVE_MAIN = 0;   // write the main snapshot to slot 0
inline constexpr int32_t OP_LOAD_MAIN = 1;   // read the main snapshot from slot 0
inline constexpr int32_t OP_RESET_SKIP = 2;  // reset the codec in skip mode (no I/O)
}  // namespace save

#endif  // GAME_CONSTANTS_H
