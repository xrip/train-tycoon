// Small general game functions.
#include "game_methods.h"

// void drawSpriteQuad(Graphics graphics, int n, int n2, int n3, int n4)
void gc_drawSpriteQuad(Game *gc, Surface *graphics, int32_t n,
                       int32_t n2, int32_t n3, int32_t n4) {
    midp_draw_image(graphics,
                    gc->assets.tile_sprites[gc_byteToUnsignedInt(gc, gc->assets.tileSpriteIndex[n])].get(), n2, n3, n4);
}

void gc_openSpriteStream(Game *gc, const char *resource_name) {
    gc->assets.sprite_stream = resources::open(resource_name);
}

// boolean spendMoney(int n)
bool gc_spendMoney(Game *gc, int32_t n) {
    if (n <= gc->economy.money) {
        gc->economy.money -= n;
        gc->renderer.lastViewportCenterX = -1;
        gc->economy.totalSpent += n;
        gc_pushMoneyHistory(gc, -n);
        return true;
    }
    gc_enterOutOfMoneyState(gc);
    return false;
}

// void everyTenTick()
void gc_everyTenTick(Game *gc) {
    gc->economy.yearCompanyValueHistory[0] = gc_scanMapForMetric(gc);
    gc->economy.altDecadeTotalCost[0] = gc->economy.yearCompanyValueHistory[0];
}
