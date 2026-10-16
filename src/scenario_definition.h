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
    int32_t stage7FeatureCount = 0;
    int32_t stage6FeatureCount = 0;
    int32_t weightAccel1 = 0;
    int32_t weightAccel2 = 0;
    int32_t weightAccel3 = 0;
    int32_t weightAccel4 = 0;
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
