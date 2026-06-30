// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\GAME.OBJ   from: (directly linked into exe)
// functions: 99   data: 26
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdio.h>
#include <SOURCE/game.h>
#include <SOURCE/playerData.h>
#include <EDITOR/mapcell.h>
#include <BASE/soundManager.h>
#include <BASE/resourceManager.h>
#include <BASE/heroWindowManager.h>

class advManager;

extern soundManager *gpSoundManager;
extern game *gpGame;
extern advManager *gpAdvManager;
extern resourceManager *gpResourceManager;
extern heroWindowManager *gpWindowManager;

extern int Random(int, int);
extern int xPasswordStringsIndex[];
extern int bShowIt;
extern void AiPrint(char *);
extern void IconToBitmap(class icon *, class bitmap *, int, int, int, int, int, int, int, int, int);
void CompressTest2(void);

struct SCreatureInfo {
    unsigned short value;
    char pad[24];
};
extern SCreatureInfo gCreatureInfo[];

struct SThievesData {
    char pad[0x4e3];
    signed char list[8][283];
};

VA(0x004708b0, 0x23d)
// void playerData::Write(int);

VA(0x00470aed, 0x22d)
// void playerData::Read(int);

VA(0x00470d1a, 0x12d)
// int playerData::NextHero(int);

VA(0x00470e47, 0x65)
int playerData::HasMobileHero(void)
{
    int i;
    for (i = 0; i < ((signed char *)this)[1]; i++) {
        if (gpGame->IsMobile(((signed char *)this)[i + 4]))
            return 1;
    }
    return 0;
}

VA(0x00470eac, 0x64)
int GetNumObelisks(int color)
{
    int count = 0;
    int i;
    for (i = 0; i < 0x30; i++) {
        if (((signed char *)gpGame)[i + 0x634d] & (1 << color))
            count++;
    }
    return count;
}

VA(0x00470f10, 0xca)
// int playerData::BuildingsOwned(int, int, int);

VA(0x00470fda, 0x97)
int playerData::NumOfGivenArtifact(int artifact)
{
    int count = 0;
    int i;
    for (i = 0; i < ((signed char *)this)[1]; i++) {
        int j;
        for (j = 0; j < 0xe; j++) {
            if (((signed char *)gpGame)[((signed char *)this)[i + 4] * 250 + j + 0x2899] == artifact)
                count++;
        }
    }
    return count;
}

VA(0x00471071, 0x82)
int game::MineTypesOwned(int col, int row)
{
    int num = 0;
    int i;
    for (i = 0; i < 0x90; i++) {
        if (((signed char *)this)[i * 7 + 0x5cb7] == col &&
            ((signed char *)this)[i * 7 + 0x5cb8] == row)
            num++;
    }
    return num;
}

VA(0x004710f3, 0x40d)
// void ComputeUALoc(int);

VA(0x00471500, 0x2ac)
// int game::SetupPuzzlePieces(int, int);

VA(0x004717ac, 0xb5)
// int game::IsMobile(int);

VA(0x00471861, 0x1e)
fullMap *game::GetWorldMapData(void)
{
    return (fullMap *)((char *)this + 0xb3e);
}

VA(0x0047187f, 0x11e)
// int game::CreateBoat(int, int, int);

VA(0x0047199d, 0x5a)
int game::Scan(signed char *array, int start, int length)
{
    int i;
    for (i = start; i < length + start; i++) {
        if (array[i] == -1)
            return i;
    }
    return -1;
}

VA(0x004719f7, 0x76)
int game::RandomScan(signed char *array, int start, int range, int unused, signed char target)
{
    int idx = target;
    int i;
    for (i = 0; i < 0x2710; i++) {
        idx = start + Random(0, range - 1);
        if (array[idx] == target)
            return idx;
    }
    return -1;
}

VA(0x00471a6d, 0x213)
// int game::GetNewHeroId(int, int, int);

VA(0x00471c80, 0x85)
int game::GetTownId(int col, int row)
{
    int i;
    for (i = 0; i < 0x48; i++) {
        if (((unsigned char *)this)[i * 100 + 0xb57] == col &&
            ((unsigned char *)this)[i * 100 + 0xb58] == row)
            return i;
    }
    return -1;
}

