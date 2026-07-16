# Current work item

## Item

`FLEET-01` - train, service, and city records with stable slot IDs.

## Base

- Commit: PATH-01 closed green at `a9a99be` (search scratch in `Pathfinder`;
  endpoints named `pathAnchor/Target`; cost/owner codes deferred to BUILD-01).
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Give the fleet its own data. Extract the train, service-site, and city-record
state that is loose on `GameCanvas` into a clear owned type (e.g. `Fleet`),
keeping the exact stable slot IDs, save byte order, and tick/draw order. Reuse
the proven extraction pattern: introduce the owned type, make it a `GameCanvas`
member, do a mechanical `gc->field` -> `gc->fleet.field` rename, fix any `IO_*`
save-macro sites, then run both full gates.

This also unblocks the entangled map queries deferred from WORLD-01
(`gc_isCityOrStationTile`, station/track scans) once the city/train state they
read lives in a known owner.

## Non-goals

- Do not change slot IDs, save bytes, tick order, or draw order. The scenario,
  save, and frame goldens are the guard.
- Do not move the cost/owner fields deferred to BUILD-01 in this item.
- Movement, cargo, merge, load, and unload rules are FLEET-02.

## Last proof

PATH-01 closed: Debug and Release full gates passed on 2026-07-16, no golden
change (fast 0.02 s / full 0.37 s Debug; fast 0.01 s / full 0.05 s Release).

## Progress

- Done (D-015, leaf 1): `struct Fleet` owns the 16 per-train-slot record arrays.
- Done (D-015, leaf 2): `Fleet` also owns the 250-slot service-vehicle records
  (`vehicleFlags`, `serviceVehicleX/Y`, `serviceResourceStock`,
  `serviceUnloadFlags`, `serviceLoadFlags`). Both leaves: Debug + Release full
  gates green on 2026-07-16, no golden change.

## Next action

FLEET-01 final leaf: the city/station records (48-slot: `stationDirectionCode`,
`cityPopulation`, `cityIndex`, `stationRevenue`, `prevStationRevenue`). Decide
the owner: records (geography) fit `Fleet` or a city type, but
`stationRevenue`/`prevStationRevenue` overlap `Economy` (ECON-01). Plan: move the
three record arrays into `Fleet` and leave the two revenue arrays on
`GameCanvas` for ECON-01, then close FLEET-01 and open FLEET-02. Train-field name
recovery stays a separate small leaf.
