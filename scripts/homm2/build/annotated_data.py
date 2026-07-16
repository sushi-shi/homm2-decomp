"""Shared Clang VarDecl inventory for source ``DATA()`` definitions."""

from __future__ import annotations

import glob
import hashlib
import json
import os
import re
import tempfile
from dataclasses import dataclass
from pathlib import Path

import clang.cindex as ci


IMAGE_BASE = 0x400000
DATA_TOKEN = re.compile(rb"\bDATA\s*\(\s*(0x[0-9a-fA-F]+)\s*\)")
INCLUDE_TOKEN = re.compile(r'^[ \t]*#[ \t]*include[ \t]*[<"]([^>"]+)[>"]', re.M)
INVENTORY_CACHE_SCHEMA = 1


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
    project_includes = [repo / "include"]
    vendor = repo / "vendor"
    if vendor.is_dir():
        project_includes.extend(sorted(path for path in vendor.iterdir() if path.is_dir()))
    for include in project_includes:
        args.extend(("-I", str(include)))
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
        args.extend(("-D__fastcall=", "-D__stdcall="))
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


def _source_dependencies(path: Path, include_roots: list[Path]) -> list[Path]:
    seen = set()
    stack = [path.resolve()]
    while stack:
        current = stack.pop()
        if current in seen:
            continue
        seen.add(current)
        try:
            text = current.read_text(errors="replace")
        except OSError:
            continue
        for include in INCLUDE_TOKEN.findall(text):
            for candidate in (current.parent / include,
                              *(root / include for root in include_roots)):
                if candidate.is_file():
                    stack.append(candidate.resolve())
                    break
    seen.discard(path.resolve())
    return sorted(seen)


def _inventory_cache_key(path: Path, unit: str, object_root: Path,
                         compile_database: bytes, include_roots: list[Path]) -> str | None:
    object_path = object_root / f"{unit}.obj"
    if not object_path.is_file():
        return None
    digest = hashlib.sha256()
    digest.update(f"annotated-data-v{INVENTORY_CACHE_SCHEMA}\0".encode("ascii"))
    digest.update(Path(__file__).read_bytes())
    digest.update(compile_database)
    digest.update(path.read_bytes())
    digest.update(object_path.read_bytes())
    for dependency in _source_dependencies(path, include_roots):
        digest.update(str(dependency).encode("utf-8"))
        digest.update(dependency.read_bytes())
    return digest.hexdigest()


def _load_inventory_cache(path: Path) -> dict:
    try:
        data = json.loads(path.read_text())
    except (FileNotFoundError, json.JSONDecodeError, OSError):
        return {}
    if data.get("schema") != INVENTORY_CACHE_SCHEMA:
        return {}
    entries = data.get("entries")
    return entries if isinstance(entries, dict) else {}


def _write_inventory_cache(path: Path, entries: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile("w", encoding="utf-8", prefix=f".{path.name}.",
                                     dir=path.parent, delete=False) as stream:
        temporary = Path(stream.name)
        json.dump({"schema": INVENTORY_CACHE_SCHEMA, "entries": entries},
                  stream, separators=(",", ":"))
    os.replace(temporary, path)


def source_definitions(source_root: Path, repo: Path, object_root: Path | None = None,
                       cache_path: Path | None = None) -> list[AnnotatedDataDefinition]:
    source_root = Path(source_root)
    repo = Path(repo)
    if object_root is None and source_root.resolve() == (repo / "src").resolve():
        candidate = repo / "build/objdiff/base"
        if candidate.is_dir():
            object_root = candidate
    if cache_path is None and object_root is not None:
        cache_path = repo / "build/gen/annotated_data_cache.json"
    object_root = Path(object_root) if object_root is not None else None
    cache_path = Path(cache_path) if cache_path is not None else None
    compile_path = repo / "build/clangd/compile_commands.json"
    compile_database = compile_path.read_bytes() if compile_path.is_file() else b""
    include_roots = [repo / "include"]
    vendor = repo / "vendor"
    if vendor.is_dir():
        include_roots.extend(sorted(path for path in vendor.iterdir() if path.is_dir()))
    cached = _load_inventory_cache(cache_path) if cache_path is not None else {}
    retained = {}
    rows = []
    for path in sorted(source_root.rglob("*.cpp")):
        unit = path.relative_to(source_root).with_suffix("").as_posix()
        key = (_inventory_cache_key(path, unit, object_root, compile_database, include_roots)
               if object_root is not None else None)
        entry = cached.get(unit) if key is not None else None
        if isinstance(entry, dict) and entry.get("key") == key:
            values = [AnnotatedDataDefinition(**row) for row in entry.get("rows", [])]
        else:
            values = definitions_for_file(path, source_root, repo)
        rows.extend(values)
        if key is not None:
            retained[unit] = {
                "key": key,
                "rows": [{field: getattr(row, field)
                          for field in AnnotatedDataDefinition.__dataclass_fields__}
                         for row in values],
            }
    if cache_path is not None:
        _write_inventory_cache(cache_path, retained)
    identities = {(row.unit, row.rva) for row in rows}
    if len(identities) != len(rows):
        raise ValueError("duplicate DATA RVA within a translation unit")
    return rows
