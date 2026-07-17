#ifndef CAMERA_H
#define CAMERA_H

#include <cstdint>

struct Camera {
    void pan(int32_t key, int32_t map_screen_width,
             int32_t map_screen_height, int32_t canvas_width,
             int32_t viewport_height);

    int32_t viewportCenterX = 0;
    int32_t viewportCenterY = 0;
    int32_t viewportBaseX = 0;
    int32_t viewportBaseY = 0;
    int32_t mapScaleX = 0;
    int32_t mapScaleY = 0;
    int32_t viewportHalfWidth = 0;
    int32_t viewportHalfHeight = 0;
};

#endif  // CAMERA_H
