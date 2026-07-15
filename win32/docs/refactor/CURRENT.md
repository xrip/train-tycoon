# Current work item

## Item

`RT-05` - add `WinMmMusicPlayer` and a silent test output.

## Base

- Commit: the green `RT-04` record-store commit.
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Put WinMM MIDI state and lifetime in one owner. Keep game music rules separate
from the device, and give tests a silent output that needs no sound hardware.

## Non-goals

- Do not change game-array ownership yet.
- Do not add sound effects or change MIDI loop, stop, or volume rules.
- Do not make the tests need a MIDI device.
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

`RT-04`: `RecordStore` owns record data with standard containers and writes on
scope end. The selected save root is a `std::filesystem::path`. All old raw
record calls are gone. Debug and Release fast/full tests passed, and a direct
21-byte check proves the `.rms` form is unchanged on 2026-07-15.

## Next action

Map game music rules against the current global WinMM state. Add a small music
output interface and silent state test before moving the device code.
