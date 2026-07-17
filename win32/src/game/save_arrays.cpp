// Save and load of game arrays and matrices.
#include "game_internal.h"
#include "support/numeric.h"
#include <string.h>
#include <vector>

void SaveCodec::ensure_capacity(int32_t n) {
    // Grow by n, preserving existing bytes and zeroing the new tail.
    saveBuffer.resize(static_cast<std::size_t>(snapshotSize + n));
    snapshotSize += n;
}

void SaveCodec::io_bytes(int8_t *byArray, int len) {
    if (savePendingMode == save::MODE_SKIP) {
        for (int i = 0; i < len; ++i) byArray[i] = 0;
    } else if (savePendingMode == save::MODE_WRITE) {
        ensure_capacity(len);
        memcpy(saveBuffer.data() + saveReadOffset, byArray, (size_t)len);
        saveReadOffset += len;
    } else if (snapshotLoaded) {
        memcpy(byArray, saveBuffer.data() + saveReadOffset, (size_t)len);
        saveReadOffset += len;
    }
}

// Short array save/load over an explicit length.
void SaveCodec::io_shorts(int16_t *sArray, int len) {
    if (savePendingMode == save::MODE_SKIP) {
        for (int i = 0; i < len; ++i) sArray[i] = 0;
    } else if (savePendingMode == save::MODE_WRITE) {
        ensure_capacity(2 * len);
        for (int i = 0; i < len; ++i) {
            saveBuffer[saveReadOffset++] = (int8_t)(sArray[i] & 0xFF);
            saveBuffer[saveReadOffset++] = (int8_t)(sArray[i] >> 8 & 0xFF);
        }
    } else if (snapshotLoaded) {
        for (int i = 0; i < len; ++i) {
            uint16_t bits = (uint16_t)read_unsigned_byte();
            const uint16_t high = (uint16_t)read_unsigned_byte();
            bits |= (uint16_t)(high << 8);
            memcpy(&sArray[i], &bits, sizeof(sArray[i]));
        }
    }
}

// Int array save/load over an explicit length.
void SaveCodec::io_ints(int32_t *nArray, int len) {
    if (savePendingMode == save::MODE_SKIP) {
        for (int i = 0; i < len; ++i) nArray[i] = 0;
    } else if (savePendingMode == save::MODE_WRITE) {
        ensure_capacity(4 * len);
        for (int i = 0; i < len; ++i) {
            saveBuffer[saveReadOffset++] = (int8_t)(nArray[i] & 0xFF);
            saveBuffer[saveReadOffset++] = (int8_t)(nArray[i] >> 8 & 0xFF);
            saveBuffer[saveReadOffset++] = (int8_t)(nArray[i] >> 16 & 0xFF);
            saveBuffer[saveReadOffset++] = (int8_t)(nArray[i] >> 24 & 0xFF);
        }
    } else if (snapshotLoaded) {
        for (int i = 0; i < len; ++i) {
            uint32_t bits = (uint32_t)read_unsigned_byte();
            const uint32_t byte1 = (uint32_t)read_unsigned_byte();
            const uint32_t byte2 = (uint32_t)read_unsigned_byte();
            const uint32_t byte3 = (uint32_t)read_unsigned_byte();
            bits |= byte1 << 8;
            bits |= byte2 << 16;
            bits |= byte3 << 24;
            memcpy(&nArray[i], &bits, sizeof(nArray[i]));
        }
    }
}

void SaveCodec::io_byte_grid(Grid<int8_t>& grid) {
    for (auto& row : grid.storage()) {
        io_bytes(row.data(), static_cast<int>(row.size()));
    }
}

bool SaveCodec::load_main_snapshot() {
    load_slot(0, save::MODE_READ);
    return snapshotLoaded;
}
