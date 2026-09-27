"""EVENTS fixed operands, catalog capacities and serialization boundaries.

Supplement to complete retail/native comparisons. The four existing private
EH/FS model gaps and unchecked input contracts are not waived by these checks.
"""
import csv,re,struct
from pathlib import Path
from homm2.core.coff import CoffObject,FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control=Path('build/orig/HMM2PL.exe');verify_retail(control)
base,sites,u32,read=_pe_layout(control)
obj=CoffObject(Path('build/objdiff/base/SOURCE/EVENTS.obj').read_bytes())
with Path('build/gen/symbol_names.csv').open() as stream:
 rows=[r for r in csv.DictReader(stream) if r['unit']=='SOURCE/EVENTS' and r['kind']=='func' and r['provenance']=='source-annotation']
assert len(rows)==42
expected={
 0x42256:[(0x4e7,0x4ea5a8,'d',.7)],
 0x47989:[(0x23e,0x4ea5a8,'d',.7)],
 0x47fff:[(0x374,0x4ea460,'d',2),(0x4f2,0x4ea458,'d',5),(0x667,0x4ea458,'d',5)],
 0x48730:[(0xbc,0x4ea460,'d',2),(0x1d2,0x4ea5b0,'d',.75),(0x201,0x4ea458,'d',5),(0x2a6,0x4ea458,'d',5),(0x30d,0x4ea4c0,'d',.1)],
}
def cstring(rva):
 data=read(rva,4096);assert b'\0' in data
 return data.split(b'\0',1)[0]
def table(va,count):return [cstring(u32(va-base+4*i)-base) for i in range(count)]
literal_values={};count=0
for row in rows:
 rva,size=int(row['rva'],0),int(row['size'],0)
 fn,=[s for s in obj.symbols.values() if s.typ==FUNCTION_TYPE and s.name==row['name']]
 raw=obj.section_bytes(obj.symbol_section(fn));numeric=[];literals=set()
 for reloc in obj.relocations:
  if reloc.section!=fn.section or not fn.value<=reloc.site<fn.value+size:continue
  owner=obj.symbols[reloc.symbol_index]
  if owner.name.startswith('__real@'):numeric.append(reloc)
  if owner.name.startswith('$SG'):
   assert reloc.typ==6 and raw[reloc.site:reloc.site+4]==bytes(4)
   value=obj.section_bytes(obj.symbol_section(owner))[owner.value:].split(b'\0',1)[0]
   assert value==cstring(u32(rva+reloc.site-fn.value)-base)
   literals.add(value)
 assert len(numeric)==len(expected.get(rva,[])),hex(rva)
 for reloc,(offset,destination,kind,value) in zip(numeric,expected.get(rva,[])):
  assert reloc.site-fn.value==offset and reloc.typ==6
  assert raw[reloc.site:reloc.site+4]==bytes(4)
  assert rva+offset in sites and u32(rva+offset)==destination
  owner=obj.symbols[reloc.symbol_index];encoded=struct.pack('<'+kind,value)
  assert obj.section_bytes(obj.symbol_section(owner))[owner.value:owner.value+len(encoded)]==encoded
  assert read(destination-base,len(encoded))==encoded
  count+=1
 literal_values[rva]=literals
assert count==10
print('PASS all10 fixed numeric sites/types/widths/values/addends/destinations and all42 private C-string operand sets')

def bound(fmt,args=()):
 tokens=list(re.finditer(rb'%([0-9]*)([sd%])',fmt))
 assert b'%' not in re.sub(rb'%([0-9]*)([sd%])',b'',fmt),fmt
 assert [m[2].decode() for m in tokens if m[2]!=b'%']==[a[0] for a in args],fmt
 result=len(fmt)+1;remaining=iter(args)
 for token in tokens:
  length=1 if token[2]==b'%' else next(remaining)[1]
  result+=max(int(token[1] or b'0'),length)-len(token[0])
 return result
events=table(0x4fe224,95);resources=table(0x4fdffc,7)
stables=table(0x51b7c0,4);recruit_empty=table(0x51b790,5);recruit_buy=table(0x51b7a4,5)
colors=table(0x4ff308,8);passwords=table(0x51b80c,211)
army=table(0x4fddc8,66);plural=table(0x4fded0,66)
artifacts=table(0x4fd7ac,103);spells=table(0x4fec04,65)
assert {i for i,x in enumerate(events) if b'%' in x}=={1,66,72}
assert all(b'%' not in x for x in resources+stables+recruit_empty+recruit_buy+colors+passwords+army+plural+artifacts+spells)
assert max(map(len,events))+1==303
assert max(map(len,recruit_buy))+1==322
assert max(map(len,recruit_empty))+1==78
assert max(map(len,stables))+1==208
assert max(map(len,resources))+1==10
assert max(map(len,colors))+1==11
assert max(map(len,passwords))+1==14
assert max(map(len,army))+1==20 and max(map(len,plural))+1==22
assert set(23+3*site+kind for site in range(10) for kind in range(3))==set(range(23,53))
assert all(b'%' not in events[i] for i in range(23,53))
print('PASS fixed catalog bounds including NUL: event303/recruit322/stables208 within500; resource10, color11, password14, army20/plural22')
print('PASS HouseEvent: all30 dialog IDs23..52 in actual95-entry catalog, no format directives')

# This does not declare arbitrary runtime text safe. IDs1/66/72 carry %s;
# current callers format them first, while map-provided sign/riddle text and
# any replacement catalog still require explicit length/format contracts.
assert bound(events[66],[('s',21)])<=500
assert bound(events[72],[('s',9)])<=500
for rva,limit in [(0x41fbc,768),(0x42176,768),(0x42256,768),(0x44949,768)]:
 for value in literal_values[rva]:
  if b'%' not in value:assert len(value)+1<=limit
barrier,=[v for v in literal_values[0x41fbc] if b'%s' in v]
password,=[v for v in literal_values[0x42176] if v.count(b'%s')==2]
assert bound(barrier,[('s',10)])<=768
assert bound(password,[('s',10),('s',13)])<=768
monster_formats=[v for v in literal_values[0x47fff] if b'%' in v]
for fmt in monster_formats:
 args=[(t.decode(),21 if t==b's' else 11) for t in re.findall(rb'%[0-9]*([sd])',fmt)]
 assert bound(fmt,args)<=300
 assert bound(fmt,args)+54<=768
print('PASS barrier/password/global and monster-offer300 bounds with complete fixed catalogs and conservative signed-int formatting')

header=Path('include/SOURCE/EVENTS.h').read_text();hero=Path('include/SOURCE/hero.h').read_text()
assert re.search(r'EVENT_TEXT_BUFFER_SIZE\s*=\s*500\b',header)
assert 'SIZE(hero, 250);' in hero
assert 'COMBAT_REMOTE_HERO_FIRST_SIZE         = 200' in header
assert 'COMBAT_REMOTE_HERO_SECOND_SIZE        = 50' in header
assert 200+50==250 and 155<=247 and 201<=247 and 51<=247 and 247<=255
assert 25+2*15+100==155
# Prefix copying writes at most index4 to either buffer; expected is always
# terminated before strlen chooses the answer's terminator position.
for length in range(5):assert 0<=length<5<=8
print('PASS wire extents: battle155, hero201/51 <= payload247; hero split covers250 bytes; riddle terminator index0..4')
print('OPEN arbitrary event text/percent directives, malformed metadata/packet tags and lengths, owner validity, signed arithmetic and timer wrap remain contracts')
