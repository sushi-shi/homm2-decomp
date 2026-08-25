#!/usr/bin/env python3
"""Build build/gen/symbol_names.csv from the VA/DATA annotations in the source.

This image is stripped: no debug stream names anything, so there is no gift
inventory that exists before reconstruction starts. The only thing that knows
an address here is the source itself, and only where somebody has written the
address down.

That makes the rule strict and deliberate: a symbol exists for the delinker if and
only if a `VA(...)` or `DATA(...)` marker names it. Nothing is inferred from
Ghidra's function inventory, from signature matching, or from a neighbouring
symbol's extent. config/retail_functions.csv is a work list of 2,472 candidate
boundaries; this is the far smaller set that has been claimed and can be delinked
against. The two are not the same and must not be conflated - one is analysis
opinion, the other is a reviewed claim.

The set is empty until the campaign starts marking, and an empty inventory is the
correct answer to "what has been proven so far", not a failure.

Unlike homm2.build.annotated_functions - a library for the static-helper case -
this names every annotated definition, free function and method alike, since
nothing else will.

    python3 -m homm2.build.source_symbols            # -> build/gen/symbol_names.csv
    python3 -m homm2.build.source_symbols --check    # report, write nothing
    python3 -m homm2.build.source_symbols --ignore-donations  # bootstrap only
"""
from __future__ import annotations

import argparse
from concurrent.futures import ProcessPoolExecutor
import gc
import re
import sys
from dataclasses import dataclass
from pathlib import Path

import clang.cindex as ci

from homm2.build.annotated_functions import (
    IMAGE_BASE, VA_TOKEN, _annotation, configure_libclang,
)
from homm2.build.annotated_data import (
    DATA_TOKEN, ClangMode, _clang_args, definitions_for_file,
)
from homm2.build.annotated_compgen_data import (
    compgen_data_symbol_name,
    source_compgen_data,
)
from homm2.build.annotated_vtables import source_vtables
from homm2.core.paths import REPO

OUTPUT = REPO / "build/gen/symbol_names.csv"
COMPGEN_OUTPUT = REPO / "build/gen/compiler_generated_functions.csv"
HEADER = "rva,name,unit,size,kind,provenance\n"
COMPGEN_HEADER = "rva,name,unit,size,kind,owner,source,line\n"
COMPGEN_MARKER = re.compile(
    r"^\s*VA_COMPGEN\(\s*(0x[0-9a-fA-F]+)\s*,\s*"
    r"(0x[0-9a-fA-F]+|[0-9]+)\s*,\s*([A-Z][A-Z0-9_]*)\s*,\s*"
    r"([A-Za-z_]\w*(?:::[A-Za-z_]\w*)*)\s*\)\s*$")
COMPGEN_KINDS = {
    "STATIC_INIT_DISPATCH", "STATIC_ATEXIT", "STATIC_DTOR", "STATIC_CTOR",
    "LOCALE_FACET_ID_INIT", "LOCALE_FACET_ID_ATEXIT",
}

# Every cursor kind that can carry a VA marker and produce a linker symbol.
DEFINITION_KINDS = (
    ci.CursorKind.FUNCTION_DECL,
    ci.CursorKind.CXX_METHOD,
    ci.CursorKind.CONSTRUCTOR,
    ci.CursorKind.DESTRUCTOR,
    ci.CursorKind.CONVERSION_FUNCTION,
)


@dataclass(frozen=True, order=True)
class SourceSymbol:
    rva: int
    name: str
    unit: str
    size: int
    kind: str
    provenance: str


@dataclass(frozen=True, order=True)
class SourceCompgenFunction:
    rva: int
    name: str
    unit: str
    size: int
    kind: str
    owner: str
    source: str
    line: int


