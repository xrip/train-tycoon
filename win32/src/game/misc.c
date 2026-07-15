// Small general game functions.
#include "game_methods.h"

// void drawSpriteQuad(Graphics graphics, int n, int n2, int n3, int n4)
void gc_drawSpriteQuad(GameCanvas *gc, MidpGraphics *graphics, int32_t n,
                       int32_t n2, int32_t n3, int32_t n4) {
    midp_draw_image(graphics,
                    gc->tileSprites[gc_byteToUnsignedInt(gc, gc->tileSpriteIndex[n])], n2, n3, n4);
}

// void loadSpriteStream(String string)
void gc_loadSpriteStream(GameCanvas *gc, char *string) {
    gc->spriteStream = midp_resource_open(string);
}

// boolean spendMoney(int n)
bool gc_spendMoney(GameCanvas *gc, int32_t n) {
    if (n <= gc->money) {
        gc->money -= n;
        gc->lastViewportCenterX = -1;
        gc->totalSpent += n;
        gc_pushMoneyHistory(gc, -n);
        return true;
    }
    gc_enterOutOfMoneyState(gc);
    return false;
}

// void everyTenTick()
void gc_everyTenTick(GameCanvas *gc) {
    gc->yearCompanyValueHistory[0] = gc_scanMapForMetric(gc);
    gc->altDecadeTotalCost[0] = gc->yearCompanyValueHistory[0];
}
