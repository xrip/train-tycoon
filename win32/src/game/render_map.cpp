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
    int x = game->renderer.viewportCenterX - 2 * game->renderer.viewportCenterY
        + 16 + 2 * game->world.mapHalfWidth;
    int y = game->renderer.viewportCenterX + 2 * game->renderer.viewportCenterY
        - 16 - 2 * game->world.mapHalfWidth;
    return {x / 32, y / 32};
}

void redrawStaticMapIfNeeded(Game *game, const MapOrigin &origin) {
    if (game->renderer.viewportCenterX == game->renderer.lastViewportCenterX
        && game->renderer.viewportCenterY == game->renderer.viewportCenterYCached)
        return;

    Surface *backbuffer = game->renderer.backbuffer_surface;
    midp_set_clip(backbuffer, 0, 15, canvasWidth, gameplayViewportHeight);
    midp_set_color(backbuffer, 0);
    midp_fill_rect(backbuffer, 0, 15, canvasWidth, gameplayViewportHeight);
    game->renderer.visibleTrainCount = 0;
    game->renderer.viewportX0 = 2 * game->world.mapWidth;
    game->renderer.viewportY0 = 2 * game->world.mapHeight;
    game->renderer.viewportX1 = -game->world.mapWidth;
    game->renderer.viewportY1 = -game->world.mapWidth;
    for (int drawIndex = 0; drawIndex < game->assets.numActiveTileAnimations; ++drawIndex) {
        int mapX = origin.x + game->assets.tileOffsetX[drawIndex];
        int mapY = origin.y + game->assets.tileOffsetY[drawIndex];
        if (mapX >= 0 && mapX < game->world.mapWidth && mapY >= 0 && mapY < game->world.mapHeight) {
            int tileCode = unsignedByte(game->world.tileMap[mapX][mapY]);
            if (tileCode >= 128) tileCode -= 128;
            int drawX = mapY * 16 + mapX * 16 + 16 - game->renderer.viewportCenterX;
            int drawY = game->world.mapHalfWidth - mapX * 8 + mapY * 8 + 16
                - game->renderer.viewportCenterY - 4 * game->world.tileElevation[mapX][mapY] + 15;
            if (!((tileCode != 0 && tileCode <= 28) || (tileCode >= 77 && tileCode <= 86))) {
                if (game->world.tileElevation[mapX][mapY] > -1) {
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
    if (game->renderer.viewportX1 >= game->world.mapWidth) game->renderer.viewportX1 = game->world.mapWidth - 1;
    if (game->renderer.viewportY1 >= game->world.mapHeight) game->renderer.viewportY1 = game->world.mapHeight - 1;
    game->renderer.lastViewportCenterX = game->renderer.viewportCenterX;
    game->renderer.viewportCenterYCached = game->renderer.viewportCenterY;
    midp_set_clip(backbuffer, 0, 0, canvasWidth, canvasHeight);
}

} // namespace

void game_renderMap(Game *game, Surface *g) {
    int n, n2, n3, n4, n5, n6;
    MapOrigin origin = getMapOrigin(game);
    int n9 = origin.x;
    int n10 = origin.y;
    redrawStaticMapIfNeeded(game, origin);
    game_paintMain(game, g);
    midp_draw_image(g, game->renderer.backbuffer.get(), 0, 0, 20);
    midp_set_clip(g, 0, 15, canvasWidth, gameplayViewportHeight);
    game_drawBuildMenuCursor(game, g);
    int visible_width = game->renderer.viewportX1 - game->renderer.viewportX0 + 1;
    int visible_height = game->renderer.viewportY1 - game->renderer.viewportY0 + 1;
    if (visible_width < 1) visible_width = 1;
    if (visible_height < 1) visible_height = 1;
    std::vector<uint8_t> visible_train_tiles(
        static_cast<std::size_t>(visible_width * visible_height));
    game->renderer.visibleTrainCount = 0;
    for (n6 = 0; n6 < 500; ++n6) {
        if (!train_flag::isUsed(game->fleet.trainFlags[n6])) continue;
        n5 = unsignedByte(game->fleet.trainX[n6]);
        n4 = unsignedByte(game->fleet.trainY[n6]);
        if (n5 < game->renderer.viewportX0 || n4 < game->renderer.viewportY0 || n5 > game->renderer.viewportX1 || n4 > game->renderer.viewportY1) continue;
        game->renderer.visibleTrains[game->renderer.visibleTrainCount++] = (int16_t)n6;
        visible_train_tiles[(n5 - game->renderer.viewportX0) * visible_height +
                            n4 - game->renderer.viewportY0] = 1;
    }
    for (n6 = 0; n6 < game->assets.numActiveTileAnimations; ++n6) {
        int n12;
        n5 = n9 + game->assets.tileOffsetX[n6];
        n4 = n10 + game->assets.tileOffsetY[n6];
        if (n5 < 0 || n5 >= game->world.mapWidth || n4 < 0 || n4 >= game->world.mapHeight) continue;
        n3 = unsignedByte(game->world.tileMap[n5][n4]);
        n2 = n3;
        if (n2 >= 128) n2 -= 128;
        if (n2 > 28 && n2 < 128 && (((n2 < 42 || n2 > 48) && n2 < 70) || n2 > 86)) {
            n = n4 * 16 + n5 * 16 + 16 - game->renderer.viewportCenterX;
            n12 = game->world.mapHalfWidth - n5 * 8 + n4 * 8 + 16 - game->renderer.viewportCenterY - 4 * game->world.tileElevation[n5][n4] + 15;
            if (!animationsDisabled) {
                if (n2 == 54 || n2 == 55)
                    midp_draw_image(g, game->assets.tile_sprites[game->assets.tileSpriteIndex[53]].get(), n, n12, 33);
                if (game->world.phaseTimer == 1 && n2 >= 65 && n2 <= 68) {
                    int8_t *row = game->world.tileMap[n5].data();
                    row[n4] = (int8_t)(row[n4] + 1);
                    if (game->world.tileMap[n5][n4] == 69) game->world.tileMap[n5][n4] = 65;
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
                if (!train_flag::isUsed(game->fleet.trainFlags[n12]) || n5 != unsignedByte(game->fleet.trainX[n12]) || n4 != unsignedByte(game->fleet.trainY[n12])) continue;
                game_drawSpriteSingle(game, g, n12);
            }
        }
        if (n3 < 128) continue;
        n = n4 * 16 + n5 * 16 + 16 - game->renderer.viewportCenterX;
        n12 = game->world.mapHalfWidth - n5 * 8 + n4 * 8 + 16 - game->renderer.viewportCenterY - 4 * game->world.tileElevation[n5][n4] + 15;
        int n14 = -1;
        for (int i = 0; i < 50; ++i) {
            if (unsignedByte(game->world.tileQueue[i].x) != n5 || unsignedByte(game->world.tileQueue[i].y) != n4) continue;
            n14 = game->world.tileQueue[i].countdown;
        }
        if (n14 <= 0) continue;
        midp_draw_image(g, game->assets.tile_sprites[unsignedByte(game->assets.tileSpriteIndex[250 + n14 / 3])].get(), n, n12 - 3, 33);
    }
    if (!selectionHidden) {
        for (n6 = 0; n6 < game->assets.numActiveTileAnimations; ++n6) {
            n5 = n9 + game->assets.tileOffsetX[n6];
            n4 = n10 + game->assets.tileOffsetY[n6];
            for (n3 = 0; n3 < game->ui.numSelectedTrains; ++n3) {
                if (game->ui.trainSelectionY[n3] != n5 || game->ui.trainSelectionX[n3] != n4) continue;
                n2 = n4 * 16 + n5 * 16 + 16 - game->renderer.viewportCenterX;
                n = game->world.mapHalfWidth - n5 * 8 + n4 * 8 + 16 - game->renderer.viewportCenterY - 4 * game->world.tileElevation[n5][n4] + 15;
                game_drawSelectedTrainLabel(game, g, n2, n - 16, (int8_t)n3);
            }
        }
    }
    game_drawBuildMenuRow(game, g);
    midp_set_clip(g, 0, 0, canvasWidth, canvasHeight);
    game_drawConfirmDialog(game, g);
}
