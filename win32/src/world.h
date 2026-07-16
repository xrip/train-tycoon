// The game map and its owned grids.
#ifndef WORLD_H
#define WORLD_H
#include "support/grid.h"
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
};

#endif  // WORLD_H
