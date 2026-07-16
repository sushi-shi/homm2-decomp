import json
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from homm2.build.annotated_data import (
    AnnotatedDataDefinition,
    _clang_args,
    source_definitions,
)


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

    def test_inventory_cache_tracks_source_and_candidate_object_content(self):
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            source_root = repo / "src"
            source = source_root / "SOURCE/UNIT.cpp"
            source.parent.mkdir(parents=True)
            include = repo / "include"
            include.mkdir()
            header = include / "layout.h"
            header.write_text("struct Layout { int value; };\n")
            source.write_text(
                "#include <layout.h>\nDATA(0x00400100) int value;\n")
            object_root = repo / "build/objdiff/base"
            object_path = object_root / "SOURCE/UNIT.obj"
            object_path.parent.mkdir(parents=True)
            object_path.write_bytes(b"object-v1")
            cache = repo / "build/gen/cache.json"
            row = AnnotatedDataDefinition(
                "SOURCE/UNIT", "value", "value", 0x100, 4,
                "src/SOURCE/UNIT.cpp:1", False)

            with mock.patch(
                    "homm2.build.annotated_data.definitions_for_file",
                    return_value=[row]) as parser:
                self.assertEqual(source_definitions(
                    source_root, repo, object_root, cache), [row])
                self.assertEqual(source_definitions(
                    source_root, repo, object_root, cache), [row])
                self.assertEqual(parser.call_count, 1)

                header.write_text("struct Layout { int value[2]; };\n")
                source_definitions(source_root, repo, object_root, cache)
                source.write_text(
                    "\n#include <layout.h>\nDATA(0x00400100) int value;\n")
                source_definitions(source_root, repo, object_root, cache)
                object_path.write_bytes(b"object-v2")
                source_definitions(source_root, repo, object_root, cache)
                self.assertEqual(parser.call_count, 4)


if __name__ == "__main__":
    unittest.main()
