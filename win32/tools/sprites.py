#!/usr/bin/env python3
"""Sprite container tool for the sprite_images.bin migration.

The game stores every sprite (cycle flag, six font pages, ~192 tile sprites) in
resources/sprite_images.bin as a sequence of self-describing blobs -- format 1
(indexed PNG, reconstructed field-by-field) and format 2 (a PNG stream with the
signature stripped). The decoder is win32/src/game/boot.cpp
`decode_png_from_sprite_stream`. This is the legacy J2ME obfuscated-container
format we are migrating away from.

Subcommands:
    verify           round-trip the parser; assert the blob stream is unchanged
    extract DIR      decode every sprite to DIR/NNN.png (RGBA) + DIR/manifest.txt
    diff DIR         for the MAPPING below, write old/new side-by-side PNGs

The extractor rebuilds proper PNGs from the parsed primitives, reusing each
format-1 sprite's original compressed IDAT bytes so the decoded pixels are
bit-identical to what the game renders. It is the source of truth for migrating
to loose-PNG loading and for swapping in OpenGFX2 art.

Tile sprite stream layout (verified by extraction): indices 0..7 are the cycle
flag and font pages; index 8 is tile slot 0 (tile code 1); slot = code - 1.
"""
from __future__ import annotations

import argparse
import io
import struct
import sys
import zlib
from pathlib import Path

from PIL import Image

WIN32_ROOT = Path(__file__).resolve().parent.parent
RESOURCES = WIN32_ROOT / "resources"
OPENGFX2 = WIN32_ROOT / "OpenGFX2"
SPRITE_BIN = RESOURCES / "sprite_images.bin"

K_HEADER_SPRITES = 8  # cycle flag + font pages before tile slot 0


# ----------------------------------------------------------------------------
# Container parse: split the stream into (format, payload) blobs.
# ----------------------------------------------------------------------------
def parse_stream(data: bytes):
    sprites = []
    pos = 0
    while pos < len(data):
        start = pos
        fmt = data[pos]
        pos += 1
        if fmt not in (1, 2):
            raise ValueError(f"unknown sprite format {fmt} at byte {start}")
        count = struct.unpack_from(">H", data, pos)[0]  # format 1: image bytes; 2: png bytes
        pos += 2
        if fmt == 2:
            body = data[pos : pos + count - 4]  # signature is prepended on decode
            pos += count - 4
            sprites.append((fmt, body, None))
        else:
            fields_start = pos
            pal_count = data[pos]
            palette_bytes = pal_count * 3
            # fmt(1)+count(2)+palcount(1)+tr(1)+w(2)+h(2)+bitdepth(1)+ihdrcrc(4)
            # +palette_bytes+4 (PLTE crc) + count (IDAT data) + 4 (IDAT crc)
            pos += 1 + 1 + 4 + 1 + 4 + palette_bytes + 4 + count + 4
            fields = data[fields_start:pos]
            sprites.append((fmt, fields, count))
    return sprites


def round_trip(data: bytes) -> bool:
    """Rebuild the blob stream and compare to the original."""
    out = bytearray()
    for fmt, body, count in parse_stream(data):
        if fmt == 2:
            out += bytes([2]) + struct.pack(">H", len(body) + 4) + body
        else:
            out += bytes([1]) + struct.pack(">H", count) + body
    return bytes(out) == data


# ----------------------------------------------------------------------------
# PNG reconstruction (format 1) and decode (format 2).
# ----------------------------------------------------------------------------
def _png_chunk(chunk_type: bytes, data: bytes) -> bytes:
    crc = zlib.crc32(chunk_type + data) & 0xFFFFFFFF
    return struct.pack(">I", len(data)) + chunk_type + data + struct.pack(">I", crc)


def decode_to_image(fmt: int, body: bytes, count):
    """Return a PIL RGBA image for one sprite blob."""
    if fmt == 2:
        # The stream stores PNG[4:]; the decoder prepends only the 4-byte
        # signature prefix (the remaining "\r\n\x1a\n" is already in the body).
        png = b"\x89PNG" + body
        return Image.open(io.BytesIO(png)).convert("RGBA")

    # format 1: body is the field stream after fmt+count.
    # layout: palcount(1) tr(1) w(2 BE) h(2 BE) bitdepth(1) ihdrcrc(4)
    #         PLTE-data(palcount*3) PLTE-crc(4) IDAT-data(count) IDAT-crc(4)
    p = 0
    pal_count = body[p]; p += 1
    tr_count = body[p]; p += 1
    width = struct.unpack_from(">H", body, p)[0]; p += 2
    height = struct.unpack_from(">H", body, p)[0]; p += 2
    bit_depth = body[p]; p += 1
    p += 4  # ihdr crc (recomputed below)
    palette = body[p : p + pal_count * 3]; p += pal_count * 3
    p += 4  # plte crc
    idat = body[p : p + count]; p += count

    png = b"\x89PNG\r\n\x1a\n"
    ihdr = struct.pack(">IIBBBBB", width, height, bit_depth,
                       3, 0, 0, 0)  # indexed, deflate, adaptive, no interlace
    png += _png_chunk(b"IHDR", ihdr)
    png += _png_chunk(b"PLTE", palette)
    if tr_count == 1:
        png += _png_chunk(b"tRNS", bytes([0]))  # palette index 0 is transparent
    png += _png_chunk(b"IDAT", idat)
    png += _png_chunk(b"IEND", b"")
    return Image.open(io.BytesIO(png)).convert("RGBA")


