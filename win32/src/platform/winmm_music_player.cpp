#include "platform/winmm_music_player.h"

#include "midp.h"

#include <windows.h>
#include <mmsystem.h>

#include <algorithm>
#include <cstdio>
#include <cstring>

namespace {

constexpr char kMusicAlias[] = "train_tycoon_music";

} // namespace

WinMmMusicPlayer::~WinMmMusicPlayer() {
    shutdown();
}

bool WinMmMusicPlayer::set_device_volume(std::uint32_t volume,
                                          bool save_current) {
    HMIDIOUT output = nullptr;
    if (midiOutOpen(&output, MIDI_MAPPER, 0, 0, CALLBACK_NULL) !=
        MMSYSERR_NOERROR) {
        return false;
    }
    if (save_current && !saved_volume_valid_) {
        DWORD saved_volume = 0;
        saved_volume_valid_ =
            midiOutGetVolume(output, &saved_volume) == MMSYSERR_NOERROR;
        if (saved_volume_valid_) {
            saved_volume_ = saved_volume;
        }
    }
    const bool changed =
        midiOutSetVolume(output, volume) == MMSYSERR_NOERROR;
    midiOutClose(output);
    return changed;
}

bool WinMmMusicPlayer::send_mci(const char* operation, const char* command,
                                char* result, unsigned int result_size) {
    const MCIERROR error =
        mciSendStringA(command, result, result_size, nullptr);
    if (error == 0) {
        return true;
    }

    char detail[192] = "unknown MCI error";
    mciGetErrorStringA(error, detail, sizeof(detail));
    char message[256];
    std::snprintf(message, sizeof(message), "%s: %s (%u)", operation,
                  detail, static_cast<unsigned int>(error));
    error_ = message;
    return false;
}

bool WinMmMusicPlayer::start_from_zero() {
    if (!send_mci("MCI seek", "seek train_tycoon_music to start")) {
        return false;
    }
    if (!send_mci("MCI play", "play train_tycoon_music")) {
        return false;
    }
    playing_ = true;
    next_status_check_ = midp_current_time_millis() + 100;
    return true;
}

bool WinMmMusicPlayer::load(const std::filesystem::path& path) {
    shutdown();
    error_.clear();
    if (!std::filesystem::is_regular_file(path)) {
        error_ = "MIDI resource open failed";
        return false;
    }

    const std::string path_text = path.string();
    char command[1280];
    std::snprintf(command, sizeof(command),
                  "open \"%s\" type sequencer alias %s",
                  path_text.c_str(), kMusicAlias);
    if (!send_mci("MCI open", command)) {
        return false;
    }
    loaded_ = true;
    return true;
}

bool WinMmMusicPlayer::play() {
    if (!loaded_) {
        error_ = "MIDI music is not loaded";
        return false;
    }
    if (playing_) {
        stop();
    }
    return start_from_zero();
}

void WinMmMusicPlayer::stop() {
    playing_ = false;
    if (loaded_) {
        send_mci("MCI stop", "stop train_tycoon_music");
    }
}

void WinMmMusicPlayer::set_volume(int level) {
    level = std::clamp(level, 0, 100);
    const bool restart = playing_;
    if (restart) {
        stop();
    }
    const std::uint32_t channel =
        static_cast<std::uint32_t>(level * 0xffff / 100);
    set_device_volume(channel | channel << 16, true);
    if (restart) {
        start_from_zero();
    }
}

void WinMmMusicPlayer::update() {
    if (!playing_ || midp_current_time_millis() < next_status_check_) {
        return;
    }
    next_status_check_ = midp_current_time_millis() + 100;
    char mode[32] = {};
    if (!send_mci("MCI status", "status train_tycoon_music mode", mode,
                  sizeof(mode))) {
        playing_ = false;
        return;
    }
    if (std::strcmp(mode, "stopped") == 0) {
        start_from_zero();
    }
}

void WinMmMusicPlayer::shutdown() {
    stop();
    if (loaded_) {
        send_mci("MCI close", "close train_tycoon_music");
        loaded_ = false;
    }
    if (saved_volume_valid_) {
        set_device_volume(saved_volume_, false);
    }
    saved_volume_valid_ = false;
}
