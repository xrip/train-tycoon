// Small general game functions.
#include "game_internal.h"

void Renderer::draw_sprite_quad(Surface& surface, const Assets& assets,
                                int32_t n, int32_t n2, int32_t n3,
                                int32_t n4) const {
    midp_draw_image(&surface,
                    assets.tile_sprites[unsignedByte(assets.tileSpriteIndex[n])].get(), n2, n3, n4);
}

void Assets::open_sprite_stream(const char *resource_name) {
    sprite_stream = resources::open(resource_name);
}

bool Game::spend_money(int32_t n) {
    Game* game = this;
    if (game->economy.spend(n)) {
        game->renderer.lastViewportCenterX = -1;
        game->record_money_change(-n);
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

void Game::update_ten_day_metric() {
    Game* game = this;
    game->economy.yearCompanyValueHistory[0] = game_scanMapForMetric(game);
    game->economy.altDecadeTotalCost[0] = game->economy.yearCompanyValueHistory[0];
}
