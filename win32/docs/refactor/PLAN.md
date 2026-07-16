# Train Tycoon refactor plan

## Goal

Make `win32/` a clear C++20 project while keeping game behavior, resource
reading, random call order, draw order, tick order, and the current `.rms`
file form.

This project uses C++ as C with Classes:

- classes own state and keep simple rules true;
- structs hold plain data;
- free functions hold small, pure operations;
- plain fixed arrays, plus `std::vector`, `std::span`, `std::string`,
  `std::filesystem::path`, and `std::unique_ptr` are welcome when they make
  ownership or size clear;
- a long standard namespace may have a clear alias in a `.cpp` file, such as
  `namespace filesystem = std::filesystem`; public headers keep full `std::`
  names;
- one small `Grid<T>` is allowed;
- template metaprogramming, CRTP, concepts, policy systems, generic
  frameworks, deep class trees, and event systems are not allowed.

## Behavior rules

- `win32/` is the source of truth.
- The old Java decompile is only evidence for an unclear name or game bug.
- A refactor commit must not change a golden value.
- A game bug fix needs its own test, evidence, work item, and commit.
- Keep all numeric save values, signed-byte behavior, stable train slot IDs,
  random calls, and simulation calls in the same order.
- `Grid<T>` stores old `grid[x][y]` data as `x * height + y`.
- Do not use an existing user `TTStore.rms` as test data.

## Target parts

- `train_tycoon_support`: values, random, byte buffers, resources, images,
  software graphics, and record files.
- `train_tycoon_game`: game model, rules, save codec, UI, and renderer.
- `train_tycoon_win32`: MiniFB window, Win32 input and clock, and WinMM MIDI.
- `train_tycoon`: small application start and frame loop.
- `train_tycoon_tests`: vendored doctest tests.

`Game` will own and join these clear parts:

- `World`
- `Pathfinder`
- `Fleet`
- `Economy`
- `Scenario`
- `BuildController`
- `UiController`
- `Assets`
- `Renderer`
- `SaveCodec`

Do not make `Game` another large public state bag.

## Work order

Work is strictly sequential. Only one item may be `in_progress`.

### Stage 0: behavior lock

1. `BASE-01` - add work-control documents and record the current proof.
2. `BASE-02` - make a testable C library without changing game bodies.
3. `BASE-03` - vendor doctest 2.5.3 and move smoke checks out of `main`.
4. `BASE-04` - give tests fixed random, time, resource, and save inputs.
5. `BASE-05` - add rule checks and golden digests for resources, save data,
   all scenarios, core input, and key frames.

Exit: Debug and Release are green; two full runs give the same results.

### Stage 1: mechanical C++20 move

1. `CXX-01` - rename first-party `.c` files to `.cpp` and use C++20.
2. `CXX-02` - make first-party code clean under `/W4 /permissive- /WX`.
3. `CXX-03` - add build/test presets and the final style rules.

Exit: every Stage 0 check and digest is unchanged.

### Stage 2: ownership and support code

1. `RT-01` - value types, exact `JavaRandom`, byte buffer, and input queue.
2. `RT-02` - decrypt old data once, use clear resource names, and add an RAII
   reader with path-based roots. No run-time XOR code remains.
3. `RT-03` - owning `Image` and non-owning `Surface`.
4. `RT-04` - RAII record store with a selected save root.
5. `RT-05` - `WinMmMusicPlayer` and a silent test music output.
6. `OWN-01` - local arrays and text to standard containers.
7. `OWN-02` - fixed state arrays and tables with named compile-time counts.
8. `OWN-03` - maps and other matrices to `Grid<T>` or nested containers.
9. `OWN-04` - remove raw ownership and the array-length registry.

Exit: no first-party raw owning pointer or allocation registry remains; the
full AddressSanitizer run is green.

### Stage 3: game model

1. `MODEL-01` - exact-value enums and named constants.
2. `SAVE-01` - one explicit codec for the old save byte order.
3. `WORLD-01` - world data, map queries, and geometry.
4. `WORLD-02` - terrain work and map generation.
5. `PATH-01` - path search and its scratch state.
6. `FLEET-01` - train, service, and city records with stable slot IDs.
7. `FLEET-02` - movement, cargo, merge, load, and unload rules.
8. `ECON-01` - money, loans, calendar, history, and reports.
9. `SCENARIO-01` - scenario data, AI, offers, scores, and milestones.
10. `SIM-01` - one clear coordinator with the exact old tick order.
11. `BUILD-01` - terrain, track, removal, and train-buy actions.

Exit: core rules no longer use `GameCanvas`; save bytes and all core digests
are unchanged.

### Stage 4: UI, drawing, and application

1. `ASSET-01` - images, font, and packed sprite data.
2. `RENDER-01` - read-only world and train drawing.
3. `RENDER-02` - named screen and widget drawing functions.
4. `UI-01` - menus, dialogs, and text values.
5. `UI-02` - input, cursor, pause, and screen changes.
6. `APP-01` - small `Win32Application`, `Game::update`, and `Game::render`.

Exit: drawing cannot change simulation state; all frame digests are
unchanged; the manual play check is green.

### Stage 5: remove the old shape

1. `CLEAN-01` - remove `GameCanvas`, `game_methods.h`, compatibility calls,
   dead fields, and Java-signature comments.
2. `CLEAN-02` - final name, include, raw ownership, and global-state audit.
3. `DOC-01` - final design, save, resource, simulation, and build guides.
4. `RELEASE-01` - Debug, Release, AddressSanitizer, MIDI, save, all scenarios,
   long simulation, and manual proof.

## Standard extraction loop

Use this loop for every state group:

1. Add focused checks for present behavior.
2. Recover every weak name touched by the item.
3. Add the new type behind a small old-call adapter.
4. Move data first and operations second.
5. Change one caller group at a time.
6. Keep each normal commit buildable.
7. Remove the adapter only when no caller remains.
8. Run the full tests and update the work records.

## Test gates

- Every normal commit: Debug build and fast tests.
- Every item exit: Debug build and full tests.
- Every stage exit: Debug, Release, full tests, and AddressSanitizer.
- UI and final exits: manual play check.
- Real MIDI device play is a separate device check.

Golden updates are not part of this refactor. On a real game fix, make a
separate approved item with a failing rule check and native/decompile evidence.

## Handoff rule

Before code work, update `CURRENT.md` with the base commit and exact goal.
After every useful substep, record the last test and next action. At handoff,
leave either a green commit or a clear `WIP(<item>)` commit with the failing
command and error. Never start a second item while one is active.
