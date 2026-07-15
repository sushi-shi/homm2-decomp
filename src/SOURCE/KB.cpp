// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\KB.OBJ   from: (directly linked into exe)
// functions: 71   data: 441
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <_globals_model.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/town.h>
#include <SOURCE/townManager.h>
#include <SOURCE/ARMY.h>
#include <BASE/executive.h>
#include <BASE/mouseManager.h>
#include <SOURCE/game.h>
#include <SOURCE/GAME.h>
#include <BASE/Misc.h>
#include <BASE/WINMGR.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/ExpCampaign.h>
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
#include <BASE/bmap2.h>
#include <SOURCE/KB.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <io.h>

#include <SOURCE/advManager.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/hero.h>
#include <SOURCE/highScoreManager.h>
#include <SOURCE/philAI.h>
#include <SOURCE/playerData.h>
#include <SOURCE/searchArray.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/iconWidget.h>
#include <BASE/inputManager.h>
#include <BASE/palette.h>
#include <BASE/resourceManager.h>
#include <BASE/soundManager.h>
#include <BASE/icon.h>
#include <BASE/font.h>
#include <BASE/textWidget.h>
#include <BASE/border.h>

// Types now from headers: game/mouseManager/townManager/town/executive + combatManager/
// palette/font -> _all.h; tag_message -> _carcass_types.h; SAMPLE2/tag_monsterInfo/SSpellInfo/
// SWinSetup -> _types.h; SNetPlayerInfo -> REMOTE_TYPES.h;
// SPlayerExit -> KB.h.

#define KBFILE ((char *)"I:\\Projects\\Heroes\\Prog\\SOURCE\\KB.CPP")
#define KBLINE (*(short *)"\xBE\x0C")











static long glNextPollTime;
static long glNextMouseTime;
static long glNextCycleTime;

static char cBlank0[4], cBlank1[4], cBlank2[4], cBlank3[4];

inline town *GetCastleRec(int i)
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

// @early-stop
// Normalized disassembly is identical for all 264 instructions; the frame and
// 62/62 relocation sites also match with no base-only target. The retained
// 99.995% residual is local compiler-symbol identity, not a code-byte mismatch.
VA(0x004965be, 0x39e)
void InitMainClasses(void)
{
    gpExec = new executive;
    gpInputManager = new inputManager;
    gpMouseManager = new mouseManager;
    gpWindowManager = new heroWindowManager;
    gpResourceManager = new resourceManager;
    gpSoundManager = new soundManager;
    gpHighScoreManager = new highScoreManager;
    gpGame = new game;
    gpAdvManager = new advManager;
    gpCombatManager = new combatManager;
    gpTownManager = new townManager;
    gpSearchArray = new searchArray;
    gpPhilAI = new philAI;
    gpMonGroup = new armyGroup;
    gpBufferPalette = new palette;
}

VA(0x0049695c, 0x344)
void DeleteMainClasses(void)
{
    if (gpBufferPalette)
        delete gpBufferPalette;
    gpBufferPalette = 0;
    if (gpMonGroup)
        delete gpMonGroup;
    gpMonGroup = 0;
    if (gpPhilAI)
        delete gpPhilAI;
    gpPhilAI = 0;
    if (gpSearchArray)
        delete gpSearchArray;
    gpSearchArray = 0;
    if (gpTownManager)
        delete gpTownManager;
    gpTownManager = 0;
    if (gpCombatManager)
        delete gpCombatManager;
    gpCombatManager = 0;
    if (gpAdvManager)
        delete gpAdvManager;
    gpAdvManager = 0;
    if (gpGame)
        delete gpGame;
    gpGame = 0;
    if (gpHighScoreManager)
        delete gpHighScoreManager;
    gpHighScoreManager = 0;
    if (gpSoundManager)
        delete gpSoundManager;
    gpSoundManager = 0;
    if (gpWindowManager)
        delete gpWindowManager;
    gpWindowManager = 0;
    if (gpMouseManager)
        delete gpMouseManager;
    gpMouseManager = 0;
    if (gpInputManager)
        delete gpInputManager;
    gpInputManager = 0;
    if (gpExec)
        delete gpExec;
    gpExec = 0;
    if (gpResourceManager)
        delete gpResourceManager;
    gpResourceManager = 0;
}

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

// @match-note
// Pre-95 structural checkpoint (97.74%): exact 0x164 frame and live slots after
// recovering the 0x100 network buffer with its 0xd4 transmitted prefix, the
// shared game-player index, and four retail-unreferenced /Od words. The first
// non-relocation residual is one local continuation jump before the first menu
// table. Base tables are [0x4d1,0x501) and [0x8d6,0x8ea); retail tables are
// [0x4d4,0x504) and [0x8f4,0x908). Relocations are 486/489 with no base-only
// external target. Tried direct giSetupGameType dispatch, both menu case orders,
// inline high-score/credits bodies, and the shared setup label. Revisit at 95%
// for systematic continuation/table-placement steering.
VA(0x00496e98, 0x16c0)
int oldmain(void)
{
    int command_a;
    int quit_g;
    int mainScreenLoaded_b;
    int firstMainScreen_c;
    int savedUpdateFlags_l;
    int player_h;
    // Retail reserves four unreferenced /Od locals around the live state slots.
    int unusedMainState_o;
    int unusedMenuState_d;
    int unusedPlayerState_c;
    int unusedNetworkState_i;
    int netPlayer_i;
    int gamePlayer_m;
    int result_i;
    int transmissionResult_i;
    char matchedNetPlayers_d[OLD_MAIN_MATCH_BUFFER_SIZE];
    char matchedGamePlayers_e[OLD_MAIN_MATCH_BUFFER_SIZE];
    OldMainNetBuffer netBuffer_b;

    if (bKBDone)
        return 0;
    bKBDone = 1;
    LogStr("OM1");
    LogStr("OM2");
    command_a = -1;
    if (gpExec->InitSystem())
        ShutDown("Initialization failed!");
    LogStr("OM3");
    KBChangeMenu(hmnuDflt);
    gPalette = gpResourceManager->GetPalette("kb.pal");
    gpWindowManager->m_updateFlags = 1;
    smallFont = gpResourceManager->GetFont("smalfont.fnt");
    bigFont = gpResourceManager->GetFont("bigfont.fnt");
    gpMouseManager->SetPointer("advmice.mse", 0, OLD_MAIN_POINTER_TYPE);
    gpMouseManager->SetColorMice(gConfig.gfx[giCurExe].colorMouseCursor);
    LogStr("OM4");
    SetupCDRom();
    LogStr("OM5");
    if (gpSoundManager->Open(-1))
        ShutDown("Unable to initialize sound.");
    if (giDebugLevel < OLD_MAIN_DEBUG_MEMORY_CHECK_LEVEL)
        CheckMem();
    LogStr("OM6");

    if (giShowIntro) {
        FillBitmapArea(gpWindowManager->m_screen, 0, 0,
                       OLD_MAIN_SCREEN_WIDTH, OLD_MAIN_SCREEN_HEIGHT, 0);
        BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0,
                           OLD_MAIN_SCREEN_WIDTH, OLD_MAIN_SCREEN_HEIGHT, 0, 0);
        if (!gbSkipIntro) {
            savedUpdateFlags_l = gpWindowManager->m_updateFlags;
            gpWindowManager->m_updateFlags = 0;
            if (PlaySmacker(OLD_MAIN_INTRO_PRIMARY_VIDEO))
                PlaySmacker(OLD_MAIN_INTRO_FALLBACK_VIDEO);
            PlaySmacker(OLD_MAIN_INTRO_SECONDARY_VIDEO);
            gpWindowManager->m_updateFlags = savedUpdateFlags_l;
        }
    }

    LoadSystemwideIcons();
    memset(gbThisNetHumanPlayer, 0, OLD_MAIN_PLAYER_COUNT);
    gpMouseManager->ShowColorPointer();
    quit_g = 0;
    mainScreenLoaded_b = 0;
    firstMainScreen_c = 1;

    while (!quit_g) {
main_menu:
        if (!gShingleAnim)
            gShingleAnim = gpResourceManager->GetIcon("shnganim.icn");
        if (gGameCommand != OLD_MAIN_EXIT)
            gpSoundManager->SwitchAmbientMusic(OLD_MAIN_MAIN_MUSIC);

        if (!mainScreenLoaded_b) {
            if (gGameCommand != OLD_MAIN_EXIT) {
                gpResourceManager->GetBackdrop("heroes.icn", gpWindowManager->m_screen, 1);
                gpWindowManager->UpdateScreenRegion(0, 0,
                                                    OLD_MAIN_SCREEN_WIDTH,
                                                    OLD_MAIN_SCREEN_HEIGHT);
                if (firstMainScreen_c)
                    SetPalette(gPalette->m_data, 1);
                else
                    gpWindowManager->FadeScreen(0, OLD_MAIN_FADE_SPEED,
                                                gPalette);
                firstMainScreen_c = 0;
            }
            gpMouseManager->SetPointer("advmice.mse", 0, OLD_MAIN_POINTER_TYPE);
        }
        mainScreenLoaded_b = 1;
        if (gGameCommand != OLD_MAIN_EXIT)
            gpWindowManager->m_updateFlags = 1;

        if (giTCPHostStatus != -1 && gbTCPFirstTime) {
            gbTCPFirstTime = 0;
            giNumHumanPlayers = 1;
            iMPBaseType = OLD_MAIN_MULTIPLAYER_NETWORK;
            iMPNetProtocol = OLD_MAIN_NETWORK_PROTOCOL;
            iMPExtendedType = giTCPHostStatus ? OLD_MAIN_REMOTE_HOST
                                             : OLD_MAIN_REMOTE_CLIENT;
            giSetupGameType = static_cast<unsigned char>(giTCPType);
            RemoteMain(iMPExtendedType);
            gbWaitForRemoteReceive =
                iMPExtendedType == OLD_MAIN_REMOTE_CLIENT;
            giTCPHostStatus = -1;
            switch (giSetupGameType) {
            case OLD_MAIN_SETUP_NEW:
                if (!gpGame->NewGame())
                    goto main_menu;
                break;
            case OLD_MAIN_SETUP_LOAD:
                if (!gpGame->PickLoadGame())
                    goto main_menu;
                break;
            }
            goto game_setup_complete;
        }

process_menu_command:
        if (giMenuCommand != -1) {
            switch (giMenuCommand) {
            case APP_MENU_RESTART_0:
            case APP_MENU_RESTART_1:
            case APP_MENU_RESTART_2:
            case APP_MENU_RESTART_3:
            case APP_MENU_RESTART_4:
            case APP_MENU_RESTART_5:
            case APP_MENU_RESTART_6:
            case APP_MENU_RESTART_7:
            case APP_MENU_RESTART_8:
            case APP_MENU_RESTART_9:
            case APP_MENU_RESTART_10:
            case APP_MENU_RESTART_11:
            case APP_MENU_RESTART_12:
            case APP_MENU_RESTART_13:
                result_i = gpGame->NewGame();
                break;
            default:
                result_i = 1;
                break;
            case APP_MENU_LOAD_0:
            case APP_MENU_LOAD_1:
            case APP_MENU_LOAD_2:
            case APP_MENU_LOAD_3:
            case APP_MENU_LOAD_4:
            case APP_MENU_LOAD_5:
            case APP_MENU_LOAD_6:
            case APP_MENU_LOAD_7:
            case APP_MENU_LOAD_8:
            case APP_MENU_LOAD_9:
            case APP_MENU_LOAD_10:
                result_i = gpGame->PickLoadGame();
                break;
            }
            if (!result_i)
                goto main_menu;
            command_a = -1;
            goto game_setup_complete;
        }

        if (gGameCommand == -1) {
            gpInitWin = new heroWindow(0, 0, "stpmain.bin");
            if (!gpInitWin)
                MemError();
            gbInSetupDialog = 1;
            gpWindowManager->DoDialog(gpInitWin, InitMenuHandler, 0);
            delete gpInitWin;
            gpInitWin = 0;
            command_a = gpWindowManager->m_dialogResult;
            gbInSetupDialog = 0;
        } else {
            command_a = gGameCommand;
            gGameCommand = -1;
        }
        if (giMenuCommand != -1)
            goto process_menu_command;

        switch (command_a) {
        case OLD_MAIN_LOAD_GAME:
            giSetupGameType = OLD_MAIN_SETUP_LOAD;
            goto setup_selected;
        case OLD_MAIN_NEW_GAME:
            giSetupGameType = OLD_MAIN_SETUP_NEW;

setup_selected:
            for (player_h = 0; player_h < OLD_MAIN_PLAYER_COUNT; player_h++)
                sprintf(cPlayerNames[player_h], "");
            if (!gpGame->SetupGame())
                goto main_menu;

            result_i = giSetupGameType;
            if (result_i == OLD_MAIN_SETUP_NEW) {
                if (gbInCampaign) {
                    gpGame->InitEntireCampaign(gbCampaignSideChoice);
                    if (gpGame->HandleCampaignWin()) {
                        gpGame->InitCampaignMap();
                        goto initialize_game;
                    } else {
                        gpWindowManager->FadeScreen(1, OLD_MAIN_FADE_SPEED,
                                                    gPalette);
                        mainScreenLoaded_b = 0;
                        goto main_menu;
                    }
                }
                if (xIsPlayingExpansionCampaign) {
                    if (xCampaign.HandleVictory()) {
                        xCampaign.InitMap();
                        goto initialize_game;
                    }
                    gpWindowManager->FadeScreen(1, OLD_MAIN_FADE_SPEED, gPalette);
                    mainScreenLoaded_b = 0;
                    goto main_menu;
                }
                LogStr("New Game 1");
                if (!gpGame->NewGame())
                    goto main_menu;
                LogStr("New Game 2");
            } else if (result_i == OLD_MAIN_SETUP_LOAD) {
                LogStr("Load Game 1");
                if (!gpGame->PickLoadGame())
                    goto main_menu;
                LogStr("Load Game 2");
            }
            goto game_setup_complete;
        case OLD_MAIN_HIGH_SCORES:
            if (gpExec->AddManager(gpHighScoreManager, -1))
                ShutDown("Can't add manager!");
            gpExec->MainLoop();
            gpExec->RemoveManager(gpHighScoreManager);
            mainScreenLoaded_b = 0;
            goto main_menu;
        case OLD_MAIN_CREDITS:
            gpWindowManager->FadeScreen(1, OLD_MAIN_FADE_SPEED, gPalette);
            PlaySmacker(OLD_MAIN_CREDITS_FIRST_VIDEO);
            PlaySmacker(OLD_MAIN_CREDITS_SECOND_VIDEO);
            mainScreenLoaded_b = 0;
            gpWindowManager->FadeScreen(1, OLD_MAIN_LONG_FADE_SPEED, gPalette);
            goto main_menu;
        case OLD_MAIN_EXIT:
            quit_g = 1;
            goto game_setup_complete;
        default:
            goto game_setup_complete;
        }

game_setup_complete:
        if (giMenuCommand == -1) {
            LogStr("DWM 1");
            if (quit_g)
                goto game_finished;
            LogStr("DWM 2");
            if (gbRemoteOn && giThisNetPos == 0) {
                LogStr("DWM 3");
                memset(matchedGamePlayers_e, 0, OLD_MAIN_PLAYER_COUNT);
                memset(matchedNetPlayers_d, 0, OLD_MAIN_PLAYER_COUNT);
                for (netPlayer_i = 0; netPlayer_i < OLD_MAIN_PLAYER_COUNT; netPlayer_i++) {
                    if (gbHumanPlayer[netPlayer_i]) {
                        for (gamePlayer_m = 0; gamePlayer_m < OLD_MAIN_PLAYER_COUNT; gamePlayer_m++) {
                            char *defaultName =
                                &gpGame->m_defaultPlayerNames[
                                    gamePlayer_m * OLD_MAIN_DEFAULT_NAME_STRIDE];
                            if (strlen(defaultName) == OLD_MAIN_DEFAULT_NAME_LENGTH &&
                                !strcmp(defaultName, gsNetPlayerInfo[netPlayer_i].name) &&
                                !gpGame->m_playerDead[gamePlayer_m] &&
                                !matchedGamePlayers_e[gamePlayer_m] &&
                                !matchedNetPlayers_d[netPlayer_i]) {
                                matchedGamePlayers_e[gamePlayer_m] = 1;
                                matchedNetPlayers_d[netPlayer_i] = 1;
                                gbGamePosToNetPos[gamePlayer_m] = static_cast<signed char>(netPlayer_i);
                            }
                        }
                    }
                }
                for (gamePlayer_m = 0;
                     gamePlayer_m < OLD_MAIN_PLAYER_COUNT &&
                         matchedGamePlayers_e[gamePlayer_m];
                     gamePlayer_m++) {
                }
                for (netPlayer_i = 0; netPlayer_i < OLD_MAIN_PLAYER_COUNT; netPlayer_i++) {
                    if (!matchedNetPlayers_d[netPlayer_i]) {
                        if (!gbHumanPlayer[netPlayer_i]) {
                            gbGamePosToNetPos[netPlayer_i] = -1;
                        } else {
                            gbGamePosToNetPos[netPlayer_i] =
                                static_cast<signed char>(gamePlayer_m);
                            strcpy(&gpGame->m_defaultPlayerNames[
                                       gamePlayer_m *
                                       OLD_MAIN_DEFAULT_NAME_STRIDE],
                                   gsNetPlayerInfo[netPlayer_i].name);
                            for (gamePlayer_m++;
                                 gamePlayer_m < OLD_MAIN_PLAYER_COUNT &&
                                     matchedGamePlayers_e[gamePlayer_m];
                                 gamePlayer_m++) {
                            }
                        }
                    }
                }

                memcpy(netBuffer_b.setup.gamePosToNetPos, gbGamePosToNetPos,
                       OLD_MAIN_PLAYER_COUNT);
                memcpy(netBuffer_b.setup.players, gsNetPlayerInfo,
                       sizeof(netBuffer_b.setup.players));
                giThisGamePos = NetPosToGamePos(0);
                gbUseDiffCompression = 1;
                gbUseRegularCompression =
                    giHighMemBuffer > OLD_MAIN_REGULAR_COMPRESSION_MEMORY_LIMIT;
                for (netPlayer_i = 0; netPlayer_i < giNumHumanPlayers; netPlayer_i++) {
                    if (!gsNetPlayerInfo[netPlayer_i].useRegularCompression)
                        gbUseRegularCompression = 0;
                    if (!gsNetPlayerInfo[netPlayer_i].useDiffCompression)
                        gbUseDiffCompression = 0;
                }
                netBuffer_b.setup.useRegularCompression = gbUseRegularCompression;
                netBuffer_b.setup.useDiffCompression = gbUseDiffCompression;
                for (netPlayer_i = 1; netPlayer_i < giNumHumanPlayers; netPlayer_i++) {
                    transmissionResult_i = TransmitRemoteData(
                        netBuffer_b.bytes, netPlayer_i, OLD_MAIN_NET_SETUP_SIZE,
                        OLD_MAIN_NETWORK_PACKET, 1, 1, -1);
                    if (!transmissionResult_i)
                        ShutDown(0);
                }
                for (netPlayer_i = 1; netPlayer_i < giNumHumanPlayers; netPlayer_i++) {
                    if (!gpGame->TransmitSaveGame(netPlayer_i, 0, 1))
                        ShutDown(0);
                }
                memset(gbThisNetHumanPlayer, 0, OLD_MAIN_PLAYER_COUNT);
                gbThisNetHumanPlayer[giThisGamePos] = 1;
                iLastDiffSendTo = -1;
                gpGame->SaveGame(gConfig.rmtRLName, 0, 0);
            }
            LogStr("DWM 4");
            if (gbRemoteOn && gbWaitForRemoteReceive) {
                LogStr("DWM 5");
                giWaitType = 0;
                NormalDialog("Waiting to receive game data.", OLD_MAIN_DIALOG_WAIT,
                             -1, -1, -1, 0, -1, 0, -1, 0);
                if (!gbFunctionComplete)
                    ShutDown(0);
                gpGame->LoadGame(gConfig.rmtRCName, 0, 1);
                sprintf(gpGame->m_saveName, "NEWGAME");
                iLastDiffSendTo = -1;
                gpGame->SaveGame(gConfig.rmtSLName, 0, 0);
            }
        }

initialize_game:
        gpWindowManager->m_updateFlags = 1;
        if (gShingleAnim)
            gpResourceManager->Dispose(gShingleAnim);
        gShingleAnim = 0;

        if (giNumHumanPlayers > 1) {
            for (player_h = 0; player_h < giNumHumanPlayers; player_h++) {
                if (iMPBaseType != OLD_MAIN_MULTIPLAYER_LOCAL)
                    strcpy(cPlayerNames[NetPosToGamePos(player_h)],
                           gsNetPlayerInfo[player_h].name);
            }
        }
        for (player_h = 0; player_h < gpGame->m_playerCount; player_h++) {
            if (!strlen(cPlayerNames[player_h])) {
                sprintf(cPlayerNames[player_h], "%s player",
                        gColors[gpGame->m_players[player_h].m_color]);
                cPlayerNames[player_h][0] -= ' ';
            }
        }
        ComputeAdvNetControl();
        gbGameInitialized = 1;
        mainScreenLoaded_b = 0;
        gpSoundManager->StopAllSamples(1);
        gpWindowManager->FadeScreen(1, OLD_MAIN_FADE_SPEED, 0);
        gMapX = 0;
        gMapY = 0;
        for (giCurWatchPlayer = giCurPlayer;
             !gbThisNetHumanPlayer[giCurWatchPlayer];
             giCurWatchPlayer = (giCurWatchPlayer + 1) % gpGame->m_playerCount) {
        }
        giCurWatchPlayerBit = static_cast<unsigned char>(1 << giCurWatchPlayer);

        if (gbInCampaign && gpGame->m_campaignScenarioWon) {
            giEndSequence = 1;
        } else if (xIsPlayingExpansionCampaign && xCampaign.IsThisMapCompleted()) {
            giEndSequence = 1;
        } else {
            if (gpExec->AddManager(gpAdvManager, -1))
                ShutDown("Can't add manager!");
            if (command_a == OLD_MAIN_NEW_GAME) {
                gpAdvManager->SetHeroContext(
                    gpGame->m_players[0].NextHero(0), 0);
            }
            if (command_a == OLD_MAIN_NEW_GAME || bForceCheckTimeEvent) {
                bForceCheckTimeEvent = 0;
                gpGame->CheckForTimeEvent();
            }
            gpExec->MainLoop();
            gMapX = gpAdvManager->m_mapOriginX;
            gMapY = gpAdvManager->m_mapOriginY;
            gpExec->RemoveManager(gpAdvManager);
            gpWindowManager->FadeScreen(1, OLD_MAIN_FADE_SPEED, gPalette);
            if (!gbGameOver)
                goto game_finished;
        }

        RemoteCleanup();
        bShowIt = 1;
        gpMouseManager->SetPointer("advmice.mse", 0, OLD_MAIN_POINTER_TYPE);
        sprintf(reinterpret_cast<char *>(&gcWinText),
                "My heroes, our foes have been scattered, their castles broken and laid bare.  The great campaign is now complete, and I stand before you as the undisputed High King!",
                giCurTurn);

        if (giEndSequence != 1) {
            gpWindowManager->m_updateFlags = 0;
            if (!xIsExpansionMap)
                PlaySmacker(OLD_MAIN_STANDARD_VICTORY_VIDEO);
            else
                PlaySmacker(OLD_MAIN_EXPANSION_VICTORY_VIDEO);
            gpWindowManager->FadeScreen(1, OLD_MAIN_LONG_FADE_SPEED, gPalette);
            gpResourceManager->GetBackdrop("heroes.icn", gpWindowManager->m_screen, 1);
            gpWindowManager->UpdateScreenRegion(0, 0,
                                                OLD_MAIN_SCREEN_WIDTH,
                                                OLD_MAIN_SCREEN_HEIGHT);
            gpWindowManager->FadeScreen(0, OLD_MAIN_FADE_SPEED, gPalette);
            gpWindowManager->m_updateFlags = 1;
            mainScreenLoaded_b = 1;
            gpSoundManager->PlayAmbientMusic(OLD_MAIN_MAIN_MUSIC, 0, -1);
        } else if (gbInCampaign) {
            result_i = gpGame->HandleCampaignWin();
            if ((gpGame->m_campaignScenario == OLD_MAIN_ARCHIBALD_FINAL_SCENARIO &&
                 gpGame->m_campaignScenarioCompleted[gpGame->m_campaignType]
                                                    [OLD_MAIN_ARCHIBALD_FINAL_SCENARIO]) ||
                (gpGame->m_campaignScenario == OLD_MAIN_ROLAND_FINAL_SCENARIO &&
                 gpGame->m_campaignType == OLD_MAIN_ROLAND_CAMPAIGN &&
                 gpGame->m_campaignScenarioCompleted[OLD_MAIN_ROLAND_CAMPAIGN]
                                                    [OLD_MAIN_ROLAND_FINAL_SCENARIO])) {
                gbShowHighScore = 1;
                ShowCongrats(CONGRATS_CAMPAIGN);
                AddScoreToHighScore(gpGame->m_campaignScore,
                                    gpGame->m_campaignScore, 0,
                                    CONGRATS_CAMPAIGN,
                                    gpGame->m_campaignType ? "Archibald" : "Roland");
            }
            if (result_i) {
                for (player_h = 0; player_h < OLD_MAIN_PLAYER_COUNT; player_h++)
                    sprintf(cPlayerNames[player_h], "");
                gpGame->InitCampaignMap();
                gbGameOver = 0;
                bForceCheckTimeEvent = 1;
                goto initialize_game;
            }
        } else if (xIsPlayingExpansionCampaign) {
            result_i = xCampaign.HandleVictory();
            if (xCampaign.IsCompleted()) {
                gbShowHighScore = 1;
                ShowCongrats(CONGRATS_EXPANSION_CAMPAIGN);
                AddScoreToHighScore(xCampaign.Days(), xCampaign.Days(), 0,
                                    CONGRATS_EXPANSION_CAMPAIGN,
                                    xHSCampaignNames[xCampaign.CampaignID()]);
            }
            if (result_i) {
                for (player_h = 0; player_h < OLD_MAIN_PLAYER_COUNT; player_h++)
                    sprintf(cPlayerNames[player_h], "");
                xCampaign.InitMap();
                gbGameOver = 0;
                bForceCheckTimeEvent = 1;
                goto initialize_game;
            }
        } else {
            ShowCongrats(CONGRATS_STANDARD);
            if (!gbShowHighScore) {
                gpWindowManager->FadeScreen(1, OLD_MAIN_FADE_SPEED, gPalette);
                gpResourceManager->GetBackdrop("heroes.icn", gpWindowManager->m_screen, 1);
                gpWindowManager->UpdateScreenRegion(
                    0, 0, OLD_MAIN_SCREEN_WIDTH, OLD_MAIN_SCREEN_HEIGHT);
                gpWindowManager->FadeScreen(0, OLD_MAIN_FADE_SPEED, gPalette);
                gpWindowManager->m_updateFlags = 1;
                mainScreenLoaded_b = 1;
                gpSoundManager->PlayAmbientMusic(OLD_MAIN_MAIN_MUSIC, 0, -1);
            } else {
                gpSoundManager->PlayAmbientMusic(OLD_MAIN_HIGH_SCORE_MUSIC, 0, -1);
            }
        }

        gbGameOver = 0;
        if (gbShowHighScore) {
            gbShowHighScore = 0;
            if (gpExec->AddManager(gpHighScoreManager, -1))
                ShutDown("Can't add manager!");
            gpExec->MainLoop();
            gpExec->RemoveManager(gpHighScoreManager);
            giHighScoreRank = -1;
            gpSoundManager->SwitchAmbientMusic(OLD_MAIN_MAIN_MUSIC);
            gpResourceManager->GetBackdrop("heroes.icn", gpWindowManager->m_screen, 1);
            gpWindowManager->UpdateScreenRegion(0, 0,
                                                OLD_MAIN_SCREEN_WIDTH,
                                                OLD_MAIN_SCREEN_HEIGHT);
            gpWindowManager->FadeScreen(0, OLD_MAIN_FADE_SPEED, gPalette);
            mainScreenLoaded_b = 1;
        }

game_finished:
        if (gbRemoteOn)
            quit_g = 1;
    }

    ShutDown(0);
    return 0;
}

