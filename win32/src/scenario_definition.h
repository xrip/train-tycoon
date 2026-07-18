#ifndef SCENARIO_DEFINITION_H
#define SCENARIO_DEFINITION_H

#include "support/resource_reader.h"

#include <cstdint>

struct ScenarioDefinition {
    int32_t scenarioIndex = 0;
    int32_t mapSize = 0;
    int32_t terrainRaiseSteps = 0;
    int32_t terrainLowerSteps = 0;
    int32_t obstacleCount = 0;
    int32_t terrainPatchSize = 0;
    int32_t mapGenColumns = 0;
    int32_t mapGenRows = 0;
    int32_t featureStage3Count = 0;
    int32_t featureStage2Count = 0;
    int32_t featureStage1Count = 0;
    int32_t featureStage0Count = 0;
    int32_t featureStage5Count = 0;
    int32_t featureStage4Count = 0;
    int32_t startingMoney = 0;
    int32_t loanCeiling = 0;
    int32_t startingLoan = 0;
    int32_t interestRate = 0;
    int32_t eraNumber = 0;
    int32_t noticeId = 0;
    int32_t milestoneIndex = 0;
    int32_t milestoneScore = 0;
    int32_t milestoneThreshold = 0;
    int32_t buildMenuScenario = 0;
    int32_t scenarioFlags = 0;
};

bool readScenarioDefinition(ResourceReader& resource,
                            ScenarioDefinition& definition);

#endif
