# Refactor decisions

## D-001: Language and style

- End state: C++20.
- Style: C with Classes.
- Classes are for ownership, state, and simple rules.
- Plain data uses structs; simple algorithms use free functions.
- Standard-library templates are welcome when they make code clearer.
- Clear namespace aliases may be used in `.cpp` files. Public headers keep
  full `std::` names, and `using namespace std` is not allowed.
- One small `Grid<T>` is allowed.
- No template design systems, CRTP, concepts, policy classes, deep class trees,
  event systems, or generic frameworks.

## D-002: Compatibility

Keep game behavior and the current `.rms` byte form. The model memory layout
may change, but `SaveCodec` must write the old field and array order.

## D-003: Scope

Refactor all first-party code under `win32/`. Keep MiniFB and `stb_image` as
vendor code. New sound effects, networking, new rules, and portability are out
of scope.

## D-012: Cost-model constants stay on GameCanvas after BUILD-01

BUILD-01 gave `BuildController` the transient *action* state (track-lay,
train-buy dialog, terrain-edit selection). The per-scenario economic/physics
constants (`moneyBaseA/B`, `baseTrainSpeed`, `accel1/2`, `brake1`,
`baseTrainSpeed2`, `costPower/Struct/Overhead/Maintenance/Crew`,
`gWagonCost1..6`, `costGroupC1..4`) are **not** build-action state: they are
loaded together in `GeneratingMap` and inflated together by the yearly random
event, and they feed train-buy cost, train physics, and the economy alike. They
form one cross-cutting parameter block, so forcing them into `BuildController`
would be wrong. They stay on `GameCanvas` for now; their owner (likely
`Scenario` as scenario-loaded parameters, or a small shared model) is decided
later. The build/menu *display* rows (`buildMenu*`, `menuRow*`,
`trainSelection*`) stay for UI-01.

## D-013: Stage-3 exit signature refactor is gated on shared-state homing

The stage-3 exit ("core rules no longer use `GameCanvas`") cannot be a pure
signature refactor yet: the rule functions in `sim_tick.cpp` / `economy_tick.cpp`
read shared state that has no owner (`rng` in ~30 call sites, `era`, the frame
timers, and the D-012 cost constants). A function cannot take `World&`/`Economy&`
instead of `GameCanvas*` while it still reads `gc->rng` or `gc->gWagonCost`.

Plan: keep homing the owned shared state first (tile-animation and zone-growth
state moved onto `World`; the UI/render/asset fields move onto `UiController`,
`Renderer`, and `Assets` in Stage 4). `rng` will be threaded as a `JavaRandom&`
parameter to the rules that need it, and `era` and the cost constants get a home
during that pass. The `GameCanvas*` -> owner-ref conversion of the core rules is
done last, once its inputs are all homed, so each converted group stays a clean
mechanical change with the full gates green. All golden digests stay fixed
throughout.

## D-004: Tests

Use vendored doctest 2.5.3, clear rule checks, and fixed output digests. The
build must not fetch test code from the network.

## D-005: Names

Do not keep a recoverable obfuscated name in changed native code. Read native
behavior and call sites first. Use the decompile only when native evidence is
not enough. Record uncertain evidence here before a rename.

### Open name set

- Methods: `gc_k`, `gc_u`.
- Weak method groups: `dayResetP`, `dayResetAg`, `dayResetAd`,
  `everyTwoTickZ`, and all `*Inlined` names.
- Short state fields: `aJ`, `ar`, `at`, `bT`, `bx`, `ca`, `cB`, `ce`, `cH`,
  `D`, `dc`, `dH`, `dI`, `dp`, `eD`, `ee`, `fR`, `gw`, and `hx`.
  (`dk` and `j` were removed in OWN-02 as dead state.)

### Recovered in RT-01

- `gc_a` -> `gc_appendTileDrawOffset`: its only caller adds one x/y pair to
  the isometric tile draw-order buffers.
- `gc_writeRlePairInlined` -> `gc_writeRleRun`: it writes one literal or
  repeated run to the save RLE output.

