"""C65/A02/S59/B80: fixed AI numerical operands and concrete owner boundaries.

Supplements complete check-function.py/native comparisons, not their replacement.
No wildcard exemption for an unknown relocation or source contract is made.
"""
import re
import struct
from pathlib import Path
from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control=Path('build/orig/HMM2PL.exe'); verify_retail(control)
base,sites,u32,read=_pe_layout(control)
obj=CoffObject(Path('build/objdiff/base/SOURCE/AI.obj').read_bytes())
cases=[
 ('AICheckRetreat',0x15a60,2091,{
     0x3a8:(0x4ea4b0,'d',1.2),0x43c:(0x4ea4a8,'d',1.1),0x518:(0x4ea4a8,'d',1.1),
     0x5b2:(0x4ea4a0,'d',0.06),0x5d2:(0x4ea498,'d',0.05),0x5ef:(0x4ea490,'d',0.04),
     0x66b:(0x4ea488,'d',0.08),0x69b:(0x4ea4a0,'d',0.06),0x6c8:(0x4ea490,'d',0.04),
     0x6f5:(0x4ea480,'d',0.02),0x727:(0x4ea478,'d',0.015),0x771:(0x4ea470,'d',0.03),
     0x7af:(0x4ea4a0,'d',0.06),0x7c1:(0x4ea468,'d',0.16)}),
 ('DoCompAI',0x1628b,4000,{
     0x41f:(0x4ea4c0,'d',0.1),0x425:(0x4ea4b8,'d',1.0)}),
 ('GetModLichDamage',0x1722b,171,{
     0x61:(0x4ea4d0,'d',1.3),0x7d:(0x4ea4d0,'d',1.3),0x9e:(0x4ea4c8,'f',100.0)}),
]
def function(name):
    fn,=[s for s in obj.symbols.values() if s.typ==FUNCTION_TYPE and s.name.startswith('?'+name+'@combatManager@@')]
    return fn,obj.section_bytes(obj.symbol_section(fn))

for name,rva,size,expected in cases:
    fn,raw=function(name); seen=set()
    for reloc in obj.relocations:
        if reloc.section!=fn.section or not fn.value<=reloc.site<fn.value+size: continue
        owner=obj.symbols[reloc.symbol_index]
        if not owner.name.startswith('__real@'): continue
        offset=reloc.site-fn.value; destination,fmt,value=expected[offset];seen.add(offset)
        encoded=struct.pack('<'+fmt,value)
        assert reloc.typ==6 and raw[reloc.site:reloc.site+4]==bytes(4)
        assert rva+offset in sites and u32(rva+offset)==destination
        data=obj.section_bytes(obj.symbol_section(owner))[owner.value:owner.value+len(encoded)]
        assert data==encoded==read(destination-base,len(encoded)),(name,hex(offset))
        print('PASS fixed operand',name,hex(offset),hex(destination),fmt,value)
    assert seen==set(expected)

def instructions(name,rva,offset,hexbytes):
    fn,raw=function(name); encoded=bytes.fromhex(hexbytes)
    assert raw[fn.value+offset:fn.value+offset+len(encoded)]==encoded,(name,hex(offset),'native')
    assert read(rva+offset,len(encoded))==encoded,(name,hex(offset),'retail')

# The strength penalty and experience bonus perform signed integer division
# BEFORE conversion to float. Retain those semantics, not fractional ratios.
instructions('AICheckRetreat',0x15a60,0x625,
             '99 b9204e0000 f7f9 8985b0feffff db85b0feffff d8ad00ffffff d99d00ffffff')
instructions('AICheckRetreat',0x15a60,0x74c,
             '8b4139 99 b9400d0300 f7f9 8985a8feffff db85a8feffff d99dd0feffff')
instructions('AICheckRetreat',0x15a60,0x7e9,
             '8b95f8feffff 0395fcfeffff 8995a4feffff db85a4feffff def9')
print('PASS signed penalty /20000 and experience /200000 before float conversion; final force-sum divisor has no intervening zero guard')

# Castle path: an unconditional attacker-hero archery-byte load occurs after
# the inCastle guard, before the later archeryBonus null test.
instructions('DoCompAI',0x1628b,0xb50-0x82b,
             '8b8287320000 0fbe4875 85c9')
print('PASS castle AI reads attacker hero archery without a local null guard')

header=Path('include/SOURCE/combatManager.h').read_text()
source=Path('src/SOURCE/AI.cpp').read_text()
for name,value in [('COMBAT_AI_CASTLE_BOUNDARY_COUNT',9),('COMBAT_GRID_ROW_COUNT',9),
                   ('COMBAT_GRID_ROW_LENGTH',13),('COMBAT_HEX_COUNT',117),
                   ('COMBAT_AI_ARMY_SLOT_COUNT',20),('COMBAT_AI_GROUP_SLOT_COUNT',5),
                   ('COMBAT_AI_ADJACENT_DIRECTION_COUNT',6),('COMBAT_AI_ATTACK_NONE',0)]:
    assert re.search(r'\b'+name+r'\s*=\s*'+str(value)+r'\b',header),name

# Nine byte initializers and both reads from the actual stack base -0x90.
# The original12-entry claim and recovered9-entry claim have the same VC6
# rounded frame allocation; no replacement padding is introduced in source.
values=[8,0x15,0x21,0x2e,0x3a,0x48,0x55,99,0x70]
for i,value in enumerate(values):
    encoded=b'\xc6\x85'+struct.pack('<i',-0x90+i)+bytes([value])
    instructions('DoCompAI',0x1628b,0x1519-0x82b+7*i,encoded.hex())
    assert re.search(r'\bCOMBAT_AI_CASTLE_BOUNDARY_ROW_'+str(i)+r'\s*=\s*(?:'+str(value)+'|'+hex(value)+r')\s*,',header)
for offset in (0x159a-0x82b,0x15b1-0x82b):
    instructions('DoCompAI',0x1628b,offset,'8a940d70ffffff')
assert {h//13 for h in range(117)}==set(range(9))
assert all(0<=h<117 for h in values)
assert source.count('rowLimit[')==12 # declaration + nine stores + two reads
assert 'u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];' in source
print('PASS all nine boundary values and both reads; valid hex0..116 selects only rows0..8')

# A02: the marker is a real 2x20 byte owner; pointer, row and index roles
# remain visible in the emitted stack expression (not a masked relocation).
assert 'u8 marked[COMBAT_SIDE_COUNT][COMBAT_AI_ARMY_SLOT_COUNT];' in source
assert '*(marked +' not in source
assert source.count('marked[')==4 # declaration + three consumers
instructions('DoLichShot',0x172d6,0x1952-0x1876,
             '8b45dc 8b88e6000000 6bc914 8d540db4 8b45dc 8b88ea000000 c6040a01')
instructions('DoLichShot',0x172d6,0x19fb-0x1876,'6bc914 8d540db4')
instructions('DoLichShot',0x172d6,0x1a15-0x1876,'8a0c02')
instructions('DoLichShot',0x172d6,0x1a7b-0x1876,'6bd214 8d4415b4')
instructions('DoLichShot',0x172d6,0x1a93-0x1876,'c6040801')
assert {side*20+slot for side in range(2) for slot in range(20)}==set(range(40))
print('PASS all three marker consumers: side stride20 and exact stack byte read/writes inside40 bytes for valid domains')
print('NOTE valid sides/counts/IDs/path results, nonzero force sum, representable arithmetic and live owners remain caller/data contracts')
