"""homm2.verify - `homm2 verify <verb|gate>` - status, banking and gates.

    homm2 verify status        live metrics and retained maxima (reports only)
    homm2 verify check         exit 1 unless every function and data byte is exact
    homm2 verify bank          record maxima for the current source hashes
    homm2 verify readme        refresh the generated README status block
    homm2 verify <gate> [...]  run one gate (the list below)
    homm2 verify all           run every gate of the `homm2 build verify` tier

A gate exits non-zero on a finding and writes nothing but build/ scratch.
`homm2 build verify` runs the tier after an ordinary build. Gates outside the
tier carry known findings in the current tree (docs/tooling-convergence.md);
they stay runnable so the findings remain visible.
"""
from __future__ import annotations

import sys
import time

from homm2.core.paths import REPO
from homm2.core.usage import logged, run_process

PY = sys.executable

#: gate -> (argv, description). Every gate is a separate process: the gates
#: are standalone modules with their own argument handling.
GATES = {
    "behaviour": ([PY, "-m", "homm2.verify.behaviour"],
                  "game-behaviour tests of the reconstructed code"),
    "check": ([PY, "-m", "homm2.verify.status", "check"],
              "every function and every data byte exact"),
    "link-diff": (["ninja", "link-diff"],
                  "historical link byte-identical within config/link_diff.tsv"),
    "localization": ([PY, "-m", "homm2.graph.localization"],
                     "every used text ID resolves in the catalog"),
    "strict-allocations": ([PY, "-m", "homm2.verify.strict_allocations"],
                           "reviewed initialized storage under objdiff's strict schema"),
    "assert-relocs": ([PY, "-m", "homm2.verify.assert_relocs", "--fields"],
                      "ordered relocation fields of every exact function"),
    "no-fake-labels": ([PY, "-m", "homm2.verify.no_fake_labels"],
                       "every emitted function symbol is a reviewed identity"),
    "globals-data": ([PY, "-m", "homm2.verify.globals_data"],
                     "DATA(VA) on every global definition, unique"),
    "globals-defined": ([PY, "-m", "homm2.verify.globals_defined"],
                        "every extern global has an owner definition"),
    "vtables": ([PY, "-m", "homm2.verify.vtables"],
                "source-owned vtable identities"),
    "fixed-width-ints": ([PY, "-m", "homm2.verify.fixed_width_ints"],
                         "game code uses the Ints.h aliases"),
    "usage": ([PY, "-m", "homm2.audit.usage"],
              "every tooling entry point keeps usage logging"),
    # Outside the tier: known findings in the current tree.
    "decls": ([PY, "-m", "homm2.verify.decls"],
              "no type or extern declarations in a .cpp"),
    "defs-declared": ([PY, "-m", "homm2.verify.defs_declared"],
                      "every free function is declared in its owner header"),
    "annotated-functions": ([PY, "-m", "homm2.retail_labels.annotated_functions", "--check",
                             "--objects", "build/objdiff/base"],
                            "source VA spans and private identities"),
    "text-coverage": ([PY, "-m", "homm2.verify.text_coverage"],
                      "every .text byte is claimed, padding or reviewed"),
    "constants": ([PY, "-m", "homm2.verify.constants"],
                  "numeric literals reviewed"),
    "relocs": ([PY, "-m", "homm2.verify.assert_relocs"],
               "relocation targets of near-exact functions (review; `relocs 0x<rva>`)"),
    "od-frames": ([PY, "-m", "homm2.verify.od_frames"],
                  "/Od frame and slot drift (report)"),
    "model-drift": ([PY, "-m", "homm2.verify.model_drift"],
                    "candidate COFF against the fixed delink model (warning)"),
    "data-relocs": ([PY, "-m", "homm2.verify.data_relocs"],
                    "data relocation topology of two COFF objects"),
    "data-topology": ([PY, "-m", "homm2.verify.data_topology"],
                      "candidate and target data-symbol topology"),
}

#: The `homm2 build verify` tier, in run order.
TIER = ("check", "link-diff", "behaviour", "localization", "strict-allocations",
        "assert-relocs", "no-fake-labels", "globals-data", "globals-defined",
        "vtables", "fixed-width-ints", "usage")

VERBS = {"status": [], "bank": ["update"], "readme": ["--write-readme"]}


def usage(stream=sys.stderr) -> None:
    print(__doc__.strip(), file=stream)
    width = max(map(len, GATES))
    print("\ngates (* = in the build verify tier):", file=stream)
    for name, (_argv, blurb) in GATES.items():
        mark = "*" if name in TIER else " "
        print(f"  {mark} {name:<{width}}  {blurb}", file=stream)


def run_gate(name: str, extra: list[str] = ()) -> int:
    argv, _ = GATES[name]
    return run_process([*argv, *extra], cwd=REPO)


def run_tier() -> int:
    failed = []
    for name in TIER:
        started = time.monotonic()
        rc = run_gate(name)
        verdict = "OK" if rc == 0 else f"FAIL (rc={rc})"
        print(f"[verify] {name}: {verdict} ({time.monotonic() - started:.1f}s)", flush=True)
        if rc:
            failed.append(name)
    if failed:
        print(f"[verify] {len(failed)} gate(s) failed: {', '.join(failed)}; "
              f"rerun one with `homm2 verify <gate>`")
        return 1
    print(f"[verify] all {len(TIER)} gates pass")
    return 0


@logged
def main(argv=None) -> int:
    argv = list(sys.argv[1:] if argv is None else argv)
    if not argv or argv[0] in ("-h", "--help"):
        usage(sys.stdout if argv else sys.stderr)
        return 0 if argv else 2
    verb, rest = argv[0], argv[1:]
    if verb == "all":
        return run_tier()
    if verb in VERBS:
        from homm2.verify.status import main as status
        return status([*VERBS[verb], *rest])
    if verb not in GATES:
        print(f"homm2 verify: unknown verb or gate {verb!r}", file=sys.stderr)
        usage()
        return 2
    return run_gate(verb, rest)


if __name__ == "__main__":
    sys.exit(main())