def compgen_functions_for_file(
        path: Path, source_root: Path, repo: Path) -> list[SourceCompgenFunction]:
    """Read semantic identities for bodies emitted by the compiler itself."""
    unit = path.relative_to(source_root).with_suffix("").as_posix()
    rows = []
    for line_number, line in enumerate(
            path.read_text(encoding="latin-1").splitlines(), 1):
        match = COMPGEN_MARKER.match(line)
        if match is None:
            continue
        va, size = int(match.group(1), 16), int(match.group(2), 0)
        kind, owner = match.group(3), match.group(4)
        if va < IMAGE_BASE or size <= 0 or kind not in COMPGEN_KINDS:
            raise ValueError(f"{path}:{line_number}: invalid VA_COMPGEN marker")
        name = "__h2cg$%s$%s$%s" % (
            unit.replace("/", "$"), kind.lower(), owner.replace("::", "$"))
        rows.append(SourceCompgenFunction(
            rva=va - IMAGE_BASE, name=name, unit=unit, size=size,
            kind=kind, owner=owner,
            source=path.relative_to(repo).as_posix(), line=line_number))
    return rows


def source_compgen_functions(
        source_root: Path, repo: Path) -> list[SourceCompgenFunction]:
    rows = []
    for path in sorted(source_root.rglob("*.cpp")):
        rows.extend(compgen_functions_for_file(path.resolve(), source_root, repo))
    names = set()
    rvas = set()
    for row in sorted(rows):
        if row.name in names:
            raise ValueError(f"duplicate VA_COMPGEN semantic identity: {row.name}")
        if row.rva in rvas:
            raise ValueError(f"duplicate VA_COMPGEN RVA: 0x{row.rva:x}")
        names.add(row.name)
        rvas.add(row.rva)
    return sorted(rows)


def symbols_for_file(path: Path, source_root: Path, repo: Path,
                     index=None) -> list[SourceSymbol]:
    """Every VA- or DATA-annotated definition in one translation unit."""
    blob = path.read_bytes()
    if not VA_TOKEN.search(blob) and not DATA_TOKEN.search(blob):
        return []
    configure_libclang()
    translation = (index or ci.Index.create()).parse(
        str(path), args=_clang_args(repo, path, mode=ClangMode.RETAIL_ANALYSIS))
    # An error in one of OUR files can silently drop a marker, so it is fatal.
    # Errors confined to system or vendor headers (VC6's pre-standard STL does
    # not parse as C++98 - clang recovers and the game cursors survive) only
    # warn; the annotation walk below still hard-fails on unusable markers.
    errors = [d for d in translation.diagnostics if d.severity >= ci.Diagnostic.Error]
    own = [d for d in errors if d.location.file is not None and (
        Path(str(d.location.file)).resolve().is_relative_to(repo / "src")
        or Path(str(d.location.file)).resolve().is_relative_to(repo / "include"))]
    if own:
        detail = "; ".join(str(d) for d in own[:5])
        raise ValueError(f"{path}: Clang could not read the annotations: {detail}")
    if errors:
        print(f"[source-symbols] {path.name}: tolerating {len(errors)} "
              "system/vendor header errors")

    unit = path.relative_to(source_root).with_suffix("").as_posix()
    rows = []
    for cursor in translation.cursor.walk_preorder():
        if cursor.kind not in DEFINITION_KINDS or not cursor.is_definition():
            continue
        if cursor.location.file is None:
            continue
        if Path(str(cursor.location.file)).resolve() != path:
            continue
        annotated = _annotation(cursor)
        if annotated is None:
            continue
        va, size = annotated
        # A marker that cannot produce a symbol is a source defect, not a row to
        # drop quietly: the delinker would carve a span nothing can be matched to.
        if va < IMAGE_BASE or size <= 0 or not cursor.mangled_name:
            raise ValueError(
                f"{path}:{cursor.location.line}: unusable VA marker on "
                f"{cursor.spelling!r}")
        rows.append(SourceSymbol(
            rva=va - IMAGE_BASE, name=_vc6_symbol_name(cursor), unit=unit,
            size=size, kind="func", provenance="source-annotation"))

    # DATA() names an ordinary storage definition, including a block-scope
    # static. The marker binding, the one-VarDecl-per-marker rule and the
    # complete-type rule all live in annotated_data, which the data-topology and
    # link audits already read; reusing it keeps one meaning for a marker
    # instead of two parsers that can drift apart.
    for definition in definitions_for_file(path, source_root, repo, translation):
        # Same discipline as the VA case: a marker that cannot produce a symbol
        # is a source defect, not a row to drop quietly.
        if not definition.symbol or definition.rva < 0:
            raise ValueError(
                f"{definition.location}: unusable DATA marker on "
                f"{definition.name!r}")
        rows.append(SourceSymbol(
            rva=definition.rva, name=definition.symbol, unit=definition.unit,
            size=definition.size, kind="data", provenance="source-annotation"))
    # libclang translation units retain large cursor graphs.  This scanner
    # processes the whole project in one process, so relying on cyclic GC can
    # exhaust libclang state after a few dozen TUs and terminate the process.
    del translation
    gc.collect()
    return rows


