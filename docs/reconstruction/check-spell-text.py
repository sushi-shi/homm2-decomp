"""B75: spell-message capacities for the fixed retail catalogs.

Checks complete string-table destinations and payloads, not arbitrary replacement
catalogs. Hero names must be terminated in their thirteen-byte owner. This does
not establish that unsupported creature spells initialize the message buffer.
"""
import ast
import re
from pathlib import Path
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control=Path('build/orig/HMM2PL.exe')
verify_retail(control)
base,sites,u32,read=_pe_layout(control)
kb=Path('src/SOURCE/KB.cpp').read_text()
literal=r'"(?:\\.|[^"\\])*"'
def strings(text):
    text=re.sub(r'/\*.*?\*/','',text,flags=re.S)
    return [b''.join(ast.literal_eval(token).encode('latin1')
                    for token in re.findall(literal,group))
            for group in re.findall(r'(?:'+literal+r'\s*)+',text)]

maxima={}
for name,va,count in (('gArmyNames',0x4fddc8,66),('gArmyNamesPlural',0x4fded0,66),('gSpellNames',0x4fec04,65)):
    match=re.search(r'DATA\(0x'+format(va,'08x')+r'\)[^\n]*\b'+name+r'\[',kb)
    assert match,name
    start=match.start()
    words=strings(kb[start:kb.index('\n};',start)])
    assert len(words)==count,(name,len(words))
    for index,word in enumerate(words):
        assert b'\0' not in word and b'%' not in word
        destination=u32(va-base+4*index)
        assert read(destination-base,len(word)+1)==word+b'\0',(name,index)
    maxima[name]=max(map(len,words))
    print('PASS',name,count,'retail pointer/payload pairs; maximum',maxima[name])

source=Path('src/SOURCE/SPELLS.cpp').read_text()
for path,constants in (
    ('src/SOURCE/SPELLS.cpp',{'TARGET_NAME_CAPACITY':60,'MESSAGE_CAPACITY':400}),
    ('include/SOURCE/X_GLOBAL.h',{'GLOBAL_TEXT_BUFFER_SIZE':768}),
    ('include/SOURCE/hero.h',{'HERO_NAME_SIZE':13}),
    ('include/SOURCE/KB_TYPES.h',{'CREATURE_COUNT':66,'SPELL_COUNT':65}),
):
    text=Path(path).read_text()
    for name,value in constants.items():
        assert re.search(r'\b'+name+r'\s*=\s*'+str(value)+r'\b',text),(path,name)
assert max(maxima.values())<60 and 12<60
assert maxima['gArmyNamesPlural']+1<=60

# All reviewed non-CastSpell formatter arguments are fixed names, a terminated
# hero name, the sixty-byte targetName scratch, or short literal prefixes.
# Bound each %s by sixty bytes and each signed %d by eleven; this deliberately
# exceeds every proven payload length. CastSpell's sound filename is handled
# by check-army-combat-text.py and is excluded from this larger-owner census.
start=source.index('void combatManager::CastSpell(')
end=source.index('\nVA(',start)
region=source[:start]+source[end:]
formats=sorted({word for word in strings(region) if b'%' in word})
largest=0
for fmt in formats:
    assert not re.search(rb'%(?![sd])',fmt),fmt
    bound=len(re.sub(rb'%[sd]',b'',fmt))+60*fmt.count(b'%s')+11*fmt.count(b'%d')+1
    assert bound<=400 and bound<=768,(fmt,bound)
    largest=max(largest,bound)
    print('PASS format',repr(fmt),'conservative capacity including NUL',bound)
print('PASS',len(formats),'formats; largest conservative message',largest,'/400; gText /768')
print('PASS plural catalog is a safe literal format and fits targetName /60')
print('CONTRACT supported creature spell, initialized target, valid indices and terminated hero name remain required')
