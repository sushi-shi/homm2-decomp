#!/usr/bin/env python3
"""Generate regular-COFF Smacker and NetBIOS import libraries.

The checked-in DEF is sufficient for generic links when ``--definition-only``
is explicit. Otherwise the supported retail image (or ``--exe`` override) adds
an import-table assertion for the exact-link evidence path.
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

from homm2.build.legacy_import_lib import ExportSpec
from homm2.build.link_exe import RETAIL_EXE, read_imports
from homm2.build.regular_import_lib import (
    build_named_specs_archive,
    build_ordinal_specs_archive,
)


ORDINAL = re.compile(
    r"^(?P<name>[A-Za-z_][A-Za-z0-9_]*@(?P<bytes>\d+))\s+"
    r"@(?P<ordinal>\d+)\s+NONAME$",
    re.IGNORECASE,
)


def definition_lines(path: Path, expected_dll: str) -> list[str]:
    library = None
    in_exports = False
    exports = []
    for number, raw_line in enumerate(
        path.read_text(encoding="ascii").splitlines(), start=1
    ):
        line = raw_line.split(";", 1)[0].strip()
        if not line:
            continue
        keyword, _, remainder = line.partition(" ")
        if keyword.upper() == "LIBRARY":
            if library is not None or not remainder.strip():
                raise ValueError(f"{path}:{number}: invalid LIBRARY directive")
            library = remainder.strip()
        elif keyword.upper() == "EXPORTS" and not remainder.strip():
            in_exports = True
        elif in_exports:
            exports.append(line)
        else:
            raise ValueError(f"{path}:{number}: expected LIBRARY or EXPORTS")
    if library is None or library.lower() != expected_dll.lower():
        raise ValueError(f"{path}: expected LIBRARY {expected_dll}")
    if not exports:
        raise ValueError(f"{path}: no exports")
    return exports


def retail_symbols(exe: Path, dll: str) -> list[dict]:
    matches = [
        descriptor["symbols"]
        for descriptor in read_imports(exe)
        if descriptor["dll"].lower() == dll.lower()
    ]
    if len(matches) != 1:
        raise ValueError(f"{exe}: expected one {dll} import descriptor")
    return matches[0]


def generate_ordinal(
    exe: Path | None, dll: str, definition: Path, output: Path
) -> Path:
    specs = []
    for line in definition_lines(definition, dll):
        match = ORDINAL.fullmatch(line)
        if match is None:
            raise ValueError(f"{definition}: expected name@bytes @ordinal NONAME")
        name = match.group("name")
        specs.append(
            ExportSpec(
                lookup=name,
                caller="_" + name,
                argument_bytes=int(match.group("bytes")),
                ordinal=int(match.group("ordinal")),
            )
        )
    if exe is not None:
        expected = [symbol["ordinal"] for symbol in retail_symbols(exe, dll)]
        actual = [spec.ordinal for spec in specs]
        if sorted(actual) != sorted(expected):
            raise ValueError(
                f"{definition}: ordinal set {actual} does not match retail {expected}"
            )
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(build_ordinal_specs_archive(dll, specs))
    print(f"[regular-implib] {dll}: {len(specs)} ordinal imports -> {output}")
    return output


def generate_named_alias(
    exe: Path | None,
    dll: str,
    definition: Path,
    output: Path,
    symbol: str,
    lookup: str,
    hint: int,
) -> Path:
    lines = definition_lines(definition, dll)
    if lines != [symbol]:
        raise ValueError(f"{definition}: expected sole export {symbol}")
    if exe is not None:
        expected = retail_symbols(exe, dll)
        if expected != [{"name": lookup, "hint": hint}]:
            raise ValueError(
                f"{definition}: requested {lookup!r}/{hint} does not match retail {expected}"
            )
    match = re.fullmatch(r"^_[A-Za-z_][A-Za-z0-9_]*@(\d+)$", symbol)
    if match is None:
        raise ValueError(f"{definition}: expected _stdcall@bytes export")
    spec = ExportSpec(lookup, symbol, int(match.group(1)), hint + 1)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(
        build_named_specs_archive(dll, [spec], {lookup: hint}, {lookup})
    )
    print(f"[regular-implib] {dll}: {lookup} hint {hint} -> {output}")
    return output


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    source = parser.add_mutually_exclusive_group()
    source.add_argument("--exe", type=Path)
    source.add_argument("--definition-only", action="store_true")
    parser.add_argument("--dll", required=True)
    parser.add_argument("--definition", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--symbol")
    parser.add_argument("--lookup")
    parser.add_argument("--hint", type=int)
    args = parser.parse_args(argv)
    try:
        verify_exe = None if args.definition_only else (args.exe or RETAIL_EXE)
        alias = (args.symbol, args.lookup, args.hint)
        if alias == (None, None, None):
            generate_ordinal(verify_exe, args.dll, args.definition, args.out)
        elif None not in alias:
            generate_named_alias(
                verify_exe,
                args.dll,
                args.definition,
                args.out,
                args.symbol,
                args.lookup,
                args.hint,
            )
        else:
            raise ValueError("--symbol, --lookup, and --hint must be used together")
    except (OSError, ValueError) as error:
        print(f"[regular-implib] ERROR: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
