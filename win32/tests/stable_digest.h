#ifndef TRAIN_TYCOON_TESTS_STABLE_DIGEST_H
#define TRAIN_TYCOON_TESTS_STABLE_DIGEST_H

#include <cstddef>
#include <cstdint>

class StableDigest {
public:
    void add_byte(std::uint8_t value) {
        value_ ^= value;
        value_ *= 1099511628211ULL;
    }

    void add_i32(std::int32_t value) {
        const auto bits = static_cast<std::uint32_t>(value);
        add_byte(static_cast<std::uint8_t>(bits));
        add_byte(static_cast<std::uint8_t>(bits >> 8));
        add_byte(static_cast<std::uint8_t>(bits >> 16));
        add_byte(static_cast<std::uint8_t>(bits >> 24));
    }

    void add_bytes(const void* data, std::size_t size) {
        const auto* bytes = static_cast<const std::uint8_t*>(data);
        for (std::size_t index = 0; index < size; ++index) {
            add_byte(bytes[index]);
        }
    }

    void add_pixels(const std::uint32_t* pixels, std::size_t count) {
        for (std::size_t index = 0; index < count; ++index) {
            add_i32(static_cast<std::int32_t>(pixels[index]));
        }
    }

    [[nodiscard]] std::uint64_t value() const { return value_; }

private:
    std::uint64_t value_ = 14695981039346656037ULL;
};

#endif
