import unittest
from contextlib import redirect_stdout
from io import StringIO
from pathlib import Path

from .census import EnumDeclaration, Enumerator, evaluate_declarations, parse_declarations, validate_manifest


class CensusTest(unittest.TestCase):
    def test_classic_and_dual_mode_declarations(self):
        text = """
typedef enum First { FIRST_NEG = -1, FIRST_ZERO, FIRST_MASK = 0x10UL } First;
H2_ENUM_CLASS_BEGIN(Second)
    SECOND_START = FIRST_MASK << 1,
    SECOND_NEXT
H2_ENUM_CLASS_END(Second)
"""
        declarations = parse_declarations(Path(__file__), text)
        evaluate_declarations(declarations)
        self.assertEqual([item.name for item in declarations], ["First", "Second"])
        self.assertEqual([item.value for item in declarations[0].enumerators], [-1, 0, 0x10])
        self.assertEqual([item.value for item in declarations[1].enumerators], [0x20, 0x21])
        self.assertEqual(declarations[0].minimum, -1)
        self.assertEqual(declarations[1].maximum, 0x21)

    def test_comments_and_character_constants_are_supported(self):
        text = "typedef enum Key { KEY_A = 'A', /* gap */ KEY_B } Key;"
        declarations = parse_declarations(Path(__file__), text)
        evaluate_declarations(declarations)
        self.assertEqual([item.value for item in declarations[0].enumerators], [65, 66])

    def test_anonymous_constant_group_is_inventoried(self):
        text = "enum { THING_WIDTH = 10, THING_HEIGHT = 20 };"
        declaration = parse_declarations(Path(__file__), text)[0]
        self.assertEqual(declaration.name, "AnonymousEnum@THING_WIDTH")
        self.assertEqual(declaration.form, "anonymous-enum")

    def test_manifest_requires_explicit_valid_enumerators(self):
        declaration = EnumDeclaration(
            name="Mode", tag="Mode", owner="include/mode.h", line=1, end_line=3,
            form="dual-mode", enumerators=[Enumerator("MODE_ONE", "1", 1, 2)],
        )
        manifest = {"version": 1, "domain": [{
            "name": "Mode", "declaration": "Mode", "owner": "include/mode.h",
            "category": "value", "production_carrier": "int", "strict": True,
            "status": "reviewed", "sentinels": [], "enumerators": ["MODE_ONE"],
            "interfaces": [], "members": [], "storage": [], "conversions": [],
            "evidence": ["test evidence"],
        }]}
        with redirect_stdout(StringIO()):
            result = validate_manifest([declaration], manifest, require_complete=True)
        self.assertEqual(result, 0)
        manifest["domain"][0]["enumerators"] = ["MODE_UNKNOWN"]
        with redirect_stdout(StringIO()):
            result = validate_manifest([declaration], manifest)
        self.assertEqual(result, 1)

    def test_complete_manifest_rejects_unclassified_declarations(self):
        declaration = EnumDeclaration(
            name="Mode", tag="Mode", owner="include/mode.h", line=1, end_line=3,
            form="typedef-enum", enumerators=[Enumerator("MODE_ONE", "1", 1, 2)],
        )
        with redirect_stdout(StringIO()):
            result = validate_manifest(
                [declaration], {"version": 1, "domain": []}, require_complete=True
            )
        self.assertEqual(result, 1)


if __name__ == "__main__":
    unittest.main()
