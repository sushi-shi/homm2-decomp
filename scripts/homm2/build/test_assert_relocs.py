import struct
import tempfile
import unittest
from collections import Counter
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

from homm2.build.assert_relocs import (
    _BODY_CACHE,
    _PARSE_CACHE,
    _classify_candidate_excess,
    _function_bytes,
    _maximum_data_identity_matching,
    _unique_report_functions,
    apply_folded_symbols,
    check_fn,
    check_ordinal_locals,
    check_owner_offset_multisets,
    check_ordered_owner_offsets,
    check_ordered_reloc_addresses,
    check_linked_pe_data_targets,
    check_pe_data_targets,
    classify_identity_transpositions,
    compare_function_reloc_addends,
    delinked_self_references,
    folded_comdat_symbols,
    is_ordinal_local,
    load_canonical_data_names,
    ordinal_local_relocs,
    parse_obj,
    relocation_addend_map,
    RelocAddressMismatch,
    semantic_import_rvas,
    UnresolvedCandidateReloc,
)
from homm2.build.reloc_owners import DataOwner, is_interior_reloc_alias, owners_from_rows


GCONFIG_SYMBOL = "?gConfig@@3UconfigStruct@@A"
GMONSTER_DATABASE_SYMBOL = "?gMonsterDatabase@@3PAUtag_monsterInfo@@A"


class DataIdentityTranspositionTest(unittest.TestCase):
    def test_unresolved_identity_remains_unpaired_without_crashing(self):
        records = [
            ("SOURCE/GAME", "?Run@@YIXXZ",
             UnresolvedCandidateReloc(0x10, 0xEB100, "$T1")),
            ("SOURCE/GAME", "?Run@@YIXXZ",
             RelocAddressMismatch(0x20, 0xEB104, 0xEB108, "$T2", "$T3")),
        ]
        transpositions, unmatched = classify_identity_transpositions(records)
        self.assertEqual(transpositions, [])
        self.assertEqual(unmatched, {0, 1})

    def test_concrete_address_swap_is_classified(self):
        records = [
            ("SOURCE/GAME", "?Run@@YIXXZ",
             RelocAddressMismatch(0x10, 0xEB100, 0xEB104, "$T1", "$T2")),
            ("SOURCE/GAME", "?Run@@YIXXZ",
             RelocAddressMismatch(0x20, 0xEB104, 0xEB100, "$T2", "$T1")),
        ]
        transpositions, unmatched = classify_identity_transpositions(records)
        self.assertEqual(len(transpositions), 1)
        self.assertEqual(transpositions[0]["sites"], [0x10, 0x20])
        self.assertEqual(unmatched, set())


