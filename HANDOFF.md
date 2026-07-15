# HANDOFF — Train Tycoon readable decompile and native port

## 1. Project purpose and target

This project has two connected parts:

1. Make a repeatable, readable Java decompile of the obfuscated Train Tycoon
   J2ME game.
2. Make a native Windows C23 port on MiniFB.

The Java source of truth is `src/deobfuscated/GameCanvas.java`. The native port
target is the same game behavior, with sound and high-score network work left
out by request.

The native port is complete at **196/196 full methods**, with **0 partial
methods** and no generated method bodies in `port/generated/game_stubs.c`.

## 2. Current Git state

- Branch: `master`
- Base commit before this handoff: `1b7e828 Complete native gameplay port`
- Recent main commits:
  - `1b7e828` — completed all `update()` states and added the state-30 smoke
    path.
  - `400a23f` — added the last dialog-data, money-format, and train-render
    methods; also completed the last clear name changes.
  - `ae30cbb` — added score, graph, text, life-cycle, and excluded-system
    methods.
  - `b074cca` through `f1a38fc` — added map setup, save/load, path finding,
    train buying, build actions, and view control.

`AGENTS.md` is an untracked, user-owned instruction file. It is not part of the
project commit and must stay uncommitted unless the user gives a direct request.

No other uncommitted project work was present before this handoff.

The following ignored files are normal generated data and must not be put in a
commit: `build/`, `tools/lib/`, `tools/__pycache__/`, and `TTStore.rms`.

## 3. What was made and why

### Repeatable deobfuscation

- `tools/deobf.py` drives extract, bytecode name changes, CFR decompile, xref,
  and Java proof steps.
- `tools/mappings.txt` holds descriptor-specific class, field, and method name
  changes.
- `tools/Remap.java` does bytecode name changes through ASM. This keeps
  overloaded names separate and does not make source-text substitutions.
- `tools/Xref.java` creates `analysis/XREF_GameCanvas.md` for exact callers,
  callees, field use, and string data.

This design makes every readable name repeatable from the original JAR and
keeps `src/deobfuscated/` as generated source rather than hand-edited Java.

### Native C23 port

- `tools/port.py` reads the readable Java, keeps port state in
  `tools/port_manifest.json`, and creates `port/generated/`.
- `port/impl/*.c` has the 196 hand-ported game methods, grouped by work area.
- `port/midp_*.c` and `port/midp.h` give the small MIDP layer needed by this
  game: graphics, PNG images, resources, key input, Java-style random numbers,
  vectors, byte streams, and RecordStore files.
- `port/main.c` owns the Windows frame loop. The old Java `run()` loop is one
  frame at a time in native code.
- `MiniFB.h` gives the Windows window and frame output.
- `port/third_party/stb_image.h` gives PNG decode.

This design keeps generated state and declarations separate from hand-written
game code. It also keeps the port close to the Java control flow, which lowers
the risk of behavior changes.

### Main game systems now present

- Full start and resource decode path.
- Title and menu state machines.
- Scenario read and map generation.
- Isometric terrain, tile, train, menu, and dialog drawing.
- Key input, view movement, zoom, and tile selection.
- Track, station, train, wagon, removal, and upgrade actions.
- Track path finding and preview work.
- Train movement, service, cargo, income, AI, month, year, and era work.
- Save/load state, map matrices, RLE, and RecordStore file support.
- Map overview and all `update()` state cases.
- Score, graph, number, money, and dynamic dialog data.

These changes meet the main goal: a native start-to-gameplay port with no
missing game method bodies.

### Clear-name rule

When port work shows an old obfuscated member name, use behavior and exact
xrefs to get a clear name. Put the name change in `tools/mappings.txt`, run the
deobfuscation steps, then run `port.py inventory` and `port.py gen`.

Do not put a recoverable obfuscated name only in the C code. If evidence does
not give one clear meaning, say so and keep the doubt in project notes. One
example is `legacyDecadeMetric`: bytecode gives no writes after its zero start,
so its business meaning is not known.

## 4. Architecture and important decisions

### Source-of-truth flow

