// main.c - Win32 entry point. Single translation unit that pulls in the MiniFB
// implementation; MFB_HANDLE_INPUT forwards keyboard events to the MIDP input
// bridge. Opens the 240x320 canvas (2x, 32bpp) and drives the generated
// GameCanvas: init -> per-frame (input -> update -> paint) -> blit.
//
// The original run() was a Runnable game-thread loop; here the host owns the
// loop and calls the ported update()/paint() each frame (the run() adaptation).
#include "midp_input.h"

// MiniFB.h uses the C23 `nullptr` keyword, absent from MSVC's C compiler.
#if defined(_MSC_VER) && !defined(__cplusplus)
#  include <stddef.h>
#  ifndef nullptr
#    define nullptr NULL
#  endif
#endif

#define MFB_HANDLE_INPUT(wParam, isKeyDown) midp_input_on_key((int)(wParam), (isKeyDown))
#define MINIFB_IMPLEMENTATION
#include "MiniFB.h"

#include "midp.h"
#include "game_state.h"
#include "game_methods.h"
#include <stdlib.h>

#define WIN_SCALE 2
#define TARGET_FPS 30

static uint32_t g_framebuffer[MIDP_SCREEN_W * MIDP_SCREEN_H];

static bool run_pathfinding_smoke(void) {
    GameCanvas *test = (GameCanvas *)malloc(sizeof(GameCanvas));
    if (!test) return false;
    gc_fields_init(test);
    test->mapWidth = 5;
    test->mapHeight = 5;
    test->tileMap = GC_NEW(int8_t *, test->mapWidth);
    test->trackMap = GC_NEW(int8_t *, test->mapWidth);
    test->tileElevation = GC_NEW(int8_t *, test->mapWidth);
    for (int x = 0; x < test->mapWidth; ++x) {
        test->tileMap[x] = GC_NEW(int8_t, test->mapHeight);
        test->trackMap[x] = GC_NEW(int8_t, test->mapHeight);
        test->tileElevation[x] = GC_NEW(int8_t, test->mapHeight);
    }
    test->placementX = 1;
    test->placementY = 2;
    test->demolishX = 3;
    test->demolishY = 2;
    test->accel1 = 10;
    bool ok = gc_tryBuildPath(test)
              && test->trackTileCount > 0
              && test->selectionCost > 0;
    free(test);
    return ok;
}

int main(void) {
    midp_resource_set_root("resources");

    // Smoke mode (set by `port.py verify`): run N frames then exit cleanly.
    int smoke_frames = 0;
    const char *smoke = getenv("TRAIN_TYCOON_SMOKE");
    if (smoke) smoke_frames = atoi(smoke);
    if (smoke_frames && !run_pathfinding_smoke()) return 2;

    if (!mfb_open("Train Tycoon", MIDP_SCREEN_W, MIDP_SCREEN_H, WIN_SCALE, 32))
        return 1;

    GameCanvas *gc = (GameCanvas *)malloc(sizeof(GameCanvas));
    if (!gc) { mfb_close(); return 1; }
    gc_fields_init(gc);
    gc_start(gc, NULL); // resource load + initial state (no thread spawn)

    MidpGraphics screen;
    midp_gfx_init_screen(&screen, g_framebuffer, MIDP_SCREEN_W, MIDP_SCREEN_H);

    int frame = 0;
    for (;;) {
        int keycode; bool pressed;
        while (midp_input_poll(&keycode, &pressed)) {
            if (pressed) gc_keyPressed(gc, keycode);
            else         gc_keyReleased(gc, keycode);
        }

        gc_run(gc);           // one iteration of the original run() loop
        gc_paint(gc, &screen);

        if (!mfb_update(g_framebuffer, TARGET_FPS))
            break;
        if (smoke_frames && ++frame >= smoke_frames)
            break;
    }

    free(gc);
    mfb_close();
    return 0;
}
