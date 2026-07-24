import unittest

from homm2.analysis.disasm import _slice_public_symbol


class DisasmTest(unittest.TestCase):
    def test_slice_public_symbol_keeps_private_labels(self):
        text = """\
Disassembly of section .text:

00000000 <?First@@YAXXZ>:
       0: 90                            nop
00000001 <$L1>:
       1: eb 00                         jmp 0x3
00000003 <?Second@@YAXXZ>:
       3: c3                            ret
"""
        actual = _slice_public_symbol(
            text, "?First@@YAXXZ", 0, {"?First@@YAXXZ", "?Second@@YAXXZ"})

        self.assertEqual(actual, """\
00000000 <?First@@YAXXZ>:
       0: 90                            nop
00000001 <$L1>:
       1: eb 00                         jmp 0x3
""")

    def test_slice_public_symbol_stops_at_next_comdat_section(self):
        text = """\
Disassembly of section .text:

00000000 <?First@@YAXXZ>:
       0: c3                            ret

Disassembly of section .text:

00000000 <?Second@@YAXXZ>:
       0: c3                            ret
"""
        actual = _slice_public_symbol(
            text, "?First@@YAXXZ", 0, {"?First@@YAXXZ", "?Second@@YAXXZ"})

        self.assertEqual(actual, """\
00000000 <?First@@YAXXZ>:
       0: c3                            ret
""")

    def test_slice_public_symbol_selects_duplicate_ordinal(self):
        text = """\
Disassembly of section .text:

00000000 <?Duplicate@@YAXXZ>:
       0: 90                            nop

Disassembly of section .text:

00000000 <?Duplicate@@YAXXZ>:
       0: c3                            ret
"""
        actual = _slice_public_symbol(
            text, "?Duplicate@@YAXXZ", 1, {"?Duplicate@@YAXXZ"})

        self.assertEqual(actual, """\
00000000 <?Duplicate@@YAXXZ>:
       0: c3                            ret
""")


if __name__ == "__main__":
    unittest.main()
