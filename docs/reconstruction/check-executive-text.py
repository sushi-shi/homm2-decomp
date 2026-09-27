"""C03 text owners: exact native graph with18 explicit interior-to-owner rewrites."""
import importlib.util
import csv
from pathlib import Path
from homm2.core.coff import CoffObject
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

spec = importlib.util.spec_from_file_location('comparison', 'docs/reconstruction/compare-objects.py')
comparison = importlib.util.module_from_spec(spec)
spec.loader.exec_module(comparison)
OWNERS = [{'name': 'gExecutiveResourceInitError', 'offset': 0, 'extent': 58, 'slot': 60}, {'name': 'gExecutiveInputInitError', 'offset': 60, 'extent': 87, 'slot': 88}, {'name': 'gExecutiveSoundInitError', 'offset': 148, 'extent': 31, 'slot': 32}, {'name': 'gExecutiveMouseInitError', 'offset': 180, 'extent': 31, 'slot': 32}, {'name': 'gExecutiveWindowInitError', 'offset': 212, 'extent': 66, 'slot': 68}, {'name': 'gExecutiveDialogManagerError1', 'offset': 280, 'extent': 28, 'slot': 28}, {'name': 'gExecutiveDialogManagerError2', 'offset': 308, 'extent': 28, 'slot': 28}, {'name': 'gExecutiveDialogManagerError3', 'offset': 336, 'extent': 28, 'slot': 28}, {'name': 'gExecutiveDialogManagerError4', 'offset': 364, 'extent': 28, 'slot': 28}, {'name': 'gExecutiveManagerListStart', 'offset': 392, 'extent': 29, 'slot': 32}, {'name': 'gExecutiveManagerListDivider1', 'offset': 424, 'extent': 6, 'slot': 8}, {'name': 'gExecutiveManagerListHeaderFormat', 'offset': 432, 'extent': 18, 'slot': 20}, {'name': 'gExecutiveManagerListDivider2', 'offset': 452, 'extent': 6, 'slot': 8}, {'name': 'gExecutiveManagerListEntryFormat', 'offset': 460, 'extent': 41, 'slot': 44}, {'name': 'gExecutiveManagerListStop', 'offset': 504, 'extent': 31, 'slot': 32}, {'name': 'gExecutiveCallManagerError1', 'offset': 536, 'extent': 28, 'slot': 28}, {'name': 'gExecutiveCallManagerError2', 'offset': 564, 'extent': 28, 'slot': 28}, {'name': 'gExecutiveTerminationMessage', 'offset': 592, 'extent': 11, 'slot': 12}]

def normalized_after(before_path, after_path):
    old = CoffObject(before_path.read_bytes())
    obj = CoffObject(after_path.read_bytes())
    anchor, = [s for s in old.symbols.values() if s.name == '_gExecutiveTextStorage' and s.section > 0]
    assert not any(s.name == '_gExecutiveTextStorage' for s in obj.symbols.values())
    owner_map = {}
    for row in OWNERS:
        symbol, = [s for s in obj.symbols.values() if s.name == '_'+row['name'] and s.section > 0]
        assert (symbol.section, symbol.value, symbol.storage_class) == (anchor.section, anchor.value+row['offset'], anchor.storage_class)
        data = obj.section_bytes(obj.symbol_section(symbol))[symbol.value:symbol.value+row['extent']]
        original = old.section_bytes(old.symbol_section(anchor))[anchor.value+row['offset']:anchor.value+row['offset']+row['extent']]
        assert data == original and data[-1:] == b'\0' and b'\0' not in data[:-1]
        owner_map[symbol.name] = row['offset']
    functions, sections = comparison.census(after_path)
    def rewritten(data, relocs):
        data = bytearray(data)
        targets = []
        for site, kind, target in relocs:
            if target in owner_map:
                assert kind == 6 and data[site:site+4] == bytes(4)
                data[site:site+4] = owner_map[target].to_bytes(4, 'little')
                target = '_gExecutiveTextStorage'
            targets.append((site, kind, target))
        return bytes(data), targets
    for name, value in functions.items(): functions[name] = rewritten(*value)
    for index, value in sections.items(): sections[index] = value[:2] + rewritten(*value[2:])
    return functions, sections

def main():
    before = Path('build/c03-owner-before')
    after = Path('build/objdiff/base')
    paths = sorted(p.relative_to(before) for p in before.rglob('*.obj'))
    assert len(paths) == 98 and set(paths) == {p.relative_to(after) for p in after.rglob('*.obj')}
    count = 0
    for path in paths:
        a = comparison.census(before/path)
        b = normalized_after(before/path, after/path) if str(path) == 'BASE/EXEC.obj' else comparison.census(after/path)
        assert a == b, path
        count += len(a[0])
        print('PASS', path, len(a[0]), 'functions, allocated bytes/attributes and ordered relocations')
    print('PASS all98 native objects;', count, 'emitted functions; exactly18 declared text-owner rewrites')

def retail_data():
    control = Path('build/orig/HMM2PL.exe')
    verify_retail(control)
    _, _, _, read_bytes = _pe_layout(control)
    rows = list(csv.DictReader(Path('build/gen/symbol_names.csv').open()))
    obj = CoffObject(Path('build/objdiff/base/BASE/EXEC.obj').read_bytes())
    for owner in OWNERS:
        name = '_'+owner['name']
        row, = [r for r in rows if r['name'] == name and r['unit'] == 'BASE/EXEC']
        assert int(row['rva'], 0) == 0x11f2f4+owner['offset']
        assert int(row['size'], 0) == owner['slot']
        symbol, = [s for s in obj.symbols.values() if s.name == name and s.section > 0]
        raw = obj.section_bytes(obj.symbol_section(symbol))[symbol.value:symbol.value+owner['slot']]
        assert raw == read_bytes(int(row['rva'], 0), owner['slot'])
        assert raw[owner['extent']:] == bytes(owner['slot']-owner['extent'])
        print('PASS retail text owner', name, hex(int(row['rva'], 0)+0x400000), owner['slot'], 'bytes')
    print('PASS18 text owners; all604 retail bytes; no overlapping annotation or synthetic padding owner')

if __name__ == '__main__':
    import sys
    retail_data() if sys.argv[1:] == ['--retail-data'] else main()
