"""C60/C61/B77: fixed byte owner and numeric operands, not generic exemptions."""
import re
import struct
from pathlib import Path
from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control=Path('build/orig/HMM2PL.exe'); verify_retail(control)
base,sites,u32,read=_pe_layout(control)
obj=CoffObject(Path('build/objdiff/base/SOURCE/SPELLAI.obj').read_bytes())
cases=[
 ('RawEffectSpellInfluence',0x9716f,1461,{
     0x34:(0x4ea768,'d',0),0x1b5:(0x4ea840,'f',7),0x1cc:(0x4ea840,'f',7),
     0x1e6:(0x4ea570,'f',10),0x21e:(0x4ea500,'d',0.5),0x240:(0x4ea838,'d',0.45),
     0x4f7:(0x4ea568,'d',0.3),0x503:(0x4ea518,'f',1)}),
 ('DetermineEffectOfSpell',0x95fb4,3918,{
     0x411:(0x4ea500,'d',0.5),0x9e6:(0x4ea508,'d',1.5),0x9fd:(0x4ea500,'d',0.5),
     0xa93:(0x4ea508,'d',1.5),0xaaa:(0x4ea500,'d',0.5),0xb8e:(0x4ea500,'d',0.5)}),
 ('EffectSpellCreateCreature',0x96f02,621,{0x240:(0x4ea608,'d',1.4)}),
 ('EffectSpellCure',0x97879,875,{0x129:(0x4ea5b0,'d',0.75)}),
 ('EffectSpellResurrect',0x97be4,319,{0x12b:(0x4ea5c0,'d',0.85)}),
 ('EffectSpellDamage',0x97d23,2609,{
     0x4a4:(0x4ea520,'f',0),0x4fd:(0x4ea500,'d',0.5),0x51f:(0x4ea500,'d',0.5),
     0x54f:(0x4ea500,'d',0.5),0x57c:(0x4ea500,'d',0.5),0x5a9:(0x4ea500,'d',0.5),
     0x709:(0x4ea5b0,'d',0.75),0x878:(0x4ea498,'d',0.05),0x944:(0x4ea6c0,'d',1.25)}),
]
for name,rva,size,expected in cases:
    fn,=[s for s in obj.symbols.values() if s.typ==FUNCTION_TYPE and s.name.startswith('?'+name+'@combatManager@@')]
    raw=obj.section_bytes(obj.symbol_section(fn)); seen=set()
    for reloc in obj.relocations:
        if reloc.section!=fn.section or not fn.value<=reloc.site<fn.value+size: continue
        owner=obj.symbols[reloc.symbol_index]
        if not owner.name.startswith('__real@'): continue
        offset=reloc.site-fn.value
        destination,fmt,value=expected[offset]; seen.add(offset)
        encoded=struct.pack('<'+fmt,value)
        assert reloc.typ==6 and raw[reloc.site:reloc.site+4]==bytes(4)
        assert rva+offset in sites and u32(rva+offset)==destination
        data=obj.section_bytes(obj.symbol_section(owner))[owner.value:owner.value+len(encoded)]
        assert data==encoded==read(destination-base,len(encoded)),(name,hex(offset))
        print('PASS',name,hex(offset),hex(destination),fmt,value)
    assert seen==set(expected)

# Source-owned spell domain, with one-byte physical representation in both
# the definition and declaration; all fifteen influence entries plus the
# original unused trailing zero remain in place.
values=[('SPELL_HASTE',9),('SPELL_SLOW',11),('SPELL_BLIND',13),('SPELL_BLESS',14),
        ('SPELL_CURSE',18),('SPELL_BERSERKER',26),('SPELL_PARALYZE',30),('SPELL_HYPNOTIZE',31),
        ('SPELL_DRAGON_SLAYER',37),('SPELL_BLOOD_LUST',38),('SPELL_SHIELD',41),
        ('CREATURE_SPELL_PETRIFY',101),('SPELL_ANTI_MAGIC',22),('SPELL_STONE_SKIN',16),
        ('SPELL_STEEL_SKIN',17),('SPELL_FIREBALL',0)]
types=Path('include/SOURCE/KB_TYPES.h').read_text()
for name,value in values:
    assert re.search(r'\b'+name+r'\s*=\s*'+str(value)+r'\b',types),name
declaration='H2_ENUM_STORAGE(SpellType, u8) giSpellInfluenceToSpell[KB_SPELL_INFLUENCE_MAP_COUNT]'
kb=Path('src/SOURCE/KB.cpp').read_text()
assert declaration in kb and declaration in Path('include/SOURCE/X_GLOBAL.h').read_text()
start=kb.index(declaration); block=kb[start:kb.index('};',start)]
assert re.findall(r'\b(?:SPELL_[A-Z_]+|CREATURE_SPELL_PETRIFY)\b',block)==[n for n,v in values]
encoded=bytes(v for n,v in values)
assert read(0xfba68,16)==encoded
dataobj=CoffObject(Path('build/objdiff/base/SOURCE/KB.obj').read_bytes())
symbol,=[s for s in dataobj.symbols.values() if s.name=='?giSpellInfluenceToSpell@@3PAEA']
assert dataobj.section_bytes(dataobj.symbol_section(symbol))[symbol.value:symbol.value+16]==encoded
assert u32(0x9716f+0x1f)==0x4fba68 and 0x9716f+0x1f in sites
source=Path('src/SOURCE/SPELLAI.cpp').read_text()
assert 'SpellCastWorkChance(giSpellInfluenceToSpell[IDX(influence)])' in source
assert 'SpellType(giSpellInfluenceToSpell' not in source
print('PASS all16 typed influence-map bytes and the exact retail consumer operand')

# Explicit source-owned, positive Buka modifiers. These are not anonymous
# constants accepted by name alone, and no gameplay sign is changed.
for suffix,va,value in [('BLIND',0x4ea80c,0.4),('BERSERK',0x4ea810,0.55),
    ('PARALYZE',0x4ea814,0.5),('HYPNOTIZE',0x4ea818,0.65),('PETRIFIED',0x4ea81c,0.25),
    ('BLOODLUST',0x4ea820,0.14),('STONE_SKIN',0x4ea824,0.16),('STEEL_SKIN',0x4ea828,0.28),
    ('ANTI_MAGIC',0x4ea82c,0.2),('DRAGON_SLAYER',0x4ea830,0.28),('SHIELD',0x4ea834,0.45)]:
    name='COMBAT_SPELL_AI_'+suffix+'_MODIFIER'
    assert re.search(r'\b'+name+r'\s*=\s*'+re.escape(str(value))+r'f\s*;',source),name
    assert read(va-base,4)==struct.pack('<f',value),(name,va)
print('PASS all11 named spell-scoring modifiers against retail')

# SSpellInfo is22 packed bytes; cost follows9 text bytes, three byte
# fields and the signed16-bit AI value. The four artifact categories in
# GetManaCost are mutually exclusive for a given spell, so at most one
# halving applies. This closes the divisor concern for default combat data,
# not arbitrary modifications of the spell catalog.
halved={18,19,26,31,30,13,14,15,43,44,45,46}
assert re.search(r'SPELL_INFO_ATTRIBUTE_COMBAT\s*=\s*0x02\b',types)
combat=[i for i in range(65) if read(0xfb3b8+22*i+21,1)[0]&2]
assert combat==list(range(48)),combat
costs=[read(0xfb3b8+22*i+14,1)[0] for i in range(48)]
effective=[cost//2 if i in halved else cost for i,cost in enumerate(costs)]
assert all(value>0 for value in effective)
print('PASS all48 combat mana costs stay positive; minimum after applicable artifact',min(effective))
