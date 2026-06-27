// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\ADVMGR.OBJ   from: (directly linked into exe)
// functions: 92   data: 33
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <rva.h>
RVA(0x00456350, 0x30f)
// void advManager::constructor(void);

RVA(0x0045665f, 0x9c9)
// int advManager::Open(int);   // virtual [override (implements baseManager pure virtual)]

RVA(0x00457028, 0x40a)
// void advManager::Close(void);   // virtual [override (implements baseManager pure virtual)]

RVA(0x00457432, 0xe9)
// void advManager::GetCursorSampleSet(int);

RVA(0x0045751b, 0x6c0)
// class mapCell * advManager::DoAdvCommand(void);

RVA(0x00457bdb, 0x191)
// void advManager::CheckSetEvilInterface(int, int);

RVA(0x00457d6c, 0xfda)
// int advManager::Main(struct tag_message &);   // virtual [override (implements baseManager pure virtual)]

RVA(0x00458d46, 0x22)
// void advManager::Reseed(int, int);

RVA(0x00458d68, 0xeb1)
// int advManager::ProcessSelect(struct tag_message *, class mapCell * *);

RVA(0x00459c19, 0x463)
// int advManager::ProcessDeSelect(struct tag_message *, int *, class mapCell * *);

RVA(0x0045a07c, 0x5c8)
// int advManager::ProcessSearch(int, int);

RVA(0x0045a644, 0xa50)
// int advManager::ProcessHover(int, int);

RVA(0x0045b094, 0x21a)
// void advManager::UpdateScreen(int, int);

RVA(0x0045b2ae, 0x4eb)
// void advManager::CompleteDraw(int, int, int, int);

RVA(0x0045b799, 0x3a)
// void advManager::CompleteDraw(int);

RVA(0x0045b7d3, 0x3a9)
// int advManager::GetCloudLookup(int, int);

RVA(0x0045bb7c, 0x24cb)
// void advManager::DrawCell(int, int, int, int, int, int);

RVA(0x0045e047, 0x93)
// class mapCell * advManager::GetCell(int, int);

RVA(0x0045e0da, 0x104d)
// void advManager::UpdateRadar(int, int);

RVA(0x0045f127, 0x133e)
// void advManager::QuickInfo(int, int);

RVA(0x00460465, 0x348)
// void advManager::UpdateHeroLocator(int, int, int);

RVA(0x004607ad, 0x102)
// void advManager::UpdateHeroLocators(int, int);

RVA(0x004608af, 0x2e8)
// void advManager::UpdateTownLocators(int, int);

RVA(0x00460b97, 0x1cc)
// void advManager::UpdBottomView(int, int, int);

RVA(0x00460d63, 0x132)
// void advManager::ClearBottomView(void);

RVA(0x00460e95, 0x51b)
// int advManager::UpdBottomViewEnemyTurn(void);

RVA(0x004613b0, 0x366)
// int advManager::UpdBottomViewNewTurn(void);

RVA(0x00461716, 0x35f)
// int advManager::UpdBottomViewResMsg(void);

RVA(0x00461a75, 0x363)
// int advManager::UpdBottomViewKingdom(void);

RVA(0x00461dd8, 0x583)
// int advManager::UpdBottomViewHero(void);

RVA(0x0046235b, 0xd32)
// void advManager::HeroQuickView(int, int, int, int);

RVA(0x0046308d, 0x120)
// char * advManager::GetArmySizeName(int, int);

RVA(0x004631ad, 0xc29)
// void advManager::TownQuickView(int, int, int, int);

RVA(0x00463dd6, 0x11f)
// void advManager::RedrawAdvScreen(int, int);

RVA(0x00463ef5, 0x1f)
// void advManager::DeactivateCurrTown(void);

RVA(0x00463f14, 0x27)
// void advManager::DeactivateCurrHero(void);

RVA(0x00463f3b, 0x5a)
// void advManager::MobilizeCurrHero(int);

RVA(0x00463f95, 0x16c)
// void advManager::DemobilizeCurrHero(void);

RVA(0x00464101, 0x217)
// void advManager::SetTownContext(int);

RVA(0x00464318, 0x392)
// void advManager::SetHeroContext(int, int);

RVA(0x004646aa, 0x22f)
// void advManager::DoHeroKnob(void);

RVA(0x004648d9, 0x22f)
// void advManager::DoTownKnob(void);

RVA(0x00464b08, 0x397)
// void advManager::CastSpell(int);

RVA(0x00464e9f, 0x24c)
// int SaveGame(void);

RVA(0x004650eb, 0xa6)
// void advManager::CheckCastSpell(void);

RVA(0x00465191, 0x31c)
// int DimensionDoorHandler(struct tag_message &);

RVA(0x004654ad, 0x11a9)
// int advManager::ComboDraw(int, int, int);

RVA(0x00466656, 0x38)
// int advManager::ComboDraw(int);

RVA(0x0046668e, 0x338)
// void advManager::SetEnvironmentOrigin(int, int, int);

RVA(0x004669c6, 0x69)
// void advManager::CheckLoadSample(int);

RVA(0x00466a2f, 0x4c1)
// int advManager::GetSoundId(int, int);

RVA(0x00466ef0, 0x23a)
// void advManager::InsertSound(int, int, int, int);

RVA(0x0046712a, 0x40f)
// void advManager::TeleportTo(class hero *, int, int, int, int);

RVA(0x00467539, 0x1fb)
// void advManager::DimensionDoor(void);

RVA(0x00467734, 0x129)
// int TownPortalHandler(struct tag_message &);

RVA(0x0046785d, 0x43e)
// void advManager::TownGate(int);

RVA(0x00467c9b, 0x5ac)
// void advManager::SummonBoat(void);

