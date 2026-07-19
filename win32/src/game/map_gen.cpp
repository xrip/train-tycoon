// Terrain and map generation.
// Driven by loadState25Setup across mapGenStage 0..10 (raise/lower terrain,
// place patches and features, scatter industry). The random process is exact.
#include "game.h"
#include <stdlib.h> // abs
#include <vector>

// Game::mapGenStage steps through the incremental generator in
// game_runMapGenerationStep: init, raise then lower terrain, place patches, six
// terrain-feature passes, then scatter starting obstacles.
namespace mapgen {
inline constexpr int32_t STAGE_INIT = 0;
inline constexpr int32_t STAGE_RAISE_TERRAIN = 1;
inline constexpr int32_t STAGE_LOWER_TERRAIN = 2;
inline constexpr int32_t STAGE_PLACE_PATCHES = 3;
inline constexpr int32_t STAGE_FEATURE_0 = 4;
inline constexpr int32_t STAGE_FEATURE_1 = 5;
inline constexpr int32_t STAGE_FEATURE_2 = 6;
inline constexpr int32_t STAGE_FEATURE_3 = 7;
inline constexpr int32_t STAGE_FEATURE_4 = 8;
inline constexpr int32_t STAGE_FEATURE_5 = 9;
inline constexpr int32_t STAGE_SCATTER_OBSTACLES = 10;
}  // namespace mapgen

namespace {

// The generator stores six feature families in the tile map.  The group
// labels are stable, but the source does not prove real commodity names.
namespace terrain_feature {
enum Id : int32_t {
    IndustryGroup1Destination = 0,
    IndustryGroup2Production = 1,
    IndustryGroup2Destination = 2,
    IndustryGroup3Production = 3,
    IndustryGroup3Destination = 4,
    IndustryGroup1Production = 5,
};
}  // namespace terrain_feature

namespace terrain_feature_tile {
enum Id : int32_t {
    IndustryGroup1DestinationA = 50,
    IndustryGroup1DestinationB = 51,
    IndustryGroup1DestinationC = 52,
    IndustryGroup2ProductionA = 53,
    IndustryGroup2ProductionB = 54,
    IndustryGroup2ProductionC = 55,
    IndustryGroup2DestinationA = 56,
    IndustryGroup2DestinationB = 57,
    IndustryGroup2DestinationC = 58,
    IndustryGroup3ProductionA = 59,
    IndustryGroup3ProductionB = 60,
    IndustryGroup3ProductionC = 61,
    IndustryGroup3DestinationA = 62,
    IndustryGroup3DestinationB = 63,
    IndustryGroup3DestinationC = 64,
    IndustryGroup1ProductionA = 65,
    IndustryGroup1ProductionB = 66,
    IndustryGroup1ProductionC = 67,
    IndustryGroup1ProductionD = 68,
};
}  // namespace terrain_feature_tile

namespace feature_placement {
constexpr int32_t kMapMargin = 8;
constexpr int32_t kMapOffset = 4;
constexpr int32_t kClearanceRadius = 3;
constexpr int32_t kSmallSize = 2;
constexpr int32_t kLargeSize = 3;
constexpr int32_t kMaxPlacementAttempts = 50;
constexpr int64_t kTimeoutMillis = 1500;
}  // namespace feature_placement

namespace city_generation {
constexpr int32_t kInitialPopulation = 50;
constexpr int32_t kPopulationRandomRange = 20;
}  // namespace city_generation

namespace terrain_height_generation {
constexpr int32_t kAreaSizeRandomRange = 20;
constexpr int32_t kMinimumAreaSize = 1;
constexpr int32_t kElevationStepRandomRange = 2;
constexpr int32_t kFirstElevationStep = 1;
constexpr int32_t kLoweringDelta = -1;
}  // namespace terrain_height_generation

namespace city_tile_distribution {
constexpr int32_t kRandomRange = 22;
constexpr int32_t kVariant1Limit = 6;
constexpr int32_t kVariant2Limit = 11;
constexpr int32_t kVariant3Limit = 15;
constexpr int32_t kVariant4Limit = 18;
constexpr int32_t kVariant5Limit = 20;
constexpr int32_t kVariant6Limit = 21;
constexpr int32_t kVariant7Limit = 22;
}  // namespace city_tile_distribution

namespace map_train_selection {
constexpr int32_t kEngineVariantCount = 6;
constexpr int32_t kWagonVariantCount = 14;
constexpr int32_t kEngineVariantShift = 8;
constexpr int32_t kMaxStoredZonePopulation = 32767;
constexpr int8_t kInitialConstructionTimer = 50;
}  // namespace map_train_selection

namespace terrain_edit {
constexpr int32_t kSelectionRadius = 5;
constexpr int32_t kSelectionSize = 10;
constexpr int32_t kMaximumElevation = 3;
constexpr int32_t kMinimumElevation = -1;
constexpr int32_t kRandomNeighborRange = 2;
constexpr int32_t kLoweringRepeats = 3;
constexpr int8_t kUninitializedElevation = -2;
}  // namespace terrain_edit

namespace terrain_patch {
enum SeedType : int8_t {
    FirstSeed = 1,
    SecondSeed = 2,
};
constexpr int32_t kPatchSizeRandomRange = 5;
constexpr int32_t kSeedMargin = 2;
constexpr int32_t kSeedOffset = 1;
constexpr int32_t kShapeRandomRange = 3;
constexpr int32_t kLargePatchThreshold = 7;
constexpr int32_t kSmoothingTimeoutMillis = 1000;
constexpr int32_t kCityOverlayFlag = 0x80;
}  // namespace terrain_patch

}  // namespace

int8_t World::random_city_tile(JavaRandom& random) const {
    const int city_tile_roll =
        abs(random.next_int() % city_tile_distribution::kRandomRange);
    if (city_tile_roll < city_tile_distribution::kVariant1Limit)
        return city_zone_tile::VariantA;
    if (city_tile_roll < city_tile_distribution::kVariant2Limit)
        return city_zone_tile::VariantB;
    if (city_tile_roll < city_tile_distribution::kVariant3Limit)
        return city_zone_tile::VariantC;
    if (city_tile_roll < city_tile_distribution::kVariant4Limit)
        return city_zone_tile::VariantD;
    if (city_tile_roll < city_tile_distribution::kVariant5Limit)
        return city_zone_tile::VariantE;
    if (city_tile_roll < city_tile_distribution::kVariant6Limit)
        return city_zone_tile::VariantF;
    if (city_tile_roll < city_tile_distribution::kVariant7Limit)
        return city_zone_tile::VariantG;
    return 0;
}

int8_t Game::add_city_entry(int32_t station_direction) {
    Game* game = this;
    game->world.cities[game->ui.cityListIndex].stationDirection =
        static_cast<int8_t>(station_direction);
    game->world.cities[game->ui.cityListIndex].population = static_cast<int8_t>(
        city_generation::kInitialPopulation
        + game->rng.next_int() % city_generation::kPopulationRandomRange);
    game->world.cities[game->ui.cityListIndex].zoneTileCount = 0;
    game->ui.cityListIndex = (int8_t)(game->ui.cityListIndex + 1);
    return (int8_t)(game->ui.cityListIndex - 1);
}

int8_t Game::register_map_train(int32_t zone_population,
                                int32_t selection_y,
                                int32_t selection_x) {
    Game* game = this;
    game->world.constructionTimers[game->ui.numSelectedTrains] =
        map_train_selection::kInitialConstructionTimer;
    game->world.zonePopulation[game->ui.numSelectedTrains] =
        static_cast<int16_t>(
            zone_population > map_train_selection::kMaxStoredZonePopulation
                ? map_train_selection::kMaxStoredZonePopulation
                : zone_population);
    game->ui.trainSelectionY[game->ui.numSelectedTrains] =
        static_cast<int16_t>(selection_y);
    game->ui.trainSelectionX[game->ui.numSelectedTrains] =
        static_cast<int16_t>(selection_x);
    int16_t selection_index = 0;
    bool selection_is_unique;
    do {
        const int8_t engine_variant = static_cast<int8_t>(
            abs(game->rng.next_int() % map_train_selection::kEngineVariantCount));
        const int8_t wagon_variant = static_cast<int8_t>(
            abs(game->rng.next_int() % map_train_selection::kWagonVariantCount));
        selection_index = static_cast<int16_t>(
            (engine_variant << map_train_selection::kEngineVariantShift)
            + wagon_variant);
        selection_is_unique = true;
        for (int selection_slot = 0;
             selection_slot < game->ui.numSelectedTrains; ++selection_slot)
            if (selection_index == game->ui.trainSelectionIndex[selection_slot])
                selection_is_unique = false;
    } while (!selection_is_unique);
    game->ui.trainSelectionIndex[game->ui.numSelectedTrains] = selection_index;
    game->ui.numSelectedTrains = (int8_t)(game->ui.numSelectedTrains + 1);
    return (int8_t)(game->ui.numSelectedTrains - 1);
}

