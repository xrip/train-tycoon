# Train Tycoon for Windows

This is the standalone native C++20 source for Train Tycoon. It uses C++ as C
with Classes and keeps templates and class trees simple.

The game uses a 240×320 software canvas in a 2× Win32 window. MiniFB gives the
window and key input. The local runtime gives picture, resource, save-file,
time, random-number, and MIDI support. Music uses the WinMM MCI sequencer.

## Build

Needs Windows, CMake 3.25 or later, and Visual Studio 2022 C++ build tools.

```powershell
cmake --preset windows-x64
cmake --build --preset build-debug
```

The program and its resource copy will be under `build/Debug`:

```powershell
.\build\Debug\train_tycoon.exe
```

## Test

```powershell
ctest --preset test-debug-fast
ctest --preset test-debug-full
```

The fast tests check random values, resources, record files, save data, route
finding, and MIDI data. The full tests check all 13 scenario maps, game input,
and fixed frames.

Use `build-release` and `test-release-full` for the Release gate. The native
style rules are in `docs/STYLE.md`.

For a short MIDI device test:

```powershell
$env:TRAIN_TYCOON_MIDI_TEST_MS='3000'
.\build\Debug\train_tycoon.exe
Remove-Item Env:\TRAIN_TYCOON_MIDI_TEST_MS
```

## Source map

- `src/main.cpp` — Win32 start and frame loop.
- `src/game/` — game rules, state changes, simulation, and drawing.
- `src/game_state.*` — the full game state and its first values.
- `src/game_methods.h` — game function declarations.
- `src/support/` — owned values, images, input, resources, and record files.
- `src/midp_*` — the small remaining local runtime used by the game.
- `src/midp_media_win32.cpp` — WinMM MIDI music.
- `src/third_party/stb_image.h` — PNG reading.
- `tests/` — fast and full behavior checks.
- `resources/` — plain game pictures and data, `music.mid`, and `icon.png`.

This folder is the main source for future native work. It has no build need for
Java, Python, the old JAR, or files outside this folder. If a game-behavior bug
has no clear answer in this C++ source, the decompile in the parent project may
be used only as a fact check.

The old game has no separate sound-effect file or sound-effect call. Any future
WAV effects will be a new feature.
