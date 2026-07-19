#!/usr/bin/env python3
"""Archived COFF metric helper for TU-state experiments.

This deliberately hashes raw ``.text`` bytes and the ordered relocation target
stream separately.  It handles both one-COMDAT-per-function `/O2` objects and
shared-section `/Od` objects without relying on disassembler formatting.
"""

from __future__ import annotations

import argparse
import hashlib
import struct
import sys
from pathlib import Path


_I386_RELOCATION_WIDTHS = {
    0x0001: 2,  # DIR16
    0x0002: 2,  # REL16
    0x0006: 4,  # DIR32
    0x0007: 4,  # DIR32NB
    0x0009: 2,  # SEG12
    0x000A: 2,  # SECTION
    0x000B: 4,  # SECREL
    0x000C: 4,  # TOKEN
    0x000D: 1,  # SECREL7
    0x0014: 4,  # REL32
}


def _sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()[:16]


def _cstr(data: bytes) -> str:
    return data.split(b"\0", 1)[0].decode("latin1", "replace")


def read_coff(path: Path):
    data = path.read_bytes()
    if len(data) < 20:
        raise ValueError(f"{path}: truncated COFF header")
    _machine, nsec, _stamp, symptr, nsym, optsz, _chars = struct.unpack_from("<HHLLLHH", data)
    sec_off = 20 + optsz
    str_off = symptr + nsym * 18
    strtab = data[str_off:] if str_off < len(data) else b""

    def name_at(raw: bytes) -> str:
        zero, off = struct.unpack("<LL", raw)
        if zero == 0 and off and off < len(strtab):
            return _cstr(strtab[off:])
        return _cstr(raw)

    sections = {}
    for i in range(nsec):
        off = sec_off + i * 40
        raw_name = data[off : off + 8]
        (_vsize, _vaddr, size, rawptr, relptr, _lineptr, nrel, _nline, chars) = struct.unpack_from(
            "<LLLLLLHHL", data, off + 8
        )
        sections[i + 1] = {
            "name": name_at(raw_name),
            "bytes": data[rawptr : rawptr + size] if rawptr else b"",
            "relptr": relptr,
            "nrel": nrel,
            "chars": chars,
        }

    symbols = {}
    section_functions = {}
    i = 0
    while i < nsym:
        off = symptr + i * 18
        raw_name = data[off : off + 8]
        value, secnum, typ, storage, naux = struct.unpack_from("<LhHBB", data, off + 8)
        name = name_at(raw_name)
        symbols[i] = name
        if secnum > 0 and storage == 2 and typ == 0x20:
            section_functions.setdefault(secnum, []).append((value, name))
        i += 1 + naux

    rows = []
    for secnum, functions in sorted(section_functions.items()):
        sec = sections[secnum]
        if sec["name"] != ".text":
            continue
        relocs = []
        for j in range(sec["nrel"]):
            roff = sec["relptr"] + j * 10
            offset, symidx, reltype = struct.unpack_from("<LLH", data, roff)
            width = _I386_RELOCATION_WIDTHS.get(reltype)
            addend = sec["bytes"][offset : offset + width].hex() if width is not None else None
            relocs.append(
                (offset, reltype, symbols.get(symidx, "#" + str(symidx)), addend)
            )
        starts = sorted(set(value for value, _name in functions))
        next_start = {
            start: starts[i + 1] if i + 1 < len(starts) else len(sec["bytes"])
            for i, start in enumerate(starts)
        }
        for start, fn in sorted(functions):
            end = next_start[start]
            body = sec["bytes"][start:end]
            selected_relocs = [reloc for reloc in relocs if start <= reloc[0] < end]
            targets = [f"{reltype:04x}:{target}" for _offset, reltype, target, _addend in selected_relocs]
            relocation_stream = [
                f"{offset - start:08x}:{reltype:04x}:{target}:{addend if addend is not None else '<unknown>'}"
                for offset, reltype, target, addend in selected_relocs
            ]
            rows.append(
                {
                    "function": fn,
                    "bytes": body,
                    "size": len(body),
                    "text_sha": _sha(body),
                    "relocs": len(targets),
                    "reloc_sha": _sha("\n".join(targets).encode()),
                    "reloc_stream": relocation_stream,
                    "reloc_detail_sha": _sha("\n".join(relocation_stream).encode()),
                    "reloc_stream_complete": all(
                        addend is not None for _offset, _reltype, _target, addend in selected_relocs
                    ),
                }
            )
    return _sha(data), rows


def source_hashes():
    try:
        from homm2.match.status import source_hashes as project_source_hashes
    except ImportError:
        return {}
    return project_source_hashes()


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("object", type=Path)
    ap.add_argument("--unit", help="config unit name, e.g. BASE/WIDGET")
    ap.add_argument("--function", action="append", help="exact mangled symbol filter")
    ap.add_argument("--compare", type=Path, help="second COFF object; report raw byte delta")
    args = ap.parse_args()
    object_sha, rows = read_coff(args.object)
    comparison = {}
    if args.compare:
        _comparison_sha, comparison_rows = read_coff(args.compare)
        comparison = {row["function"]: row for row in comparison_rows}
    sh = source_hashes() if args.unit else {}
    wanted = set(args.function or ())
    suffix = "\tcompare_size\tdiff_bytes" if args.compare else ""
    print("object_sha\tfunction\tsource_sha\ttext_size\ttext_sha\trelocs\treloc_sha" + suffix)
    for row in rows:
        if wanted and row["function"] not in wanted:
            continue
        src = sh.get((args.unit, row["function"]), "-") if args.unit else "-"
        compare_suffix = ""
        if args.compare:
            other = comparison.get(row["function"])
            if other is None:
                compare_suffix = "\t-\t-"
            else:
                common = min(row["size"], other["size"])
                diffs = sum(a != b for a, b in zip(row["bytes"][:common], other["bytes"][:common]))
                diffs += abs(row["size"] - other["size"])
                compare_suffix = f"\t{other['size']}\t{diffs}"
        print(
            f"{object_sha}\t{row['function']}\t{src}\t{row['size']}\t"
            f"{row['text_sha']}\t{row['relocs']}\t{row['reloc_sha']}" + compare_suffix
        )
    return 0


if __name__ == "__main__":
    sys.exit(main())
