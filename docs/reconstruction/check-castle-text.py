"""B84: Castle formatter capacities and finite building-slot domain.

Full per-function byte/target checks are separate; SetupCastle's existing
private EH/site-model gap is not waived by these text/data assertions.
"""
import re
from pathlib import Path
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control = Path('build/orig/HMM2PL.exe')
verify_retail(control)
base, sites, u32, read = _pe_layout(control)
def cstring(rva):
    raw=read(rva,4096)
    assert b'\0' in raw
    return raw.split(b'\0',1)[0]
def table(va,count):
    return [cstring(u32(va-base+4*i)-base) for i in range(count)]
def bound(fmt,args=()):
    tokens=list(re.finditer(rb'%([0-9]*)([sd%])',fmt))
    assert b'%' not in re.sub(rb'%([0-9]*)([sd%])',b'',fmt),fmt
    assert [m[2].decode() for m in tokens if m[2]!=b'%']==[a[0] for a in args]
    length=len(fmt)+1; remaining=iter(args)
    for token in tokens:
        width=1 if token[2]==b'%' else next(remaining)[1]
        length+=max(int(token[1] or b'0'),width)-len(token[0])
    return length

source=Path('src/SOURCE/Castle.cpp').read_text()
assert re.search(r'CAPTAIN_STAT_LINE_CAPACITY\s*=\s*200\b',source)
assert re.search(r'TOWN_OBJECT_FILENAME_SIZE\s*=\s*16\b',Path('include/SOURCE/TOWNMGR.h').read_text())
assert re.search(r'GLOBAL_TEXT_BUFFER_SIZE\s*=\s*768\b',Path('include/SOURCE/X_GLOBAL.h').read_text())
stats=table(0x4fdc80,4)
short=table(0x4fcd7c,6)
castle=table(0x4fe588,16)
names=(table(0x4fed4c,19)+table(0x4fed98,7)+table(0x4fedb4,7)
       +table(0x4fedd0,6*12)+table(0x51b7b8,1))
assert all(b'%' not in value for value in stats+short+names)
name_length=max(map(len,names))
assert bound(b'cstl%s.icn',[('s',max(map(len,short)))])==13
# Portrait is u8 and its minimum field width is4; %c consumes one byte.
assert bound(b'port%04d.icn',[('d',3)])==13
assert len(b'CSTLCAPx.ICN')+1==13
stat_scratch=max(max(len(value)+2 for value in stats),bound(b'%d\n',[('d',11)]))
names_total=1+sum(len(value)+1 for value in stats)
values_total=1+4*(bound(b'%d\n',[('d',11)])-1)
assert stat_scratch<=200 and max(names_total,values_total)<=768
print('PASS icon filenames13/16, captain scratch200 / names768 / numeric768:',stat_scratch,names_total,values_total)

bounds=[]
for index,fmt in enumerate(castle):
    args=[('s',name_length)] if index in (4,5,6,7) else [('d',11)] if index==9 else []
    bounds.append(bound(fmt,args))
assert max(bounds)<=768
assert b'%' not in castle[11]
print('PASS all16 castle text forms; longest building name',name_length,'bytes; bounds including NUL',bounds)
print('NOTE recruit format11 has no directives in this retail translation; evaluated hero/name/alignment arguments still need valid owners')

slots=list(read(0x4f0a78-base,18))
assert slots==[19,20,21,22,23,24,0,2,1,3,7,10,4,11,13,8,9,12]
# Enumerate all outcomes the SetupCastle branch can select from this table.
possible=set(slots)
for slot in slots:
    if 20<=slot<=24: possible.add(slot+5)
possible.add(30)
assert all(0<=slot<=30 for slot in possible)
assert all(slot<19 or 0<=slot-19<12 for slot in possible)
print('PASS all initialized base/upgraded/warlock slot outcomes: shifts0..30 and real neutral19/dwelling12 owners')
print('OPEN live faction/hero/terrain/world-map/widget/townObject count domains and initialized/unmodified slot arrays')
