# PORT_HANDOFF — Train Tycoon → native Win32 / C23

This is the **resumable** handoff for the C port. Anyone can pick up the work
from any point with the toolchain below; no knowledge lives only in one head.

## 1. Goal

Port the J2ME game (`src/deobfuscated/GameCanvas.java`, 9,727 lines, 441 fields,
196 methods) to a native Win32 C23 program on the MiniFB backend. Strategy:
**MIDP shim + faithful 1:1 transliteration**. Sound (MIDI `/6`) and high-score
networking are **stubbed for now** (by request).

## 2. The two layers (what you edit vs what is generated)

| Layer | Path | Edit by hand? |
|---|---|---|
| MIDP shim + Win32 entry | `port/*.c`, `port/*.h`, `MiniFB.h` | Yes (stable, rarely changes) |
| Generated scaffolding | `port/generated/*` | **No** — `port.py gen` rewrites it |
| Ported method bodies (the game) | `port/impl/*.c` | **Yes** — this is the port |

`port/generated/` holds: `game_state.h` (the 441-field `GameCanvas` struct +
static fields), `game_state.c` (`gc_fields_init` — all initializers + data
tables + array allocation), `game_methods.h` (prototypes for every method),
`game_stubs.c` (empty stubs for methods **not yet ported**).

## 3. The toolchain — `tools/port.py`

Single source of truth is the Java. The manifest `tools/port_manifest.json`
records each method's port status and each field's C name/type.

```
python tools/port.py inventory   # (re)scan Java -> manifest (idempotent, safe)
python tools/port.py status [-v] # ported X/196 methods; -v lists pending
python tools/port.py next        # suggest smallest pending methods (leaves)
python tools/port.py show NAME   # print a method's Java body to port it
python tools/port.py gen         # regenerate port/generated/*
python tools/port.py mark NAME ported   # flip status (stops stubbing it)
python tools/port.py build       # cmake build (VS 2022, x64)
python tools/port.py verify      # build + headless smoke run (120 frames, exit 0)
```

## 4. How to port one method (the loop)

1. `python tools/port.py next` — pick a method (start with leaves).
2. `python tools/port.py show NAME` — read its Java body.
3. Write the C body in a file under `port/impl/` (group by theme, e.g.
   `geometry.c`, `save.c`, `render.c`). Signature = the prototype in
   `port/generated/game_methods.h`.
4. `python tools/port.py mark NAME ported`
5. `python tools/port.py gen && python tools/port.py verify`
6. Commit.

### Translation conventions (Java → C)

- Instance field `this.foo` → `gc->foo`; static field `foo` → `gcs_foo`.
- Method call `this.bar(x)` → `gc_bar(gc, x)` (see `game_methods.h` for the
  exact C name; overloads get a `__N` suffix).
- Types: `int`→`int32_t`, `byte`→`int8_t`, `short`→`int16_t`, `long`→`int64_t`,
  `boolean`→`bool`. Arrays add `*` per dimension (`byte[][]`→`int8_t**`).
- `rng.nextInt(n)` → `midp_random_next_int_bound(&gc->rng, n)`.
- Graphics/Image/Vector/ByteArrayOutputStream/RecordStore → the `midp_*` shim
  in `port/midp.h`.
- `run()` is **not** ported as a blocking loop: the host (`port/main.c`) owns the
  frame loop and calls `gc_update` + `gc_paint` each frame. Port the *body* of
  the run() iteration into `update()`/`paint()` as usual.
- **Array `.length`**: raw C pointers carry no length, so every game array is
  allocated length-tracked. In hand-ported code use `GC_NEW(type, n)` to
  allocate and `midp_arr_len(ptr)` to read a length (the generated init already
  uses `midp_alloc_arr`). `System.arraycopy(src,sp,dst,dp,len)` → `memcpy(dst+dp,
  src+sp, len)` (or `memmove` if regions overlap).
- **Naming while porting:** do not add new C code with a recoverable obfuscated
  member name. Use behavior plus descriptor-precise xrefs to choose a clear
  name, add it to `tools/mappings.txt`, rerun `deobf`, then run
  `port.py inventory` and `port.py gen` before finishing the C port. Ask when
  the evidence does not support one clear name. Decompiler-only local names may
  be made clear directly in C.

## 5. Current state

- Toolchain: `inventory`, `gen`, `build`, `verify` all green.
- Methods done: **180 / 196 (91%)** — 179 full + 1 partial (`update`).
  Run `python tools/port.py status [-v]` for the live count and pending list.
- **Boots to the animated LifeWeb splash logo** — the full asset pipeline is
  proven: resource load (`/1`) → XOR decode → custom PNG reconstruction
  (`decodePngFromSpriteStream`) → **real PNG decode (vendored `stb_image.h`)** →
  ARGB blit → `paint()` state machine → `update()` loop → host frame loop.
- Window opens (240×320, 2× scale, 32bpp); frame loop runs; closes clean.
- **Renders the full title screen** (Train Tycoon logo + locomotive art + a
  menu dialog with bitmap-font text/buttons) and the **menu is navigable**
  (`handleKey` + `registerCityEntryInlined` move the cursor).
- **Map-generation chain is ported and builds clean.** `update()` state 25
  (map-gen driver) reads `/5` scenario params, then increments `loadStep`
  through `initBuildMenuForScenario` + `loadState25Setup` until
  `mapGenTilesDone == totalTilesToGenerate`, then `setState(30)`. The terrain/
  placement helpers live in `port/impl/map_gen.c` (`randomCityTile`,
  `addCityEntry`, `registerTrain`, `toggleFlagInlined`, `smoothTileAtInlined`,
  `raiseTerrainArea`, `lowerTerrainArea`, `trackBuildStepInlined`,
  `placeTerrainPatch`, `placeTerrainFeature`, `loadState25Setup`).