VA(0x00471d05, 0x84)
int game::GetMineId(int col, int row)
{
    int i;
    for (i = 0; i < 0x90; i++) {
        if (((unsigned char *)this)[i * 7 + 0x5cbb] == col &&
            ((unsigned char *)this)[i * 7 + 0x5cbc] == row)
            return i;
    }
    return -1;
}

VA(0x00471d89, 0x12e)
// void GenerateStandardFileName(char *, char *);

VA(0x00471eb7, 0xbc4)
// int game::SaveGame(char *, int, signed char);

VA(0x00472a7b, 0xb44)
// void game::SetupOrigData(void);

VA(0x004735bf, 0xc27)
// void game::LoadGame(char *, int, int);

VA(0x004741e6, 0x3ee)
// void game::GiveTroopsToNeutralTown(int);

VA(0x004745d4, 0xa4)
void game::GiveTroopsToNeutralTowns(void)
{
    int i;
    for (i = 0; i < 0x48; i++) {
        GiveTroopsToNeutralTown(i);
        if (*(int *)((char *)this + i * 100 + 0xb6b) & 0x40) {
            if (Random(0, 100) < 0x50)
                GiveTroopsToNeutralTown(i);
        } else {
            if (Random(0, 100) < 0x28)
                GiveTroopsToNeutralTown(i);
        }
    }
}

VA(0x00474678, 0x1dd0)
// void game::NewMap(char *);

VA(0x00476448, 0x2601)
// void game::RandomizeEvents(void);

VA(0x00478a49, 0xa1)
void game::InitializePasswords(void)
{
    char flag;
    int i;
    int j;
    for (i = 0; i < 8; i++) {
        flag = 0;
        while (flag == 0) {
            xPasswordStringsIndex[i] = Random(0, 0xd2);
            flag = 1;
            for (j = 0; j < i; j++) {
                if (xPasswordStringsIndex[j] == xPasswordStringsIndex[i])
                    flag = 0;
            }
        }
    }
}

VA(0x00478aea, 0x64)
void game::RandomizeBarrier(mapCell *cell)
{
    int idx = cell->w4hi;
    idx &= 7;
    int pass = xPasswordStringsIndex[idx];
    int color = (pass << 3) | idx;
    cell->w4hi = color | 0;
}

VA(0x00478b4e, 0x24)
void game::RandomizePassword(mapCell *cell)
{
    RandomizeBarrier(cell);
}

VA(0x00478b72, 0x478)
// int game::LoadMap(char *);

VA(0x00478fea, 0x3aa)
// void game::ClaimTown(int, int, int);

VA(0x00479394, 0x4c2)
// void game::ClaimMine(int, int);

VA(0x00479856, 0x1e2)
// int game::ViewSpells(class hero *, int, int (*)(struct tag_message &), int);

VA(0x00479a38, 0x403)
// void game::UpdateSpellWidgets(void);

VA(0x00479e3b, 0x692)
// int ViewSpellsHandler(struct tag_message &);

VA(0x0047a4cd, 0x17c)
// int ViewSpecialHandler(struct tag_message &);

VA(0x0047a649, 0xc86)
// void game::ViewArmy(int, int, int, int, class town *, int, int, int, class hero *, class army *, class armyGroup *, int);

VA(0x0047b2cf, 0x3f5)
// int ViewArmyHandler(struct tag_message &);

VA(0x0047b6c4, 0x671)
// int game::GetRandomNumTroops(int);

VA(0x0047bd35, 0x3f)
void game::TurnOnAIMusic(void)
{
    gpSoundManager->StopAllSamples(1);
    gpSoundManager->SwitchAmbientMusic(0x1c);
    *(int *)((char *)gpSoundManager + 0x684) = 0;
}

VA(0x0047bd74, 0x25)
void game::TurnOffAIMusic(void)
{
    *(int *)((char *)gpSoundManager + 0x684) = 1;
}

VA(0x0047bd99, 0x596)
// void game::NextPlayer(void);

VA(0x0047c32f, 0x432)
// int game::ComputeDailyGold(int);

VA(0x0047c761, 0x9aa)
// void game::PerDay(void);

VA(0x0047d10b, 0x199d)
// void game::PerWeek(void);

