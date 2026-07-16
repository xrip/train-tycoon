// Save and load of game arrays and matrices.
#include "game_internal.h"
#include <string.h>
#include <vector>

void game_ensureSaveBufferCapacity(Game *game, int32_t n) {
    // Grow by n, preserving existing bytes and zeroing the new tail.
    game->saveCodec.saveBuffer.resize(static_cast<std::size_t>(game->saveCodec.snapshotSize + n));
    game->saveCodec.snapshotSize += n;
}

void game_saveOrLoadBytes(Game *game, int8_t *byArray, int len) {
    if (game->saveCodec.savePendingMode == save::MODE_SKIP) {
        for (int i = 0; i < len; ++i) byArray[i] = 0;
    } else if (game->saveCodec.savePendingMode == save::MODE_WRITE) {
        game_ensureSaveBufferCapacity(game, len);
        memcpy(game->saveCodec.saveBuffer.data() + game->saveCodec.saveReadOffset, byArray, (size_t)len);
        game->saveCodec.saveReadOffset += len;
    } else if (game->saveCodec.snapshotLoaded) {
        memcpy(byArray, game->saveCodec.saveBuffer.data() + game->saveCodec.saveReadOffset, (size_t)len);
        game->saveCodec.saveReadOffset += len;
    }
}

// Short array save/load over an explicit length.
void game_saveOrLoadShorts(Game *game, int16_t *sArray, int len) {
    if (game->saveCodec.savePendingMode == save::MODE_SKIP) {
        for (int i = 0; i < len; ++i) sArray[i] = 0;
    } else if (game->saveCodec.savePendingMode == save::MODE_WRITE) {
        game_ensureSaveBufferCapacity(game, 2 * len);
        for (int i = 0; i < len; ++i) {
            game->saveCodec.saveBuffer[game->saveCodec.saveReadOffset++] = (int8_t)(sArray[i] & 0xFF);
            game->saveCodec.saveBuffer[game->saveCodec.saveReadOffset++] = (int8_t)(sArray[i] >> 8 & 0xFF);
        }
    } else if (game->saveCodec.snapshotLoaded) {
        for (int i = 0; i < len; ++i) {
            uint16_t bits = (uint16_t)game_readSaveByteUnsigned(game);
            bits |= (uint16_t)((uint16_t)game_readSaveByteUnsigned(game) << 8);
            memcpy(&sArray[i], &bits, sizeof(sArray[i]));
        }
    }
}

// Int array save/load over an explicit length.
void game_saveOrLoadInts(Game *game, int32_t *nArray, int len) {
    if (game->saveCodec.savePendingMode == save::MODE_SKIP) {
        for (int i = 0; i < len; ++i) nArray[i] = 0;
    } else if (game->saveCodec.savePendingMode == save::MODE_WRITE) {
        game_ensureSaveBufferCapacity(game, 4 * len);
        for (int i = 0; i < len; ++i) {
            game->saveCodec.saveBuffer[game->saveCodec.saveReadOffset++] = (int8_t)(nArray[i] & 0xFF);
            game->saveCodec.saveBuffer[game->saveCodec.saveReadOffset++] = (int8_t)(nArray[i] >> 8 & 0xFF);
            game->saveCodec.saveBuffer[game->saveCodec.saveReadOffset++] = (int8_t)(nArray[i] >> 16 & 0xFF);
            game->saveCodec.saveBuffer[game->saveCodec.saveReadOffset++] = (int8_t)(nArray[i] >> 24 & 0xFF);
        }
    } else if (game->saveCodec.snapshotLoaded) {
        for (int i = 0; i < len; ++i) {
            uint32_t bits = (uint32_t)game_readSaveByteUnsigned(game);
            bits |= (uint32_t)game_readSaveByteUnsigned(game) << 8;
            bits |= (uint32_t)game_readSaveByteUnsigned(game) << 16;
            bits |= (uint32_t)game_readSaveByteUnsigned(game) << 24;
            memcpy(&nArray[i], &bits, sizeof(nArray[i]));
        }
    }
}

void game_saveOrLoadByteGrid(Game *game, Grid<int8_t>& grid) {
    for (auto& row : grid.storage()) {
        game_saveOrLoadBytes(game, row.data(), static_cast<int>(row.size()));
    }
}

bool game_loadSnapshotAndReturn(Game *game) {
    game_loadGame(game, 0, save::MODE_READ);
    return game->saveCodec.snapshotLoaded;
}