### Recovered in RT-02

- `gc_b` -> `gc_loadPackedSpriteData`: it reads the packed sprite offset and
  byte tables.
- `gc_loadSpriteStream` -> `gc_openSpriteStream`: it opens the stream used to
  rebuild sprite PNG data.
- `spriteStream` -> `sprite_stream`: it is the owned sprite input stream.

### Recovered in RT-03

- `ca` -> `font_images`: all six images are font-code atlases used by the text
  renderer.
- `tileSprites` -> `tile_sprites`: it owns the decoded tile image set.
- `cycleFlag` -> `cycle_flag`: it is the loading-cycle image.
- `backbufferG` -> `backbuffer_surface`: it borrows pixels from `backbuffer`.

### Recovered in RT-05

- `autoMusic` -> `music_enabled`: it is the user setting that permits play.
- `gameStateCounter` -> `music_volume_level`: it is saved as a value from 1 to
  5 and maps to output levels 20 to 100.
- `musicAutoStart` -> `music_auto_start`: it requests play during image load.
- `musicAvailable` and `musicPlaying` -> `music_available` and
  `music_playing`.
- `musicPlayer` and `volumeControl` were fake native pointers. One owned
  `MusicOutput` now gives both operations.

### Recovered in OWN-01

- `gc_m` -> `gc_readPackedTextCodes`: it copies one length-prefixed text-code
  value from packed sprite data.
- Local `byArray` work buffers were named for their use: `png_bytes`,
  `seen_city_tiles`, `visible_train_tiles`, `patch_tiles`, and text-code
  values.

### Recovered in MODEL-01

- `GameCanvas::gameState` values now have names in `game_constants.h` as
  `enum class GameState : int32_t`. The field stays `int32_t` for save and
  arithmetic safety; a unary `constexpr operator+(GameState)` yields the value
  so read/write/case sites stay short (`+GameState::Playing`). Numbers are
  unchanged. Names come from each state's behavior:
  - `BootSplash0..3` (0..3): startup splash steps; `BootSplash1` decodes fonts.
    States 2 and 3 are defined by the switches but unreached in the current
    boot path.
  - `IntroReady` (4): fonts ready, arm the main menu.
  - `LoadingResources` (10): load scenario sprites and music.
  - `MenuOverlay` (20): packed-sprite menu/dialog overlay and its dispatch.
  - `GeneratingMap` (25), `Playing` (30), `MapOverview` (40).
  - `BuildingMinimap` (35): builds the minimap image, then `MapOverview`.
  - `BuildingPath` (45): `gc_tryBuildPath`.
  - `SavingGame` (55): `gc_saveAllMapArrays`.
  - `LoadingGame` (65): `gc_restoreAllMapArrays`.
  - `AiTurn` (105): AI places cities/industry or advances.
  - `HighScoreSync` (120): `gc_syncHighScores` (network).
- The previous-state copy field `hx` also holds `GameState` values and its two
  comparisons were named; the field itself keeps its short name pending a
  separate rename in the open name set.
- Not renamed (uncertain, evidence recorded): `confirmTargetState` comes from
  `buildMenuResultCodes` and also holds non-state codes (e.g. 17) used as a
  sprite-text index, so it is not a `GameState`. `keySubState` is a key-repeat
  counter that uses 40/80 arithmetically, not a state. Both were left as-is.

Track and tile codes (second MODEL-01 leaf), in `game_constants.h`:

- `namespace track`: a `trackMap` byte packs `PRESENT` (bit 6, `0x40`) with a
  6-bit `OWNER_MASK` (`0x3F`). Evidence: every track write is `0x40 | owner`;
  `gc_scanTrackTiles` counts `trackMap == 64` (present, owner 0) as one player
  track tile. At the crossing tile the owner field holds two 3-bit sub-owners:
  `DIR_OWNER_MASK` (`7`) for one direction and `>> DIR_OWNER_SHIFT` (`3`) for the
  other (`gc_findConnectedTrack`, `gc_scanTrackTiles`). AI routes cycle piece
  codes `AI_PIECE_FIRST` (66) .. 68, wrapping at `AI_PIECE_WRAP` (69);
  `nextTrackPieceCode - PRESENT` gives the owner code (`gc_placeCitiesAndIndustry`,
  state `AiTurn`).
