# Current work item

## Item

`OWN-03` - move maps and other matrices to `Grid<T>` or nested containers.

## Base

- Commit: OWN-02 closed green (fixed state arrays and tables all owned).
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Give map grids and other two-dimensional game state clear standard ownership
with known sizes. Keep `grid[x][y]` order (`x * height + y` for `Grid<T>`), save
byte order, random and tick order, and all game behavior unchanged.

## Non-goals

- Do not remove the array-length registry yet; that is OWN-04.
- Do not change save byte order or numeric game values.
- Do not split game model parts yet.

## Last proof

`OWN-02` is complete. Every fixed-size raw state array and constant table now
uses `std::array` (including the per-train 500-slot arrays, service and city
arrays, financial history, menu row tables, offer tables, and the track-step and
sprite-direction tables). The save helpers gained size-explicit `*Span` cores
with templated `std::array` overloads, so save byte order is unchanged. Dead
`dk` and `j` tables were removed. Debug and Release fast/full tests passed on
2026-07-15.

## Next action

Pick the first OWN-03 leaf. Good early candidates are the fixed-shape matrices
still on raw `int8_t**`/`int16_t**`: the map grids (`tileMap`, `trackMap`,
`tileElevation`, `heightMap`, `terrainEditBuffer`, `pathDistanceGrid`) and the
service resource stock matrix (`serviceResourceStock`, 250x5). Map grids are
sized `mapWidth * mapHeight`, so they need `Grid<T>` with `x * height + y`
order. The save codec reaches map matrices through
`gc_saveOrLoad*MatrixInlined`, which will need a size-explicit path like the
array helpers before a saved matrix moves. Check all call sites before each
group, then run the strict Debug build and full tests and commit each green leaf.
