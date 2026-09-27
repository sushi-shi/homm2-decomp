"""Fixed private numeric operands for C51 and the B71 hero-sharing proof.

Complements, never replaces, each check-function.py ordinary-byte/site/target
report. Only the explicitly listed operand destinations and payloads pass.
"""
import struct
from pathlib import Path
from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout
control=Path('build/orig/HMM2PL.exe')
verify_retail(control)
base,sites,u32,read=_pe_layout(control)
obj=CoffObject(Path('build/objdiff/base/SOURCE/PHILAI.obj').read_bytes())
cases=[
 ('?ValueOfTown@philAI@@QAEHPAVtown@@@Z',0x85601,319,{
     0x64:(0x4ea760,'f',250.0),0x70:(0x4ea754,'f',5.0),0x76:(0x4ea508,'d',1.5)}),
 ('?TurnCostResource@philAI@@QAEXH@Z',0x85740,270,{
     0x60:(0x4ea5a8,'d',0.7),0xf0:(0x4ea4e4,'f',2.0),0xf6:(0x4ea500,'d',0.5)}),
 ('?TurnValueOfObelisk@philAI@@QAEMH@Z',0x8584e,307,{
     0x74:(0x4ea520,'f',0.0),0xac:(0x4ea608,'d',1.4),0xf0:(0x4ea764,'f',48.0),
     0xf6:(0x4ea508,'d',1.5),0x116:(0x4ea620,'d',0.66)}),
 ('?HeroInteractionAtHero@philAI@@QAEXPAVhero@@0HPAH@Z',0x86b43,909,{
     0x19d:(0x4ea4b8,'d',1.0),0x1bf:(0x4ea618,'d',0.6),
     0x259:(0x4ea6e8,'d',0.15),0x379:(0x4ea4c0,'d',0.1)}),
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
    print('PASS',name,'all',len(expected),'fixed private operands')
