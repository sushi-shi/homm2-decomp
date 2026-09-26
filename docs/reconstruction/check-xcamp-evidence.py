"""C78/O08/B93: physical campaign owners, fixed catalogs and conditional bounds.

The ShowInfo private EH/FS model gap is not waived. Bounds for valid campaign
state do not establish safety for forged messages, corrupt saves or API failure.
"""
import struct
from pathlib import Path
from homm2.core.coff import CoffObject,FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout
control=Path('build/orig/HMM2PL.exe');verify_retail(control)
base,sites,u32,read=_pe_layout(control)
obj=CoffObject(Path('build/objdiff/base/SOURCE/X_CAMPGN.obj').read_bytes())
for name,rva,owner,va in [('JosephName',0xb51e3,'xJosephName',0x51b7d0),('IvanName',0xb520b,'xUncleIvanName',0x51b7e8)]:
 fn,=[s for s in obj.symbols.values() if s.typ==FUNCTION_TYPE and s.name=='?'+name+'@ExpCampaign@@QAEPADXZ']
 raw=obj.section_bytes(obj.symbol_section(fn))[fn.value:fn.value+40]
 rs=[r for r in obj.relocations if r.section==fn.section and fn.value<=r.site<fn.value+40]
 assert [(r.site-fn.value,r.typ,obj.symbols[r.symbol_index].name) for r in rs]==[(0x11,6,'?'+owner+'@@3PAPADA'),(0x20,6,'?'+owner+'@@3PAPADA')]
 assert raw[0x20:0x24]==bytes.fromhex('f0ffffff')
 assert raw[0x1d:0x20]==bytes.fromhex('8b0495')
 assert u32(rva+0x11)==va and u32(rva+0x20)==va-16
 for scenario in range(4,8):
  index=scenario-4
  assert 0<=index<6
  assert va+4*index==u32(rva+0x20)+4*scenario
 print('PASS O08',name,'real6-entry owner, -16 DIR32 addend and every valid map4..7 destination')

def cstring(rva):
 data=read(rva,4096);assert b'\0' in data
 return data.split(b'\0',1)[0]
def catalog(va,n):return [cstring(u32(va-base+4*i)-base) for i in range(n)]
tables={name:catalog(va,n) for name,va,n in [('scenario',0x51b670,32),('description',0x51b6f0,32),('award',0x51b644,11),('short',0x51b770,4),('army',0x4fded0,66),('resource',0x4fdffc,7),('spell',0x4fec04,65),('artifact',0x4fd7ac,103),('skill',0x4fed14,14),('level',0x4fed08,3),('shortlevel',0x51b800,3),('stat',0x4fdc80,4),('align',0x4fdca0,6)]}
for name,entries in tables.items():print('PASS actual catalog',name,len(entries),'max including NUL',max(map(len,entries))+1)
assert max(map(len,tables['army']))+1==22<=52
assert max(map(len,tables['description']))+1==256<=768
assert sum(map(len,tables['award']))+11+1==146<=768
assert not any(b'%' in s for s in tables['align'])
for campaign,count in enumerate([8,8,4,4]):
 assert u32(0x11b1b8+4*campaign)==count
 for scenario in range(count):
  assert len(('CAMP%d_%02d.HXC'%(campaign+1,scenario+1)).encode())+1==13
  assert len(tables['short'][campaign])+1+len(str(scenario+1))+1<=768
  for choice in range(3):
   kind,value,amount=struct.unpack('<Bhh',read(0x11b1c8+5*((campaign*8+scenario)*3+choice),5))
   domains={0:'resource',1:'artifact',2:'spell',3:'skill',4:'army',8:'align',9:'stat',10:'spell'}
   assert kind in range(11),(campaign,scenario,choice,kind)
   if kind in domains:assert 0<=value<len(tables[domains[kind]]),(kind,value)
   if kind==3:assert 1<=amount<=3
   if kind==10:assert 0<=value<=127
   if kind in (0,4,9):assert len(tables[domains[kind]][value])+2+len(str(amount))+1<=768
   if kind==3:assert max(map(len,tables['level']))+1+len(tables['skill'][value])+1<=768
   if kind==10:assert len(tables['spell'][value])+1+6+1<=768
print('PASS72 valid-map bonus records: actual tagged indices and skill levels; negative resource penalties retained')
smack=CoffObject(Path('build/objdiff/base/SOURCE/SMACKMGR.obj').read_bytes())
fn,=[s for s in smack.symbols.values() if s.typ==FUNCTION_TYPE and s.name=='?PlaySmacker@@YIHH@Z']
raw=smack.section_bytes(smack.symbol_section(fn))[fn.value:fn.value+315]
# Clear xLastChoice to -1, then take a zero-return epilogue if no-CD is set.
for offset,encoded in [(0xf,'c705'),(0x15,'ffffffff'),(0x19,'833d'),(0x1f,'00740733c0e90e010000')]:
 expected=bytes.fromhex(encoded)
 assert raw[offset:offset+len(expected)]==expected
 assert read(0x9592a+offset,len(expected))==expected
assert u32(0x9592a+0x11)==0x534858
print('PASS fixed filenames13/13, descriptions256/768, all11awards146/768, creature names22/52; no percent directives in actual alignment formats')
print('OPEN unchecked campaign/current/view/bonus/count domains, failed map header, serialized pointer/gap bytes and window lifetime')
print('OPEN track event cases0..11 exceed arrays8; dynamic normal icons0..mapCount-1 only. No invalid-event execution claimed')
print('OPEN PlaySmacker resets choice=-1 before no-CD return; Choose/Setup pass through without validity guard')
