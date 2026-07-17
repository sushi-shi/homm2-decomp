import tempfile
import unittest
from pathlib import Path

from homm2.build.assert_fixed_width_ints import violations


class FixedWidthIntegerTest(unittest.TestCase):
    def check(self, source):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "sample.cpp"
            path.write_text(source)
            return [(line, column, name) for line, column, name in violations(path)]

    def test_finds_integer_keywords_in_code(self):
        found = self.check("unsigned short value;\nstatic_cast<signed char>(value);\n")
        self.assertEqual([name for _, _, name in found],
                         ["unsigned", "short", "signed"])

    def test_ignores_comments_and_literals(self):
        found = self.check("// unsigned int\nconst char *s = \"long short\";\n"
                           "char c = 'i'; /* signed char */\n")
        self.assertEqual(found, [])

    def test_accepts_aliases_plain_char_and_long_double(self):
        found = self.check("i8 a; u8 b; i16 c; u16 d; i32 e; u32 f;\n"
                           "i32l legacyLong; u32l legacyULong; i64 g; u64 h;\n"
                           "char text[4]; long double ratio;\n")
        self.assertEqual(found, [])

    def test_does_not_confuse_separate_long_and_double_declarations(self):
        found = self.check("long value; double ratio;\n")
        self.assertEqual([name for _, _, name in found], ["long"])


if __name__ == "__main__":
    unittest.main()
