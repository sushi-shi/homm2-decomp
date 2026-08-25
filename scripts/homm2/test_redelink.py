import unittest
from unittest import mock

from homm2.redelink import main


PIPELINE = [
    ("python3", "-m", "homm2.build.source_symbols", "--ignore-donations"),
    ("python3", "configure.py"),
    ("ninja", "base"),
    ("python3", "-m", "homm2.audit.reloc_donation"),
    ("python3", "-m", "homm2.build.source_symbols"),
    ("python3", "-m", "homm2.build.name_strings"),
    ("python3", "-m", "homm2.build.synth_pdb"),
    ("python3", "-m", "homm2.build.reviewed_data", "--regenerate"),
    ("python3", "configure.py"),
]


class RedelinkTest(unittest.TestCase):
    def test_runs_the_delink_pipeline_in_order(self):
        calls = []

        def succeed(*command):
            calls.append(command)
            return 0

        with mock.patch("homm2.redelink.run", side_effect=succeed):
            self.assertEqual(main([]), 0)
        self.assertEqual(calls, PIPELINE)

    def test_force_reaches_the_regenerate_step(self):
        calls = []

        def succeed(*command):
            calls.append(command)
            return 0

        with mock.patch("homm2.redelink.run", side_effect=succeed):
            self.assertEqual(main(["--force"]), 0)
        self.assertIn(
            ("python3", "-m", "homm2.build.reviewed_data", "--regenerate",
             "--force"),
            calls)

    def test_rejects_arguments_without_mutating_artifacts(self):
        with mock.patch("homm2.redelink.run") as run:
            self.assertEqual(main(["unexpected"]), 1)
        run.assert_not_called()


if __name__ == "__main__":
    unittest.main()
