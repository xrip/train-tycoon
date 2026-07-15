#ifndef TRAIN_TYCOON_SUPPORT_BYTE_BUFFER_H
#define TRAIN_TYCOON_SUPPORT_BYTE_BUFFER_H

#include <cstdint>
#include <span>
#include <vector>

class ByteBuffer {
public:
    void write(int value) {
        bytes_.push_back(static_cast<std::uint8_t>(value & 0xff));
    }

    void clear() { bytes_.clear(); }

    [[nodiscard]] std::span<const std::uint8_t> bytes() const {
        return bytes_;
    }

private:
    std::vector<std::uint8_t> bytes_;
};

#endif
