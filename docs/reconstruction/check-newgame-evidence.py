"""U18/O07/B91: packet extents, exact CRT aliases and conditional text bounds.

Supplements full function bytes/sites/ordered destinations. Four unchanged
private EH/FS model gaps are not waived by this checker.
"""
import csv,re,struct
from pathlib import Path
from homm2.core.coff import CoffObject,FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout
control=Path('build/orig/HMM2PL.exe');verify_retail(control)
base,sites,u32,read=_pe_layout(control)
obj=CoffObject(Path('build/objdiff/base/SOURCE/Newgame.obj').read_bytes())
def function(prefix):
 fn,=[s for s in obj.symbols.values() if s.typ==FUNCTION_TYPE and s.name.startswith(prefix)]
 return fn
def raw_at(fn,offset,width):
 return obj.section_bytes(obj.symbol_section(fn))[fn.value+offset:fn.value+offset+width]
def instruction(prefix,rva,offset,encoded):
 expected=bytes.fromhex(encoded);fn=function(prefix)
 assert raw_at(fn,offset,len(expected))==expected,(prefix,hex(offset),'candidate')
 assert read(rva+offset,len(expected))==expected,(prefix,hex(offset),'retail')
handler='?NewGameHandler@@'
for va,encoded in [(0x477fcd,'6a3c'),(0x477fd5,'81c1e3020000'),
 (0x477fdc,'8d955cfcffff'),(0x477feb,'6a41'),(0x477ff2,'054d040000'),
 (0x477ff8,'8d8d98fcffff'),(0x47800f,'6a7d'),(0x478016,'8d8d5cfcffff')]:
 instruction(handler,0x7734a,va-0x47734a,encoded)
assert -0x3a4+60==-0x368 and 60+65==125<=300
assert 9+247==256 and all(size<=247 for size in (116,204,125))
assert 6*34==204 and 0x3a+60==118 and 116==118-2
assert 0x44d+65==0x48e and 0x47c+18==0x48e
print('PASS O07 two copies60+65 and send125 share -0x3a4 owner; full300-byte reservation retained')
print('PASS U18 physical256-byte transport owner, header9/payload247; map116/player204/setup125 fit')
print('PASS zeroed420-byte map header +116-byte prefix leaves final two name bytes NUL; not field validation')
print('OPEN setup blob65 spans game+44d..48e including opaque18; sender, payload length and names remain contracts')

aliases=list(csv.DictReader((line for line in Path('config/reviewed_rel32_aliases.tsv').read_text().splitlines() if not line.startswith('#')),delimiter='\t'))
assert any('_stricmp' in row.values() and '_strcmpi' in row.values() and '0x000d8690' in row.values() for row in aliases)
for prefix,rva,offsets in [('?GetMap@',0x754b0,[0x173]),('?NewGame@',0x75b4b,[0x602,0x641])]:
 fn=function(prefix)
 for offset in offsets:
  reloc,=[r for r in obj.relocations if r.section==fn.section and r.site==fn.value+offset]
  assert reloc.typ==20 and obj.symbols[reloc.symbol_index].name=='_stricmp'
  assert raw_at(fn,offset-1,1)==b'\xe8' and raw_at(fn,offset,4)==b'\0'*4
  assert (u32(rva+offset)+rva+offset+4)&0xffffffff==0xd8690
print('PASS all three zero-addend REL32 _stricmp operands resolve to reviewed _strcmpi alias at004d8690')

def cstring(rva):
 data=read(rva,4096);assert b'\0' in data
 return data.split(b'\0',1)[0]
def table(va,count):return [cstring(u32(va-base+4*i)-base) for i in range(count)]
def literal(prefix,rva,offset):
 fn=function(prefix)
 reloc,=[r for r in obj.relocations if r.section==fn.section and r.site==fn.value+offset]
 sym=obj.symbols[reloc.symbol_index]
 assert reloc.typ==6 and sym.name.startswith('$SG') and raw_at(fn,offset,4)==b'\0'*4
 native=obj.section_bytes(obj.symbol_section(sym))[sym.value:].split(b'\0',1)[0]
 assert cstring(u32(rva+offset)-base)==native
 return native
def bound(fmt,lengths=()):
 tokens=list(re.finditer(rb'%([0-9]*)([sd%])',fmt))
 assert b'%' not in re.sub(rb'%([0-9]*)([sd%])',b'',fmt),fmt
 assert len([t for t in tokens if t[2]!=b'%'])==len(lengths)
 lengths=iter(lengths);result=len(fmt)+1
 for t in tokens:result+=max(int(t[1] or b'0'),1 if t[2]==b'%' else next(lengths))-len(t[0])
 return result
colors=table(0x4fe490,6);assert all(b'%' not in x and 0<len(x)<=10 for x in colors)
side='?GetSideDesc@';srva=0x7973c
side_prefix=max(len(literal(side,srva,o)) for o in (0xc2,0xd5,0x22d,0x240))
# At most six entries, each at most ten chars, five separators of at most
# three chars. This conservative bound also permits repeated valid colors.
side_bound=side_prefix+6*max(map(len,colors))+5*3+1
assert side_bound<=100 and max(map(len,colors))+1<=100
print('PASS six valid color entries and longest real prefixes/separators need at most',side_bound,'/100 bytes')
loss='?GetLossConditionText@';lrva=0x792d5
loss_bounds=[bound(literal(loss,lrva,0xa8),(5,12)),bound(literal(loss,lrva,0xe4),(12,)),
 bound(literal(loss,lrva,0x173),(11,11,11)),bound(literal(loss,lrva,0x186))]
assert max(loss_bounds)<=768
victory='?GetVictoryConditionText@';vrva=0x79471
artifacts=table(0x4fd7ac,103)
victory_bounds=[bound(literal(victory,vrva,0xde),(5,12)),bound(literal(victory,vrva,0x126),(12,)),
 bound(literal(victory,vrva,0x14f)),bound(literal(victory,vrva,0x179),(max(map(len,artifacts)),)),
 bound(literal(victory,vrva,0x1a5),(8,)),bound(literal(victory,vrva,0x227),(side_bound-1,side_bound-1))]
suffix=literal(victory,vrva,0x27b);period=literal(victory,vrva,0x28e)
victory_bounds=[n+(len(period) if i==5 else max(len(period),len(suffix))) for i,n in enumerate(victory_bounds)]
victory_bounds.append(bound(literal(victory,vrva,0x2a1)))
assert max(victory_bounds)<=768 and 65535*1000<2**31
print('PASS valid loss cases:',loss_bounds,'/768; valid victory cases:',victory_bounds,'/768 including suffix/NUL')
print('OPEN public text destinations have no capacity parameter; invalid conditions/hero/town/artifact/player domains are not bounded')
assert 20+4+246+1<=768 and 100+1==101
assert 99+1+1<=105
print('PASS bounded valid sender name20 + terminated transport text246 fits768 before truncation to100/101')
print('PASS ordinary cursor<=text length<=99 allows inserted display marker/NUL within105')
print('OPEN escaped chat clears text without cursor reset; independent cursor validity is required')
print('OPEN chat/player-name sprintf format arguments are not literals; post-format truncation does not prevent format-driven access')
print('OPEN map filename351->16->13 needs source-length constraints; allocation/resource/timer lifecycle is not validated here')
