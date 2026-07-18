// Terrain and tile queries and changes.
#include "game.h"
#include "support/numeric.h"
#include <stdlib.h>
#include <string.h>

bool World::is_placeable_terrain(int32_t x, int32_t y) const {
    return terrain.tileElevation[x][y] >= 0
           && (terrain.tileMap[x][y] == 0
               || (terrain.tileMap[x][y] >= tile::SCENERY_FIRST
                   && terrain.tileMap[x][y] <= tile::SCENERY_LAST))
           && trackMap[x][y] == 0;
}

void World::place_track_tile(int32_t x, int32_t y, int32_t tile_code,
                             int32_t track_owner) {
    terrain.tileMap[x][y] = (int8_t)tile_code;
    trackMap[x][y] = (int8_t)(track_overlay::STATION | track_owner);
}

bool World::is_curve_tile(int32_t x, int32_t y) const {
    const int8_t tile_code = terrain.tileMap[x][y];
    if (tile_code < tile::CURVE_UP || tile_code > tile::CURVE_DOWN) return false;
    bool connects_to_curve = false;
    if (tile_code == tile::CURVE_UP)
        connects_to_curve |= has_track_step(x, y, 0, -1, tile::CURVE_DOWN);
    if (tile_code == tile::CURVE_DOWN)
        connects_to_curve |= has_track_step(x, y, 0, 1, tile::CURVE_UP);
    if (tile_code == tile::CURVE_LEFT)
        connects_to_curve |= has_track_step(x, y, -1, 0, tile::CURVE_RIGHT);
    if (tile_code == tile::CURVE_RIGHT)
        connects_to_curve |= has_track_step(x, y, 1, 0, tile::CURVE_LEFT);
    return connects_to_curve;
}

bool World::is_city_or_station_tile(const Fleet& fleet, int32_t x,
                                    int32_t y) const {
    const int tile_code = terrain.tileMap[x][y];
    const int track_code = trackMap[x][y];
    const int32_t service_site_slot = fleet.find_service_site_at(x, y);
    const bool is_road_stop = service_site_slot != kNoFleetSlot
        && (fleet.serviceSites[service_site_slot].flags
            & service_site::TYPE_MASK) == kRoadStopSiteType;
    const bool is_city_or_station =
        is_road_stop
        || (tile_code >= tile::SCENERY_FIRST && tile_code <= tile::SCENERY_LAST)
        || (tile_code >= tile::TRACK_FIRST
            && tile_code < tile::CROSSING
            && track_code == tile::SERVICE_TRACK)
        || (tile_code == tile::CROSSING
            && (track_code & track::PRESENT_AND_LOW_DIRECTION_MASK)
                   == tile::SERVICE_TRACK)
        || (tile_code == tile::CROSSING
            && (track_code & track::PRESENT_AND_UPPER_DIRECTION_MASK)
                   == track::PRESENT_WITH_UPPER_OWNER_ONE)
        || ((tile_code == tile::VERTICAL_STATION
             || tile_code == tile::HORIZONTAL_STATION
             || tile_code == tile::VERTICAL_TERMINAL
             || tile_code == tile::HORIZONTAL_TERMINAL)
            && track_code == tile::SERVICE_TRACK);
    if (is_city_or_station) {
        for (int train_slot = 0; train_slot < kTrainSlotCount; ++train_slot) {
            if (!train_flag::isUsed(fleet.trainCars[train_slot].flags))
                continue;
            const int32_t train_x =
                unsigned_byte(fleet.trainCars[train_slot].x);
            const int32_t train_y =
                unsigned_byte(fleet.trainCars[train_slot].y);
            if (x == train_x && y == train_y) return false;
            if (service_site_slot == kNoFleetSlot
                || service_site_slot != fleet.find_service_site_at(train_x, train_y))
                continue;
            return false;
        }
        return true;
    }
    if (tile_code >= construction_tile::kFirst
        && tile_code <= construction_tile::kLast
        && track_code == tile::SERVICE_TRACK) {
        for (int train_slot = 0; train_slot < kTrainSlotCount; ++train_slot) {
            if (!train_flag::isUsed(fleet.trainCars[train_slot].flags))
                continue;
            if (tile_code == construction_tile::HorizontalA
                || tile_code == construction_tile::HorizontalB
                || tile_code == construction_tile::HorizontalC) {
                for (int scan_x = x; scan_x < terrain.mapWidth
                     && (terrain.tileMap[scan_x][y] == construction_tile::HorizontalA
                         || terrain.tileMap[scan_x][y] == construction_tile::HorizontalB
                         || terrain.tileMap[scan_x][y] == construction_tile::HorizontalC); ++scan_x) {
                    if (unsigned_byte(fleet.trainCars[train_slot].x) != scan_x
                        || unsigned_byte(fleet.trainCars[train_slot].y) != y)
                        continue;
                    return false;
                }
                for (int scan_x = x - 1; scan_x >= 0
                     && (terrain.tileMap[scan_x][y] == construction_tile::HorizontalA
                         || terrain.tileMap[scan_x][y] == construction_tile::HorizontalB
                         || terrain.tileMap[scan_x][y] == construction_tile::HorizontalC); --scan_x) {
                    if (unsigned_byte(fleet.trainCars[train_slot].x) != scan_x
                        || unsigned_byte(fleet.trainCars[train_slot].y) != y)
                        continue;
                    return false;
                }
                continue;
            }
            for (int scan_y = y; scan_y < terrain.mapHeight
                 && (terrain.tileMap[x][scan_y] == construction_tile::VerticalA
                     || terrain.tileMap[x][scan_y] == construction_tile::VerticalB
                     || terrain.tileMap[x][scan_y] == construction_tile::VerticalC); ++scan_y) {
                if (unsigned_byte(fleet.trainCars[train_slot].x) != x
                    || unsigned_byte(fleet.trainCars[train_slot].y) != scan_y)
                    continue;
                return false;
            }
            for (int scan_y = y - 1; scan_y >= 0
                 && (terrain.tileMap[x][scan_y] == construction_tile::VerticalA
                     || terrain.tileMap[x][scan_y] == construction_tile::VerticalB
                     || terrain.tileMap[x][scan_y] == construction_tile::VerticalC); --scan_y) {
                if (unsigned_byte(fleet.trainCars[train_slot].x) != x
                    || unsigned_byte(fleet.trainCars[train_slot].y) != scan_y)
                    continue;
                return false;
            }
        }
        return true;
    }
    return false;
}

