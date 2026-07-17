// Small general game functions.
#include "game_internal.h"

void game_drawSpriteQuad(Game *game, Surface *graphics, int32_t n,
                       int32_t n2, int32_t n3, int32_t n4) {
    midp_draw_image(graphics,
                    game->assets.tile_sprites[unsignedByte(game->assets.tileSpriteIndex[n])].get(), n2, n3, n4);
}

void game_openSpriteStream(Game *game, const char *resource_name) {
    game->assets.sprite_stream = resources::open(resource_name);
}

bool game_spendMoney(Game *game, int32_t n) {
    if (game->economy.spend(n)) {
        game->renderer.lastViewportCenterX = -1;
        game_pushMoneyHistory(game, -n);
        return true;
    }
    game_enterOutOfMoneyState(game);
    return false;
}

bool Economy::spend(int32_t amount) {
    if (amount > money) return false;
    money -= amount;
    totalSpent += amount;
    return true;
}

void game_everyTenTick(Game *game) {
    game->economy.yearCompanyValueHistory[0] = game_scanMapForMetric(game);
    game->economy.altDecadeTotalCost[0] = game->economy.yearCompanyValueHistory[0];
}
