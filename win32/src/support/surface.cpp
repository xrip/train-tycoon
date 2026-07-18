// midp_graphics.c - software 2D rasterizer implementing the MIDP Graphics
// primitives Game uses, on a 32bpp surface. All primitives respect the
// active clip rectangle. The screen surface stores 0x00RRGGBB; Image surfaces
// store ARGB and drawImage source-over-blends onto them.
#include "midp.h"
#include <math.h>

namespace {

constexpr uint32_t kRgbColorMask = 0x00FFFFFFu;
constexpr uint32_t kOpaqueAlphaMask = 0xFF000000u;
constexpr uint32_t kColorChannelMask = 0xFFu;
constexpr uint32_t kTransparentAlpha = 0;
constexpr uint32_t kOpaqueAlpha = 255;
constexpr int kFullCircleDegrees = 360;
constexpr double kDegreesPerCircle = 360.0;
constexpr double kPi = 3.14159265358979323846;
constexpr double kUnitCircleMinimum = -1.0;
constexpr double kUnitCircleMaximum = 1.0;

}  // namespace

static inline void clamp_clip(Surface *surface) {
    if (surface->clip_x < 0) { surface->clip_w += surface->clip_x; surface->clip_x = 0; }
    if (surface->clip_y < 0) { surface->clip_h += surface->clip_y; surface->clip_y = 0; }
    if (surface->clip_x + surface->clip_w > surface->width)
        surface->clip_w = surface->width - surface->clip_x;
    if (surface->clip_y + surface->clip_h > surface->height)
        surface->clip_h = surface->height - surface->clip_y;
    if (surface->clip_w < 0) surface->clip_w = 0;
    if (surface->clip_h < 0) surface->clip_h = 0;
}

static inline bool in_clip(const Surface *surface, int x, int y) {
    return x >= surface->clip_x && x < surface->clip_x + surface->clip_w &&
           y >= surface->clip_y && y < surface->clip_y + surface->clip_h;
}

// Write one pixel (already clip-checked). For screen targets colour is opaque.
static inline void put_pixel(Surface *surface, int x, int y, uint32_t argb) {
    surface->pixels[y * surface->width + x] = argb;
}

void midp_gfx_init_screen(Surface *surface, uint32_t *framebuffer,
                          int width, int height) {
    surface->pixels = framebuffer;
    surface->width = width;
    surface->height = height;
    surface->has_alpha = false;
    surface->color = 0;
    surface->clip_x = 0;
    surface->clip_y = 0;
    surface->clip_w = width;
    surface->clip_h = height;
}

void midp_set_color(Surface *surface, int rgb_color) {
    surface->color = rgb_color & kRgbColorMask;
}

void midp_set_clip(Surface *surface, int x, int y, int width, int height) {
    surface->clip_x = x;
    surface->clip_y = y;
    surface->clip_w = width;
    surface->clip_h = height;
    clamp_clip(surface);
}

// Intersect the current clip with the given rectangle (Graphics.clipRect).
void midp_clip_rect(Surface *surface, int x, int y, int width, int height) {
    int clipped_left = surface->clip_x > x ? surface->clip_x : x;
    int clipped_top = surface->clip_y > y ? surface->clip_y : y;
    int clipped_right = (surface->clip_x + surface->clip_w) < (x + width)
        ? (surface->clip_x + surface->clip_w) : (x + width);
    int clipped_bottom = (surface->clip_y + surface->clip_h) < (y + height)
        ? (surface->clip_y + surface->clip_h) : (y + height);
    surface->clip_x = clipped_left;
    surface->clip_y = clipped_top;
    surface->clip_w = clipped_right - clipped_left;
    surface->clip_h = clipped_bottom - clipped_top;
    if (surface->clip_w < 0) surface->clip_w = 0;
    if (surface->clip_h < 0) surface->clip_h = 0;
}

void midp_fill_rect(Surface *surface, int x, int y, int width, int height) {
    const uint32_t argb = (surface->has_alpha ? kOpaqueAlphaMask : 0u)
                        | (uint32_t)surface->color;
    int clipped_left = x < surface->clip_x ? surface->clip_x : x;
    int clipped_top = y < surface->clip_y ? surface->clip_y : y;
    int clipped_right = (x + width) > (surface->clip_x + surface->clip_w)
        ? (surface->clip_x + surface->clip_w) : (x + width);
    int clipped_bottom = (y + height) > (surface->clip_y + surface->clip_h)
        ? (surface->clip_y + surface->clip_h) : (y + height);
    for (int pixel_y = clipped_top; pixel_y < clipped_bottom; ++pixel_y)
        for (int pixel_x = clipped_left; pixel_x < clipped_right; ++pixel_x)
            put_pixel(surface, pixel_x, pixel_y, argb);
}

