import json
import stat
import subprocess
import unittest
from pathlib import Path
from tempfile import TemporaryDirectory
from unittest import mock

from homm2.build.data_manifest_adapter import SYMBOL_HEADER
from homm2.build.reviewed_data import (
    _expected_delink_objects,
    data_manifest_stub_bytes,
    ensure_reviewed_targets,
    regenerate_targets,
    reviewed_manifest_bytes,
)


FAKE_DELINKER = """#!/bin/sh
out=""
while [ $# -gt 0 ]; do
    case "$1" in --output-path) out="$2";; esac
    shift
done
%s
touch "$out/(unmatched).c.obj"
"""


def write_fake_delinker(path, extra="", fail=False):
    body = FAKE_DELINKER % extra
    if fail:
        body = "#!/bin/sh\nexit 1\n"
    path.write_text(body)
    path.chmod(path.stat().st_mode | stat.S_IXUSR)
    return path


class Fixture:
    """A complete fake input tree for regenerate/ensure round trips."""

    def __init__(self, root):
        self.root = root
        self.exe = self._file("game.exe", b"exe")
        self.pdb = self._file("game.pdb", b"pdb")
        self.symbols = self._file(
            "symbol_names.csv", b"rva,name,unit,size,kind,provenance\n")
        self.retail_functions = self._file(
            "retail_functions.csv",
            b"entry_rva,byte_size,name,thunk,chunks,max_gap,total_gap,extent\n"
            b"0x1000,16,FUN_00401000,0,1,0,0,16\n")
        self.reloc_manifest = self._file("delink_relocs.tsv", b"site_rva\tkind\n")
        self.reloc_aliases = self._file(
            "delink_reloc_aliases.tsv",
            b"function_rva\ttarget_rva\tsite_rva\towner\taddend\toccurrences\n")
        self.ledger = self._file(
            "ledger.tsv", b"name\tunit\tsize\tretail_sha256\thighlow_count\taudit\n")
        self.owner_extents = self._file("reloc_data_owners.tsv", b"# none\n")
        self.manifest = root / "gen/reviewed_delink_data.tsv"
        self.data_manifest = root / "gen/delink_data_manifest.tsv"
        self.target = root / "delink"
        self.stamp = self.target / ".reviewed-data-stamp.json"
        self.delinker = write_fake_delinker(root / "vostok-delinker")

    def _file(self, name, payload):
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(payload)
        return path

    def patches(self):
        module = "homm2.build.reviewed_data"
        return (
            mock.patch(f"{module}.REPO", self.root),
            mock.patch(f"{module}.EXE", self.exe),
            mock.patch(f"{module}.PDB", self.pdb),
            mock.patch(f"{module}.SYMBOLS", self.symbols),
            mock.patch(f"{module}.RETAIL_FUNCTIONS", self.retail_functions),
            mock.patch(f"{module}.RELOC_MANIFEST", self.reloc_manifest),
            mock.patch(f"{module}.RELOC_ALIASES", self.reloc_aliases),
            mock.patch(f"{module}.LEDGER", self.ledger),
            mock.patch(f"{module}.OWNER_EXTENTS", self.owner_extents),
            mock.patch(f"{module}.MANIFEST", self.manifest),
            mock.patch(f"{module}.DATA_MANIFEST", self.data_manifest),
            mock.patch(f"{module}.TARGET", self.target),
            mock.patch(f"{module}.STAMP", self.stamp),
            mock.patch(f"{module}.load_definition_rvas", return_value={}),
            mock.patch(f"{module}.load_explicit_extents", return_value={}),
            mock.patch(f"{module}.load_owner_ranges", return_value=[]),
            # The real function's defaults bind the repo's own paths at def
            # time; the fixture wants a hermetic, deterministic manifest.
            mock.patch(f"{module}.reviewed_manifest_bytes",
                       return_value=b"# fixture manifest\n"),
        )

    def run(self, action, *args, **kwargs):
        stack = self.patches()
        for patch in stack:
            patch.start()
        try:
            return action(*args, **kwargs)
        finally:
            for patch in stack:
                patch.stop()


