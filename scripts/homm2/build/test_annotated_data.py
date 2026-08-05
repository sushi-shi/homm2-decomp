import json
import tempfile
import unittest
from pathlib import Path
from unittest import mock

import clang.cindex as ci

from homm2.clang_options import ClangMode
from homm2.build.annotated_data import (
    AnnotatedDataDefinition,
    _clang_args,
    definitions_for_file,
    source_definitions,
)


def _var_cursor(path, start, end, *, spelling="value", mangled="?value@@3HA",
                size=4, storage=ci.StorageClass.NONE):
    cursor = mock.Mock()
    cursor.kind = ci.CursorKind.VAR_DECL
    cursor.is_definition.return_value = True
    cursor.location.file = str(path)
    cursor.location.line = 1
    cursor.extent.start.offset = start
    cursor.extent.end.offset = end
    cursor.spelling = spelling
    cursor.mangled_name = mangled
    cursor.type.get_size.return_value = size
    cursor.storage_class = storage
    cursor.semantic_parent = None
    return cursor


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

            args = _clang_args(repo, source, mode=ClangMode.RETAIL_ANALYSIS)

        self.assertIn(str(repo / "include"), args)
        self.assertIn(str(repo / "vendor/smacker"), args)
        self.assertIn("-std=c++98", args)

    def test_clang_args_select_strict_mode_explicitly(self):
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            source = repo / "src/SOURCE/UNIT.cpp"
            source.parent.mkdir(parents=True)
            source.touch()
            (repo / "include").mkdir()

            args = _clang_args(repo, source, mode=ClangMode.STRICT)

        self.assertIn("-std=c++20", args)

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


class DefinitionsForFileTest(unittest.TestCase):
    def _definitions(self, text, **cursor_kwargs):
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            source_root = repo / "src"
            source = source_root / "SOURCE/UNIT.cpp"
            source.parent.mkdir(parents=True)
            source.write_text(text)
            start = text.index("int")
            translation = mock.Mock()
            translation.cursor.walk_preorder.return_value = [
                _var_cursor(source.resolve(), start, len(text) - 1,
                            **cursor_kwargs)]
            with mock.patch("homm2.build.annotated_data.ci.Index.create") as index:
                rows = definitions_for_file(source, source_root, repo, translation)
            return rows, index

    def test_the_decorated_linker_name_is_recorded(self):
        # The delinker and the relocation audits join on this spelling, so a
        # claim without it names nothing.
        rows, _index = self._definitions("DATA(0x00400100) int value;\n")
        self.assertEqual(
            [(r.rva, r.size, r.symbol, r.is_static) for r in rows],
            [(0x100, 4, "?value@@3HA", False)])

    def test_an_internal_linkage_object_keeps_its_underscore_spelling(self):
        rows, _index = self._definitions(
            "DATA(0x00400100) int value;\n", mangled="_value",
            storage=ci.StorageClass.STATIC)
        self.assertEqual([(r.symbol, r.is_static) for r in rows],
                         [("_value", True)])

    def test_a_supplied_translation_unit_is_not_reparsed(self):
        _rows, index = self._definitions("DATA(0x00400100) int value;\n")
        index.assert_not_called()


if __name__ == "__main__":
    unittest.main()