// Graphics.drawRect draws a width x height outline (right/bottom edges at
// x+width / y+height).
void midp_draw_rect(Surface *surface, int x, int y, int width, int height) {
    midp_draw_line(surface, x, y, x + width, y);
    midp_draw_line(surface, x, y + height, x + width, y + height);
    midp_draw_line(surface, x, y, x, y + height);
    midp_draw_line(surface, x + width, y, x + width, y + height);
}

void midp_draw_line(Surface *surface, int start_x, int start_y,
                    int end_x, int end_y) {
    const uint32_t argb = (surface->has_alpha ? kOpaqueAlphaMask : 0u)
                        | (uint32_t)surface->color;
    int delta_x = end_x - start_x;
    int delta_y = end_y - start_y;
    int step_x = delta_x < 0 ? -1 : 1;
    int step_y = delta_y < 0 ? -1 : 1;
    delta_x = delta_x < 0 ? -delta_x : delta_x;
    delta_y = delta_y < 0 ? -delta_y : delta_y;
    int error = (delta_x > delta_y ? delta_x : -delta_y) / 2;
    int current_x = start_x;
    int current_y = start_y;
    for (;;) {
        if (in_clip(surface, current_x, current_y))
            put_pixel(surface, current_x, current_y, argb);
        if (current_x == end_x && current_y == end_y) break;
        int doubled_error = error;
        if (doubled_error > -delta_x) {
            error -= delta_y;
            current_x += step_x;
        }
        if (doubled_error < delta_y) {
            error += delta_x;
            current_y += step_y;
        }
    }
}

// Midpoint-ellipse based arc. Game uses only full circles
// (sweep_angle == kFullCircleDegrees),
// so we render the whole outline / disc and rely on the clip rectangle. This is
// faithful for the observed call sites; partial sweeps can be added if needed.
static void arc_impl(Surface *surface, int x, int y, int width, int height,
                     int start_angle, int sweep_angle, bool fill) {
    (void)start_angle;
    (void)sweep_angle; // observed usage is full 360-degree circles
    const uint32_t argb = (surface->has_alpha ? kOpaqueAlphaMask : 0u)
                        | (uint32_t)surface->color;
    if (width <= 0 || height <= 0) return;
    const double center_x = x + width / 2.0;
    const double center_y = y + height / 2.0;
    const double radius_x = width / 2.0;
    const double radius_y = height / 2.0;
    if (fill) {
        int clipped_top = y < surface->clip_y ? surface->clip_y : y;
        int clipped_bottom = (y + height) > (surface->clip_y + surface->clip_h)
            ? (surface->clip_y + surface->clip_h) : (y + height);
        for (int pixel_y = clipped_top; pixel_y < clipped_bottom; ++pixel_y) {
            double normalized_y = (pixel_y + 0.5 - center_y) / radius_y;
            if (normalized_y < kUnitCircleMinimum
                || normalized_y > kUnitCircleMaximum)
                continue;
            double half_width = radius_x
                * sqrt(1.0 - normalized_y * normalized_y);
            int clipped_left = (int)(center_x - half_width);
            int clipped_right = (int)(center_x + half_width);
            if (clipped_left < surface->clip_x)
                clipped_left = surface->clip_x;
            if (clipped_right > surface->clip_x + surface->clip_w)
                clipped_right = surface->clip_x + surface->clip_w;
            for (int pixel_x = clipped_left; pixel_x < clipped_right; ++pixel_x)
                put_pixel(surface, pixel_x, pixel_y, argb);
        }
    } else {
        // Sample the ellipse outline in small angular steps.
        for (int angle_degrees = 0;
             angle_degrees < kFullCircleDegrees; ++angle_degrees) {
            double radians = angle_degrees * kPi / kDegreesPerCircle;
            int pixel_x = (int)(center_x + radius_x * cos(radians));
            int pixel_y = (int)(center_y + radius_y * sin(radians));
            if (in_clip(surface, pixel_x, pixel_y))
                put_pixel(surface, pixel_x, pixel_y, argb);
        }
    }
}

