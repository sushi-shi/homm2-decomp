"""The RES32 reader and the retail-comparison gate behind rc_res.

Built by hand rather than by running the era compiler: the defects these
tests exist for are record-layout misreads and a gate that passes on drift,
both reproducible offline.
"""
import struct
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from homm2.build import rc_res
from homm2.build.rc_res import compare, parse_res


def record(rtype, rname, language, data: bytes) -> bytes:
    def encode(value):
        if isinstance(value, int):
            return struct.pack("<HH", 0xFFFF, value)
        return value.encode("utf-16-le") + b"\0\0"

    header_tail = encode(rtype) + encode(rname)
    prefix = 8 + len(header_tail)
    pad = b"\0\0" if prefix % 4 else b""
    fixed = struct.pack("<IHHII", 0, 0x1030, language, 0, 0)
    header = struct.pack("<II", len(data), prefix + len(pad) + len(fixed))
    blob = header + header_tail + pad + fixed + data
    return blob + b"\0" * (-len(blob) % 4)


def null_header() -> bytes:
    return record(0, 0, 0, b"")


class ParseResTests(unittest.TestCase):
    def test_skips_the_null_header_and_reads_ordinals_names_and_payloads(self):
        blob = (null_header()
                + record(4, "MNUADV", 1049, b"\x01\x02\x03")
                + record(16, 1, 1049, b"\x04" * 5))
        rows = parse_res(blob)
        self.assertEqual(
            [(r["type"], r["name"], r["language"], r["data"]) for r in rows],
            [(4, "MNUADV", 1049, b"\x01\x02\x03"), (16, 1, 1049, b"\x04" * 5)],
        )

    def test_alignment_padding_between_records_is_consumed(self):
        blob = null_header() + record(3, 1, 1049, b"\x01") + record(3, 2, 1049, b"\x02")
        self.assertEqual([r["name"] for r in parse_res(blob)], [1, 2])


class CompareTests(unittest.TestCase):
    def rows(self):
        return [{"type": 4, "name": "MNUADV", "language": 1049, "data": b"abc"}]

    def test_exact_sets_produce_no_problems(self):
        self.assertEqual(compare(self.rows(), self.rows()), [])

    def test_byte_drift_is_reported_with_the_first_offset(self):
        theirs = [{**self.rows()[0], "data": b"abd"}]
        problems = compare(self.rows(), theirs)
        self.assertEqual(len(problems), 1)
        self.assertIn("bytes differ", problems[0])
        self.assertIn("0x2", problems[0])

    def test_identity_and_count_drift_are_reported(self):
        renamed = [{**self.rows()[0], "name": "MNUTOWN"}]
        self.assertIn("identity", compare(self.rows(), renamed)[0])
        self.assertIn("payload count", compare(self.rows(), self.rows() * 2)[0])

    def test_order_drift_is_reported_as_identity_mismatch(self):
        a = self.rows() + [{"type": 16, "name": 1, "language": 1049, "data": b"v"}]
        problems = compare(a, list(reversed(a)))
        self.assertTrue(problems and all("identity" in p for p in problems))


class ResourceCompilerStagingTests(unittest.TestCase):
    def test_icon_exists_only_in_temporary_compiler_directory(self):
        with tempfile.TemporaryDirectory() as temp_name:
            root = Path(temp_name)
            source = root / "res/HMM2PL.rc"
            output = root / "build/link/HMM2PL.res"
            retail_exe = root / "retail.exe"
            source.parent.mkdir(parents=True)
            source.write_text('HEROES ICON DISCARDABLE "heroes.ico"\n')
            retail_exe.write_bytes(b"retail")
            icon = b"icon"
            group = struct.pack("<HHH", 0, 1, 1) + struct.pack(
                "<BBBBHHIH", 32, 32, 16, 0, 1, 4, len(icon), 22
            )
            retail = [
                {"type": 3, "name": 22, "language": 1049, "data": icon},
                {"type": 14, "name": "HEROES", "language": 1049,
                 "data": group},
            ]
            staged = []

            def run_compiler(*_args, cwd, **_kwargs):
                stage = Path(cwd)
                staged.append(stage)
                self.assertNotEqual(stage, source.parent)
                self.assertEqual((stage / source.name).read_bytes(), source.read_bytes())
                self.assertTrue((stage / "heroes.ico").is_file())
                output.write_bytes(b"compiled-res")

            with (
                mock.patch.object(rc_res, "check_rc_binaries"),
                mock.patch.object(rc_res, "read_pe_resources", return_value=retail),
                mock.patch.object(rc_res, "parse_res", return_value=retail),
                mock.patch.object(rc_res.wine, "run", side_effect=run_compiler),
            ):
                self.assertEqual(
                    rc_res.main([
                        "--rc", str(source),
                        "--out", str(output),
                        "--verify-exe", str(retail_exe),
                    ]),
                    0,
                )

            self.assertEqual(len(staged), 1)
            self.assertFalse(staged[0].exists())
            self.assertFalse((source.parent / "heroes.ico").exists())
