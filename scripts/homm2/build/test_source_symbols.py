import unittest
from pathlib import Path
from tempfile import TemporaryDirectory
from unittest import mock

from homm2.build import source_symbols as mod
from homm2.build.annotated_data import AnnotatedDataDefinition
from homm2.build.source_symbols import (
    SourceSymbol,
    collect,
    compgen_functions_for_file,
    render,
    render_compgen,
)


def sym(rva, name, unit="BASE/X", size=0x10, kind="func", prov="source-annotation"):
    return SourceSymbol(rva=rva, name=name, unit=unit, size=size, kind=kind,
                        provenance=prov)


def data_definition(symbol="?value@@3HA", rva=0x100, size=4, unit="a"):
    return AnnotatedDataDefinition(
        unit=unit, name="value", qualified_name="value", rva=rva, size=size,
        location="a.cpp:1", is_static=False, symbol=symbol)


class RenderTests(unittest.TestCase):
    def test_header_and_hex_fields_match_the_consumed_schema(self):
        # synth_pdb reads rva/size as hex through int(x, 16); decimal would parse
        # to a different address rather than fail.
        text = render([sym(0xd2530, "??0baseManager@@QAE@XZ", "BASE/BASEMGR", 0x47)])
        self.assertEqual(text.splitlines()[0], "rva,name,unit,size,kind,provenance")
        self.assertEqual(
            text.splitlines()[1],
            "0xd2530,??0baseManager@@QAE@XZ,BASE/BASEMGR,0x47,func,source-annotation")

    def test_no_symbols_still_renders_the_header(self):
        # An empty inventory is the correct state before anything is marked, and
        # synth_pdb must still find a well-formed file.
        self.assertEqual(render([]), "rva,name,unit,size,kind,provenance\n")


class CompgenMarkerTests(unittest.TestCase):
    def test_marker_uses_semantic_role_instead_of_volatile_coff_counter(self):
        with TemporaryDirectory() as directory:
            repo = Path(directory)
            source_root = repo / "src"
            path = source_root / "SOURCE" / "PHILAI.cpp"
            path.parent.mkdir(parents=True)
            path.write_text(
                "VA_COMPGEN(0x0047e1bf, 0xf, STATIC_CTOR, SVSearchArray)\n")
            rows = compgen_functions_for_file(path, source_root, repo)

        self.assertEqual(len(rows), 1)
        self.assertEqual(
            rows[0].name,
            "__h2cg$SOURCE$PHILAI$static_ctor$SVSearchArray")
        self.assertEqual(rows[0].rva, 0x7e1bf)
        self.assertNotIn("_$E", render_compgen(rows))

    def test_unknown_semantic_role_is_rejected(self):
        with TemporaryDirectory() as directory:
            repo = Path(directory)
            source_root = repo / "src"
            path = source_root / "SOURCE" / "PHILAI.cpp"
            path.parent.mkdir(parents=True)
            path.write_text(
                "VA_COMPGEN(0x0047e1bf, 0xf, UNKNOWN_ROLE, SVSearchArray)\n")
            with self.assertRaisesRegex(ValueError, "invalid VA_COMPGEN"):
                compgen_functions_for_file(path, source_root, repo)

    def test_qualified_static_owner_has_a_stable_semantic_name(self):
        with TemporaryDirectory() as directory:
            repo = Path(directory)
            source_root = repo / "src"
            path = source_root / "BASE" / "AudiereMusic.cpp"
            path.parent.mkdir(parents=True)
            path.write_text(
                "VA_COMPGEN(0x004cd080, 0xf, STATIC_INIT_DISPATCH, "
                "AudiereMusicState::stream)\n")
            rows = compgen_functions_for_file(path, source_root, repo)

        self.assertEqual(
            rows[0].name,
            "__h2cg$BASE$AudiereMusic$static_init_dispatch$"
            "AudiereMusicState$stream")
        self.assertEqual(rows[0].owner, "AudiereMusicState::stream")


