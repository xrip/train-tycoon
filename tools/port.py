#!/usr/bin/env python3
"""port.py - toolchain for the Train Tycoon J2ME -> Win32/C23 port.

Goal: make the port *resumable from any point* (no bus factor). The Java in
`src/deobfuscated/GameCanvas.java` is the single source of truth. This driver
extracts its fields and methods, generates the C struct + init + method
prototypes + stubs, and tracks which methods have been hand-ported in a manifest
(the analogue of the deobfuscation project's mappings.txt).

Workflow to continue the port:

    python tools/port.py inventory   # (re)scan the Java -> manifest (idempotent)
    python tools/port.py status      # how many fields/methods are ported
    python tools/port.py gen         # regenerate port/generated/*
    python tools/port.py show NAME   # print a Java method body to port it
    python tools/port.py next        # suggest the next (leaf) method to port
    python tools/port.py build       # cmake build
    python tools/port.py verify      # build + headless smoke run

To port a method: write its C body in port/impl/<name>.c, set its status to
"ported" in tools/port_manifest.json (or run `python tools/port.py mark NAME`),
then `gen` + `build`. The generator emits a stub only for un-ported methods, so
there is never a duplicate symbol.
"""
import json, os, re, subprocess, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
JAVA = os.path.join(ROOT, "src", "deobfuscated", "GameCanvas.java")
MANIFEST = os.path.join(ROOT, "tools", "port_manifest.json")
GEN_DIR = os.path.join(ROOT, "port", "generated")
BUILD_DIR = os.path.join(ROOT, "build", "port_build")

# --- Java type -> C type ------------------------------------------------------
PRIM = {"int": "int32_t", "boolean": "bool", "byte": "int8_t",
        "short": "int16_t", "long": "int64_t", "char": "uint16_t", "void": "void"}
REF = {"Image": "MidpImage*", "Graphics": "MidpGraphics*", "Random": "MidpRandom",
       "Vector": "MidpVector*", "XorResourceInput": "MidpResource*",
       "String": "char*", "Thread": "void*", "Player": "void*",
       "VolumeControl": "void*", "GameMIDlet": "void*",
       "ByteArrayOutputStream": "MidpByteArray*"}


def c_type(java_type):
    """Map a Java type token (possibly with [] suffixes) to a C type string."""
    dims = java_type.count("[]")
    base = java_type.replace("[]", "").strip()
    if base in PRIM:
        ct = PRIM[base]
    elif base in REF:
        ct = REF[base]
    else:
        ct = "void*"  # unknown reference type
    return ct + "*" * dims


# --- Parsing ------------------------------------------------------------------
def read_java():
    with open(JAVA, "r", encoding="utf-8") as f:
        return f.read().split("\n")


FIELD_RE = re.compile(
    r"^\s*(?P<mods>(?:static\s+|final\s+|public\s+|private\s+|protected\s+)*)"
    r"(?P<type>[A-Za-z_][\w.]*(?:\s*\[\s*\])*)\s+"
    r"(?P<name>[A-Za-z_]\w*)\s*"
    r"(?:=\s*(?P<init>.*?))?;\s*$")

METHOD_RE = re.compile(
    r"^    (?!(?:if|for|while|switch|else|do|return|try|catch|synchronized)\b)"
    r"(?P<sig>(?P<mods>(?:public\s+|private\s+|protected\s+|static\s+|final\s+)*)"
    r"(?P<ret>[A-Za-z_][\w.]*(?:\s*\[\s*\])*)\s+"
    r"(?P<name>[A-Za-z_]\w*)\s*\((?P<params>[^)]*)\))\s*\{\s*$")

CTOR_RE = re.compile(r"^    public GameCanvas\(\)\s*\{")


def find_method_end(lines, start):
    """Return the line index (0-based, inclusive) of the closing brace."""
    depth = 0
    in_str = None
    for i in range(start, len(lines)):
        for ch in lines[i]:
            if in_str:
                if ch == in_str and not None:  # naive; decompiled code has no escaped quotes in braces
                    in_str = None
                continue
            if ch in "\"'":
                in_str = ch
            elif ch == "{":
                depth += 1
            elif ch == "}":
                depth -= 1
                if depth == 0:
                    return i
    return len(lines) - 1


