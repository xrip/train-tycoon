// click_map.h - captured hit-rects for on-screen interactive elements whose
// geometry is not stored anywhere (overlay menu rows). The renderer fills this
// during game_paint; the pointer dispatch layer consumes it next frame.
//
// Bottom-bar items and the confirm-bar buttons are NOT captured here: their
// geometry is re-derived from stored state at click time (menu item x codes /
// fixed button positions). Only the overlay menu rows are computed transiently
// from packed-sprite data at draw time, so those are the ones we capture.
#ifndef TRAIN_TYCOON_CLICK_MAP_H
#define TRAIN_TYCOON_CLICK_MAP_H
#include <cstdint>

enum ClickKind : std::uint8_t {
    CLICK_NONE = 0,
    CLICK_MENU_ROW = 1,   // an overlay menu row; carries row index + option code
    CLICK_CLOSE = 2,      // the overlay's top-right X (close/back) button
};

struct ClickTarget {
    std::int16_t x = 0, y = 0, width = 0, height = 0;
    std::uint8_t kind = CLICK_NONE;
    std::int16_t row_index = 0;     // overlay row index (cursorDragY domain)
    std::int16_t option_code = 0;   // the row's action code (cursorDragX domain)
};

struct ClickMap {
    static constexpr std::size_t kCapacity = 48;

    ClickTarget targets[kCapacity]{};
    std::size_t count = 0;

    void clear() { count = 0; }

    void add_menu_row(int x, int y, int width, int height,
                      int row_index, int option_code) {
        if (count >= kCapacity) return;
        ClickTarget &t = targets[count++];
        t.x = static_cast<std::int16_t>(x);
        t.y = static_cast<std::int16_t>(y);
        t.width = static_cast<std::int16_t>(width);
        t.height = static_cast<std::int16_t>(height);
        t.kind = CLICK_MENU_ROW;
        t.row_index = static_cast<std::int16_t>(row_index);
        t.option_code = static_cast<std::int16_t>(option_code);
    }

    void add_close(int x, int y, int width, int height) {
        if (count >= kCapacity) return;
        ClickTarget &t = targets[count++];
        t.x = static_cast<std::int16_t>(x);
        t.y = static_cast<std::int16_t>(y);
        t.width = static_cast<std::int16_t>(width);
        t.height = static_cast<std::int16_t>(height);
        t.kind = CLICK_CLOSE;
    }

    // Hit-test for the topmost (last drawn) menu row containing (x, y). The
    // overlay is drawn back-to-front, so the last match is the visually top one.
    bool hit_menu_row(int x, int y, int *out_row_index, int *out_option_code) const {
        for (std::size_t target_index = count; target_index-- > 0;) {
            const ClickTarget &target = targets[target_index];
            if (target.kind != CLICK_MENU_ROW) continue;
            if (x >= target.x && x < target.x + target.width
                && y >= target.y && y < target.y + target.height) {
                if (out_row_index) *out_row_index = target.row_index;
                if (out_option_code) *out_option_code = target.option_code;
                return true;
            }
        }
        return false;
    }

    // True if (x, y) falls on the overlay close (X) button, if any was drawn.
    bool hit_close(int x, int y) const {
        for (std::size_t target_index = count; target_index-- > 0;) {
            const ClickTarget &target = targets[target_index];
            if (target.kind != CLICK_CLOSE) continue;
            if (x >= target.x && x < target.x + target.width
                && y >= target.y && y < target.y + target.height) return true;
        }
        return false;
    }
};

#endif  // TRAIN_TYCOON_CLICK_MAP_H
