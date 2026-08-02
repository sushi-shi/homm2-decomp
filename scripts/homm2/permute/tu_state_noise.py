#!/usr/bin/env python3
"""Controlled parser-visible TU-state experiment engine.

This standalone diagnostic does not rewrite the target. Each trial temporarily inserts
deterministic parser-visible declarations, definitions, or
curated includes before the target's ``VA`` metadata block, compiles the real translation
unit with the pinned MSVC, scores the requested symbol with objdiff, and restores the source
immediately.  Probe-emitted symbols/storage exist only in the disposable candidate object.

The source is unchanged on normal exit, compiler failure, timeout, or Ctrl-C. Each
baseline/trial compile has a bounded timeout; on expiry the complete compiler process
group is terminated so Wine/MSVC descendants cannot survive. Results, exact snippets,
and COFF metrics are written to ``build/tu-state-noise``. Generated noise is diagnostic
input only and is never written back to reconstructed source.

Run inside ``nix develop .#build`` after entering the worktree first::

    python3 -m homm2.permute.tu_state_noise \
      --source src/BASE/WINMGR.cpp --rva 0xca6d0 --trials 40

This is appropriate only after semantics, frame/slots, CFG, and external relocations
have already been audited. It is not a substitute for reconstruction, homm2.core.od_slots,
or a bounded source-variant search.
"""

from __future__ import annotations

import argparse
import concurrent.futures
import csv
import difflib
import fcntl
import hashlib
import json
import math
import os
import random
import re
import shutil
import signal
import subprocess
import sys
import tempfile
import time
import tomllib
from contextlib import contextmanager
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Iterable

from homm2.permute.tu_state_metrics import read_coff
from homm2.build.assert_relocs import load_symbols
from homm2.build.canonicalize_data_symbols import (
    CoffObject,
    CompgenDataClaim,
    _definitions,
    canonicalize_coff,
    load_compgen_claims,
    load_compgen_data_claims,
)
from homm2.build.canonicalize_relocs import (
    canonicalize_unit,
    function_inventory,
    load_retail_symbols,
)
from homm2.analysis.disasm import _branch_kind, _cfg


IMAGE_BASE = 0x400000
DEFAULT_FAMILIES = ("forest",)
ALL_FAMILIES = (
    "forest", "typedef", "enum", "struct", "class", "packed", "member",
    "extern", "static-data", "prototype", "function", "include", "mixed",
)
SAFE_ENUM_VALUES = (-32768, -1, 0, 1, 2, 7, 31, 255, 256, 1024, 32767, 65535)
SAFE_SCALAR_TYPES = ("char", "unsigned char", "short", "unsigned short", "int", "unsigned long")
SAFE_CALLING_CONVENTIONS = ("__cdecl", "__fastcall", "__stdcall")
CURATED_INCLUDES = (
    "<stddef.h>", "<limits.h>", "<string.h>", "<stdlib.h>",
    "<va.h>", "<Ints.h>", "<windows.h>", "<BASE/bitmap.h>",
    "<BASE/IconEntry.h>", "<BASE/WINMGR.h>",
)
DEFAULT_COMPILE_TIMEOUT_SECONDS = 120.0
DEFAULT_MAX_DECLARATIONS = 64
DEFAULT_MIN_FOREST_WIDTH = 10
PROCESS_GROUP_TERMINATION_GRACE_SECONDS = 1.0


class BaselineUpdateError(ValueError):
    pass


class SourceMutationError(RuntimeError):
    pass


def sha256_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def parse_int(value: str) -> int:
    try:
        return int(value, 0)
    except ValueError as exc:
        raise argparse.ArgumentTypeError(f"invalid integer: {value}") from exc


def positive_seconds(value: str) -> float:
    try:
        seconds = float(value)
    except ValueError as exc:
        raise argparse.ArgumentTypeError(f"invalid timeout: {value}") from exc
    if not math.isfinite(seconds) or seconds <= 0:
        raise argparse.ArgumentTypeError("timeout must be a positive finite number")
    return seconds


def positive_count(value: str) -> int:
    count = parse_int(value)
    if count <= 0:
        raise argparse.ArgumentTypeError("count must be positive")
    return count


@dataclass(frozen=True)
class Target:
    source: Path
    unit: str
    rva: int
    va: int
    symbol: str
    retail_size: int
    marker_offset: int
    insertion_offset: int
    logical_line: int


@dataclass(frozen=True)
class Variant:
    trial: int
    family: str
    tag: str
    body: str
    permutation: tuple[str, ...] = ()

    def block(self, logical_line: int) -> str:
        # Restore the pre-existing logical line before any authored source token.
        # No filename is supplied, so an earlier #line filename remains unchanged.
        return f"{self.body}#line {logical_line}\n"


def make_declaration_forest(
    rng: random.Random,
    ident: str,
    width: int,
) -> tuple[str, tuple[str, ...]]:
    """Build a broad, independently permuted parser-visible declaration surface."""
    atoms = []

    typedef_shapes = (
        lambda name, scalar, index: f"typedef {scalar} {name};\n",
        lambda name, scalar, index: f"typedef {scalar} *{name};\n",
        lambda name, scalar, index: (
            f"typedef {scalar} {name}[{2 + index % 7}];\n"
        ),
        lambda name, scalar, index: (
            f"typedef {scalar} (__cdecl *{name})(int, unsigned long);\n"
        ),
        lambda name, scalar, index: (
            f"typedef const {scalar} *{name};\n"
        ),
    )
    for index in range(width):
        name = f"{ident}_FOREST_TYPEDEF_{index}"
        scalar = rng.choice(SAFE_SCALAR_TYPES)
        shape = rng.randrange(len(typedef_shapes))
        atoms.append((
            f"typedef:{shape}:{index}",
            typedef_shapes[shape](name, scalar, index),
        ))

    for index in range(width):
        name = f"{ident}_FOREST_CLASS_{index}"
        scalar = rng.choice(SAFE_SCALAR_TYPES)
        constant = rng.choice((1, 2, 3, 7, 15, 31))
        shape = rng.randrange(10)
        if shape == 0:
            body = (
                f"class {name} {{\n"
                f"public:\n"
                f"    {scalar} m_value;\n"
                f"    int ProbeRead(int value);\n"
                f"}};\n"
            )
        elif shape == 1:
            body = (
                f"class {name} {{\n"
                f"private:\n"
                f"    {scalar} m_value;\n"
                f"public:\n"
                f"    int ProbeIdentity(int value) {{ return value; }}\n"
                f"protected:\n"
                f"    unsigned long m_state;\n"
                f"}};\n"
            )
        elif shape == 2:
            body = (
                f"class {name} {{\n"
                f"public:\n"
                f"    typedef {scalar} ProbeValue;\n"
                f"    enum ProbeKind {{ PROBE_ZERO = 0, PROBE_LIMIT = {constant} }};\n"
                f"    ProbeValue m_values[{2 + index % 4}];\n"
                f"}};\n"
            )
        elif shape == 3:
            body = (
                f"class {name} {{\n"
                f"public:\n"
                f"    static {scalar} s_value;\n"
                f"    static int ProbeStatic(int value);\n"
                f"    int ProbeMember(unsigned long value) const;\n"
                f"}};\n"
            )
        elif shape == 4:
            body = (
                f"class {name} {{\n"
                f"public:\n"
                f"    virtual int ProbeVirtual(int value);\n"
                f"    virtual unsigned long ProbeWide(unsigned long value);\n"
                f"}};\n"
            )
        elif shape == 5:
            body = (
                f"class {name} {{\n"
                f"public:\n"
                f"    int ProbeOverload(int value);\n"
                f"    int ProbeOverload(unsigned long value);\n"
                f"    int ProbeOverload(const char *value);\n"
                f"}};\n"
            )
        elif shape == 6:
            body = (
                f"class {name} {{\n"
                f"public:\n"
                f"    {name}();\n"
                f"    ~{name}();\n"
                f"    {scalar} ProbeConvert({scalar} value) "
                f"{{ return value; }}\n"
                f"}};\n"
            )
        elif shape == 7:
            body = (
                f"class {name} {{\n"
                f"private:\n"
                f"    unsigned int m_low : {1 + index % 7};\n"
                f"    unsigned int m_high : {1 + (index + 3) % 7};\n"
                f"public:\n"
                f"    int ProbeBits() const;\n"
                f"}};\n"
            )
        elif shape == 8:
            body = (
                f"class {name} {{\n"
                f"public:\n"
                f"    union ProbeUnion {{ int i; unsigned long u; }};\n"
                f"    ProbeUnion m_value;\n"
                f"    {scalar} *m_pointer;\n"
                f"}};\n"
            )
        else:
            pack_value = (1, 2, 4, 8)[index % 4]
            body = (
                f"#pragma pack(push, {pack_value})\n"
                f"class {name} {{\n"
                f"public:\n"
                f"    char m_tag;\n"
                f"    {scalar} m_value;\n"
                f"    int ProbePacked(int value) {{ return value ^ {constant}; }}\n"
                f"}};\n"
                f"#pragma pack(pop)\n"
            )
        atoms.append((f"class:{shape}:{index}", body))

    prototype_shapes = (
        lambda name, convention, scalar: (
            f"{scalar} {convention} {name}({scalar} value);\n"
        ),
        lambda name, convention, scalar: (
            f"int {convention} {name}(int left, unsigned long right);\n"
        ),
        lambda name, convention, scalar: (
            f"{scalar} *{convention} {name}({scalar} *value, unsigned int count);\n"
        ),
        lambda name, convention, scalar: (
            f"void {convention} {name}(const {scalar} *first, const {scalar} *last);\n"
        ),
    )
    for index in range(width):
        name = f"{ident}_FOREST_PROTOTYPE_{index}"
        convention = rng.choice(SAFE_CALLING_CONVENTIONS)
        scalar = rng.choice(SAFE_SCALAR_TYPES)
        shape = rng.randrange(len(prototype_shapes))
        atoms.append((
            f"prototype:{shape}:{index}",
            prototype_shapes[shape](name, convention, scalar),
        ))

    function_shapes = (
        lambda name, convention, constant: (
            f"static int {convention} {name}(int value) "
            f"{{ return value; }}\n"
        ),
        lambda name, convention, constant: (
            f"static int {convention} {name}(int value) "
            f"{{ return value ^ {constant}; }}\n"
        ),
        lambda name, convention, constant: (
            f"static unsigned long {convention} {name}"
            f"(unsigned long left, unsigned long right) "
            f"{{ return (left + right) ^ {constant}UL; }}\n"
        ),
        lambda name, convention, constant: (
            f"static int {convention} {name}(int left, int right) "
            f"{{ return left < right ? left + {constant} : right - {constant}; }}\n"
        ),
        lambda name, convention, constant: (
            f"static unsigned short {convention} {name}(unsigned short value) "
            f"{{ return value | {constant}; }}\n"
        ),
    )
    for index in range(width):
        name = f"{ident}_FOREST_FUNCTION_{index}"
        convention = rng.choice(SAFE_CALLING_CONVENTIONS)
        constant = rng.choice((1, 2, 3, 7, 15, 31, 63, 127))
        shape = rng.randrange(len(function_shapes))
        atoms.append((
            f"function:{shape}:{index}",
            function_shapes[shape](name, convention, constant),
        ))

    rng.shuffle(atoms)
    permutation = tuple(label for label, _body in atoms)
    return "".join(body for _label, body in atoms), permutation


def load_units(root: Path) -> dict[str, dict]:
    raw = tomllib.loads((root / "config/units.toml").read_text())
    profiles = raw.get("flags", {})
    out = {}
    for unit in raw.get("unit", []):
        out[unit["unit"]] = {
            "source": Path(unit["source"]),
            "flags": list(profiles[unit.get("flags", "base")]),
            "profile": unit.get("flags", "base"),
        }
    return out


