#ifndef CAMERA_H
#define CAMERA_H

#include <cstdint>

struct Camera {
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
