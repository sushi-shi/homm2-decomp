"""B56: all campaign hero names copied by NewMap, conditional on valid campaign indices."""
import ast
import re
from pathlib import Path
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

retail=Path('build/orig/HMM2PL.exe');verify_retail(retail)
base,_,u32,read=_pe_layout(retail)
assert re.search(r'HERO_NAME_SIZE\s*=\s*13\b',Path('include/SOURCE/hero.h').read_text())
assert 'char m_name[HERO_NAME_SIZE];' in Path('include/SOURCE/hero.h').read_text()
assert re.search(r'X_GLOBAL_EXPANSION_HERO_NAME_COUNT\s*=\s*6\b',Path('include/SOURCE/X_GLOBAL.h').read_text())
assert re.search(r'GLOBAL_SHORT_MAP_NAME_SIZE\s*=\s*16\b',Path('include/SOURCE/X_GLOBAL.h').read_text())
for label,va in [('Joseph',0x51b7d0),('Ivan',0x51b7e8)]:
    names=[]
    for index in range(6):
        raw=read(u32(va-base+index*4)-base,64)
        assert b'\0' in raw
        names.append(raw.split(b'\0',1)[0])
    lengths=[len(value)+1 for value in names]
    assert max(lengths)<=13
    print('PASS',label,'all six lengths including NUL',lengths,'within13')
source=Path('src/SOURCE/GAME.cpp').read_text()
for name in ['Сестра Элиза','Брат Бракс']:
    encoded=name.encode('cp1251')
    # The complete function's retail operand check separately pins these
    # private literals to their actual retail destinations.
    literal,=re.findall(r'("(?:[^"\\]|\\.)*")\s*/\* "'+re.escape(name)+r'" \*/',source)
    assert ast.literal_eval('b'+literal)==encoded
    assert len(encoded)+1<=13
    print('PASS literal',name,len(encoded)+1,'/13 including NUL')
campaign=Path('src/SOURCE/X_CAMPGN.cpp').read_text()
for name,table in [('JosephName','xJosephName'),('IvanName','xUncleIvanName')]:
    start=campaign.index('H2_CONST char* ExpCampaign::'+name+'(void) {')
    body=campaign[start:campaign.index('\n}',start)]
    assert 'if (m_currentMap < EXPANSION_CAMPAIGN_FIRST_ALTERNATE_NAME_MAP)' in body
    assert 'return '+table+'[0];' in body
    assert 'return '+table+'[IDX(m_currentMap) - IDX(EXPANSION_CAMPAIGN_FIRST_ALTERNATE_NAME_MAP)];' in body
assert re.search(r'EXPANSION_CAMPAIGN_FIRST_ALTERNATE_NAME_MAP\s*=\s*4\b',Path('include/SOURCE/ExpCampaign.h').read_text())
print('PASS campaign maps below4 select entry0; maps4..9 select entries0..5. Higher indices remain invalid.')
print('Map filename remains a caller contract: gMapName[16] requires at most15 bytes plus NUL; no runtime bound is added.')
