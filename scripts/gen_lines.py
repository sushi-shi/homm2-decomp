#!/usr/bin/env python3
"""gen_lines.py <unit> - per-statement source<->code map for the nvim base overlay.

Compiles one TU's source with the matching flags PLUS /Z7 (CodeView debug; proven
codegen-NEUTRAL - the .text is byte-identical to the matching `base` obj, so the
line offsets line up 1:1 with objdiff's base-side instruction addresses), then reads
the COFF .text line-number table (IMAGE_LINENUMBER) it emits.

MSVC stores those line numbers RELATIVE to each function's first source line, and
only for code-generating lines. We resolve them to absolute lines by locating each
function's definition in the source (Class::method / free name parsed off the
mangled symbol), then bake the source text in.

Output: build/lines/<unit>.json
    { "<mangled fn>": [[code_offset, source_line, "stripped source text"], ...] }
`code_offset` is the .text section offset == objdiff base-side instruction address,
so the Lua overlay matches a statement to the asm line whose address equals it.
"""
import json, os, re, struct, subprocess, sys, tomllib
from pathlib import Path

REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[1]))


def find_def_line(src_lines, mangled):
    """Source line of a function's BODY opening brace `{` - the base MSVC's
    per-function-relative line numbers count from (abs = brace_line + rel; rel 1 is
    the first body line). Located from the mangled name: `?Name@Class@@...` ->
    grep `Class::Name`; `?Name@@YA...` -> grep `Name(`; then the next `{`."""
    m = re.match(r"\?([A-Za-z0-9_]+)@([A-Za-z0-9_]+)@@", mangled)   # method
    needle = f"{m.group(2)}::{m.group(1)}" if m else None
    if not needle:
        m = re.match(r"\?([A-Za-z0-9_]+)@@", mangled)               # free function
        needle = m.group(1) + "(" if m else None
    if not needle:
        return None
    for n, line in enumerate(src_lines, 1):
        if needle in line and not line.lstrip().startswith(("//", "*", "VA(")):
            for b in range(n, min(n + 6, len(src_lines)) + 1):      # body brace: this line or just below
                if "{" in src_lines[b - 1]:
                    return b
            return n
    return None


def parse_obj(obj):
    """{mangled -> (section_base, [(rel_line, code_offset), ...])} from a /Z7 COFF."""
    d = obj.read_bytes()
    nsec = struct.unpack_from("<H", d, 2)[0]
    psym, nsym = struct.unpack_from("<II", d, 8)
    strtab = psym + nsym * 18

    def sname(o):
        nm = d[o:o + 8]
        if nm[:4] == b"\0\0\0\0":
            s = strtab + struct.unpack_from("<I", nm, 4)[0]
            return d[s:d.index(b"\0", s)].decode("latin1")
        return nm.rstrip(b"\0").decode("latin1")

    syms = {}
    i = 0
    while i < nsym:
        o = psym + i * 18
        syms[i] = (sname(o), struct.unpack_from("<I", d, o + 8)[0])
        i += 1 + d[o + 17]
    plno = nlno = 0
    for s in range(nsec):
        o = 20 + s * 40
        if d[o:o + 5] == b".text":
            plno = struct.unpack_from("<I", d, o + 28)[0]
            nlno = struct.unpack_from("<H", d, o + 34)[0]
    out = {}
    cur = None
    for k in range(nlno):
        typ, line = struct.unpack_from("<IH", d, plno + k * 6)
        if line == 0:
            cur = syms.get(typ)
            if cur:
                out.setdefault(cur[0], (cur[1], []))
        elif cur and cur[0] in out:
            out[cur[0]][1].append((line, typ))
    return out


