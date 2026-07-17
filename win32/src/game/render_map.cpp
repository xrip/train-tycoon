// Isometric world drawing.
// Draws terrain into the offscreen backbuffer (only when the viewport moved),
// blits it, then overlays dynamic tiles, trains, selections and the build menu.
#include "game_internal.h"
#include <vector>

namespace {

struct MapOrigin {
    int x;
    int y;
};

MapOrigin getMapOrigin(const Game *game) {
    int x = game->renderer.camera.viewportCenterX - 2 * game->renderer.camera.viewportCenterY
        + 16 + 2 * game->world.terrain.mapHalfWidth;
    int y = game->renderer.camera.viewportCenterX + 2 * game->renderer.camera.viewportCenterY
        - 16 - 2 * game->world.terrain.mapHalfWidth;
    return {x / 32, y / 32};
}

void redrawStaticMapIfNeeded(Game *game, const MapOrigin &origin) {
    if (game->renderer.camera.viewportCenterX == game->renderer.lastViewportCenterX
        && game->renderer.camera.viewportCenterY == game->renderer.viewportCenterYCached)
        return;

    Surface *backbuffer = game->renderer.backbuffer_surface;
    midp_set_clip(backbuffer, 0, 15, game->renderer.canvasWidth, game->renderer.gameplayViewportHeight);
    midp_set_color(backbuffer, 0);
    midp_fill_rect(backbuffer, 0, 15, game->renderer.canvasWidth, game->renderer.gameplayViewportHeight);
    game->renderer.visibleTrainCount = 0;
    game->renderer.viewportX0 = 2 * game->world.terrain.mapWidth;
    game->renderer.viewportY0 = 2 * game->world.terrain.mapHeight;
    game->renderer.viewportX1 = -game->world.terrain.mapWidth;
    game->renderer.viewportY1 = -game->world.terrain.mapWidth;
    for (int drawIndex = 0; drawIndex < game->assets.numActiveTileAnimations; ++drawIndex) {
        int mapX = origin.x + game->assets.tileOffsetX[drawIndex];
        int mapY = origin.y + game->assets.tileOffsetY[drawIndex];
        if (mapX >= 0 && mapX < game->world.terrain.mapWidth && mapY >= 0 && mapY < game->world.terrain.mapHeight) {
            int tileCode = unsignedByte(game->world.terrain.tileMap[mapX][mapY]);
            if (tileCode >= 128) tileCode -= 128;
            int drawX = mapY * 16 + mapX * 16 + 16 - game->renderer.camera.viewportCenterX;
            int drawY = game->world.terrain.mapHalfWidth - mapX * 8 + mapY * 8 + 16
                - game->renderer.camera.viewportCenterY - 4 * game->world.terrain.tileElevation[mapX][mapY] + 15;
            if (!((tileCode != 0 && tileCode <= 28) || (tileCode >= 77 && tileCode <= 86))) {
                if (game->world.terrain.tileElevation[mapX][mapY] > -1) {
                    if ((tileCode >= 42 && tileCode <= 48) || (tileCode >= 70 && tileCode <= 76))
                        midp_draw_image(backbuffer, game->assets.tile_sprites[game->assets.tileSpriteIndex[tileCode]].get(), drawX, drawY, 33);
                    else
                        midp_draw_image(backbuffer, game->assets.tile_sprites[game->assets.tileSpriteIndex[1]].get(), drawX, drawY, 33);
                } else {
                    midp_draw_image(backbuffer, game->assets.tile_sprites[game->assets.tileSpriteIndex[2]].get(), drawX, drawY, 33);
                }
            }
            if (tileCode >= 3 && tileCode <= 28) {
                midp_draw_image(backbuffer, game->assets.tile_sprites[game->assets.tileSpriteIndex[tileCode]].get(),
                                drawX, drawY + game->assets.tileSpriteYOffsetByCode[tileCode - 3], 33);
            }
            if (tileCode >= 77 && tileCode <= 80) {
                midp_draw_image(backbuffer, game->assets.tile_sprites[game->assets.tileSpriteIndex[tileCode]].get(),
                                drawX, drawY + game->assets.tileSpriteYOffsetByCode[tileCode - 56 - 3], 33);
            }
            if (tileCode >= 80 && tileCode <= 86) {
                int yOffset = 0;
                switch (tileCode) { case 84: yOffset = 4; break; case 86: yOffset = 4; break; }
                midp_draw_image(backbuffer, game->assets.tile_sprites[game->assets.tileSpriteIndex[tileCode]].get(),
                                drawX, drawY + yOffset, 33);
            }
        }
        if (mapX > game->renderer.viewportX1) game->renderer.viewportX1 = mapX;
        if (mapY > game->renderer.viewportY1) game->renderer.viewportY1 = mapY;
        if (mapX < game->renderer.viewportX0) game->renderer.viewportX0 = mapX;
        if (mapY < game->renderer.viewportY0) game->renderer.viewportY0 = mapY;
    }
    if (game->renderer.viewportX0 < 0) game->renderer.viewportX0 = 0;
    if (game->renderer.viewportY0 < 0) game->renderer.viewportY0 = 0;
    if (game->renderer.viewportX1 >= game->world.terrain.mapWidth) game->renderer.viewportX1 = game->world.terrain.mapWidth - 1;
    if (game->renderer.viewportY1 >= game->world.terrain.mapHeight) game->renderer.viewportY1 = game->world.terrain.mapHeight - 1;
    game->renderer.lastViewportCenterX = game->renderer.camera.viewportCenterX;
    game->renderer.viewportCenterYCached = game->renderer.camera.viewportCenterY;
    midp_set_clip(backbuffer, 0, 0, game->renderer.canvasWidth, game->renderer.canvasHeight);
}

} // namespace

