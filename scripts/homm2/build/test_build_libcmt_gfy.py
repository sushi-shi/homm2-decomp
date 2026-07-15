import json
import struct
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from homm2.build.build_libcmt_gfy import (
    EXPECTED_RETAIL_LITERAL_COUNT, EXPECTED_TRANSFORMED_SECTIONS, MEMBERS,
    OUTPUT_ARCHIVE_LITERAL, PINNED_DERIVED_LIBCMT_SHA256,
    READONLY_LITERAL_CHARACTERISTICS, REPO, WRITABLE_LITERAL_CHARACTERISTICS,
    _prove_archive_object_identity, _storage_mismatch_keys, archive_entries,
    audit_link_ab, sha256, transform_archive, transform_object,
)


def coff_literal(name="??_C@_03TEST@test?$AA@"):
    payload = b"test\0"
    section_offset = 20
    raw_offset = section_offset + 40
    symbol_offset = raw_offset + len(payload)
    string_table = name.encode("ascii") + b"\0"
    data = bytearray(symbol_offset + 3 * 18 + 4 + len(string_table))
    struct.pack_into("<HHIIIHH", data, 0, 0x14C, 1, 0x12345678,
                     symbol_offset, 3, 0, 0)
    data[section_offset:section_offset + 8] = b".rdata\0\0"
    struct.pack_into("<I", data, section_offset + 16, len(payload))
    struct.pack_into("<I", data, section_offset + 20, raw_offset)
    struct.pack_into("<I", data, section_offset + 36,
                     READONLY_LITERAL_CHARACTERISTICS)
    data[raw_offset:raw_offset + len(payload)] = payload
    section_symbol = symbol_offset
    data[section_symbol:section_symbol + 8] = b".rdata\0\0"
    struct.pack_into("<h", data, section_symbol + 12, 1)
    data[section_symbol + 16] = 3
    data[section_symbol + 17] = 1
    literal_symbol = symbol_offset + 36
    struct.pack_into("<II", data, literal_symbol, 0, 4)
    struct.pack_into("<h", data, literal_symbol + 12, 1)
    data[literal_symbol + 16] = 2
    string_offset = symbol_offset + 54
    struct.pack_into("<I", data, string_offset, 4 + len(string_table))
    data[string_offset + 4:] = string_table
    return bytes(data)


def archive_member(name, payload):
    encoded_name = (name + "/").encode("ascii").ljust(16)
    header = (encoded_name + b"0".ljust(12) + b"0".ljust(6) +
              b"0".ljust(6) + b"100666".ljust(8) +
              str(len(payload)).encode("ascii").ljust(10) + b"`\n")
    return header + payload + (b"\n" if len(payload) & 1 else b"")


