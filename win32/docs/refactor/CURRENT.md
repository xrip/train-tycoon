# Current work item

## Item

`BASE-03` - vendor doctest 2.5.3 and move smoke checks out of `main`.

## Base

- Commit: the `BASE-02` library-target commit on top of `f562412`.
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Add an offline doctest test target. Move the four automatic smoke checks into
that target. Keep only the real MIDI device check and application loop in
`main`.

## Non-goals

- No game-body rewrite or rename.
- Production game/runtime files stay C in this item.
- Keep the old executable smoke entry only if it is needed as a short bridge.

## Last proof

`BASE-02`: the new `train_tycoon_legacy` library, executable, and old smoke
test built and passed on 2026-07-15 in 7.54 seconds.

## Next action

Vendor doctest and its license, add a C++ test entry, and move the current
smoke functions with no assertion change.
