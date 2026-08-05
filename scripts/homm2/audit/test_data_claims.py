import struct
import unittest

from homm2.audit.data_claims import (
    IMAGE_BASE, IMAGE_DIR32, RetailImage, element_sizes, extent_finding, has_majority,
    identifier, insert_markers, inventory_key, masked_equal, owner_votes,
    payload_verdict, proven_extent, select_claims)


def pe_image(sections, extent=0x200000):
    """Smallest PE whose section table this module's reader can walk."""
    header = bytearray(0x40 + 24 + 0xE0 + 40 * len(sections))
    struct.pack_into("<I", header, 0x3C, 0x40)
    struct.pack_into("<H", header, 0x40 + 6, len(sections))
    struct.pack_into("<H", header, 0x40 + 20, 0xE0)
    struct.pack_into("<I", header, 0x40 + 0x50, extent)   # SizeOfImage
    blob = bytearray(header)
    for index, (name, rva, virtual_size, payload) in enumerate(sections):
        offset = 0x40 + 24 + 0xE0 + index * 40
        blob[offset:offset + len(name)] = name.encode()
        struct.pack_into("<4I", blob, offset + 8,
                         virtual_size, rva, len(payload), len(blob) + 0)
        struct.pack_into("<I", blob, offset + 20, len(blob))
        blob += payload
    return RetailImage(bytes(blob))


class RetailImageTests(unittest.TestCase):
    def setUp(self):
        # .data's virtual size runs 8 bytes past its raw size: the loader-zero
        # tail every uninitialized allocation lives in.
        self.image = pe_image([(".rdata", 0x1000, 4, b"\x01\x02\x03\x04"),
                               (".data", 0x2000, 12, b"\xaa\xbb\xcc\xdd")])

    def test_payload_reads_within_a_section(self):
        self.assertEqual(self.image.payload(0x1000, 4), b"\x01\x02\x03\x04")

    def test_payload_of_an_unmapped_address_is_none(self):
        self.assertIsNone(self.image.payload(0x9000, 4))

    def test_loader_zero_tail_reads_as_zeros_not_a_short_read(self):
        self.assertEqual(self.image.payload(0x2008, 4), b"\0\0\0\0")

    def test_read_spanning_into_the_zero_tail_is_padded(self):
        self.assertEqual(self.image.payload(0x2002, 4), b"\xcc\xdd\0\0")

    def test_storage_names_the_section(self):
        self.assertEqual(self.image.storage(0x1002), ".rdata")

    def test_storage_marks_the_zero_tail(self):
        self.assertEqual(self.image.storage(0x2008), ".data-zero-tail")

    def test_extent_comes_from_the_optional_header(self):
        self.assertEqual(self.image.extent, 0x200000)


class MaskedEqualTests(unittest.TestCase):
    OURS = bytes([0x90, 0xA1, 0x00, 0x00, 0x00, 0x00, 0xC3])
    RETAIL = bytes([0x90, 0xA1, 0x20, 0x30, 0x45, 0x00, 0xC3])
    SITE = [(2, IMAGE_DIR32, "?g@@3HA", 0)]

    def test_relocated_field_is_masked_on_both_sides(self):
        self.assertTrue(masked_equal(self.OURS, self.RETAIL, self.SITE))

    def test_difference_outside_a_site_still_fails(self):
        broken = bytearray(self.RETAIL)
        broken[6] = 0xC2
        self.assertFalse(masked_equal(self.OURS, bytes(broken), self.SITE))

    def test_a_shorter_body_is_never_equal(self):
        self.assertFalse(masked_equal(self.OURS[:4], self.RETAIL, self.SITE))


