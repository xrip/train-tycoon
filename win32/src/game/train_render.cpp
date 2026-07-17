// Moving train sprite placement.
#include "game_internal.h"

#include <string.h>

static void draw_train_image(Game *game, Surface *graphics,
                             int sprite_code, int x, int y) {
    int sprite_index = unsignedByte(game->assets.tileSpriteIndex[sprite_code]);
    midp_draw_image(graphics, game->assets.tile_sprites[sprite_index].get(), x, y, 33);
}

void game_drawSpriteSingle(Game *game, Surface *graphics,
                         int32_t train) {
    int16_t sprites[8];
    memcpy(sprites, game->assets.trainSpriteDirectionTable[train_flag::type(game->fleet.trainCars[train].flags)],
           sizeof(sprites));

    int tile_x = unsignedByte(game->fleet.trainCars[train].x);
    int tile_y = unsignedByte(game->fleet.trainCars[train].y);
    if (train_flag::isHead(game->fleet.trainCars[train].flags)) {
        int tile = game->world.tileMap[tile_x][tile_y];
        int track = unsignedByte(game->world.trackMap[tile_x][tile_y]);
        bool lower_sprite = tile != tile::CROSSING && track > 65;
        if (tile == 76) {
            int low_track = track & 7;
            int high_track = (track >> 3) & 7;
            if ((game->fleet.trainCars[train].direction == 0 || game->fleet.trainCars[train].direction == 1) &&
                low_track > 1)
                lower_sprite = true;
            if ((game->fleet.trainCars[train].direction == 2 || game->fleet.trainCars[train].direction == 3) &&
                high_track > 1)
                lower_sprite = true;
        }
        if (lower_sprite)
            for (int i = 0; i < 8; ++i) sprites[i] = (int16_t)(sprites[i] - 30);
    }

    int tile = unsignedByte(game->world.tileMap[tile_x][tile_y]);
    int x = tile_y * 16 + tile_x * 16 + 16 - game->renderer.viewportCenterX;
    int y = game->world.mapHalfWidth - tile_x * 8 + tile_y * 8 + 16 -
            game->renderer.viewportCenterY - 4 * game->world.tileElevation[tile_x][tile_y] + 15;
    int phase = game->fleet.trainCars[train].phase;
    if (phase < 0) return;

    switch (game->fleet.trainCars[train].direction) {
        case 0:
            if (tile == 82) y -= 4;
            if (tile == 79)
                draw_train_image(game, graphics, sprites[3], x - 8 + phase * 2,
                                 y - 7 + phase - phase / 2);
            else if (tile == 77)
                draw_train_image(game, graphics, sprites[3], x - 8 + phase * 2,
                                 y - 11 + phase + phase / 2);
            else
                draw_train_image(game, graphics, sprites[3], x - 8 + phase * 2,
                                 y - 11 + phase);
            break;
        case 1:
            if (tile == 82) y -= 4;
            if (tile == 79)
                draw_train_image(game, graphics, sprites[7], x + 8 - phase * 2,
                                 y - 3 - phase + phase / 2);
            else if (tile == 77)
                draw_train_image(game, graphics, sprites[7], x + 8 - phase * 2,
                                 y + 1 - phase - phase / 2);
            else
                draw_train_image(game, graphics, sprites[7], x + 8 - phase * 2,
                                 y - 3 - phase);
            break;
        case 2:
            if (tile == 81) y -= 4;
            if (tile == 78)
                draw_train_image(game, graphics, sprites[1], x - 8 + phase * 2,
                                 y + 1 - phase - phase / 2);
            else if (tile == 80)
                draw_train_image(game, graphics, sprites[1], x - 8 + phase * 2,
                                 y - 3 - phase + phase / 2);
            else
                draw_train_image(game, graphics, sprites[1], x - 8 + phase * 2,
                                 y - 3 - phase);
            break;
        case 3:
            if (tile == 81) y -= 4;
            if (tile == 78)
                draw_train_image(game, graphics, sprites[5], x + 8 - phase * 2,
                                 y - 11 + phase + phase / 2);
            else if (tile == 80)
                draw_train_image(game, graphics, sprites[5], x + 8 - phase * 2,
                                 y - 7 + phase - phase / 2);
            else
                draw_train_image(game, graphics, sprites[5], x + 8 - phase * 2,
                                 y - 11 + phase);
            break;
        case 4:
            draw_train_image(game, graphics, sprites[2], x - 8 + phase * 2,
                             y - 12);
            break;
        case 5:
            draw_train_image(game, graphics, sprites[6], x + 8 - phase * 2,
                             y - 12);
            break;
        case 6:
            draw_train_image(game, graphics, sprites[2], x - 8 + phase * 2,
                             y - 4);
            break;
        case 7:
            draw_train_image(game, graphics, sprites[6], x + 8 - phase * 2,
                             y - 4);
            break;
        case 8:
            draw_train_image(game, graphics, sprites[0], x + 8, y - 11 + phase);
            break;
        case 9:
            draw_train_image(game, graphics, sprites[4], x + 8, y - 3 - phase);
            break;
        case 10:
            draw_train_image(game, graphics, sprites[0], x - 8, y - 11 + phase);
            break;
        case 11:
            draw_train_image(game, graphics, sprites[4], x - 8, y - 3 - phase);
            break;
    }
}
