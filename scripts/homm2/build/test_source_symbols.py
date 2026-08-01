import unittest
from pathlib import Path
from tempfile import TemporaryDirectory
from unittest import mock

from homm2.build import source_symbols as mod
from homm2.build.source_symbols import SourceSymbol, collect, render


def sym(rva, name, unit="BASE/X", size=0x10, kind="func", prov="source-annotation"):
    return SourceSymbol(rva=rva, name=name, unit=unit, size=size, kind=kind,
                        provenance=prov)


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
