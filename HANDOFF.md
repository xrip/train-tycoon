# HANDOFF — Train Tycoon readable decompile and Win32 port

## Project purpose and target

This project has two parts:

1. Make a repeatable, readable Java decompile of the old Train Tycoon J2ME game.
2. Make a native Windows C23 port with the same game behavior.

The readable Java source of truth is `src/deobfuscated/GameCanvas.java`. The
native target is a 240×320 game on the MiniFB Win32 back end. The port has all
196 game methods. Native MIDI music is in place. High-score network work is a
no-op. This game JAR has no separate sound-effect files or sound-effect calls.

## Current Git state

- Branch: `master`.
- Base commit before this handoff: `ed60bdc Add native WinMM MIDI music`.
- Port state: 196/196 full methods, 0 partial methods, 441 fields.
- `AGENTS.md` is an untracked user instruction file. Keep it uncommitted unless
  the user gives a direct request.
- `build/`, `tools/lib/`, `tools/__pycache__/`, and `TTStore.rms` are ignored
  local files.

## Work now in the project

### Repeatable deobfuscation

`tools/deobf.py` takes the old JAR through extract, bytecode name changes, CFR
decompile, xref, and proof steps. `tools/mappings.txt` is the name map.
`tools/Remap.java` changes names in bytecode, with JVM descriptors to keep
overloads separate. `tools/Xref.java` makes exact member cross-references.

This design keeps all readable names repeatable. Do not hand-edit
`src/deobfuscated/GameCanvas.java`; make name changes in `tools/mappings.txt`
and run the pipeline.

### Native game port

`tools/port.py` reads the readable Java and keeps method state in
`tools/port_manifest.json`. It makes `port/generated/`. Hand-written game
methods are under `port/impl/`. The small MIDP layer is under `port/midp_*`.

The native port now has:

- game start and resource reading;
- all screen and menu states;
- map and scenario generation;
- isometric map, train, menu, and dialog drawing;
- input, map move, zoom, and tile selection;
- track, station, train, wagon, remove, and upgrade work;
- route finding and track preview;
- train motion, cargo, income, AI, month, year, and era work;
- save/load, RLE, and RecordStore file work;
- score, graph, money, and dialog data work.

The main game goal is complete at the method level: 196/196 full methods and no
generated game stubs.

### Native MIDI music

Commit `ed60bdc` added the native music part because the old J2ME game opens
resource `/6` as `audio/midi`.

- `port/midp_media_win32.c` opens `resources/6` with the WinMM MCI sequencer.
- `port/main.c` calls `midp_media_update()` each frame.
- A 100 ms MCI state check starts the MIDI again when it stops.
- Start, stop, restart, and the five game volume levels are connected.
- MIDI device volume is saved before the first change and put back at shutdown.
- The smoke suite checks the MIDI header.
- `TRAIN_TYCOON_MIDI_TEST_MS` gives a short real-device test.

MCI was used because it is a small Win32 way to play a complete MIDI file. The
game code stays close to the old `Player` start/stop/loop behavior.

### Sound-effect finding from this session

The present decompiles and raw bytecode were checked. All full decompiles have
one `Manager.createPlayer` call. It opens only `/6` as `audio/midi`.

No `playTone`, WAV player, second media `Player`, or sound-effect call is in the
code. A scan of raw and XOR-decoded resources found no RIFF/WAVE, OGG, AMR,
MP3, FLAC, AU, or second MIDI file. Resource use in the decompile shows:

- `/1` is the packed image stream;
- `/2` is sprite/scenario index data;
- `/3` is sprite/table data;
- `/4` is packed image metadata;
- `/5` is game/scenario data;
- `/6` is the only sound file, Standard MIDI, 11,573 bytes.

Result: sound effects cannot be recovered from this JAR. Any wave effects will
be a new game addition, not a faithful port of old effect data.

## Architecture and key decisions

### Source flow

```text
Train_Tycoon_6233_N73_N95_240x320.jar
  -> tools/deobf.py + tools/mappings.txt
  -> src/deobfuscated/GameCanvas.java
  -> tools/port.py + tools/port_manifest.json
  -> port/generated/*
  -> port/impl/* + port/midp_* + port/main.c
```

Do not hand-edit `port/generated/*`. Run `python tools/port.py gen` after a
manifest or Java name change.

### Runtime flow

`port/main.c` sets the resource root, starts `GameCanvas`, reads Win32 key
events, runs one old game-loop step, paints, and sends the frame to MiniFB. The
window is 240×320 game pixels at 2× size and 30 frames per second.

The old Java `run()` loop is not a second native thread. The host loop calls one
`gc_run()` step per frame.

### Java behavior support

- Game arrays use `GC_NEW` and `midp_arr_len`, because Java arrays keep length.
- Random numbers use the same Java 48-bit process; map generation needs this.
- PNG reading uses the local `stb_image` copy.
- J2ME RecordStore data uses a local `.rms` file.
- Resources `/1` through `/5` use the old cycling XOR key.
- High-score network sync returns `false`.

### Clear-name rule

If port work shows an old obfuscated member name, first use behavior and exact
xrefs to get a clear name. Put the name in `tools/mappings.txt`, run the
deobfuscation work, then run port inventory and generation. If there is not
enough fact for one clear name, record the doubt and ask before a rename.

Use clear local names in hand-written C even when a Java local name is still
short and has no known meaning.

## What works

