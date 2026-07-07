// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\DRAWING.OBJ   from: (directly linked into exe)
// functions: 13   data: 2
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/combatManager.h>
VA(0x00402910, 0x41)
void combatManager::NoShowCombatLog(char *) {}

VA(0x00402951, 0xda)
void combatManager::ClearCombatMessages(int) {}

VA(0x00402a2b, 0x5d)
void combatManager::CheckUpdateCombatMessages(void) {}

VA(0x00402a88, 0x3f8)
void combatManager::CombatMessage(char *, int, int, int) {}

VA(0x00402e80, 0x3fe)
void combatManager::CombatMessage(int) {}

VA(0x0040327e, 0x126)
void combatManager::ResetLimitCreature(void) {}

VA(0x004033a4, 0x6a)
void combatManager::UpdateCombatArea(void) {}

VA(0x0040340e, 0x213)
void combatManager::SetupGridForArmy(class army *) {}

VA(0x00403621, 0x5fb)
int combatManager::UpdateGrid(int, int) { return 0; }

VA(0x00403c1c, 0x364)
void combatManager::DrawBackground(void) {}

VA(0x00403f80, 0x64c)
void combatManager::UpdateMouseGrid(int, int) {}

VA(0x004045cc, 0x173f)
void combatManager::DrawFrame(int, int, int, int, int, int, int) {}

VA(0x00405d0b, 0xb99)
void combatManager::DrawSmallView(int, int) {}