RVA(0x00468247, 0x4d9)
// void advManager::ShowRoute(int, int, int);

RVA(0x00468720, 0x107)
// void advManager::HideRoute(int, int, int);

RVA(0x00468827, 0x8d)
// void advManager::CheckDimHero(void);

RVA(0x004688b4, 0x6b)
// void advManager::CheckDimNextHeroBut(void);

RVA(0x0046891f, 0x138)
// void advManager::SeedTo(int, int);

RVA(0x00468a57, 0x5f)
// void advManager::ForceNewHover(void);

RVA(0x00468ab6, 0x1a6)
// void advManager::ScreenScroll(int, int);

RVA(0x00468c5c, 0x1bb)
// void advManager::CheckScreenScroll(void);

RVA(0x00468e17, 0x91)
// int advManager::MouseInScrollZone(void);

RVA(0x00468ea8, 0x2b8)
// void advManager::SetInitialMapOrigin(void);

RVA(0x00469160, 0x1be)
// void advManager::LoadRemote(void);

RVA(0x0046931e, 0x20c)
// char * advManager::CheckHandleNet(void);

RVA(0x0046952a, 0xcd)
// int advManager::CheckHandleNetPlayerWait(struct tag_message &, int);

RVA(0x004695f7, 0x1d5)
// void advManager::TrimLoopingSounds(int);

RVA(0x004697cc, 0xd5)
// void advManager::DisableButtons(void);

RVA(0x004698a1, 0xd5)
// void advManager::EnableButtons(void);

RVA(0x00469976, 0x145)
// void advManager::SaveAdventureBorder(void);

RVA(0x00469abb, 0x134)
// void advManager::DrawAdventureBorder(void);

RVA(0x00469bef, 0x3d3)
// int advManager::FindAdjacentMonster(int, int, int *, int *, int, int);

RVA(0x00469fc2, 0x125)
// void ComputeAdvNetControl(void);

RVA(0x0046a0e7, 0xf6)
// int MapExtraPosAndAdjacentsSet(int, int, unsigned char);

RVA(0x0046a1dd, 0x4c6)
// void advManager::ViewPuzzle(void);

RVA(0x0046a6a3, 0x81)
// void advManager::PuzzleDraw(int, int, int, int);

RVA(0x0046a724, 0x2ac)
// void advManager::AdvPanel(void);

RVA(0x0046a9d0, 0x1ca)
// int APanelHandler(struct tag_message &);

RVA(0x0046ab9a, 0x1e4)
// int advManager::ControlPanel(void);

RVA(0x0046ad7e, 0x304)
// int CPanelHandler(struct tag_message &);

RVA(0x0046b082, 0x197)
// void advManager::SystemOptions(void);

RVA(0x0046b219, 0x35f)
// void UpdateSystemOptions(int);

RVA(0x0046b578, 0x672)
// int SystemOptionsHandler(struct tag_message &);

RVA(0x0046bbea, 0x7f)
// int GetMobilityFrame(int);

RVA(0x0046bc69, 0x7f)
// int GetManaFrame(int);

RVA(0x0046bce8, 0x559)
// int advManager::DoVisions(class hero *);

RVA(0x0046c241, 0xd7)
// int advManager::IsCrystalBallInEffect(int, int, int);

RVA(0x0046c318, 0x85)
// unsigned char StopOnTrigger(class mapCell *);

// ---- data / globals / vtables ----
DATA(0x004eb6c8)  // const advManager::vftable
DATA(0x004f57b0)  // int giLimitUpdMinX
DATA(0x004f57b4)  // int iLastScrollTime
DATA(0x004f57b8)  // int iSandAnim
DATA(0x004f57bc)  // int giLastHourGlassUpdateTime
DATA(0x004f57c0)  // int TrigX
DATA(0x004f57c4)  // int TrigY
DATA(0x004f57c8)  // int iCurBottomView
DATA(0x004f57cc)  // int iCurBottomViewEnemy
DATA(0x004f57d0)  // int iCurHourGlassPhase
DATA(0x004f57d4)  // int iLastHourGlassPhase
DATA(0x004f57d8)  // int gbForceUpdate
DATA(0x004f59e8)  // int giCheatSeq
DATA(0x004f59ec)  // int iQWE
DATA(0x004f5e38)  // unsigned char * monAnimDrawFrame
DATA(0x004f60e0)  // int iLastSandAnimTime
DATA(0x004f60e4)  // int iLastNewSandAnimTime
DATA(0x004f6720)  // int giFrameCount
DATA(0x00527ec8)  // class heroWindow * cPanel
DATA(0x00527ed0)  // int iThisMaxY
DATA(0x00527edc)  // int giTownPortalChoice
DATA(0x00527ee0)  // int iThisMinY
DATA(0x00527ee8)  // class heroWindow * townPortalWin
DATA(0x00527ef0)  // struct tag_message USMsg
DATA(0x00527f14)  // int giFrameStep
DATA(0x00527f28)  // char * cArmySizeName
DATA(0x00527f34)  // int giLimitUpdMaxX
DATA(0x00527f38)  // int giLimitUpdMaxY
DATA(0x00527f40)  // int bPrefsChanged
DATA(0x00527f4c)  // int giLimitUpdMinY
DATA(0x00527f50)  // signed char (*)[18] bComboDraw
DATA(0x005280b0)  // struct tag_message CDMsg
DATA(0x005280d4)  // int iLastAnimFrame

// ===== vtable advManager : public baseManager  (3 slots) =====
//  [ 0] RVA(0x0045665f, 0x9c9)  int advManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] RVA(0x00457028, 0x40a)  void advManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] RVA(0x00457d6c, 0xfda)  int advManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)
