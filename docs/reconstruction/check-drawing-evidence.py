"""B90: actual local tables, fixed catalog text and conditional scan bounds.

Supplements full per-function bytes/sites/ordered targets. UpdateMouseGrid's
unchanged private EH/FS model gap is not waived by this checker.
"""
import re,struct
from pathlib import Path
from homm2.core.coff import CoffObject,FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout
control=Path('build/orig/HMM2PL.exe');verify_retail(control)
base,sites,u32,read=_pe_layout(control)
obj=CoffObject(Path('build/objdiff/base/SOURCE/DRAWING.obj').read_bytes())
def instructions(name,rva,offset,expected):
 fn,=[s for s in obj.symbols.values() if s.typ==FUNCTION_TYPE and s.name==name]
 raw=obj.section_bytes(obj.symbol_section(fn))
 assert raw[fn.value+offset:fn.value+offset+len(expected)]==expected,(name,hex(offset))
 assert read(rva+offset,len(expected))==expected,(name,hex(offset),'retail')
def stack_store(disp,value,width):
 prefix=b'\x66\xc7' if width==2 else b'\xc6'
 addressing=b'\x45'+struct.pack('<b',disp) if -128<=disp<=127 else b'\x85'+struct.pack('<i',disp)
 return prefix+addressing+value.to_bytes(width,'little')
wall=[443,153,399,237,399,321,443,405]*6
encoded=b''.join(stack_store(-156+2*i,v,2) for i,v in enumerate(wall))
instructions('?DrawFrame@combatManager@@QAEXHHHHHHH@Z',0x395b8,0xa4f,encoded)
offsets=[0,4,8,23,27,35,31]
encoded=b''.join(stack_store(-164+i,v,1) for i,v in enumerate(offsets))
instructions('?DrawFrame@combatManager@@QAEXHHHHHHH@Z',0x395b8,0xb99,encoded)
print('PASS DrawFrame actual 6x8 word coordinates and seven byte frame offsets, all55 initialization stores')

positions=[[(22,10)],[(11,10),(32,10)],[(1,10),(22,10),(43,10)],
           [(11,0),(32,0),(11,21),(32,21)],[(1,0),(22,0),(43,0),(11,21),(32,21)],
           [(1,0),(22,0),(43,0),(1,21),(22,21),(43,21)]]
values=[v for row in positions for pair in row+[(255,255)]*(6-len(row)) for v in pair]
assert len(values)==72
encoded=b''.join(stack_store(-76+i,v,1) for i,v in enumerate(values))
instructions('?DrawSmallView@combatManager@@QAEXHH@Z',0x3aad6,12,encoded)
for count in range(-100,101):
 visible=min(count,6)
 for slot in range(visible):
  assert 0<=visible-1<6 and 0<=slot<6
  assert positions[visible-1][slot]!=(255,255)
print('PASS actual6x6x2 byte spell-position table; loop entry bounds both indices and avoids255 filler')
moat=[8,21,33,46,58,72,85,99,112]
assert read(0x4fd634-base,9)==bytes(moat)
for row in range(9):
 assert all(0<=row*13+col<117 for col in list(range(1,12))+list(range(11,0,-1)))
 assert all(0<=moat[row]+delta<117 for delta in (-1,0,1))
print('PASS all99 normal row cells, reversed traversal and nine moat neighbor triples within117')

# For every possible 15-bit occupancy set, a consistent count guarantees the
# unguarded scan finds each of its at most six requested nonzero entries.
for mask in range(1<<15):
 active=[bool(mask&(1<<i)) for i in range(15)]
 visible=min(sum(active),6);icon=-1
 for slot in range(visible):
  icon+=1
  while not active[icon]:icon+=1
  assert icon<15
print('PASS all32768 spell occupancy masks under count==number-of-nonzero-entries contract')
print('OPEN SetSpellInfluence stores u8 rounds; zero/truncated-to-zero duration can invalidate that count contract')

def cstring(rva):
 data=read(rva,4096);assert b'\0' in data
 return data.split(b'\0',1)[0]
def table(va,count):return [cstring(u32(va-base+4*i)-base) for i in range(count)]
def bound(fmt,args=()):
 tokens=list(re.finditer(rb'%([0-9]*)([sdc%])',fmt))
 assert b'%' not in re.sub(rb'%([0-9]*)([sdc%])',b'',fmt),fmt
 assert [m[2].decode() for m in tokens if m[2]!=b'%']==[a[0] for a in args]
 result=len(fmt)+1;remaining=iter(args)
 for token in tokens:
  length=1 if token[2]==b'%' else next(remaining)[1]
  result+=max(int(token[1] or b'0'),length)-len(token[0])
 return result
names=table(0x4fddc8,66);plural=table(0x4fded0,66);messages=table(0x4fe734,12)
args={1:[('s',max(map(len,names)))],2:[('s',max(map(len,names)))],
      3:[('s',max(map(len,plural)))],4:[('s',max(map(len,plural))),('d',4)],
      7:[('s',max(map(len,names)))]}
message_bounds=[bound(fmt,args.get(i,())) for i,fmt in enumerate(messages)]
assert max(message_bounds)<=120
mini=table(0x4ff0d8,9)
mini_bounds=[bound(mini[i],[('d',11)]) for i in (0,1)]
assert max(mini_bounds)<768
assert max(bound(b'%d',[('d',4)]),bound(b'%d',[('d',5)]),bound(b'%d-%d',[('d',4),('d',4)]))<768
print('PASS default command-message catalog including NUL:',message_bounds,'max',max(message_bounds),'/120')
for name,va,count in [('view help',0x4fe708,7),('combat help',0x4fe770,5),('spell help',0x4fe630,9)]:
 help_text=table(va,count)
 assert all(len(text)<120 for text in help_text)
 print('PASS fixed',name,'catalog',count,'entries max',max(map(len,help_text))+1,'/120 including NUL')
print('PASS small-view signed i32 quantity:',mini_bounds,'/768; promoted i8 stats and u16 hitpoints fit')
assert bound(b'NC: %s',[('s',399)])==404<=700
assert max(bound(b'covr%04d.icn',[('d',11)]),bound(b'frng%04d.icn',[('d',11)]))<=20
assert bound(b'castbkg%c.icn',[('c',1)])==13
print('PASS normal400-byte message producer needs at most404/700 log bytes; background names need20/768 or13/768')

# These are independent owner extents, not a400-byte guarantee for120-byte lines.
header=Path('include/SOURCE/combatManager.h').read_text()
for name,value in [('COMBAT_MESSAGE_LINE_SIZE',120),('COMBAT_MESSAGE_WRAP_BUFFER_SIZE',400),
                   ('COMBAT_MESSAGE_LOG_BUFFER_SIZE',700),('COMBAT_HEX_COUNT',117)]:
 assert re.search(r'\b'+name+r'\s*=\s*'+str(value)+r'\b',header)
assert 399+2>400 and 398+2==400
print('OPEN general message input<=399, period/newline join needs<=398; each copied manager line needs<=119 chars')
print('OPEN configuration/resource/frame/army/player/lifecycle and walking-coordinate contracts; no blanket arbitrary-input safety')
