#!/usr/bin/env python3
"""Diff each parity-gap function's SOURCE against the PoL 2.0 tree's copy of it.

`homm2 audit cross-version` says which functions the 2.0 line reached and this one
has not. It cannot say why, and the two explanations want opposite work:

  the bodies are the same    the reconstruction is intact and the residual is
                             compiler-and-flags - go straight to `homm2 permute`,
                             the slot model, and the control-flow detectors.
  the bodies differ          either this branch's copy drifted during the port (a
                             regression to revert) or 2.1 genuinely changed the
                             function (a row for docs/version-changes.md).

Sorting the gap that way before touching anything is the cheapest triage available,
because both trees hold the same reconstruction and the comparison is textual.

Classification, weakest evidence last:

  identical   the same text once the VA marker is removed. The marker legitimately
              differs - different image, different address and size.
  formatting  the same tokens; only whitespace, comments and layout differ.
  renames     the same token shape under a consistent 1:1 identifier map. This is
              the expected outcome for a large part of the gap: local names are
              per-compiler slot-bucket pins, so VC6 forces different ones than 4.2
              did for the same code.
  divergent   anything else, with a token-level similarity so the queue can rank.

The tokenizer is deliberate but approximate - enough C++ to compare bodies, not a
parser. It is used only to classify, never to rewrite.

    homm2 audit cross-version-bodies                 # summary
    homm2 audit cross-version-bodies --write         # + the TSV
    homm2 audit cross-version-bodies --lead open-both
"""
from __future__ import annotations

import argparse
import csv
import difflib
import re
import sys
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

OUTPUT = Path("docs/cross-version-bodies.tsv")
IMAGE_BASE = 0x400000

# The marker opening a claimed function span. Matching at line start keeps a VA(...)
# mentioned inside a comment or string from splitting a body.
VA_MARKER = re.compile(r"^[ \t]*VA\(0x([0-9a-f]{8}),\s*(0x[0-9a-f]+)\)[ \t]*$",
                       re.M | re.I)

TOKEN = re.compile(r"""
      (?P<ws>\s+)
    | (?P<comment>//[^\n]*|/\*.*?\*/)
    | (?P<string>"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*')
    | (?P<number>0[xX][0-9a-fA-F]+[uUlL]*|\d+\.?\d*(?:[eE][+-]?\d+)?[uUlLfF]*)
    | (?P<ident>[A-Za-z_]\w*)
    | (?P<punct><<=|>>=|\.\.\.|->\*|[-+*/%^&|<>=!]=|<<|>>|\+\+|--|->|::|&&|\|\||.)
""", re.X | re.S)

FIELDS = ("body", "lead", "unit", "symbol", "similarity", "renames",
          "buka_tokens", "pol_tokens", "transition", "unmatched_bytes")


def unit_sources(root):
    """{unit: absolute source path} from units.toml - the authoritative mapping.

    Deriving the unit from the path instead would be guessing at a convention the
    manifest already states, and the manifest is what the build reads.
    """
    text = (root / "config/units.toml").read_text()
    sources = {}
    for block in re.finditer(r"\[\[unit\]\](.*?)(?=\[\[unit\]\]|\Z)", text, re.S):
        body = block.group(1)
        unit = re.search(r'unit\s*=\s*"([^"]+)"', body)
        source = re.search(r'source\s*=\s*"([^"]+)"', body)
        if unit and source:
            sources[unit.group(1)] = root / source.group(1)
    return sources


def spans_by_rva(root):
    """{(unit, rva): body text} for every claimed function in the tree.

    A span runs from its own VA marker to the next one, which is exactly the
    convention `homm2.permute.recover_historical_exact` mutates against, so the two
    tools always agree on where a function begins and ends.
    """
    spans = {}
    for unit, path in unit_sources(root).items():
        if not path.exists():
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        markers = list(VA_MARKER.finditer(text))
        for index, marker in enumerate(markers):
            end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
            rva = int(marker.group(1), 16) - IMAGE_BASE
            # Drop the marker line: its address and size differ between images by
            # definition, and comparing them would classify every body as divergent.
            spans[(unit, rva)] = text[marker.end():end]
    return spans


def tokenize(text):
    """Significant tokens only: whitespace and comments carry no codegen meaning."""
    tokens = []
    for match in TOKEN.finditer(text):
        kind = match.lastgroup
        if kind in ("ws", "comment"):
            continue
        tokens.append((kind, match.group()))
    return tokens


def rename_map(ours, theirs):
    """Return a consistent 1:1 identifier map turning `theirs` into `ours`, or None.

    Consistency both ways matters. A one-way map would happily fold two distinct
    locals onto one name and call the bodies equivalent when they are not.
    """
    if len(ours) != len(theirs):
        return None
    forward, backward = {}, {}
    for (our_kind, our_text), (their_kind, their_text) in zip(ours, theirs):
        if our_kind != their_kind:
            return None
        if our_kind != "ident":
            if our_text != their_text:
                return None
            continue
        if forward.setdefault(their_text, our_text) != our_text:
            return None
        if backward.setdefault(our_text, their_text) != their_text:
            return None
    return {a: b for a, b in forward.items() if a != b}


