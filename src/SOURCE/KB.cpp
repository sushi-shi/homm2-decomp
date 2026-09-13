#include <va.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/town.h>
#include <SOURCE/townManager.h>
#include <SOURCE/ARMY.h>
#include <BASE/message.h>
#include <BASE/executive.h>
#include <BASE/mouseManager.h>
#include <SOURCE/game.h>
#include <SOURCE/GAME.h>
#include <BASE/Misc.h>
#include <BASE/MiscEnums.h>
#include <BASE/MiscGraphicsConstants.h>
#include <BASE/WINMGR.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/HERO.h>
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
#include <BASE/bitmap.h>
#include <BASE/sample.h>
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

H2_ENUM_BEGIN(CampaignChoiceValue)
    CHOICE_VALUE_NONE = -1
H2_ENUM_END(CampaignChoiceValue)

H2_ENUM_BEGIN(CampaignChoiceAmount)
    CHOICE_NO_AMOUNT = -1,
    CHOICE_BASIC_SKILL = 1,
    CHOICE_RESOURCE_BONUS = 20,
    CHOICE_GOLD_BONUS = 2000
H2_ENUM_END(CampaignChoiceAmount)

H2_ENUM_BEGIN(CheckEndGameConstants)
    END_GAME_NO_PLAYER = -1,
    END_GAME_GRACE_DAYS = 7,
    END_GAME_TEXT_BUFFER_SIZE = 100,
    END_GAME_GOLD_SCALE = 1000,
    END_GAME_ULTIMATE_ARTIFACT = 0,
    END_GAME_PLAYER_DIALOG_ICON = 9,
    END_GAME_REMOTE_DIALOG_TIME = 5000,
    END_GAME_CAMPAIGN_SAVE_NAME_SIZE = 20,
    END_GAME_SCENARIO_OFFSET = 1
H2_ENUM_END(CheckEndGameConstants)

H2_ENUM_BEGIN(CheckEndGameCampaignScenario)
    END_GAME_DWARF_SCENARIO = 3,
    END_GAME_SIDE_SCENARIO = 7,
    END_GAME_ROLAND_CAPTURE_SCENARIO = 9,
    END_GAME_FIRST_NO_SAVE_SCENARIO = 10,
    END_GAME_LAST_SCENARIO = 11
H2_ENUM_END(CheckEndGameCampaignScenario)

H2_ENUM_CLASS_BEGIN(MoraleInfoTextIndex)
    MORALE_INFO_GOOD = 0,
    MORALE_INFO_NEUTRAL = 1,
    MORALE_INFO_BAD = 2,
    MORALE_INFO_HEADER = 3,
    INFO_SAME_ALIGNMENT = 5,
    INFO_THREE_ALIGNMENTS = 6,
    INFO_FOUR_ALIGNMENTS = 7,
    INFO_MEDAL_OF_VALOR = 8,
    INFO_MEDAL_OF_COURAGE = 9,
    INFO_MEDAL_OF_HONOR = 10,
    INFO_MEDAL_OF_DISTINCTION = 11,
    INFO_FIZBIN = 12,
    INFO_BUOY = 13,
    INFO_OASIS = 14,
    INFO_TEMPLE = 15,
    INFO_GRAVEYARD = 16,
    INFO_SHIPWRECK = 17,
    MORALE_INFO_NONE = 19,
    INFO_FIVE_ALIGNMENTS = 20,
    INFO_ALL_UNDEAD = 21,
    INFO_SOME_UNDEAD = 22,
    INFO_WATERING_HOLE = 23,
    INFO_DERELICT_SHIP = 24,
    INFO_COLISEUM = 25,
    INFO_TAVERN = 26,
    INFO_BASIC_LEADERSHIP = 27,
    INFO_ADVANCED_LEADERSHIP = 28,
    INFO_EXPERT_LEADERSHIP = 29,
    MORALE_INFO_MASTHEAD = 30,
    MORALE_INFO_BATTLE_GARB = 31
H2_ENUM_CLASS_END(MoraleInfoTextIndex)

H2_ENUM_CLASS_BEGIN(LuckInfoTextIndex)
    LUCK_INFO_GOOD = 0,
    LUCK_INFO_NEUTRAL = 1,
    LUCK_INFO_BAD = 2,
    LUCK_INFO_HEADER = 3,
    INFO_RABBIT_FOOT = 4,
    INFO_HORSESHOE = 5,
    INFO_LUCKY_COIN = 6,
    INFO_CLOVER = 7,
    INFO_FAERIE_RING = 8,
    INFO_FOUNTAIN = 9,
    LUCK_INFO_NONE = 10,
    INFO_RAINBOW = 12,
    INFO_IDOL = 13,
    INFO_PYRAMID = 14,
    INFO_BASIC_SKILL = 15,
    INFO_ADVANCED_SKILL = 16,
    INFO_EXPERT_SKILL = 17,
    LUCK_INFO_MASTHEAD = 18,
    INFO_MERMAID = 19,
    LUCK_INFO_BATTLE_GARB = 20
H2_ENUM_CLASS_END(LuckInfoTextIndex)

H2_ENUM_BEGIN(MoraleLuckInfoConstant)
    MORALE_LUCK_DESCRIPTION_SIZE = 200
H2_ENUM_END(MoraleLuckInfoConstant)

H2_ENUM_BEGIN(NetBoxLocalConstant)
    BOX_WINDOW_Y = 0x19b,
    BOX_HEIGHT = 0x44,
    BOX_WIDTH = 0x27f,
    BOX_INPUT_Y = 0x1d1,
    BOX_INPUT_HEIGHT = 0xc,
    BOX_TEXT_LENGTH = 150,
    BOX_LINE_COUNT = 4,
    BOX_LINE_TEXT_LIMIT = 120,
    BOX_TEXT_X = 20,
    BOX_TEXT_Y = 54,
    BOX_MAX_INPUT = 0x5d,
    BOX_MAX_COLOR = 6,
    BOX_DEFAULT_COLOR = BOX_MAX_COLOR,
    BOX_COLOR_FRAME_OFFSET = 1,
    BOX_CURSOR_DELAY = 0x168,
    BOX_CURSOR_WIDTH_PADDING = 0x32,
    BOX_CURSOR_WIDTH_LIMIT = 0x25a,
    BOX_MESSAGE_TIMEOUT = 6000,
    BOX_EXIT_DELAY_STEPS = 20,
    BOX_EXIT_DELAY = 75,
    BOX_FIRST_LINE_ID = 1,
    BOX_FIRST_COLOR_ID = 0x14,
    BOX_INPUT_ID = 5,
    BOX_THIS_PLAYER_COLOR_ID = 0x18,
    BOX_KEY_ESCAPE = 0x1b,
    BOX_KEY_ENTER = 10,
    BOX_KEY_BACKSPACE = 0x7f,
    BOX_KEY_F1 = 0x3b00,
    BOX_CURSOR_GLYPH = 0x1f,
    BOX_PACKET_BUFFER_SIZE = 0x7f,
    BOX_FIRST_PRINTABLE = 0x20,
    BOX_LAST_PRINTABLE = 0x7f,
    BOX_REMOTE_MAP_CHANGE = 0x29,
    BOX_REMOTE_SETUP = 0x20,
    BOX_REMOTE_SAVE = 1,
    BOX_REMOTE_CHAT = 0xb
H2_ENUM_END(NetBoxLocalConstant)

H2_ENUM_BEGIN(PollSoundConstant)
    MOUSE_UPDATE_INTERVAL = 13,
    COMBAT_COLOR_CYCLE_INTERVAL = 110,
    DEFAULT_COLOR_CYCLE_INTERVAL = 200,
    NON_PALETTED_COLOR_CYCLE_DELAY = 300,
    SOUND_POLL_INTERVAL = 30,
    PALETTED_VIDEO_MODE_COLOR_DEPTH = 8
H2_ENUM_END(PollSoundConstant)

H2_ENUM_BEGIN(CongratsConstant)
    CONGRATS_TEXT_SIZE = 500,
    CONGRATS_RATING_LENGTH = 32,
    CONGRATS_SMACKER = 2,
    CONGRATS_DIFFICULTY_SCALE = 100
H2_ENUM_END(CongratsConstant)

H2_ENUM_BEGIN(CommandLineConstant)
    LINE_TCP_TEXT_LENGTH = 20,
    LINE_HELP_LINE_COUNT = 14,
    LINE_HUMAN_PLAYER_SLOTS = 4,
    LINE_TCP_MIN_PLAYERS = 2,
    LINE_TCP_MAX_PLAYERS = 6,
    LINE_FRAME_STEP = 6,
    LINE_TCP_TYPE_DEFAULT = 0,
    LINE_TCP_TYPE_L = 1,
    LINE_TCP_CLIENT = 0,
    LINE_TCP_HOST = 1,
    LINE_SINGLE_PLAYER = 1
H2_ENUM_END(CommandLineConstant)

H2_ENUM_BEGIN(InitMenuConstant)
    MENU_HOTSPOT_COUNT = 5,
    MENU_FIRST_COMMAND = 0x65,
    MENU_NEW_GAME = 0x65,
    MENU_LOAD_GAME = 0x66,
    MENU_HIGH_SCORES = 0x67,
    MENU_CREDITS = 0x68,
    MENU_EXIT = 0x69,
    MENU_LAST_ACTION = 0x6b,
    MENU_MOVIE = 0x6b,
    MENU_FIRST_WIDGET = 11,
    MENU_LAST_WIDGET = 15,
    MENU_WIDGET_OFFSET = 11,
    MENU_KEY_EXIT = 0x10,
    MENU_KEY_HIGH_SCORES = 0x23,
    MENU_KEY_LOAD = 0x26,
    MENU_KEY_CREDITS = 0x2e,
    MENU_KEY_NEW = 0x31,
    MENU_DISABLE_MASK = 0x200,
    MENU_CLOSE_COMMAND = 10,
    MENU_HELP_DIALOG = 4,
    MENU_MOVIE_SMACKER = 0x26,
    MENU_MAIN_MUSIC = 0x2a,
    MENU_SCREEN_WIDTH = 640,
    MENU_SCREEN_HEIGHT = 480,
    MENU_FRAME_STRIDE = 4,
    MENU_HOVER_FRAME = 3,
    MENU_IDLE_FRAME = 1,
    MENU_ACTIVE_FRAME = 2,
    MENU_WIDGET_FRAME_BASE = 44,
    MENU_REDRAW_LEFT = 0,
    MENU_REDRAW_TOP = 105,
    MENU_REDRAW_WIDTH = 565,
    MENU_REDRAW_HEIGHT = 375,
    MENU_HELP_NEW_GAME = 0,
    MENU_HELP_LOAD_GAME = 1,
    MENU_HELP_HIGH_SCORES = 2,
    MENU_HELP_CREDITS = 3,
    MENU_HELP_EXIT = 4
H2_ENUM_END(InitMenuConstant)

H2_ENUM_BEGIN(RecruitHeroConstant)
    RECRUIT_HERO_VIEW_BUTTON = 2,
    RECRUIT_HERO_FADE_STEPS = 8
H2_ENUM_END(RecruitHeroConstant)

H2_ENUM_BEGIN(BuildingRuleConstant)
    BUILDING_INFO_BUFFER_SIZE = 400,
    MONSTER_RARE_RESOURCE_COST = 1,
    MONSTER_UPGRADED_RARE_RESOURCE_COST = 2,
    NECROMANCER_CASTLE_UPGRADE_BASE_RESOURCE_VALUE = 1000
H2_ENUM_END(BuildingRuleConstant)

H2_ENUM_BEGIN(HighScoreInputConstant)
    HIGH_SCORE_INPUT_NAME_SIZE = 20,
    HIGH_SCORE_LAST_SHIFT_SOURCE = HIGH_SCORE_ENTRY_COUNT - 2
H2_ENUM_END(HighScoreInputConstant)

inline town* GetCastleRec(i32 i) {
    return &gpGame->m_castleRecs[i];
}

inline hero* GetHeroSlot(i32 i) {
    return &gpGame->m_heroRecs[i];
}

H2_ENUM_BEGIN(NormalDialogDrawRange)
    NORMAL_DIALOG_FOREGROUND_WIDGET_LIMIT = 0x9000,
    NORMAL_DIALOG_BACKGROUND_WIDGET_LAST_ID = -256
H2_ENUM_END(NormalDialogDrawRange)

VA(0x00465bf0, 0x115)
extern "C" void PollSound(void) {
    if (gbInPollSound)
        return;
    gbInPollSound = true;
    if (glTimers[GLOBAL_MOUSE_TIMER_SLOT] < KBTickCount() && !gbPutzingWithMouseCtr) {
        glTimers[GLOBAL_MOUSE_TIMER_SLOT] = KBTickCount() + MOUSE_UPDATE_INTERVAL;
        gpMouseManager->NewUpdate(0);
    }
    if (glTimers[GLOBAL_COLOR_CYCLE_TIMER_SLOT] < KBTickCount()) {
        if (giCycleType == WINDOW_COLOR_CYCLE_COMBAT
            || giCycleType == WINDOW_COLOR_CYCLE_COMBAT_ALTERNATE)
            glTimers[GLOBAL_COLOR_CYCLE_TIMER_SLOT] = KBTickCount() + COMBAT_COLOR_CYCLE_INTERVAL;
        else
            glTimers[GLOBAL_COLOR_CYCLE_TIMER_SLOT] = KBTickCount() + DEFAULT_COLOR_CYCLE_INTERVAL;
        bDoColorCycle = true;
        if (giGraphicsType == WINGRAPH_GRAPHICS_WING
            && giMainVideoModeColorDepth != PALETTED_VIDEO_MODE_COLOR_DEPTH) {
            glTimers[GLOBAL_COLOR_CYCLE_TIMER_SLOT] += NON_PALETTED_COLOR_CYCLE_DELAY;
            if (gbHeroMoving)
                bDoColorCycle = false;
        }
        if (bDoColorCycle)
            CycleColors(0);
    }
    if (glTimers[GLOBAL_POLL_SOUND_TIMER_SLOT] < KBTickCount()) {
        glTimers[GLOBAL_POLL_SOUND_TIMER_SLOT] = KBTickCount() + SOUND_POLL_INTERVAL;
        if (gbForegroundApp)
            gpSoundManager->PollSound();
        PollRemote();
    }
    gbInPollSound = false;
}

VA(0x00465d05, 0x17)
void ForcePollSound(void) {
    glTimers[GLOBAL_POLL_SOUND_TIMER_SLOT] = KBTickCount() - 1;
    PollSound();
}

VA(0x00465d1c, 0x594)
void InitMainClasses(void) {
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

VA(0x004662b0, 0x301)
void DeleteMainClasses(void) {
    if (gpBufferPalette)
        delete gpBufferPalette;
    gpBufferPalette = NULL;
    if (gpMonGroup)
        delete gpMonGroup;
    gpMonGroup = NULL;
    if (gpPhilAI)
        delete gpPhilAI;
    gpPhilAI = NULL;
    if (gpSearchArray)
        delete gpSearchArray;
    gpSearchArray = NULL;
    if (gpTownManager)
        delete gpTownManager;
    gpTownManager = NULL;
    if (gpCombatManager)
        delete gpCombatManager;
    gpCombatManager = NULL;
    if (gpAdvManager)
        delete gpAdvManager;
    gpAdvManager = NULL;
    if (gpGame)
        delete gpGame;
    gpGame = NULL;
    if (gpHighScoreManager)
        delete gpHighScoreManager;
    gpHighScoreManager = NULL;
    if (gpSoundManager)
        delete gpSoundManager;
    gpSoundManager = NULL;
    if (gpWindowManager)
        delete gpWindowManager;
    gpWindowManager = NULL;
    if (gpMouseManager)
        delete gpMouseManager;
    gpMouseManager = NULL;
    if (gpInputManager)
        delete gpInputManager;
    gpInputManager = NULL;
    if (gpExec)
        delete gpExec;
    gpExec = NULL;
    if (gpResourceManager)
        delete gpResourceManager;
    gpResourceManager = NULL;
}

VA(0x004665b1, 0x2e)
void EarlyShutdown(H2_CONST char* caption, H2_CONST char* text) {
    MessageBoxA(hwndApp, text, caption, MB_ICONHAND);
    exit(0);
}

VA(0x004665df, 0x12a)
void SetupCDRom(void) {
    if (iCDRomErr == CD_ROM_DRIVE_UNAVAILABLE) {
        soundManager* sound;
        SetPalette(gPalette->m_data, 1);
        gpMouseManager->ShowColorPointer();
        sound = gpSoundManager;
        sound->ShutdownSoundBackends();
        gSoundDisabled = true;
        if (giTCPHostStatus)
            NormalDialog(
                localization::Tr("system.cdrom.unavailable_guest_only"),
                NORMAL_DIALOG_INFO
            );
        gbNoCDRom = true;
    } else if (iCDRomErr == CD_ROM_EXPANSION_DISC_MISSING) {
        soundManager* sound;
        SetPalette(gPalette->m_data, 1);
        gpMouseManager->ShowColorPointer();
        sound = gpSoundManager;
        sound->ShutdownSoundBackends();
        gSoundDisabled = true;
        if (giTCPHostStatus)
            NormalDialog(
                localization::Tr("system.cdrom.expansion_disc_missing"),
                NORMAL_DIALOG_INFO
            );
        gbNoCDRom = true;
    }
    if (iCDRomErr == CD_ROM_GAME_DIRECTORY_MISSING) {
        EarlyShutdown(
            localization::Tr("system.startup_error.title"),
            localization::Tr("system.startup_error.game_directory_missing")
        );
        exit(0);
    }
    if (iCDRomErr == CD_ROM_DATA_FILES_MISSING) {
        EarlyShutdown(
            localization::Tr("system.startup_error.title"),
            localization::Tr("system.startup_error.data_files_missing")
        );
        exit(0);
    }
}

VA(0x00466709, 0x5e)
i32 EarlySetup(void) {
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

VA(0x00466767, 0x14d1)
i32 oldmain(void) {
    i32 command_c;
    b32 quit;
    b32 mainScreenLoaded_h;
    b32 firstMainScreen_h;
    i32 savedUpdateFlags_l;
    i32 player_h;
    i32 H2_UNUSED(unusedMainState_o);
    i32 H2_UNUSED(unusedMenuState);
    i32 H2_UNUSED(unusedPlayerState_f);
    i32 netPlayer_k;
    i32 gamePlayer_m;
    i32 result_i;
    i32 transmissionResult_d;
    char matchedNetPlayers_d[OLD_MAIN_MATCH_BUFFER_SIZE];
    char matchedGamePlayers_c[OLD_MAIN_MATCH_BUFFER_SIZE];
    OldMainNetBuffer netBuffer_f;

    if (bKBDone)
        return 0;
    bKBDone = true;
    LogStr("OM1");
    LogStr("OM2");
    command_c = -1;
    if (gpExec->InitSystem())
        ShutDown(localization::Tr("system.initialization_failed"));
    LogStr("OM3");
    KBChangeMenu(hmnuDflt);
    gPalette = gpResourceManager->GetPalette("kb.pal");
    gpWindowManager->m_updateFlags = 1;
    smallFont = gpResourceManager->GetFont("smalfont.fnt");
    bigFont = gpResourceManager->GetFont("bigfont.fnt");
    gpMouseManager->SetPointer(
        "advmice.mse",
        0,
        MOUSE_AUTO_CURSOR_TYPE
    );
    gpMouseManager->SetColorMice(CURRENT_GRAPHICS_CONFIG.colorMouseCursor);
    LogStr("OM4");
    SetupCDRom();
    LogStr("OM5");
    if (gpSoundManager->Open(-1))
        ShutDown(localization::Tr("system.sound.initialization_failed"));
    if (giDebugLevel < OLD_MAIN_DEBUG_MEMORY_CHECK_LEVEL)
        CheckMem();
    LogStr("OM6");

    if (giShowIntro) {
        FillBitmapArea(
            gpWindowManager->m_screen,
            0,
            0,
            OLD_MAIN_SCREEN_WIDTH,
            OLD_MAIN_SCREEN_HEIGHT,
            0
        );
        BlitBitmapToScreen(
            gpWindowManager->m_screen,
            0,
            0,
            OLD_MAIN_SCREEN_WIDTH,
            OLD_MAIN_SCREEN_HEIGHT,
            0,
            0
        );
        if (!gbSkipIntro) {
            savedUpdateFlags_l = gpWindowManager->m_updateFlags;
            gpWindowManager->m_updateFlags = 0;
            if (PlaySmacker(OLD_MAIN_INTRO_PUBLISHER_VIDEO)
                && PlaySmacker(OLD_MAIN_INTRO_PRIMARY_VIDEO)
                && PlaySmacker(OLD_MAIN_INTRO_FALLBACK_VIDEO))
                PlaySmacker(OLD_MAIN_INTRO_SECONDARY_VIDEO);
            gpWindowManager->m_updateFlags = savedUpdateFlags_l;
        }
    }

    LoadSystemwideIcons();
    memset(gbThisNetHumanPlayer, 0, OLD_MAIN_PLAYER_COUNT);
    gpMouseManager->ShowColorPointer();
    quit = false;
    mainScreenLoaded_h = false;
    firstMainScreen_h = true;

    while (!quit) {
    main_menu:
        if (!gShingleAnim)
            gShingleAnim = gpResourceManager->GetIcon(
                "shnganim.icn"
            );
        if (gGameCommand != OLD_MAIN_EXIT)
            gpSoundManager->SwitchAmbientMusic(OLD_MAIN_MAIN_MUSIC);

        if (!mainScreenLoaded_h) {
            if (gGameCommand != OLD_MAIN_EXIT) {
                gpResourceManager->GetBackdrop(
                    "heroes.icn",
                    gpWindowManager->m_screen,
                    1
                );
                gpWindowManager
                    ->UpdateScreenRegion(0, 0, OLD_MAIN_SCREEN_WIDTH, OLD_MAIN_SCREEN_HEIGHT);
                if (firstMainScreen_h)
                    SetPalette(gPalette->m_data, 1);
                else
                    gpWindowManager->FadeScreen(FADE_IN, OLD_MAIN_FADE_SPEED, gPalette);
                firstMainScreen_h = false;
            }
            gpMouseManager->SetPointer(
                "advmice.mse",
                0,
                MOUSE_AUTO_CURSOR_TYPE
            );
        }
        mainScreenLoaded_h = true;
        if (gGameCommand != OLD_MAIN_EXIT)
            gpWindowManager->m_updateFlags = 1;

        if (giTCPHostStatus != -1 && gbTCPFirstTime) {
            gbTCPFirstTime = false;
            giNumHumanPlayers = 1;
            iMPBaseType = MULTIPLAYER_BASE_NETWORK;
            iMPNetProtocol = OLD_MAIN_NETWORK_PROTOCOL;
            if (giTCPHostStatus)
                iMPExtendedType = REMOTE_GAME_NETWORK_HOST;
            else
                iMPExtendedType = REMOTE_GAME_NETWORK_GUEST;
            giSetupGameType = static_cast<u8>(giTCPType);
            RemoteMain(iMPExtendedType);
            if (iMPExtendedType == REMOTE_GAME_NETWORK_GUEST)
                gbWaitForRemoteReceive = true;
            else
                gbWaitForRemoteReceive = false;
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

            if (giMenuCommand != -1) {
            process_menu_command:
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
                command_c = gGameCommand;
                gGameCommand = -1;
            } else {
                gpInitWin = new heroWindow(
                    0,
                    0,
                    "stpmain.bin"
                );
                if (!gpInitWin)
                    MemError();
                gbInSetupDialog = true;
                gpWindowManager->DoDialog(gpInitWin, InitMenuHandler, 0);
                delete gpInitWin;
                gpInitWin = NULL;
                command_c = gpWindowManager->m_dialogResult;
                gbInSetupDialog = false;
            }
        }
        if (giMenuCommand != -1)
            goto process_menu_command;

        switch (command_c) {
            case OLD_MAIN_LOAD_GAME:
                giSetupGameType = OLD_MAIN_SETUP_LOAD;
                goto setup_selected;
            case OLD_MAIN_NEW_GAME:
                giSetupGameType = OLD_MAIN_SETUP_NEW;

            setup_selected:
                for (player_h = 0; player_h < OLD_MAIN_PLAYER_COUNT; player_h++)
                    sprintf(
                        cPlayerNames[player_h],
                        ""
                    );
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
                                gpWindowManager
                                    ->FadeScreen(FADE_OUT, OLD_MAIN_FADE_SPEED, gPalette);
                                mainScreenLoaded_h = false;
                                goto main_menu;
                            }
                        } else {
                            if (xIsPlayingExpansionCampaign) {
                                if (xCampaign.HandleVictory()) {
                                    xCampaign.InitMap();
                                    goto initialize_game;
                                } else {
                                    gpWindowManager
                                        ->FadeScreen(FADE_OUT, OLD_MAIN_FADE_SPEED, gPalette);
                                    mainScreenLoaded_h = false;
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
                    ShutDown(localization::Tr("system.manager.add_failed"));
                gpExec->MainLoop();
                gpExec->RemoveManager(gpHighScoreManager);
                mainScreenLoaded_h = false;
                goto main_menu;
            case OLD_MAIN_CREDITS:
                gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_FADE_SPEED, gPalette);
                PlaySmacker(OLD_MAIN_CREDITS_FIRST_VIDEO);
                PlaySmacker(OLD_MAIN_CREDITS_SECOND_VIDEO);
                PlaySmacker(OLD_MAIN_CREDITS_THIRD_VIDEO);
                mainScreenLoaded_h = false;
                gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_LONG_FADE_SPEED, gPalette);
                goto main_menu;
            case OLD_MAIN_EXIT:
                quit = true;
                break;
        }

    game_setup_complete:
        if (giMenuCommand != -1)
            goto process_menu_command;
        LogStr("DWM 1");
        if (!quit) {
            LogStr("DWM 2");
            if (gbRemoteOn && giThisNetPos == 0) {
                LogStr("DWM 3");
                memset(matchedGamePlayers_c, 0, OLD_MAIN_PLAYER_COUNT);
                memset(matchedNetPlayers_d, 0, OLD_MAIN_PLAYER_COUNT);
                for (netPlayer_k = 0; netPlayer_k < OLD_MAIN_PLAYER_COUNT; netPlayer_k++) {
                    if (!gbHumanPlayer[netPlayer_k])
                        continue;
                    for (gamePlayer_m = 0; gamePlayer_m < OLD_MAIN_PLAYER_COUNT; gamePlayer_m++) {
                        if (strlen(&gpGame->m_defaultPlayerNames
                                        [gamePlayer_m * OLD_MAIN_DEFAULT_NAME_STRIDE])
                                == OLD_MAIN_DEFAULT_NAME_LENGTH
                            && !strcmp(
                                &gpGame->m_defaultPlayerNames
                                     [gamePlayer_m * OLD_MAIN_DEFAULT_NAME_STRIDE],
                                gsNetPlayerInfo[netPlayer_k].uniqueSystemID
                            )
                            && !gpGame->m_playerDead[gamePlayer_m]
                            && !matchedGamePlayers_c[gamePlayer_m]
                            && !matchedNetPlayers_d[netPlayer_k]) {
                            matchedGamePlayers_c[gamePlayer_m] = 1;
                            matchedNetPlayers_d[netPlayer_k] = 1;
                            gbGamePosToNetPos[gamePlayer_m] = static_cast<i8>(netPlayer_k);
                        }
                    }
                }
                gamePlayer_m = 0;
                while (gamePlayer_m < OLD_MAIN_PLAYER_COUNT && matchedGamePlayers_c[gamePlayer_m])
                    gamePlayer_m++;
                for (netPlayer_k = 0; netPlayer_k < OLD_MAIN_PLAYER_COUNT; netPlayer_k++) {
                    if (matchedNetPlayers_d[netPlayer_k])
                        continue;
                    if (gbHumanPlayer[netPlayer_k]) {
                        gbGamePosToNetPos[netPlayer_k] = static_cast<i8>(gamePlayer_m);
                        strcpy(
                            &gpGame->m_defaultPlayerNames
                                 [gamePlayer_m * OLD_MAIN_DEFAULT_NAME_STRIDE],
                            gsNetPlayerInfo[netPlayer_k].uniqueSystemID
                        );
                        gamePlayer_m++;
                        while (gamePlayer_m < OLD_MAIN_PLAYER_COUNT
                               && matchedGamePlayers_c[gamePlayer_m])
                            gamePlayer_m++;
                    } else {
                        gbGamePosToNetPos[netPlayer_k] = -1;
                    }
                }

                memcpy(netBuffer_f.setup.gamePosToNetPos, gbGamePosToNetPos, OLD_MAIN_PLAYER_COUNT);
                memcpy(
                    netBuffer_f.setup.players,
                    gsNetPlayerInfo,
                    sizeof(netBuffer_f.setup.players)
                );
                giThisGamePos = NetPosToGamePos(0);
                gbUseRegularCompression = gbUseDiffCompression = true;
                if (giHighMemBuffer < OLD_MAIN_REGULAR_COMPRESSION_MEMORY_LIMIT)
                    gbUseRegularCompression = false;
                for (player_h = 0; player_h < giNumHumanPlayers; player_h++) {
                    if (!gsNetPlayerInfo[player_h].useRegularCompression)
                        gbUseRegularCompression = false;
                    if (!gsNetPlayerInfo[player_h].useDiffCompression)
                        gbUseDiffCompression = false;
                }
                netBuffer_f.setup.useRegularCompression = gbUseRegularCompression;
                netBuffer_f.setup.useDiffCompression = gbUseDiffCompression;
                for (player_h = 1; player_h < giNumHumanPlayers; player_h++) {
                    transmissionResult_d = TransmitRemoteData(
                        netBuffer_f.bytes,
                        player_h,
                        sizeof(OldMainNetSetup),
                        OLD_MAIN_NETWORK_PACKET,
                        1
                    );
                    if (!transmissionResult_d)
                        ShutDown(NULL);
                }
                for (player_h = 1; player_h < giNumHumanPlayers; player_h++) {
                    if (!gpGame->TransmitSaveGame(player_h, 0, 1))
                        ShutDown(NULL);
                }
                memset(gbThisNetHumanPlayer, 0, OLD_MAIN_PLAYER_COUNT);
                gbThisNetHumanPlayer[giThisGamePos] = true;
                iLastDiffSendTo = -1;
                gpGame->SaveGame(gConfig.rmtRLName, 0, 0);
            }
            LogStr("DWM 4");
            if (gbRemoteOn && gbWaitForRemoteReceive) {
                LogStr("DWM 5");
                giWaitType = DIALOG_WAIT_OTHER_PLAYER;
                NormalDialog(
                    localization::Tr("network.data.waiting_to_receive"),
                    OLD_MAIN_DIALOG_WAIT
                );
                if (!gbFunctionComplete)
                    ShutDown(NULL);
                gpGame->LoadGame(gConfig.rmtRCName, 0, 1);
                sprintf(
                    gpGame->m_saveName,
                    localization::Tr("save.filename.new_game")
                );
                iLastDiffSendTo = -1;
                gpGame->SaveGame(gConfig.rmtSLName, 0, 0);
            }

        initialize_game:
            gpWindowManager->m_updateFlags = 1;
            if (gShingleAnim)
                gpResourceManager->Dispose(gShingleAnim);
            gShingleAnim = NULL;

            if (giNumHumanPlayers > 1) {
                for (player_h = 0; player_h < giNumHumanPlayers; player_h++) {
                    if (iMPBaseType != MULTIPLAYER_BASE_HOT_SEAT)
                        strcpy(
                            cPlayerNames[NetPosToGamePos(player_h)],
                            gsNetPlayerInfo[player_h].name
                        );
                }
            }
            for (player_h = 0; player_h < gpGame->m_playerCount; player_h++) {
                if (!strlen(cPlayerNames[player_h])) {
                    sprintf(
                        cPlayerNames[player_h],
                        localization::Tr("player.color_default_name"),
                        gColors[gpGame->m_players[player_h].m_color]
                    );
                    cPlayerNames[player_h][0] = CyrillicToUpper(cPlayerNames[player_h][0]);
                }
            }
            ComputeAdvNetControl();
            gbGameInitialized = true;
            mainScreenLoaded_h = false;
            gpSoundManager->StopAllSamples(1);
            gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_FADE_SPEED, NULL);
            gMapX = 0;
            gMapY = 0;
            giCurWatchPlayer = giCurPlayer;
            while (!gbThisNetHumanPlayer[giCurWatchPlayer])
                giCurWatchPlayer = (giCurWatchPlayer + 1) % gpGame->m_playerCount;
            giCurWatchPlayerBit = static_cast<u8>(1 << giCurWatchPlayer);

            if (gbInCampaign && gpGame->m_campaignScenarioWon) {
                giEndSequence = true;
                goto game_over;
            } else if (xIsPlayingExpansionCampaign && xCampaign.IsThisMapCompleted()) {
                giEndSequence = true;
                goto game_over;
            } else {
                if (gpExec->AddManager(gpAdvManager, -1))
                    ShutDown(localization::Tr("system.manager.add_failed"));
                if (command_c == OLD_MAIN_NEW_GAME) {
                    gpAdvManager->SetHeroContext(gpGame->m_players[0].NextHero(0), 0);
                }
                if (command_c == OLD_MAIN_NEW_GAME || bForceCheckTimeEvent) {
                    bForceCheckTimeEvent = false;
                    gpGame->CheckForTimeEvent();
                }
                gpExec->MainLoop();
                gMapX = gpAdvManager->m_mapOriginX;
                gMapY = gpAdvManager->m_mapOriginY;
                gpExec->RemoveManager(gpAdvManager);
                gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_FADE_SPEED, gPalette);
            }
        }

        if (gbGameOver) {
        game_over:
            RemoteCleanup();
            bShowIt = true;
            gpMouseManager->SetPointer(
                "advmice.mse",
                0,
                MOUSE_AUTO_CURSOR_TYPE
            );
            sprintf(
                gcWinText,
                localization::Tr("campaign.victory.complete"),
                giCurTurn
            );

            if (giEndSequence != 1) {
                gpWindowManager->m_updateFlags = 0;
                if (xIsExpansionMap)
                    PlaySmacker(OLD_MAIN_EXPANSION_VICTORY_VIDEO);
                else
                    PlaySmacker(OLD_MAIN_STANDARD_VICTORY_VIDEO);
                gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_LONG_FADE_SPEED, gPalette);
                gpResourceManager->GetBackdrop(
                    "heroes.icn",
                    gpWindowManager->m_screen,
                    1
                );
                gpWindowManager
                    ->UpdateScreenRegion(0, 0, OLD_MAIN_SCREEN_WIDTH, OLD_MAIN_SCREEN_HEIGHT);
                gpWindowManager->FadeScreen(FADE_IN, OLD_MAIN_FADE_SPEED, gPalette);
                gpWindowManager->m_updateFlags = 1;
                mainScreenLoaded_h = true;
                gpSoundManager->PlayAmbientMusic(OLD_MAIN_MAIN_MUSIC);
            } else {
                i32 campaignResult = 0;
                if (gbInCampaign) {
                    campaignResult = gpGame->HandleCampaignWin();
                    if ((gpGame->m_campaignScenario + 1 == OLD_MAIN_ARCHIBALD_FINAL_SCENARIO_NUMBER
                         && gpGame->m_campaignScenarioCompleted[IDX(gpGame->m_campaignType)]
                                                               [OLD_MAIN_ARCHIBALD_FINAL_SCENARIO])
                        || (gpGame->m_campaignScenario + 1 == OLD_MAIN_ROLAND_FINAL_SCENARIO_NUMBER
                            && gpGame->m_campaignType == CAMPAIGN_ROLAND
                            && gpGame
                                   ->m_campaignScenarioCompleted[IDX(gpGame->m_campaignType)]
                                                                [OLD_MAIN_ROLAND_FINAL_SCENARIO])) {
                        gbShowHighScore = true;
                        ShowCongrats(HIGH_SCORE_CAMPAIGN);
                        AddScoreToHighScore(
                            gpGame->m_campaignScore,
                            gpGame->m_campaignScore,
                            0,
                            HIGH_SCORE_CAMPAIGN,
                            const_cast<char*>(gpGame->m_campaignType == CAMPAIGN_ARCHIBALD
                                ? localization::Tr("campaign.ruler.archibald")
                                : localization::Tr("campaign.ruler.roland"))
                        );
                    }
                    if (campaignResult) {
                        for (player_h = 0; player_h < OLD_MAIN_PLAYER_COUNT; player_h++)
                            sprintf(
                                cPlayerNames[player_h],
                                ""
                            );
                        gpGame->InitCampaignMap();
                        gbGameOver = false;
                        bForceCheckTimeEvent = true;
                        goto initialize_game;
                    }
                } else if (xIsPlayingExpansionCampaign) {
                    campaignResult = xCampaign.HandleVictory();
                    if (xCampaign.IsCompleted()) {
                        gbShowHighScore = true;
                        ShowCongrats(HIGH_SCORE_EXPANSION_CAMPAIGN);
                        AddScoreToHighScore(
                            xCampaign.Days(),
                            xCampaign.Days(),
                            0,
                            HIGH_SCORE_EXPANSION_CAMPAIGN,
                            xHSCampaignNames[xCampaign.CampaignID()]
                        );
                    }
                    if (campaignResult) {
                        for (player_h = 0; player_h < OLD_MAIN_PLAYER_COUNT; player_h++)
                            sprintf(
                                cPlayerNames[player_h],
                                ""
                            );
                        xCampaign.InitMap();
                        gbGameOver = false;
                        bForceCheckTimeEvent = true;
                        goto initialize_game;
                    }
                } else {
                    ShowCongrats(HIGH_SCORE_STANDARD);
                    if (!gbShowHighScore) {
                        gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_FADE_SPEED, gPalette);
                        gpResourceManager->GetBackdrop(
                            "heroes.icn",
                            gpWindowManager->m_screen,
                            1
                        );
                        gpWindowManager->UpdateScreenRegion(
                            0,
                            0,
                            OLD_MAIN_SCREEN_WIDTH,
                            OLD_MAIN_SCREEN_HEIGHT
                        );
                        gpWindowManager->FadeScreen(FADE_IN, OLD_MAIN_FADE_SPEED, gPalette);
                        gpWindowManager->m_updateFlags = 1;
                        mainScreenLoaded_h = true;
                        gpSoundManager->PlayAmbientMusic(OLD_MAIN_MAIN_MUSIC);
                    } else {
                        gpSoundManager->PlayAmbientMusic(OLD_MAIN_HIGH_SCORE_MUSIC);
                    }
                }
            }

            gbGameOver = false;
            if (gbShowHighScore) {
                gbShowHighScore = false;
                if (gpExec->AddManager(gpHighScoreManager, -1))
                    ShutDown(localization::Tr("system.manager.add_failed"));
                gpExec->MainLoop();
                gpExec->RemoveManager(gpHighScoreManager);
                giHighScoreRank = -1;
                gpSoundManager->SwitchAmbientMusic(OLD_MAIN_MAIN_MUSIC);
                gpResourceManager->GetBackdrop(
                    "heroes.icn",
                    gpWindowManager->m_screen,
                    1
                );
                gpWindowManager
                    ->UpdateScreenRegion(0, 0, OLD_MAIN_SCREEN_WIDTH, OLD_MAIN_SCREEN_HEIGHT);
                gpWindowManager->FadeScreen(FADE_IN, OLD_MAIN_FADE_SPEED, gPalette);
                mainScreenLoaded_h = true;
            }
        }

        if (gbRemoteOn)
            quit = true;
    }

    ShutDown(NULL);
    return 0;
}

VA(0x00467c38, 0x70)
char toupper(char c) {
    if (static_cast<u8>(c) >= 'a' && static_cast<u8>(c) <= 'z')
        return c - ' ';
    if (static_cast<u8>(c) >= 0xE0 && static_cast<u8>(c) <= 0xFF)
        return c - ' ';
    if (static_cast<u8>(c) == 0xB8)
        return static_cast<char>(0xA8);
    return c;
}

#if !H2_STRICT_ENUMS
#define destinationIndex dstIndex
#define sourceIndex srcIndex
#endif
VA(0x00467ca8, 0x5f3)
i32 InterpretCommandLine(void) {
    i32 size;
    i32 i;
    b32 helpRequested;
    gbTCPFirstTime = true;
    giTCPType = -1;
    giTCPHostStatus = -1;
    giTCPNumPlayers = -1;
    strcpy(gcTCPAddress, "");
    strcpy(gcTCPName, "");
    gbUseWaveout = false;
    giDebugLevel = 0;
    giShowIntro = 1;
    gbCheatMenus = false;
    giScreenScroll = true;
    giLimitPlayer = 0;
    gbBlackoutPlayer = true;
    helpRequested = false;
    strcpy(gMapName, "Chaos.mp2");
    strcpy(
        gFullMapName,
        localization::Tr("map.default.chaos")
    );

    size = strlen(gcCommandLine);
    for (i = 0; i < size; i++) {
        if (gcCommandLine[i] == ' ' && i + 1 < size
            && (gcCommandLine[i + 1] == '?' || gcCommandLine[i + 1] == 'h'
                || gcCommandLine[i + 1] == 'H')) {
            helpRequested = true;
        }
        if (gcCommandLine[i] == '/' && i + 1 < size) {
            switch (toupper(gcCommandLine[i + 1])) {
                case 'Z':
                    gbDoMemCheck = false;
                    break;
                case 'W':
                    gbUseWaveout = true;
                    if (i + 2 < size)
                        gbUseWaveout = gcCommandLine[i + 2] - '0';
                    break;
                case 'V':
                    gConfig.slowVideo = 1;
                    WritePrefs();
                    break;
                case 'N':
                    if (i + 3 < size && toupper(gcCommandLine[i + 2]) == 'W'
                        && toupper(gcCommandLine[i + 3]) == 'C') {
                        gbCheatMenus = true;
                    }
                    break;
                case 'I':
                    if (i + 2 < size)
                        giShowIntro = gcCommandLine[i + 2] - '0';
                    break;
                case 'P':
                    if (i + 2 < size)
                        giDebugLevel = gcCommandLine[i + 2] - '0';
                    break;
                case 'T':
                    if (i + 2 < size) {
                        switch (toupper(gcCommandLine[i + 2])) {
                            case 'T':
                                if (i + 3 < size && toupper(gcCommandLine[i + 3]) == 'L') {
                                    giTCPType = LINE_TCP_TYPE_L;
                                } else {
                                    giTCPType = LINE_TCP_TYPE_DEFAULT;
                                }
                                break;
                            case 'S':
                                if (i + 3 < size && toupper(gcCommandLine[i + 3]) == 'H') {
                                    giTCPHostStatus = LINE_TCP_HOST;
                                } else {
                                    giTCPHostStatus = LINE_TCP_CLIENT;
                                }
                                break;
                            case 'P': {
                                i32 count = 0;
                                if (i + 3 < size)
                                    count = gcCommandLine[i + 3] - '0';
                                if (count >= LINE_TCP_MIN_PLAYERS
                                    && count <= LINE_TCP_MAX_PLAYERS) {
                                    giTCPNumPlayers = count;
                                }
                                break;
                            }
                            case 'A': {
                                if (i + 3 < size) {
                                    i32 destinationIndex = 0;
                                    i32 sourceIndex = i + 3;
                                    while (destinationIndex < LINE_TCP_TEXT_LENGTH
                                           && gcCommandLine[sourceIndex]
                                           && gcCommandLine[sourceIndex] != ' ') {
                                        gcTCPAddress[destinationIndex] = gcCommandLine[sourceIndex];
                                        sourceIndex++;
                                        destinationIndex++;
                                    }
                                    gcTCPAddress[destinationIndex] = 0;
                                }
                                break;
                            }
                            case 'N': {
                                if (i + 3 < size) {
                                    i32 destinationIndex = 0;
                                    i32 sourceIndex = i + 3;
                                    while (destinationIndex < LINE_TCP_TEXT_LENGTH
                                           && gcCommandLine[sourceIndex]
                                           && gcCommandLine[sourceIndex] != ' ') {
                                        gcTCPName[destinationIndex] = gcCommandLine[sourceIndex];
                                        sourceIndex++;
                                        destinationIndex++;
                                    }
                                    gcTCPName[destinationIndex] = 0;
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
        for (i = 0; i < LINE_HELP_LINE_COUNT; i++)
            strcat(gText, gcCommandLineHelp[i]);
        ShutDown(gText);
    }

    sprintf(
        cAggPathName,
        "%s%s",
        ".\\DATA\\",
        "heroes2.agg"
    );
    DEFAULT_AGGREGATE_NAME = cAggPathName;
    sprintf(
        cExpAggPathName,
        "%s%s",
        ".\\DATA\\",
        "heroes2x.agg"
    );
    EXPANSION_AGGREGATE_NAME = cExpAggPathName;
    giFrameStep = LINE_FRAME_STEP;

    for (i = 0; i < LINE_HUMAN_PLAYER_SLOTS; i++)
        gbHumanPlayer[i] = i < giNumHumanPlayers;
    if (giNumHumanPlayers == LINE_SINGLE_PLAYER)
        gbBlackoutPlayer = false;

    if (giTCPHostStatus != -1) {
        if (giTCPType == -1 || giTCPNumPlayers == -1
            || (giTCPHostStatus == LINE_TCP_CLIENT && strlen(gcTCPAddress) < 1)) {
            ShutDown(localization::Tr("network.tcp.command_line_incomplete"));
        }
        giShowIntro = 0;
    }
    return 1;
}
#if !H2_STRICT_ENUMS
#undef destinationIndex
#undef sourceIndex
#endif

VA(0x0046829b, 0x619)
MessageDispatchResult InitMenuHandler(struct tag_message& message) {
    b32 handled = false;
    i32 idx;
    i32 menu;
    i32 helpIndex;
    i32 hoverIndex;

    PollSound();
    if (message.payload.widget.parameter & MENU_DISABLE_MASK) {
        if (message.payload.widget.command == INIT_MENU_HOVER_COMMAND
            || message.payload.widget.command == INIT_MENU_HELP_COMMAND) {
            helpIndex = -1;
            switch (message.payload.widget.id) {
                case MENU_NEW_GAME:
                    helpIndex = MENU_HELP_NEW_GAME;
                    break;
                case MENU_LOAD_GAME:
                    helpIndex = MENU_HELP_LOAD_GAME;
                    break;
                case MENU_HIGH_SCORES:
                    helpIndex = MENU_HELP_HIGH_SCORES;
                    break;
                case MENU_CREDITS:
                    helpIndex = MENU_HELP_CREDITS;
                    break;
                case MENU_EXIT:
                    helpIndex = MENU_HELP_EXIT;
                    break;
            }
            if (helpIndex >= 0) {
                NormalDialog(gInitMenuHelp[helpIndex], MENU_HELP_DIALOG);
            }
        }
    } else {
        if (message.type == INIT_MENU_KEY_PRESS) {
            switch (message.payload.keyboard.keyCode) {
                case MENU_KEY_NEW:
                    gpWindowManager->m_dialogResult = MENU_NEW_GAME;
                    handled = true;
                    break;
                case MENU_KEY_LOAD:
                    gpWindowManager->m_dialogResult = MENU_LOAD_GAME;
                    handled = true;
                    break;
                case MENU_KEY_CREDITS:
                    gpWindowManager->m_dialogResult = MENU_CREDITS;
                    handled = true;
                    break;
                case MENU_KEY_HIGH_SCORES:
                    gpWindowManager->m_dialogResult = MENU_HIGH_SCORES;
                    handled = true;
                    break;
                case MENU_KEY_EXIT:
                    gpWindowManager->m_dialogResult = MENU_EXIT;
                    handled = true;
                    break;
            }
        } else if (message.type == INIT_MENU_MESSAGE) {
            if (message.payload.widget.id < MENU_FIRST_COMMAND
                || message.payload.widget.id > MENU_LAST_ACTION) {
                return MESSAGE_DISPATCH_CONTINUE;
            }
            switch (message.payload.widget.command) {
                case INIT_MENU_HOVER_COMMAND:
                    if (message.payload.widget.id == MENU_MOVIE)
                        break;
                    menu = message.payload.widget.id - MENU_FIRST_COMMAND;
                    idx = menu + MENU_WIDGET_OFFSET;
                    message.type = INIT_MENU_MESSAGE;
                    message.payload.widget.id = idx;
                    message.payload.widget.command = INIT_MENU_SET_WIDGET_COMMAND;
                    message.payload.widget.data.value = menu * MENU_FRAME_STRIDE + MENU_HOVER_FRAME;
                    gpInitWin->BroadcastMessage(message);
                    gpInitWin->DrawWindow(0, idx, idx);
                    gpWindowManager->UpdateScreenRegion(
                        IMHotSpots[menu][IDX(INIT_MENU_HOTSPOT_X)],
                        IMHotSpots[menu][IDX(INIT_MENU_HOTSPOT_Y)],
                        IMHotSpots[menu][IDX(INIT_MENU_HOTSPOT_WIDTH)],
                        IMHotSpots[menu][IDX(INIT_MENU_HOTSPOT_HEIGHT)]
                    );
                    break;
                case INIT_MENU_CLICK_COMMAND:
                    if (message.payload.widget.id == MENU_MOVIE) {
                        PlaySmacker(MENU_MOVIE_SMACKER);
                        gpResourceManager->GetBackdrop(
                            "heroes.icn",
                            gpWindowManager->m_screen,
                            1
                        );
                        gpInitWin->DrawWindow(0);
                        gpWindowManager
                            ->UpdateScreenRegion(0, 0, MENU_SCREEN_WIDTH, MENU_SCREEN_HEIGHT);
                        gpSoundManager->PlayAmbientMusic(MENU_MAIN_MUSIC);
                        break;
                    } else {
                        gpWindowManager->m_dialogResult = message.payload.widget.id;
                        for (idx = MENU_FIRST_WIDGET; idx <= MENU_LAST_WIDGET; idx++) {
                            message.type = INIT_MENU_MESSAGE;
                            message.payload.widget.id = idx;
                            message.payload.widget.command = INIT_MENU_SET_WIDGET_COMMAND;
                            message.payload.widget.data.value =
                                (idx - MENU_WIDGET_OFFSET) * MENU_FRAME_STRIDE;
                            gpInitWin->BroadcastMessage(message);
                        }
                        gpInitWin->DrawWindow(0, MENU_FIRST_WIDGET, MENU_LAST_WIDGET);
                        gpWindowManager->UpdateScreenRegion(
                            MENU_REDRAW_LEFT,
                            MENU_REDRAW_TOP,
                            MENU_REDRAW_WIDTH,
                            MENU_REDRAW_HEIGHT
                        );
                        handled = true;
                    }
                    break;
            }
        } else if (message.type == INIT_MENU_MOUSE_MOVE) {
            hoverIndex = -1;
            for (idx = 0; idx < MENU_HOTSPOT_COUNT; idx++) {
                if (message.payload.mouse.screenX >= IMHotSpots[idx][IDX(INIT_MENU_HOTSPOT_X)]
                    && message.payload.mouse.screenY >= IMHotSpots[idx][IDX(INIT_MENU_HOTSPOT_Y)]
                    && message.payload.mouse.screenX
                           < IMHotSpots[idx][IDX(INIT_MENU_HOTSPOT_X)]
                                 + IMHotSpots[idx][IDX(INIT_MENU_HOTSPOT_WIDTH)]
                    && message.payload.mouse.screenY
                           < IMHotSpots[idx][IDX(INIT_MENU_HOTSPOT_Y)]
                                 + IMHotSpots[idx][IDX(INIT_MENU_HOTSPOT_HEIGHT)]) {
                    hoverIndex = idx;
                }
            }
            if (hoverIndex != lastIMHoverID) {
                if (lastIMHoverID != -1) {
                    message.type = INIT_MENU_MESSAGE;
                    message.payload.widget.id = lastIMHoverID + MENU_WIDGET_OFFSET;
                    message.payload.widget.command = INIT_MENU_SET_WIDGET_COMMAND;
                    message.payload.widget.data.value =
                        lastIMHoverID * MENU_FRAME_STRIDE + MENU_IDLE_FRAME;
                    gpInitWin->BroadcastMessage(message);
                    gpInitWin->DrawWindow(
                        0,
                        lastIMHoverID + MENU_WIDGET_OFFSET,
                        lastIMHoverID + MENU_WIDGET_OFFSET
                    );
                    gpWindowManager->UpdateScreenRegion(
                        IMHotSpots[lastIMHoverID][IDX(INIT_MENU_HOTSPOT_X)],
                        IMHotSpots[lastIMHoverID][IDX(INIT_MENU_HOTSPOT_Y)],
                        IMHotSpots[lastIMHoverID][IDX(INIT_MENU_HOTSPOT_WIDTH)],
                        IMHotSpots[lastIMHoverID][IDX(INIT_MENU_HOTSPOT_HEIGHT)]
                    );
                }
                if (hoverIndex != -1) {
                    message.type = INIT_MENU_MESSAGE;
                    message.payload.widget.id = hoverIndex + MENU_WIDGET_OFFSET;
                    message.payload.widget.command = INIT_MENU_SET_WIDGET_COMMAND;
                    message.payload.widget.data.value =
                        hoverIndex * MENU_FRAME_STRIDE + MENU_ACTIVE_FRAME;
                    gpInitWin->BroadcastMessage(message);
                    gpInitWin->DrawWindow(
                        0,
                        hoverIndex + MENU_WIDGET_OFFSET,
                        hoverIndex + MENU_WIDGET_OFFSET
                    );
                    gpWindowManager->UpdateScreenRegion(
                        IMHotSpots[hoverIndex][IDX(INIT_MENU_HOTSPOT_X)],
                        IMHotSpots[hoverIndex][IDX(INIT_MENU_HOTSPOT_Y)],
                        IMHotSpots[hoverIndex][IDX(INIT_MENU_HOTSPOT_WIDTH)],
                        IMHotSpots[hoverIndex][IDX(INIT_MENU_HOTSPOT_HEIGHT)]
                    );
                }
                lastIMHoverID = hoverIndex;
            }
        }
    }

    if (handled || giMenuCommand != -1) {
        message.type = INIT_MENU_MESSAGE;
        message.payload.widget.id = MENU_CLOSE_COMMAND;
        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
        return MESSAGE_DISPATCH_FORWARD;
    }
    CheckShingleUpdate();
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x004688b4, 0x10)
MessageDispatchResult NullHandler(struct tag_message& H2_UNUSED(message)) {
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x004688c4, 0x145)
MessageDispatchResult RecruitHeroHandler(tag_message& message) {
    i16 H2_UNUSED(unusedLocal0L) = 2, H2_UNUSED(unusedLocal1H) = 3,
        H2_UNUSED(unusedLocal2D) = 8, H2_UNUSED(unusedLocal3A) = 9;
    b32 shouldClose = false;
    i32 H2_UNUSED(unusedResult);
    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_SELECT:
                switch (message.payload.widget.id) {
                    case RECRUIT_HERO_VIEW_BUTTON:
                        HeroView(gpTownManager->m_recruitHero->m_id, true, false);
                        gpTownManager->RedrawTownScreen();
                        gpTownManager->m_heroWindow0->DrawWindow();
                        gpTownManager->m_heroWindow1->DrawWindow();
                        gpWindowManager->FadeScreen(FADE_IN, RECRUIT_HERO_FADE_STEPS, NULL);
                        break;
                    default:
                        break;
                }
                break;
            case WIDGET_COMMAND_DESELECT:
                switch (message.payload.widget.id) {
                    case EVENT_WINDOW_SECOND_BUTTON:
                        gpTownManager->m_recruitState = -1;
                        shouldClose = true;
                        break;
                    case EVENT_WINDOW_THIRD_BUTTON:
                        gpTownManager->m_recruitState = 0;
                        gpWindowManager->m_dialogResult = message.payload.widget.id;
                        shouldClose = true;
                        break;
                }
                break;
            default:
                break;
        }
    }
    if (shouldClose == 1) {
        message.payload.widget.id = EVENT_WINDOW_CLOSE_COMMAND;
        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x00468a09, 0x179)
H2_CONST char* GetBuildingInfo(FactionType race, BuildingSlotType building, i32 mode) {
    char buf[BUILDING_INFO_BUFFER_SIZE];
    if (race == FACTION_NECROMANCER && building == BUILDING_SLOT_NECROMANCER_SHRINE) {
        sprintf(buf, xNecromancerShrineDesc);
    } else if (building == BUILDING_SLOT_WELL_EXTRA) {
        sprintf(
            buf,
            localization::Tr("town.building.weekly_growth"),
            GetBuildingName(race, building),
            gArmyNamesPlural[IDX(gDwellingType[IDX(race)][0])]
        );
    } else if (building == BUILDING_SLOT_SPECIAL) {
        sprintf(buf, gBuildingInfoSpecial[IDX(race)]);
    } else if (building < BUILDING_SLOT_DWELLING_FIRST) {
        sprintf(buf, cBuildingInfoNeutral[IDX(building)]);
    } else {
        sprintf(
            gText,
            localization::Tr("town.building.produces"),
            GetBuildingName(race, building),
            gArmyNamesPlural
                [IDX(gDwellingType[IDX(race)][IDX(building) - IDX(BUILDING_SLOT_DWELLING_FIRST)])]
        );
        return gText;
    }
    if (mode) {
        sprintf(
            gText,
            "{%s}\n\n%s",
            GetBuildingName(race, building),
            buf
        );
    } else {
        sprintf(gText, buf);
    }
    return gText;
}

VA(0x00468b82, 0x69)
H2_CONST char* GetBuildingName(FactionType race, BuildingSlotType building) {
    if (race == FACTION_NECROMANCER && building == BUILDING_SLOT_NECROMANCER_SHRINE)
        return xNecromancerShrine;
    if (building == BUILDING_SLOT_WELL_EXTRA)
        return gWellExtraNames[IDX(race)];
    else if (building == BUILDING_SLOT_SPECIAL)
        return gSpecialBuildingNames[IDX(race)];
    else if (building < BUILDING_SLOT_DWELLING_FIRST)
        return gNeutralBuildingNames[IDX(building)];
    else
        return gDwellingNames[IDX(race)][IDX(building) - IDX(BUILDING_SLOT_DWELLING_FIRST)];
}

VA(0x00468beb, 0xf0)
void GetBuildingCost(FactionType race, BuildingSlotType building, i32* const destination, i32 mageLevel) {
    i32 level;
    if (building == BUILDING_SLOT_NECROMANCER_SHRINE && race == FACTION_NECROMANCER) {
        memcpy(destination, xShrineBuildingCost, KB_BUILDING_RESOURCE_COUNT * sizeof(i32));
    } else if (building >= BUILDING_SLOT_DWELLING_FIRST
               && building <= BUILDING_SLOT_DWELLING_LAST) {
        memcpy(
            destination,
            gDwellingCosts[IDX(race)][IDX(building) - IDX(BUILDING_SLOT_DWELLING_FIRST)],
            KB_BUILDING_RESOURCE_COUNT * sizeof(i32)
        );
    } else if (building == BUILDING_SLOT_MAGE_GUILD) {
        level = mageLevel + 1;
        if (level > KB_MAGE_GUILD_MAX_LEVEL)
            level = KB_MAGE_GUILD_MAX_LEVEL;
        memcpy(destination, gMageBuildingCosts[mageLevel + 1], KB_BUILDING_RESOURCE_COUNT * sizeof(i32));
    } else if (building == BUILDING_SLOT_SPECIAL) {
        memcpy(destination, gSpecialBuildingCosts[IDX(race)], KB_BUILDING_RESOURCE_COUNT * sizeof(i32));
    } else {
        if (building >= BUILDING_SLOT_DISABLED_SECOND)
            return;
        memcpy(
            destination,
            gNeutralBuildingCosts[IDX(building)],
            KB_BUILDING_RESOURCE_COUNT * sizeof(i32)
        );
    }
}

VA(0x00468cdb, 0x15)
H2_CONST char* GetMonsterName(H2_ENUM_PARAM(CreatureType, i32) monster) {
    return gArmyNames[IDX(monster)];
}

VA(0x00468cf0, 0x15)
H2_CONST char* GetMonsterPluralName(H2_ENUM_PARAM(CreatureType, i32) monster) {
    return gArmyNamesPlural[IDX(monster)];
}

VA(0x00468d05, 0x10d)
void GetMonsterCost(CreatureType monster, i32* const cost) {
    i32 idx;
    for (idx = 0; idx < KB_BUILDING_RESOURCE_COUNT; idx++)
        cost[idx] = 0;
    cost[IDX(RES_GOLD)] = gMonsterDatabase[IDX(monster)].cost;
    switch (monster) {
        case CREATURE_GENIE:
            cost[IDX(RES_GEMS)] = MONSTER_RARE_RESOURCE_COST;
            break;
        case CREATURE_PHOENIX:
            cost[IDX(RES_MERCURY)] = MONSTER_RARE_RESOURCE_COST;
            break;
        case CREATURE_CYCLOPS:
            cost[IDX(RES_CRYSTAL)] = MONSTER_RARE_RESOURCE_COST;
            break;
        case CREATURE_GREEN_DRAGON:
        case CREATURE_RED_DRAGON:
            cost[IDX(RES_SULFUR)] = MONSTER_RARE_RESOURCE_COST;
            break;
        case CREATURE_BLACK_DRAGON:
            cost[IDX(RES_SULFUR)] = MONSTER_UPGRADED_RARE_RESOURCE_COST;
            break;
        case CREATURE_GIANT:
            cost[IDX(RES_GEMS)] = MONSTER_RARE_RESOURCE_COST;
            break;
        case CREATURE_TITAN:
            cost[IDX(RES_GEMS)] = MONSTER_UPGRADED_RARE_RESOURCE_COST;
            break;
    }
}

VA(0x00468e12, 0x273)
i32 CanBuild(town* t, BuildingSlotType building) {
    i32 reqBits;
    i32 curMask;
    if (BitTest(gpGame->m_knownTowns, t->m_id))
        return 0;
    if (building != BUILDING_SLOT_CASTLE && !HAS(t->m_buildings, IDX(TOWN_BUILDING_CASTLE)))
        return 0;
    if (!xIsExpansionMap && building == BUILDING_SLOT_NECROMANCER_SHRINE
        && t->m_type == FACTION_NECROMANCER)
        return 0;
    if (building == BUILDING_SLOT_DOCK) {
        if (t->CanBuildDock())
            return 1;
        else
            return 0;
    }
    if (building == BUILDING_SLOT_MAGE_GUILD && t->m_buildState >= KB_MAGE_GUILD_MAX_LEVEL)
        return 0;
    if (building == BUILDING_SLOT_UPGRADE_CASTLE || building == BUILDING_SLOT_DISABLED_FIRST
        || building == BUILDING_SLOT_DISABLED_SECOND || building == BUILDING_SLOT_DISABLED_THIRD
        || building == BUILDING_SLOT_DISABLED_FOURTH || building == BUILDING_SLOT_DISABLED_LAST)
        return 0;
    if (building < BUILDING_SLOT_DWELLING_FIRST || building > BUILDING_SLOT_DWELLING_LAST)
        return 1;
    if ((building == BUILDING_SLOT_DWELLING_SECOND
         && HAS(t->m_buildings, IDX(KB_DWELLING_UPGRADE_FIRST_FLAG)))
        || (building == BUILDING_SLOT_DWELLING_THIRD
            && HAS(t->m_buildings, IDX(KB_DWELLING_UPGRADE_SECOND_FLAG)))
        || (building == BUILDING_SLOT_DWELLING_FOURTH
            && HAS(t->m_buildings, IDX(KB_DWELLING_UPGRADE_THIRD_FLAG)))
        || (building == BUILDING_SLOT_DWELLING_FIFTH
            && HAS(t->m_buildings, IDX(KB_DWELLING_UPGRADE_FOURTH_FLAG)))
        || (building == BUILDING_SLOT_DWELLING_SIXTH
            && (HAS(t->m_buildings, IDX(KB_DWELLING_UPGRADE_FIFTH_FLAG))
                || HAS(t->m_buildings, IDX(KB_DWELLING_UPGRADE_SIXTH_FLAG))))
        || (building == BUILDING_SLOT_UPGRADE_LAST
            && HAS(t->m_buildings, IDX(KB_DWELLING_UPGRADE_SIXTH_FLAG))))
        return 0;
    reqBits = gHierarchyMask[IDX(t->m_type)][IDX(building) - IDX(BUILDING_SLOT_DWELLING_FIRST)];
    curMask = t->m_buildings;
    if (curMask & IDX(KB_DWELLING_UPGRADE_FIRST_FLAG))
        curMask |= IDX(KB_DWELLING_FIRST_FLAG);
    if (curMask & IDX(KB_DWELLING_UPGRADE_SECOND_FLAG))
        curMask |= IDX(KB_DWELLING_SECOND_FLAG);
    if (curMask & IDX(KB_DWELLING_UPGRADE_THIRD_FLAG))
        curMask |= IDX(KB_DWELLING_THIRD_FLAG);
    if (curMask & IDX(KB_DWELLING_UPGRADE_FOURTH_FLAG))
        curMask |= IDX(KB_DWELLING_FOURTH_FLAG);
    if (curMask & IDX(KB_DWELLING_UPGRADE_SIXTH_FLAG))
        curMask |= IDX(KB_DWELLING_UPGRADE_FIFTH_FLAG);
    if (curMask & IDX(KB_DWELLING_UPGRADE_FIFTH_FLAG))
        curMask |= IDX(KB_DWELLING_FIFTH_FLAG);
    if ((reqBits & curMask) == reqBits) {
        if (t->m_type == FACTION_NECROMANCER
            && building == BUILDING_SLOT_NECROMANCER_MAGE_PREREQUISITE && t->m_buildState <= 1)
            return 0;
        return 1;
    }
    return 0;
}

VA(0x00469085, 0x80)
i32 CanBuy(town* t, BuildingSlotType type) {
    i32 buf[KB_BUILDING_RESOURCE_COUNT];
    playerData* ptr;
    i32 r;
    GetBuildingCost(t->m_type, type, buf, t->m_buildState);
    ptr = &gpGame->m_players[giCurPlayer];
    for (r = 0; r < KB_BUILDING_RESOURCE_COUNT; r++)
        if (ptr->m_resources[r] < buf[r])
            return 0;
    return 1;
}

VA(0x00469105, 0x7b)
i32 GetBuildingBaseResourceValue(FactionType race, BuildingSlotType building, i32 level) {
    if (race == FACTION_NECROMANCER && building == BUILDING_SLOT_UPGRADE_CASTLE)
        return NECROMANCER_CASTLE_UPGRADE_BASE_RESOURCE_VALUE;
    if (building < BUILDING_SLOT_DWELLING_FIRST || building > BUILDING_SLOT_DWELLING_LAST) {
        if (building > BUILDING_SLOT_NEUTRAL_LAST)
            return 0;
        else if (building == BUILDING_SLOT_MAGE_GUILD)
            return gMageBaseResourceValues[level];
        else if (building == BUILDING_SLOT_SPECIAL)
            return gSpecialBuildingBaseResourceValues[IDX(race)];
        else
            return gNeutralBaseResourceValues[IDX(building)];
    } else {
        return gDwellingBaseResourceValues[IDX(race)]
                                          [IDX(building) - IDX(BUILDING_SLOT_DWELLING_FIRST)];
    }
}

VA(0x00469180, 0x1af)
MessageDispatchResult WaitHandler(tag_message& message) {
    i32 result = 0;
    gbFunctionComplete = true;
    PollSound();
    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                switch (message.payload.widget.id) {
                    case EVENT_WINDOW_FIRST_BUTTON:
                    case EVENT_WINDOW_SECOND_BUTTON:
                    case EVENT_WINDOW_THIRD_BUTTON:
                        gbFunctionComplete = false;
                        result = 1;
                        break;
                }
        }
    }
    if (result == 0) {
        switch (giWaitType) {
            case DIALOG_WAIT_OTHER_PLAYER:
                result = WaitForOtherPlayer();
                break;
            case DIALOG_WAIT_NETBIOS_HOST:
                result = WaitForHost();
                break;
            case DIALOG_WAIT_NETBIOS_GUEST:
                result = WaitForGuest();
                break;
            case DIALOG_WAIT_NETBIOS_INIT_GUEST:
                result = InitNetGuest();
                break;
            case DIALOG_WAIT_NETBIOS_INIT_HOST:
                result = InitNetHost();
                break;
            case DIALOG_WAIT_MODEM_COMMAND:
                result = GUIModemCommandExec();
                break;
            case DIALOG_WAIT_MODEM_RESPONSE:
                result = GUIModemResponseExec();
                break;
            case DIALOG_WAIT_DIRECT_CONNECT:
                result = WaitForDirectConnect();
                break;
            case DIALOG_WAIT_DIRECTPLAY_FIRST_GUEST:
                result = dpWaitForFirstGuest();
                break;
            case DIALOG_WAIT_DIRECTPLAY_GUESTS:
                result = dpWaitForExtraGuests();
                break;
            case DIALOG_WAIT_DIRECTPLAY_HOST:
                result = dpWaitForHost();
                break;
            case DIALOG_WAIT_WINSOCK_FIRST_GUEST:
                result = wsWaitForFirstGuest();
                break;
            case DIALOG_WAIT_WINSOCK_GUESTS:
                result = wsWaitForExtraGuests();
                break;
            case DIALOG_WAIT_WINSOCK_HOST:
                result = wsWaitForHost();
                break;
        }
    }
    CheckShingleUpdate();
    if (result != 0) {
        gpWindowManager->m_dialogResult = EVENT_WINDOW_SECOND_BUTTON;
        message.type = MESSAGE_WIDGET;
        message.payload.widget.id = EVENT_WINDOW_CLOSE_COMMAND;
        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x0046932f, 0x3f2)
MessageDispatchResult EventWindowHandler(struct tag_message& message) {
    i32 resType;
    i32 resExtra;

    if (!gpSoundManager->MusicPlaying() && gpAdvManager->m_active == true)
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[IDX(gpAdvManager->m_currentTerrain)]
        );
    if (giDialogTimeout != 0 && KBTickCount() > giDialogTimeout) {
        message.type = MESSAGE_WIDGET;
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = EVENT_WINDOW_CLOSE_COMMAND;
        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
        giDialogTimeout = 0;
        return MESSAGE_DISPATCH_FORWARD;
    }
    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_SELECT:
            case WIDGET_COMMAND_ALTERNATE_SELECT:
                resType = NORMAL_DIALOG_NO_RESOURCE;
                resExtra = NORMAL_DIALOG_NO_VALUE;
                if (message.payload.widget.parameter & EVENT_WINDOW_RESOURCE_FLAG) {
                    switch (message.payload.widget.id) {
                        case EVENT_WINDOW_FIRST_RESOURCE_WIDGET:
                            resType = giResType1;
                            resExtra = giResExtra1;
                            break;
                        case EVENT_WINDOW_SECOND_RESOURCE_WIDGET:
                            resType = giResType2;
                            resExtra = giResExtra2;
                            break;
                    }
                    switch (resType) {
                        case EVENT_WINDOW_LUCK:
                            NormalDialog(cLuckInfo[IDX(LUCK_INFO_GOOD)], NORMAL_DIALOG_QUICK_VIEW);
                            break;
                        case EVENT_WINDOW_BAD_LUCK:
                            NormalDialog(cLuckInfo[IDX(LUCK_INFO_BAD)], NORMAL_DIALOG_QUICK_VIEW);
                            break;
                        case EVENT_WINDOW_MORALE:
                            NormalDialog(
                                cMoraleInfo[IDX(MORALE_INFO_GOOD)],
                                NORMAL_DIALOG_QUICK_VIEW
                            );
                            break;
                        case EVENT_WINDOW_BAD_MORALE:
                            NormalDialog(
                                cMoraleInfo[IDX(MORALE_INFO_BAD)],
                                NORMAL_DIALOG_QUICK_VIEW
                            );
                            break;
                        case EVENT_WINDOW_EXPERIENCE:
                            NormalDialog(
                                localization::Tr("help.experience"),
                                NORMAL_DIALOG_QUICK_VIEW
                            );
                            break;
                        case NORMAL_DIALOG_ARTIFACT:
                            if (resExtra == IDX(ARTIFACT_SPELL_SCROLL)) {
                                sprintf(gText, gArtifactDesc[resExtra], gSpellNames[xTheSpell]);
                                NormalDialog(gText, NORMAL_DIALOG_QUICK_VIEW);
                            } else {
                                NormalDialog(gArtifactDesc[resExtra], NORMAL_DIALOG_QUICK_VIEW);
                            }
                            break;
                        case NORMAL_DIALOG_SPELL:
                            NormalDialog(gSpellDesc[resExtra], NORMAL_DIALOG_QUICK_VIEW);
                            break;
                        case NORMAL_DIALOG_SECONDARY_SKILL:
                            NormalDialog(
                                cSecSkillDesc[resExtra / SECONDARY_SKILL_VALUE_LEVEL_COUNT]
                                             [resExtra % SECONDARY_SKILL_VALUE_LEVEL_COUNT],
                                NORMAL_DIALOG_QUICK_VIEW
                            );
                            break;
                        case NORMAL_DIALOG_PRIMARY_SKILL:
                            NormalDialog(gStatDesc[resExtra], NORMAL_DIALOG_QUICK_VIEW);
                            break;
                        case NORMAL_DIALOG_RESOURCE_WOOD:
                        case NORMAL_DIALOG_RESOURCE_MERCURY:
                        case NORMAL_DIALOG_RESOURCE_ORE:
                        case NORMAL_DIALOG_RESOURCE_SULFUR:
                        case NORMAL_DIALOG_RESOURCE_CRYSTAL:
                        case NORMAL_DIALOG_RESOURCE_GEMS:
                        case NORMAL_DIALOG_RESOURCE_GOLD:
                            NormalDialog(
                                localization::Tr("help.resources"),
                                NORMAL_DIALOG_QUICK_VIEW
                            );
                            break;
                    }
                }
                break;
            case WIDGET_COMMAND_DESELECT:
                switch (message.payload.widget.id) {
                    case EVENT_WINDOW_FIRST_BUTTON:
                    case EVENT_WINDOW_SECOND_BUTTON:
                    case EVENT_WINDOW_THIRD_BUTTON:
                    case EVENT_WINDOW_FOURTH_BUTTON:
                    case EVENT_WINDOW_FIFTH_BUTTON:
                    case EVENT_WINDOW_SIXTH_BUTTON:
                    case EVENT_WINDOW_SEVENTH_BUTTON:
                    case EVENT_WINDOW_EIGHTH_BUTTON:
                        gpWindowManager->m_dialogResult = message.payload.widget.id;
                        message.payload.widget.id = EVENT_WINDOW_CLOSE_COMMAND;
                        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
                        giDialogTimeout = 0;
                        return MESSAGE_DISPATCH_FORWARD;
                    case EVENT_WINDOW_IGNORED_BUTTON:
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x00469721, 0x13)
MessageDispatchResult TrueFalseDialogHandler(struct tag_message& message) {
    return EventWindowHandler(message);
}

VA(0x00469734, 0x165)
void PlayerDead(i32 player) {
    playerData* rec;
    i32 i;
    gbRetreatWin = false;
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
            SendMapChange(
                MAP_CHANGE_DEAD_PLAYER,
                static_cast<i8>(player),
                0,
                0,
                MAP_CHANGE_CURRENT_PLAYER,
                0,
                0
            );
    }
}

VA(0x00469899, 0x1630)
void CheckEndGame(
    H2_ENUM_PARAM(CheckEndGameForcedResult, i32) forcedResult,
    b32 dragonCityCaptured
) {
    b32 showedDialog_o;
    b32 defeated_m;
    b32 allowNormalVictory;
    i32 H2_UNUSED(lastLivingPlayer_j);
    i32 survivingHumans_a;
    i32 lastHuman_a;
    i32 netHumanCount;
    i32 player;
    i32 heroIndex_m;
    b32 winFlag;
    playerData* rec_n;
    b32 savedRemoteOn_o;
    i32 numAlive;
    i32 sideBelow_i;
    i32 sideAbove;
    i32 bestGold;
    town* lossTown;
    town* victoryTownData;
    i32 currentDayIndex;
    b32 enemyRemaining;
    b32 hasRoland_j;
    b32 hasDwarfTown;
    char artifactName[END_GAME_TEXT_BUFFER_SIZE];
    hero* artifactHeroPtr_c;
    i32 artifactWinnerPerson;
    hero* lossHero_k;
    hero* winningHeroEntry_g;
    i32 winnerPlayer_m;
    char campaignSaveName[END_GAME_CAMPAIGN_SAVE_NAME_SIZE];
    i32 campaignHeroIndex;
    u32 carryoverHeroId;

    if (!gbThisNetGotAdventureControl)
        return;
    if (gbInNewGameSetup)
        return;
    if (gbGameOver)
        return;
    if (bInCheckEndGame)
        return;

    bInCheckEndGame = true;
    savedRemoteOn_o = gbRemoteOn;
    showedDialog_o = false;

    for (player = 0; player < gpGame->m_playerCount; player++) {
        if (!gpGame->m_playerDead[player]) {
            rec_n = &gpGame->m_players[player];
            if ((rec_n->m_heroCount == 0 && rec_n->m_townCount == 0)
                || (xIsPlayingExpansionCampaign && xCampaign.IsSpecialLossCondition(player))) {
                PlayerDead(player);
                if (player == giThisGamePos) {
                    showedDialog_o = true;
                    sprintf(gText, localization::Tr("player.eliminated"));
                    NormalDialog(gText, 1);
                } else {
                    sprintf(gText, localization::Tr("player.vanquished"), cPlayerNames[player]);
                    NormalDialog(
                        gText,
                        1,
                        -1,
                        -1,
                        END_GAME_PLAYER_DIALOG_ICON,
                        gpGame->m_players[static_cast<i8>(player)].m_color,
                        -1,
                        -1,
                        -1,
                        END_GAME_REMOTE_DIALOG_TIME
                    );
                }
            } else if (rec_n->m_townCount == 0) {
                if (rec_n->m_daysLeft == -1) {
                    if (gbThisNetHumanPlayer[player] && player == giCurPlayer) {
                        sprintf(
                            gText,
                            localization::Tr("player.last_town_warning"),
                            cPlayerNames[player]
                        );
                        NormalDialog(
                            gText,
                            1,
                            -1,
                            -1,
                            END_GAME_PLAYER_DIALOG_ICON,
                            gpGame->m_players[static_cast<i8>(player)].m_color,
                            -1,
                            0,
                            -1,
                            0
                        );
                    }
                    rec_n->m_daysLeft = END_GAME_GRACE_DAYS;
                } else if (rec_n->m_daysLeft == 0) {
                    PlayerDead(player);
                    if (gbThisNetHumanPlayer[player] && player == giCurPlayer) {
                        if (!showedDialog_o) {
                            showedDialog_o = true;
                            sprintf(
                                gText,
                                localization::Tr("player.banished.self"),
                                cPlayerNames[player]
                            );
                        }
                    } else {
                        sprintf(
                            gText,
                            localization::Tr("player.banished.other"),
                            cPlayerNames[player]
                        );
                    }
                    NormalDialog(
                        gText,
                        1,
                        -1,
                        -1,
                        END_GAME_PLAYER_DIALOG_ICON,
                        gpGame->m_players[static_cast<i8>(player)].m_color,
                        -1,
                        0,
                        -1,
                        0
                    );
                }
            } else {
                rec_n->m_daysLeft = -1;
            }
        }
    }

    numAlive = 0;
    lastLivingPlayer_j = 0;
    survivingHumans_a = 0;
    lastHuman_a = 0;
    netHumanCount = 0;
    for (player = 0; player < gpGame->m_playerCount; player++) {
        if (!gpGame->m_playerDead[player]) {
            numAlive++;
            lastLivingPlayer_j = player;
            if (gbThisNetHumanPlayer[player]) {
                netHumanCount++;
            }
            if (gbHumanPlayer[player]) {
                survivingHumans_a++;
                lastHuman_a = player;
            }
        }
    }

    winFlag = false;
    defeated_m = false;
    allowNormalVictory = true;
    if ((gpGame->m_mapHeader.victoryCondition != MAP_VICTORY_DEFEAT_ALL
         && !gpGame->m_mapHeader.allowNormalVictory)
        || (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ARCHIBALD
            && gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET == END_GAME_SIDE_SCENARIO)) {
        allowNormalVictory = false;
    }

    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_SIDE
        && gpGame->m_mapHeader.victoryConditionValue != CAMPAIGN_SWITCH_VICTORY_VALUE
        && (!gbInCampaign || gpGame->m_campaignType != CAMPAIGN_ARCHIBALD
            || gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET != END_GAME_SIDE_SCENARIO)) {
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
                if (gbThisNetHumanPlayer[player] && !gpGame->m_playerDead[player]
                    && gpGame->m_players[player].m_color
                           >= gpGame->m_mapHeader.victorySideThreshold) {
                    winFlag = true;
                }
            }
        } else if (sideAbove == 0) {
            for (player = 0; player < gpGame->m_playerCount; player++) {
                if (gbThisNetHumanPlayer[player] && !gpGame->m_playerDead[player]
                    && gpGame->m_players[player].m_color
                           < gpGame->m_mapHeader.victorySideThreshold) {
                    winFlag = true;
                }
            }
        }
        if (sideBelow_i == 0 || sideAbove == 0) {
            if (!winFlag) {
                defeated_m = true;
            }
            if (!showedDialog_o && winFlag) {
                showedDialog_o = true;
                sprintf(gText, localization::Tr("victory.side_triumph"));
                NormalDialog(gText, 1);
            }
        }
    }

    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_CAPTURE_TOWN) {
        victoryTownData = gpGame->GetTown(gpGame->GetTownId(
            gpGame->m_mapHeader.victoryConditionValue,
            gpGame->m_mapHeader.victoryTownY
        ));
        if (victoryTownData->m_owner != TOWN_OWNER_NONE
            && (gbHumanPlayer[IDX(victoryTownData->m_owner)] || gpGame->m_mapHeader.computerAlsoWins)) {
            if (gbThisNetHumanPlayer[IDX(victoryTownData->m_owner)]) {
                winFlag = true;
            } else {
                defeated_m = true;
            }
            if (!showedDialog_o) {
                showedDialog_o = true;
                if (winFlag) {
                    sprintf(
                        gText,
                        localization::Tr("victory.capture_town.self"),
                        victoryTownData->m_name
                    );
                } else {
                    sprintf(
                        gText,
                        localization::Tr("victory.capture_town.enemy"),
                        victoryTownData->m_name
                    );
                }
                NormalDialog(gText, 1);
            }
        }
    }

    if (gpGame->m_mapHeader.lossCondition == MAP_LOSS_TOWN) {
        lossTown = gpGame->GetTown(
            gpGame->GetTownId(gpGame->m_mapHeader.lossConditionValue, gpGame->m_mapHeader.lossTownY)
        );
        if (lossTown->m_owner == TOWN_OWNER_NONE || !gbHumanPlayer[IDX(lossTown->m_owner)]) {
            defeated_m = true;
            if (!showedDialog_o) {
                showedDialog_o = true;
                sprintf(gText, localization::Tr("loss.town_fallen"), lossTown->m_name);
                NormalDialog(gText, 1);
            }
        }
    }

    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_ACCUMULATE_GOLD) {
        bestGold = 0;
        winnerPlayer_m = END_GAME_NO_PLAYER;
        for (player = 0; player < gpGame->m_playerCount; player++) {
            if ((gbHumanPlayer[player] || gpGame->m_mapHeader.computerAlsoWins)
                && gpGame->m_players[player].m_resources[IDX(RES_GOLD)]
                       >= gpGame->m_mapHeader.victoryConditionValue * END_GAME_GOLD_SCALE
                && gpGame->m_players[player].m_resources[IDX(RES_GOLD)] >= bestGold) {
                bestGold = gpGame->m_players[player].m_resources[IDX(RES_GOLD)];
                winnerPlayer_m = player;
            }
            if (winnerPlayer_m != END_GAME_NO_PLAYER) {
                if (gbThisNetHumanPlayer[IDX(winnerPlayer_m)]) {
                    winFlag = true;
                } else {
                    defeated_m = true;
                }
                if (!showedDialog_o) {
                    showedDialog_o = true;
                    if (winFlag) {
                        sprintf(
                            gText,
                            localization::Tr("victory.gold.self"),
                            bestGold
                        );
                    } else {
                        sprintf(
                            gText,
                            localization::Tr("victory.gold.enemy"),
                            bestGold
                        );
                    }
                    NormalDialog(gText, 1);
                }
            }
        }
    }

    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_HERO) {
        winningHeroEntry_g = GetHeroSlot(gpGame->m_mapHeader.victoryConditionValue);
        if (winningHeroEntry_g->m_owner < 0 || winningHeroEntry_g->m_owner >= GAME_PLAYER_COUNT
            || gbHumanPlayer[IDX(winningHeroEntry_g->m_owner)]) {
            winFlag = true;
            if (!showedDialog_o) {
                showedDialog_o = true;
                sprintf(
                    gText,
                    localization::Tr("victory.hero_captured"),
                    winningHeroEntry_g->m_name
                );
                NormalDialog(gText, 1);
            }
        }
    }

    if (gpGame->m_mapHeader.lossCondition == MAP_LOSS_HERO) {
        lossHero_k = GetHeroSlot(gpGame->m_mapHeader.lossConditionValue);
        if (lossHero_k->m_owner < 0 || lossHero_k->m_owner >= GAME_PLAYER_COUNT
            || !gbHumanPlayer[IDX(lossHero_k->m_owner)]) {
            defeated_m = true;
            if (!showedDialog_o) {
                showedDialog_o = true;
                sprintf(gText, localization::Tr("loss.hero"), lossHero_k->m_name);
                NormalDialog(gText, 1);
            }
        }
    }

    if (gpGame->m_mapHeader.lossCondition == MAP_LOSS_TIME) {
        if (GAME_DAY_NUMBER(*gpGame) > gpGame->m_mapHeader.lossConditionValue) {
            defeated_m = true;
            if (!showedDialog_o) {
                showedDialog_o = true;
                sprintf(gText, localization::Tr("loss.time_expired"));
                NormalDialog(gText, 1);
            }
        }
    }

    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_FIND_ARTIFACT) {
        artifactWinnerPerson = END_GAME_NO_PLAYER;
        for (player = 0; player < gpGame->m_playerCount; player++) {
            if (!gpGame->m_playerDead[player]) {
                for (heroIndex_m = 0; heroIndex_m < gpGame->m_players[player].m_heroCount;
                     heroIndex_m++) {
                    artifactHeroPtr_c = gpGame->GetPlayerHero(player, heroIndex_m);
                    if (gpGame->m_mapHeader.victoryConditionValue > END_GAME_ULTIMATE_ARTIFACT) {
                        if (artifactHeroPtr_c->HasArtifact(
                                ArtifactType(gpGame->m_mapHeader.victoryConditionValue - 1)
                            )) {
                            artifactWinnerPerson = player;
                        }
                    } else {
                        if (artifactHeroPtr_c->HasArtifact(ARTIFACT_ULTIMATE_BOOK)
                            || artifactHeroPtr_c->HasArtifact(ARTIFACT_ULTIMATE_SWORD)
                            || artifactHeroPtr_c->HasArtifact(ARTIFACT_ULTIMATE_CLOAK)
                            || artifactHeroPtr_c->HasArtifact(ARTIFACT_ULTIMATE_WAND)
                            || artifactHeroPtr_c->HasArtifact(ARTIFACT_ULTIMATE_SHIELD)
                            || artifactHeroPtr_c->HasArtifact(ARTIFACT_ULTIMATE_STAFF)
                            || artifactHeroPtr_c->HasArtifact(ARTIFACT_ULTIMATE_CROWN)
                            || artifactHeroPtr_c->HasArtifact(ARTIFACT_GOLDEN_GOOSE)) {
                            artifactWinnerPerson = player;
                        }
                    }
                }
            }
        }
        if (artifactWinnerPerson != END_GAME_NO_PLAYER) {
            if (gbThisNetHumanPlayer[artifactWinnerPerson]) {
                winFlag = true;
            } else {
                defeated_m = true;
            }
            if (!showedDialog_o) {
                showedDialog_o = true;
                if (gpGame->m_mapHeader.victoryConditionValue == END_GAME_ULTIMATE_ARTIFACT) {
                    sprintf(
                        artifactName,
                        localization::Tr("artifact.ultimate_generic")

                    );
                } else {
                    sprintf(
                        artifactName,
                        gArtifactNames[gpGame->m_mapHeader.victoryConditionValue - 1]
                    );
                }
                if (winFlag) {
                    sprintf(gText, localization::Tr("victory.artifact_found.self"), artifactName);
                } else {
                    sprintf(
                        gText,
                        localization::Tr("victory.artifact_found.enemy"),
                        artifactName
                    );
                }
                NormalDialog(gText, 1);
            }
        }
    }

    if (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ROLAND
        && gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET == END_GAME_DWARF_SCENARIO) {
        hasDwarfTown = false;
        for (player = 0; player < gpGame->m_players[0].m_townCount; player++) {
            if (gpGame->GetTown(gpGame->m_players[0].m_townIds[player])->m_type
                == FACTION_SORCERESS) {
                hasDwarfTown = true;
            }
        }
        if (!hasDwarfTown) {
            defeated_m = true;
            if (!showedDialog_o) {
                showedDialog_o = true;
                sprintf(
                    gText,
                    localization::Tr("campaign.loss.dwarf_towns")
                );
                NormalDialog(gText, 1);
            }
        }
    }

    if (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ARCHIBALD
        && gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET == END_GAME_SIDE_SCENARIO
        && dragonCityCaptured) {
        winFlag = true;
        if (!showedDialog_o) {
            showedDialog_o = true;
            sprintf(gText, localization::Tr("campaign.victory.dragon_city"));
            NormalDialog(gText, 1);
        }
    }

    if (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ROLAND
        && gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET
               == END_GAME_ROLAND_CAPTURE_SCENARIO) {
        hasRoland_j = false;
        for (player = 0; player < GAME_HERO_COUNT; player++) {
            if (gpGame->m_heroRecs[player].m_portrait == CAMPAIGN_HERO_ROLAND
                && gpGame->m_heroRecs[player].m_owner >= 0
                && gpGame->m_heroRecs[player].m_owner <= GAME_PLAYER_COUNT - 1) {
                hasRoland_j = true;
            }
        }
        if (!hasRoland_j) {
            defeated_m = true;
            if (!showedDialog_o) {
                showedDialog_o = true;
                sprintf(gText, localization::Tr("campaign.loss.roland_captured"));
                NormalDialog(gText, 1);
            }
        }
    }

    if (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ROLAND
        && gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET
               == END_GAME_ROLAND_CAPTURE_SCENARIO) {
        enemyRemaining = false;
        for (player = 0; player < gpGame->m_playerCount; player++) {
            if (!gpGame->m_playerDead[player]
                && gpGame->m_players[player].m_color != PLAYER_COLOR_BLUE
                && gpGame->m_players[player].m_color != PLAYER_COLOR_YELLOW) {
                enemyRemaining = true;
            }
        }
        if (!enemyRemaining) {
            winFlag = true;
            if (!showedDialog_o && winFlag) {
                showedDialog_o = true;
                sprintf(gText, localization::Tr("victory.side_triumph"));
                NormalDialog(gText, 1);
            }
        }
    }

    if (defeated_m) {
        gbGameOver = true;
        giEndSequence = false;
    }
    if (winFlag) {
        gbGameOver = true;
        giEndSequence = true;
    }

    if (numAlive == 1 || survivingHumans_a == 0
        || (survivingHumans_a == 1 && !gbThisNetHumanPlayer[lastHuman_a])) {
        if (survivingHumans_a == 1 && gbThisNetHumanPlayer[lastHuman_a]) {
            if (allowNormalVictory) {
                gbGameOver = true;
                giEndSequence = true;
            }
        } else {
            gbGameOver = true;
            giEndSequence = false;
        }
    }

    if (savedRemoteOn_o && netHumanCount == 0) {
        gbGameOver = true;
        giEndSequence = false;
    }
    if (forcedResult == END_GAME_FORCE_VICTORY) {
        winFlag = true;
        gbGameOver = true;
        giEndSequence = true;
    }
    if (forcedResult == END_GAME_FORCE_DEFEAT) {
        defeated_m = true;
        gbGameOver = true;
        giEndSequence = false;
    }

    if (giEndSequence == 1 && gbGameOver) {
        winFlag = true;
    }
    if (giEndSequence == 0 && gbGameOver) {
        defeated_m = true;
    }

    if (gbInCampaign && winFlag) {
        currentDayIndex = (gpGame->m_month - 1) * CALENDAR_DAYS_PER_MONTH
                          + (gpGame->m_week - 1) * CALENDAR_DAYS_PER_WEEK + gpGame->m_day;
        gpGame->m_campaignScenarioWon = 1;
        gpGame
            ->m_campaignScenarioCompleted[IDX(gpGame->m_campaignType)][gpGame->m_campaignScenario] =
            1;
        gpGame->m_campaignScenarioDays[IDX(gpGame->m_campaignType)][gpGame->m_campaignScenario] =
            currentDayIndex;
        gpGame->m_campaignScore =
            gpGame->m_campaignScenarioBonus[IDX(gpGame->m_campaignType)][gpGame->m_campaignScenario]
            + gpGame->m_campaignScenarioDays[IDX(gpGame->m_campaignType)]
                                            [gpGame->m_campaignScenario];

        carryoverHeroId = END_GAME_NO_PLAYER;
        if (gpGame->m_campaignType == CAMPAIGN_ROLAND
            && gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET == END_GAME_SIDE_SCENARIO) {
            carryoverHeroId = CAMPAIGN_SWITCH_VICTORY_VALUE;
        }
        if (gpGame->m_campaignType == CAMPAIGN_ARCHIBALD
            && gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET
                   == END_GAME_FIRST_NO_SAVE_SCENARIO) {
            carryoverHeroId = CAMPAIGN_SWITCH_VICTORY_VALUE;
        }

        if (carryoverHeroId != static_cast<u32>(END_GAME_NO_PLAYER)) {
            for (player = 0; player < CAMPAIGN_ARMY_SLOT_COUNT; player++) {
                gpGame->m_campaignCarryoverCreatureTypes[player] = CREATURE_NONE;
                gpGame->m_campaignCarryoverCreatureCounts[player] = 0;
            }
            for (campaignHeroIndex = 0; campaignHeroIndex < gpGame->m_players[0].m_heroCount;
                 campaignHeroIndex++) {
                if (carryoverHeroId == CAMPAIGN_SWITCH_VICTORY_VALUE
                    || static_cast<u32>(IDX(
                           gpGame->m_heroRecs[gpGame->m_players[0].m_heroIds[campaignHeroIndex]]
                               .m_portrait
                       ))
                           == carryoverHeroId) {
                    break;
                }
            }
            if (campaignHeroIndex == gpGame->m_players[0].m_heroCount) {
                gpGame->m_campaignCarryoverCreatureTypes[0] = CREATURE_PEASANT;
                gpGame->m_campaignCarryoverCreatureCounts[0] = 1;
            } else {
                for (player = 0; player < CAMPAIGN_ARMY_SLOT_COUNT; player++) {
                    gpGame->m_campaignCarryoverCreatureTypes[player] =
                        gpGame->m_heroRecs[gpGame->m_players[0].m_heroIds[campaignHeroIndex]]
                            .m_army.m_creatureTypes[player];
                    gpGame->m_campaignCarryoverCreatureCounts[player] =
                        gpGame->m_heroRecs[gpGame->m_players[0].m_heroIds[campaignHeroIndex]]
                            .m_army.m_creatureCounts[player];
                }
            }
        }

        if (gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET != END_GAME_LAST_SCENARIO
            && (gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET
                    != END_GAME_FIRST_NO_SAVE_SCENARIO
                || gpGame->m_campaignType != CAMPAIGN_ROLAND)) {
            sprintf(
                campaignSaveName,
                "%s%c_%02d",
                localization::Tr("save.filename.victory_prefix"),
                gpGame->m_campaignType == CAMPAIGN_ROLAND ? 'G' : 'E',
                gpGame->m_campaignScenario + 1
            );
            gpGame->SaveGame(campaignSaveName, 1, 0);
        }
    } else if (xIsPlayingExpansionCampaign && winFlag) {
        xCampaign.Autosave();
    }

    bInCheckEndGame = false;
}

VA(0x0046aec9, 0x80)
void QuickViewWait(void) {
    tag_message event;
    b32 done;
    gpMouseManager->ReallyHidePointer();
    done = false;
    while (!done) {
        PollSound();
        Process1WindowsMessage();
        event = gpInputManager->GetEvent();
        done = event.type == MESSAGE_RIGHT_BUTTON_UP || event.type == MESSAGE_LEFT_BUTTON_DOWN
            || event.type == MESSAGE_LEFT_BUTTON_UP;
    }
    gpMouseManager->ReallyShowPointer();
}

VA(0x0046af49, 0x1cf)
void InitVars(void) {
    i32 i;
    i32 H2_UNUSED(j);
    gGameCommand = -1;
    gPalette = NULL;
    gbCombatSurrender = false;
    gpGame->m_viewArmyResult = 0;
    strcpy(gpGame->m_mapFilename, "brokena.mp2");
    gpGame->m_newGameInitialized = false;
    gbInNewGameSetup = false;
    strcpy(cNetBoxLine[0], "");
    strcpy(cNetBoxLine[1], "");
    strcpy(cNetBoxLine[2], "");
    strcpy(cNetBoxLine[3], "");
    cNetBoxColor[0] = BOX_DEFAULT_COLOR;
    cNetBoxColor[1] = BOX_DEFAULT_COLOR;
    cNetBoxColor[2] = BOX_DEFAULT_COLOR;
    cNetBoxColor[3] = BOX_DEFAULT_COLOR;
    ppMapExtra = NULL;
    pwSizeOfMapExtra = NULL;
    iMaxMapExtra = 0;
    for (i = 0; i < GLOBAL_TIMER_COUNT; i++)
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

VA(0x0046b118, 0x593)
void game::ShowMoraleInfo(hero* h, i32 dialogType) {
    b32 mixedUndead4;
    i32 alignment_e;
    ArmyGroupAlignmentResult homogeneous5;
    i32 modifierStart;
    char description7[MORALE_LUCK_DESCRIPTION_SIZE];
    i32 slot8;

    mixedUndead4 = false;
    if (h->m_army.GetMorale(h, h->GetOccupiedTown(), NULL) > 0)
        sprintf(description7, cMoraleInfo[IDX(MORALE_INFO_GOOD)]);
    else {
        if (h->m_army.GetMorale(h, h->GetOccupiedTown(), NULL) == 0)
            sprintf(description7, cMoraleInfo[IDX(MORALE_INFO_NEUTRAL)]);
        else
            sprintf(description7, cMoraleInfo[IDX(MORALE_INFO_BAD)]);
    }

    sprintf(gText, cMoraleInfo[IDX(MORALE_INFO_HEADER)], description7);
    modifierStart = strlen(gText);
    if (h->m_army.HasAllUndead()) {
        strcat(gText, cMoraleInfo[IDX(INFO_ALL_UNDEAD)]);
        goto showDialog;
    }
    if (h->m_army.HasSomeUndead() || h->HasArtifact(ARTIFACT_ARM_OF_MARTYR)) {
        strcat(gText, cMoraleInfo[IDX(INFO_SOME_UNDEAD)]);
        mixedUndead4 = true;
    }

    homogeneous5 = h->m_army.IsHomogeneous(-1);
    if (mixedUndead4 && homogeneous5 > ARMY_GROUP_ALIGNMENT_NO_MODIFIER) {
        homogeneous5 = ARMY_GROUP_ALIGNMENT_NO_MODIFIER;
    }
    if (homogeneous5 > ARMY_GROUP_ALIGNMENT_NO_MODIFIER) {
        alignment_e = 0;
        for (slot8 = 0; slot8 < ARMY_GROUP_SLOT_COUNT; slot8++) {
            if (h->m_army.m_creatureTypes[slot8] != CREATURE_NONE) {
                alignment_e = IDX(gMonsterDatabase[IDX(h->m_army.m_creatureTypes[slot8])].race);
            }
        }
        sprintf(description7, cMoraleInfo[IDX(INFO_SAME_ALIGNMENT)], gAlignmentNames[alignment_e]);
        strcat(gText, description7);
    }
    if (homogeneous5 == ARMY_GROUP_ALIGNMENT_THREE) {
        sprintf(description7, cMoraleInfo[IDX(INFO_THREE_ALIGNMENTS)]);
        strcat(gText, description7);
    }
    if (homogeneous5 == ARMY_GROUP_ALIGNMENT_FOUR) {
        sprintf(description7, cMoraleInfo[IDX(INFO_FOUR_ALIGNMENTS)]);
        strcat(gText, description7);
    }
    if (homogeneous5 == ARMY_GROUP_ALIGNMENT_FIVE_OR_MORE) {
        sprintf(description7, cMoraleInfo[IDX(INFO_FIVE_ALIGNMENTS)]);
        strcat(gText, description7);
    }

    if (h->GetOccupiedTown() != NULL && h->GetOccupiedTown()->m_type == FACTION_BARBARIAN
        && HAS(h->GetOccupiedTown()->m_buildings, IDX(TOWN_BUILDING_COLISEUM))) {
        strcat(gText, cMoraleInfo[IDX(INFO_COLISEUM)]);
    }
    if (h->GetOccupiedTown() != NULL
        && HAS(h->GetOccupiedTown()->m_buildings, IDX(TOWN_BUILDING_TAVERN))) {
        strcat(gText, cMoraleInfo[IDX(INFO_TAVERN)]);
    }

    if (h->HasArtifact(ARTIFACT_MEDAL_OF_VALOR)) {
        strcat(gText, cMoraleInfo[IDX(INFO_MEDAL_OF_VALOR)]);
    }
    if (h->HasArtifact(ARTIFACT_MEDAL_OF_COURAGE)) {
        strcat(gText, cMoraleInfo[IDX(INFO_MEDAL_OF_COURAGE)]);
    }
    if (h->HasArtifact(ARTIFACT_MEDAL_OF_HONOR)) {
        strcat(gText, cMoraleInfo[IDX(INFO_MEDAL_OF_HONOR)]);
    }
    if (h->HasArtifact(ARTIFACT_MEDAL_OF_DISTINCTION)) {
        strcat(gText, cMoraleInfo[IDX(INFO_MEDAL_OF_DISTINCTION)]);
    }
    if (h->HasArtifact(ARTIFACT_FIZBIN_OF_MISFORTUNE)) {
        strcat(gText, cMoraleInfo[IDX(INFO_FIZBIN)]);
    }
    if (HAS(h->m_eventFlags, HERO_EVENT_BUOY)) {
        strcat(gText, cMoraleInfo[IDX(INFO_BUOY)]);
    }
    if (HAS(h->m_eventFlags, HERO_EVENT_OASIS)) {
        strcat(gText, cMoraleInfo[IDX(INFO_OASIS)]);
    }
    if (HAS(h->m_eventFlags, HERO_EVENT_TEMPLE)) {
        strcat(gText, cMoraleInfo[IDX(INFO_TEMPLE)]);
    }
    if (HAS(h->m_eventFlags, HERO_EVENT_GRAVEYARD)) {
        strcat(gText, cMoraleInfo[IDX(INFO_GRAVEYARD)]);
    }
    if (HAS(h->m_eventFlags, HERO_EVENT_SHIPWRECK)) {
        strcat(gText, cMoraleInfo[IDX(INFO_SHIPWRECK)]);
    }
    if (HAS(h->m_eventFlags, HERO_EVENT_WATERING_HOLE)) {
        strcat(gText, cMoraleInfo[IDX(INFO_WATERING_HOLE)]);
    }
    if (HAS(h->m_eventFlags, HERO_EVENT_DERELICT_SHIP)) {
        strcat(gText, cMoraleInfo[IDX(INFO_DERELICT_SHIP)]);
    }
    if (h->m_secondarySkills[IDX(HERO_SKILL_LEADERSHIP)] == HERO_SKILL_LEVEL_BASIC) {
        strcat(gText, cMoraleInfo[IDX(INFO_BASIC_LEADERSHIP)]);
    }
    if (h->m_secondarySkills[IDX(HERO_SKILL_LEADERSHIP)] == HERO_SKILL_LEVEL_ADVANCED) {
        strcat(gText, cMoraleInfo[IDX(INFO_ADVANCED_LEADERSHIP)]);
    }
    if (h->m_secondarySkills[IDX(HERO_SKILL_LEADERSHIP)] == HERO_SKILL_LEVEL_EXPERT) {
        strcat(gText, cMoraleInfo[IDX(INFO_EXPERT_LEADERSHIP)]);
    }
    if (h->HasArtifact(ARTIFACT_MASTHEAD) && h->IsEmbarked()) {
        strcat(gText, cMoraleInfo[IDX(MORALE_INFO_MASTHEAD)]);
    }
    if (h->HasArtifact(ARTIFACT_BATTLE_GARB)) {
        strcat(gText, cMoraleInfo[IDX(MORALE_INFO_BATTLE_GARB)]);
    }
    if (modifierStart == static_cast<i32>(strlen(gText))) {
        strcat(gText, cMoraleInfo[IDX(MORALE_INFO_NONE)]);
    }

showDialog:
    NormalDialog(gText, dialogType);
}

VA(0x0046b6ab, 0x33e)
void game::ShowLuckInfo(hero* h, i32 dialogType) {
    char description4[MORALE_LUCK_DESCRIPTION_SIZE];
    i32 H2_UNUSED(luckValue);
    i32 modifierStart;

    if (gpGame->GetLuck(h, NULL, h->GetOccupiedTown()) > 0)
        sprintf(description4, cLuckInfo[IDX(LUCK_INFO_GOOD)]);
    else {
        if (gpGame->GetLuck(h, NULL, h->GetOccupiedTown()) == 0)
            sprintf(description4, cLuckInfo[IDX(LUCK_INFO_NEUTRAL)]);
        else
            sprintf(description4, cLuckInfo[IDX(LUCK_INFO_BAD)]);
    }

    sprintf(gText, cLuckInfo[IDX(LUCK_INFO_HEADER)], description4);
    modifierStart = strlen(gText);
    if (h->GetOccupiedTown() != NULL && h->GetOccupiedTown()->m_type == FACTION_SORCERESS
        && HAS(h->GetOccupiedTown()->m_buildings, IDX(TOWN_BUILDING_RAINBOW)))
        strcat(gText, cLuckInfo[IDX(INFO_RAINBOW)]);
    if (h->HasArtifact(ARTIFACT_RABBIT_FOOT))
        strcat(gText, cLuckInfo[IDX(INFO_RABBIT_FOOT)]);
    if (h->HasArtifact(ARTIFACT_GOLDEN_HORSESHOE))
        strcat(gText, cLuckInfo[IDX(INFO_HORSESHOE)]);
    if (h->HasArtifact(ARTIFACT_GAMBLERS_COIN))
        strcat(gText, cLuckInfo[IDX(INFO_LUCKY_COIN)]);
    if (h->HasArtifact(ARTIFACT_FOUR_LEAF_CLOVER))
        strcat(gText, cLuckInfo[IDX(INFO_CLOVER)]);
    if (HAS(h->m_eventFlags, HERO_EVENT_FAERIE_RING))
        strcat(gText, cLuckInfo[IDX(INFO_FAERIE_RING)]);
    if (HAS(h->m_eventFlags, HERO_EVENT_IDOL))
        strcat(gText, cLuckInfo[IDX(INFO_IDOL)]);
    if (HAS(h->m_eventFlags, HERO_EVENT_FOUNTAIN))
        strcat(gText, cLuckInfo[IDX(INFO_FOUNTAIN)]);
    if (HAS(h->m_eventFlags, HERO_EVENT_PYRAMID))
        strcat(gText, cLuckInfo[IDX(INFO_PYRAMID)]);
    if (h->m_secondarySkills[IDX(HERO_SKILL_LUCK)] == HERO_SKILL_LEVEL_BASIC)
        strcat(gText, cLuckInfo[IDX(INFO_BASIC_SKILL)]);
    if (h->m_secondarySkills[IDX(HERO_SKILL_LUCK)] == HERO_SKILL_LEVEL_ADVANCED)
        strcat(gText, cLuckInfo[IDX(INFO_ADVANCED_SKILL)]);
    if (h->m_secondarySkills[IDX(HERO_SKILL_LUCK)] == HERO_SKILL_LEVEL_EXPERT)
        strcat(gText, cLuckInfo[IDX(INFO_EXPERT_SKILL)]);
    if (h->HasArtifact(ARTIFACT_MASTHEAD) && h->IsEmbarked())
        strcat(gText, cLuckInfo[IDX(LUCK_INFO_MASTHEAD)]);
    if (HAS(h->m_eventFlags, HERO_EVENT_MERMAID))
        strcat(gText, cLuckInfo[IDX(INFO_MERMAID)]);
    if (h->HasArtifact(ARTIFACT_BATTLE_GARB))
        strcat(gText, cLuckInfo[IDX(LUCK_INFO_BATTLE_GARB)]);
    if (modifierStart == static_cast<i32>(strlen(gText)))
        strcat(gText, cLuckInfo[IDX(LUCK_INFO_NONE)]);

    NormalDialog(gText, dialogType);
}

VA(0x0046b9e9, 0xac)
void ClearMapExtra(void) {
    i32 i;
    for (i = 0; i < iMaxMapExtra; i++) {
        if (ppMapExtra[i])
            H2_FREE(ppMapExtra[i]);
    }
    if (ppMapExtra)
        H2_FREE(ppMapExtra);
    ppMapExtra = NULL;
    if (pwSizeOfMapExtra)
        H2_FREE(pwSizeOfMapExtra);
    pwSizeOfMapExtra = NULL;
    iMaxMapExtra = 0;
}

VA(0x0046ba95, 0x79)
i32 GetMonType(i32 score, HighScoreType highScoreType) {
    i32 idx;
    for (idx = IDX(CREATURE_COUNT) - 1; idx >= 0; idx--) {
        if (highScoreType == HIGH_SCORE_CAMPAIGN || highScoreType == HIGH_SCORE_EXPANSION_CAMPAIGN) {
            if (score <= giScoreCampaignMon[idx][IDX(MONSTER_SCORE_THRESHOLD)])
                return giScoreCampaignMon[idx][IDX(MONSTER_SCORE_TYPE)];
        } else {
            if (score >= giScoreMon[idx][IDX(MONSTER_SCORE_THRESHOLD)])
                return giScoreMon[idx][IDX(MONSTER_SCORE_TYPE)];
        }
    }
    return giScoreMon[0][IDX(MONSTER_SCORE_TYPE)];
}

#if !H2_STRICT_ENUMS
#define destinationIndex dest_o
#endif
VA(0x0046bb0e, 0x405)
i32 AddScoreToHighScore(
    i32 score,
    i32 days,
    i32 scenario,
    HighScoreType highScoreType,
    H2_CONST char* scenarioName
) {
    i32 destinationIndex;
    HighScoreEntry entries_a[HIGH_SCORE_ENTRY_COUNT];
    i32 file_c;
    i32 entry_a;
    char filename_h[HIGH_SCORE_FILENAME_LENGTH];
    char playerName_c[HIGH_SCORE_INPUT_NAME_SIZE];
    b32 missingFile_e;

    missingFile_e = false;
    if (highScoreType == HIGH_SCORE_STANDARD)
        sprintf(
            filename_h,
            "%sSTANDARD.HS",
            ".\\DATA\\"
        );
    else
        sprintf(
            filename_h,
            "%sCAMPAIGN.HS",
            ".\\DATA\\"
        );

    file_c = open(filename_h, HIGH_SCORE_FILE_READ_FLAGS);
    if (file_c == -1)
        missingFile_e = true;
    if (missingFile_e) {
        for (entry_a = 0; entry_a < HIGH_SCORE_ENTRY_COUNT; entry_a++) {
            memset(&entries_a[entry_a], 0, sizeof(HighScoreEntry));
            entries_a[entry_a].score = HIGH_SCORE_EMPTY;
        }
    } else {
        for (entry_a = 0; entry_a < HIGH_SCORE_ENTRY_COUNT; entry_a++)
            read(file_c, &entries_a[entry_a], sizeof(entries_a));
        close(file_c);
    }

    gbShowHighScore = true;
    giHighScoreType = highScoreType;
    giHighScoreRank = HIGH_SCORE_EMPTY;
    giScore = score;
    for (entry_a = 0; entry_a < HIGH_SCORE_ENTRY_COUNT; entry_a++) {
        if ((score >= entries_a[entry_a].score && highScoreType == HIGH_SCORE_STANDARD)
            || (score <= entries_a[entry_a].score && highScoreType == HIGH_SCORE_CAMPAIGN)
            || (score <= entries_a[entry_a].score && highScoreType == HIGH_SCORE_EXPANSION_CAMPAIGN)
            || entries_a[entry_a].score == HIGH_SCORE_EMPTY) {
            giHighScoreRank = entry_a;
            break;
        }
    }

    if (entry_a < HIGH_SCORE_ENTRY_COUNT) {
        for (destinationIndex = HIGH_SCORE_LAST_SHIFT_SOURCE; destinationIndex >= entry_a; destinationIndex--)
            entries_a[destinationIndex + 1] = entries_a[destinationIndex];

        GetDataEntry(
            localization::Tr("high_score.name_prompt"),
            playerName_c,
            HIGH_SCORE_NAME_LENGTH,
            NULL,
            0,
            1
        );
        memset(&entries_a[entry_a], 0, sizeof(HighScoreEntry));
        strcpy(entries_a[entry_a].playerName, playerName_c);
        strcpy(entries_a[entry_a].scenarioName, scenarioName);
        entries_a[entry_a].score = score;
        entries_a[entry_a].days = days;
        entries_a[entry_a].scenario = scenario;
        entries_a[entry_a].cheated = gpGame->m_cheated;
        if (highScoreType == HIGH_SCORE_CAMPAIGN && gpGame->m_campaignCheated)
            entries_a[entry_a].cheated = 1;

        file_c = open(filename_h, HIGH_SCORE_FILE_WRITE_FLAGS, HIGH_SCORE_FILE_PERMISSIONS);
        if (file_c == -1)
            FileError(filename_h);
        for (entry_a = 0; entry_a < HIGH_SCORE_ENTRY_COUNT; entry_a++)
            WRITE_FILE_VALUE(file_c, entries_a[entry_a]);
        close(file_c);
    } else {
        gbShowHighScore = false;
    }
    return 0;
}
#if !H2_STRICT_ENUMS
#undef destinationIndex
#endif

VA(0x0046bf13, 0x5e)
void BVResMsg(H2_CONST char* s, H2_ENUM_PARAM(ResourceType, i32) res, i32 qty) {
    giBottomViewOverride = BOTTOM_VIEW_RESOURCE;
    giBottomViewOverrideEndTime = KBTickCount() + BOTTOM_VIEW_RESOURCE_MESSAGE_DURATION;
    giBottomViewResource = res;
    giBottomViewResourceQty = qty;
    strcpy(gcBottomViewText, s);
    gpAdvManager->UpdBottomView(true, true, true);
}

VA(0x0046bf71, 0x1e)
void GOut(H2_CONST char* str) {
    if (gpAdvManager->m_active == 1)
        AiPrint(str);
}

VA(0x0046bf8f, 0x3e)
i32 NetPosToGamePos(i32 netPos) {
    for (i32 i = 0; i < GAME_PLAYER_COUNT; i++)
        if (gbGamePosToNetPos[i] == netPos)
            return i;
    return -1;
}

VA(0x0046bfcd, 0xd9)
i32 WaitForOtherPlayer(void) {
    i32 result = 0;
    KbRemotePacket* data;
    PollSound();
    data = reinterpret_cast<KbRemotePacket*>(GetRemoteData(1));
    if (data && data->type == REMOTE_MESSAGE_RELIABLE) {
        switch (data->command) {
            case BOX_REMOTE_SETUP:
                memcpy(
                    gbGamePosToNetPos,
                    data->payload.setup.gamePosToNetPos,
                    OLD_MAIN_PLAYER_COUNT
                );
                gbUseRegularCompression = data->payload.setup.useRegularCompression;
                gbUseDiffCompression = data->payload.setup.useDiffCompression;
                memcpy(
                    gsNetPlayerInfo,
                    data->payload.setup.players,
                    sizeof(data->payload.setup.players)
                );
                giThisGamePos = NetPosToGamePos(giThisNetPos);
                break;
            case BOX_REMOTE_SAVE:
                result = gpGame->ReceiveSaveGame(
                    data->payload.save.saveId,
                    data->payload.save.saveOffset,
                    data->payload.save.saveSize,
                    data->sender
                );
                break;
        }
    }
    return result;
}

VA(0x0046c0a6, 0xb25)
void PopNetBox(H2_CONST char* text, i32 netPlayer) {
    i32 H2_UNUSED(textY_h);
    i32l messageTime_b;
    heroWindow* netWindow_j;
    i32 result_p;
    i32 textWidth_b;
    i32 H2_UNUSED(textX_k);
    b32 savedShowIt_p;
    b32 updateInput_f;
    i32 inputLength_a;
    char inputText_b[BOX_TEXT_LENGTH];
    b32 exitForIncomingData_c;
    b32 sendText_b;
    tag_message event_o;
    tag_message updateMessage_i;
    i32 H2_UNUSED(firstLineId_a);
    i32 delay_e;
    i32 H2_UNUSED(lineTextLimit_g);
    b32 done_a;
    b32 redrawLines_l;
    b32 redrawSavedShowIt_a;
    KbRemotePacket* remoteData_g;
    b32 redrawAdventure_o;
    i32 cursorState_j;

    if (!gbRemoteOn)
        return;

    lineTextLimit_g = BOX_LINE_TEXT_LIMIT;
    firstLineId_a = BOX_FIRST_LINE_ID;
    textX_k = BOX_TEXT_X;
    textY_h = BOX_TEXT_Y;
    messageTime_b = 0;
    if (text != NULL) {
        if (netPlayer >= 0) {
            sprintf(gText, "%s:  %s", gsNetPlayerInfo[netPlayer].name, text);
            gText[BOX_LINE_TEXT_LIMIT] = 0;
            AddNetBoxLine(gText, gpGame->m_players[NetPosToGamePos(netPlayer)].m_color);
        } else {
            sprintf(gText, text);
            gText[BOX_LINE_TEXT_LIMIT] = 0;
            AddNetBoxLine(gText, BOX_DEFAULT_COLOR);
        }
        messageTime_b = KBTickCount();
    }

    inputLength_a = 0;
    savedShowIt_p = bShowIt;
    bShowIt = true;
    gbMoveShown = false;
    netWindow_j = new heroWindow(
        0,
        BOX_WINDOW_Y,
        "netbox.bin"
    );
    if (netWindow_j == NULL)
        MemError();

    SET_WIDGET_MESSAGE(updateMessage_i, NET_BOX_TEXT_COMMAND, BOX_FIRST_LINE_ID);
    updateMessage_i.payload.widget.data.text = cNetBoxLine[0];
    netWindow_j->BroadcastMessage(updateMessage_i);
    updateMessage_i.payload.widget.id = BOX_FIRST_LINE_ID + 1;
    updateMessage_i.payload.widget.data.text = cNetBoxLine[1];
    netWindow_j->BroadcastMessage(updateMessage_i);
    updateMessage_i.payload.widget.id = BOX_FIRST_LINE_ID + 2;
    updateMessage_i.payload.widget.data.text = cNetBoxLine[2];
    netWindow_j->BroadcastMessage(updateMessage_i);
    updateMessage_i.payload.widget.id = BOX_FIRST_LINE_ID + 3;
    updateMessage_i.payload.widget.data.text = cNetBoxLine[3];
    netWindow_j->BroadcastMessage(updateMessage_i);
    updateMessage_i.payload.widget.command = NET_BOX_COLOR_COMMAND;
    updateMessage_i.payload.widget.id = BOX_FIRST_COLOR_ID;
    updateMessage_i.payload.widget.data.value = cNetBoxColor[0] + BOX_COLOR_FRAME_OFFSET;
    netWindow_j->BroadcastMessage(updateMessage_i);
    updateMessage_i.payload.widget.id = BOX_FIRST_COLOR_ID + 1;
    updateMessage_i.payload.widget.data.value = cNetBoxColor[1] + BOX_COLOR_FRAME_OFFSET;
    netWindow_j->BroadcastMessage(updateMessage_i);
    updateMessage_i.payload.widget.id = BOX_FIRST_COLOR_ID + 2;
    updateMessage_i.payload.widget.data.value = cNetBoxColor[2] + BOX_COLOR_FRAME_OFFSET;
    netWindow_j->BroadcastMessage(updateMessage_i);
    updateMessage_i.payload.widget.id = BOX_FIRST_COLOR_ID + 3;
    updateMessage_i.payload.widget.data.value = cNetBoxColor[3] + BOX_COLOR_FRAME_OFFSET;
    netWindow_j->BroadcastMessage(updateMessage_i);
    updateMessage_i.payload.widget.id = BOX_THIS_PLAYER_COLOR_ID;
    updateMessage_i.payload.widget.data.value =
        gpGame->m_players[NetPosToGamePos(giThisNetPos)].m_color + BOX_COLOR_FRAME_OFFSET;
    netWindow_j->BroadcastMessage(updateMessage_i);

    gpWindowManager->AddWindow(netWindow_j, -1, 1);
    exitForIncomingData_c = false;
    done_a = false;
    updateInput_f = true;
    cursorState_j = 0;
    sendText_b = false;
    redrawLines_l = true;
    redrawAdventure_o = false;
    strcpy(inputText_b, "");
    gpInputManager->SetKeyCodeType(INPUT_KEY_CODE_ASCII);

    while (!done_a) {
        PollSound();
        remoteData_g = reinterpret_cast<KbRemotePacket*>(GetRemoteData(0));
        if (remoteData_g != NULL) {
            if (remoteData_g->type == REMOTE_MESSAGE_UNRELIABLE) {
                remoteData_g = reinterpret_cast<KbRemotePacket*>(GetRemoteData(1));
                switch (remoteData_g->command) {
                    case BOX_REMOTE_MAP_CHANGE:
                        gbLeaveNetBoxAlone = true;
                        if (gpAdvManager->m_active == 1) {
                            bShowIt = savedShowIt_p;
                            gpAdvManager->ProcessIncomingGroupMapChange(remoteData_g->payload.data);
                            bShowIt = true;
                            redrawAdventure_o = true;
                        }
                        gbLeaveNetBoxAlone = false;
                        updateInput_f = true;
                        break;
                }
            } else if (remoteData_g->type != REMOTE_MESSAGE_RELIABLE) {
                remoteData_g = reinterpret_cast<KbRemotePacket*>(GetRemoteData(1));
            } else {
                switch (remoteData_g->command) {
                    case BOX_REMOTE_CHAT:
                        remoteData_g = reinterpret_cast<KbRemotePacket*>(GetRemoteData(1));
                        sprintf(
                            gText,
                            "%s:  %s",
                            gsNetPlayerInfo[remoteData_g->sender].name,
                            remoteData_g->payload.data
                        );
                        AddNetBoxLine(
                            gText,
                            gpGame->m_players[NetPosToGamePos(remoteData_g->sender)].m_color
                        );
                        redrawLines_l = true;
                        if (messageTime_b != 0)
                            messageTime_b = KBTickCount();
                        break;
                    default:
                        AddNetBoxLine(
                            localization::Tr("network.incoming_data.must_exit"),
                            BOX_DEFAULT_COLOR
                        );
                        redrawLines_l = true;
                        exitForIncomingData_c = true;
                        break;
                }
            }
        }

        Process1WindowsMessage();
        event_o = gpInputManager->GetEvent();
        switch (event_o.type) {
            case MESSAGE_KEY_DOWN:
                messageTime_b = 0;
                switch (event_o.payload.keyboard.keyCode) {
                    case BOX_KEY_ESCAPE:
                    case BOX_KEY_F1:
                        done_a = true;
                        break;
                    case BOX_KEY_BACKSPACE:
                        if (inputLength_a > 0)
                            inputLength_a--;
                        updateInput_f = true;
                        cursorState_j = 1;
                        break;
                    case BOX_KEY_ENTER:
                        sendText_b = true;
                        break;
                    default:
                        if (event_o.payload.keyboard.keyByte < BOX_FIRST_PRINTABLE
                            || event_o.payload.keyboard.keyByte > BOX_LAST_PRINTABLE)
                            break;
                        if (inputLength_a < BOX_MAX_INPUT
                            && event_o.payload.keyboard.keyCode != 0) {
                            inputText_b[inputLength_a] = 0;
                            textWidth_b = smallFont->LineWidth(inputText_b);
                            if (textWidth_b + BOX_CURSOR_WIDTH_PADDING < BOX_CURSOR_WIDTH_LIMIT) {
                                inputText_b[inputLength_a] =
                                    static_cast<char>(event_o.payload.keyboard.keyCode & 0xff);
                                inputLength_a++;
                                updateInput_f = true;
                                cursorState_j = 0;
                            }
                        }
                }
        }

        if (!updateInput_f && glTimers[GLOBAL_NET_BOX_CURSOR_TIMER_SLOT] < KBTickCount()) {
            cursorState_j = 1 - cursorState_j;
            updateInput_f = true;
        }
        if (sendText_b) {
            sendText_b = false;
            inputText_b[inputLength_a] = 0;
            AddNetBoxLine(inputText_b, gpGame->m_players[NetPosToGamePos(giThisNetPos)].m_color);
            result_p = TransmitRemoteData(
                inputText_b,
                BOX_PACKET_BUFFER_SIZE,
                strlen(inputText_b) + 1,
                BOX_REMOTE_CHAT,
                1
            );
            if (!result_p)
                ShutDown(NULL);
            inputLength_a = 0;
            strcpy(inputText_b, "");
            updateInput_f = true;
            redrawLines_l = true;
        }

        if (redrawLines_l) {
            redrawLines_l = false;
            SET_WIDGET_MESSAGE(updateMessage_i, NET_BOX_TEXT_COMMAND, BOX_FIRST_LINE_ID);
            updateMessage_i.payload.widget.data.text = cNetBoxLine[0];
            netWindow_j->BroadcastMessage(updateMessage_i);
            updateMessage_i.payload.widget.id = BOX_FIRST_LINE_ID + 1;
            updateMessage_i.payload.widget.data.text = cNetBoxLine[1];
            netWindow_j->BroadcastMessage(updateMessage_i);
            updateMessage_i.payload.widget.id = BOX_FIRST_LINE_ID + 2;
            updateMessage_i.payload.widget.data.text = cNetBoxLine[2];
            netWindow_j->BroadcastMessage(updateMessage_i);
            updateMessage_i.payload.widget.id = BOX_FIRST_LINE_ID + 3;
            updateMessage_i.payload.widget.data.text = cNetBoxLine[3];
            netWindow_j->BroadcastMessage(updateMessage_i);
            updateMessage_i.payload.widget.command = NET_BOX_COLOR_COMMAND;
            updateMessage_i.payload.widget.id = BOX_FIRST_COLOR_ID;
            updateMessage_i.payload.widget.data.value = cNetBoxColor[0] + BOX_COLOR_FRAME_OFFSET;
            netWindow_j->BroadcastMessage(updateMessage_i);
            updateMessage_i.payload.widget.id = BOX_FIRST_COLOR_ID + 1;
            updateMessage_i.payload.widget.data.value = cNetBoxColor[1] + BOX_COLOR_FRAME_OFFSET;
            netWindow_j->BroadcastMessage(updateMessage_i);
            updateMessage_i.payload.widget.id = BOX_FIRST_COLOR_ID + 2;
            updateMessage_i.payload.widget.data.value = cNetBoxColor[2] + BOX_COLOR_FRAME_OFFSET;
            netWindow_j->BroadcastMessage(updateMessage_i);
            updateMessage_i.payload.widget.id = BOX_FIRST_COLOR_ID + 3;
            updateMessage_i.payload.widget.data.value = cNetBoxColor[3] + BOX_COLOR_FRAME_OFFSET;
            netWindow_j->BroadcastMessage(updateMessage_i);
            netWindow_j->DrawWindow();
            gpWindowManager->UpdateScreenRegion(0, BOX_WINDOW_Y, BOX_WIDTH, BOX_HEIGHT);
        }

        if (updateInput_f) {
            updateInput_f = false;
            glTimers[GLOBAL_NET_BOX_CURSOR_TIMER_SLOT] = KBTickCount() + BOX_CURSOR_DELAY;
            if (cursorState_j)
                inputText_b[inputLength_a] = '_';
            else
                inputText_b[inputLength_a] = BOX_CURSOR_GLYPH;
            inputText_b[inputLength_a + 1] = 0;
            SET_WIDGET_MESSAGE(updateMessage_i, NET_BOX_TEXT_COMMAND, BOX_INPUT_ID);
            updateMessage_i.payload.widget.data.text = inputText_b;
            netWindow_j->BroadcastMessage(updateMessage_i);
            netWindow_j->DrawWindow();
            gpWindowManager->UpdateScreenRegion(0, BOX_INPUT_Y, BOX_WIDTH, BOX_INPUT_HEIGHT);
        }

        if (messageTime_b != 0 && messageTime_b + BOX_MESSAGE_TIMEOUT < KBTickCount())
            done_a = true;
        if (exitForIncomingData_c) {
            for (delay_e = 0; delay_e < BOX_EXIT_DELAY_STEPS; delay_e++) {
                PollSound();
                DelayMilli(BOX_EXIT_DELAY);
            }
            done_a = true;
        }
    }

    gpInputManager->SetKeyCodeType(INPUT_KEY_CODE_SCAN);
    if (redrawAdventure_o && gbMoveShown) {
        gbDrawWindowBackground = false;
        gpWindowManager->RemoveWindow(netWindow_j);
        gbDrawWindowBackground = true;
        redrawSavedShowIt_a = bShowIt;
        bShowIt = true;
        gpAdvManager->RedrawAdvScreen(1, 0);
        bShowIt = redrawSavedShowIt_a;
    } else {
        gpWindowManager->RemoveWindow(netWindow_j);
    }
    bShowIt = savedShowIt_p;
}

VA(0x0046cbcb, 0x96)
void AddNetBoxLine(H2_CONST char* str, char color) {
    if (color < 0 || color > BOX_MAX_COLOR)
        color = BOX_DEFAULT_COLOR;

    strcpy(cNetBoxLine[0], cNetBoxLine[1]);
    strcpy(cNetBoxLine[1], cNetBoxLine[2]);
    strcpy(cNetBoxLine[2], cNetBoxLine[3]);
    strcpy(cNetBoxLine[BOX_LINE_COUNT - 1], str);

    cNetBoxColor[0] = cNetBoxColor[1];
    cNetBoxColor[1] = cNetBoxColor[2];
    cNetBoxColor[2] = cNetBoxColor[3];
    cNetBoxColor[BOX_LINE_COUNT - 1] = color;
}

VA(0x0046cc61, 0x1d9)
void ShutDown(H2_CONST char* message) {
    char buf[GLOBAL_TEXT_BUFFER_SIZE];
    if (bInShutDown)
        return;
    LogStr("Shutdown");
    bInShutDown = true;
    gbClosingApp = true;
    buf[0] = 0;
    gpMouseManager->SetColorMice(false);
    if (message) {
        strcpy(buf, message);
        SetFullScreenStatus(false);
        LogStr(buf);
        MessageBoxA(
            hwndApp,
            buf,
            localization::Tr("system.unexpected_termination"),
            MB_ICONHAND
        );
    } else {
        sprintf(buf, localization::Tr("system.goodbye"));
    }
    ShutDownSmacker();
    ClearMapExtra();
    UnloadSystemwideIcons();
    if (gbRemoteOn)
        HandleRemoteSuddenExit();
    if (gPalette) {
        gpResourceManager->Dispose((resource*)gPalette);
        gPalette = NULL;
    }
    if (bigFont) {
        gpResourceManager->Dispose((resource*)bigFont);
        bigFont = NULL;
    }
    if (smallFont) {
        gpResourceManager->Dispose((resource*)smallFont);
        smallFont = NULL;
    }
    RemoteCleanup();
    gpExec->ShutDownSystem();
    if (gEventHandle) {
        CloseHandle(gEventHandle);
        gEventHandle = NULL;
    }
    if (mapExtra)
        H2_FREE(mapExtra);
    mapExtra = NULL;
    CloseAIMapVars();
    DeleteMainClasses();
    AppExit();
    PrintMemoryLeaks();
    if (gpMemEntry)
        free(gpMemEntry);
    gpMemEntry = NULL;
    exit(0);
}

H2_ENUM_BEGIN(FileErrorConstant)
    FILE_ERROR_BUFFER_SIZE = 500
H2_ENUM_END(FileErrorConstant)

VA(0x0046ce3a, 0x99)
void FileError(H2_CONST char* filename) {
    char buf[FILE_ERROR_BUFFER_SIZE];
    i32 err;
    char buf1[FILE_ERROR_BUFFER_SIZE];
    err = errno;
    sprintf(buf1, "File Error %s", strerror(err));
    LogInt(buf1, err);
    sprintf(
        buf,
        localization::Tr("system.file.open_error"),
        filename
    );
    ShutDown(buf);
}

H2_ENUM_BEGIN(SmackFadeConstant)
    SMACK_FADE_FIRST_COLOR = WINGRAPH_SYSTEM_PALETTE_SIZE,
    SMACK_FADE_COLOR_LIMIT = WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE,
    SMACK_FADE_MATCH_COLOR_LIMIT = 36,
    SMACK_FADE_DISTANCE_SENTINEL = 999,
    SMACK_FADE_SCREEN_WIDTH = 640,
    SMACK_FADE_SCREEN_HEIGHT = 480,
    SMACK_FADE_BLUE_COMPONENT = 2,
    SMACK_FADE_GREEN_COMPONENT = 1,
    SMACK_FADE_RED_COMPONENT = 0
H2_ENUM_END(SmackFadeConstant)

VA(0x0046ced3, 0x236)
void SmackFade(u8* source, u8* destination) {
    u8* l;
    u8* g;
    i32 a;
    i32 f, h;
    i32 b, c;
    i32 k;
    i32 j;
    u8* i;
    i32 d, e;

    l = NULL;
    g = NULL;
    a = -1;
    l = static_cast<u8*>(H2_ALLOC(MISC_PALETTE_BYTE_COUNT));
    g = static_cast<u8*>(H2_ALLOC(WINGRAPH_PALETTE_SIZE));
    memset(l, 0, MISC_PALETTE_BYTE_COUNT);
    memset(g, 0, WINGRAPH_PALETTE_SIZE);
    for (f = SMACK_FADE_FIRST_COLOR; f < SMACK_FADE_COLOR_LIMIT; f++) {
        b = (source[f * MISC_PALETTE_COMPONENT_BYTES + SMACK_FADE_RED_COMPONENT]
             + source[f * MISC_PALETTE_COMPONENT_BYTES + SMACK_FADE_GREEN_COMPONENT]
             + source[f * MISC_PALETTE_COMPONENT_BYTES + SMACK_FADE_BLUE_COMPONENT])
            / MISC_PALETTE_COMPONENT_BYTES;
        k = SMACK_FADE_DISTANCE_SENTINEL;
        for (h = SMACK_FADE_FIRST_COLOR; h < SMACK_FADE_MATCH_COLOR_LIMIT; h++) {
            c = (destination[h * MISC_PALETTE_COMPONENT_BYTES + SMACK_FADE_RED_COMPONENT]
                 + destination[h * MISC_PALETTE_COMPONENT_BYTES + SMACK_FADE_GREEN_COMPONENT]
                 + destination[h * MISC_PALETTE_COMPONENT_BYTES + SMACK_FADE_BLUE_COMPONENT])
                / MISC_PALETTE_COMPONENT_BYTES;
            j = abs(b - c);
            if (j < k) {
                k = j;
                a = h;
            }
        }
        memcpy(
            l + f * MISC_PALETTE_COMPONENT_BYTES,
            destination + a * MISC_PALETTE_COMPONENT_BYTES,
            MISC_PALETTE_COMPONENT_BYTES
        );
        g[f] = (u8)a;
    }
    FadeTo(source, l, HIGH_SCORE_FADE_STEPS);
    i = gpWindowManager->m_screen->m_pixels;
    for (d = 0; d < SMACK_FADE_SCREEN_WIDTH; d++) {
        for (e = 0; e < SMACK_FADE_SCREEN_HEIGHT; e++) {
            *i = g[*i];
            i++;
        }
    }
    gpWindowManager->UpdateScreen();
    UpdatePalette(reinterpret_cast<i8*>(destination));
    H2_FREE(l);
    H2_FREE(g);
}

VA(0x0046d109, 0x3b2)
void ShowCongrats(HighScoreType highScoreType) {
    u8 palette[MISC_PALETTE_BYTE_COUNT];
    i32 H2_UNUSED(unused);
    i32 baseScore;
    i32 realScore;
    char ratingText[CONGRATS_RATING_LENGTH];

    gpMouseManager->HideColorPointer();
    memcpy(palette, gpBufferPalette->m_data, MISC_PALETTE_BYTE_COUNT);
    gpWindowManager->m_updateFlags = 0;
    congratsText = static_cast<char*>(H2_ALLOC(CONGRATS_TEXT_SIZE));
    baseScore = CalcBaseScore(giCurTurn);
    realScore = baseScore * gpGame->m_difficultyRating / CONGRATS_DIFFICULTY_SCALE;
    gpSoundManager->PlayAmbientMusic(MIDI_NO_TRACK);

    if (highScoreType == HIGH_SCORE_STANDARD) {
        sprintf(ratingText, gArmyNames[GetMonType(realScore, highScoreType)]);
    } else if (highScoreType == HIGH_SCORE_EXPANSION_CAMPAIGN) {
        sprintf(ratingText, gArmyNames[GetMonType(xCampaign.Days(), highScoreType)]);
    } else {
        sprintf(ratingText, gArmyNames[GetMonType(gpGame->m_campaignScore, highScoreType)]);
    }
    ratingText[0] = CyrillicToUpper(ratingText[0]);
    if (static_cast<i8>(gpGame->m_cheated))
        sprintf(ratingText, localization::Tr("high_score.cheater_rating"));

    if (highScoreType == HIGH_SCORE_STANDARD) {
        sprintf(
            congratsText,
            localization::Tr("high_score.congratulations.standard"),
            giCurTurn,
            baseScore,
            gpGame->m_difficultyRating,
            realScore,
            ratingText
        );
    } else if (highScoreType == HIGH_SCORE_EXPANSION_CAMPAIGN) {
        sprintf(
            congratsText,
            localization::Tr("high_score.congratulations.campaign"),
            xCampaign.Days(),
            ratingText
        );
    } else {
        sprintf(
            congratsText,
            localization::Tr("high_score.congratulations.campaign"),
            gpGame->m_campaignScore,
            ratingText
        );
    }

    PlaySmacker(CONGRATS_SMACKER);
    memcpy(gpBufferPalette->m_data, gPalette->m_data, MISC_PALETTE_BYTE_COUNT);
    SmackFade(reinterpret_cast<u8*>(gpBufferPalette->m_data), palette);
    memcpy(gPalette->m_data, palette, MISC_PALETTE_BYTE_COUNT);
    memcpy(gpBufferPalette->m_data, gPalette->m_data, MISC_PALETTE_BYTE_COUNT);
    gpMouseManager->ShowColorPointer();
    AddScoreToHighScore(
        realScore,
        giCurTurn,
        gpGame->m_difficultyRating,
        HIGH_SCORE_STANDARD,
        gpGame->m_mapHeader.name
    );
    H2_FREE(congratsText);
    congratsText = NULL;
    gpWindowManager->m_updateFlags = 1;
    memcpy(gpBufferPalette->m_data, gPalette->m_data, MISC_PALETTE_BYTE_COUNT);
}

#if !H2_STRICT_ENUMS
#define message msg
#endif
VA(0x0046d4bb, 0x79)
void CongratsWait(void) {
    i32 H2_UNUSED(command) = 0;
    b32 done = false;
    tag_message message;
    gpInputManager->Flush();
    while (!done) {
        PollSound();
        Process1WindowsMessage();
        message = gpInputManager->GetEvent();
        if (message.type == MESSAGE_KEY_DOWN || message.type == MESSAGE_LEFT_BUTTON_DOWN
            || message.type == MESSAGE_LEFT_BUTTON_UP || message.type == MESSAGE_RIGHT_BUTTON_DOWN
            || message.type == MESSAGE_RIGHT_BUTTON_UP)
            done = true;
    }
}
#if !H2_STRICT_ENUMS
#undef message
#endif

H2_ENUM_BEGIN(SamplePlaybackConstant)
    SAMPLE_PLAYBACK_CHANNEL_GROUP = 2,
    SAMPLE_DEFAULT_WAIT_TIME = 4000
H2_ENUM_END(SamplePlaybackConstant)

VA(0x0046d534, 0x41)
SAMPLE2 LoadPlaySample(H2_CONST char* name) {
    SAMPLE2 ss;
    ss = gpResourceManager->GetSample(name);
    if (ss) {
        ss->m_playbackData.channelType = SAMPLE_PLAYBACK_CHANNEL_GROUP;
        gpSoundManager->MemorySample(ss);
    }
    return ss;
}

VA(0x0046d575, 0x84)
void WaitEndSample(SAMPLE2* s, i32 waitTime) {
    i32l endTime;
    if (!s)
        return;
    if (!*s)
        return;
    if (waitTime < 0)
        waitTime = SAMPLE_DEFAULT_WAIT_TIME;
    endTime = KBTickCount() + waitTime;
    while (gpSoundManager->DigitalReport(*s) && KBTickCount() < endTime) {
        Process1WindowsMessage();
        PollSound();
    }
    gpResourceManager->Dispose((resource*)*s);
    *s = NULL;
}

H2_ENUM_BEGIN(MemoryErrorConstant)
    MEMORY_ERROR_REQUEST_SIZE = 6400
H2_ENUM_END(MemoryErrorConstant)

VA(0x0046d5f9, 0x4b)
void MemError(void) {
    if (gbInMemError)
        return;
    gbInMemError = true;
    LogStr("Out of Memory");
    sprintf(
        gText,
        cOutOfMemory,
        localization::Tr("system.memory.out_of_memory"),
        MEMORY_ERROR_REQUEST_SIZE
    );
    ShutDown(gText);
}

VA(0x0046d644, 0x29)
H2_CONST char* GetTownName(i32 i) {
    town* t = GetCastleRec(i);
    return t->m_name;
}

VA(0x0046d66d, 0x2f)
void LoadSystemwideIcons(void) {
    gBuyBuildIcons = gpResourceManager->GetIcon(
        "buybuild.icn"
    );
    gSystemIcons = gpResourceManager->GetIcon(
        "system.icn"
    );
}

VA(0x0046d69c, 0x28)
void UnloadSystemwideIcons(void) {
    gpResourceManager->Dispose((resource*)gBuyBuildIcons);
    gpResourceManager->Dispose((resource*)gSystemIcons);
}

VA(0x0046d6c4, 0x5)
void EarlyShutDownSystem(void) {}

VA(0x0046d6c9, 0x4c)
i32 GameUnsaved(void) {
    if ((gpAdvManager && gpAdvManager->m_active == 1)
        || (gpCombatManager && gpCombatManager->m_active == 1)
        || (gpTownManager && gpTownManager->m_active == 1))
        return 1;
    else
        return 0;
}

VA(0x0046d715, 0x8d0)
i32 HandleAppSpecificMenuCommands(i32 command) {
    b32 menuChanged;
    hero* currentHeroRec;
    i32 loopIndex;
    HeroSecondarySkill secondarySkillIndex;
    HeroSkillLevel ssLevel;
    i32 formationHexIndex;

    menuChanged = false;
    currentHeroRec = NULL;
    if (gpCurPlayer != NULL && gpCurPlayer->CurrentHero() != -1)
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
            strcpy(gText, localization::Tr("adventure.confirm.restart"));
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
            strcpy(
                gText,
                localization::Tr("game.confirm.load")
            );
        confirmMenuCommand:
            if (gpAdvManager->m_active == 1) {
                NormalDialog(gText, APP_MENU_CONFIRM_DIALOG);
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
            gConfig.musicVolume = CONFIG_VOLUME_MUTED;
            goto adjustMusic;
        case APP_MENU_MUSIC_FIRST + 1:
            gConfig.musicVolume = CONFIG_VOLUME_MIN;
            goto adjustMusic;
        case APP_MENU_MUSIC_FIRST + 2:
            gConfig.musicVolume = CONFIG_VOLUME_MIN + 1;
            goto adjustMusic;
        case APP_MENU_MUSIC_FIRST + 3:
            gConfig.musicVolume = CONFIG_VOLUME_MIN + 2;
            goto adjustMusic;
        case APP_MENU_MUSIC_FIRST + 4:
            gConfig.musicVolume = CONFIG_VOLUME_MIN + 3;
            goto adjustMusic;
        case APP_MENU_MUSIC_FIRST + 5:
            gConfig.musicVolume = CONFIG_VOLUME_MIN + 4;
            goto adjustMusic;
        case APP_MENU_MUSIC_FIRST + 6:
            gConfig.musicVolume = CONFIG_VOLUME_MIN + 5;
            goto adjustMusic;
        case APP_MENU_MUSIC_FIRST + 7:
            gConfig.musicVolume = CONFIG_VOLUME_MIN + 6;
            goto adjustMusic;
        case APP_MENU_MUSIC_FIRST + 8:
            gConfig.musicVolume = CONFIG_VOLUME_MIN + 7;
            goto adjustMusic;
        case APP_MENU_MUSIC_FIRST + 9:
            gConfig.musicVolume = CONFIG_VOLUME_MIN + 8;
            goto adjustMusic;
        case APP_MENU_MUSIC_LAST:
            gConfig.musicVolume = CONFIG_VOLUME_MAX;
            goto adjustMusic;
        adjustMusic:
            gpSoundManager->AdjustMusicVolumes();
            menuChanged = true;
            break;

        case APP_MENU_SOUND_FIRST:
            gConfig.soundVolume = CONFIG_VOLUME_MUTED;
            goto adjustSound;
        case APP_MENU_SOUND_FIRST + 1:
            gConfig.soundVolume = CONFIG_VOLUME_MIN;
            goto adjustSound;
        case APP_MENU_SOUND_FIRST + 2:
            gConfig.soundVolume = CONFIG_VOLUME_MIN + 1;
            goto adjustSound;
        case APP_MENU_SOUND_FIRST + 3:
            gConfig.soundVolume = CONFIG_VOLUME_MIN + 2;
            goto adjustSound;
        case APP_MENU_SOUND_FIRST + 4:
            gConfig.soundVolume = CONFIG_VOLUME_MIN + 3;
            goto adjustSound;
        case APP_MENU_SOUND_FIRST + 5:
            gConfig.soundVolume = CONFIG_VOLUME_MIN + 4;
            goto adjustSound;
        case APP_MENU_SOUND_FIRST + 6:
            gConfig.soundVolume = CONFIG_VOLUME_MIN + 5;
            goto adjustSound;
        case APP_MENU_SOUND_FIRST + 7:
            gConfig.soundVolume = CONFIG_VOLUME_MIN + 6;
            goto adjustSound;
        case APP_MENU_SOUND_FIRST + 8:
            gConfig.soundVolume = CONFIG_VOLUME_MIN + 7;
            goto adjustSound;
        case APP_MENU_SOUND_FIRST + 9:
            gConfig.soundVolume = CONFIG_VOLUME_MIN + 8;
            goto adjustSound;
        case APP_MENU_SOUND_LAST:
            gConfig.soundVolume = CONFIG_VOLUME_MAX;
            goto adjustSound;
        adjustSound:
            gpSoundManager->AdjustSoundVolumes();
            menuChanged = true;
            break;

        case APP_MENU_TOGGLE_ROUTE:
            gConfig.showRoute = 1 - gConfig.showRoute;
            menuChanged = true;
            break;
        case APP_MENU_TOGGLE_BLACKOUT:
            gConfig.blackoutComputer = 1 - gConfig.blackoutComputer;
            menuChanged = true;
            break;

        case APP_MENU_VIEW_WORLD:
            gpAdvManager->ViewWorld(SPELL_VIEW_ALL, false, false);
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
            gpGame->SetVisibility(
                APP_MENU_REVEAL_SIZE,
                APP_MENU_REVEAL_SIZE,
                giCurPlayer,
                APP_MENU_REVEAL_RADIUS
            );
            if (currentHeroRec != NULL)
                gpAdvManager->Reseed(0, 0);
            gpAdvManager->UpdateRadar(1, 0);
            gpAdvManager->CompleteDraw(0);
            gpAdvManager->UpdateScreen(0, 0);
            break;

        case APP_MENU_CHEAT_MOVEMENT:
            gpGame->m_cheated = 1;
            if (gbInCampaign)
                gpGame->m_campaignCheated = 1;
            if (currentHeroRec != NULL)
                currentHeroRec->m_remainingMobility = APP_MENU_MOVEMENT_BONUS;
            break;

        case APP_MENU_CHEAT_SPELLS:
            gpGame->m_cheated = 1;
            if (gbInCampaign)
                gpGame->m_campaignCheated = 1;
            if (currentHeroRec != NULL) {
                for (loopIndex = IDX(SPELL_FIREBALL); loopIndex < APP_MENU_MAX_SPELLS; loopIndex++)
                    currentHeroRec->AddSpell(
                        static_cast<SpellType>(loopIndex),
                        APP_MENU_SPELL_COUNT
                    );
                currentHeroRec->m_spellPoints = APP_MENU_CHEAT_SPELL_POINTS;
            }
            break;

        case APP_MENU_CHEAT_RESOURCES:
            gpGame->m_cheated = 1;
            if (gbInCampaign)
                gpGame->m_campaignCheated = 1;
            for (loopIndex = 0; loopIndex < APP_MENU_RESOURCE_COUNT; loopIndex++)
                gpCurPlayer->m_resources[loopIndex] += loopIndex == IDX(RES_GOLD)
                    ? APP_MENU_GOLD_BONUS
                    : APP_MENU_RESOURCE_BONUS;
            gpAdvManager->UpdBottomView(true, true, true);
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
                    gpGame->GiveArmy(
                        &gpGame->m_heroRecs[gpCurPlayer->CurrentHero()].m_army,
                        static_cast<CreatureType>(command - APP_MENU_ARMY_FIRST),
                        APP_MENU_CHEAT_ARMY_QUANTITY,
                        -1
                    );
                    gpAdvManager->UpdBottomView(true, true, true);
                }
                break;
            }
            if (command >= APP_MENU_SECONDARY_FIRST && command < APP_MENU_SECONDARY_LAST) {
                gpGame->m_cheated = 1;
                if (gbInCampaign)
                    gpGame->m_campaignCheated = 1;
                secondarySkillIndex = static_cast<HeroSecondarySkill>(
                    (command - APP_MENU_SECONDARY_FIRST) / APP_MENU_SECONDARY_LEVELS
                );
                ssLevel = static_cast<HeroSkillLevel>(
                    (command - APP_MENU_SECONDARY_FIRST) % APP_MENU_SECONDARY_LEVELS
                );
                if (currentHeroRec != NULL)
                    currentHeroRec->SetSS(secondarySkillIndex, ssLevel);
            }
            if (command >= APP_MENU_COMBAT_FIRST && command < APP_MENU_COMBAT_LAST) {
                gpCombatManager->m_debugFormation = command - APP_MENU_COMBAT_FIRST;
                gpCombatManager->m_backgroundDrawn = false;
                for (loopIndex = 0; loopIndex < APP_MENU_COMBAT_HEX_COUNT; loopIndex++) {
                    gpCombatManager->m_hexCells[loopIndex].m_blocked = 0;
                    gpCombatManager->m_hexCells[loopIndex].m_obstacleIndex = -1;
                }
                for (loopIndex = 0; loopIndex < APP_MENU_FORMATION_HEX_COUNT; loopIndex++) {
                    formationHexIndex =
                        sElevationOverlay[gpCombatManager->m_debugFormation].cellOffsets[loopIndex];
                    if (formationHexIndex != -1)
                        gpCombatManager->m_hexCells[formationHexIndex].m_blocked = 1;
                }
                gpCombatManager->SetupGridForArmy(
                    &gpCombatManager->m_armies[IDX(gpCombatManager->m_currentArmySide)]
                                              [gpCombatManager->m_currentArmyIndex]
                );
                gpCombatManager->DrawFrame(1, 0, 0, 0, 0, 1, 1);
            }
            return 1;
    }

    if (menuChanged)
        WritePrefs();
    return 0;
}

VA(0x0046dfe5, 0x28e)
void UpdateSystemOptionsMenu(void) {
    i32 menuCommand;
    i32 checkedCommand;

    if (CURRENT_GRAPHICS_CONFIG.showMenu == 0)
        return;
    if (hmnuApp == NULL)
        return;
    if (hmnuApp != hmnuAdv)
        return;

    for (menuCommand = APP_MENU_MUSIC_FIRST; menuCommand <= APP_MENU_MUSIC_LAST; menuCommand++)
        CheckMenuItem(hmnuApp, menuCommand, APP_MENU_UNCHECKED);
    switch (gConfig.musicVolume) {
        case CONFIG_VOLUME_MIN:
            checkedCommand = APP_MENU_MUSIC_FIRST + 1;
            break;
        case CONFIG_VOLUME_MIN + 1:
            checkedCommand = APP_MENU_MUSIC_FIRST + 2;
            break;
        case CONFIG_VOLUME_MIN + 2:
            checkedCommand = APP_MENU_MUSIC_FIRST + 3;
            break;
        case CONFIG_VOLUME_MIN + 3:
            checkedCommand = APP_MENU_MUSIC_FIRST + 4;
            break;
        case CONFIG_VOLUME_MIN + 4:
            checkedCommand = APP_MENU_MUSIC_FIRST + 5;
            break;
        case CONFIG_VOLUME_MIN + 5:
            checkedCommand = APP_MENU_MUSIC_FIRST + 6;
            break;
        case CONFIG_VOLUME_MIN + 6:
            checkedCommand = APP_MENU_MUSIC_FIRST + 7;
            break;
        case CONFIG_VOLUME_MIN + 7:
            checkedCommand = APP_MENU_MUSIC_FIRST + 8;
            break;
        case CONFIG_VOLUME_MIN + 8:
            checkedCommand = APP_MENU_MUSIC_FIRST + 9;
            break;
        case CONFIG_VOLUME_MAX:
            checkedCommand = APP_MENU_MUSIC_LAST;
            break;
        default:
            checkedCommand = APP_MENU_MUSIC_FIRST;
            break;
    }
    CheckMenuItem(hmnuApp, checkedCommand, APP_MENU_CHECKED);

    for (menuCommand = APP_MENU_SOUND_FIRST; menuCommand <= APP_MENU_SOUND_LAST; menuCommand++)
        CheckMenuItem(hmnuApp, menuCommand, APP_MENU_UNCHECKED);
    switch (gConfig.soundVolume) {
        case CONFIG_VOLUME_MIN:
            checkedCommand = APP_MENU_SOUND_FIRST + 1;
            break;
        case CONFIG_VOLUME_MIN + 1:
            checkedCommand = APP_MENU_SOUND_FIRST + 2;
            break;
        case CONFIG_VOLUME_MIN + 2:
            checkedCommand = APP_MENU_SOUND_FIRST + 3;
            break;
        case CONFIG_VOLUME_MIN + 3:
            checkedCommand = APP_MENU_SOUND_FIRST + 4;
            break;
        case CONFIG_VOLUME_MIN + 4:
            checkedCommand = APP_MENU_SOUND_FIRST + 5;
            break;
        case CONFIG_VOLUME_MIN + 5:
            checkedCommand = APP_MENU_SOUND_FIRST + 6;
            break;
        case CONFIG_VOLUME_MIN + 6:
            checkedCommand = APP_MENU_SOUND_FIRST + 7;
            break;
        case CONFIG_VOLUME_MIN + 7:
            checkedCommand = APP_MENU_SOUND_FIRST + 8;
            break;
        case CONFIG_VOLUME_MIN + 8:
            checkedCommand = APP_MENU_SOUND_FIRST + 9;
            break;
        case CONFIG_VOLUME_MAX:
            checkedCommand = APP_MENU_SOUND_LAST;
            break;
        default:
            checkedCommand = APP_MENU_SOUND_FIRST;
            break;
    }
    CheckMenuItem(hmnuApp, checkedCommand, APP_MENU_CHECKED);

    for (menuCommand = APP_MENU_SPEED_FIRST; menuCommand <= APP_MENU_SPEED_LAST; menuCommand++)
        CheckMenuItem(hmnuApp, menuCommand, APP_MENU_UNCHECKED);
    CheckMenuItem(
        hmnuApp,
        APP_MENU_TOGGLE_ROUTE,
        gConfig.showRoute ? APP_MENU_CHECKED : APP_MENU_UNCHECKED
    );
    CheckMenuItem(
        hmnuApp,
        APP_MENU_TOGGLE_BLACKOUT,
        1 - gConfig.blackoutComputer ? APP_MENU_CHECKED : APP_MENU_UNCHECKED
    );
}

VA(0x0046e273, 0x7d)
void CleanUpMenus(void) {
    if (hmnuApp) {
        SetMenu(hwndApp, NULL);
        if (hmnuAdv)
            DestroyMenu(hmnuAdv);
        if (hmnuDflt)
            DestroyMenu(hmnuDflt);
        if (hmnuCmbt)
            DestroyMenu(hmnuCmbt);
        if (hmnuTown)
            DestroyMenu(hmnuTown);
    }
    hmnuApp = NULL;
}

VA(0x0046e2f0, 0x1b)
void UpdateAppSpecificMenus(void* hMenu) {
    if (hMenu == hmnuAdv)
        UpdateSystemOptionsMenu();
}

VA(0x0046e30b, 0x12)
void EarlyResizeWindow(
    i32 H2_UNUSED(x),
    i32 H2_UNUSED(y),
    i32 H2_UNUSED(w),
    i32 H2_UNUSED(h)
) {
    if (gbClosingApp)
        return;
}

H2_ENUM_BEGIN(MapAreaConstant)
    MAP_AREA_ORIGIN = 16,
    MAP_AREA_LIMIT = 448
H2_ENUM_END(MapAreaConstant)

VA(0x0046e31d, 0x41)
i32 InMapArea(i32 x, i32 y) {
    return x >= MAP_AREA_ORIGIN && x < MAP_AREA_LIMIT && y >= MAP_AREA_ORIGIN && y < MAP_AREA_LIMIT;
}

H2_ENUM_BEGIN(DynamicWindowConstant)
    DYNAMIC_WINDOW_STONE = 0,
    TILE_SIZE = 48,
    WINDOW_PADDING = 48,
    CONTENT_LEFT = 32,
    CONTENT_TOP = 16,
    CORNER_SIZE = 72,
    CORNER_LEFT = 40,
    CORNER_RIGHT = 23,
    EDGE_OFFSET = 24,
    CORNER_FRAME_TOP_LEFT = 0,
    CORNER_FRAME_TOP_RIGHT = 1,
    CORNER_FRAME_BOTTOM_RIGHT = 2,
    CORNER_FRAME_BOTTOM_LEFT = 3,
    BACKGROUND_FRAME = 12,
    TOP_FRAME_FIRST = 4,
    TOP_FRAME_LAST = 5,
    RIGHT_FRAME_FIRST = 6,
    RIGHT_FRAME_LAST = 7,
    BOTTOM_FRAME_FIRST = 8,
    BOTTOM_FRAME_LAST = 9,
    LEFT_FRAME_FIRST = 10,
    LEFT_FRAME_LAST = 11,
H2_ENUM_END(DynamicWindowConstant)

VA(0x0046e35e, 0x829)
void SetupDynamicWindow(
    i32 x,
    i32 y,
    i32 centered,
    i32 boundsWidth,
    i32 boundsHeight,
    i32 contentWidth,
    i32 contentHeight,
    i32* windowWidth,
    i32* windowHeight,
    i32* contentLeft,
    i32* contentTop,
    i32* contentRight,
    i32* contentBottom,
    heroWindow** window,
    i32 windowType
) {
    i32 leftOffset_p;
    i32 H2_UNUSED(bottomCornerPaddingNum_j);
    i32 numRows;
    widget* newWidgetTemp_p;
    i32 columnsSize_h;
    i32 topOffsetNum_n;
    i32 H2_UNUSED(contentXPaddingCount_m);
    i32 centeredHeightCount_k;
    i32 H2_UNUSED(centeredPadding_c);
    i32 H2_UNUSED(topCornerPaddingCount);
    i32 bottomOffsetLocal_p;
    i32 rightOffset_p;
    i32 H2_UNUSED(contentYPadding);
    i32 edge_d;
    i32 tileRowPos_k;
    i32 centeredWidthValue_b;
    i32 H2_UNUSED(leftCornerPaddingLocal_e);
    i32 H2_UNUSED(rightCornerPaddingValue_j);
    i32 H2_UNUSED(stoneWidgetColorSize_c);
    i32 columnIndex_k;
    i32 H2_UNUSED(bottomEdgeOffset_l);
    i32 H2_UNUSED(tileWidth_k);
    i32 H2_UNUSED(tileHeight_h);
    i32 H2_UNUSED(topEdgeInset);

    tileWidth_k = TILE_SIZE;
    tileHeight_h = TILE_SIZE;
    topEdgeInset = -EDGE_OFFSET;
    bottomEdgeOffset_l = -EDGE_OFFSET;
    contentXPaddingCount_m = CONTENT_LEFT;
    contentYPadding = CONTENT_TOP;
    topCornerPaddingCount = CONTENT_TOP;
    bottomCornerPaddingNum_j = CONTENT_TOP;
    leftCornerPaddingLocal_e = CONTENT_TOP;
    rightCornerPaddingValue_j = CONTENT_TOP;
    centeredPadding_c = CONTENT_LEFT;
    stoneWidgetColorSize_c = CONTENT_TOP;
    newWidgetTemp_p = NULL;
    columnsSize_h = (contentWidth - 1) / TILE_SIZE + 1;
    numRows = (contentHeight - 1) / TILE_SIZE + 1;
    *windowWidth = columnsSize_h * TILE_SIZE + WINDOW_PADDING;
    *windowHeight = numRows * TILE_SIZE + WINDOW_PADDING;
    centeredWidthValue_b = columnsSize_h * TILE_SIZE + CONTENT_LEFT;
    centeredHeightCount_k = numRows * TILE_SIZE + CONTENT_LEFT;
    if (centered) {
        x += ((boundsWidth - centeredWidthValue_b) >> 1) - CONTENT_TOP;
        y += (boundsHeight - centeredHeightCount_k) >> 1;
    }
    *contentLeft = x + CONTENT_LEFT;
    *contentTop = y + CONTENT_TOP;
    *contentRight = *contentLeft + columnsSize_h * TILE_SIZE - 1;
    *contentBottom = *contentTop + numRows * TILE_SIZE - 1;

    if (windowType != DYNAMIC_WINDOW_STONE)
        return;
    *window = new heroWindow(
        x,
        y,
        *windowWidth,
        *windowHeight,
        WINDOW_FLAG_SAVE_BACKGROUND | WINDOW_FLAG_OWNS_WIDGETS
    );
    leftOffset_p = *contentLeft - x;
    topOffsetNum_n = *contentTop - y;
    rightOffset_p = *contentRight - x;
    bottomOffsetLocal_p = *contentBottom - y;

    for (tileRowPos_k = 0; tileRowPos_k < numRows; tileRowPos_k++) {
        for (columnIndex_k = 0; columnIndex_k < columnsSize_h; columnIndex_k++) {
            newWidgetTemp_p = new iconWidget(
                leftOffset_p + columnIndex_k * TILE_SIZE,
                topOffsetNum_n + tileRowPos_k * TILE_SIZE,
                TILE_SIZE,
                TILE_SIZE,
                DATA_COMPGEN(0x005152b8, setupDynamicWindowBackgroundTile, "stonebk2.icn"),
                BACKGROUND_FRAME,
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (newWidgetTemp_p == NULL)
                MemError();
            (*window)->AddWidget(newWidgetTemp_p, -1);
        }
    }

    newWidgetTemp_p = new iconWidget(
        leftOffset_p - CORNER_LEFT,
        topOffsetNum_n - CORNER_LEFT,
        CORNER_SIZE,
        CORNER_SIZE,
        DATA_COMPGEN(0x005152c8, setupDynamicWindowTopLeftCorner, "stonebk2.icn"),
        CORNER_FRAME_TOP_LEFT,
        ICON_DRAW_NORMAL,
        -1,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (newWidgetTemp_p == NULL)
        MemError();
    (*window)->AddWidget(newWidgetTemp_p, -1);

    newWidgetTemp_p = new iconWidget(
        rightOffset_p - CORNER_RIGHT,
        topOffsetNum_n - CORNER_LEFT,
        CORNER_SIZE,
        CORNER_SIZE,
        DATA_COMPGEN(0x005152d8, setupDynamicWindowTopRightCorner, "stonebk2.icn"),
        CORNER_FRAME_TOP_RIGHT,
        ICON_DRAW_NORMAL,
        -1,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (newWidgetTemp_p == NULL)
        MemError();
    (*window)->AddWidget(newWidgetTemp_p, -1);

    newWidgetTemp_p = new iconWidget(
        rightOffset_p - CORNER_RIGHT,
        bottomOffsetLocal_p - CORNER_RIGHT,
        CORNER_SIZE,
        CORNER_SIZE,
        DATA_COMPGEN(0x005152e8, setupDynamicWindowBottomRightCorner, "stonebk2.icn"),
        CORNER_FRAME_BOTTOM_RIGHT,
        ICON_DRAW_NORMAL,
        -1,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (newWidgetTemp_p == NULL)
        MemError();
    (*window)->AddWidget(newWidgetTemp_p, -1);

    newWidgetTemp_p = new iconWidget(
        leftOffset_p - CORNER_LEFT,
        bottomOffsetLocal_p - CORNER_RIGHT,
        CORNER_SIZE,
        CORNER_SIZE,
        DATA_COMPGEN(0x005152f8, setupDynamicWindowBottomLeftCorner, "stonebk2.icn"),
        CORNER_FRAME_BOTTOM_LEFT,
        ICON_DRAW_NORMAL,
        -1,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (newWidgetTemp_p == NULL)
        MemError();
    (*window)->AddWidget(newWidgetTemp_p, -1);

    for (edge_d = 0; edge_d < columnsSize_h; edge_d++) {
        newWidgetTemp_p = new iconWidget(
            leftOffset_p + edge_d * TILE_SIZE - EDGE_OFFSET,
            topOffsetNum_n - CORNER_LEFT,
            CORNER_SIZE,
            CORNER_SIZE,
            DATA_COMPGEN(0x00515308, setupDynamicWindowTopEdge, "stonebk2.icn"),
            Random(TOP_FRAME_FIRST, TOP_FRAME_LAST),
            ICON_DRAW_NORMAL,
            -1,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (newWidgetTemp_p == NULL)
            MemError();
        (*window)->AddWidget(newWidgetTemp_p, -1);

        newWidgetTemp_p = new iconWidget(
            leftOffset_p + edge_d * TILE_SIZE - EDGE_OFFSET,
            bottomOffsetLocal_p - CORNER_RIGHT,
            CORNER_SIZE,
            CORNER_SIZE,
            DATA_COMPGEN(0x00515318, setupDynamicWindowBottomEdge, "stonebk2.icn"),
            Random(BOTTOM_FRAME_FIRST, BOTTOM_FRAME_LAST),
            ICON_DRAW_NORMAL,
            -1,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (newWidgetTemp_p == NULL)
            MemError();
        (*window)->AddWidget(newWidgetTemp_p, -1);
    }

    for (edge_d = 0; edge_d < numRows; edge_d++) {
        newWidgetTemp_p = new iconWidget(
            leftOffset_p - CORNER_LEFT,
            topOffsetNum_n + edge_d * TILE_SIZE - EDGE_OFFSET,
            CORNER_SIZE,
            CORNER_SIZE,
            DATA_COMPGEN(0x00515328, setupDynamicWindowLeftEdge, "stonebk2.icn"),
            Random(LEFT_FRAME_FIRST, LEFT_FRAME_LAST),
            ICON_DRAW_NORMAL,
            -1,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (newWidgetTemp_p == NULL)
            MemError();
        (*window)->AddWidget(newWidgetTemp_p, -1);

        newWidgetTemp_p = new iconWidget(
            rightOffset_p - CORNER_RIGHT,
            topOffsetNum_n + edge_d * TILE_SIZE - EDGE_OFFSET,
            CORNER_SIZE,
            CORNER_SIZE,
            DATA_COMPGEN(0x00515338, setupDynamicWindowRightEdge, "stonebk2.icn"),
            Random(RIGHT_FRAME_FIRST, RIGHT_FRAME_LAST),
            ICON_DRAW_NORMAL,
            -1,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (newWidgetTemp_p == NULL)
            MemError();
        (*window)->AddWidget(newWidgetTemp_p, -1);
    }
}

VA(0x0046eb87, 0xd3)
void TestDynamicWindow(i32 p1, i32 p2) {
    heroWindow* d;
    i32 e, a, b, u, r, c;
    b32 t;
    SetupDynamicWindow(
        0,
        0,
        1,
        WINGRAPH_WIDTH,
        WINGRAPH_HEIGHT,
        p1 * TILE_SIZE,
        p2 * TILE_SIZE,
        &b,
        &u,
        &r,
        &c,
        &e,
        &a,
        &d,
        DYNAMIC_WINDOW_STONE
    );
    gpWindowManager->AddWindow(d, -1, 1);
    t = false;
    gpInputManager->Flush();
    while (!t) {
        Process1WindowsMessage();
        switch (gpInputManager->GetEvent().type) {
            case MESSAGE_KEY_DOWN:
            case MESSAGE_LEFT_BUTTON_DOWN:
            case MESSAGE_RIGHT_BUTTON_DOWN:
                t = true;
        }
    }
    gpWindowManager->RemoveWindow(d);
    delete d;
}

VA(0x0046ec5a, 0x88)
void HandleRemoteDeadPlayerExit(i32 pos) {
    SPlayerExit pe;
    if (pos == giThisGamePos) {
        if (!gpGame->TransmitSaveGame((giThisNetPos + 1) % giNumHumanPlayers, 1, 0))
            ShutDown(NULL);
        RemoteCleanup();
    } else {
        pe.netPosition = gbGamePosToNetPos[pos];
        pe.gamePosition = pos;
        pe.updateNetworkControl = false;
        pe.timedOut = false;
        pe.eliminated = true;
        pe.hostReported = false;
        ReceiveRemotePlayerExit(pe);
    }
}

H2_ENUM_BEGIN(PlayerExitNetPosition)
    PLAYER_EXIT_HOST_NET_POSITION = 0,
    PLAYER_EXIT_FIRST_GUEST_NET_POSITION = 1
H2_ENUM_END(PlayerExitNetPosition)

H2_ENUM_BEGIN(PlayerExitLocalConstant)
    PLAYER_EXIT_DIRECT_PLAYER_COUNT = 2,
    PLAYER_EXIT_MESSAGE_LENGTH = 500,
    PLAYER_EXIT_TRANSMIT_DELAY = 500,
    PLAYER_EXIT_MESSAGE_TIME = 5000,
    PLAYER_EXIT_HEARTBEAT_DISABLED = 0x0bebc1ff
H2_ENUM_END(PlayerExitLocalConstant)

VA(0x0046ece2, 0x8e)
void HandleRemoteSuddenExit(void) {
    SPlayerExit exitInfo;
    PlayerExitNetPosition destination;
    if (!gbGameInitialized)
        return;
    if (!gbRemoteOn)
        return;
    exitInfo.netPosition = static_cast<i8>(giThisNetPos);
    exitInfo.gamePosition = static_cast<i8>(giThisGamePos);
    exitInfo.updateNetworkControl = gbThisNetGotAdventureControl;
    exitInfo.timedOut = false;
    exitInfo.eliminated = false;
    if (giThisNetPos == PLAYER_EXIT_HOST_NET_POSITION)
        destination = PLAYER_EXIT_FIRST_GUEST_NET_POSITION;
    else
        destination = PLAYER_EXIT_HOST_NET_POSITION;
    LogStr("HRSE1");
    TransmitRemoteData(
        reinterpret_cast<char*>(&exitInfo),
        destination,
        sizeof(exitInfo),
        ADVMGR_REMOTE_COMMAND_PLAYER_EXIT,
        0,
        0,
        REMOTE_MESSAGE_RELIABLE
    );
    LogStr("HRSE2");
    DelayMilli(PLAYER_EXIT_TRANSMIT_DELAY);
}

VA(0x0046ed70, 0x4f)
void DropDownToOnePlayer(void) {
    RemoteCleanup();
    giNumHumanPlayers = 1;
    for (i32 i = 0; i < REMOTE_PLAYER_COUNT; i++)
        if (i != giThisNetPos)
            gbHumanPlayer[i] = false;
    ComputeAdvNetControl();
}

VA(0x0046edbf, 0x356)
void ReceiveHostReportsPlayerExit(i32 hostNetPosition, SPlayerExit exitInfo, i32 forwardedReport) {
    b32 showExitMessage_i;
    char playerExitMessage_k[PLAYER_EXIT_MESSAGE_LENGTH];
    i32 netPosition;

    showExitMessage_i = false;
    if (!forwardedReport) {
        if (exitInfo.eliminated) {
            if (exitInfo.netPosition == giThisNetPos) {
                RemoteCleanup();
                sprintf(gText, localization::Tr("player.eliminated"));
                NormalDialog(gText, NORMAL_DIALOG_INFO);
                gbGameOver = true;
                giEndSequence = false;
                return;
            }

            sprintf(gText, localization::Tr("player.vanquished"), gsNetPlayerInfo[exitInfo.netPosition].name);
            NormalDialog(
                gText,
                NORMAL_DIALOG_INFO,
                -1,
                -1,
                NORMAL_DIALOG_CREST,
                gpGame->m_players[exitInfo.gamePosition].m_color,
                -1,
                -1,
                -1,
                PLAYER_EXIT_MESSAGE_TIME
            );
        } else if (!exitInfo.continueGame) {
            gpGame->SaveGame(localization::Tr("save.filename.player_exit"), 1, 0);
            sprintf(
                gText,
                localization::Tr("network.player_exit.host_terminated.buka"),
                gsNetPlayerInfo[exitInfo.netPosition].name,
                gsNetPlayerInfo[hostNetPosition].name
            );
            NormalDialog(gText, NORMAL_DIALOG_CONFIRM);
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE) {
                DropDownToOnePlayer();
            } else {
                RemoteCleanup();
                ShutDown(NULL);
            }
        } else {
            if (exitInfo.timedOut) {
                sprintf(
                    playerExitMessage_k,
                    localization::Tr("network.player_exit.host_reports_timeout"),
                    gsNetPlayerInfo[hostNetPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name
                );
            } else {
                sprintf(
                    playerExitMessage_k,
                    localization::Tr("network.player_exit.host_reports_exit"),
                    gsNetPlayerInfo[hostNetPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name
                );
            }
            showExitMessage_i = true;
        }
    }

    if (giThisNetPos > exitInfo.netPosition)
        giThisNetPos--;
    gbHumanPlayer[exitInfo.gamePosition] = false;

    for (netPosition = exitInfo.netPosition; netPosition < REMOTE_PLAYER_COUNT - 1; netPosition++) {
        lLastHeartbeatReceive[netPosition] = lLastHeartbeatReceive[netPosition + 1];
        giNetPosToDCOPos[netPosition] = giNetPosToDCOPos[netPosition + 1];
        strcpy(gsNetPlayerInfo[netPosition].name, gsNetPlayerInfo[netPosition + 1].name);
    }

    for (netPosition = 0; netPosition < REMOTE_PLAYER_COUNT; netPosition++) {
        if (gbGamePosToNetPos[netPosition] == exitInfo.netPosition)
            gbGamePosToNetPos[netPosition] = -1;
        else if (gbGamePosToNetPos[netPosition] > exitInfo.netPosition)
            gbGamePosToNetPos[netPosition]--;
    }

    giNumHumanPlayers--;
    iLastDiffSendTo = DIFF_SEND_FORCE_WHOLE;
    if (exitInfo.updateNetworkControl)
        ComputeAdvNetControl();

    if (showExitMessage_i)
        NormalDialog(
            playerExitMessage_k,
            NORMAL_DIALOG_INFO,
            -1,
            -1,
            -1,
            -1,
            -1,
            -1,
            -1,
            PLAYER_EXIT_MESSAGE_TIME
        );
}

VA(0x0046f115, 0x2c9)
void ReceiveRemotePlayerExit(SPlayerExit exitInfo) {
    b32 localPlayerLost_e;
    i32 H2_UNUSED(sendReturn);
    i32 H2_UNUSED(unusedPacketResult_g);
    i32 recipient;

    localPlayerLost_e = false;
    lLastHeartbeatReceive[exitInfo.netPosition] = PLAYER_EXIT_HEARTBEAT_DISABLED;
    gpGame->SaveGame(localization::Tr("save.filename.player_exit"), 1, 0);

    if (exitInfo.eliminated) {
        exitInfo.continueGame = true;
        if (exitInfo.netPosition == giThisNetPos) {
            localPlayerLost_e = true;
            goto exitInfoProcessed;
        }
        sprintf(gText, localization::Tr("player.vanquished"), gsNetPlayerInfo[exitInfo.netPosition].name);
        NormalDialog(
            gText,
            NORMAL_DIALOG_INFO,
            -1,
            -1,
            NORMAL_DIALOG_CREST,
            gpGame->m_players[exitInfo.gamePosition].m_color,
            -1,
            -1,
            -1,
            PLAYER_EXIT_MESSAGE_TIME
        );
        exitInfo.continueGame = true;
    } else {
        if (exitInfo.timedOut) {
            sprintf(
                gText,
                localization::Tr("network.player_exit.timed_out_confirm.buka"),
                gsNetPlayerInfo[exitInfo.netPosition].name,
                gsNetPlayerInfo[exitInfo.netPosition].name
            );
        } else {
            sprintf(
                gText,
                localization::Tr("network.player_exit.exiting_confirm.buka"),
                gsNetPlayerInfo[exitInfo.netPosition].name,
                gsNetPlayerInfo[exitInfo.netPosition].name
            );
        }
        NormalDialog(gText, NORMAL_DIALOG_CONFIRM);
        exitInfo.continueGame = gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE;
    }

exitInfoProcessed:
    if (giNumHumanPlayers == PLAYER_EXIT_DIRECT_PLAYER_COUNT) {
        if (exitInfo.eliminated && !exitInfo.hostReported) {
            sendReturn = TransmitRemoteData(
                reinterpret_cast<char*>(&exitInfo),
                1 - giThisNetPos,
                sizeof(exitInfo),
                ADVMGR_REMOTE_COMMAND_HOST_PLAYER_EXIT,
                1
            );
        }
        if (localPlayerLost_e)
            goto playerExitHandled;
        giNumHumanPlayers--;
        gbHumanPlayer[exitInfo.gamePosition] = false;
        RemoteCleanup();
        ComputeAdvNetControl();
    } else {
        for (recipient = 0; recipient < REMOTE_PLAYER_COUNT; recipient++) {
            if ((recipient == exitInfo.netPosition && exitInfo.eliminated && !exitInfo.hostReported)
                || (recipient != exitInfo.netPosition && recipient < giNumHumanPlayers
                    && recipient != giThisNetPos)) {
                sendReturn = TransmitRemoteData(
                    reinterpret_cast<char*>(&exitInfo),
                    recipient,
                    sizeof(exitInfo),
                    ADVMGR_REMOTE_COMMAND_HOST_PLAYER_EXIT,
                    1
                );
            }
        }
        if (localPlayerLost_e)
            goto playerExitHandled;
        ReceiveHostReportsPlayerExit(0, exitInfo, 1);
    }

playerExitHandled:
    if (localPlayerLost_e) {
        sprintf(gText, localization::Tr("player.eliminated"));
        RemoteCleanup();
        NormalDialog(gText, NORMAL_DIALOG_INFO);
        gbGameOver = true;
        giEndSequence = false;
        return;
    }
    if (!exitInfo.continueGame) {
        ShutDown(NULL);
    }
}

H2_ENUM_BEGIN(CheckMemoryConstant)
    CHECK_MEMORY_INITIAL_AVAILABLE_KB = 5,
    CHECK_MEMORY_TOTAL_KB = 16000,
    CHECK_MEMORY_AVAILABLE_KB = 8000
H2_ENUM_END(CheckMemoryConstant)

VA(0x0046f3de, 0x1e)
i32 CheckMem(void) {
    giTotalHighMem = CHECK_MEMORY_TOTAL_KB;
    giHighMemBuffer = CHECK_MEMORY_AVAILABLE_KB;
    return 1;
}

VA(0x0046f3fc, 0xce)
i32 GetManaCost(SpellType spell, hero* h) {
    i32 c = gsSpellInfo[IDX(spell)].cost;
    if (h != NULL) {
        if (h->HasArtifact(ARTIFACT_EVIL_EYE)
            && (spell == SPELL_CURSE || spell == SPELL_MASS_CURSE))
            c >>= 1;
        if (h->HasArtifact(ARTIFACT_SKULLCAP)
            && (spell == SPELL_BERSERKER || spell == SPELL_HYPNOTIZE || spell == SPELL_PARALYZE
                || spell == SPELL_BLIND))
            c >>= 1;
        if (h->HasArtifact(ARTIFACT_SNAKE_RING)
            && (spell == SPELL_BLESS || spell == SPELL_MASS_BLESS))
            c >>= 1;
        if (h->HasArtifact(ARTIFACT_ELEMENTAL_RING)
            && (spell == SPELL_SUMMON_EARTH_ELEMENTAL || spell == SPELL_SUMMON_AIR_ELEMENTAL
                || spell == SPELL_SUMMON_FIRE_ELEMENTAL || spell == SPELL_SUMMON_WATER_ELEMENTAL))
            c >>= 1;
    }
    return c;
}

#if !H2_STRICT_ENUMS
#define message msg
#endif
VA(0x0046f4ca, 0x88)
void SetWinText(heroWindow* j, i32 id) {
    i32 H2_UNUSED(a) = 0;
    i32 i;
    tag_message message;
    for (i = 0; i < KB_WIN_SETUP_COUNT; i++) {
        if (gWinSetup[i].windowId == id) {
            a++;
            SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, gWinSetup[i].widgetId);
            message.payload.widget.data.text = gWinSetup[i].text;
            j->BroadcastMessage(message);
        }
    }
}
#if !H2_STRICT_ENUMS
#undef message
#endif

H2_ENUM_BEGIN(ShingleAnimationConstant)
    SHINGLE_ANIMATION_INTERVAL = 250,
    SHINGLE_ANIMATION_FRAME_COUNT = 39,
    SHINGLE_ANIMATION_FRAME_OFFSET = 1,
    SHINGLE_UPDATE_X = 46,
    SHINGLE_UPDATE_Y = 176,
    SHINGLE_UPDATE_WIDTH = 139,
    SHINGLE_UPDATE_HEIGHT = 187
H2_ENUM_END(ShingleAnimationConstant)

VA(0x0046f552, 0x79)
void CheckShingleUpdate(void) {
    if (!gShingleAnim)
        return;
    if (KBTickCount() > iNextShingleAnim) {
        iNextShingleAnim = KBTickCount() + SHINGLE_ANIMATION_INTERVAL;
        iShingleAnimFrame =
            (iShingleAnimFrame + SHINGLE_ANIMATION_FRAME_OFFSET) % SHINGLE_ANIMATION_FRAME_COUNT;
        gShingleAnim->DrawToBuffer(
            0,
            0,
            iShingleAnimFrame + SHINGLE_ANIMATION_FRAME_OFFSET,
            ICON_DRAW_NORMAL
        );
        gpWindowManager->UpdateScreenRegion(
            SHINGLE_UPDATE_X,
            SHINGLE_UPDATE_Y,
            SHINGLE_UPDATE_WIDTH,
            SHINGLE_UPDATE_HEIGHT
        );
    }
}

H2_ENUM_BEGIN(NormalDialogLayoutConstant)
    NORMAL_DIALOG_PRIMARY_BONUS_TEXT_LENGTH = 5,
    NORMAL_DIALOG_OR_TEXT_LENGTH = 4,
    NORMAL_DIALOG_RESOURCE_LABEL_HEIGHT = 12,
    NORMAL_DIALOG_SPELL_LABEL_HEIGHT = 24,
    NORMAL_DIALOG_RESOURCE_LABEL_WIDTH = 100,
    NORMAL_DIALOG_RESOURCE_LABEL_HALF_WIDTH = NORMAL_DIALOG_RESOURCE_LABEL_WIDTH / 2,
    NORMAL_DIALOG_PRIMARY_BONUS_TEXT_HEIGHT = 16,
    NORMAL_DIALOG_OR_TEXT_WIDTH = 40,
    NORMAL_DIALOG_OR_TEXT_HEIGHT = 12,
    NORMAL_DIALOG_RESOURCE_LAYOUT_HEIGHT = 44,
    NORMAL_DIALOG_RESOURCE_ICON_WIDTH = 38,
    NORMAL_DIALOG_RESOURCE_ICON_HEIGHT = 32,
    NORMAL_DIALOG_GOLD_LAYOUT_HEIGHT = 26,
    NORMAL_DIALOG_LARGE_ICON_WIDTH = 76,
    NORMAL_DIALOG_ARTIFACT_ICON_HEIGHT = 76,
    NORMAL_DIALOG_SPELL_LAYOUT_HEIGHT = 79,
    NORMAL_DIALOG_SPELL_ICON_WIDTH = 70,
    NORMAL_DIALOG_SPELL_ICON_HEIGHT = 55,
    NORMAL_DIALOG_CREST_ICON_WIDTH = 50,
    NORMAL_DIALOG_CREST_ICON_HEIGHT = 55,
    NORMAL_DIALOG_EXPMRL_ICON_WIDTH = 64,
    NORMAL_DIALOG_LUCK_BONUS_ICON_HEIGHT = 28,
    NORMAL_DIALOG_LUCK_PENALTY_ICON_HEIGHT = 57,
    NORMAL_DIALOG_MORALE_BONUS_ICON_HEIGHT = 62,
    NORMAL_DIALOG_MORALE_PENALTY_ICON_HEIGHT = 59,
    NORMAL_DIALOG_EXPERIENCE_ICON_HEIGHT = 64,
    NORMAL_DIALOG_HERO_LAYOUT_HEIGHT = 111,
    NORMAL_DIALOG_HERO_BACKGROUND_WIDTH = 111,
    NORMAL_DIALOG_HERO_BACKGROUND_HEIGHT = 105,
    NORMAL_DIALOG_SECONDARY_SKILL_LAYOUT_HEIGHT = 81,
    NORMAL_DIALOG_SECONDARY_SKILL_BACKGROUND_WIDTH = 75,
    NORMAL_DIALOG_SECONDARY_SKILL_ICON_HEIGHT = 65,
    NORMAL_DIALOG_PRIMARY_MONSTER_LAYOUT_HEIGHT = 105,
    NORMAL_DIALOG_PRIMARY_MONSTER_BACKGROUND_WIDTH = 94,
    NORMAL_DIALOG_PRIMARY_MONSTER_ICON_HEIGHT = 105,
    NORMAL_DIALOG_PRIMARY_MONSTER_OVERLAY_WIDTH = 82,
    NORMAL_DIALOG_PRIMARY_MONSTER_OVERLAY_HEIGHT = 93,
    NORMAL_DIALOG_CREST_OVERLAY_WIDTH = 58,
    NORMAL_DIALOG_CREST_OVERLAY_HEIGHT = 55,
    NORMAL_DIALOG_SECONDARY_SKILL_OVERLAY_WIDTH = 71,
    NORMAL_DIALOG_SECONDARY_SKILL_OVERLAY_HEIGHT = 81,
    NORMAL_DIALOG_HERO_OVERLAY_WIDTH = 101,
    NORMAL_DIALOG_HERO_OVERLAY_HEIGHT = 95,
    NORMAL_DIALOG_BUTTON_AREA_HEIGHT = 39,
    NORMAL_DIALOG_RESOURCE_VERTICAL_GAP = 14,
    NORMAL_DIALOG_ROW_CALCULATION_OFFSET = 25,
    NORMAL_DIALOG_DEFAULT_X = 159,
    NORMAL_DIALOG_SINGLE_RESOURCE_CENTER_INSET = 17,
    NORMAL_DIALOG_FIRST_RESOURCE_CENTER_X = 104,
    NORMAL_DIALOG_SECOND_RESOURCE_RIGHT_INSET = 87,
    NORMAL_DIALOG_RESOURCE_BOTTOM_INSET = 48,
    NORMAL_DIALOG_SECONDARY_PAIR_SPACING = 4,
    NORMAL_DIALOG_ICON_OVERLAY_INSET = 6,
    NORMAL_DIALOG_SPELL_BACKGROUND_X_OFFSET = 2,
    NORMAL_DIALOG_CREST_OVERLAY_OUTSET = 4,
    NORMAL_DIALOG_SECONDARY_SKILL_OVERLAY_OUTSET = 3,
    NORMAL_DIALOG_HERO_OVERLAY_INSET = 5,
    NORMAL_DIALOG_SECONDARY_NAME_Y_OFFSET = 72,
    NORMAL_DIALOG_SECONDARY_LEVEL_Y_OFFSET = 24,
    NORMAL_DIALOG_PRIMARY_LABEL_Y_OFFSET = 93,
    NORMAL_DIALOG_DEFAULT_LABEL_Y_OFFSET = 10,
    NORMAL_DIALOG_PRIMARY_BONUS_LABEL_Y_OFFSET = 22,
    NORMAL_DIALOG_OR_TEXT_CENTER_X_OFFSET = 10,
    NORMAL_DIALOG_OR_TEXT_Y_OFFSET = 43,
    NORMAL_DIALOG_CENTER_PART_COUNT = 2,
    NORMAL_DIALOG_PRIMARY_BACKGROUND_FRAME = 4,
    NORMAL_DIALOG_MONSTER_BACKGROUND_FRAME = 12,
    NORMAL_DIALOG_SECONDARY_BACKGROUND_FRAME_BASE = 1,
    NORMAL_DIALOG_HERO_BACKGROUND_FRAME = 4,
    NORMAL_DIALOG_ARTIFACT_FRAME_OFFSET = 1,
    NORMAL_DIALOG_MONSTER_RACE_FRAME_OFFSET = 4,
    NORMAL_DIALOG_CREST_OVERLAY_FRAME = 6,
    NORMAL_DIALOG_SECONDARY_OVERLAY_FRAME = 15
H2_ENUM_END(NormalDialogLayoutConstant)

inline i32 NormalDialogCenterOffset(i32 extent) {
    return extent / NORMAL_DIALOG_CENTER_PART_COUNT;
}

VA(0x0046f5cb, 0x1757)
void NormalDialog(
    H2_CONST char* text,
    i32 dialogType,
    i32 windowX,
    i32 windowY,
    i32 firstResourceType,
    i32 firstResourceValue,
    i32 secondResourceType,
    i32 secondResourceValue,
    i32 showOrText,
    i32 timeout
) {
    i32 imageHeight_p;
    i32 labelY_k;
    widget* borderWidget_k;
    i32 resourceFrame_n;
    i16 H2_UNUSED(showMessage_d);
    i32 textWidgetId;
    heroWindow* savedNormalDialogWindow;
    i32 savedPointerFrame;
    i32 windowHeight_h;
    char* orText;
    b32 showPrimaryBonus;
    tag_message message_b;
    i32 savedSecondResourceValue_n;
    i32 savedFirstResourceValue;
    widget* textPanel_j;
    i32 resourceSlot;
    i32 resourceY_f;
    i32 H2_UNUSED(iconHeight_h);
    i32 lineCount;
    i32 dialogContentHeight;
    i32 resourceCenterX_c;
    i32 resourceImageWidth;
    i32 sizingIconHeight;
    i32 savedFirstResourceType_k;
    i32 maxIconHeight;
    i32 savedSecondResourceType_m;
    i32 windowRows_b;
    char iconFile_a[NORMAL_DIALOG_FILENAME_LENGTH];
    i32 resourceValue_c[NORMAL_DIALOG_RESOURCE_COUNT];
    i32 windowWidth_f;
    char* resourceText_p[NORMAL_DIALOG_RESOURCE_COUNT];
    i32 resourceType_a[NORMAL_DIALOG_RESOURCE_COUNT];
    MouseCursorType savedPointerType_o;
    widget* iconPanel_a;
    i32 H2_UNUSED(panelHeight_d);

    if (!gbRemoteOn)
        timeout = 0;
    if (timeout > NORMAL_DIALOG_TIMEOUT_MIN && timeout < NORMAL_DIALOG_TIMEOUT_MAX) {
        giDialogTimeout = KBTickCount() + timeout;
    } else {
        giDialogTimeout = timeout;
    }

    resourceCenterX_c = 0;
    resourceY_f = 0;
    resourceFrame_n = 0;
    textWidgetId = NORMAL_DIALOG_TEXT_WIDGET_FIRST_ID;
    resourceImageWidth = 0;
    iconHeight_h = 0;
    showPrimaryBonus = false;
    showMessage_d = 1;

    if (firstResourceType == NORMAL_DIALOG_PRIMARY_SKILL
        && firstResourceValue >= NORMAL_DIALOG_PRIMARY_BONUS_OFFSET) {
        firstResourceValue -= NORMAL_DIALOG_PRIMARY_BONUS_OFFSET;
        showPrimaryBonus = true;
    }
    if (firstResourceType >= NORMAL_DIALOG_MONSTER + 1
        && firstResourceType <= NORMAL_DIALOG_PRIMARY_SKILL - 1) {
        firstResourceType = NORMAL_DIALOG_NO_RESOURCE;
    }

    savedNormalDialogWindow = pNormalDialogWindow;
    savedFirstResourceType_k = giResType1;
    savedFirstResourceValue = giResExtra1;
    savedSecondResourceType_m = giResType2;
    savedSecondResourceValue_n = giResExtra2;
    giResType1 = firstResourceType;
    giResExtra1 = firstResourceValue;
    giResType2 = secondResourceType;
    giResExtra2 = secondResourceValue;

    resourceType_a[0] = firstResourceType;
    resourceValue_c[0] = firstResourceValue;
    resourceType_a[1] = secondResourceType;
    resourceValue_c[1] = secondResourceValue;

    lineCount = bigFont->LineLength(text, NORMAL_DIALOG_TEXT_LINE_WIDTH);
    dialogContentHeight = lineCount * NORMAL_DIALOG_TEXT_LINE_HEIGHT;
    maxIconHeight = 0;
    if (dialogType != NORMAL_DIALOG_QUICK_VIEW)
        dialogContentHeight += NORMAL_DIALOG_BUTTON_AREA_HEIGHT;

    for (resourceSlot = 0; resourceSlot < NORMAL_DIALOG_RESOURCE_COUNT; resourceSlot++) {
        switch (resourceType_a[resourceSlot]) {
            case NORMAL_DIALOG_ARTIFACT:
                sizingIconHeight = NORMAL_DIALOG_ARTIFACT_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_LUCK_BONUS:
                sizingIconHeight = NORMAL_DIALOG_LUCK_BONUS_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_LUCK_PENALTY:
                sizingIconHeight = NORMAL_DIALOG_LUCK_PENALTY_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_MORALE_BONUS:
                sizingIconHeight = NORMAL_DIALOG_MORALE_BONUS_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_MORALE_PENALTY:
                sizingIconHeight = NORMAL_DIALOG_MORALE_PENALTY_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_EXPERIENCE:
                sizingIconHeight = resourceValue_c[resourceSlot] == NORMAL_DIALOG_NO_VALUE
                    ? NORMAL_DIALOG_EXPERIENCE_ICON_HEIGHT
                    : NORMAL_DIALOG_EXPERIENCE_ICON_HEIGHT + NORMAL_DIALOG_RESOURCE_LABEL_HEIGHT;
                break;
            case NORMAL_DIALOG_CREST:
                sizingIconHeight = NORMAL_DIALOG_CREST_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_HERO:
                sizingIconHeight = NORMAL_DIALOG_HERO_LAYOUT_HEIGHT;
                break;
            case NORMAL_DIALOG_RESOURCE_GOLD:
                sizingIconHeight = NORMAL_DIALOG_GOLD_LAYOUT_HEIGHT;
                break;
            case NORMAL_DIALOG_RESOURCE_WOOD:
            case NORMAL_DIALOG_RESOURCE_MERCURY:
            case NORMAL_DIALOG_RESOURCE_ORE:
            case NORMAL_DIALOG_RESOURCE_SULFUR:
            case NORMAL_DIALOG_RESOURCE_CRYSTAL:
            case NORMAL_DIALOG_RESOURCE_GEMS:
                sizingIconHeight = NORMAL_DIALOG_RESOURCE_LAYOUT_HEIGHT;
                break;
            case NORMAL_DIALOG_SPELL:
                sizingIconHeight = NORMAL_DIALOG_SPELL_LAYOUT_HEIGHT;
                break;
            case NORMAL_DIALOG_SECONDARY_SKILL:
                sizingIconHeight = NORMAL_DIALOG_SECONDARY_SKILL_LAYOUT_HEIGHT;
                break;
            case NORMAL_DIALOG_MONSTER:
                sizingIconHeight = NORMAL_DIALOG_PRIMARY_MONSTER_LAYOUT_HEIGHT;
                break;
            case NORMAL_DIALOG_PRIMARY_SKILL:
                sizingIconHeight = NORMAL_DIALOG_PRIMARY_MONSTER_LAYOUT_HEIGHT;
                break;
            default:
                sizingIconHeight = 0;
                break;
        }
        if (sizingIconHeight > maxIconHeight)
            maxIconHeight = sizingIconHeight;
    }

    if (maxIconHeight > 0)
        dialogContentHeight += maxIconHeight + NORMAL_DIALOG_RESOURCE_VERTICAL_GAP;
    windowRows_b = (dialogContentHeight - NORMAL_DIALOG_ROW_CALCULATION_OFFSET)
                   / NORMAL_DIALOG_WINDOW_ROW_HEIGHT;
    if (windowRows_b > NORMAL_DIALOG_MAX_ROWS)
        windowRows_b = NORMAL_DIALOG_MAX_ROWS;
    windowWidth_f = NORMAL_DIALOG_WINDOW_WIDTH;
    windowHeight_h =
        windowRows_b * NORMAL_DIALOG_WINDOW_ROW_HEIGHT + NORMAL_DIALOG_WINDOW_BASE_HEIGHT;

    if (windowX == -1 || windowWidth_f + windowX >= NORMAL_DIALOG_SCREEN_RIGHT)
        windowX = NORMAL_DIALOG_DEFAULT_X;
    if (windowY == -1 || windowHeight_h + windowY >= NORMAL_DIALOG_SCREEN_BOTTOM) {
        windowY = NormalDialogCenterOffset(NORMAL_DIALOG_SCREEN_HEIGHT - windowHeight_h);
        if (windowY > NORMAL_DIALOG_MAX_TOP)
            windowY = NORMAL_DIALOG_MAX_TOP;
    }

    sprintf(
        iconFile_a,
        "evntwin%d.bin",
        windowRows_b
    );
    pNormalDialogWindow = new heroWindow(windowX, windowY, iconFile_a);
    if (!pNormalDialogWindow)
        MemError();

    message_b.type = NORMAL_DIALOG_DISABLE_MESSAGE;
    message_b.payload.widget.command = NORMAL_DIALOG_DISABLE_COMMAND;
    message_b.payload.widget.data.text = reinterpret_cast<char*>(NORMAL_DIALOG_DISABLE_COMMAND);
    if (dialogType != NORMAL_DIALOG_DISABLE_SEVENTH && dialogType != NORMAL_DIALOG_DISABLE_EIGHTH) {
        message_b.payload.widget.id = NORMAL_DIALOG_BUTTON_SEVEN;
        pNormalDialogWindow->BroadcastMessage(message_b);
    }
    if (dialogType != NORMAL_DIALOG_DISABLE_SEVENTH) {
        message_b.payload.widget.id = NORMAL_DIALOG_BUTTON_EIGHT;
        pNormalDialogWindow->BroadcastMessage(message_b);
    }
    if (dialogType != NORMAL_DIALOG_WAIT_LAST && dialogType != NORMAL_DIALOG_BUTTON_PAIR) {
        message_b.payload.widget.id = NORMAL_DIALOG_BUTTON_ONE;
        pNormalDialogWindow->BroadcastMessage(message_b);
    }
    if (dialogType != NORMAL_DIALOG_WAIT_FIRST && dialogType != NORMAL_DIALOG_INFO
        && dialogType != NORMAL_DIALOG_BUTTON_PAIR) {
        message_b.payload.widget.id = NORMAL_DIALOG_BUTTON_TWO;
        pNormalDialogWindow->BroadcastMessage(message_b);
    }
    if (dialogType != NORMAL_DIALOG_CONFIRM) {
        message_b.payload.widget.id = NORMAL_DIALOG_BUTTON_FIVE;
        pNormalDialogWindow->BroadcastMessage(message_b);
        message_b.payload.widget.id = NORMAL_DIALOG_BUTTON_SIX;
        pNormalDialogWindow->BroadcastMessage(message_b);
    }

    for (resourceSlot = 0; resourceSlot < NORMAL_DIALOG_RESOURCE_COUNT; resourceSlot++) {
        iconPanel_a = NULL;
        textPanel_j = NULL;
        if (resourceType_a[resourceSlot] == NORMAL_DIALOG_NO_RESOURCE)
            break;

        resourceText_p[resourceSlot] = static_cast<char*>(H2_ALLOC(NORMAL_DIALOG_TEXT_LENGTH));
        if (resourceType_a[resourceSlot] <= NORMAL_DIALOG_RESOURCE_LAST) {
            if (resourceValue_c[resourceSlot] > 0) {
                sprintf(resourceText_p[resourceSlot], "%d", resourceValue_c[resourceSlot]);
            } else if (resourceValue_c[resourceSlot] == 0) {
                strcpy(
                    resourceText_p[resourceSlot],
                    ""
                );
            } else if (resourceValue_c[resourceSlot] < -NORMAL_DIALOG_DAILY_RESOURCE_OFFSET) {
                sprintf(
                    resourceText_p[resourceSlot],
                    "%d",
                    resourceValue_c[resourceSlot] + NORMAL_DIALOG_DAILY_RESOURCE_OFFSET
                );
            } else {
                sprintf(
                    resourceText_p[resourceSlot],
                    localization::Tr("resource.per_day"),
                    -resourceValue_c[resourceSlot]
                );
            }
            strcpy(iconFile_a, "resource.icn");
            resourceFrame_n = resourceType_a[resourceSlot];
        } else if (resourceType_a[resourceSlot] == NORMAL_DIALOG_SPELL) {
            sprintf(
                resourceText_p[resourceSlot],
                "%s",
                gSpellNames[resourceValue_c[resourceSlot]]
            );
            strcpy(iconFile_a, "spells.icn");
            resourceFrame_n = gsSpellInfo[resourceValue_c[resourceSlot]].iconIndex;
        } else if (resourceType_a[resourceSlot] == NORMAL_DIALOG_CREST) {
            sprintf(
                resourceText_p[resourceSlot],
                "%s",
                ""
            );
            strcpy(iconFile_a, "brcrest.icn");
            resourceFrame_n = resourceValue_c[resourceSlot];
        } else if (resourceType_a[resourceSlot] == NORMAL_DIALOG_PRIMARY_SKILL) {
            sprintf(
                resourceText_p[resourceSlot],
                "%s",
                ""
            );
            strcpy(iconFile_a, "primskil.icn");
            resourceFrame_n = NORMAL_DIALOG_PRIMARY_BACKGROUND_FRAME;
        } else if (resourceType_a[resourceSlot] == NORMAL_DIALOG_MONSTER) {
            sprintf(
                resourceText_p[resourceSlot],
                "%s",
                ""
            );
            strcpy(iconFile_a, "strip.icn");
            resourceFrame_n = NORMAL_DIALOG_MONSTER_BACKGROUND_FRAME;
        } else if (resourceType_a[resourceSlot] == NORMAL_DIALOG_SECONDARY_SKILL) {
            sprintf(
                resourceText_p[resourceSlot],
                "%s",
                gSecondarySkills
                    [resourceValue_c[resourceSlot] / SECONDARY_SKILL_VALUE_LEVEL_COUNT]
            );
            strcpy(iconFile_a, "secskill.icn");
            resourceFrame_n = resourceValue_c[resourceSlot] / SECONDARY_SKILL_VALUE_LEVEL_COUNT
                              + NORMAL_DIALOG_SECONDARY_BACKGROUND_FRAME_BASE;
        } else if (resourceType_a[resourceSlot] == NORMAL_DIALOG_HERO) {
            sprintf(
                resourceText_p[resourceSlot],
                "%s",
                ""
            );
            sprintf(iconFile_a, "surrendr.icn");
            resourceFrame_n = NORMAL_DIALOG_HERO_BACKGROUND_FRAME;
        } else if (resourceType_a[resourceSlot] == NORMAL_DIALOG_EXPERIENCE
                   || resourceType_a[resourceSlot] == NORMAL_DIALOG_MORALE_BONUS
                   || resourceType_a[resourceSlot] == NORMAL_DIALOG_MORALE_PENALTY
                   || resourceType_a[resourceSlot] == NORMAL_DIALOG_LUCK_BONUS
                   || resourceType_a[resourceSlot] == NORMAL_DIALOG_LUCK_PENALTY) {
            strcpy(
                resourceText_p[resourceSlot],
                ""
            );
            strcpy(iconFile_a, "expmrl.icn");
            resourceFrame_n = resourceType_a[resourceSlot] - NORMAL_DIALOG_EXPMRL_FIRST;
            if (resourceType_a[resourceSlot] == NORMAL_DIALOG_EXPMRL_LAST
                && resourceValue_c[resourceSlot] != NORMAL_DIALOG_NO_VALUE) {
                sprintf(resourceText_p[resourceSlot], "%d", resourceValue_c[resourceSlot]);
            }
        } else {
            strcpy(
                resourceText_p[resourceSlot],
                ""
            );
            strcpy(iconFile_a, "resource.icn");
            resourceFrame_n = resourceType_a[resourceSlot];
        }

        switch (resourceType_a[resourceSlot]) {
            case NORMAL_DIALOG_PRIMARY_SKILL:
                resourceImageWidth = NORMAL_DIALOG_PRIMARY_MONSTER_BACKGROUND_WIDTH;
                sizingIconHeight = NORMAL_DIALOG_PRIMARY_MONSTER_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_ARTIFACT:
                resourceImageWidth = NORMAL_DIALOG_LARGE_ICON_WIDTH;
                sizingIconHeight = NORMAL_DIALOG_ARTIFACT_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_LUCK_BONUS:
                resourceImageWidth = NORMAL_DIALOG_EXPMRL_ICON_WIDTH;
                sizingIconHeight = NORMAL_DIALOG_LUCK_BONUS_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_LUCK_PENALTY:
                resourceImageWidth = NORMAL_DIALOG_EXPMRL_ICON_WIDTH;
                sizingIconHeight = NORMAL_DIALOG_LUCK_PENALTY_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_MORALE_BONUS:
                resourceImageWidth = NORMAL_DIALOG_EXPMRL_ICON_WIDTH;
                sizingIconHeight = NORMAL_DIALOG_MORALE_BONUS_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_MORALE_PENALTY:
                resourceImageWidth = NORMAL_DIALOG_EXPMRL_ICON_WIDTH;
                sizingIconHeight = NORMAL_DIALOG_MORALE_PENALTY_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_EXPERIENCE:
                resourceImageWidth = NORMAL_DIALOG_EXPMRL_ICON_WIDTH;
                sizingIconHeight = NORMAL_DIALOG_EXPERIENCE_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_CREST:
                resourceImageWidth = NORMAL_DIALOG_CREST_ICON_WIDTH;
                sizingIconHeight = NORMAL_DIALOG_CREST_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_HERO:
                resourceImageWidth = NORMAL_DIALOG_HERO_BACKGROUND_WIDTH;
                sizingIconHeight = NORMAL_DIALOG_HERO_BACKGROUND_HEIGHT;
                break;
            case NORMAL_DIALOG_RESOURCE_GOLD:
                resourceImageWidth = NORMAL_DIALOG_LARGE_ICON_WIDTH;
                sizingIconHeight = NORMAL_DIALOG_GOLD_LAYOUT_HEIGHT;
                break;
            case NORMAL_DIALOG_RESOURCE_WOOD:
            case NORMAL_DIALOG_RESOURCE_MERCURY:
            case NORMAL_DIALOG_RESOURCE_ORE:
            case NORMAL_DIALOG_RESOURCE_SULFUR:
            case NORMAL_DIALOG_RESOURCE_CRYSTAL:
            case NORMAL_DIALOG_RESOURCE_GEMS:
                resourceImageWidth = NORMAL_DIALOG_RESOURCE_ICON_WIDTH;
                sizingIconHeight = NORMAL_DIALOG_RESOURCE_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_SPELL:
                resourceImageWidth = NORMAL_DIALOG_SPELL_ICON_WIDTH;
                sizingIconHeight = NORMAL_DIALOG_SPELL_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_SECONDARY_SKILL:
                resourceImageWidth = NORMAL_DIALOG_SECONDARY_SKILL_BACKGROUND_WIDTH;
                sizingIconHeight = NORMAL_DIALOG_SECONDARY_SKILL_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_MONSTER:
                resourceImageWidth = NORMAL_DIALOG_PRIMARY_MONSTER_BACKGROUND_WIDTH;
                sizingIconHeight = NORMAL_DIALOG_PRIMARY_MONSTER_ICON_HEIGHT;
                break;
        }

        imageHeight_p = sizingIconHeight;
        if (strlen(resourceText_p[resourceSlot]) > 0)
            sizingIconHeight += NORMAL_DIALOG_RESOURCE_LABEL_HEIGHT;

        if (resourceSlot == 0) {
            resourceCenterX_c = resourceType_a[1] == NORMAL_DIALOG_NO_RESOURCE
                ? NormalDialogCenterOffset(
                      windowWidth_f - NORMAL_DIALOG_SINGLE_RESOURCE_CENTER_INSET
                  ) + NORMAL_DIALOG_SINGLE_RESOURCE_CENTER_INSET
                : NORMAL_DIALOG_FIRST_RESOURCE_CENTER_X;
        } else {
            resourceCenterX_c = windowWidth_f - NORMAL_DIALOG_SECOND_RESOURCE_RIGHT_INSET;
        }
        resourceY_f = windowHeight_h - sizingIconHeight - NORMAL_DIALOG_RESOURCE_BOTTOM_INSET;
        if (dialogType != NORMAL_DIALOG_QUICK_VIEW)
            resourceY_f -= NORMAL_DIALOG_BUTTON_AREA_HEIGHT;
        if (resourceType_a[0] == NORMAL_DIALOG_SECONDARY_SKILL
            && resourceType_a[1] == NORMAL_DIALOG_SECONDARY_SKILL) {
            if (resourceSlot == 0)
                resourceCenterX_c -= NORMAL_DIALOG_SECONDARY_PAIR_SPACING;
            else
                resourceCenterX_c += NORMAL_DIALOG_SECONDARY_PAIR_SPACING;
        }

        iconPanel_a = new iconWidget(
            resourceCenterX_c - NormalDialogCenterOffset(resourceImageWidth)
                + (resourceType_a[resourceSlot] == NORMAL_DIALOG_SPELL
                       ? NORMAL_DIALOG_SPELL_BACKGROUND_X_OFFSET
                       : 0),
            resourceY_f,
            resourceImageWidth,
            imageHeight_p,
            iconFile_a,
            resourceFrame_n,
            ICON_DRAW_NORMAL,
            -1,
            resourceType_a[resourceSlot] == NORMAL_DIALOG_SPELL ? WIDGET_KIND_ICON_CENTERED
                                                                  : WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (!iconPanel_a)
            MemError();
        pNormalDialogWindow->AddWidget(iconPanel_a, -1);

        if (resourceType_a[resourceSlot] == NORMAL_DIALOG_ARTIFACT) {
            iconPanel_a = new iconWidget(
                resourceCenterX_c - NormalDialogCenterOffset(resourceImageWidth)
                    + NORMAL_DIALOG_ICON_OVERLAY_INSET,
                resourceY_f + NORMAL_DIALOG_ICON_OVERLAY_INSET,
                NORMAL_DIALOG_LARGE_ICON_WIDTH,
                NORMAL_DIALOG_ARTIFACT_ICON_HEIGHT,
                "artifact.icn",
                resourceValue_c[resourceSlot] + NORMAL_DIALOG_ARTIFACT_FRAME_OFFSET,
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (!iconPanel_a)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_a, -1);
        }
        if (resourceType_a[resourceSlot] == NORMAL_DIALOG_PRIMARY_SKILL) {
            iconPanel_a = new iconWidget(
                resourceCenterX_c - NormalDialogCenterOffset(resourceImageWidth)
                    + NORMAL_DIALOG_ICON_OVERLAY_INSET,
                resourceY_f + NORMAL_DIALOG_ICON_OVERLAY_INSET,
                NORMAL_DIALOG_PRIMARY_MONSTER_OVERLAY_WIDTH,
                NORMAL_DIALOG_PRIMARY_MONSTER_OVERLAY_HEIGHT,
                "primskil.icn",
                resourceValue_c[resourceSlot],
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (!iconPanel_a)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_a, -1);
            strcpy(resourceText_p[resourceSlot], gStatNames[resourceValue_c[resourceSlot]]);
        }
        if (resourceType_a[resourceSlot] == NORMAL_DIALOG_MONSTER) {
            iconPanel_a = new iconWidget(
                resourceCenterX_c - NormalDialogCenterOffset(resourceImageWidth)
                    + NORMAL_DIALOG_ICON_OVERLAY_INSET,
                resourceY_f + NORMAL_DIALOG_ICON_OVERLAY_INSET,
                NORMAL_DIALOG_PRIMARY_MONSTER_OVERLAY_WIDTH,
                NORMAL_DIALOG_PRIMARY_MONSTER_OVERLAY_HEIGHT,
                "strip.icn",
                IDX(gMonsterDatabase[resourceValue_c[resourceSlot]].race)
                    + NORMAL_DIALOG_MONSTER_RACE_FRAME_OFFSET,
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (!iconPanel_a)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_a, -1);

            sprintf(
                gText,
                "monh%04d.icn",
                resourceValue_c[resourceSlot]
            );
            iconPanel_a = new iconWidget(
                resourceCenterX_c - NormalDialogCenterOffset(resourceImageWidth)
                    + NORMAL_DIALOG_ICON_OVERLAY_INSET,
                resourceY_f + NORMAL_DIALOG_ICON_OVERLAY_INSET,
                NORMAL_DIALOG_PRIMARY_MONSTER_OVERLAY_WIDTH,
                NORMAL_DIALOG_PRIMARY_MONSTER_OVERLAY_HEIGHT,
                gText,
                0,
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (!iconPanel_a)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_a, -1);
        }
        if (resourceType_a[resourceSlot] == NORMAL_DIALOG_CREST) {
            iconPanel_a = new iconWidget(
                resourceCenterX_c - NormalDialogCenterOffset(resourceImageWidth)
                    - NORMAL_DIALOG_CREST_OVERLAY_OUTSET,
                resourceY_f - NORMAL_DIALOG_CREST_OVERLAY_OUTSET,
                NORMAL_DIALOG_CREST_OVERLAY_WIDTH,
                NORMAL_DIALOG_CREST_OVERLAY_HEIGHT,
                "brcrest.icn",
                NORMAL_DIALOG_CREST_OVERLAY_FRAME,
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (!iconPanel_a)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_a, -1);
        }
        if (resourceType_a[resourceSlot] == NORMAL_DIALOG_SECONDARY_SKILL) {
            iconPanel_a = new iconWidget(
                resourceCenterX_c - NormalDialogCenterOffset(resourceImageWidth)
                    - NORMAL_DIALOG_SECONDARY_SKILL_OVERLAY_OUTSET,
                resourceY_f - NORMAL_DIALOG_SECONDARY_SKILL_OVERLAY_OUTSET,
                NORMAL_DIALOG_SECONDARY_SKILL_OVERLAY_WIDTH,
                NORMAL_DIALOG_SECONDARY_SKILL_OVERLAY_HEIGHT,
                "secskill.icn",
                NORMAL_DIALOG_SECONDARY_OVERLAY_FRAME,
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (!iconPanel_a)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_a, -1);
        }
        if (resourceType_a[resourceSlot] == NORMAL_DIALOG_HERO) {
            sprintf(
                iconFile_a,
                "port%04d.icn",
                resourceValue_c[resourceSlot]
            );
            iconPanel_a = new iconWidget(
                resourceCenterX_c - NormalDialogCenterOffset(resourceImageWidth)
                    + NORMAL_DIALOG_HERO_OVERLAY_INSET,
                resourceY_f + NORMAL_DIALOG_HERO_OVERLAY_INSET,
                NORMAL_DIALOG_HERO_OVERLAY_WIDTH,
                NORMAL_DIALOG_HERO_OVERLAY_HEIGHT,
                iconFile_a,
                0,
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (!iconPanel_a)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_a, -1);
        }

        if (resourceType_a[resourceSlot] == NORMAL_DIALOG_SECONDARY_SKILL) {
            labelY_k = resourceY_f + sizingIconHeight - NORMAL_DIALOG_SECONDARY_NAME_Y_OFFSET;
            textPanel_j = new textWidget(
                resourceCenterX_c - NORMAL_DIALOG_RESOURCE_LABEL_HALF_WIDTH,
                labelY_k,
                NORMAL_DIALOG_RESOURCE_LABEL_WIDTH,
                resourceType_a[resourceSlot] == NORMAL_DIALOG_SPELL
                    ? NORMAL_DIALOG_SPELL_LABEL_HEIGHT
                    : NORMAL_DIALOG_RESOURCE_LABEL_HEIGHT,
                resourceText_p[resourceSlot],
                "smalfont.fnt",
                FONT_DRAW_DEFAULT,
                textWidgetId++,
                WIDGET_KIND_TEXT,
                FONT_ALIGN_CENTER
            );
            if (!textPanel_j)
                MemError();
            pNormalDialogWindow->AddWidget(textPanel_j, -1);

            resourceText_p[resourceSlot] =
                static_cast<char*>(H2_ALLOC(NORMAL_DIALOG_TEXT_LENGTH));
            labelY_k = resourceY_f + sizingIconHeight - NORMAL_DIALOG_SECONDARY_LEVEL_Y_OFFSET;
            sprintf(
                resourceText_p[resourceSlot],
                "%s",
                gSecondarySkillLevels
                    [resourceValue_c[resourceSlot] % SECONDARY_SKILL_VALUE_LEVEL_COUNT]
            );
        } else if (resourceType_a[resourceSlot] == NORMAL_DIALOG_PRIMARY_SKILL) {
            labelY_k = resourceY_f + sizingIconHeight - NORMAL_DIALOG_PRIMARY_LABEL_Y_OFFSET;
        } else {
            labelY_k = resourceY_f + sizingIconHeight - NORMAL_DIALOG_DEFAULT_LABEL_Y_OFFSET;
        }

        textPanel_j = new textWidget(
            resourceCenterX_c - NORMAL_DIALOG_RESOURCE_LABEL_HALF_WIDTH,
            labelY_k,
            NORMAL_DIALOG_RESOURCE_LABEL_WIDTH,
            resourceType_a[resourceSlot] == NORMAL_DIALOG_SPELL
                ? NORMAL_DIALOG_SPELL_LABEL_HEIGHT
                : NORMAL_DIALOG_RESOURCE_LABEL_HEIGHT,
            resourceText_p[resourceSlot],
            "smalfont.fnt",
            FONT_DRAW_DEFAULT,
            textWidgetId++,
            WIDGET_KIND_TEXT,
            FONT_ALIGN_CENTER
        );
        if (!textPanel_j)
            MemError();
        pNormalDialogWindow->AddWidget(textPanel_j, -1);

        if (resourceType_a[resourceSlot] == NORMAL_DIALOG_PRIMARY_SKILL && showPrimaryBonus) {
            char* bonusText = static_cast<char*>(H2_ALLOC(NORMAL_DIALOG_PRIMARY_BONUS_TEXT_LENGTH));
            strcpy(bonusText, "+1 ");
            textPanel_j = new textWidget(
                resourceCenterX_c - NORMAL_DIALOG_RESOURCE_LABEL_HALF_WIDTH,
                resourceY_f + sizingIconHeight - NORMAL_DIALOG_PRIMARY_BONUS_LABEL_Y_OFFSET,
                NORMAL_DIALOG_RESOURCE_LABEL_WIDTH,
                NORMAL_DIALOG_PRIMARY_BONUS_TEXT_HEIGHT,
                bonusText,
                "bigfont.fnt",
                FONT_DRAW_DEFAULT,
                textWidgetId++,
                WIDGET_KIND_TEXT,
                FONT_ALIGN_CENTER
            );
            if (!textPanel_j)
                MemError();
            pNormalDialogWindow->AddWidget(textPanel_j, -1);
        }

        borderWidget_k = new border(
            resourceCenterX_c - NormalDialogCenterOffset(resourceImageWidth),
            resourceY_f,
            resourceImageWidth,
            sizingIconHeight,
            resourceSlot + NORMAL_DIALOG_RESOURCE_BORDER_FIRST_ID,
            WIDGET_KIND_TRANSPARENT,
            0,
            NULL
        );
        pNormalDialogWindow->AddWidget(borderWidget_k, -1);
    }

    SET_WIDGET_MESSAGE(message_b, NORMAL_DIALOG_SET_TEXT_COMMAND, NORMAL_DIALOG_TEXT_WIDGET_ID);
    message_b.payload.widget.data.text = text;
    pNormalDialogWindow->BroadcastMessage(message_b);

    if (showOrText == NORMAL_DIALOG_SHOW_OR_TEXT) {
        orText = static_cast<char*>(H2_ALLOC(NORMAL_DIALOG_OR_TEXT_LENGTH));
        strcpy(orText, localization::Tr("common.or"));
        textPanel_j = new textWidget(
            windowWidth_f / NORMAL_DIALOG_CENTER_PART_COUNT
                - NORMAL_DIALOG_OR_TEXT_CENTER_X_OFFSET,
            resourceY_f + NORMAL_DIALOG_OR_TEXT_Y_OFFSET,
            NORMAL_DIALOG_OR_TEXT_WIDTH,
            NORMAL_DIALOG_OR_TEXT_HEIGHT,
            orText,
            "smalfont.fnt",
            FONT_DRAW_DEFAULT,
            textWidgetId++,
            WIDGET_KIND_TEXT,
            FONT_ALIGN_CENTER
        );
        if (!textPanel_j)
            MemError();
        pNormalDialogWindow->AddWidget(textPanel_j, -1);
    }

    savedPointerType_o = gpMouseManager->m_cursorType;
    savedPointerFrame = gpMouseManager->m_cursorFrame;
    while (gpMouseManager->m_hideCount)
        gpMouseManager->ShowColorPointer();
    gpMouseManager->SetPointer(
        "advmice.mse",
        0,
        MOUSE_AUTO_CURSOR_TYPE
    );

    if (dialogType == NORMAL_DIALOG_WAIT_LAST || dialogType == NORMAL_DIALOG_WAIT_FIRST) {
        gpWindowManager->DoDialog(pNormalDialogWindow, WaitHandler, 0);
    } else if (dialogType == NORMAL_DIALOG_QUICK_VIEW) {
        gpWindowManager->AddWindow(pNormalDialogWindow, -1, 1);
        QuickViewWait();
        gpWindowManager->RemoveWindow(pNormalDialogWindow);
    } else {
        gpWindowManager->DoDialog(pNormalDialogWindow, EventWindowHandler, 0);
    }

    delete pNormalDialogWindow;
    gpMouseManager->SetPointer(
        "",
        savedPointerFrame,
        savedPointerType_o
    );
    giResType1 = savedFirstResourceType_k;
    giResExtra1 = savedFirstResourceValue;
    giResType2 = savedSecondResourceType_m;
    giResExtra2 = savedSecondResourceValue_n;
    pNormalDialogWindow = savedNormalDialogWindow;
}

VA(0x00470d22, 0x68)
void UpdateNormalDialog(H2_CONST char* text) {
    i16 H2_UNUSED(show) = 1;
    tag_message evt;
    SET_WIDGET_MESSAGE(evt, WIDGET_COMMAND_SET_TEXT, 1);
    evt.payload.widget.data.text = text;
    pNormalDialogWindow->BroadcastMessage(evt);
    pNormalDialogWindow->DrawWindow(0, 0, NORMAL_DIALOG_FOREGROUND_WIDGET_LIMIT);
    pNormalDialogWindow
        ->DrawWindow(1, WINDOW_ALL_WIDGETS_LOW, NORMAL_DIALOG_BACKGROUND_WIDGET_LAST_ID);
}

#define GROUND_REPEAT_2(value) value, value
#define GROUND_REPEAT_4(value) GROUND_REPEAT_2(value), GROUND_REPEAT_2(value)
#define GROUND_REPEAT_8(value) GROUND_REPEAT_4(value), GROUND_REPEAT_4(value)
#define GROUND_REPEAT_16(value) GROUND_REPEAT_8(value), GROUND_REPEAT_8(value)
#define GROUND_REPEAT_32(value) GROUND_REPEAT_16(value), GROUND_REPEAT_16(value)
#define GROUND_SHAPE_STANDARD_FRAME_SET                                                            \
    GROUND_REPEAT_4(1), GROUND_REPEAT_4(2), GROUND_REPEAT_4(3), GROUND_REPEAT_4(4),                \
        GROUND_REPEAT_4(5), GROUND_REPEAT_4(6), GROUND_REPEAT_4(7), GROUND_REPEAT_4(8), 10, 11,    \
        12, 13, 14, 15, GROUND_REPEAT_8(0)

DATA(0x004f8138) H2_ENUM_STORAGE(TerrainType, u8)
giGroundToTerrain[GROUND_TILE_IMAGE_COUNT] = {
    GROUND_REPEAT_16(TERRAIN_WATER),
    GROUND_REPEAT_8(TERRAIN_WATER),
    GROUND_REPEAT_4(TERRAIN_WATER),
    GROUND_REPEAT_2(TERRAIN_WATER),
    GROUND_REPEAT_32(TERRAIN_GRASS),
    GROUND_REPEAT_16(TERRAIN_GRASS),
    GROUND_REPEAT_8(TERRAIN_GRASS),
    GROUND_REPEAT_4(TERRAIN_GRASS),
    GROUND_REPEAT_2(TERRAIN_GRASS),
    GROUND_REPEAT_32(TERRAIN_SNOW),
    GROUND_REPEAT_16(TERRAIN_SNOW),
    GROUND_REPEAT_4(TERRAIN_SNOW),
    GROUND_REPEAT_2(TERRAIN_SNOW),
    GROUND_REPEAT_32(TERRAIN_SWAMP),
    GROUND_REPEAT_16(TERRAIN_SWAMP),
    GROUND_REPEAT_8(TERRAIN_SWAMP),
    GROUND_REPEAT_4(TERRAIN_SWAMP),
    GROUND_REPEAT_2(TERRAIN_SWAMP),
    GROUND_REPEAT_32(TERRAIN_LAVA),
    GROUND_REPEAT_16(TERRAIN_LAVA),
    GROUND_REPEAT_4(TERRAIN_LAVA),
    GROUND_REPEAT_2(TERRAIN_LAVA),
    GROUND_REPEAT_32(TERRAIN_DESERT),
    GROUND_REPEAT_16(TERRAIN_DESERT),
    GROUND_REPEAT_8(TERRAIN_DESERT),
    GROUND_REPEAT_2(TERRAIN_DESERT),
    TERRAIN_DESERT,
    GROUND_REPEAT_32(TERRAIN_DIRT),
    GROUND_REPEAT_8(TERRAIN_DIRT),
    GROUND_REPEAT_32(TERRAIN_WASTELAND),
    GROUND_REPEAT_16(TERRAIN_WASTELAND),
    GROUND_REPEAT_4(TERRAIN_WASTELAND),
    GROUND_REPEAT_2(TERRAIN_WASTELAND),
    GROUND_REPEAT_16(TERRAIN_BEACH),
    TERRAIN_BEACH
};
DATA(0x004f82e8) u8 giGroundShape[GROUND_TILE_IMAGE_COUNT] = {
    GROUND_REPEAT_2(16),
    GROUND_REPEAT_2(1),
    GROUND_REPEAT_4(2),
    GROUND_REPEAT_2(17),
    GROUND_REPEAT_2(3),
    GROUND_REPEAT_4(4),
    GROUND_REPEAT_4(0),
    GROUND_REPEAT_4(18),
    GROUND_REPEAT_2(20),
    GROUND_REPEAT_2(21),
    GROUND_REPEAT_2(19),
    GROUND_SHAPE_STANDARD_FRAME_SET,
    GROUND_REPEAT_16(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_STANDARD_FRAME_SET,
    GROUND_REPEAT_8(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_STANDARD_FRAME_SET,
    GROUND_REPEAT_16(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_STANDARD_FRAME_SET,
    GROUND_REPEAT_8(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_STANDARD_FRAME_SET,
    GROUND_REPEAT_8(GROUND_SHAPE_FLIPPED),
    GROUND_REPEAT_4(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_FLIPPED,
    GROUND_REPEAT_4(5),
    GROUND_REPEAT_4(6),
    GROUND_REPEAT_4(7),
    GROUND_REPEAT_4(8),
    GROUND_REPEAT_8(0),
    GROUND_REPEAT_16(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_STANDARD_FRAME_SET,
    GROUND_REPEAT_8(GROUND_SHAPE_FLIPPED),
    GROUND_REPEAT_8(0),
    GROUND_REPEAT_8(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_FLIPPED
};

#undef GROUND_SHAPE_STANDARD_FRAME_SET
#undef GROUND_REPEAT_32
#undef GROUND_REPEAT_16
#undef GROUND_REPEAT_8
#undef GROUND_REPEAT_4
#undef GROUND_REPEAT_2

DATA(0x004f8498) u8 gColorTableTan[DIM_PALETTE_COLOR_COUNT] = {
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
DATA(0x004f8598) u8 gColorTableGray[DIM_PALETTE_COLOR_COUNT] = {
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
DATA(0x004f8698) u8 gColorTableYellow[DIM_PALETTE_COLOR_COUNT] = {
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
DATA(0x004f8798) u8 gColorTableScenWin[DIM_PALETTE_COLOR_COUNT] = {
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
DATA(0x004f8898) u8 gColorTableDarkGray[DIM_PALETTE_COLOR_COUNT] = {
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
DATA(0x004f8998) u8 gColorTableRed[DIM_PALETTE_COLOR_COUNT] = {
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
DATA(0x004f8a98) u8 gColorTableDarkBrown[DIM_PALETTE_COLOR_COUNT] = {
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
DATA(0x004f8b98) i32 MAP_WIDTH = MAP_DIMENSION_MEDIUM;
DATA(0x004f8b9c) i32 MAP_HEIGHT = MAP_DIMENSION_MEDIUM;
DATA(0x005265dc) u8* mapExtra = NULL;
DATA(0x005265e0) b32 gbClosingApp = false;
DATA(0x005265e4) b32 gbForegroundApp = false;
DATA(0x004f8ba0) i32 giMainVideoModeColorDepth = WINGRAPH_COLOR_DEPTH;
DATA(0x004f8ba4) i32 giMainVideoModeWidth = WINGRAPH_WIDTH;
DATA(0x004f8ba8) i32 giMainVideoModeHeight = WINGRAPH_HEIGHT;
DATA(0x004f8bac) u8 gMapColors[RADAR_MAP_COLOR_COUNT] = {77, 98, 13, 104, 32, 118, 54, 206, 41, 0, 0, 0};
DATA(0x004f8bb8) u8 gObjectColors[RADAR_OBJECT_COLOR_COUNT] =
    {16, 48, 98, 160, 126, 74, 110, 179, 100, 218, 12, 12, 12, 12, 12, 12};
DATA(0x004f8bc8) u8 gOwnerColors[RADAR_OWNER_COLOR_COUNT] = {73, 105, 190, 114, 205, 138, 10, 0};
DATA(0x004f8bd0) H2_CONST char* gTilesetFiles[IDX(TILESET_COUNT)] = {
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "objnhaun.icn",
    "objnarti.icn",
    "mons32.icn",
    "art32.icn",
    "flag32.icn",
    "ressmall.icn",
    "hourglas.icn",
    "route.icn",
    "",
    "stonback.icn",
    "minimon.icn",
    "minihero.icn",
    "mtnsnow.icn",
    "mtnswmp.icn",
    "mtnlava.icn",
    "mtndsrt.icn",
    "mtndirt.icn",
    "mtnmult.icn",
    "",
    "extraovr.icn",
    "road.icn",
    "mtncrck.icn",
    "mtngras.icn",
    "trejngl.icn",
    "treevil.icn",
    "objntown.icn",
    "objntwba.icn",
    "objntwsh.icn",
    "objntwrd.icn",
    "objnxtra.icn",
    "objnwat2.icn",
    "objnmul2.icn",
    "tresnow.icn",
    "trefir.icn",
    "trefall.icn",
    "stream.icn",
    "objnrsrc.icn",
    "dummy.icn",
    "objngra2.icn",
    "tredeci.icn",
    "objnwatr.icn",
    "objngras.icn",
    "objnsnow.icn",
    "objnswmp.icn",
    "objnlava.icn",
    "objndsrt.icn",
    "objndirt.icn",
    "objncrck.icn",
    "objnlav3.icn",
    "objnmult.icn",
    "objnlav2.icn",
    "x_loc1.icn",
    "x_loc2.icn",
    "x_loc3.icn"
};
DATA(0x004f8cd0) u8 bPuzzleDraw[PUZZLE_DRAW_TABLE_COUNT] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};
DATA(0x004f8d10) u8 uDimPal[DIM_PALETTE_SET_COUNT][DIM_PALETTE_LEVEL_COUNT][DIM_PALETTE_COLOR_COUNT] = {
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14,
      0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23,
      0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32,
      0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3e, 0x3e, 0x3e,
      0x3e, 0x3e, 0x3e, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
      0x50, 0x51, 0x52, 0x53, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
      0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6b, 0x6b, 0x6b,
      0x6b, 0x6b, 0x6b, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d,
      0x7e, 0x7f, 0x80, 0x81, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x88, 0x89, 0x8a, 0x8b,
      0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x97, 0x97, 0x97,
      0x97, 0x97, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa,
      0xab, 0xac, 0xad, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8,
      0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc5, 0xc5,
      0xc5, 0xc5, 0xc5, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5,
      0xd5, 0xd5, 0xd5, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xe1, 0xe2, 0xe3,
      0xe4, 0xe5, 0xe6, 0xe6, 0xe6, 0xe6, 0x49, 0x4b, 0x4d, 0x4f, 0x51, 0x4c, 0x4e, 0x4a, 0x4c,
      0x4e, 0x50, 0xf4, 0xf5, 0xf5, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x0f, 0x10, 0x11, 0x12,
      0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21,
      0x22, 0x23, 0x24, 0x24, 0x24, 0x24, 0x24, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
      0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3e,
      0x3e, 0x3e, 0x3e, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d,
      0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x54, 0x54, 0x54, 0x59, 0x5a, 0x5b, 0x5c, 0x5d,
      0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6b,
      0x6b, 0x6b, 0x6b, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b,
      0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x82, 0x82, 0x82, 0x82, 0x86, 0x87, 0x88, 0x89,
      0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x97,
      0x97, 0x97, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8,
      0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xae, 0xae, 0xae, 0xae, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
      0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5,
      0xc5, 0xc5, 0xc5, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4,
      0xd5, 0xd5, 0xd5, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xe0, 0xe1, 0xe2,
      0xe3, 0xe4, 0xe5, 0xe6, 0xe6, 0xe6, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c,
      0x4c, 0x4e, 0xf4, 0xf5, 0xf5, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
      0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
      0x20, 0x21, 0x22, 0x23, 0x24, 0x24, 0x24, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e,
      0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d,
      0x3e, 0x3e, 0x3e, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c,
      0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x54, 0x54, 0x57, 0x58, 0x59, 0x5a, 0x5b,
      0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,
      0x6b, 0x6b, 0x6b, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
      0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x82, 0x82, 0x85, 0x86, 0x87, 0x88,
      0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
      0x97, 0x97, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6,
      0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xae, 0xae, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5,
      0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4,
      0xc5, 0xc5, 0xc5, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3,
      0xd4, 0xd5, 0xd5, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xdf, 0xe0, 0xe1,
      0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe6, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c,
      0x4c, 0x4c, 0xf3, 0xf4, 0xf5, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
      0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x24, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d,
      0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c,
      0x3d, 0x3e, 0x3e, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b,
      0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x54, 0x56, 0x57, 0x58, 0x59, 0x5a,
      0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
      0x6a, 0x6b, 0x6b, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
      0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x82, 0x84, 0x85, 0x86, 0x87,
      0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,
      0x97, 0x97, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5,
      0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xae, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4,
      0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3,
      0xc4, 0xc5, 0xc5, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2,
      0xd3, 0xd4, 0xd5, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xdf, 0xe0, 0xe1,
      0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe6, 0x4b, 0x4b, 0x4b, 0x4b, 0x4b, 0x4b, 0x4b, 0x4b, 0x4b,
      0x4b, 0x4b, 0xf3, 0xf4, 0xf5, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00}},
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0a, 0x0b, 0x0b, 0x0b,
      0x0c, 0x0d, 0x0d, 0x0d, 0x0e, 0x0e, 0x0f, 0x0f, 0x0f, 0x10, 0x11, 0x11, 0x11, 0x12, 0x12,
      0x13, 0x13, 0x14, 0x14, 0x14, 0x15, 0x15, 0x0b, 0x25, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27,
      0x27, 0x28, 0x28, 0x29, 0x29, 0x29, 0x29, 0x2a, 0x2a, 0x13, 0x2a, 0x14, 0x14, 0x14, 0x14,
      0x14, 0x14, 0x15, 0x0c, 0x83, 0x3f, 0x3f, 0x3f, 0x40, 0x40, 0x40, 0x41, 0x41, 0x41, 0x41,
      0x41, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0x0d, 0x0e, 0x0f, 0x0f, 0x10,
      0x55, 0x11, 0x55, 0x55, 0x55, 0x55, 0x13, 0x56, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15,
      0x15, 0x15, 0x15, 0x0a, 0x6c, 0x6c, 0x6d, 0x6d, 0x6d, 0x6e, 0x6e, 0x6e, 0x6e, 0xc7, 0x28,
      0x29, 0x29, 0x29, 0x29, 0x29, 0x2a, 0x2a, 0x2a, 0x2a, 0x14, 0x14, 0x0b, 0x0b, 0x83, 0x83,
      0x84, 0x84, 0x84, 0x85, 0x85, 0x86, 0x86, 0x86, 0x87, 0x87, 0x12, 0x88, 0x13, 0x13, 0x14,
      0x14, 0x14, 0x0a, 0x0b, 0x0b, 0x0b, 0x0c, 0x0c, 0x0d, 0x0d, 0x0d, 0x0e, 0x0f, 0x0f, 0x0f,
      0x10, 0x11, 0x11, 0x11, 0x12, 0x12, 0x13, 0x13, 0x14, 0x14, 0x0b, 0xaf, 0xaf, 0xb0, 0xb0,
      0x26, 0xb1, 0xb1, 0xb2, 0xb2, 0xb2, 0xb3, 0xb3, 0xb3, 0xb3, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4,
      0xb4, 0x15, 0x15, 0x6c, 0x6c, 0x26, 0x6d, 0x26, 0x6d, 0x27, 0x28, 0x28, 0x29, 0x29, 0x29,
      0x2a, 0x2a, 0x2a, 0x14, 0xc7, 0xb3, 0xb4, 0xb4, 0x6e, 0x6e, 0x28, 0x2a, 0x6e, 0x6e, 0x56,
      0x56, 0x56, 0x56, 0x12, 0x12, 0x13, 0x41, 0x41, 0x41, 0x42, 0x41, 0x42, 0x41, 0x98, 0x9b,
      0x41, 0xf2, 0x0f, 0x10, 0x11, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0b, 0x0c, 0x0c,
      0x0d, 0x0d, 0x0e, 0x0f, 0x0f, 0x10, 0x10, 0x11, 0x11, 0x12, 0x13, 0x14, 0x14, 0x15, 0x15,
      0x16, 0x16, 0x17, 0x18, 0x18, 0x19, 0x19, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27, 0x28, 0x29,
      0x29, 0x29, 0x2a, 0x2a, 0x2b, 0x2b, 0x2c, 0x2c, 0x2d, 0x2d, 0x2e, 0x2e, 0x17, 0x18, 0x18,
      0x18, 0x18, 0x18, 0x83, 0x3f, 0x3f, 0x40, 0x40, 0x41, 0x41, 0x42, 0x42, 0xf2, 0x43, 0x43,
      0x44, 0x44, 0xf3, 0xf3, 0xf3, 0xf3, 0xf3, 0xf3, 0xf3, 0xf3, 0x0f, 0x0f, 0x55, 0x55, 0x55,
      0x55, 0x56, 0x56, 0x57, 0x57, 0x58, 0x58, 0x58, 0x58, 0x59, 0x18, 0x5a, 0x19, 0x19, 0x19,
      0x19, 0x19, 0x19, 0x25, 0x6c, 0x6d, 0x6d, 0x6e, 0x6e, 0x6f, 0x6f, 0xc8, 0xc8, 0xc9, 0xc9,
      0x2a, 0x2b, 0x2b, 0x2c, 0x2c, 0x2c, 0x2d, 0x2d, 0x2e, 0x2e, 0x2e, 0x0b, 0x83, 0x84, 0x84,
      0x84, 0x85, 0x85, 0x86, 0x87, 0x87, 0x88, 0xf2, 0x89, 0x89, 0x8a, 0xf3, 0xf3, 0xf3, 0xf3,
      0xf3, 0x18, 0x98, 0x98, 0x99, 0x99, 0x9a, 0x9a, 0x9b, 0x9c, 0x9c, 0x9d, 0x9e, 0x9e, 0x9f,
      0x12, 0x13, 0x13, 0x14, 0x14, 0x15, 0x16, 0x16, 0x17, 0x18, 0x25, 0xaf, 0xb0, 0xb0, 0xb1,
      0xb1, 0xb2, 0xb3, 0xb3, 0xb4, 0xb4, 0xb4, 0xb5, 0xb5, 0xb5, 0xb6, 0xb6, 0xb6, 0x2e, 0x2f,
      0x2f, 0x30, 0x19, 0x6c, 0x6d, 0x6d, 0x6d, 0xc6, 0xc7, 0xc7, 0xc9, 0xc9, 0x2a, 0x2b, 0x2b,
      0x2c, 0x2d, 0x2e, 0x2e, 0xc9, 0xb5, 0xb6, 0xb7, 0x6f, 0x6f, 0xca, 0x2d, 0x6f, 0x6f, 0x57,
      0x58, 0x58, 0x58, 0x58, 0x15, 0x16, 0x42, 0x42, 0x44, 0x44, 0x43, 0x44, 0x44, 0x98, 0x9d,
      0x42, 0x45, 0x10, 0x12, 0x14, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0b, 0x0c, 0x0d,
      0x0e, 0x0e, 0x0f, 0x10, 0x11, 0x11, 0x12, 0x13, 0x14, 0x14, 0x15, 0x16, 0x17, 0x18, 0x18,
      0x19, 0x1a, 0x1a, 0x1b, 0x1c, 0x1d, 0x1d, 0x25, 0x25, 0x26, 0x27, 0x28, 0x28, 0x29, 0x2a,
      0x2a, 0x2b, 0x2c, 0x2c, 0x2d, 0x2e, 0x2e, 0x2f, 0x2f, 0x30, 0x30, 0x31, 0x32, 0x32, 0x1b,
      0x1c, 0x1c, 0x1c, 0x3f, 0x3f, 0x40, 0x41, 0x41, 0x42, 0x43, 0x43, 0x44, 0x45, 0x45, 0x45,
      0x46, 0x46, 0x46, 0xf4, 0x47, 0xf4, 0xf4, 0xf4, 0xf4, 0xf5, 0x10, 0x55, 0x55, 0x56, 0x57,
      0x57, 0x58, 0x58, 0x59, 0x5a, 0x5a, 0x5b, 0x5b, 0x5b, 0x5c, 0x5d, 0x5d, 0x5d, 0x1d, 0x1d,
      0x1d, 0x1d, 0x1d, 0x25, 0x6d, 0x6d, 0x6e, 0x6f, 0x6f, 0x70, 0x71, 0x70, 0x70, 0xcb, 0xcb,
      0xcb, 0x2c, 0x2d, 0x2e, 0x2f, 0x2f, 0x2f, 0x30, 0x30, 0x31, 0x32, 0x83, 0x83, 0x84, 0x85,
      0x85, 0x86, 0x87, 0x88, 0x88, 0x89, 0x89, 0x8b, 0x8b, 0x8b, 0x8d, 0x8d, 0x8d, 0x8f, 0x8f,
      0xf5, 0xf5, 0x98, 0x98, 0x99, 0x9a, 0x9b, 0x9b, 0x9c, 0x9d, 0x9e, 0x9e, 0x9f, 0xa0, 0xa1,
      0xa2, 0xa3, 0xa3, 0xa4, 0xa5, 0xa5, 0xa6, 0x1a, 0x1a, 0x1b, 0xaf, 0x0d, 0xb0, 0xb1, 0xb2,
      0xb2, 0xb3, 0xb4, 0xb5, 0xb5, 0xb6, 0xb6, 0xb7, 0xb7, 0xb7, 0xb8, 0xb8, 0xb9, 0xb9, 0x32,
      0x32, 0x34, 0x34, 0x6d, 0x6d, 0xc6, 0xc7, 0xc8, 0xc9, 0xc9, 0xca, 0xca, 0x2c, 0x2d, 0x2e,
      0x2f, 0x30, 0x30, 0x31, 0xcc, 0xcd, 0xb9, 0xb9, 0x70, 0x70, 0xcc, 0x2f, 0x70, 0x71, 0x58,
      0x59, 0x5b, 0x5c, 0x5d, 0x5d, 0x19, 0x42, 0x44, 0x45, 0x45, 0x44, 0x45, 0x45, 0x99, 0x9f,
      0x44, 0x47, 0x12, 0xf2, 0xf3, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0d,
      0x0e, 0x0f, 0x10, 0x11, 0x11, 0x13, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x18, 0x1a, 0x1a,
      0x1b, 0x1c, 0x1c, 0x1e, 0x1e, 0x1f, 0x20, 0x25, 0x26, 0x27, 0x27, 0x28, 0x29, 0x2a, 0x2b,
      0x2b, 0x2c, 0x2d, 0x2e, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x32, 0x33, 0x34, 0x34, 0x35, 0x36,
      0x36, 0x1e, 0x1f, 0x3f, 0x40, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x45, 0x46, 0x47, 0x47,
      0x47, 0x48, 0x48, 0x48, 0x49, 0x49, 0x49, 0xa8, 0xa8, 0xa8, 0x55, 0x55, 0x56, 0x57, 0x58,
      0x58, 0x59, 0x5a, 0x5b, 0x5b, 0x5c, 0x5d, 0x5d, 0x5e, 0x5f, 0x5f, 0x60, 0x60, 0x60, 0x1f,
      0x20, 0x20, 0x20, 0x6c, 0x6d, 0xc6, 0x6e, 0x6f, 0x70, 0x71, 0x71, 0x71, 0x74, 0x75, 0x76,
      0x77, 0x78, 0x79, 0x2f, 0x30, 0x32, 0x32, 0x33, 0x33, 0x34, 0x34, 0x83, 0x84, 0x84, 0x85,
      0x86, 0x87, 0x88, 0x89, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8d, 0x8f, 0x8f, 0x90, 0x91, 0x92,
      0x93, 0x1e, 0x98, 0x99, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2,
      0xa3, 0xa4, 0xa5, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0x1c, 0x1d, 0xaf, 0xb0, 0xb1, 0xb1, 0xb2,
      0xb3, 0xb4, 0xb5, 0xb6, 0xb6, 0xb7, 0xb8, 0xb9, 0xb9, 0xb9, 0xba, 0xba, 0xbb, 0x32, 0x34,
      0x34, 0x36, 0x37, 0x6d, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xca, 0xcc, 0xcc, 0xcd, 0xcf, 0x2f,
      0x31, 0x32, 0x33, 0x34, 0xce, 0xce, 0xbb, 0xbc, 0x71, 0x71, 0x76, 0x31, 0xde, 0xde, 0xdf,
      0xe0, 0xe1, 0xe2, 0x5f, 0xe3, 0xe4, 0x43, 0x44, 0x46, 0x47, 0x45, 0x47, 0x46, 0x99, 0x41,
      0x45, 0x49, 0xf2, 0x16, 0xf3, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00}},
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0b, 0x0c, 0x0c,
      0x0d, 0x0e, 0x0e, 0x0f, 0x10, 0x10, 0x11, 0x12, 0x12, 0x13, 0x14, 0xf2, 0xf2, 0x16, 0x16,
      0x17, 0xf3, 0xf3, 0x19, 0xf4, 0xf4, 0xf4, 0x0b, 0x25, 0x26, 0x26, 0x27, 0x28, 0x28, 0x29,
      0xb2, 0x12, 0x13, 0x14, 0x14, 0x15, 0x15, 0x16, 0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17,
      0x17, 0x18, 0xf4, 0x3f, 0x3f, 0x40, 0x40, 0x41, 0x41, 0x42, 0x42, 0x43, 0x43, 0x44, 0x44,
      0x44, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x46, 0x46, 0x46, 0x0f, 0x0f, 0x10, 0x55, 0x56,
      0x12, 0x13, 0x13, 0x14, 0x9f, 0x15, 0x15, 0xa1, 0x16, 0xa3, 0xa3, 0xa3, 0x17, 0x17, 0xa5,
      0xa5, 0xf4, 0xf4, 0x25, 0x6c, 0x26, 0x6d, 0x6d, 0x6e, 0xc7, 0xc8, 0xc7, 0x28, 0x29, 0x2a,
      0x2a, 0x2a, 0x2b, 0x2b, 0x2c, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17, 0x83, 0x83, 0x84, 0x84,
      0x85, 0x85, 0x86, 0x86, 0x87, 0x87, 0x88, 0x88, 0x89, 0x89, 0x8a, 0x8a, 0x8b, 0x8b, 0x8c,
      0x8d, 0xf4, 0x98, 0x98, 0x99, 0x99, 0x9a, 0x9a, 0x9b, 0x9b, 0x9c, 0x9c, 0x9d, 0x9e, 0x9e,
      0x9f, 0xf2, 0x9f, 0xa1, 0xa1, 0xf3, 0xf3, 0xf3, 0xf3, 0xa4, 0x0b, 0xaf, 0xb0, 0xb0, 0xb1,
      0xb1, 0xb2, 0xb3, 0xb3, 0xb4, 0xb4, 0xb5, 0xb5, 0xb6, 0xb6, 0xb6, 0xb6, 0xb7, 0x16, 0x17,
      0x17, 0x17, 0x17, 0x6c, 0x26, 0x26, 0x27, 0x27, 0x28, 0x28, 0x29, 0xb2, 0xb4, 0x2a, 0x2c,
      0x2d, 0x17, 0x17, 0x17, 0xb4, 0xb5, 0xb5, 0xb7, 0x6e, 0xc8, 0x2a, 0x2d, 0x55, 0x56, 0x57,
      0x57, 0x57, 0x15, 0x16, 0x16, 0x17, 0x42, 0x42, 0x43, 0x44, 0x43, 0x44, 0x44, 0x99, 0x9e,
      0x43, 0x46, 0x40, 0x41, 0xf2, 0xf3, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f,
      0x0a},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0b, 0x0c, 0x0d,
      0x0e, 0x0e, 0x0f, 0x10, 0x10, 0x11, 0x12, 0x13, 0x13, 0x14, 0xf2, 0x16, 0x16, 0xf3, 0xf3,
      0xf3, 0xf4, 0xf4, 0xf4, 0xf4, 0xf5, 0xf5, 0x25, 0x25, 0x26, 0xb0, 0x27, 0xb1, 0x29, 0x29,
      0x2a, 0xb3, 0x14, 0xb4, 0x2d, 0x16, 0x17, 0x17, 0x18, 0x18, 0x18, 0x18, 0x19, 0x19, 0x19,
      0x19, 0x1a, 0x1a, 0x3f, 0x3f, 0x40, 0x40, 0x41, 0x42, 0x43, 0x43, 0x44, 0x44, 0x45, 0x45,
      0x46, 0x46, 0x46, 0x46, 0x46, 0x47, 0x47, 0x47, 0x47, 0x47, 0x0f, 0x55, 0x55, 0x56, 0x56,
      0x57, 0x14, 0x58, 0x59, 0x16, 0xa1, 0xa2, 0xa3, 0xa3, 0xa4, 0xa4, 0xa5, 0xa5, 0xa6, 0xa6,
      0xa7, 0xa7, 0xa7, 0x25, 0x6c, 0x6d, 0x6d, 0x6e, 0xc7, 0x6f, 0x6f, 0xc8, 0xc9, 0x29, 0x2b,
      0x2b, 0x2b, 0x2c, 0x2c, 0x2e, 0x2e, 0x18, 0x18, 0x19, 0x19, 0x19, 0x83, 0x83, 0x84, 0x84,
      0x85, 0x86, 0x87, 0x87, 0x88, 0x88, 0x89, 0x8a, 0x8a, 0x8b, 0x8b, 0x8c, 0x8c, 0x8d, 0x8d,
      0x8e, 0x8f, 0x98, 0x98, 0x99, 0x99, 0x9a, 0x9b, 0x9b, 0x9c, 0x9c, 0x9d, 0x9e, 0x9e, 0x9f,
      0x9f, 0xa1, 0xa1, 0xa2, 0xa2, 0xa3, 0xa4, 0xf4, 0xf4, 0xf4, 0xaf, 0xaf, 0xb0, 0xb1, 0xb1,
      0xb2, 0xb3, 0xb3, 0xb4, 0xb5, 0xb5, 0xb6, 0xb6, 0xb7, 0xb7, 0xb7, 0xb8, 0xb8, 0xb8, 0x19,
      0x19, 0x19, 0x19, 0x6c, 0x6d, 0x6d, 0x27, 0x28, 0x29, 0x29, 0x2a, 0x2a, 0x2b, 0x2b, 0x2d,
      0x2e, 0x2f, 0x19, 0x19, 0xb5, 0xb6, 0xb7, 0xb9, 0x6f, 0x6f, 0x2a, 0x2e, 0x6f, 0x57, 0x58,
      0x58, 0x58, 0x16, 0x17, 0x18, 0x19, 0x42, 0x43, 0x44, 0x45, 0x44, 0x45, 0x45, 0x99, 0x00,
      0x44, 0x47, 0x41, 0xf2, 0xf2, 0xf3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x0a},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0b, 0x0d, 0x0d,
      0x0e, 0x0f, 0x10, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x15, 0x16, 0x17, 0xf3, 0x19, 0xf4,
      0xf4, 0xf4, 0x1c, 0xf5, 0xf5, 0xf5, 0x1f, 0x25, 0x26, 0x26, 0x27, 0x28, 0x29, 0x29, 0x2a,
      0x2a, 0xb4, 0x2d, 0x2e, 0x2e, 0x2f, 0x2f, 0x19, 0x1a, 0x1a, 0x1a, 0x1a, 0x1b, 0x1b, 0x1b,
      0x1b, 0x1b, 0xf5, 0x3f, 0x3f, 0x40, 0x41, 0x42, 0x42, 0x43, 0x44, 0x45, 0x45, 0x46, 0x46,
      0x47, 0x47, 0x48, 0x48, 0x48, 0x48, 0x48, 0x49, 0x49, 0x49, 0x10, 0x55, 0x55, 0x56, 0x57,
      0x58, 0x58, 0x5a, 0x5a, 0x5b, 0x5b, 0xa3, 0xa4, 0xa4, 0xa5, 0xa6, 0xa6, 0xa7, 0xa7, 0xa8,
      0xa9, 0xa9, 0xaa, 0x25, 0x6c, 0x6d, 0xc6, 0xc7, 0x6f, 0x70, 0x70, 0xc9, 0xca, 0xca, 0x2b,
      0x2c, 0x2d, 0x2d, 0x2e, 0x2e, 0x2f, 0x30, 0x1a, 0x1b, 0x1b, 0x1b, 0x83, 0x83, 0x84, 0x85,
      0x86, 0x87, 0x87, 0x88, 0x89, 0x89, 0x8a, 0x8b, 0x8b, 0x8c, 0x8d, 0x8d, 0x8e, 0x8f, 0x8f,
      0x90, 0x91, 0x98, 0x98, 0x99, 0x9a, 0x9b, 0x9b, 0x9c, 0x9c, 0x9e, 0x9e, 0x9f, 0xa0, 0xa0,
      0xa1, 0xa2, 0xa3, 0xa4, 0xa4, 0xa5, 0xa6, 0xa6, 0xa7, 0xa7, 0xaf, 0xb0, 0xb0, 0xb1, 0xb2,
      0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb6, 0xb7, 0xb8, 0xb8, 0xb8, 0xb9, 0xba, 0xba, 0x32, 0x33,
      0x1b, 0x1b, 0x1b, 0x6d, 0x6d, 0x6d, 0x28, 0x28, 0x29, 0x2a, 0x2b, 0x2b, 0x2c, 0x2d, 0x2e,
      0x2f, 0x31, 0x1b, 0x1b, 0xb6, 0xb7, 0xb8, 0xbb, 0x70, 0x70, 0x2b, 0x2f, 0x70, 0x57, 0x59,
      0x5a, 0x5b, 0x5b, 0x18, 0x1a, 0x1a, 0x43, 0x44, 0x45, 0x46, 0x45, 0x46, 0x46, 0x99, 0x00,
      0x00, 0x49, 0x41, 0xf2, 0xf3, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x0a},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0d,
      0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x15, 0x17, 0xf3, 0x18, 0x19, 0xf4, 0xf4,
      0x1b, 0xf5, 0xf5, 0x1f, 0xaa, 0x95, 0x95, 0x25, 0x26, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2a,
      0x2c, 0x2d, 0x2e, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x1c, 0x1c, 0x1c, 0x1d, 0x1d, 0x1d,
      0x1d, 0x1d, 0x1e, 0x3f, 0x40, 0x41, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x46, 0x47, 0x48,
      0x48, 0x49, 0x49, 0x4a, 0x4a, 0x4b, 0x4b, 0x4c, 0x4c, 0x4c, 0x55, 0x55, 0x56, 0x57, 0x58,
      0x59, 0x5a, 0x5b, 0x5b, 0x5c, 0x5d, 0x5d, 0xa6, 0xa6, 0x60, 0xa8, 0xa8, 0xa9, 0xaa, 0xaa,
      0xab, 0xab, 0xab, 0x25, 0x6d, 0x6d, 0x6e, 0xc8, 0x6f, 0x70, 0x71, 0xca, 0xca, 0xcb, 0x2c,
      0x2d, 0x2e, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x1d, 0x1d, 0x83, 0x84, 0x85, 0x85,
      0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8b, 0x8c, 0x8d, 0x8e, 0x8e, 0x90, 0x90, 0x91, 0x91,
      0x92, 0x93, 0x98, 0x99, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0x9f, 0xa0, 0xa1, 0xa2,
      0xa3, 0xa4, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa8, 0xa8, 0xa9, 0xaf, 0xb0, 0xb1, 0xb1, 0xb2,
      0xb3, 0xb4, 0xb5, 0xb6, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xba, 0xbb, 0xbb, 0xbd, 0xbd, 0xc1,
      0xc1, 0x92, 0x92, 0x6d, 0x6d, 0xc6, 0xc7, 0xc9, 0xc9, 0xc9, 0x2c, 0xcd, 0x2d, 0x2e, 0x2f,
      0x30, 0x32, 0x34, 0x1d, 0xb7, 0xb9, 0xba, 0xbd, 0x70, 0x70, 0xcd, 0x31, 0xde, 0x58, 0x59,
      0x5b, 0x5c, 0x5d, 0x1a, 0x1b, 0x1c, 0x43, 0x44, 0x46, 0x47, 0x45, 0x47, 0x47, 0x9a, 0x00,
      0x00, 0x4b, 0xf2, 0xf2, 0xf3, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x0a}}
};
DATA(0x004f9910) u8 gColorTableLighten[DIM_PALETTE_COLOR_COUNT] = {
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
    0xef, 0xf0, 0xf2, 0xf2, 0xf3, 0xf4, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xFF
};
DATA(0x004f9a10) u8 gColorTableNoCycle[DIM_PALETTE_COLOR_COUNT] = {
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
    0x45, 0x45, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xFF
};
DATA(0x005265e8) font* smallFont = NULL;
DATA(0x005265ec) font* bigFont = NULL;
DATA(0x005265f0) b32 gbReturnAfterComputeExtent = false;
DATA(0x004f9b10) b32 gbAllowTextEntryEscape = true;
DATA(0x005265f4) WindowColorCycleMode giCycleType = WINDOW_COLOR_CYCLE_DEFAULT;
DATA(0x004f9b14) b32 giScreenScroll = true;
DATA(0x004f9b18) i32 giMenuCommand = -1;
DATA(0x005265f8) b32 gbSendMouseMoveMessages = false;
DATA(0x004f9b1c) b32 gbColorMice = true;
DATA(0x004f9b20) u32l gTownEligibleBuildMask[TOWN_ELIGIBLE_BUILD_MASK_COUNT] = {
    TOWN_ELIGIBLE_BUILD_KNIGHT_MASK,
    TOWN_ELIGIBLE_BUILD_BARBARIAN_MASK,
    TOWN_ELIGIBLE_BUILD_SORCERESS_MASK,
    TOWN_ELIGIBLE_BUILD_WARLOCK_MASK,
    TOWN_ELIGIBLE_BUILD_WIZARD_MASK,
    TOWN_ELIGIBLE_BUILD_NECROMANCER_MASK
};
DATA(0x004f9b38) u8 giMapSizes[KB_MAP_SIZE_COUNT] =
    {MAP_DIMENSION_SMALL, MAP_DIMENSION_MEDIUM, MAP_DIMENSION_LARGE, MAP_DIMENSION_XLARGE};
DATA(0x005265fc) b32 gbUseEvilInterface = false;
DATA(0x004f9b3c) H2_CONST char* cEvilTranslate[KB_INTERFACE_TYPE_COUNT][KB_INTERFACE_VARIANT_COUNT] = {
    {
        "advbord.icn",
        "advborde.icn"
    },
    {
        "heroextg.icn",
        "heroexte.icn"
    },
    {
        "buybuild.icn",
        "buybuile.icn"
    },
    {
        "advbtns.icn",
        "advebtns.icn"
    },
    {
        "herologo.icn",
        "herologe.icn"
    },
    {
        "sunmoon.icn",
        "sunmoone.icn"
    },
    {
        "stonback.icn",
        "stonbake.icn"
    },
    {
        "scroll.icn",
        "scrolle.icn"
    },
    {
        "locators.icn",
        "locatore.icn"
    },
    {
        "system.icn",
        "systeme.icn"
    },
    {
        "CPANBKG.ICN",
        "CPANBKGE.ICN"
    },
    {
        "CPANEL.ICN",
        "CPANELE.ICN"
    },
    {
        "APANBKG.ICN",
        "APANBKGE.ICN"
    },
    {
        "APANEL.ICN",
        "APANELE.ICN"
    },
    {
        "VIEWWRLD.ICN",
        "EVIWWRLD.ICN"
    },
    {
        "VIEWRSRC.ICN",
        "EVIWRSRC.ICN"
    },
    {
        "VIEWRTFX.ICN",
        "EVIWRTFX.ICN"
    },
    {
        "VIEWTWNS.ICN",
        "EVIWTWNS.ICN"
    },
    {
        "VIEWHROS.ICN",
        "EVIWHROS.ICN"
    },
    {
        "VIEW_ALL.ICN",
        "EVIW_ALL.ICN"
    },
    {
        "VIEWMINE.ICN",
        "EVIWMINE.ICN"
    },
    {
        "VIEWDDOR.ICN",
        "EVIWDDOR.ICN"
    },
    {
        "VIEWPUZL.ICN",
        "EVIWPUZL.ICN"
    },
    {
        "LGNDXTRA.ICN",
        "LGNDXTRE.ICN"
    },
    {
        "SPANBKG.ICN",
        "SPANBKGE.ICN"
    },
    {
        "SPANBTN.ICN",
        "SPANBTNE.ICN"
    },
    {
        "CSPANBKG.ICN",
        "CSPANBKE.ICN"
    },
    {
        "CSPANBTN.ICN",
        "CSPANBTE.ICN"
    },
    {
        "TRADPOST.ICN",
        "TRADPOSE.ICN"
    },
    {
        "VIEWARMY.ICN",
        "VIEWARME.ICN"
    },
    {
        "WINLOSE.ICN",
        "WINLOSEE.ICN"
    },
    {
        "WINCMBTB.ICN",
        "WINCMBBE.ICN"
    },
    {
        "SURRENDR.ICN",
        "SURRENDE.ICN"
    },
    {
        "SURDRBKG.ICN",
        "SURDRBKE.ICN"
    },
    {
        "VGENBKG.ICN",
        "VGENBKGE.ICN"
    },
    {
        "campbkgg.ICN",
        "campbkge.ICN"
    },
    {
        "campxtrg.ICN",
        "campxtre.ICN"
    }
};
DATA(0x004f9c64) char gcAnimPath[GLOBAL_AGGREGATE_PATH_SIZE] = "\\ANIM2\\";
DATA(0x004f9dc4) char gcGamePath[GLOBAL_GAME_PATH_SIZE] = ".\\GAMES\\";
DATA(0x004f9dd8) char gcMapPath[GLOBAL_MAP_PATH_SIZE] = ".\\MAPS\\";
DATA(0x004f9dec) char gcMusicPath[GLOBAL_AGGREGATE_PATH_SIZE] = "\\TRACKS2\\";
DATA(0x00526600) i32 gbPutzingWithMouseCtr = 0;
DATA(0x004f9f4c) float gfCombatSpeedMod[KB_COMBAT_SPEED_COUNT] = {1.0f, 0.7f, 0.35f};
DATA(0x00526604) icon* gShingleAnim = NULL;
DATA(0x00526608) i32 iNextShingleAnim = 0;
DATA(0x0052660c) i32 giDialogTimeout = 0;
DATA(0x00526610) i32 giNewMonsterCycleFrame = 0;
DATA(0x00526614) b32 gbNoCDRom = false;
DATA(0x00526618) b32 gbLeaveNetBoxAlone = false;
DATA(0x004f9f58) b32 gbDrawWindowBackground = true;
DATA(0x0052661c) b32 gbCheatMenus = false;
DATA(0x00526620) b32 gbUseWaveout = false;
DATA(0x00526624) b32 gbShowAllMaps = false;
DATA(0x004f9f5c) H2_CONST char* gCombatFxNames[KB_COMBAT_FX_COUNT] = {
    "",
    "magic01.icn",
    "magic02.icn",
    "magic03.icn",
    "magic04.icn",
    "magic05.icn",
    "magic06.icn",
    "magic07.icn",
    "magic08.icn",
    "rainbluk.icn",
    "cloudluk.icn",
    "moraleg.icn",
    "moraleb.icn",
    "reddeath.icn",
    "redfire.icn",
    "sparks.icn",
    "electric.icn",
    "physical.icn",
    "bluefire.icn",
    "icecloud.icn",
    "lichclod.icn",
    "bless.icn",
    "berzerk.icn",
    "shield.icn",
    "haste.icn",
    "paralyze.icn",
    "hypnotiz.icn",
    "dragslay.icn",
    "blind.icn",
    "curse.icn",
    "stonskin.icn",
    "stelskin.icn"
};
DATA(0x004f9fdc) i16 horseFrameFlip[MOVEMENT_FRAME_FLIP_COUNT] =
    {45, 46, 47, 48, 49, 50, 51, 52, 53, 179, 178, 177, 54, 175, 174, 55};
DATA(0x004f9ffc) i16 boatFrameFlip[MOVEMENT_FRAME_FLIP_COUNT] =
    {0, 0, 9, 9, 18, 18, 27, 27, 36, 36, 155, 155, 146, 146, 137, 137};
DATA(0x004fa01c) i8 gCastleResources[CASTLE_RESOURCE_SLOT_COUNT] =
    {IDX(RES_WOOD), IDX(RES_ORE), -1, -1};
DATA(0x004fa020) i16 gCastleAmounts[CASTLE_AMOUNT_COUNT] = {20, 20, 0, 0};
DATA(0x004fa028) i32 gHeroGoldCost = HERO_RECRUITMENT_GOLD_COST;
DATA(0x004fa02c) i16 gVesaMode[VESA_MODE_VALUE_COUNT] =
    {640, 480, 256, VESA_SET_MODE_FUNCTION, VESA_MODE_640_480_256, 0};
DATA(0x004fa038) tag_tilePoint normalDirTable[NORMAL_DIRECTION_COUNT] = {
    {0, -1, 16},
    {1, -1, 16},
    {1, 0, 16},
    {1, 1, 16},
    {0, 1, 16},
    {-1, 1, 16},
    {-1, 0, 16},
    {-1, -1, 16}
};
DATA(0x004fa058) i32 gResourceBaseValue[RESOURCE_VALUE_COUNT] = {200, 300, 200, 300, 300, 300, 1};
DATA(0x004fa074) i32 gInitResourcesHuman[STARTING_RESOURCE_DIFFICULTY_COUNT][STARTING_RESOURCE_TYPE_COUNT] = {
    {30, 10, 30, 10, 10, 10, 10000},
    {20, 5, 20, 5, 5, 5, 7500},
    {10, 2, 10, 2, 2, 2, 5000},
    {5, 0, 5, 0, 0, 0, 2500},
    {0, 0, 0, 0, 0, 0, 0}
};
DATA(0x004fa100) i32 gInitResourcesComputer[STARTING_RESOURCE_DIFFICULTY_COUNT][STARTING_RESOURCE_TYPE_COUNT] = {
    {20, 5, 20, 5, 5, 5, 7500},
    {20, 5, 20, 5, 5, 5, 7500},
    {30, 10, 30, 10, 10, 10, 10000},
    {30, 10, 30, 10, 10, 10, 10000},
    {30, 10, 30, 10, 10, 10, 10000}
};
DATA(0x004fa18c) i32 gMineCharacteristics[MINE_CHARACTERISTIC_COUNT] = {2, 1, 2, 1, 1, 1, 1000};
DATA(0x004fa1a8) i32 gSSValues[IDX(HERO_SKILL_COUNT)][SECONDARY_SKILL_VALUE_LEVEL_COUNT] = {
    {400, 750, 1000},
    {200, 450, 850},
    {450, 1000, 1675},
    {1, 2, 3},
    {50, 100, 150},
    {150, 275, 375},
    {300, 550, 800},
    {300, 600, 900},
    {250, 425, 650},
    {300, 550, 800},
    {100, 200, 300},
    {50, 100, 150},
    {100, 450, 950},
    {445, 950, 1500}
};
DATA(0x004fa250) H2_ENUM_STORAGE(ArtifactLevelMask, u8)
gArtifactLevel[KB_ARTIFACT_LEVEL_COUNT] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x04, 0x04, 0x02, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x02, 0x04, 0x02, 0x04, 0x02, 0x04, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x04, 0x02, 0x02, 0x08, 0x08, 0x08, 0x08, 0x02, 0x04, 0x04, 0x08, 0x04, 0x04,
    0x08, 0x08, 0x08, 0x04, 0x04, 0x04, 0x08, 0x04, 0x04, 0x04, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x02, 0x08, 0x08, 0x08, 0x02, 0x04, 0x04, 0x08, 0x08, 0x02, 0x02, 0x02, 0x04,
    0x04, 0x02, 0x04, 0x02, 0x04, 0x02, 0x10, 0x20, 0x20, 0x20, 0x20, 0x02, 0x08, 0x02, 0x08,
    0x02, 0x02, 0x08, 0x08, 0x02, 0x02, 0x02, 0x04, 0x02, 0x02, 0x02, 0x02, 0x04, 0x00
};
DATA(0x004fa2b8) i32 gArtifactBaseRV[KB_ARTIFACT_BASE_VALUE_COUNT] = {
    13600, 22000, 18000, 14000, 19000, 18500, 22200, 25000, 6000,  4000, 4000,  5600,  1200,
    1200,  1200,  1200,  -1200, 2000,  1800,  1800,  2000,  1000,  3600, 5600,  4000,  5040,
    3060,  4420,  5610,  6630,  7000,  6000,  4000,  4500,  2250,  1200, 1200,  1200,  1200,
    3500,  1500,  500,   1000,  750,   500,   750,   750,   750,   750,  750,   750,   500,
    1500,  1000,  1000,  750,   750,   750,   750,   750,   750,   750,  1250,  750,   150,
    250,   3500,  1250,  250,   -1,    -1,    4000,  4000,  4000,  2000, 2000,  4000,  3800,
    7600,  3700,  7400,  0,     0,     0,     0,     0,     2500,  4800, 9000,  -3250, 36200,
    2000,  -1050, -1050, 10000, 10000, 15000, 720,   7500,  10000, 9200, 10000, 1500
};
DATA(0x004fa454) i32 gUltArtifactAvgValue = ULTIMATE_ARTIFACT_AVERAGE_VALUE;
DATA(0x00526628) i32 giDebugLevel = 0;
DATA(0x004fa458) i8 giVisRangeTown = TOWN_VISIBILITY_RADIUS;
DATA(0x004fa460) tag_monsterInfo gMonsterDatabase[IDX(CREATURE_COUNT)] = {
    {{20, {33}}, 17, 12, 1, FACTION_KNIGHT, 2, 1, 1, 1, 1, 0, "psnt", {MONSTER_FLAGS_NONE}},
    {{150, {312}}, 21, 8, 10, FACTION_KNIGHT, 2, 5, 3, 2, 3, 12, "arch", {MONSTER_ATTRIBUTE_RANGED}},
    {{200, {463}}, 23, 8, 10, FACTION_KNIGHT, 4, 5, 3, 2, 3, 24, "arch", {MONSTER_ATTRIBUTE_RANGED}},
    {{200, {639}}, 32, 5, 15, FACTION_KNIGHT, 4, 5, 9, 3, 4, 0, "pike", {MONSTER_FLAGS_NONE}},
    {{250, {824}}, 33, 5, 20, FACTION_KNIGHT, 5, 5, 9, 3, 4, 0, "pike", {MONSTER_FLAGS_NONE}},
    {{250, {1130}}, 45, 4, 25, FACTION_KNIGHT, 4, 7, 9, 4, 6, 0, "swdm", {MONSTER_FLAGS_NONE}},
    {{300, {1350}}, 45, 4, 30, FACTION_KNIGHT, 5, 7, 9, 4, 6, 0, "swdm", {MONSTER_FLAGS_NONE}},
    {{300, {1830}}, 61, 3, 30, FACTION_KNIGHT, 6, 10, 9, 5, 10, 0, "cavl", {MONSTER_ATTRIBUTE_WIDE}},
    {{375, {2273}}, 61, 3, 40, FACTION_KNIGHT, 7, 10, 9, 5, 10, 0, "cavl", {MONSTER_ATTRIBUTE_WIDE}},
    {{600, {4704}}, 78, 2, 50, FACTION_KNIGHT, 5, 11, 12, 10, 20, 0, "pldn", {MONSTER_FLAGS_NONE}},
    {{1000, {5822}}, 58, 2, 65, FACTION_KNIGHT, 6, 11, 12, 10, 20, 0, "pldn", {MONSTER_FLAGS_NONE}},
    {{40, {109}}, 27, 10, 3, FACTION_BARBARIAN, 4, 3, 1, 1, 2, 0, "gbln", {MONSTER_FLAGS_NONE}},
    {{140, {299}}, 21, 8, 10, FACTION_BARBARIAN, 2, 3, 4, 2, 3, 8, "elf_", {MONSTER_ATTRIBUTE_RANGED}},
    {{175, {512}}, 29, 8, 15, FACTION_BARBARIAN, 3, 3, 4, 3, 4, 16, "elf_", {MONSTER_ATTRIBUTE_RANGED}},
    {{200, {865}}, 43, 5, 20, FACTION_BARBARIAN, 6, 6, 2, 3, 5, 0, "wolf", {MONSTER_ATTRIBUTE_WIDE}},
    {{300, {1065}}, 36, 4, 40, FACTION_BARBARIAN, 2, 9, 5, 4, 6, 0, "ogre", {MONSTER_FLAGS_NONE}},
    {{500, {2070}}, 41, 4, 60, FACTION_BARBARIAN, 4, 9, 5, 5, 7, 0, "ogre", {MONSTER_FLAGS_NONE}},
    {{600, {1921}},
     32,
     3,
     40,
     FACTION_BARBARIAN,
     4,
     10,
     5,
     5,
     7,
     8,
     "trll",
     {MONSTER_ATTRIBUTE_RANGED}},
    {{700, {2337}},
     33,
     3,
     40,
     FACTION_BARBARIAN,
     5,
     10,
     5,
     7,
     9,
     16,
     "trll",
     {MONSTER_ATTRIBUTE_RANGED}},
    {{750, {6074}},
     58,
     2,
     80,
     FACTION_BARBARIAN,
     5,
     12,
     9,
     12,
     24,
     0,
     "cycl",
     {MONSTER_ATTRIBUTE_TWO_HEX_ATTACKER}},
    {{50, {129}}, 26, 8, 2, FACTION_SORCERESS, 4, 4, 2, 1, 2, 0, "sprt", {MONSTER_ATTRIBUTE_FLYING}},
    {{200, {500}}, 25, 6, 20, FACTION_SORCERESS, 2, 6, 5, 2, 4, 0, "dwrf", {MONSTER_FLAGS_NONE}},
    {{250, {716}}, 29, 6, 20, FACTION_SORCERESS, 4, 6, 6, 2, 4, 0, "dwrf", {MONSTER_FLAGS_NONE}},
    {{250, {554}}, 22, 4, 15, FACTION_SORCERESS, 4, 4, 3, 2, 3, 24, "elf_", {MONSTER_ATTRIBUTE_RANGED}},
    {{300, {658}}, 22, 4, 15, FACTION_SORCERESS, 6, 5, 5, 2, 3, 24, "elf_", {MONSTER_ATTRIBUTE_RANGED}},
    {{350, {1290}}, 37, 3, 25, FACTION_SORCERESS, 5, 7, 5, 5, 8, 8, "drui", {MONSTER_ATTRIBUTE_RANGED}},
    {{400, {1428}},
     36,
     3,
     25,
     FACTION_SORCERESS,
     6,
     7,
     7,
     5,
     8,
     16,
     "drui",
     {MONSTER_ATTRIBUTE_RANGED}},
    {{500, {2702}}, 54, 2, 40, FACTION_SORCERESS, 5, 10, 9, 7, 14, 0, "unic", {MONSTER_ATTRIBUTE_WIDE}},
    {{1500, {10114}},
     56,
     1,
     100,
     FACTION_SORCERESS,
     7,
     12,
     10,
     20,
     40,
     0,
     "phoe",
     {MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_TWO_HEX_ATTACKER}},
    {{60, {154}},
     26,
     8,
     5,
     FACTION_WARLOCK,
     4,
     3,
     1,
     1,
     2,
     8,
     "cntr",
     {MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_RANGED}},
    {{200, {579}}, 29, 6, 15, FACTION_WARLOCK, 6, 4, 7, 2, 3, 0, "garg", {MONSTER_ATTRIBUTE_FLYING}},
    {{300, {1101}},
     37,
     4,
     25,
     FACTION_WARLOCK,
     4,
     6,
     6,
     3,
     5,
     0,
     "grif",
     {MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING}},
    {{400, {1751}}, 44, 3, 35, FACTION_WARLOCK, 4, 9, 8, 5, 10, 0, "mino", {MONSTER_FLAGS_NONE}},
    {{500, {2252}}, 45, 3, 45, FACTION_WARLOCK, 6, 9, 8, 5, 10, 0, "mino", {MONSTER_FLAGS_NONE}},
    {{800, {2878}}, 36, 2, 75, FACTION_WARLOCK, 2, 8, 9, 6, 12, 0, "hydr", {MONSTER_ATTRIBUTE_WIDE}},
    {{3000, {18153}},
     55,
     1,
     200,
     FACTION_WARLOCK,
     4,
     12,
     12,
     25,
     50,
     0,
     "drgn",
     {MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_TWO_HEX_ATTACKER}},
    {{3500, {22962}},
     68,
     1,
     250,
     FACTION_WARLOCK,
     5,
     13,
     13,
     25,
     50,
     0,
     "drgn",
     {MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_TWO_HEX_ATTACKER}},
    {{4000, {28144}},
     74,
     1,
     300,
     FACTION_WARLOCK,
     6,
     14,
     14,
     25,
     50,
     0,
     "drgn",
     {MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_TWO_HEX_ATTACKER}},
    {{50, {134}}, 27, 8, 3, FACTION_WIZARD, 3, 2, 1, 1, 3, 12, "half", {MONSTER_ATTRIBUTE_RANGED}},
    {{150, {493}}, 33, 6, 15, FACTION_WIZARD, 6, 5, 4, 2, 3, 0, "boar", {MONSTER_ATTRIBUTE_WIDE}},
    {{300, {951}}, 19, 4, 30, FACTION_WIZARD, 2, 5, 10, 4, 5, 0, "golm", {MONSTER_FLAGS_NONE}},
    {{350, {1324}}, 24, 4, 35, FACTION_WIZARD, 3, 7, 10, 4, 5, 0, "golm", {MONSTER_FLAGS_NONE}},
    {{400, {1739}},
     43,
     3,
     40,
     FACTION_WIZARD,
     4,
     7,
     7,
     4,
     8,
     0,
     "roc_",
     {MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING}},
    {{600, {1935}}, 32, 2, 30, FACTION_WIZARD, 5, 11, 7, 7, 9, 12, "mage", {MONSTER_ATTRIBUTE_RANGED}},
    {{700, {2469}}, 35, 2, 35, FACTION_WIZARD, 6, 12, 8, 7, 9, 24, "mage", {MONSTER_ATTRIBUTE_RANGED}},
    {{2000, {9589}}, 42, 1, 150, FACTION_WIZARD, 4, 13, 10, 20, 30, 0, "titn", {MONSTER_FLAGS_NONE}},
    {{5000, {22933}},
     79,
     1,
     300,
     FACTION_WIZARD,
     6,
     15,
     15,
     20,
     30,
     24,
     "titn",
     {MONSTER_ATTRIBUTE_RANGED}},
    {{75, {203}}, 27, 8, 4, FACTION_NECROMANCER, 4, 4, 3, 2, 3, 0, "skel", {MONSTER_ATTRIBUTE_UNDEAD}},
    {{150, {310}},
     21,
     6,
     15,
     FACTION_NECROMANCER,
     2,
     5,
     2,
     2,
     3,
     0,
     "zomb",
     {MONSTER_ATTRIBUTE_UNDEAD}},
    {{200, {506}},
     25,
     6,
     20,
     FACTION_NECROMANCER,
     4,
     5,
     2,
     2,
     3,
     0,
     "zomb",
     {MONSTER_ATTRIBUTE_UNDEAD}},
    {{250, {868}},
     35,
     4,
     25,
     FACTION_NECROMANCER,
     4,
     6,
     6,
     3,
     4,
     0,
     "mumy",
     {MONSTER_ATTRIBUTE_UNDEAD}},
    {{300, {1056}},
     35,
     4,
     30,
     FACTION_NECROMANCER,
     5,
     6,
     6,
     3,
     4,
     0,
     "mumy",
     {MONSTER_ATTRIBUTE_UNDEAD}},
    {{500, {1685}},
     42,
     3,
     30,
     FACTION_NECROMANCER,
     4,
     8,
     6,
     5,
     7,
     0,
     "vamp",
     {MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_UNDEAD}},
    {{650, {2461}},
     45,
     3,
     40,
     FACTION_NECROMANCER,
     5,
     8,
     6,
     5,
     7,
     0,
     "vamp",
     {MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_UNDEAD}},
    {{750, {2069}},
     28,
     2,
     25,
     FACTION_NECROMANCER,
     5,
     7,
     12,
     8,
     10,
     12,
     "lich",
     {MONSTER_ATTRIBUTE_RANGED | MONSTER_ATTRIBUTE_UNDEAD}},
    {{900, {2625}},
     29,
     2,
     35,
     FACTION_NECROMANCER,
     6,
     7,
     13,
     8,
     10,
     24,
     "lich",
     {MONSTER_ATTRIBUTE_RANGED | MONSTER_ATTRIBUTE_UNDEAD}},
    {{1500, {11744}},
     78,
     1,
     150,
     FACTION_NECROMANCER,
     4,
     11,
     9,
     25,
     45,
     0,
     "drgn",
     {MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_UNDEAD}},
    {{50, {177}}, 35, 12, 4, FACTION_NEUTRAL, 5, 6, 1, 1, 2, 0, "rogu", {MONSTER_FLAGS_NONE}},
    {{200, {805}}, 40, 4, 20, FACTION_NEUTRAL, 6, 7, 6, 2, 5, 0, "nmad", {MONSTER_ATTRIBUTE_WIDE}},
    {{1000, {1545}},
     62,
     3,
     20,
     FACTION_NEUTRAL,
     5,
     8,
     7,
     4,
     6,
     0,
     "ghst",
     {MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_UNDEAD}},
    {{650, {5692}},
     60,
     2,
     50,
     FACTION_NEUTRAL,
     6,
     10,
     9,
     20,
     30,
     0,
     "geni",
     {MONSTER_ATTRIBUTE_FLYING}},
    {{500, {1979}}, 40, 5, 35, FACTION_NEUTRAL, 4, 8, 9, 6, 10, 0, "meds", {MONSTER_ATTRIBUTE_WIDE}},
    {{500, {1732}}, 35, 3, 50, FACTION_NEUTRAL, 3, 8, 8, 4, 5, 0, "eelm", {MONSTER_FLAGS_NONE}},
    {{500, {1412}}, 28, 3, 35, FACTION_NEUTRAL, 6, 7, 7, 2, 8, 0, "aelm", {MONSTER_FLAGS_NONE}},
    {{500, {1501}}, 30, 3, 40, FACTION_NEUTRAL, 5, 8, 6, 4, 6, 0, "felm", {MONSTER_FLAGS_NONE}},
    {{500, {1690}}, 34, 3, 45, FACTION_NEUTRAL, 4, 6, 8, 3, 7, 0, "welm", {MONSTER_FLAGS_NONE}}
};
DATA(0x004fab14) float gfStatPower[KB_STAT_POWER_COUNT] = {0.5f,  0.5f,  0.5f,  0.5f,  0.52f, 0.54f, 0.56f,
                                          0.58f, 0.6f,  0.62f, 0.64f, 0.67f, 0.7f,  0.74f,
                                          0.77f, 0.8f,  0.84f, 0.88f, 0.92f, 0.96f, 1.0f,
                                          1.04f, 1.08f, 1.12f, 1.16f, 1.2f,  1.24f, 1.28f,
                                          1.32f, 1.36f, 1.4f,  1.44f, 1.48f, 1.52f, 1.56f,
                                          1.6f,  1.64f, 1.68f, 1.72f, 1.76f, 1.8f};
DATA(0x004fabb8) float gfBattleStat[KB_STAT_POWER_COUNT] = {
    0.2f, 0.2f,  0.2f, 0.2f,  0.2f, 0.25f, 0.3f, 0.35f, 0.4f, 0.45f, 0.5f, 0.55f, 0.6f, 0.65f,
    0.7f, 0.75f, 0.8f, 0.85f, 0.9f, 0.95f, 1.0f, 1.1f,  1.2f, 1.3f,  1.4f, 1.5f,  1.6f, 1.7f,
    1.8f, 1.9f,  2.0f, 2.1f,  2.2f, 2.3f,  2.4f, 2.5f,  2.6f, 2.7f,  2.8f, 2.9f,  3.0f
};
DATA(0x004fac5c) i8 gSpellLimits[KB_SPELL_LIMIT_COUNT] = {3, 3, 2, 2, 1};
DATA(0x004fac64) float gfSpellCastableCombatMod[KB_SPELL_MOD_COUNT] =
    {0.27f, 0.4f, 0.48f, 0.56f, 0.64f, 0.73f, 0.81f, 0.88f, 0.93f, 0.97f, 1.0f};
DATA(0x004fac90) float gfSpellCastNumMod[KB_SPELL_MOD_COUNT] =
    {0.0f, 1.0f, 1.75f, 2.35f, 2.85f, 3.35f, 3.7f, 4.0f, 4.26f, 4.5f, 4.7f};
DATA(0x004facbc) float gfPhilAISpellPowerMod[KB_SPELL_MOD_COUNT] =
    {0.0f, 1.0f, 1.75f, 2.5f, 3.1f, 3.5f, 4.0f, 4.5f, 5.4f, 5.75f, 6.15f};
DATA(0x004face8) float gfPhilAIDurationMod[KB_SPELL_MOD_COUNT] =
    {0.0f, 0.4f, 0.65f, 0.8f, 1.0f, 1.16f, 1.3f, 1.43f, 1.54f, 1.64f, 1.74f};
DATA(0x004fad14) float gfSpellTypeNumMod[KB_QUICK_COMBAT_SPELL_TYPE_COUNT] =
    {1.0f, 0.75f, 0.55f, 0.4f, 0.28f, 0.2f, 0.15f};
DATA(0x0052662c) b32 gbDrawSavedCursor = false;
DATA(0x004fad30) i8 gbArrow[NORMAL_DIRECTION_COUNT][NORMAL_DIRECTION_COUNT] = {
    {8, 0, 0, 0, 8, 16, 16, 16},
    {17, 9, 1, 1, 1, 9, 17, 17},
    {18, 18, 10, 2, 2, 2, 10, 18},
    {19, 19, 19, 11, 3, 3, 3, 11},
    {12, 20, 20, 20, 12, 4, 4, 4},
    {5, 13, 21, 21, 21, 13, 5, 5},
    {6, 6, 14, 22, 22, 22, 14, 6},
    {7, 7, 7, 15, 23, 23, 23, 15}
};
DATA(0x004fad70) u8 giCloudType[KB_CLOUD_MASK_COUNT] = {
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
DATA(0x004fae70) i16 giScoreMon[IDX(CREATURE_COUNT)][IDX(MONSTER_SCORE_FIELD_COUNT)] = {
    {0, 0},    {4, 11},   {8, 20},   {12, 38},  {16, 29},  {20, 57},  {24, 47},  {28, 12},
    {32, 48},  {36, 1},   {40, 2},   {44, 39},  {48, 21},  {52, 49},  {56, 13},  {60, 23},
    {64, 30},  {68, 3},   {72, 24},  {76, 22},  {80, 58},  {84, 4},   {88, 14},  {92, 50},
    {96, 40},  {100, 51}, {104, 15}, {108, 31}, {112, 5},  {116, 25}, {120, 41}, {124, 6},
    {128, 63}, {132, 26}, {135, 64}, {138, 59}, {141, 52}, {144, 65}, {147, 62}, {150, 42},
    {153, 32}, {156, 7},  {159, 17}, {162, 43}, {165, 61}, {168, 54}, {171, 16}, {174, 33},
    {177, 8},  {180, 18}, {183, 53}, {186, 44}, {189, 55}, {192, 27}, {195, 34}, {198, 9},
    {201, 60}, {204, 10}, {207, 19}, {210, 45}, {213, 28}, {216, 56}, {219, 35}, {222, 36},
    {225, 46}, {228, 37}
};
DATA(0x004faf78) i16 giScoreCampaignMon[IDX(CREATURE_COUNT)][IDX(MONSTER_SCORE_FIELD_COUNT)] = {
    {9999, 0},  {5800, 11}, {5600, 20}, {5400, 38}, {5200, 29}, {5000, 57}, {4800, 47}, {4600, 12},
    {4400, 48}, {4200, 1},  {4000, 2},  {3800, 39}, {3600, 21}, {3400, 49}, {3200, 13}, {3000, 23},
    {2800, 30}, {2600, 3},  {2400, 24}, {2200, 22}, {2000, 58}, {1900, 4},  {1800, 14}, {1700, 50},
    {1600, 40}, {1500, 51}, {1400, 15}, {1300, 31}, {1200, 5},  {1100, 25}, {1000, 41}, {980, 6},
    {960, 63},  {940, 26},  {920, 64},  {900, 59},  {880, 52},  {860, 65},  {840, 62},  {820, 42},
    {800, 32},  {780, 7},   {760, 17},  {740, 43},  {720, 61},  {700, 54},  {680, 16},  {660, 33},
    {640, 8},   {620, 18},  {600, 53},  {580, 44},  {560, 55},  {540, 27},  {520, 34},  {500, 9},
    {480, 60},  {460, 10},  {440, 19},  {420, 45},  {400, 28},  {380, 56},  {360, 35},  {340, 36},
    {320, 46},  {300, 37}
};
DATA(0x004fb080) i8 townTheme[IDX(TOWN_MUSIC_TABLE_SIZE)] = {
    IDX(TOWN_MUSIC_KNIGHT),
    IDX(TOWN_MUSIC_BARBARIAN),
    IDX(TOWN_MUSIC_WARLOCK),
    IDX(TOWN_MUSIC_WIZARD),
    IDX(TOWN_MUSIC_SORCERESS),
    IDX(TOWN_MUSIC_NECROMANCER),
    IDX(TOWN_MUSIC_NONE),
    IDX(TOWN_MUSIC_NONE)
};
DATA(0x004fb088) i8 gHeroSkillBonus[IDX(FACTION_COUNT)][KB_HERO_LEVEL_BAND_COUNT][HERO_PRIMARY_STAT_COUNT] = {
    {{35, 45, 10, 10}, {25, 25, 25, 25}},
    {{55, 35, 5, 5}, {25, 25, 25, 25}},
    {{10, 10, 30, 50}, {20, 20, 30, 30}},
    {{10, 10, 50, 30}, {20, 20, 30, 30}},
    {{10, 10, 40, 40}, {20, 20, 30, 30}},
    {{15, 15, 35, 35}, {25, 25, 25, 25}}
};
DATA(0x00526630) b32 gbLoadingMonoIcon = false;
DATA(0x004fb0b8) i32 giMonoIconSkip = -1;
DATA(0x00526634) i32 giScrollX = 0;
DATA(0x00526638) i32 giScrollY = 0;
DATA(0x0052663c) b32 gbNoBorder = false;
DATA(0x004fb0bc) b32 gbEnlargeScreenBlit = true;
DATA(0x00526640) ConfigExecutable giCurExe = CONFIG_EXECUTABLE_GAME;
DATA(0x00526644) b32 gbInDialog = false;
DATA(0x004fb0c0) struct SMenuEnableStatus gsMenuEnableStatus[MENU_ENABLE_STATUS_COUNT] = {
    {APP_MENU_NONE, 0, 0, 0},
    {IDX(KBWIN_MENU_SIZE_640_480), 1, 1, 0},
    {IDX(KBWIN_MENU_SIZE_800_600), 1, 1, 0},
    {IDX(KBWIN_MENU_SIZE_1024_768), 1, 1, 0},
    {IDX(KBWIN_MENU_SIZE_1280_1024), 1, 1, 0},
    {IDX(KBWIN_MENU_FULLSCREEN), 1, 1, 0},
    {APP_MENU_VIEW_WORLD, 0, 0, 0},
    {APP_MENU_VIEW_PUZZLE, 0, 0, 0},
    {APP_MENU_CAST_SPELL, 0, 0, 0},
    {APP_MENU_SEARCH, 0, 0, 0},
    {APP_MENU_MUSIC_FIRST, 1, 0, 0},
    {APP_MENU_MUSIC_FIRST + 1, 1, 0, 0},
    {APP_MENU_MUSIC_FIRST + 2, 1, 0, 0},
    {APP_MENU_MUSIC_FIRST + 3, 1, 0, 0},
    {APP_MENU_MUSIC_FIRST + 4, 1, 0, 0},
    {APP_MENU_MUSIC_FIRST + 5, 1, 0, 0},
    {APP_MENU_MUSIC_FIRST + 6, 1, 0, 0},
    {APP_MENU_MUSIC_FIRST + 7, 1, 0, 0},
    {APP_MENU_MUSIC_FIRST + 8, 1, 0, 0},
    {APP_MENU_MUSIC_FIRST + 9, 1, 0, 0},
    {APP_MENU_MUSIC_LAST, 1, 0, 0},
    {APP_MENU_SOUND_FIRST, 1, 0, 0},
    {APP_MENU_SOUND_FIRST + 1, 1, 0, 0},
    {APP_MENU_SOUND_FIRST + 2, 1, 0, 0},
    {APP_MENU_SOUND_FIRST + 3, 1, 0, 0},
    {APP_MENU_SOUND_FIRST + 4, 1, 0, 0},
    {APP_MENU_SOUND_FIRST + 5, 1, 0, 0},
    {APP_MENU_SOUND_FIRST + 6, 1, 0, 0},
    {APP_MENU_SOUND_FIRST + 7, 1, 0, 0},
    {APP_MENU_SOUND_FIRST + 8, 1, 0, 0},
    {APP_MENU_SOUND_FIRST + 9, 1, 0, 0},
    {APP_MENU_SOUND_LAST, 1, 0, 0},
    {APP_MENU_SPEED_FIRST, 0, 0, 0},
    {APP_MENU_SPEED_FIRST + 1, 0, 0, 0},
    {APP_MENU_SPEED_FIRST + 2, 0, 0, 0},
    {APP_MENU_SPEED_FIRST + 3, 0, 0, 0},
    {APP_MENU_SPEED_LAST, 0, 0, 0},
    {APP_MENU_UNKNOWN_9C6D, 0, 0, 0},
    {APP_MENU_TOGGLE_ROUTE, 0, 0, 0},
    {APP_MENU_TOGGLE_BLACKOUT, 0, 0, 0},
    {IDX(KBWIN_MENU_HELP), 1, 1, 0},
    {IDX(KBWIN_MENU_ABOUT), 1, 1, 0},
    {APP_MENU_RESTART_0, 0, 1, 0},
    {APP_MENU_RESTART_1, 0, 1, 0},
    {APP_MENU_RESTART_2, 0, 1, 0},
    {APP_MENU_RESTART_3, 0, 1, 0},
    {APP_MENU_RESTART_4, 0, 1, 0},
    {APP_MENU_UNKNOWN_9CAD, 0, 1, 0},
    {APP_MENU_RESTART_5, 0, 1, 0},
    {APP_MENU_RESTART_6, 0, 1, 0},
    {APP_MENU_RESTART_7, 0, 1, 0},
    {APP_MENU_RESTART_8, 0, 1, 0},
    {APP_MENU_RESTART_9, 0, 1, 0},
    {APP_MENU_RESTART_10, 0, 1, 0},
    {APP_MENU_RESTART_11, 0, 1, 0},
    {APP_MENU_RESTART_12, 0, 1, 0},
    {APP_MENU_RESTART_13, 0, 1, 0},
    {APP_MENU_LOAD_0, 0, 1, 0},
    {APP_MENU_LOAD_1, 0, 1, 0},
    {APP_MENU_LOAD_2, 0, 1, 0},
    {APP_MENU_LOAD_3, 0, 1, 0},
    {APP_MENU_LOAD_4, 0, 1, 0},
    {APP_MENU_LOAD_5, 0, 1, 0},
    {APP_MENU_LOAD_6, 0, 1, 0},
    {APP_MENU_LOAD_7, 0, 1, 0},
    {APP_MENU_LOAD_8, 0, 1, 0},
    {APP_MENU_LOAD_9, 0, 1, 0},
    {APP_MENU_LOAD_10, 0, 1, 0},
    {APP_MENU_SAVE, 0, 0, 0},
    {APP_MENU_EXIT, 0, 0, 0}
};
DATA(0x00526648) b32 gbInSetupDialog = false;
DATA(0x0052664c) b32 gbMinimized = false;
DATA(0x00526650) b32 gbHeroMoving = false;
DATA(0x00526654) b32 gbInSmackMgr = false;
DATA(0x00526658) i32 glBottomRefresh = 0;
DATA(0x0052665c) b32 gbBothMachinesWin95 = false;
DATA(0x00526660) b32 gbGotFirstHeartbeat = false;
DATA(0x00526664) HMENU hmnuDflt = NULL;
DATA(0x00526668) HMENU hmnuCmbt = NULL;
DATA(0x0052666c) HMENU hmnuAdv = NULL;
DATA(0x00526670) HMENU hmnuTown = NULL;
DATA(0x004fb2ac) H2_CONST char* cMonFilename[IDX(CREATURE_COUNT)] = {
    "peasant.icn",
    "archer.icn",
    "archer2.icn",
    "pikeman.icn",
    "pikeman2.icn",
    "swordsmn.icn",
    "swordsm2.icn",
    "cavalryr.icn",
    "cavalryb.icn",
    "paladin.icn",
    "paladin2.icn",
    "goblin.icn",
    "orc.icn",
    "orc2.icn",
    "wolf.icn",
    "ogre.icn",
    "ogre2.icn",
    "troll.icn",
    "troll2.icn",
    "cyclops.icn",
    "sprite.icn",
    "dwarf.icn",
    "dwarf2.icn",
    "elf.icn",
    "elf2.icn",
    "druid.icn",
    "druid2.icn",
    "unicorn.icn",
    "phoenix.icn",
    "centaur.icn",
    "gargoyle.icn",
    "griffin.icn",
    "minotaur.icn",
    "minotau2.icn",
    "hydra.icn",
    "draggree.icn",
    "dragred.icn",
    "dragblak.icn",
    "halfling.icn",
    "boar.icn",
    "golem.icn",
    "golem2.icn",
    "roc.icn",
    "mage1.icn",
    "mage2.icn",
    "titanblu.icn",
    "titanbla.icn",
    "skeleton.icn",
    "zombie.icn",
    "zombie2.icn",
    "mummyw.icn",
    "mummy2.icn",
    "vampire.icn",
    "vampire2.icn",
    "lich.icn",
    "lich2.icn",
    "dragbone.icn",
    "rogue.icn",
    "nomad.icn",
    "ghost.icn",
    "genie.icn",
    "medusa.icn",
    "eelem.icn",
    "aelem.icn",
    "felem.icn",
    "welem.icn"
};
DATA(0x00526674) b32 gbProcessingCombatAction = false;
DATA(0x00526678) RemoteNetworkProtocol iMPNetProtocol = REMOTE_PROTOCOL_NETBIOS;
DATA(0x004fb3b4) i32 iLastDiffSendTo = DIFF_SEND_FORCE_WHOLE;
DATA(0x004fb3b8) SSpellInfo gsSpellInfo[IDX(SPELL_COUNT)] = {
    {"fireball",
     3,
     8,
     0,
     500,
     9,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"fireball",
     4,
     9,
     0,
     750,
     15,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"lightblt",
     2,
     4,
     15,
     450,
     7,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"chainlte",
     4,
     5,
     15,
     650,
     15,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"telptout", 3, 10, 0, 30, 9, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_COMBAT},
    {"cure", 1, 6, 1, 250, 6, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_COMBAT},
    {"masscure", 4, 6, 1, 700, 15, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_COMBAT},
    {"resurect",
     4,
     13,
     1,
     400,
     12,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"resurtru",
     5,
     12,
     1,
     550,
     15,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"haste",
     1,
     14,
     24,
     300,
     3,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"masshast",
     3,
     14,
     24,
     950,
     10,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"slow",
     1,
     1,
     2,
     350,
     3,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"massslow",
     4,
     1,
     2,
     1000,
     15,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"blind",
     2,
     21,
     28,
     500,
     6,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"bless",
     1,
     7,
     21,
     200,
     3,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"massbles",
     3,
     7,
     21,
     650,
     12,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"stonskin",
     1,
     31,
     30,
     300,
     3,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"stelskin",
     2,
     30,
     31,
     575,
     6,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"curse",
     1,
     3,
     29,
     200,
     3,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"masscurs",
     3,
     3,
     29,
     500,
     12,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"masscurs",
     3,
     22,
     8,
     300,
     9,
     {10, 10, 10, 10, 10, 0},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"masscurs",
     4,
     23,
     8,
     450,
     12,
     {10, 10, 10, 10, 10, 0},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"antimagk",
     3,
     17,
     6,
     100,
     7,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"dipmagk", 1, 18, 7, 275, 5, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_COMBAT},
    {"dipmagk", 3, 18, 7, 450, 12, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_COMBAT},
    {"magcarow",
     1,
     38,
     0,
     200,
     3,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"berzerk",
     4,
     19,
     22,
     1000,
     12,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"armgedn",
     5,
     16,
     0,
     2000,
     20,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"storm",
     4,
     11,
     0,
     1300,
     15,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"meteor~1",
     4,
     24,
     0,
     1000,
     15,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"paralize",
     3,
     20,
     25,
     700,
     9,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"hypnotiz",
     5,
     37,
     26,
     1300,
     15,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"coldray",
     2,
     36,
     19,
     375,
     6,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"coldring",
     3,
     35,
     0,
     500,
     9,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"disruptr",
     2,
     34,
     0,
     500,
     7,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"mnrdeath",
     2,
     29,
     13,
     300,
     6,
     {0, 0, 0, 0, 0, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"mnrdeath",
     3,
     28,
     13,
     450,
     10,
     {0, 0, 0, 0, 0, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"drgnslay",
     2,
     32,
     27,
     100,
     6,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"bloodlus",
     1,
     27,
     0,
     300,
     3,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"resurect",
     3,
     25,
     6,
     400,
     10,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"mirrorim",
     5,
     26,
     0,
     800,
     25,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"shield",
     1,
     15,
     23,
     150,
     3,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"shield",
     4,
     15,
     23,
     375,
     7,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"sumnelm",
     5,
     56,
     0,
     1000,
     30,
     {0, 0, 0, 0, 0, 0},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"sumnelm",
     5,
     57,
     0,
     1000,
     30,
     {0, 0, 0, 0, 0, 0},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"sumnelm",
     5,
     58,
     0,
     1000,
     30,
     {0, 0, 0, 0, 0, 0},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"sumnelm",
     5,
     59,
     0,
     1000,
     30,
     {0, 0, 0, 0, 0, 0},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"erthquak",
     3,
     33,
     0,
     300,
     15,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"", 1, 39, 0, 30, 1, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {"", 1, 40, 0, 30, 1, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {"", 2, 41, 0, 30, 2, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {"", 3, 42, 0, 30, 2, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {"", 3, 43, 0, 30, 2, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {"", 4, 44, 0, 30, 3, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {"", 3, 45, 0, 30, 3, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {"", 2, 46, 0, 30, 5, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {"", 5, 47, 0, 1500, 10, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {"", 4, 48, 0, 30, 10, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {"", 5, 49, 0, 30, 20, {0, 0, 0, 0, 0, 0}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {"", 2, 50, 0, 30, 6, {0, 0, 0, 0, 0, 0}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {"", 2, 51, 0, 30, 8, {0, 0, 0, 0, 0, 0}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {"", 4, 52, 0, 700, 15, {0, 0, 0, 0, 0, 0}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {"", 4, 53, 0, 700, 15, {0, 0, 0, 0, 0, 0}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {"", 4, 54, 0, 700, 15, {0, 0, 0, 0, 0, 0}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {"", 4, 55, 0, 700, 15, {0, 0, 0, 0, 0, 0}, SPELL_INFO_ATTRIBUTE_ADVENTURE}
};
DATA(0x004fb950) H2_CONST char* cArmyFrameFileNames[IDX(CREATURE_COUNT)] = {
    "peas_frm.bin",
    "archrfrm.bin",
    "archrfrm.bin",
    "pikmnfrm.bin",
    "pikmnfrm.bin",
    "swrdsfrm.bin",
    "swrdsfrm.bin",
    "cvlryfrm.bin",
    "cvlr2frm.bin",
    "paladfrm.bin",
    "paladfrm.bin",
    "goblnfrm.bin",
    "orc__frm.bin",
    "orc__frm.bin",
    "wolf_frm.bin",
    "ogre_frm.bin",
    "ogre_frm.bin",
    "trollfrm.bin",
    "trollfrm.bin",
    "cyclofrm.bin",
    "spritfrm.bin",
    "dwarffrm.bin",
    "dwarffrm.bin",
    "elf__frm.bin",
    "elf__frm.bin",
    "druidfrm.bin",
    "druidfrm.bin",
    "unicofrm.bin",
    "phoenfrm.bin",
    "centrfrm.bin",
    "garglfrm.bin",
    "grifffrm.bin",
    "minotfrm.bin",
    "minotfrm.bin",
    "hydrafrm.bin",
    "draggfrm.bin",
    "dragrfrm.bin",
    "dragbfrm.bin",
    "halflfrm.bin",
    "boar_frm.bin",
    "golemfrm.bin",
    "golemfrm.bin",
    "roc__frm.bin",
    "mage1frm.bin",
    "mage1frm.bin",
    "titanfrm.bin",
    "tita2frm.bin",
    "skel_frm.bin",
    "zomb_frm.bin",
    "zomb_frm.bin",
    "mummyfrm.bin",
    "mummyfrm.bin",
    "vampifrm.bin",
    "vampifrm.bin",
    "lich_frm.bin",
    "lich_frm.bin",
    "drabnfrm.bin",
    "roguefrm.bin",
    "nomadfrm.bin",
    "ghostfrm.bin",
    "geniefrm.bin",
    "medusfrm.bin",
    "felemfrm.bin",
    "felemfrm.bin",
    "felemfrm.bin",
    "felemfrm.bin"
};
DATA(0x004fba58) u8 gcSpellInfluenceIcons[KB_SPELL_INFLUENCE_MAP_COUNT] = {
    0x06,
    0x07,
    0x08,
    0x09,
    0x0b,
    0x0f,
    0x13,
    0x02,
    0x02,
    0x02,
    0x02,
    0x02,
    0x0d,
    0x0a,
    0x0a,
    0x00
};
DATA(0x004fba68) u8 giSpellInfluenceToSpell[KB_SPELL_INFLUENCE_MAP_COUNT] = {
    0x09,
    0x0b,
    0x0d,
    0x0e,
    0x12,
    0x1a,
    0x1e,
    0x1f,
    0x25,
    0x26,
    0x29,
    0x65,
    0x16,
    0x10,
    0x11,
    0x00
};
DATA(0x004fba78) u8 giNumPowFrames[KB_SPELL_EFFECT_COUNT] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 8,  8,
                                            10, 10, 10, 10, 15, 10, 10, 10, 10, 10, 16,
                                            16, 14, 19, 22, 10, 17, 10, 12, 11, 16};
DATA(0x004fba98) SpellEffectDisplayType giSpellEffectShowType = SPELL_EFFECT_DISPLAY_EFFECT_STATUS;
DATA(0x004fba9c) i8 gcColorToPlayerPos[RADAR_OWNER_COLOR_COUNT] = {0, 1, 2, 3, 4, 5, 0, 0};
DATA(0x004fbaa4) H2_CONST char* cCombatBkgNames[KB_COMBAT_BACKGROUND_COUNT] = {
                                                     "CBKGWATR.icn",
                                                     "",
                                                     "CBKGGRTR.icn",
                                                     "CBKGGRMT.icn",
                                                     "CBKGSNTR.icn",
                                                     "CBKGSNMT.icn",
                                                     "CBKGSWMP.icn",
                                                     "",
                                                     "CBKGLAVA.icn",
                                                     "",
                                                     "CBKGDSRT.icn",
                                                     "",
                                                     "CBKGDITR.icn",
                                                     "CBKGDIMT.icn",
                                                     "CBKGCRCK.icn",
                                                     "",
                                                     "CBKGBEAC.icn",
                                                     "",
                                                     "CBKGGRAV.icn"};
DATA(0x004fbaf0) struct SCmbtObstacle sCmbtObstacles[KB_COMBAT_OBSTACLE_COUNT] = {
    {0x00000200, 1, 1, {0, 0, 0, 0, 0, 0, 0, 0}}, {0x00000200, 1, 1, {0, 0, 0, 0, 0, 0, 0, 0}},
    {0x00000042, 3, 1, {0, 0, 0, 0, 0, 0, 0, 0}}, {0x00000001, 1, 1, {0, 0, 0, 0, 0, 0, 0, 0}},
    {0x00000000, 2, 2, {0, 1, 0, 0, 0, 0, 0, 0}}, {0x0000014a, 2, 2, {0, 1, 0, 0, 0, 0, 0, 0}},
    {0x00000008, 1, 1, {0, 0, 0, 0, 0, 0, 0, 0}}, {0x00000048, 1, 2, {0, 1, 0, 0, 0, 0, 0, 0}},
    {0x0000000a, 2, 1, {0, 0, 0, 0, 0, 0, 0, 0}}, {0x000000a0, 3, 1, {0, 0, 0, 0, 0, 0, 0, 0}},
    {0x00000001, 1, 1, {0, 0, 0, 0, 0, 0, 0, 0}}, {0x0000014a, 1, 2, {0, 1, 0, 0, 0, 0, 0, 0}},
    {0x0000000a, 1, 1, {0, 0, 0, 0, 0, 0, 0, 0}}, {0x000000a0, 2, 1, {0, 0, 0, 0, 0, 0, 0, 0}},
    {0x00000048, 1, 2, {0, 1, 0, 0, 0, 0, 0, 0}}, {0x0000000a, 2, 2, {0, 1, 0, 0, 0, 0, 0, 0}},
    {0x00000008, 1, 1, {0, 0, 0, 0, 0, 0, 0, 0}}, {0x00000008, 2, 2, {0, 1, 0, 0, 0, 0, 0, 0}},
    {0x00000080, 1, 2, {0, 1, 0, 0, 0, 0, 0, 0}}, {0x00000142, 1, 2, {0, 1, 0, 0, 0, 0, 0, 0}},
    {0x00000080, 1, 2, {0, 1, 0, 0, 0, 0, 0, 0}}, {0x00000080, 2, 1, {0, 0, 0, 0, 0, 0, 0, 0}},
    {0x00000004, 2, 2, {0, 1, 0, 0, 0, 0, 0, 0}}, {0x00000001, 1, 1, {0, 0, 0, 0, 0, 0, 0, 0}},
    {0x000000a0, 1, 1, {0, 0, 0, 0, 0, 0, 0, 0}}, {0x00000200, 1, 2, {0, 1, 0, 0, 0, 0, 0, 0}},
    {0x00000004, 2, 1, {0, 0, 0, 0, 0, 0, 0, 0}}, {0x0000004a, 3, 1, {0, 0, 0, 0, 0, 0, 0, 0}},
    {0x00000002, 3, 1, {0, 0, 0, 0, 0, 0, 0, 0}}, {0x00000010, 1, 1, {0, 0, 0, 0, 0, 0, 0, 0}},
    {0x00000000, 1, 2, {0, 1, 0, 0, 0, 0, 0, 0}}, {0x00000010, 2, 2, {0, 1, 0, 0, 0, 0, 0, 0}}
};
DATA(0x004fbcb0) i32 gEstatesGoldLevel[IDX(HERO_SKILL_LEVEL_COUNT)] = {0, 100, 250, 500};
DATA(0x004fbcc0) float gfSSLogisticsMod[IDX(HERO_SKILL_LEVEL_COUNT)] = {1.0f, 1.1f, 1.2f, 1.3f};
DATA(0x004fbcd0) float gfSSNavigationMod[IDX(HERO_SKILL_LEVEL_COUNT)] = {1.0f, 1.33f, 1.66f, 2.0f};
DATA(0x004fbce0) float gfSSArcheryMod[IDX(HERO_SKILL_LEVEL_COUNT)] = {1.0f, 1.1f, 1.25f, 1.5f};
DATA(0x004fbcf0) float gfSSAIArcheryMod[IDX(HERO_SKILL_LEVEL_COUNT)] = {1.0f, 1.04f, 1.1f, 1.2f};
DATA(0x004fbd00) i8 giVisRange[IDX(HERO_SKILL_LEVEL_COUNT)] = {4, 5, 6, 7};
DATA(0x004fbd04) u8 gStartingHeroStats[IDX(FACTION_COUNT)][HERO_STARTING_STAT_COUNT] = {
    {2, 2, 1, 1, 1},
    {3, 1, 1, 1, 1},
    {0, 0, 2, 3, 1},
    {0, 0, 3, 2, 1},
    {0, 1, 2, 2, 1},
    {1, 0, 2, 2, 1}
};
DATA(0x004fbd24) i32 giTerrainCost[KB_TERRAIN_TYPE_COUNT][IDX(HERO_SKILL_LEVEL_COUNT)][KB_TERRAIN_STEP_TYPE_COUNT] =
    {{{100, 150}, {100, 150}, {100, 150}, {100, 150}},
     {{100, 150}, {100, 150}, {100, 150}, {100, 150}},
     {{150, 225}, {125, 187}, {100, 150}, {100, 150}},
     {{175, 262}, {150, 225}, {125, 187}, {100, 150}},
     {{100, 150}, {100, 150}, {100, 150}, {100, 150}},
     {{200, 300}, {175, 262}, {150, 225}, {100, 150}},
     {{100, 150}, {100, 150}, {100, 150}, {100, 150}},
     {{125, 187}, {100, 150}, {100, 150}, {100, 150}},
     {{125, 187}, {100, 150}, {100, 150}, {100, 150}},
     {{75, 112}, {75, 112}, {75, 112}, {75, 112}}};
DATA(0x004fbe64) u8 bStopOnTrigger[KB_TRIGGER_TYPE_COUNT] = {
    0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1
};
DATA(0x004fbee0) H2_CONST char* gTownPrefixNames[IDX(FACTION_COUNT)] = {
    "twnk",
    "twnb",
    "twns",
    "twnw",
    "twnz",
    "twnn"};
DATA(0x004fbef8) H2_CONST char* gTownObjNames[KB_TOWN_OBJECT_NAME_COUNT] = {
    "mage",
    "thie",
    "tvrn",
    "dock",
    "well",
    "tent",
    "cstl",
    "stat",
    "ltur",
    "rtur",
    "mark",
    "wel2",
    "moat",
    "spec",
    "boat",
    "capt",
    "ext0",
    "ext1",
    "ext2",
    "dw_0",
    "dw_1",
    "dw_2",
    "dw_3",
    "dw_4",
    "dw_5",
    "up_1",
    "up_2",
    "up_3",
    "up_4",
    "up_5",
    "up5b",
    "ext3"
};
DATA(0x004fbf78) H2_ENUM_STORAGE(CreatureType, i8)
gDwellingType[IDX(FACTION_COUNT)][KB_DWELLING_TYPE_COUNT] = {
    {IDX(CREATURE_PEASANT),
     IDX(CREATURE_ARCHER),
     IDX(CREATURE_PIKEMAN),
     IDX(CREATURE_SWORDSMAN),
     IDX(CREATURE_CAVALRY),
     IDX(CREATURE_PALADIN),
     IDX(CREATURE_RANGER),
     IDX(CREATURE_VETERAN_PIKEMAN),
     IDX(CREATURE_MASTER_SWORDSMAN),
     IDX(CREATURE_CHAMPION),
     IDX(CREATURE_CRUSADER),
     ARMY_GROUP_EMPTY_SLOT},
    {IDX(CREATURE_GOBLIN),
     IDX(CREATURE_ORC),
     IDX(CREATURE_WOLF),
     IDX(CREATURE_OGRE),
     IDX(CREATURE_TROLL),
     IDX(CREATURE_CYCLOPS),
     IDX(CREATURE_ORC_CHIEF),
     ARMY_GROUP_EMPTY_SLOT,
     IDX(CREATURE_OGRE_LORD),
     IDX(CREATURE_WAR_TROLL),
     ARMY_GROUP_EMPTY_SLOT,
     ARMY_GROUP_EMPTY_SLOT},
    {IDX(CREATURE_SPRITE),
     IDX(CREATURE_DWARF),
     IDX(CREATURE_ELF),
     IDX(CREATURE_DRUID),
     IDX(CREATURE_UNICORN),
     IDX(CREATURE_PHOENIX),
     IDX(CREATURE_BATTLE_DWARF),
     IDX(CREATURE_GRAND_ELF),
     IDX(CREATURE_GREATER_DRUID),
     ARMY_GROUP_EMPTY_SLOT,
     ARMY_GROUP_EMPTY_SLOT,
     ARMY_GROUP_EMPTY_SLOT},
    {IDX(CREATURE_CENTAUR),
     IDX(CREATURE_GARGOYLE),
     IDX(CREATURE_GRIFFIN),
     IDX(CREATURE_MINOTAUR),
     IDX(CREATURE_HYDRA),
     IDX(CREATURE_GREEN_DRAGON),
     ARMY_GROUP_EMPTY_SLOT,
     ARMY_GROUP_EMPTY_SLOT,
     IDX(CREATURE_MINOTAUR_KING),
     ARMY_GROUP_EMPTY_SLOT,
     IDX(CREATURE_RED_DRAGON),
     IDX(CREATURE_BLACK_DRAGON)},
    {IDX(CREATURE_HALFLING),
     IDX(CREATURE_BOAR),
     IDX(CREATURE_IRON_GOLEM),
     IDX(CREATURE_ROC),
     IDX(CREATURE_MAGE),
     IDX(CREATURE_GIANT),
     ARMY_GROUP_EMPTY_SLOT,
     IDX(CREATURE_STEEL_GOLEM),
     ARMY_GROUP_EMPTY_SLOT,
     IDX(CREATURE_ARCHMAGE),
     IDX(CREATURE_TITAN),
     ARMY_GROUP_EMPTY_SLOT},
    {IDX(CREATURE_SKELETON),
     IDX(CREATURE_ZOMBIE),
     IDX(CREATURE_MUMMY),
     IDX(CREATURE_VAMPIRE),
     IDX(CREATURE_LICH),
     IDX(CREATURE_BONE_DRAGON),
     IDX(CREATURE_MUTANT_ZOMBIE),
     IDX(CREATURE_ROYAL_MUMMY),
     IDX(CREATURE_VAMPIRE_LORD),
     IDX(CREATURE_POWER_LICH),
     ARMY_GROUP_EMPTY_SLOT,
     ARMY_GROUP_EMPTY_SLOT}
};
DATA(0x004fbfc0) i32 gMageBuildingCosts[KB_MAGE_GUILD_LEVEL_COUNT][KB_BUILDING_RESOURCE_COUNT] = {
    {0, 0, 0, 0, 0, 0, 0},
    {5, 0, 5, 0, 0, 0, 2000},
    {5, 4, 5, 4, 4, 4, 1000},
    {5, 6, 5, 6, 6, 6, 1000},
    {5, 8, 5, 8, 8, 8, 1000},
    {5, 10, 5, 10, 10, 10, 1000}
};
DATA(0x004fc068) i32 gSpecialBuildingCosts[IDX(FACTION_COUNT)][KB_BUILDING_RESOURCE_COUNT] = {
    {5, 0, 15, 0, 0, 0, 1500},
    {10, 0, 10, 0, 0, 0, 2000},
    {0, 0, 0, 0, 10, 0, 1500},
    {5, 0, 10, 0, 0, 0, 3000},
    {5, 5, 5, 5, 5, 5, 1500},
    {0, 10, 0, 10, 0, 0, 1000}
};
DATA(0x004fc110) i32 gNeutralBuildingCosts[KB_BUILDING_NEUTRAL_LIMIT][KB_BUILDING_RESOURCE_COUNT] = {
    {5, 0, 5, 0, 0, 0, 2000},
    {5, 0, 0, 0, 0, 0, 750},
    {5, 0, 0, 0, 0, 0, 500},
    {20, 0, 0, 0, 0, 0, 2000},
    {0, 0, 0, 0, 0, 0, 500},
    {0, 0, 0, 0, 0, 0, 0},
    {20, 0, 20, 0, 0, 0, 5000},
    {0, 0, 5, 0, 0, 0, 1250},
    {0, 0, 5, 0, 0, 0, 1500},
    {0, 0, 5, 0, 0, 0, 1500},
    {5, 0, 0, 0, 0, 0, 500},
    {0, 0, 0, 0, 0, 0, 1000},
    {0, 0, 0, 0, 0, 0, 750},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 500}
};
DATA(0x004fc2d0) i32 gMageBaseResourceValues[KB_MAGE_GUILD_LEVEL_COUNT] = {0, 4000, 6500, 8500, 10500, 15000};
DATA(0x004fc2e8) i32 gNeutralBaseResourceValues[IDX(BUILDING_SLOT_DWELLING_FIRST)] = {
    5000, 300, 350, 2000, 3000, 0, 12000, 2500, 1500, 1500, 200, 1000, 500, 0, 0, 1100, 0, 0, 0
};
DATA(0x004fc334) i32 gSpecialBuildingBaseResourceValues[IDX(FACTION_COUNT)] = {1500, 1000, 1000, 4500, 3500, 1000};
DATA(0x004fc34c) i32 gDwellingBaseResourceValues[IDX(FACTION_COUNT)][KB_DWELLING_TYPE_COUNT] = {
    {858, 2225, 2816, 7385, 13754, 29785, 4000, 3200, 8000, 16000, 40000, 0},
    {1802, 2615, 3414, 6967, 13212, 38141, 3500, 0, 8000, 16000, 0, 0},
    {1684, 3000, 3500, 7213, 15181, 27684, 4000, 4000, 12000, 0, 0, 0},
    {1956, 2607, 3869, 7510, 16002, 90000, 0, 0, 8500, 0, 120000, 180000},
    {1700, 3500, 2800, 9000, 11500, 85000, 0, 3500, 0, 15000, 155000, 0},
    {2200, 2100, 3800, 6000, 9500, 90000, 3000, 4900, 15000, 12000, 0, 0}
};
DATA(0x004fc46c) i32 gDwellingCosts[IDX(FACTION_COUNT)][KB_DWELLING_TYPE_COUNT][KB_BUILDING_RESOURCE_COUNT] = {
    {{0, 0, 0, 0, 0, 0, 200},
     {0, 0, 0, 0, 0, 0, 1000},
     {0, 0, 5, 0, 0, 0, 1000},
     {10, 0, 10, 0, 0, 0, 2000},
     {20, 0, 0, 0, 0, 0, 3000},
     {20, 0, 0, 0, 20, 0, 5000},
     {5, 0, 0, 0, 0, 0, 1500},
     {0, 0, 5, 0, 0, 0, 1500},
     {5, 0, 5, 0, 0, 0, 2000},
     {10, 0, 0, 0, 0, 0, 3000},
     {10, 0, 0, 0, 10, 0, 5000},
     {0, 0, 0, 0, 0, 0, 0}},
    {{0, 0, 0, 0, 0, 0, 300},
     {5, 0, 0, 0, 0, 0, 800},
     {0, 0, 0, 0, 0, 0, 1000},
     {10, 0, 10, 0, 0, 0, 2000},
     {0, 0, 20, 0, 0, 0, 4000},
     {0, 0, 20, 0, 20, 0, 6000},
     {5, 0, 0, 0, 0, 0, 1200},
     {0, 0, 0, 0, 0, 0, 0},
     {5, 0, 5, 0, 0, 0, 3000},
     {0, 0, 10, 0, 0, 0, 2000},
     {0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0}},
    {{5, 0, 0, 0, 0, 0, 500},
     {5, 0, 0, 0, 0, 0, 1000},
     {0, 0, 0, 0, 0, 0, 1500},
     {0, 0, 10, 0, 0, 0, 2500},
     {10, 0, 0, 0, 0, 10, 3000},
     {0, 20, 30, 0, 0, 0, 10000},
     {5, 0, 0, 0, 0, 0, 1500},
     {5, 0, 0, 0, 0, 0, 1500},
     {0, 5, 0, 0, 0, 0, 1500},
     {0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0}},
    {{0, 0, 0, 0, 0, 0, 500},
     {0, 0, 10, 0, 0, 0, 1000},
     {0, 0, 0, 0, 0, 0, 2000},
     {0, 0, 0, 0, 0, 10, 3000},
     {0, 0, 0, 10, 0, 0, 4000},
     {0, 0, 30, 20, 0, 0, 15000},
     {0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 5, 2000},
     {0, 0, 0, 0, 0, 0, 0},
     {0, 0, 5, 10, 0, 0, 5000},
     {0, 0, 5, 10, 0, 0, 5000}},
    {{0, 0, 0, 0, 0, 0, 400},
     {0, 0, 0, 0, 0, 0, 800},
     {5, 0, 5, 0, 0, 0, 1500},
     {5, 0, 0, 0, 0, 0, 3000},
     {5, 5, 5, 5, 5, 5, 3500},
     {5, 0, 5, 0, 0, 20, 12500},
     {0, 0, 0, 0, 0, 0, 0},
     {0, 5, 0, 0, 0, 0, 1500},
     {0, 0, 0, 0, 0, 0, 0},
     {5, 0, 5, 0, 0, 0, 4000},
     {5, 0, 5, 0, 0, 20, 12500},
     {0, 0, 0, 0, 0, 0, 0}},
    {{0, 0, 0, 0, 0, 0, 400},
     {0, 0, 0, 0, 0, 0, 1000},
     {0, 0, 10, 0, 0, 0, 1500},
     {10, 0, 0, 0, 0, 0, 3000},
     {10, 0, 0, 10, 0, 0, 4000},
     {10, 5, 10, 5, 5, 5, 10000},
     {0, 0, 0, 0, 0, 0, 1000},
     {0, 0, 5, 0, 0, 0, 1500},
     {5, 0, 0, 0, 10, 10, 4000},
     {0, 0, 5, 0, 5, 0, 3000},
     {0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0}}
};
DATA(0x004fcc4c) u32l gHierarchyMask[IDX(FACTION_COUNT)][KB_DWELLING_TYPE_COUNT] = {
    {0x00000000UL,
     0x00080000UL,
     0x00080010UL,
     0x00080004UL,
     0x00700000UL,
     0x00700000UL,
     0x00700000UL,
     0x00700000UL,
     0x00700000UL,
     0x00800000UL,
     0x01000000UL,
     0xffffffffUL},
    {0x00000000UL,
     0x00080000UL,
     0x00080000UL,
     0x00080000UL,
     0x00700000UL,
     0x00800000UL,
     0x00700000UL,
     0xffffffffUL,
     0x00700000UL,
     0x00800000UL,
     0xffffffffUL,
     0xffffffffUL},
    {0x00000000UL,
     0x00080004UL,
     0x00080000UL,
     0x00200001UL,
     0x00400000UL,
     0x00800000UL,
     0x00100010UL,
     0x00400000UL,
     0x00400000UL,
     0xffffffffUL,
     0xffffffffUL,
     0xffffffffUL},
    {0x00000000UL,
     0x00080000UL,
     0x00080000UL,
     0x00100000UL,
     0x00200000UL,
     0x00c00000UL,
     0xffffffffUL,
     0xffffffffUL,
     0x00400000UL,
     0xffffffffUL,
     0x01000000UL,
     0x20000000UL},
    {0x00000000UL,
     0x00080000UL,
     0x00080000UL,
     0x00100000UL,
     0x00200001UL,
     0x00c00000UL,
     0xffffffffUL,
     0x00200010UL,
     0xffffffffUL,
     0x00802000UL,
     0x01000000UL,
     0xffffffffUL},
    {0x00000000UL,
     0x00080000UL,
     0x00080000UL,
     0x00200002UL,
     0x00100001UL,
     0x00800000UL,
     0x00100000UL,
     0x00200000UL,
     0x00400000UL,
     0x00800000UL,
     0xffffffffUL,
     0xffffffffUL}
};
DATA(0x004fcd6c) i32 giDebugBuildingToBuild = -1;
DATA(0x004fcd70) u8 giTerrainToMusicTrack[IDX(TERRAIN_COUNT)] = {16, 18, 14, 15, 11, 13, 17, 12, 16};
DATA(0x004fcd7c) H2_CONST char* cHeroTypeShortName[IDX(FACTION_COUNT)] = {
    "kngt",
    "barb",
    "sorc",
    "wrlk",
    "wzrd",
    "necr"};
DATA(0x004fcd94) char cHeroTypeInitial[HERO_TYPE_INITIAL_COUNT] = {'k', 'b', 's', 'w', 'z', 'n'};
DATA(0x004fcd9c) i32 giDeferObjDrawX = -1;
DATA(0x004fcda0) i32 giDeferObjDrawY = -1;
DATA(0x0052667c) class heroWindow* gpInitWin = NULL;
DATA(0x004fcda4) u8 iGetSSByAlignment[IDX(HERO_SKILL_COUNT)][IDX(FACTION_COUNT)] = {
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
DATA(0x004fcdf8) struct SCmbtHero sCmbtHero[KB_COMBAT_HERO_SPRITE_COUNT] = {
    {{18, 8},
     {-61, -49},
     2,
     {1, 9, 9, 2, 1, 3, 2, 2, 1, 3, 2, 0},
     {{1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 3, 4, 5, 4, 5, 4, 3, 2},
      {6, 7, 8, 9, 8, 9, 8, 7, 6},
      {10, 11, -1, -1, -1, -1, -1, -1, -1},
      {10, -1, -1, -1, -1, -1, -1, -1, -1},
      {6, 12, 13, -1, -1, -1, -1, -1, -1},
      {12, 6, -1, -1, -1, -1, -1, -1, -1},
      {2, 14, -1, -1, -1, -1, -1, -1, -1},
      {2, -1, -1, -1, -1, -1, -1, -1, -1},
      {15, 16, 17, -1, -1, -1, -1, -1, -1},
      {18, 19, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1}}},
    {{13, 9},
     {-67, -56},
     2,
     {1, 9, 8, 3, 2, 3, 2, 2, 1, 3, 1, 0},
     {{1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 3, 4, 5, 4, 5, 4, 3, 2},
      {6, 7, 8, 9, 9, 8, 7, 6, -1},
      {6, 10, 11, -1, -1, -1, -1, -1, -1},
      {10, 6, -1, -1, -1, -1, -1, -1, -1},
      {6, 12, 13, -1, -1, -1, -1, -1, -1},
      {12, 6, -1, -1, -1, -1, -1, -1, -1},
      {6, 14, -1, -1, -1, -1, -1, -1, -1},
      {6, -1, -1, -1, -1, -1, -1, -1, -1},
      {15, 16, 17, -1, -1, -1, -1, -1, -1},
      {18, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1}}},
    {{19, 12},
     {-62, -50},
     2,
     {1, 9, 5, 3, 2, 3, 2, 2, 1, 3, 1, 0},
     {{1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 3, 4, 5, 4, 5, 4, 3, 2},
      {6, 7, 8, 7, 6, -1, -1, -1, -1},
      {6, 7, 9, -1, -1, -1, -1, -1, -1},
      {7, 6, -1, -1, -1, -1, -1, -1, -1},
      {6, 10, 11, -1, -1, -1, -1, -1, -1},
      {10, 6, -1, -1, -1, -1, -1, -1, -1},
      {6, 12, -1, -1, -1, -1, -1, -1, -1},
      {6, -1, -1, -1, -1, -1, -1, -1, -1},
      {13, 14, 15, -1, -1, -1, -1, -1, -1},
      {16, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1}}},
    {{18, 7},
     {-65, -50},
     2,
     {1, 9, 9, 4, 2, 3, 1, 2, 1, 2, 1, 0},
     {{1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 3, 4, 5, 4, 5, 4, 3, 2},
      {6, 7, 8, 9, 10, 9, 8, 7, 6},
      {6, 7, 11, 12, -1, -1, -1, -1, -1},
      {11, 6, -1, -1, -1, -1, -1, -1, -1},
      {6, 7, 13, -1, -1, -1, -1, -1, -1},
      {6, -1, -1, -1, -1, -1, -1, -1, -1},
      {6, 14, -1, -1, -1, -1, -1, -1, -1},
      {6, -1, -1, -1, -1, -1, -1, -1, -1},
      {15, 16, -1, -1, -1, -1, -1, -1, -1},
      {6, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1}}},
    {{25, 22},
     {-65, -47},
     2,
     {1, 9, 7, 5, 4, 2, 1, 2, 1, 2, 1, 0},
     {{1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 3, 4, 5, 4, 5, 4, 3, 2},
      {6, 7, 8, 9, 8, 7, 6, -1, -1},
      {6, 10, 11, 12, 13, -1, -1, -1, -1},
      {12, 11, 10, 6, -1, -1, -1, -1, -1},
      {6, 14, -1, -1, -1, -1, -1, -1, -1},
      {6, -1, -1, -1, -1, -1, -1, -1, -1},
      {6, 15, -1, -1, -1, -1, -1, -1, -1},
      {6, -1, -1, -1, -1, -1, -1, -1, -1},
      {16, 17, -1, -1, -1, -1, -1, -1, -1},
      {18, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1}}},
    {{17, 10},
     {-61, -50},
     2,
     {1, 9, 6, 5, 3, 5, 1, 5, 1, 1, 2, 0},
     {{1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 3, 4, 5, 4, 5, 4, 3, 2},
      {6, 7, 6, 7, 6, 7, -1, -1, -1},
      {7, 8, 9, 10, 11, -1, -1, -1, -1},
      {10, 9, 7, -1, -1, -1, -1, -1, -1},
      {7, 12, 13, 14, 15, -1, -1, -1, -1},
      {7, -1, -1, -1, -1, -1, -1, -1, -1},
      {7, 12, 13, 14, 16, -1, -1, -1, -1},
      {7, -1, -1, -1, -1, -1, -1, -1, -1},
      {17, -1, -1, -1, -1, -1, -1, -1, -1},
      {18, 19, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1}}},
    {{17, 12},
     {-45, -31},
     2,
     {1, 0, 0, 3, 2, 2, 1, 2, 1, 3, 1, 0},
     {{1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 3, 4, -1, -1, -1, -1, -1, -1},
      {3, 2, -1, -1, -1, -1, -1, -1, -1},
      {5, 6, -1, -1, -1, -1, -1, -1, -1},
      {5, -1, -1, -1, -1, -1, -1, -1, -1},
      {5, 7, -1, -1, -1, -1, -1, -1, -1},
      {5, -1, -1, -1, -1, -1, -1, -1, -1},
      {8, 9, 8, -1, -1, -1, -1, -1, -1},
      {10, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1}}},
    {{17, 12},
     {-45, -31},
     2,
     {1, 0, 0, 3, 2, 3, 2, 3, 2, 1, 2, 0},
     {{1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 3, 4, -1, -1, -1, -1, -1, -1},
      {3, 2, -1, -1, -1, -1, -1, -1, -1},
      {2, 5, 6, -1, -1, -1, -1, -1, -1},
      {5, 2, -1, -1, -1, -1, -1, -1, -1},
      {2, 5, 7, -1, -1, -1, -1, -1, -1},
      {5, 2, -1, -1, -1, -1, -1, -1, -1},
      {8, -1, -1, -1, -1, -1, -1, -1, -1},
      {9, 10, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1}}},
    {{17, 12},
     {-45, -31},
     2,
     {1, 0, 0, 3, 2, 2, 1, 2, 1, 2, 1, 0},
     {{1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 3, 4, -1, -1, -1, -1, -1, -1},
      {3, 2, -1, -1, -1, -1, -1, -1, -1},
      {5, 6, -1, -1, -1, -1, -1, -1, -1},
      {5, -1, -1, -1, -1, -1, -1, -1, -1},
      {5, 7, -1, -1, -1, -1, -1, -1, -1},
      {5, -1, -1, -1, -1, -1, -1, -1, -1},
      {8, 9, -1, -1, -1, -1, -1, -1, -1},
      {10, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1}}},
    {{17, 12},
     {-45, -31},
     1,
     {1, 0, 0, 3, 2, 2, 1, 2, 1, 3, 0, 0},
     {{1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 3, 4, -1, -1, -1, -1, -1, -1},
      {3, 2, -1, -1, -1, -1, -1, -1, -1},
      {5, 6, -1, -1, -1, -1, -1, -1, -1},
      {5, -1, -1, -1, -1, -1, -1, -1, -1},
      {5, 7, -1, -1, -1, -1, -1, -1, -1},
      {5, -1, -1, -1, -1, -1, -1, -1, -1},
      {8, 9, 10, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1}}},
    {{17, 12},
     {-45, -31},
     2,
     {1, 0, 0, 3, 2, 2, 1, 2, 1, 1, 2, 0},
     {{1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 3, 4, -1, -1, -1, -1, -1, -1},
      {3, 2, -1, -1, -1, -1, -1, -1, -1},
      {5, 6, -1, -1, -1, -1, -1, -1, -1},
      {5, -1, -1, -1, -1, -1, -1, -1, -1},
      {5, 7, -1, -1, -1, -1, -1, -1, -1},
      {5, -1, -1, -1, -1, -1, -1, -1, -1},
      {8, -1, -1, -1, -1, -1, -1, -1, -1},
      {9, 10, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1}}},
    {{17, 12},
     {-45, -31},
     2,
     {1, 0, 0, 3, 2, 2, 1, 2, 1, 1, 2, 0},
     {{1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 3, 4, -1, -1, -1, -1, -1, -1},
      {3, 2, -1, -1, -1, -1, -1, -1, -1},
      {5, 6, -1, -1, -1, -1, -1, -1, -1},
      {5, -1, -1, -1, -1, -1, -1, -1, -1},
      {5, 7, -1, -1, -1, -1, -1, -1, -1},
      {5, -1, -1, -1, -1, -1, -1, -1, -1},
      {8, -1, -1, -1, -1, -1, -1, -1, -1},
      {9, 10, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1}}}
};
DATA(0x004fd404) u8 iWallToHexCell[KB_CASTLE_WALL_SEGMENT_COUNT] = {9, 34, 86, 113};
DATA(0x004fd408) u8 iTowerToHexCell[KB_CASTLE_TOWER_COUNT] = {22, 47, 73, 100};
DATA(0x004fd40c) u16 wallPos[KB_CASTLE_WALL_SEGMENT_COUNT][IDX(COORDINATE_AXIS_COUNT)] =
    {{468, 58}, {421, 128}, {417, 291}, {498, 402}};
DATA(0x004fd41c) u16 towerPos[KB_CASTLE_TOWER_COUNT][IDX(COORDINATE_AXIS_COUNT)] =
    {{441, 61}, {398, 147}, {398, 233}, {441, 316}};
DATA(0x004fd42c) u16 doorPos[KB_CASTLE_DOOR_POSITION_COUNT][IDX(COORDINATE_AXIS_COUNT)] = {{393, 192}, {348, 262}};
DATA(0x004fd434) float fTradingPostEfficency[KB_TRADING_POST_EFFICIENCY_COUNT] =
    {0.0f, 0.1f, 0.15f, 0.2f, 0.25f, 0.3f, 0.35f, 0.4f, 0.45f, 0.5f, 0.5f};
DATA(0x004fd460) struct SElevationOverlay sElevationOverlay[ELEVATION_OVERLAY_COUNT] = {
    {0x0000, {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
    {0x0002, {30, 31, 32, 33, 47, 60, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
    {0x0002, {56, 57, 58, 59, 60, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
    {0x0002, {42, 55, 56, 57, 58, 59, 60, 48, -1, -1, -1, -1, -1, -1, -1}},
    {0x0002, {69, 70, 71, 72, 73, 60, 48, -1, -1, -1, -1, -1, -1, -1, -1}},
    {0x0002, {29, 30, 31, 32, 33, 34, 35, 81, 69, 70, 71, 72, 73, 74, 87}},
    {0x0002, {29, 17, 18, 19, 20, 21, 81, 95, 96, 97, 98, 99, -1, -1, -1}},
    {0x0004, {30, 31, 32, 33, 47, 60, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
    {0x0004, {56, 57, 58, 59, 60, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
    {0x0004, {42, 55, 56, 57, 58, 59, 47, -1, -1, -1, -1, -1, -1, -1, -1}},
    {0x0004, {69, 70, 71, 72, 73, 60, 48, -1, -1, -1, -1, -1, -1, -1, -1}},
    {0x0004, {18, 30, 43, 84, 85, 73, 60, -1, -1, -1, -1, -1, -1, -1, -1}},
    {0x0004, {21, 34, 48, 70, 83, 97, 98, -1, -1, -1, -1, -1, -1, -1, -1}},
    {0x0040, {30, 31, 32, 33, 47, 60, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
    {0x0040, {56, 57, 58, 59, 60, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
    {0x0040, {42, 55, 56, 57, 58, 59, 60, 48, -1, -1, -1, -1, -1, -1, -1}},
    {0x0040, {69, 70, 71, 72, 73, 60, 48, -1, -1, -1, -1, -1, -1, -1, -1}},
    {0x0040, {29, 30, 31, 32, 33, 34, 35, 81, 69, 70, 71, 72, 73, 74, 87}},
    {0x0040, {29, 17, 18, 19, 20, 21, 81, 95, 96, 97, 98, 99, -1, -1, -1}},
    {0x0080, {30, 31, 32, 33, 47, 60, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
    {0x0080, {56, 57, 58, 59, 60, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
    {0x0080, {42, 55, 56, 57, 58, 59, 60, 48, -1, -1, -1, -1, -1, -1, -1}},
    {0x0080, {69, 70, 71, 72, 73, 60, 48, -1, -1, -1, -1, -1, -1, -1, -1}},
    {0x0080, {43, 30, 18, 84, 85, 73, 60, -1, -1, -1, -1, -1, -1, -1, -1}},
    {0x0080, {21, 34, 48, 70, 83, 97, 98, -1, -1, -1, -1, -1, -1, -1, -1}}
};
DATA(0x004fd60c) i8 captainStats[IDX(FACTION_COUNT)][HERO_PRIMARY_STAT_COUNT] =
    {{1, 1, 1, 1}, {1, 1, 1, 1}, {0, 0, 2, 2}, {0, 0, 2, 2}, {0, 0, 2, 2}, {0, 0, 2, 2}};
DATA(0x00526680) b32 gbDrawingPuzzle = false;
DATA(0x004fd624) i32 giWalkingFrom = -1;
DATA(0x004fd628) i32 giWalkingFrom2 = -1;
DATA(0x004fd62c) i32 giWalkingTo = -1;
DATA(0x004fd630) i32 giWalkingTo2 = -1;
DATA(0x00526684) i32 giWalkingYMod = 0;
DATA(0x004fd634) u8 moatCell[KB_MOAT_CELL_COUNT] = {8, 21, 33, 46, 58, 72, 85, 99, 112};
DATA(0x004fd641) SCampaignChoice
    campaignChoices[IDX(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT][CAMPAIGN_BONUS_CHOICE_COUNT] = {
        {{{CAMPAIGN_CHOICE_RESOURCE, {IDX(RES_GOLD)}, CHOICE_GOLD_BONUS},
          {CAMPAIGN_CHOICE_ARTIFACT, {IDX(ARTIFACT_THUNDER_MACE)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, {IDX(ARTIFACT_ARMORED_GAUNTLETS)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_WIZARD)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_SORCERESS)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_KNIGHT)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_WIZARD)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_SORCERESS)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_KNIGHT)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_WIZARD)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_SORCERESS)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_KNIGHT)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_WIZARD)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_SORCERESS)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_KNIGHT)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_SPELL, {IDX(SPELL_MIRROR_IMAGE)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_SPELL, {IDX(SPELL_SUMMON_EARTH_ELEMENTAL)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_SPELL, {IDX(SPELL_RESURRECT)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ARTIFACT, {IDX(ARTIFACT_BLACK_PEARL)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, {IDX(ARTIFACT_DRAGON_SWORD)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, {IDX(ARTIFACT_DIVINE_BREASTPLATE)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_WIZARD)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_SORCERESS)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_KNIGHT)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_RESOURCE, {IDX(RES_CRYSTAL)}, CHOICE_RESOURCE_BONUS},
          {CAMPAIGN_CHOICE_RESOURCE, {IDX(RES_GEMS)}, CHOICE_RESOURCE_BONUS},
          {CAMPAIGN_CHOICE_RESOURCE, {IDX(RES_MERCURY)}, CHOICE_RESOURCE_BONUS}},
         {{CAMPAIGN_CHOICE_ARTIFACT, {IDX(ARTIFACT_TAX_LIEN)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, {IDX(ARTIFACT_HIDEOUS_MASK)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, {IDX(ARTIFACT_FIZBIN_OF_MISFORTUNE)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_NONE, {CHOICE_VALUE_NONE}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_NONE, {CHOICE_VALUE_NONE}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_NONE, {CHOICE_VALUE_NONE}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_WIZARD)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_SORCERESS)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_KNIGHT)}, CHOICE_NO_AMOUNT}}},
        {{{CAMPAIGN_CHOICE_RESOURCE, {IDX(RES_GOLD)}, CHOICE_GOLD_BONUS},
          {CAMPAIGN_CHOICE_ARTIFACT, {IDX(ARTIFACT_MAGE_RING)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, {IDX(ARTIFACT_MINOR_SCROLL)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_NECROMANCER)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_WARLOCK)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_BARBARIAN)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_RESOURCE, {IDX(RES_GOLD)}, CHOICE_GOLD_BONUS},
          {CAMPAIGN_CHOICE_SPELL, {IDX(SPELL_MASS_CURSE)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, {IDX(ARTIFACT_DEFENDER_HELM)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_NECROMANCER)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_WARLOCK)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_BARBARIAN)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_NECROMANCER)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_WARLOCK)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_BARBARIAN)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_NECROMANCER)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_WARLOCK)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_BARBARIAN)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_NECROMANCER)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_WARLOCK)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_BARBARIAN)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_SECONDARY_SKILL, {IDX(HERO_SKILL_LOGISTICS)}, CHOICE_BASIC_SKILL},
          {CAMPAIGN_CHOICE_ARTIFACT, {IDX(ARTIFACT_POWER_AXE)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, {IDX(ARTIFACT_WHITE_PEARL)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_NECROMANCER)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_WARLOCK)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_BARBARIAN)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ARTIFACT, {IDX(ARTIFACT_BLACK_PEARL)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, {IDX(ARTIFACT_DRAGON_SWORD)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, {IDX(ARTIFACT_DIVINE_BREASTPLATE)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ARTIFACT, {IDX(ARTIFACT_TAX_LIEN)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, {IDX(ARTIFACT_HIDEOUS_MASK)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, {IDX(ARTIFACT_FIZBIN_OF_MISFORTUNE)}, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_NECROMANCER)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_WARLOCK)}, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, {IDX(FACTION_BARBARIAN)}, CHOICE_NO_AMOUNT}}}
};
DATA(0x00526688) char* congratsText = NULL;
DATA(0x004fd7ac) H2_CONST char* gArtifactNames[IDX(ARTIFACT_COUNT)] = {
    localization::Tr("table.gArtifactNames.0"),
    localization::Tr("table.gArtifactNames.1"),
    localization::Tr("table.gArtifactNames.2"),
    localization::Tr("table.gArtifactNames.3"),
    localization::Tr("table.gArtifactNames.4"),
    localization::Tr("table.gArtifactNames.5"),
    localization::Tr("table.gArtifactNames.6"),
    localization::Tr("table.gArtifactNames.7"),
    localization::Tr("table.gArtifactNames.8"),
    localization::Tr("table.gArtifactNames.9"),
    localization::Tr("table.gArtifactNames.10"),
    localization::Tr("table.gArtifactNames.11"),
    localization::Tr("table.gArtifactNames.12"),
    localization::Tr("table.gArtifactNames.13"),
    localization::Tr("table.gArtifactNames.14"),
    localization::Tr("table.gArtifactNames.15"),
    localization::Tr("table.gArtifactNames.16"),
    localization::Tr("table.gArtifactNames.17"),
    localization::Tr("table.gArtifactNames.18"),
    localization::Tr("table.gArtifactNames.19"),
    localization::Tr("table.gArtifactNames.20"),
    localization::Tr("table.gArtifactNames.21"),
    localization::Tr("table.gArtifactNames.22"),
    localization::Tr("table.gArtifactNames.23"),
    localization::Tr("table.gArtifactNames.24"),
    localization::Tr("table.gArtifactNames.25"),
    localization::Tr("table.gArtifactNames.26"),
    localization::Tr("table.gArtifactNames.27"),
    localization::Tr("table.gArtifactNames.28"),
    localization::Tr("table.gArtifactNames.29"),
    localization::Tr("table.gArtifactNames.30"),
    localization::Tr("table.gArtifactNames.31"),
    localization::Tr("table.gArtifactNames.32"),
    localization::Tr("table.gArtifactNames.33"),
    localization::Tr("table.gArtifactNames.34"),
    localization::Tr("table.gArtifactNames.35"),
    localization::Tr("table.gArtifactNames.36"),
    localization::Tr("table.gArtifactNames.37"),
    localization::Tr("table.gArtifactNames.38"),
    localization::Tr("table.gArtifactNames.39"),
    localization::Tr("table.gArtifactNames.40"),
    localization::Tr("table.gArtifactNames.41"),
    localization::Tr("table.gArtifactNames.42"),
    localization::Tr("table.gArtifactNames.43"),
    localization::Tr("table.gArtifactNames.44"),
    localization::Tr("table.gArtifactNames.45"),
    localization::Tr("table.gArtifactNames.46"),
    localization::Tr("table.gArtifactNames.47"),
    localization::Tr("table.gArtifactNames.48"),
    localization::Tr("table.gArtifactNames.49"),
    localization::Tr("table.gArtifactNames.50"),
    localization::Tr("table.gArtifactNames.51"),
    localization::Tr("table.gArtifactNames.52"),
    localization::Tr("table.gArtifactNames.53"),
    localization::Tr("table.gArtifactNames.54"),
    localization::Tr("table.gArtifactNames.55"),
    localization::Tr("table.gArtifactNames.56"),
    localization::Tr("table.gArtifactNames.57"),
    localization::Tr("table.gArtifactNames.58"),
    localization::Tr("table.gArtifactNames.59"),
    localization::Tr("table.gArtifactNames.60"),
    localization::Tr("table.gArtifactNames.61"),
    localization::Tr("table.gArtifactNames.62"),
    localization::Tr("table.gArtifactNames.63"),
    localization::Tr("table.gArtifactNames.64"),
    localization::Tr("table.gArtifactNames.65"),
    localization::Tr("table.gArtifactNames.66"),
    localization::Tr("table.gArtifactNames.67"),
    localization::Tr("table.gArtifactNames.68"),
    localization::Tr("table.gArtifactNames.69"),
    localization::Tr("table.gArtifactNames.70"),
    localization::Tr("table.gArtifactNames.71"),
    localization::Tr("table.gArtifactNames.72"),
    localization::Tr("table.gArtifactNames.73"),
    localization::Tr("table.gArtifactNames.74"),
    localization::Tr("table.gArtifactNames.75"),
    localization::Tr("table.gArtifactNames.76"),
    localization::Tr("table.gArtifactNames.77"),
    localization::Tr("table.gArtifactNames.78"),
    localization::Tr("table.gArtifactNames.79"),
    localization::Tr("table.gArtifactNames.80"),
    localization::Tr("table.gArtifactNames.81"),
    "ERROR : Artifact 82" /* "ERROR : Artifact 82" */,
    "ERROR : Artifact 83" /* "ERROR : Artifact 83" */,
    "ERROR : Artifact 84" /* "ERROR : Artifact 84" */,
    "ERROR : Artifact 85" /* "ERROR : Artifact 85" */,
    localization::Tr("table.gArtifactNames.86"),
    localization::Tr("table.gArtifactNames.87"),
    localization::Tr("table.gArtifactNames.88"),
    localization::Tr("table.gArtifactNames.89"),
    localization::Tr("table.gArtifactNames.90"),
    localization::Tr("table.gArtifactNames.91"),
    localization::Tr("table.gArtifactNames.92"),
    localization::Tr("table.gArtifactNames.93"),
    localization::Tr("table.gArtifactNames.94"),
    localization::Tr("table.gArtifactNames.95"),
    localization::Tr("table.gArtifactNames.96"),
    localization::Tr("table.gArtifactNames.97"),
    localization::Tr("table.gArtifactNames.98"),
    localization::Tr("table.gArtifactNames.99"),
    localization::Tr("table.gArtifactNames.100"),
    localization::Tr("table.gArtifactNames.101"),
    localization::Tr("table.gArtifactNames.102")
};
DATA(0x004fd948) H2_CONST char* gArtifactDesc[IDX(ARTIFACT_COUNT)] = {
    localization::Tr("table.gArtifactDesc.0"),
    localization::Tr("table.gArtifactDesc.1"),
    localization::Tr("table.gArtifactDesc.2"),
    localization::Tr("table.gArtifactDesc.3"),
    localization::Tr("table.gArtifactDesc.4"),
    localization::Tr("table.gArtifactDesc.5"),
    localization::Tr("table.gArtifactDesc.6"),
    localization::Tr("table.gArtifactDesc.7"),
    localization::Tr("table.gArtifactDesc.8"),
    localization::Tr("table.gArtifactDesc.9"),
    localization::Tr("table.gArtifactDesc.10"),
    localization::Tr("table.gArtifactDesc.11"),
    localization::Tr("table.gArtifactDesc.12"),
    localization::Tr("table.gArtifactDesc.13"),
    localization::Tr("table.gArtifactDesc.14"),
    localization::Tr("table.gArtifactDesc.15"),
    localization::Tr("table.gArtifactDesc.16"),
    localization::Tr("table.gArtifactDesc.17"),
    localization::Tr("table.gArtifactDesc.18"),
    localization::Tr("table.gArtifactDesc.19"),
    localization::Tr("table.gArtifactDesc.20"),
    localization::Tr("table.gArtifactDesc.21"),
    localization::Tr("table.gArtifactDesc.22"),
    localization::Tr("table.gArtifactDesc.23"),
    localization::Tr("table.gArtifactDesc.24"),
    localization::Tr("table.gArtifactDesc.25"),
    localization::Tr("table.gArtifactDesc.26"),
    localization::Tr("table.gArtifactDesc.27"),
    localization::Tr("table.gArtifactDesc.28"),
    localization::Tr("table.gArtifactDesc.29"),
    localization::Tr("table.gArtifactDesc.30"),
    localization::Tr("table.gArtifactDesc.31"),
    localization::Tr("table.gArtifactDesc.32"),
    localization::Tr("table.gArtifactDesc.33"),
    localization::Tr("table.gArtifactDesc.34"),
    localization::Tr("table.gArtifactDesc.35"),
    localization::Tr("table.gArtifactDesc.36"),
    localization::Tr("table.gArtifactDesc.37"),
    localization::Tr("table.gArtifactDesc.38"),
    localization::Tr("table.gArtifactDesc.39"),
    localization::Tr("table.gArtifactDesc.40"),
    localization::Tr("table.gArtifactDesc.41"),
    localization::Tr("table.gArtifactDesc.42"),
    localization::Tr("table.gArtifactDesc.43"),
    localization::Tr("table.gArtifactDesc.44"),
    localization::Tr("table.gArtifactDesc.45"),
    localization::Tr("table.gArtifactDesc.46"),
    localization::Tr("table.gArtifactDesc.47"),
    localization::Tr("table.gArtifactDesc.48"),
    localization::Tr("table.gArtifactDesc.49"),
    localization::Tr("table.gArtifactDesc.50"),
    localization::Tr("table.gArtifactDesc.51"),
    localization::Tr("table.gArtifactDesc.52"),
    localization::Tr("table.gArtifactDesc.53"),
    localization::Tr("table.gArtifactDesc.54"),
    localization::Tr("table.gArtifactDesc.55"),
    localization::Tr("table.gArtifactDesc.56"),
    localization::Tr("table.gArtifactDesc.57"),
    localization::Tr("table.gArtifactDesc.58"),
    localization::Tr("table.gArtifactDesc.59"),
    localization::Tr("table.gArtifactDesc.60"),
    localization::Tr("table.gArtifactDesc.61"),
    localization::Tr("table.gArtifactDesc.62"),
    localization::Tr("table.gArtifactDesc.63"),
    localization::Tr("table.gArtifactDesc.64"),
    localization::Tr("table.gArtifactDesc.65"),
    localization::Tr("table.gArtifactDesc.66"),
    localization::Tr("table.gArtifactDesc.67"),
    localization::Tr("table.gArtifactDesc.68"),
    localization::Tr("table.gArtifactDesc.69"),
    localization::Tr("table.gArtifactDesc.70"),
    localization::Tr("table.gArtifactDesc.71"),
    localization::Tr("table.gArtifactDesc.72"),
    localization::Tr("table.gArtifactDesc.73"),
    localization::Tr("table.gArtifactDesc.74"),
    localization::Tr("table.gArtifactDesc.75"),
    localization::Tr("table.gArtifactDesc.76"),
    localization::Tr("table.gArtifactDesc.77"),
    localization::Tr("table.gArtifactDesc.78"),
    localization::Tr("table.gArtifactDesc.79"),
    localization::Tr("table.gArtifactDesc.80"),
    localization::Tr("table.gArtifactDesc.81"),
    "{ERROR}\n\nArtifact 82." /* "{ERROR}\n\nArtifact 82." */,
    "{ERROR}\n\nArtifact 83." /* "{ERROR}\n\nArtifact 83." */,
    "{ERROR}\n\nArtifact 84." /* "{ERROR}\n\nArtifact 84." */,
    "{ERROR}\n\nArtifact 85." /* "{ERROR}\n\nArtifact 85." */,
    localization::Tr("table.gArtifactDesc.86"),
    localization::Tr("table.gArtifactDesc.87"),
    localization::Tr("table.gArtifactDesc.88"),
    localization::Tr("table.gArtifactDesc.89"),
    localization::Tr("table.gArtifactDesc.90"),
    localization::Tr("table.gArtifactDesc.91"),
    localization::Tr("table.gArtifactDesc.92"),
    localization::Tr("table.gArtifactDesc.93"),
    localization::Tr("table.gArtifactDesc.94"),
    localization::Tr("table.gArtifactDesc.95"),
    localization::Tr("table.gArtifactDesc.96"),
    localization::Tr("table.gArtifactDesc.97"),
    localization::Tr("table.gArtifactDesc.98"),
    localization::Tr("table.gArtifactDesc.99"),
    localization::Tr("table.gArtifactDesc.100"),
    localization::Tr("table.gArtifactDesc.101"),
    localization::Tr("table.gArtifactDesc.102")};
DATA(0x004fdae4) H2_CONST char* gArtifactEvent[IDX(ARTIFACT_COUNT)] = {
    "" /* "" */,
    "" /* "" */,
    "" /* "" */,
    "" /* "" */,
    "" /* "" */,
    "" /* "" */,
    "" /* "" */,
    "" /* "" */,
    localization::Tr("table.gArtifactEvent.8"),
    localization::Tr("table.gArtifactEvent.9"),
    localization::Tr("table.gArtifactEvent.10"),
    localization::Tr("table.gArtifactEvent.11"),
    localization::Tr("table.gArtifactEvent.12"),
    localization::Tr("table.gArtifactEvent.13"),
    localization::Tr("table.gArtifactEvent.14"),
    localization::Tr("table.gArtifactEvent.15"),
    localization::Tr("table.gArtifactEvent.16"),
    localization::Tr("table.gArtifactEvent.17"),
    localization::Tr("table.gArtifactEvent.18"),
    localization::Tr("table.gArtifactEvent.19"),
    localization::Tr("table.gArtifactEvent.20"),
    localization::Tr("table.gArtifactEvent.21"),
    localization::Tr("table.gArtifactEvent.22"),
    localization::Tr("table.gArtifactEvent.23"),
    localization::Tr("table.gArtifactEvent.24"),
    localization::Tr("table.gArtifactEvent.25"),
    localization::Tr("table.gArtifactEvent.26"),
    localization::Tr("table.gArtifactEvent.27"),
    localization::Tr("table.gArtifactEvent.28"),
    localization::Tr("table.gArtifactEvent.29"),
    localization::Tr("table.gArtifactEvent.30"),
    localization::Tr("table.gArtifactEvent.31"),
    localization::Tr("table.gArtifactEvent.32"),
    localization::Tr("table.gArtifactEvent.33"),
    localization::Tr("table.gArtifactEvent.34"),
    localization::Tr("table.gArtifactEvent.35"),
    localization::Tr("table.gArtifactEvent.36"),
    localization::Tr("table.gArtifactEvent.37"),
    localization::Tr("table.gArtifactEvent.38"),
    localization::Tr("table.gArtifactEvent.39"),
    localization::Tr("table.gArtifactEvent.40"),
    localization::Tr("table.gArtifactEvent.41"),
    localization::Tr("table.gArtifactEvent.42"),
    localization::Tr("table.gArtifactEvent.43"),
    localization::Tr("table.gArtifactEvent.44"),
    localization::Tr("table.gArtifactEvent.45"),
    localization::Tr("table.gArtifactEvent.46"),
    localization::Tr("table.gArtifactEvent.47"),
    localization::Tr("table.gArtifactEvent.48"),
    localization::Tr("table.gArtifactEvent.49"),
    localization::Tr("table.gArtifactEvent.50"),
    localization::Tr("table.gArtifactEvent.51"),
    localization::Tr("table.gArtifactEvent.52"),
    localization::Tr("table.gArtifactEvent.53"),
    localization::Tr("table.gArtifactEvent.54"),
    localization::Tr("table.gArtifactEvent.55"),
    localization::Tr("table.gArtifactEvent.56"),
    localization::Tr("table.gArtifactEvent.57"),
    localization::Tr("table.gArtifactEvent.58"),
    localization::Tr("table.gArtifactEvent.59"),
    localization::Tr("table.gArtifactEvent.60"),
    localization::Tr("table.gArtifactEvent.61"),
    localization::Tr("table.gArtifactEvent.62"),
    localization::Tr("table.gArtifactEvent.63"),
    localization::Tr("table.gArtifactEvent.64"),
    localization::Tr("table.gArtifactEvent.65"),
    localization::Tr("table.gArtifactEvent.66"),
    localization::Tr("table.gArtifactEvent.67"),
    localization::Tr("table.gArtifactEvent.68"),
    localization::Tr("table.gArtifactEvent.69"),
    localization::Tr("table.gArtifactEvent.70"),
    localization::Tr("table.gArtifactEvent.71"),
    localization::Tr("table.gArtifactEvent.72"),
    localization::Tr("table.gArtifactEvent.73"),
    localization::Tr("table.gArtifactEvent.74"),
    localization::Tr("table.gArtifactEvent.75"),
    localization::Tr("table.gArtifactEvent.76"),
    localization::Tr("table.gArtifactEvent.77"),
    localization::Tr("table.gArtifactEvent.78"),
    localization::Tr("table.gArtifactEvent.79"),
    localization::Tr("table.gArtifactEvent.80"),
    "" /* "" */,
    "ERROR : Artifact event 82." /* "ERROR : Artifact event 82." */,
    "ERROR : Artifact event 83." /* "ERROR : Artifact event 83." */,
    "ERROR : Artifact event 84." /* "ERROR : Artifact event 84." */,
    "ERROR : Artifact event 85." /* "ERROR : Artifact event 85." */,
    localization::Tr("table.gArtifactEvent.86"),
    localization::Tr("table.gArtifactEvent.87"),
    localization::Tr("table.gArtifactEvent.88"),
    localization::Tr("table.gArtifactEvent.89"),
    localization::Tr("table.gArtifactEvent.90"),
    localization::Tr("table.gArtifactEvent.91"),
    localization::Tr("table.gArtifactEvent.92"),
    localization::Tr("table.gArtifactEvent.93"),
    localization::Tr("table.gArtifactEvent.94"),
    localization::Tr("table.gArtifactEvent.95"),
    localization::Tr("table.gArtifactEvent.96"),
    localization::Tr("table.gArtifactEvent.97"),
    localization::Tr("table.gArtifactEvent.98"),
    localization::Tr("table.gArtifactEvent.99"),
    localization::Tr("table.gArtifactEvent.100"),
    localization::Tr("table.gArtifactEvent.101"),
    localization::Tr("table.gArtifactEvent.102")};
DATA(0x004fdc80) H2_CONST char* gStatNames[HERO_PRIMARY_STAT_COUNT] = {
    localization::Tr("table.gStatNames.0"),
    localization::Tr("table.gStatNames.1"),
    localization::Tr("table.gStatNames.2"),
    localization::Tr("table.gStatNames.3")
};
DATA(0x004fdc90) H2_CONST char* gStatDesc[HERO_PRIMARY_STAT_COUNT] = {
    localization::Tr("table.gStatDesc.0"),
    localization::Tr("table.gStatDesc.1"),
    localization::Tr("table.gStatDesc.2"),
    localization::Tr("table.gStatDesc.3")
};
DATA(0x004fdca0) H2_CONST char* gAlignmentNames[KB_ALIGNMENT_NAME_COUNT] = {
    localization::Tr("table.gAlignmentNames.0"),
    localization::Tr("table.gAlignmentNames.1"),
    localization::Tr("table.gAlignmentNames.2"),
    localization::Tr("table.gAlignmentNames.3"),
    localization::Tr("table.gAlignmentNames.4"),
    localization::Tr("table.gAlignmentNames.5"),
    localization::Tr("table.gAlignmentNames.6"),
    localization::Tr("table.gAlignmentNames.7")
};
DATA(0x004fdcc0) H2_CONST char* gArmyShortNames[IDX(CREATURE_COUNT)] = {
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
DATA(0x004fddc8) H2_CONST char* gArmyNames[IDX(CREATURE_COUNT)] = {
    localization::Tr("table.gArmyNames.0"),
    localization::Tr("table.gArmyNames.1"),
    localization::Tr("table.gArmyNames.2"),
    localization::Tr("table.gArmyNames.3"),
    localization::Tr("table.gArmyNames.4"),
    localization::Tr("table.gArmyNames.5"),
    localization::Tr("table.gArmyNames.6"),
    localization::Tr("table.gArmyNames.7"),
    localization::Tr("table.gArmyNames.8"),
    localization::Tr("table.gArmyNames.9"),
    localization::Tr("table.gArmyNames.10"),
    localization::Tr("table.gArmyNames.11"),
    localization::Tr("table.gArmyNames.12"),
    localization::Tr("table.gArmyNames.13"),
    localization::Tr("table.gArmyNames.14"),
    localization::Tr("table.gArmyNames.15"),
    localization::Tr("table.gArmyNames.16"),
    localization::Tr("table.gArmyNames.17"),
    localization::Tr("table.gArmyNames.18"),
    localization::Tr("table.gArmyNames.19"),
    localization::Tr("table.gArmyNames.20"),
    localization::Tr("table.gArmyNames.21"),
    localization::Tr("table.gArmyNames.22"),
    localization::Tr("table.gArmyNames.23"),
    localization::Tr("table.gArmyNames.24"),
    localization::Tr("table.gArmyNames.25"),
    localization::Tr("table.gArmyNames.26"),
    localization::Tr("table.gArmyNames.27"),
    localization::Tr("table.gArmyNames.28"),
    localization::Tr("table.gArmyNames.29"),
    localization::Tr("table.gArmyNames.30"),
    localization::Tr("table.gArmyNames.31"),
    localization::Tr("table.gArmyNames.32"),
    localization::Tr("table.gArmyNames.33"),
    localization::Tr("table.gArmyNames.34"),
    localization::Tr("table.gArmyNames.35"),
    localization::Tr("table.gArmyNames.36"),
    localization::Tr("table.gArmyNames.37"),
    localization::Tr("table.gArmyNames.38"),
    localization::Tr("table.gArmyNames.39"),
    localization::Tr("table.gArmyNames.40"),
    localization::Tr("table.gArmyNames.41"),
    localization::Tr("table.gArmyNames.42"),
    localization::Tr("table.gArmyNames.43"),
    localization::Tr("table.gArmyNames.44"),
    localization::Tr("table.gArmyNames.45"),
    localization::Tr("table.gArmyNames.46"),
    localization::Tr("table.gArmyNames.47"),
    localization::Tr("table.gArmyNames.48"),
    localization::Tr("table.gArmyNames.49"),
    localization::Tr("table.gArmyNames.50"),
    localization::Tr("table.gArmyNames.51"),
    localization::Tr("table.gArmyNames.52"),
    localization::Tr("table.gArmyNames.53"),
    localization::Tr("table.gArmyNames.54"),
    localization::Tr("table.gArmyNames.55"),
    localization::Tr("table.gArmyNames.56"),
    localization::Tr("table.gArmyNames.57"),
    localization::Tr("table.gArmyNames.58"),
    localization::Tr("table.gArmyNames.59"),
    localization::Tr("table.gArmyNames.60"),
    localization::Tr("table.gArmyNames.61"),
    localization::Tr("table.gArmyNames.62"),
    localization::Tr("table.gArmyNames.63"),
    localization::Tr("table.gArmyNames.64"),
    localization::Tr("table.gArmyNames.65")
};
DATA(0x004fded0) H2_CONST char* gArmyNamesPlural[IDX(CREATURE_COUNT)] = {
    localization::Tr("table.gArmyNamesPlural.0"),
    localization::Tr("table.gArmyNamesPlural.1"),
    localization::Tr("table.gArmyNamesPlural.2"),
    localization::Tr("table.gArmyNamesPlural.3"),
    localization::Tr("table.gArmyNamesPlural.4"),
    localization::Tr("table.gArmyNamesPlural.5"),
    localization::Tr("table.gArmyNamesPlural.6"),
    localization::Tr("table.gArmyNamesPlural.7"),
    localization::Tr("table.gArmyNamesPlural.8"),
    localization::Tr("table.gArmyNamesPlural.9"),
    localization::Tr("table.gArmyNamesPlural.10"),
    localization::Tr("table.gArmyNamesPlural.11"),
    localization::Tr("table.gArmyNamesPlural.12"),
    localization::Tr("table.gArmyNamesPlural.13"),
    localization::Tr("table.gArmyNamesPlural.14"),
    localization::Tr("table.gArmyNamesPlural.15"),
    localization::Tr("table.gArmyNamesPlural.16"),
    localization::Tr("table.gArmyNamesPlural.17"),
    localization::Tr("table.gArmyNamesPlural.18"),
    localization::Tr("table.gArmyNamesPlural.19"),
    localization::Tr("table.gArmyNamesPlural.20"),
    localization::Tr("table.gArmyNamesPlural.21"),
    localization::Tr("table.gArmyNamesPlural.22"),
    localization::Tr("table.gArmyNamesPlural.23"),
    localization::Tr("table.gArmyNamesPlural.24"),
    localization::Tr("table.gArmyNamesPlural.25"),
    localization::Tr("table.gArmyNamesPlural.26"),
    localization::Tr("table.gArmyNamesPlural.27"),
    localization::Tr("table.gArmyNamesPlural.28"),
    localization::Tr("table.gArmyNamesPlural.29"),
    localization::Tr("table.gArmyNamesPlural.30"),
    localization::Tr("table.gArmyNamesPlural.31"),
    localization::Tr("table.gArmyNamesPlural.32"),
    localization::Tr("table.gArmyNamesPlural.33"),
    localization::Tr("table.gArmyNamesPlural.34"),
    localization::Tr("table.gArmyNamesPlural.35"),
    localization::Tr("table.gArmyNamesPlural.36"),
    localization::Tr("table.gArmyNamesPlural.37"),
    localization::Tr("table.gArmyNamesPlural.38"),
    localization::Tr("table.gArmyNamesPlural.39"),
    localization::Tr("table.gArmyNamesPlural.40"),
    localization::Tr("table.gArmyNamesPlural.41"),
    localization::Tr("table.gArmyNamesPlural.42"),
    localization::Tr("table.gArmyNamesPlural.43"),
    localization::Tr("table.gArmyNamesPlural.44"),
    localization::Tr("table.gArmyNamesPlural.45"),
    localization::Tr("table.gArmyNamesPlural.46"),
    localization::Tr("table.gArmyNamesPlural.47"),
    localization::Tr("table.gArmyNamesPlural.48"),
    localization::Tr("table.gArmyNamesPlural.49"),
    localization::Tr("table.gArmyNamesPlural.50"),
    localization::Tr("table.gArmyNamesPlural.51"),
    localization::Tr("table.gArmyNamesPlural.52"),
    localization::Tr("table.gArmyNamesPlural.53"),
    localization::Tr("table.gArmyNamesPlural.54"),
    localization::Tr("table.gArmyNamesPlural.55"),
    localization::Tr("table.gArmyNamesPlural.56"),
    localization::Tr("table.gArmyNamesPlural.57"),
    localization::Tr("table.gArmyNamesPlural.58"),
    localization::Tr("table.gArmyNamesPlural.59"),
    localization::Tr("table.gArmyNamesPlural.60"),
    localization::Tr("table.gArmyNamesPlural.61"),
    localization::Tr("table.gArmyNamesPlural.62"),
    localization::Tr("table.gArmyNamesPlural.63"),
    localization::Tr("table.gArmyNamesPlural.64"),
    localization::Tr("table.gArmyNamesPlural.65")
};
DATA(0x004fdfd8) H2_CONST char* gTerrainNames[IDX(TERRAIN_COUNT)] = {
    localization::Tr("table.gTerrainNames.0"),
    localization::Tr("table.gTerrainNames.1"),
    localization::Tr("table.gTerrainNames.2"),
    localization::Tr("table.gTerrainNames.3"),
    localization::Tr("table.gTerrainNames.4"),
    localization::Tr("table.gTerrainNames.5"),
    localization::Tr("table.gTerrainNames.6"),
    localization::Tr("table.gTerrainNames.7"),
    localization::Tr("table.gTerrainNames.8")
};
DATA(0x004fdffc) H2_CONST char* gResourceNames[RESOURCE_VALUE_COUNT] = {
    localization::Tr("table.gResourceNames.0"),
    localization::Tr("table.gResourceNames.1"),
    localization::Tr("table.gResourceNames.2"),
    localization::Tr("table.gResourceNames.3"),
    localization::Tr("table.gResourceNames.4"),
    localization::Tr("table.gResourceNames.5"),
    localization::Tr("table.gResourceNames.6")
};
// The localised build names the mine, not the resource it yields, in the
// adventure-map quick info; the English 2.1 tree has no such table and reads
// gResourceNames there. See docs/version-changes.md.
DATA(0x004fe018) H2_CONST char* gMineNames[KB_MINE_NAME_COUNT] = {
    localization::Tr("table.gMineNames.0"),
    localization::Tr("table.gMineNames.1"),
    localization::Tr("table.gMineNames.2"),
    localization::Tr("table.gMineNames.3"),
    localization::Tr("table.gMineNames.4"),
    localization::Tr("table.gMineNames.5"),
    localization::Tr("table.gMineNames.6")
};
DATA(0x004fe034) H2_CONST char* gQuickViewText[KB_QUICK_VIEW_TEXT_COUNT] = {
    "" /* "" */,
    localization::Tr("table.gQuickViewText.1"),
    localization::Tr("table.gQuickViewText.2"),
    localization::Tr("table.gQuickViewText.3"),
    localization::Tr("table.gQuickViewText.4"),
    localization::Tr("table.gQuickViewText.5"),
    localization::Tr("table.gQuickViewText.6"),
    localization::Tr("table.gQuickViewText.7"),
    localization::Tr("table.gQuickViewText.8"),
    localization::Tr("table.gQuickViewText.9"),
    localization::Tr("table.gQuickViewText.10"),
    localization::Tr("table.gQuickViewText.11"),
    localization::Tr("table.gQuickViewText.12"),
    localization::Tr("table.gQuickViewText.13"),
    localization::Tr("table.gQuickViewText.14"),
    localization::Tr("table.gQuickViewText.15"),
    localization::Tr("table.gQuickViewText.16"),
    localization::Tr("table.gQuickViewText.17"),
    localization::Tr("table.gQuickViewText.18"),
    localization::Tr("table.gQuickViewText.19"),
    localization::Tr("table.gQuickViewText.20"),
    localization::Tr("table.gQuickViewText.21"),
    localization::Tr("table.gQuickViewText.22"),
    localization::Tr("table.gQuickViewText.23"),
    localization::Tr("table.gQuickViewText.24"),
    localization::Tr("table.gQuickViewText.25"),
    localization::Tr("table.gQuickViewText.26"),
    localization::Tr("table.gQuickViewText.27"),
    "" /* "" */,
    localization::Tr("table.gQuickViewText.29"),
    localization::Tr("table.gQuickViewText.30"),
    localization::Tr("table.gQuickViewText.31"),
    localization::Tr("table.gQuickViewText.32"),
    localization::Tr("table.gQuickViewText.33"),
    localization::Tr("table.gQuickViewText.34"),
    localization::Tr("table.gQuickViewText.35"),
    localization::Tr("table.gQuickViewText.36"),
    localization::Tr("table.gQuickViewText.37"),
    localization::Tr("table.gQuickViewText.38"),
    localization::Tr("table.gQuickViewText.39"),
    localization::Tr("table.gQuickViewText.40"),
    localization::Tr("table.gQuickViewText.41"),
    localization::Tr("table.gQuickViewText.42"),
    localization::Tr("table.gQuickViewText.43"),
    localization::Tr("artifact.ultimate_generic"),
    localization::Tr("table.gQuickViewText.45"),
    localization::Tr("table.gQuickViewText.46"),
    localization::Tr("table.gQuickViewText.47"),
    localization::Tr("table.gQuickViewText.48"),
    localization::Tr("table.gQuickViewText.49"),
    "" /* "" */,
    localization::Tr("table.gQuickViewText.51"),
    localization::Tr("table.gQuickViewText.52"),
    localization::Tr("table.gQuickViewText.53"),
    localization::Tr("table.gQuickViewText.54"),
    localization::Tr("table.gQuickViewText.55"),
    localization::Tr("table.gQuickViewText.56"),
    "" /* "" */,
    localization::Tr("table.gQuickViewText.58"),
    localization::Tr("table.gQuickViewText.59"),
    localization::Tr("table.gQuickViewText.60"),
    localization::Tr("table.gQuickViewText.61"),
    localization::Tr("table.gQuickViewText.62"),
    localization::Tr("table.gQuickViewText.63"),
    localization::Tr("table.gQuickViewText.64"),
    localization::Tr("table.gQuickViewText.65"),
    localization::Tr("table.gQuickViewText.66"),
    localization::Tr("table.gQuickViewText.67"),
    localization::Tr("table.gQuickViewText.68"),
    localization::Tr("table.gQuickViewText.69"),
    localization::Tr("table.gQuickViewText.70"),
    localization::Tr("table.gQuickViewText.71"),
    localization::Tr("table.gQuickViewText.72"),
    localization::Tr("table.gQuickViewText.73"),
    localization::Tr("table.gQuickViewText.74"),
    localization::Tr("table.gQuickViewText.75"),
    localization::Tr("table.gQuickViewText.76"),
    localization::Tr("table.gQuickViewText.77"),
    localization::Tr("table.gQuickViewText.78"),
    localization::Tr("table.gQuickViewText.79"),
    localization::Tr("table.gQuickViewText.80"),
    localization::Tr("table.gQuickViewText.81"),
    localization::Tr("table.gQuickViewText.82"),
    localization::Tr("table.gQuickViewText.83"),
    localization::Tr("table.gQuickViewText.84"),
    localization::Tr("table.gQuickViewText.85"),
    localization::Tr("table.gQuickViewText.86"),
    localization::Tr("table.gQuickViewText.87"),
    localization::Tr("table.gQuickViewText.88"),
    localization::Tr("table.gQuickViewText.89"),
    localization::Tr("table.gQuickViewText.90"),
    localization::Tr("table.gQuickViewText.91"),
    localization::Tr("table.gQuickViewText.92"),
    localization::Tr("table.gQuickViewText.93"),
    localization::Tr("table.gQuickViewText.94"),
    localization::Tr("table.gQuickViewText.95"),
    localization::Tr("table.gQuickViewText.96"),
    localization::Tr("table.gQuickViewText.97"),
    localization::Tr("table.gQuickViewText.98"),
    localization::Tr("table.gQuickViewText.99"),
    localization::Tr("table.gQuickViewText.100"),
    localization::Tr("table.gQuickViewText.101"),
    localization::Tr("table.gQuickViewText.102"),
    localization::Tr("table.gQuickViewText.103"),
    localization::Tr("table.gQuickViewText.104"),
    localization::Tr("table.gQuickViewText.105"),
    localization::Tr("table.gQuickViewText.106"),
    localization::Tr("table.gQuickViewText.107"),
    localization::Tr("table.gQuickViewText.108"),
    localization::Tr("table.gQuickViewText.109"),
    localization::Tr("table.gQuickViewText.110"),
    localization::Tr("table.gQuickViewText.111"),
    localization::Tr("table.gQuickViewText.112"),
    localization::Tr("table.gQuickViewText.113"),
    localization::Tr("table.gQuickViewText.114"),
    localization::Tr("table.gQuickViewText.115"),
    localization::Tr("table.gQuickViewText.116"),
    localization::Tr("table.gQuickViewText.117"),
    localization::Tr("table.gQuickViewText.118"),
    localization::Tr("table.gQuickViewText.119"),
    localization::Tr("table.gQuickViewText.120"),
    "%s" /* "%s" */,
    "%s" /* "%s" */,
    localization::Tr("table.gQuickViewText.123")
};
DATA(0x004fe224) H2_CONST char* gEventText[KB_EVENT_TEXT_TABLE_COUNT] = {
    // Алхимик\n\nВы стали хозяином лаборатории местного алхимика. Она будет приносить вам по одной
    // единице ртути в день.
    localization::Tr("table.gEventText.0"),
    // Указатель\n\nНа указателе написано:\n\n%s находится неподалеку отсюда.
    localization::Tr("table.gEventText.1"),
    // Буй\n\nВаши спутники замечают морской буй. Он указывает верный курс.
    localization::Tr("table.gEventText.2"),
    // Буй\n\nВаши спутники замечают морской буй. Он указывает верный курс, и это повышает их боевой
    // дух.
    localization::Tr("table.gEventText.3"),
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    // Кольцо фейри\n\nВаше войско вступает внутрь кольца фейри, но ничего не происходит.
    localization::Tr("table.gEventText.12"),
    // Кольцо фейри\n\nВаше войско вступает внутрь кольца фейри, чары которого принесут вам удачу в
    // грядущем сражении.
    localization::Tr("table.gEventText.13"),
    // Костер\n\nОбыскав вражеский лагерь, вы находите спрятанный клад.
    localization::Tr("table.gEventText.14"),
    // Фонтан\n\nВы припадаете к струям волшебного фонтана, но ничего не происходит.
    localization::Tr("table.gEventText.15"),
    // Фонтан\n\nБлагоуханная влага волшебного фонтана принесет вам удачу в грядущем сражении.
    localization::Tr("table.gEventText.16"),
    // Беседка\n\nНа ступенях беседки появляется старый рыцарь. \"Мне жаль, храбрый воин, но я уже
    // научил тебя всему, что знаю сам.\"
    localization::Tr("table.gEventText.17"),
    // Беседка\n\nНа ступенях беседки появляется старый рыцарь. \"О храбрый воин, я научу тебя всему,
    // что знаю сам; пусть мой опыт поможет тебе в твоих странствиях.\"
    localization::Tr("table.gEventText.18"),
    // Лампа джинна\n\nВы находите засыпанную землей помятую и закопченную лампа. Хотите ее потереть?
    localization::Tr("table.gEventText.19"),
    // Кладбище\n\nВы осторожно приближаетесь к захоронению древних воинов. Хотите вскрыть их могилы?
    localization::Tr("table.gEventText.20"),
    // Одержав победу над зомби, вы несколько часов подряд обыскиваете могилы, но ничего не находите.
    // Ваш недостойный поступок отрицательно влияет на боевой дух войска.
    localization::Tr("table.gEventText.21"),
    // Одержав победу над зомби, вы обыскиваете могилы и удаляетесь с находкой!
    localization::Tr("table.gEventText.22"),
    // {Дом стрелков}\n\nГруппа стрелков в поисках славы желает примкнуть к вашему войску. Согласны ли
    // вы принять их?
    localization::Tr("table.gEventText.23"),
    // В вашем войске нет места для новых рекрутов.
    localization::Tr("table.gEventText.24"),
    // {Дом стрелков}\n\nПриблизившись к жилищу, вы обнаруживаете, что оно пустует.
    localization::Tr("table.gEventText.25"),
    // Хибара гоблинов\n\nГруппа гоблинов в поисках славы желает примкнуть к вашему войску. Согласны ли
    // вы принять их?
    localization::Tr("table.gEventText.26"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    localization::Tr("table.gEventText.27"),
    // Хибара гоблинов\n\nПриблизившись к жилищу гоблинов, вы обнаруживаете, что оно пустует.
    localization::Tr("table.gEventText.28"),
    // Хижина крестьян\n\nГруппа крестьян в поисках славы желает примкнуть к вашему войску. Согласны ли
    // вы принять их?
    localization::Tr("table.gEventText.29"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    localization::Tr("table.gEventText.30"),
    // Хижина крестьян\n\nПриблизившись к жилищу крестьян, вы обнаруживаете, что оно пустует.
    localization::Tr("table.gEventText.31"),
    // Избушка гномов\n\nГруппа стрелков в поисках славы желает примкнуть к вашему войску. Согласны ли
    // вы принять их?
    localization::Tr("table.gEventText.32"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    localization::Tr("table.gEventText.33"),
    // Избушка гномов\n\nПриблизившись к жилищу стрелков, вы обнаруживаете, что оно пустует.
    localization::Tr("table.gEventText.34"),
    // {Мазанка}\n\nГруппа крестьян в поисках славы желает примкнуть к вашему войску. Согласны ли вы
    // принять их?
    localization::Tr("table.gEventText.35"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    localization::Tr("table.gEventText.36"),
    // {Мазанка}\n\nПриблизившись к жилищу Крестьян, вы обнаруживаете, что оно пустует.
    localization::Tr("table.gEventText.37"),
    // {Древо-дом}\n\nГруппа фей в поисках славы желает примкнуть к вашему войску. Согласны ли вы
    // принять их?
    localization::Tr("table.gEventText.38"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    localization::Tr("table.gEventText.39"),
    // {Древо-дом}\n\nПриблизившись к древесному дому Фей, вы обнаруживаете, что он пустует.
    localization::Tr("table.gEventText.40"),
    // {Нора полуросликов}\n\nГруппа полуросликов в поисках славы желает примкнуть к вашему войску.
    // Согласны ли вы принять их?
    localization::Tr("table.gEventText.41"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    localization::Tr("table.gEventText.42"),
    // {Нора полуросликов}\n\nПриблизившись к норе полуросликов, вы обнаруживаете, что она пустует.
    localization::Tr("table.gEventText.43"),
    // {Сторожевая вышка}\n\nГруппа орков в поисках славы желает примкнуть к вашему войску. Согласны ли
    // вы принять их?
    localization::Tr("table.gEventText.44"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    localization::Tr("table.gEventText.45"),
    // {Сторожевая вышка}\n\nПриблизившись к сторожевой вышке орков, вы обнаруживаете, что она пустует.
    localization::Tr("table.gEventText.46"),
    // {Снежная пещера}\n\nГруппа кентавров в поисках славы желает примкнуть к вашему войску. Согласны
    // ли вы принять их?
    localization::Tr("table.gEventText.47"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    localization::Tr("table.gEventText.48"),
    // {Пещера}\n\nПриблизившись к пещере кентавров, вы обнаруживаете, что она пустует.
    localization::Tr("table.gEventText.49"),
    // {Раскопки}\n\nГруппа скелетов в поисках славы желает примкнуть к вашему войску. Согласны ли вы
    // принять их?
    localization::Tr("table.gEventText.50"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    localization::Tr("table.gEventText.51"),
    // {Раскопки}\n\nПриблизившись к захоронению скелетов, вы обнаруживаете, что оно пустует.
    localization::Tr("table.gEventText.52"),
    "",
    "",
    "",
    "",
    "",
    // Маяк\n\nТеперь маяк ваш, и все ваши корабли будут преодолевать большее расстояние за один ход.
    localization::Tr("table.gEventText.58"),
    // Водяная мельница\n\nМельник обращается к вам со словами: \"Сожалею, господин, но сегодня золота
    // у меня нет. Приходите на следующей неделе.\"
    localization::Tr("table.gEventText.59"),
    // Водяная мельница\n\nМельник обращается к вам со словами: \"Господин, я трудился в поте лица и
    // прошу вас принять мою скромную лепту. Приходите на следующей неделе, и вы получите еще столько
    // же.\"
    localization::Tr("table.gEventText.60"),
    // Рудная шахта\n\nВы стали хозяином рудной шахты. Она будет приносить вам по две меры руды в день.
    localization::Tr("table.gEventText.61"),
    // Серная шахта\n\nВы стали хозяином серной шахты. Она будут приносить вам по 1 единице серы в
    // день.
    localization::Tr("table.gEventText.62"),
    // Кристальная шахта\n\nВы стали хозяином кристальной шахты. Она будет приносить вам по одной мере
    // кристаллов в день.
    localization::Tr("table.gEventText.63"),
    // Самоцветная шахта\n\nВы стали хозяином самоцветной шахты. Она будет приносить вам по 1 единице
    // самоцветов в день.
    localization::Tr("table.gEventText.64"),
    // Золотая шахта\n\nВы стали хозяином золотой шахты. Она будет приносить вам по 1000 золотых в
    // день.
    localization::Tr("table.gEventText.65"),
    // Последователи\n\nГруппа %s в поисках славы желает примкнуть к вашему войску. Вы согласны принять
    // их?
    localization::Tr("table.gEventText.66"),
    // Оскорбленные отказом быть принятыми в ваши ряды, они нападают на вас!
    localization::Tr("table.gEventText.67"),
    // Обелиск\n\nПеред вами обелиск, высеченный из невиданного камня. Вы вглядываетесь в его гладкую
    // поверхность и вдруг замечаете, что на ней начинают проступать таинственные знаки. Знаки
    // складываются во фрагмент древней карты. Вы торопливо срисовываете его, и знаки исчезают так же
    // внезапно, как и появились.
    localization::Tr("table.gEventText.68"),
    // Обелиск\n\nВы уже посещали этот обелиск.
    localization::Tr("table.gEventText.69"),
    "",
    "",
    // Вы нашли ресурс (%s).
    localization::Tr("table.gEventText.72"),
    // Лесопилка\n\nВы стали хозяином лесопилки. Она будет приносить вам по 2 единицы древесины в день.
    localization::Tr("table.gEventText.73"),
    // {Оракул}\n\nНа поляне в окружении деревьев восседает слепой оракул. Вы рассказываете ему о целях
    // вашего похода, и он показывает вам сильные и слабые стороны ваших противников в магическом
    // хрустальном шаре.
    localization::Tr("table.gEventText.74"),
    "",
    "",
    "",
    "",
    "",
    "",
    // {Шатер}\n\nВаше внимание привлекает шатер, пологи которых трепещут на жарком ветру пустыни. В
    // нем никого нет. Пройдет время, и, быть может, сюда придет новый отряд кочевников.
    localization::Tr("table.gEventText.81"),
    // {Шатер}\n\nВаше внимание привлекают шатер, пологи которого трепещут на жарком ветру пустыни. Вы
    // хотите принять в ваше войско отряд кочевников?
    localization::Tr("table.gEventText.82"),
    // {Повозка}\n\nЦветастая повозка разбойников пуста. Пройдет время, и, быть может, здесь обоснуется
    // новая шайка.
    localization::Tr("table.gEventText.83"),
    // {Повозка}\n\nВдалеке слышится музыка и смех. Вы идете на звуки и видите цветастую повозку, в
    // которой живут разбойники. Вы хотите принять в ваше войско шайку разбойников?
    localization::Tr("table.gEventText.84"),
    // {Водоворот}\n\nВаш корабль попадает в водоворот. Часть вашего войска исчезает в пучине.
    localization::Tr("table.gEventText.85"),
    // {Ветряная мельница}\n\nМельник обращается к вам со словами: \"Сожалею, господин, но сегодня у
    // меня ничего нет. Приходите на следующей неделе.\"
    localization::Tr("table.gEventText.86"),
    // {Ветряная мельница}\n\nМельник обращается к вам со словами: \"Господин, я работал не покладая
    // рук, и прошу вас принять мой скромный дар. Приходите на следующей неделе, у меня опять найдется,
    // чем вас порадовать.\"
    localization::Tr("table.gEventText.87"),
    "",
    "",
    "",
    "",
    "",
    // {Скелет}\n\nВы находите останки незадачливого искателя приключений. Пошарив в груде лохмотьев,
    // вы ничего не находите.
    localization::Tr("table.gEventText.93"),
    // {Скелет}\n\nВы находите останки незадачливого искателя приключений. Пошарив в груде лохмотьев,
    // вы находите.
    localization::Tr("table.gEventText.94")
};
DATA(0x004fe3a0) H2_CONST char* gCPanelHelp[KB_CONTROL_PANEL_HELP_COUNT] = {
    // Начать одиночную или сетевую игру.
    localization::Tr("table.gCPanelHelp.0"),
    // Загрузить сохраненную игру.
    localization::Tr("table.gCPanelHelp.1"),
    // Сохранить игру.
    localization::Tr("table.gCPanelHelp.2"),
    // Выйти из Героев Меча и Магии II.
    localization::Tr("table.gCPanelHelp.3"),
    // Закрыть меню, ничего не делая.
    localization::Tr("table.gCPanelHelp.4")
};
DATA(0x004fe3b4) H2_CONST char* gCSPanelHelp[KB_COMBAT_SPELL_PANEL_HELP_COUNT] = {
    // {ОК}\n\nЗакрыть это меню.
    localization::Tr("table.gCSPanelHelp.0"),
    // {Скорость}\n\nУстановить скорость действий и анимации воинов в бою.
    localization::Tr("table.gCSPanelHelp.1"),
    // {Информация о воине}\n\nВключить или выключить отображение окна с информацией о выбранном и
    // атакуемом воине.
    localization::Tr("table.gCSPanelHelp.2"),
    // {Магия в автобое}\n\nЕсли эта опция включена, ваш герой будет использовать заклинания во время
    // автобоя. (Примечание: Эта опция не влияет на использование заклинаний компьютерными игроками, и
    // на быстрый бой.)
    localization::Tr("table.gCSPanelHelp.3"),
    // {Сетка}\n\nВключает или выключает отображение сетки. Все перемещения на поле боя происходят по
    // гексагональной сетке, даже если ее отображение отключено.
    localization::Tr("table.gCSPanelHelp.4"),
    // {Затенение сетки}\n\nВключает или выключает режим обозначения возможной дальности передвижения
    // выбранного отряда воинов.
    localization::Tr("table.gCSPanelHelp.5"),
    // {Курсор с тенью}\n\nВключает или выключает отрисовку тени от курсора на сетке координат.
    localization::Tr("table.gCSPanelHelp.6")
};
DATA(0x004fe3d0) H2_CONST char* gAPanelHelp[KB_ADVENTURE_PANEL_HELP_COUNT] = {
    // Осмотреть весь мир.
    localization::Tr("table.gAPanelHelp.0"),
    // Посмотреть головоломку.
    localization::Tr("table.gAPanelHelp.1"),
    // Показать информацию о сценарии, на котором идет игра.
    localization::Tr("table.gAPanelHelp.2"),
    // Копать в поисках Великого артефакта.
    (localization::Tr("table.gAPanelHelp.3")),
    // Закрыть это меню.
    localization::Tr("table.gAPanelHelp.4")
};
DATA(0x004fe3e4) H2_CONST char* gInitMenuHelp[KB_INIT_MENU_HELP_COUNT] = {
    // {Новая игра}\n\nНачать отдельный сценарий или сетевую игру.
    localization::Tr("table.gInitMenuHelp.0"),
    // {Игры}\n\nЗагрузить ранее сохраненную игру.
    localization::Tr("table.gInitMenuHelp.1"),
    // {Рекорды}\n\nПоказать таблицу рекордов.
    localization::Tr("table.gInitMenuHelp.2"),
    // {Авторы}\n\nПоказать перечень авторов игры.
    localization::Tr("table.gInitMenuHelp.3"),
    // {Выйти}\n\nВыйти из героев Меча и Магии II и вернуться в операционную систему.
    localization::Tr("table.gInitMenuHelp.4")
};
DATA(0x004fe3f8) H2_CONST char* gAdvMenuHelp[KB_ADVENTURE_MENU_HELP_COUNT] = {
    // {Следующий герой}\n\nВыбрать следующего героя.
    localization::Tr("table.gAdvMenuHelp.0"),
    // {Продолжить движение}\n\nПродолжить движение героя по намеченному пути.
    localization::Tr("table.gAdvMenuHelp.1"),
    // {Обзор королевства}\n\nОсмотреть ваши владения.
    localization::Tr("table.gAdvMenuHelp.2"),
    // {Окончить ход}\n\nОкончить ход и передать управление компьютеру.
    localization::Tr("table.gAdvMenuHelp.3"),
    // {Игровые действия}\n\nОткрыть окно доступных игровых действий.
    localization::Tr("table.gAdvMenuHelp.4"),
    // {Окно файлов}\n\nОткрывает меню, где вы можете загружать или сохранять игры.
    localization::Tr("table.gAdvMenuHelp.5"),
    // {Системные настройки}\n\nОткрывает окно системных настроек, позволяющих настроить игру.
    localization::Tr("table.gAdvMenuHelp.6"),
    // {Направить заклинание}\n\nНаправить заклинание на стратегической карте.
    localization::Tr("table.gAdvMenuHelp.7")
};
DATA(0x004fe418) H2_CONST char* gLuckText[KB_LUCK_TEXT_COUNT] = {
    // Проклятая
    localization::Tr("table.gLuckText.0"),
    // Ужасная
    localization::Tr("table.gLuckText.1"),
    // Плохая
    localization::Tr("table.gLuckText.2"),
    // Обычная
    localization::Tr("table.gLuckText.3"),
    // Хорошая
    localization::Tr("table.gLuckText.4"),
    // Отличная
    localization::Tr("table.gLuckText.5"),
    // Божественная
    localization::Tr("table.gLuckText.6")
};
DATA(0x004fe434) H2_CONST char* gMoraleText[KB_MORALE_TEXT_COUNT] = {
    // Предательская
    localization::Tr("table.gMoraleText.0"),
    // Ужасная
    localization::Tr("table.gMoraleText.1"),
    // Плохая
    localization::Tr("table.gMoraleText.2"),
    // Обычная
    localization::Tr("table.gMoraleText.3"),
    // Хорошая
    localization::Tr("table.gMoraleText.4"),
    // Отличная
    localization::Tr("table.gMoraleText.5"),
    // Кровавая!
    localization::Tr("table.gMoraleText.6")
};
DATA(0x004fe450) H2_CONST char* onOffText[KB_ON_OFF_TEXT_COUNT] = {
    // Выкл.
    localization::Tr("table.onOffText.0"),
    // Вкл.
    localization::Tr("table.onOffText.1"),
    // Вкл.\nГромкость 9
    localization::Tr("table.onOffText.2"),
    // Вкл.\nГромкость 8
    localization::Tr("table.onOffText.3"),
    // Вкл.\nГромкость 7
    localization::Tr("table.onOffText.4"),
    // Вкл.\nГромкость 6
    localization::Tr("table.onOffText.5"),
    // Вкл.\nГромкость 5
    localization::Tr("table.onOffText.6"),
    // Вкл.\nГромкость 4
    localization::Tr("table.onOffText.7"),
    // Вкл.\nГромкость 3
    localization::Tr("table.onOffText.8"),
    // Вкл.\nГромкость 2
    localization::Tr("table.onOffText.9"),
    // Вкл.\nГромкость 1
    localization::Tr("table.onOffText.10")
};
DATA(0x004fe47c) H2_CONST char* walkSpeedText[KB_WALK_SPEED_TEXT_COUNT] = {
    // Шагом
    localization::Tr("table.walkSpeedText.0"),
    // Рысью
    localization::Tr("table.walkSpeedText.1"),
    // Аллюром
    localization::Tr("table.walkSpeedText.2"),
    // Галопом
    localization::Tr("table.walkSpeedText.3"),
    // Прыжками
    localization::Tr("table.walkSpeedText.4")
};
DATA(0x004fe490) H2_CONST char* gColors[IDX(FACTION_COUNT)] = {
    localization::Tr("table.gColors.0"),
    localization::Tr("table.gColors.1"),
    localization::Tr("table.gColors.2"),
    localization::Tr("table.gColors.3"),
    localization::Tr("table.gColors.4"),
    localization::Tr("table.gColors.5")
};
DATA(0x004fe4a8) static H2_CONST char* H2_UNUSED(gColorAbbreviations)[IDX(FACTION_COUNT)] = {
    localization::Tr("color.abbreviated.blue"),
    localization::Tr("color.abbreviated.green"),
    localization::Tr("color.abbreviated.red"),
    localization::Tr("color.abbreviated.yellow"),
    localization::Tr("color.abbreviated.orange"),
    localization::Tr("color.abbreviated.purple")
};
DATA(0x004fe4c0) H2_CONST char* gMonthNames[KB_MONTH_NAME_COUNT] = {
    // Кузнечика
    localization::Tr("table.gMonthNames.0"),
    // Муравья
    localization::Tr("table.gMonthNames.1"),
    // Стрекозы
    localization::Tr("table.gMonthNames.2"),
    // Паука
    localization::Tr("table.gMonthNames.3"),
    // Бабочки
    localization::Tr("table.gMonthNames.4"),
    // Шмеля
    localization::Tr("table.gMonthNames.5"),
    // Цикады
    localization::Tr("table.gMonthNames.6"),
    // Земляного червя
    localization::Tr("table.gMonthNames.7"),
    // Шершня
    localization::Tr("table.gMonthNames.8"),
    // Жука
    localization::Tr("table.gMonthNames.9")
};
DATA(0x004fe4e8) H2_CONST char* gWeekNames[KB_WEEK_NAME_COUNT] = {
    // Белки
    localization::Tr("table.gWeekNames.0"),
    // Кролика
    localization::Tr("table.gWeekNames.1"),
    // Суслика
    localization::Tr("table.gWeekNames.2"),
    // Барсука
    localization::Tr("table.gWeekNames.3"),
    // Крысы
    localization::Tr("table.gWeekNames.4"),
    // Орла
    localization::Tr("table.gWeekNames.5"),
    // Горностая
    localization::Tr("table.gWeekNames.6"),
    // Ворона
    localization::Tr("table.gWeekNames.7"),
    // Мангуста
    localization::Tr("table.gWeekNames.8"),
    // Собаки
    localization::Tr("table.gWeekNames.9"),
    // Муравьеда
    localization::Tr("table.gWeekNames.10"),
    // Ящерицы
    localization::Tr("table.gWeekNames.11"),
    // Черепахи
    localization::Tr("table.gWeekNames.12"),
    // Дикобраза
    localization::Tr("table.gWeekNames.13"),
    // Кондора
    localization::Tr("table.gWeekNames.14")
};
DATA(0x004fe524) H2_CONST char* cHeroScreen[KB_HERO_SCREEN_TEXT_COUNT] = {
    // Обзор королевства
    localization::Tr("table.cHeroScreen.0"),
    // %s - информация
    localization::Tr("table.cHeroScreen.1"),
    // Дополнительная статистика героя
    localization::Tr("table.cHeroScreen.2"),
    // Информация о высокой морали
    localization::Tr("table.cHeroScreen.3"),
    // Информация об обычной морали
    localization::Tr("table.cHeroScreen.4"),
    // Информация о плохой морали
    localization::Tr("table.cHeroScreen.5"),
    // Информация о хорошей удаче
    localization::Tr("table.cHeroScreen.6"),
    // Информация об обычной удаче
    localization::Tr("table.cHeroScreen.7"),
    // Информация о плохой удаче
    localization::Tr("table.cHeroScreen.8"),
    // Показать опыт
    localization::Tr("table.cHeroScreen.9"),
    // Выбрать %s
    localization::Tr("table.cHeroScreen.10"),
    // Пусто
    localization::Tr("table.cHeroScreen.11"),
    // Перенести сюда отряд %s
    localization::Tr("table.cHeroScreen.12"),
    // Отряды %s и %s меняются местами
    localization::Tr("table.cHeroScreen.13"),
    // Показать заклинания
    localization::Tr("table.cHeroScreen.14"),
    // Посмотреть информацию об: %s
    localization::Tr("table.cHeroScreen.15"),
    // %s %s - уволить
    localization::Tr("table.cHeroScreen.16"),
    // Закрыть экран героя
    localization::Tr("table.cHeroScreen.17"),
    // Экран героя
    localization::Tr("table.cHeroScreen.18"),
    // %s в один отряд
    localization::Tr("table.cHeroScreen.19"),
    // Разделить отряд %s
    localization::Tr("table.cHeroScreen.20"),
    // %s %s - информация
    localization::Tr("table.cHeroScreen.21"),
    // Информация об очках магии
    localization::Tr("table.cHeroScreen.22"),
    // Выбрать широкие ряды в бою
    localization::Tr("table.cHeroScreen.23"),
    // Сгруппировать воинов
    localization::Tr("table.cHeroScreen.24")
};
DATA(0x004fe588) H2_CONST char* cCastleInfo[KB_CASTLE_INFO_TEXT_COUNT] = {
    // Построить Гильдию магов
    localization::Tr("table.cCastleInfo.0"),
    // Построены все этажи Гильдии магов.
    localization::Tr("table.cCastleInfo.1"),
    // Нельзя построить следующий этаж.
    localization::Tr("table.cCastleInfo.2"),
    // Построить следующий этаж Гильдии магов
    localization::Tr("table.cCastleInfo.3"),
    // Постройка '%s' уже возведена
    localization::Tr("table.cCastleInfo.4"),
    // Нельзя возвести постройку '%s'
    localization::Tr("table.cCastleInfo.5"),
    // Нельзя возвести постройку '%s'
    localization::Tr("table.cCastleInfo.6"),
    // Возвести постройку '%s'
    localization::Tr("table.cCastleInfo.7"),
    // Герой вам не по карману.
    localization::Tr("table.cCastleInfo.8"),
    // Нельзя нанять - у вас уже %d героев.
    localization::Tr("table.cCastleInfo.9"),
    // Нельзя нанять - в этом городе у вас уже есть герой.
    localization::Tr("table.cCastleInfo.10"),
    // Нанять нового героя
    localization::Tr("town.recruit.new_hero"),
    // Выйти из замка
    localization::Tr("table.cCastleInfo.12"),
    // Возможности замка
    localization::Tr("table.cCastleInfo.13"),
    // Сгруппировать гарнизон
    localization::Tr("table.cCastleInfo.14"),
    // Выбрать широкие ряды для гарнизона
    localization::Tr("table.cCastleInfo.15")
};
DATA(0x004fe5c8) H2_CONST char* cLuckInfo[KB_LUCK_INFO_TEXT_COUNT] = {
    // {Хорошая удача}\n\nЕсли удача вашего войска выше обычной, атаки отдельных отрядов на поле боя
    // иногда оказываются более результативными (их сила удваивается).
    localization::Tr("table.cLuckInfo.0"),
    // {Обычная удача}\n\nС обычной удачей ваше войско не имеет ни преимуществ, ни недостатков на поле
    // боя.
    localization::Tr("table.cLuckInfo.1"),
    // {Плохая удача}\n\nЕсли вашему войску не везет, урон, наносимый  отдельными отрядами на поле боя,
    // может оказаться вдвое меньше обычного.
    localization::Tr("table.cLuckInfo.2"),
    // %s\n\n\nМодификаторы удачи:
    localization::Tr("table.cLuckInfo.3"),
    // \nЛапка кролика +1
    localization::Tr("table.cLuckInfo.4"),
    // \nЗолотая подкова +1
    localization::Tr("table.cLuckInfo.5"),
    // \nМонета +1
    localization::Tr("table.cLuckInfo.6"),
    // \nКлевер +1
    localization::Tr("table.cLuckInfo.7"),
    // \nПосещен Круг фейри +1
    localization::Tr("table.cLuckInfo.8"),
    // \nПосещен фонтан +1
    localization::Tr("table.cLuckInfo.9"),
    // \nНет
    localization::Tr("table.cLuckInfo.10"),
    // \nГрабитель могил -1
    localization::Tr("table.cLuckInfo.11"),
    // \nРадуга магов +2
    localization::Tr("table.cLuckInfo.12"),
    // \nПосещен идол +1
    localization::Tr("table.cLuckInfo.13"),
    // \nОграблена пирамида -2
    localization::Tr("table.cLuckInfo.14"),
    // \nБазовая удача +1
    localization::Tr("table.cLuckInfo.15"),
    // \nВысокая удача +2
    localization::Tr("table.cLuckInfo.16"),
    // \nЭксперт удачи +3
    localization::Tr("table.cLuckInfo.17"),
    // \nБонус мачты на море +1
    localization::Tr("table.cLuckInfo.18"),
    // \nПосещена русалка +1
    localization::Tr("table.cLuckInfo.19"),
    // \nБоевое одеяние Андурана дает максимальную удачу.
    localization::Tr("table.cLuckInfo.20")
};
DATA(0x004fe61c) H2_CONST char* IQnames[KB_IQ_NAME_COUNT] = {
    // Нет
    localization::Tr("table.IQnames.0"),
    // Глупый
    localization::Tr("table.IQnames.1"),
    // Средний
    localization::Tr("table.IQnames.2"),
    // Умный
    localization::Tr("table.IQnames.3"),
    // Гений
    localization::Tr("table.IQnames.4")
};
DATA(0x004fe630) H2_CONST char* cSpellHelp[KB_SPELL_HELP_TEXT_COUNT] = {
    // Предыдущая страница
    localization::Tr("table.cSpellHelp.0"),
    // Следующая страница
    localization::Tr("table.cSpellHelp.1"),
    // Небоевые заклинания
    localization::Tr("table.cSpellHelp.2"),
    // Боевые заклинания
    localization::Tr("table.cSpellHelp.3"),
    // Закрыть волшебную книгу
    localization::Tr("table.cSpellHelp.4"),
    // Заклинания
    localization::Tr("table.cSpellHelp.5"),
    // Выбрать заклинание
    localization::Tr("table.cSpellHelp.6"),
    // Боевые заклинания
    localization::Tr("table.cSpellHelp.7"),
    // У вашего героя осталось %d оч. магии
    (localization::Tr("table.cSpellHelp.8"))
};
DATA(0x004fe654) H2_CONST char* speedText[KB_SPEED_TEXT_COUNT] = {
    /*  */ "",
     localization::Tr("table.speedText.1"),
     localization::Tr("table.speedText.2"),
     localization::Tr("table.speedText.3"),
     localization::Tr("table.speedText.4"),
     localization::Tr("table.speedText.5"),
     localization::Tr("table.speedText.6"),
     localization::Tr("table.speedText.7"),
     localization::Tr("table.speedText.8"),
     localization::Tr("table.speedText.9")
};
DATA(0x004fe67c) H2_CONST char* cArmyDetail[KB_ARMY_DETAIL_TEXT_COUNT] = {
     localization::Tr("table.cArmyDetail.0"),
     localization::Tr("table.cArmyDetail.1"),
    /* Выстрелов:  */ localization::Tr("table.cArmyDetail.2"),
    /* Урон:  */ localization::Tr("table.cArmyDetail.3"),
    /* Здоровье:  */ localization::Tr("table.cArmyDetail.4"),
    /* Скорость:  */ localization::Tr("table.cArmyDetail.5"),
    /* Мораль:  */ localization::Tr("table.cArmyDetail.6"),
    /* Удача:  */ localization::Tr("table.cArmyDetail.7"),
    /* Выстрелов:  */ localization::Tr("table.cArmyDetail.8")
};
DATA(0x004fe6a0) H2_CONST char* cWellDetail[KB_WELL_DETAIL_TEXT_COUNT] = {
     localization::Tr("table.cWellDetail.0"),
     localization::Tr("table.cWellDetail.1"),
    /* Выстр.:  */ localization::Tr("table.cWellDetail.2"),
    /* Урон:  */ localization::Tr("table.cWellDetail.3"),
    /* ЗД:  */ localization::Tr("table.cWellDetail.4"),
    /* Скор.:  */ localization::Tr("table.cWellDetail.5"),
    /* Всего:  */ localization::Tr("table.cWellDetail.6"),
     localization::Tr("table.cWellDetail.7"),
     localization::Tr("table.cWellDetail.8")
};
DATA(0x004fe6c4) H2_CONST char* cKingdomOverview[KB_KINGDOM_OVERVIEW_TEXT_COUNT] = {

    (localization::Tr("table.cKingdomOverview.0")),
     localization::Tr("table.cKingdomOverview.1"),
     localization::Tr("table.cKingdomOverview.2")
};
DATA(0x004fe6d0) H2_CONST char* cNewTurn[KB_NEW_TURN_TEXT_COUNT] = {
     localization::Tr("table.cNewTurn.0"),
     localization::Tr("table.cNewTurn.1"),
     localization::Tr("table.cNewTurn.2"),
     localization::Tr("table.cNewTurn.3"),
     localization::Tr("table.cNewTurn.4"),
     localization::Tr("table.cNewTurn.5"),
     localization::Tr("table.cNewTurn.6")
};
DATA(0x004fe6ec) H2_CONST char* cViewGeneralLabels[KB_VIEW_GENERAL_LABEL_COUNT] = {
     localization::Tr("table.cViewGeneralLabels.0"),
     localization::Tr("table.cViewGeneralLabels.1"),
     localization::Tr("table.cViewGeneralLabels.2"),
     localization::Tr("table.cViewGeneralLabels.3"),
    /* Мораль:  */ localization::Tr("table.cViewGeneralLabels.4"),
    /* Удача:  */ localization::Tr("table.cViewGeneralLabels.5"),
     localization::Tr("table.cViewGeneralLabels.6")
};
DATA(0x004fe708) H2_CONST char* cViewGeneralHelp[KB_VIEW_GENERAL_HELP_COUNT] = {
     localization::Tr("table.cViewGeneralHelp.0"),
     localization::Tr("table.cViewGeneralHelp.1"),
     localization::Tr("table.cViewGeneralHelp.2"),
     localization::Tr("table.cViewGeneralHelp.3"),
     localization::Tr("table.cViewGeneralHelp.4"),
     localization::Tr("table.cViewGeneralHelp.5"),
     localization::Tr("table.cViewGeneralHelp.6")
};
DATA(0x004fe724) H2_CONST char* cViewGeneralLongHelp[KB_VIEW_GENERAL_LONG_HELP_COUNT] = {
     localization::Tr("table.cViewGeneralLongHelp.0"),
     localization::Tr("table.cViewGeneralLongHelp.1"),
     localization::Tr("table.cViewGeneralLongHelp.2"),
     localization::Tr("table.cViewGeneralLongHelp.3")
};
DATA(0x004fe734) H2_CONST char* cCombatMessage[KB_COMBAT_MESSAGE_COUNT] = {
    /*  */ "",
     localization::Tr("table.cCombatMessage.1"),
     localization::Tr("table.cCombatMessage.2"),
     localization::Tr("table.cCombatMessage.3"),
     localization::Tr("table.cCombatMessage.4"),
     localization::Tr("table.cCombatMessage.5"),
     localization::Tr("table.cCombatMessage.6"),
     localization::Tr("table.cCombatMessage.7"),
     localization::Tr("table.cCombatMessage.8"),
     localization::Tr("table.cCombatMessage.9"),
     localization::Tr("table.cCombatMessage.10"),
     localization::Tr("table.cCombatMessage.11")
};
DATA(0x004fe764) H2_CONST char* cHeroLevel[KB_HERO_LEVEL_TEXT_COUNT] =
    { localization::Tr("table.cHeroLevel.0"), /*  уровень опыта.\n */ localization::Tr("table.cHeroLevel.1"), /*  %d уровней опыта.\n */ localization::Tr("table.cHeroLevel.2")};
DATA(0x004fe770) H2_CONST char* cCombatHelp[KB_COMBAT_HELP_COUNT] = {
     localization::Tr("table.cCombatHelp.0"),
     localization::Tr("table.cCombatHelp.1"),
     localization::Tr("table.cCombatHelp.2"),
     localization::Tr("table.cCombatHelp.3"),
    /*  */ ""
};
DATA(0x004fe784) H2_CONST char* cLongCombatHelp[KB_LONG_COMBAT_HELP_COUNT] = {
     localization::Tr("table.cLongCombatHelp.0"),
     localization::Tr("table.cLongCombatHelp.1"),
     localization::Tr("table.cLongCombatHelp.2"),
     localization::Tr("table.cLongCombatHelp.3"),
     localization::Tr("table.cLongCombatHelp.4")
};
DATA(0x004fe798) H2_CONST char* cTownCommand[KB_TOWN_COMMAND_COUNT] = {
     localization::Tr("table.cTownCommand.0"),
    /* Нельзя отнять последних воинов у героя  */ localization::Tr("table.cTownCommand.1"),
     localization::Tr("table.cTownCommand.2"),
     localization::Tr("table.cTownCommand.3"),
     localization::Tr("table.cTownCommand.4"),
     localization::Tr("table.cTownCommand.5"),
     localization::Tr("table.cTownCommand.6"),
     localization::Tr("table.cTownCommand.7"),
     localization::Tr("table.cTownCommand.8"),
    /*  */ "",
     localization::Tr("table.cTownCommand.10"),
     localization::Tr("table.cTownCommand.11"),
    /* %s */ "%s",
     localization::Tr("table.cTownCommand.13"),
     localization::Tr("table.cTownCommand.14"),
     localization::Tr("table.cTownCommand.15"),
     localization::Tr("table.cTownCommand.16"),
     localization::Tr("table.cTownCommand.17"),
     localization::Tr("table.cTownCommand.18"),
     localization::Tr("table.cTownCommand.19"),
     localization::Tr("table.cTownCommand.20"),
     localization::Tr("table.cTownCommand.21"),
     localization::Tr("table.cTownCommand.22"),
     localization::Tr("table.cTownCommand.23"),
     localization::Tr("table.cTownCommand.24"),
     localization::Tr("table.cTownCommand.25"),
     localization::Tr("table.cTownCommand.26"),
     localization::Tr("table.cTownCommand.27")
};
DATA(0x004fe808) H2_CONST char* gHeroDefaultNames[KB_HERO_DEFAULT_NAME_COUNT] = {
     localization::Tr("table.gHeroDefaultNames.0"), localization::Tr("table.gHeroDefaultNames.1"), localization::Tr("table.gHeroDefaultNames.2"), localization::Tr("table.gHeroDefaultNames.3"), localization::Tr("table.gHeroDefaultNames.4"), localization::Tr("table.gHeroDefaultNames.5"), localization::Tr("table.gHeroDefaultNames.6"),
     localization::Tr("table.gHeroDefaultNames.7"), localization::Tr("table.gHeroDefaultNames.8"), localization::Tr("table.gHeroDefaultNames.9"), localization::Tr("table.gHeroDefaultNames.10"), localization::Tr("table.gHeroDefaultNames.11"), localization::Tr("table.gHeroDefaultNames.12"), localization::Tr("table.gHeroDefaultNames.13"),
     localization::Tr("table.gHeroDefaultNames.14"), localization::Tr("table.gHeroDefaultNames.15"), localization::Tr("table.gHeroDefaultNames.16"), localization::Tr("table.gHeroDefaultNames.17"), localization::Tr("table.gHeroDefaultNames.18"), localization::Tr("table.gHeroDefaultNames.19"), localization::Tr("table.gHeroDefaultNames.20"),
     localization::Tr("table.gHeroDefaultNames.21"), localization::Tr("table.gHeroDefaultNames.22"), localization::Tr("table.gHeroDefaultNames.23"), localization::Tr("table.gHeroDefaultNames.24"), localization::Tr("table.gHeroDefaultNames.25"), localization::Tr("table.gHeroDefaultNames.26"), localization::Tr("table.gHeroDefaultNames.27"),
     localization::Tr("table.gHeroDefaultNames.28"), localization::Tr("table.gHeroDefaultNames.29"), localization::Tr("table.gHeroDefaultNames.30"), localization::Tr("table.gHeroDefaultNames.31"), localization::Tr("table.gHeroDefaultNames.32"), localization::Tr("table.gHeroDefaultNames.33"), localization::Tr("table.gHeroDefaultNames.34"),
     localization::Tr("table.gHeroDefaultNames.35"), localization::Tr("table.gHeroDefaultNames.36"), localization::Tr("table.gHeroDefaultNames.37"), localization::Tr("table.gHeroDefaultNames.38"), localization::Tr("table.gHeroDefaultNames.39"), localization::Tr("table.gHeroDefaultNames.40"), localization::Tr("table.gHeroDefaultNames.41"),
     localization::Tr("table.gHeroDefaultNames.42"), localization::Tr("table.gHeroDefaultNames.43"), localization::Tr("table.gHeroDefaultNames.44"), localization::Tr("table.gHeroDefaultNames.45"), localization::Tr("table.gHeroDefaultNames.46"), localization::Tr("table.gHeroDefaultNames.47"), localization::Tr("table.gHeroDefaultNames.48"),
     localization::Tr("table.gHeroDefaultNames.49"), localization::Tr("table.gHeroDefaultNames.50"), localization::Tr("table.gHeroDefaultNames.51"), localization::Tr("table.gHeroDefaultNames.52"), localization::Tr("table.gHeroDefaultNames.53")
};
DATA(0x004fe8e0) H2_CONST char* gNewGameHelp[KB_NEW_GAME_HELP_COUNT] = {
     localization::Tr("table.gNewGameHelp.0"),
     localization::Tr("table.gNewGameHelp.1"),
     localization::Tr("table.gNewGameHelp.2"),
     localization::Tr("table.gNewGameHelp.3"),
     localization::Tr("table.gNewGameHelp.4"),
     localization::Tr("table.gNewGameHelp.5"),
     localization::Tr("table.gNewGameHelp.6"),
     localization::Tr("table.gNewGameHelp.7")
};
DATA(0x004fe900) H2_CONST char* gSetupBaudHelp[KB_SETUP_BAUD_HELP_COUNT] = {
     localization::Tr("table.gSetupBaudHelp.0"),
     localization::Tr("table.gSetupBaudHelp.1"),
     localization::Tr("table.gSetupBaudHelp.2"),
     localization::Tr("table.gSetupBaudHelp.3"),
     localization::Tr("table.gSetupBaudHelp.4")
};
DATA(0x004fe914) H2_CONST char* gSetupComPortHelp[KB_SETUP_COM_PORT_HELP_COUNT] = {
     localization::Tr("table.gSetupComPortHelp.0"),
     localization::Tr("table.gSetupComPortHelp.1"),
     localization::Tr("table.gSetupComPortHelp.2"),
     localization::Tr("table.gSetupComPortHelp.3"),
     localization::Tr("table.gSetupComPortHelp.4")
};
DATA(0x004fe928) H2_CONST char* gSetupDCBaudHelp[KB_SETUP_DC_BAUD_HELP_COUNT] = {
     localization::Tr("table.gSetupDCBaudHelp.0"),
     localization::Tr("table.gSetupDCBaudHelp.1"),
     localization::Tr("table.gSetupDCBaudHelp.2"),
     localization::Tr("table.gSetupDCBaudHelp.3"),
     localization::Tr("table.gSetupDCBaudHelp.4")
};
DATA(0x004fe93c) H2_CONST char* gSetupDCComPortHelp[KB_SETUP_DC_COM_PORT_HELP_COUNT] = {
     localization::Tr("table.gSetupDCComPortHelp.0"),
     localization::Tr("table.gSetupDCComPortHelp.1"),
     localization::Tr("table.gSetupDCComPortHelp.2"),
     localization::Tr("table.gSetupDCComPortHelp.3"),
     localization::Tr("table.gSetupDCComPortHelp.4")
};
DATA(0x004fe950) H2_CONST char* gSetupHotSeatGameHelp[KB_SETUP_HOT_SEAT_HELP_COUNT] = {
     localization::Tr("table.gSetupHotSeatGameHelp.0"),
     localization::Tr("table.gSetupHotSeatGameHelp.1"),
     localization::Tr("table.gSetupHotSeatGameHelp.2"),
     localization::Tr("table.gSetupHotSeatGameHelp.3"),
     localization::Tr("table.gSetupHotSeatGameHelp.4"),
     localization::Tr("table.gSetupHotSeatGameHelp.5")
};
DATA(0x004fe968) H2_CONST char* gSetupModemGameHelp[KB_SETUP_MODEM_HELP_COUNT] = {
     localization::Tr("table.gSetupModemGameHelp.0"),

    (localization::Tr("table.gSetupModemGameHelp.1")),
     localization::Tr("table.gSetupModemGameHelp.2"),
     localization::Tr("table.gSetupModemGameHelp.3")
};
DATA(0x004fe978) H2_CONST char* gSetupDCGameHelp[KB_SETUP_DIRECT_CONNECT_HELP_COUNT] = {
     localization::Tr("table.gSetupDCGameHelp.0"),

    (localization::Tr("table.gSetupDCGameHelp.1")),
     localization::Tr("table.gSetupDCGameHelp.2"),
     localization::Tr("table.gSetupDCGameHelp.3")
};
DATA(0x004fe988) H2_CONST char* gSetupMultiPlayerGameHelp[KB_SETUP_MULTIPLAYER_HELP_COUNT] = {
     localization::Tr("table.gSetupMultiPlayerGameHelp.0"),
     localization::Tr("table.gSetupMultiPlayerGameHelp.1"),
     localization::Tr("table.gSetupMultiPlayerGameHelp.2"),
     localization::Tr("table.gSetupMultiPlayerGameHelp.3"),
     localization::Tr("table.gSetupMultiPlayerGameHelp.4")
};
DATA(0x004fe99c) H2_CONST char* gSetupNetworkGameHelp[KB_SETUP_NETWORK_HELP_COUNT] = {
     localization::Tr("table.gSetupNetworkGameHelp.0"),
     localization::Tr("table.gSetupNetworkGameHelp.1"),
     localization::Tr("table.gSetupNetworkGameHelp.2")
};
DATA(0x004fe9a8) H2_CONST char* gSetupNetworkGame2Help[KB_SETUP_NETWORK_SECOND_HELP_COUNT] = {
     localization::Tr("table.gSetupNetworkGame2Help.0"),
     localization::Tr("table.gSetupNetworkGame2Help.1"),
     localization::Tr("table.gSetupNetworkGame2Help.2"),
     localization::Tr("table.gSetupNetworkGame2Help.3")
};
DATA(0x004fe9b8) H2_CONST char* gSetupGameHelp[KB_SETUP_GAME_HELP_COUNT] = {
     localization::Tr("table.gSetupGameHelp.0"),
     localization::Tr("table.gSetupGameHelp.1"),
     localization::Tr("table.gSetupGameHelp.2"),
     localization::Tr("table.gSetupGameHelp.3")
};
DATA(0x004fe9c8) H2_CONST char* cBattleResults[KB_BATTLE_RESULT_TEXT_COUNT] = {
     localization::Tr("table.cBattleResults.0"),
     localization::Tr("table.cBattleResults.1"),
     localization::Tr("table.cBattleResults.2"),
     localization::Tr("table.cBattleResults.3"),
     localization::Tr("table.cBattleResults.4"),
     localization::Tr("table.cBattleResults.5"),
     localization::Tr("table.cBattleResults.6"),
     localization::Tr("table.cBattleResults.7"),
     localization::Tr("table.cBattleResults.8"),
     localization::Tr("table.cBattleResults.9"),
     localization::Tr("table.cBattleResults.10")
};
DATA(0x004fe9f4) H2_CONST char* cMoraleInfo[KB_MORALE_INFO_TEXT_COUNT] = {
     localization::Tr("table.cMoraleInfo.0"),
     localization::Tr("table.cMoraleInfo.1"),
     localization::Tr("table.cMoraleInfo.2"),
     localization::Tr("table.cMoraleInfo.3"),
     localization::Tr("table.cMoraleInfo.4"),
     localization::Tr("table.cMoraleInfo.5"),
     localization::Tr("table.cMoraleInfo.6"),
     localization::Tr("table.cMoraleInfo.7"),
     localization::Tr("table.cMoraleInfo.8"),
     localization::Tr("table.cMoraleInfo.9"),
     localization::Tr("table.cMoraleInfo.10"),
     localization::Tr("table.cMoraleInfo.11"),
     localization::Tr("table.cMoraleInfo.12"),
     localization::Tr("table.cMoraleInfo.13"),
     localization::Tr("table.cMoraleInfo.14"),
     localization::Tr("table.cMoraleInfo.15"),
     localization::Tr("table.cMoraleInfo.16"),
     localization::Tr("table.cMoraleInfo.17"),
     localization::Tr("table.cMoraleInfo.18"),
     localization::Tr("table.cMoraleInfo.19"),
     localization::Tr("table.cMoraleInfo.20"),
     localization::Tr("table.cMoraleInfo.21"),
     localization::Tr("table.cMoraleInfo.22"),
     localization::Tr("table.cMoraleInfo.23"),
     localization::Tr("table.cMoraleInfo.24"),
     localization::Tr("table.cMoraleInfo.25"),
     localization::Tr("table.cMoraleInfo.26"),
     localization::Tr("table.cMoraleInfo.27"),
     localization::Tr("table.cMoraleInfo.28"),
     localization::Tr("table.cMoraleInfo.29"),
     localization::Tr("table.cMoraleInfo.30"),
     localization::Tr("table.cMoraleInfo.31")
};
DATA(0x004fea74) H2_CONST char* cMapSize[KB_MAP_SIZE_TEXT_COUNT] = { localization::Tr("table.cMapSize.0"), localization::Tr("table.cMapSize.1"), localization::Tr("table.cMapSize.2"), localization::Tr("table.cMapSize.3")};
DATA(0x004fea84) H2_CONST char* cDifficulty[KB_DIFFICULTY_TEXT_COUNT] =
    { localization::Tr("table.cDifficulty.0"), localization::Tr("table.cDifficulty.1"), localization::Tr("table.cDifficulty.2"), localization::Tr("table.cDifficulty.3"), localization::Tr("table.cDifficulty.4")};
DATA(0x004fea98) H2_CONST char* cStartDifficulty[KB_START_DIFFICULTY_TEXT_COUNT] = { localization::Tr("table.cStartDifficulty.0"), localization::Tr("table.cStartDifficulty.1"), localization::Tr("table.cStartDifficulty.2"), localization::Tr("table.cStartDifficulty.3")};
DATA(0x004feaa8) H2_CONST char* cCampaignLeaders[KB_CAMPAIGN_LEADER_TEXT_COUNT] =
    { localization::Tr("table.cCampaignLeaders.0"), localization::Tr("table.cCampaignLeaders.1"), localization::Tr("table.cCampaignLeaders.2"), localization::Tr("table.cCampaignLeaders.3")};
DATA(0x004feab8) H2_CONST char* cWinText[KB_WIN_TEXT_COUNT] =
    { localization::Tr("table.cWinText.0"), localization::Tr("table.cWinText.1"), localization::Tr("table.cWinText.2"), localization::Tr("table.cWinText.3"), localization::Tr("table.cWinText.4")};
DATA(0x004feacc) H2_CONST char* cHumanDifficulty[KB_HUMAN_DIFFICULTY_TEXT_COUNT] =
    { localization::Tr("table.cHumanDifficulty.0"), localization::Tr("table.cHumanDifficulty.1"), localization::Tr("table.cHumanDifficulty.2"), localization::Tr("table.cHumanDifficulty.3"), localization::Tr("table.cHumanDifficulty.4")};
DATA(0x004feae0) H2_CONST char* cHumanInfoDifficulty[KB_HUMAN_INFO_DIFFICULTY_TEXT_COUNT] =
    { localization::Tr("table.cHumanInfoDifficulty.0"), localization::Tr("table.cHumanInfoDifficulty.1"), localization::Tr("table.cHumanInfoDifficulty.2"), localization::Tr("table.cHumanInfoDifficulty.3"), localization::Tr("table.cHumanInfoDifficulty.4")};
DATA(0x004feaf4) H2_CONST char* musicQualityText[KB_MUSIC_QUALITY_TEXT_COUNT] =
    {/* MIDI */ "MIDI", localization::Tr("table.musicQualityText.1"), localization::Tr("table.musicQualityText.2")};
DATA(0x004feb00) H2_CONST char* gSpellDesc[KB_SPELL_TEXT_COUNT] = {
     localization::Tr("table.gSpellDesc.0"),
     localization::Tr("table.gSpellDesc.1"),
     localization::Tr("table.gSpellDesc.2"),
     localization::Tr("table.gSpellDesc.3"),
     localization::Tr("table.gSpellDesc.4"),
     localization::Tr("table.gSpellDesc.5"),
     localization::Tr("table.gSpellDesc.6"),
     localization::Tr("table.gSpellDesc.7"),
     localization::Tr("table.gSpellDesc.8"),
     localization::Tr("table.gSpellDesc.9"),
     localization::Tr("table.gSpellDesc.10"),
     localization::Tr("table.gSpellDesc.11"),
     localization::Tr("table.gSpellDesc.12"),
     localization::Tr("table.gSpellDesc.13"),
     localization::Tr("table.gSpellDesc.14"),
     localization::Tr("table.gSpellDesc.15"),
     localization::Tr("table.gSpellDesc.16"),
     localization::Tr("table.gSpellDesc.17"),
     localization::Tr("table.gSpellDesc.18"),
     localization::Tr("table.gSpellDesc.19"),
     localization::Tr("table.gSpellDesc.20"),
     localization::Tr("table.gSpellDesc.21"),
     localization::Tr("table.gSpellDesc.22"),
     localization::Tr("table.gSpellDesc.23"),
     localization::Tr("table.gSpellDesc.24"),
     localization::Tr("table.gSpellDesc.25"),
     localization::Tr("table.gSpellDesc.26"),
     localization::Tr("table.gSpellDesc.27"),
     localization::Tr("table.gSpellDesc.28"),
     localization::Tr("table.gSpellDesc.29"),
     localization::Tr("table.gSpellDesc.30"),
     localization::Tr("table.gSpellDesc.31"),
     localization::Tr("table.gSpellDesc.32"),
     localization::Tr("table.gSpellDesc.33"),
     localization::Tr("table.gSpellDesc.34"),
     localization::Tr("table.gSpellDesc.35"),
     localization::Tr("table.gSpellDesc.36"),
     localization::Tr("table.gSpellDesc.37"),
     localization::Tr("table.gSpellDesc.38"),
     localization::Tr("table.gSpellDesc.39"),
     localization::Tr("table.gSpellDesc.40"),
     localization::Tr("table.gSpellDesc.41"),
     localization::Tr("table.gSpellDesc.42"),
     localization::Tr("table.gSpellDesc.43"),
     localization::Tr("table.gSpellDesc.44"),
     localization::Tr("table.gSpellDesc.45"),
     localization::Tr("table.gSpellDesc.46"),
     localization::Tr("table.gSpellDesc.47"),
     localization::Tr("table.gSpellDesc.48"),
     localization::Tr("table.gSpellDesc.49"),
     localization::Tr("table.gSpellDesc.50"),
     localization::Tr("table.gSpellDesc.51"),
     localization::Tr("table.gSpellDesc.52"),
     localization::Tr("table.gSpellDesc.53"),
     localization::Tr("table.gSpellDesc.54"),
     localization::Tr("table.gSpellDesc.55"),
     localization::Tr("table.gSpellDesc.56"),
     localization::Tr("table.gSpellDesc.57"),
     localization::Tr("table.gSpellDesc.58"),
     localization::Tr("table.gSpellDesc.59"),
     localization::Tr("table.gSpellDesc.60"),
     localization::Tr("table.gSpellDesc.61"),
     localization::Tr("table.gSpellDesc.62"),
     localization::Tr("table.gSpellDesc.63"),
     localization::Tr("table.gSpellDesc.64")
};
DATA(0x004fec04) H2_CONST char* gSpellNames[KB_SPELL_TEXT_COUNT] = {
     localization::Tr("table.gSpellNames.0"),
     localization::Tr("table.gSpellNames.1"),
     localization::Tr("table.gSpellNames.2"),
     localization::Tr("table.gSpellNames.3"),
     localization::Tr("table.gSpellNames.4"),
     localization::Tr("table.gSpellNames.5"),
     localization::Tr("table.gSpellNames.6"),
     localization::Tr("table.gSpellNames.7"),
     localization::Tr("table.gSpellNames.8"),
     localization::Tr("table.gSpellNames.9"),
     localization::Tr("table.gSpellNames.10"),
     localization::Tr("table.gSpellNames.11"),
     localization::Tr("table.gSpellNames.12"),
     localization::Tr("table.gSpellNames.13"),
     localization::Tr("table.gSpellNames.14"),
     localization::Tr("table.gSpellNames.15"),
     localization::Tr("table.gSpellNames.16"),
     localization::Tr("table.gSpellNames.17"),
     localization::Tr("table.gSpellNames.18"),
     localization::Tr("table.gSpellNames.19"),
     localization::Tr("table.gSpellNames.20"),
     localization::Tr("table.gSpellNames.21"),
     localization::Tr("table.gSpellNames.22"),
     localization::Tr("table.gSpellNames.23"),
     localization::Tr("table.gSpellNames.24"),
     localization::Tr("table.gSpellNames.25"),
     localization::Tr("table.gSpellNames.26"),
     localization::Tr("table.gSpellNames.27"),
     localization::Tr("table.gSpellNames.28"),
     localization::Tr("table.gSpellNames.29"),
     localization::Tr("table.gSpellNames.30"),
     localization::Tr("table.gSpellNames.31"),
     localization::Tr("table.gSpellNames.32"),
     localization::Tr("table.gSpellNames.33"),
     localization::Tr("table.gSpellNames.34"),
     localization::Tr("table.gSpellNames.35"),
     localization::Tr("table.gSpellNames.36"),
     localization::Tr("table.gSpellNames.37"),
     localization::Tr("table.gSpellNames.38"),
     localization::Tr("table.gSpellNames.39"),
     localization::Tr("table.gSpellNames.40"),
     localization::Tr("table.gSpellNames.41"),
     localization::Tr("table.gSpellNames.42"),
     localization::Tr("table.gSpellNames.43"),
     localization::Tr("table.gSpellNames.44"),
     localization::Tr("table.gSpellNames.45"),
     localization::Tr("table.gSpellNames.46"),
     localization::Tr("table.gSpellNames.47"),
     localization::Tr("table.gSpellNames.48"),
     localization::Tr("table.gSpellNames.49"),
     localization::Tr("table.gSpellNames.50"),
     localization::Tr("table.gSpellNames.51"),
     localization::Tr("table.gSpellNames.52"),
     localization::Tr("table.gSpellNames.53"),
     localization::Tr("table.gSpellNames.54"),
     localization::Tr("table.gSpellNames.55"),
     localization::Tr("table.gSpellNames.56"),
     localization::Tr("table.gSpellNames.57"),
     localization::Tr("table.gSpellNames.58"),
     localization::Tr("table.gSpellNames.59"),
     localization::Tr("table.gSpellNames.60"),
     localization::Tr("table.gSpellNames.61"),
     localization::Tr("table.gSpellNames.62"),
     localization::Tr("table.gSpellNames.63"),
     localization::Tr("table.gSpellNames.64")
};
DATA(0x004fed08) H2_CONST char* gSecondarySkillLevels[KB_SECONDARY_SKILL_LEVEL_TEXT_COUNT] =
    { localization::Tr("table.gSecondarySkillLevels.0"), localization::Tr("table.gSecondarySkillLevels.1"), localization::Tr("table.gSecondarySkillLevels.2")};
DATA(0x004fed14) H2_CONST char* gSecondarySkills[KB_SECONDARY_SKILL_TEXT_COUNT] = {
     localization::Tr("table.gSecondarySkills.0"),
     localization::Tr("table.gSecondarySkills.1"),
     localization::Tr("table.gSecondarySkills.2"),
     localization::Tr("table.gSecondarySkills.3"),
     localization::Tr("table.gSecondarySkills.4"),
     localization::Tr("table.gSecondarySkills.5"),
     localization::Tr("table.gSecondarySkills.6"),
     localization::Tr("table.gSecondarySkills.7"),
     localization::Tr("table.gSecondarySkills.8"),
     localization::Tr("table.gSecondarySkills.9"),
     localization::Tr("table.gSecondarySkills.10"),
     localization::Tr("table.gSecondarySkills.11"),
     localization::Tr("table.gSecondarySkills.12"),
     localization::Tr("table.gSecondarySkills.13")
};
DATA(0x004fed4c) H2_CONST char* gNeutralBuildingNames[KB_NEUTRAL_BUILDING_TEXT_COUNT] = {
     localization::Tr("table.gNeutralBuildingNames.0"),
     localization::Tr("table.gNeutralBuildingNames.1"),
     localization::Tr("table.gNeutralBuildingNames.2"),
     localization::Tr("table.gNeutralBuildingNames.3"),
     localization::Tr("table.gNeutralBuildingNames.4"),
     localization::Tr("table.gNeutralBuildingNames.5"),
     localization::Tr("table.gNeutralBuildingNames.6"),
     localization::Tr("table.gNeutralBuildingNames.7"),
     localization::Tr("table.gNeutralBuildingNames.8"),
     localization::Tr("table.gNeutralBuildingNames.9"),
     localization::Tr("table.gNeutralBuildingNames.10"),
    /*  */ "",
     localization::Tr("table.gNeutralBuildingNames.12"),
    /*  */ "",
     localization::Tr("table.gNeutralBuildingNames.14"),
     localization::Tr("table.gNeutralBuildingNames.15"),
    /*  */ "",
    /*  */ "",
    /*  */ ""
};
DATA(0x004fed98) H2_CONST char* gWellExtraNames[KB_WELL_EXTRA_NAME_COUNT] = {
     localization::Tr("table.gWellExtraNames.0"),
     localization::Tr("table.gWellExtraNames.1"),
     localization::Tr("table.gWellExtraNames.2"),
     localization::Tr("table.gWellExtraNames.3"),
     localization::Tr("table.gWellExtraNames.4"),
     localization::Tr("table.gWellExtraNames.5"),
     localization::Tr("table.gWellExtraNames.6")
};
DATA(0x004fedb4) H2_CONST char* gSpecialBuildingNames[KB_SPECIAL_BUILDING_NAME_COUNT] =
    { localization::Tr("table.gSpecialBuildingNames.0"), localization::Tr("table.gSpecialBuildingNames.1"), localization::Tr("table.gSpecialBuildingNames.2"), localization::Tr("table.gSpecialBuildingNames.3"), localization::Tr("table.gSpecialBuildingNames.4"), localization::Tr("table.gSpecialBuildingNames.5"), localization::Tr("table.gSpecialBuildingNames.6")};
DATA(0x004fedd0) H2_CONST char* gDwellingNames[IDX(FACTION_COUNT)][KB_DWELLING_TYPE_COUNT] = {
    { localization::Tr("table.gDwellingNames.0.0"),
      localization::Tr("table.gDwellingNames.0.1"),
      localization::Tr("table.gDwellingNames.0.2"),
      localization::Tr("table.gDwellingNames.0.3"),
      localization::Tr("table.gDwellingNames.0.4"),
      localization::Tr("table.gDwellingNames.0.5"),
      localization::Tr("table.gDwellingNames.0.6"),
      localization::Tr("table.gDwellingNames.0.7"),
      localization::Tr("table.gDwellingNames.0.8"),
      localization::Tr("table.gDwellingNames.0.9"),
      localization::Tr("table.gDwellingNames.0.10"),
     /*  */ ""},
    { localization::Tr("table.gDwellingNames.1.0"),
      localization::Tr("table.gDwellingNames.1.1"),
      localization::Tr("table.gDwellingNames.1.2"),
      localization::Tr("table.gDwellingNames.1.3"),
      localization::Tr("table.gDwellingNames.1.4"),
      localization::Tr("table.gDwellingNames.1.5"),
      localization::Tr("table.gDwellingNames.1.6"),
     /*  */ "",
      localization::Tr("table.gDwellingNames.1.8"),
      localization::Tr("table.gDwellingNames.1.9"),
     /*  */ "",
     /*  */ ""},
    { localization::Tr("table.gDwellingNames.2.0"),
      localization::Tr("table.gDwellingNames.2.1"),
      localization::Tr("table.gDwellingNames.2.2"),
      localization::Tr("table.gDwellingNames.2.3"),
      localization::Tr("table.gDwellingNames.2.4"),
      localization::Tr("table.gDwellingNames.2.5"),
      localization::Tr("table.gDwellingNames.2.6"),
      localization::Tr("table.gDwellingNames.2.7"),
      localization::Tr("table.gDwellingNames.2.8"),
     /*  */ "",
     /*  */ "",
     /*  */ ""},
    { localization::Tr("table.gDwellingNames.3.0"),
      localization::Tr("table.gDwellingNames.3.1"),
      localization::Tr("table.gDwellingNames.3.2"),
      localization::Tr("table.gDwellingNames.3.3"),
      localization::Tr("table.gDwellingNames.3.4"),
      localization::Tr("table.gDwellingNames.3.5"),
     /*  */ "",
     /*  */ "",
      localization::Tr("table.gDwellingNames.3.8"),
     /*  */ "",
      localization::Tr("table.gDwellingNames.3.10"),
      localization::Tr("table.gDwellingNames.3.11")},
    { localization::Tr("table.gDwellingNames.4.0"),
      localization::Tr("table.gDwellingNames.4.1"),
      localization::Tr("table.gDwellingNames.4.2"),
      localization::Tr("table.gDwellingNames.4.3"),
      localization::Tr("table.gDwellingNames.4.4"),
      localization::Tr("table.gDwellingNames.4.5"),
     /*  */ "",
      localization::Tr("table.gDwellingNames.4.7"),
     /*  */ "",
      localization::Tr("table.gDwellingNames.4.9"),
      localization::Tr("table.gDwellingNames.4.10"),
     /*  */ ""},
    { localization::Tr("table.gDwellingNames.5.0"),
      localization::Tr("table.gDwellingNames.5.1"),
      localization::Tr("table.gDwellingNames.5.2"),
      localization::Tr("table.gDwellingNames.5.3"),
      localization::Tr("table.gDwellingNames.5.4"),
      localization::Tr("table.gDwellingNames.5.5"),
      localization::Tr("table.gDwellingNames.5.6"),
      localization::Tr("table.gDwellingNames.5.7"),
      localization::Tr("table.gDwellingNames.5.8"),
      localization::Tr("table.gDwellingNames.5.9"),
     /*  */ "",
     /*  */ ""}
};
DATA(0x004feef0) H2_CONST char* cSecSkillDesc[IDX(HERO_SKILL_COUNT)][SECONDARY_SKILL_VALUE_LEVEL_COUNT] = {
    { localization::Tr("table.cSecSkillDesc.0.0"),
      localization::Tr("table.cSecSkillDesc.0.1"),
      localization::Tr("table.cSecSkillDesc.0.2")},
    { localization::Tr("table.cSecSkillDesc.1.0"),
      localization::Tr("table.cSecSkillDesc.1.1"),
      localization::Tr("table.cSecSkillDesc.1.2")},
    { localization::Tr("table.cSecSkillDesc.2.0"),
      localization::Tr("table.cSecSkillDesc.2.1"),
      localization::Tr("table.cSecSkillDesc.2.2")},
    { localization::Tr("table.cSecSkillDesc.3.0"),
      localization::Tr("table.cSecSkillDesc.3.1"),
      localization::Tr("table.cSecSkillDesc.3.2")},
    { localization::Tr("table.cSecSkillDesc.4.0"),
      localization::Tr("table.cSecSkillDesc.4.1"),
      localization::Tr("table.cSecSkillDesc.4.2")},
    { localization::Tr("table.cSecSkillDesc.5.0"),
      localization::Tr("table.cSecSkillDesc.5.1"),
      localization::Tr("table.cSecSkillDesc.5.2")},
    { localization::Tr("table.cSecSkillDesc.6.0"),
      localization::Tr("table.cSecSkillDesc.6.1"),
      localization::Tr("table.cSecSkillDesc.6.2")},
    { localization::Tr("table.cSecSkillDesc.7.0"),
      localization::Tr("table.cSecSkillDesc.7.1"),
      localization::Tr("table.cSecSkillDesc.7.2")},
    { localization::Tr("table.cSecSkillDesc.8.0"),
      localization::Tr("table.cSecSkillDesc.8.1"),
      localization::Tr("table.cSecSkillDesc.8.2")},
    { localization::Tr("table.cSecSkillDesc.9.0"),
      localization::Tr("table.cSecSkillDesc.9.1"),
      localization::Tr("table.cSecSkillDesc.9.2")},
    { localization::Tr("table.cSecSkillDesc.10.0"),
      localization::Tr("table.cSecSkillDesc.10.1"),
      localization::Tr("table.cSecSkillDesc.10.2")},
    { localization::Tr("table.cSecSkillDesc.11.0"),
      localization::Tr("table.cSecSkillDesc.11.1"),
      localization::Tr("table.cSecSkillDesc.11.2")},
    { localization::Tr("table.cSecSkillDesc.12.0"),
      localization::Tr("table.cSecSkillDesc.12.1"),
      localization::Tr("table.cSecSkillDesc.12.2")},
    { localization::Tr("table.cSecSkillDesc.13.0"),
      localization::Tr("table.cSecSkillDesc.13.1"),
      localization::Tr("table.cSecSkillDesc.13.2")}
};
DATA(0x004fef98) H2_CONST char* cBuildingInfoNeutral[KB_NEUTRAL_BUILDING_INFO_COUNT] = {
     localization::Tr("table.cBuildingInfoNeutral.0"),
     localization::Tr("table.cBuildingInfoNeutral.1"),
     localization::Tr("table.cBuildingInfoNeutral.2"),
     localization::Tr("table.cBuildingInfoNeutral.3"),
     localization::Tr("table.cBuildingInfoNeutral.4"),
     localization::Tr("table.cBuildingInfoNeutral.5"),
     localization::Tr("table.cBuildingInfoNeutral.6"),
     localization::Tr("table.cBuildingInfoNeutral.7"),
     localization::Tr("table.cBuildingInfoNeutral.8"),
     localization::Tr("table.cBuildingInfoNeutral.9"),
     localization::Tr("table.cBuildingInfoNeutral.10"),
    /*  */ "",
     localization::Tr("table.cBuildingInfoNeutral.12"),
    /*  */ "",
     localization::Tr("table.cBuildingInfoNeutral.14"),
     localization::Tr("table.cBuildingInfoNeutral.15"),
    /*  */ "",
    /*  */ "",
    /*  */ ""
};
DATA(0x004fefe4) H2_CONST char* gBuildingInfoSpecial[KB_SPECIAL_BUILDING_INFO_COUNT] = {
     localization::Tr("table.gBuildingInfoSpecial.0"),
     localization::Tr("table.gBuildingInfoSpecial.1"),
     localization::Tr("table.gBuildingInfoSpecial.2"),
     localization::Tr("table.gBuildingInfoSpecial.3"),
     localization::Tr("table.gBuildingInfoSpecial.4"),
     localization::Tr("table.gBuildingInfoSpecial.5")
};
DATA(0x004feffc) H2_CONST char* cDirections[KB_DIRECTION_TEXT_COUNT] = {
     localization::Tr("table.cDirections.0"),
     localization::Tr("table.cDirections.1"),
     localization::Tr("table.cDirections.2"),
     localization::Tr("table.cDirections.3"),
     localization::Tr("table.cDirections.4"),
     localization::Tr("table.cDirections.5"),
     localization::Tr("table.cDirections.6"),
     localization::Tr("table.cDirections.7"),
     localization::Tr("table.cDirections.8")
};
DATA(0x004ff020) H2_CONST char* cRumourTerrainDescriptions[KB_RUMOUR_TERRAIN_DESCRIPTION_COUNT] = {
     localization::Tr("table.cRumourTerrainDescriptions.0"),
     localization::Tr("table.cRumourTerrainDescriptions.1"),
     localization::Tr("table.cRumourTerrainDescriptions.2"),
     localization::Tr("table.cRumourTerrainDescriptions.3"),
     localization::Tr("table.cRumourTerrainDescriptions.4"),
     localization::Tr("table.cRumourTerrainDescriptions.5"),
     localization::Tr("table.cRumourTerrainDescriptions.6"),
     localization::Tr("table.cRumourTerrainDescriptions.7"),
     localization::Tr("table.cRumourTerrainDescriptions.8")
};
DATA(0x004ff044) H2_CONST char* gInterfaceTypeText[KB_INTERFACE_TYPE_TEXT_COUNT] = { localization::Tr("table.gInterfaceTypeText.0"), localization::Tr("table.gInterfaceTypeText.1"), localization::Tr("table.gInterfaceTypeText.2")};
DATA(0x004ff050) H2_CONST char* cBWMouseText[KB_BW_MOUSE_TEXT_COUNT] = { localization::Tr("table.cBWMouseText.0"), localization::Tr("table.cBWMouseText.1")};
DATA(0x004ff058) H2_CONST char* combatSpeedText[KB_COMBAT_SPEED_TEXT_COUNT] = { localization::Tr("table.combatSpeedText.0"), localization::Tr("table.combatSpeedText.1"), localization::Tr("table.combatSpeedText.2")};
DATA(0x004ff064) H2_CONST char* combatMiniInfoText[KB_COMBAT_MINI_INFO_TEXT_COUNT] = { localization::Tr("table.combatMiniInfoText.0"), localization::Tr("table.combatMiniInfoText.1"), localization::Tr("table.combatMiniInfoText.2")};
DATA(0x004ff070) H2_CONST char* gcCommandLineHelp[KB_COMMAND_LINE_HELP_COUNT] = {
    /* \n\n\n***Command Line Help***\n */ "\n\n\n***Command Line Help***\n",
    /* \n */ "\n",
     localization::Tr("system.command_line.disable_digital_sound"),
     localization::Tr("system.command_line.disable_midi"),
     localization::Tr("system.command_line.disable_music"),
     localization::Tr("system.command_line.skip_intro"),
    /* \n */ "\n",
    /* \n */ "\n",
     localization::Tr("system.command_line.example"),
    /* \n */ "\n",
    /* HEROES2D /R0 /I0\n */ "HEROES2D /R0 /I0\n",
    /* \n */ "\n",
     localization::Tr("system.command_line.dos_example"),
     localization::Tr("system.command_line.disabled_example")
};
DATA(0x004ff0a8) H2_CONST char* cOverviewText[KB_OVERVIEW_TEXT_COUNT] =
    { localization::Tr("table.cOverviewText.0"), localization::Tr("table.cOverviewText.1"), localization::Tr("table.cOverviewText.2"), localization::Tr("table.cOverviewText.3"), localization::Tr("table.cOverviewText.4"), localization::Tr("table.cOverviewText.5")};
DATA(0x004ff0c0) H2_CONST char* cWinComError[KB_WIN_COM_ERROR_TEXT_COUNT] = {
     localization::Tr("table.cWinComError.0"),
     localization::Tr("table.cWinComError.1"),
     localization::Tr("table.cWinComError.2"),
     localization::Tr("table.cWinComError.3"),
     localization::Tr("table.cWinComError.4"),
     localization::Tr("table.cWinComError.5")
};
DATA(0x004ff0d8) H2_CONST char* cMiniViewText[KB_MINI_VIEW_TEXT_COUNT] =
    { localization::Tr("table.cMiniViewText.0"), localization::Tr("table.cMiniViewText.1"), localization::Tr("table.cMiniViewText.2"), localization::Tr("table.cMiniViewText.3"), localization::Tr("table.cMiniViewText.4"), localization::Tr("table.cMiniViewText.5"), localization::Tr("table.cMiniViewText.6"), localization::Tr("table.cMiniViewText.7"), localization::Tr("table.cMiniViewText.8")};
DATA(0x004ff0fc) H2_CONST char* gFileRequestHelp[KB_FILE_REQUEST_HELP_COUNT] = {
     localization::Tr("table.gFileRequestHelp.0"),
     localization::Tr("table.gFileRequestHelp.1"),
     localization::Tr("table.gFileRequestHelp.2"),
     localization::Tr("table.gFileRequestHelp.3"),
     localization::Tr("table.gFileRequestHelp.4"),
     localization::Tr("table.gFileRequestHelp.5"),
     localization::Tr("table.gFileRequestHelp.6"),
     localization::Tr("table.gFileRequestHelp.7"),
     localization::Tr("table.gFileRequestHelp.8"),
     localization::Tr("table.gFileRequestHelp.9"),
     localization::Tr("table.gFileRequestHelp.10"),
     localization::Tr("table.gFileRequestHelp.11"),
     localization::Tr("table.gFileRequestHelp.12"),
     localization::Tr("table.gFileRequestHelp.13"),
     localization::Tr("table.gFileRequestHelp.14")
};
DATA(0x004ff138) H2_CONST char* cPersonality[KB_PERSONALITY_TEXT_COUNT] = { localization::Tr("table.cPersonality.0"), localization::Tr("table.cPersonality.1"), localization::Tr("table.cPersonality.2"), localization::Tr("table.cPersonality.3")};
DATA(0x004ff148) H2_CONST char* gArmySizeNames[KB_ARMY_SIZE_NAME_COUNT][KB_ARMY_SIZE_NAME_VARIANT_COUNT] = {
    { localization::Tr("table.gArmySizeNames.0.0"), localization::Tr("table.gArmySizeNames.0.1"), localization::Tr("table.gArmySizeNames.0.2")},
    { localization::Tr("table.gArmySizeNames.1.0"), localization::Tr("table.gArmySizeNames.1.1"), localization::Tr("table.gArmySizeNames.1.2")},
    { localization::Tr("table.gArmySizeNames.2.0"), localization::Tr("table.gArmySizeNames.2.1"), localization::Tr("table.gArmySizeNames.2.2")},
    { localization::Tr("table.gArmySizeNames.3.0"), localization::Tr("table.gArmySizeNames.3.1"), localization::Tr("table.gArmySizeNames.3.2")},
    { localization::Tr("table.gArmySizeNames.4.0"), localization::Tr("table.gArmySizeNames.4.1"), localization::Tr("table.gArmySizeNames.4.2")},
    { localization::Tr("table.gArmySizeNames.5.0"), localization::Tr("table.gArmySizeNames.5.1"), localization::Tr("table.gArmySizeNames.5.2")},
    { localization::Tr("table.gArmySizeNames.6.0"), localization::Tr("table.gArmySizeNames.6.1"), localization::Tr("table.gArmySizeNames.6.2")},
    { localization::Tr("table.gArmySizeNames.7.0"), localization::Tr("table.gArmySizeNames.7.1"), localization::Tr("table.gArmySizeNames.7.2")},
    { localization::Tr("table.gArmySizeNames.8.0"), localization::Tr("table.gArmySizeNames.8.1"), localization::Tr("table.gArmySizeNames.8.2")}
};
DATA(0x004ff1b4) H2_CONST char* cRandomTavernText[KB_RANDOM_TAVERN_TEXT_COUNT] = {
     localization::Tr("table.cRandomTavernText.0"),
     localization::Tr("table.cRandomTavernText.1"),
     localization::Tr("table.cRandomTavernText.2"),
     localization::Tr("table.cRandomTavernText.3"),
     localization::Tr("table.cRandomTavernText.4"),
     localization::Tr("table.cRandomTavernText.5"),
     localization::Tr("table.cRandomTavernText.6"),
     localization::Tr("table.cRandomTavernText.7")
};
DATA(0x004ff1d4) H2_CONST char* cRandomSignText[KB_RANDOM_SIGN_TEXT_COUNT] =
    { localization::Tr("table.cRandomSignText.0"), localization::Tr("table.cRandomSignText.1"), localization::Tr("table.cRandomSignText.2"), localization::Tr("table.cRandomSignText.3")};
DATA(0x004ff1e4) H2_CONST char* cCampaignAwards[KB_CAMPAIGN_AWARD_TEXT_COUNT] = {
     localization::Tr("table.cCampaignAwards.0"),
     localization::Tr("table.cCampaignAwards.1"),
     localization::Tr("table.cCampaignAwards.2"),
     localization::Tr("table.cCampaignAwards.3"),
     localization::Tr("table.cCampaignAwards.4"),
     localization::Tr("table.cCampaignAwards.5"),
     localization::Tr("table.cCampaignAwards.6"),
     localization::Tr("table.cCampaignAwards.7"),
     localization::Tr("table.cCampaignAwards.8"),
     localization::Tr("table.cCampaignAwards.9"),
     localization::Tr("table.cCampaignAwards.10"),
     localization::Tr("table.cCampaignAwards.11")
};
DATA(0x004ff214) H2_CONST char* cCampaignName[IDX(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT] = {
    { localization::Tr("table.cCampaignName.0.0"),
      localization::Tr("table.cCampaignName.0.1"),
      localization::Tr("table.cCampaignName.0.2"),
      localization::Tr("table.cCampaignName.0.3"),
      localization::Tr("table.cCampaignName.0.4"),
      localization::Tr("table.cCampaignName.0.5"),
      localization::Tr("table.cCampaignName.0.6"),
      localization::Tr("table.cCampaignName.0.7"),
      localization::Tr("table.cCampaignName.0.8"),
      localization::Tr("table.cCampaignName.0.9"),
     /*  */ "",
      localization::Tr("table.cCampaignName.0.11")},
    { localization::Tr("table.cCampaignName.1.0"),
      localization::Tr("table.cCampaignName.1.1"),
      localization::Tr("table.cCampaignName.1.2"),
      localization::Tr("table.cCampaignName.1.3"),
      localization::Tr("table.cCampaignName.1.4"),
      localization::Tr("table.cCampaignName.1.5"),
      localization::Tr("table.cCampaignName.1.6"),
      localization::Tr("table.cCampaignName.1.7"),
      localization::Tr("table.cCampaignName.1.8"),
      localization::Tr("table.cCampaignName.1.9"),
      localization::Tr("table.cCampaignName.1.10"),
      localization::Tr("table.cCampaignName.1.11")}
};
DATA(0x004ff274) H2_CONST char* cCampaignDescription[IDX(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT] = {
    { localization::Tr("table.cCampaignDescription.0.0"),
      localization::Tr("table.cCampaignDescription.0.1"),
      localization::Tr("table.cCampaignDescription.0.2"),
      localization::Tr("table.cCampaignDescription.0.3"),
      localization::Tr("table.cCampaignDescription.0.4"),
      localization::Tr("table.cCampaignDescription.0.5"),
      localization::Tr("table.cCampaignDescription.0.6"),
      localization::Tr("table.cCampaignDescription.0.7"),
      localization::Tr("table.cCampaignDescription.0.8"),
      localization::Tr("table.cCampaignDescription.0.9"),
     /*  */ "",
      localization::Tr("table.cCampaignDescription.0.11")},
    { localization::Tr("table.cCampaignDescription.1.0"),
      localization::Tr("table.cCampaignDescription.1.1"),
      localization::Tr("table.cCampaignDescription.1.2"),
      localization::Tr("table.cCampaignDescription.1.3"),
      localization::Tr("table.cCampaignDescription.1.4"),
      localization::Tr("table.cCampaignDescription.1.5"),
      localization::Tr("table.cCampaignDescription.1.6"),
      localization::Tr("table.cCampaignDescription.1.7"),
      localization::Tr("table.cCampaignDescription.1.8"),
      localization::Tr("table.cCampaignDescription.1.9"),
      localization::Tr("table.cCampaignDescription.1.10"),
      localization::Tr("table.cCampaignDescription.1.11")}
};
DATA(0x004ff2d4) H2_CONST char* cOutOfMemory =
     localization::Tr("system.memory.requirement");
DATA(0x004ff2d8) H2_CONST char* cSlowVideoLevelText[KB_SLOW_VIDEO_LEVEL_TEXT_COUNT] = { localization::Tr("table.cSlowVideoLevelText.0"), localization::Tr("table.cSlowVideoLevelText.1")};
DATA(0x004ff2e0) H2_CONST char* gSPanelHelp[KB_SETTINGS_PANEL_HELP_COUNT] = {
     localization::Tr("table.gSPanelHelp.0"),
     localization::Tr("table.gSPanelHelp.1"),
     localization::Tr("table.gSPanelHelp.2"),
     localization::Tr("table.gSPanelHelp.3"),
     localization::Tr("table.gSPanelHelp.4"),
     localization::Tr("table.gSPanelHelp.5"),
     localization::Tr("table.gSPanelHelp.6"),
     localization::Tr("table.gSPanelHelp.7"),
     localization::Tr("table.gSPanelHelp.8"),
     localization::Tr("table.gSPanelHelp.9")
};
DATA(0x004ff308) H2_CONST char* xBarrierColor[KB_BARRIER_COLOR_NAME_COUNT] =
    { localization::Tr("table.xBarrierColor.0"), localization::Tr("table.xBarrierColor.1"), localization::Tr("table.xBarrierColor.2"), localization::Tr("table.xBarrierColor.3"), localization::Tr("table.xBarrierColor.4"), localization::Tr("table.xBarrierColor.5"), localization::Tr("table.xBarrierColor.6"), localization::Tr("table.xBarrierColor.7")};
DATA(0x004ff328) H2_CONST char* xGenericSiteNames[KB_GENERIC_SITE_NAME_COUNT] = {
     localization::Tr("table.xGenericSiteNames.0"),
     localization::Tr("table.xGenericSiteNames.1"),
     localization::Tr("table.xGenericSiteNames.2"),
     localization::Tr("table.xGenericSiteNames.3"),
     localization::Tr("table.xGenericSiteNames.4"),
     localization::Tr("table.xGenericSiteNames.5"),
     localization::Tr("table.xGenericSiteNames.6")
};
DATA(0x004ff344) H2_CONST char* xRecruitmentSiteNames[KB_RECRUITMENT_SITE_NAME_COUNT] = {
     localization::Tr("table.xRecruitmentSiteNames.0"),
     localization::Tr("table.xRecruitmentSiteNames.1"),
     localization::Tr("table.xRecruitmentSiteNames.2"),
     localization::Tr("table.xRecruitmentSiteNames.3"),
     localization::Tr("table.xRecruitmentSiteNames.4")
};
DATA(0x004ff358) SWinSetup gWinSetup[KB_WIN_SETUP_COUNT] = {
    {0, 100, localization::Tr("table.gWinSetup.0")},
    {1, 100, localization::Tr("table.gWinSetup.1")},
    {1, 101, localization::Tr("table.gWinSetup.2")},
    {1, 102, localization::Tr("table.gWinSetup.3")},
    {1, 103, localization::Tr("table.gWinSetup.4")},
    {1, 104, localization::Tr("table.gWinSetup.5")},
    {1, 105, localization::Tr("table.gWinSetup.6")},
    {2, 100, localization::Tr("table.gWinSetup.7")},
    {2, 101, localization::Tr("table.gWinSetup.8")},
    {2, 102, localization::Tr("table.gWinSetup.9")},
    {2, 103, localization::Tr("table.gWinSetup.10")},
    {2, 104, localization::Tr("table.gWinSetup.11")},
    {2, 105, localization::Tr("table.gWinSetup.12")},
    {2, 106, localization::Tr("table.gWinSetup.13")},
    {2, 107, localization::Tr("table.gWinSetup.14")},
    {2, 108, localization::Tr("table.gWinSetup.15")},
    {6, 300, localization::Tr("table.gWinSetup.16")},
    {6, 301, localization::Tr("table.gWinSetup.17")},
    {6, 302, localization::Tr("table.gWinSetup.18")},
    {6, 303, localization::Tr("table.gWinSetup.19")},
    {7, 600, localization::Tr("table.gWinSetup.20")},
    {7, 57, localization::Tr("table.gWinSetup.21")},
    {7, 58, localization::Tr("table.gWinSetup.22")},
    {7, 59, localization::Tr("table.gWinSetup.23")},
    {7, 60, localization::Tr("table.gWinSetup.24")},
    {7, 61, localization::Tr("table.gWinSetup.25")},
    {7, 62, localization::Tr("table.gWinSetup.26")},
    {7, 84, localization::Tr("table.gWinSetup.27")},
    {9, 41, localization::Tr("table.gWinSetup.28")},
    {12, 0, localization::Tr("table.gWinSetup.29")},
    {12, 1, localization::Tr("table.gWinSetup.30")},
    {14, 800, localization::Tr("table.gWinSetup.31")},
    {14, 801, localization::Tr("table.gWinSetup.32")},
    {14, 802, localization::Tr("table.gWinSetup.33")},
    {14, 803, localization::Tr("table.gWinSetup.34")},
    {14, 804, localization::Tr("table.gWinSetup.35")},
    {14, 805, localization::Tr("table.gWinSetup.36")},
    {14, 604, localization::Tr("table.gWinSetup.37")},
    {14, 605, localization::Tr("table.gWinSetup.38")},
    {14, 606, localization::Tr("table.gWinSetup.39")},
    {14, 607, localization::Tr("table.gWinSetup.40")},
    {14, 608, localization::Tr("table.gWinSetup.41")},
    {14, 609, localization::Tr("table.gWinSetup.42")},
    {14, 610, localization::Tr("table.gWinSetup.43")},
    {14, 611, localization::Tr("table.gWinSetup.44")},
    {14, 612, localization::Tr("table.gWinSetup.45")},
    {14, 613, localization::Tr("table.gWinSetup.46")},
    {14, 620, localization::Tr("table.gWinSetup.47")},
    {14, 621, localization::Tr("table.gWinSetup.48")},
    {14, 622, localization::Tr("table.gWinSetup.49")},
    {14, 623, localization::Tr("table.gWinSetup.50")},
    {14, 0, localization::Tr("table.gWinSetup.51")},
    {17, 110, localization::Tr("table.gWinSetup.52")},
    {18, 600, localization::Tr("table.gWinSetup.53")},
    {18, 601, localization::Tr("table.gWinSetup.54")},
    {18, 602, localization::Tr("table.gWinSetup.55")},
    {18, 603, localization::Tr("table.gWinSetup.56")},
    {18, 604, localization::Tr("table.gWinSetup.57")},
    {19, 600, localization::Tr("table.gWinSetup.58")},
    {20, 600, localization::Tr("table.gWinSetup.59")},
    {21, 600, localization::Tr("table.gWinSetup.60")},
    {21, 601, localization::Tr("table.gWinSetup.61")},
    {21, 602, localization::Tr("table.gWinSetup.62")},
    {21, 603, localization::Tr("table.gWinSetup.63")},
    {22, 0, localization::Tr("table.gWinSetup.64")},
    {23, 600, localization::Tr("table.gWinSetup.65")},
    {23, 601, localization::Tr("table.gWinSetup.66")},
    {23, 602, localization::Tr("table.gWinSetup.67")},
    {23, 603, localization::Tr("table.gWinSetup.68")},
    {23, 604, localization::Tr("table.gWinSetup.69")},
    {23, 605, localization::Tr("table.gWinSetup.70")},
    {23, 606, localization::Tr("table.gWinSetup.71")},
    {23, 607, localization::Tr("table.gWinSetup.72")}
};
DATA(0x0052668c) b32 gbHeroWindShowing = false;
DATA(0x004ff558) b32 gbFullCombatScreenDrawn = true;
DATA(0x00526690) b32 gbLimitedCombatUpdatePalette = false;
DATA(0x00526694) b32 gbFirstTimeThrough = false;
DATA(0x00526698) b32 gbSkipIntro = false;
DATA(0x004ff55c) b32 gbDoMemCheck = true;
DATA(0x0052669c) b32 gbAllBlack = false;
DATA(0x005266a0) b32 gbInCombat = false;
DATA(0x005266a4) b32 gbDirectConnect = false;
DATA(0x004ff560) i32 giForceSwitchMusic = -1;
DATA(0x005266a8) b32 gbComputeExtent = false;
DATA(0x005266ac) b32 gbSaveBiggestExtent = false;
DATA(0x005266b0) b32 gbLimitToExtent = false;
DATA(0x004ff564) b32 gbCurrArmyDrawn = true;
DATA(0x005266b4) AdventureDisposeLevel gAdvDisposeLevel = ADV_DISPOSE_NONE;
DATA(0x005266b8) b32 gbRemoteOn = false;
DATA(0x005266bc) b32 gbGameInitialized = false;
DATA(0x004ff568) i32 giHighScoreRank = -1;
DATA(0x004ff56c) HighScoreType giHighScoreType = HIGH_SCORE_STANDARD;
DATA(0x005266c0) b32 gbShowHighScore = false;
DATA(0x005266c4) b32 gbLowMemory = false;
DATA(0x004ff570) i32 giHighMemBuffer = CHECK_MEMORY_INITIAL_AVAILABLE_KB;
DATA(0x005266c8) void* gLowPage = NULL;
DATA(0x005266cc) b32 gbInPollSound = false;
DATA(0x005266d0) H2_ENUM_STORAGE(CDRomSetupResult, i32) iCDRomErr = CD_ROM_READY;
DATA(0x005266d4) i32 bEarlySetupDone = 0;
DATA(0x005266d8) b32 bKBDone = false;
DATA(0x005266dc) struct _REDBOOK* hRedbookz = NULL;
DATA(0x005266e0) b32 bForceCheckTimeEvent = false;
DATA(0x004ff574) u16 IMHotSpots[KB_INIT_MENU_HOTSPOT_COUNT][IDX(INIT_MENU_HOTSPOT_FIELD_COUNT)] = {
    {481, 185, 83, 96},
    {194, 179, 82, 79},
    {412, 105, 75, 76},
    {303, 137, 75, 44},
    {0, 389, 86, 90}
};

DATA(0x004ff59c) i32 lastIMHoverID = -1;
DATA(0x005266e4) b32 bInCheckEndGame = false;
DATA(0x005266e8) b32 bInShutDown = false;
DATA(0x005266ec) b32 gbInMemError = false;
DATA(0x005266f0) i32 iShingleAnimFrame = 0;
DATA(0x005261b0) b32 gbHumanPlayer[IDX(GAME_PLAYER_COUNT)];
DATA(0x00525884) b32 gbHitEvent;
DATA(0x005260e0) i32 giMaxExtentX;
DATA(0x005260e4) i32 giMaxExtentY;
DATA(0x00526398) i32 giRandomClouds;
DATA(0x00526110) char cOverrideDigitalDriver[GLOBAL_DRIVER_NAME_SIZE];
DATA(0x00526140) i32 giBottomViewOverrideEndTime;
DATA(0x005265b0) b8 gArmyEffected[COMBAT_SIDE_COUNT][KB_ARMY_EFFECT_COUNT];
DATA(0x005258bc) H2_ENUM_STORAGE(ResourceType, i32) giBottomViewResource;
DATA(0x00524e04) b32 gbInCampaign;
DATA(0x00526130) i32 giResExtra1;
DATA(0x00526138) i32 giResExtra2;
DATA(0x00524de4) i8 puzzlePiecesRemoved[PUZZLE_PIECE_STORAGE_SIZE];
DATA(0x00526164) b32 giSeedingValid;
DATA(0x0052613c) i32 giLimitPlayer;
DATA(0x005265a8) i32 giShowClouds;
DATA(0x005263fc) b32 bDoColorCycle;
DATA(0x00526100) inputManager* gpInputManager;
DATA(0x00525a58) i32 iMaxMapExtra;
DATA(0x00525be4) palette* gPalette;
DATA(0x005258d0) resourceManager* gpResourceManager;
DATA(0x00524a34) char gcBotViewText[GLOBAL_BOTTOM_VIEW_TEXT_SIZE];
DATA(0x005258c4) b32 bSpecialHideCursor;
DATA(0x005258a0) searchArray* gpSearchArray;
DATA(0x00525bd8) i32 giResType1;
DATA(0x005260e8) b32 gbBlackoutPlayer;
DATA(0x00525be0) i32 giResType2;
DATA(0x00525bec) char cNetBoxLine[BOX_LINE_COUNT][NET_BOX_LINE_SIZE];
DATA(0x00526380) i32 gIndex;
DATA(0x005258c8) i32 giWeekTypeExtra;
DATA(0x005258d8) philAI* gpPhilAI;
DATA(0x00525898) i32 giTCPType;
DATA(0x00524e00) H2_ENUM_STORAGE(CombatEffectType, i32) gCurLoadedSpellEffect;
DATA(0x00524acc) class mouseManager* gpMouseManager;
DATA(0x0052610c) i32 giCurTempMobility;
DATA(0x00525be8) OverviewReturnAction giOverviewReturnAction;
DATA(0x005249e0) char cOverrideMIDIDriver[GLOBAL_DRIVER_NAME_SIZE];
DATA(0x00526150) icon* gSystemIcons;
DATA(0x00524e10) b8 gbCombatSurrender;
DATA(0x005249d8) heroWindow* pNormalDialogWindow;
DATA(0x00524dd4) i32 giTCPHostStatus;
DATA(0x005260ec) char gMapName[GLOBAL_SHORT_MAP_NAME_SIZE];
DATA(0x00526390) i32 giMinExtentX;
DATA(0x00526394) i32 giMinExtentY;
DATA(0x00526120) MultiplayerBaseType iMPBaseType;
DATA(0x00526578) b32 gbTCPFirstTime;
DATA(0x0052638c) i16* pwSizeOfMapExtra;
DATA(0x00524dec) i32 giHeroScreenSrcIndex;
DATA(0x00524e08) CalendarPeriodType giWeekType;
DATA(0x00524ad0) char gText[GLOBAL_TEXT_BUFFER_SIZE];
DATA(0x005258d4) b32 gbInNewGameSetup;
DATA(0x00525bdc) class palette* gpBufferPalette;
DATA(0x0052615c) H2_ENUM_STORAGE(CampaignSide, i32) gbCampaignSideChoice;
DATA(0x005265d8) char cNetBoxColor[BOX_LINE_COUNT];
DATA(0x00524dfc) i32 giMonthTypeExtra;
DATA(0x00525890) H2_ENUM_STORAGE(RemoteGameMode, i32) iMPExtendedType;
DATA(0x00525bd0) i8 gcColorToSetupPos[RADAR_OWNER_COLOR_COUNT];
DATA(0x005258a8) char gFullMapName[GLOBAL_MAP_NAME_SIZE];
DATA(0x005261c8) char gcTCPName[GLOBAL_TCP_TEXT_SIZE];
DATA(0x00525a50) i32 giShowIntro;
DATA(0x00526580) i32 glTimers[GLOBAL_TIMER_COUNT];
DATA(0x00526154) i32 giScore;
DATA(0x00526160) armyGroup* gpMonGroup;
DATA(0x005261e0) configStruct gConfig;
DATA(0x00525f7c) char gcRegAppPath[GLOBAL_AGGREGATE_PATH_SIZE];
DATA(0x00526404) u32l gTimeMark;
DATA(0x00524e0c) char* EXPANSION_AGGREGATE_NAME;
DATA(0x00524f98) char cPlayerNames[X_GLOBAL_PLAYER_COUNT][GLOBAL_PLAYER_NAME_SIZE];
DATA(0x00526124) game* gpGame;
DATA(0x00525bc3) b8 gbRetreatWin;
DATA(0x005260fc) DialogWaitType giWaitType;
DATA(0x005258a4) class icon* gCurLoadedSpellIcon;
DATA(0x005249f4) u8 bSaveMusicPosition[KB_MUSIC_TRACK_COUNT];
DATA(0x00525a10) H2_ENUM_STORAGE(BottomViewMode, i32) giBottomViewOverride;
DATA(0x00524f74) char gcTCPAddress[GLOBAL_TCP_TEXT_SIZE];
DATA(0x005249dc) u8 giSetupGameType;
DATA(0x00525a64) char gLastFilename[GLOBAL_LAST_FILENAME_SIZE];
DATA(0x005261ac) b32 giFullySeeded;
DATA(0x00524de0) icon* gBuyBuildIcons;
DATA(0x00524df0) i32 iCombatControlNetPos[COMBAT_SIDE_COUNT];
DATA(0x00524e14) char cExpAggPathName[GLOBAL_AGGREGATE_PATH_SIZE];
DATA(0x0052614c) b32 gbMoveShown;
DATA(0x0052612c) void** ppMapExtra;
DATA(0x0052639c) char gcBottomViewText[GLOBAL_BOTTOM_VIEW_MESSAGE_SIZE];
DATA(0x005258cc) i32 giThisNetPos;
DATA(0x005258dc) i8 gbSetupGamePosToRealGamePos[RADAR_OWNER_COLOR_COUNT];
DATA(0x00526418) char gcRegCDRomPath[GLOBAL_AGGREGATE_PATH_SIZE];
DATA(0x00525a54) class heroWindow* heroWin;
DATA(0x00526158) i32 giOverviewReturnActionExtra;
DATA(0x0052657c) H2_ENUM_STORAGE(CombatSide, i32) giCurGeneral;
DATA(0x005260dc) i32 giThisGamePos;
DATA(0x00524f90) i32 giNumHumanPlayers;
DATA(0x00526388) b32 gbIconClipOn;
DATA(0x00526408) b32 gbRemoteGameOpen;
DATA(0x00525bcc) combatManager* gpCombatManager;
DATA(0x00524dd8) i32 giTCPNumPlayers;
DATA(0x005263f8) executive* gpExec;
DATA(0x00524a30) void* hMainWindow;
DATA(0x0052640c) i32 giCurWindowsStyleFlags;
DATA(0x00524dd0) i32 gGameCommand;
DATA(0x00526410) CalendarPeriodType giMonthType;
DATA(0x00524f8c) char* DEFAULT_AGGREGATE_NAME;
DATA(0x00526400) i32 gCurSpellEffectFrame;
DATA(0x00526104) b8 gbThisNetHumanPlayer[RADAR_OWNER_COLOR_COUNT];
DATA(0x00525e1c) char cAggPathName[GLOBAL_AGGREGATE_PATH_SIZE];
DATA(0x00525a60) class highScoreManager* gpHighScoreManager;
DATA(0x005249f0) b32 gbFunctionComplete;
DATA(0x00526134) b32 gbIAmGreatest;
DATA(0x00525888) b32 gbTextEntryEscaped;
DATA(0x00526128) i32 giTotalHighMem;
DATA(0x00525bc4) i32 gMapX;
DATA(0x00525bc8) i32 gMapY;
DATA(0x005258e4) char gcWinText[GLOBAL_WINDOW_TEXT_SIZE];
DATA(0x00524f94) b32 bFreshSave;
DATA(0x005265ac) b32 bShowIt;
DATA(0x00524ddc) i32 gLowPageScreenSelector;
DATA(0x0052589c) class heroWindowManager* gpWindowManager;
DATA(0x00525894) i32 giCurWatchPlayer;
DATA(0x0052616c) char gcCommandLine[GLOBAL_COMMAND_LINE_SIZE];
DATA(0x00526384) i32 giBottomViewResourceQty;
DATA(0x00525a5c) soundManager* gpSoundManager;
DATA(0x0052588c) b32 gbThisNetGotAdventureControl;
DATA(0x00526414) i32 giMapChangeCtr;
DATA(0x00525018) SMapChange sMapChangeQueue[CURSOR_MAP_CHANGE_QUEUE_COUNT];
DATA(0x005258c0) b32 gbWaitForRemoteReceive;
DATA(0x00525a14) u8 bMusicIsLooping[KB_MUSIC_TRACK_COUNT];
DATA(0x00526168) townManager* gpTownManager;
DATA(0x00524df8) advManager* gpAdvManager;
DATA(0x00526144) i8 gbGamePosToNetPos[OLD_MAIN_MATCH_BUFFER_SIZE];
