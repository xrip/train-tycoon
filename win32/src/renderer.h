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
};

#endif  // RENDERER_H
