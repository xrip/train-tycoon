// midp_stub.c - no-op media (MIDI) and networking (high-score sync) for this
// build. Wire up winmm midiStream / an HTTP client in a later batch.
#include "midp.h"

void midp_media_play_midi(const char *resource_name) { (void)resource_name; }
void midp_media_stop(void) {}
void midp_media_set_volume(int level) { (void)level; }

bool midp_net_sync_highscores(const uint8_t *payload, int len) {
    (void)payload; (void)len;
    return false; // no network in this build
}