- The old classes decompile and build against J2ME support JARs.
- The native target builds with Visual Studio 2022 and MSVC C23 mode.
- The real resources decode and the PNG pictures draw.
- The title/menu path, scenario 0 map generation, and game state 30 pass smoke.
- Path finding passes a small 5×5 build/cost smoke test.
- In-memory save, RLE pack/unpack, and field restore pass smoke.
- MIDI header, load, play, stop, volume change, restart, and shutdown pass.
- The main game has no missing port method body.

## Incomplete work and known limits

- There are no old sound effects to port. New WAV files and an event list need
  a user decision before any sound-effect code is made.
- High-score network work is a no-op by request.
- A full manual Windows play test has not been recorded.
- Smoke is not proof for every menu, scenario, AI result, long game, or disk
  save/load path.
- The build is Windows-only. `CMakeLists.txt` stops on other systems.
- MCI repeat uses a 100 ms state check, so a small end-to-start gap is possible.
- Volume uses MIDI mapper device volume. The old value is put back at normal
  shutdown, but a hard process stop may leave the changed device value.
- A volume change while music is playing stops it and starts it from the start.
- Some short smoke objects do not free every nested game allocation before the
  test process ends. No game-loop growth path is known from this.
- CFR has known output errors fixed by `tools/deobf.py`; see
  `analysis/VERIFICATION.md`.

## Build and run

Needs:

- Windows;
- CMake 3.25 or later;
- Visual Studio 2022 C build tools and MSVC, or a Windows GCC/Clang setup;
- Python 3;
- JDK 8 or later;
- project resources under `resources/`.

If the ignored Java support JARs are not present:

```powershell
python tools/deobf.py setup
```

Normal native build:

```powershell
cmake -S . -B build/port_build -G "Visual Studio 17 2022" -A x64
cmake --build build/port_build --config Debug
```

Run from the project root:

```powershell
.\build\port_build\Debug\train_tycoon.exe
```

Full automatic checks:

```powershell
python tools/deobf.py verify
python tools/port.py status -v
python tools/port.py verify
```

Short real MIDI-device check:

```powershell
$env:TRAIN_TYCOON_MIDI_TEST_MS='3000'
.\build\port_build\Debug\train_tycoon.exe
Remove-Item Env:\TRAIN_TYCOON_MIDI_TEST_MS
```

## Exact handoff checks

All checks were run on 2026-07-15 in
`E:\Train_Tycoon_readable_decompile`:

- `python tools/deobf.py verify` — PASS, exit 0. Three classes built. The
  round-trip difference was local-variable names only. Java gave the known
  old-API note.
- `python tools/port.py status -v` — PASS. 196/196 full, 0 partial, 441 fields,
  no pending methods.
- `python tools/port.py verify` — PASS, exit 0. MSBuild made
  `build/port_build/Debug/train_tycoon.exe`; all 120-frame smoke work passed.
- `TRAIN_TYCOON_MIDI_TEST_MS=3000` device check — PASS, exit 0.
- `git diff --check` — PASS before commit. The only planned change was this
  handoff; `AGENTS.md` stayed untracked by design.

## Current development objective

The faithful game port and MIDI phase are complete. The next choice is whether
to add new sound effects even though the old JAR has none. Do not make that
choice without the user.

## Next tasks, in order

1. Ask the user whether new sound effects are wanted after the no-old-effects
   result.
2. If yes, agree on game events and WAV assets before code. `PlaySound` is the
   simple choice for one effect at a time. Use `waveOut` only if effects need
   overlap or separate volume control.
3. Do a full manual Windows play test: start through the title menu, build
   track and stations, buy a train, run service, use reports and map views,
   change music settings, save, load, and exit.
4. For each real failure, first make a small repeatable check, then make the
   smallest close-to-source fix.
5. Test other scenarios and a long game if manual work shows a need.
6. Add high-score network work only after a new direct user request.

## Important files

- `CLAUDE.md` — project rules and work process.
- `src/deobfuscated/GameCanvas.java` — readable Java source of truth.
- `src/decompiled_cfr/` — raw CFR forms for fact checks.
- `raw-bytecode/a.javap.txt` — full old class bytecode.
- `tools/mappings.txt` — repeatable member name map.
- `analysis/XREF_GameCanvas.md` — exact member xrefs.
- `analysis/RESOURCE_FORMAT.md` — packed resource format.
- `analysis/VERIFICATION.md` — CFR proof and known fixes.
- `tools/deobf.py` — decompile and Java proof driver.
- `tools/port.py` — port inventory, generation, build, and smoke driver.
- `tools/port_manifest.json` — method port state and field map.
- `CMakeLists.txt` — native Windows build.
- `port/main.c` — entry, frame loop, and smoke tests.
- `port/midp_media_win32.c` — WinMM MCI MIDI back end.
- `port/midp.h` and `port/midp_*.c` — small MIDP support layer.
- `port/impl/` — hand-written game methods.
- `port/generated/` — generated state, declarations, and stubs.
- `resources/` — game data, MIDI, and decoded review files.

## Safe change process

For a Java member name change:

```powershell
# edit tools/mappings.txt
python tools/deobf.py deobf
python tools/deobf.py xref
python tools/deobf.py verify
python tools/port.py inventory
python tools/port.py gen
python tools/port.py verify
```

For a native bug fix, change only the needed `port/impl` or MIDP file and run
`python tools/port.py verify`. Keep unrelated files and user-owned
`AGENTS.md` unchanged.
