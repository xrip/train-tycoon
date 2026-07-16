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

## Progress

- Verified the codec: the `*MatrixInlined` helpers already forward to the
  size-explicit `*Span` cores (done in OWN-04), so no matrix split is left.
- Done (D-011): named the codec direction with `save::MODE_WRITE/READ/SKIP` and
  renamed `gc_loadGame(n, n2)` -> `(slot, mode)`. Debug and Release full gates
  passed on 2026-07-16, no golden change.

## Next action

Continue making the codec explicit: name the `gc_initSaveOrLoad(mode)` selector
values (0/1/2/4/5) and rename the misleading `gc_readShortFromBuffer` (it reads
one unsigned byte). Consider grouping the codec into a `SaveCodec` boundary once
the field walk reads clearly. Keep byte order identical; commit each leaf after
the strict Debug and full tests pass.
