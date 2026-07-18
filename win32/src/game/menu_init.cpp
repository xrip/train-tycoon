// Build-menu tables for a scenario.
#include "game.h"

namespace {

constexpr int32_t kBuildMenuItemCodeStep = 15;
constexpr int32_t kWideMenuItemCodeExtra = 2;
constexpr int32_t kToolbarBaseItemCount = 6;
constexpr int32_t kRailToolsBaseItemCount = 4;
constexpr int32_t kTrackBuildConfirmItemCount = 2;
constexpr int32_t kStationToolsBaseItemCount = 3;
constexpr int32_t kPathTargetItemCount = 2;
constexpr int32_t kTrackPieceItemCount = 5;
constexpr int32_t kRoadToolItemCount = 5;

// Menu sprite codes are resource-table ids. The names describe the menu slot
// that uses each code; they do not claim a recovered name for the graphic.
namespace menu_sprite {
constexpr int32_t kToolbarTrainSelection = 202;
constexpr int32_t kRailToolsEntry = 205;
constexpr int32_t kRoadToolsEntry = 44;
constexpr int32_t kBank = 208;
constexpr int32_t kGraphsAndStats = 218;
constexpr int32_t kCurrentMission = 207;
constexpr int32_t kTasks = 206;
constexpr int32_t kPause = 236;
constexpr int32_t kTimeCompression = 234;
constexpr int32_t kMainMenu = 210;
constexpr int32_t kRouteAnchor = 211;
constexpr int32_t kBridgeOrTunnel = 216;
constexpr int32_t kStationMenu = 217;
constexpr int32_t kDemolish = 219;
constexpr int32_t kSmoothDown = 203;
constexpr int32_t kSmoothUp = 204;
constexpr int32_t kBack = 201;
constexpr int32_t kConfirm = 225;
constexpr int32_t kCancel = 226;
constexpr int32_t kVerticalStation = 230;
constexpr int32_t kHorizontalStation = 231;
constexpr int32_t kVerticalTerminal = 232;
constexpr int32_t kHorizontalTerminal = 233;
constexpr int32_t kVerticalTrack = 212;
constexpr int32_t kHorizontalTrack = 213;
constexpr int32_t kDiagonalTrackA = 214;
constexpr int32_t kDiagonalTrackB = 215;
}  // namespace menu_sprite

}  // namespace

void UiController::init_build_menu_row(int32_t menu_mode, int32_t item_count) {
    const auto row_length = static_cast<std::size_t>(item_count);
    buildMenuItems[menu_mode].assign(row_length, 0);
    buildMenuResultCodes[menu_mode].assign(row_length, 0);
    menuItemCode[menu_mode].assign(row_length, 0);
    menuTextIndex[menu_mode].assign(row_length, 0);
    menuTransitionCode[menu_mode].assign(row_length, 0);
    menuRowEnabled[menu_mode].assign(row_length, false);
    buildMenuItemCursor = 0;
    buildMenuModeIndex = menu_mode;
    buildMenuItemCode = 0;
}

void UiController::add_build_menu_item(int32_t text_sprite_code,
                                       int32_t menu_item_id,
                                       int32_t confirm_result_code,
                                       bool wide_item, bool item_enabled,
                                       int32_t cursor_action_code) {
    const int menu_mode = buildMenuModeIndex;
    const int item_index = buildMenuItemCursor;
    buildMenuItems[menu_mode][item_index] = static_cast<int8_t>(menu_item_id);
    buildMenuResultCodes[menu_mode][item_index] = static_cast<int8_t>(confirm_result_code);
    menuItemCode[menu_mode][item_index] = static_cast<int8_t>(buildMenuItemCode);
    menuTextIndex[menu_mode][item_index] = static_cast<int8_t>(text_sprite_code);
    menuTransitionCode[menu_mode][item_index] = static_cast<int8_t>(cursor_action_code);
    menuRowEnabled[menu_mode][item_index] = item_enabled;
    buildMenuItemCode += kBuildMenuItemCodeStep
                       + (wide_item ? kWideMenuItemCodeExtra : 0);
    ++buildMenuItemCursor;
}

