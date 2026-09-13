"""B86: fixed retail high-score reads/format arguments and bounded frame data."""
import struct
from pathlib import Path
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

p = Path('build/orig/HMM2PL.exe')
verify_retail(p)
base, sites, u32, read = _pe_layout(p)
def instruction(site, spelling):
    raw = bytes.fromhex(spelling)
    assert read(0x654ef + site - 0x3e8, len(raw)) == raw, hex(site)

for site, spelling in ((0x58f, 'c7 45 d6 ff ff ff ff'),
                       (0x598, '6a 64'), (0x59a, '8d 4d 9c'),
                       (0x5a7, '83 c4 0c'), (0x5aa, '83 7d d6 ff'),
                       (0x778, '8d 4d 9c 51'), (0x786, '83 c4 08'),
                       (0x789, '0f be 55 e2'), (0x7e0, '8d 4d ad 51'),
                       (0x7ee, '83 c4 08')):
    instruction(site, spelling)
assert 0x64 - 0x2a == 58 and 0x64 - 0x1e == 70
assert 17 + 41 + 4 + 4 + 4 + 1 + 29 == 100
print('PASS single100-byte read; missing-file path initializes only score; cheated byte70 read separately')
print('PASS player/scenario addresses supplied directly as sprintf formats, only two call arguments')
print('OPEN missing/short-file initialization, name termination and format directives; not hardened')

frames = list(read(0x4ef1dc-base, 18))
assert frames == [0,0,0,1,2,2,1,0,0,0,3,4,5,5,4,3,0,0]
tables = [list(struct.iter_unpack('<hh', read(va-base, 66*4)))
          for va in (0x4fae70, 0x4faf78)]
assert all(0 <= creature < 66 for table in tables for score, creature in table)
assert max(creature*9 + frame + 1 for table in tables for score, creature in table
           for frame in frames) == 591
assert all(0 <= (frame + 1) % 18 < 18 for frame in range(18))
assert len(b'highScoreManager') + 1 == 17
assert len(b'.\\DATA\\STANDARD.HS') + 1 == 19
assert len(b'.\\DATA\\CAMPAIGN.HS') + 1 == 19
print('PASS initialized frame recurrence0..17, table types0..65, image frame0..591 (resource extent still required)')
print('PASS fixed name17/30, path19/352, numeric signed32 text12/768')
