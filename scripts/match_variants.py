#!/usr/bin/env python3
"""Unified source-variant search frontend.

This is the public entry point for the matching campaign. It computes the explicit product of
optional exact hand-authored choices, conservative libclang AST source shapes, and parser-visible
TU-state declarations/includes/functions. A candidate cap reduces source-shape breadth without
sampling only part of the state dimension. Source transformations are never regex-based, source
is restored after every compile, and generated source is emitted only for exact closure.

Example::

    python3 scripts/match_variants.py src/BASE/icon2bs.cpp 0xd2f90 \
        --axes-from /tmp/icon2bs-axes.json --state-trials 32 \
        --min-depth 2 --max-depth 3 --limit 512 \
        -o /tmp/icon2bs-combined.json --run

The historical ``permute_ast.py`` command is retained under ``scripts/archive`` only to
reproduce old audit-ledger commands. New searches should use this frontend.
"""

from generate_ast_variants import main as generate_main


def main(argv=None) -> int:
    return generate_main(argv, prog="match_variants.py", description=__doc__)


if __name__ == "__main__":
    raise SystemExit(main())
