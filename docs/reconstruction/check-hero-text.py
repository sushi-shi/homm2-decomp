"""B50: bound HERO formatter outputs using pinned Buka bytes, not translations.

Run with PYTHONPATH=scripts. Requires current HERO object and symbol manifest.
This verifies literal destinations, not whole-function code: use check-function.py
for the corresponding bytes/ordered targets. Valid table indices and a terminated
12-character hero name are explicit assumptions, not established by this script.
"""
import csv
import re
from pathlib import Path

from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control = Path('build/orig/HMM2PL.exe')
GLOBAL_TEXT_SIZE = 768
assert re.search(r'GLOBAL_TEXT_BUFFER_SIZE\s*=\s*768',
                 Path('include/SOURCE/X_GLOBAL.h').read_text())
verify_retail(control)
base, _, read_u32, read_bytes = _pe_layout(control)
obj = CoffObject(Path('build/objdiff/base/SOURCE/HERO.obj').read_bytes())
with Path('build/gen/symbol_names.csv').open() as stream:
    rows = list(csv.DictReader(stream))


def cstring(rva):
    data = read_bytes(rva, 4096)
    assert b'\0' in data
    return data.split(b'\0', 1)[0]


def table(name, va, count):
    values = [cstring(read_u32(va - base + 4 * i) - base) for i in range(count)]
    print(name, 'count', count, 'max bytes excluding NUL', max(map(len, values)),
          'formats', sorted(set(re.findall(rb'%[^ ]*', b' '.join(values)))))
    return values


def literals(rva):
    row, = [r for r in rows if r['unit'] == 'SOURCE/HERO'
            and r['kind'] == 'func' and int(r['rva'], 0) == rva]
    symbol, = [s for s in obj.symbols.values()
               if s.name == row['name'] and s.typ == FUNCTION_TYPE]
    size = int(row['size'], 0)
    section = obj.symbol_section(symbol)
    raw = obj.section_bytes(section)
    result = set()
    for reloc in obj.relocations:
        if reloc.section != symbol.section or not symbol.value <= reloc.site < symbol.value + size:
            continue
        owner = obj.symbols[reloc.symbol_index]
        if not owner.name.startswith('$SG'):
            continue
        assert reloc.typ == 6 and raw[reloc.site:reloc.site + 4] == b'\0' * 4
        value = obj.section_bytes(obj.symbol_section(owner))[owner.value:].split(b'\0', 1)[0]
        target = read_u32(rva + reloc.site - symbol.value) - base
        assert cstring(target) == value
        print('literal', hex(rva), hex(base + target), repr(value.decode('cp1251')))
        result.add(value)
    return result


def bound(fmt, args=()):
    """Output bytes including NUL. Args are (conversion, max output bytes)."""
    tokens = list(re.finditer(rb'%([0-9]*)([sd%])', fmt))
    assert b'%' not in re.sub(rb'%([0-9]*)([sd%])', b'', fmt), repr(fmt)
    assert [m[2].decode() for m in tokens if m[2] != b'%'] == [a[0] for a in args]
    output = len(fmt) + 1
    arguments = iter(args)
    for token in tokens:
        width = int(token[1] or b'0')
        size = 1 if token[2] == b'%' else next(arguments)[1]
        output += max(width, size) - len(token[0])
    return output


def choose(values, prefix):
    value, = [v for v in values if v.startswith(prefix.encode('cp1251'))]
    return value


names = table('artifact names', 0x4fd7ac, 103)
descriptions = table('artifact descriptions', 0x4fd948, 103)
stats = table('stat names', 0x4fdc80, 4)
alignments = table('alignment names', 0x4fdca0, 8)
army = table('plural army names', 0x4fded0, 66)
screen = table('hero status templates', 0x4fe524, 25)
level = table('level templates', 0x4fe764, 3)
spells = table('spell names', 0x4fec04, 65)
tiers = table('skill tiers', 0x4fed08, 3)
skills = table('skill names', 0x4fed14, 14)
skill_descriptions = table('skill descriptions', 0x4feef0, 42)
max_name, max_align, max_army = 12, max(map(len, alignments)), max(map(len, army))
max_stat, max_tier, max_skill = max(map(len, stats)), max(map(len, tiers)), max(map(len, skills))
s = lambda n: ('s', n)
d = ('d', 11)  # Any signed 32-bit value, including INT_MIN.

