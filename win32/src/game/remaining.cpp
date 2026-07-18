// Score, graph, text, and life-cycle functions.
#include "game.h"

namespace {

namespace graph_scale {
constexpr int32_t kLegacyYearBase = 1940;
constexpr int32_t kMonthsPerYear = 12;
constexpr int32_t kProfitHeight = 30;
constexpr int32_t kRevenueHeight = 60;
}  // namespace graph_scale

}  // namespace

void UiController::set_year_graph_mode(const Economy& economy, int32_t mode, bool alternate) {
    graphSeries = mode;
    graphUsesDecadeData = alternate;
    int minimum_graph_value = 0;
    int maximum_graph_value = 0;
    for (int index = 0; index < kHistoryCount; ++index) {
        int graph_value;
        if (!alternate) {
            graphYearLabels[index] =
                (int8_t)(economy.year - graph_scale::kLegacyYearBase);
            graphPeriodLabels[index] = (int8_t)(economy.monthOfYear - index);
            if (graphPeriodLabels[index] < 1) {
                graphPeriodLabels[index] =
                    (int8_t)(graph_scale::kMonthsPerYear
                             + graphPeriodLabels[index]);
                --graphYearLabels[index];
            }
            graph_value = mode == report_series::Profit ? economy.yearProfitHistory[index]
                    : mode == report_series::Revenue ? economy.yearRevenueHistory[index]
                    : economy.yearCompanyValueHistory[index];
        } else {
            graphPeriodLabels[index] =
                (int8_t)(economy.year - index - graph_scale::kLegacyYearBase);
            graph_value = mode == report_series::Profit ? economy.decadeProfitHistory[index]
                    : mode == report_series::Revenue ? economy.altDecadeRevenueTotal[index]
                    : economy.altDecadeTotalCost[index];
        }
        if (graph_value > maximum_graph_value)
            maximum_graph_value = graph_value;
        if (graph_value < minimum_graph_value)
            minimum_graph_value = graph_value;
    }
    int value_scale = maximum_graph_value > -minimum_graph_value
        ? maximum_graph_value : -minimum_graph_value;
    for (int index = 0; index < kHistoryCount; ++index) {
        if (!alternate && mode == report_series::Profit) {
            graphBarHeights[index] =
                economy.yearProfitHistory[index] == 0 ? 0
                : (int8_t)(graph_scale::kProfitHeight
                           * economy.yearProfitHistory[index] / value_scale);
        } else if (!alternate && mode == report_series::Revenue) {
            graphBarHeights[index] =
                economy.yearRevenueHistory[index] == 0 ? 0
                : (int8_t)(graph_scale::kRevenueHeight
                           * economy.yearRevenueHistory[index] / value_scale);
        } else if (!alternate && mode == report_series::CompanyValue) {
            // The original tests profit for zero before scaling company value.
            graphBarHeights[index] =
                economy.yearProfitHistory[index] == 0 ? 0
                : (int8_t)(graph_scale::kProfitHeight
                           * economy.yearCompanyValueHistory[index] / value_scale);
        } else if (alternate && mode == report_series::Profit) {
            graphBarHeights[index] =
                economy.decadeProfitHistory[index] == 0 ? 0
                : (int8_t)(graph_scale::kProfitHeight
                           * economy.decadeProfitHistory[index] / value_scale);
        } else if (alternate && mode == report_series::Revenue) {
            graphBarHeights[index] =
                economy.altDecadeRevenueTotal[index] == 0 ? 0
                : (int8_t)(graph_scale::kRevenueHeight
                           * economy.altDecadeRevenueTotal[index] / value_scale);
        } else if (alternate && mode == report_series::CompanyValue) {
            // The original tests the other-total array before scaling cost.
            graphBarHeights[index] =
                economy.decadeProfitHistory[index] == 0 ? 0
                : (int8_t)(graph_scale::kProfitHeight
                           * economy.altDecadeTotalCost[index] / value_scale);
        }
    }
}