- `namespace tile`: `CROSSING` (76) is the tile where two tracks cross and the
  `trackMap` byte carries two owners. Evidence: dual-owner reads gated on tile
  76, and the join code that sets tile 76 while OR-ing a second owner into the
  high sub-field.
- Left unnamed (uncertain, recorded for later leaves):
  - Track overlay bits `0x80` and `0xC0` (`map_gen` sets `0x80 | n`, `terrain`
    sets `0xC0 | n`, `sim_tick` tests `& 0xC0`). They mark non-track overlays
    (scenery/obstacle/water); exact meaning not yet proven.
  - Owner code `1` / `trackMap == 65` (station/service track) and the crossing
    station masks `& 0x47`, `& 0x78` in `gc_isCityOrStationTile`.
  - The individual tile shape ids: straight/turn/bridge codes 70..90, city and
    terrain ranges (0, 3..28, 29..33, 34..41, 42..48, 50..68) whose minimap
    colour grouping is in `gc_update` state `BuildingMinimap`. These need a
    per-range evidence pass before naming.

Menu transition codes (third MODEL-01 leaf), in `game_constants.h`
`namespace transition`:

- Named the arithmetic bases only: `OFFER_TYPE_BASE` (40) and `OFFER_ALT_BASE`
  (60) build an offer screen code from `menuRowType`/`menuRowAltType`;
  `SCENARIO_NOTICE_BASE` (80) builds a notice code from the scenario notice id.
  Evidence: the `+40`/`+60`/`80 +` construction sites in `gc_handleBuildMenuState`
  and the `BuildingMinimap`/notice path in `gc_paint`.
- Deferred (decision): the ~40 per-screen menu ids (1, 2, 3, 4, 5, 10, 14, 23..28,
  31..36, and the 41..54 / 61..72 offer screens) are UI navigation. They select
  the `renderPackedSprite` layout and the `gc_handleBuildMenuState` branch, so
  their meaning is only clear once the menu builder and dispatcher become a
  `UiController` in UI-01. Naming them now would be guesswork against the "no
  guess renames" rule, so per-screen naming is scheduled for UI-01. The literals
  stay unchanged until then.
- "Resource kinds" from the original handoff list are already plain-named: the
  numbered resource files became named resources in RT-02 (see D-006). No numeric
  resource-kind group remains in the model to name.

MODEL-01 is closed on this basis: the game-model constant groups (game states,
track/tile codes, and the transition-code bases) are named; UI-screen ids are
deferred to UI-01 by the decision above.

## D-011: Save codec direction is explicit (SAVE-01)

The one `IO_*` field walk in `gc_initSaveOrLoad` serves both save and load; the
direction lives in `GameCanvas::savePendingMode` and in the `mode` argument of
`gc_loadGame`. These were opaque `0`/`1`/`2` literals. `game_constants.h`
`namespace save` now names them: `MODE_WRITE` (0) serializes, `MODE_READ` (1)
deserializes, `MODE_SKIP` (2) touches nothing (returns 0, zeros arrays). Applied
at every `savePendingMode` compare/assign and every `gc_loadGame` call, and the
`gc_loadGame` parameters were renamed `n`/`n2` -> `slot`/`mode`. Values and byte
order are unchanged.

A second leaf named the higher-level `gc_initSaveOrLoad` operation selector as
`save::OP_SAVE_MAIN` (0), `OP_LOAD_MAIN` (1), and `OP_RESET_SKIP` (2), and renamed
the misleading `gc_readShortFromBuffer` -> `gc_readSaveByteUnsigned` (it reads one
unsigned byte, not a short). Selector values 4 and 5 exist in `gc_initSaveOrLoad`
but are never called; this dead branch is recorded as a smell for a separate
cleanup item, not removed under a refactor commit.

