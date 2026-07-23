import unittest
from unittest import mock

from homm2.init import main


class InitTest(unittest.TestCase):
    def test_redelinks_before_configuring_editor_tooling(self):
        with (mock.patch("homm2.init.redelink", return_value=0) as redelink,
              mock.patch("homm2.init.run", return_value=0) as run):
            self.assertEqual(main([]), 0)
        redelink.assert_called_once_with([])
        run.assert_called_once_with("python3", "-m", "homm2.init.clangd")


if __name__ == "__main__":
    unittest.main()
