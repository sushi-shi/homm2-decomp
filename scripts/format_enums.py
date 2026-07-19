#!/usr/bin/env python3
"""Align assignments within each H2 enum macro block."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path


BEGIN_RE = re.compile(r"^H2_ENUM_(?:CLASS_)?BEGIN(?:_T)?\(")
END_RE = re.compile(r"^H2_ENUM_(?:CLASS_)?END(?:_T)?\(")
ASSIGN_RE = re.compile(r"^(\s*)([A-Za-z_][A-Za-z0-9_]*)(?:\s*)=(?:\s*)(.*)$")


def format_text(text: str) -> str:
    lines = text.splitlines(keepends=True)
    first = None

    for index, line in enumerate(lines):
        content = line.rstrip("\r\n")
        if first is None:
            if BEGIN_RE.match(content):
                first = index + 1
            continue
        if not END_RE.match(content):
            continue

        assignments = []
        for enum_index in range(first, index):
            enum_content = lines[enum_index].rstrip("\r\n")
            match = ASSIGN_RE.match(enum_content)
            if match:
                assignments.append((enum_index, match))

        if assignments:
            width = max(len(match.group(2)) for _, match in assignments)
            for enum_index, match in assignments:
                ending = lines[enum_index][len(lines[enum_index].rstrip("\r\n")) :]
                right_hand_side = match.group(3)
                lines[enum_index] = (
                    f"{match.group(1)}{match.group(2):<{width}} ="
                    f"{' ' + right_hand_side if right_hand_side else ''}{ending}"
                )
        first = None

    return "".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true")
    parser.add_argument("paths", nargs="+", help="files to format, or - for stdin")
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