## D-012: World owns the map grids (WORLD-01)

`world.h` defines `struct World` owning the four map grids (`tileMap`, `trackMap`,
`tileElevation`, `heightMap` as `Grid<int8_t>`). `GameCanvas` now has one `World
world;` member instead of the four loose grid fields, and every `gc->tileMap`
style access became `gc->world.tileMap` (a pure member-path change; the golden
map and save digests did not move). Map dimensions and geometry, and the map
query helpers, move onto `World` in later WORLD-01 leaves. The terrain-edit
scratch grid `terrainEditBuffer` stays in `GameCanvas` for now; it is edit state,
not map data.

A second leaf moved the core map dimensions (`mapWidth`, `mapHeight`,
`mapHalfWidth`, `mapScreenWidth`, `mapScreenHeight`) onto `World`. The save codec
keeps byte order: `IO_INT(mapWidth)`/`IO_INT(mapHeight)` became
`IO_INT(world.mapWidth)`/`IO_INT(world.mapHeight)` (same `gc->world.field`
expansion, same bytes). The `viewport*` camera fields stay in `GameCanvas`; they
are view state, not map data.

A third leaf seeded `World`'s query API with `World::in_bounds(x, y)` and adopted
it at the full out-of-bounds checks (pathfinding, input, build, terrain, sim).
WORLD-01 closes here: `World` owns the map data (grids + dimensions) and its first
query. The remaining map-query operations (`gc_isCityOrStationTile`, track scans,
station checks) read train/economy state that is not yet extracted, so moving
them onto `World` waits for FLEET/ECON; forcing it now would couple `World` to
unextracted state against the "one caller group at a time" rule.

## D-013: Tile-shape taxonomy needs one holistic pass (WORLD-02 groundwork)

Evidence gathered for the map tile ids, recorded before any rename (per the
"record uncertain evidence first" rule):

- Tiles `0` = empty/flat ground; `3..28` = isometric slope and cliff shapes
  computed from height-corner comparisons in `gc_toggleFlagInlined` (16+ distinct
  configurations); `29..33` = passable scenery/obstacle tiles scattered in
  generation stage `STAGE_SCATTER_OBSTACLES` (`29 + rng%5`); `34..40` = city
  zone tiles (`gc_randomCityTile`); `41..68` = structures, resources, and
  water/coast; `70..90` = track, bridge, and station shapes; `76` = `tile::CROSSING`.
- The catch: the same numeric bound means different things in different
  predicates. `>= 29 && <= 33` tests "is scenery", but `> 33` in
  `gc_placeTerrainPatch` tests "is a structure tile (>= 34)", i.e. 33 is the top
  of the terrain range there, not the top of scenery. A blanket
  `SCENERY_LAST = 33` rename would mislabel the boundary check.
- Done (focused taxonomy leaf): named the range groups that are used as clear
  membership tests: `tile::SCENERY_FIRST/LAST` (29..33) and `tile::BUILT_FIRST`
  (34, first non-terrain tile) with `tile::CITY_ZONE_LAST` (40). Each boundary
  predicate was named by its intent: `>= 29 && <= 33` -> "is scenery";
  `> 33`/`>= 34` -> "is built" (`>= tile::BUILT_FIRST`, the `> 33` form rewritten
  to the equivalent `>= 34`); `< 34 || > 40` -> "not a city-zone tile". Applied
  across terrain, sim, map_gen, query, build, pathfinding, and the minimap in
  paint. Map goldens unchanged.
- Still deferred: the slope/cliff shape ids `3..28` computed in
  `gc_toggleFlagInlined` from height-corner patterns. Those encode isometric
  sprite orientations and need the tile sprite art to name without guessing, so
  they stay literal for now.

## D-014: Pathfinder owns the search scratch (PATH-01)

