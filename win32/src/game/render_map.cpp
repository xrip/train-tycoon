// Isometric world drawing.
// Draws terrain into the offscreen backbuffer (only when the viewport moved),
// blits it, then overlays dynamic tiles, trains, selections and the build menu.
#include "game_internal.h"
#include <vector>

void game_renderMap(Game *game, Surface *g) {
    Surface *bb = game->renderer.backbuffer_surface;
    int n, n2, n3, n4, n5, n6, n7;
    int n9 = game->renderer.viewportCenterX - 2 * game->renderer.viewportCenterY + 16 + 2 * game->world.mapHalfWidth;
    int n10 = game->renderer.viewportCenterX + 2 * game->renderer.viewportCenterY - 16 - 2 * game->world.mapHalfWidth;
    n9 /= 32;
    n10 /= 32;
    if (game->renderer.viewportCenterX != game->renderer.lastViewportCenterX || game->renderer.viewportCenterY != game->renderer.viewportCenterYCached) {
        midp_set_clip(bb, 0, 15, canvasWidth, gameplayViewportHeight);
        midp_set_color(bb, 0);
        midp_fill_rect(bb, 0, 15, canvasWidth, gameplayViewportHeight);
        game->renderer.visibleTrainCount = 0;
        game->renderer.viewportX0 = 2 * game->world.mapWidth;
        game->renderer.viewportY0 = 2 * game->world.mapHeight;
        game->renderer.viewportX1 = -game->world.mapWidth;
        game->renderer.viewportY1 = -game->world.mapWidth;
        for (int n8 = 0; n8 < game->assets.numActiveTileAnimations; ++n8) {
            n7 = n9 + game->assets.tileOffsetX[n8];
            int n11 = n10 + game->assets.tileOffsetY[n8];
            if (n7 >= 0 && n7 < game->world.mapWidth && n11 >= 0 && n11 < game->world.mapHeight) {
                n6 = unsignedByte(game->world.tileMap[n7][n11]);
                if (n6 >= 128) n6 -= 128;
                if (!((n6 != 0 && n6 <= 28) || (n6 >= 77 && n6 <= 86))) {
                    n5 = n11 * 16 + n7 * 16 + 16 - game->renderer.viewportCenterX;
                    n4 = game->world.mapHalfWidth - n7 * 8 + n11 * 8 + 16 - game->renderer.viewportCenterY - 4 * game->world.tileElevation[n7][n11] + 15;
                    if (game->world.tileElevation[n7][n11] > -1) {
                        if ((n6 >= 42 && n6 <= 48) || (n6 >= 70 && n6 <= 76))
                            midp_draw_image(bb, game->assets.tile_sprites[game->assets.tileSpriteIndex[n6]].get(), n5, n4, 33);
                        else
                            midp_draw_image(bb, game->assets.tile_sprites[game->assets.tileSpriteIndex[1]].get(), n5, n4, 33);
                    } else {
                        midp_draw_image(bb, game->assets.tile_sprites[game->assets.tileSpriteIndex[2]].get(), n5, n4, 33);
                    }
                }
                if (n6 >= 3 && n6 <= 28) {
                    n5 = n11 * 16 + n7 * 16 + 16 - game->renderer.viewportCenterX;
                    n4 = game->world.mapHalfWidth - n7 * 8 + n11 * 8 + 16 - game->renderer.viewportCenterY - 4 * game->world.tileElevation[n7][n11] + 15;
                    midp_draw_image(bb, game->assets.tile_sprites[game->assets.tileSpriteIndex[n6]].get(), n5, n4 + game->assets.tileSpriteYOffsetByCode[n6 - 3], 33);
                }
                if (n6 >= 77 && n6 <= 80) {
                    n5 = n11 * 16 + n7 * 16 + 16 - game->renderer.viewportCenterX;
                    n4 = game->world.mapHalfWidth - n7 * 8 + n11 * 8 + 16 - game->renderer.viewportCenterY - 4 * game->world.tileElevation[n7][n11] + 15;
                    midp_draw_image(bb, game->assets.tile_sprites[game->assets.tileSpriteIndex[n6]].get(), n5, n4 + game->assets.tileSpriteYOffsetByCode[n6 - 56 - 3], 33);
                }
                if (n6 >= 80 && n6 <= 86) {
                    n5 = n11 * 16 + n7 * 16 + 16 - game->renderer.viewportCenterX;
                    n4 = game->world.mapHalfWidth - n7 * 8 + n11 * 8 + 16 - game->renderer.viewportCenterY - 4 * game->world.tileElevation[n7][n11] + 15;
                    n3 = 0;
                    switch (n6) { case 84: n3 = 4; break; case 86: n3 = 4; break; }
                    midp_draw_image(bb, game->assets.tile_sprites[game->assets.tileSpriteIndex[n6]].get(), n5, n4 + n3, 33);
                }
            }
            if (n7 > game->renderer.viewportX1) game->renderer.viewportX1 = n7;
            if (n11 > game->renderer.viewportY1) game->renderer.viewportY1 = n11;
            if (n7 < game->renderer.viewportX0) game->renderer.viewportX0 = n7;
            if (n11 >= game->renderer.viewportY0) continue;
            game->renderer.viewportY0 = n11;
        }
        if (game->renderer.viewportX0 < 0) game->renderer.viewportX0 = 0;
        if (game->renderer.viewportY0 < 0) game->renderer.viewportY0 = 0;
        if (game->renderer.viewportX1 >= game->world.mapWidth) game->renderer.viewportX1 = game->world.mapWidth - 1;
        if (game->renderer.viewportY1 >= game->world.mapHeight) game->renderer.viewportY1 = game->world.mapHeight - 1;
        game->renderer.lastViewportCenterX = game->renderer.viewportCenterX;
        game->renderer.viewportCenterYCached = game->renderer.viewportCenterY;
        midp_set_clip(bb, 0, 0, canvasWidth, canvasHeight);
    }
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
        if ((game->fleet.trainFlags[n6] & 0x80) != 128) continue;
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
                if ((game->fleet.trainFlags[n12] & 0x80) != 128 || n5 != unsignedByte(game->fleet.trainX[n12]) || n4 != unsignedByte(game->fleet.trainY[n12])) continue;
                game_drawSpriteSingle(game, g, n12);
            }
        }
        if (n3 < 128) continue;
        n = n4 * 16 + n5 * 16 + 16 - game->renderer.viewportCenterX;
        n12 = game->world.mapHalfWidth - n5 * 8 + n4 * 8 + 16 - game->renderer.viewportCenterY - 4 * game->world.tileElevation[n5][n4] + 15;
        int n14 = -1;
        for (int i = 0; i < 50; ++i) {
            if (unsignedByte(game->world.tileQueue[i][1]) != n5 || unsignedByte(game->world.tileQueue[i][2]) != n4) continue;
            n14 = game->world.tileQueue[i][0];
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
                game_drawRectFill(game, g, n2, n - 16, (int8_t)n3);
            }
        }
    }
    game_drawBuildMenuRow(game, g);
    midp_set_clip(g, 0, 0, canvasWidth, canvasHeight);
    game_drawConfirmDialog(game, g);
}
