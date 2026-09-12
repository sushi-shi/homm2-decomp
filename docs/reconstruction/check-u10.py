"""Verify U10's remaining private double-constant targets in genuine retail.

The ordinary whole-function checker covers every other ordered target/site
and byte. This supplement checks exactly its eleven unresolved references.
"""
import csv
from pathlib import Path

from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control = Path('build/orig/HMM2PL.exe')
verify_retail(control)
base, sites, read_u32, read_bytes = _pe_layout(control)
obj = CoffObject(Path('build/objdiff/base/SOURCE/PHILAI.obj').read_bytes())
with Path('build/gen/symbol_names.csv').open() as stream:
    rows = list(csv.DictReader(stream))
expected = {
    0x81c03: {0xfc, 0x113, 0x12a, 0x150, 0x176, 0x19e, 0x1bb},
    0x7fd23: {0x3b6, 0x3bc, 0x3eb, 0x3f1},
}
for rva, offsets in expected.items():
    row, = [r for r in rows if r['unit'] == 'SOURCE/PHILAI'
            and r['kind'] == 'func' and int(r['rva'], 0) == rva]
    symbol, = [s for s in obj.symbols.values()
               if s.name == row['name'] and s.typ == FUNCTION_TYPE]
    raw = obj.section_bytes(obj.symbol_section(symbol))
    seen = set()
    for reloc in obj.relocations:
        offset = reloc.site - symbol.value
        if reloc.section != symbol.section or offset not in offsets:
            continue
        owner = obj.symbols[reloc.symbol_index]
        assert reloc.typ == 6 and owner.name.startswith('__real@8@')
        assert int.from_bytes(raw[reloc.site:reloc.site + 4], 'little') == 0
        data = obj.section_bytes(obj.symbol_section(owner))[owner.value:owner.value + 8]
        target = read_u32(rva + offset) - base
        assert read_bytes(target, 8) == data
        seen.add(offset)
        print(hex(rva), hex(offset), hex(base + target), data.hex(), 'PASS')
    assert seen == offsets
print('All eleven numeric targets PASS')
