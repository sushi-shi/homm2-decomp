import tempfile
import unittest
from pathlib import Path

from homm2 import constants_audit


class ConstantsAuditTest(unittest.TestCase):
    def test_lexical_inventory_classifies_contexts(self):
        source = """\
DATA(0x401000) i32 values[3] = {4, 5, 6};
enum { VALUE_SEVEN = 7 };
H2_ENUM_BEGIN(SampleValue)
    VALUE_ELEVEN = 11
H2_ENUM_END(SampleValue)
VA(0x402000, 0x20)
i32 Function(i32 value) {
    i32 table[2] = {8, 9};
    return value + 10;
}
"""
        with tempfile.TemporaryDirectory(dir=constants_audit.REPO) as directory:
            path = Path(directory) / "sample.cpp"
            path.write_text(source)
            rows = constants_audit.lexical_inventory(path)
        categories = {(row.token, row.line): row.category for row in rows}
        self.assertEqual(categories[("0x401000", 1)], "annotation")
        self.assertEqual(categories[("5", 1)], "data-payload")
        self.assertEqual(categories[("7", 2)], "enum")
        self.assertEqual(categories[("11", 4)], "enum")
        self.assertEqual(categories[("0x402000", 6)], "annotation")
        self.assertEqual(categories[("9", 8)], "local-table")
        self.assertEqual(categories[("10", 9)], "code")

    def test_diagnostics_use_source_literal_and_deduplicate(self):
        source = "void Test() {\n    return 12;\n}\n"
        with tempfile.TemporaryDirectory(dir=constants_audit.REPO) as directory:
            path = Path(directory) / "Test.cpp"
            path.write_text(source)
            relative = str(path.relative_to(constants_audit.REPO))
            lexical = [constants_audit.Literal(relative, 2, 12, "12", "code", "return 12;")]
            warning = (f"{relative}:2:12: warning: 12 is a magic number; consider replacing it "
                       "with a named constant [readability-magic-numbers]")
            rows = constants_audit._diagnostic_rows(
                warning + "\n" + warning, constants_audit.MAGIC_RE, lexical)
        self.assertEqual(rows, [{
            "path": relative,
            "line": 2,
            "column": 12,
            "literal": "12",
            "category": "code",
            "context": "return 12;",
        }])


if __name__ == "__main__":
    unittest.main()
