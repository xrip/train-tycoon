// main.cpp - Win32 entry point and game frame loop. The loop owns the timing and
// drives the Game each frame: poll input, game_run (one update step), then
// game_paint (render). The Game and its parts hold no Win32 or window state.
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
#include "game.h"
#include "platform/winmm_music_player.h"
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
    Game *test = new Game{};
    game_fields_init(test);
    test->music_output = std::make_unique<WinMmMusicPlayer>();
    MusicOutput* output = test->music_output.get();
    test->music_enabled = true;
    test->music_volume_level = 1;
    game_loadMusic(test);
    if (!test->music_available || !output->is_loaded()) {
        const std::string_view error = output->last_error();
        fprintf(stderr, "MIDI load failed: %.*s\n",
                static_cast<int>(error.size()), error.data());
        delete test;
        return false;
    }
    game_playMusic(test);
    if (!test->music_playing || !output->is_playing()) {
        const std::string_view error = output->last_error();
        fprintf(stderr, "MIDI first play failed: %.*s\n",
                static_cast<int>(error.size()), error.data());
        delete test;
        return false;
    }
    int first_part = total_millis / 2;
    int64_t end = midp_current_time_millis() + first_part;
    while (midp_current_time_millis() < end) {
        output->update();
        midp_sleep(10);
    }
    game_stopMusic(test);
    if (test->music_playing || output->is_playing()) {
        fprintf(stderr, "MIDI stop state failed\n");
        delete test;
        return false;
    }
    game_setMusicVolume(test, 3);
    game_playMusic(test);
    if (!test->music_playing || !output->is_playing()) {
        const std::string_view error = output->last_error();
        fprintf(stderr, "MIDI second play failed: %.*s\n",
                static_cast<int>(error.size()), error.data());
        delete test;
        return false;
    }
    end = midp_current_time_millis() + total_millis - first_part;
    while (midp_current_time_millis() < end) {
        output->update();
        midp_sleep(10);
    }
    game_stopMusic(test);
    delete test;
    return true;
}

int main(void) {
    set_resource_root_from_executable();

    const char *midi_test = getenv("TRAIN_TYCOON_MIDI_TEST_MS");
    if (midi_test) return run_midi_device_test(atoi(midi_test)) ? 0 : 6;

    if (!mfb_open("Train Tycoon", MIDP_SCREEN_W, MIDP_SCREEN_H, WIN_SCALE, 32))
        return 1;

    Game *game = new Game{};
    game_fields_init(game);
    game->music_output = std::make_unique<WinMmMusicPlayer>();
    game_start(game, NULL); // resource load + initial state (no thread spawn)

    Surface screen;
    midp_gfx_init_screen(&screen, g_framebuffer, MIDP_SCREEN_W, MIDP_SCREEN_H);

    for (;;) {
        game->music_output->update();
        int keycode; bool pressed;
        while (midp_input_poll(&keycode, &pressed)) {
            if (pressed) game_keyPressed(game, keycode);
            else         game_keyReleased(game, keycode);
        }

        // Mouse: a left-click is forwarded as a pointer press. The game's pointer
        // dispatch classifies it against the current screen (bottom-bar items,
        // overlay menu rows, confirm buttons) using the hit-rects captured during
        // the previous frame's paint.
        int click_x, click_y;
        if (mfb_poll_lclick(&click_x, &click_y))
            game_pointerPressed(game, click_x, click_y);

        game_run(game);           // one iteration of the original run() loop
        game_paint(game, &screen);

        if (!mfb_update(g_framebuffer, TARGET_FPS))
            break;
    }

    delete game;
    mfb_close();
    return 0;
}
