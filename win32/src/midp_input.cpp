// midp_input.c - Win32 virtual-key -> MIDP key-code translation + a small event
// queue + getGameAction. The target handset was a Nokia N73/N95, so we adopt the
// Nokia key-code convention for the D-pad (up=-1 .. select=-5) and the standard
// numeric key codes for the phone keypad mapped to the number-row keys.
#include "midp_input.h"
#include "midp.h"
#include <windows.h>

// Nokia-style navigation key codes.
#define NK_UP    (-1)
#define NK_DOWN  (-2)
#define NK_LEFT  (-3)
#define NK_RIGHT (-4)
#define NK_FIRE  (-5)

// Translate a Win32 VK to a MIDP key code, or 0 if unmapped.
static int vk_to_keycode(int vk) {
    switch (vk) {
        case VK_UP:    return NK_UP;
        case VK_DOWN:  return NK_DOWN;
        case VK_LEFT:  return NK_LEFT;
        case VK_RIGHT: return NK_RIGHT;
        case VK_RETURN:
        case VK_SPACE: return NK_FIRE;
        case VK_BACK:  return MIDP_KEY_SOFT_RIGHT; // "back"/clear
        case VK_ESCAPE:return MIDP_KEY_SOFT_LEFT;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return MIDP_KEY_NUM0 + (vk - '0'); // 48..57
        case VK_NUMPAD0: case VK_NUMPAD1: case VK_NUMPAD2: case VK_NUMPAD3:
        case VK_NUMPAD4: case VK_NUMPAD5: case VK_NUMPAD6: case VK_NUMPAD7:
        case VK_NUMPAD8: case VK_NUMPAD9:
            return MIDP_KEY_NUM0 + (vk - VK_NUMPAD0);
        case VK_OEM_PLUS: return MIDP_KEY_POUND; // '#'
        case VK_OEM_MINUS:return MIDP_KEY_STAR;  // '*'
        default: return 0;
    }
}

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

// Simple ring buffer of key events.
#define QCAP 64
typedef struct { int keycode; bool pressed; } KeyEvent;
static KeyEvent q[QCAP];
static int q_read = 0, q_write = 0;
static bool key_down[256]; // debounce auto-repeat per VK

void midp_input_on_key(int vk, bool down) {
    if (vk < 0 || vk > 255) return;
    if (down && key_down[vk]) return;   // ignore auto-repeat
    if (down == key_down[vk]) return;   // no state change
    key_down[vk] = down;
    int kc = vk_to_keycode(vk);
    if (kc == 0) return;
    int next = (q_write + 1) % QCAP;
    if (next == q_read) return; // queue full, drop
    q[q_write].keycode = kc;
    q[q_write].pressed = down;
    q_write = next;
}

bool midp_input_poll(int *keycode, bool *pressed) {
    if (q_read == q_write) return false;
    if (keycode) *keycode = q[q_read].keycode;
    if (pressed) *pressed = q[q_read].pressed;
    q_read = (q_read + 1) % QCAP;
    return true;
}
