// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\SPELLAI.OBJ   from: (directly linked into exe)
// functions: 11   data: 3
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

RVA(0x004867c0, 0x279)
int combatManager::DoSpellAI(int, int);

RVA(0x00486a39, 0x1155)
void combatManager::DetermineEffectOfSpell(int, int *, int *);

RVA(0x00487b8e, 0x34c)
int combatManager::EffectSpellCreateCreature(int, int);

RVA(0x00487eda, 0x72d)
int combatManager::RawEffectSpellInfluence(class army *, int);

RVA(0x00488607, 0x66)
void combatManager::ClearEffects(void);

RVA(0x0048866d, 0x40)
void combatManager::NextPos(int *);

RVA(0x004886ad, 0xa1)
int combatManager::FirstArmy(int, int, int *);

RVA(0x0048874e, 0x73)
int combatManager::FirstResurrectable(int, int *, int);

RVA(0x004887c1, 0x421)
void combatManager::EffectSpellCure(int *, int, int, int);

RVA(0x00488be2, 0x176)
void combatManager::EffectSpellResurrect(int *, int, int);

RVA(0x00488d58, 0xcc9)
void combatManager::EffectSpellDamage(int *, int, int);

// ---- data / globals / vtables ----
DATA(0x004f80b8)  // float * gfDurationMods
DATA(0x004f80e8)  // float * gfCancelDurationMods
DATA(0x005284b4)  // int giCurrSpellGroup
