# HANDOFF — Train Tycoon readable decompilation

## 1. Project purpose and target state

Recover readable, faithful Java source from the obfuscated 2007 J2ME MIDlet
`Train_Tycoon_6233_N73_N95_240x320.jar` (LifeWeb Interactive; MIDP 1.0 / CLDC
1.0, 240×320). "Faithful" means the recovered code provably preserves the
original behaviour; "readable" means obfuscated identifiers are replaced with
meaningful names. **Target end state**: all three classes decompiled,
compilable, verified faithful, and progressively renamed to human-readable
identifiers — delivered as a reproducible toolchain, not hand edits.

## 2. What the JAR contains

Three classes plus resources:

- `GameMIDlet` (448 B) — MIDlet entry point.
- `b` (~675 B) — XOR resource reader. Renamed to `XorResourceInput`.
- `a` (~144 KB) — the entire game: one `Canvas implements Runnable` with
  **442 fields and 197 methods + static init**. Renamed to `GameCanvas`.
- Resources: `/1`..`/5` XOR-encrypted (cycling key 1..127), `/6` a plain
  MIDI, `/ico.png`. 200 game PNGs already extracted to `resources/images/`.

## 3. Obfuscation profile (established this session)

Name/layout obfuscation only — **ProGuard-style aggressive renaming**:

- Short 1–2 char, random-looking, heavily overloaded names (method `a(...)`
  appears 39×; field `a`, `b`, `do` (a Java keyword) etc.).
- **No** string encryption of the constant pool, **except**: high-score
  network strings (URL + HTTP headers) are XOR-encoded in `byte[]` fields
  decoded at runtime by `decodeString`. A light, targeted hiding.
- **No** control-flow flattening.

Original identifier *names* are unrecoverable (renaming is one-way). Every
method, branch, constant, and operation is recoverable. Recovery = full
logic + names we assign by understanding the code.

## 4. Toolchain (the deliverable)

All steps are subcommands of one driver, `python tools/deobf.py`:

```
setup      # fetch pinned CFR + ASM + J2ME stubs into tools/lib/
extract    # extract classes from JAR
remap      # apply mappings.txt via ASM bytecode rewrite
deobf      # full pipeline: extract + remap + CFR decompile
decompile  # CFR-only (raw classes)
xref       # regenerate analysis/XREF_GameCanvas.md
verify     # compile + round-trip decompile; the faithfulness gate
show NAME  # print a method body (resolves renamed + overloaded names)
```

Supporting tools:

- `tools/mappings.txt` — the living name map (the primary deliverable).
- `tools/Remap.java` — ASM bytecode remapper (descriptor-aware, scope-safe).
- `tools/Xref.java` — ASM cross-reference generator.
- `tools/unmapped.py` — list class-a members not yet in mappings.txt
  (supports `--obfuscated` filter and `--usage` counts).
- `tools/all_a_members.txt` — raw `javap -p` dump for cross-checking.

**Why bytecode rename, not source.** ASM `SimpleRemapper` keys methods by
`owner+name+descriptor` (overload-safe) and fields by `owner+name`. This
guarantees no false positives, and a remap is a pure rename that cannot
change behaviour. **The faithfulness gate (`verify`) is invariant under
`mappings.txt` edits** — renames need not be re-verified per batch.

## 5. Faithfulness proof (current state)

`b` and `GameMIDlet`: byte-identical round-trip through CFR 0.152.

`GameCanvas` (`a`): compiles clean after **11 documented, behaviour-
preserving fixes** to CFR emission artifacts:

- 8 lost narrowing casts (CFR dropped `i2b`/`i2s` conversions).
- 1 boolean-return-assigned-to-int.
- **2 methods** (`S()` and `g(int,int)`) where CFR split one logical flag
  into two locals — an artifact of the obfuscator's register/slot reuse.
  The correct reconstruction was read from `raw-bytecode/a.javap.txt`
  and re-decompiles clean — a stable fixpoint.

The 11 fixes are codified in `deobf.py` as exact string replacements with a
drift check that fails loudly if a different CFR version emits different
code. Details: `analysis/VERIFICATION.md`.

## 6. Renaming progress (this session)

**500 mapping entries total** as of the last shipped batch (Batch 28,
`10bfcf9`). Breakdown: **2 classes** + **381 fields** + **120 methods**.

Shipped across 28 evidence-checked batches. Each name is justified by
reading the surrounding code, never by blind text-substitution. Per-batch
counts (derived from `git show <commit>:tools/mappings.txt | grep -c '^'`):

