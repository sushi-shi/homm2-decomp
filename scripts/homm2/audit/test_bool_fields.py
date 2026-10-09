"""Unit tests for the Boolean audit's whole-program contract proofs.

Run inside ``nix develop .#build``::

    python -m unittest homm2.audit.test_bool_fields
"""
from __future__ import annotations

import tempfile
import textwrap
import unittest
from pathlib import Path

import clang.cindex as ci

from homm2.audit import bool_fields
from homm2.retail_labels.annotated_data import configure_libclang

HEADER = """\
typedef int i32;
typedef signed char i8;
typedef i32 b32;
typedef i8 b8;
typedef char bchar;
"""


class BooleanContractsTest(unittest.TestCase):
    def scan(self, *sources: str, exceptions: str = "") -> dict:
        configure_libclang()
        with tempfile.TemporaryDirectory() as temporary:
            repo = Path(temporary)
            (repo / "include").mkdir()
            (repo / "src").mkdir()
            (repo / "include/Ints.h").write_text(HEADER)
            if exceptions:
                manifest = repo / bool_fields.RETAIL_EXCEPTION_MANIFEST
                manifest.parent.mkdir(parents=True)
                manifest.write_text(
                    "\t".join(bool_fields.RETAIL_EXCEPTION_FIELDS) + "\n" + exceptions)
            index = ci.Index.create()
            parsed = []
            for number, text in enumerate(sources):
                path = repo / f"src/unit{number}.cpp"
                path.write_text('#include "Ints.h"\n' + textwrap.dedent(text))
                parsed.append(bool_fields.parse_translation_unit(
                    index, path, ["-x", "c++", "-std=c++98", "-I", str(repo / "include")],
                    repo))
            report = bool_fields.build_report(repo, parsed, translation_units=len(sources))
        self.assertEqual(report["parse_diagnostics"], [])
        return report

    @staticmethod
    def names(rows: list[dict]) -> list[str]:
        return [row["qualified_name"] for row in rows]

    @staticmethod
    def reasons(rows: list[dict], name: str) -> list[str]:
        return next(row["reasons"] for row in rows if row["qualified_name"] == name)

    def test_boolean_returns_become_candidates(self):
        report = self.scan("""
            i32 IsZero(i32 value) {
                if (value == 0)
                    return 1;
                return 0;
            }
            i32 Count(i32 value) { return value + 1; }
            i32 Wrapped(i32 value) { return IsZero(value); }
            i32 Mixed(i32 value) {
                if (value)
                    return Count(value);
                return 0;
            }
            i32 OnlyZero(void) { return 0; }
            enum ViewResult { VIEW_CLOSED, VIEW_DISMISSED };
            i32 View(i32 value) {
                if (value)
                    return VIEW_DISMISSED;
                return VIEW_CLOSED;
            }
            i32 Stored(i32 value) {
                i32 result;
                result = 0;
                if (value > 3)
                    result = 1;
                return result;
            }
        """)
        self.assertEqual(self.names(report["result_candidates"]),
                         ["IsZero", "Stored", "Wrapped"])
        is_zero = report["result_candidates"][0]
        self.assertEqual(is_zero["target_type"], "b32")
        self.assertEqual([row["write"]["replacement"] for row in is_zero["returns"]],
                         ["true", "false"])
        self.assertEqual(len(is_zero["result_type_spans"]), 1)
        rejected = report["rejected_results"]
        self.assertIn("non-boolean-return", self.reasons(rejected, "Count"))
        self.assertIn("unproven-dependency", self.reasons(rejected, "Mixed"))
        self.assertIn("insufficient-observed-domain", self.reasons(rejected, "OnlyZero"))
        self.assertEqual(self.reasons(rejected, "View"), ["enumerator-return"])
        self.assertTrue(bool_fields.check_failures(report))

    def test_numeric_caller_use_keeps_integer_result(self):
        report = self.scan("""
            i32 Above(i32 value) { return value > 2; }
            i32 Tested(i32 value) { return value < 2; }
            #define BONUS(value) (3 + (Tested(value) != 0))
            i32 Total(void) {
                i32 total = 0;
                total += Above(3);
                if (Tested(1) != 0 && !Tested(2))
                    total++;
                total += BONUS(4);
                return total;
            }
        """)
        self.assertEqual(self.names(report["result_candidates"]), ["Tested"])
        self.assertEqual(self.reasons(report["rejected_results"], "Above"),
                         ["numeric-caller-use"])
        tested = report["result_candidates"][0]
        self.assertEqual(tested["result_uses"], {"comparison": 2, "condition": 1})

    def test_recursive_results_prove_together(self):
        report = self.scan("""
            i32 Odd(i32 value);
            i32 Even(i32 value) {
                if (value == 0)
                    return 1;
                return Odd(value - 1);
            }
            i32 Odd(i32 value) {
                if (value == 0)
                    return 0;
                return Even(value - 1);
            }
        """)
        self.assertEqual(self.names(report["result_candidates"]), ["Even", "Odd"])
        odd = next(row for row in report["result_candidates"]
                   if row["qualified_name"] == "Odd")
        self.assertEqual(len(odd["result_type_spans"]), 2)

    def test_virtual_and_undefined_results_stay(self):
        report = self.scan("""
            struct Widget {
                virtual i32 Visible(void) { return 1 - 0; }
                i32 Enabled(void) { return m_enabled != 0; }
                i32 m_enabled;
            };
            i32 External(void);
        """)
        self.assertEqual(self.names(report["result_candidates"]), ["Widget::Enabled"])
        self.assertIn("virtual", self.reasons(report["rejected_results"], "Widget::Visible"))
        self.assertIn("no-definition", self.reasons(report["rejected_results"], "External"))

    def test_boolean_results_report_literals_and_unproven_returns(self):
        report = self.scan("""
            b32 Ready(i32 value) {
                if (value)
                    return 1;
                return false;
            }
            b32 Masked(i32 value) { return value & 4; }
        """)
        self.assertEqual([row["write"]["replacement"]
                          for row in report["boolean_numeric_returns"]], ["true"])
        self.assertEqual(self.names(report["boolean_unproven_returns"]), ["Masked"])
        self.assertTrue(bool_fields.check_failures(report))

    def test_reviewed_unproven_return_is_accepted(self):
        report = self.scan("""
            b32 Masked(i32 value) { return value & 4; }
        """, exceptions="return\tsrc/unit0.cpp\tMasked\treturn\tvalue & 4\tretail mask\n")
        self.assertEqual(report["boolean_unproven_returns"], [])
        self.assertEqual(len(report["accepted_boolean_unproven_returns"]), 1)
        self.assertEqual(report["unused_retail_exceptions"], [])
        self.assertFalse(bool_fields.check_failures(report))

    def test_parameters_need_call_site_proofs(self):
        report = self.scan("""
            void Show(i32 visible) {}
            void Size(i32 width) {}
            void Chained(i32 flag) { Show(flag); }
            void Defaulted(i32 redraw = 0) {}
            void Callback(i32 flag) {}
            void Written(i32 flag) { flag = 7; }
            i32 gTable[2];
            void Indexed(i32 view) { gTable[view] = 3; }
            void (*gHandler)(i32) = Callback;
            void Use(i32 value) {
                Show(1);
                Show(value == 2);
                Size(1);
                Size(640);
                Chained(0);
                Chained(value != 0);
                Defaulted();
                Defaulted(1);
                Callback(0);
                Callback(1);
                Written(0);
                Written(1);
                Indexed(0);
                Indexed(1);
            }
        """, """
            void Show(i32 visible);
            void Later(void) { Show(0); }
        """)
        self.assertEqual(self.names(report["parameter_candidates"]),
                         ["Chained::flag", "Defaulted::redraw", "Show::visible"])
        show = next(row for row in report["parameter_candidates"]
                    if row["qualified_name"] == "Show::visible")
        self.assertEqual(len(show["type_spans"]), 2)
        rejected = report["rejected_parameters"]
        self.assertIn("non-boolean-argument", self.reasons(rejected, "Size::width"))
        self.assertIn("address-taken", self.reasons(rejected, "Callback::flag"))
        self.assertIn("non-boolean-body-write", self.reasons(rejected, "Written::flag"))
        self.assertEqual(self.reasons(rejected, "Indexed::view"), ["numeric-body-use"])
        self.assertTrue(bool_fields.check_failures(report))

    def test_boolean_default_literal_is_reported(self):
        report = self.scan("""
            void Redraw(b32 now = 0) {}
            void Use(void) { Redraw(); Redraw(true); }
        """)
        self.assertEqual([row["write"]["replacement"]
                          for row in report["boolean_numeric_default_arguments"]], ["false"])
        self.assertTrue(bool_fields.check_failures(report))

    def test_boolean_comparisons_with_literals_are_reported(self):
        report = self.scan("""
            b32 Ready(i32 value) { return value != 0; }
            bool Real(void) { return true; }
            void Use(b32 flag, i32 count) {
                if (flag == 0 || Ready(count) != 1 || Real() == 0 || count == 1)
                    flag = false;
            }
        """)
        self.assertEqual([row["write"]["replacement"]
                          for row in report["boolean_numeric_comparisons"]],
                         ["false", "true", "false"])
        self.assertTrue(bool_fields.check_failures(report))

    def test_unsigned_one_bit_fields_are_boolean(self):
        report = self.scan("""
            struct Cell { unsigned char ground : 1; unsigned char kind : 2; signed char odd : 1; };
            void Use(Cell* cell) {
                b32 ground = cell->ground;
                b32 kind = cell->kind;
                b32 odd = cell->odd;
            }
        """)
        self.assertEqual([row["write"]["expression"]
                          for row in report["boolean_unproven_writes"]],
                         ["cell -> kind", "cell -> odd"])

    def test_clean_boolean_contracts_pass_check(self):
        report = self.scan("""
            b32 IsZero(i32 value) { return value == 0; }
            void Show(b32 visible) {}
            void Redraw(b32 now = false) {}
            void Use(i32 value) {
                Redraw();
                b32 shown = false;
                if (IsZero(value))
                    shown = true;
                Show(shown);
                Show(false);
            }
        """)
        self.assertEqual(report["result_candidates"], [])
        self.assertEqual(report["parameter_candidates"], [])
        self.assertFalse(bool_fields.check_failures(report))


if __name__ == "__main__":
    unittest.main()
