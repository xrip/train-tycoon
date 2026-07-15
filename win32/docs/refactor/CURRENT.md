# Current work item

## Item

`OWN-04` - remove raw ownership and the array-length registry.

## Base

- Commit: OWN-03 closed green (maps and matrices all owned).
- Branch: `master`
- User-owned untracked file: root `AGENTS.md`; do not add or change it.

## Goal

Move the last dynamically-sized raw arrays to standard ownership, then remove the
`midp_alloc_arr`/`midp_arr_len` array-length registry and `GC_NEW`. Keep the save
byte order and all game behavior unchanged.

## Non-goals

- Do not split the game model yet (that is the MODEL stage).
- Do not change save byte order or numeric game values.

## Last proof

`OWN-03` is complete. All map grids and matrices use `Grid<T>` or nested
`std::array`. No raw `**` matrix field remains. Debug and Release fast/full tests
passed on 2026-07-15.

## Remaining raw single arrays

Six dynamically-sized 1D raw pointers remain in `GameCanvas`:

- `spriteOffsetTable` (int16_t, n+1) and `spriteData` (int8_t, n): packed sprite
  tables built once during boot.
- `spriteIndexTable` (int8_t, n+1): sprite id lookup built during boot.
- `tileOffsetX` / `tileOffsetY` (int8_t, sz): draw-order offsets rebuilt at load
  with a computed size.
- `saveBuffer` (int8_t): the growable save/compress buffer used by the codec.

## Next action

Move the sprite and tile-offset arrays to `std::vector` first (they are simple
load-time buffers). Handle `saveBuffer` last: it grows through
`gc_ensureSaveBufferCapacity` and is read with pointer arithmetic in the save
codec, so give it a `std::vector<int8_t>` with explicit offsets and keep the
exact compressed byte order. After no raw owning array remains, remove the dead
raw save wrappers, the `gc_tracked_codes(const int8_t*)` helper, and the
`midp_alloc_arr`/`midp_arr_len` registry, then run the Debug and Release full
gates and update the work records.
