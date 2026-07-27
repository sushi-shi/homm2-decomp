import contextlib
import io
import json
import os
import subprocess
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

from homm2.build.clang_cxx11 import main


class ClangCxx11Test(unittest.TestCase):
    def run_check(self, returncode, diagnostics):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            database = root / "build/clangd/compile_commands.json"
            database.parent.mkdir(parents=True)
            database.write_text(json.dumps([{"file": "src/test.cpp", "arguments": []}]))
            previous = Path.cwd()
            output = io.StringIO()
            try:
                os.chdir(root)
                with patch(
                    "homm2.build.clang_cxx11.subprocess.run",
                    return_value=subprocess.CompletedProcess(
                        [], returncode, stdout="", stderr=diagnostics
                    ),
                ), contextlib.redirect_stdout(output):
                    result = main(["--errors-only"])
            finally:
                os.chdir(previous)
        return result, output.getvalue()

    def test_errors_only_suppresses_warnings_from_successful_units(self):
        result, output = self.run_check(0, "test.cpp: warning: expected warning\n")

        self.assertEqual(result, 0)
        self.assertNotIn("expected warning", output)
        self.assertIn("failed: 0", output)
        self.assertIn("warnings: 1", output)

    def test_errors_only_keeps_diagnostics_from_failed_units(self):
        result, output = self.run_check(1, "test.cpp: error: expected error\n")

        self.assertEqual(result, 1)
        self.assertIn("expected error", output)
        self.assertIn("failed: 1", output)


if __name__ == "__main__":
    unittest.main()
