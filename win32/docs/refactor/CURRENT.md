# Current work item

## Item

`SCENARIO-01` - scenario data, AI, offers, scores, and milestones.

## Base

- Commit: ECON-01 closed green at `38ca6f6` (Economy owns money, loans, calendar,
  revenue/spend aggregates, station revenue, and decade/period report totals).
- Branch: `master`
- User-owned file: root `AGENTS.md` (tracked by the user; do not change it).

## Goal

Give the scenario its own data. Extract the scenario/campaign selection, AI
opponent state, offer generation, score/milestone tracking fields that are loose
on `GameCanvas` into a clear owned type (`Scenario`), keeping the exact save byte
order and tick order.

Reuse the proven extraction pattern: introduce `struct Scenario`, make it a
`GameCanvas` member, mechanical `gc->field` -> `gc->scenario.field` rename, fix
the `IO_*` save sites, run both full gates, commit each leaf immediately.

## Non-goals

- Do not change save bytes, tick order, or any golden.
- Do not move scenario rule functions off `GameCanvas*` (that is the stage-3 exit).
- Offer/score *rendering* is RENDER/UI territory.
- Cost-model constants and build/train-buy transient costs stay for BUILD-01.

## Last proof

ECON-01 closed: Debug and Release full gates passed on 2026-07-16, no golden
change. Committed `38ca6f6`.

## Progress

- None yet for SCENARIO-01. First step is the field survey below.

## Next action

Survey the scenario fields on `GameCanvas`: scenario/campaign selection and
difficulty, AI opponent state (`aiFrequency`, `aiThreshold`, AI money/spend
already in Economy), offer generation (`offerTypesByEffect`,
`offerEffectSchedule`, `offerGenerationCount`, `routeResourceFlags`), and
score/milestone tracking. Group them, confirm their `IO_*` save sites, then
extract the cleanest leaf behind the mechanical rename.
