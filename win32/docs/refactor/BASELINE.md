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

- one small path search;
- save-buffer compress/decompress and four restored fields;
- MIDI file parse;
- boot, resource load, scenario 0 generation, one key, one update, and paint.

The scenario seed comes from the clock. The test checks broad facts, not exact
maps or frames. Stage 0 must fix this before structural work.

## Manual baseline still needed

- boot and menu;
- scenario 0 start;
- pan and zoom;
- track build and removal;
- train buy and movement;
- date change;
- save and load;
- MIDI start, volume, stop, and restart.
