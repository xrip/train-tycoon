# Current work item

## Item

`OWN-02` - move fixed state arrays and tables to `std::array`.

## Base

- Commit: `a8be35f` (`OWN-01` code is green).
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

## Next action

List fixed raw arrays in `GameCanvas` and static game tables by size and use.
Separate arrays which are written by the save codec, then select the smallest
independent caller group for the first `std::array` move.
