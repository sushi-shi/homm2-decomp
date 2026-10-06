#!/usr/bin/env python3
"""Build build/gen/symbol_names.csv from source VA/DATA annotations.

This image is stripped: no debug stream names anything, so there is no gift
inventory that exists before reconstruction starts. Every address and identity
must therefore come from explicit, reviewable project evidence.

That makes the rule strict and deliberate: identities come from source markers
or one of three explicit providers: the fixed MASM units, the retail PE import
table joined to current candidate spellings, and reviewed compiler-generated
data re-proven against current COFF COMMON definitions. Nothing is inferred
from Ghidra names, neighbouring extents, or a previous generated inventory.

Unlike homm2.retail_labels.annotated_functions - a library for the static-helper case -
this names every annotated definition, free function and method alike, since
nothing else will.

    python3 -m homm2.retail_labels.source            # -> build/gen/symbol_names.csv
    python3 -m homm2.retail_labels.source --check    # report, write nothing
"""
from __future__ import annotations
from homm2.manifest import claim_files

import argparse
from concurrent.futures import ProcessPoolExecutor
import gc
import json
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

import clang.cindex as ci

from homm2.retail_labels.annotated_functions import (
    IMAGE_BASE, VA_TOKEN, VA_MARKER, _annotation, configure_libclang,
)
from homm2.retail_labels.annotated_data import (
    DATA_TOKEN, ClangMode, _clang_args, _mask_lexical_noise, definitions_for_file,
)
from homm2.retail_labels.annotated_compgen_data import (
    compgen_data_symbol_name,
    source_compgen_data,
)
from homm2.retail_labels.annotated_vtables import source_vtables
from homm2.retail_labels.providers import (
    assembly_claims,
    compiler_data_claims,
    import_claims,
)
from homm2.core.paths import DEFAULT_IMAGE, REPO, gen_dir, image_build, image_key, retail_dir, retail_exe

OUTPUT = gen_dir() / "symbol_names.csv"
COMPGEN_OUTPUT = gen_dir() / "compiler_generated_functions.csv"
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
    for path in claim_files(source_root):
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


def unreviewed_diagnostics(diagnostics, repo: Path):
    # The boolean audit already records the precise VC6/Clang language
    # incompatibilities in this source tree. Reuse those reviewed exceptions;
    # an unrelated error in the same file must still fail symbol recovery.
    from homm2.audit.bool_fields import _project_relative, _reviewed_exceptions
    reviewed = _reviewed_exceptions(repo)
    return [d for d in diagnostics if (
        "parse-diagnostic", _project_relative(str(d.location.file), repo), "", "", d.spelling
    ) not in reviewed]


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
    unreviewed = unreviewed_diagnostics(own, repo)
    if unreviewed:
        detail = "; ".join(str(d) for d in unreviewed[:5])
        raise ValueError(f"{path}: Clang could not read the annotations: {detail}")
    if own:
        print(f"[source-symbols] {path.name}: {len(own)} reviewed VC6/Clang diagnostics")
    if errors:
        print(f"[source-symbols] {path.name}: tolerating {len(errors)} "
              "system/vendor header errors")

    unit = path.relative_to(source_root).with_suffix("").as_posix()
    rows = []
    template_bodies = {}
    for cursor in translation.cursor.walk_preorder():
        if cursor.kind not in DEFINITION_KINDS:
            continue
        if cursor.location.file is None:
            continue
        if Path(str(cursor.location.file)).resolve() != path:
            continue
        annotated = _annotation(cursor)
        if annotated is None:
            continue
        owner = _dependent_template_owner(cursor) if not cursor.mangled_name else None
        # With delayed template parsing, an uninstantiated body is reported
        # as a declaration. Still audit its VA claim rather than dropping it.
        if not cursor.is_definition() and not owner:
            continue
        va, size = annotated
        # A marker that cannot produce a symbol is a source defect, not a row to
        # drop quietly: the delinker would carve a span nothing can be matched to.
        name = cursor.mangled_name
        if not name and owner:
            if owner not in template_bodies:
                template_bodies[owner] = _instantiated_template_bodies(path, repo, owner)
            matches = template_bodies[owner].get(cursor.location.offset, set())
            if len(matches) != 1:
                raise ValueError(
                    f"{path}:{cursor.location.line}: VA template definition "
                    f"{cursor.spelling!r} requires exactly one instantiated body, "
                    f"found {len(matches)}")
            name = next(iter(matches))
        if va < IMAGE_BASE or size <= 0 or not name:
            raise ValueError(
                f"{path}:{cursor.location.line}: unusable VA marker on "
                f"{cursor.spelling!r}")
        rows.append(SourceSymbol(
            rva=va - IMAGE_BASE, name=_vc6_symbol_name(cursor, name), unit=unit,
            size=size, kind="func", provenance="source-annotation"))
    expected = [(int(m[1], 16) - IMAGE_BASE, int(m[2], 0))
                for m in VA_MARKER.finditer(_mask_lexical_noise(blob))]
    recovered = [(r.rva, r.size) for r in rows]
    if sorted(expected) != sorted(recovered):
        raise ValueError(f"{path}: VA markers and recovered function definitions disagree")

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


