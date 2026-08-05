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
#include <BASE/MiscEnums.h>
#include <BASE/MiscGraphicsConstants.h>
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
#include <BASE/bitmap.h>
#include <BASE/sample.h>
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
void EarlyShutdown(char* caption, char* text) {
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
                "\xcd\xe5\xf2 \xe4\xee\xf1\xf2\xf3\xef\xe0 \xea CD-ROM. \xc1\xe5\xe7 "
                "\xe4\xee\xf1\xf2\xf3\xef\xe0 \xea CD-ROM \xc3\xe5\xf0\xee\xe8 2 \xe1\xf3\xe4\xf3"
                "\xf2 \xef\xee\xe4\xe4\xe5\xf0\xe6\xe8\xe2\xe0\xf2\xfc \xf2\xee\xeb\xfc\xea\xee "
                "\xf1\xe5\xf2\xe5\xe2\xf3\xfe \xe8\xe3\xf0\xf3 \xe2 \xf0\xe5\xe6\xe8\xec\xe5 "
                "\xe3\xee\xf1\xf2\xff.",
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
                " \xcd\xe5\xf2 \xe4\xee\xf1\xf2\xf3\xef\xe0 \xea CD-ROM. \xc1\xe5\xe7 "
                "\xe4\xee\xf1\xf2\xf3\xef\xe0 \xea CD-ROM \xc3\xe5\xf0\xee\xe8 2 \xe1\xf3\xe4\xf3"
                "\xf2 \xef\xee\xe4\xe4\xe5\xf0\xe6\xe8\xe2\xe0\xf2\xfc \xf2\xee\xeb\xfc\xea\xee "
                "\xf1\xe5\xf2\xe5\xe2\xf3\xfe \xe8\xe3\xf0\xf3 \xe2 \xf0\xe5\xe6\xe8\xec\xe5 "
                "\xe3\xee\xf1\xf2\xff. \xc5\xf1\xeb\xe8 \xf3 \xe2\xe0\xf1 \xe5\xf1\xf2\xfc "
                "\xe4\xe8\xf1\xea, \xe7\xe0\xea\xf0\xee\xe9\xf2\xe5 \xfd\xf2\xf3 \xef\xf0\xee"
                "\xe3\xf0\xe0\xec\xec\xf3 \xe8 \xe2\xf1\xf2\xe0\xe2\xfc\xf2\xe5 \xe4\xe8\xf1\xea "
                "\xe2 \xef\xf0\xe8\xe2\xee\xe4 \xe8 \xef\xee\xef\xf0\xee\xe1\xf3\xe9\xf2\xe5 "
                "\xe7\xe0\xef\xf3\xf1\xf2\xe8\xf2\xfc \xe8\xe3\xf0\xf3 \xe5\xf9\xe5 \xf0\xe0"
                "\xe7.",
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
            "\xce\xf8\xe8\xe1\xea\xe0 \xe7\xe0\xe3\xf0\xf3\xe7\xea\xe8",
            "\xcd\xe5 \xec\xee\xe3\xf3 \xef\xe5\xf0\xe5\xea\xeb\xfe\xf7\xe8\xf2\xfc\xf1\xff \xe2 "
            "\xe4\xe8\xf0\xe5\xea\xf2\xee\xf0\xe8\xfe \xc3\xe5\xf0\xee\xe5\xe2 II.  \xc7\xe0\xef"
            "\xf3\xf1\xf2\xe8\xf2\xe5 \xef\xf0\xee\xe3\xf0\xe0\xec\xec\xf3 \xf3\xf1\xf2\xe0\xed"
            "\xee\xe2\xea\xe8."
        );
        exit(0);
    }
    if (iCDRomErr == CD_ROM_DATA_FILES_MISSING) {
        EarlyShutdown(
            "\xce\xf8\xe8\xe1\xea\xe0 \xe7\xe0\xe3\xf0\xf3\xe7\xea\xe8",
            "\xcd\xe5 \xec\xee\xe3\xf3 \xed\xe0\xe9\xf2\xe8 \xf4\xe0\xe9\xeb\xfb \xe4\xe0\xed"
            "\xed\xfb\xf5 \xc3\xe5\xf0\xee\xe5\xe2 II.  \xcf\xee\xe6\xe0\xeb\xf3\xe9\xf1\xf2"
            "\xe0, \xe7\xe0\xef\xf3\xf1\xf2\xe8\xf2\xe5 \xef\xf0\xee\xe3\xf0\xe0\xec\xec\xf3 "
            "\xf3\xf1\xf2\xe0\xed\xee\xe2\xea\xe8."
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
        ShutDown("\xce\xf8\xe8\xe1\xea\xe0 \xe8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe0\xf6\xe8\xe8!");
    LogStr("OM3");
    KBChangeMenu(hmnuDflt);
    gPalette = gpResourceManager->GetPalette("kb.pal");
    gpWindowManager->m_updateFlags = 1;
    smallFont = gpResourceManager->GetFont("smalfont.fnt");
    bigFont = gpResourceManager->GetFont("bigfont.fnt");
    gpMouseManager->SetPointer("advmice.mse", 0, MOUSE_AUTO_CURSOR_TYPE);
    gpMouseManager->SetColorMice(gConfig.gfx[IDX(giCurExe)].colorMouseCursor);
    LogStr("OM4");
    SetupCDRom();
    LogStr("OM5");
    if (gpSoundManager->Open(-1))
        ShutDown(
            "\xcd\xe5 \xec\xee\xe3\xf3 \xe8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe8\xf0\xee\xe2\xe0"
            "\xf2\xfc \xe7\xe2\xf3\xea."
        );
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
            gShingleAnim = gpResourceManager->GetIcon("shnganim.icn");
        if (gGameCommand != OLD_MAIN_EXIT)
            gpSoundManager->SwitchAmbientMusic(OLD_MAIN_MAIN_MUSIC);

        if (!mainScreenLoaded_h) {
            if (gGameCommand != OLD_MAIN_EXIT) {
                gpResourceManager->GetBackdrop("heroes.icn", gpWindowManager->m_screen, 1);
                gpWindowManager
                    ->UpdateScreenRegion(0, 0, OLD_MAIN_SCREEN_WIDTH, OLD_MAIN_SCREEN_HEIGHT);
                if (firstMainScreen_h)
                    SetPalette(gPalette->m_data, 1);
                else
                    gpWindowManager->FadeScreen(FADE_IN, OLD_MAIN_FADE_SPEED, gPalette);
                firstMainScreen_h = 0;
            }
            gpMouseManager->SetPointer("advmice.mse", 0, MOUSE_AUTO_CURSOR_TYPE);
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
                gpInitWin = new heroWindow(0, 0, "stpmain.bin");
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
                    ShutDown(
                        "\xcd\xe5 \xec\xee\xe3\xf3 \xe4\xee\xe1\xe0\xe2\xe8\xf2\xfc \xec\xe5\xed"
                        "\xe5\xe4\xe6\xe5\xf0\xe0!"
                    );
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
                    "\xce\xe6\xe8\xe4\xe0\xfe \xef\xee\xeb\xf3\xf7\xe5\xed\xe8\xff \xe8\xe3\xf0\xee"
                    "\xe2\xfb\xf5 \xe4\xe0\xed\xed\xfb\xf5.",
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
                sprintf(gpGame->m_saveName, "\xcd\xce\xc2\xc0\xdf \xc8\xc3\xd0\xc0");
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
                        "%s \xe8\xe3\xf0\xee\xea",
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
                    ShutDown(
                        "\xcd\xe5 \xec\xee\xe3\xf3 \xe4\xee\xe1\xe0\xe2\xe8\xf2\xfc \xec\xe5\xed"
                        "\xe5\xe4\xe6\xe5\xf0\xe0!"
                    );
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
            gpMouseManager->SetPointer("advmice.mse", 0, MOUSE_AUTO_CURSOR_TYPE);
            sprintf(
                gcWinText,
                "\xcc\xee\xe8 \xe3\xe5\xf0\xee\xe8! \xcd\xe0\xf8\xe8 \xe2\xf0\xe0\xe3\xe8 "
                "\xe1\xfb\xeb\xe8 \xf0\xe0\xe7\xe1\xe8\xf2\xfb, \xe0 \xe8\xf5 \xe7\xe0"
                "\xec\xea\xe8 \xef\xf0\xe5\xe4\xe0\xed\xfb \xf0\xe0\xe7\xee\xf0\xe5\xed"
                "\xe8\xfe. \xc2\xe5\xeb\xe8\xea\xe8\xe9 \xef\xee\xf5\xee\xe4 \xee\xea"
                "\xee\xed\xf7\xe5\xed, \xe8 \xff \xef\xf0\xe5\xe4\xf1\xf2\xe0\xfe \xef"
                "\xe5\xf0\xe5\xe4 \xe2\xe0\xec\xe8 \xea\xe0\xea \xe2\xf1\xe5\xec\xe8 "
                "\xef\xf0\xe8\xe7\xed\xe0\xed\xed\xfb\xe9 \xc2\xe5\xeb\xe8\xea\xe8\xe9 "
                "\xca\xee\xf0\xee\xeb\xfc!\n\n\xcc\xfb \xe4\xee\xf1\xf2\xe8\xe3\xeb"
                "\xe8  \xef\xee\xe1\xe5\xe4\xfb \xe7\xe0 %d \xe4\xed\xe5\xe9!",
                giCurTurn
            );

            if (giEndSequence != 1) {
                gpWindowManager->m_updateFlags = 0;
                if (xIsExpansionMap)
                    PlaySmacker(OLD_MAIN_EXPANSION_VICTORY_VIDEO);
                else
                    PlaySmacker(OLD_MAIN_STANDARD_VICTORY_VIDEO);
                gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_LONG_FADE_SPEED, gPalette);
                gpResourceManager->GetBackdrop("heroes.icn", gpWindowManager->m_screen, 1);
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
                            const_cast<char*>(
                                gpGame->m_campaignType == CAMPAIGN_ARCHIBALD
                                    ? "\xc0\xf0\xf7\xe8\xe1\xe0\xeb\xfc\xe4"
                                    : "\xd0\xee\xeb\xe0\xed\xe4"
                            )
                        );
                    }
                    if (campaignResult) {
                        for (player_h = 0; player_h < OLD_MAIN_PLAYER_COUNT; player_h++)
                            sprintf(cPlayerNames[player_h], "");
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
                            sprintf(cPlayerNames[player_h], "");
                        xCampaign.InitMap();
                        gbGameOver = false;
                        bForceCheckTimeEvent = 1;
                        goto initialize_game;
                    }
                } else {
                    ShowCongrats(HIGH_SCORE_STANDARD);
                    if (!gbShowHighScore) {
                        gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_FADE_SPEED, gPalette);
                        gpResourceManager->GetBackdrop("heroes.icn", gpWindowManager->m_screen, 1);
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
                    ShutDown(
                        "\xcd\xe5 \xec\xee\xe3\xf3 \xe4\xee\xe1\xe0\xe2\xe8\xf2\xfc \xec\xe5\xed"
                        "\xe5\xe4\xe6\xe5\xf0\xe0!"
                    );
                gpExec->MainLoop();
                gpExec->RemoveManager(gpHighScoreManager);
                giHighScoreRank = -1;
                gpSoundManager->SwitchAmbientMusic(OLD_MAIN_MAIN_MUSIC);
                gpResourceManager->GetBackdrop("heroes.icn", gpWindowManager->m_screen, 1);
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

VA(0x00467ca8, 0x5f3)
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
    strcpy(gFullMapName, "\xd5\xe0\xee\xf1");

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

    sprintf(cAggPathName, "%s%s", ".\\DATA\\", "heroes2.agg");
    DEFAULT_AGGREGATE_NAME = cAggPathName;
    sprintf(cExpAggPathName, "%s%s", ".\\DATA\\", "heroes2x.agg");
    EXPANSION_AGGREGATE_NAME = cExpAggPathName;
    giFrameStep = LINE_FRAME_STEP;

    for (i = 0; i < LINE_HUMAN_PLAYER_SLOTS; i++)
        gbHumanPlayer[i] = i < giNumHumanPlayers;
    if (giNumHumanPlayers == LINE_SINGLE_PLAYER)
        gbBlackoutPlayer = false;

    if (giTCPHostStatus != -1) {
        if (giTCPType == -1 || giTCPNumPlayers == -1
            || (giTCPHostStatus == LINE_TCP_CLIENT && strlen(gcTCPAddress) < 1)) {
            ShutDown(
                "\xcd\xe5\xe7\xe0\xe2\xe5\xf0\xf8\xe5\xed\xed\xe0\xff TCP/IP \xea\xee\xec"
                "\xe0\xed\xe4\xed\xe0\xff \xf1\xf2\xf0\xee\xea\xe0"
            );
        }
        giShowIntro = 0;
    }
    return 1;
}

VA(0x0046829b, 0x619)
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
                        IMHotSpots[menu][IDX(INIT_MENU_HOTSPOT_X)],
                        IMHotSpots[menu][IDX(INIT_MENU_HOTSPOT_Y)],
                        IMHotSpots[menu][IDX(INIT_MENU_HOTSPOT_WIDTH)],
                        IMHotSpots[menu][IDX(INIT_MENU_HOTSPOT_HEIGHT)]
                    );
                    break;
                case INIT_MENU_CLICK_COMMAND:
                    if (msg.payload.widget.id == MENU_MOVIE) {
                        PlaySmacker(MENU_MOVIE_SMACKER);
                        gpResourceManager->GetBackdrop("heroes.icn", gpWindowManager->m_screen, 1);
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
                if (msg.payload.mouse.screenX >= IMHotSpots[idx][IDX(INIT_MENU_HOTSPOT_X)]
                    && msg.payload.mouse.screenY >= IMHotSpots[idx][IDX(INIT_MENU_HOTSPOT_Y)]
                    && msg.payload.mouse.screenX
                           < IMHotSpots[idx][IDX(INIT_MENU_HOTSPOT_X)]
                                 + IMHotSpots[idx][IDX(INIT_MENU_HOTSPOT_WIDTH)]
                    && msg.payload.mouse.screenY
                           < IMHotSpots[idx][IDX(INIT_MENU_HOTSPOT_Y)]
                                 + IMHotSpots[idx][IDX(INIT_MENU_HOTSPOT_HEIGHT)]) {
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
                        IMHotSpots[lastIMHoverID][IDX(INIT_MENU_HOTSPOT_X)],
                        IMHotSpots[lastIMHoverID][IDX(INIT_MENU_HOTSPOT_Y)],
                        IMHotSpots[lastIMHoverID][IDX(INIT_MENU_HOTSPOT_WIDTH)],
                        IMHotSpots[lastIMHoverID][IDX(INIT_MENU_HOTSPOT_HEIGHT)]
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
        msg.type = INIT_MENU_MESSAGE;
        msg.payload.widget.id = MENU_CLOSE_COMMAND;
        msg.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
        return MESSAGE_DISPATCH_FORWARD;
    }
    CheckShingleUpdate();
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x004688b4, 0x10)
MessageDispatchResult NullHandler(struct tag_message& msg) {
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x004688c4, 0x145)
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

VA(0x00468a09, 0x179)
char* GetBuildingInfo(FactionType race, BuildingSlotType building, i32 mode) {
    char buf[BUILDING_INFO_BUFFER_SIZE];
    if (race == FACTION_NECROMANCER && building == BUILDING_SLOT_NECROMANCER_SHRINE) {
        sprintf(buf, xNecromancerShrineDesc);
    } else if (building == BUILDING_SLOT_WELL_EXTRA) {
        sprintf(
            buf,
            "%s \xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2 \xef\xf0\xe8\xf0\xee\xf1\xf2 %s "
            "\xed\xe0 8 \xe2 \xed\xe5\xe4\xe5\xeb\xfe." /* "%s увеличивает прирост %s на 8 в неделю." */
            ,
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
            "\xc2 \xef\xee\xf1\xf2\xf0\xee\xe9\xea\xe5 '%s' \xec\xee\xe6\xed\xee "
            "\xea\xf3\xef\xe8\xf2\xfc %s." /* "В постройке '%s' можно купить %s." */,
            GetBuildingName(race, building),
            gArmyNamesPlural
                [IDX(gDwellingType[IDX(race)][IDX(building) - IDX(BUILDING_SLOT_DWELLING_FIRST)])]
        );
        return gText;
    }
    if (mode) {
        sprintf(gText, "{%s}\n\n%s", GetBuildingName(race, building), buf);
    } else {
        sprintf(gText, buf);
    }
    return gText;
}

VA(0x00468b82, 0x69)
char* GetBuildingName(FactionType race, BuildingSlotType building) {
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
void GetBuildingCost(FactionType race, BuildingSlotType building, i32* const dest, i32 mageLevel) {
    i32 level;
    if (building == BUILDING_SLOT_NECROMANCER_SHRINE && race == FACTION_NECROMANCER) {
        memcpy(dest, xShrineBuildingCost, KB_BUILDING_RESOURCE_COUNT * sizeof(i32));
    } else if (building >= BUILDING_SLOT_DWELLING_FIRST
               && building <= BUILDING_SLOT_DWELLING_LAST) {
        memcpy(
            dest,
            gDwellingCosts[IDX(race)][IDX(building) - IDX(BUILDING_SLOT_DWELLING_FIRST)],
            KB_BUILDING_RESOURCE_COUNT * sizeof(i32)
        );
    } else if (building == BUILDING_SLOT_MAGE_GUILD) {
        level = mageLevel + 1;
        if (level > KB_MAGE_GUILD_MAX_LEVEL)
            level = KB_MAGE_GUILD_MAX_LEVEL;
        memcpy(dest, gMageBuildingCosts[mageLevel + 1], KB_BUILDING_RESOURCE_COUNT * sizeof(i32));
    } else if (building == BUILDING_SLOT_SPECIAL) {
        memcpy(dest, gSpecialBuildingCosts[IDX(race)], KB_BUILDING_RESOURCE_COUNT * sizeof(i32));
    } else {
        if (building >= BUILDING_SLOT_DISABLED_SECOND)
            return;
        memcpy(
            dest,
            gNeutralBuildingCosts[IDX(building)],
            KB_BUILDING_RESOURCE_COUNT * sizeof(i32)
        );
    }
}

VA(0x00468cdb, 0x15)
char* GetMonsterName(H2_ENUM_PARAM(CreatureType, i32) monster) {
    return gArmyNames[IDX(monster)];
}

VA(0x00468cf0, 0x15)
char* GetMonsterPluralName(H2_ENUM_PARAM(CreatureType, i32) monster) {
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
    if (building != BUILDING_SLOT_CASTLE && !(t->m_buildings & IDX(TOWN_BUILDING_CASTLE)))
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
         && (t->m_buildings & IDX(KB_DWELLING_UPGRADE_FIRST_FLAG)))
        || (building == BUILDING_SLOT_DWELLING_THIRD
            && (t->m_buildings & IDX(KB_DWELLING_UPGRADE_SECOND_FLAG)))
        || (building == BUILDING_SLOT_DWELLING_FOURTH
            && (t->m_buildings & IDX(KB_DWELLING_UPGRADE_THIRD_FLAG)))
        || (building == BUILDING_SLOT_DWELLING_FIFTH
            && (t->m_buildings & IDX(KB_DWELLING_UPGRADE_FOURTH_FLAG)))
        || (building == BUILDING_SLOT_DWELLING_SIXTH
            && ((t->m_buildings & IDX(KB_DWELLING_UPGRADE_FIFTH_FLAG))
                || (t->m_buildings & IDX(KB_DWELLING_UPGRADE_SIXTH_FLAG))))
        || (building == BUILDING_SLOT_UPGRADE_LAST
            && (t->m_buildings & IDX(KB_DWELLING_UPGRADE_SIXTH_FLAG))))
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

VA(0x0046932f, 0x3f2)
MessageDispatchResult EventWindowHandler(struct tag_message& msg) {
    i32 resType;
    i32 resExtra;

    if (!gpSoundManager->MusicPlaying() && gpAdvManager->m_active == true)
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[IDX(gpAdvManager->m_currentTerrain)]
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
                                cLuckInfo[IDX(LUCK_INFO_GOOD)],
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
                                cLuckInfo[IDX(LUCK_INFO_BAD)],
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
                                cMoraleInfo[IDX(MORALE_INFO_GOOD)],
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
                                cMoraleInfo[IDX(MORALE_INFO_BAD)],
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
                                "{\xce\xef\xfb\xf2}\n\n\xce\xef\xfb\xf2 \xef\xee\xe7"
                                "\xe2\xee\xeb\xff\xe5\xf2 \xef\xee\xe2\xfb\xf8\xe0"
                                "\xf2\xfc \xe2\xe0\xf8\xe8\xec \xe3\xe5\xf0\xee\xff"
                                "\xec \xf1\xe2\xee\xe8 \xf3\xf0\xee\xe2\xed\xe8, \xf3"
                                "\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xf2\xfc \xef\xe5"
                                "\xf0\xe2\xe8\xf7\xed\xfb\xe5 \xe8 \xe2\xf2\xee\xf0"
                                "\xe8\xf7\xed\xfb\xe5 \xed\xe0\xe2\xfb\xea\xe8.",
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
                            if (resExtra == IDX(ARTIFACT_SPELL_SCROLL)) {
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
                                "{\xd0\xe5\xf1\xf3\xf0\xf1\xfb}\n\n\xc2 \xc3\xe5\xf0\xee"
                                "\xff\xf5 II \xe5\xf1\xf2\xfc 7 \xf2\xe8\xef\xee\xe2 "
                                "\xf0\xe5\xf1\xf3\xf0\xf1\xee\xe2, \xe8\xf1\xef\xee"
                                "\xeb\xfc\xe7\xf3\xe5\xec\xfb\xf5 \xe4\xeb\xff \xe2"
                                "\xee\xe7\xe2\xe5\xe4\xe5\xed\xe8\xff \xef\xee\xf1"
                                "\xf2\xf0\xee\xe5\xea, \xf3\xeb\xf3\xf7\xf8\xe5\xed"
                                "\xe8\xe9 \xe7\xe0\xec\xea\xee\xe2, \xef\xee\xea\xf3"
                                "\xef\xea\xe8 \xe2\xee\xe8\xed\xee\xe2 \xe8 \xe3\xe5"
                                "\xf0\xee\xe5\xe2. \xd1\xe0\xec\xfb\xe9 \xf0\xe0\xf1"
                                "\xef\xf0\xee\xf1\xf2\xf0\xe0\xed\xe5\xed\xed\xfb\xe9 "
                                "\xf0\xe5\xf1\xf3\xf0\xf1 - \xe7\xee\xeb\xee\xf2\xee, "
                                "\xf2\xf0\xe5\xe1\xf3\xe5\xec \xef\xf0\xe0\xea\xf2"
                                "\xe8\xf7\xe5\xf1\xea\xe8 \xe2\xe5\xe7\xe4\xe5. \xc4"
                                "\xf0\xe5\xe2\xe5\xf1\xe8\xed\xe0 \xe8 \xf0\xf3\xe4"
                                "\xe0 \xe8\xf1\xef\xee\xeb\xfc\xe7\xf3\xfe\xf2\xf1"
                                "\xff \xe4\xeb\xff \xe2\xee\xe7\xe2\xe5\xe4\xe5\xed"
                                "\xe8\xff \xe1\xee\xeb\xfc\xf8\xe8\xed\xf1\xf2\xe2"
                                "\xe0 \xef\xee\xf1\xf2\xf0\xee\xe5\xea. \xd1\xe0\xec"
                                "\xee\xf6\xe2\xe5\xf2\xfb, \xf0\xf2\xf3\xf2\xfc, \xf1"
                                "\xe5\xf0\xe0 \xe8 \xea\xf0\xe8\xf1\xf2\xe0\xeb\xeb"
                                "\xfb - \xf0\xe5\xe4\xea\xe8\xe5 \xec\xe0\xe3\xe8\xf7"
                                "\xe5\xf1\xea\xe8\xe5 \xf0\xe5\xf1\xf3\xf0\xf1\xfb, "
                                "\xed\xf3\xe6\xed\xfb\xe5 \xe4\xeb\xff \xe2\xee\xe7"
                                "\xe2\xe5\xe4\xe5\xed\xe8\xff \xeb\xf3\xf7\xf8\xe8"
                                "\xf5 \xef\xee\xf1\xf2\xf0\xee\xe5\xea \xe8 \xef\xee"
                                "\xea\xf3\xef\xea\xe8 \xf1\xe8\xeb\xfc\xed\xfb\xf5 "
                                "\xe2\xee\xe8\xed\xee\xe2.",
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

VA(0x00469721, 0x13)
MessageDispatchResult TrueFalseDialogHandler(struct tag_message& msg) {
    return EventWindowHandler(msg);
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
                    sprintf(gText, "\xc2\xfb \xe1\xfb\xeb\xe8 \xe8\xf1\xea\xeb\xfe\xf7\xe5\xed\xfb \xe8\xe7 \xe8\xe3\xf0\xfb!!!");
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                } else {
                    sprintf(gText, "%s \xf1\xee\xea\xf0\xf3\xf8\xe5\xed!", cPlayerNames[player]);
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
                            "%s, \xe2\xfb \xef\xee\xf2\xe5\xf0\xff\xeb\xe8 \xe2\xe0\xf8 \xef\xee\xf1\xeb\xe5\xe4\xed\xe8\xe9 \xe3\xee\xf0\xee\xe4. \xc5\xf1\xeb\xe8 \xe2\xfb \xed\xe5 \xe7\xe0\xe2\xee\xfe\xe5\xf2\xe5 \xf1\xe5\xe1\xe5 \xe3\xee\xf0\xee\xe4 \xe2 \xf2\xe5\xf7\xe5\xed\xe8\xe5 \xf1\xeb\xe5\xe4\xf3\xfe\xf9\xe8\xf5 7 \xe4\xed\xe5\xe9, \xe2\xfb \xef\xf0\xee\xe8\xe3\xf0\xe0\xe5\xf2\xe5.",
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
                                "%s, \xe2\xe0\xf8\xe8 \xe3\xe5\xf0\xee\xe8 \xef\xee\xea\xe8\xed\xf3\xeb\xe8 \xe2\xe0\xf1, \xe0 \xe2\xfb \xe1\xfb\xeb\xe8 \xe8\xe7\xe3\xed\xe0\xed\xfb \xf1 \xef\xee\xe7\xee\xf0\xee\xec \xe8\xe7 \xfd\xf2\xe8\xf5 \xe7\xe5\xec\xe5\xeb\xfc.",
                                cPlayerNames[player]
                            );
                        }
                    } else {
                        sprintf(
                            gText,
                            "%s \xe1\xfb\xeb \xe1\xf0\xee\xf8\xe5\xed \xf1\xe2\xee\xe8\xec\xe8 \xe3\xe5\xf0\xee\xff\xec\xe8 \xe8 \xe8\xe7\xe3\xed\xe0\xed \xf1 \xef\xee\xe7\xee\xf0\xee\xec \xe8\xe7 \xfd\xf2\xe8\xf5 \xe7\xe5\xec\xe5\xeb\xfc.",
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
                sprintf(gText, "\xc2\xf0\xe0\xe3 \xf0\xe0\xe7\xe1\xe8\xf2, \xe0 \xe2\xe0\xf8\xe0 \xe0\xf0\xec\xe8\xff \xef\xf0\xe0\xe7\xe4\xed\xf3\xe5\xf2 \xf2\xf0\xe8\xf3\xec\xf4!");
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
                        "\xc2\xfb \xe7\xe0\xf5\xe2\xe0\xf2\xe8\xeb\xe8 %s! \xc2\xfb \xef\xee\xe1\xe5\xe4\xe8\xeb\xe8.",
                        victoryTownData->m_name
                    );
                } else {
                    sprintf(
                        gText,
                        "\xc2\xf0\xe0\xe3 \xe7\xe0\xf5\xe2\xe0\xf2\xe8\xeb %s! \xce\xed\xe8 \xef\xf0\xe0\xe7\xe4\xed\xf3\xfe\xf2 \xf2\xf0\xe8\xf3\xec\xf4.",
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
                sprintf(gText, "%s \xef\xe0\xeb! \xc2\xf1\xe5 \xef\xee\xf2\xe5\xf0\xff\xed\xee.", lossTown->m_name);
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
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
                    winFlag = 1;
                } else {
                    defeated_m = 1;
                }
                if (!showedDialog_o) {
                    showedDialog_o = 1;
                    if (winFlag) {
                        sprintf(
                            gText,
                            "\xc2\xfb \xf1\xee\xe1\xf0\xe0\xeb\xe8 \xe1\xee\xeb\xe5\xe5 %d \xe7\xee\xeb\xee\xf2\xfb\xf5 \xe2 \xf1\xe2\xee\xe5\xe9 \xea\xe0\xe7\xed\xe5. \xc2\xf1\xe5 \xe2\xf0\xe0\xe3\xe8 \xef\xf0\xe5\xea\xeb\xee\xed\xe8\xeb\xe8\xf1\xfc \xef\xe5\xf0\xe5\xe4 \xf1\xe8\xeb\xee\xe9 \xe2\xe0\xf8\xe5\xe3\xee \xf1\xee\xf1\xf2\xee\xff\xed\xe8\xff.",
                            bestGold
                        );
                    } else {
                        sprintf(
                            gText,
                            "\xc2\xf0\xe0\xe3 \xf1\xee\xe1\xf0\xe0\xeb \xe1\xee\xeb\xe5\xe5 %d \xe7\xee\xeb\xee\xf2\xfb\xf5 \xe2 \xf1\xe2\xee\xe5\xe9 \xea\xe0\xe7\xed\xe5. \xc2\xfb \xe4\xee\xeb\xe6\xed\xfb \xef\xf0\xe5\xea\xeb\xee\xed\xe8\xf2\xfc\xf1\xff \xef\xe5\xf0\xe5\xe4 \xf1\xe8\xeb\xee\xe9 \xf2\xe0\xea\xee\xe3\xee \xf1\xee\xf1\xf2\xee\xff\xed\xe8\xff \xe8 \xef\xf0\xe8\xe7\xed\xe0\xf2\xfc \xf1\xe2\xee\xe5 \xef\xee\xf0\xe0\xe6\xe5\xed\xe8\xe5.",
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
                    "%s - \xe2\xf0\xe0\xe6\xe5\xf1\xea\xe8\xe9 \xe3\xe5\xf0\xee\xe9, \xf3 \xe2\xe0\xf1 \xe2 \xef\xeb\xe5\xed\xf3! \xc2\xe0\xf8\xe5 \xe7\xe0\xe4\xe0\xed\xe8\xe5 \xe7\xe0\xe2\xe5\xf0\xf8\xe5\xed\xee.",
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
                sprintf(gText, "%s - \xe2\xe0\xf8 \xe3\xe5\xf0\xee\xe9, \xe1\xfb\xeb \xef\xee\xe2\xe5\xf0\xe6\xe5\xed.  \xc2\xfb \xef\xf0\xee\xe2\xe0\xeb\xe8\xeb\xe8 \xe2\xe0\xf8\xe5 \xe7\xe0\xe4\xe0\xed\xe8\xe5.", lossHero_k->m_name);
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
                sprintf(gText, "\xc2\xfb \xed\xe5 \xf3\xf1\xef\xe5\xeb\xe8 \xe7\xe0\xe2\xe5\xf0\xf8\xe8\xf2\xfc \xe2\xe0\xf8\xe5 \xe7\xe0\xe4\xe0\xed\xe8\xe5 \xe2 \xf1\xf0\xee\xea. \xc2\xf1\xe5 \xef\xee\xf2\xe5\xf0\xff\xed\xee.");
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
                    sprintf(artifactName, "\xcc\xee\xe3\xf3\xf9\xe5\xf1\xf2\xe2\xe5\xed\xed\xfb\xe9 \xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2");
                } else {
                    sprintf(
                        artifactName,
                        gArtifactNames[gpGame->m_mapHeader.victoryConditionValue - 1]
                    );
                }
                if (winFlag) {
                    sprintf(gText, "\xc2\xfb \xed\xe0\xf8\xeb\xe8 %s. \xc2\xfb \xf1\xef\xf0\xe0\xe2\xe8\xeb\xe8\xf1\xfc \xf1 \xe7\xe0\xe4\xe0\xed\xe8\xe5\xec.", artifactName);
                } else {
                    sprintf(
                        gText,
                        "\xc2\xf0\xe0\xe3 \xed\xe0\xf8\xe5\xeb %s. \xc2\xfb \xed\xe5 \xf1\xef\xf0\xe0\xe2\xe8\xeb\xe8\xf1\xfc \xf1\xee \xf1\xe2\xee\xe8\xec \xe7\xe0\xe4\xe0\xed\xe8\xe5\xec.",
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
                    "\xc2\xf1\xe5 \xe3\xee\xf0\xee\xe4\xe0 \xe3\xed\xee\xec\xee\xe2 \xef\xe0\xeb\xe8. \xdd\xf2\xee \xf1\xee\xea\xf0\xf3\xf8\xe8\xf2\xe5\xeb\xfc\xed\xee\xe5 \xef\xee\xf0\xe0\xe6\xe5\xed\xe8\xe5! \xc2\xfb \xef\xf0\xee\xe8\xe3\xf0\xe0\xeb\xe8."
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
            sprintf(gText, "\xc4\xf0\xe0\xea\xee\xed\xe8\xe9 \xe3\xee\xf0\xee\xe4 \xef\xe0\xeb! \xd2\xe5\xef\xe5\xf0\xfc \xe2\xfb \xcf\xee\xe2\xe5\xeb\xe8\xf2\xe5\xeb\xfc \xe4\xf0\xe0\xea\xee\xed\xee\xe2.");
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
                sprintf(gText, "\xd0\xee\xeb\xe0\xed\xe4 \xef\xeb\xe5\xed\xe5\xed! \xc2\xf1\xe5 \xef\xee\xf2\xe5\xf0\xff\xed\xee.");
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
                sprintf(gText, "\xc2\xf0\xe0\xe3 \xf0\xe0\xe7\xe1\xe8\xf2, \xe0 \xe2\xe0\xf8\xe0 \xe0\xf0\xec\xe8\xff \xef\xf0\xe0\xe7\xe4\xed\xf3\xe5\xf2 \xf2\xf0\xe8\xf3\xec\xf4!");
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

        if (carryoverHeroId != END_GAME_NO_PLAYER) {
            for (player = 0; player < CAMPAIGN_ARMY_SLOT_COUNT; player++) {
                gpGame->m_campaignCarryoverCreatureTypes[player] = CREATURE_NONE;
                gpGame->m_campaignCarryoverCreatureCounts[player] = 0;
            }
            for (campaignHeroIndex = 0; campaignHeroIndex < gpGame->m_players[0].m_heroCount;
                 campaignHeroIndex++) {
                if (carryoverHeroId == CAMPAIGN_SWITCH_VICTORY_VALUE
                    || IDX(gpGame->m_heroRecs[gpGame->m_players[0].m_heroIds[campaignHeroIndex]]
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
                "\xcf\xce\xc1\xc5\xc4\xc0_",
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

VA(0x0046aec9, 0x80)
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

VA(0x0046af49, 0x1cf)
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

VA(0x0046b118, 0x593)
void game::ShowMoraleInfo(hero* h, i32 dialogType) {
    i32 mixedUndead4;
    i32 alignment_e;
    ArmyGroupAlignmentResult homogeneous5;
    i32 modifierStart;
    char description7[MORALE_LUCK_DESCRIPTION_SIZE];
    i32 slot8;

    mixedUndead4 = 0;
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
        && (h->GetOccupiedTown()->m_buildings & IDX(TOWN_BUILDING_COLISEUM))) {
        strcat(gText, cMoraleInfo[IDX(INFO_COLISEUM)]);
    }
    if (h->GetOccupiedTown() != NULL
        && (h->GetOccupiedTown()->m_buildings & IDX(TOWN_BUILDING_TAVERN))) {
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
    if (h->HasArtifact(ARTIFACT_MASTHEAD) && HAS(h->m_eventFlags, HERO_EVENT_EMBARKED)) {
        strcat(gText, cMoraleInfo[IDX(MORALE_INFO_MASTHEAD)]);
    }
    if (h->HasArtifact(ARTIFACT_BATTLE_GARB)) {
        strcat(gText, cMoraleInfo[IDX(MORALE_INFO_BATTLE_GARB)]);
    }
    if (modifierStart == static_cast<i32>(strlen(gText))) {
        strcat(gText, cMoraleInfo[IDX(MORALE_INFO_NONE)]);
    }

showDialog:
    NormalDialog(gText, dialogType, -1, -1, -1, 0, -1, 0, -1, 0);
}

VA(0x0046b6ab, 0x33e)
void game::ShowLuckInfo(hero* h, i32 dialogType) {
    char description4[MORALE_LUCK_DESCRIPTION_SIZE];
    i32 luckValue;
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
        && (h->GetOccupiedTown()->m_buildings & IDX(TOWN_BUILDING_RAINBOW)))
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
    if (h->HasArtifact(ARTIFACT_MASTHEAD) && HAS(h->m_eventFlags, HERO_EVENT_EMBARKED))
        strcat(gText, cLuckInfo[IDX(LUCK_INFO_MASTHEAD)]);
    if (HAS(h->m_eventFlags, HERO_EVENT_MERMAID))
        strcat(gText, cLuckInfo[IDX(INFO_MERMAID)]);
    if (h->HasArtifact(ARTIFACT_BATTLE_GARB))
        strcat(gText, cLuckInfo[IDX(LUCK_INFO_BATTLE_GARB)]);
    if (modifierStart == static_cast<i32>(strlen(gText)))
        strcat(gText, cLuckInfo[IDX(LUCK_INFO_NONE)]);

    NormalDialog(gText, dialogType, -1, -1, -1, 0, -1, 0, -1, 0);
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
i32 GetMonType(i32 score, HighScoreType campaign) {
    i32 idx;
    for (idx = IDX(CREATURE_COUNT) - 1; idx >= 0; idx--) {
        if (campaign == HIGH_SCORE_CAMPAIGN || campaign == HIGH_SCORE_EXPANSION_CAMPAIGN) {
            if (score <= giScoreCampaignMon[idx][IDX(MONSTER_SCORE_THRESHOLD)])
                return giScoreCampaignMon[idx][IDX(MONSTER_SCORE_TYPE)];
        } else {
            if (score >= giScoreMon[idx][IDX(MONSTER_SCORE_THRESHOLD)])
                return giScoreMon[idx][IDX(MONSTER_SCORE_TYPE)];
        }
    }
    return giScoreMon[0][IDX(MONSTER_SCORE_TYPE)];
}

VA(0x0046bb0e, 0x405)
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
        sprintf(filename_h, "%sSTANDARD.HS", ".\\DATA\\");
    else
        sprintf(filename_h, "%sCAMPAIGN.HS", ".\\DATA\\");

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
            "\xcf\xee\xe6\xe0\xeb\xf3\xe9\xf1\xf2\xe0, \xe2\xe2\xe5\xe4\xe8\xf2\xe5 \xf1\xe2\xee\xe5 \xe8\xec\xff \xe4\xeb\xff \xf2\xe0\xe1\xeb\xe8\xf6\xfb \xf0\xe5\xea\xee\xf0\xe4\xee\xe2.",
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

VA(0x0046bf13, 0x5e)
void BVResMsg(char* s, H2_ENUM_PARAM(ResourceType, i32) res, i32 qty) {
    giBottomViewOverride = BOTTOM_VIEW_RESOURCE;
    giBottomViewOverrideEndTime = KBTickCount() + BOTTOM_VIEW_RESOURCE_MESSAGE_DURATION;
    giBottomViewResource = res;
    giBottomViewResourceQty = qty;
    strcpy(gcBottomViewText, s);
    gpAdvManager->UpdBottomView(1, 1, 1);
}

VA(0x0046bf71, 0x1e)
void GOut(char* str) {
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
    netWindow_j = new heroWindow(0, BOX_WINDOW_Y, "netbox.bin");
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
                        AddNetBoxLine("[ \xc2\xf5\xee\xe4\xff\xf9\xe0\xff \xe8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xff, \xed\xe5\xee\xe1\xf5\xee\xe4\xe8\xec\xee \xe2\xfb\xe9\xf2\xe8... ]", BOX_DEFAULT_COLOR);
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

VA(0x0046cbcb, 0x96)
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

VA(0x0046cc61, 0x1d9)
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
        MessageBoxA(hwndApp, buf, "\xcd\xe5\xef\xf0\xe5\xe4\xe2\xe8\xe4\xe5\xed\xed\xee\xe5 \xef\xf0\xe5\xf0\xfb\xe2\xe0\xed\xe8\xe5 \xef\xf0\xee\xe3\xf0\xe0\xec\xec\xfb", MB_ICONHAND);
    } else {
        sprintf(buf, "\xcf\xee\xea\xe0!");
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
        "\xce\xf8\xe8\xe1\xea\xe0 \xee\xf2\xea\xf0\xfb\xf2\xe8\xff \xf4\xe0\xe9\xeb\xe0 %s!",
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

VA(0x0046d109, 0x3b2)
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
        sprintf(ratingText, "\xd7\xe8\xf2\xe5\xf0!!!");

    if (highScoreType == HIGH_SCORE_STANDARD) {
        sprintf(
            congratsText,
            "\xcf\xee\xe7\xe4\xf0\xe0\xe2\xeb\xff\xe5\xec!\n\n\xc4\xed\xe5\xe9: %d\n"
            "\xd1\xf7\xe5\xf2: %d\n\xd1\xeb\xee\xe6\xed\xee\xf1\xf2\xfc: %d\n\n"
            "\xce\xf7\xea\xe8: %d\n\n\xd0\xe5\xe9\xf2\xe8\xed\xe3:\n%s\n",
            giCurTurn,
            baseScore,
            gpGame->m_difficultyRating,
            realScore,
            ratingText
        );
    } else if (highScoreType == HIGH_SCORE_EXPANSION_CAMPAIGN) {
        sprintf(
            congratsText,
            "\xcf\xee\xe7\xe4\xf0\xe0\xe2\xeb\xff\xe5\xec!\n\n\xc4\xed\xe5\xe9: "
            "%d\n\n\xd0\xe5\xe9\xf2\xe8\xed\xe3:\n%s\n",
            xCampaign.Days(),
            ratingText
        );
    } else {
        sprintf(
            congratsText,
            "\xcf\xee\xe7\xe4\xf0\xe0\xe2\xeb\xff\xe5\xec!\n\n\xc4\xed\xe5\xe9: "
            "%d\n\n\xd0\xe5\xe9\xf2\xe8\xed\xe3:\n%s\n",
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

VA(0x0046d4bb, 0x79)
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

H2_ENUM_BEGIN(SamplePlaybackConstant)
    SAMPLE_PLAYBACK_CHANNEL_GROUP = 2,
    SAMPLE_DEFAULT_WAIT_TIME = 4000
H2_ENUM_END(SamplePlaybackConstant)

VA(0x0046d534, 0x41)
SAMPLE2 LoadPlaySample(char* name) {
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
        "\xcd\xe5\xe4\xee\xf1\xf2\xe0\xf2\xee\xf7\xed\xee \xef\xe0\xec\xff\xf2\xe8." /* "Недостаточно памяти." */
        ,
        MEMORY_ERROR_REQUEST_SIZE
    );
    ShutDown(gText);
}

VA(0x0046d644, 0x29)
char* GetTownName(i32 i) {
    town* t = GetCastleRec(i);
    return t->m_name;
}

VA(0x0046d66d, 0x2f)
void LoadSystemwideIcons(void) {
    gBuyBuildIcons = gpResourceManager->GetIcon("buybuild.icn");
    gSystemIcons = gpResourceManager->GetIcon("system.icn");
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
            strcpy(gText, "\xc2\xfb \xe4\xe5\xe9\xf1\xf2\xe2\xe8\xf2\xe5\xeb\xfc\xed\xee \xf5\xee\xf2\xe8\xf2\xe5 \xed\xe0\xf7\xe0\xf2\xfc \xf1\xed\xe0\xf7\xe0\xeb\xe0?  (\xdd\xf2\xe0 \xe8\xe3\xf0\xe0 \xe1\xf3\xe4\xe5\xf2 \xef\xee\xf2\xe5\xf0\xff\xed\xe0)");
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
                "\xc2\xfb \xe4\xe5\xe9\xf1\xf2\xe2\xe8\xf2\xe5\xeb\xfc\xed\xee \xf5\xee\xf2\xe8\xf2\xe5 \xe7\xe0\xe3\xf0\xf3\xe7\xe8\xf2\xfc \xed\xee\xe2\xf3\xfe \xe8\xe3\xf0\xf3? (\xdd\xf2\xe0 \xe8\xe3\xf0\xe0 \xe1\xf3\xe4\xe5\xf2 \xef\xee\xf2\xe5\xf0\xff\xed\xe0)"
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

    if (gConfig.gfx[IDX(giCurExe)].showMenu == 0)
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
void EarlyResizeWindow(i32 x, i32 y, i32 w, i32 h) {
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

VA(0x0046eb87, 0xd3)
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
        pe.updateNetworkControl = 0;
        pe.timedOut = 0;
        pe.eliminated = 1;
        pe.hostReported = 0;
        ReceiveRemotePlayerExit(pe);
    }
}

H2_ENUM_BEGIN(PlayerExitLocalConstant)
    PLAYER_EXIT_HOST_NET_POSITION = 0,
    PLAYER_EXIT_FIRST_GUEST_NET_POSITION = 1,
    PLAYER_EXIT_DIRECT_PLAYER_COUNT = 2,
    PLAYER_EXIT_MESSAGE_LENGTH = 500,
    PLAYER_EXIT_TRANSMIT_DELAY = 500,
    PLAYER_EXIT_MESSAGE_TIME = 5000,
    PLAYER_EXIT_HEARTBEAT_DISABLED = 0x0bebc1ff
H2_ENUM_END(PlayerExitLocalConstant)

VA(0x0046ece2, 0x8e)
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

VA(0x0046ed70, 0x4f)
void DropDownToOnePlayer(void) {
    RemoteCleanup();
    giNumHumanPlayers = 1;
    for (i32 i = 0; i < REMOTE_PLAYER_COUNT; i++)
        if (i != giThisNetPos)
            gbHumanPlayer[i] = 0;
    ComputeAdvNetControl();
}

VA(0x0046edbf, 0x356)
void ReceiveHostReportsPlayerExit(i32 hostNetPosition, SPlayerExit exitInfo, i32 forwardedReport) {
    i32 showExitMessage_i;
    char playerExitMessage_k[PLAYER_EXIT_MESSAGE_LENGTH];
    i32 netPosition;

    showExitMessage_i = 0;
    if (!forwardedReport) {
        if (exitInfo.eliminated) {
            if (exitInfo.netPosition == giThisNetPos) {
                RemoteCleanup();
                sprintf(gText, "\xc2\xfb \xe1\xfb\xeb\xe8 \xe8\xf1\xea\xeb\xfe\xf7\xe5\xed\xfb \xe8\xe7 \xe8\xe3\xf0\xfb!!!");
                NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                gbGameOver = true;
                giEndSequence = 0;
                return;
            }

            sprintf(gText, "%s \xf1\xee\xea\xf0\xf3\xf8\xe5\xed!", gsNetPlayerInfo[exitInfo.netPosition].name);
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
            gpGame->SaveGame("\xc8\xe3\xf0\xee\xea \xc2\xfb\xf8\xe5\xeb", 1, 0);
            sprintf(
                gText,
                "%s \xef\xee\xea\xe8\xe4\xe0\xe5\xf2 \xe8\xe3\xf0\xf3, \xe0 %s \xf0\xe5\xf8\xe0\xe5\xf2 \xef\xf0\xe5\xea\xf0\xe0\xf2\xe8\xf2\xfc \xe2\xf1\xfe \xe8\xe3\xf0\xf3. \xc8\xe3\xf0\xe0 \xe1\xfb\xeb\xe0 \xf1\xee\xf5\xf0\xe0\xed\xe5\xed\xe0 \xef\xee\xe4 \xed\xe0\xe7\xe2\xe0\xed\xe8\xe5\xec '\xc8\xc3\xd0\xce\xca \xc2\xdb\xd8\xc5\xcb'. \xc6\xe5\xeb\xe0\xe5\xf2\xe5 \xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc \xe8\xe3\xf0\xf3 \xf1\xe0\xec\xee\xf1\xf2\xee\xff\xf2\xe5\xeb\xfc\xed\xee, \xe3\xe4\xe5 \xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0 \xe7\xe0\xe9\xec\xe5\xf2 \xec\xe5\xf1\xf2\xe0 \xe2\xfb\xe1\xfb\xe2\xf8\xe8\xf5 \xe8\xe3\xf0\xee\xea\xee\xe2?",
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
                    "\xc8\xe3\xf0\xee\xea-\xf1\xe5\xf0\xe2\xe5\xf0 %s \xf1\xee\xee\xe1\xf9\xe0\xe5\xf2, \xf7\xf2\xee \xe8\xe3\xf0\xee\xea %s \xef\xee\xea\xe8\xed\xf3\xeb \xe8\xe3\xf0\xf3. \xc8\xe3\xf0\xe0 \xe1\xf3\xe4\xe5\xf2 \xef\xf0\xee\xe4\xee\xeb\xe6\xe5\xed\xe0, \xe0 \xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0 \xe7\xe0\xe9\xec\xe5\xf2 \xec\xe5\xf1\xf2\xee %s.",
                    gsNetPlayerInfo[hostNetPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name
                );
            } else {
                sprintf(
                    playerExitMessage_k,
                    "\xc8\xe3\xf0\xee\xea-\xf1\xe5\xf0\xe2\xe5\xf0 %s \xf1\xee\xee\xe1\xf9\xe0\xe5\xf2, \xf7\xf2\xee \xe8\xe3\xf0\xee\xea %s \xef\xee\xea\xe8\xed\xf3\xeb \xe8\xe3\xf0\xf3. \xc8\xe3\xf0\xe0 \xe1\xf3\xe4\xe5\xf2 \xef\xf0\xee\xe4\xee\xeb\xe6\xe5\xed\xe0, \xe0 \xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0 \xe7\xe0\xe9\xec\xe5\xf2 \xec\xe5\xf1\xf2\xee %s.",
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

VA(0x0046f115, 0x2c9)
void ReceiveRemotePlayerExit(SPlayerExit exitInfo) {
    i32 localPlayerLost_e;
    i32 sendReturn;
    i32 unusedPacketResult_g;
    i32 recipient;

    localPlayerLost_e = 0;
    lLastHeartbeatReceive[exitInfo.netPosition] = PLAYER_EXIT_HEARTBEAT_DISABLED;
    gpGame->SaveGame("\xc8\xe3\xf0\xee\xea \xc2\xfb\xf8\xe5\xeb", 1, 0);

    if (exitInfo.eliminated) {
        exitInfo.continueGame = 1;
        if (exitInfo.netPosition == giThisNetPos) {
            localPlayerLost_e = 1;
            goto exitInfoProcessed;
        }
        sprintf(gText, "%s \xf1\xee\xea\xf0\xf3\xf8\xe5\xed!", gsNetPlayerInfo[exitInfo.netPosition].name);
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
                "%s \xe2\xfb\xf8\xe5\xeb \xe8\xe7 \xe8\xe3\xf0\xfb. \xc4\xe0\xed\xed\xe0\xff \xe8\xe3\xf0\xe0 \xf1\xee\xf5\xf0\xe0\xed\xe5\xed\xe0 \xef\xee\xe4 \xed\xe0\xe7\xe2\xe0\xed\xe8\xe5\xec '\xc8\xc3\xd0\xce\xca \xc2\xdb\xd8\xc5\xcb'. \xc6\xe5\xeb\xe0\xe5\xf2\xe5 \xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc \xe8\xe3\xf0\xf3, \xf1 \xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xee\xec, \xe7\xe0\xed\xff\xe2\xf8\xe8\xec \xec\xe5\xf1\xf2\xee %s?",
                gsNetPlayerInfo[exitInfo.netPosition].name,
                gsNetPlayerInfo[exitInfo.netPosition].name
            );
        } else {
            sprintf(
                gText,
                "%s \xef\xee\xea\xe8\xe4\xe0\xe5\xf2 \xe8\xe3\xf0\xf3. \xc4\xe0\xed\xed\xe0\xff \xe8\xe3\xf0\xe0 \xf1\xee\xf5\xf0\xe0\xed\xe5\xed\xe0 \xef\xee\xe4 \xed\xe0\xe7\xe2\xe0\xed\xe8\xe5\xec '\xc8\xc3\xd0\xce\xca \xc2\xdb\xd8\xc5\xcb'. \xc6\xe5\xeb\xe0\xe5\xf2\xe5 \xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc \xe8\xe3\xf0\xf3, \xf1 \xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xee\xec, \xe7\xe0\xed\xff\xe2\xf8\xe8\xec \xec\xe5\xf1\xf2\xee %s?",
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
        sprintf(gText, "\xc2\xfb \xe1\xfb\xeb\xe8 \xe8\xf1\xea\xeb\xfe\xf7\xe5\xed\xfb \xe8\xe7 \xe8\xe3\xf0\xfb!!!");
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

VA(0x0046f4ca, 0x88)
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

    sprintf(iconFile_a, "evntwin%d.bin", windowRows_b);
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
                strcpy(resourceText_p[resourceSlot], "");
            } else if (resourceValue_c[resourceSlot] < -NORMAL_DIALOG_DAILY_RESOURCE_OFFSET) {
                sprintf(
                    resourceText_p[resourceSlot],
                    "%d",
                    resourceValue_c[resourceSlot] + NORMAL_DIALOG_DAILY_RESOURCE_OFFSET
                );
            } else {
                sprintf(
                    resourceText_p[resourceSlot],
                    "%d/\xe4\xe5\xed\xfc",
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
            sprintf(resourceText_p[resourceSlot], "%s", "");
            strcpy(iconFile_a, "brcrest.icn");
            resourceFrame_n = resourceValue_c[resourceSlot];
        } else if (resourceType_a[resourceSlot] == NORMAL_DIALOG_PRIMARY_SKILL) {
            sprintf(resourceText_p[resourceSlot], "%s", "");
            strcpy(iconFile_a, "primskil.icn");
            resourceFrame_n = NORMAL_DIALOG_PRIMARY_BACKGROUND_FRAME;
        } else if (resourceType_a[resourceSlot] == NORMAL_DIALOG_MONSTER) {
            sprintf(resourceText_p[resourceSlot], "%s", "");
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
            sprintf(resourceText_p[resourceSlot], "%s", "");
            sprintf(iconFile_a, "surrendr.icn");
            resourceFrame_n = NORMAL_DIALOG_HERO_BACKGROUND_FRAME;
        } else if (resourceType_a[resourceSlot] == NORMAL_DIALOG_EXPERIENCE
                   || resourceType_a[resourceSlot] == NORMAL_DIALOG_MORALE_BONUS
                   || resourceType_a[resourceSlot] == NORMAL_DIALOG_MORALE_PENALTY
                   || resourceType_a[resourceSlot] == NORMAL_DIALOG_LUCK_BONUS
                   || resourceType_a[resourceSlot] == NORMAL_DIALOG_LUCK_PENALTY) {
            strcpy(resourceText_p[resourceSlot], "");
            strcpy(iconFile_a, "expmrl.icn");
            resourceFrame_n = resourceType_a[resourceSlot] - NORMAL_DIALOG_EXPMRL_FIRST;
            if (resourceType_a[resourceSlot] == NORMAL_DIALOG_EXPMRL_LAST
                && resourceValue_c[resourceSlot] != NORMAL_DIALOG_NO_VALUE) {
                sprintf(resourceText_p[resourceSlot], "%d", resourceValue_c[resourceSlot]);
            }
        } else {
            strcpy(resourceText_p[resourceSlot], "");
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

            sprintf(gText, "monh%04d.icn", resourceValue_c[resourceSlot]);
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
            sprintf(iconFile_a, "port%04d.icn", resourceValue_c[resourceSlot]);
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
        strcpy(orText, "\xe8\xeb\xe8");
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
    gpMouseManager->SetPointer("advmice.mse", 0, MOUSE_AUTO_CURSOR_TYPE);

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
    gpMouseManager->SetPointer("", savedPointerFrame, savedPointerType_o);
    giResType1 = savedFirstResourceType_k;
    giResExtra1 = savedFirstResourceValue;
    giResType2 = savedSecondResourceType_m;
    giResExtra2 = savedSecondResourceValue_n;
    pNormalDialogWindow = savedNormalDialogWindow;
}

VA(0x00470d22, 0x68)
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

VA(0x00470dd0, 0x19)
game::~game() {}

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
u32l glMilliCounter = 0;
DATA(0x004f8bac) u8 gMapColors[RADAR_MAP_COLOR_COUNT] = {77, 98, 13, 104, 32, 118, 54, 206, 41, 0, 0, 0};
u8 gObjectColors[RADAR_OBJECT_COLOR_COUNT] =
    {16, 48, 98, 160, 126, 74, 110, 179, 100, 218, 12, 12, 12, 12, 12, 12};
DATA(0x004f8bc8) u8 gOwnerColors[RADAR_OWNER_COLOR_COUNT] = {73, 105, 190, 114, 205, 138, 10, 0};
DATA(0x004f8bd0) char* gTilesetFiles[IDX(TILESET_COUNT)] = {
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
DATA(0x004f9b14) i32 giScreenScroll = 1;
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
DATA(0x004f9b3c) char* cEvilTranslate[KB_INTERFACE_TYPE_COUNT][KB_INTERFACE_VARIANT_COUNT] = {
    {"advbord.icn", "advborde.icn"},  {"heroextg.icn", "heroexte.icn"},
    {"buybuild.icn", "buybuile.icn"}, {"advbtns.icn", "advebtns.icn"},
    {"herologo.icn", "herologe.icn"}, {"sunmoon.icn", "sunmoone.icn"},
    {"stonback.icn", "stonbake.icn"}, {"scroll.icn", "scrolle.icn"},
    {"locators.icn", "locatore.icn"}, {"system.icn", "systeme.icn"},
    {"CPANBKG.ICN", "CPANBKGE.ICN"},  {"CPANEL.ICN", "CPANELE.ICN"},
    {"APANBKG.ICN", "APANBKGE.ICN"},  {"APANEL.ICN", "APANELE.ICN"},
    {"VIEWWRLD.ICN", "EVIWWRLD.ICN"}, {"VIEWRSRC.ICN", "EVIWRSRC.ICN"},
    {"VIEWRTFX.ICN", "EVIWRTFX.ICN"}, {"VIEWTWNS.ICN", "EVIWTWNS.ICN"},
    {"VIEWHROS.ICN", "EVIWHROS.ICN"}, {"VIEW_ALL.ICN", "EVIW_ALL.ICN"},
    {"VIEWMINE.ICN", "EVIWMINE.ICN"}, {"VIEWDDOR.ICN", "EVIWDDOR.ICN"},
    {"VIEWPUZL.ICN", "EVIWPUZL.ICN"}, {"LGNDXTRA.ICN", "LGNDXTRE.ICN"},
    {"SPANBKG.ICN", "SPANBKGE.ICN"},  {"SPANBTN.ICN", "SPANBTNE.ICN"},
    {"CSPANBKG.ICN", "CSPANBKE.ICN"}, {"CSPANBTN.ICN", "CSPANBTE.ICN"},
    {"TRADPOST.ICN", "TRADPOSE.ICN"}, {"VIEWARMY.ICN", "VIEWARME.ICN"},
    {"WINLOSE.ICN", "WINLOSEE.ICN"},  {"WINCMBTB.ICN", "WINCMBBE.ICN"},
    {"SURRENDR.ICN", "SURRENDE.ICN"}, {"SURDRBKG.ICN", "SURDRBKE.ICN"},
    {"VGENBKG.ICN", "VGENBKGE.ICN"},  {"campbkgg.ICN", "campbkge.ICN"},
    {"campxtrg.ICN", "campxtre.ICN"}
};
char gcAnimPath[GLOBAL_AGGREGATE_PATH_SIZE] = "\\HEROES2\\ANIM\\";
DATA(0x004f9dc4) char gcGamePath[GLOBAL_GAME_PATH_SIZE] = ".\\GAMES\\";
DATA(0x004f9dd8) char gcMapPath[GLOBAL_MAP_PATH_SIZE] = ".\\MAPS\\";
DATA(0x004f9dec) char gcMusicPath[GLOBAL_AGGREGATE_PATH_SIZE] = "\\TRACKS2\\";
DATA(0x00526600) b32 gbPutzingWithMouseCtr = false;
b32 gbDontTryRedbook = false;
b32 gbDontTryMIDI = false;
b32 gbDontTryDigital = false;
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
b32 gbShowAllMaps = false;
DATA(0x004f9f5c) char* gCombatFxNames[KB_COMBAT_FX_COUNT] = {
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
DATA(0x004fad30) b8 gbArrow[NORMAL_DIRECTION_COUNT][NORMAL_DIRECTION_COUNT] = {
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
i32 glBottomRefresh = 0;
b32 gbBothMachinesWin95 = false;
DATA(0x00526660) b32 gbGotFirstHeartbeat = false;
DATA(0x00526664) HMENU hmnuDflt = NULL;
DATA(0x00526668) HMENU hmnuCmbt = NULL;
DATA(0x0052666c) HMENU hmnuAdv = NULL;
DATA(0x00526670) HMENU hmnuTown = NULL;
DATA(0x004fb2ac) char* cMonFilename[IDX(CREATURE_COUNT)] = {
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
DATA(0x004fb950) char* cArmyFrameFileNames[IDX(CREATURE_COUNT)] = {
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
DATA(0x004fbaa4) char* cCombatBkgNames[KB_COMBAT_BACKGROUND_COUNT] = {"CBKGWATR.icn", "",
                                                     "CBKGGRTR.icn", "CBKGGRMT.icn",
                                                     "CBKGSNTR.icn", "CBKGSNMT.icn",
                                                     "CBKGSWMP.icn", "",
                                                     "CBKGLAVA.icn", "",
                                                     "CBKGDSRT.icn", "",
                                                     "CBKGDITR.icn", "CBKGDIMT.icn",
                                                     "CBKGCRCK.icn", "",
                                                     "CBKGBEAC.icn", "",
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
DATA(0x004fbee0) char* gTownPrefixNames[IDX(FACTION_COUNT)] = {"twnk", "twnb", "twns", "twnw", "twnz", "twnn"};
DATA(0x004fbef8) char* gTownObjNames[KB_TOWN_OBJECT_NAME_COUNT] = {
    "mage", "thie", "tvrn", "dock", "well", "tent", "cstl", "stat", "ltur", "rtur", "mark",
    "wel2", "moat", "spec", "boat", "capt", "ext0", "ext1", "ext2", "dw_0", "dw_1", "dw_2",
    "dw_3", "dw_4", "dw_5", "up_1", "up_2", "up_3", "up_4", "up_5", "up5b", "ext3"
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
DATA(0x004fcd7c) char* cHeroTypeShortName[IDX(FACTION_COUNT)] = {"kngt", "barb", "sorc", "wrlk", "wzrd", "necr"};
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
        {{{CAMPAIGN_CHOICE_RESOURCE, IDX(RES_GOLD), CHOICE_GOLD_BONUS},
          {CAMPAIGN_CHOICE_ARTIFACT, IDX(ARTIFACT_THUNDER_MACE), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, IDX(ARTIFACT_ARMORED_GAUNTLETS), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_WIZARD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_SORCERESS), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_KNIGHT), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_WIZARD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_SORCERESS), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_KNIGHT), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_WIZARD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_SORCERESS), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_KNIGHT), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_WIZARD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_SORCERESS), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_KNIGHT), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_SPELL, IDX(SPELL_MIRROR_IMAGE), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_SPELL, IDX(SPELL_SUMMON_EARTH_ELEMENTAL), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_SPELL, IDX(SPELL_RESURRECT), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ARTIFACT, IDX(ARTIFACT_BLACK_PEARL), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, IDX(ARTIFACT_DRAGON_SWORD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, IDX(ARTIFACT_DIVINE_BREASTPLATE), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_WIZARD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_SORCERESS), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_KNIGHT), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_RESOURCE, IDX(RES_CRYSTAL), CHOICE_RESOURCE_BONUS},
          {CAMPAIGN_CHOICE_RESOURCE, IDX(RES_GEMS), CHOICE_RESOURCE_BONUS},
          {CAMPAIGN_CHOICE_RESOURCE, IDX(RES_MERCURY), CHOICE_RESOURCE_BONUS}},
         {{CAMPAIGN_CHOICE_ARTIFACT, IDX(ARTIFACT_TAX_LIEN), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, IDX(ARTIFACT_HIDEOUS_MASK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, IDX(ARTIFACT_FIZBIN_OF_MISFORTUNE), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_NONE, CHOICE_VALUE_NONE, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_NONE, CHOICE_VALUE_NONE, CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_NONE, CHOICE_VALUE_NONE, CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_WIZARD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_SORCERESS), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_KNIGHT), CHOICE_NO_AMOUNT}}},
        {{{CAMPAIGN_CHOICE_RESOURCE, IDX(RES_GOLD), CHOICE_GOLD_BONUS},
          {CAMPAIGN_CHOICE_ARTIFACT, IDX(ARTIFACT_MAGE_RING), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, IDX(ARTIFACT_MINOR_SCROLL), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_BARBARIAN), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_RESOURCE, IDX(RES_GOLD), CHOICE_GOLD_BONUS},
          {CAMPAIGN_CHOICE_SPELL, IDX(SPELL_MASS_CURSE), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, IDX(ARTIFACT_DEFENDER_HELM), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_BARBARIAN), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_BARBARIAN), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_BARBARIAN), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_BARBARIAN), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_SECONDARY_SKILL, IDX(HERO_SKILL_LOGISTICS), CHOICE_BASIC_SKILL},
          {CAMPAIGN_CHOICE_ARTIFACT, IDX(ARTIFACT_POWER_AXE), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, IDX(ARTIFACT_WHITE_PEARL), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_BARBARIAN), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ARTIFACT, IDX(ARTIFACT_BLACK_PEARL), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, IDX(ARTIFACT_DRAGON_SWORD), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, IDX(ARTIFACT_DIVINE_BREASTPLATE), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ARTIFACT, IDX(ARTIFACT_TAX_LIEN), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, IDX(ARTIFACT_HIDEOUS_MASK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ARTIFACT, IDX(ARTIFACT_FIZBIN_OF_MISFORTUNE), CHOICE_NO_AMOUNT}},
         {{CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_NECROMANCER), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_WARLOCK), CHOICE_NO_AMOUNT},
          {CAMPAIGN_CHOICE_ALIGNMENT, IDX(FACTION_BARBARIAN), CHOICE_NO_AMOUNT}}}
};
DATA(0x00526688) char* congratsText = NULL;
DATA(0x004fd7ac) char* gArtifactNames[IDX(ARTIFACT_COUNT)] = {
    DATA_COMPGEN(0x00500820, gArtifactNamesUltimateBookOfKnowledge, "\xca\xed\xe8\xe3\xe0\x20\xe2\xf1\xe5\xe7\xed\xe0\xed\xe8\xff" /* "Книга всезнания" */),
    DATA_COMPGEN(0x00500830, gArtifactNamesUltimateSwordOfDominion, "\xcc\xe5\xf7\x20\xe2\xeb\xe0\xf1\xf2\xe8" /* "Меч власти" */),
    DATA_COMPGEN(0x0050083c, gArtifactNamesUltimateCloakOfProtection, "\xc7\xe0\xf9\xe8\xf2\xed\xe0\xff\x20\xed\xe0\xea\xe8\xe4\xea\xe0" /* "Защитная накидка" */),
    DATA_COMPGEN(0x00500850, gArtifactNamesUltimateWandOfMagic, "\xc6\xe5\xe7\xeb\x20\xec\xe0\xe3\xe8\xe8" /* "Жезл магии" */),
    DATA_COMPGEN(0x0050085c, gArtifactNamesUltimateShield, "\xc2\xf1\xe5\xec\xee\xe3\xf3\xf9\xe8\xe9\x20\xf9\xe8\xf2" /* "Всемогущий щит" */),
    DATA_COMPGEN(0x0050086c, gArtifactNamesUltimateStaff, "\xc2\xf1\xe5\xec\xee\xe3\xf3\xf9\xe8\xe9\x20\xef\xee\xf1\xee\xf5" /* "Всемогущий посох" */),
    DATA_COMPGEN(0x00500880, gArtifactNamesUltimateCrown, "\xca\xee\xf0\xee\xed\xe0\x20\xe2\xf1\xe5\xe2\xeb\xe0\xf1\xf2\xe8\xff" /* "Корона всевластия" */),
    DATA_COMPGEN(0x00500894, gArtifactNamesGoldenGoose, "\xc7\xee\xeb\xee\xf2\xee\xe9\x20\xe3\xf3\xf1\xfc" /* "Золотой гусь" */),
    DATA_COMPGEN(0x005008a4, gArtifactNamesArcaneNecklaceOfMagic, "\xce\xe6\xe5\xf0\xe5\xeb\xfc\xe5\x20\xf2\xe0\xe9\xed\xee\xe9\x20\xec\xe0\xe3\xe8\xe8" /* "Ожерелье тайной магии" */),
    DATA_COMPGEN(0x005008bc, gArtifactNamesCasterSBraceletOfMagic, "\xcc\xe0\xe3\xe8\xf7\xe5\xf1\xea\xe8\xe9\x20\xe1\xf0\xe0\xf1\xeb\xe5\xf2" /* "Магический браслет" */),
    DATA_COMPGEN(0x005008d0, gArtifactNamesMageSRingOfPower, "\xca\xee\xeb\xfc\xf6\xee\x20\xec\xe0\xe3\xe0" /* "Кольцо мага" */),
    DATA_COMPGEN(0x005008dc, gArtifactNamesWitchSBroachOfMagic, "\xc1\xf0\xee\xf8\xfc\x20\xe2\xe5\xe4\xfc\xec\xfb" /* "Брошь ведьмы" */),
    DATA_COMPGEN(0x005008ec, gArtifactNamesMedalOfValor, "\xcc\xe5\xe4\xe0\xeb\xfc\x20\xee\xf2\xe2\xe0\xe3\xe8" /* "Медаль отваги" */),
    DATA_COMPGEN(0x005008fc, gArtifactNamesMedalOfCourage, "\xcc\xe5\xe4\xe0\xeb\xfc\x20\xec\xf3\xe6\xe5\xf1\xf2\xe2\xe0" /* "Медаль мужества" */),
    DATA_COMPGEN(0x0050090c, gArtifactNamesMedalOfHonor, "\xcc\xe5\xe4\xe0\xeb\xfc\x20\xe4\xee\xe1\xeb\xe5\xf1\xf2\xe8" /* "Медаль доблести" */),
    DATA_COMPGEN(0x0050091c, gArtifactNamesMedalOfDistinction, "\xcc\xe5\xe4\xe0\xeb\xfc\x20\xef\xee\xf7\xe5\xf2\xe0" /* "Медаль почета" */),
    DATA_COMPGEN(0x0050092c, gArtifactNamesFizbinOfMisfortune, "\xd1\xe8\xec\xe2\xee\xeb\x20\xed\xe5\xf3\xe4\xe0\xf7\xe8" /* "Символ неудачи" */),
    DATA_COMPGEN(0x0050093c, gArtifactNamesThunderMaceOfDominion, "\xc3\xf0\xee\xec\xee\xe2\xe0\xff\x20\xef\xe0\xeb\xe8\xf6\xe0" /* "Громовая палица" */),
    DATA_COMPGEN(0x0050094c, gArtifactNamesArmoredGauntletsOfProtection, "\xc7\xe0\xf9\xe8\xf2\xed\xe0\xff\x20\xef\xe5\xf0\xf7\xe0\xf2\xea\xe0" /* "Защитная перчатка" */),
    DATA_COMPGEN(0x00500960, gArtifactNamesDefenderHelmOfProtection, "\xd8\xeb\xe5\xec\x20\xe7\xe0\xf9\xe8\xf2\xed\xe8\xea\xe0" /* "Шлем защитника" */),
    DATA_COMPGEN(0x00500970, gArtifactNamesGiantFlailOfDominion, "\xc3\xe8\xe3\xe0\xed\xf2\xf1\xea\xe8\xe9\x20\xf6\xe5\xef" /* "Гигантский цеп" */),
    DATA_COMPGEN(0x00500980, gArtifactNamesBallistaOfQuickness, "\xc1\xe0\xeb\xeb\xe8\xf1\xf2\xe0" /* "Баллиста" */),
    DATA_COMPGEN(0x0050098c, gArtifactNamesStealthShieldOfProtection, "\xcd\xe5\xe7\xf0\xe8\xec\xfb\xe9\x20\xf9\xe8\xf2" /* "Незримый щит" */),
    DATA_COMPGEN(0x0050099c, gArtifactNamesDragonSwordOfDominion, "\xc4\xf0\xe0\xea\xee\xed\xe8\xe9\x20\xec\xe5\xf7" /* "Драконий меч" */),
    DATA_COMPGEN(0x005009ac, gArtifactNamesPowerAxeOfDominion, "\xd2\xee\xef\xee\xf0\x20\xe2\xeb\xe0\xf1\xf2\xe8" /* "Топор власти" */),
    DATA_COMPGEN(0x005009bc, gArtifactNamesDivineBreastplateOfProtection, "\xc1\xee\xe6\xe5\xf1\xf2\xe2\xe5\xed\xed\xfb\xe9\x20\xe4\xee\xf1\xef\xe5\xf5" /* "Божественный доспех" */),
    DATA_COMPGEN(0x005009d0, gArtifactNamesMinorScrollOfKnowledge, "\xcc\xe0\xeb\xfb\xe9\x20\xf1\xe2\xe8\xf2\xee\xea\x20\xe7\xed\xe0\xed\xe8\xff" /* "Малый свиток знания" */),
    DATA_COMPGEN(0x005009e4, gArtifactNamesMajorScrollOfKnowledge, "\xc1\xee\xeb\xfc\xf8\xee\xe9\x20\xf1\xe2\xe8\xf2\xee\xea\x20\xe7\xed\xe0\xed\xe8\xff" /* "Большой свиток знания" */),
    DATA_COMPGEN(0x005009fc, gArtifactNamesSuperiorScrollOfKnowledge, "\xcc\xee\xe3\xf3\xf9\xe5\xf1\xf2\xe2\xe5\xed\xed\xfb\xe9\x20\xf1\xe2\xe8\xf2\xee\xea\x20\xe7\xed\xe0\xed\xe8\xff" /* "Могущественный свиток знания" */),
    DATA_COMPGEN(0x00500a1c, gArtifactNamesForemostScrollOfKnowledge, "\xd1\xe2\xe8\xf2\xee\xea\x20\xe2\xfb\xf1\xf8\xe5\xe3\xee\x20\xe7\xed\xe0\xed\xe8\xff" /* "Свиток высшего знания" */),
    DATA_COMPGEN(0x00500a34, gArtifactNamesEndlessSackOfGold, "\xc1\xe5\xe7\xe4\xee\xed\xed\xfb\xe9\x20\xec\xe5\xf8\xee\xea" /* "Бездонный мешок" */),
    DATA_COMPGEN(0x00500a44, gArtifactNamesEndlessBagOfGold, "\xc1\xe5\xe7\xe4\xee\xed\xed\xe0\xff\x20\xf1\xf3\xec\xe0" /* "Бездонная сума" */),
    DATA_COMPGEN(0x00500a54, gArtifactNamesEndlessPurseOfGold, "\xc1\xe5\xe7\xe4\xee\xed\xed\xfb\xe9\x20\xea\xee\xf8\xe5\xeb\xfc" /* "Бездонный кошель" */),
    DATA_COMPGEN(0x00500a68, gArtifactNamesNomadBootsOfMobility, "\xc1\xe0\xf8\xec\xe0\xea\xe8\x20\xea\xee\xf7\xe5\xe2\xed\xe8\xea\xe0" /* "Башмаки кочевника" */),
    DATA_COMPGEN(0x00500a7c, gArtifactNamesTravelerSBootsOfMobility, "\xc1\xe0\xf8\xec\xe0\xea\xe8\x20\xef\xf3\xf2\xed\xe8\xea\xe0" /* "Башмаки путника" */),
    DATA_COMPGEN(0x00500a8c, gArtifactNamesLuckyRabbitSFoot, "\xcb\xe0\xef\xea\xe0\x20\xea\xf0\xee\xeb\xe8\xea\xe0" /* "Лапка кролика" */),
    DATA_COMPGEN(0x00500a9c, gArtifactNamesGoldenHorseshoe, "\xc7\xee\xeb\xee\xf2\xe0\xff\x20\xef\xee\xe4\xea\xee\xe2\xe0" /* "Золотая подкова" */),
    DATA_COMPGEN(0x00500aac, gArtifactNamesGamblerSLuckyCoin, "\xd1\xf7\xe0\xf1\xf2\xeb\xe8\xe2\xe0\xff\x20\xec\xee\xed\xe5\xf2\xe0" /* "Счастливая монета" */),
    DATA_COMPGEN(0x00500ac0, gArtifactNamesFourLeafClover, "\xca\xeb\xe5\xe2\xe5\xf0" /* "Клевер" */),
    DATA_COMPGEN(0x00500ac8, gArtifactNamesTrueCompassOfMobility, "\xca\xee\xec\xef\xe0\xf1" /* "Компас" */),
    DATA_COMPGEN(0x00500ad0, gArtifactNamesSailorSAstrolabeOfMobility, "\xc0\xf1\xf2\xf0\xee\xeb\xff\xe1\xe8\xff" /* "Астролябия" */),
    DATA_COMPGEN(0x00500adc, gArtifactNamesEvilEye, "\xc4\xf3\xf0\xed\xee\xe9\x20\xe3\xeb\xe0\xe7" /* "Дурной глаз" */),
    DATA_COMPGEN(0x00500ae8, gArtifactNamesEnchantedHourglass, "\xc7\xe0\xf7\xe0\xf0\xee\xe2\xe0\xed\xed\xfb\xe5\x20\xf7\xe0\xf1\xfb" /* "Зачарованные часы" */),
    DATA_COMPGEN(0x00500afc, gArtifactNamesGoldWatch, "\xc7\xee\xeb\xee\xf2\xfb\xe5\x20\xf7\xe0\xf1\xfb" /* "Золотые часы" */),
    DATA_COMPGEN(0x00500b0c, gArtifactNamesSkullcap, "\xd8\xe0\xef\xee\xf7\xea\xe0" /* "Шапочка" */),
    DATA_COMPGEN(0x00500b14, gArtifactNamesIceCloak, "\xcb\xe5\xe4\xff\xed\xe0\xff\x20\xed\xe0\xea\xe8\xe4\xea\xe0" /* "Ледяная накидка" */),
    DATA_COMPGEN(0x00500b24, gArtifactNamesFireCloak, "\xce\xe3\xed\xe5\xed\xed\xe0\xff\x20\xed\xe0\xea\xe8\xe4\xea\xe0" /* "Огненная накидка" */),
    DATA_COMPGEN(0x00500b38, gArtifactNamesLightningHelm, "\xc3\xf0\xee\xec\xee\xe2\xee\xe9\x20\xf8\xeb\xe5\xec" /* "Громовой шлем" */),
    DATA_COMPGEN(0x00500b48, gArtifactNamesEvercoldIcicle, "\xcd\xe5\xf2\xe0\xfe\xf9\xe8\xe9\x20\xeb\xe5\xe4" /* "Нетающий лед" */),
    DATA_COMPGEN(0x00500b58, gArtifactNamesEverhotLavaRock, "\xc3\xee\xf0\xff\xf7\xe8\xe9\x20\xea\xe0\xec\xe5\xed\xfc" /* "Горячий камень" */),
    DATA_COMPGEN(0x00500b68, gArtifactNamesLightningRod, "\xc6\xe5\xe7\xeb\x20\xec\xee\xeb\xed\xe8\xe9" /* "Жезл молний" */),
    DATA_COMPGEN(0x00500b74, gArtifactNamesSnakeRing, "\xca\xee\xeb\xfc\xf6\xee\x20\xe7\xec\xe5\xe8" /* "Кольцо змеи" */),
    DATA_COMPGEN(0x00500b80, gArtifactNamesAnkh, "\xd1\xe8\xec\xe2\xee\xeb\x20\xe6\xe8\xe7\xed\xe8" /* "Символ жизни" */),
    DATA_COMPGEN(0x00500b90, gArtifactNamesBookOfElements, "\xca\xed\xe8\xe3\xe0\x20\xf1\xf2\xe8\xf5\xe8\xe9" /* "Книга стихий" */),
    DATA_COMPGEN(0x00500ba0, gArtifactNamesElementalRing, "\xca\xee\xeb\xfc\xf6\xee\x20\xf1\xf2\xe8\xf5\xe8\xe9" /* "Кольцо стихий" */),
    DATA_COMPGEN(0x00500bb0, gArtifactNamesHolyPendant, "\xd1\xe2\xff\xf2\xee\xe9\x20\xea\xf3\xeb\xee\xed" /* "Святой кулон" */),
    DATA_COMPGEN(0x00500bc0, gArtifactNamesPendantOfFreeWill, "\xcf\xee\xe4\xe2\xe5\xf1\xea\xe0\x20\xf1\xe2\xee\xe1\xee\xe4\xed\xee\xe9\x20\xe2\xee\xeb\xe8" /* "Подвеска свободной воли" */),
    DATA_COMPGEN(0x00500bd8, gArtifactNamesPendantOfLife, "\xca\xf3\xeb\xee\xed\x20\xe6\xe8\xe7\xed\xe8" /* "Кулон жизни" */),
    DATA_COMPGEN(0x00500be4, gArtifactNamesSerenityPendant, "\xcf\xee\xe4\xe2\xe5\xf1\xea\xe0\x20\xef\xee\xea\xee\xff" /* "Подвеска покоя" */),
    DATA_COMPGEN(0x00500bf4, gArtifactNamesSeeingEyePendant, "\xc2\xf1\xe5\xe2\xe8\xe4\xff\xf9\xe8\xe9\x20\xe3\xeb\xe0\xe7" /* "Всевидящий глаз" */),
    DATA_COMPGEN(0x00500c04, gArtifactNamesKineticPendant, "\xca\xf3\xeb\xee\xed\x20\xe4\xe2\xe8\xe6\xe5\xed\xe8\xff" /* "Кулон движения" */),
    DATA_COMPGEN(0x00500c14, gArtifactNamesPendantOfDeath, "\xca\xf3\xeb\xee\xed\x20\xf1\xec\xe5\xf0\xf2\xe8" /* "Кулон смерти" */),
    DATA_COMPGEN(0x00500c24, gArtifactNamesWandOfNegation, "\xcf\xee\xf1\xee\xf5\x20\xee\xf2\xf0\xe8\xf6\xe0\xed\xe8\xff" /* "Посох отрицания" */),
    DATA_COMPGEN(0x00500c34, gArtifactNamesGoldenBow, "\xc7\xee\xeb\xee\xf2\xee\xe9\x20\xeb\xf3\xea" /* "Золотой лук" */),
    DATA_COMPGEN(0x00500c40, gArtifactNamesTelescope, "\xd2\xe5\xeb\xe5\xf1\xea\xee\xef" /* "Телескоп" */),
    DATA_COMPGEN(0x00500c4c, gArtifactNamesStatesmanSQuill, "\xcf\xe5\xf0\xee\x20\xe4\xe8\xef\xeb\xee\xec\xe0\xf2\xe0" /* "Перо дипломата" */),
    DATA_COMPGEN(0x00500c5c, gArtifactNamesWizardSHat, "\xd8\xeb\xff\xef\xe0\x20\xec\xe0\xe3\xe0" /* "Шляпа мага" */),
    DATA_COMPGEN(0x00500c68, gArtifactNamesPowerRing, "\xca\xee\xeb\xfc\xf6\xee\x20\xf1\xe8\xeb\xfb" /* "Кольцо силы" */),
    DATA_COMPGEN(0x00500c74, gArtifactNamesAmmoCart, "\xce\xe1\xee\xe7" /* "Обоз" */),
    DATA_COMPGEN(0x00500c7c, gArtifactNamesTaxLien, "\xcf\xee\xe4\xe0\xf2\xfc" /* "Подать" */),
    DATA_COMPGEN(0x00500c84, gArtifactNamesHideousMask, "\xd3\xe6\xe0\xf1\xed\xe0\xff\x20\xec\xe0\xf1\xea\xe0" /* "Ужасная маска" */),
    DATA_COMPGEN(0x00500c94, gArtifactNamesEndlessPouchOfSulfur, "\xc1\xe5\xe7\xe4\xee\xed\xed\xe0\xff\x20\xf1\xf3\xec\xe0\x20\xf1\xe5\xf0\xfb" /* "Бездонная сума серы" */),
    DATA_COMPGEN(0x00500ca8, gArtifactNamesEndlessVialOfMercury, "\xc1\xe5\xe7\xe4\xee\xed\xed\xe0\xff\x20\xea\xee\xeb\xe1\xe0\x20\xf0\xf2\xf3\xf2\xe8" /* "Бездонная колба ртути" */),
    DATA_COMPGEN(0x00500cc0, gArtifactNamesEndlessPouchOfGems, "\xc1\xe5\xe7\xe4\xee\xed\xed\xe0\xff\x20\xf1\xf3\xec\xe0\x20\xf1\xe0\xec\xee\xf6\xe2\xe5\xf2\xee\xe2" /* "Бездонная сума самоцветов" */),
    DATA_COMPGEN(0x00500cdc, gArtifactNamesEndlessCordOfWood, "\xcd\xe5\xf1\xea\xee\xed\xf7\xe0\xe5\xec\xe0\xff\x20\xe2\xff\xe7\xe0\xed\xea\xe0\x20\xe4\xf0\xee\xe2" /* "Нескончаемая вязанка дров" */),
    DATA_COMPGEN(0x00500cf8, gArtifactNamesEndlessCartOfOre, "\xc1\xe5\xe7\xe4\xee\xed\xed\xe0\xff\x20\xe2\xe0\xe3\xee\xed\xe5\xf2\xea\xe0\x20\xf0\xf3\xe4\xfb" /* "Бездонная вагонетка руды" */),
    DATA_COMPGEN(0x00500d14, gArtifactNamesEndlessPouchOfCrystal, "\xc1\xe5\xe7\xe4\xee\xed\xed\xe0\xff\x20\xf1\xf3\xec\xe0\x20\xea\xf0\xe8\xf1\xf2\xe0\xeb\xeb\xee\xe2" /* "Бездонная сума кристаллов" */),
    DATA_COMPGEN(0x00500d30, gArtifactNamesSpikedHelm, "\xd8\xeb\xe5\xec\x20\xf1\x20\xf8\xe8\xef\xe0\xec\xe8" /* "Шлем с шипами" */),
    DATA_COMPGEN(0x00500d40, gArtifactNamesSpikedShield, "\xd9\xe8\xf2\x20\xf1\x20\xf8\xe8\xef\xe0\xec\xe8" /* "Щит с шипами" */),
    DATA_COMPGEN(0x00500d50, gArtifactNamesWhitePearl, "\xc1\xe5\xeb\xe0\xff\x20\xe6\xe5\xec\xf7\xf3\xe6\xe8\xed\xe0" /* "Белая жемчужина" */),
    DATA_COMPGEN(0x00500d60, gArtifactNamesBlackPearl, "\xd7\xe5\xf0\xed\xe0\xff\x20\xe6\xe5\xec\xf7\xf3\xe6\xe8\xed\xe0" /* "Черная жемчужина" */),
    DATA_COMPGEN(0x00500d74, gArtifactNamesMagicBook, "\xc2\xee\xeb\xf8\xe5\xe1\xed\xe0\xff\x20\xea\xed\xe8\xe3\xe0" /* "Волшебная книга" */),
    DATA_COMPGEN(0x00500d84, gArtifactNamesERRORArtifact82, "\x45\x52\x52\x4f\x52\x20\x3a\x20\x41\x72\x74\x69\x66\x61\x63\x74\x20\x38\x32" /* "ERROR : Artifact 82" */),
    DATA_COMPGEN(0x00500d98, gArtifactNamesERRORArtifact83, "\x45\x52\x52\x4f\x52\x20\x3a\x20\x41\x72\x74\x69\x66\x61\x63\x74\x20\x38\x33" /* "ERROR : Artifact 83" */),
    DATA_COMPGEN(0x00500dac, gArtifactNamesERRORArtifact84, "\x45\x52\x52\x4f\x52\x20\x3a\x20\x41\x72\x74\x69\x66\x61\x63\x74\x20\x38\x34" /* "ERROR : Artifact 84" */),
    DATA_COMPGEN(0x00500dc0, gArtifactNamesERRORArtifact85, "\x45\x52\x52\x4f\x52\x20\x3a\x20\x41\x72\x74\x69\x66\x61\x63\x74\x20\x38\x35" /* "ERROR : Artifact 85" */),
    DATA_COMPGEN(0x00500dd4, gArtifactNamesSpellScroll, "\xd1\xe2\xe8\xf2\xee\xea\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe9" /* "Свиток заклинаний" */),
    DATA_COMPGEN(0x00500de8, gArtifactNamesArmOfTheMartyr, "\xd0\xf3\xea\xe0\x20\xec\xf3\xf7\xe5\xed\xe8\xea\xe0" /* "Рука мученика" */),
    DATA_COMPGEN(0x00500df8, gArtifactNamesBreastplateOfAnduran, "\xc4\xee\xf1\xef\xe5\xf5\x20\xc0\xed\xe4\xf3\xf0\xe0\xed\xe0" /* "Доспех Андурана" */),
    DATA_COMPGEN(0x00500e08, gArtifactNamesBroachOfShielding, "\xc7\xe0\xf9\xe8\xf2\xed\xe0\xff\x20\xe1\xf0\xee\xf8\xfc" /* "Защитная брошь" */),
    DATA_COMPGEN(0x00500e18, gArtifactNamesBattleGarbOfAnduran, "\xc1\xee\xe5\xe2\xee\xe5\x20\xee\xe4\xe5\xff\xed\xe8\xe5\x20\xc0\xed\xe4\xf3\xf0\xe0\xed\xe0" /* "Боевое одеяние Андурана" */),
    DATA_COMPGEN(0x00500e30, gArtifactNamesCrystalBall, "\xca\xf0\xe8\xf1\xf2\xe0\xeb\xfc\xed\xfb\xe9\x20\xf8\xe0\xf0" /* "Кристальный шар" */),
    DATA_COMPGEN(0x00500e40, gArtifactNamesHeartOfFire, "\xd1\xe5\xf0\xe4\xf6\xe5\x20\xee\xe3\xed\xff" /* "Сердце огня" */),
    DATA_COMPGEN(0x00500e4c, gArtifactNamesHeartOfIce, "\xcb\xe5\xe4\xff\xed\xee\xe5\x20\xf1\xe5\xf0\xe4\xf6\xe5" /* "Ледяное сердце" */),
    DATA_COMPGEN(0x00500e5c, gArtifactNamesHelmetOfAnduran, "\xd8\xeb\xe5\xec\x20\xc0\xed\xe4\xf3\xf0\xe0\xed\xe0" /* "Шлем Андурана" */),
    DATA_COMPGEN(0x00500e6c, gArtifactNamesHolyHammer, "\xd1\xe2\xff\xf2\xee\xe9\x20\xec\xee\xeb\xee\xf2" /* "Святой молот" */),
    DATA_COMPGEN(0x00500e7c, gArtifactNamesLegendaryScepter, "\xcb\xe5\xe3\xe5\xed\xe4\xe0\xf0\xed\xfb\xe9\x20\xf1\xea\xe8\xef\xe5\xf2\xf0" /* "Легендарный скипетр" */),
    DATA_COMPGEN(0x00500e90, gArtifactNamesMasthead, "\xcd\xe0\xea\xee\xed\xe5\xf7\xed\xe8\xea\x20\xec\xe0\xf7\xf2\xfb" /* "Наконечник мачты" */),
    DATA_COMPGEN(0x00500ea4, gArtifactNamesSphereOfNegation, "\xd1\xf4\xe5\xf0\xe0\x20\xe0\xed\xf2\xe8\xec\xe0\xe3\xe8\xe8" /* "Сфера антимагии" */),
    DATA_COMPGEN(0x00500eb4, gArtifactNamesStaffOfWizardry, "\xc2\xee\xeb\xf8\xe5\xe1\xed\xfb\xe9\x20\xef\xee\xf1\xee\xf5" /* "Волшебный посох" */),
    DATA_COMPGEN(0x00500ec4, gArtifactNamesSwordBreaker, "\xcc\xe5\xf7\xe5\xeb\xee\xec" /* "Мечелом" */),
    DATA_COMPGEN(0x00500ecc, gArtifactNamesSwordOfAnduran, "\xcc\xe5\xf7\x20\xc0\xed\xe4\xf3\xf0\xe0\xed\xe0" /* "Меч Андурана" */),
    DATA_COMPGEN(0x00500edc, gArtifactNamesSpadeOfNecromancy, "\xcb\xee\xef\xe0\xf2\xe0\x20\xec\xee\xe3\xe8\xeb\xfc\xf9\xe8\xea\xe0" /* "Лопата могильщика" */)
};
DATA(0x004fd948) char* gArtifactDesc[IDX(ARTIFACT_COUNT)] = {
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
DATA(0x004fdae4) char* gArtifactEvent[IDX(ARTIFACT_COUNT)] = {
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
DATA(0x004fdc80) char* gStatNames[HERO_PRIMARY_STAT_COUNT] = {
    DATA_COMPGEN(0x00506d08, gStatNamesAttackSkill, "\xc0\xf2\xe0\xea\xe0" /* "Атака" */),
    DATA_COMPGEN(0x00506d10, gStatNamesDefenseSkill, "\xc7\xe0\xf9\xe8\xf2\xe0" /* "Защита" */),
    DATA_COMPGEN(0x00506d18, gStatNamesSpellPower, "\xd1\xe8\xeb\xe0\x20\xec\xe0\xe3\xe8\xe8" /* "Сила магии" */),
    DATA_COMPGEN(0x00506d24, gStatNamesKnowledge, "\xc7\xed\xe0\xed\xe8\xff" /* "Знания" */)
};
DATA(0x004fdc90) char* gStatDesc[HERO_PRIMARY_STAT_COUNT] = {
    DATA_COMPGEN(0x00506d2c, gStatDescAttackSkillYourAttackSkillIs, "\x7b\xc0\xf2\xe0\xea\xe0\x7d\x0a\x0a\xc2\xe0\xf8\x20\xed\xe0\xe2\xfb\xea\x20\xe0\xf2\xe0\xea\xe8\x20\x2d\x20\xe1\xee\xed\xf3\xf1\x2c\x20\xe4\xee\xe1\xe0\xe2\xeb\xff\xe5\xec\xfb\xe9\x20\xea\x20\xed\xe0\xe2\xfb\xea\xf3\x20\xe0\xf2\xe0\xea\xe8\x20\xea\xe0\xe6\xe4\xee\xe3\xee\x20\xe2\xee\xe8\xed\xe0\x2e" /* "{Атака}\n\nВаш навык атаки - бонус, добавляемый к навыку атаки каждого воина." */),
    DATA_COMPGEN(0x00506d78, gStatDescDefenseSkillYourDefenseSkillIs, "\x7b\xc7\xe0\xf9\xe8\xf2\xe0\x7d\x0a\x0a\xc2\xe0\xf8\x20\xed\xe0\xe2\xfb\xea\x20\xe7\xe0\xf9\xe8\xf2\xfb\x20\x2d\x20\xe1\xee\xed\xf3\xf1\x2c\x20\xe4\xee\xe1\xe0\xe2\xeb\xff\xe5\xec\xfb\xe9\x20\xea\x20\xed\xe0\xe2\xfb\xea\xf3\x20\xe7\xe0\xf9\xe8\xf2\xfb\x20\xea\xe0\xe6\xe4\xee\xe3\xee\x20\xe2\xee\xe8\xed\xe0\x2e" /* "{Защита}\n\nВаш навык защиты - бонус, добавляемый к навыку защиты каждого воина." */),
    DATA_COMPGEN(0x00506dc8, gStatDescSpellPowerYourSpellPowerDetermines, "\x7b\xd1\xe8\xeb\xe0\x20\xec\xe0\xe3\xe8\xe8\x7d\x0a\x0a\xc2\xe0\xf8\x20\xf3\xf0\xee\xe2\xe5\xed\xfc\x20\xf1\xe8\xeb\xfb\x20\xec\xe0\xe3\xe8\xe8\x20\xee\xef\xf0\xe5\xe4\xe5\xeb\xff\xe5\xf2\x20\xe4\xeb\xe8\xf2\xe5\xeb\xfc\xed\xee\xf1\xf2\xfc\x20\xe4\xe5\xe9\xf1\xf2\xe2\xe8\xff\x20\xe8\xeb\xe8\x20\xf1\xe8\xeb\xf3\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\x2e" /* "{Сила магии}\n\nВаш уровень силы магии определяет длительность действия или силу заклинания." */),
    DATA_COMPGEN(0x00506e24, gStatDescKnowledgeYourKnowledgeDeterminesHowMany, "\x7b\xc7\xed\xe0\xed\xe8\xff\x7d\x0a\x0a\xd3\xf0\xee\xe2\xe5\xed\xfc\x20\xe7\xed\xe0\xed\xe8\xe9\x20\xee\xef\xf0\xe5\xe4\xe5\xeb\xff\xe5\xf2\x20\xea\xee\xeb\xe8\xf7\xe5\xf1\xf2\xe2\xee\x20\xee\xf7\xea\xee\xe2\x20\xec\xe0\xe3\xe8\xe8\x20\xe3\xe5\xf0\xee\xff\x2e" /* "{Знания}\n\nУровень знаний определяет количество очков магии героя." */)
};
DATA(0x004fdca0) char* gAlignmentNames[KB_ALIGNMENT_NAME_COUNT] = {
    DATA_COMPGEN(0x00506e68, gAlignmentNamesKnight, "\xd0\xfb\xf6\xe0\xf0\xfc" /* "Рыцарь" */),
    DATA_COMPGEN(0x00506e70, gAlignmentNamesBarbarian, "\xc2\xe0\xf0\xe2\xe0\xf0" /* "Варвар" */),
    DATA_COMPGEN(0x00506e78, gAlignmentNamesSorceress, "\xca\xee\xeb\xe4\xf3\xed\xfc\xff" /* "Колдунья" */),
    DATA_COMPGEN(0x00506e84, gAlignmentNamesWarlock, "\xd7\xe5\xf0\xed\xee\xea\xed\xe8\xe6\xed\xe8\xea" /* "Чернокнижник" */),
    DATA_COMPGEN(0x00506e94, gAlignmentNamesWizard, "\xd7\xe0\xf0\xee\xe4\xe5\xe9" /* "Чародей" */),
    DATA_COMPGEN(0x00506e9c, gAlignmentNamesNecromancer, "\xcd\xe5\xea\xf0\xee\xec\xe0\xed\xf2" /* "Некромант" */),
    DATA_COMPGEN(0x00506ea8, gAlignmentNamesMultiple, "\xcc\xf3\xeb\xfc\xf2\xe8" /* "Мульти" */),
    DATA_COMPGEN(0x00506eb0, gAlignmentNamesRandom, "\xd1\xeb\xf3\xf7\xe0\xe9\xed\xee" /* "Случайно" */)
};
char* gArmyShortNames[IDX(CREATURE_COUNT)] = {
    "peasn", "archr", "arch2", "pikmn", "pikm2", "swman", "swma2", "cvlry", "cvlr2", "paldn",
    "pald2", "gobln", "orc__", "orc_2", "Wolf_", "Ogre_", "Ogre2", "Troll", "trol2", "cyclp",
    "sprit", "Dwarf", "dwar2", "elf__", "elf_2", "druid", "drui2", "uncrn", "phoen", "centr",
    "gargl", "griff", "mintr", "mint2", "Hydra", "dragn", "drag2", "drag3", "hlflg", "Boar_",
    "irong", "iron2", "roc__", "archm", "arch2", "titan", "tita2", "skel_", "zomb_", "zomb2",
    "Mummy", "mumm2", "vampr", "vamp2", "lich_", "lich2", "boned", "Rogue", "Nomad", "Ghost",
    "Genie", "medus", "eleme", "elema", "elemf", "elemw"
};
DATA(0x004fddc8) char* gArmyNames[IDX(CREATURE_COUNT)] = {
    DATA_COMPGEN(0x005070cc, gArmyNamesPeasant, "\xca\xf0\xe5\xf1\xf2\xfc\xff\xed\xe8\xed" /* "Крестьянин" */),
    DATA_COMPGEN(0x005070d8, gArmyNamesArcher, "\xd1\xf2\xf0\xe5\xeb\xee\xea" /* "Стрелок" */),
    DATA_COMPGEN(0x005070e0, gArmyNamesRanger, "\xd0\xe5\xe9\xed\xe4\xe6\xe5\xf0" /* "Рейнджер" */),
    DATA_COMPGEN(0x005070ec, gArmyNamesPikeman, "\xca\xee\xef\xe5\xe9\xf9\xe8\xea" /* "Копейщик" */),
    DATA_COMPGEN(0x005070f8, gArmyNamesVeteranPikeman, "\xca\xee\xef\xe5\xe9\xf9\xe8\xea\x20\xe2\xe5\xf2\xe5\xf0\xe0\xed" /* "Копейщик ветеран" */),
    DATA_COMPGEN(0x0050710c, gArmyNamesSwordsman, "\xcc\xe5\xf7\xed\xe8\xea" /* "Мечник" */),
    DATA_COMPGEN(0x00507114, gArmyNamesMasterSwordsman, "\xcc\xe5\xf7\xed\xe8\xea\x20\xec\xe0\xf1\xf2\xe5\xf0" /* "Мечник мастер" */),
    DATA_COMPGEN(0x00507124, gArmyNamesCavalry, "\xc2\xf1\xe0\xe4\xed\xe8\xea" /* "Всадник" */),
    DATA_COMPGEN(0x0050712c, gArmyNamesChampion, "\xd7\xe5\xec\xef\xe8\xee\xed" /* "Чемпион" */),
    DATA_COMPGEN(0x00507134, gArmyNamesPaladin, "\xcf\xe0\xeb\xe0\xe4\xe8\xed" /* "Паладин" */),
    DATA_COMPGEN(0x0050713c, gArmyNamesCrusader, "\xca\xf0\xe5\xf1\xf2\xee\xed\xee\xf1\xe5\xf6" /* "Крестоносец" */),
    DATA_COMPGEN(0x00507148, gArmyNamesGoblin, "\xc3\xee\xe1\xeb\xe8\xed" /* "Гоблин" */),
    DATA_COMPGEN(0x00507150, gArmyNamesOrc, "\xce\xf0\xea" /* "Орк" */),
    DATA_COMPGEN(0x00507154, gArmyNamesOrcChief, "\xc2\xee\xe6\xe4\xfc\x20\xee\xf0\xea\xee\xe2" /* "Вождь орков" */),
    DATA_COMPGEN(0x00507160, gArmyNamesWolf, "\xc2\xee\xeb\xea" /* "Волк" */),
    DATA_COMPGEN(0x00507168, gArmyNamesOgre, "\xce\xe3\xf0" /* "Огр" */),
    DATA_COMPGEN(0x0050716c, gArmyNamesOgreLord, "\xcb\xee\xf0\xe4\x20\xee\xe3\xf0\xee\xe2" /* "Лорд огров" */),
    DATA_COMPGEN(0x00507178, gArmyNamesTroll, "\xd2\xf0\xee\xeb\xeb\xfc" /* "Тролль" */),
    DATA_COMPGEN(0x00507180, gArmyNamesWarTroll, "\xc1\xee\xe5\xe2\xee\xe9\x20\xf2\xf0\xee\xeb\xeb\xfc" /* "Боевой тролль" */),
    DATA_COMPGEN(0x00507190, gArmyNamesCyclops, "\xd6\xe8\xea\xeb\xee\xef" /* "Циклоп" */),
    DATA_COMPGEN(0x00507198, gArmyNamesSprite, "\xd4\xe5\xff" /* "Фея" */),
    DATA_COMPGEN(0x0050719c, gArmyNamesDwarf, "\xc3\xed\xee\xec" /* "Гном" */),
    DATA_COMPGEN(0x005071a4, gArmyNamesBattleDwarf, "\xc1\xee\xe5\xe2\xee\xe9\x20\xe3\xed\xee\xec" /* "Боевой гном" */),
    DATA_COMPGEN(0x005071b0, gArmyNamesElf, "\xdd\xeb\xfc\xf4" /* "Эльф" */),
    DATA_COMPGEN(0x005071b8, gArmyNamesGrandElf, "\xc2\xfb\xf1\xee\xea\xe8\xe9\x20\xfd\xeb\xfc\xf4" /* "Высокий эльф" */),
    DATA_COMPGEN(0x005071c8, gArmyNamesDruid, "\xc4\xf0\xf3\xe8\xe4" /* "Друид" */),
    DATA_COMPGEN(0x005071d0, gArmyNamesGreaterDruid, "\xd1\xf2\xe0\xf0\xf8\xe8\xe9\x20\xe4\xf0\xf3\xe8\xe4" /* "Старший друид" */),
    DATA_COMPGEN(0x005071e0, gArmyNamesUnicorn, "\xc5\xe4\xe8\xed\xee\xf0\xee\xe3" /* "Единорог" */),
    DATA_COMPGEN(0x005071ec, gArmyNamesPhoenix, "\xd4\xe5\xed\xe8\xea\xf1" /* "Феникс" */),
    DATA_COMPGEN(0x005071f4, gArmyNamesCentaur, "\xca\xe5\xed\xf2\xe0\xe2\xf0" /* "Кентавр" */),
    DATA_COMPGEN(0x005071fc, gArmyNamesGargoyle, "\xc3\xee\xf0\xe3\xf3\xeb\xfc\xff" /* "Горгулья" */),
    DATA_COMPGEN(0x00507208, gArmyNamesGriffin, "\xc3\xf0\xe8\xf4\xee\xed" /* "Грифон" */),
    DATA_COMPGEN(0x00507210, gArmyNamesMinotaur, "\xcc\xe8\xed\xee\xf2\xe0\xe2\xf0" /* "Минотавр" */),
    DATA_COMPGEN(0x0050721c, gArmyNamesMinotaurKing, "\xd6\xe0\xf0\xfc\x20\xec\xe8\xed\xee\xf2\xe0\xe2\xf0\xee\xe2" /* "Царь минотавров" */),
    DATA_COMPGEN(0x0050722c, gArmyNamesHydra, "\xc3\xe8\xe4\xf0\xe0" /* "Гидра" */),
    DATA_COMPGEN(0x00507234, gArmyNamesGreenDragon, "\xc7\xe5\xeb\xe5\xed\xfb\xe9\x20\xe4\xf0\xe0\xea\xee\xed" /* "Зеленый дракон" */),
    DATA_COMPGEN(0x00507244, gArmyNamesRedDragon, "\xca\xf0\xe0\xf1\xed\xfb\xe9\x20\xe4\xf0\xe0\xea\xee\xed" /* "Красный дракон" */),
    DATA_COMPGEN(0x00507254, gArmyNamesBlackDragon, "\xd7\xe5\xf0\xed\xfb\xe9\x20\xe4\xf0\xe0\xea\xee\xed" /* "Черный дракон" */),
    DATA_COMPGEN(0x00507264, gArmyNamesHalfling, "\xcf\xee\xeb\xf3\xf0\xee\xf1\xeb\xe8\xea" /* "Полурослик" */),
    DATA_COMPGEN(0x00507270, gArmyNamesBoar, "\xc1\xee\xf0\xee\xe2" /* "Боров" */),
    DATA_COMPGEN(0x00507278, gArmyNamesIronGolem, "\xc6\xe5\xeb\xe5\xe7\xed\xfb\xe9\x20\xe3\xee\xeb\xe5\xec" /* "Железный голем" */),
    DATA_COMPGEN(0x00507288, gArmyNamesSteelGolem, "\xd1\xf2\xe0\xeb\xfc\xed\xee\xe9\x20\xe3\xee\xeb\xe5\xec" /* "Стальной голем" */),
    DATA_COMPGEN(0x00507298, gArmyNamesRoc, "\xd0\xf3\xf5" /* "Рух" */),
    DATA_COMPGEN(0x0050729c, gArmyNamesMage, "\xcc\xe0\xe3" /* "Маг" */),
    DATA_COMPGEN(0x005072a0, gArmyNamesArchmage, "\xc0\xf0\xf5\xe8\xec\xe0\xe3" /* "Архимаг" */),
    DATA_COMPGEN(0x005072a8, gArmyNamesGiant, "\xc3\xe8\xe3\xe0\xed\xf2" /* "Гигант" */),
    DATA_COMPGEN(0x005072b0, gArmyNamesTitan, "\xd2\xe8\xf2\xe0\xed" /* "Титан" */),
    DATA_COMPGEN(0x005072b8, gArmyNamesSkeleton, "\xd1\xea\xe5\xeb\xe5\xf2" /* "Скелет" */),
    DATA_COMPGEN(0x005072c0, gArmyNamesZombie, "\xc7\xee\xec\xe1\xe8" /* "Зомби" */),
    DATA_COMPGEN(0x005072c8, gArmyNamesMutantZombie, "\xc7\xee\xec\xe1\xe8\x20\xec\xf3\xf2\xe0\xed\xf2" /* "Зомби мутант" */),
    DATA_COMPGEN(0x005072d8, gArmyNamesMummy, "\xcc\xf3\xec\xe8\xff" /* "Мумия" */),
    DATA_COMPGEN(0x005072e0, gArmyNamesRoyalMummy, "\xca\xee\xf0\xee\xeb\xe5\xe2\xf1\xea\xe0\xff\x20\xec\xf3\xec\xe8\xff" /* "Королевская мумия" */),
    DATA_COMPGEN(0x005072f4, gArmyNamesVampire, "\xc2\xe0\xec\xef\xe8\xf0" /* "Вампир" */),
    DATA_COMPGEN(0x005072fc, gArmyNamesVampireLord, "\xcb\xee\xf0\xe4\x20\xe2\xe0\xec\xef\xe8\xf0\xee\xe2" /* "Лорд вампиров" */),
    DATA_COMPGEN(0x0050730c, gArmyNamesLich, "\xcb\xe8\xf7" /* "Лич" */),
    DATA_COMPGEN(0x00507310, gArmyNamesPowerLich, "\xcc\xee\xe3\xf3\xf7\xe8\xe9\x20\xeb\xe8\xf7" /* "Могучий лич" */),
    DATA_COMPGEN(0x0050731c, gArmyNamesBoneDragon, "\xca\xee\xf1\xf2\xff\xed\xee\xe9\x20\xe4\xf0\xe0\xea\xee\xed" /* "Костяной дракон" */),
    DATA_COMPGEN(0x0050732c, gArmyNamesRogue, "\xd0\xe0\xe7\xe1\xee\xe9\xed\xe8\xea" /* "Разбойник" */),
    DATA_COMPGEN(0x00507338, gArmyNamesNomad, "\xca\xee\xf7\xe5\xe2\xed\xe8\xea" /* "Кочевник" */),
    DATA_COMPGEN(0x00507344, gArmyNamesGhost, "\xcf\xf0\xe8\xe7\xf0\xe0\xea" /* "Призрак" */),
    DATA_COMPGEN(0x0050734c, gArmyNamesGenie, "\xc4\xe6\xe8\xed\xed" /* "Джинн" */),
    DATA_COMPGEN(0x00507354, gArmyNamesMedusa, "\xcc\xe5\xe4\xf3\xe7\xe0" /* "Медуза" */),
    DATA_COMPGEN(0x0050735c, gArmyNamesEarthElemental, "\xc7\xe5\xec\xed\xee\xe9\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb" /* "Земной элементал" */),
    DATA_COMPGEN(0x00507370, gArmyNamesAirElemental, "\xc2\xee\xe7\xe4\xf3\xf8\xed\xfb\xe9\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb" /* "Воздушный элементал" */),
    DATA_COMPGEN(0x00507384, gArmyNamesFireElemental, "\xce\xe3\xed\xe5\xed\xed\xfb\xe9\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb" /* "Огненный элементал" */),
    DATA_COMPGEN(0x00507398, gArmyNamesWaterElemental, "\xc2\xee\xe4\xff\xed\xee\xe9\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb" /* "Водяной элементал" */)
};
DATA(0x004fded0) char* gArmyNamesPlural[IDX(CREATURE_COUNT)] = {
    DATA_COMPGEN(0x005073ac, gArmyNamesPluralPeasants, "\xea\xf0\xe5\xf1\xf2\xfc\xff\xed" /* "крестьян" */),
    DATA_COMPGEN(0x005073b8, gArmyNamesPluralArchers, "\xf1\xf2\xf0\xe5\xeb\xea\xee\xe2" /* "стрелков" */),
    DATA_COMPGEN(0x005073c4, gArmyNamesPluralRangers, "\xf0\xe5\xe9\xed\xe4\xe6\xe5\xf0\xee\xe2" /* "рейнджеров" */),
    DATA_COMPGEN(0x005073d0, gArmyNamesPluralPikemen, "\xea\xee\xef\xe5\xe9\xf9\xe8\xea\xee\xe2" /* "копейщиков" */),
    DATA_COMPGEN(0x005073dc, gArmyNamesPluralVeteranPikemen, "\xea\xee\xef\xe5\xe9\xf9\xe8\xea\xee\xe2\x20\xe2\xe5\xf2\xe5\xf0\xe0\xed\xee\xe2" /* "копейщиков ветеранов" */),
    DATA_COMPGEN(0x005073f4, gArmyNamesPluralSwordsmen, "\xec\xe5\xf7\xed\xe8\xea\xee\xe2" /* "мечников" */),
    DATA_COMPGEN(0x00507400, gArmyNamesPluralMasterSwordsmen, "\xec\xe5\xf7\xed\xe8\xea\xee\xe2\x20\xec\xe0\xf1\xf2\xe5\xf0\xee\xe2" /* "мечников мастеров" */),
    DATA_COMPGEN(0x00507414, gArmyNamesPluralCavalries, "\xe2\xf1\xe0\xe4\xed\xe8\xea\xee\xe2" /* "всадников" */),
    DATA_COMPGEN(0x00507420, gArmyNamesPluralChampions, "\xf7\xe5\xec\xef\xe8\xee\xed\xee\xe2" /* "чемпионов" */),
    DATA_COMPGEN(0x0050742c, gArmyNamesPluralPaladins, "\xef\xe0\xeb\xe0\xe4\xe8\xed\xee\xe2" /* "паладинов" */),
    DATA_COMPGEN(0x00507438, gArmyNamesPluralCrusaders, "\xea\xf0\xe5\xf1\xf2\xee\xed\xee\xf1\xf6\xe5\xe2" /* "крестоносцев" */),
    DATA_COMPGEN(0x00507448, gArmyNamesPluralGoblins, "\xe3\xee\xe1\xeb\xe8\xed\xee\xe2" /* "гоблинов" */),
    DATA_COMPGEN(0x00507454, gArmyNamesPluralOrcs, "\xee\xf0\xea\xee\xe2" /* "орков" */),
    DATA_COMPGEN(0x0050745c, gArmyNamesPluralOrcChiefs, "\xe2\xee\xe6\xe4\xe5\xe9\x20\xee\xf0\xea\xee\xe2" /* "вождей орков" */),
    DATA_COMPGEN(0x0050746c, gArmyNamesPluralWolves, "\xe2\xee\xeb\xea\xee\xe2" /* "волков" */),
    DATA_COMPGEN(0x00507474, gArmyNamesPluralOgres, "\xee\xe3\xf0\xee\xe2" /* "огров" */),
    DATA_COMPGEN(0x0050747c, gArmyNamesPluralOgreLords, "\xeb\xee\xf0\xe4\xee\xe2\x20\xee\xe3\xf0\xee\xe2" /* "лордов огров" */),
    DATA_COMPGEN(0x0050748c, gArmyNamesPluralTrolls, "\xf2\xf0\xee\xeb\xeb\xe5\xe9" /* "троллей" */),
    DATA_COMPGEN(0x00507494, gArmyNamesPluralWarTroll, "\xe1\xee\xe5\xe2\xfb\xf5\x20\xf2\xf0\xee\xeb\xeb\xe5\xe9" /* "боевых троллей" */),
    DATA_COMPGEN(0x005074a4, gArmyNamesPluralCyclopes, "\xf6\xe8\xea\xeb\xee\xef\xee\xe2" /* "циклопов" */),
    DATA_COMPGEN(0x005074b0, gArmyNamesPluralSprites, "\xf4\xe5\xe9" /* "фей" */),
    DATA_COMPGEN(0x005074b4, gArmyNamesPluralDwarves, "\xe3\xed\xee\xec\xee\xe2" /* "гномов" */),
    DATA_COMPGEN(0x005074bc, gArmyNamesPluralBattleDwarves, "\xe1\xee\xe5\xe2\xfb\xf5\x20\xe3\xed\xee\xec\xee\xe2" /* "боевых гномов" */),
    DATA_COMPGEN(0x005074cc, gArmyNamesPluralElves, "\xfd\xeb\xfc\xf4\xee\xe2" /* "эльфов" */),
    DATA_COMPGEN(0x005074d4, gArmyNamesPluralGrandElves, "\xe2\xfb\xf1\xee\xea\xe8\xf5\x20\xfd\xeb\xfc\xf4\xee\xe2" /* "высоких эльфов" */),
    DATA_COMPGEN(0x005074e4, gArmyNamesPluralDruids, "\xe4\xf0\xf3\xe8\xe4\xee\xe2" /* "друидов" */),
    DATA_COMPGEN(0x005074ec, gArmyNamesPluralGreaterDruids, "\xf1\xf2\xe0\xf0\xf8\xe8\xf5\x20\xe4\xf0\xf3\xe8\xe4\xee\xe2" /* "старших друидов" */),
    DATA_COMPGEN(0x005074fc, gArmyNamesPluralUnicorns, "\xe5\xe4\xe8\xed\xee\xf0\xee\xe3\xee\xe2" /* "единорогов" */),
    DATA_COMPGEN(0x00507508, gArmyNamesPluralPhoenix, "\xf4\xe5\xed\xe8\xea\xf1\xee\xe2" /* "фениксов" */),
    DATA_COMPGEN(0x00507514, gArmyNamesPluralCentaurs, "\xea\xe5\xed\xf2\xe0\xe2\xf0\xee\xe2" /* "кентавров" */),
    DATA_COMPGEN(0x00507520, gArmyNamesPluralGargoyles, "\xe3\xee\xf0\xe3\xf3\xeb\xe8\xe9" /* "горгулий" */),
    DATA_COMPGEN(0x0050752c, gArmyNamesPluralGriffins, "\xe3\xf0\xe8\xf4\xee\xed\xee\xe2" /* "грифонов" */),
    DATA_COMPGEN(0x00507538, gArmyNamesPluralMinotaurs, "\xec\xe8\xed\xee\xf2\xe0\xe2\xf0\xee\xe2" /* "минотавров" */),
    DATA_COMPGEN(0x00507544, gArmyNamesPluralMinotaurKings, "\xf6\xe0\xf0\xe5\xe9\x20\xec\xe8\xed\xee\xf2\xe0\xe2\xf0\xee\xe2" /* "царей минотавров" */),
    DATA_COMPGEN(0x00507558, gArmyNamesPluralHydras, "\xe3\xe8\xe4\xf0" /* "гидр" */),
    DATA_COMPGEN(0x00507560, gArmyNamesPluralGreenDragons, "\xe7\xe5\xeb\xe5\xed\xfb\xf5\x20\xe4\xf0\xe0\xea\xee\xed\xee\xe2" /* "зеленых драконов" */),
    DATA_COMPGEN(0x00507574, gArmyNamesPluralRedDragons, "\xea\xf0\xe0\xf1\xed\xfb\xf5\x20\xe4\xf0\xe0\xea\xee\xed\xee\xe2" /* "красных драконов" */),
    DATA_COMPGEN(0x00507588, gArmyNamesPluralBlackDragons, "\xf7\xe5\xf0\xed\xfb\xf5\x20\xe4\xf0\xe0\xea\xee\xed\xee\xe2" /* "черных драконов" */),
    DATA_COMPGEN(0x00507598, gArmyNamesPluralHalflings, "\xef\xee\xeb\xf3\xf0\xee\xf1\xeb\xe8\xea\xee\xe2" /* "полуросликов" */),
    DATA_COMPGEN(0x005075a8, gArmyNamesPluralBoars, "\xe1\xee\xf0\xee\xe2\xee\xe2" /* "боровов" */),
    DATA_COMPGEN(0x005075b0, gArmyNamesPluralIronGolems, "\xe6\xe5\xeb\xe5\xe7\xed\xfb\xf5\x20\xe3\xee\xeb\xe5\xec\xee\xe2" /* "железных големов" */),
    DATA_COMPGEN(0x005075c4, gArmyNamesPluralSteelGolems, "\xf1\xf2\xe0\xeb\xfc\xed\xfb\xf5\x20\xe3\xee\xeb\xe5\xec\xee\xe2" /* "стальных големов" */),
    DATA_COMPGEN(0x005075d8, gArmyNamesPluralRocs, "\xf0\xf3\xf5\xee\xe2" /* "рухов" */),
    DATA_COMPGEN(0x005075e0, gArmyNamesPluralMagi, "\xec\xe0\xe3\xee\xe2" /* "магов" */),
    DATA_COMPGEN(0x005075e8, gArmyNamesPluralArchmagi, "\xe0\xf0\xf5\xe8\xec\xe0\xe3\xee\xe2" /* "архимагов" */),
    DATA_COMPGEN(0x005075f4, gArmyNamesPluralGiants, "\xe3\xe8\xe3\xe0\xed\xf2\xee\xe2" /* "гигантов" */),
    DATA_COMPGEN(0x00507600, gArmyNamesPluralTitans, "\xf2\xe8\xf2\xe0\xed\xee\xe2" /* "титанов" */),
    DATA_COMPGEN(0x00507608, gArmyNamesPluralSkeletons, "\xf1\xea\xe5\xeb\xe5\xf2\xee\xe2" /* "скелетов" */),
    DATA_COMPGEN(0x00507614, gArmyNamesPluralZombies, "\xe7\xee\xec\xe1\xe8" /* "зомби" */),
    DATA_COMPGEN(0x0050761c, gArmyNamesPluralMutantZombies, "\xe7\xee\xec\xe1\xe8\x20\xec\xf3\xf2\xe0\xed\xf2\xee\xe2" /* "зомби мутантов" */),
    DATA_COMPGEN(0x0050762c, gArmyNamesPluralMummies, "\xec\xf3\xec\xe8\xe9" /* "мумий" */),
    DATA_COMPGEN(0x00507634, gArmyNamesPluralRoyalMummies, "\xea\xee\xf0\xee\xeb\xe5\xe2\xf1\xea\xe8\xf5\x20\xec\xf3\xec\xe8\xe9" /* "королевских мумий" */),
    DATA_COMPGEN(0x00507648, gArmyNamesPluralVampires, "\xe2\xe0\xec\xef\xe8\xf0\xee\xe2" /* "вампиров" */),
    DATA_COMPGEN(0x00507654, gArmyNamesPluralVampireLords, "\xeb\xee\xf0\xe4\xee\xe2\x20\xe2\xe0\xec\xef\xe8\xf0\xee\xe2" /* "лордов вампиров" */),
    DATA_COMPGEN(0x00507664, gArmyNamesPluralLiches, "\xeb\xe8\xf7\xe5\xe9" /* "личей" */),
    DATA_COMPGEN(0x0050766c, gArmyNamesPluralPowerLiches, "\xec\xee\xe3\xf3\xf7\xe8\xf5\x20\xeb\xe8\xf7\xe5\xe9" /* "могучих личей" */),
    DATA_COMPGEN(0x0050767c, gArmyNamesPluralBoneDragons, "\xea\xee\xf1\xf2\xff\xed\xfb\xf5\x20\xe4\xf0\xe0\xea\xee\xed\xee\xe2" /* "костяных драконов" */),
    DATA_COMPGEN(0x00507690, gArmyNamesPluralRogues, "\xf0\xe0\xe7\xe1\xee\xe9\xed\xe8\xea\xee\xe2" /* "разбойников" */),
    DATA_COMPGEN(0x0050769c, gArmyNamesPluralNomads, "\xea\xee\xf7\xe5\xe2\xed\xe8\xea\xee\xe2" /* "кочевников" */),
    DATA_COMPGEN(0x005076a8, gArmyNamesPluralGhosts, "\xef\xf0\xe8\xe7\xf0\xe0\xea\xee\xe2" /* "призраков" */),
    DATA_COMPGEN(0x005076b4, gArmyNamesPluralGenies, "\xe4\xe6\xe8\xed\xed\xee\xe2" /* "джиннов" */),
    DATA_COMPGEN(0x005076bc, gArmyNamesPluralMedusas, "\xec\xe5\xe4\xf3\xe7" /* "медуз" */),
    DATA_COMPGEN(0x005076c4, gArmyNamesPluralEarthElementals, "\xe7\xe5\xec\xed\xfb\xf5\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb\xee\xe2" /* "земных элементалов" */),
    DATA_COMPGEN(0x005076d8, gArmyNamesPluralAirElementals, "\xe2\xee\xe7\xe4\xf3\xf8\xed\xfb\xf5\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb\xee\xe2" /* "воздушных элементалов" */),
    DATA_COMPGEN(0x005076f0, gArmyNamesPluralFireElementals, "\xee\xe3\xed\xe5\xed\xed\xfb\xf5\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb\xee\xe2" /* "огненных элементалов" */),
    DATA_COMPGEN(0x00507708, gArmyNamesPluralWaterElementals, "\xe2\xee\xe4\xed\xfb\xf5\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb\xee\xe2" /* "водных элементалов" */)
};
DATA(0x004fdfd8) char* gTerrainNames[IDX(TERRAIN_COUNT)] = {
    DATA_COMPGEN(0x0050771c, gTerrainNamesOcean, "\xc2\xee\xe4\xe0" /* "Вода" */),
    DATA_COMPGEN(0x00507724, gTerrainNamesGrass, "\xd2\xf0\xe0\xe2\xe0" /* "Трава" */),
    DATA_COMPGEN(0x0050772c, gTerrainNamesSnow, "\xd1\xed\xe5\xe3" /* "Снег" */),
    DATA_COMPGEN(0x00507734, gTerrainNamesSwamp, "\xc1\xee\xeb\xee\xf2\xee" /* "Болото" */),
    DATA_COMPGEN(0x0050773c, gTerrainNamesLava, "\xcb\xe0\xe2\xe0" /* "Лава" */),
    DATA_COMPGEN(0x00507744, gTerrainNamesDesert, "\xcf\xf3\xf1\xf2\xfb\xed\xff" /* "Пустыня" */),
    DATA_COMPGEN(0x0050774c, gTerrainNamesDirt, "\xc3\xf0\xff\xe7\xfc" /* "Грязь" */),
    DATA_COMPGEN(0x00507754, gTerrainNamesWasteland, "\xcf\xf3\xf1\xf2\xee\xf8\xfc" /* "Пустошь" */),
    DATA_COMPGEN(0x0050775c, gTerrainNamesBeach, "\xcf\xee\xe1\xe5\xf0\xe5\xe6\xfc\xe5" /* "Побережье" */)
};
DATA(0x004fdffc) char* gResourceNames[RESOURCE_VALUE_COUNT] = {
    DATA_COMPGEN(0x00507768, gResourceNamesWood, "\xc4\xf0\xe5\xe2\xe5\xf1\xe8\xed\xe0" /* "Древесина" */),
    DATA_COMPGEN(0x00507774, gResourceNamesMercury, "\xd0\xf2\xf3\xf2\xfc" /* "Ртуть" */),
    DATA_COMPGEN(0x0050777c, gResourceNamesOre, "\xd0\xf3\xe4\xe0" /* "Руда" */),
    DATA_COMPGEN(0x00507784, gResourceNamesSulfur, "\xd1\xe5\xf0\xe0" /* "Сера" */),
    DATA_COMPGEN(0x0050778c, gResourceNamesCrystal, "\xca\xf0\xe8\xf1\xf2\xe0\xeb\xeb\xfb" /* "Кристаллы" */),
    DATA_COMPGEN(0x00507798, gResourceNamesGems, "\xd1\xe0\xec\xee\xf6\xe2\xe5\xf2\xfb" /* "Самоцветы" */),
    DATA_COMPGEN(0x005077a4, gResourceNamesGold, "\xc7\xee\xeb\xee\xf2\xee" /* "Золото" */)
};
// The localised build names the mine, not the resource it yields, in the
// adventure-map quick info; the English 2.1 tree has no such table and reads
// gResourceNames there. See docs/version-changes.md.
DATA(0x004fe018) char* gMineNames[KB_MINE_NAME_COUNT] = {
    DATA_COMPGEN(0x005077ac, gMineNamesSawmill, "\xcb\xe5\xf1\xee\xef\xe8\xeb\xea\xe0" /* "Лесопилка" */),
    DATA_COMPGEN(0x005077b8, gMineNamesAlchemistLab, "\xcb\xe0\xe1\xee\xf0\xe0\xf2\xee\xf0\xe8\xff\x20\xe0\xeb\xf5\xe8\xec\xe8\xea\xe0" /* "Лаборатория алхимика" */),
    DATA_COMPGEN(0x005077d0, gMineNamesOreMine, "\xd0\xf3\xe4\xed\xe0\xff\x20\xf8\xe0\xf5\xf2\xe0" /* "Рудная шахта" */),
    DATA_COMPGEN(0x005077e0, gMineNamesSulfurMine, "\xd1\xe5\xf0\xed\xe0\xff\x20\xf8\xe0\xf5\xf2\xe0" /* "Серная шахта" */),
    DATA_COMPGEN(0x005077f0, gMineNamesCrystalMine, "\xca\xf0\xe8\xf1\xf2\xe0\xeb\xfc\xed\xe0\xff\x20\xf8\xe0\xf5\xf2\xe0" /* "Кристальная шахта" */),
    DATA_COMPGEN(0x00507804, gMineNamesGemMine, "\xd1\xe0\xec\xee\xf6\xe2\xe5\xf2\xed\xe0\xff\x20\xf8\xe0\xf5\xf2\xe0" /* "Самоцветная шахта" */),
    DATA_COMPGEN(0x00507818, gMineNamesGoldMine, "\xc7\xee\xeb\xee\xf2\xe0\xff\x20\xf8\xe0\xf5\xf2\xe0" /* "Золотая шахта" */)
};
DATA(0x004fe034) char* gQuickViewText[KB_QUICK_VIEW_TEXT_COUNT] = {
    DATA_COMPGEN(0x00526764, gQuickViewTextEmpty0, "" /* "" */),
    DATA_COMPGEN(0x00507828, gQuickViewTextAlchemistLab, "\xcb\xe0\xe1\xee\xf0\xe0\xf2\xee\xf0\xe8\xff\x20\xe0\xeb\xf5\xe8\xec\xe8\xea\xe0" /* "Лаборатория алхимика" */),
    DATA_COMPGEN(0x00507840, gQuickViewTextSign, "\xd3\xea\xe0\xe7\xe0\xf2\xe5\xeb\xfc" /* "Указатель" */),
    DATA_COMPGEN(0x0050784c, gQuickViewTextBuoy, "\xc1\xf3\xe9" /* "Буй" */),
    DATA_COMPGEN(0x00507850, gQuickViewTextSkeleton, "\xd1\xea\xe5\xeb\xe5\xf2" /* "Скелет" */),
    DATA_COMPGEN(0x00507858, gQuickViewTextDaemonCave, "\xcf\xe5\xf9\xe5\xf0\xe0\x20\xe4\xe5\xec\xee\xed\xe0" /* "Пещера демона" */),
    DATA_COMPGEN(0x00507868, gQuickViewTextTreasureChest, "\xcb\xe0\xf0\xe5\xf6\x20\xf1\x20\xf1\xee\xea\xf0\xee\xe2\xe8\xf9\xe0\xec\xe8" /* "Ларец с сокровищами" */),
    DATA_COMPGEN(0x0050787c, gQuickViewTextFaerieRing, "\xca\xee\xeb\xfc\xf6\xee\x20\xf4\xe5\xe9\xf0\xe8" /* "Кольцо фейри" */),
    DATA_COMPGEN(0x0050788c, gQuickViewTextCampfire, "\xca\xee\xf1\xf2\xe5\xf0" /* "Костер" */),
    DATA_COMPGEN(0x00507894, gQuickViewTextFountain, "\xd4\xee\xed\xf2\xe0\xed" /* "Фонтан" */),
    DATA_COMPGEN(0x0050789c, gQuickViewTextGazebo, "\xc1\xe5\xf1\xe5\xe4\xea\xe0" /* "Беседка" */),
    DATA_COMPGEN(0x005078a4, gQuickViewTextAncientLamp, "\xc4\xf0\xe5\xe2\xed\xff\xff\x20\xeb\xe0\xec\xef\xe0" /* "Древняя лампа" */),
    DATA_COMPGEN(0x005078b4, gQuickViewTextGraveyard, "\xca\xeb\xe0\xe4\xe1\xe8\xf9\xe5" /* "Кладбище" */),
    DATA_COMPGEN(0x005078c0, gQuickViewTextArcherSHouse, "\xc4\xee\xec\x20\xf1\xf2\xf0\xe5\xeb\xea\xee\xe2" /* "Дом стрелков" */),
    DATA_COMPGEN(0x005078d0, gQuickViewTextGoblinHut, "\xd5\xe8\xe1\xe0\xf0\xe0\x20\xe3\xee\xe1\xeb\xe8\xed\xe0" /* "Хибара гоблина" */),
    DATA_COMPGEN(0x005078e0, gQuickViewTextDwarfCottage, "\xc8\xe7\xe1\xf3\xf8\xea\xe0\x20\xe3\xed\xee\xec\xee\xe2" /* "Избушка гномов" */),
    DATA_COMPGEN(0x005078f0, gQuickViewTextPeasantHut, "\xd5\xe8\xe6\xe8\xed\xe0\x20\xea\xf0\xe5\xf1\xf2\xfc\xff\xed" /* "Хижина крестьян" */),
    DATA_COMPGEN(0x00507900, gQuickViewTextLogCabin, "\xd5\xe8\xe6\xe8\xed\xe0" /* "Хижина" */),
    DATA_COMPGEN(0x00507908, gQuickViewTextRoad, "\xc4\xee\xf0\xee\xe3\xe0" /* "Дорога" */),
    DATA_COMPGEN(0x00507910, gQuickViewTextEvent, "\xd1\xee\xe1\xfb\xf2\xe8\xe5" /* "Событие" */),
    DATA_COMPGEN(0x00507918, gQuickViewTextDragonCity, "\xc4\xf0\xe0\xea\xee\xed\xe8\xe9\x20\xe3\xee\xf0\xee\xe4" /* "Драконий город" */),
    DATA_COMPGEN(0x00507928, gQuickViewTextLighthouse, "\xcc\xe0\xff\xea" /* "Маяк" */),
    DATA_COMPGEN(0x00507930, gQuickViewTextWaterWheel, "\xc2\xee\xe4\xff\xed\xe0\xff\x20\xec\xe5\xeb\xfc\xed\xe8\xf6\xe0" /* "Водяная мельница" */),
    DATA_COMPGEN(0x00507944, gQuickViewTextMine, "\xd8\xe0\xf5\xf2\xe0" /* "Шахта" */),
    DATA_COMPGEN(0x0050794c, gQuickViewTextArmyCamp, "\xc1\xe8\xe2\xf3\xe0\xea" /* "Бивуак" */),
    DATA_COMPGEN(0x00507954, gQuickViewTextObelisk, "\xce\xe1\xe5\xeb\xe8\xf1\xea" /* "Обелиск" */),
    DATA_COMPGEN(0x0050795c, gQuickViewTextOasis, "\xce\xe0\xe7\xe8\xf1" /* "Оазис" */),
    DATA_COMPGEN(0x00507964, gQuickViewTextResource, "\xd0\xe5\xf1\xf3\xf0\xf1\xfb" /* "Ресурсы" */),
    DATA_COMPGEN(0x00526768, gQuickViewTextEmpty28, "" /* "" */),
    DATA_COMPGEN(0x0050796c, gQuickViewTextSawmill, "\xcb\xe5\xf1\xee\xef\xe8\xeb\xea\xe0" /* "Лесопилка" */),
    DATA_COMPGEN(0x00507978, gQuickViewTextOracle, "\xce\xf0\xe0\xea\xf3\xeb" /* "Оракул" */),
    DATA_COMPGEN(0x00507980, gQuickViewTextShrineOfTheFirstCircle, "\xd1\xe2\xff\xf2\xe8\xeb\xe8\xf9\xe5\x20\x31\x2d\xe3\xee\x20\xca\xf0\xf3\xe3\xe0" /* "Святилище 1-го Круга" */),
    DATA_COMPGEN(0x00507998, gQuickViewTextShipwreck, "\xca\xee\xf0\xe0\xe1\xeb\xe5\xea\xf0\xf3\xf8\xe5\xed\xe8\xe5" /* "Кораблекрушение" */),
    DATA_COMPGEN(0x005079a8, gQuickViewTextSeaChest, "\xd1\xf3\xed\xe4\xf3\xea" /* "Сундук" */),
    DATA_COMPGEN(0x005079b0, gQuickViewTextDesertTent, "\xd8\xe0\xf2\xe5\xf0" /* "Шатер" */),
    DATA_COMPGEN(0x005079b8, gQuickViewTextTown, "\xc3\xee\xf0\xee\xe4" /* "Город" */),
    DATA_COMPGEN(0x005079c0, gQuickViewTextStoneLiths, "\xcc\xe5\xed\xe3\xe8\xf0" /* "Менгир" */),
    DATA_COMPGEN(0x005079c8, gQuickViewTextWagonCamp, "\xd4\xf3\xf0\xe3\xee\xed\xfb" /* "Фургоны" */),
    DATA_COMPGEN(0x005079d0, gQuickViewTextWell, "\xca\xee\xeb\xee\xe4\xe5\xf6" /* "Колодец" */),
    DATA_COMPGEN(0x005079d8, gQuickViewTextWhirlpool, "\xc2\xee\xe4\xee\xe2\xee\xf0\xee\xf2" /* "Водоворот" */),
    DATA_COMPGEN(0x005079e4, gQuickViewTextWindmill, "\xc2\xe5\xf2\xf0\xff\xed\xe0\xff\x20\xec\xe5\xeb\xfc\xed\xe8\xf6\xe0" /* "Ветряная мельница" */),
    DATA_COMPGEN(0x005079f8, gQuickViewTextArtifact, "\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2" /* "Артефакт" */),
    DATA_COMPGEN(0x00507a04, gQuickViewTextHero, "\xc3\xe5\xf0\xee\xe9" /* "Герой" */),
    DATA_COMPGEN(0x00507a0c, gQuickViewTextBoat, "\xca\xee\xf0\xe0\xe1\xeb\xfc" /* "Корабль" */),
    DATA_COMPGEN(0x00507a14, gQuickViewTextEmpty44, "\xcc\xee\xe3\xf3\xf9\xe5\xf1\xf2\xe2\xe5\xed\xed\xfb\xe9\x20\xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2" /* "Могущественный артефакт" */),
    DATA_COMPGEN(0x00507a2c, gQuickViewTextRandomArtifact, "\xd1\xeb\xf3\xf7\xe0\xe9\xed\xfb\xe9\x20\xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2" /* "Случайный артефакт" */),
    DATA_COMPGEN(0x00507a40, gQuickViewTextRandomResource, "\xd1\xeb\xf3\xf7\xe0\xe9\xed\xfb\xe9\x20\xf0\xe5\xf1\xf3\xf0\xf1" /* "Случайный ресурс" */),
    DATA_COMPGEN(0x00507a54, gQuickViewTextRandomMonster, "\xd1\xeb\xf3\xf7\xe0\xe9\xed\xfb\xe9\x20\xec\xee\xed\xf1\xf2\xf0" /* "Случайный монстр" */),
    DATA_COMPGEN(0x00507a68, gQuickViewTextRandomTown, "\xd1\xeb\xf3\xf7\xe0\xe9\xed\xfb\xe9\x20\xe3\xee\xf0\xee\xe4" /* "Случайный город" */),
    DATA_COMPGEN(0x00507a78, gQuickViewTextRandomCastle, "\xd1\xeb\xf3\xf7\xe0\xe9\xed\xfb\xe9\x20\xe7\xe0\xec\xee\xea" /* "Случайный замок" */),
    DATA_COMPGEN(0x0052676c, gQuickViewTextEmpty50, "" /* "" */),
    DATA_COMPGEN(0x00507a88, gQuickViewTextRandomMonsterWeak, "\xd1\xeb\xf3\xf7\xe0\xe9\xed\xfb\xe9\x20\xec\xee\xed\xf1\xf2\xf0\x20\x2d\x20\xf1\xeb\xe0\xe1\xfb\xe9" /* "Случайный монстр - слабый" */),
    DATA_COMPGEN(0x00507aa4, gQuickViewTextRandomMonsterMedium, "\xd1\xeb\xf3\xf7\xe0\xe9\xed\xfb\xe9\x20\xec\xee\xed\xf1\xf2\xf0\x20\x2d\x20\xf1\xf0\xe5\xe4\xed\xe8\xe9" /* "Случайный монстр - средний" */),
    DATA_COMPGEN(0x00507ac0, gQuickViewTextRandomMonsterStrong, "\xd1\xeb\xf3\xf7\xe0\xe9\xed\xfb\xe9\x20\xec\xee\xed\xf1\xf2\xf0\x20\x2d\x20\xf1\xe8\xeb\xfc\xed\xfb\xe9" /* "Случайный монстр - сильный" */),
    DATA_COMPGEN(0x00507adc, gQuickViewTextRandomMonsterVeryStrong, "\xd1\xeb\xf3\xf7\xe0\xe9\xed\xfb\xe9\x20\xec\xee\xed\xf1\xf2\xf0\x20\x2d\x20\xee\xf7\xe5\xed\xfc\x20\xf1\xe8\xeb\xfc\xed\xfb\xe9" /* "Случайный монстр - очень сильный" */),
    DATA_COMPGEN(0x00507b00, gQuickViewTextRandomHero, "\xd1\xeb\xf3\xf7\xe0\xe9\xed\xfb\xe9\x20\xe3\xe5\xf0\xee\xe9" /* "Случайный герой" */),
    DATA_COMPGEN(0x00507b10, gQuickViewTextNothingSpecial, "\xcd\xe8\xf7\xe5\xe3\xee\x20\xee\xf1\xee\xe1\xe5\xed\xed\xee\xe3\xee" /* "Ничего особенного" */),
    DATA_COMPGEN(0x00526770, gQuickViewTextEmpty57, "" /* "" */),
    DATA_COMPGEN(0x00507b24, gQuickViewTextWatchTower, "\xd1\xf2\xee\xf0\xee\xe6\xe5\xe2\xe0\xff\x20\xe2\xfb\xf8\xea\xe0" /* "Сторожевая вышка" */),
    DATA_COMPGEN(0x00507b38, gQuickViewTextTreeHouse, "\xc4\xf0\xe5\xe2\xee\x2d\xe3\xee\xf0\xee\xe4" /* "Древо-город" */),
    DATA_COMPGEN(0x00507b44, gQuickViewTextTreeCity, "\xc4\xf0\xe5\xe2\xee\x2d\xe3\xee\xf0\xee\xe4" /* "Древо-город" */),
    DATA_COMPGEN(0x00507b50, gQuickViewTextRuins, "\xd0\xf3\xe8\xed\xfb" /* "Руины" */),
    DATA_COMPGEN(0x00507b58, gQuickViewTextFort, "\xd4\xee\xf0\xf2" /* "Форт" */),
    DATA_COMPGEN(0x00507b60, gQuickViewTextTradingPost, "\xc1\xe0\xe7\xe0\xf0" /* "Базар" */),
    DATA_COMPGEN(0x00507b68, gQuickViewTextAbandonedMine, "\xc7\xe0\xe1\xf0\xee\xf8\xe5\xed\xed\xe0\xff\x20\xf8\xe0\xf5\xf2\xe0" /* "Заброшенная шахта" */),
    DATA_COMPGEN(0x00507b7c, gQuickViewTextDwarfCabin, "\xcb\xe0\xf7\xf3\xe3\xe0\x20\xe3\xed\xee\xec\xee\xe2" /* "Лачуга гномов" */),
    DATA_COMPGEN(0x00507b8c, gQuickViewTextStandingStones, "\xd1\xf2\xee\xff\xf7\xe8\xe5\x20\xea\xe0\xec\xed\xe8" /* "Стоячие камни" */),
    DATA_COMPGEN(0x00507b9c, gQuickViewTextIdol, "\xc8\xe4\xee\xeb" /* "Идол" */),
    DATA_COMPGEN(0x00507ba4, gQuickViewTextTreeOfKnowledge, "\xc4\xf0\xe5\xe2\xee\x20\xe7\xed\xe0\xed\xe8\xff" /* "Древо знания" */),
    DATA_COMPGEN(0x00507bb4, gQuickViewTextWitchDoctorSHut, "\xd5\xe8\xe6\xe8\xed\xe0\x20\xe2\xe5\xe4\xfc\xec\xfb" /* "Хижина ведьмы" */),
    DATA_COMPGEN(0x00507bc4, gQuickViewTextTemple, "\xd5\xf0\xe0\xec" /* "Храм" */),
    DATA_COMPGEN(0x00507bcc, gQuickViewTextHillFort, "\xd4\xee\xf0\xf2\x20\xed\xe0\x20\xf5\xee\xeb\xec\xe5" /* "Форт на холме" */),
    DATA_COMPGEN(0x00507bdc, gQuickViewTextHalflingHole, "\xcd\xee\xf0\xe0\x20\xef\xee\xeb\xf3\xf0\xee\xf1\xeb\xe8\xea\xe0" /* "Нора полурослика" */),
    DATA_COMPGEN(0x00507bf0, gQuickViewTextMercenaryCamp, "\xcb\xe0\xe3\xe5\xf0\xfc\x20\xed\xe0\xe5\xec\xed\xe8\xea\xee\xe2" /* "Лагерь наемников" */),
    DATA_COMPGEN(0x00507c04, gQuickViewTextShrineOfTheSecondCircle, "\xd1\xe2\xff\xf2\xe8\xeb\xe8\xf9\xe5\x20\x32\x2d\xe3\xee\x20\xca\xf0\xf3\xe3\xe0" /* "Святилище 2-го Круга" */),
    DATA_COMPGEN(0x00507c1c, gQuickViewTextShrineOfTheThirdCircle, "\xd1\xe2\xff\xf2\xe8\xeb\xe8\xf9\xe5\x20\x33\x2d\xe3\xee\x20\xca\xf0\xf3\xe3\xe0" /* "Святилище 3-го Круга" */),
    DATA_COMPGEN(0x00507c34, gQuickViewTextPyramid, "\xcf\xe8\xf0\xe0\xec\xe8\xe4\xe0" /* "Пирамида" */),
    DATA_COMPGEN(0x00507c40, gQuickViewTextCityOfTheDead, "\xc3\xee\xf0\xee\xe4\x20\xec\xe5\xf0\xf2\xe2\xfb\xf5" /* "Город мертвых" */),
    DATA_COMPGEN(0x00507c50, gQuickViewTextExcavation, "\xca\xee\xf2\xeb\xee\xe2\xe0\xed" /* "Котлован" */),
    DATA_COMPGEN(0x00507c5c, gQuickViewTextSphinx, "\xd1\xf4\xe8\xed\xea\xf1" /* "Сфинкс" */),
    DATA_COMPGEN(0x00507c64, gQuickViewTextWagon, "\xd2\xe5\xeb\xe5\xe6\xea\xe0" /* "Тележка" */),
    DATA_COMPGEN(0x00507c6c, gQuickViewTextTarPit, "\xd1\xec\xee\xeb\xff\xed\xe0\xff\x20\xff\xec\xe0" /* "Смоляная яма" */),
    DATA_COMPGEN(0x00507c7c, gQuickViewTextArtesianSpring, "\xc0\xf0\xf2\xe5\xe7\xe8\xe0\xed\xf1\xea\xe8\xe9\x20\xe8\xf1\xf2\xee\xf7\xed\xe8\xea" /* "Артезианский источник" */),
    DATA_COMPGEN(0x00507c94, gQuickViewTextTrollBridge, "\xcc\xee\xf1\xf2\x20\xf2\xf0\xee\xeb\xeb\xe5\xe9" /* "Мост троллей" */),
    DATA_COMPGEN(0x00507ca4, gQuickViewTextWateringHole, "\xcf\xf0\xee\xec\xee\xe8\xed\xe0" /* "Промоина" */),
    DATA_COMPGEN(0x00507cb0, gQuickViewTextWitchSHut, "\xd5\xe8\xe6\xe8\xed\xe0\x20\xe2\xe5\xe4\xfc\xec\xfb" /* "Хижина ведьмы" */),
    DATA_COMPGEN(0x00507cc0, gQuickViewTextXanadu, "\xca\xf1\xe0\xed\xe0\xe4\xf3" /* "Ксанаду" */),
    DATA_COMPGEN(0x00507cc8, gQuickViewTextCave, "\xcf\xe5\xf9\xe5\xf0\xe0" /* "Пещера" */),
    DATA_COMPGEN(0x00507cd0, gQuickViewTextLeanTo, "\xcd\xe0\xe2\xe5\xf1" /* "Навес" */),
    DATA_COMPGEN(0x00507cd8, gQuickViewTextMagellanSMaps, "\xca\xe0\xf0\xf2\xfb\x20\xcc\xe0\xe3\xe5\xeb\xeb\xe0\xed\xe0" /* "Карты Магеллана" */),
    DATA_COMPGEN(0x00507ce8, gQuickViewTextFlotsam, "\xce\xe1\xeb\xee\xec\xea\xe8" /* "Обломки" */),
    DATA_COMPGEN(0x00507cf0, gQuickViewTextDerelictShip, "\xc7\xe0\xe1\xf0\xee\xf8\xe5\xed\xed\xfb\xe9\x20\xea\xee\xf0\xe0\xe1\xeb\xfc" /* "Заброшенный корабль" */),
    DATA_COMPGEN(0x00507d04, gQuickViewTextShipwreckSurvivor, "\xcf\xee\xf2\xe5\xf0\xef\xe5\xe2\xf8\xe8\xe9\x20\xea\xee\xf0\xe0\xe1\xeb\xe5\xea\xf0\xf3\xf8\xe5\xed\xe8\xe5" /* "Потерпевший кораблекрушение" */),
    DATA_COMPGEN(0x00507d20, gQuickViewTextBottle, "\xc1\xf3\xf2\xfb\xeb\xea\xe0" /* "Бутылка" */),
    DATA_COMPGEN(0x00507d28, gQuickViewTextMagicWell, "\xc2\xee\xeb\xf8\xe5\xe1\xed\xfb\xe9\x20\xea\xee\xeb\xee\xe4\xe5\xf6" /* "Волшебный колодец" */),
    DATA_COMPGEN(0x00507d3c, gQuickViewTextMagicGarden, "\xc2\xee\xeb\xf8\xe5\xe1\xed\xfb\xe9\x20\xf1\xe0\xe4" /* "Волшебный сад" */),
    DATA_COMPGEN(0x00507d4c, gQuickViewTextObservationTower, "\xce\xe1\xe7\xee\xf0\xed\xe0\xff\x20\xe1\xe0\xf8\xed\xff" /* "Обзорная башня" */),
    DATA_COMPGEN(0x00507d5c, gQuickViewTextFreemanSFoundry, "\xcb\xe8\xf2\xe5\xe9\xed\xfb\xe9\x20\xf6\xe5\xf5" /* "Литейный цех" */),
    DATA_COMPGEN(0x00507d6c, gQuickViewTextStream, "\xcf\xee\xf2\xee\xea\xe8" /* "Потоки" */),
    DATA_COMPGEN(0x00507d74, gQuickViewTextTrees, "\xc4\xe5\xf0\xe5\xe2\xfc\xff" /* "Деревья" */),
    DATA_COMPGEN(0x00507d7c, gQuickViewTextMountains, "\xc3\xee\xf0\xfb" /* "Горы" */),
    DATA_COMPGEN(0x00507d84, gQuickViewTextVolcano, "\xc2\xf3\xeb\xea\xe0\xed" /* "Вулкан" */),
    DATA_COMPGEN(0x00507d8c, gQuickViewTextFlowers, "\xd6\xe2\xe5\xf2\xfb" /* "Цветы" */),
    DATA_COMPGEN(0x00507d94, gQuickViewTextRock, "\xca\xe0\xec\xe5\xed\xfc" /* "Камень" */),
    DATA_COMPGEN(0x00507d9c, gQuickViewTextLake, "\xce\xe7\xe5\xf0\xee" /* "Озеро" */),
    DATA_COMPGEN(0x00507da4, gQuickViewTextMandrake, "\xcc\xe0\xed\xe4\xf0\xe0\xe3\xee\xf0\xe0" /* "Мандрагора" */),
    DATA_COMPGEN(0x00507db0, gQuickViewTextDeadTree, "\xcc\xe5\xf0\xf2\xe2\xee\xe5\x20\xe4\xe5\xf0\xe5\xe2\xee" /* "Мертвое дерево" */),
    DATA_COMPGEN(0x00507dc0, gQuickViewTextStump, "\xcf\xe5\xed\xfc" /* "Пень" */),
    DATA_COMPGEN(0x00507dc8, gQuickViewTextCrater, "\xca\xf0\xe0\xf2\xe5\xf0" /* "Кратер" */),
    DATA_COMPGEN(0x00507dd0, gQuickViewTextCactus, "\xca\xe0\xea\xf2\xf3\xf1" /* "Кактус" */),
    DATA_COMPGEN(0x00507dd8, gQuickViewTextMound, "\xca\xf3\xf0\xe3\xe0\xed" /* "Курган" */),
    DATA_COMPGEN(0x00507de0, gQuickViewTextDune, "\xc4\xfe\xed\xe0" /* "Дюна" */),
    DATA_COMPGEN(0x00507de8, gQuickViewTextLavaPool, "\xcb\xe0\xe2\xee\xe2\xfb\xe9\x20\xe1\xe0\xf1\xf1\xe5\xe9\xed" /* "Лавовый бассейн" */),
    DATA_COMPGEN(0x00507df8, gQuickViewTextShrub, "\xca\xf3\xf1\xf2" /* "Куст" */),
    DATA_COMPGEN(0x00507e00, gQuickViewTextHole, "\xc4\xfb\xf0\xe0" /* "Дыра" */),
    DATA_COMPGEN(0x00507e08, gQuickViewTextOutcropping, "\xcf\xeb\xe0\xf1\xf2" /* "Пласт" */),
    DATA_COMPGEN(0x00507e10, gQuickViewTextRandomArtifactTreasure, "\xd1\xeb\xf3\xf7\xe0\xe9\xed\xfb\xe9\x20\xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\x2d\x20\xf1\xee\xea\xf0\xee\xe2\xe8\xf9\xe5" /* "Случайный артефакт - сокровище" */),
    DATA_COMPGEN(0x00507e30, gQuickViewTextRandomArtifactMinor, "\xd1\xeb\xf3\xf7\xe0\xe9\xed\xfb\xe9\x20\xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\x2d\x20\xee\xe1\xfb\xf7\xed\xfb\xe9" /* "Случайный артефакт - обычный" */),
    DATA_COMPGEN(0x00507e50, gQuickViewTextRandomArtifactMajor, "\xd1\xeb\xf3\xf7\xe0\xe9\xed\xfb\xe9\x20\xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\x2d\x20\xf6\xe5\xed\xed\xfb\xe9" /* "Случайный артефакт - ценный" */),
    DATA_COMPGEN(0x00507e6c, gQuickViewTextFormatBarrier, "\x25\x73\x20\xc1\xe0\xf0\xfc\xe5\xf0" /* "%s Барьер" */),
    DATA_COMPGEN(0x00507e78, gQuickViewTextFormatTravellerSTent, "\x25\x73\x20\xd8\xe0\xf2\xe5\xf0\x20\xef\xf3\xf2\xed\xe8\xea\xe0" /* "%s Шатер путника" */),
    DATA_COMPGEN(0x00507e8c, gQuickViewTextFormat121, "\x25\x73" /* "%s" */),
    DATA_COMPGEN(0x00507e90, gQuickViewTextFormat122, "\x25\x73" /* "%s" */),
    DATA_COMPGEN(0x00507e94, gQuickViewTextJail, "\xd2\xe5\xec\xed\xe8\xf6\xe0" /* "Темница" */)
};
DATA(0x004fe224) char* gEventText[KB_EVENT_TEXT_TABLE_COUNT] = {
    // Алхимик\n\nВы стали хозяином лаборатории местного алхимика. Она будет приносить вам по одной
    // единице ртути в день.
    DATA_COMPGEN(0x00507e9c, gEventTextAlchemist0,
        "\xc0\xeb\xf5\xe8\xec\xe8\xea\x0a\x0a\xc2\xfb\x20\xf1\xf2\xe0\xeb\xe8\x20\xf5\xee\xe7\xff\xe8\xed"
        "\xee\xec\x20\xeb\xe0\xe1\xee\xf0\xe0\xf2\xee\xf0\xe8\xe8\x20\xec\xe5\xf1\xf2\xed\xee\xe3\xee\x20"
        "\xe0\xeb\xf5\xe8\xec\xe8\xea\xe0\x2e\x20\xce\xed\xe0\x20\xe1\xf3\xe4\xe5\xf2\x20\xef\xf0\xe8\xed"
        "\xee\xf1\xe8\xf2\xfc\x20\xe2\xe0\xec\x20\xef\xee\x20\xee\xe4\xed\xee\xe9\x20\xe5\xe4\xe8\xed\xe8"
        "\xf6\xe5\x20\xf0\xf2\xf3\xf2\xe8\x20\xe2\x20\xe4\xe5\xed\xfc\x2e"),
    // Указатель\n\nНа указателе написано:\n\n%s находится неподалеку отсюда.
    DATA_COMPGEN(0x00507f10, gEventTextSignpost1,
        "\xd3\xea\xe0\xe7\xe0\xf2\xe5\xeb\xfc\x0a\x0a\xcd\xe0\x20\xf3\xea\xe0\xe7\xe0\xf2\xe5\xeb\xe5\x20"
        "\xed\xe0\xef\xe8\xf1\xe0\xed\xee\x3a\x0a\x0a\x25\x73\x20\xed\xe0\xf5\xee\xe4\xe8\xf2\xf1\xff\x20"
        "\xed\xe5\xef\xee\xe4\xe0\xeb\xe5\xea\xf3\x20\xee\xf2\xf1\xfe\xe4\xe0\x2e"),
    // Буй\n\nВаши спутники замечают морской буй. Он указывает верный курс.
    DATA_COMPGEN(0x00507f54, gEventTextBuoy2,
        "\xc1\xf3\xe9\x0a\x0a\xc2\xe0\xf8\xe8\x20\xf1\xef\xf3\xf2\xed\xe8\xea\xe8\x20\xe7\xe0\xec\xe5\xf7"
        "\xe0\xfe\xf2\x20\xec\xee\xf0\xf1\xea\xee\xe9\x20\xe1\xf3\xe9\x2e\x20\xce\xed\x20\xf3\xea\xe0\xe7"
        "\xfb\xe2\xe0\xe5\xf2\x20\xe2\xe5\xf0\xed\xfb\xe9\x20\xea\xf3\xf0\xf1\x2e"),
    // Буй\n\nВаши спутники замечают морской буй. Он указывает верный курс, и это повышает их боевой
    // дух.
    DATA_COMPGEN(0x00507f98, gEventTextBuoy3,
        "\xc1\xf3\xe9\x0a\x0a\xc2\xe0\xf8\xe8\x20\xf1\xef\xf3\xf2\xed\xe8\xea\xe8\x20\xe7\xe0\xec\xe5\xf7"
        "\xe0\xfe\xf2\x20\xec\xee\xf0\xf1\xea\xee\xe9\x20\xe1\xf3\xe9\x2e\x20\xce\xed\x20\xf3\xea\xe0\xe7"
        "\xfb\xe2\xe0\xe5\xf2\x20\xe2\xe5\xf0\xed\xfb\xe9\x20\xea\xf3\xf0\xf1\x2c\x20\xe8\x20\xfd\xf2\xee"
        "\x20\xef\xee\xe2\xfb\xf8\xe0\xe5\xf2\x20\xe8\xf5\x20\xe1\xee\xe5\xe2\xee\xe9\x20\xe4\xf3\xf5\x2e"),
    DATA_COMPGEN(0x00526774, gEventTextEmpty4, ""),
    DATA_COMPGEN(0x00526778, gEventTextEmpty5, ""),
    DATA_COMPGEN(0x0052677c, gEventTextEmpty6, ""),
    DATA_COMPGEN(0x00526780, gEventTextEmpty7, ""),
    DATA_COMPGEN(0x00526784, gEventTextEmpty8, ""),
    DATA_COMPGEN(0x00526788, gEventTextEmpty9, ""),
    DATA_COMPGEN(0x0052678c, gEventTextEmpty10, ""),
    DATA_COMPGEN(0x00526790, gEventTextEmpty11, ""),
    // Кольцо фейри\n\nВаше войско вступает внутрь кольца фейри, но ничего не происходит.
    DATA_COMPGEN(0x00507ffc, gEventTextFaerieRing12,
        "\xca\xee\xeb\xfc\xf6\xee\x20\xf4\xe5\xe9\xf0\xe8\x0a\x0a\xc2\xe0\xf8\xe5\x20\xe2\xee\xe9\xf1\xea"
        "\xee\x20\xe2\xf1\xf2\xf3\xef\xe0\xe5\xf2\x20\xe2\xed\xf3\xf2\xf0\xfc\x20\xea\xee\xeb\xfc\xf6\xe0"
        "\x20\xf4\xe5\xe9\xf0\xe8\x2c\x20\xed\xee\x20\xed\xe8\xf7\xe5\xe3\xee\x20\xed\xe5\x20\xef\xf0\xee"
        "\xe8\xf1\xf5\xee\xe4\xe8\xf2\x2e"),
    // Кольцо фейри\n\nВаше войско вступает внутрь кольца фейри, чары которого принесут вам удачу в
    // грядущем сражении.
    DATA_COMPGEN(0x00508050, gEventTextFaerieRing13,
        "\xca\xee\xeb\xfc\xf6\xee\x20\xf4\xe5\xe9\xf0\xe8\x0a\x0a\xc2\xe0\xf8\xe5\x20\xe2\xee\xe9\xf1\xea"
        "\xee\x20\xe2\xf1\xf2\xf3\xef\xe0\xe5\xf2\x20\xe2\xed\xf3\xf2\xf0\xfc\x20\xea\xee\xeb\xfc\xf6\xe0"
        "\x20\xf4\xe5\xe9\xf0\xe8\x2c\x20\xf7\xe0\xf0\xfb\x20\xea\xee\xf2\xee\xf0\xee\xe3\xee\x20\xef\xf0"
        "\xe8\xed\xe5\xf1\xf3\xf2\x20\xe2\xe0\xec\x20\xf3\xe4\xe0\xf7\xf3\x20\xe2\x20\xe3\xf0\xff\xe4\xf3"
        "\xf9\xe5\xec\x20\xf1\xf0\xe0\xe6\xe5\xed\xe8\xe8\x2e"),
    // Костер\n\nОбыскав вражеский лагерь, вы находите спрятанный клад.
    DATA_COMPGEN(0x005080c0, gEventTextCampfire14,
        "\xca\xee\xf1\xf2\xe5\xf0\x0a\x0a\xce\xe1\xfb\xf1\xea\xe0\xe2\x20\xe2\xf0\xe0\xe6\xe5\xf1\xea\xe8"
        "\xe9\x20\xeb\xe0\xe3\xe5\xf0\xfc\x2c\x20\xe2\xfb\x20\xed\xe0\xf5\xee\xe4\xe8\xf2\xe5\x20\xf1\xef"
        "\xf0\xff\xf2\xe0\xed\xed\xfb\xe9\x20\xea\xeb\xe0\xe4\x2e"),
    // Фонтан\n\nВы припадаете к струям волшебного фонтана, но ничего не происходит.
    DATA_COMPGEN(0x00508100, gEventTextFountain15,
        "\xd4\xee\xed\xf2\xe0\xed\x0a\x0a\xc2\xfb\x20\xef\xf0\xe8\xef\xe0\xe4\xe0\xe5\xf2\xe5\x20\xea\x20"
        "\xf1\xf2\xf0\xf3\xff\xec\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xee\xe3\xee\x20\xf4\xee\xed\xf2\xe0\xed"
        "\xe0\x2c\x20\xed\xee\x20\xed\xe8\xf7\xe5\xe3\xee\x20\xed\xe5\x20\xef\xf0\xee\xe8\xf1\xf5\xee\xe4"
        "\xe8\xf2\x2e"),
    // Фонтан\n\nБлагоуханная влага волшебного фонтана принесет вам удачу в грядущем сражении.
    DATA_COMPGEN(0x0050814c, gEventTextFountain16,
        "\xd4\xee\xed\xf2\xe0\xed\x0a\x0a\xc1\xeb\xe0\xe3\xee\xf3\xf5\xe0\xed\xed\xe0\xff\x20\xe2\xeb\xe0"
        "\xe3\xe0\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xee\xe3\xee\x20\xf4\xee\xed\xf2\xe0\xed\xe0\x20\xef\xf0"
        "\xe8\xed\xe5\xf1\xe5\xf2\x20\xe2\xe0\xec\x20\xf3\xe4\xe0\xf7\xf3\x20\xe2\x20\xe3\xf0\xff\xe4\xf3"
        "\xf9\xe5\xec\x20\xf1\xf0\xe0\xe6\xe5\xed\xe8\xe8\x2e"),
    // Беседка\n\nНа ступенях беседки появляется старый рыцарь. \"Мне жаль, храбрый воин, но я уже
    // научил тебя всему, что знаю сам.\"
    DATA_COMPGEN(0x005081a4, gEventTextGazebo17,
        "\xc1\xe5\xf1\xe5\xe4\xea\xe0\x0a\x0a\xcd\xe0\x20\xf1\xf2\xf3\xef\xe5\xed\xff\xf5\x20\xe1\xe5\xf1"
        "\xe5\xe4\xea\xe8\x20\xef\xee\xff\xe2\xeb\xff\xe5\xf2\xf1\xff\x20\xf1\xf2\xe0\xf0\xfb\xe9\x20\xf0"
        "\xfb\xf6\xe0\xf0\xfc\x2e\x20\x22\xcc\xed\xe5\x20\xe6\xe0\xeb\xfc\x2c\x20\xf5\xf0\xe0\xe1\xf0\xfb"
        "\xe9\x20\xe2\xee\xe8\xed\x2c\x20\xed\xee\x20\xff\x20\xf3\xe6\xe5\x20\xed\xe0\xf3\xf7\xe8\xeb\x20"
        "\xf2\xe5\xe1\xff\x20\xe2\xf1\xe5\xec\xf3\x2c\x20\xf7\xf2\xee\x20\xe7\xed\xe0\xfe\x20\xf1\xe0\xec"
        "\x2e\x22"),
    // Беседка\n\nНа ступенях беседки появляется старый рыцарь. \"О храбрый воин, я научу тебя всему,
    // что знаю сам; пусть мой опыт поможет тебе в твоих странствиях.\"
    DATA_COMPGEN(0x00508220, gEventTextGazebo18,
        "\xc1\xe5\xf1\xe5\xe4\xea\xe0\x0a\x0a\xcd\xe0\x20\xf1\xf2\xf3\xef\xe5\xed\xff\xf5\x20\xe1\xe5\xf1"
        "\xe5\xe4\xea\xe8\x20\xef\xee\xff\xe2\xeb\xff\xe5\xf2\xf1\xff\x20\xf1\xf2\xe0\xf0\xfb\xe9\x20\xf0"
        "\xfb\xf6\xe0\xf0\xfc\x2e\x20\x22\xce\x20\xf5\xf0\xe0\xe1\xf0\xfb\xe9\x20\xe2\xee\xe8\xed\x2c\x20"
        "\xff\x20\xed\xe0\xf3\xf7\xf3\x20\xf2\xe5\xe1\xff\x20\xe2\xf1\xe5\xec\xf3\x2c\x20\xf7\xf2\xee\x20"
        "\xe7\xed\xe0\xfe\x20\xf1\xe0\xec\x3b\x20\xef\xf3\xf1\xf2\xfc\x20\xec\xee\xe9\x20\xee\xef\xfb\xf2"
        "\x20\xef\xee\xec\xee\xe6\xe5\xf2\x20\xf2\xe5\xe1\xe5\x20\xe2\x20\xf2\xe2\xee\xe8\xf5\x20\xf1\xf2"
        "\xf0\xe0\xed\xf1\xf2\xe2\xe8\xff\xf5\x2e\x22"),
    // Лампа джинна\n\nВы находите засыпанную землей помятую и закопченную лампа. Хотите ее потереть?
    DATA_COMPGEN(0x005082bc, gEventTextGenieLamp19,
        "\xcb\xe0\xec\xef\xe0\x20\xe4\xe6\xe8\xed\xed\xe0\x0a\x0a\xc2\xfb\x20\xed\xe0\xf5\xee\xe4\xe8\xf2"
        "\xe5\x20\xe7\xe0\xf1\xfb\xef\xe0\xed\xed\xf3\xfe\x20\xe7\xe5\xec\xeb\xe5\xe9\x20\xef\xee\xec\xff"
        "\xf2\xf3\xfe\x20\xe8\x20\xe7\xe0\xea\xee\xef\xf7\xe5\xed\xed\xf3\xfe\x20\xeb\xe0\xec\xef\xe0\x2e"
        "\x20\xd5\xee\xf2\xe8\xf2\xe5\x20\xe5\xe5\x20\xef\xee\xf2\xe5\xf0\xe5\xf2\xfc\x3f"),
    // Кладбище\n\nВы осторожно приближаетесь к захоронению древних воинов. Хотите вскрыть их могилы?
    DATA_COMPGEN(0x0050831c, gEventTextGraveyard20,
        "\xca\xeb\xe0\xe4\xe1\xe8\xf9\xe5\x0a\x0a\xc2\xfb\x20\xee\xf1\xf2\xee\xf0\xee\xe6\xed\xee\x20\xef"
        "\xf0\xe8\xe1\xeb\xe8\xe6\xe0\xe5\xf2\xe5\xf1\xfc\x20\xea\x20\xe7\xe0\xf5\xee\xf0\xee\xed\xe5\xed"
        "\xe8\xfe\x20\xe4\xf0\xe5\xe2\xed\xe8\xf5\x20\xe2\xee\xe8\xed\xee\xe2\x2e\x20\xd5\xee\xf2\xe8\xf2"
        "\xe5\x20\xe2\xf1\xea\xf0\xfb\xf2\xfc\x20\xe8\xf5\x20\xec\xee\xe3\xe8\xeb\xfb\x3f"),
    // Одержав победу над зомби, вы несколько часов подряд обыскиваете могилы, но ничего не находите.
    // Ваш недостойный поступок отрицательно влияет на боевой дух войска.
    DATA_COMPGEN(0x0050837c, gEventTextUponDefeatingTheZombiesYou21,
        "\xce\xe4\xe5\xf0\xe6\xe0\xe2\x20\xef\xee\xe1\xe5\xe4\xf3\x20\xed\xe0\xe4\x20\xe7\xee\xec\xe1\xe8"
        "\x2c\x20\xe2\xfb\x20\xed\xe5\xf1\xea\xee\xeb\xfc\xea\xee\x20\xf7\xe0\xf1\xee\xe2\x20\xef\xee\xe4"
        "\xf0\xff\xe4\x20\xee\xe1\xfb\xf1\xea\xe8\xe2\xe0\xe5\xf2\xe5\x20\xec\xee\xe3\xe8\xeb\xfb\x2c\x20"
        "\xed\xee\x20\xed\xe8\xf7\xe5\xe3\xee\x20\xed\xe5\x20\xed\xe0\xf5\xee\xe4\xe8\xf2\xe5\x2e\x20\xc2"
        "\xe0\xf8\x20\xed\xe5\xe4\xee\xf1\xf2\xee\xe9\xed\xfb\xe9\x20\xef\xee\xf1\xf2\xf3\xef\xee\xea\x20"
        "\xee\xf2\xf0\xe8\xf6\xe0\xf2\xe5\xeb\xfc\xed\xee\x20\xe2\xeb\xe8\xff\xe5\xf2\x20\xed\xe0\x20\xe1"
        "\xee\xe5\xe2\xee\xe9\x20\xe4\xf3\xf5\x20\xe2\xee\xe9\xf1\xea\xe0\x2e"),
    // Одержав победу над зомби, вы обыскиваете могилы и удаляетесь с находкой!
    DATA_COMPGEN(0x00508420, gEventTextUponDefeatingTheZomiesYou22,
        "\xce\xe4\xe5\xf0\xe6\xe0\xe2\x20\xef\xee\xe1\xe5\xe4\xf3\x20\xed\xe0\xe4\x20\xe7\xee\xec\xe1\xe8"
        "\x2c\x20\xe2\xfb\x20\xee\xe1\xfb\xf1\xea\xe8\xe2\xe0\xe5\xf2\xe5\x20\xec\xee\xe3\xe8\xeb\xfb\x20"
        "\xe8\x20\xf3\xe4\xe0\xeb\xff\xe5\xf2\xe5\xf1\xfc\x20\xf1\x20\xed\xe0\xf5\xee\xe4\xea\xee\xe9\x21"),
    // {Дом стрелков}\n\nГруппа стрелков в поисках славы желает примкнуть к вашему войску. Согласны ли
    // вы принять их?
    DATA_COMPGEN(0x0050846c, gEventTextArcherSHouse23,
        "\x7b\xc4\xee\xec\x20\xf1\xf2\xf0\xe5\xeb\xea\xee\xe2\x7d\x0a\x0a\xc3\xf0\xf3\xef\xef\xe0\x20\xf1"
        "\xf2\xf0\xe5\xeb\xea\xee\xe2\x20\xe2\x20\xef\xee\xe8\xf1\xea\xe0\xf5\x20\xf1\xeb\xe0\xe2\xfb\x20"
        "\xe6\xe5\xeb\xe0\xe5\xf2\x20\xef\xf0\xe8\xec\xea\xed\xf3\xf2\xfc\x20\xea\x20\xe2\xe0\xf8\xe5\xec"
        "\xf3\x20\xe2\xee\xe9\xf1\xea\xf3\x2e\x20\xd1\xee\xe3\xeb\xe0\xf1\xed\xfb\x20\xeb\xe8\x20\xe2\xfb"
        "\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xe8\xf5\x3f"),
    // В вашем войске нет места для новых рекрутов.
    DATA_COMPGEN(0x005084dc, gEventTextYouAreUnableToRecruit24,
        "\xc2\x20\xe2\xe0\xf8\xe5\xec\x20\xe2\xee\xe9\xf1\xea\xe5\x20\xed\xe5\xf2\x20\xec\xe5\xf1\xf2\xe0"
        "\x20\xe4\xeb\xff\x20\xed\xee\xe2\xfb\xf5\x20\xf0\xe5\xea\xf0\xf3\xf2\xee\xe2\x2e"),
    // {Дом стрелков}\n\nПриблизившись к жилищу, вы обнаруживаете, что оно пустует.
    DATA_COMPGEN(0x0050850c, gEventTextArcherSHouse25,
        "\x7b\xc4\xee\xec\x20\xf1\xf2\xf0\xe5\xeb\xea\xee\xe2\x7d\x0a\x0a\xcf\xf0\xe8\xe1\xeb\xe8\xe7\xe8"
        "\xe2\xf8\xe8\xf1\xfc\x20\xea\x20\xe6\xe8\xeb\xe8\xf9\xf3\x2c\x20\xe2\xfb\x20\xee\xe1\xed\xe0\xf0"
        "\xf3\xe6\xe8\xe2\xe0\xe5\xf2\xe5\x2c\x20\xf7\xf2\xee\x20\xee\xed\xee\x20\xef\xf3\xf1\xf2\xf3\xe5"
        "\xf2\x2e"),
    // Хибара гоблинов\n\nГруппа гоблинов в поисках славы желает примкнуть к вашему войску. Согласны ли
    // вы принять их?
    DATA_COMPGEN(0x00508558, gEventTextGoblinHut26,
        "\xd5\xe8\xe1\xe0\xf0\xe0\x20\xe3\xee\xe1\xeb\xe8\xed\xee\xe2\x0a\x0a\xc3\xf0\xf3\xef\xef\xe0\x20"
        "\xe3\xee\xe1\xeb\xe8\xed\xee\xe2\x20\xe2\x20\xef\xee\xe8\xf1\xea\xe0\xf5\x20\xf1\xeb\xe0\xe2\xfb"
        "\x20\xe6\xe5\xeb\xe0\xe5\xf2\x20\xef\xf0\xe8\xec\xea\xed\xf3\xf2\xfc\x20\xea\x20\xe2\xe0\xf8\xe5"
        "\xec\xf3\x20\xe2\xee\xe9\xf1\xea\xf3\x2e\x20\xd1\xee\xe3\xeb\xe0\xf1\xed\xfb\x20\xeb\xe8\x20\xe2"
        "\xfb\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xe8\xf5\x3f"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    DATA_COMPGEN(0x005085c8, gEventTextYouAreUnableToRecruit27,
        "\xc2\xfb\x20\xed\xe5\x20\xec\xee\xe6\xe5\xf2\xe5\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xed\xee\xe2"
        "\xfb\xf5\x20\xf0\xe5\xea\xf0\xf3\xf2\xee\xe2\x20\xe2\x20\xf1\xe2\xee\xe5\x20\xe2\xee\xe9\xf1\xea"
        "\xee\x2c\x20\xe5\xe3\xee\x20\xf0\xff\xe4\xfb\x20\xef\xee\xeb\xed\xfb\x2e"),
    // Хибара гоблинов\n\nПриблизившись к жилищу гоблинов, вы обнаруживаете, что оно пустует.
    DATA_COMPGEN(0x0050860c, gEventTextGoblinHut28,
        "\xd5\xe8\xe1\xe0\xf0\xe0\x20\xe3\xee\xe1\xeb\xe8\xed\xee\xe2\x0a\x0a\xcf\xf0\xe8\xe1\xeb\xe8\xe7"
        "\xe8\xe2\xf8\xe8\xf1\xfc\x20\xea\x20\xe6\xe8\xeb\xe8\xf9\xf3\x20\xe3\xee\xe1\xeb\xe8\xed\xee\xe2"
        "\x2c\x20\xe2\xfb\x20\xee\xe1\xed\xe0\xf0\xf3\xe6\xe8\xe2\xe0\xe5\xf2\xe5\x2c\x20\xf7\xf2\xee\x20"
        "\xee\xed\xee\x20\xef\xf3\xf1\xf2\xf3\xe5\xf2\x2e"),
    // Хижина крестьян\n\nГруппа крестьян в поисках славы желает примкнуть к вашему войску. Согласны ли
    // вы принять их?
    DATA_COMPGEN(0x00508664, gEventTextPeasantHut29,
        "\xd5\xe8\xe6\xe8\xed\xe0\x20\xea\xf0\xe5\xf1\xf2\xfc\xff\xed\x0a\x0a\xc3\xf0\xf3\xef\xef\xe0\x20"
        "\xea\xf0\xe5\xf1\xf2\xfc\xff\xed\x20\xe2\x20\xef\xee\xe8\xf1\xea\xe0\xf5\x20\xf1\xeb\xe0\xe2\xfb"
        "\x20\xe6\xe5\xeb\xe0\xe5\xf2\x20\xef\xf0\xe8\xec\xea\xed\xf3\xf2\xfc\x20\xea\x20\xe2\xe0\xf8\xe5"
        "\xec\xf3\x20\xe2\xee\xe9\xf1\xea\xf3\x2e\x20\xd1\xee\xe3\xeb\xe0\xf1\xed\xfb\x20\xeb\xe8\x20\xe2"
        "\xfb\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xe8\xf5\x3f"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    DATA_COMPGEN(0x005086d4, gEventTextYouAreUnableToRecruit30,
        "\xc2\xfb\x20\xed\xe5\x20\xec\xee\xe6\xe5\xf2\xe5\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xed\xee\xe2"
        "\xfb\xf5\x20\xf0\xe5\xea\xf0\xf3\xf2\xee\xe2\x20\xe2\x20\xf1\xe2\xee\xe5\x20\xe2\xee\xe9\xf1\xea"
        "\xee\x2c\x20\xe5\xe3\xee\x20\xf0\xff\xe4\xfb\x20\xef\xee\xeb\xed\xfb\x2e"),
    // Хижина крестьян\n\nПриблизившись к жилищу крестьян, вы обнаруживаете, что оно пустует.
    DATA_COMPGEN(0x00508718, gEventTextPeasantHut31,
        "\xd5\xe8\xe6\xe8\xed\xe0\x20\xea\xf0\xe5\xf1\xf2\xfc\xff\xed\x0a\x0a\xcf\xf0\xe8\xe1\xeb\xe8\xe7"
        "\xe8\xe2\xf8\xe8\xf1\xfc\x20\xea\x20\xe6\xe8\xeb\xe8\xf9\xf3\x20\xea\xf0\xe5\xf1\xf2\xfc\xff\xed"
        "\x2c\x20\xe2\xfb\x20\xee\xe1\xed\xe0\xf0\xf3\xe6\xe8\xe2\xe0\xe5\xf2\xe5\x2c\x20\xf7\xf2\xee\x20"
        "\xee\xed\xee\x20\xef\xf3\xf1\xf2\xf3\xe5\xf2\x2e"),
    // Избушка гномов\n\nГруппа стрелков в поисках славы желает примкнуть к вашему войску. Согласны ли
    // вы принять их?
    DATA_COMPGEN(0x00508770, gEventTextDwarfCottage32,
        "\xc8\xe7\xe1\xf3\xf8\xea\xe0\x20\xe3\xed\xee\xec\xee\xe2\x0a\x0a\xc3\xf0\xf3\xef\xef\xe0\x20\xf1"
        "\xf2\xf0\xe5\xeb\xea\xee\xe2\x20\xe2\x20\xef\xee\xe8\xf1\xea\xe0\xf5\x20\xf1\xeb\xe0\xe2\xfb\x20"
        "\xe6\xe5\xeb\xe0\xe5\xf2\x20\xef\xf0\xe8\xec\xea\xed\xf3\xf2\xfc\x20\xea\x20\xe2\xe0\xf8\xe5\xec"
        "\xf3\x20\xe2\xee\xe9\xf1\xea\xf3\x2e\x20\xd1\xee\xe3\xeb\xe0\xf1\xed\xfb\x20\xeb\xe8\x20\xe2\xfb"
        "\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xe8\xf5\x3f"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    DATA_COMPGEN(0x005087e0, gEventTextYouAreUnableToRecruit33,
        "\xc2\xfb\x20\xed\xe5\x20\xec\xee\xe6\xe5\xf2\xe5\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xed\xee\xe2"
        "\xfb\xf5\x20\xf0\xe5\xea\xf0\xf3\xf2\xee\xe2\x20\xe2\x20\xf1\xe2\xee\xe5\x20\xe2\xee\xe9\xf1\xea"
        "\xee\x2c\x20\xe5\xe3\xee\x20\xf0\xff\xe4\xfb\x20\xef\xee\xeb\xed\xfb\x2e"),
    // Избушка гномов\n\nПриблизившись к жилищу стрелков, вы обнаруживаете, что оно пустует.
    DATA_COMPGEN(0x00508824, gEventTextDwarfCottage34,
        "\xc8\xe7\xe1\xf3\xf8\xea\xe0\x20\xe3\xed\xee\xec\xee\xe2\x0a\x0a\xcf\xf0\xe8\xe1\xeb\xe8\xe7\xe8"
        "\xe2\xf8\xe8\xf1\xfc\x20\xea\x20\xe6\xe8\xeb\xe8\xf9\xf3\x20\xf1\xf2\xf0\xe5\xeb\xea\xee\xe2\x2c"
        "\x20\xe2\xfb\x20\xee\xe1\xed\xe0\xf0\xf3\xe6\xe8\xe2\xe0\xe5\xf2\xe5\x2c\x20\xf7\xf2\xee\x20\xee"
        "\xed\xee\x20\xef\xf3\xf1\xf2\xf3\xe5\xf2\x2e"),
    // {Мазанка}\n\nГруппа крестьян в поисках славы желает примкнуть к вашему войску. Согласны ли вы
    // принять их?
    DATA_COMPGEN(0x00508878, gEventTextThatchedHut35,
        "\x7b\xcc\xe0\xe7\xe0\xed\xea\xe0\x7d\x0a\x0a\xc3\xf0\xf3\xef\xef\xe0\x20\xea\xf0\xe5\xf1\xf2\xfc"
        "\xff\xed\x20\xe2\x20\xef\xee\xe8\xf1\xea\xe0\xf5\x20\xf1\xeb\xe0\xe2\xfb\x20\xe6\xe5\xeb\xe0\xe5"
        "\xf2\x20\xef\xf0\xe8\xec\xea\xed\xf3\xf2\xfc\x20\xea\x20\xe2\xe0\xf8\xe5\xec\xf3\x20\xe2\xee\xe9"
        "\xf1\xea\xf3\x2e\x20\xd1\xee\xe3\xeb\xe0\xf1\xed\xfb\x20\xeb\xe8\x20\xe2\xfb\x20\xef\xf0\xe8\xed"
        "\xff\xf2\xfc\x20\xe8\xf5\x3f"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    DATA_COMPGEN(0x005088e0, gEventTextYouAreUnableToRecruit36,
        "\xc2\xfb\x20\xed\xe5\x20\xec\xee\xe6\xe5\xf2\xe5\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xed\xee\xe2"
        "\xfb\xf5\x20\xf0\xe5\xea\xf0\xf3\xf2\xee\xe2\x20\xe2\x20\xf1\xe2\xee\xe5\x20\xe2\xee\xe9\xf1\xea"
        "\xee\x2c\x20\xe5\xe3\xee\x20\xf0\xff\xe4\xfb\x20\xef\xee\xeb\xed\xfb\x2e"),
    // {Мазанка}\n\nПриблизившись к жилищу Крестьян, вы обнаруживаете, что оно пустует.
    DATA_COMPGEN(0x00508924, gEventTextThatchedHut37,
        "\x7b\xcc\xe0\xe7\xe0\xed\xea\xe0\x7d\x0a\x0a\xcf\xf0\xe8\xe1\xeb\xe8\xe7\xe8\xe2\xf8\xe8\xf1\xfc"
        "\x20\xea\x20\xe6\xe8\xeb\xe8\xf9\xf3\x20\xca\xf0\xe5\xf1\xf2\xfc\xff\xed\x2c\x20\xe2\xfb\x20\xee"
        "\xe1\xed\xe0\xf0\xf3\xe6\xe8\xe2\xe0\xe5\xf2\xe5\x2c\x20\xf7\xf2\xee\x20\xee\xed\xee\x20\xef\xf3"
        "\xf1\xf2\xf3\xe5\xf2\x2e"),
    // {Древо-дом}\n\nГруппа фей в поисках славы желает примкнуть к вашему войску. Согласны ли вы
    // принять их?
    DATA_COMPGEN(0x00508974, gEventTextTreeHouse38,
        "\x7b\xc4\xf0\xe5\xe2\xee\x2d\xe4\xee\xec\x7d\x0a\x0a\xc3\xf0\xf3\xef\xef\xe0\x20\xf4\xe5\xe9\x20"
        "\xe2\x20\xef\xee\xe8\xf1\xea\xe0\xf5\x20\xf1\xeb\xe0\xe2\xfb\x20\xe6\xe5\xeb\xe0\xe5\xf2\x20\xef"
        "\xf0\xe8\xec\xea\xed\xf3\xf2\xfc\x20\xea\x20\xe2\xe0\xf8\xe5\xec\xf3\x20\xe2\xee\xe9\xf1\xea\xf3"
        "\x2e\x20\xd1\xee\xe3\xeb\xe0\xf1\xed\xfb\x20\xeb\xe8\x20\xe2\xfb\x20\xef\xf0\xe8\xed\xff\xf2\xfc"
        "\x20\xe8\xf5\x3f"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    DATA_COMPGEN(0x005089dc, gEventTextYouAreUnableToRecruit39,
        "\xc2\xfb\x20\xed\xe5\x20\xec\xee\xe6\xe5\xf2\xe5\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xed\xee\xe2"
        "\xfb\xf5\x20\xf0\xe5\xea\xf0\xf3\xf2\xee\xe2\x20\xe2\x20\xf1\xe2\xee\xe5\x20\xe2\xee\xe9\xf1\xea"
        "\xee\x2c\x20\xe5\xe3\xee\x20\xf0\xff\xe4\xfb\x20\xef\xee\xeb\xed\xfb\x2e"),
    // {Древо-дом}\n\nПриблизившись к древесному дому Фей, вы обнаруживаете, что он пустует.
    DATA_COMPGEN(0x00508a20, gEventTextTreeHouse40,
        "\x7b\xc4\xf0\xe5\xe2\xee\x2d\xe4\xee\xec\x7d\x0a\x0a\xcf\xf0\xe8\xe1\xeb\xe8\xe7\xe8\xe2\xf8\xe8"
        "\xf1\xfc\x20\xea\x20\xe4\xf0\xe5\xe2\xe5\xf1\xed\xee\xec\xf3\x20\xe4\xee\xec\xf3\x20\xd4\xe5\xe9"
        "\x2c\x20\xe2\xfb\x20\xee\xe1\xed\xe0\xf0\xf3\xe6\xe8\xe2\xe0\xe5\xf2\xe5\x2c\x20\xf7\xf2\xee\x20"
        "\xee\xed\x20\xef\xf3\xf1\xf2\xf3\xe5\xf2\x2e"),
    // {Нора полуросликов}\n\nГруппа полуросликов в поисках славы желает примкнуть к вашему войску.
    // Согласны ли вы принять их?
    DATA_COMPGEN(0x00508a74, gEventTextHalflingHole41,
        "\x7b\xcd\xee\xf0\xe0\x20\xef\xee\xeb\xf3\xf0\xee\xf1\xeb\xe8\xea\xee\xe2\x7d\x0a\x0a\xc3\xf0\xf3"
        "\xef\xef\xe0\x20\xef\xee\xeb\xf3\xf0\xee\xf1\xeb\xe8\xea\xee\xe2\x20\xe2\x20\xef\xee\xe8\xf1\xea"
        "\xe0\xf5\x20\xf1\xeb\xe0\xe2\xfb\x20\xe6\xe5\xeb\xe0\xe5\xf2\x20\xef\xf0\xe8\xec\xea\xed\xf3\xf2"
        "\xfc\x20\xea\x20\xe2\xe0\xf8\xe5\xec\xf3\x20\xe2\xee\xe9\xf1\xea\xf3\x2e\x20\xd1\xee\xe3\xeb\xe0"
        "\xf1\xed\xfb\x20\xeb\xe8\x20\xe2\xfb\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xe8\xf5\x3f"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    DATA_COMPGEN(0x00508aec, gEventTextYouAreUnableToRecruit42,
        "\xc2\xfb\x20\xed\xe5\x20\xec\xee\xe6\xe5\xf2\xe5\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xed\xee\xe2"
        "\xfb\xf5\x20\xf0\xe5\xea\xf0\xf3\xf2\xee\xe2\x20\xe2\x20\xf1\xe2\xee\xe5\x20\xe2\xee\xe9\xf1\xea"
        "\xee\x2c\x20\xe5\xe3\xee\x20\xf0\xff\xe4\xfb\x20\xef\xee\xeb\xed\xfb\x2e"),
    // {Нора полуросликов}\n\nПриблизившись к норе полуросликов, вы обнаруживаете, что она пустует.
    DATA_COMPGEN(0x00508b30, gEventTextHalflingHole43,
        "\x7b\xcd\xee\xf0\xe0\x20\xef\xee\xeb\xf3\xf0\xee\xf1\xeb\xe8\xea\xee\xe2\x7d\x0a\x0a\xcf\xf0\xe8"
        "\xe1\xeb\xe8\xe7\xe8\xe2\xf8\xe8\xf1\xfc\x20\xea\x20\xed\xee\xf0\xe5\x20\xef\xee\xeb\xf3\xf0\xee"
        "\xf1\xeb\xe8\xea\xee\xe2\x2c\x20\xe2\xfb\x20\xee\xe1\xed\xe0\xf0\xf3\xe6\xe8\xe2\xe0\xe5\xf2\xe5"
        "\x2c\x20\xf7\xf2\xee\x20\xee\xed\xe0\x20\xef\xf3\xf1\xf2\xf3\xe5\xf2\x2e"),
    // {Сторожевая вышка}\n\nГруппа орков в поисках славы желает примкнуть к вашему войску. Согласны ли
    // вы принять их?
    DATA_COMPGEN(0x00508b8c, gEventTextWatchTower44,
        "\x7b\xd1\xf2\xee\xf0\xee\xe6\xe5\xe2\xe0\xff\x20\xe2\xfb\xf8\xea\xe0\x7d\x0a\x0a\xc3\xf0\xf3\xef"
        "\xef\xe0\x20\xee\xf0\xea\xee\xe2\x20\xe2\x20\xef\xee\xe8\xf1\xea\xe0\xf5\x20\xf1\xeb\xe0\xe2\xfb"
        "\x20\xe6\xe5\xeb\xe0\xe5\xf2\x20\xef\xf0\xe8\xec\xea\xed\xf3\xf2\xfc\x20\xea\x20\xe2\xe0\xf8\xe5"
        "\xec\xf3\x20\xe2\xee\xe9\xf1\xea\xf3\x2e\x20\xd1\xee\xe3\xeb\xe0\xf1\xed\xfb\x20\xeb\xe8\x20\xe2"
        "\xfb\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xe8\xf5\x3f"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    DATA_COMPGEN(0x00508bfc, gEventTextYouAreUnableToRecruit45,
        "\xc2\xfb\x20\xed\xe5\x20\xec\xee\xe6\xe5\xf2\xe5\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xed\xee\xe2"
        "\xfb\xf5\x20\xf0\xe5\xea\xf0\xf3\xf2\xee\xe2\x20\xe2\x20\xf1\xe2\xee\xe5\x20\xe2\xee\xe9\xf1\xea"
        "\xee\x2c\x20\xe5\xe3\xee\x20\xf0\xff\xe4\xfb\x20\xef\xee\xeb\xed\xfb\x2e"),
    // {Сторожевая вышка}\n\nПриблизившись к сторожевой вышке орков, вы обнаруживаете, что она пустует.
    DATA_COMPGEN(0x00508c40, gEventTextWatchTower46,
        "\x7b\xd1\xf2\xee\xf0\xee\xe6\xe5\xe2\xe0\xff\x20\xe2\xfb\xf8\xea\xe0\x7d\x0a\x0a\xcf\xf0\xe8\xe1"
        "\xeb\xe8\xe7\xe8\xe2\xf8\xe8\xf1\xfc\x20\xea\x20\xf1\xf2\xee\xf0\xee\xe6\xe5\xe2\xee\xe9\x20\xe2"
        "\xfb\xf8\xea\xe5\x20\xee\xf0\xea\xee\xe2\x2c\x20\xe2\xfb\x20\xee\xe1\xed\xe0\xf0\xf3\xe6\xe8\xe2"
        "\xe0\xe5\xf2\xe5\x2c\x20\xf7\xf2\xee\x20\xee\xed\xe0\x20\xef\xf3\xf1\xf2\xf3\xe5\xf2\x2e"),
    // {Снежная пещера}\n\nГруппа кентавров в поисках славы желает примкнуть к вашему войску. Согласны
    // ли вы принять их?
    DATA_COMPGEN(0x00508ca0, gEventTextSnowCave47,
        "\x7b\xd1\xed\xe5\xe6\xed\xe0\xff\x20\xef\xe5\xf9\xe5\xf0\xe0\x7d\x0a\x0a\xc3\xf0\xf3\xef\xef\xe0"
        "\x20\xea\xe5\xed\xf2\xe0\xe2\xf0\xee\xe2\x20\xe2\x20\xef\xee\xe8\xf1\xea\xe0\xf5\x20\xf1\xeb\xe0"
        "\xe2\xfb\x20\xe6\xe5\xeb\xe0\xe5\xf2\x20\xef\xf0\xe8\xec\xea\xed\xf3\xf2\xfc\x20\xea\x20\xe2\xe0"
        "\xf8\xe5\xec\xf3\x20\xe2\xee\xe9\xf1\xea\xf3\x2e\x20\xd1\xee\xe3\xeb\xe0\xf1\xed\xfb\x20\xeb\xe8"
        "\x20\xe2\xfb\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xe8\xf5\x3f"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    DATA_COMPGEN(0x00508d10, gEventTextYouAreUnableToRecruit48,
        "\xc2\xfb\x20\xed\xe5\x20\xec\xee\xe6\xe5\xf2\xe5\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xed\xee\xe2"
        "\xfb\xf5\x20\xf0\xe5\xea\xf0\xf3\xf2\xee\xe2\x20\xe2\x20\xf1\xe2\xee\xe5\x20\xe2\xee\xe9\xf1\xea"
        "\xee\x2c\x20\xe5\xe3\xee\x20\xf0\xff\xe4\xfb\x20\xef\xee\xeb\xed\xfb\x2e"),
    // {Пещера}\n\nПриблизившись к пещере кентавров, вы обнаруживаете, что она пустует.
    DATA_COMPGEN(0x00508d54, gEventTextCave49,
        "\x7b\xcf\xe5\xf9\xe5\xf0\xe0\x7d\x0a\x0a\xcf\xf0\xe8\xe1\xeb\xe8\xe7\xe8\xe2\xf8\xe8\xf1\xfc\x20"
        "\xea\x20\xef\xe5\xf9\xe5\xf0\xe5\x20\xea\xe5\xed\xf2\xe0\xe2\xf0\xee\xe2\x2c\x20\xe2\xfb\x20\xee"
        "\xe1\xed\xe0\xf0\xf3\xe6\xe8\xe2\xe0\xe5\xf2\xe5\x2c\x20\xf7\xf2\xee\x20\xee\xed\xe0\x20\xef\xf3"
        "\xf1\xf2\xf3\xe5\xf2\x2e"),
    // {Раскопки}\n\nГруппа скелетов в поисках славы желает примкнуть к вашему войску. Согласны ли вы
    // принять их?
    DATA_COMPGEN(0x00508da4, gEventTextExcavation50,
        "\x7b\xd0\xe0\xf1\xea\xee\xef\xea\xe8\x7d\x0a\x0a\xc3\xf0\xf3\xef\xef\xe0\x20\xf1\xea\xe5\xeb\xe5"
        "\xf2\xee\xe2\x20\xe2\x20\xef\xee\xe8\xf1\xea\xe0\xf5\x20\xf1\xeb\xe0\xe2\xfb\x20\xe6\xe5\xeb\xe0"
        "\xe5\xf2\x20\xef\xf0\xe8\xec\xea\xed\xf3\xf2\xfc\x20\xea\x20\xe2\xe0\xf8\xe5\xec\xf3\x20\xe2\xee"
        "\xe9\xf1\xea\xf3\x2e\x20\xd1\xee\xe3\xeb\xe0\xf1\xed\xfb\x20\xeb\xe8\x20\xe2\xfb\x20\xef\xf0\xe8"
        "\xed\xff\xf2\xfc\x20\xe8\xf5\x3f"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    DATA_COMPGEN(0x00508e10, gEventTextYouAreUnableToRecruit51,
        "\xc2\xfb\x20\xed\xe5\x20\xec\xee\xe6\xe5\xf2\xe5\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xed\xee\xe2"
        "\xfb\xf5\x20\xf0\xe5\xea\xf0\xf3\xf2\xee\xe2\x20\xe2\x20\xf1\xe2\xee\xe5\x20\xe2\xee\xe9\xf1\xea"
        "\xee\x2c\x20\xe5\xe3\xee\x20\xf0\xff\xe4\xfb\x20\xef\xee\xeb\xed\xfb\x2e"),
    // {Раскопки}\n\nПриблизившись к захоронению скелетов, вы обнаруживаете, что оно пустует.
    DATA_COMPGEN(0x00508e54, gEventTextExcavation52,
        "\x7b\xd0\xe0\xf1\xea\xee\xef\xea\xe8\x7d\x0a\x0a\xcf\xf0\xe8\xe1\xeb\xe8\xe7\xe8\xe2\xf8\xe8\xf1"
        "\xfc\x20\xea\x20\xe7\xe0\xf5\xee\xf0\xee\xed\xe5\xed\xe8\xfe\x20\xf1\xea\xe5\xeb\xe5\xf2\xee\xe2"
        "\x2c\x20\xe2\xfb\x20\xee\xe1\xed\xe0\xf0\xf3\xe6\xe8\xe2\xe0\xe5\xf2\xe5\x2c\x20\xf7\xf2\xee\x20"
        "\xee\xed\xee\x20\xef\xf3\xf1\xf2\xf3\xe5\xf2\x2e"),
    DATA_COMPGEN(0x00526794, gEventTextEmpty53, ""),
    DATA_COMPGEN(0x00526798, gEventTextEmpty54, ""),
    DATA_COMPGEN(0x0052679c, gEventTextEmpty55, ""),
    DATA_COMPGEN(0x005267a0, gEventTextEmpty56, ""),
    DATA_COMPGEN(0x005267a4, gEventTextEmpty57, ""),
    // Маяк\n\nТеперь маяк ваш, и все ваши корабли будут преодолевать большее расстояние за один ход.
    DATA_COMPGEN(0x00508eac, gEventTextLighthouse58,
        "\xcc\xe0\xff\xea\x0a\x0a\xd2\xe5\xef\xe5\xf0\xfc\x20\xec\xe0\xff\xea\x20\xe2\xe0\xf8\x2c\x20\xe8"
        "\x20\xe2\xf1\xe5\x20\xe2\xe0\xf8\xe8\x20\xea\xee\xf0\xe0\xe1\xeb\xe8\x20\xe1\xf3\xe4\xf3\xf2\x20"
        "\xef\xf0\xe5\xee\xe4\xee\xeb\xe5\xe2\xe0\xf2\xfc\x20\xe1\xee\xeb\xfc\xf8\xe5\xe5\x20\xf0\xe0\xf1"
        "\xf1\xf2\xee\xff\xed\xe8\xe5\x20\xe7\xe0\x20\xee\xe4\xe8\xed\x20\xf5\xee\xe4\x2e"),
    // Водяная мельница\n\nМельник обращается к вам со словами: \"Сожалею, господин, но сегодня золота
    // у меня нет. Приходите на следующей неделе.\"
    DATA_COMPGEN(0x00508f0c, gEventTextWaterWheel59,
        "\xc2\xee\xe4\xff\xed\xe0\xff\x20\xec\xe5\xeb\xfc\xed\xe8\xf6\xe0\x0a\x0a\xcc\xe5\xeb\xfc\xed\xe8"
        "\xea\x20\xee\xe1\xf0\xe0\xf9\xe0\xe5\xf2\xf1\xff\x20\xea\x20\xe2\xe0\xec\x20\xf1\xee\x20\xf1\xeb"
        "\xee\xe2\xe0\xec\xe8\x3a\x20\x22\xd1\xee\xe6\xe0\xeb\xe5\xfe\x2c\x20\xe3\xee\xf1\xef\xee\xe4\xe8"
        "\xed\x2c\x20\xed\xee\x20\xf1\xe5\xe3\xee\xe4\xed\xff\x20\xe7\xee\xeb\xee\xf2\xe0\x20\xf3\x20\xec"
        "\xe5\xed\xff\x20\xed\xe5\xf2\x2e\x20\xcf\xf0\xe8\xf5\xee\xe4\xe8\xf2\xe5\x20\xed\xe0\x20\xf1\xeb"
        "\xe5\xe4\xf3\xfe\xf9\xe5\xe9\x20\xed\xe5\xe4\xe5\xeb\xe5\x2e\x22"),
    // Водяная мельница\n\nМельник обращается к вам со словами: \"Господин, я трудился в поте лица и
    // прошу вас принять мою скромную лепту. Приходите на следующей неделе, и вы получите еще столько
    // же.\"
    DATA_COMPGEN(0x00508f98, gEventTextWaterWheel60,
        "\xc2\xee\xe4\xff\xed\xe0\xff\x20\xec\xe5\xeb\xfc\xed\xe8\xf6\xe0\x0a\x0a\xcc\xe5\xeb\xfc\xed\xe8"
        "\xea\x20\xee\xe1\xf0\xe0\xf9\xe0\xe5\xf2\xf1\xff\x20\xea\x20\xe2\xe0\xec\x20\xf1\xee\x20\xf1\xeb"
        "\xee\xe2\xe0\xec\xe8\x3a\x20\x22\xc3\xee\xf1\xef\xee\xe4\xe8\xed\x2c\x20\xff\x20\xf2\xf0\xf3\xe4"
        "\xe8\xeb\xf1\xff\x20\xe2\x20\xef\xee\xf2\xe5\x20\xeb\xe8\xf6\xe0\x20\xe8\x20\xef\xf0\xee\xf8\xf3"
        "\x20\xe2\xe0\xf1\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xec\xee\xfe\x20\xf1\xea\xf0\xee\xec\xed\xf3"
        "\xfe\x20\xeb\xe5\xef\xf2\xf3\x2e\x20\xcf\xf0\xe8\xf5\xee\xe4\xe8\xf2\xe5\x20\xed\xe0\x20\xf1\xeb"
        "\xe5\xe4\xf3\xfe\xf9\xe5\xe9\x20\xed\xe5\xe4\xe5\xeb\xe5\x2c\x20\xe8\x20\xe2\xfb\x20\xef\xee\xeb"
        "\xf3\xf7\xe8\xf2\xe5\x20\xe5\xf9\xe5\x20\xf1\xf2\xee\xeb\xfc\xea\xee\x20\xe6\xe5\x2e\x22"),
    // Рудная шахта\n\nВы стали хозяином рудной шахты. Она будет приносить вам по две меры руды в день.
    DATA_COMPGEN(0x00509058, gEventTextOreMine61,
        "\xd0\xf3\xe4\xed\xe0\xff\x20\xf8\xe0\xf5\xf2\xe0\x0a\x0a\xc2\xfb\x20\xf1\xf2\xe0\xeb\xe8\x20\xf5"
        "\xee\xe7\xff\xe8\xed\xee\xec\x20\xf0\xf3\xe4\xed\xee\xe9\x20\xf8\xe0\xf5\xf2\xfb\x2e\x20\xce\xed"
        "\xe0\x20\xe1\xf3\xe4\xe5\xf2\x20\xef\xf0\xe8\xed\xee\xf1\xe8\xf2\xfc\x20\xe2\xe0\xec\x20\xef\xee"
        "\x20\xe4\xe2\xe5\x20\xec\xe5\xf0\xfb\x20\xf0\xf3\xe4\xfb\x20\xe2\x20\xe4\xe5\xed\xfc\x2e"),
    // Серная шахта\n\nВы стали хозяином серной шахты. Она будут приносить вам по 1 единице серы в
    // день.
    DATA_COMPGEN(0x005090b8, gEventTextSulfurMine62,
        "\xd1\xe5\xf0\xed\xe0\xff\x20\xf8\xe0\xf5\xf2\xe0\x0a\x0a\xc2\xfb\x20\xf1\xf2\xe0\xeb\xe8\x20\xf5"
        "\xee\xe7\xff\xe8\xed\xee\xec\x20\xf1\xe5\xf0\xed\xee\xe9\x20\xf8\xe0\xf5\xf2\xfb\x2e\x20\xce\xed"
        "\xe0\x20\xe1\xf3\xe4\xf3\xf2\x20\xef\xf0\xe8\xed\xee\xf1\xe8\xf2\xfc\x20\xe2\xe0\xec\x20\xef\xee"
        "\x20\x31\x20\xe5\xe4\xe8\xed\xe8\xf6\xe5\x20\xf1\xe5\xf0\xfb\x20\xe2\x20\xe4\xe5\xed\xfc\x2e"),
    // Кристальная шахта\n\nВы стали хозяином кристальной шахты. Она будет приносить вам по одной мере
    // кристаллов в день.
    DATA_COMPGEN(0x00509118, gEventTextCrystalMine63,
        "\xca\xf0\xe8\xf1\xf2\xe0\xeb\xfc\xed\xe0\xff\x20\xf8\xe0\xf5\xf2\xe0\x0a\x0a\xc2\xfb\x20\xf1\xf2"
        "\xe0\xeb\xe8\x20\xf5\xee\xe7\xff\xe8\xed\xee\xec\x20\xea\xf0\xe8\xf1\xf2\xe0\xeb\xfc\xed\xee\xe9"
        "\x20\xf8\xe0\xf5\xf2\xfb\x2e\x20\xce\xed\xe0\x20\xe1\xf3\xe4\xe5\xf2\x20\xef\xf0\xe8\xed\xee\xf1"
        "\xe8\xf2\xfc\x20\xe2\xe0\xec\x20\xef\xee\x20\xee\xe4\xed\xee\xe9\x20\xec\xe5\xf0\xe5\x20\xea\xf0"
        "\xe8\xf1\xf2\xe0\xeb\xeb\xee\xe2\x20\xe2\x20\xe4\xe5\xed\xfc\x2e"),
    // Самоцветная шахта\n\nВы стали хозяином самоцветной шахты. Она будет приносить вам по 1 единице
    // самоцветов в день.
    DATA_COMPGEN(0x0050918c, gEventTextGemMine64,
        "\xd1\xe0\xec\xee\xf6\xe2\xe5\xf2\xed\xe0\xff\x20\xf8\xe0\xf5\xf2\xe0\x0a\x0a\xc2\xfb\x20\xf1\xf2"
        "\xe0\xeb\xe8\x20\xf5\xee\xe7\xff\xe8\xed\xee\xec\x20\xf1\xe0\xec\xee\xf6\xe2\xe5\xf2\xed\xee\xe9"
        "\x20\xf8\xe0\xf5\xf2\xfb\x2e\x20\xce\xed\xe0\x20\xe1\xf3\xe4\xe5\xf2\x20\xef\xf0\xe8\xed\xee\xf1"
        "\xe8\xf2\xfc\x20\xe2\xe0\xec\x20\xef\xee\x20\x31\x20\xe5\xe4\xe8\xed\xe8\xf6\xe5\x20\xf1\xe0\xec"
        "\xee\xf6\xe2\xe5\xf2\xee\xe2\x20\xe2\x20\xe4\xe5\xed\xfc\x2e"),
    // Золотая шахта\n\nВы стали хозяином золотой шахты. Она будет приносить вам по 1000 золотых в
    // день.
    DATA_COMPGEN(0x005091fc, gEventTextGoldMine65,
        "\xc7\xee\xeb\xee\xf2\xe0\xff\x20\xf8\xe0\xf5\xf2\xe0\x0a\x0a\xc2\xfb\x20\xf1\xf2\xe0\xeb\xe8\x20"
        "\xf5\xee\xe7\xff\xe8\xed\xee\xec\x20\xe7\xee\xeb\xee\xf2\xee\xe9\x20\xf8\xe0\xf5\xf2\xfb\x2e\x20"
        "\xce\xed\xe0\x20\xe1\xf3\xe4\xe5\xf2\x20\xef\xf0\xe8\xed\xee\xf1\xe8\xf2\xfc\x20\xe2\xe0\xec\x20"
        "\xef\xee\x20\x31\x30\x30\x30\x20\xe7\xee\xeb\xee\xf2\xfb\xf5\x20\xe2\x20\xe4\xe5\xed\xfc\x2e"),
    // Последователи\n\nГруппа %s в поисках славы желает примкнуть к вашему войску. Вы согласны принять
    // их?
    DATA_COMPGEN(0x0050925c, gEventTextFollowers66,
        "\xcf\xee\xf1\xeb\xe5\xe4\xee\xe2\xe0\xf2\xe5\xeb\xe8\x0a\x0a\xc3\xf0\xf3\xef\xef\xe0\x20\x25\x73"
        "\x20\xe2\x20\xef\xee\xe8\xf1\xea\xe0\xf5\x20\xf1\xeb\xe0\xe2\xfb\x20\xe6\xe5\xeb\xe0\xe5\xf2\x20"
        "\xef\xf0\xe8\xec\xea\xed\xf3\xf2\xfc\x20\xea\x20\xe2\xe0\xf8\xe5\xec\xf3\x20\xe2\xee\xe9\xf1\xea"
        "\xf3\x2e\x20\xc2\xfb\x20\xf1\xee\xe3\xeb\xe0\xf1\xed\xfb\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xe8"
        "\xf5\x3f"),
    // Оскорбленные отказом быть принятыми в ваши ряды, они нападают на вас!
    DATA_COMPGEN(0x005092c0, gEventTextInsultedByYourRefusalOf67,
        "\xce\xf1\xea\xee\xf0\xe1\xeb\xe5\xed\xed\xfb\xe5\x20\xee\xf2\xea\xe0\xe7\xee\xec\x20\xe1\xfb\xf2"
        "\xfc\x20\xef\xf0\xe8\xed\xff\xf2\xfb\xec\xe8\x20\xe2\x20\xe2\xe0\xf8\xe8\x20\xf0\xff\xe4\xfb\x2c"
        "\x20\xee\xed\xe8\x20\xed\xe0\xef\xe0\xe4\xe0\xfe\xf2\x20\xed\xe0\x20\xe2\xe0\xf1\x21"),
    // Обелиск\n\nПеред вами обелиск, высеченный из невиданного камня. Вы вглядываетесь в его гладкую
    // поверхность и вдруг замечаете, что на ней начинают проступать таинственные знаки. Знаки
    // складываются во фрагмент древней карты. Вы торопливо срисовываете его, и знаки исчезают так же
    // внезапно, как и появились.
    DATA_COMPGEN(0x00509308, gEventTextObelisk68,
        "\xce\xe1\xe5\xeb\xe8\xf1\xea\x0a\x0a\xcf\xe5\xf0\xe5\xe4\x20\xe2\xe0\xec\xe8\x20\xee\xe1\xe5\xeb"
        "\xe8\xf1\xea\x2c\x20\xe2\xfb\xf1\xe5\xf7\xe5\xed\xed\xfb\xe9\x20\xe8\xe7\x20\xed\xe5\xe2\xe8\xe4"
        "\xe0\xed\xed\xee\xe3\xee\x20\xea\xe0\xec\xed\xff\x2e\x20\xc2\xfb\x20\xe2\xe3\xeb\xff\xe4\xfb\xe2"
        "\xe0\xe5\xf2\xe5\xf1\xfc\x20\xe2\x20\xe5\xe3\xee\x20\xe3\xeb\xe0\xe4\xea\xf3\xfe\x20\xef\xee\xe2"
        "\xe5\xf0\xf5\xed\xee\xf1\xf2\xfc\x20\xe8\x20\xe2\xe4\xf0\xf3\xe3\x20\xe7\xe0\xec\xe5\xf7\xe0\xe5"
        "\xf2\xe5\x2c\x20\xf7\xf2\xee\x20\xed\xe0\x20\xed\xe5\xe9\x20\xed\xe0\xf7\xe8\xed\xe0\xfe\xf2\x20"
        "\xef\xf0\xee\xf1\xf2\xf3\xef\xe0\xf2\xfc\x20\xf2\xe0\xe8\xed\xf1\xf2\xe2\xe5\xed\xed\xfb\xe5\x20"
        "\xe7\xed\xe0\xea\xe8\x2e\x20\xc7\xed\xe0\xea\xe8\x20\xf1\xea\xeb\xe0\xe4\xfb\xe2\xe0\xfe\xf2\xf1"
        "\xff\x20\xe2\xee\x20\xf4\xf0\xe0\xe3\xec\xe5\xed\xf2\x20\xe4\xf0\xe5\xe2\xed\xe5\xe9\x20\xea\xe0"
        "\xf0\xf2\xfb\x2e\x20\xc2\xfb\x20\xf2\xee\xf0\xee\xef\xeb\xe8\xe2\xee\x20\xf1\xf0\xe8\xf1\xee\xe2"
        "\xfb\xe2\xe0\xe5\xf2\xe5\x20\xe5\xe3\xee\x2c\x20\xe8\x20\xe7\xed\xe0\xea\xe8\x20\xe8\xf1\xf7\xe5"
        "\xe7\xe0\xfe\xf2\x20\xf2\xe0\xea\x20\xe6\xe5\x20\xe2\xed\xe5\xe7\xe0\xef\xed\xee\x2c\x20\xea\xe0"
        "\xea\x20\xe8\x20\xef\xee\xff\xe2\xe8\xeb\xe8\xf1\xfc\x2e"),
    // Обелиск\n\nВы уже посещали этот обелиск.
    DATA_COMPGEN(0x00509438, gEventTextObelisk69,
        "\xce\xe1\xe5\xeb\xe8\xf1\xea\x0a\x0a\xc2\xfb\x20\xf3\xe6\xe5\x20\xef\xee\xf1\xe5\xf9\xe0\xeb\xe8"
        "\x20\xfd\xf2\xee\xf2\x20\xee\xe1\xe5\xeb\xe8\xf1\xea\x2e"),
    DATA_COMPGEN(0x005267a8, gEventTextEmpty70, ""),
    DATA_COMPGEN(0x005267ac, gEventTextEmpty71, ""),
    // Вы нашли ресурс (%s).
    DATA_COMPGEN(0x00509460, gEventTextYouFindASmallQuantity72, "\xc2\xfb\x20\xed\xe0\xf8\xeb\xe8\x20\xf0\xe5\xf1\xf3\xf0\xf1\x20\x28\x25\x73\x29\x2e"),
    // Лесопилка\n\nВы стали хозяином лесопилки. Она будет приносить вам по 2 единицы древесины в день.
    DATA_COMPGEN(0x00509478, gEventTextSawmill73,
        "\xcb\xe5\xf1\xee\xef\xe8\xeb\xea\xe0\x0a\x0a\xc2\xfb\x20\xf1\xf2\xe0\xeb\xe8\x20\xf5\xee\xe7\xff"
        "\xe8\xed\xee\xec\x20\xeb\xe5\xf1\xee\xef\xe8\xeb\xea\xe8\x2e\x20\xce\xed\xe0\x20\xe1\xf3\xe4\xe5"
        "\xf2\x20\xef\xf0\xe8\xed\xee\xf1\xe8\xf2\xfc\x20\xe2\xe0\xec\x20\xef\xee\x20\x32\x20\xe5\xe4\xe8"
        "\xed\xe8\xf6\xfb\x20\xe4\xf0\xe5\xe2\xe5\xf1\xe8\xed\xfb\x20\xe2\x20\xe4\xe5\xed\xfc\x2e"),
    // {Оракул}\n\nНа поляне в окружении деревьев восседает слепой оракул. Вы рассказываете ему о целях
    // вашего похода, и он показывает вам сильные и слабые стороны ваших противников в магическом
    // хрустальном шаре.
    DATA_COMPGEN(0x005094d8, gEventTextOracle74,
        "\x7b\xce\xf0\xe0\xea\xf3\xeb\x7d\x0a\x0a\xcd\xe0\x20\xef\xee\xeb\xff\xed\xe5\x20\xe2\x20\xee\xea"
        "\xf0\xf3\xe6\xe5\xed\xe8\xe8\x20\xe4\xe5\xf0\xe5\xe2\xfc\xe5\xe2\x20\xe2\xee\xf1\xf1\xe5\xe4\xe0"
        "\xe5\xf2\x20\xf1\xeb\xe5\xef\xee\xe9\x20\xee\xf0\xe0\xea\xf3\xeb\x2e\x20\xc2\xfb\x20\xf0\xe0\xf1"
        "\xf1\xea\xe0\xe7\xfb\xe2\xe0\xe5\xf2\xe5\x20\xe5\xec\xf3\x20\xee\x20\xf6\xe5\xeb\xff\xf5\x20\xe2"
        "\xe0\xf8\xe5\xe3\xee\x20\xef\xee\xf5\xee\xe4\xe0\x2c\x20\xe8\x20\xee\xed\x20\xef\xee\xea\xe0\xe7"
        "\xfb\xe2\xe0\xe5\xf2\x20\xe2\xe0\xec\x20\xf1\xe8\xeb\xfc\xed\xfb\xe5\x20\xe8\x20\xf1\xeb\xe0\xe1"
        "\xfb\xe5\x20\xf1\xf2\xee\xf0\xee\xed\xfb\x20\xe2\xe0\xf8\xe8\xf5\x20\xef\xf0\xee\xf2\xe8\xe2\xed"
        "\xe8\xea\xee\xe2\x20\xe2\x20\xec\xe0\xe3\xe8\xf7\xe5\xf1\xea\xee\xec\x20\xf5\xf0\xf3\xf1\xf2\xe0"
        "\xeb\xfc\xed\xee\xec\x20\xf8\xe0\xf0\xe5\x2e"),
    DATA_COMPGEN(0x005267b0, gEventTextEmpty75, ""),
    DATA_COMPGEN(0x005267b4, gEventTextEmpty76, ""),
    DATA_COMPGEN(0x005267b8, gEventTextEmpty77, ""),
    DATA_COMPGEN(0x005267bc, gEventTextEmpty78, ""),
    DATA_COMPGEN(0x005267c0, gEventTextEmpty79, ""),
    DATA_COMPGEN(0x005267c4, gEventTextEmpty80, ""),
    // {Шатер}\n\nВаше внимание привлекает шатер, пологи которых трепещут на жарком ветру пустыни. В
    // нем никого нет. Пройдет время, и, быть может, сюда придет новый отряд кочевников.
    DATA_COMPGEN(0x005095a4, gEventTextDesertTent81,
        "\x7b\xd8\xe0\xf2\xe5\xf0\x7d\x0a\x0a\xc2\xe0\xf8\xe5\x20\xe2\xed\xe8\xec\xe0\xed\xe8\xe5\x20\xef"
        "\xf0\xe8\xe2\xeb\xe5\xea\xe0\xe5\xf2\x20\xf8\xe0\xf2\xe5\xf0\x2c\x20\xef\xee\xeb\xee\xe3\xe8\x20"
        "\xea\xee\xf2\xee\xf0\xfb\xf5\x20\xf2\xf0\xe5\xef\xe5\xf9\xf3\xf2\x20\xed\xe0\x20\xe6\xe0\xf0\xea"
        "\xee\xec\x20\xe2\xe5\xf2\xf0\xf3\x20\xef\xf3\xf1\xf2\xfb\xed\xe8\x2e\x20\xc2\x20\xed\xe5\xec\x20"
        "\xed\xe8\xea\xee\xe3\xee\x20\xed\xe5\xf2\x2e\x20\xcf\xf0\xee\xe9\xe4\xe5\xf2\x20\xe2\xf0\xe5\xec"
        "\xff\x2c\x20\xe8\x2c\x20\xe1\xfb\xf2\xfc\x20\xec\xee\xe6\xe5\xf2\x2c\x20\xf1\xfe\xe4\xe0\x20\xef"
        "\xf0\xe8\xe4\xe5\xf2\x20\xed\xee\xe2\xfb\xe9\x20\xee\xf2\xf0\xff\xe4\x20\xea\xee\xf7\xe5\xe2\xed"
        "\xe8\xea\xee\xe2\x2e"),
    // {Шатер}\n\nВаше внимание привлекают шатер, пологи которого трепещут на жарком ветру пустыни. Вы
    // хотите принять в ваше войско отряд кочевников?
    DATA_COMPGEN(0x00509654, gEventTextDesertTent82,
        "\x7b\xd8\xe0\xf2\xe5\xf0\x7d\x0a\x0a\xc2\xe0\xf8\xe5\x20\xe2\xed\xe8\xec\xe0\xed\xe8\xe5\x20\xef"
        "\xf0\xe8\xe2\xeb\xe5\xea\xe0\xfe\xf2\x20\xf8\xe0\xf2\xe5\xf0\x2c\x20\xef\xee\xeb\xee\xe3\xe8\x20"
        "\xea\xee\xf2\xee\xf0\xee\xe3\xee\x20\xf2\xf0\xe5\xef\xe5\xf9\xf3\xf2\x20\xed\xe0\x20\xe6\xe0\xf0"
        "\xea\xee\xec\x20\xe2\xe5\xf2\xf0\xf3\x20\xef\xf3\xf1\xf2\xfb\xed\xe8\x2e\x20\xc2\xfb\x20\xf5\xee"
        "\xf2\xe8\xf2\xe5\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xe2\x20\xe2\xe0\xf8\xe5\x20\xe2\xee\xe9\xf1"
        "\xea\xee\x20\xee\xf2\xf0\xff\xe4\x20\xea\xee\xf7\xe5\xe2\xed\xe8\xea\xee\xe2\x3f"),
    // {Повозка}\n\nЦветастая повозка разбойников пуста. Пройдет время, и, быть может, здесь обоснуется
    // новая шайка.
    DATA_COMPGEN(0x005096e4, gEventTextWagon83,
        "\x7b\xcf\xee\xe2\xee\xe7\xea\xe0\x7d\x0a\x0a\xd6\xe2\xe5\xf2\xe0\xf1\xf2\xe0\xff\x20\xef\xee\xe2"
        "\xee\xe7\xea\xe0\x20\xf0\xe0\xe7\xe1\xee\xe9\xed\xe8\xea\xee\xe2\x20\xef\xf3\xf1\xf2\xe0\x2e\x20"
        "\xcf\xf0\xee\xe9\xe4\xe5\xf2\x20\xe2\xf0\xe5\xec\xff\x2c\x20\xe8\x2c\x20\xe1\xfb\xf2\xfc\x20\xec"
        "\xee\xe6\xe5\xf2\x2c\x20\xe7\xe4\xe5\xf1\xfc\x20\xee\xe1\xee\xf1\xed\xf3\xe5\xf2\xf1\xff\x20\xed"
        "\xee\xe2\xe0\xff\x20\xf8\xe0\xe9\xea\xe0\x2e"),
    // {Повозка}\n\nВдалеке слышится музыка и смех. Вы идете на звуки и видите цветастую повозку, в
    // которой живут разбойники. Вы хотите принять в ваше войско шайку разбойников?
    DATA_COMPGEN(0x00509750, gEventTextWagon84,
        "\x7b\xcf\xee\xe2\xee\xe7\xea\xe0\x7d\x0a\x0a\xc2\xe4\xe0\xeb\xe5\xea\xe5\x20\xf1\xeb\xfb\xf8\xe8"
        "\xf2\xf1\xff\x20\xec\xf3\xe7\xfb\xea\xe0\x20\xe8\x20\xf1\xec\xe5\xf5\x2e\x20\xc2\xfb\x20\xe8\xe4"
        "\xe5\xf2\xe5\x20\xed\xe0\x20\xe7\xe2\xf3\xea\xe8\x20\xe8\x20\xe2\xe8\xe4\xe8\xf2\xe5\x20\xf6\xe2"
        "\xe5\xf2\xe0\xf1\xf2\xf3\xfe\x20\xef\xee\xe2\xee\xe7\xea\xf3\x2c\x20\xe2\x20\xea\xee\xf2\xee\xf0"
        "\xee\xe9\x20\xe6\xe8\xe2\xf3\xf2\x20\xf0\xe0\xe7\xe1\xee\xe9\xed\xe8\xea\xe8\x2e\x20\xc2\xfb\x20"
        "\xf5\xee\xf2\xe8\xf2\xe5\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xe2\x20\xe2\xe0\xf8\xe5\x20\xe2\xee"
        "\xe9\xf1\xea\xee\x20\xf8\xe0\xe9\xea\xf3\x20\xf0\xe0\xe7\xe1\xee\xe9\xed\xe8\xea\xee\xe2\x3f"),
    // {Водоворот}\n\nВаш корабль попадает в водоворот. Часть вашего войска исчезает в пучине.
    DATA_COMPGEN(0x005097f8, gEventTextWhirlpool85,
        "\x7b\xc2\xee\xe4\xee\xe2\xee\xf0\xee\xf2\x7d\x0a\x0a\xc2\xe0\xf8\x20\xea\xee\xf0\xe0\xe1\xeb\xfc"
        "\x20\xef\xee\xef\xe0\xe4\xe0\xe5\xf2\x20\xe2\x20\xe2\xee\xe4\xee\xe2\xee\xf0\xee\xf2\x2e\x20\xd7"
        "\xe0\xf1\xf2\xfc\x20\xe2\xe0\xf8\xe5\xe3\xee\x20\xe2\xee\xe9\xf1\xea\xe0\x20\xe8\xf1\xf7\xe5\xe7"
        "\xe0\xe5\xf2\x20\xe2\x20\xef\xf3\xf7\xe8\xed\xe5\x2e"),
    // {Ветряная мельница}\n\nМельник обращается к вам со словами: \"Сожалею, господин, но сегодня у
    // меня ничего нет. Приходите на следующей неделе.\"
    DATA_COMPGEN(0x00509850, gEventTextWindmill86,
        "\x7b\xc2\xe5\xf2\xf0\xff\xed\xe0\xff\x20\xec\xe5\xeb\xfc\xed\xe8\xf6\xe0\x7d\x0a\x0a\xcc\xe5\xeb"
        "\xfc\xed\xe8\xea\x20\xee\xe1\xf0\xe0\xf9\xe0\xe5\xf2\xf1\xff\x20\xea\x20\xe2\xe0\xec\x20\xf1\xee"
        "\x20\xf1\xeb\xee\xe2\xe0\xec\xe8\x3a\x20\x22\xd1\xee\xe6\xe0\xeb\xe5\xfe\x2c\x20\xe3\xee\xf1\xef"
        "\xee\xe4\xe8\xed\x2c\x20\xed\xee\x20\xf1\xe5\xe3\xee\xe4\xed\xff\x20\xf3\x20\xec\xe5\xed\xff\x20"
        "\xed\xe8\xf7\xe5\xe3\xee\x20\xed\xe5\xf2\x2e\x20\xcf\xf0\xe8\xf5\xee\xe4\xe8\xf2\xe5\x20\xed\xe0"
        "\x20\xf1\xeb\xe5\xe4\xf3\xfe\xf9\xe5\xe9\x20\xed\xe5\xe4\xe5\xeb\xe5\x2e\x22"),
    // {Ветряная мельница}\n\nМельник обращается к вам со словами: \"Господин, я работал не покладая
    // рук, и прошу вас принять мой скромный дар. Приходите на следующей неделе, у меня опять найдется,
    // чем вас порадовать.\"
    DATA_COMPGEN(0x005098dc, gEventTextWindmill87,
        "\x7b\xc2\xe5\xf2\xf0\xff\xed\xe0\xff\x20\xec\xe5\xeb\xfc\xed\xe8\xf6\xe0\x7d\x0a\x0a\xcc\xe5\xeb"
        "\xfc\xed\xe8\xea\x20\xee\xe1\xf0\xe0\xf9\xe0\xe5\xf2\xf1\xff\x20\xea\x20\xe2\xe0\xec\x20\xf1\xee"
        "\x20\xf1\xeb\xee\xe2\xe0\xec\xe8\x3a\x20\x22\xc3\xee\xf1\xef\xee\xe4\xe8\xed\x2c\x20\xff\x20\xf0"
        "\xe0\xe1\xee\xf2\xe0\xeb\x20\xed\xe5\x20\xef\xee\xea\xeb\xe0\xe4\xe0\xff\x20\xf0\xf3\xea\x2c\x20"
        "\xe8\x20\xef\xf0\xee\xf8\xf3\x20\xe2\xe0\xf1\x20\xef\xf0\xe8\xed\xff\xf2\xfc\x20\xec\xee\xe9\x20"
        "\xf1\xea\xf0\xee\xec\xed\xfb\xe9\x20\xe4\xe0\xf0\x2e\x20\xcf\xf0\xe8\xf5\xee\xe4\xe8\xf2\xe5\x20"
        "\xed\xe0\x20\xf1\xeb\xe5\xe4\xf3\xfe\xf9\xe5\xe9\x20\xed\xe5\xe4\xe5\xeb\xe5\x2c\x20\xf3\x20\xec"
        "\xe5\xed\xff\x20\xee\xef\xff\xf2\xfc\x20\xed\xe0\xe9\xe4\xe5\xf2\xf1\xff\x2c\x20\xf7\xe5\xec\x20"
        "\xe2\xe0\xf1\x20\xef\xee\xf0\xe0\xe4\xee\xe2\xe0\xf2\xfc\x2e\x22"),
    DATA_COMPGEN(0x005267c8, gEventTextEmpty88, ""),
    DATA_COMPGEN(0x005267cc, gEventTextEmpty89, ""),
    DATA_COMPGEN(0x005267d0, gEventTextEmpty90, ""),
    DATA_COMPGEN(0x005267d4, gEventTextEmpty91, ""),
    DATA_COMPGEN(0x005267d8, gEventTextEmpty92, ""),
    // {Скелет}\n\nВы находите останки незадачливого искателя приключений. Пошарив в груде лохмотьев,
    // вы ничего не находите.
    DATA_COMPGEN(0x005099b0, gEventTextSkeleton93,
        "\x7b\xd1\xea\xe5\xeb\xe5\xf2\x7d\x0a\x0a\xc2\xfb\x20\xed\xe0\xf5\xee\xe4\xe8\xf2\xe5\x20\xee\xf1"
        "\xf2\xe0\xed\xea\xe8\x20\xed\xe5\xe7\xe0\xe4\xe0\xf7\xeb\xe8\xe2\xee\xe3\xee\x20\xe8\xf1\xea\xe0"
        "\xf2\xe5\xeb\xff\x20\xef\xf0\xe8\xea\xeb\xfe\xf7\xe5\xed\xe8\xe9\x2e\x20\xcf\xee\xf8\xe0\xf0\xe8"
        "\xe2\x20\xe2\x20\xe3\xf0\xf3\xe4\xe5\x20\xeb\xee\xf5\xec\xee\xf2\xfc\xe5\xe2\x2c\x20\xe2\xfb\x20"
        "\xed\xe8\xf7\xe5\xe3\xee\x20\xed\xe5\x20\xed\xe0\xf5\xee\xe4\xe8\xf2\xe5\x2e"),
    // {Скелет}\n\nВы находите останки незадачливого искателя приключений. Пошарив в груде лохмотьев,
    // вы находите.
    DATA_COMPGEN(0x00509a24, gEventTextSkeleton94,
        "\x7b\xd1\xea\xe5\xeb\xe5\xf2\x7d\x0a\x0a\xc2\xfb\x20\xed\xe0\xf5\xee\xe4\xe8\xf2\xe5\x20\xee\xf1"
        "\xf2\xe0\xed\xea\xe8\x20\xed\xe5\xe7\xe0\xe4\xe0\xf7\xeb\xe8\xe2\xee\xe3\xee\x20\xe8\xf1\xea\xe0"
        "\xf2\xe5\xeb\xff\x20\xef\xf0\xe8\xea\xeb\xfe\xf7\xe5\xed\xe8\xe9\x2e\x20\xcf\xee\xf8\xe0\xf0\xe8"
        "\xe2\x20\xe2\x20\xe3\xf0\xf3\xe4\xe5\x20\xeb\xee\xf5\xec\xee\xf2\xfc\xe5\xe2\x2c\x20\xe2\xfb\x20"
        "\xed\xe0\xf5\xee\xe4\xe8\xf2\xe5\x2e")
};
DATA(0x004fe3a0) char* gCPanelHelp[KB_CONTROL_PANEL_HELP_COUNT] = {
    // Начать одиночную или сетевую игру.
    DATA_COMPGEN(0x00509a90, gCPanelHelpStartASingleOrMultiPlayer0,
        "\xcd\xe0\xf7\xe0\xf2\xfc\x20\xee\xe4\xe8\xed\xee\xf7\xed\xf3\xfe\x20\xe8\xeb\xe8\x20\xf1\xe5\xf2"
        "\xe5\xe2\xf3\xfe\x20\xe8\xe3\xf0\xf3\x2e"),
    // Загрузить сохраненную игру.
    DATA_COMPGEN(0x00509ab4, gCPanelHelpLoadAPreviouslySavedGame1,
        "\xc7\xe0\xe3\xf0\xf3\xe7\xe8\xf2\xfc\x20\xf1\xee\xf5\xf0\xe0\xed\xe5\xed\xed\xf3\xfe\x20\xe8\xe3"
        "\xf0\xf3\x2e"),
    // Сохранить игру.
    DATA_COMPGEN(0x00509ad0, gCPanelHelpSaveTheCurrentGame2, "\xd1\xee\xf5\xf0\xe0\xed\xe8\xf2\xfc\x20\xe8\xe3\xf0\xf3\x2e"),
    // Выйти из Героев Меча и Магии II.
    DATA_COMPGEN(0x00509ae0, gCPanelHelpQuitOutOfHeroesOf3,
        "\xc2\xfb\xe9\xf2\xe8\x20\xe8\xe7\x20\xc3\xe5\xf0\xee\xe5\xe2\x20\xcc\xe5\xf7\xe0\x20\xe8\x20\xcc"
        "\xe0\xe3\xe8\xe8\x20\x49\x49\x2e"),
    // Закрыть меню, ничего не делая.
    DATA_COMPGEN(0x00509b04, gCPanelHelpExitThisMenuWithoutDoing4,
        "\xc7\xe0\xea\xf0\xfb\xf2\xfc\x20\xec\xe5\xed\xfe\x2c\x20\xed\xe8\xf7\xe5\xe3\xee\x20\xed\xe5\x20"
        "\xe4\xe5\xeb\xe0\xff\x2e")
};
DATA(0x004fe3b4) char* gCSPanelHelp[KB_COMBAT_SPELL_PANEL_HELP_COUNT] = {
    // {ОК}\n\nЗакрыть это меню.
    DATA_COMPGEN(0x00509b24, gCSPanelHelpOK0, "\x7b\xce\xca\x7d\x0a\x0a\xc7\xe0\xea\xf0\xfb\xf2\xfc\x20\xfd\xf2\xee\x20\xec\xe5\xed\xfe\x2e"),
    // {Скорость}\n\nУстановить скорость действий и анимации воинов в бою.
    DATA_COMPGEN(0x00509b3c, gCSPanelHelpSpeed1,
        "\x7b\xd1\xea\xee\xf0\xee\xf1\xf2\xfc\x7d\x0a\x0a\xd3\xf1\xf2\xe0\xed\xee\xe2\xe8\xf2\xfc\x20\xf1"
        "\xea\xee\xf0\xee\xf1\xf2\xfc\x20\xe4\xe5\xe9\xf1\xf2\xe2\xe8\xe9\x20\xe8\x20\xe0\xed\xe8\xec\xe0"
        "\xf6\xe8\xe8\x20\xe2\xee\xe8\xed\xee\xe2\x20\xe2\x20\xe1\xee\xfe\x2e"),
    // {Информация о воине}\n\nВключить или выключить отображение окна с информацией о выбранном и
    // атакуемом воине.
    DATA_COMPGEN(0x00509b80, gCSPanelHelpMonsterInfo2,
        "\x7b\xc8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xff\x20\xee\x20\xe2\xee\xe8\xed\xe5\x7d\x0a\x0a\xc2\xea"
        "\xeb\xfe\xf7\xe8\xf2\xfc\x20\xe8\xeb\xe8\x20\xe2\xfb\xea\xeb\xfe\xf7\xe8\xf2\xfc\x20\xee\xf2\xee"
        "\xe1\xf0\xe0\xe6\xe5\xed\xe8\xe5\x20\xee\xea\xed\xe0\x20\xf1\x20\xe8\xed\xf4\xee\xf0\xec\xe0\xf6"
        "\xe8\xe5\xe9\x20\xee\x20\xe2\xfb\xe1\xf0\xe0\xed\xed\xee\xec\x20\xe8\x20\xe0\xf2\xe0\xea\xf3\xe5"
        "\xec\xee\xec\x20\xe2\xee\xe8\xed\xe5\x2e"),
    // {Магия в автобое}\n\nЕсли эта опция включена, ваш герой будет использовать заклинания во время
    // автобоя. (Примечание: Эта опция не влияет на использование заклинаний компьютерными игроками, и
    // на быстрый бой.)
    DATA_COMPGEN(0x00509bec, gCSPanelHelpAutoCombatSpellCasting3,
        "\x7b\xcc\xe0\xe3\xe8\xff\x20\xe2\x20\xe0\xe2\xf2\xee\xe1\xee\xe5\x7d\x0a\x0a\xc5\xf1\xeb\xe8\x20"
        "\xfd\xf2\xe0\x20\xee\xef\xf6\xe8\xff\x20\xe2\xea\xeb\xfe\xf7\xe5\xed\xe0\x2c\x20\xe2\xe0\xf8\x20"
        "\xe3\xe5\xf0\xee\xe9\x20\xe1\xf3\xe4\xe5\xf2\x20\xe8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xf2\xfc"
        "\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\x20\xe2\xee\x20\xe2\xf0\xe5\xec\xff\x20\xe0\xe2\xf2"
        "\xee\xe1\xee\xff\x2e\x20\x28\xcf\xf0\xe8\xec\xe5\xf7\xe0\xed\xe8\xe5\x3a\x20\xdd\xf2\xe0\x20\xee"
        "\xef\xf6\xe8\xff\x20\xed\xe5\x20\xe2\xeb\xe8\xff\xe5\xf2\x20\xed\xe0\x20\xe8\xf1\xef\xee\xeb\xfc"
        "\xe7\xee\xe2\xe0\xed\xe8\xe5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe9\x20\xea\xee\xec\xef\xfc"
        "\xfe\xf2\xe5\xf0\xed\xfb\xec\xe8\x20\xe8\xe3\xf0\xee\xea\xe0\xec\xe8\x2c\x20\xe8\x20\xed\xe0\x20"
        "\xe1\xfb\xf1\xf2\xf0\xfb\xe9\x20\xe1\xee\xe9\x2e\x29"),
    // {Сетка}\n\nВключает или выключает отображение сетки. Все перемещения на поле боя происходят по
    // гексагональной сетке, даже если ее отображение отключено.
    DATA_COMPGEN(0x00509cbc, gCSPanelHelpGrid4,
        "\x7b\xd1\xe5\xf2\xea\xe0\x7d\x0a\x0a\xc2\xea\xeb\xfe\xf7\xe0\xe5\xf2\x20\xe8\xeb\xe8\x20\xe2\xfb"
        "\xea\xeb\xfe\xf7\xe0\xe5\xf2\x20\xee\xf2\xee\xe1\xf0\xe0\xe6\xe5\xed\xe8\xe5\x20\xf1\xe5\xf2\xea"
        "\xe8\x2e\x20\xc2\xf1\xe5\x20\xef\xe5\xf0\xe5\xec\xe5\xf9\xe5\xed\xe8\xff\x20\xed\xe0\x20\xef\xee"
        "\xeb\xe5\x20\xe1\xee\xff\x20\xef\xf0\xee\xe8\xf1\xf5\xee\xe4\xff\xf2\x20\xef\xee\x20\xe3\xe5\xea"
        "\xf1\xe0\xe3\xee\xed\xe0\xeb\xfc\xed\xee\xe9\x20\xf1\xe5\xf2\xea\xe5\x2c\x20\xe4\xe0\xe6\xe5\x20"
        "\xe5\xf1\xeb\xe8\x20\xe5\xe5\x20\xee\xf2\xee\xe1\xf0\xe0\xe6\xe5\xed\xe8\xe5\x20\xee\xf2\xea\xeb"
        "\xfe\xf7\xe5\xed\xee\x2e"),
    // {Затенение сетки}\n\nВключает или выключает режим обозначения возможной дальности передвижения
    // выбранного отряда воинов.
    DATA_COMPGEN(0x00509d54, gCSPanelHelpShadowMovement5,
        "\x7b\xc7\xe0\xf2\xe5\xed\xe5\xed\xe8\xe5\x20\xf1\xe5\xf2\xea\xe8\x7d\x0a\x0a\xc2\xea\xeb\xfe\xf7"
        "\xe0\xe5\xf2\x20\xe8\xeb\xe8\x20\xe2\xfb\xea\xeb\xfe\xf7\xe0\xe5\xf2\x20\xf0\xe5\xe6\xe8\xec\x20"
        "\xee\xe1\xee\xe7\xed\xe0\xf7\xe5\xed\xe8\xff\x20\xe2\xee\xe7\xec\xee\xe6\xed\xee\xe9\x20\xe4\xe0"
        "\xeb\xfc\xed\xee\xf1\xf2\xe8\x20\xef\xe5\xf0\xe5\xe4\xe2\xe8\xe6\xe5\xed\xe8\xff\x20\xe2\xfb\xe1"
        "\xf0\xe0\xed\xed\xee\xe3\xee\x20\xee\xf2\xf0\xff\xe4\xe0\x20\xe2\xee\xe8\xed\xee\xe2\x2e"),
    // {Курсор с тенью}\n\nВключает или выключает отрисовку тени от курсора на сетке координат.
    DATA_COMPGEN(0x00509dcc, gCSPanelHelpShadowCursor6,
        "\x7b\xca\xf3\xf0\xf1\xee\xf0\x20\xf1\x20\xf2\xe5\xed\xfc\xfe\x7d\x0a\x0a\xc2\xea\xeb\xfe\xf7\xe0"
        "\xe5\xf2\x20\xe8\xeb\xe8\x20\xe2\xfb\xea\xeb\xfe\xf7\xe0\xe5\xf2\x20\xee\xf2\xf0\xe8\xf1\xee\xe2"
        "\xea\xf3\x20\xf2\xe5\xed\xe8\x20\xee\xf2\x20\xea\xf3\xf0\xf1\xee\xf0\xe0\x20\xed\xe0\x20\xf1\xe5"
        "\xf2\xea\xe5\x20\xea\xee\xee\xf0\xe4\xe8\xed\xe0\xf2\x2e")
};
DATA(0x004fe3d0) char* gAPanelHelp[KB_ADVENTURE_PANEL_HELP_COUNT] = {
    // Осмотреть весь мир.
    DATA_COMPGEN(0x00509e24, gAPanelHelpViewTheEntireWorld0, "\xce\xf1\xec\xee\xf2\xf0\xe5\xf2\xfc\x20\xe2\xe5\xf1\xfc\x20\xec\xe8\xf0\x2e"),
    // Посмотреть головоломку.
    DATA_COMPGEN(0x00509e38, gAPanelHelpViewTheObeliskPuzzle1, "\xcf\xee\xf1\xec\xee\xf2\xf0\xe5\xf2\xfc\x20\xe3\xee\xeb\xee\xe2\xee\xeb\xee\xec\xea\xf3\x2e"),
    // Показать информацию о сценарии, на котором идет игра.
    DATA_COMPGEN(0x00509e50, gAPanelHelpViewInformationOnTheScenario2,
        "\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xe8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xfe\x20\xee\x20\xf1\xf6"
        "\xe5\xed\xe0\xf0\xe8\xe8\x2c\x20\xed\xe0\x20\xea\xee\xf2\xee\xf0\xee\xec\x20\xe8\xe4\xe5\xf2\x20"
        "\xe8\xe3\xf0\xe0\x2e"),
    // Копать в поисках Великого артефакта.
    DATA_COMPGEN(0x00509e88, gAPanelHelpDigForTheUltimateArtifact3,
        "\xca\xee\xef\xe0\xf2\xfc\x20\xe2\x20\xef\xee\xe8\xf1\xea\xe0\xf5\x20\xc2\xe5\xeb\xe8\xea\xee\xe3"
        "\xee\x20\xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2\xe0\x2e"),
    // Закрыть это меню.
    DATA_COMPGEN(0x00509eb0, gAPanelHelpExitThisMenuWithoutDoing4, "\xc7\xe0\xea\xf0\xfb\xf2\xfc\x20\xfd\xf2\xee\x20\xec\xe5\xed\xfe\x2e")
};
DATA(0x004fe3e4) char* gInitMenuHelp[KB_INIT_MENU_HELP_COUNT] = {
    // {Новая игра}\n\nНачать отдельный сценарий или сетевую игру.
    DATA_COMPGEN(0x00509ec4, gInitMenuHelpNewGame0,
        "\x7b\xcd\xee\xe2\xe0\xff\x20\xe8\xe3\xf0\xe0\x7d\x0a\x0a\xcd\xe0\xf7\xe0\xf2\xfc\x20\xee\xf2\xe4"
        "\xe5\xeb\xfc\xed\xfb\xe9\x20\xf1\xf6\xe5\xed\xe0\xf0\xe8\xe9\x20\xe8\xeb\xe8\x20\xf1\xe5\xf2\xe5"
        "\xe2\xf3\xfe\x20\xe8\xe3\xf0\xf3\x2e"),
    // {Игры}\n\nЗагрузить ранее сохраненную игру.
    DATA_COMPGEN(0x00509f00, gInitMenuHelpLoadGame1,
        "\x7b\xc8\xe3\xf0\xfb\x7d\x0a\x0a\xc7\xe0\xe3\xf0\xf3\xe7\xe8\xf2\xfc\x20\xf0\xe0\xed\xe5\xe5\x20"
        "\xf1\xee\xf5\xf0\xe0\xed\xe5\xed\xed\xf3\xfe\x20\xe8\xe3\xf0\xf3\x2e"),
    // {Рекорды}\n\nПоказать таблицу рекордов.
    DATA_COMPGEN(0x00509f2c, gInitMenuHelpHighScores2,
        "\x7b\xd0\xe5\xea\xee\xf0\xe4\xfb\x7d\x0a\x0a\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xf2\xe0\xe1\xeb"
        "\xe8\xf6\xf3\x20\xf0\xe5\xea\xee\xf0\xe4\xee\xe2\x2e"),
    // {Авторы}\n\nПоказать перечень авторов игры.
    DATA_COMPGEN(0x00509f54, gInitMenuHelpCredits3,
        "\x7b\xc0\xe2\xf2\xee\xf0\xfb\x7d\x0a\x0a\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xef\xe5\xf0\xe5\xf7"
        "\xe5\xed\xfc\x20\xe0\xe2\xf2\xee\xf0\xee\xe2\x20\xe8\xe3\xf0\xfb\x2e"),
    // {Выйти}\n\nВыйти из героев Меча и Магии II и вернуться в операционную систему.
    DATA_COMPGEN(0x00509f80, gInitMenuHelpQuit4,
        "\x7b\xc2\xfb\xe9\xf2\xe8\x7d\x0a\x0a\xc2\xfb\xe9\xf2\xe8\x20\xe8\xe7\x20\xe3\xe5\xf0\xee\xe5\xe2"
        "\x20\xcc\xe5\xf7\xe0\x20\xe8\x20\xcc\xe0\xe3\xe8\xe8\x20\x49\x49\x20\xe8\x20\xe2\xe5\xf0\xed\xf3"
        "\xf2\xfc\xf1\xff\x20\xe2\x20\xee\xef\xe5\xf0\xe0\xf6\xe8\xee\xed\xed\xf3\xfe\x20\xf1\xe8\xf1\xf2"
        "\xe5\xec\xf3\x2e")
};
DATA(0x004fe3f8) char* gAdvMenuHelp[KB_ADVENTURE_MENU_HELP_COUNT] = {
    // {Следующий герой}\n\nВыбрать следующего героя.
    DATA_COMPGEN(0x00509fd0, gAdvMenuHelpNextHero0,
        "\x7b\xd1\xeb\xe5\xe4\xf3\xfe\xf9\xe8\xe9\x20\xe3\xe5\xf0\xee\xe9\x7d\x0a\x0a\xc2\xfb\xe1\xf0\xe0"
        "\xf2\xfc\x20\xf1\xeb\xe5\xe4\xf3\xfe\xf9\xe5\xe3\xee\x20\xe3\xe5\xf0\xee\xff\x2e"),
    // {Продолжить движение}\n\nПродолжить движение героя по намеченному пути.
    DATA_COMPGEN(0x0050a000, gAdvMenuHelpContinueMovement1,
        "\x7b\xcf\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc\x20\xe4\xe2\xe8\xe6\xe5\xed\xe8\xe5\x7d\x0a\x0a\xcf"
        "\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc\x20\xe4\xe2\xe8\xe6\xe5\xed\xe8\xe5\x20\xe3\xe5\xf0\xee\xff"
        "\x20\xef\xee\x20\xed\xe0\xec\xe5\xf7\xe5\xed\xed\xee\xec\xf3\x20\xef\xf3\xf2\xe8\x2e"),
    // {Обзор королевства}\n\nОсмотреть ваши владения.
    DATA_COMPGEN(0x0050a048, gAdvMenuHelpKingdomSummary2,
        "\x7b\xce\xe1\xe7\xee\xf0\x20\xea\xee\xf0\xee\xeb\xe5\xe2\xf1\xf2\xe2\xe0\x7d\x0a\x0a\xce\xf1\xec"
        "\xee\xf2\xf0\xe5\xf2\xfc\x20\xe2\xe0\xf8\xe8\x20\xe2\xeb\xe0\xe4\xe5\xed\xe8\xff\x2e"),
    // {Окончить ход}\n\nОкончить ход и передать управление компьютеру.
    DATA_COMPGEN(0x0050a078, gAdvMenuHelpEndTurn3,
        "\x7b\xce\xea\xee\xed\xf7\xe8\xf2\xfc\x20\xf5\xee\xe4\x7d\x0a\x0a\xce\xea\xee\xed\xf7\xe8\xf2\xfc"
        "\x20\xf5\xee\xe4\x20\xe8\x20\xef\xe5\xf0\xe5\xe4\xe0\xf2\xfc\x20\xf3\xef\xf0\xe0\xe2\xeb\xe5\xed"
        "\xe8\xe5\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xf3\x2e"),
    // {Игровые действия}\n\nОткрыть окно доступных игровых действий.
    DATA_COMPGEN(0x0050a0b8, gAdvMenuHelpAdventureOptions4,
        "\x7b\xc8\xe3\xf0\xee\xe2\xfb\xe5\x20\xe4\xe5\xe9\xf1\xf2\xe2\xe8\xff\x7d\x0a\x0a\xce\xf2\xea\xf0"
        "\xfb\xf2\xfc\x20\xee\xea\xed\xee\x20\xe4\xee\xf1\xf2\xf3\xef\xed\xfb\xf5\x20\xe8\xe3\xf0\xee\xe2"
        "\xfb\xf5\x20\xe4\xe5\xe9\xf1\xf2\xe2\xe8\xe9\x2e"),
    // {Окно файлов}\n\nОткрывает меню, где вы можете загружать или сохранять игры.
    DATA_COMPGEN(0x0050a0f8, gAdvMenuHelpFileOptions5,
        "\x7b\xce\xea\xed\xee\x20\xf4\xe0\xe9\xeb\xee\xe2\x7d\x0a\x0a\xce\xf2\xea\xf0\xfb\xe2\xe0\xe5\xf2"
        "\x20\xec\xe5\xed\xfe\x2c\x20\xe3\xe4\xe5\x20\xe2\xfb\x20\xec\xee\xe6\xe5\xf2\xe5\x20\xe7\xe0\xe3"
        "\xf0\xf3\xe6\xe0\xf2\xfc\x20\xe8\xeb\xe8\x20\xf1\xee\xf5\xf0\xe0\xed\xff\xf2\xfc\x20\xe8\xe3\xf0"
        "\xfb\x2e"),
    // {Системные настройки}\n\nОткрывает окно системных настроек, позволяющих настроить игру.
    DATA_COMPGEN(0x0050a144, gAdvMenuHelpSystemOptions6,
        "\x7b\xd1\xe8\xf1\xf2\xe5\xec\xed\xfb\xe5\x20\xed\xe0\xf1\xf2\xf0\xee\xe9\xea\xe8\x7d\x0a\x0a\xce"
        "\xf2\xea\xf0\xfb\xe2\xe0\xe5\xf2\x20\xee\xea\xed\xee\x20\xf1\xe8\xf1\xf2\xe5\xec\xed\xfb\xf5\x20"
        "\xed\xe0\xf1\xf2\xf0\xee\xe5\xea\x2c\x20\xef\xee\xe7\xe2\xee\xeb\xff\xfe\xf9\xe8\xf5\x20\xed\xe0"
        "\xf1\xf2\xf0\xee\xe8\xf2\xfc\x20\xe8\xe3\xf0\xf3\x2e"),
    // {Направить заклинание}\n\nНаправить заклинание на стратегической карте.
    DATA_COMPGEN(0x0050a19c, gAdvMenuHelpCastSpell7,
        "\x7b\xcd\xe0\xef\xf0\xe0\xe2\xe8\xf2\xfc\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5\x7d\x0a\x0a"
        "\xcd\xe0\xef\xf0\xe0\xe2\xe8\xf2\xfc\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5\x20\xed\xe0\x20"
        "\xf1\xf2\xf0\xe0\xf2\xe5\xe3\xe8\xf7\xe5\xf1\xea\xee\xe9\x20\xea\xe0\xf0\xf2\xe5\x2e")
};
DATA(0x004fe418) char* gLuckText[KB_LUCK_TEXT_COUNT] = {
    // Проклятая
    DATA_COMPGEN(0x0050a1e4, gLuckTextCursed0, "\xcf\xf0\xee\xea\xeb\xff\xf2\xe0\xff"),
    // Ужасная
    DATA_COMPGEN(0x0050a1f0, gLuckTextAwful1, "\xd3\xe6\xe0\xf1\xed\xe0\xff"),
    // Плохая
    DATA_COMPGEN(0x0050a1f8, gLuckTextBad2, "\xcf\xeb\xee\xf5\xe0\xff"),
    // Обычная
    DATA_COMPGEN(0x0050a200, gLuckTextNormal3, "\xce\xe1\xfb\xf7\xed\xe0\xff"),
    // Хорошая
    DATA_COMPGEN(0x0050a208, gLuckTextGood4, "\xd5\xee\xf0\xee\xf8\xe0\xff"),
    // Отличная
    DATA_COMPGEN(0x0050a210, gLuckTextGreat5, "\xce\xf2\xeb\xe8\xf7\xed\xe0\xff"),
    // Божественная
    DATA_COMPGEN(0x0050a21c, gLuckTextIrish6, "\xc1\xee\xe6\xe5\xf1\xf2\xe2\xe5\xed\xed\xe0\xff")
};
DATA(0x004fe434) char* gMoraleText[KB_MORALE_TEXT_COUNT] = {
    // Предательская
    DATA_COMPGEN(0x0050a22c, gMoraleTextTreason0, "\xcf\xf0\xe5\xe4\xe0\xf2\xe5\xeb\xfc\xf1\xea\xe0\xff"),
    // Ужасная
    DATA_COMPGEN(0x0050a23c, gMoraleTextAwful1, "\xd3\xe6\xe0\xf1\xed\xe0\xff"),
    // Плохая
    DATA_COMPGEN(0x0050a244, gMoraleTextPoor2, "\xcf\xeb\xee\xf5\xe0\xff"),
    // Обычная
    DATA_COMPGEN(0x0050a24c, gMoraleTextNormal3, "\xce\xe1\xfb\xf7\xed\xe0\xff"),
    // Хорошая
    DATA_COMPGEN(0x0050a254, gMoraleTextGood4, "\xd5\xee\xf0\xee\xf8\xe0\xff"),
    // Отличная
    DATA_COMPGEN(0x0050a25c, gMoraleTextGreat5, "\xce\xf2\xeb\xe8\xf7\xed\xe0\xff"),
    // Кровавая!
    DATA_COMPGEN(0x0050a268, gMoraleTextBlood6, "\xca\xf0\xee\xe2\xe0\xe2\xe0\xff\x21")
};
DATA(0x004fe450) char* onOffText[KB_ON_OFF_TEXT_COUNT] = {
    // Выкл.
    DATA_COMPGEN(0x0050a274, onOffTextOff0, "\xc2\xfb\xea\xeb\x2e"),
    // Вкл.
    DATA_COMPGEN(0x0050a27c, onOffTextOn1, "\xc2\xea\xeb\x2e"),
    // Вкл.\nГромкость 9
    DATA_COMPGEN(0x0050a284, onOffTextOnVolume92, "\xc2\xea\xeb\x2e\x0a\xc3\xf0\xee\xec\xea\xee\xf1\xf2\xfc\x20\x39"),
    // Вкл.\nГромкость 8
    DATA_COMPGEN(0x0050a298, onOffTextOnVolume83, "\xc2\xea\xeb\x2e\x0a\xc3\xf0\xee\xec\xea\xee\xf1\xf2\xfc\x20\x38"),
    // Вкл.\nГромкость 7
    DATA_COMPGEN(0x0050a2ac, onOffTextOnVolume74, "\xc2\xea\xeb\x2e\x0a\xc3\xf0\xee\xec\xea\xee\xf1\xf2\xfc\x20\x37"),
    // Вкл.\nГромкость 6
    DATA_COMPGEN(0x0050a2c0, onOffTextOnVolume65, "\xc2\xea\xeb\x2e\x0a\xc3\xf0\xee\xec\xea\xee\xf1\xf2\xfc\x20\x36"),
    // Вкл.\nГромкость 5
    DATA_COMPGEN(0x0050a2d4, onOffTextOnVolume56, "\xc2\xea\xeb\x2e\x0a\xc3\xf0\xee\xec\xea\xee\xf1\xf2\xfc\x20\x35"),
    // Вкл.\nГромкость 4
    DATA_COMPGEN(0x0050a2e8, onOffTextOnVolume47, "\xc2\xea\xeb\x2e\x0a\xc3\xf0\xee\xec\xea\xee\xf1\xf2\xfc\x20\x34"),
    // Вкл.\nГромкость 3
    DATA_COMPGEN(0x0050a2fc, onOffTextOnVolume38, "\xc2\xea\xeb\x2e\x0a\xc3\xf0\xee\xec\xea\xee\xf1\xf2\xfc\x20\x33"),
    // Вкл.\nГромкость 2
    DATA_COMPGEN(0x0050a310, onOffTextOnVolume29, "\xc2\xea\xeb\x2e\x0a\xc3\xf0\xee\xec\xea\xee\xf1\xf2\xfc\x20\x32"),
    // Вкл.\nГромкость 1
    DATA_COMPGEN(0x0050a324, onOffTextOnVolume110, "\xc2\xea\xeb\x2e\x0a\xc3\xf0\xee\xec\xea\xee\xf1\xf2\xfc\x20\x31")
};
DATA(0x004fe47c) char* walkSpeedText[KB_WALK_SPEED_TEXT_COUNT] = {
    // Шагом
    DATA_COMPGEN(0x0050a338, walkSpeedTextWalk0, "\xd8\xe0\xe3\xee\xec"),
    // Рысью
    DATA_COMPGEN(0x0050a340, walkSpeedTextTrot1, "\xd0\xfb\xf1\xfc\xfe"),
    // Аллюром
    DATA_COMPGEN(0x0050a348, walkSpeedTextCanter2, "\xc0\xeb\xeb\xfe\xf0\xee\xec"),
    // Галопом
    DATA_COMPGEN(0x0050a350, walkSpeedTextGallop3, "\xc3\xe0\xeb\xee\xef\xee\xec"),
    // Прыжками
    DATA_COMPGEN(0x0050a358, walkSpeedTextJump4, "\xcf\xf0\xfb\xe6\xea\xe0\xec\xe8")
};
DATA(0x004fe490) char* gColors[IDX(FACTION_COUNT)] = {
    DATA_COMPGEN(0x0050a364, gColorsBlue, "\xf1\xe8\xed\xe8\xe9" /* "синий" */),
    DATA_COMPGEN(0x0050a36c, gColorsGreen, "\xe7\xe5\xeb\xe5\xed\xfb\xe9" /* "зеленый" */),
    DATA_COMPGEN(0x0050a374, gColorsRed, "\xea\xf0\xe0\xf1\xed\xfb\xe9" /* "красный" */),
    DATA_COMPGEN(0x0050a37c, gColorsYellow, "\xe6\xe5\xeb\xf2\xfb\xe9" /* "желтый" */),
    DATA_COMPGEN(0x0050a384, gColorsOrange, "\xee\xf0\xe0\xed\xe6\xe5\xe2\xfb\xe9" /* "оранжевый" */),
    DATA_COMPGEN(0x0050a390, gColorsPurple, "\xf4\xe8\xee\xeb\xe5\xf2\xee\xe2\xfb\xe9" /* "фиолетовый" */)
};
DATA(0x004fe4a8) static char* gColorAbbreviations[IDX(FACTION_COUNT)] = {
    DATA_COMPGEN(0x0050a39c, gColorAbbreviationBlue, "\xf1\xe8\xed." /* "син." */),
    DATA_COMPGEN(0x0050a3a4, gColorAbbreviationGreen, "\xe7\xe5\xeb." /* "зел." */),
    DATA_COMPGEN(0x0050a3ac, gColorAbbreviationRed, "\xea\xf0." /* "кр." */),
    DATA_COMPGEN(0x0050a3b0, gColorAbbreviationYellow, "\xe6\xe5\xeb." /* "жел." */),
    DATA_COMPGEN(0x0050a3b8, gColorAbbreviationOrange, "\xee\xf0." /* "ор." */),
    DATA_COMPGEN(0x0050a3bc, gColorAbbreviationPurple, "\xf4\xe8\xee\xeb." /* "фиол." */)
};
DATA(0x004fe4c0) char* gMonthNames[KB_MONTH_NAME_COUNT] = {
    // Кузнечика
    DATA_COMPGEN(0x0050a3c4, gMonthNamesGrasshopper0, "\xca\xf3\xe7\xed\xe5\xf7\xe8\xea\xe0"),
    // Муравья
    DATA_COMPGEN(0x0050a3d0, gMonthNamesAnt1, "\xcc\xf3\xf0\xe0\xe2\xfc\xff"),
    // Стрекозы
    DATA_COMPGEN(0x0050a3d8, gMonthNamesDragonfly2, "\xd1\xf2\xf0\xe5\xea\xee\xe7\xfb"),
    // Паука
    DATA_COMPGEN(0x0050a3e4, gMonthNamesSpider3, "\xcf\xe0\xf3\xea\xe0"),
    // Бабочки
    DATA_COMPGEN(0x0050a3ec, gMonthNamesButterfly4, "\xc1\xe0\xe1\xee\xf7\xea\xe8"),
    // Шмеля
    DATA_COMPGEN(0x0050a3f4, gMonthNamesBumblebee5, "\xd8\xec\xe5\xeb\xff"),
    // Цикады
    DATA_COMPGEN(0x0050a3fc, gMonthNamesLocust6, "\xd6\xe8\xea\xe0\xe4\xfb"),
    // Земляного червя
    DATA_COMPGEN(0x0050a404, gMonthNamesEarthworm7, "\xc7\xe5\xec\xeb\xff\xed\xee\xe3\xee\x20\xf7\xe5\xf0\xe2\xff"),
    // Шершня
    DATA_COMPGEN(0x0050a414, gMonthNamesHornet8, "\xd8\xe5\xf0\xf8\xed\xff"),
    // Жука
    DATA_COMPGEN(0x0050a41c, gMonthNamesBeetle9, "\xc6\xf3\xea\xe0")
};
DATA(0x004fe4e8) char* gWeekNames[KB_WEEK_NAME_COUNT] = {
    // Белки
    DATA_COMPGEN(0x0050a424, gWeekNamesSquirrel0, "\xc1\xe5\xeb\xea\xe8"),
    // Кролика
    DATA_COMPGEN(0x0050a42c, gWeekNamesRabbit1, "\xca\xf0\xee\xeb\xe8\xea\xe0"),
    // Суслика
    DATA_COMPGEN(0x0050a434, gWeekNamesGopher2, "\xd1\xf3\xf1\xeb\xe8\xea\xe0"),
    // Барсука
    DATA_COMPGEN(0x0050a43c, gWeekNamesBadger3, "\xc1\xe0\xf0\xf1\xf3\xea\xe0"),
    // Крысы
    DATA_COMPGEN(0x0050a444, gWeekNamesRat4, "\xca\xf0\xfb\xf1\xfb"),
    // Орла
    DATA_COMPGEN(0x0050a44c, gWeekNamesEagle5, "\xce\xf0\xeb\xe0"),
    // Горностая
    DATA_COMPGEN(0x0050a454, gWeekNamesWeasel6, "\xc3\xee\xf0\xed\xee\xf1\xf2\xe0\xff"),
    // Ворона
    DATA_COMPGEN(0x0050a460, gWeekNamesRaven7, "\xc2\xee\xf0\xee\xed\xe0"),
    // Мангуста
    DATA_COMPGEN(0x0050a468, gWeekNamesMongoose8, "\xcc\xe0\xed\xe3\xf3\xf1\xf2\xe0"),
    // Собаки
    DATA_COMPGEN(0x0050a474, gWeekNamesDog9, "\xd1\xee\xe1\xe0\xea\xe8"),
    // Муравьеда
    DATA_COMPGEN(0x0050a47c, gWeekNamesAardvark10, "\xcc\xf3\xf0\xe0\xe2\xfc\xe5\xe4\xe0"),
    // Ящерицы
    DATA_COMPGEN(0x0050a488, gWeekNamesLizard11, "\xdf\xf9\xe5\xf0\xe8\xf6\xfb"),
    // Черепахи
    DATA_COMPGEN(0x0050a490, gWeekNamesTortoise12, "\xd7\xe5\xf0\xe5\xef\xe0\xf5\xe8"),
    // Дикобраза
    DATA_COMPGEN(0x0050a49c, gWeekNamesHedgehog13, "\xc4\xe8\xea\xee\xe1\xf0\xe0\xe7\xe0"),
    // Кондора
    DATA_COMPGEN(0x0050a4a8, gWeekNamesCondor14, "\xca\xee\xed\xe4\xee\xf0\xe0")
};
DATA(0x004fe524) char* cHeroScreen[KB_HERO_SCREEN_TEXT_COUNT] = {
    // Обзор королевства
    DATA_COMPGEN(0x0050a4b0, cHeroScreenKingdomOverview0, "\xce\xe1\xe7\xee\xf0\x20\xea\xee\xf0\xee\xeb\xe5\xe2\xf1\xf2\xe2\xe0"),
    // %s - информация
    DATA_COMPGEN(0x0050a4c4, cHeroScreenViewFormatInfo1, "\x25\x73\x20\x2d\x20\xe8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xff"),
    // Дополнительная статистика героя
    DATA_COMPGEN(0x0050a4d4, cHeroScreenAdditionalHeroCharacteristics2,
        "\xc4\xee\xef\xee\xeb\xed\xe8\xf2\xe5\xeb\xfc\xed\xe0\xff\x20\xf1\xf2\xe0\xf2\xe8\xf1\xf2\xe8\xea"
        "\xe0\x20\xe3\xe5\xf0\xee\xff"),
    // Информация о высокой морали
    DATA_COMPGEN(0x0050a4f4, cHeroScreenViewGoodMoraleInfo3,
        "\xc8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xff\x20\xee\x20\xe2\xfb\xf1\xee\xea\xee\xe9\x20\xec\xee\xf0"
        "\xe0\xeb\xe8"),
    // Информация об обычной морали
    DATA_COMPGEN(0x0050a510, cHeroScreenViewNeutralMoraleInfo4,
        "\xc8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xff\x20\xee\xe1\x20\xee\xe1\xfb\xf7\xed\xee\xe9\x20\xec\xee"
        "\xf0\xe0\xeb\xe8"),
    // Информация о плохой морали
    DATA_COMPGEN(0x0050a530, cHeroScreenViewBadMoraleInfo5,
        "\xc8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xff\x20\xee\x20\xef\xeb\xee\xf5\xee\xe9\x20\xec\xee\xf0\xe0"
        "\xeb\xe8"),
    // Информация о хорошей удаче
    DATA_COMPGEN(0x0050a54c, cHeroScreenViewGoodLuckInfo6,
        "\xc8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xff\x20\xee\x20\xf5\xee\xf0\xee\xf8\xe5\xe9\x20\xf3\xe4\xe0"
        "\xf7\xe5"),
    // Информация об обычной удаче
    DATA_COMPGEN(0x0050a568, cHeroScreenViewNeutralLuckInfo7,
        "\xc8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xff\x20\xee\xe1\x20\xee\xe1\xfb\xf7\xed\xee\xe9\x20\xf3\xe4"
        "\xe0\xf7\xe5"),
    // Информация о плохой удаче
    DATA_COMPGEN(0x0050a584, cHeroScreenViewBadLuckInfo8,
        "\xc8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xff\x20\xee\x20\xef\xeb\xee\xf5\xee\xe9\x20\xf3\xe4\xe0\xf7"
        "\xe5"),
    // Показать опыт
    DATA_COMPGEN(0x0050a5a0, cHeroScreenViewExperienceInfo9, "\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xee\xef\xfb\xf2"),
    // Выбрать %s
    DATA_COMPGEN(0x0050a5b0, cHeroScreenSelectFormat10, "\xc2\xfb\xe1\xf0\xe0\xf2\xfc\x20\x25\x73"),
    // Пусто
    DATA_COMPGEN(0x0050a5bc, cHeroScreenEmpty11, "\xcf\xf3\xf1\xf2\xee"),
    // Перенести сюда отряд %s
    DATA_COMPGEN(0x0050a5c4, cHeroScreenMoveFormat12, "\xcf\xe5\xf0\xe5\xed\xe5\xf1\xf2\xe8\x20\xf1\xfe\xe4\xe0\x20\xee\xf2\xf0\xff\xe4\x20\x25\x73"),
    // Отряды %s и %s меняются местами
    DATA_COMPGEN(0x0050a5dc, cHeroScreenExchangeFormatWithFormat13,
        "\xce\xf2\xf0\xff\xe4\xfb\x20\x25\x73\x20\xe8\x20\x25\x73\x20\xec\xe5\xed\xff\xfe\xf2\xf1\xff\x20"
        "\xec\xe5\xf1\xf2\xe0\xec\xe8"),
    // Показать заклинания
    DATA_COMPGEN(0x0050a5fc, cHeroScreenViewSpells14, "\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff"),
    // Посмотреть информацию об: %s
    DATA_COMPGEN(0x0050a610, cHeroScreenViewFormatInfo15,
        "\xcf\xee\xf1\xec\xee\xf2\xf0\xe5\xf2\xfc\x20\xe8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xfe\x20\xee\xe1"
        "\x3a\x20\x25\x73"),
    // %s %s - уволить
    DATA_COMPGEN(0x0050a630, cHeroScreenDismissFormatTheFormat16, "\x25\x73\x20\x25\x73\x20\x2d\x20\xf3\xe2\xee\xeb\xe8\xf2\xfc"),
    // Закрыть экран героя
    DATA_COMPGEN(0x0050a640, cHeroScreenExitHeroScreen17, "\xc7\xe0\xea\xf0\xfb\xf2\xfc\x20\xfd\xea\xf0\xe0\xed\x20\xe3\xe5\xf0\xee\xff"),
    // Экран героя
    DATA_COMPGEN(0x0050a654, cHeroScreenHeroScreen18, "\xdd\xea\xf0\xe0\xed\x20\xe3\xe5\xf0\xee\xff"),
    // %s в один отряд
    DATA_COMPGEN(0x0050a660, cHeroScreenCombineFormat19, "\x25\x73\x20\xe2\x20\xee\xe4\xe8\xed\x20\xee\xf2\xf0\xff\xe4"),
    // Разделить отряд %s
    DATA_COMPGEN(0x0050a670, cHeroScreenSplitFormat20, "\xd0\xe0\xe7\xe4\xe5\xeb\xe8\xf2\xfc\x20\xee\xf2\xf0\xff\xe4\x20\x25\x73"),
    // %s %s - информация
    DATA_COMPGEN(0x0050a684, cHeroScreenViewFormatFormatInfo21, "\x25\x73\x20\x25\x73\x20\x2d\x20\xe8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xff"),
    // Информация об очках магии
    DATA_COMPGEN(0x0050a698, cHeroScreenViewSpellPointsInfo22,
        "\xc8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xff\x20\xee\xe1\x20\xee\xf7\xea\xe0\xf5\x20\xec\xe0\xe3\xe8"
        "\xe8"),
    // Выбрать широкие ряды в бою
    DATA_COMPGEN(0x0050a6b4, cHeroScreenSetArmyCombatFormationTo23,
        "\xc2\xfb\xe1\xf0\xe0\xf2\xfc\x20\xf8\xe8\xf0\xee\xea\xe8\xe5\x20\xf0\xff\xe4\xfb\x20\xe2\x20\xe1"
        "\xee\xfe"),
    // Сгруппировать воинов
    DATA_COMPGEN(0x0050a6d0, cHeroScreenSetArmyCombatFormationTo24, "\xd1\xe3\xf0\xf3\xef\xef\xe8\xf0\xee\xe2\xe0\xf2\xfc\x20\xe2\xee\xe8\xed\xee\xe2")
};
DATA(0x004fe588) char* cCastleInfo[KB_CASTLE_INFO_TEXT_COUNT] = {
    // Построить Гильдию магов
    DATA_COMPGEN(0x0050a6e8, cCastleInfoBuildMageGuild0, "\xcf\xee\xf1\xf2\xf0\xee\xe8\xf2\xfc\x20\xc3\xe8\xeb\xfc\xe4\xe8\xfe\x20\xec\xe0\xe3\xee\xe2"),
    // Построены все этажи Гильдии магов.
    DATA_COMPGEN(0x0050a700, cCastleInfoMageGuildIsAtHighest1,
        "\xcf\xee\xf1\xf2\xf0\xee\xe5\xed\xfb\x20\xe2\xf1\xe5\x20\xfd\xf2\xe0\xe6\xe8\x20\xc3\xe8\xeb\xfc"
        "\xe4\xe8\xe8\x20\xec\xe0\xe3\xee\xe2\x2e"),
    // Нельзя построить следующий этаж.
    DATA_COMPGEN(0x0050a724, cCastleInfoCannotAffordNextLevel2,
        "\xcd\xe5\xeb\xfc\xe7\xff\x20\xef\xee\xf1\xf2\xf0\xee\xe8\xf2\xfc\x20\xf1\xeb\xe5\xe4\xf3\xfe\xf9"
        "\xe8\xe9\x20\xfd\xf2\xe0\xe6\x2e"),
    // Построить следующий этаж Гильдии магов
    DATA_COMPGEN(0x0050a748, cCastleInfoAddAnotherLevelToMage3,
        "\xcf\xee\xf1\xf2\xf0\xee\xe8\xf2\xfc\x20\xf1\xeb\xe5\xe4\xf3\xfe\xf9\xe8\xe9\x20\xfd\xf2\xe0\xe6"
        "\x20\xc3\xe8\xeb\xfc\xe4\xe8\xe8\x20\xec\xe0\xe3\xee\xe2\x20"),
    // Постройка '%s' уже возведена
    DATA_COMPGEN(0x0050a770, cCastleInfoFormatIsAlreadyBuilt4,
        "\xcf\xee\xf1\xf2\xf0\xee\xe9\xea\xe0\x20\x27\x25\x73\x27\x20\xf3\xe6\xe5\x20\xe2\xee\xe7\xe2\xe5"
        "\xe4\xe5\xed\xe0"),
    // Нельзя возвести постройку '%s'
    DATA_COMPGEN(0x0050a790, cCastleInfoCannotBuildFormat5,
        "\xcd\xe5\xeb\xfc\xe7\xff\x20\xe2\xee\xe7\xe2\xe5\xf1\xf2\xe8\x20\xef\xee\xf1\xf2\xf0\xee\xe9\xea"
        "\xf3\x20\x27\x25\x73\x27"),
    // Нельзя возвести постройку '%s'
    DATA_COMPGEN(0x0050a7b0, cCastleInfoCannotAffordFormat6,
        "\xcd\xe5\xeb\xfc\xe7\xff\x20\xe2\xee\xe7\xe2\xe5\xf1\xf2\xe8\x20\xef\xee\xf1\xf2\xf0\xee\xe9\xea"
        "\xf3\x20\x27\x25\x73\x27"),
    // Возвести постройку '%s'
    DATA_COMPGEN(0x0050a7d0, cCastleInfoBuildFormat7, "\xc2\xee\xe7\xe2\xe5\xf1\xf2\xe8\x20\xef\xee\xf1\xf2\xf0\xee\xe9\xea\xf3\x20\x27\x25\x73\x27"),
    // Герой вам не по карману.
    DATA_COMPGEN(0x0050a7e8, cCastleInfoCannotAffordAHero8, "\xc3\xe5\xf0\xee\xe9\x20\xe2\xe0\xec\x20\xed\xe5\x20\xef\xee\x20\xea\xe0\xf0\xec\xe0\xed\xf3\x2e"),
    // Нельзя нанять - у вас уже %d героев.
    DATA_COMPGEN(0x0050a804, cCastleInfoCannotRecruitYouAlready9,
        "\xcd\xe5\xeb\xfc\xe7\xff\x20\xed\xe0\xed\xff\xf2\xfc\x20\x2d\x20\xf3\x20\xe2\xe0\xf1\x20\xf3\xe6"
        "\xe5\x20\x25\x64\x20\xe3\xe5\xf0\xee\xe5\xe2\x2e"),
    // Нельзя нанять - в этом городе у вас уже есть герой.
    DATA_COMPGEN(0x0050a82c, cCastleInfoCannotRecruitYouAlready10,
        "\xcd\xe5\xeb\xfc\xe7\xff\x20\xed\xe0\xed\xff\xf2\xfc\x20\x2d\x20\xe2\x20\xfd\xf2\xee\xec\x20\xe3"
        "\xee\xf0\xee\xe4\xe5\x20\xf3\x20\xe2\xe0\xf1\x20\xf3\xe6\xe5\x20\xe5\xf1\xf2\xfc\x20\xe3\xe5\xf0"
        "\xee\xe9\x2e"),
    // Нанять нового героя
    DATA_COMPGEN(0x0050a860, cCastleInfoRecruitFormatTheFormat11, "\xcd\xe0\xed\xff\xf2\xfc\x20\xed\xee\xe2\xee\xe3\xee\x20\xe3\xe5\xf0\xee\xff"),
    // Выйти из замка
    DATA_COMPGEN(0x0050a874, cCastleInfoExitCastle12, "\xc2\xfb\xe9\xf2\xe8\x20\xe8\xe7\x20\xe7\xe0\xec\xea\xe0"),
    // Возможности замка
    DATA_COMPGEN(0x0050a884, cCastleInfoCastleOptions13, "\xc2\xee\xe7\xec\xee\xe6\xed\xee\xf1\xf2\xe8\x20\xe7\xe0\xec\xea\xe0"),
    // Сгруппировать гарнизон
    DATA_COMPGEN(0x0050a898, cCastleInfoSetGarrisonCombatFormationTo14, "\xd1\xe3\xf0\xf3\xef\xef\xe8\xf0\xee\xe2\xe0\xf2\xfc\x20\xe3\xe0\xf0\xed\xe8\xe7\xee\xed"),
    // Выбрать широкие ряды для гарнизона
    DATA_COMPGEN(0x0050a8b0, cCastleInfoSetGarrisonCombatFormationTo15,
        "\xc2\xfb\xe1\xf0\xe0\xf2\xfc\x20\xf8\xe8\xf0\xee\xea\xe8\xe5\x20\xf0\xff\xe4\xfb\x20\xe4\xeb\xff"
        "\x20\xe3\xe0\xf0\xed\xe8\xe7\xee\xed\xe0")
};
DATA(0x004fe5c8) char* cLuckInfo[KB_LUCK_INFO_TEXT_COUNT] = {
    // {Хорошая удача}\n\nЕсли удача вашего войска выше обычной, атаки отдельных отрядов на поле боя
    // иногда оказываются более результативными (их сила удваивается).
    DATA_COMPGEN(0x0050a8d4, cLuckInfoGoodLuck0,
        "\x7b\xd5\xee\xf0\xee\xf8\xe0\xff\x20\xf3\xe4\xe0\xf7\xe0\x7d\x0a\x0a\xc5\xf1\xeb\xe8\x20\xf3\xe4"
        "\xe0\xf7\xe0\x20\xe2\xe0\xf8\xe5\xe3\xee\x20\xe2\xee\xe9\xf1\xea\xe0\x20\xe2\xfb\xf8\xe5\x20\xee"
        "\xe1\xfb\xf7\xed\xee\xe9\x2c\x20\xe0\xf2\xe0\xea\xe8\x20\xee\xf2\xe4\xe5\xeb\xfc\xed\xfb\xf5\x20"
        "\xee\xf2\xf0\xff\xe4\xee\xe2\x20\xed\xe0\x20\xef\xee\xeb\xe5\x20\xe1\xee\xff\x20\xe8\xed\xee\xe3"
        "\xe4\xe0\x20\xee\xea\xe0\xe7\xfb\xe2\xe0\xfe\xf2\xf1\xff\x20\xe1\xee\xeb\xe5\xe5\x20\xf0\xe5\xe7"
        "\xf3\xeb\xfc\xf2\xe0\xf2\xe8\xe2\xed\xfb\xec\xe8\x20\x28\xe8\xf5\x20\xf1\xe8\xeb\xe0\x20\xf3\xe4"
        "\xe2\xe0\xe8\xe2\xe0\xe5\xf2\xf1\xff\x29\x2e"),
    // {Обычная удача}\n\nС обычной удачей ваше войско не имеет ни преимуществ, ни недостатков на поле
    // боя.
    DATA_COMPGEN(0x0050a970, cLuckInfoNeutralLuck1,
        "\x7b\xce\xe1\xfb\xf7\xed\xe0\xff\x20\xf3\xe4\xe0\xf7\xe0\x7d\x0a\x0a\xd1\x20\xee\xe1\xfb\xf7\xed"
        "\xee\xe9\x20\xf3\xe4\xe0\xf7\xe5\xe9\x20\xe2\xe0\xf8\xe5\x20\xe2\xee\xe9\xf1\xea\xee\x20\xed\xe5"
        "\x20\xe8\xec\xe5\xe5\xf2\x20\xed\xe8\x20\xef\xf0\xe5\xe8\xec\xf3\xf9\xe5\xf1\xf2\xe2\x2c\x20\xed"
        "\xe8\x20\xed\xe5\xe4\xee\xf1\xf2\xe0\xf2\xea\xee\xe2\x20\xed\xe0\x20\xef\xee\xeb\xe5\x20\xe1\xee"
        "\xff\x2e"),
    // {Плохая удача}\n\nЕсли вашему войску не везет, урон, наносимый  отдельными отрядами на поле боя,
    // может оказаться вдвое меньше обычного.
    DATA_COMPGEN(0x0050a9d4, cLuckInfoBadLuck2,
        "\x7b\xcf\xeb\xee\xf5\xe0\xff\x20\xf3\xe4\xe0\xf7\xe0\x7d\x0a\x0a\xc5\xf1\xeb\xe8\x20\xe2\xe0\xf8"
        "\xe5\xec\xf3\x20\xe2\xee\xe9\xf1\xea\xf3\x20\xed\xe5\x20\xe2\xe5\xe7\xe5\xf2\x2c\x20\xf3\xf0\xee"
        "\xed\x2c\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\x20\xee\xf2\xe4\xe5\xeb\xfc\xed\xfb\xec\xe8"
        "\x20\xee\xf2\xf0\xff\xe4\xe0\xec\xe8\x20\xed\xe0\x20\xef\xee\xeb\xe5\x20\xe1\xee\xff\x2c\x20\xec"
        "\xee\xe6\xe5\xf2\x20\xee\xea\xe0\xe7\xe0\xf2\xfc\xf1\xff\x20\xe2\xe4\xe2\xee\xe5\x20\xec\xe5\xed"
        "\xfc\xf8\xe5\x20\xee\xe1\xfb\xf7\xed\xee\xe3\xee\x2e"),
    // %s\n\n\nМодификаторы удачи:
    DATA_COMPGEN(0x0050aa5c, cLuckInfoFormatCurrentLuckModifiers3, "\x25\x73\x0a\x0a\x0a\xcc\xee\xe4\xe8\xf4\xe8\xea\xe0\xf2\xee\xf0\xfb\x20\xf3\xe4\xe0\xf7\xe8\x3a"),
    // \nЛапка кролика +1
    DATA_COMPGEN(0x0050aa78, cLuckInfoLuckyRabbitSFoot14, "\x0a\xcb\xe0\xef\xea\xe0\x20\xea\xf0\xee\xeb\xe8\xea\xe0\x20\x2b\x31"),
    // \nЗолотая подкова +1
    DATA_COMPGEN(0x0050aa8c, cLuckInfoGoldenHorseshoe15, "\x0a\xc7\xee\xeb\xee\xf2\xe0\xff\x20\xef\xee\xe4\xea\xee\xe2\xe0\x20\x2b\x31"),
    // \nМонета +1
    DATA_COMPGEN(0x0050aaa0, cLuckInfoGamblerSLuckyCoin16, "\x0a\xcc\xee\xed\xe5\xf2\xe0\x20\x2b\x31"),
    // \nКлевер +1
    DATA_COMPGEN(0x0050aaac, cLuckInfoFourLeafClover17, "\x0a\xca\xeb\xe5\xe2\xe5\xf0\x20\x2b\x31"),
    // \nПосещен Круг фейри +1
    DATA_COMPGEN(0x0050aab8, cLuckInfoFaerieRingVisited18, "\x0a\xcf\xee\xf1\xe5\xf9\xe5\xed\x20\xca\xf0\xf3\xe3\x20\xf4\xe5\xe9\xf0\xe8\x20\x2b\x31"),
    // \nПосещен фонтан +1
    DATA_COMPGEN(0x0050aad0, cLuckInfoFountainVisited19, "\x0a\xcf\xee\xf1\xe5\xf9\xe5\xed\x20\xf4\xee\xed\xf2\xe0\xed\x20\x2b\x31"),
    // \nНет
    DATA_COMPGEN(0x0050aae4, cLuckInfoNone10, "\x0a\xcd\xe5\xf2"),
    // \nГрабитель могил -1
    DATA_COMPGEN(0x0050aaec, cLuckInfoTombRobber111, "\x0a\xc3\xf0\xe0\xe1\xe8\xf2\xe5\xeb\xfc\x20\xec\xee\xe3\xe8\xeb\x20\x2d\x31"),
    // \nРадуга магов +2
    DATA_COMPGEN(0x0050ab00, cLuckInfoSorceressRainbow212, "\x0a\xd0\xe0\xe4\xf3\xe3\xe0\x20\xec\xe0\xe3\xee\xe2\x20\x2b\x32"),
    // \nПосещен идол +1
    DATA_COMPGEN(0x0050ab14, cLuckInfoIdolVisited113, "\x0a\xcf\xee\xf1\xe5\xf9\xe5\xed\x20\xe8\xe4\xee\xeb\x20\x2b\x31"),
    // \nОграблена пирамида -2
    DATA_COMPGEN(0x0050ab28, cLuckInfoPyramidRaided214, "\x0a\xce\xe3\xf0\xe0\xe1\xeb\xe5\xed\xe0\x20\xef\xe8\xf0\xe0\xec\xe8\xe4\xe0\x20\x2d\x32"),
    // \nБазовая удача +1
    DATA_COMPGEN(0x0050ab40, cLuckInfoBasicLuck115, "\x0a\xc1\xe0\xe7\xee\xe2\xe0\xff\x20\xf3\xe4\xe0\xf7\xe0\x20\x2b\x31"),
    // \nВысокая удача +2
    DATA_COMPGEN(0x0050ab54, cLuckInfoAdvancedLuck216, "\x0a\xc2\xfb\xf1\xee\xea\xe0\xff\x20\xf3\xe4\xe0\xf7\xe0\x20\x2b\x32"),
    // \nЭксперт удачи +3
    DATA_COMPGEN(0x0050ab68, cLuckInfoExpertLuck317, "\x0a\xdd\xea\xf1\xef\xe5\xf0\xf2\x20\xf3\xe4\xe0\xf7\xe8\x20\x2b\x33"),
    // \nБонус мачты на море +1
    DATA_COMPGEN(0x0050ab7c, cLuckInfoMastheadBonusAtSea118, "\x0a\xc1\xee\xed\xf3\xf1\x20\xec\xe0\xf7\xf2\xfb\x20\xed\xe0\x20\xec\xee\xf0\xe5\x20\x2b\x31"),
    // \nПосещена русалка +1
    DATA_COMPGEN(0x0050ab94, cLuckInfoMermaidVisited119, "\x0a\xcf\xee\xf1\xe5\xf9\xe5\xed\xe0\x20\xf0\xf3\xf1\xe0\xeb\xea\xe0\x20\x2b\x31"),
    // \nБоевое одеяние Андурана дает максимальную удачу.
    DATA_COMPGEN(0x0050abac, cLuckInfoBattleGarbOfAnduranGives20,
        "\x0a\xc1\xee\xe5\xe2\xee\xe5\x20\xee\xe4\xe5\xff\xed\xe8\xe5\x20\xc0\xed\xe4\xf3\xf0\xe0\xed\xe0"
        "\x20\xe4\xe0\xe5\xf2\x20\xec\xe0\xea\xf1\xe8\xec\xe0\xeb\xfc\xed\xf3\xfe\x20\xf3\xe4\xe0\xf7\xf3"
        "\x2e")
};
DATA(0x004fe61c) char* IQnames[KB_IQ_NAME_COUNT] = {
    // Нет
    DATA_COMPGEN(0x0050abe0, IQnamesNone0, "\xcd\xe5\xf2"),
    // Глупый
    DATA_COMPGEN(0x0050abe4, IQnamesDumb1, "\xc3\xeb\xf3\xef\xfb\xe9"),
    // Средний
    DATA_COMPGEN(0x0050abec, IQnamesAverage2, "\xd1\xf0\xe5\xe4\xed\xe8\xe9"),
    // Умный
    DATA_COMPGEN(0x0050abf4, IQnamesSmart3, "\xd3\xec\xed\xfb\xe9"),
    // Гений
    DATA_COMPGEN(0x0050abfc, IQnamesGenius4, "\xc3\xe5\xed\xe8\xe9")
};
DATA(0x004fe630) char* cSpellHelp[KB_SPELL_HELP_TEXT_COUNT] = {
    // Предыдущая страница
    DATA_COMPGEN(0x0050ac04, cSpellHelpViewPreviousPage0, "\xcf\xf0\xe5\xe4\xfb\xe4\xf3\xf9\xe0\xff\x20\xf1\xf2\xf0\xe0\xed\xe8\xf6\xe0\x20"),
    // Следующая страница
    DATA_COMPGEN(0x0050ac1c, cSpellHelpViewNextPage1, "\xd1\xeb\xe5\xe4\xf3\xfe\xf9\xe0\xff\x20\xf1\xf2\xf0\xe0\xed\xe8\xf6\xe0"),
    // Небоевые заклинания
    DATA_COMPGEN(0x0050ac30, cSpellHelpViewAdventureSpells2, "\xcd\xe5\xe1\xee\xe5\xe2\xfb\xe5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff"),
    // Боевые заклинания
    DATA_COMPGEN(0x0050ac44, cSpellHelpViewCombatSpells3, "\xc1\xee\xe5\xe2\xfb\xe5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff"),
    // Закрыть волшебную книгу
    DATA_COMPGEN(0x0050ac58, cSpellHelpCloseSpellbook4, "\xc7\xe0\xea\xf0\xfb\xf2\xfc\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xf3\xfe\x20\xea\xed\xe8\xe3\xf3"),
    // Заклинания
    DATA_COMPGEN(0x0050ac70, cSpellHelpViewSpells5, "\xc7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff"),
    // Выбрать заклинание
    DATA_COMPGEN(0x0050ac7c, cSpellHelpSelectSpell6, "\xc2\xfb\xe1\xf0\xe0\xf2\xfc\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5"),
    // Боевые заклинания
    DATA_COMPGEN(0x0050ac90, cSpellHelpViewCombatSpells7, "\xc1\xee\xe5\xe2\xfb\xe5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff"),
    // У вашего героя осталось %d оч. магии
    DATA_COMPGEN(0x0050aca4, cSpellHelpYourHeroHasDSpell8,
        "\xd3\x20\xe2\xe0\xf8\xe5\xe3\xee\x20\xe3\xe5\xf0\xee\xff\x20\xee\xf1\xf2\xe0\xeb\xee\xf1\xfc\x20"
        "\x25\x64\x20\xee\xf7\x2e\x20\xec\xe0\xe3\xe8\xe8")
};
DATA(0x004fe654) char* speedText[KB_SPEED_TEXT_COUNT] = {
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
DATA(0x004fe67c) char* cArmyDetail[KB_ARMY_DETAIL_TEXT_COUNT] = {
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
DATA(0x004fe6a0) char* cWellDetail[KB_WELL_DETAIL_TEXT_COUNT] = {
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
DATA(0x004fe6c4) char* cKingdomOverview[KB_KINGDOM_OVERVIEW_TEXT_COUNT] = {
    "Kingdom Overview     Month %d, Week %d, Day %d",
    "You own Dragon City.",
    "You own the Lighthouse."
};
DATA(0x004fe6d0) char* cNewTurn[KB_NEW_TURN_TEXT_COUNT] = {
    "%s, you only have %d days left to capture a town, or you will be banished from this land.",
    "%s, this is your last day to capture a town, or you will be banished from this land.",
    "Astrologers proclaim month of the %s.\n\nAll dwellings increase population.",
    "Astrologers proclaim month of the %s.\n\n%s population doubles!\n\nAll dwellings increase "
    "population.",
    "Astrologers proclaim month of the PLAGUE!\n\nAll populations are halved.",
    "Astrologers proclaim week of the %s.\n\nAll dwellings increase population.",
    "Astrologers proclaim week of the %s.\n\n%s growth +5.\n\nAll dwellings increase population."
};
DATA(0x004fe6ec) char* cViewGeneralLabels[KB_VIEW_GENERAL_LABEL_COUNT] = {
    "Attack: ",
    "Defense: ",
    "Spell Power: ",
    "Knowledge: ",
    "Morale: ",
    "Luck: ",
    "Spell Points: "
};
DATA(0x004fe708) char* cViewGeneralHelp[KB_VIEW_GENERAL_HELP_COUNT] = {
    "Stop Catapult",
    "Cast Spell",
    "Retreat",
    "Surrender",
    "Cancel",
    "Hero's Options",
    "Captain's Options"
};
DATA(0x004fe724) char* cViewGeneralLongHelp[KB_VIEW_GENERAL_LONG_HELP_COUNT] = {
    "{Cast Spell}\n\nCast a magical spell.  You may only cast one spell per combat round.  The "
    "round is reset when every creature has had a turn.",
    "{Retreat}\n\nRetreat your hero, abandoning your creatures.  Your hero will be available for "
    "you to recruit again, however, the hero will have only a novice hero's forces.",
    "{Surrender}\n\nSurrendering costs gold.  However, if you pay the ransom, the hero and all of "
    "his or her surviving creatures will be available to recruit again.",
    "{Cancel}\n\nReturn to the battle."
};
DATA(0x004fe734) char* cCombatMessage[KB_COMBAT_MESSAGE_COUNT] = {
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
DATA(0x004fe764) char* cHeroLevel[KB_HERO_LEVEL_TEXT_COUNT] =
    {"%s has gained", " a level.\n", " %d levels.\n"};
DATA(0x004fe770) char* cCombatHelp[KB_COMBAT_HELP_COUNT] = {
    "Wait for other units to go.",
    "Skip this unit",
    "Auto combat",
    "Customize system options.",
    ""
};
DATA(0x004fe784) char* cLongCombatHelp[KB_LONG_COMBAT_HELP_COUNT] = {
    "{Wait}\n\nHave the current creature wait to take its turn until all the other creatures have "
    "gone.  The creature does not lose its turn.",
    "{Skip}\n\nSkips the current creature.  The current creature loses its turn and does not get "
    "to go again until the next round.",
    "{Auto Combat}\n\nAllows the computer to fight out the battle for you.",
    "{System Options}\n\nAllows you to customize the combat screen.",
    "{Message Bar}\n\nShows the results of individual monster's actions."
};
DATA(0x004fe798) char* cTownCommand[KB_TOWN_COMMAND_COUNT] = {
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
DATA(0x004fe808) char* gHeroDefaultNames[KB_HERO_DEFAULT_NAME_COUNT] = {
    "Lord Kilburn", "Sir Gallant", "Ector",    "Gwenneth", "Tyro",    "Ambrose",   "Ruby",
    "Maximus",      "Dimitri",     "Thundax",  "Fineous",  "Jojosh",  "Crag Hack", "Jezebel",
    "Jaclyn",       "Ergon",       "Tsabu",    "Atlas",    "Astra",   "Natasha",   "Troyan",
    "Vatawna",      "Rebecca",     "Gem",      "Ariel",    "Carlawn", "Luna",      "Arie",
    "Alamar",       "Vesper",      "Crodo",    "Barok",    "Kastore", "Agar",      "Falagar",
    "Wrathmont",    "Myra",        "Flint",    "Dawn",     "Halon",   "Myrini",    "Wilfrey",
    "Sarakin",      "Kalindra",    "Mandigal", "Zom",      "Darlana", "Zam",       "Ranloo",
    "Charity",      "Rialdo",      "Roxana",   "Sandro",   "Celia"
};
DATA(0x004fe8e0) char* gNewGameHelp[KB_NEW_GAME_HELP_COUNT] = {
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
DATA(0x004fe900) char* gSetupBaudHelp[KB_SETUP_BAUD_HELP_COUNT] = {
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
DATA(0x004fe914) char* gSetupComPortHelp[KB_SETUP_COM_PORT_HELP_COUNT] = {
    "{COM 1}\n\nUse COM Port 1 for the modem connection.",
    "{COM 2}\n\nUse COM Port 2 for the modem connection.",
    "{COM 3}\n\nUse COM Port 3 for the modem connection.",
    "{COM 4}\n\nUse COM Port 4 for the modem connection.",
    "{Cancel}\n\nCancel back to the main menu."
};
DATA(0x004fe928) char* gSetupDCBaudHelp[KB_SETUP_DC_BAUD_HELP_COUNT] = {
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
DATA(0x004fe93c) char* gSetupDCComPortHelp[KB_SETUP_DC_COM_PORT_HELP_COUNT] = {
    "{COM 1}\n\nUse COM Port 1 for the direct connection.",
    "{COM 2}\n\nUse COM Port 2 for the direct connection.",
    "{COM 3}\n\nUse COM Port 3 for the direct connection.",
    "{Com 4}\n\nUse COM Port 4 for the direct connection.",
    "{Cancel}\n\nCancel back to the main menu."
};
DATA(0x004fe950) char* gSetupHotSeatGameHelp[KB_SETUP_HOT_SEAT_HELP_COUNT] = {
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
DATA(0x004fe968) char* gSetupModemGameHelp[KB_SETUP_MODEM_HELP_COUNT] = {
    "{Host}\n\nThe host sets up the game options, chooses the number to dial, and places the call.",
    "{Guest}\n\nThe guest waits for the host to call and set up the game.",
    "{Config}\n\nChange your modem configuration.",
    "{Cancel}\n\nCancel back to the main menu."
};
DATA(0x004fe978) char* gSetupDCGameHelp[KB_SETUP_DIRECT_CONNECT_HELP_COUNT] = {
    "{Host}\n\nThe host sets up the game options.",
    "{Guest}\n\nThe guest waits for the host to set up the game.",
    "{Config}\n\nChange your direct connect port configuration.",
    "{Cancel}\n\nCancel back to the main menu."
};
DATA(0x004fe988) char* gSetupMultiPlayerGameHelp[KB_SETUP_MULTIPLAYER_HELP_COUNT] = {
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
DATA(0x004fe99c) char* gSetupNetworkGameHelp[KB_SETUP_NETWORK_HELP_COUNT] = {
    "{Host}\n\nThe host sets up the game options.  There can only be one host per network game.",
    "{Guest}\n\nThe guest waits for the host to set up the game, then is automatically added in.  "
    "There can be multiple guests for TCP/IP and IPX games, but only 1 guest for NetBIOS games.",
    "{Cancel}\n\nCancel back to the main menu."
};
DATA(0x004fe9a8) char* gSetupNetworkGame2Help[KB_SETUP_NETWORK_SECOND_HELP_COUNT] = {
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
DATA(0x004fe9b8) char* gSetupGameHelp[KB_SETUP_GAME_HELP_COUNT] = {
    "{Standard Game}\n\nA single player game playing out a single map.",
    "{Campaign Game}\n\nA single player game playing through a series of maps.",
    "{Multi-Player Game}\n\nA multi-player game, with several human players competing against each "
    "other on a single map.",
    "{Cancel}\n\nCancel back to the main menu."
};
DATA(0x004fe9c8) char* cBattleResults[KB_BATTLE_RESULT_TEXT_COUNT] = {
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
DATA(0x004fe9f4) char* cMoraleInfo[KB_MORALE_INFO_TEXT_COUNT] = {
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
DATA(0x004fea74) char* cMapSize[KB_MAP_SIZE_TEXT_COUNT] = {"Small", "Medium", "Large", "Huge"};
DATA(0x004fea84) char* cDifficulty[KB_DIFFICULTY_TEXT_COUNT] =
    {"Easy", "Normal", "Hard", "Expert", "Impossible"};
DATA(0x004fea98) char* cStartDifficulty[KB_START_DIFFICULTY_TEXT_COUNT] = {"Easy", "Normal", "Hard", "Expert"};
DATA(0x004feaa8) char* cCampaignLeaders[KB_CAMPAIGN_LEADER_TEXT_COUNT] =
    {"Lord Ironfist", "Lord Slayer", "Queen Lamanda", "Lord Alamar"};
DATA(0x004feab8) char* cWinText[KB_WIN_TEXT_COUNT] =
    {"Days Spent:", "Base Score:", "Difficulty Rating:", "Final Score:", "Ranking:"};
DATA(0x004feacc) char* cHumanDifficulty[KB_HUMAN_DIFFICULTY_TEXT_COUNT] =
    {"Human\n", "Human\nEasy", "Human\nNormal", "Human\nHard", "Human\nExpert"};
DATA(0x004feae0) char* cHumanInfoDifficulty[KB_HUMAN_INFO_DIFFICULTY_TEXT_COUNT] =
    {"Human-", "Human-Easy", "Human-Normal", "Human-Hard", "Human-Expert"};
DATA(0x004feaf4) char* musicQualityText[KB_MUSIC_QUALITY_TEXT_COUNT] =
    {"MIDI", "CD Stereo w/o Opera", "CD Stereo with Opera"};
DATA(0x004feb00) char* gSpellDesc[KB_SPELL_TEXT_COUNT] = {
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
    "{Set Water Guardian}\n\nSets Water Elementals to guard a mine against enemy armies."
};
DATA(0x004fec04) char* gSpellNames[KB_SPELL_TEXT_COUNT] = {
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
    "Set Water Guardian"
};
DATA(0x004fed08) char* gSecondarySkillLevels[KB_SECONDARY_SKILL_LEVEL_TEXT_COUNT] =
    {"Basic", "Advanced", "Expert"};
DATA(0x004fed14) char* gSecondarySkills[KB_SECONDARY_SKILL_TEXT_COUNT] = {
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
DATA(0x004fed4c) char* gNeutralBuildingNames[KB_NEUTRAL_BUILDING_TEXT_COUNT] = {
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
DATA(0x004fed98) char* gWellExtraNames[KB_WELL_EXTRA_NAME_COUNT] = {
    "Farm",
    "Garbage Heap",
    "Crystal Garden",
    "Waterfall",
    "Orchard",
    "Skull Pile",
    "1st Lvl Growth"
};
DATA(0x004fedb4) char* gSpecialBuildingNames[KB_SPECIAL_BUILDING_NAME_COUNT] =
    {"Fortifications", "Coliseum", "Rainbow", "Dungeon", "Library", "Storm", "Special"};
DATA(0x004fedd0) char* gDwellingNames[IDX(FACTION_COUNT)][KB_DWELLING_TYPE_COUNT] = {
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
DATA(0x004feef0) char* cSecSkillDesc[IDX(HERO_SKILL_COUNT)][SECONDARY_SKILL_VALUE_LEVEL_COUNT] = {
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
DATA(0x004fef98) char* cBuildingInfoNeutral[KB_NEUTRAL_BUILDING_INFO_COUNT] = {
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
DATA(0x004fefe4) char* gBuildingInfoSpecial[KB_SPECIAL_BUILDING_INFO_COUNT] = {
    "The Fortifications increase the toughness of the walls, increasing the number of turns it "
    "takes to knock them down.",
    "The Coliseum provides inspiring spectacles to defending troops, raising their morale by two "
    "during combat.",
    "The Rainbow increases the luck of the defending units by two.",
    "The Dungeon increases the income of the town by 500 / day.",
    "The Library increases the number of spells in the Guild by one for each level of the guild.",
    "The Storm adds +2 to the power of spells of a defending spell caster."
};
DATA(0x004feffc) char* cDirections[KB_DIRECTION_TEXT_COUNT] = {
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
DATA(0x004ff020) char* cRumourTerrainDescriptions[KB_RUMOUR_TERRAIN_DESCRIPTION_COUNT] = {
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
DATA(0x004ff044) char* gInterfaceTypeText[KB_INTERFACE_TYPE_TEXT_COUNT] = {"Dynamic", "Good", "Evil"};
DATA(0x004ff050) char* cBWMouseText[KB_BW_MOUSE_TEXT_COUNT] = {"Black & White", "Color"};
DATA(0x004ff058) char* combatSpeedText[KB_COMBAT_SPEED_TEXT_COUNT] = {"Normal", "Fast", "Very Fast"};
DATA(0x004ff064) char* combatMiniInfoText[KB_COMBAT_MINI_INFO_TEXT_COUNT] = {"None", "Spells Only", "Full"};
DATA(0x004ff070) char* gcCommandLineHelp[KB_COMMAND_LINE_HELP_COUNT] = {
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
DATA(0x004ff0a8) char* cOverviewText[KB_OVERVIEW_TEXT_COUNT] =
    {"Hero/Stats", "Skills", "Artifacts", "Town/Castle", "Garrison", "Available"};
DATA(0x004ff0c0) char* cWinComError[KB_WIN_COM_ERROR_TEXT_COUNT] = {
    "Communications error on function '%s'\n\nWin95 Error Code: %d\nWin95 Error Meaning: %s\n\n",
    "Suggested solutions:",
    "\n  1) Make sure all cables are firmly connected.",
    "\n  2) Reboot computer.",
    "\n  3) Check to make sure you have the correct COM port setting in 'CONFIG'. (The 3rd button "
    "down on the screen where you choose Host or Guest.)",
    "\n  4) Consider lowering the BAUD rate in 'CONFIG' to 19200 or 9600."
};
DATA(0x004ff0d8) char* cMiniViewText[KB_MINI_VIEW_TEXT_COUNT] =
    {"%d Units", "%d Unit", "Attack", "Defense", "HP ", "Dmg", "Mrl", "Luk", "Shots"};
DATA(0x004ff0fc) char* gFileRequestHelp[KB_FILE_REQUEST_HELP_COUNT] = {
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
DATA(0x004ff138) char* cPersonality[KB_PERSONALITY_TEXT_COUNT] = {"Warrior", "Builder", "Explorer", "Human"};
DATA(0x004ff148) char* gArmySizeNames[KB_ARMY_SIZE_NAME_COUNT][KB_ARMY_SIZE_NAME_VARIANT_COUNT] = {
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
DATA(0x004ff1b4) char* cRandomTavernText[KB_RANDOM_TAVERN_TEXT_COUNT] = {
    "The truth is out there.",
    "The dark side is stronger.",
    "The end of the world is near.",
    "The bones of Lord Slayer are buried in the foundation of the arena.",
    "He's innocent.",
    "A Black Dragon will take out a Titan any day of the week.",
    "He told her \"Yada yada yada\"...  and then she said \"Blah, blah, blah...\"",
    "There once was a man from Nantucket..."
};
DATA(0x004ff1d4) char* cRandomSignText[KB_RANDOM_SIGN_TEXT_COUNT] =
    {"See Rock City", "This space for rent", "Next sign 50 miles", "Burma shave"};
DATA(0x004ff1e4) char* cCampaignAwards[KB_CAMPAIGN_AWARD_TEXT_COUNT] = {
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
DATA(0x004ff214) char* cCampaignName[IDX(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT] = {
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
DATA(0x004ff274) char* cCampaignDescription[IDX(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT] = {
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
DATA(0x004ff2d4) char* cOutOfMemory =
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n%s\nHeroes II requires a minimum of\n%dK of Extended Memory (XMS) "
    "and\n480K of Conventional Memory\n\n";
DATA(0x004ff2d8) char* cSlowVideoLevelText[KB_SLOW_VIDEO_LEVEL_TEXT_COUNT] = {"Normal", "Interlaced"};
DATA(0x004ff2e0) char* gSPanelHelp[KB_SETTINGS_PANEL_HELP_COUNT] = {
    "{OK}\n\nExit this menu.",
    "{Music}\n\nToggle ambient music level.\n\n(Note: When using CD Stereo music in DOS, the music "
    "may only be turned on or off - the level has no effect.)",
    "{Effects}\n\nToggle foreground sounds level.",
    "{Speed}\n\nChange the speed at which your heroes move on the main screen.",
    "{Music Type}\n\nChange the type of music.  MIDI music does not usually sound as good, but "
    "hurts performance less than CD Stereo.  If you use CD Stereo, you can select whether or not "
    "to have the operatic town themes play or not.\n\n(Note: Some older computers do not handle CD "
    "Stereo music well.  If you have a computer that leaves the music playing when you quit or "
    "crashes when you switch music tracks, try MIDI music.)",
    "{Show Path}\n\nToggle 'Show Path' on/off.  If 'Show Path' is on, your first click on a map "
    "location will show the path to get there, your second will start you moving. If this option "
    "is off, one click starts you moving immediately.",
    "{Enemy Speed}\n\nSets the speed that A.I. heroes move at.  You can also elect not to view "
    "A.I. movement at all.",
    "{Interface}\n\nSets what type of interface you want to use.  The default selection is a "
    "dynamic interface, which uses Evil graphics for the 3 evil heroes (Barbarian, Warlock, and "
    "Necromancer).",
    "{Video}\n\nDetermines if the video sequences play normally or in interlaced mode.  Interlaced "
    "mode runs better on slower machines, or machines with double-speed CD drives.",
    "{Mouse Cursor}\n\nToggle color cursors on/off.  Color cursors look nicer, but sometimes don't "
    "move as smoothly as black and white ones."
};
DATA(0x004ff308) char* xBarrierColor[KB_BARRIER_COLOR_NAME_COUNT] =
    {"aqua", "blue", "brown", "gold", "green", "orange", "purple", "red"};
DATA(0x004ff328) char* xGenericSiteNames[KB_GENERIC_SITE_NAME_COUNT] = {
    "Alchemist's Tower",
    "Arena",
    "Hut of the Magi",
    "Eye of the Magi",
    "Stables",
    "Mermaid",
    "Sirens"
};
DATA(0x004ff344) char* xRecruitmentSiteNames[KB_RECRUITMENT_SITE_NAME_COUNT] = {
    "Barrow Mounds",
    "Earth Summoning Altar",
    "Air Summoning Altar",
    "Fire Summoning Altar",
    "Water Summoning Altar"
};
DATA(0x004ff358) SWinSetup gWinSetup[KB_WIN_SETUP_COUNT] = {
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
b32 gbLowMemory = false;
DATA(0x004ff570) i32 giHighMemBuffer = CHECK_MEMORY_INITIAL_AVAILABLE_KB;
void* gLowPage = NULL;
b32 gbLowPageGrabbed = false;
i8 xSmackFromNetwork = 0;
DATA(0x005266cc) b32 gbInPollSound = false;
DATA(0x005266d0) H2_ENUM_STORAGE(CDRomSetupResult, i32) iCDRomErr = CD_ROM_READY;
DATA(0x005266d4) i32 bEarlySetupDone = 0;
DATA(0x005266d8) i32 bKBDone = 0;
struct _REDBOOK* hRedbookz = NULL;
DATA(0x005266e0) i32 bForceCheckTimeEvent = 0;
DATA(0x004ff574) u16 IMHotSpots[KB_INIT_MENU_HOTSPOT_COUNT][IDX(INIT_MENU_HOTSPOT_FIELD_COUNT)] = {
    {481, 185, 83, 96},
    {194, 179, 82, 79},
    {412, 105, 75, 76},
    {303, 137, 75, 44},
    {0, 389, 86, 90}
};

DATA(0x004ff59c) i32 lastIMHoverID = -1;
DATA(0x005266e4) i32 bInCheckEndGame = 0;
DATA(0x005266e8) i32 bInShutDown = 0;
DATA(0x005266ec) b32 gbInMemError = false;
DATA(0x005266f0) i32 iShingleAnimFrame = 0;
DATA(0x005261b0) b32 gbHumanPlayer[IDX(GAME_PLAYER_COUNT)];
DATA(0x00525884) b32 gbHitEvent;
DATA(0x005260e0) i32 giMaxExtentX;
DATA(0x005260e4) i32 giMaxExtentY;
i32 giRandomClouds;
char cOverrideDigitalDriver[GLOBAL_DRIVER_NAME_SIZE];
DATA(0x00526140) i32 giBottomViewOverrideEndTime;
DATA(0x005265b0) i8 gArmyEffected[COMBAT_SIDE_COUNT][KB_ARMY_EFFECT_COUNT];
DATA(0x005258bc) H2_ENUM_STORAGE(ResourceType, i32) giBottomViewResource;
DATA(0x00524e04) b32 gbInCampaign;
DATA(0x00526130) i32 giResExtra1;
DATA(0x00526138) i32 giResExtra2;
DATA(0x00524de4) i8 puzzlePiecesRemoved[PUZZLE_PIECE_STORAGE_SIZE];
DATA(0x00526164) i32 giSeedingValid;
DATA(0x0052613c) i32 giLimitPlayer;
i32 giShowClouds;
DATA(0x005263fc) i32 bDoColorCycle;
DATA(0x00526100) inputManager* gpInputManager;
DATA(0x00525a58) i32 iMaxMapExtra;
DATA(0x00525be4) palette* gPalette;
DATA(0x005258d0) resourceManager* gpResourceManager;
char gcBotViewText[GLOBAL_BOTTOM_VIEW_TEXT_SIZE];
DATA(0x005258c4) i32 bSpecialHideCursor;
DATA(0x005258a0) searchArray* gpSearchArray;
DATA(0x00525bd8) i32 giResType1;
DATA(0x005260e8) b32 gbBlackoutPlayer;
DATA(0x00525be0) i32 giResType2;
DATA(0x00525bec) char cNetBoxLine[BOX_LINE_COUNT][NET_BOX_LINE_SIZE];
i32 gIndex;
DATA(0x005258c8) i32 giWeekTypeExtra;
DATA(0x005258d8) philAI* gpPhilAI;
DATA(0x00525898) i32 giTCPType;
DATA(0x00524e00) H2_ENUM_STORAGE(CombatEffectType, i32) gCurLoadedSpellEffect;
DATA(0x00524acc) class mouseManager* gpMouseManager;
DATA(0x0052610c) i32 giCurTempMobility;
DATA(0x00525be8) OverviewReturnAction giOverviewReturnAction;
char cOverrideMIDIDriver[GLOBAL_DRIVER_NAME_SIZE];
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
u32l gTimeMark;
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
DATA(0x005261ac) i32 giFullySeeded;
DATA(0x00524de0) icon* gBuyBuildIcons;
b32 gbNoSound;
DATA(0x00524df0) i32 iCombatControlNetPos[COMBAT_SIDE_COUNT];
DATA(0x00524e14) char cExpAggPathName[GLOBAL_AGGREGATE_PATH_SIZE];
DATA(0x0052614c) b32 gbMoveShown;
DATA(0x0052612c) void** ppMapExtra;
DATA(0x0052639c) char gcBottomViewText[GLOBAL_BOTTOM_VIEW_MESSAGE_SIZE];
DATA(0x005258cc) i32 giThisNetPos;
DATA(0x005258dc) b8 gbSetupGamePosToRealGamePos[RADAR_OWNER_COLOR_COUNT];
DATA(0x00526418) char gcRegCDRomPath[GLOBAL_AGGREGATE_PATH_SIZE];
DATA(0x00525a54) class heroWindow* heroWin;
DATA(0x00526158) i32 giOverviewReturnActionExtra;
DATA(0x0052657c) H2_ENUM_STORAGE(CombatSide, i32) giCurGeneral;
DATA(0x005260dc) i32 giThisGamePos;
DATA(0x00524f90) i32 giNumHumanPlayers;
b32 gbIconClipOn;
DATA(0x00526408) b32 gbRemoteGameOpen;
DATA(0x00525bcc) combatManager* gpCombatManager;
DATA(0x00524dd8) i32 giTCPNumPlayers;
DATA(0x005263f8) executive* gpExec;
void* hMainWindow;
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
DATA(0x00524f94) i32 bFreshSave;
DATA(0x005265ac) i32 bShowIt;
i32 gLowPageScreenSelector;
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
DATA(0x00526144) b8 gbGamePosToNetPos[OLD_MAIN_MATCH_BUFFER_SIZE];
