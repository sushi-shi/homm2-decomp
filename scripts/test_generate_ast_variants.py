import tempfile
import unittest
from pathlib import Path

import clang.cindex as ci

from generate_ast_variants import (
    AstEdit,
    AstMutation,
    balance_mutations,
    candidate_payloads,
    clang_args,
    classify_parse_errors,
    configure_libclang,
    declaration_edits,
    expression_edits,
    helper_parameters,
    helper_return_spelling,
    inline_expression_edits,
    inline_member_access_edits,
    inline_nested_expression_edits,
    inline_read_advance_edits,
    identifier_rename_edits,
    mutation_name,
    statement_order_edits,
    utf8_byte_offset,
)


class AstVariantGenerationTests(unittest.TestCase):
    def test_clang_args_include_project_and_vendor_headers_without_database_entry(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "src/UNIT.cpp"
            (root / "include").mkdir()
            (root / "vendor/sdk").mkdir(parents=True)
            args = clang_args(root, source)
        self.assertIn(str(root / "include"), args)
        self.assertIn(str(root / "vendor/sdk"), args)

    def test_disjoint_mutations_are_combined(self):
        blob = b"abcdefghij"
        mutations = [
            AstMutation("first", "a", (AstEdit(0, 1, b"A"),)),
            AstMutation("second", "c", (AstEdit(2, 3, b"C"),)),
        ]
        candidates, truncated = candidate_payloads(blob, mutations, 2, 20)
        self.assertFalse(truncated)
        self.assertEqual(len(candidates), 3)
        self.assertEqual(len(candidates[-1]["edits"]), 2)

    def test_helpers_at_the_same_insertion_point_are_merged(self):
        blob = b"abcdefghij"
        mutations = [
            AstMutation("inline", "one", (
                AstEdit(0, 0, b"helper1 "), AstEdit(2, 3, b"C"),
            )),
            AstMutation("inline", "two", (
                AstEdit(0, 0, b"helper2 "), AstEdit(5, 6, b"F"),
            )),
        ]
        candidates, truncated = candidate_payloads(blob, mutations, 2, 20)
        self.assertFalse(truncated)
        self.assertEqual(len(candidates), 3)
        combined = candidates[-1]
        self.assertEqual(len(combined["edits"]), 3)
        self.assertEqual(combined["edits"][0]["replace"], "helper1 helper2 ")

    def test_distinct_edits_with_the_same_human_label_have_unique_names(self):
        blob = b"abcdefghij"
        mutations = [
            AstMutation("inline", "same", (AstEdit(2, 3, b"C"),)),
            AstMutation("inline", "same", (AstEdit(2, 3, b"X"),)),
        ]
        candidates, _truncated = candidate_payloads(blob, mutations, 1, 20)
        self.assertEqual(len({candidate["name"] for candidate in candidates}), 2)

    def test_required_mutation_is_present_in_every_combination(self):
        blob = b"abcdefghij"
        mutations = [
            AstMutation("family", "a", (AstEdit(0, 1, b"A"),)),
            AstMutation("family", "b", (AstEdit(2, 3, b"C"),)),
            AstMutation("family", "c", (AstEdit(4, 5, b"E"),)),
        ]
        required = mutation_name(mutations[1])
        candidates, truncated = candidate_payloads(
            blob, mutations, 2, 20, min_depth=2, required_names={required}
        )
        self.assertFalse(truncated)
        self.assertEqual(len(candidates), 2)
        self.assertTrue(all(required in candidate["name"] for candidate in candidates))

    def test_character_offsets_are_converted_for_utf8_manifests(self):
        text = "// en dash –\nVA()\n"
        character_offset = text.index("VA")
        self.assertEqual(utf8_byte_offset(text, character_offset), len(text[:character_offset].encode()))
        self.assertGreater(utf8_byte_offset(text, character_offset), character_offset)

    def test_mutation_families_are_round_robin_balanced(self):
        mutations = [
            AstMutation("ast", "a", (AstEdit(0, 1, b"A"),)),
            AstMutation("ast", "b", (AstEdit(2, 3, b"B"),)),
            AstMutation("ast", "c", (AstEdit(4, 5, b"C"),)),
            AstMutation("state", "a", (AstEdit(6, 6, b"typedef int X;"),)),
        ]
        balanced = balance_mutations(mutations)
        self.assertEqual([mutation.family for mutation in balanced[:2]], ["ast", "state"])


class AstVariantSemanticTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        configure_libclang()
        cls.source = Path(__file__).with_name("testdata") / "ast_variant_semantics.cpp"
        cls.blob = cls.source.read_bytes()
        cls.tu = ci.Index.create().parse(
            str(cls.source), args=["-x", "c++", "-std=c++14", "-fms-compatibility"]
        )
        errors = [str(item) for item in cls.tu.diagnostics if item.severity >= 3]
        if errors:
            raise AssertionError("fixture parse failed:\n" + "\n".join(errors))
        cls.functions = {
            cursor.spelling: cursor
            for cursor in cls.tu.cursor.walk_preorder()
            if cursor.kind in (ci.CursorKind.FUNCTION_DECL, ci.CursorKind.CXX_METHOD)
            and cursor.is_definition()
        }

    @classmethod
    def _text(cls, cursor):
        return cls.blob[cursor.extent.start.offset:cursor.extent.end.offset].decode()

    @classmethod
    def _node(cls, function, kind, text):
        matches = [
            node for node in function.walk_preorder()
            if node.kind == kind and cls._text(node) == text
        ]
        if len(matches) != 1:
            raise AssertionError("expected one %s node, found %d" % (text, len(matches)))
        return matches[0]

    @classmethod
    def _offset(cls, line, expression):
        line_start = cls.blob.index(line.encode())
        return line_start + line.index(expression)

    @classmethod
    def _render_mutation(cls, mutation):
        rendered = cls.blob
        for edit in sorted(mutation.edits, key=lambda item: (item.start, item.end), reverse=True):
            rendered = rendered[:edit.start] + edit.replacement + rendered[edit.end:]
        return rendered

    def test_record_parameters_are_never_copied_by_helpers(self):
        function = self.functions["SafeHelpers"]
        by_value = self._node(
            function, ci.CursorKind.BINARY_OPERATOR, "record.value + scalar"
        )
        by_reference = self._node(
            function, ci.CursorKind.BINARY_OPERATOR, "recordRef.value + scalar"
        )
        self.assertEqual(
            helper_parameters(by_value, function),
            [("const AstRecord &", "record"), ("int", "scalar")],
        )
        record_parameter = next(
            child for child in function.get_children()
            if child.kind == ci.CursorKind.PARM_DECL and child.spelling == "record"
        )
        self.assertIsNone(helper_return_spelling(record_parameter.type))
        self.assertEqual(
            helper_parameters(by_reference, function),
            [("AstRecord &", "recordRef"), ("int", "scalar")],
        )

        helpers = inline_expression_edits(function, self.blob, 0, 1)
        helpers += inline_member_access_edits(function, self.blob, 0, 1)
        helpers += inline_nested_expression_edits(function, self.blob, 0, 1)
        helper_text = b"".join(
            edit.replacement for mutation in helpers for edit in mutation.edits
            if edit.start == edit.end
        ).decode()
        self.assertIn("const AstRecord & record", helper_text)
        self.assertIn("AstRecord & recordRef", helper_text)
        self.assertIn("AstChoice choice", helper_text)
        self.assertIn("static inline int * H2AstMember", helper_text)
        self.assertNotIn("AstRecord record", helper_text)

    def test_member_helpers_skip_lvalue_and_volatile_contexts(self):
        function = self.functions["RejectedMemberContexts"]
        mutations = inline_member_access_edits(function, self.blob, 0, 1)
        edited_offsets = {
            edit.start for mutation in mutations for edit in mutation.edits if edit.start != edit.end
        }
        rejected = {
            self._offset("    record.value = scalar;", "record.value"),
            self._offset("    int *address = &record.value;", "record.value"),
            self._offset("    ConsumeReference(record.value);", "record.value"),
            self._offset("    const int &alias = record.value;", "record.value"),
            self._offset(
                "    int observed = volatileRecord.value + scalar;", "volatileRecord.value"
            ),
        }
        self.assertTrue(edited_offsets.isdisjoint(rejected))

    def test_volatile_expression_is_absent_from_all_expression_families(self):
        function = self.functions["RejectedMemberContexts"]
        line = "    int observed = volatileRecord.value + scalar;"
        start = self._offset(line, "volatileRecord.value + scalar")
        end = start + len("volatileRecord.value + scalar")
        mutations = expression_edits(function, self.blob)
        mutations += inline_expression_edits(function, self.blob, 0, 1)
        mutations += inline_nested_expression_edits(function, self.blob, 0, 1)
        self.assertFalse(any(
            edit.start < end and start < edit.end
            for mutation in mutations for edit in mutation.edits if edit.start != edit.end
        ))

    def test_pure_integer_division_and_subtraction_can_be_inlined(self):
        function = self.functions["SafeIntegerOperators"]
        mutations = inline_expression_edits(function, self.blob, 0, 1)
        helpers = b"".join(
            edit.replacement for mutation in mutations for edit in mutation.edits
            if edit.start == edit.end
        )
        self.assertIn(b"return value / divisor;", helpers)
        self.assertIn(b"return value - divisor;", helpers)

    def test_identifier_rename_tracks_declaration_identity_and_all_references(self):
        function = self.functions["SafeIntegerOperators"]
        mutations = identifier_rename_edits(function, self.blob, 2)
        mutation = next(item for item in mutations if "divisor-to-divisorValue" in item.label)
        candidate = self._render_mutation(mutation).decode()
        self.assertIn("int divisorValue", candidate)
        self.assertIn("value / divisorValue", candidate)
        self.assertIn("value - divisorValue", candidate)
        tu = ci.Index.create().parse(
            str(self.source), args=["-x", "c++", "-std=c++14"],
            unsaved_files=[(str(self.source), candidate)],
        )
        errors = [item for item in tu.diagnostics if item.severity >= ci.Diagnostic.Error]
        self.assertEqual(errors, [])

    def test_identifier_rename_can_be_limited_by_declaration_spelling(self):
        function = self.functions["SafeIntegerOperators"]
        mutations = identifier_rename_edits(function, self.blob, 2, {"divisor"})
        self.assertEqual(len(mutations), 2)
        self.assertTrue(all("divisor-to-divisor" in item.label for item in mutations))

    def test_read_advance_requires_independent_value_local(self):
        safe = inline_read_advance_edits(
            self.functions["SafeReadAdvance"], self.blob, 0, 1
        )
        rejected = inline_read_advance_edits(
            self.functions["RejectedReadAdvance"], self.blob, 0, 1
        )
        self.assertEqual(len(safe), 1)
        self.assertEqual(rejected, [])

    def test_indirect_aliasing_stores_are_not_reordered(self):
        function = self.functions["RejectedAliasingStores"]
        self.assertEqual(statement_order_edits(function, self.blob), [])

    def test_direct_independent_local_stores_remain_available(self):
        function = self.functions["SafeStatementOrder"]
        self.assertEqual(len(statement_order_edits(function, self.blob)), 1)

    def test_volatile_member_read_is_not_reordered(self):
        function = self.functions["RejectedVolatileOrder"]
        self.assertEqual(statement_order_edits(function, self.blob), [])

    def test_line_sensitive_declarations_and_statements_are_not_moved(self):
        function = self.functions["RejectedLineMacros"]
        self.assertEqual(declaration_edits(function, self.blob), [])
        self.assertEqual(statement_order_edits(function, self.blob), [])

    def test_safe_declaration_split_and_merge_remain_available(self):
        function = self.functions["SafeDeclarations"]
        families = {mutation.family for mutation in declaration_edits(function, self.blob)}
        self.assertEqual(families, {"declaration_split", "declaration_merge"})

    def test_one_safe_edit_from_every_family_remains_parseable(self):
        helpers = self.functions["SafeHelpers"]
        statements = self.functions["SafeStatementOrder"]
        declarations = self.functions["SafeDeclarations"]
        read_advance = self.functions["SafeReadAdvance"]
        groups = [
            expression_edits(statements, self.blob),
            statement_order_edits(statements, self.blob),
            declaration_edits(declarations, self.blob),
            inline_expression_edits(helpers, self.blob, helpers.extent.start.offset, 1),
            inline_member_access_edits(helpers, self.blob, helpers.extent.start.offset, 1),
            inline_nested_expression_edits(helpers, self.blob, helpers.extent.start.offset, 1),
            inline_read_advance_edits(
                read_advance, self.blob, read_advance.extent.start.offset, 1
            ),
        ]
        self.assertTrue(all(groups))
        mutations = [group[0] for group in groups]
        mutations.extend(
            mutation for mutation in groups[2]
            if mutation.family != mutations[2].family
        )
        for mutation in mutations:
            candidate = self._render_mutation(mutation).decode()
            tu = ci.Index.create().parse(
                str(self.source), args=["-x", "c++", "-std=c++14"],
                unsaved_files=[(str(self.source), candidate)],
            )
            errors = [item for item in tu.diagnostics if item.severity >= ci.Diagnostic.Error]
            self.assertEqual(errors, [], mutation.family)

    def test_floating_addition_is_not_treated_as_commutative(self):
        function = self.functions["RejectedFloatingCommutative"]
        self.assertEqual(expression_edits(function, self.blob), [])
        self.assertEqual(inline_expression_edits(function, self.blob, 0, 1), [])

    def test_implicit_this_is_not_extracted_to_namespace_helper(self):
        function = self.functions["Read"]
        self.assertEqual(inline_expression_edits(function, self.blob, 0, 1), [])
        self.assertEqual(inline_member_access_edits(function, self.blob, 0, 1), [])

    def test_only_nonfatal_errors_after_target_are_ignorable(self):
        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory) / "trailing_error.cpp"
            source.write_text(
                "int Target() { return 1; }\n"
                "int Later() { int *pointer = 1; return *pointer; }\n"
            )
            tu = ci.Index.create().parse(str(source), args=["-x", "c++", "-std=c++14"])
            functions = {
                cursor.spelling: cursor for cursor in tu.cursor.get_children()
                if cursor.kind == ci.CursorKind.FUNCTION_DECL
            }
            blocking, trailing, allowed, unmatched = classify_parse_errors(
                tu, source, functions["Target"]
            )
            later_blocking, _later_trailing, _allowed, _unmatched = classify_parse_errors(
                tu, source, functions["Later"]
            )
        self.assertEqual(blocking, [])
        self.assertEqual(len(trailing), 1)
        self.assertEqual(allowed, [])
        self.assertEqual(unmatched, set())
        self.assertEqual(len(later_blocking), 1)

    def test_reviewed_external_error_can_be_allowed_exactly(self):
        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory) / "earlier_error.cpp"
            source.write_text(
                "int Earlier() { int *pointer = 1; return *pointer; }\n"
                "int Target() { return 1; }\n"
            )
            tu = ci.Index.create().parse(str(source), args=["-x", "c++", "-std=c++14"])
            target = next(
                cursor for cursor in tu.cursor.get_children()
                if cursor.kind == ci.CursorKind.FUNCTION_DECL and cursor.spelling == "Target"
            )
            blocking, trailing, allowed, unmatched = classify_parse_errors(
                tu, source, target, ("cannot initialize a variable of type 'int *'",)
            )
            earlier = next(
                cursor for cursor in tu.cursor.get_children()
                if cursor.kind == ci.CursorKind.FUNCTION_DECL and cursor.spelling == "Earlier"
            )
            target_blocking, _trailing, target_allowed, target_unmatched = \
                classify_parse_errors(
                    tu, source, earlier, ("cannot initialize a variable of type 'int *'",)
                )
        self.assertEqual(blocking, [])
        self.assertEqual(trailing, [])
        self.assertEqual(len(allowed), 1)
        self.assertEqual(unmatched, set())
        self.assertEqual(len(target_blocking), 1)
        self.assertEqual(target_allowed, [])
        self.assertEqual(
            target_unmatched, {"cannot initialize a variable of type 'int *'"}
        )


if __name__ == "__main__":
    unittest.main()