```text
original JAR
  -> tools/deobf.py + tools/mappings.txt
  -> src/deobfuscated/GameCanvas.java
  -> tools/port.py + tools/port_manifest.json
  -> port/generated/* declarations and state
  -> port/impl/* native method bodies
```

Do not hand-edit `port/generated/*`. Run `python tools/port.py gen` after a
manifest or Java name change.

### Runtime flow

`port/main.c` does:

```text
resource root -> GameCanvas field start -> game start
-> key events -> one gc_run frame -> gc_paint -> MiniFB frame output
```

The window is 240×320 game pixels at 2× display scale and 30 frames per second.

### Java array support

Native game arrays keep their length through `GC_NEW` and `midp_arr_len`.
Use these helpers for game arrays. Do not replace them with plain `malloc` when
code needs Java `.length` behavior.

### Random numbers

The MIDP layer has the Java 48-bit random-number process. Map generation needs
this exact behavior.

### Sound and network

Sound and high-score network work are outside the requested target:

- `port/impl/remaining.c` has clear no-op game methods.
- `port/midp_stub.c` has no-op MIDI and network support.
- High-score sync returns `false`, so the game takes its normal failure result
  state.

Do not treat these as missing port work unless the user puts them back in
scope.

## 5. What works now

- Java proof build and round-trip decompile.
- Native MSVC C23 build.
- Real game resource decode and PNG drawing.
- Start screen and menu drawing/input.
- Scenario 0 map generation to game state 30.
- One state-30 input, update, and paint cycle in the smoke suite.
- Small path-finding smoke check.
- In-memory save/RLE/load round-trip smoke check.
- Native RecordStore file support through `TTStore.rms` for normal game use.

`python tools/port.py status` reports:

```text
methods done: 196/196 (100%)  [196 full, 0 partial]
fields (all generated): 441
```

## 6. What is not complete or not proved

- Sound is a no-op by request.
- High-score network work is a no-op by request.
- The smoke suite is not a full test of every menu selection, every scenario,
  long game play, all AI results, or every save/load path on disk.
- A full manual play test on a normal Windows desktop is still needed.
- The build is Windows-only. `CMakeLists.txt` gives an error on non-Windows
  systems.
- The old `README.md` still gives the early decompile view. Use this file,
  `CLAUDE.md`, and `PORT_HANDOFF.md` for the current native-port state.

No known build or smoke failure is present at handoff time.

## 7. Known limits, risks, and technical debt

- MSVC does not give the C23 `nullptr` word used by MiniFB. `port/main.c` has a
  guarded `nullptr -> NULL` definition before `MiniFB.h`.
- CFR left out a real `break` in `computeTrackBuildInlined`. Bytecode shows that
  the scan stops at the first non-empty tile. The C port follows bytecode.
- `autoExtendTrackStation` checks the “track above” bit for both vertical ways.
  Bytecode shows the same strange behavior, so the C port keeps it.
- `pathSearchLimit` is a low-confidence name. The game sets it to 10, but
  bytecode has no read of the field.
- `legacyDecadeMetric` has no bytecode writes after its zero start. Its exact
  old business meaning is not known.
- Some smoke test objects do not free every nested game allocation before the
  short test process ends. This is test-only process-lifetime waste, not a
  known game-loop growth path.
- The main smoke path directly selects scenario 0 after the real asset load.
  It proves the main game state chain, but not every title-menu key sequence.

## 8. Build, run, and check

### Requirements

- Windows.
- CMake 3.25 or later.
- Visual Studio 2022 C build tools and MSVC, or a Windows GCC/Clang setup.
- Python 3.
- JDK 8 or later for deobfuscation checks.
- `resources/1` through `resources/6` and `resources/ico.png` in the project
  tree.

Pinned Java tool JARs are under ignored `tools/lib/`. If they are missing, run:

```powershell
python tools/deobf.py setup
```

### Normal build

```powershell
cmake -S . -B build/port_build -G "Visual Studio 17 2022" -A x64
cmake --build build/port_build --config Debug
```

Output:

```text
build/port_build/Debug/train_tycoon.exe
```

Run it from the project root so the relative `resources` path is valid:

