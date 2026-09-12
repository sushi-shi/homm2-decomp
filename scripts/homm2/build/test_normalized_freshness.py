import json
import os
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

    def test_modified_output_is_rejected(self):
        raw, normalized = self._tree()
        write_stamp(normalized, {"input": raw})
        normalized.write_bytes(b"retail bytes copied over candidate")
        self.assertTrue(freshness_problems(normalized))

    def test_same_size_output_change_with_preserved_mtime_is_rejected(self):
        raw, normalized = self._tree()
        write_stamp(normalized, {"input": raw})
        before = normalized.stat()
        normalized.write_bytes(b"normalized-object-v2")
        os.utime(normalized, ns=(before.st_atime_ns, before.st_mtime_ns))
        self.assertTrue(freshness_problems(normalized))

    def test_empty_or_malformed_provenance_is_rejected(self):
        raw, normalized = self._tree()
        for inputs in ({}, [], {"input": None}, {"input": {}}):
            with self.subTest(inputs=inputs):
                write_stamp(normalized, {"input": raw})
                record = json.loads(stamp_path(normalized).read_text())
                record["inputs"] = inputs
                stamp_path(normalized).write_text(json.dumps(record))
                self.assertTrue(freshness_problems(normalized))

    def test_missing_intermediate_stamp_is_rejected(self):
        raw, normalized = self._tree()
        intermediate = self.root / "objdiff/paired/UNIT.obj"
        intermediate.parent.mkdir(parents=True)
        intermediate.write_bytes(b"paired")
        write_stamp(intermediate, {"input": raw})
        write_stamp(normalized, {"input": intermediate})
        stamp_path(intermediate).unlink()
        self.assertTrue(freshness_problems(normalized))

    def test_cyclic_provenance_is_rejected(self):
        raw, normalized = self._tree()
        write_stamp(raw, {"input": normalized})
        write_stamp(normalized, {"input": raw})
        self.assertTrue(any("cyclic" in problem for problem in freshness_problems(normalized)))

    def test_old_schema_requires_rebuild(self):
        raw, normalized = self._tree()
        write_stamp(normalized, {"input": raw})
        record = json.loads(stamp_path(normalized).read_text())
        record["schema"] = 1
        stamp_path(normalized).write_text(json.dumps(record))
        self.assertTrue(any("unknown schema" in problem for problem in freshness_problems(normalized)))

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