// Rebuild slope tiles from the height map.
void Terrain::rebuild(BuildController& build, bool rebuild_selection) {
    int min_x, max_x, min_y, max_y;
    if (rebuild_selection) {
        commit_edit(build);
        build.terrainEditBuffer.clear();
        min_x = build.selectionStartX;
        min_y = build.selectionStartY;
        max_x = build.selectionStartX + terrain_edit::kSelectionSize;
        max_y = build.selectionStartY + terrain_edit::kSelectionSize;
    } else {
        min_x = 0;
        min_y = 0;
        max_x = mapWidth;
        max_y = mapHeight;
    }
    for (int tile_x = min_x; tile_x < max_x; ++tile_x) {
        for (int tile_y = min_y; tile_y < max_y; ++tile_y) {
            int8_t height_00 = heightMap[tile_x][tile_y];
            int8_t height_01 = heightMap[tile_x][tile_y + 1];
            int8_t height_10 = heightMap[tile_x + 1][tile_y];
            int8_t height_11 = heightMap[tile_x + 1][tile_y + 1];
            if (tileMap[tile_x][tile_y] >= tile::BUILT_FIRST) continue;
            if (height_00 == height_01 && height_00 == height_10
                && height_00 == height_11) {
                tileElevation[tile_x][tile_y] = height_00;
                if (tileMap[tile_x][tile_y] <= 28)
                    tileMap[tile_x][tile_y] = 0;
            } else if (height_00 == height_01 && height_00 == height_10
                       && height_11 < height_00) {
                tileElevation[tile_x][tile_y] = height_00;
                tileMap[tile_x][tile_y] = 11;
            }
            if (height_00 == height_10 && height_00 == height_11
                && height_01 < height_00) {
                tileElevation[tile_x][tile_y] = height_00;
                tileMap[tile_x][tile_y] = 13;
            }
            if (height_00 == height_01 && height_00 == height_11
                && height_10 < height_00) {
                tileElevation[tile_x][tile_y] = height_00;
                tileMap[tile_x][tile_y] = 14;
            }
            if (height_01 == height_10 && height_01 == height_11
                && height_00 < height_01) {
                tileElevation[tile_x][tile_y] = height_01;
                tileMap[tile_x][tile_y] = 12;
            }
            if (height_00 == height_11 && height_01 == height_10
                && height_01 < height_00) {
                tileElevation[tile_x][tile_y] = height_00;
                tileMap[tile_x][tile_y] = 16;
            }
            if (height_00 == height_11 && height_01 == height_10
                && height_00 < height_01) {
                tileElevation[tile_x][tile_y] = height_01;
                tileMap[tile_x][tile_y] = 15;
            }
            if (height_00 == height_01 && height_10 == height_11
                && height_10 < height_00) {
                tileElevation[tile_x][tile_y] = height_00;
                tileMap[tile_x][tile_y] = static_cast<int8_t>(
                    height_10 == -1 && tile_x + 2 <= mapWidth
                        && (heightMap[tile_x + 2][tile_y] == -1
                            || heightMap[tile_x + 2][tile_y + 1] == -1)
                        ? 9 : 24);
            }
            if (height_00 == height_01 && height_10 == height_11
                && height_00 < height_10) {
                tileElevation[tile_x][tile_y] = height_10;
                tileMap[tile_x][tile_y] = static_cast<int8_t>(
                    height_00 == -1 && tile_x - 1 >= 0
                        && (heightMap[tile_x - 1][tile_y] == -1
                            || heightMap[tile_x - 1][tile_y + 1] == -1)
                        ? 8 : 22);
            }
            if (height_00 == height_10 && height_01 == height_11
                && height_01 < height_00) {
                tileElevation[tile_x][tile_y] = height_00;
                tileMap[tile_x][tile_y] = static_cast<int8_t>(
                    height_01 == -1 && tile_y + 2 <= mapHeight
                        && (heightMap[tile_x][tile_y + 2] == -1
                            || heightMap[tile_x + 1][tile_y + 2] == -1)
                        ? 10 : 21);
            }
            if (height_00 == height_10 && height_01 == height_11
                && height_00 < height_01) {
                tileElevation[tile_x][tile_y] = height_01;
                tileMap[tile_x][tile_y] = static_cast<int8_t>(
                    height_00 == -1 && tile_y - 1 >= 0
                        && (heightMap[tile_x][tile_y - 1] == -1
                            || heightMap[tile_x + 1][tile_y - 1] == -1)
                        ? 7 : 23);
            }
            if (height_00 == height_01 && height_00 == height_10
                && height_00 < height_11) {
                tileElevation[tile_x][tile_y] = height_00;
                tileMap[tile_x][tile_y] = static_cast<int8_t>(
                    height_00 == -1 ? 3 : 19);
            }
            if (height_00 == height_01 && height_00 == height_11
                && height_00 < height_10) {
                tileElevation[tile_x][tile_y] = height_00;
                tileMap[tile_x][tile_y] = static_cast<int8_t>(
                    height_00 == -1 ? 6 : 18);
            }
            if (height_00 == height_10 && height_00 == height_11
                && height_00 < height_01) {
                tileElevation[tile_x][tile_y] = height_00;
                tileMap[tile_x][tile_y] = static_cast<int8_t>(
                    height_00 == -1 ? 5 : 20);
            }
            if (height_01 == height_10 && height_01 == height_11
                && height_11 < height_00) {
                tileElevation[tile_x][tile_y] = height_11;
                tileMap[tile_x][tile_y] = static_cast<int8_t>(
                    height_11 == -1 ? 4 : 17);
            }
            if (height_00 == height_11 && height_01 < height_00
                && height_10 > height_00) {
                tileElevation[tile_x][tile_y] = height_00;
                tileMap[tile_x][tile_y] = 25;
            }
            if (height_10 == height_01 && height_00 < height_10
                && height_11 > height_10) {
                tileElevation[tile_x][tile_y] = height_10;
                tileMap[tile_x][tile_y] = 26;
            }
            if (height_00 == height_11 && height_01 > height_00
                && height_10 < height_00) {
                tileElevation[tile_x][tile_y] = height_00;
                tileMap[tile_x][tile_y] = 27;
            }
            if (height_10 != height_01 || height_00 <= height_10
                || height_11 >= height_10)
                continue;
            tileElevation[tile_x][tile_y] = height_10;
            tileMap[tile_x][tile_y] = 28;
        }
    }
}

void Game::smooth_tile_at(int32_t tile_x, int32_t tile_y, bool raise) {
    Game *game = this;
    const int current_elevation =
        game->world.terrain.heightMap[tile_x][tile_y];
    if ((raise && current_elevation < terrain_edit::kMaximumElevation)
        || (!raise && current_elevation > terrain_edit::kMinimumElevation)) {
        game->build.set_terrain_selection(game->world.terrain, tile_x, tile_y);
        if (game->build.adjust_terrain_point(
                game->world.terrain, game->economy, tile_x, tile_y,
                current_elevation + (raise ? 1 : -1), raise)) {
            if (game->world.initialIndustryPlaced) {
                if (game->build.terrainSmoothCost <= game->economy.money) {
                    game->economy.money -= game->build.terrainSmoothCost;
                    game->record_money_change(-game->build.terrainSmoothCost);
                    game->economy.subsidyCost += game->build.terrainSmoothCost;
                    game->world.terrain.rebuild(game->build, true);
                } else {
                    game->enter_out_of_money_state();
                }
            } else {
                game->world.terrain.rebuild(game->build, true);
            }
        }
    }
    game->renderer.lastViewportCenterX = -1;
}

