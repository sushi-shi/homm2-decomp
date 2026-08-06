"""Recover source-bound compiler-generated literal data claims."""

from __future__ import annotations

import ast
import argparse
import gc
import hashlib
import json
import os
import re
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass
from pathlib import Path

import clang.cindex as ci

from homm2.clang_options import ClangMode
from homm2.build.annotated_data import (
    _clang_args,
    _mask_lexical_noise,
    _source_dependencies,
    configure_libclang,
)


IMAGE_BASE = 0x400000
CACHE_SCHEMA = 2
TOKEN = re.compile(rb"\bDATA_COMPGEN\s*\(")
GUARD_TOKEN = re.compile(rb"\bDATA_COMPGEN_GUARD\s*\(")
IDENTIFIER = re.compile(rb"^[A-Za-z_][A-Za-z0-9_]*$")
ADDRESS = re.compile(rb"^0[xX][0-9a-fA-F]+$")
STRING_TOKEN = re.compile(rb'(?:(?:u8)?"(?:\\.|[^"\\])*")')
FLOAT_TOKEN = re.compile(
    rb"^[-+]?(?:(?:[0-9]+\.[0-9]*|\.[0-9]+)(?:[eE][-+]?[0-9]+)?|"
    rb"[0-9]+[eE][-+]?[0-9]+)([fF])?$")


@dataclass(frozen=True, order=True)
class CompgenDataClaim:
    unit: str
    rva: int
    semantic_name: str
    kind: str
    size: int
    location: str
    value_location: str
    owners: tuple[str, ...]


def compgen_data_symbol_name(unit: str, semantic_name: str) -> str:
    """Stable COFF/PDB identity for one source DATA_COMPGEN claim."""
    return "__h2cg$%s$data$%s" % (unit.replace("/", "$"), semantic_name)


@dataclass(frozen=True)
class _Invocation:
    offset: int
    address: int
    semantic_name: str
    value_start: int
    value_end: int


def _split_arguments(masked: bytes, start: int, end: int) -> list[tuple[int, int]]:
    rows = []
    depth = 0
    argument = start
    for offset in range(start, end):
        byte = masked[offset]
        if byte in (40, 91, 123):
            depth += 1
        elif byte in (41, 93, 125):
            depth -= 1
        elif byte == 44 and depth == 0:
            rows.append((argument, offset))
            argument = offset + 1
    rows.append((argument, end))
    return rows


def _trim(blob: bytes, start: int, end: int) -> tuple[int, int]:
    while start < end and blob[start] in b" \t\r\n":
        start += 1
    while end > start and blob[end - 1] in b" \t\r\n":
        end -= 1
    return start, end


def _invocations(path: Path) -> list[_Invocation]:
    blob = path.read_bytes()
    masked = _mask_lexical_noise(blob)
    rows = []
    for match in TOKEN.finditer(masked):
        opening = masked.find(b"(", match.start(), match.end())
        depth = 1
        closing = opening + 1
        while closing < len(masked) and depth:
            if masked[closing] == 40:
                depth += 1
            elif masked[closing] == 41:
                depth -= 1
            closing += 1
        if depth:
            raise ValueError(f"{path}: unterminated DATA_COMPGEN")
        arguments = _split_arguments(masked, opening + 1, closing - 1)
        if len(arguments) != 3:
            line = blob.count(b"\n", 0, match.start()) + 1
            raise ValueError(f"{path}:{line}: DATA_COMPGEN requires three arguments")
        arguments = [_trim(blob, *argument) for argument in arguments]
        address = blob[arguments[0][0]:arguments[0][1]]
        semantic = blob[arguments[1][0]:arguments[1][1]]
        line = blob.count(b"\n", 0, match.start()) + 1
        if not ADDRESS.fullmatch(address) or not IDENTIFIER.fullmatch(semantic):
            raise ValueError(f"{path}:{line}: invalid DATA_COMPGEN identity")
        rows.append(_Invocation(
            match.start(), int(address, 16), semantic.decode("ascii"),
            arguments[2][0], arguments[2][1]))
    return rows


