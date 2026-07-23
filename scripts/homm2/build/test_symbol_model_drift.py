import unittest
from collections import Counter
from contextlib import redirect_stdout
from io import StringIO
from unittest import mock

from homm2.build.symbol_model_drift import _counter_drift, main


class SymbolModelDriftTest(unittest.TestCase):
    def test_reports_both_unmodeled_and_no_longer_emitted_identities(self):
        diagnostics = _counter_drift(
            "SOURCE/Test data", Counter({"new": 1}), Counter({"old": 1}))
        self.assertEqual(len(diagnostics), 2)
        self.assertIn("modeled identities not emitted", diagnostics[0])
        self.assertIn("emitted identities are not modeled (new)", diagnostics[1])

    def test_exact_inventory_has_no_drift(self):
        inventory = Counter({"one": 1, "alias": 2})
        self.assertEqual(_counter_drift("data", inventory, inventory), [])

    def test_drift_warns_without_failing(self):
        output = StringIO()
        with (mock.patch(
                "homm2.build.symbol_model_drift.audit_symbol_model",
                return_value=["SOURCE/Test: new compiler data"]),
              redirect_stdout(output)):
            self.assertEqual(main([]), 0)
        self.assertIn("WARN", output.getvalue())
        self.assertIn("homm2 redelink", output.getvalue())


if __name__ == "__main__":
    unittest.main()
