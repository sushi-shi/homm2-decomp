"""Compare C01 compiled code to the preserved pre-edit native objects.

This checks raw instruction bytes, relocation sites/types/addends and targets.
Only the two deliberate signature renames and the repository's established
compiler-private symbol normalization are allowed. It is a before/after check,
not an assertion that every baseline function already matches retail.
"""
import argparse
import json
from pathlib import Path

from homm2.core.coff import CoffObject, MEM_EXECUTE, FUNCTION_TYPE, RELOCATION_WIDTHS
from homm2.build.canonicalize_data_symbols import canonicalize_coff

RENAMES = {
    "?Read13@resourceManager@@QAEXPAC@Z": "?Read13@resourceManager@@QAEXPAD@Z",
    "?ReadBlock@resourceManager@@QAEXPACK@Z": "?ReadBlock@resourceManager@@QAEXPAXK@Z",
}


def functions(path):
    normalized = canonicalize_coff(path.read_bytes())
    obj = CoffObject(normalized.data)
    symbols = obj.symbols
    result = {}
    for symbol in obj.symbols.values():
        section = obj.symbol_section(symbol)
        if not section or not section.characteristics & MEM_EXECUTE or symbol.typ != FUNCTION_TYPE:
            continue
        others = [s.value for s in obj.symbols.values() if s.section == symbol.section
                  and s.typ == FUNCTION_TYPE and s.value > symbol.value]
        end = min(others, default=section.raw_size)
        data = obj.section_bytes(section)[symbol.value:end]
        relocs = []
        for reloc in obj.relocations:
            if reloc.section != section.index or not symbol.value <= reloc.site < end:
                continue
            target = symbols[reloc.symbol_index]
            name = RENAMES.get(target.name, target.name)
            if target.name.startswith(("$L", "$anon_data_")) and target.section > 0:
                name = f"local:{target.section}:{target.value}"
            offset = reloc.site - symbol.value
            width = RELOCATION_WIDTHS[reloc.typ]
            relocs.append((offset, reloc.typ, name, data[offset:offset + width].hex()))
        key = RENAMES.get(symbol.name, symbol.name)
        if key in result:
            raise ValueError(f"Duplicate function {path}:{key}")
        result[key] = (data.hex(), relocs)
    return result


def allocated_sections(path):
    obj = CoffObject(canonicalize_coff(path.read_bytes()).data)
    result = {}
    for section in obj.sections:
        if not section.characteristics & 0xe0000000:
            continue
        data = obj.section_bytes(section)
        relocs = []
        for reloc in obj.relocations:
            if reloc.section != section.index:
                continue
            target = obj.symbols[reloc.symbol_index]
            name = RENAMES.get(target.name, target.name)
            if target.name.startswith(("$L", "$anon_data_")) and target.section > 0:
                name = f"local:{target.section}:{target.value}"
            relocs.append((reloc.site, reloc.typ, name))
        result[section.index] = (section.name, section.characteristics, data.hex(), relocs)
    return result


rows = []
section_differences = []
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--isolated-c01', action='store_true',
                    help='use preserved pre-O01/O02 objects for those two later owner changes')
args = parser.parse_args()
later_owner_changes = {
    'BASE/MilesSound.obj': Path('build/o01-before/MilesSound.obj'),
    'SOURCE/SEARCH.obj': Path('build/reconstruction-SEARCH-before.obj'),
}
for before in sorted(Path("build/reconstruction-baseline").rglob("*.obj")):
    relative = before.relative_to("build/reconstruction-baseline")
    after = Path("build/objdiff/base") / relative
    if args.isolated_c01:
        after = later_owner_changes.get(relative.as_posix(), after)
    a, b = functions(before), functions(after)
    sa, sb = allocated_sections(before), allocated_sections(after)
    for index in sorted(sa.keys() | sb.keys()):
        if sa.get(index) != sb.get(index):
            section_differences.append(dict(unit=str(relative), section=index))
    for name in sorted(a.keys() | b.keys()):
        old, new = a.get(name), b.get(name)
        rows.append(dict(unit=str(relative), function=name,
                         bytes_equal=old is not None and new is not None and old[0] == new[0],
                         relocations_equal=old is not None and new is not None and old[1] == new[1]))
Path("build/c01-before-after.json").write_text(json.dumps(rows, indent=2) + "\n")
differences = [r for r in rows if not r["bytes_equal"] or not r["relocations_equal"]]
summary = dict(functions=len(rows), differences=differences,
               allocated_section_differences=section_differences)
Path("build/c01-summary.json").write_text(json.dumps(summary, indent=2) + "\n")
print(json.dumps(summary, indent=2))
