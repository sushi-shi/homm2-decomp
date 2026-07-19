import unittest

from format_headers import format_text


class FormatHeadersTests(unittest.TestCase):
    def test_separates_guard_includes_and_declarations(self):
        source = """\
#ifndef HOMM2_ITEM_H
#define HOMM2_ITEM_H
#include <va.h>
#include <BASE/item.h>
class Item;
#endif
"""
        expected = """\
#ifndef HOMM2_ITEM_H
#define HOMM2_ITEM_H

#include <va.h>
#include <BASE/item.h>

class Item;
#endif
"""
        self.assertEqual(format_text(source), expected)
        self.assertEqual(format_text(expected), expected)

    def test_leaves_non_guarded_files_unchanged(self):
        source = "#pragma once\n#include <item.h>\n"
        self.assertEqual(format_text(source), source)


if __name__ == "__main__":
    unittest.main()