void Game::raise_terrain_area(int32_t start_x, int32_t start_y,
                              int32_t width, int32_t height,
                              int32_t elevation_step) {
    Game *game = this;
    for (int tile_x = start_x; tile_x < start_x + width; ++tile_x) {
        for (int tile_y = start_y; tile_y < start_y + height; ++tile_y) {
            if (tile_x <= 0 || tile_y <= 0
                || tile_x > game->world.terrain.mapWidth
                || tile_y > game->world.terrain.mapHeight)
                continue;
            game->build.set_terrain_selection(game->world.terrain,
                                               tile_x, tile_y);
            game->build.adjust_terrain_point(
                game->world.terrain, game->economy, tile_x, tile_y,
                elevation_step, true);
            if (game->rng.next_int() % terrain_edit::kRandomNeighborRange < 0
                && tile_x > 0)
                game->build.adjust_terrain_point(
                    game->world.terrain, game->economy, tile_x - 1, tile_y,
                    elevation_step, true);
            if (game->rng.next_int() % terrain_edit::kRandomNeighborRange < 0
                && tile_y > 0)
                game->build.adjust_terrain_point(
                    game->world.terrain, game->economy, tile_x, tile_y - 1,
                    elevation_step, true);
            if (game->rng.next_int() % terrain_edit::kRandomNeighborRange < 0
                && tile_x < game->world.terrain.mapWidth)
                game->build.adjust_terrain_point(
                    game->world.terrain, game->economy, tile_x + 1, tile_y,
                    elevation_step, true);
            if (game->rng.next_int() % terrain_edit::kRandomNeighborRange < 0
                && tile_y < game->world.terrain.mapHeight)
                game->build.adjust_terrain_point(
                    game->world.terrain, game->economy, tile_x, tile_y + 1,
                    elevation_step, true);
            game->world.terrain.commit_edit(game->build);
        }
    }
}

void Game::lower_terrain_area(int32_t start_x, int32_t start_y,
                              int32_t width, int32_t height,
                              int32_t elevation_step) {
    Game *game = this;
    (void)elevation_step;
    for (int tile_x = start_x; tile_x < start_x + width; ++tile_x) {
        for (int tile_y = start_y; tile_y < start_y + height; ++tile_y) {
            if (tile_x <= 0 || tile_y <= 0
                || tile_x > game->world.terrain.mapWidth
                || tile_y > game->world.terrain.mapHeight)
                continue;
            for (int repeat = 0; repeat < terrain_edit::kLoweringRepeats;
                 ++repeat)
                game->smooth_tile_at(tile_x, tile_y, false);
            if (game->rng.next_int() % terrain_edit::kRandomNeighborRange < 0
                && tile_x > 0)
                game->smooth_tile_at(tile_x - 1, tile_y, false);
            if (game->rng.next_int() % terrain_edit::kRandomNeighborRange < 0
                && tile_y > 0)
                game->smooth_tile_at(tile_x, tile_y - 1, false);
            if (game->rng.next_int() % terrain_edit::kRandomNeighborRange < 0
                && tile_x < game->world.terrain.mapWidth)
                game->smooth_tile_at(tile_x + 1, tile_y, false);
            if (game->rng.next_int() % terrain_edit::kRandomNeighborRange >= 0
                || tile_y >= game->world.terrain.mapHeight)
                continue;
            game->smooth_tile_at(tile_x, tile_y + 1, false);
        }
    }
}

void Game::track_build_step(int32_t start_x, int32_t start_y,
                            int32_t width, int32_t height) {
    Game *game = this;
    int8_t target_elevation = terrain_edit::kUninitializedElevation;
    for (int tile_x = start_x; tile_x < start_x + width; ++tile_x) {
        for (int tile_y = start_y; tile_y < start_y + height; ++tile_y) {
            if (tile_x >= game->world.terrain.mapWidth
                || tile_y >= game->world.terrain.mapHeight)
                continue;
            if (target_elevation == terrain_edit::kUninitializedElevation
                && (target_elevation =
                        game->world.terrain.heightMap[tile_x][tile_y]) < 0)
                target_elevation = 0;
            while (target_elevation
                   != game->world.terrain.heightMap[tile_x][tile_y])
                game->smooth_tile_at(
                    tile_x, tile_y,
                    target_elevation
                        > game->world.terrain.heightMap[tile_x][tile_y]);
            while (target_elevation
                   != game->world.terrain.heightMap[tile_x + 1][tile_y])
                game->smooth_tile_at(
                    tile_x + 1, tile_y,
                    target_elevation
                        > game->world.terrain.heightMap[tile_x + 1][tile_y]);
            while (target_elevation
                   != game->world.terrain.heightMap[tile_x][tile_y + 1])
                game->smooth_tile_at(
                    tile_x, tile_y + 1,
                    target_elevation
                        > game->world.terrain.heightMap[tile_x][tile_y + 1]);
            while (target_elevation
                   != game->world.terrain.heightMap[tile_x + 1][tile_y + 1])
                game->smooth_tile_at(
                    tile_x + 1, tile_y + 1,
                    target_elevation
                        > game->world.terrain.heightMap[tile_x + 1][tile_y + 1]);
        }
    }
}

