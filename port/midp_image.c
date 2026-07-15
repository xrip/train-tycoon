// midp_image.c - Image objects (mutable offscreen surfaces + decoded PNGs).
//
// createImage(w,h) makes a white, opaque, mutable image with a Graphics.
// createImage(byte[]) decodes a PNG via the vendored stb_image (public domain).
#include "midp.h"
#include "stb_image.h"
#include <stdlib.h>
#include <string.h>

MidpImage *midp_image_create_blank(int w, int h) {
    if (w <= 0 || h <= 0) return NULL;
    MidpImage *img = (MidpImage *)calloc(1, sizeof(MidpImage));
    if (!img) return NULL;
    img->width = w;
    img->height = h;
    img->pixels = (uint32_t *)malloc((size_t)w * h * sizeof(uint32_t));
    if (!img->pixels) { free(img); return NULL; }
    for (int i = 0; i < w * h; ++i) img->pixels[i] = 0xFFFFFFFFu; // opaque white
    img->g = (MidpGraphics *)calloc(1, sizeof(MidpGraphics));
    if (!img->g) { free(img->pixels); free(img); return NULL; }
    img->g->pixels = img->pixels;
    img->g->width = w;
    img->g->height = h;
    img->g->target_has_alpha = true;
    img->g->color = 0;
    img->g->clip_x = 0; img->g->clip_y = 0; img->g->clip_w = w; img->g->clip_h = h;
    return img;
}

MidpImage *midp_image_create_from_png(const uint8_t *data, int off, int len) {
    int w = 0, h = 0, ch = 0;
    // Decode to RGBA (4 channels); stb order in memory is R,G,B,A.
    stbi_uc *rgba = stbi_load_from_memory(data + off, len, &w, &h, &ch, 4);
    if (!rgba || w <= 0 || h <= 0) {
        if (rgba) stbi_image_free(rgba);
        return NULL;
    }
    MidpImage *img = (MidpImage *)calloc(1, sizeof(MidpImage));
    if (!img) { stbi_image_free(rgba); return NULL; }
    img->width = w;
    img->height = h;
    img->pixels = (uint32_t *)malloc((size_t)w * h * sizeof(uint32_t));
    if (!img->pixels) { free(img); stbi_image_free(rgba); return NULL; }
    for (int i = 0; i < w * h; ++i) {
        stbi_uc r = rgba[i * 4 + 0], g = rgba[i * 4 + 1];
        stbi_uc b = rgba[i * 4 + 2], a = rgba[i * 4 + 3];
        img->pixels[i] = ((uint32_t)a << 24) | ((uint32_t)r << 16)
                         | ((uint32_t)g << 8) | b; // ARGB
    }
    stbi_image_free(rgba);
    img->g = NULL; // decoded PNGs are immutable
    return img;
}

MidpGraphics *midp_image_get_graphics(MidpImage *img) {
    return img ? img->g : NULL;
}

int midp_image_get_width(const MidpImage *img)  { return img ? img->width : 0; }
int midp_image_get_height(const MidpImage *img) { return img ? img->height : 0; }

void midp_image_free(MidpImage *img) {
    if (!img) return;
    free(img->g);
    free(img->pixels);
    free(img);
}
