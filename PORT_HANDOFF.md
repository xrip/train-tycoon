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

## 5. Current state

- Toolchain: `inventory`, `gen`, `build`, `verify` all green.
- Methods done: **98 / 196 (50%)** — 97 full + 1 partial (`update`).
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
  full state-30 control branch (Java L9024-9282) are done. The port builds and
  passes smoke, but core simulation calls are still stubs; menu/popup states are
  not done, and the smoke check does not drive the game into state 30.
- **Menu system is functional** — `handleBuildMenuState` (~786 L) is ported, so
  selecting menu items drives the state transitions (new game → scenario →
  `setState(25)` map-gen, options, etc.).
- **Biggest remaining gate to gameplay:** replace the direct stubs used by the
  state-30 branch: `updateTrain`, `everyMonthlyTick`, and `tickInputOrAi`.
  Twenty-three helpers are now ported in `port/impl/sim_tick.c`, including:
  `dayResetP`, `advanceConstructionTimers`, `maybeAITick`, `dayResetAg`,
  `revenueOrYear`, `everyTwoTick`, `countCarsInTrain`, `maybeAiTakeTurn`,
  `dayResetAd`, `postRevenueReset`, `isTrainHeadCarAt`,
  `removeTrainFromMapInlined`, `mergeTrainAt`, `archiveOrRecall`, and
  `computeYearEndMetrics`, `handleCursorClickInlined`, `pickValidForestTile`,
  `everyTwoTickZ`, and `newYearNotify`. The milestone save path still reaches
  the pending `saveGame`; port the save chain before save behavior is complete.
  Then port the remaining UI-draw methods
  (`paintMain`, `drawMoneyTicker`, `drawConfirmBar`, `drawConfirmDialog`,
  `drawBuildMenuCursor/Row`, `q` [372 L], `computeTrainCost`, …).
- Raw JAR resources (`1`..`6`, `ico.png`) are extracted to `resources/`.

## 6. Known issues / flags

- **`mappings.txt` collision:** two fields named `buildMenuIndex` (Java L162
  `int`, L215 `byte`). The Java would not compile with `javac`. The port tool
  renamed the second to `buildMenuIndex_2` and prints a WARNING on `inventory`.
  Fix the mapping upstream and pick distinct names.
- **MSVC has no C23 `nullptr`** — `port/main.c` defines a guarded fallback.
- No behavioural test harness yet; `verify` only checks it builds and runs.

## 6a. Next-session plan: complete playable simulation (state 30)

The map-gen chain and the state-30 control branch are **done**. Selecting
"new game" generates a world, paints it via `renderMap`, and enters the sim
tick. Core calls are still stubs, so simulation and interaction are incomplete.
Continue from here:

1. Port the three direct pending helpers called by state 30, starting with their
   smallest leaf dependencies: `updateTrain`, `everyMonthlyTick`, and
   `tickInputOrAi`.
2. The UI-draw methods called from the state-30/40 paint paths:
   `paintMain`, `drawMoneyTicker`, `drawConfirmBar`, `drawConfirmDialog`,
   `drawBuildMenuCursor`, `drawBuildMenuRow`, `drawHeaderBar`, `drawNumber`.
3. Interaction/build: `openBuildMenu`, `cycleBuildMenuIndex`,
   `viewportPanKey`/`viewportZoomKey`, `computeViewportBounds`,
   `demolishAtCursor`, `pickTrainsAtCursor`, `buildPlaceTrackPiece`,
   `computeTrainCost`, `buyTrainAtCursor`, and the pathfinding cluster
   (`tryBuildPath`, `validatePathStep`, `findPathStep`, `floodFillPath`,
   `hPath`, `clearPathDistanceGrid`, `q` [372 L]).

Use `python tools/port.py next` (picks smallest pending leaves) and
`show NAME` throughout. Each is a mechanical faithful port like the 98 already
done. `verify` after each method, then commit.

## 7. Build requirements

- CMake ≥ 3.25, MSVC (VS 2022 Build Tools) or GCC/Clang with C23.
- Python 3 for `tools/port.py`.
- `cmake -S . -B build/port_build -G "Visual Studio 17 2022" -A x64` then
  `cmake --build build/port_build --config Debug`.
