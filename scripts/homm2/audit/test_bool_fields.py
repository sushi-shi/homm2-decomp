from __future__ import annotations

import json
import importlib
import tempfile
import unittest
from contextlib import redirect_stdout
from io import StringIO
from pathlib import Path
from unittest import mock

import clang.cindex as ci

from homm2.audit.bool_fields import (
    _boolean_domain,
    _command_arguments,
    _entries,
    _integer_literal,
    _merge,
    _parse_driver_includes,
    _type_spans_requiring_split,
    analyze_translation_unit,
    main,
)
from homm2.build.annotated_data import configure_libclang


SOURCE = r"""
typedef int i32;
typedef i32 b32;

struct Good {
    i32 flag;
    Good() : flag(0) {}
    void Set() { flag = 1; }
    i32 Get() const { return flag; }
};

struct BoolExpression {
    i32 flag;
    void Set(i32 value) { flag = value == 4; }
};

struct UnknownValue {
    i32 flag;
    UnknownValue() : flag(0) {}
    void Set(i32 value) { flag = value; }
};

struct Addressed {
    i32 flag;
    Addressed() : flag(0) {}
    static void Fill(i32 *);
    void Set() { Fill(&flag); }
};

struct Updated {
    i32 flag;
    Updated() : flag(0) {}
    void Set() { ++flag; }
};

struct Existing {
    b32 flag;
    Existing() : flag(false) {}
};

struct NeverWritten {
    i32 value;
};

struct OneSided {
    i32 value;
    OneSided() : value(0) {}
};
"""


def parse(repo: Path, text: str = SOURCE):
    source = repo / "src" / "SOURCE" / "TEST.cpp"
    source.parent.mkdir(parents=True)
    source.write_text(text)
    configure_libclang()
    translation = ci.Index.create().parse(
        str(source), args=["-x", "c++", "-std=c++20"])
    diagnostics = [str(item) for item in translation.diagnostics
                   if item.severity >= ci.Diagnostic.Error]
    if diagnostics:
        raise AssertionError(diagnostics)
    return source, translation