def _normalize_rva(value: int) -> int:
    return value - IMAGE_BASE if value >= IMAGE_BASE else value


def _leading_metadata_offset(text: str, marker_offset: int) -> int:
    """Include contiguous blank/comment metadata immediately before a VA marker."""
    lines = text[:marker_offset].splitlines(keepends=True)
    offset = marker_offset
    for line in reversed(lines):
        stripped = line.strip()
        if not stripped or stripped.startswith("//"):
            offset -= len(line)
            continue
        break
    return offset


def _top_level_insertion_offset(text: str) -> int:
    """Return the first authored token after the leading preprocessor/include block.

    A directive line ending in a backslash continues on the next physical
    line; the continuation belongs to the directive even though it does not
    start with ``#``. Splitting such a macro with a probe corrupts the TU.
    """
    offset = 0
    continuation = False
    for line in text.splitlines(keepends=True):
        stripped = line.strip()
        if continuation or not stripped or stripped.startswith("#"):
            offset += len(line)
            continuation = stripped.endswith("\\")
            continue
        break
    return offset


_LINE_DIRECTIVE = re.compile(r'^\s*#\s*line\s+(\d+)(?:\s+"[^"]*")?')
_INCLUDE_DIRECTIVE = re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]', re.M)
_DEFINE_DIRECTIVE = re.compile(r'^\s*#\s*define\s+([A-Za-z_]\w*)', re.M)
_IDENTIFIER = re.compile(r'\b[A-Za-z_]\w*\b')
_VA_MARKER_LINE = re.compile(r'^[ \t]*VA\(0x[0-9a-f]+,', re.M | re.I)
_COMPILER_PRIVATE_COUNTER = re.compile(r'(\$(?:SG|T))\d+')


def logical_line_at(text: str, offset: int) -> int:
    """Return the logical line number of the source line beginning at *offset*."""
    logical = 1
    for line in text[:offset].splitlines(keepends=True):
        match = _LINE_DIRECTIVE.match(line)
        logical = int(match.group(1)) if match else logical + 1
    return logical


def target_identifiers(text: str, target: Target) -> set[str]:
    """Conservative identifier census for the canonical target's VA-delimited block."""
    next_marker = _VA_MARKER_LINE.search(text, target.marker_offset + 1)
    block = text[target.marker_offset : next_marker.start() if next_marker else len(text)]
    return set(_IDENTIFIER.findall(block))


def target_suffix_digest(text: str, va: int) -> str | None:
    """Hash the exact authored suffix beginning at one real target VA marker.

    Noise is inserted before that marker, so suffix identity is a stronger and much
    cheaper per-trial invariant than recomputing normalized hashes for the whole tree.
    """
    marker_pattern = re.compile(rf"^[ \t]*(VA\(0x{va:08x},)", re.M | re.I)
    matches = list(marker_pattern.finditer(text))
    if len(matches) != 1:
        return None
    return sha256_bytes(text[matches[0].start(1) :].encode("utf-8"))


def _case_insensitive_child(parent: Path, relative: str) -> Path | None:
    current = parent
    for part in Path(relative).parts:
        direct = current / part
        if direct.exists():
            current = direct
            continue
        if not current.is_dir():
            return None
        match = next((entry for entry in current.iterdir() if entry.name.lower() == part.lower()), None)
        if match is None:
            return None
        current = match
    return current if current.is_file() else None


def include_macro_guard(root: Path, probe_body: str, target_tokens: set[str]) -> dict:
    """Fail closed if curated includes can macro-rewrite an identifier in the target block."""
    requested = _INCLUDE_DIRECTIVE.findall(probe_body)
    if not requested:
        return {"checked": False, "headers": [], "macro_conflicts": []}
    allowed = {header[1:-1] for header in CURATED_INCLUDES}
    include_roots = [root / "include", root / "build/toolchain/msvc/include"]
    vendor = root / "vendor"
    if vendor.is_dir():
        include_roots.extend(sorted(path for path in vendor.iterdir() if path.is_dir()))
    pending = [(None, header) for header in requested]
    visited: set[Path] = set()
    macros: set[str] = set()
    unresolved = []
    while pending:
        owner, header = pending.pop()
        if owner is None and header not in allowed:
            unresolved.append(f"not allowlisted: {header}")
            continue
        search_roots = ([owner.parent] if owner is not None else []) + include_roots
        resolved = next(
            (candidate for base in search_roots if (candidate := _case_insensitive_child(base, header))),
            None,
        )
        if resolved is None:
            unresolved.append(header)
            continue
        resolved = resolved.resolve()
        if resolved in visited:
            continue
        visited.add(resolved)
        text = resolved.read_text(errors="replace")
        macros.update(_DEFINE_DIRECTIVE.findall(text))
        pending.extend((resolved, child) for child in _INCLUDE_DIRECTIVE.findall(text))
    conflicts = sorted(macros & target_tokens)
    return {
        "checked": True,
        "headers": requested,
        "transitive_header_count": len(visited),
        "defined_macro_count": len(macros),
        "macro_conflicts": conflicts,
        "unresolved_headers": sorted(set(unresolved)),
        "passed": not conflicts and not unresolved,
    }


def resolve_target(root: Path, source_arg: Path, requested_rva: int) -> tuple[Target, list[str]]:
    source = (root / source_arg).resolve() if not source_arg.is_absolute() else source_arg.resolve()
    try:
        source_rel = source.relative_to(root)
    except ValueError as exc:
        raise ValueError(f"source must be inside the worktree: {source}") from exc
    units = load_units(root)
    matches = [(name, cfg) for name, cfg in units.items() if cfg["source"] == source_rel]
    if len(matches) != 1:
        raise ValueError(f"source is not the unique source of a configured unit: {source_rel}")
    unit, cfg = matches[0]
    rva = _normalize_rva(requested_rva)
    with (root / "build/gen/symbol_names.csv").open(newline="") as handle:
        rows = [
            row for row in csv.DictReader(handle)
            if row["kind"] == "func" and row["unit"] == unit and int(row["rva"], 0) == rva
        ]
    if len(rows) != 1:
        raise ValueError(f"RVA 0x{rva:x} is not a unique inventory function in {unit}")
    row = rows[0]
    text = source.read_text()
    va = rva + IMAGE_BASE
    marker = f"VA(0x{va:08x},"
    marker_pattern = re.compile(
        rf"^[ \t]*(VA\(0x{va:08x},)",
        re.M | re.I,
    )
    positions = [match.start(1) for match in marker_pattern.finditer(text)]
    if len(positions) != 1:
        raise ValueError(f"expected one source marker {marker}, found {len(positions)}")
    marker_offset = positions[0]
    insertion_offset = _leading_metadata_offset(text, marker_offset)
    return Target(
        source=source,
        unit=unit,
        rva=rva,
        va=va,
        symbol=row["name"],
        retail_size=int(row["size"], 0),
        marker_offset=marker_offset,
        insertion_offset=insertion_offset,
        logical_line=logical_line_at(text, insertion_offset),
    ), cfg["flags"]


def make_variants(
    count: int,
    families: Iterable[str],
    seed: int,
    max_declarations: int = DEFAULT_MAX_DECLARATIONS,
) -> list[Variant]:
    selected = tuple(families)
    unknown = sorted(set(selected) - set(ALL_FAMILIES))
    if unknown:
        raise ValueError(f"unknown noise families: {', '.join(unknown)}")
    if not selected:
        raise ValueError("at least one noise family is required")
    if (
        {"forest", "mixed"} & set(selected)
        and max_declarations < DEFAULT_MIN_FOREST_WIDTH
    ):
        raise ValueError(
            "forest probes require --max-declarations of at least "
            f"{DEFAULT_MIN_FOREST_WIDTH}"
        )
    rng = random.Random(seed)
    variants = []
    for trial in range(1, count + 1):
        family = selected[(trial - 1) % len(selected)]
        tag = f"{seed:08x}-{trial:04d}-{rng.getrandbits(32):08x}"
        ident = f"HOMM2_TU_STATE_PROBE_{tag.replace('-', '_').upper()}"
        # Walk the complete declaration-train range instead of sampling only a
        # few short blocks. Old MSVC front-end state can remain unchanged for
        # several declarations and then flip at a later counter value.
        repeat = 1 + ((trial - 1) % max_declarations)
        aliases = "".join(
            f"typedef {rng.choice(SAFE_SCALAR_TYPES)} {ident}_ALIAS_{i};\n"
            for i in range(repeat)
        )

        enum_count = 1 + rng.randrange(8)
        enum_values = [rng.choice(SAFE_ENUM_VALUES) for _ in range(enum_count)]
        rng.shuffle(enum_values)
        enumerators = ",\n".join(
            f"    {ident}_ENUM_VALUE_{i} = {value}" for i, value in enumerate(enum_values)
        )
        enum_decl = (
            f"typedef enum {ident}_ENUM_TAG {{\n"
            f"{enumerators}\n"
            f"}} {ident}_ENUM;\n"
        )

        member_count = 1 + rng.randrange(6)
        record_members = "".join(
            f"    {rng.choice(SAFE_SCALAR_TYPES)} m_probe_{i}"
            f"{'[' + str(1 + rng.randrange(4)) + ']' if rng.randrange(3) == 0 else ''};\n"
            for i in range(member_count)
        )
        struct_decl = (
            f"struct {ident}_STRUCT {{\n"
            f"{record_members}"
            f"}};\n"
        )
        class_decl = (
            f"class {ident}_CLASS {{\n"
            f"public:\n{record_members}"
            f"}};\n"
        )

        pack_value = rng.choice((1, 2, 4, 8))
        packed_members = "".join(
            f"    {rng.choice(SAFE_SCALAR_TYPES)} m_packed_{i};\n"
            for i in range(2 + rng.randrange(5))
        )
        packed_decl = (
            f"#pragma pack(push, {pack_value})\n"
            f"struct {ident}_PACKED {{\n{packed_members}}};\n"
            f"#pragma pack(pop)\n"
        )

        method_count = 1 + rng.randrange(3)
        method_decls = "".join(
            f"    int ProbeDecl_{i}(int value);\n" for i in range(method_count)
        )
        body_constant = rng.choice((0, 1, 3, 7, 15, 31))
        member_decl = (
            f"class {ident}_MEMBERS {{\n"
            f"public:\n{method_decls}"
            f"    int ProbeIdentity(int value) {{ return value; }}\n"
            f"    unsigned long ProbeMix(unsigned long value) {{ return value ^ {body_constant}UL; }}\n"
            f"}};\n"
        )

        extern_decl = "".join(
            f"extern {rng.choice(SAFE_SCALAR_TYPES)} {ident}_EXTERN_{i};\n"
            for i in range(repeat)
        )
        static_values = [rng.choice(SAFE_ENUM_VALUES) for _ in range(repeat)]
        static_data = "".join(
            f"static {rng.choice(SAFE_SCALAR_TYPES)} {ident}_STATIC_{i} = {value};\n"
            for i, value in enumerate(static_values)
        )
        prototype_decl = "".join(
            f"int __fastcall {ident}_PROTOTYPE_{i}(int left, int right);\n"
            for i in range(repeat)
        )
        function_constant = rng.choice((0, 1, 3, 7, 15, 31, 63, 127))
        function_defs = (
            f"static int {ident}_FUNCTION_A(int value) {{ return value ^ {function_constant}; }}\n"
            f"static unsigned long {ident}_FUNCTION_B(unsigned long left, unsigned long right) "
            f"{{ return (left + right) ^ {function_constant}UL; }}\n"
        )
        include_count = 1 + rng.randrange(min(4, len(CURATED_INCLUDES)))
        include_choices = list(CURATED_INCLUDES)
        rng.shuffle(include_choices)
        includes = (
            "".join(f"#include {header}\n" for header in include_choices[:include_count])
            + f"typedef int {ident}_INCLUDE_MARKER;\n"
        )
        declaration_forest = ""
        forest_permutation = ()
        if family in ("forest", "mixed"):
            forest_width = DEFAULT_MIN_FOREST_WIDTH + (
                (trial - 1) % (max_declarations - DEFAULT_MIN_FOREST_WIDTH + 1)
            )
            declaration_forest, forest_permutation = make_declaration_forest(
                rng, ident, forest_width
            )
        bodies = {
            "forest": declaration_forest,
            "typedef": aliases,
            "enum": enum_decl,
            "struct": struct_decl,
            "class": class_decl,
            "packed": packed_decl,
            "member": member_decl,
            "extern": extern_decl,
            "static-data": static_data,
            "prototype": prototype_decl,
            "function": function_defs,
            "include": includes,
            "mixed": (
                includes + declaration_forest + enum_decl + struct_decl + class_decl
                + packed_decl + member_decl + extern_decl + static_data
            ),
        }
        variants.append(Variant(
            trial,
            family,
            tag,
            bodies[family],
            forest_permutation if family in ("forest", "mixed") else (),
        ))
    return variants


