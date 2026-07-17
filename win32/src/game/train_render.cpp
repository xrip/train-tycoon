// Moving train sprite placement.
#include "game_internal.h"

#include <string.h>

static void draw_train_image(const Assets& assets, Surface *graphics,
                             int sprite_code, int x, int y) {
    int sprite_index = unsignedByte(assets.tileSpriteIndex[sprite_code]);
    midp_draw_image(graphics, assets.tile_sprites[sprite_index].get(), x, y, 33);
}

void Renderer::draw_train(Surface& surface, const Assets& assets,
                          const World& world, const Fleet& fleet,
                          int32_t train) const {
    Surface *graphics = &surface;
    int16_t sprites[8];
    memcpy(sprites, assets.trainSpriteDirectionTable[train_flag::type(fleet.trainCars[train].flags)],
           sizeof(sprites));

    int tile_x = unsignedByte(fleet.trainCars[train].x);
    int tile_y = unsignedByte(fleet.trainCars[train].y);
    if (train_flag::isHead(fleet.trainCars[train].flags)) {
        int tile = world.terrain.tileMap[tile_x][tile_y];
        int track = unsignedByte(world.trackMap[tile_x][tile_y]);
        bool lower_sprite = tile != tile::CROSSING && track > 65;
        if (tile == 76) {
            int low_track = track & 7;
            int high_track = (track >> 3) & 7;
            if ((fleet.trainCars[train].direction == 0 || fleet.trainCars[train].direction == 1) &&
                low_track > 1)
                lower_sprite = true;
            if ((fleet.trainCars[train].direction == 2 || fleet.trainCars[train].direction == 3) &&
                high_track > 1)
                lower_sprite = true;
        }
        if (lower_sprite)
            for (int i = 0; i < 8; ++i) sprites[i] = (int16_t)(sprites[i] - 30);
    }

    int tile = unsignedByte(world.terrain.tileMap[tile_x][tile_y]);
    int x = tile_y * 16 + tile_x * 16 + 16 - camera.viewportCenterX;
    int y = world.terrain.mapHalfWidth - tile_x * 8 + tile_y * 8 + 16 -
            camera.viewportCenterY - 4 * world.terrain.tileElevation[tile_x][tile_y] + 15;
    int phase = fleet.trainCars[train].phase;
    if (phase < 0) return;

    switch (fleet.trainCars[train].direction) {
        case 0:
            if (tile == 82) y -= 4;
            if (tile == 79)
                draw_train_image(assets, graphics, sprites[3], x - 8 + phase * 2,
                                 y - 7 + phase - phase / 2);
            else if (tile == 77)
                draw_train_image(assets, graphics, sprites[3], x - 8 + phase * 2,
                                 y - 11 + phase + phase / 2);
            else
                draw_train_image(assets, graphics, sprites[3], x - 8 + phase * 2,
                                 y - 11 + phase);
            break;
        case 1:
            if (tile == 82) y -= 4;
            if (tile == 79)
                draw_train_image(assets, graphics, sprites[7], x + 8 - phase * 2,
                                 y - 3 - phase + phase / 2);
            else if (tile == 77)
                draw_train_image(assets, graphics, sprites[7], x + 8 - phase * 2,
                                 y + 1 - phase - phase / 2);
            else
                draw_train_image(assets, graphics, sprites[7], x + 8 - phase * 2,
                                 y - 3 - phase);
            break;
        case 2:
            if (tile == 81) y -= 4;
            if (tile == 78)
                draw_train_image(assets, graphics, sprites[1], x - 8 + phase * 2,
                                 y + 1 - phase - phase / 2);
            else if (tile == 80)
                draw_train_image(assets, graphics, sprites[1], x - 8 + phase * 2,
                                 y - 3 - phase + phase / 2);
            else
                draw_train_image(assets, graphics, sprites[1], x - 8 + phase * 2,
                                 y - 3 - phase);
            break;
        case 3:
            if (tile == 81) y -= 4;
            if (tile == 78)
                draw_train_image(assets, graphics, sprites[5], x + 8 - phase * 2,
                                 y - 11 + phase + phase / 2);
            else if (tile == 80)
                draw_train_image(assets, graphics, sprites[5], x + 8 - phase * 2,
                                 y - 7 + phase - phase / 2);
            else
                draw_train_image(assets, graphics, sprites[5], x + 8 - phase * 2,
                                 y - 11 + phase);
            break;
        case 4:
            draw_train_image(assets, graphics, sprites[2], x - 8 + phase * 2,
                             y - 12);
            break;
        case 5:
            draw_train_image(assets, graphics, sprites[6], x + 8 - phase * 2,
                             y - 12);
            break;
        case 6:
            draw_train_image(assets, graphics, sprites[2], x - 8 + phase * 2,
                             y - 4);
            break;
        case 7:
            draw_train_image(assets, graphics, sprites[6], x + 8 - phase * 2,
                             y - 4);
            break;
        case 8:
            draw_train_image(assets, graphics, sprites[0], x + 8, y - 11 + phase);
            break;
        case 9:
            draw_train_image(assets, graphics, sprites[4], x + 8, y - 3 - phase);
            break;
        case 10:
            draw_train_image(assets, graphics, sprites[0], x - 8, y - 11 + phase);
            break;
        case 11:
            draw_train_image(assets, graphics, sprites[4], x - 8, y - 3 - phase);
            break;
    }
}
