# Current work item

## Item

`ECON-01` - money, loans, calendar, history, and reports.

## Base

- Commit: FLEET-02 closed green at `<pending>` (movement-rule tables in `Fleet`;
  rule functions stay free until the stage-3 exit).
- Branch: `master`
- User-owned file: root `AGENTS.md` (now tracked by the user; do not change it).

## Goal

Give the economy its own data. Extract the money, loan, calendar (date), revenue
history, and report fields that are loose on `GameCanvas` into a clear owned type
(`Economy`), keeping the exact save byte order and tick order. This also picks up
the per-station revenue history (`stationRevenue`, `prevStationRevenue`) deferred
from FLEET-01, since revenue is economy state.

Reuse the proven extraction pattern: introduce `struct Economy`, make it a
`GameCanvas` member, mechanical `gc->field` -> `gc->economy.field` rename, fix
the `IO_*` save sites, run both full gates.

## Non-goals

- Do not change save bytes, tick order, or any golden.
- Do not move economy rule functions off `GameCanvas*` (that is the stage-3 exit).
- Calendar/report *rendering* is RENDER/UI territory.

## Last proof

FLEET-02 closed: Debug and Release full gates passed on 2026-07-16, no golden
change.

## Progress

- None yet for ECON-01. First step is the field survey below.

## Next action

Survey the economy fields on `GameCanvas`: money/loan/total-spent
(`money`, `loanBalance`, `totalSpent`, `cash`...), the calendar
(`dayOfMonth`, `monthOfYear`, `year`, day counters), revenue history
(`stationRevenue`, `prevStationRevenue`, history arrays, graph modes), and report
fields. Group them, confirm their `IO_*` save sites, then extract the cleanest
leaf (likely money+loan+calendar first, history second) behind the mechanical
rename.
