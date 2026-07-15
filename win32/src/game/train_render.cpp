// Moving train sprite placement.
#include "game_methods.h"

#include <string.h>

static void draw_train_image(GameCanvas *gc, Surface *graphics,
                             int sprite_code, int x, int y) {
    int sprite_index = gc_byteToUnsignedInt(gc, gc->tileSpriteIndex[sprite_code]);
    midp_draw_image(graphics, gc->tile_sprites[sprite_index].get(), x, y, 33);
}

void gc_drawSpriteSingle(GameCanvas *gc, Surface *graphics,
                         int32_t train) {
    int16_t sprites[8];
    memcpy(sprites, gc->trainSpriteDirectionTable[gc->trainFlags[train] & 0x0f],
           sizeof(sprites));

    int tile_x = gc_byteToUnsignedInt(gc, gc->trainX[train]);
    int tile_y = gc_byteToUnsignedInt(gc, gc->trainY[train]);
    if ((gc->trainFlags[train] & 0x40) != 0) {
        int tile = gc->tileMap[tile_x][tile_y];
        int track = gc_byteToUnsignedInt(gc, gc->trackMap[tile_x][tile_y]);
        bool lower_sprite = tile != 76 && track > 65;
        if (tile == 76) {
            int low_track = track & 7;
            int high_track = (track >> 3) & 7;
            if ((gc->trainDir[train] == 0 || gc->trainDir[train] == 1) &&
                low_track > 1)
                lower_sprite = true;
            if ((gc->trainDir[train] == 2 || gc->trainDir[train] == 3) &&
                high_track > 1)
                lower_sprite = true;
        }
        if (lower_sprite)
            for (int i = 0; i < 8; ++i) sprites[i] = (int16_t)(sprites[i] - 30);
    }

    int tile = gc_byteToUnsignedInt(gc, gc->tileMap[tile_x][tile_y]);
    int x = tile_y * 16 + tile_x * 16 + 16 - gc->viewportCenterX;
    int y = gc->mapHalfWidth - tile_x * 8 + tile_y * 8 + 16 -
            gc->viewportCenterY - 4 * gc->tileElevation[tile_x][tile_y] + 15;
    int phase = gc->trainPhase[train];
    if (phase < 0) return;

    switch (gc->trainDir[train]) {
        case 0:
            if (tile == 82) y -= 4;
            if (tile == 79)
                draw_train_image(gc, graphics, sprites[3], x - 8 + phase * 2,
                                 y - 7 + phase - phase / 2);
            else if (tile == 77)
                draw_train_image(gc, graphics, sprites[3], x - 8 + phase * 2,
                                 y - 11 + phase + phase / 2);
            else
                draw_train_image(gc, graphics, sprites[3], x - 8 + phase * 2,
                                 y - 11 + phase);
            break;
        case 1:
            if (tile == 82) y -= 4;
            if (tile == 79)
                draw_train_image(gc, graphics, sprites[7], x + 8 - phase * 2,
                                 y - 3 - phase + phase / 2);
            else if (tile == 77)
                draw_train_image(gc, graphics, sprites[7], x + 8 - phase * 2,
                                 y + 1 - phase - phase / 2);
            else
                draw_train_image(gc, graphics, sprites[7], x + 8 - phase * 2,
                                 y - 3 - phase);
            break;
        case 2:
            if (tile == 81) y -= 4;
            if (tile == 78)
                draw_train_image(gc, graphics, sprites[1], x - 8 + phase * 2,
                                 y + 1 - phase - phase / 2);
            else if (tile == 80)
                draw_train_image(gc, graphics, sprites[1], x - 8 + phase * 2,
                                 y - 3 - phase + phase / 2);
            else
                draw_train_image(gc, graphics, sprites[1], x - 8 + phase * 2,
                                 y - 3 - phase);
            break;
        case 3:
            if (tile == 81) y -= 4;
            if (tile == 78)
                draw_train_image(gc, graphics, sprites[5], x + 8 - phase * 2,
                                 y - 11 + phase + phase / 2);
            else if (tile == 80)
                draw_train_image(gc, graphics, sprites[5], x + 8 - phase * 2,
                                 y - 7 + phase - phase / 2);
            else
                draw_train_image(gc, graphics, sprites[5], x + 8 - phase * 2,
                                 y - 11 + phase);
            break;
        case 4:
            draw_train_image(gc, graphics, sprites[2], x - 8 + phase * 2,
                             y - 12);
            break;
        case 5:
            draw_train_image(gc, graphics, sprites[6], x + 8 - phase * 2,
                             y - 12);
            break;
        case 6:
            draw_train_image(gc, graphics, sprites[2], x - 8 + phase * 2,
                             y - 4);
            break;
        case 7:
            draw_train_image(gc, graphics, sprites[6], x + 8 - phase * 2,
                             y - 4);
            break;
        case 8:
            draw_train_image(gc, graphics, sprites[0], x + 8, y - 11 + phase);
            break;
        case 9:
            draw_train_image(gc, graphics, sprites[4], x + 8, y - 3 - phase);
            break;
        case 10:
            draw_train_image(gc, graphics, sprites[0], x - 8, y - 11 + phase);
            break;
        case 11:
            draw_train_image(gc, graphics, sprites[4], x - 8, y - 3 - phase);
            break;
    }
}
