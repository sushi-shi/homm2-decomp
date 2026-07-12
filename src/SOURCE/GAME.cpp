// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\GAME.OBJ   from: (directly linked into exe)
// functions: 99   data: 26
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/GAME.h>
#include <_types.h>
#include <_globals_model.h>
#include <BASE/Icon2b.h>
#include <BASE/Misc.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/FINDPATH.h>
#include <SOURCE/KB.h>
#include <SOURCE/REMOTE.h>
#include <io.h>
#include <string.h>
#include <stdio.h>
#include <SOURCE/game.h>
#include <SOURCE/playerData.h>
#include <SOURCE/town.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/advManager.h>
#include <SOURCE/armyGroup.h>
#include <EDITOR/mapcell.h>
#include <EDITOR/fullMap.h>
#include <BASE/soundManager.h>
#include <BASE/resourceManager.h>
#include <BASE/heroWindowManager.h>


// configStruct/SCreatureInfo shared in _types.h; EventExtra/SThievesData in SOURCE/GAME.h.

// GAME's BaseAlloc/BaseFree pass __FILE__ + a source line number. The retail
// encodes the base line as a 2-byte string read via movswl, then adds a per-call
// delta; reproduce byte-exactly. __FILE__ is the original build path (reloc-masked).
#define GFILE ((char *)"I:\\Projects\\Heroes\\Prog\\SOURCE\\GAME.CPP")

// fullMap is embedded in `game` at this+0xb3e; the retail folds the +0xb3e into the
// member offsets and inlines Row/Extra (/Ob1), so access it inline via the cast.
#define WORLDMAP ((fullMap *)((char *)this + 0xb3e))

// Inline accessors that reference gpGame directly (the retail emits `add [gpGame]`
// + a per-call `jmp $+0`), so they are free inline helpers, not game methods.
inline town *GetCastle(int idx) { return (town *)((char *)gpGame + idx * 100 + 0xb53); }
inline signed char PlayerEventByte(signed char color) { return ((signed char *)gpGame)[color * 283 + 0x49c]; }

VA(0x004708b0, 0x23d)
void playerData::Write(int) {}

VA(0x00470aed, 0x22d)
void playerData::Read(int) {}

VA(0x00470d1a, 0x12d)
int playerData::NextHero(int) { return 0; }

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
int playerData::BuildingsOwned(int a, int b, int c)
{
    int count = 0;
    int i;
    for (i = 0; i < ((signed char *)this)[0x44]; i++) {
        signed char *town = (signed char *)((char *)gpGame + ((signed char *)this)[i + 0x47] * 100 + 0xb53);
        if (b < 0x13 || town[3] == a) {
            if (b == 0) {
                if (*(int *)(town + 0x18) & 1) {
                    if (town[0x1c] == c)
                        count++;
                }
            } else {
                if (*(int *)(town + 0x18) & (1 << b))
                    count++;
            }
        }
    }
    return count;
}

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
void ComputeUALoc(int) {}

VA(0x00471500, 0x2ac)
int game::SetupPuzzlePieces(int, int) { return 0; }

VA(0x004717ac, 0xb5)
int game::IsMobile(int heroId)
{
    if (heroId == -1)
        return 0;
    char *hp = (char *)this + heroId * 250 + 0x27c4;
    mapCell *cp = gpAdvManager->GetCell(*(int *)(hp + 0x19), *(int *)(hp + 0x1d));
    return CalcTerrainCost(giGroundToTerrain[cp->tile], 1, *(int *)(hp + 0x35),
                           ((signed char *)hp)[0x74], cp->objFlag1, 0) <= *(int *)(hp + 0x35);
}

VA(0x00471861, 0x1e)
fullMap *game::GetWorldMapData(void)
{
    return (fullMap *)((char *)this + 0xb3e);
}

