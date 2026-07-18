// Build-menu action dispatch.
// Routes a (menu id, option) selection to state transitions / economy actions.
// Some actions finish through the legacy response-screen tail.
#include "game.h"

namespace {

using MenuScreenId = menu_screen::Id;

namespace offer_choice_option {
constexpr int32_t Objective = 1;
constexpr int32_t Effect = 2;
}  // namespace offer_choice_option

namespace loan_option {
constexpr int32_t Borrow = 1;
constexpr int32_t Repay = 2;
}  // namespace loan_option

namespace road_vehicle_option {
constexpr int32_t Bus = 1;
constexpr int32_t Truck = 2;
}  // namespace road_vehicle_option

namespace main_menu_option {
enum Id : int32_t {
    FreeGame = 1,
    LoadGame = 2,
    SaveGame = 3,
    Settings = 4,
    Help = 5,
    About = 6,
    ContinueGame = 7,
    Missions = 8,
    HighScore = 9,
    Exit = 99,
};
}  // namespace main_menu_option

namespace save_load_option {
constexpr int32_t No = 2;
}  // namespace save_load_option

namespace train_build_option {
constexpr int32_t BuyTrain = 1;
}  // namespace train_build_option

namespace train_purchase_option {
enum Id : int32_t {
    Engine = 0,
    Wagons = 1,
    Buy = 2,
};
}  // namespace train_purchase_option

namespace wagon_selection_option {
enum Id : int32_t {
    Coal = 0,
    Oil = 1,
    Wood = 2,
    PassengerMail = 3,
    RemoveLast = 9,
};
}  // namespace wagon_selection_option

namespace offer_summary_option {
constexpr int32_t Objective = offer_choice_option::Objective;
constexpr int32_t Effect = offer_choice_option::Effect;
}  // namespace offer_summary_option

namespace high_score_option {
constexpr int32_t Enter = 40;
constexpr int32_t Download = 41;
}  // namespace high_score_option

namespace scenario_notice_option {
constexpr int32_t Description = 1;
}  // namespace scenario_notice_option

namespace scenario_selection_option {
constexpr int32_t FirstScenario = 2;
}  // namespace scenario_selection_option

namespace monthly_report_option {
enum Id : int32_t {
    CurrentPeriod = 0,
    PreviousPeriod = 1,
    YearProfit = 2,
    YearProfitAlternate = 3,
    YearRevenue = 4,
    YearRevenueAlternate = 5,
    YearCompanyValue = 6,
    YearCompanyValueAlternate = 7,
    DecadeProfit = 8,
    DecadeProfitAlternate = 9,
    DecadeRevenue = 10,
    DecadeRevenueAlternate = 11,
    DecadeCompanyValue = 12,
    DecadeCompanyValueAlternate = 13,
    CurrentDecade = 14,
    PreviousDecade = 15,
};
}  // namespace monthly_report_option

namespace monthly_report_action {
constexpr int32_t NoGraph = -1;
}  // namespace monthly_report_action

struct MonthlyReportAction {
    int32_t option;
    int32_t transitionScreen;
    int32_t graphSeries;
    bool alternateGraph;
    bool updateComparison;
    bool compareToPrevious;
    bool calculatePeriodProfit;
};

// The report menu has sixteen saved options but only three action shapes:
// period comparison, yearly/decade graph, and current/previous decade. Keeping
// the protocol in one table makes each option's result visible without changing
// the old transition or calculation order.
constexpr MonthlyReportAction kMonthlyReportActions[] = {
    {.option = monthly_report_option::CurrentPeriod,
     .transitionScreen = MenuScreenId::ReportComparison,
     .graphSeries = monthly_report_action::NoGraph,
     .alternateGraph = false,
     .updateComparison = true,
     .compareToPrevious = false,
     .calculatePeriodProfit = true},
    {.option = monthly_report_option::PreviousPeriod,
     .transitionScreen = MenuScreenId::ReportComparison,
     .graphSeries = monthly_report_action::NoGraph,
     .alternateGraph = false,
     .updateComparison = true,
     .compareToPrevious = true,
     .calculatePeriodProfit = false},
    {.option = monthly_report_option::YearProfit,
     .transitionScreen = MenuScreenId::ReportGraph,
     .graphSeries = report_series::Profit,
     .alternateGraph = false,
     .updateComparison = false,
     .compareToPrevious = false,
     .calculatePeriodProfit = false},
    {.option = monthly_report_option::YearProfitAlternate,
     .transitionScreen = MenuScreenId::AlternateReportGraph,
     .graphSeries = report_series::Profit,
     .alternateGraph = false,
     .updateComparison = false,
     .compareToPrevious = false,
     .calculatePeriodProfit = false},
    {.option = monthly_report_option::YearRevenue,
     .transitionScreen = MenuScreenId::ReportGraph,
     .graphSeries = report_series::Revenue,
     .alternateGraph = false,
     .updateComparison = false,
     .compareToPrevious = false,
     .calculatePeriodProfit = false},
    {.option = monthly_report_option::YearRevenueAlternate,
     .transitionScreen = MenuScreenId::AlternateReportGraph,
     .graphSeries = report_series::Revenue,
     .alternateGraph = false,
     .updateComparison = false,
     .compareToPrevious = false,
     .calculatePeriodProfit = false},
    {.option = monthly_report_option::YearCompanyValue,
     .transitionScreen = MenuScreenId::ReportGraph,
     .graphSeries = report_series::CompanyValue,
     .alternateGraph = false,
     .updateComparison = false,
     .compareToPrevious = false,
     .calculatePeriodProfit = false},
    {.option = monthly_report_option::YearCompanyValueAlternate,
     .transitionScreen = MenuScreenId::AlternateReportGraph,
     .graphSeries = report_series::CompanyValue,
     .alternateGraph = false,
     .updateComparison = false,
     .compareToPrevious = false,
     .calculatePeriodProfit = false},
    {.option = monthly_report_option::DecadeProfit,
     .transitionScreen = MenuScreenId::ReportGraph,
     .graphSeries = report_series::Profit,
     .alternateGraph = true,
     .updateComparison = false,
     .compareToPrevious = false,
     .calculatePeriodProfit = false},
    {.option = monthly_report_option::DecadeProfitAlternate,
     .transitionScreen = MenuScreenId::AlternateReportGraph,
     .graphSeries = report_series::Profit,
     .alternateGraph = true,
     .updateComparison = false,
     .compareToPrevious = false,
     .calculatePeriodProfit = false},
    {.option = monthly_report_option::DecadeRevenue,
     .transitionScreen = MenuScreenId::ReportGraph,
     .graphSeries = report_series::Revenue,
     .alternateGraph = true,
     .updateComparison = false,
     .compareToPrevious = false,
     .calculatePeriodProfit = false},
    {.option = monthly_report_option::DecadeRevenueAlternate,
     .transitionScreen = MenuScreenId::AlternateReportGraph,
     .graphSeries = report_series::Revenue,
     .alternateGraph = true,
     .updateComparison = false,
     .compareToPrevious = false,
     .calculatePeriodProfit = false},
    {.option = monthly_report_option::DecadeCompanyValue,
     .transitionScreen = MenuScreenId::ReportGraph,
     .graphSeries = report_series::CompanyValue,
     .alternateGraph = true,
     .updateComparison = false,
     .compareToPrevious = false,
     .calculatePeriodProfit = false},
    {.option = monthly_report_option::DecadeCompanyValueAlternate,
     .transitionScreen = MenuScreenId::AlternateReportGraph,
     .graphSeries = report_series::CompanyValue,
     .alternateGraph = true,
     .updateComparison = false,
     .compareToPrevious = false,
     .calculatePeriodProfit = false},
    {.option = monthly_report_option::CurrentDecade,
     .transitionScreen = MenuScreenId::AnnualReport,
     .graphSeries = monthly_report_action::NoGraph,
     .alternateGraph = false,
     .updateComparison = true,
     .compareToPrevious = false,
     .calculatePeriodProfit = true},
    {.option = monthly_report_option::PreviousDecade,
     .transitionScreen = MenuScreenId::AnnualReport,
     .graphSeries = monthly_report_action::NoGraph,
     .alternateGraph = false,
     .updateComparison = true,
     .compareToPrevious = true,
     .calculatePeriodProfit = false},
};

namespace settings_option {
enum Id : int32_t {
    Animations = 1,
    Selection = 2,
    MonthlyReport = 3,
    AnnualReport = 4,
    Music = 5,
    Volume = 6,
};
}  // namespace settings_option

namespace scenario_settings_option {
enum Id : int32_t {
    MapSize = 1,
    FinancialDifficulty = 2,
    AiDifficulty = 3,
    TownLevel = 4,
    IndustryLevel = 5,
    DifficultyCycle = 6,
    StartScenario = 10,
};
}  // namespace scenario_settings_option

void cycle_free_game_level(int8_t& level, int8_t after_last) {
    level = static_cast<int8_t>(level + 1);
    if (level == after_last) level = 0;
}

namespace train_upgrade_option {
constexpr int32_t Discount = 1;
constexpr int32_t Upgrade = 2;
constexpr int32_t Cancel = 3;
constexpr int32_t ToggleAi = 4;
}  // namespace train_upgrade_option

namespace toggle_value {
constexpr int32_t Off = 0;
constexpr int32_t On = 1;
}  // namespace toggle_value

namespace scenario_difficulty_cycle {
constexpr int8_t First = 1;
constexpr int8_t AfterLast = 4;
}  // namespace scenario_difficulty_cycle

namespace high_score_entry_option {
enum Id : int32_t {
    Close = -1,
    PreviousSlot = 0,
    NextSlot = 1,
    PreviousGlyph = 2,
    NextGlyph = 3,
};
}  // namespace high_score_entry_option

namespace player_name_glyph {
constexpr int8_t FirstCode = 0;
constexpr int8_t LastPunctuationCode = 17;
constexpr int8_t FirstLowercaseCode = 40;
constexpr int8_t LastLowercaseCode = 65;
constexpr int8_t FirstUppercaseCode = 70;
constexpr int8_t LastCode = 95;
constexpr int8_t AfterLastCode = 96;
}  // namespace player_name_glyph

namespace value {
constexpr int32_t kLoanAmount = 20000;
constexpr int32_t kRefundPercent = 70;
constexpr int32_t kMusicVolumeFirst = 1;
constexpr int32_t kMusicVolumeAfterLast = 6;
constexpr int32_t kPlayerNameSlotCount = 10;
constexpr int32_t kPlayerNameLastSlot = kPlayerNameSlotCount - 1;
}  // namespace value

namespace menu_dispatch_tail {
// The original common tail forced unnamed response screens 81..99 back to
// Playing. Keep the exact half-open range while giving it a name.
constexpr int32_t kFirstResponseScreen = 81;
constexpr int32_t kAfterLastResponseScreen = 100;
}  // namespace menu_dispatch_tail

}  // namespace

