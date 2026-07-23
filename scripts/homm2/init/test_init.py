import unittest
from unittest import mock

from homm2.init import main


class InitTest(unittest.TestCase):
    def test_builds_candidate_objects_before_canonical_regeneration(self):
        calls = []

        def succeed(*command):
            calls.append(command)
            return 0

        with mock.patch("homm2.init.run", side_effect=succeed):
            self.assertEqual(main([]), 0)

        configure = ("python3", "configure.py")
        self.assertEqual(calls.count(configure), 2)
        self.assertLess(calls.index(configure), calls.index(("ninja", "base")))
        self.assertLess(
            calls.index(("ninja", "base")),
            calls.index(("python3", "-m", "homm2.build.reviewed_data",
                         "--regenerate")))


if __name__ == "__main__":
    unittest.main()
