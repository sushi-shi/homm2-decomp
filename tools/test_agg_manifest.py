#!/usr/bin/env python3

import hashlib
import importlib.util
import pathlib
import struct
import tempfile
import unittest


MODULE_PATH = pathlib.Path(__file__).with_name("agg_manifest.py")
SPEC = importlib.util.spec_from_file_location("agg_manifest", MODULE_PATH)
agg_manifest = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(agg_manifest)


def make_agg(path, entries):
    directory_end = 2 + len(entries) * 12
    offset = directory_end
    directory = []
    payload = bytearray()
    names = bytearray()
    for identifier, name, data in entries:
        directory.append(struct.pack("<III", identifier, offset, len(data)))
        payload.extend(data)
        offset += len(data)
        encoded = name.encode("ascii")
        names.extend(encoded + b"\0" * (15 - len(encoded)))
    path.write_bytes(
        struct.pack("<H", len(entries)) + b"".join(directory) + payload + names
    )


class AggManifestTest(unittest.TestCase):
    def test_manifest_hashes_payload_without_extracting_it(self):
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / "HEROES2.AGG"
            make_agg(path, [(0x12345678, "FONT.ICN", b"font"),
                            (0x87654321, "WINDOW.BIN", b"window")])
            result = agg_manifest.parse(path)
            self.assertEqual(result["entry_count"], 2)
            self.assertEqual(result["entries"][0]["name"], "FONT.ICN")
            self.assertEqual(
                result["entries"][0]["sha256"], hashlib.sha256(b"font").hexdigest()
            )

    def test_compare_reports_only_differences(self):
        with tempfile.TemporaryDirectory() as directory:
            directory = pathlib.Path(directory)
            primary_path = directory / "english.agg"
            locale_path = directory / "russian.agg"
            make_agg(primary_path, [(1, "SAME.ICN", b"same"),
                                    (2, "FONT.ICN", b"english")])
            make_agg(locale_path, [(1, "SAME.ICN", b"same"),
                                   (2, "FONT.ICN", b"russian"),
                                   (3, "DIALOG.BIN", b"localized")])
            result = agg_manifest.compare(
                agg_manifest.parse(primary_path), agg_manifest.parse(locale_path)
            )
            self.assertEqual(result["changed_entry_count"], 2)
            self.assertEqual(
                [(entry["id"], entry["status"]) for entry in result["changes"]],
                [("0x00000002", "changed"), ("0x00000003", "added")],
            )

    def test_rejects_entry_outside_payload_area(self):
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / "bad.agg"
            path.write_bytes(
                struct.pack("<HIII", 1, 1, 999, 4) + b"BAD.ICN\0\0\0\0\0\0\0\0"
            )
            with self.assertRaises(agg_manifest.AggError):
                agg_manifest.parse(path)


if __name__ == "__main__":
    unittest.main()