- **Isometric world renderer ported:** `renderMap` (`port/impl/render_map.c`,
  155 L) draws terrain into the offscreen backbuffer when the viewport moves,
  blits it, then overlays dynamic tiles, trains and the build menu.
- `update()` is **partial**: boot/loading (0/1/2/4/10), map-gen (25), and the
  full state-30 simulation branch (Java L9024-9282) are done. The train
  movement/load/delivery chain is ported in `port/impl/train_tick.c`. The port
  builds and passes smoke, but menu/popup update states are not done, and the
  smoke check does not drive the game into state 30.
- **Menu system is functional** — `handleBuildMenuState` (~786 L) is ported, so
  selecting menu items drives the state transitions (new game → scenario →
  `setState(25)` map-gen, options, etc.).
- **The direct state-30 simulation gate is complete.** `updateTrain`,
  `tickInputOrAi`, `everyTwoTick`, `everyMonthlyTick`,
  `advanceConstructionTimers`, and `maybeAITick` and their direct dependency
  chains are ported. Monthly offers live in `port/impl/economy_tick.c`.
  Twenty-four helpers are now ported in `port/impl/sim_tick.c`, including:
  `dayResetP`, `advanceConstructionTimers`, `maybeAITick`, `dayResetAg`,
  `revenueOrYear`, `everyTwoTick`, `countCarsInTrain`, `maybeAiTakeTurn`,
  `dayResetAd`, `postRevenueReset`, `isTrainHeadCarAt`,
  `removeTrainFromMapInlined`, `mergeTrainAt`, `archiveOrRecall`, and
  `computeYearEndMetrics`, `handleCursorClickInlined`, `pickValidForestTile`,
  `everyTwoTickZ`, `newYearNotify`, and `tickInputOrAi`. The milestone save path
  still reaches the pending `saveGame`; port the save chain before save behavior is complete.
- **The direct state-30/40 gameplay UI is ported.** `port/impl/gameplay_ui.c`
  contains the main header, money ticker, confirm dialog/bar, build-menu rows,
  and build cursor/selection drawing. `isTileBuildable` is in `terrain.c`.
  The smoke test still does not enter state 30, so this path has build proof but
  not end-to-end runtime proof yet.
- **Basic state-30 control is ported.** Build-menu item cycling, map panning,
  cursor movement with view limits, zoom movement, and isometric tile picking
  are in `port/impl/input.c`.
- **Direct build actions are ported.** `port/impl/build_actions.c` has the build
  menu action, track preview/confirm/cancel, track and station placement,
  removal, train picking, service-slot allocation, train cost, and city service
  counts. The direct `openBuildMenu` path no longer enters generated stubs.
- **Train buying and upgrades are ported.** The same file now has train-slot
  setup, engine and cargo cost use, cargo option counts, buy, and upgrade
  actions. The related menu-dispatch calls no longer enter generated stubs.
- **Build pathfinding is ported.** `port/impl/pathfinding.c` has grid setup,
  flood relaxation, tunnel links, path choice, preview tile shaping, track
  owner matching, cost work, and track counting. `port.py verify` now runs a
  smoke-only 5x5 path build and checks for preview tiles and a non-zero cost.
- **Save/load is ported.** State fields, all four map matrices, RLE, record-store
  writes, and restore allocation are in `port/impl/save.c`. The smoke check now
  makes an in-memory snapshot, compresses and expands it, compares every byte,
  and loads it back to check key fields. It does not change `TTStore.rms`.
- **Remaining city/industry route setup is ported.** Random endpoint selection,
  path construction, station extension, initial train and wagon creation, and
  owner assignment are in `port/impl/map_gen.c`.
- Raw JAR resources (`1`..`6`, `ico.png`) are extracted to `resources/`.

## 6. Known issues / flags

- **Resolved mapping collision:** the two old `buildMenuIndex` mappings are now
  `buildMenuItemIndex` (general menu cursor) and `offerRowIndex` (6-slot offer
  list). `port.py inventory` no longer gives a duplicate-field warning.
- **MSVC has no C23 `nullptr`** — `port/main.c` defines a guarded fallback.
- No behavioural test harness yet; `verify` only checks it builds and runs.
- CFR dropped a `break` from `computeTrackBuildInlined`: bytecode offsets
  106-134 stop at the first non-empty tile. The C port follows the bytecode,
  not the incorrect decompiled loop text.
- `pathSearchLimit` is a low-confidence field name: the game sets the field to
  10 before path search, but no bytecode reads it.
- `autoExtendTrackStation` tests the “track above” flag for both vertical
  extension directions. Bytecode confirms this odd original behavior; the C
  port keeps it.

## 6a. Next-session plan: make state 30 playable

The map-gen and state-30 simulation chains are **done**. Selecting "new game"
generates a world, paints it via `renderMap`, and enters the sim tick. Gameplay
UI, input, building, and runtime proof are still incomplete. Continue from here:

1. Port the remaining large UI/data method `q` [372 L] after giving it a clear
   upstream name.
2. Port the remaining score/graph/text helpers and lifecycle calls.
2. Add a smoke path that enters state 30 and exercises one full paint/input
   frame before treating gameplay rendering as behaviorally proved.

Use `python tools/port.py next` (picks smallest pending leaves) and
`show NAME` throughout. Each is a mechanical faithful port like the 180 already
done. `verify` after each method, then commit.

## 7. Build requirements

- CMake ≥ 3.25, MSVC (VS 2022 Build Tools) or GCC/Clang with C23.
- Python 3 for `tools/port.py`.
- `cmake -S . -B build/port_build -G "Visual Studio 17 2022" -A x64` then
  `cmake --build build/port_build --config Debug`.
