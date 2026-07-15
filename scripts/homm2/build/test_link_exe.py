import struct
import tempfile
import unittest
from pathlib import Path

from homm2.build.link_exe import (
    classify_missing_public_data, classify_pe_storage, decode_map_symbol_name,
    decode_s_compile_banner, load_retail_data_symbols, load_retail_order,
    normalized_vendor_imports, parse_map_contributions, parse_map_symbol_records,
    parse_map_symbols, parse_unresolved, read_coff_section, read_imports, read_order_response,
    read_pe, resolve_link_executable, static_symbol_diagnostics)


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

    def test_s_compile_banner_decodes_owned_tool_record(self):
        body = b"\x03\x07\x00\x08\x04LINK"
        self.assertEqual(decode_s_compile_banner(body), "LINK")

    def test_explicit_linker_override_is_validated_and_selected(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            linker = root / "LINK30.EXE"
            linker.write_bytes(b"MZ")
            selected, source = resolve_link_executable(root, linker)
            self.assertEqual(selected, linker.resolve())
            self.assertEqual(source, "argument")
            with self.assertRaisesRegex(RuntimeError, "override is not a file"):
                resolve_link_executable(root, root / "missing.exe")

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

    def test_map_symbol_record_preserves_storage_location_and_object(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "test.map"
            path.write_text(
                " 0001:0000004e fn 0040104e f unit.obj\n"
                " 0003:00000140 global 00403140 <common>\n")
            records = parse_map_symbol_records(path)
        self.assertEqual(records[0], {
            "segment": 1, "offset": 0x4E, "name": "fn", "va": 0x40104E,
            "flag": "f", "object": "unit.obj",
        })
        self.assertEqual(records[1], {
            "segment": 3, "offset": 0x140, "name": "global", "va": 0x403140,
            "flag": None, "object": "<common>",
        })

    def test_map_symbol_name_decodes_link_printable_octal_escape(self):
        self.assertEqual(decode_map_symbol_name(r"\177KERNEL32_NULL_THUNK_DATA"),
                         "\x7fKERNEL32_NULL_THUNK_DATA")

    def test_missing_runtime_literal_is_classified_as_non_actionable(self):
        result = classify_missing_public_data({
            "name": "??_C@_0L@ODMK@new_p?5?$DN?$DN?50?$AA@", "unit": "handler",
        }, {})
        self.assertEqual(result["root_cause"], "runtime-library-private-literal")
        self.assertFalse(result["actionable"])

    def test_legacy_import_descriptor_reports_candidate_equivalent(self):
        result = classify_missing_public_data({
            "name": "WING32_IMPORT_DESCRIPTOR", "unit": "wing32.def",
        }, {"__IMPORT_DESCRIPTOR_WING32": [{}]})
        self.assertEqual(result["root_cause"], "legacy-import-library-bookkeeping-name")
        self.assertEqual(result["candidate_equivalent"], "__IMPORT_DESCRIPTOR_WING32")

    def test_map_contribution_parser_keeps_initialized_and_bss_rows(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "test.map"
            path.write_text(
                " 0003:00000040 00000100H .data DATA\n"
                " 0003:00000140 00000200H .bss DATA\n")
            rows = parse_map_contributions(path)
        self.assertEqual([(row["name"], row["size"]) for row in rows],
                         [(".data", 0x100), (".bss", 0x200)])

    def test_retail_public_data_loader_excludes_synthetic_relocation_constants(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "symbols.csv"
            path.write_text(
                "rva,name,unit,size,kind,provenance\n"
                "0x3000,global,UNIT,0x4,data,cv-public-data\n"
                "0x2000,const,_const,0x0,data,pe-reloc-constant\n"
                "0x1000,fn,UNIT,0x10,func,cv-public-gap\n")
            symbols = load_retail_data_symbols(path)
        self.assertEqual(symbols, [{
            "name": "global", "unit": "UNIT", "rva": 0x3000, "size": 4,
            "provenance": "cv-public-data",
        }])

    def test_pe_storage_classifier_separates_initialized_and_loader_zero_data(self):
        pe = {"sections": {
            ".rdata": {"rva": 0x2000, "raw_size": 0x20, "virtual_size": 0x18},
            ".data": {"rva": 0x3000, "raw_size": 0x10, "virtual_size": 0x30},
        }}
        self.assertEqual(classify_pe_storage(pe, 0x2004)["class"], "rdata")
        self.assertEqual(classify_pe_storage(pe, 0x300C)["class"], "data-initialized")
        self.assertEqual(classify_pe_storage(pe, 0x3010)["class"],
                         "data-loader-zero-tail")

    def test_static_symbol_audit_correlates_classes_and_reports_missing_symbols(self):
        retail = {"image_base": 0x400000, "sections": {
            ".rdata": {"rva": 0x2000, "raw_size": 0x20, "virtual_size": 0x20},
            ".data": {"rva": 0x3000, "raw_size": 0x10, "virtual_size": 0x30},
        }}
        candidate = {"image_base": 0x400000, "sections": {
            ".rdata": {"rva": 0x2000, "raw_size": 0x20, "virtual_size": 0x20},
            ".data": {"rva": 0x4000, "raw_size": 0x20, "virtual_size": 0x40},
        }}
        symbols = [
            {"name": "initialized", "unit": "UNIT", "rva": 0x3000, "size": 4},
            {"name": "zeroed", "unit": "UNIT", "rva": 0x3010, "size": 4},
            {"name": "missing", "unit": "UNIT", "rva": 0x3014, "size": 4},
        ]
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "test.map"
            path.write_text(
                " 0003:00000000 00000020H .data DATA\n"
                " 0003:00000020 00000020H .bss DATA\n"
                " 0003:00000000 initialized 00404000 unit.obj\n"
                " 0003:00000020 zeroed 00404020 <common>\n")
            audit = static_symbol_diagnostics(retail, candidate, path, symbols)
        summary = audit["summary"]
        self.assertEqual(summary["candidate_unique_name_matches"], 2)
        self.assertEqual(summary["candidate_missing"], 1)
        self.assertEqual(summary["storage_class_matches"], 2)
        self.assertEqual(summary["storage_class_mismatches"], 0)
        self.assertEqual(audit["symbols"][1]["candidate_storage"]["class"],
                         "data-loader-zero")

    def test_static_symbol_audit_reports_only_start_of_constant_displacement_run(self):
        retail = {"image_base": 0x400000, "sections": {
            ".rdata": {"rva": 0x2000, "raw_size": 0x40, "virtual_size": 0x40},
        }}
        candidate = {"image_base": 0x400000, "sections": {
            ".rdata": {"rva": 0x2000, "raw_size": 0x80, "virtual_size": 0x80},
        }}
        symbols = [
            {"name": "a", "unit": "UNIT", "rva": 0x2000, "size": 4},
            {"name": "b", "unit": "UNIT", "rva": 0x2004, "size": 4},
            {"name": "c", "unit": "UNIT", "rva": 0x2008, "size": 4},
        ]
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "test.map"
            path.write_text(
                " 0002:00000000 00000080H .rdata DATA\n"
                " 0002:00000010 a 00402010 unit.obj\n"
                " 0002:00000014 b 00402014 unit.obj\n"
                " 0002:0000001c c 0040201c unit.obj\n")
            audit = static_symbol_diagnostics(retail, candidate, path, symbols)
        self.assertEqual(audit["summary"]["constant_displacement_runs"], 2)
        self.assertEqual(audit["displacement_runs"][0]["count"], 2)
        self.assertEqual([row["name"] for row in audit["first_divergences"]], ["a", "c"])

    def test_coff_section_parser_reports_raw_and_alignment_rounded_size(self):
        data = bytearray(0x100)
        struct.pack_into("<HHIIIHH", data, 0, 0x14C, 1, 0, 0, 0, 0, 0)
        section = 20
        data[section:section + 8] = b".text\0\0\0"
        struct.pack_into("<I", data, section + 16, 0x123)
        struct.pack_into("<I", data, section + 36, 0x00500020)
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "test.obj"
            path.write_bytes(data)
            parsed = read_coff_section(path, ".text")
        self.assertEqual(parsed, {"raw_size": 0x123, "alignment": 0x10,
                                  "aligned_size": 0x130})

    def test_static_symbol_audit_separates_section_rva_and_contribution_drift(self):
        retail = {"image_base": 0x400000, "sections": {
            ".rdata": {"rva": 0x2000, "raw_size": 0x40, "virtual_size": 0x40},
        }}
        candidate = {"image_base": 0x400000, "sections": {
            ".rdata": {"rva": 0x3000, "raw_size": 0x80, "virtual_size": 0x80},
        }}
        symbols = [
            {"name": "same_offset", "unit": "ONE", "rva": 0x2004, "size": 4},
            {"name": "later_offset", "unit": "TWO", "rva": 0x2008, "size": 4},
        ]
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "test.map"
            path.write_text(
                " 0002:00000000 00000080H .rdata DATA\n"
                " 0002:00000004 same_offset 00403004 one.obj\n"
                " 0002:00000010 later_offset 00403010 two.obj\n")
            audit = static_symbol_diagnostics(retail, candidate, path, symbols)
        self.assertEqual(audit["symbols"][0]["delta"], 0x1000)
        self.assertEqual(audit["symbols"][0]["section_relative_delta"], 0)
        self.assertEqual(audit["symbols"][1]["section_relative_delta"], 8)
        self.assertEqual(audit["first_section_relative_divergences"][0]["name"],
                         "later_offset")

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
                         {"rva": 0x2000, "virtual_size": 0x90, "raw_size": 0x40,
                          "characteristics": 0})
        self.assertEqual(pe["section_order"], [".data"])
        self.assertEqual(pe["resource_directory"], {"rva": 0, "size": 0})

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
