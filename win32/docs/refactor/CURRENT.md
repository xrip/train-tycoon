# Current work item

## Item

`RT-02` - add an RAII resource reader and clear resource paths.

## Base

- Commit: the green `RT-01` support-value commit.
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Make resource input close itself, keep the selected resource root in a path
type, and remove raw resource ownership from first-party callers.

## Non-goals

- Do not change image, record-store, or WinMM ownership yet.
- Do not change XOR decoding or resource file names.
- Do not change game model fields.

## Last proof

`RT-01`: `JavaRandom`, `ByteBuffer`, `KeyEvent`, and `InputQueue` replaced their
old C shapes. Debug and Release fast/full tests passed with all fixed values
unchanged on 2026-07-15. `RT-02` now has direct path, missing-file, XOR byte,
length, and EOF checks for the old reader.

## Next action

Add the RAII `ResourceReader` and path-based resource functions. Move callers
one group at a time, starting with local readers before the sprite stream.
