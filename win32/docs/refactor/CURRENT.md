# Current work item

## Item

`BASE-02` - make a testable C library without changing game bodies.

## Base

- Commit: the `BASE-01` documentation commit on top of
  `cd3c98b4bbe16e359215ff2136b9f1a628d1f109`
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Put all game/runtime sources except the Win32 entry and MiniFB implementation
in a linkable library. Keep the executable and smoke behavior unchanged.

## Non-goals

- No game-body rewrite or rename.
- No C++ language move.
- No test-framework change yet.

## Last proof

`BASE-01`: `cmake --build build --config Debug` and
`ctest --test-dir build -C Debug --output-on-failure` passed on 2026-07-15 in
7.53 seconds.

## Next action

Split the CMake source list into `train_tycoon_legacy` and the small executable
source. Build and run the old smoke test.
