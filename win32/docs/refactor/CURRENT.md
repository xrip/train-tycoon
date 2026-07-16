# Current work item

## Item

`RENDER-02` - named screen and widget drawing functions.

## Base

- Commit: RENDER-01 closed; Renderer owns the backbuffer and view/clip state
  (`4918329`).
- Branch: `master`
- User-owned file: root `AGENTS.md` (tracked by the user; do not change it).

## Goal

Give the screen and widget drawing functions clear behavioural names. Recover any
weak/obfuscated names left on the draw functions in `render.cpp`,
`render_map.cpp`, `train_render.cpp`, and the paint dispatch, so each drawing
function reads by intent. No data moves and no behaviour change.

## Non-goals

- Do not change draw order, save bytes, tick order, or any golden.
- Do not move state; that was RENDER-01/ASSET-01.

## Last proof

RENDER-01 closed: Debug and Release full gates passed on 2026-07-16 (`4918329`).

## Progress

- None yet for RENDER-02. First step is the weak-name survey below.

## Next action

Survey the draw functions for weak names (single-letter or descriptorless `gc_`
helpers) in the render files, recover a behavioural name for each from its body
and call sites, and rename directly in the native source. Keep each rename group
buildable with the full gates green.
