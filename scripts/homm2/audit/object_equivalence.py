"""Strict before/after control, not a retail matcher or a normalization pass.

Compare every non-debug section, byte, relocation and named symbol. Only the
numeric suffix of local $SG/$T/$L symbols may change; its section, value, type
and storage must still agree. No input object is modified.
"""
import argparse
import json
import re
import sys
from pathlib import Path
from homm2.core.coff import CoffObject


def is_counter_local(symbol):
    if symbol.section <= 0 or symbol.typ != 0:
        return False
    # VC6 uses STATIC (3) for data counters and LABEL (6) for code labels.
    pattern = (r'\$(?:SG|T)\d+' if symbol.storage_class == 3
               else r'(?:\$L|\$[A-Za-z_]\w*\$)\d+')
    return (symbol.storage_class in (3, 6)
            and re.fullmatch(pattern, symbol.name) is not None)


def symbol_key(symbol):
    name = symbol.name
    if is_counter_local(symbol):
        name = re.sub(r'\d+$', '#', name)
    return name, symbol.section, symbol.value, symbol.typ, symbol.storage_class


def compare(before, after):
    a, b = CoffObject(before), CoffObject(after)
    errors = []
    sections_a = [s for s in a.sections if not s.name.startswith('.debug')]
    sections_b = [s for s in b.sections if not s.name.startswith('.debug')]
    if len(sections_a) != len(sections_b):
        errors.append('section count')
    for x, y in zip(sections_a, sections_b):
        if (x.index, x.name, x.raw_size, x.characteristics) != (y.index, y.name, y.raw_size, y.characteristics):
            errors.append('section layout: ' + x.name)
        if a.section_bytes(x) != b.section_bytes(y):
            errors.append('section bytes: ' + x.name)

    def relocation_keys(obj):
        return [(r.section, r.site, r.typ, symbol_key(obj.symbols[r.symbol_index]))
                for r in obj.relocations if not obj.section(r.section).name.startswith('.debug')]

    ar, br = relocation_keys(a), relocation_keys(b)
    if ar != br:
        errors.append('ordered relocations')
    # File/section bookkeeping is checked above. Check every nonlocal semantic
    # symbol independently, including ones that no relocation currently uses.
    def named_symbols(obj):
        return sorted(symbol_key(s) for s in obj.symbols.values()
                      if s.storage_class in (2, 3, 105) and not s.name.startswith('.')
                      and not is_counter_local(s))
    if named_symbols(a) != named_symbols(b):
        errors.append('named symbols')
    return dict(errors=errors, sections=len(sections_a), relocations=len(ar))


def compare_directories(baseline: Path, current: Path):
    report = {}
    for before in sorted(baseline.rglob('*.obj')):
        relative = before.relative_to(baseline)
        after = current / relative
        report[str(relative)] = (compare(before.read_bytes(), after.read_bytes())
                                 if after.is_file() else dict(errors=['missing object']))
    added = sorted(str(p.relative_to(current)) for p in current.rglob('*.obj')
                   if not (baseline / p.relative_to(current)).exists())
    return dict(objects=report, added=added,
                passed=bool(report) and not added and not any(r['errors'] for r in report.values()))


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('baseline', type=Path)
    parser.add_argument('current', type=Path)
    args = parser.parse_args(argv)
    if not args.baseline.is_dir() or not args.current.is_dir():
        parser.error('both object directories must exist')
    result = compare_directories(args.baseline, args.current)
    print(json.dumps(result, indent=2))
    return int(not result['passed'])


if __name__ == '__main__':
    sys.exit(main())
