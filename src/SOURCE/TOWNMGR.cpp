// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\TOWNMGR.OBJ   from: (directly linked into exe)
// functions: 33   data: 2
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <rva.h>
RVA(0x00413900, 0x16a)
// void townObject::constructor(int, int, char *);

RVA(0x00413a6a, 0x60)
// void townObject::~destructor(void);

RVA(0x00413aca, 0x437)
// void townObject::Draw(int);

RVA(0x00413f01, 0x68)
// void townManager::constructor(void);

RVA(0x00413f69, 0x1a0)
// void townManager::SetupExtraStuff(void);

RVA(0x00414109, 0x1ef)
// int townManager::Open(int);   // virtual [override (implements baseManager pure virtual)]

RVA(0x004142f8, 0x77)
// void townManager::ChangeTown(void);

RVA(0x0041436f, 0x95a)
// void townManager::SetupTown(void);

RVA(0x00414cc9, 0x1cf)
// void townManager::UnloadTown(void);

RVA(0x00414e98, 0xca)
// void townManager::Close(void);   // virtual [override (implements baseManager pure virtual)]

RVA(0x00414f62, 0x3b9)
// void townManager::SetArmyCommand(int);

RVA(0x0041531b, 0x5c5)
// void townManager::SetCommandAndText(struct tag_message &);

RVA(0x004158e0, 0x7d)
// void townManager::ShowText(char *);

RVA(0x0041595d, 0x1830)
// int townManager::Main(struct tag_message &);   // virtual [override (implements baseManager pure virtual)]

RVA(0x0041718d, 0x4e3)
// void townManager::DoCommand(int);

RVA(0x00417670, 0xad)
// void townManager::RedrawTownScreen(void);

RVA(0x0041771d, 0x374)
// void townManager::SplitArmy(void);

RVA(0x00417a91, 0x21)
// void townManager::ShiftQualChange(void);

RVA(0x00417ab2, 0xb0)
// void townManager::ResetStrips(void);

RVA(0x00417b62, 0x7b)
// void townManager::Toggle(int);

RVA(0x00417bdd, 0xc0)
// void townManager::DrawTown(int, int);

RVA(0x00417c9d, 0xf35)
// int townManager::BuyBuild(int, int, int);

RVA(0x00418bd2, 0x3e9)
// void townManager::BuildObj(int);

RVA(0x00418fbb, 0x3d8)
// void townManager::SetupMage(class heroWindow *);

RVA(0x00419393, 0x190)
// int MageGuildHandler(struct tag_message &);

RVA(0x00419523, 0x706)
// int townManager::RecruitHero(int, int);

RVA(0x00419c29, 0x153)
// int TavernHandler(struct tag_message &);

RVA(0x00419d7c, 0x110)
// void townManager::DoTavern(void);

RVA(0x00419e8c, 0x328)
// int SplitArmyHandler(struct tag_message &);

RVA(0x0041a1b4, 0x5cf)
// void townManager::SetupWell(class heroWindow *);

RVA(0x0041a783, 0xf0f)
// void townManager::SetupThievesGuild(class heroWindow *, int);

RVA(0x0041b692, 0x56a)
// void GetCategoryStats(int, long int * const, signed char * const);

RVA(0x0041bbfc, 0xd9)
// void SortStats(long int * const, signed char * const);

// ---- data / globals / vtables ----
DATA(0x004eb140)  // const townManager::vftable
DATA(0x004ee750)  // struct SBuildingInfo (*)[32] sBuildingInfo

// ===== vtable townManager : public baseManager  (3 slots) =====
//  [ 0] RVA(0x00414109, 0x1ef)  int townManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] RVA(0x00414e98, 0xca)  void townManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] RVA(0x0041595d, 0x1830)  int townManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)
