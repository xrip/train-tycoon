# Train Tycoon 1.0 — decompilation notes

## Result

The JAR contains only three classes:

- `GameMIDlet` — 448 bytes; reconstructed essentially exactly.
- `b` — 675 bytes; reconstructed exactly as `XorResourceInput`.
- `a` — 143,905 bytes; one heavily obfuscated monolithic `Canvas` with 441 fields and 197 declared methods plus a static initializer.

A conventional source-level decompiler was not available in the execution environment.
The package therefore contains:

1. bytecode-faithful Java for the two small classes;
2. a readable structural reconstruction of the large class's verified lifecycle, frame loop, input normalization, resource loader, screen state machine and audio control;
3. the complete `javap -c -p -s` disassembly and method inventory for continued manual recovery;
4. decoded resources and all 200 PNG images reconstructed from resource `/1`.

The blank subsystem methods in `TrainTycoonCanvas.java` are explicit boundaries, not guessed implementations. The original behavior remains available in `raw-bytecode/a.javap.txt`.

## Platform

- Java ME / MIDP 1.0
- CLDC 1.0
- class-file major version 45
- target display package: 240×320
- frame budget: 35 ms (about 28.6 updates/s)

## Original manifest

```text
Manifest-Version: 1.0
Ant-Version: Apache Ant 1.7.0
Created-By: 1.5.0_11-b03 (Sun Microsystems Inc.)
MIDlet-Name: Train Tycoon
MIDlet-Vendor: LifeWeb Interactive
MIDlet-Version: 1.0
MIDlet-1: Train Tycoon, /ico.png, GameMIDlet
MIDlet-Icon: /ico.png
MicroEdition-Configuration: CLDC-1.0
MicroEdition-Profile: MIDP-1.0
```

## Resource layout

- `/1`: encrypted packed image stream; 200 PNG images recovered
- `/2`: encrypted sprite/descriptor table
- `/3`: encrypted variable-length table, probably text or UI data
- `/4`: encrypted metadata/data table
- `/5`: encrypted data table used later by the game
- `/6`: unencrypted Standard MIDI file, format 1, 8 tracks, division 120
- `/ico.png`: application icon, 50×50

Resources `/1` through `/5` use a cycling XOR key 1..127. `/6` is read directly as `audio/midi`.
