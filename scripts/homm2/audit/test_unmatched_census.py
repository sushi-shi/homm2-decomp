import unittest
from types import SimpleNamespace

from homm2.audit.unmatched_census import (
    align_sizes,
    function_spans,
    mask_bytes,
    order_runs,
    trim_padding,
)


class UnmatchedCensusTest(unittest.TestCase):
    def test_mask_zeroes_relocation_fields_only(self):
        masked = mask_bytes(bytes(range(1, 11)), [2, 8])
        self.assertEqual(masked, bytes([1, 2, 0, 0, 0, 0, 7, 8, 0, 0]))

    def test_trim_drops_alignment_fill_but_keeps_real_zero_tails(self):
        self.assertEqual(trim_padding(b"\x5e\x5d\xc3\x90\x90"), b"\x5e\x5d\xc3")
        self.assertEqual(trim_padding(b"\xc3\xcc\xcc"), b"\xc3")
        self.assertEqual(trim_padding(b"\xe9\x00\x00\x00\x00"),
                         b"\xe9\x00\x00\x00\x00")
        self.assertEqual(trim_padding(b"\xcc"), b"\xcc")

    def test_function_spans_follow_definition_order(self):
        symbols = [SimpleNamespace(name="b", value=8),
                   SimpleNamespace(name="a", value=0)]
        self.assertEqual(
            [(symbol.name, start, end)
             for symbol, start, end in function_spans(symbols, 20)],
            [("a", 0, 8), ("b", 8, 20)])

    def test_size_alignment_is_monotone_and_tolerant(self):
        # 100~104 pair, 400 skips the 30-byte slot, 500~someday pairs at end
        pairs = align_sizes([100, 400, 500], [104, 30, 480])
        self.assertEqual(pairs, [(0, 0), (2, 2)])
        self.assertEqual(align_sizes([], [10]), [])

    def test_order_runs_split_on_address_regressions(self):
        runs = order_runs([(0, 0x100), (1, 0x180), (3, 0x80), (4, 0x200)])
        self.assertEqual(runs, [[(0, 0x100), (1, 0x180)],
                                [(3, 0x80), (4, 0x200)]])


if __name__ == "__main__":
    unittest.main()
