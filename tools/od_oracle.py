#!/usr/bin/env python3
"""Ground-truth oracle for the /Od local stack-slot model (see docs/od-stack-layout.md).

Compiles a one-function probe with the real MSVC 4.2 toolchain and reads each
local's ebp-relative frame offset straight from the CodeView S_BPREL32 records the
compiler emits with /Z7. /Z7 adds debug records but does NOT change /Od code
generation, so the slot assignment is byte-identical to the real `/Od /MT /Gr`
build. Use this to verify any prediction from tools/od_slots.py.

  slots(["foo","bar","baz"]) -> [("foo",-4),("bar",-8),...]   # iteration order (-4 first)

Run inside `nix develop .#build` (needs wine + MSVC_DIR + WINEPREFIX).
CLI:  od_oracle.py a b c d ...        # print the real frame slot order
"""
import os, struct, subprocess, sys, hashlib
from pathlib import Path

MSVC = Path(os.environ["MSVC_DIR"]); CL = MSVC / "bin" / "CL.EXE"
WORK = Path(os.environ.get("OD_ORACLE_WORK", "/tmp/od_oracle")); WORK.mkdir(parents=True, exist_ok=True)
_cache = {}


def _emit(names):
    # parameterless fn (no fastcall ECX spill to pollute the table); chain-add so
    # every local is live and gets its own slot under /Od.
    lines, prev = ["int probe(){"], "1"
    for n in names:
        lines.append(f"  int {n}={prev}+1;"); prev = n
    lines.append(f"  return {prev};\n}}")
    return "\n".join(lines) + "\n"


def _parse_bprel(obj: bytes):
    """{name: frame_offset} for every S_BPREL32 (rectyp 0x0200) record in .debug$S."""
    nsec = struct.unpack_from("<H", obj, 2)[0]
    symoff, nsym = struct.unpack_from("<II", obj, 8)
    stroff = symoff + nsym * 18
    blob = b""
    for i in range(nsec):
        o = 20 + i * 40
        name = obj[o:o + 8]
        if name[:1] == b"/":
            name = obj[stroff + int(name[1:].rstrip(b"\0")):].split(b"\0", 1)[0]
        else:
            name = name.rstrip(b"\0")
        size, ptr = struct.unpack_from("<II", obj, o + 16)
        if name == b".debug$S":
            blob = obj[ptr:ptr + size]; break
    if not blob:
        return {}
    out, p = {}, (4 if struct.unpack_from("<I", blob, 0)[0] in (1, 2) else 0)
    while p + 4 <= len(blob):
        rl, rt = struct.unpack_from("<HH", blob, p)
        if rl < 2:
            break
        body = blob[p + 4:p + 2 + rl]
        if rt == 0x0200 and len(body) >= 7:           # S_BPREL32: off(i32) typ(u16) name(pstr)
            off, _typ = struct.unpack_from("<iH", body, 0)
            nl = body[6]; out[body[7:7 + nl].decode("latin1")] = off
        p += 2 + rl
    return out


def slots(names):
    key = tuple(names)
    if key in _cache:
        return _cache[key]
    tag = "p" + hashlib.md5("\0".join(names).encode()).hexdigest()[:12]
    d = WORK / tag; d.mkdir(exist_ok=True)
    (d / "probe.cpp").write_text(_emit(names))
    r = subprocess.run(["wine", str(CL), "/nologo", "/c", "/Od", "/MT", "/Gr", "/Z7", "probe.cpp"],
                       cwd=d, env=dict(os.environ, WINEDEBUG="-all"), capture_output=True, text=True)
    obj = d / "probe.obj"
    if not obj.exists():
        raise RuntimeError(f"cl failed for {names}:\n{r.stdout}\n{r.stderr}")
    name2off = _parse_bprel(obj.read_bytes())
    order = sorted(((n, name2off[n]) for n in names if n in name2off), key=lambda t: -t[1])
    _cache[key] = order
    return order


if __name__ == "__main__":
    for i, (n, off) in enumerate(slots(sys.argv[1:] or ["alpha", "bravo", "charlie"])):
        print(f"  slot{i}  ebp{off:+#06x}  {n}")
