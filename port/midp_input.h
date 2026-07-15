// midp_input.h - keyboard event bridge from Win32 (MiniFB WndProc) to the
// MIDP Canvas model. WndProc pushes raw virtual keys via midp_input_on_key();
// the game loop drains them as (keyCode, pressed) and feeds keyPressed/Released.
#ifndef MIDP_INPUT_H
#define MIDP_INPUT_H

#include <stdbool.h>

// Called from the MiniFB input hook. `vk` is a Win32 virtual-key code.
void midp_input_on_key(int vk, bool down);

// Drain one queued event. Returns false when the queue is empty.
bool midp_input_poll(int *keycode, bool *pressed);

// Canvas.getGameAction: map a MIDP key code to a game action (0 if none).
int midp_get_game_action(int keycode);

#endif // MIDP_INPUT_H
