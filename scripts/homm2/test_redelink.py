import unittest
from unittest import mock

from homm2.redelink import main


class RedelinkTest(unittest.TestCase):
    def test_refreshes_candidate_objects_before_vostok_regeneration(self):
        calls = []

        def succeed(*command):
            calls.append(command)
            return 0

        with mock.patch("homm2.redelink.run", side_effect=succeed):
            self.assertEqual(main([]), 0)

        configure = ("python3", "configure.py")
        regenerate = (
            "python3", "-m", "homm2.build.reviewed_data", "--regenerate")
        self.assertEqual(calls.count(configure), 2)
        self.assertLess(calls.index(configure), calls.index(("ninja", "base")))
        self.assertLess(calls.index(("ninja", "base")), calls.index(regenerate))
        self.assertLess(calls.index(regenerate), len(calls) - 1)

    def test_rejects_arguments_without_mutating_artifacts(self):
        with mock.patch("homm2.redelink.run") as run:
            self.assertEqual(main(["unexpected"]), 1)
        run.assert_not_called()
