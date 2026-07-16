# Current work item

## Item

`WORLD-02` - terrain work and map generation.

## Base

- Commit: WORLD-01 closed green (`World` owns the map grids, dimensions, and
  `in_bounds`).
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Make map generation and terrain editing clear. The generation code lives in
`win32/src/game/map_gen.cpp` and terrain work in `win32/src/game/terrain.cpp`.
Give the generation stages and terrain scratch state readable names and shape,
and let generation write through `World`. Keep the exact random call order and
generated output; the 13 scenario map goldens must not move.

## Non-goals

- Do not change the RNG call order, generated tile/track output, or save bytes.
  The scenario map digests are the guard.
- Do not remove `GameCanvas`; generation still reads scenario/config fields on it.

## Last proof

WORLD-01 closed: Debug and Release full gates passed on 2026-07-16, no golden
change.

## Progress

- Done: renamed the RNG helper `rint` -> `nextRngInt` (was shadowing libm) and
  removed a dead `RNG` macro.
- Done: named the generator state machine `gc->mapGenStage` 0..10 as
  `mapgen::STAGE_*` (init, raise, lower, patches, six feature passes, scatter
  obstacles) in `map_gen.cpp`. Debug and Release full gates passed on 2026-07-16,
  all 13 scenario-map goldens unchanged.

## Next action

Name the remaining generator magic where evidence is clear: the slope-tile ids in
`gc_toggleFlagInlined` (needs the deferred tile-shape taxonomy first, so record
evidence before renaming), and the terrain scratch fields. Keep the random
sequence identical and lean on the scenario map goldens after every leaf. Then
close WORLD-02 and open PATH-01.