`pathfinder.h` defines `struct Pathfinder` owning the path-search scratch that
was loose on `GameCanvas`: the BFS `distanceGrid` (was `pathDistanceGrid`),
`searchLimit`, the back-walk cursor `nextX`/`nextY`, and the `tieBreakCounter`.
`GameCanvas` now has one `Pathfinder pathfinder;` member and the fields were
renamed onto it (`gc->pathDistanceGrid` -> `gc->pathfinder.distanceGrid`, etc.).
These five fields are used only by `pathfinding.cpp` (plus `terrain.cpp` reading
the grid), so the move is clean. Search result and tie-break order are
unchanged; goldens did not move.

A second leaf named the two shared path endpoints by behavior. They are written
by map-gen (the city/industry stations) and by the build menu (the player's two
picks), and read only by the search, so they stay on `GameCanvas`:
- `placementX/Y` -> `pathAnchorX/Y`: the reference endpoint (the city station in
  AI gen, the first tile the player picks). It is the reverse-BFS seed
  (`distanceGrid == 20`) and the back-walk terminus (`gc_validatePathStep` stops
  when it reaches the anchor).
- `demolishX/Y` -> `pathTargetX/Y`: the endpoint the track reaches toward (the
  industry station / second pick). The flood-fill must reach it and the
  back-walk starts there. The old `demolish` name was wrong: the real demolish
  action (`gc_demolishAtCursor`) never touches these coordinates.

Deferred to BUILD-01: the path cost accumulator `selectionCost` and the
track-owner codes `previewTrackOwnerCode` / `existingTrackOwnerCode`. They are
written by the build menu (`gc_setTrackOwnerCode`, the confirm/revert flow) and
read by the search as a side effect, so they are build/preview state, not
path-search scratch. Moving them onto `Pathfinder` now would couple the search
to unextracted build state against the "one caller group at a time" rule; they
land with the `BuildController`. PATH-01 closes here.

## D-015: Fleet owns the train records (FLEET-01 leaf 1)

`fleet.h` defines `struct Fleet` owning the sixteen per-train-slot record arrays
that were loose on `GameCanvas`: position/dir/phase/flags (`trainX`, `trainY`,
`trainDir`, `trainPhase`, `trainFlags`), state (`trainEraOrStage`,
`trainAgeOrTimer`, `wagonEnergyRemaining`, `trainEnergyOrCount`, `cargoAge`,
`trainStateCode`, `trainAiOverride`), the car linked-list pointer `nextCar`
(`-1` terminator; a train is a chain of car slots), the last service site each
train visited (`lastServiceVehicle`), and revenue history (`trainRevenue`,
`prevTrainRevenue`). `GameCanvas` now has one `Fleet fleet;` member and the
fields were renamed onto it (`gc->trainX` -> `gc->fleet.trainX`, etc., ~280
sites across 14 files). The `IO_BYTES`/`IO_SHORTS`/`IO_INTS` save sites became
`fleet.<field>`, so the save walks `gc->fleet.<field>` in the same byte order.
Stable slot ids, save bytes, tick order, and draw order are unchanged; goldens
did not move.

A pure ownership move: the field names are inherited as-is. Some carry recorded
uncertainty (`trainEraOrStage`, `trainAgeOrTimer`, `trainEnergyOrCount`) and are
recovered in a later FLEET-01 sub-leaf once the tick code is re-read. Related
per-train scalars not tied to a slot (`trainAtCursor`, `trainRevenueScore`,
`lifetimeTrainRevenue`, `visibleTrains`, `trainSelection*`,
`trainSpriteDirectionTable`) stay on `GameCanvas` for now as UI/economy/state.

Leaf 2 added the service-vehicle records to `Fleet` (250-slot: `vehicleFlags`,
`serviceVehicleX/Y`, `serviceResourceStock[site][5]`, `serviceUnloadFlags`,
`serviceLoadFlags`). These are the depots/stations where trains load and unload;
`serviceLoadFlags` is runtime-only (recomputed on load, not saved). Same
mechanical rename and `IO_*`/matrix save-site fix; goldens did not move.