| Batch | Anchor | Δ | Running |
|---|---|---|---|
| 1 (init) | loop bounds, byte code grids | 15 | 15 |
| 2 (init) | musicPlayer / syncHighScores / etc. | 10 | 25 |
| 3 (init) | run() timing | 17 | 42 |
| 4 | update() dispatcher + per-frame sim | 72 | 124 |
| 5 | renderMap() | 21 | 145 |
| 6 | handleKey + openBuildMenu + smoothHeightMap | 37 | 182 |
| 7 | saveGame + loadGame | 6 | 188 |
| 8 | wagon cost + AI buy + cursor/track ops + viewport pan/zoom | 41 | 229 |
| 9 | tick economy + scenario milestones + year-end balance-sheet | 36 | 265 |
| 10 | v(int) build-menu data tables | 7 | 272 |
| 11 | computeTrainCost() call sites + cost snapshots | 4 | 276 |
| 12 | packed-sprite renderer state | 12 | 288 |
| 13 | map-gen pipeline + terrain edit + selection rect | 17 | 305 |
| 14 | loan economy + per-train state + per-menu-row tables | 12 | 317 |
| 15 | previous-year + previous-decade snapshots | 17 | 334 |
| 16 | key algorithms + save/load pipeline methods | 18 | 352 |
| 17 | snapshot state + per-city/per-train code arrays | 10 | 362 |
| 18 | rendering + sprite loading + viewport tracking | 8 | 370 |
| 19 | terrain/path/placeability query methods | 10 | 380 |
| 20 | rendering pipeline + terrain editing + AI train registration | 26 | 406 |
| 21 | save/restore + year-graph + menu-build helpers | 13 | 419 |
| 22 | more menu + save helpers + canvas resize | 6 | 425 |
| 23 | AI/track + sprite atlas + display statics | 8 | 433 |
| 24 | year-graph alt data + money ticker frames | 4 | 437 |
| 25 | state counters + per-row metadata | 19 | 456 |
| 26 | AI + scenario + train placement + build-menu construction | 17 | 473 |
| 27 | high-score network decode tables | 9 | 482 |
| 28 | scenario/map-gen parameters + display half-dimensions | 18 | 500 |

Method names shipped in batches 4–28 cover most major algorithms:
`computeKeyRepeatDelay`, `rebuildCityList`, `tryBuildPath`, `validatePathStep`,
`floodFillPath`, `scanTrackTiles`, `countCarsInTrain`, `accumulateTrainTurns`,
`compressSaveBuffer`, `writeSaveSnapshot`, `pickCitySite`, `pickIndustrySite`,
`computeMilestoneScore`, `spendMoney`, `isResourceTile`, `findPathStep`,
`clearBuildPathTile`, `findLineIntersection`, `isCurveTile`,
`isPlaceableTerrain`, `isCityOrStationTile`, `randomCityTile`,
`drawTileSprite`, `resetKeyState`, `setSelectionStart`, `flattenHeightArea`,
`placeTrackTile`, `crossProduct2D`, `computeIsoCorners`,
`computeViewportBounds`, `drawMenuRow`, `drawConfirmBar`, `drawHeaderBar`,
`paintMain`, `pushMoneyHistory`, `drawNumber`, `drawMoneyTicker`,
`drawConfirmDialog`, `registerTrain`, `initBuildMenuRow`,
`initBuildMenuForScenario`, `revertTrackTiles`, `applyBackgroundCost`,
`advanceConstructionTimers`, `restoreAllMapArrays`, `computeYearEndMetrics`,
`scanMapForMetric`, `setYearGraphMode`, `placeCitiesAndIndustry`,
`autoExtendTrackStation`, `rebuildMenuTypeIndexes`,
`computeMenuRowDisplayValue`, `placeTrainAt`, `duplicateMenuRow`,
`incrementRowValueType1`, `incrementRowValueType13/14`, `adjustRowValueByCode`,
`setCanvasSize`, `saveIntToBuffer`.

The full table of renames lives in `tools/mappings.txt`.

## 7. Architecture and key design decisions

- **Rename in bytecode, not source.** ASM `SimpleRemapper` keyed by
  `owner+name+descriptor` for methods (overload-safe) and `owner+name` for
  fields. Guarantees no false positives and behaviour preservation.
- **Verify once, independent of naming.** `verify` runs on the *unmapped*
  decompilation + the embedded CFR fixes. Renames are pure, so `verify`
  need not re-run per batch.
- **CFR fixes are codified, not hand-applied.** The 11 fixes live in
  `deobf.py` as exact string replacements with a drift check.
- **`mappings.txt` is the single growing deliverable.** Everything else
  regenerates from `JAR + mappings.txt`.
- **CFR `--renameillegalidents true`** is used to emit Java-legal
  identifiers for fields whose original names were Java keywords
  (notably `do` — renamed to `currentTrainCostSnapshot`). Our mappings
  key on the **original bytecode name** (`do`), not the CFR-emitted alias
  (`cfr_renamed_0`). This pattern caught us in Batch 11; documented in
  the Batch 11 commit message.