def select_variants(
    variants: list[Variant],
    requested_trials: Iterable[int] | None,
    generation_horizon: int,
) -> list[Variant]:
    requested = set(requested_trials or ())
    if not requested:
        return variants
    unavailable = sorted(trial for trial in requested if trial > generation_horizon)
    if unavailable:
        raise ValueError(
            "--only-trial exceeds --trials generation horizon: "
            + ", ".join(str(trial) for trial in unavailable)
        )
    return [variant for variant in variants if variant.trial in requested]


def load_layer_body(
    summary_path: Path,
    trial: int,
    target: Target,
    insertion: str,
) -> tuple[str, dict]:
    """Recover one audited representative from an earlier state census."""
    summary = json.loads(summary_path.read_text())
    summary_target = summary.get("target", {})
    expected = {
        "unit": target.unit,
        "rva": f"0x{target.rva:x}",
        "symbol": target.symbol,
    }
    actual = {
        "unit": summary_target.get("unit"),
        "rva": str(summary_target.get("rva", "")).lower(),
        "symbol": summary_target.get("symbol"),
    }
    if actual != expected:
        raise ValueError(
            "layer state summary targets "
            f"{actual['unit']}::{actual['symbol']}@{actual['rva']}, expected "
            f"{expected['unit']}::{expected['symbol']}@{expected['rva']}"
        )
    if summary.get("insertion") != insertion:
        raise ValueError(
            "layer state summary insertion "
            f"{summary.get('insertion')!r} does not match {insertion!r}"
        )
    matches = [
        state
        for state in summary.get("states", [])
        if (state.get("representative") or {}).get("trial") == trial
    ]
    if len(matches) != 1:
        raise ValueError(
            f"layer trial {trial} has {len(matches)} representative states in {summary_path}"
        )
    representative = matches[0]["representative"]
    body = representative.get("body")
    if not isinstance(body, str) or not body.strip():
        raise ValueError(f"layer trial {trial} has no parser-visible body")
    if not body.endswith("\n"):
        body += "\n"
    return body, {
        "summary": str(summary_path),
        "trial": trial,
        "state": matches[0].get("state"),
        "score": matches[0].get("scores", [None])[0],
        "family": representative.get("family"),
        "tag": representative.get("tag"),
        "body_sha256": sha256_bytes(body.encode("utf-8")),
    }


def insert_variant(
    original: str,
    target: Target,
    variant: Variant,
    insertion: str = "target",
    layer_body: str = "",
) -> str:
    insertion_offset = (
        target.insertion_offset
        if insertion == "target"
        else _top_level_insertion_offset(original)
    )
    block = layer_body + variant.block(logical_line_at(original, insertion_offset))
    return original[:insertion_offset] + block + original[insertion_offset:]


@contextmanager
def temporary_source(path: Path, original: bytes, candidate: bytes):
    """Expose one candidate while refusing to overwrite an unexpected source edit."""
    if path.read_bytes() != original:
        raise SourceMutationError(f"source changed before probe write: {path}")
    path.write_bytes(candidate)
    try:
        yield
    finally:
        if path.read_bytes() != candidate:
            raise SourceMutationError(
                f"source changed during probe; refusing stale restoration: {path}"
            )
        path.write_bytes(original)


def acquire_source_mutation_lock(root: Path, source: Path):
    """Hold an exclusive non-blocking lock for one source-mutating search run."""
    lock_root = root / "build/tu-state-noise/.locks"
    lock_root.mkdir(parents=True, exist_ok=True)
    identity = hashlib.sha256(str(source.resolve()).encode("utf-8")).hexdigest()
    lock_path = lock_root / f"{identity}.lock"
    handle = lock_path.open("a+")
    try:
        fcntl.flock(handle.fileno(), fcntl.LOCK_EX | fcntl.LOCK_NB)
    except BlockingIOError as exc:
        handle.seek(0)
        owner = handle.read().strip() or "unknown owner"
        handle.close()
        raise SourceMutationError(
            f"another source-variant process owns {source}: {owner}"
        ) from exc
    handle.seek(0)
    handle.truncate()
    handle.write(f"pid={os.getpid()} source={source}\n")
    handle.flush()
    return handle


@contextmanager
def measure_stage(timings: dict[str, dict[str, float | int]], stage: str):
    """Accumulate monotonic wall time for one diagnostic pipeline stage."""
    started = time.perf_counter()
    try:
        yield
    finally:
        entry = timings.setdefault(stage, {"seconds": 0.0, "calls": 0})
        entry["seconds"] += time.perf_counter() - started
        entry["calls"] += 1


def format_timings(timings: dict[str, dict[str, float | int]]) -> str:
    fields = []
    for stage, entry in timings.items():
        seconds = float(entry["seconds"])
        calls = int(entry["calls"])
        fields.append(f"{stage}={seconds:.3f}s/{calls} ({seconds / calls:.3f}s avg)")
    return "timings: " + "; ".join(fields)


def staged_artifact_path(requested: Path, scratch_path: Path, final_path: Path) -> Path:
    """Write paths inside the retained directory through its temporary staging tree."""
    try:
        relative = requested.relative_to(final_path)
    except ValueError:
        return requested
    return scratch_path / relative


def finalize_compiled_artifacts(
    scratch: tempfile.TemporaryDirectory,
    scratch_path: Path,
    final_path: Path,
    preserve_artifacts: bool,
) -> Path | None:
    """Delete a compiled run by default, or atomically retain its reviewed artifacts."""
    if preserve_artifacts:
        scratch_path.rename(final_path)
        scratch.cleanup()
        return final_path
    scratch.cleanup()
    return None


def preserve_comparison_objects(
    output: Path,
    raw_candidate: Path,
    normalized_candidate: Path,
    normalized_retail: Path,
    prefix: str,
) -> None:
    """Replace one retained comparison triple while all inputs still exist."""
    shutil.copy2(raw_candidate, output / f"{prefix}.raw.obj")
    shutil.copy2(normalized_candidate, output / f"{prefix}.candidate.obj")
    shutil.copy2(normalized_retail, output / f"{prefix}.retail.obj")


def disassemble_symbol(path: Path, symbol: str) -> str:
    result = subprocess.run(
        [
            "llvm-objdump",
            "-dr",
            f"--disassemble-symbols={symbol}",
            str(path),
        ],
        capture_output=True,
        text=True,
    )
    assembly = result.stdout + result.stderr
    if result.returncode:
        raise RuntimeError(
            f"llvm-objdump failed for retained object {path} ({result.returncode})"
        )
    return assembly


def write_comparison_disassembly(
    output: Path, symbol: str, prefix: str
) -> dict[str, str]:
    """Disassemble one retained candidate and its paired retail target."""
    paths = {
        "candidate_object": f"{prefix}.candidate.obj",
        "retail_object": f"{prefix}.retail.obj",
        "candidate_assembly": f"{prefix}.candidate.asm",
        "retail_assembly": f"{prefix}.retail.asm",
        "assembly_diff": f"{prefix}.diff",
    }
    assemblies = {}
    for side in ("candidate", "retail"):
        assembly = disassemble_symbol(
            output / paths[f"{side}_object"], symbol
        )
        assemblies[side] = assembly
        (output / paths[f"{side}_assembly"]).write_text(assembly)
    diff = difflib.unified_diff(
        assemblies["retail"].splitlines(keepends=True),
        assemblies["candidate"].splitlines(keepends=True),
        fromfile="retail",
        tofile="candidate",
    )
    (output / paths["assembly_diff"]).write_text("".join(diff))
    return paths


def cfg_outgoing_edges(term: str | None) -> list[tuple[str, int]]:
    return [
        (match.group(1), int(match.group(2)))
        for match in re.finditer(r"(jcc|jmp|fall) B(\d+)", term or "")
    ]


def canonical_cfg_signature(cfg) -> tuple:
    """Canonicalize a labeled CFG by entry-rooted edge traversal.

    Address-order block labels are disposable. Edge labels are not: conditional,
    fallthrough, and jump entries remain distinct, and incoming edges are recorded
    explicitly alongside their corresponding successors.
    """
    if not cfg:
        return ()
    mapping = {}
    order = []

    def visit_component(seed: int) -> None:
        queue = [seed]
        if seed not in mapping:
            mapping[seed] = len(mapping)
        while queue:
            source = queue.pop(0)
            order.append(source)
            for _kind, target in cfg_outgoing_edges(cfg[source][2]):
                if target not in mapping:
                    mapping[target] = len(mapping)
                    queue.append(target)

    visit_component(0)
    for seed in range(len(cfg)):
        if seed not in mapping:
            visit_component(seed)

    outgoing = [[] for _ in cfg]
    incoming = [[] for _ in cfg]
    terminal = []
    for source in order:
        term = cfg[source][2] or ""
        edges = cfg_outgoing_edges(term)
        canonical_source = mapping[source]
        for kind, target in edges:
            canonical_target = mapping[target]
            outgoing[canonical_source].append((kind, canonical_target))
            incoming[canonical_target].append((kind, canonical_source))
        if edges:
            terminal.append(tuple(kind for kind, _target in edges))
        elif term.startswith("ret"):
            terminal.append(("ret",))
        elif term == "jmp <ext>":
            terminal.append(("jmp-ext",))
        elif term:
            terminal.append((term,))
        else:
            terminal.append(())
    return tuple(
        (
            terminal[index],
            tuple(outgoing[index]),
            tuple(sorted(incoming[index])),
        )
        for index in range(len(cfg))
    )


def signature_edge_delta(candidate: tuple, retail: tuple) -> int:
    from collections import Counter

    def edges(signature: tuple) -> Counter:
        return Counter(
            (source, kind, target)
            for source, (_terminal, outgoing, _incoming) in enumerate(signature)
            for kind, target in outgoing
        )

    candidate_edges = edges(candidate)
    retail_edges = edges(retail)
    return sum((candidate_edges - retail_edges).values()) + sum(
        (retail_edges - candidate_edges).values()
    )


def signature_predecessor_delta(candidate: tuple, retail: tuple) -> int:
    delta = abs(len(candidate) - len(retail))
    for index in range(min(len(candidate), len(retail))):
        if candidate[index][2] != retail[index][2]:
            delta += 1
    return delta


