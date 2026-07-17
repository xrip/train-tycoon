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
    test->music.output = std::make_unique<WinMmMusicPlayer>();
    MusicOutput* output = test->music.output.get();
    test->music.enabled = true;
    test->music.volumeLevel = 1;
    game_loadMusic(test);
    if (!test->music.available || !output->is_loaded()) {
        const std::string_view error = output->last_error();
        fprintf(stderr, "MIDI load failed: %.*s\n",
                static_cast<int>(error.size()), error.data());
        delete test;
        return false;
    }
    game_playMusic(test);
    if (!test->music.playing || !output->is_playing()) {
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
    if (test->music.playing || output->is_playing()) {
        fprintf(stderr, "MIDI stop state failed\n");
        delete test;
        return false;
    }
    game_setMusicVolume(test, 3);
    game_playMusic(test);
    if (!test->music.playing || !output->is_playing()) {
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
    game->music.output = std::make_unique<WinMmMusicPlayer>();
    game_start(game); // resource load + initial state (no thread spawn)

    Surface screen;
    midp_gfx_init_screen(&screen, g_framebuffer, MIDP_SCREEN_W, MIDP_SCREEN_H);

    for (;;) {
        game->music.update();
        int keycode; bool pressed;
        while (midp_input_poll(&keycode, &pressed)) {
            if (pressed) game_keyPressed(game, keycode);
            else         game_keyReleased(game, keycode);
        }

        // Mouse position (framebuffer pixels); valid while the pointer is inside
        // the window. Polled up front so both the click handler and the post-run
        // hover (below) use the same sample.
        int mouse_x, mouse_y;
        const bool mouse_inside = mfb_mousepos(&mouse_x, &mouse_y);

        // Mouse: a left-click is forwarded as a pointer press. The game's pointer
        // dispatch classifies it against the current screen (game-field tile,
        // bottom-bar items, overlay menu rows, confirm buttons).
        int click_x, click_y;
        if (mfb_poll_lclick(&click_x, &click_y))
            game_pointerPressed(game, click_x, click_y);

        // Edge-scroll: pan the map when the mouse sits near a viewport edge.
        // Applied before game_run so game_viewportZoomKey (inside run) clamps
        // the advance to the map and keeps viewportBase in lockstep with the
        // camera (see game_pointerEdgeScroll).
        if (mouse_inside)
            game_pointerEdgeScroll(game, mouse_x, mouse_y);

        game_run(game);           // one iteration of the original run() loop

        // Apply the mouse hover AFTER game_run. The Playing run loop re-anchors the
        // tile cursor to the screen centre every tick (game_viewportZoomKey ->
        // game_computeViewportBounds(viewportBase)), so applying it before run()
        // gets clobbered and the visible cursor never leaves centre. Re-applying it
        // here, right before paint, is what locks the game cursor to the mouse.
        // (Clicks still resolve correctly either way -- pointerPressed resolves and
        // consumes the tile inline, before run() can reset it.)
        if (mouse_inside)
            game_pointerMoved(game, mouse_x, mouse_y);
        else
            game->ui.mouseOverField = false;

        game_paint(game, &screen);

        if (!mfb_update(g_framebuffer, TARGET_FPS))
            break;
    }

    delete game;
    mfb_close();
    return 0;
}