- **Pinned tools:** CFR 0.152, ASM 9.7, microemu stubs 2.0.4 (cldcapi10,
  midpapi20, microemu-jsr-135).

## 8. What currently works

- `python tools/deobf.py deobf` — JAR → renamed readable source.
- `python tools/deobf.py verify` — compile + round-trip.
- `python tools/deobf.py xref` — regenerate the cross-reference.
- `python tools/deobf.py show NAME` — print a method body.
- `python tools/unmapped.py [--obfuscated] [--usage] [fields|methods]` —
  list unmapped identifiers.

Pipeline state at handoff: `deobf` and `verify` both **exit 0** on
Java 25.0.2 / Python 3.14.2 / Windows 11.

`run()`, `update()`, `renderMap()`, `paint()`, `handleKey()`,
`openBuildMenu()`, `computeTrainCost()`, `saveGame()`, `loadGame()`,
`syncHighScores()` all read as ordinary code.

## 9. What remains incomplete

- **Naming: 500 of ~640 GameCanvas members (~78%).** Remaining ~140 are
  ~65 fields + ~136 methods (after filtering the obfuscated-name heuristic).
  Most are deeply-nested methods that drive the build-menu animation cycle,
  per-frame input dispatch, and a few obscure per-zone/per-track arrays.
- **Method-local variables are not renamed.** The obfuscator stripped the
  `LocalVariableTable`, so locals have no bytecode identity and CFR names
  them `n`, `n2`, `bl`. A per-method source pass (JavaParser) would be
  needed; low risk since locals are method-scoped.
- **Encrypted resources `/2`..`/5`** are decrypted but their table formats
  are only partially understood (see `analysis/RESOURCE_FORMAT.md`).
- **No emulator run** has been wired up to validate behaviour dynamically.

## 10. Known issues, constraints, assumptions, risks

- **Byte-for-byte identity with the 2007 JAR is impossible** (Java 25
  cannot emit class-file v45; local slot allocation differs). Faithfulness
  is proven by *fixpoint equivalence* (re-compile + re-decompile = same
  logic, modulo local-variable names).
- **CFR-version dependence:** the 11 embedded fixes match CFR 0.152 output
  exactly. A different CFR version may emit different code; `verify`
  detects this and aborts with a clear message rather than silently
  misfixing.
- **Naming risk:** every batch is evidence-checked. A few names are
  medium-confidence — the `decadeXxxWithCurrent` family (Batch 15) and
  `prevDecade*` snapshots are described in terms of their decade array
  names, not in domain terms (e.g. "subsidy", "fuel", "revenue"). They
  are trivially revised — edit `mappings.txt`, re-run `deobf`.
- **`trainY2` / `trainX2` CFR aliases:** CFR's `--renameillegalidents`
  silently renamed two fields `gi → trainY2` and `bL → trainX2` to
  disambiguate from existing `trainY` / `trainX`. Our mappings key on
  the **original bytecode names** (`gi`, `bL`), not the CFR alias.
  Documented in the Batch 20 commit message.
- **Tooling assumptions:** requires `java`/`javac` (JDK 8+; developed
  on 25) and Python 3 on PATH, and network access once for `setup`.
- **Windows note:** classpaths use `os.pathsep` in the driver; do not
  hand-run the Java commands with `:` separators on Windows.

## 11. Build / run / test / verify

Prerequisites: JDK 8+ and Python 3 on PATH; network for first `setup`.

```sh
python tools/deobf.py setup     # fetch pinned CFR + ASM + J2ME stubs -> tools/lib/
python tools/deobf.py deobf     # regenerate src/deobfuscated/ from JAR + mappings.txt
python tools/deobf.py xref      # regenerate analysis/XREF_GameCanvas.md
python tools/deobf.py verify    # faithfulness gate: compile + round-trip
python tools/deobf.py show NAME # inspect a method
python tools/unmapped.py        # list unmapped identifiers
```

`build/` and `tools/lib/` are generated and git-ignored.

### Test results at handoff

- `deobf`: **exit 0** — 503 mapping entries applied (2 classes + 381 fields
  + 120 methods).
- `verify`: **exit 0** — "verify OK: patched decompilation compiles
  (3 classes)"; round-trip re-decompile stable (diff vs raw = local-
  variable renaming only).
- Environment: `java 25.0.2`, `Python 3.14.2`, Windows 11.
- There is no unit-test suite; `verify` is the project's correctness gate.

### Repo state and commit history

Ship point: 32 commits on `master`. Most recent first:

