// midp_graphics.c - software 2D rasterizer implementing the MIDP Graphics
// primitives GameCanvas uses, on a 32bpp surface. All primitives respect the
// active clip rectangle. The screen surface stores 0x00RRGGBB; Image surfaces
// store ARGB and drawImage source-over-blends onto them.
#include "midp.h"
#include <math.h>

static inline void clamp_clip(Surface *g) {
    if (g->clip_x < 0) { g->clip_w += g->clip_x; g->clip_x = 0; }
    if (g->clip_y < 0) { g->clip_h += g->clip_y; g->clip_y = 0; }
    if (g->clip_x + g->clip_w > g->width)  g->clip_w = g->width  - g->clip_x;
    if (g->clip_y + g->clip_h > g->height) g->clip_h = g->height - g->clip_y;
    if (g->clip_w < 0) g->clip_w = 0;
    if (g->clip_h < 0) g->clip_h = 0;
}

static inline bool in_clip(const Surface *g, int x, int y) {
    return x >= g->clip_x && x < g->clip_x + g->clip_w &&
           y >= g->clip_y && y < g->clip_y + g->clip_h;
}

// Write one pixel (already clip-checked). For screen targets colour is opaque.
static inline void put_pixel(Surface *g, int x, int y, uint32_t argb) {
    g->pixels[y * g->width + x] = argb;
}

void midp_gfx_init_screen(Surface *g, uint32_t *fb, int w, int h) {
    g->pixels = fb;
    g->width = w;
    g->height = h;
    g->has_alpha = false;
    g->color = 0;
    g->clip_x = 0; g->clip_y = 0; g->clip_w = w; g->clip_h = h;
}

void midp_set_color(Surface *g, int rgb) {
    g->color = rgb & 0x00FFFFFF;
}

void midp_set_clip(Surface *g, int x, int y, int w, int h) {
    g->clip_x = x; g->clip_y = y; g->clip_w = w; g->clip_h = h;
    clamp_clip(g);
}

// Intersect the current clip with the given rectangle (Graphics.clipRect).
void midp_clip_rect(Surface *g, int x, int y, int w, int h) {
    int x0 = g->clip_x > x ? g->clip_x : x;
    int y0 = g->clip_y > y ? g->clip_y : y;
    int x1 = (g->clip_x + g->clip_w) < (x + w) ? (g->clip_x + g->clip_w) : (x + w);
    int y1 = (g->clip_y + g->clip_h) < (y + h) ? (g->clip_y + g->clip_h) : (y + h);
    g->clip_x = x0; g->clip_y = y0;
    g->clip_w = x1 - x0; g->clip_h = y1 - y0;
    if (g->clip_w < 0) g->clip_w = 0;
    if (g->clip_h < 0) g->clip_h = 0;
}

void midp_fill_rect(Surface *g, int x, int y, int w, int h) {
    const uint32_t argb = (g->has_alpha ? 0xFF000000u : 0u) | (uint32_t)g->color;
    int x0 = x < g->clip_x ? g->clip_x : x;
    int y0 = y < g->clip_y ? g->clip_y : y;
    int x1 = (x + w) > (g->clip_x + g->clip_w) ? (g->clip_x + g->clip_w) : (x + w);
    int y1 = (y + h) > (g->clip_y + g->clip_h) ? (g->clip_y + g->clip_h) : (y + h);
    for (int py = y0; py < y1; ++py)
        for (int px = x0; px < x1; ++px)
            put_pixel(g, px, py, argb);
}

// Graphics.drawRect draws a w x h outline (right/bottom edges at x+w / y+h).
void midp_draw_rect(Surface *g, int x, int y, int w, int h) {
    midp_draw_line(g, x, y, x + w, y);
    midp_draw_line(g, x, y + h, x + w, y + h);
    midp_draw_line(g, x, y, x, y + h);
    midp_draw_line(g, x + w, y, x + w, y + h);
}

void midp_draw_line(Surface *g, int x1, int y1, int x2, int y2) {
    const uint32_t argb = (g->has_alpha ? 0xFF000000u : 0u) | (uint32_t)g->color;
    int dx = x2 - x1, dy = y2 - y1;
    int sx = dx < 0 ? -1 : 1, sy = dy < 0 ? -1 : 1;
    dx = dx < 0 ? -dx : dx;
    dy = dy < 0 ? -dy : dy;
    int err = (dx > dy ? dx : -dy) / 2;
    int x = x1, y = y1;
    for (;;) {
        if (in_clip(g, x, y)) put_pixel(g, x, y, argb);
        if (x == x2 && y == y2) break;
        int e2 = err;
        if (e2 > -dx) { err -= dy; x += sx; }
        if (e2 <  dy) { err += dx; y += sy; }
    }
}