def _guard_claims(path: Path, source_root: Path, repo: Path):
    blob = path.read_bytes()
    masked = _mask_lexical_noise(blob)
    unit = path.relative_to(source_root).with_suffix("").as_posix()
    rows = []
    for match in GUARD_TOKEN.finditer(masked):
        opening = masked.find(b"(", match.start(), match.end())
        depth = 1
        closing = opening + 1
        while closing < len(masked) and depth:
            if masked[closing] == 40:
                depth += 1
            elif masked[closing] == 41:
                depth -= 1
            closing += 1
        arguments = (_split_arguments(masked, opening + 1, closing - 1)
                     if depth == 0 else [])
        line = blob.count(b"\n", 0, match.start()) + 1
        if len(arguments) != 3:
            raise ValueError(f"{path}:{line}: DATA_COMPGEN_GUARD requires three arguments")
        arguments = [_trim(blob, *argument) for argument in arguments]
        address = blob[arguments[0][0]:arguments[0][1]]
        semantic = blob[arguments[1][0]:arguments[1][1]]
        owner = blob[arguments[2][0]:arguments[2][1]]
        if (not ADDRESS.fullmatch(address) or not IDENTIFIER.fullmatch(semantic)
                or not IDENTIFIER.fullmatch(owner)):
            raise ValueError(f"{path}:{line}: invalid DATA_COMPGEN_GUARD identity")
        try:
            relative = path.relative_to(repo).as_posix()
        except ValueError:
            relative = path.as_posix()
        rows.append(CompgenDataClaim(
            unit, int(address, 16) - IMAGE_BASE, semantic.decode("ascii"),
            "STATIC_INIT_GUARD", 4, f"{relative}:{line}",
            f"{relative}:{line}", (owner.decode("ascii"),)))
    return rows


FUNCTION_KINDS = {
    ci.CursorKind.CONSTRUCTOR,
    ci.CursorKind.CONVERSION_FUNCTION,
    ci.CursorKind.CXX_METHOD,
    ci.CursorKind.DESTRUCTOR,
    ci.CursorKind.FUNCTION_DECL,
    ci.CursorKind.FUNCTION_TEMPLATE,
}


def _literal_inventory(path: Path, repo: Path, wanted_lines: set[int]):
    configure_libclang()
    translation = ci.Index.create().parse(
        str(path),
        args=_clang_args(repo, path, mode=ClangMode.RETAIL_ANALYSIS),
    )
    errors = [diagnostic for diagnostic in translation.diagnostics
              if diagnostic.severity >= ci.Diagnostic.Error]
    if errors:
        detail = "; ".join(str(diagnostic) for diagnostic in errors[:5])
        raise ValueError(f"{path}: Clang could not recover DATA_COMPGEN values: {detail}")
    rows = {}
    resolved_path = path.resolve()

    def visit(cursor, owner=None):
        location = cursor.location
        if (cursor.kind != ci.CursorKind.TRANSLATION_UNIT and
                location.file is not None and
                Path(str(location.file)).resolve() != resolved_path):
            # Included declarations cannot own a source-local claim. Pruning
            # their subtrees is also important for large TUs such as KB.cpp.
            return
        if cursor.kind in FUNCTION_KINDS and cursor.mangled_name:
            owner = cursor.mangled_name
        elif cursor.kind == ci.CursorKind.VAR_DECL and cursor.mangled_name:
            owner = cursor.mangled_name
        if (location.file is not None and
                Path(str(location.file)).resolve() == resolved_path and
                location.line in wanted_lines and
                cursor.kind in (
                    ci.CursorKind.FLOATING_LITERAL,
                    ci.CursorKind.STRING_LITERAL)):
            size = cursor.type.get_size()
            if cursor.kind == ci.CursorKind.STRING_LITERAL:
                kind = "STRING_LITERAL"
            elif size == 4:
                kind = "FLOAT32_LITERAL"
            elif size == 8:
                kind = "FLOAT64_LITERAL"
            else:
                raise ValueError(
                    f"{path}:{cursor.location.line}: unsupported floating "
                    f"literal size {size}")
            rows.setdefault(cursor.location.offset, []).append(
                (kind, size, cursor.location.line, owner))
        elif (location.file is not None and
              Path(str(location.file)).resolve() == resolved_path and
              location.line in wanted_lines and
              cursor.type.kind in (ci.TypeKind.FLOAT, ci.TypeKind.DOUBLE)):
            size = cursor.type.get_size()
            kind = "FLOAT32_LITERAL" if size == 4 else "FLOAT64_LITERAL"
            rows.setdefault(cursor.location.offset, []).append(
                (kind, size, cursor.location.line, owner))
        for child in cursor.get_children():
            visit(child, owner)

    visit(translation.cursor)
    del translation
    gc.collect()
    return rows