void Game::handle_build_menu_state(int32_t menu_screen, int32_t menu_option) {
    Game *game = this;
    const auto apply_menu_dispatch_tail = [&](int32_t screen) {
        if (screen >= menu_dispatch_tail::kFirstResponseScreen
            && screen < menu_dispatch_tail::kAfterLastResponseScreen)
            game->set_state(GameState::Playing);
    };
    switch (menu_screen) {
        case MenuScreenId::MainMenu:
            switch (menu_option) {
                case main_menu_option::FreeGame:
                    if (game->ready) { game->ui.transitionCode = MenuScreenId::ScenarioSettings; game->reset_overlay_render_state(); break; }
                    game->ui.pendingTransitionCode = MenuScreenId::ScenarioSettings; game->set_state(GameState::Playing); break;
                case main_menu_option::LoadGame:
                    game->ui.screenParam = save_load_mode::LoadGame;
                    if (game->ready) { game->ui.transitionCode = MenuScreenId::SaveLoadDialog; game->reset_overlay_render_state(); break; }
                    game->ui.pendingTransitionCode = MenuScreenId::SaveLoadDialog; game->set_state(GameState::Playing); break;
                case main_menu_option::SaveGame:
                    game->ui.screenParam = save_load_mode::SaveGame;
                    if (game->ready) { game->ui.transitionCode = MenuScreenId::SaveLoadDialog; game->reset_overlay_render_state(); break; }
                    game->ui.pendingTransitionCode = MenuScreenId::SaveLoadDialog; game->set_state(GameState::Playing); break;
                case main_menu_option::Settings:
                    if (game->ready) { game->ui.transitionCode = MenuScreenId::Settings; game->reset_overlay_render_state(); break; }
                    game->ui.pendingTransitionCode = MenuScreenId::Settings; game->set_state(GameState::Playing); break;
                case main_menu_option::Help:
                    if (game->ready) { game->ui.transitionCode = MenuScreenId::GenericOverlayA; game->reset_overlay_render_state(); break; }
                    game->ui.pendingTransitionCode = MenuScreenId::GenericOverlayA; game->set_state(GameState::Playing); break;
                case main_menu_option::About:
                    if (game->ready) { game->ui.transitionCode = MenuScreenId::GenericOverlayB; game->reset_overlay_render_state(); break; }
                    game->ui.pendingTransitionCode = MenuScreenId::GenericOverlayB; game->set_state(GameState::Playing); break;
                case main_menu_option::ContinueGame:
                    game->set_state(GameState::Playing); break;
                case main_menu_option::Missions:
                    if (game->ready) { game->ui.transitionCode = MenuScreenId::ScenarioSelection; game->reset_overlay_render_state(); break; }
                    game->ui.pendingTransitionCode = MenuScreenId::ScenarioSelection; game->set_state(GameState::Playing); break;
                case main_menu_option::HighScore:
                    if (game->ready) { game->ui.transitionCode = MenuScreenId::HighScoreMenu; game->reset_overlay_render_state(); break; }
                    game->ui.pendingTransitionCode = MenuScreenId::HighScoreMenu; game->set_state(GameState::Playing); break;
                case main_menu_option::Exit:
                    game->ui.screenParam = save_load_mode::ExitGame;
                    if (game->ready) { game->ui.transitionCode = MenuScreenId::SaveLoadDialog; game->reset_overlay_render_state(); break; }
                    game->ui.pendingTransitionCode = MenuScreenId::SaveLoadDialog; game->set_state(GameState::Playing); break;
            }
            break;
        case MenuScreenId::SaveLoadDialog:
            if (menu_option == save_load_option::No) {
                if (game->ui.screenParam == save_load_mode::CancelTrainPurchase) { game->ui.pendingTransitionCode = MenuScreenId::TrainUpgrade; game->set_state(GameState::Playing); break; }
                if (game->ui.screenParam == save_load_mode::LoadResources) { game->set_state(GameState::LoadingResources); break; }
                if (game->ready) { game->ui.transitionCode = MenuScreenId::MainMenu; game->reset_overlay_render_state(); break; }
                game->ui.pendingTransitionCode = MenuScreenId::MainMenu; game->set_state(GameState::Playing); break;
            }
            switch (game->ui.screenParam) {
                case save_load_mode::SaveGame: game->set_state(GameState::SavingGame); break;
                case save_load_mode::LoadGame: game->set_state(GameState::LoadingGame); break;
                case save_load_mode::ExitGame: game->stopRequested = true; break;
                case save_load_mode::LoadResources: game->music.autoStart = true; game->music.enabled = true; game->set_state(GameState::LoadingResources); break;
                case save_load_mode::CancelTrainPurchase: {
                    game->economy.money += game->build.currentTrainCostSnapshot * value::kRefundPercent / percentage::kBase;
                    game->economy.aiMoneySpent -= game->build.currentTrainCostSnapshot * value::kRefundPercent / percentage::kBase;
                    if (game->build.currentTrainCostSnapshot != 0)
                        game->record_money_change(
                            game->build.currentTrainCostSnapshot * value::kRefundPercent / percentage::kBase);
                    int16_t next_car = game->fleet.trainCars[game->trainAtCursor].nextCar;
                    game->decrement_train_cargo_option_counts(next_car);
                    game->fleet.trainCars[game->trainAtCursor].flags = 0;
                    while (next_car != train_chain::kEnd) {
                        game->fleet.trainCars[next_car].flags = 0;
                        next_car = game->fleet.trainCars[next_car].nextCar;
                    }
                    game->set_state(GameState::Playing);
                    break;
                }
            }
            break;
        case MenuScreenId::MapOverview:
            game->set_state(GameState::MapOverview); break;
        case MenuScreenId::ScenarioNotice:
            if (menu_option == scenario_notice_option::Description)
                game->ui.pendingTransitionCode = transition::SCENARIO_NOTICE_BASE + game->scenario.scenarioNoticeId;
            game->set_state(GameState::Playing); break;
        case MenuScreenId::GenericOverlayA:
        case MenuScreenId::GenericOverlayB:
            if (game->ready) { game->ui.transitionCode = MenuScreenId::MainMenu; game->reset_overlay_render_state(); break; }
            game->ui.pendingTransitionCode = MenuScreenId::MainMenu; game->set_state(GameState::Playing); break;
        case MenuScreenId::ContinueGame:
            game->set_state(GameState::Playing); break;
        case MenuScreenId::AiScenarioSetup:
            game->ready = false; game->scenario.aiEnabled = true; game->ui.transitionCode = MenuScreenId::MainMenu;
            game->reset_overlay_render_state(); game->set_state(GameState::MenuOverlay); break;
        case MenuScreenId::ScenarioSelection:
            if (menu_option >= scenario_selection_option::FirstScenario) { game->scenario.activeScenario = menu_option; game->set_state(GameState::GeneratingMap); break; }
            if (game->ready) { game->ui.transitionCode = MenuScreenId::MainMenu; game->reset_overlay_render_state(); break; }
            game->ui.pendingTransitionCode = MenuScreenId::MainMenu; game->set_state(GameState::Playing); break;
        case MenuScreenId::ScenarioSettings:
            switch (menu_option) {
                case scenario_settings_option::MapSize:
                    cycle_free_game_level(game->scenario.freeGameMapSize,
                                          free_game_map_size::AfterLast);
                    apply_menu_dispatch_tail(menu_screen); return;
                case scenario_settings_option::FinancialDifficulty:
                    if (game->scenario.financialDifficulty == toggle_value::Off) { game->scenario.financialDifficulty = toggle_value::On; apply_menu_dispatch_tail(menu_screen); return; }
                    game->scenario.financialDifficulty = toggle_value::Off; apply_menu_dispatch_tail(menu_screen); return;
                case scenario_settings_option::AiDifficulty:
                    if (game->scenario.aiDifficulty == toggle_value::Off) { game->scenario.aiDifficulty = toggle_value::On; apply_menu_dispatch_tail(menu_screen); return; }
                    game->scenario.aiDifficulty = toggle_value::Off; apply_menu_dispatch_tail(menu_screen); return;
                case scenario_settings_option::TownLevel:
                    cycle_free_game_level(game->scenario.freeGameTownLevel,
                                          free_game_town_level::AfterLast);
                    apply_menu_dispatch_tail(menu_screen); return;
                case scenario_settings_option::IndustryLevel:
                    cycle_free_game_level(game->scenario.freeGameIndustryLevel,
                                          free_game_industry_level::AfterLast);
                    apply_menu_dispatch_tail(menu_screen); return;
                case scenario_settings_option::DifficultyCycle:
                    game->scenario.difficultyCycle = (int8_t)(game->scenario.difficultyCycle + 1);
                    if (game->scenario.difficultyCycle != scenario_difficulty_cycle::AfterLast) { apply_menu_dispatch_tail(menu_screen); return; }
                    game->scenario.difficultyCycle = scenario_difficulty_cycle::First; apply_menu_dispatch_tail(menu_screen); return;
                case scenario_settings_option::StartScenario:
                    game->scenario.activeScenario =
                        game->scenario.freeGameMapSize == free_game_map_size::Small
                            ? 0 : 1;
                    game->set_state(GameState::GeneratingMap); apply_menu_dispatch_tail(menu_screen); return;
            }
            if (game->ready) { game->ui.transitionCode = MenuScreenId::MainMenu; game->reset_overlay_render_state(); break; }
            game->ui.pendingTransitionCode = MenuScreenId::MainMenu; game->set_state(GameState::Playing); break;
        case MenuScreenId::GenericOverlayC:
            game->set_state(GameState::Playing); break;
        case MenuScreenId::GenericOverlayD:
            game->set_state(GameState::Playing); break;
        case MenuScreenId::TrainBuildReset:
            if (menu_option == train_build_option::BuyTrain) {
                game->build.wagonType = train_era::First;
                game->build.currentTrainCostSnapshot = 0;
                game->build.wagonDiscountType = game->build.wagonType;
                for (int option_index = 0; option_index < kWagonOptionCount; ++option_index)
                    game->build.wagonOptionSlots[option_index] = wagon::NoOption;
                for (int option_index = 0; option_index < kWagonOptionCount; ++option_index)
                    game->build.matchingWagonOptions[option_index] = wagon::NoOption;
                game->scenario.difficultyPct = percentage::kBase;
                game->build.discountApplied = false;
                game->build.compute_train_cost(game->economy, game->scenario);
                game->ui.pendingTransitionCode = MenuScreenId::TrainPurchase; game->set_state(GameState::Playing); apply_menu_dispatch_tail(menu_screen); return;
            }
            game->set_state(GameState::Playing); break;
        case MenuScreenId::TrainPurchase:
            if (menu_option == train_purchase_option::Engine) { game->ui.pendingTransitionCode = MenuScreenId::TrainTypeSelection; game->set_state(GameState::Playing); break; }
            if (menu_option == train_purchase_option::Wagons) { game->ui.pendingTransitionCode = MenuScreenId::WagonOptionSelection; game->set_state(GameState::Playing); break; }
            if (menu_option == train_purchase_option::Buy) {
                if (!game->build.discountApplied) {
                    if (!game->buy_train_at_cursor()) break;
                    game->set_state(GameState::Playing); break;
                }
                if (!game->apply_train_upgrade(game->trainAtCursor, game->build.wagonType, game->build.wagonOptionSlots)) break;
                game->ui.pendingTransitionCode = MenuScreenId::TrainUpgrade; game->set_state(GameState::Playing); break;
            }
            game->ui.pendingTransitionCode = !game->build.discountApplied ? MenuScreenId::TrainBuildReset : MenuScreenId::TrainUpgrade;
            game->set_state(GameState::Playing); break;
        case MenuScreenId::TrainTypeSelection:
            if (menu_option >= train_era::First && menu_option <= train_era::Sixth) {
                game->build.wagonType = menu_option;
                game->build.compute_train_cost(game->economy, game->scenario);
            }
            game->ui.pendingTransitionCode = MenuScreenId::TrainPurchase; game->set_state(GameState::Playing); break;
        case MenuScreenId::WagonOptionSelection:
            if (menu_option == wagon_selection_option::PassengerMail) game->add_selected_option(wagon::PassengerMail);
            else if (menu_option == wagon_selection_option::Oil) game->add_selected_option(wagon::IndustryGroup1);
            else if (menu_option == wagon_selection_option::Coal) game->add_selected_option(wagon::IndustryGroup2);
            else if (menu_option == wagon_selection_option::Wood) game->add_selected_option(wagon::IndustryGroup3);
            else if (menu_option == wagon_selection_option::RemoveLast) game->pop_selected_option();
            game->build.compute_train_cost(game->economy, game->scenario);
            game->ui.pendingTransitionCode = MenuScreenId::TrainPurchase; game->set_state(GameState::Playing); break;
        case MenuScreenId::TrainUpgrade:
            if (menu_option == train_upgrade_option::Discount) {
                game->build.discountApplied = true;
                game->ui.pendingTransitionCode = MenuScreenId::TrainPurchase; game->set_state(GameState::Playing);
            } else if (menu_option == train_upgrade_option::Upgrade) {
                int upgrade_cost = game->build.baseTrainCostSnapshot - game->build.currentTrainCostSnapshot;
                if (upgrade_cost <= game->economy.money) {
                    game->fleet.trainCars[game->trainAtCursor].ageYears = 0;
                    game->economy.money -= upgrade_cost;
                    game->record_money_change(-upgrade_cost);
                    game->economy.aiMoneySpent += upgrade_cost;
                    game->scenario.difficultyPct = percentage::kBase - 3 * game->fleet.trainCars[game->trainAtCursor].ageYears;
                    game->build.compute_train_cost(game->economy, game->scenario);
                    game->build.currentTrainCostSnapshot = game->build.unitCost;
                    game->ui.pendingTransitionCode = MenuScreenId::TrainUpgrade; game->set_state(GameState::Playing);
                } else {
                    game->enter_out_of_money_state();
                }
            } else if (menu_option == train_upgrade_option::Cancel) {
                game->ui.screenParam = save_load_mode::CancelTrainPurchase;
                game->ui.pendingTransitionCode = MenuScreenId::SaveLoadDialog; game->set_state(GameState::Playing);
            }
            if (menu_option == train_upgrade_option::ToggleAi) {
                game->fleet.trainCars[game->trainAtCursor].aiOverride =
                    (int8_t)(game->fleet.trainCars[game->trainAtCursor].aiOverride == toggle_value::Off ? toggle_value::On : toggle_value::Off);
                break;
            }
            game->set_state(GameState::Playing); break;
        case MenuScreenId::Settings:
            switch (menu_option) {
                case settings_option::Animations: game->settings.animationsDisabled = !game->settings.animationsDisabled; apply_menu_dispatch_tail(menu_screen); return;
                case settings_option::Selection: game->settings.selectionHidden = !game->settings.selectionHidden; apply_menu_dispatch_tail(menu_screen); return;
                case settings_option::MonthlyReport: game->ui.monthlyReportEnabled = !game->ui.monthlyReportEnabled; apply_menu_dispatch_tail(menu_screen); return;
                case settings_option::AnnualReport: game->ui.annualReportEnabled = !game->ui.annualReportEnabled; apply_menu_dispatch_tail(menu_screen); return;
                case settings_option::Music:
                    game->music.enabled = !game->music.enabled;
                    if (game->music.enabled) { game->play_music(); apply_menu_dispatch_tail(menu_screen); return; }
                    game->stop_music(); apply_menu_dispatch_tail(menu_screen); return;
                case settings_option::Volume:
                    ++game->music.volumeLevel;
                    if (game->music.volumeLevel == value::kMusicVolumeAfterLast) game->music.volumeLevel = value::kMusicVolumeFirst;
                    game->set_music_volume(game->music.volumeLevel); apply_menu_dispatch_tail(menu_screen); return;
            }
            game->persist_settings_snapshot();
            if (game->ready) { game->ui.transitionCode = MenuScreenId::MainMenu; game->reset_overlay_render_state(); break; }
            game->ui.pendingTransitionCode = MenuScreenId::MainMenu; game->set_state(GameState::Playing); break;
        case MenuScreenId::GenericOverlayE:
        case MenuScreenId::GenericOverlayF:
            if (game->ready) { game->ui.transitionCode = MenuScreenId::MainMenu; game->reset_overlay_render_state(); break; }
            game->ui.pendingTransitionCode = MenuScreenId::MainMenu; game->set_state(GameState::Playing); break;
        case MenuScreenId::OfferTypeChoice:
        case MenuScreenId::OfferEffectChoice:
            if (menu_option == offer_choice_option::Objective) {
                game->ui.pendingTransitionCode = game->scenario.offerTypeTransitionCode;
                game->set_state(GameState::Playing);
                break;
            }
            if (menu_option == offer_choice_option::Effect) {
                game->ui.pendingTransitionCode = game->scenario.offerEffectTransitionCode;
                game->set_state(GameState::Playing);
                break;
            }
            game->set_state(GameState::Playing); break;
        case MenuScreenId::GenericOverlayG: case MenuScreenId::GenericOverlayH: case MenuScreenId::GenericOverlayI: case MenuScreenId::GenericOverlayJ:
            game->set_state(GameState::Playing); break;
        case MenuScreenId::ResourceLoading:
            if (menu_option == toggle_value::On) game->music.autoStart = true;
            game->set_state(GameState::LoadingResources); break;
        case MenuScreenId::LoanDialog:
            switch (menu_option) {
                case loan_option::Borrow:
                    if (game->economy.loanCeiling - value::kLoanAmount < game->economy.loanBalance) break;
                    game->economy.loanBalance += value::kLoanAmount; game->economy.money += value::kLoanAmount;
                    game->offerBoard.add_progress(offer_objective::Objective14,
                                                  -value::kLoanAmount);
                    break;
                case loan_option::Repay:
                    if (game->economy.loanBalance < value::kLoanAmount || game->economy.money < value::kLoanAmount) break;
                    game->economy.loanBalance -= value::kLoanAmount; game->economy.money -= value::kLoanAmount;
                    game->offerBoard.add_progress(offer_objective::Objective14,
                                                  value::kLoanAmount);
                    break;
                default:
                    game->set_state(GameState::Playing); game->ui.menuDirty = true;
            }
            game->renderer.mapRedrawPending = true;
            break;
        case MenuScreenId::RoadVehicleDialog:
            // Road-stop buy dialog. The exact stop was captured when the dialog
            // opened (build.roadStopCursor); buy_road_vehicle starts there and
            // serves its connected network. Action 1 = bus (passengers,
            // carType 2), action 2 = truck (freight, carType 4). Any other input
            // (including the close X, menu_option = -1) just dismisses the dialog.
            switch (menu_option) {
                case road_vehicle_option::Bus:
                    game->buy_road_vehicle(road_vehicle_type::Bus,
                                           game->build.roadStopCursor);
                    break;
                case road_vehicle_option::Truck:
                    game->buy_road_vehicle(road_vehicle_type::Truck,
                                           game->build.roadStopCursor);
                    break;
            }
            game->set_state(GameState::Playing);
            game->ui.menuDirty = true;
            game->renderer.mapRedrawPending = true;
            break;
        case MenuScreenId::MonthlyReport: {
            game->monthRolloverFlag = false;
            bool monthly_report_action_applied = false;
            for (const MonthlyReportAction& action : kMonthlyReportActions) {
                if (action.option != menu_option) continue;
                if (action.updateComparison)
                    game->compareToPrevious = action.compareToPrevious;
                if (action.calculatePeriodProfit)
                    game->economy.calculate_period_profit();
                if (action.graphSeries != monthly_report_action::NoGraph)
                    game->ui.set_year_graph_mode(game->economy,
                                                 action.graphSeries,
                                                 action.alternateGraph);
                game->ui.pendingTransitionCode = action.transitionScreen;
                game->set_state(GameState::Playing);
                monthly_report_action_applied = true;
                break;
            }
            if (monthly_report_action_applied) {
                apply_menu_dispatch_tail(menu_screen);
                return;
            }
            game->set_state(GameState::Playing); game->ui.menuDirty = true;
            break;
        }
        case MenuScreenId::ReportComparison:
        case MenuScreenId::AnnualReport:
            if (!game->monthRolloverFlag) { game->ui.pendingTransitionCode = MenuScreenId::MonthlyReport; game->set_state(GameState::Playing); break; }
            game->set_state(GameState::Playing); game->ui.menuDirty = true; break;
        case MenuScreenId::ReportGraph:
        case MenuScreenId::AlternateReportGraph:
            game->ui.pendingTransitionCode = MenuScreenId::MonthlyReport; game->set_state(GameState::Playing); break;
        case MenuScreenId::OfferSummary:
            switch (menu_option) {
                case offer_summary_option::Objective:
                    game->ui.pendingTransitionCode = game->offerBoard.offers[game->scenario.offerRowIndex].objectiveType + transition::OFFER_TYPE_BASE;
                    game->scenario.offerFlowState = offer_flow::Summary; break;
                case offer_summary_option::Effect:
                    game->ui.pendingTransitionCode = game->offerBoard.offers[game->scenario.offerRowIndex].effectType + transition::OFFER_ALT_BASE;
                    game->scenario.offerFlowState = offer_flow::Summary; break;
            }
            game->set_state(GameState::Playing); break;
        case MenuScreenId::OfferResponse41: case MenuScreenId::OfferResponse42: case MenuScreenId::OfferResponse44: case MenuScreenId::OfferResponse45: case MenuScreenId::OfferResponse46: case MenuScreenId::OfferResponse47: case MenuScreenId::OfferResponse48:
        case MenuScreenId::OfferResponse49: case MenuScreenId::OfferResponse50: case MenuScreenId::OfferResponse51: case MenuScreenId::OfferResponse52: case MenuScreenId::OfferResponse53: case MenuScreenId::OfferResponse54:
            if (game->scenario.offerFlowState == offer_flow::Selection) game->ui.pendingTransitionCode = MenuScreenId::OfferSelection;
            if (game->scenario.offerFlowState == offer_flow::Summary) game->ui.pendingTransitionCode = MenuScreenId::OfferSummary;
            if (game->scenario.offerFlowState == offer_flow::EffectChoice) game->ui.pendingTransitionCode = MenuScreenId::OfferEffectChoice;
            if (game->scenario.offerFlowState == offer_flow::ObjectiveChoice) game->ui.pendingTransitionCode = MenuScreenId::OfferTypeChoice;
            game->set_state(GameState::Playing); break;
        case MenuScreenId::OfferSelection:
            if (menu_option % 2 == 1) {
                game->scenario.offerRowIndex = game->ui.enabledMenuRowIndexes[menu_option / 2];
                game->ui.pendingTransitionCode = game->offerBoard.offers[game->scenario.offerRowIndex].objectiveType + transition::OFFER_TYPE_BASE;
            }
            if (menu_option % 2 == 0) {
                game->scenario.offerRowIndex = game->ui.enabledMenuRowIndexes[(menu_option - 1) / 2];
                game->ui.pendingTransitionCode = game->offerBoard.offers[game->scenario.offerRowIndex].effectType + transition::OFFER_ALT_BASE;
            }
            game->scenario.offerFlowState = offer_flow::Selection;
            game->set_state(GameState::Playing); break;
        case MenuScreenId::OfferResponse61: case MenuScreenId::OfferResponse62: case MenuScreenId::OfferResponse63: case MenuScreenId::OfferResponse64: case MenuScreenId::OfferResponse65: case MenuScreenId::OfferResponse66:
        case MenuScreenId::OfferResponse67: case MenuScreenId::OfferResponse68: case MenuScreenId::OfferResponse69: case MenuScreenId::OfferResponse70: case MenuScreenId::OfferResponse71: case MenuScreenId::OfferResponse72:
            if (game->scenario.offerFlowState == offer_flow::Selection) game->ui.pendingTransitionCode = MenuScreenId::OfferSelection;
            if (game->scenario.offerFlowState == offer_flow::Summary) game->ui.pendingTransitionCode = MenuScreenId::OfferSummary;
            if (game->scenario.offerFlowState == offer_flow::EffectChoice) game->ui.pendingTransitionCode = MenuScreenId::OfferEffectChoice;
            if (game->scenario.offerFlowState == offer_flow::ObjectiveChoice) game->ui.pendingTransitionCode = MenuScreenId::OfferTypeChoice;
            game->set_state(GameState::Playing); break;
        case MenuScreenId::HighScoreMenu:
            if (menu_option == high_score_option::Download) {
                game->scenario.highScoreUploadRequested = false;
                game->set_state(GameState::HighScoreSync);
                break;
            }
            if (game->ready) {
                game->ui.transitionCode = menu_option == high_score_option::Enter
                    ? MenuScreenId::HighScoreEntry : MenuScreenId::MainMenu;
                game->reset_overlay_render_state();
                break;
            }
            game->ui.pendingTransitionCode = menu_option == high_score_option::Enter
                ? MenuScreenId::HighScoreEntry : MenuScreenId::MainMenu;
            game->set_state(GameState::Playing);
            break;
        case MenuScreenId::HighScoreEntry:
            if (menu_option == high_score_option::Enter) {
                game->scenario.highScoreUploadRequested = true;
                game->set_state(GameState::HighScoreSync);
                break;
            }
            if (menu_option == high_score_entry_option::PreviousSlot)
                game->scenario.playerNameCursor = (int8_t)(game->scenario.playerNameCursor - 1);
            if (menu_option == high_score_entry_option::NextSlot)
                game->scenario.playerNameCursor = (int8_t)(game->scenario.playerNameCursor + 1);
            if (game->scenario.playerNameCursor < 0) game->scenario.playerNameCursor = value::kPlayerNameLastSlot;
            if (game->scenario.playerNameCursor == value::kPlayerNameSlotCount) game->scenario.playerNameCursor = 0;
            game->assets.spriteCycleCursor = game->scenario.playerNameCodes[game->scenario.playerNameCursor];
            if (menu_option == high_score_entry_option::PreviousGlyph) {
                game->assets.spriteCycleCursor = (int8_t)(game->assets.spriteCycleCursor - 1);
                if (game->assets.spriteCycleCursor < player_name_glyph::FirstCode)
                    game->assets.spriteCycleCursor = player_name_glyph::LastCode;
                if (game->assets.spriteCycleCursor == player_name_glyph::FirstUppercaseCode - 1)
                    game->assets.spriteCycleCursor = player_name_glyph::LastLowercaseCode;
                if (game->assets.spriteCycleCursor == player_name_glyph::FirstLowercaseCode - 1)
                    game->assets.spriteCycleCursor = player_name_glyph::LastPunctuationCode;
                game->scenario.playerNameCodes[game->scenario.playerNameCursor] = game->assets.spriteCycleCursor;
            }
            if (menu_option == high_score_entry_option::NextGlyph) {
                game->assets.spriteCycleCursor = (int8_t)(game->assets.spriteCycleCursor + 1);
                if (game->assets.spriteCycleCursor == player_name_glyph::LastPunctuationCode + 1)
                    game->assets.spriteCycleCursor = player_name_glyph::FirstLowercaseCode;
                if (game->assets.spriteCycleCursor == player_name_glyph::LastLowercaseCode + 1)
                    game->assets.spriteCycleCursor = player_name_glyph::FirstUppercaseCode;
                if (game->assets.spriteCycleCursor == player_name_glyph::AfterLastCode)
                    game->assets.spriteCycleCursor = player_name_glyph::FirstCode;
                game->scenario.playerNameCodes[game->scenario.playerNameCursor] = game->assets.spriteCycleCursor;
            }
            if (menu_option != high_score_entry_option::Close) break;
            game->persist_settings_snapshot();
            if (game->ready) { game->ui.transitionCode = MenuScreenId::HighScoreMenu; game->reset_overlay_render_state(); break; }
            game->ui.pendingTransitionCode = MenuScreenId::HighScoreMenu; game->set_state(GameState::Playing); break;
    }
}
