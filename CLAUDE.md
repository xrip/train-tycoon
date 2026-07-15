# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this repo is

A Train Tycoon native Windows game with its old J2ME decompile kept as a
behavior reference.

**Current development is only under `win32/`.** Treat it as a normal native C++20
project. It has its own source, resources, CMake build, and tests. It does not
need Java, Python, the old JAR, `port/`, or generated files.

Use the Java decompile and raw bytecode only when a game-behavior bug has no
clear answer in the native C++ source. The old deobfuscation and port pipeline is
kept for that fact-check work; it is no longer the normal development path.

## Native Win32 project — current source of truth

```powershell
cd win32
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

Run `win32/build/Debug/train_tycoon.exe`. Resources are copied beside the
program and found from the executable path.

- `win32/src/game/*.cpp` — game rules, state changes, simulation, and drawing.
- `win32/src/game_state.*` — game state and first values.
- `win32/src/game_methods.h` — game function declarations.
- `win32/src/midp_*` — small remaining local runtime.
- `win32/src/platform/winmm_music_player.*` — owned WinMM MIDI output.
- `win32/src/main.cpp` — Win32 start and frame loop.
- `win32/tests/*.cpp` — fast and full behavior checks.
- `win32/resources/` — game data and MIDI.

Edit these native files directly. There is no manifest, generation, or port
status step for `win32/`.

## Historical decompile source layout (`src/`)

Several variants exist; know which is which before editing:
- `src/deobfuscated/` — the deobfuscation output and main bug-reference source.
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

## Historical Win32 port pipeline — `tools/port.py`

`src/deobfuscated/GameCanvas.java` (9,727 lines, 441 fields, 196 methods) was
the source for the first native C work. This toolchain remains only for old
port checks and bug comparison.

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

### Naming rule for native work

Do not keep an obfuscated identifier in new or changed native C++ code when its
purpose can be recovered. If native work reaches such a name:

1. Read its behavior and descriptor-precise cross-references.
2. Pick a clear behavioral name. If the evidence is not strong, record the
   uncertainty and ask before renaming.
3. Rename it directly in `win32/`. If needed, use `tools/mappings.txt`, the
   readable Java, xrefs, and raw bytecode only to get the right meaning.

Use clear local names in native C++. If old short names such as `n`, `by`, or
`bl` make changed code unclear, give them behavioral names as part of that
change.

## Historical root build

- CMake ≥ 3.25; MSVC (VS 2022 Build Tools) or GCC/Clang with C23. Python 3 for tooling.
- `python tools/port.py verify` runs the whole chain (build + headless smoke, ~120 frames). Set the `TRAIN_TYCOON_SMOKE` env var for smoke mode in `main.c`.
- Set `TRAIN_TYCOON_MIDI_TEST_MS=3000` when running the native executable for a
  short WinMM MIDI device check without the game window.
- The root CMake build is kept for old port proof. Normal work and builds start
  in `win32/`.
- Game assets extracted from the JAR live in `resources/` (`1`..`6`, `ico.png`) — required at runtime.

## Known gotchas

- **MSVC has no C23 `nullptr`** — `port/main.c` defines a guarded `#define nullptr NULL` fallback before including `MiniFB.h`.
- The old `buildMenuIndex` mapping conflict is fixed upstream as
  `buildMenuItemIndex` and `offerRowIndex`.
- MIDI music (`/6`) uses the WinMM MCI sequencer. Wave sound effects are the
  planned second sound phase. High-score networking is intentionally stubbed.