VA(0x0047eaa8, 0x12d)
// void game::WeeklyRecruitSite(class mapCell *);

VA(0x0047ebd5, 0x6f)
void game::WeeklyGenericSite(mapCell *cell)
{
    int type = cell->w4hi;
    type &= 0x3f;
    switch (type) {
    case 4:
        cell->w4hi = type;
        break;
    }
}

VA(0x0047ec44, 0x375)
// void game::PerMonth(void);

VA(0x0047efb9, 0x476)
// void game::ConvertObject(int, int, int, int, int, int, int, int, int, int, int);

VA(0x0047f42f, 0x1c2)
// void game::RandomizeTown(int, int, int);

VA(0x0047f5f1, 0x619)
// void game::RandomizeMine(int, int);

VA(0x0047fc0a, 0xc6)
// void game::InitRandomArtifacts(void);

VA(0x0047fcd0, 0x17f)
// int game::GetRandomArtifactId(int, int);

VA(0x0047fe4f, 0x68)
int IsCursedItem(int item)
{
    if (item == 0x10 || item == 0x46 || item == 0x45 || item == 0x57 ||
        item == 0x59 || item == 0x5c || item == 0x5d)
        return 1;
    return 0;
}

VA(0x0047feb7, 0x1ef)
// void game::RandomizeHeroPool(void);

VA(0x004800a6, 0x378)
// void game::SetRandomHeroArmies(int, int);

VA(0x0048041e, 0x746)
// void game::ProcessRandomObjects(void);

VA(0x00480b64, 0x230)
// void game::SetVisibility(int, int, int, int);

VA(0x00480d94, 0xd8)
// void game::MakeAllWaterVisible(int);

VA(0x00480e6c, 0xfc)
// void game::GiveArmy(class armyGroup *, int, int, int);

// @early-stop
// ~96%: only the operand-load order of one ((signed char*)group)[i] read differs
// (retail loads i then group; we load group then i) — the movsbl(%eax,%ecx) bytes are
// identical (commutative address), only the two preceding movs swap. Proven to compile
// byte-exact in isolation; the flip is a TU-global eval-order effect of the partial GAME
// TU. Re-check when GAME is fuller.
VA(0x00480f68, 0x91)
int game::ExperienceValueOfStack(armyGroup *group, hero *h)
{
    int exp = 0;
    int i;
    for (i = 0; i < 5; i++) {
        if (((short *)((char *)group + 5))[i] > 0) {
            exp += gCreatureInfo[((signed char *)group)[i]].value * ((short *)((char *)group + 5))[i];
        }
    }
    if (h != 0)
        exp += 0x1f4;
    return exp;
}

VA(0x00480ff9, 0x126)
// int game::GetLuck(class hero *, class army *, class town *);

VA(0x0048111f, 0xf1)
// void game::SetupAdjacentMons(void);

VA(0x00481210, 0x61)
void game::CancelComputerScreen(void)
{
    TurnOffAIMusic();
    bShowIt = 1;
    int i;
    for (i = 1; i <= 6; i++) {
        gpWindowManager->BroadcastMessage(0x200, 6, i, 0x4008);
    }
}

VA(0x00481271, 0xed)
// void game::ShowComputerScreen(void);

VA(0x0048135e, 0xa0)
void game::ShowHeroesLogo(void)
{
    if (*(int *)((char *)gpAdvManager + 0x37a) == 0) {
        *(int *)((char *)gpAdvManager + 0x37a) = 1;
        icon *theIcon = gpResourceManager->GetIcon("herologo.icn");
        IconToBitmap(theIcon, *(bitmap **)((char *)gpWindowManager + 0x46),
                     0x1e0, 0x10, 0, 0, 0, 0, 0x280, 0x1e0, 0);
        gpWindowManager->UpdateScreenRegion(0x1e0, 0x10, 0x90, 0x90);
        gpResourceManager->Dispose((resource *)theIcon);
    }
}

VA(0x004813fe, 0x143)
// void game::WaitForPlayer(char *, int);

VA(0x00481541, 0x104)
// int game::HasLateOverlay(int, int);

VA(0x00481645, 0x120)
// void game::ConvertFlagToLateOverlay(int, int);

