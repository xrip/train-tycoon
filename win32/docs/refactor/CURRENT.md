# Current work item

## Item

`RT-01` - add value types, exact Java random, byte buffer, and input queue.

## Base

- Commit: the green `CXX-03` Stage 1 exit commit.
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Replace small support-code C shapes with direct C++ value and ownership types
while keeping every fixed random, input, save, scenario, and frame value.

## Non-goals

- Do not move game model fields in this item.
- Do not change image, resource, record-store, or WinMM ownership yet.
- Do not add a general template utility.

## Last proof

Stage 1: preset Debug and Release full tests passed, and the MSVC
AddressSanitizer full test passed on 2026-07-15. All first-party code is C++20
and strict-warning clean. `RT-01` JavaRandom is now a small value class; the
vector-backed ByteBuffer now owns save RLE and tile-offset output. The Debug
full test kept every random, save, scenario, and frame value.

## Next action

Replace the global Win32 input ring with `KeyEvent` and a small `InputQueue`.
Add direct debounce, mapping, order, and full-queue rule tests first.