status_args = {
    1: [s(max_stat)], 10: [s(max_army)], 12: [s(max_army)],
    13: [s(max_army), s(max_army)], 15: [s(max(map(len, names)))],
    16: [s(max_name), s(max_align)], 19: [s(max_army)], 20: [s(max_army)],
    21: [s(max_tier), s(max_skill)],
}
status_bounds = [bound(v, status_args.get(i, ())) for i, v in enumerate(screen)]
assert max(status_bounds) <= GLOBAL_TEXT_SIZE
print('status per-entry bounds including NUL', status_bounds)

check_level = literals(0x620c8)
stat_bound = bound(choose(check_level, '\n%s +%d'), [s(max_stat), ('d', 1)])
one_choice = bound(choose(check_level, '\n\nВаш герой'), [s(max_tier), s(max_skill)])
two_choices = bound(choose(check_level, '\n\nВы также'),
                    [s(max_skill), s(max_tier), s(max_skill), s(max_tier)])
prefix_bound = bound(level[0], [s(max_name)]) + bound(level[1]) - 1
combined = prefix_bound + stat_bound - 1 + max(one_choice, two_choices) - 1
assert max(bound(level[1]), stat_bound, one_choice, two_choices) <= 200 and combined <= GLOBAL_TEXT_SIZE
print('CheckLevel prefix/stat/one/two/combined including NUL',
      prefix_bound, stat_bound, one_choice, two_choices, combined)

handler = literals(0x62ef8)
mana = bound(choose(handler, '{Очки магии}'), [s(max_name), d, d])
experience = bound(choose(handler, '{%d уровень}'), [d, d, d])
assert max(mana, experience) <= GLOBAL_TEXT_SIZE
print('HeroHandler mana/experience including NUL', mana, experience)

skill_dialog = literals(0x64a9e)
bonus = bound(choose(skill_dialog, '{%s Некромантия'), [s(max_tier), d, s(max_tier), d, d])
plain = max(bound(v) for v in skill_descriptions)
assert max(bonus, plain) <= GLOBAL_TEXT_SIZE
print('DoSSLevelDialog bonus/plain including NUL', bonus, plain)

scroll = bound(descriptions[86], [s(max(map(len, spells)))])
assert scroll <= GLOBAL_TEXT_SIZE
print('ViewArtifact spell scroll including NUL', scroll)

# Remaining HERO direct formats: bounds valid even for arbitrary i32 inputs.
setup_literals = literals(0x63b0f)
army_literals = literals(0x61520)
split_literals = literals(0x64415)
assert {b'%s - %s', b'port%04d.icn', b'%d', b'%d/%d', b'%s+%d',
        b'%s', b'crest.icn'} <= setup_literals
assert {b'monh%04d.icn', b'%d'} <= army_literals
assert {b'%d', 'Сколько воинов перенести?'.encode('cp1251')} <= split_literals
direct = {
    'title': bound(b'%s - %s', [s(max_name), s(max_align)]),
    'creature icon': bound(b'monh%04d.icn', [d]),
    'portrait icon': bound(b'port%04d.icn', [d]),
    'integer': bound(b'%d', [d]),
    'spell points': bound(b'%d/%d', [d, d]),
    'skill bonus': bound(b'%s+%d', [s(max_tier), d]),
    'skill tier': bound(b'%s', [s(max_tier)]),
    'crest': bound(b'crest.icn'),
    'split prompt': bound('Сколько воинов перенести?'.encode('cp1251')),
}
assert max(direct.values()) <= GLOBAL_TEXT_SIZE
print('direct format bounds including NUL', direct)
print('B50 formatter extents PASS, conditional on valid source table indices/name termination')
