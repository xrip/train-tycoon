// midp_input.cpp - Win32 virtual-key -> MIDP key-code translation + a small event
// queue + getGameAction. The target handset was a Nokia N73/N95, so we adopt the
// Nokia key-code convention for the D-pad (up=-1 .. select=-5) and the standard
// numeric key codes for the phone keypad mapped to the number-row keys.
#include "midp_input.h"
#include "midp.h"
#include "support/input_queue.h"

// Nokia-style navigation key codes.
namespace nokia_key {
constexpr int Up = -1;
constexpr int Down = -2;
constexpr int Left = -3;
constexpr int Right = -4;
constexpr int Fire = -5;
}  // namespace nokia_key

// Number-row keys that act as the phone D-pad. The source proves the keypad
// positions, but they are still host key codes rather than game directions.
namespace keypad_key {
constexpr int Up = MIDP_KEY_NUM0 + 2;
constexpr int Down = MIDP_KEY_NUM0 + 8;
constexpr int Left = MIDP_KEY_NUM0 + 4;
constexpr int Right = MIDP_KEY_NUM0 + 6;
constexpr int Fire = MIDP_KEY_NUM0 + 5;
}  // namespace keypad_key

int midp_get_game_action(int keycode) {
    switch (keycode) {
        case nokia_key::Up:    case keypad_key::Up: return MIDP_UP;
        case nokia_key::Down:  case keypad_key::Down: return MIDP_DOWN;
        case nokia_key::Left:  case keypad_key::Left: return MIDP_LEFT_ACTION;
        case nokia_key::Right: case keypad_key::Right: return MIDP_RIGHT_ACTION;
        case nokia_key::Fire:  case keypad_key::Fire: return MIDP_FIRE;
        default: return 0;
    }
}

static InputQueue input_queue;

void midp_input_on_key(int virtual_key, bool is_down) {
    input_queue.on_virtual_key(virtual_key, is_down);
}

bool midp_input_poll(int *keycode, bool *pressed) {
    KeyEvent event;
    if (!input_queue.poll(event)) return false;
    if (keycode) *keycode = event.key_code;
    if (pressed) *pressed = event.pressed;
    return true;
}
