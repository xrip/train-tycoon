// UI text catalog: readable, editable, i18n-ready strings.
//
// The game stores no ASCII strings -- every label is glyph codes for the bitmap
// font atlas (see game/render.cpp draw_font_string). This catalog holds the text
// as normal, greppable, editable literals and encodes them to glyph codes for
// the existing renderer. Edit the English in text/text_catalog_data.inc; the few
// strings that use accent marks or non-atlas glyphs carry a pre-baked code
// override in text/text_code_overrides.inc.
//
// Latin-only for now: encode_text handles ASCII plus the i/j "dot" digraph the
// atlas uses. Non-Latin scripts would need a real font backend behind
// catalog_codes; the interface is kept backend-agnostic for that.
#ifndef TEXT_CATALOG_H
#define TEXT_CATALOG_H

#include <cstdint>
#include <string_view>
#include <vector>

enum class Text : std::uint16_t {
#define TT_TEXT(name, str) name,
#include "text/text_catalog_data.inc"
#undef TT_TEXT
    count,
    none = 0xFFFF,   // absent title / non-text slot
};

enum class Lang : std::uint8_t { english, count };

// ASCII (+ i/j digraph) text -> font glyph codes. Mirrors the runtime encoder in
// tools/decode_packed_text.py. Characters the atlas cannot draw are skipped.
std::vector<std::int8_t> encode_text(std::string_view text);

void set_language(Lang lang);
Lang current_language();

// The active language's UTF-8 string for a key ("" for Text::none).
std::string_view text_string(Text key);

// Cached glyph codes for a key (override codes when present, else encode_text of
// the current language string). Empty for Text::none. Reference stays valid
// until the next set_language.
const std::vector<std::int8_t>& catalog_codes(Text key);

// sprite_image_map.bin semantic index -> catalog key (Text::none if unbound).
Text key_for_index(int index);

#endif  // TEXT_CATALOG_H