bool World::is_tile_buildable(int32_t x, int32_t y, int32_t build_type) const {
    if (build_type == terrain_build_type::VerticalTerminal)
        return is_tile_buildable(x, y, terrain_build_type::SingleTile)
               && is_tile_buildable(x, y - 1, terrain_build_type::SingleTile)
               && is_tile_buildable(x, y + 1, terrain_build_type::SingleTile);
    if (build_type == terrain_build_type::HorizontalTerminal)
        return is_tile_buildable(x, y, terrain_build_type::SingleTile)
               && is_tile_buildable(x - 1, y, terrain_build_type::SingleTile)
               && is_tile_buildable(x + 1, y, terrain_build_type::SingleTile);
    if (!in_bounds(x, y)) return false;
    int tile = terrain.tileMap[x][y];
    bool buildable = terrain.tileElevation[x][y] >= 0
                     && (tile == 0 || (tile >= tile::SCENERY_FIRST && tile <= tile::SCENERY_LAST))
                     && trackMap[x][y] == 0;
    if (build_type == terrain_build_type::DiagonalA
        && (tile == track_build_tile::AlongX
            || tile == track_build_tile::OpenGroundVariantA
            || tile == track_build_tile::OpenGroundVariantC))
        buildable = true;
    if (build_type == terrain_build_type::DiagonalB
        && (tile == track_build_tile::AlongY
            || tile == track_build_tile::OpenGroundVariantB
            || tile == track_build_tile::OpenGroundVariantD))
        buildable = true;
    return buildable;
}

// void commitTerrainEdit() -- copy the edit buffer back into heightMap
void Terrain::commit_edit(const BuildController& build) {
    for (int edit_row = 0; edit_row < kTerrainEditGridSide; ++edit_row) {
        memcpy(heightMap[build.selectionStartX + edit_row].data()
                   + build.selectionStartY,
               build.terrainEditBuffer[edit_row].data(),
               kTerrainEditGridSide);
    }
}

void BuildController::set_terrain_selection(const Terrain& terrain,
                                            int32_t center_x,
                                            int32_t center_y) {
    const int32_t edit_center_offset = kTerrainEditGridSide / 2;
    const int32_t last_start_adjustment = kTerrainEditGridSide - 1;
    selectionStartX = center_x - edit_center_offset;
    selectionStartY = center_y - edit_center_offset;
    if (selectionStartX < 0) selectionStartX = 0;
    if (selectionStartY < 0) selectionStartY = 0;
    if (selectionStartX + kTerrainEditGridSide >= terrain.mapWidth)
        selectionStartX = terrain.mapWidth - last_start_adjustment;
    if (selectionStartY + kTerrainEditGridSide >= terrain.mapHeight)
        selectionStartY = terrain.mapHeight - last_start_adjustment;
    // this.terrainEditBuffer = new byte[kTerrainEditGridSide][kTerrainEditGridSide];
    terrainEditBuffer.resize(kTerrainEditGridSide, kTerrainEditGridSide);
    for (int edit_row = 0; edit_row < kTerrainEditGridSide; ++edit_row) {
        memcpy(terrainEditBuffer[edit_row].data(),
               terrain.heightMap[selectionStartX + edit_row].data()
                   + selectionStartY,
               kTerrainEditGridSide);
    }
    terrainSmoothCost = 0;
}
