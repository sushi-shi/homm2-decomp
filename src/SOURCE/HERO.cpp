// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\HERO.OBJ   from: (directly linked into exe)
// functions: 41   data: 5
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <_all.h>
VA(0x0046c3a0, 0x6f)
hero::hero(void) {}

VA(0x0046c40f, 0x53)
void hero::Read(int, signed char) {}

VA(0x0046c462, 0x53)
void hero::Write(int, signed char) {}

VA(0x0046c4b5, 0x18)
void hero::GetArmyStrengths(unsigned long int * const) {}

VA(0x0046c4cd, 0x59)
int hero::HasArtifact(int) { return 0; }

VA(0x0046c526, 0x277)
int hero::CalcMobility(void) { return 0; }

VA(0x0046c79d, 0xcf)
int hero::HasSpell(int) { return 0; }

VA(0x0046c86c, 0xc5)
int hero::GetNthSpell(int, int) { return 0; }

VA(0x0046c931, 0xd0)
int hero::GetNumSpells(int) { return 0; }

VA(0x0046ca01, 0x8a)
void hero::UseSpell(int) {}

VA(0x0046ca8b, 0x26)
void hero::AddSpell(int, int) {}

VA(0x0046cab1, 0x82)
void HeroMessageUpdate(char *) {}

VA(0x0046cb33, 0xa8)
void hero::HeroScreenUpdate(void) {}

VA(0x0046cbdb, 0x1d2)
void hero::UpdateArmies(void) {}

VA(0x0046cdad, 0x43)
void hero::ViewStat(int, int) {}

VA(0x0046cdf0, 0x9b)
void hero::ViewArtifact(int, int, int) {}

VA(0x0046ce8b, 0x5d)
int hero::Dismiss(void) { return 0; }

VA(0x0046cee8, 0x587)
void hero::Deallocate(int) {}

VA(0x0046d46f, 0x9e)
int hero::GetExperience(int) { return 0; }

VA(0x0046d50d, 0xc0)
int hero::GetLevel(int) { return 0; }

VA(0x0046d5cd, 0x254)
void hero::ApplyBattleWinTemps(void) {}

VA(0x0046d821, 0x1e)
void hero::ApplyBattleLossTemps(void) {}

VA(0x0046d83f, 0x828)
void hero::CheckLevel(void) {}

VA(0x0046e067, 0x57)
int hero::NumArtifacts(void) { return 0; }

VA(0x0046e0be, 0x758)
void UpdateHeroScreenStatusBar(struct tag_message &) {}

VA(0x0046e816, 0xaef)
int HeroHandler(struct tag_message &) { return 0; }

VA(0x0046f305, 0x4f)
void RedrawHeroScreen(void) {}

VA(0x0046f354, 0x218)
int HeroView(int, int, int) { return 0; }

VA(0x0046f56c, 0x9c5)
void SetupHeroView(void) {}

VA(0x0046ff31, 0x2b0)
void DoHeroSplit(int, int) {}

VA(0x004701e1, 0x6a)
void hero::SetSS(int, int) {}

VA(0x0047024b, 0xfa)
int hero::TakeSS(int, int) { return 0; }

VA(0x00470345, 0xbf)
int hero::GiveSS(int, int) { return 0; }

VA(0x00470404, 0x6a)
int hero::CreatureTypeCount(int) { return 0; }

VA(0x0047046e, 0x5e)
void hero::UpgradeCreatures(int, int) {}

VA(0x004704cc, 0x5e)
int hero::GetNthSS(int) { return 0; }

VA(0x0047052a, 0x51)
class town * hero::GetOccupiedTown(void) { return 0; }

VA(0x0047057b, 0x47)
signed char hero::Stats(int) { return 0; }

VA(0x004705c2, 0xc3)
signed char hero::GetSSLevel(int) { return 0; }

VA(0x00470685, 0xf4)
void hero::DoSSLevelDialog(int, int) {}

VA(0x00470779, 0x12f)
void hero::CheckAnduranPieces(int) {}

// ---- data / globals / vtables ----
DATA(0x004f6c88)  // class hero * gpHVHero
DATA(0x004f6c8c)  // class heroWindow * gheroWin
DATA(0x004f6cd0)  // short int * gMinExpForLevel
DATA(0x005280dc)  // int iOrigHeroViewID
DATA(0x005280e0)  // int gbNoDismiss
