import io
import sys
import unittest
from contextlib import redirect_stderr
from pathlib import Path
from tempfile import TemporaryDirectory
from unittest import mock

from homm2.format import headers
from homm2.format.headers import format_text


class FormatHeadersTests(unittest.TestCase):
    def test_separates_guard_includes_and_declarations(self):
        source = """\
#ifndef HOMM2_ITEM_H
#define HOMM2_ITEM_H
#include <va.h>
#include <BASE/item.h>
class Item;
#endif
"""
        expected = """\
#ifndef HOMM2_ITEM_H
#define HOMM2_ITEM_H

#include <va.h>
#include <BASE/item.h>

class Item;
#endif
"""
        self.assertEqual(format_text(source), expected)
        self.assertEqual(format_text(expected), expected)

    def test_leaves_non_guarded_files_unchanged(self):
        source = "#pragma once\n#include <item.h>\n"
        self.assertEqual(format_text(source), source)


class CheckNamesOffendersTests(unittest.TestCase):
    """`--check` must name the file, not only set an exit code (see test_enums)."""

    UNSPACED = "#ifndef HOMM2_ITEM_H\n#define HOMM2_ITEM_H\n#include <va.h>\n#endif\n"

    def _check(self, text):
        with TemporaryDirectory() as directory:
            path = Path(directory) / "probe.h"
            path.write_text(text)
            stderr = io.StringIO()
            with mock.patch.object(sys, "argv", ["prog", "--check", str(path)]), \
                    redirect_stderr(stderr):
                code = headers.main()
            return code, stderr.getvalue(), path.read_text()

    def test_check_names_the_offending_file(self):
        code, stderr, _ = self._check(self.UNSPACED)
        self.assertEqual(code, 1)
        self.assertIn("probe.h", stderr)

    def test_check_does_not_rewrite_the_file_it_reports(self):
        _, _, after = self._check(self.UNSPACED)
        self.assertEqual(after, self.UNSPACED)

    def test_spaced_file_is_silent_and_passes(self):
        code, stderr, _ = self._check(format_text(self.UNSPACED))
        self.assertEqual(code, 0)
        self.assertEqual(stderr, "")


if __name__ == "__main__":
    unittest.main()