```
f6e5824  Add tools/unmapped.py + tools/all_a_members.txt
10bfcf9  Batch 28: scenario/map-gen parameters + display half-dimensions (500)
935d9dd  Batch 27: high-score network decode tables (482)
b5b7547  Batch 26: AI + scenario + train placement + build-menu construction (473)
ad73b41  Batch 25: state counters + per-row metadata (456)
921adf8  Batch 24: year-graph alt data + money ticker frames (437)
5a1b183  Batch 23: AI/track + sprite atlas + display statics (433)
82bc359  Batch 22: more menu + save helpers + canvas resize (425)
0669246  Batch 21: save/restore + year-graph + menu-build helpers (419)
e5c3df5  Batch 20: rendering pipeline + terrain editing + AI train registration (406)
1fb6ea5  Batch 19: terrain/path/placeability query methods (380)
0a81e58  Batch 18: rendering + sprite loading + viewport tracking (370)
6221dad  Batch 17: snapshot state + per-city/per-train code arrays (362)
7de434e  Batch 16: key algorithms + save/load pipeline methods (352)
6cd2533  Batch 15: previous-year + previous-decade snapshots (334)
388980d  Batch 14: loan economy + per-train state + per-menu-row tables (317)
88760e6  Batch 13: map-generation pipeline + terrain-edit + selection-rect state (305)
1eb8f72  Batch 12: packed-sprite renderer state (288)
197f677  HANDOFF.md: sync to ship point (276 mappings across 11 batches)
4d03b75  Batch 11: train-cost snapshots + pending-transition plumbing (276)
b4086e6  Batch 10: build-menu data tables + scenario modifier flags (272)
27a9d5d  Batch 9: tick economy + scenario milestones + year-end balance-sheet (265)
10d1c42  Batch 8: wagon cost + AI buy + cursor/track ops + viewport pan/zoom (229)
0259cbb  HANDOFF.md: update to ship point (188 mappings across 7 batches)
5b7f6ca  Batch 7: save/load record format (188)
d90d147  Batch 6: input dispatcher + build menu machinery (182)
aa3006e  Batch 5: renderMap + map-renderer infrastructure (145)
32a6fc2  Batch 4: update() dispatcher state + per-frame simulation names (124)
ead719c  Initial import: Train Tycoon J2ME deobfuscation
```

Working tree is clean (HANDOFF.md deletion is staged; will be committed
with the new handoff in this same transfer).

## 12. Current development objective and next steps

The project's goal is **complete readability** ("no obfuscated names
remain"). At handoff, 500/640 GameCanvas members are named (~78%). The
remaining ~140 are mostly methods — naming a method requires reading
its full body (which can be 100+ lines).

Priority order:

1. **Build-menu animation cycle methods** — the dozen-or-so methods that
   drive `duplicateMenuRow` / `incrementRowValueTypeN` /
   `adjustRowValueByCode` and their helpers. Reading these will likely
   name several more fields used only in the menu.
2. **Per-zone/per-track arrays** — `bE`, `bT`, `bW`, `cQ`, `cW`, `cy`
   (byte[6] / short[] / int[] etc) are mostly already-named in their
   common uses; a sweep with `unmapped.py --obfuscated --usage fields`
   will isolate them.
3. **Per-frame render-method overloads** — `a(Graphics, ...)` has many
   overloads still obfuscated. These are rendering primitives (draw
   circle, draw number, draw string); reading one or two bodies will
   reveal the pattern.
4. **Method-local variables** — add a JavaParser source pass to name
   method locals; and/or wire up microemu to run the JAR for dynamic
   validation.
5. **Decade-3 `decadeEnterpriseWithCurrent` etc. — revisit naming** once
   the game's economy is fully understood; current names are accurate
   to the code but not to the domain.

Workflow for each: `unmapped.py --obfuscated --usage fields` →
`show NAME` → read 1-3 call sites → add an evidence-checked entry to
`tools/mappings.txt` → `deobf` + `verify` → commit. `verify` stays
green for free because renames are pure bytecode transforms.

## 13. Important files and entry points

| Path | What it is |
|---|---|
| `tools/deobf.py` | **the driver** — all subcommands; embeds the 11 CFR fixes |
| `tools/mappings.txt` | **the living name map** — the 500-entry deliverable |
| `tools/Remap.java` | ASM bytecode remapper (descriptor-aware) |
| `tools/Xref.java` | ASM cross-reference generator |
| `tools/unmapped.py` | List class-a members not yet in mappings.txt |
| `tools/all_a_members.txt` | Reference javap -p dump of class `a` |
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
| `.gitignore` | excludes regenerated `build/` and fetched `tools/lib/` |

`src/faithful/` and `src/readable/` are the prior hand-reconstruction,
kept for reference; `src/decompiled_cfr/` + `src/deobfuscated/`
supersede them.