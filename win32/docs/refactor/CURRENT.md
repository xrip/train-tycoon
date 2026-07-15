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

## Next action

Map every `MidpImage` and `MidpGraphics` owner, creation call, draw target, and
free call. Add direct pixel lifetime and surface tests before changing types.
