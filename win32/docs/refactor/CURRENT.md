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

## Next action

Follow the standard extraction loop, data first. First leaf: introduce
`struct World` owning the four `Grid<int8_t>` map grids, make it a `GameCanvas`
member, and mechanically move `gc->tileMap` etc. to `gc->world.tileMap` (a pure
member-path change the goldens verify). Then move map dimensions and the map
query/geometry helpers in later leaves. Run the strict Debug build and full
tests, and commit each leaf.
