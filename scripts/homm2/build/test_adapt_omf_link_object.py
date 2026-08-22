import struct
import unittest

from homm2.build.adapt_omf_link_object import (
    CVTOMF_600_COMP_ID,
    IMAGE_SCN_ALIGN_4BYTES,
    IMAGE_SCN_ALIGN_MASK,
    VC6_CPP_COMP_ID,
    CoffError,
    adapt,
)


def fixture(*, comp_id=VC6_CPP_COMP_ID, text_alignment=0x00500000):
    sections = [
        struct.pack(
            "<8sIIIIIIHHI",
            b".drectve",
            0,
            0,
            4,
            140,
            0,
            0,
            0,
            0,
            0x00100A00,
        ),
        struct.pack(
            "<8sIIIIIIHHI",
            b".text\0\0\0",
            0,
            0,
            3,
            144,
            147,
            0,
            1,
            0,
            0x60101020 | text_alignment,
        ),
        struct.pack(
            "<8sIIIIIIHHI",
            b".text\0\0\0",
            0,
            0,
            2,
            157,
            0,
            0,
            0,
            0,
            0x60101020 | 0x00100000,
        ),
    ]
    symbol_offset = 159
    header = struct.pack(
        "<HHIIIHH", 0x14C, len(sections), 0, symbol_offset, 1, 0, 0
    )
    payload = b"DIR!" + b"\x90\x91\xc3" + b"\0" * 10 + b"\xcc\xc3"
    symbol = b"@comp.id" + struct.pack("<IhHBB", comp_id, -1, 0, 3, 0)
    return header + b"".join(sections) + payload + symbol + struct.pack("<I", 4)


class AdaptOmfLinkObjectTests(unittest.TestCase):
    def test_changes_only_first_text_alignment_and_comp_id(self):
        before = fixture()
        after = adapt(before, "BASE/TILE")

        first_characteristics = struct.unpack_from("<I", after, 20 + 40 + 36)[0]
        second_characteristics = struct.unpack_from("<I", after, 20 + 80 + 36)[0]
        self.assertEqual(
            first_characteristics & IMAGE_SCN_ALIGN_MASK,
            IMAGE_SCN_ALIGN_4BYTES,
        )
        self.assertEqual(second_characteristics & IMAGE_SCN_ALIGN_MASK, 0x00100000)
        self.assertEqual(struct.unpack_from("<I", after, 159 + 8)[0], CVTOMF_600_COMP_ID)

        self.assertEqual(before[:96], after[:96])
        self.assertEqual(before[100:167], after[100:167])
        self.assertEqual(before[171:], after[171:])

    def test_rejects_unreviewed_unit(self):
        with self.assertRaisesRegex(CoffError, "not a reviewed"):
            adapt(fixture(), "BASE/Misc")

    def test_rejects_unexpected_producer(self):
        with self.assertRaisesRegex(CoffError, "unexpected @comp.id"):
            adapt(fixture(comp_id=0), "BASE/BITS")


if __name__ == "__main__":
    unittest.main()