void Game::place_terrain_patch(int32_t origin_x, int32_t origin_y) {
    Game* game = this;
    const int patch_width = abs(game->rng.next_int()
                                % terrain_patch::kPatchSizeRandomRange)
                          + game->world.terrainPatchBaseSize;
    const int patch_height = abs(game->rng.next_int()
                                 % terrain_patch::kPatchSizeRandomRange)
                           + game->world.terrainPatchBaseSize;
    std::vector<std::vector<int8_t>> patch_tiles(
        static_cast<size_t>(patch_width),
        std::vector<int8_t>(static_cast<size_t>(patch_height)));
    bool seed_conflict;
    int seed_line;
    int seed_position;
    seed_line = abs(game->rng.next_int() % (patch_width - terrain_patch::kSeedMargin))
              + terrain_patch::kSeedOffset;
    for (seed_position = abs(game->rng.next_int()
                             % terrain_patch::kShapeRandomRange);
         seed_position < patch_height
             - abs(game->rng.next_int() % terrain_patch::kShapeRandomRange);
         ++seed_position)
        patch_tiles[seed_line][seed_position] = terrain_patch::FirstSeed;
    if (patch_width >= terrain_patch::kLargePatchThreshold) {
        do {
            seed_line = abs(game->rng.next_int()
                            % (patch_width - terrain_patch::kSeedMargin))
                      + terrain_patch::kSeedOffset;
            seed_conflict = false;
            for (seed_position = 0; seed_position < patch_height; ++seed_position)
                if (patch_tiles[seed_line - 1][seed_position]
                        == terrain_patch::FirstSeed
                    || patch_tiles[seed_line + 1][seed_position]
                        == terrain_patch::FirstSeed)
                    seed_conflict = true;
        } while (seed_conflict);
        for (seed_position = abs(game->rng.next_int()
                                 % terrain_patch::kShapeRandomRange);
             seed_position < patch_height
                 - abs(game->rng.next_int() % terrain_patch::kShapeRandomRange);
             ++seed_position)
            patch_tiles[seed_line][seed_position] = terrain_patch::FirstSeed;
    }
    seed_line = abs(game->rng.next_int()
                    % (patch_height - terrain_patch::kSeedMargin))
              + terrain_patch::kSeedOffset;
    for (int seed_row = abs(game->rng.next_int()
                            % terrain_patch::kShapeRandomRange);
         seed_row < patch_width
             - abs(game->rng.next_int() % terrain_patch::kShapeRandomRange);
         ++seed_row)
        patch_tiles[seed_row][seed_line] = terrain_patch::SecondSeed;
    if (patch_height >= terrain_patch::kLargePatchThreshold) {
        do {
            seed_line = abs(game->rng.next_int()
                            % (patch_height - terrain_patch::kSeedMargin))
                      + terrain_patch::kSeedOffset;
            seed_conflict = false;
            for (int seed_row = 0; seed_row < patch_width; ++seed_row)
                if (patch_tiles[seed_row][seed_line - 1]
                        == terrain_patch::SecondSeed
                    || patch_tiles[seed_row][seed_line + 1]
                        == terrain_patch::SecondSeed)
                    seed_conflict = true;
        } while (seed_conflict);
    }

    for (int patch_x = 0; patch_x < patch_width; ++patch_x) {
        for (int patch_y = 0; patch_y < patch_height; ++patch_y) {
            if (patch_tiles[patch_x][patch_y] != terrain_patch::FirstSeed
                && patch_tiles[patch_x][patch_y] != terrain_patch::SecondSeed)
                continue;
            const bool previous_row_present = patch_x > 0
                && patch_tiles[patch_x - 1][patch_y] > 0;
            const bool next_row_present = patch_x < patch_width - 1
                && patch_tiles[patch_x + 1][patch_y] > 0;
            const bool next_column_present = patch_y < patch_height - 1
                && patch_tiles[patch_x][patch_y + 1] > 0;
            const bool previous_column_present = patch_y > 0
                && patch_tiles[patch_x][patch_y - 1] > 0;
            const int present_neighbor_count =
                (previous_row_present ? 1 : 0)
                + (next_row_present ? 1 : 0)
                + (next_column_present ? 1 : 0)
                + (previous_column_present ? 1 : 0);
            // Isolated seeds keep their marker; every connected cell gets the
            // shared road-shape rule (the same one the road editor writes).
            if (present_neighbor_count == 0) continue;
            patch_tiles[patch_x][patch_y] = static_cast<int8_t>(
                road_shape_from_neighbors(previous_row_present,
                                          next_row_present,
                                          previous_column_present,
                                          next_column_present));
        }
    }

    for (int patch_x = 0; patch_x < patch_width; ++patch_x) {
        for (int patch_y = 0; patch_y < patch_height; ++patch_y) {
            if (patch_tiles[patch_x][patch_y] != 0) continue;
            bool can_fill_hole = true;
            const int8_t previous_row_tile = patch_x > 0
                ? patch_tiles[patch_x - 1][patch_y] : 0;
            const int8_t next_row_tile = patch_x < patch_width - 1
                ? patch_tiles[patch_x + 1][patch_y] : 0;
            const int8_t previous_column_tile = patch_y > 0
                ? patch_tiles[patch_x][patch_y - 1] : 0;
            const int8_t next_column_tile = patch_y < patch_height - 1
                ? patch_tiles[patch_x][patch_y + 1] : 0;
            if (previous_row_tile < road_tile::kStraightShape42
                && next_row_tile < road_tile::kStraightShape42
                && previous_column_tile < road_tile::kStraightShape42
                && next_column_tile < road_tile::kStraightShape42)
                can_fill_hole = false;
            if (previous_row_tile == road_tile::kJunctionShape44
                || next_row_tile == road_tile::kJunctionShape44
                || previous_column_tile == road_tile::kJunctionShape44
                || next_column_tile == road_tile::kJunctionShape44)
                can_fill_hole = false;
            if (previous_row_tile == road_tile::kCornerShape45
                || previous_row_tile == road_tile::kCornerShape46
                || previous_row_tile == road_tile::kStraightShape43)
                can_fill_hole = false;
            if (next_row_tile == road_tile::kCornerShape47
                || next_row_tile == road_tile::kCornerShape48
                || next_row_tile == road_tile::kStraightShape43)
                can_fill_hole = false;
            if (previous_column_tile == road_tile::kCornerShape46
                || previous_column_tile == road_tile::kCornerShape47
                || previous_column_tile == road_tile::kStraightShape42)
                can_fill_hole = false;
            if (next_column_tile == road_tile::kCornerShape45
                || next_column_tile == road_tile::kCornerShape48
                || next_column_tile == road_tile::kStraightShape42)
                can_fill_hole = false;
            if (previous_row_tile == road_tile::kJunctionShape44)
                patch_tiles[patch_x][patch_y] = road_tile::kStraightShape43;
            if (next_row_tile == road_tile::kJunctionShape44)
                patch_tiles[patch_x][patch_y] = road_tile::kStraightShape43;
            if (previous_column_tile == road_tile::kJunctionShape44)
                patch_tiles[patch_x][patch_y] = road_tile::kStraightShape42;
            if (next_column_tile == road_tile::kJunctionShape44)
                patch_tiles[patch_x][patch_y] = road_tile::kStraightShape42;
            if (!can_fill_hole) continue;
            patch_tiles[patch_x][patch_y] = game->world.random_city_tile(game->rng);
        }
    }

    int8_t target_elevation = terrain_edit::kUninitializedElevation;
    for (int patch_x = 0; patch_x < patch_width; ++patch_x) {
        for (int patch_y = 0; patch_y < patch_height; ++patch_y) {
            if (origin_x + patch_x >= game->world.terrain.mapWidth
                || origin_y + patch_y >= game->world.terrain.mapHeight
                || (patch_tiles[patch_x][patch_y] == 0
                    && (patch_tiles[patch_x][patch_y] != 0
                        || game->rng.next_int() % 3 >= 0)))
                continue;
            if (target_elevation == terrain_edit::kUninitializedElevation
                && (target_elevation = game->world.terrain.heightMap[
                        origin_x + patch_x][origin_y + patch_y]) < 0)
                target_elevation = 0;
            int64_t smoothing_start_time = midp_current_time_millis();
            while (target_elevation
                   != game->world.terrain.heightMap[origin_x + patch_x][origin_y + patch_y]) {
                game->smooth_tile_at(
                    origin_x + patch_x, origin_y + patch_y,
                    target_elevation
                        > game->world.terrain.heightMap[origin_x + patch_x][origin_y + patch_y]);
                if (midp_current_time_millis() - smoothing_start_time
                    <= terrain_patch::kSmoothingTimeoutMillis)
                    continue;
                game->build.trackBuildInProgress = true;
                return;
            }
            smoothing_start_time = midp_current_time_millis();
            while (target_elevation
                   != game->world.terrain.heightMap[origin_x + patch_x + 1][origin_y + patch_y]) {
                game->smooth_tile_at(
                    origin_x + patch_x + 1, origin_y + patch_y,
                    target_elevation
                        > game->world.terrain.heightMap[origin_x + patch_x + 1][origin_y + patch_y]);
                if (midp_current_time_millis() - smoothing_start_time
                    <= terrain_patch::kSmoothingTimeoutMillis)
                    continue;
                game->build.trackBuildInProgress = true;
                return;
            }
            smoothing_start_time = midp_current_time_millis();
            while (target_elevation
                   != game->world.terrain.heightMap[origin_x + patch_x][origin_y + patch_y + 1]) {
                game->smooth_tile_at(
                    origin_x + patch_x, origin_y + patch_y + 1,
                    target_elevation
                        > game->world.terrain.heightMap[origin_x + patch_x][origin_y + patch_y + 1]);
                if (midp_current_time_millis() - smoothing_start_time
                    <= terrain_patch::kSmoothingTimeoutMillis)
                    continue;
                game->build.trackBuildInProgress = true;
                return;
            }
            smoothing_start_time = midp_current_time_millis();
            while (target_elevation
                   != game->world.terrain.heightMap[origin_x + patch_x + 1][origin_y + patch_y + 1]) {
                game->smooth_tile_at(
                    origin_x + patch_x + 1, origin_y + patch_y + 1,
                    target_elevation
                        > game->world.terrain.heightMap[origin_x + patch_x + 1][origin_y + patch_y + 1]);
                if (midp_current_time_millis() - smoothing_start_time
                    <= terrain_patch::kSmoothingTimeoutMillis)
                    continue;
                game->build.trackBuildInProgress = true;
                return;
            }
        }
    }

    int32_t city_tile_count = 0;
    for (int patch_x = 0; patch_x < patch_width; ++patch_x)
        for (int patch_y = 0; patch_y < patch_height; ++patch_y)
            if (patch_tiles[patch_x][patch_y] >= tile::BUILT_FIRST
                && patch_tiles[patch_x][patch_y] <= tile::CITY_ZONE_LAST)
                ++city_tile_count;
    const int8_t train_slot = game->register_map_train(
        city_tile_count, origin_x + patch_width / 2,
        origin_y + patch_height / 2);
    for (int patch_x = 0; patch_x < patch_width; ++patch_x) {
        for (int patch_y = 0; patch_y < patch_height; ++patch_y) {
            if (origin_x + patch_x >= game->world.terrain.mapWidth
                || origin_y + patch_y >= game->world.terrain.mapHeight
                || patch_tiles[patch_x][patch_y] == 0)
                continue;
            game->world.terrain.tileMap[origin_x + patch_x][origin_y + patch_y] =
                patch_tiles[patch_x][patch_y];
            game->world.trackMap[origin_x + patch_x][origin_y + patch_y] =
                static_cast<int8_t>(terrain_patch::kCityOverlayFlag | train_slot);
        }
    }
}

