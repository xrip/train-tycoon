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

## Resource SHA-256

| File | SHA-256 |
|---|---|
| `1` | `839DC6ADA42CEBAF73953A313EAFA9CA43B9164C7A572FBC9E51CEFC71129018` |
| `2` | `E6E9552CDB0D085645090CC40B4D63F83880E600953F4826182FB6774651E019` |
| `3` | `4E1EF14AFB7011BF087CB81C5772E6450AE69D8100256DB3DD8EB175CC4EF704` |
| `4` | `A95D19A4DBB9CAB02E094E4D9799FCCB9F309FC1229BB5BCD561AB21CC2211A1` |
| `5` | `F6853EA0419CEBDC7237F0D5A9A57003C95DB4BCEE75EA7F5CBEC8CC70D650DC` |
| `6` | `EB931E0229A07605983FF215FABD2988244FA4CB38CA4FA077C764ECF9321DAA` |
| `ico.png` | `07A3558E02E4A3A5371C1821893C3425C82EAF4B9646B2E29AF4E2CA0A309BFB` |

## Current automatic coverage

- exact Java random values;
- fixed raw digests for all seven resources;
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

## Manual baseline still needed

- boot and menu;
- scenario 0 start;
- pan and zoom;
- track build and removal;
- train buy and movement;
- date change;
- save and load;
- MIDI start, volume, stop, and restart.
