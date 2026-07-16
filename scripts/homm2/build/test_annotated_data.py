import json
import tempfile
import unittest
from pathlib import Path

from homm2.build.annotated_data import _clang_args


class AnnotatedDataTest(unittest.TestCase):
    def test_clang_args_include_vendor_headers_with_stale_database(self):
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            source = repo / "src/SOURCE/UNIT.cpp"
            source.parent.mkdir(parents=True)
            source.touch()
            (repo / "include").mkdir()
            (repo / "vendor/smacker").mkdir(parents=True)
            database = repo / "build/clangd/compile_commands.json"
            database.parent.mkdir(parents=True)
            database.write_text(json.dumps([{
                "directory": str(repo),
                "file": str(source),
                "arguments": ["clang-cl", "/c", str(source), "/I", str(repo / "include")],
            }]))

            args = _clang_args(repo, source)

        self.assertIn(str(repo / "include"), args)
        self.assertIn(str(repo / "vendor/smacker"), args)


if __name__ == "__main__":
    unittest.main()
