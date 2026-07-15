// midp_rms.c - RMS RecordStore backed by a file (<name>.rms in the save dir).
//
// The game uses a single small store for the saved game and high-score table.
// Records get a monotonically increasing 1-based id (like MIDP). The store is
// loaded fully into memory on open and flushed to disk on close.
#include "midp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char g_rms_root[512] = ".";

void midp_rms_set_root(const char *root_dir) {
    if (!root_dir) return;
    snprintf(g_rms_root, sizeof(g_rms_root), "%s", root_dir);
}

typedef struct { int id; uint8_t *data; int len; } RmsRecord;

struct MidpRecordStore {
    char path[1024];
    RmsRecord *records;
    int count;
    int cap;
    int next_id;
};

static void rms_load(MidpRecordStore *s) {
    FILE *fp = fopen(s->path, "rb");
    if (!fp) return;
    int32_t count = 0, next_id = 1;
    if (fread(&count, sizeof(int32_t), 1, fp) != 1) { fclose(fp); return; }
    if (fread(&next_id, sizeof(int32_t), 1, fp) != 1) { fclose(fp); return; }
    for (int i = 0; i < count; ++i) {
        int32_t id = 0, len = 0;
        if (fread(&id, sizeof(int32_t), 1, fp) != 1) break;
        if (fread(&len, sizeof(int32_t), 1, fp) != 1) break;
        uint8_t *data = len > 0 ? (uint8_t *)malloc((size_t)len) : NULL;
        if (len > 0 && fread(data, 1, (size_t)len, fp) != (size_t)len) { free(data); break; }
        if (s->count == s->cap) {
            s->cap = s->cap ? s->cap * 2 : 8;
            s->records = (RmsRecord *)realloc(s->records, (size_t)s->cap * sizeof(RmsRecord));
        }
        s->records[s->count].id = id;
        s->records[s->count].data = data;
        s->records[s->count].len = len;
        s->count++;
    }
    s->next_id = next_id;
    fclose(fp);
}

static void rms_flush(MidpRecordStore *s) {
    FILE *fp = fopen(s->path, "wb");
    if (!fp) return;
    int32_t count = s->count, next_id = s->next_id;
    fwrite(&count, sizeof(int32_t), 1, fp);
    fwrite(&next_id, sizeof(int32_t), 1, fp);
    for (int i = 0; i < s->count; ++i) {
        int32_t id = s->records[i].id, len = s->records[i].len;
        fwrite(&id, sizeof(int32_t), 1, fp);
        fwrite(&len, sizeof(int32_t), 1, fp);
        if (len > 0) fwrite(s->records[i].data, 1, (size_t)len, fp);
    }
    fclose(fp);
}

MidpRecordStore *midp_rms_open(const char *name, bool create_if_needed) {
    MidpRecordStore *s = (MidpRecordStore *)calloc(1, sizeof(MidpRecordStore));
    if (!s) return NULL;
    snprintf(s->path, sizeof(s->path), "%s/%s.rms", g_rms_root, name);
    s->next_id = 1;
    FILE *probe = fopen(s->path, "rb");
    if (probe) { fclose(probe); rms_load(s); }
    else if (!create_if_needed) { free(s); return NULL; }
    return s;
}

int midp_rms_add_record(MidpRecordStore *s, const uint8_t *data, int off, int len) {
    if (s->count == s->cap) {
        s->cap = s->cap ? s->cap * 2 : 8;
        s->records = (RmsRecord *)realloc(s->records, (size_t)s->cap * sizeof(RmsRecord));
    }
    uint8_t *copy = len > 0 ? (uint8_t *)malloc((size_t)len) : NULL;
    if (len > 0) memcpy(copy, data + off, (size_t)len);
    int id = s->next_id++;
    s->records[s->count].id = id;
    s->records[s->count].data = copy;
    s->records[s->count].len = len;
    s->count++;
    return id;
}

static RmsRecord *rms_find(MidpRecordStore *s, int id) {
    for (int i = 0; i < s->count; ++i)
        if (s->records[i].id == id) return &s->records[i];
    return NULL;
}

bool midp_rms_get_record(MidpRecordStore *s, int id, uint8_t **out, int *out_len) {
    RmsRecord *r = rms_find(s, id);
    if (!r) return false;
    uint8_t *copy = r->len > 0 ? (uint8_t *)malloc((size_t)r->len) : NULL;
    if (r->len > 0) memcpy(copy, r->data, (size_t)r->len);
    *out = copy;
    *out_len = r->len;
    return true;
}

void midp_rms_delete_record(MidpRecordStore *s, int id) {
    for (int i = 0; i < s->count; ++i) {
        if (s->records[i].id == id) {
            free(s->records[i].data);
            for (int j = i; j < s->count - 1; ++j) s->records[j] = s->records[j + 1];
            s->count--;
            return;
        }
    }
}

int midp_rms_num_records(MidpRecordStore *s) { return s->count; }

int midp_rms_first_record_id(MidpRecordStore *s) {
    return s->count > 0 ? s->records[0].id : -1;
}

int midp_rms_record_id_at(MidpRecordStore *s, int index) {
    return (index >= 0 && index < s->count) ? s->records[index].id : -1;
}

int midp_rms_record_size(MidpRecordStore *s, int id) {
    RmsRecord *r = rms_find(s, id);
    return r ? r->len : -1;
}

void midp_rms_close(MidpRecordStore *s) {
    if (!s) return;
    rms_flush(s);
    for (int i = 0; i < s->count; ++i) free(s->records[i].data);
    free(s->records);
    free(s);
}
