#include <Ints.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/town.h>
#include <SOURCE/townManager.h>
#include <SOURCE/ARMY.h>
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

typedef enum CampaignChoiceValue {
    CHOICE_VALUE_NONE = -1
} CampaignChoiceValue;

typedef enum CampaignChoiceAmount {
    CHOICE_NO_AMOUNT = -1,
    CHOICE_BASIC_SKILL = 1,
    CHOICE_RESOURCE_BONUS = 20,
    CHOICE_GOLD_BONUS = 2000
} CampaignChoiceAmount;

typedef enum CheckEndGameConstants {
    END_GAME_NO_PLAYER = -1,
    END_GAME_GRACE_DAYS = 7,
    END_GAME_TEXT_BUFFER_SIZE = 100,
    END_GAME_GOLD_SCALE = 1000,
    END_GAME_ULTIMATE_ARTIFACT = 0,
    END_GAME_PLAYER_DIALOG_ICON = 9,
    END_GAME_REMOTE_DIALOG_TIME = 5000,
    END_GAME_CAMPAIGN_SAVE_NAME_SIZE = 20,
    END_GAME_SCENARIO_OFFSET = 1
} CheckEndGameConstants;

typedef enum CheckEndGameCampaignScenario {
    END_GAME_DWARF_SCENARIO = 3,
    END_GAME_SIDE_SCENARIO = 7,
    END_GAME_ROLAND_CAPTURE_SCENARIO = 9,
    END_GAME_FIRST_NO_SAVE_SCENARIO = 10,
    END_GAME_LAST_SCENARIO = 11
} CheckEndGameCampaignScenario;

enum {
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
};
typedef i32 MoraleInfoTextIndex;
enum {
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
};
typedef i32 LuckInfoTextIndex;
typedef enum MoraleLuckInfoConstant {
    MORALE_LUCK_DESCRIPTION_SIZE = 200
} MoraleLuckInfoConstant;

typedef enum NetBoxLocalConstant {
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
} NetBoxLocalConstant;

typedef enum PollSoundConstant {
    MOUSE_UPDATE_INTERVAL = 13,
    COMBAT_COLOR_CYCLE_INTERVAL = 110,
    DEFAULT_COLOR_CYCLE_INTERVAL = 200,
    NON_PALETTED_COLOR_CYCLE_DELAY = 300,
    SOUND_POLL_INTERVAL = 30,
    PALETTED_VIDEO_MODE_COLOR_DEPTH = 8
} PollSoundConstant;

typedef enum CongratsConstant {
    CONGRATS_TEXT_SIZE = 500,
    CONGRATS_RATING_LENGTH = 32,
    CONGRATS_SMACKER = 2,
    CONGRATS_DIFFICULTY_SCALE = 100
} CongratsConstant;

typedef enum CommandLineConstant {
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
} CommandLineConstant;

typedef enum InitMenuConstant {
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
} InitMenuConstant;

typedef enum RecruitHeroConstant {
    RECRUIT_HERO_VIEW_BUTTON = 2,
    RECRUIT_HERO_FADE_STEPS = 8
} RecruitHeroConstant;

typedef enum BuildingRuleConstant {
    BUILDING_INFO_BUFFER_SIZE = 400,
    MONSTER_RARE_RESOURCE_COST = 1,
    MONSTER_UPGRADED_RARE_RESOURCE_COST = 2,
    NECROMANCER_CASTLE_UPGRADE_BASE_RESOURCE_VALUE = 1000
} BuildingRuleConstant;

typedef enum HighScoreInputConstant {
    HIGH_SCORE_INPUT_NAME_SIZE = 20,
    HIGH_SCORE_LAST_SHIFT_SOURCE = HIGH_SCORE_ENTRY_COUNT - 2
} HighScoreInputConstant;

inline town* GetCastleRec(i32 i) {
    return &gpGame->m_castleRecs[i];
}

inline hero* GetHeroSlot(i32 i) {
    return &gpGame->m_heroRecs[i];
}

typedef enum NormalDialogDrawRange {
    NORMAL_DIALOG_FOREGROUND_WIDGET_LIMIT = 0x9000,
    NORMAL_DIALOG_BACKGROUND_WIDGET_LAST_ID = -256
} NormalDialogDrawRange;

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
        bDoColorCycle = 1;
        if (giGraphicsType == WINGRAPH_GRAPHICS_WING
            && giMainVideoModeColorDepth != PALETTED_VIDEO_MODE_COLOR_DEPTH) {
            glTimers[GLOBAL_COLOR_CYCLE_TIMER_SLOT] += NON_PALETTED_COLOR_CYCLE_DELAY;
            if (gbHeroMoving)
                bDoColorCycle = 0;
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

void ForcePollSound(void) {
    glTimers[GLOBAL_POLL_SOUND_TIMER_SLOT] = KBTickCount() - 1;
    PollSound();
}

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

void EarlyShutdown(char* caption, char* text) {
    MessageBoxA(hwndApp, text, caption, MB_ICONHAND);
    exit(0);
}

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
                "Нет доступа к CD-ROM. Без "
                "доступа к CD-ROM Герои 2 буду"
                "т поддерживать только "
                "сетевую игру в режиме "
                    "гостя.",
                NORMAL_DIALOG_INFO,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
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
                " Нет доступа к CD-ROM. Без "
                "доступа к CD-ROM Герои 2 буду"
                "т поддерживать только "
                "сетевую игру в режиме "
                "гостя. Если у вас есть "
                "диск, закройте эту про"
                "грамму и вставьте диск "
                "в привод и попробуйте "
                "запустить игру еще ра"
                    "з.",
                NORMAL_DIALOG_INFO,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
        gbNoCDRom = true;
    }
    if (iCDRomErr == CD_ROM_GAME_DIRECTORY_MISSING) {
        EarlyShutdown(
            "Ошибка загрузки",
            "Не могу переключиться в "
                "директорию Героев II.  Зап"
                "устите программу устан"
                "овки."
        );
        exit(0);
    }
    if (iCDRomErr == CD_ROM_DATA_FILES_MISSING) {
        EarlyShutdown(
            "Ошибка загрузки",
            "Не могу найти файлы дан"
                "ных Героев II.  Пожалуйст"
                "а, запустите программу "
                "установки."
        );
        exit(0);
    }
}

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

i32 oldmain(void) {
    i32 command_c;
    i32 quit;
    i32 mainScreenLoaded_h;
    i32 firstMainScreen_h;
    i32 savedUpdateFlags_l;
    i32 player_h;
    i32 unusedMainState_o;
    i32 unusedMenuState;
    i32 unusedPlayerState_f;
    i32 netPlayer_k;
    i32 gamePlayer_m;
    i32 result_i;
    i32 transmissionResult_d;
    char matchedNetPlayers_d[OLD_MAIN_MATCH_BUFFER_SIZE];
    char matchedGamePlayers_c[OLD_MAIN_MATCH_BUFFER_SIZE];
    OldMainNetBuffer netBuffer_f;

    if (bKBDone)
        return 0;
    bKBDone = 1;
    LogStr("OM1");
    LogStr("OM2");
    command_c = -1;
    if (gpExec->InitSystem())
        ShutDown("Ошибка инициализации!");
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
    gpMouseManager->SetColorMice(gConfig.gfx[(giCurExe)].colorMouseCursor);
    LogStr("OM4");
    SetupCDRom();
    LogStr("OM5");
    if (gpSoundManager->Open(-1))
        ShutDown("Не могу инициализирова"
            "ть звук.");
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
    quit = 0;
    mainScreenLoaded_h = 0;
    firstMainScreen_h = 1;

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
                firstMainScreen_h = 0;
            }
            gpMouseManager->SetPointer(
                "advmice.mse",
                0,
                MOUSE_AUTO_CURSOR_TYPE
            );
        }
        mainScreenLoaded_h = 1;
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
                gbWaitForRemoteReceive = 1;
            else
                gbWaitForRemoteReceive = 0;
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
                                mainScreenLoaded_h = 0;
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
                                    mainScreenLoaded_h = 0;
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
                    ShutDown("Не могу добавить мен"
                        "еджера!");
                gpExec->MainLoop();
                gpExec->RemoveManager(gpHighScoreManager);
                mainScreenLoaded_h = 0;
                goto main_menu;
            case OLD_MAIN_CREDITS:
                gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_FADE_SPEED, gPalette);
                PlaySmacker(OLD_MAIN_CREDITS_FIRST_VIDEO);
                PlaySmacker(OLD_MAIN_CREDITS_SECOND_VIDEO);
                PlaySmacker(OLD_MAIN_CREDITS_THIRD_VIDEO);
                mainScreenLoaded_h = 0;
                gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_LONG_FADE_SPEED, gPalette);
                goto main_menu;
            case OLD_MAIN_EXIT:
                quit = 1;
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
                gbUseRegularCompression = gbUseDiffCompression = 1;
                if (giHighMemBuffer < OLD_MAIN_REGULAR_COMPRESSION_MEMORY_LIMIT)
                    gbUseRegularCompression = 0;
                for (player_h = 0; player_h < giNumHumanPlayers; player_h++) {
                    if (!gsNetPlayerInfo[player_h].useRegularCompression)
                        gbUseRegularCompression = 0;
                    if (!gsNetPlayerInfo[player_h].useDiffCompression)
                        gbUseDiffCompression = 0;
                }
                netBuffer_f.setup.useRegularCompression = gbUseRegularCompression;
                netBuffer_f.setup.useDiffCompression = gbUseDiffCompression;
                for (player_h = 1; player_h < giNumHumanPlayers; player_h++) {
                    transmissionResult_d = TransmitRemoteData(
                        netBuffer_f.bytes,
                        player_h,
                        sizeof(OldMainNetSetup),
                        OLD_MAIN_NETWORK_PACKET,
                        1,
                        1,
                        REMOTE_MESSAGE_DEFAULT
                    );
                    if (!transmissionResult_d)
                        ShutDown(NULL);
                }
                for (player_h = 1; player_h < giNumHumanPlayers; player_h++) {
                    if (!gpGame->TransmitSaveGame(player_h, 0, 1))
                        ShutDown(NULL);
                }
                memset(gbThisNetHumanPlayer, 0, OLD_MAIN_PLAYER_COUNT);
                gbThisNetHumanPlayer[giThisGamePos] = 1;
                iLastDiffSendTo = -1;
                gpGame->SaveGame(gConfig.rmtRLName, 0, 0);
            }
            LogStr("DWM 4");
            if (gbRemoteOn && gbWaitForRemoteReceive) {
                LogStr("DWM 5");
                giWaitType = DIALOG_WAIT_OTHER_PLAYER;
                NormalDialog(
                    "Ожидаю получения игро"
                        "вых данных.",
                    OLD_MAIN_DIALOG_WAIT,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
                if (!gbFunctionComplete)
                    ShutDown(NULL);
                gpGame->LoadGame(gConfig.rmtRCName, 0, 1);
                sprintf(
                    gpGame->m_saveName,
                    "НОВАЯ ИГРА"
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
                        "%s игрок",
                        gColors[gpGame->m_players[player_h].m_color]
                    );
                    cPlayerNames[player_h][0] = CyrillicToUpper(cPlayerNames[player_h][0]);
                }
            }
            ComputeAdvNetControl();
            gbGameInitialized = true;
            mainScreenLoaded_h = 0;
            gpSoundManager->StopAllSamples(1);
            gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_FADE_SPEED, NULL);
            gMapX = 0;
            gMapY = 0;
            giCurWatchPlayer = giCurPlayer;
            while (!gbThisNetHumanPlayer[giCurWatchPlayer])
                giCurWatchPlayer = (giCurWatchPlayer + 1) % gpGame->m_playerCount;
            giCurWatchPlayerBit = static_cast<u8>(1 << giCurWatchPlayer);

            if (gbInCampaign && gpGame->m_campaignScenarioWon) {
                giEndSequence = 1;
                goto game_over;
            } else if (xIsPlayingExpansionCampaign && xCampaign.IsThisMapCompleted()) {
                giEndSequence = 1;
                goto game_over;
            } else {
                if (gpExec->AddManager(gpAdvManager, -1))
                    ShutDown("Не могу добавить мен"
                        "еджера!");
                if (command_c == OLD_MAIN_NEW_GAME) {
                    gpAdvManager->SetHeroContext(gpGame->m_players[0].NextHero(0), 0);
                }
                if (command_c == OLD_MAIN_NEW_GAME || bForceCheckTimeEvent) {
                    bForceCheckTimeEvent = 0;
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
            bShowIt = 1;
            gpMouseManager->SetPointer(
                "advmice.mse",
                0,
                MOUSE_AUTO_CURSOR_TYPE
            );
            sprintf(
                gcWinText,
                "Мои герои! Наши враги "
                "были разбиты, а их за"
                "мки преданы разорен"
                "ию. Великий поход ок"
                "ончен, и я предстаю п"
                "еред вами как всеми "
                "признанный Великий "
                "Король!\n\nМы достигл"
                    "и  победы за %d дней!",
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
                mainScreenLoaded_h = 1;
                gpSoundManager->PlayAmbientMusic(OLD_MAIN_MAIN_MUSIC);
            } else {
                i32 campaignResult = 0;
                if (gbInCampaign) {
                    campaignResult = gpGame->HandleCampaignWin();
                    if ((gpGame->m_campaignScenario + 1 == OLD_MAIN_ARCHIBALD_FINAL_SCENARIO_NUMBER
                         && gpGame->m_campaignScenarioCompleted[(gpGame->m_campaignType)]
                                                               [OLD_MAIN_ARCHIBALD_FINAL_SCENARIO])
                        || (gpGame->m_campaignScenario + 1 == OLD_MAIN_ROLAND_FINAL_SCENARIO_NUMBER
                            && gpGame->m_campaignType == CAMPAIGN_ROLAND
                            && gpGame
                                   ->m_campaignScenarioCompleted[(gpGame->m_campaignType)]
                                                                [OLD_MAIN_ROLAND_FINAL_SCENARIO])) {
                        gbShowHighScore = true;
                        ShowCongrats(HIGH_SCORE_CAMPAIGN);
                        AddScoreToHighScore(
                            gpGame->m_campaignScore,
                            gpGame->m_campaignScore,
                            0,
                            HIGH_SCORE_CAMPAIGN,
                            const_cast<char*>(
                                gpGame->m_campaignType == CAMPAIGN_ARCHIBALD
                                    ? "Арчибальд"
                                    : "Роланд"
                            )
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
                        bForceCheckTimeEvent = 1;
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
                        bForceCheckTimeEvent = 1;
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
                        mainScreenLoaded_h = 1;
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
                    ShutDown("Не могу добавить мен"
                        "еджера!");
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
                mainScreenLoaded_h = 1;
            }
        }

        if (gbRemoteOn)
            quit = 1;
    }

    ShutDown(NULL);
    return 0;
}

char toupper(char c) {
    if (static_cast<u8>(c) >= 'a' && static_cast<u8>(c) <= 'z')
        return c - ' ';
    if (static_cast<u8>(c) >= 0xE0 && static_cast<u8>(c) <= 0xFF)
        return c - ' ';
    if (static_cast<u8>(c) == 0xB8)
        return static_cast<char>(0xA8);
    return c;
}

i32 InterpretCommandLine(void) {
    i32 size;
    i32 i;
    i32 helpRequested;
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
    giScreenScroll = 1;
    giLimitPlayer = 0;
    gbBlackoutPlayer = true;
    helpRequested = 0;
    strcpy(gMapName, "Chaos.mp2");
    strcpy(
        gFullMapName,
        "Хаос"
    );

    size = strlen(gcCommandLine);
    for (i = 0; i < size; i++) {
        if (gcCommandLine[i] == ' ' && i + 1 < size
            && (gcCommandLine[i + 1] == '?' || gcCommandLine[i + 1] == 'h'
                || gcCommandLine[i + 1] == 'H')) {
            helpRequested = 1;
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
                                    i32 dstIndex = 0;
                                    i32 srcIndex = i + 3;
                                    while (dstIndex < LINE_TCP_TEXT_LENGTH
                                           && gcCommandLine[srcIndex]
                                           && gcCommandLine[srcIndex] != ' ') {
                                        gcTCPAddress[dstIndex] = gcCommandLine[srcIndex];
                                        srcIndex++;
                                        dstIndex++;
                                    }
                                    gcTCPAddress[dstIndex] = 0;
                                }
                                break;
                            }
                            case 'N': {
                                if (i + 3 < size) {
                                    i32 dstIndex = 0;
                                    i32 srcIndex = i + 3;
                                    while (dstIndex < LINE_TCP_TEXT_LENGTH
                                           && gcCommandLine[srcIndex]
                                           && gcCommandLine[srcIndex] != ' ') {
                                        gcTCPName[dstIndex] = gcCommandLine[srcIndex];
                                        srcIndex++;
                                        dstIndex++;
                                    }
                                    gcTCPName[dstIndex] = 0;
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
            ShutDown("Незавершенная TCP/IP ком"
                "андная строка");
        }
        giShowIntro = 0;
    }
    return 1;
}

MessageDispatchResult InitMenuHandler(struct tag_message& msg) {
    i32 handled = 0;
    i32 idx;
    i32 menu;
    i32 helpIndex;
    i32 hoverIndex;

    PollSound();
    if (msg.payload.widget.parameter & MENU_DISABLE_MASK) {
        if (msg.payload.widget.command == INIT_MENU_HOVER_COMMAND
            || msg.payload.widget.command == INIT_MENU_HELP_COMMAND) {
            helpIndex = -1;
            switch (msg.payload.widget.id) {
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
                NormalDialog(
                    gInitMenuHelp[helpIndex],
                    MENU_HELP_DIALOG,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
            }
        }
    } else {
        if (msg.type == INIT_MENU_KEY_PRESS) {
            switch (msg.payload.keyboard.keyCode) {
                case MENU_KEY_NEW:
                    gpWindowManager->m_dialogResult = MENU_NEW_GAME;
                    handled = 1;
                    break;
                case MENU_KEY_LOAD:
                    gpWindowManager->m_dialogResult = MENU_LOAD_GAME;
                    handled = 1;
                    break;
                case MENU_KEY_CREDITS:
                    gpWindowManager->m_dialogResult = MENU_CREDITS;
                    handled = 1;
                    break;
                case MENU_KEY_HIGH_SCORES:
                    gpWindowManager->m_dialogResult = MENU_HIGH_SCORES;
                    handled = 1;
                    break;
                case MENU_KEY_EXIT:
                    gpWindowManager->m_dialogResult = MENU_EXIT;
                    handled = 1;
                    break;
            }
        } else if (msg.type == INIT_MENU_MESSAGE) {
            if (msg.payload.widget.id < MENU_FIRST_COMMAND
                || msg.payload.widget.id > MENU_LAST_ACTION) {
                return MESSAGE_DISPATCH_CONTINUE;
            }
            switch (msg.payload.widget.command) {
                case INIT_MENU_HOVER_COMMAND:
                    if (msg.payload.widget.id == MENU_MOVIE)
                        break;
                    menu = msg.payload.widget.id - MENU_FIRST_COMMAND;
                    idx = menu + MENU_WIDGET_OFFSET;
                    msg.type = INIT_MENU_MESSAGE;
                    msg.payload.widget.id = idx;
                    msg.payload.widget.command = INIT_MENU_SET_WIDGET_COMMAND;
                    msg.payload.widget.data.value = menu * MENU_FRAME_STRIDE + MENU_HOVER_FRAME;
                    gpInitWin->BroadcastMessage(msg);
                    gpInitWin->DrawWindow(0, idx, idx);
                    gpWindowManager->UpdateScreenRegion(
                        IMHotSpots[menu][(INIT_MENU_HOTSPOT_X)],
                        IMHotSpots[menu][(INIT_MENU_HOTSPOT_Y)],
                        IMHotSpots[menu][(INIT_MENU_HOTSPOT_WIDTH)],
                        IMHotSpots[menu][(INIT_MENU_HOTSPOT_HEIGHT)]
                    );
                    break;
                case INIT_MENU_CLICK_COMMAND:
                    if (msg.payload.widget.id == MENU_MOVIE) {
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
                        gpWindowManager->m_dialogResult = msg.payload.widget.id;
                        for (idx = MENU_FIRST_WIDGET; idx <= MENU_LAST_WIDGET; idx++) {
                            msg.type = INIT_MENU_MESSAGE;
                            msg.payload.widget.id = idx;
                            msg.payload.widget.command = INIT_MENU_SET_WIDGET_COMMAND;
                            msg.payload.widget.data.value =
                                (idx - MENU_WIDGET_OFFSET) * MENU_FRAME_STRIDE;
                            gpInitWin->BroadcastMessage(msg);
                        }
                        gpInitWin->DrawWindow(0, MENU_FIRST_WIDGET, MENU_LAST_WIDGET);
                        gpWindowManager->UpdateScreenRegion(
                            MENU_REDRAW_LEFT,
                            MENU_REDRAW_TOP,
                            MENU_REDRAW_WIDTH,
                            MENU_REDRAW_HEIGHT
                        );
                        handled = 1;
                    }
                    break;
            }
        } else if (msg.type == INIT_MENU_MOUSE_MOVE) {
            hoverIndex = -1;
            for (idx = 0; idx < MENU_HOTSPOT_COUNT; idx++) {
                if (msg.payload.mouse.screenX >= IMHotSpots[idx][(INIT_MENU_HOTSPOT_X)]
                    && msg.payload.mouse.screenY >= IMHotSpots[idx][(INIT_MENU_HOTSPOT_Y)]
                    && msg.payload.mouse.screenX
                           < IMHotSpots[idx][(INIT_MENU_HOTSPOT_X)]
                                 + IMHotSpots[idx][(INIT_MENU_HOTSPOT_WIDTH)]
                    && msg.payload.mouse.screenY
                           < IMHotSpots[idx][(INIT_MENU_HOTSPOT_Y)]
                                 + IMHotSpots[idx][(INIT_MENU_HOTSPOT_HEIGHT)]) {
                    hoverIndex = idx;
                }
            }
            if (hoverIndex != lastIMHoverID) {
                if (lastIMHoverID != -1) {
                    msg.type = INIT_MENU_MESSAGE;
                    msg.payload.widget.id = lastIMHoverID + MENU_WIDGET_OFFSET;
                    msg.payload.widget.command = INIT_MENU_SET_WIDGET_COMMAND;
                    msg.payload.widget.data.value =
                        lastIMHoverID * MENU_FRAME_STRIDE + MENU_IDLE_FRAME;
                    gpInitWin->BroadcastMessage(msg);
                    gpInitWin->DrawWindow(
                        0,
                        lastIMHoverID + MENU_WIDGET_OFFSET,
                        lastIMHoverID + MENU_WIDGET_OFFSET
                    );
                    gpWindowManager->UpdateScreenRegion(
                        IMHotSpots[lastIMHoverID][(INIT_MENU_HOTSPOT_X)],
                        IMHotSpots[lastIMHoverID][(INIT_MENU_HOTSPOT_Y)],
                        IMHotSpots[lastIMHoverID][(INIT_MENU_HOTSPOT_WIDTH)],
                        IMHotSpots[lastIMHoverID][(INIT_MENU_HOTSPOT_HEIGHT)]
                    );
                }
                if (hoverIndex != -1) {
                    msg.type = INIT_MENU_MESSAGE;
                    msg.payload.widget.id = hoverIndex + MENU_WIDGET_OFFSET;
                    msg.payload.widget.command = INIT_MENU_SET_WIDGET_COMMAND;
                    msg.payload.widget.data.value =
                        hoverIndex * MENU_FRAME_STRIDE + MENU_ACTIVE_FRAME;
                    gpInitWin->BroadcastMessage(msg);
                    gpInitWin->DrawWindow(
                        0,
                        hoverIndex + MENU_WIDGET_OFFSET,
                        hoverIndex + MENU_WIDGET_OFFSET
                    );
                    gpWindowManager->UpdateScreenRegion(
                        IMHotSpots[hoverIndex][(INIT_MENU_HOTSPOT_X)],
                        IMHotSpots[hoverIndex][(INIT_MENU_HOTSPOT_Y)],
                        IMHotSpots[hoverIndex][(INIT_MENU_HOTSPOT_WIDTH)],
                        IMHotSpots[hoverIndex][(INIT_MENU_HOTSPOT_HEIGHT)]
                    );
                }
                lastIMHoverID = hoverIndex;
            }
        }
    }

    if (handled || giMenuCommand != -1) {
        msg.type = INIT_MENU_MESSAGE;
        msg.payload.widget.id = MENU_CLOSE_COMMAND;
        msg.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
        return MESSAGE_DISPATCH_FORWARD;
    }
    CheckShingleUpdate();
    return MESSAGE_DISPATCH_CONSUME;
}

MessageDispatchResult NullHandler(struct tag_message& msg) {
    return MESSAGE_DISPATCH_CONSUME;
}

MessageDispatchResult RecruitHeroHandler(tag_message& msg) {
    i16 unusedLocal0L = 2, unusedLocal1H = 3, unusedLocal2D = 8, unusedLocal3A = 9;
    i32 shouldClose = 0;
    i32 unusedResult;
    if (msg.type == MESSAGE_WIDGET) {
        switch (msg.payload.widget.command) {
            case WIDGET_COMMAND_SELECT:
                switch (msg.payload.widget.id) {
                    case RECRUIT_HERO_VIEW_BUTTON:
                        HeroView(static_cast<u8>(gpTownManager->m_recruitHero->m_id), true, false);
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
                switch (msg.payload.widget.id) {
                    case EVENT_WINDOW_SECOND_BUTTON:
                        gpTownManager->m_recruitState = -1;
                        shouldClose = 1;
                        break;
                    case EVENT_WINDOW_THIRD_BUTTON:
                        gpTownManager->m_recruitState = 0;
                        gpWindowManager->m_dialogResult = msg.payload.widget.id;
                        shouldClose = 1;
                        break;
                }
                break;
            default:
                break;
        }
    }
    if (shouldClose == 1) {
        msg.payload.widget.id = EVENT_WINDOW_CLOSE_COMMAND;
        msg.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

char* GetBuildingInfo(FactionType race, BuildingSlotType building, i32 mode) {
    char buf[BUILDING_INFO_BUFFER_SIZE];
    if (race == FACTION_NECROMANCER && building == BUILDING_SLOT_NECROMANCER_SHRINE) {
        sprintf(buf, xNecromancerShrineDesc);
    } else if (building == BUILDING_SLOT_WELL_EXTRA) {
        sprintf(
            buf,
            "%s увеличивает прирост %s "
                "на 8 в неделю."  ,
            GetBuildingName(race, building),
            gArmyNamesPlural[(gDwellingType[(race)][0])]
        );
    } else if (building == BUILDING_SLOT_SPECIAL) {
        sprintf(buf, gBuildingInfoSpecial[(race)]);
    } else if (building < BUILDING_SLOT_DWELLING_FIRST) {
        sprintf(buf, cBuildingInfoNeutral[(building)]);
    } else {
        sprintf(
            gText,
            "В постройке '%s' можно "
                "купить %s."  ,
            GetBuildingName(race, building),
            gArmyNamesPlural
                [(gDwellingType[(race)][(building) - (BUILDING_SLOT_DWELLING_FIRST)])]
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

char* GetBuildingName(FactionType race, BuildingSlotType building) {
    if (race == FACTION_NECROMANCER && building == BUILDING_SLOT_NECROMANCER_SHRINE)
        return xNecromancerShrine;
    if (building == BUILDING_SLOT_WELL_EXTRA)
        return gWellExtraNames[(race)];
    else if (building == BUILDING_SLOT_SPECIAL)
        return gSpecialBuildingNames[(race)];
    else if (building < BUILDING_SLOT_DWELLING_FIRST)
        return gNeutralBuildingNames[(building)];
    else
        return gDwellingNames[(race)][(building) - (BUILDING_SLOT_DWELLING_FIRST)];
}

void GetBuildingCost(FactionType race, BuildingSlotType building, i32* const dest, i32 mageLevel) {
    i32 level;
    if (building == BUILDING_SLOT_NECROMANCER_SHRINE && race == FACTION_NECROMANCER) {
        memcpy(dest, xShrineBuildingCost, KB_BUILDING_RESOURCE_COUNT * sizeof(i32));
    } else if (building >= BUILDING_SLOT_DWELLING_FIRST
               && building <= BUILDING_SLOT_DWELLING_LAST) {
        memcpy(
            dest,
            gDwellingCosts[(race)][(building) - (BUILDING_SLOT_DWELLING_FIRST)],
            KB_BUILDING_RESOURCE_COUNT * sizeof(i32)
        );
    } else if (building == BUILDING_SLOT_MAGE_GUILD) {
        level = mageLevel + 1;
        if (level > KB_MAGE_GUILD_MAX_LEVEL)
            level = KB_MAGE_GUILD_MAX_LEVEL;
        memcpy(dest, gMageBuildingCosts[mageLevel + 1], KB_BUILDING_RESOURCE_COUNT * sizeof(i32));
    } else if (building == BUILDING_SLOT_SPECIAL) {
        memcpy(dest, gSpecialBuildingCosts[(race)], KB_BUILDING_RESOURCE_COUNT * sizeof(i32));
    } else {
        if (building >= BUILDING_SLOT_DISABLED_SECOND)
            return;
        memcpy(
            dest,
            gNeutralBuildingCosts[(building)],
            KB_BUILDING_RESOURCE_COUNT * sizeof(i32)
        );
    }
}

char* GetMonsterName(CreatureType monster) {
    return gArmyNames[(monster)];
}

char* GetMonsterPluralName(CreatureType monster) {
    return gArmyNamesPlural[(monster)];
}

void GetMonsterCost(CreatureType monster, i32* const cost) {
    i32 idx;
    for (idx = 0; idx < KB_BUILDING_RESOURCE_COUNT; idx++)
        cost[idx] = 0;
    cost[(RES_GOLD)] = gMonsterDatabase[(monster)].cost;
    switch (monster) {
        case CREATURE_GENIE:
            cost[(RES_GEMS)] = MONSTER_RARE_RESOURCE_COST;
            break;
        case CREATURE_PHOENIX:
            cost[(RES_MERCURY)] = MONSTER_RARE_RESOURCE_COST;
            break;
        case CREATURE_CYCLOPS:
            cost[(RES_CRYSTAL)] = MONSTER_RARE_RESOURCE_COST;
            break;
        case CREATURE_GREEN_DRAGON:
        case CREATURE_RED_DRAGON:
            cost[(RES_SULFUR)] = MONSTER_RARE_RESOURCE_COST;
            break;
        case CREATURE_BLACK_DRAGON:
            cost[(RES_SULFUR)] = MONSTER_UPGRADED_RARE_RESOURCE_COST;
            break;
        case CREATURE_GIANT:
            cost[(RES_GEMS)] = MONSTER_RARE_RESOURCE_COST;
            break;
        case CREATURE_TITAN:
            cost[(RES_GEMS)] = MONSTER_UPGRADED_RARE_RESOURCE_COST;
            break;
    }
}

i32 CanBuild(town* t, BuildingSlotType building) {
    i32 reqBits;
    i32 curMask;
    if (H2BitTest(gpGame->m_knownTowns, t->m_id))
        return 0;
    if (building != BUILDING_SLOT_CASTLE && !(t->m_buildings & (TOWN_BUILDING_CASTLE)))
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
         && (t->m_buildings & (KB_DWELLING_UPGRADE_FIRST_FLAG)))
        || (building == BUILDING_SLOT_DWELLING_THIRD
            && (t->m_buildings & (KB_DWELLING_UPGRADE_SECOND_FLAG)))
        || (building == BUILDING_SLOT_DWELLING_FOURTH
            && (t->m_buildings & (KB_DWELLING_UPGRADE_THIRD_FLAG)))
        || (building == BUILDING_SLOT_DWELLING_FIFTH
            && (t->m_buildings & (KB_DWELLING_UPGRADE_FOURTH_FLAG)))
        || (building == BUILDING_SLOT_DWELLING_SIXTH
            && ((t->m_buildings & (KB_DWELLING_UPGRADE_FIFTH_FLAG))
                || (t->m_buildings & (KB_DWELLING_UPGRADE_SIXTH_FLAG))))
        || (building == BUILDING_SLOT_UPGRADE_LAST
            && (t->m_buildings & (KB_DWELLING_UPGRADE_SIXTH_FLAG))))
        return 0;
    reqBits = gHierarchyMask[(t->m_type)][(building) - (BUILDING_SLOT_DWELLING_FIRST)];
    curMask = t->m_buildings;
    if (curMask & (KB_DWELLING_UPGRADE_FIRST_FLAG))
        curMask |= (KB_DWELLING_FIRST_FLAG);
    if (curMask & (KB_DWELLING_UPGRADE_SECOND_FLAG))
        curMask |= (KB_DWELLING_SECOND_FLAG);
    if (curMask & (KB_DWELLING_UPGRADE_THIRD_FLAG))
        curMask |= (KB_DWELLING_THIRD_FLAG);
    if (curMask & (KB_DWELLING_UPGRADE_FOURTH_FLAG))
        curMask |= (KB_DWELLING_FOURTH_FLAG);
    if (curMask & (KB_DWELLING_UPGRADE_SIXTH_FLAG))
        curMask |= (KB_DWELLING_UPGRADE_FIFTH_FLAG);
    if (curMask & (KB_DWELLING_UPGRADE_FIFTH_FLAG))
        curMask |= (KB_DWELLING_FIFTH_FLAG);
    if ((reqBits & curMask) == reqBits) {
        if (t->m_type == FACTION_NECROMANCER
            && building == BUILDING_SLOT_NECROMANCER_MAGE_PREREQUISITE && t->m_buildState <= 1)
            return 0;
        return 1;
    }
    return 0;
}

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

i32 GetBuildingBaseResourceValue(FactionType race, BuildingSlotType building, i32 level) {
    if (race == FACTION_NECROMANCER && building == BUILDING_SLOT_UPGRADE_CASTLE)
        return NECROMANCER_CASTLE_UPGRADE_BASE_RESOURCE_VALUE;
    if (building < BUILDING_SLOT_DWELLING_FIRST || building > BUILDING_SLOT_DWELLING_LAST) {
        if (building > BUILDING_SLOT_NEUTRAL_LAST)
            return 0;
        else if (building == BUILDING_SLOT_MAGE_GUILD)
            return gMageBaseResourceValues[level];
        else if (building == BUILDING_SLOT_SPECIAL)
            return gSpecialBuildingBaseResourceValues[(race)];
        else
            return gNeutralBaseResourceValues[(building)];
    } else {
        return gDwellingBaseResourceValues[(race)]
                                          [(building) - (BUILDING_SLOT_DWELLING_FIRST)];
    }
}

MessageDispatchResult WaitHandler(tag_message& msg) {
    i32 result = 0;
    gbFunctionComplete = true;
    PollSound();
    if (msg.type == MESSAGE_WIDGET) {
        switch (msg.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                switch (msg.payload.widget.id) {
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
        msg.type = MESSAGE_WIDGET;
        msg.payload.widget.id = EVENT_WINDOW_CLOSE_COMMAND;
        msg.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

MessageDispatchResult EventWindowHandler(struct tag_message& msg) {
    i32 resType;
    i32 resExtra;

    if (!gpSoundManager->MusicPlaying() && gpAdvManager->m_active == true)
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[(gpAdvManager->m_currentTerrain)]
        );
    if (giDialogTimeout != 0 && KBTickCount() > giDialogTimeout) {
        msg.type = MESSAGE_WIDGET;
        gpWindowManager->m_dialogResult = msg.payload.widget.id;
        msg.payload.widget.id = EVENT_WINDOW_CLOSE_COMMAND;
        msg.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
        giDialogTimeout = 0;
        return MESSAGE_DISPATCH_FORWARD;
    }
    if (msg.type == MESSAGE_WIDGET) {
        switch (msg.payload.widget.command) {
            case WIDGET_COMMAND_SELECT:
            case WIDGET_COMMAND_ALTERNATE_SELECT:
                resType = NORMAL_DIALOG_NO_RESOURCE;
                resExtra = NORMAL_DIALOG_NO_VALUE;
                if (msg.payload.widget.parameter & EVENT_WINDOW_RESOURCE_FLAG) {
                    switch (msg.payload.widget.id) {
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
                            NormalDialog(
                                cLuckInfo[(LUCK_INFO_GOOD)],
                                NORMAL_DIALOG_QUICK_VIEW,
                                -1,
                                -1,
                                -1,
                                0,
                                -1,
                                0,
                                -1,
                                0
                            );
                            break;
                        case EVENT_WINDOW_BAD_LUCK:
                            NormalDialog(
                                cLuckInfo[(LUCK_INFO_BAD)],
                                NORMAL_DIALOG_QUICK_VIEW,
                                -1,
                                -1,
                                -1,
                                0,
                                -1,
                                0,
                                -1,
                                0
                            );
                            break;
                        case EVENT_WINDOW_MORALE:
                            NormalDialog(
                                cMoraleInfo[(MORALE_INFO_GOOD)],
                                NORMAL_DIALOG_QUICK_VIEW,
                                -1,
                                -1,
                                -1,
                                0,
                                -1,
                                0,
                                -1,
                                0
                            );
                            break;
                        case EVENT_WINDOW_BAD_MORALE:
                            NormalDialog(
                                cMoraleInfo[(MORALE_INFO_BAD)],
                                NORMAL_DIALOG_QUICK_VIEW,
                                -1,
                                -1,
                                -1,
                                0,
                                -1,
                                0,
                                -1,
                                0
                            );
                            break;
                        case EVENT_WINDOW_EXPERIENCE:
                            NormalDialog(
                                "{Опыт}\n\nОпыт поз"
                                "воляет повыша"
                                "ть вашим героя"
                                "м свои уровни, у"
                                "величивать пе"
                                "рвичные и втор"
                                    "ичные навыки.",
                                NORMAL_DIALOG_QUICK_VIEW,
                                -1,
                                -1,
                                -1,
                                0,
                                -1,
                                0,
                                -1,
                                0
                            );
                            break;
                        case NORMAL_DIALOG_ARTIFACT:
                            if (resExtra == (ARTIFACT_SPELL_SCROLL)) {
                                sprintf(gText, gArtifactDesc[resExtra], gSpellNames[xTheSpell]);
                                NormalDialog(
                                    gText,
                                    NORMAL_DIALOG_QUICK_VIEW,
                                    -1,
                                    -1,
                                    -1,
                                    0,
                                    -1,
                                    0,
                                    -1,
                                    0
                                );
                            } else {
                                NormalDialog(
                                    gArtifactDesc[resExtra],
                                    NORMAL_DIALOG_QUICK_VIEW,
                                    -1,
                                    -1,
                                    -1,
                                    0,
                                    -1,
                                    0,
                                    -1,
                                    0
                                );
                            }
                            break;
                        case NORMAL_DIALOG_SPELL:
                            NormalDialog(
                                gSpellDesc[resExtra],
                                NORMAL_DIALOG_QUICK_VIEW,
                                -1,
                                -1,
                                -1,
                                0,
                                -1,
                                0,
                                -1,
                                0
                            );
                            break;
                        case NORMAL_DIALOG_SECONDARY_SKILL:
                            NormalDialog(
                                cSecSkillDesc[resExtra / SECONDARY_SKILL_VALUE_LEVEL_COUNT]
                                             [resExtra % SECONDARY_SKILL_VALUE_LEVEL_COUNT],
                                NORMAL_DIALOG_QUICK_VIEW,
                                -1,
                                -1,
                                -1,
                                0,
                                -1,
                                0,
                                -1,
                                0
                            );
                            break;
                        case NORMAL_DIALOG_PRIMARY_SKILL:
                            NormalDialog(
                                gStatDesc[resExtra],
                                NORMAL_DIALOG_QUICK_VIEW,
                                -1,
                                -1,
                                -1,
                                0,
                                -1,
                                0,
                                -1,
                                0
                            );
                            break;
                        case NORMAL_DIALOG_RESOURCE_WOOD:
                        case NORMAL_DIALOG_RESOURCE_MERCURY:
                        case NORMAL_DIALOG_RESOURCE_ORE:
                        case NORMAL_DIALOG_RESOURCE_SULFUR:
                        case NORMAL_DIALOG_RESOURCE_CRYSTAL:
                        case NORMAL_DIALOG_RESOURCE_GEMS:
                        case NORMAL_DIALOG_RESOURCE_GOLD:
                            NormalDialog(
                                "{Ресурсы}\n\nВ Геро"
                                "ях II есть 7 типов "
                                "ресурсов, испо"
                                "льзуемых для в"
                                "озведения пос"
                                "троек, улучшен"
                                "ий замков, поку"
                                "пки воинов и ге"
                                "роев. Самый рас"
                                "пространенный "
                                "ресурс - золото, "
                                "требуем практ"
                                "ически везде. Д"
                                "ревесина и руд"
                                "а используютс"
                                "я для возведен"
                                "ия большинств"
                                "а построек. Сам"
                                "оцветы, ртуть, с"
                                "ера и кристалл"
                                "ы - редкие магич"
                                "еские ресурсы, "
                                "нужные для воз"
                                "ведения лучши"
                                "х построек и по"
                                "купки сильных "
                                    "воинов.",
                                NORMAL_DIALOG_QUICK_VIEW,
                                -1,
                                -1,
                                -1,
                                0,
                                -1,
                                0,
                                -1,
                                0
                            );
                            break;
                    }
                }
                break;
            case WIDGET_COMMAND_DESELECT:
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
                        msg.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
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

MessageDispatchResult TrueFalseDialogHandler(struct tag_message& msg) {
    return EventWindowHandler(msg);
}

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

void CheckEndGame(
    CheckEndGameForcedResult forcedResult,
    b32 dragonCityCaptured
) {
    i32 showedDialog_o;
    i32 defeated_m;
    i32 allowNormalVictory;
    i32 lastLivingPlayer_j;
    i32 survivingHumans_a;
    i32 lastHuman_a;
    i32 netHumanCount;
    i32 player;
    i32 heroIndex_m;
    i32 winFlag;
    playerData* rec_n;
    i32 savedRemoteOn_o;
    i32 numAlive;
    i32 sideBelow_i;
    i32 sideAbove;
    i32 bestGold;
    town* lossTown;
    town* victoryTownData;
    i32 currentDayIndex;
    i32 enemyRemaining;
    i32 hasRoland_j;
    i32 hasDwarfTown;
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

    bInCheckEndGame = 1;
    savedRemoteOn_o = gbRemoteOn;
    showedDialog_o = 0;

    for (player = 0; player < gpGame->m_playerCount; player++) {
        if (!gpGame->m_playerDead[player]) {
            rec_n = &gpGame->m_players[player];
            if ((rec_n->m_heroCount == 0 && rec_n->m_townCount == 0)
                || (xIsPlayingExpansionCampaign && xCampaign.IsSpecialLossCondition(player))) {
                PlayerDead(player);
                if (player == giThisGamePos) {
                    showedDialog_o = 1;
                    sprintf(gText, "Вы были исключены из игры!!!");
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                } else {
                    sprintf(gText, "%s сокрушен!", cPlayerNames[player]);
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
                            "%s, вы потеряли ваш последний город. Если вы не завоюете себе город в течение следующих 7 дней, вы проиграете.",
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
                            showedDialog_o = 1;
                            sprintf(
                                gText,
                                "%s, ваши герои покинули вас, а вы были изгнаны с позором из этих земель.",
                                cPlayerNames[player]
                            );
                        }
                    } else {
                        sprintf(
                            gText,
                            "%s был брошен своими героями и изгнан с позором из этих земель.",
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

    winFlag = 0;
    defeated_m = 0;
    allowNormalVictory = 1;
    if ((gpGame->m_mapHeader.victoryCondition != MAP_VICTORY_DEFEAT_ALL
         && !gpGame->m_mapHeader.allowNormalVictory)
        || (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ARCHIBALD
            && gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET == END_GAME_SIDE_SCENARIO)) {
        allowNormalVictory = 0;
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
                    winFlag = 1;
                }
            }
        } else if (sideAbove == 0) {
            for (player = 0; player < gpGame->m_playerCount; player++) {
                if (gbThisNetHumanPlayer[player] && !gpGame->m_playerDead[player]
                    && gpGame->m_players[player].m_color
                           < gpGame->m_mapHeader.victorySideThreshold) {
                    winFlag = 1;
                }
            }
        }
        if (sideBelow_i == 0 || sideAbove == 0) {
            if (!winFlag) {
                defeated_m = 1;
            }
            if (!showedDialog_o && winFlag) {
                showedDialog_o = 1;
                sprintf(gText, "Враг разбит, а ваша армия празднует триумф!");
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_CAPTURE_TOWN) {
        victoryTownData = gpGame->GetTown(gpGame->GetTownId(
            gpGame->m_mapHeader.victoryConditionValue,
            gpGame->m_mapHeader.victoryTownY
        ));
        if (victoryTownData->m_owner != TOWN_OWNER_NONE
            && (gbHumanPlayer[victoryTownData->m_owner] || gpGame->m_mapHeader.computerAlsoWins)) {
            if (gbThisNetHumanPlayer[victoryTownData->m_owner]) {
                winFlag = 1;
            } else {
                defeated_m = 1;
            }
            if (!showedDialog_o) {
                showedDialog_o = 1;
                if (winFlag) {
                    sprintf(
                        gText,
                        "Вы захватили %s! Вы победили.",
                        victoryTownData->m_name
                    );
                } else {
                    sprintf(
                        gText,
                        "Враг захватил %s! Они празднуют триумф.",
                        victoryTownData->m_name
                    );
                }
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (gpGame->m_mapHeader.lossCondition == MAP_LOSS_TOWN) {
        lossTown = gpGame->GetTown(
            gpGame->GetTownId(gpGame->m_mapHeader.lossConditionValue, gpGame->m_mapHeader.lossTownY)
        );
        if (lossTown->m_owner == TOWN_OWNER_NONE || !gbHumanPlayer[lossTown->m_owner]) {
            defeated_m = 1;
            if (!showedDialog_o) {
                showedDialog_o = 1;
                sprintf(gText, "%s пал! Все потеряно.", lossTown->m_name);
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_ACCUMULATE_GOLD) {
        bestGold = 0;
        winnerPlayer_m = END_GAME_NO_PLAYER;
        for (player = 0; player < gpGame->m_playerCount; player++) {
            if ((gbHumanPlayer[player] || gpGame->m_mapHeader.computerAlsoWins)
                && gpGame->m_players[player].m_resources[(RES_GOLD)]
                       >= gpGame->m_mapHeader.victoryConditionValue * END_GAME_GOLD_SCALE
                && gpGame->m_players[player].m_resources[(RES_GOLD)] >= bestGold) {
                bestGold = gpGame->m_players[player].m_resources[(RES_GOLD)];
                winnerPlayer_m = player;
            }
            if (winnerPlayer_m != END_GAME_NO_PLAYER) {
                if (gbThisNetHumanPlayer[(winnerPlayer_m)]) {
                    winFlag = 1;
                } else {
                    defeated_m = 1;
                }
                if (!showedDialog_o) {
                    showedDialog_o = 1;
                    if (winFlag) {
                        sprintf(
                            gText,
                            "Вы собрали более %d золотых в своей казне. Все враги преклонились перед силой вашего состояния.",
                            bestGold
                        );
                    } else {
                        sprintf(
                            gText,
                            "Враг собрал более %d золотых в своей казне. Вы должны преклониться перед силой такого состояния и признать свое поражение.",
                            bestGold
                        );
                    }
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }
    }

    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_HERO) {
        winningHeroEntry_g = GetHeroSlot(gpGame->m_mapHeader.victoryConditionValue);
        if (winningHeroEntry_g->m_owner < 0 || winningHeroEntry_g->m_owner >= GAME_PLAYER_COUNT
            || gbHumanPlayer[winningHeroEntry_g->m_owner]) {
            winFlag = 1;
            if (!showedDialog_o) {
                showedDialog_o = 1;
                sprintf(
                    gText,
                    "%s - вражеский герой, у вас в плену! Ваше задание завершено.",
                    winningHeroEntry_g->m_name
                );
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (gpGame->m_mapHeader.lossCondition == MAP_LOSS_HERO) {
        lossHero_k = GetHeroSlot(gpGame->m_mapHeader.lossConditionValue);
        if (lossHero_k->m_owner < 0 || lossHero_k->m_owner >= GAME_PLAYER_COUNT
            || !gbHumanPlayer[lossHero_k->m_owner]) {
            defeated_m = 1;
            if (!showedDialog_o) {
                showedDialog_o = 1;
                sprintf(gText, "%s - ваш герой, был повержен.  Вы провалили ваше задание.", lossHero_k->m_name);
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (gpGame->m_mapHeader.lossCondition == MAP_LOSS_TIME) {
        if (gpGame->m_day + (gpGame->m_week - 1) * CALENDAR_DAYS_PER_WEEK
                + (gpGame->m_month - 1) * CALENDAR_DAYS_PER_MONTH
            > gpGame->m_mapHeader.lossConditionValue) {
            defeated_m = 1;
            if (!showedDialog_o) {
                showedDialog_o = 1;
                sprintf(gText, "Вы не успели завершить ваше задание в срок. Все потеряно.");
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
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
                winFlag = 1;
            } else {
                defeated_m = 1;
            }
            if (!showedDialog_o) {
                showedDialog_o = 1;
                if (gpGame->m_mapHeader.victoryConditionValue == END_GAME_ULTIMATE_ARTIFACT) {
                    sprintf(
                        artifactName,
                        "Могущественный артефакт"

                    );
                } else {
                    sprintf(
                        artifactName,
                        gArtifactNames[gpGame->m_mapHeader.victoryConditionValue - 1]
                    );
                }
                if (winFlag) {
                    sprintf(gText, "Вы нашли %s. Вы справились с заданием.", artifactName);
                } else {
                    sprintf(
                        gText,
                        "Враг нашел %s. Вы не справились со своим заданием.",
                        artifactName
                    );
                }
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ROLAND
        && gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET == END_GAME_DWARF_SCENARIO) {
        hasDwarfTown = 0;
        for (player = 0; player < gpGame->m_players[0].m_townCount; player++) {
            if (gpGame->GetTown(gpGame->m_players[0].m_townIds[player])->m_type
                == FACTION_SORCERESS) {
                hasDwarfTown = 1;
            }
        }
        if (!hasDwarfTown) {
            defeated_m = 1;
            if (!showedDialog_o) {
                showedDialog_o = 1;
                sprintf(
                    gText,
                    "Все города гномов пали. Это сокрушительное поражение! Вы проиграли."
                );
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ARCHIBALD
        && gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET == END_GAME_SIDE_SCENARIO
        && dragonCityCaptured) {
        winFlag = 1;
        if (!showedDialog_o) {
            showedDialog_o = 1;
            sprintf(gText, "Драконий город пал! Теперь вы Повелитель драконов.");
            NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
    }

    if (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ROLAND
        && gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET
               == END_GAME_ROLAND_CAPTURE_SCENARIO) {
        hasRoland_j = 0;
        for (player = 0; player < GAME_HERO_COUNT; player++) {
            if (gpGame->m_heroRecs[player].m_portrait == CAMPAIGN_HERO_ROLAND
                && gpGame->m_heroRecs[player].m_owner >= 0
                && gpGame->m_heroRecs[player].m_owner <= GAME_PLAYER_COUNT - 1) {
                hasRoland_j = 1;
            }
        }
        if (!hasRoland_j) {
            defeated_m = 1;
            if (!showedDialog_o) {
                showedDialog_o = 1;
                sprintf(gText, "Роланд пленен! Все потеряно.");
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ROLAND
        && gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET
               == END_GAME_ROLAND_CAPTURE_SCENARIO) {
        enemyRemaining = 0;
        for (player = 0; player < gpGame->m_playerCount; player++) {
            if (!gpGame->m_playerDead[player]
                && gpGame->m_players[player].m_color != PLAYER_COLOR_BLUE
                && gpGame->m_players[player].m_color != PLAYER_COLOR_YELLOW) {
                enemyRemaining = 1;
            }
        }
        if (!enemyRemaining) {
            winFlag = 1;
            if (!showedDialog_o && winFlag) {
                showedDialog_o = 1;
                sprintf(gText, "Враг разбит, а ваша армия празднует триумф!");
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (defeated_m) {
        gbGameOver = true;
        giEndSequence = 0;
    }
    if (winFlag) {
        gbGameOver = true;
        giEndSequence = 1;
    }

    if (numAlive == 1 || survivingHumans_a == 0
        || (survivingHumans_a == 1 && !gbThisNetHumanPlayer[lastHuman_a])) {
        if (survivingHumans_a == 1 && gbThisNetHumanPlayer[lastHuman_a]) {
            if (allowNormalVictory) {
                gbGameOver = true;
                giEndSequence = 1;
            }
        } else {
            gbGameOver = true;
            giEndSequence = 0;
        }
    }

    if (savedRemoteOn_o && netHumanCount == 0) {
        gbGameOver = true;
        giEndSequence = 0;
    }
    if (forcedResult == END_GAME_FORCE_VICTORY) {
        winFlag = 1;
        gbGameOver = true;
        giEndSequence = 1;
    }
    if (forcedResult == END_GAME_FORCE_DEFEAT) {
        defeated_m = 1;
        gbGameOver = true;
        giEndSequence = 0;
    }

    if (giEndSequence == 1 && gbGameOver) {
        winFlag = 1;
    }
    if (giEndSequence == 0 && gbGameOver) {
        defeated_m = 1;
    }

    if (gbInCampaign && winFlag) {
        currentDayIndex = (gpGame->m_month - 1) * CALENDAR_DAYS_PER_MONTH
                          + (gpGame->m_week - 1) * CALENDAR_DAYS_PER_WEEK + gpGame->m_day;
        gpGame->m_campaignScenarioWon = 1;
        gpGame
            ->m_campaignScenarioCompleted[(gpGame->m_campaignType)][gpGame->m_campaignScenario] =
            1;
        gpGame->m_campaignScenarioDays[(gpGame->m_campaignType)][gpGame->m_campaignScenario] =
            currentDayIndex;
        gpGame->m_campaignScore =
            gpGame->m_campaignScenarioBonus[(gpGame->m_campaignType)][gpGame->m_campaignScenario]
            + gpGame->m_campaignScenarioDays[(gpGame->m_campaignType)]
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

        if (carryoverHeroId != END_GAME_NO_PLAYER) {
            for (player = 0; player < CAMPAIGN_ARMY_SLOT_COUNT; player++) {
                gpGame->m_campaignCarryoverCreatureTypes[player] = CREATURE_NONE;
                gpGame->m_campaignCarryoverCreatureCounts[player] = 0;
            }
            for (campaignHeroIndex = 0; campaignHeroIndex < gpGame->m_players[0].m_heroCount;
                 campaignHeroIndex++) {
                if (carryoverHeroId == CAMPAIGN_SWITCH_VICTORY_VALUE
                    || (gpGame->m_heroRecs[gpGame->m_players[0].m_heroIds[campaignHeroIndex]]
                               .m_portrait)
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
                "ПОБЕДА_",
                gpGame->m_campaignType == CAMPAIGN_ROLAND ? 'G' : 'E',
                gpGame->m_campaignScenario + 1
            );
            gpGame->SaveGame(campaignSaveName, 1, 0);
        }
    } else if (xIsPlayingExpansionCampaign && winFlag) {
        xCampaign.Autosave();
    }

    bInCheckEndGame = 0;
}

void QuickViewWait(void) {
    tag_message event;
    i32 done;
    gpMouseManager->ReallyHidePointer();
    done = 0;
    while (!done) {
        PollSound();
        Process1WindowsMessage();
        event = gpInputManager->GetEvent();
        done = event.type == MESSAGE_RIGHT_BUTTON_UP || event.type == MESSAGE_LEFT_BUTTON_DOWN
            || event.type == MESSAGE_LEFT_BUTTON_UP;
    }
    gpMouseManager->ReallyShowPointer();
}

void InitVars(void) {
    i32 i;
    i32 j;
    gGameCommand = -1;
    gPalette = NULL;
    gbCombatSurrender = false;
    gpGame->m_viewArmyResult = 0;
    strcpy(gpGame->m_mapFilename, "brokena.mp2");
    gpGame->m_newGameInitialized = 0;
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

void game::ShowMoraleInfo(hero* h, i32 dialogType) {
    i32 mixedUndead4;
    i32 alignment_e;
    ArmyGroupAlignmentResult homogeneous5;
    i32 modifierStart;
    char description7[MORALE_LUCK_DESCRIPTION_SIZE];
    i32 slot8;

    mixedUndead4 = 0;
    if (h->m_army.GetMorale(h, h->GetOccupiedTown(), NULL) > 0)
        sprintf(description7, cMoraleInfo[(MORALE_INFO_GOOD)]);
    else {
        if (h->m_army.GetMorale(h, h->GetOccupiedTown(), NULL) == 0)
            sprintf(description7, cMoraleInfo[(MORALE_INFO_NEUTRAL)]);
        else
            sprintf(description7, cMoraleInfo[(MORALE_INFO_BAD)]);
    }

    sprintf(gText, cMoraleInfo[(MORALE_INFO_HEADER)], description7);
    modifierStart = strlen(gText);
    if (h->m_army.HasAllUndead()) {
        strcat(gText, cMoraleInfo[(INFO_ALL_UNDEAD)]);
        goto showDialog;
    }
    if (h->m_army.HasSomeUndead() || h->HasArtifact(ARTIFACT_ARM_OF_MARTYR)) {
        strcat(gText, cMoraleInfo[(INFO_SOME_UNDEAD)]);
        mixedUndead4 = 1;
    }

    homogeneous5 = h->m_army.IsHomogeneous(-1);
    if (mixedUndead4 && homogeneous5 > ARMY_GROUP_ALIGNMENT_NO_MODIFIER) {
        homogeneous5 = ARMY_GROUP_ALIGNMENT_NO_MODIFIER;
    }
    if (homogeneous5 > ARMY_GROUP_ALIGNMENT_NO_MODIFIER) {
        alignment_e = 0;
        for (slot8 = 0; slot8 < ARMY_GROUP_SLOT_COUNT; slot8++) {
            if (h->m_army.m_creatureTypes[slot8] != CREATURE_NONE) {
                alignment_e = (gMonsterDatabase[(h->m_army.m_creatureTypes[slot8])].race);
            }
        }
        sprintf(description7, cMoraleInfo[(INFO_SAME_ALIGNMENT)], gAlignmentNames[alignment_e]);
        strcat(gText, description7);
    }
    if (homogeneous5 == ARMY_GROUP_ALIGNMENT_THREE) {
        sprintf(description7, cMoraleInfo[(INFO_THREE_ALIGNMENTS)]);
        strcat(gText, description7);
    }
    if (homogeneous5 == ARMY_GROUP_ALIGNMENT_FOUR) {
        sprintf(description7, cMoraleInfo[(INFO_FOUR_ALIGNMENTS)]);
        strcat(gText, description7);
    }
    if (homogeneous5 == ARMY_GROUP_ALIGNMENT_FIVE_OR_MORE) {
        sprintf(description7, cMoraleInfo[(INFO_FIVE_ALIGNMENTS)]);
        strcat(gText, description7);
    }

    if (h->GetOccupiedTown() != NULL && h->GetOccupiedTown()->m_type == FACTION_BARBARIAN
        && (h->GetOccupiedTown()->m_buildings & (TOWN_BUILDING_COLISEUM))) {
        strcat(gText, cMoraleInfo[(INFO_COLISEUM)]);
    }
    if (h->GetOccupiedTown() != NULL
        && (h->GetOccupiedTown()->m_buildings & (TOWN_BUILDING_TAVERN))) {
        strcat(gText, cMoraleInfo[(INFO_TAVERN)]);
    }

    if (h->HasArtifact(ARTIFACT_MEDAL_OF_VALOR)) {
        strcat(gText, cMoraleInfo[(INFO_MEDAL_OF_VALOR)]);
    }
    if (h->HasArtifact(ARTIFACT_MEDAL_OF_COURAGE)) {
        strcat(gText, cMoraleInfo[(INFO_MEDAL_OF_COURAGE)]);
    }
    if (h->HasArtifact(ARTIFACT_MEDAL_OF_HONOR)) {
        strcat(gText, cMoraleInfo[(INFO_MEDAL_OF_HONOR)]);
    }
    if (h->HasArtifact(ARTIFACT_MEDAL_OF_DISTINCTION)) {
        strcat(gText, cMoraleInfo[(INFO_MEDAL_OF_DISTINCTION)]);
    }
    if (h->HasArtifact(ARTIFACT_FIZBIN_OF_MISFORTUNE)) {
        strcat(gText, cMoraleInfo[(INFO_FIZBIN)]);
    }
    if ((((h->m_eventFlags) & (HERO_EVENT_BUOY)))) {
        strcat(gText, cMoraleInfo[(INFO_BUOY)]);
    }
    if ((((h->m_eventFlags) & (HERO_EVENT_OASIS)))) {
        strcat(gText, cMoraleInfo[(INFO_OASIS)]);
    }
    if ((((h->m_eventFlags) & (HERO_EVENT_TEMPLE)))) {
        strcat(gText, cMoraleInfo[(INFO_TEMPLE)]);
    }
    if ((((h->m_eventFlags) & (HERO_EVENT_GRAVEYARD)))) {
        strcat(gText, cMoraleInfo[(INFO_GRAVEYARD)]);
    }
    if ((((h->m_eventFlags) & (HERO_EVENT_SHIPWRECK)))) {
        strcat(gText, cMoraleInfo[(INFO_SHIPWRECK)]);
    }
    if ((((h->m_eventFlags) & (HERO_EVENT_WATERING_HOLE)))) {
        strcat(gText, cMoraleInfo[(INFO_WATERING_HOLE)]);
    }
    if ((((h->m_eventFlags) & (HERO_EVENT_DERELICT_SHIP)))) {
        strcat(gText, cMoraleInfo[(INFO_DERELICT_SHIP)]);
    }
    if (h->m_secondarySkills[(HERO_SKILL_LEADERSHIP)] == HERO_SKILL_LEVEL_BASIC) {
        strcat(gText, cMoraleInfo[(INFO_BASIC_LEADERSHIP)]);
    }
    if (h->m_secondarySkills[(HERO_SKILL_LEADERSHIP)] == HERO_SKILL_LEVEL_ADVANCED) {
        strcat(gText, cMoraleInfo[(INFO_ADVANCED_LEADERSHIP)]);
    }
    if (h->m_secondarySkills[(HERO_SKILL_LEADERSHIP)] == HERO_SKILL_LEVEL_EXPERT) {
        strcat(gText, cMoraleInfo[(INFO_EXPERT_LEADERSHIP)]);
    }
    if (h->HasArtifact(ARTIFACT_MASTHEAD) && (((h->m_eventFlags) & (HERO_EVENT_EMBARKED)))) {
        strcat(gText, cMoraleInfo[(MORALE_INFO_MASTHEAD)]);
    }
    if (h->HasArtifact(ARTIFACT_BATTLE_GARB)) {
        strcat(gText, cMoraleInfo[(MORALE_INFO_BATTLE_GARB)]);
    }
    if (modifierStart == static_cast<i32>(strlen(gText))) {
        strcat(gText, cMoraleInfo[(MORALE_INFO_NONE)]);
    }

showDialog:
    NormalDialog(gText, dialogType, -1, -1, -1, 0, -1, 0, -1, 0);
}

void game::ShowLuckInfo(hero* h, i32 dialogType) {
    char description4[MORALE_LUCK_DESCRIPTION_SIZE];
    i32 luckValue;
    i32 modifierStart;

    if (gpGame->GetLuck(h, NULL, h->GetOccupiedTown()) > 0)
        sprintf(description4, cLuckInfo[(LUCK_INFO_GOOD)]);
    else {
        if (gpGame->GetLuck(h, NULL, h->GetOccupiedTown()) == 0)
            sprintf(description4, cLuckInfo[(LUCK_INFO_NEUTRAL)]);
        else
            sprintf(description4, cLuckInfo[(LUCK_INFO_BAD)]);
    }

    sprintf(gText, cLuckInfo[(LUCK_INFO_HEADER)], description4);
    modifierStart = strlen(gText);
    if (h->GetOccupiedTown() != NULL && h->GetOccupiedTown()->m_type == FACTION_SORCERESS
        && (h->GetOccupiedTown()->m_buildings & (TOWN_BUILDING_RAINBOW)))
        strcat(gText, cLuckInfo[(INFO_RAINBOW)]);
    if (h->HasArtifact(ARTIFACT_RABBIT_FOOT))
        strcat(gText, cLuckInfo[(INFO_RABBIT_FOOT)]);
    if (h->HasArtifact(ARTIFACT_GOLDEN_HORSESHOE))
        strcat(gText, cLuckInfo[(INFO_HORSESHOE)]);
    if (h->HasArtifact(ARTIFACT_GAMBLERS_COIN))
        strcat(gText, cLuckInfo[(INFO_LUCKY_COIN)]);
    if (h->HasArtifact(ARTIFACT_FOUR_LEAF_CLOVER))
        strcat(gText, cLuckInfo[(INFO_CLOVER)]);
    if ((((h->m_eventFlags) & (HERO_EVENT_FAERIE_RING))))
        strcat(gText, cLuckInfo[(INFO_FAERIE_RING)]);
    if ((((h->m_eventFlags) & (HERO_EVENT_IDOL))))
        strcat(gText, cLuckInfo[(INFO_IDOL)]);
    if ((((h->m_eventFlags) & (HERO_EVENT_FOUNTAIN))))
        strcat(gText, cLuckInfo[(INFO_FOUNTAIN)]);
    if ((((h->m_eventFlags) & (HERO_EVENT_PYRAMID))))
        strcat(gText, cLuckInfo[(INFO_PYRAMID)]);
    if (h->m_secondarySkills[(HERO_SKILL_LUCK)] == HERO_SKILL_LEVEL_BASIC)
        strcat(gText, cLuckInfo[(INFO_BASIC_SKILL)]);
    if (h->m_secondarySkills[(HERO_SKILL_LUCK)] == HERO_SKILL_LEVEL_ADVANCED)
        strcat(gText, cLuckInfo[(INFO_ADVANCED_SKILL)]);
    if (h->m_secondarySkills[(HERO_SKILL_LUCK)] == HERO_SKILL_LEVEL_EXPERT)
        strcat(gText, cLuckInfo[(INFO_EXPERT_SKILL)]);
    if (h->HasArtifact(ARTIFACT_MASTHEAD) && (((h->m_eventFlags) & (HERO_EVENT_EMBARKED))))
        strcat(gText, cLuckInfo[(LUCK_INFO_MASTHEAD)]);
    if ((((h->m_eventFlags) & (HERO_EVENT_MERMAID))))
        strcat(gText, cLuckInfo[(INFO_MERMAID)]);
    if (h->HasArtifact(ARTIFACT_BATTLE_GARB))
        strcat(gText, cLuckInfo[(LUCK_INFO_BATTLE_GARB)]);
    if (modifierStart == static_cast<i32>(strlen(gText)))
        strcat(gText, cLuckInfo[(LUCK_INFO_NONE)]);

    NormalDialog(gText, dialogType, -1, -1, -1, 0, -1, 0, -1, 0);
}

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

i32 GetMonType(i32 score, HighScoreType campaign) {
    i32 idx;
    for (idx = (CREATURE_COUNT) - 1; idx >= 0; idx--) {
        if (campaign == HIGH_SCORE_CAMPAIGN || campaign == HIGH_SCORE_EXPANSION_CAMPAIGN) {
            if (score <= giScoreCampaignMon[idx][(MONSTER_SCORE_THRESHOLD)])
                return giScoreCampaignMon[idx][(MONSTER_SCORE_TYPE)];
        } else {
            if (score >= giScoreMon[idx][(MONSTER_SCORE_THRESHOLD)])
                return giScoreMon[idx][(MONSTER_SCORE_TYPE)];
        }
    }
    return giScoreMon[0][(MONSTER_SCORE_TYPE)];
}

i32 AddScoreToHighScore(
    i32 score,
    i32 days,
    i32 scenario,
    HighScoreType highScoreType,
    char* scenarioName
) {
    i32 dest_o;
    HighScoreEntry entries_a[HIGH_SCORE_ENTRY_COUNT];
    i32 file_c;
    i32 entry_a;
    char filename_h[HIGH_SCORE_FILENAME_LENGTH];
    char playerName_c[HIGH_SCORE_INPUT_NAME_SIZE];
    i32 missingFile_e;

    missingFile_e = 0;
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
        missingFile_e = 1;
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
        for (dest_o = HIGH_SCORE_LAST_SHIFT_SOURCE; dest_o >= entry_a; dest_o--)
            entries_a[dest_o + 1] = entries_a[dest_o];

        GetDataEntry(
            "Пожалуйста, введите свое имя для таблицы рекордов.",
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
            write(file_c, &entries_a[entry_a], sizeof(HighScoreEntry));
        close(file_c);
    } else {
        gbShowHighScore = false;
    }
    return 0;
}

void BVResMsg(char* s, ResourceType res, i32 qty) {
    giBottomViewOverride = BOTTOM_VIEW_RESOURCE;
    giBottomViewOverrideEndTime = KBTickCount() + BOTTOM_VIEW_RESOURCE_MESSAGE_DURATION;
    giBottomViewResource = res;
    giBottomViewResourceQty = qty;
    strcpy(gcBottomViewText, s);
    gpAdvManager->UpdBottomView(1, 1, 1);
}

void GOut(char* str) {
    if (gpAdvManager->m_active == 1)
        AiPrint(str);
}

i32 NetPosToGamePos(i32 netPos) {
    for (i32 i = 0; i < GAME_PLAYER_COUNT; i++)
        if (gbGamePosToNetPos[i] == netPos)
            return i;
    return -1;
}

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

void PopNetBox(char* text, i32 netPlayer) {
    i32 textY_h;
    i32l messageTime_b;
    heroWindow* netWindow_j;
    i32 result_p;
    i32 textWidth_b;
    i32 textX_k;
    i32 savedShowIt_p;
    i32 updateInput_f;
    i32 inputLength_a;
    char inputText_b[BOX_TEXT_LENGTH];
    i32 exitForIncomingData_c;
    i32 sendText_b;
    tag_message event_o;
    tag_message updateMessage_i;
    i32 firstLineId_a;
    i32 delay_e;
    i32 lineTextLimit_g;
    i32 done_a;
    i32 redrawLines_l;
    i32 redrawSavedShowIt_a;
    KbRemotePacket* remoteData_g;
    i32 redrawAdventure_o;
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
    bShowIt = 1;
    gbMoveShown = false;
    netWindow_j = new heroWindow(
        0,
        BOX_WINDOW_Y,
        "netbox.bin"
    );
    if (netWindow_j == NULL)
        MemError();

    updateMessage_i.type = NET_BOX_UPDATE_MESSAGE;
    updateMessage_i.payload.widget.command = NET_BOX_TEXT_COMMAND;
    updateMessage_i.payload.widget.id = BOX_FIRST_LINE_ID;
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
    exitForIncomingData_c = 0;
    done_a = 0;
    updateInput_f = 1;
    cursorState_j = 0;
    sendText_b = 0;
    redrawLines_l = 1;
    redrawAdventure_o = 0;
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
                            bShowIt = 1;
                            redrawAdventure_o = 1;
                        }
                        gbLeaveNetBoxAlone = false;
                        updateInput_f = 1;
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
                        redrawLines_l = 1;
                        if (messageTime_b != 0)
                            messageTime_b = KBTickCount();
                        break;
                    default:
                        AddNetBoxLine(
                            "[ Входящая информация, необходимо выйти... ]",
                            BOX_DEFAULT_COLOR
                        );
                        redrawLines_l = 1;
                        exitForIncomingData_c = 1;
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
                        done_a = 1;
                        break;
                    case BOX_KEY_BACKSPACE:
                        if (inputLength_a > 0)
                            inputLength_a--;
                        updateInput_f = 1;
                        cursorState_j = 1;
                        break;
                    case BOX_KEY_ENTER:
                        sendText_b = 1;
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
                                updateInput_f = 1;
                                cursorState_j = 0;
                            }
                        }
                }
        }

        if (!updateInput_f && glTimers[GLOBAL_NET_BOX_CURSOR_TIMER_SLOT] < KBTickCount()) {
            cursorState_j = 1 - cursorState_j;
            updateInput_f = 1;
        }
        if (sendText_b) {
            sendText_b = 0;
            inputText_b[inputLength_a] = 0;
            AddNetBoxLine(inputText_b, gpGame->m_players[NetPosToGamePos(giThisNetPos)].m_color);
            result_p = TransmitRemoteData(
                inputText_b,
                BOX_PACKET_BUFFER_SIZE,
                strlen(inputText_b) + 1,
                BOX_REMOTE_CHAT,
                1,
                1,
                REMOTE_MESSAGE_DEFAULT
            );
            if (!result_p)
                ShutDown(NULL);
            inputLength_a = 0;
            strcpy(inputText_b, "");
            updateInput_f = 1;
            redrawLines_l = 1;
        }

        if (redrawLines_l) {
            redrawLines_l = 0;
            updateMessage_i.type = NET_BOX_UPDATE_MESSAGE;
            updateMessage_i.payload.widget.command = NET_BOX_TEXT_COMMAND;
            updateMessage_i.payload.widget.id = BOX_FIRST_LINE_ID;
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
            updateInput_f = 0;
            glTimers[GLOBAL_NET_BOX_CURSOR_TIMER_SLOT] = KBTickCount() + BOX_CURSOR_DELAY;
            if (cursorState_j)
                inputText_b[inputLength_a] = '_';
            else
                inputText_b[inputLength_a] = BOX_CURSOR_GLYPH;
            inputText_b[inputLength_a + 1] = 0;
            updateMessage_i.type = NET_BOX_UPDATE_MESSAGE;
            updateMessage_i.payload.widget.command = NET_BOX_TEXT_COMMAND;
            updateMessage_i.payload.widget.id = BOX_INPUT_ID;
            updateMessage_i.payload.widget.data.text = inputText_b;
            netWindow_j->BroadcastMessage(updateMessage_i);
            netWindow_j->DrawWindow();
            gpWindowManager->UpdateScreenRegion(0, BOX_INPUT_Y, BOX_WIDTH, BOX_INPUT_HEIGHT);
        }

        if (messageTime_b != 0 && messageTime_b + BOX_MESSAGE_TIMEOUT < KBTickCount())
            done_a = 1;
        if (exitForIncomingData_c) {
            for (delay_e = 0; delay_e < BOX_EXIT_DELAY_STEPS; delay_e++) {
                PollSound();
                DelayMilli(BOX_EXIT_DELAY);
            }
            done_a = 1;
        }
    }

    gpInputManager->SetKeyCodeType(INPUT_KEY_CODE_SCAN);
    if (redrawAdventure_o && gbMoveShown) {
        gbDrawWindowBackground = false;
        gpWindowManager->RemoveWindow(netWindow_j);
        gbDrawWindowBackground = true;
        redrawSavedShowIt_a = bShowIt;
        bShowIt = 1;
        gpAdvManager->RedrawAdvScreen(1, 0);
        bShowIt = redrawSavedShowIt_a;
    } else {
        gpWindowManager->RemoveWindow(netWindow_j);
    }
    bShowIt = savedShowIt_p;
}

void AddNetBoxLine(char* str, char color) {
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

void ShutDown(char* msg) {
    char buf[GLOBAL_TEXT_BUFFER_SIZE];
    if (bInShutDown)
        return;
    LogStr("Shutdown");
    bInShutDown = 1;
    gbClosingApp = true;
    buf[0] = 0;
    gpMouseManager->SetColorMice(0);
    if (msg) {
        strcpy(buf, msg);
        SetFullScreenStatus(0);
        LogStr(buf);
        MessageBoxA(
            hwndApp,
            buf,
            "Непредвиденное прерывание программы",
            MB_ICONHAND
        );
    } else {
        sprintf(buf, "Пока!");
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

typedef enum FileErrorConstant {
    FILE_ERROR_BUFFER_SIZE = 500
} FileErrorConstant;

void FileError(char* filename) {
    char buf[FILE_ERROR_BUFFER_SIZE];
    i32 err;
    char buf1[FILE_ERROR_BUFFER_SIZE];
    err = errno;
    sprintf(buf1, "File Error %s", strerror(err));
    LogInt(
        buf1,
        err,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );
    sprintf(
        buf,
        "Ошибка открытия файла %s!",
        filename
    );
    ShutDown(buf);
}

typedef enum SmackFadeConstant {
    SMACK_FADE_FIRST_COLOR = WINGRAPH_SYSTEM_PALETTE_SIZE,
    SMACK_FADE_COLOR_LIMIT = WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE,
    SMACK_FADE_MATCH_COLOR_LIMIT = 36,
    SMACK_FADE_DISTANCE_SENTINEL = 999,
    SMACK_FADE_SCREEN_WIDTH = 640,
    SMACK_FADE_SCREEN_HEIGHT = 480,
    SMACK_FADE_BLUE_COMPONENT = 2,
    SMACK_FADE_GREEN_COMPONENT = 1,
    SMACK_FADE_RED_COMPONENT = 0
} SmackFadeConstant;

void SmackFade(u8* src, u8* dst) {
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
        b = (src[f * MISC_PALETTE_COMPONENT_BYTES + SMACK_FADE_RED_COMPONENT]
             + src[f * MISC_PALETTE_COMPONENT_BYTES + SMACK_FADE_GREEN_COMPONENT]
             + src[f * MISC_PALETTE_COMPONENT_BYTES + SMACK_FADE_BLUE_COMPONENT])
            / MISC_PALETTE_COMPONENT_BYTES;
        k = SMACK_FADE_DISTANCE_SENTINEL;
        for (h = SMACK_FADE_FIRST_COLOR; h < SMACK_FADE_MATCH_COLOR_LIMIT; h++) {
            c = (dst[h * MISC_PALETTE_COMPONENT_BYTES + SMACK_FADE_RED_COMPONENT]
                 + dst[h * MISC_PALETTE_COMPONENT_BYTES + SMACK_FADE_GREEN_COMPONENT]
                 + dst[h * MISC_PALETTE_COMPONENT_BYTES + SMACK_FADE_BLUE_COMPONENT])
                / MISC_PALETTE_COMPONENT_BYTES;
            j = abs(b - c);
            if (j < k) {
                k = j;
                a = h;
            }
        }
        memcpy(
            l + f * MISC_PALETTE_COMPONENT_BYTES,
            dst + a * MISC_PALETTE_COMPONENT_BYTES,
            MISC_PALETTE_COMPONENT_BYTES
        );
        g[f] = (u8)a;
    }
    FadeTo(src, l, HIGH_SCORE_FADE_STEPS);
    i = gpWindowManager->m_screen->m_pixels;
    for (d = 0; d < SMACK_FADE_SCREEN_WIDTH; d++) {
        for (e = 0; e < SMACK_FADE_SCREEN_HEIGHT; e++) {
            *i = g[*i];
            i++;
        }
    }
    gpWindowManager->UpdateScreen();
    UpdatePalette(reinterpret_cast<i8*>(dst));
    H2_FREE(l);
    H2_FREE(g);
}

void ShowCongrats(HighScoreType highScoreType) {
    u8 palette[MISC_PALETTE_BYTE_COUNT];
    i32 unused;
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
        sprintf(ratingText, "Читер!!!");

    if (highScoreType == HIGH_SCORE_STANDARD) {
        sprintf(
            congratsText,
            "Поздравляем!\n\nДней: %d\n"
                "Счет: %d\nСложность: %d\n\n"
                "Очки: %d\n\nРейтинг:\n%s\n",
            giCurTurn,
            baseScore,
            gpGame->m_difficultyRating,
            realScore,
            ratingText
        );
    } else if (highScoreType == HIGH_SCORE_EXPANSION_CAMPAIGN) {
        sprintf(
            congratsText,
            "Поздравляем!\n\nДней: "
                "%d\n\nРейтинг:\n%s\n",
            xCampaign.Days(),
            ratingText
        );
    } else {
        sprintf(
            congratsText,
            "Поздравляем!\n\nДней: "
                "%d\n\nРейтинг:\n%s\n",
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

void CongratsWait(void) {
    i32 command = 0;
    i32 done = 0;
    tag_message msg;
    gpInputManager->Flush();
    while (!done) {
        PollSound();
        Process1WindowsMessage();
        msg = gpInputManager->GetEvent();
        if (msg.type == MESSAGE_KEY_DOWN || msg.type == MESSAGE_LEFT_BUTTON_DOWN
            || msg.type == MESSAGE_LEFT_BUTTON_UP || msg.type == MESSAGE_RIGHT_BUTTON_DOWN
            || msg.type == MESSAGE_RIGHT_BUTTON_UP)
            done = 1;
    }
}

typedef enum SamplePlaybackConstant {
    SAMPLE_PLAYBACK_CHANNEL_GROUP = 2,
    SAMPLE_DEFAULT_WAIT_TIME = 4000
} SamplePlaybackConstant;

SAMPLE2 LoadPlaySample(char* name) {
    SAMPLE2 ss;
    ss = gpResourceManager->GetSample(name);
    if (ss) {
        ss->m_playbackData.channelType = SAMPLE_PLAYBACK_CHANNEL_GROUP;
        gpSoundManager->MemorySample(ss);
    }
    return ss;
}

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

typedef enum MemoryErrorConstant {
    MEMORY_ERROR_REQUEST_SIZE = 6400
} MemoryErrorConstant;

void MemError(void) {
    if (gbInMemError)
        return;
    gbInMemError = true;
    LogStr("Out of Memory");
    sprintf(
        gText,
        cOutOfMemory,
        "Недостаточно памяти."  ,
        MEMORY_ERROR_REQUEST_SIZE
    );
    ShutDown(gText);
}

char* GetTownName(i32 i) {
    town* t = GetCastleRec(i);
    return t->m_name;
}

void LoadSystemwideIcons(void) {
    gBuyBuildIcons = gpResourceManager->GetIcon(
        "buybuild.icn"
    );
    gSystemIcons = gpResourceManager->GetIcon(
        "system.icn"
    );
}

void UnloadSystemwideIcons(void) {
    gpResourceManager->Dispose((resource*)gBuyBuildIcons);
    gpResourceManager->Dispose((resource*)gSystemIcons);
}

void EarlyShutDownSystem(void) {}

i32 GameUnsaved(void) {
    if ((gpAdvManager && gpAdvManager->m_active == 1)
        || (gpCombatManager && gpCombatManager->m_active == 1)
        || (gpTownManager && gpTownManager->m_active == 1))
        return 1;
    else
        return 0;
}

i32 HandleAppSpecificMenuCommands(i32 command) {
    i32 menuChanged;
    hero* currentHeroRec;
    i32 loopIndex;
    HeroSecondarySkill secondarySkillIndex;
    HeroSkillLevel ssLevel;
    i32 formationHexIndex;

    menuChanged = 0;
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
            strcpy(gText, "Вы действительно хотите начать сначала?  (Эта игра будет потеряна)");
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
                "Вы действительно хотите загрузить новую игру? (Эта игра будет потеряна)"
            );
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
            menuChanged = 1;
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
                for (loopIndex = (SPELL_FIREBALL); loopIndex < APP_MENU_MAX_SPELLS; loopIndex++)
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
                gpCurPlayer->m_resources[loopIndex] += loopIndex == (RES_GOLD)
                    ? APP_MENU_GOLD_BONUS
                    : APP_MENU_RESOURCE_BONUS;
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
                    gpGame->GiveArmy(
                        &gpGame->m_heroRecs[gpCurPlayer->CurrentHero()].m_army,
                        static_cast<CreatureType>(command - APP_MENU_ARMY_FIRST),
                        APP_MENU_CHEAT_ARMY_QUANTITY,
                        -1
                    );
                    gpAdvManager->UpdBottomView(1, 1, 1);
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
                gpCombatManager->m_backgroundDrawn = 0;
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
                    &gpCombatManager->m_armies[(gpCombatManager->m_currentArmySide)]
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

void UpdateSystemOptionsMenu(void) {
    i32 menuCommand;
    i32 checkedCommand;

    if (gConfig.gfx[(giCurExe)].showMenu == 0)
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

void UpdateAppSpecificMenus(void* hMenu) {
    if (hMenu == hmnuAdv)
        UpdateSystemOptionsMenu();
}

void EarlyResizeWindow(i32 x, i32 y, i32 w, i32 h) {
    if (gbClosingApp)
        return;
}

typedef enum MapAreaConstant {
    MAP_AREA_ORIGIN = 16,
    MAP_AREA_LIMIT = 448
} MapAreaConstant;

i32 InMapArea(i32 x, i32 y) {
    return x >= MAP_AREA_ORIGIN && x < MAP_AREA_LIMIT && y >= MAP_AREA_ORIGIN && y < MAP_AREA_LIMIT;
}

typedef enum DynamicWindowConstant {
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
} DynamicWindowConstant;

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
    i32 bottomCornerPaddingNum_j;
    i32 numRows;
    widget* newWidgetTemp_p;
    i32 columnsSize_h;
    i32 topOffsetNum_n;
    i32 contentXPaddingCount_m;
    i32 centeredHeightCount_k;
    i32 centeredPadding_c;
    i32 topCornerPaddingCount;
    i32 bottomOffsetLocal_p;
    i32 rightOffset_p;
    i32 contentYPadding;
    i32 edge_d;
    i32 tileRowPos_k;
    i32 centeredWidthValue_b;
    i32 leftCornerPaddingLocal_e;
    i32 rightCornerPaddingValue_j;
    i32 stoneWidgetColorSize_c;
    i32 columnIndex_k;
    i32 bottomEdgeOffset_l;
    i32 tileWidth_k;
    i32 tileHeight_h;
    i32 topEdgeInset;

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
                "stonebk2.icn",
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
        "stonebk2.icn",
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
        "stonebk2.icn",
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
        "stonebk2.icn",
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
        "stonebk2.icn",
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
            "stonebk2.icn",
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
            "stonebk2.icn",
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
            "stonebk2.icn",
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
            "stonebk2.icn",
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

void TestDynamicWindow(i32 p1, i32 p2) {
    heroWindow* d;
    i32 e, a, b, u, r, c;
    i32 t;
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
    t = 0;
    gpInputManager->Flush();
    while (!t) {
        Process1WindowsMessage();
        switch (gpInputManager->GetEvent().type) {
            case MESSAGE_KEY_DOWN:
            case MESSAGE_LEFT_BUTTON_DOWN:
            case MESSAGE_RIGHT_BUTTON_DOWN:
                t = 1;
        }
    }
    gpWindowManager->RemoveWindow(d);
    delete d;
}

void HandleRemoteDeadPlayerExit(i32 pos) {
    SPlayerExit pe;
    if (pos == giThisGamePos) {
        if (!gpGame->TransmitSaveGame((giThisNetPos + 1) % giNumHumanPlayers, 1, 0))
            ShutDown(NULL);
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

typedef enum PlayerExitLocalConstant {
    PLAYER_EXIT_HOST_NET_POSITION = 0,
    PLAYER_EXIT_FIRST_GUEST_NET_POSITION = 1,
    PLAYER_EXIT_DIRECT_PLAYER_COUNT = 2,
    PLAYER_EXIT_MESSAGE_LENGTH = 500,
    PLAYER_EXIT_TRANSMIT_DELAY = 500,
    PLAYER_EXIT_MESSAGE_TIME = 5000,
    PLAYER_EXIT_HEARTBEAT_DISABLED = 0x0bebc1ff
} PlayerExitLocalConstant;

void HandleRemoteSuddenExit(void) {
    SPlayerExit exitInfo;
    i32 destination;
    if (!gbGameInitialized)
        return;
    if (!gbRemoteOn)
        return;
    exitInfo.netPosition = static_cast<i8>(giThisNetPos);
    exitInfo.gamePosition = static_cast<i8>(giThisGamePos);
    exitInfo.updateNetworkControl = gbThisNetGotAdventureControl;
    exitInfo.timedOut = 0;
    exitInfo.eliminated = 0;
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

void DropDownToOnePlayer(void) {
    RemoteCleanup();
    giNumHumanPlayers = 1;
    for (i32 i = 0; i < REMOTE_PLAYER_COUNT; i++)
        if (i != giThisNetPos)
            gbHumanPlayer[i] = 0;
    ComputeAdvNetControl();
}

void ReceiveHostReportsPlayerExit(i32 hostNetPosition, SPlayerExit exitInfo, i32 forwardedReport) {
    i32 showExitMessage_i;
    char playerExitMessage_k[PLAYER_EXIT_MESSAGE_LENGTH];
    i32 netPosition;

    showExitMessage_i = 0;
    if (!forwardedReport) {
        if (exitInfo.eliminated) {
            if (exitInfo.netPosition == giThisNetPos) {
                RemoteCleanup();
                sprintf(gText, "Вы были исключены из игры!!!");
                NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                gbGameOver = true;
                giEndSequence = 0;
                return;
            }

            sprintf(gText, "%s сокрушен!", gsNetPlayerInfo[exitInfo.netPosition].name);
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
            gpGame->SaveGame("Игрок Вышел", 1, 0);
            sprintf(
                gText,
                "%s покидает игру, а %s решает прекратить всю игру. Игра была сохранена под названием 'ИГРОК ВЫШЕЛ'. Желаете продолжить игру самостоятельно, где компьютер займет места выбывших игроков?",
                gsNetPlayerInfo[exitInfo.netPosition].name,
                gsNetPlayerInfo[hostNetPosition].name
            );
            NormalDialog(gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
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
                    "Игрок-сервер %s сообщает, что игрок %s покинул игру. Игра будет продолжена, а компьютер займет место %s.",
                    gsNetPlayerInfo[hostNetPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name
                );
            } else {
                sprintf(
                    playerExitMessage_k,
                    "Игрок-сервер %s сообщает, что игрок %s покинул игру. Игра будет продолжена, а компьютер займет место %s.",
                    gsNetPlayerInfo[hostNetPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name
                );
            }
            showExitMessage_i = 1;
        }
    }

    if (giThisNetPos > exitInfo.netPosition)
        giThisNetPos--;
    gbHumanPlayer[exitInfo.gamePosition] = 0;

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

void ReceiveRemotePlayerExit(SPlayerExit exitInfo) {
    i32 localPlayerLost_e;
    i32 sendReturn;
    i32 unusedPacketResult_g;
    i32 recipient;

    localPlayerLost_e = 0;
    lLastHeartbeatReceive[exitInfo.netPosition] = PLAYER_EXIT_HEARTBEAT_DISABLED;
    gpGame->SaveGame("Игрок Вышел", 1, 0);

    if (exitInfo.eliminated) {
        exitInfo.continueGame = 1;
        if (exitInfo.netPosition == giThisNetPos) {
            localPlayerLost_e = 1;
            goto exitInfoProcessed;
        }
        sprintf(gText, "%s сокрушен!", gsNetPlayerInfo[exitInfo.netPosition].name);
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
        exitInfo.continueGame = 1;
    } else {
        if (exitInfo.timedOut) {
            sprintf(
                gText,
                "%s вышел из игры. Данная игра сохранена под названием 'ИГРОК ВЫШЕЛ'. Желаете продолжить игру, с компьютером, занявшим место %s?",
                gsNetPlayerInfo[exitInfo.netPosition].name,
                gsNetPlayerInfo[exitInfo.netPosition].name
            );
        } else {
            sprintf(
                gText,
                "%s покидает игру. Данная игра сохранена под названием 'ИГРОК ВЫШЕЛ'. Желаете продолжить игру, с компьютером, занявшим место %s?",
                gsNetPlayerInfo[exitInfo.netPosition].name,
                gsNetPlayerInfo[exitInfo.netPosition].name
            );
        }
        NormalDialog(gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
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
                1,
                1,
                REMOTE_MESSAGE_DEFAULT
            );
        }
        if (localPlayerLost_e)
            goto playerExitHandled;
        giNumHumanPlayers--;
        gbHumanPlayer[exitInfo.gamePosition] = 0;
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
                    1,
                    1,
                    REMOTE_MESSAGE_DEFAULT
                );
            }
        }
        if (localPlayerLost_e)
            goto playerExitHandled;
        ReceiveHostReportsPlayerExit(0, exitInfo, 1);
    }

playerExitHandled:
    if (localPlayerLost_e) {
        sprintf(gText, "Вы были исключены из игры!!!");
        RemoteCleanup();
        NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
        gbGameOver = true;
        giEndSequence = 0;
        return;
    }
    if (!exitInfo.continueGame) {
        ShutDown(NULL);
    }
}

typedef enum CheckMemoryConstant {
    CHECK_MEMORY_INITIAL_AVAILABLE_KB = 5,
    CHECK_MEMORY_TOTAL_KB = 16000,
    CHECK_MEMORY_AVAILABLE_KB = 8000
} CheckMemoryConstant;

i32 CheckMem(void) {
    giTotalHighMem = CHECK_MEMORY_TOTAL_KB;
    giHighMemBuffer = CHECK_MEMORY_AVAILABLE_KB;
    return 1;
}

i32 GetManaCost(SpellType spell, hero* h) {
    i32 c = gsSpellInfo[(spell)].cost;
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

void SetWinText(heroWindow* j, i32 id) {
    i32 a = 0;
    i32 i;
    tag_message msg;
    for (i = 0; i < KB_WIN_SETUP_COUNT; i++) {
        if (gWinSetup[i].windowId == id) {
            a++;
            msg.type = MESSAGE_WIDGET;
            msg.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            msg.payload.widget.id = gWinSetup[i].widgetId;
            msg.payload.widget.data.text = gWinSetup[i].text;
            j->BroadcastMessage(msg);
        }
    }
}

typedef enum ShingleAnimationConstant {
    SHINGLE_ANIMATION_INTERVAL = 250,
    SHINGLE_ANIMATION_FRAME_COUNT = 39,
    SHINGLE_ANIMATION_FRAME_OFFSET = 1,
    SHINGLE_UPDATE_X = 46,
    SHINGLE_UPDATE_Y = 176,
    SHINGLE_UPDATE_WIDTH = 139,
    SHINGLE_UPDATE_HEIGHT = 187
} ShingleAnimationConstant;

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

typedef enum NormalDialogLayoutConstant {
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
} NormalDialogLayoutConstant;

inline i32 NormalDialogCenterOffset(i32 extent) {
    return extent / NORMAL_DIALOG_CENTER_PART_COUNT;
}

void NormalDialog(
    char* text,
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
    i16 showMessage_d;
    i32 textWidgetId;
    heroWindow* savedNormalDialogWindow;
    i32 savedPointerFrame;
    i32 windowHeight_h;
    char* orText;
    i32 showPrimaryBonus;
    tag_message message_b;
    i32 savedSecondResourceValue_n;
    i32 savedFirstResourceValue;
    widget* textPanel_j;
    i32 resourceSlot;
    i32 resourceY_f;
    i32 iconHeight_h;
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
    i32 panelHeight_d;

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
    showPrimaryBonus = 0;
    showMessage_d = 1;

    if (firstResourceType == NORMAL_DIALOG_PRIMARY_SKILL
        && firstResourceValue >= NORMAL_DIALOG_PRIMARY_BONUS_OFFSET) {
        firstResourceValue -= NORMAL_DIALOG_PRIMARY_BONUS_OFFSET;
        showPrimaryBonus = 1;
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
                    "%d/день",
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
                (gMonsterDatabase[resourceValue_c[resourceSlot]].race)
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

    message_b.type = NORMAL_DIALOG_DISABLE_MESSAGE;
    message_b.payload.widget.command = NORMAL_DIALOG_SET_TEXT_COMMAND;
    message_b.payload.widget.id = NORMAL_DIALOG_TEXT_WIDGET_ID;
    message_b.payload.widget.data.text = text;
    pNormalDialogWindow->BroadcastMessage(message_b);

    if (showOrText == NORMAL_DIALOG_SHOW_OR_TEXT) {
        orText = static_cast<char*>(H2_ALLOC(NORMAL_DIALOG_OR_TEXT_LENGTH));
        strcpy(orText, "или");
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

void UpdateNormalDialog(char* text) {
    i16 show = 1;
    tag_message evt;
    evt.type = MESSAGE_WIDGET;
    evt.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    evt.payload.widget.id = 1;
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

u8
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
u8 giGroundShape[GROUND_TILE_IMAGE_COUNT] = {
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

u8 gColorTableTan[DIM_PALETTE_COLOR_COUNT] = {
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
u8 gColorTableGray[DIM_PALETTE_COLOR_COUNT] = {
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
u8 gColorTableYellow[DIM_PALETTE_COLOR_COUNT] = {
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
u8 gColorTableScenWin[DIM_PALETTE_COLOR_COUNT] = {
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
u8 gColorTableDarkGray[DIM_PALETTE_COLOR_COUNT] = {
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
u8 gColorTableRed[DIM_PALETTE_COLOR_COUNT] = {
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
u8 gColorTableDarkBrown[DIM_PALETTE_COLOR_COUNT] = {
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
i32 MAP_WIDTH = MAP_DIMENSION_MEDIUM;
i32 MAP_HEIGHT = MAP_DIMENSION_MEDIUM;
u8* mapExtra = NULL;
b32 gbClosingApp = false;
b32 gbForegroundApp = false;
i32 giMainVideoModeColorDepth = WINGRAPH_COLOR_DEPTH;
i32 giMainVideoModeWidth = WINGRAPH_WIDTH;
i32 giMainVideoModeHeight = WINGRAPH_HEIGHT;
u8 gMapColors[RADAR_MAP_COLOR_COUNT] = {77, 98, 13, 104, 32, 118, 54, 206, 41, 0, 0, 0};
u8 gObjectColors[RADAR_OBJECT_COLOR_COUNT] =
    {16, 48, 98, 160, 126, 74, 110, 179, 100, 218, 12, 12, 12, 12, 12, 12};
u8 gOwnerColors[RADAR_OWNER_COLOR_COUNT] = {73, 105, 190, 114, 205, 138, 10, 0};
char* gTilesetFiles[(TILESET_COUNT)] = {
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
u8 bPuzzleDraw[PUZZLE_DRAW_TABLE_COUNT] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};
u8 uDimPal[DIM_PALETTE_SET_COUNT][DIM_PALETTE_LEVEL_COUNT][DIM_PALETTE_COLOR_COUNT] = {
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
u8 gColorTableLighten[DIM_PALETTE_COLOR_COUNT] = {
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
u8 gColorTableNoCycle[DIM_PALETTE_COLOR_COUNT] = {
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
font* smallFont = NULL;
font* bigFont = NULL;
b32 gbReturnAfterComputeExtent = false;
b32 gbAllowTextEntryEscape = true;
WindowColorCycleMode giCycleType = WINDOW_COLOR_CYCLE_DEFAULT;
i32 giScreenScroll = 1;
i32 giMenuCommand = -1;
b32 gbSendMouseMoveMessages = false;
b32 gbColorMice = true;
u32l gTownEligibleBuildMask[TOWN_ELIGIBLE_BUILD_MASK_COUNT] = {
    TOWN_ELIGIBLE_BUILD_KNIGHT_MASK,
    TOWN_ELIGIBLE_BUILD_BARBARIAN_MASK,
    TOWN_ELIGIBLE_BUILD_SORCERESS_MASK,
    TOWN_ELIGIBLE_BUILD_WARLOCK_MASK,
    TOWN_ELIGIBLE_BUILD_WIZARD_MASK,
    TOWN_ELIGIBLE_BUILD_NECROMANCER_MASK
};
u8 giMapSizes[KB_MAP_SIZE_COUNT] =
    {MAP_DIMENSION_SMALL, MAP_DIMENSION_MEDIUM, MAP_DIMENSION_LARGE, MAP_DIMENSION_XLARGE};
b32 gbUseEvilInterface = false;
char* cEvilTranslate[KB_INTERFACE_TYPE_COUNT][KB_INTERFACE_VARIANT_COUNT] = {
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
char gcAnimPath[GLOBAL_AGGREGATE_PATH_SIZE] = "\\ANIM2\\";
char gcGamePath[GLOBAL_GAME_PATH_SIZE] = ".\\GAMES\\";
char gcMapPath[GLOBAL_MAP_PATH_SIZE] = ".\\MAPS\\";
char gcMusicPath[GLOBAL_AGGREGATE_PATH_SIZE] = "\\TRACKS2\\";
b32 gbPutzingWithMouseCtr = false;
float gfCombatSpeedMod[KB_COMBAT_SPEED_COUNT] = {1.0f, 0.7f, 0.35f};
icon* gShingleAnim = NULL;
i32 iNextShingleAnim = 0;
i32 giDialogTimeout = 0;
i32 giNewMonsterCycleFrame = 0;
b32 gbNoCDRom = false;
b32 gbLeaveNetBoxAlone = false;
b32 gbDrawWindowBackground = true;
b32 gbCheatMenus = false;
b32 gbUseWaveout = false;
b32 gbShowAllMaps = false;
char* gCombatFxNames[KB_COMBAT_FX_COUNT] = {
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
i16 horseFrameFlip[MOVEMENT_FRAME_FLIP_COUNT] =
    {45, 46, 47, 48, 49, 50, 51, 52, 53, 179, 178, 177, 54, 175, 174, 55};
i16 boatFrameFlip[MOVEMENT_FRAME_FLIP_COUNT] =
    {0, 0, 9, 9, 18, 18, 27, 27, 36, 36, 155, 155, 146, 146, 137, 137};
i8 gCastleResources[CASTLE_RESOURCE_SLOT_COUNT] =
    {(RES_WOOD), (RES_ORE), -1, -1};
i16 gCastleAmounts[CASTLE_AMOUNT_COUNT] = {20, 20, 0, 0};
i32 gHeroGoldCost = HERO_RECRUITMENT_GOLD_COST;
i16 gVesaMode[VESA_MODE_VALUE_COUNT] =
    {640, 480, 256, VESA_SET_MODE_FUNCTION, VESA_MODE_640_480_256, 0};
tag_tilePoint normalDirTable[NORMAL_DIRECTION_COUNT] = {
    {0, -1, 16},
    {1, -1, 16},
    {1, 0, 16},
    {1, 1, 16},
    {0, 1, 16},
    {-1, 1, 16},
    {-1, 0, 16},
    {-1, -1, 16}
};
i32 gResourceBaseValue[RESOURCE_VALUE_COUNT] = {200, 300, 200, 300, 300, 300, 1};
i32 gInitResourcesHuman[STARTING_RESOURCE_DIFFICULTY_COUNT][STARTING_RESOURCE_TYPE_COUNT] = {
    {30, 10, 30, 10, 10, 10, 10000},
    {20, 5, 20, 5, 5, 5, 7500},
    {10, 2, 10, 2, 2, 2, 5000},
    {5, 0, 5, 0, 0, 0, 2500},
    {0, 0, 0, 0, 0, 0, 0}
};
i32 gInitResourcesComputer[STARTING_RESOURCE_DIFFICULTY_COUNT][STARTING_RESOURCE_TYPE_COUNT] = {
    {20, 5, 20, 5, 5, 5, 7500},
    {20, 5, 20, 5, 5, 5, 7500},
    {30, 10, 30, 10, 10, 10, 10000},
    {30, 10, 30, 10, 10, 10, 10000},
    {30, 10, 30, 10, 10, 10, 10000}
};
i32 gMineCharacteristics[MINE_CHARACTERISTIC_COUNT] = {2, 1, 2, 1, 1, 1, 1000};
i32 gSSValues[(HERO_SKILL_COUNT)][SECONDARY_SKILL_VALUE_LEVEL_COUNT] = {
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
u8
gArtifactLevel[KB_ARTIFACT_LEVEL_COUNT] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x04, 0x04, 0x02, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x02, 0x04, 0x02, 0x04, 0x02, 0x04, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x04, 0x02, 0x02, 0x08, 0x08, 0x08, 0x08, 0x02, 0x04, 0x04, 0x08, 0x04, 0x04,
    0x08, 0x08, 0x08, 0x04, 0x04, 0x04, 0x08, 0x04, 0x04, 0x04, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x02, 0x08, 0x08, 0x08, 0x02, 0x04, 0x04, 0x08, 0x08, 0x02, 0x02, 0x02, 0x04,
    0x04, 0x02, 0x04, 0x02, 0x04, 0x02, 0x10, 0x20, 0x20, 0x20, 0x20, 0x02, 0x08, 0x02, 0x08,
    0x02, 0x02, 0x08, 0x08, 0x02, 0x02, 0x02, 0x04, 0x02, 0x02, 0x02, 0x02, 0x04, 0x00
};
i32 gArtifactBaseRV[KB_ARTIFACT_BASE_VALUE_COUNT] = {
    13600, 22000, 18000, 14000, 19000, 18500, 22200, 25000, 6000,  4000, 4000,  5600,  1200,
    1200,  1200,  1200,  -1200, 2000,  1800,  1800,  2000,  1000,  3600, 5600,  4000,  5040,
    3060,  4420,  5610,  6630,  7000,  6000,  4000,  4500,  2250,  1200, 1200,  1200,  1200,
    3500,  1500,  500,   1000,  750,   500,   750,   750,   750,   750,  750,   750,   500,
    1500,  1000,  1000,  750,   750,   750,   750,   750,   750,   750,  1250,  750,   150,
    250,   3500,  1250,  250,   -1,    -1,    4000,  4000,  4000,  2000, 2000,  4000,  3800,
    7600,  3700,  7400,  0,     0,     0,     0,     0,     2500,  4800, 9000,  -3250, 36200,
    2000,  -1050, -1050, 10000, 10000, 15000, 720,   7500,  10000, 9200, 10000, 1500
};
i32 gUltArtifactAvgValue = ULTIMATE_ARTIFACT_AVERAGE_VALUE;
i32 giDebugLevel = 0;
i8 giVisRangeTown = TOWN_VISIBILITY_RADIUS;
tag_monsterInfo gMonsterDatabase[(CREATURE_COUNT)] = {
    {{20, 33}, 17, 12, 1, FACTION_KNIGHT, 2, 1, 1, 1, 1, 0, "psnt", MONSTER_FLAGS_NONE},
    {{150, 312}, 21, 8, 10, FACTION_KNIGHT, 2, 5, 3, 2, 3, 12, "arch", MONSTER_ATTRIBUTE_RANGED},
    {{200, 463}, 23, 8, 10, FACTION_KNIGHT, 4, 5, 3, 2, 3, 24, "arch", MONSTER_ATTRIBUTE_RANGED},
    {{200, 639}, 32, 5, 15, FACTION_KNIGHT, 4, 5, 9, 3, 4, 0, "pike", MONSTER_FLAGS_NONE},
    {{250, 824}, 33, 5, 20, FACTION_KNIGHT, 5, 5, 9, 3, 4, 0, "pike", MONSTER_FLAGS_NONE},
    {{250, 1130}, 45, 4, 25, FACTION_KNIGHT, 4, 7, 9, 4, 6, 0, "swdm", MONSTER_FLAGS_NONE},
    {{300, 1350}, 45, 4, 30, FACTION_KNIGHT, 5, 7, 9, 4, 6, 0, "swdm", MONSTER_FLAGS_NONE},
    {{300, 1830}, 61, 3, 30, FACTION_KNIGHT, 6, 10, 9, 5, 10, 0, "cavl", MONSTER_ATTRIBUTE_WIDE},
    {{375, 2273}, 61, 3, 40, FACTION_KNIGHT, 7, 10, 9, 5, 10, 0, "cavl", MONSTER_ATTRIBUTE_WIDE},
    {{600, 4704}, 78, 2, 50, FACTION_KNIGHT, 5, 11, 12, 10, 20, 0, "pldn", MONSTER_FLAGS_NONE},
    {{1000, 5822}, 58, 2, 65, FACTION_KNIGHT, 6, 11, 12, 10, 20, 0, "pldn", MONSTER_FLAGS_NONE},
    {{40, 109}, 27, 10, 3, FACTION_BARBARIAN, 4, 3, 1, 1, 2, 0, "gbln", MONSTER_FLAGS_NONE},
    {{140, 299}, 21, 8, 10, FACTION_BARBARIAN, 2, 3, 4, 2, 3, 8, "elf_", MONSTER_ATTRIBUTE_RANGED},
    {{175, 512}, 29, 8, 15, FACTION_BARBARIAN, 3, 3, 4, 3, 4, 16, "elf_", MONSTER_ATTRIBUTE_RANGED},
    {{200, 865}, 43, 5, 20, FACTION_BARBARIAN, 6, 6, 2, 3, 5, 0, "wolf", MONSTER_ATTRIBUTE_WIDE},
    {{300, 1065}, 36, 4, 40, FACTION_BARBARIAN, 2, 9, 5, 4, 6, 0, "ogre", MONSTER_FLAGS_NONE},
    {{500, 2070}, 41, 4, 60, FACTION_BARBARIAN, 4, 9, 5, 5, 7, 0, "ogre", MONSTER_FLAGS_NONE},
    {{600, 1921},
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
     MONSTER_ATTRIBUTE_RANGED},
    {{700, 2337},
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
     MONSTER_ATTRIBUTE_RANGED},
    {{750, 6074},
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
     MONSTER_ATTRIBUTE_TWO_HEX_ATTACKER},
    {{50, 129}, 26, 8, 2, FACTION_SORCERESS, 4, 4, 2, 1, 2, 0, "sprt", MONSTER_ATTRIBUTE_FLYING},
    {{200, 500}, 25, 6, 20, FACTION_SORCERESS, 2, 6, 5, 2, 4, 0, "dwrf", MONSTER_FLAGS_NONE},
    {{250, 716}, 29, 6, 20, FACTION_SORCERESS, 4, 6, 6, 2, 4, 0, "dwrf", MONSTER_FLAGS_NONE},
    {{250, 554}, 22, 4, 15, FACTION_SORCERESS, 4, 4, 3, 2, 3, 24, "elf_", MONSTER_ATTRIBUTE_RANGED},
    {{300, 658}, 22, 4, 15, FACTION_SORCERESS, 6, 5, 5, 2, 3, 24, "elf_", MONSTER_ATTRIBUTE_RANGED},
    {{350, 1290}, 37, 3, 25, FACTION_SORCERESS, 5, 7, 5, 5, 8, 8, "drui", MONSTER_ATTRIBUTE_RANGED},
    {{400, 1428},
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
     MONSTER_ATTRIBUTE_RANGED},
    {{500, 2702}, 54, 2, 40, FACTION_SORCERESS, 5, 10, 9, 7, 14, 0, "unic", MONSTER_ATTRIBUTE_WIDE},
    {{1500, 10114},
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
     MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_TWO_HEX_ATTACKER},
    {{60, 154},
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
     MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_RANGED},
    {{200, 579}, 29, 6, 15, FACTION_WARLOCK, 6, 4, 7, 2, 3, 0, "garg", MONSTER_ATTRIBUTE_FLYING},
    {{300, 1101},
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
     MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING},
    {{400, 1751}, 44, 3, 35, FACTION_WARLOCK, 4, 9, 8, 5, 10, 0, "mino", MONSTER_FLAGS_NONE},
    {{500, 2252}, 45, 3, 45, FACTION_WARLOCK, 6, 9, 8, 5, 10, 0, "mino", MONSTER_FLAGS_NONE},
    {{800, 2878}, 36, 2, 75, FACTION_WARLOCK, 2, 8, 9, 6, 12, 0, "hydr", MONSTER_ATTRIBUTE_WIDE},
    {{3000, 18153},
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
     MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_TWO_HEX_ATTACKER},
    {{3500, 22962},
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
     MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_TWO_HEX_ATTACKER},
    {{4000, 28144},
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
     MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_TWO_HEX_ATTACKER},
    {{50, 134}, 27, 8, 3, FACTION_WIZARD, 3, 2, 1, 1, 3, 12, "half", MONSTER_ATTRIBUTE_RANGED},
    {{150, 493}, 33, 6, 15, FACTION_WIZARD, 6, 5, 4, 2, 3, 0, "boar", MONSTER_ATTRIBUTE_WIDE},
    {{300, 951}, 19, 4, 30, FACTION_WIZARD, 2, 5, 10, 4, 5, 0, "golm", MONSTER_FLAGS_NONE},
    {{350, 1324}, 24, 4, 35, FACTION_WIZARD, 3, 7, 10, 4, 5, 0, "golm", MONSTER_FLAGS_NONE},
    {{400, 1739},
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
     MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING},
    {{600, 1935}, 32, 2, 30, FACTION_WIZARD, 5, 11, 7, 7, 9, 12, "mage", MONSTER_ATTRIBUTE_RANGED},
    {{700, 2469}, 35, 2, 35, FACTION_WIZARD, 6, 12, 8, 7, 9, 24, "mage", MONSTER_ATTRIBUTE_RANGED},
    {{2000, 9589}, 42, 1, 150, FACTION_WIZARD, 4, 13, 10, 20, 30, 0, "titn", MONSTER_FLAGS_NONE},
    {{5000, 22933},
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
     MONSTER_ATTRIBUTE_RANGED},
    {{75, 203}, 27, 8, 4, FACTION_NECROMANCER, 4, 4, 3, 2, 3, 0, "skel", MONSTER_ATTRIBUTE_UNDEAD},
    {{150, 310},
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
     MONSTER_ATTRIBUTE_UNDEAD},
    {{200, 506},
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
     MONSTER_ATTRIBUTE_UNDEAD},
    {{250, 868},
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
     MONSTER_ATTRIBUTE_UNDEAD},
    {{300, 1056},
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
     MONSTER_ATTRIBUTE_UNDEAD},
    {{500, 1685},
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
     MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_UNDEAD},
    {{650, 2461},
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
     MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_UNDEAD},
    {{750, 2069},
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
     MONSTER_ATTRIBUTE_RANGED | MONSTER_ATTRIBUTE_UNDEAD},
    {{900, 2625},
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
     MONSTER_ATTRIBUTE_RANGED | MONSTER_ATTRIBUTE_UNDEAD},
    {{1500, 11744},
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
     MONSTER_ATTRIBUTE_WIDE | MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_UNDEAD},
    {{50, 177}, 35, 12, 4, FACTION_NEUTRAL, 5, 6, 1, 1, 2, 0, "rogu", MONSTER_FLAGS_NONE},
    {{200, 805}, 40, 4, 20, FACTION_NEUTRAL, 6, 7, 6, 2, 5, 0, "nmad", MONSTER_ATTRIBUTE_WIDE},
    {{1000, 1545},
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
     MONSTER_ATTRIBUTE_FLYING | MONSTER_ATTRIBUTE_UNDEAD},
    {{650, 5692},
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
     MONSTER_ATTRIBUTE_FLYING},
    {{500, 1979}, 40, 5, 35, FACTION_NEUTRAL, 4, 8, 9, 6, 10, 0, "meds", MONSTER_ATTRIBUTE_WIDE},
    {{500, 1732}, 35, 3, 50, FACTION_NEUTRAL, 3, 8, 8, 4, 5, 0, "eelm", MONSTER_FLAGS_NONE},
    {{500, 1412}, 28, 3, 35, FACTION_NEUTRAL, 6, 7, 7, 2, 8, 0, "aelm", MONSTER_FLAGS_NONE},
    {{500, 1501}, 30, 3, 40, FACTION_NEUTRAL, 5, 8, 6, 4, 6, 0, "felm", MONSTER_FLAGS_NONE},
    {{500, 1690}, 34, 3, 45, FACTION_NEUTRAL, 4, 6, 8, 3, 7, 0, "welm", MONSTER_FLAGS_NONE}
};
float gfStatPower[KB_STAT_POWER_COUNT] = {0.5f,  0.5f,  0.5f,  0.5f,  0.52f, 0.54f, 0.56f,
                                          0.58f, 0.6f,  0.62f, 0.64f, 0.67f, 0.7f,  0.74f,
                                          0.77f, 0.8f,  0.84f, 0.88f, 0.92f, 0.96f, 1.0f,
                                          1.04f, 1.08f, 1.12f, 1.16f, 1.2f,  1.24f, 1.28f,
                                          1.32f, 1.36f, 1.4f,  1.44f, 1.48f, 1.52f, 1.56f,
                                          1.6f,  1.64f, 1.68f, 1.72f, 1.76f, 1.8f};
float gfBattleStat[KB_STAT_POWER_COUNT] = {
    0.2f, 0.2f,  0.2f, 0.2f,  0.2f, 0.25f, 0.3f, 0.35f, 0.4f, 0.45f, 0.5f, 0.55f, 0.6f, 0.65f,
    0.7f, 0.75f, 0.8f, 0.85f, 0.9f, 0.95f, 1.0f, 1.1f,  1.2f, 1.3f,  1.4f, 1.5f,  1.6f, 1.7f,
    1.8f, 1.9f,  2.0f, 2.1f,  2.2f, 2.3f,  2.4f, 2.5f,  2.6f, 2.7f,  2.8f, 2.9f,  3.0f
};
i8 gSpellLimits[KB_SPELL_LIMIT_COUNT] = {3, 3, 2, 2, 1};
float gfSpellCastableCombatMod[KB_SPELL_MOD_COUNT] =
    {0.27f, 0.4f, 0.48f, 0.56f, 0.64f, 0.73f, 0.81f, 0.88f, 0.93f, 0.97f, 1.0f};
float gfSpellCastNumMod[KB_SPELL_MOD_COUNT] =
    {0.0f, 1.0f, 1.75f, 2.35f, 2.85f, 3.35f, 3.7f, 4.0f, 4.26f, 4.5f, 4.7f};
float gfPhilAISpellPowerMod[KB_SPELL_MOD_COUNT] =
    {0.0f, 1.0f, 1.75f, 2.5f, 3.1f, 3.5f, 4.0f, 4.5f, 5.4f, 5.75f, 6.15f};
float gfPhilAIDurationMod[KB_SPELL_MOD_COUNT] =
    {0.0f, 0.4f, 0.65f, 0.8f, 1.0f, 1.16f, 1.3f, 1.43f, 1.54f, 1.64f, 1.74f};
float gfSpellTypeNumMod[KB_QUICK_COMBAT_SPELL_TYPE_COUNT] =
    {1.0f, 0.75f, 0.55f, 0.4f, 0.28f, 0.2f, 0.15f};
b32 gbDrawSavedCursor = false;
b8 gbArrow[NORMAL_DIRECTION_COUNT][NORMAL_DIRECTION_COUNT] = {
    {8, 0, 0, 0, 8, 16, 16, 16},
    {17, 9, 1, 1, 1, 9, 17, 17},
    {18, 18, 10, 2, 2, 2, 10, 18},
    {19, 19, 19, 11, 3, 3, 3, 11},
    {12, 20, 20, 20, 12, 4, 4, 4},
    {5, 13, 21, 21, 21, 13, 5, 5},
    {6, 6, 14, 22, 22, 22, 14, 6},
    {7, 7, 7, 15, 23, 23, 23, 15}
};
u8 giCloudType[KB_CLOUD_MASK_COUNT] = {
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
i16 giScoreMon[(CREATURE_COUNT)][(MONSTER_SCORE_FIELD_COUNT)] = {
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
i16 giScoreCampaignMon[(CREATURE_COUNT)][(MONSTER_SCORE_FIELD_COUNT)] = {
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
i8 townTheme[(TOWN_MUSIC_TABLE_SIZE)] = {
    (TOWN_MUSIC_KNIGHT),
    (TOWN_MUSIC_BARBARIAN),
    (TOWN_MUSIC_WARLOCK),
    (TOWN_MUSIC_WIZARD),
    (TOWN_MUSIC_SORCERESS),
    (TOWN_MUSIC_NECROMANCER),
    (TOWN_MUSIC_NONE),
    (TOWN_MUSIC_NONE)
};
i8 gHeroSkillBonus[(FACTION_COUNT)][KB_HERO_LEVEL_BAND_COUNT][HERO_PRIMARY_STAT_COUNT] = {
    {{35, 45, 10, 10}, {25, 25, 25, 25}},
    {{55, 35, 5, 5}, {25, 25, 25, 25}},
    {{10, 10, 30, 50}, {20, 20, 30, 30}},
    {{10, 10, 50, 30}, {20, 20, 30, 30}},
    {{10, 10, 40, 40}, {20, 20, 30, 30}},
    {{15, 15, 35, 35}, {25, 25, 25, 25}}
};
b32 gbLoadingMonoIcon = false;
i32 giMonoIconSkip = -1;
i32 giScrollX = 0;
i32 giScrollY = 0;
b32 gbNoBorder = false;
b32 gbEnlargeScreenBlit = true;
ConfigExecutable giCurExe = CONFIG_EXECUTABLE_GAME;
b32 gbInDialog = false;
struct SMenuEnableStatus gsMenuEnableStatus[MENU_ENABLE_STATUS_COUNT] = {
    {APP_MENU_NONE, 0, 0, 0},
    {(KBWIN_MENU_SIZE_640_480), 1, 1, 0},
    {(KBWIN_MENU_SIZE_800_600), 1, 1, 0},
    {(KBWIN_MENU_SIZE_1024_768), 1, 1, 0},
    {(KBWIN_MENU_SIZE_1280_1024), 1, 1, 0},
    {(KBWIN_MENU_FULLSCREEN), 1, 1, 0},
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
    {(KBWIN_MENU_HELP), 1, 1, 0},
    {(KBWIN_MENU_ABOUT), 1, 1, 0},
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
b32 gbInSetupDialog = false;
b32 gbMinimized = false;
b32 gbHeroMoving = false;
b32 gbInSmackMgr = false;
i32 glBottomRefresh = 0;
b32 gbBothMachinesWin95 = false;
b32 gbGotFirstHeartbeat = false;
HMENU hmnuDflt = NULL;
HMENU hmnuCmbt = NULL;
HMENU hmnuAdv = NULL;
HMENU hmnuTown = NULL;
char* cMonFilename[(CREATURE_COUNT)] = {
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
b32 gbProcessingCombatAction = false;
RemoteNetworkProtocol iMPNetProtocol = REMOTE_PROTOCOL_NETBIOS;
i32 iLastDiffSendTo = DIFF_SEND_FORCE_WHOLE;
SSpellInfo gsSpellInfo[(SPELL_COUNT)] = {
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
char* cArmyFrameFileNames[(CREATURE_COUNT)] = {
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
u8 gcSpellInfluenceIcons[KB_SPELL_INFLUENCE_MAP_COUNT] = {
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
u8 giSpellInfluenceToSpell[KB_SPELL_INFLUENCE_MAP_COUNT] = {
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
u8 giNumPowFrames[KB_SPELL_EFFECT_COUNT] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 8,  8,
                                            10, 10, 10, 10, 15, 10, 10, 10, 10, 10, 16,
                                            16, 14, 19, 22, 10, 17, 10, 12, 11, 16};
SpellEffectDisplayType giSpellEffectShowType = SPELL_EFFECT_DISPLAY_EFFECT_STATUS;
i8 gcColorToPlayerPos[RADAR_OWNER_COLOR_COUNT] = {0, 1, 2, 3, 4, 5, 0, 0};
char* cCombatBkgNames[KB_COMBAT_BACKGROUND_COUNT] = {
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
struct SCmbtObstacle sCmbtObstacles[KB_COMBAT_OBSTACLE_COUNT] = {
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
i32 gEstatesGoldLevel[(HERO_SKILL_LEVEL_COUNT)] = {0, 100, 250, 500};
float gfSSLogisticsMod[(HERO_SKILL_LEVEL_COUNT)] = {1.0f, 1.1f, 1.2f, 1.3f};
float gfSSNavigationMod[(HERO_SKILL_LEVEL_COUNT)] = {1.0f, 1.33f, 1.66f, 2.0f};
float gfSSArcheryMod[(HERO_SKILL_LEVEL_COUNT)] = {1.0f, 1.1f, 1.25f, 1.5f};
float gfSSAIArcheryMod[(HERO_SKILL_LEVEL_COUNT)] = {1.0f, 1.04f, 1.1f, 1.2f};
i8 giVisRange[(HERO_SKILL_LEVEL_COUNT)] = {4, 5, 6, 7};
u8 gStartingHeroStats[(FACTION_COUNT)][HERO_STARTING_STAT_COUNT] = {
    {2, 2, 1, 1, 1},
    {3, 1, 1, 1, 1},
    {0, 0, 2, 3, 1},
    {0, 0, 3, 2, 1},
    {0, 1, 2, 2, 1},
    {1, 0, 2, 2, 1}
};
i32 giTerrainCost[KB_TERRAIN_TYPE_COUNT][(HERO_SKILL_LEVEL_COUNT)][KB_TERRAIN_STEP_TYPE_COUNT] =
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
u8 bStopOnTrigger[KB_TRIGGER_TYPE_COUNT] = {
    0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1
};
char* gTownPrefixNames[(FACTION_COUNT)] = {
    "twnk",
    "twnb",
    "twns",
    "twnw",
    "twnz",
    "twnn"};
char* gTownObjNames[KB_TOWN_OBJECT_NAME_COUNT] = {
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
i8
gDwellingType[(FACTION_COUNT)][KB_DWELLING_TYPE_COUNT] = {
    {(CREATURE_PEASANT),
     (CREATURE_ARCHER),
     (CREATURE_PIKEMAN),
     (CREATURE_SWORDSMAN),
     (CREATURE_CAVALRY),
     (CREATURE_PALADIN),
     (CREATURE_RANGER),
     (CREATURE_VETERAN_PIKEMAN),
     (CREATURE_MASTER_SWORDSMAN),
     (CREATURE_CHAMPION),
     (CREATURE_CRUSADER),
     ARMY_GROUP_EMPTY_SLOT},
    {(CREATURE_GOBLIN),
     (CREATURE_ORC),
     (CREATURE_WOLF),
     (CREATURE_OGRE),
     (CREATURE_TROLL),
     (CREATURE_CYCLOPS),
     (CREATURE_ORC_CHIEF),
     ARMY_GROUP_EMPTY_SLOT,
     (CREATURE_OGRE_LORD),
     (CREATURE_WAR_TROLL),
     ARMY_GROUP_EMPTY_SLOT,
     ARMY_GROUP_EMPTY_SLOT},
    {(CREATURE_SPRITE),
     (CREATURE_DWARF),
     (CREATURE_ELF),
     (CREATURE_DRUID),
     (CREATURE_UNICORN),
     (CREATURE_PHOENIX),
     (CREATURE_BATTLE_DWARF),
     (CREATURE_GRAND_ELF),
     (CREATURE_GREATER_DRUID),
     ARMY_GROUP_EMPTY_SLOT,
     ARMY_GROUP_EMPTY_SLOT,
     ARMY_GROUP_EMPTY_SLOT},
    {(CREATURE_CENTAUR),
     (CREATURE_GARGOYLE),
     (CREATURE_GRIFFIN),
     (CREATURE_MINOTAUR),
     (CREATURE_HYDRA),
     (CREATURE_GREEN_DRAGON),
     ARMY_GROUP_EMPTY_SLOT,
     ARMY_GROUP_EMPTY_SLOT,
     (CREATURE_MINOTAUR_KING),
     ARMY_GROUP_EMPTY_SLOT,
     (CREATURE_RED_DRAGON),
     (CREATURE_BLACK_DRAGON)},
    {(CREATURE_HALFLING),
     (CREATURE_BOAR),
     (CREATURE_IRON_GOLEM),
     (CREATURE_ROC),
     (CREATURE_MAGE),
     (CREATURE_GIANT),
     ARMY_GROUP_EMPTY_SLOT,
     (CREATURE_STEEL_GOLEM),
     ARMY_GROUP_EMPTY_SLOT,
     (CREATURE_ARCHMAGE),
     (CREATURE_TITAN),
     ARMY_GROUP_EMPTY_SLOT},
    {(CREATURE_SKELETON),
     (CREATURE_ZOMBIE),
     (CREATURE_MUMMY),
     (CREATURE_VAMPIRE),
     (CREATURE_LICH),
     (CREATURE_BONE_DRAGON),
     (CREATURE_MUTANT_ZOMBIE),
     (CREATURE_ROYAL_MUMMY),
     (CREATURE_VAMPIRE_LORD),
     (CREATURE_POWER_LICH),
     ARMY_GROUP_EMPTY_SLOT,
     ARMY_GROUP_EMPTY_SLOT}
};
i32 gMageBuildingCosts[KB_MAGE_GUILD_LEVEL_COUNT][KB_BUILDING_RESOURCE_COUNT] = {
    {0, 0, 0, 0, 0, 0, 0},
    {5, 0, 5, 0, 0, 0, 2000},
    {5, 4, 5, 4, 4, 4, 1000},
    {5, 6, 5, 6, 6, 6, 1000},
    {5, 8, 5, 8, 8, 8, 1000},
    {5, 10, 5, 10, 10, 10, 1000}
};
i32 gSpecialBuildingCosts[(FACTION_COUNT)][KB_BUILDING_RESOURCE_COUNT] = {
    {5, 0, 15, 0, 0, 0, 1500},
    {10, 0, 10, 0, 0, 0, 2000},
    {0, 0, 0, 0, 10, 0, 1500},
    {5, 0, 10, 0, 0, 0, 3000},
    {5, 5, 5, 5, 5, 5, 1500},
    {0, 10, 0, 10, 0, 0, 1000}
};
i32 gNeutralBuildingCosts[KB_BUILDING_NEUTRAL_LIMIT][KB_BUILDING_RESOURCE_COUNT] = {
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
i32 gMageBaseResourceValues[KB_MAGE_GUILD_LEVEL_COUNT] = {0, 4000, 6500, 8500, 10500, 15000};
i32 gNeutralBaseResourceValues[(BUILDING_SLOT_DWELLING_FIRST)] = {
    5000, 300, 350, 2000, 3000, 0, 12000, 2500, 1500, 1500, 200, 1000, 500, 0, 0, 1100, 0, 0, 0
};
i32 gSpecialBuildingBaseResourceValues[(FACTION_COUNT)] = {1500, 1000, 1000, 4500, 3500, 1000};
i32 gDwellingBaseResourceValues[(FACTION_COUNT)][KB_DWELLING_TYPE_COUNT] = {
    {858, 2225, 2816, 7385, 13754, 29785, 4000, 3200, 8000, 16000, 40000, 0},
    {1802, 2615, 3414, 6967, 13212, 38141, 3500, 0, 8000, 16000, 0, 0},
    {1684, 3000, 3500, 7213, 15181, 27684, 4000, 4000, 12000, 0, 0, 0},
    {1956, 2607, 3869, 7510, 16002, 90000, 0, 0, 8500, 0, 120000, 180000},
    {1700, 3500, 2800, 9000, 11500, 85000, 0, 3500, 0, 15000, 155000, 0},
    {2200, 2100, 3800, 6000, 9500, 90000, 3000, 4900, 15000, 12000, 0, 0}
};
i32 gDwellingCosts[(FACTION_COUNT)][KB_DWELLING_TYPE_COUNT][KB_BUILDING_RESOURCE_COUNT] = {
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
u32l gHierarchyMask[(FACTION_COUNT)][KB_DWELLING_TYPE_COUNT] = {
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
i32 giDebugBuildingToBuild = -1;
u8 giTerrainToMusicTrack[(TERRAIN_COUNT)] = {16, 18, 14, 15, 11, 13, 17, 12, 16};
char* cHeroTypeShortName[(FACTION_COUNT)] = {
    "kngt",
    "barb",
    "sorc",
    "wrlk",
    "wzrd",
    "necr"};
char cHeroTypeInitial[HERO_TYPE_INITIAL_COUNT] = {'k', 'b', 's', 'w', 'z', 'n'};
i32 giDeferObjDrawX = -1;
i32 giDeferObjDrawY = -1;
class heroWindow* gpInitWin = NULL;
u8 iGetSSByAlignment[(HERO_SKILL_COUNT)][(FACTION_COUNT)] = {
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
struct SCmbtHero sCmbtHero[KB_COMBAT_HERO_SPRITE_COUNT] = {
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
u8 iWallToHexCell[KB_CASTLE_WALL_SEGMENT_COUNT] = {9, 34, 86, 113};
u8 iTowerToHexCell[KB_CASTLE_TOWER_COUNT] = {22, 47, 73, 100};
u16 wallPos[KB_CASTLE_WALL_SEGMENT_COUNT][(COORDINATE_AXIS_COUNT)] =
    {{468, 58}, {421, 128}, {417, 291}, {498, 402}};
u16 towerPos[KB_CASTLE_TOWER_COUNT][(COORDINATE_AXIS_COUNT)] =
    {{441, 61}, {398, 147}, {398, 233}, {441, 316}};
u16 doorPos[KB_CASTLE_DOOR_POSITION_COUNT][(COORDINATE_AXIS_COUNT)] = {{393, 192}, {348, 262}};
float fTradingPostEfficency[KB_TRADING_POST_EFFICIENCY_COUNT] =
    {0.0f, 0.1f, 0.15f, 0.2f, 0.25f, 0.3f, 0.35f, 0.4f, 0.45f, 0.5f, 0.5f};
struct SElevationOverlay sElevationOverlay[ELEVATION_OVERLAY_COUNT] = {
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
i8 captainStats[(FACTION_COUNT)][HERO_PRIMARY_STAT_COUNT] =
    {{1, 1, 1, 1}, {1, 1, 1, 1}, {0, 0, 2, 2}, {0, 0, 2, 2}, {0, 0, 2, 2}, {0, 0, 2, 2}};
b32 gbDrawingPuzzle = false;
i32 giWalkingFrom = -1;
i32 giWalkingFrom2 = -1;
i32 giWalkingTo = -1;
i32 giWalkingTo2 = -1;
i32 giWalkingYMod = 0;
u8 moatCell[KB_MOAT_CELL_COUNT] = {8, 21, 33, 46, 58, 72, 85, 99, 112};
SCampaignChoice
    campaignChoices[(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT][CAMPAIGN_BONUS_CHOICE_COUNT] = {
        {{{CAMPAIGN_CHOICE_RESOURCE, (RES_GOLD), CHOICE_GOLD_BONUS},
          {CAMPAIGN_CHOICE_ARTIFACT, (ARTIFACT_THUNDER_MACE), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, (ARTIFACT_ARMORED_GAUNTLETS), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_WIZARD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_SORCERESS), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_KNIGHT), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_WIZARD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_SORCERESS), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_KNIGHT), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_WIZARD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_SORCERESS), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_KNIGHT), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_WIZARD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_SORCERESS), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_KNIGHT), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_SPELL, (SPELL_MIRROR_IMAGE), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_SPELL, (SPELL_SUMMON_EARTH_ELEMENTAL), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_SPELL, (SPELL_RESURRECT), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ARTIFACT, (ARTIFACT_BLACK_PEARL), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, (ARTIFACT_DRAGON_SWORD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, (ARTIFACT_DIVINE_BREASTPLATE), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_WIZARD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_SORCERESS), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_KNIGHT), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_RESOURCE, (RES_CRYSTAL), CHOICE_RESOURCE_BONUS},
          {CAMPAIGN_CHOICE_RESOURCE, (RES_GEMS), CHOICE_RESOURCE_BONUS},
          {CAMPAIGN_CHOICE_RESOURCE, (RES_MERCURY), CHOICE_RESOURCE_BONUS}},
         {{CAMPAIGN_CHOICE_ARTIFACT, (ARTIFACT_TAX_LIEN), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, (ARTIFACT_HIDEOUS_MASK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, (ARTIFACT_FIZBIN_OF_MISFORTUNE), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_NONE, CHOICE_VALUE_NONE, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_NONE, CHOICE_VALUE_NONE, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_NONE, CHOICE_VALUE_NONE, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_WIZARD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_SORCERESS), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_KNIGHT), CHOICE_NO_AMOUNT}}},
        {{{CAMPAIGN_CHOICE_RESOURCE, (RES_GOLD), CHOICE_GOLD_BONUS},
          {CAMPAIGN_CHOICE_ARTIFACT, (ARTIFACT_MAGE_RING), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, (ARTIFACT_MINOR_SCROLL), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_BARBARIAN), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_RESOURCE, (RES_GOLD), CHOICE_GOLD_BONUS},
          {CAMPAIGN_CHOICE_SPELL, (SPELL_MASS_CURSE), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, (ARTIFACT_DEFENDER_HELM), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_BARBARIAN), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_BARBARIAN), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_BARBARIAN), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_BARBARIAN), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_SECONDARY_SKILL, (HERO_SKILL_LOGISTICS), CHOICE_BASIC_SKILL},
          {CAMPAIGN_CHOICE_ARTIFACT, (ARTIFACT_POWER_AXE), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, (ARTIFACT_WHITE_PEARL), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_BARBARIAN), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ARTIFACT, (ARTIFACT_BLACK_PEARL), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, (ARTIFACT_DRAGON_SWORD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, (ARTIFACT_DIVINE_BREASTPLATE), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ARTIFACT, (ARTIFACT_TAX_LIEN), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, (ARTIFACT_HIDEOUS_MASK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, (ARTIFACT_FIZBIN_OF_MISFORTUNE), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, (FACTION_BARBARIAN), CHOICE_NO_AMOUNT}}}
};
char* congratsText = NULL;
char* gArtifactNames[(ARTIFACT_COUNT)] = {
    "Книга\x20всезнания"  ,
    "Меч\x20власти"  ,
    "Защитная\x20накидка"  ,
    "Жезл\x20магии"  ,
    "Всемогущий\x20щит"  ,
    "Всемогущий\x20посох"  ,
    "Корона\x20всевластия"  ,
    "Золотой\x20гусь"  ,
    "Ожерелье\x20тайной\x20магии"  ,
    "Магический\x20браслет"  ,
    "Кольцо\x20мага"  ,
    "Брошь\x20ведьмы"  ,
    "Медаль\x20отваги"  ,
    "Медаль\x20мужества"  ,
    "Медаль\x20доблести"  ,
    "Медаль\x20почета"  ,
    "Символ\x20неудачи"  ,
    "Громовая\x20палица"  ,
    "Защитная\x20перчатка"  ,
    "Шлем\x20защитника"  ,
    "Гигантский\x20цеп"  ,
    "Баллиста"  ,
    "Незримый\x20щит"  ,
    "Драконий\x20меч"  ,
    "Топор\x20власти"  ,
    "Божественный\x20доспех"  ,
    "Малый\x20свиток\x20знания"  ,
    "Большой\x20свиток\x20знания"  ,
    "Могущественный\x20свиток\x20знания"  ,
    "Свиток\x20высшего\x20знания"  ,
    "Бездонный\x20мешок"  ,
    "Бездонная\x20сума"  ,
    "Бездонный\x20кошель"  ,
    "Башмаки\x20кочевника"  ,
    "Башмаки\x20путника"  ,
    "Лапка\x20кролика"  ,
    "Золотая\x20подкова"  ,
    "Счастливая\x20монета"  ,
    "Клевер"  ,
    "Компас"  ,
    "Астролябия"  ,
    "Дурной\x20глаз"  ,
    "Зачарованные\x20часы"  ,
    "Золотые\x20часы"  ,
    "Шапочка"  ,
    "Ледяная\x20накидка"  ,
    "Огненная\x20накидка"  ,
    "Громовой\x20шлем"  ,
    "Нетающий\x20лед"  ,
    "Горячий\x20камень"  ,
    "Жезл\x20молний"  ,
    "Кольцо\x20змеи"  ,
    "Символ\x20жизни"  ,
    "Книга\x20стихий"  ,
    "Кольцо\x20стихий"  ,
    "Святой\x20кулон"  ,
    "Подвеска\x20свободной\x20воли"  ,
    "Кулон\x20жизни"  ,
    "Подвеска\x20покоя"  ,
    "Всевидящий\x20глаз"  ,
    "Кулон\x20движения"  ,
    "Кулон\x20смерти"  ,
    "Посох\x20отрицания"  ,
    "Золотой\x20лук"  ,
    "Телескоп"  ,
    "Перо\x20дипломата"  ,
    "Шляпа\x20мага"  ,
    "Кольцо\x20силы"  ,
    "Обоз"  ,
    "Подать"  ,
    "Ужасная\x20маска"  ,
    "Бездонная\x20сума\x20серы"  ,
    "Бездонная\x20колба\x20ртути"  ,
    "Бездонная\x20сума\x20самоцветов"  ,
    "Нескончаемая\x20вязанка\x20дров"  ,
    "Бездонная\x20вагонетка\x20руды"  ,
    "Бездонная\x20сума\x20кристаллов"  ,
    "Шлем\x20с\x20шипами"  ,
    "Щит\x20с\x20шипами"  ,
    "Белая\x20жемчужина"  ,
    "Черная\x20жемчужина"  ,
    "Волшебная\x20книга"  ,
    "\x45\x52\x52\x4f\x52\x20\x3a\x20\x41\x72\x74\x69\x66\x61\x63\x74\x20\x38\x32"  ,
    "\x45\x52\x52\x4f\x52\x20\x3a\x20\x41\x72\x74\x69\x66\x61\x63\x74\x20\x38\x33"  ,
    "\x45\x52\x52\x4f\x52\x20\x3a\x20\x41\x72\x74\x69\x66\x61\x63\x74\x20\x38\x34"  ,
    "\x45\x52\x52\x4f\x52\x20\x3a\x20\x41\x72\x74\x69\x66\x61\x63\x74\x20\x38\x35"  ,
    "Свиток\x20заклинаний"  ,
    "Рука\x20мученика"  ,
    "Доспех\x20Андурана"  ,
    "Защитная\x20брошь"  ,
    "Боевое\x20одеяние\x20Андурана"  ,
    "Кристальный\x20шар"  ,
    "Сердце\x20огня"  ,
    "Ледяное\x20сердце"  ,
    "Шлем\x20Андурана"  ,
    "Святой\x20молот"  ,
    "Легендарный\x20скипетр"  ,
    "Наконечник\x20мачты"  ,
    "Сфера\x20антимагии"  ,
    "Волшебный\x20посох"  ,
    "Мечелом"  ,
    "Меч\x20Андурана"  ,
    "Лопата\x20могильщика"
};
char* gArtifactDesc[(ARTIFACT_COUNT)] = {
    "\x7bКнига\x20всезнания\x7d\x0a\x28Знания\x20\x2b\x31\x32\x29\x0a\x0aКнига\x20всезнания\x20увеличивает\x20Знания\x20на\x20\x31\x32\x20единиц\x2e"  ,
    "\x7bМеч\x20власти\x7d\x0a\x28Атака\x20\x2b\x31\x32\x29\x0a\x0aМеч\x20власти\x20увеличивает\x20навык\x20Атаки\x20на\x20\x31\x32\x20единиц\x2e"  ,
    "\x7bЗащитная\x20накидка\x7d\x0a\x28Защита\x20\x2b\x31\x32\x29\x0a\x0aЗащитная\x20накидка\x20увеличивает\x20Защиту\x20на\x20\x31\x32\x20единиц\x2e"  ,
    "\x7bЖезл\x20магии\x7d\x0a\x28Сила\x20магии\x20\x2b\x31\x32\x29\x0a\x0aЖезл\x20магии\x20увеличивает\x20Силу\x20заклинаний\x20на\x20\x31\x32\x20единиц\x2e"  ,
    "\x7bВсемогущий\x20щит\x7d\x0a\x0aВсемогущий\x20щит\x20увеличивает\x20Атаку\x20и\x20Защиту\x20на\x20\x36\x20единиц\x20каждый\x2e"  ,
    "\x7bВсемогущий\x20посох\x7d\x0a\x0aВсемогущий\x20посох\x20увеличивает\x20Силу\x20магии\x20и\x20Знания\x20на\x20\x36\x20единиц\x20каждый\x2e"  ,
    "\x7bКорона\x20всевластия\x7d\x0a\x0aКорона\x20всевластия\x20увеличивает\x20каждый\x20из\x20базовых\x20навыков\x20на\x20\x34\x20единицы\x2e"  ,
    "\x7bЗолотой\x20гусь\x7d\x0a\x0aЗолотой\x20гусь\x20приносит\x20в\x20вашу\x20казну\x20по\x20\x31\x30\x2e\x30\x30\x30\x20золотых\x20каждый\x20день\x2e"  ,
    "\x7bОжерелье\x20тайной\x20магии\x7d\x0a\x28Сила\x20магии\x20\x2b\x34\x29\x0a\x0aОжерелье\x20тайной\x20магии\x20увеличивает\x20Силу\x20магии\x20на\x20\x34\x20единицы\x2e"  ,
    "\x7bМагический\x20браслет\x7d\x0a\x28Сила\x20магии\x20\x2b\x32\x29\x0a\x0aМагический\x20браслет\x20увеличивает\x20Силу\x20магии\x20на\x20\x32\x20единицы\x2e"  ,
    "\x7bКольцо\x20мага\x7d\x0a\x28Сила\x20магии\x20\x2b\x32\x29\x0a\x0aКольцо\x20мага\x20увеличивает\x20Силу\x20магии\x20на\x20\x32\x20единицы\x2e"  ,
    "\x7bБрошь\x20ведьмы\x7d\x0a\x28Сила\x20магии\x20\x2b\x33\x29\x0a\x0aБрошь\x20ведьмы\x20увеличивает\x20Силу\x20магии\x20на\x20\x33\x20единицы\x2e"  ,
    "\x7bМедаль\x20отваги\x7d\x0a\x0aМедаль\x20отваги\x20увеличивает\x20мораль\x2e"  ,
    "\x7bМедаль\x20мужества\x7d\x0a\x0aМедаль\x20мужества\x20увеличивает\x20мораль\x2e"  ,
    "\x7bМедаль\x20доблести\x7d\x0a\x0aМедаль\x20доблести\x20увеличивает\x20мораль\x2e"  ,
    "\x7bМедаль\x20почета\x7d\x0a\x0aМедаль\x20почета\x20увеличивает\x20мораль\x2e"  ,
    "\x7bСимвол\x20неудачи\x7d\x0a\x0aСимвол\x20неудачи\x20сильно\x20уменьшает\x20мораль\x2e"  ,
    "\x7bГромовая\x20палица\x7d\x0a\x28Атака\x20\x2b\x31\x29\x0a\x0aГромовая\x20палица\x20увеличивает\x20навык\x20Атаки\x20на\x20\x31\x20единицу\x2e"  ,
    "\x7bЗащитная\x20перчатка\x7d\x0a\x28Защита\x20\x2b\x31\x29\x0a\x0aЗащитная\x20перчатка\x20увеличивает\x20навык\x20Защиты\x20на\x20\x31\x20единицу\x2e"  ,
    "\x7bШлем\x20защитника\x7d\x0a\x28Защита\x20\x2b\x31\x29\x0a\x0aШлем\x20защитника\x20увеличивает\x20навык\x20Защиты\x20на\x20\x31\x20единицу\x2e"  ,
    "\x7bГигантский\x20цеп\x7d\x0a\x28Атака\x20\x2b\x31\x29\x0a\x0aГигантский\x20цеп\x20увеличивает\x20навык\x20Атаки\x20на\x20\x31\x20единицу\x2e"  ,
    "\x7bБаллиста\x7d\x0a\x0aБаллиста\x20позволяет\x20вашей\x20катапульте\x20дважды\x20стрелять\x20в\x20один\x20ход\x20боя\x2e"  ,
    "\x7bНезримый\x20щит\x7d\x0a\x28Защита\x20\x2b\x32\x29\x0a\x0aНезримый\x20щит\x20увеличивает\x20навык\x20Защиты\x20на\x20\x32\x20единицы\x2e"  ,
    "\x7bДраконий\x20меч\x7d\x0a\x28Атака\x20\x2b\x33\x29\x0a\x0aДраконий\x20меч\x20увеличивает\x20навык\x20Атаки\x20на\x20\x33\x20единицы\x2e"  ,
    "\x7bТопор\x20власти\x7d\x0a\x28Атака\x20\x2b\x32\x29\x0a\x0aТопор\x20власти\x20увеличивает\x20навык\x20Атаки\x20на\x20\x32\x20единицы\x2e"  ,
    "\x7bБожественный\x20доспех\x7d\x0a\x28Защита\x20\x2b\x33\x29\x0a\x0aБожественный\x20доспех\x20увеличивает\x20навык\x20Защиты\x20на\x20\x33\x20единицы\x2e"  ,
    "\x7bМалый\x20свиток\x20знания\x7d\x0a\x28Знания\x20\x2b\x32\x29\x0a\x0aМалый\x20свиток\x20знания\x20увеличивает\x20Знания\x20на\x20\x32\x20единицы\x2e"  ,
    "\x7bБольшой\x20свиток\x20знания\x7d\x0a\x28Знания\x20\x2b\x33\x29\x0a\x0aБольшой\x20свиток\x20знания\x20увеличивает\x20Знания\x20на\x20\x33\x20единицы\x2e"  ,
    "\x7bМогущественный\x20свиток\x20знания\x7d\x0a\x28Знания\x20\x2b\x34\x29\x0a\x0aМогущественный\x20свиток\x20Знания\x20увеличивает\x20Знания\x20на\x20\x34\x20единицы\x2e"  ,
    "\x7bСвиток\x20высшего\x20знания\x7d\x0a\x28Знания\x20\x2b\x35\x29\x0a\x0aСвиток\x20высшего\x20знания\x20увеличивает\x20Знания\x20на\x20\x35\x20единиц\x2e"  ,
    "\x7bБездонный\x20мешок\x7d\x0a\x0aБездонный\x20мешок\x20приносит\x20вам\x20\x31\x30\x30\x30\x20золотых\x20в\x20день\x2e"  ,
    "\x7bБездонная\x20сума\x7d\x0a\x0aБездонная\x20сума\x20приносит\x20вам\x20\x37\x35\x30\x20золотых\x20в\x20день\x2e"  ,
    "\x7bБездонный\x20кошель\x7d\x0a\x0aБездонный\x20кошель\x20приносит\x20вам\x20\x35\x30\x30\x20золотых\x20в\x20день\x2e"  ,
    "\x7bБашмаки\x20кочевника\x7d\x0a\x0aБашмаки\x20кочевника\x20увеличивают\x20дальность\x20передвижения\x20по\x20суше\x2e"  ,
    "\x7bБашмаки\x20путника\x7d\x0a\x0aБашмаки\x20путника\x20увеличивают\x20подвижность\x20отряда\x20на\x20суше\x2e"  ,
    "\x7bЛапка\x20кролика\x7d\x0a\x0aЛапка\x20кролика\x20увеличивает\x20удачу\x20в\x20бою\x2e"  ,
    "\x7bЗолотая\x20подкова\x7d\x0a\x0aЗолотая\x20подкова\x20увеличивает\x20удачу\x20в\x20бою\x2e"  ,
    "\x7bСчастливая\x20монета\x7d\x0a\x0aСчастливая\x20монета\x20увеличивает\x20удачу\x20в\x20бою\x2e"  ,
    "\x7bКлевер\x7d\x0a\x0aКлевер\x20увеличивает\x20удачу\x20в\x20бою\x2e"  ,
    "\x7bКомпас\x7d\x0a\x0aКомпас\x20увеличивает\x20подвижность\x20отряда\x20на\x20суше\x20и\x20на\x20море\x2e"  ,
    "\x7bАстролябия\x7d\x0a\x0aАстролябия\x20увеличивает\x20подвижность\x20отряда\x20на\x20море\x2e"  ,
    "\x7bДурной\x20глаз\x7d\x0a\x0aАртефакт\x20снижает\x20вполовину\x20количество\x20магической\x20энергии\x2c\x20требуемой\x20на\x20направление\x20заклинаний\x2dпроклятий\x2e"  ,
    "\x7bЗачарованные\x20часы\x7d\x0a\x0aАртефакт\x20продлевает\x20действие\x20всех\x20ваших\x20заклинаний\x20на\x20\x32\x20хода\x2e"  ,
    "\x7bЗолотые\x20часы\x7d\x0a\x0aАртефакт\x20удваивает\x20эффективность\x20использования\x20заклинания\x20гипноза\x2e"  ,
    "\x7bШапочка\x7d\x0a\x0aСнижает\x20вполовину\x20затраты\x20магической\x20энергии\x20на\x20все\x20заклинания\x20влияющие\x20на\x20разум\x2e"  ,
    "\x7bЛедяная\x20накидка\x7d\x0a\x0aСнижает\x20вполовину\x20урон\x2c\x20наносимый\x20вашим\x20воинам\x20заклинаниями\x20холода\x2e"  ,
    "\x7bОгненная\x20накидка\x7d\x0a\x0aСнижает\x20вполовину\x20урон\x2c\x20наносимый\x20вашим\x20воинам\x20заклинаниями\x20огня\x2e"  ,
    "\x7bГромовой\x20шлем\x7d\x0a\x0aСнижает\x20вполовину\x20урон\x2c\x20наносимый\x20вашим\x20воинам\x20заклинаниями\x20молний\x2e"  ,
    "\x7bНетающий\x20лед\x7d\x0a\x0aУвеличивает\x20на\x20\x35\x30\x25\x20урон\x2c\x20наносимый\x20врагу\x20вашими\x20заклинаниями\x20холода\x2e"  ,
    "\x7bГорячий\x20камень\x7d\x0a\x0aУвеличивает\x20на\x20\x35\x30\x25\x20урон\x2c\x20наносимый\x20врагу\x20вашими\x20заклинаниями\x20огня\x2e"  ,
    "\x7bЖезл\x20молний\x7d\x0a\x0aУвеличивает\x20на\x20\x35\x30\x25\x20урон\x2c\x20наносимый\x20врагу\x20вашими\x20заклинаниями\x20молний\x2e"  ,
    "\x7bКольцо\x20змеи\x7d\x0a\x0aСнижает\x20вполовину\x20затраты\x20магической\x20энергии\x20на\x20заклинания\x2dблагословения\x2e"  ,
    "\x7bСимвол\x20жизни\x7d\x0a\x0aУвеличивает\x20вдвое\x20эффективность\x20всех\x20заклинаний\x20связанных\x20с\x20воскрешением\x20и\x20оживлением\x20существ\x2e"  ,
    "\x7bКнига\x20стихий\x7d\x0a\x0aУвеличивает\x20вдвое\x20эффективность\x20всех\x20заклинаний\x2c\x20связанных\x20с\x20призывом\x20существ\x2e"  ,
    "\x7bКольцо\x20стихий\x7d\x0a\x0aСнижает\x20вполовину\x20затраты\x20на\x20все\x20заклинания\x2c\x20связанные\x20с\x20вызовом\x20существ\x2e"  ,
    "\x7bСвятой\x20кулон\x7d\x0a\x0aНаделяет\x20ваших\x20воинов\x20иммунитетом\x20к\x20заклинаниям\x2dпроклятиям\x2e"  ,
    "\x7bПодвеска\x20свободной\x20воли\x7d\x0a\x0aНаделяет\x20ваших\x20воинов\x20иммунитетом\x20к\x20заклинаниям\x2c\x20связанным\x20с\x20гипнозом\x2e"  ,
    "\x7bКулон\x20жизни\x7d\x0a\x0aНаделяет\x20ваших\x20воинов\x20иммунитетом\x20ко\x20всем\x20заклинаниям\x20Смерти\x2e"  ,
    "\x7bПодвеска\x20покоя\x7d\x0a\x0aНаделяет\x20ваших\x20воинов\x20иммунитетом\x20к\x20заклинанию\x20Берсерк\x2e"  ,
    "\x7bВсевидящий\x20глаз\x7d\x0a\x0aНаделяет\x20ваших\x20воинов\x20иммунитетом\x20ко\x20всем\x20заклинаниям\x20ослепления\x2e"  ,
    "\x7bКулон\x20движения\x7d\x0a\x0aНаделяет\x20ваших\x20воинов\x20иммунитетом\x20ко\x20всем\x20парализующим\x20заклинаниям\x2e"  ,
    "\x7bКулон\x20смерти\x7d\x0a\x0aНаделяет\x20ваших\x20воинов\x20иммунитетом\x20ко\x20всем\x20святым\x20заклинаниям\x2e"  ,
    "\x7bПосох\x20отрицания\x7d\x0a\x0aАртефакт\x20защищает\x20ваших\x20воинов\x20от\x20заклинания\x20снятия\x20чар\x2e"  ,
    "\x7bЗолотой\x20лук\x7d\x0a\x0aСнижает\x20вполовину\x20штраф\x20на\x20урон\x20для\x20ваших\x20воинов\x2c\x20стреляющих\x20через\x20препятствия\x20\x28например\x2c\x20стены\x20замка\x29\x2e"  ,
    "\x7bТелескоп\x7d\x0a\x0aУвеличивает\x20радиус\x20обзора\x20странствующего\x20героя\x20на\x20\x31\x20клетку\x2e"  ,
    "\x7bПеро\x20дипломата\x7d\x0a\x0aСнижает\x20стоимость\x20сдачи\x20на\x20\x31\x30\x25\x20от\x20общей\x20стоимости\x20армии\x20вашего\x20героя\x2e"  ,
    "\x7bШляпа\x20мага\x7d\x0a\x0aАртефакт\x20продлевает\x20действие\x20ваших\x20заклинаний\x20на\x20\x31\x30\x20ходов\x21"  ,
    "\x7bКольцо\x20силы\x7d\x0a\x0aАртефакт\x20возвращает\x20герою\x20\x32\x20дополнительных\x20очка\x20магии\x20за\x20ход\x2e"  ,
    "\x7bОбоз\x7d\x0a\x0aОбеспечивает\x20ваших\x20воинов\x2dстрелков\x20нескончаемым\x20запасом\x20стрел\x2e"  ,
    "\x7bПодать\x7d\x0a\x0aАртефакт\x20принуждает\x20вас\x20выплачивать\x20каждый\x20ход\x20\x32\x35\x30\x20золотых\x20налогов\x2e"  ,
    "\x7bУжасная\x20маска\x7d\x0a\x0aЭтот\x20артефакт\x20не\x20позволяет\x20любым\x20воинам\x20и\x20существам\x20вступить\x20в\x20вашу\x20армию\x2e"  ,
    "\x7bБездонная\x20сума\x20серы\x7d\x0a\x0aАртефакт\x20приносит\x20вам\x20\x31\x20единицу\x20серы\x20в\x20день\x2e"  ,
    "\x7bБездонная\x20колба\x20ртути\x7d\x0a\x0aАртефакт\x20приносит\x20вам\x20\x31\x20единицу\x20ртути\x20в\x20день\x2e"  ,
    "\x7bБездонная\x20сума\x20самоцветов\x7d\x0a\x0aАртефакт\x20приносит\x20вам\x20\x31\x20единицу\x20самоцветов\x20в\x20день\x2e"  ,
    "\x7bНескончаемая\x20вязанка\x20дров\x7d\x0a\x0aАртефакт\x20приносит\x20вам\x20\x31\x20единицу\x20древесины\x20в\x20день\x2e"  ,
    "\x7bБездонная\x20вагонетка\x20руды\x7d\x0a\x0aАртефакт\x20приносит\x20вам\x20\x31\x20единицу\x20руды\x20в\x20день\x2e"  ,
    "\x7bБездонная\x20сума\x20кристаллов\x7d\x0a\x0aАртефакт\x20приносит\x20вам\x20\x31\x20единицу\x20кристаллов\x20в\x20день\x2e"  ,
    "\x7bШлем\x20с\x20шипами\x7d\x0a\x0aАртефакт\x20увеличивает\x20параметры\x20Атаки\x20и\x20Защиты\x20на\x20\x31\x20единицу\x20каждый\x2e"  ,
    "\x7bЩит\x20с\x20шипами\x7d\x0a\x0a\x20Артефакт\x20увеличивает\x20параметры\x20Атаки\x20и\x20Защиты\x20на\x20\x32\x20единицы\x20каждый\x2e"  ,
    "\x7bБелая\x20жемчужина\x7d\x0a\x0a\x20Артефакт\x20увеличивает\x20параметры\x20Силы\x20магии\x20и\x20Знания\x20на\x20\x31\x20единицу\x20каждый\x2e"  ,
    "\x7bЧерная\x20жемчужина\x7d\x0a\x0a\x20Артефакт\x20увеличивает\x20параметры\x20Силы\x20магии\x20и\x20Знания\x20на\x20\x32\x20единицы\x20каждый\x2e"  ,
    "\x7bВолшебная\x20книга\x7d\x0a\x0aВолшебная\x20книга\x20позволяет\x20направлять\x20заклинания\x2e"  ,
    "\x7b\x45\x52\x52\x4f\x52\x7d\x0a\x0a\x41\x72\x74\x69\x66\x61\x63\x74\x20\x38\x32\x2e"  ,
    "\x7b\x45\x52\x52\x4f\x52\x7d\x0a\x0a\x41\x72\x74\x69\x66\x61\x63\x74\x20\x38\x33\x2e"  ,
    "\x7b\x45\x52\x52\x4f\x52\x7d\x0a\x0a\x41\x72\x74\x69\x66\x61\x63\x74\x20\x38\x34\x2e"  ,
    "\x7b\x45\x52\x52\x4f\x52\x7d\x0a\x0a\x41\x72\x74\x69\x66\x61\x63\x74\x20\x38\x35\x2e"  ,
    "\x7bСвиток\x20заклинаний\x7d\x0a\x0aЭтот\x20Свиток\x20заклинаний\x20позволяет\x20вам\x20направлять\x20заклинание\x20\x27\x25\x73\x27\x2e"  ,
    "\x7bРука\x20мученика\x7d\x0a\x0aРука\x20мученика\x20увеличивает\x20Силу\x20заклинаний\x20вашего\x20героя\x20на\x20\x33\x20единицы\x2c\x20но\x20дает\x20штраф\x20к\x20морали\x20за\x20присутствия\x20нежити\x20в\x20армии\x2e"  ,
    "\x7bДоспех\x20Андурана\x7d\x0a\x0aУвеличивает\x20Защиту\x20на\x20\x35\x20единиц\x2e"  ,
    "\x7bЗащитная\x20брошь\x7d\x0a\x0aЗащитная\x20брошь\x20снижает\x20на\x20\x35\x30\x20процентов\x20урон\x2c\x20наносимый\x20заклинаниями\x20Армагеддон\x20и\x20Буря\x20Стихий\x2e\x20При\x20этом\x2c\x20артефакт\x20снижает\x20Силу\x20магии\x20на\x20\x32\x20единицы\x2e"  ,
    "\x7bБоевое\x20одеяние\x7d\x0a\x0aБоевое\x20одеяние\x20Андурана\x20сочетает\x20в\x20себе\x20силу\x20трех\x20артефактов\x20Андурана\x2e\x20Также\x2c\x20артефакт\x20повышает\x20до\x20максимума\x20удачу\x20и\x20мораль\x20вашей\x20армии\x20и\x20дает\x20возможность\x20направлять\x20заклинание\x20Портал\x20города\x2e"  ,
    "\x7bКристальный\x20шар\x7d\x0a\x0aКристальный\x20шар\x20дает\x20вам\x20более\x20детальную\x20информацию\x20о\x20монстрах\x2c\x20вражеских\x20героях\x20и\x20том\x2c\x20кто\x20защищает\x20близлежащие\x20от\x20героя\x20замки\x2e"  ,
    "\x7bСердце\x20огня\x7d\x0a\x0aСердце\x20огня\x20снижает\x20на\x20\x35\x30\x20процентов\x20урон\x2c\x20наносимый\x20силами\x20огня\x2c\x20но\x20удваивает\x20урон\x2c\x20наносимый\x20вам\x20холодом\x2e"  ,
    "\x7bЛедяное\x20сердце\x7d\x0a\x0aЛедяное\x20сердце\x20снижает\x20на\x20\x35\x30\x20процентов\x20урон\x2c\x20наносимый\x20силами\x20холода\x2c\x20но\x20удваивает\x20урон\x2c\x20наносимый\x20вам\x20огнем\x2e"  ,
    "\x7bШлем\x20Андурана\x7d\x0a\x0aУвеличивает\x20Силу\x20заклинаний\x20на\x20\x35\x20единиц\x2e"  ,
    "\x7bСвятой\x20молот\x7d\x0a\x0aУвеличивает\x20Атаку\x20на\x20\x35\x20единиц\x2e"  ,
    "\x7bЛегендарный\x20скипетр\x7d\x0a\x0aУвеличивает\x20на\x20\x32\x20все\x20характеристики\x20героя\x2e"  ,
    "\x7bНаконечник\x20мачты\x7d\x0a\x0aВ\x20сражении\x20на\x20море\x20увеличивает\x20удачу\x20и\x20мораль\x20вашей\x20армии\x20на\x20\x31\x20единицу\x2e"  ,
    "\x7bСфера\x20антимагии\x7d\x0a\x0aВ\x20бою\x20артефакт\x20не\x20позволяет\x20обеим\x20сторонам\x20направлять\x20заклинания\x2e"  ,
    "\x7bВолшебный\x20посох\x7d\x0a\x0aУвеличивает\x20Силу\x20заклинаний\x20на\x20\x35\x20единиц\x2e"  ,
    "\x7bМечелом\x7d\x0a\x0aУвеличивает\x20Защиту\x20на\x20\x34\x20единицы\x20и\x20Атаку\x20на\x20\x31\x20единицу\x2e"  ,
    "\x7bМеч\x20Андурана\x7d\x0a\x0aУвеличивает\x20Атаку\x20на\x20\x35\x20единиц\x2e"  ,
    "\x7bЛопата\x20могильщика\x7d\x0a\x0aУвеличивает\x20эффективность\x20использования\x20навыка\x20некромантии\x2e"  };
char* gArtifactEvent[(ARTIFACT_COUNT)] = {
    ""  ,
    ""  ,
    ""  ,
    ""  ,
    ""  ,
    ""  ,
    ""  ,
    ""  ,
    "Вы\x20вызволяете\x20волшебницу\x2c\x20заточенную\x20в\x20проклятой\x20гробнице\x2c\x20и\x20в\x20награду\x20она\x20вручает\x20вам\x20изысканное\x20алмазное\x20ожерелье\x2e"  ,
    "Изучая\x20завалы\x20в\x20заброшенной\x20шахте\x2c\x20вы\x20спасаете\x20артель\x20гномов\x2dстарателей\x2e\x20В\x20знак\x20благодарности\x20их\x20старшина\x20дарит\x20вам\x20золотой\x20браслет\x2e"  ,
    "Вы\x20спешите\x20на\x20звук\x20отчаянного\x20вопля\x20боли\x20и\x20видите\x20кентавра\x2c\x20попавшего\x20в\x20западню\x2e\x20Вы\x20помогаете\x20ему\x20освободиться\x2c\x20и\x20он\x20вручает\x20вам\x20кожаный\x20мешочек\x2e\x20Заглянув\x20внутрь\x2c\x20вы\x20видите\x20ослепительное\x20бриллиантовое\x20кольцо\x2e"  ,
    "Рядом\x20с\x20останками\x20сожженной\x20колдуньи\x20лежит\x20изящная\x20брошь\x20прекрасной\x20работы\x2e\x20Осторожно\x20приблизившись\x20к\x20обугленному\x20трупу\x2c\x20вы\x20забираете\x20брошь\x20себе\x2e"  ,
    "В\x20награду\x20за\x20спасение\x20прекрасной\x20девы\x20от\x20посягательств\x20ненавистного\x20барона\x20королевский\x20герольд\x20вручает\x20вам\x20Медаль\x20отваги\x2e"  ,
    "Вы\x20спасаете\x20маленького\x20мальчика\x20от\x20стаи\x20кровожадных\x20волков\x20и\x20провожаете\x20в\x20имение\x20родителей\x2e\x20Счастливый\x20отец\x20награждает\x20вас\x20Медалью\x20мужества\x2e"  ,
    "Вы\x20вырываете\x20принцессу\x20соседнего\x20королевства\x20из\x20мерзких\x20лап\x20презренных\x20работорговцев\x20и\x20в\x20награду\x20за\x20подвиг\x20получаете\x20Медаль\x20доблести\x2e"  ,
    "Вы\x20избавляете\x20округу\x20от\x20ужасного\x20минотавра\x2c\x20добычей\x20которому\x20служили\x20благородные\x20рыцари\x2c\x20и\x20становитесь\x20кавалером\x20Медали\x20почета\x2e"  ,
    "На\x20обочине\x20пустынной\x20дороги\x20вы\x20находите\x20медаль\x2e\x20Вы\x20подобрали\x20ее\x20и\x20обнаружили\x2c\x20что\x20стали\x20несчастным\x20обладателем\x20Символа\x20неудачи\x2c\x20который\x20понижает\x20боевой\x20дух\x20вашей\x20армии\x2e"  ,
    "Во\x20время\x20жуткой\x20грозы\x20молния\x20бьет\x20в\x20дерево\x2c\x20разнося\x20его\x20на\x20мелкие\x20щепки\x2e\x20Среди\x20обломков\x20вы\x20обнаруживаете\x20таинственную\x20палицу\x2e"  ,
    "Вы\x20повстречали\x20печально\x20известного\x20Черного\x20Рыцаря\x21\x20Ваш\x20поединок\x20заканчивается\x20вничью\x2c\x20и\x20рыцарь\x20в\x20знак\x20уважения\x20дарит\x20вам\x20пару\x20латных\x20перчаток\x2e"  ,
    "Краем\x20глаза\x20вы\x20замечаете\x20золотистый\x20блеск\x20среди\x20пышной\x20зелени\x2e\x20Приглядевшись\x20внимательнее\x2c\x20вы\x20находите\x20под\x20кустами\x20великолепный\x20золотой\x20шлем\x2e"  ,
    "Неуклюжий\x20гигант\x20нанес\x20себе\x20смертельную\x20рану\x20собственным\x20боевым\x20цепом\x2e\x20Вы\x20прекрасно\x20владеете\x20этим\x20оружием\x20и\x20с\x20уверенностью\x20вынимаете\x20цеп\x20из\x20мертвых\x20рук\x20гиганта\x2e"  ,
    "Пробираясь\x20через\x20развалины\x20древней\x20крепости\x2c\x20вы\x20находите\x20орудие\x2c\x20которое\x20превратило\x20ее\x20в\x20руины\x2c\x20удивительную\x20баллисту\x20замысловатой\x20конструкции\x2e"  ,
    "В\x20руках\x20у\x20каменной\x20статуи\x20воина\x20\x2d\x20великолепный\x20серебряный\x20щит\x2e\x20Как\x20только\x20вы\x20забираете\x20щит\x20себе\x2c\x20статуя\x20рассыпается\x20в\x20прах\x2e"  ,
    "Вы\x20пробираетесь\x20узкой\x20тропой\x2c\x20как\x20вдруг\x20ближайший\x20куст\x20загорается\x20ярким\x20пламенем\x2e\x20В\x20огненном\x20смерче\x20появляется\x20прекрасная\x20дама\x2c\x20которая\x20протягивает\x20вам\x20волшебный\x20меч\x2e"  ,
    "Вы\x20видите\x20серебряный\x20топор\x2c\x20вогнанный\x20в\x20землю\x20по\x20самую\x20рукоять\x2e\x20Ваши\x20воины\x20пытаются\x20выдернуть\x20его\x2c\x20но\x20усилия\x20их\x20тщетны\x2e\x20Вам\x20же\x20хватило\x20одного\x20усилия\x20и\x20топор\x20у\x20вас\x20в\x20руках\x21"  ,
    "Шайка\x20разбойников\x20обыскивает\x20тела\x20мертвых\x20воинов\x2e\x20Вы\x20разгоняете\x20мародеров\x20и\x20вдруг\x20замечаете\x2c\x20что\x20в\x20спешке\x20они\x20потеряли\x20великолепный\x20доспех\x2e"  ,
    "Перед\x20вами\x20возникает\x20парящий\x20в\x20воздухе\x20стеклянный\x20ларец\x20со\x20свитком\x20внутри\x2c\x20лежащем\x20на\x20подушке\x20из\x20пурпурного\x20бархата\x2e\x20От\x20прикосновения\x2c\x20крышка\x20ларца\x20открывается\x2c\x20и\x20свиток\x20оказывается\x20у\x20вас\x20в\x20руках\x2e"  ,
    "Вы\x20навещаете\x20местного\x20мудреца\x20и\x20рассказываете\x20о\x20цели\x20вашего\x20путешествия\x2e\x20Он\x20достает\x20из\x20мешка\x20пожелтевший\x20свиток\x20и\x20передает\x20его\x20вам\x2e"  ,
    "Вы\x20стоите\x20перед\x20останками\x20давно\x20умершей\x20жрицы\x20друидов\x2e\x20Пожелтевшие\x20от\x20времени\x20кости\x20проглядывают\x20через\x20прорехи\x20истлевшего\x20одеяния\x2e\x20Пошевелив\x20груду\x20ветоши\x2c\x20вы\x20находите\x20древний\x20свиток\x2e"  ,
    "Груда\x20пожелтевших\x20костей\x20и\x20обрывки\x20истлевшей\x20материи\x20\x2d\x20вот\x20все\x2c\x20что\x20осталось\x20от\x20жрицы\x20друидов\x2e\x20Среди\x20этих\x20останков\x20вы\x20замечаете\x20таинственный\x20свиток\x2e"  ,
    "Маленький\x20лепрекон\x20пританцовывает\x20у\x20волшебного\x20мешка\x2e\x20Завидев\x20вас\x2c\x20он\x20замирает\x20на\x20месте\x2c\x20затем\x20издает\x20возмущенный\x20возглас\x2c\x20топает\x20ножкой\x20и\x20растворяется\x20в\x20воздухе\x2e\x20Вы\x20забираете\x20мешок\x20себе\x2e"  ,
    "Благородная\x20путешественница\x2c\x20отбившаяся\x20от\x20спутников\x2c\x20просит\x20вас\x20о\x20помощи\x2e\x20Проводив\x20ее\x20до\x20дома\x2c\x20вы\x20получаете\x20в\x20награду\x20суму\x2c\x20полную\x20золота\x2e"  ,
    "Однажды\x20вам\x20в\x20руки\x20попадает\x20наполненный\x20золотом\x20кожаный\x20кошель\x2c\x20принадлежавший\x20великому\x20королю\x2c\x20который\x20умел\x20превращать\x20любой\x20предмет\x20в\x20золото\x2e"  ,
    "Бродячий\x20торговец\x20просит\x20вас\x20защитить\x20его\x20от\x20банды\x20гоблинов\x2e\x20В\x20награду\x20он\x20дарит\x20вам\x20пару\x20изящных\x20башмаков\x2c\x20испещренных\x20загадочными\x20древними\x20письменами\x2e"  ,
    "Обнаружив\x20пару\x20замечательных\x20башмаков\x20украшенных\x20бисером\x2c\x20вы\x20благодарите\x20загадочного\x20благодетеля\x20и\x20оставляете\x20их\x20себе\x2e"  ,
    "В\x20уплату\x20за\x20охрану\x20в\x20пути\x20странствующий\x20торговец\x20предлагает\x20вам\x20лапку\x20кролика\x2e\x20По\x20его\x20словам\x2c\x20она\x20принесет\x20вам\x20удачу\x20в\x20бою\x2e"  ,
    "Попавший\x20в\x20ловушку\x20единорог\x20испуганно\x20кричит\x2e\x20Вы\x20успокаиваете\x20его\x20и\x20освобождаете\x20от\x20пут\x2e\x20Всхрапнув\x20и\x20ударив\x20копытом\x2c\x20он\x20уносится\x20прочь\x2e\x20Там\x2c\x20где\x20он\x20только\x20что\x20стоял\x2c\x20осталась\x20лежать\x20золотая\x20подкова\x2e"  ,
    "Вы\x20поймали\x20озорного\x20бесенка\x2c\x20который\x20не\x20давал\x20покоя\x20всей\x20округе\x2e\x20В\x20обмен\x20на\x20свободу\x20он\x20предлагает\x20вам\x20волшебную\x20монету\x2e"  ,
    "Посреди\x20мертвой\x20лощины\x2c\x20заполненной\x20иссохшей\x20растительностью\x2c\x20вы\x2c\x20к\x20своему\x20удивлению\x2c\x20замечете\x20веселый\x20зеленый\x20побег\x20четырехлистного\x20клевера\x2e"  ,
    "Странноватый\x20старикашка\x20утверждает\x2c\x20что\x20он\x20\x2d\x20великий\x20изобретатель\x2c\x20и\x20просит\x20вас\x20испытать\x20его\x20новое\x20творение\x2e\x20Надувшись\x20от\x20важности\x2c\x20он\x20вручает\x20вам\x20компас\x2e"  ,
    "Старый\x20мореход\x20стал\x20добычей\x20людоедов\x2e\x20Вы\x20спасаете\x20его\x2c\x20и\x20в\x20знак\x20благодарности\x20он\x20дарит\x20вам\x20чудесный\x20инструмент\x2c\x20позволяющий\x20измерять\x20расстояния\x20по\x20звездам\x2e"  ,
    "В\x20заброшенной\x20хижине\x20вы\x20находите\x20скелет\x20давно\x20почившей\x20колдуньи\x2e\x20Приглядевшись\x2c\x20вы\x20замечаете\x2c\x20что\x20в\x20глазнице\x20пожелтевшего\x20черепа\x20зловеще\x20вращается\x20стеклянный\x20глаз\x2e"  ,
    "За\x20невысоким\x20холмом\x20перед\x20вами\x20открывается\x20зловещая\x20картина\x20\x2d\x20стаи\x20стервятников\x20пируют\x20на\x20поле\x20недавней\x20битвы\x2e\x20Среди\x20тел\x20поверженных\x20воинов\x20вы\x20находите\x20волшебные\x20песочные\x20часы\x2e"  ,
    "Вы\x20помогаете\x20бродячему\x20торговцу\x20снадобьями\x20вытащить\x20повозку\x20из\x20придорожной\x20канавы\x2e\x20В\x20знак\x20благодарности\x20он\x20вручает\x20вам\x20золотые\x20часы\x2e\x20Он\x20и\x20не\x20подозревал\x2c\x20что\x20часы\x20волшебные\x21"  ,
    "Вы\x20делаете\x20короткую\x20остановку\x20в\x20маленькой\x20придорожной\x20харчевне\x2e\x20Под\x20звон\x20монет\x20происходит\x20обмен\x20новостями\x2c\x20а\x20то\x20и\x20редкими\x20вещицами\x2e\x20Вот\x20таким\x2dто\x20образом\x20в\x20вашем\x20багаже\x20и\x20оказывается\x20волшебная\x20шапочка\x2e"  ,
    "Вы\x20спешите\x20на\x20отчаянные\x20крики\x20и\x20видите\x20очаровательную\x20девушку\x2c\x20за\x20которой\x20гонится\x20разъяренный\x20медведь\x2e\x20Через\x20мгновение\x20зверь\x20повержен\x2c\x20и\x20благодарная\x20волшебница\x20шьет\x20вам\x20из\x20его\x20шкуры\x20волшебный\x20плащ\x2e"  ,
    "За\x20поворотом\x20дороги\x20вы\x20видите\x20сражающихся\x20некроманта\x20и\x20паладина\x2e\x20Некромант\x20атакует\x20паладина\x2c\x20и\x20тот\x20падает\x20на\x20колени\x2e\x20Вы\x20спасаете\x20жизнь\x20паладину\x2c\x20убивая\x20его\x20врага\x2e\x20Паладин\x20дарит\x20вам\x20свою\x20огненную\x20накидку\x2e"  ,
    "Бродячий\x20медник\x2c\x20у\x20которого\x20кончилась\x20провизия\x2c\x20предлагает\x20вам\x20шлем\x20с\x20гребнем\x20в\x20виде\x20молнии\x20в\x20обмен\x20на\x20еду\x20и\x20питье\x2e\x20Вы\x20соглашаетесь\x20на\x20обмен\x2c\x20а\x20вскоре\x20обнаруживаете\x2c\x20что\x20шлем\x20обладает\x20еще\x20и\x20магическими\x20свойствами\x2e"  ,
    "Ваше\x20внимание\x20привлекает\x20ледяная\x20сосулька\x2c\x20которая\x20не\x20тает\x2c\x20несмотря\x20на\x20полуденный\x20зной\x2e\x20Вы\x20отламываете\x20ее\x20от\x20карниза\x20и\x20с\x20удивлением\x20обнаруживаете\x2c\x20что\x20даже\x20тепло\x20ваших\x20рук\x20ей\x20нипочем\x2e"  ,
    "В\x20дальней\x20стране\x20вы\x20встречаете\x20племя\x20приматов\x2e\x20Они\x20разжигают\x20костры\x20при\x20помощи\x20волшебного\x20куска\x20лавы\x2e\x20Вы\x20научили\x20их\x20добывать\x20огонь\x20обычным\x20способом\x2e\x20Обезьяны\x20считают\x20вас\x20богом\x20и\x20дарят\x20свой\x20заветный\x20кусок\x20лавы\x2e"  ,
    "Во\x20время\x20ужасной\x20грозы\x20на\x20ваших\x20глазах\x20в\x20громоотвод\x20дома\x20бьет\x20молния\x2e\x20Расплавленный\x20громоотвод\x20падает\x20на\x20землю\x2c\x20но\x20его\x20наконечник\x20остается\x20целым\x20и\x20невредимым\x2e\x20Вы\x20подобрали\x20его\x20\x2d\x20оказалось\x2c\x20это\x20магический\x20предмет\x21"  ,
    "На\x20пальце\x20мертвого\x20странника\x20вы\x20видите\x20необычное\x20кольцо\x2e\x20Оно\x20имеет\x20форму\x20змеи\x2c\x20вцепившейся\x20зубами\x20в\x20собственный\x20хвост\x2e"  ,
    "Песчаная\x20буря\x20обнажила\x20вход\x20в\x20подземную\x20гробницу\x2e\x20Вы\x20спускаетесь\x20внутрь\x20и\x20обнаруживаете\x2c\x20что\x20здесь\x20уже\x20побывали\x20грабители\x2c\x20однако\x20в\x20темноте\x20они\x20не\x20заметили\x20символ\x20вечной\x20жизни\x2c\x20висящий\x20на\x20серебряной\x20цепи\x2e"  ,
    "Вы\x20встречаете\x20заклинателя\x2c\x20который\x20просит\x20разрешить\x20ему\x20воспользоваться\x20вашим\x20покровительством\x20на\x20опасном\x20участке\x20пути\x2e\x20Вы\x20соглашаетесь\x2c\x20и\x20в\x20награду\x20он\x20дарит\x20вам\x20Книгу\x20Стихий\x2e"  ,
    "Расположившись\x20на\x20отдых\x20под\x20невысоким\x20деревом\x2c\x20вы\x20замечаете\x20дикого\x20кота\x2c\x20который\x20подбирается\x20к\x20вороньему\x20гнезду\x2e\x20Вы\x20прогоняете\x20кота\x2c\x20и\x20сами\x20залезаете\x20на\x20дерево\x2e\x20В\x20гнезде\x20вы\x20находите\x20кольцо\x20тонкой\x20работы\x2e"  ,
    "Странствуя\x20по\x20дальним\x20землям\x2c\x20вы\x20встречаете\x20отшельника\x2c\x20живущего\x20в\x20маленькой\x20аккуратной\x20хижине\x2e\x20Узнав\x20о\x20цели\x20ваших\x20скитаний\x2c\x20он\x20прерывает\x20свои\x20размышления\x2c\x20благословляет\x20вас\x20и\x20дарит\x20амулет\x2c\x20защищающий\x20от\x20злых\x20чар\x2e"  ,
    "Вы\x20слышите\x20крики\x20о\x20помощи\x20и\x2c\x20поспешив\x20на\x20берег\x20реки\x2c\x20видите\x20фей\x2c\x20потешающихся\x20над\x20стариком\x2c\x20окуная\x20его\x20в\x20воду\x2e\x20Вы\x20выручаете\x20старика\x20из\x20беды\x20и\x20вытаскиваете\x20одну\x20фею\x20на\x20берег\x2e\x20В\x20обмен\x20на\x20свободу\x20она\x20отдает\x20вам\x20подвеску\x2e"  ,
    "В\x20дороге\x20вы\x20встречаете\x20небольшой\x20караван\x2e\x20Сыграв\x20с\x20хозяином\x20каравана\x20в\x20кости\x2c\x20вы\x20выигрываете\x20волшебную\x20подвеску\x2e\x20Ее\x20прежний\x20владелец\x20утверждает\x2c\x20что\x20она\x20может\x20противостоять\x20чарам\x20смерти\x20некромантов\x2e"  ,
    "Вы\x20спешите\x20на\x20шум\x20сражения\x20и\x20видите\x20старика\x2dварвара\x2c\x20который\x20с\x20трудом\x20отбивается\x20от\x20гидры\x2e\x20В\x20награду\x20за\x20помощь\x20варвар\x20дарит\x20вам\x20волшебный\x20кулон\x2e"  ,
    "В\x20хижине\x20у\x20дороги\x20вы\x20находите\x20слепую\x20старуху\x2c\x20умирающую\x20в\x20полном\x20одиночестве\x2e\x20Вы\x20обещаете\x20устроить\x20ей\x20достойные\x20похороны\x2e\x20В\x20знак\x20благодарности\x20она\x20дарит\x20вам\x20волшебную\x20подвеску\x2e"  ,
    "Дорогу\x20вам\x20преграждает\x20голем\x2c\x20на\x20шее\x20которого\x20сверкает\x20кулон\x2e\x20Вы\x20перерезаете\x20шнурок\x2c\x20и\x20он\x20падает\x20на\x20землю\x2e\x20Голем\x20рассыпается\x20у\x20вас\x20на\x20глазах\x2c\x20а\x20кулон\x20достается\x20вам\x2e"  ,
    "После\x20короткой\x20ожесточенной\x20схватки\x20с\x20некромантом\x20у\x20вас\x20в\x20руках\x20остается\x20его\x20волшебный\x20кулон\x2e\x20Знакомый\x20чародей\x20объясняет\x20вам\x2c\x20что\x20этот\x20кулон\x20защищает\x20нежить\x2c\x20состоящую\x20в\x20вашей\x20армии\x2c\x20от\x20святого\x20слова\x2e"  ,
    "Навстречу\x20вам\x20попадается\x20старый\x20друг\x2dчародей\x2e\x20Он\x20вручает\x20вам\x20подарок\x20\x2d\x20волшебный\x20жезл\x2c\x20который\x20делает\x20невозможным\x20применение\x20заклинания\x20снятие\x20чар\x20против\x20ваших\x20соратников\x2e"  ,
    "Вы\x20случайно\x20встречаете\x20знаменитого\x20стрелка\x20и\x20предлагаете\x20ему\x20сыграть\x20в\x20кости\x2e\x20Он\x20соглашается\x20и\x20ставит\x20свой\x20лук\x20против\x20вашего\x20коня\x2e\x20Вы\x20выигрываете\x2e"  ,
    "Торговец\x20из\x20далеких\x20земель\x20предлагает\x20вам\x20новейшее\x20изобретение\x20своего\x20народа\x20в\x20обмен\x20на\x20съестные\x20припасы\x2e\x20Эта\x20штука\x2c\x20благодаря\x20которой\x20удаленные\x20предметы\x20кажутся\x20ближе\x2c\x20называется\x20телескопом\x2e"  ,
    "Вы\x20помогаете\x20дипломату\x20починить\x20сломанную\x20ось\x20в\x20его\x20экипаже\x2c\x20и\x20в\x20знак\x20благодарности\x20он\x20дарит\x20вам\x20перо\x2e\x20Он\x20говорит\x2c\x20что\x20это\x20перо\x20заставляет\x20людей\x20смотреть\x20на\x20вещи\x20глазами\x20его\x20обладателя\x2e"  ,
    "Вы\x20видите\x20чародея\x2c\x20который\x20удирает\x20от\x20грифона\x2e\x20Вот\x20он\x20распахнул\x20портал\x20и\x20ринулся\x20внутрь\x2c\x20но\x20при\x20этом\x20зацепился\x20шляпой\x2c\x20и\x20она\x20упала\x20она\x20на\x20землю\x2e\x20Вы\x20поднимаете\x20шляпу\x2c\x20отряхиваете\x20ее\x20от\x20пыли\x20и\x20оставляете\x20себе\x2e"  ,
    "Вы\x20замечаете\x20дерево\x2c\x20похожее\x20на\x20чернокнижника\x20Карнота\x2e\x20На\x20одной\x20из\x20его\x20веток\x20сверкает\x20кольцо\x2e\x20Вы\x20все\x20равно\x20ничем\x20не\x20можете\x20ему\x20помочь\x2c\x20и\x20поэтому\x20забираете\x20кольцо\x20себе\x2e"  ,
    "Ваше\x20внимание\x20привлекает\x20повозка\x20с\x20боеприпасами\x2c\x20стоящая\x20посреди\x20поля\x2c\x20где\x20когда\x2dто\x20гремела\x20битва\x2e\x20Убедившись\x2c\x20что\x20она\x20в\x20хорошем\x20состоянии\x2c\x20вы\x20присоединяете\x20ее\x20к\x20своему\x20обозу\x2e"  ,
    "Ваша\x20налоговая\x20декларация\x20превысила\x20приделы\x2e\x20Мытарь\x20сжалился\x20над\x20вами\x20и\x20согласился\x20ежедневно\x20получать\x20от\x20вас\x20всего\x20по\x20\x32\x35\x30\x20золотых\x2e"  ,
    "Вы\x20вскрыли\x20могилу\x20Синфилия\x20Гардолада\x2c\x20знаменитого\x20чернокнижника\x2c\x20и\x20находите\x20в\x20ней\x20маску\x2e\x20Надев\x20ее\x2c\x20ваше\x20лицо\x20искажает\x20гримаса\x20ужаса\x2e\x20Видимо\x20вам\x20достался\x20маска\x20Громлака\x20Грина\x2e\x20Теперь\x20от\x20нее\x20не\x20избавиться\x21"  ,
    "Вы\x20посещаете\x20алхимика\x2c\x20который\x20при\x20виде\x20вашей\x20армии\x20незамедлительно\x20признает\x20вас\x20достойнейшим\x20из\x20достойных\x2e\x20Новый\x20подданный\x20дарит\x20вам\x20бездонную\x20сумку\x20серы\x2c\x20которая\x20вам\x20очень\x20даже\x20пригодится\x2e"  ,
    "Вы\x20делаете\x20короткий\x20привал\x20в\x20башне\x20чародея\x2c\x20покинутой\x20хозяином\x2c\x20и\x20находите\x20волшебный\x20сосуд\x20с\x20ртутью\x2c\x20содержимое\x20которого\x20никогда\x20не\x20кончается\x2e\x20Это\x20же\x20настоящее\x20сокровище\x21"  ,
    "После\x20короткого\x20ливня\x20на\x20небе\x20появляется\x20радуга\x2e\x20Заметив\x20место\x2c\x20где\x20она\x20упирается\x20в\x20землю\x2c\x20вы\x20находите\x20там\x20горшок\x20золота\x2e\x20Его\x20хозяин\x2c\x20маленький\x20эльф\x2c\x20предлагает\x20взамен\x20бездонную\x20суму\x20самоцветов\x2e"  ,
    "Вы\x20останавливаетесь\x20на\x20отдых\x20и\x20разводите\x20костер\x2e\x20Неподалеку\x20лежит\x20куча\x20дров\x2e\x20Вы\x20берете\x20одно\x20полено\x20за\x20другим\x2c\x20но\x20куча\x20не\x20уменьшается\x2e\x20Вы\x20с\x20радостью\x20понимаете\x2c\x20что\x20дрова\x20зачарованы\x2c\x20и\x20забираете\x20их\x20себе\x2e"  ,
    "Вы\x20находите\x20кузницу\x20гоблинов\x2c\x20где\x20они\x20куют\x20оружие\x2e\x20С\x20воинственным\x20кличем\x2c\x20ваши\x20воины\x20нападают\x20на\x20их\x20лагерь\x20и\x20убивают\x20всех\x20врагов\x2e\x20Осмотрев\x20трофеи\x2c\x20вы\x20обнаруживаете\x20волшебную\x20вагонетку\x20с\x20рудой\x2e"  ,
    "Укрывшись\x20от\x20бури\x20в\x20небольшой\x20пещерке\x2c\x20вы\x20замечаете\x20в\x20углу\x20друзу\x20кристаллов\x2e\x20Вы\x20отламываете\x20кусок\x2c\x20а\x20на\x20его\x20месте\x20вырастает\x20новый\x20кристалл\x2e\x20Вы\x20забераете\x20это\x20сокровище\x20с\x20собой\x2e"  ,
    "Небольшой\x20отряд\x20орков\x20нападает\x20на\x20вашу\x20армию\x2e\x20Вы\x20без\x20труда\x20отбиваете\x20атаку\x2e\x20На\x20теле\x20одного\x20из\x20нападавших\x20вы\x20видите\x20блестящий\x20шлем\x20с\x20шипами\x2e"  ,
    "Вы\x20приближаетесь\x20к\x20мосту\x20через\x20глубокий\x20овраг\x2e\x20Неожиданно\x20из\x2dпод\x20моста\x20появляется\x20тролль\x20и\x20требует\x20плату\x20за\x20проход\x2e\x20После\x20отказа\x2c\x20тролль\x20нападает\x20на\x20вас\x2e\x20Убив\x20его\x2c\x20вы\x20забираете\x20себе\x20его\x20шит\x20с\x20шипами\x2e"  ,
    "Вы\x20пересекаете\x20пересохшее\x20соляное\x20озеро\x2c\x20и\x20вдруг\x20среди\x20обломков\x20ракушек\x20и\x20кусков\x20коралла\x20замечаете\x20великолепную\x20белую\x20жемчужину\x2e"  ,
    "Слухи\x20об\x20огромном\x20грифоне\x2c\x20нагоняющем\x20ужас\x20на\x20всю\x20округу\x2c\x20приводят\x20вас\x20в\x20его\x20логово\x2e\x20Жестокая\x20схватка\x20заканчивается\x20вашей\x20победой\x2c\x20и\x20в\x20опустевшем\x20гнезде\x20вы\x20находите\x20черную\x20жемчужину\x2e"  ,
    ""  ,
    "\x45\x52\x52\x4f\x52\x20\x3a\x20\x41\x72\x74\x69\x66\x61\x63\x74\x20\x65\x76\x65\x6e\x74\x20\x38\x32\x2e"  ,
    "\x45\x52\x52\x4f\x52\x20\x3a\x20\x41\x72\x74\x69\x66\x61\x63\x74\x20\x65\x76\x65\x6e\x74\x20\x38\x33\x2e"  ,
    "\x45\x52\x52\x4f\x52\x20\x3a\x20\x41\x72\x74\x69\x66\x61\x63\x74\x20\x65\x76\x65\x6e\x74\x20\x38\x34\x2e"  ,
    "\x45\x52\x52\x4f\x52\x20\x3a\x20\x41\x72\x74\x69\x66\x61\x63\x74\x20\x65\x76\x65\x6e\x74\x20\x38\x35\x2e"  ,
    "Вы\x20нашли\x20резной\x20ларец\x2c\x20в\x20котором\x20хранился\x20древний\x20свиток\x2e\x20Руны\x20на\x20ларце\x20очень\x20древние\x2e\x20Развернув\x20свиток\x2c\x20вы\x20почувствовали\x20пульсацию\x20магических\x20сил\x2e"  ,
    "Один\x20из\x20ваших\x20воинов\x20подобрал\x20с\x20земли\x20оторванную\x20руку\x2e\x20Несмотря\x20на\x20то\x2c\x20что\x20рука\x20была\x20оторвана\x20от\x20тела\x2c\x20она\x20все\x20еще\x20продолжала\x20шевелиться\x2e\x20Ваши\x20воины\x20испытали\x20великое\x20отвращение\x20к\x20этому\x20предмету\x2c\x20но\x20вы\x20не\x20смогли\x20заставить\x20себя\x20выкинуть\x20ее\x2e"  ,
    "Вы\x20обнаружили\x20указатель\x2c\x20на\x20котором\x20было\x20написано\x2c\x20что\x20здесь\x20покоится\x20великий\x20Андуран\x2e\x20Надпись\x20молвила\x2c\x20что\x20преклонивший\x20чело\x20перед\x20могилой\x20будет\x20вознагражден\x2e\x20Вы\x20поступили\x2c\x20как\x20того\x20требовалось\x2c\x20и\x20получили\x20в\x20награду\x20волшебный\x20доспех\x2e"  ,
    "Добрая\x20колдунья\x20сочла\x2c\x20что\x20ваша\x20армия\x20плохо\x20защищена\x20и\x20даровала\x20вам\x20свою\x20волшебную\x20брошь\x2e"  ,
    "Вы\x20купили\x20у\x20бедняка\x20ящик\x20со\x20всяким\x20барахлом\x20и\x20на\x20свое\x20удивление\x20нашли\x20в\x20нем\x20три\x20вещи\x20из\x20боевого\x20одеяния\x20Андурана\x21\x20Вот\x20это\x20удача\x21"  ,
    "Вы\x20проходили\x20мимо\x20труппы\x20бродячих\x20актеров\x2e\x20Они\x20попросили\x20вас\x20станцевать\x20рума\x2dбуту\x2e\x20Вы\x20исполнили\x20несколько\x20произвольных\x20движений\x2c\x20и\x20они\x20за\x20храбрость\x20даровали\x20вам\x20кристальный\x20шар\x2e"  ,
    "Вы\x20попали\x20на\x20недавно\x20сгоревшую\x20поляну\x2e\x20Посреди\x20поляны\x2c\x20на\x20камне\x20стоял\x20сосуд\x2c\x20в\x20котором\x20сидел\x20огненный\x20элементал\x2e\x20Вы\x20решили\x20взять\x20с\x20собой\x20эту\x20диковинную\x20находку\x2e"  ,
    "Неожиданно\x20вас\x20сковал\x20пронзительный\x20холод\x2e\x20От\x20неожиданного\x20шока\x20вы\x20упали\x20с\x20коня\x20на\x20землю\x2e\x20Мимо\x20вас\x20промчался\x20огромный\x20ледяной\x20гигант\x2e\x20В\x20спешке\x20он\x20обронил\x20одну\x20ценную\x20вещь\x21"  ,
    "Вы\x20заметили\x20сверкающий\x20объект\x20невдалеке\x2e\x20Вы\x20послали\x20одного\x20из\x20ваших\x20воинов\x20посмотреть\x2c\x20что\x20это\x20там\x2e\x20Он\x20вернулся\x20с\x20золотым\x20шлемом\x20в\x20руках\x2c\x20который\x20оказался\x20ни\x20чем\x20иным\x2c\x20как\x20шлемом\x20легендарного\x20Андурана\x21"  ,
    "Вы\x20стали\x20свидетелем\x20поединка\x2c\x20в\x20котором\x20паладин\x20был\x20смертельно\x20ранен\x20отрядом\x20зомби\x2e\x20Он\x20попросил\x20вас\x20взять\x20его\x20молот\x20и\x20завершить\x20начатое\x20им\x20дело\x2e\x20Убив\x20зомби\x20вы\x20повесили\x20молот\x20на\x20свой\x20пояс\x20и\x20удалились\x2e"  ,
    "Минуя\x20небольшой\x20холм\x2c\x20вы\x20увидели\x2c\x20как\x20маленькая\x20фея\x20тащит\x20огромный\x20скипетр\x2e\x20Улыбнувшись\x2c\x20вы\x20спросили\x2c\x20не\x20нужна\x20ли\x20ей\x20помощь\x2e\x20Фея\x20обиженно\x20спросила\x2c\x20мол\x2c\x20думаешь\x2c\x20это\x20смешно\x3f\x20Вспорхнула\x20и\x20улетела\x2c\x20а\x20скипетр\x20остался\x20вам\x2e"  ,
    "Старый\x20моряк\x20рассказал\x20вам\x2c\x20что\x20в\x20былые\x20времена\x2c\x20на\x20его\x20ботике\x20стояла\x20мачта\x2c\x20приносящая\x20ему\x20удачу\x2e\x20Он\x20бал\x20вам\x20схему\x2c\x20где\x20ее\x20можно\x20будет\x20найти\x2e\x20Через\x20несколько\x20часов\x20поиска\x2c\x20вы\x20нашли\x20мачту\x20в\x20старом\x20доке\x2e"  ,
    "На\x20вас\x20налетел\x20торопыга\x2dкрестьянин\x2e\x20Он\x20хотел\x20убежать\x2c\x20но\x20вы\x20остановили\x20его\x2e\x20Извинившись\x2c\x20крестьянин\x20вручил\x20вам\x20необычную\x20сферу\x2e\x20Едва\x20вы\x20дотронулись\x20до\x20нее\x2c\x20как\x20почувствовали\x2c\x20что\x20сфера\x20втягивает\x20в\x20себя\x20магию\x2e\x2e\x2e"  ,
    "Ваши\x20солдаты\x20нашли\x20необычную\x20вещь\x20и\x20решили\x20принести\x20ее\x20вам\x2e\x20Вы\x20отчистили\x20ее\x20от\x20грязи\x20и\x20смогли\x20прочитать\x20на\x20ней\x20необычные\x20слова\x3a\x20\x22Ум\x20\x2d\x20лучшая\x20сила\x2c\x20а\x20магия\x20сильнее\x20грубой\x20силы\x2e\x20Помни\x20мои\x20слова\x2c\x20и\x20ты\x20всегда\x20будешь\x20побеждать\x2e\x22"  ,
    "Отставной\x20капитан\x20городской\x20стражи\x20узнал\x20о\x20вашем\x20походе\x20и\x20даровал\x20вам\x20свой\x20меч\x2c\x20сослуживший\x20ему\x20добрую\x20службу\x20в\x20былые\x20времена\x2e"  ,
    "Тролль\x20остановил\x20вас\x2c\x20сказав\x3a\x20\x22Плати\x20мне\x20\x35\x30\x30\x30\x20золотых\x20или\x20я\x20убью\x20тебя\x20мечом\x20Анудрана\x21\x22\x20Вы\x20отказались\x20платить\x2e\x20Тролль\x20схватился\x20за\x20клинок\x20меча\x2c\x20взвыл\x20от\x20боли\x20и\x20бросив\x20меч\x20убежал\x2e\x20Хорошо\x2c\x20что\x20он\x20был\x20настолько\x20глуп\x2c\x20что\x20не\x20знал\x2c\x20как\x20правильно\x20держать\x20острые\x20предметы\x2e"  ,
    "В\x20грязи\x20вы\x20подобрали\x20старую\x20лопату\x2e\x20Присмотревшись\x2c\x20вы\x20поняли\x2c\x20что\x20вам\x20посчастливилось\x20найти\x20зачарованную\x20лопату\x20грабителей\x20могил\x2e"  };
char* gStatNames[HERO_PRIMARY_STAT_COUNT] = {
    "Атака"  ,
    "Защита"  ,
    "Сила\x20магии"  ,
    "Знания"
};
char* gStatDesc[HERO_PRIMARY_STAT_COUNT] = {
    "\x7bАтака\x7d\x0a\x0aВаш\x20навык\x20атаки\x20\x2d\x20бонус\x2c\x20добавляемый\x20к\x20навыку\x20атаки\x20каждого\x20воина\x2e"  ,
    "\x7bЗащита\x7d\x0a\x0aВаш\x20навык\x20защиты\x20\x2d\x20бонус\x2c\x20добавляемый\x20к\x20навыку\x20защиты\x20каждого\x20воина\x2e"  ,
    "\x7bСила\x20магии\x7d\x0a\x0aВаш\x20уровень\x20силы\x20магии\x20определяет\x20длительность\x20действия\x20или\x20силу\x20заклинания\x2e"  ,
    "\x7bЗнания\x7d\x0a\x0aУровень\x20знаний\x20определяет\x20количество\x20очков\x20магии\x20героя\x2e"
};
char* gAlignmentNames[KB_ALIGNMENT_NAME_COUNT] = {
    "Рыцарь"  ,
    "Варвар"  ,
    "Колдунья"  ,
    "Чернокнижник"  ,
    "Чародей"  ,
    "Некромант"  ,
    "Мульти"  ,
    "Случайно"
};
char* gArmyShortNames[(CREATURE_COUNT)] = {
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
char* gArmyNames[(CREATURE_COUNT)] = {
    "Крестьянин"  ,
    "Стрелок"  ,
    "Рейнджер"  ,
    "Копейщик"  ,
    "Копейщик\x20ветеран"  ,
    "Мечник"  ,
    "Мечник\x20мастер"  ,
    "Всадник"  ,
    "Чемпион"  ,
    "Паладин"  ,
    "Крестоносец"  ,
    "Гоблин"  ,
    "Орк"  ,
    "Вождь\x20орков"  ,
    "Волк"  ,
    "Огр"  ,
    "Лорд\x20огров"  ,
    "Тролль"  ,
    "Боевой\x20тролль"  ,
    "Циклоп"  ,
    "Фея"  ,
    "Гном"  ,
    "Боевой\x20гном"  ,
    "Эльф"  ,
    "Высокий\x20эльф"  ,
    "Друид"  ,
    "Старший\x20друид"  ,
    "Единорог"  ,
    "Феникс"  ,
    "Кентавр"  ,
    "Горгулья"  ,
    "Грифон"  ,
    "Минотавр"  ,
    "Царь\x20минотавров"  ,
    "Гидра"  ,
    "Зеленый\x20дракон"  ,
    "Красный\x20дракон"  ,
    "Черный\x20дракон"  ,
    "Полурослик"  ,
    "Боров"  ,
    "Железный\x20голем"  ,
    "Стальной\x20голем"  ,
    "Рух"  ,
    "Маг"  ,
    "Архимаг"  ,
    "Гигант"  ,
    "Титан"  ,
    "Скелет"  ,
    "Зомби"  ,
    "Зомби\x20мутант"  ,
    "Мумия"  ,
    "Королевская\x20мумия"  ,
    "Вампир"  ,
    "Лорд\x20вампиров"  ,
    "Лич"  ,
    "Могучий\x20лич"  ,
    "Костяной\x20дракон"  ,
    "Разбойник"  ,
    "Кочевник"  ,
    "Призрак"  ,
    "Джинн"  ,
    "Медуза"  ,
    "Земной\x20элементал"  ,
    "Воздушный\x20элементал"  ,
    "Огненный\x20элементал"  ,
    "Водяной\x20элементал"
};
char* gArmyNamesPlural[(CREATURE_COUNT)] = {
    "крестьян"  ,
    "стрелков"  ,
    "рейнджеров"  ,
    "копейщиков"  ,
    "копейщиков\x20ветеранов"  ,
    "мечников"  ,
    "мечников\x20мастеров"  ,
    "всадников"  ,
    "чемпионов"  ,
    "паладинов"  ,
    "крестоносцев"  ,
    "гоблинов"  ,
    "орков"  ,
    "вождей\x20орков"  ,
    "волков"  ,
    "огров"  ,
    "лордов\x20огров"  ,
    "троллей"  ,
    "боевых\x20троллей"  ,
    "циклопов"  ,
    "фей"  ,
    "гномов"  ,
    "боевых\x20гномов"  ,
    "эльфов"  ,
    "высоких\x20эльфов"  ,
    "друидов"  ,
    "старших\x20друидов"  ,
    "единорогов"  ,
    "фениксов"  ,
    "кентавров"  ,
    "горгулий"  ,
    "грифонов"  ,
    "минотавров"  ,
    "царей\x20минотавров"  ,
    "гидр"  ,
    "зеленых\x20драконов"  ,
    "красных\x20драконов"  ,
    "черных\x20драконов"  ,
    "полуросликов"  ,
    "боровов"  ,
    "железных\x20големов"  ,
    "стальных\x20големов"  ,
    "рухов"  ,
    "магов"  ,
    "архимагов"  ,
    "гигантов"  ,
    "титанов"  ,
    "скелетов"  ,
    "зомби"  ,
    "зомби\x20мутантов"  ,
    "мумий"  ,
    "королевских\x20мумий"  ,
    "вампиров"  ,
    "лордов\x20вампиров"  ,
    "личей"  ,
    "могучих\x20личей"  ,
    "костяных\x20драконов"  ,
    "разбойников"  ,
    "кочевников"  ,
    "призраков"  ,
    "джиннов"  ,
    "медуз"  ,
    "земных\x20элементалов"  ,
    "воздушных\x20элементалов"  ,
    "огненных\x20элементалов"  ,
    "водных\x20элементалов"
};
char* gTerrainNames[(TERRAIN_COUNT)] = {
    "Вода"  ,
    "Трава"  ,
    "Снег"  ,
    "Болото"  ,
    "Лава"  ,
    "Пустыня"  ,
    "Грязь"  ,
    "Пустошь"  ,
    "Побережье"
};
char* gResourceNames[RESOURCE_VALUE_COUNT] = {
    "Древесина"  ,
    "Ртуть"  ,
    "Руда"  ,
    "Сера"  ,
    "Кристаллы"  ,
    "Самоцветы"  ,
    "Золото"
};


char* gMineNames[KB_MINE_NAME_COUNT] = {
    "Лесопилка"  ,
    "Лаборатория\x20алхимика"  ,
    "Рудная\x20шахта"  ,
    "Серная\x20шахта"  ,
    "Кристальная\x20шахта"  ,
    "Самоцветная\x20шахта"  ,
    "Золотая\x20шахта"
};
char* gQuickViewText[KB_QUICK_VIEW_TEXT_COUNT] = {
    ""  ,
    "Лаборатория\x20алхимика"  ,
    "Указатель"  ,
    "Буй"  ,
    "Скелет"  ,
    "Пещера\x20демона"  ,
    "Ларец\x20с\x20сокровищами"  ,
    "Кольцо\x20фейри"  ,
    "Костер"  ,
    "Фонтан"  ,
    "Беседка"  ,
    "Древняя\x20лампа"  ,
    "Кладбище"  ,
    "Дом\x20стрелков"  ,
    "Хибара\x20гоблина"  ,
    "Избушка\x20гномов"  ,
    "Хижина\x20крестьян"  ,
    "Хижина"  ,
    "Дорога"  ,
    "Событие"  ,
    "Драконий\x20город"  ,
    "Маяк"  ,
    "Водяная\x20мельница"  ,
    "Шахта"  ,
    "Бивуак"  ,
    "Обелиск"  ,
    "Оазис"  ,
    "Ресурсы"  ,
    ""  ,
    "Лесопилка"  ,
    "Оракул"  ,
    "Святилище\x20\x31\x2dго\x20Круга"  ,
    "Кораблекрушение"  ,
    "Сундук"  ,
    "Шатер"  ,
    "Город"  ,
    "Менгир"  ,
    "Фургоны"  ,
    "Колодец"  ,
    "Водоворот"  ,
    "Ветряная\x20мельница"  ,
    "Артефакт"  ,
    "Герой"  ,
    "Корабль"  ,
    "Могущественный\x20артефакт"  ,
    "Случайный\x20артефакт"  ,
    "Случайный\x20ресурс"  ,
    "Случайный\x20монстр"  ,
    "Случайный\x20город"  ,
    "Случайный\x20замок"  ,
    ""  ,
    "Случайный\x20монстр\x20\x2d\x20слабый"  ,
    "Случайный\x20монстр\x20\x2d\x20средний"  ,
    "Случайный\x20монстр\x20\x2d\x20сильный"  ,
    "Случайный\x20монстр\x20\x2d\x20очень\x20сильный"  ,
    "Случайный\x20герой"  ,
    "Ничего\x20особенного"  ,
    ""  ,
    "Сторожевая\x20вышка"  ,
    "Древо\x2dгород"  ,
    "Древо\x2dгород"  ,
    "Руины"  ,
    "Форт"  ,
    "Базар"  ,
    "Заброшенная\x20шахта"  ,
    "Лачуга\x20гномов"  ,
    "Стоячие\x20камни"  ,
    "Идол"  ,
    "Древо\x20знания"  ,
    "Хижина\x20ведьмы"  ,
    "Храм"  ,
    "Форт\x20на\x20холме"  ,
    "Нора\x20полурослика"  ,
    "Лагерь\x20наемников"  ,
    "Святилище\x20\x32\x2dго\x20Круга"  ,
    "Святилище\x20\x33\x2dго\x20Круга"  ,
    "Пирамида"  ,
    "Город\x20мертвых"  ,
    "Котлован"  ,
    "Сфинкс"  ,
    "Тележка"  ,
    "Смоляная\x20яма"  ,
    "Артезианский\x20источник"  ,
    "Мост\x20троллей"  ,
    "Промоина"  ,
    "Хижина\x20ведьмы"  ,
    "Ксанаду"  ,
    "Пещера"  ,
    "Навес"  ,
    "Карты\x20Магеллана"  ,
    "Обломки"  ,
    "Заброшенный\x20корабль"  ,
    "Потерпевший\x20кораблекрушение"  ,
    "Бутылка"  ,
    "Волшебный\x20колодец"  ,
    "Волшебный\x20сад"  ,
    "Обзорная\x20башня"  ,
    "Литейный\x20цех"  ,
    "Потоки"  ,
    "Деревья"  ,
    "Горы"  ,
    "Вулкан"  ,
    "Цветы"  ,
    "Камень"  ,
    "Озеро"  ,
    "Мандрагора"  ,
    "Мертвое\x20дерево"  ,
    "Пень"  ,
    "Кратер"  ,
    "Кактус"  ,
    "Курган"  ,
    "Дюна"  ,
    "Лавовый\x20бассейн"  ,
    "Куст"  ,
    "Дыра"  ,
    "Пласт"  ,
    "Случайный\x20артефакт\x20\x2d\x20сокровище"  ,
    "Случайный\x20артефакт\x20\x2d\x20обычный"  ,
    "Случайный\x20артефакт\x20\x2d\x20ценный"  ,
    "\x25\x73\x20Барьер"  ,
    "\x25\x73\x20Шатер\x20путника"  ,
    "\x25\x73"  ,
    "\x25\x73"  ,
    "Темница"
};
char* gEventText[KB_EVENT_TEXT_TABLE_COUNT] = {


    "Алхимик\x0a\x0aВы\x20стали\x20хозяин"
        "ом\x20лаборатории\x20местного\x20"
        "алхимика\x2e\x20Она\x20будет\x20прин"
        "осить\x20вам\x20по\x20одной\x20едини"
        "це\x20ртути\x20в\x20день\x2e",

    "Указатель\x0a\x0aНа\x20указателе\x20"
        "написано\x3a\x0a\x0a\x25\x73\x20находится\x20"
        "неподалеку\x20отсюда\x2e",

    "Буй\x0a\x0aВаши\x20спутники\x20замеч"
        "ают\x20морской\x20буй\x2e\x20Он\x20указ"
        "ывает\x20верный\x20курс\x2e",


    "Буй\x0a\x0aВаши\x20спутники\x20замеч"
        "ают\x20морской\x20буй\x2e\x20Он\x20указ"
        "ывает\x20верный\x20курс\x2c\x20и\x20это"
        "\x20повышает\x20их\x20боевой\x20дух\x2e",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    "Кольцо\x20фейри\x0a\x0aВаше\x20войск"
        "о\x20вступает\x20внутрь\x20кольца"
        "\x20фейри\x2c\x20но\x20ничего\x20не\x20про"
        "исходит\x2e",


    "Кольцо\x20фейри\x0a\x0aВаше\x20войск"
        "о\x20вступает\x20внутрь\x20кольца"
        "\x20фейри\x2c\x20чары\x20которого\x20пр"
        "инесут\x20вам\x20удачу\x20в\x20гряду"
        "щем\x20сражении\x2e",

    "Костер\x0a\x0aОбыскав\x20вражески"
        "й\x20лагерь\x2c\x20вы\x20находите\x20сп"
        "рятанный\x20клад\x2e",

    "Фонтан\x0a\x0aВы\x20припадаете\x20к\x20"
        "струям\x20волшебного\x20фонтан"
        "а\x2c\x20но\x20ничего\x20не\x20происход"
        "ит\x2e",

    "Фонтан\x0a\x0aБлагоуханная\x20вла"
        "га\x20волшебного\x20фонтана\x20пр"
        "инесет\x20вам\x20удачу\x20в\x20гряду"
        "щем\x20сражении\x2e",


    "Беседка\x0a\x0aНа\x20ступенях\x20бес"
        "едки\x20появляется\x20старый\x20р"
        "ыцарь\x2e\x20\x22Мне\x20жаль\x2c\x20храбры"
        "й\x20воин\x2c\x20но\x20я\x20уже\x20научил\x20"
        "тебя\x20всему\x2c\x20что\x20знаю\x20сам"
        "\x2e\x22",


    "Беседка\x0a\x0aНа\x20ступенях\x20бес"
        "едки\x20появляется\x20старый\x20р"
        "ыцарь\x2e\x20\x22О\x20храбрый\x20воин\x2c\x20"
        "я\x20научу\x20тебя\x20всему\x2c\x20что\x20"
        "знаю\x20сам\x3b\x20пусть\x20мой\x20опыт"
        "\x20поможет\x20тебе\x20в\x20твоих\x20ст"
        "ранствиях\x2e\x22",

    "Лампа\x20джинна\x0a\x0aВы\x20находит"
        "е\x20засыпанную\x20землей\x20помя"
        "тую\x20и\x20закопченную\x20лампа\x2e"
        "\x20Хотите\x20ее\x20потереть\x3f",

    "Кладбище\x0a\x0aВы\x20осторожно\x20п"
        "риближаетесь\x20к\x20захоронен"
        "ию\x20древних\x20воинов\x2e\x20Хотит"
        "е\x20вскрыть\x20их\x20могилы\x3f",


    "Одержав\x20победу\x20над\x20зомби"
        "\x2c\x20вы\x20несколько\x20часов\x20под"
        "ряд\x20обыскиваете\x20могилы\x2c\x20"
        "но\x20ничего\x20не\x20находите\x2e\x20В"
        "аш\x20недостойный\x20поступок\x20"
        "отрицательно\x20влияет\x20на\x20б"
        "оевой\x20дух\x20войска\x2e",

    "Одержав\x20победу\x20над\x20зомби"
        "\x2c\x20вы\x20обыскиваете\x20могилы\x20"
        "и\x20удаляетесь\x20с\x20находкой\x21",


    "\x7bДом\x20стрелков\x7d\x0a\x0aГруппа\x20с"
        "трелков\x20в\x20поисках\x20славы\x20"
        "желает\x20примкнуть\x20к\x20вашем"
        "у\x20войску\x2e\x20Согласны\x20ли\x20вы"
        "\x20принять\x20их\x3f",

    "В\x20вашем\x20войске\x20нет\x20места"
        "\x20для\x20новых\x20рекрутов\x2e",

    "\x7bДом\x20стрелков\x7d\x0a\x0aПриблизи"
        "вшись\x20к\x20жилищу\x2c\x20вы\x20обнар"
        "уживаете\x2c\x20что\x20оно\x20пустуе"
        "т\x2e",


    "Хибара\x20гоблинов\x0a\x0aГруппа\x20"
        "гоблинов\x20в\x20поисках\x20славы"
        "\x20желает\x20примкнуть\x20к\x20ваше"
        "му\x20войску\x2e\x20Согласны\x20ли\x20в"
        "ы\x20принять\x20их\x3f",

    "Вы\x20не\x20можете\x20принять\x20нов"
        "ых\x20рекрутов\x20в\x20свое\x20войск"
        "о\x2c\x20его\x20ряды\x20полны\x2e",

    "Хибара\x20гоблинов\x0a\x0aПриблиз"
        "ившись\x20к\x20жилищу\x20гоблинов"
        "\x2c\x20вы\x20обнаруживаете\x2c\x20что\x20"
        "оно\x20пустует\x2e",


    "Хижина\x20крестьян\x0a\x0aГруппа\x20"
        "крестьян\x20в\x20поисках\x20славы"
        "\x20желает\x20примкнуть\x20к\x20ваше"
        "му\x20войску\x2e\x20Согласны\x20ли\x20в"
        "ы\x20принять\x20их\x3f",

    "Вы\x20не\x20можете\x20принять\x20нов"
        "ых\x20рекрутов\x20в\x20свое\x20войск"
        "о\x2c\x20его\x20ряды\x20полны\x2e",

    "Хижина\x20крестьян\x0a\x0aПриблиз"
        "ившись\x20к\x20жилищу\x20крестьян"
        "\x2c\x20вы\x20обнаруживаете\x2c\x20что\x20"
        "оно\x20пустует\x2e",


    "Избушка\x20гномов\x0a\x0aГруппа\x20с"
        "трелков\x20в\x20поисках\x20славы\x20"
        "желает\x20примкнуть\x20к\x20вашем"
        "у\x20войску\x2e\x20Согласны\x20ли\x20вы"
        "\x20принять\x20их\x3f",

    "Вы\x20не\x20можете\x20принять\x20нов"
        "ых\x20рекрутов\x20в\x20свое\x20войск"
        "о\x2c\x20его\x20ряды\x20полны\x2e",

    "Избушка\x20гномов\x0a\x0aПриблизи"
        "вшись\x20к\x20жилищу\x20стрелков\x2c"
        "\x20вы\x20обнаруживаете\x2c\x20что\x20о"
        "но\x20пустует\x2e",


    "\x7bМазанка\x7d\x0a\x0aГруппа\x20кресть"
        "ян\x20в\x20поисках\x20славы\x20желае"
        "т\x20примкнуть\x20к\x20вашему\x20вой"
        "ску\x2e\x20Согласны\x20ли\x20вы\x20прин"
        "ять\x20их\x3f",

    "Вы\x20не\x20можете\x20принять\x20нов"
        "ых\x20рекрутов\x20в\x20свое\x20войск"
        "о\x2c\x20его\x20ряды\x20полны\x2e",

    "\x7bМазанка\x7d\x0a\x0aПриблизившись"
        "\x20к\x20жилищу\x20Крестьян\x2c\x20вы\x20о"
        "бнаруживаете\x2c\x20что\x20оно\x20пу"
        "стует\x2e",


    "\x7bДрево\x2dдом\x7d\x0a\x0aГруппа\x20фей\x20"
        "в\x20поисках\x20славы\x20желает\x20п"
        "римкнуть\x20к\x20вашему\x20войску"
        "\x2e\x20Согласны\x20ли\x20вы\x20принять"
        "\x20их\x3f",

    "Вы\x20не\x20можете\x20принять\x20нов"
        "ых\x20рекрутов\x20в\x20свое\x20войск"
        "о\x2c\x20его\x20ряды\x20полны\x2e",

    "\x7bДрево\x2dдом\x7d\x0a\x0aПриблизивши"
        "сь\x20к\x20древесному\x20дому\x20Фей"
        "\x2c\x20вы\x20обнаруживаете\x2c\x20что\x20"
        "он\x20пустует\x2e",


    "\x7bНора\x20полуросликов\x7d\x0a\x0aГру"
        "ппа\x20полуросликов\x20в\x20поиск"
        "ах\x20славы\x20желает\x20примкнут"
        "ь\x20к\x20вашему\x20войску\x2e\x20Согла"
        "сны\x20ли\x20вы\x20принять\x20их\x3f",

    "Вы\x20не\x20можете\x20принять\x20нов"
        "ых\x20рекрутов\x20в\x20свое\x20войск"
        "о\x2c\x20его\x20ряды\x20полны\x2e",

    "\x7bНора\x20полуросликов\x7d\x0a\x0aПри"
        "близившись\x20к\x20норе\x20полуро"
        "сликов\x2c\x20вы\x20обнаруживаете"
        "\x2c\x20что\x20она\x20пустует\x2e",


    "\x7bСторожевая\x20вышка\x7d\x0a\x0aГруп"
        "па\x20орков\x20в\x20поисках\x20славы"
        "\x20желает\x20примкнуть\x20к\x20ваше"
        "му\x20войску\x2e\x20Согласны\x20ли\x20в"
        "ы\x20принять\x20их\x3f",

    "Вы\x20не\x20можете\x20принять\x20нов"
        "ых\x20рекрутов\x20в\x20свое\x20войск"
        "о\x2c\x20его\x20ряды\x20полны\x2e",

    "\x7bСторожевая\x20вышка\x7d\x0a\x0aПриб"
        "лизившись\x20к\x20сторожевой\x20в"
        "ышке\x20орков\x2c\x20вы\x20обнаружив"
        "аете\x2c\x20что\x20она\x20пустует\x2e",


    "\x7bСнежная\x20пещера\x7d\x0a\x0aГруппа"
        "\x20кентавров\x20в\x20поисках\x20сла"
        "вы\x20желает\x20примкнуть\x20к\x20ва"
        "шему\x20войску\x2e\x20Согласны\x20ли"
        "\x20вы\x20принять\x20их\x3f",

    "Вы\x20не\x20можете\x20принять\x20нов"
        "ых\x20рекрутов\x20в\x20свое\x20войск"
        "о\x2c\x20его\x20ряды\x20полны\x2e",

    "\x7bПещера\x7d\x0a\x0aПриблизившись\x20"
        "к\x20пещере\x20кентавров\x2c\x20вы\x20о"
        "бнаруживаете\x2c\x20что\x20она\x20пу"
        "стует\x2e",


    "\x7bРаскопки\x7d\x0a\x0aГруппа\x20скеле"
        "тов\x20в\x20поисках\x20славы\x20жела"
        "ет\x20примкнуть\x20к\x20вашему\x20во"
        "йску\x2e\x20Согласны\x20ли\x20вы\x20при"
        "нять\x20их\x3f",

    "Вы\x20не\x20можете\x20принять\x20нов"
        "ых\x20рекрутов\x20в\x20свое\x20войск"
        "о\x2c\x20его\x20ряды\x20полны\x2e",

    "\x7bРаскопки\x7d\x0a\x0aПриблизившис"
        "ь\x20к\x20захоронению\x20скелетов"
        "\x2c\x20вы\x20обнаруживаете\x2c\x20что\x20"
        "оно\x20пустует\x2e",
    "",
    "",
    "",
    "",
    "",

    "Маяк\x0a\x0aТеперь\x20маяк\x20ваш\x2c\x20и"
        "\x20все\x20ваши\x20корабли\x20будут\x20"
        "преодолевать\x20большее\x20рас"
        "стояние\x20за\x20один\x20ход\x2e",


    "Водяная\x20мельница\x0a\x0aМельни"
        "к\x20обращается\x20к\x20вам\x20со\x20сл"
        "овами\x3a\x20\x22Сожалею\x2c\x20господи"
        "н\x2c\x20но\x20сегодня\x20золота\x20у\x20м"
        "еня\x20нет\x2e\x20Приходите\x20на\x20сл"
        "едующей\x20неделе\x2e\x22",


    "Водяная\x20мельница\x0a\x0aМельни"
        "к\x20обращается\x20к\x20вам\x20со\x20сл"
        "овами\x3a\x20\x22Господин\x2c\x20я\x20труд"
        "ился\x20в\x20поте\x20лица\x20и\x20прошу"
        "\x20вас\x20принять\x20мою\x20скромну"
        "ю\x20лепту\x2e\x20Приходите\x20на\x20сл"
        "едующей\x20неделе\x2c\x20и\x20вы\x20пол"
        "учите\x20еще\x20столько\x20же\x2e\x22",

    "Рудная\x20шахта\x0a\x0aВы\x20стали\x20х"
        "озяином\x20рудной\x20шахты\x2e\x20Он"
        "а\x20будет\x20приносить\x20вам\x20по"
        "\x20две\x20меры\x20руды\x20в\x20день\x2e",


    "Серная\x20шахта\x0a\x0aВы\x20стали\x20х"
        "озяином\x20серной\x20шахты\x2e\x20Он"
        "а\x20будут\x20приносить\x20вам\x20по"
        "\x20\x31\x20единице\x20серы\x20в\x20день\x2e",


    "Кристальная\x20шахта\x0a\x0aВы\x20ст"
        "али\x20хозяином\x20кристальной"
        "\x20шахты\x2e\x20Она\x20будет\x20принос"
        "ить\x20вам\x20по\x20одной\x20мере\x20кр"
        "исталлов\x20в\x20день\x2e",


    "Самоцветная\x20шахта\x0a\x0aВы\x20ст"
        "али\x20хозяином\x20самоцветной"
        "\x20шахты\x2e\x20Она\x20будет\x20принос"
        "ить\x20вам\x20по\x20\x31\x20единице\x20сам"
        "оцветов\x20в\x20день\x2e",


    "Золотая\x20шахта\x0a\x0aВы\x20стали\x20"
        "хозяином\x20золотой\x20шахты\x2e\x20"
        "Она\x20будет\x20приносить\x20вам\x20"
        "по\x20\x31\x30\x30\x30\x20золотых\x20в\x20день\x2e",


    "Последователи\x0a\x0aГруппа\x20\x25\x73"
        "\x20в\x20поисках\x20славы\x20желает\x20"
        "примкнуть\x20к\x20вашему\x20войск"
        "у\x2e\x20Вы\x20согласны\x20принять\x20и"
        "х\x3f",

    "Оскорбленные\x20отказом\x20быт"
        "ь\x20принятыми\x20в\x20ваши\x20ряды\x2c"
        "\x20они\x20нападают\x20на\x20вас\x21",


    "Обелиск\x0a\x0aПеред\x20вами\x20обел"
        "иск\x2c\x20высеченный\x20из\x20невид"
        "анного\x20камня\x2e\x20Вы\x20вглядыв"
        "аетесь\x20в\x20его\x20гладкую\x20пов"
        "ерхность\x20и\x20вдруг\x20замечае"
        "те\x2c\x20что\x20на\x20ней\x20начинают\x20"
        "проступать\x20таинственные\x20"
        "знаки\x2e\x20Знаки\x20складываютс"
        "я\x20во\x20фрагмент\x20древней\x20ка"
        "рты\x2e\x20Вы\x20торопливо\x20срисов"
        "ываете\x20его\x2c\x20и\x20знаки\x20исче"
        "зают\x20так\x20же\x20внезапно\x2c\x20ка"
        "к\x20и\x20появились\x2e",

    "Обелиск\x0a\x0aВы\x20уже\x20посещали"
        "\x20этот\x20обелиск\x2e",
    "",
    "",

    "Вы\x20нашли\x20ресурс\x20\x28\x25\x73\x29\x2e",

    "Лесопилка\x0a\x0aВы\x20стали\x20хозя"
        "ином\x20лесопилки\x2e\x20Она\x20буде"
        "т\x20приносить\x20вам\x20по\x20\x32\x20еди"
        "ницы\x20древесины\x20в\x20день\x2e",


    "\x7bОракул\x7d\x0a\x0aНа\x20поляне\x20в\x20ок"
        "ружении\x20деревьев\x20восседа"
        "ет\x20слепой\x20оракул\x2e\x20Вы\x20рас"
        "сказываете\x20ему\x20о\x20целях\x20в"
        "ашего\x20похода\x2c\x20и\x20он\x20показ"
        "ывает\x20вам\x20сильные\x20и\x20слаб"
        "ые\x20стороны\x20ваших\x20противн"
        "иков\x20в\x20магическом\x20хруста"
        "льном\x20шаре\x2e",
    "",
    "",
    "",
    "",
    "",
    "",


    "\x7bШатер\x7d\x0a\x0aВаше\x20внимание\x20п"
        "ривлекает\x20шатер\x2c\x20пологи\x20"
        "которых\x20трепещут\x20на\x20жарк"
        "ом\x20ветру\x20пустыни\x2e\x20В\x20нем\x20"
        "никого\x20нет\x2e\x20Пройдет\x20врем"
        "я\x2c\x20и\x2c\x20быть\x20может\x2c\x20сюда\x20п"
        "ридет\x20новый\x20отряд\x20кочевн"
        "иков\x2e",


    "\x7bШатер\x7d\x0a\x0aВаше\x20внимание\x20п"
        "ривлекают\x20шатер\x2c\x20пологи\x20"
        "которого\x20трепещут\x20на\x20жар"
        "ком\x20ветру\x20пустыни\x2e\x20Вы\x20хо"
        "тите\x20принять\x20в\x20ваше\x20войс"
        "ко\x20отряд\x20кочевников\x3f",


    "\x7bПовозка\x7d\x0a\x0aЦветастая\x20пов"
        "озка\x20разбойников\x20пуста\x2e\x20"
        "Пройдет\x20время\x2c\x20и\x2c\x20быть\x20м"
        "ожет\x2c\x20здесь\x20обоснуется\x20н"
        "овая\x20шайка\x2e",


    "\x7bПовозка\x7d\x0a\x0aВдалеке\x20слыши"
        "тся\x20музыка\x20и\x20смех\x2e\x20Вы\x20ид"
        "ете\x20на\x20звуки\x20и\x20видите\x20цв"
        "етастую\x20повозку\x2c\x20в\x20котор"
        "ой\x20живут\x20разбойники\x2e\x20Вы\x20"
        "хотите\x20принять\x20в\x20ваше\x20во"
        "йско\x20шайку\x20разбойников\x3f",

    "\x7bВодоворот\x7d\x0a\x0aВаш\x20корабль"
        "\x20попадает\x20в\x20водоворот\x2e\x20Ч"
        "асть\x20вашего\x20войска\x20исчез"
        "ает\x20в\x20пучине\x2e",


    "\x7bВетряная\x20мельница\x7d\x0a\x0aМел"
        "ьник\x20обращается\x20к\x20вам\x20со"
        "\x20словами\x3a\x20\x22Сожалею\x2c\x20госп"
        "один\x2c\x20но\x20сегодня\x20у\x20меня\x20"
        "ничего\x20нет\x2e\x20Приходите\x20на"
        "\x20следующей\x20неделе\x2e\x22",


    "\x7bВетряная\x20мельница\x7d\x0a\x0aМел"
        "ьник\x20обращается\x20к\x20вам\x20со"
        "\x20словами\x3a\x20\x22Господин\x2c\x20я\x20р"
        "аботал\x20не\x20покладая\x20рук\x2c\x20"
        "и\x20прошу\x20вас\x20принять\x20мой\x20"
        "скромный\x20дар\x2e\x20Приходите\x20"
        "на\x20следующей\x20неделе\x2c\x20у\x20м"
        "еня\x20опять\x20найдется\x2c\x20чем\x20"
        "вас\x20порадовать\x2e\x22",
    "",
    "",
    "",
    "",
    "",


    "\x7bСкелет\x7d\x0a\x0aВы\x20находите\x20ос"
        "танки\x20незадачливого\x20иска"
        "теля\x20приключений\x2e\x20Пошари"
        "в\x20в\x20груде\x20лохмотьев\x2c\x20вы\x20"
        "ничего\x20не\x20находите\x2e",


    "\x7bСкелет\x7d\x0a\x0aВы\x20находите\x20ос"
        "танки\x20незадачливого\x20иска"
        "теля\x20приключений\x2e\x20Пошари"
        "в\x20в\x20груде\x20лохмотьев\x2c\x20вы\x20"
        "находите\x2e"
};
char* gCPanelHelp[KB_CONTROL_PANEL_HELP_COUNT] = {

    "Начать\x20одиночную\x20или\x20сет"
        "евую\x20игру\x2e",

    "Загрузить\x20сохраненную\x20иг"
        "ру\x2e",

    "Сохранить\x20игру\x2e",

    "Выйти\x20из\x20Героев\x20Меча\x20и\x20М"
        "агии\x20\x49\x49\x2e",

    "Закрыть\x20меню\x2c\x20ничего\x20не\x20"
        "делая\x2e"
};
char* gCSPanelHelp[KB_COMBAT_SPELL_PANEL_HELP_COUNT] = {

    "\x7bОК\x7d\x0a\x0aЗакрыть\x20это\x20меню\x2e",

    "\x7bСкорость\x7d\x0a\x0aУстановить\x20с"
        "корость\x20действий\x20и\x20анима"
        "ции\x20воинов\x20в\x20бою\x2e",


    "\x7bИнформация\x20о\x20воине\x7d\x0a\x0aВк"
        "лючить\x20или\x20выключить\x20ото"
        "бражение\x20окна\x20с\x20информац"
        "ией\x20о\x20выбранном\x20и\x20атакуе"
        "мом\x20воине\x2e",


    "\x7bМагия\x20в\x20автобое\x7d\x0a\x0aЕсли\x20"
        "эта\x20опция\x20включена\x2c\x20ваш\x20"
        "герой\x20будет\x20использовать"
        "\x20заклинания\x20во\x20время\x20авт"
        "обоя\x2e\x20\x28Примечание\x3a\x20Эта\x20о"
        "пция\x20не\x20влияет\x20на\x20исполь"
        "зование\x20заклинаний\x20компь"
        "ютерными\x20игроками\x2c\x20и\x20на\x20"
        "быстрый\x20бой\x2e\x29",


    "\x7bСетка\x7d\x0a\x0aВключает\x20или\x20вы"
        "ключает\x20отображение\x20сетк"
        "и\x2e\x20Все\x20перемещения\x20на\x20по"
        "ле\x20боя\x20происходят\x20по\x20гек"
        "сагональной\x20сетке\x2c\x20даже\x20"
        "если\x20ее\x20отображение\x20откл"
        "ючено\x2e",


    "\x7bЗатенение\x20сетки\x7d\x0a\x0aВключ"
        "ает\x20или\x20выключает\x20режим\x20"
        "обозначения\x20возможной\x20да"
        "льности\x20передвижения\x20выб"
        "ранного\x20отряда\x20воинов\x2e",

    "\x7bКурсор\x20с\x20тенью\x7d\x0a\x0aВключа"
        "ет\x20или\x20выключает\x20отрисов"
        "ку\x20тени\x20от\x20курсора\x20на\x20се"
        "тке\x20координат\x2e"
};
char* gAPanelHelp[KB_ADVENTURE_PANEL_HELP_COUNT] = {

    "Осмотреть\x20весь\x20мир\x2e",

    "Посмотреть\x20головоломку\x2e",

    "Показать\x20информацию\x20о\x20сц"
        "енарии\x2c\x20на\x20котором\x20идет\x20"
        "игра\x2e",

    "Копать\x20в\x20поисках\x20Великог"
        "о\x20артефакта\x2e",

    "Закрыть\x20это\x20меню\x2e"
};
char* gInitMenuHelp[KB_INIT_MENU_HELP_COUNT] = {

    "\x7bНовая\x20игра\x7d\x0a\x0aНачать\x20отд"
        "ельный\x20сценарий\x20или\x20сете"
        "вую\x20игру\x2e",

    "\x7bИгры\x7d\x0a\x0aЗагрузить\x20ранее\x20"
        "сохраненную\x20игру\x2e",

    "\x7bРекорды\x7d\x0a\x0aПоказать\x20табл"
        "ицу\x20рекордов\x2e",

    "\x7bАвторы\x7d\x0a\x0aПоказать\x20переч"
        "ень\x20авторов\x20игры\x2e",

    "\x7bВыйти\x7d\x0a\x0aВыйти\x20из\x20героев"
        "\x20Меча\x20и\x20Магии\x20\x49\x49\x20и\x20верну"
        "ться\x20в\x20операционную\x20сист"
        "ему\x2e"
};
char* gAdvMenuHelp[KB_ADVENTURE_MENU_HELP_COUNT] = {

    "\x7bСледующий\x20герой\x7d\x0a\x0aВыбра"
        "ть\x20следующего\x20героя\x2e",

    "\x7bПродолжить\x20движение\x7d\x0a\x0aП"
        "родолжить\x20движение\x20героя"
        "\x20по\x20намеченному\x20пути\x2e",

    "\x7bОбзор\x20королевства\x7d\x0a\x0aОсм"
        "отреть\x20ваши\x20владения\x2e",

    "\x7bОкончить\x20ход\x7d\x0a\x0aОкончить"
        "\x20ход\x20и\x20передать\x20управлен"
        "ие\x20компьютеру\x2e",

    "\x7bИгровые\x20действия\x7d\x0a\x0aОткр"
        "ыть\x20окно\x20доступных\x20игров"
        "ых\x20действий\x2e",

    "\x7bОкно\x20файлов\x7d\x0a\x0aОткрывает"
        "\x20меню\x2c\x20где\x20вы\x20можете\x20заг"
        "ружать\x20или\x20сохранять\x20игр"
        "ы\x2e",

    "\x7bСистемные\x20настройки\x7d\x0a\x0aО"
        "ткрывает\x20окно\x20системных\x20"
        "настроек\x2c\x20позволяющих\x20на"
        "строить\x20игру\x2e",

    "\x7bНаправить\x20заклинание\x7d\x0a\x0a"
        "Направить\x20заклинание\x20на\x20"
        "стратегической\x20карте\x2e"
};
char* gLuckText[KB_LUCK_TEXT_COUNT] = {

    "Проклятая",

    "Ужасная",

    "Плохая",

    "Обычная",

    "Хорошая",

    "Отличная",

    "Божественная"
};
char* gMoraleText[KB_MORALE_TEXT_COUNT] = {

    "Предательская",

    "Ужасная",

    "Плохая",

    "Обычная",

    "Хорошая",

    "Отличная",

    "Кровавая\x21"
};
char* onOffText[KB_ON_OFF_TEXT_COUNT] = {

    "Выкл\x2e",

    "Вкл\x2e",

    "Вкл\x2e\x0aГромкость\x20\x39",

    "Вкл\x2e\x0aГромкость\x20\x38",

    "Вкл\x2e\x0aГромкость\x20\x37",

    "Вкл\x2e\x0aГромкость\x20\x36",

    "Вкл\x2e\x0aГромкость\x20\x35",

    "Вкл\x2e\x0aГромкость\x20\x34",

    "Вкл\x2e\x0aГромкость\x20\x33",

    "Вкл\x2e\x0aГромкость\x20\x32",

    "Вкл\x2e\x0aГромкость\x20\x31"
};
char* walkSpeedText[KB_WALK_SPEED_TEXT_COUNT] = {

    "Шагом",

    "Рысью",

    "Аллюром",

    "Галопом",

    "Прыжками"
};
char* gColors[(FACTION_COUNT)] = {
    "синий"  ,
    "зеленый"  ,
    "красный"  ,
    "желтый"  ,
    "оранжевый"  ,
    "фиолетовый"
};
static char* gColorAbbreviations[(FACTION_COUNT)] = {
    "син."  ,
    "зел."  ,
    "кр."  ,
    "жел."  ,
    "ор."  ,
    "фиол."
};
char* gMonthNames[KB_MONTH_NAME_COUNT] = {

    "Кузнечика",

    "Муравья",

    "Стрекозы",

    "Паука",

    "Бабочки",

    "Шмеля",

    "Цикады",

    "Земляного\x20червя",

    "Шершня",

    "Жука"
};
char* gWeekNames[KB_WEEK_NAME_COUNT] = {

    "Белки",

    "Кролика",

    "Суслика",

    "Барсука",

    "Крысы",

    "Орла",

    "Горностая",

    "Ворона",

    "Мангуста",

    "Собаки",

    "Муравьеда",

    "Ящерицы",

    "Черепахи",

    "Дикобраза",

    "Кондора"
};
char* cHeroScreen[KB_HERO_SCREEN_TEXT_COUNT] = {

    "Обзор\x20королевства",

    "\x25\x73\x20\x2d\x20информация",

    "Дополнительная\x20статистик"
        "а\x20героя",

    "Информация\x20о\x20высокой\x20мор"
        "али",

    "Информация\x20об\x20обычной\x20мо"
        "рали",

    "Информация\x20о\x20плохой\x20мора"
        "ли",

    "Информация\x20о\x20хорошей\x20уда"
        "че",

    "Информация\x20об\x20обычной\x20уд"
        "аче",

    "Информация\x20о\x20плохой\x20удач"
        "е",

    "Показать\x20опыт",

    "Выбрать\x20\x25\x73",

    "Пусто",

    "Перенести\x20сюда\x20отряд\x20\x25\x73",

    "Отряды\x20\x25\x73\x20и\x20\x25\x73\x20меняются\x20"
        "местами",

    "Показать\x20заклинания",

    "Посмотреть\x20информацию\x20об"
        "\x3a\x20\x25\x73",

    "\x25\x73\x20\x25\x73\x20\x2d\x20уволить",

    "Закрыть\x20экран\x20героя",

    "Экран\x20героя",

    "\x25\x73\x20в\x20один\x20отряд",

    "Разделить\x20отряд\x20\x25\x73",

    "\x25\x73\x20\x25\x73\x20\x2d\x20информация",

    "Информация\x20об\x20очках\x20маги"
        "и",

    "Выбрать\x20широкие\x20ряды\x20в\x20б"
        "ою",

    "Сгруппировать\x20воинов"
};
char* cCastleInfo[KB_CASTLE_INFO_TEXT_COUNT] = {

    "Построить\x20Гильдию\x20магов",

    "Построены\x20все\x20этажи\x20Гиль"
        "дии\x20магов\x2e",

    "Нельзя\x20построить\x20следующ"
        "ий\x20этаж\x2e",

    "Построить\x20следующий\x20этаж"
        "\x20Гильдии\x20магов\x20",

    "Постройка\x20\x27\x25\x73\x27\x20уже\x20возве"
        "дена",

    "Нельзя\x20возвести\x20постройк"
        "у\x20\x27\x25\x73\x27",

    "Нельзя\x20возвести\x20постройк"
        "у\x20\x27\x25\x73\x27",

    "Возвести\x20постройку\x20\x27\x25\x73\x27",

    "Герой\x20вам\x20не\x20по\x20карману\x2e",

    "Нельзя\x20нанять\x20\x2d\x20у\x20вас\x20уж"
        "е\x20\x25\x64\x20героев\x2e",

    "Нельзя\x20нанять\x20\x2d\x20в\x20этом\x20г"
        "ороде\x20у\x20вас\x20уже\x20есть\x20гер"
        "ой\x2e",

    "Нанять\x20нового\x20героя",

    "Выйти\x20из\x20замка",

    "Возможности\x20замка",

    "Сгруппировать\x20гарнизон",

    "Выбрать\x20широкие\x20ряды\x20для"
        "\x20гарнизона"
};
char* cLuckInfo[KB_LUCK_INFO_TEXT_COUNT] = {


    "\x7bХорошая\x20удача\x7d\x0a\x0aЕсли\x20уд"
        "ача\x20вашего\x20войска\x20выше\x20о"
        "бычной\x2c\x20атаки\x20отдельных\x20"
        "отрядов\x20на\x20поле\x20боя\x20иног"
        "да\x20оказываются\x20более\x20рез"
        "ультативными\x20\x28их\x20сила\x20уд"
        "ваивается\x29\x2e",


    "\x7bОбычная\x20удача\x7d\x0a\x0aС\x20обычн"
        "ой\x20удачей\x20ваше\x20войско\x20не"
        "\x20имеет\x20ни\x20преимуществ\x2c\x20н"
        "и\x20недостатков\x20на\x20поле\x20бо"
        "я\x2e",


    "\x7bПлохая\x20удача\x7d\x0a\x0aЕсли\x20ваш"
        "ему\x20войску\x20не\x20везет\x2c\x20уро"
        "н\x2c\x20наносимый\x20\x20отдельными"
        "\x20отрядами\x20на\x20поле\x20боя\x2c\x20м"
        "ожет\x20оказаться\x20вдвое\x20мен"
        "ьше\x20обычного\x2e",

    "\x25\x73\x0a\x0a\x0aМодификаторы\x20удачи\x3a",

    "\x0aЛапка\x20кролика\x20\x2b\x31",

    "\x0aЗолотая\x20подкова\x20\x2b\x31",

    "\x0aМонета\x20\x2b\x31",

    "\x0aКлевер\x20\x2b\x31",

    "\x0aПосещен\x20Круг\x20фейри\x20\x2b\x31",

    "\x0aПосещен\x20фонтан\x20\x2b\x31",

    "\x0aНет",

    "\x0aГрабитель\x20могил\x20\x2d\x31",

    "\x0aРадуга\x20магов\x20\x2b\x32",

    "\x0aПосещен\x20идол\x20\x2b\x31",

    "\x0aОграблена\x20пирамида\x20\x2d\x32",

    "\x0aБазовая\x20удача\x20\x2b\x31",

    "\x0aВысокая\x20удача\x20\x2b\x32",

    "\x0aЭксперт\x20удачи\x20\x2b\x33",

    "\x0aБонус\x20мачты\x20на\x20море\x20\x2b\x31",

    "\x0aПосещена\x20русалка\x20\x2b\x31",

    "\x0aБоевое\x20одеяние\x20Андурана"
        "\x20дает\x20максимальную\x20удачу"
        "\x2e"
};
char* IQnames[KB_IQ_NAME_COUNT] = {

    "Нет",

    "Глупый",

    "Средний",

    "Умный",

    "Гений"
};
char* cSpellHelp[KB_SPELL_HELP_TEXT_COUNT] = {

    "Предыдущая\x20страница\x20",

    "Следующая\x20страница",

    "Небоевые\x20заклинания",

    "Боевые\x20заклинания",

    "Закрыть\x20волшебную\x20книгу",

    "Заклинания",

    "Выбрать\x20заклинание",

    "Боевые\x20заклинания",

    "У\x20вашего\x20героя\x20осталось\x20"
        "\x25\x64\x20оч\x2e\x20магии"
};
char* speedText[KB_SPEED_TEXT_COUNT] = {
      "",
      "Ползает",
      "Оч\x2e\x20низкая",
      "Низкая",
      "Средняя",
      "Высокая",
      "Оч\x2e\x20высокая",
      "Ультра\x20высокая",
      "Молниеносная",
      "Абсолютная"
};
char* cArmyDetail[KB_ARMY_DETAIL_TEXT_COUNT] = {
      "Атака\x3a\x20",
      "Защита\x3a\x20",
      "Выстрелов\x3a\x20",
      "Урон\x3a\x20",
      "Здоровье\x3a\x20",
      "Скорость\x3a\x20",
      "Мораль\x3a\x20",
      "Удача\x3a\x20",
      "Выстрелов\x3a\x20"
};
char* cWellDetail[KB_WELL_DETAIL_TEXT_COUNT] = {
      "Атака\x3a\x20",
      "Защита\x3a\x20",
      "Выстр\x2e\x3a\x20",
      "Урон\x3a\x20",
      "ЗД\x3a\x20",
      "Скор\x2e\x3a\x20",
      "Всего\x3a\x20",
      "\x0a\x0aСкорость\x3a\x0a\x25\x73",
      "\x0a\x0aПрирост\x0a\x20\x2b\x20\x25\x64\x2fнед\x2e"
};
char* cKingdomOverview[KB_KINGDOM_OVERVIEW_TEXT_COUNT] = {
      "Обзор\x20королевства\x20\x20\x20Месяц\x3a\x20\x25\x64\x2c\x20Неделя\x3a\x20\x25\x64\x2c\x20День\x3a"
        "\x20\x25\x64",
      "Ваш\x20Драконий\x20город\x2e",
      "Ваш\x20маяк\x2e"
};
char* cNewTurn[KB_NEW_TURN_TEXT_COUNT] = {
      "\x25\x73\x2c\x20у\x20вас\x20осталось\x20всего\x20\x25\x64\x20дней\x20на\x20то\x2c\x20чтобы\x20за"
        "воевать\x20хотя\x20бы\x20один\x20город\x3b\x20иначе\x20вы\x20будете\x20наве"
        "ки\x20изгнаны\x20из\x20страны\x2e",
      "\x25\x73\x2c\x20настал\x20последний\x20день\x2c\x20когда\x20вы\x20еще\x20можете\x20з"
        "авоевать\x20себе\x20город\x3b\x20в\x20противном\x20случае\x20вы\x20будет"
        "е\x20навеки\x20изгнаны\x20из\x20страны\x2e",
      "Астрологи\x20объявляют\x20месяц\x20\x25\x73\x2e\x0a\x0aНаселение\x20всех\x20жи"
        "лищ\x20возросло\x2e",
      "Астрологи\x20объявляют\x2c\x20что\x20этому\x20месяцу\x20покровител"
        "ьствует\x20сила\x20\x25\x73\x2e\x0a\x0aПопуляция\x20\x25\x73\x20удваивается\x21\x0a\x0aНас"
        "еление\x20всех\x20жилищ\x20возросло\x2e",
      "Астрологи\x20объявляют\x20месяц\x20ЧУМЫ\x21\x0a\x0aНаселение\x20всех\x20"
        "жилищ\x20уменьшилось\x20вдвое\x2e",
      "Астрологи\x20объявляют\x20неделю\x20\x25\x73\x2e\x0a\x0aНаселение\x20всех\x20ж"
        "илищ\x20возросло\x2e",
      "Астрологи\x20объявляют\x2c\x20что\x20этой\x20неделе\x20покровитель"
        "ствует\x20сила\x20\x25\x73\x2e\x0a\x0aПопуляция\x20\x25\x73\x20\x2b\x35\x2e\x0a\x0aНаселение\x20все"
        "х\x20жилищ\x20возросло\x2e"
};
char* cViewGeneralLabels[KB_VIEW_GENERAL_LABEL_COUNT] = {
      "Атака\x3a\x20",
      "Защита\x3a\x20",
      "Сила\x20магии\x3a\x20",
      "Знания\x3a\x20",
      "Мораль\x3a\x20",
      "Удача\x3a\x20",
      "Очки\x20магии\x3a\x20"
};
char* cViewGeneralHelp[KB_VIEW_GENERAL_HELP_COUNT] = {
      "Остановить\x20катапульту",
      "Направить\x20заклинание",
      "Отступить",
      "Сдаться",
      "Отменить",
      "Возможности\x20героя",
      "Возможности\x20капитана"
};
char* cViewGeneralLongHelp[KB_VIEW_GENERAL_LONG_HELP_COUNT] = {
      "\x7bНаправить\x20заклинание\x7d\x0a\x0aНаправить\x20заклинание\x2e\x20В\x20"
        "течение\x20каждого\x20раунда\x20боя\x20можно\x20направить\x20лишь\x20"
        "одно\x20заклинание\x2e\x20Новый\x20раунд\x20начинается\x20после\x20то"
        "го\x2c\x20как\x20все\x20отряды\x20на\x20поле\x20боя\x20завершили\x20свой\x20хо"
        "д\x2e",
      "\x7bОтступить\x7d\x0a\x0aГерой\x20отступает\x20с\x20поля\x20боя\x2c\x20бросив\x20"
        "свое\x20войско\x20на\x20произвол\x20судьбы\x2e\x20Отступившего\x20гер"
        "оя\x20можно\x20будет\x20снова\x20нанять\x20на\x20службу\x2c\x20но\x20при\x20эт"
        "ом\x20сопровождать\x20его\x20будет\x20лишь\x20очень\x20небольшая\x20а"
        "рмия\x2c\x20как\x20если\x20бы\x20ваш\x20герой\x20был\x20зеленым\x20новичком"
        "\x2e",
      "\x7bСдаться\x7d\x0a\x0aКапитуляция\x20стоит\x20денег\x2e\x20Тем\x20не\x20менее"
        "\x2c\x20если\x20выкуп\x20будет\x20уплачен\x2c\x20героя\x20можно\x20будет\x20сн"
        "ова\x20нанять\x20на\x20службу\x20вместе\x20со\x20всеми\x20уцелевшими\x20"
        "в\x20битве\x20войсками\x2e",
      "\x7bОтмена\x7d\x0a\x0aВернуться\x20в\x20бой\x2e"
};
char* cCombatMessage[KB_COMBAT_MESSAGE_COUNT] = {
      "",
      "\x25\x73\x3a\x20Идти\x20сюда\x2e",
      "\x25\x73\x3a\x20Перелететь\x20сюда\x2e",
      "Атаковать\x20\x25\x73",
      "Стрелять\x20в\x20\x25\x73\x20\x28осталось\x20\x25\x64\x20выстр\x2e\x29",
      "Возможности\x20героя",
      "Вражеский\x20герой",
      "\x25\x73\x3a\x20Показать\x20информацию\x2e",
      "Нет\x20стрел\x21",
      "Возможности\x20капитана",
      "Показать\x20вражеского\x20капитана",
      "Информация\x20о\x20баллисте"
};
char* cHeroLevel[KB_HERO_LEVEL_TEXT_COUNT] =
    {  "\x25\x73\x20получает",   "\x20уровень\x20опыта\x2e\x0a",   "\x20\x25\x64\x20уровней\x20опыта\x2e\x0a"};
char* cCombatHelp[KB_COMBAT_HELP_COUNT] = {
      "Подождать\x2c\x20пока\x20походят\x20другие",
      "Пропустить\x20ход\x20этого\x20воина",
      "Автобой",
      "Системные\x20настройки",
      ""
};
char* cLongCombatHelp[KB_LONG_COMBAT_HELP_COUNT] = {
      "\x7bЖдать\x7d\x0a\x0aДанный\x20отряд\x20откладывает\x20свой\x20ход\x20и\x20сов"
        "ершает\x20действие\x20после\x20того\x2c\x20как\x20все\x20остальные\x20от"
        "ряды\x20походили\x2e",
      "\x7bПропустить\x20ход\x7d\x0a\x0aОтряд\x20пропускает\x20свой\x20ход\x20в\x20эт"
        "ом\x20раунде\x2e",
      "\x7bАвтобой\x7d\x0a\x0aКомпьютер\x20вместо\x20вас\x20управляет\x20вашими"
        "\x20войсками\x20во\x20время\x20боя\x2e",
      "\x7bНастройки\x7d\x0a\x0aПозволяет\x20изменять\x20настройки\x20боя\x2e",
      "\x7bИнформационная\x20строка\x7d\x0a\x0aЗдесь\x20отображаются\x20резу"
        "льтаты\x20действий\x20отдельных\x20отрядов\x2e"
};
char* cTownCommand[KB_TOWN_COMMAND_COUNT] = {
      "Разделить\x20отряд\x20\x25\x73",
      "Нельзя\x20отнять\x20последних\x20воинов\x20у\x20героя\x20",
      "Соединить\x20отряды\x20\x25\x73",
      "Разделить\x20отряд\x20\x25\x73",
      "Посмотреть\x20на\x20\x25\x73",
      "Нельзя\x20перенести\x20в\x20гарнизон\x20последний\x20отряд\x2e",
      "Передвинуть\x20сюда\x20отряд\x20\x25\x73",
      "Отряды\x20\x25\x73\x20и\x20\x25\x73\x20меняются\x20местами",
      "Выйти\x20из\x20города",
      "",
      "Обзор\x20королевства",
      "Пусто",
      "\x25\x73",
      "Показать\x20героя",
      "Гильдия\x20магов",
      "Гильдия\x20воров",
      "Таверна",
      "Верфь",
      "Колодец",
      "Шатер",
      "Замок",
      "Нанять\x20\x25\x73",
      "Статуя",
      "Левая\x20башня",
      "Правая\x20башня",
      "Ров",
      "Рынок",
      "Дом\x20капитана"
};
char* gHeroDefaultNames[KB_HERO_DEFAULT_NAME_COUNT] = {
      "Лорд\x20Килбурн",   "Сэр\x20Галлант",   "Эктор",      "Гвеннет",   "Тиро",      "Амброзий",     "Руби",
      "Максимус",        "Димитри",       "Сундакс",    "Финеоз",    "Джоджош",    "Крэг\x20Хак",   "Джезебель",
      "Жаклин",         "Эргон",         "Тсабу",      "Атлас",      "Астра",     "Наташа",     "Троян",
      "Ватавна",        "Ребекка",       "Гем",        "Ариэль",      "Карлавн",   "Луна",        "Арий",
      "Аламар",         "Виспер",        "Кродо",      "Барок",      "Кастор",   "Агар",        "Фалагар",
      "Расмонт",      "Мира",          "Флинт",      "Давн",       "Галон",     "Мирини",      "Вилфрей",
      "Саракин",        "Калиндра",      "Мандигал",   "Зом",        "Дарлана",   "Зам",         "Ранлу",
      "Чарити",        "Риалдо",        "Роксана",     "Сандро",     "Келия"
};
char* gNewGameHelp[KB_NEW_GAME_HELP_COUNT] = {
      "\x7bУровень\x20сложности\x7d\x0a\x0aЭта\x20опция\x20позволяет\x20устанав"
        "ливать\x20стартовый\x20уровень\x20сложности\x20игры\x2e\x20Чем\x20выш"
        "е\x20уровень\x20сложности\x2c\x20тем\x20с\x20меньшим\x20количеством\x20р"
        "есурсов\x20вы\x20начинаете\x20игру\x2c\x20и\x20тем\x20больше\x20ресурсов"
        "\x20получают\x20ваши\x20компьютерные\x20противники\x2e",
      "\x7bФора\x7d\x0a\x0aЭта\x20опция\x20позволяет\x20задавать\x20тому\x20или\x20ин"
        "ому\x20игроку\x2dчеловеку\x20дать\x20фору\x20другим\x20игрокам\x2e\x20Ес"
        "ли\x20игрок\x20дает\x20другим\x20фору\x2c\x20он\x20начинает\x20игру\x20с\x20ме"
        "ньшим\x20количеством\x20ресурсов\x20и\x20каждый\x20ход\x20получает"
        "\x20на\x20\x31\x35\x20или\x20\x33\x30\x20процентов\x20меньше\x20ресурсов\x20в\x20зависи"
        "мости\x20от\x20того\x2c\x20насколько\x20большую\x20фору\x20он\x20дает\x2e",
      "\x7bОппоненты\x7d\x0a\x0aЭта\x20опция\x20позволяет\x20вам\x20задать\x20цвет"
        "\x20игрока\x20и\x20его\x20стартовую\x20позицию\x2e\x20Каждому\x20цвету\x20с"
        "оответствует\x20определенная\x20стартовая\x20позиция\x2e\x20Нек"
        "оторые\x20цвета\x20жестко\x20закреплены\x20либо\x20за\x20компьютер"
        "ными\x2c\x20либо\x20за\x20живыми\x20игроками\x2e",
      "\x7bКласс\x7d\x0a\x0aЭта\x20опция\x20позволяет\x20задавать\x20класс\x20игро"
        "ка\x2e\x20Классы\x20не\x20всегда\x20можно\x20изменять\x2e\x20В\x20зависимос"
        "ти\x20от\x20сценария\x20игрок\x20может\x20получать\x20дополнительн"
        "ые\x20города\x20и\x2fили\x20героев\x2c\x20направленность\x20которых\x20н"
        "е\x20совпадает\x20с\x20изначальной\x20направленностью\x20игрока"
        "\x2e",
      "\x7bСценарий\x7d\x0a\x0aЭта\x20опция\x20позволяет\x20выбрать\x20игровой\x20"
        "сценарий\x2e",
      "\x7bРейтинг\x7d\x0a\x0aРейтинг\x20отражает\x20сочетание\x20различных\x20"
        "игровых\x20установок\x2e\x20Он\x20используется\x20при\x20расчете\x20к"
        "онечного\x20результата\x2c\x20достигнутого\x20игроком\x2e",
      "\x7bОК\x7d\x0a\x0aПодтверждает\x20заданные\x20установки\x20и\x20начинает"
        "\x20новую\x20игру\x2e",
      "\x7bОтмена\x7d\x0a\x0aНажмите\x2c\x20чтобы\x20вернуться\x20в\x20главное\x20мен"
        "ю\x2e"
};
char* gSetupBaudHelp[KB_SETUP_BAUD_HELP_COUNT] = {
      "\x7b\x32\x34\x30\x30\x20бод\x7d\x0a\x0aИспользовать\x20соединение\x20на\x20скорости\x20"
        "\x32\x34\x30\x30\x20бод\x2e\x0a\x0aЗамечание\x3a\x20Для\x20модемов\x20\x31\x34\x34\x30\x30\x20бод\x20испо"
        "льзуйте\x20соединение\x20на\x20скорости\x20\x31\x39\x32\x30\x30\x2e\x20\x20Для\x20модем"
        "ов\x20\x32\x38\x38\x30\x30\x20бод\x20используйте\x20соединение\x20на\x20скорости\x20"
        "\x33\x38\x34\x30\x30\x20бод\x2e",
      "\x7b\x39\x36\x30\x30\x20бод\x7d\x0a\x0aИспользовать\x20соединение\x20на\x20скорости\x20"
        "\x39\x36\x30\x30\x20бод\x2e\x0a\x0aЗамечание\x3a\x20Для\x20модемов\x20\x31\x34\x34\x30\x30\x20бод\x20испо"
        "льзуйте\x20соединение\x20на\x20скорости\x20\x31\x39\x32\x30\x30\x2e\x20\x20Для\x20модем"
        "ов\x20\x32\x38\x38\x30\x30\x20бод\x20используйте\x20соединение\x20на\x20скорости\x20"
        "\x33\x38\x34\x30\x30\x20бод\x2e",
      "\x7b\x31\x39\x32\x30\x30\x20бод\x7d\x0a\x0aИспользовать\x20соединение\x20на\x20скорости"
        "\x20\x31\x39\x32\x30\x30\x20бод\x2e\x0a\x0aЗамечание\x3a\x20Для\x20модемов\x20\x31\x34\x34\x30\x30\x20бод\x20ис"
        "пользуйте\x20соединение\x20на\x20скорости\x20\x31\x39\x32\x30\x30\x2e\x20\x20Для\x20мод"
        "емов\x20\x32\x38\x38\x30\x30\x20бод\x20используйте\x20соединение\x20на\x20скорост"
        "и\x20\x33\x38\x34\x30\x30\x20бод\x2e",
      "\x7b\x33\x38\x34\x30\x30\x20бод\x7d\x0a\x0aИспользовать\x20соединение\x20на\x20скорости"
        "\x20\x33\x38\x34\x30\x30\x20бод\x2e\x0a\x0aЗамечание\x3a\x20Для\x20модемов\x20\x31\x34\x34\x30\x30\x20бод\x20ис"
        "пользуйте\x20соединение\x20на\x20скорости\x20\x31\x39\x32\x30\x30\x2e\x20\x20Для\x20мод"
        "емов\x20\x32\x38\x38\x30\x30\x20бод\x20используйте\x20соединение\x20на\x20скорост"
        "и\x20\x33\x38\x34\x30\x30\x20бод\x2e",
      "\x7bОтменить\x7d\x0a\x0aОтменить\x20и\x20вернуться\x20в\x20главное\x20меню\x2e"
};
char* gSetupComPortHelp[KB_SETUP_COM_PORT_HELP_COUNT] = {
      "\x7b\x43\x4f\x4d\x20\x31\x7d\x0a\x0aИспользовать\x20для\x20модемного\x20соединения\x20п"
        "орт\x20\x43\x4f\x4d\x20\x31\x2e",
      "\x7b\x43\x4f\x4d\x20\x32\x7d\x0a\x0aИспользовать\x20для\x20модемного\x20соединения\x20п"
        "орт\x20\x43\x4f\x4d\x20\x32\x2e",
      "\x7b\x43\x4f\x4d\x20\x33\x7d\x0a\x0aИспользовать\x20для\x20модемного\x20соединения\x20п"
        "орт\x20\x43\x4f\x4d\x20\x33\x2e",
      "\x7b\x43\x4f\x4d\x20\x34\x7d\x0a\x0aИспользовать\x20для\x20модемного\x20соединения\x20п"
        "орт\x20\x43\x4f\x4d\x20\x34\x2e",
      "\x7bОтменить\x7d\x0a\x0aОтменить\x20и\x20вернуться\x20в\x20главное\x20меню\x2e"
};
char* gSetupDCBaudHelp[KB_SETUP_DC_BAUD_HELP_COUNT] = {
      "\x7bСкорость\x20соединения\x20\x32\x34\x30\x30\x20бод\x2e\x7d\x0a\x0aДля\x20компьютеров"
        "\x20с\x20устаревшим\x20чипом\x20\x55\x41\x52\x54\x20\x38\x32\x35\x30\x20следует\x20использова"
        "ть\x20скорость\x20\x31\x39\x32\x30\x30\x20бод\x2c\x20а\x20для\x20компьютеров\x20с\x20более"
        "\x20современным\x20чипом\x20\x55\x41\x52\x54\x20\x31\x36\x35\x35\x30\x20\x2d\x20скорость\x20\x33\x38\x34\x30\x30\x20б"
        "од\x2e\x20Если\x20вы\x20не\x20уверены\x2c\x20какой\x20у\x20вас\x20чип\x2c\x20начните"
        "\x20с\x20более\x20низких\x20скоростей\x2e\x20В\x20большинстве\x20компьют"
        "еров\x2c\x20произведенных\x20в\x20\x31\x39\x39\x34\x20году\x20и\x20позднее\x2c\x20испол"
        "ьзуется\x20чип\x20\x55\x41\x52\x54\x20\x31\x36\x35\x35\x30\x2e",
      "\x7bСкорость\x20соединения\x20\x39\x36\x30\x30\x20бод\x2e\x7d\x0a\x0a\x20Для\x20компьютеро"
        "в\x20с\x20устаревшим\x20чипом\x20\x55\x41\x52\x54\x20\x38\x32\x35\x30\x20следует\x20использов"
        "ать\x20скорость\x20\x31\x39\x32\x30\x30\x20бод\x2c\x20а\x20для\x20компьютеров\x20с\x20боле"
        "е\x20современным\x20чипом\x20\x55\x41\x52\x54\x20\x31\x36\x35\x35\x30\x20\x2d\x20скорость\x20\x33\x38\x34\x30\x30\x20"
        "бод\x2e\x20Если\x20вы\x20не\x20уверены\x2c\x20какой\x20у\x20вас\x20чип\x2c\x20начнит"
        "е\x20с\x20более\x20низких\x20скоростей\x2e\x20В\x20большинстве\x20компью"
        "теров\x2c\x20произведенных\x20в\x20\x31\x39\x39\x34\x20году\x20и\x20позднее\x2c\x20испо"
        "льзуется\x20чип\x20\x55\x41\x52\x54\x20\x31\x36\x35\x35\x30\x2e",
      "\x7bСкорость\x20соединения\x20\x31\x39\x32\x30\x30\x20бод\x2e\x7d\x0a\x0a\x20Для\x20компьютер"
        "ов\x20с\x20устаревшим\x20чипом\x20\x55\x41\x52\x54\x20\x38\x32\x35\x30\x20следует\x20использо"
        "вать\x20скорость\x20\x31\x39\x32\x30\x30\x20бод\x2c\x20а\x20для\x20компьютеров\x20с\x20бол"
        "ее\x20современным\x20чипом\x20\x55\x41\x52\x54\x20\x31\x36\x35\x35\x30\x20\x2d\x20скорость\x20\x33\x38\x34\x30\x30"
        "\x20бод\x2e\x20Если\x20вы\x20не\x20уверены\x2c\x20какой\x20у\x20вас\x20чип\x2c\x20начни"
        "те\x20с\x20более\x20низких\x20скоростей\x2e\x20В\x20большинстве\x20компь"
        "ютеров\x2c\x20произведенных\x20в\x20\x31\x39\x39\x34\x20году\x20и\x20позднее\x2c\x20исп"
        "ользуется\x20чип\x20\x55\x41\x52\x54\x20\x31\x36\x35\x35\x30\x2e",
      "\x7bСкорость\x20соединения\x20\x33\x38\x34\x30\x30\x20бод\x2e\x7d\x0a\x0a\x20Для\x20компьютер"
        "ов\x20с\x20устаревшим\x20чипом\x20\x55\x41\x52\x54\x20\x38\x32\x35\x30\x20следует\x20использо"
        "вать\x20скорость\x20\x31\x39\x32\x30\x30\x20бод\x2c\x20а\x20для\x20компьютеров\x20с\x20бол"
        "ее\x20современным\x20чипом\x20\x55\x41\x52\x54\x20\x31\x36\x35\x35\x30\x20\x2d\x20скорость\x20\x33\x38\x34\x30\x30"
        "\x20бод\x2e\x20Если\x20вы\x20не\x20уверены\x2c\x20какой\x20у\x20вас\x20чип\x2c\x20начни"
        "те\x20с\x20более\x20низких\x20скоростей\x2e\x20В\x20большинстве\x20компь"
        "ютеров\x2c\x20произведенных\x20в\x20\x31\x39\x39\x34\x20году\x20и\x20позднее\x2c\x20исп"
        "ользуется\x20чип\x20\x55\x41\x52\x54\x20\x31\x36\x35\x35\x30\x2e",
      "\x7bОтменить\x7d\x0a\x0aОтменить\x20и\x20вернуться\x20в\x20главное\x20меню\x2e"
};
char* gSetupDCComPortHelp[KB_SETUP_DC_COM_PORT_HELP_COUNT] = {
      "\x7b\x43\x4f\x4d\x20\x31\x7d\x0a\x0aИспользовать\x20для\x20прямого\x20соединения\x20пор"
        "т\x20\x43\x4f\x4d\x20\x31\x2e",
      "\x7b\x43\x4f\x4d\x20\x32\x7d\x0a\x0aИспользовать\x20для\x20прямого\x20соединения\x20пор"
        "т\x20\x43\x4f\x4d\x20\x32\x2e",
      "\x7b\x43\x4f\x4d\x20\x33\x7d\x0a\x0aИспользовать\x20для\x20прямого\x20соединения\x20пор"
        "т\x20\x43\x4f\x4d\x20\x33\x2e",
      "\x7b\x43\x4f\x4d\x20\x34\x7d\x0a\x0aИспользовать\x20для\x20прямого\x20соединения\x20пор"
        "т\x20\x43\x4f\x4d\x20\x34\x2e",
      "\x7bОтменить\x7d\x0a\x0aОтменить\x20и\x20вернуться\x20в\x20главное\x20меню\x2e"
};
char* gSetupHotSeatGameHelp[KB_SETUP_HOT_SEAT_HELP_COUNT] = {
      "\x7b\x32\x20игрока\x7d\x0a\x0aИграть\x20с\x20\x32\x20людьми\x20и\x2c\x20опционально\x2c\x20до"
        "\x20\x34\x20дополнительных\x20компьютерных\x20игроков\x2e",
      "\x7b\x33\x20игрока\x7d\x0a\x0aИграть\x20с\x20\x33\x20людьми\x20и\x2c\x20опционально\x2c\x20до"
        "\x20\x33\x20дополнительных\x20компьютерных\x20игроков\x2e",
      "\x7b\x34\x20игрока\x7d\x0a\x0aИграть\x20с\x20\x34\x20людьми\x20и\x2c\x20опционально\x2c\x20до"
        "\x20\x32\x20дополнительных\x20компьютерных\x20игроков\x2e",
      "\x7b\x35\x20игроков\x7d\x0a\x0a\x20Играть\x20с\x20\x35\x20людьми\x20и\x2c\x20опционально\x2c\x20"
        "с\x20\x31\x20компьютерным\x20игроком\x2e",
      "\x7b\x36\x20игроков\x7d\x0a\x0a\x20Играть\x20с\x20\x36\x20людьми\x2e",
      "\x7bОтменить\x7d\x0a\x0aОтменить\x20и\x20вернуться\x20в\x20главное\x20меню\x2e"
};
char* gSetupModemGameHelp[KB_SETUP_MODEM_HELP_COUNT] = {
      "\x7bСервер\x7d\x0a\x0aСервер\x20задает\x20настройки\x20игры\x2e\x20Может\x20бы"
        "ть\x2c\x20только\x20один\x20хост\x20в\x20одном\x20сетевом\x20соединении\x2e",
      "\x7bГость\x7d\x0a\x0aГость\x20ожидает\x2c\x20пока\x20сервер\x20задаст\x20настр"
        "ойки\x20игры\x2c\x20после\x20чего\x20он\x20автоматически\x20вступит\x20в"
        "\x20игру\x2e",
      "\x7bНастройки\x7d\x0a\x0aИзменить\x20конфигурацию\x20модема\x2e",
      "\x7bОтменить\x7d\x0a\x0aОтменить\x20и\x20вернуться\x20в\x20главное\x20меню\x2e"
};
char* gSetupDCGameHelp[KB_SETUP_DIRECT_CONNECT_HELP_COUNT] = {
      "\x7bСервер\x7d\x0a\x0aСервер\x20задает\x20настройки\x20игры\x2e",
      "\x7bГость\x7d\x0a\x0aГость\x20ожидает\x2c\x20пока\x20сервер\x20задаст\x20настр"
        "ойки\x20игры\x2e",
      "\x7bНастройки\x7d\x0a\x0aИзменить\x20конфигурацию\x20модема\x2e",
      "\x7bОтменить\x7d\x0a\x0aОтменить\x20и\x20вернуться\x20в\x20главное\x20меню\x2e"
};
char* gSetupMultiPlayerGameHelp[KB_SETUP_MULTIPLAYER_HELP_COUNT] = {
      "\x7bЗа\x20одной\x20машиной\x7d\x0a\x0aИграть\x20за\x20одной\x20машиной\x2c\x20где"
        "\x20от\x20\x32\x20до\x20\x34\x20игроков\x20людей\x2e",
      "\x7bЛокальная\x20сеть\x7d\x0a\x0aИграть\x20по\x20сети\x2c\x20где\x20двое\x20игрок"
        "ов\x20играют\x20по\x20локальной\x20сети\x2c\x20сидя\x20за\x20своими\x20комп"
        "ьютерами\x2e",
      "\x7bМодем\x7d\x0a\x0aДвое\x20игроков\x20играют\x20через\x20модемы\x20сидя\x20з"
        "а\x20своими\x20компьютерами\x2e",
      "\x7bПрямое\x20соединение\x7d\x0a\x0aДвое\x20игроков\x20играют\x20через\x20н"
        "оль\x2dмодем\x20сидя\x20за\x20своими\x20компьютерами\x2e",
      "\x7bОтменить\x7d\x0a\x0aОтменить\x20и\x20вернуться\x20в\x20главное\x20меню\x2e"
};
char* gSetupNetworkGameHelp[KB_SETUP_NETWORK_HELP_COUNT] = {
      "\x7bСервер\x7d\x0a\x0aОпределяет\x20настройки\x20игры\x2e\x20Может\x20быть\x20"
        "только\x20один\x20сервер\x20в\x20одном\x20соединении\x2e",
      "\x7bГость\x7d\x0a\x0a\x20Гость\x20ожидает\x2c\x20пока\x20сервер\x20задаст\x20наст"
        "ройки\x20игры\x2c\x20после\x20чего\x20он\x20автоматически\x20вступит\x20"
        "в\x20игру\x2e\x20В\x20игре\x20через\x20\x54\x43\x50\x2f\x49\x50\x20и\x20\x49\x50\x58\x20может\x20быть\x20нес"
        "колько\x20гостей\x2e\x20В\x20игре\x20через\x20\x4e\x65\x74\x42\x49\x4f\x53\x20\x2d\x20только\x20\x31\x2e",
      "\x7bОтменить\x7d\x0a\x0aОтменить\x20и\x20вернуться\x20в\x20главное\x20меню\x2e"
};
char* gSetupNetworkGame2Help[KB_SETUP_NETWORK_SECOND_HELP_COUNT] = {
      "\x7b\x49\x50\x58\x7d\x0a\x0a\x49\x50\x58\x20является\x20часто\x20используемым\x20сетевым\x20п"
        "ротоколом\x20для\x20\x57\x69\x6e\x64\x6f\x77\x73\x2e\x20По\x20\x49\x50\x58\x20могут\x20играть\x20до\x20\x36\x20"
        "человек\x20одновременно\x2e\x20Протокол\x20\x49\x50\x58\x20поддерживает\x20"
        "только\x20версия\x20игры\x2c\x20работающая\x20под\x20\x57\x69\x6e\x64\x6f\x77\x73\x20\x39\x35\x2e",
      "\x7b\x54\x43\x50\x2f\x49\x50\x7d\x0a\x0aПротокол\x20\x54\x43\x50\x2f\x49\x50\x20наиболее\x20часто\x20использ"
        "уется\x20для\x20соединения\x20компьютеров\x20через\x20Интернет\x2e"
        "\x20По\x20\x54\x43\x50\x2f\x49\x50\x20могут\x20играть\x20до\x20\x36\x20человек\x20одновременн"
        "о\x2e\x20Протокол\x20\x54\x43\x50\x2f\x49\x50\x20поддерживает\x20только\x20версия\x20иг"
        "ры\x2c\x20работающая\x20под\x20\x57\x69\x6e\x64\x6f\x77\x73\x2e",
      "\x7b\x4e\x45\x54\x42\x69\x6f\x73\x7d\x0a\x0aПротокол\x20\x4e\x45\x54\x42\x69\x6f\x73\x20является\x20единственно"
        "\x20возможным\x20для\x20компьютеров\x2c\x20работающим\x20под\x20\x44\x4f\x53\x2c\x20"
        "но\x20может\x20быть\x20использован\x20и\x20с\x20\x57\x69\x6e\x64\x6f\x77\x73\x20\x39\x35\x2e\x20\x20Этот\x20"
        "протокол\x20обеспечивает\x20соединение\x20не\x20более\x20двух\x20и"
        "гроков\x2e\x20Мы\x20рекомендуем\x20использовать\x20протокол\x20\x49\x50\x58"
        "\x2e",
      "\x7bОтмена\x7d\x0a\x0aЗакрыть\x20меню\x2e"
};
char* gSetupGameHelp[KB_SETUP_GAME_HELP_COUNT] = {
      "\x7bОбычная\x20игра\x7d\x0a\x0aОдиночная\x20игра\x20на\x20отдельной\x20карт"
        "е\x2e",
      "\x7bКампания\x7d\x0a\x0aОдиночная\x20игра\x20на\x20серии\x20карт\x2e",
      "\x7bСетевая\x20игра\x7d\x0a\x0aСетевая\x20игра\x2c\x20где\x20несколько\x20игро"
        "ков\x2dлюдей\x20сражаются\x20друг\x20против\x20друга\x20на\x20одной\x20к"
        "арте\x2e",
      "\x7bОтменить\x7d\x0a\x0aОтменить\x20и\x20вернуться\x20в\x20главное\x20меню\x2e"
};
char* cBattleResults[KB_BATTLE_RESULT_TEXT_COUNT] = {
      "Враг\x20сдался\x21",
      "Враг\x20повержен\x21",
      "Великая\x20победа\x21",
      "\x0a\x0aЗа\x20мужество\x2c\x20проявленное\x20в\x20бою\x2c\x20\x25\x73\x20получает\x20\x25\x64"
        "\x20оч\x2e\x20опыта\x2e",
      "\x25\x73\x20сдается\x20врагу\x20и\x20отступает\x20с\x20позором\x2e",
      "\x25\x73\x20трусливо\x20бежит\x20с\x20поля\x20боя\x2e",
      "Ваши\x20войска\x20потерпели\x20поражение\x20и\x20\x25\x73\x20покидает\x20ва"
        "с\x2e",
      "Ваши\x20силы\x20сдались\x20врагу\x20и\x20отступили\x20с\x20позором\x2e",
      "Ваши\x20трусливые\x20войска\x20бежали\x20с\x20поля\x20боя\x2e",
      "Ваши\x20войска\x20потерпели\x20поражение\x2e",
      "\x0a\x0aЗа\x20мужество\x2c\x20проявленное\x20в\x20бою\x2c\x20\x25\x73\x20получает\x20\x25\x64"
        "\x20оч\x2e\x20опыта\x2c\x20и\x20получает\x20\x25\x64\x20уровень\x28я\x29\x2e"
};
char* cMoraleInfo[KB_MORALE_INFO_TEXT_COUNT] = {
      "\x7bВысокая\x20мораль\x7d\x0a\x0aВысокая\x20мораль\x20может\x20дать\x20в\x20бо"
        "ю\x20вашим\x20бойцам\x20дополнительную\x20атаку\x2e",
      "\x7bОбычная\x20мораль\x7d\x0a\x0aС\x20обычной\x20моралью\x20ваши\x20армии\x20н"
        "икогда\x20не\x20получат\x20дополнительную\x20атаку\x20и\x20не\x20буду"
        "т\x20прокляты\x2e",
      "\x7bПлохая\x20мораль\x7d\x0a\x0aПлохая\x20мораль\x20может\x20привести\x20к\x20"
        "потере\x20хода\x20в\x20бою\x20вашими\x20бойцами\x2e",
      "\x25\x73\x0a\x0a\x0aМодификаторы\x20морали\x3a",
      "\x0aБонус\x20рыцаря\x20\x2b\x31",
      "\x0a\x25\x73\x20со\x20своей\x20армией\x20\x2b\x31",
      "\x0aВоины\x20\x33\x20рас\x20\x2d\x31",
      "\x0aВоины\x20\x34\x20рас\x20\x2d\x32",
      "\x0aМедаль\x20отваги\x20\x2b\x31",
      "\x0aМедаль\x20мужества\x20\x2b\x31",
      "\x0aМедаль\x20доблести\x20\x2b\x31",
      "\x0aМедаль\x20почета\x20\x2b\x31",
      "\x0aСимвол\x20неудачи\x20\x2d\x32",
      "\x0aПосещен\x20буй\x20\x2b\x31",
      "\x0aПосещен\x20оазис\x20\x2b\x31",
      "\x0aПосещен\x20храм\x20\x2b\x32",
      "\x0aРасхититель\x20гробниц\x20\x2d\x31",
      "\x0aРасхититель\x20обломков\x20\x2d\x31",
      "\x0aТрусость\x20в\x20бою\x20\x25\x64",
      "\x0aНет",
      "\x0aВоины\x20\x35\x20рас\x20\x2d\x33",
      "\x0aВся\x20армия\x20одна\x20нежить\x2c\x20мораль\x20не\x20важна\x2e",
      "\x0aВ\x20армии\x20нежить\x20\x2d\x31",
      "\x0aПосещена\x20промоина\x20\x2b\x31",
      "\x0aРасхититель\x20кораблей\x20\x2d\x31",
      "\x0aКолизей\x20варваров\x20\x2b\x32",
      "\x0aТаверна\x20\x2b\x31",
      "\x0aЛидерство\x20\x31\x2dй\x20ступени\x20\x2b\x31",
      "\x0aЛидерство\x20\x32\x2dй\x20ступени\x20\x2b\x32",
      "\x0aЛидерство\x20\x33\x2dй\x20ступени\x20\x2b\x33",
      "\x0aБонус\x20мачты\x20на\x20море\x20\x2b\x31",
      "\x0aБоевое\x20одеяние\x20Андурана\x20дает\x20максимальную\x20морал"
        "ь\x2e"
};
char* cMapSize[KB_MAP_SIZE_TEXT_COUNT] = {  "Маленькая",   "Средняя",   "Большая",   "Огромная"};
char* cDifficulty[KB_DIFFICULTY_TEXT_COUNT] =
    {  "Легкая",   "Обычная",   "Высокая",   "Эксперт",   "Невозможно\x21"};
char* cStartDifficulty[KB_START_DIFFICULTY_TEXT_COUNT] = {  "Легкая",   "Обычная",   "Тяжелая",   "Эксперт"};
char* cCampaignLeaders[KB_CAMPAIGN_LEADER_TEXT_COUNT] =
    {  "Лорд\x20Айронфист",   "Лорд\x20Слэйер",   "Королева\x20Ламанда",   "Лорд\x20Аламар"};
char* cWinText[KB_WIN_TEXT_COUNT] =
    {  "Дней\x3a",   "Очки\x3a",   "Сложность\x3a",   "Счет\x3a",   "Ранг\x3a"};
char* cHumanDifficulty[KB_HUMAN_DIFFICULTY_TEXT_COUNT] =
    {  "Человек\x0a",   "Человек\x0aЛегкая\x20игра",   "Человек\x0aОбычная\x20игра",   "Человек\x0aТяжелая\x20игра",   "Человек\x0aЭксперт"};
char* cHumanInfoDifficulty[KB_HUMAN_INFO_DIFFICULTY_TEXT_COUNT] =
    {  "Чел\x2e\x2d",   "Чел\x2e\x2dЛегкая\x20игра",   "Чел\x2e\x2dОбычная\x20игра",   "Чел\x2e\x2dТяжелая\x20игра",   "Чел\x2e\x2dЭксперт"};
char* musicQualityText[KB_MUSIC_QUALITY_TEXT_COUNT] =
    {  "\x4d\x49\x44\x49",   "\x43\x44\x2dстерео\x20без\x20вокала",   "\x43\x44\x2dстерео\x20с\x20вокалом"};
char* gSpellDesc[KB_SPELL_TEXT_COUNT] = {
      "\x7bОгненный\x20шар\x7d\x0a\x0aОгромный\x20огненный\x20шар\x20взрывается"
        "\x20над\x20выбранным\x20участком\x20поля\x20боя\x2c\x20поражая\x20все\x20на"
        "ходящиеся\x20поблизости\x20отряды\x2e",
      "\x7bОгненный\x20удар\x7d\x0a\x0aУсовершенствованный\x20вариант\x20огн"
        "енного\x20шара\x2e\x20Огненный\x20удар\x20поражает\x20отряды\x2c\x20нахо"
        "дящиеся\x20в\x20радиусе\x20не\x20одного\x2c\x20а\x20двух\x20полей\x20от\x20эпи"
        "центра\x2e",
      "\x7bМолния\x7d\x0a\x0aМощный\x20электрический\x20разряд\x20поражает\x20в"
        "ыбранный\x20отряд\x20противника\x2e",
      "\x7bЦепь\x20молний\x7d\x0a\x0aЭлектрический\x20разряд\x20поражает\x20выб"
        "ранный\x20отряд\x20противника\x2c\x20затем\x20ближайший\x20к\x20нему\x20"
        "отряд\x20с\x20половинной\x20силой\x2c\x20затем\x20следующий\x20отряд\x20"
        "\x63\x20еще\x20вдвое\x20меньшей\x20силой\x2c\x20и\x20так\x20далее\x20до\x20тех\x20по"
        "р\x2c\x20пока\x20не\x20уходит\x20в\x20землю\x2e\x20Будьте\x20осторожны\x3a\x20это"
        "\x20заклинание\x20может\x20поразить\x20и\x20ваши\x20собственные\x20от"
        "ряды\x21",
      "\x7bТелепорт\x7d\x0a\x0aМгновенно\x20перемещает\x20выбранный\x20отряд"
        "\x20в\x20любую\x20свободную\x20точку\x20на\x20поле\x20боя\x2e",
      "\x7bЛечение\x7d\x0a\x0aНейтрализует\x20все\x20враждебные\x20заклинани"
        "я\x2c\x20примененные\x20против\x20одного\x20из\x20ваших\x20отрядов\x20и\x20"
        "восстанавливает\x20по\x20\x35\x20единиц\x20здоровья\x20в\x20расчете\x20н"
        "а\x20каждый\x20уровень\x20магических\x20способностей\x20героя\x2e",
      "\x7bОбщее\x20лечение\x7d\x0a\x0aНейтрализует\x20враждебные\x20заклина"
        "ния\x2c\x20примененные\x20против\x20всех\x20ваших\x20отрядов\x20и\x20вос"
        "станавливает\x20по\x20\x35\x20единиц\x20здоровья\x20у\x20каждого\x20суще"
        "ства\x20за\x20каждый\x20уровень\x20магических\x20способностей\x20г"
        "ероя\x2e",
      "\x7bВоскрешение\x7d\x0a\x0aДо\x20конца\x20сражения\x20воскрешает\x20воин"
        "ов\x20в\x20отряде\x2c\x20которому\x20был\x20нанесен\x20урон\x2e",
      "\x7bИстинное\x20воскрешение\x7d\x0a\x0aНавсегда\x20воскрешает\x20воин"
        "ов\x20в\x20отряде\x2c\x20которому\x20был\x20нанесен\x20урон\x2e",
      "\x7bУскорение\x7d\x0a\x0aУвеличивает\x20дальность\x20передвижения\x20"
        "любого\x20отряда\x20на\x20\x32\x20единицы\x2e",
      "\x7bОбщее\x20ускорение\x7d\x0a\x0aУвеличивает\x20дальность\x20передви"
        "жения\x20всех\x20ваших\x20отрядов\x20на\x20\x32\x20единицы\x2e",
      "\x7bЗамедление\x7d\x0a\x0aВдвое\x20уменьшает\x20дальность\x20передвиж"
        "ения\x20выбранного\x20отряда\x20противника\x2e",
      "\x7bОбщее\x20замедление\x7d\x0a\x0aВдвое\x20снижает\x20дальность\x20пере"
        "мещения\x20всех\x20отрядов\x20противника\x2e",
      "\x7bОслепление\x7d\x0a\x0aЗатуманивает\x20взоры\x20воинов\x20выбранно"
        "го\x20отряда\x20и\x20тем\x20самым\x20не\x20позволяет\x20им\x20перемещать"
        "ся\x20по\x20полю\x20боя\x2e",
      "\x7bБлагословение\x7d\x0a\x0aУвеличивает\x20до\x20максимума\x20урон\x2c\x20"
        "наносимый\x20выбранным\x20отрядом\x2e",
      "\x7bОбщее\x20благословение\x7d\x0a\x0aУвеличивает\x20до\x20максимума\x20"
        "урон\x2c\x20наносимый\x20всеми\x20вашими\x20отрядами\x2e",
      "\x7bКаменная\x20кожа\x7d\x0a\x0aВолшебным\x20образом\x20повышает\x20защи"
        "щенность\x20выбранного\x20отряда\x2e",
      "\x7bСтальная\x20кожа\x7d\x0a\x0aПовышает\x20защищенность\x20выбранног"
        "о\x20отряда\x2e\x20Усовершенствованный\x20вариант\x20заклинания"
        "\x20Каменная\x20кожа\x2e",
      "\x7bПроклятие\x7d\x0a\x0aУменьшает\x20до\x20минимума\x20урон\x2c\x20причиня"
        "емый\x20выбранным\x20отрядом\x20противника\x2e",
      "\x7bОбщее\x20проклятие\x7d\x0a\x0aУменьшает\x20до\x20минимума\x20урон\x2c\x20п"
        "ричиняемый\x20всеми\x20отрядами\x20противника\x2e",
      "\x7bСвятое\x20слово\x7d\x0a\x0aНаносит\x20урон\x20всей\x20нежити\x20на\x20поле"
        "\x20боя\x2e",
      "\x7bСвятой\x20глас\x7d\x0a\x0aНаносит\x20урон\x20всей\x20нежити\x20на\x20поле\x20"
        "боя\x2e\x20Усовершенствованный\x20вариант\x20заклинания\x20Свят"
        "ое\x20слово\x2e",
      "\x7bАнтимагия\x7d\x0a\x0aЗащищает\x20выбранный\x20отряд\x20от\x20враждеб"
        "ных\x20заклинаний\x2e",
      "\x7bСнятие\x20чар\x7d\x0a\x0aСнимает\x20все\x20чары\x20с\x20выбранного\x20отря"
        "да\x2e",
      "\x7bОбщее\x20снятие\x20чар\x7d\x0a\x0aСнимает\x20все\x20чары\x20со\x20всех\x20отр"
        "ядов\x2e",
      "\x7bВолшебная\x20стрела\x7d\x0a\x0aВолшебная\x20стрела\x20поражает\x20вы"
        "бранный\x20отряд\x20противника\x2e",
      "\x7bБерсерк\x7d\x0a\x0aЗаставляет\x20выбранный\x20отряд\x20противника"
        "\x20нападать\x20на\x20ближайший\x20к\x20нему\x20отряд\x2e",
      "\x7bАрмагеддон\x7d\x0a\x0aУжасный\x20катаклизм\x20обрушивается\x20на\x20"
        "поле\x20боя\x2c\x20нанося\x20жестокий\x20урон\x20всем\x20участникам\x20с"
        "ражения\x2e",
      "\x7bБуря\x20стихий\x7d\x0a\x0aСилы\x20стихий\x20обрушиваются\x20на\x20поле\x20"
        "боя\x2c\x20нанося\x20урон\x20всем\x20участникам\x20сражения\x2e",
      "\x7bЗвездопад\x7d\x0a\x0aЗвездопад\x20поражает\x20выбранный\x20участо"
        "к\x20поля\x20боя\x2c\x20нанося\x20урон\x20всем\x20находящимся\x20поблизо"
        "сти\x20участникам\x20сражения\x2e",
      "\x7bПаралич\x7d\x0a\x0aОтряд\x2c\x20против\x20которого\x20направлено\x20это"
        "\x20заклинание\x2c\x20поражает\x20паралич\x2c\x20и\x20он\x20теряет\x20спосо"
        "бность\x20передвигаться\x20или\x20отвечать\x20на\x20удары\x2e",
      "\x7bГипноз\x7d\x0a\x0aВыбранный\x20отряд\x20противника\x20переходит\x20п"
        "од\x20контроль\x20вашего\x20героя\x20на\x20один\x20ход\x2c\x20если\x20его\x20с"
        "уммарное\x20здоровье\x20не\x20превышает\x20магических\x20способ"
        "ностей\x20героя\x2c\x20умноженных\x20на\x20\x32\x35\x2e",
      "\x7bХладный\x20луч\x7d\x0a\x0aВысасывает\x20тепло\x20жизни\x20из\x20выбранн"
        "ого\x20отряда\x20противника\x2e",
      "\x7bКольцо\x20стужи\x7d\x0a\x0aВысасывает\x20тепло\x20жизни\x20из\x20всех\x20о"
        "трядов\x20вокруг\x20эпицентра\x20заклинания\x2c\x20за\x20исключени"
        "ем\x20находящегося\x20в\x20самом\x20эпицентре\x2e",
      "\x7bРазрушительный\x20луч\x7d\x0a\x0aПонижает\x20защиту\x20выбранного"
        "\x20отряда\x20противника\x20на\x20\x33\x20единицы\x2e",
      "\x7bДрожь\x20смерти\x7d\x0a\x0aНаносит\x20урон\x20всем\x20отрядам\x20живых\x20"
        "воинов\x20в\x20сражении\x2c\x20но\x20не\x20действует\x20на\x20нежить\x2e",
      "\x7bВолна\x20смерти\x7d\x0a\x0aНаносит\x20урон\x20всем\x20отрядам\x20живых\x20"
        "воинов\x20в\x20сражении\x2c\x20но\x20не\x20действует\x20на\x20нежить\x2e\x20Ус"
        "овершенствованный\x20вариант\x20заклинания\x20Дрожь\x20смерт"
        "и\x2e",
      "\x7bУбийца\x20драконов\x7d\x0a\x0aЗначительно\x20увеличивает\x20урон\x2c"
        "\x20наносимый\x20выбранным\x20отрядом\x20в\x20бою\x20против\x20дракон"
        "ов\x2e",
      "\x7bЖажда\x20крови\x7d\x0a\x0aУвеличивает\x20урон\x2c\x20наносимый\x20выбра"
        "нным\x20отрядом\x2e",
      "\x7bПоднять\x20мертвых\x7d\x0a\x0aНавсегда\x20\x22воскрешает\x22\x20из\x20ране"
        "нных\x20или\x20уничтоженных\x20отрядов\x20нежити\x2e",
      "\x7bФантом\x7d\x0a\x0aЗаклинание\x20создает\x20призрачный\x20отряд\x2c\x20к"
        "оторый\x20является\x20двойником\x20существующего\x20отряда\x2e\x20"
        "Призрачный\x20отряд\x20наносит\x20противнику\x20такой\x20же\x20уро"
        "н\x2c\x20как\x20и\x20настоящий\x2c\x20но\x20исчезает\x2c\x20если\x20ему\x20был\x20на"
        "несен\x20хотя\x20бы\x20минимальный\x20урон\x2e",
      "\x7bЩит\x7d\x0a\x0aВдвое\x20уменьшает\x20урон\x2c\x20получаемый\x20выбранны"
        "м\x20отрядом\x20от\x20стрелковых\x20атак\x20противника\x2e",
      "\x7bОбщий\x20щит\x7d\x0a\x0aВдвое\x20уменьшает\x20урон\x2c\x20получаемый\x20вс"
        "еми\x20отрядами\x20от\x20стрелковых\x20атак\x20противника\x2e",
      "\x7bЗемной\x20элементал\x7d\x0a\x0aЗаклинание\x20вызывает\x20отряд\x20эл"
        "ементалов\x20земли\x2c\x20которые\x20присоединяются\x20к\x20вашей\x20"
        "армии\x2e",
      "\x7bВоздушный\x20элементал\x7d\x0a\x0aЗаклинание\x20вызывает\x20отряд"
        "\x20элементалов\x20воздуха\x2c\x20которые\x20присоединяются\x20к\x20в"
        "ашей\x20армии\x2e",
      "\x7bОгненный\x20элементал\x7d\x0a\x0aЗаклинание\x20вызывает\x20отряд\x20"
        "элементалов\x20огня\x2c\x20которые\x20присоединяются\x20к\x20вашей"
        "\x20армии\x2e",
      "\x7bВодный\x20элементал\x7d\x0a\x0aЗаклинание\x20вызывает\x20отряд\x20эл"
        "еманталов\x20воздуха\x2c\x20которей\x20присоединяются\x20к\x20ваше"
        "й\x20армии\x2e",
      "\x7bЗемлетрясение\x7d\x0a\x0aНаносит\x20ущерб\x20крепостным\x20стенам"
        "\x2e",
      "\x7bПоказать\x20шахты\x7d\x0a\x0aДелает\x20видимыми\x20все\x20шахты\x20на\x20и"
        "гровой\x20карте\x2e",
      "\x7bПоказать\x20ресурсы\x7d\x0a\x0aПоказывает\x20все\x20ресурсы\x20на\x20иг"
        "ровой\x20карте\x2e",
      "\x7bПоказать\x20артефакты\x7d\x0a\x0aДелает\x20видимыми\x20все\x20артефа"
        "кты\x20на\x20игровой\x20карте\x2e",
      "\x7bПоказать\x20города\x7d\x0a\x0aДелает\x20видимыми\x20все\x20города\x20и\x20"
        "замки\x20на\x20игровой\x20карте\x2e",
      "\x7bПоказать\x20героев\x7d\x0a\x0aДелает\x20видимыми\x20всех\x20героев\x20н"
        "а\x20игровой\x20карте\x2e",
      "\x7bПоказать\x20все\x7d\x0a\x0aДелает\x20видимыми\x20все\x20объекты\x20на\x20и"
        "гровой\x20карте\x2e",
      "\x7bОпознать\x20героев\x7d\x0a\x0aПозволяет\x20получить\x20подробную\x20"
        "информацию\x20о\x20героях\x20противника\x2e",
      "\x7bПризвать\x20корабль\x7d\x0a\x0aПеремещает\x20ваш\x20ближайший\x20нез"
        "анятый\x20корабль\x20в\x20ближайшую\x20к\x20вам\x20точку\x20побережья"
        "\x2e\x20Вашим\x20считается\x20корабль\x2c\x20который\x20вы\x20только\x20что"
        "\x20построили\x2c\x20либо\x20тот\x2c\x20на\x20котором\x20вы\x20плавали\x20посл"
        "едним\x2e",
      "\x7bПортал\x7d\x0a\x0aПереносит\x20героя\x20в\x20расположенную\x20поблиз"
        "ости\x20точку\x20на\x20карте\x2e",
      "\x7bВрата\x20города\x7d\x0a\x0aПереносит\x20героя\x20в\x20ближайший\x20прин"
        "адлежащий\x20игроку\x20город\x20или\x20замок\x2e",
      "\x7bПортал\x20города\x7d\x0a\x0aПереносит\x20героя\x20в\x20принадлежащий"
        "\x20игроку\x20город\x20или\x20замок\x20по\x20его\x20выбору\x2e",
      "\x7bВиденье\x7d\x0a\x0aЭто\x20заклинание\x20позволяет\x20предсказать\x20"
        "вероятный\x20исход\x20встречи\x20с\x20нейтральной\x20армией\x2e",
      "\x7bЗапустение\x7d\x0a\x0aНаводняет\x20принадлежащую\x20игроку\x20шах"
        "ту\x20призраками\x2c\x20после\x20чего\x20она\x20перестает\x20производ"
        "ить\x20ресурсы\x2e\x20\x28Не\x20доставайся\x20же\x20ты\x20никому\x21\x29",
      "\x7bСтража\x20земли\x7d\x0a\x0aОтряд\x20земных\x20элементалов\x20охраняе"
        "т\x20шахту\x20от\x20нападения\x20армий\x20противника\x2e",
      "\x7bСтража\x20воздуха\x7d\x0a\x0aОтряд\x20воздушных\x20элементалов\x20ох"
        "раняет\x20шахту\x20от\x20нападения\x20армий\x20противника\x2e",
      "\x7bСтража\x20огня\x7d\x0a\x0aОтряд\x20огненных\x20элементалов\x20охраня"
        "ет\x20шахту\x20от\x20нападения\x20армий\x20противника\x2e",
      "\x7bСтража\x20воды\x7d\x0a\x0aОтряд\x20водных\x20элементалов\x20охраняет"
        "\x20шахту\x20от\x20нападения\x20армий\x20противника\x2e"
};
char* gSpellNames[KB_SPELL_TEXT_COUNT] = {
      "Огненный\x20шар",
      "Огненный\x20взрыв",
      "Молния",
      "Цепь\x20молний",
      "Телепорт",
      "Лечение",
      "Общее\x20лечение",
      "Воскрешение",
      "Истинное\x20воскрешение",
      "Ускорение",
      "Общее\x20ускорение",
      "Замедление",
      "Общее\x20замедление",
      "Ослепление",
      "Благословение",
      "Общее\x20благословение",
      "Каменная\x20кожа",
      "Стальная\x20кожа",
      "Проклятие",
      "Общее\x20проклятие",
      "Святое\x20слово",
      "Святой\x20глас",
      "Антимагия",
      "Снятие\x20чар",
      "Общее\x20снятие\x20чар",
      "Волшебная\x20стрела",
      "Берсерк",
      "Армагеддон",
      "Буря\x20стихий",
      "Звездопад",
      "Паралич",
      "Гипноз",
      "Хладный\x20луч",
      "Кольцо\x20стужи",
      "Разрушительный\x20луч",
      "Дрожь\x20смерти",
      "Волна\x20смерти",
      "Убийца\x20драконов",
      "Жажда\x20крови",
      "Поднять\x20мертвых",
      "Фантом",
      "Щит",
      "Общий\x20щит",
      "Земной\x20элементал",
      "Воздушный\x20элементал",
      "Огненный\x20элементал",
      "Водный\x20элементал",
      "Землетрясение",
      "Показать\x20шахты",
      "Показать\x20ресурсы",
      "Показать\x20артефакты",
      "Показать\x20города",
      "Показать\x20героев",
      "Показать\x20все",
      "Опознать\x20героев",
      "Призвать\x20корабль",
      "Портал",
      "Врата\x20города",
      "Портал\x20города",
      "Виденье",
      "Запустение",
      "Страж\x20земли",
      "Страж\x20воздуха",
      "Страж\x20огня",
      "Страж\x20воды"
};
char* gSecondarySkillLevels[KB_SECONDARY_SKILL_LEVEL_TEXT_COUNT] =
    {  "\x31\x20ступени",   "\x32\x20ступени",   "\x33\x20ступени"};
char* gSecondarySkills[KB_SECONDARY_SKILL_TEXT_COUNT] = {
      "Следопыт",
      "Стрелок",
      "Логистика",
      "Разведка",
      "Дипломатия",
      "Навигация",
      "Лидерство",
      "Мудрость",
      "Мистицизм",
      "Удача",
      "Баллистика",
      "Орлиный\x20взор",
      "Некромантия",
      "Казначей"
};
char* gNeutralBuildingNames[KB_NEUTRAL_BUILDING_TEXT_COUNT] = {
      "Гильдия\x20магов",
      "Гильдия\x20воров",
      "Таверна",
      "Верфь",
      "Колодец",
      "Шатер",
      "Замок",
      "Статуя",
      "Левая\x20башня",
      "Правая\x20башня",
      "Рынок",
      "",
      "Ров",
      "",
      "Док\x20с\x20кораблем",
      "Дом\x20капитана",
      "",
      "",
      ""
};
char* gWellExtraNames[KB_WELL_EXTRA_NAME_COUNT] = {
      "Ферма",
      "Свалка\x20истории",
      "Хрустальный\x20сад",
      "Водопад",
      "Фруктовый\x20сад",
      "Груда\x20черепов",
      "Прирост\x20воинов\x20\x31\x20ур\x2e"
};
char* gSpecialBuildingNames[KB_SPECIAL_BUILDING_NAME_COUNT] =
    {  "Укрепления",   "Колизей",   "Радуга",   "Подземелье",   "Библиотека",   "Шторм",   "Специальная"};
char* gDwellingNames[(FACTION_COUNT)][KB_DWELLING_TYPE_COUNT] = {
    {  "Мазанка",
       "Стрельбище",
       "Кузница",
       "Оружейная",
       "Ристалище",
       "Собор",
       "Полигон",
       "Ковальня",
       "Арсенал",
       "Арена",
       "Храм",
       ""},
    {  "Хижина",
       "Халупа",
       "Логово",
       "Дом\x20огров",
       "Мост",
       "Пирамида",
       "Хибара",
       "",
       "Логово\x20огров",
       "Царь\x2dмост",
       "",
       ""},
    {  "Древо\x2dдом",
       "Избушка",
       "Стрельбище",
       "Стоунхендж",
       "Загон",
       "Алая\x20башня",
       "Хоромы",
       "Полигон",
       "Менгиры",
       "",
       "",
       ""},
    {  "Пещера",
       "Крипта",
       "Гнездо",
       "Лабиринт",
       "Болото",
       "Зеленая\x20башня",
       "",
       "",
       "Большой\x20лабиринт",
       "",
       "Красная\x20башня",
       "Черная\x20башня"},
    {  "Нора",
       "Хлев",
       "Литейный\x20цех",
       "Гнездовье",
       "Башня\x20магов",
       "Небесный\x20замок",
       "",
       "Фабрика",
       "",
       "Обитель\x20магов",
       "Небесный\x20чертог",
       ""},
    {  "Могильник",
       "Кладбище",
       "Пирамида",
       "Особняк",
       "Мавзолей",
       "Лаборатория",
       "Погост",
       "Великая\x20пирамида",
       "Цитадель",
       "Некрополь",
       "",
       ""}
};
char* cSecSkillDesc[(HERO_SKILL_COUNT)][SECONDARY_SKILL_VALUE_LEVEL_COUNT] = {
    {  "\x7bСледопыт\x20\x31\x20ступени\x7d\x0a\x0aУменьшает\x20замедление\x20при\x20п"
        "ередвижении\x20по\x20пересеченной\x20местности\x20на\x20\x32\x35\x20проц"
        "ентов\x2e",
       "\x7bСледопыт\x20\x32\x20ступени\x7d\x0a\x0aУменьшает\x20замедление\x20при\x20п"
         "ередвижении\x20по\x20пересеченной\x20местности\x20на\x20\x35\x30\x20проц"
         "ентов\x2e",
       "\x7bСледопыт\x20\x33\x20ступени\x7d\x0a\x0aПолностью\x20нейтрализует\x20зам"
         "едление\x20при\x20передвижении\x20по\x20пересеченной\x20местнос"
         "ти\x2e"},
    {  "\x7bСтрелок\x20\x31\x20ступени\x7d\x0a\x0aУвеличивает\x20на\x20\x31\x30\x20процентов"
        "\x20урон\x2c\x20наносимый\x20стреляющими\x20отрядами\x2e",
       "\x7bСтрелок\x20\x32\x20ступени\x7d\x0a\x0aУвеличивает\x20на\x20\x32\x35\x20процентов"
         "\x20урон\x2c\x20наносимый\x20стреляющими\x20отрядами\x2e",
       "\x7bСтрелок\x20\x33\x20ступени\x7d\x0a\x0aУвеличивает\x20на\x20\x35\x30\x20процентов"
         "\x20урон\x2c\x20наносимый\x20стреляющими\x20отрядами\x2e"},
    {  "\x7bЛогистика\x20\x31\x20ступени\x7d\x0a\x0aУвеличивает\x20запас\x20движени"
        "я\x20героя\x20на\x20\x31\x30\x20процентов\x2e",
       "\x7bЛогистика\x20\x32\x20ступени\x7d\x0a\x0aУвеличивает\x20запас\x20движени"
         "я\x20героя\x20на\x20\x32\x30\x20процентов\x2e",
       "\x7bЛогистика\x20\x33\x20ступени\x7d\x0a\x0aУвеличивает\x20запас\x20движени"
         "я\x20героя\x20на\x20\x33\x30\x20процентов\x2e"},
    {  "\x7bРазведка\x20\x31\x20ступени\x7d\x0a\x0aУвеличивает\x20на\x20\x31\x20клетку\x20ра"
        "диус\x20обзора\x20героя\x2e",
       "\x7bРазведка\x20\x32\x20ступени\x7d\x0a\x0aУвеличивает\x20на\x20\x32\x20клетки\x20ра"
         "диус\x20обзора\x20героя\x2e",
       "\x7bРазведка\x20\x33\x20ступени\x7d\x0a\x0aУвеличивает\x20на\x20\x33\x20клетки\x20ра"
         "диус\x20обзора\x20героя\x2e"},
    {  "\x7bДипломатия\x20\x31\x20ступени\x7d\x0a\x0aПозволяет\x20вести\x20перегово"
        "ры\x20с\x20отрядами\x20монстров\x2c\x20более\x20слабыми\x2c\x20чем\x20ваша\x20"
        "армия\x2e\x20На\x20таком\x20уровне\x20дипломатии\x20к\x20вам\x20может\x20пр"
        "исоединиться\x20до\x20\x31\x2f\x34\x20отряда\x20монстров\x2e",
       "\x7bДипломатия\x20\x32\x20ступени\x7d\x0a\x0aПозволяет\x20вести\x20перегово"
         "ры\x20с\x20отрядами\x20монстров\x2c\x20более\x20слабыми\x2c\x20чем\x20ваша\x20"
         "армия\x2e\x20На\x20таком\x20уровне\x20дипломатии\x20к\x20вам\x20может\x20пр"
         "исоединиться\x20до\x20\x31\x2f\x32\x20отряда\x20монстров\x2e",
       "\x7bДипломатия\x20\x33\x20ступени\x7d\x0a\x0aПозволяет\x20вести\x20перегово"
         "ры\x20с\x20отрядами\x20монстров\x2c\x20более\x20слабыми\x2c\x20чем\x20ваша\x20"
         "армия\x2e\x20На\x20таком\x20уровне\x20дипломатии\x20к\x20вам\x20может\x20пр"
         "исоединиться\x20весь\x20отряд\x20монстров\x2e"},
    {  "\x7bНавигация\x20\x31\x20ступени\x7d\x0a\x0aУвеличивает\x20на\x20\x31\x2f\x33\x20запас\x20"
        "движения\x20героя\x20при\x20передвижении\x20по\x20воде\x2e",
       "\x7bНавигация\x20\x32\x20ступени\x7d\x0a\x0aУвеличивает\x20на\x20\x32\x2f\x33\x20запас\x20"
         "движения\x20героя\x20при\x20передвижении\x20по\x20воде\x2e",
       "\x7bНавигация\x20\x33\x20ступени\x7d\x0a\x0aУдваивает\x20запас\x20движения\x20"
         "героя\x20при\x20передвижении\x20по\x20воде\x2e"},
    {  "\x7bЛидерство\x20\x31\x20ступени\x7d\x0a\x0aУвеличивает\x20на\x20\x31\x20единицу\x20"
        "мораль\x20войск\x20вашего\x20героя\x2e",
       "\x7bЛидерство\x20\x32\x20ступени\x7d\x0a\x0aУвеличивает\x20на\x20\x32\x20единицы\x20"
         "мораль\x20войск\x20вашего\x20героя\x2e",
       "\x7bЛидерство\x20\x33\x20ступени\x7d\x0a\x0aУвеличивает\x20на\x20\x33\x20единицы\x20"
         "мораль\x20войск\x20вашего\x20героя\x2e"},
    {  "\x7bМудрость\x20\x31\x20ступени\x7d\x0a\x0aПозволяет\x20вашему\x20герою\x20изу"
        "чать\x20заклинания\x20третьего\x20уровня\x2e",
       "\x7bМудрость\x20\x32\x20ступени\x7d\x0a\x0aПозволяет\x20вашему\x20герою\x20изу"
         "чать\x20заклинания\x20четвертого\x20уровня\x2e",
       "\x7bМудрость\x20\x33\x20ступени\x7d\x0a\x0aПозволяет\x20вашему\x20герою\x20изу"
         "чать\x20заклинания\x20пятого\x20уровня\x2e"},
    {  "\x7bМистицизм\x20\x31\x20ступени\x7d\x0a\x0aВаш\x20герой\x20восстанавливает"
        "\x20по\x20\x32\x20очка\x20магии\x20в\x20день\x2e",
       "\x7bМистицизм\x20\x32\x20ступени\x7d\x0a\x0aВаш\x20герой\x20восстанавливает"
         "\x20по\x20\x33\x20очка\x20магии\x20в\x20день\x2e",
       "\x7bМистицизм\x20\x33\x20ступени\x7d\x0a\x0aВаш\x20герой\x20восстанавливает"
         "\x20по\x20\x34\x20очка\x20магии\x20в\x20день\x2e"},
    {  "\x7bУдача\x20\x31\x20ступени\x7d\x0a\x0aУвеличивает\x20на\x20\x31\x20удачу\x20вашего"
        "\x20героя\x2e",
       "\x7bУдача\x20\x32\x20ступени\x7d\x0a\x0aУвеличивает\x20на\x20\x32\x20удачу\x20вашего"
         "\x20героя\x2e",
       "\x7bУдача\x20\x33\x20ступени\x7d\x0a\x0aУвеличивает\x20на\x20\x33\x20удачу\x20вашего"
         "\x20героя\x2e"},
    {  "\x7bБаллистика\x20\x31\x20ступени\x7d\x0a\x0aУвеличивает\x20точность\x20стр"
        "ельбы\x20\x20катапульты\x20вашего\x20героя\x20и\x20урон\x2c\x20наносимый"
        "\x20крепостным\x20стенам\x2e",
       "\x7bБаллистика\x20\x32\x20ступени\x7d\x0a\x0aКатапульта\x20вашего\x20героя\x20"
         "делает\x20дополнительный\x20выстрел\x3b\x20при\x20этом\x20увеличив"
         "ается\x20точность\x20ее\x20стрельбы\x20и\x20урон\x2c\x20наносимый\x20кре"
         "постным\x20стенам\x2e",
       "\x7bБаллистика\x20\x33\x20ступени\x7d\x0a\x0aКатапульта\x20вашего\x20героя\x20"
         "делает\x20дополнительный\x20выстрел\x3b\x20при\x20этом\x20каждый\x20в"
         "ыстрел\x20разрушает\x20любую\x20стену\x2c\x20за\x20исключением\x20укр"
         "епленных\x20стен\x20рыцарского\x20замка\x2e"},
    {  "\x7bОрлиный\x20взор\x20\x31\x20ступени\x7d\x0a\x0aДает\x20вашему\x20герою\x20\x32\x30\x2dп"
        "роцентный\x20шанс\x20выучить\x20любое\x20заклинание\x20первого\x20"
        "или\x20второго\x20уровней\x2c\x20примененное\x20против\x20него\x20в\x20б"
        "ою\x2e",
       "\x7bОрлиный\x20взор\x20\x32\x20ступени\x7d\x0a\x0aДает\x20вашему\x20герою\x20\x33\x30\x2dп"
         "роцентный\x20шанс\x20выучить\x20любое\x20заклинание\x20третьего"
         "\x20или\x20более\x20низких\x20уровней\x2c\x20примененное\x20против\x20не"
         "го\x20в\x20бою\x2e",
       "\x7bОрлиный\x20глаз\x20\x33\x20ступени\x7d\x0a\x0aДает\x20вашему\x20герою\x20\x34\x30\x2dп"
         "роцентный\x20шанс\x20выучить\x20любое\x20заклинание\x20четверто"
         "го\x20или\x20более\x20низких\x20уровней\x2c\x20примененное\x20против\x20"
         "него\x20в\x20бою\x2e"},
    {  "\x7bНекромантия\x20\x31\x20ступени\x7d\x0a\x0aВоскрешает\x20\x31\x30\x20процентов"
        "\x20существ\x2c\x20павших\x20на\x20поле\x20боя\x2c\x20и\x20превращает\x20их\x20в\x20"
        "скелеты\x20для\x20вашей\x20армии\x2e",
       "\x7bНекромантия\x20\x32\x20ступени\x7d\x0a\x0aВоскрешает\x20\x32\x30\x20процентов"
         "\x20существ\x2c\x20павших\x20на\x20поле\x20боя\x2c\x20и\x20превращает\x20их\x20в\x20"
         "скелеты\x20для\x20вашей\x20армии\x2e",
       "\x7bНекромантия\x20\x33\x20ступени\x7d\x0a\x0aВоскрешает\x20\x33\x30\x20процентов"
         "\x20существ\x2c\x20павших\x20на\x20поле\x20боя\x2c\x20и\x20превращает\x20их\x20в\x20"
         "скелеты\x20для\x20вашей\x20армии\x2e"},
    {  "\x7bКазначей\x20\x31\x20ступени\x7d\x0a\x0aВаш\x20герой\x20ежедневно\x20собира"
        "ет\x20со\x20своих\x20владений\x20налоги\x20в\x20размере\x20\x31\x30\x30\x20золоты"
        "х\x2e",
       "\x7bКазанчей\x20\x32\x20ступени\x7d\x0a\x0aВаш\x20герой\x20ежедневно\x20собира"
         "ет\x20со\x20своих\x20владений\x20налоги\x20в\x20размере\x20\x32\x35\x30\x20золоты"
         "х\x2e",
       "\x7bКазначей\x20\x33\x20ступени\x7d\x0a\x0a\x20Ваш\x20герой\x20ежедневно\x20собир"
         "ает\x20со\x20своих\x20владений\x20налоги\x20в\x20размере\x20\x35\x30\x30\x20золот"
         "ых\x2e"}
};
char* cBuildingInfoNeutral[KB_NEUTRAL_BUILDING_INFO_COUNT] = {
      "Гильдия\x20магов\x20позволяет\x20разучивать\x20новые\x20заклина"
        "ния\x20и\x20восстанавливает\x20запас\x20очков\x20магии\x2e",
      "Гильдия\x20воров\x20дает\x20информацию\x20о\x20врагах\x2e\x20Также\x2c\x20Г"
        "ильдия\x20воров\x20дает\x20разведывательную\x20информацию\x20о\x20"
        "вражеских\x20городах\x2e\x20Дополнительные\x20гильдии\x20дают\x20д"
        "ополнительную\x20информацию\x2e",
      "Таверна\x20увеличивает\x20мораль\x20бойцов\x2c\x20защищающих\x20за"
        "мок\x2e",
      "Верфь\x20позволяет\x20строить\x20корабли\x2e",
      "Колодец\x20увеличивает\x20прирост\x20всех\x20воинов\x20на\x20\x32\x20в\x20н"
        "еделю\x2e",
      "Шатер\x20дает\x20рабочих\x2c\x20которые\x20могут\x20возвести\x20замок"
        "\x2e",
      "Замок\x20улучшает\x20защиту\x20города\x20и\x20увеличивает\x20доход"
        "\x20до\x20\x31\x30\x30\x30\x20золотых\x20в\x20день\x2e",
      "Статуя\x20увеличивает\x20доход\x20города\x20на\x20\x32\x35\x30\x20золотых\x20в"
        "\x20день\x2e",
      "Левая\x20башня\x20обеспечивает\x20в\x20бою\x20дополнительную\x20ог"
        "невую\x20мощь\x20замку\x2e",
      "Правая\x20башня\x20обеспечивает\x20в\x20бою\x20дополнительную\x20о"
        "гневую\x20мощь\x20замку\x2e",
      "Рынок\x20можно\x20использовать\x20для\x20перевода\x20одного\x20тип"
        "а\x20ресурсов\x20в\x20другой\x2e\x20Чем\x20больше\x20рынков\x20вы\x20контро"
        "лируете\x2c\x20тем\x20выгодней\x20цена\x2e",
      "",
      "Ров\x20замедляет\x20атаку\x20вражеских\x20воинов\x2e\x20Любой\x20воин"
        "\x2c\x20вошедший\x20в\x20ров\x2c\x20окончит\x20тут\x20свое\x20движение\x20и\x20ст"
        "анет\x20более\x20уязвимым\x20для\x20атаки\x2e",
      "",
      "Верфь\x20позволяет\x20строить\x20корабли\x2e",
      "Дом\x20капитана\x20позволяет\x20капитану\x20городской\x20стражи"
        "\x20организовать\x20защиту\x20замка\x20в\x20отсутствии\x20героя\x2e",
      "",
      "",
      ""
};
char* gBuildingInfoSpecial[KB_SPECIAL_BUILDING_INFO_COUNT] = {
      "Укрепления\x20увеличивают\x20прочность\x20стен\x2c\x20увеличива"
        "я\x20число\x20раундов\x2c\x20необходимых\x20для\x20полного\x20их\x20разр"
        "ушения\x2e",
      "Представления\x2c\x20проходимые\x20в\x20Колизее\x2c\x20увеличивают"
        "\x20мораль\x20защитников\x20замка\x20на\x20\x32\x20единицы\x2e",
      "Радуга\x20увеличивает\x20удачу\x20защитников\x20замка\x20на\x20\x32\x20е"
        "диницы\x2e",
      "Подземелье\x20увеличивает\x20доход\x20города\x20на\x20\x35\x30\x30\x20золот"
        "ых\x20в\x20день\x2e",
      "Библиотека\x20увеличивает\x20число\x20заклинаний\x2c\x20доступн"
        "ых\x20в\x20Гильдии\x20на\x20\x31\x20на\x20каждый\x20ее\x20этаж\x2e",
      "Шторм\x20добавляет\x20\x2b\x32\x20единицы\x20к\x20силе\x20заклинаний\x20защ"
        "итников\x20замка\x2e"
};
char* cDirections[KB_DIRECTION_TEXT_COUNT] = {
      "севернее",
      "северо\x2dвосточнее",
      "восточнее",
      "юго\x2dвосточнее",
      "южнее",
      "юго\x2dвосточнее",
      "западнее",
      "северо\x2dзападнее",
      "в\x20центре"
};
char* cRumourTerrainDescriptions[KB_RUMOUR_TERRAIN_DESCRIPTION_COUNT] = {
      "Темные\x20пучины\x20океана",
      "Зеленые\x20равнины",
      "Глубокие\x20снега",
      "Топкие\x20болота",
      "Застывшая\x20лава",
      "Бескрайние\x20пески",
      "Грязь",
      "Бесплодная\x20пустошь",
      "Побережье"
};
char* gInterfaceTypeText[KB_INTERFACE_TYPE_TEXT_COUNT] = {  "Разный",   "\x27Добрый\x27",   "\x27Злой\x27"};
char* cBWMouseText[KB_BW_MOUSE_TEXT_COUNT] = {  "Монохром",   "Цветной"};
char* combatSpeedText[KB_COMBAT_SPEED_TEXT_COUNT] = {  "Обычная",   "Высокая",   "Оч\x2e\x20высокая"};
char* combatMiniInfoText[KB_COMBAT_MINI_INFO_TEXT_COUNT] = {  "Нет",   "Только\x20чары",   "Полная"};
char* gcCommandLineHelp[KB_COMMAND_LINE_HELP_COUNT] = {
      "\x0a\x0a\x0a\x2a\x2a\x2a\x43\x6f\x6d\x6d\x61\x6e\x64\x20\x4c\x69\x6e\x65\x20\x48\x65\x6c\x70\x2a\x2a\x2a\x0a",
      "\x0a",
      "\x2f\x44\x30\x20\x2d\x20отключить\x20цифровой\x20звук\x0a",
      "\x2f\x4d\x30\x20\x2d\x20отключить\x20\x4d\x49\x44\x49\x20музыку\x0a",
      "\x2f\x52\x30\x20\x2d\x20отключить\x20музыку\x0a",
      "\x2f\x49\x30\x20\x2d\x20пропустить\x20интро\x0a",
      "\x0a",
      "\x0a",
      "Пример\x3a\x0a",
      "\x0a",
      "\x48\x45\x52\x4f\x45\x53\x32\x44\x20\x2f\x52\x30\x20\x2f\x49\x30\x0a",
      "\x0a",
      "Загрузить\x20\x44\x4f\x53\x20версию\x20Героев\x20\x32\x2e\x0a",
      "Звук\x20отключен\x20и\x20интро\x20пропущено\x2e\x0a"
};
char* cOverviewText[KB_OVERVIEW_TEXT_COUNT] =
    {  "Герой\x2fПараметры",   "Навыки",   "Артефакты",   "Города\x2fЗамки",   "Гарнизон",   "Доступно"};
char* cWinComError[KB_WIN_COM_ERROR_TEXT_COUNT] = {
      "Ошибка\x20передачи\x20данных\x20при\x20выполнении\x20функции\x20\x25\x73"
        "\x0a\x0aКод\x20ошибки\x3a\x20\x25\x64\x0aЗначение\x20ошибки\x3a\x20\x25\x73\x0a\x0a",
      "Предлагаемые\x20меры\x20устранения\x20ошибки\x3a",
      "\x0a\x31\x29\x20Убедитесь\x20в\x20надежности\x20подсоединения\x20кабелей"
        "\x2e",
      "\x0a\x32\x29\x20Перезагрузите\x20компьютер\x2e",
      "\x0a\x33\x29\x20Убедитесь\x20в\x20том\x2c\x20что\x20в\x20\x27\x43\x4f\x4e\x46\x49\x47\x27\x20задан\x20правил"
        "ьный\x20\x43\x4f\x4d\x20порт\x2e\x20\x28Третья\x20кнопка\x20на\x20экране\x2c\x20где\x20вы\x20"
        "выбираете\x20Хозяина\x20или\x20Гостя\x2e\x29",
      "\x0a\x34\x29\x20Попробуйте\x20уменьшить\x20скорость\x20передачи\x20данны"
        "х\x20в\x20\x27\x43\x4f\x4e\x46\x49\x47\x27\x20до\x20\x31\x39\x32\x30\x30\x20или\x20до\x20\x39\x36\x30\x30\x2e"
};
char* cMiniViewText[KB_MINI_VIEW_TEXT_COUNT] =
    {  "\x25\x64\x20воинов",   "\x25\x64\x20воин",   "Атака",   "Защита",   "ЗД",   "Урон",   "МР",   "УЧ",   "Выстр\x2e"};
char* gFileRequestHelp[KB_FILE_REQUEST_HELP_COUNT] = {
      "\x7bМаленькие\x20карты\x7d\x0a\x0aПросмотр\x20только\x20маленьких\x20кар"
        "т\x20\x28\x33\x36\x20\x78\x20\x33\x36\x29\x2e",
      "\x7bСредние\x20карты\x7d\x0a\x0aПросмотр\x20только\x20средних\x20карт\x20\x28\x37"
        "\x32\x20\x78\x20\x37\x32\x29\x2e",
      "\x7bБольшие\x20карты\x7d\x0a\x0aПросмотр\x20только\x20больших\x20карт\x20\x28\x31"
        "\x30\x38\x20\x78\x20\x31\x30\x38\x29\x2e",
      "\x7bОчень\x20большие\x20карты\x7d\x0a\x0aПросмотр\x20только\x20очень\x20бол"
        "ьших\x20карт\x20\x28\x31\x34\x34\x20\x78\x20\x31\x34\x34\x29\x2e",
      "\x7bВсе\x20карты\x7d\x0a\x0aПросмотр\x20всех\x20карт\x2e",
      "\x7bВвод\x20имени\x7d\x0a\x0aВведите\x20имя\x20файла\x2c\x20под\x20которым\x20вых"
        "отите\x20сохранить\x20игру\x2e",
      "\x7bОК\x7d\x0a\x0aПодтверждение\x20выбора\x2e",
      "\x7bОтмена\x7d\x0a\x0aОтмена\x20без\x20подтверждения\x20выбора\x2e",
      "\x7bЗначок\x20размера\x7d\x0a\x0aОбозначает\x20размер\x20карты\x3a\x20мален"
        "ькая\x20\x28\x33\x36\x20\x78\x20\x33\x36\x29\x2c\x20средняя\x20\x28\x37\x32\x20\x78\x20\x37\x32\x29\x2c\x20большая\x20\x28\x31\x30\x38\x20"
        "\x78\x20\x31\x30\x38\x29\x20или\x20очень\x20большая\x20\x28\x31\x34\x34\x20\x78\x20\x31\x34\x34\x29\x2e",
      "\x7bЗначок\x20игроков\x7d\x0a\x0aОбозначает\x20количество\x20игроков\x20"
        "в\x20данном\x20сценарии\x2e\x20При\x20отсутствии\x20игроков\x2dлюдей\x20"
        "их\x20места\x20занимает\x20компьютер\x2e",
      "\x7bУсловия\x20победы\x7d\x0aПредусмотрено\x20\x36\x20возможных\x20вариа"
        "нтов\x3a\x0a\x7bНадгробный\x20камень\x7d\x20\x2d\x20Разгромить\x20всех\x20геро"
        "ев\x20противника\x20и\x20захватить\x20его\x20замки\x2e\x0a\x7bГород\x7d\x20\x2d\x20З"
        "ахватить\x20определенный\x20замок\x2e\x0a\x7bПортрет\x20героя\x7d\x20\x2d\x20Р"
        "азгромить\x20определенного\x20героя\x2e\x0a\x7bМедаль\x7d\x20\x2d\x20Найти\x20"
        "определенный\x20артефакт\x2e\x0a\x7bРукопожатие\x7d\x20\x2d\x20Ваш\x20альян"
        "с\x20должен\x20разгромить\x20альянс\x20противника\x2e\x0a\x7bМонеты\x7d\x20"
        "\x2d\x20Накопить\x20нужное\x20количество\x20золота\x2e",
      "\x7bУссловия\x20поражения\x7d\x0a\x0aПредусмотрено\x20\x34\x20возможных\x20"
        "условия\x3a\x0a\x7bНадгробный\x20камень\x7d\x20\x2d\x20Потеря\x20всех\x20ваших"
        "\x20героев\x20и\x20городов\x2e\x0a\x7bГород\x7d\x20\x2d\x20Потеря\x20определенног"
        "о\x20замка\x2e\x0a\x7bПортрет\x20героя\x7d\x20\x2d\x20Потеря\x20указанного\x20гер"
        "оя\x2e\x0a\x7bПесочные\x20часы\x7d\x20\x2d\x20Победа\x20не\x20была\x20достигнута\x20"
        "до\x20указанного\x20срока\x2e\x29",
      "\x7bНазвание\x7d\x0a\x0aНазвание\x20карты\x2e",
      "\x7bОписание\x7d\x0a\x0aОписание\x20карты\x2e",
      "\x7bТрудность\x20карты\x7d\x0a\x0aСтепень\x20сложности\x20игры\x20на\x20это"
        "й\x20карте\x2e\x20Трудность\x20карты\x20определяется\x20разработчи"
        "ком\x20сценария\x2e\x20Более\x20сложные\x20карты\x20характеризуютс"
        "я\x20большим\x20числом\x20сильных\x20противников\x2c\x20меньшим\x20ко"
        "личеством\x20ресурсов\x20или\x20специальными\x20условиями\x2c\x20з"
        "атрудняющими\x20достижение\x20победы\x2e"
};
char* cPersonality[KB_PERSONALITY_TEXT_COUNT] = {  "Воин",   "Строитель",   "Исследователь",   "Человек"};
char* gArmySizeNames[KB_ARMY_SIZE_NAME_COUNT][KB_ARMY_SIZE_NAME_VARIANT_COUNT] = {
    {  "Мало",   "Мало",   "мало"},
    {  "Немного",   "Немного",   "немного"},
    {  "Стая",   "Стая",   "стая"},
    {  "Много",   "Много",   "много"},
    {  "Орда",   "Орда",   "орда"},
    {  "Толпа",   "Толпа",   "толпа"},
    {  "Свора",   "Свора",   "свора"},
    {  "Тысячи",   "Тысячи\x2e\x2e\x2e",   "тысячи"},
    {  "Легион",   "Легион",   "легион"}
};
char* cRandomTavernText[KB_RANDOM_TAVERN_TEXT_COUNT] = {
      "Истина\x20где\x2dто\x20рядом\x2e",
      "Темная\x20сторона\x20сильнее\x2e",
      "Конец\x20Света\x20близок\x2e",
      "Прах\x20Лорда\x20Слэйера\x20захоронен\x20в\x20основании\x20арены\x2e",
      "Он\x20невиновен\x2e",
      "Черный\x20дракон\x20сделает\x20Титана\x20в\x20любой\x20день\x20недели"
        "\x2e",
      "Он\x20сказал\x20ей\x2c\x20\x22Я\x2dда\x2dда\x2dяда\x2dда\x22\x2e\x2e\x2e\x20а\x20она\x20сказала\x2c"
        "\x20\x22Ля\x2dля\x2dля\x2c\x20ля\x2dля\x2dля\x2e\x2e\x2e\x22",
      "Тут\x20бывал\x20человек\x20из\x20Нунтукета\x2e\x2e\x2e"
};
char* cRandomSignText[KB_RANDOM_SIGN_TEXT_COUNT] =
    {  "Прямо\x20пойдешь\x20\x2d\x20коня\x20потеряешь\x2e",   "Сдается\x20в\x20аренду\x2e",   "До\x20следующего\x20знака\x20\x35\x30\x20миль\x2e",   "Кто\x20идет\x20за\x20Блинским\x3f"};
char* cCampaignAwards[KB_CAMPAIGN_AWARD_TEXT_COUNT] = {
      "Альянс\x20гномов",
      "Гильдия\x20колдуний",
      "Роланд\x20становится\x20сильнее",
      "Перенос\x20войск",
      "Корлагон\x20побежден",
      "Корона\x20всевластия",
      "Гильдия\x20некромантов",
      "Смерть\x20гномам",
      "Союз\x20огров",
      "Союз\x20драконов",
      "Корона\x20всевластия",
      "Перенос\x20войск"
};
char* cCampaignName[(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT] = {
    {  "Сила\x20оружия",
       "Аннексия",
       "Спасти\x20гномов",
       "Копи\x20Каратора",
       "Переломный\x20момент",
       "Защитник",
       "Вызов\x20брошен\x21",
       "Корона",
       "Акт\x20отчаяния",
       "Час\x20нашей\x20славы",
       "",
       "Предательство"},
    {  "Первая\x20кровь",
       "Войны\x20с\x20варварами",
       "Некроманты\x21",
       "Смерть\x20гномам",
       "Переломный\x20момент\x20",
       "Крестьяне\x21",
       "Владыка\x20драконов",
       "Лорды\x20провинций",
       "Корона",
       "К\x20вящей\x20славе",
       "Апокалипсис",
       "Предательство\x21"}
};
char* cCampaignDescription[(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT] = {
    {  "Прежде\x20чем\x20поднять\x20восстание\x20против\x20брата\x2c\x20Ролан"
        "д\x20хочет\x2c\x20чтобы\x20вы\x20одержали\x20победу\x20над\x20соседними\x20"
        "властителями\x2e\x20Между\x20ними\x20нет\x20единства\x2c\x20поэтому\x20б"
        "ольшую\x20часть\x20времени\x20они\x20будут\x20заняты\x20стычками\x20д"
        "руг\x20с\x20другом\x2e\x20Победа\x20будет\x20вашей\x2c\x20когда\x20вы\x20захва"
        "тите\x20все\x20города\x2e",
       "Властители\x20сопредельных\x20земель\x20отказываются\x20прин"
         "ести\x20клятву\x20верности\x20Роланду\x2c\x20и\x20поэтому\x20должны\x20б"
         "ыть\x20повержены\x2e\x20Богатства\x20и\x20власти\x20им\x20не\x20занимать"
         "\x2c\x20поэтому\x20будьте\x20готовы\x20к\x20нелегкой\x20борьбе\x2e\x20Чтобы"
         "\x20победить\x2c\x20захватите\x20все\x20вражеские\x20замки\x2e",
       "Вам\x20следует\x20защитить\x20гномов\x20от\x20армий\x20Арчибальда\x2e"
         "\x20Чтобы\x20победить\x2c\x20захватите\x20все\x20города\x20и\x20замки\x20пр"
         "отивника\x2e\x20Следите\x20за\x20тем\x2c\x20чтобы\x20враг\x20не\x20захватил"
         "\x20все\x20города\x20гномов\x2c\x20иначе\x20победа\x20достанется\x20ему\x2e",
       "В\x20борьбе\x20за\x20ресурсы\x20и\x20сокровища\x20вам\x20противостоят"
         "\x20четыре\x20противника\x2c\x20объединившихся\x20в\x20союз\x2e\x20Чтобы"
         "\x20победить\x2c\x20захватите\x20все\x20вражеские\x20замки\x2e",
       "Ваши\x20враги\x20заключили\x20против\x20вас\x20союз\x2e\x20Они\x20где\x2dто"
         "\x20рядом\x2c\x20поэтому\x20в\x20любой\x20момент\x20будьте\x20готовы\x20к\x20б"
         "итве\x2e\x20Вы\x20победите\x2c\x20когда\x20завладеете\x20всеми\x20четырь"
         "мя\x20замками\x2c\x20находящимися\x20в\x20этой\x20небольшой\x20долине"
         "\x2e",
       "Гильдия\x20колдунов\x20славного\x20города\x20Норастона\x20попро"
         "сила\x20Роланда\x20помочь\x20ей\x20отбиться\x20от\x20союзников\x20Арч"
         "ибальда\x2e\x20Чтобы\x20победить\x2c\x20вы\x20должны\x20захватить\x20все"
         "\x20вражеские\x20замки\x2e\x20Не\x20потеряйте\x20Норастон\x2c\x20иначе\x20в"
         "ы\x20проиграли\x2e\x20\x28Один\x20из\x20вражеских\x20замков\x20на\x20остров"
         "е\x20в\x20океане\x29\x2e",
       "Соберите\x20армию\x20побольше\x20и\x20захватите\x20замок\x20против"
         "ника\x20не\x20позднее\x2c\x20чем\x20через\x20\x38\x20недель\x2e\x20Вам\x20противо"
         "стоит\x20всего\x20один\x20противник\x2c\x20но\x20до\x20его\x20замка\x20скак"
         "ать\x20и\x20скакать\x2e\x20Все\x20войска\x2c\x20которые\x20останутся\x20у\x20в"
         "ас\x20к\x20концу\x20этого\x20сценария\x2c\x20примут\x20участие\x20в\x20закл"
         "ючительной\x20битве\x2e",
       "Найдите\x20корону\x2c\x20прежде\x20чем\x20это\x20сделают\x20герои\x20Арч"
         "ибальда\x2e\x20Корона\x20понадобится\x20Роланду\x20для\x20победы\x20в"
         "\x20заключительной\x20битве\x2e",
       "Три\x20противника\x2c\x20и\x20среди\x20них\x20сам\x20лорд\x20Корлагон\x2c\x20з"
         "аключили\x20союз\x20и\x20стоят\x20между\x20вами\x20и\x20великой\x20побед"
         "ой\x2e\x20Роланд\x20обосновался\x20в\x20замке\x20на\x20северо\x2dзападе\x2c"
         "\x20и\x20если\x20этот\x20замок\x20падет\x2c\x20вы\x20проиграете\x2e\x20Если\x20вы"
         "\x20захватите\x20Корлагона\x20сейчас\x2c\x20он\x20не\x20будет\x20драться"
         "\x20против\x20вас\x20в\x20последней\x20битве\x2e",
       "Итак\x2c\x20пробил\x20час\x20последнего\x20и\x20решительного\x20боя\x2e\x20"
         "И\x20вы\x2c\x20и\x20ваши\x20противники\x20вооружены\x20до\x20зубов\x2c\x20и\x20вс"
         "е\x20кругом\x20объединились\x20против\x20вас\x2e\x20Война\x20будет\x20за"
         "кончена\x2c\x20когда\x20вы\x20захватите\x20в\x20плен\x20Арчибальда\x21",
       "",
       "Вы\x20сменили\x20сюзерена\x2c\x20и\x20теперь\x20у\x20вас\x20три\x20замка\x20пр"
         "отив\x20одного\x20у\x20противника\x2e\x20Эта\x20миссия\x20будет\x20для\x20в"
         "ас\x20самой\x20легкой\x20во\x20всей\x20войне\x2e\x2e\x2e\x20Предатель\x21"},
    {  "Король\x20Арчибальд\x20требует\x2c\x20чтобы\x20вы\x20разгромили\x20тр"
        "ех\x20противников\x2c\x20которые\x20обосновались\x20в\x20этих\x20земл"
        "ях\x2e\x20Они\x20не\x20связаны\x20между\x20собой\x20союзным\x20договором"
        "\x2c\x20поэтому\x20по\x20большей\x20части\x20они\x20будут\x20тратить\x20сил"
        "ы\x20на\x20вражду\x20друг\x20с\x20другом\x2e\x20Вы\x20победите\x2c\x20когда\x20вс"
        "е\x20их\x20замки\x20окажутся\x20в\x20ваших\x20руках\x2e",
       "Вам\x20предстоит\x20объединить\x20племена\x20северных\x20варвар"
         "ов\x2c\x20предварительно\x20усмирив\x20их\x2e\x20Как\x20и\x20в\x20предыдуще"
         "й\x20миссии\x2c\x20противники\x20не\x20состоят\x20в\x20союзе\x20друг\x20с\x20д"
         "ругом\x2c\x20но\x20у\x20них\x20больше\x20ресурсов\x2e\x20Победа\x20будет\x20ва"
         "шей\x2c\x20когда\x20вы\x20захватите\x20все\x20вражеские\x20замки\x20и\x20пе"
         "ребьете\x20всех\x20героев\x20противника\x2e",
       "Добрые\x20волшебники\x20захватили\x20замок\x20некромантов\x2e\x20Ч"
         "тобы\x20победить\x2c\x20вы\x20должны\x20отобрать\x20его\x20обратно\x2e\x20П"
         "омните\x2c\x20что\x20хотя\x20вы\x20и\x20начинаете\x20с\x20сильной\x20армией"
         "\x2c\x20в\x20самом\x20начале\x20у\x20вас\x20нет\x20своего\x20замка\x2e\x20Вы\x20долж"
         "ны\x20заиметь\x20его\x20за\x20\x37\x20дней\x2c\x20иначе\x20все\x20потеряно\x2e\x20\x28Б"
         "лижайший\x20замок\x20на\x20юго\x2dвостоке\x29\x2e",
       "Гномов\x20следует\x20привести\x20к\x20покорности\x2c\x20прежде\x20чем"
         "\x20они\x20смогут\x20помешать\x20планам\x20короля\x20Арчибальда\x2e\x20П"
         "од\x20знаменами\x20Роланда\x20много\x20героев\x2c\x20у\x20него\x20нескол"
         "ько\x20замков\x2c\x20поэтому\x20будьте\x20готовы\x20к\x20нападению\x20ср"
         "азу\x20с\x20нескольких\x20сторон\x2e\x20Вам\x20надо\x20захватить\x20все\x20"
         "города\x20противника\x2e",
       "Ваши\x20противники\x20объединились\x20против\x20вас\x20и\x20притаи"
         "лись\x20неподалеку\x2c\x20поэтому\x20будьте\x20начеку\x2e\x20Победа\x20б"
         "удет\x20вашей\x2c\x20когда\x20вы\x20завладеете\x20всеми\x20четырьмя\x20з"
         "амками\x2c\x20находящимися\x20в\x20этой\x20небольшой\x20долине\x2e",
       "Вам\x20предстоит\x20подавить\x20крестьянский\x20бунт\x2c\x20во\x20гла"
         "ве\x20которого\x20стоят\x20агенты\x20Роланда\x2e\x20Все\x20ваши\x20сосед"
         "и\x20объединились\x20против\x20вас\x2c\x20но\x20на\x20вашей\x20стороне\x20л"
         "орд\x20Корлагон\x20\x2d\x20опытный\x20и\x20сильный\x20боец\x2e\x20Чтобы\x20поб"
         "едить\x2c\x20вы\x20должны\x20захватить\x20все\x20замки\x20противника\x2e",
       "В\x20этой\x20миссии\x20вам\x20противостоят\x20два\x20противника\x2e\x20О"
         "ба\x20хорошо\x20вооружены\x20и\x20полны\x20решимости\x20выставить\x20"
         "вас\x20со\x20своего\x20острова\x2e\x20Избегая\x20встречи\x20с\x20ними\x2c\x20з"
         "ахватите\x20Драконий\x20город\x20\x2d\x20тогда\x20победа\x20будет\x20за\x20"
         "вами\x2e",
       "Вам\x20приказано\x20разгромить\x20удельных\x20властителей\x2c\x20к"
         "оторые\x20присягнули\x20на\x20верность\x20Роланду\x2e\x20Все\x20враже"
         "ские\x20замки\x20объединились\x20и\x20выступают\x20против\x20вас\x2e\x20"
         "Вы\x20начинаете\x20игру\x20без\x20замка\x2e\x20Вам\x20надо\x20захватить\x20"
         "замок\x20за\x20\x37\x20дней\x2e\x20Победа\x20будет\x20вашей\x2c\x20когда\x20все\x20з"
         "амки\x20противника\x20падут\x2e",
       "Найдите\x20корону\x2c\x20пока\x20ею\x20не\x20завладели\x20герои\x20Ролан"
         "да\x2e\x20Корона\x20понадобится\x20Арчибальду\x20для\x20победы\x20в\x20з"
         "аключительной\x20битве\x20с\x20Роландом\x2e",
       "Соберите\x20армию\x20побольше\x20и\x20захватите\x20замок\x20против"
         "ника\x20не\x20позднее\x2c\x20чем\x20через\x20\x38\x20недель\x2e\x20Вам\x20противо"
         "стоит\x20всего\x20один\x20противник\x2c\x20но\x20до\x20его\x20замка\x20скак"
         "ать\x20и\x20скакать\x2e\x20Все\x20войска\x2c\x20которые\x20останутся\x20у\x20в"
         "ас\x20к\x20концу\x20этого\x20сценария\x2c\x20будут\x20с\x20вами\x20в\x20заключ"
         "ительной\x20битве\x2e",
       "Итак\x2c\x20пробил\x20час\x20последней\x20битвы\x2e\x20И\x20вы\x2c\x20и\x20ваши\x20п"
         "ротивники\x20вооружены\x20до\x20зубов\x2c\x20и\x20все\x20объединились"
         "\x20против\x20вас\x2e\x20Война\x20закончится\x2c\x20когда\x20вы\x20захватит"
         "е\x20в\x20плен\x20Роланда\x2c\x20и\x20смотрите\x2c\x20не\x20потеряйте\x20Арчиб"
         "альда\x20в\x20пылу\x20битвы\x21",
       "Вы\x20сменили\x20сюзерена\x2c\x20и\x20теперь\x20у\x20вас\x20три\x20замка\x20пр"
         "отив\x20одного\x20у\x20противника\x2e\x20Эта\x20миссия\x20будет\x20для\x20в"
         "ас\x20самой\x20легкой\x20во\x20всей\x20войне\x2e\x2e\x2e\x20Предатель\x21"}
};
char* cOutOfMemory =
      "\x0a\x0a\x0a\x0a\x0a\x0a\x0a\x0a\x0a\x0a\x0a\x0a\x0a\x0a\x25\x73\x0aГероям\x20\x49\x49\x20требуется\x20минимум\x20\x0a\x25\x64"
        "\x4b\x20Расширенной\x20\x20памяти\x20\x28\x58\x4d\x53\x29\x20и\x0a\x34\x38\x30\x4b\x20общей\x20памяти\x2e"
        "\x0a\x0a";
char* cSlowVideoLevelText[KB_SLOW_VIDEO_LEVEL_TEXT_COUNT] = {  "Обычное",   "Черес\x2d\x0aстрочное"};
char* gSPanelHelp[KB_SETTINGS_PANEL_HELP_COUNT] = {
      "\x7bОК\x7d\x0a\x0aЗакрыть\x20меню\x2e",
      "\x7bМузыка\x7d\x0a\x0aВключить\x20или\x20выключить\x20фоновую\x20музыку\x2e",
      "\x7bЭффекты\x7d\x0a\x0aВключить\x20или\x20выключить\x20звуковые\x20эффек"
        "ты\x2e",
      "\x7bСкорость\x7d\x0a\x0aВыбрать\x20скорость\x20передвижения\x20героев"
        "\x20по\x20карте\x2e",
      "\x7bКачество\x20звука\x7d\x0a\x0aВыбрать\x20формат\x20музыки\x2e\x20Как\x20пра"
        "вило\x2c\x20музыка\x20в\x20формате\x20\x4d\x49\x44\x49\x20не\x20отличается\x20качест"
        "вом\x2c\x20но\x20она\x20предъявляет\x20меньшие\x20требования\x20к\x20про"
        "изводительности\x20системы\x2c\x20чем\x20формат\x20Стерео\x20\x43\x44\x2e\x20Ф"
        "ормат\x20Стерео\x20\x43\x44\x20дает\x20\x20возможность\x20воспроизводить"
        "\x20оперную\x20музыку\x2e",
      "\x7bПоказывать\x20путь\x7d\x0a\x0aВключить\x20или\x20выключить\x20отобра"
        "жение\x20пути\x20героя\x20на\x20карте\x2e\x20\x20Если\x20опция\x20включена\x2c"
        "\x20первое\x20нажатие\x20по\x20объекту\x20на\x20карте\x20показывает\x20п"
        "уть\x20к\x20этому\x20объекту\x2c\x20а\x20по\x20второму\x20нажатию\x20левой\x20"
        "кнпоки\x20мыши\x20начинается\x20движение\x2e\x20Если\x20эта\x20опция\x20"
        "отключена\x2c\x20движение\x20начинается\x20по\x20первому\x20нажати"
        "ю\x2e",
      "\x7bСкорость\x20врага\x7d\x0a\x0aВыбрать\x20скорости\x20перемещения\x20г"
        "ероев\x2c\x20управляемых\x20компьютером\x2e\x20При\x20этом\x20можно\x20в"
        "ыбрать\x20режим\x2c\x20в\x20котором\x20не\x20будет\x20отображаться\x20пе"
        "редвижение\x20противника\x2e",
      "\x7bИнтерфейс\x7d\x0a\x0aВыбор\x20желаемого\x20типа\x20интерфейса\x2e\x20По"
        "\x20умолчанию\x20задан\x20динамический\x20интерфейс\x2c\x20в\x20котор"
        "ом\x20\x27злое\x27\x20графическое\x20оформление\x20используется\x20дл"
        "я\x20трех\x20\x27злых\x27\x20классов\x20героев\x20\x28варвара\x2c\x20чернокниж"
        "ника\x20и\x20некроманта\x29\x2e",
      "\x7bБыстрый\x20бой\x7d\x0a\x0aПри\x20включении\x20этой\x20опции\x20перед\x20ка"
        "ждым\x20сражением\x20компьютер\x20будет\x20делать\x20запрос\x20о\x20п"
        "роведении\x20этого\x20сражения\x20в\x20режиме\x20быстрого\x20боя\x2e\x20"
        "Сражение\x20протекает\x20автоматически\x2c\x20и\x20компьютер\x20де"
        "монстрирует\x20вам\x20только\x20его\x20результат\x2e",
      "\x7bКурсор\x7d\x0a\x0aПереключение\x20курсора\x20с\x20черно\x2dбелого\x20на"
        "\x20цветной\x20и\x20обратно\x2e\x20Цветной\x20курсор\x20выглядит\x20симп"
        "атичнее\x2c\x20но\x20иногда\x20он\x20перемещается\x20по\x20экрану\x20не\x20"
        "так\x20плавно\x2c\x20как\x20черно\x2dбелый\x2e"
};
char* xBarrierColor[KB_BARRIER_COLOR_NAME_COUNT] =
    {  "Сизый",   "Синий",   "Коричневый",   "Золотой",   "Зеленый",   "Оранжевый",   "Фиолетовый",   "Красный"};
char* xGenericSiteNames[KB_GENERIC_SITE_NAME_COUNT] = {
      "Башня\x20алхимика",
      "Арена",
      "Лачуга\x20волхва",
      "Око\x20волхва",
      "Конюшни",
      "Русалка",
      "Сирены"
};
char* xRecruitmentSiteNames[KB_RECRUITMENT_SITE_NAME_COUNT] = {
      "Земляные\x20холмы",
      "Алтарь\x20Земли",
      "Алтарь\x20Воздуха",
      "Алтарь\x20Огня",
      "Алтарь\x20Воды"
};
SWinSetup gWinSetup[KB_WIN_SETUP_COUNT] = {
    {0, 100,   "Построить\x3a"},
    {1, 100,   "Скорость"},
    {1, 101,   "Информация\x0aо\x20враге"},
    {1, 102,   "Магия\x0aв\x20автобое"},
    {1, 103,   "Сетка"},
    {1, 104,   "Курсор\x0aс\x20тенью"},
    {1, 105,   "Затенение\x0aсетки"},
    {2, 100,   "Музыка"},
    {2, 101,   "Эффекты"},
    {2, 102,   "Тип\x20музыки"},
    {2, 103,   "Скорость"},
    {2, 104,   "Путь"},
    {2, 105,   "Враг"},
    {2, 106,   "Интерфейс"},
    {2, 107,   "Видео"},
    {2, 108,   "Тип\x20курсора"},
    {6, 300,   "Атака"},
    {6, 301,   "Защита"},
    {6, 302,   "Сила\x20магии"},
    {6, 303,   "Знания"},
    {7, 600,   "Сложность\x20игры\x3a"},
    {7, 57,   "Легкая"},
    {7, 58,   "Обычная"},
    {7, 59,   "Тяжелая"},
    {7, 60,   "Эксперт"},
    {7, 61,   "Невозможная"},
    {7, 62,   "Оппоненты\x3a"},
    {7, 84,   "Класс\x3a"},
    {9, 41,   "Золота\x20в\x20день\x3a"},
    {12, 0,   "Строить\x20корабль\x3a"},
    {12, 1,   "Цена\x3a"},
    {14, 800,   "\x31\x2dй"},
    {14, 801,   "\x32\x2dй"},
    {14, 802,   "\x33\x2dй"},
    {14, 803,   "\x34\x2dй"},
    {14, 804,   "\x35\x2dй"},
    {14, 805,   "\x36\x2dй"},
    {14, 604,   "Городов\x3a"},
    {14, 605,   "Замков\x3a"},
    {14, 606,   "Героев\x3a"},
    {14, 607,   "Золота\x20в\x20казне\x3a"},
    {14, 608,   "Дерево\x20и\x20руда\x3a"},
    {14, 609,   "Прочие\x20ресурсы\x3a"},
    {14, 610,   "Найдено\x20обелисков\x3a"},
    {14, 611,   "Артефакты\x3a"},
    {14, 612,   "Общая\x20сила\x20армии\x3a"},
    {14, 613,   "Доход\x3a"},
    {14, 620,   "Лучший\x20герой\x3a"},
    {14, 621,   "Лучшие\x20параметры\x3a"},
    {14, 622,   "Характер\x3a"},
    {14, 623,   "Лучший\x20воин\x3a"},
    {14, 0,   "Гильдия\x20воров\x3a\x20достижения\x20игроков"},
    {17, 110,   "Доступные\x20заклинания\x20были\x20записаны\x20в\x20книгу\x2e"},
    {18, 600,   "Атака\x3a"},
    {18, 601,   "Защита\x3a"},
    {18, 602,   "Сила\x20магии\x3a"},
    {18, 603,   "Знания\x3a"},
    {18, 604,   "Очки\x20магии\x3a"},
    {19, 600,   "Оборона\x3a"},
    {20, 600,   "Нанять\x20героя"},
    {21, 600,   "Атака"},
    {21, 601,   "Защита"},
    {21, 602,   "Сила\x20магии"},
    {21, 603,   "Знания"},
    {22, 0,   "Таверна"},
    {23, 600,   "Сложность\x0aкарты"},
    {23, 601,   "Сложность\x0aигры"},
    {23, 602,   "\x0aРейтинг"},
    {23, 603,   "Размер\x0aкарты"},
    {23, 604,   "Оппоненты"},
    {23, 605,   "Классы"},
    {23, 606,   "Условия\x0aпобеды"},
    {23, 607,   "Условия\x0aпоражения"}
};
b32 gbHeroWindShowing = false;
b32 gbFullCombatScreenDrawn = true;
b32 gbLimitedCombatUpdatePalette = false;
b32 gbFirstTimeThrough = false;
b32 gbSkipIntro = false;
b32 gbDoMemCheck = true;
b32 gbAllBlack = false;
b32 gbInCombat = false;
b32 gbDirectConnect = false;
i32 giForceSwitchMusic = -1;
b32 gbComputeExtent = false;
b32 gbSaveBiggestExtent = false;
b32 gbLimitToExtent = false;
b32 gbCurrArmyDrawn = true;
AdventureDisposeLevel gAdvDisposeLevel = ADV_DISPOSE_NONE;
b32 gbRemoteOn = false;
b32 gbGameInitialized = false;
i32 giHighScoreRank = -1;
HighScoreType giHighScoreType = HIGH_SCORE_STANDARD;
b32 gbShowHighScore = false;
b32 gbLowMemory = false;
i32 giHighMemBuffer = CHECK_MEMORY_INITIAL_AVAILABLE_KB;
void* gLowPage = NULL;
b32 gbInPollSound = false;
i32 iCDRomErr = CD_ROM_READY;
i32 bEarlySetupDone = 0;
i32 bKBDone = 0;
struct _REDBOOK* hRedbookz = NULL;
i32 bForceCheckTimeEvent = 0;
u16 IMHotSpots[KB_INIT_MENU_HOTSPOT_COUNT][(INIT_MENU_HOTSPOT_FIELD_COUNT)] = {
    {481, 185, 83, 96},
    {194, 179, 82, 79},
    {412, 105, 75, 76},
    {303, 137, 75, 44},
    {0, 389, 86, 90}
};

i32 lastIMHoverID = -1;
i32 bInCheckEndGame = 0;
i32 bInShutDown = 0;
b32 gbInMemError = false;
i32 iShingleAnimFrame = 0;
b32 gbHumanPlayer[(GAME_PLAYER_COUNT)];
b32 gbHitEvent;
i32 giMaxExtentX;
i32 giMaxExtentY;
i32 giRandomClouds;
char cOverrideDigitalDriver[GLOBAL_DRIVER_NAME_SIZE];
i32 giBottomViewOverrideEndTime;
i8 gArmyEffected[COMBAT_SIDE_COUNT][KB_ARMY_EFFECT_COUNT];
i32 giBottomViewResource;
b32 gbInCampaign;
i32 giResExtra1;
i32 giResExtra2;
i8 puzzlePiecesRemoved[PUZZLE_PIECE_STORAGE_SIZE];
i32 giSeedingValid;
i32 giLimitPlayer;
i32 giShowClouds;
i32 bDoColorCycle;
inputManager* gpInputManager;
i32 iMaxMapExtra;
palette* gPalette;
resourceManager* gpResourceManager;
char gcBotViewText[GLOBAL_BOTTOM_VIEW_TEXT_SIZE];
i32 bSpecialHideCursor;
searchArray* gpSearchArray;
i32 giResType1;
b32 gbBlackoutPlayer;
i32 giResType2;
char cNetBoxLine[BOX_LINE_COUNT][NET_BOX_LINE_SIZE];
i32 gIndex;
i32 giWeekTypeExtra;
philAI* gpPhilAI;
i32 giTCPType;
i32 gCurLoadedSpellEffect;
class mouseManager* gpMouseManager;
i32 giCurTempMobility;
OverviewReturnAction giOverviewReturnAction;
char cOverrideMIDIDriver[GLOBAL_DRIVER_NAME_SIZE];
icon* gSystemIcons;
b8 gbCombatSurrender;
heroWindow* pNormalDialogWindow;
i32 giTCPHostStatus;
char gMapName[GLOBAL_SHORT_MAP_NAME_SIZE];
i32 giMinExtentX;
i32 giMinExtentY;
MultiplayerBaseType iMPBaseType;
b32 gbTCPFirstTime;
i16* pwSizeOfMapExtra;
i32 giHeroScreenSrcIndex;
CalendarPeriodType giWeekType;
char gText[GLOBAL_TEXT_BUFFER_SIZE];
b32 gbInNewGameSetup;
class palette* gpBufferPalette;
i32 gbCampaignSideChoice;
char cNetBoxColor[BOX_LINE_COUNT];
i32 giMonthTypeExtra;
i32 iMPExtendedType;
i8 gcColorToSetupPos[RADAR_OWNER_COLOR_COUNT];
char gFullMapName[GLOBAL_MAP_NAME_SIZE];
char gcTCPName[GLOBAL_TCP_TEXT_SIZE];
i32 giShowIntro;
i32 glTimers[GLOBAL_TIMER_COUNT];
i32 giScore;
armyGroup* gpMonGroup;
configStruct gConfig;
char gcRegAppPath[GLOBAL_AGGREGATE_PATH_SIZE];
u32l gTimeMark;
char* EXPANSION_AGGREGATE_NAME;
char cPlayerNames[X_GLOBAL_PLAYER_COUNT][GLOBAL_PLAYER_NAME_SIZE];
game* gpGame;
b8 gbRetreatWin;
DialogWaitType giWaitType;
class icon* gCurLoadedSpellIcon;
u8 bSaveMusicPosition[KB_MUSIC_TRACK_COUNT];
i32 giBottomViewOverride;
char gcTCPAddress[GLOBAL_TCP_TEXT_SIZE];
u8 giSetupGameType;
char gLastFilename[GLOBAL_LAST_FILENAME_SIZE];
i32 giFullySeeded;
icon* gBuyBuildIcons;
i32 iCombatControlNetPos[COMBAT_SIDE_COUNT];
char cExpAggPathName[GLOBAL_AGGREGATE_PATH_SIZE];
b32 gbMoveShown;
void** ppMapExtra;
char gcBottomViewText[GLOBAL_BOTTOM_VIEW_MESSAGE_SIZE];
i32 giThisNetPos;
b8 gbSetupGamePosToRealGamePos[RADAR_OWNER_COLOR_COUNT];
char gcRegCDRomPath[GLOBAL_AGGREGATE_PATH_SIZE];
class heroWindow* heroWin;
i32 giOverviewReturnActionExtra;
i32 giCurGeneral;
i32 giThisGamePos;
i32 giNumHumanPlayers;
b32 gbIconClipOn;
b32 gbRemoteGameOpen;
combatManager* gpCombatManager;
i32 giTCPNumPlayers;
executive* gpExec;
void* hMainWindow;
i32 giCurWindowsStyleFlags;
i32 gGameCommand;
CalendarPeriodType giMonthType;
char* DEFAULT_AGGREGATE_NAME;
i32 gCurSpellEffectFrame;
b8 gbThisNetHumanPlayer[RADAR_OWNER_COLOR_COUNT];
char cAggPathName[GLOBAL_AGGREGATE_PATH_SIZE];
class highScoreManager* gpHighScoreManager;
b32 gbFunctionComplete;
b32 gbIAmGreatest;
b32 gbTextEntryEscaped;
i32 giTotalHighMem;
i32 gMapX;
i32 gMapY;
char gcWinText[GLOBAL_WINDOW_TEXT_SIZE];
i32 bFreshSave;
i32 bShowIt;
i32 gLowPageScreenSelector;
class heroWindowManager* gpWindowManager;
i32 giCurWatchPlayer;
char gcCommandLine[GLOBAL_COMMAND_LINE_SIZE];
i32 giBottomViewResourceQty;
soundManager* gpSoundManager;
b32 gbThisNetGotAdventureControl;
i32 giMapChangeCtr;
SMapChange sMapChangeQueue[CURSOR_MAP_CHANGE_QUEUE_COUNT];
b32 gbWaitForRemoteReceive;
u8 bMusicIsLooping[KB_MUSIC_TRACK_COUNT];
townManager* gpTownManager;
advManager* gpAdvManager;
b8 gbGamePosToNetPos[OLD_MAIN_MATCH_BUFFER_SIZE];