VA(0x0047187f, 0x11e)
int game::CreateBoat(int x, int y, int notify)
{
    int boatIdx = Scan((signed char *)this + 0x631d, 0, 0x30);
    if (boatIdx != -1) {
        if (notify == 0)
            SendMapChange(4, 0, x, y, -999, 0, 0);
        ((char *)this)[boatIdx + 0x631d] = (char)boatIdx;
        char *boat = (char *)this + boatIdx * 8 + 0x619d;
        boat[0] = (char)boatIdx;
        boat[1] = (char)x;
        boat[2] = (char)y;
        boat[3] = 2;
        boat[7] = (char)giCurPlayer;
        mapCell *cell = WORLDMAP->Row(y) + x;
        boat[4] = ((char *)cell)[9];
        boat[5] = (char)cell->w4hi;
        ((char *)cell)[9] = (char)0xab;
        cell->w4hi = boatIdx;
    }
    return boatIdx;
}

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
int game::GetNewHeroId(int, int, int) { return 0; }

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
void GenerateStandardFileName(char *, char *) {}

VA(0x00471eb7, 0xbc4)
int game::SaveGame(char *, int, signed char) { return 0; }

VA(0x00472a7b, 0xb44)
void game::SetupOrigData(void) {}

VA(0x004735bf, 0xc27)
void game::LoadGame(char *, int, int) {}

VA(0x004741e6, 0x3ee)
void game::GiveTroopsToNeutralTown(int) {}

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
void game::NewMap(char *) {}

VA(0x00476448, 0x2601)
void game::RandomizeEvents(void) {}

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
int game::LoadMap(char *) { return 0; }

VA(0x00478fea, 0x3aa)
void game::ClaimTown(int, int, int) {}

VA(0x00479394, 0x4c2)
void game::ClaimMine(int, int) {}

VA(0x00479856, 0x1e2)
int game::ViewSpells(class hero *, int, int (*)(struct tag_message &), int) { return 0; }

VA(0x00479a38, 0x403)
void game::UpdateSpellWidgets(void) {}

VA(0x00479e3b, 0x692)
int ViewSpellsHandler(struct tag_message &) { return 0; }

VA(0x0047a4cd, 0x17c)
int ViewSpecialHandler(struct tag_message &) { return 0; }

VA(0x0047a649, 0xc86)
void game::ViewArmy(int, int, int, int, class town *, int, int, int, class hero *, class army *, class armyGroup *, int) {}

VA(0x0047b2cf, 0x3f5)
int ViewArmyHandler(struct tag_message &) { return 0; }

VA(0x0047b6c4, 0x671)
int game::GetRandomNumTroops(int) { return 0; }

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
void game::NextPlayer(void) {}

VA(0x0047c32f, 0x432)
int game::ComputeDailyGold(int) { return 0; }

VA(0x0047c761, 0x9aa)
void game::PerDay(void) {}

VA(0x0047d10b, 0x199d)
void game::PerWeek(void) {}

VA(0x0047eaa8, 0x12d)
void game::WeeklyRecruitSite(class mapCell *) {}

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
void game::PerMonth(void) {}

VA(0x0047efb9, 0x476)
void game::ConvertObject(int, int, int, int, int, int, int, int, int, int, int) {}

VA(0x0047f42f, 0x1c2)
void game::RandomizeTown(int, int, int) {}

VA(0x0047f5f1, 0x619)
void game::RandomizeMine(int, int) {}

VA(0x0047fc0a, 0xc6)
void game::InitRandomArtifacts(void)
{
    int xx;
    memset((char *)this + 0x6136, 0, 0x67);
    int x;
    for (x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            mapCell *cell = WORLDMAP->Row(y) + x;
            if (((unsigned char *)cell)[9] == 0xa9)
                ((char *)this)[0x6136 + (((unsigned char *)cell)[3] >> 1)] = 1;
        }
    }
}

VA(0x0047fcd0, 0x17f)
int game::GetRandomArtifactId(int, int) { return 0; }

VA(0x0047fe4f, 0x68)
int IsCursedItem(int item)
{
    if (item == 0x10 || item == 0x46 || item == 0x45 || item == 0x57 ||
        item == 0x59 || item == 0x5c || item == 0x5d)
        return 1;
    return 0;
}