# ----------------------------------------------------------------------------
# tile_sprite_ids.bin: tile code -> slot (so the manifest can name sprites).
# ----------------------------------------------------------------------------
def load_tile_code_to_slot():
    """Return {tile_code: slot}; empty if the index file is missing."""
    path = RESOURCES / "tile_sprite_ids.bin"
    if not path.exists():
        return {}
    data = path.read_bytes()
    count = data[0]
    mapping = {}
    for resource_index in range(count):
        code = data[1 + resource_index]
        slot = resource_index - 8  # kScenarioResourceHeaderOffset
        if code != 0 and slot >= 0:
            mapping[code] = slot
    return mapping


def sprite_label(index, code_to_slot):
    if index == 0:
        return "cycle_flag"
    if 1 <= index <= 6:
        return f"font_page_{index - 1}"
    if index == 7:
        return "header_7"
    slot = index - K_HEADER_SPRITES
    codes = [c for c, s in code_to_slot.items() if s == slot]
    return f"slot_{slot:03d}_code_{codes[0]}" if codes else f"slot_{slot:03d}"


# ----------------------------------------------------------------------------
# Subcommands
# ----------------------------------------------------------------------------
def cmd_verify():
    data = SPRITE_BIN.read_bytes()
    sprites = parse_stream(data)
    ok = round_trip(data)
    print(f"parsed {len(sprites)} sprites "
          f"({len(sprites) - K_HEADER_SPRITES} tile slots)")
    print(f"round-trip {'OK' if ok else 'FAILED'}")
    return 0 if ok else 1


def cmd_extract(out_dir: Path):
    data = SPRITE_BIN.read_bytes()
    sprites = parse_stream(data)
    code_to_slot = load_tile_code_to_slot()
    out_dir.mkdir(parents=True, exist_ok=True)
    manifest = []
    for index, (fmt, body, count) in enumerate(sprites):
        image = decode_to_image(fmt, body, count)
        label = sprite_label(index, code_to_slot)
        image.save(out_dir / f"{index:03d}.png")
        manifest.append(f"{index:03d}  fmt{fmt}  {image.width}x{image.height}  {label}")
    (out_dir / "manifest.txt").write_text("\n".join(manifest) + "\n")
    print(f"extracted {len(sprites)} sprites to {out_dir}")
    print(f"manifest: {out_dir / 'manifest.txt'}")
    return 0


# Stage-1A mapping placeholder (used by `diff` once the loader exists).
MAPPING = {
    0: ("graphics/terrain/64/temperate_groundtiles_32bpp.png",
        (1, 1, 64, 31), (32, 16)),
}


def cmd_diff(out_dir: Path):
    out_dir.mkdir(parents=True, exist_ok=True)
    data = SPRITE_BIN.read_bytes()
    sprites = parse_stream(data)
    sheet_cache = {}

    def load_sheet(rel):
        if rel not in sheet_cache:
            sheet_cache[rel] = Image.open(OPENGFX2 / rel).convert("RGBA")
        return sheet_cache[rel]

    for slot, (sheet_rel, rect, target) in MAPPING.items():
        index = K_HEADER_SPRITES + slot
        old = decode_to_image(*sprites[index])
        sheet = load_sheet(sheet_rel)
        sx, sy, sw, sh = rect
        new = sheet.crop((sx, sy, sx + sw, sy + sh)).resize(target, Image.LANCZOS)
        canvas = Image.new("RGBA", (old.width + 8 + new.width,
                                    max(old.height, new.height)), (0, 0, 0, 0))
        canvas.paste(old, (0, 0))
        canvas.paste(new, (old.width + 8, 0))
        canvas.save(out_dir / f"slot_{slot:03d}_old_new.png")
        print(f"wrote {out_dir / f'slot_{slot:03d}_old_new.png'}")
    return 0


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__)
    sub = ap.add_subparsers(dest="cmd", required=True)
    sub.add_parser("verify", help="round-trip the parser, assert unchanged")
    p_extract = sub.add_parser("extract", help="decode every sprite to PNGs")
    p_extract.add_argument("out_dir", type=Path)
    p_diff = sub.add_parser("diff", help="write old/new side-by-side for MAPPING")
    p_diff.add_argument("out_dir", type=Path)
    args = ap.parse_args(argv)
    if args.cmd == "verify":
        return cmd_verify()
    if args.cmd == "extract":
        return cmd_extract(args.out_dir)
    if args.cmd == "diff":
        return cmd_diff(args.out_dir)


if __name__ == "__main__":
    sys.exit(main())
