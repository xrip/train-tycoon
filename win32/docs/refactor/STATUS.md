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
| CXX-03 | in_progress | CXX-02 green | Presets and style |
| RT-01..RT-05 | queued | - | Support classes |
| OWN-01..OWN-04 | queued | - | Standard ownership |
| MODEL-01 | queued | - | Enums and constants |
| SAVE-01 | queued | - | Legacy save codec |
| WORLD-01..WORLD-02 | queued | - | World and generation |
| PATH-01 | queued | - | Path search |
| FLEET-01..FLEET-02 | queued | - | Trains and service sites |
| ECON-01 | queued | - | Economy and calendar |
| SCENARIO-01 | queued | - | Scenarios and AI |
| SIM-01 | queued | - | Tick coordinator |
| BUILD-01 | queued | - | Build actions |
| ASSET-01 | queued | - | Game assets |
| RENDER-01..RENDER-02 | queued | - | Drawing |
| UI-01..UI-02 | queued | - | UI and input |
| APP-01 | queued | - | Application loop |
| CLEAN-01..CLEAN-02 | queued | - | Remove old shape |
| DOC-01 | queued | - | Final guides |
| RELEASE-01 | queued | - | Final proof |