VA(0x0047feb7, 0x1ef)
void game::RandomizeHeroPool(void) {}

VA(0x004800a6, 0x378)
void game::SetRandomHeroArmies(int, int) {}

VA(0x0048041e, 0x746)
void game::ProcessRandomObjects(void) {}

VA(0x00480b64, 0x230)
void game::SetVisibility(int, int, int, int) {}

// @early-stop
// Logic + frame slots byte-exact; residual is 3 commutative operand-load swaps (the
// inner-loop test y<MAP_HEIGHT and the two y*MAP_WIDTH index multiplies load the OTHER
// operand into eax first). Not source-steerable (operand order / reversed compare /
// extra temp all tested - no effect): it is the TU-cumulative /Od eval-order parity of
// the partial GAME TU (most preceding functions are still placeholders, so the temp
// counter is off from retail). Same class as the ExperienceValueOfStack @early-stop;
// aligns when GAME is fuller.
VA(0x00480d94, 0xd8)
void game::MakeAllWaterVisible(int player)
{
    char mask = (char)(1 << player);
    int x;
    int y;
    for (x = 0; x < MAP_WIDTH; x++) {
        for (y = 0; y < MAP_HEIGHT; y++) {
            if (giGroundToTerrain[WORLDMAP->Row(y)[x].tile] == 0)
                mapExtra[y * MAP_WIDTH + x] |= mask;
        }
    }
}

VA(0x00480e6c, 0xfc)
void game::GiveArmy(armyGroup *group, int type, int count, int slot)
{
    int tmp;
    int i;
    if (slot >= 0) {
        i = slot;
        ((char *)group)[i] = (char)type;
        i[(short *)((char *)group + 5)] = 0;
    } else {
        for (i = 0; i < 5; i++) {
            if (((signed char *)group)[i] == type)
                break;
        }
        if (i >= 5) {
            for (i = 0; i < 5; i++) {
                if (((signed char *)group)[i] < 0) {
                    i[(short *)((char *)group + 5)] = 0;
                    break;
                }
            }
        }
        if (i >= 5)
            return;
    }
    ((char *)group)[i] = (char)type;
    i[(short *)((char *)group + 5)] += count;
}

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
int game::GetLuck(class hero *, class army *, class town *) { return 0; }

// @early-stop
// Logic + frame slots byte-exact (col/row/mask + nested x/y land on retail's -0x4..-0x14
// via the {} block); residual is the same TU-cumulative /Od eval-order parity as
// MakeAllWaterVisible - the inner-loop test and the y*MAP_WIDTH multiplies load the other
// operand first. Aligns when GAME is fuller.
VA(0x0048111f, 0xf1)
void game::SetupAdjacentMons(void)
{
    int col;
    int row;
    unsigned char mask = 0x7f;
    {
        int x;
        int y;
        for (x = 0; x < MAP_WIDTH; x++) {
            for (y = 0; y < MAP_HEIGHT; y++) {
                if (gpAdvManager->FindAdjacentMonster(x, y, &col, &row, -1, -1))
                    mapExtra[y * MAP_WIDTH + x] |= 0x80;
                else
                    mapExtra[y * MAP_WIDTH + x] &= mask;
            }
        }
    }
}

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
void game::ShowComputerScreen(void)
{
    if (gConfig.blackoutComputer) {
        int saved = gbThisNetHumanPlayer[giCurPlayer];
        gbThisNetHumanPlayer[giCurPlayer] = 1;
        int i;
        for (i = 1; i <= 6; i++)
            gpWindowManager->BroadcastMessage(0x200, 5, i, 0x4008);
        gbAllBlack = 1;
        gpAdvManager->CompleteDraw(1);
        gpAdvManager->UpdateHeroLocators(1, 1);
        gpAdvManager->UpdateTownLocators(1, 1);
        gpAdvManager->UpdBottomView(1, 1, 1);
        gpAdvManager->UpdateScreen(0, 1);
        gbAllBlack = 0;
        gbThisNetHumanPlayer[giCurPlayer] = (signed char)saved;
    }
    ShowHeroesLogo();
}

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
void game::WaitForPlayer(char *, int) {}

