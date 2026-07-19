// Isometric world drawing.
// Draws terrain into the offscreen backbuffer (only when the viewport moved),
// blits it, then overlays dynamic tiles, trains, selections and the build menu.
#include "game.h"
#include "support/numeric.h"
#include <vector>

namespace {
namespace construction_tile_sprite {
constexpr int32_t kFirstCode = track_turn_tile::VariantA;
constexpr int32_t kLastYOffsetCode = track_turn_tile::VariantD;
constexpr int32_t kLastCode = construction_tile::kLast;
constexpr int32_t kExtraYOffsetVariantA = construction_tile::HorizontalB;
constexpr int32_t kExtraYOffsetVariantB = construction_tile::VerticalC;
constexpr int32_t kExtraYOffset = 4;
}  // namespace construction_tile_sprite

namespace resource_animation_tile {
constexpr int32_t kFirstCode = 65;
constexpr int32_t kLastCode = 68;
constexpr int32_t kWrapCode = 69;
}  // namespace resource_animation_tile

namespace industry_animation_tile {
constexpr int32_t kGroup2ProductionFrameFirst =
    industry_tile::Group2ProductionFirst + 1;
constexpr int32_t kGroup2ProductionFrameLast =
    industry_tile::Group2ProductionLast;
constexpr int32_t kGroup1DestinationMarker =
    industry_tile::Group1DestinationFirst;
constexpr int32_t kGroup3DestinationMarker =
    industry_tile::Group3DestinationLast;
constexpr int32_t kGroup2ProductionSpriteIndex = 53;
constexpr int32_t kGroup2ProductionFrameSpriteBase = 243;
constexpr int32_t kGroup1DestinationFrameSpriteBase = 240;
constexpr int32_t kGroup3DestinationFrameSpriteBase = 246;
constexpr int32_t kStaticFallbackSpriteIndex = 41;
}  // namespace industry_animation_tile

namespace map_render_geometry {
constexpr int32_t kTopStripHeight = 15;
constexpr int32_t kTileWidth = map_geometry::kTileWidthPixels;
constexpr int32_t kTileHalfWidth = map_geometry::kTileHalfWidthPixels;
constexpr int32_t kElevationScale = map_geometry::kElevationPixelScale;
constexpr int32_t kMapOriginTileSpan = 2 * kTileWidth;
constexpr int32_t kTileImageAnchorFlags = 33;
constexpr int32_t kGroundSpriteIndex = 1;
constexpr int32_t kRaisedTerrainSpriteIndex = 2;
constexpr int32_t kConstructionProgressSpriteBase = 250;
constexpr int32_t kGroup1DestinationOffsetX = 12;
constexpr int32_t kGroup1DestinationOffsetY = -28;
constexpr int32_t kGroup3DestinationOffsetX = 3;
constexpr int32_t kGroup3DestinationOffsetY = -39;
constexpr int32_t kConstructionOverlayOffsetY = -3;
}  // namespace map_render_geometry

constexpr int32_t kTileOverlayFlag = 0x80;
constexpr int32_t kVisibleTrainMarker = 1;
constexpr uint8_t kRoadVehicleMarker = 1;
constexpr uint8_t kRoadShelterMarker = 2;
constexpr int32_t kNoConstructionCountdown = -1;
}  // namespace

