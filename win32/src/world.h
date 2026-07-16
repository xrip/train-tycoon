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
};

#endif  // WORLD_H
