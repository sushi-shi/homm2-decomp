import unittest
from pathlib import Path
from tempfile import TemporaryDirectory
from unittest import mock

from homm2.build.reviewed_data import (
    _expected_objects,
    ensure_reviewed_targets,
    refresh_required,
)


class ReviewedDataTest(unittest.TestCase):
    def test_freshness_requires_exact_identity(self):
        expected = {"schema": 1, "manifest_sha256": "abc"}
        self.assertFalse(refresh_required(dict(expected), expected))
        self.assertFalse(refresh_required({**expected, "extra": "retained"}, expected))
        self.assertTrue(refresh_required(
            {"schema": 1, "manifest_sha256": "def"}, expected))
        self.assertTrue(refresh_required(None, expected))

    def test_manifest_object_paths_map_to_delinker_outputs(self):
        manifest = (
            b"name\tobject\trva\tsize\tstorage\talignment\tprovenance\n"
            b"table\tengine\\world.c\t0x100\t4\tdata\t4\ttest\n")
        self.assertEqual(_expected_objects(manifest), {"engine/world.c.obj"})

    def test_refresh_fails_when_required_target_inputs_are_missing(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            with (mock.patch("homm2.build.reviewed_data.EXE", root / "missing.exe"),
                  mock.patch("homm2.build.reviewed_data.PDB", root / "missing.pdb")):
                with self.assertRaisesRegex(RuntimeError, "run `homm2 init`"):
                    ensure_reviewed_targets(delinker=root / "missing-delinker")


if __name__ == "__main__":
    unittest.main()
