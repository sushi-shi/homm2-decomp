import tempfile
import unittest
from pathlib import Path
from unittest import mock

from homm2 import cli


class CliTest(unittest.TestCase):
    def test_format_checks_headers_and_enum_sources(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            header = root / "include/Item.h"
            source = root / "src/Item.cpp"
            header.parent.mkdir()
            source.parent.mkdir()
            header.write_text("")
            source.write_text("")
            with mock.patch.object(cli, "REPO", root), mock.patch.object(
                    cli, "sh", return_value=0) as run:
                self.assertEqual(cli.main(["format", "--check"]), 0)
        self.assertEqual(run.call_args_list, [
            mock.call(
                "python3", "scripts/format_headers.py", "--check", header),
            mock.call(
                "python3", "scripts/format_enums.py", "--check", header, source),
        ])

    def test_format_rejects_unknown_arguments(self):
        with mock.patch.object(cli, "sh") as run:
            self.assertEqual(cli.main(["format", "--write"]), 1)
        run.assert_not_called()


if __name__ == "__main__":
    unittest.main()
