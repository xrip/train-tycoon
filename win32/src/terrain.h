#ifndef TERRAIN_H
#define TERRAIN_H

#include "game_constants.h"
#include "support/grid.h"
#include <cstdint>

struct BuildController;

struct TileChange {
    int8_t countdown = 0;
    int8_t x = 0;
    int8_t y = 0;
};

class Terrain {
public:
    Grid<int8_t> tileMap;
    Grid<int8_t> tileElevation;
    Grid<int8_t> heightMap;

    int32_t mapWidth = 0;
    int32_t mapHeight = 0;
    int32_t mapHalfWidth = 0;
    int32_t mapScreenWidth = 0;
    int32_t mapScreenHeight = 0;

    TileChange tileQueue[kTileQueueCount]{};
    bool tileQueueVisualOnly[kTileQueueCount]{};
    int32_t woodAgeTimer = 0;
    int32_t phaseTimer = 0;

    void resize(int32_t width, int32_t height) {
        mapWidth = width;
        mapHeight = height;
        mapHalfWidth = (width - 1) * 8;
        mapScreenWidth = (width + height) * 16;
        mapScreenHeight = (width + height) * 8;
        tileMap.resize(width + 1, height + 1);
        tileElevation.resize(width + 1, height + 1);
        heightMap.resize(width + 2, height + 2);
    }

    void commit_edit(const BuildController& build);
    void rebuild(BuildController& build, bool edit_only);

    bool in_bounds(int32_t x, int32_t y) const {
        return x >= 0 && x < mapWidth && y >= 0 && y < mapHeight;
    }
};

#endif  // TERRAIN_H
