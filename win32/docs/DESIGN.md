# Train Tycoon native design

This C++20 source keeps the old save bytes, resource reads, stable slot ids,
random reads, tick order, and draw order. Fixed-seed tests lock these rules.

## Owner tree

`Game`, defined in `src/game.h`, is the public app boundary and the coordinator
for work that changes three or more owners.

| Owner | Main state and rules |
|---|---|
| `World` | `Terrain`, track grid, `City[48]`, growth, and map generation state |
| `Terrain` | tile, elevation, and height grids; size, edits, rebuild, and queued tile work |
| `Fleet` | `TrainCar[500]`, `ServiceSite[250]`, chains, movement, cargo, load, and unload |
| `Economy` | money, calendar, reports, costs, profit, and history |
| `OfferBoard` | `Offer[6]`, issue dates, progress, expiry, generation, and effects |
| `Scenario` | identity, difficulty, era, AI policy, milestones, and scores |
| `Pathfinder` | search state and ordered path search |
| `BuildController` | build, remove, terrain-edit, and train-buy action state |
| `Assets` | packed data, images, text data, and fixed asset tables |
| `Renderer` | draw caches, canvas state, graph colors, and `Camera` |
| `UiController` | input, menus, reports, selection, and screen flow |
| `SaveCodec` | concrete save walkers, compression, and snapshot state |
| `MusicController` | output ownership, volume, load, enabled, and playing state |

Owners do not keep a pointer or reference to another owner. A rule receives a
needed partner by reference. Small text, number, and geometry operations stay
free and file-local.

## Public API

The app can start, advance, render, send key or pointer input, and control music
through `Game`. All owner state and internal coordinator methods are private.
Tests use the one friend `GameTestAccess`, defined under `tests/`; product code
has no test setters. `SaveCodec` has direct access for the fixed save walk.

## Fixed records and save form

Train cars, service sites, cities, and offers are plain fixed records with
stable integer slots. There is no heap object per record. Defaults keep the old
zero-filled values and sentinels.

`SaveCodec` writes fields in the old field-major order. It never writes raw
record memory or `sizeof(record)`. City revenue is stored in `City`, but its
bytes remain at the old early save position.

## Order rules

- Map generation keeps each random read and stage call in its old order.
- A simulation frame runs animation, queued tile work, trains, then calendar.
- Multi-byte reads use ordered statements.
- Render-time state changes remain in `Game`; model inputs to draw helpers are
  const where drawing does not own the old timing change.

## Build and test

```powershell
cmake --build --preset build-debug
ctest --preset test-debug-full
cmake --build --preset build-release
ctest --preset test-release-full
```

Run a fresh AddressSanitizer tree at each main stage. The final human check is a
normal play session with build, train, offer, year, save/load, overview, input,
and music work.