class OwnerVoteTests(unittest.TestCase):
    def _body(self, target, addend, symbol="?gMap@@3HA", kind=IMAGE_DIR32):
        retail = bytearray(8)
        struct.pack_into("<I", retail, 2, target)
        return bytes(retail), [(2, kind, symbol, addend)]

    def test_site_votes_for_target_minus_addend(self):
        retail, sites = self._body(IMAGE_BASE + 0x1234, 0x10)
        self.assertEqual(owner_votes(retail, sites, 0x200000),
                         [("?gMap@@3HA", 0x1224)])

    def test_relative_relocations_do_not_vote(self):
        retail, sites = self._body(IMAGE_BASE + 0x1234, 0, kind=20)
        self.assertEqual(owner_votes(retail, sites, 0x200000), [])

    def test_compiler_owned_symbols_do_not_vote(self):
        retail, sites = self._body(IMAGE_BASE + 0x1234, 0, symbol="??_C@_01A@x@")
        self.assertEqual(owner_votes(retail, sites, 0x200000), [])

    def test_a_dword_outside_the_image_is_not_an_address(self):
        retail, sites = self._body(0x1234, 0)
        self.assertEqual(owner_votes(retail, sites, 0x200000), [])

    def test_an_addend_that_walks_the_owner_below_zero_is_dropped(self):
        retail, sites = self._body(IMAGE_BASE + 0x10, 0x40)
        self.assertEqual(owner_votes(retail, sites, 0x200000), [])


class MajorityTests(unittest.TestCase):
    def test_unanimous_wins(self):
        self.assertTrue(has_majority([[0x1000, 30]], 30))

    def test_balanced_transposition_minority_loses(self):
        # MAP_WIDTH: 217 sites name it, 11 name its co-factor instead.
        self.assertTrue(has_majority([[0x1000, 217], [0x1004, 11]], 228))

    def test_a_runner_up_over_half_the_winner_is_not_a_clear_owner(self):
        self.assertFalse(has_majority([[0x1000, 10], [0x1004, 6]], 16))

    def test_a_plurality_below_half_of_all_votes_is_not_a_clear_owner(self):
        self.assertFalse(has_majority([[0x1000, 4], [0x1004, 2], [0x1008, 2],
                                       [0x100c, 2]], 10), )

    def test_no_votes_at_all(self):
        self.assertFalse(has_majority([], 0))


class PayloadVerdictTests(unittest.TestCase):
    def setUp(self):
        self.image = pe_image([(".data", 0x2000, 24, b"\x01\x02\x03\x04\xaa\xbb\xcc\xdd")])

    def _record(self, payload, pointers=(), size=None):
        return {"payload": payload, "pointers": list(pointers),
                "size": size if size is not None else len(payload or b"")}

    def test_initialized_payload_matching_retail_is_exact(self):
        verdict, span = payload_verdict(self._record(b"\x01\x02\x03\x04"), 0x2000, self.image)
        self.assertEqual((verdict, span), ("exact", None))

    def test_matching_only_after_masking_a_pointer_field_is_reported_as_such(self):
        record = self._record(b"\x00\x00\x00\x00\xaa\xbb\xcc\xdd", pointers=[0])
        self.assertEqual(payload_verdict(record, 0x2000, self.image)[0], "exact-masked")

    def test_a_contradicted_payload_reports_the_differing_span(self):
        verdict, span = payload_verdict(
            self._record(b"\x01\x02\x03\x04\x00\x00\x00\x00"), 0x2000, self.image)
        self.assertEqual(verdict, "differs(4/8)")
        self.assertEqual(span, (4, 7, 8))

    def test_uninitialized_allocation_on_zero_storage_is_zero(self):
        record = self._record(None, size=4)
        self.assertEqual(payload_verdict(record, 0x2010, self.image)[0], "zero")

    def test_uninitialized_allocation_on_initialized_storage_is_rejected(self):
        record = self._record(None, size=4)
        self.assertEqual(payload_verdict(record, 0x2000, self.image)[0], "nonzero-bss")

    def test_an_unmapped_address_is_unreadable(self):
        self.assertEqual(
            payload_verdict(self._record(b"\x01"), 0x9000, self.image)[0], "unreadable")


class IdentifierTests(unittest.TestCase):
    def test_external_global(self):
        self.assertEqual(identifier("?gMapX@@3HA"), "gMapX")

    def test_internal_global(self):
        self.assertEqual(identifier("_s_currentNode"), "s_currentNode")

    def test_a_class_member_is_not_a_namespace_scope_global(self):
        self.assertIsNone(identifier("?count@widget@@2HA"))

    def test_a_function_is_not_a_global(self):
        self.assertIsNone(identifier("?Main@@YIXXZ"))


