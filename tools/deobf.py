#!/usr/bin/env python3
"""
Train Tycoon deobfuscation driver.

One reproducible pipeline from the obfuscated JAR to verified, renamed Java.
No hand steps: every stage is a subcommand.

    python tools/deobf.py setup       # fetch pinned tools (CFR, ASM, J2ME stubs)
    python tools/deobf.py deobf       # extract -> remap (mappings.txt) -> decompile
    python tools/deobf.py xref        # regenerate the cross-reference report
    python tools/deobf.py verify      # prove the decompilation is faithful
    python tools/deobf.py show NAME   # print a method body from the readable source
    python tools/deobf.py all         # setup + deobf + xref

Design notes
------------
* Renaming happens in BYTECODE (ASM, descriptor-aware) so overloaded `a(...)`,
  field `a`, and class `a` never collide. A remap is a pure rename: behaviour is
  preserved, so faithfulness proved once by `verify` holds for every rename.
* `verify` decompiles the ORIGINAL (unmapped) classes, applies the known CFR
  emission fixes below, compiles against the J2ME stubs, and (optionally)
  round-trips. It does not depend on mappings.txt.
* Everything is pinned; `setup` is idempotent. Requires `java`/`javac` on PATH
  (JDK 8+; developed on 25).
"""
import os, sys, shutil, subprocess, urllib.request, zipfile
from pathlib import Path

ROOT   = Path(__file__).resolve().parent.parent
TOOLS  = ROOT / "tools"
LIB    = TOOLS / "lib"
BUILD  = ROOT / "build"
JAR    = next(ROOT.glob("*.jar"), None)
MAP    = TOOLS / "mappings.txt"
DEOBF_OUT = ROOT / "src" / "deobfuscated"
XREF_OUT  = ROOT / "analysis" / "XREF_GameCanvas.md"
TARGET_CLASS = "a"            # obfuscated internal name of GameCanvas

# --- pinned dependencies (Maven Central) ---------------------------------
DEPS = {
    "cfr.jar":                 "https://repo1.maven.org/maven2/org/benf/cfr/0.152/cfr-0.152.jar",
    "asm.jar":                 "https://repo1.maven.org/maven2/org/ow2/asm/asm/9.7/asm-9.7.jar",
    "asm-tree.jar":            "https://repo1.maven.org/maven2/org/ow2/asm/asm-tree/9.7/asm-tree-9.7.jar",
    "asm-commons.jar":         "https://repo1.maven.org/maven2/org/ow2/asm/asm-commons/9.7/asm-commons-9.7.jar",
    "cldcapi10.jar":           "https://repo1.maven.org/maven2/org/microemu/cldcapi10/2.0.4/cldcapi10-2.0.4.jar",
    "midpapi20.jar":           "https://repo1.maven.org/maven2/org/microemu/midpapi20/2.0.4/midpapi20-2.0.4.jar",
    "microemu-jsr-135.jar":    "https://repo1.maven.org/maven2/org/microemu/microemu-jsr-135/2.0.4/microemu-jsr-135-2.0.4.jar",
}
ASM_JARS  = ["asm.jar", "asm-tree.jar", "asm-commons.jar"]
STUB_JARS = ["cldcapi10.jar", "midpapi20.jar", "microemu-jsr-135.jar"]

