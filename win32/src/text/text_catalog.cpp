#include "text/text_catalog.h"

#include <array>

namespace {

namespace glyph_code {
constexpr int8_t kUnavailable = -1;
constexpr int8_t kSpace = 0;
constexpr int8_t kFirstDigit = 1;
constexpr int8_t kFirstLowercase = 40;
constexpr int8_t kFirstUppercase = 70;
constexpr int8_t kDotMark = 32;
constexpr int8_t kDotlessI = 48;
constexpr int8_t kDotlessJ = 49;
constexpr int8_t kPeriod = 11;
constexpr int8_t kComma = 12;
constexpr int8_t kExclamation = 14;
constexpr int8_t kLeftParenthesis = 16;
constexpr int8_t kRightParenthesis = 17;
constexpr int8_t kSlash = 20;
constexpr int8_t kMinus = 22;
constexpr int8_t kDollar = 24;
constexpr int8_t kApostrophe = 27;
constexpr int8_t kPercent = 29;
constexpr int8_t kColon = 31;
}  // namespace glyph_code

// English strings, one per Text key, in enum order.
const char* const kEnglish[] = {
#define TT_TEXT(name, str) str,
#include "text/text_catalog_data.inc"
#undef TT_TEXT
};

constexpr int kTextCount = static_cast<int>(Text::count);
static_assert(sizeof(kEnglish) / sizeof(kEnglish[0]) == kTextCount);

// Pre-baked glyph codes for the handful of strings that cannot be produced from
// ASCII (accent marks, unknown atlas glyphs). Returns nullptr when the key has
// no override and should be encoded from its string.
const std::int8_t* override_codes(Text key, int& code_count) {
    switch (key) {
#define TT_OVERRIDE(name, ...)                                        \
    case Text::name: {                                                \
        static const std::int8_t codes[] = {__VA_ARGS__};            \
        code_count = static_cast<int>(sizeof(codes));                \
        return codes;                                                 \
    }
#include "text/text_code_overrides.inc"
#undef TT_OVERRIDE
        default:
            code_count = 0;
            return nullptr;
    }
}

Lang g_language = Lang::english;

// Encoded codes per key for the active language; (re)built on set_language.
std::array<std::vector<std::int8_t>, kTextCount> g_codes_cache;
const std::vector<std::int8_t> g_empty;

void rebuild_cache() {
    for (int text_index = 0; text_index < kTextCount; ++text_index) {
        const Text key = static_cast<Text>(text_index);
        int override_code_count = 0;
        const std::int8_t* codes = override_codes(key, override_code_count);
        if (codes != nullptr) {
            g_codes_cache[text_index].assign(codes, codes + override_code_count);
        } else {
            g_codes_cache[text_index] = encode_text(text_string(key));
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
            out.push_back(glyph_code::kDotMark);
            out.push_back(glyph_code::kDotlessI);
            continue;
        }
        if (ch == 'j') {                    // dot mark + dotless-j body
            out.push_back(glyph_code::kDotMark);
            out.push_back(glyph_code::kDotlessJ);
            continue;
        }
        std::int8_t code = glyph_code::kUnavailable;
        if (ch >= '0' && ch <= '9') code = static_cast<std::int8_t>(ch - '0' + glyph_code::kFirstDigit);
        else if (ch >= 'a' && ch <= 'z') code = static_cast<std::int8_t>(ch - 'a' + glyph_code::kFirstLowercase);
        else if (ch >= 'A' && ch <= 'Z') code = static_cast<std::int8_t>(ch - 'A' + glyph_code::kFirstUppercase);
        else if (ch == ' ') code = glyph_code::kSpace;
        else switch (ch) {
            case '.': code = glyph_code::kPeriod; break;
            case ',': code = glyph_code::kComma; break;
            case '!': code = glyph_code::kExclamation; break;
            case '(': code = glyph_code::kLeftParenthesis; break;
            case ')': code = glyph_code::kRightParenthesis; break;
            case '/': code = glyph_code::kSlash; break;
            case '-': code = glyph_code::kMinus; break;
            case '$': code = glyph_code::kDollar; break;
            case '\'': code = glyph_code::kApostrophe; break;
            case '%': code = glyph_code::kPercent; break;
            case ':': code = glyph_code::kColon; break;
            default: code = glyph_code::kUnavailable; break;      // not drawable by the atlas -> skip
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
#define TT_INDEX(text_index, text_name) case text_index: return Text::text_name;
#include "text/text_index_bindings.inc"
#undef TT_INDEX
        default:
            return Text::none;
    }
}
