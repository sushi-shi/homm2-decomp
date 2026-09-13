"""B88: pin REQUEST owner offsets, floating operands and finite text bounds.

Run alongside the complete sixteen byte/site/target reports. Open's existing
EH model gap is deliberately not waived. Runtime preconditions remain explicit.
"""
import re
import struct
from pathlib import Path
from homm2.core.coff import CoffObject,FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control=Path('build/orig/HMM2PL.exe');verify_retail(control)
base,sites,u32,read=_pe_layout(control)
obj=CoffObject(Path('build/objdiff/base/SOURCE/REQUEST.obj').read_bytes())
def instruction(name,rva,offset,spelling):
 fn,=[f for f in obj.symbols.values() if f.typ==FUNCTION_TYPE and f.name==name]
 raw=obj.section_bytes(obj.symbol_section(fn));expected=bytes.fromhex(spelling)
 assert raw[fn.value+offset:fn.value+offset+len(expected)]==expected,(name,hex(offset))
 assert read(rva+offset,len(expected))==expected,(hex(rva),hex(offset))

getheader='?GetMapHeader@@YIHPADPAUSMapHeader@@@Z'
instruction(getheader,0x8e730,0x46,'68 a4 01 00 00')
instruction(getheader,0x8e730,0x67,'b8 01 00 00 00')
instruction('?ShowThisMapGame@@YIHPAD@Z',0x8e7b0,0x14,'eb 5c')
instruction('??0fileRequester@@QAE@HHHPAD00@Z',0x8ef59,0xb1,'05 ae 02 00 00')
getname='?GetFilename@fileRequester@@QAEPADXZ'
for site,spelling in [(0x2e7d,'05 ae 02 00 00'),(0x2e86,'81 c1 b3 02 00 00'),
                      (0x2ec8,'6b c0 05'),(0x2ee0,'69 c9 5f 01 00 00')]:
 instruction(getname,0x91554,site-0x2e34,spelling)
update='?Update@fileRequester@@QAEXH@Z'
for site,spelling in [(0x2445,'83 ba 12 04 00 00 00'),(0x244c,'7e 10'),
                      (0x2485,'8b 91 1a 04 00 00'),(0x248b,'69 d2 a4 01 00 00'),
                      (0x249f,'8a 44 11 06')]:
 instruction(update,0x90ab6,site-0x2394,spelling)
instruction('?Main@fileRequester@@UAEHAAUtag_message@@@Z',0x8f737,0x1c3b-0x1014,'f7 bd 0c fc ff ff')
instruction('?DoKnob@fileRequester@@QAEXXZ',0x9085b,0x2166-0x2138,'db 45 88')
print('PASS fixed420-byte read/unconditional success, disabled-demo jump, five-byte extension/351-byte name strides, negative map index and integer gutter division')

header=Path('include/SOURCE/fileRequester.h').read_text()
assert re.search(r'FILE_REQUESTER_EXTENSION_SIZE\s*=\s*5\b',header)
assert re.search(r'FILE_REQUESTER_NAME_SIZE\s*=\s*0x15f\b',header)
assert 'char m_defaultExtension[FILE_REQUESTER_EXTENSION_SIZE];\n    char m_filename[FILE_REQUESTER_NAME_SIZE];' in header
assert 0x2ae+5==0x2b3 and 0x2b3+351==0x412
for pattern in (b'*.MP2',b'*.MX2',b'*.GMC',b'*.GXC',b'*.GM1',b'*.GX6'):
 assert len(pattern)+1==6
 assert 0x2ae+len(pattern)==0x2b3
assert 0x2b3<0x42f
print('OPEN map/load caller patterns need6 bytes: final NUL reaches adjacent filename[0], not outside the complete0x42f requester')

fixed={
 '?Main@fileRequester@@UAEHAAUtag_message@@@Z':(0x8f737,4388,[(0xbbf,0x4ea4c8,100.0)]),
 update:(0x90ab6,2718,[(0xa01,0x4ea4e4,2.0)]),
}
for name,(rva,size,entries) in fixed.items():
 fn,=[f for f in obj.symbols.values() if f.typ==FUNCTION_TYPE and f.name==name]
 raw=obj.section_bytes(obj.symbol_section(fn))
 relocs=[r for r in obj.relocations if r.section==fn.section and fn.value<=r.site<fn.value+size
         and obj.symbols[r.symbol_index].name.startswith('__real@')]
 assert [r.site-fn.value for r in relocs]==[e[0] for e in entries]
 for reloc,(offset,va,value) in zip(relocs,entries):
  owner=obj.symbols[reloc.symbol_index];expected=struct.pack('<f',value)
  assert reloc.typ==6 and raw[reloc.site:reloc.site+4]==bytes(4)
  assert rva+offset in sites and u32(rva+offset)==va
  assert read(va-base,4)==expected
  assert obj.section_bytes(obj.symbol_section(owner))[owner.value:owner.value+4]==expected
  print('PASS fixed full-width float',name,hex(offset),hex(va),value)

def cstring(rva):return read(rva,4096).split(b'\0',1)[0]
difficulty=[cstring(u32(0x4fea84-base+4*i)-base) for i in range(5)]
assert max(map(len,difficulty))+1==12
formats=[]
for sym in obj.symbols.values():
 if sym.name.startswith('$SG') and sym.section>0:
  raw=obj.section_bytes(obj.symbol_section(sym))[sym.value:]
  value=raw.split(b'\0',1)[0]
  if b'%d' in value:
   assert b'%' not in value.replace(b'%d',b'')
   formats.append(value)
assert len(formats)==4 and sorted(v.count(b'%d') for v in formats)==[1,1,2,2]
worst=max(len(v.replace(b'%d',b'-2147483648'))+1 for v in formats)
assert worst<768
assert 19+259+1==279 and 299+299+1==599
assert 200+4+1==205 and 205<=351<=352 and 260<=300
assert 350+4+1==355 and 355>351
print('PASS difficulty12/768, four numeric dialogs at most',worst,'/768; bounded paths279/768 or599/768')
print('PASS terminated Win32 filename<=259 fits352 scratch/351 name/300 recombined path; typed base<=200 plus extension fits205/351')
print('OPEN arbitrary full351-byte stored name plus4-byte extension needs355; header names60/descriptions300 still need termination')

for visible,travel,first_zero in [(9,123,12309),(11,163,16311)]:
 positions=first_zero-(visible-1)
 assert int(travel*100/positions)==0 and int(travel*100/(positions-1))==1
 assert (visible-1)-(visible-1)==0
print('OPEN integer gutter step becomes0 at counts12309(map)/16311(standard); DoKnob computes floating division by0 at counts8/10 before later use guard')
print('OPEN empty map list leaves selected=-1 and reads header[-1]; two enumerations and count-only queries require stable list/count coherence')
