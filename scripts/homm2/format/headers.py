#!/usr/bin/env python3
"""Normalize top-level spacing in project headers."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path


GUARD_RE = re.compile(r"^#ifndef\s+([A-Za-z_][A-Za-z0-9_]*)$")


def format_text(text: str) -> str:
    trailing_newline = text.endswith("\n")
    lines = text.splitlines()
    if len(lines) < 2:
        return text

    guard = GUARD_RE.match(lines[0])
    if not guard or lines[1] != f"#define {guard.group(1)}":
        return text

    body = lines[2:]
    while body and body[0] == "":
        body.pop(0)
    lines = lines[:2] + [""] + body

    first_include = 3
    if first_include < len(lines) and lines[first_include].startswith("#include "):
        after_includes = first_include
        while after_includes < len(lines) and lines[after_includes].startswith("#include "):
            after_includes += 1
        while after_includes < len(lines) and lines[after_includes] == "":
            lines.pop(after_includes)
        if after_includes < len(lines):
            lines.insert(after_includes, "")

    result = "\n".join(lines)
    return result + ("\n" if trailing_newline else "")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true")
    parser.add_argument("paths", nargs="+", help="headers to format, or - for stdin")
    args = parser.parse_args()

    changed = False
    for raw_path in args.paths:
        if raw_path == "-":
            original = sys.stdin.read()
            formatted = format_text(original)
            sys.stdout.write(formatted)
            changed = changed or formatted != original
            continue

        path = Path(raw_path)
        original = path.read_text()
        formatted = format_text(original)
        if formatted != original:
            changed = True
            if not args.check:
                path.write_text(formatted)

    return int(args.check and changed)


if __name__ == "__main__":
    raise SystemExit(main())
