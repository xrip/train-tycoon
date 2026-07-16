# Train Tycoon native refactor — handoff

## Project purpose and target state

This repository holds a native Windows **Train Tycoon** game (`win32/`) plus the
old J2ME decompile kept only as a behavior reference. All current development is
in `win32/`; treat it as a normal native C++20 project.

The target is a clear C++20 game in the **C-with-Classes** style: classes own
state and simple rules, plain data lives in structs, small operations are free
functions, and standard `std::array`/`std::vector`/`std::span`/`std::optional`/
`std::unique_ptr`/`std::filesystem::path` are welcome. No template metaprogramming,
CRTP, concepts, policy systems, deep class trees, or event frameworks.

**Behavior must stay identical through every refactor commit.** That means the
random call order, draw order, tick order, signed-byte rules, stable train slot
ids, resource values, and the current `.rms` save byte order are all preserved.
The guard is a golden-digest test suite (13 fixed-seed scenario maps, saves,
input, resources, and frames). A refactor commit must never move a golden.

The end goal (see `win32/docs/refactor/PLAN.md`) is that a small `Game` owns clear
parts — `World`, `Pathfinder`, `Fleet`, `Economy`, `Scenario`, `BuildController`,
`UiController`, `Assets`, `Renderer`, `SaveCodec` — instead of one large
`GameCanvas` state bag.

## Current state (branch `master`)

The refactor plan runs in strict order across stages 0–5. Stages 0–2 (behavior
lock, mechanical C++20 move, ownership/support) were complete before this work.
Progress in stage 3 (game model):

| Item | State | Note |
|---|---|---|
| MODEL-01 | done | Named enums/constants |
| SAVE-01 | done | Explicit save codec |
| WORLD-01 | done | `World` owns map grids/dimensions |
| WORLD-02 | done | Generation stages + tile-range taxonomy |
| PATH-01 | done | Endpoints named `pathAnchor/Target`; cost/owner deferred to BUILD-01 |
| FLEET-01 | **next** | Train/service/city records (not yet started) |
| FLEET-02…RELEASE-01 | queued | Not started |

Authoritative status is `win32/docs/refactor/STATUS.md`. The active leaf and next
action are in `win32/docs/refactor/CURRENT.md`. Design decisions and every
recovered name are in `win32/docs/refactor/DECISIONS.md`.

The working tree is clean except the untracked root `AGENTS.md`, which is a
user-owned instruction file. **Do not add, change, or remove it.**

## What changed this session and why

All commits below are behavior-identical (Debug + Release full gates green, no
golden moved). Range `bf87d96..HEAD`:

**MODEL-01 — name the magic numbers (`win32/src/game_constants.h`):**
- `7976ccf` / `c95d412` — `enum class GameState : int32_t` for the game-state
  codes, with a unary `operator+` so the `int32_t gameState` field, switch
  subjects, and case labels read as e.g. `+GameState::Playing`. The field stays
  `int32_t` for save/arithmetic safety. Reason: the state machine was opaque
  numeric literals across ~10 files.
- `d73e598` — `track::` bitfield constants (`PRESENT`, `OWNER_MASK`,
  crossing sub-owner mask/shift, AI piece-code cycle) and `tile::CROSSING` (76),
  applied at strongly-evidenced sites. Reason: the `trackMap` byte packs a
  "present" flag with an owner field, previously all raw hex/decimal.
- `f49e7de` — `transition::` bases (offer-type 40, offer-alt 60, scenario-notice
  80) and closed MODEL-01. Per-screen menu ids were **deferred to UI-01** because
  they are UI navigation whose meaning only becomes clear with the menu builder;
  naming them now would be guesswork.

**SAVE-01 — make the save codec explicit (`win32/src/game/save.cpp`,
`save_arrays.cpp`, `game_constants.h`):**
- `9a0a5c5` — `save::MODE_WRITE/READ/SKIP` for `savePendingMode` and the
  `gc_loadGame` mode arg, applied everywhere; `gc_loadGame(n, n2)` params renamed
  to `(slot, mode)`. Reason: one `IO_*` field walk serves save and load; the
  direction was an unlabeled 0/1/2.
- `e1952f0` — `save::OP_SAVE_MAIN/OP_LOAD_MAIN/OP_RESET_SKIP` for the
  `gc_initSaveOrLoad` selector, and renamed `gc_readShortFromBuffer` →
  `gc_readSaveByteUnsigned` (it reads one unsigned byte, not a short). Recorded
  that `gc_initSaveOrLoad` modes 4/5 are never called (dead-branch smell). Closed
  SAVE-01 (`78e5e52`); the `SaveCodec` class boundary is deferred to the model
  stages, where the codec can read owned state instead of `GameCanvas`.