def _symbols_for_files_worker(
        arguments: tuple[list[Path], Path, Path]) -> list[SourceSymbol]:
    """Parse a bounded TU batch in a disposable process.

    libclang retains native state after Python releases a translation unit.  A
    full-tree scan eventually exhausts the host process, so real project scans
    recycle workers before that accumulated state becomes material.
    """
    paths, source_root, repo = arguments
    configure_libclang()
    index = ci.Index.create()
    rows = []
    for path in paths:
        rows.extend(symbols_for_file(path, source_root, repo, index=index))
    return rows


def _vc6_symbol_name(cursor) -> str:
    """clang's MSVC mangler names destructor definitions as the vbase
    destructor (??_D...@@QAEXXZ); VC6 only emits that helper for classes with
    virtual bases, which this codebase never uses. The retail symbol for a
    user destructor is the plain ??1, virtual destructors as UAE."""
    name = cursor.mangled_name
    if cursor.kind == ci.CursorKind.DESTRUCTOR and name.startswith("??_D"):
        access = "U" if cursor.is_virtual_method() else "Q"
        assert name.endswith("@@QAEXXZ"), name
        return "??1" + name[len("??_D"):-len("@@QAEXXZ")] + f"@@{access}AE@XZ"
    return name


# Reviewed identification CSVs: interim claims kept out of source until the
# campaign converts them to markers (compgen rows become VA_COMPGEN, CRT and
# thunk rows stay config-owned). Each maps to (unit, name-builder).
REVIEWED_CLAIMS = (
    ("config/compgen_functions.csv", None,
     lambda row: (row["unit"], row["symbol"])),
    ("config/crt_functions.csv", "reviewed-crt",
     lambda row: ("(libcmt)", row["symbol"])),
    ("config/import_thunks.csv", "reviewed-thunk",
     lambda row: ("(imports)", (row.get("coff") or "").strip() or "%s@%s" % (
         row["symbol"], row["dll"].rsplit(".", 1)[0]))),
    ("config/eh_funclets.csv", "reviewed-funclet",
     lambda row: ("(funclets)", row["name"])),
)


def reviewed_claims(repo: Path) -> list[SourceSymbol]:
    """Function claims carried by the reviewed identification CSVs."""
    import csv as _csv
    rows: list[SourceSymbol] = []
    for name, provenance, build in REVIEWED_CLAIMS:
        path = repo / name
        if not path.is_file():
            continue
        provenance = provenance or "reviewed-compgen"
        with path.open(newline="") as stream:
            for row in _csv.DictReader(
                    line for line in stream
                    if not line.lstrip().startswith("#")):
                unit, symbol = build(row)
                rows.append(SourceSymbol(
                    rva=int(row["entry_rva"], 16), name=symbol, unit=unit,
                    size=int(row["size"], 0), kind="func",
                    provenance=provenance))
    return rows


