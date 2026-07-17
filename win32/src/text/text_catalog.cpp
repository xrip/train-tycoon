#include "text/text_catalog.h"

#include <array>

namespace {

// English strings, one per Text key, in enum order.
const char* const kEnglish[] = {
#define TT_TEXT(name, str) str,
#include "text/text_catalog_data.inc"
#undef TT_TEXT
};

constexpr int kTextCount = static_cast<int>(Text::count);

// Pre-baked glyph codes for the handful of strings that cannot be produced from
// ASCII (accent marks, unknown atlas glyphs). Returns nullptr when the key has
// no override and should be encoded from its string.
const std::int8_t* override_codes(Text key, int& length) {
    switch (key) {
#define TT_OVERRIDE(name, ...)                                        \
    case Text::name: {                                                \
        static const std::int8_t codes[] = {__VA_ARGS__};            \
        length = static_cast<int>(sizeof(codes));                    \
        return codes;                                                 \
    }
#include "text/text_code_overrides.inc"
#undef TT_OVERRIDE
        default:
            length = 0;
            return nullptr;
    }
}

Lang g_language = Lang::english;

// Encoded codes per key for the active language; (re)built on set_language.
std::array<std::vector<std::int8_t>, kTextCount> g_codes_cache;
const std::vector<std::int8_t> g_empty;

void rebuild_cache() {
    for (int i = 0; i < kTextCount; ++i) {
        const Text key = static_cast<Text>(i);
        int override_length = 0;
        const std::int8_t* codes = override_codes(key, override_length);
        if (codes != nullptr) {
            g_codes_cache[i].assign(codes, codes + override_length);
        } else {
            g_codes_cache[i] = encode_text(text_string(key));
        }
    }
}

bool g_cache_ready = false;

}  // namespace

std::vector<std::int8_t> encode_text(std::string_view text) {
    std::vector<std::int8_t> out;
    out.reserve(text.size() + 4);
    for (const char ch : text) {
        if (ch == 'i') {                    // dot mark + dotless-i body
            out.push_back(32);
            out.push_back(48);
            continue;
        }
        if (ch == 'j') {                    // dot mark + dotless-j body
            out.push_back(32);
            out.push_back(49);
            continue;
        }
        std::int8_t code = -1;
        if (ch >= '0' && ch <= '9') code = static_cast<std::int8_t>(ch - '0' + 1);
        else if (ch >= 'a' && ch <= 'z') code = static_cast<std::int8_t>(ch - 'a' + 40);
        else if (ch >= 'A' && ch <= 'Z') code = static_cast<std::int8_t>(ch - 'A' + 70);
        else if (ch == ' ') code = 0;
        else switch (ch) {
            case '.': code = 11; break;
            case ',': code = 12; break;
            case '!': code = 14; break;
            case '(': code = 16; break;
            case ')': code = 17; break;
            case '/': code = 20; break;
            case '-': code = 22; break;
            case '$': code = 24; break;
            case '\'': code = 27; break;
            case '%': code = 29; break;
            case ':': code = 31; break;
            default: code = -1; break;      // not drawable by the atlas -> skip
        }
        if (code >= 0) out.push_back(code);
    }
    return out;
}

void set_language(Lang lang) {
    g_language = lang;
    rebuild_cache();
    g_cache_ready = true;
}

Lang current_language() { return g_language; }

std::string_view text_string(Text key) {
    const int index = static_cast<int>(key);
    if (index < 0 || index >= kTextCount) return {};
    return kEnglish[index];
}

const std::vector<std::int8_t>& catalog_codes(Text key) {
    const int index = static_cast<int>(key);
    if (index < 0 || index >= kTextCount) return g_empty;
    if (!g_cache_ready) {
        rebuild_cache();
        g_cache_ready = true;
    }
    return g_codes_cache[index];
}

Text key_for_index(int index) {
    switch (index) {
#define TT_INDEX(n, name) case n: return Text::name;
#include "text/text_index_bindings.inc"
#undef TT_INDEX
        default:
            return Text::none;
    }
}
