import unittest

from historical_exact_losses import (
    Commit,
    ExactEpoch,
    hash_relation,
    loss_rows,
    parse_baseline,
)


class HistoricalExactLossesTest(unittest.TestCase):
    def test_parse_baseline_keeps_greatest_duplicate(self):
        rows = parse_baseline(
            "# comment\n"
            "SOURCE/U\tfn\t99.0000\thash-a\n"
            "SOURCE/U\tfn\t100.0000\thash-a\n"
        )
        self.assertEqual(rows[("SOURCE/U", "fn")], (100.0, "hash-a"))

    def test_parse_pre_hash_baseline_preserves_unknown_identity(self):
        rows = parse_baseline("SOURCE/U\tfn\t100.0000\n")
        self.assertEqual(rows[("SOURCE/U", "fn")], (100.0, ""))

    def test_hash_relation_distinguishes_dependency_upgrade(self):
        self.assertEqual(
            hash_relation("body.dep", {"body"}), "body-hash-upgrade"
        )
        self.assertEqual(
            hash_relation("body.dep", {"body.olddep"}), "changed-effective-hash"
        )
        self.assertEqual(
            hash_relation("body.dep", {"body.dep"}), "same-effective-hash"
        )
        self.assertEqual(
            hash_relation("body.dep", {""}), "unknown-historical-hash"
        )

    def test_loss_rows_omit_current_exact_and_sort_same_hash_first(self):
        old = Commit("1" * 40, "2026-01-01T00:00:00+00:00", "old")
        live = {
            ("SOURCE/U", "same"): {
                "name": "same", "size": "20", "fuzzy_match_percent": 99.0
            },
            ("SOURCE/U", "changed"): {
                "name": "changed", "size": "30", "fuzzy_match_percent": 99.9
            },
            ("SOURCE/U", "exact"): {
                "name": "exact", "size": "40", "fuzzy_match_percent": 100.0
            },
        }
        current = {
            ("SOURCE/U", "same"): (100.0, "same-hash"),
            ("SOURCE/U", "changed"): (99.9, "new-hash"),
            ("SOURCE/U", "exact"): (100.0, "exact-hash"),
        }
        epochs = {
            ("SOURCE/U", "same"): [ExactEpoch("same-hash", old, old)],
            ("SOURCE/U", "changed"): [ExactEpoch("old-hash", old, old)],
            ("SOURCE/U", "exact"): [ExactEpoch("exact-hash", old, old)],
        }
        origins = {
            ("SOURCE/U", "same"): (old, (100.0, "older-same-hash")),
            ("SOURCE/U", "changed"): (old, (100.0, "old-hash")),
        }
        rows = loss_rows(live, current, epochs, {}, origins)
        self.assertEqual([row["symbol"] for row in rows], ["same", "changed"])
        self.assertEqual(rows[0]["relation"], "same-effective-hash")
        self.assertEqual(rows[1]["relation"], "changed-effective-hash")
        self.assertTrue(rows[0]["transition_from_exact"])
        self.assertTrue(rows[1]["transition_from_exact"])


if __name__ == "__main__":
    unittest.main()
