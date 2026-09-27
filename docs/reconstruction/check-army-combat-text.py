"""B73: fixed retail creature-name tables and scoped army-message capacity.

Checks all66 singular/plural entries against the source literals and retail
pointer destinations. This proves default-catalog bounds, not validity of
arbitrary creature IDs or replacement/modded strings.
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
for path,constants in (
    ('include/SOURCE/ARMY.h',{'ARMY_QUANTITY_TEXT_SIZE':12,'ARMY_COMBAT_TEXT_SIZE':800}),
    ('src/SOURCE/ARMY.cpp',{'TARGET_NAME_SIZE':100}),
    ('include/SOURCE/X_GLOBAL.h',{'GLOBAL_TEXT_BUFFER_SIZE':768}),
    ('include/SOURCE/SPELLS.h',{'SPELL_SAMPLE_NAME_CAPACITY':16}),
    ('include/SOURCE/KB_TYPES.h',{'MONSTER_SPRITE_NAME_SIZE':5,'SPELL_INFO_SOUND_NAME_SIZE':9,'CREATURE_COUNT':66,'SPELL_COUNT':65}),
):
    declaration=Path(path).read_text()
    for name,value in constants.items():
        assert re.search(r'\b'+name+r'\s*=\s*'+str(value)+r'\b',declaration),(path,name)
maxima={}
for name,va in (('gArmyNames',0x4fddc8),('gArmyNamesPlural',0x4fded0)):
    start=kb.index(' '+name+'[IDX(CREATURE_COUNT)] = {')
    block=kb[start:kb.index('\n};',start)]
    block=re.sub(r'/\*.*?\*/','',block,flags=re.S)
    words=[ast.literal_eval('"'+x+'"').encode('latin1') for x in re.findall(r'"((?:\\.|[^"\\])*)"',block)]
    assert len(words)==66
    for index,word in enumerate(words):
        destination=u32(va-base+index*4)
        assert b'\0' not in word and read(destination-base,len(word)+1)==word+b'\0',(name,index)
    maxima[name]=max(map(len,words))
    print('PASS',name,'66 retail destinations/payloads; maximum bytes excluding NUL',maxima[name])
assert maxima['gArmyNames']+1<=100
sprite_lengths=[]
for index in range(66):
    # Packed26-byte monster record: spriteName is the five bytes at+17.
    prefix=read(0xfa460+26*index+17,5)
    assert b'\0' in prefix,index
    sprite_lengths.append(prefix.index(0))
print('PASS all66 spriteName fields terminated; maximum prefix length',max(sprite_lengths))
assert max(sprite_lengths)+len('move.82M')+1<=768

# Scope: source-reviewed sprintf calls in SpecialAttack, DoHydraAttack,
# DoAttack and CheckLuck. All %s arguments are names above, the case-folded
# singular scratch, or fixed literals. CP1251 case folding preserves length.
# Bound every %s by100 bytes (more than the entire fixed-literal maximum),
# every %d by11 (INT_MIN), and count literal format characters separately.
army=Path('src/SOURCE/ARMY.cpp').read_text()
start=army.index('void army::SpecialAttack(')
end=army.index('void army::DamageEnemy(',start)
region=army[start:end]
literal_pattern=r'"(?:\\.|[^"\\])*"'
groups=re.findall(r'(?:'+literal_pattern+r'\s*)+',region)
literals=[b''.join(ast.literal_eval(x).encode('latin1')
                  for x in re.findall(literal_pattern,group)) for group in groups]
assert max(map(len,literals))<=100
assert max(maxima.values())<=100
formats=sorted({s for s in literals if b'%' in s})
largest=0
for fmt in formats:
    assert not re.search(rb'%(?![sd])',fmt),fmt
    literal=re.sub(rb'%[sd]',b'',fmt)
    bound=len(literal)+100*fmt.count(b'%s')+11*fmt.count(b'%d')+1
    assert bound<=768 and bound<=800,(fmt,bound)
    largest=max(largest,bound)
    print('PASS format',fmt,'conservative bytes including NUL',bound)
print('PASS targetName100, gText768 and snapshots800; conservative formatted maximum',largest)
print('PASS countText12: signed i32 minimum has11 characters plus NUL')

# B74: the only external animation-state writer, CastSpell, uses a16-byte
# sound filename. All combat spell IDs0..47 have a nonempty <=8-byte prefix;
# creature spell IDs101/102 are explicitly mapped to PARALYZE/DISPEL first.
sounds=[read(0xfb3b8+22*i,9) for i in range(65)]
assert all(b'\0' in name for name in sounds)
assert all(name[0]!=0 for name in sounds[:48])
assert all(name[0]==0 for name in sounds[48:])
maximum=max(name.index(0) for name in sounds)
assert maximum+len('.82M')+1<=16
print('PASS combat spell sound names0..47 initialized, maximum filename',maximum+5,'/16')
print('NOTE adventure spell IDs48..64 have empty sound names; CastSpell requires combat-domain input')
