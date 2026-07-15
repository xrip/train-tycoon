// main.c - Win32 entry point and game frame loop.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIN_SCALE 2
#define TARGET_FPS 30

static uint32_t g_framebuffer[MIDP_SCREEN_W * MIDP_SCREEN_H];

static void set_resource_root_from_executable(void) {
    char path[MAX_PATH];
    DWORD length = GetModuleFileNameA(NULL, path, sizeof(path));
    if (length == 0 || length >= sizeof(path)) {
        midp_resource_set_root("resources");
        return;
    }
    char *slash = strrchr(path, '\\');
    if (!slash) slash = strrchr(path, '/');
    if (!slash) {
        midp_resource_set_root("resources");
        return;
    }
    snprintf(slash + 1, sizeof(path) - (size_t)(slash + 1 - path),
             "resources");
    midp_resource_set_root(path);
}

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

static bool run_save_roundtrip_smoke(void) {
    GameCanvas *test = (GameCanvas *)malloc(sizeof(GameCanvas));
    if (!test) return false;
    gc_fields_init(test);
    test->money = 123456;
    test->dayOfMonth = 17;
    test->monthOfYear = 8;
    test->year = 1973;
    gc_initSaveOrLoad(test, 4);
    int plain_size = test->snapshotSize;
    int8_t *plain = plain_size > 0 ? (int8_t *)malloc((size_t)plain_size) : NULL;
    if (plain) memcpy(plain, test->saveBuffer, (size_t)plain_size);
    gc_compressSaveBuffer(test);
    gc_decompressSaveBuffer(test);
    bool bytes_match = plain && test->snapshotSize == plain_size
                       && memcmp(test->saveBuffer, plain, (size_t)plain_size) == 0;
    test->money = 0;
    test->dayOfMonth = 0;
    test->monthOfYear = 0;
    test->year = 0;
    gc_initSaveOrLoad(test, 5);
    bool fields_match = test->money == 123456
                        && test->dayOfMonth == 17
                        && test->monthOfYear == 8
                        && test->year == 1973;
    free(plain);
    free(test);
    return bytes_match && fields_match;
}

static bool run_midi_parser_smoke(void) {
    return midp_media_validate_midi("/6");
}

static bool run_state30_smoke(void) {
    GameCanvas *test = (GameCanvas *)malloc(sizeof(GameCanvas));
    if (!test) return false;
    gc_fields_init(test);
    gc_start(test, NULL);

    // Skip the original five-second logo waits, but keep the real decode chain.
    test->loadingStartTime = 0;
    gc_update(test);
    test->loadingStartTime = 0;
    gc_update(test);
    gc_update(test);
    if (test->gameState != 20) { free(test); return false; }

    // Load the full tile sprite stream, then build the first scenario.
    gc_setState(test, 10);
    for (int step = 0; test->gameState == 10 && step < 1000; ++step)
        gc_update(test);
    if (test->gameState != 20) {
        midp_media_shutdown();
        free(test);
        return false;
    }
    test->activeScenario = 0;
    gc_setState(test, 25);
    for (int step = 0; test->gameState == 25 && step < 200000; ++step)
        gc_update(test);
    if (test->gameState != 30 || !test->tileMap || !test->trackMap) {
        midp_media_shutdown();
        free(test);
        return false;
    }

    MidpGraphics screen;
    midp_gfx_init_screen(&screen, g_framebuffer, MIDP_SCREEN_W, MIDP_SCREEN_H);
    gc_paint(test, &screen);
    gc_keyPressed(test, 54);
    gc_run(test);
    gc_paint(test, &screen);
    bool ok = test->gameState == 30 && test->backbuffer != NULL;
    midp_media_shutdown();
    free(test);
    return ok;
}

static bool run_midi_device_test(int total_millis) {
    if (total_millis < 200) total_millis = 200;
    GameCanvas *test = (GameCanvas *)malloc(sizeof(GameCanvas));
    if (!test) return false;
    gc_fields_init(test);
    test->autoMusic = true;
    test->gameStateCounter = 1;
    gc_loadMusic(test);
    if (!test->musicAvailable || !midp_media_is_loaded()) {
        fprintf(stderr, "MIDI load failed: %s\n", midp_media_last_error());
        free(test);
        return false;
    }
    gc_playMusic(test);
    if (!test->musicPlaying || !midp_media_is_playing()) {
        fprintf(stderr, "MIDI first play failed: %s\n",
                midp_media_last_error());
        midp_media_shutdown();
        free(test);
        return false;
    }
    int first_part = total_millis / 2;
    int64_t end = midp_current_time_millis() + first_part;
    while (midp_current_time_millis() < end) {
        midp_media_update();
        midp_sleep(10);
    }
    gc_stopMusic(test);
    if (test->musicPlaying || midp_media_is_playing()) {
        fprintf(stderr, "MIDI stop state failed\n");
        midp_media_shutdown();
        free(test);
        return false;
    }
    gc_setMusicVolume(test, 3);
    gc_playMusic(test);
    if (!test->musicPlaying || !midp_media_is_playing()) {
        fprintf(stderr, "MIDI second play failed: %s\n",
                midp_media_last_error());
        midp_media_shutdown();
        free(test);
        return false;
    }
    end = midp_current_time_millis() + total_millis - first_part;
    while (midp_current_time_millis() < end) {
        midp_media_update();
        midp_sleep(10);
    }
    gc_stopMusic(test);
    midp_media_shutdown();
    free(test);
    return true;
}

int main(void) {
    set_resource_root_from_executable();

    const char *midi_test = getenv("TRAIN_TYCOON_MIDI_TEST_MS");
    if (midi_test) return run_midi_device_test(atoi(midi_test)) ? 0 : 6;

    // Smoke mode (set by `port.py verify`): run N frames then exit cleanly.
    int smoke_frames = 0;
    const char *smoke = getenv("TRAIN_TYCOON_SMOKE");
    if (smoke) smoke_frames = atoi(smoke);
    if (smoke_frames && !run_pathfinding_smoke()) return 2;
    if (smoke_frames && !run_save_roundtrip_smoke()) return 3;
    if (smoke_frames && !run_midi_parser_smoke()) return 4;
    if (smoke_frames && !run_state30_smoke()) return 5;

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
        midp_media_update();
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

    midp_media_shutdown();
    free(gc);
    mfb_close();
    return 0;
}
