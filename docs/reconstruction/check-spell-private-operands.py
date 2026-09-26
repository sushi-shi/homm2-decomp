"""O05/C58/B75: fixed private numeric operands; no generic checker relaxation.

Together with check-function.py this closes the listed non-EH functions.
DoBolt and Armageddon retain separate EH-helper/FS-relative proof limits;
their complete before/after native-object comparison proves preservation.
"""
import struct
from pathlib import Path
from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control=Path('build/orig/HMM2PL.exe')
verify_retail(control)
base,sites,u32,read=_pe_layout(control)
obj=CoffObject(Path('build/objdiff/base/SOURCE/SPELLS.obj').read_bytes())
cases=[
 ('Fireball',0x9b2bd,1352,{0xda:(0x4ea54c,'f',75),0x47c:(0x4ea500,'d',0.5)}),
 ('MeteorShower',0x9b805,816,{0xeb:(0x4ea848,'d',112.5)}),
 ('ResetBoltAngle',0x9cc2f,602,{
     0x196:(0x4ea748,'d',2.5),0x19c:(0x4ea460,'d',2),0x1d6:(0x4ea508,'d',1.5),
     0x202:(0x4ea4c8,'f',100),0x224:(0x4ea4c8,'f',100),0x233:(0x4ea4e4,'f',2),0x23c:(0x4ea508,'d',1.5)}),
 ('DoBolt',0x9d510,2338,{
     0x5f5:(0x4ea5b0,'d',0.75),0x658:(0x4ea4c8,'f',100),0x78c:(0x4ea620,'d',0.66),
     0x792:(0x4ea860,'d',20),0x7a1:(0x4ea620,'d',0.66),0x7a7:(0x4ea858,'d',-20)}),
 ('GetNextChainLightningTarget',0x9de32,356,{0xd5:(0x4ea520,'f',0)}),
 ('ChainLightning',0x9df96,797,{
     0x106:(0x4ea500,'d',0.5),0x25d:(0x4ea4c8,'f',100),0x2c4:(0x4ea4c8,'f',100)}),
 ('DoBlast',0xa0178,699,{0x177:(0x4ea520,'f',0),0x22b:(0x4ea570,'f',10)}),
 ('RippleCreature',0x9e51a,1261,{
     0x18b:(0x4ea850,'f',20),0x19f:(0x4ea500,'d',0.5),0x468:(0x4ea6a0,'f',500)}),
 ('VaporizeCreature',0x9e2b3,615,{0x1fc:(0x4ea6a0,'f',500)}),
 ('Resurrect',0xa0433,1426,{0x45a:(0x4ea54c,'f',75)}),
 ('Armageddon',0x9bdf2,2660,{
     0xe1:(0x4ea500,'d',0.5),0x120:(0x4ea500,'d',0.5),0x254:(0x4ea54c,'f',75),
     0x3c6:(0x4ea850,'f',20),0x557:(0x4ea5b0,'d',0.75),0x6ab:(0x4ea550,'f',15),0x9bb:(0x4ea850,'f',20)}),
]
for name,rva,size,expected in cases:
    fn,=[s for s in obj.symbols.values() if s.typ==FUNCTION_TYPE and s.name.startswith('?'+name+'@combatManager@@')]
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
        print('PASS',name,hex(offset),hex(destination),fmt,value,data.hex())
    assert seen==set(expected)
print('PASS all fixed private numeric operands in eleven functions')