def collect(source_root: Path, repo: Path,
            include_donations: bool = True) -> list[SourceSymbol]:
    rows: list[SourceSymbol] = []
    paths = [path.resolve() for path in sorted(source_root.rglob("*.cpp"))]
    if len(paths) <= 1:
        # Keep the small-fixture path direct so failures are easy to debug and
        # callers can substitute the parser in unit tests.
        for path in paths:
            rows.extend(symbols_for_file(path, source_root, repo))
    else:
        # ProcessPoolExecutor's max_tasks_per_child recycler can deadlock after
        # every worker reaches its cap.  Explicit 16-TU pools give each of two
        # processes at most eight translations and make lifetime unambiguous.
        for start in range(0, len(paths), 16):
            batch = paths[start:start + 16]
            worker_paths = [batch[::2], batch[1::2]]
            arguments = [
                (owned, source_root, repo)
                for owned in worker_paths if owned
            ]
            with ProcessPoolExecutor(max_workers=len(arguments)) as executor:
                for file_rows in executor.map(
                        _symbols_for_files_worker, arguments):
                    rows.extend(file_rows)
    for vtable in source_vtables(source_root, repo):
        rows.append(SourceSymbol(
            rva=vtable.rva, name=vtable.mangled_name, unit=vtable.unit,
            size=0, kind="data", provenance="source-vtable"))
    for claim in source_compgen_data(source_root, repo):
        rows.append(SourceSymbol(
            rva=claim.rva,
            name=compgen_data_symbol_name(claim.unit, claim.semantic_name),
            unit=claim.unit, size=claim.size, kind="data",
            provenance=f"source-DATA_COMPGEN:{claim.location}"))
    for claim in source_compgen_functions(source_root, repo):
        rows.append(SourceSymbol(
            rva=claim.rva, name=claim.name, unit=claim.unit, size=claim.size,
            kind="func", provenance=f"source-VA_COMPGEN:{claim.kind}"))

    seen: dict[int, SourceSymbol] = {}
    for row in sorted(rows):
        clash = seen.get(row.rva)
        # Two markers on one address means one of them is wrong, and delinking
        # would silently keep whichever sorted first.
        if clash is not None and clash.name != row.name:
            raise ValueError(
                f"0x{row.rva:x} is claimed by both {clash.name} ({clash.unit}) "
                f"and {row.name} ({row.unit})")
        seen[row.rva] = row

    # Reviewed CSV claims fill addresses source markers have not taken; a
    # source marker always wins its address. Duplicate names within one unit
    # get an @<rva> suffix so the delinked object stays one-symbol-one-name.
    named: dict[tuple[str, str], int] = {}
    for row in sorted(seen.values()):
        named[(row.unit, row.name)] = row.rva
    for row in sorted(reviewed_claims(repo)):
        if row.rva in seen:
            continue
        key = (row.unit, row.name)
        if key in named:
            row = SourceSymbol(
                rva=row.rva, name="%s@0x%x" % (row.name, row.rva),
                unit=row.unit, size=row.size, kind=row.kind,
                provenance=row.provenance)
            key = (row.unit, row.name)
        named[key] = row.rva
        seen[row.rva] = row

    # Donation evidence names data owners: every masked-identical function's
    # relocations vote (symbol, addend) per target, and target - addend pins
    # the owner's linked address. Unanimous owners become real data rows.
    claimed_names = {row.name: row.rva for row in seen.values()
                     if row.kind == "data" and
                     row.provenance == "source-annotation"}
    donated_names = _donated_owner_names(repo) if include_donations else []
    for rva, name in donated_names:
        # Donation and a written claim disagreeing about one name means the
        # claim's address is wrong; keeping both would put one symbol at two
        # addresses and let the delinker pick either.
        if claimed_names.get(name, rva) != rva:
            raise ValueError(
                f"{name} is claimed by DATA() at 0x{claimed_names[name]:x} "
                f"but donated at 0x{rva:x}")
        if rva not in seen:
            seen[rva] = SourceSymbol(
                rva=rva, name=name, unit="_data",
                size=0, kind="data", provenance="reloc-donation-owner")

    # Every DIR32 site in the reviewed manifest names a target the delinker
    # must be able to symbolize ("all constants must be named"). Targets not
    # covered by a claim get synthetic const_<RVA> aliases; name_strings later
    # upgrades the string-bearing ones to their canonical spellings.
    for rva in _manifest_targets(repo):
        if rva not in seen:
            seen[rva] = SourceSymbol(
                rva=rva, name="const_%08x" % rva, unit="_const",
                size=0, kind="data", provenance="reloc-manifest-target")
    return sorted(seen.values())


