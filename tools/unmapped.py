"""unmapped.py — list class-a members that have no entry in mappings.txt.

Usage:
    python tools/unmapped.py            # all unmapped
    python tools/unmapped.py fields     # fields only
    python tools/unmapped.py methods    # methods only
    python tools/unmapped.py --usage    # also print usage counts from source

Output columns: <kind> <owner> <name> <descriptor> [<uses>]
  uses = occurrences of `this.NAME` / `NAME(` in src/deobfuscated/GameCanvas.java
"""

import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
JAVAP = r"C:\Program Files\Java\jdk-25.0.2\bin\javap.exe"
MAPPINGS = ROOT / "tools" / "mappings.txt"
JAVAP_IN = ROOT / "build" / "classes" / "a.class"
SOURCE = ROOT / "src" / "deobfuscated" / "GameCanvas.java"


def parse_javap():
    """Return [(kind, owner, name, descriptor), ...] for class a."""
    out = subprocess.check_output(
        [JAVAP, "-p", str(JAVAP_IN)], text=True, encoding="utf-8"
    )
    members = []
    for line in out.splitlines():
        # Field example: "  public boolean gQ;"
        # Method example: "  public void a(int);"
        m = re.match(
            r"\s+(public|private|protected|static|final|synchronized|abstract|native|\s)*\s*"
            r"([\w.\[\]]+)\s+([A-Za-z_$<>][\w$]*)\s*(\(.*)?",
            line,
        )
        if not m:
            continue
        sig_etc = m.group(4) or ""
        kind = "method" if sig_etc.startswith("(") else "field"
        members.append((kind, "a", m.group(3), sig_etc.rstrip(";")))
    return members


JAVA_KEYWORDS = {
    "do", "if", "in", "is", "or", "go", "as", "on", "to", "by", "at",
    "new", "try", "for", "int", "long", "char", "byte", "void", "null",
    "true", "false", "this", "super", "class",
}


def is_obfuscated_name(name):
    """A name needs renaming if it's 1-2 chars AND not a Java keyword the
    obfuscator couldn't rename (the obfuscator left `do` alone because it's
    a keyword). Real Java override names like `run`, `paint`, `keyPressed`
    are NOT short and thus not flagged here.
    """
    if len(name) > 2:
        return False
    return name not in JAVA_KEYWORDS


def count_uses(name, kind="field"):
    """Count real references to a member in the deobfuscated source.

    For fields we match `this.NAME` to filter out local variables that share
    the name. For methods we match `name(` to filter out field-name collisions.
    """
    if not SOURCE.exists():
        return 0
    text = SOURCE.read_text(encoding="utf-8")
    if kind == "field":
        pat = re.compile(r"\bthis\." + re.escape(name) + r"\b")
    else:
        pat = re.compile(r"\b" + re.escape(name) + r"\(")
    return len(pat.findall(text))


JVM_TO_SOURCE_TYPES = {
    "B": "byte", "S": "short", "I": "int", "J": "long", "F": "float",
    "D": "double", "C": "char", "Z": "boolean", "V": "void",
}


def jvm_args_to_source(jvm_desc):
    """Convert JVM-style descriptor args like (LGameMIDlet;I[B)V to source-style
    like (GameMIDlet,int,byte[])."""
    if not jvm_desc.startswith("("):
        return jvm_desc
    close = jvm_desc.find(")")
    if close < 0:
        return jvm_desc
    args = jvm_desc[1:close]
    src_parts = []
    i = 0
    while i < len(args):
        c = args[i]
        if c == "L":
            # L<class>; — read up to ';'
            end = args.find(";", i)
            if end < 0:
                src_parts.append(c)
                break
            cls = args[i + 1 : end].replace("/", ".")
            src_parts.append(cls)
            i = end + 1
        elif c == "[":
            # array — count brackets, then element type
            depth = 1
            while depth < len(args) and args[i + depth] == "[":
                depth += 1
            elem = args[i + depth]
            if elem == "L":
                end = args.find(";", i + depth)
                cls = args[i + depth + 1 : end].replace("/", ".")
                src_parts.append(cls + "[]" * depth)
                i = end + 1
            else:
                src_parts.append(JVM_TO_SOURCE_TYPES.get(elem, elem) + "[]" * depth)
                i += depth + 1
        else:
            src_parts.append(JVM_TO_SOURCE_TYPES.get(c, c))
            i += 1
    return "(" + ",".join(src_parts) + ")"


def parse_mappings():
    """Return:
        mapped_fields:  set of (owner, name)
        mapped_methods: set of (owner, name, source-style-args-descriptor)
    """
    mapped_fields = set()
    mapped_methods = set()
    for line in MAPPINGS.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        tokens = line.split()
        kind = tokens[0]
        if kind == "class":
            continue
        if kind == "field":
            mapped_fields.add((tokens[1], tokens[2]))
        elif kind == "method":
            owner, name, desc = tokens[1], tokens[2], tokens[3]
            # Convert JVM-style descriptor args to source-style
            source_args = jvm_args_to_source(desc)
            mapped_methods.add((owner, name, source_args))
    return mapped_fields, mapped_methods


def main(argv):
    args = argv[1:]
    show_usage = "--usage" in args
    only = None
    obfuscated_only = "--obfuscated" in args
    for a in args:
        if a in ("fields", "methods"):
            only = a

    members = parse_javap()
    mapped_fields, mapped_methods = parse_mappings()

    rows = []
    for kind, owner, name, desc in members:
        if obfuscated_only and not is_obfuscated_name(name):
            continue
        if kind == "field":
            if (owner, name) in mapped_fields:
                continue
        elif kind == "method":
            # Check exact (owner, name, args-only-descriptor) match
            if (owner, name, desc) in mapped_methods:
                continue
        else:
            continue
        if only == "fields" and kind != "field":
            continue
        if only == "methods" and kind != "method":
            continue
        uses = count_uses(name, kind) if show_usage else ""
        rows.append((kind, owner, name, desc, uses))

    # Sort by kind then uses desc then name
    rows.sort(key=lambda r: (r[0], -int(r[4]) if str(r[4]).isdigit() else 0, r[2]))
    print(f"# {len(rows)} unmapped {'fields' if only=='fields' else 'methods' if only=='methods' else 'members'}")
    print("# kind  owner  name  descriptor  [uses]")
    for r in rows:
        print(*r, sep="\t")


if __name__ == "__main__":
    main(sys.argv)