import unittest

from homm2.analysis.disasm import (
    _blocks,
    _blocks_diff,
    _branch_insns,
    _branch_seq,
    _branches_compare,
    _cfg,
    _parse_ins,
    _skeleton_diff,
    _slice_public_symbol,
    _sym_branch_target,
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

    def test_branch_insns_rebases_offsets_and_targets_into_one_space(self):
        # A function sliced from mid-object starts at a nonzero offset. Offsets AND
        # branch targets must land in the same rebased space, or every symbolic
        # target comparison spans two address spaces.
        text = """\
00000100 <?Late@@YAXXZ>:
     100: 83 f8 00\tcmp eax, 0x0
     103: 74 05\tje 0x10a
     105: 48\tdec eax
     106: eb f8\tjmp 0x100
     10a: c3\tret
"""
        insns = _branch_insns(text)

        self.assertEqual(insns[0][0], 0)
        self.assertEqual(insns[1], (0x3, "je", "0xa"))
        self.assertEqual(insns[3], (0x6, "jmp", "0x0"))

    def test_branch_seq_takes_conditionals_only(self):
        insns = _branch_insns(self.CFG_TEXT)

        brs = _branch_seq(insns)

        self.assertEqual(brs, [(0x3, "je", 0xa)])

    def test_branch_seq_dies_on_unclassified_jump(self):
        with self.assertRaises(SystemExit):
            _branch_seq([(0, "jecxz", "0x4"), (2, "ret", "")])

    def test_branches_compare_truncates_at_table_decode_garbage(self):
        # Jump-table bytes decode as valid-looking loop/jecxz forms the retail compiler never
        # emits; the comparison must stop there (reliable prefix), not die mid-render.
        base = [(0x0, "je", "0x8"), (0x4, "jmp", "dword ptr [eax*4 + 0x10]"),
                (0x8, "jecxz", "0x2"), (0xa, "loop", "0x4")]
        res = _branches_compare(base, base)

        self.assertEqual(res["status"], "clean")
        self.assertEqual(res["trunc"], (0x8, 0x8))
        self.assertEqual(res["nbr"], 1)

    def test_sym_branch_target_makes_uniform_shift_equal(self):
        base = [(0x3, "je", 0x20), (0x10, "jne", 0x30)]
        shifted = [(0x3, "je", 0x24), (0x12, "jne", 0x34)]

        self.assertEqual([_sym_branch_target(base, t) for _, _, t in base],
                         [_sym_branch_target(shifted, t) for _, _, t in shifted])

    def test_branches_compare_classifies_flip_kinds(self):
        base = [(0x0, "cmp", "eax, 0x0"), (0x3, "jl", "0x10"), (0x10, "ret", "")]
        signed = [(0x0, "cmp", "eax, 0x0"), (0x3, "jb", "0x10"), (0x10, "ret", "")]
        inverted = [(0x0, "cmp", "eax, 0x0"), (0x3, "jge", "0x10"), (0x10, "ret", "")]

        self.assertEqual(_branches_compare(base, signed)["kind"], "SIGNEDNESS")
        self.assertEqual(_branches_compare(base, inverted)["kind"], "POLARITY")
        self.assertEqual(_branches_compare(base, base)["status"], "clean")

    def test_branches_compare_sees_retarget_as_topology(self):
        # Identical mnemonics; the second branch lands before vs after the first
        # branch's landing site - the exact shape --diff masks.
        base = [(0x0, "je", "0x8"), (0x4, "jne", "0x8"),
                (0x8, "inc", "eax"), (0x9, "ret", "")]
        moved = [(0x0, "je", "0x8"), (0x4, "jne", "0x2"),
                 (0x8, "inc", "eax"), (0x9, "ret", "")]

        res = _branches_compare(base, moved)

        self.assertEqual(res["status"], "topology")
        self.assertEqual(res["rows"], [(1, 2, 1)])

    def test_branches_compare_prefixes_when_one_side_truncates(self):
        # The jump table decodes as garbage on one side only, so the two
        # truncation points are not comparable: compare the common prefix
        # instead of reporting a spurious structural difference.
        base = [(0x0, "je", "0x8"), (0x4, "jne", "0x8"),
                (0x8, "jmp", "dword ptr [eax*4 + 0x10]"), (0xe, "loop", "0x4")]
        target = [(0x0, "je", "0x8"), (0x4, "jne", "0x8"),
                  (0x8, "jmp", "dword ptr [eax*4 + 0x10]"),
                  (0xe, "push", "eax"), (0xf, "jge", "0x2")]
        res = _branches_compare(base, target)

        self.assertEqual(res["status"], "clean")
        self.assertTrue(res["partial"])
        self.assertEqual((res["nbr"], res["nbr_t"]), (2, 3))

    def test_branches_compare_count_mismatch_is_struct(self):
        base = [(0x0, "je", "0x4"), (0x4, "ret", "")]
        extra = [(0x0, "je", "0x4"), (0x4, "jne", "0x8"), (0x8, "ret", "")]

        res = _branches_compare(base, extra)

        self.assertEqual(res["status"], "struct")
        self.assertEqual((res["nbr"], res["nbr_t"]), (1, 2))

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
