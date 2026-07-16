// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\KB.OBJ   from: (directly linked into exe)
// functions: 71   data: 441
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
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

#define KBFILE const_cast<char *>("I:\\Projects\\Heroes\\Prog\\SOURCE\\KB.CPP")
#define GLOBAL_POLL_SOUND_TIMER_SLOT 5
#define GLOBAL_MOUSE_TIMER_SLOT 6
#define GLOBAL_COLOR_CYCLE_TIMER_SLOT 7











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
    if (KBTickCount() > glTimers[GLOBAL_MOUSE_TIMER_SLOT] && !gbPutzingWithMouseCtr) {
        glTimers[GLOBAL_MOUSE_TIMER_SLOT] = KBTickCount() + 13;
        gpMouseManager->NewUpdate(0);
    }
    if (KBTickCount() > glTimers[GLOBAL_COLOR_CYCLE_TIMER_SLOT]) {
        if (giCycleType == 1 || giCycleType == 3)
            glTimers[GLOBAL_COLOR_CYCLE_TIMER_SLOT] = KBTickCount() + 110;
        else
            glTimers[GLOBAL_COLOR_CYCLE_TIMER_SLOT] = KBTickCount() + 200;
        bDoColorCycle = 1;
        if (giGraphicsType == 1 && giMainVideoModeColorDepth != 8) {
            glTimers[GLOBAL_COLOR_CYCLE_TIMER_SLOT] += 300;
            if (gbHeroMoving)
                bDoColorCycle = 0;
        }
        if (bDoColorCycle)
            CycleColors(0);
    }
    if (KBTickCount() > glTimers[GLOBAL_POLL_SOUND_TIMER_SLOT]) {
        glTimers[GLOBAL_POLL_SOUND_TIMER_SLOT] = KBTickCount() + 30;
        if (gbForegroundApp)
            gpSoundManager->PollSound();
        PollRemote();
    }
    gbInPollSound = 0;
}

VA(0x0049659e, 0x20)
void ForcePollSound(void)
{
    glTimers[GLOBAL_POLL_SOUND_TIMER_SLOT] = KBTickCount() - 1;
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

// @semantic
// Exact 0x164 frame, live/generated slots, CFG, and first table at +0x4d4 after
// recovering the menu/TCP arm order and both setup switches. Excluding table
// data, the first residual is retail's local continuation jump at +0x533; two
// more retail-only continuations at +0x652/+0x657 account exactly for the
// second-table offset (base +0x8e5, retail +0x8f4) and 486/489 relocations.
// External targets agree except two proven gsNetPlayerInfo interior aliases.
// Tried raw assignments, a void setter (wrong local jmp $+0), and a reference
// accessor (wrong pointer temporaries/frame). Revisit for inline-tail placement.
VA(0x00496e98, 0x16c0)
int oldmain(void)
{
    int command_a;
    int quit_g;
    int mainScreenLoaded_b;
    int firstMainScreen_c;
    int savedUpdateFlags_l;
    int player_h;
    // Retail reserves three unreferenced /Od locals around the live state slots.
    int unusedMainState_o;
    int unusedMenuState_d;
    int unusedPlayerState_c;
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
        } else {

process_menu_command:
            if (giMenuCommand != -1) {
                switch (giMenuCommand) {
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
                    if (!gpGame->PickLoadGame())
                        goto main_menu;
                    break;
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
                    if (!gpGame->NewGame())
                        goto main_menu;
                    break;
                }
                goto game_setup_complete;
            }

            if (gGameCommand != -1) {
                command_a = gGameCommand;
                gGameCommand = -1;
            } else {
                gpInitWin = new heroWindow(0, 0, "stpmain.bin");
                if (!gpInitWin)
                    MemError();
                gbInSetupDialog = 1;
                gpWindowManager->DoDialog(gpInitWin, InitMenuHandler, 0);
                delete gpInitWin;
                gpInitWin = 0;
                command_a = gpWindowManager->m_dialogResult;
                gbInSetupDialog = 0;
            }
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

            switch (giSetupGameType) {
            case OLD_MAIN_SETUP_NEW:
                if (gbInCampaign) {
                    gpGame->InitEntireCampaign(gbCampaignSideChoice);
                    result_i = gpGame->HandleCampaignWin();
                    if (result_i) {
                        gpGame->InitCampaignMap();
                        goto initialize_game;
                    } else {
                        gpWindowManager->FadeScreen(1, OLD_MAIN_FADE_SPEED,
                                                    gPalette);
                        mainScreenLoaded_b = 0;
                        goto main_menu;
                    }
                } else {
                    if (xIsPlayingExpansionCampaign) {
                        if (xCampaign.HandleVictory()) {
                            xCampaign.InitMap();
                            goto initialize_game;
                        } else {
                            gpWindowManager->FadeScreen(1, OLD_MAIN_FADE_SPEED,
                                                        gPalette);
                            mainScreenLoaded_b = 0;
                            goto main_menu;
                        }
                    } else {
                        LogStr("New Game 1");
                        if (!gpGame->NewGame())
                            goto main_menu;
                        LogStr("New Game 2");
                    }
                }
                break;
            case OLD_MAIN_SETUP_LOAD:
                LogStr("Load Game 1");
                if (!gpGame->PickLoadGame())
                    goto main_menu;
                LogStr("Load Game 2");
                break;
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
            break;
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
                "My heroes, our foes have been scattered, their castles broken and laid bare.  The great campaign is now complete, and I stand before you as the undisputed High King!\n\nOur victory was achieved in %d days!",
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
    for (i = 0; 0[&len] > i; i++) {
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

// @semantic
// Explicit-range first opcode divergence at +0x354: ours emits a five-byte
// continuation jmp where retail enters the next body; retail later uses lea at
// +0x39b where ours uses shl/sub at +0x3a0 after that shifted continuation.
// The 0x24 frame/slots, source CFG and semantics are complete; jump tables at
// +0xad/0x14 and +0x1ab/0x18 align, and all 89 relocations resolve with no
// base-only target. Direct retail-order cases and the frame-expression operand
// swap were tried without changing the residual. Revisit with focused exact-span
// variants after the TU's pre-target libclang diagnostics are resolved.
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
                    IMHotSpots[menu][INIT_MENU_HOTSPOT_X],
                    IMHotSpots[menu][INIT_MENU_HOTSPOT_Y],
                    IMHotSpots[menu][INIT_MENU_HOTSPOT_WIDTH],
                    IMHotSpots[menu][INIT_MENU_HOTSPOT_HEIGHT]);
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
                if (IMHotSpots[idx][INIT_MENU_HOTSPOT_X] <= msg.payload.mouse.screenX &&
                    IMHotSpots[idx][INIT_MENU_HOTSPOT_Y] <= msg.payload.mouse.screenY &&
                    msg.payload.mouse.screenX <
                        IMHotSpots[idx][INIT_MENU_HOTSPOT_X] +
                            IMHotSpots[idx][INIT_MENU_HOTSPOT_WIDTH] &&
                    msg.payload.mouse.screenY <
                        IMHotSpots[idx][INIT_MENU_HOTSPOT_Y] +
                            IMHotSpots[idx][INIT_MENU_HOTSPOT_HEIGHT]) {
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
                        IMHotSpots[lastIMHoverID][INIT_MENU_HOTSPOT_X],
                        IMHotSpots[lastIMHoverID][INIT_MENU_HOTSPOT_Y],
                        IMHotSpots[lastIMHoverID][INIT_MENU_HOTSPOT_WIDTH],
                        IMHotSpots[lastIMHoverID][INIT_MENU_HOTSPOT_HEIGHT]);
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
                        IMHotSpots[hoverIndex][INIT_MENU_HOTSPOT_X],
                        IMHotSpots[hoverIndex][INIT_MENU_HOTSPOT_Y],
                        IMHotSpots[hoverIndex][INIT_MENU_HOTSPOT_WIDTH],
                        IMHotSpots[hoverIndex][INIT_MENU_HOTSPOT_HEIGHT]);
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
                gArmyNamesPlural[
                    gDwellingType[race][building - KB_BUILDING_DWELLING_FIRST]]);
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
    } else {
        if (building >= KB_BUILDING_NEUTRAL_LIMIT)
            return;
        memcpy(dest, gNeutralBuildingCosts[building], KB_BUILDING_RESOURCE_COUNT * sizeof(int));
    }
}

VA(0x0049990c, 0x20)
char *GetMonsterName(int m)
{
    return gArmyNames[m];
}

// @early-stop
// All 320 relocation-masked bytes are identical and 11/11 effective targets
// agree. Only delinked local-label DIR32 names differ at +0xe1, +0xe8 and the
// +0xec..+0x108 switch table ($L... versus containing-function plus addend).
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

// @semantic
// Complete 0x2b5 body, 0x10 frame, local roles/slots, CFG, and 5/5 relocations
// align. Only +0x261/+0x264 differ: retail loads reqMask [ebp-8] then ANDs
// haveMask [ebp-4], while ours loads haveMask then ANDs reqMask. Commuting the
// AND/equality and wrapping either scalar lvalue with 0[&...] were byte-neutral;
// revisit after later KB TU/header state changes.
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
        case 2:
            result = WaitForHost();
            break;
        case 1:
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
// All 1,138 authoritative bytes are identical after masking relocations, and
// all 63 effective relocation targets agree. The retained residual is delinked
// identity for local jump tables at +0x35d..+0x388 and +0x41c..+0x427, plus
// equivalent string and cLuckInfo/cMoraleInfo interior aliases.
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