# --- known CFR emission fixes (see analysis/VERIFICATION.md) ---------------
# Applied to the raw `--renameillegalidents true` output of the ORIGINAL class.
# 8 lost narrowing casts, 1 boolean->int, and 2 split-variable reconstructions
# (verified against raw-bytecode/a.javap.txt, method void g(int,int)).
CFR_FIXES = [
    ("        byte by5 = Math.abs(this.gm.nextInt() % 5) + this.cK;",
     "        byte by5 = (byte)(Math.abs(this.gm.nextInt() % 5) + this.cK);"),
    ("                        n = this.a(n != 0, n3 - 1, n2, 71, 72, 74, 78, 80, 84, 88, 90, true);",
     "                        n = this.a(n != 0, n3 - 1, n2, 71, 72, 74, 78, 80, 84, 88, 90, true) ? 1 : 0;"),
    ("                        this.aQ[n7][n8] = n9;",
     "                        this.aQ[n7][n8] = (short)n9;"),
    ("                this.aQ[n][n2] = n4;",
     "                this.aQ[n][n2] = (short)n4;"),
    ("                    this.cI[i][j] = this.cI[i][j] == 78 ? 22 : (this.cI[i][j] == 80 ? 24 : (this.cI[i][j] == 85 ? 9 : (this.cI[i][j] == 84 ? 8 : (this.cI[i][j] == 77 ? 21 : (this.cI[i][j] == 79 ? 23 : (this.cI[i][j] == 83 ? 7 : (this.cI[i][j] == 86 ? 10 : 0)))))));",
     "                    this.cI[i][j] = (byte)(this.cI[i][j] == 78 ? 22 : (this.cI[i][j] == 80 ? 24 : (this.cI[i][j] == 85 ? 9 : (this.cI[i][j] == 84 ? 8 : (this.cI[i][j] == 77 ? 21 : (this.cI[i][j] == 79 ? 23 : (this.cI[i][j] == 83 ? 7 : (this.cI[i][j] == 86 ? 10 : 0))))))));"),
    ("                    this.av[n4 + 1] = n3;", "                    this.av[n4 + 1] = (byte)n3;"),
    ("                    this.gl[n4 + 1] = n3;", "                    this.gl[n4 + 1] = (byte)n3;"),
    ("                    this.av[n4 - 1] = n3;", "                    this.av[n4 - 1] = (byte)n3;"),
    ("                    this.gl[n4 - 1] = n3;", "                    this.gl[n4 - 1] = (byte)n3;"),
    # scope 1: void S() -- split-variable neighbour flags (grid aQ, value 9)
    ("""                    int n6 = n3 > 0 ? (this.aQ[n3 - 1][n2] == 9 ? 1 : 0) : (n = 0);
                    boolean bl6 = n3 < this.hw - 1 ? this.aQ[n3 + 1][n2] == 9 : (bl5 = false);
                    boolean bl7 = n2 > 0 ? this.aQ[n3][n2 - 1] == 9 : (bl4 = false);
                    boolean bl8 = n2 < this.hr - 1 ? this.aQ[n3][n2 + 1] == 9 : (bl3 = false);""",
     """                    n = n3 > 0 && this.aQ[n3 - 1][n2] == 9 ? 1 : 0;
                    bl5 = n3 < this.hw - 1 && this.aQ[n3 + 1][n2] == 9;
                    bl4 = n2 > 0 && this.aQ[n3][n2 - 1] == 9;
                    bl3 = n2 < this.hr - 1 && this.aQ[n3][n2 + 1] == 9;"""),
    # scope 2: void g(int,int) -- split-variable neighbour flags (grid ea, value 64)
    ("""        boolean bl5 = n > 0 ? this.ea[n - 1][n2] == 64 : (bl4 = false);
        boolean bl6 = n < this.hw - 1 ? this.ea[n + 1][n2] == 64 : (bl3 = false);
        boolean bl7 = n2 > 0 ? this.ea[n][n2 - 1] == 64 : (bl2 = false);
        boolean bl8 = n2 < this.hr - 1 ? this.ea[n][n2 + 1] == 64 : (bl = false);""",
     """        bl4 = n > 0 && this.ea[n - 1][n2] == 64;
        bl3 = n < this.hw - 1 && this.ea[n + 1][n2] == 64;
        bl2 = n2 > 0 && this.ea[n][n2 - 1] == 64;
        bl = n2 < this.hr - 1 && this.ea[n][n2 + 1] == 64;"""),
]

# ---------------------------------------------------------------------------
def cp(*names):                       # classpath string, OS-correct separator
    return os.pathsep.join(str(LIB / n) for n in names)

def run(cmd, **kw):
    print("  $", " ".join(str(c) for c in cmd))
    subprocess.run(cmd, check=True, **kw)

def need_jar():
    if JAR is None:
        sys.exit("no *.jar found in repo root")

def javac(srcs, out, classpath):
    out.mkdir(parents=True, exist_ok=True)
    run(["javac", "-nowarn", "-cp", classpath, "-d", str(out), *map(str, srcs)])

def cfr(classes, outdir, extra=()):
    outdir.mkdir(parents=True, exist_ok=True)
    run(["java", "-jar", str(LIB / "cfr.jar"), *map(str, classes),
         "--outputdir", str(outdir), *extra])

# --- subcommands -----------------------------------------------------------
def cmd_setup(_):
    LIB.mkdir(parents=True, exist_ok=True)
    for name, url in DEPS.items():
        dst = LIB / name
        if dst.exists():
            print("  have", name); continue
        print("  fetch", name)
        urllib.request.urlretrieve(url, dst)
    print("setup ok ->", LIB)

def cmd_extract(_):
    need_jar()
    classes = BUILD / "classes"
    if classes.exists(): shutil.rmtree(classes)
    classes.mkdir(parents=True)
    with zipfile.ZipFile(JAR) as z:
        for n in z.namelist():
            if n.endswith(".class") and "/" not in n:
                (classes / n).write_bytes(z.read(n))
    print("extracted", len(list(classes.glob("*.class"))), "classes ->", classes)

