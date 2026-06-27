// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\EVENTS.OBJ   from: (directly linked into exe)
// functions: 42   data: 1
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

RVA(0x004a8530, 0x5adb)
void advManager::DoEvent(class mapCell *, int, int);

RVA(0x004ae00b, 0x9f7)
void advManager::EraseObj(class mapCell *, int, int);

RVA(0x004aea02, 0x90)
void advManager::HeroSwap(class hero *, class hero *);

RVA(0x004aea92, 0x12f)
int advManager::BarrierEvent(class mapCell *, class hero *);

RVA(0x004aebc1, 0x99)
signed char StrEqNoCase(char *, char *);

RVA(0x004aec5a, 0xde)
void advManager::PasswordEvent(class mapCell *, class hero *);

RVA(0x004aed38, 0x6fe)
void advManager::GenericSiteEvent(class mapCell *, class hero *);

RVA(0x004af436, 0x191)
void advManager::RecruitSiteEvent(class mapCell *, class hero *);

RVA(0x004af5c7, 0x8b)
void advManager::ExpansionRecruitEvent(class hero *, int, short int *);

RVA(0x004af652, 0x22a)
void advManager::JailEvent(class mapCell *, class hero *, int, int);

RVA(0x004af87c, 0x1da)
void advManager::TownEvent(class mapCell *, int, int);

RVA(0x004afa56, 0x516)
void advManager::EventSound(int, int, struct SAMPLE2 *);

RVA(0x004aff6c, 0xc7)
void advManager::EventWindow(int, int, char *, int, int, int, int, int);

RVA(0x004b0033, 0xb6)
int GiveArtifact(class hero *, int, int, signed char);

RVA(0x004b00e9, 0x5e)
int advManager::GiveRandomArtifact(class hero *);

RVA(0x004b0147, 0x67)
int advManager::GiveExperience(class hero *, int, int);

RVA(0x004b01ae, 0x80)
void advManager::GiveResource(class hero *, int, int);

RVA(0x004b022e, 0xbb)
void advManager::RecruitEvent(class hero *, int, class mapCell *);

RVA(0x004b02e9, 0x261)
int advManager::SkeletonEvent(class hero *, class mapCell *, char *, int, int);

RVA(0x004b054a, 0x29b)
int advManager::ZombieEvent(class hero *, class mapCell *, char *, int, int);

RVA(0x004b07e5, 0x2f8)
int advManager::GhostEvent(class hero *, class mapCell *, char *, int, int);

RVA(0x004b0add, 0x274)
void advManager::HouseEvent(class hero *, class mapCell *);

RVA(0x004b0d51, 0x62f)
int advManager::CombatMonsterEvent(class hero *, int, int, class mapCell *, int, int, int, int, int, int, int, int, int, int, int);

RVA(0x004b1380, 0x5f3)
void GiveTakeArtifactStat(class hero *, int, int);

RVA(0x004b1973, 0x1dd)
void advManager::TransferArtifacts(class hero *, class hero *);

RVA(0x004b1b50, 0x7f)
void advManager::HeroLoses(class hero *);

RVA(0x004b1bcf, 0x132)
void advManager::DoWhirlpool(class hero *);

RVA(0x004b1d01, 0x142)
void advManager::FizzleCenter(int);

RVA(0x004b1e43, 0x2a40)
void advManager::DoAIEvent(class mapCell *, class hero *, int, int);

RVA(0x004b4883, 0x65)
int advManager::BarrierAIEvent(class mapCell *, class hero *);

RVA(0x004b48e8, 0x53)
void advManager::PasswordAIEvent(class mapCell *, class hero *);

RVA(0x004b493b, 0x369)
void advManager::GenericSiteAIEvent(class mapCell *, class hero *);

RVA(0x004b4ca4, 0x18f)
void advManager::RecruitSiteAIEvent(class mapCell *, class hero *);

RVA(0x004b4e33, 0x1a2)
void advManager::JailAIEvent(class mapCell *, class hero *, int, int);

RVA(0x004b4fd5, 0x82b)
void advManager::PlayerMonsterInteract(class mapCell *, class mapCell *, class hero *, int *, int, int, int, int, int);

RVA(0x004b5800, 0x440)
void advManager::ComputerMonsterInteract(class mapCell *, class hero *, int *);

RVA(0x004b5c40, 0x1d0)
int advManager::DoNetCombat(char *);

RVA(0x004b5e10, 0x64e)
int advManager::DoCombat(int, int, class hero *, class armyGroup *, class town *, class hero *, class armyGroup *, int, int, int, int);

RVA(0x004b645e, 0x36f)
void advManager::SendHeroTownData(int, int, class hero *, class armyGroup *, class town *, class hero *, class armyGroup *, int, int, int, int, int, int, int);

RVA(0x004b67cd, 0x462)
void advManager::ReceiveHeroTownData(char *, int *, int *, int *, class hero * *, class armyGroup * *, class town * *, class hero * *, class armyGroup * *, int *, int *, int *, signed char *, signed char *, signed char *);

RVA(0x004b6c2f, 0x254)
int advManager::AutoResolveCombat(int, int, class hero *, class armyGroup *, class town *, class hero *, class armyGroup *, int, int, int, int);

RVA(0x004b6e83, 0xb8)
int RiddleStringsEqual(char *, char *);

// ---- data / globals / vtables ----
DATA(0x0051cc0c)  // int gbNoShowCombat
