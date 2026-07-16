import unittest
from types import SimpleNamespace
from unittest import mock

from homm2.build.data_topology_audit import audit_candidate_topology


class DataTopologyAuditTest(unittest.TestCase):
    def test_audit_builds_candidates_and_derives_all_outputs_once(self):
        stats = SimpleNamespace()
        with (
            mock.patch("homm2.build.data_topology_audit.subprocess.run") as run,
            mock.patch("homm2.build.data_topology_audit.build_manifests",
                       return_value={"section_assignment_diagnostics": 0}) as assemble,
            mock.patch("homm2.build.data_topology_audit._canonical_union_rows",
                       return_value=["canonical"]) as canonical,
            mock.patch("homm2.build.data_topology_audit.derive_allocations",
                       return_value=(["allocation"], stats, ["group"])) as derive,
            mock.patch("homm2.build.data_topology_audit.review_queue_bytes",
                       return_value=(b"queue", {"review_queue_rows": 1})) as queue,
            mock.patch("homm2.build.data_topology_audit.candidate_diagnostics_bytes",
                       return_value=b"diagnostics") as diagnostics,
            mock.patch("homm2.build.data_topology_audit._atomic_write") as write,
            mock.patch("homm2.build.data_topology_audit._build_coverage_proposal",
                       return_value=(b"coverage", [], ["coverage"])) as coverage,
        ):
            result = audit_candidate_topology(jobs=7, migrate_identities=True)

        self.assertEqual(
            [call.args[0] for call in run.call_args_list],
            [
                ["python3", "configure.py"],
                ["ninja", "-j", "7", "all"],
                ["python3", "-m", "homm2.build.gen_reloc_aliases"],
            ],
        )
        self.assertTrue(all(call.kwargs == {"check": True}
                            for call in run.call_args_list))
        assemble.assert_called_once_with(
            strict=False,
            migrate_from=mock.ANY,
        )
        canonical.assert_called_once_with()
        derive.assert_called_once_with(reviewed_rows=["canonical"])
        queue.assert_called_once_with(["allocation"], ["group"])
        diagnostics.assert_called_once_with(stats, ["group"])
        self.assertEqual(write.call_count, 2)
        coverage.assert_called_once_with(["allocation"])
        self.assertEqual(result[2:], (["group"], ["coverage"],
                                     {"review_queue_rows": 1}))


if __name__ == "__main__":
    unittest.main()