// @early-stop
// Computation byte-exact; residual is 2 inline-accessor jmp$+0 brackets the /Ob1
// expander places leading (after the ternary test) where retail places them trailing
// (after the Extra() body) - the documented /Od /Ob1 block-boundary artifact, identical
// in kind to EDITOR/mapcell GetNewCellExtra*. See docs/patterns/inline-accessors.md.
VA(0x00481541, 0x104)
int game::HasLateOverlay(int col, int row)
{
    mapCell *cell = WORLDMAP->Row(row) + col;
    if (cell->ovlFlag1)
        return 1;
    mapCellExtra *extra = cell->extra ? WORLDMAP->Extra(cell->extra) : 0;
    while (extra) {
        if (extra->ovlFlag1)
            return 1;
        extra = extra->index ? WORLDMAP->Extra(extra->index) : 0;
    }
    return 0;
}

VA(0x00481645, 0x120)
void game::ConvertFlagToLateOverlay(int, int) {}

VA(0x00481765, 0x13b)
int game::HasObjectTilesetIndex(int, int, int, int) { return 0; }

// @early-stop
// Twin of HasLateOverlay: computation byte-exact; only the 2 inline-accessor jmp$+0
// brackets (Extra()) are placed leading vs retail's trailing - the same /Od /Ob1
// block-boundary artifact. See docs/patterns/inline-accessors.md.
VA(0x004818a0, 0x112)
void game::ConvertAllToLateOverlay(int col, int row)
{
    mapCell *cell = WORLDMAP->Row(row) + col;
    if (cell->ovlIndex != 0xff)
        cell->ovlFlag1 = 1;
    mapCellExtra *extra = cell->extra ? WORLDMAP->Extra(cell->extra) : 0;
    while (extra) {
        if (extra->ovlIndex != 0xff)
            extra->ovlFlag1 = 1;
        extra = extra->index ? WORLDMAP->Extra(extra->index) : 0;
    }
}

VA(0x004819b2, 0x295)
void game::ProcessMapExtra(void) {}

VA(0x00481c47, 0x900)
void game::SetupTowns(void) {}

VA(0x00482547, 0x774)
void game::ProcessOnMapHeroes(void) {}

VA(0x00482cbb, 0x55e)
void game::CheckHeroConsistency(void) {}

VA(0x00483219, 0x71e)
int game::TransmitSaveGame(int, int, int) { return 0; }

VA(0x00483937, 0x68d)
int game::ReceiveSaveGame(int, int, int, int) { return 0; }

VA(0x00483fc4, 0x455)
void game::DoNewTurn(void) {}

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
int game::CalcDifficultyRating(void)
{
    int notused;
    int rating = 0;
    if (((signed char *)this)[0x465] == 0)
        rating += 0x32;
    else if (((signed char *)this)[0x465] == 1)
        rating += 0x50;
    else if (((signed char *)this)[0x465] == 2)
        rating += 0x64;
    else if (((signed char *)this)[0x465] == 3)
        rating += 0x78;
    else if (((signed char *)this)[0x465] == 4)
        rating += 0x8c;
    if (((unsigned char *)this)[0x2ad] == 0)
        ;
    else if (((unsigned char *)this)[0x2ad] == 1)
        rating += 0x14;
    else if (((unsigned char *)this)[0x2ad] == 2)
        rating += 0x28;
    else if (((unsigned char *)this)[0x2ad] == 3)
        rating += 0x50;
    return rating;
}

VA(0x00484620, 0x1ea)
int CalcBaseScore(int) { return 0; }

