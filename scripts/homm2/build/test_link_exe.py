import struct
import tempfile
import unittest
from pathlib import Path

from homm2.build.link_exe import (
    load_retail_order, normalized_vendor_imports, parse_map_contributions, parse_map_symbols,
    parse_unresolved, read_imports, read_order_response, read_pe)


class LinkExeTest(unittest.TestCase):
    def test_order_uses_numeric_nb09_contribution_not_manifest_or_public_order(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            (root / "units.toml").write_text(
                '[[unit]]\nunit="late"\nsource="late.cpp"\n'
                '[[unit]]\nunit="early"\nsource="early.cpp"\n')
            (root / "symbols.csv").write_text(
                "rva,name,unit,size,kind,provenance\n"
                "0x10000,late_fn,late,0x1,func,test\n"
                "0x900,early_fn,early,0x1,func,test\n")
            modules = {
                "late": [{"module": "late.obj", "contributions": [
                    {"section": 1, "offset": 0x1000, "size": 1, "rva": 0x2000}]}],
                "early": [{"module": "early.obj", "contributions": [
                    {"section": 1, "offset": 0x100, "size": 1, "rva": 0x1100}]}],
            }
            order = load_retail_order(root / "units.toml", root / "symbols.csv",
                                      module_contributions=modules)
            self.assertEqual([row["unit"] for row in order], ["early", "late"])

    def test_missing_order_evidence_is_rejected(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            (root / "units.toml").write_text('[[unit]]\nunit="missing"\nsource="x.cpp"\n')
            (root / "symbols.csv").write_text("rva,name,unit,size,kind,provenance\n")
            with self.assertRaisesRegex(ValueError, "no CodeView function RVA"):
                load_retail_order(root / "units.toml", root / "symbols.csv",
                                  module_contributions={})

    def test_unresolved_symbols_are_grouped_by_owner(self):
        parsed = parse_unresolved(
            "a.obj : error LNK2001: unresolved external symbol __imp__SmackOpen@12\n"
            "b.obj : error LNK2001: unresolved external symbol __imp___AIL_startup_0@0\n"
            "c.obj : error LNK2001: unresolved external symbol ?project@@YAXXZ\n")
        self.assertEqual(parsed["count"], 3)
        self.assertEqual(set(parsed["classes"]),
                         {"smackw32.dll", "mss32.dll", "project or runtime"})

    def test_map_parser_reads_public_symbol_absolute_address(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "test.map"
            path.write_text(" 0001:0000004e fn 0040104e f unit.obj\n")
            self.assertEqual(parse_map_symbols(path), {"fn": [0x40104E]})

    def test_map_contribution_parser_keeps_initialized_and_bss_rows(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "test.map"
            path.write_text(
                " 0003:00000040 00000100H .data DATA\n"
                " 0003:00000140 00000200H .bss DATA\n")
            rows = parse_map_contributions(path)
        self.assertEqual([(row["name"], row["size"]) for row in rows],
                         [(".data", 0x100), (".bss", 0x200)])

    def test_order_response_paths_are_relative_to_the_artifact(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            path = root / "link/objects.rsp"
            path.parent.mkdir()
            path.write_text('"..\\obj\\one.obj"\n')
            self.assertEqual(read_order_response(path), [(root / "obj/one.obj").resolve()])

    def test_pe_parser_reports_virtual_and_raw_section_sizes(self):
        data = bytearray(0x200)
        data[:2] = b"MZ"
        struct.pack_into("<I", data, 0x3C, 0x80)
        data[0x80:0x84] = b"PE\0\0"
        struct.pack_into("<H", data, 0x84 + 2, 1)
        struct.pack_into("<H", data, 0x84 + 16, 0xE0)
        optional = 0x84 + 20
        struct.pack_into("<HBB", data, optional, 0x10B, 4, 20)
        struct.pack_into("<I", data, optional + 16, 0x1234)
        struct.pack_into("<I", data, optional + 28, 0x400000)
        section = optional + 0xE0
        data[section:section + 8] = b".data\0\0\0"
        struct.pack_into("<III", data, section + 8, 0x90, 0x2000, 0x40)
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "test.exe"
            path.write_bytes(data)
            pe = read_pe(path)
        self.assertEqual(pe["linker_version"], "4.20")
        self.assertEqual(pe["entry_point_rva"], 0x1234)
        self.assertEqual(pe["sections"][".data"],
                         {"rva": 0x2000, "virtual_size": 0x90, "raw_size": 0x40})

    def test_import_parser_preserves_name_and_ordinal(self):
        data = bytearray(0x400)
        data[:2] = b"MZ"
        struct.pack_into("<I", data, 0x3C, 0x80)
        data[0x80:0x84] = b"PE\0\0"
        struct.pack_into("<H", data, 0x84 + 2, 1)
        struct.pack_into("<H", data, 0x84 + 16, 0xE0)
        optional = 0x84 + 20
        struct.pack_into("<H", data, optional, 0x10B)
        struct.pack_into("<II", data, optional + 96 + 8, 0x2000, 0x28)
        section = optional + 0xE0
        data[section:section + 8] = b".idata\0\0"
        struct.pack_into("<IIII", data, section + 8, 0x200, 0x2000, 0x200, 0x200)
        struct.pack_into("<IIIII", data, 0x200, 0x2040, 0, 0, 0x2080, 0x2060)
        struct.pack_into("<III", data, 0x240, 0x8000000E, 0x2090, 0)
        data[0x280:0x286] = b"x.dll\0"
        data[0x292:0x295] = b"Fn\0"
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "test.exe"
            path.write_bytes(data)
            imports = read_imports(path)
        self.assertEqual(imports, [{"dll": "x.dll", "symbols": [
            {"ordinal": 14}, {"name": "Fn", "hint": 0}]}])

    def test_vendor_abi_normalization_ignores_only_intra_dll_iat_order(self):
        imports = [{"dll": "mss32.dll", "symbols": [
            {"name": "_B@0", "hint": 2}, {"name": "_A@0", "hint": 1}]}]
        self.assertEqual(normalized_vendor_imports(imports), [{"dll": "mss32.dll", "symbols": [
            {"name": "_A@0", "hint": 1}, {"name": "_B@0", "hint": 2}]}])


if __name__ == "__main__":
    unittest.main()