def _macro_expansion_offsets(blob: bytes, masked: bytes,
                             invocation: _Invocation) -> list[int]:
    line_start = blob.rfind(b"\n", 0, invocation.offset) + 1
    line_end = blob.find(b"\n", invocation.offset)
    if line_end < 0:
        line_end = len(blob)
    prefix = masked[line_start:invocation.offset]
    match = re.match(rb"\s*#\s*define\s+([A-Za-z_][A-Za-z0-9_]*)\b", prefix)
    if not match:
        return [invocation.offset]
    macro_name = match.group(1)
    token = re.compile(rb"\b" + re.escape(macro_name) + rb"\b")
    return [row.start() for row in token.finditer(masked)
            if not line_start <= row.start() < line_end]


def _macro_definitions(blob: bytes) -> dict[bytes, bytes]:
    return {
        match.group(1): match.group(2).strip()
        for match in re.finditer(
            rb"(?m)^\s*#\s*define\s+([A-Za-z_][A-Za-z0-9_]*)\s+([^\r\n]+)",
            blob)
    }


def _strip_comments(expression: bytes) -> bytes:
    """Remove comments without interpreting comment markers inside literals."""
    out = bytearray(expression)
    index = 0
    state = "code"
    quote = 0
    while index < len(expression):
        byte = expression[index]
        following = expression[index + 1] if index + 1 < len(expression) else 0
        if state == "code":
            if byte == 47 and following == 47:
                out[index:index + 2] = b"  "
                index += 2
                state = "line"
                continue
            if byte == 47 and following == 42:
                out[index:index + 2] = b"  "
                index += 2
                state = "block"
                continue
            if byte in (34, 39):
                quote = byte
                state = "literal"
        elif state == "line":
            if byte == 10:
                state = "code"
            else:
                out[index] = 32
        elif state == "block":
            if byte == 42 and following == 47:
                out[index:index + 2] = b"  "
                index += 2
                state = "code"
                continue
            if byte != 10:
                out[index] = 32
        else:
            if byte == 92 and index + 1 < len(expression):
                index += 2
                continue
            if byte == quote:
                state = "code"
        index += 1
    if state == "block":
        raise ValueError("unterminated source comment")
    return bytes(out)


def _string_size(expression: bytes, blob: bytes,
                 definitions: dict[bytes, bytes] | None = None) -> int | None:
    definitions = definitions if definitions is not None else _macro_definitions(blob)
    expression = expression.strip()
    cast = re.fullmatch(
        rb"(?:const_cast|static_cast)\s*<\s*(?:const\s+)?char\s*\*\s*>\s*\((.*)\)",
        expression,
        re.DOTALL,
    )
    if cast:
        expression = cast.group(1).strip()
    position = 0
    values = []
    while position < len(expression):
        whitespace = re.match(rb"\s*", expression[position:])
        position += whitespace.end()
        if position == len(expression):
            break
        string = STRING_TOKEN.match(expression, position)
        if string:
            token = string.group(0)
            quote = token.find(b'"')
            values.append(ast.literal_eval(token[quote:].decode("latin-1")).encode("latin-1"))
            position = string.end()
            continue
        identifier = re.match(rb"[A-Za-z_][A-Za-z0-9_]*", expression[position:])
        if identifier and identifier.group(0) in definitions:
            nested = _string_size(
                definitions[identifier.group(0)], blob, definitions)
            if nested is None:
                return None
            values.append(b"\0" * (nested - 1))
            position += identifier.end()
            continue
        return None
    return sum(map(len, values)) + 1 if values else None


def _lexical_expression(expression: bytes, blob: bytes,
                        seen: frozenset[bytes] = frozenset(),
                        definitions: dict[bytes, bytes] | None = None):
    expression = _strip_comments(expression).strip()
    definitions = definitions if definitions is not None else _macro_definitions(blob)
    string_size = _string_size(expression, blob, definitions)
    if string_size is not None:
        return "STRING_LITERAL", string_size
    floating = FLOAT_TOKEN.fullmatch(expression)
    if floating:
        size = 4 if floating.group(1) else 8
        return ("FLOAT32_LITERAL" if size == 4 else "FLOAT64_LITERAL"), size
    cast_floating = re.fullmatch(
        rb"static_cast\s*<\s*(float|double)\s*>\s*\(.*\)",
        expression,
        re.DOTALL,
    )
    if cast_floating:
        size = 4 if cast_floating.group(1) == b"float" else 8
        return ("FLOAT32_LITERAL" if size == 4 else "FLOAT64_LITERAL"), size
    if (len(expression) >= 2 and expression.startswith(b"(")
            and expression.endswith(b")")):
        depth = 0
        encloses = True
        for index, byte in enumerate(expression):
            if byte == 40:
                depth += 1
            elif byte == 41:
                depth -= 1
                if depth == 0 and index != len(expression) - 1:
                    encloses = False
                    break
        if encloses:
            return _lexical_expression(
                expression[1:-1], blob, seen, definitions)
    if IDENTIFIER.fullmatch(expression) and expression not in seen:
        definition = definitions.get(expression)
        if definition is not None:
            return _lexical_expression(
                definition.rstrip(b"\\").strip(), blob,
                seen | frozenset((expression,)), definitions)
    return None