void Game::render_map(Surface& surface) {
    Game *game = this;
    Surface *g = &surface;
    int n, n2, n3, n4, n5, n6;
    MapOrigin origin = getMapOrigin(game);
    int n9 = origin.x;
    int n10 = origin.y;
    redrawStaticMapIfNeeded(game, origin);
    game->paint_main(*g);
    midp_draw_image(g, game->renderer.backbuffer.get(), 0, 0, 20);
    midp_set_clip(g, 0, 15, game->renderer.canvasWidth, game->renderer.gameplayViewportHeight);
    game->draw_build_menu_cursor(*g);
    int visible_width = game->renderer.viewportX1 - game->renderer.viewportX0 + 1;
    int visible_height = game->renderer.viewportY1 - game->renderer.viewportY0 + 1;
    if (visible_width < 1) visible_width = 1;
    if (visible_height < 1) visible_height = 1;
    std::vector<uint8_t> visible_train_tiles(
        static_cast<std::size_t>(visible_width * visible_height));
    game->renderer.visibleTrainCount = 0;
    for (n6 = 0; n6 < 500; ++n6) {
        if (!train_flag::isUsed(game->fleet.trainCars[n6].flags)) continue;
        n5 = unsignedByte(game->fleet.trainCars[n6].x);
        n4 = unsignedByte(game->fleet.trainCars[n6].y);
        if (n5 < game->renderer.viewportX0 || n4 < game->renderer.viewportY0 || n5 > game->renderer.viewportX1 || n4 > game->renderer.viewportY1) continue;
        game->renderer.visibleTrains[game->renderer.visibleTrainCount++] = (int16_t)n6;
        visible_train_tiles[(n5 - game->renderer.viewportX0) * visible_height +
                            n4 - game->renderer.viewportY0] = 1;
    }
    for (n6 = 0; n6 < game->assets.numActiveTileAnimations; ++n6) {
        int n12;
        n5 = n9 + game->assets.tileOffsetX[n6];
        n4 = n10 + game->assets.tileOffsetY[n6];
        if (n5 < 0 || n5 >= game->world.terrain.mapWidth || n4 < 0 || n4 >= game->world.terrain.mapHeight) continue;
        n3 = unsignedByte(game->world.terrain.tileMap[n5][n4]);
        n2 = n3;
        if (n2 >= 128) n2 -= 128;
        if (n2 > 28 && n2 < 128 && (((n2 < 42 || n2 > 48) && n2 < 70) || n2 > 86)) {
            n = n4 * 16 + n5 * 16 + 16 - game->renderer.camera.viewportCenterX;
            n12 = game->world.terrain.mapHalfWidth - n5 * 8 + n4 * 8 + 16 - game->renderer.camera.viewportCenterY - 4 * game->world.terrain.tileElevation[n5][n4] + 15;
            if (!game->settings.animationsDisabled) {
                if (n2 == 54 || n2 == 55)
                    midp_draw_image(g, game->assets.tile_sprites[game->assets.tileSpriteIndex[53]].get(), n, n12, 33);
                if (game->world.terrain.phaseTimer == 1 && n2 >= 65 && n2 <= 68) {
                    int8_t *row = game->world.terrain.tileMap[n5].data();
                    row[n4] = (int8_t)(row[n4] + 1);
                    if (game->world.terrain.tileMap[n5][n4] == 69) game->world.terrain.tileMap[n5][n4] = 65;
                }
                if (n2 != 55) midp_draw_image(g, game->assets.tile_sprites[game->assets.tileSpriteIndex[n2]].get(), n, n12, 33);
                else midp_draw_image(g, game->assets.tile_sprites[game->assets.tileSpriteIndex[243 + game->tile55AnimationFrame]].get(), n, n12, 33);
                if (n2 == 50) midp_draw_image(g, game->assets.tile_sprites[game->assets.tileSpriteIndex[240 + game->tile50AnimationFrame]].get(), n + 12, n12 - 28, 33);
                if (n2 == 64) midp_draw_image(g, game->assets.tile_sprites[game->assets.tileSpriteIndex[246 + game->tile64AnimationFrame]].get(), n + 3, n12 - 39, 33);
            } else {
                midp_draw_image(g, game->assets.tile_sprites[game->assets.tileSpriteIndex[41]].get(), n, n12, 33);
            }
        }
        if (visible_train_tiles[(n5 - game->renderer.viewportX0) * visible_height +
                                n4 - game->renderer.viewportY0]) {
            for (n = 0; n < game->renderer.visibleTrainCount; ++n) {
                n12 = game->renderer.visibleTrains[n];
                if (!train_flag::isUsed(game->fleet.trainCars[n12].flags) || n5 != unsignedByte(game->fleet.trainCars[n12].x) || n4 != unsignedByte(game->fleet.trainCars[n12].y)) continue;
                game->renderer.draw_train(*g, game->assets, game->world,
                                          game->fleet, n12);
            }
        }
        if (n3 < 128) continue;
        n = n4 * 16 + n5 * 16 + 16 - game->renderer.camera.viewportCenterX;
        n12 = game->world.terrain.mapHalfWidth - n5 * 8 + n4 * 8 + 16 - game->renderer.camera.viewportCenterY - 4 * game->world.terrain.tileElevation[n5][n4] + 15;
        int n14 = -1;
        for (int i = 0; i < 50; ++i) {
            if (unsignedByte(game->world.terrain.tileQueue[i].x) != n5 || unsignedByte(game->world.terrain.tileQueue[i].y) != n4) continue;
            n14 = game->world.terrain.tileQueue[i].countdown;
        }
        if (n14 <= 0) continue;
        midp_draw_image(g, game->assets.tile_sprites[unsignedByte(game->assets.tileSpriteIndex[250 + n14 / 3])].get(), n, n12 - 3, 33);
    }
    if (!game->settings.selectionHidden) {
        for (n6 = 0; n6 < game->assets.numActiveTileAnimations; ++n6) {
            n5 = n9 + game->assets.tileOffsetX[n6];
            n4 = n10 + game->assets.tileOffsetY[n6];
            for (n3 = 0; n3 < game->ui.numSelectedTrains; ++n3) {
                if (game->ui.trainSelectionY[n3] != n5 || game->ui.trainSelectionX[n3] != n4) continue;
                n2 = n4 * 16 + n5 * 16 + 16 - game->renderer.camera.viewportCenterX;
                n = game->world.terrain.mapHalfWidth - n5 * 8 + n4 * 8 + 16 - game->renderer.camera.viewportCenterY - 4 * game->world.terrain.tileElevation[n5][n4] + 15;
                game->renderer.draw_selected_train_label(*g, game->assets, game->ui, n2, n - 16, (int8_t)n3);
            }
        }
    }
    game->draw_build_menu_row(*g);
    midp_set_clip(g, 0, 0, game->renderer.canvasWidth, game->renderer.canvasHeight);
    game->draw_confirm_dialog(*g);
}