**WORLD-01 — give the map its own data (`win32/src/world.h`):**
- `b474584` — `struct World` owning the four map grids (`tileMap`, `trackMap`,
  `tileElevation`, `heightMap`); `GameCanvas` holds one `World world;` member and
  every `gc->tileMap` style access became `gc->world.tileMap` (~470 sites, a pure
  member-path change).
- `f5ed65a` — moved `mapWidth`, `mapHeight`, `mapHalfWidth`, `mapScreenWidth`,
  `mapScreenHeight` onto `World`; the two saved dims keep byte order via
  `IO_INT(world.mapWidth)`/`IO_INT(world.mapHeight)`.
- `f114f47` — added `World::in_bounds(x, y)` and adopted it at the full
  out-of-bounds checks. Closed WORLD-01; entangled map queries move with their
  co-dependencies (Fleet/Economy) later.

**WORLD-02 — clarify terrain/generation (`win32/src/game/map_gen.cpp`):**
- `27537bb` — renamed the RNG helper `rint` → `nextRngInt` (it shadowed libm
  `rint`) and removed a dead `RNG` macro.
- `f75c86e` — named the generator state machine `gc->mapGenStage` 0..10 as
  `mapgen::STAGE_*` (init, raise, lower, patches, six feature passes, scatter
  obstacles).
- `fce3217` / `24a49f8` — recorded the tile-shape taxonomy evidence, then named
  the range groups that are used as clear membership tests: `tile::SCENERY_FIRST/
  LAST` (29..33), `tile::BUILT_FIRST` (34), `tile::CITY_ZONE_LAST` (40), applied
  by each predicate's intent (is-scenery vs is-built vs city-zone). Closed
  WORLD-02. The isometric **slope/cliff shape ids 3..28 stay literal** — they
  encode sprite orientations and need the tile art to name without guessing.

**PATH-01 — path-search scratch (`win32/src/pathfinder.h`):**
- `1e5306f` — `struct Pathfinder` owning `distanceGrid` (was `pathDistanceGrid`),
  `searchLimit`, `nextX`, `nextY`, `tieBreakCounter`, renamed off `GameCanvas`.
  These five are pathfinding-only (terrain just reads the grid).
- (this session) — named the shared endpoints by behavior: `placementX/Y` →
  `pathAnchorX/Y` (reverse-BFS seed and back-walk terminus; city station / first
  player pick) and `demolishX/Y` → `pathTargetX/Y` (flood-fill target and
  back-walk start; industry station / second pick). The old `demolish` name was
  wrong — the real demolish action never uses these. The cost accumulator
  `selectionCost` and owner codes `previewTrackOwnerCode`/`existingTrackOwnerCode`
  are deferred to BUILD-01 (build/preview state, not search scratch). **PATH-01
  closed**; Debug and Release full gates green, no golden moved.

## Current architecture and key design decisions

- `win32/src/main.cpp` — Win32 start and the frame loop.
- `win32/src/game_state.*` — the large `GameCanvas` state bag (still the main old
  shape). It now embeds `World world;` and `Pathfinder pathfinder;`.
- `win32/src/world.h`, `pathfinder.h`, `game_constants.h` — the new owned model
  types and the named constants (`GameState`, `track::`, `tile::`, `transition::`,
  `save::`; plus file-local `mapgen::` in `map_gen.cpp`).
- `win32/src/game/*.cpp` — game rules, state changes, save codec, simulation, UI,
  and drawing (still free functions taking `GameCanvas*`).
- `win32/src/support/` — owned resource, image, record, random, byte, input, music.
- `win32/src/platform/winmm_music_player.*` — the WinMM MIDI boundary.
- `win32/tests/test_smoke.cpp` — fast rule checks and full golden digests.

Decisions worth knowing (full list in `DECISIONS.md`):
- C++ as C-with-Classes; public headers keep full `std::` names.
- A refactor commit must not change a golden value; a real game fix needs its own
  test, evidence, and item.
- Extraction pattern that has worked cleanly here: introduce the owned type,
  make it a `GameCanvas` member, do a mechanical `gc->field` → `gc->owner.field`
  rename (sed across `src/` and `tests/` with word boundaries), fix any `IO_*`
  save-macro sites, then run both full gates before committing.
- Do not rename by guesswork. Where evidence is thin (per-screen menu ids, slope
  tile shapes, entangled queries), the literal stays and the evidence/deferral is
  recorded.

## What works

- Builds and runs as a native x64 Windows program.
- All 13 fixed-seed scenario maps generate with stable Debug and Release digests.
- Software drawing, fixed frame output, save/load byte form, plain resources, PNG
  data, record files, input, path work, and fixed simulation checks pass.
