"""Strict native object-tree comparison; no added owner/type normalization.

Usage: PYTHONPATH=scripts python3 docs/reconstruction/compare-object-trees.py BEFORE AFTER
The fixed98-object census is intentional for this Buka audit. Retail equality
must be checked separately; this proves only preservation of native objects.
"""
import argparse
import importlib.util
from pathlib import Path

spec = importlib.util.spec_from_file_location('comparison', Path(__file__).with_name('compare-objects.py'))
comparison = importlib.util.module_from_spec(spec)
spec.loader.exec_module(comparison)

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('before', type=Path)
    parser.add_argument('after', type=Path)
    args = parser.parse_args()
    paths = sorted(p.relative_to(args.before) for p in args.before.rglob('*.obj'))
    assert len(paths) == 98
    assert set(paths) == {p.relative_to(args.after) for p in args.after.rglob('*.obj')}
    functions = 0
    for path in paths:
        a = comparison.census(args.before/path)
        b = comparison.census(args.after/path)
        assert a == b, path
        functions += len(a[0])
        print('PASS', path, len(a[0]), 'functions; all allocated bytes/attributes and ordered relocations')
    print('PASS all98 native objects;', functions, 'emitted functions; no owner/type/addend exceptions')

if __name__ == '__main__':
    main()
