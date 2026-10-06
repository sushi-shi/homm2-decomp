"""Recover source-owned primary and secondary vtable identities."""

from __future__ import annotations

import re
from collections import Counter
from dataclasses import dataclass
from pathlib import Path

import clang.cindex as ci

from homm2.retail_labels.annotated_data import ClangMode, _clang_args, configure_libclang
from homm2.retail_labels.annotated_functions import _mask_lexical_noise


IMAGE_BASE = 0x400000
IDENTIFIER = rb"[A-Za-z_][A-Za-z0-9_]*"
PRIMARY = re.compile(
    rb"^[ \t]*VTBL\(\s*(" + IDENTIFIER + rb")\s*,\s*"
    rb"(0x[0-9a-fA-F]+)\s*\)", re.M)
SECONDARY = re.compile(
    rb"^[ \t]*VTBL2\(\s*(" + IDENTIFIER + rb")\s*,\s*(" + IDENTIFIER +
    rb")\s*,\s*(0x[0-9a-fA-F]+)\s*\)", re.M)


@dataclass(frozen=True, order=True)
class AnnotatedVtable:
    unit: str
    rva: int
    derived: str
    base: str | None
    mangled_name: str
    location: str


def _mangled_name(derived: str, base: str | None) -> str:
    """Form a vftable name from compiler-resolved record encodings."""
    if base is None:
        return f"??_7{derived}6B@"
    # Secondary tables can use name backreferences spanning the derived and
    # base encodings. The existing global-class contract has none; do not
    # silently invent a decorated name for a newly encountered complex case.
    if not all(re.fullmatch(r"[A-Za-z_]\w*@@", value) for value in (derived, base)):
        raise ValueError("secondary vtable requires simple global record types")
    return f"??_7{derived}6B{base}@"


def _record_encodings(path: Path, blob: bytes, matches: list,
                      repo: Path) -> dict[tuple[int, str], str]:
    """Resolve marker types in their actual lexical scope, including typedefs.

    libclang exposes destructor manglings but not vftable manglings. Replace
    only metadata markers in an unsaved analysis view with destructor references;
    their Microsoft encodings supply the vftable's canonical owner. Unlike an
    extern variable's type encoding, a destructor's owner does not inherit name
    backreferences from an unrelated enclosing variable name. These functions
    are never written, compiled into COFF, or linked. Candidate
    COFF still supplies and validates the physical table definition downstream.
    """
    edits = []
    probes = {}
    for offset, match, secondary in matches:
        declarations = []
        for role, group in (("derived", 1), ("base", 2)) if secondary else (("derived", 1),):
            identifier = f"__h2_vtable_type_{offset}_{role}"
            if identifier.encode() in blob:
                raise ValueError(f"{path}: reserved analysis identifier {identifier}")
            record = match.group(group).decode("ascii")
            declarations.append(
                f"void {identifier}({record}* object) {{ object->~{record}(); }}")
            probes[identifier] = (offset, role)
        edits.append((match.start(), match.end(), " ".join(declarations).encode()))
    view = blob
    for start, end, replacement in sorted(edits, reverse=True):
        view = view[:start] + replacement + view[end:]
    configure_libclang()
    translation = ci.Index.create().parse(
        str(path), args=_clang_args(repo, path, mode=ClangMode.RETAIL_ANALYSIS),
        unsaved_files=[(str(path), view.decode("utf-8"))])
    result = {}
    # Markers are namespace-scope metadata. Avoid walking every function body
    # and instantiated SDK/STL declaration just to find the analysis helpers.
    pending = [translation.cursor]
    while pending:
        cursor = pending.pop()
        if cursor.kind in (ci.CursorKind.TRANSLATION_UNIT,
                           ci.CursorKind.NAMESPACE, ci.CursorKind.LINKAGE_SPEC):
            pending.extend(cursor.get_children())
            continue
        if cursor.kind != ci.CursorKind.FUNCTION_DECL or cursor.spelling not in probes:
            continue
        parameter, = cursor.get_arguments()
        if parameter.type.get_pointee().get_canonical().kind != ci.TypeKind.RECORD:
            raise ValueError(f"{path}:{cursor.location.line}: vtable owner is not a record type")
        destructors = [child.referenced for child in cursor.walk_preorder()
                       if child.kind == ci.CursorKind.MEMBER_REF_EXPR
                       and child.referenced is not None
                       and child.referenced.kind == ci.CursorKind.DESTRUCTOR]
        # MSVC ordinary and vbase destructor names share the canonical record
        # owner encoding with its primary vftable. Clang supplies that encoding,
        # including nested/template identities and their name backreferences.
        mangled = (re.fullmatch(r"\?\?(?:1|_D)(.+@@)[A-Z]AE(?:@|X)XZ",
                                destructors[0].mangled_name)
                   if len(destructors) == 1 else None)
        if mangled is None:
            raise ValueError(f"{path}:{cursor.location.line}: unsupported vtable owner type")
        result[probes[cursor.spelling]] = mangled[1]
    missing = sorted(set(probes.values()) - result.keys())
    if missing:
        raise ValueError(f"{path}: unresolved vtable owner types: {missing}")
    return result


def source_vtables(source_root: Path, repo: Path) -> list[AnnotatedVtable]:
    source_root = Path(source_root).resolve()
    repo = Path(repo).resolve()
    rows = []
    for path in sorted(source_root.rglob("*.cpp")):
        blob = path.read_bytes()
        masked = _mask_lexical_noise(blob)
        unit = path.relative_to(source_root).with_suffix("").as_posix()
        matches = [
            (match.start(), match, False) for match in PRIMARY.finditer(masked)
        ]
        matches.extend(
            (match.start(), match, True) for match in SECONDARY.finditer(masked))
        if not matches:
            continue
        encodings = _record_encodings(path, blob, matches, repo)
        for offset, match, secondary in sorted(matches):
            derived = match.group(1).decode("ascii")
            base = match.group(2).decode("ascii") if secondary else None
            va_group = 3 if secondary else 2
            va = int(match.group(va_group), 16)
            line = blob.count(b"\n", 0, offset) + 1
            if va < IMAGE_BASE:
                raise ValueError(f"{path}:{line}: invalid vtable VA")
            rows.append(AnnotatedVtable(
                unit=unit,
                rva=va - IMAGE_BASE,
                derived=derived,
                base=base,
                mangled_name=_mangled_name(
                    encodings[offset, "derived"],
                    encodings[offset, "base"] if secondary else None),
                location=f"{path.relative_to(repo).as_posix()}:{line}",
            ))
    duplicates = sorted(
        name for name, count in Counter(row.mangled_name for row in rows).items()
        if count > 1)
    if duplicates:
        raise ValueError("duplicate source vtable identities: " + ", ".join(duplicates))
    return sorted(rows, key=lambda row: (row.unit, row.rva, row.mangled_name))
