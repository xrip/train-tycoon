#!/usr/bin/env python3
"""Decode the game's UI text and dialogs, and generate the C++ text catalog.

The game has no ASCII/ANSI strings: every label is a byte array of *font glyph
codes*. There are two stores:

  * sprite_image_map.bin -- a flat table of strings addressed by a semantic
    index N (train/difficulty/direction/settings labels, confirm prompts,
    loading screens). See boot.cpp (GameState::IntroReady) and the many
    spriteBitmapTable[spriteIndexTable[N]] reads in dialog_data.cpp / paint.cpp.
  * packed_sprites.bin -- a declarative *dialog* format (header colours/size,
    optional title, content items, drawing commands). Parsed by
    PackedDialogReader in render.cpp; text is inline.

This tool inverts the glyph encoding so text is greppable, and can transcode the
dialog format into a readable C++ table (see the `codegen` subcommand) so both
the strings and the dialogs live in source and can be edited freely.

The glyph map was recovered by rendering the six font pages from sprite_images.bin
and reading decoded strings in context. Note the two letter ranges: codes 40..65
are lowercase and 70..95 are uppercase (this is what makes proper nouns like
"Baldwin M34" and the "Sun"+"field" city generator read correctly).

Usage:
    python tools/decode_packed_text.py                 # print every string
    python tools/decode_packed_text.py SEARCH          # filter by substring
    python tools/decode_packed_text.py --ids           # group by sprite id
    python tools/decode_packed_text.py --raw           # also show code arrays
    python tools/decode_packed_text.py --index-dump    # dump the index store
    python tools/decode_packed_text.py codegen [--out DIR] [--check]
"""
from __future__ import annotations

import argparse
import keyword
import sys
import unicodedata
from dataclasses import dataclass, field
from pathlib import Path

RES_DIR = Path(__file__).resolve().parent.parent / "resources"
DEFAULT_PACKED = RES_DIR / "packed_sprites.bin"
DEFAULT_INDEX = RES_DIR / "sprite_image_map.bin"

# ---------------------------------------------------------------------------
# Glyph <-> character mapping
# ---------------------------------------------------------------------------
# Codes not present here render as a visible placeholder so they stay traceable
# instead of silently disappearing. Verified ranges:
#   0          space (advance, no glyph)
#   1..10      '0'..'9'   (digit = code + 47)
#   40..65     'a'..'z'   (lower = code + 57)
#   70..95     'A'..'Z'   (upper = code - 5)
#   codes 32..39 are combining marks drawn (via midp_draw_line) over the *next*
#   glyph without advancing x, so a mark plus the following body glyph form one
#   character. Only three occur in the data:
#     32  the dot of i/j:  [32,48]->'i', [32,49]->'j' (48/49 are dotless bodies)
#     33  acute (carka):   [33, base]  e.g. [33,'a']->'a-acute'  (Vaclav)
#     34  hacek (caron):   [34, base]  e.g. [34,'r']->'r-caron'  (Ondrej Bures)
GLYPH_TO_CHAR: dict[int, str] = {
    0: " ",
    **{c: chr(c + 47) for c in range(1, 11)},        # '0'..'9'
    11: ".",
    12: ",",
    14: "!",
    16: "(",
    17: ")",
    20: "/",
    22: "-",
    24: "$",
    27: "'",
    29: "%",
    31: ":",
    **{c: chr(c + 57) for c in range(40, 66)},        # 'a'..'z' (48/49 = i/j bodies)
    **{c: chr(c - 5) for c in range(70, 96)},         # 'A'..'Z'
}
CHAR_TO_GLYPH: dict[str, int] = {ch: code for code, ch in GLYPH_TO_CHAR.items()}

DOT_MARK = 32                                # dot for i (48) and j (49)
COMBINING = {33: "́", 34: "̌"}     # acute, caron -> Unicode combiners


