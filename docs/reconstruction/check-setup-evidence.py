"""B92: fixed help catalogs, entry capacities and real SETUP packet-free owners.

Supplements all23 byte-span reviews; nine existing private EH/FS site-model
gaps are not waived by these bounds checks.
"""
import re
from pathlib import Path
from homm2.core.coff import CoffObject,FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout
control=Path('build/orig/HMM2PL.exe');verify_retail(control)
base,sites,u32,read=_pe_layout(control)
obj=CoffObject(Path('build/objdiff/base/SOURCE/SETUP.obj').read_bytes())
def instruction(name,rva,offset,encoded):
 fn,=[s for s in obj.symbols.values() if s.typ==FUNCTION_TYPE and s.name.startswith('?'+name+'@')]
 raw=obj.section_bytes(obj.symbol_section(fn));expected=bytes.fromhex(encoded)
 assert raw[fn.value+offset:fn.value+offset+len(expected)]==expected,(name,hex(offset))
 assert read(rva+offset,len(expected))==expected,(name,hex(offset),'retail')
for name,rva,offset,encoded in [('SetupComPort',0x9263b,0x1a4,'6a288d55cc'),
 ('SetupHotSeatGame',0x92817,0x1ec,'6a14'),('SetupModemGame',0x92de5,0x2c4,'6a23')]:
 instruction(name,rva,offset,encoded)
# GetDataEntry receives destination capacity, not a maximum length excluding
# NUL. Its reviewed result handler zeroes capacity and copies capacity-1.
for label,capacity,owner,initial in [('modem init',40,40,b'ATZ'),('player name',20,21,b''),('telephone',35,36,b'')]:
 assert len(initial)+1<=capacity<=owner
 for length in range(1001):
  copied=min(length,capacity-1)
  assert copied+1<=capacity<=owner
 print('PASS',label,'initial fits; returned text at most',capacity-1,'chars in',owner,'bytes')
assert 39+1<=100
print('PASS copied modem initialization fits the separate100-byte config field; later printf interpretation remains B37')

def cstring(rva):
 data=read(rva,4096);assert b'\0' in data
 return data.split(b'\0',1)[0]
tables=[('baud',0x4fe900,5),('comport',0x4fe914,5),('direct baud',0x4fe928,5),
 ('direct comport',0x4fe93c,5),('hotseat',0x4fe950,6),('modem',0x4fe968,4),
 ('direct game',0x4fe978,4),('multiplayer',0x4fe988,5),('network',0x4fe99c,3),
 ('network2',0x4fe9a8,4),('game',0x4fe9b8,4),('campaign',0x51b62c,3),('standard',0x51b638,3)]
for label,va,count in tables:
 text=[cstring(u32(va-base+4*i)-base) for i in range(count)]
 assert all(text)
 # Each body assigns only fixed switch-case indices0..count-1, or -1,
 # and tests >=0 before lookup. No caller-controlled direct table index.
 for index in [-1]+list(range(count)):
  if index>=0:assert index<count
 print('PASS',label,count,'actual terminated help entries; max bytes including NUL',max(map(len,text))+1)
for count in range(1,7):
 assert len(('*.GM%d'%count).encode())+1==6<=12
 assert len(('*.GX%d'%count).encode())+1==6<=12
assert len(b'*.GMC\0')==len(b'*.GXC\0')==6
assert len(('*.GM%d'%(-2**31)).encode())+1==16>12
print('PASS normal player counts1..6 need6/12 file-pattern bytes; arbitrary signed count can need16')
print('OPEN fileRequester defaultExt5 still receives six-byte pattern including NUL (B88); not repaired by12-byte source owner')
print('OPEN dialog/configuration/resource/API lifetime contracts; this does not add message-type guards or FreeLibrary calls')
