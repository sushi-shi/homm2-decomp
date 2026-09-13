"""Validate the pinned focused queue and its current-hash dispositions.

Run from the repository root with PYTHONPATH=scripts; optionally pass --ctags.
This validates bookkeeping, not the substantive code/retail evidence.
"""
import argparse
import csv
from pathlib import Path
from homm2.audit.reconstruction import inventory

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--ctags', default='ctags')
args = parser.parse_args()
root = Path.cwd()
directory = root/'docs/reconstruction/casts-pointers'
with (directory/'scope.tsv').open() as stream:
    scope = list(csv.DictReader(stream, delimiter='\t'))
assert len(scope) == len({row['id'] for row in scope}) == 148
functions, _, _ = inventory(root, args.ctags)
current = {row['id']: row for row in functions}
for row in scope:
    assert row['id'] in current
    assert len(row['baseline_body_sha256']) == 64
    for category in ('casts', 'address', 'outside'):
        assert row[category] in ('', 'pending', 'partial', 'fixed')
        if row[category] in ('partial', 'fixed'):
            assert row['reviewed_body_sha256'] == current[row['id']]['body_sha256'], row['id']
            assert row['evidence']
    if row['evidence']:
        assert (directory/row['evidence']).is_file()
for category, pinned in (('casts', 86), ('address', 9), ('outside', 66)):
    assert sum(bool(row[category]) for row in scope) == pinned
    closed = sum(row[category] == 'fixed' for row in scope)
    partial = sum(row[category] == 'partial' for row in scope)
    print(category, 'fixed', closed, 'open', pinned-closed, '(including', partial, 'partial)')
print('PASS pinned denominator and current body hashes; no broad-review recertification')
