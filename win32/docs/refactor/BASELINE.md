# Native baseline

## Source

- Commit: `cd3c98b4bbe16e359215ff2136b9f1a628d1f109`
- Date: 2026-07-15
- CMake: 4.2.3
- Generator: Visual Studio 17 2022, x64
- Compiler seen in build probes: MSVC 19.44.35222

## Green commands

```powershell
cd win32
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

The one current smoke test passed in about 7.5 seconds.

## Build probes

- All current `.c` sources built as C++20 with no source change.
- The same smoke test passed in that C++20 probe.
- `/W4` gave five first-party warnings and one MiniFB warning.
- The smoke test passed with MSVC AddressSanitizer after its runtime DLL was
  put on `PATH`.

## Original resource SHA-256 before RT-02

| File | SHA-256 |
|---|---|
| `1` | `839DC6ADA42CEBAF73953A313EAFA9CA43B9164C7A572FBC9E51CEFC71129018` |
| `2` | `E6E9552CDB0D085645090CC40B4D63F83880E600953F4826182FB6774651E019` |
| `3` | `4E1EF14AFB7011BF087CB81C5772E6450AE69D8100256DB3DD8EB175CC4EF704` |
| `4` | `A95D19A4DBB9CAB02E094E4D9799FCCB9F309FC1229BB5BCD561AB21CC2211A1` |
| `5` | `F6853EA0419CEBDC7237F0D5A9A57003C95DB4BCEE75EA7F5CBEC8CC70D650DC` |
| `6` | `EB931E0229A07605983FF215FABD2988244FA4CB38CA4FA077C764ECF9321DAA` |
| `ico.png` | `07A3558E02E4A3A5371C1821893C3425C82EAF4B9646B2E29AF4E2CA0A309BFB` |

RT-02 changed files `1` to `5` to plain data and gave every resource a clear
name. The original values above stay here as conversion proof.

## Plain resource SHA-256 after RT-02

| File | SHA-256 |
|---|---|
| `sprite_images.bin` | `E987F1E597BC32608C46450B6A28715B660677897D0B856013DA206FCD4868A2` |
| `tile_sprite_ids.bin` | `849FA389100FC0D39BDB2B3D88E8DF2B17BD5016F4575EDBDD3A3BE80E1DE6A6` |
| `sprite_image_map.bin` | `847B2FE10F3D6D3C7DFA285D3F4F767B5A3BEBB15E8830A5BBFEDCAABF9C1E44` |
| `packed_sprites.bin` | `4BB82ECB8834FB43B988A5758B86DB63AF63E83D76EE1033023285DAE2EDC5A5` |
| `scenarios.bin` | `67C034FD39D94239460E3C024E6A936CD61C52C448B6BF225E9BC4C4B86DEAF2` |
| `music.mid` | `EB931E0229A07605983FF215FABD2988244FA4CB38CA4FA077C764ECF9321DAA` |
| `icon.png` | `07A3558E02E4A3A5371C1821893C3425C82EAF4B9646B2E29AF4E2CA0A309BFB` |

## Current automatic coverage

- exact Java random values;
- fixed plain digests for all seven resources;
- record-store bytes, ids, reload, and delete;
- fixed save-buffer size and digest, compress/decompress, and restored fields;
- one small path search;
- MIDI file parse;
- fixed map digests for all 13 scenarios;
- boot, resource load, scenario 0 generation, input, update, and two fixed
  frame digests.

All tests use seed `0x5eed1234`, explicit resource and save roots, and an
isolated test save directory. Fast and full CTest labels are separate. The
real MIDI device test is an opt-in `device` label.

## Stage 0 proof

- Debug fast/full passed twice on 2026-07-15.
- Release fast/full passed twice on 2026-07-15.
- MSVC AddressSanitizer fast/full passed on 2026-07-15.
- Debug and Release use the same scenario and frame golden values.

## Stage 1 proof

- All 35 source files build as C++20.
- First-party targets use `/W4 /permissive- /WX` under MSVC.
- Preset Debug and Release full tests passed on 2026-07-15.
- MSVC AddressSanitizer fast/full passed on 2026-07-15.

## RT-02 proof

- The native resource folder has seven plain files with clear names.
- Debug and Release fast/full tests passed on 2026-07-15.
- All scenario, input, save, and frame golden values are unchanged.

## RT-03 proof

- `Image` owns pixels; `Surface` only borrows them.
- All game image owners use `std::unique_ptr` containers.
- Debug and Release fast/full tests passed on 2026-07-15.
- All fixed frame golden values are unchanged.

## RT-04 proof

- `RecordStore` owns all record bytes and writes on scope end.
- The save root uses `std::filesystem::path`.
- A direct test proves the exact 21-byte `.rms` form for one fixed record.
- Debug and Release fast/full tests passed on 2026-07-15.

## RT-05 proof

- `GameCanvas` owns one music output; no global WinMM state remains.
- Silent tests prove load, disabled play, play, stop, and volume rules.
- Normal tests do not link `winmm` and need no MIDI device.
- Debug and Release fast/full tests passed on 2026-07-15.
- The real MIDI device test remains an opt-in CTest gate.

## OWN-01 proof

- Short-lived PNG, city, train-visibility, terrain-patch, and save-growth
  buffers use `std::vector`.
- Made text code values use `std::vector`, optional dialog values use
  `std::optional`, and drawing borrows text with `std::span`.
- Debug and Release fast/full tests passed on 2026-07-15.
- All fixed map, save, input, and frame values are unchanged.

## OWN-02 proof

- Every fixed-size raw state array and constant table is now `std::array`,
  including per-train 500-slot arrays, service and city arrays, financial
  history, menu row tables, offer tables, and the track-step and
  sprite-direction constant tables.
- Save helpers split into size-explicit `*Span` cores plus registry wrappers,
  with templated `std::array` overloads of the `*Inlined` names; save byte
  order is unchanged.
- Dead `dk` and `j` offset tables were removed.
- Debug and Release fast/full tests passed on 2026-07-15.
- All scenario, input, save, and frame golden values are unchanged.

## Manual baseline still needed

- boot and menu;
- scenario 0 start;
- pan and zoom;
- track build and removal;
- train buy and movement;
- date change;
- save and load;
- MIDI start, volume, stop, and restart.
