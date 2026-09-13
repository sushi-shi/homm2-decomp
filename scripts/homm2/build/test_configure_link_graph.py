"""Archive ownership must not depend on already-generated symbol inventories."""
import unittest
from unittest import mock

from homm2.build.configure.link_graph import emit_link_graph
from homm2.build.fixed_asm import UNITS as FIXED_ASM_UNITS


def obj(unit):
    return "build/objdiff/base/" + unit + ".obj"


class ConfigureLinkGraphTests(unittest.TestCase):
    def edges(self, ordered_units, functions=None, generated=None):
        units = [dict(unit=name, source=record.source)
                 for name, record in FIXED_ASM_UNITS.items()]
        writer = mock.Mock()
        emit_link_graph(writer, units, [obj(name) for name in ordered_units], [],
                        functions or {}, generated or {})
        return {call.args[0]: call.kwargs for call in writer.build.call_args_list
                if isinstance(call.args[0], str)}

    def assert_partition(self, edges, source, prefix, suffix):
        # VC6 LIB prepends each input, so the emitted archive lists are reversed.
        self.assertEqual(edges["build/link/BASE-prefix.lib"]["inputs"], prefix[::-1])
        self.assertEqual(edges["build/link/Midi.lib"]["inputs"], [obj("BASE/Midi")])
        self.assertEqual(edges["build/link/BASE-suffix.lib"]["inputs"], suffix[::-1])
        direct = [path for path in edges["link-generic-inputs"]["inputs"]
                  if path.endswith(".obj")]
        self.assertEqual(direct, source)

    def test_fresh_checkout_without_symbol_inventory(self):
        edges = self.edges(["BASE/Before", "BASE/Midi", "BASE/After",
                            "EDITOR/mapcell", "SOURCE/GAME", "SOURCE/X_GLOBAL"])
        self.assert_partition(edges,
                              [obj("EDITOR/mapcell"), obj("SOURCE/GAME"),
                               obj("SOURCE/X_GLOBAL")],
                              [obj("BASE/Before")], [obj("BASE/After")])

    def test_data_only_source_unit_stays_direct_when_sorted_last(self):
        edges = self.edges(["BASE/Midi", "SOURCE/GAME", "SOURCE/X_GLOBAL"],
                           {"SOURCE/GAME": 0x1000, "BASE/Midi": 0x2000})
        self.assert_partition(edges, [obj("SOURCE/GAME"), obj("SOURCE/X_GLOBAL")],
                              [], [])

    def test_known_rvas_keep_archive_and_direct_input_order(self):
        edges = self.edges(["BASE/After", "BASE/Midi", "BASE/Before",
                            "SOURCE/X_GLOBAL", "SOURCE/GAME"],
                           {"SOURCE/GAME": 0x1000, "BASE/Before": 0x3000,
                            "BASE/Midi": 0x4000, "BASE/After": 0x5000},
                           {"SOURCE/X_GLOBAL": 0x2000, "BASE/Before": 0x6000})
        self.assert_partition(edges, [obj("SOURCE/GAME"), obj("SOURCE/X_GLOBAL")],
                              [obj("BASE/Before")], [obj("BASE/After")])

    def test_fixed_assembly_members_keep_omf_inputs(self):
        edges = self.edges(["BASE/BITS", "BASE/Midi", "BASE/TILE", "SOURCE/X_GLOBAL"])
        self.assert_partition(edges, [obj("SOURCE/X_GLOBAL")],
                              ["build/link/omf/BASE/BITS.obj"],
                              ["build/link/omf/BASE/TILE.obj"])


if __name__ == "__main__":
    unittest.main()
