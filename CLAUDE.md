# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this repo is

A two-phase project on the Train Tycoon J2ME game (originally a single obfuscated class `a` in a `.jar`):

1. **Deobfuscation** — reproducibly rename the obfuscated bytecode into readable Java.
2. **Win32 port** (currently active) — port that readable Java to a native C23 program on the MiniFB backend.

Both phases are driven by **reproducible, subcommand-based Python toolchains** — never hand edits to generated output. Resumability is a hard requirement: anyone can continue from any point with no bus factor. When continuing the port, read **`PORT_HANDOFF.md`** first.

## Source layout (`src/`)

Several variants exist; know which is which before editing:
- `src/deobfuscated/` — the deobfuscation output. **`GameCanvas.java` here is the single source of truth** for both renaming and the port.
- `src/readable/` — earlier hand-recovered readable Java (historical).
- `src/faithful/` — the two classes reconstructable near-exactly, original identifiers preserved.
- `src/decompiled_cfr/` — raw CFR decompilation (input to the pipeline).

## Deobfuscation — `tools/deobf.py`

Renaming happens in **bytecode** (ASM, descriptor-aware) so overloaded `a(...)`, field `a`, and class `a` never collide. A remap is a pure rename: behaviour is preserved, so faithfulness proved once by `verify` holds for every rename.

```
python tools/deobf.py setup      # fetch pinned tools (CFR, ASM, J2ME stubs) — idempotent
python tools/deobf.py deobf      # extract -> remap -> decompile
python tools/deobf.py xref       # regenerate the cross-reference report
python tools/deobf.py verify     # prove faithfulness (compiles vs J2ME stubs)
python tools/deobf.py show NAME  # print a method body from the readable source
python tools/deobf.py all        # setup + deobf + xref
```
Requires `java`/`javac` (JDK 8+) on PATH. Renames live in `tools/mappings.txt`:
`class|field|method <original-obfuscated keys> <newName>`; method rows carry the
original JVM descriptor. Keys are always the *original obfuscated* owner/name.

## Win32 port — `tools/port.py`

`src/deobfuscated/GameCanvas.java` (9,727 lines, 441 fields, 196 methods) is the
source of truth. The toolchain extracts fields/methods, generates C scaffolding,
and tracks per-method port status in `tools/port_manifest.json`.

```
python tools/port.py inventory    # (re)scan Java -> manifest (idempotent, safe)
python tools/port.py status [-v]  # ported count; -v lists pending methods
python tools/port.py next         # suggest smallest pending (leaf) method
python tools/port.py show NAME    # print a method's Java body
python tools/port.py mark NAME [ported|partial|pending]
python tools/port.py gen          # regenerate port/generated/*
python tools/port.py build        # cmake build (VS 2022, x64)
python tools/port.py verify       # build + headless smoke run (exit 0 = good)
```

### Port architecture — what you edit vs. what is generated

| Path | Edit by hand? |
|---|---|
| `port/midp_*.{c,h}`, `MiniFB.h`, `port/main.c` | Yes — stable MIDP shim + Win32 entry |
| `port/impl/*.c` | **Yes — this is the port** (hand-ported method bodies, grouped by theme) |
| `port/generated/*` | **No** — `port.py gen` rewrites it from the manifest |
| `tools/port_manifest.json` | Via `port.py mark`, or edit directly |

`port/generated/` holds: `game_state.h` (441-field `GameCanvas` struct + statics),
`game_state.c` (`gc_fields_init` — all initializers + data tables),
`game_methods.h` (every method prototype), `game_stubs.c` (stubs for
**not-yet-ported** methods only — so there is never a duplicate symbol).

The MIDP shim reproduces only the MIDP calls the game makes: `Graphics` (software
rasterizer), `Image` (real PNG decode via vendored `port/third_party/stb_image.h`),
`java.util.Random` (exact 48-bit LCG — map-gen depends on it), `Vector`,
`ByteArrayOutputStream`, `RecordStore`→file, XOR resource reader.

### How to port one method (the loop)

1. `python tools/port.py next` — pick a pending leaf method.
2. `python tools/port.py show NAME` — read its Java body.
3. Write the C body in `port/impl/<theme>.c`. Signature = the prototype in `port/generated/game_methods.h`.
4. `python tools/port.py mark NAME ported`
5. `python tools/port.py gen && python tools/port.py verify`
6. Commit.

**Java → C conventions:**
- Instance field `this.foo` → `gc->foo`; static field `foo` → `gcs_foo`.
- Method `this.bar(x)` → `gc_bar(gc, x)` (overloads get a `__N` suffix; see `game_methods.h`).
- Types: `int`→`int32_t`, `byte`→`int8_t`, `short`→`int16_t`, `long`→`int64_t`, `boolean`→`bool`; arrays add `*` per dimension.
- `rng.nextInt(n)` → `midp_random_next_int_bound(&gc->rng, n)`.
- Java array `.length` is impossible on raw C pointers — every game array is length-tracked: allocate with `GC_NEW(type, n)`, read length with `midp_arr_len(ptr)`. `System.arraycopy(...)` → `memcpy` (or `memmove` if regions overlap).
- `run()` is **not** a blocking loop: `port/main.c` owns the frame loop and calls `gc_update` + `gc_paint` each frame. Port the run-iteration body into `update()`/`paint()`.

### Naming rule during the port

Do not carry an obfuscated identifier into new C code when its purpose can be
recovered. If a method being ported still has an obfuscated member name:

1. Read its behavior and descriptor-precise cross-references.
2. Pick a clear behavioral name. If the evidence is not strong, record the
   uncertainty and ask before renaming.
3. Add the member rename to `tools/mappings.txt`, then run the deobfuscation
   pipeline so `src/deobfuscated/GameCanvas.java` stays the source of truth.
4. Run `python tools/port.py inventory` and regenerate the C scaffolding before
   finishing the C method.

Use clear local names in hand-ported C even when a decompiler-only Java local
still has a name such as `n`, `by`, or `bl`. Keep a short Java-name comment only
when it helps a line-by-line faithfulness check.

## Build & run

- CMake ≥ 3.25; MSVC (VS 2022 Build Tools) or GCC/Clang with C23. Python 3 for tooling.
- `python tools/port.py verify` runs the whole chain (build + headless smoke, ~120 frames). Set the `TRAIN_TYCOON_SMOKE` env var for smoke mode in `main.c`.
- Raw build: `cmake -S . -B build/port_build -G "Visual Studio 17 2022" -A x64` then `cmake --build build/port_build --config Debug`.
- Game assets extracted from the JAR live in `resources/` (`1`..`6`, `ico.png`) — required at runtime.

## Known gotchas

- **MSVC has no C23 `nullptr`** — `port/main.c` defines a guarded `#define nullptr NULL` fallback before including `MiniFB.h`.
- **`mappings.txt` has a duplicate field name** `buildMenuIndex` (Java L162 `int` + L215 `byte`); the port tool renames the second to `buildMenuIndex_2` and warns on `inventory`. Fix upstream in `mappings.txt`.
- Sound (MIDI `/6`) and high-score networking are intentionally **stubbed**.
