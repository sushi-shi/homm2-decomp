"""S44 supplement: prove the twelve private 0.33/0.66 retail operands.

The general checker remains strict about unknown owners. Only this fixed
function, operand list and two exact double payloads are checked here.
"""
import struct
from pathlib import Path
from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control=Path('build/orig/HMM2PL.exe'); verify_retail(control)
base,sites,u32,read=_pe_layout(control)
obj=CoffObject(Path('build/objdiff/base/SOURCE/GAME.obj').read_bytes())
fn,=[s for s in obj.symbols.values() if s.typ==FUNCTION_TYPE and s.name=='?SetupNewRumour@game@@QAEXXZ']
raw=obj.section_bytes(obj.symbol_section(fn))
expected={
    0x2b3:0.33,0x2e3:0.33,0x31f:0.33,0x34f:0.66,
    0x38b:0.33,0x3c7:0.66,0x3f7:0.33,0x433:0.66,
    0x469:0.66,0x4b1:0.66,0x4f9:0.33,0x53e:0.66,
}
seen=set()
for reloc in obj.relocations:
    if reloc.section!=fn.section or not fn.value<=reloc.site<fn.value+1613:continue
    owner=obj.symbols[reloc.symbol_index]
    if not owner.name.startswith('__real@'):continue
    offset=reloc.site-fn.value; value=expected[offset]; seen.add(offset)
    assert reloc.typ==6 and raw[reloc.site:reloc.site+4]==bytes(4)
    assert 0x5fdd7+offset in sites
    destination=u32(0x5fdd7+offset)-base
    assert destination=={0.33:0xea628,0.66:0xea620}[value]
    data=obj.section_bytes(obj.symbol_section(owner))[owner.value:owner.value+8]
    assert data==read(destination,8)==struct.pack('<d',value)
    print('PASS',hex(offset),value,hex(destination+base),data.hex())
assert seen==set(expected)