void Game::place_terrain_feature(int32_t feature_id) {
    Game *game = this;
    int tile_x, tile_y;
    int feature_width, feature_height;
    int clear_check_x, clear_check_y;
    int8_t city_id;
    bool area_is_clear;
    if (game->build.trackBuildInProgress) return;
    const int64_t placement_start_time = midp_current_time_millis();
    do {
        area_is_clear = true;
        tile_x = abs(game->rng.next_int() % (game->world.terrain.mapWidth
                                             - feature_placement::kMapMargin))
                 + feature_placement::kMapOffset;
        tile_y = abs(game->rng.next_int() % (game->world.terrain.mapHeight
                                             - feature_placement::kMapMargin))
                 + feature_placement::kMapOffset;
        feature_width = game->rng.next_int() % 2 <= 0
            ? feature_placement::kLargeSize : feature_placement::kSmallSize;
        feature_height = game->rng.next_int() % 2 <= 0
            ? feature_placement::kLargeSize : feature_placement::kSmallSize;
        if (feature_id == terrain_feature::IndustryGroup2Destination) {
            feature_width = feature_placement::kSmallSize;
            feature_height = feature_placement::kSmallSize;
        }
        for (clear_check_x = tile_x - feature_placement::kClearanceRadius;
             clear_check_x < tile_x + feature_width + feature_placement::kClearanceRadius;
             ++clear_check_x) {
            for (clear_check_y = tile_y - feature_placement::kClearanceRadius;
                 clear_check_y < tile_y + feature_height + feature_placement::kClearanceRadius;
                 ++clear_check_y) {
                if (game->world.terrain.tileMap[clear_check_x][clear_check_y]
                    >= tile::BUILT_FIRST)
                    area_is_clear = false;
                if (game->world.terrain.tileElevation[clear_check_x][clear_check_y] != -1)
                    continue;
                area_is_clear = false;
            }
        }
        if (midp_current_time_millis() - placement_start_time
            <= feature_placement::kTimeoutMillis)
            continue;
        game->build.trackBuildInProgress = true;
        break;
    } while (!area_is_clear);
    if (game->build.trackBuildInProgress) return;
    game->track_build_step(tile_x, tile_y, feature_width, feature_height);
    city_id = game->add_city_entry(feature_id);
    switch (feature_id) {
        case terrain_feature::IndustryGroup1Destination:
            if (feature_width == feature_placement::kSmallSize
                && feature_height == feature_placement::kSmallSize) {
                game->world.place_track_tile(tile_x, tile_y,
                    terrain_feature_tile::IndustryGroup1DestinationC, city_id);
                game->world.place_track_tile(tile_x, tile_y + 1,
                    terrain_feature_tile::IndustryGroup1DestinationC, city_id);
                game->world.place_track_tile(tile_x + 1, tile_y,
                    terrain_feature_tile::IndustryGroup1DestinationB, city_id);
                game->world.place_track_tile(tile_x + 1, tile_y + 1,
                    terrain_feature_tile::IndustryGroup1DestinationA, city_id);
            }
            if ((feature_width == feature_placement::kLargeSize
                 && feature_height == feature_placement::kSmallSize)
                || (feature_width == feature_placement::kLargeSize
                    && feature_height == feature_placement::kLargeSize)) {
                game->world.place_track_tile(tile_x, tile_y,
                    terrain_feature_tile::IndustryGroup1DestinationC, city_id);
                game->world.place_track_tile(tile_x, tile_y + 1,
                    terrain_feature_tile::IndustryGroup1DestinationC, city_id);
                game->world.place_track_tile(tile_x + 1, tile_y,
                    terrain_feature_tile::IndustryGroup1DestinationB, city_id);
                game->world.place_track_tile(tile_x + 2, tile_y,
                    terrain_feature_tile::IndustryGroup1DestinationB, city_id);
                game->world.place_track_tile(tile_x + 1, tile_y + 1,
                    terrain_feature_tile::IndustryGroup1DestinationA, city_id);
                game->world.place_track_tile(tile_x + 2, tile_y + 1,
                    terrain_feature_tile::IndustryGroup1DestinationA, city_id);
            }
            if (feature_width != feature_placement::kSmallSize
                || feature_height != feature_placement::kLargeSize)
                break;
            game->world.place_track_tile(tile_x, tile_y,
                terrain_feature_tile::IndustryGroup1DestinationC, city_id);
            game->world.place_track_tile(tile_x + 1, tile_y,
                terrain_feature_tile::IndustryGroup1DestinationC, city_id);
            game->world.place_track_tile(tile_x, tile_y + 1,
                terrain_feature_tile::IndustryGroup1DestinationA, city_id);
            game->world.place_track_tile(tile_x + 1, tile_y + 1,
                terrain_feature_tile::IndustryGroup1DestinationB, city_id);
            game->world.place_track_tile(tile_x, tile_y + 2,
                terrain_feature_tile::IndustryGroup1DestinationA, city_id);
            game->world.place_track_tile(tile_x + 1, tile_y + 2,
                terrain_feature_tile::IndustryGroup1DestinationB, city_id);
            break;
        case terrain_feature::IndustryGroup2Production:
            if (feature_width == feature_placement::kSmallSize
                && feature_height == feature_placement::kSmallSize) {
                game->world.place_track_tile(tile_x, tile_y,
                    terrain_feature_tile::IndustryGroup2ProductionA, city_id);
                game->world.place_track_tile(tile_x, tile_y + 1,
                    terrain_feature_tile::IndustryGroup2ProductionA, city_id);
                game->world.place_track_tile(tile_x + 1, tile_y,
                    terrain_feature_tile::IndustryGroup2ProductionC, city_id);
                game->world.place_track_tile(tile_x + 1, tile_y + 1,
                    terrain_feature_tile::IndustryGroup2ProductionB, city_id);
            }
            if ((feature_width == feature_placement::kLargeSize
                 && feature_height == feature_placement::kSmallSize)
                || (feature_width == feature_placement::kLargeSize
                    && feature_height == feature_placement::kLargeSize)) {
                game->world.place_track_tile(tile_x, tile_y,
                    terrain_feature_tile::IndustryGroup2ProductionA, city_id);
                game->world.place_track_tile(tile_x, tile_y + 1,
                    terrain_feature_tile::IndustryGroup2ProductionA, city_id);
                game->world.place_track_tile(tile_x + 1, tile_y,
                    terrain_feature_tile::IndustryGroup2ProductionC, city_id);
                game->world.place_track_tile(tile_x + 2, tile_y,
                    terrain_feature_tile::IndustryGroup2ProductionC, city_id);
                game->world.place_track_tile(tile_x + 1, tile_y + 1,
                    terrain_feature_tile::IndustryGroup2ProductionB, city_id);
                game->world.place_track_tile(tile_x + 2, tile_y + 1,
                    terrain_feature_tile::IndustryGroup2ProductionB, city_id);
            }
            if (feature_width != feature_placement::kSmallSize
                || feature_height != feature_placement::kLargeSize)
                break;
            game->world.place_track_tile(tile_x, tile_y,
                terrain_feature_tile::IndustryGroup2ProductionA, city_id);
            game->world.place_track_tile(tile_x + 1, tile_y,
                terrain_feature_tile::IndustryGroup2ProductionA, city_id);
            game->world.place_track_tile(tile_x, tile_y + 1,
                terrain_feature_tile::IndustryGroup2ProductionC, city_id);
            game->world.place_track_tile(tile_x + 1, tile_y + 1,
                terrain_feature_tile::IndustryGroup2ProductionC, city_id);
            game->world.place_track_tile(tile_x, tile_y + 2,
                terrain_feature_tile::IndustryGroup2ProductionB, city_id);
            game->world.place_track_tile(tile_x + 1, tile_y + 2,
                terrain_feature_tile::IndustryGroup2ProductionB, city_id);
            break;
        case terrain_feature::IndustryGroup2Destination:
            if (feature_width != feature_placement::kSmallSize
                || feature_height != feature_placement::kSmallSize)
                break;
            game->world.place_track_tile(tile_x, tile_y,
                terrain_feature_tile::IndustryGroup2DestinationB
                    + abs(game->rng.next_int() % 2), city_id);
            game->world.place_track_tile(tile_x, tile_y + 1,
                terrain_feature_tile::IndustryGroup2DestinationA, city_id);
            game->world.place_track_tile(tile_x + 1, tile_y,
                terrain_feature_tile::IndustryGroup2DestinationB
                    + abs(game->rng.next_int() % 2), city_id);
            game->world.place_track_tile(tile_x + 1, tile_y + 1,
                terrain_feature_tile::IndustryGroup2DestinationB
                    + abs(game->rng.next_int() % 2), city_id);
            break;
        case terrain_feature::IndustryGroup3Production:
            for (int feature_x = tile_x;
                 feature_x < tile_x + feature_width; ++feature_x)
                for (int feature_y = tile_y;
                     feature_y < tile_y + feature_height; ++feature_y)
                    game->world.place_track_tile(feature_x, feature_y,
                        terrain_feature_tile::IndustryGroup3ProductionA
                            + abs(game->rng.next_int() % 3), city_id);
            break;
        case terrain_feature::IndustryGroup3Destination:
            if (feature_width == feature_placement::kSmallSize
                && feature_height == feature_placement::kSmallSize) {
                game->world.place_track_tile(tile_x, tile_y,
                    terrain_feature_tile::IndustryGroup3DestinationA, city_id);
                game->world.place_track_tile(tile_x, tile_y + 1,
                    terrain_feature_tile::IndustryGroup3DestinationA, city_id);
                game->world.place_track_tile(tile_x + 1, tile_y,
                    terrain_feature_tile::IndustryGroup3DestinationC, city_id);
                game->world.place_track_tile(tile_x + 1, tile_y + 1,
                    terrain_feature_tile::IndustryGroup3DestinationB, city_id);
            }
            if ((feature_width == feature_placement::kLargeSize
                 && feature_height == feature_placement::kSmallSize)
                || (feature_width == feature_placement::kLargeSize
                    && feature_height == feature_placement::kLargeSize)) {
                game->world.place_track_tile(tile_x, tile_y,
                    terrain_feature_tile::IndustryGroup3DestinationA, city_id);
                game->world.place_track_tile(tile_x, tile_y + 1,
                    terrain_feature_tile::IndustryGroup3DestinationA, city_id);
                game->world.place_track_tile(tile_x + 1, tile_y,
                    terrain_feature_tile::IndustryGroup3DestinationC, city_id);
                game->world.place_track_tile(tile_x + 2, tile_y,
                    terrain_feature_tile::IndustryGroup3DestinationC, city_id);
                game->world.place_track_tile(tile_x + 1, tile_y + 1,
                    terrain_feature_tile::IndustryGroup3DestinationA, city_id);
                game->world.place_track_tile(tile_x + 2, tile_y + 1,
                    terrain_feature_tile::IndustryGroup3DestinationB, city_id);
            }
            if (feature_width != feature_placement::kSmallSize
                || feature_height != feature_placement::kLargeSize)
                break;
            game->world.place_track_tile(tile_x, tile_y,
                terrain_feature_tile::IndustryGroup3DestinationA, city_id);
            game->world.place_track_tile(tile_x + 1, tile_y,
                terrain_feature_tile::IndustryGroup3DestinationB, city_id);
            game->world.place_track_tile(tile_x, tile_y + 1,
                terrain_feature_tile::IndustryGroup3DestinationA, city_id);
            game->world.place_track_tile(tile_x + 1, tile_y + 1,
                terrain_feature_tile::IndustryGroup3DestinationC, city_id);
            game->world.place_track_tile(tile_x, tile_y + 2,
                terrain_feature_tile::IndustryGroup3DestinationA, city_id);
            game->world.place_track_tile(tile_x + 1, tile_y + 2,
                terrain_feature_tile::IndustryGroup3DestinationC, city_id);
            break;
        case terrain_feature::IndustryGroup1Production:
            if (feature_width == feature_placement::kSmallSize
                && feature_height == feature_placement::kSmallSize) {
                game->world.place_track_tile(tile_x, tile_y,
                    terrain_feature_tile::IndustryGroup1ProductionA, city_id);
                game->world.place_track_tile(tile_x + 1, tile_y,
                    terrain_feature_tile::IndustryGroup1ProductionB, city_id);
                game->world.place_track_tile(tile_x, tile_y + 1,
                    terrain_feature_tile::IndustryGroup1ProductionC, city_id);
            }
            if ((feature_width == feature_placement::kLargeSize
                 && feature_height == feature_placement::kSmallSize)
                || (feature_width == feature_placement::kLargeSize
                    && feature_height == feature_placement::kLargeSize)) {
                game->world.place_track_tile(tile_x, tile_y,
                    terrain_feature_tile::IndustryGroup1ProductionA, city_id);
                game->world.place_track_tile(tile_x + 1, tile_y,
                    terrain_feature_tile::IndustryGroup1ProductionB, city_id);
                game->world.place_track_tile(tile_x + 2, tile_y,
                    terrain_feature_tile::IndustryGroup1ProductionC, city_id);
                game->world.place_track_tile(tile_x, tile_y + 1,
                    terrain_feature_tile::IndustryGroup1ProductionD, city_id);
            }
            if (feature_width != feature_placement::kSmallSize
                || feature_height != feature_placement::kLargeSize)
                break;
            game->world.place_track_tile(tile_x, tile_y,
                terrain_feature_tile::IndustryGroup1ProductionA, city_id);
            game->world.place_track_tile(tile_x, tile_y + 1,
                terrain_feature_tile::IndustryGroup1ProductionB, city_id);
            game->world.place_track_tile(tile_x, tile_y + 2,
                terrain_feature_tile::IndustryGroup1ProductionC, city_id);
            game->world.place_track_tile(tile_x + 1, tile_y,
                terrain_feature_tile::IndustryGroup1ProductionD, city_id);
            break;
    }
}