Leaf 3 moved the city/station records to their geographic home, `World`
(48-slot: `stationDirectionCode`, `cityPopulation`, `cityIndex`) — cities are map
features, so they belong with the map data (D-012), not the fleet. The per-station
revenue history (`stationRevenue`, `prevStationRevenue`) overlaps `Economy` and
stays on `GameCanvas` for ECON-01. FLEET-01 closes here: all three slot-indexed
entity-record groups (trains and service vehicles in `Fleet`, cities in `World`)
are extracted; the city map-query operations (`gc_isCityOrStationTile` and friends)
can move onto `World`/`Fleet` once their remaining train/economy co-dependencies
land.

## D-016: Fleet movement-rule tables; rule functions stay free (FLEET-02)

The movement/cargo/merge/load/unload functions in `train_tick.cpp`
(`gc_updateTrain`, `gc_loadTrainAtServiceVehicle`, `gc_unloadTrainRevenue`,
`gc_spawnTrainCar`, `gc_resetTrailingCars`, etc.) were already clear behavioral
names, so FLEET-02 had no rename work. Its clean increment was data: the
movement-rule lookup tables `forwardTrackSteps[6][12]` / `reverseTrackSteps[6][12]`
(for each track-shape variant, the entering `[0]` and exiting `[1]` train
direction for forward/reverse travel) moved into `Fleet`. They are constant,
populated in `game_state.cpp` at init, and not saved.

The rule functions themselves stay as free functions taking `GameCanvas*`.
Converting them to `Fleet` methods is deferred to the stage-3 exit ("core rules
no longer use GameCanvas"): each touches `Fleet` + `World` + `Economy` + service
state, so it needs all collaborators extracted first, and the C-with-Classes
style permits free functions for operations. The function-signature refactor
(`GameCanvas*` -> specific owner refs) is done holistically at the stage exit,
not forced per-item. `trainSpriteDirectionTable` is render data and defers to
RENDER-01.

## D-007: Music output boundary

Game music rules use one small `MusicOutput` interface. The app owns WinMM
through `WinMmMusicPlayer`; tests use `SilentMusicOutput` and need no device or
`winmm` link. This is the only class split needed for music.

## D-008: Save helpers over owned fixed arrays

OWN-02 moved every fixed-size state array to `std::array`, which has no entry in
the old array-length registry that `midp_arr_len` reads. To keep the save byte
order exactly, each array save/load helper was split into a size-explicit
`gc_saveOrLoad*Span(gc, data, len)` core plus a thin registry wrapper for
still-raw arrays. Templated `std::array` overloads of the `*Inlined` names let
the save codec `IO_*` macros resolve to the size-explicit path automatically, so
no macro site changed. Constant tables use direct brace initializers instead of
`midp_alloc_arr` plus `memcpy`. Uniform matrices use nested `std::array`; the one
mixed-row table (`offerTypesByEffect`) uses `std::array<std::vector<int8_t>, N>`.

## D-009: Row-owning Grid<T> for maps and matrices

OWN-03 chose a row-owning `Grid<T>` (a `std::vector<std::vector<T>>` behind
`operator[]`) rather than a flat `x * height + y` buffer. This keeps every ported
`grid[x][y]` read compiling unchanged, so the map conversion touched allocation
sites, the save codec, and a few row-pointer spots instead of hundreds of index
sites. Grid rows may be jagged, which matches the padded map sizes (`mapWidth+1`,
`heightMap` `+2`) and the variable-length menu and sprite rows. The save codec
gets a `Grid<int8_t>` matrix overload that walks rows, preserving byte order.

## D-010: No array-length registry

OWN-04 removed the `midp_alloc_arr` / `midp_arr_len` / `GC_NEW` array-length
registry. It existed only because raw C pointers do not carry a length like Java
arrays. Now every game array is a `std::array`, `std::vector`, or `Grid<T>` that
knows its own size, so lengths come from `.size()` and the registry is gone. The
save codec keeps byte order through the size-explicit `*Span` cores and the
`std::array` / `Grid` overloads.

## D-006: Plain native resources

The native project keeps plain resource data. Old files `1` to `5` were changed
once with their 1-to-127 XOR key and given names based on their use. Run-time
XOR code and numeric resource paths are not part of the native design.
