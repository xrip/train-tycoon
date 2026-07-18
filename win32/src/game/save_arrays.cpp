// Save and load of game arrays and matrices.
#include "game.h"
#include "support/numeric.h"
#include <string.h>
#include <vector>

void SaveCodec::ensure_capacity(int32_t additional_bytes) {
    // Grow by additional_bytes, preserving existing bytes and zeroing the new tail.
    saveBuffer.resize(static_cast<std::size_t>(snapshotSize + additional_bytes));
    snapshotSize += additional_bytes;
}

void SaveCodec::io_bytes(int8_t *values, int count) {
    if (savePendingMode == save::MODE_SKIP) {
        for (int index = 0; index < count; ++index) values[index] = 0;
    } else if (savePendingMode == save::MODE_WRITE) {
        ensure_capacity(count);
        memcpy(saveBuffer.data() + saveReadOffset, values,
               static_cast<std::size_t>(count));
        saveReadOffset += count;
    } else if (snapshotLoaded) {
        memcpy(values, saveBuffer.data() + saveReadOffset,
               static_cast<std::size_t>(count));
        saveReadOffset += count;
    }
}

// Short array save/load over an explicit length.
void SaveCodec::io_shorts(int16_t *values, int count) {
    if (savePendingMode == save::MODE_SKIP) {
        for (int index = 0; index < count; ++index) values[index] = 0;
    } else if (savePendingMode == save::MODE_WRITE) {
        ensure_capacity(2 * count);
        for (int index = 0; index < count; ++index) {
            saveBuffer[saveReadOffset++] = static_cast<int8_t>(values[index] & 0xFF);
            saveBuffer[saveReadOffset++] = static_cast<int8_t>(values[index] >> 8 & 0xFF);
        }
    } else if (snapshotLoaded) {
        for (int index = 0; index < count; ++index) {
            uint16_t bits = (uint16_t)read_unsigned_byte();
            const uint16_t high = (uint16_t)read_unsigned_byte();
            bits |= (uint16_t)(high << 8);
            memcpy(&values[index], &bits, sizeof(values[index]));
        }
    }
}

// Int array save/load over an explicit length.
void SaveCodec::io_ints(int32_t *values, int count) {
    if (savePendingMode == save::MODE_SKIP) {
        for (int index = 0; index < count; ++index) values[index] = 0;
    } else if (savePendingMode == save::MODE_WRITE) {
        ensure_capacity(4 * count);
        for (int index = 0; index < count; ++index) {
            saveBuffer[saveReadOffset++] = static_cast<int8_t>(values[index] & 0xFF);
            saveBuffer[saveReadOffset++] = static_cast<int8_t>(values[index] >> 8 & 0xFF);
            saveBuffer[saveReadOffset++] = static_cast<int8_t>(values[index] >> 16 & 0xFF);
            saveBuffer[saveReadOffset++] = static_cast<int8_t>(values[index] >> 24 & 0xFF);
        }
    } else if (snapshotLoaded) {
        for (int index = 0; index < count; ++index) {
            uint32_t bits = (uint32_t)read_unsigned_byte();
            const uint32_t byte1 = (uint32_t)read_unsigned_byte();
            const uint32_t byte2 = (uint32_t)read_unsigned_byte();
            const uint32_t byte3 = (uint32_t)read_unsigned_byte();
            bits |= byte1 << 8;
            bits |= byte2 << 16;
            bits |= byte3 << 24;
            memcpy(&values[index], &bits, sizeof(values[index]));
        }
    }
}

void SaveCodec::io_byte_grid(Grid<int8_t>& grid) {
    for (auto& row : grid.storage()) {
        io_bytes(row.data(), static_cast<int>(row.size()));
    }
}

bool SaveCodec::load_main_snapshot() {
    load_slot(save::slot::Main, save::MODE_READ);
    return snapshotLoaded;
}