class ReviewedDataTest(unittest.TestCase):
    def test_ledger_rejects_raw_padding_boundary_crossing(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            symbols = root / "symbols.csv"
            symbols.write_text(
                "name,unit,rva,kind,provenance\n"
                "global,SOURCE/Test,0x300c,data,source-annotation\n")
            ledger = root / "ledger.tsv"
            ledger.write_text(
                "name\tunit\tsize\tretail_sha256\thighlow_count\taudit\n"
                "global\tSOURCE/Test\t0x8\tdeadbeef\t0\tbytes\n")
            pe = {"sections": {
                ".data": {"rva": 0x3000, "raw_size": 0x10,
                          "virtual_size": 0x30},
            }}
            with (mock.patch("homm2.build.reviewed_data.read_pe", return_value=pe),
                  mock.patch("homm2.build.reviewed_data.load_owner_ranges",
                             return_value=[]),
                  mock.patch("homm2.build.reviewed_data.read_pe_payload_evidence",
                             side_effect=AssertionError(
                                 "crossing allocation must fail before payload review"))):
                with self.assertRaisesRegex(
                        RuntimeError,
                        "crosses retail storage data-initialized -> "
                        "data-loader-zero-tail"):
                    reviewed_manifest_bytes(
                        symbols=symbols, ledger=ledger, exe=root / "game.exe")

    def test_ledger_accepts_source_annotation_rows(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            symbols = root / "symbols.csv"
            symbols.write_text(
                "name,unit,rva,kind,provenance\n"
                "?gTable@@3PAHA,SOURCE/Test,0x3000,data,source-annotation\n")
            ledger = root / "ledger.tsv"
            ledger.write_text(
                "name\tunit\tsize\tretail_sha256\thighlow_count\taudit\n"
                "?gTable@@3PAHA\tSOURCE/Test\t0x8\tfeedface\t0\tbytes\n")
            pe = {"sections": {
                ".data": {"rva": 0x3000, "raw_size": 0x10,
                          "virtual_size": 0x10},
            }}
            evidence = {"sha256": "feedface", "highlow_base_relocation_count": 0}
            with (mock.patch("homm2.build.reviewed_data.read_pe", return_value=pe),
                  mock.patch("homm2.build.reviewed_data.load_owner_ranges",
                             return_value=[]),
                  mock.patch("homm2.build.reviewed_data.read_pe_payload_evidence",
                             return_value=evidence)):
                payload = reviewed_manifest_bytes(
                    symbols=symbols, ledger=ledger, exe=root / "game.exe")
        self.assertIn(b"?gTable@@3PAHA\tSOURCE\\Test.c\t0x3000\t0x8\tdata", payload)

    def test_data_manifest_stub_carries_the_adapter_header(self):
        lines = data_manifest_stub_bytes().decode().splitlines()
        self.assertEqual(lines[-1], "\t".join(SYMBOL_HEADER))
        self.assertTrue(all(line.startswith("#") for line in lines[:-1]))

    def test_expected_objects_derive_claimed_units_and_unmatched(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            symbols = root / "symbols.csv"
            symbols.write_text(
                "rva,name,unit,size,kind,provenance\n"
                "0x1000,?Fn@@YIXXZ,SOURCE/AGG,0x10,func,source-annotation\n")
            retail = root / "retail.csv"
            retail.write_text(
                "# comment\n"
                "entry_rva,byte_size,name,thunk,chunks,max_gap,total_gap,extent\n"
                "0x1000,16,FUN_00401000,0,1,0,0,16\n"
                "0x2000,32,FUN_00402000,0,1,0,0,32\n")
            expected = _expected_delink_objects(symbols, retail)
        self.assertEqual(expected, {"SOURCE/AGG.c.obj", "(unmatched).c.obj"})

    def test_expected_objects_without_unclaimed_rows_has_no_unmatched(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            symbols = root / "symbols.csv"
            symbols.write_text(
                "rva,name,unit,size,kind,provenance\n"
                "0x1000,?Fn@@YIXXZ,SOURCE/AGG,0x10,func,source-annotation\n")
            retail = root / "retail.csv"
            retail.write_text(
                "entry_rva,byte_size,name,thunk,chunks,max_gap,total_gap,extent\n"
                "0x1000,16,FUN_00401000,0,1,0,0,16\n")
            expected = _expected_delink_objects(symbols, retail)
        self.assertEqual(expected, {"SOURCE/AGG.c.obj"})

    def test_regenerate_stamps_and_ensure_round_trips(self):
        with TemporaryDirectory() as directory:
            fixture = Fixture(Path(directory))
            fixture.run(regenerate_targets, delinker=fixture.delinker)
            self.assertTrue((fixture.target / "(unmatched).c.obj").is_file())
            self.assertTrue(fixture.manifest.is_file())
            self.assertTrue(fixture.data_manifest.is_file())
            stamp = json.loads(fixture.stamp.read_text())
            self.assertEqual(sorted(stamp), [
                "delinker_sha256", "exe_sha256", "generator_sha256",
                "ledger_sha256", "manifest_sha256", "owner_definitions_sha256",
                "owner_extents_sha256", "pdb_sha256", "reloc_aliases_sha256",
                "reloc_manifest_sha256", "retail_functions_sha256",
                "schema", "symbols_sha256"])
            self.assertFalse(
                fixture.run(ensure_reviewed_targets, delinker=fixture.delinker))

    def test_regenerate_is_a_noop_when_the_stamp_matches(self):
        with TemporaryDirectory() as directory:
            fixture = Fixture(Path(directory))
            fixture.run(regenerate_targets, delinker=fixture.delinker)
            marker = fixture.target / "(unmatched).c.obj"
            marker.write_bytes(b"kept")
            fixture.run(regenerate_targets, delinker=fixture.delinker)
            self.assertEqual(marker.read_bytes(), b"kept")
            fixture.run(regenerate_targets, delinker=fixture.delinker, force=True)
            self.assertEqual(marker.read_bytes(), b"")

    def test_ensure_names_the_changed_input(self):
        with TemporaryDirectory() as directory:
            fixture = Fixture(Path(directory))
            fixture.run(regenerate_targets, delinker=fixture.delinker)
            fixture.reloc_manifest.write_bytes(
                b"site_rva\tkind\n0x2000\tdir32\n")
            with self.assertRaisesRegex(
                    RuntimeError, r"reloc_manifest_sha256.*homm2 redelink"):
                fixture.run(ensure_reviewed_targets, delinker=fixture.delinker)

    def test_ensure_requires_target_and_stamp(self):
        with TemporaryDirectory() as directory:
            fixture = Fixture(Path(directory))
            with self.assertRaisesRegex(RuntimeError, "run `homm2 redelink`"):
                fixture.run(ensure_reviewed_targets, delinker=fixture.delinker)
            fixture.target.mkdir()
            with self.assertRaisesRegex(RuntimeError, "no valid provenance stamp"):
                fixture.run(ensure_reviewed_targets, delinker=fixture.delinker)

    def test_regenerate_restores_the_backup_when_the_delinker_fails(self):
        with TemporaryDirectory() as directory:
            fixture = Fixture(Path(directory))
            fixture.run(regenerate_targets, delinker=fixture.delinker)
            marker = fixture.target / "(unmatched).c.obj"
            marker.write_bytes(b"previous target")
            failing = write_fake_delinker(
                fixture.root / "failing-delinker", fail=True)
            # A changed input forces a real regeneration attempt.
            fixture.reloc_manifest.write_bytes(b"site_rva\tkind\n0x2000\tdir32\n")
            with self.assertRaises(subprocess.CalledProcessError):
                fixture.run(regenerate_targets, delinker=failing)
            self.assertEqual(marker.read_bytes(), b"previous target")

    def test_regenerate_rejects_a_delink_missing_expected_objects(self):
        with TemporaryDirectory() as directory:
            fixture = Fixture(Path(directory))
            empty = fixture.root / "empty-delinker"
            empty.write_text("#!/bin/sh\nexit 0\n")
            empty.chmod(empty.stat().st_mode | stat.S_IXUSR)
            with self.assertRaisesRegex(RuntimeError, "delink omitted expected"):
                fixture.run(regenerate_targets, delinker=empty)
            self.assertFalse(fixture.target.exists())


if __name__ == "__main__":
    unittest.main()
