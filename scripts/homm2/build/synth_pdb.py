#!/usr/bin/env python3
"""synth_pdb.py - synthesize a PDB for vostok-delinker from build/gen/symbol_names.csv.

vostok-delinker consumes a PDB (not CodeView) to slice HEROES2W.EXE into per-symbol
COFF .obj files. HEROES2W ships a CodeView NB09 stream, but the delinker wants a PDB -
so we build one. Unlike gruntz (which re-synths as Ghidra discovers names), our symbol
set is COMPLETE and authoritative from CodeView, so this is a ONE-TIME step run by
`homm2 init`: every function is already named + unit-attributed, so we always emit
per-unit line info (no Ghidra functions.csv, no address-bucketed remainder).

Pipeline:
  build/gen/symbol_names.csv (rva,name,unit,size,kind,provenance)
      -> PDB-YAML (one DBI module; per-function C13 line info -> c:\\proj\\<unit>.c;
         S_GPROC32 for funcs, S_LDATA32 for data)
      -> `llvm-pdbutil yaml2pdb`  -> build/pdb/HEROES2W.pdb
      -> byte-patch DBIHeader.symbol_records_stream (0x14) to an empty stream so
         pdb2's global_symbols() returns empty (yaml2pdb writes 0xFFFF nil there).
"""
import argparse, csv, hashlib, os, re, struct, subprocess, sys
from pathlib import Path

from homm2.build.reloc_owners import (
    is_interior_reloc_alias, load_definition_rvas, load_explicit_extents,
    owners_from_rows,
)
from homm2.build.reloc_alias_metadata import encode_pdb_alias, load_aliases

REPO = next((p for p in Path(__file__).resolve().parents if (p / "flake.nix").exists()), Path.cwd())


def read_sections(exe):
    d = exe.read_bytes()
    e = struct.unpack_from("<I", d, 0x3c)[0]
    n = struct.unpack_from("<H", d, e + 6)[0]
    osz = struct.unpack_from("<H", d, e + 20)[0]
    secs = {}  # name -> (seg_index_1based, base_rva, end_rva)
    for i in range(n):
        o = e + 24 + osz + i * 40
        nm = d[o:o + 8].rstrip(b"\0").decode("latin1")
        vs, va = struct.unpack_from("<II", d, o + 8)
        secs[nm] = (i + 1, va, va + vs)
    return secs


def seg_of(secs, rva):
    for nm, (seg, base, end) in secs.items():
        if base <= rva < end:
            return seg, base
    return None, None


def sanitize(name):
    return "".join(c if 32 <= ord(c) < 127 else "_" for c in name).replace("'", "''")


def read_csv(path):
    rows = [ln for ln in open(path) if not ln.lstrip().startswith("#")]
    funcs, data = [], []   # funcs: (rva,size,name,unit) ; data: (rva,name,seg,off)
    return rows


