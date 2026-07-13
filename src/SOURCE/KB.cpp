// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\KB.OBJ   from: (directly linked into exe)
// functions: 71   data: 441
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/town.h>
#include <SOURCE/townManager.h>
#include <BASE/executive.h>
#include <BASE/mouseManager.h>
#include <SOURCE/game.h>
#include <BASE/Misc.h>
#include <BASE/WINMGR.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/HERO.h>
#include <SOURCE/KB.h>
#include <SOURCE/Modem.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/Netbios.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/SMACKMGR.h>
#include <SOURCE/Wsnetwin.h>
#include <SOURCE/dpnetwin.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/wingraph.h>
#include <BASE/BITS.h>
#include <SOURCE/KB.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <SOURCE/advManager.h>
#include <SOURCE/hero.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/resourceManager.h>
#include <BASE/soundManager.h>
#include <BASE/icon.h>

// Types now from headers: game/mouseManager/townManager/town/executive + combatManager/
// palette/font -> _all.h; tag_message -> _carcass_types.h; SAMPLE2/tag_monsterInfo/SSpellInfo/
// SWinSetup/SNetPlayerInfo -> _types.h; townSlot/heroRec/SPlayerExit/playerRec -> KB.h.

#define KBFILE ((char *)"I:\\Projects\\Heroes\\Prog\\SOURCE\\KB.CPP")
#define KBLINE (*(short *)"\xBE\x0C")











static long glNextPollTime;
static long glNextMouseTime;
static long glNextCycleTime;

static char cBlank0[4], cBlank1[4], cBlank2[4], cBlank3[4];

inline townSlot *GetCastleRec(int i)
{
    return &gpGame->m_castleRecs[i];
}

inline hero *GetHeroSlot(int i)
{
    return &gpGame->m_heroRecs[i];
}

VA(0x00496450, 0x14e)
extern "C" void PollSound(void)
{
    if (gbInPollSound)
        return;
    gbInPollSound = 1;
    if (KBTickCount() > glNextMouseTime && !gbPutzingWithMouseCtr) {
        glNextMouseTime = KBTickCount() + 13;
        gpMouseManager->NewUpdate(0);
    }
    if (KBTickCount() > glNextCycleTime) {
        if (giCycleType == 1 || giCycleType == 3)
            glNextCycleTime = KBTickCount() + 110;
        else
            glNextCycleTime = KBTickCount() + 200;
        bDoColorCycle = 1;
        if (giGraphicsType == 1 && giMainVideoModeColorDepth != 8) {
            glNextCycleTime += 300;
            if (gbHeroMoving)
                bDoColorCycle = 0;
        }
        if (bDoColorCycle)
            CycleColors(0);
    }
    if (KBTickCount() > glNextPollTime) {
        glNextPollTime = KBTickCount() + 30;
        if (gbForegroundApp)
            gpSoundManager->PollSound();
        PollRemote();
    }
    gbInPollSound = 0;
}

VA(0x0049659e, 0x20)
void ForcePollSound(void)
{
    glNextPollTime = KBTickCount() - 1;
    PollSound();
}

VA(0x004965be, 0x39e)
void InitMainClasses(void) {}

VA(0x0049695c, 0x344)
void DeleteMainClasses(void) {}

VA(0x00496ca0, 0x39)
void EarlyShutdown(char *caption, char *text)
{
    MessageBoxA(hwndApp, text, caption, 0x10);
    exit(0);
}

