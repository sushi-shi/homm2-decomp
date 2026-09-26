"""Fixed-site float supplement for the two B69 movement functions.

Run with check-function.py for each function; this does not replace ordinary
byte, DIR32-site or other ordered-target checks.
"""
import struct
from pathlib import Path
from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control=Path('build/orig/HMM2PL.exe')
verify_retail(control)
base, sites, u32, read=_pe_layout(control)
obj=CoffObject(Path('build/objdiff/base/SOURCE/PHILAI.obj').read_bytes())
cases=[
    ('?DoAI@philAI@@QAEXH@Z',0x7fd23,2879,{
        0x3b6:(0x4ea4c0,0.1),0x3bc:(0x4ea680,1.7),
        0x3eb:(0x4ea4a0,0.06),0x3f1:(0x4ea610,0.8),
    }),
    ('?DetermineTargetPosition@philAI@@QAEHAAH0H0@Z',0x81c03,3430,{
        0xfc:(0x4ea678,1.15),0x113:(0x4ea5b8,1.35),
        0x12a:(0x4ea6c0,1.25),0x150:(0x4ea6b8,0.9),
        0x176:(0x4ea6b0,1.05),0x19e:(0x4ea4a8,1.1),
        0x1bb:(0x4ea678,1.15),
    }),
]
for name,rva,size,expected in cases:
    fn,=[s for s in obj.symbols.values() if s.typ==FUNCTION_TYPE and s.name==name]
    raw=obj.section_bytes(obj.symbol_section(fn))
    seen=set()
    for reloc in obj.relocations:
        if reloc.section!=fn.section or not fn.value<=reloc.site<fn.value+size:
            continue
        owner=obj.symbols[reloc.symbol_index]
        if not owner.name.startswith('__real@'):
            continue
        offset=reloc.site-fn.value
        destination,value=expected[offset]
        seen.add(offset)
        encoded=struct.pack('<d',value)
        assert reloc.typ==6 and raw[reloc.site:reloc.site+4]==bytes(4)
        assert rva+offset in sites and u32(rva+offset)==destination
        data=obj.section_bytes(obj.symbol_section(owner))[owner.value:owner.value+8]
        assert data==encoded==read(destination-base,8),(name,hex(offset),value,data.hex())
        print('PASS',hex(rva),hex(offset),hex(destination),value,data.hex())
    assert seen==set(expected)
    print('PASS',name,'all',len(expected),'fixed double operands')
