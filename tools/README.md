# Deobfuscation toolchain

One reproducible pipeline from the obfuscated JAR to verified, renamed Java.
Everything is a subcommand of `deobf.py` — no hand steps, so the work is
transferable: clone, `setup`, and run.

## Requirements

- JDK 8+ on PATH (`java`, `javac`) — developed on 25.
- Python 3.
- Network (once) for `setup` to fetch pinned tools from Maven Central.

## Usage

```sh
python tools/deobf.py setup     # fetch pinned CFR, ASM, J2ME stubs -> tools/lib/
python tools/deobf.py deobf     # extract -> remap (mappings.txt) -> decompile -> src/deobfuscated/
python tools/deobf.py xref      # regenerate analysis/XREF_GameCanvas.md
python tools/deobf.py verify    # prove the decompilation is faithful (compile + round-trip)
python tools/deobf.py show NAME # print a method body from the readable source
python tools/deobf.py all       # setup + deobf + xref
```

The naming work lives entirely in **`mappings.txt`**. Edit it, re-run `deobf`,
read the result. That is the whole loop.

## How it works

- **Renaming is done in bytecode** (`Remap.java`, ASM). Members are keyed by
  `owner + name + descriptor`, so the overloaded method `a(...)`, the field `a`,
  and the class `a` are distinct and never collide — no text/scope false
  positives. A remap is a *pure rename*: behaviour cannot change.
- Because renaming can't change behaviour, faithfulness only has to be proved
  once. **`verify`** decompiles the ORIGINAL (unmapped) classes, applies the 11
  known CFR emission fixes (embedded in `deobf.py`, see
  `analysis/VERIFICATION.md`), compiles against the J2ME stubs, and re-decompiles
  to confirm a stable fixpoint. It does not depend on `mappings.txt`.
- **`xref`** (`Xref.java`, ASM) builds a descriptor-precise cross-reference of
  every field (readers/writers) and method (callers/callees/field access/string
  constants), with subsystem auto-tags. This drives naming — pick an anchor,
  read it with `show`, harvest names, add a batch.

## Files

| File | Role |
|---|---|
| `deobf.py` | the driver (all subcommands; embeds the CFR fixes) |
| `mappings.txt` | human-editable name map — the deliverable that grows |
| `Remap.java` | ASM bytecode remapper |
| `Xref.java` | ASM cross-reference generator |
| `lib/` | pinned tool jars (created by `setup`, not committed) |
| `../build/` | intermediate artifacts (safe to delete; regenerated) |

## mappings.txt format

```
class  <oldInternal>          <newInternal>
field  <owner> <name>         <newName>
method <owner> <name> <descr> <newName>      # e.g. method a y (I)V updateTrain
```

Keys use the ORIGINAL obfuscated owner/name; method descriptors use original
types. Trailing `# ...` comments are ignored. A wrong descriptor is a harmless
no-op, never a corruption.

## Pinned versions

CFR 0.152 · ASM 9.7 · microemu J2ME stubs 2.0.4 (`cldcapi10`, `midpapi20`,
`microemu-jsr-135`). Change them in the `DEPS` table in `deobf.py`.
```
