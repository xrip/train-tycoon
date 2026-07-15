# Current work item

## Item

`OWN-02` - move fixed state arrays and tables to `std::array`.

## Base

- Commit: `25f7edd` (first `OWN-02` leaf group is green).
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Give fixed-size game state direct standard ownership and known sizes. Keep all
indexes, first values, save order, and game behavior unchanged.

## Non-goals

- Do not move maps, variable rows, or other matrices yet.
- Do not change save byte order or numeric game values.
- Do not add a general container layer.
- Do not split game model parts yet.

## Last proof

`OWN-01`: all short-lived owned game buffers and returned text-code values now
use standard containers. Text drawing borrows `std::span`; a missing dialog
value is explicit with `std::optional`. Debug and Release fast/full tests
passed with all fixed values unchanged on 2026-07-15.

The first `OWN-02` group moved `selectionTileX` and `selectionTileY` from raw
owned pointers to `std::array<int32_t, 4>`. The strict Debug build and fast
tests passed on 2026-07-15.

The second `OWN-02` group moved `moneyHistory` to `std::array<int32_t, 3>` and
`moneyTickerFrames` to `std::array<int8_t, 3>`, and removed their two
`midp_alloc_arr` calls. The strict Debug build and full tests passed on
2026-07-15 with all fixed values unchanged.

## Next action

Continue with small non-save fixed groups by size and use. Good next candidates
are player name codes, wagon option arrays, route flags, enabled menu rows, and
the 12-value graph arrays. Check all call sites before each group, then run the
strict Debug build and fast tests and commit each green leaf.
