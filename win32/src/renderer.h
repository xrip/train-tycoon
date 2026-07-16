// Renderer: the off-screen draw target and the per-frame view state used while
// drawing the world and trains (see game/render*.cpp, game/paint.cpp). The
// renderer reads the model and produces pixels; it does not change simulation
// state.
#ifndef RENDERER_H
#define RENDERER_H
#include "midp.h"  // Image, Surface
#include <array>
#include <cstdint>
#include <memory>

// Renderer owns the backbuffer and the on-screen view bookkeeping. backbuffer is
// the off-screen image and backbuffer_surface is a borrowed view into it. The
// sprite-id fields track the currently drawn packed sprite. visibleTrains /
// visibleTrainCount is the culled train list and viewportX0..Y1 the drawn tile
// bounds -- both are part of the save format and keep their exact byte order.
// lastViewportCenterX / viewportCenterYCached cache the last drawn camera centre.
struct Renderer {
    std::unique_ptr<Image> backbuffer;
    Surface* backbuffer_surface = nullptr;
    int32_t currentSpriteId = 0;
    int32_t lastRenderedSpriteId = 0;
    int32_t spriteJustChanged = 0;
    std::array<int16_t, 500> visibleTrains{};
    int32_t visibleTrainCount = 0;
    int32_t viewportX0 = 0;
    int32_t viewportX1 = 0;
    int32_t viewportY0 = 0;
    int32_t viewportY1 = 0;
    int32_t lastViewportCenterX = 0;
    int32_t viewportCenterYCached = 0;

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

    // Camera / view transform. viewportCenter is the live camera centre (panned
    // by input, read by drawing); viewportBase and mapScale set the isometric
    // projection; the half sizes are derived from the map screen size.
    // viewportCenter, viewportBase, and mapScale are saved and keep their exact
    // byte order; the half sizes are recomputed on load.
    int32_t viewportCenterX = 0;
    int32_t viewportCenterY = 0;
    int32_t viewportBaseX = 0;
    int32_t viewportBaseY = 0;
    int32_t mapScaleX = 0;
    int32_t mapScaleY = 0;
    int32_t viewportHalfWidth = 0;
    int32_t viewportHalfHeight = 0;
};

#endif  // RENDERER_H
