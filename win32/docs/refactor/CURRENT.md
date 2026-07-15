# Current work item

## Item

`RT-04` - add an RAII record store with a selected save root.

## Base

- Commit: the green `RT-03` image commit.
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Make record files close themselves, keep the selected save root in a path type,
and remove raw record-store ownership from first-party callers.

## Non-goals

- Do not change WinMM or game-array ownership yet.
- Do not change the `.rms` byte form, record ids, or missing-file behavior.
- Do not split game model parts yet.

## Last proof

`RT-02`: resources `1` to `5` were changed once to plain data and given clear
names. `music.mid` and `icon.png` were already plain. `ResourceReader` owns a
plain stream, the selected root is a `std::filesystem::path`, and all old raw
resource calls are gone. The scenario reader now reads its real record count.
Debug and Release fast/full tests passed with all fixed game digests unchanged
on 2026-07-15.

`RT-03`: `Image` owns pixels with `std::vector`; `Surface` borrows pixels.
`backbuffer`, six font images, `cycle_flag`, and `tile_sprites` now use standard
ownership. A fully clipped image draw now returns before making an invalid
source pointer. Debug and Release fast/full tests passed with all fixed frame
digests unchanged on 2026-07-15.

## Next action

Map every `MidpRecordStore` open, close, read, write, and delete call. Add direct
scope-close and root-path checks before changing the type.
