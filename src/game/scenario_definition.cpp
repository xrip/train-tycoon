#include "scenario_definition.h"

namespace {

int readU16Le(ResourceReader& resource) {
    const int low = resource.read_byte();
    const int high = resource.read_byte();
    return low + high * 256;
}

int readU24Le(ResourceReader& resource) {
    const int low = resource.read_byte();
    const int middle = resource.read_byte();
    const int high = resource.read_byte();
    return low + middle * 256 + high * 65536;
}

} // namespace

bool readScenarioDefinition(ResourceReader& resource,
                            ScenarioDefinition& definition) {
    const int scenarioIndex = resource.read_byte();
    if (scenarioIndex < 0) return false;

    definition.scenarioIndex = scenarioIndex;
    definition.mapSize = resource.read_byte();
    definition.terrainRaiseSteps = resource.read_byte();
    definition.terrainLowerSteps = resource.read_byte();
    definition.obstacleCount = readU16Le(resource);
    definition.terrainPatchSize = resource.read_byte();
    definition.mapGenColumns = resource.read_byte();
    definition.mapGenRows = resource.read_byte();
    definition.stage7FeatureCount = resource.read_byte();
    definition.stage6FeatureCount = resource.read_byte();
    definition.weightAccel1 = resource.read_byte();
    definition.weightAccel2 = resource.read_byte();
    definition.weightAccel3 = resource.read_byte();
    definition.weightAccel4 = resource.read_byte();
    definition.startingMoney = readU24Le(resource);
    definition.loanCeiling = readU24Le(resource);
    definition.startingLoan = readU24Le(resource);
    definition.interestRate = resource.read_byte();
    definition.eraNumber = resource.read_byte();
    definition.noticeId = resource.read_byte();
    definition.milestoneIndex = resource.read_byte();
    definition.milestoneScore = readU24Le(resource);
    definition.milestoneThreshold = readU24Le(resource);
    definition.buildMenuScenario = resource.read_byte();
    definition.scenarioFlags = resource.read_byte();
    return true;
}
