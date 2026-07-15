import unittest

from homm2.build.candidate_data_manifest import (
    REPO,
    _contains,
    candidate_definitions,
    derive_allocations,
)


class CandidateDataManifestTest(unittest.TestCase):
    def test_interval_containment_is_extent_aware(self):
        intervals = [(0x100, 0x120)]
        self.assertTrue(_contains(intervals, 0x100, 0x20))
        self.assertTrue(_contains(intervals, 0x11F))
        self.assertFalse(_contains(intervals, 0x0FF))
        self.assertFalse(_contains(intervals, 0x11F, 2))

    def test_icondf2b_candidate_topology_and_retail_bijection(self):
        path = REPO / "build/objdiff/base/BASE/Icondf2b.obj"
        if not path.is_file():
            self.skipTest("candidate objects are not built")
        definitions, _coff = candidate_definitions(path, "BASE/Icondf2b")
        local = [row for row in definitions if row["storage"] == "bss"]
        self.assertEqual(len(local), 13)
        self.assertEqual([row["section_offset"] for row in local],
                         list(range(0, 0x34, 4)))
        self.assertTrue(all(row["size"] == 4 and row["scope"] == "local"
                            for row in local))

        allocations, _stats, diagnostics = derive_allocations()
        mapped = [row for row in allocations if row.unit == "BASE/Icondf2b"]
        self.assertEqual(len(mapped), 13, diagnostics)
        self.assertEqual({row.rva for row in mapped}, set(range(0x1381B8, 0x1381EC, 4)))
        self.assertTrue(all(row.proof_count >= 1 for row in mapped))


if __name__ == "__main__":
    unittest.main()
