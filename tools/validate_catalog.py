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


def metadata(entries):
    for entry in entries:
        if entry.get("msgid") == "":
            result = {}
            for line in entry.get("msgstr", "").splitlines():
                if ":" in line:
                    name, value = line.split(":", 1)
                    result[name.strip().lower()] = value.strip()
            return result
    return {}


def profile_supports(profile, text):
    for character in text:
        codepoint = ord(character)
        if character in "\b\n\r\t" or 0x20 <= codepoint <= 0x7f:
            continue
        if profile == "buka-cyrillic" and (
            0x0400 <= codepoint <= 0x04ff or codepoint in (0x2013, 0x2014)
        ):
            continue
        return False, character
    return True, None


def main():
    if len(sys.argv) != 3:
        raise SystemExit("usage: validate_catalog.py CATALOG.po TEMPLATE.pot")
    catalog_path = pathlib.Path(sys.argv[1])
    template_path = pathlib.Path(sys.argv[2])
    catalog_entries = parse(catalog_path)
    catalog = {
        entry["msgctxt"]: entry
        for entry in catalog_entries
        if "msgctxt" in entry
    }
    template = messages(template_path)
    errors = []
    fields = metadata(catalog_entries)
    charset = fields.get("content-type", "").lower().replace(" ", "")
    if "charset=utf-8" not in charset:
        errors.append("catalog Content-Type must declare UTF-8")
    language = fields.get("language", "").lower().replace("_", "-")
    if not language:
        errors.append("catalog has no Language metadata")
    elif language.split("-", 1)[0] != catalog_path.stem.lower().split("-", 1)[0]:
        errors.append(
            f"Language metadata {language!r} does not match filename {catalog_path.name!r}"
        )
    profile = fields.get("x-homm2-required-resource-profile", "").lower()
    if profile not in ("western", "buka-cyrillic"):
        errors.append(
            "X-Homm2-Required-Resource-Profile must be 'western' or 'buka-cyrillic'"
        )
    for context, entry in catalog.items():
        reference = template.get(context)
        if reference is None:
            errors.append(f"translated context {context!r} is no longer defined")
            continue
        for field in ("msgid", "msgid_plural"):
            if entry.get(field) != reference.get(field):
                errors.append(f"{context!r} has stale {field}")
        if profile in ("western", "buka-cyrillic"):
            for key, text in entry.items():
                if not key.startswith("msgstr") or not text:
                    continue
                supported, character = profile_supports(profile, text)
                if not supported:
                    errors.append(
                        f"{context!r} uses {character!r}, which is not renderable by "
                        f"resource profile {profile!r}"
                    )
                    break
    if errors:
        for error in errors:
            print(f"{catalog_path}: error: {error}", file=sys.stderr)
        raise SystemExit(1)


if __name__ == "__main__":
    main()
