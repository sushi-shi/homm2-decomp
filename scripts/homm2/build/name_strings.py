#!/usr/bin/env python3
# name_strings.py - give string-literal relocation targets their canonical name.
#
# Reloc-manifest targets enter build/gen/symbol_names.csv as const_<RVA> rows
# (unit "_const"). The comparison layer canonicalizes anonymous string data in
# our compiled objects to $anon_str_<sha256(bytes+NUL)>_<occurrence>, so the
# delinked target pairs by name when its string constants carry the same
# spelling. Cells whose payload reads as an in-image pointer (vtables, pointer
# tables) are NOT strings and keep their const_<RVA> name until a reviewed
# claim covers them.
#
# Rewrites build/gen/symbol_names.csv in place. Run by `homm2 redelink`.
import hashlib
import struct
from pathlib import Path

from homm2.build.canonicalize_relocs import CoffFile
from homm2.build.data_manifest_adapter import candidate_topology, _string_payload
from homm2.core.paths import REPO

EXE = REPO / "build/orig/HMM2PL.exe"
CSV = REPO / "build/gen/symbol_names.csv"
IMAGE_BASE = 0x400000
BASE_OBJS = REPO / "build/objdiff/base"


def candidate_string_payloads(base_root=BASE_OBJS):
    """Literal payloads current candidate objects prove were compiler strings."""
    root = Path(base_root)
    payloads = set()
    for path in sorted(root.rglob("*.obj")):
        unit = path.relative_to(root).with_suffix("").as_posix()
        definitions, _sections = candidate_topology(path, unit)
        coff = CoffFile(path)
        for candidate in definitions:
            if not candidate.symbol.startswith(("??_C@", "$SG")):
                continue
            payload = _string_payload(coff, candidate)
            if payload is not None:
                payloads.add(payload)
    return payloads


def sections(d):
    e = struct.unpack_from("<I", d, 0x3c)[0]
    n = struct.unpack_from("<H", d, e + 6)[0]
    osz = struct.unpack_from("<H", d, e + 20)[0]
    out = []
    for i in range(n):
        o = e + 24 + osz + i * 40
        nm = d[o:o + 8].rstrip(b"\0").decode("latin1")
        vs, va = struct.unpack_from("<II", d, o + 8)
        pr = struct.unpack_from("<I", d, o + 20)[0]
        out.append((nm, va, va + vs, pr))
    return out


def va2off(secs, rva):
    for nm, va, en, pr in secs:
        if va <= rva < en:
            return pr + (rva - va)
    return None


def image_end(d):
    e = struct.unpack_from("<I", d, 0x3c)[0]
    return struct.unpack_from("<I", d, e + 0x50)[0]


def read_str(d, secs, rva):
    o = va2off(secs, rva)
    if o is None:
        return None
    b = d[o:o + 512]
    z = b.find(b"\0")
    if 1 <= z and all(c >= 9 for c in b[:z]):
        return b[:z]
    return None


def looks_like_pointer(d, secs, rva, end):
    o = va2off(secs, rva)
    if o is None or o + 4 > len(d):
        return False
    value = struct.unpack_from("<I", d, o)[0]
    return IMAGE_BASE <= value < IMAGE_BASE + end


def main():
    d = EXE.read_bytes()
    secs = sections(d)
    end = image_end(d)
    # A short string can numerically resemble an image pointer (for example
    # "ATA"). Current candidate COFF literal payloads provide the type proof;
    # no generated donation state participates.
    candidate_strings = candidate_string_payloads()
    rows = [ln.rstrip("\n") for ln in open(CSV)]
    hdr, body = rows[0], rows[1:]

    named = 0
    pointers = 0
    occurrences = {}
    # occurrence suffixes follow ascending RVA so duplicate literals stay
    # deterministic: first copy _0, next _1, ...
    const_rows = []
    for i, ln in enumerate(body):
        f = ln.split(",")
        if len(f) < 5 or f[2] != "_const" or not f[1].startswith("const_"):
            continue
        const_rows.append((int(f[0], 16), i))
    for rva, i in sorted(const_rows):
        s = read_str(d, secs, rva)
        if (looks_like_pointer(d, secs, rva, end)
                and (s is None or s + b"\0" not in candidate_strings)):
            pointers += 1
            continue
        if s is None:
            continue
        digest = hashlib.sha256(s + b"\0").hexdigest()
        occurrence = occurrences.get(digest, 0)
        occurrences[digest] = occurrence + 1
        f = body[i].split(",")
        f[1] = f"$anon_str_{digest}_{occurrence}"
        body[i] = ",".join(f)
        named += 1
    CSV.write_text(hdr + "\n" + "\n".join(body) + "\n")
    print(f"[name_strings] {len(const_rows)} manifest constants: "
          f"{named} strings named canonically, {pointers} pointer cells kept const_")


if __name__ == "__main__":
    main()
