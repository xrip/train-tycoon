#include "support/input_queue.h"

#include "midp.h"

#include <windows.h>

namespace {

constexpr int kNokiaUp = -1;
constexpr int kNokiaDown = -2;
constexpr int kNokiaLeft = -3;
constexpr int kNokiaRight = -4;
constexpr int kNokiaFire = -5;

int to_midp_key_code(int virtual_key) {
    switch (virtual_key) {
        case VK_UP: return kNokiaUp;
        case VK_DOWN: return kNokiaDown;
        case VK_LEFT: return kNokiaLeft;
        case VK_RIGHT: return kNokiaRight;
        case VK_RETURN:
        case VK_SPACE: return kNokiaFire;
        case VK_BACK: return MIDP_KEY_SOFT_RIGHT;
        case VK_ESCAPE: return MIDP_KEY_SOFT_LEFT;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return MIDP_KEY_NUM0 + (virtual_key - '0');
        case VK_NUMPAD0: case VK_NUMPAD1: case VK_NUMPAD2: case VK_NUMPAD3:
        case VK_NUMPAD4: case VK_NUMPAD5: case VK_NUMPAD6: case VK_NUMPAD7:
        case VK_NUMPAD8: case VK_NUMPAD9:
            return MIDP_KEY_NUM0 + (virtual_key - VK_NUMPAD0);
        case VK_OEM_PLUS: return MIDP_KEY_POUND;
        case VK_OEM_MINUS: return MIDP_KEY_STAR;
        default: return 0;
    }
}

} // namespace

void InputQueue::on_virtual_key(int virtual_key, bool pressed) {
    if (virtual_key < 0 || virtual_key >= static_cast<int>(key_down_.size())) {
        return;
    }
    if (pressed == key_down_[virtual_key]) {
        return;
    }

    key_down_[virtual_key] = pressed;
    const int key_code = to_midp_key_code(virtual_key);
    if (key_code == 0) {
        return;
    }

    const std::size_t next = (write_index_ + 1) % kCapacity;
    if (next == read_index_) {
        return;
    }
    events_[write_index_] = KeyEvent{key_code, pressed};
    write_index_ = next;
}

bool InputQueue::poll(KeyEvent& event) {
    if (read_index_ == write_index_) {
        return false;
    }
    event = events_[read_index_];
    read_index_ = (read_index_ + 1) % kCapacity;
    return true;
}

void InputQueue::clear() {
    events_.fill({});
    key_down_.fill(false);
    read_index_ = 0;
    write_index_ = 0;
}
