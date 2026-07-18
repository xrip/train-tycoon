// Moving train sprite placement.
#include "game.h"
#include "support/numeric.h"

#include <string.h>

static void draw_train_image(const Assets& assets, Surface* target_surface,
                             int sprite_code, int screen_x, int screen_y) {
    int sprite_index = unsigned_byte(assets.tileSpriteIndex[sprite_code]);
    midp_draw_image(target_surface, assets.tile_sprites[sprite_index].get(),
                    screen_x, screen_y, 33);
}

void Renderer::draw_train(Surface& surface, const Assets& assets,
                          const World& world, const Fleet& fleet,
                          int32_t train) const {
    Surface* target_surface = &surface;
    int16_t train_sprites[kTrainSpriteDirectionCount];
    memcpy(train_sprites,
           assets.trainSpriteDirectionTable[train_flag::type(fleet.trainCars[train].flags)],
           sizeof(train_sprites));

    int tile_x = unsigned_byte(fleet.trainCars[train].x);
    int tile_y = unsigned_byte(fleet.trainCars[train].y);
    if (train_flag::isHead(fleet.trainCars[train].flags)) {
        const int tile_code = world.terrain.tileMap[tile_x][tile_y];
        const int track_code = unsigned_byte(world.trackMap[tile_x][tile_y]);
        bool lower_sprite = tile_code != tile::CROSSING
                         && track_code > tile::SERVICE_TRACK;
        if (tile_code == tile::CROSSING) {
            const int lower_track_owner = track_code & track::DIR_OWNER_MASK;
            const int upper_track_owner =
                (track_code >> track::DIR_OWNER_SHIFT) & track::DIR_OWNER_MASK;
            if ((fleet.trainCars[train].direction == train_direction_slot::Slot0
                 || fleet.trainCars[train].direction == train_direction_slot::Slot1) &&
                lower_track_owner > 1)
                lower_sprite = true;
            if ((fleet.trainCars[train].direction == train_direction_slot::Slot2
                 || fleet.trainCars[train].direction == train_direction_slot::Slot3) &&
                upper_track_owner > 1)
                lower_sprite = true;
        }
        if (lower_sprite)
            for (int sprite_slot = 0; sprite_slot < kTrainSpriteDirectionCount; ++sprite_slot)
                train_sprites[sprite_slot] =
                    (int16_t)(train_sprites[sprite_slot] - 30);
    }

    const int tile_code = unsigned_byte(world.terrain.tileMap[tile_x][tile_y]);
    int screen_x = tile_y * 16 + tile_x * 16 + 16 - camera.viewportCenterX;
    int screen_y = world.terrain.mapHalfWidth - tile_x * 8 + tile_y * 8 + 16 -
            camera.viewportCenterY - 4 * world.terrain.tileElevation[tile_x][tile_y] + 15;
    const int phase_step = fleet.trainCars[train].phase;
    if (phase_step < 0) return;

    switch (fleet.trainCars[train].direction) {
        case train_direction_slot::Slot0:
            if (tile_code == construction_tile::VerticalA) screen_y -= 4;
            if (tile_code == track_turn_tile::VariantC)
                draw_train_image(assets, target_surface, train_sprites[3],
                                 screen_x - 8 + phase_step * 2,
                                 screen_y - 7 + phase_step - phase_step / 2);
            else if (tile_code == track_turn_tile::VariantA)
                draw_train_image(assets, target_surface, train_sprites[3],
                                 screen_x - 8 + phase_step * 2,
                                 screen_y - 11 + phase_step + phase_step / 2);
            else
                draw_train_image(assets, target_surface, train_sprites[3],
                                 screen_x - 8 + phase_step * 2,
                                 screen_y - 11 + phase_step);
            break;
        case train_direction_slot::Slot1:
            if (tile_code == construction_tile::VerticalA) screen_y -= 4;
            if (tile_code == track_turn_tile::VariantC)
                draw_train_image(assets, target_surface, train_sprites[7],
                                 screen_x + 8 - phase_step * 2,
                                 screen_y - 3 - phase_step + phase_step / 2);
            else if (tile_code == track_turn_tile::VariantA)
                draw_train_image(assets, target_surface, train_sprites[7],
                                 screen_x + 8 - phase_step * 2,
                                 screen_y + 1 - phase_step - phase_step / 2);
            else
                draw_train_image(assets, target_surface, train_sprites[7],
                                 screen_x + 8 - phase_step * 2,
                                 screen_y - 3 - phase_step);
            break;
        case train_direction_slot::Slot2:
            if (tile_code == construction_tile::HorizontalA) screen_y -= 4;
            if (tile_code == track_turn_tile::VariantB)
                draw_train_image(assets, target_surface, train_sprites[1],
                                 screen_x - 8 + phase_step * 2,
                                 screen_y + 1 - phase_step - phase_step / 2);
            else if (tile_code == track_turn_tile::VariantD)
                draw_train_image(assets, target_surface, train_sprites[1],
                                 screen_x - 8 + phase_step * 2,
                                 screen_y - 3 - phase_step + phase_step / 2);
            else
                draw_train_image(assets, target_surface, train_sprites[1],
                                 screen_x - 8 + phase_step * 2,
                                 screen_y - 3 - phase_step);
            break;
        case train_direction_slot::Slot3:
            if (tile_code == construction_tile::HorizontalA) screen_y -= 4;
            if (tile_code == track_turn_tile::VariantB)
                draw_train_image(assets, target_surface, train_sprites[5],
                                 screen_x + 8 - phase_step * 2,
                                 screen_y - 11 + phase_step + phase_step / 2);
            else if (tile_code == track_turn_tile::VariantD)
                draw_train_image(assets, target_surface, train_sprites[5],
                                 screen_x + 8 - phase_step * 2,
                                 screen_y - 7 + phase_step - phase_step / 2);
            else
                draw_train_image(assets, target_surface, train_sprites[5],
                                 screen_x + 8 - phase_step * 2,
                                 screen_y - 11 + phase_step);
            break;
        case train_direction_slot::Slot4:
            draw_train_image(assets, target_surface, train_sprites[2],
                             screen_x - 8 + phase_step * 2, screen_y - 12);
            break;
        case train_direction_slot::Slot5:
            draw_train_image(assets, target_surface, train_sprites[6],
                             screen_x + 8 - phase_step * 2, screen_y - 12);
            break;
        case train_direction_slot::Slot6:
            draw_train_image(assets, target_surface, train_sprites[2],
                             screen_x - 8 + phase_step * 2, screen_y - 4);
            break;
        case train_direction_slot::Slot7:
            draw_train_image(assets, target_surface, train_sprites[6],
                             screen_x + 8 - phase_step * 2, screen_y - 4);
            break;
        case train_direction_slot::Slot8:
            draw_train_image(assets, target_surface, train_sprites[0],
                             screen_x + 8, screen_y - 11 + phase_step);
            break;
        case train_direction_slot::Slot9:
            draw_train_image(assets, target_surface, train_sprites[4],
                             screen_x + 8, screen_y - 3 - phase_step);
            break;
        case train_direction_slot::Slot10:
            draw_train_image(assets, target_surface, train_sprites[0],
                             screen_x - 8, screen_y - 11 + phase_step);
            break;
        case train_direction_slot::Slot11:
            draw_train_image(assets, target_surface, train_sprites[4],
                             screen_x - 8, screen_y - 3 - phase_step);
            break;
    }
}
