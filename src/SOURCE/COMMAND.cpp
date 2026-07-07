// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\COMMAND.OBJ   from: (directly linked into exe)
// functions: 36   data: 15
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/COMMAND.h>
VA(0x0042a6d0, 0x36d)
int combatManager::Main(struct tag_message &) { return 0; }

VA(0x0042aa3d, 0x181)
int combatManager::ValidHexToStandOn(int) { return 0; }

VA(0x0042abbe, 0x8a1)
void combatManager::SetCombatDirections(int) {}

VA(0x0042b45f, 0x63c)
void combatManager::CheckSetMouseDirection(int, int, int) {}

VA(0x0042ba9b, 0x8b)
int combatManager::GetPointer(int, int) { return 0; }

VA(0x0042bb26, 0x8e4)
int combatManager::ProcessCombatMsg(struct tag_message &) { return 0; }

VA(0x0042c40a, 0x70)
int combatManager::IsNegationSphereInEffect(void) { return 0; }

VA(0x0042c47a, 0x205)
void combatManager::ResetRound(void) {}

VA(0x0042c67f, 0x280)
int combatManager::CheckWin(struct tag_message *) { return 0; }

VA(0x0042c8ff, 0x51a)
int combatManager::GetCommand(int) { return 0; }

VA(0x0042ce19, 0x2a6)
int combatManager::RightClick(int) { return 0; }

VA(0x0042d0bf, 0x3b3)
void combatManager::DoCommand(int) {}

VA(0x0042d472, 0x57b)
int WinCombatHandler(struct tag_message &) { return 0; }

VA(0x0042d9ed, 0x110)
void combatManager::ClearWinLoseBottom(class heroWindow *) {}

VA(0x0042dafd, 0x29a)
void combatManager::ShowWinLoseArtifact(class heroWindow *, int) {}

VA(0x0042dd97, 0x232)
void combatManager::ShowSkeletons(class heroWindow *) {}

VA(0x0042dfc9, 0x2f6)
void combatManager::ShowEagleEyeSpell(class heroWindow *) {}

VA(0x0042e2bf, 0x9cc)
void combatManager::ShowDeadArmies(class heroWindow *) {}

VA(0x0042ec8b, 0xba9)
void combatManager::DoVictory(int) {}

VA(0x0042f834, 0x3bc)
void combatManager::DoLoseWindow(void) {}

VA(0x0042fbf0, 0x43d)
int combatManager::DoSurrender(void) { return 0; }

VA(0x0043002d, 0xdc)
void combatManager::CheckChangeSelector(void) {}

VA(0x00430109, 0xea)
void combatManager::CheckCastleAttack(void) {}

VA(0x004301f3, 0xdd)
void combatManager::CheckGetAIMove(void) {}

VA(0x004302d0, 0x18f)
void combatManager::GetControl(void) {}

VA(0x0043045f, 0xd7)
void combatManager::ResetMouse(void) {}

VA(0x00430536, 0x65b)
int combatManager::ProcessNextAction(struct tag_message &) { return 0; }

VA(0x00430b91, 0x237)
void combatManager::ResetCyclingCreatures(void) {}

VA(0x00430dc8, 0xf9)
void combatManager::ResetCycleTimers(void) {}

VA(0x00430ec1, 0x53)
int InCombatArea(int, int) { return 0; }

VA(0x00430f14, 0x9d9)
void combatManager::CycleCombatScreen(void) {}

VA(0x004318ed, 0x3b)
void combatManager::SetCombatViewArmySmallLevel(int) {}

VA(0x00431928, 0xe3)
void combatManager::SetCombatGrid(int, int, int) {}

VA(0x00431a0b, 0x3ab)
void combatManager::AddArmy(int, int, int, int, int, int) {}

VA(0x00431db6, 0x169)
void combatManager::SetupSmallView(void) {}

VA(0x00431f1f, 0x304)
void combatManager::ViewBallista(int) {}

// ---- globals (definitions, RVA order) ----
int gbThisNetHasControl;
int iCurTransferArtifact;
signed char *iTransferArtifactsInfo;
int gbWhichAnimationPlaying;
int iMaxTransferArtifacts;
int giNextActionExtra;
int bSkeletonsShown;
int gbShowingLoseWindow;
int giWinCmbtFrame;
int giNextActionGridIndex;
int giSurrenderCost;
int giSkeletonsCreated;
signed char *iTransferArtifacts;
int giNextAction;
int giNextActionGridIndex2;
