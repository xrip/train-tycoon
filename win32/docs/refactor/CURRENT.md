# Current work item

## Item

`SAVE-01` - one explicit codec for the old save byte order.

## Base

- Commit: MODEL-01 closed green (states, track/tile, and transition-code bases
  named; UI screen ids deferred to UI-01).
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Give the legacy `.rms` save format one clear codec. The save/load path currently
lives in `win32/src/game/save.cpp` (the `IO_*` and matrix macros) and
`save_arrays.cpp` (array helpers already split into size-explicit `*Span` cores
plus thin wrappers). Make the byte order explicit and readable while keeping the
exact same bytes.

## Non-goals

- Do not change the save byte order, field order, array order, or any numeric
  value. Golden save digests must not move.
- Do not split the `GameCanvas` struct yet; that is a later model stage.

## Last proof

MODEL-01 closed: Debug and Release full gates passed on 2026-07-16, no golden
change. Constants live in `game_constants.h`.

## Next action

Read `save.cpp` and `save_arrays.cpp` in full and map the exact field/array/
matrix write order. Per the handoff, the `*MatrixInlined` helpers still need the
same size-explicit `*Span` split that the array helpers got in OWN-02/D-008.
Start there: give each saved matrix a size-explicit core, keep the byte order,
run the strict Debug build and full tests, and commit each leaf.