class SemanticImportIdentityTest(unittest.TestCase):
    def test_iat_slots_match_by_dll_and_name_despite_order(self):
        first = ("kernel32.dll", "name", "FindFirstFileA")
        close = ("kernel32.dll", "name", "FindClose")
        retail = {0xEA070: close, 0xEA078: first}
        candidate = {0xEA080: first, 0xEA084: close}
        records = [
            {"name": "__imp__FindFirstFileA@8", "va": 0x4EA080},
            {"name": "__imp__FindClose@4", "va": 0x4EA084},
        ]
        self.assertEqual(semantic_import_rvas(
            retail, candidate, records, 0x400000), {
                "__imp__FindFirstFileA@8": {0xEA078},
                "__imp__FindClose@4": {0xEA070},
            })

    def test_ordinal_import_uses_dll_scoped_semantic_identity(self):
        smack_open = ("smackw32.dll", "ordinal", 14)
        retail = {0xEA3C8: smack_open}
        candidate = {0xEA390: smack_open}
        records = [{"name": "__imp__SmackOpen@12", "va": 0x4EA390}]
        self.assertEqual(semantic_import_rvas(
            retail, candidate, records, 0x400000), {
                "__imp__SmackOpen@12": {0xEA3C8},
            })


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

    def test_pe_data_gate_catches_wrong_compiler_local_identity(self):
        unit = "SOURCE/PHILAI"
        base = [(0xB4E, "DIR32", "$T40623", 0)]
        target = [(0xB4E, "DIR32", "_AI_EVENT_HUMAN_VALUE_FACTOR$S1540", 0)]
        local_rvas = {(unit, "$T40623"): {0xEB4D4}}
        pe_read = lambda _rva, _size: struct.pack("<I", 0x4EB280)
        problems = check_pe_data_targets(
            {}, {}, {}, local_rvas, unit, 0x3B865, base, target,
            [(".rdata", 0xEB000, 0xEBD00)], pe_read)
        self.assertEqual(len(problems), 1)
        self.assertIn("expected RVA 0xeb280, actual RVA 0xeb4d4",
                      problems[0].diagnostic())

    def test_pe_data_gate_accepts_exact_compiler_local_identity(self):
        unit = "SOURCE/PHILAI"
        base = [(0xB4E, "DIR32", "_AI_EVENT_HUMAN_VALUE_FACTOR$S1540", 0)]
        target = list(base)
        local_rvas = {(unit, base[0][2]): {0xEB280}}
        pe_read = lambda _rva, _size: struct.pack("<I", 0x4EB280)
        self.assertEqual(check_pe_data_targets(
            {}, {}, {}, local_rvas, unit, 0x3B865, base, target,
            [(".rdata", 0xEB000, 0xEBD00)], pe_read), [])

    def test_linked_pe_gate_compares_section_relative_destinations(self):
        def image(function_rva, target_rva):
            payload = bytearray(0x500)
            struct.pack_into("<I", payload, function_rva + 2,
                             0x400000 + target_rva)
            return (payload, 0x400000, [
                (".text", 0x100, 0x200, 0x100, 0x100),
                (".rdata", 0x300, 0x400, 0x300, 0x100),
            ])

        sites = [(2, "DIR32", "$T1", 0)]
        problems = check_linked_pe_data_targets(
            sites, sites, 0x110, 0x120,
            image(0x110, 0x320), image(0x120, 0x338))
        self.assertEqual(len(problems), 1)
        self.assertIn("retail .rdata+0x20, candidate .rdata+0x38",
                      problems[0].diagnostic())
        self.assertEqual(check_linked_pe_data_targets(
            sites, sites, 0x110, 0x120,
            image(0x110, 0x320), image(0x120, 0x320)), [])

    def test_linked_pe_gate_counts_each_section(self):
        def image(function_rva, target_rva):
            payload = bytearray(0x600)
            struct.pack_into("<I", payload, function_rva + 2,
                             0x400000 + target_rva)
            return (payload, 0x400000, [
                (".text", 0x100, 0x200, 0x100, 0x100),
                (".rdata", 0x300, 0x400, 0x300, 0x100),
                (".data", 0x400, 0x500, 0x400, 0x100),
            ])

        sites = [(2, "DIR32", "$T1", 0)]
        stats = Counter()
        self.assertEqual(check_linked_pe_data_targets(
            sites, sites, 0x110, 0x120,
            image(0x110, 0x320), image(0x120, 0x320), stats=stats), [])
        self.assertEqual(stats["compared_section_sites"], 1)
        self.assertEqual(stats["compared_rdata_sites"], 1)

    def test_linked_pe_gate_rejects_same_offset_in_different_context(self):
        def image(function_rva, target_rva, opcode):
            payload = bytearray(0x500)
            payload[function_rva + 1] = opcode
            struct.pack_into("<I", payload, function_rva + 2,
                             0x400000 + target_rva)
            return (payload, 0x400000, [
                (".text", 0x100, 0x200, 0x100, 0x100),
                (".rdata", 0x300, 0x400, 0x300, 0x100),
            ])

        sites = [(2, "DIR32", "$T1", 0)]
        stats = Counter()
        self.assertEqual(check_linked_pe_data_targets(
            sites, sites, 0x110, 0x120,
            image(0x110, 0x320, 0xA1), image(0x120, 0x338, 0x68),
            stats=stats), [])
        self.assertEqual(stats["context_mismatch"], 1)

    def test_multiset_matching_preserves_duplicate_occurrences(self):
        expected = [{"rva": 0x100}, {"rva": 0x100}, {"rva": 0x104}]
        candidate = [
            {"site": 8, "identities": [0x100, 0x104]},
            {"site": 4, "identities": [0x100]},
            {"site": 12, "identities": [0x100]},
        ]
        self.assertEqual(
            _maximum_data_identity_matching(expected, candidate), ([], []))

    def test_multiset_matching_reports_each_unmatched_side(self):
        expected = [{"rva": 0x100}, {"rva": 0x104}]
        candidate = [
            {"site": 4, "identities": [0x100]},
            {"site": 8, "identities": [0x108]},
        ]
        self.assertEqual(
            _maximum_data_identity_matching(expected, candidate), ([1], [1]))

    def test_candidate_excess_separates_overpublication_from_novel_identity(self):
        expected = [{"rva": 0x100}, {"rva": 0x104}]
        excess = [
            {"site": 4, "identities": [0x100]},
            {"site": 8, "identities": [0x108]},
        ]
        annotated, novel = _classify_candidate_excess(expected, excess)
        self.assertEqual(
            [record["retail_function_identity_overlap"] for record in annotated],
            [[0x100], []])
        self.assertEqual([record["site"] for record in novel], [8])

    def test_duplicate_objdiff_record_prefers_live_score(self):
        functions, duplicates = _unique_report_functions([
            {"name": "?same@@YIXXZ", "fuzzy_match_percent": None},
            {"name": "?same@@YIXXZ", "fuzzy_match_percent": 100.0},
            {"name": "?other@@YIXXZ", "fuzzy_match_percent": 80.0},
        ])
        self.assertEqual(duplicates, {"?same@@YIXXZ": 2})
        self.assertEqual(
            {record["name"]: record["fuzzy_match_percent"]
             for record in functions},
            {"?other@@YIXXZ": 80.0, "?same@@YIXXZ": 100.0})


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


