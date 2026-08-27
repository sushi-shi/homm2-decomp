import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest


class ClangdRootTest(unittest.TestCase):
    def test_homm2_dir_selects_the_target_worktree(self):
        with tempfile.TemporaryDirectory() as directory:
            target = Path(directory).resolve()
            environment = os.environ.copy()
            environment["HOMM2_DIR"] = str(target)
            result = subprocess.run(
                [
                    sys.executable,
                    "-c",
                    "from homm2.init.clangd import REPO; print(REPO)",
                ],
                check=True,
                capture_output=True,
                env=environment,
                text=True,
            )

        self.assertEqual(result.stdout.strip(), str(target))


if __name__ == "__main__":
    unittest.main()
