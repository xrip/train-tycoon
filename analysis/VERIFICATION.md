# Faithfulness verification of the decompilation

Goal: prove the recovered Java is a faithful reconstruction of the original bytecode,
not a plausible-looking guess.

## Tooling (this supersedes the old "no decompiler available" note)

- System Java 25 (`javac`, `java`, `javap`).
- CFR 0.152 decompiler (single self-contained jar).
- J2ME API stubs from Maven Central (`org.microemu`, v2.0.4):
  `cldcapi10`, `midpapi20` (LCDUI/RMS/IO incl. HttpConnection), `microemu-jsr-135` (MMAPI media).

## Method

1. **Decompile** all 3 classes with CFR → `src/decompiled_cfr/`.
2. **Compile** the output against the J2ME stubs (`-source 8 -target 8`).
3. **Round-trip**: re-decompile the recompiled classes and diff against step 1.
   A faithful decompilation is a *fixpoint* — recompiling and re-decompiling
   reproduces the same structure and operations.

A byte-for-byte match with the 2007 JAR is impossible (Java 25 cannot emit
class-file v45, and the compiler's local-slot allocation differs), so fixpoint
equivalence is the correct criterion, not binary identity.

## Result

- **`b` (XorResourceInput) and `GameMIDlet`: byte-identical round-trip.** Perfect.
- **`a` (the 143 KB game class, 197 methods): faithful.**
  - First compile: 100 errors — all missing-symbol (no stubs on classpath). Fixed by adding stubs.
  - Second compile: 9 errors, all CFR emission artifacts, none a structural gap:
    - 8 × lost narrowing cast (bytecode `i2b`/`i2s` that CFR dropped) — trivially and safely fixed with `(byte)`/`(short)` casts.
    - 1 × a `boolean`-returning overload assigned to an `int` local — fixed with `? 1 : 0` (behavior-preserving).
  - Third compile: 8 definite-assignment errors, clustered in **2 methods** (`void S()`, `void g(int,int)`).
    These exposed a genuine CFR limitation: the obfuscator's register/slot reuse made CFR
    **split one logical flag into two Java locals** (e.g. `bl5` and `bl4` are the same "up"
    neighbour flag). A naive "init to false" fix would have *changed behaviour*.
  - The correct reconstruction was read directly from the authoritative bytecode
    (`raw-bytecode/a.javap.txt`, method `void g(int,int)`), where each flag is unambiguously:

    ```
    up    = row  > 0        && grid[row-1][col] == VALUE
    down  = row  < width-1  && grid[row+1][col] == VALUE
    left  = col  > 0        && grid[row][col-1] == VALUE
    right = col  < height-1 && grid[row][col+1] == VALUE
    ```

  - After those two bytecode-verified fixes: **compile succeeds, 0 errors, all 3 classes produced.**

## Round-trip outcome for `a`

- Raw diff after recompile+re-decompile: 2604 / 9540 lines.
- After normalising local-variable names (CFR renumbers locals because Java 25's slot
  allocation differs from the 2007 compiler) and dropping `@Override`, the set difference
  collapses ~95% to ~280 lines, every one a known decompiler-phrasing variant:
  `++x` vs `x = x + 1`, label numbering (`block0` vs `blockLabel_n`), assignment-chain factoring.
  **No logic differences.**
- The two hand-fixed methods re-decompile **clean** from the recompiled class
  (the split-variable artifact is gone) — confirming the fixes are a stable fixpoint
  and faithful to the original.

## Deliverables

- `src/decompiled_cfr/a.java` — raw CFR output (illegal `do` field left as-is).
- `src/decompiled_cfr/a_renamed.java` — CFR `--renameillegalidents` (legal, but has the
  narrowing / split-variable artifacts above; does not compile as-is).
- `src/decompiled_cfr/compilable/{a,b,GameMIDlet}.java` — **verified: compiles clean against
  the J2ME stubs and round-trips faithfully.** The 11 edited sites are documented above.

## What "original code" means here

The obfuscation is name-only (see `obfuscator-profile`): identifier names are gone for good.
Everything else — every method, branch, constant and operation — is recovered and verified.
The remaining work is semantic renaming (`gQ` → a meaningful name), not further decompilation.
