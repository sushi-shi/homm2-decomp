"""C62/C63/B78/B79: fixed numeric operands, combat owners and catalog bounds.

This supplements, not replaces, check-function.py and native comparisons.
ShootMissile's EH/FS operands remain outside independent whole-retail closure.
"""
import ast
import re
import struct
from pathlib import Path
from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control=Path('build/orig/HMM2PL.exe'); verify_retail(control)
base,sites,u32,read=_pe_layout(control)
obj=CoffObject(Path('build/objdiff/base/SOURCE/CMBTMGR.obj').read_bytes())
cases=[
 ('CatAttack',0x28bb8,3544,{
     0x71a:(0x4ea568,'d',0.3),0x725:(0x4ea560,'d',0.35),
     0x736:(0x4ea558,'d',12.5),0x745:(0x4ea554,'f',78),0x784:(0x4ea554,'f',78),
     0x7a6:(0x4ea550,'f',15),0x7b9:(0x4ea550,'f',15),0x7cc:(0x4ea550,'f',15),
     0xad5:(0x4ea54c,'f',75)}),
 ('ShotIsThroughWall',0x2aabf,667,{
     0x183:(0x4ea570,'f',10),0x18f:(0x4ea570,'f',10)}),
 ('ShootMissile',0x2ad5a,1248,{
     0x9f:(0x4ea4f0,'d',180),0xa5:(0x4ea4e8,'d',3.14159),
     0xd9:(0x4ea4e4,'f',2),0x407:(0x4ea4e0,'f',25)}),
]
for name,rva,size,expected in cases:
    fn,=[s for s in obj.symbols.values() if s.typ==FUNCTION_TYPE and s.name.startswith('?'+name+'@combatManager@@')]
    raw=obj.section_bytes(obj.symbol_section(fn)); seen=set()
    for reloc in obj.relocations:
        if reloc.section!=fn.section or not fn.value<=reloc.site<fn.value+size: continue
        owner=obj.symbols[reloc.symbol_index]
        if not owner.name.startswith('__real@'): continue
        offset=reloc.site-fn.value; destination,fmt,value=expected[offset];seen.add(offset)
        encoded=struct.pack('<'+fmt,value)
        assert reloc.typ==6 and raw[reloc.site:reloc.site+4]==bytes(4)
        assert rva+offset in sites and u32(rva+offset)==destination
        data=obj.section_bytes(obj.symbol_section(owner))[owner.value:owner.value+len(encoded)]
        assert data==encoded==read(destination-base,len(encoded)),(name,hex(offset))
        print('PASS fixed operand',name,hex(offset),hex(destination),fmt,value)
    assert seen==set(expected)

kb=Path('src/SOURCE/KB.cpp').read_text()
xglobal=Path('include/SOURCE/X_GLOBAL.h').read_text()
header=Path('include/SOURCE/combatManager.h').read_text()
assert 'SIZE(SCmbtObstacle, 0xe)' in xglobal
assert 'SIZE(SElevationOverlay, 0x11)' in xglobal
for name,value in [('KB_COMBAT_OBSTACLE_COUNT',32),('KB_COMBAT_BACKGROUND_COUNT',19)]:
    assert re.search(r'\b'+name+r'\s*=\s*'+str(value)+r'\b',xglobal)
for name,value in [('COMBAT_HEX_COUNT',117),('COMBAT_OBSTACLE_TYPE_COUNT',32),
                   ('COMBAT_OBSTACLE_INCLUSIVE_ROLL_HIGH',32),('COMBAT_OBSTACLE_CELL_ROLL_MAX',116),
                   ('COMBAT_BACKGROUND_NAME_SIZE',13),('COMBAT_OBSTACLE_ICON_LOAD_COUNT',8)]:
    assert re.search(r'\b'+name+r'\s*=\s*'+str(value)+r'\b',header)

start=kb.index('struct SCmbtObstacle sCmbtObstacles[')
block=kb[start:kb.index('\n};',start)]
entries=re.findall(r'\{(0x[0-9a-f]+),\s*(\d+),\s*(\d+),\s*\{([^}]+)\}\}',block)
assert len(entries)==32
obstacles=[];encoded=b''
for mask,column,count,offsets in entries:
    record=(int(mask,16),int(column),int(count),[int(v.strip()) for v in offsets.split(',')])
    mask,column,count,offsets=record
    assert count in (1,2) and column in (1,2,3)
    assert offsets[:count]==list(range(count)) and len(offsets)==8
    obstacles.append(record)
    encoded+=struct.pack('<IBB8b',mask,column,count,*offsets)
assert len(encoded)==448 and read(0xfbaf0,448)==encoded
dataobj=CoffObject(Path('build/objdiff/base/SOURCE/KB.obj').read_bytes())
symbol,=[s for s in dataobj.symbols.values() if s.name=='?sCmbtObstacles@@3PAUSCmbtObstacle@@A']
assert dataobj.section_bytes(dataobj.symbol_section(symbol))[symbol.value:symbol.value+448]==encoded
assert 'DATA(0x004fbcb0) i32 gEstatesGoldLevel' in kb
assert read(0xfbaf0+448,4)==bytes(4)
print('PASS32 actual14-byte obstacle records; index32 terrainMask reads adjacent gEstatesGoldLevel[0]=0')

