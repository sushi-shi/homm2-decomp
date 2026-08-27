#!/usr/bin/env python3

from __future__ import annotations

import re
import sys
from pathlib import Path


POLICIES = (
    (
        re.compile(r"(?<![A-Za-z0-9_])(?:sprintf|vsprintf)\s*\("),
        "unbounded printf formatting; use utf8::Format",
    ),
    (
        re.compile(r"\bFile(?:Read|Write)\s*\("),
        "partial application file transfer; use FileReadExact/FileWriteExact",
    ),
)

ENUM_DECLARATION = re.compile(r"\benum\s+class\s+(\w+)")
TYPEDEF_ALIAS = re.compile(r"\btypedef\s+((?:(?:\w+)::)*\w+)\s+(\w+)\s*;")
USING_ALIAS = re.compile(r"\busing\s+(\w+)\s*=\s*((?:(?:\w+)::)*\w+)\s*;")
ENUM_CAST = re.compile(r"\bstatic_cast\s*<\s*(?:(?:\w+)::)*(\w+)\s*>")
ENUM_BOUNDARY_MARKER = "H2_ENUM_CODE_BOUNDARY"


def main() -> int:
    root = Path(sys.argv[1] if len(sys.argv) > 1 else ".").resolve()
    failures: list[str] = []
    sources = [
        path
        for directory in (root / "include", root / "src")
        for path in sorted(directory.rglob("*"))
        if path.suffix in {".cpp", ".h"}
    ]
    contents = {path: path.read_text(encoding="utf-8") for path in sources}
    enum_names = {
        match.group(1)
        for source in contents.values()
        for match in ENUM_DECLARATION.finditer(source)
    }
    aliases = [
        (match.group(1).rsplit("::", 1)[-1], match.group(2))
        for source in contents.values()
        for match in TYPEDEF_ALIAS.finditer(source)
    ]
    aliases.extend(
        (match.group(2).rsplit("::", 1)[-1], match.group(1))
        for source in contents.values()
        for match in USING_ALIAS.finditer(source)
    )
    while True:
        discovered = {
            alias for target, alias in aliases if target in enum_names and alias not in enum_names
        }
        if not discovered:
            break
        enum_names.update(discovered)
    for path, source in contents.items():
        relative = path.relative_to(root)
        lines = source.splitlines()
        for line_number, line in enumerate(lines, start=1):
            for pattern, explanation in POLICIES:
                if pattern.search(line):
                    failures.append(
                        f"{relative}:{line_number}: {explanation}\n  {line.strip()}"
                    )
            if "h2_enum_code_detail::Convert" in line and relative.as_posix() != "include/EnumCode.h":
                failures.append(
                    f"{relative}:{line_number}: bypasses the named enum-code boundary\n"
                    f"  {line.strip()}"
                )
        for match in ENUM_CAST.finditer(source):
            if match.group(1) not in enum_names:
                continue
            line_number = source.count("\n", 0, match.start()) + 1
            line = lines[line_number - 1]
            if ENUM_BOUNDARY_MARKER not in line:
                failures.append(
                    f"{relative}:{line_number}: direct numeric conversion to enum; "
                    "use a domain-named FromCode/FromOrdinal boundary\n"
                    f"  {line.strip()}"
                )
    if failures:
        print("\n".join(failures), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
