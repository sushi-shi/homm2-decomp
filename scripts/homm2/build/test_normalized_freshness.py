import json
import shutil
import unittest
from pathlib import Path
from tempfile import TemporaryDirectory

from homm2.build.normalized_freshness import (
    freshness_problems, stamp_path, write_stamp,
)


class StampRoundTripTest(unittest.TestCase):
    def setUp(self):
        directory = TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        self.root = Path(directory.name)

    def _tree(self):
        raw = self.root / "objdiff/base/BASE/UNIT.obj"
        raw.parent.mkdir(parents=True)
        raw.write_bytes(b"raw-object-v1")
        normalized = self.root / "objdiff/normalized/base/BASE/UNIT.obj"
        normalized.parent.mkdir(parents=True)
        normalized.write_bytes(b"normalized-object-v1")
        return raw, normalized

    def test_fresh_chain_reports_no_problems(self):
        raw, normalized = self._tree()
        write_stamp(normalized, {"input": raw})

        self.assertEqual(freshness_problems(normalized), [])

    def test_rebuilt_raw_input_is_reported_stale(self):
        raw, normalized = self._tree()
        write_stamp(normalized, {"input": raw})
        raw.write_bytes(b"raw-object-v2")

        problems = freshness_problems(normalized)

        self.assertEqual(len(problems), 1)
        self.assertIn("is stale", problems[0])
        self.assertIn("homm2 build", problems[0])

    def test_missing_stamp_is_unverifiable_not_silent(self):
        _raw, normalized = self._tree()

        problems = freshness_problems(normalized)

        self.assertEqual(len(problems), 1)
        self.assertIn("no provenance stamp", problems[0])

    def test_two_stage_chain_catches_base_change_through_paired_copy(self):
        raw, _normalized = self._tree()
        paired = self.root / "objdiff/paired/target/BASE/UNIT.c.obj"
        paired.parent.mkdir(parents=True)
        paired.write_bytes(b"paired-target-v1")
        write_stamp(paired, {"base": raw})
        normalized_target = (
            self.root / "objdiff/normalized/target/BASE/UNIT.c.obj")
        normalized_target.parent.mkdir(parents=True)
        normalized_target.write_bytes(b"normalized-target-v1")
        write_stamp(normalized_target, {"input": paired})
        raw.write_bytes(b"raw-object-v2")

        problems = freshness_problems(normalized_target)

        self.assertEqual(len(problems), 1)
        self.assertIn(str(paired), problems[0])
        self.assertIn("base input changed", problems[0])

    def test_relative_stamps_survive_a_copied_build_tree(self):
        raw, normalized = self._tree()
        write_stamp(normalized, {"input": raw})
        record = json.loads(stamp_path(normalized).read_text())
        self.assertFalse(Path(record["inputs"]["input"]["path"]).is_absolute())

        copied = self.root.parent / (self.root.name + "-copy")
        shutil.copytree(self.root, copied)
        self.addCleanup(shutil.rmtree, copied)

        copied_normalized = copied / "objdiff/normalized/base/BASE/UNIT.obj"
        self.assertEqual(freshness_problems(copied_normalized), [])

        (copied / "objdiff/base/BASE/UNIT.obj").write_bytes(b"raw-object-v2")
        problems = freshness_problems(copied_normalized)
        self.assertEqual(len(problems), 1)
        self.assertIn("is stale", problems[0])


if __name__ == "__main__":
    unittest.main()
