import tempfile
import unittest
from collections import Counter
from pathlib import Path

from homm2.build.build_reloc_target import assert_no_metadata_symbols
from homm2.build.gen_reloc_aliases import choose_aliases
from homm2.build.reloc_alias_metadata import (
    RelocAlias, encode_pdb_alias, load_aliases, write_aliases,
)
from homm2.build.synth_pdb import validate_reloc_aliases


OWNER = "?gConfig@@3UconfigStruct@@A"


class AliasChoiceTest(unittest.TestCase):
    def test_majority_wins_and_ties_are_lexical(self):
        proofs = {
            0x128D50: Counter({(OWNER, 0x30): 14, ("?zOwner@@3HA", 0x10): 2}),
            0x2000: Counter({("?bOwner@@3HA", 0x10): 3,
                             ("?aOwner@@3HA", 0x20): 3}),
        }

        aliases = choose_aliases(proofs)

        self.assertEqual(
            aliases[0x128D50], RelocAlias(0x128D50, OWNER, 0x30, 14, 1))
        self.assertEqual(
            aliases[0x2000],
            RelocAlias(0x2000, "?aOwner@@3HA", 0x20, 3, 1))

    def test_zero_addend_public_symbol_needs_no_alias(self):
        self.assertEqual(
            choose_aliases({0x128D20: Counter({(OWNER, 0): 4})}), {})


class AliasMetadataTest(unittest.TestCase):
    def test_tsv_round_trip_and_pdb_encoding_are_deterministic(self):
        aliases = {
            0x128D50: RelocAlias(0x128D50, OWNER, 0x30, 14, 0),
            0x1004: RelocAlias(0x1004, "?owner@@3HA", 4, 1, 0),
        }
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / "aliases.tsv"
            write_aliases(path, aliases)

            self.assertEqual(load_aliases(path), aliases)
            self.assertEqual(
                path.read_text(encoding="latin-1").splitlines()[1],
                "0x00001004\t?owner@@3HA\t0x00000004\t1\t0")

        self.assertEqual(
            encode_pdb_alias(aliases[0x128D50]),
            "__homm2_data_alias$00000030$" + OWNER)

    def test_synth_validation_rejects_wrong_addend(self):
        rows = [{
            "rva": "0x128d20", "name": OWNER, "kind": "data",
            "provenance": "cv-public-data",
        }]
        aliases = {
            0x128D50: RelocAlias(0x128D50, OWNER, 0x1C, 1, 0),
        }

        with self.assertRaisesRegex(ValueError, "does not resolve"):
            validate_reloc_aliases(rows, aliases)

    def test_synth_validation_rejects_non_public_owner(self):
        rows = [{
            "rva": "0x128d20", "name": OWNER, "kind": "data",
            "provenance": "recovered-data",
        }]
        aliases = {
            0x128D50: RelocAlias(0x128D50, OWNER, 0x30, 1, 0),
        }

        with self.assertRaisesRegex(ValueError, "not public data"):
            validate_reloc_aliases(rows, aliases)

    def test_delink_output_must_not_contain_metadata_alias(self):
        with tempfile.TemporaryDirectory() as temporary:
            obj = Path(temporary) / "unit.obj"
            obj.write_bytes(b"coff\0__homm2_data_alias$00000030$owner")

            with self.assertRaisesRegex(RuntimeError, "leaked PDB owner metadata"):
                assert_no_metadata_symbols(temporary)


if __name__ == "__main__":
    unittest.main()
