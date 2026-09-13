"""B66: town text capacities from hash-pinned retail CP1251 bytes.

Run from the worktree after building. These are conditional text bounds for
valid table indices and terminated runtime strings, not a UI/resource-safety
claim. Building descriptions include every prerequisite append.
"""
import re
from pathlib import Path

from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

retail = Path('build/orig/HMM2PL.exe')
verify_retail(retail)
base, _, u32, read = _pe_layout(retail)
source = Path('src/SOURCE/TOWNMGR.cpp').read_text()
for name, value in [('BUILDING_DESCRIPTION_CAPACITY', 400),
                    ('WELL_DETAIL_TEXT_CAPACITY', 40),
                    ('THIEVES_STAT_TEXT_CAPACITY', 200),
                    ('BUILD_AMOUNT_TEXT_CAPACITY', 10)]:
    assert re.search(r'\b' + name + r'\s*=\s*' + str(value) + r'\b', source), name
assert 'TOWN_MANAGER_STATUS_TEXT_SIZE = 0x50' in Path('include/SOURCE/townManager.h').read_text()
assert re.search(r'GLOBAL_TEXT_BUFFER_SIZE\s*=\s*768', Path('include/SOURCE/X_GLOBAL.h').read_text())
assert re.search(r'TOWN_OBJECT_FILENAME_SIZE\s*=\s*16', Path('include/SOURCE/TOWNMGR.h').read_text())

def string(va):
    data = read(va - base, 4096)
    assert b'\0' in data, hex(va)
    return data.split(b'\0', 1)[0]

def table(label, va, count):
    values = [string(u32(va - base + 4*i)) for i in range(count)]
    print(label, 'lengths without NUL', list(map(len, values)))
    return values

def formatted(fmt, *widths):
    tokens = list(re.finditer(rb'%[sd]', fmt))
    assert len(tokens) == len(widths), (fmt, widths)
    assert b'%' not in re.sub(rb'%[sd]', b'', fmt), fmt
    return len(fmt) + 1 + sum(width - 2 for width in widths)

def literals(unit, prefix, rva, size):
    obj = CoffObject(Path('build/objdiff/base/' + unit + '.obj').read_bytes())
    fn, = [s for s in obj.symbols.values() if s.typ == FUNCTION_TYPE and s.name.startswith(prefix)]
    raw = obj.section_bytes(obj.symbol_section(fn))
    result = set()
    for reloc in obj.relocations:
        if reloc.section != fn.section or not fn.value <= reloc.site < fn.value + size:
            continue
        owner = obj.symbols[reloc.symbol_index]
        if not owner.name.startswith('$SG'):
            continue
        assert reloc.typ == 6 and raw[reloc.site:reloc.site+4] == bytes(4)
        value = obj.section_bytes(obj.symbol_section(owner))[owner.value:].split(b'\0', 1)[0]
        assert string(u32(rva + reloc.site - fn.value)) == value
        result.add(value)
    return result

def cp(text):
    return text.encode('cp1251')

names = table('creature names', 0x4fddc8, 66)
plurals = table('creature plurals', 0x4fded0, 66)
commands = table('town commands', 0x4fe798, 28)
special = table('special names', 0x4fedb4, 7)
wells = table('extra-well names', 0x4fed98, 7)
shrine = string(u32(0x51b7b8 - base))
prefixes = table('town icon prefixes', 0x4fbee0, 6)
objects = table('town icon object names', 0x4fbef8, 32)
short = table('faction short names', 0x4fcd7c, 6)
details = table('well detail text', 0x4fe6a0, 9)
speeds = table('speed text', 0x4fe654, 10)
neutral = table('neutral building names', 0x4fed4c, 19)
dwellings = table('dwelling names', 0x4fedd0, 6*12)
neutral_info = table('neutral building info', 0x4fef98, 19)
special_info = table('special building info', 0x4fefe4, 6)
shrine_info = string(u32(0x51b7bc - base))
for value in names + plurals + special + wells + neutral + dwellings + neutral_info + special_info + [shrine, shrine_info]:
    assert b'%' not in value, value

# Every status writer: army actions, fixed commands, the date, faction labels,
# selected creature and dwelling recruitment. Dates are three u16 values.
status_bounds = {str(i): formatted(commands[i], *([max(map(len, plurals))] * commands[i].count(b'%s')))
                 for i in range(8)}
for i in (8, 9, 11, 13, 14, 15, 16, 17, 18, 19, 20, 22, 23, 24, 25, 26, 27):
    assert b'%' not in commands[i]
    status_bounds[str(i)] = len(commands[i]) + 1
status_bounds['select'] = formatted(commands[12], max(map(len, names)))
status_bounds['recruit'] = formatted(commands[21], max(map(len, plurals)))
status_bounds['faction'] = max(map(len, special[:6] + wells[:6] + [shrine])) + 1
date_fmt = b'%s: %d, %s: %d, %s: %d'
assert date_fmt in literals('SOURCE/TOWNMGR', '?SetCommandAndText@', 0xa61d0, 0x5fe)
status_bounds['date'] = formatted(date_fmt, len(cp('Месяц')), 5, len(cp('Неделя')), 5, len(cp('День')), 5)
assert max(status_bounds.values()) <= 80, status_bounds
print('PASS status80 including NUL:', status_bounds)