class BoolFieldAnalyzerTests(unittest.TestCase):
    def test_integer_literal_parser_accepts_cpp_bases_and_suffixes(self):
        self.assertEqual(_integer_literal("0"), 0)
        self.assertEqual(_integer_literal("01U"), 1)
        self.assertEqual(_integer_literal("0x1L"), 1)
        self.assertEqual(_integer_literal("0b1"), 1)
        self.assertEqual(_integer_literal("2"), 2)
        self.assertIsNone(_integer_literal("VALUE"))

    def test_analysis_requires_only_proven_boolean_writes(self):
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            source, translation = parse(repo)
            fields = analyze_translation_unit(translation, source, repo)

        by_name = {f"{item.record}::{item.name}": item for item in fields}
        self.assertTrue(by_name["Good::flag"].eligible)
        self.assertTrue(by_name["BoolExpression::flag"].eligible)
        self.assertFalse(by_name["UnknownValue::flag"].eligible)
        self.assertFalse(by_name["Addressed::flag"].eligible)
        self.assertFalse(by_name["Updated::flag"].eligible)
        self.assertFalse(by_name["NeverWritten::value"].eligible)
        self.assertFalse(by_name["OneSided::value"].eligible)
        self.assertEqual(by_name["Existing::flag"].declared_type, "b32")
        self.assertFalse(by_name["Existing::flag"].eligible)

        good = by_name["Good::flag"]
        self.assertEqual({write.domain for write in good.writes}, {(0,), (1,)})
        self.assertEqual({write.replacement for write in good.writes}, {"false", "true"})
        self.assertEqual(good.unknown_writes, set())
        self.assertEqual(len(good.read_locations), 1)

        self.assertEqual(
            {write.kind for write in by_name["Addressed::flag"].unknown_writes},
            {"address-escape"},
        )
        self.assertEqual(
            {write.kind for write in by_name["Updated::flag"].unknown_writes},
            {"unary-update"},
        )

    def test_mutable_reference_argument_is_rejected(self):
        text = r"""
typedef int i32;
struct Value {
    i32 flag;
    Value() : flag(0) {}
    static void Mutate(i32 &);
    void Set() { Mutate(flag); }
};
"""
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            source, translation = parse(repo, text)
            facts = next(
                item for item in analyze_translation_unit(translation, source, repo)
                if item.storage_kind == "field"
            )
        self.assertFalse(facts.eligible)
        self.assertIn("mutable-reference-argument",
                      {write.kind for write in facts.unknown_writes})

    def test_array_index_on_assignment_lhs_is_a_read(self):
        text = r"""
typedef int i32;
typedef i32 b32;
i32 values[2];
b32 selector = false;
void Set() { values[selector] = 4; }
"""
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            source, translation = parse(repo, text)
            selector = next(
                item for item in analyze_translation_unit(translation, source, repo)
                if item.name == "selector"
            )
        self.assertEqual(selector.unknown_writes, set())
        self.assertEqual(len(selector.read_locations), 1)

    def test_array_elements_share_one_storage_domain(self):
        text = r"""
typedef signed char i8;
typedef i8 b8;
i8 flags[2] = {0, 1};
b8 positions[2];
void Set() {
    flags[0] = 1;
    positions[0] = -1;
}
"""
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            source, translation = parse(repo, text)
            by_name = {
                item.name: item
                for item in analyze_translation_unit(translation, source, repo)
            }
        self.assertTrue(by_name["flags"].eligible)
        self.assertIn("assignment",
                      {write.kind for write in by_name["positions"].unknown_writes})
        self.assertNotIn("variable-initializer",
                         {write.kind for write in by_name["positions"].unknown_writes})

    def test_uninitialized_local_declaration_is_not_a_value_write(self):
        text = r"""
typedef int i32;
void Set(i32 input) {
    i32 flag;
    if (input)
        flag = 0;
    else
        flag = 1;
    (void)flag;
}
"""
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            source, translation = parse(repo, text)
            flag = next(
                item for item in analyze_translation_unit(translation, source, repo)
                if item.name == "flag"
            )
        self.assertTrue(flag.eligible)
        self.assertEqual(
            {write.kind for write in flag.unknown_writes},
            {"uninitialized-local"},
        )

    def test_enum_storage_macro_is_not_a_boolean_candidate(self):
        text = r"""
typedef signed char i8;
#define H2_ENUM_STORAGE(name, storage) storage
enum Formation { SPREAD = 0, GROUPED = 1 };
struct Town {
    H2_ENUM_STORAGE(Formation, i8) formation;
    void Set() { formation = GROUPED; }
};
"""
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            source, translation = parse(repo, text)
            fields = analyze_translation_unit(translation, source, repo)
        self.assertNotIn("formation", {item.name for item in fields})

    def test_boolean_domain_understands_conditionals_and_bool_results(self):
        text = r"""
typedef int i32;
struct Value {
    i32 a;
    i32 b;
    void Set(i32 x) {
        a = x ? 0 : 1;
        b = x != 0;
    }
};
"""
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            source, translation = parse(repo, text)
            fields = [
                item for item in analyze_translation_unit(translation, source, repo)
                if item.storage_kind == "field"
            ]
        self.assertEqual(len(fields), 2)
        self.assertTrue(all(item.eligible for item in fields))
        domains = {item.name: {write.domain for write in item.writes} for item in fields}
        self.assertEqual(domains, {"a": {(0, 1)}, "b": {(0, 1)}})

    def test_boolean_alias_reference_is_proven_boolean_flow(self):
        text = r"""
typedef int i32;
typedef i32 b32;
b32 source = false;
struct Value {
    i32 flag;
    void Set() { flag = source; }
};
"""
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            source, translation = parse(repo, text)
            flag = next(
                item for item in analyze_translation_unit(translation, source, repo)
                if item.name == "flag"
            )
        self.assertTrue(flag.eligible)
        self.assertEqual(flag.observed_domain, (0, 1))

    def test_one_minus_boolean_alias_is_proven_boolean_flow(self):
        text = r"""
typedef int i32;
typedef i32 b32;
b32 source = false;
struct Value {
    i32 flag;
    void Set() { flag = 1 - source; }
};
"""
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            source, translation = parse(repo, text)
            flag = next(
                item for item in analyze_translation_unit(translation, source, repo)
                if item.name == "flag"
            )
        self.assertTrue(flag.eligible)
        self.assertEqual(flag.observed_domain, (0, 1))

    def test_aggregate_initializers_are_mapped_by_field_position(self):
        text = r"""
typedef int i32;
struct Value {
    i32 flag;
    i32 count;
};
Value values[] = {{0, 9}, {1, 10}};
"""
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            source, translation = parse(repo, text)
            fields = analyze_translation_unit(translation, source, repo)
        by_name = {item.name: item for item in fields}
        self.assertTrue(by_name["flag"].eligible)
        self.assertFalse(by_name["count"].eligible)
        self.assertEqual(
            {write.kind for write in by_name["flag"].writes},
            {"aggregate-initializer"},
        )

    def test_globals_and_locals_use_width_preserving_boolean_targets(self):
        text = r"""
typedef int i32;
typedef signed char i8;
typedef i32 b32;
typedef i8 b8;
typedef char bchar;

i32 globalFlag;
char charFlag = 0;
i8 byteFlag = 0;
bchar existingFlag = false;

void SetGlobals() {
    globalFlag = 1;
    charFlag = 1;
    byteFlag = 1;
    existingFlag = true;
}

void LocalFlags(i32 incoming) {
    i32 localFlag = 0;
    char localChar = 0;
    if (incoming) {
        localFlag = 1;
        localChar = 1;
    }
}
"""
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            source, translation = parse(repo, text)
            facts = analyze_translation_unit(translation, source, repo)

        by_name = {item.name: item for item in facts if item.name != "incoming"}
        self.assertEqual(by_name["globalFlag"].target_type, "b32")
        self.assertEqual(by_name["charFlag"].target_type, "bchar")
        self.assertEqual(by_name["byteFlag"].target_type, "b8")
        self.assertEqual(by_name["localFlag"].storage_kind, "variable")
        self.assertEqual(by_name["localChar"].target_type, "bchar")
        self.assertTrue(all(by_name[name].eligible for name in (
            "globalFlag", "charFlag", "byteFlag", "localFlag", "localChar")))
        self.assertEqual(
            {write.replacement for write in by_name["localChar"].writes},
            {"false", "true"},
        )
        self.assertEqual(by_name["existingFlag"].declared_type, "bchar")
        incoming = next(item for item in facts if item.name == "incoming")
        self.assertEqual(
            {write.kind for write in incoming.unknown_writes},
            {"incoming-parameter"},
        )

    def test_mixed_declaration_requires_a_split_before_type_replacement(self):
        text = r"""
typedef int i32;
void Set(i32 input) {
    i32 flag = 0, count = 4;
    if (input)
        flag = 1;
    (void)flag;
    (void)count;
}
"""
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            source, translation = parse(repo, text)
            facts = analyze_translation_unit(translation, source, repo)

        flag = next(item for item in facts if item.name == "flag")
        count = next(item for item in facts if item.name == "count")
        self.assertTrue(flag.eligible)
        self.assertFalse(count.eligible)
        self.assertIn(next(iter(flag.type_spans)),
                      _type_spans_requiring_split(facts))

    def test_compilation_database_rejects_wrong_worktree(self):
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            (repo / "build/clangd").mkdir(parents=True)
            (repo / "build/clangd/compile_commands.json").write_text(json.dumps([{
                "directory": "/somewhere/else",
                "file": "src/TEST.cpp",
                "arguments": ["clang-cl"],
            }]))
            with self.assertRaisesRegex(RuntimeError, "outside this worktree"):
                _entries(repo)

    def test_compilation_database_accepts_arguments_or_shell_command(self):
        self.assertEqual(
            _command_arguments({"arguments": ["c++", "-DVALUE=one two"]}),
            ["c++", "-DVALUE=one two"],
        )
        self.assertEqual(
            _command_arguments({"command": "c++ '-DVALUE=one two' file.cpp"}),
            ["c++", "-DVALUE=one two", "file.cpp"],
        )

    def test_compilation_database_ignores_non_game_targets(self):
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            (repo / "build/clangd").mkdir(parents=True)
            game_source = repo / "src/SOURCE/TEST.cpp"
            vendor_source = repo / "vendor/library.c"
            game_source.parent.mkdir(parents=True)
            vendor_source.parent.mkdir(parents=True)
            game_source.write_text("")
            vendor_source.write_text("")
            (repo / "build/clangd/compile_commands.json").write_text(json.dumps([
                {
                    "directory": str(repo),
                    "file": str(game_source),
                    "arguments": ["c++", "-c", str(game_source)],
                },
                {
                    "directory": str(repo),
                    "file": str(vendor_source),
                    "arguments": ["cc", "-c", str(vendor_source)],
                },
            ]))

            self.assertEqual(
                [Path(entry["file"]).resolve() for entry in _entries(repo)],
                [game_source.resolve()],
            )

    def test_portable_audit_uses_the_cmake_compilation_database(self):
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            (repo / "build").mkdir()
            game_source = repo / "src/SOURCE/TEST.cpp"
            game_source.parent.mkdir(parents=True)
            game_source.write_text("")
            (repo / "build/compile_commands.json").write_text(json.dumps([{
                "directory": str(repo),
                "file": str(game_source),
                "arguments": ["c++", "-c", str(game_source)],
            }]))

            self.assertEqual(
                [Path(entry["file"]).resolve()
                 for entry in _entries(repo, portable=True)],
                [game_source.resolve()],
            )

    def test_native_driver_include_paths_are_extracted(self):
        stderr = """ignored
#include <...> search starts here:
 /toolchain/include/c++
 /sdk/include (framework directory)
End of search list.
ignored
"""
        self.assertEqual(
            _parse_driver_includes(stderr),
            ("/toolchain/include/c++", "/sdk/include"),
        )

    def test_merge_deduplicates_header_evidence_seen_by_multiple_tus(self):
        declaration = {
            "file": "include/value.h", "line": 2, "column": 9,
            "start": 20, "end": 28,
        }
        type_span = {
            "file": "include/value.h", "line": 2, "column": 5,
            "start": 20, "end": 23,
        }
        write = {
            "file": "include/value.h", "line": 3, "column": 16,
            "start": 44, "end": 45, "kind": "assignment",
            "expression": "1", "domain": (1,), "replacement": "true",
        }
        common = {
            "usr": "field-usr", "record_usr": "record-usr", "record": "Value",
            "name": "flag", "declared_type": "i32",
            "declarations": [declaration], "type_spans": [type_span],
            "writes": [write], "unknown_writes": [], "read_locations": [],
        }
        fields = _merge([
            {**common, "translation_units": ["A"]},
            {**common, "translation_units": ["B"]},
        ])
        self.assertEqual(len(fields), 1)
        self.assertEqual(len(fields[0].writes), 1)
        self.assertEqual(fields[0].translation_units, {"A", "B"})

    def test_process_worker_has_a_canonical_import_identity(self):
        worker = importlib.import_module("homm2.audit.bool_fields")._parse_batch
        self.assertEqual(worker.__module__, "homm2.audit.bool_fields")

    def test_check_exit_status_is_always_an_integer(self):
        base = {
            "translation_units": 1,
            "i32_fields": 0,
            "b32_fields": 0,
            "eligible_fields": 0,
            "rejected_fields": 0,
            "candidates": [],
            "rejected": [],
            "b32_numeric_literal_writes": [],
            "b32_unproven_writes": [],
        }
        with mock.patch("homm2.audit.bool_fields.scan", return_value=base), \
                redirect_stdout(StringIO()):
            self.assertEqual(main(["--check"]), 0)
        dirty = {**base, "eligible_fields": 1, "candidates": [{
            "declarations": [{"file": "include/x.h", "line": 1}],
            "qualified_name": "X::flag", "writes": [], "read_count": 0,
        }]}
        with mock.patch("homm2.audit.bool_fields.scan", return_value=dirty), \
                redirect_stdout(StringIO()):
            self.assertEqual(main(["--check"]), 1)

        unproven = {**base, "b32_unproven_writes": [{
            "declarations": [{"file": "include/x.h", "line": 1}],
            "qualified_name": "X::flag",
            "write": {
                "file": "src/x.cpp", "line": 2, "kind": "assignment",
                "expression": "value", "domain": None,
            },
        }]}
        with mock.patch("homm2.audit.bool_fields.scan", return_value=unproven), \
                redirect_stdout(StringIO()):
            self.assertEqual(main(["--check"]), 1)

    def test_portable_mode_is_forwarded_to_scan(self):
        base = {
            "translation_units": 1,
            "i32_fields": 0,
            "b32_fields": 0,
            "eligible_fields": 0,
            "rejected_fields": 0,
            "candidates": [],
            "rejected": [],
            "b32_numeric_literal_writes": [],
            "b32_unproven_writes": [],
        }
        with mock.patch("homm2.audit.bool_fields.scan", return_value=base) as scan_mock, \
                redirect_stdout(StringIO()):
            self.assertEqual(main(["--portable"]), 0)
        scan_mock.assert_called_once_with(jobs=0, filters=[], portable=True)

    def test_check_rejects_partial_translation_unit_evidence(self):
        with mock.patch("homm2.audit.bool_fields.scan") as scan_mock, \
                mock.patch("sys.stderr", new_callable=StringIO) as stderr:
            self.assertEqual(main(["--check", "--tu", "REMOTE"]), 1)
        scan_mock.assert_not_called()
        self.assertIn("requires the full compilation database", stderr.getvalue())

    def test_boolean_parameters_are_inventoried_separately(self):
        text = r"""
typedef int i32;
typedef i32 b32;
void Consume(b32 enabled) { (void)enabled; }
"""
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            source, translation = parse(repo, text)
            facts = analyze_translation_unit(translation, source, repo)

        enabled = next(item for item in facts if item.name == "enabled")
        self.assertEqual(enabled.storage_kind, "parameter")
        self.assertEqual(
            {write.kind for write in enabled.unknown_writes},
            {"incoming-parameter"},
        )


if __name__ == "__main__":
    unittest.main()
