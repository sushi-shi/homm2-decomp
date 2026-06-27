// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\GAME.OBJ   from: (directly linked into exe)
// functions: 99   data: 26
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <rva.h>
RVA(0x004708b0, 0x23d)
// void playerData::Write(int);

RVA(0x00470aed, 0x22d)
// void playerData::Read(int);

RVA(0x00470d1a, 0x12d)
// int playerData::NextHero(int);

RVA(0x00470e47, 0x65)
// int playerData::HasMobileHero(void);

RVA(0x00470eac, 0x64)
// int GetNumObelisks(int);

RVA(0x00470f10, 0xca)
// int playerData::BuildingsOwned(int, int, int);

RVA(0x00470fda, 0x97)
// int playerData::NumOfGivenArtifact(int);

RVA(0x00471071, 0x82)
// int game::MineTypesOwned(int, int);

RVA(0x004710f3, 0x40d)
// void ComputeUALoc(int);

RVA(0x00471500, 0x2ac)
// int game::SetupPuzzlePieces(int, int);

RVA(0x004717ac, 0xb5)
// int game::IsMobile(int);

RVA(0x00471861, 0x1e)
// class fullMap * game::GetWorldMapData(void);

RVA(0x0047187f, 0x11e)
// int game::CreateBoat(int, int, int);

RVA(0x0047199d, 0x5a)
// int game::Scan(signed char *, int, int);

RVA(0x004719f7, 0x76)
// int game::RandomScan(signed char *, int, int, int, signed char);

RVA(0x00471a6d, 0x213)
// int game::GetNewHeroId(int, int, int);

RVA(0x00471c80, 0x85)
// int game::GetTownId(int, int);

RVA(0x00471d05, 0x84)
// int game::GetMineId(int, int);

RVA(0x00471d89, 0x12e)
// void GenerateStandardFileName(char *, char *);

RVA(0x00471eb7, 0xbc4)
// int game::SaveGame(char *, int, signed char);

RVA(0x00472a7b, 0xb44)
// void game::SetupOrigData(void);

RVA(0x004735bf, 0xc27)
// void game::LoadGame(char *, int, int);

RVA(0x004741e6, 0x3ee)
// void game::GiveTroopsToNeutralTown(int);

RVA(0x004745d4, 0xa4)
// void game::GiveTroopsToNeutralTowns(void);

RVA(0x00474678, 0x1dd0)
// void game::NewMap(char *);

RVA(0x00476448, 0x2601)
// void game::RandomizeEvents(void);

RVA(0x00478a49, 0xa1)
// void game::InitializePasswords(void);

RVA(0x00478aea, 0x64)
// void game::RandomizeBarrier(class mapCell *);

RVA(0x00478b4e, 0x24)
// void game::RandomizePassword(class mapCell *);

RVA(0x00478b72, 0x478)
// int game::LoadMap(char *);

RVA(0x00478fea, 0x3aa)
// void game::ClaimTown(int, int, int);

RVA(0x00479394, 0x4c2)
// void game::ClaimMine(int, int);

RVA(0x00479856, 0x1e2)
// int game::ViewSpells(class hero *, int, int (*)(struct tag_message &), int);

RVA(0x00479a38, 0x403)
// void game::UpdateSpellWidgets(void);

RVA(0x00479e3b, 0x692)
// int ViewSpellsHandler(struct tag_message &);

RVA(0x0047a4cd, 0x17c)
// int ViewSpecialHandler(struct tag_message &);

RVA(0x0047a649, 0xc86)
// void game::ViewArmy(int, int, int, int, class town *, int, int, int, class hero *, class army *, class armyGroup *, int);

RVA(0x0047b2cf, 0x3f5)
// int ViewArmyHandler(struct tag_message &);

RVA(0x0047b6c4, 0x671)
// int game::GetRandomNumTroops(int);

RVA(0x0047bd35, 0x3f)
// void game::TurnOnAIMusic(void);

RVA(0x0047bd74, 0x25)
// void game::TurnOffAIMusic(void);

RVA(0x0047bd99, 0x596)
// void game::NextPlayer(void);

RVA(0x0047c32f, 0x432)
// int game::ComputeDailyGold(int);

RVA(0x0047c761, 0x9aa)
// void game::PerDay(void);

RVA(0x0047d10b, 0x199d)
// void game::PerWeek(void);

RVA(0x0047eaa8, 0x12d)
// void game::WeeklyRecruitSite(class mapCell *);

RVA(0x0047ebd5, 0x6f)
// void game::WeeklyGenericSite(class mapCell *);

RVA(0x0047ec44, 0x375)
// void game::PerMonth(void);

RVA(0x0047efb9, 0x476)
// void game::ConvertObject(int, int, int, int, int, int, int, int, int, int, int);

RVA(0x0047f42f, 0x1c2)
// void game::RandomizeTown(int, int, int);

RVA(0x0047f5f1, 0x619)
// void game::RandomizeMine(int, int);

RVA(0x0047fc0a, 0xc6)
// void game::InitRandomArtifacts(void);

RVA(0x0047fcd0, 0x17f)
// int game::GetRandomArtifactId(int, int);

RVA(0x0047fe4f, 0x68)
// int IsCursedItem(int);

RVA(0x0047feb7, 0x1ef)
// void game::RandomizeHeroPool(void);

RVA(0x004800a6, 0x378)
// void game::SetRandomHeroArmies(int, int);

RVA(0x0048041e, 0x746)
// void game::ProcessRandomObjects(void);

