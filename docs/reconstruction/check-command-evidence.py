"""U17/C68/B83: fixed numeric operands and selected retail text capacities.

Companion to full check-function.py and all98 native-object comparisons.
Does not waive the existing seven private EH-owner/site-model gaps.
"""
import csv
import re
import struct
from pathlib import Path
from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control = Path('build/orig/HMM2PL.exe')
verify_retail(control)
base, sites, u32, read = _pe_layout(control)
obj = CoffObject(Path('build/objdiff/base/SOURCE/COMMAND.obj').read_bytes())
with Path('build/gen/symbol_names.csv').open() as stream:
    rows = [r for r in csv.DictReader(stream)
            if r['unit'] == 'SOURCE/COMMAND' and r['kind'] == 'func'
            and r['provenance'] == 'source-annotation']
assert len(rows) == 36
expected = {
    0x2b850: [(0x3f, 0x4ea54c, 'f', 75)],
    0x2c389: [(0x127, 0x4ea590, 'd', 3.73), (0x142, 0x4ea588, 'd', 1.73),
              (0x15d, 0x4ea518, 'f', 1), (0x178, 0x4ea580, 'd', .58),
              (0x193, 0x4ea578, 'd', .27), (0x1b1, 0x4ea578, 'd', .27),
              (0x1cc, 0x4ea580, 'd', .58), (0x1e7, 0x4ea518, 'f', 1),
              (0x202, 0x4ea588, 'd', 1.73), (0x21d, 0x4ea590, 'd', 3.73)],
    0x2f601: [(0x2ca, 0x4ea4c0, 'd', .1)],
    0x3049b: [(0x12a, 0x4ea4c0, 'd', .1), (0x142, 0x4ea500, 'd', .5),
              (0x175, 0x4ea598, 'd', .2), (0x17b, 0x4ea4b8, 'd', 1)],
    0x315fa: [(0x668, 0x4ea4c8, 'f', 100), (0x7c7, 0x4ea500, 'd', .5),
              (0x7d6, 0x4ea5a0, 'd', .25), (0x910, 0x4ea4f8, 'f', 150)],
}
literal_values = {}
count = 0

def cstring(rva):
    data = read(rva, 4096)
    assert b'\0' in data
    return data.split(b'\0', 1)[0]

for row in rows:
    rva, size = int(row['rva'], 0), int(row['size'], 0)
    fn, = [s for s in obj.symbols.values()
           if s.typ == FUNCTION_TYPE and s.name == row['name']]
    raw = obj.section_bytes(obj.symbol_section(fn))
    numeric = []
    literals = set()
    for reloc in obj.relocations:
        if reloc.section != fn.section or not fn.value <= reloc.site < fn.value + size:
            continue
        owner = obj.symbols[reloc.symbol_index]
        if owner.name.startswith('__real@'):
            numeric.append(reloc)
        if owner.name.startswith('$SG'):
            assert reloc.typ == 6 and raw[reloc.site:reloc.site+4] == bytes(4)
            value = obj.section_bytes(obj.symbol_section(owner))[owner.value:].split(b'\0', 1)[0]
            assert cstring(u32(rva+reloc.site-fn.value)-base) == value
            literals.add(value)
    assert len(numeric) == len(expected.get(rva, [])), hex(rva)
    for reloc, (offset, destination, kind, value) in zip(numeric, expected.get(rva, [])):
        assert reloc.site-fn.value == offset and reloc.typ == 6
        assert raw[reloc.site:reloc.site+4] == bytes(4)
        assert rva+offset in sites and u32(rva+offset) == destination
        owner = obj.symbols[reloc.symbol_index]
        encoded = struct.pack('<'+kind, value)
        assert obj.section_bytes(obj.symbol_section(owner))[owner.value:owner.value+len(encoded)] == encoded
        assert read(destination-base, len(encoded)) == encoded
        count += 1
    literal_values[rva] = literals
