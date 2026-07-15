import struct
import tempfile
import unittest
from pathlib import Path

from homm2.build.extract_resources import (
    read_pe_resources, resource_summary, write_res)


def synthetic_resource_pe():
    data = bytearray(0x400)
    data[:2] = b"MZ"
    struct.pack_into("<I", data, 0x3C, 0x80)
    data[0x80:0x84] = b"PE\0\0"
    struct.pack_into("<H", data, 0x84 + 2, 1)
    struct.pack_into("<H", data, 0x84 + 16, 0xE0)
    optional = 0x84 + 20
    struct.pack_into("<H", data, optional, 0x10B)
    struct.pack_into("<II", data, optional + 96 + 2 * 8, 0x2000, 0x100)
    section = optional + 0xE0
    data[section:section + 8] = b".rsrc\0\0\0"
    struct.pack_into("<IIII", data, section + 8, 0x100, 0x2000, 0x200, 0x200)

    base = 0x200
    struct.pack_into("<IIHHHH", data, base, 0, 1, 0, 0, 0, 1)
    struct.pack_into("<II", data, base + 0x10, 3, 0x80000020)
    struct.pack_into("<IIHHHH", data, base + 0x20, 0, 2, 0, 0, 0, 1)
    struct.pack_into("<II", data, base + 0x30, 1, 0x80000040)
    struct.pack_into("<IIHHHH", data, base + 0x40, 0, 3, 0, 0, 0, 1)
    struct.pack_into("<II", data, base + 0x50, 0x409, 0x60)
    struct.pack_into("<IIII", data, base + 0x60, 0x2080, 4, 0, 0)
    data[base + 0x80:base + 0x84] = b"icon"
    return data


class ExtractResourcesTest(unittest.TestCase):
    def test_reads_typed_resource_leaf_and_payload(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "test.exe"
            path.write_bytes(synthetic_resource_pe())
            resources = read_pe_resources(path)
        self.assertEqual(len(resources), 1)
        self.assertEqual((resources[0]["type"], resources[0]["name"],
                          resources[0]["language"]), (3, 1, 0x409))
        self.assertEqual(resources[0]["data"], b"icon")

    def test_writes_standard_res_null_header_and_resource_record(self):
        with tempfile.TemporaryDirectory() as temp:
            exe = Path(temp) / "test.exe"
            output = Path(temp) / "test.res"
            exe.write_bytes(synthetic_resource_pe())
            write_res(read_pe_resources(exe), output)
            data = output.read_bytes()
        self.assertEqual(struct.unpack_from("<II", data, 0), (0, 32))
        self.assertEqual(struct.unpack_from("<II", data, 32), (4, 32))
        self.assertEqual(struct.unpack_from("<HHHH", data, 40), (0xFFFF, 3, 0xFFFF, 1))
        self.assertEqual(data[64:68], b"icon")

    def test_summary_counts_resource_types_and_payload_bytes(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "test.exe"
            path.write_bytes(synthetic_resource_pe())
            summary = resource_summary(read_pe_resources(path))
        self.assertEqual(summary["count"], 1)
        self.assertEqual(summary["payload_bytes"], 4)
        self.assertEqual(summary["types"], {"3": 1})


if __name__ == "__main__":
    unittest.main()
