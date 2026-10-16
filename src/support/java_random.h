#ifndef TRAIN_TYCOON_SUPPORT_JAVA_RANDOM_H
#define TRAIN_TYCOON_SUPPORT_JAVA_RANDOM_H

#include <cstdint>

class JavaRandom {
public:
    void set_seed(std::int64_t seed);
    [[nodiscard]] std::int32_t next_int();
    [[nodiscard]] std::int32_t next_int(std::int32_t bound);

private:
    [[nodiscard]] std::uint32_t next_bits(int count);

    std::uint64_t seed_;
};

#endif