assert count == 20
print('PASS all20 fixed numerical operands: exact sites/types/widths/values, zero addends and pinned retail destinations')
print('PASS every private C-string operand in all36 functions against retail destination bytes')

def table(va, count):
    return [cstring(u32(va-base+4*i)-base) for i in range(count)]

def bound(fmt, args=()):
    tokens = list(re.finditer(rb'%([0-9]*)([sd%])', fmt))
    assert b'%' not in re.sub(rb'%([0-9]*)([sd%])', b'', fmt), fmt
    assert [m[2].decode() for m in tokens if m[2] != b'%'] == [a[0] for a in args]
    result = len(fmt)+1
    remaining = iter(args)
    for token in tokens:
        length = 1 if token[2] == b'%' else next(remaining)[1]
        result += max(int(token[1] or b'0'), length)-len(token[0])
    return result

header = Path('include/SOURCE/KB_TYPES.h').read_text()
assert re.search(r'ARTIFACT_COUNT\s*=\s*103\b', header)
assert re.search(r'SPELL_COUNT\s*=\s*65\b', header)
source = Path('src/SOURCE/COMMAND.cpp').read_text()
for name, value in [('ARTIFACT_NAME_CAPACITY',60), ('EAGLE_SPELL_NAME_CAPACITY',200),
                    ('VICTORY_EXPERIENCE_TEXT_SIZE',152), ('VIEW_DESCRIPTION_SIZE',300),
                    ('SKELETON_COUNT_CAPACITY',9), ('CASUALTY_QUANTITY_CAPACITY',9)]:
    assert re.search(r'\b'+name+r'\s*=\s*'+str(value)+r'\b', source), name
assert re.search(r'GLOBAL_TEXT_BUFFER_SIZE\s*=\s*768\b', Path('include/SOURCE/X_GLOBAL.h').read_text())
artifacts, spells, battle = table(0x4fd7ac, 103), table(0x4fec04, 65), table(0x4fe9c8, 11)
assert all(b'%' not in s for s in artifacts+spells)
artifact_bound, spell_bound = max(map(len, artifacts))+1, max(map(len, spells))+1
assert artifact_bound <= 60 and spell_bound <= 200
print('PASS artifact60/spell200 name buffers including NUL:', artifact_bound, spell_bound)

# Hero names require a terminator within their real13-byte field (max12).
experience = max(bound(battle[3], [('s',12),('d',11)]),
                 bound(battle[10], [('s',12),('d',11),('d',11)]))
victory = max(bound(s) for s in battle[:3])+experience-1
loss = max([bound(s,[('s',12)]) for s in battle[4:7]] + [bound(s) for s in battle[7:10]])
assert experience <= 152 and max(victory, loss) <= 768
print('PASS experience152 / victory768 / loss768 including NUL:', experience, victory, loss)
eagle, = [v for v in literal_values[0x2e93a] if v.count(b'%s') == 2]
surrender, = [v for v in literal_values[0x3049b] if b'%s' in v]
assert bound(eagle,[('s',12),('s',spell_bound-1)]) <= 768
assert bound(surrender,[('s',12),('d',11)]) <= 768
print('PASS Eagle Eye / surrender global text:', bound(eagle,[('s',12),('s',spell_bound-1)]), bound(surrender,[('s',12),('d',11)]))

ballista = literal_values[0x32475]
formats = [s for s in ballista if b'%s' in s]
assert len(formats) == 3
labels = [s for s in ballista if s and b'%' not in s]
assert len(labels) == 3
maximum_label = max(map(len,labels))
description = max(bound(s,[('s',maximum_label)]+[('d',11)]*s.count(b'%d')) for s in formats)
total = maximum_label+1+3*(description-1)
assert description <= 300 and total <= 768
print('PASS ballista description300 / global768 conservative bounds:', description, total)
print('OPEN skeleton/casualty9-byte decimal buffers: need at most8 characters, not arbitrary i32; packet sender/text/payload domains remain unvalidated')
