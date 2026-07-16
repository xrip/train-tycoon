// Scenario progress: the campaign score, high-score tables, and milestone
// thresholds (see game/sim_tick.cpp). AI opponent state and offer generation
// move into Scenario in later SCENARIO-01 leaves.
#ifndef SCENARIO_H
#define SCENARIO_H
#include <array>
#include <cstdint>
#include <vector>

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

    // AI opponent behaviour. aiEnabled is runtime-only; frequency, threshold,
    // and the wagon-purchase counter are saved. Values and save byte order are
    // unchanged. (The AI money/spend totals live in Economy.)
    bool aiEnabled = false;
    int8_t aiFrequency = 0;
    int8_t aiThreshold = 0;
    int32_t aiWagonsBought = 0;

    // Periodic contract/subsidy offer generation. offerEffectNextCount,
    // offerGenerationCount, and lastOfferDateCode are saved; the rest are
    // runtime-only. The shared menu-row display arrays (menuRow*) that these
    // offers populate stay on GameCanvas for UI-01. Save byte order is unchanged.
    int8_t offerRowIndex = 0;
    int8_t duplicateRowState = 0;
    int32_t offerTypeTransitionCode = 0;
    int32_t offerEffectTransitionCode = 0;
    int8_t routeSlotIndex = 0;
    std::array<int8_t, 6> routeResourceFlags{};
    std::array<std::vector<int8_t>, 12> offerTypesByEffect{};
    std::array<std::array<int8_t, 2>, 12> offerEffectSchedule{};
    int32_t offerGenerationCount = 0;
    std::array<int16_t, 12> offerEffectNextCount{};
    int32_t lastOfferDateCode = 0;

    // Scenario identity, difficulty setup, run progression, and high-score name
    // entry. scenarioBits, difficultyCycle, scenarioStep, and scenarioNoticeId
    // are saved through the main codec; scenarioStepCompare, playerNameCodes,
    // and scenarioResourceCount are read/written by game/boot.cpp. The rest are
    // runtime-only. activeScenario feeds the map digest. Save byte order is
    // unchanged.
    int8_t scenarioBits = 0;
    int32_t activeScenario = 0;
    int8_t difficultyA = 0;
    int8_t difficultySlot = 0;
    int8_t difficultyB = 0;
    int8_t difficultyC = 0;
    int32_t difficultyPct = 0;
    int8_t difficultyCycle = 0;
    int32_t scenarioStepCompare = 0;
    int32_t scenarioStep = 0;
    int32_t scenarioNoticeId = 0;
    int32_t scenarioResourceCount = 0;
    int8_t scenarioNoticeCode = 0;
    bool highScoreUploadRequested = false;
    std::array<int8_t, 10> playerNameCodes{};
    std::array<int8_t, 10> playerNameCursorMarker{};

    // Scenario feature flags (from the scenario_flags byte). aiExpansionEnabled
    // turns on the AI opponent's forest expansion and turns; monthlyOffersEnabled
    // turns on the monthly offer/economy tick. Both are saved; save byte order is
    // unchanged.
    bool aiExpansionEnabled = false;
    bool monthlyOffersEnabled = false;
};

#endif  // SCENARIO_H
