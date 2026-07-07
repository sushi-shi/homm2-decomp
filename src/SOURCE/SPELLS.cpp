// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\SPELLS.OBJ   from: (directly linked into exe)
// functions: 37   data: 7
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <_all.h>
VA(0x004204c0, 0x86)
int combatManager::HasValidSpellTarget(int) { return 0; }

VA(0x00420546, 0x44a)
int combatManager::ViewSpells(int) { return 0; }

VA(0x00420990, 0x15c)
int CombatSpecialHandler(struct tag_message &) { return 0; }

VA(0x00420aec, 0x2aa)
int HandleCastSpell(struct tag_message &) { return 0; }

VA(0x00420d96, 0x2e5)
int combatManager::FindResurrectArmyIndex(int, int, int) { return 0; }

VA(0x0042107b, 0x521)
int combatManager::ValidSpellTarget(int, int) { return 0; }

VA(0x0042159c, 0x222)
void combatManager::SpellMessage(int, int) {}

VA(0x004217be, 0x1eca)
void combatManager::CastSpell(int, int, int, int) {}

VA(0x00423688, 0xda)
void combatManager::DefaultSpell(int) {}

VA(0x00423762, 0x623)
void combatManager::Fireball(int, int) {}

VA(0x00423d85, 0x3c9)
void combatManager::MeteorShower(int) {}

VA(0x0042414e, 0x2fb)
void combatManager::ElementalStorm(void) {}

VA(0x00424449, 0x9ff)
void combatManager::Armageddon(void) {}

VA(0x00424e48, 0x101)
void combatManager::TurnToStone(class army *) {}

VA(0x00424f49, 0x192)
void combatManager::BloodLustEffect(class army *, int) {}

VA(0x004250db, 0x80)
void combatManager::Ripple(int) {}

VA(0x0042515b, 0x72)
void combatManager::Blur(int, int, int) {}

VA(0x004251cd, 0x320)
void combatManager::ResetBoltAngle(struct SBolt *) {}

VA(0x004254ed, 0x4f0)
void combatManager::DrawBolt(struct SBolt *, int) {}

VA(0x004259dd, 0x26f)
void combatManager::AddBolt(struct SBolt *, int, int, int, int, int, int, int, int, int, int, int, int) {}

VA(0x00425c4c, 0xa82)
void combatManager::DoBolt(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int) {}

VA(0x004266ce, 0x18c)
int combatManager::GetNextChainLightningTarget(class army *, int) { return 0; }

VA(0x0042685a, 0x361)
void combatManager::ChainLightning(int, int) {}

VA(0x00426bbb, 0x292)
void combatManager::VaporizeCreature(int, int) {}

VA(0x00426e4d, 0x592)
void combatManager::RippleCreature(int, int, int) {}

VA(0x004273df, 0x6b2)
void combatManager::ShowMassSpell(signed char (* const)[20], int, int) {}

VA(0x00427a91, 0x8f8)
void combatManager::CastMassSpell(int, int) {}

VA(0x00428389, 0x5c8)
void combatManager::MirrorImage(int) {}

VA(0x00428951, 0x218)
void combatManager::SummonElemental(int, int) {}

VA(0x00428b69, 0x1e6)
void combatManager::DoLuck(int, int) {}

VA(0x00428d4f, 0x33a)
void combatManager::DoBlast(int, int) {}

VA(0x00429089, 0x655)
void combatManager::Resurrect(int, int, int) {}

VA(0x004296de, 0xb9)
int combatManager::SpaceForElementalExists(void) { return 0; }

VA(0x00429797, 0xd9)
void combatManager::ShowSpellCastFailure(class army *, int) {}

VA(0x00429870, 0x270)
void combatManager::ModifyDamageForArtifacts(long int *, int, class hero *, class hero *) {}

VA(0x00429ae0, 0x931)
void combatManager::Earthquake(void) {}

VA(0x0042a411, 0x2b1)
void combatManager::ShowSpellMessage(int, int, class army *) {}

// ---- data / globals / vtables ----
DATA(0x004f00b0)  // int castX
DATA(0x004f00b4)  // int castY
DATA(0x004f00b8)  // int bInTeleportGetDest
DATA(0x004f0278)  // int indexToCastOn
DATA(0x004f04c0)  // unsigned char * uRedBeam
DATA(0x004f04c8)  // unsigned char * uRainbow
DATA(0x004f04e0)  // signed char * gyModify
