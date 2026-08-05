#!/usr/bin/env python3
"""Find where 2.1 changed a function, by comparing the two RETAIL images directly.

Every other channel in this campaign compares our reconstruction against one image.
That cannot separate "we have not matched it yet" from "the function changed between
versions", because both read as a residual. This compares PoL 2.0's bytes against
Buka's bytes with our source out of the loop entirely, so what it finds is a
property of the two releases.

Two flagging channels, plus one demoted to context, each calibrated against the
functions our source already matches in BOTH images. Those have the same semantics
by construction, so whatever rate they show is the channel's noise floor:

  calls       the set of functions called, resolved through each tree's inventory.
              Names mean the same thing in both images; addresses do not.
              Baseline 10%, and reading those turned out to be almost entirely
              real-but-already-known changes, not noise - see KNOWN below.
  strings     the contents of literals reached by absolute operands. Baseline 14%,
              against 33% across the parity gap. This is how the 29 Russian CP1251
              in-code literals were found.
  constants   immediate operands. Baseline **80%** - VC 4.2 and VC6 disagree about
              address arithmetic constantly, and jump tables inside .text decode as
              garbage instructions. Reported as context, never flagged on: a channel
              that fires on four of five known-identical functions would flood
              docs/version-changes.md with nothing.

Two normalizations earn their keep. Decoration is stripped before comparing call
names, because `@nb_stat@4` and `_nb_stat` are the same function under different
calling conventions - a real finding, but a convention finding, reported as its own
channel rather than as a phantom added/removed pair. And the build-level changes
already recorded in docs/version-changes.md (the BaseAlloc/BaseFree allocator layer,
LogStr, the /QIfdiv helpers, the assert path) are suppressed unless --include-known,
so a sweep surfaces what is NOT yet written down.

What this is NOT: a proof. A difference is a CANDIDATE; read both bodies before
writing a row. Size delta is reported for context but never flags on its own - see
`homm2 audit cross-version` for why it cannot.

Classification stops at [Buka] / [unclassified]: separating upstream NWC Gold 2.1
changes from Buka's localization needs the Gold binary as a third channel, which is
a later round.

    homm2 audit version-delta                      # summary
    homm2 audit version-delta --write              # + the candidate TSV
    homm2 audit version-delta --lead all           # sweep every paired function
"""
from __future__ import annotations

import argparse
import csv
import re
import subprocess
import sys
from collections import Counter
from pathlib import Path

from homm2.core.paths import REPO
from homm2.audit.cross_version import (
    ReferenceError,
    build_rows,
    find_reference,
    read_ledger,
    read_profiles,
    read_report,
    read_symbols,
)
from homm2.audit.reloc_donation import IMAGE_BASE, pe_sections, retail_cstring

OUTPUT = Path("docs/version-delta-candidates.tsv")

# "  401006: 89 4d f8   mov  dword ptr [ebp - 0x8], ecx"
INSTRUCTION = re.compile(r"^\s*([0-9a-f]+):\s+(?:[0-9a-f]{2} )+\s*(\S+)\s*(.*)$")
# Any bracketed memory operand: [ebp - 0x8], [eax + esi*4 + 0x10].
MEMORY_OPERAND = re.compile(r"\[[^\]]*\]")
HEX = re.compile(r"\b0x[0-9a-f]+\b")
CALL_TARGET = re.compile(r"^0x([0-9a-f]+)")
# A literal worth comparing: printable, long enough not to be coincidence.
# CP1251 high bytes are included - Buka's translated literals live there.
PRINTABLE = re.compile(rb"^[\x20-\x7e\xa8\xb8\xc0-\xff\t]{3,}$")

# Calling-convention decoration. `_f` (cdecl), `@f@8` (fastcall) and `?f@@YIXXZ`
# vs `?f@@YAXXZ` all name one function; only the convention differs. Comparing the
# decorated forms would report a phantom added/removed pair for every such site.
FASTCALL_C = re.compile(r"^@(.+)@\d+$")
CDECL_C = re.compile(r"^_(.+)$")
CONVENTION_MANGLED = re.compile(r"@@(Y|Q[A-Z]?)[A-Z]")