def _dependent_template_owner(cursor) -> str | None:
    """Filter the AST dump to the annotated method's semantic class owner."""
    parent = cursor.semantic_parent
    names = []
    dependent = False
    while parent is not None and parent.kind != ci.CursorKind.TRANSLATION_UNIT:
        dependent |= parent.kind == ci.CursorKind.CLASS_TEMPLATE
        if parent.spelling:
            names.append(parent.spelling.split("<", 1)[0])
        parent = parent.semantic_parent
    return "::".join(reversed(names)) if dependent and names else None


def _json_roots(payload: str):
    """Clang's filtered JSON dump can contain several concatenated roots."""
    decoder = json.JSONDecoder()
    offset = 0
    while offset < len(payload):
        while offset < len(payload) and payload[offset].isspace():
            offset += 1
        if offset == len(payload):
            break
        root, offset = decoder.raw_decode(payload, offset)
        yield root


def _instantiated_template_bodies(path: Path, repo: Path, owner: str):
    """Read actual Clang-instantiated names missing from libclang's child walk.

    Match the annotated primary definition offset and instantiated body.
    Declarations or implicit helpers cannot supply a VA identity. Never infer mangling
    from a template's spelling or choose between multiple specializations.
    """
    command = [
        "clang", *_clang_args(repo, path, mode=ClangMode.RETAIL_ANALYSIS),
        "-fsyntax-only", "-Xclang", "-ast-dump=json", "-Xclang",
        "-ast-dump-filter", "-Xclang", owner, str(path),
    ]
    result = subprocess.run(command, capture_output=True, text=True, timeout=60)
    if result.returncode:
        # Match symbols_for_file's policy: errors in owned source are fatal;
        # vendor-header recovery is permitted when Clang still produces a body.
        diagnostics = re.findall(
            r"^(.+?):[0-9]+:[0-9]+: (?:fatal )?error:", result.stderr, re.M)
        owned = [name for name in diagnostics if any(
            Path(name).resolve().is_relative_to(repo / directory)
            for directory in ("src", "include"))]
        if not diagnostics or owned:
            raise ValueError(f"{path}: template AST dump failed: {result.stderr.strip()}")
    bodies = {}

    def visit(node):
        if not isinstance(node, dict):
            return
        children = node.get("inner", [])
        kinds = {child.get("kind") for child in children}
        location = node.get("loc", {})
        filename = location.get("file")
        if (node.get("kind") in {
                "CXXConstructorDecl", "CXXDestructorDecl", "CXXMethodDecl",
                "CXXConversionDecl"}
                and node.get("mangledName") and "offset" in location
                and "CompoundStmt" in kinds
                and (filename is None or Path(filename).resolve() == path.resolve())):
            bodies.setdefault(location["offset"], set()).add(node["mangledName"])
        for child in children:
            visit(child)

    try:
        for root in _json_roots(result.stdout):
            visit(root)
    except json.JSONDecodeError as error:
        raise ValueError(f"{path}: invalid template AST dump: {error}") from error
    return bodies


def _vc6_symbol_name(cursor, mangled_name: str | None = None) -> str:
    """clang's MSVC mangler names destructor definitions as the vbase
    destructor (??_D...@@QAEXXZ); VC6 only emits that helper for classes with
    virtual bases, which this codebase never uses. The retail symbol for a
    user destructor is the plain ??1, virtual destructors as UAE."""
    name = cursor.mangled_name if mangled_name is None else mangled_name
    if cursor.kind == ci.CursorKind.DESTRUCTOR and name.startswith("??_D"):
        access = "U" if cursor.is_virtual_method() else "Q"
        assert name.endswith("@@QAEXXZ"), name
        return "??1" + name[len("??_D"):-len("@@QAEXXZ")] + f"@@{access}AE@XZ"
    return name