void Game::redraw_static_map_if_needed(int32_t origin_x, int32_t origin_y) {
    Game *game = this;
    if (game->renderer.camera.viewportCenterX == game->renderer.lastViewportCenterX
        && game->renderer.camera.viewportCenterY == game->renderer.viewportCenterYCached)
        return;

    Surface *backbuffer = game->renderer.backbuffer_surface;
    midp_set_clip(backbuffer, 0, map_render_geometry::kTopStripHeight,
                  game->renderer.canvasWidth,
                  game->renderer.gameplayViewportHeight);
    midp_set_color(backbuffer, 0);
    midp_fill_rect(backbuffer, 0, map_render_geometry::kTopStripHeight,
                   game->renderer.canvasWidth,
                   game->renderer.gameplayViewportHeight);
    game->renderer.visibleTrainCount = 0;
    game->renderer.viewportX0 = 2 * game->world.terrain.mapWidth;
    game->renderer.viewportY0 = 2 * game->world.terrain.mapHeight;
    game->renderer.viewportX1 = -game->world.terrain.mapWidth;
    game->renderer.viewportY1 = -game->world.terrain.mapWidth;
    for (int draw_index = 0;
         draw_index < game->assets.numActiveTileAnimations; ++draw_index) {
        int map_x = origin_x + game->assets.tileOffsetX[draw_index];
        int map_y = origin_y + game->assets.tileOffsetY[draw_index];
        if (map_x >= 0 && map_x < game->world.terrain.mapWidth
            && map_y >= 0 && map_y < game->world.terrain.mapHeight) {
            int tile_code =
                unsigned_byte(game->world.terrain.tileMap[map_x][map_y]);
            if (tile_code >= kTileOverlayFlag)
                tile_code -= kTileOverlayFlag;
            int draw_x = map_y * map_render_geometry::kTileWidth
                         + map_x * map_render_geometry::kTileWidth
                         + map_render_geometry::kTileWidth
                         - game->renderer.camera.viewportCenterX;
            int draw_y = game->world.terrain.mapHalfWidth
                         - map_x * map_render_geometry::kTileHalfWidth
                         + map_y * map_render_geometry::kTileHalfWidth
                         + map_render_geometry::kTileWidth
                         - game->renderer.camera.viewportCenterY
                         - map_render_geometry::kElevationScale
                           * game->world.terrain.tileElevation[map_x][map_y]
                         + map_render_geometry::kTopStripHeight;
            if (!((tile_code != 0 && tile_code <= tile::SLOPE_LAST)
                  || (tile_code >= construction_tile_sprite::kFirstCode
                      && tile_code <= construction_tile_sprite::kLastCode))) {
                if (game->world.terrain.tileElevation[map_x][map_y] > -1) {
                    if ((tile_code >= road_tile::kFirstShape
                         && tile_code <= road_tile::kLastShape)
                        || (tile_code >= tile::TRACK_FIRST
                            && tile_code <= tile::CROSSING))
                        midp_draw_image(backbuffer, game->assets.tile_sprites[game->assets.tileSpriteIndex[tile_code]].get(), draw_x, draw_y,
                                        map_render_geometry::kTileImageAnchorFlags);
                    else
                        midp_draw_image(backbuffer, game->assets.tile_sprites[game->assets.tileSpriteIndex[map_render_geometry::kGroundSpriteIndex]].get(),
                                        draw_x, draw_y,
                                        map_render_geometry::kTileImageAnchorFlags);
                } else {
                    midp_draw_image(backbuffer, game->assets.tile_sprites[game->assets.tileSpriteIndex[map_render_geometry::kRaisedTerrainSpriteIndex]].get(),
                                    draw_x, draw_y,
                                    map_render_geometry::kTileImageAnchorFlags);
                }
            }
            if (tile_code >= tile::SLOPE_FIRST
                && tile_code <= tile::SLOPE_LAST) {
                midp_draw_image(backbuffer, game->assets.tile_sprites[game->assets.tileSpriteIndex[tile_code]].get(),
                                draw_x, draw_y + game->assets.tileSpriteYOffsetByCode[tile_code - tile::SLOPE_FIRST],
                                map_render_geometry::kTileImageAnchorFlags);
            }
            if (tile_code >= construction_tile_sprite::kFirstCode
                && tile_code <= construction_tile_sprite::kLastYOffsetCode) {
                midp_draw_image(backbuffer, game->assets.tile_sprites[game->assets.tileSpriteIndex[tile_code]].get(),
                                draw_x, draw_y + game->assets.raised_track_y_offset(tile_code),
                                map_render_geometry::kTileImageAnchorFlags);
            }
            if (tile_code >= construction_tile_sprite::kLastYOffsetCode
                && tile_code <= construction_tile_sprite::kLastCode) {
                int y_offset = 0;
                if (tile_code == construction_tile_sprite::kExtraYOffsetVariantA
                    || tile_code == construction_tile_sprite::kExtraYOffsetVariantB)
                    y_offset = construction_tile_sprite::kExtraYOffset;
                midp_draw_image(backbuffer, game->assets.tile_sprites[game->assets.tileSpriteIndex[tile_code]].get(),
                                draw_x, draw_y + y_offset,
                                map_render_geometry::kTileImageAnchorFlags);
            }
        }
        if (map_x > game->renderer.viewportX1) game->renderer.viewportX1 = map_x;
        if (map_y > game->renderer.viewportY1) game->renderer.viewportY1 = map_y;
        if (map_x < game->renderer.viewportX0) game->renderer.viewportX0 = map_x;
        if (map_y < game->renderer.viewportY0) game->renderer.viewportY0 = map_y;
    }
    if (game->renderer.viewportX0 < 0) game->renderer.viewportX0 = 0;
    if (game->renderer.viewportY0 < 0) game->renderer.viewportY0 = 0;
    if (game->renderer.viewportX1 >= game->world.terrain.mapWidth) game->renderer.viewportX1 = game->world.terrain.mapWidth - 1;
    if (game->renderer.viewportY1 >= game->world.terrain.mapHeight) game->renderer.viewportY1 = game->world.terrain.mapHeight - 1;
    game->renderer.lastViewportCenterX = game->renderer.camera.viewportCenterX;
    game->renderer.viewportCenterYCached = game->renderer.camera.viewportCenterY;
    midp_set_clip(backbuffer, 0, 0, game->renderer.canvasWidth, game->renderer.canvasHeight);
}

