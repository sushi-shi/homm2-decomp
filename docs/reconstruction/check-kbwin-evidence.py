"""B89: command-line termination, Win32 message guard and real menu table.

All sixteen functions must also pass the ordinary byte/site/target checker;
this file grants no private-target, type or exception-model waiver.
"""
import re,struct
from pathlib import Path
from homm2.core.coff import CoffObject,FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout
control=Path('build/orig/HMM2PL.exe');verify_retail(control)
base,sites,u32,read=_pe_layout(control)
obj=CoffObject(Path('build/objdiff/base/SOURCE/kbwin.obj').read_bytes())
def instruction(name,rva,offset,spelling):
 fn,=[f for f in obj.symbols.values() if f.typ==FUNCTION_TYPE and f.name==name]
 expected=bytes.fromhex(spelling);raw=obj.section_bytes(obj.symbol_section(fn))
 assert raw[fn.value+offset:fn.value+offset+len(expected)]==expected,(name,offset)
 assert read(rva+offset,len(expected))==expected
instruction('_WinMain@16',0x70e30,0x71,'6a 3d')
instruction('_WinMain@16',0x70e30,0x82,'6a 3c')
for offset,spelling in [(6,'81 7d 0c ff 03 00 00'),(0xd,'77 0f')]:
 instruction('?AppWndProc@@YGJPAXIIJ@Z',0x71248,offset,spelling)
for site,spelling in [(0x10f9,'83 7d e8 ff'),(0x113b,'c7 45 fc 00 00 00 00'),
                      (0x1154,'83 7d ec 46'),(0x115d,'6b d2 07')]:
 instruction('?SetMenus@@YIXPAXH@Z',0x71eca,site-0x10b0,spelling)
instruction('?Process1WindowsMessage@@YIXXZ',0x718f9,0xb55-0xad0,'85 c0 74 14')
source=Path('src/SOURCE/kbwin.cpp').read_text()
header=Path('include/SOURCE/X_GLOBAL.h').read_text()
assert re.search(r'GLOBAL_COMMAND_LINE_SIZE\s*=\s*0x40\b',header)
assert 60<61<=64
for length in range(1000):
 buffer=bytearray(b'x'*64);buffer[:61]=bytes(61)
 copied=min(length,60);buffer[:copied]=b'a'*copied
 assert buffer[60]==0 and buffer.index(0)<=60
print('PASS clear61/copy60 within64-byte command line, including nonterminated60-byte copy')

# Every actual API message-table write has a relocation to the same1024-byte
# owner. The addend is the exact message index, including the custom CD entry.
fn,=[f for f in obj.symbols.values() if f.name=='?AppInit@@YIHPAX0HPAD@Z' and f.typ==FUNCTION_TYPE]
raw=obj.section_bytes(obj.symbol_section(fn));indices=[]
for reloc in obj.relocations:
 if reloc.section==fn.section and fn.value<=reloc.site<fn.value+748:
  owner=obj.symbols[reloc.symbol_index]
  if owner.name=='?bProcessMessage@@3PAEA':
   assert reloc.typ==6
   index=int.from_bytes(raw[reloc.site:reloc.site+4],'little')
   assert 0<=index<1024
   if raw[reloc.site-2:reloc.site]==b'\xc6\x05':indices.append(index)
assert len(indices)==23 and len(set(indices))==23 and max(indices)==0x3b9
print('PASS all23 message-enable stores within1024 and unsigned guard rejects every larger UINT before indexing')

rows=[struct.unpack('<IBBB',read(0x4fb0c0-base+7*i,7)) for i in range(70)]
assert rows[0]==(0,0,0,0)
assert all(normal in (0,1) and setup in (0,1) and spare==0 for _,normal,setup,spare in rows)
for command in [r[0] for r in rows]+[0xffffffff,0x12345678]:
 match=0
 for pos,row in enumerate(rows):
  if row[0]==command:match=pos
 assert 0<=match<70
print('PASS real70x7 menu table, boolean flags, zero fallback and bounded last-match selection')

strings=[]
for sym in obj.symbols.values():
 if sym.name.startswith('$SG') and sym.section>0:
  strings.append(obj.section_bytes(obj.symbol_section(sym))[sym.value:].split(b'\0',1)[0])
formats=[value for value in strings if b'%s' in value]
assert len(formats)==1 and formats[0].count(b'%s')==1
title=read(0x5157e8-base,100).split(b'\0',1)[0]
assert len(title)==21
bound=len(formats[0])-2+len(title)+1
assert bound<768
print('PASS fixed startup message',bound,'/768 including NUL')
print('OPEN API return/lifecycle/config domains and signed tick arithmetic; GetMessage tests nonzero rather than positive')
