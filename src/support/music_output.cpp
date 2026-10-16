#include "support/music_output.h"

#include <algorithm>
#include <cstring>

bool is_valid_midi(std::span<const std::uint8_t> data) {
    return data.size() >= 14 && std::memcmp(data.data(), "MThd", 4) == 0 &&
           data[8] == 0 && data[9] <= 1 &&
           (data[10] != 0 || data[11] != 0) &&
           (data[12] & 0x80) == 0 &&
           (data[12] != 0 || data[13] != 0);
}

bool SilentMusicOutput::load(const std::filesystem::path& path) {
    playing_ = false;
    loaded_ = std::filesystem::is_regular_file(path);
    error_ = loaded_ ? "" : "MIDI resource open failed";
    return loaded_;
}

bool SilentMusicOutput::play() {
    if (!loaded_) {
        error_ = "MIDI music is not loaded";
        return false;
    }
    playing_ = true;
    ++play_count_;
    return true;
}

void SilentMusicOutput::stop() {
    playing_ = false;
    ++stop_count_;
}

void SilentMusicOutput::set_volume(int level) {
    volume_ = std::clamp(level, 0, 100);
}
