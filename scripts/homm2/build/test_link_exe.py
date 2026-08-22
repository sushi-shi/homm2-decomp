import hashlib
import struct
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from homm2.build.link_exe import (
    RETAIL_LINK_FLAGS, SYSTEM_LIBS_AFTER_VENDOR,
    SYSTEM_LIBS_BEFORE_VENDOR, build_link_command,
    classify_missing_public_data,
    classify_pe_storage, decode_map_symbol_name,
    compare_pe_section_bytes, compare_pe_section_range, load_required_initialized_storage,
    function_placement_diagnostics, import_diagnostics,
    load_claimed_data_symbols, load_link_order,
    link_environment, normalized_dll_import, normalized_vendor_imports,
    parse_map_contributions, parse_map_symbol_records,
    parse_map_symbols, parse_unresolved, read_coff_section, read_imports, read_order_response,
    read_pe, required_initialized_storage_diagnostics, resolve_link_executable,
    sibling_tool_identities, static_symbol_diagnostics,
    _import_slot_identities, _pair_volatile_function_rows,
    _shifted_rel32_diagnostics)
from homm2.build.link_exe import strip_coff_export_directives, write_order_response


class LinkExeTest(unittest.TestCase):
    def test_final_link_copy_strips_only_export_directives(self):
        payload = bytearray(20 + 40 + 64)
        struct.pack_into("<HHIIIHH", payload, 0, 0x14C, 1, 0, 0, 0, 0, 0)
        payload[20:28] = b".drectve"
        struct.pack_into("<II", payload, 20 + 16, 64, 60)
        directives = b"-defaultlib:LIBCMT -export:?body@@YAXXZ /export:other"
        payload[60:60 + len(directives)] = directives
        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory) / "in.obj"
            output = Path(directory) / "out.obj"
            source.write_bytes(payload)
            self.assertTrue(strip_coff_export_directives(source, output))
            copied = output.read_bytes()[60:124]
        self.assertIn(b"-defaultlib:LIBCMT", copied)
        self.assertNotIn(b"export:", copied.lower())
    def test_link_command_does_not_force_resolution_history(self):
        objects = [r"Z:\\obj\\one.obj", r"Z:\\obj\\two.obj"]
        vendors = [r"Z:\\lib\\smack.lib", r"Z:\\lib\\mss.lib", r"Z:\\lib\\wing.lib"]
        command = build_link_command(
            "LINK.EXE", r"Z:\\out\\game.map", r"Z:\\out\\game.exe",
            objects, vendors, r"Z:\\out\\game.res")
        self.assertFalse(any(argument.startswith("/INCLUDE:") for argument in command))
        self.assertLess(command.index(SYSTEM_LIBS_BEFORE_VENDOR[0]), command.index(vendors[0]))
        self.assertLess(command.index(vendors[-1]), command.index(SYSTEM_LIBS_AFTER_VENDOR[0]))
        self.assertLess(command.index(SYSTEM_LIBS_AFTER_VENDOR[-1]), command.index(objects[0]))
        self.assertLess(command.index(objects[-1]), command.index(r"Z:\\out\\game.res"))

    def test_dll_import_normalization_detects_wrong_runtime_symbol(self):
        imports = [{"dll": "ADVAPI32.dll", "symbols": [
            {"name": "RegQueryValueExA", "hint": 225},
            {"name": "RegCreateKeyA", "hint": 197},
        ]}]
        self.assertEqual(normalized_dll_import(imports, "advapi32.DLL"), {
            "dll": "advapi32.dll",
            "symbols": [
                {"name": "RegCreateKeyA", "hint": 197},
                {"name": "RegQueryValueExA", "hint": 225},
            ],
        })
        self.assertIsNone(normalized_dll_import(imports, "missing.dll"))

    def test_complete_import_diagnostics_separates_abi_from_iat_order(self):
        retail = [{"dll": "x.dll", "symbols": [
            {"name": "A", "hint": 1}, {"name": "B", "hint": 2}]}]
        candidate = [{"dll": "X.DLL", "symbols": [
            {"name": "B", "hint": 2}, {"name": "A", "hint": 1}]}]
        result = import_diagnostics(retail, candidate)
        self.assertTrue(result["dll_order_matches_retail"])
        self.assertTrue(result["complete_abi_matches_retail"])
        self.assertFalse(result["complete_iat_order_matches_retail"])
        self.assertEqual(result["iat_order_classification"], "resolution-history-wall")
        self.assertFalse(result["per_dll"][0]["iat_order_matches_retail"])

    def test_link_environment_uses_toolchain_lib_search_path(self):
        environment = link_environment(
            r"Z:\msvc\lib", "/msvc/bin", {"PATH": "tools", "LIB": "stale"})
        self.assertEqual(environment, {
            "PATH": "/msvc/bin" + __import__("os").pathsep + "tools",
            "LIB": r"Z:\msvc\lib",
        })

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

    def test_sibling_tool_identities_records_only_present_tools(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            linker = root / "LINK.EXE"
            linker.write_bytes(b"link")
            (root / "CVTRES.EXE").write_bytes(b"pack")
            identities = sibling_tool_identities(linker)
        self.assertEqual(set(identities), {"CVTRES.EXE"})
        self.assertEqual(
            identities["CVTRES.EXE"]["sha256"],
            "4862f447f2c7f272fa2f4aaf89dadb3b1ac09105bd5864f8d1a0c9452bb0a226",
        )

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

    def test_function_placement_audit_collapses_duplicate_map_rows(self):
        candidate = {"image_base": 0x400000}
        symbols = [
            {"name": "exact", "unit": "A", "rva": 0x1000, "size": 4,
             "provenance": "source-annotation"},
            {"name": "moved", "unit": "A", "rva": 0x1020, "size": 4,
             "provenance": "source-annotation"},
            {"name": "generated", "unit": "B", "rva": 0x1040, "size": 4,
             "provenance": "source-VA_COMPGEN"},
        ]
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "test.map"
            path.write_text(
                " 0001:00000000 exact 00401000 f A.obj\n"
                " 0001:00000000 exact 00401000 f A.obj\n"
                " 0001:00000024 moved 00401024 f A.obj\n")
            result = function_placement_diagnostics(candidate, path, symbols, {})
        self.assertEqual(result["summary"], {
            "total": 3, "exact_rva": 1, "displaced_rva": 1,
            "missing": 1, "ambiguous": 0,
        })
        self.assertEqual(result["source_summary"], result["summary"])
        self.assertEqual(result["project_summary"], result["summary"])
        self.assertEqual(result["residuals"][0]["delta"], 4)
        self.assertEqual(result["by_provenance"]["source-VA_COMPGEN"]["missing"], 1)

    def test_function_placement_uses_semantic_compgen_alias_in_own_object(self):
        candidate = {"image_base": 0x400000}
        symbols = [{
            "name": "__h2cg$SOURCE$A$static_ctor$Thing", "unit": "SOURCE/A",
            "rva": 0x1234, "size": 0x0F,
            "provenance": "source-VA_COMPGEN:STATIC_CTOR",
        }]
        aliases = {("SOURCE/A", symbols[0]["name"]): "_$E14"}
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "test.map"
            path.write_text(
                " 0001:00001234 _$E14 00401234 f A.obj\n"
                " 0001:00002234 _$E14 00402234 f OTHER.obj\n")
            result = function_placement_diagnostics(
                candidate, path, symbols, aliases)
        self.assertEqual(result["source_summary"]["exact_rva"], 1)
        self.assertEqual(result["functions"][0]["candidate_name"], "_$E14")
        self.assertEqual(result["functions"][0]["match_evidence"],
                         "semantic-compgen-sidecar")

    def test_function_placement_uses_semantic_volatile_alias_in_own_object(self):
        candidate = {"image_base": 0x400000}
        symbols = [{
            "name": "_$E14", "unit": "SOURCE/A", "rva": 0x1234,
            "size": 0x0F, "provenance": "reviewed-compgen",
        }]
        aliases = {("SOURCE/A", "_$E14"): "_$E91"}
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "test.map"
            path.write_text(
                " 0001:00001234 _$E91 00401234 f A.obj\n"
                " 0001:00002234 _$E91 00402234 f OTHER.obj\n")
            result = function_placement_diagnostics(
                candidate, path, symbols, {}, aliases)
        self.assertEqual(result["summary"]["exact_rva"], 1)
        self.assertEqual(result["functions"][0]["candidate_name"], "_$E91")
        self.assertEqual(result["functions"][0]["match_evidence"],
                         "normalized-volatile-unit-order")

    def test_volatile_function_pairing_requires_every_signature(self):
        retail = [
            {"name": "_$E1", "signature": (b"one", ((1, 6),))},
            {"name": "_$E2", "signature": (b"two", ())},
        ]
        candidate = [
            {"name": "_$E8", "signature": (b"one", ((1, 6),))},
            {"name": "_$E9", "signature": (b"two", ())},
        ]
        self.assertEqual(_pair_volatile_function_rows(retail, candidate), {
            "_$E1": "_$E8", "_$E2": "_$E9",
        })
        candidate[1]["signature"] = (b"different", ())
        self.assertEqual(_pair_volatile_function_rows(retail, candidate), {})

    def test_shifted_rel32_diagnostics_preserves_absolute_target(self):
        target = 0x2000
        candidate_va = 0x1000
        retail_va = 0x1100

        def body(base):
            site_after_operand = base + 1 + 5
            return (b"\x90\xe8" + struct.pack("<i", target - site_after_operand) +
                    b"\xc3")

        result = _shifted_rel32_diagnostics(
            body(retail_va), body(candidate_va), retail_va, candidate_va)
        self.assertTrue(result["relocation_masked_bytes_exact"])
        self.assertEqual(result["mismatch_runs"], 1)
        self.assertEqual(result["classified_rel32_fields"], 1)
        self.assertEqual(result["same_absolute_target_fields"], 1)
        self.assertEqual(result["different_absolute_targets"], [])

    def test_shifted_rel32_diagnostics_rejects_an_opcode_difference(self):
        result = _shifted_rel32_diagnostics(b"\x90", b"\x91", 0x1100, 0x1000)
        self.assertFalse(result["relocation_masked_bytes_exact"])
        self.assertEqual(result["unclassified_runs"], [{
            "offset": 0, "size": 1, "candidate_fields": [],
        }])

    def test_function_placement_uses_proved_shifted_funclet_band(self):
        candidate = {"image_base": 0x400000}
        symbol = {
            "name": "Unwind@00401234", "unit": "(funclets)",
            "rva": 0x1234, "size": 0x0B, "provenance": "reviewed-funclet",
        }
        placements = {((symbol["unit"], symbol["name"], symbol["rva"])): 0x1224}
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "test.map"
            path.write_text("")
            result = function_placement_diagnostics(
                candidate, path, [symbol], {}, {}, placements)
        self.assertEqual(result["summary"]["displaced_rva"], 1)
        self.assertEqual(result["functions"][0]["delta"], -0x10)
        self.assertEqual(result["functions"][0]["match_evidence"],
                         "shifted-text-x-rel32-proof")

    def test_function_placement_uses_semantic_import_thunk(self):
        candidate = {"image_base": 0x400000}
        symbol = {
            "name": "RtlUnwind@KERNEL32", "unit": "(imports)",
            "rva": 0x1234, "size": 6, "provenance": "reviewed-thunk",
        }
        placements = {(symbol["unit"], symbol["name"], symbol["rva"]): 0x1298}
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "test.map"
            path.write_text("")
            result = function_placement_diagnostics(
                candidate, path, [symbol], {}, {}, {}, placements)
        self.assertEqual(result["summary"]["displaced_rva"], 1)
        self.assertEqual(result["functions"][0]["delta"], 0x64)
        self.assertEqual(result["functions"][0]["match_evidence"],
                         "semantic-import-iat-target")

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

    def test_claimed_data_loader_excludes_synthetic_relocation_constants(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "symbols.csv"
            path.write_text(
                "rva,name,unit,size,kind,provenance\n"
                "0x3000,global,UNIT,0x4,data,source-annotation\n"
                "0x2000,const,_const,0x0,data,pe-reloc-constant\n"
                "0x1000,fn,UNIT,0x10,func,source-annotation\n")
            symbols = load_claimed_data_symbols(path)
        self.assertEqual(symbols, [{
            "name": "global", "unit": "UNIT", "rva": 0x3000, "size": 4,
            "provenance": "source-annotation",
        }])

    def test_link_order_is_the_manifest_order_with_source_anchors(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            units = root / "units.toml"
            units.write_text(
                '[[unit]]\nunit = "SOURCE/B"\nsource = "src/SOURCE/B.cpp"\n'
                '[[unit]]\nunit = "SOURCE/A"\nsource = "src/SOURCE/A.cpp"\n')
            symbols = root / "symbols.csv"
            symbols.write_text(
                "rva,name,unit,size,kind,provenance\n"
                "0x2000,?B2@@YIXXZ,SOURCE/B,0x10,func,source-annotation\n"
                "0x1000,?B1@@YIXXZ,SOURCE/B,0x10,func,source-annotation\n"
                "0x3000,?gD@@3HA,SOURCE/A,0x4,data,source-annotation\n")
            order = load_link_order(units, symbols)
        self.assertEqual([row["unit"] for row in order],
                         ["SOURCE/B", "SOURCE/A"])
        self.assertEqual(order[0]["first_function_rva"], 0x1000)
        self.assertEqual(order[0]["first_function_symbol"], "?B1@@YIXXZ")
        self.assertEqual(order[0]["order_evidence"], "units-manifest")
        self.assertIsNone(order[1]["first_function_rva"])

    def test_write_order_response_reports_anchor_order_violations(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            order = [
                {"unit": "SOURCE/B", "object": root / "obj/B.obj",
                 "first_function_rva": 0x2000},
                {"unit": "SOURCE/A", "object": root / "obj/A.obj",
                 "first_function_rva": 0x1000},
            ]
            response = root / "link/objects.rsp"
            with mock.patch("homm2.build.link_exe.load_link_order",
                            return_value=order):
                with mock.patch("builtins.print") as printer:
                    self.assertEqual(write_order_response(response), 0)
            self.assertEqual(read_order_response(response),
                             [(root / "obj/B.obj").resolve(),
                              (root / "obj/A.obj").resolve()])
        message = printer.call_args[0][0]
        self.assertIn("2 manifest units", message)
        self.assertIn("1 order violations", message)

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

    def test_static_symbol_audit_treats_corroborated_raw_zero_tail_as_bss(self):
        retail = {"image_base": 0x400000, "sections": {
            ".data": {"rva": 0x3000, "raw_size": 0x20, "virtual_size": 0x40},
        }}
        candidate = {"image_base": 0x400000, "sections": {
            ".data": {"rva": 0x4000, "raw_size": 0x10, "virtual_size": 0x40},
        }}
        symbols = [{"name": "zeroed", "unit": "UNIT", "rva": 0x3018, "size": 0x10}]
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "test.map"
            path.write_text(
                " 0002:00000000 00000010H .data DATA\n"
                " 0002:00000010 00000030H .bss DATA\n"
                " 0002:00000010 zeroed 00404010 <common>\n")
            audit = static_symbol_diagnostics(
                retail, candidate, path, symbols,
                retail_data_zero_tail_start=0x3014)
        row = audit["symbols"][0]
        self.assertEqual(row["retail_storage"]["class"],
                         "data-loader-zero-padding")
        self.assertTrue(row["storage_class_matches"])
        self.assertEqual(audit["summary"]["storage_class_mismatches"], 0)

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

    def test_required_initialized_storage_rejects_loader_zero_regression(self):
        public_symbols = {"symbols": [
            {
                "name": "recovered", "unit": "SOURCE/KB", "candidate_count": 1,
                "status": "storage-class-mismatch", "storage_class_matches": False,
                "retail_storage": {"class": "data-initialized"},
                "candidate_storage": {"class": "data-loader-zero"},
            },
            {
                "name": "pending", "unit": "SOURCE/KB", "candidate_count": 1,
                "status": "storage-class-mismatch", "storage_class_matches": False,
                "retail_storage": {"class": "data-initialized"},
                "candidate_storage": {"class": "data-loader-zero"},
            },
        ]}
        diagnostics = required_initialized_storage_diagnostics(
            public_symbols, [{
                "name": "recovered", "unit": "SOURCE/KB", "size": 4,
                "retail_sha256": "retail", "highlow_count": 0, "audit": "bytes",
            }])
        self.assertEqual(diagnostics["verified"], 0)
        self.assertEqual(diagnostics["violations"][0]["status"],
                         "storage-class-mismatch")
        self.assertNotIn("pending", [row["name"] for row in diagnostics["symbols"]])

    def test_required_initialized_storage_loader_skips_comments(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "required.tsv"
            path.write_text(
                "# reviewed recovery enrollment\n"
                "name\tunit\tsize\tretail_sha256\thighlow_count\taudit\n"
                "?global@@3HA\tSOURCE/KB\t0x4\tdeadbeef\t1\tbytes\n")
            required = load_required_initialized_storage(path)
        self.assertEqual(required, [{
            "name": "?global@@3HA", "unit": "SOURCE/KB", "size": 4,
            "retail_sha256": "deadbeef", "highlow_count": 1, "audit": "bytes",
        }])

    def test_required_initialized_storage_loader_parses_readable_span(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "required.tsv"
            path.write_text(
                "name\tunit\tsize\tretail_sha256\thighlow_count\taudit\t"
                "readable_size\treadable_sha256\n"
                "global\tUNIT\t0x10\tallocation\t0\tbytes\t0x12\treadable\n")
            required = load_required_initialized_storage(path)
        self.assertEqual(required, [{
            "name": "global", "unit": "UNIT", "size": 0x10,
            "retail_sha256": "allocation", "highlow_count": 0, "audit": "bytes",
            "readable_size": 0x12, "readable_sha256": "readable",
        }])

    def test_required_initialized_storage_loader_rejects_partial_readable_span(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "required.tsv"
            path.write_text(
                "name\tunit\tsize\tretail_sha256\thighlow_count\taudit\t"
                "readable_size\treadable_sha256\n"
                "global\tUNIT\t0x10\tallocation\t0\tbytes\t0x12\t\n")
            with self.assertRaisesRegex(RuntimeError, "must be provided together"):
                load_required_initialized_storage(path)

    def test_required_initialized_storage_compares_candidate_payload_and_targets(self):
        retail_payload = {
            "size": 4, "sha256": "retail", "highlow_base_relocation_count": 1,
            "highlow_relative_offsets": [0], "normalized_sha256": "normalized",
            "cstring_targets": [{"sha256": "target", "text": "name"}],
            "pointer_target_alias_pattern": [0],
        }
        candidate_payload = dict(retail_payload)
        public_symbols = {"symbols": [{
            "name": "table", "unit": "SOURCE/KB", "candidate_count": 1,
            "status": "displaced", "storage_class_matches": True,
            "retail_storage": {"class": "data-initialized"},
            "candidate_storage": {"class": "data-initialized"},
            "retail_payload": retail_payload, "candidate_payload": candidate_payload,
        }]}
        required = [{
            "name": "table", "unit": "SOURCE/KB", "size": 4,
            "retail_sha256": "retail", "highlow_count": 1,
            "audit": "cstring-pointer-table",
        }]
        diagnostics = required_initialized_storage_diagnostics(public_symbols, required)
        self.assertEqual(diagnostics["verified"], 1)
        candidate_payload["normalized_sha256"] = "changed"
        diagnostics = required_initialized_storage_diagnostics(public_symbols, required)
        self.assertEqual(diagnostics["violations"][0]["status"],
                         "normalized-payload-mismatch")

    def test_required_initialized_storage_rejects_wrong_source_extent(self):
        payload = {
            "size": 18, "sha256": "retail", "highlow_base_relocation_count": 0,
            "highlow_relative_offsets": [], "normalized_sha256": "same",
        }
        public_symbols = {"symbols": [{
            "name": "global", "unit": "UNIT", "retail_rva": "0x3000",
            "candidate_count": 1, "status": "exact", "storage_class_matches": True,
            "retail_storage": {"class": "data-initialized"},
            "candidate_storage": {"class": "data-initialized"},
            "retail_payload": payload, "candidate_payload": dict(payload),
        }]}
        required = [{
            "name": "global", "unit": "UNIT", "size": 18,
            "retail_sha256": "retail", "highlow_count": 0, "audit": "bytes",
        }]
        diagnostics = required_initialized_storage_diagnostics(
            public_symbols, required, {("UNIT", 0x3000): 16})
        self.assertEqual(diagnostics["violations"][0]["status"],
                         "source-extent-mismatch")
        self.assertEqual(diagnostics["violations"][0]["source_definition_size"], 16)

    def test_required_initialized_storage_checks_readable_span_separately(self):
        allocation = {
            "size": 16, "sha256": "allocation", "highlow_base_relocation_count": 0,
            "highlow_relative_offsets": [], "normalized_sha256": "allocation-normalized",
        }
        readable = {
            "size": 18, "sha256": "readable", "highlow_base_relocation_count": 0,
            "highlow_relative_offsets": [], "normalized_sha256": "readable-normalized",
        }
        symbol = {
            "name": "global", "unit": "UNIT", "retail_rva": "0x3000",
            "candidate_count": 1, "status": "exact", "storage_class_matches": True,
            "retail_storage": {"class": "data-initialized"},
            "candidate_storage": {"class": "data-initialized"},
            "retail_payload": allocation, "candidate_payload": dict(allocation),
            "retail_readable_payload": readable,
            "candidate_readable_payload": dict(readable),
        }
        required = [{
            "name": "global", "unit": "UNIT", "size": 16,
            "retail_sha256": "allocation", "highlow_count": 0, "audit": "bytes",
            "readable_size": 18, "readable_sha256": "readable",
        }]
        diagnostics = required_initialized_storage_diagnostics(
            {"symbols": [symbol]}, required, {("UNIT", 0x3000): 16})
        self.assertEqual(diagnostics["verified"], 1)

        symbol["candidate_readable_payload"]["normalized_sha256"] = "changed"
        diagnostics = required_initialized_storage_diagnostics(
            {"symbols": [symbol]}, required, {("UNIT", 0x3000): 16})
        self.assertEqual(diagnostics["violations"][0]["status"],
                         "readable-span-mismatch")

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

    def test_pe_section_byte_comparison_reports_ranges_and_tail(self):
        def pe(section_bytes):
            data = bytearray(0x200 + len(section_bytes))
            data[:2] = b"MZ"
            struct.pack_into("<I", data, 0x3C, 0x80)
            data[0x80:0x84] = b"PE\0\0"
            struct.pack_into("<H", data, 0x84 + 2, 1)
            struct.pack_into("<H", data, 0x84 + 16, 0xE0)
            optional = 0x84 + 20
            struct.pack_into("<H", data, optional, 0x10B)
            section = optional + 0xE0
            data[section:section + 8] = b".data\0\0\0"
            struct.pack_into("<IIII", data, section + 8,
                             len(section_bytes), 0x2000, len(section_bytes), 0x200)
            data[0x200:0x200 + len(section_bytes)] = section_bytes
            return data

        with tempfile.TemporaryDirectory() as temp:
            retail = Path(temp) / "retail.exe"
            candidate = Path(temp) / "candidate.exe"
            retail.write_bytes(pe(b"abcDEF"))
            candidate.write_bytes(pe(b"abcXYFz"))
            result = compare_pe_section_bytes(retail, candidate, ".data")
            middle = compare_pe_section_range(retail, candidate, ".data", 2, 3)
        self.assertFalse(result["exact"])
        self.assertEqual(result["matched_bytes"], 4)
        self.assertEqual(result["mismatched_bytes"], 3)
        self.assertEqual(result["first_mismatch_ranges"], [
            {"offset": 3, "size": 2, "retail_hex": "4445",
             "candidate_hex": "5859"},
            {"offset": 6, "size": 1, "kind": "candidate-tail",
             "retail_hex": "", "candidate_hex": "7a"},
        ])
        self.assertEqual(middle["offset"], 2)
        self.assertEqual(middle["size"], 3)
        self.assertEqual(middle["mismatched_bytes"], 2)

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
            slots = _import_slot_identities(path)
        self.assertEqual(imports, [{"dll": "x.dll", "symbols": [
            {"ordinal": 14}, {"name": "Fn", "hint": 0}]}])
        self.assertEqual(slots, {
            0x2060: ("x.dll", "ordinal", 14),
            0x2064: ("x.dll", "name", "Fn"),
        })

    def test_vendor_abi_normalization_ignores_only_intra_dll_iat_order(self):
        imports = [{"dll": "mss32.dll", "symbols": [
            {"name": "_B@0", "hint": 2}, {"name": "_A@0", "hint": 1}]}]
        self.assertEqual(normalized_vendor_imports(imports), [{"dll": "mss32.dll", "symbols": [
            {"name": "_A@0", "hint": 1}, {"name": "_B@0", "hint": 2}]}])


if __name__ == "__main__":
    unittest.main()
