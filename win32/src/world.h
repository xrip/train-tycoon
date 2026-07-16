// The game map and its owned grids.
#ifndef WORLD_H
#define WORLD_H
#include "support/grid.h"
#include <array>
#include <cstdint>

// World owns the four map grids. Each is a Grid<int8_t> indexed [x][y]; rows may
// be padded (tileMap/trackMap/tileElevation to mapWidth+1/mapHeight+1, heightMap
// to +2). Values and byte order match the legacy save. Map dimensions and the
// map query/geometry helpers move onto World in later WORLD-01 leaves.
struct World {
    Grid<int8_t> tileMap;        // terrain and structure tile id per cell
    Grid<int8_t> trackMap;       // track byte per cell (see namespace track)
    Grid<int8_t> tileElevation;  // elevation step per cell
    Grid<int8_t> heightMap;      // height field, padded by two

    int32_t mapWidth = 0;        // map cell columns
    int32_t mapHeight = 0;       // map cell rows
    int32_t mapHalfWidth = 0;    // (mapWidth - 1) * 8
    int32_t mapScreenWidth = 0;  // (mapWidth + mapHeight) * 16
    int32_t mapScreenHeight = 0; // (mapWidth + mapHeight) * 8

    // City/station records (48-slot): each slot is one city on the map.
    // stationDirectionCode is the track approach direction; cityPopulation and
    // cityIndex describe the city. Per-station revenue history
    // (stationRevenue/prevStationRevenue) is economy state and lives elsewhere.
    std::array<int8_t, 48> stationDirectionCode{};
    std::array<int8_t, 48> cityPopulation{};
    std::array<int8_t, 48> cityIndex{};

    // Tile-map animation state. tileQueue holds up to 50 pending tile changes
    // (each: countdown, x, y) applied to tileMap during the tick;
    // tileQueueVisualOnly marks the ones that are display-only. woodAgeTimer
    // ages forest tiles and phaseTimer cycles the tile animation phase. All
    // runtime-only (not saved). Values and tick order are unchanged.
    std::array<std::array<int8_t, 3>, 50> tileQueue{};
    std::array<bool, 50> tileQueueVisualOnly{};
    int32_t woodAgeTimer = 0;
    int32_t phaseTimer = 0;

    // Per-zone growth state (64-slot zone grid derived from the map). zonePopulation
    // is recomputed each growth pass by counting populated tiles; constructionTimers
    // is the per-zone construction level. Both are saved. Values and save byte order
    // are unchanged.
    std::array<int8_t, 64> constructionTimers{};
    std::array<int16_t, 64> zonePopulation{};

    // Map-generation parameters (loaded per scenario), the generation progress
    // counters, and the industry/city placement bookkeeping used while building
    // the map. industryCategory and industrySiteFound are saved; the rest are
    // runtime-only. Values and save byte order are unchanged.
    int32_t initialObstacleCount = 0;
    int32_t terrainRaiseStepCount = 0;
    int32_t terrainLowerStepCount = 0;
    int32_t mapGenColumns = 0;
    int32_t mapGenRows = 0;
    int32_t terrainPatchBaseSize = 0;
    int32_t terrainFeatureStage7Count = 0;
    int32_t terrainFeatureStage6Count = 0;
    int32_t weightAccel = 0;
    int32_t weightAccel2 = 0;
    int32_t weightAccel3 = 0;
    int32_t weightAccel4 = 0;
    int32_t mapGenStage = 0;
    int32_t tilesGeneratedInStage = 0;
    int32_t totalTilesToGenerate = 0;
    int32_t mapGenTilesDone = 0;
    int32_t industryCategory = 0;
    bool industrySiteFound = false;
    int32_t cityStationX = 0;
    int32_t cityStationY = 0;
    int32_t industryStationX = 0;
    int32_t industryTileY = 0;
    bool initialIndustryPlaced = false;

    // True when (x, y) is a valid map cell.
    bool in_bounds(int32_t x, int32_t y) const {
        return x >= 0 && x < mapWidth && y >= 0 && y < mapHeight;
    }
};

#endif  // WORLD_H