GTARGET_SYMBOL = "_gTargetName"
STREAM_SYMBOL = "?stream@AudiereMusicState@@2V?$RefPtr@VOutputStream@audiere@@@audiere@@A"
SOURCE_SYMBOL = "?source@AudiereMusicState@@2V?$RefPtr@VSampleSource@audiere@@@audiere@@A"


def _one_text_object(payload):
    """A minimal COFF wrapper holding one .text section with ``payload``."""
    header = 20 + 40
    data = bytearray(header + len(payload))
    struct.pack_into("<HHIIIHH", data, 0, 0x14C, 1, 0, 0, 0, 0, 0)
    data[20:28] = b".text\0\0\0"
    struct.pack_into("<II", data, 20 + 16, len(payload), header)
    data[header:] = payload
    return bytes(data)


class OrdinalLocalTest(unittest.TestCase):
    """MSVC counter-named thunks have no identity across two compilations."""

    def test_counter_names_are_recognised(self):
        self.assertTrue(is_ordinal_local("_$E16"))
        self.assertTrue(is_ordinal_local("$S18"))
        self.assertFalse(is_ordinal_local("_atexit"))
        self.assertFalse(is_ordinal_local("$SG59945"))
        self.assertFalse(is_ordinal_local(STREAM_SYMBOL))

    def test_references_between_counter_symbols_are_dropped(self):
        functions = {
            "_$E19": [("REL32", "_$E17", 0), ("REL32", "_atexit", 0)],
            "?Real@@YIXXZ": [("DIR32", STREAM_SYMBOL, 0)],
        }
        self.assertEqual(ordinal_local_relocs(functions),
                         [("REL32", "_atexit", 0)])

    def test_renumbered_group_with_the_same_targets_passes(self):
        sym = {"_atexit": 0xD7548, STREAM_SYMBOL: 0x1395E0, SOURCE_SYMBOL: 0x1395DC}
        dups = {name: {rva} for name, rva in sym.items()}
        base = {
            "_$E16": [("DIR32", STREAM_SYMBOL, 0)],
            "_$E19": [("REL32", "_$E17", 0), ("REL32", "_atexit", 0)],
            "_$E22": [("DIR32", SOURCE_SYMBOL, 0)],
        }
        target = {                                # retail numbers the same thunks lower
            "_$E14": [("DIR32", STREAM_SYMBOL, 0)],
            "_$E16": [("REL32", "_$E15", 0), ("REL32", "_atexit", 0)],
            "_$E19": [("DIR32", SOURCE_SYMBOL, 0)],
        }
        self.assertEqual(
            check_ordinal_locals(sym, {}, dups, "BASE/AudiereMusic", base, target), [])

    def test_group_still_reports_a_global_retail_never_touches(self):
        sym = {STREAM_SYMBOL: 0x1395E0, SOURCE_SYMBOL: 0x1395DC}
        dups = {name: {rva} for name, rva in sym.items()}
        base = {"_$E16": [("DIR32", STREAM_SYMBOL, 0), ("DIR32", SOURCE_SYMBOL, 0)]}
        target = {"_$E14": [("DIR32", STREAM_SYMBOL, 0)]}
        problems = check_ordinal_locals(sym, {}, dups, "BASE/AudiereMusic", base, target)
        self.assertEqual(len(problems), 1)
        self.assertIn("0x1395dc", problems[0])