def classify(ours, theirs):
    """Compare two function bodies. Returns (class, similarity, renames)."""
    if ours is None or theirs is None:
        return "missing", None, {}
    if ours == theirs:
        return "identical", 1.0, {}
    our_tokens, their_tokens = tokenize(ours), tokenize(theirs)
    if our_tokens == their_tokens:
        return "formatting", 1.0, {}
    renames = rename_map(our_tokens, their_tokens)
    if renames is not None:
        return "renames", 1.0, renames
    similarity = difflib.SequenceMatcher(
        a=[text for _, text in their_tokens],
        b=[text for _, text in our_tokens],
        autojunk=False,
    ).ratio()
    return "divergent", similarity, {}


def compare(rows, buka_spans, pol_spans, buka_syms, pol_syms):
    """Attach a body classification to every row that has a span on both sides."""
    buka_rva = {key: rva for key, (rva, _size) in buka_syms.items()}
    pol_rva = {key: rva for key, (rva, _size) in pol_syms.items()}
    results = []
    for row in rows:
        key = (row["unit"], row["symbol"])
        ours = buka_spans.get((row["unit"], buka_rva.get(key)))
        theirs = pol_spans.get((row["unit"], pol_rva.get(key)))
        body, similarity, renames = classify(ours, theirs)
        results.append({
            "body": body,
            "lead": row["lead"],
            "unit": row["unit"],
            "symbol": row["symbol"],
            "similarity": similarity,
            "renames": renames,
            "buka_tokens": len(tokenize(ours)) if ours else 0,
            "pol_tokens": len(tokenize(theirs)) if theirs else 0,
            "transition": row["transition"],
            "unmatched_bytes": row["unmatched_bytes"],
        })
    return results


def summarise(results, stream=sys.stdout):
    counts, by_transition = {}, {}
    for row in results:
        counts[row["body"]] = counts.get(row["body"], 0) + 1
        if row["body"] in ("identical", "formatting", "renames"):
            by_transition[row["transition"]] = by_transition.get(row["transition"], 0) + 1
    for name in ("identical", "formatting", "renames", "divergent", "missing"):
        if name in counts:
            print(f"cross-version-bodies: {name:<12} {counts[name]}", file=stream)
    if by_transition:
        print("cross-version-bodies: same-source split  " + ", ".join(
            f"{name} {count}" for name, count in sorted(by_transition.items())),
            file=stream)


def format_row(row):
    out = dict(row)
    out["similarity"] = "" if row["similarity"] is None else f"{row['similarity']:.3f}"
    out["renames"] = " ".join(
        f"{old}={new}" for old, new in sorted(row["renames"].items()))
    return out


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--reference", help="path to the 2.0 checkout")
    parser.add_argument("--lead", default="parity-gap",
                        help="which cross-version class to diff (default parity-gap; "
                             "'all' for every paired function)")
    parser.add_argument("--write", action="store_true", help=f"write {OUTPUT}")
    parser.add_argument("--output", type=Path, default=REPO / OUTPUT)
    args = parser.parse_args(argv)

    try:
        reference = find_reference(args.reference)
    except ReferenceError as error:
        print(f"cross-version-bodies: {error}", file=sys.stderr)
        return 1

    buka_syms, pol_syms = read_symbols(REPO), read_symbols(reference)
    rows, _envelope = build_rows(
        (read_report(REPO), read_ledger(REPO), buka_syms, read_profiles(REPO)),
        (read_report(reference), read_ledger(reference), pol_syms,
         read_profiles(reference)))
    if args.lead != "all":
        rows = [row for row in rows if row["lead"] == args.lead]

    results = compare(rows, spans_by_rva(REPO), spans_by_rva(reference),
                      buka_syms, pol_syms)
    print(f"cross-version-bodies: reference {reference}")
    summarise(results)

    if args.write:
        # Same-source first - that is the queue that goes straight to the permuter -
        # then divergent bodies ordered by how far apart they are.
        order = {"identical": 0, "formatting": 1, "renames": 2,
                 "divergent": 3, "missing": 4}
        results.sort(key=lambda row: (order[row["body"]], row["similarity"] or 0.0,
                                      -row["unmatched_bytes"]))
        args.output.parent.mkdir(parents=True, exist_ok=True)
        with args.output.open("w", encoding="utf-8", newline="") as stream:
            stream.write(
                "# Generated by `homm2 audit cross-version-bodies`; do not hand-edit.\n"
                f"# reference: {reference}\n"
                "# body: identical/formatting/renames = the reconstruction is intact,\n"
                "#       so the residual is compiler-and-flags - hand it to the permuter.\n"
                "#       divergent = this branch's copy drifted, or 2.1 changed the\n"
                "#       function; read the bodies before assuming which.\n")
            writer = csv.DictWriter(stream, fieldnames=FIELDS, delimiter="\t",
                                    lineterminator="\n")
            writer.writeheader()
            for row in results:
                writer.writerow(format_row(row))
        print(f"cross-version-bodies: wrote {args.output}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