class InventoryKeyTests(unittest.TestCase):
    def test_an_external_global_is_named_program_wide(self):
        self.assertEqual(inventory_key("SOURCE/KB", "?gMapX@@3HA"), ("", "gMapX"))

    def test_a_file_static_is_named_within_its_unit(self):
        self.assertEqual(inventory_key("BASE/Icon2b", "_s_dst"),
                         ("BASE/Icon2b", "s_dst"))

    def test_two_decoders_statics_are_different_keys(self):
        self.assertNotEqual(inventory_key("BASE/Icon2b", "_s_dst"),
                            inventory_key("BASE/Iconf2b", "_s_dst"))

    def test_a_member_has_no_key(self):
        self.assertIsNone(inventory_key("SOURCE/KB", "?count@widget@@2HA"))


def proposal(**overrides):
    row = {"symbol": "?gMapX@@3HA", "unit": "SOURCE/KB", "rva": 0x1000, "size": 4,
           "section": ".data", "storage": ".data", "votes": 9, "total": 9,
           "distinct": 1, "payload": "exact", "diff_span": None,
           "ambiguous_unit": False, "collides_function": None,
           "collides_claim": None, "ranked": [[0x1000, 9]]}
    row.update(overrides)
    return row


def vardecl(**overrides):
    row = {"unit": "SOURCE/KB", "file": "src/SOURCE/KB.cpp", "name": "gMapX",
           "symbol": "?gMapX@@3HA", "scope": "TRANSLATION_UNIT", "static": False,
           "size": 4, "element_size": 0, "type": "i32", "start": 100, "end": 120,
           "line": 7, "line_prefix_blank": True, "siblings_in_span": 1,
           "marked": False}
    row.update(overrides)
    return row


