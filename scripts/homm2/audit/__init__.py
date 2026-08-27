"""homm2.audit - `homm2 audit <tool>` - campaign audits, run on demand.

Nothing here is a build gate. The always-on gates live in homm2/build/ (assert_*)
and run inside `homm2 build`; these are the read-only diagnostics you reach for
when a residual will not explain itself, grouped by the question they answer:

  regression  ledger - match_baseline.tsv rows banked against source that no longer
              exists. The ledger is only rewritten by a build, so drift is otherwise
              invisible; this answers "is it current?" without a build.
              historical_exact_losses - functions that were exact historically and
              are not now, split by whether the effective-source hash also moved.
              An unchanged hash is TU/compiler-state evidence; a changed one usually
              means a source or helper edit. Feeds the permute sweep drivers.
  shape       scan_bitfield_residuals - ranks object diffs that look like a
              byte-vs-bitfield load mismatch, over retained best.* pairs or (--live)
              every current normalized unit.
              bool_fields - resolves every project-owned i32 data member and
              proposes b32 only when all observed writes are provably Boolean
              and no mutable escape makes the domain uncertain.
              casts - classifies explicit C++ casts by their source and
              destination type domains, enum identities, and storage widths.
              gotos - resolves goto/label edges and separates loops, cleanup
              exits, shared tails, and one-off forward jumps.
  data        strict_allocation_diff - strict reviewed-allocation comparison from
              objdiff one-shot JSON. Also the checker homm2.build.strict_allocations
              shells out to, so it is a library as much as a command.
              data_claims - derive a global's retail address from the donation
              evidence transposed per symbol, check it against the retail payload,
              and write the DATA() marker. Its `extents` mode audits how long a
              claimed allocation may be, which is the only channel that catches a
              trailing element the reconstruction invented.
  evidence    harvest_max_observations - copies banked-MAX replay coordinates and
              winning disassembly out of build/ into tracked docs, so a maximum keeps
              its evidence and not just its score.
  oracle      od_oracle - ground truth for homm2.core.od_slots, read straight from the
              S_BPREL32 debug records our compiler emits under /Z7. Needs wine.
  relocs      reloc_sweep - regenerate config/delink_relocs.tsv from the image with
              the delinker's find_relocs.py. The primary DIR32 channel.
              reloc_donation - SUPERSEDED by the sweep; kept as a cross-check and
              optional review of interior owner/addend aliases. It supplies no
              build identity or generated labelling input.

`homm2 audit` with no argument lists them. Dispatch is in-process via runpy, which
is why these five keep their own differing entry points instead of being reshaped
to one signature they never needed.
"""
import runpy
import sys

TOOLS = {
    "ledger": ("ledger",
               "match_baseline rows banked against source that changed"),
    "historical-losses": ("historical_exact_losses",
                          "functions exact historically but not now"),
    "bitfield-residuals": ("scan_bitfield_residuals",
                           "rank byte-vs-bitfield load mismatches"),
    "bool-fields": ("bool_fields",
                    "find i32 fields whose observed write domain is Boolean"),
    "casts": ("casts",
              "classify explicit casts and preserve their type provenance"),
    "gotos": ("gotos",
              "classify goto/label edges by control-flow role"),
    "allocations": ("strict_allocation_diff",
                    "strict reviewed-allocation diff from objdiff JSON"),
    "data-claims": ("data_claims",
                    "derive DATA() addresses for globals; `extents` audits lengths"),
    "harvest-max": ("harvest_max_observations",
                    "bank MAX replay coordinates + asm into docs"),
    "od-oracle": ("od_oracle",
                  "compiler ground truth for the /Od slot model (needs wine)"),
    "unmatched-census": ("unmatched_census",
                         "classify (unmatched) functions by masked bytes + order"),
    "reloc-sweep": ("reloc_sweep",
                    "regenerate the DIR32 site manifest from the image"),
    "reloc-donation": ("reloc_donation",
                       "superseded by reloc-sweep; donates target identity"),
}


def usage(stream=sys.stderr) -> None:
    print("usage: homm2 audit <tool> [args...]\n", file=stream)
    width = max(len(name) for name in TOOLS)
    for name, (_, blurb) in TOOLS.items():
        print(f"  {name:<{width}}  {blurb}", file=stream)


def main(argv=None) -> int:
    argv = list(sys.argv[1:] if argv is None else argv)
    if not argv or argv[0] in ("-h", "--help", "help"):
        usage(sys.stdout if argv else sys.stderr)
        return 0 if argv else 1
    name = argv[0]
    if name not in TOOLS:
        print(f"homm2 audit: unknown tool {name!r}\n", file=sys.stderr)
        usage()
        return 1
    module = f"homm2.audit.{TOOLS[name][0]}"
    saved = sys.argv
    sys.argv = [f"homm2 audit {name}", *argv[1:]]
    try:
        runpy.run_module(module, run_name="__main__")
    except SystemExit as exit_code:          # the tools raise SystemExit(main())
        return int(exit_code.code or 0)
    finally:
        sys.argv = saved
    return 0