# Fixed retail instructions: high=32, stride14, terrainMask load and short
# circuit before the stack array. The complete function check proves the rest.
rva=0x2a1c4
for offset,expected in [(0x298,'ba20000000'),(0x2aa,'6bc00e'),
                        (0x2b8,'740d'),(0x2bf,'8a4415b8'),
                        (0x359,'c745d801000000'),(0x36b,'8a8c1022050000')]:
    assert read(rva+offset,len(bytes.fromhex(expected)))==bytes.fromhex(expected),hex(offset)
assert rva+0x2af in sites and u32(rva+0x2af)==0x4fbaf0
assert read(rva+0x29f,1)==b'\xe8'
assert rva+0x2a4+struct.unpack('<i',read(rva+0x2a0,4))[0]==0xc03f0
assert 0x522+117*0x62==0x31ec
assert all(0<=site+offset<=117 for site in range(117) for _,_,count,offsets in obstacles for offset in offsets[:count])
two_cells=next(i for i,(mask,_,count,_) in enumerate(obstacles) if mask and count==2)
assert obstacles[two_cells][1]<=116//13+1
assert 117%13<=2
print('PASS site116 plus second cell reaches hex117; blocked flag is set before the unconditional read at this+0x31ec')
print('NOTE that read exceeds m_hexCells but lies inside combatManager; no array enlargement or gameplay fix is applied')

# Every default obstacle contributes at least one occupied cell. Goal<=7,
# elevation only decreases it, and successful placement increases placedCells.
# Thus at most seven icon pointers are appended after normal zeroing of count.
assert min(count for mask,column,count,offsets in obstacles)==1
assert max(count for mask,column,count,offsets in obstacles)==2
print('PASS normal obstacle append count<=7, inside eight icon slots; replacement data is outside this proof')

# Actual constructor reservation, not inferred padding: 117 hexcell objects,
# 42 army objects and their98/1154-byte strides are emitted in retail.
for offset,expected in [(0x11,'c745f475000000'),(0x18,'c745f862000000'),
                        (0x5a,'c745e82a000000'),(0x61,'c745ec82040000')]:
    assert read(0x25de0+offset,len(bytes.fromhex(expected)))==bytes.fromhex(expected)
assert read(0x271ad+0x7c,6)==bytes.fromhex('69c9aa5e0000')
assert 21*0x482==0x5eaa
print('PASS constructor117 hexcell and42 army objects; two21-slot rows are real storage, not removable padding')

# C63: all8 bytes, both native symbols, both declarations and exact typed
# initializers. Existing table names/order are preserved even though the
# wall table contains enum values named tower and vice versa.
types=Path('include/SOURCE/combatTypes.h').read_text()
assert 'H2_ENUM_CLASS_BEGIN(CombatCastleHex)' in types
assert 'H2_ENUM_CLASS_BEGIN(CombatCastleHex)' not in header
for name,va,size_name,values in [
    ('iWallToHexCell',0x4fd404,'KB_CASTLE_WALL_SEGMENT_COUNT',[
        ('COMBAT_CASTLE_HEX_TOP_TOWER',9),('COMBAT_CASTLE_HEX_SECOND_TOWER',34),
        ('COMBAT_CASTLE_HEX_THIRD_TOWER',86),('COMBAT_CASTLE_HEX_BOTTOM_TOWER',113)]),
    ('iTowerToHexCell',0x4fd408,'KB_CASTLE_TOWER_COUNT',[
        ('COMBAT_CASTLE_HEX_TOP_WALL',22),('COMBAT_CASTLE_HEX_SECOND_WALL',47),
        ('COMBAT_CASTLE_HEX_THIRD_WALL',73),('COMBAT_CASTLE_HEX_BOTTOM_WALL',100)]),
]:
    declaration='H2_ENUM_STORAGE(CombatCastleHex, u8) '+name+'['+size_name+']'
    assert declaration in kb and declaration in xglobal
    start=kb.index(declaration);block=kb[start:kb.index('};',start)]
    assert re.findall(r'\bCOMBAT_CASTLE_HEX_[A-Z_]+\b',block)==[n for n,v in values]
    for n,v in values:
        assert re.search(r'\b'+n+r'\s*=\s*'+str(v)+r'\b',types)
    encoded=bytes(v for n,v in values)
    symbol,=[s for s in dataobj.symbols.values() if s.name=='?'+name+'@@3PAEA']
    assert dataobj.section_bytes(dataobj.symbol_section(symbol))[symbol.value:symbol.value+4]==encoded
    assert read(va-base,4)==encoded
spellobj=CoffObject(Path('build/objdiff/base/SOURCE/SPELLS.obj').read_bytes())
consumers={0x28bb8:(obj,'CatAttack'),0x2aabf:(obj,'ShotIsThroughWall'),
           0xa0cb9:(spellobj,'Earthquake')}
