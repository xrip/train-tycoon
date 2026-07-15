#ifndef TRAIN_TYCOON_SUPPORT_INPUT_QUEUE_H
#define TRAIN_TYCOON_SUPPORT_INPUT_QUEUE_H

#include <array>
#include <cstddef>

struct KeyEvent {
    int key_code = 0;
    bool pressed = false;
};

class InputQueue {
public:
    void on_virtual_key(int virtual_key, bool pressed);
    [[nodiscard]] bool poll(KeyEvent& event);
    void clear();

private:
    static constexpr std::size_t kCapacity = 64;

    std::array<KeyEvent, kCapacity> events_{};
    std::array<bool, 256> key_down_{};
    std::size_t read_index_ = 0;
    std::size_t write_index_ = 0;
};

#endif
