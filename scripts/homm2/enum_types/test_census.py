import unittest
from pathlib import Path

from .census import evaluate_declarations, parse_declarations


class CensusTest(unittest.TestCase):
    def test_classic_and_dual_mode_declarations(self):
        text = """
typedef enum First { FIRST_NEG = -1, FIRST_ZERO, FIRST_MASK = 0x10UL } First;
HOMM2_ENUM_BEGIN(Second)
    SECOND_START = FIRST_MASK << 1,
    SECOND_NEXT
HOMM2_ENUM_END(Second)
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


if __name__ == "__main__":
    unittest.main()