// @early-stop
// ~99.9%: logic byte-exact (matched 100% standalone); tiny residual is the same
// TU-cumulative /Od operand-load-order parity as the other GAME parks — should resolve
// as the surrounding GAME functions are reconstructed.
VA(0x0048480a, 0xb5)
void game::RestoreCell(int x, int y, int obj, int barrier, mapCell *passedCell, int p6)
{
    mapCell *cell;
    if (passedCell)
        cell = passedCell;
    else
        cell = gpAdvManager->GetCell(x, y);
    if (y > 0 && obj == 0xa3 &&
        ((unsigned char *)gpAdvManager->GetCell(x, y - 1))[9] != 0x23) {
        ((char *)cell)[9] = 0;
        cell->w4hi = 0;
    } else {
        ((char *)cell)[9] = (char)obj;
        cell->w4hi = barrier;
    }
}

// @early-stop
// Condition (3-term &&), reinit, and realloc (BaseFree/BaseAlloc/memset) all byte-exact;
// residual is 2 redundant jumps retail /Od emits for the empty then-branch of the
// if/else - an end-of-function trampoline (jmp $+0 class) plus a dead `jmp realloc` -
// that my build collapses to one direct jmp. A /Od jump-layout artifact of the empty
// then; not behaviorally meaningful.
VA(0x004848bf, 0xe3)
void game::SetMapSize(int w, int h)
{
    if (MAP_HEIGHT == h && MAP_WIDTH == w && bMapInitialized) {
    } else {
        bMapInitialized = 1;
        MAP_WIDTH = w;
        MAP_HEIGHT = h;
        gpSearchArray->Init();
    }
    if (mapExtra)
        BaseFree(mapExtra, GFILE, *(short *)"\x0d\x1d" + 0xc);
    mapExtra = (unsigned char *)BaseAlloc(MAP_WIDTH * MAP_HEIGHT, GFILE, *(short *)"\x0d\x1d" + 0xd);
    memset(mapExtra, 0, MAP_WIDTH * MAP_HEIGHT);
}

// @early-stop
// Logic + frame slots byte-exact; residual is the operand-eval order of the two
// `flags |= <extracted len bits>` ORs: retail loads `flags` into al first (then keeps
// len in ecx and pulls the shifted byte via ch), my build evaluates the value first and
// ORs flags from memory. Same TU-cumulative /Od eval-order parity as MakeAllWaterVisible;
// not source-steerable. Aligns when GAME is fuller.
VA(0x004849a2, 0x100)
void WriteDiffHeaderInfo(unsigned char cmd, int len, unsigned char *buf, int *pos)
{
    unsigned char flags = 0;
    flags = (cmd << 7) | flags;
    if (len > 0x1fff) {
        flags |= 0x40;
        flags |= (len & 0x2f0000) >> 16;
        unsigned short word = (unsigned short)(len & 0xffff);
        buf[*pos] = flags;
        *(unsigned short *)(buf + *pos + 1) = word;
        *pos += 3;
    } else if (len > 0x1f) {
        flags |= 0x20;
        flags |= (len >> 8) & 0x1f;
        unsigned char lo = (unsigned char)len;
        buf[*pos] = flags;
        buf[*pos + 1] = lo;
        *pos += 2;
    } else {
        flags |= (unsigned char)len;
        buf[*pos] = flags;
        (*pos)++;
    }
}

VA(0x00484aa2, 0xab)
int GetSkipCopyLen(unsigned char *buf, int *pos)
{
    unsigned char b = buf[*pos];
    int len;
    if (b & 0x40) {
        len = b & 0x3f;
        len <<= 16;
        len |= *(unsigned short *)(buf + *pos + 1);
        *pos += 3;
    } else if (b & 0x20) {
        len = b & 0x1f;
        len <<= 8;
        len |= buf[*pos + 1];
        *pos += 2;
    } else {
        len = b & 0x1f;
        (*pos)++;
    }
    return len;
}

VA(0x00484b4d, 0x5ba)
void CreateDiffFile(char *, char *, char *, int, int) {}

VA(0x00485107, 0x3ce)
void CreateJoinFile(char *, char *, char *) {}

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
void game::SetupNewRumour(void) {}

