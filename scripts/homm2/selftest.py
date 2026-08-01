"""homm2.selftest - `homm2 selftest [package]` - run the tool test suite.

The tests are colocated with what they test, one test_<module>.py beside each
<module>.py, and this is what runs them. Before it existed nothing did: there is
no CI, no flake check, no git hook and no ninja target, so a suite that had grown
to ~500 cases only ran when somebody typed a command by hand.

What it guards is not the decompilation - the retail bytes do that - but the
tools that can damage it in ways objdiff cannot see: the permuter edits src/ in
place and must restore it byte-identically even on exception or timeout, the
sweep drivers write config/match_baseline.tsv where an overwritten MAX row cannot
be recovered by reverting, the variant generator must never emit a shape that
changes what the program does, and the clean generator deletes directories and
publishes a branch.

A minimum case count is asserted because the failure mode here is silence, not
red: a package directory missing __init__.py is not importable, so unittest
discovery walks straight past it and still reports OK. That happened while
carving homm2/audit/ - 32 cases vanished and the run stayed green at 425.

    homm2 selftest              # everything
    homm2 selftest permute      # one package
    homm2 selftest --list       # per-package counts, run nothing
"""
import sys
import unittest

from homm2.core.paths import REPO

SCRIPTS = REPO / "scripts"
PACKAGE = SCRIPTS / "homm2"

# Raised deliberately when cases are ADDED - the number is a floor against silent
# loss, not a target. If a package legitimately shrinks, lower it in the same
# commit that removes the tests, so the drop is reviewed rather than absorbed.
MINIMUM_CASES = 517


def _packages():
    return sorted(
        path.parent.name
        for path in PACKAGE.glob("*/__init__.py")
        if any(path.parent.glob("test_*.py"))
    )


def _count(suite):
    if isinstance(suite, unittest.TestCase):
        return 1
    return sum(_count(child) for child in suite)


def _discover(package=None):
    start = str(PACKAGE / package) if package else str(PACKAGE)
    return unittest.defaultTestLoader.discover(
        start_dir=start, top_level_dir=str(SCRIPTS), pattern="test_*.py")


def main(argv=None):
    argv = list(sys.argv[1:] if argv is None else argv)
    verbose = "-v" in argv or "--verbose" in argv
    argv = [a for a in argv if a not in ("-v", "--verbose")]

    if "--list" in argv:
        total = _count(_discover())
        grouped = 0
        for name in _packages():
            count = _count(_discover(name))
            grouped += count
            print(f"  {name:<10} {count:>4}")
        # cli / redelink / constants_audit sit directly under homm2/, not in a
        # role package, so they are the remainder rather than a listed row.
        print(f"  {'(root)':<10} {total - grouped:>4}")
        print(f"  {'TOTAL':<10} {total:>4}")
        return 0

    if len(argv) > 1:
        print("usage: homm2 selftest [package] [--list] [-v]", file=sys.stderr)
        return 1

    package = argv[0] if argv else None
    if package and package not in _packages():
        print(f"homm2 selftest: no tests in package {package!r}; have: "
              + ", ".join(_packages()), file=sys.stderr)
        return 1

    suite = _discover(package)
    found = _count(suite)
    result = unittest.TextTestRunner(verbosity=2 if verbose else 1).run(suite)
    if not result.wasSuccessful():
        return 1
    # Only meaningful for a whole-tree run; a single package is a deliberate subset.
    if package is None and found < MINIMUM_CASES:
        print(f"homm2 selftest: FAILED - discovered {found} cases, expected at least "
              f"{MINIMUM_CASES}. A package is probably missing __init__.py and was "
              f"skipped silently.", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