def decode_codes(codes: list[int]) -> str:
    """Codes -> readable Unicode text; unknown codes shown as <n>.

    Combining marks (32-34) merge with the following body glyph into one
    precomposed character so the text stays plain, editable Unicode.
    """
    out: list[str] = []
    i = 0
    n = len(codes)
    while i < n:
        code = codes[i]
        if code == DOT_MARK and i + 1 < n and codes[i + 1] in (48, 49):
            out.append("i" if codes[i + 1] == 48 else "j")
            i += 2
            continue
        if code in COMBINING and i + 1 < n and codes[i + 1] in GLYPH_TO_CHAR:
            base = GLYPH_TO_CHAR[codes[i + 1]]
            out.append(unicodedata.normalize("NFC", base + COMBINING[code]))
            i += 2
            continue
        ch = GLYPH_TO_CHAR.get(code)
        out.append(ch if ch is not None else f"<{code}>")
        i += 1
    return "".join(out)


def encode_text(text: str) -> list[int] | None:
    """ASCII (+ i/j digraph) text -> glyph codes. This is the *runtime* encoder
    the C++ mirrors, so it deliberately handles only what plain ASCII editing
    needs; strings with combining accents or unknown glyphs return None and are
    served from a pre-baked code override instead (see build_catalog).
    """
    out: list[int] = []
    for ch in text:
        if ch == "i":                       # dot + dotless-i body
            out += [DOT_MARK, 48]
            continue
        if ch == "j":                       # dot + dotless-j body
            out += [DOT_MARK, 49]
            continue
        code = CHAR_TO_GLYPH.get(ch)
        if code is None:
            return None
        out.append(code)
    return out


def round_trips(codes: list[int]) -> bool:
    """True if the codes survive decode -> encode unchanged (pure ASCII text)."""
    return encode_text(decode_codes(codes)) == list(codes)


# ---------------------------------------------------------------------------
# Little-endian / big-endian helpers matching the C++ readers
# ---------------------------------------------------------------------------
class ByteReader:
    def __init__(self, data: bytes, pos: int = 0) -> None:
        self.data = data
        self.pos = pos

    def u8(self) -> int:
        v = self.data[self.pos]
        self.pos += 1
        return v

    def u16_le(self) -> int:            # PackedDialogReader::u16 (low, high)
        low = self.u8()
        high = self.u8()
        return low + 256 * high

    def u16_be(self) -> int:            # boot.cpp read_u16_be (high, low)
        high = self.u8()
        low = self.u8()
        return high * 256 + low

    def rgb(self) -> int:               # PackedDialogReader::rgb (R, G, B)
        r = self.u8()
        g = self.u8()
        b = self.u8()
        return (r << 16) | (g << 8) | b

    def text(self) -> list[int]:        # read_packed_text_codes + skip
        length = self.u8()
        codes = [self.data[self.pos + i] for i in range(length)]
        self.pos += length
        return codes


