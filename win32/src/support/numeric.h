#ifndef SUPPORT_NUMERIC_H
#define SUPPORT_NUMERIC_H

#include <cstdint>

constexpr int32_t unsigned_byte(int8_t value) {
    return static_cast<uint8_t>(value);
}

#endif  // SUPPORT_NUMERIC_H
