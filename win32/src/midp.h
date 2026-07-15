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
// Framebuffer geometry (the J2ME canvas was 240x320).
#define MIDP_SCREEN_W 240
#define MIDP_SCREEN_H 320

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

// --- Graphics primitives (only what GameCanvas uses) --------------------------
void midp_gfx_init_screen(Surface *g, uint32_t *fb, int w, int h);
void midp_set_color(Surface *g, int rgb);
void midp_set_clip(Surface *g, int x, int y, int w, int h);
void midp_clip_rect(Surface *g, int x, int y, int w, int h);
void midp_fill_rect(Surface *g, int x, int y, int w, int h);
void midp_draw_rect(Surface *g, int x, int y, int w, int h);
void midp_draw_line(Surface *g, int x1, int y1, int x2, int y2);
void midp_draw_arc(Surface *g, int x, int y, int w, int h, int start, int arc);
void midp_fill_arc(Surface *g, int x, int y, int w, int h, int start, int arc);
void midp_draw_image(Surface *g, const Image *img, int x, int y, int anchor);

// --- java.util.Vector (holds void* elements) ----------------------------------
typedef struct { void **data; int size; int cap; } MidpVector;
MidpVector *midp_vector_new(void);
void midp_vector_add(MidpVector *v, void *element);
void *midp_vector_get(const MidpVector *v, int index);
int midp_vector_size(const MidpVector *v);
void midp_vector_remove_at(MidpVector *v, int index);
void midp_vector_free(MidpVector *v); // frees the vector, not the elements

// --- Array length registry ----------------------------------------------------
// Java arrays carry their length; raw C pointers do not. GameCanvas methods use
// `array.length`, so every game array is allocated through midp_alloc_arr, which
// records its element count. midp_arr_len(ptr) recovers it (0 if unknown).
// Use GC_NEW(type, n) in hand-ported code for length-tracked allocation.
void *midp_alloc_arr(size_t elem_size, int count); // calloc'd, length-tracked
int midp_arr_len(const void *ptr);
void midp_arr_set_len(const void *ptr, int count);
void midp_arr_forget(const void *ptr); // on free, to avoid stale entries
#define GC_NEW(type, n) ((type *)midp_alloc_arr(sizeof(type), (n)))

// --- Timing -------------------------------------------------------------------
int64_t midp_current_time_millis(void);
void midp_sleep(int millis);

// --- WinMM MIDI music ----------------------------------------------------------
bool midp_media_validate_midi(const char *resource_name);
bool midp_media_load_midi(const char *resource_name);
bool midp_media_play_midi(void);
void midp_media_stop(void);
void midp_media_set_volume(int level);
void midp_media_update(void); // main-thread loop check for infinite repeat
void midp_media_shutdown(void);
bool midp_media_is_loaded(void);
bool midp_media_is_playing(void);
const char *midp_media_last_error(void);

// --- Networking stub ----------------------------------------------------------
bool midp_net_sync_highscores(const uint8_t *payload, int len); // stubbed -> false

#ifdef __cplusplus
}
#endif

#endif // MIDP_H
