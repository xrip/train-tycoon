// Proves the in-code text catalog and dialog table reproduce the original game
// resources exactly: catalog_codes() matches sprite_image_map.bin, and the
// generated dialog table re-serializes byte-identical to packed_sprites.bin.
#include "doctest/doctest.h"

#include "game/dialog_table.h"
#include "text/text_catalog.h"
#include "support/resource_reader.h"

#include <cstdint>
#include <map>
#include <vector>

namespace {

using Bytes = std::vector<std::int8_t>;

Bytes to_codes(const std::uint8_t* data, std::size_t count) {
    Bytes out(count);
    for (std::size_t i = 0; i < count; ++i) out[i] = static_cast<std::int8_t>(data[i]);
    return out;
}

// sprite_image_map.bin -> {semantic index: codes}, mirroring boot.cpp IntroReady.
std::map<int, Bytes> load_index_store() {
    const auto raw = resources::read_all("sprite_image_map.bin");
    REQUIRE(raw.has_value());
    const std::vector<std::uint8_t>& data = *raw;
    std::size_t pos = 0;
    const int string_count = data[pos++];
    pos++;  // max index
    std::map<int, Bytes> result;
    for (int row = 0; row < string_count; ++row) {
        const int semantic_index = data[pos++];
        const int code_count = data[pos++];
        result[semantic_index] = to_codes(&data[pos], static_cast<std::size_t>(code_count));
        pos += static_cast<std::size_t>(code_count);
    }
    return result;
}

// packed_sprites.bin -> {sprite id: original record bytes}, mirroring boot.cpp
// load_packed_sprite_data.
std::map<int, std::vector<std::uint8_t>> load_dialog_bytes() {
    const auto raw = resources::read_all("packed_sprites.bin");
    REQUIRE(raw.has_value());
    const std::vector<std::uint8_t>& data = *raw;
    std::size_t pos = 0;
    const int sprite_count = data[pos++];
    pos++;                                   // max sprite id
    pos += 2;                                // total bytes (u16 BE)
    std::map<int, std::vector<std::uint8_t>> result;
    for (int i = 0; i < sprite_count; ++i) {
        const int sid = data[pos++];
        const int length = data[pos] * 256 + data[pos + 1];   // u16 BE
        pos += 2;
        result[sid].assign(&data[pos], &data[pos] + length);
        pos += static_cast<std::size_t>(length);
    }
    return result;
}

void put_u8(std::vector<std::uint8_t>& out, int value) {
    out.push_back(static_cast<std::uint8_t>(value & 0xFF));
}
void put_rgb(std::vector<std::uint8_t>& out, int value) {
    out.push_back(static_cast<std::uint8_t>((value >> 16) & 0xFF));
    out.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
    out.push_back(static_cast<std::uint8_t>(value & 0xFF));
}
void put_u16_le(std::vector<std::uint8_t>& out, int value) {
    out.push_back(static_cast<std::uint8_t>(value & 0xFF));
    out.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
}
void put_text(std::vector<std::uint8_t>& out, Text key) {
    const Bytes& codes = catalog_codes(key);
    put_u8(out, static_cast<int>(codes.size()));
    for (std::int8_t c : codes) out.push_back(static_cast<std::uint8_t>(c));
}

// Inverse of the dialog parser -- must match tools/decode_packed_text.py
// serialize_dialog and the render.cpp reader field order.
std::vector<std::uint8_t> serialize(const PackedDialog& d) {
    std::vector<std::uint8_t> out;
    put_u8(out, d.flags);
    put_rgb(out, d.background_color);
    put_rgb(out, d.edge_color);
    put_rgb(out, d.border_color);
    put_rgb(out, d.selection_color);
    put_u8(out, d.width);
    put_u8(out, d.height);
    put_u8(out, d.nested_sprite_id);
    if (d.flags & 2) put_text(out, d.title);
    put_u8(out, static_cast<int>(d.items.size()));
    for (const DialogItem& item : d.items) {
        put_u8(out, item.flags);
        if (!item.image) {
            if (!(item.flags & 4)) put_u8(out, item.x_offset);
            if (!(item.flags & 2)) put_u8(out, item.y_offset);
            if (item.has_resource_condition) put_u8(out, item.resource_id);
            if (item.has_dynamic_value) put_u8(out, item.value_id);
            if (item.flags & 0x10) {
                put_u8(out, item.action_code);
                put_u8(out, item.sel_left);
                put_u8(out, item.sel_top);
                put_u8(out, item.sel_right);
                put_u8(out, item.sel_bottom);
            }
            if (!item.has_dynamic_value) put_text(out, item.text);
        } else {
            put_u8(out, item.slot_index);
            if (!(item.flags & 0x20)) put_u8(out, item.x_offset);
            if (!(item.flags & 0x10)) put_u8(out, item.y_offset);
            if (item.has_resource_condition) put_u8(out, item.resource_id);
        }
    }
    put_u8(out, static_cast<int>(d.commands.size()));
    for (const DialogCommand& c : d.commands) {
        put_u8(out, c.flags);
        put_rgb(out, c.color);
        put_u16_le(out, c.x);
        put_u16_le(out, c.y);
        put_u16_le(out, c.w);
        put_u16_le(out, c.h);
        if (c.has_resource_condition) put_u8(out, c.resource_id);
    }
    return out;
}

}  // namespace

TEST_CASE("fast:text_encode maps atlas glyphs") {
    CHECK(encode_text("A") == Bytes{70});
    CHECK(encode_text("a") == Bytes{40});
    CHECK(encode_text("0") == Bytes{1});
    CHECK(encode_text(" ") == Bytes{0});
    CHECK(encode_text(":") == Bytes{31});
    CHECK(encode_text("i") == Bytes{32, 48});   // dot + dotless-i body
    CHECK(encode_text("j") == Bytes{32, 49});
    CHECK(encode_text("Hi") == Bytes{77, 32, 48});
}

TEST_CASE("fast:catalog matches sprite_image_map.bin") {
    set_language(Lang::english);
    const std::map<int, Bytes> store = load_index_store();
    int checked = 0;
    for (const auto& [index, codes] : store) {
        const Text key = key_for_index(index);
        CAPTURE(index);
        REQUIRE(key != Text::none);
        CHECK(catalog_codes(key) == codes);
        ++checked;
    }
    CHECK(checked == static_cast<int>(store.size()));
}

TEST_CASE("fast:dialog table re-serializes to packed_sprites.bin") {
    set_language(Lang::english);
    const std::map<int, std::vector<std::uint8_t>> original = load_dialog_bytes();
    int checked = 0;
    for (const PackedDialog& dialog : all_dialogs()) {
        CAPTURE(dialog.sprite_id);
        const auto it = original.find(dialog.sprite_id);
        REQUIRE(it != original.end());
        CHECK(serialize(dialog) == it->second);
        ++checked;
    }
    CHECK(checked == static_cast<int>(all_dialogs().size()));
}
