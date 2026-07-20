import unittest

from format_enums import format_text


class FormatEnumsTests(unittest.TestCase):
    def test_aligns_each_enum_independently(self):
        source = """\
H2_ENUM_BEGIN(First)
    SHORT = -1,
    LONG_CONSTANT = 16
H2_ENUM_END(First)

H2_ENUM_CLASS_BEGIN(Second)
    A = 1,
    BB = OTHER = 2
H2_ENUM_CLASS_END(Second)
"""
        expected = """\
H2_ENUM_BEGIN(First)
    SHORT         = -1,
    LONG_CONSTANT = 16
H2_ENUM_END(First)

H2_ENUM_CLASS_BEGIN(Second)
    A  = 1,
    BB = OTHER = 2
H2_ENUM_CLASS_END(Second)
"""
        self.assertEqual(format_text(source), expected)

    def test_is_idempotent_and_preserves_non_enum_assignments(self):
        source = """\
value = 1;
H2_ENUM_BEGIN_T(Item, u8)
    ONE   = 1,
    THREE = 3
H2_ENUM_END_T(Item, u8)
other = 2;
"""
        self.assertEqual(format_text(format_text(source)), source)

    def test_multiline_assignment_has_no_trailing_space(self):
        source = """\
H2_ENUM_BEGIN(Item)
    SHORT = 1,
    LONG_NAME =
        OTHER + 1
H2_ENUM_END(Item)
"""
        expected = """\
H2_ENUM_BEGIN(Item)
    SHORT     = 1,
    LONG_NAME =
        OTHER + 1
H2_ENUM_END(Item)
"""
        self.assertEqual(format_text(source), expected)

    def test_aligns_split_storage_enum(self):
        source = """\
H2_ENUM_CLASS_BEGIN_SPLIT(FactionType, i8)
    FACTION_UNINITIALIZED = -2,
    FACTION_ANY = -1
H2_ENUM_CLASS_END_SPLIT(FactionType, i8)
"""
        expected = """\
H2_ENUM_CLASS_BEGIN_SPLIT(FactionType, i8)
    FACTION_UNINITIALIZED = -2,
    FACTION_ANY           = -1
H2_ENUM_CLASS_END_SPLIT(FactionType, i8)
"""
        self.assertEqual(format_text(source), expected)


if __name__ == "__main__":
    unittest.main()
