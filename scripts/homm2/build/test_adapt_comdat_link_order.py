import struct
import unittest

from homm2.build.adapt_comdat_link_order import CoffError, adapt


def _symbol(name_offset, section, *, storage=2, auxiliary=b""):
    name = b"\0\0\0\0" + struct.pack("<I", name_offset)
    primary = name + struct.pack("<IhHBB", 0, section, 0, storage, bool(auxiliary))
    return primary + auxiliary


def _section_symbol(section, auxiliary):
    return b".text\0\0\0" + struct.pack("<IhHBB", 0, section, 0, 3, 1) + auxiliary


def fixture():
    names = [
        "??1AudiereSampleNode@@QAE@XZ",
        "_$E20",
    ]
    strings = bytearray(struct.pack("<I", 0))
    offsets = {}
    for name in names:
        offsets[name] = len(strings)
        strings.extend(name.encode("ascii") + b"\0")
    struct.pack_into("<I", strings, 0, len(strings))

    section_count = 4
    section_table_size = section_count * 40
    payload_offset = 20 + section_table_size
    headers = []
    for index, name in enumerate((b".text", b".text", b".text", b".data")):
        characteristics = 0x60101020 if name == b".text" else 0xC0300040
        headers.append(struct.pack(
            "<8sIIIIIIHHI",
            name.ljust(8, b"\0"), 0, 0, 1, payload_offset + index,
            0, 0, 0, 0, characteristics,
        ))

    section_aux = bytearray(18)
    struct.pack_into("<hB", section_aux, 12, 2, 5)
    symbols = b"".join((
        _symbol(offsets[names[0]], 2),
        _symbol(offsets[names[1]], 3),
        _section_symbol(1, bytes(section_aux)),
    ))
    symbol_count = 4
    symbol_offset = payload_offset + section_count
    header = struct.pack(
        "<HHIIIHH", 0x14C, section_count, 0, symbol_offset, symbol_count, 0, 0
    )
    return header + b"".join(headers) + b"ABCD" + symbols + bytes(strings)


class AdaptComdatLinkOrderTests(unittest.TestCase):
    def test_moves_section_after_anchor_and_remaps_symbols(self):
        before = fixture()
        after = adapt(before, "BASE/AudiereEffects")

        # Old section 2 moves after old section 3: [1, 3, 2, 4].
        pointers = [struct.unpack_from("<I", after, 20 + i * 40 + 20)[0]
                    for i in range(4)]
        self.assertEqual(pointers, [180, 182, 181, 183])

        symbol_offset = struct.unpack_from("<I", after, 8)[0]
        self.assertEqual(struct.unpack_from("<h", after, symbol_offset + 12)[0], 3)
        self.assertEqual(struct.unpack_from("<h", after, symbol_offset + 18 + 12)[0], 2)
        section_symbol = symbol_offset + 36
        self.assertEqual(struct.unpack_from("<h", after, section_symbol + 12)[0], 1)
        self.assertEqual(struct.unpack_from("<h", after, section_symbol + 18 + 12)[0], 3)
        self.assertEqual(before[180:184], after[180:184])

    def test_rejects_unreviewed_unit(self):
        with self.assertRaisesRegex(CoffError, "not a reviewed"):
            adapt(fixture(), "BASE/Misc")


if __name__ == "__main__":
    unittest.main()
