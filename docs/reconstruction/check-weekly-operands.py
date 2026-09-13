"""Close PerWeek's three fixed private double operands without general normalization."""
import ast
import re
import struct
import subprocess
import sys
from pathlib import Path
from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control=Path('build/orig/HMM2PL.exe')
verify_retail(control)
base,sites,u32,read=_pe_layout(control)
source=Path('src/SOURCE/GAME.cpp').read_text()
for macro,value in [('HARD','1.20'),('EXPERT','1.32'),('IMPOSSIBLE','1.44')]:
    assert '#define WEEKLY_'+macro+'_GROWTH_FACTOR '+value in source
expected={0x195:(0x4ea4b0,1.20),0x1b7:(0x4ea5e8,1.32),0x1da:(0x4ea5e0,1.44)}
result=subprocess.run([sys.executable,'docs/reconstruction/check-function.py','SOURCE/GAME','0x57ed6'],text=True,capture_output=True)
print(result.stdout,end='')
assert result.returncode==1 and not result.stderr
summary=result.stdout.splitlines()[-1]
assert '?PerWeek@game@@QAEXXZ bytes True size 6824 6824 DIR32 sites True ordered target errors ' in summary
errors=ast.literal_eval(summary.split('ordered target errors ',1)[1])
assert len(errors)==3
assert {int(error[0],16) for error in errors}==set(expected)
obj=CoffObject(Path('build/objdiff/base/SOURCE/GAME.obj').read_bytes())
fn,=[s for s in obj.symbols.values() if s.typ==FUNCTION_TYPE and s.name=='?PerWeek@game@@QAEXXZ']
raw=obj.section_bytes(obj.symbol_section(fn));seen=set()
for reloc in obj.relocations:
    if reloc.section!=fn.section or not fn.value<=reloc.site<fn.value+6824:continue
    owner=obj.symbols[reloc.symbol_index]
    if not owner.name.startswith('__real@'):continue
    offset=reloc.site-fn.value
    destination,value=expected[offset];seen.add(offset)
    assert reloc.typ==6 and raw[reloc.site:reloc.site+4]==bytes(4)
    assert 0x57ed6+offset in sites and u32(0x57ed6+offset)==destination
    encoded=struct.pack('<d',value)
    data=obj.section_bytes(obj.symbol_section(owner))[owner.value:owner.value+8]
    assert data==encoded==read(destination-base,8)
    error,=[error for error in errors if int(error[0],16)==offset]
    assert error==(hex(offset),owner.name,'0x0',hex(destination-base),None)
    print('PASS fixed PerWeek double',hex(offset),hex(destination),value,data.hex())
assert seen==set(expected)
print('PASS PerWeek complete bytes/size/DIR32 sites/all ordered targets, including exactly three fixed private doubles')

# Domain arithmetic: the unsigned13-bit owner consists of a12-bit count and
# guard bit. The mask makes integer remainder0..6, so no cast is necessary.
header=Path('include/SOURCE/GAME.h').read_text()
for name,value in [('MAP_MONSTER_COUNT_MASK','0xfff'),('MAP_MONSTER_GUARD_FLAG','0x1000'),('WEEKLY_MONSTER_LIMIT','4000')]:
    assert re.search(r'\b'+name+r'\s*=\s*'+value+r'\b',header)
assert 'u16 m_objectMetadata : 13;' in Path('include/EDITOR/mapcell.h').read_text()
for metadata in range(8192):
    count=metadata&0xfff
    for roll in range(1,8):
        growth=count//7+(roll<=count%7)
        updated=(metadata&0x1000)|min(4000,count+growth)
        assert 0<=count%7<=6 and 0<=updated<8192
        assert (updated&0x1000)==(metadata&0x1000) and updated&0xfff<=4000
print('PASS all8192 metadata values ×7 rolls: remainder0..6, guard preserved, final count<=4000 fits13 bits')
