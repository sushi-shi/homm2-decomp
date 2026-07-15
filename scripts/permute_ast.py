#!/usr/bin/env python3
"""Legacy CLI compatibility wrapper for scripts/match_variants.py.

The former in-place AST hill climber has been consolidated into the unified manifest
search.  This wrapper accepts the historical positional arguments, resolves the symbol's
RVA, and runs the libclang-backed exact-only pipeline.  It never writes a sub-100 best
candidate into the source file.

Historical usage::

    python3 scripts/permute_ast.py <src.cpp> <TU> <mangled-symbol> [variant-limit]
"""

from __future__ import annotations

import argparse
import csv
import os
import time
from pathlib import Path

from generate_ast_variants import main as unified_main


def resolve_rva(root: Path, unit: str, symbol: str) -> int:
    matches = []
    with (root / "build/gen/symbol_names.csv").open(encoding="latin-1") as handle:
        for row in csv.DictReader(handle):
            if row.get("unit") == unit and row.get("name") == symbol and row.get("kind") == "func":
                matches.append(int(row["rva"], 0))
    if len(matches) != 1:
        raise ValueError(
            f"expected one CodeView function for {unit}::{symbol}, found {len(matches)}"
        )
    return matches[0]


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path)
    parser.add_argument("unit")
    parser.add_argument("symbol")
    parser.add_argument("variant_limit", nargs="?", type=int, default=300)
    args = parser.parse_args(argv)
    if args.variant_limit < 1:
        parser.error("variant-limit must be positive")

    root = Path(os.environ.get("HOMM2_DIR", Path.cwd())).resolve()
    try:
        rva = resolve_rva(root, args.unit, args.symbol)
    except (OSError, KeyError, ValueError) as exc:
        parser.error(str(exc))
    stamp = time.strftime("%Y%m%d-%H%M%S")
    manifest = root / "build/source-variant-manifests" / f"{stamp}-{args.unit.replace('/', '-')}-0x{rva:x}.json"
    manifest.parent.mkdir(parents=True, exist_ok=True)
    print(
        "permute_ast.py is a compatibility wrapper; using the unified exact-only "
        "match_variants.py pipeline",
        flush=True,
    )
    return unified_main([
        str(args.source), hex(rva),
        "--min-depth", "1", "--max-depth", "3",
        "--limit", str(args.variant_limit),
        "--output", str(manifest), "--run",
    ])


if __name__ == "__main__":
    raise SystemExit(main())
