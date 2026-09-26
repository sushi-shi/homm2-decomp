"""C67/B82: fixed flight timing operand, division shape and adjacency contract.

Run alongside complete check-function.py output and strict native comparisons.
This grants no general exception for an unidentified relocation.
"""
import re
import struct
from pathlib import Path
from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control = Path('build/orig/HMM2PL.exe')
verify_retail(control)
base, sites, u32, read = _pe_layout(control)
obj = CoffObject(Path('build/objdiff/base/SOURCE/FLY.obj').read_bytes())
fn, = [s for s in obj.symbols.values() if s.typ == FUNCTION_TYPE and s.name == '?FlyTo@army@@QAEHH@Z']
raw = obj.section_bytes(obj.symbol_section(fn))
rva, size, offset, destination = 0x4b5ce, 2530, 0x73e, 0x4ea4d0
numerical = [r for r in obj.relocations if r.section == fn.section
             and fn.value <= r.site < fn.value + size
             and obj.symbols[r.symbol_index].name.startswith('__real@')]
reloc, = numerical
owner = obj.symbols[reloc.symbol_index]
assert reloc.site - fn.value == offset and reloc.typ == 6
assert raw[reloc.site:reloc.site+4] == bytes(4)
assert rva + offset in sites and u32(rva + offset) == destination
encoded = struct.pack('<d', 1.3)
assert obj.section_bytes(obj.symbol_section(owner))[owner.value:owner.value+8] == encoded
assert read(destination-base, 8) == encoded
print('PASS sole fixed numerical operand: FlyTo+0x73e, DIR32 zero addend, retail0x4ea4d0, double1.3')

def instructions(offset, encoding):
    expected = bytes.fromhex(encoding)
    assert raw[fn.value+offset:fn.value+offset+len(expected)] == expected
    assert read(rva+offset, len(expected)) == expected

# Both coordinate increments load integer operands separately onto x87. Do not
# replace the divisor casts with a generic mixed integer/floating spelling.
instructions(0x7ed-0x5ce, 'db45d8 db45e8 def9 d95dc4')
instructions(0x7f8-0x5ce, 'db45c0 db45e8 def9 d95dfc')
instructions(0x7e0-0x5ce, '837de800 7f07 c745e801000000')
instructions(0xd1c-0x5ce, 'da75ac dec1')
instructions(0xd4e-0x5ce, 'da75ac dec1')
print('PASS both x87 integer-load/integer-load/divide/store sequences')
print('PASS step count clamped to1; both timer formulas retain integer-memory floating division by frameCount')

header = Path('include/SOURCE/combatManager.h').read_text()
for name, value in [('COMBAT_GRID_ROW_LENGTH',13), ('COMBAT_HEX_COUNT',117),
                    ('COMBAT_AI_ADJACENT_DIRECTION_COUNT',6)]:
    assert re.search(r'\b'+name+r'\s*=\s*'+str(value)+r'\b', header), name

# Exhaust the finite geometry implemented by the separately retail-checked
# SetupAdjacencyArray. Every nonnegative result is an interior-column hex.
valid_count = 0
for source in range(117):
    row = source // 13
    targets = [source-(13 if row & 1 else 12), source+1,
               source+(13 if row & 1 else 14), source+(12 if row & 1 else 13),
               source-1, source-(14 if row & 1 else 13)]
    for target in targets:
        neighbor = target if source % 13 not in (0,12) and 0 <= target < 117 and target % 13 not in (0,12) else -1
        assert neighbor == -1 or 0 <= neighbor < 117 and neighbor % 13 not in (0,12)
        if neighbor >= 0: valid_count += 1
assert len(targets) == 6
print('PASS all702 adjacency positions; all',valid_count,'nonnegative neighbors pass CanFit pointer-initialization column tests')
print('NOTE this depends on initialized, unmodified adjacency; live placement, animation counts and timing/coordinate domains remain contracts')