class DelinkedSelfReferenceTest(unittest.TestCase):
    """A recursive call stays inside the carved span, so the delinker relocates nothing."""

    def setUp(self):
        _BODY_CACHE.clear()
        self.addCleanup(_BODY_CACHE.clear)

    def _target(self, displacement):
        # push ebp; mov ebp,esp; call <disp32>; ret -> the call field sits at +4.
        field = struct.pack("<i", displacement)
        body = b"\x55\x8b\xec\xe8" + field + b"\xc3"
        call = " ".join("%02x" % byte for byte in b"\xe8" + field)
        dump = ("Disassembly of section .text:\n"
                "00000530 <?Recurse@@YIXXZ>:\n"
                "     530: 55                           \tpushl\t%ebp\n"
                "     531: 8b ec                        \tmovl\t%esp, %ebp\n"
                "     533: " + call + "               \tcalll\t0x530\n"
                "     538: c3                           \tretl\n")
        return body, dump

    def _count(self, displacement, base_sites, target_sites):
        body, dump = self._target(displacement)
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "target.c.obj"
            path.write_bytes(_one_text_object(body))
            with patch("homm2.build.assert_relocs.subprocess.run",
                       return_value=SimpleNamespace(stdout=dump, returncode=0)):
                return delinked_self_references(
                    str(path), "?Recurse@@YIXXZ", 0x98CBC, base_sites, target_sites)

    def test_self_call_without_a_target_relocation_is_agreement(self):
        base = [(4, "REL32", "?Recurse@@YIXXZ", 0)]
        self.assertEqual(self._count(-8, base, []), Counter({0x98CBC: 1}))

    def test_a_call_that_leaves_the_function_is_not_counted(self):
        base = [(4, "REL32", "?Recurse@@YIXXZ", 0)]
        self.assertEqual(self._count(0x40, base, []), Counter())

    def test_a_relocated_target_site_is_not_counted(self):
        base = [(4, "REL32", "?Recurse@@YIXXZ", 0)]
        target = [(4, "REL32", "?Elsewhere@@YIXXZ", 0)]
        self.assertEqual(self._count(-8, base, target), Counter())

    def test_a_call_to_another_function_is_not_counted(self):
        base = [(4, "REL32", "?Elsewhere@@YIXXZ", 0)]
        self.assertEqual(self._count(-8, base, []), Counter())


