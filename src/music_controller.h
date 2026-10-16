#ifndef MUSIC_CONTROLLER_H
#define MUSIC_CONTROLLER_H

#include "support/music_output.h"
#include <cstdint>
#include <filesystem>
#include <memory>

class MusicController {
public:
    void load(const std::filesystem::path& path);
    void play();
    void stop();
    void set_volume(int32_t level);
    void update();

    bool playing = false;
    bool available = false;
    bool autoStart = false;
    bool enabled = false;
    int32_t volumeLevel = 2;
    std::unique_ptr<MusicOutput> output;
};

#endif  // MUSIC_CONTROLLER_H
