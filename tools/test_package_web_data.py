#!/usr/bin/env python3

from pathlib import Path
import tempfile
import unittest

from package_web_data import preloads


class WebDataTest(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="homm2 web data ")
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)

    def create(self, path):
        result = self.root / path
        result.parent.mkdir(parents=True, exist_ok=True)
        result.write_bytes(b"fixture")
        return result

    def test_lowercase_and_mixed_case_inputs(self):
        self.create("data/heroes2.agg")
        self.create("maps/Scenario.mp2")
        self.create("MuSiC/Track02.ogg")
        self.create("h2camp.txt")
        self.create("polcamp.txt")
        self.create("heroes2.cfg")
        self.assertEqual(preloads(str(self.root)), [
            "--preload", f"{self.root}/data@/game/DATA",
            "--preload", f"{self.root}/maps@/game/MAPS",
            "--preload", f"{self.root}/MuSiC@/game/MUSIC",
            "--preload", f"{self.root}/h2camp.txt@/game/H2CAMP.TXT",
            "--preload", f"{self.root}/polcamp.txt@/game/POLCAMP.TXT",
            "--preload", f"{self.root}/heroes2.cfg@/game/HEROES2.CFG",
        ])

    def test_exact_case_wins(self):
        self.create("DATA/HEROES2.AGG")
        self.create("data/HEROES2.AGG")
        self.assertEqual(preloads(str(self.root)), ["--preload", f"{self.root}/DATA@/game/DATA"])

    def test_ambiguous_case_is_rejected(self):
        self.create("DaTa/HEROES2.AGG")
        self.create("dAtA/HEROES2.AGG")
        with self.assertRaisesRegex(ValueError, "ambiguous"):
            preloads(str(self.root))

    def test_missing_archive_is_rejected(self):
        self.create("data/OTHER.AGG")
        with self.assertRaisesRegex(ValueError, "HEROES2.AGG"):
            preloads(str(self.root))
        with self.assertRaisesRegex(ValueError, "HOMM2_DATA"):
            preloads("")

    def test_symlinked_resources_are_included(self):
        self.create("original/heroes2.agg")
        (self.root / "data").symlink_to(self.root / "original", target_is_directory=True)
        self.assertEqual(preloads(str(self.root)), ["--preload", f"{self.root}/data@/game/DATA"])

    def test_file_is_not_accepted_as_a_resource_directory(self):
        self.create("DATA/HEROES2.AGG")
        self.create("maps")
        with self.assertRaisesRegex(ValueError, "directory"):
            preloads(str(self.root))


if __name__ == "__main__":
    unittest.main()
