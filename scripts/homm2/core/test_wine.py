import os
import unittest
from pathlib import Path
from tempfile import TemporaryDirectory
from unittest import mock

from homm2.core import wine


class PrepareEnvironmentTest(unittest.TestCase):
    def test_empty_prefix_does_not_mean_the_current_directory(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            prefix = root / "build/wineprefix"
            prefix.mkdir(parents=True)
            with mock.patch.object(wine, "REPO", root), \
                    mock.patch.object(wine.shutil, "which", return_value="wine"), \
                    mock.patch.object(wine, "ensure_wineserver"), \
                    mock.patch.dict(os.environ, {"WINEPREFIX": ""}):
                wine._ENV_READY = False
                wine.prepare_env()
                self.assertEqual(os.environ["WINEPREFIX"], str(prefix))
        wine._ENV_READY = False


class RunTest(unittest.TestCase):
    def test_captures_output_without_a_pipe(self):
        def complete(_command, **kwargs):
            self.assertIsNot(kwargs["stdout"], wine.subprocess.PIPE)
            kwargs["stdout"].write(b"tool output\n")
            return wine.subprocess.CompletedProcess([], 0)

        with mock.patch.object(wine, "prepare_env"), \
                mock.patch.object(wine, "child_env", return_value={}), \
                mock.patch.object(wine.subprocess, "run", side_effect=complete):
            self.assertEqual(wine.run(Path("TOOL.EXE"), quiet=True),
                             "tool output\n")


if __name__ == "__main__":
    unittest.main()
