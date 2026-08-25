import struct
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest import mock

from homm2.build.fixed_asm import claims as fixed_asm_claims, unit as fixed_asm_unit
from homm2.build.symbol_providers import (
    _short_import_records,
    compiler_data_claims,
)


def short_import_archive(public, dll, value, name_type):
    payload = public.encode("ascii") + b"\0" + dll.encode("ascii") + b"\0"
    member = struct.pack(
        "<HHHHIIHH",
        0, 0xFFFF, 0, 0x14C, 0, len(payload), value, name_type << 2,
    ) + payload
    header = (
        f"{dll:<16}".encode("ascii")
        + b"0           "
        + b"0     "
        + b"0     "
        + b"100644  "
        + f"{len(member):<10}".encode("ascii")
        + b"`\n"
    )
    return b"!<arch>\n" + header + member + (b"\n" if len(member) & 1 else b"")


class FixedAsmTest(unittest.TestCase):
    def test_the_exception_table_contains_only_four_functions_and_tile_data(self):
        functions = list(fixed_asm_claims("func"))
        data = list(fixed_asm_claims("data"))
        self.assertEqual([row[2].name for row in functions], [
            "_BitTest", "_BitSet", "_BitClear", "_TileToBitmap",
        ])
        self.assertEqual(
            [(row[0], row[2].name, row[2].size) for row in data],
            [("BASE/TILE", "_gTileScratch", 8)],
        )

    def test_manifest_source_drift_is_rejected(self):
        with self.assertRaisesRegex(ValueError, "must use fixed MASM source"):
            fixed_asm_unit("BASE/BITS", "src/BASE/BITS.cpp")


class ImportProviderTest(unittest.TestCase):
    def test_ordinal_short_import_preserves_exact_public_spelling(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "WSOCK32.LIB"
            path.write_bytes(short_import_archive(
                "_sendto@24", "WSOCK32.dll", 20, 0,
            ))
            names, ordinals = _short_import_records(path, "wsock32.dll")
        self.assertEqual(names, {})
        self.assertEqual(ordinals, {20: "__imp__sendto@24"})

    def test_undecorated_lookup_keeps_stdcall_caller_spelling(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "KERNEL32.LIB"
            path.write_bytes(short_import_archive(
                "_CompareStringW@24", "KERNEL32.dll", 0, 3,
            ))
            names, ordinals = _short_import_records(path, "kernel32.dll")
        self.assertEqual(names, {"CompareStringW": "__imp__CompareStringW@24"})
        self.assertEqual(ordinals, {})


class CompilerDataProviderTest(unittest.TestCase):
    NAME = "??_B?1???id@?$ctype@G@std@@$D@@9@51"

    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.root = Path(self.temporary.name)
        self.base = self.root / "base"
        self.base.mkdir()
        (self.base / "UNIT.obj").write_bytes(b"placeholder")
        self.table = self.root / "compiler.tsv"
        self.table.write_text(
            "rva\tsize\tname\tclass\towner\n"
            f"0x139c80\t0x1\t{self.NAME}\tcommon\tstd::ctype<wchar_t>::id\n"
        )

    def tearDown(self):
        self.temporary.cleanup()

    def coff(self, name=None, size=1):
        data = bytearray(18)
        struct.pack_into("<HB", data, 14, 0, 2)
        symbol = SimpleNamespace(
            name=name or self.NAME, section=0, value=size, offset=0,
        )
        return SimpleNamespace(data=data, symbols={0: symbol})

    def test_current_common_emission_authorizes_the_reviewed_identity(self):
        with mock.patch(
            "homm2.build.symbol_providers.CoffFile", return_value=self.coff(),
        ):
            rows = compiler_data_claims(self.table, self.base)
        self.assertEqual(
            [(row.rva, row.size, row.name, row.provenance) for row in rows],
            [(0x139C80, 1, self.NAME, "reviewed-compiler-data:common")],
        )

    def test_missing_common_emission_fails_closed(self):
        with mock.patch(
            "homm2.build.symbol_providers.CoffFile",
            return_value=self.coff("?somethingElse@@3HA"),
        ):
            with self.assertRaisesRegex(ValueError, "not emitted"):
                compiler_data_claims(self.table, self.base)

    def test_wrong_common_size_fails_closed(self):
        with mock.patch(
            "homm2.build.symbol_providers.CoffFile", return_value=self.coff(size=4),
        ):
            with self.assertRaisesRegex(ValueError, "emits size"):
                compiler_data_claims(self.table, self.base)

    def test_compiler_local_label_cannot_become_a_provider_identity(self):
        self.table.write_text(
            "rva\tsize\tname\tclass\towner\n"
            "0x139c80\t0x1\t$continue_route$60490\tcommon\tcompiler-local\n"
        )
        with mock.patch(
            "homm2.build.symbol_providers.CoffFile",
            return_value=self.coff("$continue_route$60490"),
        ):
            with self.assertRaisesRegex(ValueError, "invalid compiler-data row"):
                compiler_data_claims(self.table, self.base)


if __name__ == "__main__":
    unittest.main()
