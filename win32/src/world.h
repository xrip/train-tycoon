// The game map and its owned grids.
#ifndef WORLD_H
#define WORLD_H
#include "game_constants.h"
#include "terrain.h"
#include "support/grid.h"
#include <cstdint>

// World owns terrain, track, cities, growth, and map-generation state. resize
// keeps the terrain and track grids at matching map dimensions.
struct City {
    int8_t stationDirection = 0;
    int8_t population = 0;
    int8_t zoneTileCount = 0;
    int16_t stationRevenue = 0;
    int16_t previousStationRevenue = 0;
};

struct World {
    Terrain terrain;
    Grid<int8_t> trackMap;       // track byte per cell (see namespace track)

    // City/station records (48-slot): each slot is one city on the map, with its
    // station direction, population, zone-tile count, and revenue history.
    City cities[kCitySlotCount]{};

    // Tile-map animation state. tileQueue holds up to 50 pending tile changes
    // (each: countdown, x, y) applied to tileMap during the tick;
    // tileQueueVisualOnly marks the ones that are display-only. woodAgeTimer
    // ages forest tiles and phaseTimer cycles the tile animation phase. All
    // runtime-only (not saved). Values and tick order are unchanged.

    // Per-zone growth state (64-slot zone grid derived from the map). zonePopulation
    // is recomputed each growth pass by counting populated tiles; constructionTimers
    // is the per-zone construction level. Both are saved. Values and save byte order
    // are unchanged.
    int8_t constructionTimers[kZoneCount]{};
    int16_t zonePopulation[kZoneCount]{};

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
    bool initialIndustryPlaced = true;

    // True when (x, y) is a valid map cell.
    void resize(int32_t width, int32_t height) {
        terrain.resize(width, height);
        trackMap.resize(width + 1, height + 1);
    }

    bool in_bounds(int32_t x, int32_t y) const {
        return terrain.in_bounds(x, y);
    }
};

#endif  // WORLD_H
