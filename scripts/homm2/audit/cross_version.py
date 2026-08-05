#!/usr/bin/env python3
"""Rank this branch's functions against what the PoL 2.0 line already achieved.

This branch was seeded from the 2.0 reconstruction, so almost every function here
has a proven-correct 2.0 body behind it. But the target changed underneath: a
different image, VC6 SP5 instead of VC 4.2, and ~40 TUs moved /O2 -> /Od. Work the
2.0 line had banked therefore reads as unmatched here, and nothing distinguished
"never solved" from "solved once, against a different compiler".

`homm2 audit historical-losses` answers the same question for this repository's own
history. It cannot see the 2.0 line at all, because that evidence lives in another
checkout's ledger and reports. This joins the two.

The join key is (unit, mangled symbol): both trees reconstruct the same source, so
the mangled name is stable across them even where the retail address is not.
`docs/buka-va-queue.tsv` carries the address mapping (`pol_va`) and is used only to
cross-check the pairing, never to make it.

What the 2.0 evidence is worth depends on the flag change, so every paired row
carries its profile transition:

  base -> base     /Od on both sides. Control-flow shape, operand order and loop
                   shape all transfer; the residual is the VC4.2-vs-VC6 slot-bucket
                   hash split, or a real 2.1 body change.
  o2   -> base     2.0 proved semantics only. /O2 regalloc erased every /Od-visible
                   detail, so slot order, local names and evaluation order are
                   unproven - new work, but with a known-correct body in hand.

Retail size delta is reported but is NOT a version-change detector on its own: it is
dominated by known systematics (asserts compiled out of all but five TUs, no /Gi line
machinery, /QIfdiv off), which is why the both-exact population - same source,
matched in both images - is measured each run to calibrate the envelope. Only rows
outside that envelope are flagged as body-change candidates.

    homm2 audit cross-version                       # summary
    homm2 audit cross-version --write               # + docs/cross-version-parity.tsv
    homm2 audit cross-version --reference ../homm2-decomp
"""
from __future__ import annotations

import argparse
import csv
import json
import os
import re
import subprocess
import sys
from pathlib import Path

from homm2.core.paths import REPO

REFERENCE_BRANCH = "decomp-pol-2.0"
REFERENCE_ENV = "HOMM2_POL_DIR"
OUTPUT = Path("docs/cross-version-parity.tsv")
VA_QUEUE = Path("docs/buka-va-queue.tsv")

REPORT = Path("build/objdiff/report.json")
LEDGER = Path("config/match_baseline.tsv")
SYMBOLS = Path("build/gen/symbol_names.csv")
UNITS = Path("config/units.toml")

EXACT = 100.0
# /Od-family profiles. The distinction that matters is whether the 2.0 build kept
# the frame - an /O2 unit erases the detail this campaign has to reproduce.
OD_PROFILES = {"base", "base_oi", "base_nogf", "base_gx", "base_gf"}
# Percentile band of the calibrated size envelope. Wide on purpose: the systematics
# it absorbs are large and one-sided (2.1 bodies are typically smaller).
ENVELOPE_LOW, ENVELOPE_HIGH = 0.02, 0.98

FIELDS = (
    "lead",
    "unit",
    "symbol",
    "buka_best",
    "pol_best",
    "buka_live",
    "buka_max",
    "pol_live",
    "pol_max",
    "unmatched_bytes",
    "buka_rva",
    "pol_rva",
    "buka_size",
    "pol_size",
    "size_delta",
    "size_rel",
    "profile",
    "transition",
    "size_outlier",
)


class ReferenceError(RuntimeError):
    pass


