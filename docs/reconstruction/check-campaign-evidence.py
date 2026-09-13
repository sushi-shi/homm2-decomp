"""B85/O06: fixed retail operands, campaign text bounds and choice domains.

Run from the worktree in the build shell. This supplements, never replaces,
the seven complete function checks. It does not waive ShowCampaignInfo's EH gap.
"""
import re
import struct
from pathlib import Path
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control = Path('build/orig/HMM2PL.exe')
verify_retail(control)
base, sites, u32, read = _pe_layout(control)

def instruction(rva, offset, spelling):
    expected = bytes.fromhex(spelling)
    assert read(rva + offset, len(expected)) == expected, (hex(rva), hex(offset))

# Info: thirteen iterations, twelve-byte row stride, load before map<11 guard.
for offset, spelling in ((0x22, '83 7d fc 0d'), (0x32, '6b c9 0c'),
                         (0x3f, '8a 8c 10 a2 00 00 00'), (0x53, '83 7d fc 0b')):
    instruction(0x22adc, offset, spelling)
# Handler: scenario=4, then scenario*12 as the first index, side as second.
# Exact offsets below are derived from the delinked function start14bc.
for site, spelling in ((0x17a3, 'c6 42 04 04'), (0x17bd, '88 50 02'),
                       (0x17c6, '0f be 51 04'), (0x17ca, '6b d2 0c'),
                       (0x17d2, '8d 8c 10 a2 00 00 00'),
                       (0x17e1, '8a 42 02'), (0x17e4, 'c6 04 01 01')):
    instruction(0x233b8, site - 0x14bc, spelling)
instruction(0x23842, 7, '68 47 01 00 00')
instruction(0x23842, 0x11, '83 c0 02')
instruction(0x2387e, 0x9a, '6a 41')
instruction(0x2387e, 0xa1, '05 4d 04 00 00')

kb = Path('include/SOURCE/KB.h').read_text()
game = Path('include/SOURCE/game.h').read_text()
for name, value in (('CAMPAIGN_MAP_COUNT', '12'), ('CAMPAIGN_TRACK_POINT_COUNT', '13'),
                    ('CAMPAIGN_STATE_RESET_SIZE', '0x147'), ('CAMPAIGN_SETUP_RESET_SIZE', '0x41'),
                    ('CAMPAIGN_ARMY_NAME_BUFFER_SIZE', '52')):
    assert re.search(r'\b' + name + r'\s*=\s*' + value + r'\b', kb)
assert 'u8 m_campaignMapEnabled[IDX(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT];\n    i16 m_campaignScore;' in game
assert 'char _pad_0xd2[GAME_CAMPAIGN_STATE_PAD_SIZE];' in game
# Packed prefix: difficulty2,type1,start1,scenario1, completed24, bonus48,
# days48, unknown1, awards12, choices24, then enabled24.
enabled = sum((2, 1, 1, 1, 24, 48, 48, 1, 12, 24))
assert enabled == 0xa2
assert [enabled + side * 12 + 12 for side in range(2)] == [0xae, 0xba]
assert [enabled + 4 * 12 + side for side in range(2)] == [0xd2, 0xd3]
assert enabled + 24 + 2 + 10 + 10 + 1 + 1 == 0xd2
assert 2 + 0x147 == 0x149 and 0xd2 + 0x78 == 0x14a
assert sum((6, 6, 6, 6, 1, 13, 6, 1, 1, 1, 18)) == 0x41
print('PASS retail crossed indices: read row1[0]/score low byte; write padding d2/d3')
print('OPEN real campaign/setup subobject-spanning reset owners; 327-byte campaign prefix leaves byte149')

def cstring(rva):
    raw = read(rva, 4096)
    assert b'\0' in raw
    return raw.split(b'\0', 1)[0]

def table(va, count):
    return [cstring(u32(va - base + 4 * i) - base) for i in range(count)]

names = table(0x4ff214, 24)
descriptions = table(0x4ff274, 24)
awards = table(0x4ff1e4, 12)
army = table(0x4fded0, 66)
resource = table(0x4fdffc, 7)
artifact = table(0x4fd7ac, 103)
spell = table(0x4fec04, 65)
levels = table(0x4fed08, 3)
skills = table(0x4fed14, 14)
alignment = table(0x4fdca0, 8)
assert all(b'%' not in value for value in alignment)
army_bound = max(map(len, army)) + 1
award_bound = 1 + sum(len(value) + 1 for value in awards)
bounds = {
    'names': max(map(len, names)) + 1,
    'descriptions': max(map(len, descriptions)) + 1,
    'all awards': award_bound,
    'resource': max(map(len, resource)) + 2 + 11 + 1,
    'artifact': max(map(len, artifact)) + 1,
    'spell': max(map(len, spell)) + 1,
    'skill': max(map(len, levels)) + 1 + max(map(len, skills)) + 1,
    'creatures': 11 + 1 + army_bound,
    'puzzle': 11 + 1 + len('Обрывки карты'.encode('cp1251')) + 1,
    'experience': 11 + 1 + len('Опыт'.encode('cp1251')) + 1,
    'alignment': max(map(len, alignment)) + 1,
    'track filename': len(b'ctrack') + 11 + len(b'.icn') + 1,
}
assert army_bound <= 52 and max(bounds.values()) <= 768
assert re.search(r'GLOBAL_TEXT_BUFFER_SIZE\s*=\s*768\b', Path('include/SOURCE/X_GLOBAL.h').read_text())
for scenario in range(1, 13):
    for side in ('G', 'E'):
        assert len(f'CAMP{side}{scenario:02}.H2C') + 1 <= 13
        if scenario == 5:
            assert len(f'CAMP{side}{scenario:02}B.H2C') + 1 == 13
print('PASS table text bounds including NUL', bounds, 'army scratch', army_bound)
print('PASS valid scenario filenames12/13 bytes, copied into gMapName16')

choices = list(struct.iter_unpack('<Bhh', read(0x4fd641 - base, 72 * 5)))
assert len(choices) == 72
for kind, value, amount in choices:
    if kind == 0: assert 0 <= value < 7
    elif kind == 1: assert 0 <= value < 103
    elif kind == 2: assert 0 <= value < 65
    elif kind == 3: assert 0 <= value < 14 and 1 <= amount <= 3
    elif kind == 7: pass
    elif kind == 8: assert 0 <= value < 6
    else: raise AssertionError((kind, value, amount))
print('PASS all72 retail packed choices: resource/artifact/spell/skill/none/alignment domains')
print('OPEN mutable table integrity, live side/map/choice/hero/resource owners and widget text lifetime')