# Build-level changes already recorded in docs/version-changes.md. Suppressed by
# default so a sweep surfaces what is not yet written down; --include-known
# restores them. Each entry is a callee whose presence or absence IS the known
# change, not evidence of a new one.
KNOWN_SYSTEMATIC = {
    # "[Buka] Debug allocation layer abandoned" - BaseAlloc/BaseFree have zero
    # callers; every site calls the CRT operators directly.
    "?BaseAlloc@@YIPAXIPADH@Z", "?BaseFree@@YIXPAXPADH@Z",
    "??2@YAPAXI@Z", "??3@YAXPAX@Z",
    # "[Buka] /QIfdiv off" - VC6 emits raw fdiv, VC 4.2 called these helpers.
    "__adj_fdiv_r", "__adj_fdiv_m64", "__adj_fdiv_m32", "__adj_fdiv_m16i",
    "__adj_fdiv_m32i", "__adj_fdivr_m32", "__adj_fdivr_m64", "__adj_fdivr_m16i",
    # "[Buka] Asserts survive in five TUs only", and the PoL logging helper.
    "?ProcessAssert@@YIXPADPADH@Z", "?LogStr@@YIXPAD@Z",
}

FIELDS = ("channels", "unit", "symbol", "lead", "transition",
          "buka_size", "pol_size", "size_delta",
          "calls_added", "calls_removed", "convention",
          "strings_buka", "strings_pol",
          "constants_added", "constants_removed")


def undecorate(name):
    """Strip calling-convention decoration so two spellings of one callee compare.

    Returns (identity, decorated) - the identity is what the call channel compares,
    the decorated form is kept so a convention change can still be reported.
    """
    match = FASTCALL_C.match(name) or CDECL_C.match(name)
    if match:
        return match.group(1)
    return CONVENTION_MANGLED.sub("@@?", name)


def find_image(root):
    """The retail target in a checkout. Each branch names its own image."""
    candidates = sorted((root / "build/orig").glob("*.[eE][xX][eE]"))
    if not candidates:
        raise ReferenceError(f"no retail image under {root / 'build/orig'}")
    return candidates[0]