def parse_locals(obj):
    """{qualified name -> {bp_offset -> local/param name}} from the /Z7 .debug$S
    S_BPREL32 records (grouped by their enclosing S_GPROC32)."""
    d = obj.read_bytes()
    nsec = struct.unpack_from("<H", d, 2)[0]
    out = {}
    cur = None
    for s in range(nsec):
        o = 20 + s * 40
        if d[o:o + 8] != b".debug$S":
            continue
        praw = struct.unpack_from("<I", d, o + 20)[0]
        size = struct.unpack_from("<I", d, o + 16)[0]
        b = d[praw:praw + size]
        p = 4                                          # skip the CV4 signature
        while p + 4 <= len(b):
            rl, rt = struct.unpack_from("<HH", b, p)
            if rl == 0:
                break
            body = b[p + 4:p + 2 + rl]
            if rt in (0x0204, 0x0205) and len(body) > 34:   # L/GPROC32: name @ body[34]
                nl = body[33]
                cur = body[34:34 + nl].decode("latin1")
                out.setdefault(cur, {})
            elif rt == 0x0200 and cur is not None:           # S_BPREL32: i32 off, u16 ty, name
                off = struct.unpack_from("<i", body, 0)[0]
                nl = body[6]
                out[cur][off] = body[7:7 + nl].decode("latin1")
            elif rt == 0x0006:                               # S_END
                cur = None
            p += 2 + rl
    return out


def mangled_to_qual(m):
    """`?Name@Class@@...` -> `Class::Name`; `?Name@@YA...` -> `Name`."""
    x = re.match(r"\?([A-Za-z0-9_]+)@([A-Za-z0-9_]+)@@", m)
    if x:
        return f"{x.group(2)}::{x.group(1)}"
    x = re.match(r"\?([A-Za-z0-9_]+)@@", m)
    return x.group(1) if x else None


def main(argv):
    unit = argv[0]
    man = tomllib.loads((REPO / "config/units.toml").read_text())
    u = next((x for x in man.get("unit", []) if x["unit"] == unit), None)
    if not u:
        sys.exit(f"gen_lines: unknown unit {unit}")
    flags = man.get("flags", {}).get(u.get("flags", "base"), [])
    src = REPO / u["source"]
    obj = REPO / "build/lines" / f"{unit}.z7.obj"
    obj.parent.mkdir(parents=True, exist_ok=True)
    rc = subprocess.run(["python3", "-m", "homm2.build.cc_wrap", "--out", str(obj),
                         "--src", str(src), "--", *flags, "/Z7"],
                        cwd=REPO, env={**os.environ, "PYTHONPATH": str(REPO / "scripts")}).returncode
    if rc or not obj.exists():
        sys.exit(f"gen_lines: /Z7 compile failed for {unit}")
    src_lines = src.read_text(errors="replace").splitlines()
    fns = parse_obj(obj)
    loc_by_qual = parse_locals(obj)
    result = {"stmts": {}, "locals": {}}
    for mangled, (base, recs) in fns.items():
        dl = find_def_line(src_lines, mangled)
        if dl is not None:
            rows = []
            for rel, off in recs:
                ab = dl + rel
                line = src_lines[ab - 1] if 0 < ab <= len(src_lines) else ""
                if line.strip():
                    rows.append([off, ab, line])
            if rows:
                # dedent by the function's OUTERMOST statement indent: the body sits
                # at column 0 while RELATIVE C++ nesting (scope) is preserved.
                ind = min(len(r[2]) - len(r[2].lstrip()) for r in rows)
                for r in rows:
                    r[2] = r[2][ind:].rstrip()
                result["stmts"][mangled] = rows
        loc = loc_by_qual.get(mangled_to_qual(mangled) or "")
        if loc:                                          # [[bp_offset, name], ...]
            result["locals"][mangled] = [[o, n] for o, n in sorted(loc.items())]
    outp = REPO / "build/lines" / f"{unit}.json"
    outp.write_text(json.dumps(result) + "\n")
    print(f"gen_lines: {unit} -> {outp.relative_to(REPO)} "
          f"({len(result['stmts'])} fns, "
          f"{sum(len(v) for v in result['stmts'].values())} statements, "
          f"{sum(len(v) for v in result['locals'].values())} locals)")


if __name__ == "__main__":
    main(sys.argv[1:])
