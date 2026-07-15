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
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4244) // MiniFB stores a Win32 BOOL in a char.
#endif
#include "MiniFB.h"
#ifdef _MSC_VER
#  pragma warning(pop)
#endif

#include "midp.h"
#include "game_state.h"
#include "game_methods.h"
#include "support/resource_reader.h"
#include <filesystem>
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
        resources::set_root("resources");
        return;
    }
    resources::set_root(std::filesystem::path(path).parent_path() / "resources");
}

static bool run_midi_device_test(int total_millis) {
    if (total_millis < 200) total_millis = 200;
    GameCanvas *test = new GameCanvas{};
    gc_fields_init(test);
    test->autoMusic = true;
    test->gameStateCounter = 1;
    gc_loadMusic(test);
    if (!test->musicAvailable || !midp_media_is_loaded()) {
        fprintf(stderr, "MIDI load failed: %s\n", midp_media_last_error());
        delete test;
        return false;
    }
    gc_playMusic(test);
    if (!test->musicPlaying || !midp_media_is_playing()) {
        fprintf(stderr, "MIDI first play failed: %s\n",
                midp_media_last_error());
        midp_media_shutdown();
        delete test;
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
        delete test;
        return false;
    }
    gc_setMusicVolume(test, 3);
    gc_playMusic(test);
    if (!test->musicPlaying || !midp_media_is_playing()) {
        fprintf(stderr, "MIDI second play failed: %s\n",
                midp_media_last_error());
        midp_media_shutdown();
        delete test;
        return false;
    }
    end = midp_current_time_millis() + total_millis - first_part;
    while (midp_current_time_millis() < end) {
        midp_media_update();
        midp_sleep(10);
    }
    gc_stopMusic(test);
    midp_media_shutdown();
    delete test;
    return true;
}

int main(void) {
    set_resource_root_from_executable();

    const char *midi_test = getenv("TRAIN_TYCOON_MIDI_TEST_MS");
    if (midi_test) return run_midi_device_test(atoi(midi_test)) ? 0 : 6;

    if (!mfb_open("Train Tycoon", MIDP_SCREEN_W, MIDP_SCREEN_H, WIN_SCALE, 32))
        return 1;

    GameCanvas *gc = new GameCanvas{};
    gc_fields_init(gc);
    gc_start(gc, NULL); // resource load + initial state (no thread spawn)

    Surface screen;
    midp_gfx_init_screen(&screen, g_framebuffer, MIDP_SCREEN_W, MIDP_SCREEN_H);

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
    }

    midp_media_shutdown();
    delete gc;
    mfb_close();
    return 0;
}