// @semantic
// Complete 0x19bb body, 0x1c4 frame/slots, CFG, and 300/300 effective relocation
// targets align. First raw divergence is +0xd00: the day calculation evaluates
// month then week and emits `lea [week7+month7*4]` with reversed base/index from
// retail; the same residual repeats at +0x15e5. At +0x1632 retail evaluates the
// campaign completed-table scenario index before type (ours does type first),
// shifting one gpGame relocation by four bytes; the days+bonus addition at
// +0x16a8/+0x16d1 is likewise commutative load order. Commuted terms, explicit
// 7*4 scaling, 0[&month/type], and a commuted subscript were byte-neutral.
// Revisit after later KB TU/header changes.
VA(0x0049a6c1, 0x19bb)
void CheckEndGame(int forcedResult, int dragonCityCaptured)
{
    int showedDialog;
    int defeated;
    int allowNormalVictory;
    int lastLivingPlayer;
    int survivingHumans;
    int lastHuman_i;
    int netHumanCount;
    int player;
    int heroIndex;
    int winFlag;
    playerData *rec;
    int savedRemoteOn;
    int numAlive;
    char unusedTextA_c[CHECK_END_GAME_TEXT_BUFFER_SIZE];
    char unusedTextB_c[CHECK_END_GAME_TEXT_BUFFER_SIZE];
    int sideBelow_i;
    int sideAbove;
    int bestGold;
    town *lossTown;
    town *victoryTownData;
    int currentDayIndex;
    int enemyRemaining;
    int hasRoland_j;
    int hasDwarfTown;
    char artifactName[CHECK_END_GAME_TEXT_BUFFER_SIZE];
    hero *artifactHeroPtr;
    int artifactWinnerPerson;
    hero *lossHero;
    hero *winningHeroEntry;
    int winnerPlayer;
    char campaignSaveName[20];
    int campaignHeroIndex;
    unsigned int carryoverHeroId;

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
    showedDialog = 0;

        for (player = 0; player < gpGame->m_playerCount; player++) {
            if (!gpGame->m_playerDead[player]) {
                rec = &gpGame->m_players[player];
                if ((rec->m_heroCount == 0 && rec->m_townCount == 0) ||
                    (xIsPlayingExpansionCampaign && xCampaign.IsSpecialLossCondition(player))) {
                    PlayerDead(player);
                    if (giThisGamePos == player) {
                        showedDialog = 1;
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
                            if (!showedDialog) {
                                showedDialog = 1;
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

        numAlive = 0;
        lastLivingPlayer = 0;
        survivingHumans = 0;
        lastHuman_i = 0;
        netHumanCount = 0;
        for (player = 0; player < gpGame->m_playerCount; player++) {
            if (!gpGame->m_playerDead[player]) {
                numAlive++;
                lastLivingPlayer = player;
                if (gbThisNetHumanPlayer[player]) {
                    netHumanCount++;
                }
                if (gbHumanPlayer[player]) {
                    survivingHumans++;
                    lastHuman_i = player;
                }
            }
        }

        winFlag = 0;
        defeated = 0;
        allowNormalVictory = 1;
        if ((gpGame->m_mapHeader.victoryCondition != CHECK_END_GAME_VICTORY_STANDARD && !gpGame->m_mapHeader.allowNormalVictory) ||
            (gbInCampaign && gpGame->m_campaignType == CHECK_END_GAME_ARCHIBALD_CAMPAIGN &&
             gpGame->m_campaignScenario + CHECK_END_GAME_SCENARIO_OFFSET == CHECK_END_GAME_SIDE_SCENARIO)) {
            allowNormalVictory = 0;
        }

        if (gpGame->m_mapHeader.victoryCondition == CHECK_END_GAME_VICTORY_SIDE &&
            gpGame->m_mapHeader.victoryConditionValue != CHECK_END_GAME_SIDE_SPECIAL_VALUE &&
            (!gbInCampaign || gpGame->m_campaignType != CHECK_END_GAME_ARCHIBALD_CAMPAIGN ||
             gpGame->m_campaignScenario + CHECK_END_GAME_SCENARIO_OFFSET != CHECK_END_GAME_SIDE_SCENARIO)) {
            sideBelow_i = 0;
            sideAbove = 0;
            for (player = 0; player < gpGame->m_playerCount; player++) {
                if (!gpGame->m_playerDead[player]) {
                    if (gpGame->m_players[player].m_color < gpGame->m_mapHeader.victorySideThreshold) {
                        sideBelow_i++;
                    } else {
                        sideAbove++;
                    }
                }
            }
            if (sideBelow_i == 0) {
                for (player = 0; player < gpGame->m_playerCount; player++) {
                    if (gbThisNetHumanPlayer[player] && !gpGame->m_playerDead[player] &&
                        gpGame->m_players[player].m_color >= gpGame->m_mapHeader.victorySideThreshold) {
                        winFlag = 1;
                    }
                }
            } else if (sideAbove == 0) {
                for (player = 0; player < gpGame->m_playerCount; player++) {
                    if (gbThisNetHumanPlayer[player] && !gpGame->m_playerDead[player] &&
                        gpGame->m_players[player].m_color < gpGame->m_mapHeader.victorySideThreshold) {
                        winFlag = 1;
                    }
                }
            }
            if (sideBelow_i == 0 || sideAbove == 0) {
                if (!winFlag) {
                    defeated = 1;
                }
                if (!showedDialog && winFlag) {
                    showedDialog = 1;
                    sprintf(gText, "The enemy is beaten.  Your side has triumphed!");
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }

        if (gpGame->m_mapHeader.victoryCondition == CHECK_END_GAME_VICTORY_CAPTURE_TOWN) {
            victoryTownData = gpGame->GetTown(
                gpGame->GetTownId(gpGame->m_mapHeader.victoryConditionValue, gpGame->m_mapHeader.victoryTownY));
            if (victoryTownData->m_owner != CHECK_END_GAME_NO_PLAYER &&
                (gbHumanPlayer[victoryTownData->m_owner] || gpGame->m_mapHeader.computerAlsoWins)) {
                if (gbThisNetHumanPlayer[victoryTownData->m_owner]) {
                    winFlag = 1;
                } else {
                    defeated = 1;
                }
                if (!showedDialog) {
                    showedDialog = 1;
                    if (winFlag) {
                        sprintf(gText, "You captured %s!  You are victorious.", victoryTownData->m_name);
                    } else {
                        sprintf(gText, "The enemy has captured %s!  They are triumphant.", victoryTownData->m_name);
                    }
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }

        if (gpGame->m_mapHeader.lossCondition == CHECK_END_GAME_LOSS_TOWN) {
            lossTown =
                gpGame->GetTown(gpGame->GetTownId(gpGame->m_mapHeader.lossConditionValue, gpGame->m_mapHeader.lossTownY));
            if (lossTown->m_owner == CHECK_END_GAME_NO_PLAYER || !gbHumanPlayer[lossTown->m_owner]) {
                defeated = 1;
                if (!showedDialog) {
                    showedDialog = 1;
                    sprintf(gText, "%s has fallen!  All is lost.", lossTown->m_name);
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }

        if (gpGame->m_mapHeader.victoryCondition == CHECK_END_GAME_VICTORY_GOLD) {
            bestGold = 0;
            winnerPlayer = CHECK_END_GAME_NO_PLAYER;
            for (player = 0; player < gpGame->m_playerCount; player++) {
                if ((gbHumanPlayer[player] || gpGame->m_mapHeader.computerAlsoWins) &&
                    gpGame->m_players[player].m_resources[CHECK_END_GAME_GOLD_RESOURCE] >=
                        gpGame->m_mapHeader.victoryConditionValue * CHECK_END_GAME_GOLD_SCALE &&
                    gpGame->m_players[player].m_resources[CHECK_END_GAME_GOLD_RESOURCE] >= bestGold) {
                    bestGold = gpGame->m_players[player].m_resources[CHECK_END_GAME_GOLD_RESOURCE];
                    winnerPlayer = player;
                }
                if (winnerPlayer != CHECK_END_GAME_NO_PLAYER) {
                    if (gbThisNetHumanPlayer[winnerPlayer]) {
                        winFlag = 1;
                    } else {
                        defeated = 1;
                    }
                    if (!showedDialog) {
                        showedDialog = 1;
                        if (winFlag) {
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
            winningHeroEntry = GetHeroSlot(gpGame->m_mapHeader.victoryConditionValue);
            if (winningHeroEntry->m_owner < 0 || winningHeroEntry->m_owner >= CHECK_END_GAME_PLAYER_COUNT ||
                gbHumanPlayer[winningHeroEntry->m_owner]) {
                winFlag = 1;
                if (!showedDialog) {
                    showedDialog = 1;
                    sprintf(gText, "You have captured the enemy hero %s!  Your quest is complete.",
                            winningHeroEntry->m_name);
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }

        if (gpGame->m_mapHeader.lossCondition == CHECK_END_GAME_LOSS_HERO) {
            lossHero = GetHeroSlot(gpGame->m_mapHeader.lossConditionValue);
            if (lossHero->m_owner < 0 || lossHero->m_owner >= CHECK_END_GAME_PLAYER_COUNT ||
                !gbHumanPlayer[lossHero->m_owner]) {
                defeated = 1;
                if (!showedDialog) {
                    showedDialog = 1;
                    sprintf(gText, "You have lost the hero %s.  Your quest is over.", lossHero->m_name);
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }

        if (gpGame->m_mapHeader.lossCondition == CHECK_END_GAME_LOSS_TIME) {
            if (gpGame->m_mapHeader.lossConditionValue <
                    (gpGame->m_week - 1) * CHECK_END_GAME_DAYS_PER_WEEK +
                    (gpGame->m_month - 1) * CHECK_END_GAME_DAYS_PER_MONTH +
                                                   gpGame->m_day) {
                defeated = 1;
                if (!showedDialog) {
                    showedDialog = 1;
                    sprintf(gText, "You have failed to complete your quest in time.  All is lost.");
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }

        if (gpGame->m_mapHeader.victoryCondition == CHECK_END_GAME_VICTORY_ARTIFACT) {
            artifactWinnerPerson = CHECK_END_GAME_NO_PLAYER;
            for (player = 0; player < gpGame->m_playerCount; player++) {
                if (!gpGame->m_playerDead[player]) {
                    for (heroIndex = 0; heroIndex < gpGame->m_players[player].m_heroCount; heroIndex++) {
                        artifactHeroPtr = gpGame->GetPlayerHero(player, heroIndex);
                        if (gpGame->m_mapHeader.victoryConditionValue > CHECK_END_GAME_ULTIMATE_ARTIFACT) {
                            if (artifactHeroPtr->HasArtifact(gpGame->m_mapHeader.victoryConditionValue - 1)) {
                                artifactWinnerPerson = player;
                            }
                        } else {
                            if (artifactHeroPtr->HasArtifact(0) || artifactHeroPtr->HasArtifact(1) ||
                                artifactHeroPtr->HasArtifact(2) || artifactHeroPtr->HasArtifact(3) ||
                                artifactHeroPtr->HasArtifact(4) || artifactHeroPtr->HasArtifact(5) ||
                                artifactHeroPtr->HasArtifact(6) || artifactHeroPtr->HasArtifact(7)) {
                                artifactWinnerPerson = player;
                            }
                        }
                    }
                }
            }
            if (artifactWinnerPerson != CHECK_END_GAME_NO_PLAYER) {
                if (gbThisNetHumanPlayer[artifactWinnerPerson]) {
                    winFlag = 1;
                } else {
                    defeated = 1;
                }
                if (!showedDialog) {
                    showedDialog = 1;
                    if (gpGame->m_mapHeader.victoryConditionValue == CHECK_END_GAME_ULTIMATE_ARTIFACT) {
                        sprintf(artifactName, "Ultimate Artifact");
                    } else {
                        sprintf(artifactName, gArtifactNames[gpGame->m_mapHeader.victoryConditionValue - 1]);
                    }
                    if (winFlag) {
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
                defeated = 1;
                if (!showedDialog) {
                    showedDialog = 1;
                    sprintf(gText, "All the dwarf towns have fallen.  This is a disastrous defeat!  You have "
                                   "lost.");
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }

        if (gbInCampaign && gpGame->m_campaignType == CHECK_END_GAME_ARCHIBALD_CAMPAIGN &&
            gpGame->m_campaignScenario + CHECK_END_GAME_SCENARIO_OFFSET == CHECK_END_GAME_SIDE_SCENARIO &&
            dragonCityCaptured) {
            winFlag = 1;
            if (!showedDialog) {
                showedDialog = 1;
                sprintf(gText, "Dragon city has fallen!  You are now the Master of the Dragons.");
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }

        if (gbInCampaign && gpGame->m_campaignType == CHECK_END_GAME_ROLAND_CAMPAIGN &&
            gpGame->m_campaignScenario + CHECK_END_GAME_SCENARIO_OFFSET == CHECK_END_GAME_ROLAND_CAPTURE_SCENARIO) {
            hasRoland_j = 0;
            for (player = 0; player < CHECK_END_GAME_HERO_COUNT; player++) {
                if (gpGame->m_heroRecs[player].m_portrait == CHECK_END_GAME_ROLAND_HERO &&
                    gpGame->m_heroRecs[player].m_owner >= 0 &&
                    gpGame->m_heroRecs[player].m_owner <= CHECK_END_GAME_PLAYER_COUNT - 1) {
                    hasRoland_j = 1;
                }
            }
            if (!hasRoland_j) {
                defeated = 1;
                if (!showedDialog) {
                    showedDialog = 1;
                    sprintf(gText, "Roland has been captured!  All is lost.");
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }

        if (gbInCampaign && gpGame->m_campaignType == CHECK_END_GAME_ROLAND_CAMPAIGN &&
            gpGame->m_campaignScenario + CHECK_END_GAME_SCENARIO_OFFSET == CHECK_END_GAME_ROLAND_CAPTURE_SCENARIO) {
            enemyRemaining = 0;
            for (player = 0; player < gpGame->m_playerCount; player++) {
                if (!gpGame->m_playerDead[player] && gpGame->m_players[player].m_color != CHECK_END_GAME_ROLAND_COLOR &&
                    gpGame->m_players[player].m_color != CHECK_END_GAME_ALLY_COLOR) {
                    enemyRemaining = 1;
                }
            }
            if (!enemyRemaining) {
                winFlag = 1;
                if (!showedDialog && winFlag) {
                    showedDialog = 1;
                    sprintf(gText, "The enemy is beaten.  Your side has triumphed!");
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }

        if (defeated) {
            gbGameOver = 1;
            giEndSequence = 0;
        }
        if (winFlag) {
            gbGameOver = 1;
            giEndSequence = 1;
        }

        if (numAlive == 1 || survivingHumans == 0 || (survivingHumans == 1 && !gbThisNetHumanPlayer[lastHuman_i])) {
            if (survivingHumans == 1 && gbThisNetHumanPlayer[lastHuman_i]) {
                if (allowNormalVictory) {
                    gbGameOver = 1;
                    giEndSequence = 1;
                }
            } else {
                gbGameOver = 1;
                giEndSequence = 0;
            }
        }

        if (savedRemoteOn && netHumanCount == 0) {
            gbGameOver = 1;
            giEndSequence = 0;
        }
        if (forcedResult == CHECK_END_GAME_FORCE_VICTORY) {
            winFlag = 1;
            gbGameOver = 1;
            giEndSequence = 1;
        }
        if (forcedResult == CHECK_END_GAME_FORCE_DEFEAT) {
            defeated = 1;
            gbGameOver = 1;
            giEndSequence = 0;
        }

        if (giEndSequence == 1 && gbGameOver) {
            winFlag = 1;
        }
        if (giEndSequence == 0 && gbGameOver) {
            defeated = 1;
        }

        if (gbInCampaign && winFlag) {
            currentDayIndex = (gpGame->m_week - 1) * CHECK_END_GAME_DAYS_PER_WEEK +
                         (gpGame->m_month - 1) * CHECK_END_GAME_DAYS_PER_MONTH + gpGame->m_day;
            gpGame->m_campaignScenarioWon = 1;
            gpGame->m_campaignScenarioCompleted[gpGame->m_campaignType][gpGame->m_campaignScenario] = 1;
            gpGame->m_campaignScenarioDays[gpGame->m_campaignType][gpGame->m_campaignScenario] = currentDayIndex;
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
                for (campaignHeroIndex = 0; campaignHeroIndex < gpGame->m_players[0].m_heroCount;
                     campaignHeroIndex++) {
                    if (carryoverHeroId == CHECK_END_GAME_SIDE_SPECIAL_VALUE ||
                        gpGame->m_heroRecs[gpGame->m_players[0].m_heroIds[campaignHeroIndex]].m_portrait ==
                            carryoverHeroId) {
                        break;
                    }
                }
                if (gpGame->m_players[0].m_heroCount == campaignHeroIndex) {
                    gpGame->m_campaignCarryoverCreatureTypes[0] = 0;
                    gpGame->m_campaignCarryoverCreatureCounts[0] = 1;
                } else {
                    for (player = 0; player < CHECK_END_GAME_ARMY_SLOTS; player++) {
                        gpGame->m_campaignCarryoverCreatureTypes[player] =
                            gpGame->m_heroRecs[gpGame->m_players[0].m_heroIds[campaignHeroIndex]]
                                .m_army.m_creatureTypes[player];
                        gpGame->m_campaignCarryoverCreatureCounts[player] =
                            gpGame->m_heroRecs[gpGame->m_players[0].m_heroIds[campaignHeroIndex]]
                                .m_army.m_creatureCounts[player];
                    }
                }
            }

            if (gpGame->m_campaignScenario + CHECK_END_GAME_SCENARIO_OFFSET != CHECK_END_GAME_LAST_SCENARIO &&
                (gpGame->m_campaignScenario + CHECK_END_GAME_SCENARIO_OFFSET != CHECK_END_GAME_FIRST_NO_SAVE_SCENARIO ||
                 gpGame->m_campaignType != CHECK_END_GAME_ROLAND_CAMPAIGN)) {
                sprintf(campaignSaveName, "%s%c_%02d", "WIN_",
                        gpGame->m_campaignType == CHECK_END_GAME_ROLAND_CAMPAIGN ? 'G' : 'E',
                        gpGame->m_campaignScenario + 1);
                gpGame->SaveGame(campaignSaveName, 1, 0);
            }
        } else if (xIsPlayingExpansionCampaign && winFlag) {
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

// @early-stop
// All 513 authoritative bytes are identical after masking the union of COFF
// relocation sites. All 57 effective targets agree; the only unpaired sites
// are eight base __imp__LoadMenuA@8 relocations whose target operands retain
// the linked 0x53a648 IAT address without COFF relocation records.
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
    gpGame->m_newGameInitialized = 0;
    gbInNewGameSetup = 0;
    strcpy(cNetBoxLine[0], "");
    strcpy(cNetBoxLine[1], "");
    strcpy(cNetBoxLine[2], "");
    strcpy(cNetBoxLine[3], "");
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

// @early-stop
// The 0xe0 frame and all retail stack slots match after hash-derived local
// renaming. Retail has exactly two extra five-byte continuation jumps at +0x101
// and +0x60f. Deleting those ranges leaves only four branch-displacement bytes
// caused by the inserted jumps; every non-jump opcode/operand matches, the size
// delta is exactly 10, and all 119 relocation tuples align after offset adjustment.
VA(0x0049c312, 0x61b)
void game::ShowMoraleInfo(hero *h, int dialogType)
{
    int mixedUndead4;
    int alignment_e;
    int homogeneous3;
    int modifierStart;
    char description[200];
    int slot8;

    mixedUndead4 = 0;
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
            mixedUndead4 = 1;
        }

        homogeneous3 = h->m_army.IsHomogeneous(-1);
        if (mixedUndead4 && homogeneous3 > 0) {
            homogeneous3 = 0;
        }
        if (homogeneous3 > 0) {
            alignment_e = 0;
            for (slot8 = 0; slot8 < 5; slot8++) {
                if (h->m_army.m_creatureTypes[slot8] != -1) {
                    alignment_e = gMonsterDatabase[h->m_army.m_creatureTypes[slot8]].race;
                }
            }
            sprintf(
                description,
                cMoraleInfo[MORALE_INFO_SAME_ALIGNMENT],
                gAlignmentNames[alignment_e]
            );
            strcat(gText, description);
        }
        if (homogeneous3 == -1) {
            sprintf(description, cMoraleInfo[MORALE_INFO_THREE_ALIGNMENTS]);
            strcat(gText, description);
        }
        if (homogeneous3 == -2) {
            sprintf(description, cMoraleInfo[MORALE_INFO_FOUR_ALIGNMENTS]);
            strcat(gText, description);
        }
        if (homogeneous3 == -3) {
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
    DATA(0x005164bc) static short clearMapExtraSourceLineBase =
        KB_SOURCE_LINE_CLEAR_MAP_EXTRA_BASE;
    int i;
    for (i = 0; i < iMaxMapExtra; i++) {
        if (ppMapExtra[i])
            BaseFree(ppMapExtra[i], KBFILE,
                     clearMapExtraSourceLineBase +
                         KB_SOURCE_LINE_CLEAR_MAP_EXTRA_ITEM_FREE_OFFSET);
    }
    if (ppMapExtra)
        BaseFree(ppMapExtra, KBFILE,
                 clearMapExtraSourceLineBase +
                     KB_SOURCE_LINE_CLEAR_MAP_EXTRA_POINTER_FREE_OFFSET);
    ppMapExtra = 0;
    if (pwSizeOfMapExtra)
        BaseFree(pwSizeOfMapExtra, KBFILE,
                 clearMapExtraSourceLineBase +
                     KB_SOURCE_LINE_CLEAR_MAP_EXTRA_SIZE_FREE_OFFSET);
    pwSizeOfMapExtra = 0;
    iMaxMapExtra = 0;
}

VA(0x0049cd75, 0x9f)
int GetMonType(int score, int campaign)
{
    int idx;
    for (idx = MONSTER_DATABASE_COUNT - 1; idx >= 0; idx--) {
        if (campaign == HIGH_SCORE_CAMPAIGN || campaign == HIGH_SCORE_EXPANSION_CAMPAIGN) {
            if (giScoreCampaignMon[idx][MONSTER_SCORE_THRESHOLD] >= score)
                return giScoreCampaignMon[idx][MONSTER_SCORE_TYPE];
        } else {
            if (giScoreMon[idx][MONSTER_SCORE_THRESHOLD] <= score)
                return giScoreMon[idx][MONSTER_SCORE_TYPE];
        }
    }
    return giScoreMon[0][MONSTER_SCORE_TYPE];
}

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

    file_a = open(filename_a, HIGH_SCORE_FILE_READ_FLAGS);
    if (file_a == -1)
        missingFile = 1;
    if (missingFile) {
        for (entry = 0; entry < HIGH_SCORE_ENTRY_COUNT; entry++) {
            memset(&entries_a[entry], 0, sizeof(HighScoreEntry));
            entries_a[entry].score = HIGH_SCORE_EMPTY;
        }
    } else {
        for (entry = 0; entry < HIGH_SCORE_ENTRY_COUNT; entry++)
            read(file_a, &entries_a[entry], sizeof(entries_a));
        close(file_a);
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
        for (destination = HIGH_SCORE_ENTRY_COUNT - 2;
             destination >= 0[&entry]; destination--)
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

        file_a = open(filename_a, HIGH_SCORE_FILE_WRITE_FLAGS, HIGH_SCORE_FILE_PERMISSIONS);
        if (file_a == -1)
            FileError(filename_a);
        for (entry = 0; entry < HIGH_SCORE_ENTRY_COUNT; entry++)
            write(file_a, &entries_a[entry], sizeof(HighScoreEntry));
        close(file_a);
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

// @semantic
// Complete 0xb85 body, 0x158 frame/slots, CFG, and 131/131 relocations align.
// The sole code residual is the printable-key guard: retail emits direct unsigned
// byte comparisons (`cmp byte; jb/jbe`), while ours zero-extends the low keyCode
// byte before each signed int comparison, making the sequence eight bytes longer.
// An unsigned-byte pointer lvalue and byte-cast bounds were byte-neutral; revisit
// after a genuine keyboard payload low-byte view is recovered in the shared type.
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
    DATA(0x005165e0) static short shutdownSourceLineBase =
        KB_SOURCE_LINE_SHUTDOWN_BASE;
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
        BaseFree(mapExtra, KBFILE,
                 shutdownSourceLineBase + KB_SOURCE_LINE_SHUTDOWN_MAP_FREE_OFFSET);
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
// All 191 normalized instructions and every relocation-masked code byte match.
// The 0x38 frame/14 stack slots align, and all 21 relocation sites and effective
// targets agree. The retained objdiff residual is only the delinked identity of
// the function-local smackFadeSourceLineBase data referenced by both objects.
VA(0x0049e3a8, 0x255)
void SmackFade(unsigned char *src, unsigned char *dst)
{
    DATA(0x00516668) static short smackFadeSourceLineBase =
        KB_SOURCE_LINE_SMACK_FADE_BASE;
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
    a = static_cast<unsigned char *>(BaseAlloc(
        0x300, KBFILE,
        smackFadeSourceLineBase + KB_SOURCE_LINE_SMACK_FADE_PALETTE_ALLOC_OFFSET));
    f = static_cast<unsigned char *>(BaseAlloc(
        0x100, KBFILE,
        smackFadeSourceLineBase + KB_SOURCE_LINE_SMACK_FADE_MAP_ALLOC_OFFSET));
    memset(a, 0, 0x300);
    memset(f, 0, 0x100);
    for (h = 0xa; h < 0xf6; h++) {
        e = (src[h * 3 + 2] + 0[&src[h * 3]] + src[h * 3 + 1]) / 3;
        d = 0x3e7;
        for (i = 0xa; i < 0x24; i++) {
            b = (0[&dst[i * 3 + 2]] + 0[&dst[i * 3]] +
                 dst[i * 3 + 1]) / 3;
            p = abs(e - b);
            if (0[&d] > p) {
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
    UpdatePalette(reinterpret_cast<signed char *>(dst));
    BaseFree(a, KBFILE,
             smackFadeSourceLineBase + KB_SOURCE_LINE_SMACK_FADE_PALETTE_FREE_OFFSET);
    BaseFree(f, KBFILE,
             smackFadeSourceLineBase + KB_SOURCE_LINE_SMACK_FADE_MAP_FREE_OFFSET);
}

VA(0x0049e5fd, 0x303)
void ShowCongrats(int highScoreType)
{
    DATA(0x0051670c) static short congratsSourceLineBase =
        KB_SOURCE_LINE_CONGRATS_BASE;
    unsigned char savedPalette[CONGRATS_PALETTE_BUFFER_SIZE];
    int baseScore;
    int score_e;
    char rating[CONGRATS_RATING_LENGTH];

    gpMouseManager->HideColorPointer();
    memcpy(savedPalette, gpBufferPalette->m_data, CONGRATS_PALETTE_SIZE);
    gpWindowManager->m_updateFlags = 0;
    congratsText = static_cast<char *>(BaseAlloc(
        CONGRATS_TEXT_SIZE, KBFILE,
        congratsSourceLineBase + KB_SOURCE_LINE_CONGRATS_ALLOC_OFFSET));
    baseScore = CalcBaseScore(giCurTurn);
    score_e = gpGame->m_difficultyRating * baseScore / CONGRATS_DIFFICULTY_SCALE;
    gpSoundManager->PlayAmbientMusic(CONGRATS_MUSIC_SILENT, 0, CONGRATS_MUSIC_SILENT);

    if (highScoreType == CONGRATS_STANDARD) {
        sprintf(rating, gArmyNames[GetMonType(score_e, highScoreType)]);
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
                giCurTurn, baseScore, gpGame->m_difficultyRating, score_e, rating);
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
    AddScoreToHighScore(score_e, giCurTurn, gpGame->m_difficultyRating,
                        CONGRATS_STANDARD, gpGame->m_mapHeader.name);
    BaseFree(congratsText, KBFILE,
             congratsSourceLineBase + KB_SOURCE_LINE_CONGRATS_FREE_OFFSET);
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
        while (gpSoundManager->DigitalReport(s.pMem, SOUND_DIGITAL_REPORT_PLAYING) &&
               KBTickCount() < endTime) {
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

VA(0x0049f61d, 0x310)
void UpdateSystemOptionsMenu(void)
{
    int menuCommand;
    int checkedCommand;

    if (gConfig.gfx[giCurExe].showMenu == 0)
        return;
    if (hmnuApp == 0)
        return;
    if (0[&hmnuAdv] != hmnuApp)
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

    for (tileRowPos = 0; 0[&tileRowPos] < numRows; tileRowPos++) {
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

    for (edge = 0; 0[&edge] < numRows; edge++) {
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
        return;
    }
    if (!exitInfo.continueGame) {
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
    int c = gsSpellInfo[spell].cost;
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
    for (i = 0; i < KB_WIN_SETUP_COUNT; i++) {
        if (gWinSetup[i].windowId == id) {
            a++;
            c.type = 0x200;
            c.payload.widget.command = 3;
            c.payload.widget.id = gWinSetup[i].widgetId;
            c.payload.widget.data.text = gWinSetup[i].text;
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
    DATA(0x00516d20) static short normalDialogSourceLineBase =
        KB_SOURCE_LINE_NORMAL_DIALOG_BASE;
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
            NORMAL_DIALOG_TEXT_LENGTH, KBFILE,
            normalDialogSourceLineBase +
                KB_SOURCE_LINE_NORMAL_DIALOG_FIRST_TEXT_ALLOC_OFFSET));
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
                normalDialogSourceLineBase +
                    KB_SOURCE_LINE_NORMAL_DIALOG_SECONDARY_TEXT_ALLOC_OFFSET));
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
                5, KBFILE,
                normalDialogSourceLineBase +
                    KB_SOURCE_LINE_NORMAL_DIALOG_PRIMARY_BONUS_ALLOC_OFFSET));
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
            3, KBFILE,
            normalDialogSourceLineBase +
                KB_SOURCE_LINE_NORMAL_DIALOG_OR_TEXT_ALLOC_OFFSET));
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
#define GROUND_REPEAT_2(value) value, value
#define GROUND_REPEAT_4(value) GROUND_REPEAT_2(value), GROUND_REPEAT_2(value)
#define GROUND_REPEAT_8(value) GROUND_REPEAT_4(value), GROUND_REPEAT_4(value)
#define GROUND_REPEAT_16(value) GROUND_REPEAT_8(value), GROUND_REPEAT_8(value)
#define GROUND_REPEAT_32(value) GROUND_REPEAT_16(value), GROUND_REPEAT_16(value)
#define GROUND_SHAPE_STANDARD_FRAME_SET                                           \
    GROUND_REPEAT_4(1), GROUND_REPEAT_4(2), GROUND_REPEAT_4(3),                   \
        GROUND_REPEAT_4(4), GROUND_REPEAT_4(5), GROUND_REPEAT_4(6),               \
        GROUND_REPEAT_4(7), GROUND_REPEAT_4(8), 10, 11, 12, 13, 14, 15,           \
        GROUND_REPEAT_8(0)

DATA(0x004f8c58) unsigned char giGroundToTerrain[GROUND_TILE_IMAGE_COUNT] = {
    GROUND_REPEAT_16(GROUND_TERRAIN_WATER), GROUND_REPEAT_8(GROUND_TERRAIN_WATER),
    GROUND_REPEAT_4(GROUND_TERRAIN_WATER), GROUND_REPEAT_2(GROUND_TERRAIN_WATER),
    GROUND_REPEAT_32(GROUND_TERRAIN_GRASS), GROUND_REPEAT_16(GROUND_TERRAIN_GRASS),
    GROUND_REPEAT_8(GROUND_TERRAIN_GRASS), GROUND_REPEAT_4(GROUND_TERRAIN_GRASS),
    GROUND_REPEAT_2(GROUND_TERRAIN_GRASS), GROUND_REPEAT_32(GROUND_TERRAIN_SNOW),
    GROUND_REPEAT_16(GROUND_TERRAIN_SNOW), GROUND_REPEAT_4(GROUND_TERRAIN_SNOW),
    GROUND_REPEAT_2(GROUND_TERRAIN_SNOW), GROUND_REPEAT_32(GROUND_TERRAIN_SWAMP),
    GROUND_REPEAT_16(GROUND_TERRAIN_SWAMP), GROUND_REPEAT_8(GROUND_TERRAIN_SWAMP),
    GROUND_REPEAT_4(GROUND_TERRAIN_SWAMP), GROUND_REPEAT_2(GROUND_TERRAIN_SWAMP),
    GROUND_REPEAT_32(GROUND_TERRAIN_LAVA), GROUND_REPEAT_16(GROUND_TERRAIN_LAVA),
    GROUND_REPEAT_4(GROUND_TERRAIN_LAVA), GROUND_REPEAT_2(GROUND_TERRAIN_LAVA),
    GROUND_REPEAT_32(GROUND_TERRAIN_DESERT), GROUND_REPEAT_16(GROUND_TERRAIN_DESERT),
    GROUND_REPEAT_8(GROUND_TERRAIN_DESERT), GROUND_REPEAT_2(GROUND_TERRAIN_DESERT),
    GROUND_TERRAIN_DESERT, GROUND_REPEAT_32(GROUND_TERRAIN_DIRT),
    GROUND_REPEAT_8(GROUND_TERRAIN_DIRT), GROUND_REPEAT_32(GROUND_TERRAIN_WASTELAND),
    GROUND_REPEAT_16(GROUND_TERRAIN_WASTELAND), GROUND_REPEAT_4(GROUND_TERRAIN_WASTELAND),
    GROUND_REPEAT_2(GROUND_TERRAIN_WASTELAND), GROUND_REPEAT_16(GROUND_TERRAIN_BEACH),
    GROUND_TERRAIN_BEACH
};
DATA(0x004f8e08) unsigned char giGroundShape[GROUND_TILE_IMAGE_COUNT] = {
    GROUND_REPEAT_2(16), GROUND_REPEAT_2(1), GROUND_REPEAT_4(2),
    GROUND_REPEAT_2(17), GROUND_REPEAT_2(3), GROUND_REPEAT_4(4),
    GROUND_REPEAT_4(0), GROUND_REPEAT_4(18), GROUND_REPEAT_2(20),
    GROUND_REPEAT_2(21), GROUND_REPEAT_2(19),
    GROUND_SHAPE_STANDARD_FRAME_SET, GROUND_REPEAT_16(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_STANDARD_FRAME_SET, GROUND_REPEAT_8(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_STANDARD_FRAME_SET, GROUND_REPEAT_16(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_STANDARD_FRAME_SET, GROUND_REPEAT_8(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_STANDARD_FRAME_SET, GROUND_REPEAT_8(GROUND_SHAPE_FLIPPED),
    GROUND_REPEAT_4(GROUND_SHAPE_FLIPPED), GROUND_SHAPE_FLIPPED,
    GROUND_REPEAT_4(5), GROUND_REPEAT_4(6), GROUND_REPEAT_4(7),
    GROUND_REPEAT_4(8), GROUND_REPEAT_8(0),
    GROUND_REPEAT_16(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_STANDARD_FRAME_SET, GROUND_REPEAT_8(GROUND_SHAPE_FLIPPED),
    GROUND_REPEAT_8(0), GROUND_REPEAT_8(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_FLIPPED
};

#undef GROUND_SHAPE_STANDARD_FRAME_SET
#undef GROUND_REPEAT_32
#undef GROUND_REPEAT_16
#undef GROUND_REPEAT_8
#undef GROUND_REPEAT_4
#undef GROUND_REPEAT_2
DATA(0x004f8fb8) unsigned char gColorTableTan[DIM_PALETTE_COLOR_COUNT] = {
    0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,
    0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc8, 0xc9, 0xcb, 0xcc, 0xce, 0xcf, 0xd0, 0xd1, 0xd2,
    0xd3, 0xd5, 0xd5, 0xd5, 0xd5, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,
    0xc6, 0xc6, 0xc6, 0xc7, 0xc8, 0xc9, 0xcb, 0xcc, 0xcd, 0xcf, 0xcf, 0xd1, 0xd2, 0xd3, 0xd4, 0xc6,
    0xc6, 0xc6, 0xc6, 0xc6, 0xc9, 0xcb, 0xcd, 0xcf, 0xd0, 0xd2, 0xd2, 0xd3, 0xd4, 0xd4, 0xd5, 0xd5,
    0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xc6, 0xc6, 0xc6, 0xc7, 0xc9, 0xcb, 0xcc, 0xce, 0xcf, 0xd0, 0xd1,
    0xd2, 0xd3, 0xd4, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xc6, 0xc6, 0xc6, 0xc6,
    0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc8, 0xc9, 0xcb,
    0xcc, 0xce, 0xcf, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc8, 0xc9, 0xcb, 0xcc,
    0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd4, 0xd5, 0xd5, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,
    0xc6, 0xc6, 0xc6, 0xc6, 0xc7, 0xc9, 0xcb, 0xcc, 0xce, 0xcf, 0xd0, 0xd1, 0xd3, 0xd4, 0xd5, 0xc6,
    0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,
    0xc7, 0xc9, 0xca, 0xcc, 0xce, 0xcf, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,
    0xc6, 0xc6, 0xc6, 0xc7, 0xca, 0xcd, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,
    0xc6, 0xc8, 0xcb, 0xcc, 0xcf, 0xd0, 0xd1, 0xc9, 0xcb, 0xcf, 0xd1, 0xce, 0xd1, 0xd0, 0xc6, 0xc6,
    0xcf, 0xd5, 0xc6, 0xc9, 0xce, 0xd0, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5
};
DATA(0x004f90b8) unsigned char gColorTableGray[DIM_PALETTE_COLOR_COUNT] = {
    0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x0a, 0x0b, 0x0c, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x12,
    0x13, 0x14, 0x14, 0x15, 0x16, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1a, 0x1b, 0x1c, 0x1d, 0x1f, 0x0e,
    0x10, 0x11, 0x12, 0x14, 0x15, 0x16, 0x18, 0x19, 0x1a, 0x1c, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x20,
    0x21, 0x21, 0x21, 0x22, 0x22, 0x10, 0x11, 0x12, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
    0x1c, 0x1e, 0x1f, 0x20, 0x20, 0x21, 0x22, 0x23, 0x23, 0x24, 0x24, 0x24, 0x0b, 0x0b, 0x0b, 0x0b,
    0x0b, 0x0c, 0x0c, 0x0c, 0x0d, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x15, 0x16,
    0x17, 0x18, 0x19, 0x0c, 0x0d, 0x0e, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x18, 0x18, 0x19,
    0x1a, 0x1b, 0x1c, 0x1d, 0x1f, 0x20, 0x20, 0x21, 0x0b, 0x0c, 0x0c, 0x0d, 0x0e, 0x0e, 0x10, 0x10,
    0x11, 0x12, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1d, 0x1e, 0x20, 0x0a,
    0x0b, 0x0b, 0x0c, 0x0c, 0x0c, 0x0d, 0x0d, 0x0e, 0x0e, 0x0f, 0x0f, 0x10, 0x10, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x16, 0x17, 0x18, 0x1a, 0x0a, 0x0a, 0x0b, 0x0b, 0x0b, 0x0c, 0x0c, 0x0c, 0x0c, 0x0e,
    0x10, 0x11, 0x12, 0x14, 0x16, 0x18, 0x11, 0x0a, 0x0c, 0x0f, 0x13, 0x0a, 0x0a, 0x0f, 0x11, 0x12,
    0x14, 0x15, 0x16, 0x17, 0x19, 0x1a, 0x1b, 0x1b, 0x18, 0x15, 0x16, 0x1a, 0x1a, 0x1b, 0x24, 0x0c,
    0x12, 0x19, 0x13, 0x15, 0x18, 0x1a, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24
};
DATA(0x004f91b8) unsigned char gColorTableYellow[DIM_PALETTE_COLOR_COUNT] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x72, 0x73, 0x73, 0x74, 0x75, 0x75,
    0x76, 0x77, 0x77, 0x78, 0x79, 0x79, 0x7a, 0x7b, 0x7b, 0x7c, 0x7d, 0x7d, 0x7e, 0x7f, 0x7f, 0x80,
    0x81, 0x81, 0x82, 0x82, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
DATA(0x004f92b8) unsigned char gColorTableScenWin[DIM_PALETTE_COLOR_COUNT] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
DATA(0x004f93b8) unsigned char gColorTableDarkGray[DIM_PALETTE_COLOR_COUNT] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14,
    0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24,
    0x24, 0x24, 0x24, 0x24, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
DATA(0x004f94b8) unsigned char gColorTableRed[DIM_PALETTE_COLOR_COUNT] = {
    0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xb4, 0xb6, 0xb8, 0xba, 0xd0, 0xd1,
    0xd2, 0xd2, 0xd3, 0xd3, 0xd4, 0xd5, 0xd5, 0xc4, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5,
    0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xb4, 0xb4, 0xb6, 0xb6, 0xb8, 0xba, 0xd0, 0xd1, 0xd1, 0xd2, 0xd2,
    0xd2, 0xd3, 0xd3, 0xc1, 0xd4, 0xd4, 0xd5, 0xd5, 0xc4, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xd0,
    0xd1, 0xd2, 0xd3, 0xc1, 0xd4, 0xd5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5,
    0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xd1, 0xd2, 0xd2, 0xd3, 0xd4, 0xd4, 0xd5, 0xc4, 0xc5, 0xc5, 0xc5,
    0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xb4, 0xb4, 0xb4, 0xb4,
    0xb4, 0xb6, 0xb6, 0xb7, 0xb8, 0xb9, 0xd0, 0xbc, 0xbd, 0xbe, 0xd2, 0xbf, 0xd3, 0xc1, 0xc1, 0xc2,
    0xc3, 0xc4, 0xc5, 0xb6, 0xb8, 0xd0, 0xd1, 0xd1, 0xd2, 0xd3, 0xd3, 0xd4, 0xd5, 0xd5, 0xc4, 0xc5,
    0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xd1, 0xd1, 0xd2, 0xd2, 0xd3, 0xd3, 0xc1, 0xd4,
    0xd5, 0xd5, 0xd5, 0xc4, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xb4,
    0xb4, 0xb4, 0xb6, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xbb, 0xd3, 0xb4, 0xb4, 0xd7, 0xbf,
    0xc0, 0xc1, 0xc2, 0xd5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5,
    0xd3, 0xc5, 0xd3, 0xd4, 0xc4, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5
};
DATA(0x004f95b8) unsigned char gColorTableDarkBrown[DIM_PALETTE_COLOR_COUNT] = {
    0x32, 0x2a, 0x2a, 0x2a, 0x2a, 0x32, 0x32, 0x32, 0x32, 0x35, 0x2a, 0x2b, 0x2b, 0x2c, 0x2c, 0x2d,
    0x2e, 0x2e, 0x2f, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a,
    0x3c, 0x3e, 0x3e, 0x3e, 0x3e, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x2d,
    0x2e, 0x2f, 0x30, 0x32, 0x33, 0x34, 0x36, 0x37, 0x38, 0x3a, 0x3a, 0x3b, 0x3c, 0x3c, 0x3d, 0x3e,
    0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x39, 0x3a,
    0x3a, 0x3c, 0x3c, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x2b, 0x2b, 0x2c, 0x2c,
    0x2c, 0x2d, 0x2d, 0x2d, 0x2e, 0x2f, 0x30, 0x30, 0x31, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x3a, 0x3b, 0x3c, 0x2c, 0x2c, 0x2d, 0x2e, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x35,
    0x36, 0x37, 0x38, 0x39, 0x3a, 0x3c, 0x3e, 0x3e, 0x2b, 0x2b, 0x2c, 0x2c, 0x2d, 0x2d, 0x2e, 0x2f,
    0x2f, 0x30, 0x31, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3c, 0x3d, 0x2b,
    0x2c, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x34, 0x35, 0x36, 0x37, 0x39, 0x3a, 0x3c, 0x3c, 0x3e, 0x3e,
    0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x2c, 0x2c, 0x2d, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33,
    0x35, 0x36, 0x37, 0x39, 0x3a, 0x3b, 0x36, 0x3a, 0x3e, 0x3e, 0x2a, 0x2d, 0x31, 0x37, 0x2f, 0x31,
    0x32, 0x33, 0x34, 0x35, 0x37, 0x38, 0x3a, 0x33, 0x34, 0x37, 0x39, 0x36, 0x39, 0x38, 0x2c, 0x30,
    0x36, 0x3e, 0x31, 0x33, 0x36, 0x39, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x2a
};
DATA(0x004f96b8) int MAP_WIDTH = 72;
DATA(0x004f96bc) int MAP_HEIGHT = 72;
DATA(0x004f96c0) unsigned char *mapExtra = 0;
DATA(0x004f96c4) int gbClosingApp = 0;
DATA(0x004f96c8) int gbForegroundApp = 0;
DATA(0x004f96cc) int giMainVideoModeColorDepth = 8;
DATA(0x004f96d0) int giMainVideoModeWidth = 640;
DATA(0x004f96d4) int giMainVideoModeHeight = 480;
DATA(0x004f96d8) unsigned long glMilliCounter = 0;
DATA(0x004f96e0) unsigned char gMapColors[RADAR_MAP_COLOR_COUNT] = {
    77, 98, 13, 104, 32, 118, 54, 206, 41, 0, 0, 0, 0, 0, 0, 0
};
DATA(0x004f96f0) unsigned char gObjectColors[RADAR_OBJECT_COLOR_COUNT] = {
    16, 48, 98, 160, 126, 74, 110, 179, 100, 218, 12, 12, 12, 12, 12, 12
};
DATA(0x004f9700) unsigned char gOwnerColors[RADAR_OWNER_COLOR_COUNT] = {
    73, 105, 190, 114, 205, 138, 10, 0
};
DATA(0x004f9708) char *gTilesetFiles[64] = {
    "", "", "", "", "", "", "", "", "", "", "objnhaun.icn",
    "objnarti.icn", "mons32.icn", "art32.icn", "flag32.icn", "ressmall.icn",
    "hourglas.icn", "route.icn", "", "stonback.icn", "minimon.icn",
    "minihero.icn", "mtnsnow.icn", "mtnswmp.icn", "mtnlava.icn",
    "mtndsrt.icn", "mtndirt.icn", "mtnmult.icn", "", "extraovr.icn",
    "road.icn", "mtncrck.icn", "mtngras.icn", "trejngl.icn", "treevil.icn",
    "objntown.icn", "objntwba.icn", "objntwsh.icn", "objntwrd.icn",
    "objnxtra.icn", "objnwat2.icn", "objnmul2.icn", "tresnow.icn",
    "trefir.icn", "trefall.icn", "stream.icn", "objnrsrc.icn", "dummy.icn",
    "objngra2.icn", "tredeci.icn", "objnwatr.icn", "objngras.icn",
    "objnsnow.icn", "objnswmp.icn", "objnlava.icn", "objndsrt.icn",
    "objndirt.icn", "objncrck.icn", "objnlav3.icn", "objnmult.icn",
    "objnlav2.icn", "x_loc1.icn", "x_loc2.icn", "x_loc3.icn"
};
DATA(0x004f9808) unsigned char bPuzzleDraw[PUZZLE_DRAW_TABLE_COUNT] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};
DATA(0x004f9848) unsigned char uDimPal[DIM_PALETTE_SET_COUNT]
                                               [DIM_PALETTE_LEVEL_COUNT]
                                               [DIM_PALETTE_COLOR_COUNT] = {
    {
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
            0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x24,
            0x24, 0x24, 0x24, 0x24, 0x24, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
            0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x44,
            0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54,
            0x54, 0x54, 0x54, 0x54, 0x54, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65,
            0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6b, 0x6b, 0x6b, 0x6b, 0x6b, 0x6b, 0x72, 0x73, 0x74, 0x75,
            0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x82, 0x82, 0x82,
            0x82, 0x82, 0x82, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94,
            0x95, 0x96, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5,
            0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xb4,
            0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4,
            0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3,
            0xd4, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xe1, 0xe2,
            0xe3, 0xe4, 0xe5, 0xe6, 0xe6, 0xe6, 0xe6, 0x49, 0x4b, 0x4d, 0x4f, 0x51, 0x4c, 0x4e, 0x4a, 0x4c,
            0x4e, 0x50, 0xf4, 0xf5, 0xf5, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        },
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13,
            0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23,
            0x24, 0x24, 0x24, 0x24, 0x24, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33,
            0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x42,
            0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52,
            0x53, 0x54, 0x54, 0x54, 0x54, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63,
            0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6b, 0x6b, 0x6b, 0x6b, 0x70, 0x71, 0x72, 0x73,
            0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x82,
            0x82, 0x82, 0x82, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92,
            0x93, 0x94, 0x95, 0x96, 0x97, 0x97, 0x97, 0x97, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3,
            0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xae, 0xae, 0xae, 0xae, 0xb2,
            0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2,
            0xc3, 0xc4, 0xc5, 0xc5, 0xc5, 0xc5, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2,
            0xd3, 0xd4, 0xd5, 0xd5, 0xd5, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xe0, 0xe1,
            0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe6, 0xe6, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c,
            0x4c, 0x4e, 0xf4, 0xf5, 0xf5, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        },
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11,
            0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21,
            0x22, 0x23, 0x24, 0x24, 0x24, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31,
            0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3e, 0x3e, 0x41,
            0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51,
            0x52, 0x53, 0x54, 0x54, 0x54, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61,
            0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6b, 0x6b, 0x6e, 0x6f, 0x70, 0x71,
            0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81,
            0x82, 0x82, 0x82, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91,
            0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x97, 0x97, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1,
            0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xae, 0xae, 0xb1,
            0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1,
            0xc2, 0xc3, 0xc4, 0xc5, 0xc5, 0xc5, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1,
            0xd2, 0xd3, 0xd4, 0xd5, 0xd5, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xdf, 0xe0,
            0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe6, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c,
            0x4c, 0x4c, 0xf3, 0xf4, 0xf5, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        },
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
            0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
            0x21, 0x22, 0x23, 0x24, 0x24, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
            0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3e, 0x40,
            0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,
            0x51, 0x52, 0x53, 0x54, 0x54, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60,
            0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6b, 0x6d, 0x6e, 0x6f, 0x70,
            0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80,
            0x81, 0x82, 0x82, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90,
            0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x97, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0,
            0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xae, 0xb0,
            0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0,
            0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc5, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0,
            0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xdf, 0xe0,
            0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe6, 0x4b, 0x4b, 0x4b, 0x4b, 0x4b, 0x4b, 0x4b, 0x4b, 0x4b,
            0x4b, 0x4b, 0xf3, 0xf4, 0xf5, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        }
    },
    {
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0a, 0x0b, 0x0b, 0x0b, 0x0c,
            0x0d, 0x0d, 0x0d, 0x0e, 0x0e, 0x0f, 0x0f, 0x0f, 0x10, 0x11, 0x11, 0x11, 0x12, 0x12, 0x13, 0x13,
            0x14, 0x14, 0x14, 0x15, 0x15, 0x0b, 0x25, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27, 0x27, 0x28, 0x28,
            0x29, 0x29, 0x29, 0x29, 0x2a, 0x2a, 0x13, 0x2a, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x15, 0x0c,
            0x83, 0x3f, 0x3f, 0x3f, 0x40, 0x40, 0x40, 0x41, 0x41, 0x41, 0x41, 0x41, 0xf2, 0xf2, 0xf2, 0xf2,
            0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0x0d, 0x0e, 0x0f, 0x0f, 0x10, 0x55, 0x11, 0x55, 0x55, 0x55, 0x55,
            0x13, 0x56, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x0a, 0x6c, 0x6c, 0x6d,
            0x6d, 0x6d, 0x6e, 0x6e, 0x6e, 0x6e, 0xc7, 0x28, 0x29, 0x29, 0x29, 0x29, 0x29, 0x2a, 0x2a, 0x2a,
            0x2a, 0x14, 0x14, 0x0b, 0x0b, 0x83, 0x83, 0x84, 0x84, 0x84, 0x85, 0x85, 0x86, 0x86, 0x86, 0x87,
            0x87, 0x12, 0x88, 0x13, 0x13, 0x14, 0x14, 0x14, 0x0a, 0x0b, 0x0b, 0x0b, 0x0c, 0x0c, 0x0d, 0x0d,
            0x0d, 0x0e, 0x0f, 0x0f, 0x0f, 0x10, 0x11, 0x11, 0x11, 0x12, 0x12, 0x13, 0x13, 0x14, 0x14, 0x0b,
            0xaf, 0xaf, 0xb0, 0xb0, 0x26, 0xb1, 0xb1, 0xb2, 0xb2, 0xb2, 0xb3, 0xb3, 0xb3, 0xb3, 0xb4, 0xb4,
            0xb4, 0xb4, 0xb4, 0xb4, 0x15, 0x15, 0x6c, 0x6c, 0x26, 0x6d, 0x26, 0x6d, 0x27, 0x28, 0x28, 0x29,
            0x29, 0x29, 0x2a, 0x2a, 0x2a, 0x14, 0xc7, 0xb3, 0xb4, 0xb4, 0x6e, 0x6e, 0x28, 0x2a, 0x6e, 0x6e,
            0x56, 0x56, 0x56, 0x56, 0x12, 0x12, 0x13, 0x41, 0x41, 0x41, 0x42, 0x41, 0x42, 0x41, 0x98, 0x9b,
            0x41, 0xf2, 0x0f, 0x10, 0x11, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        },
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0b, 0x0c, 0x0c, 0x0d,
            0x0d, 0x0e, 0x0f, 0x0f, 0x10, 0x10, 0x11, 0x11, 0x12, 0x13, 0x14, 0x14, 0x15, 0x15, 0x16, 0x16,
            0x17, 0x18, 0x18, 0x19, 0x19, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27, 0x28, 0x29, 0x29, 0x29, 0x2a,
            0x2a, 0x2b, 0x2b, 0x2c, 0x2c, 0x2d, 0x2d, 0x2e, 0x2e, 0x17, 0x18, 0x18, 0x18, 0x18, 0x18, 0x83,
            0x3f, 0x3f, 0x40, 0x40, 0x41, 0x41, 0x42, 0x42, 0xf2, 0x43, 0x43, 0x44, 0x44, 0xf3, 0xf3, 0xf3,
            0xf3, 0xf3, 0xf3, 0xf3, 0xf3, 0x0f, 0x0f, 0x55, 0x55, 0x55, 0x55, 0x56, 0x56, 0x57, 0x57, 0x58,
            0x58, 0x58, 0x58, 0x59, 0x18, 0x5a, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x25, 0x6c, 0x6d, 0x6d,
            0x6e, 0x6e, 0x6f, 0x6f, 0xc8, 0xc8, 0xc9, 0xc9, 0x2a, 0x2b, 0x2b, 0x2c, 0x2c, 0x2c, 0x2d, 0x2d,
            0x2e, 0x2e, 0x2e, 0x0b, 0x83, 0x84, 0x84, 0x84, 0x85, 0x85, 0x86, 0x87, 0x87, 0x88, 0xf2, 0x89,
            0x89, 0x8a, 0xf3, 0xf3, 0xf3, 0xf3, 0xf3, 0x18, 0x98, 0x98, 0x99, 0x99, 0x9a, 0x9a, 0x9b, 0x9c,
            0x9c, 0x9d, 0x9e, 0x9e, 0x9f, 0x12, 0x13, 0x13, 0x14, 0x14, 0x15, 0x16, 0x16, 0x17, 0x18, 0x25,
            0xaf, 0xb0, 0xb0, 0xb1, 0xb1, 0xb2, 0xb3, 0xb3, 0xb4, 0xb4, 0xb4, 0xb5, 0xb5, 0xb5, 0xb6, 0xb6,
            0xb6, 0x2e, 0x2f, 0x2f, 0x30, 0x19, 0x6c, 0x6d, 0x6d, 0x6d, 0xc6, 0xc7, 0xc7, 0xc9, 0xc9, 0x2a,
            0x2b, 0x2b, 0x2c, 0x2d, 0x2e, 0x2e, 0xc9, 0xb5, 0xb6, 0xb7, 0x6f, 0x6f, 0xca, 0x2d, 0x6f, 0x6f,
            0x57, 0x58, 0x58, 0x58, 0x58, 0x15, 0x16, 0x42, 0x42, 0x44, 0x44, 0x43, 0x44, 0x44, 0x98, 0x9d,
            0x42, 0x45, 0x10, 0x12, 0x14, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        },
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0b, 0x0c, 0x0d, 0x0e,
            0x0e, 0x0f, 0x10, 0x11, 0x11, 0x12, 0x13, 0x14, 0x14, 0x15, 0x16, 0x17, 0x18, 0x18, 0x19, 0x1a,
            0x1a, 0x1b, 0x1c, 0x1d, 0x1d, 0x25, 0x25, 0x26, 0x27, 0x28, 0x28, 0x29, 0x2a, 0x2a, 0x2b, 0x2c,
            0x2c, 0x2d, 0x2e, 0x2e, 0x2f, 0x2f, 0x30, 0x30, 0x31, 0x32, 0x32, 0x1b, 0x1c, 0x1c, 0x1c, 0x3f,
            0x3f, 0x40, 0x41, 0x41, 0x42, 0x43, 0x43, 0x44, 0x45, 0x45, 0x45, 0x46, 0x46, 0x46, 0xf4, 0x47,
            0xf4, 0xf4, 0xf4, 0xf4, 0xf5, 0x10, 0x55, 0x55, 0x56, 0x57, 0x57, 0x58, 0x58, 0x59, 0x5a, 0x5a,
            0x5b, 0x5b, 0x5b, 0x5c, 0x5d, 0x5d, 0x5d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x25, 0x6d, 0x6d, 0x6e,
            0x6f, 0x6f, 0x70, 0x71, 0x70, 0x70, 0xcb, 0xcb, 0xcb, 0x2c, 0x2d, 0x2e, 0x2f, 0x2f, 0x2f, 0x30,
            0x30, 0x31, 0x32, 0x83, 0x83, 0x84, 0x85, 0x85, 0x86, 0x87, 0x88, 0x88, 0x89, 0x89, 0x8b, 0x8b,
            0x8b, 0x8d, 0x8d, 0x8d, 0x8f, 0x8f, 0xf5, 0xf5, 0x98, 0x98, 0x99, 0x9a, 0x9b, 0x9b, 0x9c, 0x9d,
            0x9e, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa3, 0xa4, 0xa5, 0xa5, 0xa6, 0x1a, 0x1a, 0x1b, 0xaf,
            0x0d, 0xb0, 0xb1, 0xb2, 0xb2, 0xb3, 0xb4, 0xb5, 0xb5, 0xb6, 0xb6, 0xb7, 0xb7, 0xb7, 0xb8, 0xb8,
            0xb9, 0xb9, 0x32, 0x32, 0x34, 0x34, 0x6d, 0x6d, 0xc6, 0xc7, 0xc8, 0xc9, 0xc9, 0xca, 0xca, 0x2c,
            0x2d, 0x2e, 0x2f, 0x30, 0x30, 0x31, 0xcc, 0xcd, 0xb9, 0xb9, 0x70, 0x70, 0xcc, 0x2f, 0x70, 0x71,
            0x58, 0x59, 0x5b, 0x5c, 0x5d, 0x5d, 0x19, 0x42, 0x44, 0x45, 0x45, 0x44, 0x45, 0x45, 0x99, 0x9f,
            0x44, 0x47, 0x12, 0xf2, 0xf3, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        },
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0d, 0x0e,
            0x0f, 0x10, 0x11, 0x11, 0x13, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x18, 0x1a, 0x1a, 0x1b, 0x1c,
            0x1c, 0x1e, 0x1e, 0x1f, 0x20, 0x25, 0x26, 0x27, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2b, 0x2c, 0x2d,
            0x2e, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x32, 0x33, 0x34, 0x34, 0x35, 0x36, 0x36, 0x1e, 0x1f, 0x3f,
            0x40, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x45, 0x46, 0x47, 0x47, 0x47, 0x48, 0x48, 0x48, 0x49,
            0x49, 0x49, 0xa8, 0xa8, 0xa8, 0x55, 0x55, 0x56, 0x57, 0x58, 0x58, 0x59, 0x5a, 0x5b, 0x5b, 0x5c,
            0x5d, 0x5d, 0x5e, 0x5f, 0x5f, 0x60, 0x60, 0x60, 0x1f, 0x20, 0x20, 0x20, 0x6c, 0x6d, 0xc6, 0x6e,
            0x6f, 0x70, 0x71, 0x71, 0x71, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x2f, 0x30, 0x32, 0x32, 0x33,
            0x33, 0x34, 0x34, 0x83, 0x84, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x89, 0x8a, 0x8b, 0x8c, 0x8d,
            0x8d, 0x8f, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x1e, 0x98, 0x99, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e,
            0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0x1c, 0x1d, 0xaf,
            0xb0, 0xb1, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb6, 0xb7, 0xb8, 0xb9, 0xb9, 0xb9, 0xba, 0xba,
            0xbb, 0x32, 0x34, 0x34, 0x36, 0x37, 0x6d, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xca, 0xcc, 0xcc, 0xcd,
            0xcf, 0x2f, 0x31, 0x32, 0x33, 0x34, 0xce, 0xce, 0xbb, 0xbc, 0x71, 0x71, 0x76, 0x31, 0xde, 0xde,
            0xdf, 0xe0, 0xe1, 0xe2, 0x5f, 0xe3, 0xe4, 0x43, 0x44, 0x46, 0x47, 0x45, 0x47, 0x46, 0x99, 0x41,
            0x45, 0x49, 0xf2, 0x16, 0xf3, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        }
    },
    {
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0b, 0x0c, 0x0c, 0x0d,
            0x0e, 0x0e, 0x0f, 0x10, 0x10, 0x11, 0x12, 0x12, 0x13, 0x14, 0xf2, 0xf2, 0x16, 0x16, 0x17, 0xf3,
            0xf3, 0x19, 0xf4, 0xf4, 0xf4, 0x0b, 0x25, 0x26, 0x26, 0x27, 0x28, 0x28, 0x29, 0xb2, 0x12, 0x13,
            0x14, 0x14, 0x15, 0x15, 0x16, 0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17, 0x18, 0xf4, 0x3f,
            0x3f, 0x40, 0x40, 0x41, 0x41, 0x42, 0x42, 0x43, 0x43, 0x44, 0x44, 0x44, 0x45, 0x45, 0x45, 0x45,
            0x45, 0x45, 0x46, 0x46, 0x46, 0x0f, 0x0f, 0x10, 0x55, 0x56, 0x12, 0x13, 0x13, 0x14, 0x9f, 0x15,
            0x15, 0xa1, 0x16, 0xa3, 0xa3, 0xa3, 0x17, 0x17, 0xa5, 0xa5, 0xf4, 0xf4, 0x25, 0x6c, 0x26, 0x6d,
            0x6d, 0x6e, 0xc7, 0xc8, 0xc7, 0x28, 0x29, 0x2a, 0x2a, 0x2a, 0x2b, 0x2b, 0x2c, 0x16, 0x16, 0x17,
            0x17, 0x17, 0x17, 0x83, 0x83, 0x84, 0x84, 0x85, 0x85, 0x86, 0x86, 0x87, 0x87, 0x88, 0x88, 0x89,
            0x89, 0x8a, 0x8a, 0x8b, 0x8b, 0x8c, 0x8d, 0xf4, 0x98, 0x98, 0x99, 0x99, 0x9a, 0x9a, 0x9b, 0x9b,
            0x9c, 0x9c, 0x9d, 0x9e, 0x9e, 0x9f, 0xf2, 0x9f, 0xa1, 0xa1, 0xf3, 0xf3, 0xf3, 0xf3, 0xa4, 0x0b,
            0xaf, 0xb0, 0xb0, 0xb1, 0xb1, 0xb2, 0xb3, 0xb3, 0xb4, 0xb4, 0xb5, 0xb5, 0xb6, 0xb6, 0xb6, 0xb6,
            0xb7, 0x16, 0x17, 0x17, 0x17, 0x17, 0x6c, 0x26, 0x26, 0x27, 0x27, 0x28, 0x28, 0x29, 0xb2, 0xb4,
            0x2a, 0x2c, 0x2d, 0x17, 0x17, 0x17, 0xb4, 0xb5, 0xb5, 0xb7, 0x6e, 0xc8, 0x2a, 0x2d, 0x55, 0x56,
            0x57, 0x57, 0x57, 0x15, 0x16, 0x16, 0x17, 0x42, 0x42, 0x43, 0x44, 0x43, 0x44, 0x44, 0x99, 0x9e,
            0x43, 0x46, 0x40, 0x41, 0xf2, 0xf3, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x0a
        },
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0b, 0x0c, 0x0d, 0x0e,
            0x0e, 0x0f, 0x10, 0x10, 0x11, 0x12, 0x13, 0x13, 0x14, 0xf2, 0x16, 0x16, 0xf3, 0xf3, 0xf3, 0xf4,
            0xf4, 0xf4, 0xf4, 0xf5, 0xf5, 0x25, 0x25, 0x26, 0xb0, 0x27, 0xb1, 0x29, 0x29, 0x2a, 0xb3, 0x14,
            0xb4, 0x2d, 0x16, 0x17, 0x17, 0x18, 0x18, 0x18, 0x18, 0x19, 0x19, 0x19, 0x19, 0x1a, 0x1a, 0x3f,
            0x3f, 0x40, 0x40, 0x41, 0x42, 0x43, 0x43, 0x44, 0x44, 0x45, 0x45, 0x46, 0x46, 0x46, 0x46, 0x46,
            0x47, 0x47, 0x47, 0x47, 0x47, 0x0f, 0x55, 0x55, 0x56, 0x56, 0x57, 0x14, 0x58, 0x59, 0x16, 0xa1,
            0xa2, 0xa3, 0xa3, 0xa4, 0xa4, 0xa5, 0xa5, 0xa6, 0xa6, 0xa7, 0xa7, 0xa7, 0x25, 0x6c, 0x6d, 0x6d,
            0x6e, 0xc7, 0x6f, 0x6f, 0xc8, 0xc9, 0x29, 0x2b, 0x2b, 0x2b, 0x2c, 0x2c, 0x2e, 0x2e, 0x18, 0x18,
            0x19, 0x19, 0x19, 0x83, 0x83, 0x84, 0x84, 0x85, 0x86, 0x87, 0x87, 0x88, 0x88, 0x89, 0x8a, 0x8a,
            0x8b, 0x8b, 0x8c, 0x8c, 0x8d, 0x8d, 0x8e, 0x8f, 0x98, 0x98, 0x99, 0x99, 0x9a, 0x9b, 0x9b, 0x9c,
            0x9c, 0x9d, 0x9e, 0x9e, 0x9f, 0x9f, 0xa1, 0xa1, 0xa2, 0xa2, 0xa3, 0xa4, 0xf4, 0xf4, 0xf4, 0xaf,
            0xaf, 0xb0, 0xb1, 0xb1, 0xb2, 0xb3, 0xb3, 0xb4, 0xb5, 0xb5, 0xb6, 0xb6, 0xb7, 0xb7, 0xb7, 0xb8,
            0xb8, 0xb8, 0x19, 0x19, 0x19, 0x19, 0x6c, 0x6d, 0x6d, 0x27, 0x28, 0x29, 0x29, 0x2a, 0x2a, 0x2b,
            0x2b, 0x2d, 0x2e, 0x2f, 0x19, 0x19, 0xb5, 0xb6, 0xb7, 0xb9, 0x6f, 0x6f, 0x2a, 0x2e, 0x6f, 0x57,
            0x58, 0x58, 0x58, 0x16, 0x17, 0x18, 0x19, 0x42, 0x43, 0x44, 0x45, 0x44, 0x45, 0x45, 0x99, 0x00,
            0x44, 0x47, 0x41, 0xf2, 0xf2, 0xf3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a
        },
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0b, 0x0d, 0x0d, 0x0e,
            0x0f, 0x10, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x15, 0x16, 0x17, 0xf3, 0x19, 0xf4, 0xf4, 0xf4,
            0x1c, 0xf5, 0xf5, 0xf5, 0x1f, 0x25, 0x26, 0x26, 0x27, 0x28, 0x29, 0x29, 0x2a, 0x2a, 0xb4, 0x2d,
            0x2e, 0x2e, 0x2f, 0x2f, 0x19, 0x1a, 0x1a, 0x1a, 0x1a, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0xf5, 0x3f,
            0x3f, 0x40, 0x41, 0x42, 0x42, 0x43, 0x44, 0x45, 0x45, 0x46, 0x46, 0x47, 0x47, 0x48, 0x48, 0x48,
            0x48, 0x48, 0x49, 0x49, 0x49, 0x10, 0x55, 0x55, 0x56, 0x57, 0x58, 0x58, 0x5a, 0x5a, 0x5b, 0x5b,
            0xa3, 0xa4, 0xa4, 0xa5, 0xa6, 0xa6, 0xa7, 0xa7, 0xa8, 0xa9, 0xa9, 0xaa, 0x25, 0x6c, 0x6d, 0xc6,
            0xc7, 0x6f, 0x70, 0x70, 0xc9, 0xca, 0xca, 0x2b, 0x2c, 0x2d, 0x2d, 0x2e, 0x2e, 0x2f, 0x30, 0x1a,
            0x1b, 0x1b, 0x1b, 0x83, 0x83, 0x84, 0x85, 0x86, 0x87, 0x87, 0x88, 0x89, 0x89, 0x8a, 0x8b, 0x8b,
            0x8c, 0x8d, 0x8d, 0x8e, 0x8f, 0x8f, 0x90, 0x91, 0x98, 0x98, 0x99, 0x9a, 0x9b, 0x9b, 0x9c, 0x9c,
            0x9e, 0x9e, 0x9f, 0xa0, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa4, 0xa5, 0xa6, 0xa6, 0xa7, 0xa7, 0xaf,
            0xb0, 0xb0, 0xb1, 0xb2, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb6, 0xb7, 0xb8, 0xb8, 0xb8, 0xb9, 0xba,
            0xba, 0x32, 0x33, 0x1b, 0x1b, 0x1b, 0x6d, 0x6d, 0x6d, 0x28, 0x28, 0x29, 0x2a, 0x2b, 0x2b, 0x2c,
            0x2d, 0x2e, 0x2f, 0x31, 0x1b, 0x1b, 0xb6, 0xb7, 0xb8, 0xbb, 0x70, 0x70, 0x2b, 0x2f, 0x70, 0x57,
            0x59, 0x5a, 0x5b, 0x5b, 0x18, 0x1a, 0x1a, 0x43, 0x44, 0x45, 0x46, 0x45, 0x46, 0x46, 0x99, 0x00,
            0x00, 0x49, 0x41, 0xf2, 0xf3, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a
        },
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0d, 0x0e,
            0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x15, 0x17, 0xf3, 0x18, 0x19, 0xf4, 0xf4, 0x1b, 0xf5,
            0xf5, 0x1f, 0xaa, 0x95, 0x95, 0x25, 0x26, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2a, 0x2c, 0x2d, 0x2e,
            0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x1c, 0x1c, 0x1c, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1e, 0x3f,
            0x40, 0x41, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x46, 0x47, 0x48, 0x48, 0x49, 0x49, 0x4a, 0x4a,
            0x4b, 0x4b, 0x4c, 0x4c, 0x4c, 0x55, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5b, 0x5c, 0x5d,
            0x5d, 0xa6, 0xa6, 0x60, 0xa8, 0xa8, 0xa9, 0xaa, 0xaa, 0xab, 0xab, 0xab, 0x25, 0x6d, 0x6d, 0x6e,
            0xc8, 0x6f, 0x70, 0x71, 0xca, 0xca, 0xcb, 0x2c, 0x2d, 0x2e, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33,
            0x34, 0x1d, 0x1d, 0x83, 0x84, 0x85, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8b, 0x8c, 0x8d,
            0x8e, 0x8e, 0x90, 0x90, 0x91, 0x91, 0x92, 0x93, 0x98, 0x99, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e,
            0x9f, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa8, 0xa8, 0xa9, 0xaf,
            0xb0, 0xb1, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xba, 0xbb, 0xbb,
            0xbd, 0xbd, 0xc1, 0xc1, 0x92, 0x92, 0x6d, 0x6d, 0xc6, 0xc7, 0xc9, 0xc9, 0xc9, 0x2c, 0xcd, 0x2d,
            0x2e, 0x2f, 0x30, 0x32, 0x34, 0x1d, 0xb7, 0xb9, 0xba, 0xbd, 0x70, 0x70, 0xcd, 0x31, 0xde, 0x58,
            0x59, 0x5b, 0x5c, 0x5d, 0x1a, 0x1b, 0x1c, 0x43, 0x44, 0x46, 0x47, 0x45, 0x47, 0x47, 0x9a, 0x00,
            0x00, 0x4b, 0xf2, 0xf2, 0xf3, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a
        }
    }
};
DATA(0x004fa448) unsigned char gColorTableLighten[DIM_PALETTE_COLOR_COUNT] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
    0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x25, 0x25, 0x25, 0x25, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
    0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3f,
    0x3f, 0x3f, 0x3f, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b,
    0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x55, 0x55, 0x55, 0x55, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b,
    0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x6c, 0x6c, 0x6c, 0x6c,
    0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b,
    0x7c, 0x7d, 0x7e, 0x83, 0x83, 0x83, 0x83, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b,
    0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x98, 0x98, 0x98, 0x98, 0x98, 0x99, 0x9a, 0x9b,
    0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xaf,
    0xaf, 0xaf, 0xaf, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb,
    0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb,
    0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xde,
    0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xee,
    0xef, 0xf0, 0xf2, 0xf2, 0xf3, 0xf4, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};
DATA(0x004fa548) unsigned char gColorTableNoCycle[DIM_PALETTE_COLOR_COUNT] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xbc, 0xbc, 0xbc, 0xbc, 0x76, 0x76, 0x76, 0x76, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45,
    0x45, 0x45, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};
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
DATA(0x004fa688) unsigned char giMapSizes[KB_MAP_SIZE_COUNT] = {
    MAP_DIMENSION_SMALL, MAP_DIMENSION_MEDIUM, MAP_DIMENSION_LARGE,
    MAP_DIMENSION_XLARGE
};
DATA(0x004fa68c) int gbUseEvilInterface = 0;
DATA(0x004fa690) char
    *cEvilTranslate[KB_INTERFACE_TYPE_COUNT][KB_INTERFACE_VARIANT_COUNT] = {
    { "advbord.icn", "advborde.icn" },
    { "heroextg.icn", "heroexte.icn" },
    { "buybuild.icn", "buybuile.icn" },
    { "advbtns.icn", "advebtns.icn" },
    { "herologo.icn", "herologe.icn" },
    { "sunmoon.icn", "sunmoone.icn" },
    { "stonback.icn", "stonbake.icn" },
    { "scroll.icn", "scrolle.icn" },
    { "locators.icn", "locatore.icn" },
    { "system.icn", "systeme.icn" },
    { "CPANBKG.ICN", "CPANBKGE.ICN" },
    { "CPANEL.ICN", "CPANELE.ICN" },
    { "APANBKG.ICN", "APANBKGE.ICN" },
    { "APANEL.ICN", "APANELE.ICN" },
    { "VIEWWRLD.ICN", "EVIWWRLD.ICN" },
    { "VIEWRSRC.ICN", "EVIWRSRC.ICN" },
    { "VIEWRTFX.ICN", "EVIWRTFX.ICN" },
    { "VIEWTWNS.ICN", "EVIWTWNS.ICN" },
    { "VIEWHROS.ICN", "EVIWHROS.ICN" },
    { "VIEW_ALL.ICN", "EVIW_ALL.ICN" },
    { "VIEWMINE.ICN", "EVIWMINE.ICN" },
    { "VIEWDDOR.ICN", "EVIWDDOR.ICN" },
    { "VIEWPUZL.ICN", "EVIWPUZL.ICN" },
    { "LGNDXTRA.ICN", "LGNDXTRE.ICN" },
    { "SPANBKG.ICN", "SPANBKGE.ICN" },
    { "SPANBTN.ICN", "SPANBTNE.ICN" },
    { "CSPANBKG.ICN", "CSPANBKE.ICN" },
    { "CSPANBTN.ICN", "CSPANBTE.ICN" },
    { "TRADPOST.ICN", "TRADPOSE.ICN" },
    { "VIEWARMY.ICN", "VIEWARME.ICN" },
    { "WINLOSE.ICN", "WINLOSEE.ICN" },
    { "WINCMBTB.ICN", "WINCMBBE.ICN" },
    { "SURRENDR.ICN", "SURRENDE.ICN" },
    { "SURDRBKG.ICN", "SURDRBKE.ICN" },
    { "VGENBKG.ICN", "VGENBKGE.ICN" },
    { "campbkgg.ICN", "campbkge.ICN" },
    { "campxtrg.ICN", "campxtre.ICN" }
};
DATA(0x004fa7b8) char gcAnimPath[0x160] = "\\HEROES2\\ANIM\\";
DATA(0x004fa918) char gcGamePath[0x18] = ".\\GAMES\\";
DATA(0x004fa930) char gcMapPath[0x14] = ".\\MAPS\\";
DATA(0x004fa944) int gbPutzingWithMouseCtr = 0;
DATA(0x004fa948) int gbDontTryRedbook = 0;
DATA(0x004fa94c) int gbDontTryMIDI = 0;
DATA(0x004fa950) int gbDontTryDigital = 0;
DATA(0x004fa958) float gfCombatSpeedMod[KB_COMBAT_SPEED_COUNT] = {
    1.0f, 0.7f, 0.35f
};
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
DATA(0x004fa990) char *gCombatFxNames[KB_COMBAT_FX_COUNT] = {
    "", "magic01.icn", "magic02.icn", "magic03.icn",
    "magic04.icn", "magic05.icn", "magic06.icn", "magic07.icn",
    "magic08.icn", "rainbluk.icn", "cloudluk.icn", "moraleg.icn",
    "moraleb.icn", "reddeath.icn", "redfire.icn", "sparks.icn",
    "electric.icn", "physical.icn", "bluefire.icn", "icecloud.icn",
    "lichclod.icn", "bless.icn", "berzerk.icn", "shield.icn",
    "haste.icn", "paralyze.icn", "hypnotiz.icn", "dragslay.icn",
    "blind.icn", "curse.icn", "stonskin.icn", "stelskin.icn"
};
DATA(0x004faa10) short horseFrameFlip[MOVEMENT_FRAME_FLIP_COUNT] = {
    45, 46, 47, 48, 49, 50, 51, 52, 53, 179, 178, 177, 54, 175, 174, 55
};
DATA(0x004faa30) short boatFrameFlip[MOVEMENT_FRAME_FLIP_COUNT] = {
    0, 0, 9, 9, 18, 18, 27, 27, 36, 36, 155, 155, 146, 146, 137, 137
};
DATA(0x004faa50) signed char gCastleResources[CASTLE_RESOURCE_SLOT_COUNT] = {
    RES_WOOD, RES_ORE, -1, -1, 0, 0, 0, 0
};
DATA(0x004faa58) short gCastleAmounts[CASTLE_AMOUNT_COUNT] = { 20, 20, 0, 0 };
DATA(0x004faa60) int gHeroGoldCost = 2500;
DATA(0x004faa68) short gVesaMode[VESA_MODE_VALUE_COUNT] = {
    640, 480, 256, VESA_SET_MODE_FUNCTION, VESA_MODE_640_480_256, 0, 0, 0
};
DATA(0x004faa78) tag_tilePoint normalDirTable[NORMAL_DIRECTION_COUNT] = {
    { 0, -1, 16 }, { 1, -1, 16 }, { 1, 0, 16 },  { 1, 1, 16 },
    { 0, 1, 16 },  { -1, 1, 16 }, { -1, 0, 16 }, { -1, -1, 16 }
};
DATA(0x004faa98) int gResourceBaseValue[RESOURCE_VALUE_COUNT] = {
    200, 300, 200, 300, 300, 300, 1, 0
};
DATA(0x004faab8) int
    gInitResourcesHuman[STARTING_RESOURCE_DIFFICULTY_COUNT]
                       [STARTING_RESOURCE_TYPE_COUNT] = {
        { 30, 10, 30, 10, 10, 10, 10000 },
        { 20, 5, 20, 5, 5, 5, 7500 },
        { 10, 2, 10, 2, 2, 2, 5000 },
        { 5, 0, 5, 0, 0, 0, 2500 },
        { 0, 0, 0, 0, 0, 0, 0 }
    };
DATA(0x004fab48) int
    gInitResourcesComputer[STARTING_RESOURCE_DIFFICULTY_COUNT]
                          [STARTING_RESOURCE_TYPE_COUNT] = {
        { 20, 5, 20, 5, 5, 5, 7500 },
        { 20, 5, 20, 5, 5, 5, 7500 },
        { 30, 10, 30, 10, 10, 10, 10000 },
        { 30, 10, 30, 10, 10, 10, 10000 },
        { 30, 10, 30, 10, 10, 10, 10000 }
    };
DATA(0x004fabd8) int gMineCharacteristics[MINE_CHARACTERISTIC_COUNT] = {
    2, 1, 2, 1, 1, 1, 1000, 0
};
DATA(0x004fabf8) int gSSValues[HERO_SKILL_COUNT]
                                 [SECONDARY_SKILL_VALUE_LEVEL_COUNT] = {
    { 400, 750, 1000 }, { 200, 450, 850 }, { 450, 1000, 1675 },
    { 1, 2, 3 },        { 50, 100, 150 },  { 150, 275, 375 },
    { 300, 550, 800 },  { 300, 600, 900 }, { 250, 425, 650 },
    { 300, 550, 800 },  { 100, 200, 300 }, { 50, 100, 150 },
    { 100, 450, 950 },  { 445, 950, 1500 }
};
DATA(0x004faca0) unsigned char gArtifactLevel[KB_ARTIFACT_LEVEL_COUNT] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x04, 0x04, 0x02, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x02, 0x04, 0x02, 0x04, 0x02, 0x04, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x04, 0x02, 0x02, 0x08, 0x08, 0x08, 0x08, 0x02, 0x04, 0x04, 0x08, 0x04, 0x04, 0x08, 0x08, 0x08,
    0x04, 0x04, 0x04, 0x08, 0x04, 0x04, 0x04, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x02, 0x08,
    0x08, 0x08, 0x02, 0x04, 0x04, 0x08, 0x08, 0x02, 0x02, 0x02, 0x04, 0x04, 0x02, 0x04, 0x02, 0x04,
    0x02, 0x10, 0x20, 0x20, 0x20, 0x20, 0x02, 0x08, 0x02, 0x08, 0x02, 0x02, 0x08, 0x08, 0x02, 0x02,
    0x02, 0x04, 0x02, 0x02, 0x02, 0x02, 0x04, 0x00
};
DATA(0x004fad08) int gArtifactBaseRV[KB_ARTIFACT_BASE_VALUE_COUNT] = {
    13600, 22000, 18000, 14000, 19000, 18500, 22200, 25000,
    6000, 4000, 4000, 5600, 1200, 1200, 1200, 1200,
    -1200, 2000, 1800, 1800, 2000, 1000, 3600, 5600,
    4000, 5040, 3060, 4420, 5610, 6630, 7000, 6000,
    4000, 4500, 2250, 1200, 1200, 1200, 1200, 3500,
    1500, 500, 1000, 750, 500, 750, 750, 750,
    750, 750, 750, 500, 1500, 1000, 1000, 750,
    750, 750, 750, 750, 750, 750, 1250, 750,
    150, 250, 3500, 1250, 250, -1, -1, 4000,
    4000, 4000, 2000, 2000, 4000, 3800, 7600, 3700,
    7400, 0, 0, 0, 0, 0, 2500, 4800,
    9000, -3250, 36200, 2000, -1050, -1050, 10000, 10000,
    15000, 720, 7500, 10000, 9200, 10000, 1500
};
DATA(0x004faea4) int gUltArtifactAvgValue = 16500;
DATA(0x004faea8) int giDebugLevel = 0;
DATA(0x004faeac) signed char giVisRangeTown = 5;
DATA(0x004faeb0) tag_monsterInfo gMonsterDatabase[MONSTER_DATABASE_COUNT] = {
    { { 20, 33 }, 17, 12, 1, 0, 2, 1, 1, 1, 1, 0, "psnt", 0 },
    { { 150, 312 }, 21, 8, 10, 0, 2, 5, 3, 2, 3, 12, "arch", MONSTER_ATTRIBUTE_RANGED },
    { { 200, 463 }, 23, 8, 10, 0, 4, 5, 3, 2, 3, 24, "arch", MONSTER_ATTRIBUTE_RANGED },
    { { 200, 639 }, 32, 5, 15, 0, 4, 5, 9, 3, 4, 0, "pike", 0 },
    { { 250, 824 }, 33, 5, 20, 0, 5, 5, 9, 3, 4, 0, "pike", 0 },
    { { 250, 1130 }, 45, 4, 25, 0, 4, 7, 9, 4, 6, 0, "swdm", 0 },
    { { 300, 1350 }, 45, 4, 30, 0, 5, 7, 9, 4, 6, 0, "swdm", 0 },
    { { 300, 1830 }, 61, 3, 30, 0, 6, 10, 9, 5, 10, 0, "cavl", MONSTER_ATTRIBUTE_WIDE },
    { { 375, 2273 }, 61, 3, 40, 0, 7, 10, 9, 5, 10, 0, "cavl", MONSTER_ATTRIBUTE_WIDE },
    { { 600, 4704 }, 78, 2, 50, 0, 5, 11, 12, 10, 20, 0, "pldn", 0 },
    { { 1000, 5822 }, 58, 2, 65, 0, 6, 11, 12, 10, 20, 0, "pldn", 0 },
    { { 40, 109 }, 27, 10, 3, 1, 4, 3, 1, 1, 2, 0, "gbln", 0 },
    { { 140, 299 }, 21, 8, 10, 1, 2, 3, 4, 2, 3, 8, "elf_", MONSTER_ATTRIBUTE_RANGED },
    { { 175, 512 }, 29, 8, 15, 1, 3, 3, 4, 3, 4, 16, "elf_", MONSTER_ATTRIBUTE_RANGED },
    { { 200, 865 }, 43, 5, 20, 1, 6, 6, 2, 3, 5, 0, "wolf", MONSTER_ATTRIBUTE_WIDE },
    { { 300, 1065 }, 36, 4, 40, 1, 2, 9, 5, 4, 6, 0, "ogre", 0 },
    { { 500, 2070 }, 41, 4, 60, 1, 4, 9, 5, 5, 7, 0, "ogre", 0 },
    { { 600, 1921 }, 32, 3, 40, 1, 4, 10, 5, 5, 7, 8, "trll", MONSTER_ATTRIBUTE_RANGED },
    { { 700, 2337 }, 33, 3, 40, 1, 5, 10, 5, 7, 9, 16, "trll", MONSTER_ATTRIBUTE_RANGED },
    { { 750, 6074 }, 58, 2, 80, 1, 5, 12, 9, 12, 24, 0, "cycl", MONSTER_ATTRIBUTE_TWO_HEX_ATTACKER },
    { { 50, 129 }, 26, 8, 2, 2, 4, 4, 2, 1, 2, 0, "sprt", MONSTER_ATTRIBUTE_FLYING },
    { { 200, 500 }, 25, 6, 20, 2, 2, 6, 5, 2, 4, 0, "dwrf", 0 },
    { { 250, 716 }, 29, 6, 20, 2, 4, 6, 6, 2, 4, 0, "dwrf", 0 },
    { { 250, 554 }, 22, 4, 15, 2, 4, 4, 3, 2, 3, 24, "elf_", MONSTER_ATTRIBUTE_RANGED },
    { { 300, 658 }, 22, 4, 15, 2, 6, 5, 5, 2, 3, 24, "elf_", MONSTER_ATTRIBUTE_RANGED },
    { { 350, 1290 }, 37, 3, 25, 2, 5, 7, 5, 5, 8, 8, "drui", MONSTER_ATTRIBUTE_RANGED },
    { { 400, 1428 }, 36, 3, 25, 2, 6, 7, 7, 5, 8, 16, "drui", MONSTER_ATTRIBUTE_RANGED },
    { { 500, 2702 }, 54, 2, 40, 2, 5, 10, 9, 7, 14, 0, "unic", MONSTER_ATTRIBUTE_WIDE },
    { { 1500, 10114 }, 56, 1, 100, 2, 7, 12, 10, 20, 40, 0, "phoe", MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_TWO_HEX_ATTACKER },
    { { 60, 154 }, 26, 8, 5, 3, 4, 3, 1, 1, 2, 8, "cntr", MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_RANGED },
    { { 200, 579 }, 29, 6, 15, 3, 6, 4, 7, 2, 3, 0, "garg", MONSTER_ATTRIBUTE_FLYING },
    { { 300, 1101 }, 37, 4, 25, 3, 4, 6, 6, 3, 5, 0, "grif", MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING },
    { { 400, 1751 }, 44, 3, 35, 3, 4, 9, 8, 5, 10, 0, "mino", 0 },
    { { 500, 2252 }, 45, 3, 45, 3, 6, 9, 8, 5, 10, 0, "mino", 0 },
    { { 800, 2878 }, 36, 2, 75, 3, 2, 8, 9, 6, 12, 0, "hydr", MONSTER_ATTRIBUTE_WIDE },
    { { 3000, 18153 }, 55, 1, 200, 3, 4, 12, 12, 25, 50, 0, "drgn", MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_TWO_HEX_ATTACKER },
    { { 3500, 22962 }, 68, 1, 250, 3, 5, 13, 13, 25, 50, 0, "drgn", MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_TWO_HEX_ATTACKER },
    { { 4000, 28144 }, 74, 1, 300, 3, 6, 14, 14, 25, 50, 0, "drgn", MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_TWO_HEX_ATTACKER },
    { { 50, 134 }, 27, 8, 3, 4, 3, 2, 1, 1, 3, 12, "half", MONSTER_ATTRIBUTE_RANGED },
    { { 150, 493 }, 33, 6, 15, 4, 6, 5, 4, 2, 3, 0, "boar", MONSTER_ATTRIBUTE_WIDE },
    { { 300, 951 }, 19, 4, 30, 4, 2, 5, 10, 4, 5, 0, "golm", 0 },
    { { 350, 1324 }, 24, 4, 35, 4, 3, 7, 10, 4, 5, 0, "golm", 0 },
    { { 400, 1739 }, 43, 3, 40, 4, 4, 7, 7, 4, 8, 0, "roc_", MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING },
    { { 600, 1935 }, 32, 2, 30, 4, 5, 11, 7, 7, 9, 12, "mage", MONSTER_ATTRIBUTE_RANGED },
    { { 700, 2469 }, 35, 2, 35, 4, 6, 12, 8, 7, 9, 24, "mage", MONSTER_ATTRIBUTE_RANGED },
    { { 2000, 9589 }, 42, 1, 150, 4, 4, 13, 10, 20, 30, 0, "titn", 0 },
    { { 5000, 22933 }, 79, 1, 300, 4, 6, 15, 15, 20, 30, 24, "titn", MONSTER_ATTRIBUTE_RANGED },
    { { 75, 203 }, 27, 8, 4, 5, 4, 4, 3, 2, 3, 0, "skel", MONSTER_ATTRIBUTE_UNDEAD },
    { { 150, 310 }, 21, 6, 15, 5, 2, 5, 2, 2, 3, 0, "zomb", MONSTER_ATTRIBUTE_UNDEAD },
    { { 200, 506 }, 25, 6, 20, 5, 4, 5, 2, 2, 3, 0, "zomb", MONSTER_ATTRIBUTE_UNDEAD },
    { { 250, 868 }, 35, 4, 25, 5, 4, 6, 6, 3, 4, 0, "mumy", MONSTER_ATTRIBUTE_UNDEAD },
    { { 300, 1056 }, 35, 4, 30, 5, 5, 6, 6, 3, 4, 0, "mumy", MONSTER_ATTRIBUTE_UNDEAD },
    { { 500, 1685 }, 42, 3, 30, 5, 4, 8, 6, 5, 7, 0, "vamp", MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_UNDEAD },
    { { 650, 2461 }, 45, 3, 40, 5, 5, 8, 6, 5, 7, 0, "vamp", MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_UNDEAD },
    { { 750, 2069 }, 28, 2, 25, 5, 5, 7, 12, 8, 10, 12, "lich", MONSTER_ATTRIBUTE_RANGED | MONSTER_ATTRIBUTE_UNDEAD },
    { { 900, 2625 }, 29, 2, 35, 5, 6, 7, 13, 8, 10, 24, "lich", MONSTER_ATTRIBUTE_RANGED | MONSTER_ATTRIBUTE_UNDEAD },
    { { 1500, 11744 }, 78, 1, 150, 5, 4, 11, 9, 25, 45, 0, "drgn", MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_UNDEAD },
    { { 50, 177 }, 35, 12, 4, 6, 5, 6, 1, 1, 2, 0, "rogu", 0 },
    { { 200, 805 }, 40, 4, 20, 6, 6, 7, 6, 2, 5, 0, "nmad", MONSTER_ATTRIBUTE_WIDE },
    { { 1000, 1545 }, 62, 3, 20, 6, 5, 8, 7, 4, 6, 0, "ghst", MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_UNDEAD },
    { { 650, 5692 }, 60, 2, 50, 6, 6, 10, 9, 20, 30, 0, "geni", MONSTER_ATTRIBUTE_FLYING },
    { { 500, 1979 }, 40, 5, 35, 6, 4, 8, 9, 6, 10, 0, "meds", MONSTER_ATTRIBUTE_WIDE },
    { { 500, 1732 }, 35, 3, 50, 6, 3, 8, 8, 4, 5, 0, "eelm", 0 },
    { { 500, 1412 }, 28, 3, 35, 6, 6, 7, 7, 2, 8, 0, "aelm", 0 },
    { { 500, 1501 }, 30, 3, 40, 6, 5, 8, 6, 4, 6, 0, "felm", 0 },
    { { 500, 1690 }, 34, 3, 45, 6, 4, 6, 8, 3, 7, 0, "welm", 0 }
};
DATA(0x004fb568) float gfStatPower[KB_STAT_POWER_COUNT] = {
    0.5f, 0.5f, 0.5f, 0.5f, 0.52f, 0.54f,
    0.56f, 0.58f, 0.6f, 0.62f, 0.64f, 0.67f,
    0.7f, 0.74f, 0.77f, 0.8f, 0.84f, 0.88f,
    0.92f, 0.96f, 1.0f, 1.04f, 1.08f, 1.12f,
    1.16f, 1.2f, 1.24f, 1.28f, 1.32f, 1.36f,
    1.4f, 1.44f, 1.48f, 1.52f, 1.56f, 1.6f,
    1.64f, 1.68f, 1.72f, 1.76f, 1.8f, 0.0f
};
DATA(0x004fb610) float gfBattleStat[KB_STAT_POWER_COUNT] = {
    0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.25f,
    0.3f, 0.35f, 0.4f, 0.45f, 0.5f, 0.55f,
    0.6f, 0.65f, 0.7f, 0.75f, 0.8f, 0.85f,
    0.9f, 0.95f, 1.0f, 1.1f, 1.2f, 1.3f,
    1.4f, 1.5f, 1.6f, 1.7f, 1.8f, 1.9f,
    2.0f, 2.1f, 2.2f, 2.3f, 2.4f, 2.5f,
    2.6f, 2.7f, 2.8f, 2.9f, 3.0f, 0.0f
};
DATA(0x004fb6b8) signed char gSpellLimits[KB_SPELL_LIMIT_COUNT] = {
    3, 3, 2, 2, 1
};
DATA(0x004fb6c0) float gfSpellCastableCombatMod[KB_SPELL_MOD_COUNT] = {
    0.27f, 0.4f, 0.48f, 0.56f, 0.64f, 0.73f,
    0.81f, 0.88f, 0.93f, 0.97f, 1.0f, 0.0f
};
DATA(0x004fb6f0) float gfSpellCastNumMod[KB_SPELL_MOD_COUNT] = {
    0.0f, 1.0f, 1.75f, 2.35f, 2.85f, 3.35f,
    3.7f, 4.0f, 4.26f, 4.5f, 4.7f, 0.0f
};
DATA(0x004fb720) float gfPhilAISpellPowerMod[KB_SPELL_MOD_COUNT] = {
    0.0f, 1.0f, 1.75f, 2.5f, 3.1f, 3.5f,
    4.0f, 4.5f, 5.4f, 5.75f, 6.15f, 0.0f
};
DATA(0x004fb750) float gfPhilAIDurationMod[KB_SPELL_MOD_COUNT] = {
    0.0f, 0.4f, 0.65f, 0.8f, 1.0f, 1.16f,
    1.3f, 1.43f, 1.54f, 1.64f, 1.74f, 0.0f
};
DATA(0x004fb780) float gfSpellTypeNumMod[KB_QUICK_COMBAT_SPELL_TYPE_COUNT] = {
    1.0f, 0.75f, 0.55f, 0.4f, 0.28f, 0.2f, 0.15f
};
DATA(0x004fb79c) int gbDrawSavedCursor = 0;
DATA(0x004fb7a0) signed char gbArrow[NORMAL_DIRECTION_COUNT][NORMAL_DIRECTION_COUNT] = {
    { 8, 0, 0, 0, 8, 16, 16, 16 },
    { 17, 9, 1, 1, 1, 9, 17, 17 },
    { 18, 18, 10, 2, 2, 2, 10, 18 },
    { 19, 19, 19, 11, 3, 3, 3, 11 },
    { 12, 20, 20, 20, 12, 4, 4, 4 },
    { 5, 13, 21, 21, 21, 13, 5, 5 },
    { 6, 6, 14, 22, 22, 22, 14, 6 },
    { 7, 7, 7, 15, 23, 23, 23, 15 }
};
DATA(0x004fb7e0) unsigned char giCloudType[KB_CLOUD_MASK_COUNT] = {
    0x0b, 0x07, 0x08, 0x81, 0x09, 0x0a, 0x80, 0x21, 0x6c, 0x1d, 0x1e, 0x20, 0x1c, 0x85, 0x22, 0x16,
    0x0b, 0x07, 0x08, 0x71, 0x09, 0x0a, 0x80, 0x7e, 0x6c, 0x1d, 0x1e, 0x83, 0x1c, 0x85, 0x22, 0x78,
    0x0b, 0x07, 0x08, 0x81, 0x09, 0x0a, 0x70, 0x7f, 0x6c, 0x1d, 0x1e, 0x20, 0x1c, 0x85, 0x7d, 0x79,
    0x0b, 0x07, 0x08, 0x71, 0x09, 0x0a, 0x70, 0x67, 0x6c, 0x1d, 0x1e, 0x83, 0x1c, 0x85, 0x7d, 0x75,
    0x0b, 0x07, 0x08, 0x81, 0x09, 0x0a, 0x80, 0x21, 0x6c, 0x1d, 0x1e, 0x20, 0x0c, 0x1b, 0x19, 0x15,
    0x0b, 0x07, 0x08, 0x71, 0x09, 0x0a, 0x80, 0x7e, 0x6c, 0x1d, 0x1e, 0x83, 0x0c, 0x1b, 0x19, 0x76,
    0x0b, 0x07, 0x08, 0x81, 0x09, 0x0a, 0x70, 0x7f, 0x6c, 0x1d, 0x1e, 0x20, 0x0c, 0x1b, 0x01, 0x13,
    0x0b, 0x07, 0x08, 0x71, 0x09, 0x0a, 0x72, 0x67, 0x6c, 0x1d, 0x1e, 0x83, 0x0c, 0x1b, 0x01, 0x74,
    0x0b, 0x07, 0x08, 0x81, 0x09, 0x0a, 0x80, 0x21, 0x6c, 0x0d, 0x1e, 0x1f, 0x1c, 0x1a, 0x22, 0x14,
    0x0b, 0x07, 0x08, 0x71, 0x09, 0x0a, 0x80, 0x7e, 0x6c, 0x0d, 0x1e, 0x05, 0x1c, 0x1a, 0x22, 0x18,
    0x0b, 0x07, 0x08, 0x81, 0x09, 0x0a, 0x70, 0x7f, 0x6c, 0x0d, 0x1e, 0x1f, 0x1c, 0x1a, 0x7d, 0x12,
    0x0b, 0x07, 0x08, 0x73, 0x09, 0x0a, 0x70, 0x67, 0x6c, 0x0d, 0x1e, 0x05, 0x1c, 0x1a, 0x7d, 0x7b,
    0x0b, 0x07, 0x08, 0x81, 0x09, 0x0a, 0x80, 0x21, 0x6c, 0x0d, 0x1e, 0x1f, 0x0c, 0x03, 0x19, 0x11,
    0x0b, 0x07, 0x08, 0x71, 0x09, 0x0a, 0x80, 0x7e, 0x6c, 0x0f, 0x1e, 0x05, 0x0c, 0x03, 0x19, 0x17,
    0x0b, 0x07, 0x08, 0x81, 0x09, 0x0a, 0x70, 0x7f, 0x6c, 0x0d, 0x1e, 0x1f, 0x0e, 0x03, 0x01, 0x10,
    0x0b, 0x07, 0x08, 0x73, 0x09, 0x0a, 0x72, 0x67, 0x6c, 0x0f, 0x1e, 0x05, 0x0e, 0x03, 0x01, 0x00
};
DATA(0x004fb8e0) short giScoreMon[MONSTER_DATABASE_COUNT][MONSTER_SCORE_FIELD_COUNT] = {
    { 0, 0 }, { 4, 11 }, { 8, 20 }, { 12, 38 }, { 16, 29 }, { 20, 57 },
    { 24, 47 }, { 28, 12 }, { 32, 48 }, { 36, 1 }, { 40, 2 }, { 44, 39 },
    { 48, 21 }, { 52, 49 }, { 56, 13 }, { 60, 23 }, { 64, 30 }, { 68, 3 },
    { 72, 24 }, { 76, 22 }, { 80, 58 }, { 84, 4 }, { 88, 14 }, { 92, 50 },
    { 96, 40 }, { 100, 51 }, { 104, 15 }, { 108, 31 }, { 112, 5 }, { 116, 25 },
    { 120, 41 }, { 124, 6 }, { 128, 63 }, { 132, 26 }, { 135, 64 }, { 138, 59 },
    { 141, 52 }, { 144, 65 }, { 147, 62 }, { 150, 42 }, { 153, 32 }, { 156, 7 },
    { 159, 17 }, { 162, 43 }, { 165, 61 }, { 168, 54 }, { 171, 16 }, { 174, 33 },
    { 177, 8 }, { 180, 18 }, { 183, 53 }, { 186, 44 }, { 189, 55 }, { 192, 27 },
    { 195, 34 }, { 198, 9 }, { 201, 60 }, { 204, 10 }, { 207, 19 }, { 210, 45 },
    { 213, 28 }, { 216, 56 }, { 219, 35 }, { 222, 36 }, { 225, 46 }, { 228, 37 }
};
DATA(0x004fb9e8) short giScoreCampaignMon[MONSTER_DATABASE_COUNT][MONSTER_SCORE_FIELD_COUNT] = {
    { 9999, 0 }, { 5800, 11 }, { 5600, 20 }, { 5400, 38 }, { 5200, 29 }, { 5000, 57 },
    { 4800, 47 }, { 4600, 12 }, { 4400, 48 }, { 4200, 1 }, { 4000, 2 }, { 3800, 39 },
    { 3600, 21 }, { 3400, 49 }, { 3200, 13 }, { 3000, 23 }, { 2800, 30 }, { 2600, 3 },
    { 2400, 24 }, { 2200, 22 }, { 2000, 58 }, { 1900, 4 }, { 1800, 14 }, { 1700, 50 },
    { 1600, 40 }, { 1500, 51 }, { 1400, 15 }, { 1300, 31 }, { 1200, 5 }, { 1100, 25 },
    { 1000, 41 }, { 980, 6 }, { 960, 63 }, { 940, 26 }, { 920, 64 }, { 900, 59 },
    { 880, 52 }, { 860, 65 }, { 840, 62 }, { 820, 42 }, { 800, 32 }, { 780, 7 },
    { 760, 17 }, { 740, 43 }, { 720, 61 }, { 700, 54 }, { 680, 16 }, { 660, 33 },
    { 640, 8 }, { 620, 18 }, { 600, 53 }, { 580, 44 }, { 560, 55 }, { 540, 27 },
    { 520, 34 }, { 500, 9 }, { 480, 60 }, { 460, 10 }, { 440, 19 }, { 420, 45 },
    { 400, 28 }, { 380, 56 }, { 360, 35 }, { 340, 36 }, { 320, 46 }, { 300, 37 }
};
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
DATA(0x004fbaf8) signed char
    gHeroSkillBonus[HERO_CLASS_COUNT][KB_HERO_LEVEL_BAND_COUNT][HERO_PRIMARY_STAT_COUNT] = {
        { { 35, 45, 10, 10 }, { 25, 25, 25, 25 } },
        { { 55, 35, 5, 5 }, { 25, 25, 25, 25 } },
        { { 10, 10, 30, 50 }, { 20, 20, 30, 30 } },
        { { 10, 10, 50, 30 }, { 20, 20, 30, 30 } },
        { { 10, 10, 40, 40 }, { 20, 20, 30, 30 } },
        { { 15, 15, 35, 35 }, { 25, 25, 25, 25 } }
    };
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
DATA(0x004fbd60) char *cMonFilename[MONSTER_DATABASE_COUNT] = {
    "peasant.icn", "archer.icn", "archer2.icn", "pikeman.icn",
    "pikeman2.icn", "swordsmn.icn", "swordsm2.icn", "cavalryr.icn",
    "cavalryb.icn", "paladin.icn", "paladin2.icn", "goblin.icn",
    "orc.icn", "orc2.icn", "wolf.icn", "ogre.icn",
    "ogre2.icn", "troll.icn", "troll2.icn", "cyclops.icn",
    "sprite.icn", "dwarf.icn", "dwarf2.icn", "elf.icn",
    "elf2.icn", "druid.icn", "druid2.icn", "unicorn.icn",
    "phoenix.icn", "centaur.icn", "gargoyle.icn", "griffin.icn",
    "minotaur.icn", "minotau2.icn", "hydra.icn", "draggree.icn",
    "dragred.icn", "dragblak.icn", "halfling.icn", "boar.icn",
    "golem.icn", "golem2.icn", "roc.icn", "mage1.icn",
    "mage2.icn", "titanblu.icn", "titanbla.icn", "skeleton.icn",
    "zombie.icn", "zombie2.icn", "mummyw.icn", "mummy2.icn",
    "vampire.icn", "vampire2.icn", "lich.icn", "lich2.icn",
    "dragbone.icn", "rogue.icn", "nomad.icn", "ghost.icn",
    "genie.icn", "medusa.icn", "eelem.icn", "aelem.icn",
    "felem.icn", "welem.icn"
};
DATA(0x004fbe68) int gbProcessingCombatAction = 0;
DATA(0x004fbe6c) int iMPNetProtocol = 0;
DATA(0x004fbe70) int iLastDiffSendTo = -2;
DATA(0x004fbe78) SSpellInfo gsSpellInfo[HERO_SPELL_COUNT] = {
    { "fireball", 3, 8, 0, 500, 9, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "fireball", 4, 9, 0, 750, 15, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "lightblt", 2, 4, 15, 450, 7, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "chainlte", 4, 5, 15, 650, 15, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "telptout", 3, 10, 0, 30, 9, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_COMBAT },
    { "cure", 1, 6, 1, 250, 6, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_COMBAT },
    { "masscure", 4, 6, 1, 700, 15, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_COMBAT },
    { "resurect", 4, 13, 1, 400, 12, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "resurtru", 5, 12, 1, 550, 15, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "haste", 1, 14, 24, 300, 3, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "masshast", 3, 14, 24, 950, 10, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "slow", 1, 1, 2, 350, 3, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "massslow", 4, 1, 2, 1000, 15, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "blind", 2, 21, 28, 500, 6, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "bless", 1, 7, 21, 200, 3, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "massbles", 3, 7, 21, 650, 12, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "stonskin", 1, 31, 30, 300, 3, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "stelskin", 2, 30, 31, 575, 6, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "curse", 1, 3, 29, 200, 3, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "masscurs", 3, 3, 29, 500, 12, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "masscurs", 3, 22, 8, 300, 9, { 10, 10, 10, 10, 10, 0 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "masscurs", 4, 23, 8, 450, 12, { 10, 10, 10, 10, 10, 0 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "antimagk", 3, 17, 6, 100, 7, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "dipmagk", 1, 18, 7, 275, 5, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_COMBAT },
    { "dipmagk", 3, 18, 7, 450, 12, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_COMBAT },
    { "magcarow", 1, 38, 0, 200, 3, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "berzerk", 4, 19, 22, 1000, 12, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "armgedn", 5, 16, 0, 2000, 20, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "storm", 4, 11, 0, 1300, 15, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "meteor~1", 4, 24, 0, 1000, 15, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "paralize", 3, 20, 25, 700, 9, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "hypnotiz", 5, 37, 26, 1300, 15, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "coldray", 2, 36, 19, 375, 6, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "coldring", 3, 35, 0, 500, 9, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "disruptr", 2, 34, 0, 500, 7, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "mnrdeath", 2, 29, 13, 300, 6, { 0, 0, 0, 0, 0, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "mnrdeath", 3, 28, 13, 450, 10, { 0, 0, 0, 0, 0, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "drgnslay", 2, 32, 27, 100, 6, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "bloodlus", 1, 27, 0, 300, 3, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "resurect", 3, 25, 6, 400, 10, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "mirrorim", 5, 26, 0, 800, 25, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "shield", 1, 15, 23, 150, 3, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "shield", 4, 15, 23, 375, 7, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION },
    { "sumnelm", 5, 56, 0, 1000, 30, { 0, 0, 0, 0, 0, 0 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "sumnelm", 5, 57, 0, 1000, 30, { 0, 0, 0, 0, 0, 0 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "sumnelm", 5, 58, 0, 1000, 30, { 0, 0, 0, 0, 0, 0 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "sumnelm", 5, 59, 0, 1000, 30, { 0, 0, 0, 0, 0, 0 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "erthquak", 3, 33, 0, 300, 15, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT },
    { "", 1, 39, 0, 30, 1, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_ADVENTURE },
    { "", 1, 40, 0, 30, 1, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_ADVENTURE },
    { "", 2, 41, 0, 30, 2, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_ADVENTURE },
    { "", 3, 42, 0, 30, 2, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_ADVENTURE },
    { "", 3, 43, 0, 30, 2, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_ADVENTURE },
    { "", 4, 44, 0, 30, 3, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_ADVENTURE },
    { "", 3, 45, 0, 30, 3, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_ADVENTURE },
    { "", 2, 46, 0, 30, 5, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_ADVENTURE },
    { "", 5, 47, 0, 1500, 10, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_ADVENTURE },
    { "", 4, 48, 0, 30, 10, { 10, 10, 10, 10, 10, 10 }, SPELL_INFO_ATTRIBUTE_ADVENTURE },
    { "", 5, 49, 0, 30, 20, { 0, 0, 0, 0, 0, 0 }, SPELL_INFO_ATTRIBUTE_ADVENTURE },
    { "", 2, 50, 0, 30, 6, { 0, 0, 0, 0, 0, 0 }, SPELL_INFO_ATTRIBUTE_ADVENTURE },
    { "", 2, 51, 0, 30, 8, { 0, 0, 0, 0, 0, 0 }, SPELL_INFO_ATTRIBUTE_ADVENTURE },
    { "", 4, 52, 0, 700, 15, { 0, 0, 0, 0, 0, 0 }, SPELL_INFO_ATTRIBUTE_ADVENTURE },
    { "", 4, 53, 0, 700, 15, { 0, 0, 0, 0, 0, 0 }, SPELL_INFO_ATTRIBUTE_ADVENTURE },
    { "", 4, 54, 0, 700, 15, { 0, 0, 0, 0, 0, 0 }, SPELL_INFO_ATTRIBUTE_ADVENTURE },
    { "", 4, 55, 0, 700, 15, { 0, 0, 0, 0, 0, 0 }, SPELL_INFO_ATTRIBUTE_ADVENTURE }
};
DATA(0x004fc410) char *cArmyFrameFileNames[MONSTER_DATABASE_COUNT] = {
    "peas_frm.bin", "archrfrm.bin", "archrfrm.bin", "pikmnfrm.bin",
    "pikmnfrm.bin", "swrdsfrm.bin", "swrdsfrm.bin", "cvlryfrm.bin",
    "cvlr2frm.bin", "paladfrm.bin", "paladfrm.bin", "goblnfrm.bin",
    "orc__frm.bin", "orc__frm.bin", "wolf_frm.bin", "ogre_frm.bin",
    "ogre_frm.bin", "trollfrm.bin", "trollfrm.bin", "cyclofrm.bin",
    "spritfrm.bin", "dwarffrm.bin", "dwarffrm.bin", "elf__frm.bin",
    "elf__frm.bin", "druidfrm.bin", "druidfrm.bin", "unicofrm.bin",
    "phoenfrm.bin", "centrfrm.bin", "garglfrm.bin", "grifffrm.bin",
    "minotfrm.bin", "minotfrm.bin", "hydrafrm.bin", "draggfrm.bin",
    "dragrfrm.bin", "dragbfrm.bin", "halflfrm.bin", "boar_frm.bin",
    "golemfrm.bin", "golemfrm.bin", "roc__frm.bin", "mage1frm.bin",
    "mage1frm.bin", "titanfrm.bin", "tita2frm.bin", "skel_frm.bin",
    "zomb_frm.bin", "zomb_frm.bin", "mummyfrm.bin", "mummyfrm.bin",
    "vampifrm.bin", "vampifrm.bin", "lich_frm.bin", "lich_frm.bin",
    "drabnfrm.bin", "roguefrm.bin", "nomadfrm.bin", "ghostfrm.bin",
    "geniefrm.bin", "medusfrm.bin", "felemfrm.bin", "felemfrm.bin",
    "felemfrm.bin", "felemfrm.bin"
};
DATA(0x004fc518) unsigned char gcSpellInfluenceIcons[KB_SPELL_INFLUENCE_MAP_COUNT] = {
    0x06, 0x07, 0x08, 0x09, 0x0b, 0x0f, 0x13, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x0d, 0x0a, 0x0a, 0x00
};
DATA(0x004fc528) unsigned char giSpellInfluenceToSpell[KB_SPELL_INFLUENCE_MAP_COUNT] = {
    0x09, 0x0b, 0x0d, 0x0e, 0x12, 0x1a, 0x1e, 0x1f,
    0x25, 0x26, 0x29, 0x65, 0x16, 0x10, 0x11, 0x00
};
DATA(0x004fc538) unsigned char giNumPowFrames[KB_SPELL_EFFECT_COUNT] = {
    10, 10, 10, 10, 10, 10, 10, 10,
    10, 8, 8, 10, 10, 10, 10, 15,
    10, 10, 10, 10, 10, 16, 16, 14,
    19, 22, 10, 17, 10, 12, 11, 16
};
DATA(0x004fc558) int giSpellEffectShowType = 2;
DATA(0x004fc560) signed char gcColorToPlayerPos[RADAR_OWNER_COLOR_COUNT] = {
    0, 1, 2, 3, 4, 5, 0, 0
};
DATA(0x004fc568) char *cCombatBkgNames[KB_COMBAT_BACKGROUND_COUNT] = {
    "CBKGWATR.icn", "", "CBKGGRTR.icn", "CBKGGRMT.icn",
    "CBKGSNTR.icn", "CBKGSNMT.icn", "CBKGSWMP.icn", "",
    "CBKGLAVA.icn", "", "CBKGDSRT.icn", "",
    "CBKGDITR.icn", "CBKGDIMT.icn", "CBKGCRCK.icn", "",
    "CBKGBEAC.icn", "", "CBKGGRAV.icn", 0
};
DATA(0x004fc5b8) struct SCmbtObstacle sCmbtObstacles[KB_COMBAT_OBSTACLE_COUNT] = {
    { 0x00000200, 1, 1, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x00000200, 1, 1, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x00000042, 3, 1, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x00000001, 1, 1, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x00000000, 2, 2, { 0, 1, 0, 0, 0, 0, 0, 0 } },
    { 0x0000014a, 2, 2, { 0, 1, 0, 0, 0, 0, 0, 0 } },
    { 0x00000008, 1, 1, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x00000048, 1, 2, { 0, 1, 0, 0, 0, 0, 0, 0 } },
    { 0x0000000a, 2, 1, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x000000a0, 3, 1, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x00000001, 1, 1, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x0000014a, 1, 2, { 0, 1, 0, 0, 0, 0, 0, 0 } },
    { 0x0000000a, 1, 1, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x000000a0, 2, 1, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x00000048, 1, 2, { 0, 1, 0, 0, 0, 0, 0, 0 } },
    { 0x0000000a, 2, 2, { 0, 1, 0, 0, 0, 0, 0, 0 } },
    { 0x00000008, 1, 1, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x00000008, 2, 2, { 0, 1, 0, 0, 0, 0, 0, 0 } },
    { 0x00000080, 1, 2, { 0, 1, 0, 0, 0, 0, 0, 0 } },
    { 0x00000142, 1, 2, { 0, 1, 0, 0, 0, 0, 0, 0 } },
    { 0x00000080, 1, 2, { 0, 1, 0, 0, 0, 0, 0, 0 } },
    { 0x00000080, 2, 1, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x00000004, 2, 2, { 0, 1, 0, 0, 0, 0, 0, 0 } },
    { 0x00000001, 1, 1, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x000000a0, 1, 1, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x00000200, 1, 2, { 0, 1, 0, 0, 0, 0, 0, 0 } },
    { 0x00000004, 2, 1, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x0000004a, 3, 1, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x00000002, 3, 1, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x00000010, 1, 1, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x00000000, 1, 2, { 0, 1, 0, 0, 0, 0, 0, 0 } },
    { 0x00000010, 2, 2, { 0, 1, 0, 0, 0, 0, 0, 0 } }
};
DATA(0x004fc778) int gEstatesGoldLevel[HERO_SKILL_LEVEL_COUNT] = {
    0, 100, 250, 500
};
DATA(0x004fc788) float gfSSLogisticsMod[HERO_SKILL_LEVEL_COUNT] = {
    1.0f, 1.1f, 1.2f, 1.3f
};
DATA(0x004fc798) float gfSSNavigationMod[HERO_SKILL_LEVEL_COUNT] = {
    1.0f, 1.33f, 1.66f, 2.0f
};
DATA(0x004fc7a8) float gfSSArcheryMod[HERO_SKILL_LEVEL_COUNT] = {
    1.0f, 1.1f, 1.25f, 1.5f
};
DATA(0x004fc7b8) float gfSSAIArcheryMod[HERO_SKILL_LEVEL_COUNT] = {
    1.0f, 1.04f, 1.1f, 1.2f
};
DATA(0x004fc7c8) signed char giVisRange[HERO_SKILL_LEVEL_COUNT] = {
    4, 5, 6, 7
};
DATA(0x004fc7d0)
unsigned char gStartingHeroStats[HERO_CLASS_COUNT][HERO_STARTING_STAT_COUNT] = {
    { 2, 2, 1, 1, 1 },
    { 3, 1, 1, 1, 1 },
    { 0, 0, 2, 3, 1 },
    { 0, 0, 3, 2, 1 },
    { 0, 1, 2, 2, 1 },
    { 1, 0, 2, 2, 1 }
};
DATA(0x004fc7f0)
int giTerrainCost[KB_TERRAIN_TYPE_COUNT][HERO_SKILL_LEVEL_COUNT]
                 [KB_TERRAIN_STEP_TYPE_COUNT] = {
    { { 100, 150 }, { 100, 150 }, { 100, 150 }, { 100, 150 } },
    { { 100, 150 }, { 100, 150 }, { 100, 150 }, { 100, 150 } },
    { { 150, 225 }, { 125, 187 }, { 100, 150 }, { 100, 150 } },
    { { 175, 262 }, { 150, 225 }, { 125, 187 }, { 100, 150 } },
    { { 100, 150 }, { 100, 150 }, { 100, 150 }, { 100, 150 } },
    { { 200, 300 }, { 175, 262 }, { 150, 225 }, { 100, 150 } },
    { { 100, 150 }, { 100, 150 }, { 100, 150 }, { 100, 150 } },
    { { 125, 187 }, { 100, 150 }, { 100, 150 }, { 100, 150 } },
    { { 125, 187 }, { 100, 150 }, { 100, 150 }, { 100, 150 } },
    { { 75, 112 }, { 75, 112 }, { 75, 112 }, { 75, 112 } }
};
DATA(0x004fc930) unsigned char bStopOnTrigger[KB_TRIGGER_TYPE_COUNT] = {
    0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0
};
DATA(0x004fc9b0) char *gTownPrefixNames[HERO_CLASS_COUNT] = {
    "twnk", "twnb", "twns", "twnw", "twnz", "twnn"
};
DATA(0x004fc9c8) char *gTownObjNames[KB_TOWN_OBJECT_NAME_COUNT] = {
    "mage", "thie", "tvrn", "dock",
    "well", "tent", "cstl", "stat",
    "ltur", "rtur", "mark", "wel2",
    "moat", "spec", "boat", "capt",
    "ext0", "ext1", "ext2", "dw_0",
    "dw_1", "dw_2", "dw_3", "dw_4",
    "dw_5", "up_1", "up_2", "up_3",
    "up_4", "up_5", "up5b", "ext3"
};
DATA(0x004fca48) signed char gDwellingType[TOWN_TYPE_COUNT][KB_DWELLING_TYPE_COUNT] = {
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
DATA(0x004fca90)
int gMageBuildingCosts[KB_MAGE_GUILD_LEVEL_COUNT][KB_BUILDING_RESOURCE_COUNT] = {
    { 0, 0, 0, 0, 0, 0, 0 },
    { 5, 0, 5, 0, 0, 0, 2000 },
    { 5, 4, 5, 4, 4, 4, 1000 },
    { 5, 6, 5, 6, 6, 6, 1000 },
    { 5, 8, 5, 8, 8, 8, 1000 },
    { 5, 10, 5, 10, 10, 10, 1000 }
};
DATA(0x004fcb38)
int gSpecialBuildingCosts[TOWN_TYPE_COUNT][KB_BUILDING_RESOURCE_COUNT] = {
    { 5, 0, 15, 0, 0, 0, 1500 },
    { 10, 0, 10, 0, 0, 0, 2000 },
    { 0, 0, 0, 0, 10, 0, 1500 },
    { 5, 0, 10, 0, 0, 0, 3000 },
    { 5, 5, 5, 5, 5, 5, 1500 },
    { 0, 10, 0, 10, 0, 0, 1000 }
};
DATA(0x004fcbe0)
int gNeutralBuildingCosts[KB_BUILDING_NEUTRAL_LIMIT][KB_BUILDING_RESOURCE_COUNT] = {
    { 5, 0, 5, 0, 0, 0, 2000 },
    { 5, 0, 0, 0, 0, 0, 750 },
    { 5, 0, 0, 0, 0, 0, 500 },
    { 20, 0, 0, 0, 0, 0, 2000 },
    { 0, 0, 0, 0, 0, 0, 500 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 20, 0, 20, 0, 0, 0, 5000 },
    { 0, 0, 5, 0, 0, 0, 1250 },
    { 0, 0, 5, 0, 0, 0, 1500 },
    { 0, 0, 5, 0, 0, 0, 1500 },
    { 5, 0, 0, 0, 0, 0, 500 },
    { 0, 0, 0, 0, 0, 0, 1000 },
    { 0, 0, 0, 0, 0, 0, 750 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 500 }
};
DATA(0x004fcda0) int gMageBaseResourceValues[KB_MAGE_GUILD_LEVEL_COUNT] = {
    0, 4000, 6500, 8500, 10500, 15000
};
DATA(0x004fcdb8) int
    gNeutralBaseResourceValues[KB_BUILDING_DWELLING_FIRST + 1] = {
    5000, 300, 350, 2000, 3000, 0, 12000, 2500,
    1500, 1500, 200, 1000, 500, 0, 0, 1100,
    0, 0, 0, 0
};
DATA(0x004fce08) int gSpecialBuildingBaseResourceValues[TOWN_TYPE_COUNT] = {
    1500, 1000, 1000, 4500, 3500, 1000
};
DATA(0x004fce20)
int gDwellingBaseResourceValues[TOWN_TYPE_COUNT][KB_DWELLING_TYPE_COUNT] = {
    { 858, 2225, 2816, 7385, 13754, 29785, 4000, 3200, 8000, 16000, 40000, 0 },
    { 1802, 2615, 3414, 6967, 13212, 38141, 3500, 0, 8000, 16000, 0, 0 },
    { 1684, 3000, 3500, 7213, 15181, 27684, 4000, 4000, 12000, 0, 0, 0 },
    { 1956, 2607, 3869, 7510, 16002, 90000, 0, 0, 8500, 0, 120000, 180000 },
    { 1700, 3500, 2800, 9000, 11500, 85000, 0, 3500, 0, 15000, 155000, 0 },
    { 2200, 2100, 3800, 6000, 9500, 90000, 3000, 4900, 15000, 12000, 0, 0 }
};
DATA(0x004fcf40)
int gDwellingCosts[TOWN_TYPE_COUNT][KB_DWELLING_TYPE_COUNT][KB_BUILDING_RESOURCE_COUNT] = {
    {
        { 0, 0, 0, 0, 0, 0, 200 }, { 0, 0, 0, 0, 0, 0, 1000 },
        { 0, 0, 5, 0, 0, 0, 1000 }, { 10, 0, 10, 0, 0, 0, 2000 },
        { 20, 0, 0, 0, 0, 0, 3000 }, { 20, 0, 0, 0, 20, 0, 5000 },
        { 5, 0, 0, 0, 0, 0, 1500 }, { 0, 0, 5, 0, 0, 0, 1500 },
        { 5, 0, 5, 0, 0, 0, 2000 }, { 10, 0, 0, 0, 0, 0, 3000 },
        { 10, 0, 0, 0, 10, 0, 5000 }, { 0, 0, 0, 0, 0, 0, 0 }
    },
    {
        { 0, 0, 0, 0, 0, 0, 300 }, { 5, 0, 0, 0, 0, 0, 800 },
        { 0, 0, 0, 0, 0, 0, 1000 }, { 10, 0, 10, 0, 0, 0, 2000 },
        { 0, 0, 20, 0, 0, 0, 4000 }, { 0, 0, 20, 0, 20, 0, 6000 },
        { 5, 0, 0, 0, 0, 0, 1200 }, { 0, 0, 0, 0, 0, 0, 0 },
        { 5, 0, 5, 0, 0, 0, 3000 }, { 0, 0, 10, 0, 0, 0, 2000 },
        { 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 }
    },
    {
        { 5, 0, 0, 0, 0, 0, 500 }, { 5, 0, 0, 0, 0, 0, 1000 },
        { 0, 0, 0, 0, 0, 0, 1500 }, { 0, 0, 10, 0, 0, 0, 2500 },
        { 10, 0, 0, 0, 0, 10, 3000 }, { 0, 20, 30, 0, 0, 0, 10000 },
        { 5, 0, 0, 0, 0, 0, 1500 }, { 5, 0, 0, 0, 0, 0, 1500 },
        { 0, 5, 0, 0, 0, 0, 1500 }, { 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 }
    },
    {
        { 0, 0, 0, 0, 0, 0, 500 }, { 0, 0, 10, 0, 0, 0, 1000 },
        { 0, 0, 0, 0, 0, 0, 2000 }, { 0, 0, 0, 0, 0, 10, 3000 },
        { 0, 0, 0, 10, 0, 0, 4000 }, { 0, 0, 30, 20, 0, 0, 15000 },
        { 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 5, 2000 }, { 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 5, 10, 0, 0, 5000 }, { 0, 0, 5, 10, 0, 0, 5000 }
    },
    {
        { 0, 0, 0, 0, 0, 0, 400 }, { 0, 0, 0, 0, 0, 0, 800 },
        { 5, 0, 5, 0, 0, 0, 1500 }, { 5, 0, 0, 0, 0, 0, 3000 },
        { 5, 5, 5, 5, 5, 5, 3500 }, { 5, 0, 5, 0, 0, 20, 12500 },
        { 0, 0, 0, 0, 0, 0, 0 }, { 0, 5, 0, 0, 0, 0, 1500 },
        { 0, 0, 0, 0, 0, 0, 0 }, { 5, 0, 5, 0, 0, 0, 4000 },
        { 5, 0, 5, 0, 0, 20, 12500 }, { 0, 0, 0, 0, 0, 0, 0 }
    },
    {
        { 0, 0, 0, 0, 0, 0, 400 }, { 0, 0, 0, 0, 0, 0, 1000 },
        { 0, 0, 10, 0, 0, 0, 1500 }, { 10, 0, 0, 0, 0, 0, 3000 },
        { 10, 0, 0, 10, 0, 0, 4000 }, { 10, 5, 10, 5, 5, 5, 10000 },
        { 0, 0, 0, 0, 0, 0, 1000 }, { 0, 0, 5, 0, 0, 0, 1500 },
        { 5, 0, 0, 0, 10, 10, 4000 }, { 0, 0, 5, 0, 5, 0, 3000 },
        { 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 }
    }
};
DATA(0x004fd720)
unsigned long gHierarchyMask[TOWN_TYPE_COUNT][KB_DWELLING_TYPE_COUNT] = {
    { 0x00000000UL, 0x00080000UL, 0x00080010UL, 0x00080004UL,
      0x00700000UL, 0x00700000UL, 0x00700000UL, 0x00700000UL,
      0x00700000UL, 0x00800000UL, 0x01000000UL, 0xffffffffUL },
    { 0x00000000UL, 0x00080000UL, 0x00080000UL, 0x00080000UL,
      0x00700000UL, 0x00800000UL, 0x00700000UL, 0xffffffffUL,
      0x00700000UL, 0x00800000UL, 0xffffffffUL, 0xffffffffUL },
    { 0x00000000UL, 0x00080004UL, 0x00080000UL, 0x00200001UL,
      0x00400000UL, 0x00800000UL, 0x00100010UL, 0x00400000UL,
      0x00400000UL, 0xffffffffUL, 0xffffffffUL, 0xffffffffUL },
    { 0x00000000UL, 0x00080000UL, 0x00080000UL, 0x00100000UL,
      0x00200000UL, 0x00c00000UL, 0xffffffffUL, 0xffffffffUL,
      0x00400000UL, 0xffffffffUL, 0x01000000UL, 0x20000000UL },
    { 0x00000000UL, 0x00080000UL, 0x00080000UL, 0x00100000UL,
      0x00200001UL, 0x00c00000UL, 0xffffffffUL, 0x00200010UL,
      0xffffffffUL, 0x00802000UL, 0x01000000UL, 0xffffffffUL },
    { 0x00000000UL, 0x00080000UL, 0x00080000UL, 0x00200002UL,
      0x00100001UL, 0x00800000UL, 0x00100000UL, 0x00200000UL,
      0x00400000UL, 0x00800000UL, 0xffffffffUL, 0xffffffffUL }
};
DATA(0x004fd840) int giDebugBuildingToBuild = -1;
DATA(0x004fd848) unsigned char giTerrainToMusicTrack[GROUND_TERRAIN_TYPE_COUNT] = {
    16, 18, 14, 15, 11, 13, 17, 12, 16
};
DATA(0x004fd858) char *cHeroTypeShortName[HERO_CLASS_COUNT] = {
    "kngt", "barb", "sorc", "wrlk", "wzrd", "necr"
};
DATA(0x004fd870) char cHeroTypeInitial[HERO_TYPE_INITIAL_COUNT] = {
    'k', 'b', 's', 'w', 'z', 'n'
};
DATA(0x004fd878) int giDeferObjDrawX = -1;
DATA(0x004fd87c) int giDeferObjDrawY = -1;
DATA(0x004fd880) class heroWindow *gpInitWin = 0;
DATA(0x004fd888) unsigned char iGetSSByAlignment[HERO_SKILL_COUNT][HERO_CLASS_COUNT] = {
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
DATA(0x004fd8e0)
struct SCmbtHero sCmbtHero[KB_COMBAT_HERO_SPRITE_COUNT] = {
    { { 18, 8 }, { -61, -49 }, 2, { 1, 9, 9, 2, 1, 3, 2, 2, 1, 3, 2, 0 }, {
        {
1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
2, 3, 4, 5, 4, 5, 4, 3, 2
        },
        {
6, 7, 8, 9, 8, 9, 8, 7, 6
        },
        {
10, 11, -1, -1, -1, -1, -1, -1, -1
        },
        {
10, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
6, 12, 13, -1, -1, -1, -1, -1, -1
        },
        {
12, 6, -1, -1, -1, -1, -1, -1, -1
        },
        {
2, 14, -1, -1, -1, -1, -1, -1, -1
        },
        {
2, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
15, 16, 17, -1, -1, -1, -1, -1, -1
        },
        {
18, 19, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        }
    } },
    { { 13, 9 }, { -67, -56 }, 2, { 1, 9, 8, 3, 2, 3, 2, 2, 1, 3, 1, 0 }, {
        {
1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
2, 3, 4, 5, 4, 5, 4, 3, 2
        },
        {
6, 7, 8, 9, 9, 8, 7, 6, -1
        },
        {
6, 10, 11, -1, -1, -1, -1, -1, -1
        },
        {
10, 6, -1, -1, -1, -1, -1, -1, -1
        },
        {
6, 12, 13, -1, -1, -1, -1, -1, -1
        },
        {
12, 6, -1, -1, -1, -1, -1, -1, -1
        },
        {
6, 14, -1, -1, -1, -1, -1, -1, -1
        },
        {
6, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
15, 16, 17, -1, -1, -1, -1, -1, -1
        },
        {
18, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        }
    } },
    { { 19, 12 }, { -62, -50 }, 2, { 1, 9, 5, 3, 2, 3, 2, 2, 1, 3, 1, 0 }, {
        {
1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
2, 3, 4, 5, 4, 5, 4, 3, 2
        },
        {
6, 7, 8, 7, 6, -1, -1, -1, -1
        },
        {
6, 7, 9, -1, -1, -1, -1, -1, -1
        },
        {
7, 6, -1, -1, -1, -1, -1, -1, -1
        },
        {
6, 10, 11, -1, -1, -1, -1, -1, -1
        },
        {
10, 6, -1, -1, -1, -1, -1, -1, -1
        },
        {
6, 12, -1, -1, -1, -1, -1, -1, -1
        },
        {
6, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
13, 14, 15, -1, -1, -1, -1, -1, -1
        },
        {
16, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        }
    } },
    { { 18, 7 }, { -65, -50 }, 2, { 1, 9, 9, 4, 2, 3, 1, 2, 1, 2, 1, 0 }, {
        {
1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
2, 3, 4, 5, 4, 5, 4, 3, 2
        },
        {
6, 7, 8, 9, 10, 9, 8, 7, 6
        },
        {
6, 7, 11, 12, -1, -1, -1, -1, -1
        },
        {
11, 6, -1, -1, -1, -1, -1, -1, -1
        },
        {
6, 7, 13, -1, -1, -1, -1, -1, -1
        },
        {
6, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
6, 14, -1, -1, -1, -1, -1, -1, -1
        },
        {
6, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
15, 16, -1, -1, -1, -1, -1, -1, -1
        },
        {
6, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        }
    } },
    { { 25, 22 }, { -65, -47 }, 2, { 1, 9, 7, 5, 4, 2, 1, 2, 1, 2, 1, 0 }, {
        {
1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
2, 3, 4, 5, 4, 5, 4, 3, 2
        },
        {
6, 7, 8, 9, 8, 7, 6, -1, -1
        },
        {
6, 10, 11, 12, 13, -1, -1, -1, -1
        },
        {
12, 11, 10, 6, -1, -1, -1, -1, -1
        },
        {
6, 14, -1, -1, -1, -1, -1, -1, -1
        },
        {
6, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
6, 15, -1, -1, -1, -1, -1, -1, -1
        },
        {
6, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
16, 17, -1, -1, -1, -1, -1, -1, -1
        },
        {
18, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        }
    } },
    { { 17, 10 }, { -61, -50 }, 2, { 1, 9, 6, 5, 3, 5, 1, 5, 1, 1, 2, 0 }, {
        {
1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
2, 3, 4, 5, 4, 5, 4, 3, 2
        },
        {
6, 7, 6, 7, 6, 7, -1, -1, -1
        },
        {
7, 8, 9, 10, 11, -1, -1, -1, -1
        },
        {
10, 9, 7, -1, -1, -1, -1, -1, -1
        },
        {
7, 12, 13, 14, 15, -1, -1, -1, -1
        },
        {
7, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
7, 12, 13, 14, 16, -1, -1, -1, -1
        },
        {
7, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
17, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
18, 19, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        }
    } },
    { { 17, 12 }, { -45, -31 }, 2, { 1, 0, 0, 3, 2, 2, 1, 2, 1, 3, 1, 0 }, {
        {
1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
2, 3, 4, -1, -1, -1, -1, -1, -1
        },
        {
3, 2, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, 6, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, 7, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
8, 9, 8, -1, -1, -1, -1, -1, -1
        },
        {
10, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        }
    } },
    { { 17, 12 }, { -45, -31 }, 2, { 1, 0, 0, 3, 2, 3, 2, 3, 2, 1, 2, 0 }, {
        {
1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
2, 3, 4, -1, -1, -1, -1, -1, -1
        },
        {
3, 2, -1, -1, -1, -1, -1, -1, -1
        },
        {
2, 5, 6, -1, -1, -1, -1, -1, -1
        },
        {
5, 2, -1, -1, -1, -1, -1, -1, -1
        },
        {
2, 5, 7, -1, -1, -1, -1, -1, -1
        },
        {
5, 2, -1, -1, -1, -1, -1, -1, -1
        },
        {
8, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
9, 10, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        }
    } },
    { { 17, 12 }, { -45, -31 }, 2, { 1, 0, 0, 3, 2, 2, 1, 2, 1, 2, 1, 0 }, {
        {
1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
2, 3, 4, -1, -1, -1, -1, -1, -1
        },
        {
3, 2, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, 6, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, 7, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
8, 9, -1, -1, -1, -1, -1, -1, -1
        },
        {
10, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        }
    } },
    { { 17, 12 }, { -45, -31 }, 1, { 1, 0, 0, 3, 2, 2, 1, 2, 1, 3, 0, 0 }, {
        {
1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
2, 3, 4, -1, -1, -1, -1, -1, -1
        },
        {
3, 2, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, 6, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, 7, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
8, 9, 10, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        }
    } },
    { { 17, 12 }, { -45, -31 }, 2, { 1, 0, 0, 3, 2, 2, 1, 2, 1, 1, 2, 0 }, {
        {
1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
2, 3, 4, -1, -1, -1, -1, -1, -1
        },
        {
3, 2, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, 6, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, 7, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
8, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
9, 10, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        }
    } },
    { { 17, 12 }, { -45, -31 }, 2, { 1, 0, 0, 3, 2, 2, 1, 2, 1, 1, 2, 0 }, {
        {
1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
2, 3, 4, -1, -1, -1, -1, -1, -1
        },
        {
3, 2, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, 6, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, 7, -1, -1, -1, -1, -1, -1, -1
        },
        {
5, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
8, -1, -1, -1, -1, -1, -1, -1, -1
        },
        {
9, 10, -1, -1, -1, -1, -1, -1, -1
        },
        {
-1, -1, -1, -1, -1, -1, -1, -1, -1
        }
    } }
};
DATA(0x004fdeec) unsigned char iWallToHexCell[KB_CASTLE_WALL_SEGMENT_COUNT] = {
    9, 34, 86, 113
};
DATA(0x004fdef0) unsigned char iTowerToHexCell[KB_CASTLE_TOWER_COUNT] = {
    22, 47, 73, 100
};
DATA(0x004fdef8)
unsigned short wallPos[KB_CASTLE_WALL_SEGMENT_COUNT][KB_COMBAT_COORDINATE_COUNT] = {
    { 468, 58 }, { 421, 128 }, { 417, 291 }, { 498, 402 }
};
DATA(0x004fdf08)
unsigned short towerPos[KB_CASTLE_TOWER_COUNT][KB_COMBAT_COORDINATE_COUNT] = {
    { 441, 61 }, { 398, 147 }, { 398, 233 }, { 441, 316 }
};
DATA(0x004fdf18)
unsigned short doorPos[KB_CASTLE_DOOR_POSITION_COUNT][KB_COMBAT_COORDINATE_COUNT] = {
    { 393, 192 }, { 348, 262 }
};
DATA(0x004fdf20)
float fTradingPostEfficency[KB_TRADING_POST_EFFICIENCY_COUNT] = {
    0.0f, 0.1f, 0.15f, 0.2f, 0.25f, 0.3f,
    0.35f, 0.4f, 0.45f, 0.5f, 0.5f
};
DATA(0x004fdf50) struct SElevationOverlay
    sElevationOverlay[ELEVATION_OVERLAY_COUNT] = {
    { 0x0000, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0002, { 30, 31, 32, 33, 47, 60, -1, -1, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0002, { 56, 57, 58, 59, 60, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0002, { 42, 55, 56, 57, 58, 59, 60, 48, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0002, { 69, 70, 71, 72, 73, 60, 48, -1, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0002, { 29, 30, 31, 32, 33, 34, 35, 81, 69, 70, 71, 72, 73, 74, 87 } },
    { 0x0002, { 29, 17, 18, 19, 20, 21, 81, 95, 96, 97, 98, 99, -1, -1, -1 } },
    { 0x0004, { 30, 31, 32, 33, 47, 60, -1, -1, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0004, { 56, 57, 58, 59, 60, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0004, { 42, 55, 56, 57, 58, 59, 47, -1, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0004, { 69, 70, 71, 72, 73, 60, 48, -1, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0004, { 18, 30, 43, 84, 85, 73, 60, -1, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0004, { 21, 34, 48, 70, 83, 97, 98, -1, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0040, { 30, 31, 32, 33, 47, 60, -1, -1, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0040, { 56, 57, 58, 59, 60, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0040, { 42, 55, 56, 57, 58, 59, 60, 48, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0040, { 69, 70, 71, 72, 73, 60, 48, -1, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0040, { 29, 30, 31, 32, 33, 34, 35, 81, 69, 70, 71, 72, 73, 74, 87 } },
    { 0x0040, { 29, 17, 18, 19, 20, 21, 81, 95, 96, 97, 98, 99, -1, -1, -1 } },
    { 0x0080, { 30, 31, 32, 33, 47, 60, -1, -1, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0080, { 56, 57, 58, 59, 60, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0080, { 42, 55, 56, 57, 58, 59, 60, 48, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0080, { 69, 70, 71, 72, 73, 60, 48, -1, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0080, { 43, 30, 18, 84, 85, 73, 60, -1, -1, -1, -1, -1, -1, -1, -1 } },
    { 0x0080, { 21, 34, 48, 70, 83, 97, 98, -1, -1, -1, -1, -1, -1, -1, -1 } }
};
DATA(0x004fe100)
signed char captainStats[HERO_CLASS_COUNT][HERO_PRIMARY_STAT_COUNT] = {
    { 1, 1, 1, 1 }, { 1, 1, 1, 1 },
    { 0, 0, 2, 2 }, { 0, 0, 2, 2 },
    { 0, 0, 2, 2 }, { 0, 0, 2, 2 }
};
DATA(0x004fe118) int gbDrawingPuzzle = 0;
DATA(0x004fe11c) int giWalkingFrom = -1;
DATA(0x004fe120) int giWalkingFrom2 = -1;
DATA(0x004fe124) int giWalkingTo = -1;
DATA(0x004fe128) int giWalkingTo2 = -1;
DATA(0x004fe12c) int giWalkingYMod = 0;
DATA(0x004fe130) unsigned char moatCell[KB_MOAT_CELL_COUNT] = {
    8, 21, 33, 46, 58, 72, 85, 99, 112
};
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
DATA(0x004fe2b0) char *gArtifactNames[KB_ARTIFACT_LEVEL_COUNT] = {
    "Ultimate Book of Knowledge",
    "Ultimate Sword of Dominion",
    "Ultimate Cloak of Protection",
    "Ultimate Wand of Magic",
    "Ultimate Shield",
    "Ultimate Staff",
    "Ultimate Crown",
    "Golden Goose",
    "Arcane Necklace of Magic",
    "Caster's Bracelet of Magic",
    "Mage's Ring of Power",
    "Witch's Broach of Magic",
    "Medal of Valor",
    "Medal of Courage",
    "Medal of Honor",
    "Medal of Distinction",
    "Fizbin of Misfortune",
    "Thunder Mace of Dominion",
    "Armored Gauntlets of Protection",
    "Defender Helm of Protection",
    "Giant Flail of Dominion",
    "Ballista of Quickness",
    "Stealth Shield of Protection",
    "Dragon Sword of Dominion",
    "Power Axe of Dominion",
    "Divine Breastplate of Protection",
    "Minor Scroll of Knowledge",
    "Major Scroll of Knowledge",
    "Superior Scroll of Knowledge",
    "Foremost Scroll of Knowledge",
    "Endless Sack of Gold",
    "Endless Bag of Gold",
    "Endless Purse of Gold",
    "Nomad Boots of Mobility",
    "Traveler's Boots of Mobility",
    "Lucky Rabbit's Foot",
    "Golden Horseshoe",
    "Gambler's Lucky Coin",
    "Four-Leaf Clover",
    "True Compass of Mobility",
    "Sailor's Astrolabe of Mobility",
    "Evil Eye",
    "Enchanted Hourglass",
    "Gold Watch",
    "Skullcap",
    "Ice Cloak",
    "Fire Cloak",
    "Lightning Helm",
    "Evercold Icicle",
    "Everhot Lava Rock",
    "Lightning Rod ",
    "Snake-Ring",
    "Ankh",
    "Book of Elements",
    "Elemental Ring",
    "Holy Pendant",
    "Pendant of Free Will",
    "Pendant of Life",
    "Serenity Pendant",
    "Seeing-eye Pendant",
    "Kinetic Pendant",
    "Pendant of Death",
    "Wand of Negation",
    "Golden Bow",
    "Telescope",
    "Statesman's Quill",
    "Wizard's Hat",
    "Power Ring",
    "Ammo Cart",
    "Tax Lien",
    "Hideous Mask",
    "Endless Pouch of Sulfur",
    "Endless Vial of Mercury",
    "Endless Pouch of Gems",
    "Endless Cord of Wood",
    "Endless Cart of Ore",
    "Endless Pouch of Crystal",
    "Spiked Helm",
    "Spiked Shield",
    "White Pearl",
    "Black Pearl",
    "Magic Book",
    "ERROR : Artifact 82",
    "ERROR : Artifact 83",
    "ERROR : Artifact 84",
    "ERROR : Artifact 85",
    "Spell Scroll",
    "Arm of the Martyr",
    "Breastplate of Anduran",
    "Broach of Shielding",
    "Battle Garb of Anduran",
    "Crystal Ball",
    "Heart of Fire",
    "Heart of Ice",
    "Helmet of Anduran",
    "Holy Hammer",
    "Legendary Scepter",
    "Masthead",
    "Sphere of Negation",
    "Staff of Wizardry",
    "Sword Breaker",
    "Sword of Anduran",
    "Spade of Necromancy",
    0
};
DATA(0x004fe450) char *gArtifactDesc[KB_ARTIFACT_LEVEL_COUNT] = {
    "{Ultimate Book\n(+12 Knowledge)}\n\nThe Ultimate Book of Knowledge increases your knowledge by 12.",
    "{Ultimate Sword\n(+12 Attack)}\n\nThe Ultimate Sword of Dominion increases your attack skill by 12.",
    "{Ultimate Cloak\n(+12 Defense)}\n\nThe Ultimate Cloak of Protection increases your defense skill by 12.",
    "{Ultimate Wand\n(+12 Spell Power)}\n\nThe Ultimate Wand of Magic increases your spell power by 12.",
    "{Ultimate Shield}\n\nThe Ultimate Shield increases your attack and defense skills by 6 each.",
    "{Ultimate Staff}\n\nThe Ultimate Staff increases your spell power and knowledge by 6 each.",
    "{Ultimate Crown}\n\nThe Ultimate Crown increases each of your basic skills by 4 points.",
    "{Golden Goose}\n\nThe Golden Goose brings in an income of 10,000 gold per turn.",
    "{Arcane Necklace\n(+4 Spell Power)}\n\nThe Arcane Necklace of Magic increases your spell power by 4.",
    "{Caster's Bracelet\n(+2 Spell Power)}\n\nThe Caster's Bracelet of Magic increases your spell power by 2.",
    "{Mage's Ring\n(+2 Spell Power)}\n\nThe Mage's Ring of Power increases your spell power by 2.",
    "{Witches Broach\n(+3 Spell Power)}\n\nThe Witch's Broach of Magic increases your spell power by 3.",
    "{Medal}\n\nThe Medal of Valor increases your morale.",
    "{Medal}\n\nThe Medal of Courage increases your morale.",
    "{Medal}\n\nThe Medal of Honor increases your morale.",
    "{Medal}\n\nThe Medal of Distinction increases your morale.",
    "{Fizbin}\n\nThe Fizbin of Misfortune greatly decreases your morale.",
    "{Thunder Mace\n(+1 Attack)}\n\nThe Thunder Mace of Dominion increases your attack skill by 1.",
    "{Armored Gauntlets\n(+1 Defense)}\n\nThe Armored Gauntlets of Protection increase your defense skill by 1.",
    "{Defender Helm\n(+1 Defense)}\n\nThe Defender Helm of Protection increases your defense skill by 1.",
    "{Giant Flail\n(+1 Attack)}\n\nThe Giant Flail of Dominion increases your attack skill by 1.",
    "{Ballista}\n\nThe Ballista of Quickness lets your catapult fire twice per combat round.",
    "{Stealth Shield\n(+2 Defense)}\n\nThe Stealth Shield of Protection increases your defense skill by 2.",
    "{Dragon Sword\n(+3 Attack)}\n\nThe Dragon Sword of Dominion increases your attack skill by 3.",
    "{Power Axe\n(+2 Attack)}\n\nThe Power Axe of Dominion increases your attack skill by 2.",
    "{Divine Breastplate\n(+3 Defense)}\n\nThe Divine Breastplate of Protection increases your defense skill by 3.",
    "{Minor Scroll\n(+2 Knowledge)}\n\nThe Minor Scroll of Knowledge increases your knowledge by 2.",
    "{Major Scroll\n(+3 Knowledge)}\n\nThe Major Scroll of Knowledge increases your knowledge by 3.",
    "{Superior Scroll\n(+4 Knowledge)}\n\nThe Superior Scroll of Knowledge increases your knowledge by 4.",
    "{Foremost Scroll\n(+5 Knowledge)}\n\nThe Foremost Scroll of Knowledge increases your knowledge by 5.",
    "{Endless Sack}\n\nThe Endless Sack of Gold provides you with 1000 gold per day.",
    "{Endless Bag}\n\nThe Endless Bag of Gold provides you with 750 gold per day.",
    "{Endless Purse}\n\nThe Endless Purse of Gold provides you with 500 gold per day.",
    "{Nomad Boots}\n\nThe Nomad Boots of Mobility increase your movement on land.",
    "{Traveler's Boots}\n\nThe Traveler's Boots of Mobility increase your movement on land.",
    "{Rabbit's Foot}\n\nThe Lucky Rabbit's Foot increases your luck in combat.",
    "{Horseshoe}\n\nThe Golden Horseshoe increases your luck in combat.",
    "{Coin}\n\nThe Gambler's Lucky Coin increases your luck in combat.",
    "{Clover}\n\nThe Four-Leaf Clover increases your luck in combat.",
    "{Compass}\n\nThe True Compass of Mobility increases your movement on land and sea.",
    "{Astrolabe}\n\nThe Sailors' Astrolabe of Mobility increases your movement on sea.",
    "{Evil Eye}\n\nThe Evil Eye reduces the casting cost of curse spells by half.",
    "{Enchanted hourglass}\n\nThe Enchanted Hourglass extends the duration of all your spells by 2 turns.",
    "{Gold watch}\n\nThe Gold Watch doubles the effectiveness of your hypnotize spells.",
    "{Skullcap}\n\nThe Skullcap halves the casting cost of all mind influencing spells.",
    "{Ice cloak}\n\nThe Ice Cloak halves all damage your troops take from cold spells.",
    "{Fire cloak}\n\nThe Fire Cloak halves all damage your troops take from fire spells.",
    "{Lightning helm}\n\nThe Lightning Helm halves all damage your troops take from lightning spells.",
    "{Evercold icicle}\n\nThe Evercold Icicle causes your cold spells to do 50% more damage to enemy troops.",
    "{Everhot lava rock}\n\nThe Everhot Lava Rock causes your fire spells to do 50% more damage to enemy troops.",
    "{Lightning rod }\n\nThe Lightning Rod causes your lightning spells to do 50% more damage to enemy troops.",
    "{Snake-ring}\n\nThe Snake Ring halves the casting cost of all your bless spells.",
    "{Ankh}\n\nThe Ankh doubles the effectiveness of all your resurrect and animate spells.",
    "{Book of Elements}\n\nThe Book of Elements doubles the effectiveness of all your summoning spells.",
    "{Elemental ring}\n\nThe Elemental Ring halves the casting cost of all summoning spells.",
    "{Holy Pendant}\n\nThe Holy Pendant makes all your troops immune to curse spells.",
    "{Pendant of free will}\n\nThe Pendant of Free Will makes all your troops immune to hypnotize spells.",
    "{Pendant of life}\n\nThe Pendant of Life makes all your troops immune to death spells.",
    "{Serenity pendant}\n\nThe Serenity Pendant makes all your troops immune to berserk spells.",
    "{Seeing-eye pendant}\n\nThe Seeing-eye Pendant makes all your troops immune to blindness spells.",
    "{Kinetic pendant}\n\nThe Kinetic Pendant makes all your troops immune to paralyze spells.",
    "{Pendant of death}\n\nThe Pendant of Death makes all your troops immune to holy spells.",
    "{Wand of negation}\n\nThe Wand of Negation protects your troops from the Dispel Magic spell.",
    "{Golden bow}\n\nThe Golden Bow eliminates the 50% penalty for your troops shooting past obstacles. (e.g. castle walls)",
    "{Telescope}\n\nThe Telescope increases the amount of terrain your hero reveals when adventuring by 1 extra square.",
    "{Statesman's quill}\n\nThe Statesman's Quill reduces the cost of surrender to 10% of the total cost of troops you have in your army.",
    "{Wizard's hat}\n\nThe Wizard's Hat increases the duration of your spells by 10 turns!",
    "{Power ring}\n\nThe Power Ring returns 2 extra spell points/turn to your hero.",
    "{Ammo cart}\n\nThe Ammo Cart provides endless ammunition for all your troops that shoot.",
    "{Tax lien}\n\nThe Tax Lien costs you 250 gold pieces/turn.",
    "{Hideous mask}\n\nThe Hideous Mask prevents all 'wandering' armies from joining your hero.",
    "{Endless pouch of sulfur}\n\nThe Endless Pouch of Sulfur provides 1 unit of sulfur per day.",
    "{Endless vial of mercury}\n\nThe Endless Vial of Mercury provides 1 unit of mercury per day.",
    "{Endless pouch of gems}\n\nThe Endless Pouch of Gems provides 1 unit of gems per day.",
    "{Endless cord of wood}\n\nThe Endless Cord of Wood provides 1 unit of wood per day.",
    "{Endless cart of ore}\n\nThe Endless Cart of Ore provides 1 unit of ore per day.",
    "{Endless pouch of crystal}\n\nThe Endless Pouch of Crystal provides 1 unit of crystal/day.",
    "{Spiked helm}\n\nThe Spiked Helm increases your attack and defense skills by 1 each.",
    "{Spiked shield}\n\nThe Spiked Shield increases your attack and defense skills by 2 each.",
    "{White pearl}\n\nThe White Pearl increases your spell power and knowledge by 1 each.",
    "{Black pearl}\n\nThe Black Pearl increases your spell power and knowledge by 2 each.",
    "{Magic Book}\n\nThe Magic Book enables you to cast spells.",
    "{ERROR}\n\nArtifact 82.",
    "{ERROR}\n\nArtifact 83.",
    "{ERROR}\n\nArtifact 84.",
    "{ERROR}\n\nArtifact 85.",
    "{Spell Scroll}\n\nThis Spell Scroll gives your hero the ability to cast the '%s' spell.",
    "{Arm of the Martyr}\n\nThe Arm of the Martyr increases your spell power by 3 but adds the undead morale penalty.",
    "{Breastplate of Anduran}\n\nThe Breastplate increases your defense by 5.",
    "{Broach of Shielding}\n\nThe Broach of Shielding provides 50% protection from Armageddon and Elemental Storm, but decreases spell power by 2.",
    "{Battle Garb}\n\nThe Battle Garb of Anduran combines the powers of the three Anduran artifacts.  It provides maximum luck and morale for your troops and gives you the Town Portal spell.",
    "{Crystal Ball}\n\nThe Crystal Ball lets you get more specific information about monsters, enemy heroes, and castles nearby the hero who holds it.",
    "{Heart of Fire}\n\nThe Heart of Fire provides 50% protection from fire, but doubles the damage taken from cold.",
    "{Heart of Ice}\n\nThe Heart of Ice provides 50% protection from cold, but doubles the damage taken from fire.",
    "{Helmet of Anduran}\n\nThe Helmet increases your spell power by 5.",
    "{Holy Hammer}\n\nThe Holy Hammer increases your attack skill by 5.",
    "{Legendary Scepter}\n\nThe Legendary Scepter adds 2 points to all attributes.",
    "{Masthead}\n\nThe Masthead boosts your luck and morale by 1 each in sea combat.",
    "{Sphere of Negation}\n\nThe Sphere of Negation disables all spell casting, for both sides, in combat.",
    "{Staff of Wizardry}\n\nThe Staff of Wizardry boosts your spell power by 5.",
    "{Sword Breaker}\n\nThe Sword Breaker increases your defense by 4 and attack by 1.",
    "{Sword of Anduran}\n\nThe Sword increases your attack skill by 5.",
    "{Spade of Necromancy}\n\nThe Spade gives you increased necromancy skill.",
    0
};
DATA(0x004fe5f0) char *gArtifactEvent[KB_ARTIFACT_LEVEL_COUNT] = {
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "After rescuing a Sorceress from a cursed tomb, she rewards your heroism with an exquisite jeweled necklace.",
    "While searching through the rubble of a caved-in mine, you free a group of trapped Dwarves.  Grateful, the leader gives you a golden bracelet.",
    "A cry of pain leads you to a Centaur, caught in a trap.  Upon setting the creature free, he hands you a small pouch.  Emptying the contents, you find a dazzling jeweled ring.",
    "Alongside the remains of a burnt witch lies a beautiful broach, intricately designed.  Approaching the corpse with caution, you add the broach to your inventory.",
    "Freeing a virtuous maiden from the clutches of an evil overlord, you are granted a Medal of Valor by the King's herald.",
    "After saving a young boy from a vicious pack of Wolves, you return him to his father's manor.  The grateful nobleman awards you with a Medal of Courage.",
    "After freeing a princess of a neighboring kingdom from the evil clutches of despicable slavers, she awards you with a Medal of Honor.",
    "Ridding the countryside of the hideous Minotaur who made a sport of eating noblemen's Knights, you are honored with the Medal of Distinction.",
    "You stumble upon a medal lying alongside the empty road.  Adding the medal to your inventory, you become aware that you have acquired the undesirable Fizbin of Misfortune, greatly decreasing your army's morale.",
    "During a sudden storm, a bolt of lightning strikes a tree, splitting it.  Inside the tree you find a mysterious mace.",
    "You encounter the infamous Black Knight!  After a grueling duel ending in a draw, the Knight, out of respect, offers you a pair of armored gauntlets.",
    "A glint of golden light catches your eye.  Upon further investigation, you find a golden helm hidden under a bush.",
    "A clumsy Giant has killed himself with his own flail.  Knowing your superior skill with this weapon, you confidently remove the spectacular flail from the fallen Giant.",
    "Walking through the ruins of an ancient walled city, you find the instrument of the city's destruction, an elaborately crafted ballista.",
    "A stone statue of a warrior holds a silver shield.  As you remove the shield, the statue crumbles into dust.",
    "As you are walking along a narrow path, a nearby bush suddenly bursts into flames.  Before your eyes the flames become the image of a beautiful woman.  She holds out a magnificent sword to you.",
    "You see a silver axe embedded deeply in the ground.  After several unsuccessful attempts by your army to remove the axe, you tightly grip the handle of the axe and effortlessly pull it free.",
    "A gang of Rogues is sifting through the possessions of dead warriors.  Scaring off the scavengers, you note the Rogues had overlooked a beautiful breastplate.",
    "Before you appears a levitating glass case with a scroll, perched upon a bed of crimson velvet.  At your touch, the lid opens and the scroll floats into your awaiting hands.",
    "Visiting a local wiseman, you explain the intent of your journey.  He reaches into a sack and withdraws a yellowed scroll and hands it to you.",
    "You come across the remains of an ancient Druid.  Bones, yellowed with age, peer from the ragged folds of her robe.  Searching the robe, you discover a scroll hidden in the folds.",
    "Mangled bones, yellowed with age, peer from the ragged folds of a dead Druid's robe.  Searching the robe, you discover a scroll hidden within.",
    "A little leprechaun dances gleefully around a magic sack.  Seeing you approach, he stops in mid-stride.  The little man screams and stamps his foot ferociously, vanishing into thin air.  Remembering the old leprechaun saying 'Finders Keepers', you grab the sack and leave.",
    "A noblewoman, separated from her traveling companions, asks for your help.  After escorting her home, she rewards you with a bag filled with gold.",
    "In your travels, you find a leather purse filled with gold that once belonged to a great warrior king who had the ability to transform any inanimate object into gold.",
    "A Nomad trader seeks protection from a tribe of Goblins.  For your assistance, he gives you a finely crafted pair of boots made from the softest leather.  Looking closely, you see fascinating ancient carvings engraved on the leather.",
    "Discovering a pair of beautifully beaded boots made from the finest and softest leather, you thank the anonymous donor and add the boots to your inventory.",
    "A traveling merchant offers you a rabbit's foot, made of gleaming silver fur, for safe passage.  The merchant explains the charm will increase your luck in combat.",
    "An ensnared Unicorn whinnies in fright.  Murmuring soothing words, you set her free.  Snorting and stamping her front hoof once, she gallops off.  Looking down you see a golden horseshoe.",
    "You have captured a mischievous imp who has been terrorizing the region.  In exchange for his release, he rewards you with a magical coin.",
    "In the middle of a patch of dead and dry vegetation, to your surprise you find a healthy green four-leaf clover.",
    "An old man claiming to be an inventor asks you to try his latest invention.  He then hands you a compass.",
    "An old sea captain is being tortured by Ogres.  You save him, and in return he rewards you with a wondrous instrument to measure the distance of a star.",
    "While venturing into a decrepit hut you find the Skeleton of a long dead witch.  Investigation of the remains reveals a glass eye rolling around inside an empty skull.",
    "A surprise turn in the landscape finds you in the midst of a grisly scene:  Vultures picking at the aftermath of a terrible battle.  Your cursory search of the remains turns up an enchanted hourglass.",
    "In reward for helping his cart out of a ditch, a traveling potion salesman gives you a \"magic\" gold watch.  Unbeknownst to him, the watch really is magical.",
    "A brief stop at an improbable rural inn yields an exchange of money, tales, and accidentally, luggage.  You find a magical skullcap in your new backpack.",
    "Responding to the panicked cries of a damsel in distress, you discover a young woman fleeing from a hungry bear.  You slay the beast in the nick of time, and the grateful Sorceress weaves a magic cloak from the bear's hide.",
    "You've come upon a fight between a Necromancer and a Paladin.  The Necromancer blasts the Paladin with a fire bolt, bringing him to his knees.  Acting quickly, you slay the evil one before the final blow.  The grateful Paladin gives you the fire cloak that saved him.",
    "A traveling tinker in need of supplies offers you a helm with a thunderbolt design on its top in exchange for food and water.  Curious, you accept, and later find out that the helm is magical.",
    "An icicle withstanding the full heat of the noonday sun attracts your attention.  Intrigued, you break it off, and find that it does not melt in your hand.",
    "Your wanderings bring you into contact with a tribe of ape-like beings using a magical lava rock that never cools to light their fires.  You take pity on them and teach them to make fire with sticks.  Believing you to be a god, the apes give you their rock.",
    "While waiting out a storm, a lighting bolt strikes a nearby cottage's lightning rod, which melts and falls to the ground.  The tip of the rod, however, survives intact and makes your hair stand on end when you touch it.  Hmm...",
    "You've found an oddly shaped ring on the finger of a long dead traveler.  The ring looks like a snake biting its own tail.",
    "A fierce windstorm reveals the entrance to a buried tomb.  Your investigation reveals that the tomb has already been looted, but the thieves overlooked an ankh on a silver chain in the dark.",
    "You come across a conjurer who begs to accompany you and your army awhile for safety.  You agree, and he offers as payment a copy of the book of the elements.",
    "While pausing to rest, you notice a bobcat climbing a short tree to get at a crow's nest.  On impulse, you climb the tree yourself and scare off the cat.  When you look in the nest, you find a collection of shiny stones and a ring.",
    "In your wanderings you come across a hermit living in a small, tidy hut.  Impressed with your mission, he takes time out from his meditations to bless and give you a charm against curses.",
    "Responding to cries for help, you find river Sprites making a sport of dunking an old man.  Feeling vengeful, you rescue the man and drag a Sprite onto dry land for awhile.  The Sprite, uncomfortable in the air, gives you a magic pendant to let him go.",
    "A brief roadside encounter with a small caravan and a game of knucklebones wins a magic pendant.  Its former owner says that it protects from Necromancers' death spells.",
    "The sounds of combat draw you to the scene of a fight between an old Barbarian and an eight-headed Hydra.  Your timely intervention swings the battle in favor of the man, and he rewards you with a pendant he used to use to calm his mind for battle.",
    "You come upon a very old woman, long blind from cataracts and dying alone.  You tend to her final needs and promise a proper burial.  Grateful, she gives you a magic pendant emblazoned with a stylized eye.  It lets you see with your eyes closed.",
    "You come across a golem wearing a glowing pendant and blocking your way.   Acting on a hunch, you cut the pendant from its neck.  Deprived of its power source, the golem breaks down, leaving you with the magical pendant.",
    "A quick and deadly battle with a Necromancer wins you his magical pendant.  Later, a Wizard tells you that the pendant protects undead under your control from holy word spells.",
    "You meet an old Wizard friend of yours traveling in the opposite direction.  He presents  you with a gift:  A wand that prevents the use of the dispel magic spell on your allies.",
    "A chance meeting with a famous Archer finds you in a game of knucklebones pitting his bow against your horse.  You win.",
    "A merchant from far away lands trades you a new invention of his people for traveling supplies.  It makes distant objects appear closer, and he calls it...\n\na telescope.",
    "You pause to help a diplomat with a broken axle fix his problem.  In gratitude, he gives you a writing quill with magical properties which he says will \"help people see things your way\".",
    "You see a Wizard fleeing from a Griffin and riding like the wind.  The Wizard opens a portal and rides through, getting his hat knocked off by the edge of the gate.  The Griffin follows; the gate closes.  You pick the hat up, dust it off, and put it on.",
    "You find a small tree that closely resembles the great Warlock Carnauth with a ring around one of its twigs.  Scraps of clothing and rotting leather lead you to suspect that it IS Carnauth, transformed.  Since you can't help him, you take the magic ring.",
    "An ammunition cart in the middle of an old battlefield catches your eye.  Inspection shows it to be in good working order, so  you take it along.",
    "Your big spending habits have earned you a massive tax bill that you can't hope to pay.  The tax man takes pity and agrees to only take 250 gold a day from your account for life.  Check here if you want one dollar to go to the presidential campaign election fund.",
    "Your looting of the grave of Sinfilas Gardolad, the famous shapeshifting Warlock, unearths his fabled mask.  Trembling, you put it on and it twists your visage into an awful grimace!  Oh no!  It's actually the hideous mask of Gromluck Greene, and you are stuck with it.",
    "You visit an alchemist who, upon seeing your army, is swayed by the righteousness of your cause.  The newly loyal subject gives you his endless pouch of sulfur to help with the war effort.",
    "A brief stop at a hastily abandoned Wizard's tower turns up a magical vial of mercury that always has a little left on the bottom.  Recognizing a treasure when you see one, you cap it and slip it in your pocket.",
    "A short rainstorm brings forth a rainbow...and you can see the end of it.  Riding quickly, you seize the pot of gold you find there.  The leprechaun who owns it, unable to stop you from taking it, offers an endless pouch of gems for the return of his gold.  You accept.",
    "Pausing to rest and light a cook fire, you pull wood out of a nearby pile of dead wood.  As you keep pulling wood from the pile, you notice that it doesn't shrink.  You realize to your delight that the wood is enchanted, so you take it along.",
    "You've found a Goblin weapon smithy making weapons for use against humans.  With a tremendous yell you and your army descend upon their camp and drive them away.  A search finds a magic ore cart that never runs out of iron.",
    "Taking shelter from a storm in a small cave,  you notice a small patch of crystal in one corner.  Curious, you break a piece off and notice that the original crystal grows the lost piece back.  You decide to stuff the entire patch into a pouch and take it with you.",
    "Your army is ambushed by a small tribe of wild (and none too bright) Orcs.  You fend them off easily and the survivors flee in all directions.  One of the Orcs was wearing a polished spiked helm.  Figuring it will make a good souvenir, you take it.",
    "You come upon a bridge spanning a dry gully.  Before you can cross, a Troll steps out from under the bridge and demands payment before it will permit you to pass.  You refuse, and the Troll charges, forcing you to slay it.  You take its spiked shield as a trophy.",
    "A walk across a dry saltwater lake bed yields an unlikely prize:  A white pearl amidst shattered shells and debris.",
    "Rumors of a Griffin of unusual size preying upon the countryside lead you to its cave lair.  A quick, brutal fight dispatches the beast, and a search of its foul nest turns up a huge black pearl.",
    "",
    "ERROR : Artifact event 82.",
    "ERROR : Artifact event 83.",
    "ERROR : Artifact event 84.",
    "ERROR : Artifact event 85.",
    "You find an elaborate container which houses an old vellum scroll. The runes on the container are very old, and the artistry with which it was put together is stunning. As you pull the scroll out, you feel imbued with magical power.",
    "One of the less intelligent members of your party picks up an arm off of the ground.  Despite its missing a body, it is still moving.  Your troops find the dismembered arm repulsive, but you cannot bring yourself to drop it: it seems to hold some sort of magical power that influences your decision making.",
    "You come upon a sign.  It reads: \"Here lies the body of Anduran.  Bow and swear fealty, and you shall be rewarded.\"  You decide to do as it says.  As you stand up, you feel a coldness against your skin.  Looking down, you find that you are suddenly wearing a gleaming, ornate breastplate.",
    "A kindly Sorceress thinks that your army's defenses could use a magical boost.  She offers to enchant the Broach that you wear on your cloak, and you accept.",
    "Out of pity for a poor peasant, you purchase a chest of old junk they are hawking for too much gold. Later, as you search through it, you find it contains the 3 pieces of the legendary battle garb of Anduran!",
    "You come upon a caravan of gypsies who are feasting and fortifying their bodies with mead.  They call you forward and say \"If you prove that you can dance the Rama-Buta, we will reward you.\"  You don't know it, but try anyway.  They laugh hysterically, but admire your bravery, giving you a Crystal Ball.",
    "You enter a recently burned glade and come upon a Fire Elemental sitting atop a rock.  It looks up, its flaming face contorted in a look of severe pain.  It then tosses a glowing object at you.  You put up your hands to block it, but it passes right through them and sears itself into your chest.",
    "Suddenly, a biting coldness engulfs your body.  You seize up, falling from your horse.  The pain subsides, but you still feel as if your chest is frozen.  As you pick yourself up off of the ground, you hear hearty laughter.  You turn around just in time to see a Frost Giant run off into the woods and disappear.",
    "You spy a gleaming object poking up out of the ground.   You send a member of your party over to investigate.  He comes back with a golden helmet in his hands.  You realize that it must be the helmet of the legendary Anduran, the only man who was known to wear solid gold armor.",
    "You come upon a battle where a Paladin has been mortally wounded by a group of Zombies.  He asks you to take his hammer and finish what he started.  As you pick it up, it begins to hum, and then everything becomes a blur.  The Zombies lie dead, the hammer dripping with blood.  You strap it to your belt.",
    "Upon cresting a small hill, you come upon a ridiculous looking sight.  A Sprite is attempting to carry a Scepter that is almost as big as it is.  Trying not to laugh, you ask, \"Need help?\"  The Sprite glares at you and answers: \"You think this is funny?  Fine.  You can carry it.  I much prefer flying anyway.\"",
    "An old seaman tells you a tale of an enchanted masthead that he used in his youth to rally his crew during times of trouble.  He then hands you a faded map that shows where he hid it.  After much exploring, you find it stashed underneath a nearby dock.",
    "You stop to help a Peasant catch a runaway mare.  To show his gratitude, he hands you a tiny sphere.  As soon as you grasp it, you feel the magical energy drain from your limbs...",
    "While out scaring up game, your troops find a mysterious staff levitating about three feet off of the ground.  They hand it to you, and you notice an inscription.  It reads: \"Brains best brawn and magic beats might.  Heed my words, and you'll win every fight.\"",
    "A former Captain of the Guard admires your quest and gives you the enchanted Sword Breaker that he relied on during his tour of duty.",
    "A Troll stops you and says: \"Pay me 5,000 gold, or the Sword of Anduran will slay you where you stand.\"  You refuse.  The troll grabs the sword hanging from its belt, screams in pain, and runs away.  Picking up the fabled sword, you give thanks that half-witted Trolls tend to grab the wrong end of sharp objects.",
    "A dirty shovel has been thrust into a dirt mound nearby. Upon investigation, you discover it to be the enchanted shovel of the Gravediggers, long thought lost by mortals.",
    0
};
DATA(0x004fe790) char *gStatNames[HERO_PRIMARY_STAT_COUNT] = {
    "Attack Skill",
    "Defense Skill",
    "Spell Power",
    "Knowledge"
};
DATA(0x004fe7a0) char *gStatDesc[HERO_PRIMARY_STAT_COUNT] = {
    "{Attack Skill}\n\nYour attack skill is a bonus added to each creature's attack skill.",
    "{Defense Skill}\n\nYour defense skill is a bonus added to each creature's defense skill.",
    "{Spell Power}\n\nYour spell power determines the length or power of a spell.",
    "{Knowledge}\n\nYour knowledge determines how many spell points your hero may have.  Under normal cirumstances, a hero is limited to 10 spell points per level of knowledge."
};
DATA(0x004fe7b0) char *gAlignmentNames[KB_ALIGNMENT_NAME_COUNT] = {
    "Knight",
    "Barbarian",
    "Sorceress",
    "Warlock",
    "Wizard",
    "Necromancer",
    "Multiple",
    "Random"
};
DATA(0x004fe7d0) char *gArmyShortNames[MONSTER_DATABASE_COUNT] = {
    "peasn",
    "archr",
    "arch2",
    "pikmn",
    "pikm2",
    "swman",
    "swma2",
    "cvlry",
    "cvlr2",
    "paldn",
    "pald2",
    "gobln",
    "orc__",
    "orc_2",
    "Wolf_",
    "Ogre_",
    "Ogre2",
    "Troll",
    "trol2",
    "cyclp",
    "sprit",
    "Dwarf",
    "dwar2",
    "elf__",
    "elf_2",
    "druid",
    "drui2",
    "uncrn",
    "phoen",
    "centr",
    "gargl",
    "griff",
    "mintr",
    "mint2",
    "Hydra",
    "dragn",
    "drag2",
    "drag3",
    "hlflg",
    "Boar_",
    "irong",
    "iron2",
    "roc__",
    "archm",
    "arch2",
    "titan",
    "tita2",
    "skel_",
    "zomb_",
    "zomb2",
    "Mummy",
    "mumm2",
    "vampr",
    "vamp2",
    "lich_",
    "lich2",
    "boned",
    "Rogue",
    "Nomad",
    "Ghost",
    "Genie",
    "medus",
    "eleme",
    "elema",
    "elemf",
    "elemw"
};
DATA(0x004fe8d8) char *gArmyNames[MONSTER_DATABASE_COUNT] = {
    "peasant",
    "archer",
    "ranger",
    "pikeman",
    "veteran pikeman",
    "swordsman",
    "master swordsman",
    "cavalry",
    "champion",
    "paladin",
    "crusader",
    "goblin",
    "orc",
    "orc chief",
    "wolf",
    "ogre",
    "ogre lord",
    "troll",
    "war troll",
    "cyclops",
    "sprite",
    "dwarf",
    "battle dwarf",
    "elf",
    "grand elf",
    "druid",
    "greater druid",
    "unicorn",
    "phoenix",
    "centaur",
    "gargoyle",
    "griffin",
    "minotaur",
    "minotaur king",
    "hydra",
    "green dragon",
    "red dragon",
    "black dragon",
    "halfling",
    "boar",
    "iron golem",
    "steel golem",
    "roc",
    "mage",
    "archmage",
    "giant",
    "titan",
    "skeleton",
    "zombie",
    "mutant zombie",
    "mummy",
    "royal mummy",
    "vampire",
    "vampire lord",
    "lich",
    "power lich",
    "bone dragon",
    "rogue",
    "nomad",
    "ghost",
    "genie",
    "medusa",
    "earth elemental",
    "air elemental",
    "fire elemental",
    "water elemental"
};
DATA(0x004fe9e0) char *gArmyNamesPlural[MONSTER_DATABASE_COUNT] = {
    "peasants",
    "archers",
    "rangers",
    "pikemen",
    "veteran pikemen",
    "swordsmen",
    "master swordsmen",
    "cavalries",
    "champions",
    "paladins",
    "crusaders",
    "goblins",
    "orcs",
    "orc chiefs",
    "wolves",
    "ogres",
    "ogre lords",
    "trolls",
    "war troll",
    "cyclopes",
    "sprites",
    "dwarves",
    "battle dwarves",
    "elves",
    "grand elves",
    "druids",
    "greater druids",
    "unicorns",
    "phoenix",
    "centaurs",
    "gargoyles",
    "griffins",
    "minotaurs",
    "minotaur kings",
    "hydras",
    "green dragons",
    "red dragons",
    "black dragons",
    "halflings",
    "boars",
    "iron golems",
    "steel golems",
    "rocs",
    "magi",
    "archmagi",
    "giants",
    "titans",
    "skeletons",
    "zombies",
    "mutant zombies",
    "mummies",
    "royal mummies",
    "vampires",
    "vampire lords",
    "liches",
    "power liches",
    "bone dragons",
    "rogues",
    "nomads",
    "ghosts",
    "genies",
    "medusas",
    "earth elementals",
    "air elementals",
    "fire elementals",
    "water elementals"
};
DATA(0x004feae8) char *gTerrainNames[KB_TERRAIN_TYPE_COUNT] = {
    "Ocean",
    "Grass",
    "Snow",
    "Swamp",
    "Lava",
    "Desert",
    "Dirt",
    "Wasteland",
    "Beach",
    0
};
DATA(0x004feb10) char *gResourceNames[RESOURCE_VALUE_COUNT] = {
    "Wood",
    "Mercury",
    "Ore",
    "Sulfur",
    "Crystal",
    "Gems",
    "Gold",
    0
};
DATA(0x004feb30) char *gQuickViewText[KB_QUICK_VIEW_TEXT_COUNT] = {
    "",
    "Alchemist Lab",
    "Sign",
    "Buoy",
    "Skeleton",
    "Daemon Cave",
    "Treasure Chest",
    "Faerie Ring",
    "Campfire",
    "Fountain",
    "Gazebo",
    "Ancient Lamp",
    "Graveyard",
    "Archer's House",
    "Goblin Hut",
    "Dwarf Cottage",
    "Peasant Hut",
    "Log Cabin",
    "Road",
    "Event",
    "Dragon City",
    "Lighthouse",
    "Water Wheel",
    "Mine",
    "Army Camp",
    "Obelisk",
    "Oasis",
    "Resource",
    "",
    "Sawmill",
    "Oracle",
    "Shrine of the First Circle",
    "Shipwreck",
    "Sea Chest",
    "Desert Tent",
    "Town",
    "Stone Liths",
    "Wagon Camp",
    "Well",
    "Whirlpool",
    "Windmill",
    "Artifact",
    "Hero",
    "Boat",
    "",
    "Random Artifact",
    "Random Resource",
    "Random Monster",
    "Random Town",
    "Random Castle",
    "",
    "Random Monster - weak",
    "Random Monster - medium",
    "Random Monster - strong",
    "Random Monster - very strong",
    "Random Hero",
    "Nothing Special",
    "",
    "Watch Tower",
    "Tree House",
    "Tree City",
    "Ruins",
    "Fort",
    "Trading Post",
    "Abandoned Mine",
    "Dwarf Cabin",
    "Standing Stones",
    "Idol",
    "Tree of Knowledge",
    "Witch Doctor's Hut",
    "Temple",
    "Hill Fort",
    "Halfling Hole",
    "Mercenary Camp",
    "Shrine of the Second Circle",
    "Shrine of the Third Circle",
    "Pyramid",
    "City of the Dead",
    "Excavation",
    "Sphinx",
    "Wagon",
    "Tar Pit",
    "Artesian Spring",
    "Troll Bridge",
    "Watering Hole",
    "Witch's Hut",
    "Xanadu",
    "Cave",
    "Lean-To",
    "Magellan's Maps",
    "Flotsam",
    "Derelict Ship",
    "Shipwreck Survivor",
    "Bottle",
    "Magic Well",
    "Magic Garden",
    "Observation Tower",
    "Freeman's Foundry",
    "Stream",
    "Trees",
    "Mountains",
    "Volcano",
    "Flowers",
    "Rock",
    "Lake",
    "Mandrake",
    "Dead Tree",
    "Stump",
    "Crater",
    "Cactus",
    "Mound",
    "Dune",
    "Lava Pool",
    "Shrub",
    "Hole",
    "Outcropping",
    "Random Artifact - Treasure",
    "Random Artifact - Minor",
    "Random Artifact - Major",
    "%s Barrier",
    "%s Traveller's Tent",
    "%s",
    "%s",
    "Jail"
};
DATA(0x004fed20) char *gEventText[KB_EVENT_TEXT_TABLE_COUNT] = {
    "{Alchemist}\n\nYou have taken control of the local Alchemist shop. It will provide you with one unit of Mercury per day.",
    "{Signpost}\n\nA signpost reads:}\n\n%s is near.",
    "{Buoy}\n\nYour men spot a navigational buoy, confirming that you are on course.",
    "{Buoy}\n\nYour men spot a navigational buoy, confirming that you are on course and increasing their morale.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "{Faerie Ring}\n\nYou enter the faerie ring, but nothing happens.",
    "{Faerie Ring}\n\nUpon entering the mystical faerie ring, your army gains luck for its next battle.",
    "{Campfire}\n\nRansacking an enemy camp, you discover a hidden cache of treasures.",
    "{Fountain}\n\nYou drink from the enchanted fountain, but nothing happens.",
    "{Fountain}\n\nAs you drink the sweet water, you gain luck for your next battle.",
    "{Gazebo}\n\nAn old Knight appears on the steps of the gazebo. \"I am sorry, my liege, I have taught you all I can.\"",
    "{Gazebo}\n\nAn old Knight appears on the steps of the gazebo. \"My liege, I will teach you all that I know to aid you in your travels.\"",
    "{Genie Lamp}\n\nYou stumble upon a dented and tarnished lamp lodged deep in the earth. Do you wish to rub the lamp?",
    "{Graveyard}\n\nYou tentatively approach the burial ground of ancient warriors.  Do you want to search the graves?",
    "Upon defeating the Zombies you spend several hours searching the graves and find nothing.  Such a despicable act reduces your army's morale.",
    "Upon defeating the zomies you search the graves and find something!",
    "{Archer's House}\n\nA group of Archers with a desire for greater glory wish to join you. Do you accept? ",
    "You are unable to recruit at this time, your ranks are full.",
    "{Archer's House}\n\nAs you approach the dwelling, you notice that there is no one here.",
    "{Goblin Hut}\n\nA group of Goblins with a desire for greater glory wish to join you. Do you accept?",
    "You are unable to recruit at this time, your ranks are full.",
    "{Goblin Hut}\n\nAs you approach the dwelling, you notice that there is no one here.",
    "{Peasant Hut}\n\nA group of Peasants with a desire for greater glory wish to join you. Do you accept? ",
    "You are unable to recruit at this time, your ranks are full.",
    "{Peasant Hut}\n\nAs you approach the dwelling, you notice that there is no one here.",
    "{Dwarf Cottage}\n\nA group of Dwarves with a desire for greater glory wish to join you. Do you accept? ",
    "You are unable to recruit at this time, your ranks are full.",
    "{Dwarf Cottage}\n\nAs you approach the dwelling, you notice that there is no one here.",
    "{Thatched Hut}\n\nA group of Peasants with a desire for greater glory wish to join you. Do you accept? ",
    "You are unable to recruit at this time, your ranks are full.",
    "{Thatched Hut}\n\nAs you approach the Peasant dwelling you notice that there is no one here.",
    "{Tree House}\n\nA group of Sprites with a desire for greater glory wish to join you. Do you accept? ",
    "You are unable to recruit at this time, your ranks are full.",
    "{Tree House}\n\nAs you approach the Sprites' tree house you notice that there is no one here.",
    "{Halfling Hole}\n\nA group of Halflings with a desire for greater glory wish to join you. Do you accept? ",
    "You are unable to recruit at this time, your ranks are full.",
    "{Halfling Hole}\n\nAs you approach the habitat you notice that there is no one here.",
    "{Watch Tower}\n\nA group of Orcs with a desire for greater glory wish to join you. Do you accept? ",
    "You are unable to recruit at this time, your ranks are full.",
    "{Watch Tower}\n\nAs you approach the Orcs' watch tower you notice that there is no one here.",
    "{Snow Cave}\n\nA group of Centaurs with a desire for greater glory wish to join you. Do you accept? ",
    "You are unable to recruit at this time, your ranks are full.",
    "{Cave}\n\nAs you approach the Centaurs' cave you notice that there is no one here.",
    "{Excavation}\n\nA group of Skeletons with a desire for greater glory wish to join you. Do you accept? ",
    "You are unable to recruit at this time, your ranks are full.",
    "{Excavation}\n\nAs you approach the Skeletons' excavation you notice that there is no one here.",
    "",
    "",
    "",
    "",
    "",
    "{Lighthouse}\n\nThe lighthouse is now under your control, and all of your ships will now move further each turn.",
    "{Water Wheel}\n\nThe keeper of the mill announces: \"Milord, I am sorry, there is no gold currently available.  Please try again next week.\"",
    "{Water Wheel}\n\nThe keeper of the mill announces: \"Milord, I have been working very hard to provide you with this gold, come back next week for more.\"",
    "{Ore Mine}\n\nYou gain control of an ore mine. It will provide you with two units of ore per day.",
    "{Sulfur Mine}\n\nYou gain control of a sulfur mine. It will provide you with one unit of sulfur per day.",
    "{Crystal Mine}\n\nYou gain control of a crystal mine. It will provide you with one unit of crystal per day.",
    "{Gem Mine}\n\nYou gain control of a gem mine. It will provide you with one unit of gems per day.",
    "{Gold Mine}\n\nYou gain control of a gold mine. It will provide you with 1000 gold per day.",
    "{Followers}\n\nA group of %s with a desire for greater glory wish to join you. Do you accept? ",
    "Insulted by your refusal of their offer, the monsters attack!",
    "{Obelisk}\n\nYou come upon an obelisk made from a type of stone you have never seen before.  Staring at it intensely, the smooth surface suddenly changes to an inscription.  The inscription is a piece of a lost ancient map.  Quickly you copy down the piece and the inscription vanishes as abruptly as it had appeared.",
    "{Obelisk}\n\nYou have already been to this obelisk.",
    "",
    "",
    "You find a small quantity of %s.",
    "{Sawmill}\n\nYou gain control of a sawmill. It will provide you with two units of wood per day.",
    "{Oracle}\n\nNestled among the trees sits a blind seer. After explaining the intent of your journey, the seer activates his crystal ball, allowing you to see the strengths and weaknesses of your opponents.",
    "",
    "",
    "",
    "",
    "",
    "",
    "{Desert Tent}\n\nA group of tattered tents, billowing in the sandy wind, beckons you.  The tents are unoccupied.  Perhaps more Nomads will be here later.",
    "{Desert Tent}\n\nA group of tattered tents, billowing in the sandy wind, beckons you.  Do you wish to have any Nomads join you during your travels?",
    "{Wagon}\n\nA colorful Rogues' wagon stands empty here.  Perhaps more Rogues will be here later.",
    "{Wagon}\n\nDistant sounds of music and laughter draw you to a colorful wagon housing Rogues.  Do you wish to have any Rogues join your army?",
    "{Whirlpool}\n\nA whirlpool engulfs your ship.  Some of your army has fallen overboard.",
    "{Windmill}\n\nThe keeper of the mill announces: \"Milord, I am sorry, there are no resources currently available. Please try again next week.\"",
    "{Windmill}\n\nThe keeper of the mill announces: \"Milord, I have been working very hard to provide you with these resources, come back next week for more.\"",
    "",
    "",
    "",
    "",
    "",
    "{Skeleton}\n\nYou come upon the remains of an unfortunate adventurer.  Searching through the tattered clothing, you find nothing.",
    "{Skeleton}\n\nYou come upon the remains of an unfortunate adventurer.  Searching through the tattered clothing, you find ",
    0
};
DATA(0x004feea0) char *gCPanelHelp[KB_CONTROL_PANEL_HELP_COUNT] = {
    "Start a single or multi-player game.",
    "Load a previously saved game.",
    "Save the current game.",
    "Quit out of Heroes of Might and Magic II.",
    "Exit this menu without doing anything.",
    0
};
DATA(0x004feeb8) char *gCSPanelHelp[KB_COMBAT_SPELL_PANEL_HELP_COUNT] = {
    "{OK}\n\nExit this menu.",
    "{Speed}\n\nSet the speed of combat actions and animations.",
    "{Monster Info}\n\nToggle the monster info window, which shows information on the active and targeted monsters.",
    "{Auto Combat\nSpell Casting}\n\nToggle whether or not the computer will cast spells for you when auto combat is on.  (Note: This does not affect spell casting for computer players in any way, nor does it affect quick combat.)",
    "{Grid}\n\nToggle the hex grid on or off.  The hex grid always underlies movement, even if turned off.  This switch only determines if the grid is visible.",
    "{Shadow Movement}\n\nToggle on or off a shadowing system showing where your creatures can move and attack.  When this is turned on, there is a slight performance cost, most noticeable on 486 computers.",
    "{Shadow Cursor}\n\nToggle on or off a shadowing system showing where the hex grid that the mouse cursor is currently over is shadowed.  When this is turned on, there is a slight performance cost, most noticeable on 486 computers.",
    0
};
DATA(0x004feed8) char *gAPanelHelp[KB_ADVENTURE_PANEL_HELP_COUNT] = {
    "View the entire world.",
    "View the obelisk puzzle.",
    "View information on the scenario you are currently playing.",
    "Dig for the Ultimate Artifact.",
    "Exit this menu without doing anything.",
    0
};
DATA(0x004feef0) char *gInitMenuHelp[KB_INIT_MENU_HELP_COUNT] = {
    "{New Game}\n\nStart a single or multi-player game.",
    "{Load Game}\n\nLoad a previously saved game.",
    "{High Scores}\n\nView the high score screen.",
    "{Credits}\n\nView the credits screen.",
    "{Quit}\n\nQuit Heroes of Might and Magic and return to the operating system.",
    0
};
DATA(0x004fef08) char *gAdvMenuHelp[KB_ADVENTURE_MENU_HELP_COUNT] = {
    "{Next Hero}\n\nSelect the next Hero.",
    "{Continue Movement}\n\nContinue the Hero's movement along the current path.",
    "{Kingdom Summary}\n\nView a summary of your kingdom.",
    "{End Turn}\n\nEnd your turn and let the computer take its turn.",
    "{Adventure Options}\n\nBring up the adventure options menu.",
    "{File Options}\n\nBring up the file options menu, allowing you to load, save etc.",
    "{System Options}\n\nBring up the system options menu, allowing you to customize your game.",
    "{Cast Spell}\n\nCast an adventure spell."
};
DATA(0x004fef28) char *gLuckText[KB_LUCK_TEXT_COUNT] = {
    "Cursed",
    "Awful",
    "Bad",
    "Normal",
    "Good",
    "Great",
    "Irish",
    0
};
DATA(0x004fef48) char *gMoraleText[KB_MORALE_TEXT_COUNT] = {
    "Treason",
    "Awful",
    "Poor",
    "Normal",
    "Good",
    "Great",
    "Blood!",
    0
};
DATA(0x004fef68) char *onOffText[KB_ON_OFF_TEXT_COUNT] = {
    "Off",
    "On",
    "On\nVolume 9",
    "On\nVolume 8",
    "On\nVolume 7",
    "On\nVolume 6",
    "On\nVolume 5",
    "On\nVolume 4",
    "On\nVolume 3",
    "On\nVolume 2",
    "On\nVolume 1",
    0
};
DATA(0x004fef98) char *walkSpeedText[KB_WALK_SPEED_TEXT_COUNT] = {
    "Walk",
    "Trot",
    "Canter",
    "Gallop",
    "Jump",
    0
};
DATA(0x004fefb0) char *gColors[HERO_CLASS_COUNT] = {
    "blue",
    "green",
    "red",
    "yellow",
    "orange",
    "purple"
};
DATA(0x004fefc8) char *gMonthNames[KB_MONTH_NAME_COUNT] = {
    "Grasshopper",
    "Ant",
    "Dragonfly",
    "Spider",
    "Butterfly",
    "Bumblebee",
    "Locust",
    "Earthworm",
    "Hornet",
    "Beetle"
};
DATA(0x004feff0) char *gWeekNames[KB_WEEK_NAME_COUNT] = {
    "Squirrel",
    "Rabbit",
    "Gopher",
    "Badger",
    "Rat",
    "Eagle",
    "Weasel",
    "Raven",
    "Mongoose",
    "Dog",
    "Aardvark",
    "Lizard",
    "Tortoise",
    "Hedgehog",
    "Condor",
    0
};
DATA(0x004ff030) char *cHeroScreen[KB_HERO_SCREEN_TEXT_COUNT] = {
    "Kingdom Overview",
    "View %s Info",
    "Additional hero characteristics",
    "View Good Morale Info",
    "View Neutral Morale Info",
    "View Bad Morale Info",
    "View Good Luck Info",
    "View Neutral Luck Info",
    "View Bad Luck Info",
    "View Experience Info",
    "Select %s",
    "Empty",
    "Move %s",
    "Exchange %s with %s",
    "View Spells",
    "View %s Info",
    "Dismiss %s the %s",
    "Exit Hero Screen",
    "Hero Screen",
    "Combine %s",
    "Split %s",
    "View %s %s Info",
    "View Spell Points Info",
    "Set army combat formation to 'Spread'",
    "Set army combat formation to 'Grouped'",
    0
};
DATA(0x004ff098) char *cCastleInfo[KB_CASTLE_INFO_TEXT_COUNT] = {
    "Build Mage Guild",
    "Mage Guild is at highest level.",
    "Cannot afford next level.",
    "Add another level to Mage Guild",
    "%s is already built",
    "Cannot build %s",
    "Cannot afford %s",
    "Build %s",
    "Cannot afford a Hero.",
    "Cannot recruit - you already have %d Heroes.",
    "Cannot recruit - you already have a Hero in this town.",
    "Recruit %s the %s",
    "Exit Castle",
    "Castle Options",
    "Set garrison combat formation to 'Grouped'",
    "Set garrison combat formation to 'Spread'"
};
DATA(0x004ff0d8) char *cLuckInfo[KB_LUCK_INFO_TEXT_COUNT] = {
    "{Good Luck}\n\nGood luck sometimes lets your armies get lucky attacks (double strength) in combat.",
    "{Neutral Luck}\n\nNeutral luck means your armies will never get lucky or unlucky attacks on the enemy.",
    "{Bad Luck}\n\nBad luck sometimes falls on your armies in combat, causing their attacks to only do half damage.",
    "%s\n\n\nCurrent Luck Modifiers:",
    "\nLucky Rabbit's Foot +1",
    "\nGolden Horseshoe +1",
    "\nGambler's Lucky Coin +1",
    "\nFour-Leaf Clover +1",
    "\nFaerie ring visited +1",
    "\nFountain visited +1",
    "\nnone",
    "\nTomb robber -1",
    "\nSorceress Rainbow +2",
    "\nIdol visited +1",
    "\nPyramid raided -2",
    "\nBasic Luck +1",
    "\nAdvanced Luck +2",
    "\nExpert Luck +3",
    "\nMasthead bonus at sea +1",
    "\nMermaid visited +1",
    "\nBattle Garb of Anduran gives you maximum luck.",
    0
};
DATA(0x004ff130) char *IQnames[KB_IQ_NAME_COUNT] = {
    "None",
    "Dumb",
    "Average",
    "Smart",
    "Genius",
    0
};
DATA(0x004ff148) char *cSpellHelp[KB_SPELL_HELP_TEXT_COUNT] = {
    "View previous page",
    "View next page",
    "View adventure Spells",
    "View Combat Spells",
    "Close Spellbook",
    "View Spells",
    "Select Spell",
    "View Combat Spells",
    "Your hero has %d spell points remaining",
    0
};
DATA(0x004ff170) char *speedText[KB_SPEED_TEXT_COUNT] = {
    "",
    "Crawling",
    "Very slow",
    "Slow",
    "Average",
    "Fast",
    "Very Fast",
    "Ultra Fast",
    "Blazing",
    "Instant"
};
DATA(0x004ff198) char *cArmyDetail[KB_ARMY_DETAIL_TEXT_COUNT] = {
    "Attack Skill: ",
    "Defense Skill: ",
    "Shots left: ",
    "Damage: ",
    "Hit Points: ",
    "Speed: ",
    "Morale: ",
    "Luck: ",
    "Shots: ",
    0
};
DATA(0x004ff1c0) char *cWellDetail[KB_WELL_DETAIL_TEXT_COUNT] = {
    "Attack: ",
    "Defense: ",
    "Shots: ",
    "Damg: ",
    "HP: ",
    "Speed: ",
    "Available: ",
    "\n\nSpeed:\n%s",
    "\n\nGrowth\n + %d / week",
    0
};
DATA(0x004ff1e8) char *cKingdomOverview[KB_KINGDOM_OVERVIEW_TEXT_COUNT] = {
    "Kingdom Overview     Month %d, Week %d, Day %d",
    "You own Dragon City.",
    "You own the Lighthouse.",
    0
};
DATA(0x004ff1f8) char *cNewTurn[KB_NEW_TURN_TEXT_COUNT] = {
    "%s, you only have %d days left to capture a town, or you will be banished from this land.",
    "%s, this is your last day to capture a town, or you will be banished from this land.",
    "Astrologers proclaim month of the %s.\n\nAll dwellings increase population.",
    "Astrologers proclaim month of the %s.\n\n%s population doubles!\n\nAll dwellings increase population.",
    "Astrologers proclaim month of the PLAGUE!\n\nAll populations are halved.",
    "Astrologers proclaim week of the %s.\n\nAll dwellings increase population.",
    "Astrologers proclaim week of the %s.\n\n%s growth +5.\n\nAll dwellings increase population.",
    0
};
DATA(0x004ff218) char *cViewGeneralLabels[KB_VIEW_GENERAL_LABEL_COUNT] = {
    "Attack: ",
    "Defense: ",
    "Spell Power: ",
    "Knowledge: ",
    "Morale: ",
    "Luck: ",
    "Spell Points: ",
    0
};
DATA(0x004ff238) char *cViewGeneralHelp[KB_VIEW_GENERAL_HELP_COUNT] = {
    "Stop Catapult",
    "Cast Spell",
    "Retreat",
    "Surrender",
    "Cancel",
    "Hero's Options",
    "Captain's Options",
    0
};
DATA(0x004ff258) char *cViewGeneralLongHelp[KB_VIEW_GENERAL_LONG_HELP_COUNT] = {
    "{Cast Spell}\n\nCast a magical spell.  You may only cast one spell per combat round.  The round is reset when every creature has had a turn.",
    "{Retreat}\n\nRetreat your hero, abandoning your creatures.  Your hero will be available for you to recruit again, however, the hero will have only a novice hero's forces.",
    "{Surrender}\n\nSurrendering costs gold.  However, if you pay the ransom, the hero and all of his or her surviving creatures will be available to recruit again.",
    "{Cancel}\n\nReturn to the battle."
};
DATA(0x004ff268) char *cCombatMessage[KB_COMBAT_MESSAGE_COUNT] = {
    "",
    "Move %s here.",
    "Fly %s here.",
    "Attack %s",
    "Shoot %s(%d shot(s) left)",
    "Hero's Options",
    "View Opposing Hero",
    "View %s info.",
    "No shots left!",
    "Captain's Options",
    "View Opposing Captain",
    "View Ballista Info"
};
DATA(0x004ff298) char *cHeroLevel[KB_HERO_LEVEL_TEXT_COUNT] = {
    "%s has gained",
    " a level.\n",
    " %d levels.\n",
    0
};
DATA(0x004ff2a8) char *cCombatHelp[KB_COMBAT_HELP_COUNT] = {
    "Wait for other units to go.",
    "Skip this unit",
    "Auto combat",
    "Customize system options.",
    "",
    0
};
DATA(0x004ff2c0) char *cLongCombatHelp[KB_LONG_COMBAT_HELP_COUNT] = {
    "{Wait}\n\nHave the current creature wait to take its turn until all the other creatures have gone.  The creature does not lose its turn.",
    "{Skip}\n\nSkips the current creature.  The current creature loses its turn and does not get to go again until the next round.",
    "{Auto Combat}\n\nAllows the computer to fight out the battle for you.",
    "{System Options}\n\nAllows you to customize the combat screen.",
    "{Message Bar}\n\nShows the results of individual monster's actions.",
    0
};
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
DATA(0x004ff348) char *gHeroDefaultNames[54] = {
    "Lord Kilburn", "Sir Gallant", "Ector", "Gwenneth", "Tyro", "Ambrose",
    "Ruby", "Maximus", "Dimitri", "Thundax", "Fineous", "Jojosh",
    "Crag Hack", "Jezebel", "Jaclyn", "Ergon", "Tsabu", "Atlas",
    "Astra", "Natasha", "Troyan", "Vatawna", "Rebecca", "Gem",
    "Ariel", "Carlawn", "Luna", "Arie", "Alamar", "Vesper", "Crodo",
    "Barok", "Kastore", "Agar", "Falagar", "Wrathmont", "Myra", "Flint",
    "Dawn", "Halon", "Myrini", "Wilfrey", "Sarakin", "Kalindra",
    "Mandigal", "Zom", "Darlana", "Zam", "Ranloo", "Charity", "Rialdo",
    "Roxana", "Sandro", "Celia"
};
DATA(0x004ff420) char *gNewGameHelp[KB_NEW_GAME_HELP_COUNT] = {
    "{Game Difficulty}\n\nThis lets you change the starting difficulty at which you will play.  Higher difficulty levels start you off with fewer resources, and at the higher settings, give extra resources to the computer.",
    "{Handicap}\n\nThis lets you change the handicap of a particular player.  Only humans may be handicapped.  Handicapped players start with fewer resources and earn 15 or 30% fewer resources per turn for mild and severe handicaps, respectively.",
    "{Opponents}\n\nThis lets you change player starting positions and colors.  A particular color will always start in a particular location.  Some positions may only be played by a computer player or only by a human player.",
    "{Class}\n\nThis lets you change the class of a player.  Classes are not always changeable.  Depending on the scenario, a player may receive additional towns and/or heroes not of their primary alignment.",
    "{Scenario}\n\nClick here to select which scenario to play.",
    "{Difficulty Rating}\n\nThe difficulty rating reflects a combination of various settings for your game.  This number will be applied to your final score.",
    "{OK}\n\nClick to accept these settings and start a new game.",
    "{Cancel}\n\nClick to return to the main menu."
};
DATA(0x004ff440) char *gSetupBaudHelp[KB_SETUP_BAUD_HELP_COUNT] = {
    "{2400 Baud}\n\nUse a 2400 baud connection speed. \n\nNote: For a 14400 baud modem, use the 19200 baud speed.  For a 28800 baud modem, use the 38400 baud speed.",
    "{9600 Baud}\n\nUse a 9600 baud connection speed. \n\nNote: For a 14400 baud modem, use the 19200 baud speed.  For a 28800 baud modem, use the 38400 baud speed.",
    "{19200 Baud}\n\nUse a 19200 baud connection speed.\n\nNote: For a 14400 baud modem, use the 19200 baud speed.  For a 28800 baud modem, use the 38400 baud speed.",
    "{38400 Baud}\n\nUse a 38400 baud connection speed.\n\nNote: For a 14400 baud modem, use the 19200 baud speed.  For a 28800 baud modem, use the 38400 baud speed.",
    "{Cancel}\n\nCancel back to the main menu.",
    0
};
DATA(0x004ff458) char *gSetupComPortHelp[KB_SETUP_COM_PORT_HELP_COUNT] = {
    "{COM 1}\n\nUse COM Port 1 for the modem connection.",
    "{COM 2}\n\nUse COM Port 2 for the modem connection.",
    "{COM 3}\n\nUse COM Port 3 for the modem connection.",
    "{COM 4}\n\nUse COM Port 4 for the modem connection.",
    "{Cancel}\n\nCancel back to the main menu.",
    0
};
DATA(0x004ff470) char *gSetupDCBaudHelp[KB_SETUP_DC_BAUD_HELP_COUNT] = {
    "{2400 Baud}\n\nUse a 2400 baud connection speed. \n\nNote: In general, computers with the older UART 8250 chip should use 19200 baud, and computers with the newer UART 16550 chip should use 38400 baud.  When in doubt, try slower speeds first, and if they work, then try faster speeds.  Most computers made in 1994 or later have a UART 16550 chip.",
    "{9600 Baud}\n\nUse a 9600 baud connection speed. \n\nNote: In general, computers with the older UART 8250 chip should use 19200 baud, and computers with the newer UART 16550 chip should use 38400 baud.  When in doubt, try slower speeds first, and if they work, then try faster speeds.  Most computers made in 1994 or later have a UART 16550 chip.",
    "{19200 Baud}\n\nUse a 19200 baud connection speed.\n\nNote: In general, computers with the older UART 8250 chip should use 19200 baud, and computers with the newer UART 16550 chip should use 38400 baud.  When in doubt, try slower speeds first, and if they work, then try faster speeds.  Most computers made in 1994 or later have a UART 16550 chip.",
    "{38400 Baud}\n\nUse a 38400 baud connection speed.\n\nNote: In general, computers with the older UART 8250 chip should use 19200 baud, and computers with the newer UART 16550 chip should use 38400 baud.  When in doubt, try slower speeds first, and if they work, then try faster speeds.  Most computers made in 1994 or later have a UART 16550 chip.",
    "{Cancel}\n\nCancel back to the main menu.",
    0
};
DATA(0x004ff488) char *gSetupDCComPortHelp[KB_SETUP_DC_COM_PORT_HELP_COUNT] = {
    "{COM 1}\n\nUse COM Port 1 for the direct connection.",
    "{COM 2}\n\nUse COM Port 2 for the direct connection.",
    "{COM 3}\n\nUse COM Port 3 for the direct connection.",
    "{Com 4}\n\nUse COM Port 4 for the direct connection.",
    "{Cancel}\n\nCancel back to the main menu.",
    0
};
DATA(0x004ff4a0) char *gSetupHotSeatGameHelp[KB_SETUP_HOT_SEAT_HELP_COUNT] = {
    "{2 Players}\n\nPlay with 2 human players, and optionally, up to 4 additional computer players.",
    "{3 Players}\n\nPlay with 3 human players, and optionally, up to 3 additional computer players.",
    "{4 Players}\n\nPlay with 4 human players, and optionally, up to 2 additional computer players.",
    "{5 Players}\n\nPlay with 5 human players, and optionally 1 computer player.",
    "{6 Players}\n\nPlay with 6 human players.",
    "{Cancel}\n\nCancel back to the main menu."
};
DATA(0x004ff4b8) char *gSetupModemGameHelp[KB_SETUP_MODEM_HELP_COUNT] = {
    "{Host}\n\nThe host sets up the game options, chooses the number to dial, and places the call.",
    "{Guest}\n\nThe guest waits for the host to call and set up the game.",
    "{Config}\n\nChange your modem configuration.",
    "{Cancel}\n\nCancel back to the main menu."
};
DATA(0x004ff4c8) char *gSetupDCGameHelp[KB_SETUP_DIRECT_CONNECT_HELP_COUNT] = {
    "{Host}\n\nThe host sets up the game options.",
    "{Guest}\n\nThe guest waits for the host to set up the game.",
    "{Config}\n\nChange your direct connect port configuration.",
    "{Cancel}\n\nCancel back to the main menu."
};
DATA(0x004ff4d8) char *gSetupMultiPlayerGameHelp[KB_SETUP_MULTIPLAYER_HELP_COUNT] = {
    "{Hot Seat}\n\nPlay a Hot Seat game, where 2 to 4 players play around the same computer, switching into the 'Hot Seat' when it is their turn.",
    "{Network}\n\nPlay a network game, where 2 players use their own computers connected through a LAN (Local Area Network).",
    "{Modem}\n\nPlay a modem game, where 2 players use their own computers connected over the phone lines using modems.",
    "{Direct Connect}\n\nPlay a direct connect game, where 2 players use their own computers directly connected through their serial port by a null modem.",
    "{Cancel}\n\nCancel back to the main menu.",
    0
};
DATA(0x004ff4f0) char *gSetupNetworkGameHelp[KB_SETUP_NETWORK_HELP_COUNT] = {
    "{Host}\n\nThe host sets up the game options.  There can only be one host per network game.",
    "{Guest}\n\nThe guest waits for the host to set up the game, then is automatically added in.  There can be multiple guests for TCP/IP and IPX games, but only 1 guest for NetBIOS games.",
    "{Cancel}\n\nCancel back to the main menu.",
    0
};
DATA(0x004ff500) char *gSetupNetworkGame2Help[KB_SETUP_NETWORK_SECOND_HELP_COUNT] = {
    "{IPX}\n\nIPX networking is the most commonly used form of network in Windows 95.  Up to 6 players can play using IPX.  Only the Windows 95 version of Heroes II supports IPX networking.",
    "{TCP/IP}\n\nTCP/IP is most commonly used as a way of connecting machines over the internet.  Up to 6 players can play using IPX.  To connect over the internet, you must have an account with an ISP (Independent Service Provider).  Only the Windows 95 version of Heroes II supports TCP/IP networking.",
    "{NETBios}\n\nNETBios networking is the only form of networking available for DOS machines, and is also available for Windows 95.  Only two players may connect with NetBIOS.  If all machines you wish to connect are using the Windows 95 version of Heroes II, then IPX networking is recommended.",
    "{Cancel}\n\nCancel back to the main menu."
};
DATA(0x004ff510) char *gSetupGameHelp[KB_SETUP_GAME_HELP_COUNT] = {
    "{Standard Game}\n\nA single player game playing out a single map.",
    "{Campaign Game}\n\nA single player game playing through a series of maps.",
    "{Multi-Player Game}\n\nA multi-player game, with several human players competing against each other on a single map.",
    "{Cancel}\n\nCancel back to the main menu."
};
DATA(0x004ff520) char *cBattleResults[KB_BATTLE_RESULT_TEXT_COUNT] = {
    "The enemy has surrendered!",
    "The enemy has fled!",
    "A glorious victory!",
    "\n\nFor valor in combat, %s receives %d experience",
    "%s surrenders to the enemy, and departs in shame.",
    "The cowardly %s flees from battle.",
    "Your forces suffer a bitter defeat, and %s abandons your cause.",
    "Your forces surrender to the enemy, and depart in shame.",
    "Your cowardly forces flee from battle.",
    "Your forces suffer a bitter defeat.",
    "\n\nFor valor in combat, %s receives %d experience, and gains %d level(s).",
    0
};
DATA(0x004ff550) char *cMoraleInfo[KB_MORALE_INFO_TEXT_COUNT] = {
    "{Good Morale}\n\nGood morale may give your armies extra attacks in combat.",
    "{Neutral Morale}\n\nNeutral morale means your armies will never be blessed with extra attacks or freeze in combat.",
    "{Bad Morale}\n\nBad morale may cause your armies to freeze in combat.",
    "%s\n\n\nCurrent Morale Modifiers:",
    "\nKnight bonus +1",
    "\nAll %s troops +1",
    "\nTroops of 3 alignments -1",
    "\nTroops of 4 alignments -2",
    "\nMedal of Valor +1",
    "\nMedal of Courage +1",
    "\nMedal of Honor +1",
    "\nMedal of Distinction +1",
    "\nFizbin of Misfortune -2",
    "\nBuoy visited +1",
    "\nOasis visited +1",
    "\nTemple visited +2",
    "\nGraveyard robber -1",
    "\nShipwreck robber -1",
    "\nBattle cowardice %d",
    "\nnone",
    "\nTroops of 5 alignments -3",
    "\nEntire unit is undead, so morale does not apply.",
    "\nSome undead in group -1",
    "\nWatering hole visited +1",
    "\nDerelict ship robber -1",
    "\nBarbarian Coliseum +2",
    "\nTavern +1",
    "\nBasic Leadership +1",
    "\nAdvanced Leadership +2",
    "\nExpert Leadership +3",
    "\nMasthead bonus at sea +1",
    "\nBattle Garb of Anduran gives you maximum morale."
};
DATA(0x004ff5d0) char *cMapSize[KB_MAP_SIZE_TEXT_COUNT] = {
    "Small",
    "Medium",
    "Large",
    "Huge"
};
DATA(0x004ff5e0) char *cDifficulty[KB_DIFFICULTY_TEXT_COUNT] = {
    "Easy",
    "Normal",
    "Hard",
    "Expert",
    "Impossible",
    0
};
DATA(0x004ff5f8) char *cStartDifficulty[KB_START_DIFFICULTY_TEXT_COUNT] = {
    "Easy",
    "Normal",
    "Hard",
    "Expert"
};
DATA(0x004ff608) char *cCampaignLeaders[KB_CAMPAIGN_LEADER_TEXT_COUNT] = {
    "Lord Ironfist",
    "Lord Slayer",
    "Queen Lamanda",
    "Lord Alamar"
};
DATA(0x004ff618) char *cWinText[KB_WIN_TEXT_COUNT] = {
    "Days Spent:",
    "Base Score:",
    "Difficulty Rating:",
    "Final Score:",
    "Ranking:",
    0
};
DATA(0x004ff630) char *cHumanDifficulty[KB_HUMAN_DIFFICULTY_TEXT_COUNT] = {
    "Human\n",
    "Human\nEasy",
    "Human\nNormal",
    "Human\nHard",
    "Human\nExpert",
    0
};
DATA(0x004ff648) char *cHumanInfoDifficulty[KB_HUMAN_INFO_DIFFICULTY_TEXT_COUNT] = {
    "Human-",
    "Human-Easy",
    "Human-Normal",
    "Human-Hard",
    "Human-Expert",
    0
};
DATA(0x004ff660) char *musicQualityText[KB_MUSIC_QUALITY_TEXT_COUNT] = {
    "MIDI",
    "CD Stereo w/o Opera",
    "CD Stereo with Opera",
    0
};
DATA(0x004ff670) char *gSpellDesc[KB_SPELL_TEXT_COUNT] = {
    "{Fireball}\n\nCauses a giant fireball to strike the selected area, damaging all nearby creatures.",
    "{Fireblast}\n\nAn improved version of fireball, fireblast affects two hexes around the center point of the spell, rather than one.",
    "{Lightning Bolt}\n\nCauses a bolt of electrical energy to strike the selected creature.",
    "{Chain Lightning}\n\nCauses a bolt of electrical energy to strike a selected creature, then strike the nearest creature with half damage, then strike the NEXT nearest creature with half again damage, and so on, until it becomes too weak to be harmful.  Warning:  This spell can hit your own creatures!",
    "{Teleport}\n\nTeleports the creature you select to any open position on the battlefield.",
    "{Cure}\n\nRemoves all negative spells cast upon one of your units, and restores up to 5 HP per level of spell power.",
    "{Mass Cure}\n\nRemoves all negative spells cast upon your forces, and restores up to 5 HP per level of spell power, per creature.",
    "{Resurrect}\n\nResurrects creatures from a damaged or dead unit until end of combat.",
    "{Resurrect True}\n\nResurrects creatures from a damaged or dead unit permanently.",
    "{Haste}\n\nIncreases the speed of any creature by two.",
    "{Mass Haste}\n\nIncreases the speed of all of your creatures by two.",
    "{Slow}\n\nSlows target to half movement rate.",
    "{Mass Slow}\n\nSlows all enemies to half movement rate.",
    "{Blind }\n\nClouds the affected creatures' eyes, preventing them from moving.",
    "{Bless}\n\nCauses the selected creatures to inflict maximum damage.",
    "{Mass Bless}\n\nCauses all of your units to inflict maximum damage.",
    "{Stoneskin}\n\nMagically increases the defense skill of the selected creatures.",
    "{Steelskin}\n\nIncreases the defense skill of the targeted creatures.  This is an improved version of Stoneskin.",
    "{Curse}\n\nCauses the selected creatures to inflict minimum damage.",
    "{Mass Curse}\n\nCauses all enemy troops to inflict minimum damage.",
    "{Holy Word}\n\nDamages all undead in the battle.",
    "{Holy Shout}\n\nDamages all undead in the battle.  This is an improved version of Holy Word.",
    "{Anti-Magic}\n\nPrevents harmful magic against the selected creatures.",
    "{Dispel Magic}\n\nRemoves all magic spells from a single target.",
    "{Mass Dispel}\n\nRemoves all magic spells from all creatures.",
    "{Magic Arrow}\n\nCauses a magic arrow to strike the selected target.",
    "{Berzerker}\n\nCauses a creature to attack its nearest neighbor.",
    "{Armageddon}\n\nHoly terror strikes the battlefield, causing severe damage to all creatures.",
    "{Elemental Storm}\n\nMagical elements pour down on the battlefield, damaging all creatures.",
    "{Meteor Shower}\n\nA rain of rocks strikes an area of the battlefield, damaging all nearby creatures.",
    "{Paralyze}\n\nThe targeted creatures are paralyzed, unable to move or retaliate.",
    "{Hypnotize}\n\nBrings a single enemy unit under your control for one combat round if its hits are less than 25 times the caster's spell power.",
    "{Cold Ray}\n\nDrains body heat from a single enemy unit.",
    "{Cold Ring}\n\nDrains body heat from all units surrounding the center point, but not including the center point.",
    "{Disrupting Ray}\n\nReduces the defense rating of an enemy unit by three.",
    "{Death Ripple}\n\nDamages all living (non-undead) units in the battle.",
    "{Death Wave}\n\nDamages all living (non-undead) units in the battle.  This spell is an improved version of Death Ripple.",
    "{Dragon Slayer}\n\nGreatly increases a unit's attack skill vs. Dragons.",
    "{Blood Lust}\n\nIncreases a unit's attack skill.",
    "{Animate Dead}\n\n'Resurrects' creatures from a damaged or dead undead unit permanently.",
    "{Mirror Image}\n\nCreates an illusionary unit that duplicates one of your existing units.  This illusionary unit does the same damages as the original, but will vanish if it takes any damage.",
    "{Shield}\n\nHalves damage received from ranged attacks for a single unit.",
    "{Mass Shield}\n\nHalves damage received from ranged attacks for all of your units.",
    "{Summon Earth Elemental}\n\nSummons Earth Elementals to fight for your army.",
    "{Summon Air Elemental}\n\nSummons Air Elementals to fight for your army.",
    "{Summon Fire Elemental}\n\nSummons Fire Elementals to fight for your army.",
    "{Summon Water Elemental}\n\nSummons Water Elementals to fight for your army.",
    "{Earthquake}\n\nDamages castle walls.",
    "{View Mines}\n\nCauses all mines across the land to become visible.",
    "{View Resources}\n\nCauses all resources across the land to become visible.",
    "{View Artifacts}\n\nCauses all artifacts across the land to become visible.",
    "{View Towns}\n\nCauses all towns and castles across the land to become visible.",
    "{View Heroes}\n\nCauses all Heroes across the land to become visible.",
    "{View All}\n\nCauses the entire land to become visible.",
    "{Identify Hero}\n\nAllows the caster to view detailed information on enemy Heroes.",
    "{Summon Boat}\n\nSummons the nearest unoccupied, friendly boat to an adjacent shore location.  A friendly boat is one which you just built or were the most recent player to occupy.",
    "{Dimension Door}\n\nAllows the caster to magically transport to a nearby location.",
    "{Town Gate}\n\nReturns the caster to any town or castle currently owned.",
    "{Town Portal}\n\nReturns the hero to the town or castle of choice, provided it is controlled by you.",
    "{Visions}\n\n Visions predicts the likely outcome of an encounter with a neutral army camp.",
    "{Haunt}\n\nHaunts a mine you control with Ghosts.  This mine stops producing resources.  (If I can't keep it, nobody will!)",
    "{Set Earth Guardian}\n\nSets Earth Elementals to guard a mine against enemy armies.",
    "{Set Air Guardian}\n\nSets Air Elementals to guard a mine against enemy armies.",
    "{Set Fire Guardian}\n\nSets Fire Elementals to guard a mine against enemy armies.",
    "{Set Water Guardian}\n\nSets Water Elementals to guard a mine against enemy armies.",
    0
};
DATA(0x004ff778) char *gSpellNames[KB_SPELL_TEXT_COUNT] = {
    "Fireball",
    "Fireblast",
    "Lightning Bolt",
    "Chain Lightning",
    "Teleport",
    "Cure",
    "Mass Cure",
    "Resurrect",
    "Resurrect True",
    "Haste",
    "Mass Haste",
    "Slow",
    "Mass Slow",
    "Blind ",
    "Bless",
    "Mass Bless",
    "Stoneskin",
    "Steelskin",
    "Curse",
    "Mass Curse",
    "Holy Word",
    "Holy Shout",
    "Anti-Magic",
    "Dispel Magic",
    "Mass Dispel",
    "Magic Arrow",
    "Berzerker",
    "Armageddon",
    "Elemental Storm",
    "Meteor Shower",
    "Paralyze",
    "Hypnotize",
    "Cold Ray",
    "Cold Ring",
    "Disrupting Ray",
    "Death Ripple",
    "Death Wave",
    "Dragon Slayer",
    "Blood Lust",
    "Animate Dead",
    "Mirror Image",
    "Shield",
    "Mass Shield",
    "Summon Earth Elemental",
    "Summon Air Elemental",
    "Summon Fire Elemental",
    "Summon Water Elemental",
    "Earthquake",
    "View Mines",
    "View Resources",
    "View Artifacts",
    "View Towns",
    "View Heroes",
    "View All",
    "Identify Hero",
    "Summon Boat",
    "Dimension Door",
    "Town Gate",
    "Town Portal",
    "Visions",
    "Haunt",
    "Set Earth Guardian",
    "Set Air Guardian",
    "Set Fire Guardian",
    "Set Water Guardian",
    0
};
DATA(0x004ff880) char *gSecondarySkillLevels[KB_SECONDARY_SKILL_LEVEL_TEXT_COUNT] = {
    "Basic",
    "Advanced",
    "Expert",
    0
};
DATA(0x004ff890) char *gSecondarySkills[KB_SECONDARY_SKILL_TEXT_COUNT] = {
    "Pathfinding",
    "Archery",
    "Logistics",
    "Scouting",
    "Diplomacy",
    "Navigation",
    "Leadership",
    "Wisdom",
    "Mysticism",
    "Luck",
    "Ballistics",
    "Eagle Eye",
    "Necromancy",
    "Estates"
};
DATA(0x004ff8c8) char *gNeutralBuildingNames[KB_NEUTRAL_BUILDING_TEXT_COUNT] = {
    "Mage Guild",
    "Thieves' Guild",
    "Tavern",
    "Shipyard",
    "Well",
    "Tent",
    "Castle",
    "Statue",
    "Left Turret",
    "Right Turret",
    "Marketplace",
    "",
    "Moat",
    "",
    "Dock with boat",
    "Captain's Quarters",
    "",
    "",
    "",
    0
};
DATA(0x004ff918) char *gWellExtraNames[KB_WELL_EXTRA_NAME_COUNT] = {
    "Farm",
    "Garbage Heap",
    "Crystal Garden",
    "Waterfall",
    "Orchard",
    "Skull Pile",
    "1st Lvl Growth",
    0
};
DATA(0x004ff938) char *gSpecialBuildingNames[KB_SPECIAL_BUILDING_NAME_COUNT] = {
    "Fortifications",
    "Coliseum",
    "Rainbow",
    "Dungeon",
    "Library",
    "Storm",
    "Special",
    0
};
DATA(0x004ff958) char *gDwellingNames[TOWN_TYPE_COUNT][KB_DWELLING_TYPE_COUNT] = {
    {
        "Thatched Hut",
        "Archery Range",
        "Blacksmith",
        "Armory",
        "Jousting Arena",
        "Cathedral",
        "Upg. Archery Range",
        "Upg. Blacksmith",
        "Upg. Armory",
        "Upg. Jousting Arena",
        "Upg. Cathedral",
        ""
    },
    {
        "Hut",
        "Stick Hut",
        "Den",
        "Adobe",
        "Bridge",
        "Pyramid",
        "Upg. Stick Hut",
        "",
        "Upg. Adobe",
        "Upg. Bridge",
        "",
        ""
    },
    {
        "Treehouse",
        "Cottage",
        "Archery Range",
        "Stonehenge",
        "Fenced Meadow",
        "Red Tower",
        "Upg. Cottage",
        "Upg. Archery Range",
        "Upg. Stonehenge",
        "",
        "",
        ""
    },
    {
        "Cave",
        "Crypt",
        "Nest",
        "Maze",
        "Swamp",
        "Green Tower",
        "",
        "",
        "Upg. Maze",
        "",
        "Red Tower",
        "Black Tower"
    },
    {
        "Habitat",
        "Pen",
        "Foundry",
        "Cliff Nest",
        "Ivory Tower",
        "Cloud Castle",
        "",
        "Upg. Foundry",
        "",
        "Upg. Ivory Tower",
        "Upg. Cloud Castle",
        ""
    },
    {
        "Excavation",
        "Graveyard",
        "Pyramid",
        "Mansion",
        "Mausoleum",
        "Laboratory",
        "Upg. Graveyard",
        "Upg. Pyramid",
        "Upg. Mansion",
        "Upg. Mausoleum",
        "",
        ""
    }
};
DATA(0x004ffa78) char *cSecSkillDesc[HERO_SKILL_COUNT][SECONDARY_SKILL_VALUE_LEVEL_COUNT] = {
    {
        "{Basic Pathfinding}\n\nBasic Pathfinding reduces the movement penalty for rough terrain by 25 percent.",
        "{Advanced Pathfinding}\n\nAdvanced Pathfinding reduces the movement penalty for rough terrain by 50 percent.",
        "{Expert Pathfinding}\n\nExpert Pathfinding eliminates the movement penalty for rough terrain."
    },
    {
        "{Basic Archery}\n\nBasic Archery increases the damage done by range attacking creatures by 10 percent.",
        "{Advanced Archery}\n\nAdvanced Archery increases the damage done by range attacking creatures by 25 percent.",
        "{Expert Archery}\n\nExpert Archery increases the damage done by range attacking creatures by 50 percent."
    },
    {
        "{Basic Logistics}\n\nBasic Logistics increases your hero's movement points by 10 percent.",
        "{Advanced Logistics}\n\nAdvanced Logistics increases your hero's movement points by 20 percent.",
        "{Expert Logistics}\n\nExpert Logistics increases your hero's movement points by 30 percent."
    },
    {
        "{Basic Scouting}\n\nBasic Scouting increases your hero's viewable area by 1 square.",
        "{Advanced Scouting}\n\nAdvanced Scouting increases your hero's viewable area by 2 squares.",
        "{Expert Scouting}\n\nExpert Scouting increases your hero's viewable area by 3 squares."
    },
    {
        "{Basic Diplomacy}\n\nBasic Diplomacy allows you to negotiate with monsters who are weaker than your group.  Approximately 1/4 of the creatures may offer to join you.",
        "{Advanced Diplomacy}\n\nAdvanced Diplomacy allows you to negotiate with monsters who are weaker than your group.  Approximately 1/2 of the creatures may offer to join you.",
        "{Expert Diplomacy}\n\nExpert Diplomacy allows you to negotiate with monsters who are weaker than your group.  All of the creatures may offer to join you."
    },
    {
        "{Basic Navigation}\n\nBasic Navigation increases your hero's movement points over water by 1/3.",
        "{Advanced Navigation}\n\nAdvanced Navigation increases your hero's movement points over water by 2/3.",
        "{Expert Navigation}\n\nExpert Navigation doubles your hero's movement points over water."
    },
    {
        "{Basic Leadership}\n\nBasic Leadership increases your hero's troops' morale by 1.",
        "{Advanced Leadership}\n\nAdvanced Leadership increases your hero's troops' morale by 2.",
        "{Expert Leadership}\n\nExpert Leadership increases your hero's troops' morale by 3."
    },
    {
        "{Basic Wisdom}\n\nBasic Wisdom allows your hero to learn third level spells.",
        "{Advanced Wisdom}\n\nAdvanced Wisdom allows your hero to learn fourth level spells.",
        "{Expert Wisdom}\n\nExpert Wisdom allows your hero to learn fifth level spells."
    },
    {
        "{Basic Mysticism}\n\nBasic Mysticism regenerates two of your hero's spell points per day.",
        "{Advanced Mysticism}\n\nAdvanced Mysticism regenerates three of your hero's spell points per day.",
        "{Expert Mysticism}\n\nExpert Mysticism regenerates four of your hero's spell points per day."
    },
    {
        "{Basic Luck}\n\nBasic Luck increases your hero's luck by 1.",
        "{Advanced Luck}\n\nAdvanced Luck increases your hero's luck by 2.",
        "{Expert Luck}\n\nExpert Luck increases your hero's luck by 3."
    },
    {
        "{Basic Ballistics}\n\nBasic Ballistics gives your hero's catapult shots a greater chance to hit and do damage to castle walls.",
        "{Advanced Ballistics}\n\nAdvanced Ballistics gives your hero's catapult an extra shot, and each shot has a greater chance to hit and do damage to castle walls.",
        "{Expert Ballistics}\n\nExpert Ballistics gives your hero's catapult an extra shot, and each shot automatically destroys any wall, except a fortified wall in a Knight town."
    },
    {
        "{Basic Eagle Eye}\n\nBasic Eagle Eye gives your hero a 20 percent chance to learn any given 1st or 2nd level enemy spell used against him in a combat.",
        "{Advanced Eagle Eye}\n\nAdvanced Eagle Eye gives your hero a 30 percent chance to learn any given 3rd level spell (or below) used against him in combat.",
        "{Expert Eagle Eye}\n\nExpert Eagle Eye gives your hero a 40 percent chance to learn any given 4th level spell (or below) used against him in combat."
    },
    {
        "{Basic Necromancy}\n\nBasic Necromancy allows 10 percent of the creatures killed in combat to be brought back from the dead as Skeletons.",
        "{Advanced Necromancy}\n\nAdvanced Necromancy allows 20 percent of the creatures killed in combat to be brought back from the dead as Skeletons.",
        "{Expert Necromancy}\n\nExpert Necromancy allows 30 percent of the creatures killed in combat to be brought back from the dead as Skeletons."
    },
    {
        "{Small Estates}\n\nYour hero produces 100 gold pieces per turn as tax revenue from estates.",
        "{Moderate Estates}\n\nYour hero produces 250 gold pieces per turn as tax revenue from estates.",
        "{Grand Estates}\n\nYour hero produces 500 gold pieces per turn as tax revenue from estates."
    }
};
DATA(0x004ffb20) char *cBuildingInfoNeutral[KB_NEUTRAL_BUILDING_INFO_COUNT] = {
    "The Mage Guild allows heroes to learn spells and replenish their spell points.",
    "The Thieves' Guild provides information on enemy players.  Thieves' Guilds can also provide scouting information on enemy towns.  Additional Guilds provide more information.",
    "The Tavern increases morale for troops defending the castle.",
    "The Shipyard allows ships to be built.",
    "The Well increases the growth rate of all dwellings by 2 creatures per week.",
    "The Tent provides workers to build a castle, provided the materials and the gold are available.",
    "The Castle improves town defense and increases income to 1000 gold per day.",
    "The Statue increases your town's income by 250 per day.",
    "The Left Turret provides extra firepower during castle combat.",
    "The Right Turret provides extra firepower during castle combat.",
    "The Marketplace can be used to convert one type of resource into another.  The more marketplaces you control, the better the exchange rate.",
    "",
    "The Moat slows attacking units.  Any unit entering the moat must end its turn there and becomes more vulnerable to attack.",
    "",
    "The Shipyard allows ships to be built.",
    "The Captain's Quarters provides a captain to assist in the castle's defense when no hero is present.",
    "",
    "",
    "",
    0
};
DATA(0x004ffb70) char *gBuildingInfoSpecial[KB_SPECIAL_BUILDING_INFO_COUNT] = {
    "The Fortifications increase the toughness of the walls, increasing the number of turns it takes to knock them down.",
    "The Coliseum provides inspiring spectacles to defending troops, raising their morale by two during combat.",
    "The Rainbow increases the luck of the defending units by two.",
    "The Dungeon increases the income of the town by 500 / day.",
    "The Library increases the number of spells in the Guild by one for each level of the guild.",
    "The Storm adds +2 to the power of spells of a defending spell caster."
};
DATA(0x004ffb88) char *cDirections[KB_DIRECTION_TEXT_COUNT] = {
    "northern",
    "northeastern",
    "eastern",
    "southeastern",
    "southern",
    "southwestern",
    "western",
    "northwestern",
    "central",
    0
};
DATA(0x004ffbb0) char *cRumourTerrainDescriptions[KB_RUMOUR_TERRAIN_DESCRIPTION_COUNT] = {
    "in the murky depths of the ocean.",
    "on a grassy plain.",
    "in the driven snow.",
    "in a murky swamp.",
    "buried in barren lava.",
    "under a desert drift.",
    "in the dirt.",
    "in a parched wasteland.",
    "buried on a beach.",
    0
};
DATA(0x004ffbd8) char *gInterfaceTypeText[KB_INTERFACE_TYPE_TEXT_COUNT] = {
    "Dynamic",
    "Good",
    "Evil",
    0
};
DATA(0x004ffbe8) char *cBWMouseText[KB_BW_MOUSE_TEXT_COUNT] = {
    "Black & White",
    "Color"
};
DATA(0x004ffbf0) char *combatSpeedText[KB_COMBAT_SPEED_TEXT_COUNT] = {
    "Normal",
    "Fast",
    "Very Fast",
    0
};
DATA(0x004ffc00) char *combatMiniInfoText[KB_COMBAT_MINI_INFO_TEXT_COUNT] = {
    "None",
    "Spells Only",
    "Full",
    0
};
DATA(0x004ffc10) char *gcCommandLineHelp[KB_COMMAND_LINE_HELP_COUNT] = {
    "\n\n\n***Command Line Help***\n",
    "\n",
    "/D0 - disable digital sound\n",
    "/M0 - disable MIDI music\n",
    "/R0 - disable Redbook music\n",
    "/I0 - skip the intro\n",
    "\n",
    "\n",
    "Example:\n",
    "\n",
    "HEROES2D /R0 /I0\n",
    "\n",
    "Starts the DOS version of Heroes2 with redbook\n",
    "sound disabled and the intro skipped.\n"
};
DATA(0x004ffc48) char *cOverviewText[KB_OVERVIEW_TEXT_COUNT] = {
    "Hero/Stats",
    "Skills",
    "Artifacts",
    "Town/Castle",
    "Garrison",
    "Available"
};
DATA(0x004ffc60) char *cWinComError[KB_WIN_COM_ERROR_TEXT_COUNT] = {
    "Communications error on function '%s'\n\nWin95 Error Code: %d\nWin95 Error Meaning: %s\n\n",
    "Suggested solutions:",
    "\n  1) Make sure all cables are firmly connected.",
    "\n  2) Reboot computer.",
    "\n  3) Check to make sure you have the correct COM port setting in 'CONFIG'. (The 3rd button down on the screen where you choose Host or Guest.)",
    "\n  4) Consider lowering the BAUD rate in 'CONFIG' to 19200 or 9600."
};
DATA(0x004ffc78) char *cMiniViewText[KB_MINI_VIEW_TEXT_COUNT] = {
    "%d Units",
    "%d Unit",
    "Attack",
    "Defense",
    "HP ",
    "Dmg",
    "Mrl",
    "Luk",
    "Shots",
    0
};
DATA(0x004ffca0) char *gFileRequestHelp[KB_FILE_REQUEST_HELP_COUNT] = {
    "{Small Maps}\n\nView only maps of size small (36 x 36).",
    "{Medium Maps}\n\nView only maps of size medium (72 x 72).",
    "{Large Maps}\n\nView only maps of size large (108 x 108).",
    "{Extra Large Maps}\n\nView only maps of size extra large (144 x 144).",
    "{All Maps}\n\nView all maps, regardless of size.",
    "{Enter Name}\n\nEnter the name of the file you wish to save.",
    "{OK}\n\nAccept the choice made.",
    "{Cancel}\n\nCancel without making a choice.",
    "{Size Icon}\n\nIndicates whether the map is small (36 x 36), medium (72 x 72), large (108 x 108), or extra large (144 x 144).",
    "{Players Icon}\n\nIndicates how many players total are in the scenario.  Any positions not occupied by humans will be occupied by computer players.",
    "{Victory Condition Icon}\n\nThere are 6 possibilities:\n\n{Tombstone} - Defeat all enemy heroes and towns.\n\n{Town} - Capture a specific town.\n\n{Hero Face} - Defeat a specific hero.\n\n{Medal} - Find a specific artifact.\n\n{Handshake} - Your side defeats the opposing side.\n\n{Coins} - Accumulate a large amount of gold.",
    "{Loss Condition Icon}\n\nThere are 4 possible loss conditions, as indicated by the following icons:\n\n{Tombstone} - Lose all your heroes and towns.\n\n{Town} - Lose a specific town.\n\n{Hero Face} - Lose a specific hero.\n\n{Hourglass} - Run out of time. (Fail to win by a certain point.)",
    "{Selected Name}\n\nThe name of the currently selected map.",
    "{Selected Description}\n\nThe description of the currently selected map.",
    "{Selected Map Difficulty}\n\nThe map difficulty of the currently selected map.  The map difficulty is determined by the scenario designer.  More difficult maps might include more or stronger enemies, fewer resources, or other special conditions making things tougher for the human player.",
    0
};
DATA(0x004ffce0) char *cPersonality[KB_PERSONALITY_TEXT_COUNT] = {
    "Warrior",
    "Builder",
    "Explorer",
    "Human"
};
DATA(0x004ffcf0) char *gArmySizeNames[KB_ARMY_SIZE_NAME_COUNT][KB_ARMY_SIZE_NAME_VARIANT_COUNT] = {
    {
        "Few",
        "A few",
        "a few"
    },
    {
        "Several",
        "Several",
        "several"
    },
    {
        "Pack",
        "A pack of",
        "a pack of"
    },
    {
        "Lots",
        "Lots of",
        "lots of"
    },
    {
        "Horde",
        "A Horde of",
        "a horde of"
    },
    {
        "Throng",
        "A Throng of",
        "a throng of"
    },
    {
        "Swarm",
        "A Swarm of",
        "a swarm of"
    },
    {
        "Zounds",
        "Zounds...",
        "zounds of"
    },
    {
        "Legion",
        "A Legion of",
        "a legion of"
    }
};
DATA(0x004ffd60) char *cRandomTavernText[KB_RANDOM_TAVERN_TEXT_COUNT] = {
    "The truth is out there.",
    "The dark side is stronger.",
    "The end of the world is near.",
    "The bones of Lord Slayer are buried in the foundation of the arena.",
    "He's innocent.",
    "A Black Dragon will take out a Titan any day of the week.",
    "He told her \"Yada yada yada\"...  and then she said \"Blah, blah, blah...\"",
    "There once was a man from Nantucket..."
};
DATA(0x004ffd80) char *cRandomSignText[KB_RANDOM_SIGN_TEXT_COUNT] = {
    "See Rock City",
    "This space for rent",
    "Next sign 50 miles",
    "Burma shave"
};
DATA(0x004ffd90) char *cCampaignAwards[KB_CAMPAIGN_AWARD_TEXT_COUNT] = {
    "Dwarven alliance",
    "Sorceress guild",
    "Roland strengthened",
    "Carry over forces",
    "Corlagon defeated",
    "Ultimate crown",
    "Necromancer guild",
    "DwarfBane",
    "Ogre alliance",
    "Dragon alliance",
    "Ultimate crown",
    "Carry over forces"
};
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
DATA(0x004ffe80) char *cOutOfMemory = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n%s\nHeroes II requires a minimum of\n%dK of Extended Memory (XMS) and\n480K of Conventional Memory\n\n";
DATA(0x004ffe88) char *cSlowVideoLevelText[KB_SLOW_VIDEO_LEVEL_TEXT_COUNT] = {
    "Normal",
    "Interlaced"
};
DATA(0x004ffe90) char *gSPanelHelp[KB_SETTINGS_PANEL_HELP_COUNT] = {
    "{OK}\n\nExit this menu.",
    "{Music}\n\nToggle ambient music level.\n\n(Note: When using CD Stereo music in DOS, the music may only be turned on or off - the level has no effect.)",
    "{Effects}\n\nToggle foreground sounds level.",
    "{Speed}\n\nChange the speed at which your heroes move on the main screen.",
    "{Music Type}\n\nChange the type of music.  MIDI music does not usually sound as good, but hurts performance less than CD Stereo.  If you use CD Stereo, you can select whether or not to have the operatic town themes play or not.\n\n(Note: Some older computers do not handle CD Stereo music well.  If you have a computer that leaves the music playing when you quit or crashes when you switch music tracks, try MIDI music.)",
    "{Show Path}\n\nToggle 'Show Path' on/off.  If 'Show Path' is on, your first click on a map location will show the path to get there, your second will start you moving. If this option is off, one click starts you moving immediately.",
    "{Enemy Speed}\n\nSets the speed that A.I. heroes move at.  You can also elect not to view A.I. movement at all.",
    "{Interface}\n\nSets what type of interface you want to use.  The default selection is a dynamic interface, which uses Evil graphics for the 3 evil heroes (Barbarian, Warlock, and Necromancer).",
    "{Video}\n\nDetermines if the video sequences play normally or in interlaced mode.  Interlaced mode runs better on slower machines, or machines with double-speed CD drives.",
    "{Mouse Cursor}\n\nToggle color cursors on/off.  Color cursors look nicer, but sometimes don't move as smoothly as black and white ones."
};
DATA(0x004ffeb8) char *xBarrierColor[KB_BARRIER_COLOR_NAME_COUNT] = {
    "aqua",
    "blue",
    "brown",
    "gold",
    "green",
    "orange",
    "purple",
    "red"
};
DATA(0x004ffed8) char *xGenericSiteNames[KB_GENERIC_SITE_NAME_COUNT] = {
    "Alchemist's Tower",
    "Arena",
    "Hut of the Magi",
    "Eye of the Magi",
    "Stables",
    "Mermaid",
    "Sirens",
    0
};
DATA(0x004ffef8) char *xRecruitmentSiteNames[KB_RECRUITMENT_SITE_NAME_COUNT] = {
    "Barrow Mounds",
    "Earth Summoning Altar",
    "Air Summoning Altar",
    "Fire Summoning Altar",
    "Water Summoning Altar",
    0
};
DATA(0x004fff10) SWinSetup gWinSetup[KB_WIN_SETUP_COUNT] = {
    {0, 100, "Build improvement:"},
    {1, 100, "Speed"},
    {1, 101, "Monster Info"},
    {1, 102, "Auto Combat\nSpell Casting"},
    {1, 103, "Grid"},
    {1, 104, "Shadow\nCursor"},
    {1, 105, "Shadow\nMovement"},
    {2, 100, "Music"},
    {2, 101, "Effects"},
    {2, 102, "Music Type"},
    {2, 103, "Speed"},
    {2, 104, "Show Path"},
    {2, 105, "Enemy Speed"},
    {2, 106, "Interface"},
    {2, 107, "Video"},
    {2, 108, "Mouse Cursor"},
    {6, 300, "Attack Skill"},
    {6, 301, "Defense Skill"},
    {6, 302, "Spell Power"},
    {6, 303, "Knowledge"},
    {7, 600, "Game Difficulty:"},
    {7, 57, "Easy"},
    {7, 58, "Normal"},
    {7, 59, "Hard"},
    {7, 60, "Expert"},
    {7, 61, "Impossible"},
    {7, 62, "Opponents:"},
    {7, 84, "Class:"},
    {9, 41, "Gold Per Day:"},
    {12, 0, "Build a new ship:"},
    {12, 1, "Resource cost:"},
    {14, 800, "1st"},
    {14, 801, "2nd"},
    {14, 802, "3rd"},
    {14, 803, "4th"},
    {14, 804, "5th"},
    {14, 805, "6th"},
    {14, 604, "Number of Towns:"},
    {14, 605, "Number of Castles:"},
    {14, 606, "Number of Heroes:"},
    {14, 607, "Gold in Treasury:"},
    {14, 608, "Wood & Ore:"},
    {14, 609, "Gems, Cr, Slf & Mer:"},
    {14, 610, "Obelisks Found:"},
    {14, 611, "Artifacts:"},
    {14, 612, "Total Army Strength:"},
    {14, 613, "Income:"},
    {14, 620, "Best Hero:"},
    {14, 621, "Best Hero Stats:"},
    {14, 622, "Personality:"},
    {14, 623, "Best Monster:"},
    {14, 0, "Thieves' Guild: Player Rankings"},
    {17, 110, "The above spells have been added to your book."},
    {18, 600, "Attack:"},
    {18, 601, "Defense:"},
    {18, 602, "Spell Power:"},
    {18, 603, "Knowledge:"},
    {18, 604, "Spell Points:"},
    {19, 600, "Defenders:"},
    {20, 600, "Recruit Hero"},
    {21, 600, "Attack Skill"},
    {21, 601, "Defense Skill"},
    {21, 602, "Spell Power"},
    {21, 603, "Knowledge"},
    {22, 0, "Tavern"},
    {23, 600, "Map\nDifficulty"},
    {23, 601, "Game\nDifficulty"},
    {23, 602, "\nRating"},
    {23, 603, "\nMap Size"},
    {23, 604, "Opponents"},
    {23, 605, "Class"},
    {23, 606, "Victory\nConditions"},
    {23, 607, "Loss\nConditions"}
};
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
// @data-layout-note Retail's initialized KB contribution is
// 0xf8c58..0x116f60 (0x1e308); candidate .data is 0x1e309. All 2,719 initialized
// candidate definitions close: 315 typed source DATA owners plus 2,404 reviewed
// compiler-private allocations. Every logical non-relocation byte agrees, and
// all 2,196 candidate DIR32 sites agree with retail in offset, target identity,
// addend, HIGHLOW presence, and value. The 36 uncovered ranges (132 bytes) are
// all retail zero fill; this includes the gArtifactNames-4 prebias target at
// 0xfe2ac, which is an alignment gap rather than an allocation. The residual 33
// private owners use 31 relocation/payload-proved remaining slots and one
// explicit two-member equivalence class for NormalDialog's identical "%d"
// literals. Five former string-backed short views are now typed owners at
// 0x1164bc, 0x1165e0, 0x116668, 0x11670c, and 0x116d20. Candidate/retail raw
// SHA-256 values are fa172a30c9d76e7541ceae2c4aac45b190bd4aacef277452cef3fed57e9a487c
// and 75bba73ed144117efade24fa386a3dfb14f2717d48ef2246817e900e687499b7.
// KB has no candidate or NB09 .rdata contribution. Do not invent padding,
// aliases, synthetic owners, or section pragmas for the physical stream delta.
DATA(0x005157a8) int iCDRomErr = 0;
DATA(0x005157ac) int bEarlySetupDone = 0;
DATA(0x005159f8) int bKBDone = 0;
DATA(0x005159fc) struct _REDBOOK *hRedbookz = 0;
DATA(0x00515a00) int bForceCheckTimeEvent = 0;
DATA(0x00515ca0) unsigned short
    IMHotSpots[KB_INIT_MENU_HOTSPOT_COUNT][INIT_MENU_HOTSPOT_FIELD_COUNT] = {
    {481, 185, 83, 96},
    {194, 179, 82, 79},
    {412, 105, 75, 76},
    {303, 137, 75, 44},
    {0, 389, 86, 90}
};
DATA(0x00515cc8) int lastIMHoverID = -1;
DATA(0x00515f78) int bInCheckEndGame = 0;
DATA(0x005165dc) int bInShutDown = 0;
DATA(0x00516810) int gbInMemError = 0;
DATA(0x00516d1c) int iShingleAnimFrame = 0;
// @data-layout-note Retail's loader-zero KB contribution is
// 0x128598..0x12a1d8 (0x1c40); candidate .bss is 0x1a7c. Its 131 public
// definitions have 125 inconsistent section bases because VC4.2 emits the
// candidate in identifier-hash order, not retail RVA order. All 7,970 retail
// HIGHLOW references into the contribution land inside recovered logical DATA
// extents; none targets unexplained storage. In particular, the unreferenced
// holes 0x12864c..0x1286df and 0x12978c..0x1298b3 must not become invented
// arrays. PollSound/ForcePollSound prove that 0x128d04, 0x128d08, and 0x128d0c
// are glTimers[5], glTimers[6], and glTimers[7], so the former glNext* globals
// were duplicate identities. Declaration reorder, extern-before-definition,
// and file/function-static probes do not recover retail's order. Revisit only
// with evidence for original common/section topology or referenced private
// storage; do not add padding definitions solely to close the 0x1c4 size delta.
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