# The constructor's only callers form prefix+object into gText; the final
# filename appends .icn in a distinct16-byte local.
icon_name = max(map(len, prefixes)) + max(map(len, objects)) + len(b'.icn') + 1
castle_icon = len(b'cstl') + max(map(len, short)) + len(b'.icn') + 1
assert icon_name <= 16 and castle_icon <= 16
assert b'%s.icn' in literals('SOURCE/TOWNMGR', '??0townObject@@', 0xa4740, 0x18d)
print('PASS town-object/castle icon filenames', icon_name, castle_icon, '/16 including NUL')

# All optional well-description appends are included; signed i8 statistics
# require at most4 chars, u16 hit points5, i8 growth+2+8 at most4.
well_literals = literals('SOURCE/TOWNMGR', '?SetupWell@', 0xaacfd, 0x647)
assert {b'%s%d', b'\n%s%d', b'-%d', b'cstl%s.icn', b'%d'} <= well_literals
parts = [formatted(b'%s%d', len(details[0]), 4),
         formatted(b'\n%s%d', len(details[1]), 4),
         formatted(b'\n%s%d', len(details[3]), 4),
         formatted(b'-%d', 4),
         formatted(b'\n%s%d', len(details[4]), 5),
         formatted(details[7], max(map(len, speeds))),
         formatted(details[8], 4)]
well_total = 1 + sum(part - 1 for part in parts)
assert max(parts) <= 40 and well_total <= 768
assert formatted(b'%d', 6) <= 10  # signed16 resource costs, regardless of normal gameplay
assert formatted(b'%d\n', 11) <= 200
assert 1 + 4*(formatted(b'%d\n', 11)-1) <= 768
print('PASS well fragments', max(parts), '/40; complete chain', well_total, '/768 including NUL')
print('PASS amount7/10; primary stat line13/200; four-line stat chain49/768 including NUL')

# Model GetBuildingName/GetBuildingInfo, then the complete BuyBuild append
# chain, for every valid faction/dwelling entry. Unsupported creatures(-1)
# remain invalid-domain obligations, not invented table entries.
creatures = read(0x4fbf78-base, 72)
well_fmt = cp('%s увеличивает прирост %s на 8 в неделю.')
dwelling_fmt = cp("В постройке '%s' можно купить %s.")
info_literals = literals('SOURCE/KB', '?GetBuildingInfo@@', 0x68a09, 0x179)
assert {well_fmt, dwelling_fmt, b'{%s}\n\n%s'} <= info_literals
prereq_heading = cp('\n\nНеобходимо:')
mage_prereq = cp('\nГильдия магов, 2 этаж')
buy_literals = literals('SOURCE/TOWNMGR', '?BuyBuild@', 0xa89fe, 0xe6c)
assert {prereq_heading, mage_prereq, b'\n', b'\n '} <= buy_literals

def building_name(race, building):
    assert 0 <= race < 6 and 0 <= building <= 30
    if race == 5 and building == 2:
        return shrine
    if building == 11:
        return wells[race]
    if building == 13:
        return special[race]
    if building < 19:
        return neutral[building]
    return dwellings[12*race+building-19]

def info(race, building, titled):
    name = building_name(race, building)
    if race == 5 and building == 2:
        body = shrine_info
    elif building == 11:
        creature = creatures[12*race]
        assert creature < 66
        body = well_fmt % (name, plurals[creature])
    elif building == 13:
        body = special_info[race]
    elif building < 19:
        body = neutral_info[building]
    else:
        creature = creatures[12*race+building-19]
        assert creature < 66
        return dwelling_fmt % (name, plurals[creature])
    assert len(body)+1 <= 400
    return b'{%s}\n\n%s' % (name, body) if titled else body

max_info = (0, None)
max_buy = (0, None)
count = 0
for race in range(6):
    for building in range(31):
        if building >= 19 and creatures[12*race+building-19] >= 66:
            continue
        count += 1
        titled = info(race, building, True)
        assert b'%' not in titled
        max_info = max(max_info, (len(titled)+1, (race, building)))
        body = info(race, building, False)
        if building >= 19:
            mask = u32(0x4fcc4c-base+4*(12*race+building-19))
            prerequisites = [bit for bit in range(32) if mask & (1 << bit)]
            assert all(bit <= 30 for bit in prerequisites)
            if prerequisites:
                body += prereq_heading
            for bit in prerequisites:
                body += b'\n' + building_name(race, bit)
            if race == 5 and building == 28:
                body += mage_prereq  # include the optional low-mage-level append
        body += b'\n '
        assert b'%' not in body
        max_buy = max(max_buy, (len(body)+1, (race, building)))
assert max_info[0] <= 400 and max_buy[0] <= 400, (max_info, max_buy)
print('PASS', count, 'valid building/faction entries: titled description', max_info,
      '/400; complete prerequisite description', max_buy, '/400 including NUL')
print('Conditional bounds only: runtime town names used as formats, invalid table IDs,')
print('object lifetimes, widget/resource counts and non-text game arithmetic remain separate.')