- MIDI load/play/stop/volume rules pass with a silent test output; a real device
  check is opt-in.

## What is incomplete or deferred

- **FLEET-01 is the active item** (not yet started). Extract train, service-site,
  and city-record state into a `Fleet` owned type with stable slot IDs, reusing
  the proven extraction pattern. This also unblocks the entangled map queries
  deferred from WORLD-01.
- Deferred, recorded work: path cost/owner fields (`selectionCost`,
  `previewTrackOwnerCode`, `existingTrackOwnerCode`) → BUILD-01; per-screen menu
  transition ids (→ UI-01); the isometric slope/cliff tile ids 3..28 (need sprite
  art); entangled map-query operations (move with Fleet/Economy); the `SaveCodec`
  class boundary.
- Stage 3 items FLEET-01/02, ECON-01, SCENARIO-01, SIM-01, BUILD-01 not started;
  all of stage 4 (ASSET/RENDER/UI/APP) and stage 5 (CLEAN/DOC/RELEASE) not started.

## Known issues, constraints, risks, technical debt

- `gc_computeMenuRowDisplayValue` may divide by zero when money is zero. It needs
  its own bug test and decision, **not** a refactor edit.
- `gc_initSaveOrLoad` selector modes 4 and 5 are never called (dead branch);
  recorded as a smell for a separate cleanup item, not removed here.
- High-score networking is an intentional no-op. The old game has MIDI music but
  no sound-effect file; WAV sound would be a new feature, out of scope.
- Git reports CRLF conversion warnings on commit; harmless (repo files are LF).
- **Autonomous limits:** UI-01 and RELEASE-01 exits require a **manual play-through**
  (boot/menu/build/save/load), and RELEASE-01 also needs a **real MIDI-device
  check**. These cannot be completed without a human.

## How to build, run, test, verify

From `win32/`:

```powershell
cmake --preset windows-x64
cmake --build --preset build-debug
ctest --preset test-debug-full
cmake --build --preset build-release
ctest --preset test-release-full
```

Run the game: `win32/build/Debug/train_tycoon.exe` (resources are copied beside
the executable). Short real-MIDI device check:

```powershell
$env:TRAIN_TYCOON_MIDI_TEST_MS='3000'; .\build\Debug\train_tycoon.exe; Remove-Item Env:\TRAIN_TYCOON_MIDI_TEST_MS
```

### Verification at this handoff (2026-07-16, after PATH-01 close)

- Debug build clean under strict first-party `/W4 /permissive- /WX`.
- Debug tests: 2/2 passed (fast 0.02 s, full 0.37 s).
- Release build clean.
- Release tests: 2/2 passed (fast 0.01 s, full 0.05 s).
- No test failed; no golden value changed.

## Current objective and next tasks (priority order)

1. **FLEET-01** — trains, service, and city records with stable slot ids. Reuse
   the proven extraction pattern (owned type → member → mechanical rename →
   gates). This also unblocks the deferred entangled map queries in `World`.
2. **FLEET-02, ECON-01, SCENARIO-01, SIM-01, BUILD-01** — remaining stage-3
   model classes, in that order. (BUILD-01 also picks up the deferred path
   cost/owner fields.)
3. Stage 4 (ASSET/RENDER/UI/APP) and stage 5 (CLEAN/DOC/RELEASE). Note the
   human-gated manual-play and MIDI-device checks at UI-01 and RELEASE-01.
4. Opportunistic, evidence-first cleanups when the co-dependencies land: slope
   tile ids (3..28), per-screen menu ids, the `SaveCodec` class boundary, and the
   dead `gc_initSaveOrLoad` 4/5 branches.

## Files to read first

- `AGENTS.md` — user rules; keep it untracked.
- `CLAUDE.md` — project rules and source map.
- `win32/docs/refactor/CURRENT.md` — active leaf and last proof.
- `win32/docs/refactor/PLAN.md` — full work order, gates, and target parts.
- `win32/docs/refactor/STATUS.md` — item states.
- `win32/docs/refactor/DECISIONS.md` — C++ style and every recovered name/decision
  (D-011..D-014 are this session's SAVE/WORLD/PATH work).
- `win32/src/game_constants.h`, `world.h`, `pathfinder.h` — the new owned types
  and named constants.
- `win32/src/game_state.h` — `GameCanvas`, now embedding `World` and `Pathfinder`.
- `win32/src/game/save.cpp` + `save_arrays.cpp` — the save codec and `IO_*` macros.
- `win32/src/game/pathfinding.cpp` — the active PATH-01 code.
- `win32/tests/test_smoke.cpp` — golden digests and rule checks.
- `win32/CMakeLists.txt`, `CMakePresets.json` — build and test setup.