VA(0x00485d2e, 0xd9)
EventExtra *GetMapEvent(int x, int y)
{
    int i;
    for (i = 0; i < *(unsigned short *)((char *)gpGame + 0x657b); i++) {
        EventExtra *ev = (EventExtra *)ppMapExtra[*(unsigned short *)((char *)gpGame + 0x657d + i * 2)];
        if (*(unsigned short *)((char *)ev + 0x26) == x &&
            *(unsigned short *)((char *)ev + 0x28) == y &&
            ((signed char *)ev)[0x25] != 0 &&
            ((unsigned char *)ev)[0x2b + PlayerEventByte(giCurPlayer)] != 0)
            return ev;
    }
    return 0;
}

VA(0x00485e07, 0x34c)
void game::CheckForTimeEvent(void) {}

VA(0x00486153, 0x143)
void CheckValidAvailableHeroes(void) {}

VA(0x00486296, 0xab)
int CalcFileCRC(char *filename)
{
    long size = FileSize(filename);
    char *block = (char *)BaseAlloc(size, GFILE, *(short *)"\x5e\x1f" + 3);
    int hand = _open(filename, 0x8000);
    if (hand == -1)
        FileError(filename);
    _read(hand, block, size);
    int crc = calc_crc_long((unsigned char *)block, size);
    _close(hand);
    BaseFree(block, GFILE, *(short *)"\x5e\x1f" + 0xe);
    return crc;
}

VA(0x00486341, 0x153)
void CompressTest2(void) {}

VA(0x00486494, 0x1be)
void CompressTest(void) {}

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

// @early-stop
// Logic + frame slots byte-exact (count/cell function-scope, col/row/castle in the {}
// block -> retail's -0x4..-0x14); residual is the same TU-cumulative /Od eval-order
// parity - both loop tests (row<MAP_HEIGHT, col<MAP_WIDTH) load the global into eax
// first in retail vs the loop var in my build. Aligns when GAME is fuller.
VA(0x004866a5, 0x119)
int game::CountShrines(int player)
{
    if (xIsExpansionMap == 0)
        return 0;
    int count = 0;
    mapCell *cell;
    {
        int col;
        int row;
        town *castle;
        for (row = 0; row < MAP_HEIGHT; row++) {
            for (col = 0; col < MAP_WIDTH; col++) {
                cell = WORLDMAP->Row(row) + col;
                if (((unsigned char *)cell)[9] == 0xa3) {
                    castle = GetCastle(cell->w4hi);
                    if (((signed char *)castle)[1] == player &&
                        (*(int *)((char *)castle + 0x18) & 4) &&
                        ((signed char *)castle)[3] == 5)
                        count++;
                }
            }
        }
    }
    return count;
}

// ---- globals (definitions, RVA order) ----
DATA(0x004f70e0) int gbGameOver;
DATA(0x004f7550) signed char *giMonType;
DATA(0x004f7a08) char bMapInitialized;
DATA(0x005280e8) int iViewArmyNumTroops;
DATA(0x005280ec) signed char *gbNGHeroType;
DATA(0x00528430) short giUABaseX;
DATA(0x00528434) short giUABaseY;
DATA(0x00528438) int giEndSequence;
DATA(0x0052843c) int gbDismissArmy;
DATA(0x00528440) signed char *gbNGHuman;
DATA(0x00528448) int iViewArmyFrame;
DATA(0x0052844c) int gbAllowUpgrade;
DATA(0x00528450) int iViewArmyType;
DATA(0x00528454) class hero *viewSpellsHero;
DATA(0x00528458) int gbUpgradeArmy;
DATA(0x00528460) short *RandMineQty;
DATA(0x00528470) char *gcCurMapName;
DATA(0x00528480) signed char *gbNGDifficulty;
DATA(0x00528488) int iViewArmyUpgradeToType;
DATA(0x0052848c) int viewArmyBaseX;
DATA(0x00528490) int viewArmyBaseY;
DATA(0x00528498) signed char *gbNGColor;
DATA(0x005284a0) short giUARadius;
DATA(0x005284a8) signed char *gbNGPlayerPos;
DATA(0x005284b0) int viewArmyFacingWIPXMod;
