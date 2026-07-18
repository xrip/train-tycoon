// midp.h - small game runtime on a 32-bit MiniFB canvas.
//
// The runtime provides Graphics {setColor, fillRect, drawRect, drawLine,
// drawImage(anchor),
// setClip, clipRect, drawArc, fillArc}; Image {createImage(bytes)->PNG,
// createImage(w,h)->mutable, getGraphics, getWidth, getHeight}; java.util.Random
// (exact LCG); Vector; RecordStore files; input (keycodes + game actions);
// WinMM MIDI music. High-score
// networking is a no-op stub for this build.
#ifndef MIDP_H
#define MIDP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "support/image.h"

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// Fixed landscape framebuffer geometry for the native port.
#define MIDP_SCREEN_W 640
#define MIDP_SCREEN_H 480

///////////////////////////////////////////////////////////////////////////////
// drawImage anchor flags (javax.microedition.lcdui.Graphics).
#define MIDP_HCENTER 1
#define MIDP_VCENTER 2
#define MIDP_LEFT    4
#define MIDP_RIGHT   8
#define MIDP_TOP     16
#define MIDP_BOTTOM  32

///////////////////////////////////////////////////////////////////////////////
// Canvas game actions (Canvas.getGameAction result).
#define MIDP_UP    1
#define MIDP_LEFT_ACTION  2
#define MIDP_RIGHT_ACTION 5
#define MIDP_DOWN  6
#define MIDP_FIRE  8
#define MIDP_GAME_A 9
#define MIDP_GAME_B 10
#define MIDP_GAME_C 11
#define MIDP_GAME_D 12

// Key codes (CLDC/MIDP standard). Negative soft keys are device specific; we
// pick common values used by Nokia handsets (the target was N73/N95).
#define MIDP_KEY_NUM0 48
#define MIDP_KEY_NUM9 57
#define MIDP_KEY_STAR 42
#define MIDP_KEY_POUND 35
#define MIDP_KEY_SOFT_LEFT  -6
#define MIDP_KEY_SOFT_RIGHT -7

// --- Graphics primitives (only what Game uses) --------------------------
void midp_gfx_init_screen(Surface *surface, uint32_t *framebuffer,
                          int width, int height);
void midp_set_color(Surface *surface, int rgb_color);
void midp_set_clip(Surface *surface, int x, int y, int width, int height);
void midp_clip_rect(Surface *surface, int x, int y, int width, int height);
void midp_fill_rect(Surface *surface, int x, int y, int width, int height);
void midp_draw_rect(Surface *surface, int x, int y, int width, int height);
void midp_draw_line(Surface *surface, int x1, int y1, int x2, int y2);
void midp_draw_arc(Surface *surface, int x, int y, int width, int height,
                   int start_angle, int sweep_angle);
void midp_fill_arc(Surface *surface, int x, int y, int width, int height,
                   int start_angle, int sweep_angle);
void midp_draw_image(Surface *surface, const Image *image, int x, int y,
                     int anchor);

// --- java.util.Vector (holds void* elements) ----------------------------------
typedef struct { void **data; int size; int capacity; } MidpVector;
MidpVector *midp_vector_new(void);
void midp_vector_add(MidpVector *vector, void *element);
void *midp_vector_get(const MidpVector *vector, int index);
int midp_vector_size(const MidpVector *vector);
void midp_vector_remove_at(MidpVector *vector, int index);
void midp_vector_free(MidpVector *vector); // frees the vector, not the elements

// --- Timing -------------------------------------------------------------------
int64_t midp_current_time_millis(void);
void midp_sleep(int millis);

// --- Networking stub ----------------------------------------------------------
bool midp_net_sync_highscores(const uint8_t *payload, int len); // stubbed -> false

#ifdef __cplusplus
}
#endif

#endif // MIDP_H
