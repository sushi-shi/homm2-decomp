// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\CMBTMGR.OBJ   from: (directly linked into exe)
// functions: 39   data: 7
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/CMBTMGR.h>
VA(0x0048fd50, 0x1ba)
combatManager::combatManager(void) {}

VA(0x0048ff0a, 0x128)
void combatManager::CombineGroups(class armyGroup *, class armyGroup *) {}

VA(0x00490032, 0x5f2)
void combatManager::SetupCombat(int, int, class hero *, class armyGroup *, class town *, class hero *, class armyGroup *, int, int, int) {}

VA(0x00490624, 0x279)
void combatManager::InitNonVisualVars(void) {}

VA(0x0049089d, 0x203)
void combatManager::SetupAdjacencyArray(void) {}

VA(0x00490aa0, 0x43f)
int combatManager::Open(int) { return 0; }

VA(0x00490edf, 0x3d6)
void combatManager::Close(void) {}

VA(0x004912b5, 0x38c)
void combatManager::UpdateArmyGroup(int) {}

VA(0x00491641, 0x365)
void combatManager::GenerateMap(void) {}

VA(0x004919a6, 0x224)
char * combatManager::GetBackgroundName(void) { return 0; }

VA(0x00491bca, 0x210)
int combatManager::MoreTreesNear(void) { return 0; }

VA(0x00491dda, 0x3e7)
void combatManager::LoadIcons(void) {}

VA(0x004921c1, 0x124)
void combatManager::FreeIcons(void) {}

VA(0x004922e5, 0x36d)
void combatManager::LoadArmies(void) {}

VA(0x00492652, 0xdb)
void combatManager::FreeArmies(void) {}

VA(0x0049272d, 0x1e2)
int combatManager::GetGridIndex(int, int) { return 0; }

VA(0x0049290f, 0x1eb)
void combatManager::CheckApplyGoodMorale(int, int) {}

VA(0x00492afa, 0x1cd)
int combatManager::CheckApplyBadMorale(int, int) { return 0; }

VA(0x00492cc7, 0x382)
int combatManager::GetNextArmy(int) { return 0; }

VA(0x00493049, 0xd6)
int combatManager::IsWinner(int) { return 0; }

VA(0x0049311f, 0x100e)
void combatManager::CatAttack(int) {}

VA(0x0049412d, 0x74f)
void combatManager::KeepAttack(int) {}

VA(0x0049487c, 0x17b)
int combatManager::ExperienceValueOfStack(int) { return 0; }

VA(0x004949f7, 0x88)
void combatManager::ResetHitByCreature(void) {}

VA(0x00494a7f, 0x36)
int ValidHex(int) { return 0; }

VA(0x00494ab5, 0x16)
void combatManager::SaveCombatBorder(void) {}

VA(0x00494acb, 0x16)
void combatManager::DrawCombatBorder(void) {}

VA(0x00494ae1, 0x4d8)
void combatManager::SetupAndLoadObstacles(void) {}

VA(0x00494fb9, 0x2a1)
void combatManager::MakeCreaturesVanish(void) {}

VA(0x0049525a, 0xbd)
void combatManager::LowerDoor(void) {}

VA(0x00495317, 0xe6)
void combatManager::RaiseDoor(void) {}

VA(0x004953fd, 0x84)
void combatManager::TestRaiseDoor(void) {}

VA(0x00495481, 0xd8)
int combatManager::InCastle(int) { return 0; }

VA(0x00495559, 0x346)
int combatManager::ShotIsThroughWall(int, int, int) { return 0; }

VA(0x0049589f, 0x52e)
void combatManager::ShootMissile(int, int, int, int, float *, class icon *) {}

VA(0x00495dcd, 0xf2)
void combatManager::CombatSystemOptions(void) {}

VA(0x00495ebf, 0x1ea)
void UpdateCombatSystemOptions(int) {}

VA(0x004960a9, 0x39a)
int CombatSystemOptionsHandler(struct tag_message &) { return 0; }


// ===== vtable combatManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x00490aa0, 0x43f)  int combatManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x00490edf, 0x3d6)  void combatManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x0042a6d0, 0x36d)  int combatManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(combatManager, 0x004eb898);
