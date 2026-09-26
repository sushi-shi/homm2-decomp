"""C55: fixed numeric operands in the two changed army attack bodies.

Supplements check-function.py, without changing its general acceptance rules.
SpecialAttack's EH helper/FS-relative aliases remain a separate proof limit;
whole-native-object equivalence is the preservation proof for those operands.
"""
import struct
from pathlib import Path
from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control=Path('build/orig/HMM2PL.exe')
verify_retail(control)
base,sites,u32,read=_pe_layout(control)
obj=CoffObject(Path('build/objdiff/base/SOURCE/ARMY.obj').read_bytes())
cases=[
 ('?SpecialAttack@army@@QAEXXZ',0x1a1d9,5727,{
     0x45a:(0x4ea4f0,'d',180.0),0x460:(0x4ea4e8,'d',3.14159),
     0x4c3:(0x4ea4e4,'f',2.0),0x518:(0x4ea4e0,'f',25.0),
     0x544:(0x4ea4dc,'f',-25.0),0x8f5:(0x4ea4d8,'f',115.0)}),
 ('?DoAttack@army@@QAEXH@Z',0x1bddb,5189,{
     0xfd6:(0x4ea4f8,'f',150.0),0x11f6:(0x4ea4c8,'f',100.0)}),
]
for name,rva,size,expected in cases:
    fn,=[s for s in obj.symbols.values() if s.typ==FUNCTION_TYPE and s.name==name]
    raw=obj.section_bytes(obj.symbol_section(fn)); seen=set()
    for reloc in obj.relocations:
        if reloc.section!=fn.section or not fn.value<=reloc.site<fn.value+size: continue
        owner=obj.symbols[reloc.symbol_index]
        if not owner.name.startswith('__real@'): continue
        offset=reloc.site-fn.value
        destination,fmt,value=expected[offset]; seen.add(offset)
        encoded=struct.pack('<'+fmt,value)
        assert reloc.typ==6 and raw[reloc.site:reloc.site+4]==bytes(4)
        assert rva+offset in sites and u32(rva+offset)==destination
        data=obj.section_bytes(obj.symbol_section(owner))[owner.value:owner.value+len(encoded)]
        assert data==encoded==read(destination-base,len(encoded)),(name,hex(offset),value,data.hex())
        print('PASS',hex(rva),hex(offset),hex(destination),fmt,value,data.hex())
    assert seen==set(expected)
    print('PASS',name,'all',len(expected),'fixed private numeric operands')