class SelectClaimsTests(unittest.TestCase):
    def _select(self, proposals=None, vardecls=None):
        return select_claims(proposals or [proposal()], vardecls or [vardecl()])

    def _reason(self, **overrides):
        accepted, rejected = self._select([proposal(**overrides)])
        self.assertEqual(accepted, [])
        return rejected[0]["reason"]

    def test_a_clean_proposal_is_accepted_with_its_source_location(self):
        accepted, rejected = self._select()
        self.assertEqual(rejected, [])
        self.assertEqual(accepted[0]["file"], "src/SOURCE/KB.cpp")
        self.assertEqual(accepted[0]["identifier"], "gMapX")

    def test_a_member_spelling_is_rejected(self):
        self.assertEqual(self._reason(symbol="?count@widget@@2HA"),
                         "not a namespace-scope global spelling")

    def test_a_symbol_with_no_source_definition_is_rejected(self):
        self.assertEqual(self._reason(symbol="?gOther@@3HA"), "no file-scope definition")

    def test_two_definitions_of_one_symbol_are_rejected(self):
        accepted, rejected = self._select(
            vardecls=[vardecl(), vardecl(file="src/SOURCE/GAME.cpp")])
        self.assertEqual(accepted, [])
        self.assertEqual(rejected[0]["reason"], "defined in more than one place")

    def test_an_object_defined_in_two_units_is_rejected(self):
        self.assertEqual(self._reason(ambiguous_unit=True),
                         "defined in more than one place")

    def test_object_owner_must_be_the_source_owner(self):
        self.assertIn("!=", self._reason(unit="SOURCE/GAME"))

    def test_two_external_definitions_sharing_an_identifier_are_rejected(self):
        # Different mangled types, one identifier: the inventory cannot tell them
        # apart, so neither may claim an address.
        accepted, rejected = self._select(
            vardecls=[vardecl(), vardecl(symbol="?gMapX@@3PADA", unit="SOURCE/GAME",
                                         file="src/SOURCE/GAME.cpp")])
        self.assertEqual(accepted, [])
        self.assertEqual(rejected[0]["reason"], "identifier is shared by another definition")

    def test_a_file_static_in_another_unit_is_a_different_object(self):
        accepted, _ = self._select(
            vardecls=[vardecl(), vardecl(symbol="_gMapX", unit="SOURCE/GAME",
                                         file="src/SOURCE/GAME.cpp")])
        self.assertEqual([row["symbol"] for row in accepted], ["?gMapX@@3HA"])

    def test_each_decoder_claims_its_own_copy_of_a_shared_static_name(self):
        units = ["BASE/Icon2b", "BASE/Iconf2b"]
        proposals = [proposal(symbol="_s_dst", unit=unit, rva=0x1000 + index * 4,
                              payload="zero", section=".bss",
                              ranked=[[0x1000 + index * 4, 9]])
                     for index, unit in enumerate(units)]
        decls = [vardecl(symbol="_s_dst", name="s_dst", unit=unit,
                         file="src/%s.cpp" % unit) for unit in units]
        accepted, rejected = self._select(proposals, decls)
        self.assertEqual(rejected, [])
        self.assertEqual([row["unit"] for row in accepted], units)
        self.assertEqual([row["rva"] for row in accepted], [0x1000, 0x1004])

    def test_an_address_already_claimed_by_a_function_is_rejected(self):
        self.assertEqual(self._reason(collides_function="?Main@@YIXXZ"),
                         "address is a claimed function")

    def test_an_address_a_landed_marker_already_owns_is_rejected(self):
        self.assertEqual(self._reason(collides_claim="_s_drawHeroYOffset"),
                         "address is already claimed by _s_drawHeroYOffset")

    def test_a_definition_that_already_carries_a_marker_is_not_marked_twice(self):
        accepted, rejected = self._select(vardecls=[vardecl(marked=True)])
        self.assertEqual(accepted, [])
        self.assertEqual(rejected[0]["reason"],
                         "definition already carries a DATA marker")

    def test_a_declaration_sharing_its_line_is_rejected(self):
        accepted, rejected = self._select(vardecls=[vardecl(line_prefix_blank=False)])
        self.assertEqual(rejected[0]["reason"], "declaration is not a lone statement")

    def test_a_declaration_with_a_sibling_in_its_span_is_rejected(self):
        accepted, rejected = self._select(vardecls=[vardecl(siblings_in_span=2)])
        self.assertEqual(rejected[0]["reason"], "declaration is not a lone statement")

    def test_a_split_vote_is_rejected(self):
        self.assertEqual(
            self._reason(ranked=[[0x1000, 5], [0x2000, 4]], votes=5, total=9),
            "no clear owner majority")

    def test_an_initialized_allocation_on_zero_storage_is_rejected(self):
        self.assertEqual(self._reason(payload="zero"),
                         "initialized allocation lands on zero storage")

    def test_an_uninitialized_allocation_on_zero_storage_is_accepted(self):
        accepted, _ = self._select([proposal(payload="zero", section=".bss")])
        self.assertEqual(accepted[0]["payload"], "zero")

    def test_a_wholesale_payload_contradiction_is_rejected(self):
        self.assertEqual(
            self._reason(payload="differs(8/16)", diff_span=(0, 15, 16)),
            "payload contradicts the address")

    def test_a_contradiction_confined_to_the_final_dword_is_a_tail_overrun(self):
        accepted, _ = self._select(
            [proposal(payload="differs(4/16)", diff_span=(12, 15, 16))])
        self.assertEqual(accepted[0]["payload"], "tail-overrun")

    def test_read_only_data_must_land_in_rdata(self):
        self.assertEqual(self._reason(section=".rdata", storage=".data"),
                         "read-only allocation lands outside .rdata")

    def test_writable_data_must_land_in_data(self):
        self.assertEqual(self._reason(section=".data", storage=".rdata"),
                         "writable allocation lands in .rdata")

    def test_one_address_keeps_one_owner_and_the_proven_payload_wins(self):
        rival = proposal(symbol="?gMapY@@3HA", payload="zero", section=".bss")
        decls = [vardecl(), vardecl(symbol="?gMapY@@3HA", name="gMapY", start=200, end=220)]
        accepted, rejected = self._select([rival, proposal()], decls)
        self.assertEqual([row["symbol"] for row in accepted], ["?gMapX@@3HA"])
        self.assertEqual(rejected[0]["reason"], "address also claimed by ?gMapX@@3HA")

    def test_the_object_extent_overrides_a_stale_proposal_size(self):
        accepted, _ = self._select([proposal(size=8)])
        self.assertEqual(accepted[0]["size"], 4)