void midp_draw_arc(Surface *surface, int x, int y, int width, int height,
                   int start_angle, int sweep_angle) {
    arc_impl(surface, x, y, width, height, start_angle, sweep_angle, false);
}
void midp_fill_arc(Surface *surface, int x, int y, int width, int height,
                   int start_angle, int sweep_angle) {
    arc_impl(surface, x, y, width, height, start_angle, sweep_angle, true);
}

// Source-over blend of one ARGB source pixel onto the target.
static inline uint32_t blend(uint32_t destination_pixel,
                             uint32_t source_pixel,
                             bool destination_has_alpha) {
    const uint32_t source_alpha = source_pixel >> 24;
    if (source_alpha == kTransparentAlpha) return destination_pixel;
    if (source_alpha == kOpaqueAlpha)
        return destination_has_alpha
            ? (kOpaqueAlphaMask | (source_pixel & kRgbColorMask))
            : (source_pixel & kRgbColorMask);
    const uint32_t source_red = (source_pixel >> 16) & kColorChannelMask;
    const uint32_t source_green = (source_pixel >> 8) & kColorChannelMask;
    const uint32_t source_blue = source_pixel & kColorChannelMask;
    const uint32_t destination_red = (destination_pixel >> 16) & kColorChannelMask;
    const uint32_t destination_green = (destination_pixel >> 8) & kColorChannelMask;
    const uint32_t destination_blue = destination_pixel & kColorChannelMask;
    const uint32_t inverse_alpha = kOpaqueAlpha - source_alpha;
    const uint32_t red = (source_red * source_alpha
                          + destination_red * inverse_alpha)
                       / kOpaqueAlpha;
    const uint32_t green = (source_green * source_alpha
                            + destination_green * inverse_alpha)
                         / kOpaqueAlpha;
    const uint32_t blue = (source_blue * source_alpha
                           + destination_blue * inverse_alpha)
                        / kOpaqueAlpha;
    const uint32_t blended_pixel = (red << 16) | (green << 8) | blue;
    return destination_has_alpha ? (kOpaqueAlphaMask | blended_pixel)
                                 : blended_pixel;
}

void midp_draw_image(Surface *surface, const Image *image,
                     int x, int y, int anchor) {
    if (!image) return;
    // Resolve the anchor to a top-left origin.
    int origin_x = x;
    int origin_y = y;
    if (anchor & MIDP_HCENTER) origin_x = x - image->width() / 2;
    else if (anchor & MIDP_RIGHT) origin_x = x - image->width();
    if (anchor & MIDP_VCENTER) origin_y = y - image->height() / 2;
    else if (anchor & MIDP_BOTTOM) origin_y = y - image->height();
    // (MIDP_LEFT / MIDP_TOP keep the given coordinate.)

    int source_start_x = 0;
    int source_start_y = 0;
    int destination_start_x = origin_x;
    int destination_start_y = origin_y;
    if (destination_start_x < surface->clip_x) {
        source_start_x += surface->clip_x - destination_start_x;
        destination_start_x = surface->clip_x;
    }
    if (destination_start_y < surface->clip_y) {
        source_start_y += surface->clip_y - destination_start_y;
        destination_start_y = surface->clip_y;
    }
    int destination_end_x = origin_x + image->width();
    int destination_end_y = origin_y + image->height();
    if (destination_end_x > surface->clip_x + surface->clip_w)
        destination_end_x = surface->clip_x + surface->clip_w;
    if (destination_end_y > surface->clip_y + surface->clip_h)
        destination_end_y = surface->clip_y + surface->clip_h;
    if (destination_start_x >= destination_end_x
        || destination_start_y >= destination_end_y)
        return;

    for (int destination_y = destination_start_y, source_y = source_start_y;
         destination_y < destination_end_y; ++destination_y, ++source_y) {
        const uint32_t *source_row =
            &image->pixels()[source_y * image->width() + source_start_x];
        uint32_t *destination_row =
            &surface->pixels[destination_y * surface->width
                             + destination_start_x];
        for (int destination_x = destination_start_x, source_x = 0;
             destination_x < destination_end_x;
             ++destination_x, ++source_x) {
            destination_row[source_x] = blend(
                destination_row[source_x], source_row[source_x],
                surface->has_alpha);
        }
    }
}
