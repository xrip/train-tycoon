# Current work item

## Item

`WORLD-01` - world data, map queries, and geometry.

## Base

- Commit: SAVE-01 closed green (explicit save codec; SaveCodec class deferred to
  the model stages).
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Give the map its own owned data. Introduce a `World` type that owns the four map
grids (`tileMap`, `trackMap`, `tileElevation`, `heightMap`) and the core map
dimensions/geometry, then move map queries and coordinate math onto it. Keep
every read identical and the save byte order unchanged.

## Non-goals

- Do not change tile/track values, map generation output, or save bytes. Golden
  map and save digests must not move.
- Do not remove `GameCanvas` yet; `World` becomes a member it owns, migrated one
  caller group at a time behind the existing field access.

## Last proof

SAVE-01 closed: Debug and Release full gates passed on 2026-07-16, no golden
change.

## Progress

- Done (D-012): `struct World` in `world.h` owns the four map grids; `GameCanvas`
  holds one `World world;` and all `gc->tileMap` style access moved to
  `gc->world.tileMap`. Debug and Release full gates passed on 2026-07-16, no
  golden change.

## Next action

Continue the extraction, data first. Next leaves: move the core map dimensions
(`mapWidth`, `mapHeight`, `mapHalfWidth`, `mapScreenWidth/Height`, and the map
half-extent/scale) onto `World`, then move the pure map-query helpers
(`gc_isCityOrStationTile`, connected-track scans, in-bounds checks) to take a
`World&`. Keep values and byte order identical; commit each leaf after the strict
Debug and full tests pass.
