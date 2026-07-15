# Current work item

## Item

`OWN-01` - move local arrays and text to standard containers.

## Base

- Commit: the green `RT-05` music commit.
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Remove raw ownership from short-lived arrays and text made inside game methods.
Keep return-value and save behavior unchanged.

## Non-goals

- Do not change fixed state arrays, maps, or matrix ownership yet.
- Do not change public game rules or save order.
- Do not add a general container layer.
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

`RT-05`: `GameCanvas` owns one `MusicOutput`. Tests use `SilentMusicOutput` and
the app uses `WinMmMusicPlayer`; all WinMM state is inside that owner. Fake
MIDP player/control pointers are gone. Debug and Release fast/full tests pass
without a `winmm` test link on 2026-07-15. The hardware test stays opt-in.

## Next action

List every local `GC_NEW`, `malloc`, returned raw text, and temporary array.
Group them by lifetime and add focused tests before changing the first group.
