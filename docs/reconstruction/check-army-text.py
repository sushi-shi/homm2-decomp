"""B60: complete ViewArmy text bounds from pinned retail string operands.

The bound is conditional on valid creature/speed/morale/luck indices and live
owners. It covers text capacities, not icons, packet data, or dialog lifetime.
"""
import re
from pathlib import Path
from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

retail=Path('build/orig/HMM2PL.exe')
verify_retail(retail)
base,_,u32,read=_pe_layout(retail)
source=Path('src/SOURCE/GAME.cpp').read_text()
for name,value in [('VIEW_ARMY_FILENAME_SIZE',16),('VIEW_ARMY_NAME_SIZE',32),
                   ('VIEW_ARMY_DETAIL_BUFFER_SIZE',550),('VIEW_ARMY_COUNT_TEXT_SIZE',12)]:
    assert re.search(r'\b'+name+r'\s*=\s*'+str(value)+r'\b',source),name
assert re.search(r'GLOBAL_TEXT_BUFFER_SIZE\s*=\s*768',Path('include/SOURCE/X_GLOBAL.h').read_text())

def string(rva):
    data=read(rva,4096)
    assert b'\0' in data
    return data.split(b'\0',1)[0]

def table(label,va,count):
    result=[string(u32(va-base+4*i)-base) for i in range(count)]
    print(label,'lengths without NUL',list(map(len,result)))
    return result

names=table('creature names',0x4fddc8,66)
files=table('creature icon filenames',0x4fb2ac,66)
speed=table('speed text',0x4fe654,10)
morale=table('morale text',0x4fe434,7)
luck=table('luck text',0x4fe418,7)
detail=table('detail labels',0x4fe67c,9)

# Establish that each private format/literal in the current body points at the
# same bytes in retail, without relaxing other relocation-owner requirements.
obj=CoffObject(Path('build/objdiff/base/SOURCE/GAME.obj').read_bytes())
fn,=[s for s in obj.symbols.values() if s.typ==FUNCTION_TYPE and s.name.startswith('?ViewArmy@game@@')]
raw=obj.section_bytes(obj.symbol_section(fn)); literals=set()
for reloc in obj.relocations:
    if reloc.section!=fn.section or not fn.value<=reloc.site<fn.value+3336: continue
    owner=obj.symbols[reloc.symbol_index]
    if not owner.name.startswith('$SG'): continue
    assert reloc.typ==6 and raw[reloc.site:reloc.site+4]==bytes(4)
    value=obj.section_bytes(obj.symbol_section(owner))[owner.value:].split(b'\0',1)[0]
    assert string(u32(0x55529+reloc.site-fn.value)-base)==value
    literals.add(value)
for fmt in [b'',b'%s%d',b' (%d)',b'\n%s%d',b'-%d',b'\n%s%s',b'%d']:
    assert fmt in literals,fmt
remaining_label='Осталось здоровья: '.encode('cp1251')
assert remaining_label in literals

def formatted(fmt,*widths):
    tokens=list(re.finditer(rb'%[sd]',fmt))
    assert len(tokens)==len(widths) and b'%' not in re.sub(rb'%[sd]',b'',fmt)
    return len(fmt)+1+sum(width-len(token[0]) for token,width in zip(tokens,widths))

# i8 extrema require at most4 characters; u16 hit points require5. Modified
# attack/defense and the raw 32-bit remaining-health printf word get11, even
# when wider than realistic game values. Shots is positive signed i8, <=127.
parts=[
    formatted(b'%s%d',len(detail[0]),4), formatted(b' (%d)',11),
    formatted(b'\n%s%d',len(detail[1]),4), formatted(b' (%d)',11),
    max(formatted(b'\n%s%d',len(detail[i]),3) for i in (2,8)),
    formatted(b'\n%s%d',len(detail[3]),4), formatted(b'-%d',4),
    formatted(b'\n%s%d',len(detail[4]),5),
    formatted(b'\n%s%d',len(remaining_label),11),
    formatted(b'\n%s%s',len(detail[5]),max(map(len,speed))),
    formatted(b'\n%s%s',len(detail[6]),max(map(len,morale))),
    formatted(b'\n%s%s',len(detail[7]),max(map(len,luck))),
]
# Each optional append is included. strcat overwrites the previous terminator.
total=1+sum(part-1 for part in parts)
assert total<=550 and max(parts)<=768
assert max(map(len,names))+1<=32
assert max(map(len,files))+1<=16
# Count is formatted only for numTroops>=1; INT_MAX has10 decimal digits.
assert formatted(b'%d',10)<=12
print('PASS including NUL: detail chain',total,'/550; largest gText part',max(parts),'/768;')
print('name',max(map(len,names))+1,'/32; icon filename',max(map(len,files))+1,'/16; count11/12.')
print('One-byte CP1251 uppercase preserves length; every optional append included.')
