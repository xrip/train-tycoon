// SaveCodec: the working state of the save/load codec (see game/save.cpp). It
// holds the serialization buffer and cursor plus the snapshot bookkeeping; the
// codec preserves the legacy .rms byte order.
#ifndef SAVE_CODEC_H
#define SAVE_CODEC_H
#include <cstdint>
#include <vector>

// SaveCodec owns the transient codec state: the byte buffer being written or
// read, the read cursor, the target slot and pending mode, the snapshot size and
// loaded flag, and the compressed/uncompressed byte counts. None of these are
// part of the saved data; the on-disk byte order is unchanged.
struct SaveCodec {
    std::vector<int8_t> saveBuffer;
    int32_t saveReadOffset = 0;
    int32_t saveSlot = 0;
    int32_t savePendingMode = 0;
    int32_t snapshotSize = 0;
    bool snapshotLoaded = false;
    int32_t bytesUncompressed = 0;
    int32_t bytesCompressed = 0;
};

#endif  // SAVE_CODEC_H
