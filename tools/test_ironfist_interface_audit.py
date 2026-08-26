#!/usr/bin/env python3

import importlib.util
import pathlib
import unittest


MODULE_PATH = pathlib.Path(__file__).with_name("ironfist_interface_audit.py")
SPEC = importlib.util.spec_from_file_location("ironfist_interface_audit", MODULE_PATH)
audit = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(audit)


class IronfistInterfaceAuditTest(unittest.TestCase):
    def test_comment_stripping_preserves_literals(self):
        source = r'''
            const char* url = "https://example.invalid/a/*b*/";
            // lua_register(L, "CommentedOut", ignored);
            lua_register(L, "Visible", visible); /* "OnCommentedOut" */
        '''
        stripped = audit.strip_cpp_comments(source)
        self.assertIn('"https://example.invalid/a/*b*/"', stripped)
        self.assertEqual(
            audit.extract_lua_registration_map(source), {"Visible": "visible"}
        )
        self.assertEqual(audit.extract_callback_names([source]), set())

    def test_lua_registration_targets_and_handlers(self):
        source = r'''
            static int l_one(lua_State *L) { return 0; }
            static i32 l_two(lua_State* L) { return 0; }
            // static int l_hidden(lua_State *L) { return 0; }
            lua_register(L, "One", l_one);
            lua_register(L, "Two", l_two);
        '''
        self.assertEqual(
            audit.extract_lua_registration_map(source),
            {"One": "l_one", "Two": "l_two"},
        )
        self.assertEqual(audit.extract_lua_handlers(source), {"l_one", "l_two"})

    def test_lua_return_arities_resolve_constants(self):
        source = r'''
            enum Counts { NO_RESULTS = 0, ONE_RESULT = 1 };
            static int l_none(lua_State *L) { return NO_RESULTS; }
            static i32 l_one(lua_State* L) {
                if (L) return H2EnumIndex(ONE_RESULT);
                return 1;
            }
        '''
        arities, unresolved = audit.resolve_lua_return_arities(source, [source])
        self.assertEqual(arities, {"l_none": [0], "l_one": [1]})
        self.assertEqual(unresolved, {})

    def test_binary_string_surface_requires_nul_terminated_names(self):
        comparison = audit.compare_binary_strings(
            {"One", "Two", "Three"}, b"prefixOne\0TwoSuffix\0Three\0"
        )
        self.assertEqual(comparison["present_count"], 1)
        self.assertEqual(comparison["present"], ["Three"])
        self.assertEqual(comparison["missing"], ["One", "Two"])

    def test_extracts_constants_and_nil(self):
        source = r'''
            lua_setconst(L, "FIRST", VALUE);
            lua_setconst(L, "FIRST", VALUE);
            lua_setconst_nil(L, "DEFAULT_VALUE");
            SetConstant(L, "SECOND", OTHER_VALUE);
            SetNilConstant(L, "OTHER_DEFAULT");
            /* lua_setconst(L, "OLD", 9); */
        '''
        calls, nil_names, duplicates = audit.extract_constant_calls(source)
        self.assertEqual(calls, {"FIRST": "VALUE", "SECOND": "OTHER_VALUE"})
        self.assertEqual(nil_names, {"DEFAULT_VALUE", "OTHER_DEFAULT"})
        self.assertEqual(duplicates, {"FIRST": ["VALUE", "VALUE"]})

    def test_resolves_enum_const_and_macro_values(self):
        source = r'''
            #define FLAG 0x10U
            enum Values {
                VALUE_ZERO,
                VALUE_ONE,
                VALUE_MASK = FLAG | (1 << 2),
            };
            const int VALUE_NEXT = VALUE_MASK + 1;
        '''
        resolver = audit.IntegerResolver(audit.collect_integer_definitions([source]))
        self.assertEqual(resolver.resolve_name("VALUE_ZERO"), 0)
        self.assertEqual(resolver.resolve_name("VALUE_ONE"), 1)
        self.assertEqual(resolver.resolve_name("VALUE_MASK"), 20)
        self.assertEqual(resolver.evaluate("H2EnumIndex(VALUE_NEXT)"), 21)

    def test_reports_ambiguous_unqualified_symbol(self):
        resolver = audit.IntegerResolver({"VALUE": ["1", "2"]})
        with self.assertRaises(audit.AuditError):
            resolver.resolve_name("VALUE")


if __name__ == "__main__":
    unittest.main()