void Game::render_map(Surface& surface) {
    Game* game = this;
    Surface* graphics = &surface;
    const int origin_x = game->renderer.camera.viewportCenterX
        - 2 * game->renderer.camera.viewportCenterY
        + map_render_geometry::kTileWidth
        + 2 * game->world.terrain.mapHalfWidth;
    const int origin_y = game->renderer.camera.viewportCenterX
        + 2 * game->renderer.camera.viewportCenterY
        - map_render_geometry::kTileWidth
        - 2 * game->world.terrain.mapHalfWidth;
    const int origin_tile_x = origin_x / map_render_geometry::kMapOriginTileSpan;
    const int origin_tile_y = origin_y / map_render_geometry::kMapOriginTileSpan;
    game->redraw_static_map_if_needed(origin_tile_x, origin_tile_y);
    game->paint_main(*graphics);
    midp_draw_image(graphics, game->renderer.backbuffer.get(), 0, 0, 20);
    midp_set_clip(graphics, 0, map_render_geometry::kTopStripHeight,
                  game->renderer.canvasWidth,
                  game->renderer.gameplayViewportHeight);
    game->draw_build_menu_cursor(*graphics);
    int visible_width = game->renderer.viewportX1
                      - game->renderer.viewportX0 + 1;
    int visible_height = game->renderer.viewportY1
                       - game->renderer.viewportY0 + 1;
    if (visible_width < 1) visible_width = 1;
    if (visible_height < 1) visible_height = 1;
    std::vector<uint8_t> visible_train_tiles(
        static_cast<std::size_t>(visible_width * visible_height));
    game->renderer.visibleTrainCount = 0;
    for (int train_slot = 0; train_slot < kTrainSlotCount; ++train_slot) {
        if (!train_flag::isUsed(game->fleet.trainCars[train_slot].flags))
            continue;
        const int train_x = unsigned_byte(game->fleet.trainCars[train_slot].x);
        const int train_y = unsigned_byte(game->fleet.trainCars[train_slot].y);
        if (train_x < game->renderer.viewportX0
            || train_y < game->renderer.viewportY0
            || train_x > game->renderer.viewportX1
            || train_y > game->renderer.viewportY1)
            continue;
        game->renderer.visibleTrains[game->renderer.visibleTrainCount++] =
            static_cast<int16_t>(train_slot);
        visible_train_tiles[(train_x - game->renderer.viewportX0) * visible_height
                            + train_y - game->renderer.viewportY0] =
            kVisibleTrainMarker;
    }
    // Road vehicles and old road-stop shelters join the tile sweep at their
    // tile (a vehicle at its depth_tile()), so they get the same map depth as
    // trains: buildings south of them are drawn later and cover them.
    std::vector<uint8_t> road_draw_tiles(
        static_cast<std::size_t>(visible_width * visible_height), 0);
    std::vector<int32_t> visible_road_vehicles;
    for (int32_t vehicle_index = 0;
         vehicle_index < static_cast<int32_t>(game->roadVehicles.size());
         ++vehicle_index) {
        const RoadVehicle& vehicle = game->roadVehicles[vehicle_index];
        if (!vehicle.active()) continue;
        const RoadPoint depth = vehicle.depth_tile();
        if (depth.x < game->renderer.viewportX0
            || depth.y < game->renderer.viewportY0
            || depth.x > game->renderer.viewportX1
            || depth.y > game->renderer.viewportY1)
            continue;
        visible_road_vehicles.push_back(vehicle_index);
        road_draw_tiles[(depth.x - game->renderer.viewportX0) * visible_height
                        + depth.y - game->renderer.viewportY0] |=
            kRoadVehicleMarker;
    }
    for (int service_slot = 0; service_slot < kServiceVehicleSlotCount;
         ++service_slot) {
        const ServiceSite& site = game->fleet.serviceSites[service_slot];
        if ((site.flags & service_site::USED_SLOT) == 0
            || (site.flags & service_site::TYPE_MASK) != kRoadStopSiteType)
            continue;
        const int stop_x = unsigned_byte(site.x);
        const int stop_y = unsigned_byte(site.y);
        if (!game->world.in_bounds(stop_x, stop_y)
            || unsigned_byte(game->world.terrain.tileMap[stop_x][stop_y])
                   == tile::VERTICAL_STATION)
            continue;  // new stops are tile code 87, drawn in map depth already
        if (stop_x < game->renderer.viewportX0
            || stop_y < game->renderer.viewportY0
            || stop_x > game->renderer.viewportX1
            || stop_y > game->renderer.viewportY1)
            continue;
        road_draw_tiles[(stop_x - game->renderer.viewportX0) * visible_height
                        + stop_y - game->renderer.viewportY0] |=
            kRoadShelterMarker;
    }
    for (int draw_index = 0;
         draw_index < game->assets.numActiveTileAnimations; ++draw_index) {
        const int map_x = origin_tile_x + game->assets.tileOffsetX[draw_index];
        const int map_y = origin_tile_y + game->assets.tileOffsetY[draw_index];
        if (map_x < 0 || map_x >= game->world.terrain.mapWidth
            || map_y < 0 || map_y >= game->world.terrain.mapHeight)
            continue;
        const int raw_tile_code =
            unsigned_byte(game->world.terrain.tileMap[map_x][map_y]);
        int tile_code = raw_tile_code;
        if (tile_code >= kTileOverlayFlag) tile_code -= kTileOverlayFlag;
        if (tile_code > tile::SLOPE_LAST && tile_code < kTileOverlayFlag
            && (((tile_code < road_tile::kFirstShape
                  || tile_code > road_tile::kLastShape)
                 && tile_code < tile::TRACK_FIRST)
                || tile_code > construction_tile_sprite::kLastCode)) {
            const int draw_x = map_y * map_render_geometry::kTileWidth
                + map_x * map_render_geometry::kTileWidth
                + map_render_geometry::kTileWidth
                - game->renderer.camera.viewportCenterX;
            const int draw_y = game->world.terrain.mapHalfWidth
                - map_x * map_render_geometry::kTileHalfWidth
                + map_y * map_render_geometry::kTileHalfWidth
                + map_render_geometry::kTileWidth
                - game->renderer.camera.viewportCenterY
                - map_render_geometry::kElevationScale
                  * game->world.terrain.tileElevation[map_x][map_y]
                + map_render_geometry::kTopStripHeight;
            if (!game->settings.animationsDisabled) {
                if (tile_code == industry_animation_tile::kGroup2ProductionFrameFirst
                    || tile_code == industry_animation_tile::kGroup2ProductionFrameLast)
                    midp_draw_image(graphics,
                        game->assets.tile_sprites[
                            game->assets.tileSpriteIndex[
                                industry_animation_tile::kGroup2ProductionSpriteIndex]].get(),
                        draw_x, draw_y,
                        map_render_geometry::kTileImageAnchorFlags);
                if (game->world.terrain.phaseTimer == 1
                    && tile_code >= resource_animation_tile::kFirstCode
                    && tile_code <= resource_animation_tile::kLastCode) {
                    int8_t* tile_row =
                        game->world.terrain.tileMap[map_x].data();
                    tile_row[map_y] = static_cast<int8_t>(tile_row[map_y] + 1);
                    if (game->world.terrain.tileMap[map_x][map_y]
                        == resource_animation_tile::kWrapCode)
                        game->world.terrain.tileMap[map_x][map_y] =
                            resource_animation_tile::kFirstCode;
                }
                if (tile_code != industry_animation_tile::kGroup2ProductionFrameLast)
                    midp_draw_image(graphics,
                        game->assets.tile_sprites[
                            game->assets.tileSpriteIndex[tile_code]].get(),
                        draw_x, draw_y,
                        map_render_geometry::kTileImageAnchorFlags);
                else
                    midp_draw_image(graphics,
                        game->assets.tile_sprites[game->assets.tileSpriteIndex[
                            industry_animation_tile::kGroup2ProductionFrameSpriteBase
                            + game->tile55AnimationFrame]].get(),
                        draw_x, draw_y,
                        map_render_geometry::kTileImageAnchorFlags);
                if (tile_code == industry_animation_tile::kGroup1DestinationMarker)
                    midp_draw_image(graphics,
                        game->assets.tile_sprites[game->assets.tileSpriteIndex[
                            industry_animation_tile::kGroup1DestinationFrameSpriteBase
                            + game->tile50AnimationFrame]].get(),
                        draw_x + map_render_geometry::kGroup1DestinationOffsetX,
                        draw_y + map_render_geometry::kGroup1DestinationOffsetY,
                        map_render_geometry::kTileImageAnchorFlags);
                if (tile_code == industry_animation_tile::kGroup3DestinationMarker)
                    midp_draw_image(graphics,
                        game->assets.tile_sprites[game->assets.tileSpriteIndex[
                            industry_animation_tile::kGroup3DestinationFrameSpriteBase
                            + game->tile64AnimationFrame]].get(),
                        draw_x + map_render_geometry::kGroup3DestinationOffsetX,
                        draw_y + map_render_geometry::kGroup3DestinationOffsetY,
                        map_render_geometry::kTileImageAnchorFlags);
            } else {
                midp_draw_image(graphics,
                    game->assets.tile_sprites[game->assets.tileSpriteIndex[
                        industry_animation_tile::kStaticFallbackSpriteIndex]].get(),
                    draw_x, draw_y,
                    map_render_geometry::kTileImageAnchorFlags);
            }
        }
        if (visible_train_tiles[(map_x - game->renderer.viewportX0) * visible_height
                                + map_y - game->renderer.viewportY0]) {
            for (int visible_index = 0;
                 visible_index < game->renderer.visibleTrainCount;
                 ++visible_index) {
                const int visible_train_slot =
                    game->renderer.visibleTrains[visible_index];
                if (!train_flag::isUsed(
                        game->fleet.trainCars[visible_train_slot].flags)
                    || map_x != unsigned_byte(
                        game->fleet.trainCars[visible_train_slot].x)
                    || map_y != unsigned_byte(
                        game->fleet.trainCars[visible_train_slot].y))
                    continue;
                game->renderer.draw_train(*graphics, game->assets, game->world,
                                          game->fleet, visible_train_slot);
            }
        }
        const uint8_t road_marker =
            road_draw_tiles[(map_x - game->renderer.viewportX0) * visible_height
                            + map_y - game->renderer.viewportY0];
        if ((road_marker & kRoadVehicleMarker) != 0) {
            for (const int32_t vehicle_index : visible_road_vehicles) {
                const RoadVehicle& vehicle =
                    game->roadVehicles[vehicle_index];
                if (!vehicle.active()) continue;
                const RoadPoint depth = vehicle.depth_tile();
                if (depth.x != map_x || depth.y != map_y) continue;
                game->draw_road_vehicle(*graphics, vehicle);
            }
        }
        if ((road_marker & kRoadShelterMarker) != 0)
            game->draw_road_stop_shelter(*graphics, map_x, map_y);
        if (raw_tile_code < kTileOverlayFlag) continue;
        const int overlay_draw_x = map_y * map_render_geometry::kTileWidth
            + map_x * map_render_geometry::kTileWidth
            + map_render_geometry::kTileWidth
            - game->renderer.camera.viewportCenterX;
        const int overlay_draw_y = game->world.terrain.mapHalfWidth
            - map_x * map_render_geometry::kTileHalfWidth
            + map_y * map_render_geometry::kTileHalfWidth
            + map_render_geometry::kTileWidth
            - game->renderer.camera.viewportCenterY
            - map_render_geometry::kElevationScale
              * game->world.terrain.tileElevation[map_x][map_y]
            + map_render_geometry::kTopStripHeight;
        int construction_countdown = kNoConstructionCountdown;
        for (int queue_slot = 0; queue_slot < kTileQueueCount; ++queue_slot) {
            if (unsigned_byte(game->world.terrain.tileQueue[queue_slot].x)
                    != map_x
                || unsigned_byte(game->world.terrain.tileQueue[queue_slot].y)
                    != map_y)
                continue;
            construction_countdown =
                game->world.terrain.tileQueue[queue_slot].countdown;
        }
        if (construction_countdown <= 0) continue;
        midp_draw_image(graphics,
            game->assets.tile_sprites[unsigned_byte(
                game->assets.tileSpriteIndex[
                    map_render_geometry::kConstructionProgressSpriteBase
                    + construction_countdown / 3])].get(),
            overlay_draw_x,
            overlay_draw_y + map_render_geometry::kConstructionOverlayOffsetY,
            map_render_geometry::kTileImageAnchorFlags);
    }
    if (!game->settings.selectionHidden) {
        for (int draw_index = 0;
             draw_index < game->assets.numActiveTileAnimations; ++draw_index) {
            const int map_x = origin_tile_x
                + game->assets.tileOffsetX[draw_index];
            const int map_y = origin_tile_y
                + game->assets.tileOffsetY[draw_index];
            for (int selection_index = 0;
                 selection_index < game->ui.numSelectedTrains;
                 ++selection_index) {
                if (game->ui.trainSelectionY[selection_index] != map_x
                    || game->ui.trainSelectionX[selection_index] != map_y)
                    continue;
                const int label_x = map_y * map_render_geometry::kTileWidth
                    + map_x * map_render_geometry::kTileWidth
                    + map_render_geometry::kTileWidth
                    - game->renderer.camera.viewportCenterX;
                const int label_y = game->world.terrain.mapHalfWidth
                    - map_x * map_render_geometry::kTileHalfWidth
                    + map_y * map_render_geometry::kTileHalfWidth
                    + map_render_geometry::kTileWidth
                    - game->renderer.camera.viewportCenterY
                    - map_render_geometry::kElevationScale
                      * game->world.terrain.tileElevation[map_x][map_y]
                    + map_render_geometry::kTopStripHeight;
                game->renderer.draw_selected_train_label(
                    *graphics, game->assets, game->ui, label_x,
                    label_y - map_render_geometry::kTileWidth,
                    static_cast<int8_t>(selection_index));
            }
        }
    }
    game->draw_build_menu_row(*graphics);
    midp_set_clip(graphics, 0, 0, game->renderer.canvasWidth,
                  game->renderer.canvasHeight);
    game->draw_confirm_dialog(*graphics);
}
