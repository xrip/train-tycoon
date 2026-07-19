// Small general game functions.
#include "game.h"
#include "support/numeric.h"

void Renderer::resize_canvas(int32_t width, int32_t height) {
    canvasWidth = width;
    canvasHeight = height;
    gameplayViewportHeight = height - 30;
    bottomBarY = height - 15;
    backbuffer = Image::create_blank(width, height);
    backbuffer_surface = backbuffer->drawing_surface();
}

void Renderer::draw_sprite_quad(Surface& surface, const Assets& assets,
                                int32_t sprite_code, int32_t x, int32_t y,
                                int32_t alignment) const {
    midp_draw_image(&surface,
                    assets.tile_sprites[unsigned_byte(
                        assets.tileSpriteIndex[sprite_code])].get(),
                    x, y, alignment);
}

void Assets::open_sprite_stream(const char *resource_name) {
    // Sprites now load from loose PNG files (resources/sprites/<index>.png); this
    // just (re)initialises the consume counter that preserves the original read
    // order. The resource name is retained for call-site compatibility.
    (void)resource_name;
    sprite_load_index = 0;
}

bool Game::spend_money(int32_t amount) {
    Game* game = this;
    if (game->economy.spend(amount)) {
        game->renderer.lastViewportCenterX = -1;
        game->record_money_change(-amount);
        return true;
    }
    game->enter_out_of_money_state();
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
    game->economy.yearCompanyValueHistory[0] = game->scan_map_metric();
    game->economy.altDecadeTotalCost[0] = game->economy.yearCompanyValueHistory[0];
}
