# What is and is not decompiled

## High-confidence source

- Complete MIDlet lifecycle.
- Complete XOR resource reader.
- Complete packed PNG reader.
- Main thread/frame limiter.
- Canvas hide/show behavior.
- Input alias normalization.
- Top-level state transitions and resource loading.
- Music start/stop lifecycle.

## Retained as bytecode

The economic simulation, pathing/map logic, rendering primitives, save-game serialization, RMS handling and possible network/high-score code remain inside the giant obfuscated class. They are all present in `raw-bytecode/a.javap.txt` and indexed in `method_inventory.csv`.

Blindly emitting 197 methods as low-quality stack-machine Java would look complete but be harder to understand and easier to misinterpret. The readable class therefore marks unknown subsystem boundaries explicitly. A full recovery would require iterative semantic renaming and validation in an emulator.
