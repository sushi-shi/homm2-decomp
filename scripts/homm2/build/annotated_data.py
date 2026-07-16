"""Shared Clang VarDecl inventory for source ``DATA()`` definitions."""

from __future__ import annotations

import glob
import json
import re
from dataclasses import dataclass
from pathlib import Path

import clang.cindex as ci


IMAGE_BASE = 0x400000
DATA_TOKEN = re.compile(rb"\bDATA\s*\(\s*(0x[0-9a-fA-F]+)\s*\)")


@dataclass(frozen=True, order=True)
class AnnotatedDataDefinition:
    unit: str
    name: str
    qualified_name: str
    rva: int
    size: int
    location: str
    is_static: bool


def configure_libclang() -> None:
    libraries = glob.glob("/nix/store/*clang*-lib/lib/libclang.so")
    if libraries:
        try:
            ci.Config.set_library_file(libraries[0])
        except Exception:
            pass


def _clang_args(repo: Path, source: Path) -> list[str]:
    database_path = repo / "build/clangd/compile_commands.json"
    database = json.loads(database_path.read_text()) if database_path.is_file() else []
    source = source.resolve()
    raw = []
    for entry in database:
        value = Path(entry.get("file", ""))
        if not value.is_absolute():
            value = Path(entry.get("directory", repo)) / value
        if value.resolve() == source:
            raw = entry.get("arguments", [])
            break
    args = ["-x", "c++", "-std=c++14", "-fms-compatibility",
            "-fdelayed-template-parsing", "-ferror-limit=0"]
    index = 0
    while index < len(raw):
        value = raw[index]
        if value in ("/I", "/imsvc") and index + 1 < len(raw):
            args.extend(("-I" if value == "/I" else "-isystem", raw[index + 1]))
            index += 2
            continue
        if value.startswith("/D"):
            args.append("-D" + value[2:])
        elif value.startswith(("--target=", "-fms", "-fdelayed")):
            args.append(value)
        index += 1
    if not raw:
        args.extend(("-I", str(repo / "include"), "-D__fastcall=", "-D__stdcall="))
    return args


def _mask_lexical_noise(blob: bytes) -> bytes:
    out = bytearray(blob)
    index = 0
    state = "code"
    quote = 0
    while index < len(blob):
        byte = blob[index]
        following = blob[index + 1] if index + 1 < len(blob) else 0
        if state == "code":
            if byte == 47 and following == 47:
                out[index:index + 2] = b"  "; index += 2; state = "line"; continue
            if byte == 47 and following == 42:
                out[index:index + 2] = b"  "; index += 2; state = "block"; continue
            if byte in (34, 39):
                quote = byte; out[index] = 32; index += 1; state = "literal"; continue
        elif state == "line":
            if byte == 10:
                state = "code"
            else:
                out[index] = 32
            index += 1; continue
        elif state == "block":
            if byte == 42 and following == 47:
                out[index:index + 2] = b"  "; index += 2; state = "code"; continue
            if byte != 10:
                out[index] = 32
            index += 1; continue
        else:
            if byte == 92 and index + 1 < len(blob):
                out[index:index + 2] = b"  "; index += 2; continue
            if byte == quote:
                state = "code"
            if byte != 10:
                out[index] = 32
            index += 1; continue
        index += 1
    if state in ("block", "literal"):
        raise ValueError("unterminated source comment or literal")
    return bytes(out)


def _declaration_end(masked: bytes, start: int) -> int:
    depth = {40: 0, 91: 0, 123: 0}
    closing = {41: 40, 93: 91, 125: 123}
    for index in range(start, len(masked)):
        byte = masked[index]
        if byte in depth:
            depth[byte] += 1
        elif byte in closing:
            depth[closing[byte]] -= 1
        elif byte == 59 and not any(depth.values()):
            return index + 1
    raise ValueError("unterminated DATA declaration")


def _qualified_name(cursor) -> str:
    owners = []
    parent = cursor.semantic_parent
    owner_kinds = {
        ci.CursorKind.CLASS_DECL, ci.CursorKind.STRUCT_DECL,
        ci.CursorKind.CLASS_TEMPLATE, ci.CursorKind.NAMESPACE,
    }
    while parent is not None and parent.kind in owner_kinds:
        if parent.spelling:
            owners.append(parent.spelling)
        parent = parent.semantic_parent
    return "::".join([*reversed(owners), cursor.spelling])


def definitions_for_file(path: Path, source_root: Path, repo: Path) -> list[AnnotatedDataDefinition]:
    path = path.resolve()
    blob = path.read_bytes()
    masked = _mask_lexical_noise(blob)
    markers = [(match, _declaration_end(masked, match.end()))
               for match in DATA_TOKEN.finditer(masked)]
    if not markers:
        return []
    configure_libclang()
    index = ci.Index.create()
    # libclang offsets are UTF-8 byte offsets. Decoding as latin-1 would
    # re-encode non-ASCII comments and shift every later cursor.
    text = blob.decode("utf-8")
    tu = index.parse(str(path), args=_clang_args(repo, path),
                     unsaved_files=[(str(path), text)],
                     options=ci.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD)
    variables = []
    for cursor in tu.cursor.walk_preorder():
        if cursor.kind != ci.CursorKind.VAR_DECL or not cursor.is_definition():
            continue
        if cursor.location.file is None or Path(str(cursor.location.file)).resolve() != path:
            continue
        variables.append(cursor)
    rows = []
    unit = path.relative_to(source_root.resolve()).with_suffix("").as_posix()
    for marker, end in markers:
        matches = [cursor for cursor in variables
                   if marker.start() <= cursor.extent.start.offset < end
                   and cursor.extent.end.offset <= end]
        if len(matches) != 1:
            line = blob.count(b"\n", 0, marker.start()) + 1
            raise ValueError(f"{path}:{line}: DATA marker covers {len(matches)} VarDecls")
        cursor = matches[0]
        size = cursor.type.get_size()
        if size <= 0:
            raise ValueError(f"{path}:{cursor.location.line}: incomplete DATA type")
        try:
            display = path.relative_to(repo)
        except ValueError:
            display = path.relative_to(source_root.resolve())
        marker_line = blob.count(b"\n", 0, marker.start()) + 1
        rows.append(AnnotatedDataDefinition(
            unit, cursor.spelling, _qualified_name(cursor),
            int(marker.group(1), 16) - IMAGE_BASE, size,
            f"{display.as_posix()}:{marker_line}",
            cursor.storage_class == ci.StorageClass.STATIC,
        ))
    return rows


def source_definitions(source_root: Path, repo: Path) -> list[AnnotatedDataDefinition]:
    rows = []
    for path in sorted(Path(source_root).rglob("*.cpp")):
        rows.extend(definitions_for_file(path, Path(source_root), Path(repo)))
    identities = {(row.unit, row.rva) for row in rows}
    if len(identities) != len(rows):
        raise ValueError("duplicate DATA RVA within a translation unit")
    return rows