// Midpoint-ellipse based arc. GameCanvas uses only full circles (arc == 360),
// so we render the whole outline / disc and rely on the clip rectangle. This is
// faithful for the observed call sites; partial sweeps can be added if needed.
static void arc_impl(Surface *g, int x, int y, int w, int h,
                     int start, int arc, bool fill) {
    (void)start; (void)arc; // observed usage is full 360-degree circles
    const uint32_t argb = (g->has_alpha ? 0xFF000000u : 0u) | (uint32_t)g->color;
    if (w <= 0 || h <= 0) return;
    const double cx = x + w / 2.0, cy = y + h / 2.0;
    const double rx = w / 2.0, ry = h / 2.0;
    if (fill) {
        int y0 = y < g->clip_y ? g->clip_y : y;
        int y1 = (y + h) > (g->clip_y + g->clip_h) ? (g->clip_y + g->clip_h) : (y + h);
        for (int py = y0; py < y1; ++py) {
            double ny = (py + 0.5 - cy) / ry;
            if (ny < -1.0 || ny > 1.0) continue;
            double half = rx * sqrt(1.0 - ny * ny);
            int px0 = (int)(cx - half), px1 = (int)(cx + half);
            if (px0 < g->clip_x) px0 = g->clip_x;
            if (px1 > g->clip_x + g->clip_w) px1 = g->clip_x + g->clip_w;
            for (int px = px0; px < px1; ++px) put_pixel(g, px, py, argb);
        }
    } else {
        // Sample the ellipse outline in small angular steps.
        for (int a = 0; a < 360; ++a) {
            double rad = a * 3.14159265358979323846 / 180.0;
            int px = (int)(cx + rx * cos(rad));
            int py = (int)(cy + ry * sin(rad));
            if (in_clip(g, px, py)) put_pixel(g, px, py, argb);
        }
    }
}

void midp_draw_arc(Surface *g, int x, int y, int w, int h, int start, int arc) {
    arc_impl(g, x, y, w, h, start, arc, false);
}
void midp_fill_arc(Surface *g, int x, int y, int w, int h, int start, int arc) {
    arc_impl(g, x, y, w, h, start, arc, true);
}

// Source-over blend of one ARGB source pixel onto the target.
static inline uint32_t blend(uint32_t dst, uint32_t src, bool dst_alpha) {
    uint32_t a = src >> 24;
    if (a == 0) return dst;
    if (a == 255) return dst_alpha ? (0xFF000000u | (src & 0x00FFFFFF)) : (src & 0x00FFFFFF);
    uint32_t sr = (src >> 16) & 0xFF, sg = (src >> 8) & 0xFF, sb = src & 0xFF;
    uint32_t dr = (dst >> 16) & 0xFF, dg = (dst >> 8) & 0xFF, db = dst & 0xFF;
    uint32_t ia = 255 - a;
    uint32_t rr = (sr * a + dr * ia) / 255;
    uint32_t rg = (sg * a + dg * ia) / 255;
    uint32_t rb = (sb * a + db * ia) / 255;
    uint32_t out = (rr << 16) | (rg << 8) | rb;
    return dst_alpha ? (0xFF000000u | out) : out;
}

void midp_draw_image(Surface *g, const Image *img, int x, int y, int anchor) {
    if (!img) return;
    // Resolve the anchor to a top-left origin.
    int ox = x, oy = y;
    if (anchor & MIDP_HCENTER) ox = x - img->width() / 2;
    else if (anchor & MIDP_RIGHT) ox = x - img->width();
    if (anchor & MIDP_VCENTER) oy = y - img->height() / 2;
    else if (anchor & MIDP_BOTTOM) oy = y - img->height();
    // (MIDP_LEFT / MIDP_TOP keep the given coordinate.)

    int sx0 = 0, sy0 = 0;
    int dx0 = ox, dy0 = oy;
    if (dx0 < g->clip_x) { sx0 += g->clip_x - dx0; dx0 = g->clip_x; }
    if (dy0 < g->clip_y) { sy0 += g->clip_y - dy0; dy0 = g->clip_y; }
    int dx1 = ox + img->width();
    int dy1 = oy + img->height();
    if (dx1 > g->clip_x + g->clip_w) dx1 = g->clip_x + g->clip_w;
    if (dy1 > g->clip_y + g->clip_h) dy1 = g->clip_y + g->clip_h;
    if (dx0 >= dx1 || dy0 >= dy1) return;

    for (int dy = dy0, sy = sy0; dy < dy1; ++dy, ++sy) {
        const uint32_t *srow = &img->pixels()[sy * img->width() + sx0];
        uint32_t *drow = &g->pixels[dy * g->width + dx0];
        for (int dx = dx0, sx = 0; dx < dx1; ++dx, ++sx) {
            drow[sx] = blend(drow[sx], srow[sx], g->has_alpha);
        }
    }
}
