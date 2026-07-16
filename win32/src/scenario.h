// Scenario progress: the campaign score, high-score tables, and milestone
// thresholds (see game/sim_tick.cpp). AI opponent state and offer generation
// move into Scenario in later SCENARIO-01 leaves.
#ifndef SCENARIO_H
#define SCENARIO_H
#include <array>
#include <cstdint>

// Scenario owns the campaign scoring and milestone progression. The milestone
// index/flag and thresholds are saved through the main codec; the best score
// and the score/leaderboard tables are saved through the high-score store
// (game/boot.cpp). scenarioMilestone and currentScenarioScore are runtime-only.
// Values and save byte order are unchanged.
struct Scenario {
    int8_t scenarioMilestone = 0;
    int8_t scenarioMilestoneIndex = 0;
    int32_t currentScenarioScore = 0;
    int32_t currentMilestoneScore = 0;
    int32_t milestoneCoThreshold2 = 0;
    int32_t milestoneThreshold1 = 0;
    int32_t milestoneThreshold2 = 0;
    int32_t milestoneThreshold3 = 0;
    int32_t milestoneThreshold4 = 0;
    int32_t milestoneThreshold5 = 0;
    bool milestonesEnabled = false;
    int32_t bestScenarioScore = 0;
    std::array<int32_t, 13> scenarioScores{};
    std::array<int32_t, 14> leaderboardScores{};
};

#endif  // SCENARIO_H