def structural_frontier(candidate_cfg: list, retail_cfg: list) -> dict[str, int | str | None]:
    """Measure the exact flow-and-instruction-count prefix of two CFGs.

    This is an experimental clue metric, not an exactness criterion. It answers
    how far block-by-block reconstruction proceeds before the first structural
    disagreement, even when a lower fuzzy score reaches farther than MAX.
    """
    leading_blocks = 0
    leading_instructions = 0
    shared_blocks = min(len(candidate_cfg), len(retail_cfg))
    for index in range(shared_blocks):
        candidate_body, candidate_term = candidate_cfg[index][1:]
        retail_body, retail_term = retail_cfg[index][1:]
        if candidate_term != retail_term:
            kind = (
                "target_shift"
                if _branch_kind(candidate_term, index)
                == _branch_kind(retail_term, index)
                else "flow_kind"
            )
            return {
                "leading_exact_blocks": leading_blocks,
                "leading_exact_instructions": leading_instructions,
                "first_structural_divergence": index,
                "first_structural_divergence_kind": kind,
            }
        if len(candidate_body) != len(retail_body):
            return {
                "leading_exact_blocks": leading_blocks,
                "leading_exact_instructions": leading_instructions,
                "first_structural_divergence": index,
                "first_structural_divergence_kind": "size_only",
            }
        leading_blocks += 1
        leading_instructions += len(retail_body)
    if len(candidate_cfg) != len(retail_cfg):
        return {
            "leading_exact_blocks": leading_blocks,
            "leading_exact_instructions": leading_instructions,
            "first_structural_divergence": shared_blocks,
            "first_structural_divergence_kind": "missing",
        }
    return {
        "leading_exact_blocks": leading_blocks,
        "leading_exact_instructions": leading_instructions,
        "first_structural_divergence": None,
        "first_structural_divergence_kind": None,
    }


def cfg_topology_metrics(
    candidate_text: str, retail_text: str
) -> dict[str, int | bool | str | None]:
    """Measure CFG proximity independently from byte-level fuzzy similarity."""
    candidate_cfg = _cfg(candidate_text)
    retail_cfg = _cfg(retail_text)
    candidate_signature = canonical_cfg_signature(candidate_cfg)
    retail_signature = canonical_cfg_signature(retail_cfg)
    counts = {
        "exact": 0,
        "size_only": 0,
        "target_shift": 0,
        "flow_kind": 0,
        "missing": abs(len(candidate_cfg) - len(retail_cfg)),
    }
    for index in range(min(len(candidate_cfg), len(retail_cfg))):
        candidate_body, candidate_term = candidate_cfg[index][1:]
        retail_body, retail_term = retail_cfg[index][1:]
        candidate_kind = _branch_kind(candidate_term, index)
        retail_kind = _branch_kind(retail_term, index)
        if candidate_term == retail_term:
            if len(candidate_body) == len(retail_body):
                counts["exact"] += 1
            else:
                counts["size_only"] += 1
        elif candidate_kind == retail_kind:
            counts["target_shift"] += 1
        else:
            counts["flow_kind"] += 1
    return {
        "candidate_blocks": len(candidate_cfg),
        "retail_blocks": len(retail_cfg),
        "block_count_delta": abs(len(candidate_cfg) - len(retail_cfg)),
        "graph_exact": candidate_signature == retail_signature,
        "labeled_edge_delta": signature_edge_delta(
            candidate_signature, retail_signature
        ),
        "predecessor_delta": signature_predecessor_delta(
            candidate_signature, retail_signature
        ),
        "candidate_graph_sha": sha256_bytes(
            repr(candidate_signature).encode("utf-8")
        )[:16],
        "retail_graph_sha": sha256_bytes(
            repr(retail_signature).encode("utf-8")
        )[:16],
        **structural_frontier(candidate_cfg, retail_cfg),
        **counts,
        "flow_exact": (
            len(candidate_cfg) == len(retail_cfg)
            and counts["target_shift"] == 0
            and counts["flow_kind"] == 0
        ),
    }


def topology_rank(metrics: dict[str, int | bool | str], score: float) -> tuple:
    """Lower is better; fuzzy score breaks otherwise identical CFG ties."""
    return (
        metrics["block_count_delta"],
        metrics["labeled_edge_delta"],
        metrics["predecessor_delta"],
        metrics["flow_kind"],
        metrics["target_shift"],
        metrics["size_only"],
        -metrics["exact"],
        -score,
    )


def structural_frontier_rank(
    metrics: dict[str, int | bool | str | None], score: float
) -> tuple:
    """Lower is better; rank the farthest exact structural prefix first."""
    return (
        -int(metrics["leading_exact_blocks"]),
        -int(metrics["leading_exact_instructions"]),
        metrics["block_count_delta"],
        metrics["labeled_edge_delta"],
        metrics["predecessor_delta"],
        metrics["flow_kind"],
        metrics["target_shift"],
        metrics["size_only"],
        -metrics["exact"],
        -score,
    )


def object_topology_metrics(
    candidate_obj: Path, retail_obj: Path, symbol: str
) -> dict[str, int | bool | str]:
    return cfg_topology_metrics(
        disassemble_symbol(candidate_obj, symbol),
        disassemble_symbol(retail_obj, symbol),
    )


