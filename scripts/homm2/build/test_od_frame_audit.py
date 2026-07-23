import unittest

from homm2.build.od_frame_audit import (
    compare_profiles,
    frame_profile,
    parse_disassembly,
)


class OdFrameAuditTests(unittest.TestCase):
    def test_parses_batched_functions_and_profiles_slots(self):
        text = """
00000000 <?one@@YAXXZ>:
       0: 55                           \tpush\tebp
       1: 8b ec                        \tmov\tebp, esp
       3: 83 ec 14                     \tsub\tesp, 0x14
       6: 53                           \tpush\tebx
       7: 8b 45 fc                     \tmov\teax, dword ptr [ebp - 0x4]
       a: 89 4d ec                     \tmov\tdword ptr [ebp - 0x14], ecx
00000010 <?two@@YAXXZ>:
      10: c3                           \tret
"""
        parsed = parse_disassembly(text, {"?one@@YAXXZ", "?two@@YAXXZ"})
        profile = frame_profile(parsed["?one@@YAXXZ"])
        self.assertEqual(profile.frame_size, 0x14)
        self.assertEqual(profile.saved_registers, ("ebx",))
        self.assertEqual(profile.local_references, (4, 0x14))
        self.assertEqual(profile.local_offsets, (4, 0x14))
        self.assertFalse(frame_profile(parsed["?two@@YAXXZ"]).has_frame_pointer)

    def test_recognizes_push_ecx_as_four_byte_frame(self):
        profile = frame_profile([
            "push ebp",
            "mov ebp, esp",
            "push ecx",
            "push esi",
            "mov dword ptr [ebp - 0x4], eax",
            "mov eax, dword ptr [ebp - 0xdfffff2]",
        ])
        self.assertEqual(profile.frame_size, 4)
        self.assertEqual(profile.saved_registers, ("esi",))
        self.assertEqual(profile.local_references, (4,))

    def test_distinguishes_frame_offsets_and_reference_order(self):
        target = frame_profile([
            "push ebp",
            "mov ebp, esp",
            "sub esp, 0x8",
            "mov eax, dword ptr [ebp - 0x4]",
            "mov ecx, dword ptr [ebp - 0x8]",
        ])
        short = frame_profile([
            "push ebp",
            "mov ebp, esp",
            "sub esp, 0x4",
            "mov eax, dword ptr [ebp - 0x4]",
        ])
        reordered = frame_profile([
            "push ebp",
            "mov ebp, esp",
            "sub esp, 0x8",
            "mov ecx, dword ptr [ebp - 0x8]",
            "mov eax, dword ptr [ebp - 0x4]",
        ])
        self.assertEqual(
            compare_profiles(short, target),
            ("frame-size", "local-offsets"))
        self.assertEqual(
            compare_profiles(reordered, target),
            ("local-reference-order",))


if __name__ == "__main__":
    unittest.main()
