# Verified identifier map

These names are based on direct bytecode use, not speculative semantic matching.

| Original | Readable name | Evidence |
|---|---|---|
| `a` | `TrainTycoonCanvas` | Extends `Canvas`, implements `Runnable`, owns all game state |
| `b` | `XorResourceInput` | Reads a resource and XORs bytes with key 1..127 |
| `ex` | `midlet` | Set by `a(GameMIDlet)`, passed to `Display.getDisplay`, destroyed at shutdown |
| `gn` | `gameThread` | Constructed as `new Thread(this)` and started |
| `gQ` | `exitRequested` | Main `run()` loop terminates when true |
| `cv` | `applicationHidden` | Set by `hideNotify`, cleared by `showNotify`, gates update/render |
| `gy` | `displayInstalled` | Prevents repeated `Display.setCurrent` |
| `bF` | `screen` | Used by all state switches and assigned by `t(int)` |
| `hx` | `previousScreen` | Receives old `bF` at each transition |
| `aS` | `pendingKeyCode` | Written by `keyPressed`; `-999` from `keyReleased` |
| `z` | `repeatedKeyCode` | Re-injected after repeat delay reaches zero |
| `aF` | `repeatDelayFrames` | Decremented each frame before repeat injection |
| `dw` | `inputCooldown` | Decremented every frame and gates game actions |
| `ge` | `frameNumber` | Incremented once per loop |
| `aZ` | `simulationStepsPerFrame` | Number of state-30 simulation substeps |
| `cX` | `frameStartedAt` | Current frame timestamp |
| `dL` | `updateDuration` | Elapsed update time before painting |
| `cS` | `sleepBudget` | `35 - elapsed` frame limiter |
| `aU` | `packedResource` | Instance of class `b`, consumed by image/data readers |
| `P` | `currentFullScreenImage` | Receives images from `aa()` during splash states |
| `fG` | `gameBackBuffer` | Off-screen image allocated when entering state 30 |
| `C` | `gameBackBufferGraphics` | Graphics object from `fG.getGraphics()` |
| `bi` | `musicPlayer` | `javax.microedition.media.Player`, start/stop/loop operations |
| `E` | `volumeControl` | `javax.microedition.media.control.VolumeControl` |
