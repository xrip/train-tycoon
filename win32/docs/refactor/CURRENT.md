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

- Done (D-015, leaf 1): extracted `struct Fleet` (`fleet.h`) owning the sixteen
  per-train-slot record arrays (position/dir/phase/flags, state, the `nextCar`
  linked-list pointer, revenue history). `GameCanvas` holds one `Fleet fleet;`
  member; ~280 `gc->field` sites became `gc->fleet.field`, and the `IO_*` save
  sites became `fleet.<field>` (same byte order). Pure ownership move; Debug and
  Release full gates passed on 2026-07-16, no golden change.

## Next action

Pick the next FLEET-01 leaf. Candidates, smallest first:
1. City/station records (48-slot: `stationDirectionCode`, `cityPopulation`,
   `cityIndex`, `stationRevenue`, `prevStationRevenue`) — but `stationRevenue`
   overlaps Economy; likely split: records into Fleet/a city type, revenue waits
   for ECON-01.
2. Service-site records (250-slot: `vehicleFlags`, `serviceVehicleX/Y`,
   `serviceResourceStock`, `serviceUnloadFlags`, `serviceLoadFlags`).
3. Train-field name recovery (re-read `train_tick.cpp` to settle
   `trainEraOrStage`, `trainAgeOrTimer`, `trainEnergyOrCount`).

Decide the service/city owner when those leaves land; reuse the proven owned-type
-> member -> mechanical rename -> fix `IO_*` -> gates loop for each.
