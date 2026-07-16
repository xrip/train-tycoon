# Current work item

## Item

`PATH-01` - path search and its scratch state.

## Base

- Commit: WORLD-02 closed green (generation stages, RNG helper, tile-range
  taxonomy named; slope shapes deferred).
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Make the track path search clear and give its scratch state a home. The search
lives in `win32/src/game/pathfinding.cpp` (breadth-first distance fill over
`pathDistanceGrid`, the walk in `gc_validatePathStep`, and the track-owner
bitfield writes). Name the search fields and steps by behavior and keep the
exact search result and RNG/tie-break order.

## Non-goals

- Do not change the search result, tie-break order, or the built track output.
  The scenario/build goldens are the guard.
- Do not remove `GameCanvas`; the scratch grids can become a `Pathfinder` member
  it owns, migrated behind the existing field access.

## Last proof

WORLD-02 closed: Debug and Release full gates passed on 2026-07-16, no golden
change.

## Progress

- Done (D-014): extracted `struct Pathfinder` (`pathfinder.h`) owning the search
  scratch (`distanceGrid`, `searchLimit`, `nextX`, `nextY`, `tieBreakCounter`),
  renamed off `GameCanvas`. The path functions were already well named. Debug and
  Release full gates passed on 2026-07-16, no golden change.

## Next action

Name the shared path endpoints `placementX/Y` (goal) and `demolishX/Y` (source of
the back-walk; the "demolish" name is misleading here) by behavior, being careful
they are shared with build/demolish. Consider whether the cost accumulator and
owner-code fields belong with `Pathfinder` or a later build class. Keep the search
order identical and lean on the goldens after each leaf; then close PATH-01 and
open FLEET-01.
