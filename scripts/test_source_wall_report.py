import unittest

from source_wall_report import category, marker_kind


class SourceWallReportTests(unittest.TestCase):
    def test_all_durable_marker_kinds_are_classified(self):
        self.assertEqual(marker_kind("// @early-stop"), "early-stop")
        self.assertEqual(marker_kind("// @semantic"), "semantic")
        self.assertEqual(marker_kind("// @match-note"), "match-note")
        self.assertIsNone(marker_kind("// ordinary comment"))
        self.assertEqual(category({"marker": "early-stop"}), "accepted")
        self.assertEqual(category({"marker": "semantic"}), "provisional")
        self.assertEqual(category({"marker": "match-note"}), "provisional")
        self.assertEqual(category({"marker": None}), "unmarked")


if __name__ == "__main__":
    unittest.main()