void Game::initialize_map_generator() {
        Game* game = this;
        game->world.resize(game->world.terrain.mapWidth, game->world.terrain.mapHeight);
        game->world.initialIndustryPlaced = false;
        game->world.mapGenStage = mapgen::STAGE_INIT;
        game->world.tilesGeneratedInStage = 0;
        game->world.totalTilesToGenerate = game->world.terrainRaiseStepCount + game->world.terrainLowerStepCount + game->world.mapGenColumns * (game->world.mapGenColumns - 1)
            + game->world.mapGenRows + game->world.featureStage0Count
            + game->world.featureStage1Count + game->world.featureStage2Count
            + game->world.featureStage3Count + game->world.featureStage4Count
            + game->world.featureStage5Count + 1;
        game->world.mapGenTilesDone = 0;
}

void Game::advance_map_generation_stage() {
        Game* game = this;
        if (game->world.mapGenStage == mapgen::STAGE_INIT) { game->world.mapGenStage = mapgen::STAGE_RAISE_TERRAIN; game->world.tilesGeneratedInStage = 0; }
        if (game->world.mapGenStage == mapgen::STAGE_RAISE_TERRAIN && game->world.tilesGeneratedInStage == game->world.terrainRaiseStepCount) { game->world.mapGenStage = mapgen::STAGE_LOWER_TERRAIN; game->world.tilesGeneratedInStage = 0; game->world.terrain.rebuild(game->build, false); }
        if (game->world.mapGenStage == mapgen::STAGE_LOWER_TERRAIN && game->world.tilesGeneratedInStage == game->world.terrainLowerStepCount) { game->world.mapGenStage = mapgen::STAGE_PLACE_PATCHES; game->world.tilesGeneratedInStage = 0; game->world.terrain.rebuild(game->build, false); }
        if (game->world.mapGenStage == mapgen::STAGE_PLACE_PATCHES && game->world.tilesGeneratedInStage == game->world.mapGenColumns * (game->world.mapGenColumns - 1) + game->world.mapGenRows) { game->world.mapGenStage = mapgen::STAGE_FEATURE_0; game->world.tilesGeneratedInStage = 0; }
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_0 && game->world.tilesGeneratedInStage == game->world.featureStage0Count) { game->world.mapGenStage = mapgen::STAGE_FEATURE_1; game->world.tilesGeneratedInStage = 0; }
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_1 && game->world.tilesGeneratedInStage == game->world.featureStage1Count) { game->world.mapGenStage = mapgen::STAGE_FEATURE_2; game->world.tilesGeneratedInStage = 0; }
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_2 && game->world.tilesGeneratedInStage == game->world.featureStage2Count) { game->world.mapGenStage = mapgen::STAGE_FEATURE_3; game->world.tilesGeneratedInStage = 0; }
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_3 && game->world.tilesGeneratedInStage == game->world.featureStage3Count) { game->world.mapGenStage = mapgen::STAGE_FEATURE_4; game->world.tilesGeneratedInStage = 0; }
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_4 && game->world.tilesGeneratedInStage == game->world.featureStage4Count) { game->world.mapGenStage = mapgen::STAGE_FEATURE_5; game->world.tilesGeneratedInStage = 0; }
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_5 && game->world.tilesGeneratedInStage == game->world.featureStage5Count) { game->world.mapGenStage = mapgen::STAGE_SCATTER_OBSTACLES; game->world.tilesGeneratedInStage = 0; }
}