int32_t Scenario::milestone_progress(const Economy& economy) const {
    int target = currentMilestoneScore;
    int current = 0;
    if (scenarioMilestoneIndex >= scenario_milestone::CombinedThreshold1And2
        && scenarioMilestoneIndex <= scenario_milestone::CombinedThreshold4And5)
        target += milestoneCoThreshold2;
    switch (scenarioMilestoneIndex) {
        case scenario_milestone::Threshold1: current = milestoneThreshold1; break;
        case scenario_milestone::Threshold2: current = milestoneThreshold2; break;
        case scenario_milestone::Threshold3: current = milestoneThreshold3; break;
        case scenario_milestone::Threshold4: current = milestoneThreshold4; break;
        case scenario_milestone::Threshold5: current = milestoneThreshold5; break;
        case scenario_milestone::CombinedThreshold1And2:
            current = milestoneThreshold1 + milestoneThreshold2;
            break;
        case scenario_milestone::CombinedThreshold1And3:
            current = milestoneThreshold1 + milestoneThreshold3;
            break;
        case scenario_milestone::CombinedThreshold1And4:
            current = milestoneThreshold1 + milestoneThreshold4;
            break;
        case scenario_milestone::CombinedThreshold2And3:
            current = milestoneThreshold2 + milestoneThreshold3;
            break;
        case scenario_milestone::CombinedThreshold2And4:
            current = milestoneThreshold2 + milestoneThreshold4;
            break;
        case scenario_milestone::CombinedThreshold4And5:
            current = milestoneThreshold4 + milestoneThreshold5;
            break;
        case scenario_milestone::LifetimeRevenue: current = economy.lifetimeTrainRevenue; break;
        case scenario_milestone::LoanCleared:
            current = economy.loanCeiling - economy.loanBalance;
            target = economy.loanCeiling;
            break;
        case scenario_milestone::TargetYear:
            current = economy.year - game_calendar::kBaseYear;
            target = currentMilestoneScore - game_calendar::kBaseYear;
            break;
        case scenario_milestone::WagonsBought: current = aiWagonsBought; break;
    }
    int score = percentage::kBase * current / target;
    if (score < 0) score = 0;
    if (score > percentage::kBase)
        score = percentage::kBase;
    return score;
}

int8_t OfferBoard::display_progress(int32_t row, const Economy& economy) const {
    int progress_percent;
    if (offers[row].objectiveType == offer_objective::Objective1) {
        progress_percent = percentage::kBase * offers[row].progress
                         / offers[row].target;
    } else if (offers[row].objectiveType == offer_objective::Objective2) {
        if (economy.money == 0) {
            progress_percent = 0;
        } else {
            progress_percent = economy.money > 0
                ? percentage::kBase * economy.money
                  / offers[row].target
                : percentage::kBase * offers[row].target
                  / economy.money;
        }
    } else if (offers[row].objectiveType >= offer_objective::kFirstProgress
               && offers[row].objectiveType <= offer_objective::kLastProgress) {
        progress_percent = percentage::kBase * offers[row].progress
                         / offers[row].target;
    } else {
        return 0;
    }
    if (progress_percent > percentage::kBase)
        progress_percent = percentage::kBase;
    if (progress_percent < 0) progress_percent = 0;
    return (int8_t)progress_percent;
}

void MusicController::load(const std::filesystem::path& path) {
    available = output && output->load(path);
    if (available) {
        output->set_volume(20 * volumeLevel);
    }
    playing = false;
}

void MusicController::play() {
    if (available && !playing && enabled && output->play()) {
        playing = true;
    }
}

void MusicController::set_volume(int32_t level) {
    if (available) {
        output->set_volume(level * 20);
    }
}

void MusicController::stop() {
    if (available && playing) {
        output->stop();
    }
    playing = false;
}

void MusicController::update() {
    if (output) output->update();
}

void Game::update_audio() {
    music.update();
}

void Game::load_music() {
    music.load(resources::path_for("music.mid"));
}

void Game::play_music() {
    music.play();
}

void Game::set_music_volume(int32_t level) {
    music.set_volume(level);
}

void Game::stop_music() {
    music.stop();
}

bool Game::sync_high_scores(bool upload) {
    (void)upload;
    return false;
}
