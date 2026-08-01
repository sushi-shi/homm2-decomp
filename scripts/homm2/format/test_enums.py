import io
import sys
import unittest
from contextlib import redirect_stderr
from pathlib import Path
from tempfile import TemporaryDirectory
from unittest import mock

from homm2.format import enums
from homm2.format.enums import format_text


class FormatEnumsTests(unittest.TestCase):
    def test_aligns_each_enum_independently(self):
        source = """\
H2_ENUM_BEGIN(First)
    SHORT = -1,
    LONG_CONSTANT = 16
H2_ENUM_END(First)

H2_ENUM_CLASS_BEGIN(Second)
    A = 1,
    BB = OTHER = 2
H2_ENUM_CLASS_END(Second)
"""
        expected = """\
H2_ENUM_BEGIN(First)
    SHORT         = -1,
    LONG_CONSTANT = 16
H2_ENUM_END(First)

H2_ENUM_CLASS_BEGIN(Second)
    A  = 1,
    BB = OTHER = 2
H2_ENUM_CLASS_END(Second)
"""
        self.assertEqual(format_text(source), expected)

    def test_is_idempotent_and_preserves_non_enum_assignments(self):
        source = """\
value = 1;
H2_ENUM_BEGIN_T(Item, u8)
    ONE   = 1,
    THREE = 3
H2_ENUM_END_T(Item, u8)
other = 2;
"""
        self.assertEqual(format_text(format_text(source)), source)

    def test_multiline_assignment_has_no_trailing_space(self):
        source = """\
H2_ENUM_BEGIN(Item)
    SHORT = 1,
    LONG_NAME =
        OTHER + 1
H2_ENUM_END(Item)
"""
        expected = """\
H2_ENUM_BEGIN(Item)
    SHORT     = 1,
    LONG_NAME =
        OTHER + 1
H2_ENUM_END(Item)
"""
        self.assertEqual(format_text(source), expected)

    def test_aligns_split_storage_enum(self):
        source = """\
H2_ENUM_CLASS_BEGIN_SPLIT(FactionType, i8)
    FACTION_UNINITIALIZED = -2,
    FACTION_ANY = -1
H2_ENUM_CLASS_END_SPLIT(FactionType, i8)
"""
        expected = """\
H2_ENUM_CLASS_BEGIN_SPLIT(FactionType, i8)
    FACTION_UNINITIALIZED = -2,
    FACTION_ANY           = -1
H2_ENUM_CLASS_END_SPLIT(FactionType, i8)
"""
        self.assertEqual(format_text(source), expected)


class CheckNamesOffendersTests(unittest.TestCase):
    """`--check` must name the file, not only set an exit code.

    It used to do only the latter, so a red `homm2 format --check` over 235 paths
    reported that something was misaligned without saying what. The single real
    offender - one line in include/SOURCE/combatTypes.h - stayed unnoticed behind
    that silence.
    """

    MISALIGNED = (
        "H2_ENUM_BEGIN(Side)\n"
        "    LONG_CONSTANT = -1,\n"
        "    SHORT = 1\n"
        "H2_ENUM_END(Side)\n"
    )

    def _check(self, text):
        with TemporaryDirectory() as directory:
            path = Path(directory) / "probe.h"
            path.write_text(text)
            stderr = io.StringIO()
            with mock.patch.object(sys, "argv", ["prog", "--check", str(path)]), \
                    redirect_stderr(stderr):
                code = enums.main()
            return code, stderr.getvalue(), path.read_text()

    def test_check_names_the_offending_file(self):
        code, stderr, _ = self._check(self.MISALIGNED)
        self.assertEqual(code, 1)
        self.assertIn("probe.h", stderr)

    def test_check_does_not_rewrite_the_file_it_reports(self):
        _, _, after = self._check(self.MISALIGNED)
        self.assertEqual(after, self.MISALIGNED)

    def test_aligned_file_is_silent_and_passes(self):
        code, stderr, _ = self._check(format_text(self.MISALIGNED))
        self.assertEqual(code, 0)
        self.assertEqual(stderr, "")


if __name__ == "__main__":
    unittest.main()
