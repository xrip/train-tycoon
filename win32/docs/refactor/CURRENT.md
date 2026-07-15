# Current work item

## Item

`CXX-02` - make first-party C++ clean under strict MSVC warnings.

## Base

- Commit: the green `CXX-01` C++20 move commit.
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Build first-party code with `/W4 /permissive- /WX`. Keep vendor code out of
the warning-as-error gate.

## Non-goals

- No ownership or model change.
- No broad formatting or name pass.
- Do not edit MiniFB or `stb_image` for a first-party warning rule.

## Last proof

`CXX-01`: all 35 source files build as C++20. Debug and Release fast/full tests
passed with every Stage 0 digest unchanged on 2026-07-15.

## Next action

Split vendor compilation from the first-party warning gate, enable the strict
MSVC flags, and record each warning before making a fix.