def parse_fields(lines):
    """Fields live between the class '{' and the constructor."""
    start = None
    end = None
    for i, l in enumerate(lines):
        if start is None and re.search(r"implements Runnable \{", l):
            start = i + 1
        if CTOR_RE.match(l):
            end = i
            break
    fields = []
    for i in range(start, end):
        l = lines[i]
        if not l.strip() or l.strip().startswith("//"):
            continue
        m = FIELD_RE.match(l)
        if not m:
            continue
        mods = m.group("mods") or ""
        fields.append({
            "java_name": m.group("name"),
            "java_type": re.sub(r"\s+", "", m.group("type")),
            "init": (m.group("init") or "").strip(),
            "static": "static" in mods,
            "final": "final" in mods,
            "line": i + 1,
        })
    return fields


def parse_methods(lines):
    methods = []
    for i, l in enumerate(lines):
        m = METHOD_RE.match(l)
        if not m:
            continue
        if m.group("name") == "GameCanvas":
            continue  # constructor is folded into gc_fields_init
        end = find_method_end(lines, i)
        methods.append({
            "java_name": m.group("name"),
            "ret": re.sub(r"\s+", "", m.group("ret")),
            "params": m.group("params").strip(),
            "sig": m.group("sig").strip(),
            "start": i + 1,
            "end": end + 1,
        })
    return methods


# --- Manifest -----------------------------------------------------------------
def load_manifest():
    if os.path.exists(MANIFEST):
        with open(MANIFEST, "r", encoding="utf-8") as f:
            return json.load(f)
    return {"fields": [], "methods": []}


def save_manifest(man):
    with open(MANIFEST, "w", encoding="utf-8") as f:
        json.dump(man, f, indent=1)
    print(f"wrote {MANIFEST}")


def c_method_name(name, seen):
    """Stable, unique C name for a (possibly overloaded) Java method."""
    base = "gc_" + name
    if base not in seen:
        seen[base] = 0
        return base
    seen[base] += 1
    return f"{base}__{seen[base]}"


def cmd_inventory():
    lines = read_java()
    fields = parse_fields(lines)
    methods = parse_methods(lines)
    man = load_manifest()
    old_m = {(x["java_name"], x.get("params", "")): x for x in man.get("methods", [])}
    old_f = {x["java_name"]: x for x in man.get("fields", [])}

    seen = {}
    new_methods = []
    for m in methods:
        key = (m["java_name"], m["params"])
        prev = old_m.get(key, {})
        m["c_name"] = prev.get("c_name") or c_method_name(m["java_name"], seen)
        # keep the counter in sync for names that already exist
        if m["c_name"] in seen:
            pass
        else:
            seen[m["c_name"]] = 0
        m["status"] = prev.get("status", "pending")
        new_methods.append(m)

    new_fields = []
    used_names = {}
    for f in fields:
        prev = old_f.get(f["java_name"], {})
        cn = prev.get("c_name") or f["java_name"]
        # Disambiguate duplicate field names (a mappings.txt collision in the
        # readable Java, e.g. two `buildMenuIndex` fields). Keep the first; the
        # later one gets a suffix so the C struct is well-formed. FLAGGED.
        if cn in used_names:
            used_names[cn] += 1
            print(f"WARNING: duplicate field name '{f['java_name']}' "
                  f"(L{f['line']}) -> renamed c '{cn}_{used_names[cn]}' "
                  f"[mappings.txt collision]")
            cn = f"{cn}_{used_names[cn]}"
            f["dup_of"] = f["java_name"]
        else:
            used_names[cn] = 1
        f["c_name"] = cn
        f["c_type"] = prev.get("c_type_override") or c_type(f["java_type"])
        if prev.get("c_type_override"):
            f["c_type_override"] = prev["c_type_override"]
        new_fields.append(f)

    man["fields"] = new_fields
    man["methods"] = new_methods
    save_manifest(man)
    print(f"inventory: {len(new_fields)} fields, {len(new_methods)} methods")


DONE = ("ported", "partial")  # statuses the generator does NOT stub


def cmd_status():
    man = load_manifest()
    methods = man.get("methods", [])
    ported = [m for m in methods if m["status"] == "ported"]
    partial = [m for m in methods if m["status"] == "partial"]
    done = len(ported) + len(partial)
    print(f"methods done: {done}/{len(methods)} "
          f"({100*done//max(1,len(methods))}%)  "
          f"[{len(ported)} full, {len(partial)} partial]")
    print(f"fields (all generated): {len(man.get('fields', []))}")
    if partial:
        print("partial (finish these):")
        for m in partial:
            print(f"  {m['c_name']:<28} {m.get('note','')}")
    pend = [m for m in methods if m["status"] not in DONE]
    if "-v" in sys.argv:
        print("pending:")
        for m in pend:
            print(f"  {m['c_name']:<28} {m['java_name']}({m['params']})  "
                  f"L{m['start']}-{m['end']}")


