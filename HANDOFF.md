# Train Tycoon native refactor handoff

## Project and target

This repository has a native Windows Train Tycoon game and its old J2ME
decompile. All normal development is in `win32/`. The decompile is only a fact
source when native behavior or a name is not clear.

The target is a clear C++20 game in the C with Classes style. Classes own
state and keep simple rules. Plain data stays in structs. Small operations stay
as free functions. Normal `std::array`, `std::vector`, `std::span`,
`std::optional`, `std::filesystem::path`, and `std::unique_ptr` are welcome.
Template design systems, deep class trees, and general frameworks are not.

Behavior must stay the same during refactor work. This includes random call
order, draw order, tick order, signed byte rules, stable train slot numbers,
resource values, and the current `.rms` save byte order.

## Current state

Branch: `master`.

Active item: `OWN-04`, remove raw ownership and the array-length registry.

`OWN-02` and `OWN-03` are complete. Every fixed-size raw state array and constant
table uses `std::array`, and every map grid and matrix uses a row-owning
`Grid<T>` (`win32/src/support/grid.h`) or nested `std::array`. The save helpers
gained size-explicit `*Span` cores plus `std::array`/`Grid` overloads so save
byte order is unchanged, and dead `dk`/`j` state was removed. No raw `**` matrix
field remains. Debug and Release full gates passed on 2026-07-15.

Six dynamically-sized 1D raw arrays remain for OWN-04: `spriteOffsetTable`,
`spriteData`, `spriteIndexTable`, `tileOffsetX`, `tileOffsetY`, and the growable
`saveBuffer`.

The work tree is clean except for root `AGENTS.md`. It is a user-owned,
untracked instruction file. Do not add, change, or remove it.

## Work completed

The work-control plan, fixed tests, strict C++20 build, and behavior goldens
were already complete before the latest ownership work. See
`win32/docs/refactor/PLAN.md` and `STATUS.md`.

Recent completed changes are:

- `ce6ffd1` uses plain named resources and an owned `ResourceReader`. Files 1
  to 5 were decoded once, so native run-time XOR code is gone. This makes the
  native resource form clear and independent from the old JAR.
- `088896f` gives image pixels and game images standard ownership. `Image`
  owns pixels, while `Surface` only borrows them. This removes image leaks and
  makes drawing lifetime clear.
- `5cd70a3` gives record stores RAII ownership and a path save root. Record
  bytes and exact `.rms` form remain the same.
- `8034497` puts all WinMM MIDI state in `WinMmMusicPlayer`. `GameCanvas` owns
  one small `MusicOutput`; tests use `SilentMusicOutput`. This removes global
  MCI state and lets all normal tests run without MIDI hardware or a `winmm`
  link.
- `67a8f97` moves short-lived PNG, city-seen, visible-train, and text buffers
  to standard containers. Text values own `std::vector<int8_t>`, missing
  dialog text uses `std::optional`, and drawing borrows `std::span`.
- `a8be35f` moves the terrain-patch matrix and save-growth work buffer to
  `std::vector`. It also renames `gc_m` to `gc_readPackedTextCodes`, because
  its use is now clear.
- `1760b20` closes `OWN-01` in the work records after Debug and Release full
  proof.
- `25f7edd` starts `OWN-02` with owned selection-corner arrays.

All these changes keep fixed map, save, input, and frame values unchanged.

## Current design

- `win32/src/main.cpp` owns Win32 start and the frame loop.
- `win32/src/game_state.*` still holds the large `GameCanvas` state. This is
  the main old shape still to remove in later stages.
- `win32/src/game/*.cpp` holds game rules, state changes, save work,
  simulation, UI, and drawing.
- `win32/src/support/` holds owned resource, image, record, random, byte, input,
  and music support.
- `win32/src/platform/winmm_music_player.*` is the WinMM MIDI boundary.
- `win32/src/midp_*` is the small old compatibility layer. Its array-length
  registry still supports many raw game arrays.
- `win32/resources/` has seven plain named resource files.
- `win32/tests/test_smoke.cpp` has fast rule checks and full fixed-output
  checks for all 13 scenarios, input, saves, resources, and frames.

Important decisions:

- C++ is used as C with Classes, not as a template design system.
- Public headers use full `std::` names. A clear namespace alias is allowed in
  a `.cpp` file.
- One small `Grid<T>` is allowed later for maps.
- A refactor commit must not change a golden value.
- A game bug needs its own test, evidence, work item, and commit.
- Any recoverable old name touched by work must get a clear behavior name.

## What works

- The game builds and starts as a native x64 Windows program.
- All 13 fixed-seed scenario maps generate with stable Debug and Release
  values.
