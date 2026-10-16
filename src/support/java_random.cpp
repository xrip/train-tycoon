#include "support/java_random.h"

#include <bit>
#include <type_traits>

namespace {

constexpr std::uint64_t kMultiplier = 0x5deece66dULL;
constexpr std::uint64_t kAddend = 0xbULL;
constexpr std::uint64_t kMask = (1ULL << 48) - 1;

} // namespace

static_assert(std::is_trivially_copyable_v<JavaRandom>);

void JavaRandom::set_seed(std::int64_t seed) {
    seed_ = (static_cast<std::uint64_t>(seed) ^ kMultiplier) & kMask;
}

std::uint32_t JavaRandom::next_bits(int count) {
    seed_ = (seed_ * kMultiplier + kAddend) & kMask;
    return static_cast<std::uint32_t>(seed_ >> (48 - count));
}

std::int32_t JavaRandom::next_int() {
    return std::bit_cast<std::int32_t>(next_bits(32));
}

std::int32_t JavaRandom::next_int(std::int32_t bound) {
    if (bound <= 0) {
        return 0;
    }

    if ((bound & -bound) == bound) {
        return static_cast<std::int32_t>(
            (static_cast<std::int64_t>(bound) * next_bits(31)) >> 31);
    }

    std::uint32_t bits;
    std::int32_t value;
    do {
        bits = next_bits(31);
        value = static_cast<std::int32_t>(bits % static_cast<std::uint32_t>(bound));
        const std::uint32_t java_sum =
            bits - static_cast<std::uint32_t>(value)
            + static_cast<std::uint32_t>(bound - 1);
        if ((java_sum & 0x80000000U) == 0) {
            return value;
        }
    } while (true);
}
