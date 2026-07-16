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

## Progress

- Done (D-012, leaf 1): `World` owns the four map grids.
- Done (D-012, leaf 2): `World` owns `mapWidth`, `mapHeight`, `mapHalfWidth`,
  `mapScreenWidth`, `mapScreenHeight`; save macros repointed to `world.*`. Debug
  and Release full gates passed on 2026-07-16, no golden change.

## Next action

Operations next: move the pure map-query helpers (`gc_isCityOrStationTile`, the
connected-track scans, in-bounds checks, and coordinate/geometry math) to read a
`World&` rather than reaching through `GameCanvas`. Keep values and byte order
identical; commit each leaf after the strict Debug and full tests pass, then close
WORLD-01 and open WORLD-02 (terrain and generation).