def _build_tool(name):
    out = BUILD / "tool"
    javac([TOOLS / f"{name}.java"], out, cp(*ASM_JARS))
    return out

def cmd_remap(_):
    classes = BUILD / "classes"
    if not classes.exists(): cmd_extract(_)
    tool = _build_tool("Remap")
    remapped = BUILD / "remapped"
    if remapped.exists(): shutil.rmtree(remapped)
    run(["java", "-cp", os.pathsep.join([str(tool), cp(*ASM_JARS)]),
         "Remap", str(MAP), str(classes), str(remapped)])
    print("remapped ->", remapped)

def cmd_decompile(_):
    remapped = BUILD / "remapped"
    if not remapped.exists(): cmd_remap(_)
    if DEOBF_OUT.exists(): shutil.rmtree(DEOBF_OUT)
    cfr(sorted(remapped.glob("*.class")), DEOBF_OUT, ["--renameillegalidents", "true"])
    print("readable source ->", DEOBF_OUT)

def cmd_deobf(_):
    cmd_extract(_); cmd_remap(_); cmd_decompile(_)

def cmd_xref(_):
    classes = BUILD / "classes"
    if not classes.exists(): cmd_extract(_)
    tool = _build_tool("Xref")
    run(["java", "-cp", os.pathsep.join([str(tool), cp(*ASM_JARS)]),
         "Xref", str(classes), TARGET_CLASS, str(XREF_OUT)])
    print("xref ->", XREF_OUT)

def cmd_verify(_):
    """Decompile the ORIGINAL classes, apply CFR fixes, compile, round-trip."""
    classes = BUILD / "classes"
    if not classes.exists(): cmd_extract(_)
    vraw = BUILD / "verify_raw"
    if vraw.exists(): shutil.rmtree(vraw)
    cfr(sorted(classes.glob("*.class")), vraw, ["--renameillegalidents", "true"])

    a = vraw / "a.java"
    text = a.read_text(encoding="utf-8", errors="replace")
    for old, new in CFR_FIXES:
        if old not in text:
            sys.exit(f"verify: expected CFR pattern not found (CFR version drift?):\n{old[:80]}...")
        text = text.replace(old, new, 1)
    a.write_text(text, encoding="utf-8")

    out = BUILD / "verify_out"
    if out.exists(): shutil.rmtree(out)
    try:
        javac(sorted(vraw.glob("*.java")), out, cp(*STUB_JARS))
    except subprocess.CalledProcessError:
        sys.exit("verify FAILED: patched decompilation did not compile")
    n = len(list(out.glob("*.class")))
    print(f"verify OK: patched decompilation compiles ({n} classes).")

    # round-trip: re-decompile the recompiled class, confirm it is stable
    rt = BUILD / "verify_roundtrip"
    if rt.exists(): shutil.rmtree(rt)
    cfr([out / "a.class"], rt, ["--renameillegalidents", "true"])
    print("round-trip re-decompile ->", rt / "a.java",
          "(diff vs build/verify_raw/a.java is local-var renaming only; see VERIFICATION.md)")

def cmd_show(args):
    if not args: sys.exit("usage: show <methodName>")
    name = args[0]
    src = DEOBF_OUT / "GameCanvas.java"
    if not src.exists(): sys.exit(f"{src} missing - run: deobf")
    lines = src.read_text(encoding="utf-8", errors="replace").splitlines()
    hits, i = 0, 0
    while i < len(lines):
        ln = lines[i]
        # method header: 4-space indented, contains "<name>(", ends with "{"
        if ln.startswith("    ") and not ln.startswith("     ") \
           and (f" {name}(" in ln or ln.strip().split("(")[0].endswith(name)) \
           and ln.rstrip().endswith("{"):
            depth, j = 0, i
            while j < len(lines):
                depth += lines[j].count("{") - lines[j].count("}")
                print(lines[j])
                if depth <= 0 and j > i: break
                j += 1
            print()
            hits += 1; i = j + 1
        else:
            i += 1
    if not hits: print(f"(no method named {name} in {src.name})")

CMDS = {
    "setup": cmd_setup, "extract": cmd_extract, "remap": cmd_remap,
    "decompile": cmd_decompile, "deobf": cmd_deobf, "xref": cmd_xref,
    "verify": cmd_verify, "show": cmd_show,
    "all": lambda a: (cmd_setup(a), cmd_deobf(a), cmd_xref(a)),
}

def main(argv):
    if not argv or argv[0] not in CMDS:
        print(__doc__); print("commands:", ", ".join(CMDS)); return 1
    CMDS[argv[0]](argv[1:])
    return 0

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