def cmd_show():
    name = sys.argv[2]
    man = load_manifest()
    lines = read_java()
    hits = [m for m in man["methods"] if m["java_name"] == name or m["c_name"] == name]
    if not hits:
        print(f"no method named {name}")
        return
    for m in hits:
        print(f"// ==== {m['java_name']}({m['params']}) -> {m['c_name']}  "
              f"[{m['status']}]  L{m['start']}-{m['end']} ====")
        print("\n".join(lines[m["start"]-1:m["end"]]))
        print()


def cmd_mark():
    name = sys.argv[2]
    status = sys.argv[3] if len(sys.argv) > 3 else "ported"
    note = sys.argv[4] if len(sys.argv) > 4 else None
    man = load_manifest()
    n = 0
    for m in man["methods"]:
        if m["java_name"] == name or m["c_name"] == name:
            m["status"] = status
            if note is not None:
                m["note"] = note
            n += 1
    save_manifest(man)
    print(f"marked {n} method(s) '{name}' -> {status}")


def cmd_next():
    """Suggest small, likely-leaf pending methods (fewest source lines first)."""
    man = load_manifest()
    pend = [m for m in man["methods"] if m["status"] != "ported"]
    pend.sort(key=lambda m: m["end"] - m["start"])
    print("next candidates (smallest first):")
    for m in pend[:15]:
        print(f"  {m['end']-m['start']:>4}L  {m['c_name']:<26} "
              f"{m['java_name']}({m['params']})")


# --- Generation ---------------------------------------------------------------
def cmd_gen():
    man = load_manifest()
    os.makedirs(GEN_DIR, exist_ok=True)
    gen_state_header(man)
    gen_state_source(man)
    gen_methods_header(man)
    gen_stubs(man)
    print("generated port/generated/{game_state.h,game_state.c,"
          "game_methods.h,game_stubs.c}")


def gen_state_header(man):
    inst = [f for f in man["fields"] if not f["static"]]
    stat = [f for f in man["fields"] if f["static"]]
    lines = [
        "// GENERATED by tools/port.py - do not edit by hand.",
        "// The GameCanvas struct (instance fields) + extern static fields.",
        "#ifndef GAME_STATE_H",
        "#define GAME_STATE_H",
        '#include "midp.h"',
        "",
        "typedef struct GameCanvas {",
    ]
    for f in inst:
        lines.append(f"    {f['c_type']} {f['c_name']}; "
                     f"// {f['java_type']} {f['java_name']}")
    lines += ["} GameCanvas;", ""]
    for f in stat:
        lines.append(f"extern {f['c_type']} gcs_{f['c_name']}; "
                     f"// static {f['java_type']} {f['java_name']}")
    lines += ["", "void gc_fields_init(GameCanvas *gc);", "", "#endif"]
    _write(os.path.join(GEN_DIR, "game_state.h"), "\n".join(lines))


def gen_state_source(man):
    inst = [f for f in man["fields"] if not f["static"]]
    stat = [f for f in man["fields"] if f["static"]]
    static_names = {f["java_name"] for f in stat}
    lines = [
        "// GENERATED by tools/port.py - do not edit by hand.",
        '#include "game_state.h"',
        "#include <stdlib.h>",
        "#include <string.h>",
        "",
    ]
    for f in stat:
        lines.append(f"{f['c_type']} gcs_{f['c_name']};")
    lines += ["", "void gc_fields_init(GameCanvas *gc) {",
              "    memset(gc, 0, sizeof(*gc));",
              "    // --- constructor finals (GameCanvas()) ---",
              "    gc->at = 3; gc->cB = 33; gc->ar = 20; gc->D = 240;",
              "    // --- static fields (declaration order) ---"]
    for f in stat:
        lines += ["    " + s for s in init_statements(f, static_names)]
    lines.append("    // --- instance fields (declaration order) ---")
    for f in inst:
        lines += ["    " + s for s in init_statements(f, static_names)]
    lines.append("}")
    _write(os.path.join(GEN_DIR, "game_state.c"), "\n".join(lines))


NUM_RE = re.compile(r"^-?\d+$")


def _lhs(f):
    return f"gcs_{f['c_name']}" if f["static"] else f"gc->{f['c_name']}"


def _base_ctype(java_type):
    base = java_type.replace("[]", "")
    return PRIM.get(base) or REF.get(base) or "void*"


