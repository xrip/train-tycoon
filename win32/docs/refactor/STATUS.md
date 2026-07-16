# Refactor status

Only one row may be `in_progress`.

| Item | State | Last proof | Note |
|---|---|---|---|
| BASE-01 | done | Debug smoke passed, 2026-07-15 | Work-control files |
| BASE-02 | done | Library build and old smoke passed | Testable C library |
| BASE-03 | done | 4 doctest cases passed in 1.94 s | doctest and smoke move |
| BASE-04 | done | Two fixed-input runs passed | Fixed test inputs |
| BASE-05 | done | Debug, Release, and ASan full tests passed | Rule checks and goldens |
| BUG-ORDER-01 | done | Debug and Release digests match | Java read order restored |
| CXX-01 | done | Debug and Release full tests passed | Mechanical C++20 move |
| CXX-02 | done | Strict Debug and Release full tests passed | Strict warnings |
| CXX-03 | done | Preset Debug, Release, and ASan full tests passed | Presets and style |
| RT-01 | done | Debug and Release full tests passed | Values, random, bytes, and input |
| RT-02 | done | Debug and Release full tests passed | Plain resources, RAII reader, and paths |
| RT-03 | done | Debug and Release full tests passed | Owned images and borrowed surfaces |
| RT-04 | done | Debug and Release full tests passed | RAII record store and save path |
| RT-05 | done | Debug and Release full tests passed | Owned WinMM player and silent output |
| OWN-01 | done | Debug and Release full tests passed | Local arrays and text ownership |
| OWN-02 | done | Debug and Release full tests passed, 2026-07-15 | Fixed state arrays and tables |
| OWN-03 | done | Debug and Release full tests passed, 2026-07-15 | Maps and matrices |
| OWN-04 | done | Debug and Release full tests passed, 2026-07-15 | Remove raw registry |
| MODEL-01 | done | Debug and Release full tests passed, 2026-07-16 | Enums and constants (states, track/tile, transition bases; UI screen ids deferred to UI-01) |
| SAVE-01 | done | Debug and Release full tests passed, 2026-07-16 | Explicit save codec (direction, op selector, primitive names); SaveCodec class deferred to model stages |
| WORLD-01 | done | Debug and Release full tests passed, 2026-07-16 | World owns map grids, dimensions, in_bounds; entangled queries deferred |
| WORLD-02 | done | Debug and Release full tests passed, 2026-07-16 | Generation stages, RNG helper, and tile-range taxonomy named; slope shapes deferred |
| PATH-01 | done | Debug and Release full tests passed, 2026-07-16 | Path search scratch in `Pathfinder`; endpoints named `pathAnchor/Target`; cost/owner codes deferred to BUILD-01 |
| FLEET-01 | done | Debug and Release full tests passed, 2026-07-16 | Train + service records in `Fleet`; city records in `World`; revenue deferred to ECON-01 |
| FLEET-02 | done | Debug and Release full tests passed, 2026-07-16 | Movement-rule tables in `Fleet`; rule functions stay free until stage-3 exit |
| ECON-01 | done | Debug and Release full tests passed, 2026-07-16 | Economy owns money, loans, calendar, revenue/spend aggregates, station revenue, and decade/period report totals; cost-model constants and build/train-buy costs stay for BUILD-01 |
| SCENARIO-01 | done | Debug and Release full tests passed, 2026-07-16 | Scenario owns scores/leaderboard, milestones, AI opponent state, offer generation, scenario identity/difficulty/progression, and high-score entry; shared menu-row display arrays stay for UI-01 |
| SIM-01 | done | Debug and Release full tests passed, 2026-07-16 | Playing tick extracted into one named gc_simulationTick coordinator stating the exact tick order; gc_update dispatcher still lives in paint.cpp (smell, left for a later cleanup) |
| BUILD-01 | done | Debug and Release full tests passed, 2026-07-16 | BuildController owns track-lay, train-buy, and terrain-edit/selection action state (picks up PATH-01's selectionCost/track owner codes); cost-model constants and build-menu display stay per D-012 |
| STAGE-3-EXIT | partial | ASan full tests passed, 2026-07-16 | All Stage-3 data owners exist (World/Pathfinder/Fleet/Economy/Scenario/BuildController) and ASan is clean. Tile-animation and zone-growth state homed onto World. The core-rule `GameCanvas*` -> owner-ref signature refactor is gated on shared-state homing (rng/era/cost constants) per D-013; done last, after Stage 4 homes the UI/render/asset fields. |
| ASSET-01 | done | Debug and Release full tests passed, 2026-07-16 | Assets owns the packed sprite data tables, the loaded font/tile/cycle images, and the sprite stream reader; backbuffer and per-frame sprite-id render state stay for RENDER-01 |
| RENDER-01 | done | Debug and Release full tests passed, 2026-07-16 | Renderer owns the backbuffer/surface, current-sprite tracking, the culled visibleTrains list and drawn tile bounds (saved), the cached camera centre, and the clip/repaint/dialog-height frame state. The "drawing cannot change simulation state" property is verified at the Stage-4 exit (manual play). |
| RENDER-02 | done | Debug and Release full tests passed, 2026-07-16 | Draw functions read by intent; recovered the last weak name gc_u -> gc_wagonCarSprite |
| UI-01 | done (data) | Debug and Release full tests passed, 2026-07-16 | UiController owns the build/offer menu rows, build-menu grids/cursors, confirm-dialog and screen-transition state, report-graph display, and train/city selection. The item exit's manual play check remains a human step. |
| UI-02 | in_progress | UI-01 green | Input, cursor, pause, screen changes |
| APP-01 | queued | - | Application loop |
| CLEAN-01..CLEAN-02 | queued | - | Remove old shape |
| DOC-01 | queued | - | Final guides |
| RELEASE-01 | queued | - | Final proof |
