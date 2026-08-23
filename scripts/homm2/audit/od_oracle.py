#!/usr/bin/env python3
"""Ground-truth oracle for the /Od local stack-slot model (homm2/core/od_slots.py).

Compiles a one-function probe with the real pinned MSVC toolchain and reads each
local's ebp-relative frame offset straight from the S_BPREL32 debug records the
compiler emits with /Z7. /Z7 adds debug records but does NOT change /Od code
generation, so the slot assignment is byte-identical to the real `/Od /MT /Gr`
build. Use this to verify any prediction from homm2.core.od_slots.

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


S_END, S_BPREL32, S_LPROC32, S_GPROC32 = 0x0006, 0x1006, 0x100A, 0x100B
S_BLOCK32 = 0x0207


def _symbol_name(obj: bytes, entry: int, stroff: int) -> str:
    raw = obj[entry:entry + 8]
    if raw[:4] == b"\0\0\0\0":
        rel = struct.unpack_from("<I", raw, 4)[0]
        return obj[stroff + rel:].split(b"\0", 1)[0].decode("latin1")
    return raw.rstrip(b"\0").decode("latin1")


def _debug_section(obj: bytes):
    """(.debug$S payload, {section_offset: mangled_symbol}) or (b"", {})."""
    nsec = struct.unpack_from("<H", obj, 2)[0]
    symoff, nsym = struct.unpack_from("<II", obj, 8)
    stroff = symoff + nsym * 18
    for i in range(nsec):
        o = 20 + i * 40
        name = obj[o:o + 8]
        if name[:1] == b"/":
            name = obj[stroff + int(name[1:].rstrip(b"\0")):].split(b"\0", 1)[0]
        else:
            name = name.rstrip(b"\0")
        if name != b".debug$S":
            continue
        size, ptr, relptr = struct.unpack_from("<III", obj, o + 16)
        nreloc = struct.unpack_from("<H", obj, o + 32)[0]
        relocs = {}
        for r in range(nreloc):
            off, idx, _kind = struct.unpack_from("<IIH", obj, relptr + r * 10)
            relocs[off] = _symbol_name(obj, symoff + idx * 18, stroff)
        return obj[ptr:ptr + size], relocs
    return b"", {}


def _records(blob: bytes):
    """Yield (section_offset, rectyp, body) for a CV-signed .debug$S payload."""
    if len(blob) < 4 or struct.unpack_from("<I", blob, 0)[0] not in (1, 2):
        return
    p = 4
    while p + 4 <= len(blob):
        rl, rt = struct.unpack_from("<HH", blob, p)
        if rl < 2:
            break
        yield p, rt, blob[p + 4:p + 2 + rl]
        p += 2 + rl


def parse_frames(obj: bytes):
    """{mangled_name: [(local, ebp_offset), ...]} per S_GPROC32/S_LPROC32.

    The procedure record's CodeView name is `class::method`; the join to the
    mangled world goes through the relocation on the record's offset field
    (body offset 28). Procedures without that relocation are skipped. A `{ }`
    block emits S_BLOCK32 with its own S_END, so procedure ends are tracked by
    depth, not by the first S_END.
    """
    blob, relocs = _debug_section(obj)
    frames, current, depth = {}, None, 0
    for pos, rt, body in _records(blob):
        if rt in (S_GPROC32, S_LPROC32):
            current, depth = relocs.get(pos + 4 + 28), 1
            if current is not None:
                frames.setdefault(current, [])
        elif rt == S_BLOCK32 and depth:
            depth += 1
        elif rt == S_END and depth:
            depth -= 1
            if depth == 0:
                current = None
        elif rt == S_BPREL32 and current is not None and len(body) >= 9:
            off = struct.unpack_from("<i", body, 0)[0]
            nl = body[8]
            frames[current].append((body[9:9 + nl].decode("latin1"), off))
    return frames


def parse_bprel(obj: bytes):
    """{name: frame_offset} for every S_BPREL32 record, across all procedures."""
    out = {}
    blob, _relocs = _debug_section(obj)
    for _pos, rt, body in _records(blob):
        if rt == S_BPREL32 and len(body) >= 9:
            off = struct.unpack_from("<i", body, 0)[0]
            nl = body[8]
            out[body[9:9 + nl].decode("latin1")] = off
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
    name2off = parse_bprel(obj.read_bytes())
    order = sorted(((n, name2off[n]) for n in names if n in name2off), key=lambda t: -t[1])
    _cache[key] = order
    return order


if __name__ == "__main__":
    for i, (n, off) in enumerate(slots(sys.argv[1:] or ["alpha", "bravo", "charlie"])):
        print(f"  slot{i}  ebp{off:+#06x}  {n}")
