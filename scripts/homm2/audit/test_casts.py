import json
import tempfile
import unittest
from pathlib import Path

import clang.cindex as ci

from homm2.audit.casts import (
    HIGH_PRIORITY,
    _lexical_counts,
    _reviewed_exceptions,
    _text,
    analyze_translation_unit,
)
from homm2.build.annotated_data import configure_libclang


SOURCE = r"""
typedef int i32;
typedef short i16;

enum class First : i32 { A = 0, B = 1 };
enum class Second : i32 { A = 0, B = 1 };

struct Base { virtual ~Base() {} };
struct Derived : Base {};

i32 raw();

void Review() {
    i32 value = raw();
    First decoded = static_cast<First>(value);
    First literal = static_cast<First>(1);
    i32 indexed = static_cast<i32>(First::A);
    Second crossed = static_cast<Second>(First::B);
    i32 same = static_cast<i32>(value);
    i16 narrow = static_cast<i16>(value);
    Base *base = static_cast<Base *>(static_cast<Derived *>(nullptr));
    (void)decoded; (void)literal; (void)indexed; (void)crossed;
    (void)same; (void)narrow; (void)base;
}
"""


def parse(repo: Path, text: str = SOURCE):
    source = repo / "src/SOURCE/TEST.cpp"
    source.parent.mkdir(parents=True)
    source.write_text(text)
    configure_libclang()
    translation = ci.Index.create().parse(
        str(source), args=["-x", "c++", "-std=c++20"])
    errors = [str(item) for item in translation.diagnostics
              if item.severity >= ci.Diagnostic.Error]
    if errors:
        raise AssertionError(errors)
    return translation


class CastAuditTests(unittest.TestCase):
    def test_classifies_semantic_type_boundaries(self):
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            rows = analyze_translation_unit(parse(repo), repo)

        categories = [item.category for item in rows]
        self.assertIn("integer-to-enum", categories)
        self.assertIn("literal-to-enum", categories)
        self.assertIn("enum-to-integer", categories)
        self.assertIn("cross-enum", categories)
        self.assertIn("same-type", categories)
        self.assertIn("integer-narrowing", categories)
        self.assertEqual(categories.count("pointer-conversion"), 2)
        self.assertTrue({"cross-enum", "literal-to-enum", "same-type"}
                        <= HIGH_PRIORITY)
        self.assertNotIn("integer-narrowing", HIGH_PRIORITY)

        crossed = next(item for item in rows if item.category == "cross-enum")
        self.assertNotEqual(crossed.source_enum, crossed.destination_enum)
        self.assertEqual(crossed.function, "Review")
        self.assertGreater(crossed.end, crossed.start)

    def test_header_sites_are_source_identity_deduplicated_by_scan_contract(self):
        # Keep the identity contract explicit without paying for compilation
        # database/process setup in this focused unit test.
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            row = analyze_translation_unit(parse(
                repo,
                "typedef int i32; i32 f(i32 x) { return static_cast<i32>(x); }",
            ), repo)[0]
        key = (row.file, row.start, row.end, row.cast_kind)
        self.assertEqual(len({key: row, key: row}), 1)

    def test_macro_expansion_is_not_misattributed_to_each_call_site(self):
        text = """
#define TO_INT(value) static_cast<int>(value)
int f() { return TO_INT(1); }
int g() { return TO_INT(2); }
"""
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            rows = analyze_translation_unit(parse(repo, text), repo)
            counts = _lexical_counts(repo)
        self.assertEqual(rows, [])
        self.assertEqual(counts, {"static": 1})

    def test_implicit_case_conversion_does_not_borrow_a_later_cast(self):
        text = """
enum Command { A = 0 };
int f(Command command, int value) {
    switch (value) {
    case A:
        return static_cast<int>(command);
    }
    return 0;
}
"""
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            rows = analyze_translation_unit(parse(repo, text), repo)
        self.assertEqual(len(rows), 1)
        self.assertEqual(rows[0].category, "enum-to-integer")
        self.assertEqual(rows[0].line, 6)

    def test_explicit_conversion_reports_the_wrapped_source_type(self):
        text = """
struct Wrapper {
    explicit operator char() const { return 0; }
};
char f(Wrapper value) { return static_cast<char>(value); }
"""
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            row = analyze_translation_unit(parse(repo, text), repo)[0]
        self.assertEqual(row.source_type, "Wrapper")
        self.assertEqual(row.destination_type, "char")
        self.assertEqual(row.category, "explicit-scalar-conversion")

    def test_text_defaults_to_high_priority_and_json_is_serializable(self):
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            rows = analyze_translation_unit(parse(repo), repo)
        report = {
            "whole_program": True,
            "translation_units": 1,
            "casts": [row.__dict__ for row in rows],
            "counts": {name: sum(row.category == name for row in rows)
                       for name in sorted({row.category for row in rows})},
            "high_priority": sum(row.category in HIGH_PRIORITY for row in rows),
            "strict_diagnostics": [],
        }
        text = _text(report, include_all=False, categories=frozenset())
        self.assertIn("CROSS-ENUM", text)
        self.assertNotIn("ENUM-TO-INTEGER", text)
        json.dumps(report)

    def test_review_manifest_uses_exact_source_identity(self):
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            path = repo / "config/retail_cast_exceptions.tsv"
            path.parent.mkdir()
            path.write_text(
                "category\tfile\tfunction\tsource_type\tdestination_type\t"
                "expression\treason\n"
                "same-type\tsrc/SOURCE/TEST.cpp\tReview\ti32\ti32\t"
                "static_cast<i32>(value)\tRetail code-generation evidence.\n"
            )
            rows = _reviewed_exceptions(repo)
        self.assertEqual(len(rows), 1)
        self.assertIn(
            (
                "same-type", "src/SOURCE/TEST.cpp", "Review", "i32", "i32",
                "static_cast<i32>(value)",
            ),
            rows,
        )


if __name__ == "__main__":
    unittest.main()
