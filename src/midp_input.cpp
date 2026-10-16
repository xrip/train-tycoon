// midp_input.cpp - Win32 virtual-key -> MIDP key-code translation + a small event
// queue + getGameAction. The target handset was a Nokia N73/N95, so we adopt the
// Nokia key-code convention for the D-pad (up=-1 .. select=-5) and the standard
// numeric key codes for the phone keypad mapped to the number-row keys.
#include "midp_input.h"
#include "midp.h"
#include "support/input_queue.h"

// Nokia-style navigation key codes.
#define NK_UP    (-1)
#define NK_DOWN  (-2)
#define NK_LEFT  (-3)
#define NK_RIGHT (-4)
#define NK_FIRE  (-5)

int midp_get_game_action(int keycode) {
    switch (keycode) {
        case NK_UP:    case MIDP_KEY_NUM0 + 2: return MIDP_UP;
        case NK_DOWN:  case MIDP_KEY_NUM0 + 8: return MIDP_DOWN;
        case NK_LEFT:  case MIDP_KEY_NUM0 + 4: return MIDP_LEFT_ACTION;
        case NK_RIGHT: case MIDP_KEY_NUM0 + 6: return MIDP_RIGHT_ACTION;
        case NK_FIRE:  case MIDP_KEY_NUM0 + 5: return MIDP_FIRE;
        default: return 0;
    }
}

static InputQueue input_queue;

void midp_input_on_key(int vk, bool down) {
    input_queue.on_virtual_key(vk, down);
}

bool midp_input_poll(int *keycode, bool *pressed) {
    KeyEvent event;
    if (!input_queue.poll(event)) return false;
    if (keycode) *keycode = event.key_code;
    if (pressed) *pressed = event.pressed;
    return true;
}