```powershell
.\build\port_build\Debug\train_tycoon.exe
```

### Port checks

```powershell
python tools/port.py status
python tools/port.py verify
```

`verify` builds the native target and runs 120 smoke frames. Before the window
frame loop, it also runs:

1. A 5×5 path build with track and cost checks.
2. An in-memory state save, RLE pack/unpack byte comparison, and field restore.
3. Real asset load, scenario 0 generation, state-30 paint, movement-key input,
   one game frame, and a second paint.

### Java proof check

```powershell
python tools/deobf.py verify
```

This decompiles the original classes, applies the known CFR output fixes,
builds all three Java classes against the J2ME support JARs, and checks a stable
round-trip decompile.

### Exact handoff check results

Run on 2026-07-15 from `E:\Train_Tycoon_readable_decompile`:

- `python tools/deobf.py verify` — **PASS**, exit 0. The patched decompile built
  3 classes. Round-trip change was local-variable naming only. Java gave only
  the known old-API note.
- `python tools/port.py verify` — **PASS**, exit 0. MSBuild made
  `build/port_build/Debug/train_tycoon.exe`; all smoke work returned 0.
- `python tools/port.py status` — **PASS**: 196/196 full, 0 partial, 441 fields.
- `git diff --check` — **PASS**, exit 0.

## 9. Current development objective

The requested port objective is complete. The next objective is proof by
manual play and small, local fixes for any behavior difference found there.
Sound and network work remain out of scope.

## 10. Next tasks, in order

1. **Do a manual Windows play test.** Start a new game through the real title
   menu. Build track and stations, buy a train, run service, open finance/map
   views, save, load, and exit. Keep exact steps for any failure.
2. **Add a small check for each found failure before its fix.** Keep changes
   close to the Java and bytecode source.
3. **Test more scenarios.** Extend the smoke suite only when a real problem or
   high-risk path gives a reason.
4. **Check long-run state.** Let months and years pass, then check AI, money,
   era changes, reports, and save/load stability.
5. **Only by a new user request:** add MIDI sound or high-score network support.

## 11. Important files and entry points

- `CLAUDE.md` — project rules, tool use, and port design.
- `PORT_HANDOFF.md` — detailed port history and known bytecode facts.
- `src/deobfuscated/GameCanvas.java` — readable Java source of truth.
- `tools/mappings.txt` — repeatable bytecode name map.
- `analysis/XREF_GameCanvas.md` — exact member cross-references.
- `tools/deobf.py` — deobfuscation and Java proof driver.
- `tools/port.py` — port inventory, generation, build, and smoke driver.
- `tools/port_manifest.json` — 196-method port state and 441-field C map.
- `CMakeLists.txt` — Windows C23 build.
- `port/main.c` — Windows entry, frame loop, and smoke suite.
- `port/midp.h` and `port/midp_*.c` — MIDP support layer.
- `port/impl/paint.c` — full paint and update state switches.
- `port/impl/menu_dispatch.c` — menu action state changes.
- `port/impl/map_gen.c` — terrain, city, industry, and route setup.
- `port/impl/render_map.c` — isometric world draw.
- `port/impl/train_tick.c` and `port/impl/sim_tick.c` — train and time work.
- `port/impl/build_actions.c` and `port/impl/pathfinding.c` — building work.
- `port/impl/save.c` and `port/impl/save_arrays.c` — game and map save/load.
- `port/generated/game_state.h` — generated `GameCanvas` C state.
- `port/generated/game_methods.h` — generated method declarations.
- `port/generated/game_stubs.c` — generated and empty at this handoff.
- `resources/` — required game data and extracted review material.

## 12. Safe change process

For a Java name change:

```powershell
# edit tools/mappings.txt
python tools/deobf.py deobf
python tools/deobf.py xref
python tools/deobf.py verify
python tools/port.py inventory
python tools/port.py gen
python tools/port.py verify
```

For a C-only bug fix, edit only the needed `port/impl` or MIDP file, then run
`python tools/port.py verify`. Do not edit generated C files by hand. Keep
unrelated code and the user-owned `AGENTS.md` unchanged.