def _terminate_process_group(process: subprocess.Popen) -> tuple[str, str]:
    """Terminate and reap a process group created with ``start_new_session=True``."""
    try:
        os.killpg(process.pid, signal.SIGTERM)
    except ProcessLookupError:
        pass
    try:
        stdout, stderr = process.communicate(timeout=PROCESS_GROUP_TERMINATION_GRACE_SECONDS)
    except subprocess.TimeoutExpired:
        try:
            os.killpg(process.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass
        stdout, stderr = process.communicate()
    else:
        # The session leader may have exited while a descendant closed its inherited
        # pipes and ignored SIGTERM. Fail closed by killing any remaining group member.
        try:
            os.killpg(process.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass
    return stdout or "", stderr or ""


def _run_command_with_timeout(
    command: list[str], cwd: Path, timeout_seconds: float
) -> tuple[int | None, str, str, bool]:
    process = subprocess.Popen(
        command,
        cwd=cwd,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        start_new_session=True,
    )
    try:
        stdout, stderr = process.communicate(timeout=timeout_seconds)
        return process.returncode, stdout or "", stderr or "", False
    except subprocess.TimeoutExpired:
        stdout, stderr = _terminate_process_group(process)
        return process.returncode, stdout, stderr, True
    except BaseException:
        _terminate_process_group(process)
        raise


def compile_object(
    root: Path,
    source: Path,
    output: Path,
    flags: list[str],
    timeout_seconds: float,
) -> tuple[bool, str, bool]:
    """Compile one disposable probe object through the in-process cc_wrap.

    The shared toolchain/INCLUDE resolution and pure winepath spelling are
    prepared once per process, so a trial pays only the wine cl child itself.
    Probe objects never emit ninja depfiles. cc_wrap kills the compiler
    process group itself when the per-call timeout expires.
    """
    from homm2.build.cc_wrap import run_compile

    try:
        returncode, log, timed_out = run_compile(
            source, output, flags, depfile=False, cl_timeout=timeout_seconds
        )
    except RuntimeError as exc:
        return False, f"{exc}\n", False
    if timed_out:
        output.unlink(missing_ok=True)
        Path(str(output) + ".d").unlink(missing_ok=True)
        log += (
            f"\ncompile timed out after {timeout_seconds:g} seconds; "
            "terminated compiler process group\n"
        )
        return False, log, True
    return returncode == 0 and output.exists(), log, False


def objdiff_scores(
    target_obj: Path, candidate_obj: Path, symbol: str
) -> tuple[dict[str, float], dict[str, int], dict[str, int], str]:
    command = [
        "objdiff-cli", "diff", "-1", str(target_obj), "-2", str(candidate_obj), symbol,
        "-o", "-", "--format", "json",
    ]
    result = subprocess.run(command, capture_output=True, text=True)
    if result.returncode:
        return {}, {}, {}, result.stdout + result.stderr
    try:
        payload = json.loads(result.stdout)
    except json.JSONDecodeError:
        return {}, {}, {}, result.stdout + result.stderr
    entries = payload.get("right", {}).get("symbols", [])
    scores = {
        entry["name"]: float(entry["match_percent"])
        for entry in entries
        if isinstance(entry.get("name"), str) and entry.get("match_percent") is not None
    }
    sizes = {
        entry["name"]: int(str(entry["size"]), 0)
        for entry in entries
        if isinstance(entry.get("name"), str) and entry.get("size") is not None
    }
    counts = {}
    for entry in entries:
        name = entry.get("name")
        if isinstance(name, str):
            counts[name] = counts.get(name, 0) + 1
    return scores, sizes, counts, result.stderr


def object_metrics(path: Path) -> dict[str, dict]:
    _object_sha, rows = read_coff(path)
    return {
        row["function"]: {
            "size": row["size"],
            "text_sha": row["text_sha"],
            "text_hex": row["bytes"].hex(),
            "relocs": row["relocs"],
            "reloc_sha": row["reloc_sha"],
            "reloc_stream": row["reloc_stream"],
            "reloc_detail_sha": row["reloc_detail_sha"],
            "reloc_stream_complete": row["reloc_stream_complete"],
        }
        for row in rows
    }


def load_pairing_context(root: Path, unit: str) -> dict:
    public_data, function_rvas, function_sizes = load_retail_symbols(
        root / "build/gen/symbol_names.csv")
    previous_directory = Path.cwd()
    try:
        os.chdir(root)
        symbols, data, duplicates = load_symbols()
    finally:
        os.chdir(previous_directory)
    return {
        "unit": unit,
        "names": function_inventory(function_rvas).get(unit, set()),
        "public_data": public_data,
        "function_rvas": function_rvas,
        "function_sizes": function_sizes,
        "symbols": symbols,
        "data": data,
        "duplicates": duplicates,
        "compgen": load_compgen_claims(
            root / "build/gen/compiler_generated_functions.csv", unit
        ),
        "compgen_data": load_compgen_data_claims(
            root / "build/gen/delink_data_manifest.tsv", unit
        ),
    }


def normalize_comparison_pair(
    candidate_raw: Path,
    retail_raw: Path,
    prefix: Path,
    pairing: dict,
    target_symbol: str | None = None,
) -> tuple[Path, Path, list[Path]]:
    """Build the same candidate-paired normalized copies used by canonical objdiff."""
    paired_retail = prefix.with_suffix(".paired-retail.obj")
    normalized_candidate = prefix.with_suffix(".normalized-candidate.obj")
    normalized_retail = prefix.with_suffix(".normalized-retail.obj")
    shutil.copy2(retail_raw, paired_retail)
    canonicalize_unit(
        pairing["unit"],
        pairing["names"],
        pairing["public_data"],
        pairing["function_rvas"],
        pairing["function_sizes"],
        pairing["symbols"],
        pairing["data"],
        pairing["duplicates"],
        candidate_raw,
        paired_retail,
    )
    candidate_compgen_data, retail_compgen_data = target_compgen_data_claims(
        candidate_raw,
        paired_retail,
        target_symbol,
        pairing["compgen_data"],
    )
    normalized_candidate.write_bytes(
        canonicalize_coff(
            candidate_raw.read_bytes(),
            compgen=pairing["compgen"],
            compgen_data=candidate_compgen_data,
        ).data
    )
    normalized_retail.write_bytes(
        canonicalize_coff(
            paired_retail.read_bytes(),
            compgen=pairing["compgen"],
            compgen_data=retail_compgen_data,
        ).data
    )
    return normalized_candidate, normalized_retail, [
        paired_retail,
        normalized_candidate,
        normalized_retail,
    ]


def _function_relocations(coff: CoffObject, symbol_name: str):
    functions = [
        symbol for symbol in coff.symbols.values()
        if symbol.name == symbol_name and symbol.section > 0 and symbol.typ == 0x20
    ]
    if len(functions) != 1:
        raise ValueError(
            f"{symbol_name} has {len(functions)} external function definitions"
        )
    function = functions[0]
    following = [
        symbol.value for symbol in coff.symbols.values()
        if (
            symbol.section == function.section
            and symbol.typ == 0x20
            and symbol.storage_class == 2
            and symbol.value > function.value
        )
    ]
    end = min(following, default=coff.sections[function.section - 1].raw_size)
    return sorted(
        (
            relocation for relocation in coff.relocations
            if (
                relocation.section == function.section
                and function.value <= relocation.site < end
            )
        ),
        key=lambda relocation: (relocation.site, relocation.offset),
    )


def target_compgen_data_claims(
    candidate_raw: Path,
    paired_retail: Path,
    target_symbol: str | None,
    claims: tuple[CompgenDataClaim, ...],
) -> tuple[tuple[CompgenDataClaim, ...], tuple[CompgenDataClaim, ...]]:
    """Bind only reviewed compiler data referenced by the target function.

    Parser-visible probes may insert anonymous storage and move unrelated private
    definitions away from their reviewed manifest offsets. Pairing every TU-wide
    DATA_COMPGEN claim against such a disposable object would either bind the wrong
    datum or fail before the target can be scored. After retail pairing, a semantic
    claim name identifies the retail site. The reviewed payload then identifies the
    corresponding referenced candidate definition even when control-flow relocations
    differ or its physical offset moved.
    """
    if target_symbol is None or not claims:
        return claims, claims

    candidate = CoffObject(candidate_raw.read_bytes())
    retail = CoffObject(paired_retail.read_bytes())
    candidate_relocations = _function_relocations(candidate, target_symbol)
    retail_relocations = _function_relocations(retail, target_symbol)
    claims_by_name = {claim.name: claim for claim in claims}
    candidate_definitions = {
        definition.symbol.index: definition for definition in _definitions(candidate)
    }
    retail_definitions = {
        definition.symbol.index: definition for definition in _definitions(retail)
    }
    candidate_claims = []
    retail_claims = []
    seen = set()
    used_candidate_definitions = set()
    for retail_ordinal, retail_relocation in enumerate(retail_relocations):
        retail_symbol = retail.symbols[retail_relocation.symbol_index]
        claim = claims_by_name.get(retail_symbol.name)
        if claim is None or claim.name in seen:
            continue
        retail_definition = retail_definitions.get(retail_relocation.symbol_index)
        if retail_definition is None:
            raise ValueError(
                f"{claim.name} retail target relocation does not reference a data definition"
            )
        retail_payload = retail.section_bytes(retail_definition.section)[
            retail_definition.start:retail_definition.start + claim.size
        ]
        candidate_matches = []
        for candidate_ordinal, candidate_relocation in enumerate(candidate_relocations):
            candidate_definition = candidate_definitions.get(
                candidate_relocation.symbol_index
            )
            if (
                candidate_definition is None
                or candidate_definition.symbol.index in used_candidate_definitions
                or candidate_definition.storage != retail_definition.storage
                or candidate_definition.end - candidate_definition.start < claim.size
            ):
                continue
            candidate_payload = candidate.section_bytes(candidate_definition.section)[
                candidate_definition.start:candidate_definition.start + claim.size
            ]
            if candidate_payload == retail_payload:
                candidate_matches.append((candidate_ordinal, candidate_definition))

        same_ordinal = [
            definition
            for ordinal, definition in candidate_matches
            if ordinal == retail_ordinal
        ]
        if len(same_ordinal) == 1:
            candidate_definition = same_ordinal[0]
        elif len(candidate_matches) == 1:
            candidate_definition = candidate_matches[0][1]
        elif not candidate_matches:
            raise ValueError(
                f"{claim.name} has no payload-identical candidate target relocation"
            )
        else:
            # Payload duplicates: bind order-preservingly. Claims are processed in
            # retail relocation order and consumed definitions are excluded above,
            # so the lowest unused candidate ordinal pairs the k-th retail duplicate
            # with the k-th candidate duplicate. The exact-closure audit still
            # verifies complete byte and ordered-relocation identity, so a wrong
            # pairing can only fail to close, never falsely close.
            candidate_definition = candidate_matches[0][1]

        for side, definition in (
            ("candidate", candidate_definition),
            ("retail", retail_definition),
        ):
            physical_size = definition.end - definition.start
            if claim.size > physical_size:
                raise ValueError(
                    f"{claim.name} logical size 0x{claim.size:x} exceeds {side} "
                    f"target extent 0x{physical_size:x}"
                )
        candidate_scope = (
            "external"
            if candidate_definition.symbol.storage_class == 2
            else "local"
        )
        retail_scope = (
            "external"
            if retail_definition.symbol.storage_class == 2
            else "local"
        )
        candidate_claims.append(CompgenDataClaim(
            claim.name,
            candidate_definition.section.index,
            candidate_definition.start,
            claim.size,
            candidate_definition.storage,
            candidate_scope,
        ))
        retail_claims.append(CompgenDataClaim(
            claim.name,
            retail_definition.section.index,
            retail_definition.start,
            claim.size,
            retail_definition.storage,
            retail_scope,
        ))
        seen.add(claim.name)
        used_candidate_definitions.add(candidate_definition.symbol.index)
    return tuple(candidate_claims), tuple(retail_claims)


def normalized_relocation_stream(metrics: dict) -> list[str]:
    """Hide unstable compiler-private counters while preserving relocation topology."""
    return [
        _COMPILER_PRIVATE_COUNTER.sub(r"\1#", relocation)
        for relocation in metrics.get("reloc_stream", [])
    ]


def byte_differences(left_hex: str, right_hex: str) -> list[dict[str, str | int | None]]:
    """Return every differing byte, including bytes present on only one side."""
    left = bytes.fromhex(left_hex)
    right = bytes.fromhex(right_hex)
    return [
        {
            "offset": offset,
            "candidate": f"{left[offset]:02x}" if offset < len(left) else None,
            "retail": f"{right[offset]:02x}" if offset < len(right) else None,
        }
        for offset in range(max(len(left), len(right)))
        if offset >= len(left) or offset >= len(right) or left[offset] != right[offset]
    ]


def trial_status(trial: dict) -> str:
    """Report only whether the target was observed or closed exactly."""
    if trial.get("exact_closure_eligible"):
        return "exact-closure"
    if trial.get("observed"):
        return "observed"
    return "unusable"


def target_state_identity(metrics: dict) -> str:
    """Identify codegen state without treating private label counters as new states."""
    normalized_reloc_sha = sha256_bytes(
        "\n".join(normalized_relocation_stream(metrics)).encode("utf-8")
    )[:16]
    payload = (
        f"{metrics.get('objdiff_size')}\n{metrics.get('text_sha')}\n"
        f"{normalized_reloc_sha}\n"
    )
    return sha256_bytes(payload.encode("utf-8"))[:16]


def exact_closure_rejections(
    score: float,
    candidate_size: int | None,
    retail_size: int,
    candidate_metrics: dict,
    retail_metrics: dict,
) -> list[str]:
    """Return fail-closed reasons why a 100% objdiff score cannot close the target."""
    out = []
    if score != 100.0:
        out.append("unrounded objdiff score is not exactly 100.0")
    if candidate_size != retail_size:
        out.append(f"target size is not exact: candidate {candidate_size}, retail {retail_size}")
    if not candidate_metrics.get("reloc_stream_complete"):
        out.append("candidate ordered relocation addends are not fully decoded")
    if not retail_metrics.get("reloc_stream_complete"):
        out.append("retail ordered relocation addends are not fully decoded")
    if candidate_metrics.get("reloc_stream") != retail_metrics.get("reloc_stream"):
        out.append("ordered relocation offsets/types/identities/addends differ from retail")
    return out


def record_target_max(
    baseline_path: Path,
    unit: str,
    symbol: str,
    current_hash: str | None,
    new_score: float | None,
) -> dict:
    """Validate one retained-max row and record a higher observed target score.

    All non-target bytes and all other target-row fields are preserved exactly.  Validation
    happens even when *new_score* is absent or non-improving, so
    ``--record-max`` never silently accepts a missing, duplicate, or stale-hash ledger.
    """
    original = baseline_path.read_bytes()
    lines = original.splitlines(keepends=True)
    matches = []
    for index, line in enumerate(lines):
        body = line.rstrip(b"\r\n")
        if not body or body.startswith(b"#"):
            continue
        fields = body.split(b"\t")
        if len(fields) >= 2 and fields[0].decode("utf-8") == unit and fields[1].decode("utf-8") == symbol:
            matches.append((index, line, fields))
    if not matches:
        raise BaselineUpdateError(f"missing baseline row for {unit}::{symbol}")
    if len(matches) != 1:
        raise BaselineUpdateError(f"duplicate baseline rows for {unit}::{symbol}")
    index, line, fields = matches[0]
    if len(fields) < 4 or not fields[3]:
        raise BaselineUpdateError(f"baseline row has no source hash for {unit}::{symbol}")
    stored_hash = fields[3].decode("utf-8")
    if current_hash is None:
        raise BaselineUpdateError(f"current normalized source hash is missing for {unit}::{symbol}")
    if current_hash != stored_hash:
        raise BaselineUpdateError(
            f"source hash mismatch for {unit}::{symbol}: baseline {stored_hash}, current {current_hash}"
        )
    try:
        old_max = float(fields[2])
    except (IndexError, ValueError) as exc:
        raise BaselineUpdateError(f"invalid baseline max for {unit}::{symbol}") from exc
    if not math.isfinite(old_max) or not 0.0 <= old_max <= 100.0:
        raise BaselineUpdateError(f"invalid baseline max for {unit}::{symbol}: {old_max}")
    result = {
        "requested": True,
        "updated": False,
        "unit": unit,
        "symbol": symbol,
        "source_hash": current_hash,
        "old_max": old_max,
        "new_max": old_max,
    }
    if new_score is None:
        result["reason"] = "no_observed_score"
        return result
    if not math.isfinite(new_score) or not 0.0 <= new_score <= 100.0:
        raise BaselineUpdateError(f"invalid observed score for {unit}::{symbol}: {new_score}")
    result["observed_score"] = new_score
    written_max = float(f"{new_score:.4f}")
    if written_max <= old_max:
        result["reason"] = "not_higher"
        return result
    formatted_score = f"{written_max:.4f}"
    ending = line[len(line.rstrip(b"\r\n")) :]
    replacement_fields = list(fields)
    replacement_fields[2] = formatted_score.encode("ascii")
    lines[index] = b"\t".join(replacement_fields) + ending
    updated = b"".join(lines)
    if updated == original:
        result["reason"] = "not_higher"
        return result

    mode = baseline_path.stat().st_mode & 0o777
    temporary_name = None
    try:
        with tempfile.NamedTemporaryFile(dir=baseline_path.parent, delete=False) as handle:
            temporary_name = Path(handle.name)
            handle.write(updated)
            handle.flush()
            os.fsync(handle.fileno())
        os.chmod(temporary_name, mode)
        os.replace(temporary_name, baseline_path)
    finally:
        if temporary_name is not None and temporary_name.exists():
            temporary_name.unlink()
    result.update({"updated": True, "new_max": written_max, "reason": "higher_observation"})
    return result


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument("--source", required=True, type=Path, help="configured TU source path")
    parser.add_argument("--rva", required=True, type=parse_int, help="exact inventory RVA or image VA")
    parser.add_argument("--trials", type=int, default=30, help="number of deterministic trials")
    parser.add_argument(
        "--only-trial",
        type=positive_count,
        action="append",
        help=(
            "compile only this deterministic trial index after generating the full "
            "--trials sequence; repeat to replay several island representatives"
        ),
    )
    parser.add_argument(
        "--compile-timeout-seconds",
        type=positive_seconds,
        default=DEFAULT_COMPILE_TIMEOUT_SECONDS,
        help=(
            "maximum seconds for each baseline/trial compile; a timeout terminates the "
            "compiler process group (default: 120)"
        ),
    )
    parser.add_argument("--seed", type=parse_int, default=0x484F4D32)
    parser.add_argument(
        "--jobs", type=positive_count, default=1,
        help=(
            "compile this many trials concurrently from disposable source "
            "copies beside the target; the reconstructed source is never "
            "edited in parallel mode and every score is produced by the same "
            "serial pipeline (default: 1, the in-place serial behavior)"
        ),
    )
    parser.add_argument(
        "--insertion", choices=("target", "top"), default="target",
        help="insert immediately before the target or after the TU's leading include block",
    )
    parser.add_argument(
        "--max-declarations", type=positive_count, default=DEFAULT_MAX_DECLARATIONS,
        help=(
            "largest deterministic declaration-forest width or compact-family train; "
            f"the default forest walks from {DEFAULT_MIN_FOREST_WIDTH} through this "
            f"value (default: {DEFAULT_MAX_DECLARATIONS})"
        ),
    )
    parser.add_argument(
        "--families", default=",".join(DEFAULT_FAMILIES),
        help=(
            f"comma-separated subset of {','.join(ALL_FAMILIES)} "
            f"(default: {','.join(DEFAULT_FAMILIES)})"
        ),
    )
    parser.add_argument("--output", type=Path, help="artifact directory (default: build/tu-state-noise/...)")
    parser.add_argument(
        "--state-summary", type=Path,
        help=(
            "write a compact reproducible JSON census of unique target byte/relocation "
            "states even when no exact closure is found"
        ),
    )
    parser.add_argument(
        "--layer-state-summary",
        type=Path,
        help=(
            "layer every generated trial on one representative body recovered from "
            "an earlier --state-summary census"
        ),
    )
    parser.add_argument(
        "--layer-trial",
        type=positive_count,
        help="representative trial number to recover from --layer-state-summary",
    )
    parser.add_argument("--dry-run", action="store_true", help="resolve target and emit snippets without compiling")
    parser.add_argument(
        "--record-max", action="store_true",
        help="after restoration, record 100 only for an audited exact target closure",
    )
    parser.add_argument(
        "--retain-best", action="store_true",
        help="retain the best paired objects, disassemblies, diff, and reproducible probe manifest",
    )
    args = parser.parse_args(argv)
    if args.trials < 1:
        parser.error("--trials must be positive")
    if args.dry_run and args.record_max:
        parser.error("--record-max requires compiled trials, not --dry-run")
    if (args.layer_state_summary is None) != (args.layer_trial is None):
        parser.error("--layer-state-summary and --layer-trial must be used together")

    root = Path(os.environ.get("HOMM2_DIR", Path.cwd())).resolve()
    try:
        target, flags = resolve_target(root, args.source, args.rva)
        families = tuple(item.strip() for item in args.families.split(",") if item.strip())
        variants = make_variants(args.trials, families, args.seed, args.max_declarations)
        variants = select_variants(variants, args.only_trial, args.trials)
        layer_body = ""
        layer = None
        if args.layer_state_summary is not None:
            layer_path = (
                (root / args.layer_state_summary).resolve()
                if not args.layer_state_summary.is_absolute()
                else args.layer_state_summary
            )
            layer_body, layer = load_layer_body(
                layer_path, args.layer_trial, target, args.insertion
            )
    except (OSError, KeyError, ValueError) as exc:
        parser.error(str(exc))

    stamp = time.strftime("%Y%m%d-%H%M%S")
    default_name = f"{stamp}-{target.unit.replace('/', '-')}-0x{target.rva:x}"
    final_output = (
        (root / args.output).resolve()
        if args.output
        else root / "build/tu-state-noise" / default_name
    )
    source_rel = target.source.relative_to(root)
    try:
        source_lock = acquire_source_mutation_lock(root, target.source)
    except (OSError, SourceMutationError) as exc:
        parser.error(str(exc))
    original_bytes = target.source.read_bytes()
    original = original_bytes.decode("utf-8")
    from homm2.match.status import source_hashes as project_source_hashes

    target_key = (target.unit, target.symbol)
    canonical_target_hash = project_source_hashes().get(target_key)
    if canonical_target_hash is None:
        parser.error(f"normalized source hash is missing for {target.unit}::{target.symbol}")
    canonical_target_tokens = target_identifiers(original, target)
    target_token_digest = sha256_bytes("\n".join(sorted(canonical_target_tokens)).encode("utf-8"))
    canonical_target_suffix_digest = target_suffix_digest(original, target.va)
    if canonical_target_suffix_digest is None:
        parser.error(f"target VA marker is not uniquely identifiable in {source_rel}")
    target_obj = root / "build/delink" / f"{target.unit}.c.obj"
    if not args.dry_run and not target_obj.exists():
        parser.error(f"retail object is missing: {target_obj}")

    scratch = None
    if args.dry_run:
        output = final_output
        output.mkdir(parents=True, exist_ok=False)
    else:
        final_output.parent.mkdir(parents=True, exist_ok=True)
        if final_output.exists():
            parser.error(f"output path already exists: {final_output}")
        scratch = tempfile.TemporaryDirectory(
            prefix=f".{final_output.name}.tmp-", dir=final_output.parent
        )
        output = Path(scratch.name)
    (output / "original.cpp").write_bytes(original_bytes)
    manifest = {
        "schema": 1,
        "mode": "dry-run-non-matching-diagnostic" if args.dry_run else "compile",
        "root": str(root),
        "git_head": subprocess.run(
            ["git", "rev-parse", "HEAD"], cwd=root, capture_output=True, text=True
        ).stdout.strip(),
        "source": str(source_rel),
        "source_sha256": sha256_bytes(original_bytes),
        "insertion": args.insertion,
        "target": {
            "unit": target.unit,
            "rva": f"0x{target.rva:x}",
            "va": f"0x{target.va:08x}",
            "symbol": target.symbol,
            "codeview_size": target.retail_size,
            "logical_insertion_line": target.logical_line,
            "canonical_source_hash": canonical_target_hash,
            "canonical_identifier_digest": target_token_digest,
        },
        "compiler_flags": flags,
        "compile_timeout_seconds": args.compile_timeout_seconds,
        "seed": args.seed,
        "generated_trial_horizon": args.trials,
        "selected_trials": sorted(args.only_trial) if args.only_trial else None,
        "jobs": args.jobs,
        "layer": layer,
        "policy": {
            "parser_visible_temporary_probes": True,
            "probe_symbols_or_storage_may_exist_only_in_candidate_object": True,
            "source_restored_after_every_trial": True,
            "generated_noise_retained_in_source": False,
            "default_repository_mutation": False,
            "sub_100_source_is_disposable": True,
            "record_max_accepts_higher_observed_target_score": True,
            "exact_closure_requires_unrounded_100_size_and_ordered_relocations": True,
            "only_target_function_is_evaluated": True,
            "sibling_scores_or_metrics_checked": False,
            "compiler_process_group_terminated_on_timeout": True,
        },
        "baseline": None,
        "trials": [],
        "best_observed_disposable": None,
        "exact_closure": None,
        "record_max": {"requested": args.record_max, "updated": False},
    }
    timings = {}
    manifest["timings"] = timings

    if args.dry_run:
        for variant in variants:
            snippet_path = output / f"trial-{variant.trial:04d}-{variant.family}.snippet"
            snippet_path.write_text(layer_body + variant.block(target.logical_line))
            manifest["trials"].append({**asdict(variant), "snippet": snippet_path.name})
        manifest["source_restored"] = target.source.read_bytes() == original_bytes
        restored_target_hash = project_source_hashes().get(target_key)
        manifest["target_source_hash_restored"] = restored_target_hash == canonical_target_hash
        manifest["restored_target_source_hash"] = restored_target_hash
        (output / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
        print(f"dry-run non-matching diagnostic: {len(variants)} auditable variants in {output}")
        source_lock.close()
        return 0

    assert scratch is not None
    pairing = load_pairing_context(root, target.unit)

    baseline_obj = output / "baseline.obj"
    with measure_stage(timings, "baseline_compile"):
        ok, log, baseline_timed_out = compile_object(
            root, target.source, baseline_obj, flags, args.compile_timeout_seconds
        )
    (output / "baseline.compile.log").write_text(log)
    if not ok:
        finalize_compiled_artifacts(scratch, output, final_output, False)
        reason = "timed out" if baseline_timed_out else "failed"
        print(f"baseline compile {reason}; disposable artifacts removed", file=sys.stderr)
        source_lock.close()
        return 2
    with measure_stage(timings, "baseline_normalize"):
        baseline_normalized, baseline_target_normalized, _baseline_generated = (
            normalize_comparison_pair(
                baseline_obj,
                target_obj,
                output / "baseline",
                pairing,
                target.symbol,
            )
        )
    with measure_stage(timings, "baseline_objdiff"):
        baseline_scores, baseline_sizes, baseline_counts, diff_log = objdiff_scores(
            baseline_target_normalized, baseline_normalized, target.symbol
        )
    (output / "baseline.objdiff.log").write_text(diff_log)
    with measure_stage(timings, "baseline_coff_metrics"):
        baseline_metrics = object_metrics(baseline_normalized)
        retail_metrics = object_metrics(baseline_target_normalized)
    if baseline_counts.get(target.symbol) != 1:
        finalize_compiled_artifacts(scratch, output, final_output, False)
        print(f"target symbol is not unique in baseline objdiff: {target.symbol}", file=sys.stderr)
        source_lock.close()
        return 2
    if target.symbol not in baseline_scores or target.symbol not in baseline_metrics:
        finalize_compiled_artifacts(scratch, output, final_output, False)
        print(f"target symbol absent from baseline object: {target.symbol}", file=sys.stderr)
        source_lock.close()
        return 2
    if target.symbol not in retail_metrics:
        finalize_compiled_artifacts(scratch, output, final_output, False)
        print(f"target symbol absent from retail object: {target.symbol}", file=sys.stderr)
        source_lock.close()
        return 2
    baseline_target = baseline_metrics[target.symbol]
    baseline_target["objdiff_size"] = baseline_sizes.get(target.symbol)
    retail_target = retail_metrics.get(target.symbol, {})
    retail_target["codeview_size"] = target.retail_size
    baseline_score = baseline_scores[target.symbol]
    with measure_stage(timings, "baseline_topology"):
        retail_assembly = disassemble_symbol(
            baseline_target_normalized, target.symbol
        )
        baseline_topology = cfg_topology_metrics(
            disassemble_symbol(baseline_normalized, target.symbol),
            retail_assembly,
        )
    manifest["baseline"] = {
        "score": baseline_score,
        "candidate": baseline_target,
        "retail": retail_target,
        "topology": baseline_topology,
    }
    print(
        f"target {target.unit} {target.symbol} RVA 0x{target.rva:x}: "
        f"baseline {baseline_score:.6f}% size {baseline_target['size']} "
        f"relocs {baseline_target['relocs']}/{retail_target.get('relocs', '?')}",
        flush=True,
    )
    if args.retain_best:
        preserve_comparison_objects(
            output,
            baseline_obj,
            baseline_normalized,
            baseline_target_normalized,
            "best",
        )
        preserve_comparison_objects(
            output,
            baseline_obj,
            baseline_normalized,
            baseline_target_normalized,
            "best-topology",
        )
        preserve_comparison_objects(
            output,
            baseline_obj,
            baseline_normalized,
            baseline_target_normalized,
            "best-structural-frontier",
        )

    observed_states = {}

    def observe_state(
        label: str,
        score: float,
        metrics: dict,
        topology: dict,
        variant: Variant | None = None,
    ):
        state_id = target_state_identity(metrics)
        state = observed_states.setdefault(
            state_id,
            {
                "state": state_id,
                "text_sha": metrics["text_sha"],
                "text_hex": metrics["text_hex"],
                "normalized_reloc_sha": sha256_bytes(
                    "\n".join(normalized_relocation_stream(metrics)).encode("utf-8")
                )[:16],
                "normalized_reloc_stream": normalized_relocation_stream(metrics),
                "raw_reloc_detail_shas": [],
                "objdiff_size": metrics.get("objdiff_size"),
                "relocs": metrics["relocs"],
                "scores": [],
                "topology": topology,
                "occurrences": 0,
                "trials": [],
                "baseline": False,
                "representative": None,
            },
        )
        state["occurrences"] += 1
        if score not in state["scores"]:
            state["scores"].append(score)
        if metrics["reloc_detail_sha"] not in state["raw_reloc_detail_shas"]:
            state["raw_reloc_detail_shas"].append(metrics["reloc_detail_sha"])
        if label == "baseline":
            state["baseline"] = True
        elif variant is not None:
            state["trials"].append(variant.trial)
            if state["representative"] is None:
                permutation_text = "\n".join(variant.permutation)
                state["representative"] = {
                    "trial": variant.trial,
                    "family": variant.family,
                    "tag": variant.tag,
                    "body": variant.body,
                    "permutation": variant.permutation,
                    "permutation_sha": sha256_bytes(
                        permutation_text.encode("utf-8")
                    )[:16],
                }
        return state_id

    baseline_state_id = observe_state(
        "baseline", baseline_score, baseline_target, baseline_topology
    )

    best_score = baseline_score
    best_observed = None
    best_origin = {
        "origin": "baseline",
        "trial": None,
        "score": baseline_score,
        "score_delta": 0.0,
        "state": baseline_state_id,
        "candidate": baseline_target,
        "topology": baseline_topology,
        "body": "",
        "permutation": (),
    }
    best_topology_key = topology_rank(baseline_topology, baseline_score)
    best_topology_origin = {
        "origin": "baseline",
        "trial": None,
        "score": baseline_score,
        "score_delta": 0.0,
        "state": baseline_state_id,
        "candidate": baseline_target,
        "topology": baseline_topology,
        "body": "",
        "permutation": (),
    }
    best_frontier_key = structural_frontier_rank(baseline_topology, baseline_score)
    best_frontier_origin = {
        "origin": "baseline",
        "trial": None,
        "score": baseline_score,
        "score_delta": 0.0,
        "state": baseline_state_id,
        "candidate": baseline_target,
        "topology": baseline_topology,
        "body": "",
        "permutation": (),
    }
    exact_closure = None
    rows = []
    interrupted = False
    old_term = signal.getsignal(signal.SIGTERM)

    def stop_for_signal(_signum, _frame):
        raise KeyboardInterrupt

    signal.signal(signal.SIGTERM, stop_for_signal)

    def candidate_admissible(candidate: str, variant) -> tuple[str | None, dict]:
        """Pure per-trial gates shared by the serial and parallel paths."""
        candidate_target_hash = (
            canonical_target_hash
            if target_suffix_digest(candidate, target.va) == canonical_target_suffix_digest
            else None
        )
        include_guard = include_macro_guard(
            root, layer_body + variant.body, canonical_target_tokens
        )
        return candidate_target_hash, include_guard

    precompiled: dict[int, tuple[bool, str, bool]] = {}
    if args.jobs > 1 and variants:

        def parallel_compile(variant):
            candidate = insert_variant(
                original, target, variant, args.insertion, layer_body
            )
            candidate_target_hash, include_guard = candidate_admissible(candidate, variant)
            if candidate_target_hash != canonical_target_hash or not include_guard.get(
                "passed", True
            ):
                return variant.trial, None
            probe_source = target.source.with_name(
                f".{target.source.stem}.trial{variant.trial:04d}{target.source.suffix}"
            )
            trial_obj = output / f"trial-{variant.trial:04d}.obj"
            probe_source.write_bytes(candidate.encode("utf-8"))
            try:
                return variant.trial, compile_object(
                    root,
                    probe_source,
                    trial_obj,
                    flags,
                    args.compile_timeout_seconds,
                )
            finally:
                probe_source.unlink(missing_ok=True)

        with measure_stage(timings, "trial_compile"):
            with concurrent.futures.ThreadPoolExecutor(
                max_workers=args.jobs
            ) as pool:
                for trial_index, result in pool.map(parallel_compile, variants):
                    if result is not None:
                        precompiled[trial_index] = result

    try:
        for variant in variants:
            candidate = insert_variant(
                original, target, variant, args.insertion, layer_body
            )
            candidate_bytes = candidate.encode("utf-8")
            trial_obj = output / f"trial-{variant.trial:04d}.obj"
            trial_generated = []
            compile_timed_out = False
            with measure_stage(timings, "target_hash_check"):
                candidate_target_hash, include_guard = candidate_admissible(
                    candidate, variant
                )
            if candidate_target_hash != canonical_target_hash:
                ok = False
                compile_log = (
                    "canonical target normalized source hash changed: "
                    f"{canonical_target_hash} -> {candidate_target_hash}\n"
                )
            elif not include_guard.get("passed", True):
                ok = False
                compile_log = "include macro guard rejected candidate: " + json.dumps(include_guard) + "\n"
            elif variant.trial in precompiled:
                ok, compile_log, compile_timed_out = precompiled.pop(variant.trial)
            else:
                with temporary_source(target.source, original_bytes, candidate_bytes):
                    with measure_stage(timings, "trial_compile"):
                        ok, compile_log, compile_timed_out = compile_object(
                            root,
                            target.source,
                            trial_obj,
                            flags,
                            args.compile_timeout_seconds,
                        )
            trial = {
                **asdict(variant),
                "source_sha256": sha256_bytes(candidate_bytes),
                "compiled": ok,
                "compile_timed_out": compile_timed_out,
                "score": None,
                "score_delta": None,
                "candidate": None,
                "topology": None,
                "canonical_target_source_hash": candidate_target_hash,
                "include_macro_guard": include_guard,
                "observed": False,
                "exact_closure_eligible": False,
                "exact_closure_rejections": [],
                "rejections": [],
            }
            if not ok:
                (output / f"trial-{variant.trial:04d}.compile.log").write_text(compile_log)
                if candidate_target_hash != canonical_target_hash:
                    trial["rejections"].append("canonical target normalized source hash changed")
                elif not include_guard.get("passed", True):
                    trial["rejections"].append("include macro guard failed")
                elif compile_timed_out:
                    trial["rejections"].append("compile timed out")
                else:
                    trial["rejections"].append("compile failed")
            else:
                with measure_stage(timings, "trial_normalize"):
                    trial_normalized, trial_target_normalized, trial_generated = (
                        normalize_comparison_pair(
                            trial_obj,
                            target_obj,
                            output / f"trial-{variant.trial:04d}",
                            pairing,
                            target.symbol,
                        )
                    )
                with measure_stage(timings, "trial_objdiff"):
                    scores, sizes, symbol_counts, trial_diff_log = objdiff_scores(
                        trial_target_normalized, trial_normalized, target.symbol
                    )
                if trial_diff_log:
                    (output / f"trial-{variant.trial:04d}.objdiff.log").write_text(trial_diff_log)
                with measure_stage(timings, "trial_coff_metrics"):
                    metrics = object_metrics(trial_normalized)
                    trial_retail_metrics = object_metrics(trial_target_normalized)
                score = scores.get(target.symbol)
                target_metrics = metrics.get(target.symbol)
                if symbol_counts.get(target.symbol) != 1:
                    trial["rejections"].append("target symbol is not uniquely identifiable")
                if score is None or target_metrics is None:
                    trial["rejections"].append("target absent from candidate object/diff")
                else:
                    target_metrics["objdiff_size"] = sizes.get(target.symbol)
                    with measure_stage(timings, "trial_topology"):
                        topology = cfg_topology_metrics(
                            disassemble_symbol(trial_normalized, target.symbol),
                            retail_assembly,
                        )
                    trial["score"] = score
                    trial["score_delta"] = score - baseline_score
                    trial["candidate"] = target_metrics
                    trial["topology"] = topology
                    trial["state"] = observe_state(
                        "trial", score, target_metrics, topology, variant
                    )
                    with measure_stage(timings, "target_gates"):
                        candidate_size = target_metrics.get("objdiff_size")
                        if candidate_size is None:
                            trial["rejections"].append("objdiff function size unavailable")
                        trial["observed"] = not trial["rejections"]
                        trial["exact_closure_rejections"] = exact_closure_rejections(
                            score,
                            candidate_size,
                            target.retail_size,
                            target_metrics,
                            trial_retail_metrics.get(target.symbol, retail_target),
                        )
                        trial["exact_closure_eligible"] = not trial["exact_closure_rejections"]
                    if trial["exact_closure_eligible"] and exact_closure is None:
                        exact_closure = trial
                    if trial["observed"] and score > baseline_score + 1e-6:
                        if best_observed is None or score > best_observed["score"] + 1e-6:
                            best_observed = trial
                    if trial["observed"] and score > best_score + 1e-6:
                        best_score = score
                        best_origin = {
                            "origin": "trial",
                            "trial": trial["trial"],
                            "family": trial["family"],
                            "tag": trial["tag"],
                            "score": score,
                            "score_delta": trial["score_delta"],
                            "state": trial["state"],
                            "candidate": target_metrics,
                            "topology": topology,
                            "body": trial["body"],
                            "permutation": trial["permutation"],
                        }
                        if args.retain_best:
                            preserve_comparison_objects(
                                output,
                                trial_obj,
                                trial_normalized,
                                trial_target_normalized,
                                "best",
                            )
                    trial_topology_key = topology_rank(topology, score)
                    if trial["observed"] and trial_topology_key < best_topology_key:
                        best_topology_key = trial_topology_key
                        best_topology_origin = {
                            "origin": "trial",
                            "trial": trial["trial"],
                            "family": trial["family"],
                            "tag": trial["tag"],
                            "score": score,
                            "score_delta": trial["score_delta"],
                            "state": trial["state"],
                            "candidate": target_metrics,
                            "topology": topology,
                            "body": trial["body"],
                            "permutation": trial["permutation"],
                        }
                        if args.retain_best:
                            preserve_comparison_objects(
                                output,
                                trial_obj,
                                trial_normalized,
                                trial_target_normalized,
                                "best-topology",
                            )
                    trial_frontier_key = structural_frontier_rank(topology, score)
                    if trial["observed"] and trial_frontier_key < best_frontier_key:
                        best_frontier_key = trial_frontier_key
                        best_frontier_origin = {
                            "origin": "trial",
                            "trial": trial["trial"],
                            "family": trial["family"],
                            "tag": trial["tag"],
                            "score": score,
                            "score_delta": trial["score_delta"],
                            "state": trial["state"],
                            "candidate": target_metrics,
                            "topology": topology,
                            "body": trial["body"],
                            "permutation": trial["permutation"],
                        }
                        if args.retain_best:
                            preserve_comparison_objects(
                                output,
                                trial_obj,
                                trial_normalized,
                                trial_target_normalized,
                                "best-structural-frontier",
                            )
                trial_obj.unlink(missing_ok=True)
                Path(str(trial_obj) + ".d").unlink(missing_ok=True)
                for generated in trial_generated:
                    generated.unlink(missing_ok=True)
            manifest["trials"].append(trial)
            rows.append(
                f"{variant.trial}\t{variant.family}\t"
                f"{trial['score'] if trial['score'] is not None else 'NA'}\t"
                f"{trial['score_delta'] if trial['score_delta'] is not None else 'NA'}\t"
                f"{int(trial['observed'])}\t{' | '.join(trial['rejections'])}\t"
                f"{int(trial['exact_closure_eligible'])}\t"
                f"{' | '.join(trial['exact_closure_rejections'])}\n"
            )
            state = trial_status(trial)
            score_text = "compile-failed" if trial["score"] is None else f"{trial['score']:.6f}%"
            print(f"[{variant.trial:04d}/{len(variants):04d}] {variant.family}: {score_text} {state}", flush=True)
            if trial["exact_closure_eligible"]:
                break
    except KeyboardInterrupt:
        interrupted = True
        manifest["interrupted"] = True
        print("probe interrupted; restoring source and removing disposable artifacts", file=sys.stderr)
    finally:
        # temporary_source owns restoration. Never overwrite a manual or foreign edit
        # observed outside its guarded candidate interval.
        signal.signal(signal.SIGTERM, old_term)

    (output / "trials.tsv").write_text(
        "trial\tfamily\tscore\tdelta\tobserved\trejections\t"
        "exact_closure_eligible\texact_closure_rejections\n"
        + "".join(rows)
    )
    state_rows = sorted(observed_states.values(), key=lambda row: (not row["baseline"], row["state"]))
    retail_text_hex = retail_target["text_hex"][: target.retail_size * 2]
    for state in state_rows:
        state_text_hex = state["text_hex"][: state["objdiff_size"] * 2]
        state["retail_byte_differences"] = byte_differences(
            state_text_hex, retail_text_hex
        )
    state_byte_delta_matrix = {
        left["state"]: {
            right["state"]: len(byte_differences(
                left["text_hex"][: left["objdiff_size"] * 2],
                right["text_hex"][: right["objdiff_size"] * 2],
            ))
            for right in state_rows
        }
        for left in state_rows
    }
    manifest["target_states"] = state_rows
    print(f"target states: {len(state_rows)} unique byte/relocation states", flush=True)
    for index, state in enumerate(state_rows, 1):
        representative = state["representative"]
        source = "baseline" if state["baseline"] else f"trial {representative['trial']}"
        permutation = (
            ""
            if representative is None
            else (
                f" tag={representative['tag']}"
                f" permutation={representative['permutation_sha']}"
                f"/{len(representative['permutation'])}"
            )
        )
        scores = ",".join(f"{score:.6f}" for score in sorted(state["scores"]))
        print(
            f"  state {index:02d} {state['state']}: {source}; occurrences={state['occurrences']} "
            f"scores={scores}% size={state['objdiff_size']} "
            f"blocks={state['topology']['candidate_blocks']}/"
            f"{state['topology']['retail_blocks']} "
            f"edges={state['topology']['labeled_edge_delta']} "
            f"pred={state['topology']['predecessor_delta']} "
            f"flow={state['topology']['flow_kind']} "
            f"shift={state['topology']['target_shift']} "
            f"retail-byte-delta={len(state['retail_byte_differences'])} "
            f"text={state['text_sha']} relocs={state['normalized_reloc_sha']} "
            f"raw-label-spellings={len(state['raw_reloc_detail_shas'])}{permutation}",
            flush=True,
        )
    if args.state_summary:
        state_summary_path = (
            (root / args.state_summary).resolve()
            if not args.state_summary.is_absolute()
            else args.state_summary
        )
        state_summary_output = staged_artifact_path(
            state_summary_path, output, final_output
        )
        state_summary_output.parent.mkdir(parents=True, exist_ok=True)
        state_summary_output.write_text(
            json.dumps(
                {
                    "schema": 3,
                    "source": str(source_rel),
                    "target": manifest["target"],
                    "seed": args.seed,
                    "generated_trial_horizon": args.trials,
                    "selected_trials": (
                        sorted(args.only_trial) if args.only_trial else None
                    ),
                    "families": families,
                    "max_declarations": args.max_declarations,
                    "insertion": args.insertion,
                    "layer": layer,
                    "retail": retail_target,
                    "best_fuzzy": {
                        key: best_origin.get(key)
                        for key in ("origin", "trial", "state", "score", "topology")
                    },
                    "best_topology": {
                        key: best_topology_origin.get(key)
                        for key in ("origin", "trial", "state", "score", "topology")
                    },
                    "best_structural_frontier": {
                        key: best_frontier_origin.get(key)
                        for key in ("origin", "trial", "state", "score", "topology")
                    },
                    "states": state_rows,
                    "state_byte_delta_matrix": state_byte_delta_matrix,
                },
                indent=2,
            )
            + "\n"
        )
    if best_observed is not None:
        manifest["best_observed_disposable"] = {
            "trial": best_observed["trial"],
            "family": best_observed["family"],
            "score": best_observed["score"],
            "score_delta": best_observed["score_delta"],
            "candidate": best_observed["candidate"],
            "topology": best_observed["topology"],
            "permutation": best_observed["permutation"],
            "source_hash_unchanged": True,
            "generated_noise_retained": False,
        }
    if args.retain_best:
        best_paths = write_comparison_disassembly(output, target.symbol, "best")
        manifest["best_retained"] = {
            **best_origin,
            "source_hash_unchanged": True,
            "generated_noise_retained_in_source": False,
            "artifacts": best_paths,
        }
        best_topology_paths = write_comparison_disassembly(
            output, target.symbol, "best-topology"
        )
        manifest["best_topology_retained"] = {
            **best_topology_origin,
            "source_hash_unchanged": True,
            "generated_noise_retained_in_source": False,
            "artifacts": best_topology_paths,
        }
        best_frontier_paths = write_comparison_disassembly(
            output, target.symbol, "best-structural-frontier"
        )
        manifest["best_structural_frontier_retained"] = {
            **best_frontier_origin,
            "source_hash_unchanged": True,
            "generated_noise_retained_in_source": False,
            "experimental_metric": True,
            "artifacts": best_frontier_paths,
        }
    if exact_closure is not None:
        manifest["exact_closure"] = {
            "trial": exact_closure["trial"],
            "family": exact_closure["family"],
            "tag": exact_closure["tag"],
            "score": exact_closure["score"],
            "body": exact_closure["body"],
            "permutation": exact_closure["permutation"],
            "target_size": exact_closure["candidate"]["objdiff_size"],
            "reloc_detail_sha": exact_closure["candidate"]["reloc_detail_sha"],
            "source_hash_unchanged": True,
            "generated_noise_retained": False,
        }
    manifest["source_restored"] = target.source.read_bytes() == original_bytes
    restored_target_hash = project_source_hashes().get(target_key)
    manifest["target_source_hash_restored"] = restored_target_hash == canonical_target_hash
    manifest["restored_target_source_hash"] = restored_target_hash
    if not manifest["source_restored"] or not manifest["target_source_hash_restored"]:
        finalize_compiled_artifacts(scratch, output, final_output, False)
        print("FATAL: source or normalized target-hash restoration check failed", file=sys.stderr)
        source_lock.close()
        return 3
    record_error = False
    if args.record_max and interrupted:
        manifest["record_max"] = {
            "requested": True,
            "updated": False,
            "reason": "search_interrupted",
        }
    elif args.record_max:
        try:
            manifest["record_max"] = record_target_max(
                root / "config/match_baseline.tsv",
                target.unit,
                target.symbol,
                restored_target_hash,
                # The baseline is itself an observation of the current source
                # hash; without this a source fix that raises the clean score
                # above the ledger is never recorded unless an island beats it.
                best_observed["score"] if best_observed is not None else baseline_score,
            )
        except (OSError, BaselineUpdateError) as exc:
            record_error = True
            manifest["record_max"] = {
                "requested": True,
                "updated": False,
                "refused": True,
                "error": str(exc),
            }
    (output / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print(format_timings(timings), flush=True)
    if record_error:
        finalize_compiled_artifacts(scratch, output, final_output, False)
        print(f"record-max refused: {manifest['record_max']['error']}", file=sys.stderr)
        source_lock.close()
        return 4
    if interrupted:
        finalize_compiled_artifacts(scratch, output, final_output, False)
        source_lock.close()
        return 130

    if exact_closure is None:
        retained_output = finalize_compiled_artifacts(
            scratch, output, final_output, args.retain_best
        )
        if best_observed is None:
            print("no audited exact closure; source restored; baseline remains the best state")
        else:
            print(
                f"best observed {baseline_score:.6f}% -> "
                f"{best_observed['score']:.6f}% (trial {best_observed['trial']}); "
                "no audited exact closure; source restored",
            )
        topology_trial = best_topology_origin["trial"]
        topology_source = (
            "baseline" if topology_trial is None else f"trial {topology_trial}"
        )
        topology = best_topology_origin["topology"]
        print(
            f"best topology {topology_source}: "
            f"{topology['candidate_blocks']}/{topology['retail_blocks']} blocks, "
            f"labeled-edge-delta={topology['labeled_edge_delta']}, "
            f"predecessor-delta={topology['predecessor_delta']}, "
            f"flow-kind={topology['flow_kind']}, "
            f"target-shift={topology['target_shift']}, "
            f"size-only={topology['size_only']}; "
            f"fuzzy={best_topology_origin['score']:.6f}%"
        )
        frontier_trial = best_frontier_origin["trial"]
        frontier_source = (
            "baseline" if frontier_trial is None else f"trial {frontier_trial}"
        )
        frontier = best_frontier_origin["topology"]
        divergence = frontier["first_structural_divergence"]
        divergence_text = (
            "none"
            if divergence is None
            else f"B{divergence}:{frontier['first_structural_divergence_kind']}"
        )
        print(
            f"best structural frontier (experimental) {frontier_source}: "
            f"{frontier['leading_exact_blocks']} blocks/"
            f"{frontier['leading_exact_instructions']} instructions, "
            f"first-divergence={divergence_text}; "
            f"fuzzy={best_frontier_origin['score']:.6f}%"
        )
        if retained_output is not None:
            print(f"best fuzzy and topology objects retained: {retained_output}")
        if args.record_max:
            state = manifest["record_max"]
            if state["updated"]:
                print(
                    f"observed maximum retained {state['old_max']:.4f}% -> "
                    f"{state['new_max']:.4f}% "
                    f"for unchanged source hash {state['source_hash']}; "
                    "generated probe not retained"
                )
            else:
                print(f"retained maximum unchanged: {state['reason']}")
        source_lock.close()
        return 0

    retained_output = finalize_compiled_artifacts(scratch, output, final_output, True)
    assert retained_output is not None
    print(
        f"audited exact closure 100.0000% (trial {exact_closure['trial']}); "
        f"source restored; reproducible artifact preserved: {retained_output}"
    )
    if args.record_max:
        state = manifest["record_max"]
        if state["updated"]:
            print(
                f"audited exact maximum retained {state['old_max']:.4f}% -> "
                f"{state['new_max']:.4f}% "
                f"for unchanged source hash {state['source_hash']}; generated probe not retained"
            )
        else:
            print(f"exact closure already retained; baseline unchanged: {state['reason']}")
    source_lock.close()
    return 0


if __name__ == "__main__":
    sys.exit(main())
