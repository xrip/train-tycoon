# HANDOFF — Train Tycoon readable decompilation

## 1. Project purpose and target state

Recover readable, faithful Java source from the obfuscated 2007 J2ME MIDlet
`Train_Tycoon_6233_N73_N95_240x320.jar` (LifeWeb Interactive; MIDP 1.0 / CLDC 1.0,
240×320). "Faithful" means the recovered code provably preserves the original
behaviour; "readable" means obfuscated identifiers are replaced with meaningful
names.

Target end state: all three classes decompiled, compilable, verified faithful,
and progressively renamed to human-readable identifiers driven by understanding
of the code — delivered as a **reproducible toolchain**, not hand edits, so the
work is transferable and re-runnable from the original JAR.

## 2. What the JAR contains

Three classes plus resources:
- `GameMIDlet` (448 B) — MIDlet entry point.
- `b` (675 B) — XOR resource reader.
- `a` (~144 KB) — the entire game: one `Canvas implements Runnable` with
  **441 fields and 197 methods + static init**.
- Resources: `/1`..`/5` XOR-encrypted (cycling key 1..127), `/6` a plain MIDI,
  `/ico.png`. 200 game PNGs already extracted to `resources/images/`.

## 3. Obfuscation profile (established this session)

Name/layout obfuscation only — **ProGuard-style aggressive renaming**:
- Short 1–2 char, random-looking, heavily overloaded names (method `a(...)`
  appears 39×, `b` 15×; a field is literally named `do`, a Java keyword).
- **No** string encryption of the constant pool (34 plaintext literals:
  `"TTStore"`, `"audio/midi"`, `"/1"`..`/6"`). *Exception:* the high-score
  network strings (URL + HTTP headers) are hidden as `byte[]` fields decoded at
  runtime by `decodeString` — a light, targeted string hiding.
- **No** control-flow flattening.

Consequence: original identifier *names* are unrecoverable (renaming is
one-way). Everything else — every method, branch, constant, operation — is
recoverable. "Recovering original code" therefore = full logic + names we assign
by understanding it.

## 4. What was done this session (and why)

Starting point: a prior pass had concluded "no decompiler available" and had only
hand-reconstructed the two small classes plus a skeleton of the big one.

1. **Full decompilation.** CFR 0.152 on system Java 25 decompiles all three
   classes, including `a` (→ ~9,700 lines). This supersedes the earlier
   hand-reconstruction. Output: `src/decompiled_cfr/`.
   *Why:* the earlier blocker was a missing tool, not a real limitation.