for rva,offset,va in [(0x28bb8,0x367,0x4fd404),(0x28bb8,0x39e,0x4fd408),
                       (0x28bb8,0xce9,0x4fd404),(0x2aabf,0x20f,0x4fd404),
                       (0x2aabf,0x24c,0x4fd408),(0xa0cb9,0x55f,0x4fd404)]:
    assert rva+offset in sites and u32(rva+offset)==va,(hex(rva),hex(offset))
    consumer,name=consumers[rva]
    fn,=[s for s in consumer.symbols.values() if s.typ==FUNCTION_TYPE and s.name.startswith('?'+name+'@combatManager@@')]
    reloc,=[r for r in consumer.relocations if r.section==fn.section and r.site==fn.value+offset]
    expected='?'+('iWallToHexCell' if va==0x4fd404 else 'iTowerToHexCell')+'@@3PAEA'
    assert reloc.typ==6 and consumer.symbols[reloc.symbol_index].name==expected
    raw=consumer.section_bytes(consumer.symbol_section(fn))
    assert raw[reloc.site:reloc.site+4]==bytes(4)

# Fixed instructions for the second measured typed-byte-enum pattern.
fn,=[s for s in obj.symbols.values() if s.typ==FUNCTION_TYPE and s.name.startswith('?CatAttack@combatManager@@')]
raw=obj.section_bytes(obj.symbol_section(fn))
for offset,encoded in [(0x363,'33d2'),(0x36b,'899578ffffff'),
                       (0x39a,'33d2'),(0x3a2,'899578ffffff')]:
    encoded=bytes.fromhex(encoded)
    assert raw[fn.value+offset:fn.value+offset+len(encoded)]==encoded==read(0x28bb8+offset,len(encoded))
for offset,va in [(0x365,0x4fd404),(0x39c,0x4fd408)]:
    assert raw[fn.value+offset:fn.value+offset+6]==bytes.fromhex('8a9100000000')
    assert read(0x28bb8+offset,6)==bytes.fromhex('8a91')+struct.pack('<I',va)
source=Path('src/SOURCE/CMBTMGR.cpp').read_text()
assert 'targetHex4 = iWallToHexCell[wallIndex];' in source
assert 'targetHex4 = iTowerToHexCell[IDX(towerIndex6)];' in source
assert 'CombatCastleHex(iWallToHexCell' not in source and 'CombatCastleHex(iTowerToHexCell' not in source
assert 'm_hexCells[IDX(iWallToHexCell[index7])].m_blocked = 0;' in Path('src/SOURCE/SPELLS.cpp').read_text()
print('PASS C63 both typed four-byte castle-hex owners, all six CMBTMGR/SPELLS consumer operands and both typed assignment sequences')

# Fixed background format catalog: all19 source strings and actual retail
# pointer destinations, including unused empty entries. No percent directives.
start=kb.index(' cCombatBkgNames[');block=kb[start:kb.index('};',start)]
words=[ast.literal_eval('"'+v+'"').encode('latin1') for v in re.findall(r'"((?:\\.|[^"\\])*)"',block)]
assert len(words)==19 and max(map(len,words))+1==13
for i,word in enumerate(words):
    assert b'%' not in word and b'\0' not in word
    assert read(u32(0xfbaa4+4*i)-base,len(word)+1)==word+b'\0'
print('PASS all19 background names: literal sprintf formats fit13 bytes including NUL')

maxima={}
for name,va in [('gArmyNames',0x4fddc8),('gArmyNamesPlural',0x4fded0)]:
    start=kb.index(' '+name+'[IDX(CREATURE_COUNT)] = {')
    block=re.sub(r'/\*.*?\*/','',kb[start:kb.index('\n};',start)],flags=re.S)
    words=[ast.literal_eval('"'+v+'"').encode('latin1') for v in re.findall(r'"((?:\\.|[^"\\])*)"',block)]
    assert len(words)==66
    for i,word in enumerate(words):
        assert read(u32(va-base+4*i)-base,len(word)+1)==word+b'\0'
    maxima[name]=max(map(len,words))

source=re.sub(r'/\*.*?\*/','',Path('src/SOURCE/CMBTMGR.cpp').read_text(),flags=re.S)
literal=r'"(?:\\.|[^"\\])*"'
literals=[b''.join(ast.literal_eval(v).encode('latin1') for v in re.findall(literal,group))
          for group in re.findall(r'(?:'+literal+r'\s*)+',source)]
assert max(map(len,literals))<=100 and max(maxima.values())<=100
formats=sorted({v for v in literals if b'%' in v});maximum=0
for fmt in formats:
    conversions=re.findall(rb'%(?:0[24])?[sdc]',fmt)
    assert not re.search(rb'%(?!(?:0[24])?[sdc])',fmt),fmt
    bound=len(re.sub(rb'%(?:0[24])?[sdc]',b'',fmt))+1
    bound+=sum(100 if v.endswith(b's') else 11 if v.endswith(b'd') else 1 for v in conversions)
    assert bound<=768,(fmt,bound)
    maximum=max(maximum,bound)
    print('PASS gText format upper bound',bound,fmt)
print('PASS all',len(formats),'literal formats fit gText768; conservative maximum',maximum)
