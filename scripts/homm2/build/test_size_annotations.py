"""Game layout declarations do not depend on removed size-assertion macros."""
from pathlib import Path
import re
import unittest


ROOT = Path(__file__).resolve().parents[3]


class SizeAnnotationRemovalTests(unittest.TestCase):
    def test_game_tree_has_no_size_annotation_definitions_or_calls(self):
        for folder in ('src', 'include'):
            for path in sorted((ROOT / folder).rglob('*')):
                if path.suffix not in ('.cpp', '.h'):
                    continue
                with self.subTest(path=str(path.relative_to(ROOT))):
                    self.assertIsNone(re.search(r'\b(?:SIZE|NEW_SIZE)\s*\(', path.read_text()))

    def parse_enum_fixture(self, dialect, assertions):
        from clang import cindex
        source = str(ROOT / 'build/size-annotation-fixture.cpp')
        text = '''#include <va.h>
#if defined(SIZE) || defined(NEW_SIZE)
#error obsolete size annotation is defined
#endif
H2_ENUM_BEGIN(PlainDomain)
    PLAIN_FIRST = 0
H2_ENUM_END(PlainDomain)
H2_ENUM_CLASS_BEGIN_T(ByteDomain, u8)
    BYTE_FIRST = 0
H2_ENUM_CLASS_END_T(ByteDomain, u8)
H2_ENUM_CLASS_BEGIN_SPLIT(SplitDomain, i8)
    SPLIT_FIRST = 0
H2_ENUM_CLASS_END_SPLIT(SplitDomain, i8)
struct StoredDomain {
    H2_ENUM_STORAGE(SplitDomain, i8) field;
};
'''
        tu = cindex.Index.create().parse(source, args=[
            '-x', 'c++', '-std=' + dialect, '-fms-extensions',
            '-target', 'i686-pc-windows-msvc', '-I' + str(ROOT / 'include'),
        ], unsaved_files=[(source, text + assertions)])
        errors = [str(d) for d in tu.diagnostics if d.severity >= cindex.Diagnostic.Error]
        self.assertEqual(errors, [])

    def test_retail_enum_paths_remain_integer_typedefs(self):
        self.parse_enum_fixture('c++98', '''
typedef char PlainIsInt[__is_same(PlainDomain, i32) ? 1 : -1];
typedef char ByteIsByte[__is_same(ByteDomain, u8) ? 1 : -1];
typedef char SplitIsInt[__is_same(SplitDomain, i32) ? 1 : -1];
typedef char StoredIsByte[sizeof(StoredDomain) == 1 ? 1 : -1];
''')

    def test_modern_enum_paths_still_have_declared_storage(self):
        self.parse_enum_fixture('c++20', '''
static_assert(__is_enum(PlainDomain));
static_assert(__is_enum(ByteDomain));
static_assert(__is_enum(SplitDomain));
static_assert(sizeof(ByteDomain) == 1);
static_assert(sizeof(SplitDomain) == 1);
static_assert(sizeof(StoredDomain) == 1);
''')


if __name__ == '__main__':
    unittest.main()
