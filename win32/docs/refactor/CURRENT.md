# Current work item

## Item

`CXX-01` - move first-party source files to C++20.

## Base

- Commit: the green `BASE-05` behavior-lock commit.
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Rename first-party `.c` files to `.cpp` and build all first-party code as
C++20 without a structural rewrite or behavior change.

## Non-goals

- No ownership or model change.
- No broad formatting or name pass.
- Warning cleanup belongs to `CXX-02` unless a warning stops the C++ build.

## Last proof

`BASE-05`: Debug and Release fast/full tests passed twice, and the full MSVC
AddressSanitizer run passed on 2026-07-15. The tests lock Java random values,
all raw resources, record files, save bytes, all 13 scenarios, input, and two
frames. `BUG-ORDER-01` restored Java evaluation order for map RNG and
multi-byte reads.

## Next action

List all first-party `.c` files, rename them to `.cpp`, update CMake, and build
the fast gate before any C++ cleanup.