def validate_reloc_aliases(rows, reloc_aliases):
    public_data = {
        row["name"]: int(row["rva"], 0) for row in rows
        if (row.get("kind") == "data" and
            row.get("provenance") == "cv-public-data")
    }
    for alias in reloc_aliases.values():
        owner_rva = public_data.get(alias.owner)
        if owner_rva is None:
            raise ValueError("reloc alias owner is not public data: %s" % alias.owner)
        if (owner_rva + alias.addend) & 0xffffffff != alias.target_rva:
            raise ValueError("reloc alias does not resolve to target RVA 0x%x" %
                             alias.target_rva)


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument("--exe", default=os.environ.get("HOMM2_EXE", str(REPO / "build/orig/HEROES2W.EXE")))
    ap.add_argument("--csv", default=str(REPO / "build/gen/symbol_names.csv"))
    ap.add_argument("--out", default=str(REPO / "build/pdb/HEROES2W.pdb"))
    ap.add_argument("--reloc-aliases",
                    help="paired exact-address public-owner alias TSV")
    a = ap.parse_args(argv)
    exe, out = Path(a.exe), Path(a.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    secs = read_sections(exe)
    text_seg, text_base = secs[".text"][0], secs[".text"][1]

    funcs, data = [], []
    rows = list(csv.DictReader(
        [ln for ln in open(a.csv) if not ln.lstrip().startswith("#")]))
    reloc_aliases = load_aliases(a.reloc_aliases) if a.reloc_aliases else {}
    validate_reloc_aliases(rows, reloc_aliases)
    owners = owners_from_rows(
        rows, load_definition_rvas(), load_explicit_extents())
    canonicalized_aliases = 0
    paired_aliases = set()
    for row in rows:
        rva = int(row["rva"], 16); name = row["name"].strip(); unit = row["unit"].strip()
        size = int((row.get("size") or "0"), 16); kind = (row.get("kind") or "func").strip()
        seg, base = seg_of(secs, rva)
        if seg is None:
            continue
        if kind == "func" and seg == text_seg and size > 0:
            funcs.append((rva, size, name, unit))
        else:
            paired_alias = reloc_aliases.get(rva)
            if paired_alias is not None:
                name = encode_pdb_alias(paired_alias)
                paired_aliases.add(rva)
            elif is_interior_reloc_alias(row, owners):
                canonicalized_aliases += 1
                continue
            data.append((rva, name, seg, rva - base))
    missing_aliases = sorted(set(reloc_aliases) - paired_aliases)
    if missing_aliases:
        raise ValueError("reloc aliases lack exact symbol rows: %s" %
                         ", ".join("0x%x" % rva for rva in missing_aliases[:8]))
    funcs.sort(); data.sort()

    # per-unit source files (the delinker emits one <unit>.c.obj per file)
    files = []
    seen = set()
    func_file = {}
    for rva, size, name, unit in funcs:
        sf = r"c:\proj\%s.c" % unit.replace("/", "\\")   # tier folder -> subdir in delink output
        func_file[rva] = sf
        if sf not in seen:
            seen.add(sf); files.append(sf)

    yaml = out.with_suffix(".yaml")
    with open(yaml, "w") as w:
        W = w.write
        W("MSF:\n  SuperBlock:\n    BlockSize:       4096\n    FreeBlockMap:    2\n"
          "    NumBlocks:       0\n    NumDirectoryBytes: 0\n    Unknown1:        0\n    BlockMapAddr:    0\n")
        W("PdbStream:\n  Age:             1\n  Guid:            '{00000000-0000-0000-0000-000000000000}'\n"
          "  Signature:       0\n  Features:        [ VC140 ]\n  Version:         VC70\n")
        W("DbiStream:\n  VerHeader:       V70\n  Age:             1\n  BuildNumber:     0\n"
          "  PdbDllVersion:   0\n  PdbDllRbld:      0\n  Flags:           0\n  MachineType:     x86\n")
        mod = r"c:\proj\HEROES2W"
        W("  Modules:\n    - Module:          '%s'\n      ObjFile:         '%s'\n" % (mod, mod))
        W("      SourceFiles:\n")
        for sf in files:
            W("        - '%s'\n" % sf)
        W("      Subsections:\n        - !FileChecksums\n          Checksums:\n")
        for sf in files:
            W("            - FileName:        '%s'\n              Kind:            MD5\n"
              "              Checksum:        %s\n" % (sf, hashlib.md5(sf.encode()).hexdigest().upper()))
        for rva, size, name, unit in funcs:
            W("        - !Lines\n          CodeSize:        %d\n          Flags:           [  ]\n"
              "          RelocOffset:     %d\n          RelocSegment:    %d\n          Blocks:\n"
              % (size, rva - text_base, text_seg))
            W("            - FileName:        '%s'\n              Lines:\n"
              "                - Offset:          0\n                  LineStart:       1\n"
              "                  EndDelta:        0\n                  IsStatement:     true\n"
              "              Columns:         []\n" % func_file[rva])
        W("      Modi:\n        Records:\n")
        for rva, size, name, unit in funcs:
            W("          - Kind:            S_GPROC32\n            ProcSym:\n"
              "              CodeSize:        %d\n              DbgStart:        0\n              DbgEnd:          0\n"
              "              FunctionType:    0\n              Offset:          %d\n              Segment:         %d\n"
              "              Flags:           [  ]\n              DisplayName:     '%s'\n"
              % (size, rva - text_base, text_seg, sanitize(name)))
            W("          - Kind:            S_END\n            ScopeEndSym:     {}\n")
        for rva, name, seg, off in data:
            W("          - Kind:            S_LDATA32\n            DataSym:\n              Type:            0\n"
              "              Offset:          %d\n              Segment:         %d\n              DisplayName:     '%s'\n"
              % (off, seg, sanitize(name)))
        W("StringTable:\n")
        for sf in files:
            W("  - '%s'\n" % sf)

    subprocess.run(["llvm-pdbutil", "yaml2pdb", "-pdb", str(out), str(yaml)], check=True)

    # patch DBIHeader.symbol_records_stream (offset 0x14) -> an empty stream index
    dump = subprocess.run(["llvm-pdbutil", "dump", "--streams", str(out)],
                          capture_output=True, text=True, check=True).stdout
    empty = next((int(m.group(1)) for line in dump.splitlines()
                  if (m := re.search(r"Stream\s+(\d+)\s+\(\s*0 bytes\)", line))), None)
    if empty is not None:
        _patch_dbi(out, empty)
    print("[synth_pdb] %d funcs + %d data -> %s (%d units); "
          "%d explicit-owner aliases omitted; %d paired owner aliases encoded" %
          (len(funcs), len(data), out, len(files), canonicalized_aliases,
           len(paired_aliases)))


def _patch_dbi(pdb, empty_stream):
    # Walk the MSF to find the DBI stream (stream 3) first block, patch offset 0x14.
    d = bytearray(pdb.read_bytes())
    # MSF SuperBlock fields follow the 32-byte magic signature.
    bs = struct.unpack_from("<I", d, 32)[0]            # BlockSize
    num_blocks = struct.unpack_from("<I", d, 40)[0]    # NumBlocks
    num_dir_bytes = struct.unpack_from("<I", d, 44)[0] # NumDirectoryBytes
    blk_map_addr = struct.unpack_from("<I", d, 52)[0]  # BlockMapAddr
    # directory block list
    ndir_blocks = (num_dir_bytes + bs - 1) // bs
    dir_blocks = [struct.unpack_from("<I", d, blk_map_addr * bs + 4 * i)[0] for i in range(ndir_blocks)]
    directory = b"".join(d[b * bs:b * bs + bs] for b in dir_blocks)[:num_dir_bytes]
    nstreams = struct.unpack_from("<I", directory, 0)[0]
    sizes = [struct.unpack_from("<i", directory, 4 + 4 * i)[0] for i in range(nstreams)]
    pos = 4 + 4 * nstreams
    stream_blocks = []
    for s in sizes:
        nb = 0 if s < 0 else (s + bs - 1) // bs
        stream_blocks.append([struct.unpack_from("<I", directory, pos + 4 * j)[0] for j in range(nb)])
        pos += 4 * nb
    DBI = 3
    if DBI < len(stream_blocks) and stream_blocks[DBI]:
        off = stream_blocks[DBI][0] * bs + 0x14
        struct.pack_into("<H", d, off, empty_stream)
        pdb.write_bytes(d)


if __name__ == "__main__":
    main()
