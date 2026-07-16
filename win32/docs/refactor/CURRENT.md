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
- Done (D-011): named the codec direction (`save::MODE_WRITE/READ/SKIP`), the
  `gc_initSaveOrLoad` selector (`save::OP_SAVE_MAIN/OP_LOAD_MAIN/OP_RESET_SKIP`),
  renamed `gc_loadGame(n, n2)` -> `(slot, mode)` and
  `gc_readShortFromBuffer` -> `gc_readSaveByteUnsigned`. Debug and Release full
  gates passed on 2026-07-16, no golden change. Noted: `gc_initSaveOrLoad` modes
  4/5 are dead branches (smell, separate cleanup).

## Next action

The codec now reads explicitly. Remaining SAVE-01 judgement: whether to lift the
field walk into a `SaveCodec` boundary now or after the model classes exist.
Recommend deferring the class extraction until WORLD/FLEET/ECON own their fields
(so the codec reads their state, not `GameCanvas`), and closing SAVE-01 on the
explicit-codec basis. Next concrete step: review whether any remaining save
primitive names or the `saveReadOffset` dual read/write cursor need clearer
names, then close SAVE-01 and open WORLD-01.
