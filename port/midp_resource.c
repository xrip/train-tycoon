// midp_resource.c - resource loading + the XOR resource reader.
//
// In the JAR, getResourceAsStream("/N") returned classpath entry "N". Here we
// map "/N" to <root>/N on disk (root defaults to "resources"). XorResourceInput
// reproduces the game's decoder: XOR each byte with a key that cycles 1..127.
// Plain resources (/6 MIDI, /ico.png) are read raw via midp_resource_read_all.
#include "midp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char g_root[512] = "resources";

void midp_resource_set_root(const char *root_dir) {
    if (!root_dir) return;
    strncpy(g_root, root_dir, sizeof(g_root) - 1);
    g_root[sizeof(g_root) - 1] = '\0';
}

struct MidpResource {
    FILE *fp;
    int key; // cycles 1..127
};

// Build "<root>/<name without leading '/'>" into buf.
static void resource_path(char *buf, size_t buflen, const char *name) {
    const char *n = name;
    if (n[0] == '/') n++;
    snprintf(buf, buflen, "%s/%s", g_root, n);
}

bool midp_resource_path(const char *name, char *out, size_t out_size) {
    if (!name || !out || out_size == 0) return false;
    const char *n = name[0] == '/' ? name + 1 : name;
    int written = snprintf(out, out_size, "%s/%s", g_root, n);
    return written >= 0 && (size_t)written < out_size;
}

MidpResource *midp_resource_open(const char *name) {
    char path[1024];
    resource_path(path, sizeof(path), name);
    FILE *fp = fopen(path, "rb");
    if (!fp) return NULL;
    MidpResource *r = (MidpResource *)calloc(1, sizeof(MidpResource));
    if (!r) { fclose(fp); return NULL; }
    r->fp = fp;
    r->key = 1;
    return r;
}

int midp_resource_read_byte(MidpResource *r) {
    int c = fgetc(r->fp);
    if (c < 0) return -1;
    c ^= r->key;
    r->key++;
    if (r->key == 128) r->key = 1;
    return c;
}

void midp_resource_close(MidpResource *r) {
    if (!r) return;
    if (r->fp) fclose(r->fp);
    free(r);
}

bool midp_resource_read_all(const char *name, uint8_t **out, int *out_len) {
    char path[1024];
    resource_path(path, sizeof(path), name);
    FILE *fp = fopen(path, "rb");
    if (!fp) return false;
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (sz < 0) { fclose(fp); return false; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz + 1);
    if (!buf) { fclose(fp); return false; }
    size_t got = fread(buf, 1, (size_t)sz, fp);
    fclose(fp);
    buf[got] = 0;
    *out = buf;
    *out_len = (int)got;
    return true;
}
