# Current work item

## Item

`MODEL-01` - name game enums and constants.

## Base

- Commit: OWN-04 closed green (no raw owning array, no length registry).
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Give the game's magic numbers and state codes clear names: game states, tile and
track codes, menu transition codes, resource kinds, and similar fixed constants.
Keep every numeric value and all game behavior unchanged.

## Non-goals

- Do not split the `GameCanvas` struct or move fields between files yet.
- Do not change save byte order or numeric game values.
- Do not rename by guesswork; use behavior and cross-references, and record
  uncertain evidence in `DECISIONS.md` before a rename.

## Last proof

The ownership arc (OWN-01 through OWN-04) is complete. Every game array is
`std::array`, `std::vector`, or `Grid<T>`, and the raw array-length registry is
removed. Debug and Release fast/full tests passed on 2026-07-15.

## Next action

Done: the `gc_setState`/`gameState` value group is named. `game_constants.h`
holds `enum class GameState : int32_t` (values unchanged); all read/write/case
sites use `+GameState::Name`. The previous-state copy `hx` comparisons were named
too. `confirmTargetState` and `keySubState` were left as-is (not states; see
DECISIONS D-006/MODEL-01). Debug and Release full gates passed on 2026-07-16.

Next: name the tile and track codes, then the menu transition codes
(`transitionCode`/`pendingTransitionCode`) and resource kinds. Use behavior and
cross-references; record uncertain evidence in `DECISIONS.md` before a rename.
Keep numbers identical and commit each leaf after the strict Debug and full
tests pass.
