import tempfile
import unittest
from collections import Counter
from pathlib import Path

from homm2.build.gen_reloc_aliases import choose_aliases
from homm2.build.reloc_alias_metadata import (
    RelocAlias, load_aliases, write_aliases,
)


OWNER = "?gConfig@@3UconfigStruct@@A"


class AliasChoiceTest(unittest.TestCase):
    def test_unanimous_owner_is_selected(self):
        proofs = {
            (0x4000, 0x128D50): Counter({(OWNER, 0x30): 14}),
        }

        aliases, ambiguous = choose_aliases(proofs)

        self.assertEqual(
            aliases,
            [RelocAlias(
                0x4000, 0x128D50, OWNER, 0x30, 14,
                "candidate-function-reloc-multiset")])
        self.assertEqual(ambiguous, [])

    def test_competing_owner_spellings_remain_a_multiset(self):
        proofs = {
            (0x4000, 0x128D50): Counter({
                (OWNER, 0x30): 14, ("?other@@3HA", 0x10): 2}),
        }
        aliases, ambiguous = choose_aliases(proofs)
        self.assertEqual(aliases, [])
        self.assertEqual(ambiguous[0]["function_rva"], 0x4000)
        self.assertEqual(len(ambiguous[0]["spellings"]), 2)

    def test_zero_addend_public_symbol_needs_no_alias(self):
        self.assertEqual(
            choose_aliases({(0x4000, 0x128D20): Counter({(OWNER, 0): 4})}),
            ([], []))


class AliasMetadataTest(unittest.TestCase):
    def test_tsv_round_trip_and_pdb_encoding_are_deterministic(self):
        aliases = [
            RelocAlias(0x3000, 0x1004, "?owner@@3HA", 4, 1, "reviewed"),
            RelocAlias(0x4000, 0x128D50, OWNER, 0x30, 14, "reviewed"),
        ]
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / "aliases.tsv"
            write_aliases(path, aliases)

            self.assertEqual(load_aliases(path), aliases)
            self.assertEqual(
                path.read_text(encoding="latin-1").splitlines()[1],
                "0x00003000\t0x00001004\t?owner@@3HA\t0x00000004\t1\treviewed")


if __name__ == "__main__":
    unittest.main()