class FoldedComdatTest(unittest.TestCase):
    """VC6 merges identical COMDATs, so two instantiations share one retail address."""

    OURS = "??4?$RefPtr@VSampleSource@audiere@@@audiere@@QAEAAV01@PAVSampleSource@1@@Z"
    THEIRS = "??4?$RefPtr@VOutputStream@audiere@@@audiere@@QAEAAV01@PAVOutputStream@1@@Z"

    def setUp(self):
        _BODY_CACHE.clear()
        self.addCleanup(_BODY_CACHE.clear)

    def _folded(self, second_body, base_sites, target_sites):
        dump = ("Disassembly of section .text:\n"
                "00000000 <%s>:\n"
                "       0: 55                           \tpushl\t%%ebp\n"
                "       1: c3                           \tretl\n"
                "Disassembly of section .text:\n"
                "00000000 <%s>:\n"
                "%s"
                % (self.THEIRS, self.OURS, second_body))
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "base.obj"
            path.write_bytes(_one_text_object(b"\x55\xc3"))
            with patch("homm2.build.assert_relocs.subprocess.run",
                       return_value=SimpleNamespace(stdout=dump, returncode=0)):
                return folded_comdat_symbols(
                    str(path), {self.THEIRS: 0xCCFA0}, {}, base_sites, target_sites)

    IDENTICAL = ("       0: 55                           \tpushl\t%ebp\n"
                 "       1: c3                           \tretl\n")
    DIFFERENT = ("       0: 90                           \tnop\n"
                 "       1: c3                           \tretl\n")

    def test_identical_bodies_named_at_the_same_site_fold(self):
        folded = self._folded(
            self.IDENTICAL,
            [(0x131, "REL32", self.OURS, 0)],
            [(0x131, "REL32", self.THEIRS, 0)])
        self.assertEqual(folded, {self.OURS: self.THEIRS})
        self.assertEqual(
            apply_folded_symbols([("REL32", self.OURS, 0)], folded),
            [("REL32", self.THEIRS, 0)])

    def test_different_bodies_never_fold(self):
        self.assertEqual(
            self._folded(
                self.DIFFERENT,
                [(0x131, "REL32", self.OURS, 0)],
                [(0x131, "REL32", self.THEIRS, 0)]),
            {})

    def test_folding_needs_retail_to_name_the_partner_at_that_site(self):
        self.assertEqual(
            self._folded(self.IDENTICAL, [(0x131, "REL32", self.OURS, 0)], []),
            {})

    def test_a_resolvable_symbol_is_never_rewritten(self):
        self.assertEqual(
            self._folded(
                self.IDENTICAL,
                [(0x131, "REL32", self.THEIRS, 0)],
                [(0x131, "REL32", self.OURS, 0)]),
            {})


class FunctionBytesTest(unittest.TestCase):
    def setUp(self):
        _BODY_CACHE.clear()
        self.addCleanup(_BODY_CACHE.clear)

    def test_a_maximal_length_instruction_keeps_its_last_byte(self):
        # llvm-objdump leaves no space between the tenth byte and the mnemonic tab.
        dump = ("Disassembly of section .text:\n"
                "00001330 <?Wide@@YIXXZ>:\n"
                "    1330: c7 05 00 00 00 00 01 00 00 00\tmovl\t$0x1, 0x0\n"
                "    133a: c3                           \tretl\n")
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "wide.obj"
            path.write_bytes(_one_text_object(b"\0" * 11))
            with patch("homm2.build.assert_relocs.subprocess.run",
                       return_value=SimpleNamespace(stdout=dump, returncode=0)):
                bodies = _function_bytes(str(path))
        self.assertEqual(
            bodies["?Wide@@YIXXZ"],
            bytes.fromhex("c7050000000001000000") + b"\xc3")


class WrongTargetStillFailsTest(unittest.TestCase):
    """The reconciliations above must not blunt the ordinary wrong-global check."""

    def test_a_wrong_global_is_still_reported(self):
        sym = {"?gArmyNames@@3PAPADA": 0xFDDC8, "?gArmyNamesPlural@@3PAPADA": 0xFDED0}
        dups = {name: {rva} for name, rva in sym.items()}
        problems = check_fn(
            sym, {}, dups, "SOURCE/TOWNMGR", "?SplitArmy@townManager@@QAEXXZ",
            [("DIR32", "?gArmyNames@@3PAPADA", 0)],
            [("DIR32", "?gArmyNamesPlural@@3PAPADA", 0)])
        self.assertEqual(len(problems), 1)
        self.assertIn("0xfddc8", problems[0])

    def test_an_extra_target_address_is_not_invented_by_tgt_extra(self):
        sym = {"?Recurse@@YIXXZ": 0x98CBC, "?gArmyNames@@3PAPADA": 0xFDDC8}
        dups = {name: {rva} for name, rva in sym.items()}
        problems = check_fn(
            sym, {}, dups, "SOURCE/SPELLS", "?Recurse@@YIXXZ",
            [("REL32", "?Recurse@@YIXXZ", 0), ("DIR32", "?gArmyNames@@3PAPADA", 0)],
            [], Counter({0x98CBC: 1}))
        self.assertEqual(len(problems), 1)
        self.assertIn("0xfddc8", problems[0])


if __name__ == "__main__":
    unittest.main()
