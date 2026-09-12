"""B52: selected KB formatter bounds from pinned genuine-retail strings.

Run with PYTHONPATH=scripts and a current KB object. This verifies private
literal destinations, not complete function code; use check-function.py for
that. Valid owner lifetimes and source table indices remain caller contracts.
Bounds include NUL and intentionally overapproximate independent flags.
"""
import csv
import re
from pathlib import Path

from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control = Path('build/orig/HMM2PL.exe')
verify_retail(control)
base, _, read_u32, read_bytes = _pe_layout(control)
obj = CoffObject(Path('build/objdiff/base/SOURCE/KB.obj').read_bytes())
with Path('build/gen/symbol_names.csv').open() as stream:
    rows = list(csv.DictReader(stream))
assert re.search(r'GLOBAL_TEXT_BUFFER_SIZE\s*=\s*768',
                 Path('include/SOURCE/X_GLOBAL.h').read_text())
source = Path('src/SOURCE/KB.cpp').read_text()
for name, value in [('MORALE_LUCK_DESCRIPTION_SIZE', 200), ('CONGRATS_RATING_LENGTH', 32),
                    ('CONGRATS_TEXT_SIZE', 500), ('LINE_HELP_LINE_COUNT', 14),
                    ('MEMORY_ERROR_REQUEST_SIZE', 6400)]:
    assert re.search(r'\b' + name + r'\s*=\s*' + str(value) + r'\b', source), name


def cstring(rva):
    data = read_bytes(rva, 4096)
    assert b'\0' in data
    return data.split(b'\0', 1)[0]


def table(name, va, count):
    values = [cstring(read_u32(va - base + 4 * i) - base) for i in range(count)]
    print(name, 'lengths excluding NUL', list(map(len, values)))
    return values


def literals(rva):
    row, = [r for r in rows if r['unit'] == 'SOURCE/KB'
            and r['kind'] == 'func' and int(r['rva'], 0) == rva]
    symbol, = [s for s in obj.symbols.values()
               if s.name == row['name'] and s.typ == FUNCTION_TYPE]
    section = obj.symbol_section(symbol)
    raw = obj.section_bytes(section)
    values = set()
    for reloc in obj.relocations:
        if (reloc.section != symbol.section
                or not symbol.value <= reloc.site < symbol.value + int(row['size'], 0)):
            continue
        owner = obj.symbols[reloc.symbol_index]
        if not owner.name.startswith('$SG'):
            continue
        assert reloc.typ == 6 and raw[reloc.site:reloc.site + 4] == b'\0' * 4
        value = obj.section_bytes(obj.symbol_section(owner))[owner.value:].split(b'\0', 1)[0]
        target = read_u32(rva + reloc.site - symbol.value) - base
        assert cstring(target) == value
        print('literal', hex(rva), hex(target + base), repr(value.decode('cp1251')))
        values.add(value)
    return values


def bound(fmt, args=()):
    tokens = list(re.finditer(rb'%([0-9]*)([sd%])', fmt))
    assert b'%' not in re.sub(rb'%([0-9]*)([sd%])', b'', fmt), repr(fmt)
    assert [m[2].decode() for m in tokens if m[2] != b'%'] == [a[0] for a in args]
    result = len(fmt) + 1
    remaining = iter(args)
    for token in tokens:
        size = 1 if token[2] == b'%' else next(remaining)[1]
        result += max(int(token[1] or b'0'), size) - len(token[0])
    return result


morale = table('morale', 0x4fe9f4, 32)
luck = table('luck', 0x4fe5c8, 21)
alignment = table('alignment', 0x4fdca0, 8)
army = table('army', 0x4fddc8, 66)
help_text = table('command help', 0x4ff070, 14)

# Only the three initial descriptions and same-alignment formatter write the
# local morale scratch. The other alignment texts also replace, not append it.
assert all(b'%' not in s for s in alignment + army)
morale_scratch = max([bound(s) for s in morale[:3]] +
                     [bound(morale[5], [('s', max(map(len, alignment)))])] +
                     [bound(morale[i]) for i in (6, 7, 20)])
luck_scratch = max(bound(s) for s in luck[:3])
assert morale_scratch <= 200 and luck_scratch <= 200
morale_prefix = bound(morale[3], [('s', max(map(len, morale[:3])))])
luck_prefix = bound(luck[3], [('s', max(map(len, luck[:3])))])

# All-undead exits before the other appends. The non-undead bound permits
# every independently tested artifact/event/town flag, even combinations that
# may not be simultaneously reachable. Alignment alternatives and skill tiers
# are mutually exclusive. Including some-undead beside the positive-alignment
# upper bound deliberately overestimates, since the former suppresses the latter.
morale_independent = (8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 22, 23, 24, 25, 26, 30, 31)
morale_alignment = max(bound(morale[5], [('s', max(map(len, alignment)))]) - 1,
                       *(bound(morale[i]) - 1 for i in (6, 7, 20)))
morale_modifiers = (sum(bound(morale[i]) - 1 for i in morale_independent)
                   + morale_alignment + max(bound(morale[i]) - 1 for i in (27, 28, 29)))
morale_total = morale_prefix + max(bound(morale[21]) - 1,
                                  bound(morale[19]) - 1, morale_modifiers)
luck_independent = (4, 5, 6, 7, 8, 9, 12, 13, 14, 18, 19, 20)
luck_modifiers = (sum(bound(luck[i]) - 1 for i in luck_independent)
                 + max(bound(luck[i]) - 1 for i in (15, 16, 17)))
luck_total = luck_prefix + max(bound(luck[10]) - 1, luck_modifiers)
assert morale_total <= 768 and luck_total <= 768
print('morale scratch/prefix/complete including NUL', morale_scratch, morale_prefix, morale_total)
print('luck scratch/prefix/complete including NUL', luck_scratch, luck_prefix, luck_total)

# InterpretCommandLine resets gText to the empty string, appends exactly the
# fourteen help lines and terminates via ShutDown. No user argument is appended.
help_bound = 1 + sum(len(s) for s in help_text)
assert help_bound <= 768
print('complete command-line help including NUL', help_bound)

# ShowCongrats uses a checked 0..65 GetMonType result for the ranking string;
# all names have no format directives. Its first-byte uppercase mapping leaves
# byte length unchanged, and the fixed cheat replacement is also considered.
congrats_literals = literals(0x6d109)
cheater, = [v for v in congrats_literals if v.endswith(b'!!!')]
rating_bound = max(max(map(len, army)), len(cheater)) + 1
assert rating_bound <= 32
formats = [v for v in congrats_literals if v.startswith('Поздравляем!'.encode('cp1251'))]
assert len(formats) == 2
congrats_bounds = []
for fmt in formats:
    assert fmt.count(b'%s') == 1 and fmt.count(b'%d') in (1, 4)
    args = [('d', 11)] * fmt.count(b'%d') + [('s', rating_bound - 1)]
    congrats_bounds.append(bound(fmt, args))
assert max(congrats_bounds) <= 500
print('ranking / congrats bounds including NUL', rating_bound, sorted(congrats_bounds))

# MemError's message and memory requirement are fixed, not arbitrary input.
memory_literals = literals(0x6d5f9)
memory_message, = [v for v in memory_literals if v.startswith('Недостаточно'.encode('cp1251'))]
memory_format = cstring(read_u32(0xff2d4) - base)
memory_bound = bound(memory_format, [('s', len(memory_message)), ('d', len(str(6400)))])
assert memory_bound <= 768
print('MemError including NUL', memory_bound)
print('B52 selected formatter bounds PASS; owner/index/lifetime contracts remain')
