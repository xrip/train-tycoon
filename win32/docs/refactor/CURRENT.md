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

## Next action

Read `map_gen.cpp` end to end and map the generation stages (obstacle, terrain
raise/lower, feature, city/industry placement) and their RNG use. Name the stage
helpers and the terrain scratch fields (`terrainEditBuffer`, patch buffers) by
behavior. Because map-gen output is RNG-sensitive, make only name/shape changes
that leave the random sequence identical, and lean on the scenario map goldens
after every leaf. Commit each leaf after the strict Debug and full tests pass.
