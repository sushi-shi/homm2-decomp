#!/usr/bin/env python3
"""Report match_baseline.tsv rows that no longer describe the current source.

Each row banks a score against the normalized source hash it was earned at, and
`homm2 status` resets a maximum whenever that hash moves - a changed hash starts a
new epoch. So a row whose banked hash differs from the live one is not merely old:
its score was earned under source that no longer exists, and until a build runs,
the queue reads a number that does not apply to the current code.

Nothing reported that. The drift is invisible because the ledger is only rewritten
by `homm2 build` / `homm2 status update`, so a source edit committed without one
leaves the row keyed to a dead hash and no command complains. That is how
BASE/TILESET's constructor sat stale for eleven days: its hash was banked
2026-07-20, 07442adb changed the tileset field order on 2026-07-30, and the drift
surfaced only when an unrelated build happened to rewrite the row.

That case was benign - the function was still exact, so the refreshed row kept
100.0000. The reverse costs more: a stale row can advertise a maximum the current
source never earned, which is exactly the evidence the queue trusts.

This reads source and the ledger only. It runs without a build, which is the point -
it answers "is the ledger current?" without doing the thing that makes it current.

    homm2 audit ledger            # summary, plus every drifted row
    homm2 audit ledger --quiet    # exit status only
"""
from __future__ import annotations

import argparse
import sys

from homm2.core.paths import REPO
from homm2.match.source_hashes import source_hashes

LEDGER = REPO / "config/match_baseline.tsv"


def read_ledger(path=None):
    """Parse the ledger into {(unit, symbol): (score, banked_hash)}.

    No ledger is a real state, not an error: a campaign that has banked nothing
    has nothing to be stale. `homm2 status` treats it the same way.
    """
    path = path or LEDGER
    rows = {}
    if not path.exists():
        return rows
    for line in path.read_text().splitlines():
        if not line.strip() or line.startswith("#"):
            continue
        unit, symbol, score, banked = line.split("\t")
        rows[(unit, symbol)] = (float(score), banked)
    return rows


def classify(rows, live):
    """Split ledger rows against the live source hashes.

    stale   banked under a source hash that no longer exists - the score is
            evidence for a previous epoch, not this one
    orphan  no live hash at all: the function was renamed, removed, or its VA
            annotation moved, so the row can never be refreshed by a build
    """
    stale, orphan = [], []
    for key, (score, banked) in sorted(rows.items()):
        current = live.get(key)
        if current is None:
            orphan.append((key, score, banked))
        elif current != banked:
            stale.append((key, score, banked, current))
    return stale, orphan


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--quiet", action="store_true",
                        help="report nothing; use the exit status")
    args = parser.parse_args(argv)

    rows = read_ledger()
    live = source_hashes()
    stale, orphan = classify(rows, live)

    if not args.quiet:
        for (unit, symbol), score, banked, current in stale:
            print(f"ledger: STALE  {unit} {symbol}\n"
                  f"          banked {score:.4f} at {banked}, source is now {current}")
        for (unit, symbol), score, banked in orphan:
            print(f"ledger: ORPHAN {unit} {symbol}\n"
                  f"          banked {score:.4f} at {banked}, no live function hash")
        print(f"ledger: {len(rows)} rows; {len(rows) - len(stale) - len(orphan)} current, "
              f"{len(stale)} stale, {len(orphan)} orphaned")
        if stale or orphan:
            print("ledger: run `homm2 build` to re-earn these rows against current source",
                  file=sys.stderr)

    return 1 if (stale or orphan) else 0


if __name__ == "__main__":
    raise SystemExit(main())
