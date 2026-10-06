import unittest

from homm2.permute.emission_order import alias_for, parse_map, render, unit_flag_list


class EmissionOrderTests(unittest.TestCase):
    def manifest(self):
        return {
            "files": {"a.cpp": "@@outer@@ end", "b.h": "plain"},
            "units": ["a.cpp"],
            "flags": ["/c", "/Gy"],
            "defaults": {"outer": "x", "inner": "1"},
        }

    def test_slots_expand_recursively_and_options_adjust_flags_and_units(self):
        option = {"name": "o", "slots": {"outer": "<@@inner@@>"},
                  "flags_remove": ["/Gy"], "flags_add": ["/Ob2"],
                  "units": ["a.cpp", "b.cpp"]}
        files, units, flags = render(self.manifest(), (option,))
        self.assertEqual(files["a.cpp"], "<1> end")
        self.assertEqual(units, ["a.cpp", "b.cpp"])
        self.assertEqual(flags, ["/c", "/Ob2"])

    def test_unit_flags_apply_only_to_their_unit(self):
        manifest = self.manifest()
        manifest["unit_flags"] = {"a.cpp": ["/Ycpch.h"]}
        option = {"name": "use", "unit_flags": {"b.cpp": ["/Yupch.h"]}}
        _files, _units, flags = render(manifest, (option,))
        self.assertEqual(unit_flag_list(flags, "a.cpp"), ["/c", "/Gy", "/Ycpch.h"])
        self.assertEqual(unit_flag_list(flags, "b.cpp"), ["/c", "/Gy", "/Yupch.h"])

    def test_removing_an_absent_flag_is_an_error(self):
        with self.assertRaises(ValueError):
            render(self.manifest(), ({"name": "bad", "flags_remove": ["/Ob1"]},))

    def test_exact_edits_must_be_unique(self):
        manifest = self.manifest()
        manifest["files"]["b.h"] = "x x"
        with self.assertRaises(ValueError):
            render(manifest, ({"name": "e", "edits": [
                {"file": "b.h", "find": "x", "replace": "y"}]},))

    def test_map_rows_keep_owner_after_function_and_inline_flags(self):
        text = (" 0001:00000010       ?f@@YAXXZ                  00401010 f   a.obj\n"
                " 0001:00000020       ??1N@@QAE@XZ               00401020 f i b.obj\n")
        self.assertEqual(parse_map(text), [(0x401010, "?f@@YAXXZ", "a.obj"),
                                           (0x401020, "??1N@@QAE@XZ", "b.obj")])

    def test_first_matching_alias_wins(self):
        import re
        aliases = [(re.compile("^\\?\\?1N"), "N"), (re.compile("N"), "other")]
        self.assertEqual(alias_for("??1N@@QAE@XZ", aliases), "N")
        self.assertIsNone(alias_for("?g@@YAXXZ", aliases))


if __name__ == "__main__":
    unittest.main()
