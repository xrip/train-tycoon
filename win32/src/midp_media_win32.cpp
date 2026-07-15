// midp_media_win32.c - MIDI music through the WinMM MCI sequencer.
#include "midp.h"
#include "support/resource_reader.h"

#include <windows.h>
#include <mmsystem.h>
#include <filesystem>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MUSIC_ALIAS "train_tycoon_music"

static DWORD g_saved_volume;
static bool g_saved_volume_valid;
static bool g_loaded;
static bool g_playing;
static int64_t g_next_status_check;
static char g_last_error[256];

static bool set_device_volume(DWORD volume, bool save_current) {
    HMIDIOUT output = NULL;
    if (midiOutOpen(&output, MIDI_MAPPER, 0, 0, CALLBACK_NULL) !=
        MMSYSERR_NOERROR)
        return false;
    if (save_current && !g_saved_volume_valid)
        g_saved_volume_valid =
            midiOutGetVolume(output, &g_saved_volume) == MMSYSERR_NOERROR;
    bool changed = midiOutSetVolume(output, volume) == MMSYSERR_NOERROR;
    midiOutClose(output);
    return changed;
}

static void set_error(const char *text) {
    snprintf(g_last_error, sizeof(g_last_error), "%s", text);
}

static void set_mci_error(const char *operation, MCIERROR error) {
    char detail[192] = "unknown MCI error";
    mciGetErrorStringA(error, detail, sizeof(detail));
    snprintf(g_last_error, sizeof(g_last_error), "%s: %s (%u)", operation,
             detail, (unsigned)error);
}

static bool send_mci(const char *operation, const char *command,
                     char *result, UINT result_size) {
    MCIERROR error = mciSendStringA(command, result, result_size, NULL);
    if (error == 0) return true;
    set_mci_error(operation, error);
    return false;
}

static bool start_from_zero(void) {
    if (!send_mci("MCI seek", "seek " MUSIC_ALIAS " to start", NULL, 0))
        return false;
    if (!send_mci("MCI play", "play " MUSIC_ALIAS, NULL, 0)) return false;
    g_playing = true;
    g_next_status_check = midp_current_time_millis() + 100;
    return true;
}

bool midp_media_validate_midi(const char *resource_name) {
    const auto data = resources::read_all(resource_name);
    return data && data->size() >= 14 && memcmp(data->data(), "MThd", 4) == 0 &&
           (*data)[8] == 0 && (*data)[9] <= 1 &&
           ((*data)[10] != 0 || (*data)[11] != 0) &&
           ((*data)[12] & 0x80) == 0 &&
           ((*data)[12] != 0 || (*data)[13] != 0);
}

bool midp_media_load_midi(const char *resource_name) {
    midp_media_shutdown();
    g_last_error[0] = '\0';

    const std::filesystem::path path = resources::path_for(resource_name);
    if (!std::filesystem::is_regular_file(path)) {
        set_error("MIDI resource open failed");
        return false;
    }
    const std::string path_text = path.string();

    char command[1280];
    snprintf(command, sizeof(command),
             "open \"%s\" type sequencer alias " MUSIC_ALIAS,
             path_text.c_str());
    if (!send_mci("MCI open", command, NULL, 0)) return false;

    g_loaded = true;
    return true;
}

bool midp_media_play_midi(void) {
    if (!g_loaded) {
        set_error("MIDI music is not loaded");
        return false;
    }
    if (g_playing) midp_media_stop();
    return start_from_zero();
}

void midp_media_stop(void) {
    g_playing = false;
    if (g_loaded)
        send_mci("MCI stop", "stop " MUSIC_ALIAS, NULL, 0);
}

void midp_media_set_volume(int level) {
    if (level < 0) level = 0;
    if (level > 100) level = 100;
    bool restart = g_playing;
    if (restart) midp_media_stop();
    DWORD channel = (DWORD)(level * 0xffff / 100);
    set_device_volume(channel | channel << 16, true);
    if (restart) start_from_zero();
}

void midp_media_update(void) {
    if (!g_playing || midp_current_time_millis() < g_next_status_check) return;
    g_next_status_check = midp_current_time_millis() + 100;
    char mode[32] = {0};
    if (!send_mci("MCI status", "status " MUSIC_ALIAS " mode", mode,
                  sizeof(mode))) {
        g_playing = false;
        return;
    }
    if (strcmp(mode, "stopped") == 0) start_from_zero();
}

void midp_media_shutdown(void) {
    midp_media_stop();
    if (g_loaded) {
        send_mci("MCI close", "close " MUSIC_ALIAS, NULL, 0);
        g_loaded = false;
    }
    if (g_saved_volume_valid) set_device_volume(g_saved_volume, false);
    g_saved_volume_valid = false;
}

bool midp_media_is_loaded(void) { return g_loaded; }
bool midp_media_is_playing(void) { return g_playing; }
const char *midp_media_last_error(void) { return g_last_error; }
