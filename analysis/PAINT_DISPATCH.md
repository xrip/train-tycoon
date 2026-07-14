# Paint dispatcher

The full bytecode is in `raw-bytecode/paint.javap.txt`. The method switches on the screen state and delegates to separate rendering branches for all states listed in `STATE_MAP.md`. Rendering state 30 uses the off-screen image allocated in `t(30)`.

No semantic renderer names were assigned because most invoked methods are overloaded one-letter identifiers and require image-by-image emulator tracing to distinguish map, train, HUD and dialog passes reliably.
