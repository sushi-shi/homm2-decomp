"""Refresh candidate COFFs and every non-destructive data-topology audit."""

import argparse
import subprocess

from homm2.build.candidate_data_manifest import (
    diagnostics_bytes as candidate_diagnostics_bytes,
    derive_allocations,
)
from homm2.build.data_manifest_adapter import SUPPLEMENTAL, build_manifests
from homm2.build.reviewed_data import (
    CANDIDATE_DIAGNOSTICS,
    REVIEW_QUEUE,
    _atomic_write,
    _build_coverage_proposal,
    _canonical_union_rows,
    review_queue_bytes,
)


def audit_candidate_topology(jobs=None, migrate_identities=False):
    """Build candidates and refresh manifests, proposals, and coverage once."""
    subprocess.run(["python3", "configure.py"], check=True)
    command = ["ninja"]
    if jobs is not None:
        command.extend(("-j", str(jobs)))
    command.append("all")
    subprocess.run(command, check=True)

    manifest_summary = build_manifests(
        strict=False,
        migrate_from=SUPPLEMENTAL if migrate_identities else None,
    )
    canonical = _canonical_union_rows()
    allocations, stats, group_diagnostics = derive_allocations(
        reviewed_rows=canonical)
    queue, queue_stats = review_queue_bytes(
        allocations, group_diagnostics)
    _atomic_write(REVIEW_QUEUE, queue)
    _atomic_write(
        CANDIDATE_DIAGNOSTICS,
        candidate_diagnostics_bytes(stats, group_diagnostics),
    )
    _coverage, _padding, coverage_diagnostics = _build_coverage_proposal(
        allocations)
    subprocess.run([
        "python3", "-m", "homm2.build.gen_reloc_aliases",
    ], check=True)
    return (
        manifest_summary, stats, group_diagnostics,
        coverage_diagnostics, queue_stats,
    )


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--jobs", type=int)
    parser.add_argument(
        "--migrate-identities", action="store_true",
        help=("update compiler-local names only where reviewed candidate section "
              "topology remains invariant"),
    )
    parser.add_argument("--strict", action="store_true")
    args = parser.parse_args(argv)
    if args.jobs is not None and args.jobs <= 0:
        parser.error("--jobs must be positive")

    summary, stats, groups, coverage, queue = audit_candidate_topology(
        args.jobs, args.migrate_identities)
    blockers = len(groups) + len(coverage) + summary["section_assignment_diagnostics"]
    print(
        "candidate data audit: %d/%d individually evidenced; %d/%d in %d "
        "closed groups; %d open groups; %d section diagnostics; %d coverage "
        "diagnostics; %d review rows"
        % (
            stats.evidenced_definitions,
            stats.candidate_definitions,
            stats.mapped_definitions,
            stats.candidate_definitions,
            stats.closed_groups,
            stats.open_groups,
            summary["section_assignment_diagnostics"],
            len(coverage),
            queue["review_queue_rows"],
        )
    )
    if args.strict and blockers:
        print("candidate data audit: strict closure refused with %d blockers" % blockers)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