def disassemble(image):
    """{va: (mnemonic, operands)} for the whole image, one objdump pass.

    Disassembling the PE directly rather than a carved blob keeps llvm-objdump's
    addresses absolute, so call displacements are already resolved for us.
    """
    result = subprocess.run(
        ("llvm-objdump", "-d", "--x86-asm-syntax=intel", str(image)),
        text=True, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    listing = {}
    for line in result.stdout.splitlines():
        match = INSTRUCTION.match(line)
        if match:
            listing[int(match.group(1), 16)] = (match.group(2), match.group(3))
    return listing


def _string_at(exe, sections, va):
    # pe_sections reports section addresses as RVAs, so retail_cstring wants an
    # RVA too - handing it the absolute VA silently finds nothing at all.
    raw = retail_cstring(exe, sections, va - IMAGE_BASE)
    if raw is None or not PRINTABLE.match(raw):
        return None
    return raw.decode("latin-1")


def features(listing, exe, sections, names, va, size):
    """Extract the three comparison channels for one function.

    `names` maps a target VA to a symbol, so the call channel compares names
    rather than addresses - the only form that means the same thing in both images.
    """
    calls, constants, strings = Counter(), Counter(), Counter()
    for address in range(va, va + size):
        entry = listing.get(address)
        if entry is None:
            continue
        mnemonic, operands = entry

        if mnemonic.startswith("call"):
            target = CALL_TARGET.match(operands.strip())
            if target:
                destination = int(target.group(1), 16)
                calls[names.get(destination, f"sub_{destination:x}")] += 1
            else:
                calls["(indirect)"] += 1
            continue
        if mnemonic.startswith(("j", "loop")):
            # Branch displacements are pure layout; they say nothing about version.
            continue

        # Frame and index displacements are codegen, not meaning: strip the whole
        # bracketed operand before looking for literals.
        bare = MEMORY_OPERAND.sub("[]", operands)
        for literal in HEX.findall(bare):
            value = int(literal, 16)
            if IMAGE_BASE <= value < IMAGE_BASE + 0x400000:
                text = _string_at(exe, sections, value)
                if text is not None:
                    strings[text] += 1
                continue
            constants[value] += 1
    return calls, constants, strings


def compare_counters(ours, theirs):
    """Two-way multiset difference: (only in ours, only in theirs)."""
    return ours - theirs, theirs - ours


def render(counter, limit=6):
    if not counter:
        return ""
    items = sorted(counter.items(), key=lambda item: (-item[1], str(item[0])))
    shown = [f"{key}" + (f"x{count}" if count > 1 else "")
             for key, count in items[:limit]]
    if len(items) > limit:
        shown.append(f"+{len(items) - limit}")
    return " ".join(shown)


def render_constants(counter, limit=6):
    if not counter:
        return ""
    items = sorted(counter.items(), key=lambda item: (-item[1], item[0]))
    shown = [f"0x{value:x}" + (f"x{count}" if count > 1 else "")
             for value, count in items[:limit]]
    if len(items) > limit:
        shown.append(f"+{len(items) - limit}")
    return " ".join(shown)


def split_calls(ours, theirs, include_known):
    """Compare call sets by identity, reporting convention changes separately.

    Returns (added, removed, convention). `added` and `removed` hold callees that
    genuinely appear on one side only; `convention` holds callees present on both
    under different decoration - the /Gr-vs-__cdecl class, a finding in its own
    right rather than a phantom pair.
    """
    def identities(counter):
        out = Counter()
        for name, count in counter.items():
            if not include_known and name in KNOWN_SYSTEMATIC:
                continue
            out[undecorate(name)] += count
        return out

    def spellings(counter):
        out = {}
        for name in counter:
            out.setdefault(undecorate(name), set()).add(name)
        return out

    our_ids, their_ids = identities(ours), identities(theirs)
    added, removed = our_ids - their_ids, their_ids - our_ids

    convention = Counter()
    our_spellings, their_spellings = spellings(ours), spellings(theirs)
    for identity in set(our_spellings) & set(their_spellings):
        if our_spellings[identity] != their_spellings[identity]:
            convention[
                f"{sorted(their_spellings[identity])[0]}->"
                f"{sorted(our_spellings[identity])[0]}"] += 1
    return added, removed, convention


def analyse(rows, buka, pol, include_known=False):
    """Run every paired row through the channels; flag on calls and strings only."""
    buka_listing, buka_exe, buka_sections, buka_names, buka_syms = buka
    pol_listing, pol_exe, pol_sections, pol_names, pol_syms = pol

    results = []
    for row in rows:
        key = (row["unit"], row["symbol"])
        if key not in buka_syms or key not in pol_syms:
            continue
        buka_rva, buka_size = buka_syms[key]
        pol_rva, pol_size = pol_syms[key]

        ours = features(buka_listing, buka_exe, buka_sections, buka_names,
                        buka_rva + IMAGE_BASE, buka_size)
        theirs = features(pol_listing, pol_exe, pol_sections, pol_names,
                          pol_rva + IMAGE_BASE, pol_size)

        calls_added, calls_removed, convention = split_calls(
            ours[0], theirs[0], include_known)
        constants_added, constants_removed = compare_counters(ours[1], theirs[1])
        strings_ours, strings_theirs = compare_counters(ours[2], theirs[2])

        # Constants never open a row on their own: 80% of known-identical functions
        # differ there, so a constants-only flag carries no information.
        channels = []
        if calls_added or calls_removed:
            channels.append("calls")
        if convention:
            channels.append("convention")
        if strings_ours or strings_theirs:
            channels.append("strings")
        if not channels:
            continue
        if constants_added or constants_removed:
            channels.append("constants")

        results.append({
            "channels": ",".join(channels),
            "unit": row["unit"],
            "symbol": row["symbol"],
            "lead": row["lead"],
            "transition": row["transition"],
            "buka_size": f"0x{buka_size:x}",
            "pol_size": f"0x{pol_size:x}",
            "size_delta": buka_size - pol_size,
            "calls_added": render(calls_added),
            "calls_removed": render(calls_removed),
            "convention": render(convention, limit=3),
            "strings_buka": render(strings_ours, limit=3),
            "strings_pol": render(strings_theirs, limit=3),
            "constants_added": render_constants(constants_added),
            "constants_removed": render_constants(constants_removed),
            # Ranking: a changed call set is the strongest evidence, a changed
            # string next. Constants only break ties among already-flagged rows.
            "_rank": (bool(calls_added or calls_removed) * 4
                      + bool(strings_ours or strings_theirs) * 2
                      + bool(convention)),
        })
    return results


def summarise(results, scanned, stream=sys.stdout):
    counts = Counter(row["channels"] for row in results)
    print(f"version-delta: scanned {scanned} paired functions", file=stream)
    print(f"version-delta: {len(results)} carry a flagged difference", file=stream)
    for channels, count in sorted(counts.items(), key=lambda item: -item[1]):
        print(f"version-delta:   {channels:<28} {count}", file=stream)


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--reference", help="path to the 2.0 checkout")
    parser.add_argument("--lead", default="all",
                        help="restrict to one cross-version class "
                             "(parity-gap, both-exact, ...); default all paired")
    parser.add_argument("--include-known", action="store_true",
                        help="also report the build-level changes already recorded "
                             "in docs/version-changes.md")
    parser.add_argument("--write", action="store_true", help=f"write {OUTPUT}")
    parser.add_argument("--output", type=Path, default=REPO / OUTPUT)
    args = parser.parse_args(argv)

    try:
        reference = find_reference(args.reference)
        buka_image, pol_image = find_image(REPO), find_image(reference)
    except ReferenceError as error:
        print(f"version-delta: {error}", file=sys.stderr)
        return 1

    buka_syms, pol_syms = read_symbols(REPO), read_symbols(reference)
    rows, _envelope = build_rows(
        (read_report(REPO), read_ledger(REPO), buka_syms, read_profiles(REPO)),
        (read_report(reference), read_ledger(reference), pol_syms,
         read_profiles(reference)))
    paired = {"parity-gap", "both-exact", "forward", "open-both"}
    rows = [row for row in rows
            if (row["lead"] in paired if args.lead == "all"
                else row["lead"] == args.lead)]

    buka_exe, pol_exe = buka_image.read_bytes(), pol_image.read_bytes()
    buka = (disassemble(buka_image), buka_exe, pe_sections(buka_exe),
            {rva + IMAGE_BASE: name for (_unit, name), (rva, _s) in buka_syms.items()},
            buka_syms)
    pol = (disassemble(pol_image), pol_exe, pe_sections(pol_exe),
           {rva + IMAGE_BASE: name for (_unit, name), (rva, _s) in pol_syms.items()},
           pol_syms)

    print(f"version-delta: {buka_image.name} vs {pol_image.name}")
    results = analyse(rows, buka, pol, include_known=args.include_known)
    summarise(results, len(rows))

    if args.write:
        results.sort(key=lambda row: (-row["_rank"], row["unit"], row["symbol"]))
        args.output.parent.mkdir(parents=True, exist_ok=True)
        with args.output.open("w", encoding="utf-8", newline="") as stream:
            stream.write(
                "# Generated by `homm2 audit version-delta`; do not hand-edit.\n"
                f"# {buka_image} vs {pol_image}\n"
                "# CANDIDATES, not findings: VC 4.2 and VC6 disagree about inlining\n"
                "# and constant chunking, so read both bodies before writing a row\n"
                "# into docs/version-changes.md.\n"
                "# added/removed are from Buka's side: 'added' is in 2.1 only.\n")
            writer = csv.DictWriter(stream, fieldnames=FIELDS, delimiter="\t",
                                    lineterminator="\n", extrasaction="ignore")
            writer.writeheader()
            for row in results:
                writer.writerow(row)
        print(f"version-delta: wrote {args.output}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
