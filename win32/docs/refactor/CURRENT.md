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

Start with the most-used unnamed constant group: the `gc_setState`/`gameState`
values. Read their behavior at each call site, introduce a named constant set
(an `enum` or `constexpr` group in a small header), and replace the literals in
small green leaves. Keep the numeric values identical. Run the strict Debug build
and full tests, then commit each leaf. Move to tile/track codes and menu
transition codes next.
