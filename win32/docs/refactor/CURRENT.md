# Current work item

## Item

`RT-03` - add owning images and non-owning drawing surfaces.

## Base

- Commit: the green `RT-02` resource commit.
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Give image pixels normal ownership and make a clear split between an owned
image and a non-owning drawing surface.

## Non-goals

- Do not change record-store, WinMM, or game-array ownership yet.
- Do not change drawing rules, blend rules, PNG output, or frame order.
- Do not split game model parts yet.

## Last proof

`RT-02`: resources `1` to `5` were changed once to plain data and given clear
names. `music.mid` and `icon.png` were already plain. `ResourceReader` owns a
plain stream, the selected root is a `std::filesystem::path`, and all old raw
resource calls are gone. The scenario reader now reads its real record count.
Debug and Release fast/full tests passed with all fixed game digests unchanged
on 2026-07-15.

`RT-03` ownership map: `backbuffer`, six font images, `cycleFlag`, and the tile
image list own images. `backbufferG` and the screen drawing context borrow pixel
storage. Direct tests now lock blank-image pixels, shared drawing storage,
invalid size handling, decoded PNG size, and decoded-image immutability.

## Next action

Add owning `Image` and non-owning `Surface` types. Move the image runtime first,
then move each `GameCanvas` image owner to standard ownership.
