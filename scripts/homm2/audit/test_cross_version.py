import io
import json
import unittest
from contextlib import redirect_stdout
from pathlib import Path
from tempfile import TemporaryDirectory

from homm2.audit.cross_version import (
    ReferenceError,
    best_known,
    build_rows,
    check_pairing,
    find_reference,
    percentile,
    read_ledger,
    read_profiles,
    read_report,
    read_symbols,
    read_va_queue,
    size_envelope,
    summarise,
    transition,
)

KEY = ("SOURCE/GAME", "?GetLuck@game@@QAEHPAVhero@@@Z")


def write_tree(root, *, report, ledger="", symbols="", units="", va_queue=None):
    root = Path(root)
    (root / "build/objdiff").mkdir(parents=True, exist_ok=True)
    (root / "config").mkdir(parents=True, exist_ok=True)
    (root / "build/gen").mkdir(parents=True, exist_ok=True)
    (root / "build/objdiff/report.json").write_text(json.dumps(report))
    (root / "config/match_baseline.tsv").write_text(ledger)
    (root / "build/gen/symbol_names.csv").write_text(
        "rva,name,unit,size,kind,provenance\n" + symbols)
    (root / "config/units.toml").write_text(units)
    if va_queue is not None:
        (root / "docs").mkdir(parents=True, exist_ok=True)
        (root / "docs/buka-va-queue.tsv").write_text(
            "unit\tbuka_rva\tsize\tinventory\tpol_va\tsignature\n" + va_queue)
    return root


def report_of(*functions, unit="SOURCE/GAME"):
    return {"units": [{"name": unit, "functions": list(functions)}], "version": 1}


class ReadReportTests(unittest.TestCase):
    def test_missing_fuzzy_percent_is_zero_not_missing_data(self):
        # objdiff omits the key entirely when nothing matched. Reading that as
        # absent would drop the worst functions out of the queue silently.
        with TemporaryDirectory() as directory:
            root = write_tree(directory, report=report_of(
                {"name": "?a@@YIXXZ", "size": "100"}))
            rows = read_report(root)
        self.assertEqual(rows[("SOURCE/GAME", "?a@@YIXXZ")], (0.0, 100))

    def test_parenthesised_modules_are_not_reconstruction_targets(self):
        with TemporaryDirectory() as directory:
            report = {"units": [
                {"name": "(libcmt)", "functions": [{"name": "_memcpy", "size": "9"}]},
                {"name": "SOURCE/GAME", "functions": [{"name": "?a@@YIXXZ", "size": "9"}]},
            ]}
            rows = read_report(write_tree(directory, report=report))
        self.assertEqual(list(rows), [("SOURCE/GAME", "?a@@YIXXZ")])


class ReadProfilesTests(unittest.TestCase):
    def test_unit_without_explicit_flags_takes_the_base_profile(self):
        # The build defaults these to base; a "?" here would misreport the
        # transition class for every unit that never needed an override.
        with TemporaryDirectory() as directory:
            root = write_tree(directory, report=report_of(), units=(
                '[[unit]]\nunit = "BASE/A"\nsource = "a.cpp"\n\n'
                '[[unit]]\nunit = "BASE/B"\nsource = "b.cpp"\nflags = "o2"\n'))
            profiles = read_profiles(root)
        self.assertEqual(profiles, {"BASE/A": "base", "BASE/B": "o2"})


class BestKnownTests(unittest.TestCase):
    def test_a_banked_maximum_counts_when_it_beats_the_live_score(self):
        # 2.0's headline number uses maxima, so parity has to be measured the
        # same way or the gap is overstated.
        self.assertEqual(best_known(KEY, {KEY: (91.0, 10)}, {KEY: 100.0}), 100.0)

    def test_live_score_counts_when_it_beats_the_banked_maximum(self):
        self.assertEqual(best_known(KEY, {KEY: (100.0, 10)}, {KEY: 91.0}), 100.0)

    def test_function_in_neither_channel_has_no_evidence(self):
        self.assertIsNone(best_known(KEY, {}, {}))


class TransitionTests(unittest.TestCase):
    def test_od_on_both_sides_transfers_shape(self):
        self.assertEqual(transition("base", "base_nogf"), "od-od")

    def test_o2_to_od_is_the_class_where_the_proof_is_weaker(self):
        self.assertEqual(transition("o2", "base"), "o2-od")

    def test_unknown_unit_does_not_masquerade_as_a_class(self):
        self.assertEqual(transition(None, "base"), "unknown")


class PercentileTests(unittest.TestCase):
    def test_empty_population_has_no_percentile(self):
        self.assertIsNone(percentile([], 0.5))

    def test_top_fraction_never_indexes_past_the_end(self):
        self.assertEqual(percentile([1, 2, 3], 1.0), 3)


