import struct
import unittest

from homm2.build.gen_vendor_imports import (
    ADVAPI_IMPORTS,
    MSS_IMPORTS,
    SMACK_IMPORTS,
    WING_IMPORTS,
    _patch_linker_names,
    _patch_short_import_member,
    ImportSpec,
    import_specs,
    parse_archive_members,
)


class VendorImportTests(unittest.TestCase):
    def test_retail_import_inventory(self):
        self.assertEqual(len(MSS_IMPORTS), 33)
        self.assertEqual({ordinal for _, ordinal in SMACK_IMPORTS},
                         {14, 18, 19, 20, 21, 23, 28, 32, 33, 38})
        self.assertEqual(len(WING_IMPORTS), 6)
        self.assertEqual(
            [lookup for _, _, lookup in ADVAPI_IMPORTS],
            ["RegOpenKeyExA", "RegSetValueExA", "RegCreateKeyA",
             "RegQueryValueExA", "RegCloseKey"],
        )

    def test_each_dll_uses_its_retail_import_form(self):
        specs = import_specs()
        self.assertTrue(all(not spec.noname and spec.lookup_name is None
                            for spec in specs if spec.dll == "mss32.dll"))
        self.assertTrue(all(spec.noname
                            for spec in specs if spec.dll == "smackw32.DLL"))
        self.assertTrue(all(not spec.noname and spec.lookup_name
                            for spec in specs if spec.dll == "WING32.dll"))
        self.assertTrue(all(not spec.noname and spec.lookup_name
                            for spec in specs if spec.dll == "ADVAPI32.dll"))

    def test_archive_parser_skips_linker_members(self):
        def member(name, payload):
            header = (name.ljust(16) + "0".ljust(12) + "0".ljust(6) +
                      "0".ljust(6) + "0".ljust(8) + str(len(payload)).ljust(10) + "`\n")
            return header.encode("ascii") + payload + (b"\n" if len(payload) & 1 else b"")

        archive = b"!<arch>\n" + member("/", b"index") + member("object/", b"coff")
        self.assertEqual(parse_archive_members(archive), [b"coff"])

    def test_second_linker_member_is_resorted_after_rename(self):
        names = ["__IMPORT_DESCRIPTOR_smackw32", "__SmackOpen@12",
                 "__imp___SmackOpen@12"]
        payload = bytearray()
        payload.extend(struct.pack("<I3I", 3, 100, 200, 300))
        payload.extend(struct.pack("<I3H", 3, 1, 2, 3))
        payload.extend(b"".join(name.encode("ascii") + b"\0" for name in names))
        patched = _patch_linker_names(
            bytes(payload),
            {"__SmackOpen@12": "_SmackOpen@12",
             "__imp___SmackOpen@12": "__imp__SmackOpen@12"},
            second=True,
        )
        indices = struct.unpack_from("<3H", patched, 20)
        cursor = 26
        patched_names = []
        for _ in range(3):
            end = patched.index(0, cursor)
            patched_names.append(patched[cursor:end].decode("ascii"))
            cursor = end + 1
        self.assertEqual(indices, (2, 1, 3))
        self.assertEqual(patched_names,
                         ["_SmackOpen@12", "__IMPORT_DESCRIPTOR_smackw32",
                          "__imp__SmackOpen@12"])

    def test_short_import_member_preserves_caller_and_undecorates_lookup(self):
        payload = b"__Netbios@4\0NETAPI32.dll\0"
        member = struct.pack(
            "<HHHHIIHH", 0, 0xFFFF, 0, 0x14C, 1234, len(payload), 0, 8
        ) + payload
        patched, is_function = _patch_short_import_member(
            member,
            (ImportSpec("NETAPI32.dll", "_Netbios@4", 180,
                        lookup_name="Netbios"),),
        )
        self.assertTrue(is_function)
        self.assertEqual(struct.unpack_from("<I", patched, 8)[0], 0)
        self.assertEqual(struct.unpack_from("<H", patched, 16)[0], 180)
        self.assertEqual((struct.unpack_from("<H", patched, 18)[0] >> 2) & 7, 3)
        size = struct.unpack_from("<I", patched, 12)[0]
        self.assertEqual(patched[20 : 20 + size],
                         b"_Netbios@4\0NETAPI32.dll\0")


if __name__ == "__main__":
    unittest.main()