void Game::generate_terrain_height_step() {
        Game* game = this;
        if (game->world.mapGenStage == mapgen::STAGE_RAISE_TERRAIN) {
            int center_x = abs(game->rng.next_int() % game->world.terrain.mapWidth);
            int center_y = abs(game->rng.next_int() % game->world.terrain.mapHeight);
            int width = abs(game->rng.next_int()
                            % terrain_height_generation::kAreaSizeRandomRange)
                        + terrain_height_generation::kMinimumAreaSize;
            int height = abs(game->rng.next_int()
                             % terrain_height_generation::kAreaSizeRandomRange)
                         + terrain_height_generation::kMinimumAreaSize;
            int elevation_step = abs(
                game->rng.next_int()
                % terrain_height_generation::kElevationStepRandomRange)
                + terrain_height_generation::kFirstElevationStep;
            game->raise_terrain_area(center_x, center_y, width, height,
                                elevation_step);
        }
        if (game->world.mapGenStage == mapgen::STAGE_LOWER_TERRAIN) {
            int center_x = abs(game->rng.next_int() % game->world.terrain.mapWidth);
            int center_y = abs(game->rng.next_int() % game->world.terrain.mapHeight);
            int width = abs(game->rng.next_int()
                            % terrain_height_generation::kAreaSizeRandomRange)
                        + terrain_height_generation::kMinimumAreaSize;
            int height = abs(game->rng.next_int()
                             % terrain_height_generation::kAreaSizeRandomRange)
                         + terrain_height_generation::kMinimumAreaSize;
            game->lower_terrain_area(center_x, center_y, width, height,
                                     terrain_height_generation::kLoweringDelta);
        }
}

void Game::generate_terrain_patch_step() {
        Game* game = this;
        game->build.trackBuildInProgress = false;
        if (game->world.mapGenStage == mapgen::STAGE_PLACE_PATCHES) {
            for (int column = 0; column < game->world.mapGenColumns; ++column) {
                bool done = false;
                for (int row = 0; row < game->world.mapGenRows && !done; ++row) {
                    if (game->world.tilesGeneratedInStage != game->world.mapGenColumns * column + row) continue;
                    int cellWidth = game->world.terrain.mapWidth / game->world.mapGenColumns;
                    int cellHeight = game->world.terrain.mapHeight / game->world.mapGenRows;
                    int patchOffsetRangeX = cellWidth / 2;
                    int patchOffsetRangeY = cellHeight / 2;
                    if (game->scenario.freeGameTownLevel
                        == free_game_town_level::Megapolis) {
                        const int maximumPatchSize =
                            game->world.terrainPatchBaseSize
                            + terrain_patch::kPatchSizeRandomRange - 1;
                        patchOffsetRangeX = cellWidth - maximumPatchSize + 1;
                        patchOffsetRangeY = cellHeight - maximumPatchSize + 1;
                        if (patchOffsetRangeX < 1) patchOffsetRangeX = 1;
                        if (patchOffsetRangeY < 1) patchOffsetRangeY = 1;
                    }
                    int patchX = column * cellWidth
                        + abs(game->rng.next_int() % patchOffsetRangeX);
                    int patchY = row * cellHeight
                        + abs(game->rng.next_int() % patchOffsetRangeY);
                    game->place_terrain_patch(patchX, patchY);
                    done = true;
                }
                if (done) break;
            }
        }
}

void Game::generate_terrain_feature_step() {
        Game* game = this;
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_0) game->place_terrain_feature(0);
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_1) game->place_terrain_feature(1);
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_2) game->place_terrain_feature(2);
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_3) game->place_terrain_feature(3);
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_4) game->place_terrain_feature(4);
        if (game->world.mapGenStage == mapgen::STAGE_FEATURE_5) game->place_terrain_feature(5);
        if (game->build.trackBuildInProgress) game->stateWorkStep = 0;
}

void Game::scatter_initial_obstacles() {
        Game* game = this;
        if (game->world.mapGenStage == mapgen::STAGE_SCATTER_OBSTACLES) {
            for (int obstacle = 0; obstacle < game->world.initialObstacleCount; ++obstacle) {
                int mapX = abs(game->rng.next_int() % game->world.terrain.mapWidth);
                int mapY = abs(game->rng.next_int() % game->world.terrain.mapHeight);
                if (game->world.terrain.tileMap[mapX][mapY] != 0 || game->world.terrain.tileElevation[mapX][mapY] < 0) continue;
                game->world.terrain.tileMap[mapX][mapY] = (int8_t)(tile::SCENERY_FIRST + abs(game->rng.next_int() % 5));
            }
            game->world.initialIndustryPlaced = true;
        }
}

void Game::generate_one_map_step() {
        Game* game = this;
        game->advance_map_generation_stage();
        game->generate_terrain_height_step();
        game->generate_terrain_patch_step();
        game->generate_terrain_feature_step();
        game->scatter_initial_obstacles();
        ++game->world.tilesGeneratedInStage;
        ++game->world.mapGenTilesDone;
}

// Set up the map generator (mode 0), or run one ordered generation step (mode 1).
void Game::run_map_generation_step(int32_t mode) {
    Game* game = this;
    if (mode == 0) game->initialize_map_generator();
    if (mode == 1) game->generate_one_map_step();
}

void World::pick_city_site(JavaRandom& random) {
    const int candidate_x = abs(random.next_int() % terrain.mapWidth);
    const int candidate_y = abs(random.next_int() % terrain.mapHeight);
    industryCategory = -1;
    const int candidate_tile = terrain.tileMap[candidate_x][candidate_y];
    if (candidate_tile != 0
        && (candidate_tile < tile::SCENERY_FIRST
            || candidate_tile > tile::SCENERY_LAST))
        return;
    int min_candidate_x = candidate_x - 2;
    int min_candidate_y = candidate_y - 2;
    int max_candidate_x = candidate_x + 2;
    int max_candidate_y = candidate_y + 2;
    if (min_candidate_x < 0) min_candidate_x = 0;
    if (min_candidate_y < 0) min_candidate_y = 0;
    if (max_candidate_x >= terrain.mapWidth) max_candidate_x = terrain.mapWidth - 1;
    if (max_candidate_y >= terrain.mapHeight) max_candidate_y = terrain.mapHeight - 1;
    for (int near_x = min_candidate_x;
         near_x <= max_candidate_x && industryCategory <= 0; ++near_x) {
        for (int near_y = min_candidate_y;
             near_y <= max_candidate_y && industryCategory <= 0; ++near_y) {
            const int nearby_tile = terrain.tileMap[near_x][near_y];
            if (nearby_tile >= tile::BUILT_FIRST
                && nearby_tile <= tile::CITY_ZONE_LAST)
                industryCategory = 0;
            if (nearby_tile >= industry_tile::Group1ProductionFirst
                && nearby_tile <= industry_tile::Group1ProductionLast)
                industryCategory = 1;
            if (nearby_tile >= industry_tile::Group2ProductionFirst
                && nearby_tile <= industry_tile::Group2ProductionLast)
                industryCategory = 2;
            if (nearby_tile >= industry_tile::Group3ProductionFirst
                && nearby_tile <= industry_tile::Group3ProductionLast)
                industryCategory = 3;
        }
    }
    if (industryCategory >= 0) {
        cityStationX = candidate_x;
        cityStationY = candidate_y;
    }
}