# Reviewed identification CSVs: interim claims kept out of source until the
# campaign converts them to markers (compgen rows become VA_COMPGEN, CRT and
# thunk rows stay config-owned). Each maps to (unit, name-builder).
REVIEWED_CLAIMS = (
    ("config/retail/functions_compgen.csv", None,
     lambda row: (row["unit"], row["symbol"])),
    ("config/retail/functions_static_libs.csv", "reviewed-crt",
     lambda row: ("(libcmt)", row["symbol"])),
    ("config/retail/functions_imports.csv", "reviewed-thunk",
     lambda row: ("(imports)", (row.get("coff") or "").strip() or "%s@%s" % (
         row["symbol"], row["dll"].rsplit(".", 1)[0]))),
    ("config/retail/functions_eh.csv", "reviewed-funclet",
     lambda row: ("(funclets)", row["name"])),
)


def reviewed_claims(repo: Path, image: str = DEFAULT_IMAGE) -> list[SourceSymbol]:
    """Function claims carried by the image's reviewed identification CSVs."""
    import csv as _csv
    rows: list[SourceSymbol] = []
    for name, provenance, build in REVIEWED_CLAIMS:
        path = (repo / name if image == DEFAULT_IMAGE
                else retail_dir(image) / Path(name).name)
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
            include_binary_providers: bool | None = None) -> list[SourceSymbol]:
    rows: list[SourceSymbol] = []
    paths = [path.resolve() for path in claim_files(source_root)]
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

    if include_binary_providers is None:
        include_binary_providers = (
            source_root.resolve() == (repo / "src").resolve()
        )
    if include_binary_providers:
        provider_rows = assembly_claims()
        provider_rows += import_claims(
            repo / "build/orig/HMM2PL.exe",
            repo / "build/objdiff/base",
            repo / "build/toolchain/msvc/lib",
        )
        provider_rows += compiler_data_claims(
            repo / "config/retail/data_compgen.tsv",
            repo / "build/objdiff/base",
        )
        rows.extend(SourceSymbol(
            row.rva, row.name, row.unit, row.size, row.kind, row.provenance,
        ) for row in provider_rows)
        provider_rvas = {row.name: row.rva for row in provider_rows}
        for row in rows:
            expected = provider_rvas.get(row.name)
            if expected is not None and expected != row.rva:
                raise ValueError(
                    f"provider identity {row.name} binds 0x{expected:x}, but "
                    f"{row.provenance} binds 0x{row.rva:x}"
                )

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


def _manifest_targets(repo: Path, image: str = DEFAULT_IMAGE) -> list[int]:
    """RVAs the reviewed DIR32 sites point at (read from the retail image)."""
    import struct as _struct
    manifest = retail_dir(image) / "absolute_relocations.tsv"
    exe = retail_exe(image)
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


from homm2.core.usage import logged


VA_AT_ANNOTATION = re.compile(
    r"^va_at:(?P<image>\w+) (?P<va>0x[0-9a-fA-F]+) size:(?P<size>0x[0-9a-fA-F]+|[0-9]+)$")
VA_AT_MARKER = re.compile(
    r"\bVA_AT\s*\(\s*(\w+)\s*,\s*(0x[0-9a-fA-F]+)\s*,\s*(0x[0-9a-fA-F]+|[0-9]+)\s*\)")


def image_claims_for_file(path: Path, source_root: Path, repo: Path,
                          image: str) -> list[SourceSymbol]:
    """`VA_AT(image, ...)` claims of one shared source, parsed with the image's
    defines so its variants are the definitions the markers sit on."""
    from homm2.manifest import image_defines
    blob = path.read_bytes()
    expected = [(int(m[2], 16) - IMAGE_BASE, int(m[3], 0))
                for m in VA_AT_MARKER.finditer(_mask_lexical_noise(blob).decode("latin-1"))
                if m[1] == image]
    if not expected:
        return []
    configure_libclang()
    args = _clang_args(repo, path, mode=ClangMode.RETAIL_ANALYSIS)
    args += ["-D" + flag[2:] for flag in image_defines(image)]
    translation = ci.Index.create().parse(str(path), args=args)
    unit = path.relative_to(source_root).with_suffix("").as_posix()
    rows = []
    for cursor in translation.cursor.walk_preorder():
        if cursor.kind not in DEFINITION_KINDS or cursor.location.file is None:
            continue
        if Path(str(cursor.location.file)).resolve() != path.resolve():
            continue
        for child in cursor.get_children():
            if child.kind != ci.CursorKind.ANNOTATE_ATTR:
                continue
            match = VA_AT_ANNOTATION.match(child.spelling)
            if not match or match["image"] != image or not cursor.is_definition():
                continue
            if not cursor.mangled_name:
                raise ValueError(f"{path}:{cursor.location.line}: unusable VA_AT marker")
            rows.append(SourceSymbol(
                rva=int(match["va"], 16) - IMAGE_BASE,
                name=_vc6_symbol_name(cursor, cursor.mangled_name), unit=unit,
                size=int(match["size"], 0), kind="func",
                provenance=f"source-annotation:{image}"))
    if sorted(expected) != sorted((r.rva, r.size) for r in rows):
        raise ValueError(f"{path}: VA_AT({image}) markers and recovered definitions disagree")
    del translation
    gc.collect()
    return rows


