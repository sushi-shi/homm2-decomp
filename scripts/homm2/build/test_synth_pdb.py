import unittest

from homm2.build.synth_pdb import merge_unmatched


def retail_row(rva, size, name):
    return {"entry_rva": "0x%x" % rva, "byte_size": str(size), "name": name}


class SynthPdbTest(unittest.TestCase):
    def test_unclaimed_functions_land_in_the_unmatched_module(self):
        funcs = []
        merged = merge_unmatched(
            funcs,
            [retail_row(0x1000, 754, "FUN_00401000"),
             retail_row(0x12f2, 2389, "FUN_004012f2")],
            lambda rva: True)
        self.assertEqual(merged, 2)
        self.assertEqual(funcs, [
            (0x1000, 754, "FUN_00401000", "(unmatched)"),
            (0x12f2, 2389, "FUN_004012f2", "(unmatched)"),
        ])

    def test_claimed_rvas_are_excluded(self):
        funcs = [(0x1000, 754, "?Claimed@@YIXXZ", "SOURCE/AGG")]
        merged = merge_unmatched(
            funcs,
            [retail_row(0x1000, 754, "FUN_00401000"),
             retail_row(0x2000, 100, "FUN_00402000")],
            lambda rva: True)
        self.assertEqual(merged, 1)
        self.assertEqual(funcs[1], (0x2000, 100, "FUN_00402000", "(unmatched)"))

    def test_rows_outside_text_or_without_size_are_skipped(self):
        funcs = []
        merged = merge_unmatched(
            funcs,
            [retail_row(0xF0000, 16, "FUN_data"),
             retail_row(0x1000, 0, "FUN_empty")],
            lambda rva: rva < 0xE0000)
        self.assertEqual(merged, 0)
        self.assertEqual(funcs, [])


if __name__ == "__main__":
    unittest.main()
