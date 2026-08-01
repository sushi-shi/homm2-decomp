import io
import unittest
from contextlib import redirect_stdout
from pathlib import Path
from tempfile import TemporaryDirectory

from homm2.audit.ledger import classify, main, read_ledger

LEDGER = (
    "# banner comment\n"
    "#\n"
    "BASE/TILESET\t??0tileset@@QAE@K@Z\t100.0000\tde2c16211a22\n"
    "BASE/ICONWDGT\t?Main@iconWidget@@QAEXXZ\t98.8800\taaaa1111.bbbb\n"
    "\n"
)


class ReadLedgerTests(unittest.TestCase):
    def _ledger(self, directory, text=LEDGER):
        path = Path(directory) / "match_baseline.tsv"
        path.write_text(text)
        return path

    def test_banner_and_blank_lines_are_not_rows(self):
        with TemporaryDirectory() as directory:
            rows = read_ledger(self._ledger(directory))
        self.assertEqual(len(rows), 2)
        self.assertEqual(
            rows[("BASE/TILESET", "??0tileset@@QAE@K@Z")], (100.0, "de2c16211a22"))

    def test_composite_dependency_hash_is_read_whole(self):
        with TemporaryDirectory() as directory:
            rows = read_ledger(self._ledger(directory))
        _, banked = rows[("BASE/ICONWDGT", "?Main@iconWidget@@QAEXXZ")]
        self.assertEqual(banked, "aaaa1111.bbbb")


class ClassifyTests(unittest.TestCase):
    KEY = ("BASE/TILESET", "??0tileset@@QAE@K@Z")

    def test_matching_hash_is_current(self):
        stale, orphan = classify({self.KEY: (100.0, "abc")}, {self.KEY: "abc"})
        self.assertEqual((stale, orphan), ([], []))

    def test_changed_hash_is_stale_and_reports_both_hashes(self):
        # The real defect: 07442adb changed tileset's source after the row was
        # banked, so the score described an epoch that no longer existed.
        stale, orphan = classify({self.KEY: (100.0, "old")}, {self.KEY: "new"})
        self.assertEqual(orphan, [])
        self.assertEqual(stale, [(self.KEY, 100.0, "old", "new")])

    def test_function_absent_from_source_is_an_orphan_not_stale(self):
        stale, orphan = classify({self.KEY: (100.0, "old")}, {})
        self.assertEqual(stale, [])
        self.assertEqual(orphan, [(self.KEY, 100.0, "old")])

    def test_a_stale_row_is_reported_even_when_its_score_is_unchanged(self):
        # The TILESET row stayed 100.0000 across the refresh. Scoring identically
        # is not evidence the row is current, so the score must not gate this.
        stale, _ = classify({self.KEY: (100.0, "old")}, {self.KEY: "new"})
        self.assertTrue(stale)


class ExitStatusTests(unittest.TestCase):
    KEY = ("BASE/TILESET", "??0tileset@@QAE@K@Z")

    def _run(self, rows, live, argv=None):
        import homm2.audit.ledger as module
        original_read, original_hashes = module.read_ledger, module.source_hashes
        module.read_ledger = lambda path=None: rows
        module.source_hashes = lambda: live
        try:
            out = io.StringIO()
            with redirect_stdout(out):
                code = main(argv or [])
            return code, out.getvalue()
        finally:
            module.read_ledger, module.source_hashes = original_read, original_hashes

    def test_clean_ledger_passes(self):
        code, out = self._run({self.KEY: (100.0, "abc")}, {self.KEY: "abc"})
        self.assertEqual(code, 0)
        self.assertIn("1 current, 0 stale", out)

    def test_drift_fails_and_names_the_row(self):
        code, out = self._run({self.KEY: (100.0, "old")}, {self.KEY: "new"})
        self.assertEqual(code, 1)
        self.assertIn("STALE", out)
        self.assertIn("??0tileset@@QAE@K@Z", out)

    def test_quiet_reports_through_the_exit_status_only(self):
        code, out = self._run({self.KEY: (100.0, "old")}, {self.KEY: "new"}, ["--quiet"])
        self.assertEqual(code, 1)
        self.assertEqual(out, "")


if __name__ == "__main__":
    unittest.main()
