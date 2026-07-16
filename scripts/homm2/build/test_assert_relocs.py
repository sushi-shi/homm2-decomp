import struct
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

from homm2.build.assert_relocs import (
    _PARSE_CACHE,
    check_owner_offset_multisets,
    check_ordered_owner_offsets,
    check_ordered_reloc_addresses,
    compare_function_reloc_addends,
    load_canonical_data_names,
    parse_obj,
    relocation_addend_map,
)
from homm2.build.reloc_owners import DataOwner, is_interior_reloc_alias, owners_from_rows


GCONFIG_SYMBOL = "?gConfig@@3UconfigStruct@@A"
GMONSTER_DATABASE_SYMBOL = "?gMonsterDatabase@@3PAUtag_monsterInfo@@A"


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

    def test_process_random_objects_wrong_monster_field_regression(self):
        # Captured from ProcessRandomObjects. Retail reads randomValue at +2;
        # the bad anonymous union made candidate source read cost at +0.
        monster_rva = 0xFAEB0
        sym = {GMONSTER_DATABASE_SYMBOL: monster_rva}
        owners = [DataOwner(
            monster_rva, 0x6B8, GMONSTER_DATABASE_SYMBOL,
            "gMonsterDatabase")]
        target = [(0x2C2, "DIR32", "const_000faeb2", 0)]
        base = [(0x2C2, "DIR32", GMONSTER_DATABASE_SYMBOL, 0)]
        problems = check_ordered_owner_offsets(
            sym, {}, owners, base, target)
        self.assertEqual(len(problems), 1)
        self.assertIn("gMonsterDatabase expected +0x2, actual +0x0",
                      problems[0].diagnostic())

    def test_owner_multiset_catches_wrong_field_at_any_order(self):
        target = [
            (0x10, "DIR32", GCONFIG_SYMBOL, 0x4),
            (0x20, "DIR32", GCONFIG_SYMBOL, 0x78),
        ]
        reordered = [
            (0x10, "DIR32", GCONFIG_SYMBOL, 0x78),
            (0x20, "DIR32", GCONFIG_SYMBOL, 0x4),
        ]
        self.assertEqual(check_owner_offset_multisets(
            self.sym, {}, self.owners, reordered, target), [])

        wrong = [
            (0x10, "DIR32", GCONFIG_SYMBOL, 0x78),
            (0x20, "DIR32", GCONFIG_SYMBOL, 0x1C),
        ]
        problems = check_owner_offset_multisets(
            self.sym, {}, self.owners, wrong, target)
        self.assertEqual(len(problems), 1)
        self.assertIn("WRONG OWNER OFFSETS: gConfig", problems[0].diagnostic())

    def test_owner_multiset_defers_different_reference_counts(self):
        target = [(0x10, "DIR32", GCONFIG_SYMBOL, 0x4)]
        base = [
            (0x10, "DIR32", GCONFIG_SYMBOL, 0x4),
            (0x20, "DIR32", GCONFIG_SYMBOL, 0x78),
        ]
        self.assertEqual(check_owner_offset_multisets(
            self.sym, {}, self.owners, base, target), [])

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
    def test_addend_map_is_grouped_by_raw_name_and_sorted(self):
        relocations = [
            ("DIR32", "?owner@@3HA", 0x78),
            ("REL32", "?callee@@YIXXZ", 0),
            ("DIR32", "?owner@@3HA", 0x1C),
        ]
        self.assertEqual(relocation_addend_map(relocations), {
            "?callee@@YIXXZ": (0,),
            "?owner@@3HA": (0x1C, 0x78),
        })

    def test_addend_comparison_catches_wrong_field_without_sites(self):
        target = [
            ("DIR32", GCONFIG_SYMBOL, 0x30),
            ("DIR32", GCONFIG_SYMBOL, 0x78),
        ]
        base = [
            ("DIR32", GCONFIG_SYMBOL, 0x78),
            ("DIR32", GCONFIG_SYMBOL, 0x1C),
        ]
        differences = compare_function_reloc_addends(base, target)
        self.assertEqual(len(differences), 1)
        self.assertEqual(differences[0]["missing"], (0x30,))
        self.assertEqual(differences[0]["excess"], (0x1C,))
        self.assertEqual(differences[0]["classification"], "value-set")

    def test_addend_comparison_catches_missing_relocation_name(self):
        differences = compare_function_reloc_addends(
            [], [("REL32", "?callee@@YIXXZ", 0)],
            canonical_data_names={"?callee@@YIXXZ"})
        self.assertEqual(differences[0]["symbol"], "?callee@@YIXXZ")
        self.assertEqual(differences[0]["missing"], (0,))
        self.assertEqual(differences[0]["excess"], ())
        self.assertEqual(differences[0]["classification"], "one-sided")
        self.assertTrue(differences[0]["canonical_data"])

    def test_canonical_data_inventory_combines_public_and_manifest_names(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            symbols = root / "symbols.csv"
            manifest = root / "manifest.tsv"
            symbols.write_text(
                "rva,name,unit,size,kind\n"
                "0x100,public_data,A,0x4,data\n"
                "0x200,public_function,A,0x10,func\n")
            manifest.write_text(
                "name\tobject\trva\tsize\tstorage\n"
                "private_data\tA.c\t0x300\t0x4\tdata\n")
            self.assertEqual(load_canonical_data_names(symbols, manifest), {
                "public_data", "private_data",
            })

    def test_addend_comparison_separates_count_only_difference(self):
        differences = compare_function_reloc_addends(
            [("DIR32", GCONFIG_SYMBOL, 0x30)],
            [("DIR32", GCONFIG_SYMBOL, 0x30),
             ("DIR32", GCONFIG_SYMBOL, 0x30)])
        self.assertEqual(differences[0]["classification"], "count-only")

    def test_addend_comparison_separates_function_local_difference(self):
        function = "?Switch@@YIHH@Z"
        differences = compare_function_reloc_addends(
            [("DIR32", function, 0)],
            [("DIR32", function, 0x20)],
            function)
        self.assertEqual(differences[0]["classification"], "code-local")

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
