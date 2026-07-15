# Current work item

## Item

`BASE-05` - add clear rule checks and fixed output digests.

## Base

- Commit: the `BASE-04` fixed-input commit on top of `9ade0a0`.
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Lock present behavior for random values, resources, record files, save bytes,
scenario generation, input, and key frames before structural work.

## Non-goals

- No game-body rewrite or broad rename.
- Do not update a value after it is recorded without a separate behavior item.
- Keep slow all-scenario work out of the fast test label.

## Last proof

`BASE-04`: two runs with fixed seed `0x5eed1234`, fixed boot fields, an
explicit resource root, and an isolated save root passed on 2026-07-15.

## Next action

Add small test support for stable byte/frame digests. Record clear expected
values first, then add scenario and save fixtures. Split CTest labels into
`fast`, `full`, and `device`.
