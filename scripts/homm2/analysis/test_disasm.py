import unittest

from homm2.analysis.disasm import (
    _blocks,
    _blocks_diff,
    _cfg,
    _parse_ins,
    _skeleton_diff,
    _slice_public_symbol,
)


class DisasmTest(unittest.TestCase):
    CFG_TEXT = """\
00000000 <?Loop@@YAXXZ>:
       0: 83 f8 00\tcmp eax, 0x0
       3: 74 05\tje 0xa
       5: 48\tdec eax
       6: eb f8\tjmp 0x0
       8: 90\tnop
       a: c3\tret
       b: 90\tnop
"""

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

    def test_parse_ins_accepts_gnu_objdump_byte_column(self):
        self.assertEqual(
            _parse_ins("   0:\t8b 44 24 0c\tmov eax,DWORD PTR [esp+0xc]"),
            (0, "mov eax,DWORD PTR [esp+0xc]"),
        )

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

    def test_cfg_splits_targets_and_marks_back_edge(self):
        cfg = _cfg(self.CFG_TEXT)

        self.assertEqual(len(cfg), 4)
        self.assertEqual(cfg[0][2], "jcc B3 | fall B1")
        self.assertEqual(cfg[1][2], "jmp B0^")
        self.assertEqual(cfg[3][2], "ret")

    def test_cfg_strips_trailing_alignment_nop(self):
        cfg = _cfg(self.CFG_TEXT)

        self.assertEqual(cfg[-1][1], ["ret"])
        self.assertNotIn("nop", "\n".join(cfg[-1][1]))

    def test_blocks_lite_exposes_skeleton(self):
        output = _blocks(self.CFG_TEXT, lite=True)

        self.assertIn("B0", output)
        self.assertIn("[jcc B3 | fall B1]", output)
        self.assertIn("LOOP", output)

    def test_identical_block_diff_is_exact(self):
        output, exact = _blocks_diff(self.CFG_TEXT, self.CFG_TEXT)

        self.assertTrue(exact)
        self.assertIn("[all aligned blocks identical]", output)

    def test_skeleton_diff_detects_terminator_change(self):
        base = _cfg(self.CFG_TEXT)
        target = [(address, body, "ret" if i == 0 else term)
                  for i, (address, body, term) in enumerate(base)]

        output, exact = _skeleton_diff(base, target)

        self.assertFalse(exact)
        self.assertIn("!!", output)
        self.assertIn("first branch-kind divergence", output)

    def test_skeleton_diff_separates_size_only_change(self):
        base = _cfg(self.CFG_TEXT)
        target = [(address, body + ["inc eax"] if i == 0 else body, term)
                  for i, (address, body, term) in enumerate(base)]

        output, exact = _skeleton_diff(base, target)

        self.assertFalse(exact)
        self.assertIn("1 size-only", output)
        self.assertIn(" ## ", output)


if __name__ == "__main__":
    unittest.main()
