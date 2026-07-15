// midp_stub.c - no-op high-score network support for this build.
#include "midp.h"

bool midp_net_sync_highscores(const uint8_t *payload, int len) {
    (void)payload; (void)len;
    return false; // no network in this build
}
