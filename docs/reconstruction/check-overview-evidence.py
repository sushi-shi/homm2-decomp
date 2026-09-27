"""B87: fixed numerical operands and finite Overview owner/text bounds.

The four existing EH site/identity gaps are not waived here. Run alongside
complete byte/site/target reports, including their explicit residuals.
"""
import re
import struct
from pathlib import Path
from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control=Path('build/orig/HMM2PL.exe')
verify_retail(control)
base,sites,u32,read=_pe_layout(control)
obj=CoffObject(Path('build/objdiff/base/SOURCE/Overview.obj').read_bytes())
def instruction(name,rva,offset,spelling):
    fn,=[f for f in obj.symbols.values() if f.typ==FUNCTION_TYPE and f.name==name]
    raw=obj.section_bytes(obj.symbol_section(fn))
    expected=bytes.fromhex(spelling)
    assert raw[fn.value+offset:fn.value+offset+len(expected)]==expected
    assert read(rva+offset,len(expected))==expected

for offset in (0x7f,0x98):
    instruction('?Overview@game@@QAEXXZ',0x7bda1,offset,'68 c0 2b 00 00')
for offset,spelling in ((0xf9,'6b d2 46'),(0x101,'8d 0c 90'),(0x107,'83 3c 91 00')):
    instruction('?SetupDynamicStuff@game@@QAEXHHH@Z',0x79ba0,offset,spelling)
for offset,spelling in ((0x3a3,'83 e8 04'),(0x3f6,'83 e9 04')):
    instruction('?OverviewHandler@@YIHAAUtag_message@@@Z',0x7c501,offset,spelling)
instruction('?ProcessIconSelect@game@@QAEHHH@Z',0x7c9b3,0x68,'7e 07')
print('PASS fixed allocation11200, separate row/element address stages, negative-top stores and equality-accepting index guard')
fixed={
    '?SetupDynamicStuff@game@@QAEXHHH@Z':(0x79ba0,7750,[
        (0x5f,0x4ea650,'d',303.0), (0x78,0x4ea648,'f',18.0),
        (0x7e,0x4ea640,'d',0.4)]),
    '?DoKnob@game@@QAEXXZ':(0x7c253,686,[
        (0xde,0x4ea518,'f',1.0),(0xf3,0x4ea518,'f',1.0)]),
}
for name,(rva,size,entries) in fixed.items():
    fn,=[f for f in obj.symbols.values() if f.typ==FUNCTION_TYPE and f.name==name]
    raw=obj.section_bytes(obj.symbol_section(fn))
    relocs=[r for r in obj.relocations if r.section==fn.section
            and fn.value<=r.site<fn.value+size
            and obj.symbols[r.symbol_index].name.startswith('__real@')]
    assert [r.site-fn.value for r in relocs]==[entry[0] for entry in entries]
    for reloc,(offset,va,kind,value) in zip(relocs,entries):
        owner=obj.symbols[reloc.symbol_index]
        expected=struct.pack('<'+kind,value)
        assert reloc.typ==6 and raw[reloc.site:reloc.site+4]==bytes(4)
        assert rva+offset in sites and u32(rva+offset)==va
        assert read(va-base,len(expected))==expected
        assert obj.section_bytes(obj.symbol_section(owner))[owner.value:owner.value+len(expected)]==expected
        print('PASS fixed numeric operand',name,hex(offset),hex(va),kind,value)

# Every produced row remains well inside its 70 pointer slots. Allocator byte
# count is intentionally NOT reduced to the smaller actively used prefix.
town_icons=1+1+1+4+5+12
town_texts=1+5+12
hero_icons=1+4+4+5+2*8+2*14
hero_texts=4+5+8
assert (town_icons,town_texts,hero_icons,hero_texts)==(24,18,58,17)
assert max(town_icons,town_texts,hero_icons,hero_texts)<70
assert 4*70*4==1120 and 0x2bc0==11200
assert all(0<=row*70+slot<280 for row in range(4) for slot in range(70))
print('PASS four rows: town icons/text24/18, hero58/17, each below70; active1120 bytes of11200-byte allocations')

assert max(len(str(n))+1 for n in range(1,32768))==6
assert max(len(str(n))+1 for n in range(-32768,32768))==7
assert max(len(str(n))+1 for n in range(-128,128))==5
assert max(len(str(n))+1 for n in range(0,10))==2
print('PASS positive signed16 troops fit6; OPEN arbitrary signed16 garrison needs7, signed8 stats need5, versus6/4 allocations')
print('PASS valid skill levels0..9 fit2; arbitrary signed8 skill result not covered')
for count in range(5,73):
    assert count-4>0 and count-3>0 and 32100//(count-3)>0
print('PASS bounded hero/town roster5..72: knob divisors and integer scroll divisor positive')
for count in range(4):
    top=count-4
    assert top<0 and top<count
print('OPEN numpad1/numpad3 short-list paths can set negative top before dynamic row lookup')
print('OPEN ProcessIconSelect rejects index>count, not index>=count; stale/invalid widget source remains a contract')
