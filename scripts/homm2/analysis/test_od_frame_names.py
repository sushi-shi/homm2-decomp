import struct
import unittest

from homm2.analysis.od_frame_names import format_frames, frame_names

SYM_END = 0x0006
SYM_BLOCK32 = 0x0207
SYM_BPREL32 = 0x1006
SYM_GPROC32 = 0x100A


def _record(record_type: int, body: bytes) -> bytes:
    return struct.pack("<HH", len(body) + 2, record_type) + body


def _proc(name: str) -> bytes:
    encoded = name.encode("latin1")
    return _record(SYM_GPROC32, b"\0" * 35 + bytes([len(encoded)]) + encoded)


def _bprel(displacement: int, name: str) -> bytes:
    encoded = name.encode("latin1")
    body = struct.pack("<i", displacement) + b"\0" * 4 + bytes([len(encoded)]) + encoded
    return _record(SYM_BPREL32, body)


def _object(debug_stream: bytes) -> bytes:
    """A one-section COFF carrying `debug_stream` as .debug$S."""
    header = struct.pack("<HHIIIHH", 0x14C, 1, 0, 0, 0, 0, 0)
    section_start = len(header) + 40
    section = (
        b".debug$S".ljust(8, b"\0")
        + struct.pack("<II", 0, 0)
        + struct.pack("<II", len(debug_stream), section_start)
        + struct.pack("<IIHHI", 0, 0, 0, 0, 0)
    )
    return header + section + debug_stream


class FrameNameTests(unittest.TestCase):
    def test_slots_are_attributed_to_their_enclosing_function(self):
        stream = struct.pack("<I", 1) + (
            _proc("?First@@YIXXZ")
            + _bprel(-4, "count")
            + _bprel(-8, "index")
            + _record(SYM_END, b"")
            + _proc("?Second@@YIXXZ")
            + _bprel(-12, "other")
            + _record(SYM_END, b"")
        )
        slots = frame_names(_object(stream))
        self.assertEqual(slots["?First@@YIXXZ"], [(-4, "count"), (-8, "index")])
        self.assertEqual(slots["?Second@@YIXXZ"], [(-12, "other")])

    def test_format_orders_deepest_slot_last_and_filters(self):
        stream = struct.pack("<I", 1) + (
            _proc("?Target@@YIXXZ")
            + _bprel(-8, "deep")
            + _bprel(-4, "shallow")
            + _record(SYM_END, b"")
            + _proc("?Other@@YIXXZ")
            + _bprel(-4, "elsewhere")
            + _record(SYM_END, b"")
        )
        out = format_frames(frame_names(_object(stream)), "Target")
        self.assertNotIn("Other", out)
        self.assertLess(out.index("shallow"), out.index("deep"))

    def test_inner_block_scope_does_not_drop_later_locals(self):
        # An inner `{ }` emits S_BLOCK32 + its own S_END. Naively popping the
        # scope stack on that S_END loses every local declared afterwards.
        stream = struct.pack("<I", 1) + (
            _proc("?Target@@YIXXZ")
            + _bprel(-4, "before")
            + _record(SYM_BLOCK32, b"\0" * 16)
            + _bprel(-8, "inner")
            + _record(SYM_END, b"")
            + _bprel(-12, "after")
            + _record(SYM_END, b"")
        )
        slots = frame_names(_object(stream))
        self.assertEqual(
            slots["?Target@@YIXXZ"],
            [(-4, "before"), (-8, "inner"), (-12, "after")],
        )

    def test_object_without_debug_records_yields_nothing(self):
        self.assertEqual(frame_names(_object(struct.pack("<I", 1))), {})


if __name__ == "__main__":
    unittest.main()
