"""homm2.verify.behaviour - `homm2 verify behaviour` - game-behaviour tests.

These tests exercise what the reconstructed program does, not how the tooling
matches it: they catch a behaviour change slipping in during cleanup or
review, where a byte-identical comparison is not the question being asked.

  test_game_contracts    compiles game_contracts.cpp against the real game
                         headers with the SOURCE/KB compiler profile, links it
                         with VC6 and runs it under wine: enum protocol values,
                         CP1251 case folding, creature classification, map-cell
                         and player predicates, combat-hex and distance
                         formulas, and the file-value record round trip.
  test_localization_text the shipped catalog's text: original English
                         wording, format arguments that render complete
                         combat, trading and requester sentences, and a tree
                         with no text outside the catalog.

Tests live in this package as test_*.py and run with unittest. The gate fails
when a test fails or errors, and when no test ran at all.
"""
from __future__ import annotations

import argparse
import sys
import unittest
from pathlib import Path

from homm2.core.usage import logged

HERE = Path(__file__).resolve().parent


@logged
def main(argv=None) -> int:
    parser = argparse.ArgumentParser(prog="homm2 verify behaviour",
                                     description=__doc__.splitlines()[0])
    parser.add_argument("-k", dest="pattern", action="append", default=[],
                        help="run only tests whose name contains PATTERN")
    parser.add_argument("-v", "--verbose", action="store_true")
    args = parser.parse_args(argv)
    loader = unittest.TestLoader()
    if args.pattern:
        loader.testNamePatterns = [f"*{p}*" for p in args.pattern]
    suite = loader.discover(str(HERE), pattern="test_*.py",
                            top_level_dir=str(HERE.parents[2]))
    result = unittest.TextTestRunner(stream=sys.stderr,
                                     verbosity=2 if args.verbose else 1).run(suite)
    if result.testsRun == 0:
        print("[behaviour] no test ran", file=sys.stderr)
        return 1
    return 0 if result.wasSuccessful() and not result.skipped else 1


if __name__ == "__main__":
    sys.exit(main())