- Software drawing and fixed frame output work.
- Save and load tests keep the old byte form.
- Plain resource input, PNG data, record files, input, path work, and fixed
  simulation checks pass.
- MIDI load, play rules, stop, and volume rules pass with silent test output.
  A real MIDI device check is opt-in.

## Verification at handoff

Run from `win32/` on 2026-07-15:

```powershell
cmake --build --preset build-debug
ctest --preset test-debug-full
cmake --build --preset build-release
ctest --preset test-release-full
```

Exact result:

- Debug build passed under strict first-party `/W4 /permissive- /WX` rules.
- Debug tests: 2/2 passed; fast 0.01 s, full 0.17 s, total 0.19 s.
- Release build passed.
- Release tests: 2/2 passed; fast 0.01 s, full 0.06 s, total 0.07 s.
- No test failed.

To run the game:

```powershell
cd win32
cmake --preset windows-x64
cmake --build --preset build-debug
.\build\Debug\train_tycoon.exe
```

For a short real MIDI device check:

```powershell
$env:TRAIN_TYCOON_MIDI_TEST_MS='3000'
.\build\Debug\train_tycoon.exe
Remove-Item Env:\TRAIN_TYCOON_MIDI_TEST_MS
```

## Incomplete work and risks

- `OWN-04` is in progress. Six dynamically-sized 1D raw arrays remain:
  `spriteOffsetTable`, `spriteData`, `spriteIndexTable`, `tileOffsetX`,
  `tileOffsetY`, and `saveBuffer`.
- `saveBuffer` is the risky one: it grows through
  `gc_ensureSaveBufferCapacity` and is read with pointer arithmetic and explicit
  offsets in the save codec. Moving it to `std::vector<int8_t>` must keep the
  exact compressed byte order.
- After the six move, remove the dead raw save wrappers and matrix helpers, the
  `gc_tracked_codes(const int8_t*)` inline, and the `midp_alloc_arr` /
  `midp_arr_len` registry entirely; that is the point of OWN-04.
- `GameCanvas`, `game_methods.h`, short state names, and Java-shape comments
  remain until later model and clean stages.
- `gc_computeMenuRowDisplayValue` may divide by zero when money is zero. Do not
  fix it inside refactor work; it needs a separate bug test and decision.
- High-score network work is a known no-op and is outside this refactor.
- The old game has MIDI music but no sound-effect file or sound-effect call.
  New WAV sound would be a new feature, not refactor work.
- A manual play pass is still needed for boot/menu, scenario start, pan/zoom,
  track work, train work, date change, save/load, and real MIDI.

## Next work, in order

1. Move the sprite and tile-offset load buffers (`spriteOffsetTable`,
   `spriteData`, `spriteIndexTable`, `tileOffsetX`, `tileOffsetY`) to
   `std::vector`. Check every call site; they are load-time buffers.
2. Move `saveBuffer` to `std::vector<int8_t>` with explicit offsets, keeping the
   exact compressed save byte order. Verify with the save/load golden.
3. Remove the dead raw save wrappers and matrix helpers, the
   `gc_tracked_codes(const int8_t*)` inline, and the `midp_alloc_arr` /
   `midp_arr_len` registry once nothing uses them.
4. Close OWN-04 only after no raw owning array remains. Run Debug and Release
   full gates and update `CURRENT.md`, `STATUS.md`, `BASELINE.md`, and
   `DECISIONS.md`.
5. Continue the main plan in strict order: model, save, world, path, fleet,
   economy, scenario, simulation, build, assets, render, UI, app, clean, docs,
   and release stages.

## Files to read first

- `AGENTS.md` - user rules; keep it untracked.
- `CLAUDE.md` - project rules and source map.
- `win32/docs/refactor/CURRENT.md` - exact active leaf and last proof.
- `win32/docs/refactor/PLAN.md` - full work order and gates.
- `win32/docs/refactor/STATUS.md` - item states.
- `win32/docs/refactor/DECISIONS.md` - C++ style and recovered names.
- `win32/docs/refactor/KNOWN_ISSUES.md` - known bugs and fixed port defects.
- `win32/src/game_state.h` and `game_state.cpp` - state fields and init; the
  raw `**` matrices here are the active OWN-03 data.
- `win32/src/game/save.cpp` - save codec and the `IO_*`/matrix macros that reach
  map grids.
- `win32/src/game/save_arrays.cpp` - array save helpers, now split into
  size-explicit `*Span` cores plus registry wrappers; the `*MatrixInlined`
  helpers still need the same split for saved matrices.
- `win32/tests/test_smoke.cpp` - fixed behavior checks and golden values.
- `win32/CMakeLists.txt` and `CMakePresets.json` - build and test setup.