class LiteralTransformTest(unittest.TestCase):
    def test_changes_only_section_name_and_write_characteristic(self):
        original = coff_literal()
        transformed, evidence = transform_object(original, 1)

        expected = bytearray(original)
        expected[21:26] = b"data\0"
        struct.pack_into("<I", expected, 56, WRITABLE_LITERAL_CHARACTERISTICS)
        symbol_offset = struct.unpack_from("<I", expected, 8)[0]
        expected[symbol_offset + 1:symbol_offset + 6] = b"data\0"
        self.assertEqual(transformed, bytes(expected))
        self.assertEqual(evidence["literal_sections"], 1)
        self.assertEqual(evidence["changed_bytes"], 11)

    def test_rejects_non_readonly_literal_section(self):
        writable, _ = transform_object(coff_literal(), 1)

        with self.assertRaisesRegex(ValueError, "not the VC4.2 /GF COMDAT"):
            transform_object(writable, 1)

    def test_archive_parser_preserves_member_offsets_and_order(self):
        first = coff_literal()
        archive = b"!<arch>\n" + archive_member("first.obj", first) + archive_member(
            "second.obj", b"other")

        entries = archive_entries(archive)

        self.assertEqual([entry.name for entry in entries], ["first.obj", "second.obj"])
        self.assertEqual(archive[entries[0].data_offset:entries[0].data_end], first)
        self.assertEqual(archive[entries[1].data_offset:entries[1].data_end], b"other")

    def test_output_source_proof_allows_only_known_literal_decoration(self):
        archive = coff_literal(OUTPUT_ARCHIVE_LITERAL)
        rebuilt_name = list(OUTPUT_ARCHIVE_LITERAL)
        for offset, value in zip((109, 112, 113), "FDM"):
            rebuilt_name[offset] = value
        rebuilt_name = "".join(rebuilt_name)
        rebuilt = coff_literal(rebuilt_name)

        evidence = _prove_archive_object_identity("output.obj", rebuilt, archive)

        self.assertEqual(evidence["archive_identity"],
                         "compiler-local-literal-name-diff")
        self.assertEqual(evidence["differing_bytes"], 3)
        with self.assertRaisesRegex(ValueError, "exception changed"):
            _prove_archive_object_identity(
                "output.obj", coff_literal(rebuilt_name[:-1] + "X"), archive)

    def test_regression_cardinality_is_exact(self):
        self.assertEqual(EXPECTED_TRANSFORMED_SECTIONS, 107)
        self.assertEqual(EXPECTED_RETAIL_LITERAL_COUNT, 102)
        self.assertEqual(
            {name: spec.literal_sections for name, spec in MEMBERS.items()},
            {"aw_map.obj": 2, "crt0msg.obj": 23, "crtmbox.obj": 4,
             "output.obj": 2, "perror.obj": 2, "syserr.obj": 40,
             "cvt.obj": 1, "fpexcept.obj": 27, "testfdiv.obj": 2,
             "x10fout.obj": 4})

    def test_pinned_archive_transform_preserves_archive_shape(self):
        path = REPO / "build/toolchain/msvc/lib/LIBCMT.LIB"
        if not path.exists():
            self.skipTest("VC4.2 toolchain is not installed")
        original = path.read_bytes()
        transformed, members = transform_archive(original)

        self.assertEqual(sha256(transformed), PINNED_DERIVED_LIBCMT_SHA256)
        self.assertEqual(len(transformed), len(original))
        self.assertEqual(archive_entries(transformed), archive_entries(original))
        self.assertEqual(sum(row["literal_sections"] for row in members), 107)

    def test_storage_mismatch_set_uses_name_and_owner(self):
        report = {"static_storage": {"public_symbols": {"symbols": [
            {"name": "fixed", "unit": "crt", "candidate_count": 1,
             "storage_class_matches": False},
            {"name": "stable", "unit": "source", "candidate_count": 1,
             "storage_class_matches": True},
            {"name": "missing", "unit": "source", "candidate_count": 0,
             "storage_class_matches": False},
        ]}}}
        self.assertEqual(_storage_mismatch_keys(report), {("fixed", "crt")})

    def test_ab_gate_requires_exact_fix_set_and_normalized_text_identity(self):
        def report(mismatches):
            return {"static_storage": {"public_symbols": {"symbols": [
                {"name": name, "unit": unit, "candidate_count": 1,
                 "storage_class_matches": (name, unit) not in mismatches}
                for name, unit in (("literal", "crt"), ("stable", "source"))
            ]}}}

        evidence = {"size": 16, "normalized_sha256": "same",
                    "highlow_base_relocation_count": 3}
        with tempfile.TemporaryDirectory() as temporary_name:
            temporary = Path(temporary_name)
            baseline = temporary / "baseline.json"
            derived = temporary / "derived.json"
            baseline.write_text(json.dumps(report({("literal", "crt")})))
            derived.write_text(json.dumps(report(set())))
            with (mock.patch(
                    "homm2.build.build_libcmt_gfy.expected_retail_literals",
                    return_value={"literal": "crt"}),
                  mock.patch("homm2.build.link_exe.read_pe", return_value={
                      "sections": {".text": {"rva": 0x1000, "raw_size": 16}}}),
                  mock.patch("homm2.build.link_exe.read_pe_payload_evidence",
                             return_value=evidence)):
                audit = audit_link_ab(
                    baseline, derived, temporary / "a.exe", temporary / "b.exe")

        self.assertEqual(audit["fixed_storage_class_mismatches"], 1)
        self.assertEqual(audit["introduced_storage_class_mismatches"], 0)
        self.assertEqual(audit["text"]["normalized_sha256"], "same")
        self.assertEqual(audit["text"]["highlow_base_relocation_count"], 3)


if __name__ == "__main__":
    unittest.main()