void World::pick_industry_site(JavaRandom& random) {
    const int candidate_x = abs(random.next_int() % terrain.mapWidth);
    const int candidate_y = abs(random.next_int() % terrain.mapHeight);
    industrySiteFound = false;
    const int candidate_tile = terrain.tileMap[candidate_x][candidate_y];
    if (candidate_tile != 0
        && (candidate_tile < tile::SCENERY_FIRST
            || candidate_tile > tile::SCENERY_LAST))
        return;
    int min_candidate_x = candidate_x - 2;
    int min_candidate_y = candidate_y - 2;
    int max_candidate_x = candidate_x + 2;
    int max_candidate_y = candidate_y + 2;
    if (min_candidate_x < 0) min_candidate_x = 0;
    if (min_candidate_y < 0) min_candidate_y = 0;
    if (max_candidate_x >= terrain.mapWidth) max_candidate_x = terrain.mapWidth - 1;
    if (max_candidate_y >= terrain.mapHeight) max_candidate_y = terrain.mapHeight - 1;
    for (int near_x = min_candidate_x;
         near_x <= max_candidate_x && !industrySiteFound; ++near_x) {
        for (int near_y = min_candidate_y;
             near_y <= max_candidate_y && !industrySiteFound; ++near_y) {
            const int nearby_tile = terrain.tileMap[near_x][near_y];
            if (nearby_tile >= tile::BUILT_FIRST
                && nearby_tile <= tile::CITY_ZONE_LAST
                && industryCategory == 0)
                industrySiteFound = true;
            if (nearby_tile >= industry_tile::Group3DestinationFirst
                && nearby_tile <= industry_tile::Group3DestinationLast
                && industryCategory == 1)
                industrySiteFound = true;
            if (nearby_tile >= industry_tile::Group1DestinationFirst
                && nearby_tile <= industry_tile::Group1DestinationLast
                && industryCategory == 2)
                industrySiteFound = true;
            if (nearby_tile >= industry_tile::Group2DestinationFirst
                && nearby_tile <= industry_tile::Group2DestinationLast
                && industryCategory == 3)
                industrySiteFound = true;
        }
    }
    if (industrySiteFound) {
        industryStationX = candidate_x;
        industryTileY = candidate_y;
    }
}

void Game::spawn_train_at(int32_t x, int32_t y) {
    Game* game = this;
    int engine_type = game->scenario.currentEra / 3;
    int wagon_count = game->build.selectionCost / 8000 + 1;
    if (wagon_count > kWagonOptionCount) wagon_count = kWagonOptionCount;
    int cargo_type = wagon::PassengerMailCar;
    if (game->world.industryCategory == 1) cargo_type = wagon::IndustryGroup1Car;
    if (game->world.industryCategory == 2) cargo_type = wagon::IndustryGroup2CarA;
    if (game->world.industryCategory == 3) cargo_type = wagon::IndustryGroup3CarA;
    int train = game->fleet.allocate_train_car(
        x, y, engine_type, game->scenario.currentEra,
        game->world.terrain.tileMap[x][y] == tile::VERTICAL_STATION ? 0 : 2,
        8 - game->scenario.currentEra);
    game->renderer.lastViewportCenterX = -1;
    for (int wagon = 0; wagon < wagon_count; ++wagon) {
        train = game->fleet.append_car(train, cargo_type);
        game->renderer.lastViewportCenterX = -1;
    }
}

static bool can_extend_station_over(const World& world, int x, int y) {
    int tile = world.terrain.tileMap[x][y];
    return tile == 0 || (tile >= tile::SCENERY_FIRST && tile <= tile::SCENERY_LAST);
}

void World::auto_extend_track_station(int32_t x, int32_t y,
                                      int32_t track_piece_code) {
    bool track_left = x > 0 && trackMap[x - 1][y] == track::PRESENT;
    bool track_right = x < terrain.mapWidth - 1 && trackMap[x + 1][y] == track::PRESENT;
    bool track_up = y > 0 && trackMap[x][y - 1] == track::PRESENT;
    bool track_down = y < terrain.mapHeight - 1 && trackMap[x][y + 1] == track::PRESENT;
    if (track_left || track_right) {
        terrain.tileMap[x][y] = tile::HORIZONTAL_STATION;
        trackMap[x][y] = (int8_t)track_piece_code;
        if (track_right && x > 1
            && can_extend_station_over(*this, x - 1, y)
            && can_extend_station_over(*this, x - 2, y)) {
            terrain.tileMap[x][y] = tile::HORIZONTAL_TERMINAL;
            terrain.tileMap[x - 1][y] = tile::HORIZONTAL_STATION;
            terrain.tileMap[x - 2][y] = tile::HORIZONTAL_TERMINAL;
            trackMap[x - 1][y] = (int8_t)track_piece_code;
            trackMap[x - 2][y] = (int8_t)track_piece_code;
        }
        if (track_left && x < terrain.mapWidth - 2
            && can_extend_station_over(*this, x + 1, y)
            && can_extend_station_over(*this, x + 2, y)) {
            terrain.tileMap[x][y] = tile::HORIZONTAL_TERMINAL;
            terrain.tileMap[x + 1][y] = tile::HORIZONTAL_STATION;
            terrain.tileMap[x + 2][y] = tile::HORIZONTAL_TERMINAL;
            trackMap[x + 1][y] = (int8_t)track_piece_code;
            trackMap[x + 2][y] = (int8_t)track_piece_code;
        }
    }
    if (track_up || track_down) {
        terrain.tileMap[x][y] = tile::VERTICAL_STATION;
        trackMap[x][y] = (int8_t)track_piece_code;
        if (track_up && y > 1
            && can_extend_station_over(*this, x, y - 1)
            && can_extend_station_over(*this, x, y - 2)) {
            terrain.tileMap[x][y] = tile::VERTICAL_TERMINAL;
            terrain.tileMap[x][y - 1] = tile::VERTICAL_STATION;
            terrain.tileMap[x][y - 2] = tile::VERTICAL_TERMINAL;
            trackMap[x][y - 1] = (int8_t)track_piece_code;
            trackMap[x][y - 2] = (int8_t)track_piece_code;
        }
        // The original bytecode tests track_up here too (not track_down).
        if (track_up && y < terrain.mapHeight - 2
            && can_extend_station_over(*this, x, y + 1)
            && can_extend_station_over(*this, x, y + 2)) {
            terrain.tileMap[x][y] = tile::VERTICAL_TERMINAL;
            terrain.tileMap[x][y + 1] = tile::VERTICAL_STATION;
            terrain.tileMap[x][y + 2] = tile::VERTICAL_TERMINAL;
            trackMap[x][y + 1] = (int8_t)track_piece_code;
            trackMap[x][y + 2] = (int8_t)track_piece_code;
        }
    }
}

void Game::place_cities_and_industry() {
    Game* game = this;
    int city_attempts = 0;
    int industry_attempts = 0;
    do {
        game->world.pick_city_site(game->rng);
    } while (++city_attempts != feature_placement::kMaxPlacementAttempts
             && game->world.industryCategory == -1);
    if (city_attempts >= feature_placement::kMaxPlacementAttempts) return;
    do {
        game->world.pick_industry_site(game->rng);
    } while (++industry_attempts != feature_placement::kMaxPlacementAttempts
             && !game->world.industrySiteFound);
    if (industry_attempts >= feature_placement::kMaxPlacementAttempts) return;

    game->pathfinder.anchorX = game->world.cityStationX;
    game->pathfinder.anchorY = game->world.cityStationY;
    game->pathfinder.targetX = game->world.industryStationX;
    game->pathfinder.targetY = game->world.industryTileY;
    if (!game->pathfinder.try_build_path(game->world, game->build,
                                         game->economy)) return;
    game->world.auto_extend_track_station(
        game->world.cityStationX, game->world.cityStationY,
        game->build.nextTrackPieceCode);
    game->world.auto_extend_track_station(
        game->world.industryStationX, game->world.industryTileY,
        game->build.nextTrackPieceCode);
    spawn_train_at(game->world.cityStationX, game->world.cityStationY);
    game->set_track_owner_code(game->build.nextTrackPieceCode - track::PRESENT);
    ++game->build.nextTrackPieceCode;
    if (game->build.nextTrackPieceCode == track::AI_PIECE_WRAP) game->build.nextTrackPieceCode = track::AI_PIECE_FIRST;
}