class InsertMarkersTests(unittest.TestCase):
    SOURCE = b"i32 a = 1;\ni32 b = 2;\n"

    def test_marker_is_written_in_front_of_the_definition(self):
        claim = {"start": 0, "rva": 0x1000}
        self.assertEqual(insert_markers(self.SOURCE, [claim]),
                         b"DATA(0x00401000) i32 a = 1;\ni32 b = 2;\n")

    def test_several_claims_in_one_file_all_land_at_their_own_offsets(self):
        claims = [{"start": 0, "rva": 0x1000}, {"start": 11, "rva": 0x2000}]
        self.assertEqual(
            insert_markers(self.SOURCE, claims),
            b"DATA(0x00401000) i32 a = 1;\nDATA(0x00402000) i32 b = 2;\n")

    def test_the_address_is_absolute_and_eight_digits(self):
        self.assertIn(b"DATA(0x004fdffc)",
                      insert_markers(self.SOURCE, [{"start": 0, "rva": 0xFDFFC}]))


class ProvenExtentTests(unittest.TestCase):
    def test_a_neighbour_bounds_the_length(self):
        self.assertEqual(proven_extent(16, None, None, (), 12), 12)

    def test_no_neighbour_and_no_contradiction_proves_nothing(self):
        self.assertEqual(proven_extent(16, b"\0" * 16, b"\0" * 16, (), 16), 16)

    def test_the_payload_bounds_the_length_where_the_bytes_diverge(self):
        ours = b"\x01\x02\x03\x04" + b"\0" * 4
        retail = b"\x01\x02\x03\x04" + b"\xaa\xbb\xcc\xdd"
        self.assertEqual(proven_extent(8, ours, retail, (), 8), 4)

    def test_our_own_relocated_fields_do_not_count_as_divergence(self):
        ours = b"\0\0\0\0" + b"\x05\x00\x00\x00"
        retail = b"\x40\x30\x20\x10" + b"\x05\x00\x00\x00"
        self.assertEqual(proven_extent(8, ours, retail, (0,), 8), 8)

    def test_the_tighter_of_the_two_channels_wins(self):
        ours = b"\x01" + b"\0" * 7
        retail = b"\x01" + b"\xff" * 7
        self.assertEqual(proven_extent(8, ours, retail, (), 4), 1)


class ExtentFindingTests(unittest.TestCase):
    def test_an_allocation_within_its_bound_is_ok(self):
        self.assertEqual(extent_finding(12, 12, b"\0" * 12, 4), ("ok", 0))

    def test_a_zero_filled_surplus_of_one_element(self):
        self.assertEqual(extent_finding(24, 20, b"\0" * 24, 4),
                         ("one-element-too-long", 4))

    def test_a_four_byte_surplus_of_a_byte_array_is_not_one_element(self):
        self.assertEqual(extent_finding(16, 12, b"\0" * 16, 1), ("too-long", 4))

    def test_a_surplus_holding_our_own_data_is_a_content_fault_not_a_length_one(self):
        payload = b"\0" * 20 + b"\x01\x00\x00\x00"
        self.assertEqual(extent_finding(24, 20, payload, 4), ("nonzero-tail", 4))

    def test_a_bss_allocation_has_no_payload_to_check(self):
        self.assertEqual(extent_finding(24, 20, None, 4),
                         ("one-element-too-long", 4))

    def test_a_multi_element_surplus_is_reported_as_plain_too_long(self):
        self.assertEqual(extent_finding(24, 16, b"\0" * 24, 4), ("too-long", 8))


class ElementSizeTests(unittest.TestCase):
    def test_arrays_are_indexed_by_unit_and_linker_name(self):
        self.assertEqual(
            element_sizes([vardecl(element_size=4)]),
            {("SOURCE/KB", "?gMapX@@3HA"): 4})

    def test_a_scalar_contributes_nothing(self):
        self.assertEqual(element_sizes([vardecl()]), {})


if __name__ == "__main__":
    unittest.main()
