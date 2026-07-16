# Current work item

## Item

`KISS-01` - remove needless C++ library form, dead state, weak names, and large
mixed-purpose coordinators without changing game behavior.

## Base and checkpoints

- Base: `ce9e091` (`Add pointer edge scrolling`).
- Behavior lock: `780f9f4`.
- Public API and dead-state cleanup: `e3dda1e`.
- Plain fixed game storage and concrete save walkers: `68507b8`.
- Core name recovery: `34ba7a9`.
- Ordered simulation, map generation, paint, map redraw, and packed-dialog
  work: `f022c9f`, `d056f1d`, `ea5a3a3`, `c6c932c`, `c739969`.
- Branch: `master`.
- User-owned `.idea/` data is not part of this work.

## Result

- `game.h` is the small application boundary. Shared core declarations are in
  `game_internal.h`.
- Dead host, timing, input, and helper state is gone.
- Fixed game records use plain named arrays. `std::vector`, `std::unique_ptr`,
  `Grid`, `std::span`, and `std::optional` stay only where size, ownership, a
  borrowed text range, or a missing dialog value needs them.
- Save work uses concrete byte, short, int, and grid walkers. Save field order
  and bytes are fixed by tests.
- `GameState` is a plain exact-value enum; no unary conversion helper remains.
- The simulation tick and incremental map generator are ordered coordinators.
  Static map redraw and overview paint are separate jobs. Packed dialog bytes
  have names for their header, content items, selection bands, images, and draw
  commands.
- `savedLegacyMetricBq` stays because it is part of the old save form.
- `game_syncHighScores` stays because live state-machine code calls it, even
  though the native implementation is now a no-op.

## Locked proof

The full Debug test set locks:

- reset defaults and resource setup;
- all scenario map digests;
- exact save size, bytes, and round trip;
- year-boundary saved-state digest;
- packed report and graph pixel digests;
- input and main frame digests.

No golden value may move during `KISS-01`.

## Remaining proof

- Run Debug and Release full presets.
- Run the throwaway AddressSanitizer build.
- Start the Release game for a short launch check. Full visual play and audible
  MIDI remain human checks.

## Separate issue

`game_computeMenuRowDisplayValue` can divide by zero for a bad row value. This
is a game bug, not a behavior-safe cleanup, so it needs its own test and change.
