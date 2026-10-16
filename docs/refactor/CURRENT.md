# Current work item

## Item

`DOMAIN-01` - C-with-Classes domain refactor.

## Result

- `Game` is a class in `game.h` with the small app API; state is private.
- The old free `game_*` API, `game_state.h`, and `game_internal.h` are gone.
- `SaveCodec`, `Terrain`, `World`, `Fleet`, `Economy`, `OfferBoard`,
  `Scenario`, `Pathfinder`, `BuildController`, `Assets`, `Renderer`, `Camera`,
  `UiController`, and `MusicController` own their state and proved rules.
- Train cars, service sites, cities, and offers are joined fixed records with
  stable slots and no dual storage.
- The app stack-owns `Game`. Tests use `SilentMusicOutput`, a fixed seed, and
  the single friend `GameTestAccess`.
- Save bytes, resource bytes, random order, tick order, and draw order did not
  change. No golden value moved in structural commits.
- The type-2 offer zero-money case has a direct test and returns `0%`.

## Proof now

- Debug fast and full: pass.
- Release full: pass.
- Static source audit: no free `game_*` API and no function taking `Game*`.
- Fresh AddressSanitizer full: pass (`build-asan-domain`).
- Bounded Release MIDI device path: pass (2 seconds, exit 0).
- Full visual play remains a human check.

User-owned `.idea/` and `tools/` data are outside this work.
