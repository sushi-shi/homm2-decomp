#!/usr/bin/env python3
"""Ceiling gate: the historical native link may not drift further from retail.

    python3 -m homm2.build.link_diff            # compare, exit 1 on a rise
    python3 -m homm2.build.link_diff --update   # bank the current counts

Counts the bytes in which `build/link/historical/HMM2PL.exe` differs from the
retail control at fixed file offsets: the headers before the first section,
each section's raw data, the overlay after the last section, and the file-size
difference. `config/link_diff.tsv` holds the highest count each region
may have. A count above its ceiling, or a region the ceiling does not list,
fails; a lower count passes and is reported as bankable with `--update`.

This is a regression ceiling, not closure. `link_exe --audit-existing --strict`
remains the exact-executable check, and its report attributes the residual.
"""

from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path

from homm2.core.paths import REPO

RETAIL = REPO / "build/orig/HMM2PL.exe"
CANDIDATE = REPO / "build/link/historical/HMM2PL.exe"
CEILING = REPO / "config/link_diff.tsv"
HEADER = ("# Highest differing byte count per region of the historical native link\n"
          "# against retail. Written by `python3 -m homm2.build.link_diff --update`;\n"
          "# lower it when the residual shrinks, never raise it to admit a regression.\n"
          "region\tbytes\n")


def sections(data: bytes) -> list[tuple[str, int, int]]:
    """[(name, raw offset, raw size)] in section-table order."""
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    count = struct.unpack_from("<H", data, pe + 6)[0]
    optional = struct.unpack_from("<H", data, pe + 20)[0]
    rows = []
    for index in range(count):
        offset = pe + 24 + optional + 40 * index
        name = data[offset:offset + 8].rstrip(b"\0").decode("latin-1")
        raw_size, raw_pointer = struct.unpack_from("<II", data, offset + 16)
        rows.append((name, raw_pointer, raw_size))
    return rows


def _differing(retail: bytes, candidate: bytes) -> int:
    shared = min(len(retail), len(candidate))
    return (sum(1 for index in range(shared) if retail[index] != candidate[index])
            + abs(len(retail) - len(candidate)))


def regions(retail: bytes, candidate: bytes) -> dict[str, int]:
    """{region: differing bytes} over the retail section layout."""
    layout = sections(retail)
    counts = {"size": abs(len(retail) - len(candidate))}
    first = min((pointer for _name, pointer, size in layout if size), default=len(retail))
    counts["headers"] = _differing(retail[:first], candidate[:first])
    end = first
    for name, pointer, size in layout:
        counts[name] = _differing(retail[pointer:pointer + size],
                                  candidate[pointer:pointer + size])
        end = max(end, pointer + size)
    counts["overlay"] = _differing(retail[end:], candidate[end:])
    return counts


def read_ceiling(path: Path) -> dict[str, int]:
    if not path.is_file():
        return {}
    ceiling = {}
    for line in path.read_text().splitlines():
        if not line or line.startswith("#") or line.startswith("region\t"):
            continue
        name, count = line.split("\t")[:2]
        ceiling[name] = int(count)
    return ceiling


def write_counts(path: Path, counts: dict[str, int]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(HEADER + "".join(f"{name}\t{count}\n" for name, count in counts.items()))


def compare(counts: dict[str, int], ceiling: dict[str, int]) -> tuple[list[str], list[str]]:
    """Return (regressions, bankable) lines."""
    regressions, bankable = [], []
    for name, count in counts.items():
        limit = ceiling.get(name)
        if limit is None:
            regressions.append(f"{name}: {count} differing bytes; no ceiling banked")
        elif count > limit:
            regressions.append(f"{name}: {count} differing bytes > ceiling {limit}")
        elif count < limit:
            bankable.append(f"{name}: {count} < ceiling {limit}")
    return regressions, bankable


from homm2.core.usage import logged


@logged
def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--retail", type=Path, default=RETAIL)
    parser.add_argument("--candidate", type=Path, default=CANDIDATE)
    parser.add_argument("--ceiling", type=Path, default=CEILING)
    parser.add_argument("--stamp", type=Path,
                        help="also write the current counts here (the Ninja output)")
    parser.add_argument("--update", action="store_true",
                        help="bank the current counts as the ceiling")
    args = parser.parse_args(argv)
    counts = regions(args.retail.read_bytes(), args.candidate.read_bytes())
    total = sum(counts.values())
    summary = ", ".join(f"{name} {count}" for name, count in counts.items())
    if args.update:
        write_counts(args.ceiling, counts)
        print(f"link diff: banked {total} differing bytes ({summary}) -> {args.ceiling}")
    regressions, bankable = compare(counts, read_ceiling(args.ceiling))
    if args.stamp and not regressions:
        write_counts(args.stamp, counts)
    print(f"link diff: {total} differing bytes vs retail ({summary})")
    for line in bankable:
        print(f"link diff: bankable {line}; run `python3 -m homm2.build.link_diff --update`")
    for line in regressions:
        print(f"link diff: REGRESSION {line}", file=sys.stderr)
    return 1 if regressions else 0


if __name__ == "__main__":
    sys.exit(main())
