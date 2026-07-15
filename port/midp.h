// midp.h - minimal MIDP/CLDC compatibility shim for the Train Tycoon Win32 port.
//
// The game (GameCanvas) is a faithful transliteration of a J2ME MIDlet. Rather
// than redesign it, we reproduce the tiny slice of the MIDP 1.0 / CLDC 1.0 API
// that GameCanvas actually calls, on top of a 32bpp framebuffer (MiniFB backend).
//
// Only the calls the game makes are provided (verified against the decompiled
// source): Graphics {setColor, fillRect, drawRect, drawLine, drawImage(anchor),
// setClip, clipRect, drawArc, fillArc}; Image {createImage(bytes)->PNG,
// createImage(w,h)->mutable, getGraphics, getWidth, getHeight}; java.util.Random
// (exact LCG); java.util.Vector; RMS RecordStore (backed by a file); the XOR
// resource reader; input (keycodes + game actions). Media (MIDI) and networking
// (high scores) are no-op stubs for this build.
#ifndef MIDP_H
#define MIDP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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

///////////////////////////////////////////////////////////////////////////////
// Image: a rectangular ARGB (0xAARRGGBB) pixel buffer. Immutable images come
// from decoded PNGs; mutable images (createImage(w,h)) expose a Graphics.
typedef struct MidpImage MidpImage;

// Graphics: a drawing context bound to a target surface (the screen framebuffer
// or a mutable Image's pixels). Colour is 0x00RRGGBB; a clip rectangle bounds
// every primitive.
typedef struct MidpGraphics MidpGraphics;

struct MidpGraphics {
    uint32_t *pixels;   // target surface (screen: 0x00RRGGBB, image: ARGB)
    int width, height;  // target dimensions in pixels
    bool target_has_alpha; // true when drawing into an Image (preserve alpha)
    int color;          // current colour, 0x00RRGGBB
    int clip_x, clip_y, clip_w, clip_h; // active clip rectangle
};

struct MidpImage {
    int width, height;
    uint32_t *pixels;   // ARGB, width*height
    MidpGraphics *g;    // non-null only for mutable images (getGraphics)
};

// --- Graphics primitives (only what GameCanvas uses) --------------------------
void midp_gfx_init_screen(MidpGraphics *g, uint32_t *fb, int w, int h);
void midp_set_color(MidpGraphics *g, int rgb);
void midp_set_clip(MidpGraphics *g, int x, int y, int w, int h);
void midp_clip_rect(MidpGraphics *g, int x, int y, int w, int h);
void midp_fill_rect(MidpGraphics *g, int x, int y, int w, int h);
void midp_draw_rect(MidpGraphics *g, int x, int y, int w, int h);
void midp_draw_line(MidpGraphics *g, int x1, int y1, int x2, int y2);
void midp_draw_arc(MidpGraphics *g, int x, int y, int w, int h, int start, int arc);
void midp_fill_arc(MidpGraphics *g, int x, int y, int w, int h, int start, int arc);
void midp_draw_image(MidpGraphics *g, const MidpImage *img, int x, int y, int anchor);

// --- Image --------------------------------------------------------------------
MidpImage *midp_image_create_blank(int w, int h); // mutable, white, opaque
MidpImage *midp_image_create_from_png(const uint8_t *data, int off, int len);
MidpGraphics *midp_image_get_graphics(MidpImage *img);
int midp_image_get_width(const MidpImage *img);
int midp_image_get_height(const MidpImage *img);
void midp_image_free(MidpImage *img);

// --- java.util.Random (exact 48-bit LCG; map-gen relies on the exact sequence)-
typedef struct { uint64_t seed; } MidpRandom;
void midp_random_init(MidpRandom *r, int64_t seed);
void midp_random_set_seed(MidpRandom *r, int64_t seed);
int32_t midp_random_next_int(MidpRandom *r);        // Random.nextInt()
int32_t midp_random_next_int_bound(MidpRandom *r, int32_t n); // Random.nextInt(n)

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

// --- java.io.ByteArrayOutputStream (growable byte buffer) ---------------------
typedef struct { uint8_t *data; int size; int cap; } MidpByteArray;
MidpByteArray *midp_bytearray_new(void);
void midp_bytearray_write(MidpByteArray *b, int byte_value); // write() low 8 bits
int midp_bytearray_size(const MidpByteArray *b);
uint8_t *midp_bytearray_data(MidpByteArray *b); // toByteArray (live pointer)
void midp_bytearray_reset(MidpByteArray *b);
void midp_bytearray_free(MidpByteArray *b);

// --- Resources ----------------------------------------------------------------
// XorResourceInput: reads /1../5 (cycling XOR key 1..127) or plain bytes.
typedef struct MidpResource MidpResource;
MidpResource *midp_resource_open(const char *name); // e.g. "/1", "/ico.png"
int midp_resource_read_byte(MidpResource *r);        // XOR-decoded byte, -1 at EOF
void midp_resource_close(MidpResource *r);
// Read a whole resource (raw bytes, no XOR). Caller frees *out.
bool midp_resource_read_all(const char *name, uint8_t **out, int *out_len);
// Where resource files live (defaults to "resources"). Set once at startup.
void midp_resource_set_root(const char *root_dir);

// --- RMS RecordStore (backed by <name>.rms in the save directory) -------------
typedef struct MidpRecordStore MidpRecordStore;
MidpRecordStore *midp_rms_open(const char *name, bool create_if_needed);
int midp_rms_add_record(MidpRecordStore *s, const uint8_t *data, int off, int len); // returns id
bool midp_rms_get_record(MidpRecordStore *s, int id, uint8_t **out, int *out_len);
void midp_rms_delete_record(MidpRecordStore *s, int id);
int midp_rms_num_records(MidpRecordStore *s);
int midp_rms_first_record_id(MidpRecordStore *s); // -1 when empty (enumeration)
int midp_rms_record_id_at(MidpRecordStore *s, int index); // enumerate; -1 if OOB
int midp_rms_record_size(MidpRecordStore *s, int id);      // bytes, -1 if missing
void midp_rms_close(MidpRecordStore *s);

// --- Timing -------------------------------------------------------------------
int64_t midp_current_time_millis(void);
void midp_sleep(int millis);

// --- Media / networking stubs (no-op for this build) --------------------------
void midp_media_play_midi(const char *resource_name); // stubbed
void midp_media_stop(void);
void midp_media_set_volume(int level);
bool midp_net_sync_highscores(const uint8_t *payload, int len); // stubbed -> false

#endif // MIDP_H