VA(0x00498558, 0x44)
char toupper(char c)
{
    if (c >= 'a' && c <= 'z')
        return c - ' ';
    else
        return c;
}

// @match-note
// Pre-95 structural checkpoint (retained 99.831%): exact 0x28 frame and 139/139
// relocation sites with no base-only target. The first code residual is the
// outer command-line scan backedge (`jge` here versus retail `jle`); subsequent
// diff alignment is obscured by two delinked character tables at +0x567 and
// +0x5bd. The direct buffer-index and positive-bound spellings are represented;
// revisit at 95% for systematic loop-polarity/table steering.
VA(0x0049859c, 0x791)
int InterpretCommandLine(void)
{
    int len;
    int i;
    int helpRequested;
    gbTCPFirstTime = 1;
    giTCPType = -1;
    giTCPHostStatus = -1;
    giTCPNumPlayers = -1;
    strcpy(gcTCPAddress, "");
    strcpy(gcTCPName, "");
    gbUseWaveout = 0;
    giDebugLevel = 0;
    giShowIntro = 1;
    gbNoSound = 0;
    gbCheatMenus = 0;
    giScreenScroll = 1;
    giLimitPlayer = 0;
    gbBlackoutPlayer = 1;
    helpRequested = 0;
    strcpy(gMapName, "Chaos.mp2");
    strcpy(gFullMapName, "Chaos");

    len = strlen(gcCommandLine);
    for (i = 0; len > i; i++) {
        if (gcCommandLine[i] == ' ' &&
            i + 1 < len &&
            (gcCommandLine[i + 1] == '?' ||
             gcCommandLine[i + 1] == 'h' ||
             gcCommandLine[i + 1] == 'H')) {
            helpRequested = 1;
        }
        if (gcCommandLine[i] == '/' && i + 1 < len) {
            switch (toupper(gcCommandLine[i + 1])) {
        case 'Z':
            gbDoMemCheck = 0;
            break;
        case 'W':
            gbUseWaveout = 1;
            if (i + 2 < len)
                gbUseWaveout =
                    gcCommandLine[i + 2] - '0';
            break;
        case 'V':
            gConfig.slowVideo = 1;
            WritePrefs();
            break;
        case 'N':
            if (i + 3 < len &&
                toupper(gcCommandLine[i + 2]) == 'W' &&
                toupper(gcCommandLine[i + 3]) == 'C') {
                gbCheatMenus = 1;
            }
            break;
        case 'M':
            if (i + 2 < len)
                gbDontTryMIDI = 1 -
                    (gcCommandLine[i + 2] - '0');
            break;
        case 'R':
            if (i + 2 < len)
                gbDontTryRedbook = 1 -
                    (gcCommandLine[i + 2] - '0');
            break;
        case 'D':
            if (i + 2 < len)
                gbDontTryDigital = 1 -
                    (gcCommandLine[i + 2] - '0');
            break;
        case 'S':
            if (i + 2 < len)
                gbNoSound = 1 -
                    (gcCommandLine[i + 2] - '0');
            break;
        case 'I':
            if (i + 2 < len)
                giShowIntro =
                    gcCommandLine[i + 2] - '0';
            break;
        case 'P':
            if (i + 2 < len)
                giDebugLevel =
                    gcCommandLine[i + 2] - '0';
            break;
        case 'X':
            xSmackFromNetwork = 1;
            break;
        case 'T':
            if (i + 2 < len) {
                switch (toupper(gcCommandLine[i + 2])) {
            case 'T':
                if (i + 3 < len &&
                    toupper(gcCommandLine[i + 3]) == 'L') {
                    giTCPType = COMMAND_LINE_TCP_TYPE_L;
                } else {
                    giTCPType = COMMAND_LINE_TCP_TYPE_DEFAULT;
                }
                break;
            case 'S':
                if (i + 3 < len &&
                    toupper(gcCommandLine[i + 3]) == 'H') {
                    giTCPHostStatus = COMMAND_LINE_TCP_HOST;
                } else {
                    giTCPHostStatus = COMMAND_LINE_TCP_CLIENT;
                }
                break;
            case 'P':
            {
                int count = 0;
                if (i + 3 < len)
                    count = gcCommandLine[i + 3] - '0';
                if (count >= COMMAND_LINE_TCP_MIN_PLAYERS &&
                    count <= COMMAND_LINE_TCP_MAX_PLAYERS) {
                    giTCPNumPlayers = count;
                }
                break;
            }
            case 'A':
            {
                if (i + 3 < len) {
                    int dst = 0;
                    int src = i + 3;
                    while (dst < COMMAND_LINE_TCP_TEXT_LENGTH &&
                           gcCommandLine[src] && gcCommandLine[src] != ' ') {
                        gcTCPAddress[dst] = gcCommandLine[src];
                        src++;
                        dst++;
                    }
                    gcTCPAddress[dst] = 0;
                }
                break;
            }
            case 'N':
            {
                if (i + 3 < len) {
                    int dst = 0;
                    int src = i + 3;
                    while (dst < COMMAND_LINE_TCP_TEXT_LENGTH &&
                           gcCommandLine[src] && gcCommandLine[src] != ' ') {
                        gcTCPName[dst] = gcCommandLine[src];
                        src++;
                        dst++;
                    }
                    gcTCPName[dst] = 0;
                }
                break;
            }
                }
            }
            break;
            }
        }
    }

    if (helpRequested) {
        sprintf(gText, "");
        for (i = 0; i < COMMAND_LINE_HELP_LINE_COUNT; i++)
            strcat(gText, gcCommandLineHelp[i]);
        ShutDown(gText);
    }

    sprintf(cAggPathName, "%s%s", ".\\DATA\\", "heroes2.agg");
    DEFAULT_AGGREGATE_NAME = cAggPathName;
    sprintf(cExpAggPathName, "%s%s", ".\\DATA\\", "heroes2x.agg");
    EXPANSION_AGGREGATE_NAME = cExpAggPathName;
    giFrameStep = COMMAND_LINE_FRAME_STEP;

    for (i = 0; i < COMMAND_LINE_HUMAN_PLAYER_SLOTS; i++) {
        if (giNumHumanPlayers > i)
            gbHumanPlayer[i] = 1;
        else
            gbHumanPlayer[i] = 0;
    }
    if (giNumHumanPlayers == COMMAND_LINE_SINGLE_PLAYER)
        gbBlackoutPlayer = 0;

    if (giTCPHostStatus != -1) {
        if (giTCPType == -1 || giTCPNumPlayers == -1 ||
            (giTCPHostStatus == COMMAND_LINE_TCP_CLIENT &&
             strlen(gcTCPAddress) < 1)) {
            ShutDown("Incomplete TCP/IP command line information");
        }
        giShowIntro = 0;
    }
    return 1;
}

