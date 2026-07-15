#ifndef PLATFORM_WINMM_MUSIC_PLAYER_H
#define PLATFORM_WINMM_MUSIC_PLAYER_H

#include "support/music_output.h"

#include <cstdint>
#include <string>

class WinMmMusicPlayer final : public MusicOutput {
public:
    ~WinMmMusicPlayer() override;

    bool load(const std::filesystem::path& path) override;
    bool play() override;
    void stop() override;
    void set_volume(int level) override;
    void update() override;

    [[nodiscard]] bool is_loaded() const override { return loaded_; }
    [[nodiscard]] bool is_playing() const override { return playing_; }
    [[nodiscard]] std::string_view last_error() const override { return error_; }

private:
    bool set_device_volume(std::uint32_t volume, bool save_current);
    bool send_mci(const char* operation, const char* command,
                  char* result = nullptr, unsigned int result_size = 0);
    bool start_from_zero();
    void shutdown();

    std::uint32_t saved_volume_ = 0;
    bool saved_volume_valid_ = false;
    bool loaded_ = false;
    bool playing_ = false;
    std::int64_t next_status_check_ = 0;
    std::string error_;
};

#endif
