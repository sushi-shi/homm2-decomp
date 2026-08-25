#!/usr/bin/env python3
"""Regression tests for the catalog/source-of-truth contract."""

import pathlib
import subprocess
import sys
import tempfile


VALIDATOR = pathlib.Path(__file__).with_name("validate_catalog.py")
HEADER = '''msgid ""
msgstr ""
"Language: test\\n"
"Content-Type: text/plain; charset=UTF-8\\n"
"X-Homm2-Required-Resource-Profile: western\\n"

'''
TEMPLATE = '''msgid ""
msgstr ""
"Content-Type: text/plain; charset=UTF-8\\n"

msgctxt "present"
msgid "Current English"
msgstr ""

msgctxt "optional"
msgid "Optional English"
msgstr ""
'''


def validate(catalog, template):
    return subprocess.run(
        [sys.executable, VALIDATOR, catalog, template],
        check=False,
        capture_output=True,
        text=True,
    )


def main():
    with tempfile.TemporaryDirectory() as directory:
        root = pathlib.Path(directory)
        template = root / "messages.pot"
        catalog = root / "test.po"
        template.write_text(TEMPLATE, encoding="utf-8")

        # A missing entry is a supported incomplete catalog and falls back to
        # authoritative English.
        catalog.write_text(
            HEADER
            + '''msgctxt "present"
msgid "Current English"
msgstr "Translation"
''',
            encoding="utf-8",
        )
        assert validate(catalog, template).returncode == 0

        # An empty translation is still a PO entry. Its English snapshot must
        # not be allowed to drift away from the registry.
        catalog.write_text(
            HEADER
            + '''msgctxt "present"
msgid "Old English"
msgstr ""
''',
            encoding="utf-8",
        )
        result = validate(catalog, template)
        assert result.returncode != 0
        assert "stale msgid" in result.stderr

        catalog.write_text(
            HEADER
            + '''msgctxt "removed"
msgid "Removed English"
msgstr "Translation"
''',
            encoding="utf-8",
        )
        result = validate(catalog, template)
        assert result.returncode != 0
        assert "no longer defined" in result.stderr

        catalog.write_text(
            HEADER.replace("western", "unknown")
            + '''msgctxt "present"
msgid "Current English"
msgstr "Translation"
''',
            encoding="utf-8",
        )
        result = validate(catalog, template)
        assert result.returncode != 0
        assert "Required-Resource-Profile" in result.stderr

        catalog.write_text(
            HEADER
            + '''msgctxt "present"
msgid "Current English"
msgstr "Привет"
''',
            encoding="utf-8",
        )
        result = validate(catalog, template)
        assert result.returncode != 0
        assert "not renderable" in result.stderr

        catalog.write_text(
            HEADER.replace("western", "buka-cyrillic")
            + '''msgctxt "present"
msgid "Current English"
msgstr "Привет"
''',
            encoding="utf-8",
        )
        assert validate(catalog, template).returncode == 0


if __name__ == "__main__":
    main()
