# Current work item

## Item

`BASE-04` - give tests fixed random, time, resource, and save inputs.

## Base

- Commit: the `BASE-03` doctest commit on top of `e8e969e`.
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Make every automatic test independent of the wall clock, launch directory,
and user save files.

## Non-goals

- No game-rule change or broad rename.
- Production seeding still uses the current clock.
- The save byte form stays unchanged.

## Last proof

`BASE-03`: four doctest cases passed on 2026-07-15 in 1.94 seconds. The
executable no longer holds automatic smoke functions.

## Next action

Add a small test fixture that sets resource/save roots, fixed time fields, and
fixed `JavaRandom` seed values before each full-game check. Run the suite twice
and compare its output.
