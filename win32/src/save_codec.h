// SaveCodec: the working state of the save/load codec (see game/save.cpp). It
// holds the serialization buffer and cursor plus the snapshot bookkeeping; the
// codec preserves the legacy .rms byte order.
#ifndef SAVE_CODEC_H
#define SAVE_CODEC_H
#include "support/grid.h"
#include <cstdint>
#include <vector>

struct Game;

// SaveCodec owns the transient codec state: the byte buffer being written or
// read, the read cursor, the target slot and pending mode, the snapshot size and
// loaded flag, and the compressed/uncompressed byte counts. None of these are
// part of the saved data; the on-disk byte order is unchanged.
class SaveCodec {
public:
    void load_slot(int32_t slot, int32_t mode);
    void persist() const;

    bool io_bool(bool value);
    int8_t io_byte(int8_t value);
    int32_t io_int(int32_t value);
    void io_bytes(int8_t* values, int count);
    void io_shorts(int16_t* values, int count);
    void io_ints(int32_t* values, int count);
    void io_byte_grid(Grid<int8_t>& grid);

    void compress();
    void decompress();
    void walk(Game& game, int32_t operation);
    void walk_settings(Game& game);
    void save_all(Game& game);
    void restore_all(Game& game);
    bool load_main_snapshot();

    std::vector<int8_t> saveBuffer;
    int32_t saveReadOffset = 0;
    int32_t saveSlot = 0;
    int32_t savePendingMode = 0;
    int32_t snapshotSize = 0;
    bool snapshotLoaded = false;
    int32_t bytesUncompressed = 0;
    int32_t bytesCompressed = 0;

private:
    void ensure_capacity(int32_t count);
    int32_t read_unsigned_byte();
};

#endif  // SAVE_CODEC_H
