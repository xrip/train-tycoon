# Current work item

## Item

`BUILD-01` - terrain, track, removal, and train-buy actions.

## Base

- Commit: SIM-01 closed green at `276bfbe` (Playing tick extracted into the
  gc_simulationTick coordinator).
- Branch: `master`
- User-owned file: root `AGENTS.md` (tracked by the user; do not change it).

## Goal

Give the build actions their own data. Extract the terrain-edit, track-lay,
removal, and train-buy action state that is loose on `GameCanvas` into a clear
owned type (`BuildController`), keeping the exact save byte order and tick order.
This picks up the deferred `selectionCost`, `previewTrackOwnerCode`, and
`existingTrackOwnerCode` from PATH-01.

Reuse the proven extraction pattern: introduce `struct BuildController`, make it
a `GameCanvas` member, mechanical `gc->field` -> `gc->build.field` rename, fix
the `IO_*` save sites, run both full gates, commit each leaf immediately.

## Non-goals

- Do not change save bytes, tick order, or any golden.
- Do not move build rule functions off `GameCanvas*` (that is the stage-3 exit).
- The build/menu *display* rows (`menuRow*`, `buildMenu*` grids) are UI-01.

## Last proof

SIM-01 closed: Debug and Release full gates passed on 2026-07-16, no golden
change. Committed `276bfbe`.

## Progress

- None yet for BUILD-01. First step is the field survey below.

## Next action

Survey the build-action fields on `GameCanvas`: the track-lay transients
(`trackStart*`, `trackStep*`, `trackBuildInProgress`, `trackTileCount`,
`previewTrackOwnerCode`, `existingTrackOwnerCode`, `nextTrackPieceCode`), the
terrain-edit state (`terrainEditBuffer`, `selection*`, `swapMap*`,
`terrainSmoothCost`), removal (`demolitionAffordable`, `buildCancelled`), and
train-buy (`wagon*`, `unitCost`, `*CostSnapshot`, `discountApplied`,
`selectionCost`). Confirm their `IO_*` save sites and separate them from the
UI-owned menu display rows, then extract the cleanest leaf behind the mechanical
rename.
