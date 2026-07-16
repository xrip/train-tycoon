# Current work item

## Item

`CLEAN-01` - rename `GameCanvas` -> `Game`, remove the old shape. (Also lands the
stage-3-exit "core rules no longer use `GameCanvas`" change, since rules then
take the `Game` aggregate of parts.)

## Base

- Commit: scenario flags named/homed (`697da02`); STATUS updated (`90c2838`).
- Branch: `master`
- User-owned file: root `AGENTS.md` (tracked by the user; do not change it).

## State of the refactor (this is the big milestone)

All ten owned parts exist and every commit is behavior-identical with both full
gates green and ASan clean:

- `World`, `Pathfinder`, `Fleet`, `Economy`, `Scenario`, `BuildController`,
  `Assets`, `Renderer`, `UiController`, `SaveCodec`.

`GameCanvas` is now a thin aggregate: the ten parts plus the top-level
coordinator state (rng, era, gameState, frame/tick timers, dayFrameCounter, loading
flags, cachedHeader*, music, savedGameState, dayTickThreshold, graphColors).
All obfuscated field names are recovered except `savedLegacyMetricBq`
(save-only, carried metric). Ten dead write-only fields were removed.

## Why CLEAN-01 is not done yet

The full CLEAN-01 is a large mechanical restructure across 36+ files:
1. Type rename `GameCanvas` -> `Game` (the identifier `Game` is verified free -
   no collisions with `GameState`/`game_*`/the `game` test variable).
2. Rename the ~200 `gc_*` free functions and the `gc` receiver to the `Game`
   aggregate style, and drop the `GameCanvas*` first parameter's old shape.
3. Remove `game_methods.h`, the compatibility calls, and Java-signature comments.

It is best done in a fresh context so each mechanical pass is verified with the
full gates, not stacked on an already long session.

## Verified recipe for the type rename (step 1)

- `grep -rlE "\bGameCanvas\b" src tests --include=*.cpp --include=*.h` -> 36 files.
- Word-boundary replace `GameCanvas` -> `Game` in those files (keeps `gc_*`,
  `gcs_*`, `gc->` untouched since they do not contain "GameCanvas").
- `typedef struct GameCanvas { ... } GameCanvas;` becomes
  `typedef struct Game { ... } Game;`.
- Build + both full gates (`test-debug-full`, `test-release-full`); no golden may
  move.

## Blockers (human-gated - cannot be done autonomously)

- The UI-01 / UI-02 / APP-01 manual play check.
- RELEASE-01's manual proof + real MIDI device check.

## Remaining items after CLEAN-01

- `APP-01` - organise the frame loop (state already on the aggregate).
- `CLEAN-02` - only `savedLegacyMetricBq` left to consider; otherwise done.
- `DOC-01` - final design/save/resource/sim/build guides.
- `RELEASE-01` - full proof (human).
