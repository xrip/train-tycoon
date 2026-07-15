# Train Tycoon

The active native Windows C project is under [`win32/`](win32/). Start with
[`win32/README.md`](win32/README.md) for build and test steps.

The rest of this repository keeps the old J2ME decompile, bytecode, resource
work, and first port pipeline as a behavior reference for hard bugs:

- `src/deobfuscated/` — readable generated Java;
- `analysis/README.md` — findings and confidence boundaries;
- `analysis/NAME_MAP.md` — verified identifier renames;
- `resources/images/` — 200 recovered game images;
- `raw-bytecode/a.javap.txt` — complete disassembly of the original game class.

Do normal game work in `win32/`. Use the decompile only when native game
behavior is in doubt.