def placement_claims(image: str) -> list[SourceSymbol]:
    """The game identities a shared unit spells, at this image's addresses
    (config/retail/<image>/placements.tsv, `homm2 audit placements`)."""
    import csv as _csv
    path = retail_dir(image) / "placements.tsv"
    if not path.is_file():
        return []
    rows = []
    with path.open(newline="") as stream:
        for row in _csv.DictReader((line for line in stream if not line.startswith("#")),
                                   delimiter="\t"):
            rows.append(SourceSymbol(
                rva=int(row["rva"], 16), name=row["name"], unit=row["unit"],
                size=int(row["size"], 16), kind=row["kind"],
                provenance=f"placement:0x{int(row['game_rva'], 16):x}"))
    return rows


def collect_image(image: str, repo: Path) -> list[SourceSymbol]:
    """The claimed inventory of an image other than the game.

    Units that link only into this image spell its addresses in their own `VA`
    and `DATA` markers. Shared units spell game addresses; their identities
    come through the image's placements. Imports are named from the image's
    own import table; every reviewed DIR32 target nobody claims gets a
    `const_` alias, as for the game."""
    from homm2.manifest import all_units, unit_images
    rows: list[SourceSymbol] = []
    # A shared unit's own editor bodies (VA_AT) win over a placement.
    source_root = repo / "src"
    for unit in all_units():
        if image in unit_images(unit) and DEFAULT_IMAGE in unit_images(unit) \
                and unit["source"].endswith(".cpp"):
            rows.extend(image_claims_for_file(
                (repo / unit["source"]).resolve(), source_root, repo, image))
    own = [repo / u["source"] for u in all_units()
           if image in unit_images(u) and DEFAULT_IMAGE not in unit_images(u)
           and u["source"].endswith(".cpp")]
    for path in own:
        rows.extend(symbols_for_file(path.resolve(), source_root, repo))
    # The image's own definitions win over a game body or datum placed there
    # (the editor's copies of KB.cpp's functions and globals).
    claimed = {row.rva for row in rows}
    rows.extend(row for row in placement_claims(image) if row.rva not in claimed)
    # The scanners below read the selected image's claim space (its own units).
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
    rows += [SourceSymbol(r.rva, r.name, r.unit, r.size, r.kind, r.provenance)
             for r in import_claims(retail_exe(image), image_build(image) / "objdiff/base",
                                    repo / "build/toolchain/msvc/lib")]
    seen: dict[int, SourceSymbol] = {}
    for row in sorted(rows):
        clash = seen.get(row.rva)
        if clash is not None and clash.name != row.name:
            raise ValueError(f"0x{row.rva:x} is claimed by both {clash.name} ({clash.unit}) "
                             f"and {row.name} ({row.unit})")
        seen[row.rva] = row
    for row in sorted(reviewed_claims(repo, image)):
        seen.setdefault(row.rva, row)
    for rva in _manifest_targets(repo, image):
        if rva not in seen:
            seen[rva] = SourceSymbol(
                rva=rva, name="const_%08x" % rva, unit="_const",
                size=0, kind="data", provenance="reloc-manifest-target")
    return sorted(seen.values())


@logged
def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--source", type=Path, default=REPO / "src")
    parser.add_argument("--output", type=Path, default=OUTPUT)
    parser.add_argument("--compgen-output", type=Path, default=COMPGEN_OUTPUT)
    parser.add_argument("--check", action="store_true",
                        help="report what would be written and write nothing")
    args = parser.parse_args(argv)

    source_root = args.source.resolve()
    if image_key() == DEFAULT_IMAGE:
        compgen = source_compgen_functions(source_root, REPO)
        rows = collect(source_root, REPO)
    else:
        compgen = []
        rows = collect_image(image_key(), REPO)
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