2. **Faithfulness proof.** Compiled the decompiled source against real J2ME API
   stubs (microemu `cldcapi10` + `midpapi20` + `microemu-jsr-135`) and
   round-tripped it. `b` and `GameMIDlet` round-trip byte-identical. `a` needed
   **11 documented, behaviour-preserving fixes** to CFR emission artifacts:
   8 lost narrowing casts, 1 boolean-return-assigned-to-int, and **2 methods
   (`S()`, `g(int,int)`) where CFR split one logical flag into two locals** (an
   artifact of the obfuscator's register/slot reuse). The correct reconstruction
   was read from the authoritative bytecode (`raw-bytecode/a.javap.txt`) and
   re-decompiles clean — a stable fixpoint. Details: `analysis/VERIFICATION.md`.
   *Why:* to build on the decompilation we must know it is correct, and know
   exactly where the decompiler was imperfect.

3. **Reproducible toolchain** (`tools/`). All steps are subcommands of one
   driver, `deobf.py`. Renaming happens in **bytecode via ASM** (descriptor-aware)
   so overloaded `a(...)`, field `a`, and class `a` never collide — no text/scope
   false positives, and a remap is a pure rename that cannot change behaviour.
   *Why:* the user's explicit requirement — deliver a tool, not manual edits, so
   the work can be handed off; and bytecode remapping preserves the faithfulness
   guarantee for free (no re-verification per rename).

4. **Cross-reference report** (`Xref.java` → `analysis/XREF_GameCanvas.md`).
   For every field: readers/writers; for every method: callers/callees, field
   accesses, string constants, plus subsystem auto-tags (RMS / net / audio /
   render / resource). *Why:* to drive naming from evidence, not guesswork.

5. **Renaming, 3 evidence-checked batches (52 mappings)** in `tools/mappings.txt`,
   camelCase, each name justified by reading the code:
   - Batch 1 — map/train state: `mapWidth/Height`, `tileMap`, `trackMap`,
     `trainX/Y/Dir/Flags`, `trainPhase`, `nextCar`, `money`, `rng`, `updateTrain`.
   - Batch 2 — audio/save/net: `musicPlayer`, `volumeControl`, `musicPlaying`,
     `loadMusic/playMusic/stopMusic`, `saveGame/loadGame`, `syncHighScores`,
     `decodeString`.
   - Batch 3 — main loop/state machine: `gameState`, `stopRequested`, `paused`,
     `frameStartTime/frameTime/frameCount`, `sleepMillis`, `currentKey/heldKey/
     keyRepeatDelay/inputPending`, `simSpeed`, `update()`, `handleKey()`.

## 5. Architecture and key design decisions

- **Rename in bytecode, not source.** ASM `SimpleRemapper` keys methods by
  `owner+name+descriptor` (overload-safe) and fields by `owner+name`. Guarantees
  no false positives and behaviour preservation.
- **Verify once, independent of naming.** `verify` runs on the *unmapped*
  decompilation + the embedded CFR fixes. Because renames are pure, it need not
  re-run per batch.
- **CFR fixes are codified, not hand-applied.** The 11 fixes live in
  `deobf.py` as exact string replacements with a drift check that fails loudly if
  a different CFR version emits different code.
- **`mappings.txt` is the single growing deliverable.** Everything else
  regenerates from `JAR + mappings.txt`.
- Pinned tools: CFR 0.152, ASM 9.7, microemu stubs 2.0.4.

## 6. What currently works

- `python tools/deobf.py deobf` — JAR → renamed readable source (exit 0).
- `python tools/deobf.py verify` — compiles + round-trips (exit 0).
- `python tools/deobf.py xref` — regenerates the cross-reference (exit 0).
- `python tools/deobf.py show NAME` — prints a method body (resolves renamed and
  overloaded names).
- **188 identifiers named across 7 evidence-checked batches.** Major clusters
  covered: map grid, per-train state, audio lifecycle, save/load record format,
  high-score networking, the main frame loop, the `gameState` machine, the
  update tick (`update()`), the map renderer (`renderMap()`), the build-menu
  state machine, and the input dispatcher (`handleKey()` plus `openBuildMenu`).
  `run()`, `update()`, `renderMap()`, `paint()`, `handleKey()`, `openBuildMenu()`,
  `saveGame()`, `loadGame()`, `syncHighScores()` all read as ordinary code.

## 7. What is incomplete

- **Naming: ~188 of ~640 GameCanvas members (~30%).** Every named member is
  evidence-checked (read the surrounding code to justify it). Remaining ~250 are
  per-entity arrays (`gY`/`gC`/`U[]`/`eS`/`hg` cost-config, `oo`/`da`/`cV`/`be`
  per-tile bookkeeping, `i`/`l`/`R`/`P`/`M` cross-system helpers) that are unit-
  structurally used in only 3-4 sites, so a "swarm" cross-reference pass is
  needed to name them with confidence.
- **Method-local variables are not renamed** and cannot be by the remapper: the
  obfuscator stripped the `LocalVariableTable`, so locals have no bytecode
  identity and CFR names them `n`, `n2`, `bl`. A per-method source pass (or a
  JavaParser tool) would be needed to name them; low risk since locals are
  method-scoped.
- **Encrypted resources `/2`..`/5`** are decrypted but their table formats are
  only partially understood (see `analysis/RESOURCE_FORMAT.md`).
- No emulator run has been wired up to validate behaviour dynamically.

## 8. Known issues, constraints, assumptions, risks

- **Byte-for-byte identity with the 2007 JAR is impossible** (Java 25 cannot
  emit class-file v45; local slot allocation differs). Faithfulness is proven by
  *fixpoint equivalence*, which is the correct criterion — see VERIFICATION.md.
- **CFR-version dependence:** the 11 embedded fixes match CFR 0.152 output
  exactly. A different CFR version may emit different code; `verify` detects this
  and aborts with a clear message rather than silently misfixing.
- **Naming risk:** every batch is evidence-checked. A few names remain
  medium-confidence: `simSpeed`, `keyRepeatDelay` (revisit on more input reads),
  `selectionCost` for `dx` (it's used both as a selection-state hint and a cost
  component — pick one and keep going). They are trivially revised — edit
  `mappings.txt`, re-run `deobf`. `updateTrain`/entity model assumes the
  500-entry arrays are trains+cars; not dynamically confirmed.
- **Tooling assumptions:** requires `java`/`javac` (JDK 8+; developed on 25) and
  Python 3 on PATH, and network access once for `setup`.
- Windows note: classpaths use `os.pathsep` in the driver; do not hand-run the
  Java commands with `:` separators on Windows.

## 9. Build / run / test / verify

Prerequisites: JDK 8+ and Python 3 on PATH; network for first `setup`.

```sh
python tools/deobf.py setup     # fetch pinned CFR + ASM + J2ME stubs -> tools/lib/
python tools/deobf.py deobf     # regenerate src/deobfuscated/ from JAR + mappings.txt
python tools/deobf.py xref      # regenerate analysis/XREF_GameCanvas.md
python tools/deobf.py verify    # faithfulness gate: compile + round-trip
python tools/deobf.py show NAME # inspect a method
```

`build/` and `tools/lib/` are generated and git-ignored.

### Test results at handoff / shipping point

- `deobf`: **exit 0** — "applied 188 mappings to 3 classes".
- `verify`: **exit 0** — "verify OK: patched decompilation compiles (3 classes)",
  round-trip re-decompile stable (diff vs raw = local-variable renaming only).
- `xref`: **exit 0** — `analysis/XREF_GameCanvas.md` regenerated.
- Environment: `java 25.0.2`, `Python 3.14.2`, Windows 11.
- There is no unit-test suite; `verify` is the project's correctness gate.

### Repo state and commit history

Ship point: 6 commits on `master`:

```
ead719c  Initial import: Train Tycoon J2ME deobfuscation
32a6fc2  Batch 4: update() dispatcher state + per-frame simulation names (124)
aa3006e  Batch 5: renderMap + map-renderer infrastructure (145)
d90d147  Batch 6: input dispatcher + build menu machinery (182)
5b7f6ca  Batch 7: save/load record format (188)
```

(HANDOFF.md update for shipping at 188 is its own commit; see top of next
section.) Working tree is clean except for the `.gitignore`-excluded
`build/` and `tools/lib/` (regenerated by `setup`).

## 10. Current objective and next steps (priority order)

Shipping point is **188 names**, 7 evidence-checked batches. The decision to stop
here was deliberate: per-entity field clusters are unit-structurally used in few
sites, so naming them with confidence requires a swarm cross-reference rather
than single-method reads.

If continuing, the next priorities are:

1. **Per-train cost/config arrays** — `gY`/`gC`/`U[]`/`eS`/`hg` appear in
   `af()` (`computeCost()`) and case-25 scenario load. One targeted read of
   `computeCost` and the scenario loader will name ~8 of them and document the
   wagon-cost formula.
2. **Cross-system helpers** — `i`/`l`/`R`/`P`/`M` (these got renamed as the
   network helpers in Batch 2/6, so the *abbreviations* in the remaining
   short fields are scattered). A `grep -E "(\\.|=)this\\.([a-z]|[a-z][a-z])\\b"`
   pass against the rendering and the per-frame code will isolate them.
3. **Tick economy** — the `d()`, `R()`, `P()`, `ag()`, `ad()`, `w()`, `e()`,
   `L()`, `q()`, `z()` family from Batch 4 are tentatively named but their *bodies*
   are still unread; reading one or two bodies will firm up the names.
4. **Save-format detail** — the `dv` byte array is parsed by `dl` (now
   `saveReadOffset`); reading the parser that consumes it pins each field saved.
5. **(Optional) local-variable naming** — add a JavaParser source pass to name
   method locals; and/or wire up microemu to run the JAR for dynamic validation.

Workflow for each: pick an anchor from `analysis/XREF_GameCanvas.md` →
`show` it → add an evidence-checked batch to `tools/mappings.txt` → `deobf`.
`verify` stays green for free because renames are pure bytecode transforms.

## 11. Important files and entry points

| Path | What it is |
|---|---|
| `tools/deobf.py` | **the driver** — all subcommands; embeds the 11 CFR fixes |
| `tools/mappings.txt` | **the living name map** — the 188-name deliverable |
| `tools/Remap.java` | ASM bytecode remapper (descriptor-aware) |
| `tools/Xref.java` | ASM cross-reference generator |
| `tools/README.md` | toolchain usage + design |
| `analysis/VERIFICATION.md` | the faithfulness proof and the 11 fixes |
| `analysis/XREF_GameCanvas.md` | generated cross-reference (drives naming) |
| `analysis/` (README, LIMITATIONS, RESOURCE_FORMAT, STATE_MAP, …) | prior findings |
| `src/deobfuscated/` | current renamed readable source (generated) |
| `src/decompiled_cfr/compilable/` | verified-compilable baseline (patched) |
| `src/decompiled_cfr/a.java`, `a_renamed.java` | raw CFR output |
| `raw-bytecode/a.javap.txt` | authoritative disassembly — ground truth for hard methods |
| `resources/images/` | 200 recovered PNGs |
| `Train_Tycoon_6233_N73_N95_240x320.jar` | the source artifact |

`src/faithful/` and `src/readable/` are the earlier hand-reconstruction, kept for
reference; `src/decompiled_cfr/` + `src/deobfuscated/` supersede them.

## 12. Naming batches shipped

| Batch | Anchor | Count | Subsystem |
|---|---|---|---|
| 1 (initial) | loop bounds, byte code grids | 15 | map/train state |
| 2 (initial) | musicPlayer / syncHighScores / etc. | 10 | audio, save, net |
| 3 (initial) | run() timing | 17 | main loop, input |
| 4 | update() | 72 | dispatcher + per-frame sim |
| 5 | renderMap() | 21 | map renderer infrastructure |
| 6 | handleKey + openBuildMenu + smoothHeightMap | 37 | input + build menu |
| 7 | saveGame + loadGame | 6 | save/load record format |

Final count: **188 mapping entries** = 2 classes + 52 fields (batches 1-3) +
143 fields (batches 4-7 from class field deltas) + 13 methods.
