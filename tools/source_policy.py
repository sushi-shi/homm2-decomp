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


def main() -> int:
    root = Path(sys.argv[1] if len(sys.argv) > 1 else ".").resolve()
    failures: list[str] = []
    for directory in (root / "include", root / "src"):
        for path in sorted(directory.rglob("*")):
            if path.suffix not in {".cpp", ".h"}:
                continue
            for line_number, line in enumerate(
                path.read_text(encoding="utf-8").splitlines(), start=1
            ):
                for pattern, explanation in POLICIES:
                    if pattern.search(line):
                        relative = path.relative_to(root)
                        failures.append(
                            f"{relative}:{line_number}: {explanation}\n  {line.strip()}"
                        )
    if failures:
        print("\n".join(failures), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