void UiController::init_build_menu(Scenario& scenario, int32_t scenario_bits) {
    if (scenario_bits != build_menu::kNoScenarioOverride)
        scenario.scenarioBits = static_cast<int8_t>(scenario_bits);
    buildMenuItems.resize(build_menu::kModeCount, 0);
    buildMenuResultCodes.resize(build_menu::kModeCount, 0);
    menuItemCode.resize(build_menu::kModeCount, 0);
    menuTextIndex.resize(build_menu::kModeCount, 0);
    menuTransitionCode.resize(build_menu::kModeCount, 0);
    menuRowEnabled.resize(build_menu::kModeCount, 0);
    const bool bank_enabled =
        (scenario.scenarioBits & build_menu::scenario_feature::Bank) != 0;
    const bool graphs_enabled =
        (scenario.scenarioBits & build_menu::scenario_feature::Graphs) != 0;
    const bool tasks_enabled =
        (scenario.scenarioBits & build_menu::scenario_feature::Tasks) != 0;
    const bool route_anchor_enabled =
        (scenario.scenarioBits & build_menu::scenario_feature::RouteAnchor) != 0;
    const bool bridge_or_tunnel_enabled =
        (scenario.scenarioBits & build_menu::scenario_feature::BridgeOrTunnel) != 0;
    const bool terrain_smoothing_enabled =
        (scenario.scenarioBits & build_menu::scenario_feature::TerrainSmoothing) != 0;
    const bool terminals_enabled =
        (scenario.scenarioBits & build_menu::scenario_feature::Terminals) != 0;
    int item_count = kToolbarBaseItemCount;
    if (bank_enabled) ++item_count;
    if (graphs_enabled) ++item_count;
    if (tasks_enabled) ++item_count;
    if (scenario.scenarioStep >= 2) ++item_count;
    init_build_menu_row(build_menu::Toolbar, item_count);
    add_build_menu_item(menu_sprite::kToolbarTrainSelection,
                        build_menu::toolbar_item::SelectTrain,
                        build_menu::result_code::None, true, true,
                        build_menu::cursor_action::kNeutral);
    add_build_menu_item(menu_sprite::kRailToolsEntry,
                        build_menu::toolbar_item::RailToolsEntry,
                        build_menu::result_code::RailToolsEntry, true, false,
                        build_menu::cursor_action::kNone);
    // Roads get their own toolbar entry (next to rail) that opens the
    // RoadTools submenu. Icon is the road-crossing tile (code 44).
    add_build_menu_item(menu_sprite::kRoadToolsEntry,
                        build_menu::toolbar_item::RoadToolsEntry,
                        build_menu::result_code::None, true, false,
                        build_menu::cursor_action::kNone);
    if (bank_enabled)
        add_build_menu_item(menu_sprite::kBank, build_menu::toolbar_item::Bank,
                            build_menu::result_code::Bank, false, false,
                            build_menu::cursor_action::kNone);
    if (graphs_enabled)
        add_build_menu_item(menu_sprite::kGraphsAndStats,
                            build_menu::toolbar_item::GraphsAndStats,
                            build_menu::result_code::GraphsAndStats, false,
                            false, build_menu::cursor_action::kNone);
    if (scenario.scenarioStep >= 2)
        add_build_menu_item(menu_sprite::kCurrentMission,
                            build_menu::toolbar_item::CurrentMission,
                            build_menu::result_code::CurrentMission,
                            !tasks_enabled, false,
                            build_menu::cursor_action::kNone);
    if (tasks_enabled)
        add_build_menu_item(menu_sprite::kTasks, build_menu::toolbar_item::Tasks,
                            build_menu::result_code::Tasks, true, false,
                            build_menu::cursor_action::kNone);
    add_build_menu_item(menu_sprite::kPause, build_menu::toolbar_item::Pause,
                        build_menu::result_code::Pause, false, false,
                        build_menu::cursor_action::kNone);
    add_build_menu_item(menu_sprite::kTimeCompression,
                        build_menu::toolbar_item::TimeCompression,
                        build_menu::result_code::TimeCompression, true, false,
                        build_menu::cursor_action::kNone);
    add_build_menu_item(menu_sprite::kMainMenu, build_menu::toolbar_item::MainMenu,
                        build_menu::result_code::MainMenu, true, false,
                        build_menu::cursor_action::kNone);
    // Rail build submenu: rail tools only. Road tools live in their own submenu
    // (RoadTools, reached from the main toolbar's road-crossing button) so "what am
    // I building" stays unambiguous.
    item_count = kRailToolsBaseItemCount;
    if (route_anchor_enabled) ++item_count;
    if (bridge_or_tunnel_enabled) ++item_count;
    if (terrain_smoothing_enabled) item_count += 2;
    init_build_menu_row(build_menu::RailTools, item_count);
    if (route_anchor_enabled)
        add_build_menu_item(menu_sprite::kRouteAnchor,
                            build_menu::rail_item::RouteAnchor,
                            build_menu::result_code::RouteAnchor, true, true,
                            build_menu::cursor_action::kPlaceableTerrain);
    add_build_menu_item(menu_sprite::kRailToolsEntry,
                        build_menu::rail_item::TrackPieceMenu,
                        build_menu::result_code::RailToolsEntry, false, false,
                        build_menu::cursor_action::kNone);
    if (bridge_or_tunnel_enabled)
        add_build_menu_item(menu_sprite::kBridgeOrTunnel,
                            build_menu::rail_item::BridgeOrTunnel,
                            build_menu::result_code::BridgeOrTunnel, false, true,
                            build_menu::cursor_action::kCurve);
    add_build_menu_item(menu_sprite::kStationMenu,
                        build_menu::rail_item::StationMenu,
                        build_menu::result_code::StationMenu, true, false,
                        build_menu::cursor_action::kNone);
    add_build_menu_item(menu_sprite::kDemolish,
                        build_menu::rail_item::Demolish,
                        build_menu::result_code::Demolish, true, true,
                        build_menu::cursor_action::kCityOrStation);
    if (terrain_smoothing_enabled)
        add_build_menu_item(menu_sprite::kSmoothDown,
                            build_menu::rail_item::SmoothDown,
                            build_menu::result_code::SmoothDown, false, true,
                            build_menu::cursor_action::kNone);
    if (terrain_smoothing_enabled)
        add_build_menu_item(menu_sprite::kSmoothUp,
                            build_menu::rail_item::SmoothUp,
                            build_menu::result_code::SmoothUp, true, true,
                            build_menu::cursor_action::kNone);
    add_build_menu_item(menu_sprite::kBack, build_menu::kBackItem,
                        build_menu::result_code::Back, true, false,
                        build_menu::cursor_action::kNone);
    init_build_menu_row(build_menu::TrackBuildConfirm,
                        kTrackBuildConfirmItemCount);
    add_build_menu_item(menu_sprite::kConfirm,
                        build_menu::confirm_item::Confirm,
                        build_menu::result_code::Confirm, true, true,
                        build_menu::cursor_action::kNone);
    add_build_menu_item(menu_sprite::kCancel, build_menu::kBackItem,
                        build_menu::result_code::Cancel, true, true,
                        build_menu::cursor_action::kNone);
    item_count = kStationToolsBaseItemCount;
    if (terminals_enabled) item_count += 2;
    init_build_menu_row(build_menu::StationTools, item_count);
    add_build_menu_item(menu_sprite::kVerticalStation,
                        build_menu::station_item::VerticalStation,
                        build_menu::result_code::VerticalStation, false, true,
                        build_menu::cursor_action::kBuildTypeFirst);
    add_build_menu_item(menu_sprite::kHorizontalStation,
                        build_menu::station_item::HorizontalStation,
                        build_menu::result_code::HorizontalStation, true, true,
                        build_menu::cursor_action::kBuildTypeFirst);
    if (terminals_enabled)
        add_build_menu_item(menu_sprite::kVerticalTerminal,
                            build_menu::station_item::VerticalTerminal,
                            build_menu::result_code::VerticalTerminal, false, true,
                            build_menu::cursor_action::kVerticalTerminal);
    if (terminals_enabled)
        add_build_menu_item(menu_sprite::kHorizontalTerminal,
                            build_menu::station_item::HorizontalTerminal,
                            build_menu::result_code::HorizontalTerminal, true, true,
                            build_menu::cursor_action::kHorizontalTerminal);
    add_build_menu_item(menu_sprite::kBack, build_menu::kBackItem,
                        build_menu::result_code::Back, true, false,
                        build_menu::cursor_action::kNone);
    init_build_menu_row(build_menu::PathTarget, kPathTargetItemCount);
    add_build_menu_item(menu_sprite::kRouteAnchor,
                        build_menu::rail_item::RouteAnchor,
                        build_menu::result_code::RouteTarget, true, true,
                        build_menu::cursor_action::kPlaceableTerrain);
    add_build_menu_item(menu_sprite::kCancel, build_menu::kBackItem,
                        build_menu::result_code::Cancel, true, false,
                        build_menu::cursor_action::kNone);
    init_build_menu_row(build_menu::TrackPieceSelection,
                        kTrackPieceItemCount);
    add_build_menu_item(menu_sprite::kVerticalTrack,
                        build_menu::track_piece_item::VerticalTrack,
                        build_menu::result_code::VerticalTrack, false, true,
                        build_menu::cursor_action::kBuildTypeSecond);
    add_build_menu_item(menu_sprite::kHorizontalTrack,
                        build_menu::track_piece_item::HorizontalTrack,
                        build_menu::result_code::HorizontalTrack, true, true,
                        build_menu::cursor_action::kBuildTypeThird);
    add_build_menu_item(menu_sprite::kDiagonalTrackA,
                        build_menu::track_piece_item::DiagonalTrackA,
                        build_menu::result_code::DiagonalTrackA, false, true,
                        build_menu::cursor_action::kBuildTypeFourth);
    add_build_menu_item(menu_sprite::kDiagonalTrackB,
                        build_menu::track_piece_item::DiagonalTrackB,
                        build_menu::result_code::DiagonalTrackB, true, true,
                        build_menu::cursor_action::kBuildTypeFifth);
    add_build_menu_item(menu_sprite::kBack, build_menu::kBackItem,
                        build_menu::result_code::Back, true, false,
                        build_menu::cursor_action::kNone);
    // RoadTools submenu, reached from the main toolbar's road-crossing button:
    // lay a road tile, auto-route between two clicks, place a bus/truck stop,
    // and buy a bus/truck for the stop's road network. Item 9 returns to the
    // toolbar. Icons reuse existing toolbar sprites for now.
    init_build_menu_row(build_menu::RoadTools, kRoadToolItemCount);
    add_build_menu_item(menu_sprite::kRailToolsEntry,
                        build_menu::road_item::LayTile,
                        build_menu::result_code::None, true, true,
                        build_menu::cursor_action::kRoadBuildable);  // lay road tile (cursor tool)
    add_build_menu_item(menu_sprite::kBank, build_menu::road_item::AutoRoute,
                        build_menu::result_code::None, true, true,
                        build_menu::cursor_action::kRoadBuildable);  // auto-route road (anchor + target clicks)
    add_build_menu_item(menu_sprite::kVerticalStation,
                        build_menu::road_item::PlaceStop,
                        build_menu::result_code::None, true, true,
                        build_menu::cursor_action::kRoadStopBuildable);  // place stop beside a road
    add_build_menu_item(menu_sprite::kRouteAnchor,
                        build_menu::road_item::BuyVehicle,
                        build_menu::result_code::None, true, true,
                        build_menu::cursor_action::kRoadStop);  // buy at the selected stop
    add_build_menu_item(menu_sprite::kBack, build_menu::kBackItem,
                        build_menu::result_code::Back, true, false,
                        build_menu::cursor_action::kNone); // back to the main toolbar
}