def find_reference(explicit=None):
    """Locate the 2.0 checkout: explicit flag, then env, then the worktree list.

    The worktree lookup is the reliable one - this repository's branches are all
    worktrees of a single clone, so git already knows where `decomp-pol-2.0` lives
    and no path needs to be configured anywhere.
    """
    for candidate in (explicit, os.environ.get(REFERENCE_ENV)):
        if candidate:
            path = Path(candidate).expanduser().resolve()
            if not (path / REPORT).exists():
                raise ReferenceError(
                    f"{path} has no {REPORT} - is it a provisioned checkout?")
            return path
    listing = subprocess.run(
        ("git", "worktree", "list", "--porcelain"),
        cwd=REPO, text=True, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    path = None
    for line in listing.stdout.splitlines():
        if line.startswith("worktree "):
            path = Path(line[len("worktree "):])
        elif line == f"branch refs/heads/{REFERENCE_BRANCH}" and path is not None:
            return path.resolve()
    raise ReferenceError(
        f"no worktree on branch {REFERENCE_BRANCH}; pass --reference or set "
        f"{REFERENCE_ENV}")


def read_report(root):
    """Live objdiff scores and sizes, keyed (unit, symbol).

    objdiff omits `fuzzy_match_percent` entirely when a function matches nothing,
    so a missing key is 0.0 and not missing data. Modules in parentheses -
    (unmatched), (libcmt), (funclets), (imports) - are not reconstruction targets.
    """
    report = json.loads((root / REPORT).read_text())
    rows = {}
    for unit in report["units"]:
        if unit["name"].startswith("("):
            continue
        for function in unit.get("functions", ()):
            rows[(unit["name"], function["name"])] = (
                float(function.get("fuzzy_match_percent") or 0.0),
                int(function.get("size", 0)),
            )
    return rows


def read_ledger(root):
    """Banked maxima, keyed (unit, symbol). A missing ledger is an empty campaign."""
    path = root / LEDGER
    rows = {}
    if not path.exists():
        return rows
    for line in path.read_text().splitlines():
        if not line.strip() or line.startswith("#"):
            continue
        unit, symbol, score, _hash = line.split("\t")
        rows[(unit, symbol)] = float(score)
    return rows


def read_symbols(root):
    """Retail address and size per function, from the claimed inventory."""
    rows = {}
    with (root / SYMBOLS).open(encoding="latin-1", newline="") as stream:
        for row in csv.DictReader(stream):
            if row.get("kind") != "func":
                continue
            rows[(row["unit"], row["name"])] = (
                int(row["rva"], 16), int(row["size"], 16))
    return rows


def read_profiles(root):
    """Per-unit flag profile from units.toml.

    A unit with no explicit `flags` takes the base profile, which is what the build
    does; spelling that out here keeps the transition column honest for the units
    that never needed an override.
    """
    text = (root / UNITS).read_text()
    profiles = {}
    for block in re.finditer(r"\[\[unit\]\](.*?)(?=\[\[unit\]\]|\Z)", text, re.S):
        body = block.group(1)
        unit = re.search(r'unit\s*=\s*"([^"]+)"', body)
        flags = re.search(r'flags\s*=\s*"([^"]+)"', body)
        if unit:
            profiles[unit.group(1)] = flags.group(1) if flags else "base"
    return profiles


def read_va_queue(root):
    """The attempt-1 address remap: {(unit, buka_rva): pol_va}, for cross-checking."""
    path = root / VA_QUEUE
    rows = {}
    if not path.exists():
        return rows
    with path.open(encoding="utf-8", newline="") as stream:
        for line in stream:
            if line.startswith("#"):
                continue
            fields = line.rstrip("\n").split("\t")
            if len(fields) < 5 or fields[1] == "buka_rva":
                continue
            if fields[4]:
                rows[(fields[0], int(fields[1], 16))] = int(fields[4], 16)
    return rows


def best_known(key, live, maxima):
    """Best evidence for a function: the live score or a banked maximum.

    A banked maximum is weaker evidence than a live score - it says the current
    source reached that number once, under a TU state the tree no longer holds -
    but it is the standard the 2.0 line's own headline number uses, so parity has
    to be measured against it.
    """
    entry = live.get(key)
    scores = [value for value in (entry[0] if entry else None, maxima.get(key))
              if value is not None]
    return max(scores) if scores else None


def transition(pol_profile, buka_profile):
    """Classify what the flag change did to the 2.0 proof."""
    if pol_profile is None or buka_profile is None:
        return "unknown"
    if pol_profile in OD_PROFILES and buka_profile in OD_PROFILES:
        return "od-od"
    if pol_profile in OD_PROFILES or buka_profile in OD_PROFILES:
        return "o2-od" if buka_profile in OD_PROFILES else "od-o2"
    return "other"


def percentile(values, fraction):
    """Nearest-rank percentile over a sorted-in-place copy. Empty input has none."""
    if not values:
        return None
    ordered = sorted(values)
    index = min(len(ordered) - 1, max(0, int(fraction * len(ordered))))
    return ordered[index]


def size_envelope(rows):
    """Calibrate the compiler-noise band per transition class from both-exact rows.

    Both-exact means the same source matched in both images, so every byte of size
    delta there is compiler and build-flag difference and none of it is a version
    change. Measuring the band each run - rather than hardcoding one - keeps the
    outlier flag meaningful as the campaign moves.
    """
    samples = {}
    for row in rows:
        if row["lead"] != "both-exact" or row["size_rel"] is None:
            continue
        samples.setdefault(row["transition"], []).append(row["size_rel"])
    return {
        name: (percentile(values, ENVELOPE_LOW), percentile(values, ENVELOPE_HIGH))
        for name, values in samples.items()
        if len(values) >= 20  # too few to call a band; leave the class uncalibrated
    }


def build_rows(buka, pol):
    """Join both trees into one row per function, tagged by what it needs."""
    buka_live, buka_max, buka_syms, buka_prof = buka
    pol_live, pol_max, pol_syms, pol_prof = pol

    rows = []
    for key in sorted(set(buka_live) | set(pol_live)):
        unit, symbol = key
        in_buka, in_pol = key in buka_live, key in pol_live
        bb = best_known(key, buka_live, buka_max)
        pb = best_known(key, pol_live, pol_max)

        if not in_pol:
            lead = "buka-only"
        elif not in_buka:
            lead = "pol-only"
        elif pb == EXACT and bb == EXACT:
            lead = "both-exact"
        elif pb == EXACT:
            lead = "parity-gap"
        elif bb == EXACT:
            lead = "forward"
        else:
            lead = "open-both"

        buka_size = buka_syms.get(key, (None, None))[1]
        pol_size = pol_syms.get(key, (None, None))[1]
        delta = rel = None
        if buka_size is not None and pol_size:
            delta = buka_size - pol_size
            rel = delta / pol_size

        live_score, live_size = buka_live.get(key, (0.0, 0))
        rows.append({
            "lead": lead,
            "unit": unit,
            "symbol": symbol,
            "buka_best": bb,
            "pol_best": pb,
            "buka_live": buka_live.get(key, (None,))[0],
            "buka_max": buka_max.get(key),
            "pol_live": pol_live.get(key, (None,))[0],
            "pol_max": pol_max.get(key),
            # Size-weighted residue: what closing this function is worth to fuzzy.
            # This is a different ranking from the function count, and both matter.
            "unmatched_bytes": round(live_size * (1.0 - live_score / 100.0)),
            "buka_rva": buka_syms.get(key, (None,))[0],
            "pol_rva": pol_syms.get(key, (None,))[0],
            "buka_size": buka_size,
            "pol_size": pol_size,
            "size_delta": delta,
            "size_rel": rel,
            "profile": f"{pol_prof.get(unit, '?')}->{buka_prof.get(unit, '?')}",
            "transition": transition(pol_prof.get(unit), buka_prof.get(unit)),
            "size_outlier": "",
        })

    envelope = size_envelope(rows)
    for row in rows:
        band = envelope.get(row["transition"])
        if row["lead"] == "parity-gap" and band and row["size_rel"] is not None:
            low, high = band
            if not low <= row["size_rel"] <= high:
                row["size_outlier"] = "yes"
    return rows, envelope


def check_pairing(rows, va_queue, buka_syms):
    """Cross-check the name join against the attempt-1 address remap.

    The join is by mangled name; the queue independently recorded which PoL address
    each Buka address came from. Where both exist they must agree, and a mismatch
    means one of the two claims is wrong - worth knowing before the queue is trusted.
    """
    conflicts = []
    for row in rows:
        if row["pol_rva"] is None or row["buka_rva"] is None:
            continue
        claimed = va_queue.get((row["unit"], row["buka_rva"]))
        if claimed is None:
            continue
        if claimed - 0x400000 != row["pol_rva"]:
            conflicts.append((row["unit"], row["symbol"], claimed, row["pol_rva"]))
    return conflicts


def format_row(row):
    out = {}
    for field in FIELDS:
        value = row[field]
        if value is None:
            out[field] = ""
        elif field in ("buka_rva", "pol_rva"):
            out[field] = f"0x{value:x}"
        elif field in ("buka_size", "pol_size"):
            out[field] = f"0x{value:x}"
        elif field == "size_rel":
            out[field] = f"{value:+.3f}"
        elif field.endswith(("_best", "_live", "_max")):
            out[field] = f"{value:.4f}"
        else:
            out[field] = value
    return out


def summarise(rows, envelope, stream=sys.stdout):
    counts = {}
    for row in rows:
        counts[row["lead"]] = counts.get(row["lead"], 0) + 1

    def show(label, value):
        print(f"cross-version: {label:<22} {value}", file=stream)

    for lead in ("both-exact", "parity-gap", "forward", "open-both",
                 "buka-only", "pol-only"):
        if lead in counts:
            show(lead, counts[lead])

    gap = [row for row in rows if row["lead"] == "parity-gap"]
    if not gap:
        return
    by_transition = {}
    for row in gap:
        by_transition[row["transition"]] = by_transition.get(row["transition"], 0) + 1
    show("gap by transition", ", ".join(
        f"{name} {count}" for name, count in sorted(by_transition.items())))

    bands = [("<80", 0, 80), ("80-95", 80, 95), ("95-99", 95, 99), (">=99", 99, 101)]
    histogram = []
    for label, low, high in bands:
        count = sum(1 for row in gap if low <= (row["buka_best"] or 0) < high)
        histogram.append(f"{label} {count}")
    show("gap by buka score", ", ".join(histogram))

    show("gap unmatched bytes", f"{sum(row['unmatched_bytes'] for row in gap):,}")
    show("size outliers", sum(1 for row in gap if row["size_outlier"]))
    for name, (low, high) in sorted(envelope.items()):
        show(f"envelope {name}", f"{low:+.1%} .. {high:+.1%}")


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--reference", help="path to the 2.0 checkout")
    parser.add_argument("--write", action="store_true",
                        help=f"write {OUTPUT}")
    parser.add_argument("--output", type=Path, default=REPO / OUTPUT)
    args = parser.parse_args(argv)

    try:
        reference = find_reference(args.reference)
    except ReferenceError as error:
        print(f"cross-version: {error}", file=sys.stderr)
        return 1

    buka = (read_report(REPO), read_ledger(REPO),
            read_symbols(REPO), read_profiles(REPO))
    pol = (read_report(reference), read_ledger(reference),
           read_symbols(reference), read_profiles(reference))

    rows, envelope = build_rows(buka, pol)
    print(f"cross-version: reference {reference}", file=sys.stdout)
    summarise(rows, envelope)

    conflicts = check_pairing(rows, read_va_queue(REPO), buka[2])
    for unit, symbol, claimed, paired in conflicts:
        print(f"cross-version: PAIRING {unit} {symbol}\n"
              f"          va-queue claims pol 0x{claimed:x}, name join gives "
              f"0x{paired + 0x400000:x}", file=sys.stderr)
    if conflicts:
        print(f"cross-version: {len(conflicts)} pairing conflicts", file=sys.stderr)

    if args.write:
        # Queue order: the parity gap first, biggest fuzzy weight first inside each
        # class, so one file serves both the function-count and the percentage queue.
        order = {"parity-gap": 0, "open-both": 1, "buka-only": 2,
                 "forward": 3, "both-exact": 4, "pol-only": 5}
        rows.sort(key=lambda row: (order[row["lead"]], -row["unmatched_bytes"],
                                   row["unit"], row["symbol"]))
        args.output.parent.mkdir(parents=True, exist_ok=True)
        with args.output.open("w", encoding="utf-8", newline="") as stream:
            stream.write(
                "# Generated by `homm2 audit cross-version`; do not hand-edit.\n"
                f"# reference: {reference}\n"
                "# lead: parity-gap = 2.0 reached 100% and this branch has not.\n"
                "# transition: od-od = /Od both sides (shape transfers);\n"
                "#             o2-od = 2.0 proved semantics only, /Od detail is new work.\n"
                "# size_outlier: retail size delta outside the both-exact envelope\n"
                "#               for its transition class - a body-change candidate.\n")
            writer = csv.DictWriter(stream, fieldnames=FIELDS, delimiter="\t",
                                    lineterminator="\n")
            writer.writeheader()
            for row in rows:
                writer.writerow(format_row(row))
        print(f"cross-version: wrote {args.output}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
