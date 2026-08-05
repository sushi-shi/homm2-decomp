"""The CodeView reader behind the /Od slot oracle.

These build the COFF by hand rather than invoking the compiler: the defect the
tests exist for is a record-layout misread, and that is reproducible offline. The
numbers encode what VC6 actually emits (CV5 records, 16-bit type indices), which
is NOT what MSVC 4.2 emitted - reading the 4.2 layout is exactly how this oracle
came to return an empty table for every function without failing.
"""
import struct
import unittest

from homm2.audit.od_oracle import parse_bprel, parse_frames

S_END, S_BPREL32, S_LPROC32, S_GPROC32 = 0x0006, 0x1006, 0x100A, 0x100B
S_BLOCK32 = 0x0207


def record(kind: int, body: bytes) -> bytes:
    return struct.pack("<HH", len(body) + 2, kind) + body


def proc(name: str) -> bytes:
    """S_GPROC32 with the shape VC6 emits: name length at body[35]."""
    body = bytes(35) + bytes([len(name)]) + name.encode("latin1")
    return record(S_GPROC32, body)


def bprel(name: str, offset: int) -> bytes:
    body = struct.pack("<iI", offset, 0x1010) + bytes([len(name)])
    return record(S_BPREL32, body + name.encode("latin1"))


def coff(blob: bytes, relocs, symbols) -> bytes:
    """One .debug$S section plus its relocations and a symbol table."""
    strings = b""
    entries = b""
    for name in symbols:
        if len(name) <= 8:
            raw = name.encode("latin1").ljust(8, b"\0")
        else:
            raw = struct.pack("<II", 0, 4 + len(strings))
            strings += name.encode("latin1") + b"\0"
        entries += raw + struct.pack("<IhHBB", 0, 1, 0x20, 2, 0)
    header_end = 20 + 40
    raw_ptr = header_end
    reloc_ptr = raw_ptr + len(blob)
    reloc_blob = b"".join(struct.pack("<IIH", off, idx, 6) for off, idx in relocs)
    sym_ptr = reloc_ptr + len(reloc_blob)
    header = struct.pack("<HHIIIHH", 0x14C, 1, 0, sym_ptr, len(symbols), 0, 0)
    section = (b".debug$S" + struct.pack(
        "<IIIIIIHHI", 0, 0, len(blob), raw_ptr, reloc_ptr, 0, len(relocs), 0, 0))
    return (header + section + blob + reloc_blob + entries
            + struct.pack("<I", 4 + len(strings)) + strings)


class ParseFramesTests(unittest.TestCase):
    def _object(self):
        blob = struct.pack("<I", 1)
        starts = {}
        for name, locals_ in (("combatManager::ViewBallista",
                               [("archerCount", -4), ("msg", -308), ("this", -312)]),
                              ("combatManager::RightClick", [("col", -4)])):
            starts[name] = len(blob)
            blob += proc(name)
            for local, off in locals_:
                blob += bprel(local, off)
            blob += record(S_END, b"")
        # the procedure record's `off` field is what carries the relocation
        relocs = [(starts["combatManager::ViewBallista"] + 4 + 28, 0),
                  (starts["combatManager::RightClick"] + 4 + 28, 1)]
        return coff(blob, relocs,
                    ["?ViewBallista@combatManager@@QAEXH@Z",
                     "?RightClick@combatManager@@QAEHH@Z"])

    def test_functions_are_keyed_by_the_mangled_symbol_not_the_codeview_name(self):
        # CodeView names the procedure `class::method`; every other tool in the
        # tree speaks the mangled name, so the join has to go through the reloc.
        frames = parse_frames(self._object())
        self.assertEqual(sorted(frames),
                         ["?RightClick@combatManager@@QAEHH@Z",
                          "?ViewBallista@combatManager@@QAEXH@Z"])

    def test_locals_come_back_shallowest_first(self):
        frames = parse_frames(self._object())
        self.assertEqual(frames["?ViewBallista@combatManager@@QAEXH@Z"],
                         [("archerCount", -4), ("msg", -308), ("this", -312)])

    def test_s_end_closes_the_procedure_so_frames_do_not_bleed(self):
        frames = parse_frames(self._object())
        self.assertEqual(frames["?RightClick@combatManager@@QAEHH@Z"],
                         [("col", -4)])

    def test_bprel_view_flattens_every_procedure(self):
        self.assertEqual(parse_bprel(self._object())["msg"], -308)


class LexicalBlockTests(unittest.TestCase):
    """A `{ }` block emits S_BLOCK32 + its own S_END inside the procedure.

    Treating that S_END as the end of the procedure silently drops every local
    after the first block - which is exactly the shape (inner-scope groups) the
    slot work cares most about.
    """

    def _object(self):
        blob = struct.pack("<I", 1)
        start = len(blob)
        blob += proc("Sib")
        blob += bprel("total", -4)
        blob += record(S_BLOCK32, bytes(19))
        blob += bprel("loopy", -16)
        blob += record(S_END, b"")
        blob += record(S_BLOCK32, bytes(19))
        blob += bprel("alpha", -20)
        blob += record(S_END, b"")
        blob += record(S_END, b"")
        blob += proc("After")
        blob += bprel("tail", -4)
        blob += record(S_END, b"")
        relocs = [(start + 4 + 28, 0)]
        after = blob.index(proc("After"))
        relocs.append((after + 4 + 28, 1))
        return coff(blob, relocs, ["?Sib@@YIHXZ", "?After@@YIHXZ"])

    def test_locals_after_an_inner_block_are_not_dropped(self):
        frames = parse_frames(self._object())
        self.assertEqual(frames["?Sib@@YIHXZ"],
                         [("total", -4), ("loopy", -16), ("alpha", -20)])

    def test_sibling_blocks_do_not_end_the_procedure_early(self):
        # the second procedure must still be seen
        self.assertEqual(parse_frames(self._object())["?After@@YIHXZ"],
                         [("tail", -4)])


class RobustnessTests(unittest.TestCase):
    def test_cv4_record_numbers_yield_nothing_rather_than_wrong_offsets(self):
        # MSVC 4.2 used 0x0200/0x0204. Reading those as VC6 records must not
        # silently produce a plausible-looking table.
        blob = struct.pack("<I", 1) + record(0x0204, bytes(35) + b"\x01p")
        self.assertEqual(parse_frames(coff(blob, [], ["_probe"])), {})

    def test_procedure_without_a_relocation_is_skipped_not_crashed(self):
        blob = struct.pack("<I", 1) + proc("probe") + bprel("x", -4)
        self.assertEqual(parse_frames(coff(blob, [], ["_probe"])), {})

    def test_section_without_a_codeview_signature_is_ignored(self):
        blob = b"-defaultlib:uuid.lib " + proc("probe")
        self.assertEqual(parse_frames(coff(blob, [], ["_probe"])), {})


if __name__ == "__main__":
    unittest.main()
