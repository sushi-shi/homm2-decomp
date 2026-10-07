"""homm2.verify - `homm2 verify <verb|gate>` - status, banking and gates.

    homm2 verify status        live metrics and retained maxima (reports only)
    homm2 verify check         exit 1 unless every function and data byte is exact
    homm2 verify bank          record maxima for the current source hashes
    homm2 verify readme        refresh the generated README status block
    homm2 verify <gate> [...]  run one gate (the list below)
    homm2 verify all           run every gate of the `homm2 build verify` tier

A gate exits non-zero on a finding and writes nothing but build/ scratch.
Every `homm2 build` runs the build gates (and the staged ones, advisory);
`homm2 build verify` adds the tier. Gates outside both carry known findings or
are reports; they stay runnable so the findings remain visible.
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
    "image-link-diff": ([PY, "-m", "homm2.verify.link_diff"],
                        "another image's link within its ceiling (pending until it links)"),
    "localization": ([PY, "-m", "homm2.graph.localization"],
                     "every used text ID resolves in the catalog"),
    "strict-allocations": ([PY, "-m", "homm2.verify.strict_allocations"],
                           "reviewed initialized storage under objdiff's strict schema"),
    "assert-relocs": ([PY, "-m", "homm2.verify.assert_relocs", "--resolved"],
                      "ordered resolved relocation sites and owner offsets"),
    "reloc-fields": ([PY, "-m", "homm2.verify.assert_relocs", "--fields"],
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
    "decls": ([PY, "-m", "homm2.verify.decls"],
              "no type or extern declarations in a .cpp"),
    "annotated-functions": ([PY, "-m", "homm2.retail_labels.annotated_functions", "--check",
                             "--objects", "build/objdiff/base"],
                            "source VA spans and private identities in the objects"),
    "annotated-sources": ([PY, "-m", "homm2.retail_labels.annotated_functions", "--check"],
                          "source VA spans and private identities"),
    "reloc-identities": ([PY, "-m", "homm2.verify.assert_relocs"],
                         "unordered relocation identities of near-exact functions"),
    # Staged: it runs in every build and reports, but carries findings that
    # predate its enforcement (docs/match-provenance-audit.md).
    "defs-declared": ([PY, "-m", "homm2.verify.defs_declared"],
                      "every free function is declared in its owner header"),
    # Outside the build and the tier: known findings or reports.
    "text-coverage": ([PY, "-m", "homm2.verify.text_coverage"],
                      "every .text byte is claimed, padding or reviewed"),
    "constants": ([PY, "-m", "homm2.verify.constants", "--jobs", "4"],
                  "numeric-literal inventory; no 0 spelled for a null pointer"),
    "enum-reuse": ([PY, "-m", "homm2.verify.enum_reuse"],
                   "enum, #define and const values of both images vs the reuse "
                   "review ledger (pending rows)"),
    "relocs": ([PY, "-m", "homm2.verify.assert_relocs"],
               "focused relocation review (`relocs 0x<rva>`)"),
    "od-frames": ([PY, "-m", "homm2.verify.od_frames"],
                  "/Od frame and slot drift (report)"),
    "model-drift": ([PY, "-m", "homm2.verify.model_drift"],
                    "candidate COFF against the fixed delink model (warning)"),
    "data-relocs": ([PY, "-m", "homm2.verify.data_relocs"],
                    "data relocation topology of two COFF objects"),
    "data-topology": ([PY, "-m", "homm2.verify.data_topology"],
                      "candidate and target data-symbol topology"),
}

#: Hard gates every `homm2 build` runs after Ninja, in order (`annotated-sources`
#: runs before configuring). A score is not evidence that declarations, data
#: owners or relocations are sound.
BUILD_GATES = ("annotated-functions", "decls", "no-fake-labels", "globals-data",
               "globals-defined", "vtables", "assert-relocs", "reloc-identities",
               "fixed-width-ints")
#: Run by every build; a failure is reported, not fatal, until its recorded
#: findings are resolved.
STAGED = ("defs-declared",)
#: Added by `homm2 build verify`, in run order.
TIER = ("check", "link-diff", "behaviour", "localization", "strict-allocations",
        "reloc-fields", "text-coverage", "constants", "usage")

#: Another image's gates. Its source gates (declarations, the source-function
#: inventory, integer spelling, the catalog) read the whole tree and run with
#: the game; its build gates check the image's own inventory and objects; its
#: tier holds it to its banked maxima (`check`) and, once it links, its
#: link-diff ceiling.
IMAGE_BUILD_GATES = ("vtables", "no-fake-labels", "globals-data", "globals-defined")
#: Staged for another image while its units are reconstructed (none now: the
#: editor's objects name only reviewed functions, and every header extern its
#: own sources define carries its DATA claim and definition).
IMAGE_STAGED: tuple[str, ...] = ()
IMAGE_TIER = ("check", "image-link-diff", "strict-allocations", "reloc-fields")


def build_gates() -> tuple[str, ...]:
    from homm2.core.paths import DEFAULT_IMAGE, image_key
    return BUILD_GATES if image_key() == DEFAULT_IMAGE else IMAGE_BUILD_GATES


def staged_gates() -> tuple[str, ...]:
    from homm2.core.paths import DEFAULT_IMAGE, image_key
    return STAGED if image_key() == DEFAULT_IMAGE else IMAGE_STAGED


def tier() -> tuple[str, ...]:
    from homm2.core.paths import DEFAULT_IMAGE, image_key
    return TIER if image_key() == DEFAULT_IMAGE else IMAGE_TIER

VERBS = {"status": [], "bank": ["update"], "readme": ["--write-readme"]}


def usage(stream=sys.stderr) -> None:
    print(__doc__.strip(), file=stream)
    width = max(map(len, GATES))
    print("\ngates (b = every build, s = staged, * = build verify tier):", file=stream)
    for name, (_argv, blurb) in GATES.items():
        mark = ("b" if name in build_gates() or name == "annotated-sources" else
                "s" if name in staged_gates() else "*" if name in tier() else " ")
        print(f"  {mark} {name:<{width}}  {blurb}", file=stream)


def run_gate(name: str, extra: list[str] = ()) -> int:
    argv, _ = GATES[name]
    return run_process([*argv, *extra], cwd=REPO)


def run_gates(names, *, advisory: bool = False) -> int:
    """Run gates in order; a hard failure stops at once, an advisory one is
    reported and the run continues."""
    for name in names:
        if run_gate(name):
            if not advisory:
                print(f"[verify] {name}: FAIL; rerun with `homm2 verify {name}`")
                return 1
            print(f"[verify] staged gate {name} failed (advisory)", file=sys.stderr)
    return 0


def run_tier() -> int:
    from homm2.core.paths import image_key
    failed = []
    names = tier()
    print(f"[verify] {image_key()} tier", flush=True)
    for name in names:
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
    print(f"[verify] all {len(names)} {image_key()} gates pass")
    return 0


@logged
def main(argv=None) -> int:
    argv = list(sys.argv[1:] if argv is None else argv)
    if not argv or argv[0] in ("-h", "--help"):
        usage(sys.stdout if argv else sys.stderr)
        return 0 if argv else 2
    verb, rest = argv[0], argv[1:]
    if verb == "all":
        return (run_gates(build_gates()) or run_gates(staged_gates(), advisory=True)
                or run_tier())
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