VA(0x00481765, 0x13b)
// int game::HasObjectTilesetIndex(int, int, int, int);

VA(0x004818a0, 0x112)
// void game::ConvertAllToLateOverlay(int, int);

VA(0x004819b2, 0x295)
// void game::ProcessMapExtra(void);

VA(0x00481c47, 0x900)
// void game::SetupTowns(void);

VA(0x00482547, 0x774)
// void game::ProcessOnMapHeroes(void);

VA(0x00482cbb, 0x55e)
// void game::CheckHeroConsistency(void);

VA(0x00483219, 0x71e)
// int game::TransmitSaveGame(int, int, int);

VA(0x00483937, 0x68d)
// int game::ReceiveSaveGame(int, int, int, int);

VA(0x00483fc4, 0x455)
// void game::DoNewTurn(void);

VA(0x00484419, 0x58)
int game::GetBoatsBuilt(void)
{
    int count = 0;
    int i;
    for (i = 0; i < 0x30; i++) {
        if (((signed char *)this)[i + 0x631d] != -1)
            count++;
    }
    return count;
}

// @early-stop
// ~92%: only the 8-instr index reduction of the inner town-list read differs. Retail
// fuses i + color*283 into one lea with this-base + disp 0x4e3; MSVC emits that fused
// form only for a pointer-cast ((T(*)[283])P)[c][i], which would fold 0x4e3 into the
// base (disp 0) and break the byte-exact testb $2,mem + downstream. The member-2D form
// here keeps everything else byte-exact and localizes the residual to these 8 bytes.
VA(0x00484471, 0x9c)
int game::GetNumThievesGuilds(int color)
{
    int num = 0;
    int i;
    for (i = 0; i < ((signed char *)this)[color * 283 + 0x4e0]; i++) {
        if (*(int *)((char *)gpGame + ((SThievesData *)this)->list[color][i] * 100 + 0xb6b) & 2)
            num++;
    }
    return num;
}

VA(0x0048450d, 0x113)
// int game::CalcDifficultyRating(void);

VA(0x00484620, 0x1ea)
// int CalcBaseScore(int);

VA(0x0048480a, 0xb5)
// void game::RestoreCell(int, int, int, int, class mapCell *, int);

VA(0x004848bf, 0xe3)
// void game::SetMapSize(int, int);

VA(0x004849a2, 0x100)
// void WriteDiffHeaderInfo(unsigned char, int, unsigned char *, int *);

VA(0x00484aa2, 0xab)
// int GetSkipCopyLen(unsigned char *, int *);

VA(0x00484b4d, 0x5ba)
// void CreateDiffFile(char *, char *, char *, int, int);

VA(0x00485107, 0x3ce)
// void CreateJoinFile(char *, char *, char *);

VA(0x004854d5, 0x5d)
int game::HeroIDToHeroPos(playerData *pd, int heroId)
{
    int i;
    for (i = 0; i < ((signed char *)pd)[1]; i++) {
        if (((signed char *)pd)[i + 4] == heroId)
            return i;
    }
    return -1;
}

VA(0x00485532, 0x5d)
int game::TownIDToTownPos(playerData *pd, int townId)
{
    int i;
    for (i = 0; i < ((signed char *)pd)[0x44]; i++) {
        if (((signed char *)pd)[i + 0x47] == townId)
            return i;
    }
    return -1;
}

VA(0x0048558f, 0x79f)
// void game::SetupNewRumour(void);

VA(0x00485d2e, 0xd9)
// struct EventExtra * GetMapEvent(int, int);

VA(0x00485e07, 0x34c)
// void game::CheckForTimeEvent(void);

VA(0x00486153, 0x143)
// void CheckValidAvailableHeroes(void);

VA(0x00486296, 0xab)
// int CalcFileCRC(char *);

VA(0x00486341, 0x153)
// void CompressTest2(void);

VA(0x00486494, 0x1be)
// void CompressTest(void);

VA(0x00486652, 0x53)
void CompressTest3(void)
{
    char buf[40];
    int i;
    for (i = 0; i < 0x64; i++) {
        sprintf(buf, "Test # %d", i);
        AiPrint(buf);
        CompressTest2();
    }
}

VA(0x004866a5, 0x119)
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