class CollectTests(unittest.TestCase):
    def _collect(self, rows, vtables=()):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "a.cpp").write_text("")
            with mock.patch.object(mod, "symbols_for_file", return_value=list(rows)), \
                    mock.patch.object(mod, "source_vtables", return_value=list(vtables)):
                return collect(root, root)

    def test_rows_come_back_sorted_by_address(self):
        out = self._collect([sym(0x200, "b"), sym(0x100, "a")])
        self.assertEqual([r.rva for r in out], [0x100, 0x200])

    def test_two_names_on_one_address_is_an_error(self):
        # Silently keeping whichever sorted first would delink one span under a
        # name the other half of the tree believes belongs elsewhere.
        with self.assertRaises(ValueError) as raised:
            self._collect([sym(0x100, "?a@@YIHH@Z"), sym(0x100, "?b@@YIHH@Z")])
        self.assertIn("0x100", str(raised.exception))
        self.assertIn("?a@@YIHH@Z", str(raised.exception))

    def test_the_same_symbol_seen_twice_is_not_an_error(self):
        out = self._collect([sym(0x100, "?a@@YIHH@Z"), sym(0x100, "?a@@YIHH@Z")])
        self.assertEqual(len(out), 1)

    def test_vtables_are_carried_as_data(self):
        vtable = mock.Mock(rva=0xeba70, mangled_name="??_7baseManager@@6B@",
                           unit="BASE/BASEMGR")
        out = self._collect([], [vtable])
        self.assertEqual([(r.name, r.kind, r.provenance) for r in out],
                         [("??_7baseManager@@6B@", "data", "source-vtable")])

    def test_compgen_data_gets_a_stable_pdb_identity(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "a.cpp").write_text("")
            claim = mock.Mock(
                rva=0x11405c, unit="SOURCE/KB",
                semantic_name="gWinSetupBuildImprovement0", size=0xb,
                location="src/SOURCE/KB.cpp:11174")
            with mock.patch.object(mod, "symbols_for_file", return_value=[]), \
                    mock.patch.object(mod, "source_vtables", return_value=[]), \
                    mock.patch.object(mod, "source_compgen_data",
                                      return_value=[claim]):
                out = collect(root, root)
        self.assertEqual(len(out), 1)
        self.assertEqual(
            out[0].name,
            "__h2cg$SOURCE$KB$data$gWinSetupBuildImprovement0")
        self.assertEqual((out[0].rva, out[0].kind), (0x11405c, "data"))


class DataMarkerTests(unittest.TestCase):
    """DATA() is the only channel that can name a global in a stripped image."""

    def _symbols(self, definitions, text="DATA(0x00400100) int value;\n"):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "a.cpp"
            source.write_text(text)
            translation = mock.Mock()
            translation.diagnostics = []
            translation.cursor.walk_preorder.return_value = []
            index = mock.Mock()
            index.parse.return_value = translation
            with mock.patch.object(mod.ci.Index, "create", return_value=index), \
                    mock.patch.object(mod, "configure_libclang"), \
                    mock.patch.object(mod, "_clang_args", return_value=[]), \
                    mock.patch.object(mod, "definitions_for_file",
                                      return_value=list(definitions)) as parser:
                return mod.symbols_for_file(source.resolve(), root, root), parser

    def test_an_annotated_definition_becomes_a_data_row(self):
        rows, _ = self._symbols([data_definition()])
        self.assertEqual(
            [(r.rva, r.name, r.unit, r.size, r.kind, r.provenance) for r in rows],
            [(0x100, "?value@@3HA", "a", 4, "data", "source-annotation")])

    def test_a_file_with_only_data_markers_is_still_walked(self):
        # The VA-only early return would have dropped a pure data TU silently.
        _rows, parser = self._symbols([data_definition()])
        self.assertEqual(parser.call_count, 1)

    def test_a_marker_that_names_no_symbol_is_a_source_defect(self):
        # Same rule as an unusable VA marker: the delinker would otherwise be
        # handed an address with nothing to call it.
        with self.assertRaises(ValueError) as raised:
            self._symbols([data_definition(symbol="")])
        self.assertIn("a.cpp:1", str(raised.exception))

    def test_an_unmarked_file_is_not_parsed_at_all(self):
        rows, parser = self._symbols([], text="int value;\n")
        self.assertEqual(rows, [])
        self.assertEqual(parser.call_count, 0)


class NothingMarkedTests(unittest.TestCase):
    def test_a_stripped_tree_yields_nothing_rather_than_guessing(self):
        # The whole point of this branch: no CodeView, so an unmarked tree has no
        # symbols. It must not fall back to Ghidra's 2472 candidate boundaries.
        with TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "a.cpp").write_text("int f(int a) { return a; }\n")
            with mock.patch.object(mod, "source_vtables", return_value=[]):
                self.assertEqual(collect(root, root), [])


if __name__ == "__main__":
    unittest.main()
