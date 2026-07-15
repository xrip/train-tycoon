# Current work item

## Item

`CXX-03` - add build/test presets and final C++ style rules.

## Base

- Commit: the green `CXX-02` strict-warning commit.
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Give programmers short, fixed configure/build/test commands and record the
C-with-Classes rules used for all later work.

## Non-goals

- No source formatting pass.
- No ownership or model change.
- Do not add a new package or format tool.

## Last proof

`CXX-02`: first-party library, application, and tests build with
`/W4 /permissive- /WX`. Vendor code and headers are outside that warning gate.
Debug and Release fast/full tests passed on 2026-07-15.

## Next action

Add `CMakePresets.json` for x64 Debug/Release and fast/full tests. Add one short
style guide that makes the C-with-Classes limit clear.
