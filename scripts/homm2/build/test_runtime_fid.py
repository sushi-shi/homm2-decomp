import unittest
from pathlib import Path
from tempfile import TemporaryDirectory

from homm2.build.runtime_fid import _matches, label_seeds, signatures
from homm2.build.test_canonicalize_data_symbols import (
    DIR32,
    TEXT,
    SectionSpec,
    make_coff,
)


class RuntimeFidTest(unittest.TestCase):
    def test_reviewed_names_remain_available_as_interior_entry_anchors(self):
        with TemporaryDirectory() as temporary:
            path = Path(temporary) / "labels.csv"
            path.write_text(
                "rva,size,name,unit\n"
                "0xe362c,0x20,__fFATN2,87triga\n",
                encoding="utf-8")

            seeds = label_seeds(path)

        self.assertEqual(seeds[0]["name"], "__fFATN2")

    def test_exact_signature_masks_only_relocation_operands(self):
        body = bytearray(b"\x55\x8b\xec\xa1\x11\x22\x33\x44\x5d\xc3")
        payload = make_coff([
            SectionSpec(".text", bytes(body), TEXT, ((4, 1, DIR32),)),
        ], [
            ("_helper", 0, 1, 0x20, 2),
            ("_global", 0, 0, 0, 2),
        ])

        found = [row for row in signatures(payload, "sample.obj")
                 if row.name == "_helper"]

        self.assertEqual(len(found), 1)
        signature = found[0]
        self.assertEqual(signature.size, len(body))
        relocated = bytearray(body)
        relocated[4:8] = b"\xaa\xbb\xcc\xdd"
        self.assertTrue(_matches(bytes(relocated), signature))
        relocated[1] ^= 1
        self.assertFalse(_matches(bytes(relocated), signature))


if __name__ == "__main__":
    unittest.main()
