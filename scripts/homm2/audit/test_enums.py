import tempfile
import unittest
from dataclasses import asdict
from pathlib import Path
from unittest.mock import patch

from homm2.audit.enums import collect, group_values, scan_file, source_blocks, verify_reuse


class EnumAuditTests(unittest.TestCase):
    def scan(self, text, *, arguments=()):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "src/test.cpp"
            source.parent.mkdir()
            source.write_text(text)
            return scan_file(source, ["-x", "c++", "-std=c++98", *arguments], root=root)

    def test_evaluates_expressions_aliases_and_implicit_members(self):
        rows = self.scan("enum Value { NEG = -1, ZERO, HEX = 0x10, SHIFT = 1 << 4, ALIAS = HEX, NEXT };")
        self.assertEqual({row.name: row.value for row in rows},
                         {"NEG": -1, "ZERO": 0, "HEX": 16, "SHIFT": 16, "ALIAS": 16, "NEXT": 17})

    def test_equal_values_keep_distinct_domains_and_locations(self):
        rows = self.scan("enum First { A = 2 };\nenum Second { B = 2 };")
        groups = group_values([asdict(row) for row in rows])
        self.assertEqual(set(groups), {2})
        self.assertEqual({row["enum"] for row in groups[2]}, {"First", "Second"})
        self.assertEqual(len({row["offset"] for row in groups[2]}), 2)

    def test_preserves_macro_domain_name(self):
        rows = self.scan("#define H2_ENUM_BEGIN(name) enum {\n"
                         "#define H2_ENUM_END(name) }; typedef int name;\n"
                         "H2_ENUM_BEGIN(Domain)\n A = 7, B\nH2_ENUM_END(Domain)\n")
        self.assertEqual([(row.name, row.value, row.enum) for row in rows],
                         [("A", 7, "Domain"), ("B", 8, "Domain")])

    def test_local_enum_is_not_lost_when_pruning_function_bodies(self):
        rows = self.scan("int f() {\n enum Local { COUNT = 3 };\n return COUNT;\n}")
        self.assertEqual([(row.name, row.enum) for row in rows], [("COUNT", "Local")])

    def test_signed_and_unsigned_values_are_not_folded_together(self):
        rows = self.scan("enum Signed { NEG = -1 };\nenum Unsigned { MASK = 0xffffffffu };")
        self.assertEqual({row.value for row in rows}, {-1, 4294967295})

    def test_scoped_enum_and_typedef(self):
        rows = self.scan("enum class Scope { A = 3 };\ntypedef enum Named { B = 3 } Named;",
                         arguments=("-std=c++20",))
        self.assertEqual({row.enum for row in rows}, {"Scope", "Named"})

    def test_conditional_definitions_retain_each_context_value(self):
        text = "#ifndef VALUE\n#define VALUE 1\n#endif\nenum Domain { A = VALUE };"
        self.assertEqual(self.scan(text)[0].value, 1)
        self.assertEqual(self.scan(text, arguments=("-DVALUE=9",))[0].value, 9)

    def test_rejects_invalid_initializer(self):
        with self.assertRaisesRegex(ValueError, "parsing failed"):
            self.scan("enum Broken { VALUE = missing }; ")

    def test_rejects_missing_header(self):
        with self.assertRaisesRegex(ValueError, "file not found"):
            self.scan('#include "missing.h"\nenum Broken { VALUE = 1 };')

    def test_lexical_census_includes_inactive_blocks(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "test.h"
            path.write_text("#if 0\nenum Inactive { A };\n#endif\nenum Active { B };\n")
            self.assertEqual([row["enum"] for row in source_blocks(path)], ["Inactive", "Active"])

    def test_shared_header_is_deduplicated_but_keeps_tu_contexts(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "include").mkdir()
            (root / "src").mkdir()
            (root / "include/shared.h").write_text("enum Shared { A = 7 };\n")
            entries = []
            for name in ("first", "second"):
                source = root / f"src/{name}.cpp"
                source.write_text('#include "shared.h"\n')
                entries.append({"directory": str(root), "file": str(source)})
            args = ["-x", "c++", "-std=c++98", "-I", str(root / "include")]
            with patch("homm2.audit.enums._entries", return_value=entries), \
                 patch("homm2.audit.enums._clang_args", return_value=args):
                report = collect(root)
            self.assertEqual(len(report["constants"]), 1)
            self.assertEqual(report["constants"][0]["contexts"], ["src/first.cpp", "src/second.cpp"])
            self.assertEqual(report["unobserved_blocks"], [])

    def test_unreferenced_header_remains_a_visible_coverage_gap(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "include").mkdir()
            (root / "src").mkdir()
            (root / "include/unused.h").write_text("enum Unused { A = 7 };\n")
            source = root / "src/test.cpp"
            source.write_text("int f() { return 1; }\n")
            entries = [{"directory": str(root), "file": str(source)}]
            with patch("homm2.audit.enums._entries", return_value=entries), \
                 patch("homm2.audit.enums._clang_args", return_value=["-x", "c++"]):
                report = collect(root)
            self.assertEqual(report["constants"], [])
            self.assertEqual(report["unobserved_blocks"][0]["enum"], "Unused")


class EnumReuseTests(unittest.TestCase):
    def setUp(self):
        self.old = {"file": "include/old.h", "name": "OLD", "value": 7}
        self.target = {"file": "include/shared.h", "name": "SHARED", "value": 7}
        self.other = {"file": "include/other.h", "name": "OTHER", "value": 7}
        self.before = {"constants": [self.old, self.target, self.other]}
        self.after = {"constants": [self.target, self.other]}
        self.ledger = [{"source_file": self.old["file"], "old_name": "OLD", "value": "7",
                        "canonical_file": self.target["file"], "canonical_name": "SHARED",
                        "reason": "Same serialized field and identity."}]

    def test_accepts_direct_reuse_without_merging_unrelated_equal_value(self):
        self.assertEqual(verify_reuse(self.before, self.after, self.ledger), [])

    def test_rejects_unreviewed_deletion(self):
        self.after["constants"].pop()
        self.assertTrue(verify_reuse(self.before, self.after, self.ledger))

    def test_rejects_changed_value(self):
        self.after["constants"][1] = {**self.other, "value": 8}
        self.assertTrue(verify_reuse(self.before, self.after, self.ledger))

    def test_rejects_surviving_alias(self):
        self.after["constants"].append(self.old)
        self.assertTrue(verify_reuse(self.before, self.after, self.ledger))

    def test_rejects_duplicate_or_reasonless_decision(self):
        self.ledger[0]["reason"] = ""
        self.ledger.append(self.ledger[0])
        self.assertEqual(len(verify_reuse(self.before, self.after, self.ledger)), 3)

    def test_rejects_ambiguous_same_spelled_members(self):
        self.before["constants"].extend([{**self.other, "enum": "First"},
                                         {**self.other, "enum": "Second"}])
        with self.assertRaisesRegex(ValueError, "ambiguous member name"):
            verify_reuse(self.before, self.after, self.ledger)

    def test_rejects_malformed_ledger(self):
        with self.assertRaisesRegex(ValueError, "missing required columns"):
            verify_reuse(self.before, self.after, [{"old_name": "OLD"}])


if __name__ == "__main__":
    unittest.main()