def _lexical_literal(blob: bytes, invocation: _Invocation,
                     definitions: dict[bytes, bytes] | None = None):
    return _lexical_expression(
        blob[invocation.value_start:invocation.value_end], blob,
        definitions=definitions)


def claims_for_file(path: Path, source_root: Path,
                    repo: Path) -> list[CompgenDataClaim]:
    invocations = _invocations(path)
    if not invocations:
        return []
    blob = path.read_bytes()
    masked = _mask_lexical_noise(blob)
    expansion_offsets = {
        offset
        for invocation in invocations
        for offset in _macro_expansion_offsets(blob, masked, invocation)
    }
    wanted_lines = {
        blob.count(b"\n", 0, offset) + 1
        for offset in expansion_offsets
    }
    wanted_lines.update(
        blob.count(b"\n", 0, invocation.offset) + 1
        for invocation in invocations
    )
    definitions = _macro_definitions(blob)
    lexical_by_offset = {
        invocation.offset: _lexical_literal(blob, invocation, definitions)
        for invocation in invocations
    }
    needs_clang = any(value is None for value in lexical_by_offset.values())
    literals = (_literal_inventory(path, repo, wanted_lines)
                if needs_clang else {})
    unit = path.relative_to(source_root).with_suffix("").as_posix()
    rows = []
    for invocation in invocations:
        invocation_expansions = _macro_expansion_offsets(blob, masked, invocation)
        literal_rows = [row for offset in invocation_expansions
                        for row in literals.get(offset, ())]
        line = blob.count(b"\n", 0, invocation.offset) + 1
        if not literal_rows:
            same_line = [row for values in literals.values() for row in values
                         if row[2] == line]
            if len(same_line) == 1:
                literal_rows = same_line
        lexical = lexical_by_offset[invocation.offset]
        if lexical is not None:
            # The macro's written literal fixes the allocation type. Expansion
            # contexts may introduce additional implicit floating conversions.
            matches = {lexical}
        else:
            matches = set((kind, size) for kind, size, _line, _owner
                          in literal_rows)
        if len(matches) != 1:
            raise ValueError(
                f"{path}:{line}: DATA_COMPGEN value has {len(matches)} Clang literals")
        kind, size = matches.pop()
        value_line = literal_rows[0][2] if literal_rows else line
        owners = tuple(sorted({row[3] for row in literal_rows if row[3]}))
        if invocation.address < IMAGE_BASE or size <= 0:
            raise ValueError(f"{path}:{line}: invalid DATA_COMPGEN address or size")
        try:
            relative = path.relative_to(repo).as_posix()
        except ValueError:
            relative = path.as_posix()
        rows.append(CompgenDataClaim(
            unit, invocation.address - IMAGE_BASE, invocation.semantic_name,
            kind, size, f"{relative}:{line}", f"{relative}:{value_line}", owners))
    return rows


def _isolated_claims(path: Path, source_root: Path,
                     repo: Path) -> list[CompgenDataClaim]:
    process = subprocess.run(
        [sys.executable, "-m", "homm2.build.annotated_compgen_data",
         "--file", str(path), "--source-root", str(source_root),
         "--repo", str(repo)],
        check=False, capture_output=True, text=True)
    if process.returncode:
        raise ValueError(
            f"{path}: isolated DATA_COMPGEN parse failed: "
            f"{process.stderr.strip() or process.stdout.strip()}")
    return [
        CompgenDataClaim(**{**row, "owners": tuple(row["owners"])})
        for row in json.loads(process.stdout)
    ]


def _cache_key(path: Path, repo: Path) -> str:
    digest = hashlib.sha256()
    digest.update(f"annotated-compgen-data-v{CACHE_SCHEMA}\0".encode("ascii"))
    digest.update(Path(__file__).read_bytes())
    digest.update(Path(__file__).with_name("annotated_data.py").read_bytes())
    compile_database = repo / "build/clangd/compile_commands.json"
    if compile_database.is_file():
        digest.update(compile_database.read_bytes())
    digest.update(path.read_bytes())
    include_roots = [repo / "include"]
    vendor = repo / "vendor"
    if vendor.is_dir():
        include_roots.extend(path for path in vendor.iterdir() if path.is_dir())
    for dependency in _source_dependencies(path, include_roots):
        digest.update(str(dependency).encode("utf-8"))
        digest.update(dependency.read_bytes())
    return digest.hexdigest()


