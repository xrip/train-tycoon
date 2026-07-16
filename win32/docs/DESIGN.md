# Train Tycoon (win32) — design guide

This is the native C++20 source of truth. It keeps the original game's behavior,
resource read order, random call order, draw order, tick order, and the legacy
`.rms` save byte order. Every refactor commit is behavior-identical: the
fixed-seed golden-digest tests must not move.

## The `Game` aggregate and its parts

`Game` (in `src/game_state.h`, formerly `GameCanvas`) is a thin aggregate. It
joins the owned parts and holds only the top-level coordinator state (the RNG,
the tech era, the game-state selector, the frame/tick timers, loading flags,
cached header values, and the music player). It is deliberately **not** a large
public state bag — the game data lives in its parts:

| Part | Header | Owns |
|---|---|---|
| `World` | `world.h` | map grids (tile/track/elevation/height), dimensions, city records, tile-animation queue, per-zone growth, map-generation params |
| `Pathfinder` | `pathfinder.h` | BFS distance scratch, back-walk cursor, tie-break counter, path endpoints |
| `Fleet` | `fleet.h` | train and service-vehicle records (stable slot ids), movement/cargo rule tables |
| `Economy` | `economy.h` | money, loans, calendar, tax/interest, revenue/spend aggregates, station revenue, decade/period report totals, the era-scaled cost/performance model |
| `Scenario` | `scenario.h` | campaign scores/leaderboard, milestones, AI opponent state, offer generation, identity/difficulty, high-score entry, feature flags |
| `BuildController` | `build_controller.h` | track-laying, train-buy dialog, terrain-edit/selection, placement cursors |
| `Assets` | `assets.h` | packed sprite data tables, loaded font/tile/cycle images, the sprite stream reader |
| `Renderer` | `renderer.h` | backbuffer + borrowed surface, current-sprite tracking, culled visible-train list, drawn tile bounds, the camera/view transform, clip/repaint frame state |
| `UiController` | `ui_controller.h` | menu rows and grids, dialog/confirm/transition state, report-graph display, on-map selection, cursor and key input |
| `SaveCodec` | `save_codec.h` | serialization buffer and read cursor, snapshot bookkeeping |

A part is a plain `struct` of data (C with Classes): fields plus, where useful, a
small helper such as `World::in_bounds`. Game rules are free functions in
`src/game/*.cpp` that read and write the parts through the `Game` aggregate.

## Invariants the tests lock

- **Save bytes**: `game/save.cpp` writes fields in the exact legacy order via the
  `IO_*` macros. Moving a field into a part changes the macro argument
  (`field` -> `owner.field`) but never the order.
- **Tick order**: the per-frame simulation runs in `gc_simulationTick`
  (`game/sim_tick.cpp`): frame timers, tile-fade queue, train updates, then the
  day/month/year calendar cadence with its monthly and yearly hooks.
- **Random order**: the 48-bit LCG (`JavaRandom`) is called in the same order as
  the original; map generation depends on it.
- **Signed bytes / stable slot ids / draw order**: unchanged.

## Building and testing

```
cd win32
cmake --preset windows-x64            # configure (VS 2022, x64)
cmake --build --preset build-debug
ctest --preset test-debug-full --output-on-failure
cmake --build --preset build-release
ctest --preset test-release-full --output-on-failure
```

AddressSanitizer is run at stage exits with `/fsanitize=address` (add it to
`CMAKE_CXX_FLAGS` alongside `/EHsc` in a throwaway build dir).

## What is intentionally still on `Game`

The top-level coordinator state (rng, era, gameState, frame/tick timers,
counterA, loading flags, cachedHeader*, music, and the one save-only carried
`savedLegacyMetricBq`). These are the aggregate's own frame/coordinator concerns,
not a sub-part. See `docs/refactor/DECISIONS.md` and `STATUS.md` for the item
history and the remaining cosmetic `gc_*` cleanup (CLEAN-01 leaf 2).