class BuildRowsTests(unittest.TestCase):
    def _join(self, buka_score, pol_score, *, buka_max=None, pol_max=None,
              units='[[unit]]\nunit = "SOURCE/GAME"\nflags = "base"\n'):
        name = KEY[1]
        buka = ({KEY: (buka_score, 100)}, {KEY: buka_max} if buka_max else {},
                {KEY: (0x1000, 0x80)}, read_profiles_from(units))
        pol = ({KEY: (pol_score, 100)}, {KEY: pol_max} if pol_max else {},
               {KEY: (0x2000, 0x80)}, read_profiles_from(units))
        rows, envelope = build_rows(buka, pol)
        return rows[0], envelope

    def test_two_zero_exact_and_here_not_is_the_parity_gap(self):
        row, _ = self._join(91.0, 100.0)
        self.assertEqual(row["lead"], "parity-gap")

    def test_banked_maximum_on_the_two_zero_side_still_opens_a_gap(self):
        # 118 of the real gap rows are exactly this: 2.0's live source no longer
        # reproduces the score, but it banked it, so parity is still owed.
        row, _ = self._join(91.0, 88.0, pol_max=100.0)
        self.assertEqual(row["lead"], "parity-gap")

    def test_exact_here_but_not_on_two_zero_is_forward_progress(self):
        row, _ = self._join(100.0, 91.0)
        self.assertEqual(row["lead"], "forward")

    def test_unmatched_bytes_weight_the_fuzzy_queue(self):
        # 100 bytes at 75% leaves 25 unmatched - the number that ranks the
        # percentage queue, which orders differently from the function count.
        row, _ = self._join(75.0, 100.0)
        self.assertEqual(row["unmatched_bytes"], 25)

    def test_size_delta_is_relative_to_the_two_zero_body(self):
        name = KEY[1]
        units = '[[unit]]\nunit = "SOURCE/GAME"\nflags = "base"\n'
        buka = ({KEY: (91.0, 100)}, {}, {KEY: (0x1000, 80)}, read_profiles_from(units))
        pol = ({KEY: (100.0, 100)}, {}, {KEY: (0x2000, 100)}, read_profiles_from(units))
        rows, _ = build_rows(buka, pol)
        self.assertEqual(rows[0]["size_delta"], -20)
        self.assertAlmostEqual(rows[0]["size_rel"], -0.2)


class SizeEnvelopeTests(unittest.TestCase):
    def _rows(self, count, lead="both-exact", rel=0.0):
        return [{"lead": lead, "transition": "od-od", "size_rel": rel + i * 1e-6}
                for i in range(count)]

    def test_a_class_with_too_few_samples_is_left_uncalibrated(self):
        # Flagging outliers off a handful of points would invent findings.
        self.assertEqual(size_envelope(self._rows(5)), {})

    def test_only_both_exact_rows_calibrate_the_band(self):
        # Same source matched in both images: every byte of delta there is
        # compiler noise, so it is the only honest baseline.
        rows = self._rows(30) + self._rows(30, lead="parity-gap", rel=9.0)
        low, high = size_envelope(rows)["od-od"]
        self.assertLess(high, 1.0)


class CheckPairingTests(unittest.TestCase):
    def _row(self, pol_rva):
        return {"unit": "SOURCE/GAME", "symbol": KEY[1],
                "buka_rva": 0x1000, "pol_rva": pol_rva}

    def test_name_join_agreeing_with_the_address_remap_is_silent(self):
        queue = {("SOURCE/GAME", 0x1000): 0x402000}
        self.assertEqual(check_pairing([self._row(0x2000)], queue, {}), [])

    def test_disagreement_is_reported_because_one_claim_must_be_wrong(self):
        queue = {("SOURCE/GAME", 0x1000): 0x409999}
        self.assertEqual(len(check_pairing([self._row(0x2000)], queue, {})), 1)

    def test_function_absent_from_the_remap_is_not_a_conflict(self):
        self.assertEqual(check_pairing([self._row(0x2000)], {}, {}), [])


class ReadVaQueueTests(unittest.TestCase):
    def test_rows_without_a_pol_address_are_buka_new_not_errors(self):
        with TemporaryDirectory() as directory:
            root = write_tree(directory, report=report_of(), va_queue=(
                "BASE/A\t0xcc740\t0x162\texact\t\tvoid f(void)\n"
                "BASE/B\t0xcc8b0\t0x3a\texact\t0x4012c0\tvoid g(void)\n"))
            rows = read_va_queue(root)
        self.assertEqual(rows, {("BASE/B", 0xcc8b0): 0x4012c0})


class FindReferenceTests(unittest.TestCase):
    def test_a_path_without_a_report_is_rejected_by_name(self):
        with TemporaryDirectory() as directory:
            with self.assertRaises(ReferenceError) as caught:
                find_reference(directory)
        self.assertIn("provisioned", str(caught.exception))


class SummariseTests(unittest.TestCase):
    def test_summary_names_the_gap_and_its_transition_split(self):
        rows = [
            {"lead": "parity-gap", "transition": "od-od", "buka_best": 91.0,
             "unmatched_bytes": 10, "size_outlier": ""},
            {"lead": "parity-gap", "transition": "o2-od", "buka_best": 50.0,
             "unmatched_bytes": 5, "size_outlier": "yes"},
        ]
        out = io.StringIO()
        with redirect_stdout(out):
            summarise(rows, {}, stream=out)
        text = out.getvalue()
        self.assertIn("parity-gap             2", text)
        self.assertIn("od-od 1", text)
        self.assertIn("o2-od 1", text)


def read_profiles_from(units):
    with TemporaryDirectory() as directory:
        root = write_tree(directory, report={"units": []}, units=units)
        return read_profiles(root)


if __name__ == "__main__":
    unittest.main()