def _load_cache(path: Path) -> dict:
    try:
        payload = json.loads(path.read_text())
    except (FileNotFoundError, json.JSONDecodeError, OSError):
        return {}
    if payload.get("schema") != CACHE_SCHEMA:
        return {}
    return payload.get("entries", {})


def _write_cache(path: Path, entries: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(
            "w", encoding="utf-8", prefix=f".{path.name}.",
            dir=path.parent, delete=False) as stream:
        temporary = Path(stream.name)
        json.dump({"schema": CACHE_SCHEMA, "entries": entries}, stream,
                  separators=(",", ":"))
    os.replace(temporary, path)


def source_compgen_data(source_root: Path,
                        repo: Path,
                        cache_path: Path | None = None) -> list[CompgenDataClaim]:
    source_root = Path(source_root).resolve()
    repo = Path(repo).resolve()
    canonical = source_root == (repo / "src").resolve()
    if cache_path is None and canonical:
        cache_path = repo / "build/gen/annotated_compgen_data_cache.json"
    cache = _load_cache(cache_path) if cache_path is not None else {}
    updated_cache = dict(cache)
    rows = []
    clang_paths = []
    keys = {}
    for path in sorted(source_root.rglob("*.cpp")):
        resolved = path.resolve()
        key = _cache_key(resolved, repo) if cache_path is not None else None
        unit = resolved.relative_to(source_root).with_suffix("").as_posix()
        entry = cache.get(unit)
        if entry is not None and entry.get("key") == key:
            rows.extend(
                CompgenDataClaim(**{**row, "owners": tuple(row["owners"])})
                for row in entry.get("rows", ())
            )
            continue
        blob = resolved.read_bytes()
        invocations = _invocations(resolved)
        keys[resolved] = (unit, key)
        definitions = _macro_definitions(blob)
        if any(_lexical_literal(blob, invocation, definitions) is None
               for invocation in invocations):
            # libclang retains substantial per-TU state in long-lived Python
            # processes. Isolate the small set of claims which really need AST
            # type recovery; literal-only TUs remain an inexpensive lexical pass.
            clang_paths.append(resolved)
        else:
            claims = [*claims_for_file(resolved, source_root, repo),
                      *_guard_claims(resolved, source_root, repo)]
            rows.extend(claims)
            if cache_path is not None:
                updated_cache[unit] = {
                    "key": key, "rows": [row.__dict__ for row in claims]}
    with ThreadPoolExecutor(max_workers=min(2, len(clang_paths) or 1)) as pool:
        for path, claims in zip(clang_paths, pool.map(
                lambda path: _isolated_claims(path, source_root, repo),
                clang_paths)):
            claims = [*claims, *_guard_claims(path, source_root, repo)]
            rows.extend(claims)
            if cache_path is not None:
                unit, key = keys[path]
                updated_cache[unit] = {
                    "key": key, "rows": [row.__dict__ for row in claims]}
    if cache_path is not None and updated_cache != cache:
        _write_cache(cache_path, updated_cache)
    semantic = [(row.unit, row.semantic_name) for row in rows]
    if len(set(semantic)) != len(semantic):
        duplicates = sorted({identity for identity in semantic
                             if semantic.count(identity) > 1})
        raise ValueError(
            f"duplicate DATA_COMPGEN semantic identity within a TU: {duplicates}")
    rvas = [row.rva for row in rows]
    if len(set(rvas)) != len(rvas):
        duplicates = sorted({rva for rva in rvas if rvas.count(rva) > 1})
        raise ValueError(
            "duplicate DATA_COMPGEN retail RVA: "
            + ", ".join(f"0x{rva:x}" for rva in duplicates[:8]))
    return sorted(rows, key=lambda row: (row.unit, row.rva, row.semantic_name))


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--file", type=Path, required=True)
    parser.add_argument("--source-root", type=Path, required=True)
    parser.add_argument("--repo", type=Path, required=True)
    args = parser.parse_args(argv)
    rows = claims_for_file(
        args.file.resolve(), args.source_root.resolve(), args.repo.resolve())
    print(json.dumps([row.__dict__ for row in rows]))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
