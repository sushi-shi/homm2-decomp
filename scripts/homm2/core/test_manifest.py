"""The one flag-assembly rule."""
import unittest

from homm2.core.manifest import flag_profiles, load, unit_flags, units


class UnitFlagsTests(unittest.TestCase):
    MANIFEST = {
        "flags": {"base": ["/Od", "/MT"], "base_gx": ["/Od", "/MT", "/GX"]},
        "unit": [
            {"unit": "SOURCE/REQUEST", "source": "src/SOURCE/REQUEST.cpp"},
            {"unit": "BASE/DIMMER", "source": "src/BASE/DIMMER.cpp"},
            {"unit": "BASE/AudiereEffects", "source": "src/BASE/AudiereEffects.cpp",
             "flags": "base_gx"},
        ],
    }

    def test_source_units_get_the_bare_profile(self):
        row = self.MANIFEST["unit"][0]
        self.assertEqual(unit_flags(row, self.MANIFEST), ["/Od", "/MT"])

    def test_base_units_get_the_function_packaging_tier_rule(self):
        row = self.MANIFEST["unit"][1]
        self.assertEqual(unit_flags(row, self.MANIFEST), ["/Od", "/MT", "/Gy"])

    def test_explicit_profiles_still_get_the_tier_rule(self):
        row = self.MANIFEST["unit"][2]
        self.assertEqual(unit_flags(row, self.MANIFEST),
                         ["/Od", "/MT", "/GX", "/Gy"])

    def test_flag_profiles_copies_are_independent(self):
        profiles = flag_profiles(self.MANIFEST)
        profiles["base"].append("/POISON")
        self.assertEqual(unit_flags(self.MANIFEST["unit"][0], self.MANIFEST),
                         ["/Od", "/MT"])

    def test_the_real_manifest_loads_and_covers_every_unit(self):
        manifest = load()
        rows = units(manifest)
        self.assertGreater(len(rows), 90)
        for row in rows:
            flags = unit_flags(row, manifest)
            self.assertIn("/Od" in flags or "/O1" in flags or "/O2" in flags,
                          (True,))
            self.assertEqual(row["unit"].startswith("BASE/"),
                             flags[-1] == "/Gy")