VA(0x00496cd9, 0x148)
void SetupCDRom(void)
{
    int savedNoSound = gbNoSound;
    if (iCDRomErr == 1) {
        SetPalette(*(signed char **)((char *)gPalette + 0x10), 1);
        gpMouseManager->ShowColorPointer();
        gbNoSound = 1;
        if (giTCPHostStatus)
            NormalDialog("Unable to access CD-ROM Drive.  Without a CD-ROM drive and a Heroes 2 Expansion CD-ROM you will only be able to play as the guest in a multi-player game.",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        gbNoCDRom = 1;
    } else if (iCDRomErr == 2) {
        SetPalette(*(signed char **)((char *)gPalette + 0x10), 1);
        gpMouseManager->ShowColorPointer();
        gbNoSound = 1;
        if (giTCPHostStatus)
            NormalDialog("The Heroes 2 Expansion CD-ROM is not in the drive.  Without a Heroes 2 Expansion CD-ROM you will only be able to play as the guest in a multi-player game.  If you have the CD, then exit the program, put the CD in, and try again.",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        gbNoCDRom = 1;
    }
    if (iCDRomErr == 3) {
        EarlyShutdown("Startup Error", "Unable to change to the Heroes II directory.  Please run the installation program.");
        exit(0);
    }
    if (iCDRomErr == 4) {
        EarlyShutdown("Startup Error", "Unable to find the Heroes II data files.  Please run the installation program.");
        exit(0);
    }
    gbNoSound = savedNoSound;
}

VA(0x00496e21, 0x77)
int EarlySetup(void)
{
    if (bEarlySetupDone)
        return 0;
    InitMainClasses();
    GetGraphicsInfo();
    ReadPrefs();
    if (!InterpretCommandLine())
        return 1;
    LogTruncate();
    LogStr("ES1");
    iCDRomErr = SetupCDDrive();
    InitVars();
    LogStr("ES2");
    return 1;
}

VA(0x00496e98, 0x16c0)
int oldmain(void) { return 0; }

VA(0x00498558, 0x44)
char toupper(char c)
{
    if (c >= 'a' && c <= 'z')
        return c - ' ';
    else
        return c;
}

VA(0x0049859c, 0x791)
int InterpretCommandLine(void) { return 0; }

VA(0x00498d2d, 0x698)
int InitMenuHandler(struct tag_message &) { return 0; }

VA(0x004993c5, 0x1b)
int NullHandler(struct tag_message &msg)
{
    return 1;
}

VA(0x004993e0, 0x1a9)
int RecruitHeroHandler(tag_message &msg)
{
    // e/p/c/d are the (dead) event-id shorts; a = handled flag; b reserves a slot.
    short e = 2, p = 3, c = 8, d = 9;
    int a = 0;
    int b;
    if (msg.type == 0x200) {
        switch (msg.field4) {
        case 0xc:
            switch (msg.field8) {
            case 2:
                HeroView(static_cast<unsigned char>(gpTownManager->m_recruitHeroRec[2]), 1, 0);
                gpTownManager->RedrawTownScreen();
                gpTownManager->m_heroWindow0->DrawWindow();
                gpTownManager->m_heroWindow1->DrawWindow();
                gpWindowManager->FadeScreen(0, 8, 0);
                break;
            default:
                break;
            }
            break;
        case 0xd:
            switch (msg.field8) {
            case 0x7801:
                gpTownManager->m_recruitState = -1;
                a = 1;
                break;
            case 0x7802:
                gpTownManager->m_recruitState = 0;
                *(int *)((char *)gpWindowManager + 0x5a) = msg.field8;
                a = 1;
                break;
            }
            break;
        default:
            break;
        }
    }
    if (a == 1) {
        msg.field8 = 0xa;
        msg.field4 = msg.field8;
        return 2;
    }
    return 1;
}

VA(0x00499589, 0x1a7)
char *GetBuildingInfo(int race, int building, int mode)
{
    char buf[400];
    if (race == 5 && building == 2) {
        sprintf(buf, xNecromancerShrineDesc);
    } else if (building == 0xb) {
        sprintf(buf, "The %s increases production of %s by 8 per week.",
                GetBuildingName(race, building),
                gArmyNamesPlural[gDwellingType[race][0]]);
    } else if (building == 0xd) {
        sprintf(buf, gBuildingInfoSpecial[race]);
    } else if (building < 0x13) {
        sprintf(buf, cBuildingInfoNeutral[building]);
    } else {
        sprintf(gText, "The %s produces %s.",
                GetBuildingName(race, building),
                gArmyNamesPlural[gDwellingType[race][building]]);
        return gText;
    }
    if (mode) {
        sprintf(gText, "{%s}\n\n%s", GetBuildingName(race, building), buf);
    } else {
        sprintf(gText, buf);
    }
    return gText;
}

VA(0x00499730, 0xa4)
char *GetBuildingName(int race, int building)
{
    if (race == 5 && building == 2)
        return xNecromancerShrine;
    if (building == 0xb)
        return gWellExtraNames[race];
    else if (building == 0xd)
        return gSpecialBuildingNames[race];
    else if (building < 0x13)
        return gNeutralBuildingNames[building];
    else
        return gDwellingNames[race][building - 0x13];
}

// @early-stop
// ~98%: 2-instruction /Od body-placement variance — retail emits the final
// `else if (building<0x10)` body out-of-line (`jl body; jmp epi`); this build lowers it
// inline (`jge skip`). Logic byte-exact otherwise; resisted every source structure tried.
VA(0x004997d4, 0x138)
void GetBuildingCost(int race, int building, int *const dest, int mageLevel)
{
    int level;
    if (building == 2 && race == 5) {
        memcpy(dest, xShrineBuildingCost, 0x1c);
    } else if (building >= 0x13 && building <= 0x1e) {
        memcpy(dest, gDwellingCosts[race][building - 0x13], 0x1c);
    } else if (building == 0) {
        level = mageLevel + 1;
        if (level > 5)
            level = 5;
        memcpy(dest, gMageBuildingCosts[mageLevel + 1], 0x1c);
    } else if (building == 0xd) {
        memcpy(dest, gSpecialBuildingCosts[race], 0x1c);
    } else if (building < 0x10) {
        memcpy(dest, gNeutralBuildingCosts[building], 0x1c);
    }
}

VA(0x0049990c, 0x20)
char *GetMonsterName(int m)
{
    return gArmyNames[m];
}

// @early-stop
// ~99.6%: code bytes link-identical; residual is the switch jump/index-table reloc
// names ($L... vs func+off) the delinker assigns differently. Confirmed same linked bytes.
VA(0x0049992c, 0x140)
void GetMonsterCost(int monster, int *const cost)
{
    int idx;
    for (idx = 0; idx < 7; idx++)
        cost[idx] = 0;
    cost[6] = gMonsterDatabase[monster].cost;
    switch (monster) {
    case 60:
        cost[5] = 1;
        break;
    case 28:
        cost[1] = 1;
        break;
    case 19:
        cost[4] = 1;
        break;
    case 35:
    case 36:
        cost[3] = 1;
        break;
    case 37:
        cost[3] = 2;
        break;
    case 45:
        cost[5] = 1;
        break;
    case 46:
        cost[5] = 2;
        break;
    }
}

// @early-stop
// tu-cumulative: logic + all frame slots byte-exact (reqMask@-8, haveMask@-4 match
// retail); the only residual is 2 bytes — the commutative `&` operand load-order in
// `(reqMask & haveMask) == reqMask` (retail loads reqMask first, this cl loads the
// just-OR'd haveMask first). Not source-steerable (tried both `&` orders, `==` swap).
VA(0x00499a6c, 0x2b5)
int CanBuild(town *t, int building)
{
    int reqMask;
    int haveMask;
    if (BitTest((char *)gpGame + 0x27bb, *(signed char *)t))
        return 0;
    if (building != 6 && !(*(int *)((char *)t + 0x18) & 0x40))
        return 0;
    if (!xIsExpansionMap && building == 2 && *((signed char *)t + 3) == 5)
        return 0;
    if (building == 3) {
        if (t->CanBuildDock())
            return 1;
        else
            return 0;
    }
    if (building == 0 && *((signed char *)t + 0x1c) >= 5)
        return 0;
    if (building == 5 || building == 0xe || building == 0x10 || building == 0x11 ||
        building == 0x12 || building == 0x1f)
        return 0;
    if (building < 0x13 || building > 0x1e)
        return 1;
    if ((building == 0x14 && (*(int *)((char *)t + 0x18) & 0x02000000)) ||
        (building == 0x15 && (*(int *)((char *)t + 0x18) & 0x04000000)) ||
        (building == 0x16 && (*(int *)((char *)t + 0x18) & 0x08000000)) ||
        (building == 0x17 && (*(int *)((char *)t + 0x18) & 0x10000000)) ||
        (building == 0x18 && ((*(int *)((char *)t + 0x18) & 0x20000000) ||
                              (*(int *)((char *)t + 0x18) & 0x40000000))) ||
        (building == 0x1d && (*(int *)((char *)t + 0x18) & 0x40000000)))
        return 0;
    reqMask = gHierarchyMask[*((signed char *)t + 3)][building - 0x13];
    haveMask = *(int *)((char *)t + 0x18);
    if (haveMask & 0x02000000)
        haveMask |= 0x100000;
    if (haveMask & 0x04000000)
        haveMask |= 0x200000;
    if (haveMask & 0x08000000)
        haveMask |= 0x400000;
    if (haveMask & 0x10000000)
        haveMask |= 0x800000;
    if (haveMask & 0x40000000)
        haveMask |= 0x20000000;
    if (haveMask & 0x20000000)
        haveMask |= 0x1000000;
    if ((reqMask & haveMask) == reqMask) {
        if (*((signed char *)t + 3) == 5 && building == 0x1c && *((signed char *)t + 0x1c) <= 1)
            return 0;
        return 1;
    }
    return 0;
}

VA(0x00499d21, 0x9a)
int CanBuy(town *t, int type)
{
    int buf[7];
    playerRec *ptr;
    int idx;
    GetBuildingCost(*((signed char *)t + 3), type, buf, *((signed char *)t + 0x1c));
    ptr = (playerRec *)((char *)(giCurPlayer + (playerRec *)gpGame) + 0x49c);
    for (idx = 0; idx < 7; idx++)
        if (ptr->resources[idx] < buf[idx])
            return 0;
    return 1;
}

VA(0x00499dbb, 0xc6)
int GetBuildingBaseResourceValue(int race, int building, int level)
{
    if (race == 5 && building == 5)
        return 1000;
    if (building < 0x13 || building > 0x1e) {
        if (building > 0xf)
            return 0;
        else if (building == 0)
            return gMageBaseResourceValues[level];
        else if (building == 0xd)
            return gSpecialBuildingBaseResourceValues[race];
        else
            return gNeutralBaseResourceValues[building];
    } else {
        return gDwellingBaseResourceValues[race][building - 0x13];
    }
}

// @early-stop
// reloc-masked: code bytes byte-identical to retail (coffcmp: 0 non-reloc diffs).
// The only residual is the giWaitType switch jump-table's self-relocs (DIR32 to
// WaitHandler+offset) which the delinker labels differently — same delinker artifact
// class as GetMonsterCost above.
VA(0x00499e81, 0x21e)
int WaitHandler(tag_message &msg)
{
    int result = 0;
    gbFunctionComplete = 1;
    PollSound();
    if (msg.type == 0x200) {
        switch (msg.field4) {
        case 0xd:
            switch (msg.field8) {
            case 0x7800:
            case 0x7801:
            case 0x7802:
                gbFunctionComplete = 0;
                result = 1;
                break;
            }
        }
    }
    if (result == 0) {
        switch (giWaitType) {
        case 0:
            result = WaitForOtherPlayer();
            break;
        case 1:
            result = WaitForHost();
            break;
        case 2:
            result = WaitForGuest();
            break;
        case 3:
            result = InitNetGuest();
            break;
        case 4:
            result = InitNetHost();
            break;
        case 5:
            result = GUIModemCommandExec();
            break;
        case 6:
            result = GUIModemResponseExec();
            break;
        case 7:
            result = WaitForDirectConnect();
            break;
        case 8:
            result = dpWaitForFirstGuest();
            break;
        case 9:
            result = dpWaitForExtraGuests();
            break;
        case 0xa:
            result = dpWaitForHost();
            break;
        case 0xb:
            result = wsWaitForFirstGuest();
            break;
        case 0xc:
            result = wsWaitForExtraGuests();
            break;
        case 0xd:
            result = wsWaitForHost();
            break;
        }
    }
    CheckShingleUpdate();
    if (result != 0) {
        *(int *)((char *)gpWindowManager + 0x5a) = 0x7801;
        msg.type = 0x200;
        msg.field8 = 0xa;
        msg.field4 = msg.field8;
        return 2;
    }
    return 1;
}

VA(0x0049a09f, 0x472)
int EventWindowHandler(struct tag_message &) { return 0; }

VA(0x0049a511, 0x1e)
int TrueFalseDialogHandler(struct tag_message &msg)
{
    return EventWindowHandler(msg);
}

VA(0x0049a52f, 0x192)
void PlayerDead(int player)
{
    playerRec *rec;
    int i;
    gbRetreatWin = 0;
    rec = (playerRec *)((char *)(player + (playerRec *)gpGame) + 0x49c);
    *((char *)(player + (char *)gpGame) + 0x490) = 1;
    ++*((char *)gpGame + 0x48f);
    for (i = 0; i < 0x90; i++) {
        if (*((signed char *)(i + (char *)gpGame) + 0x60a6) == player)
            gpGame->ClaimMine(i, -1);
    }
    for (i = *((signed char *)rec + 1) - 1; i >= 0; i--) {
        GetHeroSlot(*((signed char *)(i + (char *)rec) + 4))->Deallocate(1);
    }
    for (i = 0; i < 2; i++) {
        if (*((signed char *)(*((signed char *)(i + (char *)rec) + 0xc) + (char *)gpGame) + 0x5c80) == 0x40)
            *((signed char *)(*((signed char *)(i + (char *)rec) + 0xc) + (char *)gpGame) + 0x5c80) = -1;
    }
    if (gbRemoteOn) {
        if (gbHumanPlayer[player])
            HandleRemoteDeadPlayerExit(player);
        else
            SendMapChange(0xa, player, 0, 0, -999, 0, 0);
    }
}

VA(0x0049a6c1, 0x19bb)
void CheckEndGame(int, int) {}

VA(0x0049c07c, 0x95)
void QuickViewWait(void)
{
    tag_message ev;
    int done;
    gpMouseManager->ReallyHidePointer();
    done = 0;
    while (!done) {
        PollSound();
        Process1WindowsMessage();
        ev = gpInputManager->GetEvent();
        if (ev.type == 0x40 || ev.type == 8 || ev.type == 0x10)
            done = 1;
        else
            done = 0;
    }
    gpMouseManager->ReallyShowPointer();
}

VA(0x0049c111, 0x201)
void InitVars(void)
{
    int i;
    int j;
    NULL_SAMPLE2.pSample = 0;
    NULL_SAMPLE2.pMem = (struct _SAMPLE *)NULL_SAMPLE2.pSample;
    gGameCommand = -1;
    gPalette = 0;
    gbCombatSurrender = 0;
    *(int *)((char *)gpGame + 0x65e5) = 0;
    strcpy((char *)gpGame + 0x466, "brokena.mp2");
    *((char *)gpGame + 0x47a) = 0;
    gbInNewGameSetup = 0;
    strcpy(cNetBoxLine[0], cBlank0);
    strcpy(cNetBoxLine[1], cBlank1);
    strcpy(cNetBoxLine[2], cBlank2);
    strcpy(cNetBoxLine[3], cBlank3);
    cNetBoxColor[0] = 6;
    cNetBoxColor[1] = 6;
    cNetBoxColor[2] = 6;
    cNetBoxColor[3] = 6;
    ppMapExtra = 0;
    pwSizeOfMapExtra = 0;
    iMaxMapExtra = 0;
    for (i = 0; i < 0xa; i++)
        glTimers[i] = 0;
    if (gbCheatMenus) {
        hmnuDflt = LoadMenuA(hInstApp, "mnuDflt");
        hmnuCmbt = LoadMenuA(hInstApp, "mnuCmbt");
        hmnuAdv = LoadMenuA(hInstApp, "mnuAdvD");
        hmnuTown = LoadMenuA(hInstApp, "mnuTownD");
    } else {
        hmnuDflt = LoadMenuA(hInstApp, "mnuDflt");
        hmnuCmbt = LoadMenuA(hInstApp, "mnuCmbt");
        hmnuAdv = LoadMenuA(hInstApp, "mnuAdv");
        hmnuTown = LoadMenuA(hInstApp, "mnuTown");
    }
}

VA(0x0049c312, 0x61b)
void game::ShowMoraleInfo(class hero *, int) {}

VA(0x0049c92d, 0x371)
void game::ShowLuckInfo(class hero *, int) {}

VA(0x0049cc9e, 0xd7)
void ClearMapExtra(void)
{
    int i;
    for (i = 0; i < iMaxMapExtra; i++) {
        if (ppMapExtra[i])
            BaseFree(ppMapExtra[i], KBFILE, KBLINE + 6);
    }
    if (ppMapExtra)
        BaseFree(ppMapExtra, KBFILE, KBLINE + 9);
    ppMapExtra = 0;
    if (pwSizeOfMapExtra)
        BaseFree(pwSizeOfMapExtra, KBFILE, KBLINE + 0xd);
    pwSizeOfMapExtra = 0;
    iMaxMapExtra = 0;
}

VA(0x0049cd75, 0x9f)
int GetMonType(int score, int campaign)
{
    int idx;
    for (idx = 0x41; idx >= 0; idx--) {
        if (campaign == 0 || campaign == 2) {
            if (giScoreCampaignMon[idx][0] >= score)
                return giScoreCampaignMon[idx][1];
        } else {
            if (giScoreMon[idx][0] <= score)
                return giScoreMon[idx][1];
        }
    }
    return giScoreMon[0][1];
}

VA(0x0049ce14, 0x4ac)
int AddScoreToHighScore(int, int, int, int, char *) { return 0; }

VA(0x0049d2c0, 0x66)
void BVResMsg(char *s, int res, int qty)
{
    giBottomViewOverride = 5;
    giBottomViewOverrideEndTime = KBTickCount() + 0x1388;
    giBottomViewResource = res;
    giBottomViewResourceQty = qty;
    strcpy(gcBottomViewText, s);
    gpAdvManager->UpdBottomView(1, 1, 1);
}

VA(0x0049d326, 0x2d)
void GOut(char *str)
{
    if (*(int *)((char *)gpAdvManager + 0x32) == 1)
        AiPrint(str);
}

VA(0x0049d353, 0x54)
int NetPosToGamePos(int netPos)
{
    for (int i = 0; i < 6; i++)
        if (gbGamePosToNetPos[i] == netPos)
            return i;
    return -1;
}

VA(0x0049d3a7, 0xff)
int WaitForOtherPlayer(void)
{
    int result = 0;
    char *data;
    PollSound();
    data = GetRemoteData(1);
    if (data && data[5] == 2) {
        switch (data[6]) {
        case 0x20:
            memcpy(gbGamePosToNetPos, data + 9, 6);
            gbUseRegularCompression = data[0xf];
            gbUseDiffCompression = data[0x10];
            memcpy(gsNetPlayerInfo, data + 0x11, 0xcc);
            giThisGamePos = NetPosToGamePos(giThisNetPos);
            break;
        case 1:
            result = gpGame->ReceiveSaveGame(*(int *)(data + 9), *(int *)(data + 0xd),
                                             *(int *)(data + 0x11), data[0]);
            break;
        }
    }
    return result;
}

VA(0x0049d4a6, 0xb85)
void PopNetBox(char *, int) {}

VA(0x0049e02b, 0xc7)
void AddNetBoxLine(char *str, char color)
{
    if (color < 0 || color > 6)
        color = 6;
    strcpy(cNetBoxLine[0], cNetBoxLine[1]);
    strcpy(cNetBoxLine[1], cNetBoxLine[2]);
    strcpy(cNetBoxLine[2], cNetBoxLine[3]);
    strcpy(cNetBoxLine[3], str);
    cNetBoxColor[0] = cNetBoxColor[1];
    cNetBoxColor[1] = cNetBoxColor[2];
    cNetBoxColor[2] = cNetBoxColor[3];
    cNetBoxColor[3] = color;
}

VA(0x0049e0f2, 0x214)
void ShutDown(char *msg)
{
    char buf[768];
    if (bInShutDown)
        return;
    LogStr("Shutdown");
    bInShutDown = 1;
    gbClosingApp = 1;
    buf[0] = 0;
    gpMouseManager->SetColorMice(0);
    if (msg) {
        strcpy(buf, msg);
        SetFullScreenStatus(0);
        LogStr(buf);
        MessageBoxA(hwndApp, buf, "Unexpected Program Termination", 0x10);
    } else {
        sprintf(buf, "Bye!");
    }
    ShutDownSmacker();
    gpSoundManager->CDStop();
    ClearMapExtra();
    UnloadSystemwideIcons();
    if (gbRemoteOn)
        HandleRemoteSuddenExit();
    if (gPalette) {
        gpResourceManager->Dispose((resource *)gPalette);
        gPalette = 0;
    }
    if (bigFont) {
        gpResourceManager->Dispose((resource *)bigFont);
        bigFont = 0;
    }
    if (smallFont) {
        gpResourceManager->Dispose((resource *)smallFont);
        smallFont = 0;
    }
    RemoteCleanup();
    gpExec->ShutDownSystem();
    if (gEventHandle) {
        CloseHandle(gEventHandle);
        gEventHandle = 0;
    }
    if (mapExtra)
        BaseFree(mapExtra, KBFILE, (*(short *)"\x5f\x0e") + 0x47);
    mapExtra = 0;
    CloseAIMapVars();
    DeleteMainClasses();
    AppExit();
    PrintMemoryLeaks();
    if (gpMemEntry)
        free(gpMemEntry);
    gpMemEntry = 0;
    exit(0);
}

VA(0x0049e306, 0xa2)
void FileError(char *filename)
{
    char buf1[500];
    int err;
    char buf[500];
    err = errno;
    sprintf(buf1, "File Error %s", strerror(err));
    LogInt(buf1, err, -999, -999, -999, -999, -999, -999);
    sprintf(buf, "Error opening file %s!", filename);
    ShutDown(buf);
}

// @early-stop
// tu-cumulative: logic + all 14 frame slots byte-exact (od_oracle-verified). The only
// residual (coffcmp: 40 bytes, all in the two brightness averages + the minDist test)
// is a /Od operand-evaluation-order difference this cl renders vs retail: the 3-term
// sum `p[2]+p[0]+p[1]` reads +2,+1,+0 here but +2,+0,+1 in retail, and the `d>p`
// compare loads the other operand first. Not source-steerable (probed every term
// ordering, explicit grouping, `|0`, and an inline helper — all identical here).
VA(0x0049e3a8, 0x255)
void SmackFade(unsigned char *src, unsigned char *dst)
{
    // /Od frame slots (od_oracle-verified): a=newPal(-8) b=avg2(-c) c=x(-10)
    // d=minDist(-14) e=avg1(-18) f=map(-1c) g=y(-20) h=outer(-24) i=inner(-28)
    // j=screen(-2c) k=best(-30) p=dist(-4)
    unsigned char *a;   /* newPal */
    unsigned char *f;   /* map    */
    int k;              /* best   */
    int h, i;           /* outer / inner loop */
    int e, b;           /* avg1, avg2 */
    int d;              /* minDist */
    int p;              /* dist   */
    unsigned char *j;   /* screen */
    int c, g;           /* x, y   */

    a = 0;
    f = 0;
    k = -1;
    a = (unsigned char *)BaseAlloc(0x300, KBFILE, (*(short *)"\x61\x0f") + 0xd);
    f = (unsigned char *)BaseAlloc(0x100, KBFILE, (*(short *)"\x61\x0f") + 0xe);
    memset(a, 0, 0x300);
    memset(f, 0, 0x100);
    for (h = 0xa; h < 0xf6; h++) {
        e = (src[h * 3 + 2] + src[h * 3] + src[h * 3 + 1]) / 3;
        d = 0x3e7;
        for (i = 0xa; i < 0x24; i++) {
            b = (dst[i * 3 + 2] + dst[i * 3] + dst[i * 3 + 1]) / 3;
            p = abs(e - b);
            if (d > p) {
                d = p;
                k = i;
            }
        }
        memcpy(a + h * 3, dst + k * 3, 3);
        f[h] = (unsigned char)k;
    }
    FadeTo(src, a, 8);
    j = *(unsigned char **)(*(char **)((char *)gpWindowManager + 0x46) + 0x16);
    for (c = 0; c < 0x280; c++) {
        for (g = 0; g < 0x1e0; g++) {
            *j = f[*j];
            j++;
        }
    }
    gpWindowManager->UpdateScreen();
    UpdatePalette((signed char *)dst);   // real sig is signed char* (?UpdatePalette@@YIXPAC@Z)
    BaseFree(a, KBFILE, (*(short *)"\x61\x0f") + 0x31);
    BaseFree(f, KBFILE, (*(short *)"\x61\x0f") + 0x32);
}

VA(0x0049e5fd, 0x303)
void ShowCongrats(int) {}

VA(0x0049e900, 0x99)
void CongratsWait(void)
{
    int cmd = 0;
    int done = 0;
    tag_message msg;
    gpInputManager->Flush();
    while (!done) {
        PollSound();
        Process1WindowsMessage();
        msg = gpInputManager->GetEvent();
        if (msg.type == 1 || msg.type == 8 || msg.type == 0x10 || msg.type == 0x20 ||
            msg.type == 0x40)
            done = 1;
    }
}

VA(0x0049e999, 0x54)
SAMPLE2 LoadPlaySample(char *name)
{
    SAMPLE2 ss;
    ss.pSample = gpResourceManager->GetSample(name);
    if (ss.pSample) {
        *(int *)((char *)ss.pSample + 0x1c) = 2;
        ss.pMem = gpSoundManager->MemorySample(ss.pSample);
    }
    return ss;
}

VA(0x0049e9ed, 0x8f)
void WaitEndSample(SAMPLE2 s, int waitTime)
{
    long endTime;
    if (waitTime < 0)
        waitTime = 4000;
    endTime = KBTickCount() + waitTime;
    if (s.pMem)
        while (gpSoundManager->DigitalReport(s.pMem, 4) && KBTickCount() < endTime) {
            Process1WindowsMessage();
            PollSound();
        }
    if (s.pSample)
        gpResourceManager->Dispose((resource *)s.pSample);
}

VA(0x0049ea7c, 0x5d)
void MemError(void)
{
    if (gbInMemError)
        return;
    gbInMemError = 1;
    LogStr("Out of Memory");
    sprintf(gText, cOutOfMemory, "Out of memory.", 0x1900);
    ShutDown(gText);
}

VA(0x0049ead9, 0x3b)
char *GetTownName(int i)
{
    townSlot *t = GetCastleRec(i);
    return (char *)t + 0x57;
}

VA(0x0049eb14, 0x3a)
void LoadSystemwideIcons(void)
{
    gBuyBuildIcons = gpResourceManager->GetIcon("buybuild.icn");
    gSystemIcons = gpResourceManager->GetIcon("system.icn");
}

VA(0x0049eb4e, 0x32)
void UnloadSystemwideIcons(void)
{
    gpResourceManager->Dispose((resource *)gBuyBuildIcons);
    gpResourceManager->Dispose((resource *)gSystemIcons);
}

VA(0x0049eb80, 0x10)
void EarlyShutDownSystem(void)
{
}

VA(0x0049eb90, 0x75)
int GameUnsaved(void)
{
    if ((gpAdvManager && *(int *)((char *)gpAdvManager + 0x32) == 1) ||
        (gpCombatManager && *(int *)((char *)gpCombatManager + 0x32) == 1) ||
        (gpTownManager && *(int *)((char *)gpTownManager + 0x32) == 1))
        return 1;
    else
        return 0;
}

VA(0x0049ec05, 0xa18)
int HandleAppSpecificMenuCommands(int) { return 0; }

VA(0x0049f61d, 0x310)
void UpdateSystemOptionsMenu(void) {}

VA(0x0049f92d, 0x99)
void CleanUpMenus(void)
{
    if (hmnuApp) {
        SetMenu(hwndApp, 0);
        if (hmnuAdv)
            DestroyMenu(hmnuAdv);
        if (hmnuDflt)
            DestroyMenu(hmnuDflt);
        if (hmnuCmbt)
            DestroyMenu(hmnuCmbt);
        if (hmnuTown)
            DestroyMenu(hmnuTown);
    }
    hmnuApp = 0;
}

VA(0x0049f9c6, 0x2a)
void UpdateAppSpecificMenus(void *hMenu)
{
    if (hmnuAdv == hMenu)
        UpdateSystemOptionsMenu();
}

VA(0x0049f9f0, 0x2d)
void EarlyResizeWindow(int x, int y, int w, int h)
{
    if (gbClosingApp)
        return;
}

VA(0x0049fa1d, 0x53)
int InMapArea(int x, int y)
{
    return (x >= 16 && x < 448 && y >= 16 && y < 448);
}

VA(0x0049fa70, 0x6bc)
void SetupDynamicWindow(int, int, int, int, int, int, int, int *, int *, int *, int *, int *, int *, class heroWindow * *, int) {}

VA(0x004a012c, 0x108)
void TestDynamicWindow(int p1, int p2)
{
    heroWindow *p;
    int q, r, s, u, v, w;
    int t;
    SetupDynamicWindow(0, 0, 1, 640, 480, p1 * 48, p2 * 48, &s, &u, &v, &w, &q, &r, &p, 0);
    gpWindowManager->AddWindow(p, -1, 1);
    t = 0;
    gpInputManager->Flush();
    while (!t) {
        Process1WindowsMessage();
        switch (gpInputManager->GetEvent().type) {
        case 1:
        case 8:
        case 0x20:
            t = 1;
        }
    }
    gpWindowManager->RemoveWindow(p);
    delete p;
}

VA(0x004a0234, 0x91)
void HandleRemoteDeadPlayerExit(int pos)
{
    SPlayerExit pe;
    if (giThisGamePos == pos) {
        if (!gpGame->TransmitSaveGame((giThisNetPos + 1) % giNumHumanPlayers, 1, 0))
            ShutDown(0);
        RemoteCleanup();
    } else {
        pe.m_0 = gbGamePosToNetPos[pos];
        pe.m_1 = pos;
        pe.m_2 = 0;
        pe.m_3 = 0;
        pe.m_4 = 1;
        pe.m_5 = 0;
        ReceiveRemotePlayerExit(pe);
    }
}

VA(0x004a02c5, 0xaa)
void HandleRemoteSuddenExit(void)
{
    int a;
    char buf[5];
    if (!gbGameInitialized)
        return;
    if (!gbRemoteOn)
        return;
    buf[0] = giThisNetPos;
    buf[1] = giThisGamePos;
    buf[2] = gbThisNetGotAdventureControl;
    buf[3] = 0;
    buf[4] = 0;
    if (giThisNetPos == 0)
        a = 1;
    else
        a = 0;
    LogStr("HRSE1");
    TransmitRemoteData(buf, a, 7, 0x1f, 0, 0, 2);
    LogStr("HRSE2");
    DelayMilli(500);
}

VA(0x004a036f, 0x62)
void DropDownToOnePlayer(void)
{
    RemoteCleanup();
    giNumHumanPlayers = 1;
    for (int i = 0; i < 6; i++)
        if (giThisNetPos != i)
            gbHumanPlayer[i] = 0;
    ComputeAdvNetControl();
}

VA(0x004a03d1, 0x412)
void ReceiveHostReportsPlayerExit(int, struct SPlayerExit, int) {}

VA(0x004a07e3, 0x361)
void ReceiveRemotePlayerExit(struct SPlayerExit) {}

VA(0x004a0b44, 0x29)
int CheckMem(void)
{
    giTotalHighMem = 16000;
    giHighMemBuffer = 8000;
    return 1;
}

VA(0x004a0b6d, 0x109)
int GetManaCost(int spell, hero *h)
{
    int c = gsSpellInfo[spell].m_e;
    if (h != 0) {
        if (h->HasArtifact(0x29) && (spell == 0x12 || spell == 0x13))
            c >>= 1;
        if (h->HasArtifact(0x2c) && (spell == 0x1a || spell == 0x1f || spell == 0x1e || spell == 0xd))
            c >>= 1;
        if (h->HasArtifact(0x33) && (spell == 0xe || spell == 0xf))
            c >>= 1;
        if (h->HasArtifact(0x36) && (spell == 0x2b || spell == 0x2c || spell == 0x2d || spell == 0x2e))
            c >>= 1;
    }
    return c;
}

VA(0x004a0c76, 0x9f)
void SetWinText(heroWindow *j, int id)
{
    int a = 0;
    int i;
    tag_message c;
    for (i = 0; i < 0x49; i++) {
        if (gWinSetup[i].m_0 == id) {
            a++;
            c.type = 0x200;
            c.field4 = 3;
            c.field8 = gWinSetup[i].m_1;
            c.text = gWinSetup[i].m_3;
            j->BroadcastMessage(c);
        }
    }
}

VA(0x004a0d15, 0x8a)
void CheckShingleUpdate(void)
{
    if (!gShingleAnim)
        return;
    if (KBTickCount() > iNextShingleAnim) {
        iNextShingleAnim = KBTickCount() + 250;
        iShingleAnimFrame = (iShingleAnimFrame + 1) % 39;
        gShingleAnim->DrawToBuffer(0, 0, iShingleAnimFrame + 1, 0);
        gpWindowManager->UpdateScreenRegion(0x2e, 0xb0, 0x8b, 0xbb);
    }
}

VA(0x004a0d9f, 0x17c6)
void NormalDialog(char *, int, int, int, int, int, int, int, int, int) {}

VA(0x004a2565, 0x71)
void UpdateNormalDialog(char *text)
{
    short show = 1;
    tag_message evt;
    evt.type = 0x200;
    evt.field4 = 3;
    evt.field8 = 1;
    evt.text = text;
    pNormalDialogWindow->BroadcastMessage(evt);
    pNormalDialogWindow->DrawWindow(0, 0, 0x9000);
    pNormalDialogWindow->DrawWindow(1, -65535, -256);
}

// ---- globals (definitions, RVA order) ----
DATA(0x004f8c58) unsigned char giGroundToTerrain[432];
DATA(0x004f8e08) unsigned char *giGroundShape;
DATA(0x004f8fb8) unsigned char *gColorTableTan;
DATA(0x004f90b8) unsigned char *gColorTableGray;
DATA(0x004f91b8) unsigned char gColorTableYellow[256];
DATA(0x004f92b8) unsigned char gColorTableScenWin[256];
DATA(0x004f93b8) unsigned char gColorTableDarkGray[256];
DATA(0x004f94b8) unsigned char *gColorTableRed;
DATA(0x004f95b8) unsigned char *gColorTableDarkBrown;
DATA(0x004f96b8) int MAP_WIDTH;
DATA(0x004f96bc) int MAP_HEIGHT;
DATA(0x004f96c0) unsigned char *mapExtra;
DATA(0x004f96c4) int gbClosingApp;
DATA(0x004f96c8) int gbForegroundApp;
DATA(0x004f96cc) int giMainVideoModeColorDepth;
DATA(0x004f96d0) int giMainVideoModeWidth;
DATA(0x004f96d4) int giMainVideoModeHeight;
DATA(0x004f96d8) unsigned long glMilliCounter;
DATA(0x004f96e0) unsigned char *gMapColors;
DATA(0x004f96f0) unsigned char *gObjectColors;
DATA(0x004f9700) unsigned char *gOwnerColors;
DATA(0x004f9708) char *gTilesetFiles[64];
DATA(0x004f9808) unsigned char *bPuzzleDraw;
DATA(0x004f9848) unsigned char uDimPal[3][4][256];
DATA(0x004fa448) unsigned char *gColorTableLighten;
DATA(0x004fa548) unsigned char *gColorTableNoCycle;
DATA(0x004fa648) font *smallFont;
DATA(0x004fa64c) font *bigFont;
DATA(0x004fa650) int gbReturnAfterComputeExtent;
DATA(0x004fa654) int gbAllowTextEntryEscape;
DATA(0x004fa658) int giCycleType;
DATA(0x004fa65c) int giScreenScroll;
DATA(0x004fa660) int giMenuCommand;
DATA(0x004fa664) int gbSendMouseMoveMessages;
DATA(0x004fa668) int gbColorMice;
DATA(0x004fa670) unsigned long *gTownEligibleBuildMask;
DATA(0x004fa688) unsigned char *giMapSizes;
DATA(0x004fa68c) int gbUseEvilInterface;
DATA(0x004fa690) char *cEvilTranslate[37][2];
DATA(0x004fa7b8) char gcAnimPath[0x160];
DATA(0x004fa918) char gcGamePath[0x18];
DATA(0x004fa930) char gcMapPath[0x14];
DATA(0x004fa944) int gbPutzingWithMouseCtr;
DATA(0x004fa948) int gbDontTryRedbook;
DATA(0x004fa94c) int gbDontTryMIDI;
DATA(0x004fa950) int gbDontTryDigital;
DATA(0x004fa958) float *gfCombatSpeedMod;
DATA(0x004fa964) icon *gShingleAnim;
DATA(0x004fa968) int iNextShingleAnim;
DATA(0x004fa96c) int giDialogTimeout;
DATA(0x004fa970) int giNewMonsterCycleFrame;
DATA(0x004fa974) int gbNoCDRom;
DATA(0x004fa978) int gbLeaveNetBoxAlone;
DATA(0x004fa97c) int gbDrawWindowBackground;
DATA(0x004fa980) int gbCheatMenus;
DATA(0x004fa984) int gbUseWaveout;
DATA(0x004fa988) int gbShowAllMaps;
DATA(0x004fa990) char *gCombatFxNames[32];
DATA(0x004faa10) short *horseFrameFlip;
DATA(0x004faa30) short *boatFrameFlip;
DATA(0x004faa50) signed char *gCastleResources;
DATA(0x004faa58) short *gCastleAmounts;
DATA(0x004faa60) int gHeroGoldCost;
DATA(0x004faa68) short *gVesaMode;
DATA(0x004faa78) tag_tilePoint normalDirTable[8];
DATA(0x004faa98) int gResourceBaseValue[8];
DATA(0x004faab8) int gInitResourcesHuman[5][7];
DATA(0x004fab48) int gInitResourcesComputer[5][7];
DATA(0x004fabd8) int *gMineCharacteristics;
DATA(0x004fabf8) int gSSValues[14][3];
DATA(0x004faca0) unsigned char gArtifactLevel[104];
DATA(0x004fad08) int gArtifactBaseRV[100];
DATA(0x004faea4) int gUltArtifactAvgValue;
DATA(0x004faea8) int giDebugLevel;
DATA(0x004faeac) signed char giVisRangeTown;
DATA(0x004faeb0) tag_monsterInfo gMonsterDatabase[66];
DATA(0x004fb568) float *gfStatPower;
DATA(0x004fb610) float *gfBattleStat;
DATA(0x004fb6b8) signed char *gSpellLimits;
DATA(0x004fb6c0) float *gfSpellCastableCombatMod;
DATA(0x004fb6f0) float *gfSpellCastNumMod;
DATA(0x004fb720) float *gfPhilAISpellPowerMod;
DATA(0x004fb750) float *gfPhilAIDurationMod;
DATA(0x004fb780) float *gfSpellTypeNumMod;
DATA(0x004fb79c) int gbDrawSavedCursor;
DATA(0x004fb7a0) signed char gbArrow[8][8];
DATA(0x004fb7e0) unsigned char *giCloudType;
DATA(0x004fb8e0) short giScoreMon[66][2];
DATA(0x004fb9e8) short giScoreCampaignMon[66][2];
DATA(0x004fbaf0) signed char *townTheme;
DATA(0x004fbaf8) signed char gHeroSkillBonus[6][2][4];
DATA(0x004fbb28) int gbLoadingMonoIcon;
DATA(0x004fbb2c) int giMonoIconSkip;
DATA(0x004fbb30) int giScrollX;
DATA(0x004fbb34) int giScrollY;
DATA(0x004fbb38) int gbNoBorder;
DATA(0x004fbb3c) int gbEnlargeScreenBlit;
DATA(0x004fbb40) int giCurExe;
DATA(0x004fbb44) int gbInDialog;
DATA(0x004fbb48) struct SMenuEnableStatus *gsMenuEnableStatus;
DATA(0x004fbd34) int gbInSetupDialog;
DATA(0x004fbd38) int gbMinimized;
DATA(0x004fbd3c) int gbHeroMoving;
DATA(0x004fbd40) int gbInSmackMgr;
DATA(0x004fbd44) int glBottomRefresh;
DATA(0x004fbd48) int gbBothMachinesWin95;
DATA(0x004fbd4c) int gbGotFirstHeartbeat;
DATA(0x004fbd50) void *hmnuDflt;
DATA(0x004fbd54) void *hmnuCmbt;
DATA(0x004fbd58) void *hmnuAdv;
DATA(0x004fbd5c) void *hmnuTown;
DATA(0x004fbd60) char *cMonFilename[66];
DATA(0x004fbe68) int gbProcessingCombatAction;
DATA(0x004fbe6c) int iMPNetProtocol;
DATA(0x004fbe70) int iLastDiffSendTo;
DATA(0x004fbe78) SSpellInfo gsSpellInfo[65];
DATA(0x004fc410) char *cArmyFrameFileNames[66];
DATA(0x004fc518) unsigned char *gcSpellInfluenceIcons;
DATA(0x004fc528) unsigned char *giSpellInfluenceToSpell;
DATA(0x004fc538) unsigned char *giNumPowFrames;
DATA(0x004fc558) int giSpellEffectShowType;
DATA(0x004fc560) signed char gcColorToPlayerPos[8];
DATA(0x004fc568) char *cCombatBkgNames[20];
DATA(0x004fc5b8) struct SCmbtObstacle *sCmbtObstacles;
DATA(0x004fc778) int *gEstatesGoldLevel;
DATA(0x004fc788) float *gfSSLogisticsMod;
DATA(0x004fc798) float *gfSSNavigationMod;
DATA(0x004fc7a8) float *gfSSArcheryMod;
DATA(0x004fc7b8) float *gfSSAIArcheryMod;
DATA(0x004fc7c8) signed char giVisRange[8];
DATA(0x004fc7d0) unsigned char gStartingHeroStats[6][5];
DATA(0x004fc7f0) int giTerrainCost[10][4][2];
DATA(0x004fc930) unsigned char bStopOnTrigger[128];
DATA(0x004fc9b0) char *gTownPrefixNames[6];
DATA(0x004fc9c8) char *gTownObjNames[32];
DATA(0x004fca48) signed char gDwellingType[20][12];
DATA(0x004fca90) int gMageBuildingCosts[6][7];
DATA(0x004fcb38) int gSpecialBuildingCosts[6][7];
DATA(0x004fcbe0) int gNeutralBuildingCosts[16][7];
DATA(0x004fcda0) int gMageBaseResourceValues[6];
DATA(0x004fcdb8) int gNeutralBaseResourceValues[20];
DATA(0x004fce08) int gSpecialBuildingBaseResourceValues[6];
DATA(0x004fce20) int gDwellingBaseResourceValues[6][12];
DATA(0x004fcf40) int gDwellingCosts[6][12][7];
DATA(0x004fd720) unsigned long gHierarchyMask[6][12];
DATA(0x004fd840) int giDebugBuildingToBuild;
DATA(0x004fd848) unsigned char giTerrainToMusicTrack[16];
DATA(0x004fd858) char *cHeroTypeShortName[6];
DATA(0x004fd870) char *cHeroTypeInitial;
DATA(0x004fd878) int giDeferObjDrawX;
DATA(0x004fd87c) int giDeferObjDrawY;
DATA(0x004fd880) class heroWindow *gpInitWin;
DATA(0x004fd8e0) struct SCmbtHero *sCmbtHero;
DATA(0x004fdeec) unsigned char *iWallToHexCell;
DATA(0x004fdef0) unsigned char *iTowerToHexCell;
DATA(0x004fdef8) unsigned short wallPos[4][2];
DATA(0x004fdf08) unsigned short towerPos[4][2];
DATA(0x004fdf18) unsigned short doorPos[2][2];
DATA(0x004fdf20) float *fTradingPostEfficency;
DATA(0x004fdf50) struct SElevationOverlay *sElevationOverlay;
DATA(0x004fe100) signed char captainStats[6][4];
DATA(0x004fe118) int gbDrawingPuzzle;
DATA(0x004fe11c) int giWalkingFrom;
DATA(0x004fe120) int giWalkingFrom2;
DATA(0x004fe124) int giWalkingTo;
DATA(0x004fe128) int giWalkingTo2;
DATA(0x004fe12c) int giWalkingYMod;
DATA(0x004fe130) unsigned char *moatCell;
DATA(0x004fe2a8) char *congratsText;
DATA(0x004fe2b0) char *gArtifactNames[104];
DATA(0x004fe450) char *gArtifactDesc[104];
DATA(0x004fe5f0) char *gArtifactEvent[104];
DATA(0x004fe790) char *gStatNames[4];
DATA(0x004fe7a0) char *gStatDesc[4];
DATA(0x004fe7b0) char *gAlignmentNames[8];
DATA(0x004fe7d0) char *gArmyShortNames[66];
DATA(0x004fe8d8) char *gArmyNames[66];
DATA(0x004fe9e0) char *gArmyNamesPlural[66];
DATA(0x004feae8) char *gTerrainNames[10];
DATA(0x004feb10) char *gResourceNames[8];
DATA(0x004feb30) char *gQuickViewText[124];
DATA(0x004fed20) char *gEventText[96];
DATA(0x004feea0) char *gCPanelHelp[6];
DATA(0x004feeb8) char *gCSPanelHelp[8];
DATA(0x004feed8) char *gAPanelHelp[6];
DATA(0x004feef0) char *gInitMenuHelp[6];
DATA(0x004fef08) char *gAdvMenuHelp[8];
DATA(0x004fef28) char *gLuckText[8];
DATA(0x004fef48) char *gMoraleText[8];
DATA(0x004fef68) char *onOffText[12];
DATA(0x004fef98) char *walkSpeedText[6];
DATA(0x004fefb0) char *gColors[6];
DATA(0x004fefc8) char *gMonthNames[10];
DATA(0x004feff0) char *gWeekNames[16];
DATA(0x004ff030) char *cHeroScreen[26];
DATA(0x004ff098) char *cCastleInfo[16];
DATA(0x004ff0d8) char *cLuckInfo[22];
DATA(0x004ff130) char *IQnames[6];
DATA(0x004ff148) char *cSpellHelp[10];
DATA(0x004ff170) char *speedText[10];
DATA(0x004ff198) char *cArmyDetail[10];
DATA(0x004ff1c0) char *cWellDetail[10];
DATA(0x004ff1e8) char *cKingdomOverview[4];
DATA(0x004ff1f8) char *cNewTurn[8];
DATA(0x004ff218) char *cViewGeneralLabels[8];
DATA(0x004ff238) char *cViewGeneralHelp[8];
DATA(0x004ff258) char *cViewGeneralLongHelp[4];
DATA(0x004ff268) char *cCombatMessage[12];
DATA(0x004ff298) char *cHeroLevel[4];
DATA(0x004ff2a8) char *cCombatHelp[6];
DATA(0x004ff2c0) char *cLongCombatHelp[6];
DATA(0x004ff2d8) char *cTownCommand[28];
DATA(0x004ff348) char *gHeroDefaultNames[54];
DATA(0x004ff420) char *gNewGameHelp[8];
DATA(0x004ff440) char *gSetupBaudHelp[6];
DATA(0x004ff458) char *gSetupComPortHelp[6];
DATA(0x004ff470) char *gSetupDCBaudHelp[6];
DATA(0x004ff488) char *gSetupDCComPortHelp[6];
DATA(0x004ff4a0) char *gSetupHotSeatGameHelp[6];
DATA(0x004ff4b8) char *gSetupModemGameHelp[4];
DATA(0x004ff4c8) char *gSetupDCGameHelp[4];
DATA(0x004ff4d8) char *gSetupMultiPlayerGameHelp[6];
DATA(0x004ff4f0) char *gSetupNetworkGameHelp[4];
DATA(0x004ff500) char *gSetupNetworkGame2Help[4];
DATA(0x004ff510) char *gSetupGameHelp[4];
DATA(0x004ff520) char *cBattleResults[12];
DATA(0x004ff550) char *cMoraleInfo[32];
DATA(0x004ff5d0) char *cMapSize[4];
DATA(0x004ff5e0) char *cDifficulty[6];
DATA(0x004ff5f8) char *cStartDifficulty[4];
DATA(0x004ff608) char *cCampaignLeaders[4];
DATA(0x004ff618) char *cWinText[6];
DATA(0x004ff630) char *cHumanDifficulty[6];
DATA(0x004ff648) char *cHumanInfoDifficulty[6];
DATA(0x004ff660) char *musicQualityText[4];
DATA(0x004ff670) char *gSpellDesc[66];
DATA(0x004ff778) char *gSpellNames[66];
DATA(0x004ff880) char *gSecondarySkillLevels[4];
DATA(0x004ff890) char *gSecondarySkills[14];
DATA(0x004ff8c8) char *gNeutralBuildingNames[20];
DATA(0x004ff918) char *gWellExtraNames[8];
DATA(0x004ff938) char *gSpecialBuildingNames[8];
DATA(0x004ff958) char *gDwellingNames[6][12];
DATA(0x004ffb20) char *cBuildingInfoNeutral[20];
DATA(0x004ffb70) char *gBuildingInfoSpecial[6];
DATA(0x004ffb88) char *cDirections[10];
DATA(0x004ffbb0) char *cRumourTerrainDescriptions[10];
DATA(0x004ffbd8) char *gInterfaceTypeText[4];
DATA(0x004ffbe8) char *cBWMouseText[2];
DATA(0x004ffbf0) char *combatSpeedText[4];
DATA(0x004ffc00) char *combatMiniInfoText[4];
DATA(0x004ffc10) char *gcCommandLineHelp[14];
DATA(0x004ffc48) char *cOverviewText[6];
DATA(0x004ffc60) char *cWinComError[6];
DATA(0x004ffc78) char *cMiniViewText[10];
DATA(0x004ffca0) char *gFileRequestHelp[16];
DATA(0x004ffce0) char *cPersonality[4];
DATA(0x004ffd60) char *cRandomTavernText[8];
DATA(0x004ffd80) char *cRandomSignText[4];
DATA(0x004ffd90) char *cCampaignAwards[12];
DATA(0x004ffe80) char *cOutOfMemory;
DATA(0x004ffe88) char *cSlowVideoLevelText[2];
DATA(0x004ffe90) char *gSPanelHelp[10];
DATA(0x004ffeb8) char *xBarrierColor[8];
DATA(0x004ffed8) char *xGenericSiteNames[8];
DATA(0x004ffef8) char *xRecruitmentSiteNames[6];
DATA(0x004fff10) SWinSetup gWinSetup[73];
DATA(0x00500110) int gbHeroWindShowing;
DATA(0x00500114) int gbFullCombatScreenDrawn;
DATA(0x00500118) int gbLimitedCombatUpdatePalette;
DATA(0x0050011c) int gbFirstTimeThrough;
DATA(0x00500120) int gbSkipIntro;
DATA(0x00500124) int gbDoMemCheck;
DATA(0x00500128) int gbAllBlack;
DATA(0x0050012c) int gbInCombat;
DATA(0x00500130) int gbDirectConnect;
DATA(0x00500134) int giForceSwitchMusic;
DATA(0x00500138) int gbComputeExtent;
DATA(0x0050013c) int gbSaveBiggestExtent;
DATA(0x00500140) int gbLimitToExtent;
DATA(0x00500144) int gbCurrArmyDrawn;
DATA(0x00500148) int gAdvDisposeLevel;
DATA(0x0050014c) int gbRemoteOn;
DATA(0x00500150) int gbGameInitialized;
DATA(0x00500154) int giHighScoreRank;
DATA(0x00500158) int giHighScoreType;
DATA(0x0050015c) int gbShowHighScore;
DATA(0x00500160) int gbLowMemory;
DATA(0x00500164) int giHighMemBuffer;
DATA(0x00500168) void *gLowPage;
DATA(0x0050016c) int gbLowPageGrabbed;
DATA(0x00500170) signed char xSmackFromNetwork;
DATA(0x00500174) int gbInPollSound;
DATA(0x005157a8) int iCDRomErr;
DATA(0x005157ac) int bEarlySetupDone;
DATA(0x005159f8) int bKBDone;
DATA(0x005159fc) struct _REDBOOK *hRedbookz;
DATA(0x00515a00) int bForceCheckTimeEvent;
DATA(0x00515ca0) unsigned short IMHotSpots[5][4];
DATA(0x00515cc8) int lastIMHoverID;
DATA(0x00515f78) int bInCheckEndGame;
DATA(0x005165dc) int bInShutDown;
DATA(0x00516810) int gbInMemError;
DATA(0x00516d1c) int iShingleAnimFrame;
DATA(0x00528598) int gbHumanPlayer[6];
DATA(0x005285b0) int gbHitEvent;
DATA(0x005285b4) int giMaxExtentX;
DATA(0x005285b8) int giMaxExtentY;
DATA(0x005285bc) int giRandomClouds;
DATA(0x005285c0) char *cOverrideDigitalDriver;
DATA(0x005285d0) int giBottomViewOverrideEndTime;
DATA(0x005285d8) signed char gArmyEffected[2][20];
DATA(0x00528600) int giBottomViewResource;
DATA(0x00528604) int gbInCampaign;
DATA(0x00528608) int giResExtra1;
DATA(0x0052860c) int giResExtra2;
DATA(0x00528610) signed char puzzlePiecesRemoved[6];
DATA(0x00528618) int giSeedingValid;
DATA(0x0052861c) int giLimitPlayer;
DATA(0x00528620) int giShowClouds;
DATA(0x00528624) int bDoColorCycle;
DATA(0x00528628) inputManager *gpInputManager;
DATA(0x00528630) SAMPLE2 NULL_SAMPLE2;
DATA(0x00528638) int iMaxMapExtra;
DATA(0x0052863c) palette *gPalette;
DATA(0x00528640) resourceManager *gpResourceManager;
DATA(0x00528648) char *gcBotViewText;
DATA(0x005286e0) int bSpecialHideCursor;
DATA(0x005286e4) searchArray *gpSearchArray;
DATA(0x005286e8) int giResType1;
DATA(0x005286ec) int gbBlackoutPlayer;
DATA(0x005286f0) int giResType2;
DATA(0x005286f8) char cNetBoxLine[4][140];
DATA(0x00528928) int gIndex;
DATA(0x0052892c) int giWeekTypeExtra;
DATA(0x00528930) philAI *gpPhilAI;
DATA(0x00528934) int giTCPType;
DATA(0x00528938) int gCurLoadedSpellEffect;
DATA(0x0052893c) class mouseManager *gpMouseManager;
DATA(0x00528940) int giCurTempMobility;
DATA(0x00528944) int giOverviewReturnAction;
DATA(0x00528948) char *cOverrideMIDIDriver;
DATA(0x00528958) icon *gSystemIcons;
DATA(0x0052895c) signed char gbCombatSurrender;
DATA(0x00528960) heroWindow *pNormalDialogWindow;
DATA(0x00528964) int giTCPHostStatus;
DATA(0x00528968) char gMapName[16];
DATA(0x00528978) int giMinExtentX;
DATA(0x0052897c) int giMinExtentY;
DATA(0x00528980) int iMPBaseType;
DATA(0x00528984) int gbTCPFirstTime;
DATA(0x00528988) short *pwSizeOfMapExtra;
DATA(0x0052898c) int giHeroScreenSrcIndex;
DATA(0x00528990) int giWeekType;
DATA(0x00528998) char gText[768];
DATA(0x00528c98) int gbInNewGameSetup;
DATA(0x00528c9c) class palette *gpBufferPalette;
DATA(0x00528ca0) int gbCampaignSideChoice;
DATA(0x00528ca4) char cNetBoxColor[4];
DATA(0x00528ca8) int giMonthTypeExtra;
DATA(0x00528cac) int iMPExtendedType;
DATA(0x00528cb0) signed char gcColorToSetupPos[8];
DATA(0x00528cb8) char *gFullMapName;
DATA(0x00528cd0) char *gcTCPName;
DATA(0x00528ce8) int giShowIntro;
DATA(0x00528cf0) int glTimers[10];
DATA(0x00528d18) int giScore;
DATA(0x00528d1c) armyGroup *gpMonGroup;
DATA(0x00528d20) configStruct gConfig;
DATA(0x00528ec0) char gcRegAppPath[0x160];
DATA(0x00529020) unsigned long gTimeMark;
DATA(0x00529024) char *EXPANSION_AGGREGATE_NAME;
DATA(0x00529028) char cPlayerNames[6][21];
DATA(0x005290a8) game *gpGame;
DATA(0x005290ac) signed char gbRetreatWin;
DATA(0x005290b0) int giWaitType;
DATA(0x005290b4) class icon *gCurLoadedSpellIcon;
DATA(0x005290b8) unsigned char bSaveMusicPosition[0x3c];
DATA(0x005290f4) int giBottomViewOverride;
DATA(0x005290f8) char *gcTCPAddress;
DATA(0x00529110) unsigned char giSetupGameType;
DATA(0x00529118) char *gLastFilename;
DATA(0x00529278) int giFullySeeded;
DATA(0x0052927c) icon *gBuyBuildIcons;
DATA(0x00529280) int gbNoSound;
DATA(0x00529288) int *iCombatControlNetPos;
DATA(0x00529290) char *cExpAggPathName;
DATA(0x005293f0) int gbMoveShown;
DATA(0x005293f4) void **ppMapExtra;
DATA(0x005293f8) char gcBottomViewText[92];
DATA(0x00529454) int giThisNetPos;
DATA(0x00529458) signed char gbSetupGamePosToRealGamePos[8];
DATA(0x00529460) char gcRegCDRomPath[0x160];
DATA(0x005295c0) class heroWindow *heroWin;
DATA(0x005295c4) int giOverviewReturnActionExtra;
DATA(0x005295c8) int giCurGeneral;
DATA(0x005295cc) int giThisGamePos;
DATA(0x005295d0) int giNumHumanPlayers;
DATA(0x005295d4) int gbIconClipOn;
DATA(0x005295d8) int gbRemoteGameOpen;
DATA(0x005295dc) combatManager *gpCombatManager;
DATA(0x005295e0) int giTCPNumPlayers;
DATA(0x005295e4) executive *gpExec;
DATA(0x005295e8) void *hMainWindow;
DATA(0x005295ec) int giCurWindowsStyleFlags;
DATA(0x005295f0) int gGameCommand;
DATA(0x005295f4) int giMonthType;
DATA(0x005295f8) char *DEFAULT_AGGREGATE_NAME;
DATA(0x005295fc) int gCurSpellEffectFrame;
DATA(0x00529600) signed char gbThisNetHumanPlayer[8];
DATA(0x00529608) char *cAggPathName;
DATA(0x00529768) class highScoreManager *gpHighScoreManager;
DATA(0x0052976c) int gbFunctionComplete;
DATA(0x00529770) int gbIAmGreatest;
DATA(0x00529774) int gbTextEntryEscaped;
DATA(0x00529778) int giTotalHighMem;
DATA(0x0052977c) int gMapX;
DATA(0x00529780) int gMapY;
DATA(0x00529788) char *gcWinText;
DATA(0x005298b4) int bFreshSave;
DATA(0x005298b8) int bShowIt;
DATA(0x005298bc) int gLowPageScreenSelector;
DATA(0x005298c0) class heroWindowManager *gpWindowManager;
DATA(0x005298c4) int giCurWatchPlayer;
DATA(0x005298c8) char *gcCommandLine;
DATA(0x00529908) int giBottomViewResourceQty;
DATA(0x0052990c) soundManager *gpSoundManager;
DATA(0x00529910) int gbThisNetGotAdventureControl;
DATA(0x00529914) int giMapChangeCtr;
DATA(0x00529918) struct SMapChange *sMapChangeQueue;
DATA(0x0052a184) int gbWaitForRemoteReceive;
DATA(0x0052a188) unsigned char bMusicIsLooping[0x3c];
DATA(0x0052a1c4) townManager *gpTownManager;
DATA(0x0052a1c8) advManager *gpAdvManager;
DATA(0x0052a1d0) signed char gbGamePosToNetPos[152];
