import struct
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

from homm2.build.assert_relocs import (
    _PARSE_CACHE,
    check_ordered_owner_offsets,
    check_ordered_reloc_addresses,
    parse_obj,
)
from homm2.build.reloc_owners import DataOwner, is_interior_reloc_alias, owners_from_rows


GCONFIG_SYMBOL = "?gConfig@@3UconfigStruct@@A"


class RelocOwnerTest(unittest.TestCase):
    def setUp(self):
        self.rows = [{
            "rva": "0x128d20",
            "name": GCONFIG_SYMBOL,
            "unit": "SOURCE/KB",
            "size": "0x1a0",
            "kind": "data",
            "provenance": "cv-public-data",
        }]

    def test_owner_requires_matching_data_definition(self):
        extent = {"gConfig": (0x128D20, 0x1A0)}
        self.assertEqual(owners_from_rows(self.rows, {}, extent), [])
        self.assertEqual(
            owners_from_rows(self.rows, {"gConfig": 0x128D20}, {}), [])
        self.assertEqual(
            owners_from_rows(self.rows, {"gConfig": 0x128D20}, extent),
            [DataOwner(0x128D20, 0x1A0, GCONFIG_SYMBOL, "gConfig")],
        )

    def test_synthetic_alias_inside_data_owner_is_canonicalized(self):
        owners = owners_from_rows(
            self.rows, {"gConfig": 0x128D20},
            {"gConfig": (0x128D20, 0x1A0)})
        alias = {
            "rva": "0x128d50", "name": "const_00128d50", "kind": "data",
            "provenance": "pe-reloc-constant",
        }
        self.assertTrue(is_interior_reloc_alias(alias, owners))


class OrderedRelocFieldTest(unittest.TestCase):
    def setUp(self):
        self.sym = {GCONFIG_SYMBOL: 0x128D20}
        self.owners = [DataOwner(0x128D20, 0x1A0, GCONFIG_SYMBOL, "gConfig")]

    def test_dd_init_graphics_wrong_field_regression(self):
        # Captured from retail/candidate DDInitGraphics at VA 0x4354fd.  Both
        # relocation fields are function-relative +0x5e; retail addresses
        # gConfig.fullScreen (+0x30), while the pre-fix source used showMenu (+0x1c).
        target = [(0x5E, "DIR32", "const_00128d50", 0)]
        base = [(0x5E, "DIR32", GCONFIG_SYMBOL, 0x1C)]
        problems = check_ordered_owner_offsets(
            self.sym, {}, self.owners, base, target)
        self.assertEqual(len(problems), 1)
        self.assertIn("+0x5e", problems[0].diagnostic())
        self.assertIn("gConfig expected +0x30, actual +0x1c",
                      problems[0].diagnostic())

    def test_legitimate_external_symbol_displacement_matches(self):
        target = [(0x20, "DIR32", "const_00128d50", 0)]
        base = [(0x20, "DIR32", GCONFIG_SYMBOL, 0x30)]
        self.assertEqual(check_ordered_owner_offsets(
            self.sym, {}, self.owners, base, target), [])

    def test_different_sites_are_not_assumed_to_correspond(self):
        target = [(0x24, "DIR32", "const_00128d50", 0)]
        base = [(0x20, "DIR32", GCONFIG_SYMBOL, 0x1C)]
        self.assertEqual(check_ordered_owner_offsets(
            self.sym, {}, self.owners, base, target), [])

    def test_resolved_address_gate_needs_no_owner_extent(self):
        other = "?otherGlobal@@3HA"
        sym = dict(self.sym, **{other: 0x130000})
        target = [(0x20, "DIR32", "const_00128d50", 0)]
        base = [(0x20, "DIR32", other, 0)]
        problems = check_ordered_reloc_addresses(
            sym, {}, {}, [], base, target)
        self.assertEqual(len(problems), 1)
        self.assertIn("expected RVA 0x128d50, actual RVA 0x130000",
                      problems[0].diagnostic())

    def test_resolved_address_gate_accepts_owner_plus_addend(self):
        target = [(0x20, "DIR32", "const_00128d50", 0)]
        base = [(0x20, "DIR32", GCONFIG_SYMBOL, 0x30)]
        self.assertEqual(check_ordered_reloc_addresses(
            self.sym, {}, {}, [], base, target), [])

    def test_unresolved_compiler_local_is_explicitly_out_of_scope(self):
        target = [(0x20, "DIR32", "const_00128d50", 0)]
        base = [(0x20, "DIR32", "$SG123", 0)]
        self.assertEqual(check_ordered_reloc_addresses(
            self.sym, {}, {}, [], base, target), [])


class CoffAddendTest(unittest.TestCase):
    def test_each_text_comdat_uses_its_own_implicit_addend(self):
        data = bytearray(0x8A)
        struct.pack_into("<HHIIIHH", data, 0, 0x14C, 2, 0, 0, 0, 0, 0)
        for index, raw in enumerate((0x80, 0x85)):
            section = 20 + index * 40
            data[section:section + 8] = b".text\0\0\0"
            struct.pack_into("<II", data, section + 16, 5, raw)
        data[0x80:0x85] = b"\xa1" + struct.pack("<I", 0x11)
        data[0x85:0x8A] = b"\xa1" + struct.pack("<I", 0x22)
        dump = """\
Disassembly of section .text:
00000000 <?first@@YIXXZ>:
       0: a1 11 00 00 00 movl 0x11, %eax
            00000001: IMAGE_REL_I386_DIR32 ?gConfig@@3UconfigStruct@@A
Disassembly of section .text:
00000000 <?second@@YIXXZ>:
       0: a1 22 00 00 00 movl 0x22, %eax
            00000001: IMAGE_REL_I386_DIR32 ?gConfig@@3UconfigStruct@@A
"""
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "two.obj"
            path.write_bytes(data)
            with patch("homm2.build.assert_relocs.subprocess.run",
                       return_value=SimpleNamespace(stdout=dump, returncode=0)):
                functions = parse_obj(str(path), with_sites=True)
        self.assertEqual(functions["?first@@YIXXZ"][0][3], 0x11)
        self.assertEqual(functions["?second@@YIXXZ"][0][3], 0x22)

    def test_unchanged_object_reuses_parsed_relocations(self):
        data = bytearray(0x45)
        struct.pack_into("<HHIIIHH", data, 0, 0x14C, 1, 0, 0, 0, 0, 0)
        data[20:28] = b".text\0\0\0"
        struct.pack_into("<II", data, 20 + 16, 5, 0x40)
        data[0x40:0x45] = b"\xa1" + struct.pack("<I", 0x30)
        dump = """\
Disassembly of section .text:
00000000 <?cached@@YIXXZ>:
       0: a1 30 00 00 00 movl 0x30, %eax
            00000001: IMAGE_REL_I386_DIR32 ?gConfig@@3UconfigStruct@@A
"""
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "cached.obj"
            path.write_bytes(data)
            _PARSE_CACHE.clear()
            with patch("homm2.build.assert_relocs.subprocess.run",
                       return_value=SimpleNamespace(stdout=dump, returncode=0)) as run:
                first = parse_obj(str(path), with_sites=True)
                second = parse_obj(str(path), with_sites=True)
        self.assertEqual(first, second)
        self.assertEqual(run.call_count, 1)


if __name__ == "__main__":
    unittest.main()