// @match-note
// Pre-95 structural checkpoint (retained 99.565%): exact 0x24 frame, complete
// event/help switch bodies, and 89/89 relocation sites with no base-only target.
// The first residual is the first delinked switch table at +0xad; the second is
// at +0x1ab, after which the generic diff parser loses local-label alignment.
// Direct retail-order case bodies are present. Revisit at 95% for explicit-range
// table exclusion and systematic continuation steering.
VA(0x00498d2d, 0x698)
int InitMenuHandler(struct tag_message &msg)
{
    int handled = 0;
    int idx;
    int menu;
    int helpIndex;
    int hoverIndex;

    PollSound();
    if (msg.payload.widget.parameter & INIT_MENU_DISABLE_MASK) {
        if (msg.payload.widget.command == INIT_MENU_HOVER_COMMAND ||
            msg.payload.widget.command == INIT_MENU_HELP_COMMAND) {
            helpIndex = -1;
            switch (msg.payload.widget.id) {
            case INIT_MENU_NEW_GAME:
                helpIndex = 0;
                break;
            case INIT_MENU_LOAD_GAME:
                helpIndex = 1;
                break;
            case INIT_MENU_HIGH_SCORES:
                helpIndex = 2;
                break;
            case INIT_MENU_CREDITS:
                helpIndex = 3;
                break;
            case INIT_MENU_EXIT:
                helpIndex = 4;
                break;
            }
            if (helpIndex >= 0) {
                NormalDialog(gInitMenuHelp[helpIndex], INIT_MENU_HELP_DIALOG,
                             -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    } else {
        if (msg.type == INIT_MENU_KEY_PRESS) {
            switch (msg.payload.keyboard.keyCode) {
            case INIT_MENU_KEY_NEW:
                gpWindowManager->m_dialogResult = INIT_MENU_NEW_GAME;
                handled = 1;
                break;
            case INIT_MENU_KEY_LOAD:
                gpWindowManager->m_dialogResult = INIT_MENU_LOAD_GAME;
                handled = 1;
                break;
            case INIT_MENU_KEY_CREDITS:
                gpWindowManager->m_dialogResult = INIT_MENU_CREDITS;
                handled = 1;
                break;
            case INIT_MENU_KEY_HIGH_SCORES:
                gpWindowManager->m_dialogResult = INIT_MENU_HIGH_SCORES;
                handled = 1;
                break;
            case INIT_MENU_KEY_EXIT:
                gpWindowManager->m_dialogResult = INIT_MENU_EXIT;
                handled = 1;
                break;
            }
        } else if (msg.type == INIT_MENU_MESSAGE) {
            if (msg.payload.widget.id < INIT_MENU_FIRST_COMMAND ||
                msg.payload.widget.id > INIT_MENU_LAST_ACTION) {
                return INIT_MENU_HANDLER_IGNORE;
            }
            switch (msg.payload.widget.command) {
            case INIT_MENU_HOVER_COMMAND:
                if (msg.payload.widget.id == INIT_MENU_MOVIE)
                    break;
                menu = msg.payload.widget.id - INIT_MENU_FIRST_COMMAND;
                idx = menu + INIT_MENU_WIDGET_OFFSET;
                msg.type = INIT_MENU_MESSAGE;
                msg.payload.widget.id = idx;
                msg.payload.widget.command = INIT_MENU_SET_WIDGET_COMMAND;
                msg.payload.widget.data.value =
                    menu * INIT_MENU_FRAME_STRIDE + INIT_MENU_HOVER_FRAME;
                gpInitWin->BroadcastMessage(msg);
                gpInitWin->DrawWindow(0, idx, idx);
                gpWindowManager->UpdateScreenRegion(
                    IMHotSpots[menu][0], IMHotSpots[menu][1],
                    IMHotSpots[menu][2], IMHotSpots[menu][3]);
                break;
            case INIT_MENU_CLICK_COMMAND:
                if (msg.payload.widget.id == INIT_MENU_MOVIE) {
                    PlaySmacker(INIT_MENU_MOVIE_SMACKER);
                    gpResourceManager->GetBackdrop(
                        "heroes.icn", gpWindowManager->m_screen, 1);
                    gpInitWin->DrawWindow(0);
                    gpWindowManager->UpdateScreenRegion(
                        0, 0, INIT_MENU_SCREEN_WIDTH, INIT_MENU_SCREEN_HEIGHT);
                    gpSoundManager->PlayAmbientMusic(
                        INIT_MENU_MAIN_MUSIC, 0, -1);
                    break;
                } else {
                    gpWindowManager->m_dialogResult = msg.payload.widget.id;
                    for (idx = INIT_MENU_FIRST_WIDGET;
                         idx <= INIT_MENU_LAST_WIDGET; idx++) {
                        msg.type = INIT_MENU_MESSAGE;
                        msg.payload.widget.id = idx;
                        msg.payload.widget.command = INIT_MENU_SET_WIDGET_COMMAND;
                        msg.payload.widget.data.value = idx * INIT_MENU_FRAME_STRIDE -
                                      INIT_MENU_WIDGET_FRAME_BASE;
                        gpInitWin->BroadcastMessage(msg);
                    }
                    gpInitWin->DrawWindow(
                        0, INIT_MENU_FIRST_WIDGET, INIT_MENU_LAST_WIDGET);
                    gpWindowManager->UpdateScreenRegion(
                        INIT_MENU_REDRAW_LEFT, INIT_MENU_REDRAW_TOP,
                        INIT_MENU_REDRAW_WIDTH, INIT_MENU_REDRAW_HEIGHT);
                    handled = 1;
                }
                break;
            }
        } else if (msg.type == INIT_MENU_MOUSE_MOVE) {
            hoverIndex = -1;
            for (idx = 0; idx < INIT_MENU_HOTSPOT_COUNT; idx++) {
                if (IMHotSpots[idx][0] <= msg.payload.mouse.screenX &&
                    IMHotSpots[idx][1] <= msg.payload.mouse.screenY &&
                    msg.payload.mouse.screenX < IMHotSpots[idx][0] + IMHotSpots[idx][2] &&
                    msg.payload.mouse.screenY < IMHotSpots[idx][1] + IMHotSpots[idx][3]) {
                    hoverIndex = idx;
                }
            }
            if (lastIMHoverID != hoverIndex) {
                if (lastIMHoverID != -1) {
                    msg.type = INIT_MENU_MESSAGE;
                    msg.payload.widget.id = lastIMHoverID + INIT_MENU_WIDGET_OFFSET;
                    msg.payload.widget.command = INIT_MENU_SET_WIDGET_COMMAND;
                    msg.payload.widget.data.value = lastIMHoverID * INIT_MENU_FRAME_STRIDE +
                                  INIT_MENU_IDLE_FRAME;
                    gpInitWin->BroadcastMessage(msg);
                    gpInitWin->DrawWindow(0, lastIMHoverID + INIT_MENU_WIDGET_OFFSET,
                                          lastIMHoverID + INIT_MENU_WIDGET_OFFSET);
                    gpWindowManager->UpdateScreenRegion(
                        IMHotSpots[lastIMHoverID][0], IMHotSpots[lastIMHoverID][1],
                        IMHotSpots[lastIMHoverID][2], IMHotSpots[lastIMHoverID][3]);
                }
                if (hoverIndex != -1) {
                    msg.type = INIT_MENU_MESSAGE;
                    msg.payload.widget.id = hoverIndex + INIT_MENU_WIDGET_OFFSET;
                    msg.payload.widget.command = INIT_MENU_SET_WIDGET_COMMAND;
                    msg.payload.widget.data.value = hoverIndex * INIT_MENU_FRAME_STRIDE +
                                  INIT_MENU_ACTIVE_FRAME;
                    gpInitWin->BroadcastMessage(msg);
                    gpInitWin->DrawWindow(0, hoverIndex + INIT_MENU_WIDGET_OFFSET,
                                          hoverIndex + INIT_MENU_WIDGET_OFFSET);
                    gpWindowManager->UpdateScreenRegion(
                        IMHotSpots[hoverIndex][0], IMHotSpots[hoverIndex][1],
                        IMHotSpots[hoverIndex][2], IMHotSpots[hoverIndex][3]);
                }
                lastIMHoverID = hoverIndex;
            }
        }
    }

    if (handled || giMenuCommand != -1) {
        msg.type = INIT_MENU_MESSAGE;
        msg.payload.widget.id = INIT_MENU_CLOSE_COMMAND;
        msg.payload.widget.command = msg.payload.widget.id;
        return INIT_MENU_HANDLER_CLOSE;
    }
    CheckShingleUpdate();
    return INIT_MENU_HANDLER_CONTINUE;
}

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
        switch (msg.payload.widget.command) {
        case 0xc:
            switch (msg.payload.widget.id) {
            case 2:
                HeroView(static_cast<unsigned char>(gpTownManager->m_recruitHero->m_id), 1, 0);
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
            switch (msg.payload.widget.id) {
            case 0x7801:
                gpTownManager->m_recruitState = -1;
                a = 1;
                break;
            case 0x7802:
                gpTownManager->m_recruitState = 0;
                *(int *)((char *)gpWindowManager + 0x5a) = msg.payload.widget.id;
                a = 1;
                break;
            }
            break;
        default:
            break;
        }
    }
    if (a == 1) {
        msg.payload.widget.id = 0xa;
        msg.payload.widget.command = msg.payload.widget.id;
        return 2;
    }
    return 1;
}

VA(0x00499589, 0x1a7)
char *GetBuildingInfo(int race, int building, int mode)
{
    char buf[400];
    if (race == TOWN_TYPE_NECROMANCER && building == KB_BUILDING_NECROMANCER_SHRINE) {
        sprintf(buf, xNecromancerShrineDesc);
    } else if (building == KB_BUILDING_WELL_EXTRA) {
        sprintf(buf, "The %s increases production of %s by 8 per week.",
                GetBuildingName(race, building),
                gArmyNamesPlural[gDwellingType[race][0]]);
    } else if (building == KB_BUILDING_SPECIAL) {
        sprintf(buf, gBuildingInfoSpecial[race]);
    } else if (building < KB_BUILDING_DWELLING_FIRST) {
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
    if (race == TOWN_TYPE_NECROMANCER && building == KB_BUILDING_NECROMANCER_SHRINE)
        return xNecromancerShrine;
    if (building == KB_BUILDING_WELL_EXTRA)
        return gWellExtraNames[race];
    else if (building == KB_BUILDING_SPECIAL)
        return gSpecialBuildingNames[race];
    else if (building < KB_BUILDING_DWELLING_FIRST)
        return gNeutralBuildingNames[building];
    else
        return gDwellingNames[race][building - KB_BUILDING_DWELLING_FIRST];
}

// @match-note
// ~98%: 2-instruction /Od body-placement variance — retail emits the final
// `else if (building<0x10)` body out-of-line (`jl body; jmp epi`); this build lowers it
// inline (`jge skip`). Logic byte-exact otherwise; resisted every source structure tried.
// Revisit at 95% for systematic body-placement steering.
VA(0x004997d4, 0x138)
void GetBuildingCost(int race, int building, int *const dest, int mageLevel)
{
    int level;
    if (building == KB_BUILDING_NECROMANCER_SHRINE && race == TOWN_TYPE_NECROMANCER) {
        memcpy(dest, xShrineBuildingCost, KB_BUILDING_RESOURCE_COUNT * sizeof(int));
    } else if (building >= KB_BUILDING_DWELLING_FIRST && building <= KB_BUILDING_DWELLING_LAST) {
        memcpy(dest, gDwellingCosts[race][building - KB_BUILDING_DWELLING_FIRST],
               KB_BUILDING_RESOURCE_COUNT * sizeof(int));
    } else if (building == KB_BUILDING_MAGE_GUILD) {
        level = mageLevel + 1;
        if (level > KB_MAGE_GUILD_MAX_LEVEL)
            level = KB_MAGE_GUILD_MAX_LEVEL;
        memcpy(dest, gMageBuildingCosts[mageLevel + 1], KB_BUILDING_RESOURCE_COUNT * sizeof(int));
    } else if (building == KB_BUILDING_SPECIAL) {
        memcpy(dest, gSpecialBuildingCosts[race], KB_BUILDING_RESOURCE_COUNT * sizeof(int));
    } else if (building < KB_BUILDING_NEUTRAL_LIMIT) {
        memcpy(dest, gNeutralBuildingCosts[building], KB_BUILDING_RESOURCE_COUNT * sizeof(int));
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
    for (idx = 0; idx < KB_BUILDING_RESOURCE_COUNT; idx++)
        cost[idx] = 0;
    cost[RES_GOLD] = gMonsterDatabase[monster].cost;
    switch (monster) {
    case KB_MONSTER_NEEDS_GEMS_EXPANSION:
        cost[RES_GEMS] = 1;
        break;
    case KB_MONSTER_NEEDS_MERCURY:
        cost[RES_MERCURY] = 1;
        break;
    case KB_MONSTER_NEEDS_CRYSTAL:
        cost[RES_CRYSTAL] = 1;
        break;
    case KB_MONSTER_NEEDS_SULFUR_FIRST:
    case KB_MONSTER_NEEDS_SULFUR_SECOND:
        cost[RES_SULFUR] = 1;
        break;
    case KB_MONSTER_NEEDS_TWO_SULFUR:
        cost[RES_SULFUR] = 2;
        break;
    case KB_MONSTER_NEEDS_GEMS:
        cost[RES_GEMS] = 1;
        break;
    case KB_MONSTER_NEEDS_TWO_GEMS:
        cost[RES_GEMS] = 2;
        break;
    }
}

VA(0x00499a6c, 0x2b5)
int CanBuild(town *t, int building)
{
    int reqMask;
    int haveMask;
    if (BitTest(gpGame->m_knownTowns, t->m_id))
        return 0;
    if (building != KB_BUILDING_CASTLE && !(t->m_buildings & TOWN_BUILDING_CASTLE))
        return 0;
    if (!xIsExpansionMap && building == KB_BUILDING_NECROMANCER_SHRINE &&
        t->m_type == TOWN_TYPE_NECROMANCER)
        return 0;
    if (building == KB_BUILDING_DOCK) {
        if (t->CanBuildDock())
            return 1;
        else
            return 0;
    }
    if (building == KB_BUILDING_MAGE_GUILD && t->m_buildState >= KB_MAGE_GUILD_MAX_LEVEL)
        return 0;
    if (building == KB_BUILDING_UPGRADE_CASTLE || building == KB_BUILDING_DISABLED_FIRST ||
        building == KB_BUILDING_DISABLED_SECOND || building == KB_BUILDING_DISABLED_THIRD ||
        building == KB_BUILDING_DISABLED_FOURTH || building == KB_BUILDING_DISABLED_LAST)
        return 0;
    if (building < KB_BUILDING_DWELLING_FIRST || building > KB_BUILDING_DWELLING_LAST)
        return 1;
    if ((building == KB_BUILDING_DWELLING_SECOND &&
         (t->m_buildings & KB_DWELLING_UPGRADE_FIRST_FLAG)) ||
        (building == KB_BUILDING_DWELLING_THIRD &&
         (t->m_buildings & KB_DWELLING_UPGRADE_SECOND_FLAG)) ||
        (building == KB_BUILDING_DWELLING_FOURTH &&
         (t->m_buildings & KB_DWELLING_UPGRADE_THIRD_FLAG)) ||
        (building == KB_BUILDING_DWELLING_FIFTH &&
         (t->m_buildings & KB_DWELLING_UPGRADE_FOURTH_FLAG)) ||
        (building == KB_BUILDING_DWELLING_SIXTH &&
         ((t->m_buildings & KB_DWELLING_UPGRADE_FIFTH_FLAG) ||
          (t->m_buildings & KB_DWELLING_UPGRADE_SIXTH_FLAG))) ||
        (building == KB_BUILDING_UPGRADE_LAST &&
         (t->m_buildings & KB_DWELLING_UPGRADE_SIXTH_FLAG)))
        return 0;
    reqMask = gHierarchyMask[t->m_type][building - KB_BUILDING_DWELLING_FIRST];
    haveMask = t->m_buildings;
    if (haveMask & KB_DWELLING_UPGRADE_FIRST_FLAG)
        haveMask |= KB_DWELLING_FIRST_FLAG;
    if (haveMask & KB_DWELLING_UPGRADE_SECOND_FLAG)
        haveMask |= KB_DWELLING_SECOND_FLAG;
    if (haveMask & KB_DWELLING_UPGRADE_THIRD_FLAG)
        haveMask |= KB_DWELLING_THIRD_FLAG;
    if (haveMask & KB_DWELLING_UPGRADE_FOURTH_FLAG)
        haveMask |= KB_DWELLING_FOURTH_FLAG;
    if (haveMask & KB_DWELLING_UPGRADE_SIXTH_FLAG)
        haveMask |= KB_DWELLING_UPGRADE_FIFTH_FLAG;
    if (haveMask & KB_DWELLING_UPGRADE_FIFTH_FLAG)
        haveMask |= KB_DWELLING_FIFTH_FLAG;
    if ((reqMask & haveMask) == reqMask) {
        if (t->m_type == TOWN_TYPE_NECROMANCER &&
            building == KB_BUILDING_NECROMANCER_MAGE_PREREQUISITE && t->m_buildState <= 1)
            return 0;
        return 1;
    }
    return 0;
}

VA(0x00499d21, 0x9a)
int CanBuy(town *t, int type)
{
    int buf[KB_BUILDING_RESOURCE_COUNT];
    playerData *ptr;
    int idx;
    GetBuildingCost(t->m_type, type, buf, t->m_buildState);
    ptr = &gpGame->m_players[giCurPlayer];
    for (idx = 0; idx < KB_BUILDING_RESOURCE_COUNT; idx++)
        if (ptr->m_resources[idx] < buf[idx])
            return 0;
    return 1;
}

VA(0x00499dbb, 0xc6)
int GetBuildingBaseResourceValue(int race, int building, int level)
{
    if (race == TOWN_TYPE_NECROMANCER && building == KB_BUILDING_UPGRADE_CASTLE)
        return 1000;
    if (building < KB_BUILDING_DWELLING_FIRST || building > KB_BUILDING_DWELLING_LAST) {
        if (building > KB_BUILDING_NEUTRAL_LAST)
            return 0;
        else if (building == KB_BUILDING_MAGE_GUILD)
            return gMageBaseResourceValues[level];
        else if (building == KB_BUILDING_SPECIAL)
            return gSpecialBuildingBaseResourceValues[race];
        else
            return gNeutralBaseResourceValues[building];
    } else {
        return gDwellingBaseResourceValues[race][building - KB_BUILDING_DWELLING_FIRST];
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
    if (msg.type == EVENT_WINDOW_MESSAGE) {
        switch (msg.payload.widget.command) {
        case EVENT_WINDOW_CLICK_COMMAND:
            switch (msg.payload.widget.id) {
            case EVENT_WINDOW_FIRST_BUTTON:
            case EVENT_WINDOW_SECOND_BUTTON:
            case EVENT_WINDOW_THIRD_BUTTON:
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
        gpWindowManager->m_dialogResult = EVENT_WINDOW_SECOND_BUTTON;
        msg.type = EVENT_WINDOW_MESSAGE;
        msg.payload.widget.id = EVENT_WINDOW_CLOSE_COMMAND;
        msg.payload.widget.command = msg.payload.widget.id;
        return 2;
    }
    return 1;
}

// @early-stop
// 99.8%: explicit-range comparison excluding jump-table data at function offsets
// [0x35d,0x3a3) and [0x41c,0x431) finds all 283 instructions identical after
// normalizing only branch destinations and relocation identities. External relocations
// agree in order; residual names are local tables, string literals, and the equivalent
// cLuckInfo[2]/cMoraleInfo[2] array-element aliases used by the retail delinker.
VA(0x0049a09f, 0x472)
int EventWindowHandler(struct tag_message &msg)
{
    int type;
    int extra;

    if (!gpSoundManager->MusicPlaying() && gpAdvManager->m_active == 1)
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[gpAdvManager->m_currentTerrain]);
    if (giDialogTimeout != 0 && KBTickCount() > giDialogTimeout) {
        msg.type = EVENT_WINDOW_MESSAGE;
        gpWindowManager->m_dialogResult = msg.payload.widget.id;
        msg.payload.widget.id = EVENT_WINDOW_CLOSE_COMMAND;
        msg.payload.widget.command = msg.payload.widget.id;
        giDialogTimeout = 0;
        return EVENT_WINDOW_CLOSE;
    }
    if (msg.type == EVENT_WINDOW_MESSAGE) {
        switch (msg.payload.widget.command) {
        case EVENT_WINDOW_HOVER_COMMAND:
        case EVENT_WINDOW_HELP_COMMAND:
            type = NORMAL_DIALOG_NO_RESOURCE;
            extra = NORMAL_DIALOG_NO_VALUE;
            if (msg.payload.widget.parameter & EVENT_WINDOW_RESOURCE_FLAG) {
                switch (msg.payload.widget.id) {
                case EVENT_WINDOW_FIRST_RESOURCE_WIDGET:
                    type = giResType1;
                    extra = giResExtra1;
                    break;
                case EVENT_WINDOW_SECOND_RESOURCE_WIDGET:
                    type = giResType2;
                    extra = giResExtra2;
                    break;
                }
                switch (type) {
            case EVENT_WINDOW_LUCK:
                NormalDialog(cLuckInfo[LUCK_INFO_GOOD], NORMAL_DIALOG_QUICK_VIEW,
                             -1, -1, -1, 0, -1, 0, -1, 0);
                break;
            case EVENT_WINDOW_BAD_LUCK:
                NormalDialog(cLuckInfo[LUCK_INFO_BAD], NORMAL_DIALOG_QUICK_VIEW,
                             -1, -1, -1, 0, -1, 0, -1, 0);
                break;
            case EVENT_WINDOW_MORALE:
                NormalDialog(cMoraleInfo[MORALE_INFO_GOOD], NORMAL_DIALOG_QUICK_VIEW,
                             -1, -1, -1, 0, -1, 0, -1, 0);
                break;
            case EVENT_WINDOW_BAD_MORALE:
                NormalDialog(cMoraleInfo[MORALE_INFO_BAD], NORMAL_DIALOG_QUICK_VIEW,
                             -1, -1, -1, 0, -1, 0, -1, 0);
                break;
            case EVENT_WINDOW_EXPERIENCE:
                NormalDialog("{Experience}\n\nExperience allows your heroes to go up levels, increasing their primary and secondary skills.",
                             NORMAL_DIALOG_QUICK_VIEW, -1, -1, -1, 0, -1, 0, -1, 0);
                break;
            case NORMAL_DIALOG_ARTIFACT:
                if (extra == EVENT_WINDOW_SPELL_SCROLL) {
                    sprintf(gText, gArtifactDesc[extra], gSpellNames[xTheSpell]);
                    NormalDialog(gText, NORMAL_DIALOG_QUICK_VIEW,
                                 -1, -1, -1, 0, -1, 0, -1, 0);
                } else {
                    NormalDialog(gArtifactDesc[extra], NORMAL_DIALOG_QUICK_VIEW,
                                 -1, -1, -1, 0, -1, 0, -1, 0);
                }
                break;
            case NORMAL_DIALOG_SPELL:
                NormalDialog(gSpellDesc[extra], NORMAL_DIALOG_QUICK_VIEW,
                             -1, -1, -1, 0, -1, 0, -1, 0);
                break;
            case NORMAL_DIALOG_SECONDARY_SKILL:
                NormalDialog(cSecSkillDesc[extra / 3][extra % 3], NORMAL_DIALOG_QUICK_VIEW,
                             -1, -1, -1, 0, -1, 0, -1, 0);
                break;
            case NORMAL_DIALOG_PRIMARY_SKILL:
                NormalDialog(gStatDesc[extra], NORMAL_DIALOG_QUICK_VIEW,
                             -1, -1, -1, 0, -1, 0, -1, 0);
                break;
            case RES_WOOD:
            case RES_MERCURY:
            case RES_ORE:
            case RES_SULFUR:
            case RES_CRYSTAL:
            case RES_GEMS:
            case RES_GOLD:
                NormalDialog("{Resources}\n\nThere are seven resources in Heroes 2, used to build and improves castles, purchase troops and recruit heroes.  Gold is the most common, required for virtually everything.  Wood and ore are used for most buildings.  Gems, Mercury, Sulfur and Crystal are rare magical resources used for the most powerful creatures and buildings.",
                             NORMAL_DIALOG_QUICK_VIEW, -1, -1, -1, 0, -1, 0, -1, 0);
                break;
                }
            }
            break;
        case EVENT_WINDOW_CLICK_COMMAND:
            switch (msg.payload.widget.id) {
            case EVENT_WINDOW_FIRST_BUTTON:
            case EVENT_WINDOW_SECOND_BUTTON:
            case EVENT_WINDOW_THIRD_BUTTON:
            case EVENT_WINDOW_FOURTH_BUTTON:
            case EVENT_WINDOW_FIFTH_BUTTON:
            case EVENT_WINDOW_SIXTH_BUTTON:
            case EVENT_WINDOW_SEVENTH_BUTTON:
            case EVENT_WINDOW_EIGHTH_BUTTON:
                gpWindowManager->m_dialogResult = msg.payload.widget.id;
                msg.payload.widget.id = EVENT_WINDOW_CLOSE_COMMAND;
                msg.payload.widget.command = msg.payload.widget.id;
                giDialogTimeout = 0;
                return EVENT_WINDOW_CLOSE;
            case EVENT_WINDOW_IGNORED_BUTTON:
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
    return EVENT_WINDOW_CONTINUE;
}

VA(0x0049a511, 0x1e)
int TrueFalseDialogHandler(struct tag_message &msg)
{
    return EventWindowHandler(msg);
}

VA(0x0049a52f, 0x192)
void PlayerDead(int player)
{
    playerData *rec;
    int i;
    gbRetreatWin = 0;
    rec = &gpGame->m_players[player];
    gpGame->m_playerDead[player] = 1;
    ++gpGame->m_deadPlayerCount;
    for (i = 0; i < GAME_MINE_COUNT; i++) {
        if (gpGame->m_mineOwners[i] == player)
            gpGame->ClaimMine(i, -1);
    }
    for (i = rec->m_heroCount - 1; i >= 0; i--) {
        GetHeroSlot(rec->m_heroIds[i])->Deallocate(1);
    }
    for (i = 0; i < AVAILABLE_HERO_SLOTS; i++) {
        if (gpGame->m_availableHeroes[rec->m_availableHeroIds[i]] == WEEKLY_AVAILABLE_HERO)
            gpGame->m_availableHeroes[rec->m_availableHeroIds[i]] = -1;
    }
    if (gbRemoteOn) {
        if (gbHumanPlayer[player])
            HandleRemoteDeadPlayerExit(player);
        else
            SendMapChange(PLAYER_DEAD_MAP_CHANGE, static_cast<signed char>(player), 0, 0,
                          PLAYER_DEAD_MAP_CHANGE_UNUSED, 0, 0);
    }
}

// @match-note
// Pre-95 structural checkpoint (99.57%): exact 0x1c4 frame, slots, CFG, and
// 300/300 relocation sites with no base-only target. The first code residual is
// the first player/giThisGamePos equality: retail loads player then compares the
// global, while this TU loads the global then compares player. The next two
// player/giCurPlayer tests have the same commutative load-order residual; the
// remaining diff rows are literal/interior-symbol identities. Both equality
// operand orders compile identically. Revisit at 95% for last-mile steering.
VA(0x0049a6c1, 0x19bb)
void CheckEndGame(int forcedResult, int dragonCityCaptured)
{
    char saveName[20];
    unsigned int carryoverHeroId;
    int carryoverHeroIndex;
    int sideBelow;
    int sideAbove;
    char unusedTextA[CHECK_END_GAME_TEXT_BUFFER_SIZE];
    char unusedTextB[CHECK_END_GAME_TEXT_BUFFER_SIZE];
    char artifactName[CHECK_END_GAME_TEXT_BUFFER_SIZE];
    hero *victoryHero;
    hero *lossHero;
    hero *artifactHero;
    int bestGold;
    int winner;
    int hasDwarfTown;
    int hasRoland;
    int enemyRemains;
    int artifactWinner;
    int currentDay;
    int savedRemoteOn;
    int dialogShown;
    int aliveCount;
    int lastAlive;
    int aliveHumanCount;
    int lastHuman;
    int aliveThisNetHumanCount;
    int victory;
    int defeat;
    int normalVictoryAllowed;
    int player;
    int heroIndex;
    playerData *rec;
    town *victoryTown;
    town *lossTown;

    if (!gbThisNetGotAdventureControl)
        return;
    if (gbInNewGameSetup)
        return;
    if (gbGameOver)
        return;
    if (bInCheckEndGame)
        return;

    bInCheckEndGame = 1;
    savedRemoteOn = gbRemoteOn;
    dialogShown = 0;

        for (player = 0; player < gpGame->m_playerCount; player++) {
            if (!gpGame->m_playerDead[player]) {
                rec = &gpGame->m_players[player];
                if ((rec->m_heroCount == 0 && rec->m_townCount == 0) ||
                    (xIsPlayingExpansionCampaign && xCampaign.IsSpecialLossCondition(player))) {
                    PlayerDead(player);
                    if (giThisGamePos == player) {
                        dialogShown = 1;
                        sprintf(gText, "You have been eliminated from the game!!!");
                        NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                    } else {
                        sprintf(gText, "%s has been vanquished!", cPlayerNames[player]);
                        NormalDialog(gText, 1, -1, -1, CHECK_END_GAME_PLAYER_DIALOG_ICON,
                                     gpGame->GetPlayerColor(static_cast<signed char>(player)), -1, -1, -1,
                                     CHECK_END_GAME_REMOTE_DIALOG_TIME);
                    }
                } else if (rec->m_townCount == 0) {
                    if (rec->m_daysLeft == -1) {
                        if (gbThisNetHumanPlayer[player] && giCurPlayer == player) {
                            sprintf(gText,
                                    "%s, you have lost your last town.  If you do not conquer another "
                                    "town in the next week, you will be eliminated.",
                                    cPlayerNames[player]);
                            NormalDialog(gText, 1, -1, -1, CHECK_END_GAME_PLAYER_DIALOG_ICON,
                                         gpGame->GetPlayerColor(static_cast<signed char>(player)), -1, 0, -1, 0);
                        }
                        rec->m_daysLeft = CHECK_END_GAME_GRACE_DAYS;
                    } else if (rec->m_daysLeft == 0) {
                        PlayerDead(player);
                        if (gbThisNetHumanPlayer[player] && giCurPlayer == player) {
                            if (!dialogShown) {
                                dialogShown = 1;
                                sprintf(gText,
                                        "%s, your heroes abandon you, and you are banished from this "
                                        "land.",
                                        cPlayerNames[player]);
                            }
                        } else {
                            sprintf(gText,
                                    "%s's heroes have abandoned him, and he is banished from this "
                                    "land.",
                                    cPlayerNames[player]);
                        }
                        NormalDialog(gText, 1, -1, -1, CHECK_END_GAME_PLAYER_DIALOG_ICON,
                                     gpGame->GetPlayerColor(static_cast<signed char>(player)), -1, 0, -1, 0);
                    }
                } else {
                    rec->m_daysLeft = -1;
                }
            }
        }

        aliveCount = 0;
        lastAlive = 0;
        aliveHumanCount = 0;
        lastHuman = 0;
        aliveThisNetHumanCount = 0;
        for (player = 0; player < gpGame->m_playerCount; player++) {
            if (!gpGame->m_playerDead[player]) {
                aliveCount++;
                lastAlive = player;
                if (gbThisNetHumanPlayer[player]) {
                    aliveThisNetHumanCount++;
                }
                if (gbHumanPlayer[player]) {
                    aliveHumanCount++;
                    lastHuman = player;
                }
            }
        }

        victory = 0;
        defeat = 0;
        normalVictoryAllowed = 1;
        if ((gpGame->m_mapHeader.victoryCondition != CHECK_END_GAME_VICTORY_STANDARD && !gpGame->m_mapHeader.allowNormalVictory) ||
            (gbInCampaign && gpGame->m_campaignType == CHECK_END_GAME_ARCHIBALD_CAMPAIGN &&
             gpGame->m_campaignScenario + CHECK_END_GAME_SCENARIO_OFFSET == CHECK_END_GAME_SIDE_SCENARIO)) {
            normalVictoryAllowed = 0;
        }

        if (gpGame->m_mapHeader.victoryCondition == CHECK_END_GAME_VICTORY_SIDE &&
            gpGame->m_mapHeader.victoryConditionValue != CHECK_END_GAME_SIDE_SPECIAL_VALUE &&
            (!gbInCampaign || gpGame->m_campaignType != CHECK_END_GAME_ARCHIBALD_CAMPAIGN ||
             gpGame->m_campaignScenario + CHECK_END_GAME_SCENARIO_OFFSET != CHECK_END_GAME_SIDE_SCENARIO)) {
            sideBelow = 0;
            sideAbove = 0;
            for (player = 0; player < gpGame->m_playerCount; player++) {
                if (!gpGame->m_playerDead[player]) {
                    if (gpGame->m_players[player].m_color < gpGame->m_mapHeader.victorySideThreshold) {
                        sideBelow++;
                    } else {
                        sideAbove++;
                    }
                }
            }
            if (sideBelow == 0) {
                for (player = 0; player < gpGame->m_playerCount; player++) {
                    if (gbThisNetHumanPlayer[player] && !gpGame->m_playerDead[player] &&
                        gpGame->m_players[player].m_color >= gpGame->m_mapHeader.victorySideThreshold) {
                        victory = 1;
                    }
                }
            } else if (sideAbove == 0) {
                for (player = 0; player < gpGame->m_playerCount; player++) {
                    if (gbThisNetHumanPlayer[player] && !gpGame->m_playerDead[player] &&
                        gpGame->m_players[player].m_color < gpGame->m_mapHeader.victorySideThreshold) {
                        victory = 1;
                    }
                }
            }
            if (sideBelow != 0 && sideAbove == 0 && !victory) {
                defeat = 1;
            }
            if (!dialogShown && victory) {
                dialogShown = 1;
                sprintf(gText, "The enemy is beaten.  Your side has triumphed!");
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }

        if (gpGame->m_mapHeader.victoryCondition == CHECK_END_GAME_VICTORY_CAPTURE_TOWN) {
            victoryTown = gpGame->GetTown(
                gpGame->GetTownId(gpGame->m_mapHeader.victoryConditionValue, gpGame->m_mapHeader.victoryTownY));
            if (victoryTown->m_owner != CHECK_END_GAME_NO_PLAYER &&
                (gbHumanPlayer[victoryTown->m_owner] || gpGame->m_mapHeader.computerAlsoWins)) {
                if (gbThisNetHumanPlayer[victoryTown->m_owner]) {
                    victory = 1;
                } else {
                    defeat = 1;
                }
                if (!dialogShown) {
                    dialogShown = 1;
                    if (victory) {
                        sprintf(gText, "You captured %s!  You are victorious.", victoryTown->m_name);
                    } else {
                        sprintf(gText, "The enemy has captured %s!  They are triumphant.", victoryTown->m_name);
                    }
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }

        if (gpGame->m_mapHeader.lossCondition == CHECK_END_GAME_LOSS_TOWN) {
            lossTown =
                gpGame->GetTown(gpGame->GetTownId(gpGame->m_mapHeader.lossConditionValue, gpGame->m_mapHeader.lossTownY));
            if (lossTown->m_owner == CHECK_END_GAME_NO_PLAYER || !gbHumanPlayer[lossTown->m_owner]) {
                defeat = 1;
                if (!dialogShown) {
                    dialogShown = 1;
                    sprintf(gText, "%s has fallen!  All is lost.", lossTown->m_name);
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }

        if (gpGame->m_mapHeader.victoryCondition == CHECK_END_GAME_VICTORY_GOLD) {
            bestGold = 0;
            winner = CHECK_END_GAME_NO_PLAYER;
            for (player = 0; player < gpGame->m_playerCount; player++) {
                if ((gbHumanPlayer[player] || gpGame->m_mapHeader.computerAlsoWins) &&
                    gpGame->m_players[player].m_resources[CHECK_END_GAME_GOLD_RESOURCE] >=
                        gpGame->m_mapHeader.victoryConditionValue * CHECK_END_GAME_GOLD_SCALE &&
                    gpGame->m_players[player].m_resources[CHECK_END_GAME_GOLD_RESOURCE] >= bestGold) {
                    bestGold = gpGame->m_players[player].m_resources[CHECK_END_GAME_GOLD_RESOURCE];
                    winner = player;
                }
                if (winner != CHECK_END_GAME_NO_PLAYER) {
                    if (gbThisNetHumanPlayer[winner]) {
                        victory = 1;
                    } else {
                        defeat = 1;
                    }
                    if (!dialogShown) {
                        dialogShown = 1;
                        if (victory) {
                            sprintf(gText,
                                    "You have built up over %d gold in your treasury.  All enemies bow "
                                    "before your wealth and power.",
                                    bestGold);
                        } else {
                            sprintf(gText,
                                    "The enemy has built up over %d gold in his treasury.  You must "
                                    "bow done in defeat before his wealth and power.",
                                    bestGold);
                        }
                        NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                    }
                }
            }
        }

        if (gpGame->m_mapHeader.victoryCondition == CHECK_END_GAME_VICTORY_DEFEAT_HERO) {
            victoryHero = GetHeroSlot(gpGame->m_mapHeader.victoryConditionValue);
            if (victoryHero->m_owner < 0 || victoryHero->m_owner >= CHECK_END_GAME_PLAYER_COUNT ||
                gbHumanPlayer[victoryHero->m_owner]) {
                victory = 1;
                if (!dialogShown) {
                    dialogShown = 1;
                    sprintf(gText, "You have captured the enemy hero %s!  Your quest is complete.",
                            victoryHero->m_name);
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }

        if (gpGame->m_mapHeader.lossCondition == CHECK_END_GAME_LOSS_HERO) {
            lossHero = GetHeroSlot(gpGame->m_mapHeader.lossConditionValue);
            if (lossHero->m_owner < 0 || lossHero->m_owner >= CHECK_END_GAME_PLAYER_COUNT ||
                !gbHumanPlayer[lossHero->m_owner]) {
                defeat = 1;
                if (!dialogShown) {
                    dialogShown = 1;
                    sprintf(gText, "You have lost the hero %s.  Your quest is over.", lossHero->m_name);
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }

        if (gpGame->m_mapHeader.lossCondition == CHECK_END_GAME_LOSS_TIME) {
            if (gpGame->m_mapHeader.lossConditionValue < (gpGame->m_week - 1) * CHECK_END_GAME_DAYS_PER_WEEK +
                                                   (gpGame->m_month - 1) * CHECK_END_GAME_DAYS_PER_MONTH +
                                                   gpGame->m_day) {
                defeat = 1;
                if (!dialogShown) {
                    dialogShown = 1;
                    sprintf(gText, "You have failed to complete your quest in time.  All is lost.");
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }

        if (gpGame->m_mapHeader.victoryCondition == CHECK_END_GAME_VICTORY_ARTIFACT) {
            artifactWinner = CHECK_END_GAME_NO_PLAYER;
            for (player = 0; player < gpGame->m_playerCount; player++) {
                if (!gpGame->m_playerDead[player]) {
                    for (heroIndex = 0; heroIndex < gpGame->m_players[player].m_heroCount; heroIndex++) {
                        artifactHero = gpGame->GetPlayerHero(player, heroIndex);
                        if (gpGame->m_mapHeader.victoryConditionValue > CHECK_END_GAME_ULTIMATE_ARTIFACT) {
                            if (artifactHero->HasArtifact(gpGame->m_mapHeader.victoryConditionValue - 1)) {
                                artifactWinner = player;
                            }
                        } else {
                            if (artifactHero->HasArtifact(0) || artifactHero->HasArtifact(1) ||
                                artifactHero->HasArtifact(2) || artifactHero->HasArtifact(3) ||
                                artifactHero->HasArtifact(4) || artifactHero->HasArtifact(5) ||
                                artifactHero->HasArtifact(6) || artifactHero->HasArtifact(7)) {
                                artifactWinner = player;
                            }
                        }
                    }
                }
            }
            if (artifactWinner != CHECK_END_GAME_NO_PLAYER) {
                if (gbThisNetHumanPlayer[artifactWinner]) {
                    victory = 1;
                } else {
                    defeat = 1;
                }
                if (!dialogShown) {
                    dialogShown = 1;
                    if (gpGame->m_mapHeader.victoryConditionValue == CHECK_END_GAME_ULTIMATE_ARTIFACT) {
                        sprintf(artifactName, "Ultimate Artifact");
                    } else {
                        sprintf(artifactName, gArtifactNames[gpGame->m_mapHeader.victoryConditionValue - 1]);
                    }
                    if (victory) {
                        sprintf(gText, "You have found the %s.  Your quest is complete.", artifactName);
                    } else {
                        sprintf(gText, "The enemy has found the %s.  Your quest is a failure.", artifactName);
                    }
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }

        if (gbInCampaign && gpGame->m_campaignType == CHECK_END_GAME_ROLAND_CAMPAIGN &&
            gpGame->m_campaignScenario + CHECK_END_GAME_SCENARIO_OFFSET == CHECK_END_GAME_DWARF_SCENARIO) {
            hasDwarfTown = 0;
            for (player = 0; player < gpGame->m_players[0].m_townCount; player++) {
                if (gpGame->GetTown(gpGame->m_players[0].m_townIds[player])->m_type == CHECK_END_GAME_DWARF_TOWN) {
                    hasDwarfTown = 1;
                }
            }
            if (!hasDwarfTown) {
                defeat = 1;
                if (!dialogShown) {
                    dialogShown = 1;
                    sprintf(gText, "All the dwarf towns have fallen.  This is a disastrous defeat!  You have "
                                   "lost.");
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }

        if (gbInCampaign && gpGame->m_campaignType == CHECK_END_GAME_ARCHIBALD_CAMPAIGN &&
            gpGame->m_campaignScenario + CHECK_END_GAME_SCENARIO_OFFSET == CHECK_END_GAME_SIDE_SCENARIO &&
            dragonCityCaptured) {
            victory = 1;
            if (!dialogShown) {
                dialogShown = 1;
                sprintf(gText, "Dragon city has fallen!  You are now the Master of the Dragons.");
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }

        if (gbInCampaign && gpGame->m_campaignType == CHECK_END_GAME_ROLAND_CAMPAIGN &&
            gpGame->m_campaignScenario + CHECK_END_GAME_SCENARIO_OFFSET == CHECK_END_GAME_ROLAND_CAPTURE_SCENARIO) {
            hasRoland = 0;
            for (heroIndex = 0; heroIndex < CHECK_END_GAME_HERO_COUNT; heroIndex++) {
                if (gpGame->m_heroRecs[heroIndex].m_portrait == CHECK_END_GAME_ROLAND_HERO &&
                    gpGame->m_heroRecs[heroIndex].m_owner >= 0 &&
                    gpGame->m_heroRecs[heroIndex].m_owner <= CHECK_END_GAME_PLAYER_COUNT - 1) {
                    hasRoland = 1;
                }
            }
            if (!hasRoland) {
                defeat = 1;
                if (!dialogShown) {
                    dialogShown = 1;
                    sprintf(gText, "Roland has been captured!  All is lost.");
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }

        if (gbInCampaign && gpGame->m_campaignType == CHECK_END_GAME_ROLAND_CAMPAIGN &&
            gpGame->m_campaignScenario + CHECK_END_GAME_SCENARIO_OFFSET == CHECK_END_GAME_ROLAND_CAPTURE_SCENARIO) {
            enemyRemains = 0;
            for (player = 0; player < gpGame->m_playerCount; player++) {
                if (!gpGame->m_playerDead[player] && gpGame->m_players[player].m_color != CHECK_END_GAME_ROLAND_COLOR &&
                    gpGame->m_players[player].m_color != CHECK_END_GAME_ALLY_COLOR) {
                    enemyRemains = 1;
                }
            }
            if (!enemyRemains) {
                victory = 1;
                if (!dialogShown && victory) {
                    dialogShown = 1;
                    sprintf(gText, "The enemy is beaten.  Your side has triumphed!");
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }

        if (defeat) {
            gbGameOver = 1;
            giEndSequence = 0;
        }
        if (victory) {
            gbGameOver = 1;
            giEndSequence = 1;
        }

        if (aliveCount == 1 || aliveHumanCount == 0 || (aliveHumanCount == 1 && !gbThisNetHumanPlayer[lastHuman])) {
            if (aliveHumanCount == 1 && gbThisNetHumanPlayer[lastHuman]) {
                if (normalVictoryAllowed) {
                    gbGameOver = 1;
                    giEndSequence = 1;
                }
            } else {
                gbGameOver = 1;
                giEndSequence = 0;
            }
        }

        if (savedRemoteOn && aliveThisNetHumanCount == 0) {
            gbGameOver = 1;
            giEndSequence = 0;
        }
        if (forcedResult == CHECK_END_GAME_FORCE_VICTORY) {
            victory = 1;
            gbGameOver = 1;
            giEndSequence = 1;
        }
        if (forcedResult == CHECK_END_GAME_FORCE_DEFEAT) {
            defeat = 1;
            gbGameOver = 1;
            giEndSequence = 0;
        }

        if (giEndSequence == 1 && gbGameOver) {
            victory = 1;
        }
        if (giEndSequence == 0 && gbGameOver) {
            defeat = 1;
        }

        if (gbInCampaign && victory) {
            currentDay = (gpGame->m_week - 1) * CHECK_END_GAME_DAYS_PER_WEEK +
                         (gpGame->m_month - 1) * CHECK_END_GAME_DAYS_PER_MONTH + gpGame->m_day;
            gpGame->m_campaignScenarioWon = 1;
            gpGame->m_campaignScenarioCompleted[gpGame->m_campaignType][gpGame->m_campaignScenario] = 1;
            gpGame->m_campaignScenarioDays[gpGame->m_campaignType][gpGame->m_campaignScenario] = currentDay;
            gpGame->m_campaignScore =
                gpGame->m_campaignScenarioDays[gpGame->m_campaignType][gpGame->m_campaignScenario] +
                gpGame->m_campaignScenarioBonus[gpGame->m_campaignType][gpGame->m_campaignScenario];

            carryoverHeroId = CHECK_END_GAME_NO_PLAYER;
            if (gpGame->m_campaignType == CHECK_END_GAME_ROLAND_CAMPAIGN &&
                gpGame->m_campaignScenario + CHECK_END_GAME_SCENARIO_OFFSET == CHECK_END_GAME_SIDE_SCENARIO) {
                carryoverHeroId = CHECK_END_GAME_SIDE_SPECIAL_VALUE;
            }
            if (gpGame->m_campaignType == CHECK_END_GAME_ARCHIBALD_CAMPAIGN &&
                gpGame->m_campaignScenario + CHECK_END_GAME_SCENARIO_OFFSET == CHECK_END_GAME_FIRST_NO_SAVE_SCENARIO) {
                carryoverHeroId = CHECK_END_GAME_SIDE_SPECIAL_VALUE;
            }

            if (carryoverHeroId != CHECK_END_GAME_NO_PLAYER) {
                for (player = 0; player < CHECK_END_GAME_ARMY_SLOTS; player++) {
                    gpGame->m_campaignCarryoverCreatureTypes[player] = CHECK_END_GAME_EMPTY_ARMY;
                    gpGame->m_campaignCarryoverCreatureCounts[player] = 0;
                }
                for (carryoverHeroIndex = 0; carryoverHeroIndex < gpGame->m_players[0].m_heroCount;
                     carryoverHeroIndex++) {
                    if (carryoverHeroId != CHECK_END_GAME_SIDE_SPECIAL_VALUE &&
                        gpGame->m_heroRecs[gpGame->m_players[0].m_heroIds[carryoverHeroIndex]].m_portrait ==
                            carryoverHeroId) {
                        break;
                    }
                }
                if (gpGame->m_players[0].m_heroCount == carryoverHeroIndex) {
                    gpGame->m_campaignCarryoverCreatureTypes[0] = 0;
                    gpGame->m_campaignCarryoverCreatureCounts[0] = 1;
                } else {
                    for (player = 0; player < CHECK_END_GAME_ARMY_SLOTS; player++) {
                        gpGame->m_campaignCarryoverCreatureTypes[player] =
                            gpGame->m_heroRecs[gpGame->m_players[0].m_heroIds[carryoverHeroIndex]]
                                .m_army.m_creatureTypes[player];
                        gpGame->m_campaignCarryoverCreatureCounts[player] =
                            gpGame->m_heroRecs[gpGame->m_players[0].m_heroIds[carryoverHeroIndex]]
                                .m_army.m_creatureCounts[player];
                    }
                }
            }

            if (gpGame->m_campaignScenario + CHECK_END_GAME_SCENARIO_OFFSET != CHECK_END_GAME_LAST_SCENARIO &&
                (gpGame->m_campaignScenario + CHECK_END_GAME_SCENARIO_OFFSET != CHECK_END_GAME_FIRST_NO_SAVE_SCENARIO ||
                 gpGame->m_campaignType != CHECK_END_GAME_ROLAND_CAMPAIGN)) {
                sprintf(saveName, "%s%c_%02d", "WIN_",
                        gpGame->m_campaignType == CHECK_END_GAME_ROLAND_CAMPAIGN ? 'G' : 'E',
                        gpGame->m_campaignScenario + 1);
                gpGame->SaveGame(saveName, 1, 0);
            }
        } else if (xIsPlayingExpansionCampaign && victory) {
            xCampaign.Autosave();
        }

    bInCheckEndGame = 0;
}

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
    NULL_SAMPLE2.pMem = reinterpret_cast<struct _SAMPLE *>(NULL_SAMPLE2.pSample);
    gGameCommand = -1;
    gPalette = 0;
    gbCombatSurrender = 0;
    gpGame->m_viewArmyResult = 0;
    strcpy(gpGame->m_mapFilename, "brokena.mp2");
    gpGame->m_mapFilename[sizeof(gpGame->m_mapFilename) - 1] = 0;
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

// @match-note
// Pre-95 structural checkpoint (99.399%): exact 0xe0 frame and 119/119
// relocation sites with no base-only target. The first real code residual is a
// retail continuation jump after the mixed-undead strcat; other early rows are
// string/interior-symbol aliases. The direct nested undead test is represented;
// revisit at 95% for systematic continuation and condition-shape steering.
VA(0x0049c312, 0x61b)
void game::ShowMoraleInfo(hero *h, int dialogType)
{
    int hasMixedUndead;
    int alignment;
    int homogeneous;
    int modifierStart;
    char description[200];
    int slot;

    hasMixedUndead = 0;
    if (h->m_army.GetMorale(h, h->GetOccupiedTown(), 0) > 0)
        sprintf(description, cMoraleInfo[MORALE_INFO_GOOD]);
    else {
        if (h->m_army.GetMorale(h, h->GetOccupiedTown(), 0) == 0)
            sprintf(description, cMoraleInfo[MORALE_INFO_NEUTRAL]);
        else
            sprintf(description, cMoraleInfo[MORALE_INFO_BAD]);
    }

    sprintf(gText, cMoraleInfo[MORALE_INFO_HEADER], description);
    modifierStart = strlen(gText);
    if (h->m_army.HasAllUndead()) {
        strcat(gText, cMoraleInfo[MORALE_INFO_ALL_UNDEAD]);
    } else {
        if (h->m_army.HasSomeUndead() || h->HasArtifact(ARTIFACT_UNDEAD_MORALE)) {
            strcat(gText, cMoraleInfo[MORALE_INFO_SOME_UNDEAD]);
            hasMixedUndead = 1;
        }

        homogeneous = h->m_army.IsHomogeneous(-1);
        if (hasMixedUndead && homogeneous > 0) {
            homogeneous = 0;
        }
        if (homogeneous > 0) {
            alignment = 0;
            for (slot = 0; slot < 5; slot++) {
                if (h->m_army.m_creatureTypes[slot] != -1) {
                    alignment = gMonsterDatabase[h->m_army.m_creatureTypes[slot]].race;
                }
            }
            sprintf(
                description,
                cMoraleInfo[MORALE_INFO_SAME_ALIGNMENT],
                gAlignmentNames[alignment]
            );
            strcat(gText, description);
        }
        if (homogeneous == -1) {
            sprintf(description, cMoraleInfo[MORALE_INFO_THREE_ALIGNMENTS]);
            strcat(gText, description);
        }
        if (homogeneous == -2) {
            sprintf(description, cMoraleInfo[MORALE_INFO_FOUR_ALIGNMENTS]);
            strcat(gText, description);
        }
        if (homogeneous == -3) {
            sprintf(description, cMoraleInfo[MORALE_INFO_FIVE_ALIGNMENTS]);
            strcat(gText, description);
        }

        if (h->GetOccupiedTown() != 0 && h->GetOccupiedTown()->m_type == TOWN_TYPE_BARBARIAN
            && (h->GetOccupiedTown()->m_buildings & TOWN_BUILDING_COLISEUM)) {
            strcat(gText, cMoraleInfo[MORALE_INFO_COLISEUM]);
        }
        if (h->GetOccupiedTown() != 0
            && (h->GetOccupiedTown()->m_buildings & TOWN_BUILDING_TAVERN)) {
            strcat(gText, cMoraleInfo[MORALE_INFO_TAVERN]);
        }

        if (h->HasArtifact(ARTIFACT_MEDAL_OF_VALOR)) {
            strcat(gText, cMoraleInfo[MORALE_INFO_MEDAL_OF_VALOR]);
        }
        if (h->HasArtifact(ARTIFACT_MEDAL_OF_COURAGE)) {
            strcat(gText, cMoraleInfo[MORALE_INFO_MEDAL_OF_COURAGE]);
        }
        if (h->HasArtifact(ARTIFACT_MEDAL_OF_HONOR)) {
            strcat(gText, cMoraleInfo[MORALE_INFO_MEDAL_OF_HONOR]);
        }
        if (h->HasArtifact(ARTIFACT_MEDAL_OF_DISTINCTION)) {
            strcat(gText, cMoraleInfo[MORALE_INFO_MEDAL_OF_DISTINCTION]);
        }
        if (h->HasArtifact(ARTIFACT_FIZBIN_OF_MISFORTUNE)) {
            strcat(gText, cMoraleInfo[MORALE_INFO_FIZBIN]);
        }
        if (h->m_eventFlags & HERO_EVENT_BUOY) {
            strcat(gText, cMoraleInfo[MORALE_INFO_BUOY]);
        }
        if (h->m_eventFlags & HERO_EVENT_OASIS) {
            strcat(gText, cMoraleInfo[MORALE_INFO_OASIS]);
        }
        if (h->m_eventFlags & HERO_EVENT_TEMPLE) {
            strcat(gText, cMoraleInfo[MORALE_INFO_TEMPLE]);
        }
        if (h->m_eventFlags & HERO_EVENT_GRAVEYARD) {
            strcat(gText, cMoraleInfo[MORALE_INFO_GRAVEYARD]);
        }
        if (h->m_eventFlags & HERO_EVENT_SHIPWRECK) {
            strcat(gText, cMoraleInfo[MORALE_INFO_SHIPWRECK]);
        }
        if (h->m_eventFlags & HERO_EVENT_WATERING_HOLE) {
            strcat(gText, cMoraleInfo[MORALE_INFO_WATERING_HOLE]);
        }
        if (h->m_eventFlags & HERO_EVENT_DERELICT_SHIP) {
            strcat(gText, cMoraleInfo[MORALE_INFO_DERELICT_SHIP]);
        }
        if (h->m_secondarySkills[HERO_SKILL_LEADERSHIP] == HERO_SKILL_LEVEL_BASIC) {
            strcat(gText, cMoraleInfo[MORALE_INFO_BASIC_LEADERSHIP]);
        }
        if (h->m_secondarySkills[HERO_SKILL_LEADERSHIP] == HERO_SKILL_LEVEL_ADVANCED) {
            strcat(gText, cMoraleInfo[MORALE_INFO_ADVANCED_LEADERSHIP]);
        }
        if (h->m_secondarySkills[HERO_SKILL_LEADERSHIP] == HERO_SKILL_LEVEL_EXPERT) {
            strcat(gText, cMoraleInfo[MORALE_INFO_EXPERT_LEADERSHIP]);
        }
        if (h->HasArtifact(ARTIFACT_MASTHEAD) && (h->m_eventFlags & HERO_EVENT_EMBARKED)) {
            strcat(gText, cMoraleInfo[MORALE_INFO_MASTHEAD]);
        }
        if (h->HasArtifact(ARTIFACT_BATTLE_GARB)) {
            strcat(gText, cMoraleInfo[MORALE_INFO_BATTLE_GARB]);
        }
        if (static_cast<int>(strlen(gText)) == modifierStart) {
            strcat(gText, cMoraleInfo[MORALE_INFO_NONE]);
        }
    }

    NormalDialog(gText, dialogType, -1, -1, -1, 0, -1, 0, -1, 0);
}

VA(0x0049c92d, 0x371)
void game::ShowLuckInfo(hero *h, int dialogType)
{
    char description[200];
    int luckValue;
    int modifierStart;

    if (gpGame->GetLuck(h, 0, h->GetOccupiedTown()) > 0)
        sprintf(description, cLuckInfo[LUCK_INFO_GOOD]);
    else {
        if (gpGame->GetLuck(h, 0, h->GetOccupiedTown()) == 0)
            sprintf(description, cLuckInfo[LUCK_INFO_NEUTRAL]);
        else
            sprintf(description, cLuckInfo[LUCK_INFO_BAD]);
    }

    sprintf(gText, cLuckInfo[LUCK_INFO_HEADER], description);
    modifierStart = strlen(gText);
    if (h->GetOccupiedTown() != 0 && h->GetOccupiedTown()->m_type == TOWN_TYPE_SORCERESS &&
        (h->GetOccupiedTown()->m_buildings & TOWN_BUILDING_RAINBOW))
        strcat(gText, cLuckInfo[LUCK_INFO_RAINBOW]);
    if (h->HasArtifact(ARTIFACT_RABBIT_FOOT))
        strcat(gText, cLuckInfo[LUCK_INFO_RABBIT_FOOT]);
    if (h->HasArtifact(ARTIFACT_GOLDEN_HORSESHOE))
        strcat(gText, cLuckInfo[LUCK_INFO_HORSESHOE]);
    if (h->HasArtifact(ARTIFACT_GAMBLERS_LUCKY_COIN))
        strcat(gText, cLuckInfo[LUCK_INFO_LUCKY_COIN]);
    if (h->HasArtifact(ARTIFACT_FOUR_LEAF_CLOVER))
        strcat(gText, cLuckInfo[LUCK_INFO_CLOVER]);
    if (h->m_eventFlags & HERO_EVENT_FAERIE_RING)
        strcat(gText, cLuckInfo[LUCK_INFO_FAERIE_RING]);
    if (h->m_eventFlags & HERO_EVENT_IDOL)
        strcat(gText, cLuckInfo[LUCK_INFO_IDOL]);
    if (h->m_eventFlags & HERO_EVENT_FOUNTAIN)
        strcat(gText, cLuckInfo[LUCK_INFO_FOUNTAIN]);
    if (h->m_eventFlags & HERO_EVENT_PYRAMID)
        strcat(gText, cLuckInfo[LUCK_INFO_PYRAMID]);
    if (h->m_secondarySkills[HERO_SKILL_LUCK] == HERO_SKILL_LEVEL_BASIC)
        strcat(gText, cLuckInfo[LUCK_INFO_BASIC_SKILL]);
    if (h->m_secondarySkills[HERO_SKILL_LUCK] == HERO_SKILL_LEVEL_ADVANCED)
        strcat(gText, cLuckInfo[LUCK_INFO_ADVANCED_SKILL]);
    if (h->m_secondarySkills[HERO_SKILL_LUCK] == HERO_SKILL_LEVEL_EXPERT)
        strcat(gText, cLuckInfo[LUCK_INFO_EXPERT_SKILL]);
    if (h->HasArtifact(ARTIFACT_MASTHEAD) && (h->m_eventFlags & HERO_EVENT_EMBARKED))
        strcat(gText, cLuckInfo[LUCK_INFO_MASTHEAD]);
    if (h->m_eventFlags & HERO_EVENT_MERMAID)
        strcat(gText, cLuckInfo[LUCK_INFO_MERMAID]);
    if (h->HasArtifact(ARTIFACT_BATTLE_GARB))
        strcat(gText, cLuckInfo[LUCK_INFO_BATTLE_GARB]);
    if (static_cast<int>(strlen(gText)) == modifierStart)
        strcat(gText, cLuckInfo[LUCK_INFO_NONE]);

    NormalDialog(gText, dialogType, -1, -1, -1, 0, -1, 0, -1, 0);
}

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

// @early-stop
// Exact 0x574 frame and 27/27 relocation sites with no base-only target.
// Normalized instructions are identical; the retained 99.996% residual consists
// only of delinked filename and prompt literal identities.
VA(0x0049ce14, 0x4ac)
int AddScoreToHighScore(int score, int days, int scenario, int highScoreType, char *scenarioName)
{
    int destination;
    HighScoreEntry entries_a[HIGH_SCORE_ENTRY_COUNT];
    int file_a;
    int entry;
    char filename_a[352];
    char playerName_c[20];
    int missingFile;

    missingFile = 0;
    if (highScoreType == HIGH_SCORE_STANDARD)
        sprintf(filename_a, "%sSTANDARD.HS", ".\\DATA\\");
    else
        sprintf(filename_a, "%sCAMPAIGN.HS", ".\\DATA\\");

    file_a = _open(filename_a, HIGH_SCORE_FILE_READ_FLAGS);
    if (file_a == -1)
        missingFile = 1;
    if (missingFile) {
        for (entry = 0; entry < HIGH_SCORE_ENTRY_COUNT; entry++) {
            memset(&entries_a[entry], 0, sizeof(HighScoreEntry));
            entries_a[entry].score = HIGH_SCORE_EMPTY;
        }
    } else {
        for (entry = 0; entry < HIGH_SCORE_ENTRY_COUNT; entry++)
            _read(file_a, &entries_a[entry], sizeof(entries_a[entry]));
        _close(file_a);
    }

    gbShowHighScore = 1;
    giHighScoreType = highScoreType;
    giHighScoreRank = HIGH_SCORE_EMPTY;
    giScore = score;
    for (entry = 0; entry < HIGH_SCORE_ENTRY_COUNT; entry++) {
        if ((entries_a[entry].score <= score && highScoreType == HIGH_SCORE_STANDARD) ||
            (score <= entries_a[entry].score && highScoreType == HIGH_SCORE_CAMPAIGN) ||
            (score <= entries_a[entry].score && highScoreType == HIGH_SCORE_EXPANSION_CAMPAIGN) ||
            entries_a[entry].score == HIGH_SCORE_EMPTY) {
            giHighScoreRank = entry;
            break;
        }
    }

    if (entry < HIGH_SCORE_ENTRY_COUNT) {
        for (destination = HIGH_SCORE_ENTRY_COUNT - 2; destination >= entry; destination--)
            entries_a[destination + 1] = entries_a[destination];

        GetDataEntry("Please enter your name for the high score list.", playerName_c,
                     HIGH_SCORE_NAME_LENGTH, 0, 0, 1);
        memset(&entries_a[entry], 0, sizeof(HighScoreEntry));
        strcpy(entries_a[entry].playerName, playerName_c);
        strcpy(entries_a[entry].scenarioName, scenarioName);
        entries_a[entry].score = score;
        entries_a[entry].days = days;
        entries_a[entry].scenario = scenario;
        entries_a[entry].cheated = gpGame->m_cheated;
        if (highScoreType == HIGH_SCORE_CAMPAIGN && gpGame->m_campaignCheated)
            entries_a[entry].cheated = 1;

        file_a = _open(filename_a, HIGH_SCORE_FILE_WRITE_FLAGS, HIGH_SCORE_FILE_PERMISSIONS);
        if (file_a == -1)
            FileError(filename_a);
        for (entry = 0; entry < HIGH_SCORE_ENTRY_COUNT; entry++)
            _write(file_a, &entries_a[entry], sizeof(HighScoreEntry));
        _close(file_a);
    } else {
        gbShowHighScore = 0;
    }
    return 0;
}

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
    if (gpAdvManager->m_active == 1)
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
    KbRemotePacket *data;
    PollSound();
    data = reinterpret_cast<KbRemotePacket *>(GetRemoteData(1));
    if (data && data->group == NET_BOX_REMOTE_CONTROL) {
        switch (data->command) {
        case NET_BOX_REMOTE_SETUP:
            memcpy(gbGamePosToNetPos, data->payload.setup.gamePosToNetPos,
                   OLD_MAIN_PLAYER_COUNT);
            gbUseRegularCompression = data->payload.setup.useRegularCompression;
            gbUseDiffCompression = data->payload.setup.useDiffCompression;
            memcpy(gsNetPlayerInfo, data->payload.setup.players,
                   sizeof(data->payload.setup.players));
            giThisGamePos = NetPosToGamePos(giThisNetPos);
            break;
        case NET_BOX_REMOTE_SAVE:
            result = gpGame->ReceiveSaveGame(data->payload.save.saveId,
                                             data->payload.save.saveOffset,
                                             data->payload.save.saveSize,
                                             data->sender);
            break;
        }
    }
    return result;
}

// @match-note
// Retail frame 0x158, CFG, and every stack slot align; relocations are 131/131
// with only delinked literal/addend aliases. The sole code residual is the
// printable-key guard: equivalent byte-vs-zero-extend comparisons, with the
// retail direct-byte sequence eight bytes shorter. Revisit at 95% for systematic
// guard-expression steering.
VA(0x0049d4a6, 0xb85)
void PopNetBox(char *text, int netPlayer)
{
    int textY_d;
    long messageTime_b;
    heroWindow *netWindow_j;
    int result_a;
    int textWidth_b;
    int textX_d;
    int savedShowIt_a;
    int updateInput_a;
    int inputLength_a;
    char inputText_c[NET_BOX_TEXT_LENGTH];
    int exitForIncomingData_i;
    int redrawLines_a;
    tag_message event_a;
    tag_message updateMessage_f;
    int firstLineId_a;
    int delay_h;
    int lineTextLimit_p;
    int done_i;
    int redrawAdventure;
    int redrawSavedShowIt_a;
    KbRemotePacket *remoteData_g;
    int sendText_h;
    int cursorState;

    if (!gbRemoteOn)
        return;

    lineTextLimit_p = NET_BOX_LINE_TEXT_LIMIT;
    firstLineId_a = NET_BOX_FIRST_LINE_ID;
    textX_d = NET_BOX_TEXT_X;
    textY_d = NET_BOX_TEXT_Y;
    messageTime_b = 0;
    if (text != 0) {
        if (netPlayer >= 0) {
            sprintf(gText, "%s:  %s", gsNetPlayerInfo[netPlayer].name, text);
            gText[NET_BOX_LINE_TEXT_LIMIT] = 0;
            AddNetBoxLine(gText, gpGame->m_players[NetPosToGamePos(netPlayer)].m_color);
        } else {
            sprintf(gText, text);
            gText[NET_BOX_LINE_TEXT_LIMIT] = 0;
            AddNetBoxLine(gText, NET_BOX_DEFAULT_COLOR);
        }
        messageTime_b = KBTickCount();
    }

    inputLength_a = 0;
    savedShowIt_a = bShowIt;
    bShowIt = 1;
    gbMoveShown = 0;
    netWindow_j = new heroWindow(0, NET_BOX_WINDOW_Y, "netbox.bin");
    if (netWindow_j == 0)
        MemError();

    updateMessage_f.type = NET_BOX_UPDATE_MESSAGE;
    updateMessage_f.payload.widget.command = NET_BOX_TEXT_COMMAND;
    updateMessage_f.payload.widget.id = 1;
    updateMessage_f.payload.widget.data.text = cNetBoxLine[0];
    netWindow_j->BroadcastMessage(updateMessage_f);
    updateMessage_f.payload.widget.id = 2;
    updateMessage_f.payload.widget.data.text = cNetBoxLine[1];
    netWindow_j->BroadcastMessage(updateMessage_f);
    updateMessage_f.payload.widget.id = 3;
    updateMessage_f.payload.widget.data.text = cNetBoxLine[2];
    netWindow_j->BroadcastMessage(updateMessage_f);
    updateMessage_f.payload.widget.id = 4;
    updateMessage_f.payload.widget.data.text = cNetBoxLine[3];
    netWindow_j->BroadcastMessage(updateMessage_f);
    updateMessage_f.payload.widget.command = NET_BOX_COLOR_COMMAND;
    updateMessage_f.payload.widget.id = NET_BOX_FIRST_COLOR_ID;
    updateMessage_f.payload.widget.data.value = cNetBoxColor[0] + 1;
    netWindow_j->BroadcastMessage(updateMessage_f);
    updateMessage_f.payload.widget.id = NET_BOX_SECOND_COLOR_ID;
    updateMessage_f.payload.widget.data.value = cNetBoxColor[1] + 1;
    netWindow_j->BroadcastMessage(updateMessage_f);
    updateMessage_f.payload.widget.id = NET_BOX_THIRD_COLOR_ID;
    updateMessage_f.payload.widget.data.value = cNetBoxColor[2] + 1;
    netWindow_j->BroadcastMessage(updateMessage_f);
    updateMessage_f.payload.widget.id = NET_BOX_FOURTH_COLOR_ID;
    updateMessage_f.payload.widget.data.value = cNetBoxColor[3] + 1;
    netWindow_j->BroadcastMessage(updateMessage_f);
    updateMessage_f.payload.widget.id = NET_BOX_THIS_PLAYER_COLOR_ID;
    updateMessage_f.payload.widget.data.value = gpGame->m_players[NetPosToGamePos(giThisNetPos)].m_color + 1;
    netWindow_j->BroadcastMessage(updateMessage_f);

    gpWindowManager->AddWindow(netWindow_j, -1, 1);
    exitForIncomingData_i = 0;
    done_i = 0;
    updateInput_a = 1;
    cursorState = 0;
    sendText_h = 0;
    redrawLines_a = 1;
    redrawAdventure = 0;
    strcpy(inputText_c, "");
    gpInputManager->SetKeyCodeType(0);

    while (!done_i) {
        PollSound();
        remoteData_g = reinterpret_cast<KbRemotePacket *>(GetRemoteData(0));
        if (remoteData_g != 0) {
            if (remoteData_g->group == NET_BOX_REMOTE_GROUP) {
                remoteData_g = reinterpret_cast<KbRemotePacket *>(GetRemoteData(1));
                switch (remoteData_g->command) {
                case NET_BOX_REMOTE_MAP_CHANGE:
                    gbLeaveNetBoxAlone = 1;
                    if (gpAdvManager->m_active == 1) {
                        bShowIt = savedShowIt_a;
                        gpAdvManager->ProcessIncomingGroupMapChange(remoteData_g->payload.data);
                        bShowIt = 1;
                        redrawAdventure = 1;
                    }
                    gbLeaveNetBoxAlone = 0;
                    updateInput_a = 1;
                    break;
                }
            } else if (remoteData_g->group != NET_BOX_REMOTE_CONTROL) {
                remoteData_g = reinterpret_cast<KbRemotePacket *>(GetRemoteData(1));
            } else {
                switch (remoteData_g->command) {
                case NET_BOX_REMOTE_CHAT:
                    remoteData_g = reinterpret_cast<KbRemotePacket *>(GetRemoteData(1));
                    sprintf(gText, "%s:  %s", gsNetPlayerInfo[remoteData_g->sender].name,
                            remoteData_g->payload.data);
                    AddNetBoxLine(gText,
                                  gpGame->m_players[NetPosToGamePos(remoteData_g->sender)].m_color);
                    redrawLines_a = 1;
                    if (messageTime_b != 0)
                        messageTime_b = KBTickCount();
                    break;
                default:
                    AddNetBoxLine("[ Incoming data, must exit... ]", NET_BOX_DEFAULT_COLOR);
                    redrawLines_a = 1;
                    exitForIncomingData_i = 1;
                    break;
                }
            }
        }

        Process1WindowsMessage();
        event_a = gpInputManager->GetEvent();
        switch (event_a.type) {
        case NET_BOX_KEY_MESSAGE:
            messageTime_b = 0;
            switch (event_a.payload.keyboard.keyCode) {
            case NET_BOX_KEY_ESCAPE:
            case NET_BOX_KEY_F1:
                done_i = 1;
                break;
            case NET_BOX_KEY_BACKSPACE:
                if (inputLength_a > 0)
                    inputLength_a--;
                updateInput_a = 1;
                cursorState = 1;
                break;
            case NET_BOX_KEY_ENTER:
                sendText_h = 1;
                break;
            default:
                if (static_cast<unsigned char>(event_a.payload.keyboard.keyCode) <
                        NET_BOX_FIRST_PRINTABLE ||
                    static_cast<unsigned char>(event_a.payload.keyboard.keyCode) >
                        NET_BOX_LAST_PRINTABLE)
                    break;
                if (inputLength_a < NET_BOX_MAX_INPUT && event_a.payload.keyboard.keyCode != 0) {
                    inputText_c[inputLength_a] = 0;
                    textWidth_b = smallFont->LineWidth(inputText_c);
                    if (textWidth_b + NET_BOX_CURSOR_WIDTH_PADDING < NET_BOX_CURSOR_WIDTH_LIMIT) {
                        inputText_c[inputLength_a] = static_cast<char>(event_a.payload.keyboard.keyCode);
                        inputLength_a++;
                        updateInput_a = 1;
                        cursorState = 0;
                    }
                }
            }
        }

        if (!updateInput_a && glTimers[0] < KBTickCount()) {
            cursorState = 1 - cursorState;
            updateInput_a = 1;
        }
        if (sendText_h) {
            sendText_h = 0;
            inputText_c[inputLength_a] = 0;
            AddNetBoxLine(inputText_c, gpGame->m_players[NetPosToGamePos(giThisNetPos)].m_color);
            result_a = TransmitRemoteData(inputText_c, NET_BOX_PACKET_BUFFER_SIZE,
                                        strlen(inputText_c) + 1,
                                        NET_BOX_REMOTE_CHAT, 1, 1, -1);
            if (!result_a)
                ShutDown(0);
            inputLength_a = 0;
            strcpy(inputText_c, "");
            updateInput_a = 1;
            redrawLines_a = 1;
        }

        if (redrawLines_a) {
            redrawLines_a = 0;
            updateMessage_f.type = NET_BOX_UPDATE_MESSAGE;
            updateMessage_f.payload.widget.command = NET_BOX_TEXT_COMMAND;
            updateMessage_f.payload.widget.id = 1;
            updateMessage_f.payload.widget.data.text = cNetBoxLine[0];
            netWindow_j->BroadcastMessage(updateMessage_f);
            updateMessage_f.payload.widget.id = 2;
            updateMessage_f.payload.widget.data.text = cNetBoxLine[1];
            netWindow_j->BroadcastMessage(updateMessage_f);
            updateMessage_f.payload.widget.id = 3;
            updateMessage_f.payload.widget.data.text = cNetBoxLine[2];
            netWindow_j->BroadcastMessage(updateMessage_f);
            updateMessage_f.payload.widget.id = 4;
            updateMessage_f.payload.widget.data.text = cNetBoxLine[3];
            netWindow_j->BroadcastMessage(updateMessage_f);
            updateMessage_f.payload.widget.command = NET_BOX_COLOR_COMMAND;
            updateMessage_f.payload.widget.id = NET_BOX_FIRST_COLOR_ID;
            updateMessage_f.payload.widget.data.value = cNetBoxColor[0] + 1;
            netWindow_j->BroadcastMessage(updateMessage_f);
            updateMessage_f.payload.widget.id = NET_BOX_SECOND_COLOR_ID;
            updateMessage_f.payload.widget.data.value = cNetBoxColor[1] + 1;
            netWindow_j->BroadcastMessage(updateMessage_f);
            updateMessage_f.payload.widget.id = NET_BOX_THIRD_COLOR_ID;
            updateMessage_f.payload.widget.data.value = cNetBoxColor[2] + 1;
            netWindow_j->BroadcastMessage(updateMessage_f);
            updateMessage_f.payload.widget.id = NET_BOX_FOURTH_COLOR_ID;
            updateMessage_f.payload.widget.data.value = cNetBoxColor[3] + 1;
            netWindow_j->BroadcastMessage(updateMessage_f);
            netWindow_j->DrawWindow();
            gpWindowManager->UpdateScreenRegion(0, NET_BOX_WINDOW_Y, NET_BOX_WIDTH, NET_BOX_HEIGHT);
        }

        if (updateInput_a) {
            updateInput_a = 0;
            glTimers[0] = KBTickCount() + NET_BOX_CURSOR_DELAY;
            if (cursorState)
                inputText_c[inputLength_a] = '_';
            else
                inputText_c[inputLength_a] = NET_BOX_CURSOR_GLYPH;
            inputText_c[inputLength_a + 1] = 0;
            updateMessage_f.type = NET_BOX_UPDATE_MESSAGE;
            updateMessage_f.payload.widget.command = NET_BOX_TEXT_COMMAND;
            updateMessage_f.payload.widget.id = NET_BOX_INPUT_ID;
            updateMessage_f.payload.widget.data.text = inputText_c;
            netWindow_j->BroadcastMessage(updateMessage_f);
            netWindow_j->DrawWindow();
            gpWindowManager->UpdateScreenRegion(0, NET_BOX_INPUT_Y, NET_BOX_WIDTH, NET_BOX_INPUT_HEIGHT);
        }

        if (messageTime_b != 0 && messageTime_b + NET_BOX_MESSAGE_TIMEOUT < KBTickCount())
            done_i = 1;
        if (exitForIncomingData_i) {
            for (delay_h = 0; delay_h < NET_BOX_EXIT_DELAY_STEPS; delay_h++) {
                PollSound();
                DelayMilli(NET_BOX_EXIT_DELAY);
            }
            done_i = 1;
        }
    }

    gpInputManager->SetKeyCodeType(1);
    if (redrawAdventure && gbMoveShown) {
        gbDrawWindowBackground = 0;
        gpWindowManager->RemoveWindow(netWindow_j);
        gbDrawWindowBackground = 1;
        redrawSavedShowIt_a = bShowIt;
        bShowIt = 1;
        gpAdvManager->RedrawAdvScreen(1, 0);
        bShowIt = redrawSavedShowIt_a;
    } else {
        gpWindowManager->RemoveWindow(netWindow_j);
    }
    bShowIt = savedShowIt_a;
}

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

// @match-note
// tu-cumulative: logic + all 14 frame slots byte-exact (od_oracle-verified). The only
// residual (coffcmp: 40 bytes, all in the two brightness averages + the minDist test)
// is a /Od operand-evaluation-order difference this cl renders vs retail: the 3-term
// sum `p[2]+p[0]+p[1]` reads +2,+1,+0 here but +2,+0,+1 in retail, and the `d>p`
// compare loads the other operand first. Not source-steerable (probed every term
// ordering, explicit grouping, `|0`, and an inline helper — all identical here).
// Revisit at 95% for systematic evaluation-order steering.
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

// @early-stop
// Exact 0x330 frame after recovering the 0x304 palette aggregate with its 0x300
// copied prefix. All normalized instructions and 69/69 relocation sites align;
// the 99.92% residual is limited to delinked source-line and string identities.
VA(0x0049e5fd, 0x303)
void ShowCongrats(int highScoreType)
{
    unsigned char savedPalette[CONGRATS_PALETTE_BUFFER_SIZE];
    int baseScore;
    int score;
    char rating[CONGRATS_RATING_LENGTH];

    gpMouseManager->HideColorPointer();
    memcpy(savedPalette, gpBufferPalette->m_data, CONGRATS_PALETTE_SIZE);
    gpWindowManager->m_updateFlags = 0;
    congratsText = static_cast<char *>(BaseAlloc(
        CONGRATS_TEXT_SIZE, KBFILE,
        *reinterpret_cast<const short *>("\x97\x0f") + 9));
    baseScore = CalcBaseScore(giCurTurn);
    score = gpGame->m_difficultyRating * baseScore / CONGRATS_DIFFICULTY_SCALE;
    gpSoundManager->PlayAmbientMusic(CONGRATS_MUSIC_SILENT, 0, CONGRATS_MUSIC_SILENT);

    if (highScoreType == CONGRATS_STANDARD) {
        sprintf(rating, gArmyNames[GetMonType(score, highScoreType)]);
    } else if (highScoreType == CONGRATS_EXPANSION_CAMPAIGN) {
        sprintf(rating,
                gArmyNames[GetMonType(xCampaign.Days(), highScoreType)]);
    } else {
        sprintf(rating, gArmyNames[GetMonType(gpGame->m_campaignScore, highScoreType)]);
    }
    rating[0] -= 0x20;
    if (static_cast<signed char>(gpGame->m_cheated))
        sprintf(rating, "Cheater!!!");

    if (highScoreType == CONGRATS_STANDARD) {
        sprintf(congratsText,
                "Congratulations!\n\nDays: %d\nBase Score: %d\nDifficulty: %d\n\nScore: %d\n\nRating:\n%s\n",
                giCurTurn, baseScore, gpGame->m_difficultyRating, score, rating);
    } else if (highScoreType == CONGRATS_EXPANSION_CAMPAIGN) {
        sprintf(congratsText,
                "Congratulations!\n\nDays: %d\n\nRating:\n%s\n",
                xCampaign.Days(), rating);
    } else {
        sprintf(congratsText,
                "Congratulations!\n\nDays: %d\n\nRating:\n%s\n",
                gpGame->m_campaignScore, rating);
    }

    PlaySmacker(CONGRATS_SMACKER);
    memcpy(gpBufferPalette->m_data, gPalette->m_data, CONGRATS_PALETTE_SIZE);
    SmackFade(reinterpret_cast<unsigned char *>(gpBufferPalette->m_data), savedPalette);
    memcpy(gPalette->m_data, savedPalette, CONGRATS_PALETTE_SIZE);
    memcpy(gpBufferPalette->m_data, gPalette->m_data, CONGRATS_PALETTE_SIZE);
    gpMouseManager->ShowColorPointer();
    AddScoreToHighScore(score, giCurTurn, gpGame->m_difficultyRating,
                        CONGRATS_STANDARD, gpGame->m_mapHeader.name);
    BaseFree(congratsText, KBFILE,
             *reinterpret_cast<const short *>("\x97\x0f") + 0x4e);
    congratsText = 0;
    gpWindowManager->m_updateFlags = 1;
    memcpy(gpBufferPalette->m_data, gPalette->m_data, CONGRATS_PALETTE_SIZE);
}

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
    town *t = GetCastleRec(i);
    return t->m_name;
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

// @early-stop
// Excluding the two switch data tables at function offsets [0x84d, 0x905) and
// [0x905, 0x98a), explicit-range comparison finds all 477 instructions aligned;
// only the delinked table-address operands retain different symbol identities.
// The 0x20 frame and stack slots match. All 163 target relocations agree by
// offset/type; the base-only PostMessageA and WritePrefs entries are resolved by
// delinking, and the remaining aliases are literals or local switch labels.
VA(0x0049ec05, 0xa18)
int HandleAppSpecificMenuCommands(int command)
{
    int menuChanged;
    hero *currentHeroRec;
    int loopIndex;
    int secondarySkillIndex;
    int secondaryLevel;
    int formationHexIndex;

    menuChanged = 0;
    currentHeroRec = 0;
    if (gpCurPlayer != 0 && gpCurPlayer->CurrentHero() != -1)
        currentHeroRec = &gpGame->m_heroRecs[gpCurPlayer->CurrentHero()];

    switch (command) {
    case APP_MENU_RESTART_0:
    case APP_MENU_RESTART_1:
    case APP_MENU_RESTART_2:
    case APP_MENU_RESTART_3:
    case APP_MENU_RESTART_4:
    case APP_MENU_RESTART_5:
    case APP_MENU_RESTART_6:
    case APP_MENU_RESTART_7:
    case APP_MENU_RESTART_8:
    case APP_MENU_RESTART_9:
    case APP_MENU_RESTART_10:
    case APP_MENU_RESTART_11:
    case APP_MENU_RESTART_12:
    case APP_MENU_RESTART_13:
        strcpy(gText, "Are you sure you want to restart?  (Your current game will be lost)");
        goto confirmMenuCommand;

    case APP_MENU_LOAD_0:
    case APP_MENU_LOAD_1:
    case APP_MENU_LOAD_2:
    case APP_MENU_LOAD_3:
    case APP_MENU_LOAD_4:
    case APP_MENU_LOAD_5:
    case APP_MENU_LOAD_6:
    case APP_MENU_LOAD_7:
    case APP_MENU_LOAD_8:
    case APP_MENU_LOAD_9:
    case APP_MENU_LOAD_10:
        strcpy(gText, "Are you sure you want to load a new game?  (Your current game will be lost)");
confirmMenuCommand:
        if (gpAdvManager->m_active == 1) {
            NormalDialog(gText, APP_MENU_CONFIRM_DIALOG, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gpWindowManager->m_dialogResult != APP_MENU_CONFIRM_OK)
                break;
        }
        giMenuCommand = command;
        break;

    case APP_MENU_SAVE:
        SaveGame();
        break;
    case APP_MENU_EXIT:
        PostMessageA(hwndApp, APP_MENU_CLOSE_MESSAGE, 0, 0);
        break;

    case APP_MENU_MUSIC_FIRST:
        gConfig.musicVolume = 0;
        goto adjustMusic;
    case APP_MENU_MUSIC_FIRST + 1:
        gConfig.musicVolume = 1;
        goto adjustMusic;
    case APP_MENU_MUSIC_FIRST + 2:
        gConfig.musicVolume = 2;
        goto adjustMusic;
    case APP_MENU_MUSIC_FIRST + 3:
        gConfig.musicVolume = 3;
        goto adjustMusic;
    case APP_MENU_MUSIC_FIRST + 4:
        gConfig.musicVolume = 4;
        goto adjustMusic;
    case APP_MENU_MUSIC_FIRST + 5:
        gConfig.musicVolume = 5;
        goto adjustMusic;
    case APP_MENU_MUSIC_FIRST + 6:
        gConfig.musicVolume = 6;
        goto adjustMusic;
    case APP_MENU_MUSIC_FIRST + 7:
        gConfig.musicVolume = 7;
        goto adjustMusic;
    case APP_MENU_MUSIC_FIRST + 8:
        gConfig.musicVolume = 8;
        goto adjustMusic;
    case APP_MENU_MUSIC_FIRST + 9:
        gConfig.musicVolume = 9;
        goto adjustMusic;
    case APP_MENU_MUSIC_LAST:
        gConfig.musicVolume = 10;
        goto adjustMusic;
adjustMusic:
        gpSoundManager->AdjustMusicVolumes();
        menuChanged = 1;
        break;

    case APP_MENU_SOUND_FIRST:
        gConfig.soundVolume = 0;
        goto adjustSound;
    case APP_MENU_SOUND_FIRST + 1:
        gConfig.soundVolume = 1;
        goto adjustSound;
    case APP_MENU_SOUND_FIRST + 2:
        gConfig.soundVolume = 2;
        goto adjustSound;
    case APP_MENU_SOUND_FIRST + 3:
        gConfig.soundVolume = 3;
        goto adjustSound;
    case APP_MENU_SOUND_FIRST + 4:
        gConfig.soundVolume = 4;
        goto adjustSound;
    case APP_MENU_SOUND_FIRST + 5:
        gConfig.soundVolume = 5;
        goto adjustSound;
    case APP_MENU_SOUND_FIRST + 6:
        gConfig.soundVolume = 6;
        goto adjustSound;
    case APP_MENU_SOUND_FIRST + 7:
        gConfig.soundVolume = 7;
        goto adjustSound;
    case APP_MENU_SOUND_FIRST + 8:
        gConfig.soundVolume = 8;
        goto adjustSound;
    case APP_MENU_SOUND_FIRST + 9:
        gConfig.soundVolume = 9;
        goto adjustSound;
    case APP_MENU_SOUND_LAST:
        gConfig.soundVolume = 10;
        goto adjustSound;
adjustSound:
        gpSoundManager->AdjustSoundVolumes();
        menuChanged = 1;
        break;

    case APP_MENU_TOGGLE_ROUTE:
        gConfig.showRoute = 1 - gConfig.showRoute;
        menuChanged = 1;
        break;
    case APP_MENU_TOGGLE_BLACKOUT:
        gConfig.blackoutComputer = 1 - gConfig.blackoutComputer;
        menuChanged = 1;
        break;

    case APP_MENU_VIEW_WORLD:
        gpAdvManager->ViewWorld(0x35, 0, 0);
        break;
    case APP_MENU_VIEW_PUZZLE:
        gpAdvManager->ViewPuzzle();
        break;
    case APP_MENU_CAST_SPELL:
        gpAdvManager->CheckCastSpell();
        break;
    case APP_MENU_SEARCH:
        gpAdvManager->ProcessSearch(-1, -1);
        break;

    case APP_MENU_CHEAT_REVEAL:
        gpGame->m_cheated = 1;
        if (gbInCampaign)
            gpGame->m_campaignCheated = 1;
        gpGame->SetVisibility(APP_MENU_REVEAL_SIZE, APP_MENU_REVEAL_SIZE,
                              giCurPlayer, APP_MENU_REVEAL_RADIUS);
        if (currentHeroRec != 0)
            gpAdvManager->Reseed(0, 0);
        gpAdvManager->UpdateRadar(1, 0);
        gpAdvManager->CompleteDraw(0);
        gpAdvManager->UpdateScreen(0, 0);
        break;

    case APP_MENU_CHEAT_MOVEMENT:
        gpGame->m_cheated = 1;
        if (gbInCampaign)
            gpGame->m_campaignCheated = 1;
        if (currentHeroRec != 0)
            currentHeroRec->m_remainingMobility = APP_MENU_MOVEMENT_BONUS;
        break;

    case APP_MENU_CHEAT_SPELLS:
        gpGame->m_cheated = 1;
        if (gbInCampaign)
            gpGame->m_campaignCheated = 1;
        if (currentHeroRec != 0) {
            for (loopIndex = 0; loopIndex < APP_MENU_MAX_SPELLS; loopIndex++)
                currentHeroRec->AddSpell(loopIndex, APP_MENU_SPELL_COUNT);
            currentHeroRec->m_spellPoints = 999;
        }
        break;

    case APP_MENU_CHEAT_RESOURCES:
        gpGame->m_cheated = 1;
        if (gbInCampaign)
            gpGame->m_campaignCheated = 1;
        for (loopIndex = 0; loopIndex < APP_MENU_RESOURCE_COUNT; loopIndex++) {
            if (loopIndex == RES_GOLD)
                gpCurPlayer->m_resources[loopIndex] += APP_MENU_GOLD_BONUS;
            else
                gpCurPlayer->m_resources[loopIndex] += APP_MENU_RESOURCE_BONUS;
        }
        gpAdvManager->UpdBottomView(1, 1, 1);
        break;

    default:
        if (command >= APP_MENU_BUILDING_FIRST && command < APP_MENU_BUILDING_LAST) {
            gpGame->m_cheated = 1;
            if (gbInCampaign)
                gpGame->m_campaignCheated = 1;
            giDebugBuildingToBuild = command - APP_MENU_BUILDING_FIRST;
            break;
        }
        if (command >= APP_MENU_ARMY_FIRST && command < APP_MENU_ARMY_LAST) {
            gpGame->m_cheated = 1;
            if (gbInCampaign)
                gpGame->m_campaignCheated = 1;
            if (gpCurPlayer->CurrentHero() != -1) {
                gpGame->GiveArmy(&gpGame->m_heroRecs[gpCurPlayer->CurrentHero()].m_army,
                                 command - APP_MENU_ARMY_FIRST, 5, -1);
                gpAdvManager->UpdBottomView(1, 1, 1);
            }
            break;
        }
        if (command >= APP_MENU_SECONDARY_FIRST && command < APP_MENU_SECONDARY_LAST) {
            gpGame->m_cheated = 1;
            if (gbInCampaign)
                gpGame->m_campaignCheated = 1;
            secondarySkillIndex = (command - APP_MENU_SECONDARY_FIRST) / APP_MENU_SECONDARY_LEVELS;
            secondaryLevel = (command - APP_MENU_SECONDARY_FIRST) % APP_MENU_SECONDARY_LEVELS;
            if (currentHeroRec != 0)
                currentHeroRec->SetSS(secondarySkillIndex, secondaryLevel);
        }
        if (command >= APP_MENU_COMBAT_FIRST && command < APP_MENU_COMBAT_LAST) {
            gpCombatManager->m_debugFormation = command - APP_MENU_COMBAT_FIRST;
            gpCombatManager->m_backgroundDrawn = 0;
            for (loopIndex = 0; loopIndex < APP_MENU_COMBAT_HEX_COUNT; loopIndex++) {
                gpCombatManager->m_hexCells[loopIndex].m_blocked = 0;
                gpCombatManager->m_hexCells[loopIndex].m_obstacleIndex = -1;
            }
            for (loopIndex = 0; loopIndex < APP_MENU_FORMATION_HEX_COUNT; loopIndex++) {
                formationHexIndex =
                    sElevationOverlay[gpCombatManager->m_debugFormation]
                        .cellOffsets[loopIndex];
                if (formationHexIndex != -1)
                    gpCombatManager->m_hexCells[formationHexIndex].m_blocked = 1;
            }
            gpCombatManager->SetupGridForArmy(
                &gpCombatManager->m_armies[gpCombatManager->m_currentArmySide]
                                            [gpCombatManager->m_currentArmyIndex]);
            gpCombatManager->DrawFrame(1, 0, 0, 0, 0, 1, 1);
        }
        return 1;
    }

    if (menuChanged)
        WritePrefs();
    return 0;
}

// @match-note
// Pre-95 structural checkpoint (94.755%): exact 0x10 frame, exact 0x310 span,
// complete music/sound menu loops and switches, and 38/38 relocation sites with
// no base-only target. Both 0x28-byte tables start at the retail offsets +0x130
// and +0x24a. The first residual is a delinked gConfig interior-symbol identity;
// direct loops and retail-order cases are present. Revisit at 95% for explicit-
// range raw-byte proof and any remaining local-label steering.
VA(0x0049f61d, 0x310)
void UpdateSystemOptionsMenu(void)
{
    int menuCommand;
    int checkedCommand;

    if (gConfig.gfx[giCurExe].showMenu == 0)
        return;
    if (hmnuApp == 0)
        return;
    if (hmnuApp != hmnuAdv)
        return;

    for (menuCommand = APP_MENU_MUSIC_FIRST; menuCommand <= APP_MENU_MUSIC_LAST;
         menuCommand++)
        CheckMenuItem(hmnuApp, menuCommand, APP_MENU_UNCHECKED);
    switch (gConfig.musicVolume) {
    case 1: checkedCommand = APP_MENU_MUSIC_FIRST + 1; break;
    case 2: checkedCommand = APP_MENU_MUSIC_FIRST + 2; break;
    case 3: checkedCommand = APP_MENU_MUSIC_FIRST + 3; break;
    case 4: checkedCommand = APP_MENU_MUSIC_FIRST + 4; break;
    case 5: checkedCommand = APP_MENU_MUSIC_FIRST + 5; break;
    case 6: checkedCommand = APP_MENU_MUSIC_FIRST + 6; break;
    case 7: checkedCommand = APP_MENU_MUSIC_FIRST + 7; break;
    case 8: checkedCommand = APP_MENU_MUSIC_FIRST + 8; break;
    case 9: checkedCommand = APP_MENU_MUSIC_FIRST + 9; break;
    case 10: checkedCommand = APP_MENU_MUSIC_LAST; break;
    default: checkedCommand = APP_MENU_MUSIC_FIRST; break;
    }
    CheckMenuItem(hmnuApp, checkedCommand, APP_MENU_CHECKED);

    for (menuCommand = APP_MENU_SOUND_FIRST; menuCommand <= APP_MENU_SOUND_LAST;
         menuCommand++)
        CheckMenuItem(hmnuApp, menuCommand, APP_MENU_UNCHECKED);
    switch (gConfig.soundVolume) {
    case 1: checkedCommand = APP_MENU_SOUND_FIRST + 1; break;
    case 2: checkedCommand = APP_MENU_SOUND_FIRST + 2; break;
    case 3: checkedCommand = APP_MENU_SOUND_FIRST + 3; break;
    case 4: checkedCommand = APP_MENU_SOUND_FIRST + 4; break;
    case 5: checkedCommand = APP_MENU_SOUND_FIRST + 5; break;
    case 6: checkedCommand = APP_MENU_SOUND_FIRST + 6; break;
    case 7: checkedCommand = APP_MENU_SOUND_FIRST + 7; break;
    case 8: checkedCommand = APP_MENU_SOUND_FIRST + 8; break;
    case 9: checkedCommand = APP_MENU_SOUND_FIRST + 9; break;
    case 10: checkedCommand = APP_MENU_SOUND_LAST; break;
    default: checkedCommand = APP_MENU_SOUND_FIRST; break;
    }
    CheckMenuItem(hmnuApp, checkedCommand, APP_MENU_CHECKED);

    for (menuCommand = APP_MENU_SPEED_FIRST; menuCommand <= APP_MENU_SPEED_LAST;
         menuCommand++)
        CheckMenuItem(hmnuApp, menuCommand, APP_MENU_UNCHECKED);
    CheckMenuItem(hmnuApp, APP_MENU_TOGGLE_ROUTE,
                  gConfig.showRoute ? APP_MENU_CHECKED : APP_MENU_UNCHECKED);
    CheckMenuItem(hmnuApp, APP_MENU_TOGGLE_BLACKOUT,
                  1 - gConfig.blackoutComputer ? APP_MENU_CHECKED : APP_MENU_UNCHECKED);
}

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
void SetupDynamicWindow(int x, int y, int centered, int boundsWidth, int boundsHeight,
                        int contentWidth, int contentHeight, int *windowWidth,
                        int *windowHeight, int *contentLeft, int *contentTop,
                        int *contentRight, int *contentBottom, heroWindow **window,
                        int windowType)
{
    int leftOffset;
    int bottomCornerPaddingNum;
    int numRows;
    widget *newWidgetTemp;
    int columnsSize;
    int topOffsetNum;
    int contentXPaddingCount;
    int centeredHeightCount;
    int centeredPadding;
    int topCornerPaddingCount;
    int bottomOffsetLocal;
    int rightOffset;
    int contentYPadding;
    int edge;
    int tileRowPos;
    int centeredWidthValue;
    int leftCornerPaddingLocal;
    int rightCornerPaddingValue;
    int stoneWidgetColorSize;
    int columnIndex;
    int bottomEdgeOffset;
    int tileWidth;
    int tileHeight;
    int topEdgeInset;

    tileWidth = DYNAMIC_TILE_SIZE;
    tileHeight = DYNAMIC_TILE_SIZE;
    topEdgeInset = -DYNAMIC_EDGE_OFFSET;
    bottomEdgeOffset = -DYNAMIC_EDGE_OFFSET;
    contentXPaddingCount = DYNAMIC_CONTENT_LEFT;
    contentYPadding = DYNAMIC_CONTENT_TOP;
    topCornerPaddingCount = DYNAMIC_CONTENT_TOP;
    bottomCornerPaddingNum = DYNAMIC_CONTENT_TOP;
    leftCornerPaddingLocal = DYNAMIC_CONTENT_TOP;
    rightCornerPaddingValue = DYNAMIC_CONTENT_TOP;
    centeredPadding = DYNAMIC_CONTENT_LEFT;
    stoneWidgetColorSize = DYNAMIC_WIDGET_COLOR;
    newWidgetTemp = 0;
    columnsSize = (contentWidth - 1) / DYNAMIC_TILE_SIZE + 1;
    numRows = (contentHeight - 1) / DYNAMIC_TILE_SIZE + 1;
    *windowWidth = columnsSize * DYNAMIC_TILE_SIZE + DYNAMIC_WINDOW_PADDING;
    *windowHeight = numRows * DYNAMIC_TILE_SIZE + DYNAMIC_WINDOW_PADDING;
    centeredWidthValue = columnsSize * DYNAMIC_TILE_SIZE + DYNAMIC_CONTENT_LEFT;
    centeredHeightCount = numRows * DYNAMIC_TILE_SIZE + DYNAMIC_CONTENT_LEFT;
    if (centered) {
        x += ((boundsWidth - centeredWidthValue) >> 1) - DYNAMIC_CONTENT_TOP;
        y += (boundsHeight - centeredHeightCount) >> 1;
    }
    *contentLeft = x + DYNAMIC_CONTENT_LEFT;
    *contentTop = y + DYNAMIC_CONTENT_TOP;
    *contentRight = columnsSize * DYNAMIC_TILE_SIZE + *contentLeft - 1;
    *contentBottom = numRows * DYNAMIC_TILE_SIZE + *contentTop - 1;

    if (windowType != 0)
        return;
    *window = new heroWindow(x, y, *windowWidth, *windowHeight,
                             DYNAMIC_WINDOW_FLAGS);
    leftOffset = *contentLeft - x;
    topOffsetNum = *contentTop - y;
    rightOffset = *contentRight - x;
    bottomOffsetLocal = *contentBottom - y;

    for (tileRowPos = 0; tileRowPos < numRows; tileRowPos++) {
        for (columnIndex = 0; columnIndex < columnsSize; columnIndex++) {
            newWidgetTemp = new iconWidget(
                columnIndex * DYNAMIC_TILE_SIZE + leftOffset,
                tileRowPos * DYNAMIC_TILE_SIZE + topOffsetNum,
                DYNAMIC_TILE_SIZE, DYNAMIC_TILE_SIZE, "stonebk2.icn",
                DYNAMIC_BACKGROUND_FRAME, 0, -1, DYNAMIC_WIDGET_COLOR, 1);
            if (newWidgetTemp == 0)
                MemError();
            (*window)->AddWidget(newWidgetTemp, -1);
        }
    }

    newWidgetTemp = new iconWidget(leftOffset - DYNAMIC_CORNER_LEFT,
                                   topOffsetNum - DYNAMIC_CORNER_LEFT,
                                   DYNAMIC_CORNER_SIZE, DYNAMIC_CORNER_SIZE,
                                   "stonebk2.icn", 0, 0, -1, DYNAMIC_WIDGET_COLOR, 1);
    if (newWidgetTemp == 0)
        MemError();
    (*window)->AddWidget(newWidgetTemp, -1);

    newWidgetTemp = new iconWidget(rightOffset - DYNAMIC_CORNER_RIGHT,
                                   topOffsetNum - DYNAMIC_CORNER_LEFT,
                                   DYNAMIC_CORNER_SIZE, DYNAMIC_CORNER_SIZE,
                                   "stonebk2.icn", 1, 0, -1, DYNAMIC_WIDGET_COLOR, 1);
    if (newWidgetTemp == 0)
        MemError();
    (*window)->AddWidget(newWidgetTemp, -1);

    newWidgetTemp = new iconWidget(rightOffset - DYNAMIC_CORNER_RIGHT,
                                   bottomOffsetLocal - DYNAMIC_CORNER_RIGHT,
                                   DYNAMIC_CORNER_SIZE, DYNAMIC_CORNER_SIZE,
                                   "stonebk2.icn", 2, 0, -1, DYNAMIC_WIDGET_COLOR, 1);
    if (newWidgetTemp == 0)
        MemError();
    (*window)->AddWidget(newWidgetTemp, -1);

    newWidgetTemp = new iconWidget(leftOffset - DYNAMIC_CORNER_LEFT,
                                   bottomOffsetLocal - DYNAMIC_CORNER_RIGHT,
                                   DYNAMIC_CORNER_SIZE, DYNAMIC_CORNER_SIZE,
                                   "stonebk2.icn", 3, 0, -1, DYNAMIC_WIDGET_COLOR, 1);
    if (newWidgetTemp == 0)
        MemError();
    (*window)->AddWidget(newWidgetTemp, -1);

    for (edge = 0; edge < columnsSize; edge++) {
        newWidgetTemp = new iconWidget(
            edge * DYNAMIC_TILE_SIZE + leftOffset - DYNAMIC_EDGE_OFFSET,
            topOffsetNum - DYNAMIC_CORNER_LEFT,
            DYNAMIC_CORNER_SIZE, DYNAMIC_CORNER_SIZE, "stonebk2.icn",
            Random(DYNAMIC_TOP_FRAME_FIRST, DYNAMIC_TOP_FRAME_LAST), 0, -1,
            DYNAMIC_WIDGET_COLOR, 1);
        if (newWidgetTemp == 0)
            MemError();
        (*window)->AddWidget(newWidgetTemp, -1);

        newWidgetTemp = new iconWidget(
            edge * DYNAMIC_TILE_SIZE + leftOffset - DYNAMIC_EDGE_OFFSET,
            bottomOffsetLocal - DYNAMIC_CORNER_RIGHT,
            DYNAMIC_CORNER_SIZE, DYNAMIC_CORNER_SIZE, "stonebk2.icn",
            Random(DYNAMIC_BOTTOM_FRAME_FIRST, DYNAMIC_BOTTOM_FRAME_LAST), 0, -1,
            DYNAMIC_WIDGET_COLOR, 1);
        if (newWidgetTemp == 0)
            MemError();
        (*window)->AddWidget(newWidgetTemp, -1);
    }

    for (edge = 0; edge < numRows; edge++) {
        newWidgetTemp = new iconWidget(
            leftOffset - DYNAMIC_CORNER_LEFT,
            edge * DYNAMIC_TILE_SIZE + topOffsetNum - DYNAMIC_EDGE_OFFSET,
            DYNAMIC_CORNER_SIZE, DYNAMIC_CORNER_SIZE, "stonebk2.icn",
            Random(DYNAMIC_LEFT_FRAME_FIRST, DYNAMIC_LEFT_FRAME_LAST), 0, -1,
            DYNAMIC_WIDGET_COLOR, 1);
        if (newWidgetTemp == 0)
            MemError();
        (*window)->AddWidget(newWidgetTemp, -1);

        newWidgetTemp = new iconWidget(
            rightOffset - DYNAMIC_CORNER_RIGHT,
            edge * DYNAMIC_TILE_SIZE + topOffsetNum - DYNAMIC_EDGE_OFFSET,
            DYNAMIC_CORNER_SIZE, DYNAMIC_CORNER_SIZE, "stonebk2.icn",
            Random(DYNAMIC_RIGHT_FRAME_FIRST, DYNAMIC_RIGHT_FRAME_LAST), 0, -1,
            DYNAMIC_WIDGET_COLOR, 1);
        if (newWidgetTemp == 0)
            MemError();
        (*window)->AddWidget(newWidgetTemp, -1);
    }
}

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
        pe.netPosition = gbGamePosToNetPos[pos];
        pe.gamePosition = pos;
        pe.updateNetworkControl = 0;
        pe.timedOut = 0;
        pe.eliminated = 1;
        pe.hostReported = 0;
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
void ReceiveHostReportsPlayerExit(int hostNetPosition, SPlayerExit exitInfo,
                                  int forwardedReport)
{
    int showExitMessage;
    char playerExitMessage[PLAYER_EXIT_MESSAGE_LENGTH];
    int netPosition;

    showExitMessage = 0;
    if (!forwardedReport) {
        if (exitInfo.eliminated) {
            if (exitInfo.netPosition == giThisNetPos) {
                RemoteCleanup();
                sprintf(gText, "You have been eliminated from the game!!!");
                NormalDialog(gText, PLAYER_EXIT_DIALOG_INFO, -1, -1, -1, 0,
                             -1, 0, -1, 0);
                gbGameOver = 1;
                giEndSequence = 0;
                return;
            }

            sprintf(gText, "%s has been vanquished!",
                    gsNetPlayerInfo[exitInfo.netPosition].name);
            NormalDialog(gText, PLAYER_EXIT_DIALOG_INFO, -1, -1, 9,
                         gpGame->GetPlayerColor(exitInfo.gamePosition), -1, -1,
                         -1, PLAYER_EXIT_MESSAGE_TIME);
        } else if (!exitInfo.continueGame) {
            gpGame->SaveGame("PLYREXIT", 1, 0);
            sprintf(
                gText,
                "%s left the game, and the %s decided to terminate the entire game.  The game has been saved as 'PLYREXIT'.  Do you wish to play on alone with the computer taking over for all human players?",
                gsNetPlayerInfo[exitInfo.netPosition].name,
                gsNetPlayerInfo[hostNetPosition].name);
            NormalDialog(gText, PLAYER_EXIT_DIALOG_CONFIRM, -1, -1, -1, 0,
                         -1, 0, -1, 0);
            if (gpWindowManager->m_dialogResult == PLAYER_EXIT_CONFIRM_OK) {
                DropDownToOnePlayer();
            } else {
                RemoteCleanup();
                ShutDown(0);
            }
        } else {
            if (exitInfo.timedOut) {
                sprintf(
                    playerExitMessage,
                    "Host player %s reports that player %s has been timed out of the game.  The game will continue with a computer player filling in for %s.",
                    gsNetPlayerInfo[hostNetPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name);
            } else {
                sprintf(
                    playerExitMessage,
                    "Host player %s reports that player %s has exited the game.  The game will continue with a computer player filling in for %s.",
                    gsNetPlayerInfo[hostNetPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name);
            }
            showExitMessage = 1;
        }
    }

    if (exitInfo.netPosition < giThisNetPos)
        giThisNetPos--;
    gbHumanPlayer[exitInfo.gamePosition] = 0;

    for (netPosition = exitInfo.netPosition;
         netPosition < PLAYER_EXIT_SHIFT_SLOTS; netPosition++) {
        lLastHeartbeatReceive[netPosition] = lLastHeartbeatReceive[netPosition + 1];
        giNetPosToDCOPos[netPosition] = giNetPosToDCOPos[netPosition + 1];
        strcpy(gsNetPlayerInfo[netPosition].name,
               gsNetPlayerInfo[netPosition + 1].name);
    }

    for (netPosition = 0; netPosition < PLAYER_EXIT_NETWORK_SLOTS;
         netPosition++) {
        if (gbGamePosToNetPos[netPosition] == exitInfo.netPosition)
            gbGamePosToNetPos[netPosition] = -1;
        else if (gbGamePosToNetPos[netPosition] > exitInfo.netPosition)
            gbGamePosToNetPos[netPosition]--;
    }

    giNumHumanPlayers--;
    iLastDiffSendTo = -2;
    if (exitInfo.updateNetworkControl)
        ComputeAdvNetControl();

    if (showExitMessage)
        NormalDialog(playerExitMessage, PLAYER_EXIT_DIALOG_INFO, -1, -1, -1, -1,
                     -1, -1, -1, PLAYER_EXIT_MESSAGE_TIME);
}

// @early-stop
// Explicit-range comparison finds all 230 instructions aligned over the exact
// 0x361 span, with frame 0x10 and every stack slot matching. All 45 relocations
// agree by offset/type/target; the residual is delinked literal/local identity.
VA(0x004a07e3, 0x361)
void ReceiveRemotePlayerExit(SPlayerExit exitInfo)
{
    int localPlayerLost;
    int sendReturn;
    int unusedPacketResult;
    int recipient;

    localPlayerLost = 0;
    lLastHeartbeatReceive[exitInfo.netPosition] = PLAYER_EXIT_HEARTBEAT_DISABLED;
    gpGame->SaveGame("PLYREXIT", 1, 0);

    if (exitInfo.eliminated) {
        exitInfo.continueGame = 1;
        if (exitInfo.netPosition == giThisNetPos) {
            localPlayerLost = 1;
            goto exitInfoProcessed;
        }
        sprintf(gText, "%s has been vanquished!",
                gsNetPlayerInfo[exitInfo.netPosition].name);
        NormalDialog(gText, PLAYER_EXIT_DIALOG_INFO, -1, -1, 9,
                     gpGame->GetPlayerColor(exitInfo.gamePosition), -1, -1,
                     -1, PLAYER_EXIT_MESSAGE_TIME);
        exitInfo.continueGame = 1;
    } else {
        if (exitInfo.timedOut) {
            sprintf(
                gText,
                "%s has been timed out of the game.  The current game has been saved as 'PLYREXIT'.  Do you wish to continue playing with a computer player filling in for %s?",
                gsNetPlayerInfo[exitInfo.netPosition].name,
                gsNetPlayerInfo[exitInfo.netPosition].name);
        } else {
            sprintf(
                gText,
                "%s is exiting the game.  The current game has been saved as 'PLYREXIT'.  Do you wish to continue playing with a computer player filling in for %s?",
                gsNetPlayerInfo[exitInfo.netPosition].name,
                gsNetPlayerInfo[exitInfo.netPosition].name);
        }
        NormalDialog(gText, PLAYER_EXIT_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0,
                     -1, 0);
        if (gpWindowManager->m_dialogResult == PLAYER_EXIT_CONFIRM_OK)
            exitInfo.continueGame = 1;
        else
            exitInfo.continueGame = 0;
    }

exitInfoProcessed:
    if (giNumHumanPlayers == 2) {
        if (exitInfo.eliminated && !exitInfo.hostReported) {
            sendReturn = TransmitRemoteData(
                reinterpret_cast<char *>(&exitInfo), 1 - giThisNetPos,
                PLAYER_EXIT_PACKET_TYPE, PLAYER_EXIT_PACKET_COMMAND, 1, 1, -1);
        }
        if (localPlayerLost)
            goto playerExitHandled;
        giNumHumanPlayers--;
        gbHumanPlayer[exitInfo.gamePosition] = 0;
        RemoteCleanup();
        ComputeAdvNetControl();
    } else {
        for (recipient = 0; recipient < PLAYER_EXIT_NETWORK_SLOTS; recipient++) {
            if ((exitInfo.netPosition == recipient && exitInfo.eliminated &&
                 !exitInfo.hostReported) ||
                (exitInfo.netPosition != recipient &&
                 giNumHumanPlayers > recipient && recipient != giThisNetPos)) {
                sendReturn = TransmitRemoteData(
                    reinterpret_cast<char *>(&exitInfo), recipient,
                    PLAYER_EXIT_PACKET_TYPE, PLAYER_EXIT_PACKET_COMMAND, 1, 1, -1);
            }
        }
        if (localPlayerLost)
            goto playerExitHandled;
        ReceiveHostReportsPlayerExit(0, exitInfo, 1);
    }

playerExitHandled:
    if (localPlayerLost) {
        sprintf(gText, "You have been eliminated from the game!!!");
        RemoteCleanup();
        NormalDialog(gText, PLAYER_EXIT_DIALOG_INFO, -1, -1, -1, 0, -1, 0,
                     -1, 0);
        gbGameOver = 1;
        giEndSequence = 0;
    } else if (!exitInfo.continueGame) {
        ShutDown(0);
    }
}

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
            c.payload.widget.command = 3;
            c.payload.widget.id = gWinSetup[i].m_1;
            c.payload.widget.data.text = gWinSetup[i].m_3;
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

// @match-note
// Pre-95 structural checkpoint (97.02%): exact 0x120 frame, named slots, switch
// case semantics/order, and external relocation targets (253/257 sites with no
// base-only target). The first residual is table placement: base table ranges
// begin at +0x2b2 and +0xafc, while retail begins at +0x2c6 and +0xb50. Split
// the sizing and drawing heights, restored the inner panel-Y lifetime, aligned
// every /Od bucket, and tried both resource-order and retail body-order sizing
// switches. Revisit at 95% for systematic table/continuation steering.
VA(0x004a0d9f, 0x17c6)
void NormalDialog(char *text, int dialogType, int windowX, int windowY,
                  int firstResourceType, int firstResourceValue,
                  int secondResourceType, int secondResourceValue,
                  int showOrText, int timeout)
{
    short panelHeight_p;
    short labelY_o;
    widget *borderWidget_o;
    char iconFile_h[NORMAL_DIALOG_FILENAME_LENGTH];
    char *resourceText_e[NORMAL_DIALOG_RESOURCE_COUNT];
    int iconHeight_d;
    int sizingIconHeight_l;
    int showPrimaryBonus_e;
    int resourceType_l[NORMAL_DIALOG_RESOURCE_COUNT];
    int lineCount_d;
    widget *iconPanel_j;
    heroWindow *savedNormalDialogWindow_o;
    int windowWidth_a;
    int savedFirstResourceType_p;
    int resourceImageHeight_g;
    short showMessage_h;
    int windowHeight_k;
    int resourceSlot_n;
    int savedPointerType_e;
    int dialogContentHeight_h;
    int textWidgetId_h;
    tag_message message_e;
    int savedSecondResourceType_f;
    int windowRows_j;
    int maxIconHeight_a;
    int savedFirstResourceValue_i;
    int resourceY_l;
    int resourceFrame_g;
    widget *textPanel_h;
    int resourceCenterX_a;
    int resourceValue_l[NORMAL_DIALOG_RESOURCE_COUNT];
    int savedSecondResourceValue_j;
    char *orText_f;
    int savedPointerFrame_j;

    if (!gbRemoteOn)
        timeout = 0;
    if (timeout > NORMAL_DIALOG_TIMEOUT_MIN &&
        timeout < NORMAL_DIALOG_TIMEOUT_MAX) {
        giDialogTimeout = KBTickCount() + timeout;
    } else {
        giDialogTimeout = timeout;
    }

    resourceCenterX_a = 0;
    resourceY_l = 0;
    resourceFrame_g = 0;
    textWidgetId_h = NORMAL_DIALOG_TEXT_WIDGET_FIRST_ID;
    resourceImageHeight_g = 0;
    iconHeight_d = 0;
    showPrimaryBonus_e = 0;
    showMessage_h = 1;

    if (firstResourceType == NORMAL_DIALOG_PRIMARY_SKILL &&
        firstResourceValue >= NORMAL_DIALOG_PRIMARY_BONUS_OFFSET) {
        firstResourceValue -= NORMAL_DIALOG_PRIMARY_BONUS_OFFSET;
        showPrimaryBonus_e = 1;
    }
    if (firstResourceType >= NORMAL_DIALOG_MONSTER + 1 &&
        firstResourceType <= NORMAL_DIALOG_PRIMARY_SKILL - 1) {
        firstResourceType = NORMAL_DIALOG_NO_RESOURCE;
    }

    savedNormalDialogWindow_o = pNormalDialogWindow;
    savedFirstResourceType_p = giResType1;
    savedFirstResourceValue_i = giResExtra1;
    savedSecondResourceType_f = giResType2;
    savedSecondResourceValue_j = giResExtra2;
    giResType1 = firstResourceType;
    giResExtra1 = firstResourceValue;
    giResType2 = secondResourceType;
    giResExtra2 = secondResourceValue;

    resourceType_l[0] = firstResourceType;
    resourceValue_l[0] = firstResourceValue;
    resourceType_l[1] = secondResourceType;
    resourceValue_l[1] = secondResourceValue;

    lineCount_d = bigFont->LineLength(text, NORMAL_DIALOG_TEXT_LINE_WIDTH);
    dialogContentHeight_h = lineCount_d * NORMAL_DIALOG_TEXT_LINE_HEIGHT;
    maxIconHeight_a = 0;
    if (dialogType != NORMAL_DIALOG_QUICK_VIEW)
        dialogContentHeight_h += 39;

    for (resourceSlot_n = 0; resourceSlot_n < NORMAL_DIALOG_RESOURCE_COUNT;
         resourceSlot_n++) {
        switch (resourceType_l[resourceSlot_n]) {
        case NORMAL_DIALOG_ARTIFACT:
            sizingIconHeight_l = 76;
            break;
        case NORMAL_DIALOG_EXPMRL_FIRST:
            sizingIconHeight_l = 28;
            break;
        case NORMAL_DIALOG_EXPMRL_FIRST + 1:
            sizingIconHeight_l = 57;
            break;
        case NORMAL_DIALOG_EXPMRL_FIRST + 2:
            sizingIconHeight_l = 62;
            break;
        case NORMAL_DIALOG_EXPMRL_FIRST + 3:
            sizingIconHeight_l = 59;
            break;
        case NORMAL_DIALOG_EXPMRL_LAST:
            sizingIconHeight_l =
                ((resourceValue_l[resourceSlot_n] == NORMAL_DIALOG_NO_VALUE) - 1 &
                 12) + 64;
            break;
        case NORMAL_DIALOG_CREST:
            sizingIconHeight_l = 55;
            break;
        case NORMAL_DIALOG_HERO:
            sizingIconHeight_l = 111;
            break;
        case RES_GOLD:
            sizingIconHeight_l = 26;
            break;
        case RES_WOOD:
        case RES_MERCURY:
        case RES_ORE:
        case RES_SULFUR:
        case RES_CRYSTAL:
        case RES_GEMS:
            sizingIconHeight_l = 44;
            break;
        case NORMAL_DIALOG_SPELL:
            sizingIconHeight_l = 79;
            break;
        case NORMAL_DIALOG_SECONDARY_SKILL:
            sizingIconHeight_l = 81;
            break;
        case NORMAL_DIALOG_MONSTER:
        case NORMAL_DIALOG_PRIMARY_SKILL:
            sizingIconHeight_l = 105;
            break;
        default:
            sizingIconHeight_l = 0;
            break;
        }
        if (maxIconHeight_a < sizingIconHeight_l)
            maxIconHeight_a = sizingIconHeight_l;
    }

    if (maxIconHeight_a)
        dialogContentHeight_h += maxIconHeight_a + 14;
    windowRows_j = (dialogContentHeight_h - 25) / NORMAL_DIALOG_WINDOW_ROW_HEIGHT;
    if (windowRows_j > NORMAL_DIALOG_MAX_ROWS)
        windowRows_j = NORMAL_DIALOG_MAX_ROWS;
    windowWidth_a = NORMAL_DIALOG_WINDOW_WIDTH;
    windowHeight_k = windowRows_j * NORMAL_DIALOG_WINDOW_ROW_HEIGHT +
                   NORMAL_DIALOG_WINDOW_BASE_HEIGHT;

    if (windowX == -1 || windowX + windowWidth_a > 638)
        windowX = 159;
    if (windowY == -1 || windowY + windowHeight_k > 478) {
        windowY = (480 - windowHeight_k) / 2;
        if (windowY > 28)
            windowY = 28;
    }

    sprintf(iconFile_h, "evntwin%d.bin", windowRows_j);
    pNormalDialogWindow = new heroWindow(windowX, windowY, iconFile_h);
    if (!pNormalDialogWindow)
        MemError();

    message_e.type = NORMAL_DIALOG_DISABLE_MESSAGE;
    message_e.payload.widget.command = NORMAL_DIALOG_DISABLE_COMMAND;
    message_e.payload.widget.data.text = reinterpret_cast<char *>(NORMAL_DIALOG_DISABLE_COMMAND);
    if (dialogType != NORMAL_DIALOG_DISABLE_SEVENTH &&
        dialogType != NORMAL_DIALOG_DISABLE_EIGHTH) {
        message_e.payload.widget.id = NORMAL_DIALOG_BUTTON_SEVEN;
        pNormalDialogWindow->BroadcastMessage(message_e);
    }
    if (dialogType != NORMAL_DIALOG_DISABLE_SEVENTH) {
        message_e.payload.widget.id = NORMAL_DIALOG_BUTTON_EIGHT;
        pNormalDialogWindow->BroadcastMessage(message_e);
    }
    if (dialogType != NORMAL_DIALOG_WAIT_LAST &&
        dialogType != NORMAL_DIALOG_BUTTON_PAIR) {
        message_e.payload.widget.id = NORMAL_DIALOG_BUTTON_ONE;
        pNormalDialogWindow->BroadcastMessage(message_e);
    }
    if (dialogType != NORMAL_DIALOG_WAIT_FIRST &&
        dialogType != NORMAL_DIALOG_INFO &&
        dialogType != NORMAL_DIALOG_BUTTON_PAIR) {
        message_e.payload.widget.id = NORMAL_DIALOG_BUTTON_TWO;
        pNormalDialogWindow->BroadcastMessage(message_e);
    }
    if (dialogType != NORMAL_DIALOG_CONFIRM) {
        message_e.payload.widget.id = NORMAL_DIALOG_BUTTON_FIVE;
        pNormalDialogWindow->BroadcastMessage(message_e);
        message_e.payload.widget.id = NORMAL_DIALOG_BUTTON_SIX;
        pNormalDialogWindow->BroadcastMessage(message_e);
    }

    for (resourceSlot_n = 0; resourceSlot_n < NORMAL_DIALOG_RESOURCE_COUNT;
         resourceSlot_n++) {
        iconPanel_j = 0;
        textPanel_h = 0;
        if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_NO_RESOURCE)
            break;

        resourceText_e[resourceSlot_n] = static_cast<char *>(BaseAlloc(
            NORMAL_DIALOG_TEXT_LENGTH, KBFILE, NORMAL_DIALOG_FIRST_TEXT_LINE));
        if (resourceType_l[resourceSlot_n] >= NORMAL_DIALOG_RESOURCE_FIRST &&
            resourceType_l[resourceSlot_n] <= NORMAL_DIALOG_RESOURCE_LAST) {
            if (resourceValue_l[resourceSlot_n] < 1) {
                if (resourceValue_l[resourceSlot_n] == 0) {
                    strcpy(resourceText_e[resourceSlot_n], "");
                } else if (resourceValue_l[resourceSlot_n] <
                           -NORMAL_DIALOG_DAILY_RESOURCE_OFFSET) {
                    sprintf(resourceText_e[resourceSlot_n], "%d",
                            resourceValue_l[resourceSlot_n] +
                                NORMAL_DIALOG_DAILY_RESOURCE_OFFSET);
                } else {
                    sprintf(resourceText_e[resourceSlot_n], "%d/day",
                            -resourceValue_l[resourceSlot_n]);
                }
            } else {
                sprintf(resourceText_e[resourceSlot_n], "%d",
                        resourceValue_l[resourceSlot_n]);
            }
            strcpy(iconFile_h, "resource.icn");
            resourceFrame_g = resourceType_l[resourceSlot_n];
        } else if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_SPELL) {
            sprintf(resourceText_e[resourceSlot_n], "%s",
                    gSpellNames[resourceValue_l[resourceSlot_n]]);
            strcpy(iconFile_h, "spells.icn");
            resourceFrame_g = gsSpellInfo[resourceValue_l[resourceSlot_n]].iconIndex;
        } else if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_CREST) {
            sprintf(resourceText_e[resourceSlot_n], "%s", "");
            strcpy(iconFile_h, "brcrest.icn");
            resourceFrame_g = resourceValue_l[resourceSlot_n];
        } else if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_PRIMARY_SKILL) {
            sprintf(resourceText_e[resourceSlot_n], "%s", "");
            strcpy(iconFile_h, "primskil.icn");
            resourceFrame_g = 4;
        } else if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_MONSTER) {
            sprintf(resourceText_e[resourceSlot_n], "%s", "");
            strcpy(iconFile_h, "strip.icn");
            resourceFrame_g = 12;
        } else if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_SECONDARY_SKILL) {
            sprintf(resourceText_e[resourceSlot_n], "%s",
                    gSecondarySkills[resourceValue_l[resourceSlot_n] /
                                     SECONDARY_SKILL_VALUE_LEVEL_COUNT]);
            strcpy(iconFile_h, "secskill.icn");
            resourceFrame_g = resourceValue_l[resourceSlot_n] /
                                SECONDARY_SKILL_VALUE_LEVEL_COUNT +
                            1;
        } else if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_HERO) {
            sprintf(resourceText_e[resourceSlot_n], "%s", "");
            sprintf(iconFile_h, "surrendr.icn");
            resourceFrame_g = 4;
        } else if (resourceType_l[resourceSlot_n] >= NORMAL_DIALOG_EXPMRL_FIRST &&
                   resourceType_l[resourceSlot_n] <= NORMAL_DIALOG_EXPMRL_LAST) {
            strcpy(resourceText_e[resourceSlot_n], "");
            strcpy(iconFile_h, "expmrl.icn");
            resourceFrame_g = resourceType_l[resourceSlot_n] - NORMAL_DIALOG_EXPMRL_FIRST;
            if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_EXPMRL_LAST &&
                resourceValue_l[resourceSlot_n] != NORMAL_DIALOG_NO_VALUE) {
                sprintf(resourceText_e[resourceSlot_n], "%d",
                        resourceValue_l[resourceSlot_n]);
            }
        } else {
            strcpy(resourceText_e[resourceSlot_n], "");
            strcpy(iconFile_h, "resource.icn");
            resourceFrame_g = resourceType_l[resourceSlot_n];
        }

        switch (resourceType_l[resourceSlot_n]) {
        case RES_WOOD:
        case RES_MERCURY:
        case RES_ORE:
        case RES_SULFUR:
        case RES_CRYSTAL:
        case RES_GEMS:
            resourceImageHeight_g = 38;
            sizingIconHeight_l = 32;
            break;
        case RES_GOLD:
            resourceImageHeight_g = 76;
            sizingIconHeight_l = 26;
            break;
        case NORMAL_DIALOG_ARTIFACT:
            resourceImageHeight_g = 76;
            sizingIconHeight_l = 76;
            break;
        case NORMAL_DIALOG_SPELL:
            resourceImageHeight_g = 70;
            sizingIconHeight_l = 55;
            break;
        case NORMAL_DIALOG_CREST:
            resourceImageHeight_g = 50;
            sizingIconHeight_l = 55;
            break;
        case NORMAL_DIALOG_EXPMRL_FIRST:
            resourceImageHeight_g = 64;
            sizingIconHeight_l = 28;
            break;
        case NORMAL_DIALOG_EXPMRL_FIRST + 1:
            resourceImageHeight_g = 64;
            sizingIconHeight_l = 57;
            break;
        case NORMAL_DIALOG_EXPMRL_FIRST + 2:
            resourceImageHeight_g = 64;
            sizingIconHeight_l = 62;
            break;
        case NORMAL_DIALOG_EXPMRL_FIRST + 3:
            resourceImageHeight_g = 64;
            sizingIconHeight_l = 59;
            break;
        case NORMAL_DIALOG_EXPMRL_LAST:
            resourceImageHeight_g = 64;
            sizingIconHeight_l = 64;
            break;
        case NORMAL_DIALOG_HERO:
            resourceImageHeight_g = 111;
            sizingIconHeight_l = 105;
            break;
        case NORMAL_DIALOG_SECONDARY_SKILL:
            resourceImageHeight_g = 75;
            sizingIconHeight_l = 65;
            break;
        case NORMAL_DIALOG_MONSTER:
        case NORMAL_DIALOG_PRIMARY_SKILL:
            resourceImageHeight_g = 94;
            sizingIconHeight_l = 105;
            break;
        }

        iconHeight_d = sizingIconHeight_l;
        if (strlen(resourceText_e[resourceSlot_n]))
            iconHeight_d += 12;

        if (resourceSlot_n == 0) {
            if (secondResourceType == NORMAL_DIALOG_NO_RESOURCE)
                resourceCenterX_a = (windowWidth_a - 17) / 2 + 17;
            else
                resourceCenterX_a = 104;
        } else {
            resourceCenterX_a = windowWidth_a - 87;
        }
        resourceY_l = windowHeight_k - iconHeight_d - 48;
        if (dialogType != NORMAL_DIALOG_QUICK_VIEW)
            resourceY_l = windowHeight_k - iconHeight_d - 87;
        if (resourceType_l[0] == NORMAL_DIALOG_SECONDARY_SKILL &&
            secondResourceType == NORMAL_DIALOG_SECONDARY_SKILL) {
            if (resourceSlot_n == 0)
                resourceCenterX_a -= 4;
            else
                resourceCenterX_a += 4;
        }

        iconPanel_j = new iconWidget(
            resourceCenterX_a - resourceImageHeight_g / 2 +
                (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_SPELL) * 2,
            resourceY_l, resourceImageHeight_g, sizingIconHeight_l, iconFile_h, resourceFrame_g,
            0, -1, NORMAL_DIALOG_WIDGET_COLOR +
                        (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_SPELL),
            1);
        if (!iconPanel_j)
            MemError();
        pNormalDialogWindow->AddWidget(iconPanel_j, -1);

        if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_ARTIFACT) {
            iconPanel_j = new iconWidget(
                resourceCenterX_a - resourceImageHeight_g / 2 + 6,
                resourceY_l + 6, 76, 76, "artifact.icn",
                resourceValue_l[resourceSlot_n] + 1, 0, -1,
                NORMAL_DIALOG_WIDGET_COLOR, 1);
            if (!iconPanel_j)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_j, -1);
        }
        if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_PRIMARY_SKILL) {
            iconPanel_j = new iconWidget(
                resourceCenterX_a - resourceImageHeight_g / 2 + 6,
                resourceY_l + 6, 82, 93, "primskil.icn",
                resourceValue_l[resourceSlot_n], 0, -1,
                NORMAL_DIALOG_WIDGET_COLOR, 1);
            if (!iconPanel_j)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_j, -1);
            strcpy(resourceText_e[resourceSlot_n],
                   gStatNames[resourceValue_l[resourceSlot_n]]);
        }
        if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_MONSTER) {
            iconPanel_j = new iconWidget(
                resourceCenterX_a - resourceImageHeight_g / 2 + 6,
                resourceY_l + 6, 82, 93, "strip.icn",
                gMonsterDatabase[resourceValue_l[resourceSlot_n]].race + 4,
                0, -1, NORMAL_DIALOG_WIDGET_COLOR, 1);
            if (!iconPanel_j)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_j, -1);

            sprintf(gText, "monh%04d.icn", resourceValue_l[resourceSlot_n]);
            iconPanel_j = new iconWidget(
                resourceCenterX_a - resourceImageHeight_g / 2 + 6,
                resourceY_l + 6, 82, 93, gText, 0, 0, -1,
                NORMAL_DIALOG_WIDGET_COLOR, 1);
            if (!iconPanel_j)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_j, -1);
        }
        if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_CREST) {
            iconPanel_j = new iconWidget(
                resourceCenterX_a - resourceImageHeight_g / 2 - 4,
                resourceY_l - 4, 58, 55, "brcrest.icn", 6, 0, -1,
                NORMAL_DIALOG_WIDGET_COLOR, 1);
            if (!iconPanel_j)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_j, -1);
        }
        if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_SECONDARY_SKILL) {
            iconPanel_j = new iconWidget(
                resourceCenterX_a - resourceImageHeight_g / 2 - 3,
                resourceY_l - 3, 71, 81, "secskill.icn", 15, 0, -1,
                NORMAL_DIALOG_WIDGET_COLOR, 1);
            if (!iconPanel_j)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_j, -1);
        }
        if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_HERO) {
            sprintf(iconFile_h, "port%04d.icn", resourceValue_l[resourceSlot_n]);
            iconPanel_j = new iconWidget(
                resourceCenterX_a - resourceImageHeight_g / 2 + 5,
                resourceY_l + 5, 101, 95, iconFile_h, 0, 0, -1,
                NORMAL_DIALOG_WIDGET_COLOR, 1);
            if (!iconPanel_j)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_j, -1);
        }

        short panelY;
        panelHeight_p = iconHeight_d;
        panelY = resourceY_l;
        if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_SECONDARY_SKILL) {
            textPanel_h = new textWidget(
                resourceCenterX_a - 50, panelHeight_p + panelY - 72, 100,
                (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_SPELL) * 12 + 12,
                resourceText_e[resourceSlot_n], "smalfont.fnt", 1,
                textWidgetId_h++, NORMAL_DIALOG_WIDGET_FLAGS, 1);
            if (!textPanel_h)
                MemError();
            pNormalDialogWindow->AddWidget(textPanel_h, -1);

            resourceText_e[resourceSlot_n] = static_cast<char *>(BaseAlloc(
                NORMAL_DIALOG_TEXT_LENGTH, KBFILE,
                NORMAL_DIALOG_SECONDARY_TEXT_LINE));
            labelY_o = static_cast<short>(iconHeight_d) +
                     static_cast<short>(resourceY_l) - 24;
            sprintf(resourceText_e[resourceSlot_n], "%s",
                    gSecondarySkillLevels[
                        resourceValue_l[resourceSlot_n] %
                        SECONDARY_SKILL_VALUE_LEVEL_COUNT]);
        } else if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_PRIMARY_SKILL) {
            labelY_o = panelHeight_p + panelY - 93;
        } else {
            labelY_o = panelHeight_p + panelY - 10;
        }

        textPanel_h = new textWidget(
            resourceCenterX_a - 50, labelY_o, 100,
            (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_SPELL) * 12 + 12,
            resourceText_e[resourceSlot_n], "smalfont.fnt", 1,
            textWidgetId_h++, NORMAL_DIALOG_WIDGET_FLAGS, 1);
        if (!textPanel_h)
            MemError();
        pNormalDialogWindow->AddWidget(textPanel_h, -1);

        if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_PRIMARY_SKILL &&
            showPrimaryBonus_e) {
            char *bonusText = static_cast<char *>(BaseAlloc(
                5, KBFILE, NORMAL_DIALOG_PRIMARY_BONUS_LINE));
            strcpy(bonusText, "+1 ");
            textPanel_h = new textWidget(
                resourceCenterX_a - 50, iconHeight_d + resourceY_l - 22, 100, 16,
                bonusText, "bigfont.fnt", 1, textWidgetId_h++,
                NORMAL_DIALOG_WIDGET_FLAGS, 1);
            if (!textPanel_h)
                MemError();
            pNormalDialogWindow->AddWidget(textPanel_h, -1);
        }

        borderWidget_o = new border(
            resourceCenterX_a - resourceImageHeight_g / 2, resourceY_l,
            resourceImageHeight_g, iconHeight_d,
            resourceSlot_n + NORMAL_DIALOG_RESOURCE_BORDER_FIRST_ID,
            1, 0, 0);
        pNormalDialogWindow->AddWidget(borderWidget_o, -1);
    }

    message_e.type = NORMAL_DIALOG_DISABLE_MESSAGE;
    message_e.payload.widget.command = NORMAL_DIALOG_SET_TEXT_COMMAND;
    message_e.payload.widget.id = NORMAL_DIALOG_TEXT_WIDGET_ID;
    message_e.payload.widget.data.text = text;
    pNormalDialogWindow->BroadcastMessage(message_e);

    if (showOrText == NORMAL_DIALOG_SHOW_OR_TEXT) {
        orText_f = static_cast<char *>(BaseAlloc(
            3, KBFILE, NORMAL_DIALOG_OR_TEXT_LINE));
        strcpy(orText_f, "or");
        textPanel_h = new textWidget(
            windowWidth_a / 2 - 10, resourceY_l + 43, 40, 12,
            orText_f, "smalfont.fnt", 1, textWidgetId_h++,
            NORMAL_DIALOG_WIDGET_FLAGS, 1);
        if (!textPanel_h)
            MemError();
        pNormalDialogWindow->AddWidget(textPanel_h, -1);
    }

    savedPointerType_e = gpMouseManager->m_cursorType;
    savedPointerFrame_j = gpMouseManager->m_cursorFrame;
    while (gpMouseManager->m_hideCount)
        gpMouseManager->ShowColorPointer();
    gpMouseManager->SetPointer("advmice.mse", 0, NORMAL_DIALOG_POINTER_ID);

    if (dialogType == NORMAL_DIALOG_WAIT_LAST ||
        dialogType == NORMAL_DIALOG_WAIT_FIRST) {
        gpWindowManager->DoDialog(pNormalDialogWindow, WaitHandler, 0);
    } else if (dialogType == NORMAL_DIALOG_QUICK_VIEW) {
        gpWindowManager->AddWindow(pNormalDialogWindow, -1, 1);
        QuickViewWait();
        gpWindowManager->RemoveWindow(pNormalDialogWindow);
    } else {
        gpWindowManager->DoDialog(pNormalDialogWindow, EventWindowHandler, 0);
    }

    delete pNormalDialogWindow;
    gpMouseManager->SetPointer("", savedPointerFrame_j, savedPointerType_e);
    giResType1 = savedFirstResourceType_p;
    giResExtra1 = savedFirstResourceValue_i;
    giResType2 = savedSecondResourceType_f;
    giResExtra2 = savedSecondResourceValue_j;
    pNormalDialogWindow = savedNormalDialogWindow_o;
}

VA(0x004a2565, 0x71)
void UpdateNormalDialog(char *text)
{
    short show = 1;
    tag_message evt;
    evt.type = 0x200;
    evt.payload.widget.command = 3;
    evt.payload.widget.id = 1;
    evt.payload.widget.data.text = text;
    pNormalDialogWindow->BroadcastMessage(evt);
    pNormalDialogWindow->DrawWindow(0, 0, 0x9000);
    pNormalDialogWindow->DrawWindow(1, -65535, -256);
}

// ---- globals (definitions, RVA order) ----
DATA(0x004f8c58) unsigned char giGroundToTerrain[432];
DATA(0x004f8e08) unsigned char giGroundShape[432];
DATA(0x004f8fb8) unsigned char gColorTableTan[256];
DATA(0x004f90b8) unsigned char gColorTableGray[256];
DATA(0x004f91b8) unsigned char gColorTableYellow[256];
DATA(0x004f92b8) unsigned char gColorTableScenWin[256];
DATA(0x004f93b8) unsigned char gColorTableDarkGray[256];
DATA(0x004f94b8) unsigned char gColorTableRed[256];
DATA(0x004f95b8) unsigned char gColorTableDarkBrown[256];
DATA(0x004f96b8) int MAP_WIDTH = 72;
DATA(0x004f96bc) int MAP_HEIGHT = 72;
DATA(0x004f96c0) unsigned char *mapExtra = 0;
DATA(0x004f96c4) int gbClosingApp = 0;
DATA(0x004f96c8) int gbForegroundApp = 0;
DATA(0x004f96cc) int giMainVideoModeColorDepth = 8;
DATA(0x004f96d0) int giMainVideoModeWidth = 640;
DATA(0x004f96d4) int giMainVideoModeHeight = 480;
DATA(0x004f96d8) unsigned long glMilliCounter = 0;
DATA(0x004f96e0) unsigned char gMapColors[16];
DATA(0x004f96f0) unsigned char *gObjectColors;
DATA(0x004f9700) unsigned char gOwnerColors[8];
DATA(0x004f9708) char *gTilesetFiles[64];
DATA(0x004f9808) unsigned char bPuzzleDraw[64];
DATA(0x004f9848) unsigned char uDimPal[3][4][256];
DATA(0x004fa448) unsigned char gColorTableLighten[256];
DATA(0x004fa548) unsigned char *gColorTableNoCycle;
DATA(0x004fa648) font *smallFont = 0;
DATA(0x004fa64c) font *bigFont = 0;
DATA(0x004fa650) int gbReturnAfterComputeExtent = 0;
DATA(0x004fa654) int gbAllowTextEntryEscape = 1;
DATA(0x004fa658) int giCycleType = 0;
DATA(0x004fa65c) int giScreenScroll = 1;
DATA(0x004fa660) int giMenuCommand = -1;
DATA(0x004fa664) int gbSendMouseMoveMessages = 0;
DATA(0x004fa668) int gbColorMice = 1;
DATA(0x004fa670) unsigned long
    gTownEligibleBuildMask[TOWN_ELIGIBLE_BUILD_MASK_COUNT] = {
        TOWN_ELIGIBLE_BUILD_KNIGHT_MASK,
        TOWN_ELIGIBLE_BUILD_BARBARIAN_MASK,
        TOWN_ELIGIBLE_BUILD_SORCERESS_MASK,
        TOWN_ELIGIBLE_BUILD_WARLOCK_MASK,
        TOWN_ELIGIBLE_BUILD_WIZARD_MASK,
        TOWN_ELIGIBLE_BUILD_NECROMANCER_MASK
    };
DATA(0x004fa688) unsigned char giMapSizes[4];
DATA(0x004fa68c) int gbUseEvilInterface = 0;
DATA(0x004fa690) char *cEvilTranslate[37][2];
DATA(0x004fa7b8) char gcAnimPath[0x160];
DATA(0x004fa918) char gcGamePath[0x18];
DATA(0x004fa930) char gcMapPath[0x14];
DATA(0x004fa944) int gbPutzingWithMouseCtr = 0;
DATA(0x004fa948) int gbDontTryRedbook = 0;
DATA(0x004fa94c) int gbDontTryMIDI = 0;
DATA(0x004fa950) int gbDontTryDigital = 0;
DATA(0x004fa958) float gfCombatSpeedMod[3];
DATA(0x004fa964) icon *gShingleAnim = 0;
DATA(0x004fa968) int iNextShingleAnim = 0;
DATA(0x004fa96c) int giDialogTimeout = 0;
DATA(0x004fa970) int giNewMonsterCycleFrame = 0;
DATA(0x004fa974) int gbNoCDRom = 0;
DATA(0x004fa978) int gbLeaveNetBoxAlone = 0;
DATA(0x004fa97c) int gbDrawWindowBackground = 1;
DATA(0x004fa980) int gbCheatMenus = 0;
DATA(0x004fa984) int gbUseWaveout = 0;
DATA(0x004fa988) int gbShowAllMaps = 0;
DATA(0x004fa990) char *gCombatFxNames[32];
DATA(0x004faa10) short horseFrameFlip[16];
DATA(0x004faa30) short boatFrameFlip[16];
DATA(0x004faa50) signed char *gCastleResources;
DATA(0x004faa58) short *gCastleAmounts;
DATA(0x004faa60) int gHeroGoldCost = 2500;
DATA(0x004faa68) short *gVesaMode;
DATA(0x004faa78) tag_tilePoint normalDirTable[8];
DATA(0x004faa98) int gResourceBaseValue[8];
DATA(0x004faab8) int gInitResourcesHuman[5][7];
DATA(0x004fab48) int gInitResourcesComputer[5][7];
DATA(0x004fabd8) int gMineCharacteristics[8];
DATA(0x004fabf8) int gSSValues[HERO_SKILL_COUNT]
                                 [SECONDARY_SKILL_VALUE_LEVEL_COUNT];
DATA(0x004faca0) unsigned char gArtifactLevel[104];
DATA(0x004fad08) int gArtifactBaseRV[100];
DATA(0x004faea4) int gUltArtifactAvgValue = 16500;
DATA(0x004faea8) int giDebugLevel = 0;
DATA(0x004faeac) signed char giVisRangeTown = 5;
DATA(0x004faeb0) tag_monsterInfo gMonsterDatabase[MONSTER_DATABASE_COUNT];
DATA(0x004fb568) float gfStatPower[42];
DATA(0x004fb610) float gfBattleStat[42];
DATA(0x004fb6b8) signed char gSpellLimits[5];
DATA(0x004fb6c0) float gfSpellCastableCombatMod[12];
DATA(0x004fb6f0) float gfSpellCastNumMod[12];
DATA(0x004fb720) float gfPhilAISpellPowerMod[12];
DATA(0x004fb750) float gfPhilAIDurationMod[12];
DATA(0x004fb780) float gfSpellTypeNumMod[7];
DATA(0x004fb79c) int gbDrawSavedCursor = 0;
DATA(0x004fb7a0) signed char gbArrow[8][8];
DATA(0x004fb7e0) unsigned char giCloudType[256];
DATA(0x004fb8e0) short giScoreMon[66][2];
DATA(0x004fb9e8) short giScoreCampaignMon[66][2];
DATA(0x004fbaf0) signed char townTheme[TOWN_MUSIC_TABLE_SIZE] = {
    TOWN_MUSIC_KNIGHT,
    TOWN_MUSIC_BARBARIAN,
    TOWN_MUSIC_WARLOCK,
    TOWN_MUSIC_WIZARD,
    TOWN_MUSIC_SORCERESS,
    TOWN_MUSIC_NECROMANCER,
    TOWN_MUSIC_NONE,
    TOWN_MUSIC_NONE
};
DATA(0x004fbaf8) signed char gHeroSkillBonus[6][2][4];
DATA(0x004fbb28) int gbLoadingMonoIcon = 0;
DATA(0x004fbb2c) int giMonoIconSkip = -1;
DATA(0x004fbb30) int giScrollX = 0;
DATA(0x004fbb34) int giScrollY = 0;
DATA(0x004fbb38) int gbNoBorder = 0;
DATA(0x004fbb3c) int gbEnlargeScreenBlit = 1;
DATA(0x004fbb40) int giCurExe = 0;
DATA(0x004fbb44) int gbInDialog = 0;
// APP_MENU_UNKNOWN_9C6D and APP_MENU_UNKNOWN_9CAD occur only in this retail
// table; executable and menu-resource evidence does not identify their actions.
DATA(0x004fbb48) struct SMenuEnableStatus
    gsMenuEnableStatus[MENU_ENABLE_STATUS_COUNT] = {
        { APP_MENU_NONE, 0, 0, 0 },
        { KBWIN_MENU_SIZE_640_480, 1, 1, 0 },
        { KBWIN_MENU_SIZE_800_600, 1, 1, 0 },
        { KBWIN_MENU_SIZE_1024_768, 1, 1, 0 },
        { KBWIN_MENU_SIZE_1280_1024, 1, 1, 0 },
        { KBWIN_MENU_FULLSCREEN, 1, 1, 0 },
        { APP_MENU_VIEW_WORLD, 0, 0, 0 },
        { APP_MENU_VIEW_PUZZLE, 0, 0, 0 },
        { APP_MENU_CAST_SPELL, 0, 0, 0 },
        { APP_MENU_SEARCH, 0, 0, 0 },
        { APP_MENU_MUSIC_FIRST, 1, 0, 0 },
        { APP_MENU_MUSIC_FIRST + 1, 1, 0, 0 },
        { APP_MENU_MUSIC_FIRST + 2, 1, 0, 0 },
        { APP_MENU_MUSIC_FIRST + 3, 1, 0, 0 },
        { APP_MENU_MUSIC_FIRST + 4, 1, 0, 0 },
        { APP_MENU_MUSIC_FIRST + 5, 1, 0, 0 },
        { APP_MENU_MUSIC_FIRST + 6, 1, 0, 0 },
        { APP_MENU_MUSIC_FIRST + 7, 1, 0, 0 },
        { APP_MENU_MUSIC_FIRST + 8, 1, 0, 0 },
        { APP_MENU_MUSIC_FIRST + 9, 1, 0, 0 },
        { APP_MENU_MUSIC_LAST, 1, 0, 0 },
        { APP_MENU_SOUND_FIRST, 1, 0, 0 },
        { APP_MENU_SOUND_FIRST + 1, 1, 0, 0 },
        { APP_MENU_SOUND_FIRST + 2, 1, 0, 0 },
        { APP_MENU_SOUND_FIRST + 3, 1, 0, 0 },
        { APP_MENU_SOUND_FIRST + 4, 1, 0, 0 },
        { APP_MENU_SOUND_FIRST + 5, 1, 0, 0 },
        { APP_MENU_SOUND_FIRST + 6, 1, 0, 0 },
        { APP_MENU_SOUND_FIRST + 7, 1, 0, 0 },
        { APP_MENU_SOUND_FIRST + 8, 1, 0, 0 },
        { APP_MENU_SOUND_FIRST + 9, 1, 0, 0 },
        { APP_MENU_SOUND_LAST, 1, 0, 0 },
        { APP_MENU_SPEED_FIRST, 0, 0, 0 },
        { APP_MENU_SPEED_FIRST + 1, 0, 0, 0 },
        { APP_MENU_SPEED_FIRST + 2, 0, 0, 0 },
        { APP_MENU_SPEED_FIRST + 3, 0, 0, 0 },
        { APP_MENU_SPEED_LAST, 0, 0, 0 },
        { APP_MENU_UNKNOWN_9C6D, 0, 0, 0 },
        { APP_MENU_TOGGLE_ROUTE, 0, 0, 0 },
        { APP_MENU_TOGGLE_BLACKOUT, 0, 0, 0 },
        { KBWIN_MENU_HELP, 1, 1, 0 },
        { KBWIN_MENU_ABOUT, 1, 1, 0 },
        { APP_MENU_RESTART_0, 0, 1, 0 },
        { APP_MENU_RESTART_1, 0, 1, 0 },
        { APP_MENU_RESTART_2, 0, 1, 0 },
        { APP_MENU_RESTART_3, 0, 1, 0 },
        { APP_MENU_RESTART_4, 0, 1, 0 },
        { APP_MENU_UNKNOWN_9CAD, 0, 1, 0 },
        { APP_MENU_RESTART_5, 0, 1, 0 },
        { APP_MENU_RESTART_6, 0, 1, 0 },
        { APP_MENU_RESTART_7, 0, 1, 0 },
        { APP_MENU_RESTART_8, 0, 1, 0 },
        { APP_MENU_RESTART_9, 0, 1, 0 },
        { APP_MENU_RESTART_10, 0, 1, 0 },
        { APP_MENU_RESTART_11, 0, 1, 0 },
        { APP_MENU_RESTART_12, 0, 1, 0 },
        { APP_MENU_RESTART_13, 0, 1, 0 },
        { APP_MENU_LOAD_0, 0, 1, 0 },
        { APP_MENU_LOAD_1, 0, 1, 0 },
        { APP_MENU_LOAD_2, 0, 1, 0 },
        { APP_MENU_LOAD_3, 0, 1, 0 },
        { APP_MENU_LOAD_4, 0, 1, 0 },
        { APP_MENU_LOAD_5, 0, 1, 0 },
        { APP_MENU_LOAD_6, 0, 1, 0 },
        { APP_MENU_LOAD_7, 0, 1, 0 },
        { APP_MENU_LOAD_8, 0, 1, 0 },
        { APP_MENU_LOAD_9, 0, 1, 0 },
        { APP_MENU_LOAD_10, 0, 1, 0 },
        { APP_MENU_SAVE, 0, 0, 0 },
        { APP_MENU_EXIT, 0, 0, 0 }
    };
DATA(0x004fbd34) int gbInSetupDialog = 0;
DATA(0x004fbd38) int gbMinimized = 0;
DATA(0x004fbd3c) int gbHeroMoving = 0;
DATA(0x004fbd40) int gbInSmackMgr = 0;
DATA(0x004fbd44) int glBottomRefresh = 0;
DATA(0x004fbd48) int gbBothMachinesWin95 = 0;
DATA(0x004fbd4c) int gbGotFirstHeartbeat = 0;
DATA(0x004fbd50) void *hmnuDflt = 0;
DATA(0x004fbd54) void *hmnuCmbt = 0;
DATA(0x004fbd58) void *hmnuAdv = 0;
DATA(0x004fbd5c) void *hmnuTown = 0;
DATA(0x004fbd60) char *cMonFilename[66];
DATA(0x004fbe68) int gbProcessingCombatAction = 0;
DATA(0x004fbe6c) int iMPNetProtocol = 0;
DATA(0x004fbe70) int iLastDiffSendTo = -2;
DATA(0x004fbe78) SSpellInfo gsSpellInfo[65];
DATA(0x004fc410) char *cArmyFrameFileNames[66];
DATA(0x004fc518) unsigned char *gcSpellInfluenceIcons;
DATA(0x004fc528) unsigned char giSpellInfluenceToSpell[16];
DATA(0x004fc538) unsigned char giNumPowFrames[32];
DATA(0x004fc558) int giSpellEffectShowType = 2;
DATA(0x004fc560) signed char gcColorToPlayerPos[8];
DATA(0x004fc568) char *cCombatBkgNames[20];
DATA(0x004fc5b8) struct SCmbtObstacle sCmbtObstacles[32];
DATA(0x004fc778) int gEstatesGoldLevel[4];
DATA(0x004fc788) float gfSSLogisticsMod[HERO_SKILL_LEVEL_COUNT];
DATA(0x004fc798) float gfSSNavigationMod[HERO_SKILL_LEVEL_COUNT];
DATA(0x004fc7a8) float gfSSArcheryMod[HERO_SKILL_LEVEL_COUNT];
DATA(0x004fc7b8) float gfSSAIArcheryMod[4];
DATA(0x004fc7c8) signed char giVisRange[8];
DATA(0x004fc7d0) unsigned char gStartingHeroStats[6][5];
DATA(0x004fc7f0) int giTerrainCost[10][4][2];
DATA(0x004fc930) unsigned char bStopOnTrigger[128];
DATA(0x004fc9b0) char *gTownPrefixNames[6];
DATA(0x004fc9c8) char *gTownObjNames[32];
DATA(0x004fca48) signed char gDwellingType[TOWN_TYPE_COUNT][12] = {
    { ARMY_CREATURE_PEASANT, ARMY_CREATURE_ARCHER, ARMY_CREATURE_PIKEMAN,
      ARMY_CREATURE_SWORDSMAN, ARMY_CREATURE_CAVALRY,
      ARMY_CREATURE_PALADIN, ARMY_CREATURE_RANGER,
      ARMY_CREATURE_VETERAN_PIKEMAN, ARMY_CREATURE_MASTER_SWORDSMAN,
      ARMY_CREATURE_CHAMPION, ARMY_CREATURE_CRUSADER, ARMY_GROUP_EMPTY_SLOT },
    { ARMY_CREATURE_GOBLIN, ARMY_CREATURE_ORC, ARMY_CREATURE_WOLF,
      ARMY_CREATURE_OGRE, ARMY_CREATURE_TROLL, ARMY_CREATURE_CYCLOPS,
      ARMY_CREATURE_ORC_CHIEF, ARMY_GROUP_EMPTY_SLOT, ARMY_CREATURE_OGRE_LORD,
      ARMY_CREATURE_WAR_TROLL, ARMY_GROUP_EMPTY_SLOT, ARMY_GROUP_EMPTY_SLOT },
    { ARMY_CREATURE_SPRITE, ARMY_CREATURE_DWARF, ARMY_CREATURE_ELF,
      ARMY_CREATURE_DRUID, ARMY_CREATURE_UNICORN, ARMY_CREATURE_PHOENIX,
      ARMY_CREATURE_BATTLE_DWARF, ARMY_CREATURE_GRAND_ELF,
      ARMY_CREATURE_GREATER_DRUID, ARMY_GROUP_EMPTY_SLOT,
      ARMY_GROUP_EMPTY_SLOT, ARMY_GROUP_EMPTY_SLOT },
    { ARMY_CREATURE_CENTAUR, ARMY_CREATURE_GARGOYLE, ARMY_CREATURE_GRIFFIN,
      ARMY_CREATURE_MINOTAUR, ARMY_CREATURE_HYDRA,
      ARMY_CREATURE_GREEN_DRAGON, ARMY_GROUP_EMPTY_SLOT,
      ARMY_GROUP_EMPTY_SLOT, ARMY_CREATURE_MINOTAUR_KING,
      ARMY_GROUP_EMPTY_SLOT, ARMY_CREATURE_RED_DRAGON,
      ARMY_CREATURE_BLACK_DRAGON },
    { ARMY_CREATURE_HALFLING, ARMY_CREATURE_BOAR, ARMY_CREATURE_IRON_GOLEM,
      ARMY_CREATURE_ROC, ARMY_CREATURE_MAGE, ARMY_CREATURE_GIANT,
      ARMY_GROUP_EMPTY_SLOT, ARMY_CREATURE_STEEL_GOLEM,
      ARMY_GROUP_EMPTY_SLOT, ARMY_CREATURE_ARCHMAGE, ARMY_CREATURE_TITAN,
      ARMY_GROUP_EMPTY_SLOT },
    { ARMY_CREATURE_SKELETON, ARMY_CREATURE_ZOMBIE, ARMY_CREATURE_MUMMY,
      ARMY_CREATURE_VAMPIRE, ARMY_CREATURE_LICH, ARMY_CREATURE_BONE_DRAGON,
      ARMY_CREATURE_MUTANT_ZOMBIE, ARMY_CREATURE_ROYAL_MUMMY,
      ARMY_CREATURE_VAMPIRE_LORD, ARMY_CREATURE_POWER_LICH,
      ARMY_GROUP_EMPTY_SLOT, ARMY_GROUP_EMPTY_SLOT }
};
DATA(0x004fca90) int gMageBuildingCosts[6][7];
DATA(0x004fcb38) int gSpecialBuildingCosts[6][7];
DATA(0x004fcbe0) int gNeutralBuildingCosts[16][7];
DATA(0x004fcda0) int gMageBaseResourceValues[6];
DATA(0x004fcdb8) int gNeutralBaseResourceValues[20];
DATA(0x004fce08) int gSpecialBuildingBaseResourceValues[6];
DATA(0x004fce20) int gDwellingBaseResourceValues[6][12];
DATA(0x004fcf40) int gDwellingCosts[6][12][7];
DATA(0x004fd720) unsigned long gHierarchyMask[6][12];
DATA(0x004fd840) int giDebugBuildingToBuild = -1;
DATA(0x004fd848) unsigned char giTerrainToMusicTrack[16];
DATA(0x004fd858) char *cHeroTypeShortName[6];
DATA(0x004fd870) char cHeroTypeInitial[HERO_TYPE_INITIAL_COUNT];
DATA(0x004fd878) int giDeferObjDrawX = -1;
DATA(0x004fd87c) int giDeferObjDrawY = -1;
DATA(0x004fd880) class heroWindow *gpInitWin = 0;
DATA(0x004fd888) unsigned char iGetSSByAlignment[HERO_SKILL_COUNT][6] = {
    {3, 4, 2, 2, 2, 3},
    {2, 3, 3, 1, 1, 1},
    {3, 3, 2, 2, 2, 2},
    {2, 4, 1, 4, 2, 1},
    {3, 2, 2, 2, 2, 2},
    {2, 3, 4, 2, 2, 2},
    {5, 3, 1, 1, 2, 0},
    {2, 1, 4, 5, 5, 4},
    {1, 1, 3, 3, 4, 3},
    {1, 2, 3, 1, 2, 1},
    {4, 3, 3, 3, 3, 3},
    {1, 1, 2, 3, 3, 3},
    {0, 0, 0, 1, 0, 7},
    {3, 2, 2, 2, 2, 2}
};
DATA(0x004fd8e0) struct SCmbtHero sCmbtHero[12];
DATA(0x004fdeec) unsigned char iWallToHexCell[4];
DATA(0x004fdef0) unsigned char iTowerToHexCell[4];
DATA(0x004fdef8) unsigned short wallPos[4][2];
DATA(0x004fdf08) unsigned short towerPos[4][2];
DATA(0x004fdf18) unsigned short doorPos[2][2];
DATA(0x004fdf20) float *fTradingPostEfficency = 0;
DATA(0x004fdf50) struct SElevationOverlay
    sElevationOverlay[ELEVATION_OVERLAY_COUNT];
DATA(0x004fe100) signed char captainStats[6][4];
DATA(0x004fe118) int gbDrawingPuzzle = 0;
DATA(0x004fe11c) int giWalkingFrom = -1;
DATA(0x004fe120) int giWalkingFrom2 = -1;
DATA(0x004fe124) int giWalkingTo = -1;
DATA(0x004fe128) int giWalkingTo2 = -1;
DATA(0x004fe12c) int giWalkingYMod = 0;
DATA(0x004fe130) unsigned char moatCell[16];
DATA(0x004fe140) SCampaignChoice campaignChoices[CAMPAIGN_SIDE_COUNT]
                                                   [CAMPAIGN_MAP_COUNT]
                                                   [CAMPAIGN_BONUS_CHOICE_COUNT] = {
    {
        { { CAMPAIGN_CHOICE_RESOURCE, CAMPAIGN_RESOURCE_GOLD, CAMPAIGN_CHOICE_GOLD_BONUS },
          { CAMPAIGN_CHOICE_ARTIFACT, CAMPAIGN_ARTIFACT_THUNDER_MACE, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ARTIFACT, CAMPAIGN_ARTIFACT_ARMORED_GAUNTLETS, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_WIZARD, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_SORCERESS, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_KNIGHT, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_WIZARD, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_SORCERESS, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_KNIGHT, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_WIZARD, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_SORCERESS, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_KNIGHT, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_WIZARD, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_SORCERESS, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_KNIGHT, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_SPELL, CAMPAIGN_SPELL_MIRROR_IMAGE, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_SPELL, CAMPAIGN_SPELL_SUMMON_EARTH_ELEMENTAL, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_SPELL, CAMPAIGN_SPELL_RESURRECT, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_ARTIFACT, CAMPAIGN_ARTIFACT_BLACK_PEARL, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ARTIFACT, CAMPAIGN_ARTIFACT_DRAGON_SWORD, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ARTIFACT, CAMPAIGN_ARTIFACT_DIVINE_BREASTPLATE, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_WIZARD, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_SORCERESS, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_KNIGHT, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_RESOURCE, CAMPAIGN_RESOURCE_CRYSTAL, CAMPAIGN_CHOICE_RESOURCE_BONUS },
          { CAMPAIGN_CHOICE_RESOURCE, CAMPAIGN_RESOURCE_GEMS, CAMPAIGN_CHOICE_RESOURCE_BONUS },
          { CAMPAIGN_CHOICE_RESOURCE, CAMPAIGN_RESOURCE_MERCURY, CAMPAIGN_CHOICE_RESOURCE_BONUS } },
        { { CAMPAIGN_CHOICE_ARTIFACT, CAMPAIGN_ARTIFACT_TAX_LIEN, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ARTIFACT, CAMPAIGN_ARTIFACT_HIDEOUS_MASK, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ARTIFACT, CAMPAIGN_ARTIFACT_FIZBIN_MISFORTUNE, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_NONE, CAMPAIGN_CHOICE_VALUE_NONE, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_NONE, CAMPAIGN_CHOICE_VALUE_NONE, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_NONE, CAMPAIGN_CHOICE_VALUE_NONE, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_WIZARD, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_SORCERESS, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_KNIGHT, CAMPAIGN_CHOICE_NO_AMOUNT } }
    },
    {
        { { CAMPAIGN_CHOICE_RESOURCE, CAMPAIGN_RESOURCE_GOLD, CAMPAIGN_CHOICE_GOLD_BONUS },
          { CAMPAIGN_CHOICE_ARTIFACT, CAMPAIGN_ARTIFACT_MAGE_RING, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ARTIFACT, CAMPAIGN_ARTIFACT_MINOR_SCROLL, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_NECROMANCER, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_WARLOCK, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_BARBARIAN, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_RESOURCE, CAMPAIGN_RESOURCE_GOLD, CAMPAIGN_CHOICE_GOLD_BONUS },
          { CAMPAIGN_CHOICE_SPELL, CAMPAIGN_SPELL_MASS_CURSE, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ARTIFACT, CAMPAIGN_ARTIFACT_DEFENDER_HELM, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_NECROMANCER, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_WARLOCK, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_BARBARIAN, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_NECROMANCER, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_WARLOCK, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_BARBARIAN, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_NECROMANCER, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_WARLOCK, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_BARBARIAN, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_NECROMANCER, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_WARLOCK, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_BARBARIAN, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_SECONDARY_SKILL, CAMPAIGN_SECONDARY_SKILL_LOGISTICS, CAMPAIGN_CHOICE_BASIC_SKILL },
          { CAMPAIGN_CHOICE_ARTIFACT, CAMPAIGN_ARTIFACT_POWER_AXE, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ARTIFACT, CAMPAIGN_ARTIFACT_WHITE_PEARL, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_NECROMANCER, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_WARLOCK, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_BARBARIAN, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_ARTIFACT, CAMPAIGN_ARTIFACT_BLACK_PEARL, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ARTIFACT, CAMPAIGN_ARTIFACT_DRAGON_SWORD, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ARTIFACT, CAMPAIGN_ARTIFACT_DIVINE_BREASTPLATE, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_ARTIFACT, CAMPAIGN_ARTIFACT_TAX_LIEN, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ARTIFACT, CAMPAIGN_ARTIFACT_HIDEOUS_MASK, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ARTIFACT, CAMPAIGN_ARTIFACT_FIZBIN_MISFORTUNE, CAMPAIGN_CHOICE_NO_AMOUNT } },
        { { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_NECROMANCER, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_WARLOCK, CAMPAIGN_CHOICE_NO_AMOUNT },
          { CAMPAIGN_CHOICE_ALIGNMENT, CAMPAIGN_ALIGNMENT_BARBARIAN, CAMPAIGN_CHOICE_NO_AMOUNT } }
    }
};
DATA(0x004fe2a8) char *congratsText = 0;
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
DATA(0x004ff2d8) char *cTownCommand[28] = {
    "Redistribute %s army",
    "Cannot combine hero's last army",
    "Combine %s armies",
    "Redistribute %s army",
    "View %s",
    "Cannot move last army to garrison.",
    "Move %s",
    "Exchange %s with %s",
    "Exit town",
    "",
    "Kingdom Overview",
    "Empty",
    "Select %s",
    "View Hero",
    "Mage Guild",
    "Thieves' Guild",
    "Tavern",
    "Dock",
    "Well",
    "Tent",
    "Castle",
    "Recruit %s",
    "Statue",
    "Left Turret",
    "Right Turret",
    "Moat",
    "Marketplace",
    "Captain's Quarters"
};
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
DATA(0x004ffa78) char *cSecSkillDesc[14][3];
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
DATA(0x004ffcf0) char *gArmySizeNames[9][3];
DATA(0x004ffd60) char *cRandomTavernText[8];
DATA(0x004ffd80) char *cRandomSignText[4];
DATA(0x004ffd90) char *cCampaignAwards[12];
DATA(0x004ffdc0) char *cCampaignName[CAMPAIGN_SIDE_COUNT][CAMPAIGN_MAP_COUNT] = {
    {
        "Force of Arms", "Annexation", "Save the Dwarves", "Carator Mines",
        "Turning Point", "Defender", "The Gauntlet", "The Crown",
        "Corlagon's Defense", "Final Justice", "", "Betrayal"
    },
    {
        "First Blood", "Barbarian Wars", "Necromancers", "Slay the Dwarves",
        "Turning Point", "Rebellion", "Dragon Master", "Country Lords",
        "The Crown", "Greater Glory", "Apocalypse", "Betrayal"
    }
};
DATA(0x004ffe20) char *cCampaignDescription[CAMPAIGN_SIDE_COUNT][CAMPAIGN_MAP_COUNT] = {
    {
        "Roland needs you to defeat the lords near his castle to begin his war of "
        "rebellion against his brother.  They are not allied with each other, so "
        "they will spend most of their time fighting with one another.  Victory is "
        "yours when you have defeated all of their castles and heroes.",
        "The local lords refuse to swear allegiance to Roland, and must be subdued.  "
        "They are wealthy and powerful, so be prepared for a tough fight.  Capture "
        "all enemy castles to win.",
        "Your task is to defend the Dwarves against Archibald's forces.  Capture all "
        "of the enemy towns and castles to win, and be sure not to lose all of the "
        "dwarf towns at once, or the enemy will have won.",
        "You will face four allied enemies in a straightforward fight for resources "
        "and treasure.  Capture all of the enemy castles for victory.",
        "Your enemies are allied against you and start close by, so be ready to come "
        "out fighting.  You will need to own all four castles in this small valley "
        "to win.",
        "The Sorceress' guild of Noraston has requested Roland's aid against an "
        "attack from Archibald's allies.  Capture all of the enemy castles to win, "
        "and don't lose Noraston, or you'll lose the scenario.  (Hint:  There is an "
        "enemy castle on an island in the ocean.)",
        "Gather as large an army as possible and capture the enemy castle within 8 "
        "weeks.  You are opposed by only one enemy, but must travel a long way to get "
        "to the enemy castle.  Any troops you have in your army at the end of this "
        "scenario will be with you in the final battle.",
        "Find the Crown before Archibald's heroes find it.  Roland will need the "
        "Crown for the final battle.",
        "Three allied enemies stand before you and victory, including Lord Corlagon.  "
        "Roland is in a castle to the northwest, and you will lose if he falls to the "
        "enemy.  Remember that capturing Lord Corlagon will ensure that he will not "
        "fight against you in the final scenario.",
        "This is the final battle.  Both you and your enemy are armed to the teeth, "
        "and all are allied against you.  Capture Archibald to end the war!",
        "",
        "Switching sides leaves you with three castles against the enemy's one.  This "
        "battle will be the easiest one you will face for the rest of the war...traitor."
    },
    {
        "King Archibald requires you to defeat the three enemies in this region.  "
        "They are not allied with one another, so they will spend most of their "
        "energy fighting amongst themselves.  You will win when you own all of the "
        "enemy castles and there are no more heroes left to fight.",
        "You must unify the barbarian tribes of the north by conquering them.  As in "
        "the previous mission, the enemy is not allied against you, but they have "
        "more resources at their disposal.  You will win when you own all of the "
        "enemy castles and there are no more heroes left to fight.",
        "Do-gooder wizards have taken the Necromancers' castle.  You must retake it "
        "to achieve victory.  Remember that while you start with a powerful army, "
        "you have no castle and must take one within 7 days, or lose this battle.  "
        "(Hint: The nearest castle is to the southeast.)",
        "The Dwarves need conquering before they can interfere in King Archibald's "
        "plans.  Roland's forces have more than one hero and many towns to start "
        "with, so be ready for attack from multiple directions.  You must capture "
        "all of the enemy towns and castles to claim victory.",
        "Your enemies are allied against you and start close by, so be ready to come "
        "out fighting.  You will need to own all four castles in this small valley "
        "to win.",
        "You must put down a peasant revolt led by Roland's forces.  All are allied "
        "against you, but you have Lord Corlagon, an experienced hero, to help you.  "
        "Capture all enemy castles to win.",
        "There are two enemies allied against you in this mission.  Both are well "
        "armed and seek to evict you from their island.  Avoid them and capture "
        "Dragon City to win.",
        "Your orders are to conquer the country lords that have sworn to serve "
        "Roland.  All of the enemy castles are unified against you.  Since you start "
        "without a castle, you must hurry to capture one before the end of the week.  "
        "Capture all enemy castles for victory.",
        "Find the Crown before Roland's heroes find it.  Archibald will need the "
        "Crown for the final battle with Roland.",
        "Gather as large an army as possible and capture the enemy castle within 8 "
        "weeks.  You are opposed by only one enemy, but must travel a long way to get "
        "to the enemy castle.  Any troops you have in your army at the end of this "
        "scenario will be with you in the next.",
        "This is the final battle.  Both you and your enemy are armed to the teeth, "
        "and all are allied against you.  Capture Roland to win the war, and be sure "
        "not to lose Archibald in the fight!",
        "Switching sides leaves you with three castles against the enemy's one.  This "
        "battle will be the easiest one you will face for the rest of the war...traitor."
    }
};
DATA(0x004ffe80) char *cOutOfMemory;
DATA(0x004ffe88) char *cSlowVideoLevelText[2];
DATA(0x004ffe90) char *gSPanelHelp[10];
DATA(0x004ffeb8) char *xBarrierColor[8];
DATA(0x004ffed8) char *xGenericSiteNames[8];
DATA(0x004ffef8) char *xRecruitmentSiteNames[6];
DATA(0x004fff10) SWinSetup gWinSetup[73];
DATA(0x00500110) int gbHeroWindShowing = 0;
DATA(0x00500114) int gbFullCombatScreenDrawn = 1;
DATA(0x00500118) int gbLimitedCombatUpdatePalette = 0;
DATA(0x0050011c) int gbFirstTimeThrough = 0;
DATA(0x00500120) int gbSkipIntro = 0;
DATA(0x00500124) int gbDoMemCheck = 1;
DATA(0x00500128) int gbAllBlack = 0;
DATA(0x0050012c) int gbInCombat = 0;
DATA(0x00500130) int gbDirectConnect = 0;
DATA(0x00500134) int giForceSwitchMusic = -1;
DATA(0x00500138) int gbComputeExtent = 0;
DATA(0x0050013c) int gbSaveBiggestExtent = 0;
DATA(0x00500140) int gbLimitToExtent = 0;
DATA(0x00500144) int gbCurrArmyDrawn = 1;
DATA(0x00500148) int gAdvDisposeLevel = 0;
DATA(0x0050014c) int gbRemoteOn = 0;
DATA(0x00500150) int gbGameInitialized = 0;
DATA(0x00500154) int giHighScoreRank = -1;
DATA(0x00500158) int giHighScoreType = 1;
DATA(0x0050015c) int gbShowHighScore = 0;
DATA(0x00500160) int gbLowMemory = 0;
DATA(0x00500164) int giHighMemBuffer = 5;
DATA(0x00500168) void *gLowPage = 0;
DATA(0x0050016c) int gbLowPageGrabbed = 0;
DATA(0x00500170) signed char xSmackFromNetwork = 0;
DATA(0x00500174) int gbInPollSound = 0;
DATA(0x005157a8) int iCDRomErr = 0;
DATA(0x005157ac) int bEarlySetupDone = 0;
DATA(0x005159f8) int bKBDone = 0;
DATA(0x005159fc) struct _REDBOOK *hRedbookz = 0;
DATA(0x00515a00) int bForceCheckTimeEvent = 0;
DATA(0x00515ca0) unsigned short IMHotSpots[5][4];
DATA(0x00515cc8) int lastIMHoverID = -1;
DATA(0x00515f78) int bInCheckEndGame = 0;
DATA(0x005165dc) int bInShutDown = 0;
DATA(0x00516810) int gbInMemError = 0;
DATA(0x00516d1c) int iShingleAnimFrame = 0;
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
DATA(0x00528cb8) char gFullMapName[GLOBAL_MAP_NAME_SIZE];
DATA(0x00528cd0) char gcTCPName[GLOBAL_TCP_TEXT_SIZE];
DATA(0x00528ce8) int giShowIntro;
DATA(0x00528cf0) int glTimers[GLOBAL_TIMER_COUNT];
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
DATA(0x005290f8) char gcTCPAddress[GLOBAL_TCP_TEXT_SIZE];
DATA(0x00529110) unsigned char giSetupGameType;
DATA(0x00529118) char gLastFilename[GLOBAL_AGGREGATE_PATH_SIZE];
DATA(0x00529278) int giFullySeeded;
DATA(0x0052927c) icon *gBuyBuildIcons;
DATA(0x00529280) int gbNoSound;
DATA(0x00529288) int iCombatControlNetPos[COMBAT_CONTROL_SIDE_COUNT];
DATA(0x00529290) char cExpAggPathName[GLOBAL_AGGREGATE_PATH_SIZE];
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
DATA(0x00529608) char cAggPathName[GLOBAL_AGGREGATE_PATH_SIZE];
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
DATA(0x005298c8) char gcCommandLine[GLOBAL_COMMAND_LINE_SIZE];
DATA(0x00529908) int giBottomViewResourceQty;
DATA(0x0052990c) soundManager *gpSoundManager;
DATA(0x00529910) int gbThisNetGotAdventureControl;
DATA(0x00529914) int giMapChangeCtr;
DATA(0x00529918) SMapChange sMapChangeQueue[CURSOR_MAP_CHANGE_QUEUE_COUNT];
DATA(0x0052a184) int gbWaitForRemoteReceive;
DATA(0x0052a188) unsigned char bMusicIsLooping[0x3c];
DATA(0x0052a1c4) townManager *gpTownManager;
DATA(0x0052a1c8) advManager *gpAdvManager;
DATA(0x0052a1d0) signed char gbGamePosToNetPos[OLD_MAIN_MATCH_BUFFER_SIZE];
