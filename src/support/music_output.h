#ifndef SUPPORT_MUSIC_OUTPUT_H
#define SUPPORT_MUSIC_OUTPUT_H

#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>

bool is_valid_midi(std::span<const std::uint8_t> data);

class MusicOutput {
public:
    virtual ~MusicOutput() = default;

    virtual bool load(const std::filesystem::path& path) = 0;
    virtual bool play() = 0;
    virtual void stop() = 0;
    virtual void set_volume(int level) = 0;
    virtual void update() = 0;

    [[nodiscard]] virtual bool is_loaded() const = 0;
    [[nodiscard]] virtual bool is_playing() const = 0;
    [[nodiscard]] virtual std::string_view last_error() const = 0;
};

class SilentMusicOutput final : public MusicOutput {
public:
    bool load(const std::filesystem::path& path) override;
    bool play() override;
    void stop() override;
    void set_volume(int level) override;
    void update() override {}

    [[nodiscard]] bool is_loaded() const override { return loaded_; }
    [[nodiscard]] bool is_playing() const override { return playing_; }
    [[nodiscard]] std::string_view last_error() const override { return error_; }
    [[nodiscard]] int volume() const { return volume_; }
    [[nodiscard]] int play_count() const { return play_count_; }
    [[nodiscard]] int stop_count() const { return stop_count_; }

private:
    bool loaded_ = false;
    bool playing_ = false;
    int volume_ = 100;
    int play_count_ = 0;
    int stop_count_ = 0;
    std::string error_;
};

#endif