RVA(0x00480b64, 0x230)
// void game::SetVisibility(int, int, int, int);

RVA(0x00480d94, 0xd8)
// void game::MakeAllWaterVisible(int);

RVA(0x00480e6c, 0xfc)
// void game::GiveArmy(class armyGroup *, int, int, int);

RVA(0x00480f68, 0x91)
// int game::ExperienceValueOfStack(class armyGroup *, class hero *);

RVA(0x00480ff9, 0x126)
// int game::GetLuck(class hero *, class army *, class town *);

RVA(0x0048111f, 0xf1)
// void game::SetupAdjacentMons(void);

RVA(0x00481210, 0x61)
// void game::CancelComputerScreen(void);

RVA(0x00481271, 0xed)
// void game::ShowComputerScreen(void);

RVA(0x0048135e, 0xa0)
// void game::ShowHeroesLogo(void);

RVA(0x004813fe, 0x143)
// void game::WaitForPlayer(char *, int);

RVA(0x00481541, 0x104)
// int game::HasLateOverlay(int, int);

RVA(0x00481645, 0x120)
// void game::ConvertFlagToLateOverlay(int, int);

RVA(0x00481765, 0x13b)
// int game::HasObjectTilesetIndex(int, int, int, int);

RVA(0x004818a0, 0x112)
// void game::ConvertAllToLateOverlay(int, int);

RVA(0x004819b2, 0x295)
// void game::ProcessMapExtra(void);

RVA(0x00481c47, 0x900)
// void game::SetupTowns(void);

RVA(0x00482547, 0x774)
// void game::ProcessOnMapHeroes(void);

RVA(0x00482cbb, 0x55e)
// void game::CheckHeroConsistency(void);

RVA(0x00483219, 0x71e)
// int game::TransmitSaveGame(int, int, int);

RVA(0x00483937, 0x68d)
// int game::ReceiveSaveGame(int, int, int, int);

RVA(0x00483fc4, 0x455)
// void game::DoNewTurn(void);

RVA(0x00484419, 0x58)
// int game::GetBoatsBuilt(void);

RVA(0x00484471, 0x9c)
// int game::GetNumThievesGuilds(int);

RVA(0x0048450d, 0x113)
// int game::CalcDifficultyRating(void);

RVA(0x00484620, 0x1ea)
// int CalcBaseScore(int);

RVA(0x0048480a, 0xb5)
// void game::RestoreCell(int, int, int, int, class mapCell *, int);

RVA(0x004848bf, 0xe3)
// void game::SetMapSize(int, int);

RVA(0x004849a2, 0x100)
// void WriteDiffHeaderInfo(unsigned char, int, unsigned char *, int *);

RVA(0x00484aa2, 0xab)
// int GetSkipCopyLen(unsigned char *, int *);

RVA(0x00484b4d, 0x5ba)
// void CreateDiffFile(char *, char *, char *, int, int);

RVA(0x00485107, 0x3ce)
// void CreateJoinFile(char *, char *, char *);

RVA(0x004854d5, 0x5d)
// int game::HeroIDToHeroPos(class playerData *, int);

RVA(0x00485532, 0x5d)
// int game::TownIDToTownPos(class playerData *, int);

RVA(0x0048558f, 0x79f)
// void game::SetupNewRumour(void);

RVA(0x00485d2e, 0xd9)
// struct EventExtra * GetMapEvent(int, int);

RVA(0x00485e07, 0x34c)
// void game::CheckForTimeEvent(void);

RVA(0x00486153, 0x143)
// void CheckValidAvailableHeroes(void);

RVA(0x00486296, 0xab)
// int CalcFileCRC(char *);

RVA(0x00486341, 0x153)
// void CompressTest2(void);

RVA(0x00486494, 0x1be)
// void CompressTest(void);

RVA(0x00486652, 0x53)
// void CompressTest3(void);

RVA(0x004866a5, 0x119)
// int game::CountShrines(int);

// ---- data / globals / vtables ----
DATA(0x004f70e0)  // int gbGameOver
DATA(0x004f7550)  // signed char * giMonType
DATA(0x004f7a08)  // char bMapInitialized
DATA(0x005280e8)  // int iViewArmyNumTroops
DATA(0x005280ec)  // signed char * gbNGHeroType
DATA(0x005280f8)  // struct SMonFrameInfo sViewArmyMonFrameInfo
DATA(0x00528430)  // short int giUABaseX
DATA(0x00528434)  // short int giUABaseY
DATA(0x00528438)  // int giEndSequence
DATA(0x0052843c)  // int gbDismissArmy
DATA(0x00528440)  // signed char * gbNGHuman
DATA(0x00528448)  // int iViewArmyFrame
DATA(0x0052844c)  // int gbAllowUpgrade
DATA(0x00528450)  // int iViewArmyType
DATA(0x00528454)  // class hero * viewSpellsHero
DATA(0x00528458)  // int gbUpgradeArmy
DATA(0x00528460)  // short int * RandMineQty
DATA(0x00528470)  // char * gcCurMapName
DATA(0x00528480)  // signed char * gbNGDifficulty
DATA(0x00528488)  // int iViewArmyUpgradeToType
DATA(0x0052848c)  // int viewArmyBaseX
DATA(0x00528490)  // int viewArmyBaseY
DATA(0x00528498)  // signed char * gbNGColor
DATA(0x005284a0)  // short int giUARadius
DATA(0x005284a8)  // signed char * gbNGPlayerPos
DATA(0x005284b0)  // int viewArmyFacingWIPXMod