def _split_top_rows(inner):
    """Split '{a,b}, {c,d}' into ['{a,b}','{c,d}'], dropping depth-0 separators."""
    rows, depth, cur = [], 0, ""
    for ch in inner:
        if ch == "{":
            if depth == 0:
                cur = ""  # start a fresh group, discard separators before it
            depth += 1
            cur += ch
        elif ch == "}":
            depth -= 1
            cur += ch
            if depth == 0:
                rows.append(cur)
        else:
            if depth > 0:
                cur += ch
    return rows


def _translate_expr(expr, static_names):
    expr = expr.replace("this.", "gc->")
    # bare static field identifiers -> gcs_
    for nm in static_names:
        expr = re.sub(r"(?<![\w>])" + re.escape(nm) + r"\b", "gcs_" + nm, expr)
    return expr


def init_statements(f, static_names):
    """Return a list of C statements initializing field f, faithfully."""
    name, t, init = f["c_name"], f["java_type"], f["init"]
    lhs = _lhs(f)
    if not init:
        return []
    if init == "null":
        return [f"{lhs} = NULL;"]
    # Special cases (references to the shim).
    if name == "rng":
        return [f"midp_random_init(&{lhs}, midp_current_time_millis());"]
    if name == "bT":
        return [f"{lhs} = midp_current_time_millis();"]
    if name == "backbuffer":
        return [f"{lhs} = midp_image_create_blank(gcs_screenWidth, gcs_hs);"]
    if name == "backbufferG":
        return [f"{lhs} = midp_image_get_graphics(gc->backbuffer);"]
    # Literals.
    if init in ("true", "false") or NUM_RE.match(init) or re.match(r"^0x[0-9A-Fa-f]+$", init):
        return [f"{lhs} = {init};"]
    if init.endswith("L") and NUM_RE.match(init[:-1]):
        return [f"{lhs} = {init[:-1]};"]
    m = re.match(r"^\(byte\)(-?\d+)$", init)
    if m:
        return [f"{lhs} = {m.group(1)};"]
    base = _base_ctype(t)
    # new T[N][M] 2D fixed
    m = re.match(r"^new\s+\w+\s*\[\s*(\d+)\s*\]\s*\[\s*(\d+)\s*\]$", init)
    if m and t.count("[]") == 2:
        n, mm = m.group(1), m.group(2)
        return [f"{lhs} = ({base}**)midp_alloc_arr(sizeof({base}*), {n});",
                f"for (int _i = 0; _i < {n}; ++_i) {lhs}[_i] = ({base}*)midp_alloc_arr(sizeof({base}), {mm});"]
    # new T[N] 1D fixed
    m = re.match(r"^new\s+\w+\s*\[\s*(\d+)\s*\]$", init)
    if m and t.count("[]") == 1:
        return [f"{lhs} = ({base}*)midp_alloc_arr(sizeof({base}), {m.group(1)});"]
    # new T[]{ literal } 1D
    m = re.match(r"^new\s+\w+\s*\[\s*\]\s*\{(.*)\}$", init, re.S)
    if m and t.count("[]") == 1:
        vals = m.group(1).strip()
        d = f"{f['c_name']}__d"
        return [f"static const {base} {d}[] = {{{vals}}};",
                f"{lhs} = ({base}*)midp_alloc_arr(sizeof({base}), (int)(sizeof({d})/sizeof({base})));",
                f"memcpy({lhs}, {d}, sizeof({d}));"]
    # new T[][]{ {..},{..} } 2D jagged literal
    m = re.match(r"^new\s+\w+\s*\[\s*\]\s*\[\s*\]\s*\{(.*)\}$", init, re.S)
    if m and t.count("[]") == 2:
        rows = _split_top_rows(m.group(1).strip())
        out = []
        for ri, row in enumerate(rows):
            vals = row.strip()[1:-1]  # strip { }
            out.append(f"static const {base} {f['c_name']}__r{ri}[] = {{{vals}}};")
        out.append(f"{lhs} = ({base}**)midp_alloc_arr(sizeof({base}*), {len(rows)});")
        for ri in range(len(rows)):
            d = f"{f['c_name']}__r{ri}"
            out.append(f"{lhs}[{ri}] = ({base}*)midp_alloc_arr(sizeof({base}), (int)(sizeof({d})/sizeof({base}))); "
                       f"memcpy({lhs}[{ri}], {d}, sizeof({d}));")
        return out
    # new Image[N]
    m = re.match(r"^new\s+Image\s*\[\s*(\d+)\s*\]$", init)
    if m:
        return [f"{lhs} = (MidpImage**)midp_alloc_arr(sizeof(MidpImage*), {m.group(1)});"]
    # arithmetic expression referencing other fields
    if re.match(r"^[\w\s.\-+*/()]+$", init):
        return [f"{lhs} = {_translate_expr(init, static_names)};"]
    return [f"/* TODO init: {lhs} = {init}; */"]


