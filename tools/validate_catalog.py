#!/usr/bin/env python3
"""Validate translated PO entries against the generated authoritative POT.

Unlike msgcmp, incomplete catalogs are accepted: missing entries use English.
Every entry which does exist must still have the current context and English
source text, even when its translation is empty, so stale snapshots fail the
build instead of drifting away from the single source of truth.
"""

import ast
import pathlib
import re
import sys


DIRECTIVE = re.compile(
    r'^(msgctxt|msgid|msgid_plural|msgstr(?:\[\d+\])?)\s+(".*")\s*$'
)


def parse(path):
    entries = []
    fields = {}
    active = None

    def finish():
        nonlocal fields, active
        if fields:
            entries.append(fields)
        fields = {}
        active = None

    for number, raw in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        line = raw.strip()
        if not line:
            finish()
            continue
        if line.startswith("#~"):
            continue
        match = DIRECTIVE.match(line)
        if match:
            active = match.group(1)
            fields[active] = ast.literal_eval(match.group(2))
        elif line.startswith('"') and active is not None:
            fields[active] += ast.literal_eval(line)
        elif not line.startswith("#"):
            raise ValueError(f"{path}:{number}: unsupported PO syntax")
    finish()
    return entries


def messages(path):
    result = {}
    for entry in parse(path):
        context = entry.get("msgctxt")
        if context is None:
            continue
        if context in result:
            raise ValueError(f"{path}: duplicate context {context!r}")
        result[context] = entry
    return result


def translated(entry):
    return any(value for key, value in entry.items() if key.startswith("msgstr"))


def main():
    if len(sys.argv) != 3:
        raise SystemExit("usage: validate_catalog.py CATALOG.po TEMPLATE.pot")
    catalog_path = pathlib.Path(sys.argv[1])
    template_path = pathlib.Path(sys.argv[2])
    catalog = messages(catalog_path)
    template = messages(template_path)
    errors = []
    for context, entry in catalog.items():
        reference = template.get(context)
        if reference is None:
            errors.append(f"translated context {context!r} is no longer defined")
            continue
        for field in ("msgid", "msgid_plural"):
            if entry.get(field) != reference.get(field):
                errors.append(f"{context!r} has stale {field}")
    if errors:
        for error in errors:
            print(f"{catalog_path}: error: {error}", file=sys.stderr)
        raise SystemExit(1)


if __name__ == "__main__":
    main()
