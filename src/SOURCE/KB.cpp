#include <Ints.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/town.h>
#include <SOURCE/townManager.h>
#include <SOURCE/ARMY.h>
#include <BASE/executive.h>
#include <BASE/mouseManager.h>
#include <IRONFIST/campaigns.h>
#include <IRONFIST/creatures.h>
#include <IRONFIST/hooks.h>
#include <IRONFIST/heroes.h>
#include <IRONFIST/prefs.h>
#include <IRONFIST/townconsts.h>
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
#include <PLATFORM/Network.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/SaveNames.h>
#include <SOURCE/SMACKMGR.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/wingraph.h>
#include <BASE/BITS.h>
#include <BASE/bmap2.h>
#include <BASE/bitmap.h>
#include <BASE/sample.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <PLATFORM/File.h>
#include <SOURCE/Localization.h>

#include <string>

#include <SOURCE/advManager.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/hero.h>
#include <SOURCE/highScoreManager.h>
#include <SOURCE/philAI.h>
#include <SOURCE/playerData.h>
#include <SOURCE/searchArray.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Utf8.h>
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

enum class MoraleInfoTextIndex : i32 {
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
using enum MoraleInfoTextIndex;

enum class LuckInfoTextIndex : i32 {
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
using enum LuckInfoTextIndex;

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
    if (glTimers[GLOBAL_MOUSE_TIMER_SLOT] < platform::Ticks() && !gbPutzingWithMouseCtr) {
        glTimers[GLOBAL_MOUSE_TIMER_SLOT] = platform::Ticks() + MOUSE_UPDATE_INTERVAL;
        gpMouseManager->NewUpdate(0);
    }
    if (glTimers[GLOBAL_COLOR_CYCLE_TIMER_SLOT] < platform::Ticks()) {
        if (giCycleType == WINDOW_COLOR_CYCLE_COMBAT
            || giCycleType == WINDOW_COLOR_CYCLE_COMBAT_ALTERNATE)
            glTimers[GLOBAL_COLOR_CYCLE_TIMER_SLOT] = platform::Ticks() + COMBAT_COLOR_CYCLE_INTERVAL;
        else
            glTimers[GLOBAL_COLOR_CYCLE_TIMER_SLOT] = platform::Ticks() + DEFAULT_COLOR_CYCLE_INTERVAL;
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
    if (glTimers[GLOBAL_POLL_SOUND_TIMER_SLOT] < platform::Ticks()) {
        glTimers[GLOBAL_POLL_SOUND_TIMER_SLOT] = platform::Ticks() + SOUND_POLL_INTERVAL;
        if (gbForegroundApp)
            gpSoundManager->PollSound();
        PollRemote();
    }
    gbInPollSound = false;
}

void ForcePollSound(void) {
    glTimers[GLOBAL_POLL_SOUND_TIMER_SLOT] = platform::Ticks() - 1;
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

void EarlyShutdown(const char* caption, const char* text) {
    platform::ShowMessage(caption, text);
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
                localization::Tr("system.cdrom.unavailable_guest_only"),
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
                localization::Tr("system.cdrom.expansion_disc_missing"),
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
    b32 quit;
    b32 mainScreenLoaded_h;
    b32 firstMainScreen_h;
    i32 savedUpdateFlags_l;
    i32 player_h;

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
    Ironfist_Startup();
    LogStr("OM1");
    LogStr("OM2");
    command_c = -1;
    if (gpExec->InitSystem())
        ShutDown(localization::Tr("system.initialization_failed"));
    LogStr("OM3");
    platform::ChangeMenu(hmnuDflt);
    gPalette = gpResourceManager->GetPalette("kb.pal");
    gpWindowManager->m_updateFlags = 1;
    smallFont = gpResourceManager->GetFont("smalfont.fnt");
    bigFont = gpResourceManager->GetFont("bigfont.fnt");
    gpMouseManager->SetPointer(
        "advmice.mse",
        0,
        MOUSE_AUTO_CURSOR_TYPE
    );
    gpMouseManager->SetColorMice(gConfig.gfx[H2EnumIndex(giCurExe)].colorMouseCursor);
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
                    cPlayerNames[player_h][0] = 0;
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
                gbUseBzip2Compression = gbUseDiffCompression = 1;
                for (player_h = 0; player_h < giNumHumanPlayers; player_h++) {
                    if (!gsNetPlayerInfo[player_h].useBzip2Compression)
                        gbUseBzip2Compression = 0;
                    if (!gsNetPlayerInfo[player_h].useDiffCompression)
                        gbUseDiffCompression = 0;
                }
                netBuffer_f.setup.useBzip2Compression = gbUseBzip2Compression;
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
                strcpy(gpGame->m_saveName, save_names::NewGame);
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
                        gText,
                        localization::Tr("player.color_default_name"),
                        gColors[gpGame->m_players[player_h].m_color]
                    );
                    utf8::Copy(
                        cPlayerNames[player_h],
                        sizeof(cPlayerNames[player_h]),
                        gText
                    );
                    utf8::UppercaseFirst(cPlayerNames[player_h]);
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
                Ironfist_AdvManagerReady();
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
            bShowIt = 1;
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
                         && gpGame->m_campaignScenarioCompleted[H2EnumIndex(gpGame->m_campaignType)]
                                                               [OLD_MAIN_ARCHIBALD_FINAL_SCENARIO])
                        || (gpGame->m_campaignScenario + 1 == OLD_MAIN_ROLAND_FINAL_SCENARIO_NUMBER
                            && gpGame->m_campaignType == CAMPAIGN_ROLAND
                            && gpGame
                                   ->m_campaignScenarioCompleted[H2EnumIndex(gpGame->m_campaignType)]
                                                                [OLD_MAIN_ROLAND_FINAL_SCENARIO])) {
                        gbShowHighScore = true;
                        ShowCongrats(HIGH_SCORE_CAMPAIGN);
                        AddScoreToHighScore(
                            gpGame->m_campaignScore,
                            gpGame->m_campaignScore,
                            0,
                            HIGH_SCORE_CAMPAIGN,
                            gpGame->m_campaignType == CAMPAIGN_ARCHIBALD
                                ? localization::Tr("campaign.ruler.archibald")
                                : localization::Tr("campaign.ruler.roland")
                        );
                    }
                    if (campaignResult) {
                        for (player_h = 0; player_h < OLD_MAIN_PLAYER_COUNT; player_h++)
                            cPlayerNames[player_h][0] = 0;
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
                            const_cast<char*>(
                                ironfistCampaignNames[H2EnumIndex(xCampaign.CampaignID())].c_str()
                            )
                        );
                    }
                    if (campaignResult) {
                        for (player_h = 0; player_h < OLD_MAIN_PLAYER_COUNT; player_h++)
                            cPlayerNames[player_h][0] = 0;
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

char toupper(char c) {
    return static_cast<char>(utf8::ToUpper(static_cast<unsigned char>(c)));
}

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
    giDebugLevel = 0;
    giShowIntro = 1;
    gbCheatMenus = false;
    giScreenScroll = 1;
    giLimitPlayer = 0;
    gbBlackoutPlayer = true;
    helpRequested = false;
    strcpy(gMapName, "Chaos.mp2");
    strcpy(gFullMapName, localization::Tr("map.default.chaos"));

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
        gText[0] = 0;
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

MessageDispatchResult InitMenuHandler(struct tag_message& msg) {
    b32 handled = false;
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
                        IMHotSpots[menu][H2EnumIndex(INIT_MENU_HOTSPOT_X)],
                        IMHotSpots[menu][H2EnumIndex(INIT_MENU_HOTSPOT_Y)],
                        IMHotSpots[menu][H2EnumIndex(INIT_MENU_HOTSPOT_WIDTH)],
                        IMHotSpots[menu][H2EnumIndex(INIT_MENU_HOTSPOT_HEIGHT)]
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
                        handled = true;
                    }
                    break;
            }
        } else if (msg.type == INIT_MENU_MOUSE_MOVE) {
            hoverIndex = -1;
            for (idx = 0; idx < MENU_HOTSPOT_COUNT; idx++) {
                if (msg.payload.mouse.screenX >= IMHotSpots[idx][H2EnumIndex(INIT_MENU_HOTSPOT_X)]
                    && msg.payload.mouse.screenY >= IMHotSpots[idx][H2EnumIndex(INIT_MENU_HOTSPOT_Y)]
                    && msg.payload.mouse.screenX
                           < IMHotSpots[idx][H2EnumIndex(INIT_MENU_HOTSPOT_X)]
                                 + IMHotSpots[idx][H2EnumIndex(INIT_MENU_HOTSPOT_WIDTH)]
                    && msg.payload.mouse.screenY
                           < IMHotSpots[idx][H2EnumIndex(INIT_MENU_HOTSPOT_Y)]
                                 + IMHotSpots[idx][H2EnumIndex(INIT_MENU_HOTSPOT_HEIGHT)]) {
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
                        IMHotSpots[lastIMHoverID][H2EnumIndex(INIT_MENU_HOTSPOT_X)],
                        IMHotSpots[lastIMHoverID][H2EnumIndex(INIT_MENU_HOTSPOT_Y)],
                        IMHotSpots[lastIMHoverID][H2EnumIndex(INIT_MENU_HOTSPOT_WIDTH)],
                        IMHotSpots[lastIMHoverID][H2EnumIndex(INIT_MENU_HOTSPOT_HEIGHT)]
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
                        IMHotSpots[hoverIndex][H2EnumIndex(INIT_MENU_HOTSPOT_X)],
                        IMHotSpots[hoverIndex][H2EnumIndex(INIT_MENU_HOTSPOT_Y)],
                        IMHotSpots[hoverIndex][H2EnumIndex(INIT_MENU_HOTSPOT_WIDTH)],
                        IMHotSpots[hoverIndex][H2EnumIndex(INIT_MENU_HOTSPOT_HEIGHT)]
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

MessageDispatchResult NullHandler(struct tag_message&) {
    return MESSAGE_DISPATCH_CONSUME;
}

MessageDispatchResult RecruitHeroHandler(tag_message& msg) {

    b32 shouldClose = false;

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
                        shouldClose = true;
                        break;
                    case EVENT_WINDOW_THIRD_BUTTON:
                        gpTownManager->m_recruitState = 0;
                        gpWindowManager->m_dialogResult = msg.payload.widget.id;
                        shouldClose = true;
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

const char* GetBuildingInfo(FactionType race, BuildingSlotType building, i32 mode) {
    char buf[BUILDING_INFO_BUFFER_SIZE];
    if (race == FACTION_CYBORG && building == BUILDING_SLOT_MAGE_GUILD) {
        sprintf(
            buf,
            localization::Tr("town.cyborg.cybernetics_lab.description"),
            GetBuildingName(race, building)
        );
    } else if (IsWellDisabled() && building == BUILDING_SLOT_WELL) {
        if (race == FACTION_NECROMANCER) {
            sprintf(buf, "%s", localization::Tr("town.poisoned_well.description"));
        } else {
            sprintf(buf, "%s", localization::Tr("town.well.description"));
        }
    } else if (race == FACTION_NECROMANCER && building == BUILDING_SLOT_NECROMANCER_SHRINE) {
        utf8::Copy(buf, sizeof(buf), xNecromancerShrineDesc);
    } else if (building == BUILDING_SLOT_WELL_EXTRA) {
        sprintf(
            buf,
            localization::Tr("town.building.weekly_growth")  ,
            GetBuildingName(race, building),
            gArmyNamesPlural[H2EnumIndex(gDwellingType[H2EnumIndex(race)][0])]
        );
    } else if (building == BUILDING_SLOT_SPECIAL) {
        utf8::Copy(buf, sizeof(buf), gBuildingInfoSpecial[H2EnumIndex(race)]);
    } else if (building < BUILDING_SLOT_DWELLING_FIRST) {
        utf8::Copy(buf, sizeof(buf), cBuildingInfoNeutral[H2EnumIndex(building)]);
    } else {
        sprintf(
            gText,
            localization::Tr("town.building.produces")  ,
            GetBuildingName(race, building),
            gArmyNamesPlural
                [H2EnumIndex(gDwellingType[H2EnumIndex(race)][H2EnumIndex(building) - H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)])]
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
        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, buf);
    }
    return gText;
}

const char* GetBuildingName(FactionType race, BuildingSlotType building) {
    if (race == FACTION_NECROMANCER && building == BUILDING_SLOT_NECROMANCER_SHRINE)
        return xNecromancerShrine;
    if (race == FACTION_CYBORG && building == BUILDING_SLOT_WELL)
        return const_cast<char*>(localization::Tr("town.cyborg.energy_pump.name"));
    if (race == FACTION_CYBORG && building == BUILDING_SLOT_MAGE_GUILD)
        return const_cast<char*>(localization::Tr("town.cyborg.cybernetics_lab.name"));
    if (IsWellDisabled() && race == FACTION_NECROMANCER && building == BUILDING_SLOT_WELL)
        return const_cast<char*>(localization::Tr("town.poisoned_well.name"));
    if (building == BUILDING_SLOT_WELL_EXTRA)
        return gWellExtraNames[H2EnumIndex(race)];
    else if (building == BUILDING_SLOT_SPECIAL)
        return gSpecialBuildingNames[H2EnumIndex(race)];
    else if (building < BUILDING_SLOT_DWELLING_FIRST)
        return gNeutralBuildingNames[H2EnumIndex(building)];
    else
        return GetDwellingName(
            H2EnumIndex(race), H2EnumIndex(building) - H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)
        );
}

void GetBuildingCost(FactionType race, BuildingSlotType building, i32* const dest, i32 mageLevel) {
    i32 level;
    if (building == BUILDING_SLOT_NECROMANCER_SHRINE && race == FACTION_NECROMANCER) {
        memcpy(dest, xShrineBuildingCost, KB_BUILDING_RESOURCE_COUNT * sizeof(i32));
    } else if (building >= BUILDING_SLOT_DWELLING_FIRST
               && building <= BUILDING_SLOT_DWELLING_LAST) {
        memcpy(
            dest,
            gDwellingCosts[H2EnumIndex(race)][H2EnumIndex(building) - H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)],
            KB_BUILDING_RESOURCE_COUNT * sizeof(i32)
        );
    } else if (building == BUILDING_SLOT_MAGE_GUILD) {
        level = mageLevel + 1;
        if (level > KB_MAGE_GUILD_MAX_LEVEL)
            level = KB_MAGE_GUILD_MAX_LEVEL;
        memcpy(dest, gMageBuildingCosts[mageLevel + 1], KB_BUILDING_RESOURCE_COUNT * sizeof(i32));
    } else if (building == BUILDING_SLOT_SPECIAL) {
        memcpy(dest, gSpecialBuildingCosts[H2EnumIndex(race)], KB_BUILDING_RESOURCE_COUNT * sizeof(i32));
    } else {
        if (building >= BUILDING_SLOT_DISABLED_SECOND)
            return;
        memcpy(
            dest,
            gNeutralBuildingCosts[H2EnumIndex(building)],
            KB_BUILDING_RESOURCE_COUNT * sizeof(i32)
        );
    }
}

const char* GetMonsterName(CreatureType monster) {
    return gArmyNames[H2EnumIndex(monster)];
}

const char* GetMonsterPluralName(CreatureType monster) {
    return gArmyNamesPlural[H2EnumIndex(monster)];
}

void GetMonsterCost(CreatureType monster, i32* const cost) {
    Ironfist_GetMonsterCost(H2EnumIndex(monster), cost);
}

i32 CanBuild(town* t, BuildingSlotType building) {
    i32 reqBits;
    i32 curMask;
    if (H2BitTest(gpGame->m_knownTowns, t->m_id))
        return 0;
    if (Ironfist_BuildingDisallowed(t, H2EnumIndex(building)))
        return 0;
    if (building != BUILDING_SLOT_CASTLE && !(t->m_buildings & H2EnumIndex(TOWN_BUILDING_CASTLE)))
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
         && (t->m_buildings & H2EnumIndex(KB_DWELLING_UPGRADE_FIRST_FLAG)))
        || (building == BUILDING_SLOT_DWELLING_THIRD
            && (t->m_buildings & H2EnumIndex(KB_DWELLING_UPGRADE_SECOND_FLAG)))
        || (building == BUILDING_SLOT_DWELLING_FOURTH
            && (t->m_buildings & H2EnumIndex(KB_DWELLING_UPGRADE_THIRD_FLAG)))
        || (building == BUILDING_SLOT_DWELLING_FIFTH
            && (t->m_buildings & H2EnumIndex(KB_DWELLING_UPGRADE_FOURTH_FLAG)))
        || (building == BUILDING_SLOT_DWELLING_SIXTH
            && ((t->m_buildings & H2EnumIndex(KB_DWELLING_UPGRADE_FIFTH_FLAG))
                || (t->m_buildings & H2EnumIndex(KB_DWELLING_UPGRADE_SIXTH_FLAG))))
        || (building == BUILDING_SLOT_UPGRADE_LAST
            && (t->m_buildings & H2EnumIndex(KB_DWELLING_UPGRADE_SIXTH_FLAG))))
        return 0;
    reqBits = gHierarchyMask[H2EnumIndex(t->m_type)][H2EnumIndex(building) - H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)];
    curMask = t->m_buildings;
    if (curMask & H2EnumIndex(KB_DWELLING_UPGRADE_FIRST_FLAG))
        curMask |= H2EnumIndex(KB_DWELLING_FIRST_FLAG);
    if (curMask & H2EnumIndex(KB_DWELLING_UPGRADE_SECOND_FLAG))
        curMask |= H2EnumIndex(KB_DWELLING_SECOND_FLAG);
    if (curMask & H2EnumIndex(KB_DWELLING_UPGRADE_THIRD_FLAG))
        curMask |= H2EnumIndex(KB_DWELLING_THIRD_FLAG);
    if (curMask & H2EnumIndex(KB_DWELLING_UPGRADE_FOURTH_FLAG))
        curMask |= H2EnumIndex(KB_DWELLING_FOURTH_FLAG);
    if (curMask & H2EnumIndex(KB_DWELLING_UPGRADE_SIXTH_FLAG))
        curMask |= H2EnumIndex(KB_DWELLING_UPGRADE_FIFTH_FLAG);
    if (curMask & H2EnumIndex(KB_DWELLING_UPGRADE_FIFTH_FLAG))
        curMask |= H2EnumIndex(KB_DWELLING_FIFTH_FLAG);
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
    const i32 raceIndex = H2EnumIndex(race);
    if (raceIndex < 0 || raceIndex >= KB_FACTION_TABLE_CAPACITY)
        return 0;

    if (race == FACTION_NECROMANCER && building == BUILDING_SLOT_UPGRADE_CASTLE)
        return NECROMANCER_CASTLE_UPGRADE_BASE_RESOURCE_VALUE;
    if (building < BUILDING_SLOT_DWELLING_FIRST || building > BUILDING_SLOT_DWELLING_LAST) {
        if (building > BUILDING_SLOT_NEUTRAL_LAST)
            return 0;
        else if (building == BUILDING_SLOT_MAGE_GUILD && level >= 0 && level < KB_MAGE_GUILD_LEVEL_COUNT)
            return gMageBaseResourceValues[level];
        else if (building == BUILDING_SLOT_SPECIAL)
            return gSpecialBuildingBaseResourceValues[raceIndex];
        else
            return gNeutralBaseResourceValues[H2EnumIndex(building)];
    } else {
        return gDwellingBaseResourceValues[raceIndex]
                                          [H2EnumIndex(building) - H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)];
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
            giTerrainToMusicTrack[H2EnumIndex(gpAdvManager->m_currentTerrain)]
        );
    if (giDialogTimeout != 0 && platform::Ticks() > giDialogTimeout) {
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
                                cLuckInfo[H2EnumIndex(LUCK_INFO_GOOD)],
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
                                cLuckInfo[H2EnumIndex(LUCK_INFO_BAD)],
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
                                cMoraleInfo[H2EnumIndex(MORALE_INFO_GOOD)],
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
                                cMoraleInfo[H2EnumIndex(MORALE_INFO_BAD)],
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
                                localization::Tr("help.experience"),
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
                            if (resExtra == H2EnumIndex(ARTIFACT_SPELL_SCROLL)) {
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
                                resExtra / SECONDARY_SKILL_VALUE_LEVEL_COUNT
                                        == CYBERNETICS_SKILL_ROW
                                    ? cyberneticsDesc
                                          [resExtra % SECONDARY_SKILL_VALUE_LEVEL_COUNT]
                                    : cSecSkillDesc
                                          [resExtra / SECONDARY_SKILL_VALUE_LEVEL_COUNT]
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
                                localization::Tr("help.resources"),
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
    b32 showedDialog_o;
    b32 defeated_m;
    b32 allowNormalVictory;

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
    i32 carryoverHeroId;

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
                    utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("player.eliminated"));
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
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

    survivingHumans_a = 0;
    lastHuman_a = 0;
    netHumanCount = 0;
    for (player = 0; player < gpGame->m_playerCount; player++) {
        if (!gpGame->m_playerDead[player]) {
            numAlive++;

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
                utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("victory.side_triumph"));
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
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (gpGame->m_mapHeader.lossCondition == MAP_LOSS_TOWN) {
        lossTown = gpGame->GetTown(
            gpGame->GetTownId(gpGame->m_mapHeader.lossConditionValue, gpGame->m_mapHeader.lossTownY)
        );
        if (lossTown->m_owner == TOWN_OWNER_NONE || !gbHumanPlayer[lossTown->m_owner]) {
            defeated_m = true;
            if (!showedDialog_o) {
                showedDialog_o = true;
                sprintf(gText, localization::Tr("loss.town_fallen"), lossTown->m_name);
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_ACCUMULATE_GOLD) {
        bestGold = 0;
        winnerPlayer_m = END_GAME_NO_PLAYER;
        for (player = 0; player < gpGame->m_playerCount; player++) {
            if ((gbHumanPlayer[player] || gpGame->m_mapHeader.computerAlsoWins)
                && gpGame->m_players[player].m_resources[H2EnumIndex(RES_GOLD)]
                       >= gpGame->m_mapHeader.victoryConditionValue * END_GAME_GOLD_SCALE
                && gpGame->m_players[player].m_resources[H2EnumIndex(RES_GOLD)] >= bestGold) {
                bestGold = gpGame->m_players[player].m_resources[H2EnumIndex(RES_GOLD)];
                winnerPlayer_m = player;
            }
            if (winnerPlayer_m != END_GAME_NO_PLAYER) {
                if (gbThisNetHumanPlayer[H2EnumIndex(winnerPlayer_m)]) {
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
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }
    }

    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_HERO) {
        winningHeroEntry_g = GetHeroSlot(gpGame->m_mapHeader.victoryConditionValue);
        if (winningHeroEntry_g->m_owner < 0 || winningHeroEntry_g->m_owner >= GAME_PLAYER_COUNT
            || gbHumanPlayer[winningHeroEntry_g->m_owner]) {
            winFlag = true;
            if (!showedDialog_o) {
                showedDialog_o = true;
                sprintf(
                    gText,
                    localization::Tr("victory.hero_captured"),
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
            defeated_m = true;
            if (!showedDialog_o) {
                showedDialog_o = true;
                sprintf(gText, localization::Tr("loss.hero"), lossHero_k->m_name);
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (gpGame->m_mapHeader.lossCondition == MAP_LOSS_TIME) {
        if (gpGame->m_day + (gpGame->m_week - 1) * CALENDAR_DAYS_PER_WEEK
                + (gpGame->m_month - 1) * CALENDAR_DAYS_PER_MONTH
            > gpGame->m_mapHeader.lossConditionValue) {
            defeated_m = true;
            if (!showedDialog_o) {
                showedDialog_o = true;
                utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("loss.time_expired"));
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
                winFlag = true;
            } else {
                defeated_m = true;
            }
            if (!showedDialog_o) {
                showedDialog_o = true;
                if (gpGame->m_mapHeader.victoryConditionValue == END_GAME_ULTIMATE_ARTIFACT) {
                    utf8::Copy(
                        artifactName, sizeof(artifactName),
                        localization::Tr("artifact.ultimate_generic")

                    );
                } else {
                    utf8::Copy(
                        artifactName, sizeof(artifactName),
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
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
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
                utf8::Copy(
                    gText, GLOBAL_TEXT_BUFFER_SIZE,
                    localization::Tr("campaign.loss.dwarf_towns")
                );
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ARCHIBALD
        && gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET == END_GAME_SIDE_SCENARIO
        && dragonCityCaptured) {
        winFlag = true;
        if (!showedDialog_o) {
            showedDialog_o = true;
            utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("campaign.victory.dragon_city"));
            NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
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
                utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("campaign.loss.roland_captured"));
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
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
                utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("victory.side_triumph"));
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
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
            ->m_campaignScenarioCompleted[H2EnumIndex(gpGame->m_campaignType)][gpGame->m_campaignScenario] =
            1;
        gpGame->m_campaignScenarioDays[H2EnumIndex(gpGame->m_campaignType)][gpGame->m_campaignScenario] =
            currentDayIndex;
        gpGame->m_campaignScore =
            gpGame->m_campaignScenarioBonus[H2EnumIndex(gpGame->m_campaignType)][gpGame->m_campaignScenario]
            + gpGame->m_campaignScenarioDays[H2EnumIndex(gpGame->m_campaignType)]
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
                    || H2EnumIndex(gpGame->m_heroRecs[gpGame->m_players[0].m_heroIds[campaignHeroIndex]]
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
                save_names::VictoryPrefix,
                gpGame->m_campaignType == CAMPAIGN_ROLAND ? 'G' : 'E',
                gpGame->m_campaignScenario + 1
            );
            gpGame->SaveGame(campaignSaveName, 1, 0);
        }
    } else if (xIsPlayingExpansionCampaign && winFlag) {
        xCampaign.Autosave();
    }

    bInCheckEndGame = false;
    Ironfist_CheckEndGame();
}

void QuickViewWait(void) {
    tag_message event;
    b32 done;
    gpMouseManager->ReallyHidePointer();
    done = false;
    while (!done) {
        PollSound();
        platform::PumpEvents();
        event = gpInputManager->GetEvent();
        done = event.type == MESSAGE_RIGHT_BUTTON_UP || event.type == MESSAGE_LEFT_BUTTON_DOWN
            || event.type == MESSAGE_LEFT_BUTTON_UP;
    }
    gpMouseManager->ReallyShowPointer();
}

void InitVars(void) {
    i32 i;

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
        hmnuDflt = platform::LoadMenu("mnuDflt");
        hmnuCmbt = platform::LoadMenu("mnuCmbt");
        hmnuAdv = platform::LoadMenu("mnuAdvD");
        hmnuTown = platform::LoadMenu("mnuTownD");
    } else {
        hmnuDflt = platform::LoadMenu("mnuDflt");
        hmnuCmbt = platform::LoadMenu("mnuCmbt");
        hmnuAdv = platform::LoadMenu("mnuAdv");
        hmnuTown = platform::LoadMenu("mnuTown");
    }
}

void game::ShowMoraleInfo(hero* h, i32 dialogType) {
    b32 mixedUndead4;
    i32 alignment_e;
    ArmyGroupAlignmentResult homogeneous5;
    i32 modifierStart;
    char description7[MORALE_LUCK_DESCRIPTION_SIZE];
    i32 slot8;

    mixedUndead4 = false;
    if (h->m_army.GetMorale(h, h->GetOccupiedTown(), NULL) > 0)
        utf8::Copy(description7, sizeof(description7), cMoraleInfo[H2EnumIndex(MORALE_INFO_GOOD)]);
    else {
        if (h->m_army.GetMorale(h, h->GetOccupiedTown(), NULL) == 0)
            utf8::Copy(description7, sizeof(description7), cMoraleInfo[H2EnumIndex(MORALE_INFO_NEUTRAL)]);
        else
            utf8::Copy(description7, sizeof(description7), cMoraleInfo[H2EnumIndex(MORALE_INFO_BAD)]);
    }

    sprintf(gText, cMoraleInfo[H2EnumIndex(MORALE_INFO_HEADER)], description7);
    modifierStart = strlen(gText);
    if (h->m_army.HasAllUndead()) {
        strcat(gText, cMoraleInfo[H2EnumIndex(INFO_ALL_UNDEAD)]);
        goto showDialog;
    }
    if (h->m_army.HasSomeUndead() || h->HasArtifact(ARTIFACT_ARM_OF_MARTYR)) {
        strcat(gText, cMoraleInfo[H2EnumIndex(INFO_SOME_UNDEAD)]);
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
                alignment_e = H2EnumIndex(gMonsterDatabase[H2EnumIndex(h->m_army.m_creatureTypes[slot8])].race);
            }
        }
        sprintf(description7, cMoraleInfo[H2EnumIndex(INFO_SAME_ALIGNMENT)], gAlignmentNames[alignment_e]);
        strcat(gText, description7);
    }
    if (homogeneous5 == ARMY_GROUP_ALIGNMENT_THREE) {
        utf8::Copy(description7, sizeof(description7), cMoraleInfo[H2EnumIndex(INFO_THREE_ALIGNMENTS)]);
        strcat(gText, description7);
    }
    if (homogeneous5 == ARMY_GROUP_ALIGNMENT_FOUR) {
        utf8::Copy(description7, sizeof(description7), cMoraleInfo[H2EnumIndex(INFO_FOUR_ALIGNMENTS)]);
        strcat(gText, description7);
    }
    if (homogeneous5 == ARMY_GROUP_ALIGNMENT_FIVE_OR_MORE) {
        utf8::Copy(description7, sizeof(description7), cMoraleInfo[H2EnumIndex(INFO_FIVE_ALIGNMENTS)]);
        strcat(gText, description7);
    }

    if (h->GetOccupiedTown() != NULL && h->GetOccupiedTown()->m_type == FACTION_BARBARIAN
        && (h->GetOccupiedTown()->m_buildings & H2EnumIndex(TOWN_BUILDING_COLISEUM))) {
        strcat(gText, cMoraleInfo[H2EnumIndex(INFO_COLISEUM)]);
    }
    if (h->GetOccupiedTown() != NULL
        && (h->GetOccupiedTown()->m_buildings & H2EnumIndex(TOWN_BUILDING_TAVERN))) {
        strcat(gText, cMoraleInfo[H2EnumIndex(INFO_TAVERN)]);
    }

    if (h->HasArtifact(ARTIFACT_MEDAL_OF_VALOR)) {
        strcat(gText, cMoraleInfo[H2EnumIndex(INFO_MEDAL_OF_VALOR)]);
    }
    if (h->HasArtifact(ARTIFACT_MEDAL_OF_COURAGE)) {
        strcat(gText, cMoraleInfo[H2EnumIndex(INFO_MEDAL_OF_COURAGE)]);
    }
    if (h->HasArtifact(ARTIFACT_MEDAL_OF_HONOR)) {
        strcat(gText, cMoraleInfo[H2EnumIndex(INFO_MEDAL_OF_HONOR)]);
    }
    if (h->HasArtifact(ARTIFACT_MEDAL_OF_DISTINCTION)) {
        strcat(gText, cMoraleInfo[H2EnumIndex(INFO_MEDAL_OF_DISTINCTION)]);
    }
    if (h->HasArtifact(ARTIFACT_FIZBIN_OF_MISFORTUNE)) {
        strcat(gText, cMoraleInfo[H2EnumIndex(INFO_FIZBIN)]);
    }
    if ((H2EnumIndex((h->m_eventFlags) & (HERO_EVENT_BUOY)))) {
        strcat(gText, cMoraleInfo[H2EnumIndex(INFO_BUOY)]);
    }
    if ((H2EnumIndex((h->m_eventFlags) & (HERO_EVENT_OASIS)))) {
        strcat(gText, cMoraleInfo[H2EnumIndex(INFO_OASIS)]);
    }
    if ((H2EnumIndex((h->m_eventFlags) & (HERO_EVENT_TEMPLE)))) {
        strcat(gText, cMoraleInfo[H2EnumIndex(INFO_TEMPLE)]);
    }
    if ((H2EnumIndex((h->m_eventFlags) & (HERO_EVENT_GRAVEYARD)))) {
        strcat(gText, cMoraleInfo[H2EnumIndex(INFO_GRAVEYARD)]);
    }
    if ((H2EnumIndex((h->m_eventFlags) & (HERO_EVENT_SHIPWRECK)))) {
        strcat(gText, cMoraleInfo[H2EnumIndex(INFO_SHIPWRECK)]);
    }
    if ((H2EnumIndex((h->m_eventFlags) & (HERO_EVENT_WATERING_HOLE)))) {
        strcat(gText, cMoraleInfo[H2EnumIndex(INFO_WATERING_HOLE)]);
    }
    if ((H2EnumIndex((h->m_eventFlags) & (HERO_EVENT_DERELICT_SHIP)))) {
        strcat(gText, cMoraleInfo[H2EnumIndex(INFO_DERELICT_SHIP)]);
    }
    if (h->m_secondarySkills[H2EnumIndex(HERO_SKILL_LEADERSHIP)] == HERO_SKILL_LEVEL_BASIC) {
        strcat(gText, cMoraleInfo[H2EnumIndex(INFO_BASIC_LEADERSHIP)]);
    }
    if (h->m_secondarySkills[H2EnumIndex(HERO_SKILL_LEADERSHIP)] == HERO_SKILL_LEVEL_ADVANCED) {
        strcat(gText, cMoraleInfo[H2EnumIndex(INFO_ADVANCED_LEADERSHIP)]);
    }
    if (h->m_secondarySkills[H2EnumIndex(HERO_SKILL_LEADERSHIP)] == HERO_SKILL_LEVEL_EXPERT) {
        strcat(gText, cMoraleInfo[H2EnumIndex(INFO_EXPERT_LEADERSHIP)]);
    }
    if (h->HasArtifact(ARTIFACT_MASTHEAD) && (H2EnumIndex((h->m_eventFlags) & (HERO_EVENT_EMBARKED)))) {
        strcat(gText, cMoraleInfo[H2EnumIndex(MORALE_INFO_MASTHEAD)]);
    }
    if (h->HasArtifact(ARTIFACT_BATTLE_GARB)) {
        strcat(gText, cMoraleInfo[H2EnumIndex(MORALE_INFO_BATTLE_GARB)]);
    }
showDialog:
    Ironfist_AppendMoraleInfo(h);
    if (modifierStart == static_cast<i32>(strlen(gText))) {
        strcat(gText, cMoraleInfo[H2EnumIndex(MORALE_INFO_NONE)]);
    }
    NormalDialog(gText, dialogType, -1, -1, -1, 0, -1, 0, -1, 0);
}

void game::ShowLuckInfo(hero* h, i32 dialogType) {
    char description4[MORALE_LUCK_DESCRIPTION_SIZE];

    i32 modifierStart;

    if (gpGame->GetLuck(h, NULL, h->GetOccupiedTown()) > 0)
        utf8::Copy(description4, sizeof(description4), cLuckInfo[H2EnumIndex(LUCK_INFO_GOOD)]);
    else {
        if (gpGame->GetLuck(h, NULL, h->GetOccupiedTown()) == 0)
            utf8::Copy(description4, sizeof(description4), cLuckInfo[H2EnumIndex(LUCK_INFO_NEUTRAL)]);
        else
            utf8::Copy(description4, sizeof(description4), cLuckInfo[H2EnumIndex(LUCK_INFO_BAD)]);
    }

    sprintf(gText, cLuckInfo[H2EnumIndex(LUCK_INFO_HEADER)], description4);
    modifierStart = strlen(gText);
    if (h->GetOccupiedTown() != NULL && h->GetOccupiedTown()->m_type == FACTION_SORCERESS
        && (h->GetOccupiedTown()->m_buildings & H2EnumIndex(TOWN_BUILDING_RAINBOW)))
        strcat(gText, cLuckInfo[H2EnumIndex(INFO_RAINBOW)]);
    if (h->HasArtifact(ARTIFACT_RABBIT_FOOT))
        strcat(gText, cLuckInfo[H2EnumIndex(INFO_RABBIT_FOOT)]);
    if (h->HasArtifact(ARTIFACT_GOLDEN_HORSESHOE))
        strcat(gText, cLuckInfo[H2EnumIndex(INFO_HORSESHOE)]);
    if (h->HasArtifact(ARTIFACT_GAMBLERS_COIN))
        strcat(gText, cLuckInfo[H2EnumIndex(INFO_LUCKY_COIN)]);
    if (h->HasArtifact(ARTIFACT_FOUR_LEAF_CLOVER))
        strcat(gText, cLuckInfo[H2EnumIndex(INFO_CLOVER)]);
    if ((H2EnumIndex((h->m_eventFlags) & (HERO_EVENT_FAERIE_RING))))
        strcat(gText, cLuckInfo[H2EnumIndex(INFO_FAERIE_RING)]);
    if ((H2EnumIndex((h->m_eventFlags) & (HERO_EVENT_IDOL))))
        strcat(gText, cLuckInfo[H2EnumIndex(INFO_IDOL)]);
    if ((H2EnumIndex((h->m_eventFlags) & (HERO_EVENT_FOUNTAIN))))
        strcat(gText, cLuckInfo[H2EnumIndex(INFO_FOUNTAIN)]);
    if ((H2EnumIndex((h->m_eventFlags) & (HERO_EVENT_PYRAMID))))
        strcat(gText, cLuckInfo[H2EnumIndex(INFO_PYRAMID)]);
    if (h->m_secondarySkills[H2EnumIndex(HERO_SKILL_LUCK)] == HERO_SKILL_LEVEL_BASIC)
        strcat(gText, cLuckInfo[H2EnumIndex(INFO_BASIC_SKILL)]);
    if (h->m_secondarySkills[H2EnumIndex(HERO_SKILL_LUCK)] == HERO_SKILL_LEVEL_ADVANCED)
        strcat(gText, cLuckInfo[H2EnumIndex(INFO_ADVANCED_SKILL)]);
    if (h->m_secondarySkills[H2EnumIndex(HERO_SKILL_LUCK)] == HERO_SKILL_LEVEL_EXPERT)
        strcat(gText, cLuckInfo[H2EnumIndex(INFO_EXPERT_SKILL)]);
    if (h->HasArtifact(ARTIFACT_MASTHEAD) && (H2EnumIndex((h->m_eventFlags) & (HERO_EVENT_EMBARKED))))
        strcat(gText, cLuckInfo[H2EnumIndex(LUCK_INFO_MASTHEAD)]);
    if ((H2EnumIndex((h->m_eventFlags) & (HERO_EVENT_MERMAID))))
        strcat(gText, cLuckInfo[H2EnumIndex(INFO_MERMAID)]);
    if (h->HasArtifact(ARTIFACT_BATTLE_GARB))
        strcat(gText, cLuckInfo[H2EnumIndex(LUCK_INFO_BATTLE_GARB)]);
    Ironfist_AppendLuckInfo(h);
    if (static_cast<i32>(strlen(gText)) == modifierStart)
        strcat(gText, cLuckInfo[H2EnumIndex(LUCK_INFO_NONE)]);

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
    for (idx = H2EnumIndex(CREATURE_COUNT) - 1; idx >= 0; idx--) {
        if (campaign == HIGH_SCORE_CAMPAIGN || campaign == HIGH_SCORE_EXPANSION_CAMPAIGN) {
            if (score <= giScoreCampaignMon[idx][H2EnumIndex(MONSTER_SCORE_THRESHOLD)])
                return giScoreCampaignMon[idx][H2EnumIndex(MONSTER_SCORE_TYPE)];
        } else {
            if (score >= giScoreMon[idx][H2EnumIndex(MONSTER_SCORE_THRESHOLD)])
                return giScoreMon[idx][H2EnumIndex(MONSTER_SCORE_TYPE)];
        }
    }
    return giScoreMon[0][H2EnumIndex(MONSTER_SCORE_TYPE)];
}

i32 AddScoreToHighScore(
    i32 score,
    i32 days,
    i32 scenario,
    HighScoreType highScoreType,
    const char* scenarioName
) {
    i32 dest_o;
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

    file_c = platform::FileOpen(filename_h, platform::FileMode::Read);
    if (file_c == -1)
        missingFile_e = true;
    if (missingFile_e) {
        for (entry_a = 0; entry_a < HIGH_SCORE_ENTRY_COUNT; entry_a++) {
            memset(&entries_a[entry_a], 0, sizeof(HighScoreEntry));
            entries_a[entry_a].score = HIGH_SCORE_EMPTY;
        }
    } else {
        for (entry_a = 0; entry_a < HIGH_SCORE_ENTRY_COUNT; entry_a++)
            platform::FileRead(file_c, &entries_a[entry_a], sizeof(entries_a));
        platform::FileClose(file_c);
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

        file_c = platform::FileOpen(filename_h, platform::FileMode::Write);
        if (file_c == -1)
            FileError(filename_h);
        for (entry_a = 0; entry_a < HIGH_SCORE_ENTRY_COUNT; entry_a++)
            platform::FileWrite(file_c, &entries_a[entry_a], sizeof(HighScoreEntry));
        platform::FileClose(file_c);
    } else {
        gbShowHighScore = false;
    }
    return 0;
}

void BVResMsg(const char* s, ResourceType res, i32 qty) {
    giBottomViewOverride = BOTTOM_VIEW_RESOURCE;
    giBottomViewOverrideEndTime = platform::Ticks() + BOTTOM_VIEW_RESOURCE_MESSAGE_DURATION;
    giBottomViewResource = res;
    giBottomViewResourceQty = qty;
    strcpy(gcBottomViewText, s);
    gpAdvManager->UpdBottomView(1, 1, 1);
}

void GOut(const char* str) {
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
                gbUseBzip2Compression = data->payload.setup.useBzip2Compression;
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

    i32l messageTime_b;
    heroWindow* netWindow_j;
    i32 result_p;
    i32 textWidth_b;

    i32 savedShowIt_p;
    b32 updateInput_f;
    i32 inputLength_a;
    char inputText_b[BOX_TEXT_LENGTH];
    b32 exitForIncomingData_c;
    b32 sendText_b;
    tag_message event_o;
    tag_message updateMessage_i;

    i32 delay_e;

    b32 done_a;
    b32 redrawLines_l;
    i32 redrawSavedShowIt_a;
    KbRemotePacket* remoteData_g;
    b32 redrawAdventure_o;
    i32 cursorState_j;

    if (!gbRemoteOn)
        return;

    messageTime_b = 0;
    if (text != NULL) {
        if (netPlayer >= 0) {
            sprintf(gText, "%s:  %s", gsNetPlayerInfo[netPlayer].name, text);
            gText[BOX_LINE_TEXT_LIMIT] = 0;
            AddNetBoxLine(gText, gpGame->m_players[NetPosToGamePos(netPlayer)].m_color);
        } else {
            utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, text);
            gText[BOX_LINE_TEXT_LIMIT] = 0;
            AddNetBoxLine(gText, BOX_DEFAULT_COLOR);
        }
        messageTime_b = platform::Ticks();
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
                            bShowIt = 1;
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
                            messageTime_b = platform::Ticks();
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

        platform::PumpEvents();
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

        if (!updateInput_f && glTimers[GLOBAL_NET_BOX_CURSOR_TIMER_SLOT] < platform::Ticks()) {
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
                1,
                1,
                REMOTE_MESSAGE_DEFAULT
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
            updateInput_f = false;
            glTimers[GLOBAL_NET_BOX_CURSOR_TIMER_SLOT] = platform::Ticks() + BOX_CURSOR_DELAY;
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

        if (messageTime_b != 0 && messageTime_b + BOX_MESSAGE_TIMEOUT < platform::Ticks())
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
        bShowIt = 1;
        gpAdvManager->RedrawAdvScreen(1, 0);
        bShowIt = redrawSavedShowIt_a;
    } else {
        gpWindowManager->RemoveWindow(netWindow_j);
    }
    bShowIt = savedShowIt_p;
}

void AddNetBoxLine(const char* str, char color) {
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

void ShutDown(const char* msg) {
    char buf[GLOBAL_TEXT_BUFFER_SIZE];
    if (bInShutDown)
        return;
    LogStr("Shutdown");
    bInShutDown = true;
    gbClosingApp = true;
    buf[0] = 0;
    gpMouseManager->SetColorMice(0);
    if (msg) {
        strcpy(buf, msg);
        SetFullScreenStatus(0);
        LogStr(buf);
        platform::ShowMessage(
            localization::Tr("system.unexpected_termination"),
            buf
        );
    } else {
        utf8::Copy(buf, sizeof(buf), localization::Tr("system.goodbye"));
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
    if (mapExtra)
        H2_FREE(mapExtra);
    mapExtra = NULL;
    CloseAIMapVars();
    DeleteMainClasses();
    Ironfist_Shutdown();
    CleanUpWinGraphics();
    CleanUpMenus();
    PrintMemoryLeaks();
    if (gpMemEntry)
        free(gpMemEntry);
    gpMemEntry = NULL;
    exit(0);
}

typedef enum FileErrorConstant {
    FILE_ERROR_BUFFER_SIZE = 500
} FileErrorConstant;

void FileError(const char* filename) {
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
        localization::Tr("system.file.open_error"),
        filename
    );
    ShutDown(buf);
}

typedef enum SmackFadeConstant {
    SMACK_FADE_FIRST_COLOR = GRAPHICS_SYSTEM_PALETTE_SIZE,
    SMACK_FADE_COLOR_LIMIT = GRAPHICS_PALETTE_SIZE - GRAPHICS_SYSTEM_PALETTE_SIZE,
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
    g = static_cast<u8*>(H2_ALLOC(GRAPHICS_PALETTE_SIZE));
    memset(l, 0, MISC_PALETTE_BYTE_COUNT);
    memset(g, 0, GRAPHICS_PALETTE_SIZE);
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
        utf8::Copy(
            ratingText,
            sizeof(ratingText),
            gArmyNames[GetMonType(realScore, highScoreType)]
        );
    } else if (highScoreType == HIGH_SCORE_EXPANSION_CAMPAIGN) {
        utf8::Copy(
            ratingText,
            sizeof(ratingText),
            gArmyNames[GetMonType(xCampaign.Days(), highScoreType)]
        );
    } else {
        utf8::Copy(
            ratingText,
            sizeof(ratingText),
            gArmyNames[GetMonType(gpGame->m_campaignScore, highScoreType)]
        );
    }
    utf8::UppercaseFirst(ratingText);
    if (static_cast<i8>(gpGame->m_cheated))
        utf8::Copy(ratingText, sizeof(ratingText), localization::Tr("high_score.cheater_rating"));

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

void CongratsWait(void) {

    b32 done = false;
    tag_message msg;
    gpInputManager->Flush();
    while (!done) {
        PollSound();
        platform::PumpEvents();
        msg = gpInputManager->GetEvent();
        if (msg.type == MESSAGE_KEY_DOWN || msg.type == MESSAGE_LEFT_BUTTON_DOWN
            || msg.type == MESSAGE_LEFT_BUTTON_UP || msg.type == MESSAGE_RIGHT_BUTTON_DOWN
            || msg.type == MESSAGE_RIGHT_BUTTON_UP)
            done = true;
    }
}

typedef enum SamplePlaybackConstant {
    SAMPLE_PLAYBACK_CHANNEL_GROUP = 2,
    SAMPLE_DEFAULT_WAIT_TIME = 4000
} SamplePlaybackConstant;

SAMPLE2 LoadPlaySample(const char* name) {
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
    endTime = platform::Ticks() + waitTime;
    while (gpSoundManager->DigitalReport(*s) && platform::Ticks() < endTime) {
        platform::PumpEvents();
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
        localization::Tr("system.memory.out_of_memory")  ,
        MEMORY_ERROR_REQUEST_SIZE
    );
    ShutDown(gText);
}

const char* GetTownName(i32 i) {
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
            strcpy(gText, localization::Tr("game.confirm.restart"));
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
            platform::RequestQuit();
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
                for (loopIndex = H2EnumIndex(SPELL_FIREBALL); loopIndex < APP_MENU_MAX_SPELLS; loopIndex++)
                    currentHeroRec->AddSpell(
                        static_cast<SpellType>(loopIndex),
                        APP_MENU_SPELL_COUNT
                    );
                currentHeroRec->m_spellPoints = APP_MENU_CHEAT_SPELL_POINTS;
            }
            break;

        case APP_MENU_CHEAT_AUTO_WIN:
            gbAutoWinBattles = !gbAutoWinBattles;
            break;

        case APP_MENU_CHEAT_RESOURCES:
            gpGame->m_cheated = 1;
            if (gbInCampaign)
                gpGame->m_campaignCheated = 1;
            for (loopIndex = 0; loopIndex < APP_MENU_RESOURCE_COUNT; loopIndex++)
                gpCurPlayer->m_resources[loopIndex] += loopIndex == H2EnumIndex(RES_GOLD)
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
                    &gpCombatManager->m_armies[H2EnumIndex(gpCombatManager->m_currentArmySide)]
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

    if (gConfig.gfx[H2EnumIndex(giCurExe)].showMenu == 0)
        return;
    const platform::MenuHandle currentMenu = platform::CurrentMenu();
    if (currentMenu != hmnuAdv)
        return;

    for (menuCommand = APP_MENU_MUSIC_FIRST; menuCommand <= APP_MENU_MUSIC_LAST; menuCommand++)
        platform::SetMenuItemChecked(currentMenu, menuCommand, false);
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
    platform::SetMenuItemChecked(currentMenu, checkedCommand, true);

    for (menuCommand = APP_MENU_SOUND_FIRST; menuCommand <= APP_MENU_SOUND_LAST; menuCommand++)
        platform::SetMenuItemChecked(currentMenu, menuCommand, false);
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
    platform::SetMenuItemChecked(currentMenu, checkedCommand, true);

    for (menuCommand = APP_MENU_SPEED_FIRST; menuCommand <= APP_MENU_SPEED_LAST; menuCommand++)
        platform::SetMenuItemChecked(currentMenu, menuCommand, false);
    platform::SetMenuItemChecked(currentMenu, APP_MENU_TOGGLE_ROUTE, gConfig.showRoute != 0);
    platform::SetMenuItemChecked(
        currentMenu,
        APP_MENU_TOGGLE_BLACKOUT,
        gConfig.blackoutComputer == 0
    );
}

void CleanUpMenus(void) {
    platform::DestroyMenu(hmnuAdv);
    platform::DestroyMenu(hmnuDflt);
    platform::DestroyMenu(hmnuCmbt);
    platform::DestroyMenu(hmnuTown);
}

void UpdateAppSpecificMenus(void* hMenu) {
    if (hMenu == hmnuAdv)
        UpdateSystemOptionsMenu();
}

void EarlyResizeWindow(i32, i32, i32, i32) {
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

    i32 numRows;
    widget* newWidgetTemp_p;
    i32 columnsSize_h;
    i32 topOffsetNum_n;

    i32 centeredHeightCount_k;

    i32 bottomOffsetLocal_p;
    i32 rightOffset_p;

    i32 edge_d;
    i32 tileRowPos_k;
    i32 centeredWidthValue_b;

    i32 columnIndex_k;

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
    b32 t;
    SetupDynamicWindow(
        0,
        0,
        1,
        GRAPHICS_WIDTH,
        GRAPHICS_HEIGHT,
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
        platform::PumpEvents();
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
    b32 destination;
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

void DropDownToOnePlayer(void) {
    RemoteCleanup();
    giNumHumanPlayers = 1;
    for (i32 i = 0; i < REMOTE_PLAYER_COUNT; i++)
        if (i != giThisNetPos)
            gbHumanPlayer[i] = false;
    ComputeAdvNetControl();
}

void ReceiveHostReportsPlayerExit(i32 hostNetPosition, SPlayerExit exitInfo, i32 forwardedReport) {
    b32 showExitMessage_i;
    char playerExitMessage_k[PLAYER_EXIT_MESSAGE_LENGTH];
    i32 netPosition;

    showExitMessage_i = false;
    if (!forwardedReport) {
        if (exitInfo.eliminated) {
            if (exitInfo.netPosition == giThisNetPos) {
                RemoteCleanup();
                utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("player.eliminated"));
                NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
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
            gpGame->SaveGame(save_names::PlayerExit, 1, 0);
            sprintf(
                gText,
                localization::Tr("network.player_exit.host_terminated"),
                gsNetPlayerInfo[exitInfo.netPosition].name,
                gsNetPlayerInfo[hostNetPosition].name,
                save_names::PlayerExit
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
    network_remove_player(exitInfo.netPosition);

    for (netPosition = exitInfo.netPosition; netPosition < REMOTE_PLAYER_COUNT - 1; netPosition++) {
        lLastHeartbeatReceive[netPosition] = lLastHeartbeatReceive[netPosition + 1];
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
    b32 localPlayerLost_e;

    i32 recipient;

    localPlayerLost_e = false;
    lLastHeartbeatReceive[exitInfo.netPosition] = PLAYER_EXIT_HEARTBEAT_DISABLED;
    gpGame->SaveGame(save_names::PlayerExit, 1, 0);

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
                localization::Tr("network.player_exit.timed_out_confirm"),
                gsNetPlayerInfo[exitInfo.netPosition].name,
                save_names::PlayerExit,
                gsNetPlayerInfo[exitInfo.netPosition].name
            );
        } else {
            sprintf(
                gText,
                localization::Tr("network.player_exit.exiting_confirm"),
                gsNetPlayerInfo[exitInfo.netPosition].name,
                save_names::PlayerExit,
                gsNetPlayerInfo[exitInfo.netPosition].name
            );
        }
        NormalDialog(gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
        exitInfo.continueGame = gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE;
    }

exitInfoProcessed:
    if (giNumHumanPlayers == PLAYER_EXIT_DIRECT_PLAYER_COUNT) {
        if (exitInfo.eliminated && !exitInfo.hostReported) {
            TransmitRemoteData(
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
        gbHumanPlayer[exitInfo.gamePosition] = false;
        RemoteCleanup();
        ComputeAdvNetControl();
    } else {
        for (recipient = 0; recipient < REMOTE_PLAYER_COUNT; recipient++) {
            if ((recipient == exitInfo.netPosition && exitInfo.eliminated && !exitInfo.hostReported)
                || (recipient != exitInfo.netPosition && recipient < giNumHumanPlayers
                    && recipient != giThisNetPos)) {
                TransmitRemoteData(
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
        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("player.eliminated"));
        RemoteCleanup();
        NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
        gbGameOver = true;
        giEndSequence = false;
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
    i32 c = gsSpellInfo[H2EnumIndex(spell)].cost;
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
    return Ironfist_CalcManaCost(h, H2EnumIndex(spell), c);
}

void SetWinText(heroWindow* j, i32 id) {
    i32 i;
    tag_message msg;
    for (i = 0; i < KB_WIN_SETUP_COUNT; i++) {
        if (gWinSetup[i].windowId == id) {
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
    if (platform::Ticks() > iNextShingleAnim) {
        iNextShingleAnim = platform::Ticks() + SHINGLE_ANIMATION_INTERVAL;
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
    const char* text,
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

    if (!gbRemoteOn)
        timeout = 0;
    if (timeout > NORMAL_DIALOG_TIMEOUT_MIN && timeout < NORMAL_DIALOG_TIMEOUT_MAX) {
        giDialogTimeout = platform::Ticks() + timeout;
    } else {
        giDialogTimeout = timeout;
    }

    resourceCenterX_c = 0;
    resourceY_f = 0;
    resourceFrame_n = 0;
    textWidgetId = NORMAL_DIALOG_TEXT_WIDGET_FIRST_ID;
    resourceImageWidth = 0;

    showPrimaryBonus = false;

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

    const std::string iconFile = "evntwin" + std::to_string(windowRows_b) + ".bin";
    pNormalDialogWindow = new heroWindow(windowX, windowY, iconFile.c_str());
    if (!pNormalDialogWindow)
        MemError();

    message_b.type = NORMAL_DIALOG_DISABLE_MESSAGE;
    message_b.payload.widget.command = NORMAL_DIALOG_DISABLE_COMMAND;
    message_b.payload.widget.data.text = reinterpret_cast<const char*>(NORMAL_DIALOG_DISABLE_COMMAND);
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
                resourceValue_c[resourceSlot] / SECONDARY_SKILL_VALUE_LEVEL_COUNT
                        == CYBERNETICS_SKILL_ROW
                    ? localization::Tr("hero.skill.cybernetics")
                    : gSecondarySkills
                          [resourceValue_c[resourceSlot]
                           / SECONDARY_SKILL_VALUE_LEVEL_COUNT]
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
                H2EnumIndex(gMonsterDatabase[resourceValue_c[resourceSlot]].race)
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

void UpdateNormalDialog(const char* text) {

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

H2EnumStorage<TerrainType, u8>
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
// The window comes up focused, and the first focus change corrects this.
b32 gbForegroundApp = true;
i32 giMainVideoModeColorDepth = GRAPHICS_COLOR_DEPTH;
i32 giMainVideoModeWidth = GRAPHICS_WIDTH;
i32 giMainVideoModeHeight = GRAPHICS_HEIGHT;
u8 gMapColors[RADAR_MAP_COLOR_COUNT] = {77, 98, 13, 104, 32, 118, 54, 206, 41, 0, 0, 0};
u8 gObjectColors[RADAR_OBJECT_COLOR_COUNT] =
    {16, 48, 98, 160, 126, 74, 110, 179, 100, 218, 12, 12, 12, 12, 12, 12};
u8 gOwnerColors[RADAR_OWNER_COLOR_COUNT] = {73, 105, 190, 114, 205, 138, 10, 0};
const char* gTilesetFiles[H2EnumIndex(TILESET_COUNT)] = {
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
u32l gTownEligibleBuildMask[KB_FACTION_TABLE_CAPACITY] = {
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
const char* cEvilTranslate[KB_INTERFACE_TYPE_COUNT][KB_INTERFACE_VARIANT_COUNT] = {
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
i32 gbPutzingWithMouseCtr = 0;
float gfCombatSpeedMod[KB_COMBAT_SPEED_COUNT] = {1.0f, 0.7f, 0.35f};
icon* gShingleAnim = NULL;
i32 iNextShingleAnim = 0;
i32 giDialogTimeout = 0;
i32 giNewMonsterCycleFrame = 0;
b32 gbNoCDRom = false;
b32 gbLeaveNetBoxAlone = false;
b32 gbDrawWindowBackground = true;
b32 gbCheatMenus = false;
b32 gbShowAllMaps = false;
const char* gCombatFxNames[KB_COMBAT_FX_COUNT] = {
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
    "stelskin.icn",
    "plasmblast.icn",
    "shdwmark.icn",
    "mrksmprc.icn",
    "plsmcone.icn",
    "forcshld.icn",
    "firebomb.icn",
    "implgrnd.icn"
};
i16 horseFrameFlip[MOVEMENT_FRAME_FLIP_COUNT] =
    {45, 46, 47, 48, 49, 50, 51, 52, 53, 179, 178, 177, 54, 175, 174, 55};
i16 boatFrameFlip[MOVEMENT_FRAME_FLIP_COUNT] =
    {0, 0, 9, 9, 18, 18, 27, 27, 36, 36, 155, 155, 146, 146, 137, 137};
i8 gCastleResources[CASTLE_RESOURCE_SLOT_COUNT] =
    {H2EnumIndex(RES_WOOD), H2EnumIndex(RES_ORE), -1, -1};
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
i32 gSSValues[H2EnumIndex(HERO_SKILL_COUNT)][SECONDARY_SKILL_VALUE_LEVEL_COUNT] = {
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
H2EnumStorage<ArtifactLevelMask, u8>
    gArtifactLevel[KB_ARTIFACT_TABLE_CAPACITY] = {
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
tag_monsterInfo gMonsterDatabase[KB_CREATURE_TABLE_CAPACITY] = {
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
i8 gbArrow[NORMAL_DIRECTION_COUNT][NORMAL_DIRECTION_COUNT] = {
    {8, false, false, false, 8, 16, 16, 16},
    {17, 9, true, true, true, 9, 17, 17},
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
i16 giScoreMon[H2EnumIndex(CREATURE_COUNT)][H2EnumIndex(MONSTER_SCORE_FIELD_COUNT)] = {
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
i16 giScoreCampaignMon[H2EnumIndex(CREATURE_COUNT)][H2EnumIndex(MONSTER_SCORE_FIELD_COUNT)] = {
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
i8 townTheme[KB_FACTION_TABLE_CAPACITY] = {
    H2EnumIndex(TOWN_MUSIC_KNIGHT),
    H2EnumIndex(TOWN_MUSIC_BARBARIAN),
    H2EnumIndex(TOWN_MUSIC_WARLOCK),
    H2EnumIndex(TOWN_MUSIC_WIZARD),
    H2EnumIndex(TOWN_MUSIC_SORCERESS),
    H2EnumIndex(TOWN_MUSIC_NECROMANCER),
    H2EnumIndex(TOWN_MUSIC_NONE),
    H2EnumIndex(TOWN_MUSIC_NONE),
    H2EnumIndex(TOWN_MUSIC_NONE),
    H2EnumIndex(TOWN_MUSIC_NONE),
    H2EnumIndex(TOWN_MUSIC_NONE),
    H2EnumIndex(TOWN_MUSIC_NONE),
    H2EnumIndex(TOWN_MUSIC_CYBORG)
};
i8
    gHeroSkillBonus[KB_FACTION_TABLE_CAPACITY][KB_HERO_LEVEL_BAND_COUNT][HERO_PRIMARY_STAT_COUNT] = {
        {{35, 45, 10, 10}, {25, 25, 25, 25}},
        {{55, 35, 5, 5}, {25, 25, 25, 25}},
        {{10, 10, 30, 50}, {20, 20, 30, 30}},
        {{10, 10, 50, 30}, {20, 20, 30, 30}},
        {{10, 10, 40, 40}, {20, 20, 30, 30}},
        {{15, 15, 35, 35}, {25, 25, 25, 25}},
        {},
        {},
        {},
        {},
        {},
        {},
        // Cyborg heroes follow the Necromancer skill chances upstream.
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
    {H2EnumIndex(platform::SystemMenuCommand::Size640x480), 1, 1, 0},
    {H2EnumIndex(platform::SystemMenuCommand::Size800x600), 1, 1, 0},
    {H2EnumIndex(platform::SystemMenuCommand::Size1024x768), 1, 1, 0},
    {H2EnumIndex(platform::SystemMenuCommand::Size1280x1024), 1, 1, 0},
    {H2EnumIndex(platform::SystemMenuCommand::Fullscreen), 1, 1, 0},
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
    {H2EnumIndex(platform::SystemMenuCommand::Help), 1, 1, 0},
    {H2EnumIndex(platform::SystemMenuCommand::About), 1, 1, 0},
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
platform::MenuHandle hmnuDflt = nullptr;
platform::MenuHandle hmnuCmbt = nullptr;
platform::MenuHandle hmnuAdv = nullptr;
platform::MenuHandle hmnuTown = nullptr;
const char* cMonFilename[KB_CREATURE_TABLE_CAPACITY] = {
    "peasant.icn",  "archer.icn",   "archer2.icn",  "pikeman.icn",  "pikeman2.icn", "swordsmn.icn",
    "swordsm2.icn", "cavalryr.icn", "cavalryb.icn", "paladin.icn",  "paladin2.icn", "goblin.icn",
    "orc.icn",      "orc2.icn",     "wolf.icn",     "ogre.icn",     "ogre2.icn",    "troll.icn",
    "troll2.icn",   "cyclops.icn",  "sprite.icn",   "dwarf.icn",    "dwarf2.icn",   "elf.icn",
    "elf2.icn",     "druid.icn",    "druid2.icn",   "unicorn.icn",  "phoenix.icn",  "centaur.icn",
    "gargoyle.icn", "griffin.icn",  "minotaur.icn", "minotau2.icn", "hydra.icn",    "draggree.icn",
    "dragred.icn",  "dragblak.icn", "halfling.icn", "boar.icn",     "golem.icn",    "golem2.icn",
    "roc.icn",      "mage1.icn",    "mage2.icn",    "titanblu.icn", "titanbla.icn", "skeleton.icn",
    "zombie.icn",   "zombie2.icn",  "mummyw.icn",   "mummy2.icn",   "vampire.icn",  "vampire2.icn",
    "lich.icn",     "lich2.icn",    "dragbone.icn", "rogue.icn",    "nomad.icn",    "ghost.icn",
    "genie.icn",    "medusa.icn",   "eelem.icn",    "aelem.icn",    "felem.icn",    "welem.icn"
};
b32 gbProcessingCombatAction = false;
RemoteNetworkProtocol iMPNetProtocol = REMOTE_PROTOCOL_NETBIOS;
i32 iLastDiffSendTo = DIFF_SEND_FORCE_WHOLE;
SSpellInfo gsSpellInfo[KB_SPELL_TABLE_CAPACITY] = {
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
    {"", 4, 55, 0, 700, 15, {0, 0, 0, 0, 0, 0}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    // Ironfist spells 65-72.
    {"", 1, 55, 0, 700, 20, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {"shdwmark",
     1,
     66,
     33,
     0,
     3,
     {255, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"mrksmprc",
     1,
     67,
     34,
     200,
     3,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT | SPELL_INFO_ATTRIBUTE_DURATION},
    {"plsmcone",
     2,
     68,
     35,
     50,
     3,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"forcshld",
     2,
     69,
     36,
     500,
     7,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"forcshld",
     3,
     70,
     36,
     500,
     7,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"firebomb",
     4,
     71,
     37,
     500,
     9,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT},
    {"implgrnd",
     5,
     72,
     38,
     500,
     9,
     {10, 10, 10, 10, 10, 10},
     SPELL_INFO_ATTRIBUTE_POWER | SPELL_INFO_ATTRIBUTE_COMBAT}
};
const char* cArmyFrameFileNames[KB_CREATURE_TABLE_CAPACITY] = {
    "peas_frm.bin", "archrfrm.bin", "archrfrm.bin", "pikmnfrm.bin", "pikmnfrm.bin", "swrdsfrm.bin",
    "swrdsfrm.bin", "cvlryfrm.bin", "cvlr2frm.bin", "paladfrm.bin", "paladfrm.bin", "goblnfrm.bin",
    "orc__frm.bin", "orc__frm.bin", "wolf_frm.bin", "ogre_frm.bin", "ogre_frm.bin", "trollfrm.bin",
    "trollfrm.bin", "cyclofrm.bin", "spritfrm.bin", "dwarffrm.bin", "dwarffrm.bin", "elf__frm.bin",
    "elf__frm.bin", "druidfrm.bin", "druidfrm.bin", "unicofrm.bin", "phoenfrm.bin", "centrfrm.bin",
    "garglfrm.bin", "grifffrm.bin", "minotfrm.bin", "minotfrm.bin", "hydrafrm.bin", "draggfrm.bin",
    "dragrfrm.bin", "dragbfrm.bin", "halflfrm.bin", "boar_frm.bin", "golemfrm.bin", "golemfrm.bin",
    "roc__frm.bin", "mage1frm.bin", "mage1frm.bin", "titanfrm.bin", "tita2frm.bin", "skel_frm.bin",
    "zomb_frm.bin", "zomb_frm.bin", "mummyfrm.bin", "mummyfrm.bin", "vampifrm.bin", "vampifrm.bin",
    "lich_frm.bin", "lich_frm.bin", "drabnfrm.bin", "roguefrm.bin", "nomadfrm.bin", "ghostfrm.bin",
    "geniefrm.bin", "medusfrm.bin", "felemfrm.bin", "felemfrm.bin", "felemfrm.bin", "felemfrm.bin"
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
u8 giNumPowFrames[KB_SPELL_EFFECT_COUNT] = {10, 10, 10, 10, 10, 10, 10, 10,
                                                             10, 8,  8,  10, 10, 10, 10, 15,
                                                             10, 10, 10, 10, 10, 16, 16, 14,
                                                             19, 22, 10, 17, 10, 12, 11, 16,
                                                             7,  8,  8,  8,  8,  8,  8};
SpellEffectDisplayType
giSpellEffectShowType = SPELL_EFFECT_DISPLAY_EFFECT_STATUS;
i8 gcColorToPlayerPos[RADAR_OWNER_COLOR_COUNT] = {0, 1, 2, 3, 4, 5, 0, 0};
const char* cCombatBkgNames[KB_COMBAT_BACKGROUND_COUNT] = {
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
i32 gEstatesGoldLevel[H2EnumIndex(HERO_SKILL_LEVEL_COUNT)] = {0, 100, 250, 500};
float gfSSLogisticsMod[H2EnumIndex(HERO_SKILL_LEVEL_COUNT)] = {1.0f, 1.1f, 1.2f, 1.3f};
float gfSSNavigationMod[H2EnumIndex(HERO_SKILL_LEVEL_COUNT)] = {1.0f, 1.33f, 1.66f, 2.0f};
float gfSSArcheryMod[H2EnumIndex(HERO_SKILL_LEVEL_COUNT)] = {1.0f, 1.1f, 1.25f, 1.5f};
float gfSSAIArcheryMod[H2EnumIndex(HERO_SKILL_LEVEL_COUNT)] = {1.0f, 1.04f, 1.1f, 1.2f};
i8 giVisRange[H2EnumIndex(HERO_SKILL_LEVEL_COUNT)] = {4, 5, 6, 7};
u8 gStartingHeroStats[KB_FACTION_TABLE_CAPACITY][HERO_STARTING_STAT_COUNT] = {
    {2, 2, 1, 1, 1},
    {3, 1, 1, 1, 1},
    {0, 0, 2, 3, 1},
    {0, 0, 3, 2, 1},
    {0, 1, 2, 2, 1},
    {1, 0, 2, 2, 1},
    {},
    {},
    {},
    {},
    {},
    {},
    // Cyborg heroes start with the Necromancer statline upstream.
    {1, 0, 2, 2, 1}
};
i32 giTerrainCost[KB_TERRAIN_TYPE_COUNT][H2EnumIndex(HERO_SKILL_LEVEL_COUNT)][KB_TERRAIN_STEP_TYPE_COUNT] =
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
    0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0
};
const char* gTownPrefixNames[KB_FACTION_TABLE_CAPACITY] =
    {"twnk", "twnb", "twns", "twnw", "twnz", "twnn"};
const char* gTownObjNames[KB_TOWN_OBJECT_NAME_COUNT] = {
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
H2EnumStorage<CreatureType, i8>
gDwellingType[KB_FACTION_TABLE_CAPACITY][KB_DWELLING_TYPE_COUNT] = {
    {H2EnumIndex(CREATURE_PEASANT),
     H2EnumIndex(CREATURE_ARCHER),
     H2EnumIndex(CREATURE_PIKEMAN),
     H2EnumIndex(CREATURE_SWORDSMAN),
     H2EnumIndex(CREATURE_CAVALRY),
     H2EnumIndex(CREATURE_PALADIN),
     H2EnumIndex(CREATURE_RANGER),
     H2EnumIndex(CREATURE_VETERAN_PIKEMAN),
     H2EnumIndex(CREATURE_MASTER_SWORDSMAN),
     H2EnumIndex(CREATURE_CHAMPION),
     H2EnumIndex(CREATURE_CRUSADER),
     ARMY_GROUP_EMPTY_SLOT},
    {H2EnumIndex(CREATURE_GOBLIN),
     H2EnumIndex(CREATURE_ORC),
     H2EnumIndex(CREATURE_WOLF),
     H2EnumIndex(CREATURE_OGRE),
     H2EnumIndex(CREATURE_TROLL),
     H2EnumIndex(CREATURE_CYCLOPS),
     H2EnumIndex(CREATURE_ORC_CHIEF),
     ARMY_GROUP_EMPTY_SLOT,
     H2EnumIndex(CREATURE_OGRE_LORD),
     H2EnumIndex(CREATURE_WAR_TROLL),
     ARMY_GROUP_EMPTY_SLOT,
     ARMY_GROUP_EMPTY_SLOT},
    {H2EnumIndex(CREATURE_SPRITE),
     H2EnumIndex(CREATURE_DWARF),
     H2EnumIndex(CREATURE_ELF),
     H2EnumIndex(CREATURE_DRUID),
     H2EnumIndex(CREATURE_UNICORN),
     H2EnumIndex(CREATURE_PHOENIX),
     H2EnumIndex(CREATURE_BATTLE_DWARF),
     H2EnumIndex(CREATURE_GRAND_ELF),
     H2EnumIndex(CREATURE_GREATER_DRUID),
     ARMY_GROUP_EMPTY_SLOT,
     ARMY_GROUP_EMPTY_SLOT,
     ARMY_GROUP_EMPTY_SLOT},
    {H2EnumIndex(CREATURE_CENTAUR),
     H2EnumIndex(CREATURE_GARGOYLE),
     H2EnumIndex(CREATURE_GRIFFIN),
     H2EnumIndex(CREATURE_MINOTAUR),
     H2EnumIndex(CREATURE_HYDRA),
     H2EnumIndex(CREATURE_GREEN_DRAGON),
     ARMY_GROUP_EMPTY_SLOT,
     ARMY_GROUP_EMPTY_SLOT,
     H2EnumIndex(CREATURE_MINOTAUR_KING),
     ARMY_GROUP_EMPTY_SLOT,
     H2EnumIndex(CREATURE_RED_DRAGON),
     H2EnumIndex(CREATURE_BLACK_DRAGON)},
    {H2EnumIndex(CREATURE_HALFLING),
     H2EnumIndex(CREATURE_BOAR),
     H2EnumIndex(CREATURE_IRON_GOLEM),
     H2EnumIndex(CREATURE_ROC),
     H2EnumIndex(CREATURE_MAGE),
     H2EnumIndex(CREATURE_GIANT),
     ARMY_GROUP_EMPTY_SLOT,
     H2EnumIndex(CREATURE_STEEL_GOLEM),
     ARMY_GROUP_EMPTY_SLOT,
     H2EnumIndex(CREATURE_ARCHMAGE),
     H2EnumIndex(CREATURE_TITAN),
     ARMY_GROUP_EMPTY_SLOT},
    {H2EnumIndex(CREATURE_SKELETON),
     H2EnumIndex(CREATURE_ZOMBIE),
     H2EnumIndex(CREATURE_MUMMY),
     H2EnumIndex(CREATURE_VAMPIRE),
     H2EnumIndex(CREATURE_LICH),
     H2EnumIndex(CREATURE_BONE_DRAGON),
     H2EnumIndex(CREATURE_MUTANT_ZOMBIE),
     H2EnumIndex(CREATURE_ROYAL_MUMMY),
     H2EnumIndex(CREATURE_VAMPIRE_LORD),
     H2EnumIndex(CREATURE_POWER_LICH),
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
i32 gSpecialBuildingCosts[KB_FACTION_TABLE_CAPACITY][KB_BUILDING_RESOURCE_COUNT] = {
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
i32 gNeutralBaseResourceValues[H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)] = {
    5000, 300, 350, 2000, 3000, 0, 12000, 2500, 1500, 1500, 200, 1000, 500, 0, 0, 1100, 0, 0, 0
};
i32 gSpecialBuildingBaseResourceValues[KB_FACTION_TABLE_CAPACITY] = {
    1500, 1000, 1000, 4500, 3500, 1000, 0, 0, 0, 0, 0, 0, 1500
};
i32 gDwellingBaseResourceValues[KB_FACTION_TABLE_CAPACITY][KB_DWELLING_TYPE_COUNT] = {
    {858, 2225, 2816, 7385, 13754, 29785, 4000, 3200, 8000, 16000, 40000, 0},
    {1802, 2615, 3414, 6967, 13212, 38141, 3500, 0, 8000, 16000, 0, 0},
    {1684, 3000, 3500, 7213, 15181, 27684, 4000, 4000, 12000, 0, 0, 0},
    {1956, 2607, 3869, 7510, 16002, 90000, 0, 0, 8500, 0, 120000, 180000},
    {1700, 3500, 2800, 9000, 11500, 85000, 0, 3500, 0, 15000, 155000, 0},
    {2200, 2100, 3800, 6000, 9500, 90000, 3000, 4900, 15000, 12000, 0, 0},
    {},
    {},
    {},
    {},
    {},
    {},
    // The Cyborg building costs currently mirror Knight costs, so their AI
    // resource values use the same valuation model as those costs.
    {858, 2225, 2816, 7385, 13754, 29785, 4000, 3200, 8000, 16000, 40000, 0}
};
i32
    gDwellingCosts[KB_FACTION_TABLE_CAPACITY][KB_DWELLING_TYPE_COUNT][KB_BUILDING_RESOURCE_COUNT] = {
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
u32l gHierarchyMask[KB_FACTION_TABLE_CAPACITY][KB_DWELLING_TYPE_COUNT] = {
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
     0xffffffffUL},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    // Cyborg dwelling prerequisites, following the Knight chain upstream.
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
     0xffffffffUL}
};
i32 giDebugBuildingToBuild = -1;
u8
    giTerrainToMusicTrack[H2EnumIndex(TERRAIN_COUNT)] = {16, 18, 14, 15, 11, 13, 17, 12, 16};
const char* cHeroTypeShortName[KB_FACTION_TABLE_CAPACITY] =
    {"kngt", "barb", "sorc", "wrlk", "wzrd", "necr", "", "", "", "", "", "", "cbrg"};
char cHeroTypeInitial[HERO_TYPE_INITIAL_COUNT] =
    {'k', 'b', 's', 'w', 'z', 'n', 0, 0, 0, 0, 0, 0, 'c'};
i32 giDeferObjDrawX = -1;
i32 giDeferObjDrawY = -1;
class heroWindow* gpInitWin = NULL;
// The Cyborg column mirrors the Knight skill weights upstream.
u8 iGetSSByAlignment[H2EnumIndex(HERO_SKILL_COUNT)][KB_FACTION_TABLE_CAPACITY] = {
    {3, 4, 2, 2, 2, 3, 0, 0, 0, 0, 0, 0, 3},
    {2, 3, 3, 1, 1, 1, 0, 0, 0, 0, 0, 0, 2},
    {3, 3, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 3},
    {2, 4, 1, 4, 2, 1, 0, 0, 0, 0, 0, 0, 2},
    {3, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 3},
    {2, 3, 4, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2},
    {5, 3, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 5},
    {2, 1, 4, 5, 5, 4, 0, 0, 0, 0, 0, 0, 2},
    {1, 1, 3, 3, 4, 3, 0, 0, 0, 0, 0, 0, 1},
    {1, 2, 3, 1, 2, 1, 0, 0, 0, 0, 0, 0, 1},
    {4, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 4},
    {1, 1, 2, 3, 3, 3, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 1, 0, 7, 0, 0, 0, 0, 0, 0, 0},
    {3, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 3}
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
u16 wallPos[KB_CASTLE_WALL_SEGMENT_COUNT][H2EnumIndex(COORDINATE_AXIS_COUNT)] =
    {{468, 58}, {421, 128}, {417, 291}, {498, 402}};
u16 towerPos[KB_CASTLE_TOWER_COUNT][H2EnumIndex(COORDINATE_AXIS_COUNT)] =
    {{441, 61}, {398, 147}, {398, 233}, {441, 316}};
u16 doorPos[KB_CASTLE_DOOR_POSITION_COUNT][H2EnumIndex(COORDINATE_AXIS_COUNT)] = {{393, 192}, {348, 262}};
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
i8 captainStats[KB_FACTION_TABLE_CAPACITY][HERO_PRIMARY_STAT_COUNT] =
    {{1, 1, 1, 1}, {1, 1, 1, 1}, {0, 0, 2, 2}, {0, 0, 2, 2}, {0, 0, 2, 2}, {0, 0, 2, 2},
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
     {0, 0, 2, 2}};
b32 gbDrawingPuzzle = false;
i32 giWalkingFrom = -1;
i32 giWalkingFrom2 = -1;
i32 giWalkingTo = -1;
i32 giWalkingTo2 = -1;
i32 giWalkingYMod = 0;
u8 moatCell[KB_MOAT_CELL_COUNT] = {8, 21, 33, 46, 58, 72, 85, 99, 112};
SCampaignChoice
    campaignChoices[H2EnumIndex(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT][CAMPAIGN_BONUS_CHOICE_COUNT] = {
        {{{CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_GOLD), CHOICE_GOLD_BONUS},
          {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_THUNDER_MACE), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_ARMORED_GAUNTLETS), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_WIZARD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_SORCERESS), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_KNIGHT), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_WIZARD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_SORCERESS), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_KNIGHT), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_WIZARD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_SORCERESS), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_KNIGHT), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_WIZARD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_SORCERESS), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_KNIGHT), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_SPELL, H2EnumIndex(SPELL_MIRROR_IMAGE), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_SPELL, H2EnumIndex(SPELL_SUMMON_EARTH_ELEMENTAL), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_SPELL, H2EnumIndex(SPELL_RESURRECT), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_BLACK_PEARL), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_DRAGON_SWORD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_DIVINE_BREASTPLATE), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_WIZARD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_SORCERESS), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_KNIGHT), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_CRYSTAL), CHOICE_RESOURCE_BONUS},
          {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_GEMS), CHOICE_RESOURCE_BONUS},
          {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_MERCURY), CHOICE_RESOURCE_BONUS}},
         {{CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_TAX_LIEN), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_HIDEOUS_MASK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_FIZBIN_OF_MISFORTUNE), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_NONE, CHOICE_VALUE_NONE, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_NONE, CHOICE_VALUE_NONE, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_NONE, CHOICE_VALUE_NONE, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_WIZARD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_SORCERESS), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_KNIGHT), CHOICE_NO_AMOUNT}}},
        {{{CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_GOLD), CHOICE_GOLD_BONUS},
          {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_MAGE_RING), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_MINOR_SCROLL), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_BARBARIAN), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_GOLD), CHOICE_GOLD_BONUS},
          {CAMPAIGN_CHOICE_SPELL, H2EnumIndex(SPELL_MASS_CURSE), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_DEFENDER_HELM), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_BARBARIAN), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_BARBARIAN), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_BARBARIAN), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_BARBARIAN), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_SECONDARY_SKILL, H2EnumIndex(HERO_SKILL_LOGISTICS), CHOICE_BASIC_SKILL},
          {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_POWER_AXE), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_WHITE_PEARL), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_BARBARIAN), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_BLACK_PEARL), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_DRAGON_SWORD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_DIVINE_BREASTPLATE), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_TAX_LIEN), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_HIDEOUS_MASK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_FIZBIN_OF_MISFORTUNE), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, H2EnumIndex(FACTION_BARBARIAN), CHOICE_NO_AMOUNT}}}
};
char* congratsText = NULL;
const char* gArtifactNames[KB_ARTIFACT_TABLE_CAPACITY] = {
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
    "Spade of Necromancy"
};
const char* gArtifactDesc[KB_ARTIFACT_TABLE_CAPACITY] = {
    "{Ultimate Book\n(+12 Knowledge)}\n\nThe Ultimate Book of Knowledge increases your knowledge "
    "by 12.",
    "{Ultimate Sword\n(+12 Attack)}\n\nThe Ultimate Sword of Dominion increases your attack skill "
    "by 12.",
    "{Ultimate Cloak\n(+12 Defense)}\n\nThe Ultimate Cloak of Protection increases your defense "
    "skill by 12.",
    "{Ultimate Wand\n(+12 Spell Power)}\n\nThe Ultimate Wand of Magic increases your spell power "
    "by 12.",
    "{Ultimate Shield}\n\nThe Ultimate Shield increases your attack and defense skills by 6 each.",
    "{Ultimate Staff}\n\nThe Ultimate Staff increases your spell power and knowledge by 6 each.",
    "{Ultimate Crown}\n\nThe Ultimate Crown increases each of your basic skills by 4 points.",
    "{Golden Goose}\n\nThe Golden Goose brings in an income of 10,000 gold per turn.",
    "{Arcane Necklace\n(+4 Spell Power)}\n\nThe Arcane Necklace of Magic increases your spell "
    "power by 4.",
    "{Caster's Bracelet\n(+2 Spell Power)}\n\nThe Caster's Bracelet of Magic increases your spell "
    "power by 2.",
    "{Mage's Ring\n(+2 Spell Power)}\n\nThe Mage's Ring of Power increases your spell power by 2.",
    "{Witches Broach\n(+3 Spell Power)}\n\nThe Witch's Broach of Magic increases your spell power "
    "by 3.",
    "{Medal}\n\nThe Medal of Valor increases your morale.",
    "{Medal}\n\nThe Medal of Courage increases your morale.",
    "{Medal}\n\nThe Medal of Honor increases your morale.",
    "{Medal}\n\nThe Medal of Distinction increases your morale.",
    "{Fizbin}\n\nThe Fizbin of Misfortune greatly decreases your morale.",
    "{Thunder Mace\n(+1 Attack)}\n\nThe Thunder Mace of Dominion increases your attack skill by 1.",
    "{Armored Gauntlets\n(+1 Defense)}\n\nThe Armored Gauntlets of Protection increase your "
    "defense skill by 1.",
    "{Defender Helm\n(+1 Defense)}\n\nThe Defender Helm of Protection increases your defense skill "
    "by 1.",
    "{Giant Flail\n(+1 Attack)}\n\nThe Giant Flail of Dominion increases your attack skill by 1.",
    "{Ballista}\n\nThe Ballista of Quickness lets your catapult fire twice per combat round.",
    "{Stealth Shield\n(+2 Defense)}\n\nThe Stealth Shield of Protection increases your defense "
    "skill by 2.",
    "{Dragon Sword\n(+3 Attack)}\n\nThe Dragon Sword of Dominion increases your attack skill by 3.",
    "{Power Axe\n(+2 Attack)}\n\nThe Power Axe of Dominion increases your attack skill by 2.",
    "{Divine Breastplate\n(+3 Defense)}\n\nThe Divine Breastplate of Protection increases your "
    "defense skill by 3.",
    "{Minor Scroll\n(+2 Knowledge)}\n\nThe Minor Scroll of Knowledge increases your knowledge by "
    "2.",
    "{Major Scroll\n(+3 Knowledge)}\n\nThe Major Scroll of Knowledge increases your knowledge by "
    "3.",
    "{Superior Scroll\n(+4 Knowledge)}\n\nThe Superior Scroll of Knowledge increases your "
    "knowledge by 4.",
    "{Foremost Scroll\n(+5 Knowledge)}\n\nThe Foremost Scroll of Knowledge increases your "
    "knowledge by 5.",
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
    "{Enchanted hourglass}\n\nThe Enchanted Hourglass extends the duration of all your spells by 2 "
    "turns.",
    "{Gold watch}\n\nThe Gold Watch doubles the effectiveness of your hypnotize spells.",
    "{Skullcap}\n\nThe Skullcap halves the casting cost of all mind influencing spells.",
    "{Ice cloak}\n\nThe Ice Cloak halves all damage your troops take from cold spells.",
    "{Fire cloak}\n\nThe Fire Cloak halves all damage your troops take from fire spells.",
    "{Lightning helm}\n\nThe Lightning Helm halves all damage your troops take from lightning "
    "spells.",
    "{Evercold icicle}\n\nThe Evercold Icicle causes your cold spells to do 50% more damage to "
    "enemy troops.",
    "{Everhot lava rock}\n\nThe Everhot Lava Rock causes your fire spells to do 50% more damage to "
    "enemy troops.",
    "{Lightning rod }\n\nThe Lightning Rod causes your lightning spells to do 50% more damage to "
    "enemy troops.",
    "{Snake-ring}\n\nThe Snake Ring halves the casting cost of all your bless spells.",
    "{Ankh}\n\nThe Ankh doubles the effectiveness of all your resurrect and animate spells.",
    "{Book of Elements}\n\nThe Book of Elements doubles the effectiveness of all your summoning "
    "spells.",
    "{Elemental ring}\n\nThe Elemental Ring halves the casting cost of all summoning spells.",
    "{Holy Pendant}\n\nThe Holy Pendant makes all your troops immune to curse spells.",
    "{Pendant of free will}\n\nThe Pendant of Free Will makes all your troops immune to hypnotize "
    "spells.",
    "{Pendant of life}\n\nThe Pendant of Life makes all your troops immune to death spells.",
    "{Serenity pendant}\n\nThe Serenity Pendant makes all your troops immune to berserk spells.",
    "{Seeing-eye pendant}\n\nThe Seeing-eye Pendant makes all your troops immune to blindness "
    "spells.",
    "{Kinetic pendant}\n\nThe Kinetic Pendant makes all your troops immune to paralyze spells.",
    "{Pendant of death}\n\nThe Pendant of Death makes all your troops immune to holy spells.",
    "{Wand of negation}\n\nThe Wand of Negation protects your troops from the Dispel Magic spell.",
    "{Golden bow}\n\nThe Golden Bow eliminates the 50% penalty for your troops shooting past "
    "obstacles. (e.g. castle walls)",
    "{Telescope}\n\nThe Telescope increases the amount of terrain your hero reveals when "
    "adventuring by 1 extra square.",
    "{Statesman's quill}\n\nThe Statesman's Quill reduces the cost of surrender to 10% of the "
    "total cost of troops you have in your army.",
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
    "{Arm of the Martyr}\n\nThe Arm of the Martyr increases your spell power by 3 but adds the "
    "undead morale penalty.",
    "{Breastplate of Anduran}\n\nThe Breastplate increases your defense by 5.",
    "{Broach of Shielding}\n\nThe Broach of Shielding provides 50% protection from Armageddon and "
    "Elemental Storm, but decreases spell power by 2.",
    "{Battle Garb}\n\nThe Battle Garb of Anduran combines the powers of the three Anduran "
    "artifacts.  It provides maximum luck and morale for your troops and gives you the Town Portal "
    "spell.",
    "{Crystal Ball}\n\nThe Crystal Ball lets you get more specific information about monsters, "
    "enemy heroes, and castles nearby the hero who holds it.",
    "{Heart of Fire}\n\nThe Heart of Fire provides 50% protection from fire, but doubles the "
    "damage taken from cold.",
    "{Heart of Ice}\n\nThe Heart of Ice provides 50% protection from cold, but doubles the damage "
    "taken from fire.",
    "{Helmet of Anduran}\n\nThe Helmet increases your spell power by 5.",
    "{Holy Hammer}\n\nThe Holy Hammer increases your attack skill by 5.",
    "{Legendary Scepter}\n\nThe Legendary Scepter adds 2 points to all attributes.",
    "{Masthead}\n\nThe Masthead boosts your luck and morale by 1 each in sea combat.",
    "{Sphere of Negation}\n\nThe Sphere of Negation disables all spell casting, for both sides, in "
    "combat.",
    "{Staff of Wizardry}\n\nThe Staff of Wizardry boosts your spell power by 5.",
    "{Sword Breaker}\n\nThe Sword Breaker increases your defense by 4 and attack by 1.",
    "{Sword of Anduran}\n\nThe Sword increases your attack skill by 5.",
    "{Spade of Necromancy}\n\nThe Spade gives you increased necromancy skill."
};
const char* gArtifactEvent[KB_ARTIFACT_LEVEL_COUNT] = {
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "After rescuing a Sorceress from a cursed tomb, she rewards your heroism with an exquisite "
    "jeweled necklace.",
    "While searching through the rubble of a caved-in mine, you free a group of trapped Dwarves.  "
    "Grateful, the leader gives you a golden bracelet.",
    "A cry of pain leads you to a Centaur, caught in a trap.  Upon setting the creature free, he "
    "hands you a small pouch.  Emptying the contents, you find a dazzling jeweled ring.",
    "Alongside the remains of a burnt witch lies a beautiful broach, intricately designed.  "
    "Approaching the corpse with caution, you add the broach to your inventory.",
    "Freeing a virtuous maiden from the clutches of an evil overlord, you are granted a Medal of "
    "Valor by the King's herald.",
    "After saving a young boy from a vicious pack of Wolves, you return him to his father's manor. "
    " The grateful nobleman awards you with a Medal of Courage.",
    "After freeing a princess of a neighboring kingdom from the evil clutches of despicable "
    "slavers, she awards you with a Medal of Honor.",
    "Ridding the countryside of the hideous Minotaur who made a sport of eating noblemen's "
    "Knights, you are honored with the Medal of Distinction.",
    "You stumble upon a medal lying alongside the empty road.  Adding the medal to your inventory, "
    "you become aware that you have acquired the undesirable Fizbin of Misfortune, greatly "
    "decreasing your army's morale.",
    "During a sudden storm, a bolt of lightning strikes a tree, splitting it.  Inside the tree you "
    "find a mysterious mace.",
    "You encounter the infamous Black Knight!  After a grueling duel ending in a draw, the Knight, "
    "out of respect, offers you a pair of armored gauntlets.",
    "A glint of golden light catches your eye.  Upon further investigation, you find a golden helm "
    "hidden under a bush.",
    "A clumsy Giant has killed himself with his own flail.  Knowing your superior skill with this "
    "weapon, you confidently remove the spectacular flail from the fallen Giant.",
    "Walking through the ruins of an ancient walled city, you find the instrument of the city's "
    "destruction, an elaborately crafted ballista.",
    "A stone statue of a warrior holds a silver shield.  As you remove the shield, the statue "
    "crumbles into dust.",
    "As you are walking along a narrow path, a nearby bush suddenly bursts into flames.  Before "
    "your eyes the flames become the image of a beautiful woman.  She holds out a magnificent "
    "sword to you.",
    "You see a silver axe embedded deeply in the ground.  After several unsuccessful attempts by "
    "your army to remove the axe, you tightly grip the handle of the axe and effortlessly pull it "
    "free.",
    "A gang of Rogues is sifting through the possessions of dead warriors.  Scaring off the "
    "scavengers, you note the Rogues had overlooked a beautiful breastplate.",
    "Before you appears a levitating glass case with a scroll, perched upon a bed of crimson "
    "velvet.  At your touch, the lid opens and the scroll floats into your awaiting hands.",
    "Visiting a local wiseman, you explain the intent of your journey.  He reaches into a sack and "
    "withdraws a yellowed scroll and hands it to you.",
    "You come across the remains of an ancient Druid.  Bones, yellowed with age, peer from the "
    "ragged folds of her robe.  Searching the robe, you discover a scroll hidden in the folds.",
    "Mangled bones, yellowed with age, peer from the ragged folds of a dead Druid's robe.  "
    "Searching the robe, you discover a scroll hidden within.",
    "A little leprechaun dances gleefully around a magic sack.  Seeing you approach, he stops in "
    "mid-stride.  The little man screams and stamps his foot ferociously, vanishing into thin air. "
    " Remembering the old leprechaun saying 'Finders Keepers', you grab the sack and leave.",
    "A noblewoman, separated from her traveling companions, asks for your help.  After escorting "
    "her home, she rewards you with a bag filled with gold.",
    "In your travels, you find a leather purse filled with gold that once belonged to a great "
    "warrior king who had the ability to transform any inanimate object into gold.",
    "A Nomad trader seeks protection from a tribe of Goblins.  For your assistance, he gives you a "
    "finely crafted pair of boots made from the softest leather.  Looking closely, you see "
    "fascinating ancient carvings engraved on the leather.",
    "Discovering a pair of beautifully beaded boots made from the finest and softest leather, you "
    "thank the anonymous donor and add the boots to your inventory.",
    "A traveling merchant offers you a rabbit's foot, made of gleaming silver fur, for safe "
    "passage.  The merchant explains the charm will increase your luck in combat.",
    "An ensnared Unicorn whinnies in fright.  Murmuring soothing words, you set her free.  "
    "Snorting and stamping her front hoof once, she gallops off.  Looking down you see a golden "
    "horseshoe.",
    "You have captured a mischievous imp who has been terrorizing the region.  In exchange for his "
    "release, he rewards you with a magical coin.",
    "In the middle of a patch of dead and dry vegetation, to your surprise you find a healthy "
    "green four-leaf clover.",
    "An old man claiming to be an inventor asks you to try his latest invention.  He then hands "
    "you a compass.",
    "An old sea captain is being tortured by Ogres.  You save him, and in return he rewards you "
    "with a wondrous instrument to measure the distance of a star.",
    "While venturing into a decrepit hut you find the Skeleton of a long dead witch.  "
    "Investigation of the remains reveals a glass eye rolling around inside an empty skull.",
    "A surprise turn in the landscape finds you in the midst of a grisly scene:  Vultures picking "
    "at the aftermath of a terrible battle.  Your cursory search of the remains turns up an "
    "enchanted hourglass.",
    "In reward for helping his cart out of a ditch, a traveling potion salesman gives you a "
    "\"magic\" gold watch.  Unbeknownst to him, the watch really is magical.",
    "A brief stop at an improbable rural inn yields an exchange of money, tales, and accidentally, "
    "luggage.  You find a magical skullcap in your new backpack.",
    "Responding to the panicked cries of a damsel in distress, you discover a young woman fleeing "
    "from a hungry bear.  You slay the beast in the nick of time, and the grateful Sorceress "
    "weaves a magic cloak from the bear's hide.",
    "You've come upon a fight between a Necromancer and a Paladin.  The Necromancer blasts the "
    "Paladin with a fire bolt, bringing him to his knees.  Acting quickly, you slay the evil one "
    "before the final blow.  The grateful Paladin gives you the fire cloak that saved him.",
    "A traveling tinker in need of supplies offers you a helm with a thunderbolt design on its top "
    "in exchange for food and water.  Curious, you accept, and later find out that the helm is "
    "magical.",
    "An icicle withstanding the full heat of the noonday sun attracts your attention.  Intrigued, "
    "you break it off, and find that it does not melt in your hand.",
    "Your wanderings bring you into contact with a tribe of ape-like beings using a magical lava "
    "rock that never cools to light their fires.  You take pity on them and teach them to make "
    "fire with sticks.  Believing you to be a god, the apes give you their rock.",
    "While waiting out a storm, a lighting bolt strikes a nearby cottage's lightning rod, which "
    "melts and falls to the ground.  The tip of the rod, however, survives intact and makes your "
    "hair stand on end when you touch it.  Hmm...",
    "You've found an oddly shaped ring on the finger of a long dead traveler.  The ring looks like "
    "a snake biting its own tail.",
    "A fierce windstorm reveals the entrance to a buried tomb.  Your investigation reveals that "
    "the tomb has already been looted, but the thieves overlooked an ankh on a silver chain in the "
    "dark.",
    "You come across a conjurer who begs to accompany you and your army awhile for safety.  You "
    "agree, and he offers as payment a copy of the book of the elements.",
    "While pausing to rest, you notice a bobcat climbing a short tree to get at a crow's nest.  On "
    "impulse, you climb the tree yourself and scare off the cat.  When you look in the nest, you "
    "find a collection of shiny stones and a ring.",
    "In your wanderings you come across a hermit living in a small, tidy hut.  Impressed with your "
    "mission, he takes time out from his meditations to bless and give you a charm against curses.",
    "Responding to cries for help, you find river Sprites making a sport of dunking an old man.  "
    "Feeling vengeful, you rescue the man and drag a Sprite onto dry land for awhile.  The Sprite, "
    "uncomfortable in the air, gives you a magic pendant to let him go.",
    "A brief roadside encounter with a small caravan and a game of knucklebones wins a magic "
    "pendant.  Its former owner says that it protects from Necromancers' death spells.",
    "The sounds of combat draw you to the scene of a fight between an old Barbarian and an "
    "eight-headed Hydra.  Your timely intervention swings the battle in favor of the man, and he "
    "rewards you with a pendant he used to use to calm his mind for battle.",
    "You come upon a very old woman, long blind from cataracts and dying alone.  You tend to her "
    "final needs and promise a proper burial.  Grateful, she gives you a magic pendant emblazoned "
    "with a stylized eye.  It lets you see with your eyes closed.",
    "You come across a golem wearing a glowing pendant and blocking your way.   Acting on a hunch, "
    "you cut the pendant from its neck.  Deprived of its power source, the golem breaks down, "
    "leaving you with the magical pendant.",
    "A quick and deadly battle with a Necromancer wins you his magical pendant.  Later, a Wizard "
    "tells you that the pendant protects undead under your control from holy word spells.",
    "You meet an old Wizard friend of yours traveling in the opposite direction.  He presents  you "
    "with a gift:  A wand that prevents the use of the dispel magic spell on your allies.",
    "A chance meeting with a famous Archer finds you in a game of knucklebones pitting his bow "
    "against your horse.  You win.",
    "A merchant from far away lands trades you a new invention of his people for traveling "
    "supplies.  It makes distant objects appear closer, and he calls it...\n\na telescope.",
    "You pause to help a diplomat with a broken axle fix his problem.  In gratitude, he gives you "
    "a writing quill with magical properties which he says will \"help people see things your "
    "way\".",
    "You see a Wizard fleeing from a Griffin and riding like the wind.  The Wizard opens a portal "
    "and rides through, getting his hat knocked off by the edge of the gate.  The Griffin follows; "
    "the gate closes.  You pick the hat up, dust it off, and put it on.",
    "You find a small tree that closely resembles the great Warlock Carnauth with a ring around "
    "one of its twigs.  Scraps of clothing and rotting leather lead you to suspect that it IS "
    "Carnauth, transformed.  Since you can't help him, you take the magic ring.",
    "An ammunition cart in the middle of an old battlefield catches your eye.  Inspection shows it "
    "to be in good working order, so  you take it along.",
    "Your big spending habits have earned you a massive tax bill that you can't hope to pay.  The "
    "tax man takes pity and agrees to only take 250 gold a day from your account for life.  Check "
    "here if you want one dollar to go to the presidential campaign election fund.",
    "Your looting of the grave of Sinfilas Gardolad, the famous shapeshifting Warlock, unearths "
    "his fabled mask.  Trembling, you put it on and it twists your visage into an awful grimace!  "
    "Oh no!  It's actually the hideous mask of Gromluck Greene, and you are stuck with it.",
    "You visit an alchemist who, upon seeing your army, is swayed by the righteousness of your "
    "cause.  The newly loyal subject gives you his endless pouch of sulfur to help with the war "
    "effort.",
    "A brief stop at a hastily abandoned Wizard's tower turns up a magical vial of mercury that "
    "always has a little left on the bottom.  Recognizing a treasure when you see one, you cap it "
    "and slip it in your pocket.",
    "A short rainstorm brings forth a rainbow...and you can see the end of it.  Riding quickly, "
    "you seize the pot of gold you find there.  The leprechaun who owns it, unable to stop you "
    "from taking it, offers an endless pouch of gems for the return of his gold.  You accept.",
    "Pausing to rest and light a cook fire, you pull wood out of a nearby pile of dead wood.  As "
    "you keep pulling wood from the pile, you notice that it doesn't shrink.  You realize to your "
    "delight that the wood is enchanted, so you take it along.",
    "You've found a Goblin weapon smithy making weapons for use against humans.  With a tremendous "
    "yell you and your army descend upon their camp and drive them away.  A search finds a magic "
    "ore cart that never runs out of iron.",
    "Taking shelter from a storm in a small cave,  you notice a small patch of crystal in one "
    "corner.  Curious, you break a piece off and notice that the original crystal grows the lost "
    "piece back.  You decide to stuff the entire patch into a pouch and take it with you.",
    "Your army is ambushed by a small tribe of wild (and none too bright) Orcs.  You fend them off "
    "easily and the survivors flee in all directions.  One of the Orcs was wearing a polished "
    "spiked helm.  Figuring it will make a good souvenir, you take it.",
    "You come upon a bridge spanning a dry gully.  Before you can cross, a Troll steps out from "
    "under the bridge and demands payment before it will permit you to pass.  You refuse, and the "
    "Troll charges, forcing you to slay it.  You take its spiked shield as a trophy.",
    "A walk across a dry saltwater lake bed yields an unlikely prize:  A white pearl amidst "
    "shattered shells and debris.",
    "Rumors of a Griffin of unusual size preying upon the countryside lead you to its cave lair.  "
    "A quick, brutal fight dispatches the beast, and a search of its foul nest turns up a huge "
    "black pearl.",
    "",
    "ERROR : Artifact event 82.",
    "ERROR : Artifact event 83.",
    "ERROR : Artifact event 84.",
    "ERROR : Artifact event 85.",
    "You find an elaborate container which houses an old vellum scroll. The runes on the container "
    "are very old, and the artistry with which it was put together is stunning. As you pull the "
    "scroll out, you feel imbued with magical power.",
    "One of the less intelligent members of your party picks up an arm off of the ground.  Despite "
    "its missing a body, it is still moving.  Your troops find the dismembered arm repulsive, but "
    "you cannot bring yourself to drop it: it seems to hold some sort of magical power that "
    "influences your decision making.",
    "You come upon a sign.  It reads: \"Here lies the body of Anduran.  Bow and swear fealty, and "
    "you shall be rewarded.\"  You decide to do as it says.  As you stand up, you feel a coldness "
    "against your skin.  Looking down, you find that you are suddenly wearing a gleaming, ornate "
    "breastplate.",
    "A kindly Sorceress thinks that your army's defenses could use a magical boost.  She offers to "
    "enchant the Broach that you wear on your cloak, and you accept.",
    "Out of pity for a poor peasant, you purchase a chest of old junk they are hawking for too "
    "much gold. Later, as you search through it, you find it contains the 3 pieces of the "
    "legendary battle garb of Anduran!",
    "You come upon a caravan of gypsies who are feasting and fortifying their bodies with mead.  "
    "They call you forward and say \"If you prove that you can dance the Rama-Buta, we will reward "
    "you.\"  You don't know it, but try anyway.  They laugh hysterically, but admire your bravery, "
    "giving you a Crystal Ball.",
    "You enter a recently burned glade and come upon a Fire Elemental sitting atop a rock.  It "
    "looks up, its flaming face contorted in a look of severe pain.  It then tosses a glowing "
    "object at you.  You put up your hands to block it, but it passes right through them and sears "
    "itself into your chest.",
    "Suddenly, a biting coldness engulfs your body.  You seize up, falling from your horse.  The "
    "pain subsides, but you still feel as if your chest is frozen.  As you pick yourself up off of "
    "the ground, you hear hearty laughter.  You turn around just in time to see a Frost Giant run "
    "off into the woods and disappear.",
    "You spy a gleaming object poking up out of the ground.   You send a member of your party over "
    "to investigate.  He comes back with a golden helmet in his hands.  You realize that it must "
    "be the helmet of the legendary Anduran, the only man who was known to wear solid gold armor.",
    "You come upon a battle where a Paladin has been mortally wounded by a group of Zombies.  He "
    "asks you to take his hammer and finish what he started.  As you pick it up, it begins to hum, "
    "and then everything becomes a blur.  The Zombies lie dead, the hammer dripping with blood.  "
    "You strap it to your belt.",
    "Upon cresting a small hill, you come upon a ridiculous looking sight.  A Sprite is attempting "
    "to carry a Scepter that is almost as big as it is.  Trying not to laugh, you ask, \"Need "
    "help?\"  The Sprite glares at you and answers: \"You think this is funny?  Fine.  You can "
    "carry it.  I much prefer flying anyway.\"",
    "An old seaman tells you a tale of an enchanted masthead that he used in his youth to rally "
    "his crew during times of trouble.  He then hands you a faded map that shows where he hid it.  "
    "After much exploring, you find it stashed underneath a nearby dock.",
    "You stop to help a Peasant catch a runaway mare.  To show his gratitude, he hands you a tiny "
    "sphere.  As soon as you grasp it, you feel the magical energy drain from your limbs...",
    "While out scaring up game, your troops find a mysterious staff levitating about three feet "
    "off of the ground.  They hand it to you, and you notice an inscription.  It reads: \"Brains "
    "best brawn and magic beats might.  Heed my words, and you'll win every fight.\"",
    "A former Captain of the Guard admires your quest and gives you the enchanted Sword Breaker "
    "that he relied on during his tour of duty.",
    "A Troll stops you and says: \"Pay me 5,000 gold, or the Sword of Anduran will slay you where "
    "you stand.\"  You refuse.  The troll grabs the sword hanging from its belt, screams in pain, "
    "and runs away.  Picking up the fabled sword, you give thanks that half-witted Trolls tend to "
    "grab the wrong end of sharp objects.",
    "A dirty shovel has been thrust into a dirt mound nearby. Upon investigation, you discover it "
    "to be the enchanted shovel of the Gravediggers, long thought lost by mortals."
};
const char* gStatNames[HERO_PRIMARY_STAT_COUNT] = {"Attack Skill", "Defense Skill", "Spell Power", "Knowledge"};
const char* gStatDesc[HERO_PRIMARY_STAT_COUNT] = {
    "{Attack Skill}\n\nYour attack skill is a bonus added to each creature's attack skill.",
    "{Defense Skill}\n\nYour defense skill is a bonus added to each creature's defense skill.",
    "{Spell Power}\n\nYour spell power determines the length or power of a spell.",
    "{Knowledge}\n\nYour knowledge determines how many spell points your hero may have.  Under "
    "normal cirumstances, a hero is limited to 10 spell points per level of knowledge."
};
const char* gAlignmentNames[KB_ALIGNMENT_NAME_COUNT] = {
    "Knight",
    "Barbarian",
    "Sorceress",
    "Warlock",
    "Wizard",
    "Necromancer",
    "Multiple",
    "Random",
    NULL,
    NULL,
    NULL,
    NULL,
    "Cyborg"
};
const char* gArmyShortNames[H2EnumIndex(CREATURE_COUNT)] = {
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
const char* gArmyNames[KB_CREATURE_TABLE_CAPACITY] = {
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
const char* gArmyNamesPlural[KB_CREATURE_TABLE_CAPACITY] = {
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
const char* gTerrainNames[H2EnumIndex(TERRAIN_COUNT)] = {"Ocean", "Grass", "Snow", "Swamp", "Lava", "Desert", "Dirt", "Wasteland", "Beach"
};
const char* gResourceNames[RESOURCE_VALUE_COUNT] = {"Wood", "Mercury", "Ore", "Sulfur", "Crystal", "Gems", "Gold"
};

const char* gMineNames[KB_MINE_NAME_COUNT] = {
    "Sawmill",
    "Alchemist Lab",
    "Ore Mine",
    "Sulfur Mine",
    "Crystal Mine",
    "Gem Mine",
    "Gold Mine"
};
const char* gQuickViewText[KB_QUICK_VIEW_TEXT_COUNT] = {
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
    "Jail",
    "Shipyard"
};
const char* gEventText[KB_EVENT_TEXT_TABLE_COUNT] = {
    "{Alchemist}\n\nYou have taken control of the local Alchemist shop. It will provide you with "
    "one unit of Mercury per day.",
    "{Signpost}\n\nA signpost reads:}\n\n%s is near.",
    "{Buoy}\n\nYour men spot a navigational buoy, confirming that you are on course.",
    "{Buoy}\n\nYour men spot a navigational buoy, confirming that you are on course and increasing "
    "their morale.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "{Faerie Ring}\n\nYou enter the faerie ring, but nothing happens.",
    "{Faerie Ring}\n\nUpon entering the mystical faerie ring, your army gains luck for its next "
    "battle.",
    "{Campfire}\n\nRansacking an enemy camp, you discover a hidden cache of treasures.",
    "{Fountain}\n\nYou drink from the enchanted fountain, but nothing happens.",
    "{Fountain}\n\nAs you drink the sweet water, you gain luck for your next battle.",
    "{Gazebo}\n\nAn old Knight appears on the steps of the gazebo. \"I am sorry, my liege, I have "
    "taught you all I can.\"",
    "{Gazebo}\n\nAn old Knight appears on the steps of the gazebo. \"My liege, I will teach you "
    "all that I know to aid you in your travels.\"",
    "{Genie Lamp}\n\nYou stumble upon a dented and tarnished lamp lodged deep in the earth. Do you "
    "wish to rub the lamp?",
    "{Graveyard}\n\nYou tentatively approach the burial ground of ancient warriors.  Do you want "
    "to search the graves?",
    "Upon defeating the Zombies you spend several hours searching the graves and find nothing.  "
    "Such a despicable act reduces your army's morale.",
    "Upon defeating the zomies you search the graves and find something!",
    "{Archer's House}\n\nA group of Archers with a desire for greater glory wish to join you. Do "
    "you accept? ",
    "You are unable to recruit at this time, your ranks are full.",
    "{Archer's House}\n\nAs you approach the dwelling, you notice that there is no one here.",
    "{Goblin Hut}\n\nA group of Goblins with a desire for greater glory wish to join you. Do you "
    "accept?",
    "You are unable to recruit at this time, your ranks are full.",
    "{Goblin Hut}\n\nAs you approach the dwelling, you notice that there is no one here.",
    "{Peasant Hut}\n\nA group of Peasants with a desire for greater glory wish to join you. Do you "
    "accept? ",
    "You are unable to recruit at this time, your ranks are full.",
    "{Peasant Hut}\n\nAs you approach the dwelling, you notice that there is no one here.",
    "{Dwarf Cottage}\n\nA group of Dwarves with a desire for greater glory wish to join you. Do "
    "you accept? ",
    "You are unable to recruit at this time, your ranks are full.",
    "{Dwarf Cottage}\n\nAs you approach the dwelling, you notice that there is no one here.",
    "{Thatched Hut}\n\nA group of Peasants with a desire for greater glory wish to join you. Do "
    "you accept? ",
    "You are unable to recruit at this time, your ranks are full.",
    "{Thatched Hut}\n\nAs you approach the Peasant dwelling you notice that there is no one here.",
    "{Tree House}\n\nA group of Sprites with a desire for greater glory wish to join you. Do you "
    "accept? ",
    "You are unable to recruit at this time, your ranks are full.",
    "{Tree House}\n\nAs you approach the Sprites' tree house you notice that there is no one here.",
    "{Halfling Hole}\n\nA group of Halflings with a desire for greater glory wish to join you. Do "
    "you accept? ",
    "You are unable to recruit at this time, your ranks are full.",
    "{Halfling Hole}\n\nAs you approach the habitat you notice that there is no one here.",
    "{Watch Tower}\n\nA group of Orcs with a desire for greater glory wish to join you. Do you "
    "accept? ",
    "You are unable to recruit at this time, your ranks are full.",
    "{Watch Tower}\n\nAs you approach the Orcs' watch tower you notice that there is no one here.",
    "{Snow Cave}\n\nA group of Centaurs with a desire for greater glory wish to join you. Do you "
    "accept? ",
    "You are unable to recruit at this time, your ranks are full.",
    "{Cave}\n\nAs you approach the Centaurs' cave you notice that there is no one here.",
    "{Excavation}\n\nA group of Skeletons with a desire for greater glory wish to join you. Do you "
    "accept? ",
    "You are unable to recruit at this time, your ranks are full.",
    "{Excavation}\n\nAs you approach the Skeletons' excavation you notice that there is no one "
    "here.",
    "",
    "",
    "",
    "",
    "",
    "{Lighthouse}\n\nThe lighthouse is now under your control, and all of your ships will now move "
    "further each turn.",
    "{Water Wheel}\n\nThe keeper of the mill announces: \"Milord, I am sorry, there is no gold "
    "currently available.  Please try again next week.\"",
    "{Water Wheel}\n\nThe keeper of the mill announces: \"Milord, I have been working very hard to "
    "provide you with this gold, come back next week for more.\"",
    "{Ore Mine}\n\nYou gain control of an ore mine. It will provide you with two units of ore per "
    "day.",
    "{Sulfur Mine}\n\nYou gain control of a sulfur mine. It will provide you with one unit of "
    "sulfur per day.",
    "{Crystal Mine}\n\nYou gain control of a crystal mine. It will provide you with one unit of "
    "crystal per day.",
    "{Gem Mine}\n\nYou gain control of a gem mine. It will provide you with one unit of gems per "
    "day.",
    "{Gold Mine}\n\nYou gain control of a gold mine. It will provide you with 1000 gold per day.",
    "{Followers}\n\nA group of %s with a desire for greater glory wish to join you. Do you "
    "accept? ",
    "Insulted by your refusal of their offer, the monsters attack!",
    "{Obelisk}\n\nYou come upon an obelisk made from a type of stone you have never seen before.  "
    "Staring at it intensely, the smooth surface suddenly changes to an inscription.  The "
    "inscription is a piece of a lost ancient map.  Quickly you copy down the piece and the "
    "inscription vanishes as abruptly as it had appeared.",
    "{Obelisk}\n\nYou have already been to this obelisk.",
    "",
    "",
    "You find a small quantity of %s.",
    "{Sawmill}\n\nYou gain control of a sawmill. It will provide you with two units of wood per "
    "day.",
    "{Oracle}\n\nNestled among the trees sits a blind seer. After explaining the intent of your "
    "journey, the seer activates his crystal ball, allowing you to see the strengths and "
    "weaknesses of your opponents.",
    "",
    "",
    "",
    "",
    "",
    "",
    "{Desert Tent}\n\nA group of tattered tents, billowing in the sandy wind, beckons you.  The "
    "tents are unoccupied.  Perhaps more Nomads will be here later.",
    "{Desert Tent}\n\nA group of tattered tents, billowing in the sandy wind, beckons you.  Do you "
    "wish to have any Nomads join you during your travels?",
    "{Wagon}\n\nA colorful Rogues' wagon stands empty here.  Perhaps more Rogues will be here "
    "later.",
    "{Wagon}\n\nDistant sounds of music and laughter draw you to a colorful wagon housing Rogues.  "
    "Do you wish to have any Rogues join your army?",
    "{Whirlpool}\n\nA whirlpool engulfs your ship.  Some of your army has fallen overboard.",
    "{Windmill}\n\nThe keeper of the mill announces: \"Milord, I am sorry, there are no resources "
    "currently available. Please try again next week.\"",
    "{Windmill}\n\nThe keeper of the mill announces: \"Milord, I have been working very hard to "
    "provide you with these resources, come back next week for more.\"",
    "",
    "",
    "",
    "",
    "",
    "{Skeleton}\n\nYou come upon the remains of an unfortunate adventurer.  Searching through the "
    "tattered clothing, you find nothing.",
    "{Skeleton}\n\nYou come upon the remains of an unfortunate adventurer.  Searching through the "
    "tattered clothing, you find "
};
const char* gCPanelHelp[KB_CONTROL_PANEL_HELP_COUNT] = {
    "Start a single or multi-player game.",
    "Load a previously saved game.",
    "Save the current game.",
    "Quit out of Heroes of Might and Magic II.",
    "Exit this menu without doing anything."
};
const char* gCSPanelHelp[KB_COMBAT_SPELL_PANEL_HELP_COUNT] = {
    "{OK}\n\nExit this menu.",
    "{Speed}\n\nSet the speed of combat actions and animations.",
    "{Monster Info}\n\nToggle the monster info window, which shows information on the active and "
    "targeted monsters.",
    "{Auto Combat\nSpell Casting}\n\nToggle whether or not the computer will cast spells for you "
    "when auto combat is on.  (Note: This does not affect spell casting for computer players in "
    "any way, nor does it affect quick combat.)",
    "{Grid}\n\nToggle the hex grid on or off.  The hex grid always underlies movement, even if "
    "turned off.  This switch only determines if the grid is visible.",
    "{Shadow Movement}\n\nToggle on or off a shadowing system showing where your creatures can "
    "move and attack.  When this is turned on, there is a slight performance cost, most noticeable "
    "on 486 computers.",
    "{Shadow Cursor}\n\nToggle on or off a shadowing system showing where the hex grid that the "
    "mouse cursor is currently over is shadowed.  When this is turned on, there is a slight "
    "performance cost, most noticeable on 486 computers."
};
const char* gAPanelHelp[KB_ADVENTURE_PANEL_HELP_COUNT] = {
    "View the entire world.",
    "View the obelisk puzzle.",
    "View information on the scenario you are currently playing.",
    "Dig for the Ultimate Artifact.",
    "Exit this menu without doing anything."
};
const char* gInitMenuHelp[KB_INIT_MENU_HELP_COUNT] = {
    "{New Game}\n\nStart a single or multi-player game.",
    "{Load Game}\n\nLoad a previously saved game.",
    "{High Scores}\n\nView the high score screen.",
    "{Credits}\n\nView the credits screen.",
    "{Quit}\n\nQuit Heroes of Might and Magic and return to the operating system."
};
const char* gAdvMenuHelp[KB_ADVENTURE_MENU_HELP_COUNT] = {
    "{Next Hero}\n\nSelect the next Hero.",
    "{Continue Movement}\n\nContinue the Hero's movement along the current path.",
    "{Kingdom Summary}\n\nView a summary of your kingdom.",
    "{End Turn}\n\nEnd your turn and let the computer take its turn.",
    "{Adventure Options}\n\nBring up the adventure options menu.",
    "{File Options}\n\nBring up the file options menu, allowing you to load, save etc.",
    "{System Options}\n\nBring up the system options menu, allowing you to customize your game.",
    "{Cast Spell}\n\nCast an adventure spell."
};
const char* gLuckText[KB_LUCK_TEXT_COUNT] = {"Cursed", "Awful", "Bad", "Normal", "Good", "Great", "Irish"
};
const char* gMoraleText[KB_MORALE_TEXT_COUNT] = {"Treason", "Awful", "Poor", "Normal", "Good", "Great", "Blood!"
};
const char* onOffText[KB_ON_OFF_TEXT_COUNT] = {
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
    "On\nVolume 1"
};
const char* walkSpeedText[KB_WALK_SPEED_TEXT_COUNT] = {"Walk", "Trot", "Canter", "Gallop", "Jump"
};
const char* gColors[H2EnumIndex(FACTION_COUNT)] = {"blue", "green", "red", "yellow", "orange", "purple"};
const char* gMonthNames[KB_MONTH_NAME_COUNT] = {
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
const char* gWeekNames[KB_WEEK_NAME_COUNT] = {
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
    "Condor"
};
const char* cHeroScreen[KB_HERO_SCREEN_TEXT_COUNT] = {
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
    "Set army combat formation to 'Grouped'"
};
const char* cCastleInfo[KB_CASTLE_INFO_TEXT_COUNT] = {
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
const char* cLuckInfo[KB_LUCK_INFO_TEXT_COUNT] = {
    "{Good Luck}\n\nGood luck sometimes lets your armies get lucky attacks (double strength) in "
    "combat.",
    "{Neutral Luck}\n\nNeutral luck means your armies will never get lucky or unlucky attacks on "
    "the enemy.",
    "{Bad Luck}\n\nBad luck sometimes falls on your armies in combat, causing their attacks to "
    "only do half damage.",
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
    "\nBattle Garb of Anduran gives you maximum luck."
};
const char* IQnames[KB_IQ_NAME_COUNT] = {"None", "Dumb", "Average", "Smart", "Genius"
};
const char* cSpellHelp[KB_SPELL_HELP_TEXT_COUNT] = {
    "View previous page",
    "View next page",
    "View adventure Spells",
    "View Combat Spells",
    "Close Spellbook",
    "View Spells",
    "Select Spell",
    "View Combat Spells",
    "Your hero has %d spell points remaining"
};
const char* speedText[KB_SPEED_TEXT_COUNT] = {
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
const char* cArmyDetail[KB_ARMY_DETAIL_TEXT_COUNT] = {
    "Attack Skill: ",
    "Defense Skill: ",
    "Shots left: ",
    "Damage: ",
    "Hit Points: ",
    "Speed: ",
    "Morale: ",
    "Luck: ",
    "Shots: "
};
const char* cWellDetail[KB_WELL_DETAIL_TEXT_COUNT] = {
    "Attack: ",
    "Defense: ",
    "Shots: ",
    "Damg: ",
    "HP: ",
    "Speed: ",
    "Available: ",
    "\n\nSpeed:\n%s",
    "\n\nGrowth\n + %d / week"
};
const char* cKingdomOverview[KB_KINGDOM_OVERVIEW_TEXT_COUNT] = {
    "Kingdom Overview     Month %d, Week %d, Day %d",
    "You own Dragon City.",
    "You own the Lighthouse."
};
const char* cNewTurn[KB_NEW_TURN_TEXT_COUNT] = {
    "%s, you only have %d days left to capture a town, or you will be banished from this land.",
    "%s, this is your last day to capture a town, or you will be banished from this land.",
    "Astrologers proclaim month of the %s.\n\nAll dwellings increase population.",
    "Astrologers proclaim month of the %s.\n\n%s population doubles!\n\nAll dwellings increase "
    "population.",
    "Astrologers proclaim month of the PLAGUE!\n\nAll populations are halved.",
    "Astrologers proclaim week of the %s.\n\nAll dwellings increase population.",
    "Astrologers proclaim week of the %s.\n\n%s growth +5.\n\nAll dwellings increase population."
};
const char* cViewGeneralLabels[KB_VIEW_GENERAL_LABEL_COUNT] = {
    "Attack: ",
    "Defense: ",
    "Spell Power: ",
    "Knowledge: ",
    "Morale: ",
    "Luck: ",
    "Spell Points: "
};
const char* cViewGeneralHelp[KB_VIEW_GENERAL_HELP_COUNT] = {
    "Stop Catapult",
    "Cast Spell",
    "Retreat",
    "Surrender",
    "Cancel",
    "Hero's Options",
    "Captain's Options"
};
const char* cViewGeneralLongHelp[KB_VIEW_GENERAL_LONG_HELP_COUNT] = {
    "{Cast Spell}\n\nCast a magical spell.  You may only cast one spell per combat round.  The "
    "round is reset when every creature has had a turn.",
    "{Retreat}\n\nRetreat your hero, abandoning your creatures.  Your hero will be available for "
    "you to recruit again, however, the hero will have only a novice hero's forces.",
    "{Surrender}\n\nSurrendering costs gold.  However, if you pay the ransom, the hero and all of "
    "his or her surviving creatures will be available to recruit again.",
    "{Cancel}\n\nReturn to the battle."
};
const char* cCombatMessage[KB_COMBAT_MESSAGE_COUNT] = {
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
const char* cHeroLevel[KB_HERO_LEVEL_TEXT_COUNT] =
    {"%s has gained", " a level.\n", " %d levels.\n"
};
const char* cCombatHelp[KB_COMBAT_HELP_COUNT] = {
    "Wait for other units to go.",
    "Skip this unit",
    "Auto combat",
    "Customize system options.",
    ""
};
const char* cLongCombatHelp[KB_LONG_COMBAT_HELP_COUNT] = {
    "{Wait}\n\nHave the current creature wait to take its turn until all the other creatures have "
    "gone.  The creature does not lose its turn.",
    "{Skip}\n\nSkips the current creature.  The current creature loses its turn and does not get "
    "to go again until the next round.",
    "{Auto Combat}\n\nAllows the computer to fight out the battle for you.",
    "{System Options}\n\nAllows you to customize the combat screen.",
    "{Message Bar}\n\nShows the results of individual monster's actions."
};
const char* cTownCommand[KB_TOWN_COMMAND_COUNT] = {
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
const char* gHeroDefaultNames[KB_HERO_DEFAULT_NAME_COUNT] = {
    "Lord Kilburn", "Sir Gallant", "Ector",    "Gwenneth", "Tyro",    "Ambrose",   "Ruby",
    "Maximus",      "Dimitri",     "Thundax",  "Fineous",  "Jojosh",  "Crag Hack", "Jezebel",
    "Jaclyn",       "Ergon",       "Tsabu",    "Atlas",    "Astra",   "Natasha",   "Troyan",
    "Vatawna",      "Rebecca",     "Gem",      "Ariel",    "Carlawn", "Luna",      "Arie",
    "Alamar",       "Vesper",      "Crodo",    "Barok",    "Kastore", "Agar",      "Falagar",
    "Wrathmont",    "Myra",        "Flint",    "Dawn",     "Halon",   "Myrini",    "Wilfrey",
    "Sarakin",      "Kalindra",    "Mandigal", "Zom",      "Darlana", "Zam",       "Ranloo",
    "Charity",      "Rialdo",      "Roxana",   "Sandro",   "Celia"
};
const char* gNewGameHelp[KB_NEW_GAME_HELP_COUNT] = {
    "{Game Difficulty}\n\nThis lets you change the starting difficulty at which you will play.  "
    "Higher difficulty levels start you off with fewer resources, and at the higher settings, give "
    "extra resources to the computer.",
    "{Handicap}\n\nThis lets you change the handicap of a particular player.  Only humans may be "
    "handicapped.  Handicapped players start with fewer resources and earn 15 or 30% fewer "
    "resources per turn for mild and severe handicaps, respectively.",
    "{Opponents}\n\nThis lets you change player starting positions and colors.  A particular color "
    "will always start in a particular location.  Some positions may only be played by a computer "
    "player or only by a human player.",
    "{Class}\n\nThis lets you change the class of a player.  Classes are not always changeable.  "
    "Depending on the scenario, a player may receive additional towns and/or heroes not of their "
    "primary alignment.",
    "{Scenario}\n\nClick here to select which scenario to play.",
    "{Difficulty Rating}\n\nThe difficulty rating reflects a combination of various settings for "
    "your game.  This number will be applied to your final score.",
    "{OK}\n\nClick to accept these settings and start a new game.",
    "{Cancel}\n\nClick to return to the main menu."
};
const char* gSetupBaudHelp[KB_SETUP_BAUD_HELP_COUNT] = {
    "{2400 Baud}\n\nUse a 2400 baud connection speed. \n\nNote: For a 14400 baud modem, use the "
    "19200 baud speed.  For a 28800 baud modem, use the 38400 baud speed.",
    "{9600 Baud}\n\nUse a 9600 baud connection speed. \n\nNote: For a 14400 baud modem, use the "
    "19200 baud speed.  For a 28800 baud modem, use the 38400 baud speed.",
    "{19200 Baud}\n\nUse a 19200 baud connection speed.\n\nNote: For a 14400 baud modem, use the "
    "19200 baud speed.  For a 28800 baud modem, use the 38400 baud speed.",
    "{38400 Baud}\n\nUse a 38400 baud connection speed.\n\nNote: For a 14400 baud modem, use the "
    "19200 baud speed.  For a 28800 baud modem, use the 38400 baud speed.",
    "{Cancel}\n\nCancel back to the main menu."
};
const char* gSetupComPortHelp[KB_SETUP_COM_PORT_HELP_COUNT] = {
    "{COM 1}\n\nUse COM Port 1 for the modem connection.",
    "{COM 2}\n\nUse COM Port 2 for the modem connection.",
    "{COM 3}\n\nUse COM Port 3 for the modem connection.",
    "{COM 4}\n\nUse COM Port 4 for the modem connection.",
    "{Cancel}\n\nCancel back to the main menu."
};
const char* gSetupDCBaudHelp[KB_SETUP_DC_BAUD_HELP_COUNT] = {
    "{2400 Baud}\n\nUse a 2400 baud connection speed. \n\nNote: In general, computers with the "
    "older UART 8250 chip should use 19200 baud, and computers with the newer UART 16550 chip "
    "should use 38400 baud.  When in doubt, try slower speeds first, and if they work, then try "
    "faster speeds.  Most computers made in 1994 or later have a UART 16550 chip.",
    "{9600 Baud}\n\nUse a 9600 baud connection speed. \n\nNote: In general, computers with the "
    "older UART 8250 chip should use 19200 baud, and computers with the newer UART 16550 chip "
    "should use 38400 baud.  When in doubt, try slower speeds first, and if they work, then try "
    "faster speeds.  Most computers made in 1994 or later have a UART 16550 chip.",
    "{19200 Baud}\n\nUse a 19200 baud connection speed.\n\nNote: In general, computers with the "
    "older UART 8250 chip should use 19200 baud, and computers with the newer UART 16550 chip "
    "should use 38400 baud.  When in doubt, try slower speeds first, and if they work, then try "
    "faster speeds.  Most computers made in 1994 or later have a UART 16550 chip.",
    "{38400 Baud}\n\nUse a 38400 baud connection speed.\n\nNote: In general, computers with the "
    "older UART 8250 chip should use 19200 baud, and computers with the newer UART 16550 chip "
    "should use 38400 baud.  When in doubt, try slower speeds first, and if they work, then try "
    "faster speeds.  Most computers made in 1994 or later have a UART 16550 chip.",
    "{Cancel}\n\nCancel back to the main menu."
};
const char* gSetupDCComPortHelp[KB_SETUP_DC_COM_PORT_HELP_COUNT] = {
    "{COM 1}\n\nUse COM Port 1 for the direct connection.",
    "{COM 2}\n\nUse COM Port 2 for the direct connection.",
    "{COM 3}\n\nUse COM Port 3 for the direct connection.",
    "{Com 4}\n\nUse COM Port 4 for the direct connection.",
    "{Cancel}\n\nCancel back to the main menu."
};
const char* gSetupHotSeatGameHelp[KB_SETUP_HOT_SEAT_HELP_COUNT] = {
    "{2 Players}\n\nPlay with 2 human players, and optionally, up to 4 additional computer "
    "players.",
    "{3 Players}\n\nPlay with 3 human players, and optionally, up to 3 additional computer "
    "players.",
    "{4 Players}\n\nPlay with 4 human players, and optionally, up to 2 additional computer "
    "players.",
    "{5 Players}\n\nPlay with 5 human players, and optionally 1 computer player.",
    "{6 Players}\n\nPlay with 6 human players.",
    "{Cancel}\n\nCancel back to the main menu."
};
const char* gSetupModemGameHelp[KB_SETUP_MODEM_HELP_COUNT] = {
    "{Host}\n\nThe host sets up the game options, chooses the number to dial, and places the call.",
    "{Guest}\n\nThe guest waits for the host to call and set up the game.",
    "{Config}\n\nChange your modem configuration.",
    "{Cancel}\n\nCancel back to the main menu."
};
const char* gSetupDCGameHelp[KB_SETUP_DIRECT_CONNECT_HELP_COUNT] = {
    "{Host}\n\nThe host sets up the game options.",
    "{Guest}\n\nThe guest waits for the host to set up the game.",
    "{Config}\n\nChange your direct connect port configuration.",
    "{Cancel}\n\nCancel back to the main menu."
};
const char* gSetupMultiPlayerGameHelp[KB_SETUP_MULTIPLAYER_HELP_COUNT] = {
    "{Hot Seat}\n\nPlay a Hot Seat game, where 2 to 4 players play around the same computer, "
    "switching into the 'Hot Seat' when it is their turn.",
    "{Network}\n\nPlay a network game, where 2 players use their own computers connected through a "
    "LAN (Local Area Network).",
    "{Modem}\n\nPlay a modem game, where 2 players use their own computers connected over the "
    "phone lines using modems.",
    "{Direct Connect}\n\nPlay a direct connect game, where 2 players use their own computers "
    "directly connected through their serial port by a null modem.",
    "{Cancel}\n\nCancel back to the main menu."
};
const char* gSetupNetworkGameHelp[KB_SETUP_NETWORK_HELP_COUNT] = {
    "{Host}\n\nThe host sets up the game options.  There can only be one host per network game.",
    "{Guest}\n\nThe guest waits for the host to set up the game, then is automatically added in.  "
    "There can be multiple guests for TCP/IP and IPX games, but only 1 guest for NetBIOS games.",
    "{Cancel}\n\nCancel back to the main menu."
};
const char* gSetupNetworkGame2Help[KB_SETUP_NETWORK_SECOND_HELP_COUNT] = {
    "{IPX}\n\nIPX networking is the most commonly used form of network in Windows 95.  Up to 6 "
    "players can play using IPX.  Only the Windows 95 version of Heroes II supports IPX "
    "networking.",
    "{TCP/IP}\n\nTCP/IP is most commonly used as a way of connecting machines over the internet.  "
    "Up to 6 players can play using IPX.  To connect over the internet, you must have an account "
    "with an ISP (Independent Service Provider).  Only the Windows 95 version of Heroes II "
    "supports TCP/IP networking.",
    "{NETBios}\n\nNETBios networking is the only form of networking available for DOS machines, "
    "and is also available for Windows 95.  Only two players may connect with NetBIOS.  If all "
    "machines you wish to connect are using the Windows 95 version of Heroes II, then IPX "
    "networking is recommended.",
    "{Cancel}\n\nCancel back to the main menu."
};
const char* gSetupGameHelp[KB_SETUP_GAME_HELP_COUNT] = {
    "{Standard Game}\n\nA single player game playing out a single map.",
    "{Campaign Game}\n\nA single player game playing through a series of maps.",
    "{Multi-Player Game}\n\nA multi-player game, with several human players competing against each "
    "other on a single map.",
    "{Cancel}\n\nCancel back to the main menu."
};
const char* cBattleResults[KB_BATTLE_RESULT_TEXT_COUNT] = {
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
    "\n\nFor valor in combat, %s receives %d experience, and gains %d level(s)."
};
const char* cMoraleInfo[KB_MORALE_INFO_TEXT_COUNT] = {
    "{Good Morale}\n\nGood morale may give your armies extra attacks in combat.",
    "{Neutral Morale}\n\nNeutral morale means your armies will never be blessed with extra attacks "
    "or freeze in combat.",
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
const char* cMapSize[KB_MAP_SIZE_TEXT_COUNT] = {"Small", "Medium", "Large", "Huge"};
const char* cDifficulty[KB_DIFFICULTY_TEXT_COUNT] =
    {"Easy", "Normal", "Hard", "Expert", "Impossible"
};
const char* cStartDifficulty[KB_START_DIFFICULTY_TEXT_COUNT] = {"Easy", "Normal", "Hard", "Expert"};
const char* cCampaignLeaders[KB_CAMPAIGN_LEADER_TEXT_COUNT] =
    {"Lord Ironfist", "Lord Slayer", "Queen Lamanda", "Lord Alamar"};
const char* cWinText[KB_WIN_TEXT_COUNT] =
    {"Days Spent:", "Base Score:", "Difficulty Rating:", "Final Score:", "Ranking:"
};
const char* cHumanDifficulty[KB_HUMAN_DIFFICULTY_TEXT_COUNT] =
    {"Human\n", "Human\nEasy", "Human\nNormal", "Human\nHard", "Human\nExpert"
};
const char* cHumanInfoDifficulty[KB_HUMAN_INFO_DIFFICULTY_TEXT_COUNT] =
    {"Human-", "Human-Easy", "Human-Normal", "Human-Hard", "Human-Expert"
};
const char* musicQualityText[KB_MUSIC_QUALITY_TEXT_COUNT] =
    {"MIDI", "CD Stereo w/o Opera", "CD Stereo with Opera"};
const char* gSpellDesc[KB_SPELL_TEXT_CAPACITY] = {
    "{Fireball}\n\nCauses a giant fireball to strike the selected area, damaging all nearby "
    "creatures.",
    "{Fireblast}\n\nAn improved version of fireball, fireblast affects two hexes around the center "
    "point of the spell, rather than one.",
    "{Lightning Bolt}\n\nCauses a bolt of electrical energy to strike the selected creature.",
    "{Chain Lightning}\n\nCauses a bolt of electrical energy to strike a selected creature, then "
    "strike the nearest creature with half damage, then strike the NEXT nearest creature with half "
    "again damage, and so on, until it becomes too weak to be harmful.  Warning:  This spell can "
    "hit your own creatures!",
    "{Teleport}\n\nTeleports the creature you select to any open position on the battlefield.",
    "{Cure}\n\nRemoves all negative spells cast upon one of your units, and restores up to 5 HP "
    "per level of spell power.",
    "{Mass Cure}\n\nRemoves all negative spells cast upon your forces, and restores up to 5 HP per "
    "level of spell power, per creature.",
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
    "{Steelskin}\n\nIncreases the defense skill of the targeted creatures.  This is an improved "
    "version of Stoneskin.",
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
    "{Meteor Shower}\n\nA rain of rocks strikes an area of the battlefield, damaging all nearby "
    "creatures.",
    "{Paralyze}\n\nThe targeted creatures are paralyzed, unable to move or retaliate.",
    "{Hypnotize}\n\nBrings a single enemy unit under your control for one combat round if its hits "
    "are less than 25 times the caster's spell power.",
    "{Cold Ray}\n\nDrains body heat from a single enemy unit.",
    "{Cold Ring}\n\nDrains body heat from all units surrounding the center point, but not "
    "including the center point.",
    "{Disrupting Ray}\n\nReduces the defense rating of an enemy unit by three.",
    "{Death Ripple}\n\nDamages all living (non-undead) units in the battle.",
    "{Death Wave}\n\nDamages all living (non-undead) units in the battle.  This spell is an "
    "improved version of Death Ripple.",
    "{Dragon Slayer}\n\nGreatly increases a unit's attack skill vs. Dragons.",
    "{Blood Lust}\n\nIncreases a unit's attack skill.",
    "{Animate Dead}\n\n'Resurrects' creatures from a damaged or dead undead unit permanently.",
    "{Mirror Image}\n\nCreates an illusionary unit that duplicates one of your existing units.  "
    "This illusionary unit does the same damages as the original, but will vanish if it takes any "
    "damage.",
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
    "{Summon Boat}\n\nSummons the nearest unoccupied, friendly boat to an adjacent shore location. "
    " A friendly boat is one which you just built or were the most recent player to occupy.",
    "{Dimension Door}\n\nAllows the caster to magically transport to a nearby location.",
    "{Town Gate}\n\nReturns the caster to any town or castle currently owned.",
    "{Town Portal}\n\nReturns the hero to the town or castle of choice, provided it is controlled "
    "by you.",
    "{Visions}\n\n Visions predicts the likely outcome of an encounter with a neutral army camp.",
    "{Haunt}\n\nHaunts a mine you control with Ghosts.  This mine stops producing resources.  (If "
    "I can't keep it, nobody will!)",
    "{Set Earth Guardian}\n\nSets Earth Elementals to guard a mine against enemy armies.",
    "{Set Air Guardian}\n\nSets Air Elementals to guard a mine against enemy armies.",
    "{Set Fire Guardian}\n\nSets Fire Elementals to guard a mine against enemy armies.",
    "{Set Water Guardian}\n\nSets Water Elementals to guard a mine against enemy armies.",
    "{Awareness}\n\nExplores a large area around your hero.",
    "{Shadow Mark}\n\nMarked creature receives 150% damage from your troops for 1 turn",
    "{Marksman Pierce}\n\nTarget creature receives 1000 damage and is dazed for 1 turn",
    "{Plasma Cone}\n\nCreates a cone of hot plasma in the direction of the target",
    "{Force Shield}\n\nCreates a kinetic shield over the target ally",
    "{Mass Force Shield}\n\nCreates a kinetic shield over all allies",
    "{Fire Bomb}\n\nSet an area on fire for 2 turns. Creatures will continue burning for 2 turns "
    "after leaving the area",
    "{Implosion Grenade}\n\nFires an indirect projectile that explodes inverted dark matter that "
    "will suck any creature adjacent to it together.",
    NULL
};
const char* gSpellNames[KB_SPELL_TEXT_CAPACITY] = {
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
    "Awareness",
    "Shadow Mark",
    "Marksman Pierce",
    "Plasma Cone",
    "Force Shield",
    "Mass Force Shield",
    "Fire Bomb",
    "Implosion Grenade",
    NULL
};
const char* gSecondarySkillLevels[KB_SECONDARY_SKILL_LEVEL_TEXT_COUNT] =
    {"Basic", "Advanced", "Expert"
};
const char* gSecondarySkills[KB_SECONDARY_SKILL_TEXT_COUNT] = {
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
const char* gNeutralBuildingNames[KB_NEUTRAL_BUILDING_TEXT_COUNT] = {
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
    ""
};
const char* gWellExtraNames[KB_FACTION_TABLE_CAPACITY] = {
    "Farm",
    "Garbage Heap",
    "Crystal Garden",
    "Waterfall",
    "Orchard",
    "Skull Pile",
    "1st Lvl Growth"
};
const char* gSpecialBuildingNames[KB_FACTION_TABLE_CAPACITY] =
    {"Fortifications", "Coliseum", "Rainbow", "Dungeon", "Library", "Storm", "Special", NULL};
const char* gDwellingNames[KB_FACTION_TABLE_CAPACITY][KB_DWELLING_TYPE_COUNT] = {
    {"Thatched Hut",
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
     ""},
    {"Hut",
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
     ""},
    {"Treehouse",
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
     ""},
    {"Cave",
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
     "Black Tower"},
    {"Habitat",
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
     ""},
    {"Excavation",
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
     ""}
};
// The Cyborg Wisdom slot reads as Cybernetics, with its own descriptions.
const char* cyberneticsDesc[SECONDARY_SKILL_VALUE_LEVEL_COUNT] = {
    "{Basic Cybernetics}\n\nBasic Cybernetics allows your hero to learn Level 1-3 spells "
    "from a Cybernetics Lab.",
    "{Advanced Cybernetics}\n\nAdvanced Cybernetics allows your hero to learn Level 4 "
    "spells from a Cybernetics Lab.",
    "{Expert Cybernetics}\n\nExpert Cybernetics allows your hero to learn Level 5 spells "
    "from a Cybernetics Lab."
};
const char* cSecSkillDesc[H2EnumIndex(HERO_SKILL_COUNT)][SECONDARY_SKILL_VALUE_LEVEL_COUNT] = {
    {"{Basic Pathfinding}\n\nBasic Pathfinding reduces the movement penalty for rough "
     "terrain by 25 percent.",
     "{Advanced Pathfinding}\n\nAdvanced Pathfinding reduces the movement penalty for "
     "rough terrain by 50 percent.",
     "{Expert Pathfinding}\n\nExpert Pathfinding eliminates the movement penalty for rough "
     "terrain."},
    {"{Basic Archery}\n\nBasic Archery increases the damage done by range attacking "
     "creatures by 10 percent.",
     "{Advanced Archery}\n\nAdvanced Archery increases the damage done by range attacking "
     "creatures by 25 percent.",
     "{Expert Archery}\n\nExpert Archery increases the damage done by range attacking "
     "creatures by 50 percent."},
    {"{Basic Logistics}\n\nBasic Logistics increases your hero's movement points by 10 "
     "percent.",
     "{Advanced Logistics}\n\nAdvanced Logistics increases your hero's movement points by "
     "20 percent.",
     "{Expert Logistics}\n\nExpert Logistics increases your hero's movement points by 30 "
     "percent."},
    {"{Basic Scouting}\n\nBasic Scouting increases your hero's viewable area by 1 square.",
     "{Advanced Scouting}\n\nAdvanced Scouting increases your hero's viewable area by 2 "
     "squares.",
     "{Expert Scouting}\n\nExpert Scouting increases your hero's viewable area by 3 "
     "squares."},
    {"{Basic Diplomacy}\n\nBasic Diplomacy allows you to negotiate with monsters who are "
     "weaker than your group.  Approximately 1/4 of the creatures may offer to join you.",
     "{Advanced Diplomacy}\n\nAdvanced Diplomacy allows you to negotiate with monsters who "
     "are weaker than your group.  Approximately 1/2 of the creatures may offer to join "
     "you.",
     "{Expert Diplomacy}\n\nExpert Diplomacy allows you to negotiate with monsters who are "
     "weaker than your group.  All of the creatures may offer to join you."},
    {"{Basic Navigation}\n\nBasic Navigation increases your hero's movement points over "
     "water by 1/3.",
     "{Advanced Navigation}\n\nAdvanced Navigation increases your hero's movement points "
     "over water by 2/3.",
     "{Expert Navigation}\n\nExpert Navigation doubles your hero's movement points over "
     "water."},
    {"{Basic Leadership}\n\nBasic Leadership increases your hero's troops' morale by 1.",
     "{Advanced Leadership}\n\nAdvanced Leadership increases your hero's troops' morale by "
     "2.",
     "{Expert Leadership}\n\nExpert Leadership increases your hero's troops' morale by 3."},
    {"{Basic Wisdom}\n\nBasic Wisdom allows your hero to learn third level spells.",
     "{Advanced Wisdom}\n\nAdvanced Wisdom allows your hero to learn fourth level spells.",
     "{Expert Wisdom}\n\nExpert Wisdom allows your hero to learn fifth level spells."},
    {"{Basic Mysticism}\n\nBasic Mysticism regenerates two of your hero's spell points per "
     "day.",
     "{Advanced Mysticism}\n\nAdvanced Mysticism regenerates three of your hero's spell "
     "points per day.",
     "{Expert Mysticism}\n\nExpert Mysticism regenerates four of your hero's spell points "
     "per day."},
    {"{Basic Luck}\n\nBasic Luck increases your hero's luck by 1.",
     "{Advanced Luck}\n\nAdvanced Luck increases your hero's luck by 2.",
     "{Expert Luck}\n\nExpert Luck increases your hero's luck by 3."},
    {"{Basic Ballistics}\n\nBasic Ballistics gives your hero's catapult shots a greater "
     "chance to hit and do damage to castle walls.",
     "{Advanced Ballistics}\n\nAdvanced Ballistics gives your hero's catapult an extra "
     "shot, and each shot has a greater chance to hit and do damage to castle walls.",
     "{Expert Ballistics}\n\nExpert Ballistics gives your hero's catapult an extra shot, "
     "and each shot automatically destroys any wall, except a fortified wall in a Knight "
     "town."},
    {"{Basic Eagle Eye}\n\nBasic Eagle Eye gives your hero a 20 percent chance to learn "
     "any given 1st or 2nd level enemy spell used against him in a combat.",
     "{Advanced Eagle Eye}\n\nAdvanced Eagle Eye gives your hero a 30 percent chance to "
     "learn any given 3rd level spell (or below) used against him in combat.",
     "{Expert Eagle Eye}\n\nExpert Eagle Eye gives your hero a 40 percent chance to learn "
     "any given 4th level spell (or below) used against him in combat."},
    {"{Basic Necromancy}\n\nBasic Necromancy allows 10 percent of the creatures killed in "
     "combat to be brought back from the dead as Skeletons.",
     "{Advanced Necromancy}\n\nAdvanced Necromancy allows 20 percent of the creatures "
     "killed in combat to be brought back from the dead as Skeletons.",
     "{Expert Necromancy}\n\nExpert Necromancy allows 30 percent of the creatures killed "
     "in combat to be brought back from the dead as Skeletons."},
    {"{Small Estates}\n\nYour hero produces 100 gold pieces per turn as tax revenue from "
     "estates.",
     "{Moderate Estates}\n\nYour hero produces 250 gold pieces per turn as tax revenue "
     "from estates.",
     "{Grand Estates}\n\nYour hero produces 500 gold pieces per turn as tax revenue from "
     "estates."}
};
const char* cBuildingInfoNeutral[KB_NEUTRAL_BUILDING_INFO_COUNT] = {
    "The Mage Guild allows heroes to learn spells and replenish their spell points.",
    "The Thieves' Guild provides information on enemy players.  Thieves' Guilds can also provide "
    "scouting information on enemy towns.  Additional Guilds provide more information.",
    "The Tavern increases morale for troops defending the castle.",
    "The Shipyard allows ships to be built.",
    "The Well increases the growth rate of all dwellings by 2 creatures per week.",
    "The Tent provides workers to build a castle, provided the materials and the gold are "
    "available.",
    "The Castle improves town defense and increases income to 1000 gold per day.",
    "The Statue increases your town's income by 250 per day.",
    "The Left Turret provides extra firepower during castle combat.",
    "The Right Turret provides extra firepower during castle combat.",
    "The Marketplace can be used to convert one type of resource into another.  The more "
    "marketplaces you control, the better the exchange rate.",
    "",
    "The Moat slows attacking units.  Any unit entering the moat must end its turn there and "
    "becomes more vulnerable to attack.",
    "",
    "The Shipyard allows ships to be built.",
    "The Captain's Quarters provides a captain to assist in the castle's defense when no hero is "
    "present.",
    "",
    "",
    ""
};
const char* gBuildingInfoSpecial[KB_FACTION_TABLE_CAPACITY] = {
    "The Fortifications increase the toughness of the walls, increasing the number of turns it "
    "takes to knock them down.",
    "The Coliseum provides inspiring spectacles to defending troops, raising their morale by two "
    "during combat.",
    "The Rainbow increases the luck of the defending units by two.",
    "The Dungeon increases the income of the town by 500 / day.",
    "The Library increases the number of spells in the Guild by one for each level of the guild.",
    "The Storm adds +2 to the power of spells of a defending spell caster."
};
const char* cDirections[KB_DIRECTION_TEXT_COUNT] = {
    "northern",
    "northeastern",
    "eastern",
    "southeastern",
    "southern",
    "southwestern",
    "western",
    "northwestern",
    "central"
};
const char* cRumourTerrainDescriptions[KB_RUMOUR_TERRAIN_DESCRIPTION_COUNT] = {
    "in the murky depths of the ocean.",
    "on a grassy plain.",
    "in the driven snow.",
    "in a murky swamp.",
    "buried in barren lava.",
    "under a desert drift.",
    "in the dirt.",
    "in a parched wasteland.",
    "buried on a beach."
};
const char* gInterfaceTypeText[KB_INTERFACE_TYPE_TEXT_COUNT] = {"Dynamic", "Good", "Evil"
};
const char* cBWMouseText[KB_BW_MOUSE_TEXT_COUNT] = {"Black & White", "Color"};
const char* combatSpeedText[KB_COMBAT_SPEED_TEXT_COUNT] = {"Normal", "Fast", "Very Fast"
};
const char* combatMiniInfoText[KB_COMBAT_MINI_INFO_TEXT_COUNT] = {"None", "Spells Only", "Full"
};
const char* gcCommandLineHelp[KB_COMMAND_LINE_HELP_COUNT] = {
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
const char* cOverviewText[KB_OVERVIEW_TEXT_COUNT] =
    {"Hero/Stats", "Skills", "Artifacts", "Town/Castle", "Garrison", "Available"};
const char* cWinComError[KB_WIN_COM_ERROR_TEXT_COUNT] = {
    "Communications error on function '%s'\n\nWin95 Error Code: %d\nWin95 Error Meaning: %s\n\n",
    "Suggested solutions:",
    "\n  1) Make sure all cables are firmly connected.",
    "\n  2) Reboot computer.",
    "\n  3) Check to make sure you have the correct COM port setting in 'CONFIG'. (The 3rd button "
    "down on the screen where you choose Host or Guest.)",
    "\n  4) Consider lowering the BAUD rate in 'CONFIG' to 19200 or 9600."
};
const char* cMiniViewText[KB_MINI_VIEW_TEXT_COUNT] =
    {"%d Units", "%d Unit", "Attack", "Defense", "HP ", "Dmg", "Mrl", "Luk", "Shots"
};
const char* gFileRequestHelp[KB_FILE_REQUEST_HELP_COUNT] = {
    "{Small Maps}\n\nView only maps of size small (36 x 36).",
    "{Medium Maps}\n\nView only maps of size medium (72 x 72).",
    "{Large Maps}\n\nView only maps of size large (108 x 108).",
    "{Extra Large Maps}\n\nView only maps of size extra large (144 x 144).",
    "{All Maps}\n\nView all maps, regardless of size.",
    "{Enter Name}\n\nEnter the name of the file you wish to save.",
    "{OK}\n\nAccept the choice made.",
    "{Cancel}\n\nCancel without making a choice.",
    "{Size Icon}\n\nIndicates whether the map is small (36 x 36), medium (72 x 72), large (108 x "
    "108), or extra large (144 x 144).",
    "{Players Icon}\n\nIndicates how many players total are in the scenario.  Any positions not "
    "occupied by humans will be occupied by computer players.",
    "{Victory Condition Icon}\n\nThere are 6 possibilities:\n\n{Tombstone} - Defeat all enemy "
    "heroes and towns.\n\n{Town} - Capture a specific town.\n\n{Hero Face} - Defeat a specific "
    "hero.\n\n{Medal} - Find a specific artifact.\n\n{Handshake} - Your side defeats the opposing "
    "side.\n\n{Coins} - Accumulate a large amount of gold.",
    "{Loss Condition Icon}\n\nThere are 4 possible loss conditions, as indicated by the following "
    "icons:\n\n{Tombstone} - Lose all your heroes and towns.\n\n{Town} - Lose a specific "
    "town.\n\n{Hero Face} - Lose a specific hero.\n\n{Hourglass} - Run out of time. (Fail to win "
    "by a certain point.)",
    "{Selected Name}\n\nThe name of the currently selected map.",
    "{Selected Description}\n\nThe description of the currently selected map.",
    "{Selected Map Difficulty}\n\nThe map difficulty of the currently selected map.  The map "
    "difficulty is determined by the scenario designer.  More difficult maps might include more or "
    "stronger enemies, fewer resources, or other special conditions making things tougher for the "
    "human player."
};
const char* cPersonality[KB_PERSONALITY_TEXT_COUNT] = {"Warrior", "Builder", "Explorer", "Human"};
const char* gArmySizeNames[KB_ARMY_SIZE_NAME_COUNT][KB_ARMY_SIZE_NAME_VARIANT_COUNT] = {
    {"Few", "A few", "a few"},
    {"Several", "Several", "several"},
    {"Pack", "A pack of", "a pack of"},
    {"Lots", "Lots of", "lots of"},
    {"Horde", "A Horde of", "a horde of"},
    {"Throng", "A Throng of", "a throng of"},
    {"Swarm", "A Swarm of", "a swarm of"},
    {"Zounds", "Zounds...", "zounds of"},
    {"Legion", "A Legion of", "a legion of"}
};
const char* cRandomTavernText[KB_RANDOM_TAVERN_TEXT_COUNT] = {
    "The truth is out there.",
    "The dark side is stronger.",
    "The end of the world is near.",
    "The bones of Lord Slayer are buried in the foundation of the arena.",
    "He's innocent.",
    "A Black Dragon will take out a Titan any day of the week.",
    "He told her \"Yada yada yada\"...  and then she said \"Blah, blah, blah...\"",
    "There once was a man from Nantucket..."
};
const char* cRandomSignText[KB_RANDOM_SIGN_TEXT_COUNT] =
    {"See Rock City", "This space for rent", "Next sign 50 miles", "Burma shave"};
const char* cCampaignAwards[KB_CAMPAIGN_AWARD_TEXT_COUNT] = {
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
const char* cCampaignName[H2EnumIndex(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT] = {
    {"Force of Arms",
     "Annexation",
     "Save the Dwarves",
     "Carator Mines",
     "Turning Point",
     "Defender",
     "The Gauntlet",
     "The Crown",
     "Corlagon's Defense",
     "Final Justice",
     "",
     "Betrayal"},
    {"First Blood",
     "Barbarian Wars",
     "Necromancers",
     "Slay the Dwarves",
     "Turning Point",
     "Rebellion",
     "Dragon Master",
     "Country Lords",
     "The Crown",
     "Greater Glory",
     "Apocalypse",
     "Betrayal"}
};
const char* cCampaignDescription[H2EnumIndex(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT] = {
    {"Roland needs you to defeat the lords near his castle to begin his war of "
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
     "battle will be the easiest one you will face for the rest of the war...traitor."},
    {"King Archibald requires you to defeat the three enemies in this region.  "
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
     "battle will be the easiest one you will face for the rest of the war...traitor."}
};
const char* cOutOfMemory =
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n%s\nHeroes II requires a minimum of\n%dK of Extended Memory (XMS) "
    "and\n480K of Conventional Memory\n\n";
const char* cSlowVideoLevelText[KB_SLOW_VIDEO_LEVEL_TEXT_COUNT] = {
    "Normal",
    "Interlaced"
};
const char* gSPanelHelp[KB_SETTINGS_PANEL_HELP_COUNT] = {
    "{OK}\n\nExit this menu.",
    "{Music}\n\nToggle ambient music level.",
    "{Effects}\n\nToggle foreground sounds level.",
    "{Speed}\n\nChange the speed at which your heroes move on the main screen.",
    "{Music Type}\n\nHeroes II uses MIDI music.",
    "{Show Path}\n\nToggle 'Show Path' on/off.  If 'Show Path' is on, your first click on a map "
    "location will show the path to get there, your second will start you moving. If this option "
    "is off, one click starts you moving immediately.",
    "{Enemy Speed}\n\nSets the speed that A.I. heroes move at.  You can also elect not to view "
    "A.I. movement at all.",
    "{Interface}\n\nSets what type of interface you want to use.  The default selection is a "
    "dynamic interface, which uses Evil graphics for the 3 evil heroes (Barbarian, Warlock, and "
    "Necromancer).",
    "{Video}\n\nDetermines if the video sequences play normally or in interlaced mode. Interlaced "
    "mode runs better on slower machines.",
    "{Mouse Cursor}\n\nToggle color cursors on/off.  Color cursors look nicer, but sometimes don't "
    "move as smoothly as black and white ones."
};
const char* xBarrierColor[KB_BARRIER_COLOR_NAME_COUNT] =
    {"aqua", "blue", "brown", "gold", "green", "orange", "purple", "red"};
const char* xGenericSiteNames[KB_GENERIC_SITE_NAME_COUNT] = {
    "Alchemist's Tower",
    "Arena",
    "Hut of the Magi",
    "Eye of the Magi",
    "Stables",
    "Mermaid",
    "Sirens"
};
const char* xRecruitmentSiteNames[KB_RECRUITMENT_SITE_NAME_COUNT] = {
    "Barrow Mounds",
    "Earth Summoning Altar",
    "Air Summoning Altar",
    "Fire Summoning Altar",
    "Water Summoning Altar"
};
SWinSetup gWinSetup[KB_WIN_SETUP_COUNT] = {
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
H2EnumStorage<CDRomSetupResult, i32> iCDRomErr = CD_ROM_READY;
i32 bEarlySetupDone = 0;
b32 bKBDone = false;
struct _REDBOOK* hRedbookz = NULL;
b32 bForceCheckTimeEvent = false;
u16 IMHotSpots[KB_INIT_MENU_HOTSPOT_COUNT][H2EnumIndex(INIT_MENU_HOTSPOT_FIELD_COUNT)] = {
    {481, 185, 83, 96},
    {194, 179, 82, 79},
    {412, 105, 75, 76},
    {303, 137, 75, 44},
    {0, 389, 86, 90}
};

i32 lastIMHoverID = -1;
b32 bInCheckEndGame = false;
b32 bInShutDown = false;
b32 gbInMemError = false;
i32 iShingleAnimFrame = 0;
b32 gbHumanPlayer[H2EnumIndex(GAME_PLAYER_COUNT)];
b32 gbHitEvent;
i32 giMaxExtentX;
i32 giMaxExtentY;
i32 giRandomClouds;
char cOverrideDigitalDriver[GLOBAL_DRIVER_NAME_SIZE];
i32 giBottomViewOverrideEndTime;
b8 gArmyEffected[COMBAT_SIDE_COUNT][KB_ARMY_EFFECT_COUNT];
H2EnumStorage<ResourceType, i32> giBottomViewResource;
b32 gbInCampaign;
i32 giResExtra1;
i32 giResExtra2;
i8 puzzlePiecesRemoved[PUZZLE_PIECE_STORAGE_SIZE];
b32 giSeedingValid;
i32 giLimitPlayer;
i32 giShowClouds;
b32 bDoColorCycle;
inputManager* gpInputManager;
i32 iMaxMapExtra;
palette* gPalette;
resourceManager* gpResourceManager;
char gcBotViewText[GLOBAL_BOTTOM_VIEW_TEXT_SIZE];
b32 bSpecialHideCursor;
searchArray* gpSearchArray;
i32 giResType1;
b32 gbBlackoutPlayer;
i32 giResType2;
char cNetBoxLine[BOX_LINE_COUNT][NET_BOX_LINE_SIZE];
i32 gIndex;
i32 giWeekTypeExtra;
philAI* gpPhilAI;
i32 giTCPType;
H2EnumStorage<CombatEffectType, i32> gCurLoadedSpellEffect;
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
H2EnumStorage<CampaignSide, i32> gbCampaignSideChoice;
char cNetBoxColor[BOX_LINE_COUNT];
i32 giMonthTypeExtra;
H2EnumStorage<RemoteGameMode, i32> iMPExtendedType;
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
H2EnumStorage<BottomViewMode, i32> giBottomViewOverride;
char gcTCPAddress[GLOBAL_TCP_TEXT_SIZE];
u8 giSetupGameType;
char gLastFilename[GLOBAL_LAST_FILENAME_SIZE];
b32 giFullySeeded;
icon* gBuyBuildIcons;
i32 iCombatControlNetPos[COMBAT_SIDE_COUNT];
char cExpAggPathName[GLOBAL_AGGREGATE_PATH_SIZE];
b32 gbMoveShown;
void** ppMapExtra;
char gcBottomViewText[GLOBAL_BOTTOM_VIEW_MESSAGE_SIZE];
i32 giThisNetPos;
i8 gbSetupGamePosToRealGamePos[RADAR_OWNER_COLOR_COUNT];
char gcRegCDRomPath[GLOBAL_AGGREGATE_PATH_SIZE];
class heroWindow* heroWin;
i32 giOverviewReturnActionExtra;
H2EnumStorage<CombatSide, i32> giCurGeneral;
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
b32 bFreshSave;
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
i8 gbGamePosToNetPos[OLD_MAIN_MATCH_BUFFER_SIZE];