def gen_methods_header(man):
    lines = [
        "// GENERATED by tools/port.py - do not edit by hand.",
        "// Prototypes for every ported/stubbed GameCanvas method.",
        "#ifndef GAME_METHODS_H",
        "#define GAME_METHODS_H",
        '#include "game_state.h"',
        "",
    ]
    for m in man["methods"]:
        lines.append(f"{c_proto(m)}; "
                     f"// {m['java_name']}({m['params']}) [{m['status']}]")
    lines += ["", "#endif"]
    _write(os.path.join(GEN_DIR, "game_methods.h"), "\n".join(lines))


def gen_stubs(man):
    lines = [
        "// GENERATED by tools/port.py - do not edit by hand.",
        "// Stub bodies for methods not yet ported (status != 'ported').",
        '#include "game_methods.h"',
        "",
    ]
    for m in man["methods"]:
        if m["status"] in ("ported", "partial"):
            continue
        ret = c_ret(m["ret"])
        body = "" if ret == "void" else ("    return 0;" if ret != "bool" else "    return false;")
        lines.append(c_proto(m) + " {")
        lines.append(f"    (void)gc; // TODO port: {m['java_name']}"
                     f"({m['params']})  L{m['start']}-{m['end']}")
        if body:
            lines.append(body)
        lines.append("}")
        lines.append("")
    _write(os.path.join(GEN_DIR, "game_stubs.c"), "\n".join(lines))


def c_ret(java_ret):
    dims = java_ret.count("[]")
    base = java_ret.replace("[]", "")
    if base in PRIM:
        return PRIM[base] + "*" * dims
    if base in REF:
        return REF[base] + "*" * dims
    return "void*" if dims else "void*"


def c_proto(m):
    """C prototype: gc_name(GameCanvas *gc, <translated params>)."""
    ret = c_ret(m["ret"])
    params = ["GameCanvas *gc"]
    if m["params"]:
        for i, p in enumerate(split_params(m["params"])):
            pt, pn = p
            params.append(f"{c_param_type(pt)} {pn or ('a'+str(i))}")
    return f"{ret} {m['c_name']}({', '.join(params)})"


def split_params(params):
    out = []
    for part in params.split(","):
        part = part.strip()
        if not part:
            continue
        toks = part.rsplit(" ", 1)
        if len(toks) == 2:
            out.append((re.sub(r"\s+", "", toks[0]), toks[1]))
        else:
            out.append((re.sub(r"\s+", "", toks[0]), ""))
    return out


def c_param_type(pt):
    dims = pt.count("[]")
    base = pt.replace("[]", "")
    if base in PRIM:
        return PRIM[base] + "*" * dims
    if base in REF:
        return REF[base] + "*" * dims
    return "void*"


def _write(path, text):
    with open(path, "w", encoding="utf-8") as f:
        f.write(text + "\n")


# --- Build / verify -----------------------------------------------------------
def cmd_build():
    if not os.path.exists(os.path.join(BUILD_DIR, "CMakeCache.txt")):
        subprocess.run(["cmake", "-S", ROOT, "-B", BUILD_DIR,
                        "-G", "Visual Studio 17 2022", "-A", "x64"], check=True)
    r = subprocess.run(["cmake", "--build", BUILD_DIR, "--config", "Debug"])
    sys.exit(r.returncode)


def cmd_verify():
    subprocess.run(["cmake", "--build", BUILD_DIR, "--config", "Debug"], check=True)
    exe = os.path.join(BUILD_DIR, "Debug", "train_tycoon.exe")
    # Headless smoke: run a few frames then close. TRAIN_TYCOON_SMOKE limits frames.
    env = dict(os.environ, TRAIN_TYCOON_SMOKE="120")
    r = subprocess.run([exe], env=env, timeout=30)
    print(f"verify: exit {r.returncode}")
    sys.exit(r.returncode)


CMDS = {"inventory": cmd_inventory, "status": cmd_status, "show": cmd_show,
        "mark": cmd_mark, "next": cmd_next, "gen": cmd_gen,
        "build": cmd_build, "verify": cmd_verify}

if __name__ == "__main__":
    if len(sys.argv) < 2 or sys.argv[1] not in CMDS:
        print(__doc__)
        print("commands:", ", ".join(CMDS))
        sys.exit(1)
    CMDS[sys.argv[1]]()
