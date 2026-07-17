// Renderer: the off-screen draw target and the per-frame view state used while
// drawing the world and trains (see game/render*.cpp, game/paint.cpp). The
// renderer reads the model and produces pixels; it does not change simulation
// state.
#ifndef RENDERER_H
#define RENDERER_H
#include "game_constants.h"
#include "camera.h"
#include "midp.h"  // Image, Surface
#include <cstdint>
#include <memory>

struct Assets;

// Renderer owns the backbuffer and the on-screen view bookkeeping. backbuffer is
// the off-screen image and backbuffer_surface is a borrowed view into it. The
// sprite-id fields track the currently drawn packed sprite. visibleTrains /
// visibleTrainCount is the culled train list and viewportX0..Y1 the drawn tile
// bounds -- both are part of the save format and keep their exact byte order.
// lastViewportCenterX / viewportCenterYCached cache the last drawn camera centre.
struct Renderer {
    void draw_sprite_quad(Surface& surface, const Assets& assets,
                          int32_t sprite, int32_t x, int32_t y,
                          int32_t anchor) const;

    std::unique_ptr<Image> backbuffer;
    Surface* backbuffer_surface = nullptr;
    int32_t currentSpriteId = 0;
    int32_t lastRenderedSpriteId = -1;
    int32_t spriteJustChanged = 0;
    int16_t visibleTrains[kTrainSlotCount]{};
    int32_t visibleTrainCount = 0;
    int32_t viewportX0 = 0;
    int32_t viewportX1 = 0;
    int32_t viewportY0 = 0;
    int32_t viewportY1 = 0;
    int32_t lastViewportCenterX = -1;
    int32_t viewportCenterYCached = -1;

    // Per-frame draw bookkeeping: the active clip rectangle, the repaint and
    // map-redraw request flags, and the measured dialog content height. All
    // runtime-only. Draw order is unchanged.
    int32_t clipX = 0;
    int32_t clipY = 0;
    int32_t clipWidth = 0;
    int32_t clipHeight = 0;
    bool needsRepaint = false;
    bool mapRedrawPending = false;
    int32_t dialogContentHeight = 0;

    Camera camera;

    // Camera / view transform. viewportCenter is the live camera centre (panned
    // by input, read by drawing); viewportBase and mapScale set the isometric
    // projection; the half sizes are derived from the map screen size.
    // viewportCenter, viewportBase, and mapScale are saved and keep their exact
    // byte order; the half sizes are recomputed on load.
};

#endif  // RENDERER_H
