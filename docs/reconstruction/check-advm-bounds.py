"""Supplement the ADVMGR buffer/edge review with scoped genuine-retail proof.

Run after check-function.py for UpdateRadar, HeroQuickView and ComboDraw.
This does not resolve HeroQuickView's unrelated EH placement aliases.
"""
import csv
from pathlib import Path

from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control = Path('build/orig/HMM2PL.exe')
verify_retail(control)
base, sites, read_u32, read_bytes = _pe_layout(control)
obj = CoffObject(Path('build/objdiff/base/SOURCE/ADVMGR.obj').read_bytes())
with Path('build/gen/symbol_names.csv').open() as stream:
    rows = list(csv.DictReader(stream))
owners = {r['name']: int(r['rva'], 0) for r in rows}


def function(rva):
    row, = [r for r in rows if r['unit'] == 'SOURCE/ADVMGR'
            and r['kind'] == 'func' and int(r['rva'], 0) == rva]
    symbol, = [s for s in obj.symbols.values()
               if s.name == row['name'] and s.typ == FUNCTION_TYPE]
    return symbol, obj.symbol_section(symbol)


def span(rva, start, end):
    symbol, section = function(rva)
    raw = bytearray(obj.section_bytes(section)[symbol.value + start:symbol.value + end])
    retail = bytearray(read_bytes(rva + start, end - start))
    for reloc in obj.relocations:
        offset = reloc.site - symbol.value
        if reloc.section != symbol.section or not start <= offset < end:
            continue
        owner = obj.symbols[reloc.symbol_index]
        i = offset - start
        addend = int.from_bytes(raw[i:i + 4], 'little')
        actual = read_u32(rva + offset)
        if reloc.typ == 20:
            actual = (actual + rva + offset + 4) & 0xffffffff
        else:
            assert reloc.typ == 6
            actual -= base
        if owner.name.startswith('$SG'):
            data = obj.section_bytes(obj.symbol_section(owner))[owner.value:]
            value = data[:data.index(b'\0') + 1]
            assert addend == 0 and read_bytes(actual, len(value)) == value
        else:
            assert owners[owner.name] + addend == actual, (owner.name, actual)
        raw[i:i + 4] = retail[i:i + 4] = b'\0' * 4
        print(hex(rva + offset), owner.name, 'target', hex(base + actual), 'PASS')
    assert raw == retail
    print(hex(rva), 'span', hex(start), hex(end), 'bytes and all targets PASS')


# Allocation(5), signed short load, %d, destination pointer, sprintf and cleanup.
span(0xc322, 0x592, 0x5d3)
# Both left-neighbor guards and the unchecked right-neighbor load.
span(0x84ac, 0x45d, 0x4fc)

# These six four-byte constant references are the only unresolved targets in
# the whole UpdateRadar check. Confirm native numeric data at each retail site.
symbol, section = function(0x84ac)
float_sites = {0xb13, 0xb28, 0xb6c, 0xb84, 0xbf0, 0xc02}
seen = set()
for reloc in obj.relocations:
    offset = reloc.site - symbol.value
    if reloc.section != symbol.section or offset not in float_sites:
        continue
    owner = obj.symbols[reloc.symbol_index]
    assert reloc.typ == 6 and owner.name.startswith('__real@4@')
    addend = int.from_bytes(obj.section_bytes(section)[reloc.site:reloc.site + 4], 'little')
    assert addend == 0
    data = obj.section_bytes(obj.symbol_section(owner))[owner.value:owner.value + 4]
    target = read_u32(0x84ac + offset) - base
    assert read_bytes(target, 4) == data
    seen.add(offset)
    print('radar float', hex(offset), hex(base + target), data.hex(), 'PASS')
assert seen == float_sites

# Nine rows, three grammatical variants. Inspect genuine pointer destinations,
# not Unicode comment lengths in the reconstructed source.
lengths = []
for i in range(27):
    ptr = read_u32(0xff148 + 4 * i) - base
    data = read_bytes(ptr, 64)
    lengths.append(data.index(b'\0'))
assert max(lengths) == 9 and max(lengths[::3]) == 7
assert len(str(-(2 ** 31))) + 1 == 12
print('army-name table lengths', lengths, 'title max7; all variants max9')
print('debug i32 text fits12; title copies fit15; exact hero count32767 needs6, not5')

plural_lengths = []
for i in range(66):
    ptr = read_u32(0xfded0 + 4 * i) - base
    plural_lengths.append(read_bytes(ptr, 256).index(b'\0'))
message_bounds = []
for ptr in (0xf03dc, 0xf0404, 0xf044c, 0xf0490, 0xf04c8):
    data = read_bytes(ptr, 200).split(b'\0', 1)[0]
    assert b'%' not in data.replace(b'%d', b'')
    message_bounds.append(len(data) + 9 * data.count(b'%d') + 1)
prefix = read_bytes(0xf03d0, 10)
assert prefix == b'{%d %s}\n\n\0'
prefix_bound = len(prefix) + 9 + max(plural_lengths) - 2
combined_bound = prefix_bound + max(message_bounds) - 1
assert max(message_bounds) <= 200 and combined_bound <= 500
print('Visions: plural max', max(plural_lengths), 'msg bounds', message_bounds,
      'combined including NUL', combined_bound, 'PASS')