def _donated_owner_names(repo: Path) -> list[tuple[int, str]]:
    path = repo / "build/gen/reloc_target_names.tsv"
    if not path.is_file():
        return []
    rows = []
    for line in path.read_text().splitlines()[1:]:
        fields = line.split("\t")
        if len(fields) < 2 or fields[1] == "(conflict)":
            continue
        try:
            rva = int(fields[0], 16)
        except ValueError:
            continue
        if rva >= 0:
            rows.append((rva, fields[1]))
    return rows


def _manifest_targets(repo: Path) -> list[int]:
    """RVAs the reviewed DIR32 sites point at (read from the retail image)."""
    import struct as _struct
    manifest = repo / "config/delink_relocs.tsv"
    exe = repo / "build/orig/HMM2PL.exe"
    if not manifest.is_file() or not exe.is_file():
        return []
    data = exe.read_bytes()
    pe = _struct.unpack_from("<I", data, 0x3C)[0]
    section_count = _struct.unpack_from("<H", data, pe + 6)[0]
    optional = _struct.unpack_from("<H", data, pe + 20)[0]
    sections = []
    for index in range(section_count):
        header = pe + 24 + optional + index * 40
        virtual, raw_size, raw_offset = (
            _struct.unpack_from("<I", data, header + 12)[0],
            _struct.unpack_from("<I", data, header + 16)[0],
            _struct.unpack_from("<I", data, header + 20)[0])
        sections.append((virtual, raw_size, raw_offset))

    def read_u32(rva):
        for virtual, raw_size, raw_offset in sections:
            if virtual <= rva and rva + 4 <= virtual + raw_size:
                return _struct.unpack_from(
                    "<I", data, raw_offset + rva - virtual)[0]
        return None

    targets = set()
    for line in manifest.read_text().splitlines():
        if line.startswith("#") or line.startswith("site_rva") or not line.strip():
            continue
        value = read_u32(int(line.split("\t")[0], 16))
        if value is not None and value >= IMAGE_BASE:
            targets.add(value - IMAGE_BASE)
    return sorted(targets)


def render(rows) -> str:
    lines = [HEADER]
    for row in rows:
        lines.append(f"0x{row.rva:x},{row.name},{row.unit},"
                     f"0x{row.size:x},{row.kind},{row.provenance}\n")
    return "".join(lines)


def render_compgen(rows: list[SourceCompgenFunction]) -> str:
    lines = [COMPGEN_HEADER]
    for row in rows:
        lines.append(
            f"0x{row.rva:x},{row.name},{row.unit},0x{row.size:x},"
            f"{row.kind},{row.owner},{row.source},{row.line}\n")
    return "".join(lines)


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--source", type=Path, default=REPO / "src")
    parser.add_argument("--output", type=Path, default=OUTPUT)
    parser.add_argument("--compgen-output", type=Path, default=COMPGEN_OUTPUT)
    parser.add_argument("--check", action="store_true",
                        help="report what would be written and write nothing")
    parser.add_argument(
        "--ignore-donations", action="store_true",
        help="ignore generated relocation-owner names while bootstrapping "
             "fresh donation evidence")
    args = parser.parse_args(argv)

    source_root = args.source.resolve()
    compgen = source_compgen_functions(source_root, REPO)
    rows = collect(
        source_root, REPO, include_donations=not args.ignore_donations)
    functions = sum(1 for row in rows if row.kind == "func")
    print(f"[source-symbols] {len(rows)} annotated symbols "
          f"({functions} functions, {len(rows) - functions} data)")
    if not rows:
        print("[source-symbols] nothing is marked yet; the delinker will carve nothing")
    if args.check:
        return 0
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(render(rows))
    args.compgen_output.parent.mkdir(parents=True, exist_ok=True)
    args.compgen_output.write_text(render_compgen(compgen))
    try:
        shown = args.output.relative_to(REPO)
    except ValueError:                      # --output may point outside the tree
        shown = args.output
    print(f"[source-symbols] -> {shown}")
    try:
        compgen_shown = args.compgen_output.relative_to(REPO)
    except ValueError:
        compgen_shown = args.compgen_output
    print(f"[source-symbols] -> {compgen_shown} ({len(compgen)} semantic compiler functions)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
