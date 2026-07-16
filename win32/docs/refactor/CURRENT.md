# Current work item

## Item

`ASSET-01` - images, font, and packed sprite data.

## Base

- Commit: BUILD-01 closed and the Stage-3 data owners all exist; ASan clean.
  Tile-animation and zone-growth state homed onto World (`2e036fa`).
- Branch: `master`
- User-owned file: root `AGENTS.md` (tracked by the user; do not change it).

## Goal

Give the game assets their own owner. Extract the loaded images (font, tile
sprites, cycle flag), the sprite stream reader, and the packed sprite data
tables that are loose on `GameCanvas` into a clear owned type (`Assets`),
keeping the exact resource read order and byte behaviour.

Reuse the proven extraction pattern: introduce `struct Assets`, make it a
`GameCanvas` member, mechanical `gc->field` -> `gc->assets.field` rename, run
both full gates, commit each leaf immediately. Assets is data + owned handles;
drawing that reads it stays in RENDER territory.

## Non-goals

- Do not change resource read order, save bytes, tick order, or any golden.
- The backbuffer render target and per-frame sprite-id render state
  (`currentSpriteId`, `lastRenderedSpriteId`, `spriteJustChanged`) are Renderer
  (RENDER-01), not Assets.

## Last proof

Stage-3 data ownership + ASan green on 2026-07-16 (`2e036fa`).

## Progress

- None yet for ASSET-01. First step is the field survey below.

## Next action

Survey the asset fields on `GameCanvas`: packed sprite tables
(`spriteOffsetTable`, `spriteData`, `spriteIndexTable`, `spriteBitmapTable`,
`tileSpriteIndex`, `tileSpriteYOffsetByCode`, `trainSpriteDirectionTable`,
`tileOffsetX/Y`, `numActiveTileAnimations`), the images (`font_images`,
`tile_sprites`, `cycle_flag`), and the sprite stream reader plus load counters
(`sprite_stream`, `spriteLoadCounter`, `spriteCycleCursor`). Group them and
extract the least render-entangled leaf (the packed data tables) first.