def emit_rgb(value: int) -> bytes:
    return bytes(((value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF))


def emit_u16_le(value: int) -> bytes:
    value &= 0xFFFF
    return bytes((value & 0xFF, (value >> 8) & 0xFF))


# ---------------------------------------------------------------------------
# Store 1: sprite_image_map.bin (flat indexed string table)
# ---------------------------------------------------------------------------
def load_index_store(path: Path = DEFAULT_INDEX) -> dict[int, list[int]]:
    """Return {semantic_index N: glyph codes}, mirroring boot.cpp IntroReady."""
    reader = ByteReader(path.read_bytes())
    string_count = reader.u8()
    max_index = reader.u8()
    index_to_row: dict[int, int] = {}
    rows: list[list[int]] = [[] for _ in range(string_count)]
    for row in range(string_count):
        semantic_index = reader.u8()
        code_count = reader.u8()
        index_to_row[semantic_index] = row
        rows[row] = [reader.u8() for _ in range(code_count)]
    return {n: rows[row] for n, row in index_to_row.items()}


# ---------------------------------------------------------------------------
# Store 2: packed_sprites.bin (dialog format)
# ---------------------------------------------------------------------------
@dataclass
class ContentItem:
    flags: int
    image: bool
    x_offset: int = 0
    y_offset: int = 0
    has_resource_condition: bool = False
    has_dynamic_value: bool = False
    resource_id: int = 0
    value_id: int = 0
    action_code: int = 0
    sel_left: int = 0
    sel_top: int = 0
    sel_right: int = 0
    sel_bottom: int = 0
    slot_index: int = 0
    text_codes: list[int] | None = None


@dataclass
class DrawCommand:
    flags: int
    color: int
    x: int
    y: int
    w: int
    h: int
    has_resource_condition: bool
    resource_id: int


@dataclass
class Dialog:
    sprite_id: int          # table slot this dialog is stored under
    flags: int
    background_color: int
    edge_color: int
    border_color: int
    selection_color: int
    width: int
    height: int
    nested_sprite_id: int   # header's own sprite id used for value resolution
    title_codes: list[int] | None
    items: list[ContentItem] = field(default_factory=list)
    commands: list[DrawCommand] = field(default_factory=list)


def _read_content_item(reader: ByteReader) -> ContentItem:
    flags = reader.u8()
    image = bool(flags & 0x80)
    has_resource_condition = bool(flags & 0x40)
    if not image:
        x_offset = 0 if flags & 4 else reader.u8()
        y_offset = 0 if flags & 2 else reader.u8()
        has_dynamic_value = bool(flags & 0x20)
        resource_id = reader.u8() if has_resource_condition else 0
        value_id = reader.u8() if has_dynamic_value else 0
        item = ContentItem(flags=flags, image=False, x_offset=x_offset,
                           y_offset=y_offset,
                           has_resource_condition=has_resource_condition,
                           has_dynamic_value=has_dynamic_value,
                           resource_id=resource_id, value_id=value_id)
        if flags & 0x10:
            item.action_code = reader.u8()
            item.sel_left = reader.u8()
            item.sel_top = reader.u8()
            item.sel_right = reader.u8()
            item.sel_bottom = reader.u8()
        if not has_dynamic_value:
            item.text_codes = reader.text()
        return item
    slot_index = reader.u8()
    x_offset = 0 if flags & 0x20 else reader.u8()
    y_offset = 0 if flags & 0x10 else reader.u8()
    resource_id = reader.u8() if has_resource_condition else 0
    return ContentItem(flags=flags, image=True, x_offset=x_offset,
                       y_offset=y_offset,
                       has_resource_condition=has_resource_condition,
                       resource_id=resource_id, slot_index=slot_index)


def _read_command(reader: ByteReader) -> DrawCommand:
    flags = reader.u8()
    color = reader.rgb()
    x = reader.u16_le()
    y = reader.u16_le()
    w = reader.u16_le()
    h = reader.u16_le()
    has_resource_condition = bool(flags & 0x80)
    resource_id = reader.u8() if has_resource_condition else 0
    return DrawCommand(flags=flags, color=color, x=x, y=y, w=w, h=h,
                       has_resource_condition=has_resource_condition,
                       resource_id=resource_id)


def parse_dialog(data: bytes, offset: int, sprite_id: int) -> tuple[Dialog, int]:
    """Parse one dialog record; return (Dialog, bytes_consumed)."""
    reader = ByteReader(data, offset)
    flags = reader.u8()
    background = reader.rgb()
    edge = reader.rgb()
    border = reader.rgb()
    selection = reader.rgb()
    width = reader.u8()
    height = reader.u8()
    nested = reader.u8()
    title = reader.text() if flags & 2 else None
    dialog = Dialog(sprite_id=sprite_id, flags=flags, background_color=background,
                    edge_color=edge, border_color=border,
                    selection_color=selection, width=width, height=height,
                    nested_sprite_id=nested, title_codes=title)
    item_count = reader.u8()
    for _ in range(item_count):
        dialog.items.append(_read_content_item(reader))
    command_count = reader.u8()
    for _ in range(command_count):
        dialog.commands.append(_read_command(reader))
    return dialog, reader.pos - offset


def serialize_dialog(dialog: Dialog,
                     codes_for_text: "callable[[list[int] | None], list[int] | None]") -> bytes:
    """Inverse of parse_dialog. `codes_for_text` maps stored text to the codes
    the catalog would produce (used to prove the catalog is lossless)."""
    out = bytearray()
    out.append(dialog.flags)
    out += emit_rgb(dialog.background_color)
    out += emit_rgb(dialog.edge_color)
    out += emit_rgb(dialog.border_color)
    out += emit_rgb(dialog.selection_color)
    out.append(dialog.width)
    out.append(dialog.height)
    out.append(dialog.nested_sprite_id)
    if dialog.flags & 2:
        codes = codes_for_text(dialog.title_codes)
        out.append(len(codes))
        out += bytes(codes)
    out.append(len(dialog.items))
    for item in dialog.items:
        out.append(item.flags)
        if not item.image:
            if not item.flags & 4:
                out.append(item.x_offset)
            if not item.flags & 2:
                out.append(item.y_offset)
            if item.has_resource_condition:
                out.append(item.resource_id)
            if item.has_dynamic_value:
                out.append(item.value_id)
            if item.flags & 0x10:
                out += bytes((item.action_code, item.sel_left, item.sel_top,
                              item.sel_right, item.sel_bottom))
            if not item.has_dynamic_value:
                codes = codes_for_text(item.text_codes)
                out.append(len(codes))
                out += bytes(codes)
        else:
            out.append(item.slot_index)
            if not item.flags & 0x20:
                out.append(item.x_offset)
            if not item.flags & 0x10:
                out.append(item.y_offset)
            if item.has_resource_condition:
                out.append(item.resource_id)
    out.append(len(dialog.commands))
    for command in dialog.commands:
        out.append(command.flags)
        out += emit_rgb(command.color)
        out += emit_u16_le(command.x)
        out += emit_u16_le(command.y)
        out += emit_u16_le(command.w)
        out += emit_u16_le(command.h)
        if command.has_resource_condition:
            out.append(command.resource_id)
    return bytes(out)


def load_dialogs(path: Path = DEFAULT_PACKED) -> tuple[list[Dialog], dict[int, bytes]]:
    """Parse every dialog. Also return {sprite_id: original bytes} for fidelity
    checks (mirrors boot.cpp load_packed_sprite_data)."""
    reader = ByteReader(path.read_bytes())
    sprite_count = reader.u8()
    reader.u8()                 # max sprite id (table size); unused here
    reader.u16_be()             # total data bytes; unused here
    raw: dict[int, bytes] = {}
    order: list[int] = []
    for _ in range(sprite_count):
        sid = reader.u8()
        length = reader.u16_be()
        raw[sid] = reader.data[reader.pos:reader.pos + length]
        reader.pos += length
        order.append(sid)
    dialogs = []
    for sid in order:
        dialog, consumed = parse_dialog(raw[sid], 0, sid)
        if consumed != len(raw[sid]):
            raise ValueError(f"sprite {sid}: parsed {consumed} of {len(raw[sid])} bytes")
        dialogs.append(dialog)
    return dialogs, raw


# ---------------------------------------------------------------------------
# Catalog: unique code arrays across both stores -> named keys
# ---------------------------------------------------------------------------
@dataclass
class CatalogEntry:
    name: str
    english: str
    codes: list[int]
    override: bool          # codes cannot be reproduced from `english` via ASCII


def _identifier(text: str, used: set[str]) -> str:
    # Fold accents to ASCII so the C++ enumerator is a valid identifier
    # (Vaclav, Bures) even though the displayed English keeps them.
    text = unicodedata.normalize("NFKD", text)
    text = "".join(ch for ch in text if not unicodedata.combining(ch))
    slug = "".join(ch.lower() if ch.isascii() and ch.isalnum() else "_" for ch in text)
    slug = "_".join(part for part in slug.split("_") if part)
    if not slug:
        slug = "text"
    if slug[0].isdigit():
        slug = "n_" + slug
    if keyword.iskeyword(slug) or slug in {"none", "count"}:
        slug += "_"
    candidate = slug
    suffix = 2
    while candidate in used:
        candidate = f"{slug}_{suffix}"
        suffix += 1
    used.add(candidate)
    return candidate


class Catalog:
    def __init__(self) -> None:
        self.entries: list[CatalogEntry] = []
        self._by_codes: dict[tuple[int, ...], CatalogEntry] = {}
        self._used_names: set[str] = set()

    def intern(self, codes: list[int]) -> CatalogEntry:
        key = tuple(codes)
        existing = self._by_codes.get(key)
        if existing is not None:
            return existing
        english = decode_codes(codes)
        name = _identifier(english, self._used_names)
        override = not round_trips(codes)
        entry = CatalogEntry(name=name, english=english, codes=list(codes),
                             override=override)
        self.entries.append(entry)
        self._by_codes[key] = entry
        return entry


def build_catalog(index_store: dict[int, list[int]],
                  dialogs: list[Dialog]) -> tuple[Catalog, dict[int, CatalogEntry]]:
    """Intern every static string. Index-store strings come first (stable order),
    then dialog titles and static content text in sprite/item order."""
    catalog = Catalog()
    index_keys: dict[int, CatalogEntry] = {}
    for n in sorted(index_store):
        index_keys[n] = catalog.intern(index_store[n])
    for dialog in dialogs:
        if dialog.title_codes is not None:
            catalog.intern(dialog.title_codes)
        for item in dialog.items:
            if not item.image and not item.has_dynamic_value \
                    and item.text_codes is not None:
                catalog.intern(item.text_codes)
    return catalog, index_keys


# ---------------------------------------------------------------------------
# Emitters
# ---------------------------------------------------------------------------
GEN_HEADER = ("// Generated by tools/decode_packed_text.py -- seeded from the\n"
              "// game resources, then owned and edited by hand. Re-run codegen\n"
              "// only to re-seed from the binaries.\n")


def _cpp_string(text: str) -> str:
    escaped = text.replace("\\", "\\\\").replace('"', '\\"')
    return f'"{escaped}"'


def emit_catalog_data_inc(catalog: Catalog) -> str:
    lines = [GEN_HEADER,
             "// TT_TEXT(key, \"English\") -- edit the English freely; a code\n"
             "// override (text_code_overrides.inc) exists for non-ASCII glyphs.\n"]
    for entry in catalog.entries:
        note = "  // override" if entry.override else ""
        lines.append(f"TT_TEXT({entry.name}, {_cpp_string(entry.english)}){note}\n")
    return "".join(lines)


def emit_overrides_inc(catalog: Catalog) -> str:
    lines = [GEN_HEADER,
             "// TT_OVERRIDE(key, {codes}) -- exact glyph codes for strings that\n"
             "// cannot be reproduced from ASCII (accent marks, unknown glyphs).\n"]
    for entry in catalog.entries:
        if entry.override:
            codes = ", ".join(str(c) for c in entry.codes)
            english = decode_codes(entry.codes)
            lines.append(f"TT_OVERRIDE({entry.name}, {codes})  // {english}\n")
    return "".join(lines)


def emit_index_bindings_inc(index_keys: dict[int, CatalogEntry]) -> str:
    lines = [GEN_HEADER,
             "// TT_INDEX(semantic_index, key) -- sprite_image_map.bin slots.\n"]
    for n in sorted(index_keys):
        lines.append(f"TT_INDEX({n}, {index_keys[n].name})\n")
    return "".join(lines)


def _s8(value: int) -> int:
    """Interpret a stored byte the way C++ reader.byte() does: signed int8."""
    return value - 256 if value > 127 else value


def _text_ref(codes: list[int] | None, catalog: Catalog) -> str:
    if codes is None:
        return "Text::none"
    return f"Text::{catalog._by_codes[tuple(codes)].name}"


def emit_dialog_table_cpp(dialogs: list[Dialog], catalog: Catalog) -> str:
    out = [GEN_HEADER,
           '#include "game/dialog_table.h"\n\n',
           "namespace {\n",
           "using enum Text;\n\n",
           "const PackedDialog kDialogs[] = {\n"]
    for dialog in dialogs:
        out.append(f"  {{ /* sprite {dialog.sprite_id} */\n")
        out.append(f"    .sprite_id = {dialog.sprite_id}, .flags = 0x{dialog.flags:02X},\n")
        out.append(f"    .background_color = 0x{dialog.background_color:06X}, "
                   f".edge_color = 0x{dialog.edge_color:06X},\n")
        out.append(f"    .border_color = 0x{dialog.border_color:06X}, "
                   f".selection_color = 0x{dialog.selection_color:06X},\n")
        out.append(f"    .width = {dialog.width}, .height = {dialog.height}, "
                   f".nested_sprite_id = {_s8(dialog.nested_sprite_id)},\n")
        out.append(f"    .title = {_text_ref(dialog.title_codes, catalog)},\n")
        out.append("    .items = {\n")
        for item in dialog.items:
            out.append("      " + _emit_item(item, catalog) + "\n")
        out.append("    },\n")
        out.append("    .commands = {\n")
        for command in dialog.commands:
            out.append("      " + _emit_command(command) + "\n")
        out.append("    } },\n")
    out.append("};\n\n")
    out.append("}  // namespace\n\n")
    out.append("const PackedDialog* dialog_for_sprite(int sprite_id) {\n")
    out.append("  for (const PackedDialog& d : kDialogs)\n")
    out.append("    if (d.sprite_id == sprite_id) return &d;\n")
    out.append("  return nullptr;\n}\n\n")
    out.append("std::span<const PackedDialog> all_dialogs() { return kDialogs; }\n")
    return "".join(out)


def _emit_item(item: ContentItem, catalog: Catalog) -> str:
    parts = [f".flags = 0x{item.flags:02X}", f".image = {str(item.image).lower()}"]
    if item.x_offset:
        parts.append(f".x_offset = {_s8(item.x_offset)}")
    if item.y_offset:
        parts.append(f".y_offset = {_s8(item.y_offset)}")
    if item.has_resource_condition:
        parts.append(".has_resource_condition = true")
        parts.append(f".resource_id = {_s8(item.resource_id)}")
    if item.has_dynamic_value:
        parts.append(".has_dynamic_value = true")
        parts.append(f".value_id = {_s8(item.value_id)}")
    if item.flags & 0x10 and not item.image:
        parts.append(f".action_code = {_s8(item.action_code)}")
        parts.append(f".sel_left = {_s8(item.sel_left)}, .sel_top = {_s8(item.sel_top)}")
        parts.append(f".sel_right = {_s8(item.sel_right)}, .sel_bottom = {_s8(item.sel_bottom)}")
    if item.image:
        parts.append(f".slot_index = {_s8(item.slot_index)}")
    elif not item.has_dynamic_value:
        parts.append(f".text = {_text_ref(item.text_codes, catalog)}")
    return "{ " + ", ".join(parts) + " },"


def _emit_command(command: DrawCommand) -> str:
    # x/y/w/h are unsigned u16 as stored; render.cpp applies the signed int16
    # cast itself under the graph-substitution flags (8/0x10/0x20/0x40).
    parts = [f".flags = 0x{command.flags:02X}", f".color = 0x{command.color:06X}",
             f".x = {command.x}", f".y = {command.y}",
             f".w = {command.w}", f".h = {command.h}"]
    if command.has_resource_condition:
        parts.append(".has_resource_condition = true")
        parts.append(f".resource_id = {_s8(command.resource_id)}")
    return "{ " + ", ".join(parts) + " },"


# ---------------------------------------------------------------------------
# Fidelity check (Python side, before any C++ exists)
# ---------------------------------------------------------------------------
def verify_lossless(dialogs: list[Dialog], raw: dict[int, bytes],
                    catalog: Catalog) -> None:
    def codes_for_text(codes: list[int] | None) -> list[int]:
        if codes is None:
            return []
        entry = catalog._by_codes[tuple(codes)]
        return entry.codes if entry.override else encode_text(entry.english)

    for dialog in dialogs:
        rebuilt = serialize_dialog(dialog, codes_for_text)
        if rebuilt != raw[dialog.sprite_id]:
            raise SystemExit(f"FIDELITY FAIL: sprite {dialog.sprite_id} "
                             f"re-serialized to {len(rebuilt)} bytes, "
                             f"expected {len(raw[dialog.sprite_id])}")
    print(f"fidelity OK: {len(dialogs)} dialogs re-serialize byte-identical")


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------
GEN_FILES = {
    "text_catalog_data.inc": "src/text",
    "text_code_overrides.inc": "src/text",
    "text_index_bindings.inc": "src/text",
    "dialog_table.cpp": "src/game",
}


def run_codegen(args: argparse.Namespace) -> int:
    index_store = load_index_store(args.index)
    dialogs, raw = load_dialogs(args.bin)
    catalog, index_keys = build_catalog(index_store, dialogs)
    verify_lossless(dialogs, raw, catalog)

    contents = {
        "text_catalog_data.inc": emit_catalog_data_inc(catalog),
        "text_code_overrides.inc": emit_overrides_inc(catalog),
        "text_index_bindings.inc": emit_index_bindings_inc(index_keys),
        "dialog_table.cpp": emit_dialog_table_cpp(dialogs, catalog),
    }
    print(f"catalog: {len(catalog.entries)} unique strings "
          f"({sum(e.override for e in catalog.entries)} overrides), "
          f"{len(index_keys)} index bindings, {len(dialogs)} dialogs")

    root = args.out or (Path(__file__).resolve().parent.parent)
    if args.check:
        print("(--check: files not written)")
        return 0
    for name, text in contents.items():
        dest = root / GEN_FILES[name] / name
        dest.parent.mkdir(parents=True, exist_ok=True)
        dest.write_text(text, encoding="utf-8")
        print(f"wrote {dest.relative_to(root)}")
    return 0


def run_dump(args: argparse.Namespace) -> int:
    if args.index_dump:
        store = load_index_store(args.index)
        for n in sorted(store):
            print(f"{n:3}: {decode_codes(store[n])}")
        return 0

    dialogs, _ = load_dialogs(args.bin)
    needle = args.search.lower() if args.search else None
    seen: set[str] = set()
    for dialog in dialogs:
        texts: list[list[int]] = []
        if dialog.title_codes is not None:
            texts.append(dialog.title_codes)
        for item in dialog.items:
            if not item.image and item.text_codes is not None:
                texts.append(item.text_codes)
        first = True
        for codes in texts:
            text = decode_codes(codes)
            if not text.strip():
                continue
            if needle and needle not in text.lower():
                continue
            if text in seen:
                continue
            seen.add(text)
            if args.ids and first:
                print(f"--- sprite {dialog.sprite_id} ---")
                first = False
            print(f"{text}    {codes}" if args.raw else text)
    return 0


def main(argv: list[str] | None = None) -> int:
    try:                                    # decoded text is UTF-8 (accents)
        sys.stdout.reconfigure(encoding="utf-8")
    except (AttributeError, ValueError):
        pass
    argv = list(sys.argv[1:] if argv is None else argv)
    # `codegen` is a manual subcommand so the bare `SEARCH` dump form still works.
    if argv and argv[0] == "codegen":
        gen = argparse.ArgumentParser(prog="decode_packed_text.py codegen")
        gen.add_argument("--bin", type=Path, default=DEFAULT_PACKED)
        gen.add_argument("--index", type=Path, default=DEFAULT_INDEX)
        gen.add_argument("--out", type=Path, default=None,
                         help="output root (default: win32/)")
        gen.add_argument("--check", action="store_true",
                         help="verify fidelity + report counts, do not write files")
        return run_codegen(gen.parse_args(argv[1:]))

    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("search", nargs="?", default=None,
                        help="case-insensitive substring filter (dump mode)")
    parser.add_argument("--bin", type=Path, default=DEFAULT_PACKED,
                        help="path to packed_sprites.bin")
    parser.add_argument("--index", type=Path, default=DEFAULT_INDEX,
                        help="path to sprite_image_map.bin")
    parser.add_argument("--ids", action="store_true", help="group by sprite id")
    parser.add_argument("--raw", action="store_true", help="also show code arrays")
    parser.add_argument("--index-dump", dest="index_dump",
                        action="store_true", help="dump the index store")
    return run_dump(parser.parse_args(argv))


if __name__ == "__main__":
    raise SystemExit(main())
