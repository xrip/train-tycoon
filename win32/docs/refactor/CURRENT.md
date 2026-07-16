# Current work item

## Item

`UI-02` - input, cursor, pause, and screen changes (data extraction).

## Base

- Commit: path endpoints homed into Pathfinder (`f9a8fe0`). All nine owned parts
  now exist; `GameCanvas` is down from ~440 fields to ~85.
- Branch: `master`
- User-owned file: root `AGENTS.md` (tracked by the user; do not change it).

## State of the refactor

The single `GameCanvas` state bag has been split into owned parts, each behind a
mechanical `gc->owner.field` move with both full gates green and no golden
change on any commit:

- `World` - map grids, dimensions, city records, tile-animation queue, zone
  growth (constructionTimers/zonePopulation).
- `Pathfinder` - BFS scratch and the path endpoints.
- `Fleet` - train/service records and movement-rule tables.
- `Economy` - money, loans, calendar, revenue/spend aggregates, station revenue,
  decade/period report totals.
- `Scenario` - scores/leaderboard, milestones, AI, offers, identity/difficulty.
- `BuildController` - track-lay, train-buy, terrain-edit, placement cursors.
- `Assets` - packed sprite tables, images, sprite stream.
- `Renderer` - backbuffer, view/clip/repaint state, visible-train list.
- `UiController` - menu rows and grids, dialogs, transitions, graph display,
  selection, cursor, and key input.

`UI-02` leaf 1 (cursor + key input) is done. ASan full tests are clean.

## What remains on GameCanvas (for the next items)

- Frame loop / timing (APP-01): simSpeed, frame/tick timers, counterA, dH,
  loading*, ready, gameThread, stopRequested.
- Map generation params (still a WORLD group): mapGen*, terrain*, weightAccel*,
  industry*, cityStation*, viewportBase/half, mapScale.
- Camera/viewport: viewportCenterX/Y (UI-02/camera).
- Cost-model constants (D-012): moneyBaseA/B, baseTrainSpeed, accel*, brake1,
  costPower/Struct/Overhead/Maintenance/Crew, gWagonCost*, costGroupC*.
- Save-codec state (SaveCodec, deferred by SAVE-01): saveBuffer, saveReadOffset,
  saveSlot, savePendingMode, snapshot*, bytes*, compareToPrevious.
- Simulation-shared: rng, era, flagsA/B, monthRolloverFlag, gameState,
  cachedHeader*, savedLegacyMetricBq.
- Music: music_* and music_output.
- Still-obfuscated names (CLEAN-02 name audit): at, cB, ar, D, ce, ee, dp, cH,
  dI, bx, fR, eD, aJ, hx, gw, bT.

## Non-goals / blockers

- Do not change save bytes, tick order, or any golden.
- The stage-3 exit "core rules no longer use GameCanvas" (signature refactor) is
  gated on homing rng/era/cost constants first, per D-013.
- Human-gated: the UI-01/UI-02/APP-01 manual play check and RELEASE-01's manual
  proof + real MIDI device check cannot be done autonomously.

## Next action

Finish UI-02 by homing the remaining input/screen-change flags, then start
APP-01 (frame-loop/timing state) or the SaveCodec extraction. Recover the
remaining obfuscated field names as part of CLEAN-02.
