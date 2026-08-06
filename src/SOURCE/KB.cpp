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
                DATA_COMPGEN(
                    0x005143d4,
                    setupCdRomNoCdNetworkOnly,
                    "\xcd\xe5\xf2 \xe4\xee\xf1\xf2\xf3\xef\xe0 \xea CD-ROM. \xc1\xe5\xe7 "
                "\xe4\xee\xf1\xf2\xf3\xef\xe0 \xea CD-ROM \xc3\xe5\xf0\xee\xe8 2 \xe1\xf3\xe4\xf3"
                "\xf2 \xef\xee\xe4\xe4\xe5\xf0\xe6\xe8\xe2\xe0\xf2\xfc \xf2\xee\xeb\xfc\xea\xee "
                "\xf1\xe5\xf2\xe5\xe2\xf3\xfe \xe8\xe3\xf0\xf3 \xe2 \xf0\xe5\xe6\xe8\xec\xe5 "
                    "\xe3\xee\xf1\xf2\xff."
                ),
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
                DATA_COMPGEN(
                    0x00514440,
                    setupCdRomExpansionDiscMissing,
                    " \xcd\xe5\xf2 \xe4\xee\xf1\xf2\xf3\xef\xe0 \xea CD-ROM. \xc1\xe5\xe7 "
                "\xe4\xee\xf1\xf2\xf3\xef\xe0 \xea CD-ROM \xc3\xe5\xf0\xee\xe8 2 \xe1\xf3\xe4\xf3"
                "\xf2 \xef\xee\xe4\xe4\xe5\xf0\xe6\xe8\xe2\xe0\xf2\xfc \xf2\xee\xeb\xfc\xea\xee "
                "\xf1\xe5\xf2\xe5\xe2\xf3\xfe \xe8\xe3\xf0\xf3 \xe2 \xf0\xe5\xe6\xe8\xec\xe5 "
                "\xe3\xee\xf1\xf2\xff. \xc5\xf1\xeb\xe8 \xf3 \xe2\xe0\xf1 \xe5\xf1\xf2\xfc "
                "\xe4\xe8\xf1\xea, \xe7\xe0\xea\xf0\xee\xe9\xf2\xe5 \xfd\xf2\xf3 \xef\xf0\xee"
                "\xe3\xf0\xe0\xec\xec\xf3 \xe8 \xe2\xf1\xf2\xe0\xe2\xfc\xf2\xe5 \xe4\xe8\xf1\xea "
                "\xe2 \xef\xf0\xe8\xe2\xee\xe4 \xe8 \xef\xee\xef\xf0\xee\xe1\xf3\xe9\xf2\xe5 "
                "\xe7\xe0\xef\xf3\xf1\xf2\xe8\xf2\xfc \xe8\xe3\xf0\xf3 \xe5\xf9\xe5 \xf0\xe0"
                    "\xe7."
                ),
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
            DATA_COMPGEN(
                0x00514568, setupCdRomGameDirectoryMissingCaption,
                "\xce\xf8\xe8\xe1\xea\xe0 \xe7\xe0\xe3\xf0\xf3\xe7\xea\xe8"
            ),
            DATA_COMPGEN(
                0x00514518,
                setupCdRomGameDirectoryMissingText,
                "\xcd\xe5 \xec\xee\xe3\xf3 \xef\xe5\xf0\xe5\xea\xeb\xfe\xf7\xe8\xf2\xfc\xf1\xff \xe2 "
                "\xe4\xe8\xf0\xe5\xea\xf2\xee\xf0\xe8\xfe \xc3\xe5\xf0\xee\xe5\xe2 II.  \xc7\xe0\xef"
                "\xf3\xf1\xf2\xe8\xf2\xe5 \xef\xf0\xee\xe3\xf0\xe0\xec\xec\xf3 \xf3\xf1\xf2\xe0\xed"
                "\xee\xe2\xea\xe8."
            )
        );
        exit(0);
    }
    if (iCDRomErr == CD_ROM_DATA_FILES_MISSING) {
        EarlyShutdown(
            DATA_COMPGEN(
                0x005145cc, setupCdRomDataFilesMissingCaption,
                "\xce\xf8\xe8\xe1\xea\xe0 \xe7\xe0\xe3\xf0\xf3\xe7\xea\xe8"
            ),
            DATA_COMPGEN(
                0x00514578,
                setupCdRomDataFilesMissingText,
                "\xcd\xe5 \xec\xee\xe3\xf3 \xed\xe0\xe9\xf2\xe8 \xf4\xe0\xe9\xeb\xfb \xe4\xe0\xed"
                "\xed\xfb\xf5 \xc3\xe5\xf0\xee\xe5\xe2 II.  \xcf\xee\xe6\xe0\xeb\xf3\xe9\xf1\xf2"
                "\xe0, \xe7\xe0\xef\xf3\xf1\xf2\xe8\xf2\xe5 \xef\xf0\xee\xe3\xf0\xe0\xec\xec\xf3 "
                "\xf3\xf1\xf2\xe0\xed\xee\xe2\xea\xe8."
            )
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
    LogStr(DATA_COMPGEN(0x005145dc, earlySetupES1, "ES1"));
    iCDRomErr = SetupCDDrive();
    InitVars();
    LogStr(DATA_COMPGEN(0x005145e0, earlySetupES2, "ES2"));
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
    LogStr(DATA_COMPGEN(0x005145e4, oldmainOM1, "OM1"));
    LogStr(DATA_COMPGEN(0x005145e8, oldmainOM2, "OM2"));
    command_c = -1;
    if (gpExec->InitSystem())
        ShutDown(DATA_COMPGEN(
            0x005145ec,
            oldmainInitializationFailed,
            "\xce\xf8\xe8\xe1\xea\xe0 \xe8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe0\xf6\xe8\xe8!"
        ));
    LogStr(DATA_COMPGEN(0x00514604, oldmainOM3, "OM3"));
    KBChangeMenu(hmnuDflt);
    gPalette = gpResourceManager->GetPalette(DATA_COMPGEN(0x00514608, oldmainPalette, "kb.pal"));
    gpWindowManager->m_updateFlags = 1;
    smallFont = gpResourceManager->GetFont(DATA_COMPGEN(0x00514610, oldmainSmallFont, "smalfont.fnt"));
    bigFont = gpResourceManager->GetFont(DATA_COMPGEN(0x00514620, oldmainBigFont, "bigfont.fnt"));
    gpMouseManager->SetPointer(
        DATA_COMPGEN(0x0051462c, oldmainAdventureMouse, "advmice.mse"),
        0,
        MOUSE_AUTO_CURSOR_TYPE
    );
    gpMouseManager->SetColorMice(gConfig.gfx[IDX(giCurExe)].colorMouseCursor);
    LogStr(DATA_COMPGEN(0x00514638, oldmainOM4, "OM4"));
    SetupCDRom();
    LogStr(DATA_COMPGEN(0x0051463c, oldmainOM5, "OM5"));
    if (gpSoundManager->Open(-1))
        ShutDown(DATA_COMPGEN(
            0x00514640,
            oldmainUnableToInitializeSound,
            "\xcd\xe5 \xec\xee\xe3\xf3 \xe8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe8\xf0\xee\xe2\xe0"
            "\xf2\xfc \xe7\xe2\xf3\xea."
        ));
    if (giDebugLevel < OLD_MAIN_DEBUG_MEMORY_CHECK_LEVEL)
        CheckMem();
    LogStr(DATA_COMPGEN(0x00514660, oldmainOM6, "OM6"));

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
                DATA_COMPGEN(0x00514664, oldmainShingleAnimation, "shnganim.icn")
            );
        if (gGameCommand != OLD_MAIN_EXIT)
            gpSoundManager->SwitchAmbientMusic(OLD_MAIN_MAIN_MUSIC);

        if (!mainScreenLoaded_h) {
            if (gGameCommand != OLD_MAIN_EXIT) {
                gpResourceManager->GetBackdrop(
                    DATA_COMPGEN(0x00514674, oldmainHeroesBackdrop, "heroes.icn"),
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
                DATA_COMPGEN(0x00514680, oldmainMainMenuAdventureMouse, "advmice.mse"),
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
                    DATA_COMPGEN(0x0051468c, oldmainSetupMainDialog, "stpmain.bin")
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
                        DATA_COMPGEN(0x00526858, oldmainEmptyPlayerName0, "")
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
                                LogStr(DATA_COMPGEN(0x00514698, oldmainNewGame1, "New Game 1"));
                                if (!gpGame->NewGame())
                                    goto main_menu;
                                LogStr(DATA_COMPGEN(0x005146a4, oldmainNewGame2, "New Game 2"));
                            }
                        }
                        break;
                    case OLD_MAIN_SETUP_LOAD:
                        LogStr(DATA_COMPGEN(0x005146b0, oldmainLoadGame1, "Load Game 1"));
                        if (!gpGame->PickLoadGame())
                            goto main_menu;
                        LogStr(DATA_COMPGEN(0x005146bc, oldmainLoadGame2, "Load Game 2"));
                        break;
                }
                goto game_setup_complete;
            case OLD_MAIN_HIGH_SCORES:
                if (gpExec->AddManager(gpHighScoreManager, -1))
                    ShutDown(DATA_COMPGEN(
                        0x005146c8,
                        oldmainUnableToAddHighScoreManager,
                        "\xcd\xe5 \xec\xee\xe3\xf3 \xe4\xee\xe1\xe0\xe2\xe8\xf2\xfc \xec\xe5\xed"
                        "\xe5\xe4\xe6\xe5\xf0\xe0!"
                    ));
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
        LogStr(DATA_COMPGEN(0x005146e4, oldmainDWM1, "DWM 1"));
        if (!quit) {
            LogStr(DATA_COMPGEN(0x005146ec, oldmainDWM2, "DWM 2"));
            if (gbRemoteOn && giThisNetPos == 0) {
                LogStr(DATA_COMPGEN(0x005146f4, oldmainDWM3, "DWM 3"));
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
            LogStr(DATA_COMPGEN(0x005146fc, oldmainDWM4, "DWM 4"));
            if (gbRemoteOn && gbWaitForRemoteReceive) {
                LogStr(DATA_COMPGEN(0x00514704, oldmainDWM5, "DWM 5"));
                giWaitType = DIALOG_WAIT_OTHER_PLAYER;
                NormalDialog(
                    DATA_COMPGEN(
                        0x0051470c,
                        oldmainWaitingForGameData,
                        "\xce\xe6\xe8\xe4\xe0\xfe \xef\xee\xeb\xf3\xf7\xe5\xed\xe8\xff \xe8\xe3\xf0\xee"
                        "\xe2\xfb\xf5 \xe4\xe0\xed\xed\xfb\xf5."
                    ),
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
                    DATA_COMPGEN(
                        0x00514730,
                        oldmainNewGameSaveName,
                        "\xcd\xce\xc2\xc0\xdf \xc8\xc3\xd0\xc0"
                    )
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
                        DATA_COMPGEN(0x0051473c, oldmainDefaultPlayerName, "%s \xe8\xe3\xf0\xee\xea"),
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
                    ShutDown(DATA_COMPGEN(
                        0x00514748,
                        oldmainUnableToAddAdventureManager,
                        "\xcd\xe5 \xec\xee\xe3\xf3 \xe4\xee\xe1\xe0\xe2\xe8\xf2\xfc \xec\xe5\xed"
                        "\xe5\xe4\xe6\xe5\xf0\xe0!"
                    ));
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
                DATA_COMPGEN(0x00514764, oldmainGameOverAdventureMouse, "advmice.mse"),
                0,
                MOUSE_AUTO_CURSOR_TYPE
            );
            sprintf(
                gcWinText,
                DATA_COMPGEN(
                    0x00514770,
                    oldmainVictoryText,
                    "\xcc\xee\xe8 \xe3\xe5\xf0\xee\xe8! \xcd\xe0\xf8\xe8 \xe2\xf0\xe0\xe3\xe8 "
                "\xe1\xfb\xeb\xe8 \xf0\xe0\xe7\xe1\xe8\xf2\xfb, \xe0 \xe8\xf5 \xe7\xe0"
                "\xec\xea\xe8 \xef\xf0\xe5\xe4\xe0\xed\xfb \xf0\xe0\xe7\xee\xf0\xe5\xed"
                "\xe8\xfe. \xc2\xe5\xeb\xe8\xea\xe8\xe9 \xef\xee\xf5\xee\xe4 \xee\xea"
                "\xee\xed\xf7\xe5\xed, \xe8 \xff \xef\xf0\xe5\xe4\xf1\xf2\xe0\xfe \xef"
                "\xe5\xf0\xe5\xe4 \xe2\xe0\xec\xe8 \xea\xe0\xea \xe2\xf1\xe5\xec\xe8 "
                "\xef\xf0\xe8\xe7\xed\xe0\xed\xed\xfb\xe9 \xc2\xe5\xeb\xe8\xea\xe8\xe9 "
                "\xca\xee\xf0\xee\xeb\xfc!\n\n\xcc\xfb \xe4\xee\xf1\xf2\xe8\xe3\xeb"
                    "\xe8  \xef\xee\xe1\xe5\xe4\xfb \xe7\xe0 %d \xe4\xed\xe5\xe9!"
                ),
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
                    DATA_COMPGEN(0x00514828, oldmainGameOverHeroesBackdrop, "heroes.icn"),
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
                                    ? DATA_COMPGEN(
                                          0x00514834,
                                          oldmainArchibaldCampaignName,
                                          "\xc0\xf0\xf7\xe8\xe1\xe0\xeb\xfc\xe4"
                                      )
                                    : DATA_COMPGEN(
                                          0x00514840,
                                          oldmainRolandCampaignName,
                                          "\xd0\xee\xeb\xe0\xed\xe4"
                                      )
                            )
                        );
                    }
                    if (campaignResult) {
                        for (player_h = 0; player_h < OLD_MAIN_PLAYER_COUNT; player_h++)
                            sprintf(
                                cPlayerNames[player_h],
                                DATA_COMPGEN(0x0052685c, oldmainEmptyPlayerName1, "")
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
                                DATA_COMPGEN(0x00526860, oldmainEmptyPlayerName2, "")
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
                            DATA_COMPGEN(0x00514848, oldmainStandardScoreHeroesBackdrop, "heroes.icn"),
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
                    ShutDown(DATA_COMPGEN(
                        0x00514854,
                        oldmainUnableToAddFinalHighScoreManager,
                        "\xcd\xe5 \xec\xee\xe3\xf3 \xe4\xee\xe1\xe0\xe2\xe8\xf2\xfc \xec\xe5\xed"
                        "\xe5\xe4\xe6\xe5\xf0\xe0!"
                    ));
                gpExec->MainLoop();
                gpExec->RemoveManager(gpHighScoreManager);
                giHighScoreRank = -1;
                gpSoundManager->SwitchAmbientMusic(OLD_MAIN_MAIN_MUSIC);
                gpResourceManager->GetBackdrop(
                    DATA_COMPGEN(0x00514870, oldmainFinalHeroesBackdrop, "heroes.icn"),
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
    strcpy(gcTCPAddress, DATA_COMPGEN(0x00526864, commandLineEmptyTcpAddress, ""));
    strcpy(gcTCPName, DATA_COMPGEN(0x00526868, commandLineEmptyTcpName, ""));
    gbUseWaveout = false;
    giDebugLevel = 0;
    giShowIntro = 1;
    gbCheatMenus = false;
    giScreenScroll = 1;
    giLimitPlayer = 0;
    gbBlackoutPlayer = true;
    helpRequested = 0;
    strcpy(gMapName, DATA_COMPGEN(0x0051487c, interpretCommandLineDefaultMap, "Chaos.mp2"));
    strcpy(
        gFullMapName,
        DATA_COMPGEN(0x00514888, interpretCommandLineDefaultMapName, "\xd5\xe0\xee\xf1")
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
        sprintf(gText, DATA_COMPGEN(0x0052686c, commandLineEmptyMapName, ""));
        for (i = 0; i < LINE_HELP_LINE_COUNT; i++)
            strcat(gText, gcCommandLineHelp[i]);
        ShutDown(gText);
    }

    sprintf(
        cAggPathName,
        DATA_COMPGEN(0x005148a4, interpretCommandLineAggregatePathFormat, "%s%s"),
        DATA_COMPGEN(0x0051489c, interpretCommandLineDataDirectory, ".\\DATA\\"),
        DATA_COMPGEN(0x00514890, interpretCommandLineAggregateName, "heroes2.agg")
    );
    DEFAULT_AGGREGATE_NAME = cAggPathName;
    sprintf(
        cExpAggPathName,
        DATA_COMPGEN(0x005148c4, interpretCommandLineExpansionAggregatePathFormat, "%s%s"),
        DATA_COMPGEN(0x005148bc, interpretCommandLineExpansionDataDirectory, ".\\DATA\\"),
        DATA_COMPGEN(0x005148ac, interpretCommandLineExpansionAggregateName, "heroes2x.agg")
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
            ShutDown(DATA_COMPGEN(
                0x005148cc,
                interpretCommandLineIncompleteTCPIPCommandLine,
                "\xcd\xe5\xe7\xe0\xe2\xe5\xf0\xf8\xe5\xed\xed\xe0\xff TCP/IP \xea\xee\xec"
                "\xe0\xed\xe4\xed\xe0\xff \xf1\xf2\xf0\xee\xea\xe0"
            ));
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
                        gpResourceManager->GetBackdrop(
                            DATA_COMPGEN(0x005148f4, initMenuMovieHeroesBackdrop, "heroes.icn"),
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
            DATA_COMPGEN(
                0x00514900,
                getBuildingInfoProductionBonus,
                "%s \xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2 \xef\xf0\xe8\xf0\xee\xf1\xf2 %s "
                "\xed\xe0 8 \xe2 \xed\xe5\xe4\xe5\xeb\xfe." /* "%s увеличивает прирост %s на 8 в неделю." */
            ),
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
            DATA_COMPGEN(
                0x0051492c,
                getBuildingInfoDwellingProduction,
                "\xc2 \xef\xee\xf1\xf2\xf0\xee\xe9\xea\xe5 '%s' \xec\xee\xe6\xed\xee "
                "\xea\xf3\xef\xe8\xf2\xfc %s." /* "В постройке '%s' можно купить %s." */
            ),
            GetBuildingName(race, building),
            gArmyNamesPlural
                [IDX(gDwellingType[IDX(race)][IDX(building) - IDX(BUILDING_SLOT_DWELLING_FIRST)])]
        );
        return gText;
    }
    if (mode) {
        sprintf(
            gText,
            DATA_COMPGEN(0x00514950, getBuildingInfoTitleAndBody, "{%s}\n\n%s"),
            GetBuildingName(race, building),
            buf
        );
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
                                DATA_COMPGEN(
                                    0x0051495c,
                                    eventWindowHandlerExperienceHelp,
                                    "{\xce\xef\xfb\xf2}\n\n\xce\xef\xfb\xf2 \xef\xee\xe7"
                                "\xe2\xee\xeb\xff\xe5\xf2 \xef\xee\xe2\xfb\xf8\xe0"
                                "\xf2\xfc \xe2\xe0\xf8\xe8\xec \xe3\xe5\xf0\xee\xff"
                                "\xec \xf1\xe2\xee\xe8 \xf3\xf0\xee\xe2\xed\xe8, \xf3"
                                "\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xf2\xfc \xef\xe5"
                                "\xf0\xe2\xe8\xf7\xed\xfb\xe5 \xe8 \xe2\xf2\xee\xf0"
                                    "\xe8\xf7\xed\xfb\xe5 \xed\xe0\xe2\xfb\xea\xe8."
                                ),
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
                                DATA_COMPGEN(
                                    0x005149c0,
                                    eventWindowHandlerResourcesHelp,
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
                                    "\xe2\xee\xe8\xed\xee\xe2."
                                ),
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
                    sprintf(gText, DATA_COMPGEN(0x00514b48, checkEndGameEliminated, "\xc2\xfb \xe1\xfb\xeb\xe8 \xe8\xf1\xea\xeb\xfe\xf7\xe5\xed\xfb \xe8\xe7 \xe8\xe3\xf0\xfb!!!"));
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                } else {
                    sprintf(gText, DATA_COMPGEN(0x00514b68, checkEndGamePlayerVanquished, "%s \xf1\xee\xea\xf0\xf3\xf8\xe5\xed!"), cPlayerNames[player]);
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
                            DATA_COMPGEN(0x00514b78, checkEndGameLostLastTown, "%s, \xe2\xfb \xef\xee\xf2\xe5\xf0\xff\xeb\xe8 \xe2\xe0\xf8 \xef\xee\xf1\xeb\xe5\xe4\xed\xe8\xe9 \xe3\xee\xf0\xee\xe4. \xc5\xf1\xeb\xe8 \xe2\xfb \xed\xe5 \xe7\xe0\xe2\xee\xfe\xe5\xf2\xe5 \xf1\xe5\xe1\xe5 \xe3\xee\xf0\xee\xe4 \xe2 \xf2\xe5\xf7\xe5\xed\xe8\xe5 \xf1\xeb\xe5\xe4\xf3\xfe\xf9\xe8\xf5 7 \xe4\xed\xe5\xe9, \xe2\xfb \xef\xf0\xee\xe8\xe3\xf0\xe0\xe5\xf2\xe5."),
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
                                DATA_COMPGEN(0x00514be8, checkEndGameHeroesAbandonYou, "%s, \xe2\xe0\xf8\xe8 \xe3\xe5\xf0\xee\xe8 \xef\xee\xea\xe8\xed\xf3\xeb\xe8 \xe2\xe0\xf1, \xe0 \xe2\xfb \xe1\xfb\xeb\xe8 \xe8\xe7\xe3\xed\xe0\xed\xfb \xf1 \xef\xee\xe7\xee\xf0\xee\xec \xe8\xe7 \xfd\xf2\xe8\xf5 \xe7\xe5\xec\xe5\xeb\xfc."),
                                cPlayerNames[player]
                            );
                        }
                    } else {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x00514c34, checkEndGamePlayerAbandonedByHeroes, "%s \xe1\xfb\xeb \xe1\xf0\xee\xf8\xe5\xed \xf1\xe2\xee\xe8\xec\xe8 \xe3\xe5\xf0\xee\xff\xec\xe8 \xe8 \xe8\xe7\xe3\xed\xe0\xed \xf1 \xef\xee\xe7\xee\xf0\xee\xec \xe8\xe7 \xfd\xf2\xe8\xf5 \xe7\xe5\xec\xe5\xeb\xfc."),
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
                sprintf(gText, DATA_COMPGEN(0x00514c74, checkEndGameEnemyDefeated, "\xc2\xf0\xe0\xe3 \xf0\xe0\xe7\xe1\xe8\xf2, \xe0 \xe2\xe0\xf8\xe0 \xe0\xf0\xec\xe8\xff \xef\xf0\xe0\xe7\xe4\xed\xf3\xe5\xf2 \xf2\xf0\xe8\xf3\xec\xf4!"));
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
                        DATA_COMPGEN(0x00514ca0, checkEndGameCapturedTown, "\xc2\xfb \xe7\xe0\xf5\xe2\xe0\xf2\xe8\xeb\xe8 %s! \xc2\xfb \xef\xee\xe1\xe5\xe4\xe8\xeb\xe8."),
                        victoryTownData->m_name
                    );
                } else {
                    sprintf(
                        gText,
                        DATA_COMPGEN(0x00514cc0, checkEndGameEnemyCapturedTown, "\xc2\xf0\xe0\xe3 \xe7\xe0\xf5\xe2\xe0\xf2\xe8\xeb %s! \xce\xed\xe8 \xef\xf0\xe0\xe7\xe4\xed\xf3\xfe\xf2 \xf2\xf0\xe8\xf3\xec\xf4."),
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
                sprintf(gText, DATA_COMPGEN(0x00514ce8, checkEndGameTownFallen, "%s \xef\xe0\xeb! \xc2\xf1\xe5 \xef\xee\xf2\xe5\xf0\xff\xed\xee."), lossTown->m_name);
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
                            DATA_COMPGEN(0x00514d00, checkEndGameAccumulatedGold, "\xc2\xfb \xf1\xee\xe1\xf0\xe0\xeb\xe8 \xe1\xee\xeb\xe5\xe5 %d \xe7\xee\xeb\xee\xf2\xfb\xf5 \xe2 \xf1\xe2\xee\xe5\xe9 \xea\xe0\xe7\xed\xe5. \xc2\xf1\xe5 \xe2\xf0\xe0\xe3\xe8 \xef\xf0\xe5\xea\xeb\xee\xed\xe8\xeb\xe8\xf1\xfc \xef\xe5\xf0\xe5\xe4 \xf1\xe8\xeb\xee\xe9 \xe2\xe0\xf8\xe5\xe3\xee \xf1\xee\xf1\xf2\xee\xff\xed\xe8\xff."),
                            bestGold
                        );
                    } else {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x00514d60, checkEndGameEnemyAccumulatedGold, "\xc2\xf0\xe0\xe3 \xf1\xee\xe1\xf0\xe0\xeb \xe1\xee\xeb\xe5\xe5 %d \xe7\xee\xeb\xee\xf2\xfb\xf5 \xe2 \xf1\xe2\xee\xe5\xe9 \xea\xe0\xe7\xed\xe5. \xc2\xfb \xe4\xee\xeb\xe6\xed\xfb \xef\xf0\xe5\xea\xeb\xee\xed\xe8\xf2\xfc\xf1\xff \xef\xe5\xf0\xe5\xe4 \xf1\xe8\xeb\xee\xe9 \xf2\xe0\xea\xee\xe3\xee \xf1\xee\xf1\xf2\xee\xff\xed\xe8\xff \xe8 \xef\xf0\xe8\xe7\xed\xe0\xf2\xfc \xf1\xe2\xee\xe5 \xef\xee\xf0\xe0\xe6\xe5\xed\xe8\xe5."),
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
                    DATA_COMPGEN(0x00514ddc, checkEndGameCapturedEnemyHero, "%s - \xe2\xf0\xe0\xe6\xe5\xf1\xea\xe8\xe9 \xe3\xe5\xf0\xee\xe9, \xf3 \xe2\xe0\xf1 \xe2 \xef\xeb\xe5\xed\xf3! \xc2\xe0\xf8\xe5 \xe7\xe0\xe4\xe0\xed\xe8\xe5 \xe7\xe0\xe2\xe5\xf0\xf8\xe5\xed\xee."),
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
                sprintf(gText, DATA_COMPGEN(0x00514e1c, checkEndGameLostHero, "%s - \xe2\xe0\xf8 \xe3\xe5\xf0\xee\xe9, \xe1\xfb\xeb \xef\xee\xe2\xe5\xf0\xe6\xe5\xed.  \xc2\xfb \xef\xf0\xee\xe2\xe0\xeb\xe8\xeb\xe8 \xe2\xe0\xf8\xe5 \xe7\xe0\xe4\xe0\xed\xe8\xe5."), lossHero_k->m_name);
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
                sprintf(gText, DATA_COMPGEN(0x00514e58, checkEndGameTimeExpired, "\xc2\xfb \xed\xe5 \xf3\xf1\xef\xe5\xeb\xe8 \xe7\xe0\xe2\xe5\xf0\xf8\xe8\xf2\xfc \xe2\xe0\xf8\xe5 \xe7\xe0\xe4\xe0\xed\xe8\xe5 \xe2 \xf1\xf0\xee\xea. \xc2\xf1\xe5 \xef\xee\xf2\xe5\xf0\xff\xed\xee."));
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
                        DATA_COMPGEN(
                            0x00514e94,
                            checkEndGameMightyArtifactName,
                            "\xcc\xee\xe3\xf3\xf9\xe5\xf1\xf2\xe2\xe5\xed\xed\xfb\xe9 \xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2"
                            /* "Могущественный артефакт" */
                        )
                    );
                } else {
                    sprintf(
                        artifactName,
                        gArtifactNames[gpGame->m_mapHeader.victoryConditionValue - 1]
                    );
                }
                if (winFlag) {
                    sprintf(gText, DATA_COMPGEN(0x00514eac, checkEndGameFoundArtifact, "\xc2\xfb \xed\xe0\xf8\xeb\xe8 %s. \xc2\xfb \xf1\xef\xf0\xe0\xe2\xe8\xeb\xe8\xf1\xfc \xf1 \xe7\xe0\xe4\xe0\xed\xe8\xe5\xec."), artifactName);
                } else {
                    sprintf(
                        gText,
                        DATA_COMPGEN(0x00514ed4, checkEndGameEnemyFoundArtifact, "\xc2\xf0\xe0\xe3 \xed\xe0\xf8\xe5\xeb %s. \xc2\xfb \xed\xe5 \xf1\xef\xf0\xe0\xe2\xe8\xeb\xe8\xf1\xfc \xf1\xee \xf1\xe2\xee\xe8\xec \xe7\xe0\xe4\xe0\xed\xe8\xe5\xec."),
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
                    DATA_COMPGEN(0x00514f08, checkEndGameAllDwarfTownsLost, "\xc2\xf1\xe5 \xe3\xee\xf0\xee\xe4\xe0 \xe3\xed\xee\xec\xee\xe2 \xef\xe0\xeb\xe8. \xdd\xf2\xee \xf1\xee\xea\xf0\xf3\xf8\xe8\xf2\xe5\xeb\xfc\xed\xee\xe5 \xef\xee\xf0\xe0\xe6\xe5\xed\xe8\xe5! \xc2\xfb \xef\xf0\xee\xe8\xe3\xf0\xe0\xeb\xe8.")
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
            sprintf(gText, DATA_COMPGEN(0x00514f4c, checkEndGameDragonCityCaptured, "\xc4\xf0\xe0\xea\xee\xed\xe8\xe9 \xe3\xee\xf0\xee\xe4 \xef\xe0\xeb! \xd2\xe5\xef\xe5\xf0\xfc \xe2\xfb \xcf\xee\xe2\xe5\xeb\xe8\xf2\xe5\xeb\xfc \xe4\xf0\xe0\xea\xee\xed\xee\xe2."));
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
                sprintf(gText, DATA_COMPGEN(0x00514f80, checkEndGameRolandCaptured, "\xd0\xee\xeb\xe0\xed\xe4 \xef\xeb\xe5\xed\xe5\xed! \xc2\xf1\xe5 \xef\xee\xf2\xe5\xf0\xff\xed\xee."));
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
                sprintf(gText, DATA_COMPGEN(0x00514fa0, checkEndGameRolandEnemyDefeated, "\xc2\xf0\xe0\xe3 \xf0\xe0\xe7\xe1\xe8\xf2, \xe0 \xe2\xe0\xf8\xe0 \xe0\xf0\xec\xe8\xff \xef\xf0\xe0\xe7\xe4\xed\xf3\xe5\xf2 \xf2\xf0\xe8\xf3\xec\xf4!"));
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
                DATA_COMPGEN(0x00514fd4, checkEndGameCampaignSaveFormat, "%s%c_%02d"),
                DATA_COMPGEN(0x00514fcc, checkEndGameCampaignSavePrefix, "\xcf\xce\xc1\xc5\xc4\xc0_"),
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
    strcpy(gpGame->m_mapFilename, DATA_COMPGEN(0x00514fe0, initVarsDefaultMap, "brokena.mp2"));
    gpGame->m_newGameInitialized = 0;
    gbInNewGameSetup = false;
    strcpy(cNetBoxLine[0], DATA_COMPGEN(0x00526870, initVarsEmptyNetBoxLine0, ""));
    strcpy(cNetBoxLine[1], DATA_COMPGEN(0x00526874, initVarsEmptyNetBoxLine1, ""));
    strcpy(cNetBoxLine[2], DATA_COMPGEN(0x00526878, initVarsEmptyNetBoxLine2, ""));
    strcpy(cNetBoxLine[3], DATA_COMPGEN(0x0052687c, initVarsEmptyNetBoxLine3, ""));
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
        hmnuDflt = LoadMenuA(hInstApp, DATA_COMPGEN(0x00514fec, initVarsDebugDefaultMenu, "mnuDflt"));
        hmnuCmbt = LoadMenuA(hInstApp, DATA_COMPGEN(0x00514ff4, initVarsDebugCombatMenu, "mnuCmbt"));
        hmnuAdv = LoadMenuA(hInstApp, DATA_COMPGEN(0x00514ffc, initVarsDebugAdventureMenu, "mnuAdvD"));
        hmnuTown = LoadMenuA(hInstApp, DATA_COMPGEN(0x00515004, initVarsDebugTownMenu, "mnuTownD"));
    } else {
        hmnuDflt = LoadMenuA(hInstApp, DATA_COMPGEN(0x00515010, initVarsDefaultMenu, "mnuDflt"));
        hmnuCmbt = LoadMenuA(hInstApp, DATA_COMPGEN(0x00515018, initVarsCombatMenu, "mnuCmbt"));
        hmnuAdv = LoadMenuA(hInstApp, DATA_COMPGEN(0x00515020, initVarsAdventureMenu, "mnuAdv"));
        hmnuTown = LoadMenuA(hInstApp, DATA_COMPGEN(0x00515028, initVarsTownMenu, "mnuTown"));
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
        sprintf(
            filename_h,
            DATA_COMPGEN(0x00515038, addScoreStandardPathFormat, "%sSTANDARD.HS"),
            DATA_COMPGEN(0x00515030, addScoreStandardDataDirectory, ".\\DATA\\")
        );
    else
        sprintf(
            filename_h,
            DATA_COMPGEN(0x00515050, addScoreCampaignPathFormat, "%sCAMPAIGN.HS"),
            DATA_COMPGEN(0x00515048, addScoreCampaignDataDirectory, ".\\DATA\\")
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
            DATA_COMPGEN(0x00515060, addScoreEnterHighScoreName, "\xcf\xee\xe6\xe0\xeb\xf3\xe9\xf1\xf2\xe0, \xe2\xe2\xe5\xe4\xe8\xf2\xe5 \xf1\xe2\xee\xe5 \xe8\xec\xff \xe4\xeb\xff \xf2\xe0\xe1\xeb\xe8\xf6\xfb \xf0\xe5\xea\xee\xf0\xe4\xee\xe2."),
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
            sprintf(gText, DATA_COMPGEN(0x00515094, popNetBoxInitialChatFormat, "%s:  %s"), gsNetPlayerInfo[netPlayer].name, text);
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
        DATA_COMPGEN(0x0051509c, popNetBoxWindowDefinition, "netbox.bin")
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
    strcpy(inputText_b, DATA_COMPGEN(0x00526880, popNetBoxEmptyInput0, ""));
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
                            DATA_COMPGEN(0x005150a8, popNetBoxIncomingChatFormat, "%s:  %s"),
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
                            DATA_COMPGEN(0x005150b0, popNetBoxIncomingDataNotice, "[ \xc2\xf5\xee\xe4\xff\xf9\xe0\xff \xe8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xff, \xed\xe5\xee\xe1\xf5\xee\xe4\xe8\xec\xee \xe2\xfb\xe9\xf2\xe8... ]"),
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
            strcpy(inputText_b, DATA_COMPGEN(0x00526884, popNetBoxEmptyInput1, ""));
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
    LogStr(DATA_COMPGEN(0x005150e0, shutDownLogMarker, "Shutdown"));
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
            DATA_COMPGEN(0x005150ec, shutDownUnexpectedProgramTermination, "\xcd\xe5\xef\xf0\xe5\xe4\xe2\xe8\xe4\xe5\xed\xed\xee\xe5 \xef\xf0\xe5\xf0\xfb\xe2\xe0\xed\xe8\xe5 \xef\xf0\xee\xe3\xf0\xe0\xec\xec\xfb"),
            MB_ICONHAND
        );
    } else {
        sprintf(buf, DATA_COMPGEN(0x00515110, shutDownGoodbye, "\xcf\xee\xea\xe0!"));
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
    sprintf(buf1, DATA_COMPGEN(0x00515118, fileErrorLogFormat, "File Error %s"), strerror(err));
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
        DATA_COMPGEN(0x00515128, fileErrorMessageFormat, "\xce\xf8\xe8\xe1\xea\xe0 \xee\xf2\xea\xf0\xfb\xf2\xe8\xff \xf4\xe0\xe9\xeb\xe0 %s!"),
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
        sprintf(ratingText, DATA_COMPGEN(0x00515144, showCongratsCheater, "\xd7\xe8\xf2\xe5\xf0!!!"));

    if (highScoreType == HIGH_SCORE_STANDARD) {
        sprintf(
            congratsText,
            DATA_COMPGEN(
                0x00515150,
                showCongratsStandardFormat,
                "\xcf\xee\xe7\xe4\xf0\xe0\xe2\xeb\xff\xe5\xec!\n\n\xc4\xed\xe5\xe9: %d\n"
                "\xd1\xf7\xe5\xf2: %d\n\xd1\xeb\xee\xe6\xed\xee\xf1\xf2\xfc: %d\n\n"
                "\xce\xf7\xea\xe8: %d\n\n\xd0\xe5\xe9\xf2\xe8\xed\xe3:\n%s\n"
            ),
            giCurTurn,
            baseScore,
            gpGame->m_difficultyRating,
            realScore,
            ratingText
        );
    } else if (highScoreType == HIGH_SCORE_EXPANSION_CAMPAIGN) {
        sprintf(
            congratsText,
            DATA_COMPGEN(
                0x00515198,
                showCongratsExpansionCampaignFormat,
                "\xcf\xee\xe7\xe4\xf0\xe0\xe2\xeb\xff\xe5\xec!\n\n\xc4\xed\xe5\xe9: "
                "%d\n\n\xd0\xe5\xe9\xf2\xe8\xed\xe3:\n%s\n"
            ),
            xCampaign.Days(),
            ratingText
        );
    } else {
        sprintf(
            congratsText,
            DATA_COMPGEN(
                0x005151c0,
                showCongratsCampaignFormat,
                "\xcf\xee\xe7\xe4\xf0\xe0\xe2\xeb\xff\xe5\xec!\n\n\xc4\xed\xe5\xe9: "
                "%d\n\n\xd0\xe5\xe9\xf2\xe8\xed\xe3:\n%s\n"
            ),
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
    LogStr(DATA_COMPGEN(0x005151e8, memErrorLogMarker, "Out of Memory"));
    sprintf(
        gText,
        cOutOfMemory,
        DATA_COMPGEN(
            0x005151f8,
            memErrorMessage,
            "\xcd\xe5\xe4\xee\xf1\xf2\xe0\xf2\xee\xf7\xed\xee \xef\xe0\xec\xff\xf2\xe8." /* "Недостаточно памяти." */
        ),
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
    gBuyBuildIcons = gpResourceManager->GetIcon(
        DATA_COMPGEN(0x00515210, loadSystemwideIconsBuyBuild, "buybuild.icn")
    );
    gSystemIcons = gpResourceManager->GetIcon(
        DATA_COMPGEN(0x00515220, loadSystemwideIconsSystem, "system.icn")
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
            strcpy(gText, DATA_COMPGEN(0x0051522c, handleAppSpecificMenuCommandsConfirmRestart, "\xc2\xfb \xe4\xe5\xe9\xf1\xf2\xe2\xe8\xf2\xe5\xeb\xfc\xed\xee \xf5\xee\xf2\xe8\xf2\xe5 \xed\xe0\xf7\xe0\xf2\xfc \xf1\xed\xe0\xf7\xe0\xeb\xe0?  (\xdd\xf2\xe0 \xe8\xe3\xf0\xe0 \xe1\xf3\xe4\xe5\xf2 \xef\xee\xf2\xe5\xf0\xff\xed\xe0)"));
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
                DATA_COMPGEN(0x00515270, handleAppSpecificMenuCommandsConfirmLoad, "\xc2\xfb \xe4\xe5\xe9\xf1\xf2\xe2\xe8\xf2\xe5\xeb\xfc\xed\xee \xf5\xee\xf2\xe8\xf2\xe5 \xe7\xe0\xe3\xf0\xf3\xe7\xe8\xf2\xfc \xed\xee\xe2\xf3\xfe \xe8\xe3\xf0\xf3? (\xdd\xf2\xe0 \xe8\xe3\xf0\xe0 \xe1\xf3\xe4\xe5\xf2 \xef\xee\xf2\xe5\xf0\xff\xed\xe0)")
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
    LogStr(DATA_COMPGEN(0x00515348, handleRemoteSuddenExitHRSE1, "HRSE1"));
    TransmitRemoteData(
        reinterpret_cast<char*>(&exitInfo),
        destination,
        sizeof(exitInfo),
        ADVMGR_REMOTE_COMMAND_PLAYER_EXIT,
        0,
        0,
        REMOTE_MESSAGE_RELIABLE
    );
    LogStr(DATA_COMPGEN(0x00515350, handleRemoteSuddenExitHRSE2, "HRSE2"));
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
                sprintf(gText, DATA_COMPGEN(0x00515358, receiveHostReportsPlayerExitEliminated, "\xc2\xfb \xe1\xfb\xeb\xe8 \xe8\xf1\xea\xeb\xfe\xf7\xe5\xed\xfb \xe8\xe7 \xe8\xe3\xf0\xfb!!!"));
                NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                gbGameOver = true;
                giEndSequence = 0;
                return;
            }

            sprintf(gText, DATA_COMPGEN(0x00515378, receiveHostReportsPlayerExitVanquished, "%s \xf1\xee\xea\xf0\xf3\xf8\xe5\xed!"), gsNetPlayerInfo[exitInfo.netPosition].name);
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
            gpGame->SaveGame(DATA_COMPGEN(0x00515388, receiveHostReportsPlayerExitSaveName, "\xc8\xe3\xf0\xee\xea \xc2\xfb\xf8\xe5\xeb"), 1, 0);
            sprintf(
                gText,
                DATA_COMPGEN(0x00515394, receiveHostReportsPlayerExitHostEndsGame, "%s \xef\xee\xea\xe8\xe4\xe0\xe5\xf2 \xe8\xe3\xf0\xf3, \xe0 %s \xf0\xe5\xf8\xe0\xe5\xf2 \xef\xf0\xe5\xea\xf0\xe0\xf2\xe8\xf2\xfc \xe2\xf1\xfe \xe8\xe3\xf0\xf3. \xc8\xe3\xf0\xe0 \xe1\xfb\xeb\xe0 \xf1\xee\xf5\xf0\xe0\xed\xe5\xed\xe0 \xef\xee\xe4 \xed\xe0\xe7\xe2\xe0\xed\xe8\xe5\xec '\xc8\xc3\xd0\xce\xca \xc2\xdb\xd8\xc5\xcb'. \xc6\xe5\xeb\xe0\xe5\xf2\xe5 \xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc \xe8\xe3\xf0\xf3 \xf1\xe0\xec\xee\xf1\xf2\xee\xff\xf2\xe5\xeb\xfc\xed\xee, \xe3\xe4\xe5 \xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0 \xe7\xe0\xe9\xec\xe5\xf2 \xec\xe5\xf1\xf2\xe0 \xe2\xfb\xe1\xfb\xe2\xf8\xe8\xf5 \xe8\xe3\xf0\xee\xea\xee\xe2?"),
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
                    DATA_COMPGEN(0x00515450, receiveHostReportsPlayerExitTimedOutReport, "\xc8\xe3\xf0\xee\xea-\xf1\xe5\xf0\xe2\xe5\xf0 %s \xf1\xee\xee\xe1\xf9\xe0\xe5\xf2, \xf7\xf2\xee \xe8\xe3\xf0\xee\xea %s \xef\xee\xea\xe8\xed\xf3\xeb \xe8\xe3\xf0\xf3. \xc8\xe3\xf0\xe0 \xe1\xf3\xe4\xe5\xf2 \xef\xf0\xee\xe4\xee\xeb\xe6\xe5\xed\xe0, \xe0 \xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0 \xe7\xe0\xe9\xec\xe5\xf2 \xec\xe5\xf1\xf2\xee %s."),
                    gsNetPlayerInfo[hostNetPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name
                );
            } else {
                sprintf(
                    playerExitMessage_k,
                    DATA_COMPGEN(0x005154bc, receiveHostReportsPlayerExitVoluntaryReport, "\xc8\xe3\xf0\xee\xea-\xf1\xe5\xf0\xe2\xe5\xf0 %s \xf1\xee\xee\xe1\xf9\xe0\xe5\xf2, \xf7\xf2\xee \xe8\xe3\xf0\xee\xea %s \xef\xee\xea\xe8\xed\xf3\xeb \xe8\xe3\xf0\xf3. \xc8\xe3\xf0\xe0 \xe1\xf3\xe4\xe5\xf2 \xef\xf0\xee\xe4\xee\xeb\xe6\xe5\xed\xe0, \xe0 \xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0 \xe7\xe0\xe9\xec\xe5\xf2 \xec\xe5\xf1\xf2\xee %s."),
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
    gpGame->SaveGame(DATA_COMPGEN(0x00515528, receiveRemotePlayerExitSaveName, "\xc8\xe3\xf0\xee\xea \xc2\xfb\xf8\xe5\xeb"), 1, 0);

    if (exitInfo.eliminated) {
        exitInfo.continueGame = 1;
        if (exitInfo.netPosition == giThisNetPos) {
            localPlayerLost_e = 1;
            goto exitInfoProcessed;
        }
        sprintf(gText, DATA_COMPGEN(0x00515534, receiveRemotePlayerExitVanquished, "%s \xf1\xee\xea\xf0\xf3\xf8\xe5\xed!"), gsNetPlayerInfo[exitInfo.netPosition].name);
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
                DATA_COMPGEN(0x00515544, receiveRemotePlayerExitTimedOutPrompt, "%s \xe2\xfb\xf8\xe5\xeb \xe8\xe7 \xe8\xe3\xf0\xfb. \xc4\xe0\xed\xed\xe0\xff \xe8\xe3\xf0\xe0 \xf1\xee\xf5\xf0\xe0\xed\xe5\xed\xe0 \xef\xee\xe4 \xed\xe0\xe7\xe2\xe0\xed\xe8\xe5\xec '\xc8\xc3\xd0\xce\xca \xc2\xdb\xd8\xc5\xcb'. \xc6\xe5\xeb\xe0\xe5\xf2\xe5 \xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc \xe8\xe3\xf0\xf3, \xf1 \xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xee\xec, \xe7\xe0\xed\xff\xe2\xf8\xe8\xec \xec\xe5\xf1\xf2\xee %s?"),
                gsNetPlayerInfo[exitInfo.netPosition].name,
                gsNetPlayerInfo[exitInfo.netPosition].name
            );
        } else {
            sprintf(
                gText,
                DATA_COMPGEN(0x005155c4, receiveRemotePlayerExitVoluntaryPrompt, "%s \xef\xee\xea\xe8\xe4\xe0\xe5\xf2 \xe8\xe3\xf0\xf3. \xc4\xe0\xed\xed\xe0\xff \xe8\xe3\xf0\xe0 \xf1\xee\xf5\xf0\xe0\xed\xe5\xed\xe0 \xef\xee\xe4 \xed\xe0\xe7\xe2\xe0\xed\xe8\xe5\xec '\xc8\xc3\xd0\xce\xca \xc2\xdb\xd8\xc5\xcb'. \xc6\xe5\xeb\xe0\xe5\xf2\xe5 \xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc \xe8\xe3\xf0\xf3, \xf1 \xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xee\xec, \xe7\xe0\xed\xff\xe2\xf8\xe8\xec \xec\xe5\xf1\xf2\xee %s?"),
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
        sprintf(gText, DATA_COMPGEN(0x00515644, receiveRemotePlayerExitEliminated, "\xc2\xfb \xe1\xfb\xeb\xe8 \xe8\xf1\xea\xeb\xfe\xf7\xe5\xed\xfb \xe8\xe7 \xe8\xe3\xf0\xfb!!!"));
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

    sprintf(
        iconFile_a,
        DATA_COMPGEN(0x00515664, normalDialogWindowDefinitionFormat, "evntwin%d.bin"),
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
                sprintf(resourceText_p[resourceSlot], DATA_COMPGEN(0x00515674, normalDialogPositiveResourceFormat, "%d"), resourceValue_c[resourceSlot]);
            } else if (resourceValue_c[resourceSlot] == 0) {
                strcpy(
                    resourceText_p[resourceSlot],
                    DATA_COMPGEN(0x00526888, normalDialogEmptyResource0, "")
                );
            } else if (resourceValue_c[resourceSlot] < -NORMAL_DIALOG_DAILY_RESOURCE_OFFSET) {
                sprintf(
                    resourceText_p[resourceSlot],
                    DATA_COMPGEN(0x00515678, normalDialogNegativeResourceFormat, "%d"),
                    resourceValue_c[resourceSlot] + NORMAL_DIALOG_DAILY_RESOURCE_OFFSET
                );
            } else {
                sprintf(
                    resourceText_p[resourceSlot],
                    DATA_COMPGEN(0x0051567c, normalDialogDailyResourceFormat, "%d/\xe4\xe5\xed\xfc"),
                    -resourceValue_c[resourceSlot]
                );
            }
            strcpy(iconFile_a, DATA_COMPGEN(0x00515684, normalDialogResourceIcon, "resource.icn"));
            resourceFrame_n = resourceType_a[resourceSlot];
        } else if (resourceType_a[resourceSlot] == NORMAL_DIALOG_SPELL) {
            sprintf(
                resourceText_p[resourceSlot],
                DATA_COMPGEN(0x00515694, normalDialogSpellNameFormat, "%s"),
                gSpellNames[resourceValue_c[resourceSlot]]
            );
            strcpy(iconFile_a, DATA_COMPGEN(0x00515698, normalDialogSpellIcon, "spells.icn"));
            resourceFrame_n = gsSpellInfo[resourceValue_c[resourceSlot]].iconIndex;
        } else if (resourceType_a[resourceSlot] == NORMAL_DIALOG_CREST) {
            sprintf(
                resourceText_p[resourceSlot],
                DATA_COMPGEN(0x005156a4, normalDialogCrestNameFormat, "%s"),
                DATA_COMPGEN(0x0052688c, normalDialogEmptyResource1, "")
            );
            strcpy(iconFile_a, DATA_COMPGEN(0x005156a8, normalDialogCrestIcon, "brcrest.icn"));
            resourceFrame_n = resourceValue_c[resourceSlot];
        } else if (resourceType_a[resourceSlot] == NORMAL_DIALOG_PRIMARY_SKILL) {
            sprintf(
                resourceText_p[resourceSlot],
                DATA_COMPGEN(0x005156b4, normalDialogPrimarySkillNameFormat, "%s"),
                DATA_COMPGEN(0x00526890, normalDialogEmptyResource2, "")
            );
            strcpy(iconFile_a, DATA_COMPGEN(0x005156b8, normalDialogPrimarySkillIcon, "primskil.icn"));
            resourceFrame_n = NORMAL_DIALOG_PRIMARY_BACKGROUND_FRAME;
        } else if (resourceType_a[resourceSlot] == NORMAL_DIALOG_MONSTER) {
            sprintf(
                resourceText_p[resourceSlot],
                DATA_COMPGEN(0x005156c8, normalDialogMonsterNameFormat, "%s"),
                DATA_COMPGEN(0x00526894, normalDialogEmptyResource3, "")
            );
            strcpy(iconFile_a, DATA_COMPGEN(0x005156cc, normalDialogMonsterStripIcon, "strip.icn"));
            resourceFrame_n = NORMAL_DIALOG_MONSTER_BACKGROUND_FRAME;
        } else if (resourceType_a[resourceSlot] == NORMAL_DIALOG_SECONDARY_SKILL) {
            sprintf(
                resourceText_p[resourceSlot],
                DATA_COMPGEN(0x005156d8, normalDialogSecondarySkillNameFormat, "%s"),
                gSecondarySkills
                    [resourceValue_c[resourceSlot] / SECONDARY_SKILL_VALUE_LEVEL_COUNT]
            );
            strcpy(iconFile_a, DATA_COMPGEN(0x005156dc, normalDialogSecondarySkillIcon, "secskill.icn"));
            resourceFrame_n = resourceValue_c[resourceSlot] / SECONDARY_SKILL_VALUE_LEVEL_COUNT
                              + NORMAL_DIALOG_SECONDARY_BACKGROUND_FRAME_BASE;
        } else if (resourceType_a[resourceSlot] == NORMAL_DIALOG_HERO) {
            sprintf(
                resourceText_p[resourceSlot],
                DATA_COMPGEN(0x005156ec, normalDialogHeroNameFormat, "%s"),
                DATA_COMPGEN(0x00526898, normalDialogEmptyResource4, "")
            );
            sprintf(iconFile_a, DATA_COMPGEN(0x005156f0, normalDialogHeroBackgroundIcon, "surrendr.icn"));
            resourceFrame_n = NORMAL_DIALOG_HERO_BACKGROUND_FRAME;
        } else if (resourceType_a[resourceSlot] == NORMAL_DIALOG_EXPERIENCE
                   || resourceType_a[resourceSlot] == NORMAL_DIALOG_MORALE_BONUS
                   || resourceType_a[resourceSlot] == NORMAL_DIALOG_MORALE_PENALTY
                   || resourceType_a[resourceSlot] == NORMAL_DIALOG_LUCK_BONUS
                   || resourceType_a[resourceSlot] == NORMAL_DIALOG_LUCK_PENALTY) {
            strcpy(
                resourceText_p[resourceSlot],
                DATA_COMPGEN(0x0052689c, normalDialogEmptyResource5, "")
            );
            strcpy(iconFile_a, DATA_COMPGEN(0x00515700, normalDialogExperienceMoraleLuckIcon, "expmrl.icn"));
            resourceFrame_n = resourceType_a[resourceSlot] - NORMAL_DIALOG_EXPMRL_FIRST;
            if (resourceType_a[resourceSlot] == NORMAL_DIALOG_EXPMRL_LAST
                && resourceValue_c[resourceSlot] != NORMAL_DIALOG_NO_VALUE) {
                sprintf(resourceText_p[resourceSlot], DATA_COMPGEN(0x0051570c, normalDialogExperienceValueFormat, "%d"), resourceValue_c[resourceSlot]);
            }
        } else {
            strcpy(
                resourceText_p[resourceSlot],
                DATA_COMPGEN(0x005268a0, normalDialogEmptyResource6, "")
            );
            strcpy(iconFile_a, DATA_COMPGEN(0x00515710, normalDialogFallbackResourceIcon, "resource.icn"));
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
                DATA_COMPGEN(0x00515720, normalDialogArtifactIcon, "artifact.icn"),
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
                DATA_COMPGEN(0x00515730, normalDialogPrimarySkillOverlayIcon, "primskil.icn"),
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
                DATA_COMPGEN(0x00515740, normalDialogMonsterOverlayStripIcon, "strip.icn"),
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
                DATA_COMPGEN(0x0051574c, normalDialogMonsterIconFormat, "monh%04d.icn"),
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
                DATA_COMPGEN(0x0051575c, normalDialogCrestOverlayIcon, "brcrest.icn"),
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
                DATA_COMPGEN(0x00515768, normalDialogSecondarySkillOverlayIcon, "secskill.icn"),
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
                DATA_COMPGEN(0x00515778, normalDialogHeroPortraitFormat, "port%04d.icn"),
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
                DATA_COMPGEN(0x00515788, normalDialogSecondarySkillNameFont, "smalfont.fnt"),
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
                DATA_COMPGEN(0x00515798, normalDialogSecondaryLevelFormat, "%s"),
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
            DATA_COMPGEN(0x0051579c, normalDialogResourceLabelFont, "smalfont.fnt"),
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
            strcpy(bonusText, DATA_COMPGEN(0x005157ac, normalDialogPrimaryBonusText, "+1 "));
            textPanel_j = new textWidget(
                resourceCenterX_c - NORMAL_DIALOG_RESOURCE_LABEL_HALF_WIDTH,
                resourceY_f + sizingIconHeight - NORMAL_DIALOG_PRIMARY_BONUS_LABEL_Y_OFFSET,
                NORMAL_DIALOG_RESOURCE_LABEL_WIDTH,
                NORMAL_DIALOG_PRIMARY_BONUS_TEXT_HEIGHT,
                bonusText,
                DATA_COMPGEN(0x005157b0, normalDialogPrimaryBonusFont, "bigfont.fnt"),
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
        strcpy(orText, DATA_COMPGEN(0x005157bc, normalDialogOrText, "\xe8\xeb\xe8"));
        textPanel_j = new textWidget(
            windowWidth_f / NORMAL_DIALOG_CENTER_PART_COUNT
                - NORMAL_DIALOG_OR_TEXT_CENTER_X_OFFSET,
            resourceY_f + NORMAL_DIALOG_OR_TEXT_Y_OFFSET,
            NORMAL_DIALOG_OR_TEXT_WIDTH,
            NORMAL_DIALOG_OR_TEXT_HEIGHT,
            orText,
            DATA_COMPGEN(0x005157c0, normalDialogOrTextFont, "smalfont.fnt"),
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
        DATA_COMPGEN(0x005157d0, normalDialogAdventureMouse, "advmice.mse"),
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
        DATA_COMPGEN(0x005268a4, normalDialogEmptyPointerName, ""),
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
DATA(0x004f8bd0) char* gTilesetFiles[IDX(TILESET_COUNT)] = {
    DATA_COMPGEN(0x005266f4, gTilesetFilesEmpty0, ""),
    DATA_COMPGEN(0x005266f8, gTilesetFilesEmpty1, ""),
    DATA_COMPGEN(0x005266fc, gTilesetFilesEmpty2, ""),
    DATA_COMPGEN(0x00526700, gTilesetFilesEmpty3, ""),
    DATA_COMPGEN(0x00526704, gTilesetFilesEmpty4, ""),
    DATA_COMPGEN(0x00526708, gTilesetFilesEmpty5, ""),
    DATA_COMPGEN(0x0052670c, gTilesetFilesEmpty6, ""),
    DATA_COMPGEN(0x00526710, gTilesetFilesEmpty7, ""),
    DATA_COMPGEN(0x00526714, gTilesetFilesEmpty8, ""),
    DATA_COMPGEN(0x00526718, gTilesetFilesEmpty9, ""),
    DATA_COMPGEN(0x004ff5a0, gTilesetFilesHauntedObjects10, "objnhaun.icn"),
    DATA_COMPGEN(0x004ff5b0, gTilesetFilesArtifactObjects11, "objnarti.icn"),
    DATA_COMPGEN(0x004ff5c0, gTilesetFilesMonsterIcons12, "mons32.icn"),
    DATA_COMPGEN(0x004ff5cc, gTilesetFilesArtifactIcons13, "art32.icn"),
    DATA_COMPGEN(0x004ff5d8, gTilesetFilesFlagIcons14, "flag32.icn"),
    DATA_COMPGEN(0x004ff5e4, gTilesetFilesSmallResourceIcons15, "ressmall.icn"),
    DATA_COMPGEN(0x004ff5f4, gTilesetFilesHourglassIcons16, "hourglas.icn"),
    DATA_COMPGEN(0x004ff604, gTilesetFilesRouteIcons17, "route.icn"),
    DATA_COMPGEN(0x0052671c, gTilesetFilesEmpty18, ""),
    DATA_COMPGEN(0x004ff610, gTilesetFilesStoneBackground19, "stonback.icn"),
    DATA_COMPGEN(0x004ff620, gTilesetFilesMiniMonster20, "minimon.icn"),
    DATA_COMPGEN(0x004ff62c, gTilesetFilesMiniHero21, "minihero.icn"),
    DATA_COMPGEN(0x004ff63c, gTilesetFilesSnowMountain22, "mtnsnow.icn"),
    DATA_COMPGEN(0x004ff648, gTilesetFilesSwampMountain23, "mtnswmp.icn"),
    DATA_COMPGEN(0x004ff654, gTilesetFilesLavaMountain24, "mtnlava.icn"),
    DATA_COMPGEN(0x004ff660, gTilesetFilesDesertMountain25, "mtndsrt.icn"),
    DATA_COMPGEN(0x004ff66c, gTilesetFilesDirtMountain26, "mtndirt.icn"),
    DATA_COMPGEN(0x004ff678, gTilesetFilesMultiMountain27, "mtnmult.icn"),
    DATA_COMPGEN(0x00526720, gTilesetFilesEmpty28, ""),
    DATA_COMPGEN(0x004ff684, gTilesetFilesExtraOverlay29, "extraovr.icn"),
    DATA_COMPGEN(0x004ff694, gTilesetFilesRoad30, "road.icn"),
    DATA_COMPGEN(0x004ff6a0, gTilesetFilesCrackedMountain31, "mtncrck.icn"),
    DATA_COMPGEN(0x004ff6ac, gTilesetFilesGrassMountain32, "mtngras.icn"),
    DATA_COMPGEN(0x004ff6b8, gTilesetFilesJungleTree33, "trejngl.icn"),
    DATA_COMPGEN(0x004ff6c4, gTilesetFilesEvilTree34, "treevil.icn"),
    DATA_COMPGEN(0x004ff6d0, gTilesetFilesTownObjects35, "objntown.icn"),
    DATA_COMPGEN(0x004ff6e0, gTilesetFilesTownBaseObjects36, "objntwba.icn"),
    DATA_COMPGEN(0x004ff6f0, gTilesetFilesTownShadowObjects37, "objntwsh.icn"),
    DATA_COMPGEN(0x004ff700, gTilesetFilesTownRoadObjects38, "objntwrd.icn"),
    DATA_COMPGEN(0x004ff710, gTilesetFilesExtraObjects39, "objnxtra.icn"),
    DATA_COMPGEN(0x004ff720, gTilesetFilesWaterObjects40, "objnwat2.icn"),
    DATA_COMPGEN(0x004ff730, gTilesetFilesMultiObjects41, "objnmul2.icn"),
    DATA_COMPGEN(0x004ff740, gTilesetFilesSnowTrees42, "tresnow.icn"),
    DATA_COMPGEN(0x004ff74c, gTilesetFilesFirTrees43, "trefir.icn"),
    DATA_COMPGEN(0x004ff758, gTilesetFilesFallTrees44, "trefall.icn"),
    DATA_COMPGEN(0x004ff764, gTilesetFilesStream45, "stream.icn"),
    DATA_COMPGEN(0x004ff770, gTilesetFilesResourceObjects46, "objnrsrc.icn"),
    DATA_COMPGEN(0x004ff780, gTilesetFilesDummy47, "dummy.icn"),
    DATA_COMPGEN(0x004ff78c, gTilesetFilesGrassObjects48, "objngra2.icn"),
    DATA_COMPGEN(0x004ff79c, gTilesetFilesDeciduousTrees49, "tredeci.icn"),
    DATA_COMPGEN(0x004ff7a8, gTilesetFilesWaterObjects50, "objnwatr.icn"),
    DATA_COMPGEN(0x004ff7b8, gTilesetFilesGrassObjects51, "objngras.icn"),
    DATA_COMPGEN(0x004ff7c8, gTilesetFilesSnowObjects52, "objnsnow.icn"),
    DATA_COMPGEN(0x004ff7d8, gTilesetFilesSwampObjects53, "objnswmp.icn"),
    DATA_COMPGEN(0x004ff7e8, gTilesetFilesLavaObjects54, "objnlava.icn"),
    DATA_COMPGEN(0x004ff7f8, gTilesetFilesDesertObjects55, "objndsrt.icn"),
    DATA_COMPGEN(0x004ff808, gTilesetFilesDirtObjects56, "objndirt.icn"),
    DATA_COMPGEN(0x004ff818, gTilesetFilesCrackedObjects57, "objncrck.icn"),
    DATA_COMPGEN(0x004ff828, gTilesetFilesLavaObjects58, "objnlav3.icn"),
    DATA_COMPGEN(0x004ff838, gTilesetFilesMultiObjects59, "objnmult.icn"),
    DATA_COMPGEN(0x004ff848, gTilesetFilesLavaObjects60, "objnlav2.icn"),
    DATA_COMPGEN(0x004ff858, gTilesetFilesExpansionLocation61, "x_loc1.icn"),
    DATA_COMPGEN(0x004ff864, gTilesetFilesExpansionLocation62, "x_loc2.icn"),
    DATA_COMPGEN(0x004ff870, gTilesetFilesExpansionLocation63, "x_loc3.icn")
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
    {
        DATA_COMPGEN(0x004ff87c, cEvilTranslateAdventureBorderGood, "advbord.icn"),
        DATA_COMPGEN(0x004ff888, cEvilTranslateAdventureBorderEvil, "advborde.icn")
    },
    {
        DATA_COMPGEN(0x004ff898, cEvilTranslateHeroExteriorGood, "heroextg.icn"),
        DATA_COMPGEN(0x004ff8a8, cEvilTranslateHeroExteriorEvil, "heroexte.icn")
    },
    {
        DATA_COMPGEN(0x004ff8b8, cEvilTranslateBuyBuildGood, "buybuild.icn"),
        DATA_COMPGEN(0x004ff8c8, cEvilTranslateBuyBuildEvil, "buybuile.icn")
    },
    {
        DATA_COMPGEN(0x004ff8d8, cEvilTranslateAdventureButtonsGood, "advbtns.icn"),
        DATA_COMPGEN(0x004ff8e4, cEvilTranslateAdventureButtonsEvil, "advebtns.icn")
    },
    {
        DATA_COMPGEN(0x004ff8f4, cEvilTranslateHeroLogoGood, "herologo.icn"),
        DATA_COMPGEN(0x004ff904, cEvilTranslateHeroLogoEvil, "herologe.icn")
    },
    {
        DATA_COMPGEN(0x004ff914, cEvilTranslateSunMoonGood, "sunmoon.icn"),
        DATA_COMPGEN(0x004ff920, cEvilTranslateSunMoonEvil, "sunmoone.icn")
    },
    {
        DATA_COMPGEN(0x004ff930, cEvilTranslateStoneBackgroundGood, "stonback.icn"),
        DATA_COMPGEN(0x004ff940, cEvilTranslateStoneBackgroundEvil, "stonbake.icn")
    },
    {
        DATA_COMPGEN(0x004ff950, cEvilTranslateScrollGood, "scroll.icn"),
        DATA_COMPGEN(0x004ff95c, cEvilTranslateScrollEvil, "scrolle.icn")
    },
    {
        DATA_COMPGEN(0x004ff968, cEvilTranslateLocatorsGood, "locators.icn"),
        DATA_COMPGEN(0x004ff978, cEvilTranslateLocatorsEvil, "locatore.icn")
    },
    {
        DATA_COMPGEN(0x004ff988, cEvilTranslateSystemGood, "system.icn"),
        DATA_COMPGEN(0x004ff994, cEvilTranslateSystemEvil, "systeme.icn")
    },
    {
        DATA_COMPGEN(0x004ff9a0, cEvilTranslateControlPanelBackgroundGood, "CPANBKG.ICN"),
        DATA_COMPGEN(0x004ff9ac, cEvilTranslateControlPanelBackgroundEvil, "CPANBKGE.ICN")
    },
    {
        DATA_COMPGEN(0x004ff9bc, cEvilTranslateControlPanelGood, "CPANEL.ICN"),
        DATA_COMPGEN(0x004ff9c8, cEvilTranslateControlPanelEvil, "CPANELE.ICN")
    },
    {
        DATA_COMPGEN(0x004ff9d4, cEvilTranslateAdventurePanelBackgroundGood, "APANBKG.ICN"),
        DATA_COMPGEN(0x004ff9e0, cEvilTranslateAdventurePanelBackgroundEvil, "APANBKGE.ICN")
    },
    {
        DATA_COMPGEN(0x004ff9f0, cEvilTranslateAdventurePanelGood, "APANEL.ICN"),
        DATA_COMPGEN(0x004ff9fc, cEvilTranslateAdventurePanelEvil, "APANELE.ICN")
    },
    {
        DATA_COMPGEN(0x004ffa08, cEvilTranslateViewWorldGood, "VIEWWRLD.ICN"),
        DATA_COMPGEN(0x004ffa18, cEvilTranslateViewWorldEvil, "EVIWWRLD.ICN")
    },
    {
        DATA_COMPGEN(0x004ffa28, cEvilTranslateViewResourcesGood, "VIEWRSRC.ICN"),
        DATA_COMPGEN(0x004ffa38, cEvilTranslateViewResourcesEvil, "EVIWRSRC.ICN")
    },
    {
        DATA_COMPGEN(0x004ffa48, cEvilTranslateViewArtifactsGood, "VIEWRTFX.ICN"),
        DATA_COMPGEN(0x004ffa58, cEvilTranslateViewArtifactsEvil, "EVIWRTFX.ICN")
    },
    {
        DATA_COMPGEN(0x004ffa68, cEvilTranslateViewTownsGood, "VIEWTWNS.ICN"),
        DATA_COMPGEN(0x004ffa78, cEvilTranslateViewTownsEvil, "EVIWTWNS.ICN")
    },
    {
        DATA_COMPGEN(0x004ffa88, cEvilTranslateViewHeroesGood, "VIEWHROS.ICN"),
        DATA_COMPGEN(0x004ffa98, cEvilTranslateViewHeroesEvil, "EVIWHROS.ICN")
    },
    {
        DATA_COMPGEN(0x004ffaa8, cEvilTranslateViewAllGood, "VIEW_ALL.ICN"),
        DATA_COMPGEN(0x004ffab8, cEvilTranslateViewAllEvil, "EVIW_ALL.ICN")
    },
    {
        DATA_COMPGEN(0x004ffac8, cEvilTranslateViewMinesGood, "VIEWMINE.ICN"),
        DATA_COMPGEN(0x004ffad8, cEvilTranslateViewMinesEvil, "EVIWMINE.ICN")
    },
    {
        DATA_COMPGEN(0x004ffae8, cEvilTranslateViewDwellingsGood, "VIEWDDOR.ICN"),
        DATA_COMPGEN(0x004ffaf8, cEvilTranslateViewDwellingsEvil, "EVIWDDOR.ICN")
    },
    {
        DATA_COMPGEN(0x004ffb08, cEvilTranslateViewPuzzleGood, "VIEWPUZL.ICN"),
        DATA_COMPGEN(0x004ffb18, cEvilTranslateViewPuzzleEvil, "EVIWPUZL.ICN")
    },
    {
        DATA_COMPGEN(0x004ffb28, cEvilTranslateLegendExtraGood, "LGNDXTRA.ICN"),
        DATA_COMPGEN(0x004ffb38, cEvilTranslateLegendExtraEvil, "LGNDXTRE.ICN")
    },
    {
        DATA_COMPGEN(0x004ffb48, cEvilTranslateSystemPanelBackgroundGood, "SPANBKG.ICN"),
        DATA_COMPGEN(0x004ffb54, cEvilTranslateSystemPanelBackgroundEvil, "SPANBKGE.ICN")
    },
    {
        DATA_COMPGEN(0x004ffb64, cEvilTranslateSystemPanelButtonGood, "SPANBTN.ICN"),
        DATA_COMPGEN(0x004ffb70, cEvilTranslateSystemPanelButtonEvil, "SPANBTNE.ICN")
    },
    {
        DATA_COMPGEN(0x004ffb80, cEvilTranslateCombatSystemPanelBackgroundGood, "CSPANBKG.ICN"),
        DATA_COMPGEN(0x004ffb90, cEvilTranslateCombatSystemPanelBackgroundEvil, "CSPANBKE.ICN")
    },
    {
        DATA_COMPGEN(0x004ffba0, cEvilTranslateCombatSystemPanelButtonGood, "CSPANBTN.ICN"),
        DATA_COMPGEN(0x004ffbb0, cEvilTranslateCombatSystemPanelButtonEvil, "CSPANBTE.ICN")
    },
    {
        DATA_COMPGEN(0x004ffbc0, cEvilTranslateTradingPostGood, "TRADPOST.ICN"),
        DATA_COMPGEN(0x004ffbd0, cEvilTranslateTradingPostEvil, "TRADPOSE.ICN")
    },
    {
        DATA_COMPGEN(0x004ffbe0, cEvilTranslateViewArmyGood, "VIEWARMY.ICN"),
        DATA_COMPGEN(0x004ffbf0, cEvilTranslateViewArmyEvil, "VIEWARME.ICN")
    },
    {
        DATA_COMPGEN(0x004ffc00, cEvilTranslateWinLoseGood, "WINLOSE.ICN"),
        DATA_COMPGEN(0x004ffc0c, cEvilTranslateWinLoseEvil, "WINLOSEE.ICN")
    },
    {
        DATA_COMPGEN(0x004ffc1c, cEvilTranslateCombatWinBackgroundGood, "WINCMBTB.ICN"),
        DATA_COMPGEN(0x004ffc2c, cEvilTranslateCombatWinBackgroundEvil, "WINCMBBE.ICN")
    },
    {
        DATA_COMPGEN(0x004ffc3c, cEvilTranslateSurrenderGood, "SURRENDR.ICN"),
        DATA_COMPGEN(0x004ffc4c, cEvilTranslateSurrenderEvil, "SURRENDE.ICN")
    },
    {
        DATA_COMPGEN(0x004ffc5c, cEvilTranslateSurrenderBackgroundGood, "SURDRBKG.ICN"),
        DATA_COMPGEN(0x004ffc6c, cEvilTranslateSurrenderBackgroundEvil, "SURDRBKE.ICN")
    },
    {
        DATA_COMPGEN(0x004ffc7c, cEvilTranslateViewGeneralBackgroundGood, "VGENBKG.ICN"),
        DATA_COMPGEN(0x004ffc88, cEvilTranslateViewGeneralBackgroundEvil, "VGENBKGE.ICN")
    },
    {
        DATA_COMPGEN(0x004ffc98, cEvilTranslateCampaignBackgroundGood, "campbkgg.ICN"),
        DATA_COMPGEN(0x004ffca8, cEvilTranslateCampaignBackgroundEvil, "campbkge.ICN")
    },
    {
        DATA_COMPGEN(0x004ffcb8, cEvilTranslateCampaignExtraGood, "campxtrg.ICN"),
        DATA_COMPGEN(0x004ffcc8, cEvilTranslateCampaignExtraEvil, "campxtre.ICN")
    }
};
DATA(0x004f9c64) char gcAnimPath[GLOBAL_AGGREGATE_PATH_SIZE] = "\\ANIM2\\";
DATA(0x004f9dc4) char gcGamePath[GLOBAL_GAME_PATH_SIZE] = ".\\GAMES\\";
DATA(0x004f9dd8) char gcMapPath[GLOBAL_MAP_PATH_SIZE] = ".\\MAPS\\";
DATA(0x004f9dec) char gcMusicPath[GLOBAL_AGGREGATE_PATH_SIZE] = "\\TRACKS2\\";
DATA(0x00526600) b32 gbPutzingWithMouseCtr = false;
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
DATA(0x004f9f5c) char* gCombatFxNames[KB_COMBAT_FX_COUNT] = {
    DATA_COMPGEN(0x00526724, gCombatFxNamesEmpty0, ""),
    DATA_COMPGEN(0x004ffcd8, gCombatFxNamesMagic01, "magic01.icn"),
    DATA_COMPGEN(0x004ffce4, gCombatFxNamesMagic02, "magic02.icn"),
    DATA_COMPGEN(0x004ffcf0, gCombatFxNamesMagic03, "magic03.icn"),
    DATA_COMPGEN(0x004ffcfc, gCombatFxNamesMagic04, "magic04.icn"),
    DATA_COMPGEN(0x004ffd08, gCombatFxNamesMagic05, "magic05.icn"),
    DATA_COMPGEN(0x004ffd14, gCombatFxNamesMagic06, "magic06.icn"),
    DATA_COMPGEN(0x004ffd20, gCombatFxNamesMagic07, "magic07.icn"),
    DATA_COMPGEN(0x004ffd2c, gCombatFxNamesMagic08, "magic08.icn"),
    DATA_COMPGEN(0x004ffd38, gCombatFxNamesRainBlue, "rainbluk.icn"),
    DATA_COMPGEN(0x004ffd48, gCombatFxNamesCloudLuck, "cloudluk.icn"),
    DATA_COMPGEN(0x004ffd58, gCombatFxNamesMoraleGood, "moraleg.icn"),
    DATA_COMPGEN(0x004ffd64, gCombatFxNamesMoraleBad, "moraleb.icn"),
    DATA_COMPGEN(0x004ffd70, gCombatFxNamesRedDeath, "reddeath.icn"),
    DATA_COMPGEN(0x004ffd80, gCombatFxNamesRedFire, "redfire.icn"),
    DATA_COMPGEN(0x004ffd8c, gCombatFxNamesSparks, "sparks.icn"),
    DATA_COMPGEN(0x004ffd98, gCombatFxNamesElectric, "electric.icn"),
    DATA_COMPGEN(0x004ffda8, gCombatFxNamesPhysical, "physical.icn"),
    DATA_COMPGEN(0x004ffdb8, gCombatFxNamesBlueFire, "bluefire.icn"),
    DATA_COMPGEN(0x004ffdc8, gCombatFxNamesIceCloud, "icecloud.icn"),
    DATA_COMPGEN(0x004ffdd8, gCombatFxNamesLichCloud, "lichclod.icn"),
    DATA_COMPGEN(0x004ffde8, gCombatFxNamesBless, "bless.icn"),
    DATA_COMPGEN(0x004ffdf4, gCombatFxNamesBerserk, "berzerk.icn"),
    DATA_COMPGEN(0x004ffe00, gCombatFxNamesShield, "shield.icn"),
    DATA_COMPGEN(0x004ffe0c, gCombatFxNamesHaste, "haste.icn"),
    DATA_COMPGEN(0x004ffe18, gCombatFxNamesParalyze, "paralyze.icn"),
    DATA_COMPGEN(0x004ffe28, gCombatFxNamesHypnotize, "hypnotiz.icn"),
    DATA_COMPGEN(0x004ffe38, gCombatFxNamesDragonSlayer, "dragslay.icn"),
    DATA_COMPGEN(0x004ffe48, gCombatFxNamesBlind, "blind.icn"),
    DATA_COMPGEN(0x004ffe54, gCombatFxNamesCurse, "curse.icn"),
    DATA_COMPGEN(0x004ffe60, gCombatFxNamesStoneSkin, "stonskin.icn"),
    DATA_COMPGEN(0x004ffe70, gCombatFxNamesSteelSkin, "stelskin.icn")
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
DATA(0x00526658) i32 glBottomRefresh = 0;
DATA(0x0052665c) b32 gbBothMachinesWin95 = false;
DATA(0x00526660) b32 gbGotFirstHeartbeat = false;
DATA(0x00526664) HMENU hmnuDflt = NULL;
DATA(0x00526668) HMENU hmnuCmbt = NULL;
DATA(0x0052666c) HMENU hmnuAdv = NULL;
DATA(0x00526670) HMENU hmnuTown = NULL;
DATA(0x004fb2ac) char* cMonFilename[IDX(CREATURE_COUNT)] = {
    DATA_COMPGEN(0x004ffe80, cMonFilenamePeasant, "peasant.icn"),
    DATA_COMPGEN(0x004ffe8c, cMonFilenameArcher, "archer.icn"),
    DATA_COMPGEN(0x004ffe98, cMonFilenameRanger, "archer2.icn"),
    DATA_COMPGEN(0x004ffea4, cMonFilenamePikeman, "pikeman.icn"),
    DATA_COMPGEN(0x004ffeb0, cMonFilenameVeteranPikeman, "pikeman2.icn"),
    DATA_COMPGEN(0x004ffec0, cMonFilenameSwordsman, "swordsmn.icn"),
    DATA_COMPGEN(0x004ffed0, cMonFilenameMasterSwordsman, "swordsm2.icn"),
    DATA_COMPGEN(0x004ffee0, cMonFilenameCavalry, "cavalryr.icn"),
    DATA_COMPGEN(0x004ffef0, cMonFilenameChampion, "cavalryb.icn"),
    DATA_COMPGEN(0x004fff00, cMonFilenamePaladin, "paladin.icn"),
    DATA_COMPGEN(0x004fff0c, cMonFilenameCrusader, "paladin2.icn"),
    DATA_COMPGEN(0x004fff1c, cMonFilenameGoblin, "goblin.icn"),
    DATA_COMPGEN(0x004fff28, cMonFilenameOrc, "orc.icn"),
    DATA_COMPGEN(0x004fff30, cMonFilenameOrcChief, "orc2.icn"),
    DATA_COMPGEN(0x004fff3c, cMonFilenameWolf, "wolf.icn"),
    DATA_COMPGEN(0x004fff48, cMonFilenameOgre, "ogre.icn"),
    DATA_COMPGEN(0x004fff54, cMonFilenameOgreLord, "ogre2.icn"),
    DATA_COMPGEN(0x004fff60, cMonFilenameTroll, "troll.icn"),
    DATA_COMPGEN(0x004fff6c, cMonFilenameWarTroll, "troll2.icn"),
    DATA_COMPGEN(0x004fff78, cMonFilenameCyclops, "cyclops.icn"),
    DATA_COMPGEN(0x004fff84, cMonFilenameSprite, "sprite.icn"),
    DATA_COMPGEN(0x004fff90, cMonFilenameDwarf, "dwarf.icn"),
    DATA_COMPGEN(0x004fff9c, cMonFilenameBattleDwarf, "dwarf2.icn"),
    DATA_COMPGEN(0x004fffa8, cMonFilenameElf, "elf.icn"),
    DATA_COMPGEN(0x004fffb0, cMonFilenameGrandElf, "elf2.icn"),
    DATA_COMPGEN(0x004fffbc, cMonFilenameDruid, "druid.icn"),
    DATA_COMPGEN(0x004fffc8, cMonFilenameGreaterDruid, "druid2.icn"),
    DATA_COMPGEN(0x004fffd4, cMonFilenameUnicorn, "unicorn.icn"),
    DATA_COMPGEN(0x004fffe0, cMonFilenamePhoenix, "phoenix.icn"),
    DATA_COMPGEN(0x004fffec, cMonFilenameCentaur, "centaur.icn"),
    DATA_COMPGEN(0x004ffff8, cMonFilenameGargoyle, "gargoyle.icn"),
    DATA_COMPGEN(0x00500008, cMonFilenameGriffin, "griffin.icn"),
    DATA_COMPGEN(0x00500014, cMonFilenameMinotaur, "minotaur.icn"),
    DATA_COMPGEN(0x00500024, cMonFilenameMinotaurKing, "minotau2.icn"),
    DATA_COMPGEN(0x00500034, cMonFilenameHydra, "hydra.icn"),
    DATA_COMPGEN(0x00500040, cMonFilenameGreenDragon, "draggree.icn"),
    DATA_COMPGEN(0x00500050, cMonFilenameRedDragon, "dragred.icn"),
    DATA_COMPGEN(0x0050005c, cMonFilenameBlackDragon, "dragblak.icn"),
    DATA_COMPGEN(0x0050006c, cMonFilenameHalfling, "halfling.icn"),
    DATA_COMPGEN(0x0050007c, cMonFilenameBoar, "boar.icn"),
    DATA_COMPGEN(0x00500088, cMonFilenameIronGolem, "golem.icn"),
    DATA_COMPGEN(0x00500094, cMonFilenameSteelGolem, "golem2.icn"),
    DATA_COMPGEN(0x005000a0, cMonFilenameRoc, "roc.icn"),
    DATA_COMPGEN(0x005000a8, cMonFilenameMage, "mage1.icn"),
    DATA_COMPGEN(0x005000b4, cMonFilenameArchmage, "mage2.icn"),
    DATA_COMPGEN(0x005000c0, cMonFilenameGiant, "titanblu.icn"),
    DATA_COMPGEN(0x005000d0, cMonFilenameTitan, "titanbla.icn"),
    DATA_COMPGEN(0x005000e0, cMonFilenameSkeleton, "skeleton.icn"),
    DATA_COMPGEN(0x005000f0, cMonFilenameZombie, "zombie.icn"),
    DATA_COMPGEN(0x005000fc, cMonFilenameMutantZombie, "zombie2.icn"),
    DATA_COMPGEN(0x00500108, cMonFilenameMummy, "mummyw.icn"),
    DATA_COMPGEN(0x00500114, cMonFilenameRoyalMummy, "mummy2.icn"),
    DATA_COMPGEN(0x00500120, cMonFilenameVampire, "vampire.icn"),
    DATA_COMPGEN(0x0050012c, cMonFilenameVampireLord, "vampire2.icn"),
    DATA_COMPGEN(0x0050013c, cMonFilenameLich, "lich.icn"),
    DATA_COMPGEN(0x00500148, cMonFilenamePowerLich, "lich2.icn"),
    DATA_COMPGEN(0x00500154, cMonFilenameBoneDragon, "dragbone.icn"),
    DATA_COMPGEN(0x00500164, cMonFilenameRogue, "rogue.icn"),
    DATA_COMPGEN(0x00500170, cMonFilenameNomad, "nomad.icn"),
    DATA_COMPGEN(0x0050017c, cMonFilenameGhost, "ghost.icn"),
    DATA_COMPGEN(0x00500188, cMonFilenameGenie, "genie.icn"),
    DATA_COMPGEN(0x00500194, cMonFilenameMedusa, "medusa.icn"),
    DATA_COMPGEN(0x005001a0, cMonFilenameEarthElemental, "eelem.icn"),
    DATA_COMPGEN(0x005001ac, cMonFilenameAirElemental, "aelem.icn"),
    DATA_COMPGEN(0x005001b8, cMonFilenameFireElemental, "felem.icn"),
    DATA_COMPGEN(0x005001c4, cMonFilenameWaterElemental, "welem.icn")
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
    DATA_COMPGEN(0x005001d0, cArmyFrameFileNamesPeasant, "peas_frm.bin"),
    DATA_COMPGEN(0x005001e0, cArmyFrameFileNamesArcher, "archrfrm.bin"),
    DATA_COMPGEN(0x005001f0, cArmyFrameFileNamesRanger, "archrfrm.bin"),
    DATA_COMPGEN(0x00500200, cArmyFrameFileNamesPikeman, "pikmnfrm.bin"),
    DATA_COMPGEN(0x00500210, cArmyFrameFileNamesVeteranPikeman, "pikmnfrm.bin"),
    DATA_COMPGEN(0x00500220, cArmyFrameFileNamesSwordsman, "swrdsfrm.bin"),
    DATA_COMPGEN(0x00500230, cArmyFrameFileNamesMasterSwordsman, "swrdsfrm.bin"),
    DATA_COMPGEN(0x00500240, cArmyFrameFileNamesCavalry, "cvlryfrm.bin"),
    DATA_COMPGEN(0x00500250, cArmyFrameFileNamesChampion, "cvlr2frm.bin"),
    DATA_COMPGEN(0x00500260, cArmyFrameFileNamesPaladin, "paladfrm.bin"),
    DATA_COMPGEN(0x00500270, cArmyFrameFileNamesCrusader, "paladfrm.bin"),
    DATA_COMPGEN(0x00500280, cArmyFrameFileNamesGoblin, "goblnfrm.bin"),
    DATA_COMPGEN(0x00500290, cArmyFrameFileNamesOrc, "orc__frm.bin"),
    DATA_COMPGEN(0x005002a0, cArmyFrameFileNamesOrcChief, "orc__frm.bin"),
    DATA_COMPGEN(0x005002b0, cArmyFrameFileNamesWolf, "wolf_frm.bin"),
    DATA_COMPGEN(0x005002c0, cArmyFrameFileNamesOgre, "ogre_frm.bin"),
    DATA_COMPGEN(0x005002d0, cArmyFrameFileNamesOgreLord, "ogre_frm.bin"),
    DATA_COMPGEN(0x005002e0, cArmyFrameFileNamesTroll, "trollfrm.bin"),
    DATA_COMPGEN(0x005002f0, cArmyFrameFileNamesWarTroll, "trollfrm.bin"),
    DATA_COMPGEN(0x00500300, cArmyFrameFileNamesCyclops, "cyclofrm.bin"),
    DATA_COMPGEN(0x00500310, cArmyFrameFileNamesSprite, "spritfrm.bin"),
    DATA_COMPGEN(0x00500320, cArmyFrameFileNamesDwarf, "dwarffrm.bin"),
    DATA_COMPGEN(0x00500330, cArmyFrameFileNamesBattleDwarf, "dwarffrm.bin"),
    DATA_COMPGEN(0x00500340, cArmyFrameFileNamesElf, "elf__frm.bin"),
    DATA_COMPGEN(0x00500350, cArmyFrameFileNamesGrandElf, "elf__frm.bin"),
    DATA_COMPGEN(0x00500360, cArmyFrameFileNamesDruid, "druidfrm.bin"),
    DATA_COMPGEN(0x00500370, cArmyFrameFileNamesGreaterDruid, "druidfrm.bin"),
    DATA_COMPGEN(0x00500380, cArmyFrameFileNamesUnicorn, "unicofrm.bin"),
    DATA_COMPGEN(0x00500390, cArmyFrameFileNamesPhoenix, "phoenfrm.bin"),
    DATA_COMPGEN(0x005003a0, cArmyFrameFileNamesCentaur, "centrfrm.bin"),
    DATA_COMPGEN(0x005003b0, cArmyFrameFileNamesGargoyle, "garglfrm.bin"),
    DATA_COMPGEN(0x005003c0, cArmyFrameFileNamesGriffin, "grifffrm.bin"),
    DATA_COMPGEN(0x005003d0, cArmyFrameFileNamesMinotaur, "minotfrm.bin"),
    DATA_COMPGEN(0x005003e0, cArmyFrameFileNamesMinotaurKing, "minotfrm.bin"),
    DATA_COMPGEN(0x005003f0, cArmyFrameFileNamesHydra, "hydrafrm.bin"),
    DATA_COMPGEN(0x00500400, cArmyFrameFileNamesGreenDragon, "draggfrm.bin"),
    DATA_COMPGEN(0x00500410, cArmyFrameFileNamesRedDragon, "dragrfrm.bin"),
    DATA_COMPGEN(0x00500420, cArmyFrameFileNamesBlackDragon, "dragbfrm.bin"),
    DATA_COMPGEN(0x00500430, cArmyFrameFileNamesHalfling, "halflfrm.bin"),
    DATA_COMPGEN(0x00500440, cArmyFrameFileNamesBoar, "boar_frm.bin"),
    DATA_COMPGEN(0x00500450, cArmyFrameFileNamesIronGolem, "golemfrm.bin"),
    DATA_COMPGEN(0x00500460, cArmyFrameFileNamesSteelGolem, "golemfrm.bin"),
    DATA_COMPGEN(0x00500470, cArmyFrameFileNamesRoc, "roc__frm.bin"),
    DATA_COMPGEN(0x00500480, cArmyFrameFileNamesMage, "mage1frm.bin"),
    DATA_COMPGEN(0x00500490, cArmyFrameFileNamesArchmage, "mage1frm.bin"),
    DATA_COMPGEN(0x005004a0, cArmyFrameFileNamesGiant, "titanfrm.bin"),
    DATA_COMPGEN(0x005004b0, cArmyFrameFileNamesTitan, "tita2frm.bin"),
    DATA_COMPGEN(0x005004c0, cArmyFrameFileNamesSkeleton, "skel_frm.bin"),
    DATA_COMPGEN(0x005004d0, cArmyFrameFileNamesZombie, "zomb_frm.bin"),
    DATA_COMPGEN(0x005004e0, cArmyFrameFileNamesMutantZombie, "zomb_frm.bin"),
    DATA_COMPGEN(0x005004f0, cArmyFrameFileNamesMummy, "mummyfrm.bin"),
    DATA_COMPGEN(0x00500500, cArmyFrameFileNamesRoyalMummy, "mummyfrm.bin"),
    DATA_COMPGEN(0x00500510, cArmyFrameFileNamesVampire, "vampifrm.bin"),
    DATA_COMPGEN(0x00500520, cArmyFrameFileNamesVampireLord, "vampifrm.bin"),
    DATA_COMPGEN(0x00500530, cArmyFrameFileNamesLich, "lich_frm.bin"),
    DATA_COMPGEN(0x00500540, cArmyFrameFileNamesPowerLich, "lich_frm.bin"),
    DATA_COMPGEN(0x00500550, cArmyFrameFileNamesBoneDragon, "drabnfrm.bin"),
    DATA_COMPGEN(0x00500560, cArmyFrameFileNamesRogue, "roguefrm.bin"),
    DATA_COMPGEN(0x00500570, cArmyFrameFileNamesNomad, "nomadfrm.bin"),
    DATA_COMPGEN(0x00500580, cArmyFrameFileNamesGhost, "ghostfrm.bin"),
    DATA_COMPGEN(0x00500590, cArmyFrameFileNamesGenie, "geniefrm.bin"),
    DATA_COMPGEN(0x005005a0, cArmyFrameFileNamesMedusa, "medusfrm.bin"),
    DATA_COMPGEN(0x005005b0, cArmyFrameFileNamesEarthElemental, "felemfrm.bin"),
    DATA_COMPGEN(0x005005c0, cArmyFrameFileNamesAirElemental, "felemfrm.bin"),
    DATA_COMPGEN(0x005005d0, cArmyFrameFileNamesFireElemental, "felemfrm.bin"),
    DATA_COMPGEN(0x005005e0, cArmyFrameFileNamesWaterElemental, "felemfrm.bin")
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
DATA(0x004fbaa4) char* cCombatBkgNames[KB_COMBAT_BACKGROUND_COUNT] = {
                                                     DATA_COMPGEN(0x005005f0, cCombatBkgNamesWater, "CBKGWATR.icn"),
                                                     DATA_COMPGEN(0x00526728, cCombatBkgNamesEmpty1, ""),
                                                     DATA_COMPGEN(0x00500600, cCombatBkgNamesGrassTerrain, "CBKGGRTR.icn"),
                                                     DATA_COMPGEN(0x00500610, cCombatBkgNamesGrassMountain, "CBKGGRMT.icn"),
                                                     DATA_COMPGEN(0x00500620, cCombatBkgNamesSnowTerrain, "CBKGSNTR.icn"),
                                                     DATA_COMPGEN(0x00500630, cCombatBkgNamesSnowMountain, "CBKGSNMT.icn"),
                                                     DATA_COMPGEN(0x00500640, cCombatBkgNamesSwamp, "CBKGSWMP.icn"),
                                                     DATA_COMPGEN(0x0052672c, cCombatBkgNamesEmpty7, ""),
                                                     DATA_COMPGEN(0x00500650, cCombatBkgNamesLava, "CBKGLAVA.icn"),
                                                     DATA_COMPGEN(0x00526730, cCombatBkgNamesEmpty9, ""),
                                                     DATA_COMPGEN(0x00500660, cCombatBkgNamesDesert, "CBKGDSRT.icn"),
                                                     DATA_COMPGEN(0x00526734, cCombatBkgNamesEmpty11, ""),
                                                     DATA_COMPGEN(0x00500670, cCombatBkgNamesDirtTerrain, "CBKGDITR.icn"),
                                                     DATA_COMPGEN(0x00500680, cCombatBkgNamesDirtMountain, "CBKGDIMT.icn"),
                                                     DATA_COMPGEN(0x00500690, cCombatBkgNamesCracked, "CBKGCRCK.icn"),
                                                     DATA_COMPGEN(0x00526738, cCombatBkgNamesEmpty15, ""),
                                                     DATA_COMPGEN(0x005006a0, cCombatBkgNamesBeach, "CBKGBEAC.icn"),
                                                     DATA_COMPGEN(0x0052673c, cCombatBkgNamesEmpty17, ""),
                                                     DATA_COMPGEN(0x005006b0, cCombatBkgNamesGraveyard, "CBKGGRAV.icn")};
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
DATA(0x004fbee0) char* gTownPrefixNames[IDX(FACTION_COUNT)] = {
    DATA_COMPGEN(0x005006c0, gTownPrefixNamesKnight, "twnk"),
    DATA_COMPGEN(0x005006c8, gTownPrefixNamesBarbarian, "twnb"),
    DATA_COMPGEN(0x005006d0, gTownPrefixNamesSorceress, "twns"),
    DATA_COMPGEN(0x005006d8, gTownPrefixNamesWarlock, "twnw"),
    DATA_COMPGEN(0x005006e0, gTownPrefixNamesWizard, "twnz"),
    DATA_COMPGEN(0x005006e8, gTownPrefixNamesNecromancer, "twnn")};
DATA(0x004fbef8) char* gTownObjNames[KB_TOWN_OBJECT_NAME_COUNT] = {
    DATA_COMPGEN(0x005006f0, gTownObjNamesMageGuild, "mage"),
    DATA_COMPGEN(0x005006f8, gTownObjNamesThievesGuild, "thie"),
    DATA_COMPGEN(0x00500700, gTownObjNamesTavern, "tvrn"),
    DATA_COMPGEN(0x00500708, gTownObjNamesDock, "dock"),
    DATA_COMPGEN(0x00500710, gTownObjNamesWell, "well"),
    DATA_COMPGEN(0x00500718, gTownObjNamesTent, "tent"),
    DATA_COMPGEN(0x00500720, gTownObjNamesCastle, "cstl"),
    DATA_COMPGEN(0x00500728, gTownObjNamesStatue, "stat"),
    DATA_COMPGEN(0x00500730, gTownObjNamesLeftTurret, "ltur"),
    DATA_COMPGEN(0x00500738, gTownObjNamesRightTurret, "rtur"),
    DATA_COMPGEN(0x00500740, gTownObjNamesMarketplace, "mark"),
    DATA_COMPGEN(0x00500748, gTownObjNamesUpgradedWell, "wel2"),
    DATA_COMPGEN(0x00500750, gTownObjNamesMoat, "moat"),
    DATA_COMPGEN(0x00500758, gTownObjNamesSpecial, "spec"),
    DATA_COMPGEN(0x00500760, gTownObjNamesBoat, "boat"),
    DATA_COMPGEN(0x00500768, gTownObjNamesCaptainQuarters, "capt"),
    DATA_COMPGEN(0x00500770, gTownObjNamesExtra0, "ext0"),
    DATA_COMPGEN(0x00500778, gTownObjNamesExtra1, "ext1"),
    DATA_COMPGEN(0x00500780, gTownObjNamesExtra2, "ext2"),
    DATA_COMPGEN(0x00500788, gTownObjNamesDwelling0, "dw_0"),
    DATA_COMPGEN(0x00500790, gTownObjNamesDwelling1, "dw_1"),
    DATA_COMPGEN(0x00500798, gTownObjNamesDwelling2, "dw_2"),
    DATA_COMPGEN(0x005007a0, gTownObjNamesDwelling3, "dw_3"),
    DATA_COMPGEN(0x005007a8, gTownObjNamesDwelling4, "dw_4"),
    DATA_COMPGEN(0x005007b0, gTownObjNamesDwelling5, "dw_5"),
    DATA_COMPGEN(0x005007b8, gTownObjNamesUpgrade1, "up_1"),
    DATA_COMPGEN(0x005007c0, gTownObjNamesUpgrade2, "up_2"),
    DATA_COMPGEN(0x005007c8, gTownObjNamesUpgrade3, "up_3"),
    DATA_COMPGEN(0x005007d0, gTownObjNamesUpgrade4, "up_4"),
    DATA_COMPGEN(0x005007d8, gTownObjNamesUpgrade5, "up_5"),
    DATA_COMPGEN(0x005007e0, gTownObjNamesUpgrade5b, "up5b"),
    DATA_COMPGEN(0x005007e8, gTownObjNamesExtra3, "ext3")
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
DATA(0x004fcd7c) char* cHeroTypeShortName[IDX(FACTION_COUNT)] = {
    DATA_COMPGEN(0x005007f0, cHeroTypeShortNameKnight, "kngt"),
    DATA_COMPGEN(0x005007f8, cHeroTypeShortNameBarbarian, "barb"),
    DATA_COMPGEN(0x00500800, cHeroTypeShortNameSorceress, "sorc"),
    DATA_COMPGEN(0x00500808, cHeroTypeShortNameWarlock, "wrlk"),
    DATA_COMPGEN(0x00500810, cHeroTypeShortNameWizard, "wzrd"),
    DATA_COMPGEN(0x00500818, cHeroTypeShortNameNecromancer, "necr")};
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
    DATA_COMPGEN(0x00500ef0, gArtifactDescUltimateBookOfKnowledge, "\x7b\xca\xed\xe8\xe3\xe0\x20\xe2\xf1\xe5\xe7\xed\xe0\xed\xe8\xff\x7d\x0a\x28\xc7\xed\xe0\xed\xe8\xff\x20\x2b\x31\x32\x29\x0a\x0a\xca\xed\xe8\xe3\xe0\x20\xe2\xf1\xe5\xe7\xed\xe0\xed\xe8\xff\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xc7\xed\xe0\xed\xe8\xff\x20\xed\xe0\x20\x31\x32\x20\xe5\xe4\xe8\xed\xe8\xf6\x2e" /* "{Книга всезнания}\n(Знания +12)\n\nКнига всезнания увеличивает Знания на 12 единиц." */),
    DATA_COMPGEN(0x00500f44, gArtifactDescUltimateSwordOfDominion, "\x7b\xcc\xe5\xf7\x20\xe2\xeb\xe0\xf1\xf2\xe8\x7d\x0a\x28\xc0\xf2\xe0\xea\xe0\x20\x2b\x31\x32\x29\x0a\x0a\xcc\xe5\xf7\x20\xe2\xeb\xe0\xf1\xf2\xe8\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\xe2\xfb\xea\x20\xc0\xf2\xe0\xea\xe8\x20\xed\xe0\x20\x31\x32\x20\xe5\xe4\xe8\xed\xe8\xf6\x2e" /* "{Меч власти}\n(Атака +12)\n\nМеч власти увеличивает навык Атаки на 12 единиц." */),
    DATA_COMPGEN(0x00500f90, gArtifactDescUltimateCloakOfProtection, "\x7b\xc7\xe0\xf9\xe8\xf2\xed\xe0\xff\x20\xed\xe0\xea\xe8\xe4\xea\xe0\x7d\x0a\x28\xc7\xe0\xf9\xe8\xf2\xe0\x20\x2b\x31\x32\x29\x0a\x0a\xc7\xe0\xf9\xe8\xf2\xed\xe0\xff\x20\xed\xe0\xea\xe8\xe4\xea\xe0\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xc7\xe0\xf9\xe8\xf2\xf3\x20\xed\xe0\x20\x31\x32\x20\xe5\xe4\xe8\xed\xe8\xf6\x2e" /* "{Защитная накидка}\n(Защита +12)\n\nЗащитная накидка увеличивает Защиту на 12 единиц." */),
    DATA_COMPGEN(0x00500fe4, gArtifactDescUltimateWandOfMagic, "\x7b\xc6\xe5\xe7\xeb\x20\xec\xe0\xe3\xe8\xe8\x7d\x0a\x28\xd1\xe8\xeb\xe0\x20\xec\xe0\xe3\xe8\xe8\x20\x2b\x31\x32\x29\x0a\x0a\xc6\xe5\xe7\xeb\x20\xec\xe0\xe3\xe8\xe8\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xd1\xe8\xeb\xf3\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe9\x20\xed\xe0\x20\x31\x32\x20\xe5\xe4\xe8\xed\xe8\xf6\x2e" /* "{Жезл магии}\n(Сила магии +12)\n\nЖезл магии увеличивает Силу заклинаний на 12 единиц." */),
    DATA_COMPGEN(0x00501038, gArtifactDescUltimateShield, "\x7b\xc2\xf1\xe5\xec\xee\xe3\xf3\xf9\xe8\xe9\x20\xf9\xe8\xf2\x7d\x0a\x0a\xc2\xf1\xe5\xec\xee\xe3\xf3\xf9\xe8\xe9\x20\xf9\xe8\xf2\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xc0\xf2\xe0\xea\xf3\x20\xe8\x20\xc7\xe0\xf9\xe8\xf2\xf3\x20\xed\xe0\x20\x36\x20\xe5\xe4\xe8\xed\xe8\xf6\x20\xea\xe0\xe6\xe4\xfb\xe9\x2e" /* "{Всемогущий щит}\n\nВсемогущий щит увеличивает Атаку и Защиту на 6 единиц каждый." */),
    DATA_COMPGEN(0x00501088, gArtifactDescUltimateStaff, "\x7b\xc2\xf1\xe5\xec\xee\xe3\xf3\xf9\xe8\xe9\x20\xef\xee\xf1\xee\xf5\x7d\x0a\x0a\xc2\xf1\xe5\xec\xee\xe3\xf3\xf9\xe8\xe9\x20\xef\xee\xf1\xee\xf5\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xd1\xe8\xeb\xf3\x20\xec\xe0\xe3\xe8\xe8\x20\xe8\x20\xc7\xed\xe0\xed\xe8\xff\x20\xed\xe0\x20\x36\x20\xe5\xe4\xe8\xed\xe8\xf6\x20\xea\xe0\xe6\xe4\xfb\xe9\x2e" /* "{Всемогущий посох}\n\nВсемогущий посох увеличивает Силу магии и Знания на 6 единиц каждый." */),
    DATA_COMPGEN(0x005010e4, gArtifactDescUltimateCrown, "\x7b\xca\xee\xf0\xee\xed\xe0\x20\xe2\xf1\xe5\xe2\xeb\xe0\xf1\xf2\xe8\xff\x7d\x0a\x0a\xca\xee\xf0\xee\xed\xe0\x20\xe2\xf1\xe5\xe2\xeb\xe0\xf1\xf2\xe8\xff\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xea\xe0\xe6\xe4\xfb\xe9\x20\xe8\xe7\x20\xe1\xe0\xe7\xee\xe2\xfb\xf5\x20\xed\xe0\xe2\xfb\xea\xee\xe2\x20\xed\xe0\x20\x34\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x2e" /* "{Корона всевластия}\n\nКорона всевластия увеличивает каждый из базовых навыков на 4 единицы." */),
    DATA_COMPGEN(0x00501140, gArtifactDescGoldenGoose, "\x7b\xc7\xee\xeb\xee\xf2\xee\xe9\x20\xe3\xf3\xf1\xfc\x7d\x0a\x0a\xc7\xee\xeb\xee\xf2\xee\xe9\x20\xe3\xf3\xf1\xfc\x20\xef\xf0\xe8\xed\xee\xf1\xe8\xf2\x20\xe2\x20\xe2\xe0\xf8\xf3\x20\xea\xe0\xe7\xed\xf3\x20\xef\xee\x20\x31\x30\x2e\x30\x30\x30\x20\xe7\xee\xeb\xee\xf2\xfb\xf5\x20\xea\xe0\xe6\xe4\xfb\xe9\x20\xe4\xe5\xed\xfc\x2e" /* "{Золотой гусь}\n\nЗолотой гусь приносит в вашу казну по 10.000 золотых каждый день." */),
    DATA_COMPGEN(0x00501194, gArtifactDescArcaneNecklaceOfMagic, "\x7b\xce\xe6\xe5\xf0\xe5\xeb\xfc\xe5\x20\xf2\xe0\xe9\xed\xee\xe9\x20\xec\xe0\xe3\xe8\xe8\x7d\x0a\x28\xd1\xe8\xeb\xe0\x20\xec\xe0\xe3\xe8\xe8\x20\x2b\x34\x29\x0a\x0a\xce\xe6\xe5\xf0\xe5\xeb\xfc\xe5\x20\xf2\xe0\xe9\xed\xee\xe9\x20\xec\xe0\xe3\xe8\xe8\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xd1\xe8\xeb\xf3\x20\xec\xe0\xe3\xe8\xe8\x20\xed\xe0\x20\x34\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x2e" /* "{Ожерелье тайной магии}\n(Сила магии +4)\n\nОжерелье тайной магии увеличивает Силу магии на 4 единицы." */),
    DATA_COMPGEN(0x005011f8, gArtifactDescCasterSBraceletOfMagic, "\x7b\xcc\xe0\xe3\xe8\xf7\xe5\xf1\xea\xe8\xe9\x20\xe1\xf0\xe0\xf1\xeb\xe5\xf2\x7d\x0a\x28\xd1\xe8\xeb\xe0\x20\xec\xe0\xe3\xe8\xe8\x20\x2b\x32\x29\x0a\x0a\xcc\xe0\xe3\xe8\xf7\xe5\xf1\xea\xe8\xe9\x20\xe1\xf0\xe0\xf1\xeb\xe5\xf2\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xd1\xe8\xeb\xf3\x20\xec\xe0\xe3\xe8\xe8\x20\xed\xe0\x20\x32\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x2e" /* "{Магический браслет}\n(Сила магии +2)\n\nМагический браслет увеличивает Силу магии на 2 единицы." */),
    DATA_COMPGEN(0x00501258, gArtifactDescMageSRingOfPower, "\x7b\xca\xee\xeb\xfc\xf6\xee\x20\xec\xe0\xe3\xe0\x7d\x0a\x28\xd1\xe8\xeb\xe0\x20\xec\xe0\xe3\xe8\xe8\x20\x2b\x32\x29\x0a\x0a\xca\xee\xeb\xfc\xf6\xee\x20\xec\xe0\xe3\xe0\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xd1\xe8\xeb\xf3\x20\xec\xe0\xe3\xe8\xe8\x20\xed\xe0\x20\x32\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x2e" /* "{Кольцо мага}\n(Сила магии +2)\n\nКольцо мага увеличивает Силу магии на 2 единицы." */),
    DATA_COMPGEN(0x005012a8, gArtifactDescWitchSBroachOfMagic, "\x7b\xc1\xf0\xee\xf8\xfc\x20\xe2\xe5\xe4\xfc\xec\xfb\x7d\x0a\x28\xd1\xe8\xeb\xe0\x20\xec\xe0\xe3\xe8\xe8\x20\x2b\x33\x29\x0a\x0a\xc1\xf0\xee\xf8\xfc\x20\xe2\xe5\xe4\xfc\xec\xfb\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xd1\xe8\xeb\xf3\x20\xec\xe0\xe3\xe8\xe8\x20\xed\xe0\x20\x33\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x2e" /* "{Брошь ведьмы}\n(Сила магии +3)\n\nБрошь ведьмы увеличивает Силу магии на 3 единицы." */),
    DATA_COMPGEN(0x005012fc, gArtifactDescMedalOfValor, "\x7b\xcc\xe5\xe4\xe0\xeb\xfc\x20\xee\xf2\xe2\xe0\xe3\xe8\x7d\x0a\x0a\xcc\xe5\xe4\xe0\xeb\xfc\x20\xee\xf2\xe2\xe0\xe3\xe8\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xec\xee\xf0\xe0\xeb\xfc\x2e" /* "{Медаль отваги}\n\nМедаль отваги увеличивает мораль." */),
    DATA_COMPGEN(0x00501330, gArtifactDescMedalOfCourage, "\x7b\xcc\xe5\xe4\xe0\xeb\xfc\x20\xec\xf3\xe6\xe5\xf1\xf2\xe2\xe0\x7d\x0a\x0a\xcc\xe5\xe4\xe0\xeb\xfc\x20\xec\xf3\xe6\xe5\xf1\xf2\xe2\xe0\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xec\xee\xf0\xe0\xeb\xfc\x2e" /* "{Медаль мужества}\n\nМедаль мужества увеличивает мораль." */),
    DATA_COMPGEN(0x00501368, gArtifactDescMedalOfHonor, "\x7b\xcc\xe5\xe4\xe0\xeb\xfc\x20\xe4\xee\xe1\xeb\xe5\xf1\xf2\xe8\x7d\x0a\x0a\xcc\xe5\xe4\xe0\xeb\xfc\x20\xe4\xee\xe1\xeb\xe5\xf1\xf2\xe8\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xec\xee\xf0\xe0\xeb\xfc\x2e" /* "{Медаль доблести}\n\nМедаль доблести увеличивает мораль." */),
    DATA_COMPGEN(0x005013a0, gArtifactDescMedalOfDistinction, "\x7b\xcc\xe5\xe4\xe0\xeb\xfc\x20\xef\xee\xf7\xe5\xf2\xe0\x7d\x0a\x0a\xcc\xe5\xe4\xe0\xeb\xfc\x20\xef\xee\xf7\xe5\xf2\xe0\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xec\xee\xf0\xe0\xeb\xfc\x2e" /* "{Медаль почета}\n\nМедаль почета увеличивает мораль." */),
    DATA_COMPGEN(0x005013d4, gArtifactDescFizbinOfMisfortune, "\x7b\xd1\xe8\xec\xe2\xee\xeb\x20\xed\xe5\xf3\xe4\xe0\xf7\xe8\x7d\x0a\x0a\xd1\xe8\xec\xe2\xee\xeb\x20\xed\xe5\xf3\xe4\xe0\xf7\xe8\x20\xf1\xe8\xeb\xfc\xed\xee\x20\xf3\xec\xe5\xed\xfc\xf8\xe0\xe5\xf2\x20\xec\xee\xf0\xe0\xeb\xfc\x2e" /* "{Символ неудачи}\n\nСимвол неудачи сильно уменьшает мораль." */),
    DATA_COMPGEN(0x00501410, gArtifactDescThunderMaceOfDominion, "\x7b\xc3\xf0\xee\xec\xee\xe2\xe0\xff\x20\xef\xe0\xeb\xe8\xf6\xe0\x7d\x0a\x28\xc0\xf2\xe0\xea\xe0\x20\x2b\x31\x29\x0a\x0a\xc3\xf0\xee\xec\xee\xe2\xe0\xff\x20\xef\xe0\xeb\xe8\xf6\xe0\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\xe2\xfb\xea\x20\xc0\xf2\xe0\xea\xe8\x20\xed\xe0\x20\x31\x20\xe5\xe4\xe8\xed\xe8\xf6\xf3\x2e" /* "{Громовая палица}\n(Атака +1)\n\nГромовая палица увеличивает навык Атаки на 1 единицу." */),
    DATA_COMPGEN(0x00501464, gArtifactDescArmoredGauntletsOfProtection, "\x7b\xc7\xe0\xf9\xe8\xf2\xed\xe0\xff\x20\xef\xe5\xf0\xf7\xe0\xf2\xea\xe0\x7d\x0a\x28\xc7\xe0\xf9\xe8\xf2\xe0\x20\x2b\x31\x29\x0a\x0a\xc7\xe0\xf9\xe8\xf2\xed\xe0\xff\x20\xef\xe5\xf0\xf7\xe0\xf2\xea\xe0\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\xe2\xfb\xea\x20\xc7\xe0\xf9\xe8\xf2\xfb\x20\xed\xe0\x20\x31\x20\xe5\xe4\xe8\xed\xe8\xf6\xf3\x2e" /* "{Защитная перчатка}\n(Защита +1)\n\nЗащитная перчатка увеличивает навык Защиты на 1 единицу." */),
    DATA_COMPGEN(0x005014c0, gArtifactDescDefenderHelmOfProtection, "\x7b\xd8\xeb\xe5\xec\x20\xe7\xe0\xf9\xe8\xf2\xed\xe8\xea\xe0\x7d\x0a\x28\xc7\xe0\xf9\xe8\xf2\xe0\x20\x2b\x31\x29\x0a\x0a\xd8\xeb\xe5\xec\x20\xe7\xe0\xf9\xe8\xf2\xed\xe8\xea\xe0\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\xe2\xfb\xea\x20\xc7\xe0\xf9\xe8\xf2\xfb\x20\xed\xe0\x20\x31\x20\xe5\xe4\xe8\xed\xe8\xf6\xf3\x2e" /* "{Шлем защитника}\n(Защита +1)\n\nШлем защитника увеличивает навык Защиты на 1 единицу." */),
    DATA_COMPGEN(0x00501514, gArtifactDescGiantFlailOfDominion, "\x7b\xc3\xe8\xe3\xe0\xed\xf2\xf1\xea\xe8\xe9\x20\xf6\xe5\xef\x7d\x0a\x28\xc0\xf2\xe0\xea\xe0\x20\x2b\x31\x29\x0a\x0a\xc3\xe8\xe3\xe0\xed\xf2\xf1\xea\xe8\xe9\x20\xf6\xe5\xef\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\xe2\xfb\xea\x20\xc0\xf2\xe0\xea\xe8\x20\xed\xe0\x20\x31\x20\xe5\xe4\xe8\xed\xe8\xf6\xf3\x2e" /* "{Гигантский цеп}\n(Атака +1)\n\nГигантский цеп увеличивает навык Атаки на 1 единицу." */),
    DATA_COMPGEN(0x00501568, gArtifactDescBallistaOfQuickness, "\x7b\xc1\xe0\xeb\xeb\xe8\xf1\xf2\xe0\x7d\x0a\x0a\xc1\xe0\xeb\xeb\xe8\xf1\xf2\xe0\x20\xef\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xe2\xe0\xf8\xe5\xe9\x20\xea\xe0\xf2\xe0\xef\xf3\xeb\xfc\xf2\xe5\x20\xe4\xe2\xe0\xe6\xe4\xfb\x20\xf1\xf2\xf0\xe5\xeb\xff\xf2\xfc\x20\xe2\x20\xee\xe4\xe8\xed\x20\xf5\xee\xe4\x20\xe1\xee\xff\x2e" /* "{Баллиста}\n\nБаллиста позволяет вашей катапульте дважды стрелять в один ход боя." */),
    DATA_COMPGEN(0x005015b8, gArtifactDescStealthShieldOfProtection, "\x7b\xcd\xe5\xe7\xf0\xe8\xec\xfb\xe9\x20\xf9\xe8\xf2\x7d\x0a\x28\xc7\xe0\xf9\xe8\xf2\xe0\x20\x2b\x32\x29\x0a\x0a\xcd\xe5\xe7\xf0\xe8\xec\xfb\xe9\x20\xf9\xe8\xf2\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\xe2\xfb\xea\x20\xc7\xe0\xf9\xe8\xf2\xfb\x20\xed\xe0\x20\x32\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x2e" /* "{Незримый щит}\n(Защита +2)\n\nНезримый щит увеличивает навык Защиты на 2 единицы." */),
    DATA_COMPGEN(0x00501608, gArtifactDescDragonSwordOfDominion, "\x7b\xc4\xf0\xe0\xea\xee\xed\xe8\xe9\x20\xec\xe5\xf7\x7d\x0a\x28\xc0\xf2\xe0\xea\xe0\x20\x2b\x33\x29\x0a\x0a\xc4\xf0\xe0\xea\xee\xed\xe8\xe9\x20\xec\xe5\xf7\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\xe2\xfb\xea\x20\xc0\xf2\xe0\xea\xe8\x20\xed\xe0\x20\x33\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x2e" /* "{Драконий меч}\n(Атака +3)\n\nДраконий меч увеличивает навык Атаки на 3 единицы." */),
    DATA_COMPGEN(0x00501658, gArtifactDescPowerAxeOfDominion, "\x7b\xd2\xee\xef\xee\xf0\x20\xe2\xeb\xe0\xf1\xf2\xe8\x7d\x0a\x28\xc0\xf2\xe0\xea\xe0\x20\x2b\x32\x29\x0a\x0a\xd2\xee\xef\xee\xf0\x20\xe2\xeb\xe0\xf1\xf2\xe8\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\xe2\xfb\xea\x20\xc0\xf2\xe0\xea\xe8\x20\xed\xe0\x20\x32\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x2e" /* "{Топор власти}\n(Атака +2)\n\nТопор власти увеличивает навык Атаки на 2 единицы." */),
    DATA_COMPGEN(0x005016a8, gArtifactDescDivineBreastplateOfProtection, "\x7b\xc1\xee\xe6\xe5\xf1\xf2\xe2\xe5\xed\xed\xfb\xe9\x20\xe4\xee\xf1\xef\xe5\xf5\x7d\x0a\x28\xc7\xe0\xf9\xe8\xf2\xe0\x20\x2b\x33\x29\x0a\x0a\xc1\xee\xe6\xe5\xf1\xf2\xe2\xe5\xed\xed\xfb\xe9\x20\xe4\xee\xf1\xef\xe5\xf5\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\xe2\xfb\xea\x20\xc7\xe0\xf9\xe8\xf2\xfb\x20\xed\xe0\x20\x33\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x2e" /* "{Божественный доспех}\n(Защита +3)\n\nБожественный доспех увеличивает навык Защиты на 3 единицы." */),
    DATA_COMPGEN(0x00501708, gArtifactDescMinorScrollOfKnowledge, "\x7b\xcc\xe0\xeb\xfb\xe9\x20\xf1\xe2\xe8\xf2\xee\xea\x20\xe7\xed\xe0\xed\xe8\xff\x7d\x0a\x28\xc7\xed\xe0\xed\xe8\xff\x20\x2b\x32\x29\x0a\x0a\xcc\xe0\xeb\xfb\xe9\x20\xf1\xe2\xe8\xf2\xee\xea\x20\xe7\xed\xe0\xed\xe8\xff\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xc7\xed\xe0\xed\xe8\xff\x20\xed\xe0\x20\x32\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x2e" /* "{Малый свиток знания}\n(Знания +2)\n\nМалый свиток знания увеличивает Знания на 2 единицы." */),
    DATA_COMPGEN(0x00501760, gArtifactDescMajorScrollOfKnowledge, "\x7b\xc1\xee\xeb\xfc\xf8\xee\xe9\x20\xf1\xe2\xe8\xf2\xee\xea\x20\xe7\xed\xe0\xed\xe8\xff\x7d\x0a\x28\xc7\xed\xe0\xed\xe8\xff\x20\x2b\x33\x29\x0a\x0a\xc1\xee\xeb\xfc\xf8\xee\xe9\x20\xf1\xe2\xe8\xf2\xee\xea\x20\xe7\xed\xe0\xed\xe8\xff\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xc7\xed\xe0\xed\xe8\xff\x20\xed\xe0\x20\x33\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x2e" /* "{Большой свиток знания}\n(Знания +3)\n\nБольшой свиток знания увеличивает Знания на 3 единицы." */),
    DATA_COMPGEN(0x005017bc, gArtifactDescSuperiorScrollOfKnowledge, "\x7b\xcc\xee\xe3\xf3\xf9\xe5\xf1\xf2\xe2\xe5\xed\xed\xfb\xe9\x20\xf1\xe2\xe8\xf2\xee\xea\x20\xe7\xed\xe0\xed\xe8\xff\x7d\x0a\x28\xc7\xed\xe0\xed\xe8\xff\x20\x2b\x34\x29\x0a\x0a\xcc\xee\xe3\xf3\xf9\xe5\xf1\xf2\xe2\xe5\xed\xed\xfb\xe9\x20\xf1\xe2\xe8\xf2\xee\xea\x20\xc7\xed\xe0\xed\xe8\xff\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xc7\xed\xe0\xed\xe8\xff\x20\xed\xe0\x20\x34\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x2e" /* "{Могущественный свиток знания}\n(Знания +4)\n\nМогущественный свиток Знания увеличивает Знания на 4 единицы." */),
    DATA_COMPGEN(0x00501828, gArtifactDescForemostScrollOfKnowledge, "\x7b\xd1\xe2\xe8\xf2\xee\xea\x20\xe2\xfb\xf1\xf8\xe5\xe3\xee\x20\xe7\xed\xe0\xed\xe8\xff\x7d\x0a\x28\xc7\xed\xe0\xed\xe8\xff\x20\x2b\x35\x29\x0a\x0a\xd1\xe2\xe8\xf2\xee\xea\x20\xe2\xfb\xf1\xf8\xe5\xe3\xee\x20\xe7\xed\xe0\xed\xe8\xff\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xc7\xed\xe0\xed\xe8\xff\x20\xed\xe0\x20\x35\x20\xe5\xe4\xe8\xed\xe8\xf6\x2e" /* "{Свиток высшего знания}\n(Знания +5)\n\nСвиток высшего знания увеличивает Знания на 5 единиц." */),
    DATA_COMPGEN(0x00501884, gArtifactDescEndlessSackOfGold, "\x7b\xc1\xe5\xe7\xe4\xee\xed\xed\xfb\xe9\x20\xec\xe5\xf8\xee\xea\x7d\x0a\x0a\xc1\xe5\xe7\xe4\xee\xed\xed\xfb\xe9\x20\xec\xe5\xf8\xee\xea\x20\xef\xf0\xe8\xed\xee\xf1\xe8\xf2\x20\xe2\xe0\xec\x20\x31\x30\x30\x30\x20\xe7\xee\xeb\xee\xf2\xfb\xf5\x20\xe2\x20\xe4\xe5\xed\xfc\x2e" /* "{Бездонный мешок}\n\nБездонный мешок приносит вам 1000 золотых в день." */),
    DATA_COMPGEN(0x005018cc, gArtifactDescEndlessBagOfGold, "\x7b\xc1\xe5\xe7\xe4\xee\xed\xed\xe0\xff\x20\xf1\xf3\xec\xe0\x7d\x0a\x0a\xc1\xe5\xe7\xe4\xee\xed\xed\xe0\xff\x20\xf1\xf3\xec\xe0\x20\xef\xf0\xe8\xed\xee\xf1\xe8\xf2\x20\xe2\xe0\xec\x20\x37\x35\x30\x20\xe7\xee\xeb\xee\xf2\xfb\xf5\x20\xe2\x20\xe4\xe5\xed\xfc\x2e" /* "{Бездонная сума}\n\nБездонная сума приносит вам 750 золотых в день." */),
    DATA_COMPGEN(0x00501910, gArtifactDescEndlessPurseOfGold, "\x7b\xc1\xe5\xe7\xe4\xee\xed\xed\xfb\xe9\x20\xea\xee\xf8\xe5\xeb\xfc\x7d\x0a\x0a\xc1\xe5\xe7\xe4\xee\xed\xed\xfb\xe9\x20\xea\xee\xf8\xe5\xeb\xfc\x20\xef\xf0\xe8\xed\xee\xf1\xe8\xf2\x20\xe2\xe0\xec\x20\x35\x30\x30\x20\xe7\xee\xeb\xee\xf2\xfb\xf5\x20\xe2\x20\xe4\xe5\xed\xfc\x2e" /* "{Бездонный кошель}\n\nБездонный кошель приносит вам 500 золотых в день." */),
    DATA_COMPGEN(0x00501958, gArtifactDescNomadBootsOfMobility, "\x7b\xc1\xe0\xf8\xec\xe0\xea\xe8\x20\xea\xee\xf7\xe5\xe2\xed\xe8\xea\xe0\x7d\x0a\x0a\xc1\xe0\xf8\xec\xe0\xea\xe8\x20\xea\xee\xf7\xe5\xe2\xed\xe8\xea\xe0\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xfe\xf2\x20\xe4\xe0\xeb\xfc\xed\xee\xf1\xf2\xfc\x20\xef\xe5\xf0\xe5\xe4\xe2\xe8\xe6\xe5\xed\xe8\xff\x20\xef\xee\x20\xf1\xf3\xf8\xe5\x2e" /* "{Башмаки кочевника}\n\nБашмаки кочевника увеличивают дальность передвижения по суше." */),
    DATA_COMPGEN(0x005019ac, gArtifactDescTravelerSBootsOfMobility, "\x7b\xc1\xe0\xf8\xec\xe0\xea\xe8\x20\xef\xf3\xf2\xed\xe8\xea\xe0\x7d\x0a\x0a\xc1\xe0\xf8\xec\xe0\xea\xe8\x20\xef\xf3\xf2\xed\xe8\xea\xe0\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xfe\xf2\x20\xef\xee\xe4\xe2\xe8\xe6\xed\xee\xf1\xf2\xfc\x20\xee\xf2\xf0\xff\xe4\xe0\x20\xed\xe0\x20\xf1\xf3\xf8\xe5\x2e" /* "{Башмаки путника}\n\nБашмаки путника увеличивают подвижность отряда на суше." */),
    DATA_COMPGEN(0x005019f8, gArtifactDescLuckyRabbitSFoot, "\x7b\xcb\xe0\xef\xea\xe0\x20\xea\xf0\xee\xeb\xe8\xea\xe0\x7d\x0a\x0a\xcb\xe0\xef\xea\xe0\x20\xea\xf0\xee\xeb\xe8\xea\xe0\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xf3\xe4\xe0\xf7\xf3\x20\xe2\x20\xe1\xee\xfe\x2e" /* "{Лапка кролика}\n\nЛапка кролика увеличивает удачу в бою." */),
    DATA_COMPGEN(0x00501a30, gArtifactDescGoldenHorseshoe, "\x7b\xc7\xee\xeb\xee\xf2\xe0\xff\x20\xef\xee\xe4\xea\xee\xe2\xe0\x7d\x0a\x0a\xc7\xee\xeb\xee\xf2\xe0\xff\x20\xef\xee\xe4\xea\xee\xe2\xe0\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xf3\xe4\xe0\xf7\xf3\x20\xe2\x20\xe1\xee\xfe\x2e" /* "{Золотая подкова}\n\nЗолотая подкова увеличивает удачу в бою." */),
    DATA_COMPGEN(0x00501a6c, gArtifactDescGamblerSLuckyCoin, "\x7b\xd1\xf7\xe0\xf1\xf2\xeb\xe8\xe2\xe0\xff\x20\xec\xee\xed\xe5\xf2\xe0\x7d\x0a\x0a\xd1\xf7\xe0\xf1\xf2\xeb\xe8\xe2\xe0\xff\x20\xec\xee\xed\xe5\xf2\xe0\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xf3\xe4\xe0\xf7\xf3\x20\xe2\x20\xe1\xee\xfe\x2e" /* "{Счастливая монета}\n\nСчастливая монета увеличивает удачу в бою." */),
    DATA_COMPGEN(0x00501aac, gArtifactDescFourLeafClover, "\x7b\xca\xeb\xe5\xe2\xe5\xf0\x7d\x0a\x0a\xca\xeb\xe5\xe2\xe5\xf0\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xf3\xe4\xe0\xf7\xf3\x20\xe2\x20\xe1\xee\xfe\x2e" /* "{Клевер}\n\nКлевер увеличивает удачу в бою." */),
    DATA_COMPGEN(0x00501ad8, gArtifactDescTrueCompassOfMobility, "\x7b\xca\xee\xec\xef\xe0\xf1\x7d\x0a\x0a\xca\xee\xec\xef\xe0\xf1\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xef\xee\xe4\xe2\xe8\xe6\xed\xee\xf1\xf2\xfc\x20\xee\xf2\xf0\xff\xe4\xe0\x20\xed\xe0\x20\xf1\xf3\xf8\xe5\x20\xe8\x20\xed\xe0\x20\xec\xee\xf0\xe5\x2e" /* "{Компас}\n\nКомпас увеличивает подвижность отряда на суше и на море." */),
    DATA_COMPGEN(0x00501b1c, gArtifactDescSailorSAstrolabeOfMobility, "\x7b\xc0\xf1\xf2\xf0\xee\xeb\xff\xe1\xe8\xff\x7d\x0a\x0a\xc0\xf1\xf2\xf0\xee\xeb\xff\xe1\xe8\xff\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xef\xee\xe4\xe2\xe8\xe6\xed\xee\xf1\xf2\xfc\x20\xee\xf2\xf0\xff\xe4\xe0\x20\xed\xe0\x20\xec\xee\xf0\xe5\x2e" /* "{Астролябия}\n\nАстролябия увеличивает подвижность отряда на море." */),
    DATA_COMPGEN(0x00501b60, gArtifactDescEvilEye, "\x7b\xc4\xf3\xf0\xed\xee\xe9\x20\xe3\xeb\xe0\xe7\x7d\x0a\x0a\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xf1\xed\xe8\xe6\xe0\xe5\xf2\x20\xe2\xef\xee\xeb\xee\xe2\xe8\xed\xf3\x20\xea\xee\xeb\xe8\xf7\xe5\xf1\xf2\xe2\xee\x20\xec\xe0\xe3\xe8\xf7\xe5\xf1\xea\xee\xe9\x20\xfd\xed\xe5\xf0\xe3\xe8\xe8\x2c\x20\xf2\xf0\xe5\xe1\xf3\xe5\xec\xee\xe9\x20\xed\xe0\x20\xed\xe0\xef\xf0\xe0\xe2\xeb\xe5\xed\xe8\xe5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe9\x2d\xef\xf0\xee\xea\xeb\xff\xf2\xe8\xe9\x2e" /* "{Дурной глаз}\n\nАртефакт снижает вполовину количество магической энергии, требуемой на направление заклинаний-проклятий." */),
    DATA_COMPGEN(0x00501bd8, gArtifactDescEnchantedHourglass, "\x7b\xc7\xe0\xf7\xe0\xf0\xee\xe2\xe0\xed\xed\xfb\xe5\x20\xf7\xe0\xf1\xfb\x7d\x0a\x0a\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xef\xf0\xee\xe4\xeb\xe5\xe2\xe0\xe5\xf2\x20\xe4\xe5\xe9\xf1\xf2\xe2\xe8\xe5\x20\xe2\xf1\xe5\xf5\x20\xe2\xe0\xf8\xe8\xf5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe9\x20\xed\xe0\x20\x32\x20\xf5\xee\xe4\xe0\x2e" /* "{Зачарованные часы}\n\nАртефакт продлевает действие всех ваших заклинаний на 2 хода." */),
    DATA_COMPGEN(0x00501c2c, gArtifactDescGoldWatch, "\x7b\xc7\xee\xeb\xee\xf2\xfb\xe5\x20\xf7\xe0\xf1\xfb\x7d\x0a\x0a\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xf3\xe4\xe2\xe0\xe8\xe2\xe0\xe5\xf2\x20\xfd\xf4\xf4\xe5\xea\xf2\xe8\xe2\xed\xee\xf1\xf2\xfc\x20\xe8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xed\xe8\xff\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\x20\xe3\xe8\xef\xed\xee\xe7\xe0\x2e" /* "{Золотые часы}\n\nАртефакт удваивает эффективность использования заклинания гипноза." */),
    DATA_COMPGEN(0x00501c80, gArtifactDescSkullcap, "\x7b\xd8\xe0\xef\xee\xf7\xea\xe0\x7d\x0a\x0a\xd1\xed\xe8\xe6\xe0\xe5\xf2\x20\xe2\xef\xee\xeb\xee\xe2\xe8\xed\xf3\x20\xe7\xe0\xf2\xf0\xe0\xf2\xfb\x20\xec\xe0\xe3\xe8\xf7\xe5\xf1\xea\xee\xe9\x20\xfd\xed\xe5\xf0\xe3\xe8\xe8\x20\xed\xe0\x20\xe2\xf1\xe5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\x20\xe2\xeb\xe8\xff\xfe\xf9\xe8\xe5\x20\xed\xe0\x20\xf0\xe0\xe7\xf3\xec\x2e" /* "{Шапочка}\n\nСнижает вполовину затраты магической энергии на все заклинания влияющие на разум." */),
    DATA_COMPGEN(0x00501ce0, gArtifactDescIceCloak, "\x7b\xcb\xe5\xe4\xff\xed\xe0\xff\x20\xed\xe0\xea\xe8\xe4\xea\xe0\x7d\x0a\x0a\xd1\xed\xe8\xe6\xe0\xe5\xf2\x20\xe2\xef\xee\xeb\xee\xe2\xe8\xed\xf3\x20\xf3\xf0\xee\xed\x2c\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\xe2\xe0\xf8\xe8\xec\x20\xe2\xee\xe8\xed\xe0\xec\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\xec\xe8\x20\xf5\xee\xeb\xee\xe4\xe0\x2e" /* "{Ледяная накидка}\n\nСнижает вполовину урон, наносимый вашим воинам заклинаниями холода." */),
    DATA_COMPGEN(0x00501d38, gArtifactDescFireCloak, "\x7b\xce\xe3\xed\xe5\xed\xed\xe0\xff\x20\xed\xe0\xea\xe8\xe4\xea\xe0\x7d\x0a\x0a\xd1\xed\xe8\xe6\xe0\xe5\xf2\x20\xe2\xef\xee\xeb\xee\xe2\xe8\xed\xf3\x20\xf3\xf0\xee\xed\x2c\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\xe2\xe0\xf8\xe8\xec\x20\xe2\xee\xe8\xed\xe0\xec\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\xec\xe8\x20\xee\xe3\xed\xff\x2e" /* "{Огненная накидка}\n\nСнижает вполовину урон, наносимый вашим воинам заклинаниями огня." */),
    DATA_COMPGEN(0x00501d90, gArtifactDescLightningHelm, "\x7b\xc3\xf0\xee\xec\xee\xe2\xee\xe9\x20\xf8\xeb\xe5\xec\x7d\x0a\x0a\xd1\xed\xe8\xe6\xe0\xe5\xf2\x20\xe2\xef\xee\xeb\xee\xe2\xe8\xed\xf3\x20\xf3\xf0\xee\xed\x2c\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\xe2\xe0\xf8\xe8\xec\x20\xe2\xee\xe8\xed\xe0\xec\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\xec\xe8\x20\xec\xee\xeb\xed\xe8\xe9\x2e" /* "{Громовой шлем}\n\nСнижает вполовину урон, наносимый вашим воинам заклинаниями молний." */),
    DATA_COMPGEN(0x00501de8, gArtifactDescEvercoldIcicle, "\x7b\xcd\xe5\xf2\xe0\xfe\xf9\xe8\xe9\x20\xeb\xe5\xe4\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\x20\x35\x30\x25\x20\xf3\xf0\xee\xed\x2c\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\xe2\xf0\xe0\xe3\xf3\x20\xe2\xe0\xf8\xe8\xec\xe8\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\xec\xe8\x20\xf5\xee\xeb\xee\xe4\xe0\x2e" /* "{Нетающий лед}\n\nУвеличивает на 50% урон, наносимый врагу вашими заклинаниями холода." */),
    DATA_COMPGEN(0x00501e40, gArtifactDescEverhotLavaRock, "\x7b\xc3\xee\xf0\xff\xf7\xe8\xe9\x20\xea\xe0\xec\xe5\xed\xfc\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\x20\x35\x30\x25\x20\xf3\xf0\xee\xed\x2c\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\xe2\xf0\xe0\xe3\xf3\x20\xe2\xe0\xf8\xe8\xec\xe8\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\xec\xe8\x20\xee\xe3\xed\xff\x2e" /* "{Горячий камень}\n\nУвеличивает на 50% урон, наносимый врагу вашими заклинаниями огня." */),
    DATA_COMPGEN(0x00501e98, gArtifactDescLightningRod, "\x7b\xc6\xe5\xe7\xeb\x20\xec\xee\xeb\xed\xe8\xe9\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\x20\x35\x30\x25\x20\xf3\xf0\xee\xed\x2c\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\xe2\xf0\xe0\xe3\xf3\x20\xe2\xe0\xf8\xe8\xec\xe8\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\xec\xe8\x20\xec\xee\xeb\xed\xe8\xe9\x2e" /* "{Жезл молний}\n\nУвеличивает на 50% урон, наносимый врагу вашими заклинаниями молний." */),
    DATA_COMPGEN(0x00501eec, gArtifactDescSnakeRing, "\x7b\xca\xee\xeb\xfc\xf6\xee\x20\xe7\xec\xe5\xe8\x7d\x0a\x0a\xd1\xed\xe8\xe6\xe0\xe5\xf2\x20\xe2\xef\xee\xeb\xee\xe2\xe8\xed\xf3\x20\xe7\xe0\xf2\xf0\xe0\xf2\xfb\x20\xec\xe0\xe3\xe8\xf7\xe5\xf1\xea\xee\xe9\x20\xfd\xed\xe5\xf0\xe3\xe8\xe8\x20\xed\xe0\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\x2d\xe1\xeb\xe0\xe3\xee\xf1\xeb\xee\xe2\xe5\xed\xe8\xff\x2e" /* "{Кольцо змеи}\n\nСнижает вполовину затраты магической энергии на заклинания-благословения." */),
    DATA_COMPGEN(0x00501f48, gArtifactDescAnkh, "\x7b\xd1\xe8\xec\xe2\xee\xeb\x20\xe6\xe8\xe7\xed\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xe2\xe4\xe2\xee\xe5\x20\xfd\xf4\xf4\xe5\xea\xf2\xe8\xe2\xed\xee\xf1\xf2\xfc\x20\xe2\xf1\xe5\xf5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe9\x20\xf1\xe2\xff\xe7\xe0\xed\xed\xfb\xf5\x20\xf1\x20\xe2\xee\xf1\xea\xf0\xe5\xf8\xe5\xed\xe8\xe5\xec\x20\xe8\x20\xee\xe6\xe8\xe2\xeb\xe5\xed\xe8\xe5\xec\x20\xf1\xf3\xf9\xe5\xf1\xf2\xe2\x2e" /* "{Символ жизни}\n\nУвеличивает вдвое эффективность всех заклинаний связанных с воскрешением и оживлением существ." */),
    DATA_COMPGEN(0x00501fb8, gArtifactDescBookOfElements, "\x7b\xca\xed\xe8\xe3\xe0\x20\xf1\xf2\xe8\xf5\xe8\xe9\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xe2\xe4\xe2\xee\xe5\x20\xfd\xf4\xf4\xe5\xea\xf2\xe8\xe2\xed\xee\xf1\xf2\xfc\x20\xe2\xf1\xe5\xf5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe9\x2c\x20\xf1\xe2\xff\xe7\xe0\xed\xed\xfb\xf5\x20\xf1\x20\xef\xf0\xe8\xe7\xfb\xe2\xee\xec\x20\xf1\xf3\xf9\xe5\xf1\xf2\xe2\x2e" /* "{Книга стихий}\n\nУвеличивает вдвое эффективность всех заклинаний, связанных с призывом существ." */),
    DATA_COMPGEN(0x00502018, gArtifactDescElementalRing, "\x7b\xca\xee\xeb\xfc\xf6\xee\x20\xf1\xf2\xe8\xf5\xe8\xe9\x7d\x0a\x0a\xd1\xed\xe8\xe6\xe0\xe5\xf2\x20\xe2\xef\xee\xeb\xee\xe2\xe8\xed\xf3\x20\xe7\xe0\xf2\xf0\xe0\xf2\xfb\x20\xed\xe0\x20\xe2\xf1\xe5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\x2c\x20\xf1\xe2\xff\xe7\xe0\xed\xed\xfb\xe5\x20\xf1\x20\xe2\xfb\xe7\xee\xe2\xee\xec\x20\xf1\xf3\xf9\xe5\xf1\xf2\xe2\x2e" /* "{Кольцо стихий}\n\nСнижает вполовину затраты на все заклинания, связанные с вызовом существ." */),
    DATA_COMPGEN(0x00502074, gArtifactDescHolyPendant, "\x7b\xd1\xe2\xff\xf2\xee\xe9\x20\xea\xf3\xeb\xee\xed\x7d\x0a\x0a\xcd\xe0\xe4\xe5\xeb\xff\xe5\xf2\x20\xe2\xe0\xf8\xe8\xf5\x20\xe2\xee\xe8\xed\xee\xe2\x20\xe8\xec\xec\xf3\xed\xe8\xf2\xe5\xf2\xee\xec\x20\xea\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\xec\x2d\xef\xf0\xee\xea\xeb\xff\xf2\xe8\xff\xec\x2e" /* "{Святой кулон}\n\nНаделяет ваших воинов иммунитетом к заклинаниям-проклятиям." */),
    DATA_COMPGEN(0x005020c0, gArtifactDescPendantOfFreeWill, "\x7b\xcf\xee\xe4\xe2\xe5\xf1\xea\xe0\x20\xf1\xe2\xee\xe1\xee\xe4\xed\xee\xe9\x20\xe2\xee\xeb\xe8\x7d\x0a\x0a\xcd\xe0\xe4\xe5\xeb\xff\xe5\xf2\x20\xe2\xe0\xf8\xe8\xf5\x20\xe2\xee\xe8\xed\xee\xe2\x20\xe8\xec\xec\xf3\xed\xe8\xf2\xe5\xf2\xee\xec\x20\xea\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\xec\x2c\x20\xf1\xe2\xff\xe7\xe0\xed\xed\xfb\xec\x20\xf1\x20\xe3\xe8\xef\xed\xee\xe7\xee\xec\x2e" /* "{Подвеска свободной воли}\n\nНаделяет ваших воинов иммунитетом к заклинаниям, связанным с гипнозом." */),
    DATA_COMPGEN(0x00502124, gArtifactDescPendantOfLife, "\x7b\xca\xf3\xeb\xee\xed\x20\xe6\xe8\xe7\xed\xe8\x7d\x0a\x0a\xcd\xe0\xe4\xe5\xeb\xff\xe5\xf2\x20\xe2\xe0\xf8\xe8\xf5\x20\xe2\xee\xe8\xed\xee\xe2\x20\xe8\xec\xec\xf3\xed\xe8\xf2\xe5\xf2\xee\xec\x20\xea\xee\x20\xe2\xf1\xe5\xec\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\xec\x20\xd1\xec\xe5\xf0\xf2\xe8\x2e" /* "{Кулон жизни}\n\nНаделяет ваших воинов иммунитетом ко всем заклинаниям Смерти." */),
    DATA_COMPGEN(0x00502174, gArtifactDescSerenityPendant, "\x7b\xcf\xee\xe4\xe2\xe5\xf1\xea\xe0\x20\xef\xee\xea\xee\xff\x7d\x0a\x0a\xcd\xe0\xe4\xe5\xeb\xff\xe5\xf2\x20\xe2\xe0\xf8\xe8\xf5\x20\xe2\xee\xe8\xed\xee\xe2\x20\xe8\xec\xec\xf3\xed\xe8\xf2\xe5\xf2\xee\xec\x20\xea\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xfe\x20\xc1\xe5\xf0\xf1\xe5\xf0\xea\x2e" /* "{Подвеска покоя}\n\nНаделяет ваших воинов иммунитетом к заклинанию Берсерк." */),
    DATA_COMPGEN(0x005021c0, gArtifactDescSeeingEyePendant, "\x7b\xc2\xf1\xe5\xe2\xe8\xe4\xff\xf9\xe8\xe9\x20\xe3\xeb\xe0\xe7\x7d\x0a\x0a\xcd\xe0\xe4\xe5\xeb\xff\xe5\xf2\x20\xe2\xe0\xf8\xe8\xf5\x20\xe2\xee\xe8\xed\xee\xe2\x20\xe8\xec\xec\xf3\xed\xe8\xf2\xe5\xf2\xee\xec\x20\xea\xee\x20\xe2\xf1\xe5\xec\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\xec\x20\xee\xf1\xeb\xe5\xef\xeb\xe5\xed\xe8\xff\x2e" /* "{Всевидящий глаз}\n\nНаделяет ваших воинов иммунитетом ко всем заклинаниям ослепления." */),
    DATA_COMPGEN(0x00502218, gArtifactDescKineticPendant, "\x7b\xca\xf3\xeb\xee\xed\x20\xe4\xe2\xe8\xe6\xe5\xed\xe8\xff\x7d\x0a\x0a\xcd\xe0\xe4\xe5\xeb\xff\xe5\xf2\x20\xe2\xe0\xf8\xe8\xf5\x20\xe2\xee\xe8\xed\xee\xe2\x20\xe8\xec\xec\xf3\xed\xe8\xf2\xe5\xf2\xee\xec\x20\xea\xee\x20\xe2\xf1\xe5\xec\x20\xef\xe0\xf0\xe0\xeb\xe8\xe7\xf3\xfe\xf9\xe8\xec\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\xec\x2e" /* "{Кулон движения}\n\nНаделяет ваших воинов иммунитетом ко всем парализующим заклинаниям." */),
    DATA_COMPGEN(0x00502270, gArtifactDescPendantOfDeath, "\x7b\xca\xf3\xeb\xee\xed\x20\xf1\xec\xe5\xf0\xf2\xe8\x7d\x0a\x0a\xcd\xe0\xe4\xe5\xeb\xff\xe5\xf2\x20\xe2\xe0\xf8\xe8\xf5\x20\xe2\xee\xe8\xed\xee\xe2\x20\xe8\xec\xec\xf3\xed\xe8\xf2\xe5\xf2\xee\xec\x20\xea\xee\x20\xe2\xf1\xe5\xec\x20\xf1\xe2\xff\xf2\xfb\xec\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\xec\x2e" /* "{Кулон смерти}\n\nНаделяет ваших воинов иммунитетом ко всем святым заклинаниям." */),
    DATA_COMPGEN(0x005022c0, gArtifactDescWandOfNegation, "\x7b\xcf\xee\xf1\xee\xf5\x20\xee\xf2\xf0\xe8\xf6\xe0\xed\xe8\xff\x7d\x0a\x0a\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xe7\xe0\xf9\xe8\xf9\xe0\xe5\xf2\x20\xe2\xe0\xf8\xe8\xf5\x20\xe2\xee\xe8\xed\xee\xe2\x20\xee\xf2\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\x20\xf1\xed\xff\xf2\xe8\xff\x20\xf7\xe0\xf0\x2e" /* "{Посох отрицания}\n\nАртефакт защищает ваших воинов от заклинания снятия чар." */),
    DATA_COMPGEN(0x0050230c, gArtifactDescGoldenBow, "\x7b\xc7\xee\xeb\xee\xf2\xee\xe9\x20\xeb\xf3\xea\x7d\x0a\x0a\xd1\xed\xe8\xe6\xe0\xe5\xf2\x20\xe2\xef\xee\xeb\xee\xe2\xe8\xed\xf3\x20\xf8\xf2\xf0\xe0\xf4\x20\xed\xe0\x20\xf3\xf0\xee\xed\x20\xe4\xeb\xff\x20\xe2\xe0\xf8\xe8\xf5\x20\xe2\xee\xe8\xed\xee\xe2\x2c\x20\xf1\xf2\xf0\xe5\xeb\xff\xfe\xf9\xe8\xf5\x20\xf7\xe5\xf0\xe5\xe7\x20\xef\xf0\xe5\xef\xff\xf2\xf1\xf2\xe2\xe8\xff\x20\x28\xed\xe0\xef\xf0\xe8\xec\xe5\xf0\x2c\x20\xf1\xf2\xe5\xed\xfb\x20\xe7\xe0\xec\xea\xe0\x29\x2e" /* "{Золотой лук}\n\nСнижает вполовину штраф на урон для ваших воинов, стреляющих через препятствия (например, стены замка)." */),
    DATA_COMPGEN(0x00502384, gArtifactDescTelescope, "\x7b\xd2\xe5\xeb\xe5\xf1\xea\xee\xef\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xf0\xe0\xe4\xe8\xf3\xf1\x20\xee\xe1\xe7\xee\xf0\xe0\x20\xf1\xf2\xf0\xe0\xed\xf1\xf2\xe2\xf3\xfe\xf9\xe5\xe3\xee\x20\xe3\xe5\xf0\xee\xff\x20\xed\xe0\x20\x31\x20\xea\xeb\xe5\xf2\xea\xf3\x2e" /* "{Телескоп}\n\nУвеличивает радиус обзора странствующего героя на 1 клетку." */),
    DATA_COMPGEN(0x005023cc, gArtifactDescStatesmanSQuill, "\x7b\xcf\xe5\xf0\xee\x20\xe4\xe8\xef\xeb\xee\xec\xe0\xf2\xe0\x7d\x0a\x0a\xd1\xed\xe8\xe6\xe0\xe5\xf2\x20\xf1\xf2\xee\xe8\xec\xee\xf1\xf2\xfc\x20\xf1\xe4\xe0\xf7\xe8\x20\xed\xe0\x20\x31\x30\x25\x20\xee\xf2\x20\xee\xe1\xf9\xe5\xe9\x20\xf1\xf2\xee\xe8\xec\xee\xf1\xf2\xe8\x20\xe0\xf0\xec\xe8\xe8\x20\xe2\xe0\xf8\xe5\xe3\xee\x20\xe3\xe5\xf0\xee\xff\x2e" /* "{Перо дипломата}\n\nСнижает стоимость сдачи на 10% от общей стоимости армии вашего героя." */),
    DATA_COMPGEN(0x00502424, gArtifactDescWizardSHat, "\x7b\xd8\xeb\xff\xef\xe0\x20\xec\xe0\xe3\xe0\x7d\x0a\x0a\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xef\xf0\xee\xe4\xeb\xe5\xe2\xe0\xe5\xf2\x20\xe4\xe5\xe9\xf1\xf2\xe2\xe8\xe5\x20\xe2\xe0\xf8\xe8\xf5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe9\x20\xed\xe0\x20\x31\x30\x20\xf5\xee\xe4\xee\xe2\x21" /* "{Шляпа мага}\n\nАртефакт продлевает действие ваших заклинаний на 10 ходов!" */),
    DATA_COMPGEN(0x00502470, gArtifactDescPowerRing, "\x7b\xca\xee\xeb\xfc\xf6\xee\x20\xf1\xe8\xeb\xfb\x7d\x0a\x0a\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xe2\xee\xe7\xe2\xf0\xe0\xf9\xe0\xe5\xf2\x20\xe3\xe5\xf0\xee\xfe\x20\x32\x20\xe4\xee\xef\xee\xeb\xed\xe8\xf2\xe5\xeb\xfc\xed\xfb\xf5\x20\xee\xf7\xea\xe0\x20\xec\xe0\xe3\xe8\xe8\x20\xe7\xe0\x20\xf5\xee\xe4\x2e" /* "{Кольцо силы}\n\nАртефакт возвращает герою 2 дополнительных очка магии за ход." */),
    DATA_COMPGEN(0x005024c0, gArtifactDescAmmoCart, "\x7b\xce\xe1\xee\xe7\x7d\x0a\x0a\xce\xe1\xe5\xf1\xef\xe5\xf7\xe8\xe2\xe0\xe5\xf2\x20\xe2\xe0\xf8\xe8\xf5\x20\xe2\xee\xe8\xed\xee\xe2\x2d\xf1\xf2\xf0\xe5\xeb\xea\xee\xe2\x20\xed\xe5\xf1\xea\xee\xed\xf7\xe0\xe5\xec\xfb\xec\x20\xe7\xe0\xef\xe0\xf1\xee\xec\x20\xf1\xf2\xf0\xe5\xeb\x2e" /* "{Обоз}\n\nОбеспечивает ваших воинов-стрелков нескончаемым запасом стрел." */),
    DATA_COMPGEN(0x00502508, gArtifactDescTaxLien, "\x7b\xcf\xee\xe4\xe0\xf2\xfc\x7d\x0a\x0a\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xef\xf0\xe8\xed\xf3\xe6\xe4\xe0\xe5\xf2\x20\xe2\xe0\xf1\x20\xe2\xfb\xef\xeb\xe0\xf7\xe8\xe2\xe0\xf2\xfc\x20\xea\xe0\xe6\xe4\xfb\xe9\x20\xf5\xee\xe4\x20\x32\x35\x30\x20\xe7\xee\xeb\xee\xf2\xfb\xf5\x20\xed\xe0\xeb\xee\xe3\xee\xe2\x2e" /* "{Подать}\n\nАртефакт принуждает вас выплачивать каждый ход 250 золотых налогов." */),
    DATA_COMPGEN(0x00502558, gArtifactDescHideousMask, "\x7b\xd3\xe6\xe0\xf1\xed\xe0\xff\x20\xec\xe0\xf1\xea\xe0\x7d\x0a\x0a\xdd\xf2\xee\xf2\x20\xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xed\xe5\x20\xef\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xeb\xfe\xe1\xfb\xec\x20\xe2\xee\xe8\xed\xe0\xec\x20\xe8\x20\xf1\xf3\xf9\xe5\xf1\xf2\xe2\xe0\xec\x20\xe2\xf1\xf2\xf3\xef\xe8\xf2\xfc\x20\xe2\x20\xe2\xe0\xf8\xf3\x20\xe0\xf0\xec\xe8\xfe\x2e" /* "{Ужасная маска}\n\nЭтот артефакт не позволяет любым воинам и существам вступить в вашу армию." */),
    DATA_COMPGEN(0x005025b4, gArtifactDescEndlessPouchOfSulfur, "\x7b\xc1\xe5\xe7\xe4\xee\xed\xed\xe0\xff\x20\xf1\xf3\xec\xe0\x20\xf1\xe5\xf0\xfb\x7d\x0a\x0a\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xef\xf0\xe8\xed\xee\xf1\xe8\xf2\x20\xe2\xe0\xec\x20\x31\x20\xe5\xe4\xe8\xed\xe8\xf6\xf3\x20\xf1\xe5\xf0\xfb\x20\xe2\x20\xe4\xe5\xed\xfc\x2e" /* "{Бездонная сума серы}\n\nАртефакт приносит вам 1 единицу серы в день." */),
    DATA_COMPGEN(0x005025f8, gArtifactDescEndlessVialOfMercury, "\x7b\xc1\xe5\xe7\xe4\xee\xed\xed\xe0\xff\x20\xea\xee\xeb\xe1\xe0\x20\xf0\xf2\xf3\xf2\xe8\x7d\x0a\x0a\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xef\xf0\xe8\xed\xee\xf1\xe8\xf2\x20\xe2\xe0\xec\x20\x31\x20\xe5\xe4\xe8\xed\xe8\xf6\xf3\x20\xf0\xf2\xf3\xf2\xe8\x20\xe2\x20\xe4\xe5\xed\xfc\x2e" /* "{Бездонная колба ртути}\n\nАртефакт приносит вам 1 единицу ртути в день." */),
    DATA_COMPGEN(0x00502640, gArtifactDescEndlessPouchOfGems, "\x7b\xc1\xe5\xe7\xe4\xee\xed\xed\xe0\xff\x20\xf1\xf3\xec\xe0\x20\xf1\xe0\xec\xee\xf6\xe2\xe5\xf2\xee\xe2\x7d\x0a\x0a\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xef\xf0\xe8\xed\xee\xf1\xe8\xf2\x20\xe2\xe0\xec\x20\x31\x20\xe5\xe4\xe8\xed\xe8\xf6\xf3\x20\xf1\xe0\xec\xee\xf6\xe2\xe5\xf2\xee\xe2\x20\xe2\x20\xe4\xe5\xed\xfc\x2e" /* "{Бездонная сума самоцветов}\n\nАртефакт приносит вам 1 единицу самоцветов в день." */),
    DATA_COMPGEN(0x00502690, gArtifactDescEndlessCordOfWood, "\x7b\xcd\xe5\xf1\xea\xee\xed\xf7\xe0\xe5\xec\xe0\xff\x20\xe2\xff\xe7\xe0\xed\xea\xe0\x20\xe4\xf0\xee\xe2\x7d\x0a\x0a\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xef\xf0\xe8\xed\xee\xf1\xe8\xf2\x20\xe2\xe0\xec\x20\x31\x20\xe5\xe4\xe8\xed\xe8\xf6\xf3\x20\xe4\xf0\xe5\xe2\xe5\xf1\xe8\xed\xfb\x20\xe2\x20\xe4\xe5\xed\xfc\x2e" /* "{Нескончаемая вязанка дров}\n\nАртефакт приносит вам 1 единицу древесины в день." */),
    DATA_COMPGEN(0x005026e0, gArtifactDescEndlessCartOfOre, "\x7b\xc1\xe5\xe7\xe4\xee\xed\xed\xe0\xff\x20\xe2\xe0\xe3\xee\xed\xe5\xf2\xea\xe0\x20\xf0\xf3\xe4\xfb\x7d\x0a\x0a\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xef\xf0\xe8\xed\xee\xf1\xe8\xf2\x20\xe2\xe0\xec\x20\x31\x20\xe5\xe4\xe8\xed\xe8\xf6\xf3\x20\xf0\xf3\xe4\xfb\x20\xe2\x20\xe4\xe5\xed\xfc\x2e" /* "{Бездонная вагонетка руды}\n\nАртефакт приносит вам 1 единицу руды в день." */),
    DATA_COMPGEN(0x0050272c, gArtifactDescEndlessPouchOfCrystal, "\x7b\xc1\xe5\xe7\xe4\xee\xed\xed\xe0\xff\x20\xf1\xf3\xec\xe0\x20\xea\xf0\xe8\xf1\xf2\xe0\xeb\xeb\xee\xe2\x7d\x0a\x0a\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xef\xf0\xe8\xed\xee\xf1\xe8\xf2\x20\xe2\xe0\xec\x20\x31\x20\xe5\xe4\xe8\xed\xe8\xf6\xf3\x20\xea\xf0\xe8\xf1\xf2\xe0\xeb\xeb\xee\xe2\x20\xe2\x20\xe4\xe5\xed\xfc\x2e" /* "{Бездонная сума кристаллов}\n\nАртефакт приносит вам 1 единицу кристаллов в день." */),
    DATA_COMPGEN(0x0050277c, gArtifactDescSpikedHelm, "\x7b\xd8\xeb\xe5\xec\x20\xf1\x20\xf8\xe8\xef\xe0\xec\xe8\x7d\x0a\x0a\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xef\xe0\xf0\xe0\xec\xe5\xf2\xf0\xfb\x20\xc0\xf2\xe0\xea\xe8\x20\xe8\x20\xc7\xe0\xf9\xe8\xf2\xfb\x20\xed\xe0\x20\x31\x20\xe5\xe4\xe8\xed\xe8\xf6\xf3\x20\xea\xe0\xe6\xe4\xfb\xe9\x2e" /* "{Шлем с шипами}\n\nАртефакт увеличивает параметры Атаки и Защиты на 1 единицу каждый." */),
    DATA_COMPGEN(0x005027d0, gArtifactDescSpikedShield, "\x7b\xd9\xe8\xf2\x20\xf1\x20\xf8\xe8\xef\xe0\xec\xe8\x7d\x0a\x0a\x20\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xef\xe0\xf0\xe0\xec\xe5\xf2\xf0\xfb\x20\xc0\xf2\xe0\xea\xe8\x20\xe8\x20\xc7\xe0\xf9\xe8\xf2\xfb\x20\xed\xe0\x20\x32\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x20\xea\xe0\xe6\xe4\xfb\xe9\x2e" /* "{Щит с шипами}\n\n Артефакт увеличивает параметры Атаки и Защиты на 2 единицы каждый." */),
    DATA_COMPGEN(0x00502824, gArtifactDescWhitePearl, "\x7b\xc1\xe5\xeb\xe0\xff\x20\xe6\xe5\xec\xf7\xf3\xe6\xe8\xed\xe0\x7d\x0a\x0a\x20\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xef\xe0\xf0\xe0\xec\xe5\xf2\xf0\xfb\x20\xd1\xe8\xeb\xfb\x20\xec\xe0\xe3\xe8\xe8\x20\xe8\x20\xc7\xed\xe0\xed\xe8\xff\x20\xed\xe0\x20\x31\x20\xe5\xe4\xe8\xed\xe8\xf6\xf3\x20\xea\xe0\xe6\xe4\xfb\xe9\x2e" /* "{Белая жемчужина}\n\n Артефакт увеличивает параметры Силы магии и Знания на 1 единицу каждый." */),
    DATA_COMPGEN(0x00502880, gArtifactDescBlackPearl, "\x7b\xd7\xe5\xf0\xed\xe0\xff\x20\xe6\xe5\xec\xf7\xf3\xe6\xe8\xed\xe0\x7d\x0a\x0a\x20\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xef\xe0\xf0\xe0\xec\xe5\xf2\xf0\xfb\x20\xd1\xe8\xeb\xfb\x20\xec\xe0\xe3\xe8\xe8\x20\xe8\x20\xc7\xed\xe0\xed\xe8\xff\x20\xed\xe0\x20\x32\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x20\xea\xe0\xe6\xe4\xfb\xe9\x2e" /* "{Черная жемчужина}\n\n Артефакт увеличивает параметры Силы магии и Знания на 2 единицы каждый." */),
    DATA_COMPGEN(0x005028e0, gArtifactDescMagicBook, "\x7b\xc2\xee\xeb\xf8\xe5\xe1\xed\xe0\xff\x20\xea\xed\xe8\xe3\xe0\x7d\x0a\x0a\xc2\xee\xeb\xf8\xe5\xe1\xed\xe0\xff\x20\xea\xed\xe8\xe3\xe0\x20\xef\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xed\xe0\xef\xf0\xe0\xe2\xeb\xff\xf2\xfc\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\x2e" /* "{Волшебная книга}\n\nВолшебная книга позволяет направлять заклинания." */),
    DATA_COMPGEN(0x00502924, gArtifactDescERRORArtifact82, "\x7b\x45\x52\x52\x4f\x52\x7d\x0a\x0a\x41\x72\x74\x69\x66\x61\x63\x74\x20\x38\x32\x2e" /* "{ERROR}\n\nArtifact 82." */),
    DATA_COMPGEN(0x0050293c, gArtifactDescERRORArtifact83, "\x7b\x45\x52\x52\x4f\x52\x7d\x0a\x0a\x41\x72\x74\x69\x66\x61\x63\x74\x20\x38\x33\x2e" /* "{ERROR}\n\nArtifact 83." */),
    DATA_COMPGEN(0x00502954, gArtifactDescERRORArtifact84, "\x7b\x45\x52\x52\x4f\x52\x7d\x0a\x0a\x41\x72\x74\x69\x66\x61\x63\x74\x20\x38\x34\x2e" /* "{ERROR}\n\nArtifact 84." */),
    DATA_COMPGEN(0x0050296c, gArtifactDescERRORArtifact85, "\x7b\x45\x52\x52\x4f\x52\x7d\x0a\x0a\x41\x72\x74\x69\x66\x61\x63\x74\x20\x38\x35\x2e" /* "{ERROR}\n\nArtifact 85." */),
    DATA_COMPGEN(0x00502984, gArtifactDescSpellScroll, "\x7b\xd1\xe2\xe8\xf2\xee\xea\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe9\x7d\x0a\x0a\xdd\xf2\xee\xf2\x20\xd1\xe2\xe8\xf2\xee\xea\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe9\x20\xef\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xe2\xe0\xec\x20\xed\xe0\xef\xf0\xe0\xe2\xeb\xff\xf2\xfc\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5\x20\x27\x25\x73\x27\x2e" /* "{Свиток заклинаний}\n\nЭтот Свиток заклинаний позволяет вам направлять заклинание '%s'." */),
    DATA_COMPGEN(0x005029dc, gArtifactDescArmOfTheMartyr, "\x7b\xd0\xf3\xea\xe0\x20\xec\xf3\xf7\xe5\xed\xe8\xea\xe0\x7d\x0a\x0a\xd0\xf3\xea\xe0\x20\xec\xf3\xf7\xe5\xed\xe8\xea\xe0\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xd1\xe8\xeb\xf3\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe9\x20\xe2\xe0\xf8\xe5\xe3\xee\x20\xe3\xe5\xf0\xee\xff\x20\xed\xe0\x20\x33\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x2c\x20\xed\xee\x20\xe4\xe0\xe5\xf2\x20\xf8\xf2\xf0\xe0\xf4\x20\xea\x20\xec\xee\xf0\xe0\xeb\xe8\x20\xe7\xe0\x20\xef\xf0\xe8\xf1\xf3\xf2\xf1\xf2\xe2\xe8\xff\x20\xed\xe5\xe6\xe8\xf2\xe8\x20\xe2\x20\xe0\xf0\xec\xe8\xe8\x2e" /* "{Рука мученика}\n\nРука мученика увеличивает Силу заклинаний вашего героя на 3 единицы, но дает штраф к морали за присутствия нежити в армии." */),
    DATA_COMPGEN(0x00502a68, gArtifactDescBreastplateOfAnduran, "\x7b\xc4\xee\xf1\xef\xe5\xf5\x20\xc0\xed\xe4\xf3\xf0\xe0\xed\xe0\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xc7\xe0\xf9\xe8\xf2\xf3\x20\xed\xe0\x20\x35\x20\xe5\xe4\xe8\xed\xe8\xf6\x2e" /* "{Доспех Андурана}\n\nУвеличивает Защиту на 5 единиц." */),
    DATA_COMPGEN(0x00502a9c, gArtifactDescBroachOfShielding, "\x7b\xc7\xe0\xf9\xe8\xf2\xed\xe0\xff\x20\xe1\xf0\xee\xf8\xfc\x7d\x0a\x0a\xc7\xe0\xf9\xe8\xf2\xed\xe0\xff\x20\xe1\xf0\xee\xf8\xfc\x20\xf1\xed\xe8\xe6\xe0\xe5\xf2\x20\xed\xe0\x20\x35\x30\x20\xef\xf0\xee\xf6\xe5\xed\xf2\xee\xe2\x20\xf3\xf0\xee\xed\x2c\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\xec\xe8\x20\xc0\xf0\xec\xe0\xe3\xe5\xe4\xe4\xee\xed\x20\xe8\x20\xc1\xf3\xf0\xff\x20\xd1\xf2\xe8\xf5\xe8\xe9\x2e\x20\xcf\xf0\xe8\x20\xfd\xf2\xee\xec\x2c\x20\xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xf1\xed\xe8\xe6\xe0\xe5\xf2\x20\xd1\xe8\xeb\xf3\x20\xec\xe0\xe3\xe8\xe8\x20\xed\xe0\x20\x32\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x2e" /* "{Защитная брошь}\n\nЗащитная брошь снижает на 50 процентов урон, наносимый заклинаниями Армагеддон и Буря Стихий. При этом, артефакт снижает Силу магии на 2 единицы." */),
    DATA_COMPGEN(0x00502b40, gArtifactDescBattleGarbOfAnduran, "\x7b\xc1\xee\xe5\xe2\xee\xe5\x20\xee\xe4\xe5\xff\xed\xe8\xe5\x7d\x0a\x0a\xc1\xee\xe5\xe2\xee\xe5\x20\xee\xe4\xe5\xff\xed\xe8\xe5\x20\xc0\xed\xe4\xf3\xf0\xe0\xed\xe0\x20\xf1\xee\xf7\xe5\xf2\xe0\xe5\xf2\x20\xe2\x20\xf1\xe5\xe1\xe5\x20\xf1\xe8\xeb\xf3\x20\xf2\xf0\xe5\xf5\x20\xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2\xee\xe2\x20\xc0\xed\xe4\xf3\xf0\xe0\xed\xe0\x2e\x20\xd2\xe0\xea\xe6\xe5\x2c\x20\xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xef\xee\xe2\xfb\xf8\xe0\xe5\xf2\x20\xe4\xee\x20\xec\xe0\xea\xf1\xe8\xec\xf3\xec\xe0\x20\xf3\xe4\xe0\xf7\xf3\x20\xe8\x20\xec\xee\xf0\xe0\xeb\xfc\x20\xe2\xe0\xf8\xe5\xe9\x20\xe0\xf0\xec\xe8\xe8\x20\xe8\x20\xe4\xe0\xe5\xf2\x20\xe2\xee\xe7\xec\xee\xe6\xed\xee\xf1\xf2\xfc\x20\xed\xe0\xef\xf0\xe0\xe2\xeb\xff\xf2\xfc\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5\x20\xcf\xee\xf0\xf2\xe0\xeb\x20\xe3\xee\xf0\xee\xe4\xe0\x2e" /* "{Боевое одеяние}\n\nБоевое одеяние Андурана сочетает в себе силу трех артефактов Андурана. Также, артефакт повышает до максимума удачу и мораль вашей армии и дает возможность направлять заклинание Портал города." */),
    DATA_COMPGEN(0x00502c14, gArtifactDescCrystalBall, "\x7b\xca\xf0\xe8\xf1\xf2\xe0\xeb\xfc\xed\xfb\xe9\x20\xf8\xe0\xf0\x7d\x0a\x0a\xca\xf0\xe8\xf1\xf2\xe0\xeb\xfc\xed\xfb\xe9\x20\xf8\xe0\xf0\x20\xe4\xe0\xe5\xf2\x20\xe2\xe0\xec\x20\xe1\xee\xeb\xe5\xe5\x20\xe4\xe5\xf2\xe0\xeb\xfc\xed\xf3\xfe\x20\xe8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xfe\x20\xee\x20\xec\xee\xed\xf1\xf2\xf0\xe0\xf5\x2c\x20\xe2\xf0\xe0\xe6\xe5\xf1\xea\xe8\xf5\x20\xe3\xe5\xf0\xee\xff\xf5\x20\xe8\x20\xf2\xee\xec\x2c\x20\xea\xf2\xee\x20\xe7\xe0\xf9\xe8\xf9\xe0\xe5\xf2\x20\xe1\xeb\xe8\xe7\xeb\xe5\xe6\xe0\xf9\xe8\xe5\x20\xee\xf2\x20\xe3\xe5\xf0\xee\xff\x20\xe7\xe0\xec\xea\xe8\x2e" /* "{Кристальный шар}\n\nКристальный шар дает вам более детальную информацию о монстрах, вражеских героях и том, кто защищает близлежащие от героя замки." */),
    DATA_COMPGEN(0x00502ca8, gArtifactDescHeartOfFire, "\x7b\xd1\xe5\xf0\xe4\xf6\xe5\x20\xee\xe3\xed\xff\x7d\x0a\x0a\xd1\xe5\xf0\xe4\xf6\xe5\x20\xee\xe3\xed\xff\x20\xf1\xed\xe8\xe6\xe0\xe5\xf2\x20\xed\xe0\x20\x35\x30\x20\xef\xf0\xee\xf6\xe5\xed\xf2\xee\xe2\x20\xf3\xf0\xee\xed\x2c\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\xf1\xe8\xeb\xe0\xec\xe8\x20\xee\xe3\xed\xff\x2c\x20\xed\xee\x20\xf3\xe4\xe2\xe0\xe8\xe2\xe0\xe5\xf2\x20\xf3\xf0\xee\xed\x2c\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\xe2\xe0\xec\x20\xf5\xee\xeb\xee\xe4\xee\xec\x2e" /* "{Сердце огня}\n\nСердце огня снижает на 50 процентов урон, наносимый силами огня, но удваивает урон, наносимый вам холодом." */),
    DATA_COMPGEN(0x00502d24, gArtifactDescHeartOfIce, "\x7b\xcb\xe5\xe4\xff\xed\xee\xe5\x20\xf1\xe5\xf0\xe4\xf6\xe5\x7d\x0a\x0a\xcb\xe5\xe4\xff\xed\xee\xe5\x20\xf1\xe5\xf0\xe4\xf6\xe5\x20\xf1\xed\xe8\xe6\xe0\xe5\xf2\x20\xed\xe0\x20\x35\x30\x20\xef\xf0\xee\xf6\xe5\xed\xf2\xee\xe2\x20\xf3\xf0\xee\xed\x2c\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\xf1\xe8\xeb\xe0\xec\xe8\x20\xf5\xee\xeb\xee\xe4\xe0\x2c\x20\xed\xee\x20\xf3\xe4\xe2\xe0\xe8\xe2\xe0\xe5\xf2\x20\xf3\xf0\xee\xed\x2c\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\xe2\xe0\xec\x20\xee\xe3\xed\xe5\xec\x2e" /* "{Ледяное сердце}\n\nЛедяное сердце снижает на 50 процентов урон, наносимый силами холода, но удваивает урон, наносимый вам огнем." */),
    DATA_COMPGEN(0x00502da4, gArtifactDescHelmetOfAnduran, "\x7b\xd8\xeb\xe5\xec\x20\xc0\xed\xe4\xf3\xf0\xe0\xed\xe0\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xd1\xe8\xeb\xf3\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe9\x20\xed\xe0\x20\x35\x20\xe5\xe4\xe8\xed\xe8\xf6\x2e" /* "{Шлем Андурана}\n\nУвеличивает Силу заклинаний на 5 единиц." */),
    DATA_COMPGEN(0x00502de0, gArtifactDescHolyHammer, "\x7b\xd1\xe2\xff\xf2\xee\xe9\x20\xec\xee\xeb\xee\xf2\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xc0\xf2\xe0\xea\xf3\x20\xed\xe0\x20\x35\x20\xe5\xe4\xe8\xed\xe8\xf6\x2e" /* "{Святой молот}\n\nУвеличивает Атаку на 5 единиц." */),
    DATA_COMPGEN(0x00502e10, gArtifactDescLegendaryScepter, "\x7b\xcb\xe5\xe3\xe5\xed\xe4\xe0\xf0\xed\xfb\xe9\x20\xf1\xea\xe8\xef\xe5\xf2\xf0\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\x20\x32\x20\xe2\xf1\xe5\x20\xf5\xe0\xf0\xe0\xea\xf2\xe5\xf0\xe8\xf1\xf2\xe8\xea\xe8\x20\xe3\xe5\xf0\xee\xff\x2e" /* "{Легендарный скипетр}\n\nУвеличивает на 2 все характеристики героя." */),
    DATA_COMPGEN(0x00502e54, gArtifactDescMasthead, "\x7b\xcd\xe0\xea\xee\xed\xe5\xf7\xed\xe8\xea\x20\xec\xe0\xf7\xf2\xfb\x7d\x0a\x0a\xc2\x20\xf1\xf0\xe0\xe6\xe5\xed\xe8\xe8\x20\xed\xe0\x20\xec\xee\xf0\xe5\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xf3\xe4\xe0\xf7\xf3\x20\xe8\x20\xec\xee\xf0\xe0\xeb\xfc\x20\xe2\xe0\xf8\xe5\xe9\x20\xe0\xf0\xec\xe8\xe8\x20\xed\xe0\x20\x31\x20\xe5\xe4\xe8\xed\xe8\xf6\xf3\x2e" /* "{Наконечник мачты}\n\nВ сражении на море увеличивает удачу и мораль вашей армии на 1 единицу." */),
    DATA_COMPGEN(0x00502eb0, gArtifactDescSphereOfNegation, "\x7b\xd1\xf4\xe5\xf0\xe0\x20\xe0\xed\xf2\xe8\xec\xe0\xe3\xe8\xe8\x7d\x0a\x0a\xc2\x20\xe1\xee\xfe\x20\xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x20\xed\xe5\x20\xef\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xee\xe1\xe5\xe8\xec\x20\xf1\xf2\xee\xf0\xee\xed\xe0\xec\x20\xed\xe0\xef\xf0\xe0\xe2\xeb\xff\xf2\xfc\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\x2e" /* "{Сфера антимагии}\n\nВ бою артефакт не позволяет обеим сторонам направлять заклинания." */),
    DATA_COMPGEN(0x00502f08, gArtifactDescStaffOfWizardry, "\x7b\xc2\xee\xeb\xf8\xe5\xe1\xed\xfb\xe9\x20\xef\xee\xf1\xee\xf5\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xd1\xe8\xeb\xf3\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe9\x20\xed\xe0\x20\x35\x20\xe5\xe4\xe8\xed\xe8\xf6\x2e" /* "{Волшебный посох}\n\nУвеличивает Силу заклинаний на 5 единиц." */),
    DATA_COMPGEN(0x00502f44, gArtifactDescSwordBreaker, "\x7b\xcc\xe5\xf7\xe5\xeb\xee\xec\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xc7\xe0\xf9\xe8\xf2\xf3\x20\xed\xe0\x20\x34\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x20\xe8\x20\xc0\xf2\xe0\xea\xf3\x20\xed\xe0\x20\x31\x20\xe5\xe4\xe8\xed\xe8\xf6\xf3\x2e" /* "{Мечелом}\n\nУвеличивает Защиту на 4 единицы и Атаку на 1 единицу." */),
    DATA_COMPGEN(0x00502f88, gArtifactDescSwordOfAnduran, "\x7b\xcc\xe5\xf7\x20\xc0\xed\xe4\xf3\xf0\xe0\xed\xe0\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xc0\xf2\xe0\xea\xf3\x20\xed\xe0\x20\x35\x20\xe5\xe4\xe8\xed\xe8\xf6\x2e" /* "{Меч Андурана}\n\nУвеличивает Атаку на 5 единиц." */),
    DATA_COMPGEN(0x00502fb8, gArtifactDescSpadeOfNecromancy, "\x7b\xcb\xee\xef\xe0\xf2\xe0\x20\xec\xee\xe3\xe8\xeb\xfc\xf9\xe8\xea\xe0\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xfd\xf4\xf4\xe5\xea\xf2\xe8\xe2\xed\xee\xf1\xf2\xfc\x20\xe8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xed\xe8\xff\x20\xed\xe0\xe2\xfb\xea\xe0\x20\xed\xe5\xea\xf0\xee\xec\xe0\xed\xf2\xe8\xe8\x2e" /* "{Лопата могильщика}\n\nУвеличивает эффективность использования навыка некромантии." */)};
DATA(0x004fdae4) char* gArtifactEvent[IDX(ARTIFACT_COUNT)] = {
    DATA_COMPGEN(0x00526740, gArtifactEventUltimateBookOfKnowledge, "" /* "" */),
    DATA_COMPGEN(0x00526744, gArtifactEventUltimateSwordOfDominion, "" /* "" */),
    DATA_COMPGEN(0x00526748, gArtifactEventUltimateCloakOfProtection, "" /* "" */),
    DATA_COMPGEN(0x0052674c, gArtifactEventUltimateWandOfMagic, "" /* "" */),
    DATA_COMPGEN(0x00526750, gArtifactEventUltimateShield, "" /* "" */),
    DATA_COMPGEN(0x00526754, gArtifactEventUltimateStaff, "" /* "" */),
    DATA_COMPGEN(0x00526758, gArtifactEventUltimateCrown, "" /* "" */),
    DATA_COMPGEN(0x0052675c, gArtifactEventGoldenGoose, "" /* "" */),
    DATA_COMPGEN(0x0050300c, gArtifactEventArcaneNecklaceOfMagic, "\xc2\xfb\x20\xe2\xfb\xe7\xe2\xee\xeb\xff\xe5\xf2\xe5\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xe8\xf6\xf3\x2c\x20\xe7\xe0\xf2\xee\xf7\xe5\xed\xed\xf3\xfe\x20\xe2\x20\xef\xf0\xee\xea\xeb\xff\xf2\xee\xe9\x20\xe3\xf0\xee\xe1\xed\xe8\xf6\xe5\x2c\x20\xe8\x20\xe2\x20\xed\xe0\xe3\xf0\xe0\xe4\xf3\x20\xee\xed\xe0\x20\xe2\xf0\xf3\xf7\xe0\xe5\xf2\x20\xe2\xe0\xec\x20\xe8\xe7\xfb\xf1\xea\xe0\xed\xed\xee\xe5\x20\xe0\xeb\xec\xe0\xe7\xed\xee\xe5\x20\xee\xe6\xe5\xf0\xe5\xeb\xfc\xe5\x2e" /* "Вы вызволяете волшебницу, заточенную в проклятой гробнице, и в награду она вручает вам изысканное алмазное ожерелье." */),
    DATA_COMPGEN(0x00503084, gArtifactEventCasterSBraceletOfMagic, "\xc8\xe7\xf3\xf7\xe0\xff\x20\xe7\xe0\xe2\xe0\xeb\xfb\x20\xe2\x20\xe7\xe0\xe1\xf0\xee\xf8\xe5\xed\xed\xee\xe9\x20\xf8\xe0\xf5\xf2\xe5\x2c\x20\xe2\xfb\x20\xf1\xef\xe0\xf1\xe0\xe5\xf2\xe5\x20\xe0\xf0\xf2\xe5\xeb\xfc\x20\xe3\xed\xee\xec\xee\xe2\x2d\xf1\xf2\xe0\xf0\xe0\xf2\xe5\xeb\xe5\xe9\x2e\x20\xc2\x20\xe7\xed\xe0\xea\x20\xe1\xeb\xe0\xe3\xee\xe4\xe0\xf0\xed\xee\xf1\xf2\xe8\x20\xe8\xf5\x20\xf1\xf2\xe0\xf0\xf8\xe8\xed\xe0\x20\xe4\xe0\xf0\xe8\xf2\x20\xe2\xe0\xec\x20\xe7\xee\xeb\xee\xf2\xee\xe9\x20\xe1\xf0\xe0\xf1\xeb\xe5\xf2\x2e" /* "Изучая завалы в заброшенной шахте, вы спасаете артель гномов-старателей. В знак благодарности их старшина дарит вам золотой браслет." */),
    DATA_COMPGEN(0x0050310c, gArtifactEventMageSRingOfPower, "\xc2\xfb\x20\xf1\xef\xe5\xf8\xe8\xf2\xe5\x20\xed\xe0\x20\xe7\xe2\xf3\xea\x20\xee\xf2\xf7\xe0\xff\xed\xed\xee\xe3\xee\x20\xe2\xee\xef\xeb\xff\x20\xe1\xee\xeb\xe8\x20\xe8\x20\xe2\xe8\xe4\xe8\xf2\xe5\x20\xea\xe5\xed\xf2\xe0\xe2\xf0\xe0\x2c\x20\xef\xee\xef\xe0\xe2\xf8\xe5\xe3\xee\x20\xe2\x20\xe7\xe0\xef\xe0\xe4\xed\xfe\x2e\x20\xc2\xfb\x20\xef\xee\xec\xee\xe3\xe0\xe5\xf2\xe5\x20\xe5\xec\xf3\x20\xee\xf1\xe2\xee\xe1\xee\xe4\xe8\xf2\xfc\xf1\xff\x2c\x20\xe8\x20\xee\xed\x20\xe2\xf0\xf3\xf7\xe0\xe5\xf2\x20\xe2\xe0\xec\x20\xea\xee\xe6\xe0\xed\xfb\xe9\x20\xec\xe5\xf8\xee\xf7\xe5\xea\x2e\x20\xc7\xe0\xe3\xeb\xff\xed\xf3\xe2\x20\xe2\xed\xf3\xf2\xf0\xfc\x2c\x20\xe2\xfb\x20\xe2\xe8\xe4\xe8\xf2\xe5\x20\xee\xf1\xeb\xe5\xef\xe8\xf2\xe5\xeb\xfc\xed\xee\xe5\x20\xe1\xf0\xe8\xeb\xeb\xe8\xe0\xed\xf2\xee\xe2\xee\xe5\x20\xea\xee\xeb\xfc\xf6\xee\x2e" /* "Вы спешите на звук отчаянного вопля боли и видите кентавра, попавшего в западню. Вы помогаете ему освободиться, и он вручает вам кожаный мешочек. Заглянув внутрь, вы видите ослепительное бриллиантовое кольцо." */),
    DATA_COMPGEN(0x005031e0, gArtifactEventWitchSBroachOfMagic, "\xd0\xff\xe4\xee\xec\x20\xf1\x20\xee\xf1\xf2\xe0\xed\xea\xe0\xec\xe8\x20\xf1\xee\xe6\xe6\xe5\xed\xed\xee\xe9\x20\xea\xee\xeb\xe4\xf3\xed\xfc\xe8\x20\xeb\xe5\xe6\xe8\xf2\x20\xe8\xe7\xff\xf9\xed\xe0\xff\x20\xe1\xf0\xee\xf8\xfc\x20\xef\xf0\xe5\xea\xf0\xe0\xf1\xed\xee\xe9\x20\xf0\xe0\xe1\xee\xf2\xfb\x2e\x20\xce\xf1\xf2\xee\xf0\xee\xe6\xed\xee\x20\xef\xf0\xe8\xe1\xeb\xe8\xe7\xe8\xe2\xf8\xe8\xf1\xfc\x20\xea\x20\xee\xe1\xf3\xe3\xeb\xe5\xed\xed\xee\xec\xf3\x20\xf2\xf0\xf3\xef\xf3\x2c\x20\xe2\xfb\x20\xe7\xe0\xe1\xe8\xf0\xe0\xe5\xf2\xe5\x20\xe1\xf0\xee\xf8\xfc\x20\xf1\xe5\xe1\xe5\x2e" /* "Рядом с останками сожженной колдуньи лежит изящная брошь прекрасной работы. Осторожно приблизившись к обугленному трупу, вы забираете брошь себе." */),
    DATA_COMPGEN(0x00503274, gArtifactEventMedalOfValor, "\xc2\x20\xed\xe0\xe3\xf0\xe0\xe4\xf3\x20\xe7\xe0\x20\xf1\xef\xe0\xf1\xe5\xed\xe8\xe5\x20\xef\xf0\xe5\xea\xf0\xe0\xf1\xed\xee\xe9\x20\xe4\xe5\xe2\xfb\x20\xee\xf2\x20\xef\xee\xf1\xff\xe3\xe0\xf2\xe5\xeb\xfc\xf1\xf2\xe2\x20\xed\xe5\xed\xe0\xe2\xe8\xf1\xf2\xed\xee\xe3\xee\x20\xe1\xe0\xf0\xee\xed\xe0\x20\xea\xee\xf0\xee\xeb\xe5\xe2\xf1\xea\xe8\xe9\x20\xe3\xe5\xf0\xee\xeb\xfc\xe4\x20\xe2\xf0\xf3\xf7\xe0\xe5\xf2\x20\xe2\xe0\xec\x20\xcc\xe5\xe4\xe0\xeb\xfc\x20\xee\xf2\xe2\xe0\xe3\xe8\x2e" /* "В награду за спасение прекрасной девы от посягательств ненавистного барона королевский герольд вручает вам Медаль отваги." */),
    DATA_COMPGEN(0x005032f0, gArtifactEventMedalOfCourage, "\xc2\xfb\x20\xf1\xef\xe0\xf1\xe0\xe5\xf2\xe5\x20\xec\xe0\xeb\xe5\xed\xfc\xea\xee\xe3\xee\x20\xec\xe0\xeb\xfc\xf7\xe8\xea\xe0\x20\xee\xf2\x20\xf1\xf2\xe0\xe8\x20\xea\xf0\xee\xe2\xee\xe6\xe0\xe4\xed\xfb\xf5\x20\xe2\xee\xeb\xea\xee\xe2\x20\xe8\x20\xef\xf0\xee\xe2\xee\xe6\xe0\xe5\xf2\xe5\x20\xe2\x20\xe8\xec\xe5\xed\xe8\xe5\x20\xf0\xee\xe4\xe8\xf2\xe5\xeb\xe5\xe9\x2e\x20\xd1\xf7\xe0\xf1\xf2\xeb\xe8\xe2\xfb\xe9\x20\xee\xf2\xe5\xf6\x20\xed\xe0\xe3\xf0\xe0\xe6\xe4\xe0\xe5\xf2\x20\xe2\xe0\xf1\x20\xcc\xe5\xe4\xe0\xeb\xfc\xfe\x20\xec\xf3\xe6\xe5\xf1\xf2\xe2\xe0\x2e" /* "Вы спасаете маленького мальчика от стаи кровожадных волков и провожаете в имение родителей. Счастливый отец награждает вас Медалью мужества." */),
    DATA_COMPGEN(0x00503380, gArtifactEventMedalOfHonor, "\xc2\xfb\x20\xe2\xfb\xf0\xfb\xe2\xe0\xe5\xf2\xe5\x20\xef\xf0\xe8\xed\xf6\xe5\xf1\xf1\xf3\x20\xf1\xee\xf1\xe5\xe4\xed\xe5\xe3\xee\x20\xea\xee\xf0\xee\xeb\xe5\xe2\xf1\xf2\xe2\xe0\x20\xe8\xe7\x20\xec\xe5\xf0\xe7\xea\xe8\xf5\x20\xeb\xe0\xef\x20\xef\xf0\xe5\xe7\xf0\xe5\xed\xed\xfb\xf5\x20\xf0\xe0\xe1\xee\xf2\xee\xf0\xe3\xee\xe2\xf6\xe5\xe2\x20\xe8\x20\xe2\x20\xed\xe0\xe3\xf0\xe0\xe4\xf3\x20\xe7\xe0\x20\xef\xee\xe4\xe2\xe8\xe3\x20\xef\xee\xeb\xf3\xf7\xe0\xe5\xf2\xe5\x20\xcc\xe5\xe4\xe0\xeb\xfc\x20\xe4\xee\xe1\xeb\xe5\xf1\xf2\xe8\x2e" /* "Вы вырываете принцессу соседнего королевства из мерзких лап презренных работорговцев и в награду за подвиг получаете Медаль доблести." */),
    DATA_COMPGEN(0x00503408, gArtifactEventMedalOfDistinction, "\xc2\xfb\x20\xe8\xe7\xe1\xe0\xe2\xeb\xff\xe5\xf2\xe5\x20\xee\xea\xf0\xf3\xe3\xf3\x20\xee\xf2\x20\xf3\xe6\xe0\xf1\xed\xee\xe3\xee\x20\xec\xe8\xed\xee\xf2\xe0\xe2\xf0\xe0\x2c\x20\xe4\xee\xe1\xfb\xf7\xe5\xe9\x20\xea\xee\xf2\xee\xf0\xee\xec\xf3\x20\xf1\xeb\xf3\xe6\xe8\xeb\xe8\x20\xe1\xeb\xe0\xe3\xee\xf0\xee\xe4\xed\xfb\xe5\x20\xf0\xfb\xf6\xe0\xf0\xe8\x2c\x20\xe8\x20\xf1\xf2\xe0\xed\xee\xe2\xe8\xf2\xe5\xf1\xfc\x20\xea\xe0\xe2\xe0\xeb\xe5\xf0\xee\xec\x20\xcc\xe5\xe4\xe0\xeb\xe8\x20\xef\xee\xf7\xe5\xf2\xe0\x2e" /* "Вы избавляете округу от ужасного минотавра, добычей которому служили благородные рыцари, и становитесь кавалером Медали почета." */),
    DATA_COMPGEN(0x00503488, gArtifactEventFizbinOfMisfortune, "\xcd\xe0\x20\xee\xe1\xee\xf7\xe8\xed\xe5\x20\xef\xf3\xf1\xf2\xfb\xed\xed\xee\xe9\x20\xe4\xee\xf0\xee\xe3\xe8\x20\xe2\xfb\x20\xed\xe0\xf5\xee\xe4\xe8\xf2\xe5\x20\xec\xe5\xe4\xe0\xeb\xfc\x2e\x20\xc2\xfb\x20\xef\xee\xe4\xee\xe1\xf0\xe0\xeb\xe8\x20\xe5\xe5\x20\xe8\x20\xee\xe1\xed\xe0\xf0\xf3\xe6\xe8\xeb\xe8\x2c\x20\xf7\xf2\xee\x20\xf1\xf2\xe0\xeb\xe8\x20\xed\xe5\xf1\xf7\xe0\xf1\xf2\xed\xfb\xec\x20\xee\xe1\xeb\xe0\xe4\xe0\xf2\xe5\xeb\xe5\xec\x20\xd1\xe8\xec\xe2\xee\xeb\xe0\x20\xed\xe5\xf3\xe4\xe0\xf7\xe8\x2c\x20\xea\xee\xf2\xee\xf0\xfb\xe9\x20\xef\xee\xed\xe8\xe6\xe0\xe5\xf2\x20\xe1\xee\xe5\xe2\xee\xe9\x20\xe4\xf3\xf5\x20\xe2\xe0\xf8\xe5\xe9\x20\xe0\xf0\xec\xe8\xe8\x2e" /* "На обочине пустынной дороги вы находите медаль. Вы подобрали ее и обнаружили, что стали несчастным обладателем Символа неудачи, который понижает боевой дух вашей армии." */),
    DATA_COMPGEN(0x00503534, gArtifactEventThunderMaceOfDominion, "\xc2\xee\x20\xe2\xf0\xe5\xec\xff\x20\xe6\xf3\xf2\xea\xee\xe9\x20\xe3\xf0\xee\xe7\xfb\x20\xec\xee\xeb\xed\xe8\xff\x20\xe1\xfc\xe5\xf2\x20\xe2\x20\xe4\xe5\xf0\xe5\xe2\xee\x2c\x20\xf0\xe0\xe7\xed\xee\xf1\xff\x20\xe5\xe3\xee\x20\xed\xe0\x20\xec\xe5\xeb\xea\xe8\xe5\x20\xf9\xe5\xef\xea\xe8\x2e\x20\xd1\xf0\xe5\xe4\xe8\x20\xee\xe1\xeb\xee\xec\xea\xee\xe2\x20\xe2\xfb\x20\xee\xe1\xed\xe0\xf0\xf3\xe6\xe8\xe2\xe0\xe5\xf2\xe5\x20\xf2\xe0\xe8\xed\xf1\xf2\xe2\xe5\xed\xed\xf3\xfe\x20\xef\xe0\xeb\xe8\xf6\xf3\x2e" /* "Во время жуткой грозы молния бьет в дерево, разнося его на мелкие щепки. Среди обломков вы обнаруживаете таинственную палицу." */),
    DATA_COMPGEN(0x005035b4, gArtifactEventArmoredGauntletsOfProtection, "\xc2\xfb\x20\xef\xee\xe2\xf1\xf2\xf0\xe5\xf7\xe0\xeb\xe8\x20\xef\xe5\xf7\xe0\xeb\xfc\xed\xee\x20\xe8\xe7\xe2\xe5\xf1\xf2\xed\xee\xe3\xee\x20\xd7\xe5\xf0\xed\xee\xe3\xee\x20\xd0\xfb\xf6\xe0\xf0\xff\x21\x20\xc2\xe0\xf8\x20\xef\xee\xe5\xe4\xe8\xed\xee\xea\x20\xe7\xe0\xea\xe0\xed\xf7\xe8\xe2\xe0\xe5\xf2\xf1\xff\x20\xe2\xed\xe8\xf7\xfc\xfe\x2c\x20\xe8\x20\xf0\xfb\xf6\xe0\xf0\xfc\x20\xe2\x20\xe7\xed\xe0\xea\x20\xf3\xe2\xe0\xe6\xe5\xed\xe8\xff\x20\xe4\xe0\xf0\xe8\xf2\x20\xe2\xe0\xec\x20\xef\xe0\xf0\xf3\x20\xeb\xe0\xf2\xed\xfb\xf5\x20\xef\xe5\xf0\xf7\xe0\xf2\xee\xea\x2e" /* "Вы повстречали печально известного Черного Рыцаря! Ваш поединок заканчивается вничью, и рыцарь в знак уважения дарит вам пару латных перчаток." */),
    DATA_COMPGEN(0x00503644, gArtifactEventDefenderHelmOfProtection, "\xca\xf0\xe0\xe5\xec\x20\xe3\xeb\xe0\xe7\xe0\x20\xe2\xfb\x20\xe7\xe0\xec\xe5\xf7\xe0\xe5\xf2\xe5\x20\xe7\xee\xeb\xee\xf2\xe8\xf1\xf2\xfb\xe9\x20\xe1\xeb\xe5\xf1\xea\x20\xf1\xf0\xe5\xe4\xe8\x20\xef\xfb\xf8\xed\xee\xe9\x20\xe7\xe5\xeb\xe5\xed\xe8\x2e\x20\xcf\xf0\xe8\xe3\xeb\xff\xe4\xe5\xe2\xf8\xe8\xf1\xfc\x20\xe2\xed\xe8\xec\xe0\xf2\xe5\xeb\xfc\xed\xe5\xe5\x2c\x20\xe2\xfb\x20\xed\xe0\xf5\xee\xe4\xe8\xf2\xe5\x20\xef\xee\xe4\x20\xea\xf3\xf1\xf2\xe0\xec\xe8\x20\xe2\xe5\xeb\xe8\xea\xee\xeb\xe5\xef\xed\xfb\xe9\x20\xe7\xee\xeb\xee\xf2\xee\xe9\x20\xf8\xeb\xe5\xec\x2e" /* "Краем глаза вы замечаете золотистый блеск среди пышной зелени. Приглядевшись внимательнее, вы находите под кустами великолепный золотой шлем." */),
    DATA_COMPGEN(0x005036d4, gArtifactEventGiantFlailOfDominion, "\xcd\xe5\xf3\xea\xeb\xfe\xe6\xe8\xe9\x20\xe3\xe8\xe3\xe0\xed\xf2\x20\xed\xe0\xed\xe5\xf1\x20\xf1\xe5\xe1\xe5\x20\xf1\xec\xe5\xf0\xf2\xe5\xeb\xfc\xed\xf3\xfe\x20\xf0\xe0\xed\xf3\x20\xf1\xee\xe1\xf1\xf2\xe2\xe5\xed\xed\xfb\xec\x20\xe1\xee\xe5\xe2\xfb\xec\x20\xf6\xe5\xef\xee\xec\x2e\x20\xc2\xfb\x20\xef\xf0\xe5\xea\xf0\xe0\xf1\xed\xee\x20\xe2\xeb\xe0\xe4\xe5\xe5\xf2\xe5\x20\xfd\xf2\xe8\xec\x20\xee\xf0\xf3\xe6\xe8\xe5\xec\x20\xe8\x20\xf1\x20\xf3\xe2\xe5\xf0\xe5\xed\xed\xee\xf1\xf2\xfc\xfe\x20\xe2\xfb\xed\xe8\xec\xe0\xe5\xf2\xe5\x20\xf6\xe5\xef\x20\xe8\xe7\x20\xec\xe5\xf0\xf2\xe2\xfb\xf5\x20\xf0\xf3\xea\x20\xe3\xe8\xe3\xe0\xed\xf2\xe0\x2e" /* "Неуклюжий гигант нанес себе смертельную рану собственным боевым цепом. Вы прекрасно владеете этим оружием и с уверенностью вынимаете цеп из мертвых рук гиганта." */),
    DATA_COMPGEN(0x00503778, gArtifactEventBallistaOfQuickness, "\xcf\xf0\xee\xe1\xe8\xf0\xe0\xff\xf1\xfc\x20\xf7\xe5\xf0\xe5\xe7\x20\xf0\xe0\xe7\xe2\xe0\xeb\xe8\xed\xfb\x20\xe4\xf0\xe5\xe2\xed\xe5\xe9\x20\xea\xf0\xe5\xef\xee\xf1\xf2\xe8\x2c\x20\xe2\xfb\x20\xed\xe0\xf5\xee\xe4\xe8\xf2\xe5\x20\xee\xf0\xf3\xe4\xe8\xe5\x2c\x20\xea\xee\xf2\xee\xf0\xee\xe5\x20\xef\xf0\xe5\xe2\xf0\xe0\xf2\xe8\xeb\xee\x20\xe5\xe5\x20\xe2\x20\xf0\xf3\xe8\xed\xfb\x2c\x20\xf3\xe4\xe8\xe2\xe8\xf2\xe5\xeb\xfc\xed\xf3\xfe\x20\xe1\xe0\xeb\xeb\xe8\xf1\xf2\xf3\x20\xe7\xe0\xec\xfb\xf1\xeb\xee\xe2\xe0\xf2\xee\xe9\x20\xea\xee\xed\xf1\xf2\xf0\xf3\xea\xf6\xe8\xe8\x2e" /* "Пробираясь через развалины древней крепости, вы находите орудие, которое превратило ее в руины, удивительную баллисту замысловатой конструкции." */),
    DATA_COMPGEN(0x00503808, gArtifactEventStealthShieldOfProtection, "\xc2\x20\xf0\xf3\xea\xe0\xf5\x20\xf3\x20\xea\xe0\xec\xe5\xed\xed\xee\xe9\x20\xf1\xf2\xe0\xf2\xf3\xe8\x20\xe2\xee\xe8\xed\xe0\x20\x2d\x20\xe2\xe5\xeb\xe8\xea\xee\xeb\xe5\xef\xed\xfb\xe9\x20\xf1\xe5\xf0\xe5\xe1\xf0\xff\xed\xfb\xe9\x20\xf9\xe8\xf2\x2e\x20\xca\xe0\xea\x20\xf2\xee\xeb\xfc\xea\xee\x20\xe2\xfb\x20\xe7\xe0\xe1\xe8\xf0\xe0\xe5\xf2\xe5\x20\xf9\xe8\xf2\x20\xf1\xe5\xe1\xe5\x2c\x20\xf1\xf2\xe0\xf2\xf3\xff\x20\xf0\xe0\xf1\xf1\xfb\xef\xe0\xe5\xf2\xf1\xff\x20\xe2\x20\xef\xf0\xe0\xf5\x2e" /* "В руках у каменной статуи воина - великолепный серебряный щит. Как только вы забираете щит себе, статуя рассыпается в прах." */),
    DATA_COMPGEN(0x00503884, gArtifactEventDragonSwordOfDominion, "\xc2\xfb\x20\xef\xf0\xee\xe1\xe8\xf0\xe0\xe5\xf2\xe5\xf1\xfc\x20\xf3\xe7\xea\xee\xe9\x20\xf2\xf0\xee\xef\xee\xe9\x2c\x20\xea\xe0\xea\x20\xe2\xe4\xf0\xf3\xe3\x20\xe1\xeb\xe8\xe6\xe0\xe9\xf8\xe8\xe9\x20\xea\xf3\xf1\xf2\x20\xe7\xe0\xe3\xee\xf0\xe0\xe5\xf2\xf1\xff\x20\xff\xf0\xea\xe8\xec\x20\xef\xeb\xe0\xec\xe5\xed\xe5\xec\x2e\x20\xc2\x20\xee\xe3\xed\xe5\xed\xed\xee\xec\x20\xf1\xec\xe5\xf0\xf7\xe5\x20\xef\xee\xff\xe2\xeb\xff\xe5\xf2\xf1\xff\x20\xef\xf0\xe5\xea\xf0\xe0\xf1\xed\xe0\xff\x20\xe4\xe0\xec\xe0\x2c\x20\xea\xee\xf2\xee\xf0\xe0\xff\x20\xef\xf0\xee\xf2\xff\xe3\xe8\xe2\xe0\xe5\xf2\x20\xe2\xe0\xec\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xfb\xe9\x20\xec\xe5\xf7\x2e" /* "Вы пробираетесь узкой тропой, как вдруг ближайший куст загорается ярким пламенем. В огненном смерче появляется прекрасная дама, которая протягивает вам волшебный меч." */),
    DATA_COMPGEN(0x0050392c, gArtifactEventPowerAxeOfDominion, "\xc2\xfb\x20\xe2\xe8\xe4\xe8\xf2\xe5\x20\xf1\xe5\xf0\xe5\xe1\xf0\xff\xed\xfb\xe9\x20\xf2\xee\xef\xee\xf0\x2c\x20\xe2\xee\xe3\xed\xe0\xed\xed\xfb\xe9\x20\xe2\x20\xe7\xe5\xec\xeb\xfe\x20\xef\xee\x20\xf1\xe0\xec\xf3\xfe\x20\xf0\xf3\xea\xee\xff\xf2\xfc\x2e\x20\xc2\xe0\xf8\xe8\x20\xe2\xee\xe8\xed\xfb\x20\xef\xfb\xf2\xe0\xfe\xf2\xf1\xff\x20\xe2\xfb\xe4\xe5\xf0\xed\xf3\xf2\xfc\x20\xe5\xe3\xee\x2c\x20\xed\xee\x20\xf3\xf1\xe8\xeb\xe8\xff\x20\xe8\xf5\x20\xf2\xf9\xe5\xf2\xed\xfb\x2e\x20\xc2\xe0\xec\x20\xe6\xe5\x20\xf5\xe2\xe0\xf2\xe8\xeb\xee\x20\xee\xe4\xed\xee\xe3\xee\x20\xf3\xf1\xe8\xeb\xe8\xff\x20\xe8\x20\xf2\xee\xef\xee\xf0\x20\xf3\x20\xe2\xe0\xf1\x20\xe2\x20\xf0\xf3\xea\xe0\xf5\x21" /* "Вы видите серебряный топор, вогнанный в землю по самую рукоять. Ваши воины пытаются выдернуть его, но усилия их тщетны. Вам же хватило одного усилия и топор у вас в руках!" */),
    DATA_COMPGEN(0x005039d8, gArtifactEventDivineBreastplateOfProtection, "\xd8\xe0\xe9\xea\xe0\x20\xf0\xe0\xe7\xe1\xee\xe9\xed\xe8\xea\xee\xe2\x20\xee\xe1\xfb\xf1\xea\xe8\xe2\xe0\xe5\xf2\x20\xf2\xe5\xeb\xe0\x20\xec\xe5\xf0\xf2\xe2\xfb\xf5\x20\xe2\xee\xe8\xed\xee\xe2\x2e\x20\xc2\xfb\x20\xf0\xe0\xe7\xe3\xee\xed\xff\xe5\xf2\xe5\x20\xec\xe0\xf0\xee\xe4\xe5\xf0\xee\xe2\x20\xe8\x20\xe2\xe4\xf0\xf3\xe3\x20\xe7\xe0\xec\xe5\xf7\xe0\xe5\xf2\xe5\x2c\x20\xf7\xf2\xee\x20\xe2\x20\xf1\xef\xe5\xf8\xea\xe5\x20\xee\xed\xe8\x20\xef\xee\xf2\xe5\xf0\xff\xeb\xe8\x20\xe2\xe5\xeb\xe8\xea\xee\xeb\xe5\xef\xed\xfb\xe9\x20\xe4\xee\xf1\xef\xe5\xf5\x2e" /* "Шайка разбойников обыскивает тела мертвых воинов. Вы разгоняете мародеров и вдруг замечаете, что в спешке они потеряли великолепный доспех." */),
    DATA_COMPGEN(0x00503a64, gArtifactEventMinorScrollOfKnowledge, "\xcf\xe5\xf0\xe5\xe4\x20\xe2\xe0\xec\xe8\x20\xe2\xee\xe7\xed\xe8\xea\xe0\xe5\xf2\x20\xef\xe0\xf0\xff\xf9\xe8\xe9\x20\xe2\x20\xe2\xee\xe7\xe4\xf3\xf5\xe5\x20\xf1\xf2\xe5\xea\xeb\xff\xed\xed\xfb\xe9\x20\xeb\xe0\xf0\xe5\xf6\x20\xf1\xee\x20\xf1\xe2\xe8\xf2\xea\xee\xec\x20\xe2\xed\xf3\xf2\xf0\xe8\x2c\x20\xeb\xe5\xe6\xe0\xf9\xe5\xec\x20\xed\xe0\x20\xef\xee\xe4\xf3\xf8\xea\xe5\x20\xe8\xe7\x20\xef\xf3\xf0\xef\xf3\xf0\xed\xee\xe3\xee\x20\xe1\xe0\xf0\xf5\xe0\xf2\xe0\x2e\x20\xce\xf2\x20\xef\xf0\xe8\xea\xee\xf1\xed\xee\xe2\xe5\xed\xe8\xff\x2c\x20\xea\xf0\xfb\xf8\xea\xe0\x20\xeb\xe0\xf0\xf6\xe0\x20\xee\xf2\xea\xf0\xfb\xe2\xe0\xe5\xf2\xf1\xff\x2c\x20\xe8\x20\xf1\xe2\xe8\xf2\xee\xea\x20\xee\xea\xe0\xe7\xfb\xe2\xe0\xe5\xf2\xf1\xff\x20\xf3\x20\xe2\xe0\xf1\x20\xe2\x20\xf0\xf3\xea\xe0\xf5\x2e" /* "Перед вами возникает парящий в воздухе стеклянный ларец со свитком внутри, лежащем на подушке из пурпурного бархата. От прикосновения, крышка ларца открывается, и свиток оказывается у вас в руках." */),
    DATA_COMPGEN(0x00503b2c, gArtifactEventMajorScrollOfKnowledge, "\xc2\xfb\x20\xed\xe0\xe2\xe5\xf9\xe0\xe5\xf2\xe5\x20\xec\xe5\xf1\xf2\xed\xee\xe3\xee\x20\xec\xf3\xe4\xf0\xe5\xf6\xe0\x20\xe8\x20\xf0\xe0\xf1\xf1\xea\xe0\xe7\xfb\xe2\xe0\xe5\xf2\xe5\x20\xee\x20\xf6\xe5\xeb\xe8\x20\xe2\xe0\xf8\xe5\xe3\xee\x20\xef\xf3\xf2\xe5\xf8\xe5\xf1\xf2\xe2\xe8\xff\x2e\x20\xce\xed\x20\xe4\xee\xf1\xf2\xe0\xe5\xf2\x20\xe8\xe7\x20\xec\xe5\xf8\xea\xe0\x20\xef\xee\xe6\xe5\xeb\xf2\xe5\xe2\xf8\xe8\xe9\x20\xf1\xe2\xe8\xf2\xee\xea\x20\xe8\x20\xef\xe5\xf0\xe5\xe4\xe0\xe5\xf2\x20\xe5\xe3\xee\x20\xe2\xe0\xec\x2e" /* "Вы навещаете местного мудреца и рассказываете о цели вашего путешествия. Он достает из мешка пожелтевший свиток и передает его вам." */),
    DATA_COMPGEN(0x00503bb0, gArtifactEventSuperiorScrollOfKnowledge, "\xc2\xfb\x20\xf1\xf2\xee\xe8\xf2\xe5\x20\xef\xe5\xf0\xe5\xe4\x20\xee\xf1\xf2\xe0\xed\xea\xe0\xec\xe8\x20\xe4\xe0\xe2\xed\xee\x20\xf3\xec\xe5\xf0\xf8\xe5\xe9\x20\xe6\xf0\xe8\xf6\xfb\x20\xe4\xf0\xf3\xe8\xe4\xee\xe2\x2e\x20\xcf\xee\xe6\xe5\xeb\xf2\xe5\xe2\xf8\xe8\xe5\x20\xee\xf2\x20\xe2\xf0\xe5\xec\xe5\xed\xe8\x20\xea\xee\xf1\xf2\xe8\x20\xef\xf0\xee\xe3\xeb\xff\xe4\xfb\xe2\xe0\xfe\xf2\x20\xf7\xe5\xf0\xe5\xe7\x20\xef\xf0\xee\xf0\xe5\xf5\xe8\x20\xe8\xf1\xf2\xeb\xe5\xe2\xf8\xe5\xe3\xee\x20\xee\xe4\xe5\xff\xed\xe8\xff\x2e\x20\xcf\xee\xf8\xe5\xe2\xe5\xeb\xe8\xe2\x20\xe3\xf0\xf3\xe4\xf3\x20\xe2\xe5\xf2\xee\xf8\xe8\x2c\x20\xe2\xfb\x20\xed\xe0\xf5\xee\xe4\xe8\xf2\xe5\x20\xe4\xf0\xe5\xe2\xed\xe8\xe9\x20\xf1\xe2\xe8\xf2\xee\xea\x2e" /* "Вы стоите перед останками давно умершей жрицы друидов. Пожелтевшие от времени кости проглядывают через прорехи истлевшего одеяния. Пошевелив груду ветоши, вы находите древний свиток." */),
    DATA_COMPGEN(0x00503c68, gArtifactEventForemostScrollOfKnowledge, "\xc3\xf0\xf3\xe4\xe0\x20\xef\xee\xe6\xe5\xeb\xf2\xe5\xe2\xf8\xe8\xf5\x20\xea\xee\xf1\xf2\xe5\xe9\x20\xe8\x20\xee\xe1\xf0\xfb\xe2\xea\xe8\x20\xe8\xf1\xf2\xeb\xe5\xe2\xf8\xe5\xe9\x20\xec\xe0\xf2\xe5\xf0\xe8\xe8\x20\x2d\x20\xe2\xee\xf2\x20\xe2\xf1\xe5\x2c\x20\xf7\xf2\xee\x20\xee\xf1\xf2\xe0\xeb\xee\xf1\xfc\x20\xee\xf2\x20\xe6\xf0\xe8\xf6\xfb\x20\xe4\xf0\xf3\xe8\xe4\xee\xe2\x2e\x20\xd1\xf0\xe5\xe4\xe8\x20\xfd\xf2\xe8\xf5\x20\xee\xf1\xf2\xe0\xed\xea\xee\xe2\x20\xe2\xfb\x20\xe7\xe0\xec\xe5\xf7\xe0\xe5\xf2\xe5\x20\xf2\xe0\xe8\xed\xf1\xf2\xe2\xe5\xed\xed\xfb\xe9\x20\xf1\xe2\xe8\xf2\xee\xea\x2e" /* "Груда пожелтевших костей и обрывки истлевшей материи - вот все, что осталось от жрицы друидов. Среди этих останков вы замечаете таинственный свиток." */),
    DATA_COMPGEN(0x00503d00, gArtifactEventEndlessSackOfGold, "\xcc\xe0\xeb\xe5\xed\xfc\xea\xe8\xe9\x20\xeb\xe5\xef\xf0\xe5\xea\xee\xed\x20\xef\xf0\xe8\xf2\xe0\xed\xf6\xee\xe2\xfb\xe2\xe0\xe5\xf2\x20\xf3\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xee\xe3\xee\x20\xec\xe5\xf8\xea\xe0\x2e\x20\xc7\xe0\xe2\xe8\xe4\xe5\xe2\x20\xe2\xe0\xf1\x2c\x20\xee\xed\x20\xe7\xe0\xec\xe8\xf0\xe0\xe5\xf2\x20\xed\xe0\x20\xec\xe5\xf1\xf2\xe5\x2c\x20\xe7\xe0\xf2\xe5\xec\x20\xe8\xe7\xe4\xe0\xe5\xf2\x20\xe2\xee\xe7\xec\xf3\xf9\xe5\xed\xed\xfb\xe9\x20\xe2\xee\xe7\xe3\xeb\xe0\xf1\x2c\x20\xf2\xee\xef\xe0\xe5\xf2\x20\xed\xee\xe6\xea\xee\xe9\x20\xe8\x20\xf0\xe0\xf1\xf2\xe2\xee\xf0\xff\xe5\xf2\xf1\xff\x20\xe2\x20\xe2\xee\xe7\xe4\xf3\xf5\xe5\x2e\x20\xc2\xfb\x20\xe7\xe0\xe1\xe8\xf0\xe0\xe5\xf2\xe5\x20\xec\xe5\xf8\xee\xea\x20\xf1\xe5\xe1\xe5\x2e" /* "Маленький лепрекон пританцовывает у волшебного мешка. Завидев вас, он замирает на месте, затем издает возмущенный возглас, топает ножкой и растворяется в воздухе. Вы забираете мешок себе." */),
    DATA_COMPGEN(0x00503dbc, gArtifactEventEndlessBagOfGold, "\xc1\xeb\xe0\xe3\xee\xf0\xee\xe4\xed\xe0\xff\x20\xef\xf3\xf2\xe5\xf8\xe5\xf1\xf2\xe2\xe5\xed\xed\xe8\xf6\xe0\x2c\x20\xee\xf2\xe1\xe8\xe2\xf8\xe0\xff\xf1\xff\x20\xee\xf2\x20\xf1\xef\xf3\xf2\xed\xe8\xea\xee\xe2\x2c\x20\xef\xf0\xee\xf1\xe8\xf2\x20\xe2\xe0\xf1\x20\xee\x20\xef\xee\xec\xee\xf9\xe8\x2e\x20\xcf\xf0\xee\xe2\xee\xe4\xe8\xe2\x20\xe5\xe5\x20\xe4\xee\x20\xe4\xee\xec\xe0\x2c\x20\xe2\xfb\x20\xef\xee\xeb\xf3\xf7\xe0\xe5\xf2\xe5\x20\xe2\x20\xed\xe0\xe3\xf0\xe0\xe4\xf3\x20\xf1\xf3\xec\xf3\x2c\x20\xef\xee\xeb\xed\xf3\xfe\x20\xe7\xee\xeb\xee\xf2\xe0\x2e" /* "Благородная путешественница, отбившаяся от спутников, просит вас о помощи. Проводив ее до дома, вы получаете в награду суму, полную золота." */),
    DATA_COMPGEN(0x00503e48, gArtifactEventEndlessPurseOfGold, "\xce\xe4\xed\xe0\xe6\xe4\xfb\x20\xe2\xe0\xec\x20\xe2\x20\xf0\xf3\xea\xe8\x20\xef\xee\xef\xe0\xe4\xe0\xe5\xf2\x20\xed\xe0\xef\xee\xeb\xed\xe5\xed\xed\xfb\xe9\x20\xe7\xee\xeb\xee\xf2\xee\xec\x20\xea\xee\xe6\xe0\xed\xfb\xe9\x20\xea\xee\xf8\xe5\xeb\xfc\x2c\x20\xef\xf0\xe8\xed\xe0\xe4\xeb\xe5\xe6\xe0\xe2\xf8\xe8\xe9\x20\xe2\xe5\xeb\xe8\xea\xee\xec\xf3\x20\xea\xee\xf0\xee\xeb\xfe\x2c\x20\xea\xee\xf2\xee\xf0\xfb\xe9\x20\xf3\xec\xe5\xeb\x20\xef\xf0\xe5\xe2\xf0\xe0\xf9\xe0\xf2\xfc\x20\xeb\xfe\xe1\xee\xe9\x20\xef\xf0\xe5\xe4\xec\xe5\xf2\x20\xe2\x20\xe7\xee\xeb\xee\xf2\xee\x2e" /* "Однажды вам в руки попадает наполненный золотом кожаный кошель, принадлежавший великому королю, который умел превращать любой предмет в золото." */),
    DATA_COMPGEN(0x00503ed8, gArtifactEventNomadBootsOfMobility, "\xc1\xf0\xee\xe4\xff\xf7\xe8\xe9\x20\xf2\xee\xf0\xe3\xee\xe2\xe5\xf6\x20\xef\xf0\xee\xf1\xe8\xf2\x20\xe2\xe0\xf1\x20\xe7\xe0\xf9\xe8\xf2\xe8\xf2\xfc\x20\xe5\xe3\xee\x20\xee\xf2\x20\xe1\xe0\xed\xe4\xfb\x20\xe3\xee\xe1\xeb\xe8\xed\xee\xe2\x2e\x20\xc2\x20\xed\xe0\xe3\xf0\xe0\xe4\xf3\x20\xee\xed\x20\xe4\xe0\xf0\xe8\xf2\x20\xe2\xe0\xec\x20\xef\xe0\xf0\xf3\x20\xe8\xe7\xff\xf9\xed\xfb\xf5\x20\xe1\xe0\xf8\xec\xe0\xea\xee\xe2\x2c\x20\xe8\xf1\xef\xe5\xf9\xf0\xe5\xed\xed\xfb\xf5\x20\xe7\xe0\xe3\xe0\xe4\xee\xf7\xed\xfb\xec\xe8\x20\xe4\xf0\xe5\xe2\xed\xe8\xec\xe8\x20\xef\xe8\xf1\xfc\xec\xe5\xed\xe0\xec\xe8\x2e" /* "Бродячий торговец просит вас защитить его от банды гоблинов. В награду он дарит вам пару изящных башмаков, испещренных загадочными древними письменами." */),
    DATA_COMPGEN(0x00503f70, gArtifactEventTravelerSBootsOfMobility, "\xce\xe1\xed\xe0\xf0\xf3\xe6\xe8\xe2\x20\xef\xe0\xf0\xf3\x20\xe7\xe0\xec\xe5\xf7\xe0\xf2\xe5\xeb\xfc\xed\xfb\xf5\x20\xe1\xe0\xf8\xec\xe0\xea\xee\xe2\x20\xf3\xea\xf0\xe0\xf8\xe5\xed\xed\xfb\xf5\x20\xe1\xe8\xf1\xe5\xf0\xee\xec\x2c\x20\xe2\xfb\x20\xe1\xeb\xe0\xe3\xee\xe4\xe0\xf0\xe8\xf2\xe5\x20\xe7\xe0\xe3\xe0\xe4\xee\xf7\xed\xee\xe3\xee\x20\xe1\xeb\xe0\xe3\xee\xe4\xe5\xf2\xe5\xeb\xff\x20\xe8\x20\xee\xf1\xf2\xe0\xe2\xeb\xff\xe5\xf2\xe5\x20\xe8\xf5\x20\xf1\xe5\xe1\xe5\x2e" /* "Обнаружив пару замечательных башмаков украшенных бисером, вы благодарите загадочного благодетеля и оставляете их себе." */),
    DATA_COMPGEN(0x00503fe8, gArtifactEventLuckyRabbitSFoot, "\xc2\x20\xf3\xef\xeb\xe0\xf2\xf3\x20\xe7\xe0\x20\xee\xf5\xf0\xe0\xed\xf3\x20\xe2\x20\xef\xf3\xf2\xe8\x20\xf1\xf2\xf0\xe0\xed\xf1\xf2\xe2\xf3\xfe\xf9\xe8\xe9\x20\xf2\xee\xf0\xe3\xee\xe2\xe5\xf6\x20\xef\xf0\xe5\xe4\xeb\xe0\xe3\xe0\xe5\xf2\x20\xe2\xe0\xec\x20\xeb\xe0\xef\xea\xf3\x20\xea\xf0\xee\xeb\xe8\xea\xe0\x2e\x20\xcf\xee\x20\xe5\xe3\xee\x20\xf1\xeb\xee\xe2\xe0\xec\x2c\x20\xee\xed\xe0\x20\xef\xf0\xe8\xed\xe5\xf1\xe5\xf2\x20\xe2\xe0\xec\x20\xf3\xe4\xe0\xf7\xf3\x20\xe2\x20\xe1\xee\xfe\x2e" /* "В уплату за охрану в пути странствующий торговец предлагает вам лапку кролика. По его словам, она принесет вам удачу в бою." */),
    DATA_COMPGEN(0x00504064, gArtifactEventGoldenHorseshoe, "\xcf\xee\xef\xe0\xe2\xf8\xe8\xe9\x20\xe2\x20\xeb\xee\xe2\xf3\xf8\xea\xf3\x20\xe5\xe4\xe8\xed\xee\xf0\xee\xe3\x20\xe8\xf1\xef\xf3\xe3\xe0\xed\xed\xee\x20\xea\xf0\xe8\xf7\xe8\xf2\x2e\x20\xc2\xfb\x20\xf3\xf1\xef\xee\xea\xe0\xe8\xe2\xe0\xe5\xf2\xe5\x20\xe5\xe3\xee\x20\xe8\x20\xee\xf1\xe2\xee\xe1\xee\xe6\xe4\xe0\xe5\xf2\xe5\x20\xee\xf2\x20\xef\xf3\xf2\x2e\x20\xc2\xf1\xf5\xf0\xe0\xef\xed\xf3\xe2\x20\xe8\x20\xf3\xe4\xe0\xf0\xe8\xe2\x20\xea\xee\xef\xfb\xf2\xee\xec\x2c\x20\xee\xed\x20\xf3\xed\xee\xf1\xe8\xf2\xf1\xff\x20\xef\xf0\xee\xf7\xfc\x2e\x20\xd2\xe0\xec\x2c\x20\xe3\xe4\xe5\x20\xee\xed\x20\xf2\xee\xeb\xfc\xea\xee\x20\xf7\xf2\xee\x20\xf1\xf2\xee\xff\xeb\x2c\x20\xee\xf1\xf2\xe0\xeb\xe0\xf1\xfc\x20\xeb\xe5\xe6\xe0\xf2\xfc\x20\xe7\xee\xeb\xee\xf2\xe0\xff\x20\xef\xee\xe4\xea\xee\xe2\xe0\x2e" /* "Попавший в ловушку единорог испуганно кричит. Вы успокаиваете его и освобождаете от пут. Всхрапнув и ударив копытом, он уносится прочь. Там, где он только что стоял, осталась лежать золотая подкова." */),
    DATA_COMPGEN(0x0050412c, gArtifactEventGamblerSLuckyCoin, "\xc2\xfb\x20\xef\xee\xe9\xec\xe0\xeb\xe8\x20\xee\xe7\xee\xf0\xed\xee\xe3\xee\x20\xe1\xe5\xf1\xe5\xed\xea\xe0\x2c\x20\xea\xee\xf2\xee\xf0\xfb\xe9\x20\xed\xe5\x20\xe4\xe0\xe2\xe0\xeb\x20\xef\xee\xea\xee\xff\x20\xe2\xf1\xe5\xe9\x20\xee\xea\xf0\xf3\xe3\xe5\x2e\x20\xc2\x20\xee\xe1\xec\xe5\xed\x20\xed\xe0\x20\xf1\xe2\xee\xe1\xee\xe4\xf3\x20\xee\xed\x20\xef\xf0\xe5\xe4\xeb\xe0\xe3\xe0\xe5\xf2\x20\xe2\xe0\xec\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xf3\xfe\x20\xec\xee\xed\xe5\xf2\xf3\x2e" /* "Вы поймали озорного бесенка, который не давал покоя всей округе. В обмен на свободу он предлагает вам волшебную монету." */),
    DATA_COMPGEN(0x005041a4, gArtifactEventFourLeafClover, "\xcf\xee\xf1\xf0\xe5\xe4\xe8\x20\xec\xe5\xf0\xf2\xe2\xee\xe9\x20\xeb\xee\xf9\xe8\xed\xfb\x2c\x20\xe7\xe0\xef\xee\xeb\xed\xe5\xed\xed\xee\xe9\x20\xe8\xf1\xf1\xee\xf5\xf8\xe5\xe9\x20\xf0\xe0\xf1\xf2\xe8\xf2\xe5\xeb\xfc\xed\xee\xf1\xf2\xfc\xfe\x2c\x20\xe2\xfb\x2c\x20\xea\x20\xf1\xe2\xee\xe5\xec\xf3\x20\xf3\xe4\xe8\xe2\xeb\xe5\xed\xe8\xfe\x2c\x20\xe7\xe0\xec\xe5\xf7\xe5\xf2\xe5\x20\xe2\xe5\xf1\xe5\xeb\xfb\xe9\x20\xe7\xe5\xeb\xe5\xed\xfb\xe9\x20\xef\xee\xe1\xe5\xe3\x20\xf7\xe5\xf2\xfb\xf0\xe5\xf5\xeb\xe8\xf1\xf2\xed\xee\xe3\xee\x20\xea\xeb\xe5\xe2\xe5\xf0\xe0\x2e" /* "Посреди мертвой лощины, заполненной иссохшей растительностью, вы, к своему удивлению, замечете веселый зеленый побег четырехлистного клевера." */),
    DATA_COMPGEN(0x00504234, gArtifactEventTrueCompassOfMobility, "\xd1\xf2\xf0\xe0\xed\xed\xee\xe2\xe0\xf2\xfb\xe9\x20\xf1\xf2\xe0\xf0\xe8\xea\xe0\xf8\xea\xe0\x20\xf3\xf2\xe2\xe5\xf0\xe6\xe4\xe0\xe5\xf2\x2c\x20\xf7\xf2\xee\x20\xee\xed\x20\x2d\x20\xe2\xe5\xeb\xe8\xea\xe8\xe9\x20\xe8\xe7\xee\xe1\xf0\xe5\xf2\xe0\xf2\xe5\xeb\xfc\x2c\x20\xe8\x20\xef\xf0\xee\xf1\xe8\xf2\x20\xe2\xe0\xf1\x20\xe8\xf1\xef\xfb\xf2\xe0\xf2\xfc\x20\xe5\xe3\xee\x20\xed\xee\xe2\xee\xe5\x20\xf2\xe2\xee\xf0\xe5\xed\xe8\xe5\x2e\x20\xcd\xe0\xe4\xf3\xe2\xf8\xe8\xf1\xfc\x20\xee\xf2\x20\xe2\xe0\xe6\xed\xee\xf1\xf2\xe8\x2c\x20\xee\xed\x20\xe2\xf0\xf3\xf7\xe0\xe5\xf2\x20\xe2\xe0\xec\x20\xea\xee\xec\xef\xe0\xf1\x2e" /* "Странноватый старикашка утверждает, что он - великий изобретатель, и просит вас испытать его новое творение. Надувшись от важности, он вручает вам компас." */),
    DATA_COMPGEN(0x005042d0, gArtifactEventSailorSAstrolabeOfMobility, "\xd1\xf2\xe0\xf0\xfb\xe9\x20\xec\xee\xf0\xe5\xf5\xee\xe4\x20\xf1\xf2\xe0\xeb\x20\xe4\xee\xe1\xfb\xf7\xe5\xe9\x20\xeb\xfe\xe4\xee\xe5\xe4\xee\xe2\x2e\x20\xc2\xfb\x20\xf1\xef\xe0\xf1\xe0\xe5\xf2\xe5\x20\xe5\xe3\xee\x2c\x20\xe8\x20\xe2\x20\xe7\xed\xe0\xea\x20\xe1\xeb\xe0\xe3\xee\xe4\xe0\xf0\xed\xee\xf1\xf2\xe8\x20\xee\xed\x20\xe4\xe0\xf0\xe8\xf2\x20\xe2\xe0\xec\x20\xf7\xf3\xe4\xe5\xf1\xed\xfb\xe9\x20\xe8\xed\xf1\xf2\xf0\xf3\xec\xe5\xed\xf2\x2c\x20\xef\xee\xe7\xe2\xee\xeb\xff\xfe\xf9\xe8\xe9\x20\xe8\xe7\xec\xe5\xf0\xff\xf2\xfc\x20\xf0\xe0\xf1\xf1\xf2\xee\xff\xed\xe8\xff\x20\xef\xee\x20\xe7\xe2\xe5\xe7\xe4\xe0\xec\x2e" /* "Старый мореход стал добычей людоедов. Вы спасаете его, и в знак благодарности он дарит вам чудесный инструмент, позволяющий измерять расстояния по звездам." */),
    DATA_COMPGEN(0x0050436c, gArtifactEventEvilEye, "\xc2\x20\xe7\xe0\xe1\xf0\xee\xf8\xe5\xed\xed\xee\xe9\x20\xf5\xe8\xe6\xe8\xed\xe5\x20\xe2\xfb\x20\xed\xe0\xf5\xee\xe4\xe8\xf2\xe5\x20\xf1\xea\xe5\xeb\xe5\xf2\x20\xe4\xe0\xe2\xed\xee\x20\xef\xee\xf7\xe8\xe2\xf8\xe5\xe9\x20\xea\xee\xeb\xe4\xf3\xed\xfc\xe8\x2e\x20\xcf\xf0\xe8\xe3\xeb\xff\xe4\xe5\xe2\xf8\xe8\xf1\xfc\x2c\x20\xe2\xfb\x20\xe7\xe0\xec\xe5\xf7\xe0\xe5\xf2\xe5\x2c\x20\xf7\xf2\xee\x20\xe2\x20\xe3\xeb\xe0\xe7\xed\xe8\xf6\xe5\x20\xef\xee\xe6\xe5\xeb\xf2\xe5\xe2\xf8\xe5\xe3\xee\x20\xf7\xe5\xf0\xe5\xef\xe0\x20\xe7\xeb\xee\xe2\xe5\xf9\xe5\x20\xe2\xf0\xe0\xf9\xe0\xe5\xf2\xf1\xff\x20\xf1\xf2\xe5\xea\xeb\xff\xed\xed\xfb\xe9\x20\xe3\xeb\xe0\xe7\x2e" /* "В заброшенной хижине вы находите скелет давно почившей колдуньи. Приглядевшись, вы замечаете, что в глазнице пожелтевшего черепа зловеще вращается стеклянный глаз." */),
    DATA_COMPGEN(0x00504410, gArtifactEventEnchantedHourglass, "\xc7\xe0\x20\xed\xe5\xe2\xfb\xf1\xee\xea\xe8\xec\x20\xf5\xee\xeb\xec\xee\xec\x20\xef\xe5\xf0\xe5\xe4\x20\xe2\xe0\xec\xe8\x20\xee\xf2\xea\xf0\xfb\xe2\xe0\xe5\xf2\xf1\xff\x20\xe7\xeb\xee\xe2\xe5\xf9\xe0\xff\x20\xea\xe0\xf0\xf2\xe8\xed\xe0\x20\x2d\x20\xf1\xf2\xe0\xe8\x20\xf1\xf2\xe5\xf0\xe2\xff\xf2\xed\xe8\xea\xee\xe2\x20\xef\xe8\xf0\xf3\xfe\xf2\x20\xed\xe0\x20\xef\xee\xeb\xe5\x20\xed\xe5\xe4\xe0\xe2\xed\xe5\xe9\x20\xe1\xe8\xf2\xe2\xfb\x2e\x20\xd1\xf0\xe5\xe4\xe8\x20\xf2\xe5\xeb\x20\xef\xee\xe2\xe5\xf0\xe6\xe5\xed\xed\xfb\xf5\x20\xe2\xee\xe8\xed\xee\xe2\x20\xe2\xfb\x20\xed\xe0\xf5\xee\xe4\xe8\xf2\xe5\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xfb\xe5\x20\xef\xe5\xf1\xee\xf7\xed\xfb\xe5\x20\xf7\xe0\xf1\xfb\x2e" /* "За невысоким холмом перед вами открывается зловещая картина - стаи стервятников пируют на поле недавней битвы. Среди тел поверженных воинов вы находите волшебные песочные часы." */),
    DATA_COMPGEN(0x005044c4, gArtifactEventGoldWatch, "\xc2\xfb\x20\xef\xee\xec\xee\xe3\xe0\xe5\xf2\xe5\x20\xe1\xf0\xee\xe4\xff\xf7\xe5\xec\xf3\x20\xf2\xee\xf0\xe3\xee\xe2\xf6\xf3\x20\xf1\xed\xe0\xe4\xee\xe1\xfc\xff\xec\xe8\x20\xe2\xfb\xf2\xe0\xf9\xe8\xf2\xfc\x20\xef\xee\xe2\xee\xe7\xea\xf3\x20\xe8\xe7\x20\xef\xf0\xe8\xe4\xee\xf0\xee\xe6\xed\xee\xe9\x20\xea\xe0\xed\xe0\xe2\xfb\x2e\x20\xc2\x20\xe7\xed\xe0\xea\x20\xe1\xeb\xe0\xe3\xee\xe4\xe0\xf0\xed\xee\xf1\xf2\xe8\x20\xee\xed\x20\xe2\xf0\xf3\xf7\xe0\xe5\xf2\x20\xe2\xe0\xec\x20\xe7\xee\xeb\xee\xf2\xfb\xe5\x20\xf7\xe0\xf1\xfb\x2e\x20\xce\xed\x20\xe8\x20\xed\xe5\x20\xef\xee\xe4\xee\xe7\xf0\xe5\xe2\xe0\xeb\x2c\x20\xf7\xf2\xee\x20\xf7\xe0\xf1\xfb\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xfb\xe5\x21" /* "Вы помогаете бродячему торговцу снадобьями вытащить повозку из придорожной канавы. В знак благодарности он вручает вам золотые часы. Он и не подозревал, что часы волшебные!" */),
    DATA_COMPGEN(0x00504574, gArtifactEventSkullcap, "\xc2\xfb\x20\xe4\xe5\xeb\xe0\xe5\xf2\xe5\x20\xea\xee\xf0\xee\xf2\xea\xf3\xfe\x20\xee\xf1\xf2\xe0\xed\xee\xe2\xea\xf3\x20\xe2\x20\xec\xe0\xeb\xe5\xed\xfc\xea\xee\xe9\x20\xef\xf0\xe8\xe4\xee\xf0\xee\xe6\xed\xee\xe9\x20\xf5\xe0\xf0\xf7\xe5\xe2\xed\xe5\x2e\x20\xcf\xee\xe4\x20\xe7\xe2\xee\xed\x20\xec\xee\xed\xe5\xf2\x20\xef\xf0\xee\xe8\xf1\xf5\xee\xe4\xe8\xf2\x20\xee\xe1\xec\xe5\xed\x20\xed\xee\xe2\xee\xf1\xf2\xff\xec\xe8\x2c\x20\xe0\x20\xf2\xee\x20\xe8\x20\xf0\xe5\xe4\xea\xe8\xec\xe8\x20\xe2\xe5\xf9\xe8\xf6\xe0\xec\xe8\x2e\x20\xc2\xee\xf2\x20\xf2\xe0\xea\xe8\xec\x2d\xf2\xee\x20\xee\xe1\xf0\xe0\xe7\xee\xec\x20\xe2\x20\xe2\xe0\xf8\xe5\xec\x20\xe1\xe0\xe3\xe0\xe6\xe5\x20\xe8\x20\xee\xea\xe0\xe7\xfb\xe2\xe0\xe5\xf2\xf1\xff\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xe0\xff\x20\xf8\xe0\xef\xee\xf7\xea\xe0\x2e" /* "Вы делаете короткую остановку в маленькой придорожной харчевне. Под звон монет происходит обмен новостями, а то и редкими вещицами. Вот таким-то образом в вашем багаже и оказывается волшебная шапочка." */),
    DATA_COMPGEN(0x00504640, gArtifactEventIceCloak, "\xc2\xfb\x20\xf1\xef\xe5\xf8\xe8\xf2\xe5\x20\xed\xe0\x20\xee\xf2\xf7\xe0\xff\xed\xed\xfb\xe5\x20\xea\xf0\xe8\xea\xe8\x20\xe8\x20\xe2\xe8\xe4\xe8\xf2\xe5\x20\xee\xf7\xe0\xf0\xee\xe2\xe0\xf2\xe5\xeb\xfc\xed\xf3\xfe\x20\xe4\xe5\xe2\xf3\xf8\xea\xf3\x2c\x20\xe7\xe0\x20\xea\xee\xf2\xee\xf0\xee\xe9\x20\xe3\xee\xed\xe8\xf2\xf1\xff\x20\xf0\xe0\xe7\xfa\xff\xf0\xe5\xed\xed\xfb\xe9\x20\xec\xe5\xe4\xe2\xe5\xe4\xfc\x2e\x20\xd7\xe5\xf0\xe5\xe7\x20\xec\xe3\xed\xee\xe2\xe5\xed\xe8\xe5\x20\xe7\xe2\xe5\xf0\xfc\x20\xef\xee\xe2\xe5\xf0\xe6\xe5\xed\x2c\x20\xe8\x20\xe1\xeb\xe0\xe3\xee\xe4\xe0\xf0\xed\xe0\xff\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xe8\xf6\xe0\x20\xf8\xfc\xe5\xf2\x20\xe2\xe0\xec\x20\xe8\xe7\x20\xe5\xe3\xee\x20\xf8\xea\xf3\xf0\xfb\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xfb\xe9\x20\xef\xeb\xe0\xf9\x2e" /* "Вы спешите на отчаянные крики и видите очаровательную девушку, за которой гонится разъяренный медведь. Через мгновение зверь повержен, и благодарная волшебница шьет вам из его шкуры волшебный плащ." */),
    DATA_COMPGEN(0x00504708, gArtifactEventFireCloak, "\xc7\xe0\x20\xef\xee\xe2\xee\xf0\xee\xf2\xee\xec\x20\xe4\xee\xf0\xee\xe3\xe8\x20\xe2\xfb\x20\xe2\xe8\xe4\xe8\xf2\xe5\x20\xf1\xf0\xe0\xe6\xe0\xfe\xf9\xe8\xf5\xf1\xff\x20\xed\xe5\xea\xf0\xee\xec\xe0\xed\xf2\xe0\x20\xe8\x20\xef\xe0\xeb\xe0\xe4\xe8\xed\xe0\x2e\x20\xcd\xe5\xea\xf0\xee\xec\xe0\xed\xf2\x20\xe0\xf2\xe0\xea\xf3\xe5\xf2\x20\xef\xe0\xeb\xe0\xe4\xe8\xed\xe0\x2c\x20\xe8\x20\xf2\xee\xf2\x20\xef\xe0\xe4\xe0\xe5\xf2\x20\xed\xe0\x20\xea\xee\xeb\xe5\xed\xe8\x2e\x20\xc2\xfb\x20\xf1\xef\xe0\xf1\xe0\xe5\xf2\xe5\x20\xe6\xe8\xe7\xed\xfc\x20\xef\xe0\xeb\xe0\xe4\xe8\xed\xf3\x2c\x20\xf3\xe1\xe8\xe2\xe0\xff\x20\xe5\xe3\xee\x20\xe2\xf0\xe0\xe3\xe0\x2e\x20\xcf\xe0\xeb\xe0\xe4\xe8\xed\x20\xe4\xe0\xf0\xe8\xf2\x20\xe2\xe0\xec\x20\xf1\xe2\xee\xfe\x20\xee\xe3\xed\xe5\xed\xed\xf3\xfe\x20\xed\xe0\xea\xe8\xe4\xea\xf3\x2e" /* "За поворотом дороги вы видите сражающихся некроманта и паладина. Некромант атакует паладина, и тот падает на колени. Вы спасаете жизнь паладину, убивая его врага. Паладин дарит вам свою огненную накидку." */),
    DATA_COMPGEN(0x005047d4, gArtifactEventLightningHelm, "\xc1\xf0\xee\xe4\xff\xf7\xe8\xe9\x20\xec\xe5\xe4\xed\xe8\xea\x2c\x20\xf3\x20\xea\xee\xf2\xee\xf0\xee\xe3\xee\x20\xea\xee\xed\xf7\xe8\xeb\xe0\xf1\xfc\x20\xef\xf0\xee\xe2\xe8\xe7\xe8\xff\x2c\x20\xef\xf0\xe5\xe4\xeb\xe0\xe3\xe0\xe5\xf2\x20\xe2\xe0\xec\x20\xf8\xeb\xe5\xec\x20\xf1\x20\xe3\xf0\xe5\xe1\xed\xe5\xec\x20\xe2\x20\xe2\xe8\xe4\xe5\x20\xec\xee\xeb\xed\xe8\xe8\x20\xe2\x20\xee\xe1\xec\xe5\xed\x20\xed\xe0\x20\xe5\xe4\xf3\x20\xe8\x20\xef\xe8\xf2\xfc\xe5\x2e\x20\xc2\xfb\x20\xf1\xee\xe3\xeb\xe0\xf8\xe0\xe5\xf2\xe5\xf1\xfc\x20\xed\xe0\x20\xee\xe1\xec\xe5\xed\x2c\x20\xe0\x20\xe2\xf1\xea\xee\xf0\xe5\x20\xee\xe1\xed\xe0\xf0\xf3\xe6\xe8\xe2\xe0\xe5\xf2\xe5\x2c\x20\xf7\xf2\xee\x20\xf8\xeb\xe5\xec\x20\xee\xe1\xeb\xe0\xe4\xe0\xe5\xf2\x20\xe5\xf9\xe5\x20\xe8\x20\xec\xe0\xe3\xe8\xf7\xe5\xf1\xea\xe8\xec\xe8\x20\xf1\xe2\xee\xe9\xf1\xf2\xe2\xe0\xec\xe8\x2e" /* "Бродячий медник, у которого кончилась провизия, предлагает вам шлем с гребнем в виде молнии в обмен на еду и питье. Вы соглашаетесь на обмен, а вскоре обнаруживаете, что шлем обладает еще и магическими свойствами." */),
    DATA_COMPGEN(0x005048ac, gArtifactEventEvercoldIcicle, "\xc2\xe0\xf8\xe5\x20\xe2\xed\xe8\xec\xe0\xed\xe8\xe5\x20\xef\xf0\xe8\xe2\xeb\xe5\xea\xe0\xe5\xf2\x20\xeb\xe5\xe4\xff\xed\xe0\xff\x20\xf1\xee\xf1\xf3\xeb\xfc\xea\xe0\x2c\x20\xea\xee\xf2\xee\xf0\xe0\xff\x20\xed\xe5\x20\xf2\xe0\xe5\xf2\x2c\x20\xed\xe5\xf1\xec\xee\xf2\xf0\xff\x20\xed\xe0\x20\xef\xee\xeb\xf3\xe4\xe5\xed\xed\xfb\xe9\x20\xe7\xed\xee\xe9\x2e\x20\xc2\xfb\x20\xee\xf2\xeb\xe0\xec\xfb\xe2\xe0\xe5\xf2\xe5\x20\xe5\xe5\x20\xee\xf2\x20\xea\xe0\xf0\xed\xe8\xe7\xe0\x20\xe8\x20\xf1\x20\xf3\xe4\xe8\xe2\xeb\xe5\xed\xe8\xe5\xec\x20\xee\xe1\xed\xe0\xf0\xf3\xe6\xe8\xe2\xe0\xe5\xf2\xe5\x2c\x20\xf7\xf2\xee\x20\xe4\xe0\xe6\xe5\x20\xf2\xe5\xef\xeb\xee\x20\xe2\xe0\xf8\xe8\xf5\x20\xf0\xf3\xea\x20\xe5\xe9\x20\xed\xe8\xef\xee\xf7\xe5\xec\x2e" /* "Ваше внимание привлекает ледяная сосулька, которая не тает, несмотря на полуденный зной. Вы отламываете ее от карниза и с удивлением обнаруживаете, что даже тепло ваших рук ей нипочем." */),
    DATA_COMPGEN(0x00504968, gArtifactEventEverhotLavaRock, "\xc2\x20\xe4\xe0\xeb\xfc\xed\xe5\xe9\x20\xf1\xf2\xf0\xe0\xed\xe5\x20\xe2\xfb\x20\xe2\xf1\xf2\xf0\xe5\xf7\xe0\xe5\xf2\xe5\x20\xef\xeb\xe5\xec\xff\x20\xef\xf0\xe8\xec\xe0\xf2\xee\xe2\x2e\x20\xce\xed\xe8\x20\xf0\xe0\xe7\xe6\xe8\xe3\xe0\xfe\xf2\x20\xea\xee\xf1\xf2\xf0\xfb\x20\xef\xf0\xe8\x20\xef\xee\xec\xee\xf9\xe8\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xee\xe3\xee\x20\xea\xf3\xf1\xea\xe0\x20\xeb\xe0\xe2\xfb\x2e\x20\xc2\xfb\x20\xed\xe0\xf3\xf7\xe8\xeb\xe8\x20\xe8\xf5\x20\xe4\xee\xe1\xfb\xe2\xe0\xf2\xfc\x20\xee\xe3\xee\xed\xfc\x20\xee\xe1\xfb\xf7\xed\xfb\xec\x20\xf1\xef\xee\xf1\xee\xe1\xee\xec\x2e\x20\xce\xe1\xe5\xe7\xfc\xff\xed\xfb\x20\xf1\xf7\xe8\xf2\xe0\xfe\xf2\x20\xe2\xe0\xf1\x20\xe1\xee\xe3\xee\xec\x20\xe8\x20\xe4\xe0\xf0\xff\xf2\x20\xf1\xe2\xee\xe9\x20\xe7\xe0\xe2\xe5\xf2\xed\xfb\xe9\x20\xea\xf3\xf1\xee\xea\x20\xeb\xe0\xe2\xfb\x2e" /* "В дальней стране вы встречаете племя приматов. Они разжигают костры при помощи волшебного куска лавы. Вы научили их добывать огонь обычным способом. Обезьяны считают вас богом и дарят свой заветный кусок лавы." */),
    DATA_COMPGEN(0x00504a3c, gArtifactEventLightningRod, "\xc2\xee\x20\xe2\xf0\xe5\xec\xff\x20\xf3\xe6\xe0\xf1\xed\xee\xe9\x20\xe3\xf0\xee\xe7\xfb\x20\xed\xe0\x20\xe2\xe0\xf8\xe8\xf5\x20\xe3\xeb\xe0\xe7\xe0\xf5\x20\xe2\x20\xe3\xf0\xee\xec\xee\xee\xf2\xe2\xee\xe4\x20\xe4\xee\xec\xe0\x20\xe1\xfc\xe5\xf2\x20\xec\xee\xeb\xed\xe8\xff\x2e\x20\xd0\xe0\xf1\xef\xeb\xe0\xe2\xeb\xe5\xed\xed\xfb\xe9\x20\xe3\xf0\xee\xec\xee\xee\xf2\xe2\xee\xe4\x20\xef\xe0\xe4\xe0\xe5\xf2\x20\xed\xe0\x20\xe7\xe5\xec\xeb\xfe\x2c\x20\xed\xee\x20\xe5\xe3\xee\x20\xed\xe0\xea\xee\xed\xe5\xf7\xed\xe8\xea\x20\xee\xf1\xf2\xe0\xe5\xf2\xf1\xff\x20\xf6\xe5\xeb\xfb\xec\x20\xe8\x20\xed\xe5\xe2\xf0\xe5\xe4\xe8\xec\xfb\xec\x2e\x20\xc2\xfb\x20\xef\xee\xe4\xee\xe1\xf0\xe0\xeb\xe8\x20\xe5\xe3\xee\x20\x2d\x20\xee\xea\xe0\xe7\xe0\xeb\xee\xf1\xfc\x2c\x20\xfd\xf2\xee\x20\xec\xe0\xe3\xe8\xf7\xe5\xf1\xea\xe8\xe9\x20\xef\xf0\xe5\xe4\xec\xe5\xf2\x21" /* "Во время ужасной грозы на ваших глазах в громоотвод дома бьет молния. Расплавленный громоотвод падает на землю, но его наконечник остается целым и невредимым. Вы подобрали его - оказалось, это магический предмет!" */),
    DATA_COMPGEN(0x00504b14, gArtifactEventSnakeRing, "\xcd\xe0\x20\xef\xe0\xeb\xfc\xf6\xe5\x20\xec\xe5\xf0\xf2\xe2\xee\xe3\xee\x20\xf1\xf2\xf0\xe0\xed\xed\xe8\xea\xe0\x20\xe2\xfb\x20\xe2\xe8\xe4\xe8\xf2\xe5\x20\xed\xe5\xee\xe1\xfb\xf7\xed\xee\xe5\x20\xea\xee\xeb\xfc\xf6\xee\x2e\x20\xce\xed\xee\x20\xe8\xec\xe5\xe5\xf2\x20\xf4\xee\xf0\xec\xf3\x20\xe7\xec\xe5\xe8\x2c\x20\xe2\xf6\xe5\xef\xe8\xe2\xf8\xe5\xe9\xf1\xff\x20\xe7\xf3\xe1\xe0\xec\xe8\x20\xe2\x20\xf1\xee\xe1\xf1\xf2\xe2\xe5\xed\xed\xfb\xe9\x20\xf5\xe2\xee\xf1\xf2\x2e" /* "На пальце мертвого странника вы видите необычное кольцо. Оно имеет форму змеи, вцепившейся зубами в собственный хвост." */),
    DATA_COMPGEN(0x00504b8c, gArtifactEventAnkh, "\xcf\xe5\xf1\xf7\xe0\xed\xe0\xff\x20\xe1\xf3\xf0\xff\x20\xee\xe1\xed\xe0\xe6\xe8\xeb\xe0\x20\xe2\xf5\xee\xe4\x20\xe2\x20\xef\xee\xe4\xe7\xe5\xec\xed\xf3\xfe\x20\xe3\xf0\xee\xe1\xed\xe8\xf6\xf3\x2e\x20\xc2\xfb\x20\xf1\xef\xf3\xf1\xea\xe0\xe5\xf2\xe5\xf1\xfc\x20\xe2\xed\xf3\xf2\xf0\xfc\x20\xe8\x20\xee\xe1\xed\xe0\xf0\xf3\xe6\xe8\xe2\xe0\xe5\xf2\xe5\x2c\x20\xf7\xf2\xee\x20\xe7\xe4\xe5\xf1\xfc\x20\xf3\xe6\xe5\x20\xef\xee\xe1\xfb\xe2\xe0\xeb\xe8\x20\xe3\xf0\xe0\xe1\xe8\xf2\xe5\xeb\xe8\x2c\x20\xee\xe4\xed\xe0\xea\xee\x20\xe2\x20\xf2\xe5\xec\xed\xee\xf2\xe5\x20\xee\xed\xe8\x20\xed\xe5\x20\xe7\xe0\xec\xe5\xf2\xe8\xeb\xe8\x20\xf1\xe8\xec\xe2\xee\xeb\x20\xe2\xe5\xf7\xed\xee\xe9\x20\xe6\xe8\xe7\xed\xe8\x2c\x20\xe2\xe8\xf1\xff\xf9\xe8\xe9\x20\xed\xe0\x20\xf1\xe5\xf0\xe5\xe1\xf0\xff\xed\xee\xe9\x20\xf6\xe5\xef\xe8\x2e" /* "Песчаная буря обнажила вход в подземную гробницу. Вы спускаетесь внутрь и обнаруживаете, что здесь уже побывали грабители, однако в темноте они не заметили символ вечной жизни, висящий на серебряной цепи." */),
    DATA_COMPGEN(0x00504c5c, gArtifactEventBookOfElements, "\xc2\xfb\x20\xe2\xf1\xf2\xf0\xe5\xf7\xe0\xe5\xf2\xe5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xf2\xe5\xeb\xff\x2c\x20\xea\xee\xf2\xee\xf0\xfb\xe9\x20\xef\xf0\xee\xf1\xe8\xf2\x20\xf0\xe0\xe7\xf0\xe5\xf8\xe8\xf2\xfc\x20\xe5\xec\xf3\x20\xe2\xee\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xf2\xfc\xf1\xff\x20\xe2\xe0\xf8\xe8\xec\x20\xef\xee\xea\xf0\xee\xe2\xe8\xf2\xe5\xeb\xfc\xf1\xf2\xe2\xee\xec\x20\xed\xe0\x20\xee\xef\xe0\xf1\xed\xee\xec\x20\xf3\xf7\xe0\xf1\xf2\xea\xe5\x20\xef\xf3\xf2\xe8\x2e\x20\xc2\xfb\x20\xf1\xee\xe3\xeb\xe0\xf8\xe0\xe5\xf2\xe5\xf1\xfc\x2c\x20\xe8\x20\xe2\x20\xed\xe0\xe3\xf0\xe0\xe4\xf3\x20\xee\xed\x20\xe4\xe0\xf0\xe8\xf2\x20\xe2\xe0\xec\x20\xca\xed\xe8\xe3\xf3\x20\xd1\xf2\xe8\xf5\xe8\xe9\x2e" /* "Вы встречаете заклинателя, который просит разрешить ему воспользоваться вашим покровительством на опасном участке пути. Вы соглашаетесь, и в награду он дарит вам Книгу Стихий." */),
    DATA_COMPGEN(0x00504d0c, gArtifactEventElementalRing, "\xd0\xe0\xf1\xef\xee\xeb\xee\xe6\xe8\xe2\xf8\xe8\xf1\xfc\x20\xed\xe0\x20\xee\xf2\xe4\xfb\xf5\x20\xef\xee\xe4\x20\xed\xe5\xe2\xfb\xf1\xee\xea\xe8\xec\x20\xe4\xe5\xf0\xe5\xe2\xee\xec\x2c\x20\xe2\xfb\x20\xe7\xe0\xec\xe5\xf7\xe0\xe5\xf2\xe5\x20\xe4\xe8\xea\xee\xe3\xee\x20\xea\xee\xf2\xe0\x2c\x20\xea\xee\xf2\xee\xf0\xfb\xe9\x20\xef\xee\xe4\xe1\xe8\xf0\xe0\xe5\xf2\xf1\xff\x20\xea\x20\xe2\xee\xf0\xee\xed\xfc\xe5\xec\xf3\x20\xe3\xed\xe5\xe7\xe4\xf3\x2e\x20\xc2\xfb\x20\xef\xf0\xee\xe3\xee\xed\xff\xe5\xf2\xe5\x20\xea\xee\xf2\xe0\x2c\x20\xe8\x20\xf1\xe0\xec\xe8\x20\xe7\xe0\xeb\xe5\xe7\xe0\xe5\xf2\xe5\x20\xed\xe0\x20\xe4\xe5\xf0\xe5\xe2\xee\x2e\x20\xc2\x20\xe3\xed\xe5\xe7\xe4\xe5\x20\xe2\xfb\x20\xed\xe0\xf5\xee\xe4\xe8\xf2\xe5\x20\xea\xee\xeb\xfc\xf6\xee\x20\xf2\xee\xed\xea\xee\xe9\x20\xf0\xe0\xe1\xee\xf2\xfb\x2e" /* "Расположившись на отдых под невысоким деревом, вы замечаете дикого кота, который подбирается к вороньему гнезду. Вы прогоняете кота, и сами залезаете на дерево. В гнезде вы находите кольцо тонкой работы." */),
    DATA_COMPGEN(0x00504dd8, gArtifactEventHolyPendant, "\xd1\xf2\xf0\xe0\xed\xf1\xf2\xe2\xf3\xff\x20\xef\xee\x20\xe4\xe0\xeb\xfc\xed\xe8\xec\x20\xe7\xe5\xec\xeb\xff\xec\x2c\x20\xe2\xfb\x20\xe2\xf1\xf2\xf0\xe5\xf7\xe0\xe5\xf2\xe5\x20\xee\xf2\xf8\xe5\xeb\xfc\xed\xe8\xea\xe0\x2c\x20\xe6\xe8\xe2\xf3\xf9\xe5\xe3\xee\x20\xe2\x20\xec\xe0\xeb\xe5\xed\xfc\xea\xee\xe9\x20\xe0\xea\xea\xf3\xf0\xe0\xf2\xed\xee\xe9\x20\xf5\xe8\xe6\xe8\xed\xe5\x2e\x20\xd3\xe7\xed\xe0\xe2\x20\xee\x20\xf6\xe5\xeb\xe8\x20\xe2\xe0\xf8\xe8\xf5\x20\xf1\xea\xe8\xf2\xe0\xed\xe8\xe9\x2c\x20\xee\xed\x20\xef\xf0\xe5\xf0\xfb\xe2\xe0\xe5\xf2\x20\xf1\xe2\xee\xe8\x20\xf0\xe0\xe7\xec\xfb\xf8\xeb\xe5\xed\xe8\xff\x2c\x20\xe1\xeb\xe0\xe3\xee\xf1\xeb\xee\xe2\xeb\xff\xe5\xf2\x20\xe2\xe0\xf1\x20\xe8\x20\xe4\xe0\xf0\xe8\xf2\x20\xe0\xec\xf3\xeb\xe5\xf2\x2c\x20\xe7\xe0\xf9\xe8\xf9\xe0\xfe\xf9\xe8\xe9\x20\xee\xf2\x20\xe7\xeb\xfb\xf5\x20\xf7\xe0\xf0\x2e" /* "Странствуя по дальним землям, вы встречаете отшельника, живущего в маленькой аккуратной хижине. Узнав о цели ваших скитаний, он прерывает свои размышления, благословляет вас и дарит амулет, защищающий от злых чар." */),
    DATA_COMPGEN(0x00504eb0, gArtifactEventPendantOfFreeWill, "\xc2\xfb\x20\xf1\xeb\xfb\xf8\xe8\xf2\xe5\x20\xea\xf0\xe8\xea\xe8\x20\xee\x20\xef\xee\xec\xee\xf9\xe8\x20\xe8\x2c\x20\xef\xee\xf1\xef\xe5\xf8\xe8\xe2\x20\xed\xe0\x20\xe1\xe5\xf0\xe5\xe3\x20\xf0\xe5\xea\xe8\x2c\x20\xe2\xe8\xe4\xe8\xf2\xe5\x20\xf4\xe5\xe9\x2c\x20\xef\xee\xf2\xe5\xf8\xe0\xfe\xf9\xe8\xf5\xf1\xff\x20\xed\xe0\xe4\x20\xf1\xf2\xe0\xf0\xe8\xea\xee\xec\x2c\x20\xee\xea\xf3\xed\xe0\xff\x20\xe5\xe3\xee\x20\xe2\x20\xe2\xee\xe4\xf3\x2e\x20\xc2\xfb\x20\xe2\xfb\xf0\xf3\xf7\xe0\xe5\xf2\xe5\x20\xf1\xf2\xe0\xf0\xe8\xea\xe0\x20\xe8\xe7\x20\xe1\xe5\xe4\xfb\x20\xe8\x20\xe2\xfb\xf2\xe0\xf1\xea\xe8\xe2\xe0\xe5\xf2\xe5\x20\xee\xe4\xed\xf3\x20\xf4\xe5\xfe\x20\xed\xe0\x20\xe1\xe5\xf0\xe5\xe3\x2e\x20\xc2\x20\xee\xe1\xec\xe5\xed\x20\xed\xe0\x20\xf1\xe2\xee\xe1\xee\xe4\xf3\x20\xee\xed\xe0\x20\xee\xf2\xe4\xe0\xe5\xf2\x20\xe2\xe0\xec\x20\xef\xee\xe4\xe2\xe5\xf1\xea\xf3\x2e" /* "Вы слышите крики о помощи и, поспешив на берег реки, видите фей, потешающихся над стариком, окуная его в воду. Вы выручаете старика из беды и вытаскиваете одну фею на берег. В обмен на свободу она отдает вам подвеску." */),
    DATA_COMPGEN(0x00504f8c, gArtifactEventPendantOfLife, "\xc2\x20\xe4\xee\xf0\xee\xe3\xe5\x20\xe2\xfb\x20\xe2\xf1\xf2\xf0\xe5\xf7\xe0\xe5\xf2\xe5\x20\xed\xe5\xe1\xee\xeb\xfc\xf8\xee\xe9\x20\xea\xe0\xf0\xe0\xe2\xe0\xed\x2e\x20\xd1\xfb\xe3\xf0\xe0\xe2\x20\xf1\x20\xf5\xee\xe7\xff\xe8\xed\xee\xec\x20\xea\xe0\xf0\xe0\xe2\xe0\xed\xe0\x20\xe2\x20\xea\xee\xf1\xf2\xe8\x2c\x20\xe2\xfb\x20\xe2\xfb\xe8\xe3\xf0\xfb\xe2\xe0\xe5\xf2\xe5\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xf3\xfe\x20\xef\xee\xe4\xe2\xe5\xf1\xea\xf3\x2e\x20\xc5\xe5\x20\xef\xf0\xe5\xe6\xed\xe8\xe9\x20\xe2\xeb\xe0\xe4\xe5\xeb\xe5\xf6\x20\xf3\xf2\xe2\xe5\xf0\xe6\xe4\xe0\xe5\xf2\x2c\x20\xf7\xf2\xee\x20\xee\xed\xe0\x20\xec\xee\xe6\xe5\xf2\x20\xef\xf0\xee\xf2\xe8\xe2\xee\xf1\xf2\xee\xff\xf2\xfc\x20\xf7\xe0\xf0\xe0\xec\x20\xf1\xec\xe5\xf0\xf2\xe8\x20\xed\xe5\xea\xf0\xee\xec\xe0\xed\xf2\xee\xe2\x2e" /* "В дороге вы встречаете небольшой караван. Сыграв с хозяином каравана в кости, вы выигрываете волшебную подвеску. Ее прежний владелец утверждает, что она может противостоять чарам смерти некромантов." */),
    DATA_COMPGEN(0x00505054, gArtifactEventSerenityPendant, "\xc2\xfb\x20\xf1\xef\xe5\xf8\xe8\xf2\xe5\x20\xed\xe0\x20\xf8\xf3\xec\x20\xf1\xf0\xe0\xe6\xe5\xed\xe8\xff\x20\xe8\x20\xe2\xe8\xe4\xe8\xf2\xe5\x20\xf1\xf2\xe0\xf0\xe8\xea\xe0\x2d\xe2\xe0\xf0\xe2\xe0\xf0\xe0\x2c\x20\xea\xee\xf2\xee\xf0\xfb\xe9\x20\xf1\x20\xf2\xf0\xf3\xe4\xee\xec\x20\xee\xf2\xe1\xe8\xe2\xe0\xe5\xf2\xf1\xff\x20\xee\xf2\x20\xe3\xe8\xe4\xf0\xfb\x2e\x20\xc2\x20\xed\xe0\xe3\xf0\xe0\xe4\xf3\x20\xe7\xe0\x20\xef\xee\xec\xee\xf9\xfc\x20\xe2\xe0\xf0\xe2\xe0\xf0\x20\xe4\xe0\xf0\xe8\xf2\x20\xe2\xe0\xec\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xfb\xe9\x20\xea\xf3\xeb\xee\xed\x2e" /* "Вы спешите на шум сражения и видите старика-варвара, который с трудом отбивается от гидры. В награду за помощь варвар дарит вам волшебный кулон." */),
    DATA_COMPGEN(0x005050e8, gArtifactEventSeeingEyePendant, "\xc2\x20\xf5\xe8\xe6\xe8\xed\xe5\x20\xf3\x20\xe4\xee\xf0\xee\xe3\xe8\x20\xe2\xfb\x20\xed\xe0\xf5\xee\xe4\xe8\xf2\xe5\x20\xf1\xeb\xe5\xef\xf3\xfe\x20\xf1\xf2\xe0\xf0\xf3\xf5\xf3\x2c\x20\xf3\xec\xe8\xf0\xe0\xfe\xf9\xf3\xfe\x20\xe2\x20\xef\xee\xeb\xed\xee\xec\x20\xee\xe4\xe8\xed\xee\xf7\xe5\xf1\xf2\xe2\xe5\x2e\x20\xc2\xfb\x20\xee\xe1\xe5\xf9\xe0\xe5\xf2\xe5\x20\xf3\xf1\xf2\xf0\xee\xe8\xf2\xfc\x20\xe5\xe9\x20\xe4\xee\xf1\xf2\xee\xe9\xed\xfb\xe5\x20\xef\xee\xf5\xee\xf0\xee\xed\xfb\x2e\x20\xc2\x20\xe7\xed\xe0\xea\x20\xe1\xeb\xe0\xe3\xee\xe4\xe0\xf0\xed\xee\xf1\xf2\xe8\x20\xee\xed\xe0\x20\xe4\xe0\xf0\xe8\xf2\x20\xe2\xe0\xec\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xf3\xfe\x20\xef\xee\xe4\xe2\xe5\xf1\xea\xf3\x2e" /* "В хижине у дороги вы находите слепую старуху, умирающую в полном одиночестве. Вы обещаете устроить ей достойные похороны. В знак благодарности она дарит вам волшебную подвеску." */),
    DATA_COMPGEN(0x0050519c, gArtifactEventKineticPendant, "\xc4\xee\xf0\xee\xe3\xf3\x20\xe2\xe0\xec\x20\xef\xf0\xe5\xe3\xf0\xe0\xe6\xe4\xe0\xe5\xf2\x20\xe3\xee\xeb\xe5\xec\x2c\x20\xed\xe0\x20\xf8\xe5\xe5\x20\xea\xee\xf2\xee\xf0\xee\xe3\xee\x20\xf1\xe2\xe5\xf0\xea\xe0\xe5\xf2\x20\xea\xf3\xeb\xee\xed\x2e\x20\xc2\xfb\x20\xef\xe5\xf0\xe5\xf0\xe5\xe7\xe0\xe5\xf2\xe5\x20\xf8\xed\xf3\xf0\xee\xea\x2c\x20\xe8\x20\xee\xed\x20\xef\xe0\xe4\xe0\xe5\xf2\x20\xed\xe0\x20\xe7\xe5\xec\xeb\xfe\x2e\x20\xc3\xee\xeb\xe5\xec\x20\xf0\xe0\xf1\xf1\xfb\xef\xe0\xe5\xf2\xf1\xff\x20\xf3\x20\xe2\xe0\xf1\x20\xed\xe0\x20\xe3\xeb\xe0\xe7\xe0\xf5\x2c\x20\xe0\x20\xea\xf3\xeb\xee\xed\x20\xe4\xee\xf1\xf2\xe0\xe5\xf2\xf1\xff\x20\xe2\xe0\xec\x2e" /* "Дорогу вам преграждает голем, на шее которого сверкает кулон. Вы перерезаете шнурок, и он падает на землю. Голем рассыпается у вас на глазах, а кулон достается вам." */),
    DATA_COMPGEN(0x00505244, gArtifactEventPendantOfDeath, "\xcf\xee\xf1\xeb\xe5\x20\xea\xee\xf0\xee\xf2\xea\xee\xe9\x20\xee\xe6\xe5\xf1\xf2\xee\xf7\xe5\xed\xed\xee\xe9\x20\xf1\xf5\xe2\xe0\xf2\xea\xe8\x20\xf1\x20\xed\xe5\xea\xf0\xee\xec\xe0\xed\xf2\xee\xec\x20\xf3\x20\xe2\xe0\xf1\x20\xe2\x20\xf0\xf3\xea\xe0\xf5\x20\xee\xf1\xf2\xe0\xe5\xf2\xf1\xff\x20\xe5\xe3\xee\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xfb\xe9\x20\xea\xf3\xeb\xee\xed\x2e\x20\xc7\xed\xe0\xea\xee\xec\xfb\xe9\x20\xf7\xe0\xf0\xee\xe4\xe5\xe9\x20\xee\xe1\xfa\xff\xf1\xed\xff\xe5\xf2\x20\xe2\xe0\xec\x2c\x20\xf7\xf2\xee\x20\xfd\xf2\xee\xf2\x20\xea\xf3\xeb\xee\xed\x20\xe7\xe0\xf9\xe8\xf9\xe0\xe5\xf2\x20\xed\xe5\xe6\xe8\xf2\xfc\x2c\x20\xf1\xee\xf1\xf2\xee\xff\xf9\xf3\xfe\x20\xe2\x20\xe2\xe0\xf8\xe5\xe9\x20\xe0\xf0\xec\xe8\xe8\x2c\x20\xee\xf2\x20\xf1\xe2\xff\xf2\xee\xe3\xee\x20\xf1\xeb\xee\xe2\xe0\x2e" /* "После короткой ожесточенной схватки с некромантом у вас в руках остается его волшебный кулон. Знакомый чародей объясняет вам, что этот кулон защищает нежить, состоящую в вашей армии, от святого слова." */),
    DATA_COMPGEN(0x00505310, gArtifactEventWandOfNegation, "\xcd\xe0\xe2\xf1\xf2\xf0\xe5\xf7\xf3\x20\xe2\xe0\xec\x20\xef\xee\xef\xe0\xe4\xe0\xe5\xf2\xf1\xff\x20\xf1\xf2\xe0\xf0\xfb\xe9\x20\xe4\xf0\xf3\xe3\x2d\xf7\xe0\xf0\xee\xe4\xe5\xe9\x2e\x20\xce\xed\x20\xe2\xf0\xf3\xf7\xe0\xe5\xf2\x20\xe2\xe0\xec\x20\xef\xee\xe4\xe0\xf0\xee\xea\x20\x2d\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xfb\xe9\x20\xe6\xe5\xe7\xeb\x2c\x20\xea\xee\xf2\xee\xf0\xfb\xe9\x20\xe4\xe5\xeb\xe0\xe5\xf2\x20\xed\xe5\xe2\xee\xe7\xec\xee\xe6\xed\xfb\xec\x20\xef\xf0\xe8\xec\xe5\xed\xe5\xed\xe8\xe5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\x20\xf1\xed\xff\xf2\xe8\xe5\x20\xf7\xe0\xf0\x20\xef\xf0\xee\xf2\xe8\xe2\x20\xe2\xe0\xf8\xe8\xf5\x20\xf1\xee\xf0\xe0\xf2\xed\xe8\xea\xee\xe2\x2e" /* "Навстречу вам попадается старый друг-чародей. Он вручает вам подарок - волшебный жезл, который делает невозможным применение заклинания снятие чар против ваших соратников." */),
    DATA_COMPGEN(0x005053bc, gArtifactEventGoldenBow, "\xc2\xfb\x20\xf1\xeb\xf3\xf7\xe0\xe9\xed\xee\x20\xe2\xf1\xf2\xf0\xe5\xf7\xe0\xe5\xf2\xe5\x20\xe7\xed\xe0\xec\xe5\xed\xe8\xf2\xee\xe3\xee\x20\xf1\xf2\xf0\xe5\xeb\xea\xe0\x20\xe8\x20\xef\xf0\xe5\xe4\xeb\xe0\xe3\xe0\xe5\xf2\xe5\x20\xe5\xec\xf3\x20\xf1\xfb\xe3\xf0\xe0\xf2\xfc\x20\xe2\x20\xea\xee\xf1\xf2\xe8\x2e\x20\xce\xed\x20\xf1\xee\xe3\xeb\xe0\xf8\xe0\xe5\xf2\xf1\xff\x20\xe8\x20\xf1\xf2\xe0\xe2\xe8\xf2\x20\xf1\xe2\xee\xe9\x20\xeb\xf3\xea\x20\xef\xf0\xee\xf2\xe8\xe2\x20\xe2\xe0\xf8\xe5\xe3\xee\x20\xea\xee\xed\xff\x2e\x20\xc2\xfb\x20\xe2\xfb\xe8\xe3\xf0\xfb\xe2\xe0\xe5\xf2\xe5\x2e" /* "Вы случайно встречаете знаменитого стрелка и предлагаете ему сыграть в кости. Он соглашается и ставит свой лук против вашего коня. Вы выигрываете." */),
    DATA_COMPGEN(0x00505450, gArtifactEventTelescope, "\xd2\xee\xf0\xe3\xee\xe2\xe5\xf6\x20\xe8\xe7\x20\xe4\xe0\xeb\xe5\xea\xe8\xf5\x20\xe7\xe5\xec\xe5\xeb\xfc\x20\xef\xf0\xe5\xe4\xeb\xe0\xe3\xe0\xe5\xf2\x20\xe2\xe0\xec\x20\xed\xee\xe2\xe5\xe9\xf8\xe5\xe5\x20\xe8\xe7\xee\xe1\xf0\xe5\xf2\xe5\xed\xe8\xe5\x20\xf1\xe2\xee\xe5\xe3\xee\x20\xed\xe0\xf0\xee\xe4\xe0\x20\xe2\x20\xee\xe1\xec\xe5\xed\x20\xed\xe0\x20\xf1\xfa\xe5\xf1\xf2\xed\xfb\xe5\x20\xef\xf0\xe8\xef\xe0\xf1\xfb\x2e\x20\xdd\xf2\xe0\x20\xf8\xf2\xf3\xea\xe0\x2c\x20\xe1\xeb\xe0\xe3\xee\xe4\xe0\xf0\xff\x20\xea\xee\xf2\xee\xf0\xee\xe9\x20\xf3\xe4\xe0\xeb\xe5\xed\xed\xfb\xe5\x20\xef\xf0\xe5\xe4\xec\xe5\xf2\xfb\x20\xea\xe0\xe6\xf3\xf2\xf1\xff\x20\xe1\xeb\xe8\xe6\xe5\x2c\x20\xed\xe0\xe7\xfb\xe2\xe0\xe5\xf2\xf1\xff\x20\xf2\xe5\xeb\xe5\xf1\xea\xee\xef\xee\xec\x2e" /* "Торговец из далеких земель предлагает вам новейшее изобретение своего народа в обмен на съестные припасы. Эта штука, благодаря которой удаленные предметы кажутся ближе, называется телескопом." */),
    DATA_COMPGEN(0x00505510, gArtifactEventStatesmanSQuill, "\xc2\xfb\x20\xef\xee\xec\xee\xe3\xe0\xe5\xf2\xe5\x20\xe4\xe8\xef\xeb\xee\xec\xe0\xf2\xf3\x20\xef\xee\xf7\xe8\xed\xe8\xf2\xfc\x20\xf1\xeb\xee\xec\xe0\xed\xed\xf3\xfe\x20\xee\xf1\xfc\x20\xe2\x20\xe5\xe3\xee\x20\xfd\xea\xe8\xef\xe0\xe6\xe5\x2c\x20\xe8\x20\xe2\x20\xe7\xed\xe0\xea\x20\xe1\xeb\xe0\xe3\xee\xe4\xe0\xf0\xed\xee\xf1\xf2\xe8\x20\xee\xed\x20\xe4\xe0\xf0\xe8\xf2\x20\xe2\xe0\xec\x20\xef\xe5\xf0\xee\x2e\x20\xce\xed\x20\xe3\xee\xe2\xee\xf0\xe8\xf2\x2c\x20\xf7\xf2\xee\x20\xfd\xf2\xee\x20\xef\xe5\xf0\xee\x20\xe7\xe0\xf1\xf2\xe0\xe2\xeb\xff\xe5\xf2\x20\xeb\xfe\xe4\xe5\xe9\x20\xf1\xec\xee\xf2\xf0\xe5\xf2\xfc\x20\xed\xe0\x20\xe2\xe5\xf9\xe8\x20\xe3\xeb\xe0\xe7\xe0\xec\xe8\x20\xe5\xe3\xee\x20\xee\xe1\xeb\xe0\xe4\xe0\xf2\xe5\xeb\xff\x2e" /* "Вы помогаете дипломату починить сломанную ось в его экипаже, и в знак благодарности он дарит вам перо. Он говорит, что это перо заставляет людей смотреть на вещи глазами его обладателя." */),
    DATA_COMPGEN(0x005055cc, gArtifactEventWizardSHat, "\xc2\xfb\x20\xe2\xe8\xe4\xe8\xf2\xe5\x20\xf7\xe0\xf0\xee\xe4\xe5\xff\x2c\x20\xea\xee\xf2\xee\xf0\xfb\xe9\x20\xf3\xe4\xe8\xf0\xe0\xe5\xf2\x20\xee\xf2\x20\xe3\xf0\xe8\xf4\xee\xed\xe0\x2e\x20\xc2\xee\xf2\x20\xee\xed\x20\xf0\xe0\xf1\xef\xe0\xf5\xed\xf3\xeb\x20\xef\xee\xf0\xf2\xe0\xeb\x20\xe8\x20\xf0\xe8\xed\xf3\xeb\xf1\xff\x20\xe2\xed\xf3\xf2\xf0\xfc\x2c\x20\xed\xee\x20\xef\xf0\xe8\x20\xfd\xf2\xee\xec\x20\xe7\xe0\xf6\xe5\xef\xe8\xeb\xf1\xff\x20\xf8\xeb\xff\xef\xee\xe9\x2c\x20\xe8\x20\xee\xed\xe0\x20\xf3\xef\xe0\xeb\xe0\x20\xee\xed\xe0\x20\xed\xe0\x20\xe7\xe5\xec\xeb\xfe\x2e\x20\xc2\xfb\x20\xef\xee\xe4\xed\xe8\xec\xe0\xe5\xf2\xe5\x20\xf8\xeb\xff\xef\xf3\x2c\x20\xee\xf2\xf0\xff\xf5\xe8\xe2\xe0\xe5\xf2\xe5\x20\xe5\xe5\x20\xee\xf2\x20\xef\xfb\xeb\xe8\x20\xe8\x20\xee\xf1\xf2\xe0\xe2\xeb\xff\xe5\xf2\xe5\x20\xf1\xe5\xe1\xe5\x2e" /* "Вы видите чародея, который удирает от грифона. Вот он распахнул портал и ринулся внутрь, но при этом зацепился шляпой, и она упала она на землю. Вы поднимаете шляпу, отряхиваете ее от пыли и оставляете себе." */),
    DATA_COMPGEN(0x0050569c, gArtifactEventPowerRing, "\xc2\xfb\x20\xe7\xe0\xec\xe5\xf7\xe0\xe5\xf2\xe5\x20\xe4\xe5\xf0\xe5\xe2\xee\x2c\x20\xef\xee\xf5\xee\xe6\xe5\xe5\x20\xed\xe0\x20\xf7\xe5\xf0\xed\xee\xea\xed\xe8\xe6\xed\xe8\xea\xe0\x20\xca\xe0\xf0\xed\xee\xf2\xe0\x2e\x20\xcd\xe0\x20\xee\xe4\xed\xee\xe9\x20\xe8\xe7\x20\xe5\xe3\xee\x20\xe2\xe5\xf2\xee\xea\x20\xf1\xe2\xe5\xf0\xea\xe0\xe5\xf2\x20\xea\xee\xeb\xfc\xf6\xee\x2e\x20\xc2\xfb\x20\xe2\xf1\xe5\x20\xf0\xe0\xe2\xed\xee\x20\xed\xe8\xf7\xe5\xec\x20\xed\xe5\x20\xec\xee\xe6\xe5\xf2\xe5\x20\xe5\xec\xf3\x20\xef\xee\xec\xee\xf7\xfc\x2c\x20\xe8\x20\xef\xee\xfd\xf2\xee\xec\xf3\x20\xe7\xe0\xe1\xe8\xf0\xe0\xe5\xf2\xe5\x20\xea\xee\xeb\xfc\xf6\xee\x20\xf1\xe5\xe1\xe5\x2e" /* "Вы замечаете дерево, похожее на чернокнижника Карнота. На одной из его веток сверкает кольцо. Вы все равно ничем не можете ему помочь, и поэтому забираете кольцо себе." */),
    DATA_COMPGEN(0x00505744, gArtifactEventAmmoCart, "\xc2\xe0\xf8\xe5\x20\xe2\xed\xe8\xec\xe0\xed\xe8\xe5\x20\xef\xf0\xe8\xe2\xeb\xe5\xea\xe0\xe5\xf2\x20\xef\xee\xe2\xee\xe7\xea\xe0\x20\xf1\x20\xe1\xee\xe5\xef\xf0\xe8\xef\xe0\xf1\xe0\xec\xe8\x2c\x20\xf1\xf2\xee\xff\xf9\xe0\xff\x20\xef\xee\xf1\xf0\xe5\xe4\xe8\x20\xef\xee\xeb\xff\x2c\x20\xe3\xe4\xe5\x20\xea\xee\xe3\xe4\xe0\x2d\xf2\xee\x20\xe3\xf0\xe5\xec\xe5\xeb\xe0\x20\xe1\xe8\xf2\xe2\xe0\x2e\x20\xd3\xe1\xe5\xe4\xe8\xe2\xf8\xe8\xf1\xfc\x2c\x20\xf7\xf2\xee\x20\xee\xed\xe0\x20\xe2\x20\xf5\xee\xf0\xee\xf8\xe5\xec\x20\xf1\xee\xf1\xf2\xee\xff\xed\xe8\xe8\x2c\x20\xe2\xfb\x20\xef\xf0\xe8\xf1\xee\xe5\xe4\xe8\xed\xff\xe5\xf2\xe5\x20\xe5\xe5\x20\xea\x20\xf1\xe2\xee\xe5\xec\xf3\x20\xee\xe1\xee\xe7\xf3\x2e" /* "Ваше внимание привлекает повозка с боеприпасами, стоящая посреди поля, где когда-то гремела битва. Убедившись, что она в хорошем состоянии, вы присоединяете ее к своему обозу." */),
    DATA_COMPGEN(0x005057f4, gArtifactEventTaxLien, "\xc2\xe0\xf8\xe0\x20\xed\xe0\xeb\xee\xe3\xee\xe2\xe0\xff\x20\xe4\xe5\xea\xeb\xe0\xf0\xe0\xf6\xe8\xff\x20\xef\xf0\xe5\xe2\xfb\xf1\xe8\xeb\xe0\x20\xef\xf0\xe8\xe4\xe5\xeb\xfb\x2e\x20\xcc\xfb\xf2\xe0\xf0\xfc\x20\xf1\xe6\xe0\xeb\xe8\xeb\xf1\xff\x20\xed\xe0\xe4\x20\xe2\xe0\xec\xe8\x20\xe8\x20\xf1\xee\xe3\xeb\xe0\xf1\xe8\xeb\xf1\xff\x20\xe5\xe6\xe5\xe4\xed\xe5\xe2\xed\xee\x20\xef\xee\xeb\xf3\xf7\xe0\xf2\xfc\x20\xee\xf2\x20\xe2\xe0\xf1\x20\xe2\xf1\xe5\xe3\xee\x20\xef\xee\x20\x32\x35\x30\x20\xe7\xee\xeb\xee\xf2\xfb\xf5\x2e" /* "Ваша налоговая декларация превысила приделы. Мытарь сжалился над вами и согласился ежедневно получать от вас всего по 250 золотых." */),
    DATA_COMPGEN(0x00505878, gArtifactEventHideousMask, "\xc2\xfb\x20\xe2\xf1\xea\xf0\xfb\xeb\xe8\x20\xec\xee\xe3\xe8\xeb\xf3\x20\xd1\xe8\xed\xf4\xe8\xeb\xe8\xff\x20\xc3\xe0\xf0\xe4\xee\xeb\xe0\xe4\xe0\x2c\x20\xe7\xed\xe0\xec\xe5\xed\xe8\xf2\xee\xe3\xee\x20\xf7\xe5\xf0\xed\xee\xea\xed\xe8\xe6\xed\xe8\xea\xe0\x2c\x20\xe8\x20\xed\xe0\xf5\xee\xe4\xe8\xf2\xe5\x20\xe2\x20\xed\xe5\xe9\x20\xec\xe0\xf1\xea\xf3\x2e\x20\xcd\xe0\xe4\xe5\xe2\x20\xe5\xe5\x2c\x20\xe2\xe0\xf8\xe5\x20\xeb\xe8\xf6\xee\x20\xe8\xf1\xea\xe0\xe6\xe0\xe5\xf2\x20\xe3\xf0\xe8\xec\xe0\xf1\xe0\x20\xf3\xe6\xe0\xf1\xe0\x2e\x20\xc2\xe8\xe4\xe8\xec\xee\x20\xe2\xe0\xec\x20\xe4\xee\xf1\xf2\xe0\xeb\xf1\xff\x20\xec\xe0\xf1\xea\xe0\x20\xc3\xf0\xee\xec\xeb\xe0\xea\xe0\x20\xc3\xf0\xe8\xed\xe0\x2e\x20\xd2\xe5\xef\xe5\xf0\xfc\x20\xee\xf2\x20\xed\xe5\xe5\x20\xed\xe5\x20\xe8\xe7\xe1\xe0\xe2\xe8\xf2\xfc\xf1\xff\x21" /* "Вы вскрыли могилу Синфилия Гардолада, знаменитого чернокнижника, и находите в ней маску. Надев ее, ваше лицо искажает гримаса ужаса. Видимо вам достался маска Громлака Грина. Теперь от нее не избавиться!" */),
    DATA_COMPGEN(0x00505944, gArtifactEventEndlessPouchOfSulfur, "\xc2\xfb\x20\xef\xee\xf1\xe5\xf9\xe0\xe5\xf2\xe5\x20\xe0\xeb\xf5\xe8\xec\xe8\xea\xe0\x2c\x20\xea\xee\xf2\xee\xf0\xfb\xe9\x20\xef\xf0\xe8\x20\xe2\xe8\xe4\xe5\x20\xe2\xe0\xf8\xe5\xe9\x20\xe0\xf0\xec\xe8\xe8\x20\xed\xe5\xe7\xe0\xec\xe5\xe4\xeb\xe8\xf2\xe5\xeb\xfc\xed\xee\x20\xef\xf0\xe8\xe7\xed\xe0\xe5\xf2\x20\xe2\xe0\xf1\x20\xe4\xee\xf1\xf2\xee\xe9\xed\xe5\xe9\xf8\xe8\xec\x20\xe8\xe7\x20\xe4\xee\xf1\xf2\xee\xe9\xed\xfb\xf5\x2e\x20\xcd\xee\xe2\xfb\xe9\x20\xef\xee\xe4\xe4\xe0\xed\xed\xfb\xe9\x20\xe4\xe0\xf0\xe8\xf2\x20\xe2\xe0\xec\x20\xe1\xe5\xe7\xe4\xee\xed\xed\xf3\xfe\x20\xf1\xf3\xec\xea\xf3\x20\xf1\xe5\xf0\xfb\x2c\x20\xea\xee\xf2\xee\xf0\xe0\xff\x20\xe2\xe0\xec\x20\xee\xf7\xe5\xed\xfc\x20\xe4\xe0\xe6\xe5\x20\xef\xf0\xe8\xe3\xee\xe4\xe8\xf2\xf1\xff\x2e" /* "Вы посещаете алхимика, который при виде вашей армии незамедлительно признает вас достойнейшим из достойных. Новый подданный дарит вам бездонную сумку серы, которая вам очень даже пригодится." */),
    DATA_COMPGEN(0x00505a04, gArtifactEventEndlessVialOfMercury, "\xc2\xfb\x20\xe4\xe5\xeb\xe0\xe5\xf2\xe5\x20\xea\xee\xf0\xee\xf2\xea\xe8\xe9\x20\xef\xf0\xe8\xe2\xe0\xeb\x20\xe2\x20\xe1\xe0\xf8\xed\xe5\x20\xf7\xe0\xf0\xee\xe4\xe5\xff\x2c\x20\xef\xee\xea\xe8\xed\xf3\xf2\xee\xe9\x20\xf5\xee\xe7\xff\xe8\xed\xee\xec\x2c\x20\xe8\x20\xed\xe0\xf5\xee\xe4\xe8\xf2\xe5\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xfb\xe9\x20\xf1\xee\xf1\xf3\xe4\x20\xf1\x20\xf0\xf2\xf3\xf2\xfc\xfe\x2c\x20\xf1\xee\xe4\xe5\xf0\xe6\xe8\xec\xee\xe5\x20\xea\xee\xf2\xee\xf0\xee\xe3\xee\x20\xed\xe8\xea\xee\xe3\xe4\xe0\x20\xed\xe5\x20\xea\xee\xed\xf7\xe0\xe5\xf2\xf1\xff\x2e\x20\xdd\xf2\xee\x20\xe6\xe5\x20\xed\xe0\xf1\xf2\xee\xff\xf9\xe5\xe5\x20\xf1\xee\xea\xf0\xee\xe2\xe8\xf9\xe5\x21" /* "Вы делаете короткий привал в башне чародея, покинутой хозяином, и находите волшебный сосуд с ртутью, содержимое которого никогда не кончается. Это же настоящее сокровище!" */),
    DATA_COMPGEN(0x00505ab0, gArtifactEventEndlessPouchOfGems, "\xcf\xee\xf1\xeb\xe5\x20\xea\xee\xf0\xee\xf2\xea\xee\xe3\xee\x20\xeb\xe8\xe2\xed\xff\x20\xed\xe0\x20\xed\xe5\xe1\xe5\x20\xef\xee\xff\xe2\xeb\xff\xe5\xf2\xf1\xff\x20\xf0\xe0\xe4\xf3\xe3\xe0\x2e\x20\xc7\xe0\xec\xe5\xf2\xe8\xe2\x20\xec\xe5\xf1\xf2\xee\x2c\x20\xe3\xe4\xe5\x20\xee\xed\xe0\x20\xf3\xef\xe8\xf0\xe0\xe5\xf2\xf1\xff\x20\xe2\x20\xe7\xe5\xec\xeb\xfe\x2c\x20\xe2\xfb\x20\xed\xe0\xf5\xee\xe4\xe8\xf2\xe5\x20\xf2\xe0\xec\x20\xe3\xee\xf0\xf8\xee\xea\x20\xe7\xee\xeb\xee\xf2\xe0\x2e\x20\xc5\xe3\xee\x20\xf5\xee\xe7\xff\xe8\xed\x2c\x20\xec\xe0\xeb\xe5\xed\xfc\xea\xe8\xe9\x20\xfd\xeb\xfc\xf4\x2c\x20\xef\xf0\xe5\xe4\xeb\xe0\xe3\xe0\xe5\xf2\x20\xe2\xe7\xe0\xec\xe5\xed\x20\xe1\xe5\xe7\xe4\xee\xed\xed\xf3\xfe\x20\xf1\xf3\xec\xf3\x20\xf1\xe0\xec\xee\xf6\xe2\xe5\xf2\xee\xe2\x2e" /* "После короткого ливня на небе появляется радуга. Заметив место, где она упирается в землю, вы находите там горшок золота. Его хозяин, маленький эльф, предлагает взамен бездонную суму самоцветов." */),
    DATA_COMPGEN(0x00505b74, gArtifactEventEndlessCordOfWood, "\xc2\xfb\x20\xee\xf1\xf2\xe0\xed\xe0\xe2\xeb\xe8\xe2\xe0\xe5\xf2\xe5\xf1\xfc\x20\xed\xe0\x20\xee\xf2\xe4\xfb\xf5\x20\xe8\x20\xf0\xe0\xe7\xe2\xee\xe4\xe8\xf2\xe5\x20\xea\xee\xf1\xf2\xe5\xf0\x2e\x20\xcd\xe5\xef\xee\xe4\xe0\xeb\xe5\xea\xf3\x20\xeb\xe5\xe6\xe8\xf2\x20\xea\xf3\xf7\xe0\x20\xe4\xf0\xee\xe2\x2e\x20\xc2\xfb\x20\xe1\xe5\xf0\xe5\xf2\xe5\x20\xee\xe4\xed\xee\x20\xef\xee\xeb\xe5\xed\xee\x20\xe7\xe0\x20\xe4\xf0\xf3\xe3\xe8\xec\x2c\x20\xed\xee\x20\xea\xf3\xf7\xe0\x20\xed\xe5\x20\xf3\xec\xe5\xed\xfc\xf8\xe0\xe5\xf2\xf1\xff\x2e\x20\xc2\xfb\x20\xf1\x20\xf0\xe0\xe4\xee\xf1\xf2\xfc\xfe\x20\xef\xee\xed\xe8\xec\xe0\xe5\xf2\xe5\x2c\x20\xf7\xf2\xee\x20\xe4\xf0\xee\xe2\xe0\x20\xe7\xe0\xf7\xe0\xf0\xee\xe2\xe0\xed\xfb\x2c\x20\xe8\x20\xe7\xe0\xe1\xe8\xf0\xe0\xe5\xf2\xe5\x20\xe8\xf5\x20\xf1\xe5\xe1\xe5\x2e" /* "Вы останавливаетесь на отдых и разводите костер. Неподалеку лежит куча дров. Вы берете одно полено за другим, но куча не уменьшается. Вы с радостью понимаете, что дрова зачарованы, и забираете их себе." */),
    DATA_COMPGEN(0x00505c40, gArtifactEventEndlessCartOfOre, "\xc2\xfb\x20\xed\xe0\xf5\xee\xe4\xe8\xf2\xe5\x20\xea\xf3\xe7\xed\xe8\xf6\xf3\x20\xe3\xee\xe1\xeb\xe8\xed\xee\xe2\x2c\x20\xe3\xe4\xe5\x20\xee\xed\xe8\x20\xea\xf3\xfe\xf2\x20\xee\xf0\xf3\xe6\xe8\xe5\x2e\x20\xd1\x20\xe2\xee\xe8\xed\xf1\xf2\xe2\xe5\xed\xed\xfb\xec\x20\xea\xeb\xe8\xf7\xe5\xec\x2c\x20\xe2\xe0\xf8\xe8\x20\xe2\xee\xe8\xed\xfb\x20\xed\xe0\xef\xe0\xe4\xe0\xfe\xf2\x20\xed\xe0\x20\xe8\xf5\x20\xeb\xe0\xe3\xe5\xf0\xfc\x20\xe8\x20\xf3\xe1\xe8\xe2\xe0\xfe\xf2\x20\xe2\xf1\xe5\xf5\x20\xe2\xf0\xe0\xe3\xee\xe2\x2e\x20\xce\xf1\xec\xee\xf2\xf0\xe5\xe2\x20\xf2\xf0\xee\xf4\xe5\xe8\x2c\x20\xe2\xfb\x20\xee\xe1\xed\xe0\xf0\xf3\xe6\xe8\xe2\xe0\xe5\xf2\xe5\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xf3\xfe\x20\xe2\xe0\xe3\xee\xed\xe5\xf2\xea\xf3\x20\xf1\x20\xf0\xf3\xe4\xee\xe9\x2e" /* "Вы находите кузницу гоблинов, где они куют оружие. С воинственным кличем, ваши воины нападают на их лагерь и убивают всех врагов. Осмотрев трофеи, вы обнаруживаете волшебную вагонетку с рудой." */),
    DATA_COMPGEN(0x00505d04, gArtifactEventEndlessPouchOfCrystal, "\xd3\xea\xf0\xfb\xe2\xf8\xe8\xf1\xfc\x20\xee\xf2\x20\xe1\xf3\xf0\xe8\x20\xe2\x20\xed\xe5\xe1\xee\xeb\xfc\xf8\xee\xe9\x20\xef\xe5\xf9\xe5\xf0\xea\xe5\x2c\x20\xe2\xfb\x20\xe7\xe0\xec\xe5\xf7\xe0\xe5\xf2\xe5\x20\xe2\x20\xf3\xe3\xeb\xf3\x20\xe4\xf0\xf3\xe7\xf3\x20\xea\xf0\xe8\xf1\xf2\xe0\xeb\xeb\xee\xe2\x2e\x20\xc2\xfb\x20\xee\xf2\xeb\xe0\xec\xfb\xe2\xe0\xe5\xf2\xe5\x20\xea\xf3\xf1\xee\xea\x2c\x20\xe0\x20\xed\xe0\x20\xe5\xe3\xee\x20\xec\xe5\xf1\xf2\xe5\x20\xe2\xfb\xf0\xe0\xf1\xf2\xe0\xe5\xf2\x20\xed\xee\xe2\xfb\xe9\x20\xea\xf0\xe8\xf1\xf2\xe0\xeb\xeb\x2e\x20\xc2\xfb\x20\xe7\xe0\xe1\xe5\xf0\xe0\xe5\xf2\xe5\x20\xfd\xf2\xee\x20\xf1\xee\xea\xf0\xee\xe2\xe8\xf9\xe5\x20\xf1\x20\xf1\xee\xe1\xee\xe9\x2e" /* "Укрывшись от бури в небольшой пещерке, вы замечаете в углу друзу кристаллов. Вы отламываете кусок, а на его месте вырастает новый кристалл. Вы забераете это сокровище с собой." */),
    DATA_COMPGEN(0x00505db4, gArtifactEventSpikedHelm, "\xcd\xe5\xe1\xee\xeb\xfc\xf8\xee\xe9\x20\xee\xf2\xf0\xff\xe4\x20\xee\xf0\xea\xee\xe2\x20\xed\xe0\xef\xe0\xe4\xe0\xe5\xf2\x20\xed\xe0\x20\xe2\xe0\xf8\xf3\x20\xe0\xf0\xec\xe8\xfe\x2e\x20\xc2\xfb\x20\xe1\xe5\xe7\x20\xf2\xf0\xf3\xe4\xe0\x20\xee\xf2\xe1\xe8\xe2\xe0\xe5\xf2\xe5\x20\xe0\xf2\xe0\xea\xf3\x2e\x20\xcd\xe0\x20\xf2\xe5\xeb\xe5\x20\xee\xe4\xed\xee\xe3\xee\x20\xe8\xe7\x20\xed\xe0\xef\xe0\xe4\xe0\xe2\xf8\xe8\xf5\x20\xe2\xfb\x20\xe2\xe8\xe4\xe8\xf2\xe5\x20\xe1\xeb\xe5\xf1\xf2\xff\xf9\xe8\xe9\x20\xf8\xeb\xe5\xec\x20\xf1\x20\xf8\xe8\xef\xe0\xec\xe8\x2e" /* "Небольшой отряд орков нападает на вашу армию. Вы без труда отбиваете атаку. На теле одного из нападавших вы видите блестящий шлем с шипами." */),
    DATA_COMPGEN(0x00505e40, gArtifactEventSpikedShield, "\xc2\xfb\x20\xef\xf0\xe8\xe1\xeb\xe8\xe6\xe0\xe5\xf2\xe5\xf1\xfc\x20\xea\x20\xec\xee\xf1\xf2\xf3\x20\xf7\xe5\xf0\xe5\xe7\x20\xe3\xeb\xf3\xe1\xee\xea\xe8\xe9\x20\xee\xe2\xf0\xe0\xe3\x2e\x20\xcd\xe5\xee\xe6\xe8\xe4\xe0\xed\xed\xee\x20\xe8\xe7\x2d\xef\xee\xe4\x20\xec\xee\xf1\xf2\xe0\x20\xef\xee\xff\xe2\xeb\xff\xe5\xf2\xf1\xff\x20\xf2\xf0\xee\xeb\xeb\xfc\x20\xe8\x20\xf2\xf0\xe5\xe1\xf3\xe5\xf2\x20\xef\xeb\xe0\xf2\xf3\x20\xe7\xe0\x20\xef\xf0\xee\xf5\xee\xe4\x2e\x20\xcf\xee\xf1\xeb\xe5\x20\xee\xf2\xea\xe0\xe7\xe0\x2c\x20\xf2\xf0\xee\xeb\xeb\xfc\x20\xed\xe0\xef\xe0\xe4\xe0\xe5\xf2\x20\xed\xe0\x20\xe2\xe0\xf1\x2e\x20\xd3\xe1\xe8\xe2\x20\xe5\xe3\xee\x2c\x20\xe2\xfb\x20\xe7\xe0\xe1\xe8\xf0\xe0\xe5\xf2\xe5\x20\xf1\xe5\xe1\xe5\x20\xe5\xe3\xee\x20\xf8\xe8\xf2\x20\xf1\x20\xf8\xe8\xef\xe0\xec\xe8\x2e" /* "Вы приближаетесь к мосту через глубокий овраг. Неожиданно из-под моста появляется тролль и требует плату за проход. После отказа, тролль нападает на вас. Убив его, вы забираете себе его шит с шипами." */),
    DATA_COMPGEN(0x00505f08, gArtifactEventWhitePearl, "\xc2\xfb\x20\xef\xe5\xf0\xe5\xf1\xe5\xea\xe0\xe5\xf2\xe5\x20\xef\xe5\xf0\xe5\xf1\xee\xf5\xf8\xe5\xe5\x20\xf1\xee\xeb\xff\xed\xee\xe5\x20\xee\xe7\xe5\xf0\xee\x2c\x20\xe8\x20\xe2\xe4\xf0\xf3\xe3\x20\xf1\xf0\xe5\xe4\xe8\x20\xee\xe1\xeb\xee\xec\xea\xee\xe2\x20\xf0\xe0\xea\xf3\xf8\xe5\xea\x20\xe8\x20\xea\xf3\xf1\xea\xee\xe2\x20\xea\xee\xf0\xe0\xeb\xeb\xe0\x20\xe7\xe0\xec\xe5\xf7\xe0\xe5\xf2\xe5\x20\xe2\xe5\xeb\xe8\xea\xee\xeb\xe5\xef\xed\xf3\xfe\x20\xe1\xe5\xeb\xf3\xfe\x20\xe6\xe5\xec\xf7\xf3\xe6\xe8\xed\xf3\x2e" /* "Вы пересекаете пересохшее соляное озеро, и вдруг среди обломков ракушек и кусков коралла замечаете великолепную белую жемчужину." */),
    DATA_COMPGEN(0x00505f8c, gArtifactEventBlackPearl, "\xd1\xeb\xf3\xf5\xe8\x20\xee\xe1\x20\xee\xe3\xf0\xee\xec\xed\xee\xec\x20\xe3\xf0\xe8\xf4\xee\xed\xe5\x2c\x20\xed\xe0\xe3\xee\xed\xff\xfe\xf9\xe5\xec\x20\xf3\xe6\xe0\xf1\x20\xed\xe0\x20\xe2\xf1\xfe\x20\xee\xea\xf0\xf3\xe3\xf3\x2c\x20\xef\xf0\xe8\xe2\xee\xe4\xff\xf2\x20\xe2\xe0\xf1\x20\xe2\x20\xe5\xe3\xee\x20\xeb\xee\xe3\xee\xe2\xee\x2e\x20\xc6\xe5\xf1\xf2\xee\xea\xe0\xff\x20\xf1\xf5\xe2\xe0\xf2\xea\xe0\x20\xe7\xe0\xea\xe0\xed\xf7\xe8\xe2\xe0\xe5\xf2\xf1\xff\x20\xe2\xe0\xf8\xe5\xe9\x20\xef\xee\xe1\xe5\xe4\xee\xe9\x2c\x20\xe8\x20\xe2\x20\xee\xef\xf3\xf1\xf2\xe5\xe2\xf8\xe5\xec\x20\xe3\xed\xe5\xe7\xe4\xe5\x20\xe2\xfb\x20\xed\xe0\xf5\xee\xe4\xe8\xf2\xe5\x20\xf7\xe5\xf0\xed\xf3\xfe\x20\xe6\xe5\xec\xf7\xf3\xe6\xe8\xed\xf3\x2e" /* "Слухи об огромном грифоне, нагоняющем ужас на всю округу, приводят вас в его логово. Жестокая схватка заканчивается вашей победой, и в опустевшем гнезде вы находите черную жемчужину." */),
    DATA_COMPGEN(0x00526760, gArtifactEventMagicBook, "" /* "" */),
    DATA_COMPGEN(0x00506044, gArtifactEventERRORArtifact82, "\x45\x52\x52\x4f\x52\x20\x3a\x20\x41\x72\x74\x69\x66\x61\x63\x74\x20\x65\x76\x65\x6e\x74\x20\x38\x32\x2e" /* "ERROR : Artifact event 82." */),
    DATA_COMPGEN(0x00506060, gArtifactEventERRORArtifact83, "\x45\x52\x52\x4f\x52\x20\x3a\x20\x41\x72\x74\x69\x66\x61\x63\x74\x20\x65\x76\x65\x6e\x74\x20\x38\x33\x2e" /* "ERROR : Artifact event 83." */),
    DATA_COMPGEN(0x0050607c, gArtifactEventERRORArtifact84, "\x45\x52\x52\x4f\x52\x20\x3a\x20\x41\x72\x74\x69\x66\x61\x63\x74\x20\x65\x76\x65\x6e\x74\x20\x38\x34\x2e" /* "ERROR : Artifact event 84." */),
    DATA_COMPGEN(0x00506098, gArtifactEventERRORArtifact85, "\x45\x52\x52\x4f\x52\x20\x3a\x20\x41\x72\x74\x69\x66\x61\x63\x74\x20\x65\x76\x65\x6e\x74\x20\x38\x35\x2e" /* "ERROR : Artifact event 85." */),
    DATA_COMPGEN(0x005060b4, gArtifactEventSpellScroll, "\xc2\xfb\x20\xed\xe0\xf8\xeb\xe8\x20\xf0\xe5\xe7\xed\xee\xe9\x20\xeb\xe0\xf0\xe5\xf6\x2c\x20\xe2\x20\xea\xee\xf2\xee\xf0\xee\xec\x20\xf5\xf0\xe0\xed\xe8\xeb\xf1\xff\x20\xe4\xf0\xe5\xe2\xed\xe8\xe9\x20\xf1\xe2\xe8\xf2\xee\xea\x2e\x20\xd0\xf3\xed\xfb\x20\xed\xe0\x20\xeb\xe0\xf0\xf6\xe5\x20\xee\xf7\xe5\xed\xfc\x20\xe4\xf0\xe5\xe2\xed\xe8\xe5\x2e\x20\xd0\xe0\xe7\xe2\xe5\xf0\xed\xf3\xe2\x20\xf1\xe2\xe8\xf2\xee\xea\x2c\x20\xe2\xfb\x20\xef\xee\xf7\xf3\xe2\xf1\xf2\xe2\xee\xe2\xe0\xeb\xe8\x20\xef\xf3\xeb\xfc\xf1\xe0\xf6\xe8\xfe\x20\xec\xe0\xe3\xe8\xf7\xe5\xf1\xea\xe8\xf5\x20\xf1\xe8\xeb\x2e" /* "Вы нашли резной ларец, в котором хранился древний свиток. Руны на ларце очень древние. Развернув свиток, вы почувствовали пульсацию магических сил." */),
    DATA_COMPGEN(0x00506148, gArtifactEventArmOfTheMartyr, "\xce\xe4\xe8\xed\x20\xe8\xe7\x20\xe2\xe0\xf8\xe8\xf5\x20\xe2\xee\xe8\xed\xee\xe2\x20\xef\xee\xe4\xee\xe1\xf0\xe0\xeb\x20\xf1\x20\xe7\xe5\xec\xeb\xe8\x20\xee\xf2\xee\xf0\xe2\xe0\xed\xed\xf3\xfe\x20\xf0\xf3\xea\xf3\x2e\x20\xcd\xe5\xf1\xec\xee\xf2\xf0\xff\x20\xed\xe0\x20\xf2\xee\x2c\x20\xf7\xf2\xee\x20\xf0\xf3\xea\xe0\x20\xe1\xfb\xeb\xe0\x20\xee\xf2\xee\xf0\xe2\xe0\xed\xe0\x20\xee\xf2\x20\xf2\xe5\xeb\xe0\x2c\x20\xee\xed\xe0\x20\xe2\xf1\xe5\x20\xe5\xf9\xe5\x20\xef\xf0\xee\xe4\xee\xeb\xe6\xe0\xeb\xe0\x20\xf8\xe5\xe2\xe5\xeb\xe8\xf2\xfc\xf1\xff\x2e\x20\xc2\xe0\xf8\xe8\x20\xe2\xee\xe8\xed\xfb\x20\xe8\xf1\xef\xfb\xf2\xe0\xeb\xe8\x20\xe2\xe5\xeb\xe8\xea\xee\xe5\x20\xee\xf2\xe2\xf0\xe0\xf9\xe5\xed\xe8\xe5\x20\xea\x20\xfd\xf2\xee\xec\xf3\x20\xef\xf0\xe5\xe4\xec\xe5\xf2\xf3\x2c\x20\xed\xee\x20\xe2\xfb\x20\xed\xe5\x20\xf1\xec\xee\xe3\xeb\xe8\x20\xe7\xe0\xf1\xf2\xe0\xe2\xe8\xf2\xfc\x20\xf1\xe5\xe1\xff\x20\xe2\xfb\xea\xe8\xed\xf3\xf2\xfc\x20\xe5\xe5\x2e" /* "Один из ваших воинов подобрал с земли оторванную руку. Несмотря на то, что рука была оторвана от тела, она все еще продолжала шевелиться. Ваши воины испытали великое отвращение к этому предмету, но вы не смогли заставить себя выкинуть ее." */),
    DATA_COMPGEN(0x00506238, gArtifactEventBreastplateOfAnduran, "\xc2\xfb\x20\xee\xe1\xed\xe0\xf0\xf3\xe6\xe8\xeb\xe8\x20\xf3\xea\xe0\xe7\xe0\xf2\xe5\xeb\xfc\x2c\x20\xed\xe0\x20\xea\xee\xf2\xee\xf0\xee\xec\x20\xe1\xfb\xeb\xee\x20\xed\xe0\xef\xe8\xf1\xe0\xed\xee\x2c\x20\xf7\xf2\xee\x20\xe7\xe4\xe5\xf1\xfc\x20\xef\xee\xea\xee\xe8\xf2\xf1\xff\x20\xe2\xe5\xeb\xe8\xea\xe8\xe9\x20\xc0\xed\xe4\xf3\xf0\xe0\xed\x2e\x20\xcd\xe0\xe4\xef\xe8\xf1\xfc\x20\xec\xee\xeb\xe2\xe8\xeb\xe0\x2c\x20\xf7\xf2\xee\x20\xef\xf0\xe5\xea\xeb\xee\xed\xe8\xe2\xf8\xe8\xe9\x20\xf7\xe5\xeb\xee\x20\xef\xe5\xf0\xe5\xe4\x20\xec\xee\xe3\xe8\xeb\xee\xe9\x20\xe1\xf3\xe4\xe5\xf2\x20\xe2\xee\xe7\xed\xe0\xe3\xf0\xe0\xe6\xe4\xe5\xed\x2e\x20\xc2\xfb\x20\xef\xee\xf1\xf2\xf3\xef\xe8\xeb\xe8\x2c\x20\xea\xe0\xea\x20\xf2\xee\xe3\xee\x20\xf2\xf0\xe5\xe1\xee\xe2\xe0\xeb\xee\xf1\xfc\x2c\x20\xe8\x20\xef\xee\xeb\xf3\xf7\xe8\xeb\xe8\x20\xe2\x20\xed\xe0\xe3\xf0\xe0\xe4\xf3\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xfb\xe9\x20\xe4\xee\xf1\xef\xe5\xf5\x2e" /* "Вы обнаружили указатель, на котором было написано, что здесь покоится великий Андуран. Надпись молвила, что преклонивший чело перед могилой будет вознагражден. Вы поступили, как того требовалось, и получили в награду волшебный доспех." */),
    DATA_COMPGEN(0x00506324, gArtifactEventBroachOfShielding, "\xc4\xee\xe1\xf0\xe0\xff\x20\xea\xee\xeb\xe4\xf3\xed\xfc\xff\x20\xf1\xee\xf7\xeb\xe0\x2c\x20\xf7\xf2\xee\x20\xe2\xe0\xf8\xe0\x20\xe0\xf0\xec\xe8\xff\x20\xef\xeb\xee\xf5\xee\x20\xe7\xe0\xf9\xe8\xf9\xe5\xed\xe0\x20\xe8\x20\xe4\xe0\xf0\xee\xe2\xe0\xeb\xe0\x20\xe2\xe0\xec\x20\xf1\xe2\xee\xfe\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xf3\xfe\x20\xe1\xf0\xee\xf8\xfc\x2e" /* "Добрая колдунья сочла, что ваша армия плохо защищена и даровала вам свою волшебную брошь." */),
    DATA_COMPGEN(0x00506380, gArtifactEventBattleGarbOfAnduran, "\xc2\xfb\x20\xea\xf3\xef\xe8\xeb\xe8\x20\xf3\x20\xe1\xe5\xe4\xed\xff\xea\xe0\x20\xff\xf9\xe8\xea\x20\xf1\xee\x20\xe2\xf1\xff\xea\xe8\xec\x20\xe1\xe0\xf0\xe0\xf5\xeb\xee\xec\x20\xe8\x20\xed\xe0\x20\xf1\xe2\xee\xe5\x20\xf3\xe4\xe8\xe2\xeb\xe5\xed\xe8\xe5\x20\xed\xe0\xf8\xeb\xe8\x20\xe2\x20\xed\xe5\xec\x20\xf2\xf0\xe8\x20\xe2\xe5\xf9\xe8\x20\xe8\xe7\x20\xe1\xee\xe5\xe2\xee\xe3\xee\x20\xee\xe4\xe5\xff\xed\xe8\xff\x20\xc0\xed\xe4\xf3\xf0\xe0\xed\xe0\x21\x20\xc2\xee\xf2\x20\xfd\xf2\xee\x20\xf3\xe4\xe0\xf7\xe0\x21" /* "Вы купили у бедняка ящик со всяким барахлом и на свое удивление нашли в нем три вещи из боевого одеяния Андурана! Вот это удача!" */),
    DATA_COMPGEN(0x00506404, gArtifactEventCrystalBall, "\xc2\xfb\x20\xef\xf0\xee\xf5\xee\xe4\xe8\xeb\xe8\x20\xec\xe8\xec\xee\x20\xf2\xf0\xf3\xef\xef\xfb\x20\xe1\xf0\xee\xe4\xff\xf7\xe8\xf5\x20\xe0\xea\xf2\xe5\xf0\xee\xe2\x2e\x20\xce\xed\xe8\x20\xef\xee\xef\xf0\xee\xf1\xe8\xeb\xe8\x20\xe2\xe0\xf1\x20\xf1\xf2\xe0\xed\xf6\xe5\xe2\xe0\xf2\xfc\x20\xf0\xf3\xec\xe0\x2d\xe1\xf3\xf2\xf3\x2e\x20\xc2\xfb\x20\xe8\xf1\xef\xee\xeb\xed\xe8\xeb\xe8\x20\xed\xe5\xf1\xea\xee\xeb\xfc\xea\xee\x20\xef\xf0\xee\xe8\xe7\xe2\xee\xeb\xfc\xed\xfb\xf5\x20\xe4\xe2\xe8\xe6\xe5\xed\xe8\xe9\x2c\x20\xe8\x20\xee\xed\xe8\x20\xe7\xe0\x20\xf5\xf0\xe0\xe1\xf0\xee\xf1\xf2\xfc\x20\xe4\xe0\xf0\xee\xe2\xe0\xeb\xe8\x20\xe2\xe0\xec\x20\xea\xf0\xe8\xf1\xf2\xe0\xeb\xfc\xed\xfb\xe9\x20\xf8\xe0\xf0\x2e" /* "Вы проходили мимо труппы бродячих актеров. Они попросили вас станцевать рума-буту. Вы исполнили несколько произвольных движений, и они за храбрость даровали вам кристальный шар." */),
    DATA_COMPGEN(0x005064b8, gArtifactEventHeartOfFire, "\xc2\xfb\x20\xef\xee\xef\xe0\xeb\xe8\x20\xed\xe0\x20\xed\xe5\xe4\xe0\xe2\xed\xee\x20\xf1\xe3\xee\xf0\xe5\xe2\xf8\xf3\xfe\x20\xef\xee\xeb\xff\xed\xf3\x2e\x20\xcf\xee\xf1\xf0\xe5\xe4\xe8\x20\xef\xee\xeb\xff\xed\xfb\x2c\x20\xed\xe0\x20\xea\xe0\xec\xed\xe5\x20\xf1\xf2\xee\xff\xeb\x20\xf1\xee\xf1\xf3\xe4\x2c\x20\xe2\x20\xea\xee\xf2\xee\xf0\xee\xec\x20\xf1\xe8\xe4\xe5\xeb\x20\xee\xe3\xed\xe5\xed\xed\xfb\xe9\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb\x2e\x20\xc2\xfb\x20\xf0\xe5\xf8\xe8\xeb\xe8\x20\xe2\xe7\xff\xf2\xfc\x20\xf1\x20\xf1\xee\xe1\xee\xe9\x20\xfd\xf2\xf3\x20\xe4\xe8\xea\xee\xe2\xe8\xed\xed\xf3\xfe\x20\xed\xe0\xf5\xee\xe4\xea\xf3\x2e" /* "Вы попали на недавно сгоревшую поляну. Посреди поляны, на камне стоял сосуд, в котором сидел огненный элементал. Вы решили взять с собой эту диковинную находку." */),
    DATA_COMPGEN(0x0050655c, gArtifactEventHeartOfIce, "\xcd\xe5\xee\xe6\xe8\xe4\xe0\xed\xed\xee\x20\xe2\xe0\xf1\x20\xf1\xea\xee\xe2\xe0\xeb\x20\xef\xf0\xee\xed\xe7\xe8\xf2\xe5\xeb\xfc\xed\xfb\xe9\x20\xf5\xee\xeb\xee\xe4\x2e\x20\xce\xf2\x20\xed\xe5\xee\xe6\xe8\xe4\xe0\xed\xed\xee\xe3\xee\x20\xf8\xee\xea\xe0\x20\xe2\xfb\x20\xf3\xef\xe0\xeb\xe8\x20\xf1\x20\xea\xee\xed\xff\x20\xed\xe0\x20\xe7\xe5\xec\xeb\xfe\x2e\x20\xcc\xe8\xec\xee\x20\xe2\xe0\xf1\x20\xef\xf0\xee\xec\xf7\xe0\xeb\xf1\xff\x20\xee\xe3\xf0\xee\xec\xed\xfb\xe9\x20\xeb\xe5\xe4\xff\xed\xee\xe9\x20\xe3\xe8\xe3\xe0\xed\xf2\x2e\x20\xc2\x20\xf1\xef\xe5\xf8\xea\xe5\x20\xee\xed\x20\xee\xe1\xf0\xee\xed\xe8\xeb\x20\xee\xe4\xed\xf3\x20\xf6\xe5\xed\xed\xf3\xfe\x20\xe2\xe5\xf9\xfc\x21" /* "Неожиданно вас сковал пронзительный холод. От неожиданного шока вы упали с коня на землю. Мимо вас промчался огромный ледяной гигант. В спешке он обронил одну ценную вещь!" */),
    DATA_COMPGEN(0x00506608, gArtifactEventHelmetOfAnduran, "\xc2\xfb\x20\xe7\xe0\xec\xe5\xf2\xe8\xeb\xe8\x20\xf1\xe2\xe5\xf0\xea\xe0\xfe\xf9\xe8\xe9\x20\xee\xe1\xfa\xe5\xea\xf2\x20\xed\xe5\xe2\xe4\xe0\xeb\xe5\xea\xe5\x2e\x20\xc2\xfb\x20\xef\xee\xf1\xeb\xe0\xeb\xe8\x20\xee\xe4\xed\xee\xe3\xee\x20\xe8\xe7\x20\xe2\xe0\xf8\xe8\xf5\x20\xe2\xee\xe8\xed\xee\xe2\x20\xef\xee\xf1\xec\xee\xf2\xf0\xe5\xf2\xfc\x2c\x20\xf7\xf2\xee\x20\xfd\xf2\xee\x20\xf2\xe0\xec\x2e\x20\xce\xed\x20\xe2\xe5\xf0\xed\xf3\xeb\xf1\xff\x20\xf1\x20\xe7\xee\xeb\xee\xf2\xfb\xec\x20\xf8\xeb\xe5\xec\xee\xec\x20\xe2\x20\xf0\xf3\xea\xe0\xf5\x2c\x20\xea\xee\xf2\xee\xf0\xfb\xe9\x20\xee\xea\xe0\xe7\xe0\xeb\xf1\xff\x20\xed\xe8\x20\xf7\xe5\xec\x20\xe8\xed\xfb\xec\x2c\x20\xea\xe0\xea\x20\xf8\xeb\xe5\xec\xee\xec\x20\xeb\xe5\xe3\xe5\xed\xe4\xe0\xf0\xed\xee\xe3\xee\x20\xc0\xed\xe4\xf3\xf0\xe0\xed\xe0\x21" /* "Вы заметили сверкающий объект невдалеке. Вы послали одного из ваших воинов посмотреть, что это там. Он вернулся с золотым шлемом в руках, который оказался ни чем иным, как шлемом легендарного Андурана!" */),
    DATA_COMPGEN(0x005066d4, gArtifactEventHolyHammer, "\xc2\xfb\x20\xf1\xf2\xe0\xeb\xe8\x20\xf1\xe2\xe8\xe4\xe5\xf2\xe5\xeb\xe5\xec\x20\xef\xee\xe5\xe4\xe8\xed\xea\xe0\x2c\x20\xe2\x20\xea\xee\xf2\xee\xf0\xee\xec\x20\xef\xe0\xeb\xe0\xe4\xe8\xed\x20\xe1\xfb\xeb\x20\xf1\xec\xe5\xf0\xf2\xe5\xeb\xfc\xed\xee\x20\xf0\xe0\xed\xe5\xed\x20\xee\xf2\xf0\xff\xe4\xee\xec\x20\xe7\xee\xec\xe1\xe8\x2e\x20\xce\xed\x20\xef\xee\xef\xf0\xee\xf1\xe8\xeb\x20\xe2\xe0\xf1\x20\xe2\xe7\xff\xf2\xfc\x20\xe5\xe3\xee\x20\xec\xee\xeb\xee\xf2\x20\xe8\x20\xe7\xe0\xe2\xe5\xf0\xf8\xe8\xf2\xfc\x20\xed\xe0\xf7\xe0\xf2\xee\xe5\x20\xe8\xec\x20\xe4\xe5\xeb\xee\x2e\x20\xd3\xe1\xe8\xe2\x20\xe7\xee\xec\xe1\xe8\x20\xe2\xfb\x20\xef\xee\xe2\xe5\xf1\xe8\xeb\xe8\x20\xec\xee\xeb\xee\xf2\x20\xed\xe0\x20\xf1\xe2\xee\xe9\x20\xef\xee\xff\xf1\x20\xe8\x20\xf3\xe4\xe0\xeb\xe8\xeb\xe8\xf1\xfc\x2e" /* "Вы стали свидетелем поединка, в котором паладин был смертельно ранен отрядом зомби. Он попросил вас взять его молот и завершить начатое им дело. Убив зомби вы повесили молот на свой пояс и удалились." */),
    DATA_COMPGEN(0x0050679c, gArtifactEventLegendaryScepter, "\xcc\xe8\xed\xf3\xff\x20\xed\xe5\xe1\xee\xeb\xfc\xf8\xee\xe9\x20\xf5\xee\xeb\xec\x2c\x20\xe2\xfb\x20\xf3\xe2\xe8\xe4\xe5\xeb\xe8\x2c\x20\xea\xe0\xea\x20\xec\xe0\xeb\xe5\xed\xfc\xea\xe0\xff\x20\xf4\xe5\xff\x20\xf2\xe0\xf9\xe8\xf2\x20\xee\xe3\xf0\xee\xec\xed\xfb\xe9\x20\xf1\xea\xe8\xef\xe5\xf2\xf0\x2e\x20\xd3\xeb\xfb\xe1\xed\xf3\xe2\xf8\xe8\xf1\xfc\x2c\x20\xe2\xfb\x20\xf1\xef\xf0\xee\xf1\xe8\xeb\xe8\x2c\x20\xed\xe5\x20\xed\xf3\xe6\xed\xe0\x20\xeb\xe8\x20\xe5\xe9\x20\xef\xee\xec\xee\xf9\xfc\x2e\x20\xd4\xe5\xff\x20\xee\xe1\xe8\xe6\xe5\xed\xed\xee\x20\xf1\xef\xf0\xee\xf1\xe8\xeb\xe0\x2c\x20\xec\xee\xeb\x2c\x20\xe4\xf3\xec\xe0\xe5\xf8\xfc\x2c\x20\xfd\xf2\xee\x20\xf1\xec\xe5\xf8\xed\xee\x3f\x20\xc2\xf1\xef\xee\xf0\xf5\xed\xf3\xeb\xe0\x20\xe8\x20\xf3\xeb\xe5\xf2\xe5\xeb\xe0\x2c\x20\xe0\x20\xf1\xea\xe8\xef\xe5\xf2\xf0\x20\xee\xf1\xf2\xe0\xeb\xf1\xff\x20\xe2\xe0\xec\x2e" /* "Минуя небольшой холм, вы увидели, как маленькая фея тащит огромный скипетр. Улыбнувшись, вы спросили, не нужна ли ей помощь. Фея обиженно спросила, мол, думаешь, это смешно? Вспорхнула и улетела, а скипетр остался вам." */),
    DATA_COMPGEN(0x00506878, gArtifactEventMasthead, "\xd1\xf2\xe0\xf0\xfb\xe9\x20\xec\xee\xf0\xff\xea\x20\xf0\xe0\xf1\xf1\xea\xe0\xe7\xe0\xeb\x20\xe2\xe0\xec\x2c\x20\xf7\xf2\xee\x20\xe2\x20\xe1\xfb\xeb\xfb\xe5\x20\xe2\xf0\xe5\xec\xe5\xed\xe0\x2c\x20\xed\xe0\x20\xe5\xe3\xee\x20\xe1\xee\xf2\xe8\xea\xe5\x20\xf1\xf2\xee\xff\xeb\xe0\x20\xec\xe0\xf7\xf2\xe0\x2c\x20\xef\xf0\xe8\xed\xee\xf1\xff\xf9\xe0\xff\x20\xe5\xec\xf3\x20\xf3\xe4\xe0\xf7\xf3\x2e\x20\xce\xed\x20\xe1\xe0\xeb\x20\xe2\xe0\xec\x20\xf1\xf5\xe5\xec\xf3\x2c\x20\xe3\xe4\xe5\x20\xe5\xe5\x20\xec\xee\xe6\xed\xee\x20\xe1\xf3\xe4\xe5\xf2\x20\xed\xe0\xe9\xf2\xe8\x2e\x20\xd7\xe5\xf0\xe5\xe7\x20\xed\xe5\xf1\xea\xee\xeb\xfc\xea\xee\x20\xf7\xe0\xf1\xee\xe2\x20\xef\xee\xe8\xf1\xea\xe0\x2c\x20\xe2\xfb\x20\xed\xe0\xf8\xeb\xe8\x20\xec\xe0\xf7\xf2\xf3\x20\xe2\x20\xf1\xf2\xe0\xf0\xee\xec\x20\xe4\xee\xea\xe5\x2e" /* "Старый моряк рассказал вам, что в былые времена, на его ботике стояла мачта, приносящая ему удачу. Он бал вам схему, где ее можно будет найти. Через несколько часов поиска, вы нашли мачту в старом доке." */),
    DATA_COMPGEN(0x00506944, gArtifactEventSphereOfNegation, "\xcd\xe0\x20\xe2\xe0\xf1\x20\xed\xe0\xeb\xe5\xf2\xe5\xeb\x20\xf2\xee\xf0\xee\xef\xfb\xe3\xe0\x2d\xea\xf0\xe5\xf1\xf2\xfc\xff\xed\xe8\xed\x2e\x20\xce\xed\x20\xf5\xee\xf2\xe5\xeb\x20\xf3\xe1\xe5\xe6\xe0\xf2\xfc\x2c\x20\xed\xee\x20\xe2\xfb\x20\xee\xf1\xf2\xe0\xed\xee\xe2\xe8\xeb\xe8\x20\xe5\xe3\xee\x2e\x20\xc8\xe7\xe2\xe8\xed\xe8\xe2\xf8\xe8\xf1\xfc\x2c\x20\xea\xf0\xe5\xf1\xf2\xfc\xff\xed\xe8\xed\x20\xe2\xf0\xf3\xf7\xe8\xeb\x20\xe2\xe0\xec\x20\xed\xe5\xee\xe1\xfb\xf7\xed\xf3\xfe\x20\xf1\xf4\xe5\xf0\xf3\x2e\x20\xc5\xe4\xe2\xe0\x20\xe2\xfb\x20\xe4\xee\xf2\xf0\xee\xed\xf3\xeb\xe8\xf1\xfc\x20\xe4\xee\x20\xed\xe5\xe5\x2c\x20\xea\xe0\xea\x20\xef\xee\xf7\xf3\xe2\xf1\xf2\xe2\xee\xe2\xe0\xeb\xe8\x2c\x20\xf7\xf2\xee\x20\xf1\xf4\xe5\xf0\xe0\x20\xe2\xf2\xff\xe3\xe8\xe2\xe0\xe5\xf2\x20\xe2\x20\xf1\xe5\xe1\xff\x20\xec\xe0\xe3\xe8\xfe\x2e\x2e\x2e" /* "На вас налетел торопыга-крестьянин. Он хотел убежать, но вы остановили его. Извинившись, крестьянин вручил вам необычную сферу. Едва вы дотронулись до нее, как почувствовали, что сфера втягивает в себя магию..." */),
    DATA_COMPGEN(0x00506a18, gArtifactEventStaffOfWizardry, "\xc2\xe0\xf8\xe8\x20\xf1\xee\xeb\xe4\xe0\xf2\xfb\x20\xed\xe0\xf8\xeb\xe8\x20\xed\xe5\xee\xe1\xfb\xf7\xed\xf3\xfe\x20\xe2\xe5\xf9\xfc\x20\xe8\x20\xf0\xe5\xf8\xe8\xeb\xe8\x20\xef\xf0\xe8\xed\xe5\xf1\xf2\xe8\x20\xe5\xe5\x20\xe2\xe0\xec\x2e\x20\xc2\xfb\x20\xee\xf2\xf7\xe8\xf1\xf2\xe8\xeb\xe8\x20\xe5\xe5\x20\xee\xf2\x20\xe3\xf0\xff\xe7\xe8\x20\xe8\x20\xf1\xec\xee\xe3\xeb\xe8\x20\xef\xf0\xee\xf7\xe8\xf2\xe0\xf2\xfc\x20\xed\xe0\x20\xed\xe5\xe9\x20\xed\xe5\xee\xe1\xfb\xf7\xed\xfb\xe5\x20\xf1\xeb\xee\xe2\xe0\x3a\x20\x22\xd3\xec\x20\x2d\x20\xeb\xf3\xf7\xf8\xe0\xff\x20\xf1\xe8\xeb\xe0\x2c\x20\xe0\x20\xec\xe0\xe3\xe8\xff\x20\xf1\xe8\xeb\xfc\xed\xe5\xe5\x20\xe3\xf0\xf3\xe1\xee\xe9\x20\xf1\xe8\xeb\xfb\x2e\x20\xcf\xee\xec\xed\xe8\x20\xec\xee\xe8\x20\xf1\xeb\xee\xe2\xe0\x2c\x20\xe8\x20\xf2\xfb\x20\xe2\xf1\xe5\xe3\xe4\xe0\x20\xe1\xf3\xe4\xe5\xf8\xfc\x20\xef\xee\xe1\xe5\xe6\xe4\xe0\xf2\xfc\x2e\x22" /* "Ваши солдаты нашли необычную вещь и решили принести ее вам. Вы отчистили ее от грязи и смогли прочитать на ней необычные слова: \"Ум - лучшая сила, а магия сильнее грубой силы. Помни мои слова, и ты всегда будешь побеждать.\"" */),
    DATA_COMPGEN(0x00506af8, gArtifactEventSwordBreaker, "\xce\xf2\xf1\xf2\xe0\xe2\xed\xee\xe9\x20\xea\xe0\xef\xe8\xf2\xe0\xed\x20\xe3\xee\xf0\xee\xe4\xf1\xea\xee\xe9\x20\xf1\xf2\xf0\xe0\xe6\xe8\x20\xf3\xe7\xed\xe0\xeb\x20\xee\x20\xe2\xe0\xf8\xe5\xec\x20\xef\xee\xf5\xee\xe4\xe5\x20\xe8\x20\xe4\xe0\xf0\xee\xe2\xe0\xeb\x20\xe2\xe0\xec\x20\xf1\xe2\xee\xe9\x20\xec\xe5\xf7\x2c\x20\xf1\xee\xf1\xeb\xf3\xe6\xe8\xe2\xf8\xe8\xe9\x20\xe5\xec\xf3\x20\xe4\xee\xe1\xf0\xf3\xfe\x20\xf1\xeb\xf3\xe6\xe1\xf3\x20\xe2\x20\xe1\xfb\xeb\xfb\xe5\x20\xe2\xf0\xe5\xec\xe5\xed\xe0\x2e" /* "Отставной капитан городской стражи узнал о вашем походе и даровал вам свой меч, сослуживший ему добрую службу в былые времена." */),
    DATA_COMPGEN(0x00506b78, gArtifactEventSwordOfAnduran, "\xd2\xf0\xee\xeb\xeb\xfc\x20\xee\xf1\xf2\xe0\xed\xee\xe2\xe8\xeb\x20\xe2\xe0\xf1\x2c\x20\xf1\xea\xe0\xe7\xe0\xe2\x3a\x20\x22\xcf\xeb\xe0\xf2\xe8\x20\xec\xed\xe5\x20\x35\x30\x30\x30\x20\xe7\xee\xeb\xee\xf2\xfb\xf5\x20\xe8\xeb\xe8\x20\xff\x20\xf3\xe1\xfc\xfe\x20\xf2\xe5\xe1\xff\x20\xec\xe5\xf7\xee\xec\x20\xc0\xed\xf3\xe4\xf0\xe0\xed\xe0\x21\x22\x20\xc2\xfb\x20\xee\xf2\xea\xe0\xe7\xe0\xeb\xe8\xf1\xfc\x20\xef\xeb\xe0\xf2\xe8\xf2\xfc\x2e\x20\xd2\xf0\xee\xeb\xeb\xfc\x20\xf1\xf5\xe2\xe0\xf2\xe8\xeb\xf1\xff\x20\xe7\xe0\x20\xea\xeb\xe8\xed\xee\xea\x20\xec\xe5\xf7\xe0\x2c\x20\xe2\xe7\xe2\xfb\xeb\x20\xee\xf2\x20\xe1\xee\xeb\xe8\x20\xe8\x20\xe1\xf0\xee\xf1\xe8\xe2\x20\xec\xe5\xf7\x20\xf3\xe1\xe5\xe6\xe0\xeb\x2e\x20\xd5\xee\xf0\xee\xf8\xee\x2c\x20\xf7\xf2\xee\x20\xee\xed\x20\xe1\xfb\xeb\x20\xed\xe0\xf1\xf2\xee\xeb\xfc\xea\xee\x20\xe3\xeb\xf3\xef\x2c\x20\xf7\xf2\xee\x20\xed\xe5\x20\xe7\xed\xe0\xeb\x2c\x20\xea\xe0\xea\x20\xef\xf0\xe0\xe2\xe8\xeb\xfc\xed\xee\x20\xe4\xe5\xf0\xe6\xe0\xf2\xfc\x20\xee\xf1\xf2\xf0\xfb\xe5\x20\xef\xf0\xe5\xe4\xec\xe5\xf2\xfb\x2e" /* "Тролль остановил вас, сказав: \"Плати мне 5000 золотых или я убью тебя мечом Анудрана!\" Вы отказались платить. Тролль схватился за клинок меча, взвыл от боли и бросив меч убежал. Хорошо, что он был настолько глуп, что не знал, как правильно держать острые предметы." */),
    DATA_COMPGEN(0x00506c84, gArtifactEventSpadeOfNecromancy, "\xc2\x20\xe3\xf0\xff\xe7\xe8\x20\xe2\xfb\x20\xef\xee\xe4\xee\xe1\xf0\xe0\xeb\xe8\x20\xf1\xf2\xe0\xf0\xf3\xfe\x20\xeb\xee\xef\xe0\xf2\xf3\x2e\x20\xcf\xf0\xe8\xf1\xec\xee\xf2\xf0\xe5\xe2\xf8\xe8\xf1\xfc\x2c\x20\xe2\xfb\x20\xef\xee\xed\xff\xeb\xe8\x2c\x20\xf7\xf2\xee\x20\xe2\xe0\xec\x20\xef\xee\xf1\xf7\xe0\xf1\xf2\xeb\xe8\xe2\xe8\xeb\xee\xf1\xfc\x20\xed\xe0\xe9\xf2\xe8\x20\xe7\xe0\xf7\xe0\xf0\xee\xe2\xe0\xed\xed\xf3\xfe\x20\xeb\xee\xef\xe0\xf2\xf3\x20\xe3\xf0\xe0\xe1\xe8\xf2\xe5\xeb\xe5\xe9\x20\xec\xee\xe3\xe8\xeb\x2e" /* "В грязи вы подобрали старую лопату. Присмотревшись, вы поняли, что вам посчастливилось найти зачарованную лопату грабителей могил." */)};
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
DATA(0x004fdcc0) char* gArmyShortNames[IDX(CREATURE_COUNT)] = {
    DATA_COMPGEN(0x00506ebc, gArmyShortNamesPeasant, "peasn"),
    DATA_COMPGEN(0x00506ec4, gArmyShortNamesArcher, "archr"),
    DATA_COMPGEN(0x00506ecc, gArmyShortNamesRanger, "arch2"),
    DATA_COMPGEN(0x00506ed4, gArmyShortNamesPikeman, "pikmn"),
    DATA_COMPGEN(0x00506edc, gArmyShortNamesVeteranPikeman, "pikm2"),
    DATA_COMPGEN(0x00506ee4, gArmyShortNamesSwordsman, "swman"),
    DATA_COMPGEN(0x00506eec, gArmyShortNamesMasterSwordsman, "swma2"),
    DATA_COMPGEN(0x00506ef4, gArmyShortNamesCavalry, "cvlry"),
    DATA_COMPGEN(0x00506efc, gArmyShortNamesChampion, "cvlr2"),
    DATA_COMPGEN(0x00506f04, gArmyShortNamesPaladin, "paldn"),
    DATA_COMPGEN(0x00506f0c, gArmyShortNamesCrusader, "pald2"),
    DATA_COMPGEN(0x00506f14, gArmyShortNamesGoblin, "gobln"),
    DATA_COMPGEN(0x00506f1c, gArmyShortNamesOrc, "orc__"),
    DATA_COMPGEN(0x00506f24, gArmyShortNamesOrcChief, "orc_2"),
    DATA_COMPGEN(0x00506f2c, gArmyShortNamesWolf, "Wolf_"),
    DATA_COMPGEN(0x00506f34, gArmyShortNamesOgre, "Ogre_"),
    DATA_COMPGEN(0x00506f3c, gArmyShortNamesOgreLord, "Ogre2"),
    DATA_COMPGEN(0x00506f44, gArmyShortNamesTroll, "Troll"),
    DATA_COMPGEN(0x00506f4c, gArmyShortNamesWarTroll, "trol2"),
    DATA_COMPGEN(0x00506f54, gArmyShortNamesCyclops, "cyclp"),
    DATA_COMPGEN(0x00506f5c, gArmyShortNamesSprite, "sprit"),
    DATA_COMPGEN(0x00506f64, gArmyShortNamesDwarf, "Dwarf"),
    DATA_COMPGEN(0x00506f6c, gArmyShortNamesBattleDwarf, "dwar2"),
    DATA_COMPGEN(0x00506f74, gArmyShortNamesElf, "elf__"),
    DATA_COMPGEN(0x00506f7c, gArmyShortNamesGrandElf, "elf_2"),
    DATA_COMPGEN(0x00506f84, gArmyShortNamesDruid, "druid"),
    DATA_COMPGEN(0x00506f8c, gArmyShortNamesGreaterDruid, "drui2"),
    DATA_COMPGEN(0x00506f94, gArmyShortNamesUnicorn, "uncrn"),
    DATA_COMPGEN(0x00506f9c, gArmyShortNamesPhoenix, "phoen"),
    DATA_COMPGEN(0x00506fa4, gArmyShortNamesCentaur, "centr"),
    DATA_COMPGEN(0x00506fac, gArmyShortNamesGargoyle, "gargl"),
    DATA_COMPGEN(0x00506fb4, gArmyShortNamesGriffin, "griff"),
    DATA_COMPGEN(0x00506fbc, gArmyShortNamesMinotaur, "mintr"),
    DATA_COMPGEN(0x00506fc4, gArmyShortNamesMinotaurKing, "mint2"),
    DATA_COMPGEN(0x00506fcc, gArmyShortNamesHydra, "Hydra"),
    DATA_COMPGEN(0x00506fd4, gArmyShortNamesGreenDragon, "dragn"),
    DATA_COMPGEN(0x00506fdc, gArmyShortNamesRedDragon, "drag2"),
    DATA_COMPGEN(0x00506fe4, gArmyShortNamesBlackDragon, "drag3"),
    DATA_COMPGEN(0x00506fec, gArmyShortNamesHalfling, "hlflg"),
    DATA_COMPGEN(0x00506ff4, gArmyShortNamesBoar, "Boar_"),
    DATA_COMPGEN(0x00506ffc, gArmyShortNamesIronGolem, "irong"),
    DATA_COMPGEN(0x00507004, gArmyShortNamesSteelGolem, "iron2"),
    DATA_COMPGEN(0x0050700c, gArmyShortNamesRoc, "roc__"),
    DATA_COMPGEN(0x00507014, gArmyShortNamesMage, "archm"),
    DATA_COMPGEN(0x0050701c, gArmyShortNamesArchmage, "arch2"),
    DATA_COMPGEN(0x00507024, gArmyShortNamesGiant, "titan"),
    DATA_COMPGEN(0x0050702c, gArmyShortNamesTitan, "tita2"),
    DATA_COMPGEN(0x00507034, gArmyShortNamesSkeleton, "skel_"),
    DATA_COMPGEN(0x0050703c, gArmyShortNamesZombie, "zomb_"),
    DATA_COMPGEN(0x00507044, gArmyShortNamesMutantZombie, "zomb2"),
    DATA_COMPGEN(0x0050704c, gArmyShortNamesMummy, "Mummy"),
    DATA_COMPGEN(0x00507054, gArmyShortNamesRoyalMummy, "mumm2"),
    DATA_COMPGEN(0x0050705c, gArmyShortNamesVampire, "vampr"),
    DATA_COMPGEN(0x00507064, gArmyShortNamesVampireLord, "vamp2"),
    DATA_COMPGEN(0x0050706c, gArmyShortNamesLich, "lich_"),
    DATA_COMPGEN(0x00507074, gArmyShortNamesPowerLich, "lich2"),
    DATA_COMPGEN(0x0050707c, gArmyShortNamesBoneDragon, "boned"),
    DATA_COMPGEN(0x00507084, gArmyShortNamesRogue, "Rogue"),
    DATA_COMPGEN(0x0050708c, gArmyShortNamesNomad, "Nomad"),
    DATA_COMPGEN(0x00507094, gArmyShortNamesGhost, "Ghost"),
    DATA_COMPGEN(0x0050709c, gArmyShortNamesGenie, "Genie"),
    DATA_COMPGEN(0x005070a4, gArmyShortNamesMedusa, "medus"),
    DATA_COMPGEN(0x005070ac, gArmyShortNamesEarthElemental, "eleme"),
    DATA_COMPGEN(0x005070b4, gArmyShortNamesAirElemental, "elema"),
    DATA_COMPGEN(0x005070bc, gArmyShortNamesFireElemental, "elemf"),
    DATA_COMPGEN(0x005070c4, gArmyShortNamesWaterElemental, "elemw")
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
    /*  */ DATA_COMPGEN(0x005267dc, speedTextEmpty0, ""),
    /* Ползает */ DATA_COMPGEN(0x0050accc, speedTextCrawling1, "\xcf\xee\xeb\xe7\xe0\xe5\xf2"),
    /* Оч. низкая */ DATA_COMPGEN(0x0050acd4, speedTextVerySlow2, "\xce\xf7\x2e\x20\xed\xe8\xe7\xea\xe0\xff"),
    /* Низкая */ DATA_COMPGEN(0x0050ace0, speedTextSlow3, "\xcd\xe8\xe7\xea\xe0\xff"),
    /* Средняя */ DATA_COMPGEN(0x0050ace8, speedTextAverage4, "\xd1\xf0\xe5\xe4\xed\xff\xff"),
    /* Высокая */ DATA_COMPGEN(0x0050acf0, speedTextFast5, "\xc2\xfb\xf1\xee\xea\xe0\xff"),
    /* Оч. высокая */ DATA_COMPGEN(0x0050acf8, speedTextVeryFast6, "\xce\xf7\x2e\x20\xe2\xfb\xf1\xee\xea\xe0\xff"),
    /* Ультра высокая */ DATA_COMPGEN(0x0050ad04, speedTextUltraFast7, "\xd3\xeb\xfc\xf2\xf0\xe0\x20\xe2\xfb\xf1\xee\xea\xe0\xff"),
    /* Молниеносная */ DATA_COMPGEN(0x0050ad14, speedTextBlazing8, "\xcc\xee\xeb\xed\xe8\xe5\xed\xee\xf1\xed\xe0\xff"),
    /* Абсолютная */ DATA_COMPGEN(0x0050ad24, speedTextInstant9, "\xc0\xe1\xf1\xee\xeb\xfe\xf2\xed\xe0\xff")
};
DATA(0x004fe67c) char* cArmyDetail[KB_ARMY_DETAIL_TEXT_COUNT] = {
    /* Атака:  */ DATA_COMPGEN(0x0050ad30, cArmyDetailAttackSkill0, "\xc0\xf2\xe0\xea\xe0\x3a\x20"),
    /* Защита:  */ DATA_COMPGEN(0x0050ad38, cArmyDetailDefenseSkill1, "\xc7\xe0\xf9\xe8\xf2\xe0\x3a\x20"),
    /* Выстрелов:  */ DATA_COMPGEN(0x0050ad44, cArmyDetailShotsLeft2, "\xc2\xfb\xf1\xf2\xf0\xe5\xeb\xee\xe2\x3a\x20"),
    /* Урон:  */ DATA_COMPGEN(0x0050ad50, cArmyDetailDamage3, "\xd3\xf0\xee\xed\x3a\x20"),
    /* Здоровье:  */ DATA_COMPGEN(0x0050ad58, cArmyDetailHitPoints4, "\xc7\xe4\xee\xf0\xee\xe2\xfc\xe5\x3a\x20"),
    /* Скорость:  */ DATA_COMPGEN(0x0050ad64, cArmyDetailSpeed5, "\xd1\xea\xee\xf0\xee\xf1\xf2\xfc\x3a\x20"),
    /* Мораль:  */ DATA_COMPGEN(0x0050ad70, cArmyDetailMorale6, "\xcc\xee\xf0\xe0\xeb\xfc\x3a\x20"),
    /* Удача:  */ DATA_COMPGEN(0x0050ad7c, cArmyDetailLuck7, "\xd3\xe4\xe0\xf7\xe0\x3a\x20"),
    /* Выстрелов:  */ DATA_COMPGEN(0x0050ad84, cArmyDetailShots8, "\xc2\xfb\xf1\xf2\xf0\xe5\xeb\xee\xe2\x3a\x20")
};
DATA(0x004fe6a0) char* cWellDetail[KB_WELL_DETAIL_TEXT_COUNT] = {
    /* Атака:  */ DATA_COMPGEN(0x0050ad90, cWellDetailAttack0, "\xc0\xf2\xe0\xea\xe0\x3a\x20"),
    /* Защита:  */ DATA_COMPGEN(0x0050ad98, cWellDetailDefense1, "\xc7\xe0\xf9\xe8\xf2\xe0\x3a\x20"),
    /* Выстр.:  */ DATA_COMPGEN(0x0050ada4, cWellDetailShots2, "\xc2\xfb\xf1\xf2\xf0\x2e\x3a\x20"),
    /* Урон:  */ DATA_COMPGEN(0x0050adb0, cWellDetailDamg3, "\xd3\xf0\xee\xed\x3a\x20"),
    /* ЗД:  */ DATA_COMPGEN(0x0050adb8, cWellDetailHP4, "\xc7\xc4\x3a\x20"),
    /* Скор.:  */ DATA_COMPGEN(0x0050adc0, cWellDetailSpeed5, "\xd1\xea\xee\xf0\x2e\x3a\x20"),
    /* Всего:  */ DATA_COMPGEN(0x0050adc8, cWellDetailAvailable6, "\xc2\xf1\xe5\xe3\xee\x3a\x20"),
    /* \n\nСкорость:\n%s */ DATA_COMPGEN(0x0050add0, cWellDetailSpeedFormat7, "\x0a\x0a\xd1\xea\xee\xf0\xee\xf1\xf2\xfc\x3a\x0a\x25\x73"),
    /* \n\nПрирост\n + %d/нед. */ DATA_COMPGEN(0x0050ade0, cWellDetailGrowthFormatWeek8, "\x0a\x0a\xcf\xf0\xe8\xf0\xee\xf1\xf2\x0a\x20\x2b\x20\x25\x64\x2f\xed\xe5\xe4\x2e")
};
DATA(0x004fe6c4) char* cKingdomOverview[KB_KINGDOM_OVERVIEW_TEXT_COUNT] = {
    /* Обзор королевства   Месяц: %d, Неделя: %d, День: %d */ DATA_COMPGEN(0x0050adf8, cKingdomOverviewKingdomOverviewMonthFormatWeekFormatDayFormat0,
        "\xce\xe1\xe7\xee\xf0\x20\xea\xee\xf0\xee\xeb\xe5\xe2\xf1\xf2\xe2\xe0\x20\x20\x20\xcc\xe5\xf1\xff\xf6\x3a\x20\x25\x64\x2c\x20\xcd\xe5\xe4\xe5\xeb\xff\x3a\x20\x25\x64\x2c\x20\xc4\xe5\xed\xfc\x3a"
        "\x20\x25\x64"),
    /* Ваш Драконий город. */ DATA_COMPGEN(0x0050ae2c, cKingdomOverviewYouOwnDragonCity1, "\xc2\xe0\xf8\x20\xc4\xf0\xe0\xea\xee\xed\xe8\xe9\x20\xe3\xee\xf0\xee\xe4\x2e"),
    /* Ваш маяк. */ DATA_COMPGEN(0x0050ae40, cKingdomOverviewYouOwnTheLighthouse2, "\xc2\xe0\xf8\x20\xec\xe0\xff\xea\x2e")
};
DATA(0x004fe6d0) char* cNewTurn[KB_NEW_TURN_TEXT_COUNT] = {
    /* %s, у вас осталось всего %d дней на то, чтобы завоевать хотя бы один город; иначе вы будете навеки изгнаны из страны. */ DATA_COMPGEN(0x0050ae4c, cNewTurnFormatYouOnlyHaveFormatDaysLeftTo0,
        "\x25\x73\x2c\x20\xf3\x20\xe2\xe0\xf1\x20\xee\xf1\xf2\xe0\xeb\xee\xf1\xfc\x20\xe2\xf1\xe5\xe3\xee\x20\x25\x64\x20\xe4\xed\xe5\xe9\x20\xed\xe0\x20\xf2\xee\x2c\x20\xf7\xf2\xee\xe1\xfb\x20\xe7\xe0"
        "\xe2\xee\xe5\xe2\xe0\xf2\xfc\x20\xf5\xee\xf2\xff\x20\xe1\xfb\x20\xee\xe4\xe8\xed\x20\xe3\xee\xf0\xee\xe4\x3b\x20\xe8\xed\xe0\xf7\xe5\x20\xe2\xfb\x20\xe1\xf3\xe4\xe5\xf2\xe5\x20\xed\xe0\xe2\xe5"
        "\xea\xe8\x20\xe8\xe7\xe3\xed\xe0\xed\xfb\x20\xe8\xe7\x20\xf1\xf2\xf0\xe0\xed\xfb\x2e"),
    /* %s, настал последний день, когда вы еще можете завоевать себе город; в противном случае вы будете навеки изгнаны из страны. */ DATA_COMPGEN(0x0050aec4, cNewTurnFormatThisIsYourLastDayToCapture1,
        "\x25\x73\x2c\x20\xed\xe0\xf1\xf2\xe0\xeb\x20\xef\xee\xf1\xeb\xe5\xe4\xed\xe8\xe9\x20\xe4\xe5\xed\xfc\x2c\x20\xea\xee\xe3\xe4\xe0\x20\xe2\xfb\x20\xe5\xf9\xe5\x20\xec\xee\xe6\xe5\xf2\xe5\x20\xe7"
        "\xe0\xe2\xee\xe5\xe2\xe0\xf2\xfc\x20\xf1\xe5\xe1\xe5\x20\xe3\xee\xf0\xee\xe4\x3b\x20\xe2\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xee\xec\x20\xf1\xeb\xf3\xf7\xe0\xe5\x20\xe2\xfb\x20\xe1\xf3\xe4\xe5\xf2"
        "\xe5\x20\xed\xe0\xe2\xe5\xea\xe8\x20\xe8\xe7\xe3\xed\xe0\xed\xfb\x20\xe8\xe7\x20\xf1\xf2\xf0\xe0\xed\xfb\x2e"),
    /* Астрологи объявляют месяц %s.\n\nНаселение всех жилищ возросло. */ DATA_COMPGEN(0x0050af40, cNewTurnAstrologersProclaimMonthOfTheFormatAllDwellings2,
        "\xc0\xf1\xf2\xf0\xee\xeb\xee\xe3\xe8\x20\xee\xe1\xfa\xff\xe2\xeb\xff\xfe\xf2\x20\xec\xe5\xf1\xff\xf6\x20\x25\x73\x2e\x0a\x0a\xcd\xe0\xf1\xe5\xeb\xe5\xed\xe8\xe5\x20\xe2\xf1\xe5\xf5\x20\xe6\xe8"
        "\xeb\xe8\xf9\x20\xe2\xee\xe7\xf0\xee\xf1\xeb\xee\x2e"),
    /* Астрологи объявляют, что этому месяцу покровительствует сила %s.\n\nПопуляция %s удваивается!\n\nНаселение всех жилищ возросло. */ DATA_COMPGEN(0x0050af80, cNewTurnAstrologersProclaimMonthOfTheFormatFormatPopulation3,
        "\xc0\xf1\xf2\xf0\xee\xeb\xee\xe3\xe8\x20\xee\xe1\xfa\xff\xe2\xeb\xff\xfe\xf2\x2c\x20\xf7\xf2\xee\x20\xfd\xf2\xee\xec\xf3\x20\xec\xe5\xf1\xff\xf6\xf3\x20\xef\xee\xea\xf0\xee\xe2\xe8\xf2\xe5\xeb"
        "\xfc\xf1\xf2\xe2\xf3\xe5\xf2\x20\xf1\xe8\xeb\xe0\x20\x25\x73\x2e\x0a\x0a\xcf\xee\xef\xf3\xeb\xff\xf6\xe8\xff\x20\x25\x73\x20\xf3\xe4\xe2\xe0\xe8\xe2\xe0\xe5\xf2\xf1\xff\x21\x0a\x0a\xcd\xe0\xf1"
        "\xe5\xeb\xe5\xed\xe8\xe5\x20\xe2\xf1\xe5\xf5\x20\xe6\xe8\xeb\xe8\xf9\x20\xe2\xee\xe7\xf0\xee\xf1\xeb\xee\x2e"),
    /* Астрологи объявляют месяц ЧУМЫ!\n\nНаселение всех жилищ уменьшилось вдвое. */ DATA_COMPGEN(0x0050affc, cNewTurnAstrologersProclaimMonthOfThePLAGUEAllPopulations4,
        "\xc0\xf1\xf2\xf0\xee\xeb\xee\xe3\xe8\x20\xee\xe1\xfa\xff\xe2\xeb\xff\xfe\xf2\x20\xec\xe5\xf1\xff\xf6\x20\xd7\xd3\xcc\xdb\x21\x0a\x0a\xcd\xe0\xf1\xe5\xeb\xe5\xed\xe8\xe5\x20\xe2\xf1\xe5\xf5\x20"
        "\xe6\xe8\xeb\xe8\xf9\x20\xf3\xec\xe5\xed\xfc\xf8\xe8\xeb\xee\xf1\xfc\x20\xe2\xe4\xe2\xee\xe5\x2e"),
    /* Астрологи объявляют неделю %s.\n\nНаселение всех жилищ возросло. */ DATA_COMPGEN(0x0050b048, cNewTurnAstrologersProclaimWeekOfTheFormatAllDwellings5,
        "\xc0\xf1\xf2\xf0\xee\xeb\xee\xe3\xe8\x20\xee\xe1\xfa\xff\xe2\xeb\xff\xfe\xf2\x20\xed\xe5\xe4\xe5\xeb\xfe\x20\x25\x73\x2e\x0a\x0a\xcd\xe0\xf1\xe5\xeb\xe5\xed\xe8\xe5\x20\xe2\xf1\xe5\xf5\x20\xe6"
        "\xe8\xeb\xe8\xf9\x20\xe2\xee\xe7\xf0\xee\xf1\xeb\xee\x2e"),
    /* Астрологи объявляют, что этой неделе покровительствует сила %s.\n\nПопуляция %s +5.\n\nНаселение всех жилищ возросло. */ DATA_COMPGEN(0x0050b088, cNewTurnAstrologersProclaimWeekOfTheFormatFormatGrowth6,
        "\xc0\xf1\xf2\xf0\xee\xeb\xee\xe3\xe8\x20\xee\xe1\xfa\xff\xe2\xeb\xff\xfe\xf2\x2c\x20\xf7\xf2\xee\x20\xfd\xf2\xee\xe9\x20\xed\xe5\xe4\xe5\xeb\xe5\x20\xef\xee\xea\xf0\xee\xe2\xe8\xf2\xe5\xeb\xfc"
        "\xf1\xf2\xe2\xf3\xe5\xf2\x20\xf1\xe8\xeb\xe0\x20\x25\x73\x2e\x0a\x0a\xcf\xee\xef\xf3\xeb\xff\xf6\xe8\xff\x20\x25\x73\x20\x2b\x35\x2e\x0a\x0a\xcd\xe0\xf1\xe5\xeb\xe5\xed\xe8\xe5\x20\xe2\xf1\xe5"
        "\xf5\x20\xe6\xe8\xeb\xe8\xf9\x20\xe2\xee\xe7\xf0\xee\xf1\xeb\xee\x2e")
};
DATA(0x004fe6ec) char* cViewGeneralLabels[KB_VIEW_GENERAL_LABEL_COUNT] = {
    /* Атака:  */ DATA_COMPGEN(0x0050b0fc, cViewGeneralLabelsAttack0, "\xc0\xf2\xe0\xea\xe0\x3a\x20"),
    /* Защита:  */ DATA_COMPGEN(0x0050b104, cViewGeneralLabelsDefense1, "\xc7\xe0\xf9\xe8\xf2\xe0\x3a\x20"),
    /* Сила магии:  */ DATA_COMPGEN(0x0050b110, cViewGeneralLabelsSpellPower2, "\xd1\xe8\xeb\xe0\x20\xec\xe0\xe3\xe8\xe8\x3a\x20"),
    /* Знания:  */ DATA_COMPGEN(0x0050b120, cViewGeneralLabelsKnowledge3, "\xc7\xed\xe0\xed\xe8\xff\x3a\x20"),
    /* Мораль:  */ DATA_COMPGEN(0x0050b12c, cViewGeneralLabelsMorale4, "\xcc\xee\xf0\xe0\xeb\xfc\x3a\x20"),
    /* Удача:  */ DATA_COMPGEN(0x0050b138, cViewGeneralLabelsLuck5, "\xd3\xe4\xe0\xf7\xe0\x3a\x20"),
    /* Очки магии:  */ DATA_COMPGEN(0x0050b140, cViewGeneralLabelsSpellPoints6, "\xce\xf7\xea\xe8\x20\xec\xe0\xe3\xe8\xe8\x3a\x20")
};
DATA(0x004fe708) char* cViewGeneralHelp[KB_VIEW_GENERAL_HELP_COUNT] = {
    /* Остановить катапульту */ DATA_COMPGEN(0x0050b150, cViewGeneralHelpStopCatapult0, "\xce\xf1\xf2\xe0\xed\xee\xe2\xe8\xf2\xfc\x20\xea\xe0\xf2\xe0\xef\xf3\xeb\xfc\xf2\xf3"),
    /* Направить заклинание */ DATA_COMPGEN(0x0050b168, cViewGeneralHelpCastSpell1, "\xcd\xe0\xef\xf0\xe0\xe2\xe8\xf2\xfc\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5"),
    /* Отступить */ DATA_COMPGEN(0x0050b180, cViewGeneralHelpRetreat2, "\xce\xf2\xf1\xf2\xf3\xef\xe8\xf2\xfc"),
    /* Сдаться */ DATA_COMPGEN(0x0050b18c, cViewGeneralHelpSurrender3, "\xd1\xe4\xe0\xf2\xfc\xf1\xff"),
    /* Отменить */ DATA_COMPGEN(0x0050b194, cViewGeneralHelpCancel4, "\xce\xf2\xec\xe5\xed\xe8\xf2\xfc"),
    /* Возможности героя */ DATA_COMPGEN(0x0050b1a0, cViewGeneralHelpHeroSOptions5, "\xc2\xee\xe7\xec\xee\xe6\xed\xee\xf1\xf2\xe8\x20\xe3\xe5\xf0\xee\xff"),
    /* Возможности капитана */ DATA_COMPGEN(0x0050b1b4, cViewGeneralHelpCaptainSOptions6, "\xc2\xee\xe7\xec\xee\xe6\xed\xee\xf1\xf2\xe8\x20\xea\xe0\xef\xe8\xf2\xe0\xed\xe0")
};
DATA(0x004fe724) char* cViewGeneralLongHelp[KB_VIEW_GENERAL_LONG_HELP_COUNT] = {
    /* {Направить заклинание}\n\nНаправить заклинание. В течение каждого раунда боя можно направить лишь одно заклинание. Новый раунд начинается после того, как все отряды на поле боя завершили свой ход. */ DATA_COMPGEN(0x0050b1cc, cViewGeneralLongHelpCastSpellCastAMagicalSpellYouMay0,
        "\x7b\xcd\xe0\xef\xf0\xe0\xe2\xe8\xf2\xfc\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5\x7d\x0a\x0a\xcd\xe0\xef\xf0\xe0\xe2\xe8\xf2\xfc\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5\x2e\x20\xc2\x20"
        "\xf2\xe5\xf7\xe5\xed\xe8\xe5\x20\xea\xe0\xe6\xe4\xee\xe3\xee\x20\xf0\xe0\xf3\xed\xe4\xe0\x20\xe1\xee\xff\x20\xec\xee\xe6\xed\xee\x20\xed\xe0\xef\xf0\xe0\xe2\xe8\xf2\xfc\x20\xeb\xe8\xf8\xfc\x20"
        "\xee\xe4\xed\xee\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5\x2e\x20\xcd\xee\xe2\xfb\xe9\x20\xf0\xe0\xf3\xed\xe4\x20\xed\xe0\xf7\xe8\xed\xe0\xe5\xf2\xf1\xff\x20\xef\xee\xf1\xeb\xe5\x20\xf2\xee"
        "\xe3\xee\x2c\x20\xea\xe0\xea\x20\xe2\xf1\xe5\x20\xee\xf2\xf0\xff\xe4\xfb\x20\xed\xe0\x20\xef\xee\xeb\xe5\x20\xe1\xee\xff\x20\xe7\xe0\xe2\xe5\xf0\xf8\xe8\xeb\xe8\x20\xf1\xe2\xee\xe9\x20\xf5\xee"
        "\xe4\x2e"),
    /* {Отступить}\n\nГерой отступает с поля боя, бросив свое войско на произвол судьбы. Отступившего героя можно будет снова нанять на службу, но при этом сопровождать его будет лишь очень небольшая армия, как если бы ваш герой был зеленым новичком. */ DATA_COMPGEN(0x0050b290, cViewGeneralLongHelpRetreatRetreatYourHeroAbandoningYourCreaturesYour1,
        "\x7b\xce\xf2\xf1\xf2\xf3\xef\xe8\xf2\xfc\x7d\x0a\x0a\xc3\xe5\xf0\xee\xe9\x20\xee\xf2\xf1\xf2\xf3\xef\xe0\xe5\xf2\x20\xf1\x20\xef\xee\xeb\xff\x20\xe1\xee\xff\x2c\x20\xe1\xf0\xee\xf1\xe8\xe2\x20"
        "\xf1\xe2\xee\xe5\x20\xe2\xee\xe9\xf1\xea\xee\x20\xed\xe0\x20\xef\xf0\xee\xe8\xe7\xe2\xee\xeb\x20\xf1\xf3\xe4\xfc\xe1\xfb\x2e\x20\xce\xf2\xf1\xf2\xf3\xef\xe8\xe2\xf8\xe5\xe3\xee\x20\xe3\xe5\xf0"
        "\xee\xff\x20\xec\xee\xe6\xed\xee\x20\xe1\xf3\xe4\xe5\xf2\x20\xf1\xed\xee\xe2\xe0\x20\xed\xe0\xed\xff\xf2\xfc\x20\xed\xe0\x20\xf1\xeb\xf3\xe6\xe1\xf3\x2c\x20\xed\xee\x20\xef\xf0\xe8\x20\xfd\xf2"
        "\xee\xec\x20\xf1\xee\xef\xf0\xee\xe2\xee\xe6\xe4\xe0\xf2\xfc\x20\xe5\xe3\xee\x20\xe1\xf3\xe4\xe5\xf2\x20\xeb\xe8\xf8\xfc\x20\xee\xf7\xe5\xed\xfc\x20\xed\xe5\xe1\xee\xeb\xfc\xf8\xe0\xff\x20\xe0"
        "\xf0\xec\xe8\xff\x2c\x20\xea\xe0\xea\x20\xe5\xf1\xeb\xe8\x20\xe1\xfb\x20\xe2\xe0\xf8\x20\xe3\xe5\xf0\xee\xe9\x20\xe1\xfb\xeb\x20\xe7\xe5\xeb\xe5\xed\xfb\xec\x20\xed\xee\xe2\xe8\xf7\xea\xee\xec"
        "\x2e"),
    /* {Сдаться}\n\nКапитуляция стоит денег. Тем не менее, если выкуп будет уплачен, героя можно будет снова нанять на службу вместе со всеми уцелевшими в битве войсками. */ DATA_COMPGEN(0x0050b384, cViewGeneralLongHelpSurrenderSurrenderingCostsGoldHoweverIfYouPay2,
        "\x7b\xd1\xe4\xe0\xf2\xfc\xf1\xff\x7d\x0a\x0a\xca\xe0\xef\xe8\xf2\xf3\xeb\xff\xf6\xe8\xff\x20\xf1\xf2\xee\xe8\xf2\x20\xe4\xe5\xed\xe5\xe3\x2e\x20\xd2\xe5\xec\x20\xed\xe5\x20\xec\xe5\xed\xe5\xe5"
        "\x2c\x20\xe5\xf1\xeb\xe8\x20\xe2\xfb\xea\xf3\xef\x20\xe1\xf3\xe4\xe5\xf2\x20\xf3\xef\xeb\xe0\xf7\xe5\xed\x2c\x20\xe3\xe5\xf0\xee\xff\x20\xec\xee\xe6\xed\xee\x20\xe1\xf3\xe4\xe5\xf2\x20\xf1\xed"
        "\xee\xe2\xe0\x20\xed\xe0\xed\xff\xf2\xfc\x20\xed\xe0\x20\xf1\xeb\xf3\xe6\xe1\xf3\x20\xe2\xec\xe5\xf1\xf2\xe5\x20\xf1\xee\x20\xe2\xf1\xe5\xec\xe8\x20\xf3\xf6\xe5\xeb\xe5\xe2\xf8\xe8\xec\xe8\x20"
        "\xe2\x20\xe1\xe8\xf2\xe2\xe5\x20\xe2\xee\xe9\xf1\xea\xe0\xec\xe8\x2e"),
    /* {Отмена}\n\nВернуться в бой. */ DATA_COMPGEN(0x0050b428, cViewGeneralLongHelpCancelReturnToTheBattle3, "\x7b\xce\xf2\xec\xe5\xed\xe0\x7d\x0a\x0a\xc2\xe5\xf0\xed\xf3\xf2\xfc\xf1\xff\x20\xe2\x20\xe1\xee\xe9\x2e")
};
DATA(0x004fe734) char* cCombatMessage[KB_COMBAT_MESSAGE_COUNT] = {
    /*  */ DATA_COMPGEN(0x005267e0, cCombatMessageEmpty0, ""),
    /* %s: Идти сюда. */ DATA_COMPGEN(0x0050b444, cCombatMessageMoveFormatHere1, "\x25\x73\x3a\x20\xc8\xe4\xf2\xe8\x20\xf1\xfe\xe4\xe0\x2e"),
    /* %s: Перелететь сюда. */ DATA_COMPGEN(0x0050b454, cCombatMessageFlyFormatHere2, "\x25\x73\x3a\x20\xcf\xe5\xf0\xe5\xeb\xe5\xf2\xe5\xf2\xfc\x20\xf1\xfe\xe4\xe0\x2e"),
    /* Атаковать %s */ DATA_COMPGEN(0x0050b46c, cCombatMessageAttackFormat3, "\xc0\xf2\xe0\xea\xee\xe2\xe0\xf2\xfc\x20\x25\x73"),
    /* Стрелять в %s (осталось %d выстр.) */ DATA_COMPGEN(0x0050b47c, cCombatMessageShootFormatFormatShotSLeft4, "\xd1\xf2\xf0\xe5\xeb\xff\xf2\xfc\x20\xe2\x20\x25\x73\x20\x28\xee\xf1\xf2\xe0\xeb\xee\xf1\xfc\x20\x25\x64\x20\xe2\xfb\xf1\xf2\xf0\x2e\x29"),
    /* Возможности героя */ DATA_COMPGEN(0x0050b4a0, cCombatMessageHeroSOptions5, "\xc2\xee\xe7\xec\xee\xe6\xed\xee\xf1\xf2\xe8\x20\xe3\xe5\xf0\xee\xff"),
    /* Вражеский герой */ DATA_COMPGEN(0x0050b4b4, cCombatMessageViewOpposingHero6, "\xc2\xf0\xe0\xe6\xe5\xf1\xea\xe8\xe9\x20\xe3\xe5\xf0\xee\xe9"),
    /* %s: Показать информацию. */ DATA_COMPGEN(0x0050b4c4, cCombatMessageViewFormatInfo7, "\x25\x73\x3a\x20\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xe8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xfe\x2e"),
    /* Нет стрел! */ DATA_COMPGEN(0x0050b4e0, cCombatMessageNoShotsLeft8, "\xcd\xe5\xf2\x20\xf1\xf2\xf0\xe5\xeb\x21"),
    /* Возможности капитана */ DATA_COMPGEN(0x0050b4ec, cCombatMessageCaptainSOptions9, "\xc2\xee\xe7\xec\xee\xe6\xed\xee\xf1\xf2\xe8\x20\xea\xe0\xef\xe8\xf2\xe0\xed\xe0"),
    /* Показать вражеского капитана */ DATA_COMPGEN(0x0050b504, cCombatMessageViewOpposingCaptain10, "\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xe2\xf0\xe0\xe6\xe5\xf1\xea\xee\xe3\xee\x20\xea\xe0\xef\xe8\xf2\xe0\xed\xe0"),
    /* Информация о баллисте */ DATA_COMPGEN(0x0050b524, cCombatMessageViewBallistaInfo11, "\xc8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xff\x20\xee\x20\xe1\xe0\xeb\xeb\xe8\xf1\xf2\xe5")
};
DATA(0x004fe764) char* cHeroLevel[KB_HERO_LEVEL_TEXT_COUNT] =
    {/* %s получает */ DATA_COMPGEN(0x0050b53c, cHeroLevelFormatHasGained0, "\x25\x73\x20\xef\xee\xeb\xf3\xf7\xe0\xe5\xf2"), /*  уровень опыта.\n */ DATA_COMPGEN(0x0050b548, cHeroLevelALevel1, "\x20\xf3\xf0\xee\xe2\xe5\xed\xfc\x20\xee\xef\xfb\xf2\xe0\x2e\x0a"), /*  %d уровней опыта.\n */ DATA_COMPGEN(0x0050b55c, cHeroLevelFormatLevels2, "\x20\x25\x64\x20\xf3\xf0\xee\xe2\xed\xe5\xe9\x20\xee\xef\xfb\xf2\xe0\x2e\x0a")};
DATA(0x004fe770) char* cCombatHelp[KB_COMBAT_HELP_COUNT] = {
    /* Подождать, пока походят другие */ DATA_COMPGEN(0x0050b570, cCombatHelpWaitForOtherUnitsToGo0, "\xcf\xee\xe4\xee\xe6\xe4\xe0\xf2\xfc\x2c\x20\xef\xee\xea\xe0\x20\xef\xee\xf5\xee\xe4\xff\xf2\x20\xe4\xf0\xf3\xe3\xe8\xe5"),
    /* Пропустить ход этого воина */ DATA_COMPGEN(0x0050b590, cCombatHelpSkipThisUnit1, "\xcf\xf0\xee\xef\xf3\xf1\xf2\xe8\xf2\xfc\x20\xf5\xee\xe4\x20\xfd\xf2\xee\xe3\xee\x20\xe2\xee\xe8\xed\xe0"),
    /* Автобой */ DATA_COMPGEN(0x0050b5ac, cCombatHelpAutoCombat2, "\xc0\xe2\xf2\xee\xe1\xee\xe9"),
    /* Системные настройки */ DATA_COMPGEN(0x0050b5b4, cCombatHelpCustomizeSystemOptions3, "\xd1\xe8\xf1\xf2\xe5\xec\xed\xfb\xe5\x20\xed\xe0\xf1\xf2\xf0\xee\xe9\xea\xe8"),
    /*  */ DATA_COMPGEN(0x005267e4, cCombatHelpEmpty4, "")
};
DATA(0x004fe784) char* cLongCombatHelp[KB_LONG_COMBAT_HELP_COUNT] = {
    /* {Ждать}\n\nДанный отряд откладывает свой ход и совершает действие после того, как все остальные отряды походили. */ DATA_COMPGEN(0x0050b5c8, cLongCombatHelpWaitHaveTheCurrentCreatureWaitToTake0,
        "\x7b\xc6\xe4\xe0\xf2\xfc\x7d\x0a\x0a\xc4\xe0\xed\xed\xfb\xe9\x20\xee\xf2\xf0\xff\xe4\x20\xee\xf2\xea\xeb\xe0\xe4\xfb\xe2\xe0\xe5\xf2\x20\xf1\xe2\xee\xe9\x20\xf5\xee\xe4\x20\xe8\x20\xf1\xee\xe2"
        "\xe5\xf0\xf8\xe0\xe5\xf2\x20\xe4\xe5\xe9\xf1\xf2\xe2\xe8\xe5\x20\xef\xee\xf1\xeb\xe5\x20\xf2\xee\xe3\xee\x2c\x20\xea\xe0\xea\x20\xe2\xf1\xe5\x20\xee\xf1\xf2\xe0\xeb\xfc\xed\xfb\xe5\x20\xee\xf2"
        "\xf0\xff\xe4\xfb\x20\xef\xee\xf5\xee\xe4\xe8\xeb\xe8\x2e"),
    /* {Пропустить ход}\n\nОтряд пропускает свой ход в этом раунде. */ DATA_COMPGEN(0x0050b638, cLongCombatHelpSkipSkipsTheCurrentCreatureTheCurrentCreature1,
        "\x7b\xcf\xf0\xee\xef\xf3\xf1\xf2\xe8\xf2\xfc\x20\xf5\xee\xe4\x7d\x0a\x0a\xce\xf2\xf0\xff\xe4\x20\xef\xf0\xee\xef\xf3\xf1\xea\xe0\xe5\xf2\x20\xf1\xe2\xee\xe9\x20\xf5\xee\xe4\x20\xe2\x20\xfd\xf2"
        "\xee\xec\x20\xf0\xe0\xf3\xed\xe4\xe5\x2e"),
    /* {Автобой}\n\nКомпьютер вместо вас управляет вашими войсками во время боя. */ DATA_COMPGEN(0x0050b674, cLongCombatHelpAutoCombatAllowsTheComputerToFightOut2,
        "\x7b\xc0\xe2\xf2\xee\xe1\xee\xe9\x7d\x0a\x0a\xca\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\x20\xe2\xec\xe5\xf1\xf2\xee\x20\xe2\xe0\xf1\x20\xf3\xef\xf0\xe0\xe2\xeb\xff\xe5\xf2\x20\xe2\xe0\xf8\xe8\xec\xe8"
        "\x20\xe2\xee\xe9\xf1\xea\xe0\xec\xe8\x20\xe2\xee\x20\xe2\xf0\xe5\xec\xff\x20\xe1\xee\xff\x2e"),
    /* {Настройки}\n\nПозволяет изменять настройки боя. */ DATA_COMPGEN(0x0050b6bc, cLongCombatHelpSystemOptionsAllowsYouToCustomizeTheCombat3, "\x7b\xcd\xe0\xf1\xf2\xf0\xee\xe9\xea\xe8\x7d\x0a\x0a\xcf\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xe8\xe7\xec\xe5\xed\xff\xf2\xfc\x20\xed\xe0\xf1\xf2\xf0\xee\xe9\xea\xe8\x20\xe1\xee\xff\x2e"),
    /* {Информационная строка}\n\nЗдесь отображаются результаты действий отдельных отрядов. */ DATA_COMPGEN(0x0050b6ec, cLongCombatHelpMessageBarShowsTheResultsOfIndividualMonster4,
        "\x7b\xc8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xee\xed\xed\xe0\xff\x20\xf1\xf2\xf0\xee\xea\xe0\x7d\x0a\x0a\xc7\xe4\xe5\xf1\xfc\x20\xee\xf2\xee\xe1\xf0\xe0\xe6\xe0\xfe\xf2\xf1\xff\x20\xf0\xe5\xe7\xf3"
        "\xeb\xfc\xf2\xe0\xf2\xfb\x20\xe4\xe5\xe9\xf1\xf2\xe2\xe8\xe9\x20\xee\xf2\xe4\xe5\xeb\xfc\xed\xfb\xf5\x20\xee\xf2\xf0\xff\xe4\xee\xe2\x2e")
};
DATA(0x004fe798) char* cTownCommand[KB_TOWN_COMMAND_COUNT] = {
    /* Разделить отряд %s */ DATA_COMPGEN(0x0050b740, cTownCommandRedistributeFormatArmy0, "\xd0\xe0\xe7\xe4\xe5\xeb\xe8\xf2\xfc\x20\xee\xf2\xf0\xff\xe4\x20\x25\x73"),
    /* Нельзя отнять последних воинов у героя  */ DATA_COMPGEN(0x0050b754, cTownCommandCannotCombineHeroSLastArmy1, "\xcd\xe5\xeb\xfc\xe7\xff\x20\xee\xf2\xed\xff\xf2\xfc\x20\xef\xee\xf1\xeb\xe5\xe4\xed\xe8\xf5\x20\xe2\xee\xe8\xed\xee\xe2\x20\xf3\x20\xe3\xe5\xf0\xee\xff\x20"),
    /* Соединить отряды %s */ DATA_COMPGEN(0x0050b77c, cTownCommandCombineFormatArmies2, "\xd1\xee\xe5\xe4\xe8\xed\xe8\xf2\xfc\x20\xee\xf2\xf0\xff\xe4\xfb\x20\x25\x73"),
    /* Разделить отряд %s */ DATA_COMPGEN(0x0050b790, cTownCommandRedistributeFormatArmy3, "\xd0\xe0\xe7\xe4\xe5\xeb\xe8\xf2\xfc\x20\xee\xf2\xf0\xff\xe4\x20\x25\x73"),
    /* Посмотреть на %s */ DATA_COMPGEN(0x0050b7a4, cTownCommandViewFormat4, "\xcf\xee\xf1\xec\xee\xf2\xf0\xe5\xf2\xfc\x20\xed\xe0\x20\x25\x73"),
    /* Нельзя перенести в гарнизон последний отряд. */ DATA_COMPGEN(0x0050b7b8, cTownCommandCannotMoveLastArmyToGarrison5, "\xcd\xe5\xeb\xfc\xe7\xff\x20\xef\xe5\xf0\xe5\xed\xe5\xf1\xf2\xe8\x20\xe2\x20\xe3\xe0\xf0\xed\xe8\xe7\xee\xed\x20\xef\xee\xf1\xeb\xe5\xe4\xed\xe8\xe9\x20\xee\xf2\xf0\xff\xe4\x2e"),
    /* Передвинуть сюда отряд %s */ DATA_COMPGEN(0x0050b7e8, cTownCommandMoveFormat6, "\xcf\xe5\xf0\xe5\xe4\xe2\xe8\xed\xf3\xf2\xfc\x20\xf1\xfe\xe4\xe0\x20\xee\xf2\xf0\xff\xe4\x20\x25\x73"),
    /* Отряды %s и %s меняются местами */ DATA_COMPGEN(0x0050b804, cTownCommandExchangeFormatWithFormat7, "\xce\xf2\xf0\xff\xe4\xfb\x20\x25\x73\x20\xe8\x20\x25\x73\x20\xec\xe5\xed\xff\xfe\xf2\xf1\xff\x20\xec\xe5\xf1\xf2\xe0\xec\xe8"),
    /* Выйти из города */ DATA_COMPGEN(0x0050b824, cTownCommandExitTown8, "\xc2\xfb\xe9\xf2\xe8\x20\xe8\xe7\x20\xe3\xee\xf0\xee\xe4\xe0"),
    /*  */ DATA_COMPGEN(0x005267e8, cTownCommandEmpty9, ""),
    /* Обзор королевства */ DATA_COMPGEN(0x0050b834, cTownCommandKingdomOverview10, "\xce\xe1\xe7\xee\xf0\x20\xea\xee\xf0\xee\xeb\xe5\xe2\xf1\xf2\xe2\xe0"),
    /* Пусто */ DATA_COMPGEN(0x0050b848, cTownCommandEmpty11, "\xcf\xf3\xf1\xf2\xee"),
    /* %s */ DATA_COMPGEN(0x0050b850, cTownCommandSelectFormat12, "\x25\x73"),
    /* Показать героя */ DATA_COMPGEN(0x0050b854, cTownCommandViewHero13, "\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xe3\xe5\xf0\xee\xff"),
    /* Гильдия магов */ DATA_COMPGEN(0x0050b864, cTownCommandMageGuild14, "\xc3\xe8\xeb\xfc\xe4\xe8\xff\x20\xec\xe0\xe3\xee\xe2"),
    /* Гильдия воров */ DATA_COMPGEN(0x0050b874, cTownCommandThievesGuild15, "\xc3\xe8\xeb\xfc\xe4\xe8\xff\x20\xe2\xee\xf0\xee\xe2"),
    /* Таверна */ DATA_COMPGEN(0x0050b884, cTownCommandTavern16, "\xd2\xe0\xe2\xe5\xf0\xed\xe0"),
    /* Верфь */ DATA_COMPGEN(0x0050b88c, cTownCommandDock17, "\xc2\xe5\xf0\xf4\xfc"),
    /* Колодец */ DATA_COMPGEN(0x0050b894, cTownCommandWell18, "\xca\xee\xeb\xee\xe4\xe5\xf6"),
    /* Шатер */ DATA_COMPGEN(0x0050b89c, cTownCommandTent19, "\xd8\xe0\xf2\xe5\xf0"),
    /* Замок */ DATA_COMPGEN(0x0050b8a4, cTownCommandCastle20, "\xc7\xe0\xec\xee\xea"),
    /* Нанять %s */ DATA_COMPGEN(0x0050b8ac, cTownCommandRecruitFormat21, "\xcd\xe0\xed\xff\xf2\xfc\x20\x25\x73"),
    /* Статуя */ DATA_COMPGEN(0x0050b8b8, cTownCommandStatue22, "\xd1\xf2\xe0\xf2\xf3\xff"),
    /* Левая башня */ DATA_COMPGEN(0x0050b8c0, cTownCommandLeftTurret23, "\xcb\xe5\xe2\xe0\xff\x20\xe1\xe0\xf8\xed\xff"),
    /* Правая башня */ DATA_COMPGEN(0x0050b8cc, cTownCommandRightTurret24, "\xcf\xf0\xe0\xe2\xe0\xff\x20\xe1\xe0\xf8\xed\xff"),
    /* Ров */ DATA_COMPGEN(0x0050b8dc, cTownCommandMoat25, "\xd0\xee\xe2"),
    /* Рынок */ DATA_COMPGEN(0x0050b8e0, cTownCommandMarketplace26, "\xd0\xfb\xed\xee\xea"),
    /* Дом капитана */ DATA_COMPGEN(0x0050b8e8, cTownCommandCaptainSQuarters27, "\xc4\xee\xec\x20\xea\xe0\xef\xe8\xf2\xe0\xed\xe0")
};
DATA(0x004fe808) char* gHeroDefaultNames[KB_HERO_DEFAULT_NAME_COUNT] = {
    /* Лорд Килбурн */ DATA_COMPGEN(0x0050b8f8, gHeroDefaultNamesLordKilburn0, "\xcb\xee\xf0\xe4\x20\xca\xe8\xeb\xe1\xf3\xf0\xed"), /* Сэр Галлант */ DATA_COMPGEN(0x0050b908, gHeroDefaultNamesSirGallant1, "\xd1\xfd\xf0\x20\xc3\xe0\xeb\xeb\xe0\xed\xf2"), /* Эктор */ DATA_COMPGEN(0x0050b914, gHeroDefaultNamesEctor2, "\xdd\xea\xf2\xee\xf0"),    /* Гвеннет */ DATA_COMPGEN(0x0050b91c, gHeroDefaultNamesGwenneth3, "\xc3\xe2\xe5\xed\xed\xe5\xf2"), /* Тиро */ DATA_COMPGEN(0x0050b924, gHeroDefaultNamesTyro4, "\xd2\xe8\xf0\xee"),    /* Амброзий */ DATA_COMPGEN(0x0050b92c, gHeroDefaultNamesAmbrose5, "\xc0\xec\xe1\xf0\xee\xe7\xe8\xe9"),   /* Руби */ DATA_COMPGEN(0x0050b938, gHeroDefaultNamesRuby6, "\xd0\xf3\xe1\xe8"),
    /* Максимус */ DATA_COMPGEN(0x0050b940, gHeroDefaultNamesMaximus7, "\xcc\xe0\xea\xf1\xe8\xec\xf3\xf1"),      /* Димитри */ DATA_COMPGEN(0x0050b94c, gHeroDefaultNamesDimitri8, "\xc4\xe8\xec\xe8\xf2\xf0\xe8"),     /* Сундакс */ DATA_COMPGEN(0x0050b954, gHeroDefaultNamesThundax9, "\xd1\xf3\xed\xe4\xe0\xea\xf1"),  /* Финеоз */ DATA_COMPGEN(0x0050b95c, gHeroDefaultNamesFineous10, "\xd4\xe8\xed\xe5\xee\xe7"),  /* Джоджош */ DATA_COMPGEN(0x0050b964, gHeroDefaultNamesJojosh11, "\xc4\xe6\xee\xe4\xe6\xee\xf8"),  /* Крэг Хак */ DATA_COMPGEN(0x0050b96c, gHeroDefaultNamesCragHack12, "\xca\xf0\xfd\xe3\x20\xd5\xe0\xea"), /* Джезебель */ DATA_COMPGEN(0x0050b978, gHeroDefaultNamesJezebel13, "\xc4\xe6\xe5\xe7\xe5\xe1\xe5\xeb\xfc"),
    /* Жаклин */ DATA_COMPGEN(0x0050b984, gHeroDefaultNamesJaclyn14, "\xc6\xe0\xea\xeb\xe8\xed"),       /* Эргон */ DATA_COMPGEN(0x0050b98c, gHeroDefaultNamesErgon15, "\xdd\xf0\xe3\xee\xed"),       /* Тсабу */ DATA_COMPGEN(0x0050b994, gHeroDefaultNamesTsabu16, "\xd2\xf1\xe0\xe1\xf3"),    /* Атлас */ DATA_COMPGEN(0x0050b99c, gHeroDefaultNamesAtlas17, "\xc0\xf2\xeb\xe0\xf1"),    /* Астра */ DATA_COMPGEN(0x0050b9a4, gHeroDefaultNamesAstra18, "\xc0\xf1\xf2\xf0\xe0"),   /* Наташа */ DATA_COMPGEN(0x0050b9ac, gHeroDefaultNamesNatasha19, "\xcd\xe0\xf2\xe0\xf8\xe0"),   /* Троян */ DATA_COMPGEN(0x0050b9b4, gHeroDefaultNamesTroyan20, "\xd2\xf0\xee\xff\xed"),
    /* Ватавна */ DATA_COMPGEN(0x0050b9bc, gHeroDefaultNamesVatawna21, "\xc2\xe0\xf2\xe0\xe2\xed\xe0"),      /* Ребекка */ DATA_COMPGEN(0x0050b9c4, gHeroDefaultNamesRebecca22, "\xd0\xe5\xe1\xe5\xea\xea\xe0"),     /* Гем */ DATA_COMPGEN(0x0050b9cc, gHeroDefaultNamesGem23, "\xc3\xe5\xec"),      /* Ариэль */ DATA_COMPGEN(0x0050b9d0, gHeroDefaultNamesAriel24, "\xc0\xf0\xe8\xfd\xeb\xfc"),    /* Карлавн */ DATA_COMPGEN(0x0050b9d8, gHeroDefaultNamesCarlawn25, "\xca\xe0\xf0\xeb\xe0\xe2\xed"), /* Луна */ DATA_COMPGEN(0x0050b9e0, gHeroDefaultNamesLuna26, "\xcb\xf3\xed\xe0"),      /* Арий */ DATA_COMPGEN(0x0050b9e8, gHeroDefaultNamesArie27, "\xc0\xf0\xe8\xe9"),
    /* Аламар */ DATA_COMPGEN(0x0050b9f0, gHeroDefaultNamesAlamar28, "\xc0\xeb\xe0\xec\xe0\xf0"),       /* Виспер */ DATA_COMPGEN(0x0050b9f8, gHeroDefaultNamesVesper29, "\xc2\xe8\xf1\xef\xe5\xf0"),      /* Кродо */ DATA_COMPGEN(0x0050ba00, gHeroDefaultNamesCrodo30, "\xca\xf0\xee\xe4\xee"),    /* Барок */ DATA_COMPGEN(0x0050ba08, gHeroDefaultNamesBarok31, "\xc1\xe0\xf0\xee\xea"),    /* Кастор */ DATA_COMPGEN(0x0050ba10, gHeroDefaultNamesKastore32, "\xca\xe0\xf1\xf2\xee\xf0"), /* Агар */ DATA_COMPGEN(0x0050ba18, gHeroDefaultNamesAgar33, "\xc0\xe3\xe0\xf0"),      /* Фалагар */ DATA_COMPGEN(0x0050ba20, gHeroDefaultNamesFalagar34, "\xd4\xe0\xeb\xe0\xe3\xe0\xf0"),
    /* Расмонт */ DATA_COMPGEN(0x0050ba28, gHeroDefaultNamesWrathmont35, "\xd0\xe0\xf1\xec\xee\xed\xf2"),    /* Мира */ DATA_COMPGEN(0x0050ba30, gHeroDefaultNamesMyra36, "\xcc\xe8\xf0\xe0"),        /* Флинт */ DATA_COMPGEN(0x0050ba38, gHeroDefaultNamesFlint37, "\xd4\xeb\xe8\xed\xf2"),    /* Давн */ DATA_COMPGEN(0x0050ba40, gHeroDefaultNamesDawn38, "\xc4\xe0\xe2\xed"),     /* Галон */ DATA_COMPGEN(0x0050ba48, gHeroDefaultNamesHalon39, "\xc3\xe0\xeb\xee\xed"),   /* Мирини */ DATA_COMPGEN(0x0050ba50, gHeroDefaultNamesMyrini40, "\xcc\xe8\xf0\xe8\xed\xe8"),    /* Вилфрей */ DATA_COMPGEN(0x0050ba58, gHeroDefaultNamesWilfrey41, "\xc2\xe8\xeb\xf4\xf0\xe5\xe9"),
    /* Саракин */ DATA_COMPGEN(0x0050ba60, gHeroDefaultNamesSarakin42, "\xd1\xe0\xf0\xe0\xea\xe8\xed"),      /* Калиндра */ DATA_COMPGEN(0x0050ba68, gHeroDefaultNamesKalindra43, "\xca\xe0\xeb\xe8\xed\xe4\xf0\xe0"),    /* Мандигал */ DATA_COMPGEN(0x0050ba74, gHeroDefaultNamesMandigal44, "\xcc\xe0\xed\xe4\xe8\xe3\xe0\xeb"), /* Зом */ DATA_COMPGEN(0x0050ba80, gHeroDefaultNamesZom45, "\xc7\xee\xec"),      /* Дарлана */ DATA_COMPGEN(0x0050ba84, gHeroDefaultNamesDarlana46, "\xc4\xe0\xf0\xeb\xe0\xed\xe0"), /* Зам */ DATA_COMPGEN(0x0050ba8c, gHeroDefaultNamesZam47, "\xc7\xe0\xec"),       /* Ранлу */ DATA_COMPGEN(0x0050ba90, gHeroDefaultNamesRanloo48, "\xd0\xe0\xed\xeb\xf3"),
    /* Чарити */ DATA_COMPGEN(0x0050ba98, gHeroDefaultNamesCharity49, "\xd7\xe0\xf0\xe8\xf2\xe8"),      /* Риалдо */ DATA_COMPGEN(0x0050baa0, gHeroDefaultNamesRialdo50, "\xd0\xe8\xe0\xeb\xe4\xee"),      /* Роксана */ DATA_COMPGEN(0x0050baa8, gHeroDefaultNamesRoxana51, "\xd0\xee\xea\xf1\xe0\xed\xe0"),   /* Сандро */ DATA_COMPGEN(0x0050bab0, gHeroDefaultNamesSandro52, "\xd1\xe0\xed\xe4\xf0\xee"),   /* Келия */ DATA_COMPGEN(0x0050bab8, gHeroDefaultNamesCelia53, "\xca\xe5\xeb\xe8\xff")
};
DATA(0x004fe8e0) char* gNewGameHelp[KB_NEW_GAME_HELP_COUNT] = {
    /* {Уровень сложности}\n\nЭта опция позволяет устанавливать стартовый уровень сложности игры. Чем выше уровень сложности, тем с меньшим количеством ресурсов вы начинаете игру, и тем больше ресурсов получают ваши компьютерные противники. */ DATA_COMPGEN(0x0050bac0, gNewGameHelpGameDifficultyThisLetsYouChangeTheStarting0,
        "\x7b\xd3\xf0\xee\xe2\xe5\xed\xfc\x20\xf1\xeb\xee\xe6\xed\xee\xf1\xf2\xe8\x7d\x0a\x0a\xdd\xf2\xe0\x20\xee\xef\xf6\xe8\xff\x20\xef\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xf3\xf1\xf2\xe0\xed\xe0\xe2"
        "\xeb\xe8\xe2\xe0\xf2\xfc\x20\xf1\xf2\xe0\xf0\xf2\xee\xe2\xfb\xe9\x20\xf3\xf0\xee\xe2\xe5\xed\xfc\x20\xf1\xeb\xee\xe6\xed\xee\xf1\xf2\xe8\x20\xe8\xe3\xf0\xfb\x2e\x20\xd7\xe5\xec\x20\xe2\xfb\xf8"
        "\xe5\x20\xf3\xf0\xee\xe2\xe5\xed\xfc\x20\xf1\xeb\xee\xe6\xed\xee\xf1\xf2\xe8\x2c\x20\xf2\xe5\xec\x20\xf1\x20\xec\xe5\xed\xfc\xf8\xe8\xec\x20\xea\xee\xeb\xe8\xf7\xe5\xf1\xf2\xe2\xee\xec\x20\xf0"
        "\xe5\xf1\xf3\xf0\xf1\xee\xe2\x20\xe2\xfb\x20\xed\xe0\xf7\xe8\xed\xe0\xe5\xf2\xe5\x20\xe8\xe3\xf0\xf3\x2c\x20\xe8\x20\xf2\xe5\xec\x20\xe1\xee\xeb\xfc\xf8\xe5\x20\xf0\xe5\xf1\xf3\xf0\xf1\xee\xe2"
        "\x20\xef\xee\xeb\xf3\xf7\xe0\xfe\xf2\x20\xe2\xe0\xf8\xe8\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xed\xfb\xe5\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe8\x2e"),
    /* {Фора}\n\nЭта опция позволяет задавать тому или иному игроку-человеку дать фору другим игрокам. Если игрок дает другим фору, он начинает игру с меньшим количеством ресурсов и каждый ход получает на 15 или 30 процентов меньше ресурсов в зависимости от того, насколько большую фору он дает. */ DATA_COMPGEN(0x0050bba8, gNewGameHelpHandicapThisLetsYouChangeTheHandicapOf1,
        "\x7b\xd4\xee\xf0\xe0\x7d\x0a\x0a\xdd\xf2\xe0\x20\xee\xef\xf6\xe8\xff\x20\xef\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xe7\xe0\xe4\xe0\xe2\xe0\xf2\xfc\x20\xf2\xee\xec\xf3\x20\xe8\xeb\xe8\x20\xe8\xed"
        "\xee\xec\xf3\x20\xe8\xe3\xf0\xee\xea\xf3\x2d\xf7\xe5\xeb\xee\xe2\xe5\xea\xf3\x20\xe4\xe0\xf2\xfc\x20\xf4\xee\xf0\xf3\x20\xe4\xf0\xf3\xe3\xe8\xec\x20\xe8\xe3\xf0\xee\xea\xe0\xec\x2e\x20\xc5\xf1"
        "\xeb\xe8\x20\xe8\xe3\xf0\xee\xea\x20\xe4\xe0\xe5\xf2\x20\xe4\xf0\xf3\xe3\xe8\xec\x20\xf4\xee\xf0\xf3\x2c\x20\xee\xed\x20\xed\xe0\xf7\xe8\xed\xe0\xe5\xf2\x20\xe8\xe3\xf0\xf3\x20\xf1\x20\xec\xe5"
        "\xed\xfc\xf8\xe8\xec\x20\xea\xee\xeb\xe8\xf7\xe5\xf1\xf2\xe2\xee\xec\x20\xf0\xe5\xf1\xf3\xf0\xf1\xee\xe2\x20\xe8\x20\xea\xe0\xe6\xe4\xfb\xe9\x20\xf5\xee\xe4\x20\xef\xee\xeb\xf3\xf7\xe0\xe5\xf2"
        "\x20\xed\xe0\x20\x31\x35\x20\xe8\xeb\xe8\x20\x33\x30\x20\xef\xf0\xee\xf6\xe5\xed\xf2\xee\xe2\x20\xec\xe5\xed\xfc\xf8\xe5\x20\xf0\xe5\xf1\xf3\xf0\xf1\xee\xe2\x20\xe2\x20\xe7\xe0\xe2\xe8\xf1\xe8"
        "\xec\xee\xf1\xf2\xe8\x20\xee\xf2\x20\xf2\xee\xe3\xee\x2c\x20\xed\xe0\xf1\xea\xee\xeb\xfc\xea\xee\x20\xe1\xee\xeb\xfc\xf8\xf3\xfe\x20\xf4\xee\xf0\xf3\x20\xee\xed\x20\xe4\xe0\xe5\xf2\x2e"),
    /* {Оппоненты}\n\nЭта опция позволяет вам задать цвет игрока и его стартовую позицию. Каждому цвету соответствует определенная стартовая позиция. Некоторые цвета жестко закреплены либо за компьютерными, либо за живыми игроками. */ DATA_COMPGEN(0x0050bcc8, gNewGameHelpOpponentsThisLetsYouChangePlayerStartingPositions2,
        "\x7b\xce\xef\xef\xee\xed\xe5\xed\xf2\xfb\x7d\x0a\x0a\xdd\xf2\xe0\x20\xee\xef\xf6\xe8\xff\x20\xef\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xe2\xe0\xec\x20\xe7\xe0\xe4\xe0\xf2\xfc\x20\xf6\xe2\xe5\xf2"
        "\x20\xe8\xe3\xf0\xee\xea\xe0\x20\xe8\x20\xe5\xe3\xee\x20\xf1\xf2\xe0\xf0\xf2\xee\xe2\xf3\xfe\x20\xef\xee\xe7\xe8\xf6\xe8\xfe\x2e\x20\xca\xe0\xe6\xe4\xee\xec\xf3\x20\xf6\xe2\xe5\xf2\xf3\x20\xf1"
        "\xee\xee\xf2\xe2\xe5\xf2\xf1\xf2\xe2\xf3\xe5\xf2\x20\xee\xef\xf0\xe5\xe4\xe5\xeb\xe5\xed\xed\xe0\xff\x20\xf1\xf2\xe0\xf0\xf2\xee\xe2\xe0\xff\x20\xef\xee\xe7\xe8\xf6\xe8\xff\x2e\x20\xcd\xe5\xea"
        "\xee\xf2\xee\xf0\xfb\xe5\x20\xf6\xe2\xe5\xf2\xe0\x20\xe6\xe5\xf1\xf2\xea\xee\x20\xe7\xe0\xea\xf0\xe5\xef\xeb\xe5\xed\xfb\x20\xeb\xe8\xe1\xee\x20\xe7\xe0\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0"
        "\xed\xfb\xec\xe8\x2c\x20\xeb\xe8\xe1\xee\x20\xe7\xe0\x20\xe6\xe8\xe2\xfb\xec\xe8\x20\xe8\xe3\xf0\xee\xea\xe0\xec\xe8\x2e"),
    /* {Класс}\n\nЭта опция позволяет задавать класс игрока. Классы не всегда можно изменять. В зависимости от сценария игрок может получать дополнительные города и/или героев, направленность которых не совпадает с изначальной направленностью игрока. */ DATA_COMPGEN(0x0050bda8, gNewGameHelpClassThisLetsYouChangeTheClassOf3,
        "\x7b\xca\xeb\xe0\xf1\xf1\x7d\x0a\x0a\xdd\xf2\xe0\x20\xee\xef\xf6\xe8\xff\x20\xef\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xe7\xe0\xe4\xe0\xe2\xe0\xf2\xfc\x20\xea\xeb\xe0\xf1\xf1\x20\xe8\xe3\xf0\xee"
        "\xea\xe0\x2e\x20\xca\xeb\xe0\xf1\xf1\xfb\x20\xed\xe5\x20\xe2\xf1\xe5\xe3\xe4\xe0\x20\xec\xee\xe6\xed\xee\x20\xe8\xe7\xec\xe5\xed\xff\xf2\xfc\x2e\x20\xc2\x20\xe7\xe0\xe2\xe8\xf1\xe8\xec\xee\xf1"
        "\xf2\xe8\x20\xee\xf2\x20\xf1\xf6\xe5\xed\xe0\xf0\xe8\xff\x20\xe8\xe3\xf0\xee\xea\x20\xec\xee\xe6\xe5\xf2\x20\xef\xee\xeb\xf3\xf7\xe0\xf2\xfc\x20\xe4\xee\xef\xee\xeb\xed\xe8\xf2\xe5\xeb\xfc\xed"
        "\xfb\xe5\x20\xe3\xee\xf0\xee\xe4\xe0\x20\xe8\x2f\xe8\xeb\xe8\x20\xe3\xe5\xf0\xee\xe5\xe2\x2c\x20\xed\xe0\xef\xf0\xe0\xe2\xeb\xe5\xed\xed\xee\xf1\xf2\xfc\x20\xea\xee\xf2\xee\xf0\xfb\xf5\x20\xed"
        "\xe5\x20\xf1\xee\xe2\xef\xe0\xe4\xe0\xe5\xf2\x20\xf1\x20\xe8\xe7\xed\xe0\xf7\xe0\xeb\xfc\xed\xee\xe9\x20\xed\xe0\xef\xf0\xe0\xe2\xeb\xe5\xed\xed\xee\xf1\xf2\xfc\xfe\x20\xe8\xe3\xf0\xee\xea\xe0"
        "\x2e"),
    /* {Сценарий}\n\nЭта опция позволяет выбрать игровой сценарий. */ DATA_COMPGEN(0x0050be9c, gNewGameHelpScenarioClickHereToSelectWhichScenarioTo4,
        "\x7b\xd1\xf6\xe5\xed\xe0\xf0\xe8\xe9\x7d\x0a\x0a\xdd\xf2\xe0\x20\xee\xef\xf6\xe8\xff\x20\xef\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xe2\xfb\xe1\xf0\xe0\xf2\xfc\x20\xe8\xe3\xf0\xee\xe2\xee\xe9\x20"
        "\xf1\xf6\xe5\xed\xe0\xf0\xe8\xe9\x2e"),
    /* {Рейтинг}\n\nРейтинг отражает сочетание различных игровых установок. Он используется при расчете конечного результата, достигнутого игроком. */ DATA_COMPGEN(0x0050bed8, gNewGameHelpDifficultyRatingTheDifficultyRatingReflectsACombination5,
        "\x7b\xd0\xe5\xe9\xf2\xe8\xed\xe3\x7d\x0a\x0a\xd0\xe5\xe9\xf2\xe8\xed\xe3\x20\xee\xf2\xf0\xe0\xe6\xe0\xe5\xf2\x20\xf1\xee\xf7\xe5\xf2\xe0\xed\xe8\xe5\x20\xf0\xe0\xe7\xeb\xe8\xf7\xed\xfb\xf5\x20"
        "\xe8\xe3\xf0\xee\xe2\xfb\xf5\x20\xf3\xf1\xf2\xe0\xed\xee\xe2\xee\xea\x2e\x20\xce\xed\x20\xe8\xf1\xef\xee\xeb\xfc\xe7\xf3\xe5\xf2\xf1\xff\x20\xef\xf0\xe8\x20\xf0\xe0\xf1\xf7\xe5\xf2\xe5\x20\xea"
        "\xee\xed\xe5\xf7\xed\xee\xe3\xee\x20\xf0\xe5\xe7\xf3\xeb\xfc\xf2\xe0\xf2\xe0\x2c\x20\xe4\xee\xf1\xf2\xe8\xe3\xed\xf3\xf2\xee\xe3\xee\x20\xe8\xe3\xf0\xee\xea\xee\xec\x2e"),
    /* {ОК}\n\nПодтверждает заданные установки и начинает новую игру. */ DATA_COMPGEN(0x0050bf64, gNewGameHelpOKClickToAcceptTheseSettingsAndStart6,
        "\x7b\xce\xca\x7d\x0a\x0a\xcf\xee\xe4\xf2\xe2\xe5\xf0\xe6\xe4\xe0\xe5\xf2\x20\xe7\xe0\xe4\xe0\xed\xed\xfb\xe5\x20\xf3\xf1\xf2\xe0\xed\xee\xe2\xea\xe8\x20\xe8\x20\xed\xe0\xf7\xe8\xed\xe0\xe5\xf2"
        "\x20\xed\xee\xe2\xf3\xfe\x20\xe8\xe3\xf0\xf3\x2e"),
    /* {Отмена}\n\nНажмите, чтобы вернуться в главное меню. */ DATA_COMPGEN(0x0050bfa4, gNewGameHelpCancelClickToReturnToTheMainMenu7,
        "\x7b\xce\xf2\xec\xe5\xed\xe0\x7d\x0a\x0a\xcd\xe0\xe6\xec\xe8\xf2\xe5\x2c\x20\xf7\xf2\xee\xe1\xfb\x20\xe2\xe5\xf0\xed\xf3\xf2\xfc\xf1\xff\x20\xe2\x20\xe3\xeb\xe0\xe2\xed\xee\xe5\x20\xec\xe5\xed"
        "\xfe\x2e")
};
DATA(0x004fe900) char* gSetupBaudHelp[KB_SETUP_BAUD_HELP_COUNT] = {
    /* {2400 бод}\n\nИспользовать соединение на скорости 2400 бод.\n\nЗамечание: Для модемов 14400 бод используйте соединение на скорости 19200.  Для модемов 28800 бод используйте соединение на скорости 38400 бод. */ DATA_COMPGEN(0x0050bfd8, gSetupBaudHelp2400BaudUseA2400BaudConnectionSpeed0,
        "\x7b\x32\x34\x30\x30\x20\xe1\xee\xe4\x7d\x0a\x0a\xc8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xf2\xfc\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5\x20\xed\xe0\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xe8\x20"
        "\x32\x34\x30\x30\x20\xe1\xee\xe4\x2e\x0a\x0a\xc7\xe0\xec\xe5\xf7\xe0\xed\xe8\xe5\x3a\x20\xc4\xeb\xff\x20\xec\xee\xe4\xe5\xec\xee\xe2\x20\x31\x34\x34\x30\x30\x20\xe1\xee\xe4\x20\xe8\xf1\xef\xee"
        "\xeb\xfc\xe7\xf3\xe9\xf2\xe5\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5\x20\xed\xe0\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xe8\x20\x31\x39\x32\x30\x30\x2e\x20\x20\xc4\xeb\xff\x20\xec\xee\xe4\xe5\xec"
        "\xee\xe2\x20\x32\x38\x38\x30\x30\x20\xe1\xee\xe4\x20\xe8\xf1\xef\xee\xeb\xfc\xe7\xf3\xe9\xf2\xe5\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5\x20\xed\xe0\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xe8\x20"
        "\x33\x38\x34\x30\x30\x20\xe1\xee\xe4\x2e"),
    /* {9600 бод}\n\nИспользовать соединение на скорости 9600 бод.\n\nЗамечание: Для модемов 14400 бод используйте соединение на скорости 19200.  Для модемов 28800 бод используйте соединение на скорости 38400 бод. */ DATA_COMPGEN(0x0050c0a4, gSetupBaudHelp9600BaudUseA9600BaudConnectionSpeed1,
        "\x7b\x39\x36\x30\x30\x20\xe1\xee\xe4\x7d\x0a\x0a\xc8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xf2\xfc\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5\x20\xed\xe0\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xe8\x20"
        "\x39\x36\x30\x30\x20\xe1\xee\xe4\x2e\x0a\x0a\xc7\xe0\xec\xe5\xf7\xe0\xed\xe8\xe5\x3a\x20\xc4\xeb\xff\x20\xec\xee\xe4\xe5\xec\xee\xe2\x20\x31\x34\x34\x30\x30\x20\xe1\xee\xe4\x20\xe8\xf1\xef\xee"
        "\xeb\xfc\xe7\xf3\xe9\xf2\xe5\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5\x20\xed\xe0\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xe8\x20\x31\x39\x32\x30\x30\x2e\x20\x20\xc4\xeb\xff\x20\xec\xee\xe4\xe5\xec"
        "\xee\xe2\x20\x32\x38\x38\x30\x30\x20\xe1\xee\xe4\x20\xe8\xf1\xef\xee\xeb\xfc\xe7\xf3\xe9\xf2\xe5\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5\x20\xed\xe0\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xe8\x20"
        "\x33\x38\x34\x30\x30\x20\xe1\xee\xe4\x2e"),
    /* {19200 бод}\n\nИспользовать соединение на скорости 19200 бод.\n\nЗамечание: Для модемов 14400 бод используйте соединение на скорости 19200.  Для модемов 28800 бод используйте соединение на скорости 38400 бод. */ DATA_COMPGEN(0x0050c170, gSetupBaudHelp19200BaudUseA19200BaudConnectionSpeed2,
        "\x7b\x31\x39\x32\x30\x30\x20\xe1\xee\xe4\x7d\x0a\x0a\xc8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xf2\xfc\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5\x20\xed\xe0\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xe8"
        "\x20\x31\x39\x32\x30\x30\x20\xe1\xee\xe4\x2e\x0a\x0a\xc7\xe0\xec\xe5\xf7\xe0\xed\xe8\xe5\x3a\x20\xc4\xeb\xff\x20\xec\xee\xe4\xe5\xec\xee\xe2\x20\x31\x34\x34\x30\x30\x20\xe1\xee\xe4\x20\xe8\xf1"
        "\xef\xee\xeb\xfc\xe7\xf3\xe9\xf2\xe5\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5\x20\xed\xe0\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xe8\x20\x31\x39\x32\x30\x30\x2e\x20\x20\xc4\xeb\xff\x20\xec\xee\xe4"
        "\xe5\xec\xee\xe2\x20\x32\x38\x38\x30\x30\x20\xe1\xee\xe4\x20\xe8\xf1\xef\xee\xeb\xfc\xe7\xf3\xe9\xf2\xe5\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5\x20\xed\xe0\x20\xf1\xea\xee\xf0\xee\xf1\xf2"
        "\xe8\x20\x33\x38\x34\x30\x30\x20\xe1\xee\xe4\x2e"),
    /* {38400 бод}\n\nИспользовать соединение на скорости 38400 бод.\n\nЗамечание: Для модемов 14400 бод используйте соединение на скорости 19200.  Для модемов 28800 бод используйте соединение на скорости 38400 бод. */ DATA_COMPGEN(0x0050c240, gSetupBaudHelp38400BaudUseA38400BaudConnectionSpeed3,
        "\x7b\x33\x38\x34\x30\x30\x20\xe1\xee\xe4\x7d\x0a\x0a\xc8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xf2\xfc\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5\x20\xed\xe0\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xe8"
        "\x20\x33\x38\x34\x30\x30\x20\xe1\xee\xe4\x2e\x0a\x0a\xc7\xe0\xec\xe5\xf7\xe0\xed\xe8\xe5\x3a\x20\xc4\xeb\xff\x20\xec\xee\xe4\xe5\xec\xee\xe2\x20\x31\x34\x34\x30\x30\x20\xe1\xee\xe4\x20\xe8\xf1"
        "\xef\xee\xeb\xfc\xe7\xf3\xe9\xf2\xe5\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5\x20\xed\xe0\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xe8\x20\x31\x39\x32\x30\x30\x2e\x20\x20\xc4\xeb\xff\x20\xec\xee\xe4"
        "\xe5\xec\xee\xe2\x20\x32\x38\x38\x30\x30\x20\xe1\xee\xe4\x20\xe8\xf1\xef\xee\xeb\xfc\xe7\xf3\xe9\xf2\xe5\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5\x20\xed\xe0\x20\xf1\xea\xee\xf0\xee\xf1\xf2"
        "\xe8\x20\x33\x38\x34\x30\x30\x20\xe1\xee\xe4\x2e"),
    /* {Отменить}\n\nОтменить и вернуться в главное меню. */ DATA_COMPGEN(0x0050c310, gSetupBaudHelpCancelCancelBackToTheMainMenu4, "\x7b\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x7d\x0a\x0a\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x20\xe8\x20\xe2\xe5\xf0\xed\xf3\xf2\xfc\xf1\xff\x20\xe2\x20\xe3\xeb\xe0\xe2\xed\xee\xe5\x20\xec\xe5\xed\xfe\x2e")
};
DATA(0x004fe914) char* gSetupComPortHelp[KB_SETUP_COM_PORT_HELP_COUNT] = {
    /* {COM 1}\n\nИспользовать для модемного соединения порт COM 1. */ DATA_COMPGEN(0x0050c344, gSetupComPortHelpCOM1UseCOMPort1ForThe0,
        "\x7b\x43\x4f\x4d\x20\x31\x7d\x0a\x0a\xc8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xf2\xfc\x20\xe4\xeb\xff\x20\xec\xee\xe4\xe5\xec\xed\xee\xe3\xee\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xff\x20\xef"
        "\xee\xf0\xf2\x20\x43\x4f\x4d\x20\x31\x2e"),
    /* {COM 2}\n\nИспользовать для модемного соединения порт COM 2. */ DATA_COMPGEN(0x0050c380, gSetupComPortHelpCOM2UseCOMPort2ForThe1,
        "\x7b\x43\x4f\x4d\x20\x32\x7d\x0a\x0a\xc8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xf2\xfc\x20\xe4\xeb\xff\x20\xec\xee\xe4\xe5\xec\xed\xee\xe3\xee\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xff\x20\xef"
        "\xee\xf0\xf2\x20\x43\x4f\x4d\x20\x32\x2e"),
    /* {COM 3}\n\nИспользовать для модемного соединения порт COM 3. */ DATA_COMPGEN(0x0050c3bc, gSetupComPortHelpCOM3UseCOMPort3ForThe2,
        "\x7b\x43\x4f\x4d\x20\x33\x7d\x0a\x0a\xc8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xf2\xfc\x20\xe4\xeb\xff\x20\xec\xee\xe4\xe5\xec\xed\xee\xe3\xee\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xff\x20\xef"
        "\xee\xf0\xf2\x20\x43\x4f\x4d\x20\x33\x2e"),
    /* {COM 4}\n\nИспользовать для модемного соединения порт COM 4. */ DATA_COMPGEN(0x0050c3f8, gSetupComPortHelpCOM4UseCOMPort4ForThe3,
        "\x7b\x43\x4f\x4d\x20\x34\x7d\x0a\x0a\xc8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xf2\xfc\x20\xe4\xeb\xff\x20\xec\xee\xe4\xe5\xec\xed\xee\xe3\xee\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xff\x20\xef"
        "\xee\xf0\xf2\x20\x43\x4f\x4d\x20\x34\x2e"),
    /* {Отменить}\n\nОтменить и вернуться в главное меню. */ DATA_COMPGEN(0x0050c434, gSetupComPortHelpCancelCancelBackToTheMainMenu4, "\x7b\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x7d\x0a\x0a\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x20\xe8\x20\xe2\xe5\xf0\xed\xf3\xf2\xfc\xf1\xff\x20\xe2\x20\xe3\xeb\xe0\xe2\xed\xee\xe5\x20\xec\xe5\xed\xfe\x2e")
};
DATA(0x004fe928) char* gSetupDCBaudHelp[KB_SETUP_DC_BAUD_HELP_COUNT] = {
    /* {Скорость соединения 2400 бод.}\n\nДля компьютеров с устаревшим чипом UART 8250 следует использовать скорость 19200 бод, а для компьютеров с более современным чипом UART 16550 - скорость 38400 бод. Если вы не уверены, какой у вас чип, начните с более низких скоростей. В большинстве компьютеров, произведенных в 1994 году и позднее, используется чип UART 16550. */ DATA_COMPGEN(0x0050c468, gSetupDCBaudHelp2400BaudUseA2400BaudConnectionSpeed0,
        "\x7b\xd1\xea\xee\xf0\xee\xf1\xf2\xfc\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xff\x20\x32\x34\x30\x30\x20\xe1\xee\xe4\x2e\x7d\x0a\x0a\xc4\xeb\xff\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xee\xe2"
        "\x20\xf1\x20\xf3\xf1\xf2\xe0\xf0\xe5\xe2\xf8\xe8\xec\x20\xf7\xe8\xef\xee\xec\x20\x55\x41\x52\x54\x20\x38\x32\x35\x30\x20\xf1\xeb\xe5\xe4\xf3\xe5\xf2\x20\xe8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0"
        "\xf2\xfc\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xfc\x20\x31\x39\x32\x30\x30\x20\xe1\xee\xe4\x2c\x20\xe0\x20\xe4\xeb\xff\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xee\xe2\x20\xf1\x20\xe1\xee\xeb\xe5\xe5"
        "\x20\xf1\xee\xe2\xf0\xe5\xec\xe5\xed\xed\xfb\xec\x20\xf7\xe8\xef\xee\xec\x20\x55\x41\x52\x54\x20\x31\x36\x35\x35\x30\x20\x2d\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xfc\x20\x33\x38\x34\x30\x30\x20\xe1"
        "\xee\xe4\x2e\x20\xc5\xf1\xeb\xe8\x20\xe2\xfb\x20\xed\xe5\x20\xf3\xe2\xe5\xf0\xe5\xed\xfb\x2c\x20\xea\xe0\xea\xee\xe9\x20\xf3\x20\xe2\xe0\xf1\x20\xf7\xe8\xef\x2c\x20\xed\xe0\xf7\xed\xe8\xf2\xe5"
        "\x20\xf1\x20\xe1\xee\xeb\xe5\xe5\x20\xed\xe8\xe7\xea\xe8\xf5\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xe5\xe9\x2e\x20\xc2\x20\xe1\xee\xeb\xfc\xf8\xe8\xed\xf1\xf2\xe2\xe5\x20\xea\xee\xec\xef\xfc\xfe\xf2"
        "\xe5\xf0\xee\xe2\x2c\x20\xef\xf0\xee\xe8\xe7\xe2\xe5\xe4\xe5\xed\xed\xfb\xf5\x20\xe2\x20\x31\x39\x39\x34\x20\xe3\xee\xe4\xf3\x20\xe8\x20\xef\xee\xe7\xe4\xed\xe5\xe5\x2c\x20\xe8\xf1\xef\xee\xeb"
        "\xfc\xe7\xf3\xe5\xf2\xf1\xff\x20\xf7\xe8\xef\x20\x55\x41\x52\x54\x20\x31\x36\x35\x35\x30\x2e"),
    /* {Скорость соединения 9600 бод.}\n\n Для компьютеров с устаревшим чипом UART 8250 следует использовать скорость 19200 бод, а для компьютеров с более современным чипом UART 16550 - скорость 38400 бод. Если вы не уверены, какой у вас чип, начните с более низких скоростей. В большинстве компьютеров, произведенных в 1994 году и позднее, используется чип UART 16550. */ DATA_COMPGEN(0x0050c5d0, gSetupDCBaudHelp9600BaudUseA9600BaudConnectionSpeed1,
        "\x7b\xd1\xea\xee\xf0\xee\xf1\xf2\xfc\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xff\x20\x39\x36\x30\x30\x20\xe1\xee\xe4\x2e\x7d\x0a\x0a\x20\xc4\xeb\xff\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xee"
        "\xe2\x20\xf1\x20\xf3\xf1\xf2\xe0\xf0\xe5\xe2\xf8\xe8\xec\x20\xf7\xe8\xef\xee\xec\x20\x55\x41\x52\x54\x20\x38\x32\x35\x30\x20\xf1\xeb\xe5\xe4\xf3\xe5\xf2\x20\xe8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2"
        "\xe0\xf2\xfc\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xfc\x20\x31\x39\x32\x30\x30\x20\xe1\xee\xe4\x2c\x20\xe0\x20\xe4\xeb\xff\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xee\xe2\x20\xf1\x20\xe1\xee\xeb\xe5"
        "\xe5\x20\xf1\xee\xe2\xf0\xe5\xec\xe5\xed\xed\xfb\xec\x20\xf7\xe8\xef\xee\xec\x20\x55\x41\x52\x54\x20\x31\x36\x35\x35\x30\x20\x2d\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xfc\x20\x33\x38\x34\x30\x30\x20"
        "\xe1\xee\xe4\x2e\x20\xc5\xf1\xeb\xe8\x20\xe2\xfb\x20\xed\xe5\x20\xf3\xe2\xe5\xf0\xe5\xed\xfb\x2c\x20\xea\xe0\xea\xee\xe9\x20\xf3\x20\xe2\xe0\xf1\x20\xf7\xe8\xef\x2c\x20\xed\xe0\xf7\xed\xe8\xf2"
        "\xe5\x20\xf1\x20\xe1\xee\xeb\xe5\xe5\x20\xed\xe8\xe7\xea\xe8\xf5\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xe5\xe9\x2e\x20\xc2\x20\xe1\xee\xeb\xfc\xf8\xe8\xed\xf1\xf2\xe2\xe5\x20\xea\xee\xec\xef\xfc\xfe"
        "\xf2\xe5\xf0\xee\xe2\x2c\x20\xef\xf0\xee\xe8\xe7\xe2\xe5\xe4\xe5\xed\xed\xfb\xf5\x20\xe2\x20\x31\x39\x39\x34\x20\xe3\xee\xe4\xf3\x20\xe8\x20\xef\xee\xe7\xe4\xed\xe5\xe5\x2c\x20\xe8\xf1\xef\xee"
        "\xeb\xfc\xe7\xf3\xe5\xf2\xf1\xff\x20\xf7\xe8\xef\x20\x55\x41\x52\x54\x20\x31\x36\x35\x35\x30\x2e"),
    /* {Скорость соединения 19200 бод.}\n\n Для компьютеров с устаревшим чипом UART 8250 следует использовать скорость 19200 бод, а для компьютеров с более современным чипом UART 16550 - скорость 38400 бод. Если вы не уверены, какой у вас чип, начните с более низких скоростей. В большинстве компьютеров, произведенных в 1994 году и позднее, используется чип UART 16550. */ DATA_COMPGEN(0x0050c73c, gSetupDCBaudHelp19200BaudUseA19200BaudConnectionSpeed2,
        "\x7b\xd1\xea\xee\xf0\xee\xf1\xf2\xfc\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xff\x20\x31\x39\x32\x30\x30\x20\xe1\xee\xe4\x2e\x7d\x0a\x0a\x20\xc4\xeb\xff\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0"
        "\xee\xe2\x20\xf1\x20\xf3\xf1\xf2\xe0\xf0\xe5\xe2\xf8\xe8\xec\x20\xf7\xe8\xef\xee\xec\x20\x55\x41\x52\x54\x20\x38\x32\x35\x30\x20\xf1\xeb\xe5\xe4\xf3\xe5\xf2\x20\xe8\xf1\xef\xee\xeb\xfc\xe7\xee"
        "\xe2\xe0\xf2\xfc\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xfc\x20\x31\x39\x32\x30\x30\x20\xe1\xee\xe4\x2c\x20\xe0\x20\xe4\xeb\xff\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xee\xe2\x20\xf1\x20\xe1\xee\xeb"
        "\xe5\xe5\x20\xf1\xee\xe2\xf0\xe5\xec\xe5\xed\xed\xfb\xec\x20\xf7\xe8\xef\xee\xec\x20\x55\x41\x52\x54\x20\x31\x36\x35\x35\x30\x20\x2d\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xfc\x20\x33\x38\x34\x30\x30"
        "\x20\xe1\xee\xe4\x2e\x20\xc5\xf1\xeb\xe8\x20\xe2\xfb\x20\xed\xe5\x20\xf3\xe2\xe5\xf0\xe5\xed\xfb\x2c\x20\xea\xe0\xea\xee\xe9\x20\xf3\x20\xe2\xe0\xf1\x20\xf7\xe8\xef\x2c\x20\xed\xe0\xf7\xed\xe8"
        "\xf2\xe5\x20\xf1\x20\xe1\xee\xeb\xe5\xe5\x20\xed\xe8\xe7\xea\xe8\xf5\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xe5\xe9\x2e\x20\xc2\x20\xe1\xee\xeb\xfc\xf8\xe8\xed\xf1\xf2\xe2\xe5\x20\xea\xee\xec\xef\xfc"
        "\xfe\xf2\xe5\xf0\xee\xe2\x2c\x20\xef\xf0\xee\xe8\xe7\xe2\xe5\xe4\xe5\xed\xed\xfb\xf5\x20\xe2\x20\x31\x39\x39\x34\x20\xe3\xee\xe4\xf3\x20\xe8\x20\xef\xee\xe7\xe4\xed\xe5\xe5\x2c\x20\xe8\xf1\xef"
        "\xee\xeb\xfc\xe7\xf3\xe5\xf2\xf1\xff\x20\xf7\xe8\xef\x20\x55\x41\x52\x54\x20\x31\x36\x35\x35\x30\x2e"),
    /* {Скорость соединения 38400 бод.}\n\n Для компьютеров с устаревшим чипом UART 8250 следует использовать скорость 19200 бод, а для компьютеров с более современным чипом UART 16550 - скорость 38400 бод. Если вы не уверены, какой у вас чип, начните с более низких скоростей. В большинстве компьютеров, произведенных в 1994 году и позднее, используется чип UART 16550. */ DATA_COMPGEN(0x0050c8a8, gSetupDCBaudHelp38400BaudUseA38400BaudConnectionSpeed3,
        "\x7b\xd1\xea\xee\xf0\xee\xf1\xf2\xfc\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xff\x20\x33\x38\x34\x30\x30\x20\xe1\xee\xe4\x2e\x7d\x0a\x0a\x20\xc4\xeb\xff\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0"
        "\xee\xe2\x20\xf1\x20\xf3\xf1\xf2\xe0\xf0\xe5\xe2\xf8\xe8\xec\x20\xf7\xe8\xef\xee\xec\x20\x55\x41\x52\x54\x20\x38\x32\x35\x30\x20\xf1\xeb\xe5\xe4\xf3\xe5\xf2\x20\xe8\xf1\xef\xee\xeb\xfc\xe7\xee"
        "\xe2\xe0\xf2\xfc\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xfc\x20\x31\x39\x32\x30\x30\x20\xe1\xee\xe4\x2c\x20\xe0\x20\xe4\xeb\xff\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xee\xe2\x20\xf1\x20\xe1\xee\xeb"
        "\xe5\xe5\x20\xf1\xee\xe2\xf0\xe5\xec\xe5\xed\xed\xfb\xec\x20\xf7\xe8\xef\xee\xec\x20\x55\x41\x52\x54\x20\x31\x36\x35\x35\x30\x20\x2d\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xfc\x20\x33\x38\x34\x30\x30"
        "\x20\xe1\xee\xe4\x2e\x20\xc5\xf1\xeb\xe8\x20\xe2\xfb\x20\xed\xe5\x20\xf3\xe2\xe5\xf0\xe5\xed\xfb\x2c\x20\xea\xe0\xea\xee\xe9\x20\xf3\x20\xe2\xe0\xf1\x20\xf7\xe8\xef\x2c\x20\xed\xe0\xf7\xed\xe8"
        "\xf2\xe5\x20\xf1\x20\xe1\xee\xeb\xe5\xe5\x20\xed\xe8\xe7\xea\xe8\xf5\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xe5\xe9\x2e\x20\xc2\x20\xe1\xee\xeb\xfc\xf8\xe8\xed\xf1\xf2\xe2\xe5\x20\xea\xee\xec\xef\xfc"
        "\xfe\xf2\xe5\xf0\xee\xe2\x2c\x20\xef\xf0\xee\xe8\xe7\xe2\xe5\xe4\xe5\xed\xed\xfb\xf5\x20\xe2\x20\x31\x39\x39\x34\x20\xe3\xee\xe4\xf3\x20\xe8\x20\xef\xee\xe7\xe4\xed\xe5\xe5\x2c\x20\xe8\xf1\xef"
        "\xee\xeb\xfc\xe7\xf3\xe5\xf2\xf1\xff\x20\xf7\xe8\xef\x20\x55\x41\x52\x54\x20\x31\x36\x35\x35\x30\x2e"),
    /* {Отменить}\n\nОтменить и вернуться в главное меню. */ DATA_COMPGEN(0x0050ca14, gSetupDCBaudHelpCancelCancelBackToTheMainMenu4, "\x7b\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x7d\x0a\x0a\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x20\xe8\x20\xe2\xe5\xf0\xed\xf3\xf2\xfc\xf1\xff\x20\xe2\x20\xe3\xeb\xe0\xe2\xed\xee\xe5\x20\xec\xe5\xed\xfe\x2e")
};
DATA(0x004fe93c) char* gSetupDCComPortHelp[KB_SETUP_DC_COM_PORT_HELP_COUNT] = {
    /* {COM 1}\n\nИспользовать для прямого соединения порт COM 1. */ DATA_COMPGEN(0x0050ca48, gSetupDCComPortHelpCOM1UseCOMPort1ForThe0,
        "\x7b\x43\x4f\x4d\x20\x31\x7d\x0a\x0a\xc8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xf2\xfc\x20\xe4\xeb\xff\x20\xef\xf0\xff\xec\xee\xe3\xee\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xff\x20\xef\xee\xf0"
        "\xf2\x20\x43\x4f\x4d\x20\x31\x2e"),
    /* {COM 2}\n\nИспользовать для прямого соединения порт COM 2. */ DATA_COMPGEN(0x0050ca84, gSetupDCComPortHelpCOM2UseCOMPort2ForThe1,
        "\x7b\x43\x4f\x4d\x20\x32\x7d\x0a\x0a\xc8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xf2\xfc\x20\xe4\xeb\xff\x20\xef\xf0\xff\xec\xee\xe3\xee\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xff\x20\xef\xee\xf0"
        "\xf2\x20\x43\x4f\x4d\x20\x32\x2e"),
    /* {COM 3}\n\nИспользовать для прямого соединения порт COM 3. */ DATA_COMPGEN(0x0050cac0, gSetupDCComPortHelpCOM3UseCOMPort3ForThe2,
        "\x7b\x43\x4f\x4d\x20\x33\x7d\x0a\x0a\xc8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xf2\xfc\x20\xe4\xeb\xff\x20\xef\xf0\xff\xec\xee\xe3\xee\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xff\x20\xef\xee\xf0"
        "\xf2\x20\x43\x4f\x4d\x20\x33\x2e"),
    /* {COM 4}\n\nИспользовать для прямого соединения порт COM 4. */ DATA_COMPGEN(0x0050cafc, gSetupDCComPortHelpCom4UseCOMPort4ForThe3,
        "\x7b\x43\x4f\x4d\x20\x34\x7d\x0a\x0a\xc8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xf2\xfc\x20\xe4\xeb\xff\x20\xef\xf0\xff\xec\xee\xe3\xee\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xff\x20\xef\xee\xf0"
        "\xf2\x20\x43\x4f\x4d\x20\x34\x2e"),
    /* {Отменить}\n\nОтменить и вернуться в главное меню. */ DATA_COMPGEN(0x0050cb38, gSetupDCComPortHelpCancelCancelBackToTheMainMenu4, "\x7b\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x7d\x0a\x0a\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x20\xe8\x20\xe2\xe5\xf0\xed\xf3\xf2\xfc\xf1\xff\x20\xe2\x20\xe3\xeb\xe0\xe2\xed\xee\xe5\x20\xec\xe5\xed\xfe\x2e")
};
DATA(0x004fe950) char* gSetupHotSeatGameHelp[KB_SETUP_HOT_SEAT_HELP_COUNT] = {
    /* {2 игрока}\n\nИграть с 2 людьми и, опционально, до 4 дополнительных компьютерных игроков. */ DATA_COMPGEN(0x0050cb6c, gSetupHotSeatGameHelp2PlayersPlayWith2HumanPlayersAnd0,
        "\x7b\x32\x20\xe8\xe3\xf0\xee\xea\xe0\x7d\x0a\x0a\xc8\xe3\xf0\xe0\xf2\xfc\x20\xf1\x20\x32\x20\xeb\xfe\xe4\xfc\xec\xe8\x20\xe8\x2c\x20\xee\xef\xf6\xe8\xee\xed\xe0\xeb\xfc\xed\xee\x2c\x20\xe4\xee"
        "\x20\x34\x20\xe4\xee\xef\xee\xeb\xed\xe8\xf2\xe5\xeb\xfc\xed\xfb\xf5\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xed\xfb\xf5\x20\xe8\xe3\xf0\xee\xea\xee\xe2\x2e"),
    /* {3 игрока}\n\nИграть с 3 людьми и, опционально, до 3 дополнительных компьютерных игроков. */ DATA_COMPGEN(0x0050cbc4, gSetupHotSeatGameHelp3PlayersPlayWith3HumanPlayersAnd1,
        "\x7b\x33\x20\xe8\xe3\xf0\xee\xea\xe0\x7d\x0a\x0a\xc8\xe3\xf0\xe0\xf2\xfc\x20\xf1\x20\x33\x20\xeb\xfe\xe4\xfc\xec\xe8\x20\xe8\x2c\x20\xee\xef\xf6\xe8\xee\xed\xe0\xeb\xfc\xed\xee\x2c\x20\xe4\xee"
        "\x20\x33\x20\xe4\xee\xef\xee\xeb\xed\xe8\xf2\xe5\xeb\xfc\xed\xfb\xf5\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xed\xfb\xf5\x20\xe8\xe3\xf0\xee\xea\xee\xe2\x2e"),
    /* {4 игрока}\n\nИграть с 4 людьми и, опционально, до 2 дополнительных компьютерных игроков. */ DATA_COMPGEN(0x0050cc1c, gSetupHotSeatGameHelp4PlayersPlayWith4HumanPlayersAnd2,
        "\x7b\x34\x20\xe8\xe3\xf0\xee\xea\xe0\x7d\x0a\x0a\xc8\xe3\xf0\xe0\xf2\xfc\x20\xf1\x20\x34\x20\xeb\xfe\xe4\xfc\xec\xe8\x20\xe8\x2c\x20\xee\xef\xf6\xe8\xee\xed\xe0\xeb\xfc\xed\xee\x2c\x20\xe4\xee"
        "\x20\x32\x20\xe4\xee\xef\xee\xeb\xed\xe8\xf2\xe5\xeb\xfc\xed\xfb\xf5\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xed\xfb\xf5\x20\xe8\xe3\xf0\xee\xea\xee\xe2\x2e"),
    /* {5 игроков}\n\n Играть с 5 людьми и, опционально, с 1 компьютерным игроком. */ DATA_COMPGEN(0x0050cc74, gSetupHotSeatGameHelp5PlayersPlayWith5HumanPlayersAnd3,
        "\x7b\x35\x20\xe8\xe3\xf0\xee\xea\xee\xe2\x7d\x0a\x0a\x20\xc8\xe3\xf0\xe0\xf2\xfc\x20\xf1\x20\x35\x20\xeb\xfe\xe4\xfc\xec\xe8\x20\xe8\x2c\x20\xee\xef\xf6\xe8\xee\xed\xe0\xeb\xfc\xed\xee\x2c\x20"
        "\xf1\x20\x31\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xed\xfb\xec\x20\xe8\xe3\xf0\xee\xea\xee\xec\x2e"),
    /* {6 игроков}\n\n Играть с 6 людьми. */ DATA_COMPGEN(0x0050ccc0, gSetupHotSeatGameHelp6PlayersPlayWith6HumanPlayers4, "\x7b\x36\x20\xe8\xe3\xf0\xee\xea\xee\xe2\x7d\x0a\x0a\x20\xc8\xe3\xf0\xe0\xf2\xfc\x20\xf1\x20\x36\x20\xeb\xfe\xe4\xfc\xec\xe8\x2e"),
    /* {Отменить}\n\nОтменить и вернуться в главное меню. */ DATA_COMPGEN(0x0050cce4, gSetupHotSeatGameHelpCancelCancelBackToTheMainMenu5, "\x7b\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x7d\x0a\x0a\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x20\xe8\x20\xe2\xe5\xf0\xed\xf3\xf2\xfc\xf1\xff\x20\xe2\x20\xe3\xeb\xe0\xe2\xed\xee\xe5\x20\xec\xe5\xed\xfe\x2e")
};
DATA(0x004fe968) char* gSetupModemGameHelp[KB_SETUP_MODEM_HELP_COUNT] = {
    /* {Сервер}\n\nСервер задает настройки игры. Может быть, только один хост в одном сетевом соединении. */ DATA_COMPGEN(0x0050cd18, gSetupModemGameHelpHostTheHostSetsUpTheGameOptions0,
        "\x7b\xd1\xe5\xf0\xe2\xe5\xf0\x7d\x0a\x0a\xd1\xe5\xf0\xe2\xe5\xf0\x20\xe7\xe0\xe4\xe0\xe5\xf2\x20\xed\xe0\xf1\xf2\xf0\xee\xe9\xea\xe8\x20\xe8\xe3\xf0\xfb\x2e\x20\xcc\xee\xe6\xe5\xf2\x20\xe1\xfb"
        "\xf2\xfc\x2c\x20\xf2\xee\xeb\xfc\xea\xee\x20\xee\xe4\xe8\xed\x20\xf5\xee\xf1\xf2\x20\xe2\x20\xee\xe4\xed\xee\xec\x20\xf1\xe5\xf2\xe5\xe2\xee\xec\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe8\x2e"),
    /* {Гость}\n\nГость ожидает, пока сервер задаст настройки игры, после чего он автоматически вступит в игру. */ DATA_COMPGEN(0x0050cd7c, gSetupModemGameHelpGuestTheGuestWaitsForTheHostTo1,
        "\x7b\xc3\xee\xf1\xf2\xfc\x7d\x0a\x0a\xc3\xee\xf1\xf2\xfc\x20\xee\xe6\xe8\xe4\xe0\xe5\xf2\x2c\x20\xef\xee\xea\xe0\x20\xf1\xe5\xf0\xe2\xe5\xf0\x20\xe7\xe0\xe4\xe0\xf1\xf2\x20\xed\xe0\xf1\xf2\xf0"
        "\xee\xe9\xea\xe8\x20\xe8\xe3\xf0\xfb\x2c\x20\xef\xee\xf1\xeb\xe5\x20\xf7\xe5\xe3\xee\x20\xee\xed\x20\xe0\xe2\xf2\xee\xec\xe0\xf2\xe8\xf7\xe5\xf1\xea\xe8\x20\xe2\xf1\xf2\xf3\xef\xe8\xf2\x20\xe2"
        "\x20\xe8\xe3\xf0\xf3\x2e"),
    /* {Настройки}\n\nИзменить конфигурацию модема. */ DATA_COMPGEN(0x0050cde4, gSetupModemGameHelpConfigChangeYourModemConfiguration2, "\x7b\xcd\xe0\xf1\xf2\xf0\xee\xe9\xea\xe8\x7d\x0a\x0a\xc8\xe7\xec\xe5\xed\xe8\xf2\xfc\x20\xea\xee\xed\xf4\xe8\xe3\xf3\xf0\xe0\xf6\xe8\xfe\x20\xec\xee\xe4\xe5\xec\xe0\x2e"),
    /* {Отменить}\n\nОтменить и вернуться в главное меню. */ DATA_COMPGEN(0x0050ce10, gSetupModemGameHelpCancelCancelBackToTheMainMenu3, "\x7b\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x7d\x0a\x0a\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x20\xe8\x20\xe2\xe5\xf0\xed\xf3\xf2\xfc\xf1\xff\x20\xe2\x20\xe3\xeb\xe0\xe2\xed\xee\xe5\x20\xec\xe5\xed\xfe\x2e")
};
DATA(0x004fe978) char* gSetupDCGameHelp[KB_SETUP_DIRECT_CONNECT_HELP_COUNT] = {
    /* {Сервер}\n\nСервер задает настройки игры. */ DATA_COMPGEN(0x0050ce44, gSetupDCGameHelpHostTheHostSetsUpTheGameOptions0, "\x7b\xd1\xe5\xf0\xe2\xe5\xf0\x7d\x0a\x0a\xd1\xe5\xf0\xe2\xe5\xf0\x20\xe7\xe0\xe4\xe0\xe5\xf2\x20\xed\xe0\xf1\xf2\xf0\xee\xe9\xea\xe8\x20\xe8\xe3\xf0\xfb\x2e"),
    /* {Гость}\n\nГость ожидает, пока сервер задаст настройки игры. */ DATA_COMPGEN(0x0050ce6c, gSetupDCGameHelpGuestTheGuestWaitsForTheHostTo1,
        "\x7b\xc3\xee\xf1\xf2\xfc\x7d\x0a\x0a\xc3\xee\xf1\xf2\xfc\x20\xee\xe6\xe8\xe4\xe0\xe5\xf2\x2c\x20\xef\xee\xea\xe0\x20\xf1\xe5\xf0\xe2\xe5\xf0\x20\xe7\xe0\xe4\xe0\xf1\xf2\x20\xed\xe0\xf1\xf2\xf0"
        "\xee\xe9\xea\xe8\x20\xe8\xe3\xf0\xfb\x2e"),
    /* {Настройки}\n\nИзменить конфигурацию модема. */ DATA_COMPGEN(0x0050cea8, gSetupDCGameHelpConfigChangeYourDirectConnectPortConfiguration2, "\x7b\xcd\xe0\xf1\xf2\xf0\xee\xe9\xea\xe8\x7d\x0a\x0a\xc8\xe7\xec\xe5\xed\xe8\xf2\xfc\x20\xea\xee\xed\xf4\xe8\xe3\xf3\xf0\xe0\xf6\xe8\xfe\x20\xec\xee\xe4\xe5\xec\xe0\x2e"),
    /* {Отменить}\n\nОтменить и вернуться в главное меню. */ DATA_COMPGEN(0x0050ced4, gSetupDCGameHelpCancelCancelBackToTheMainMenu3, "\x7b\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x7d\x0a\x0a\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x20\xe8\x20\xe2\xe5\xf0\xed\xf3\xf2\xfc\xf1\xff\x20\xe2\x20\xe3\xeb\xe0\xe2\xed\xee\xe5\x20\xec\xe5\xed\xfe\x2e")
};
DATA(0x004fe988) char* gSetupMultiPlayerGameHelp[KB_SETUP_MULTIPLAYER_HELP_COUNT] = {
    /* {За одной машиной}\n\nИграть за одной машиной, где от 2 до 4 игроков людей. */ DATA_COMPGEN(0x0050cf08, gSetupMultiPlayerGameHelpHotSeatPlayAHotSeatGameWhere0,
        "\x7b\xc7\xe0\x20\xee\xe4\xed\xee\xe9\x20\xec\xe0\xf8\xe8\xed\xee\xe9\x7d\x0a\x0a\xc8\xe3\xf0\xe0\xf2\xfc\x20\xe7\xe0\x20\xee\xe4\xed\xee\xe9\x20\xec\xe0\xf8\xe8\xed\xee\xe9\x2c\x20\xe3\xe4\xe5"
        "\x20\xee\xf2\x20\x32\x20\xe4\xee\x20\x34\x20\xe8\xe3\xf0\xee\xea\xee\xe2\x20\xeb\xfe\xe4\xe5\xe9\x2e"),
    /* {Локальная сеть}\n\nИграть по сети, где двое игроков играют по локальной сети, сидя за своими компьютерами. */ DATA_COMPGEN(0x0050cf54, gSetupMultiPlayerGameHelpNetworkPlayANetworkGameWhere2Players1,
        "\x7b\xcb\xee\xea\xe0\xeb\xfc\xed\xe0\xff\x20\xf1\xe5\xf2\xfc\x7d\x0a\x0a\xc8\xe3\xf0\xe0\xf2\xfc\x20\xef\xee\x20\xf1\xe5\xf2\xe8\x2c\x20\xe3\xe4\xe5\x20\xe4\xe2\xee\xe5\x20\xe8\xe3\xf0\xee\xea"
        "\xee\xe2\x20\xe8\xe3\xf0\xe0\xfe\xf2\x20\xef\xee\x20\xeb\xee\xea\xe0\xeb\xfc\xed\xee\xe9\x20\xf1\xe5\xf2\xe8\x2c\x20\xf1\xe8\xe4\xff\x20\xe7\xe0\x20\xf1\xe2\xee\xe8\xec\xe8\x20\xea\xee\xec\xef"
        "\xfc\xfe\xf2\xe5\xf0\xe0\xec\xe8\x2e"),
    /* {Модем}\n\nДвое игроков играют через модемы сидя за своими компьютерами. */ DATA_COMPGEN(0x0050cfc0, gSetupMultiPlayerGameHelpModemPlayAModemGameWhere2Players2,
        "\x7b\xcc\xee\xe4\xe5\xec\x7d\x0a\x0a\xc4\xe2\xee\xe5\x20\xe8\xe3\xf0\xee\xea\xee\xe2\x20\xe8\xe3\xf0\xe0\xfe\xf2\x20\xf7\xe5\xf0\xe5\xe7\x20\xec\xee\xe4\xe5\xec\xfb\x20\xf1\xe8\xe4\xff\x20\xe7"
        "\xe0\x20\xf1\xe2\xee\xe8\xec\xe8\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xe0\xec\xe8\x2e"),
    /* {Прямое соединение}\n\nДвое игроков играют через ноль-модем сидя за своими компьютерами. */ DATA_COMPGEN(0x0050d008, gSetupMultiPlayerGameHelpDirectConnectPlayADirectConnectGameWhere3,
        "\x7b\xcf\xf0\xff\xec\xee\xe5\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5\x7d\x0a\x0a\xc4\xe2\xee\xe5\x20\xe8\xe3\xf0\xee\xea\xee\xe2\x20\xe8\xe3\xf0\xe0\xfe\xf2\x20\xf7\xe5\xf0\xe5\xe7\x20\xed"
        "\xee\xeb\xfc\x2d\xec\xee\xe4\xe5\xec\x20\xf1\xe8\xe4\xff\x20\xe7\xe0\x20\xf1\xe2\xee\xe8\xec\xe8\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xe0\xec\xe8\x2e"),
    /* {Отменить}\n\nОтменить и вернуться в главное меню. */ DATA_COMPGEN(0x0050d060, gSetupMultiPlayerGameHelpCancelCancelBackToTheMainMenu4, "\x7b\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x7d\x0a\x0a\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x20\xe8\x20\xe2\xe5\xf0\xed\xf3\xf2\xfc\xf1\xff\x20\xe2\x20\xe3\xeb\xe0\xe2\xed\xee\xe5\x20\xec\xe5\xed\xfe\x2e")
};
DATA(0x004fe99c) char* gSetupNetworkGameHelp[KB_SETUP_NETWORK_HELP_COUNT] = {
    /* {Сервер}\n\nОпределяет настройки игры. Может быть только один сервер в одном соединении. */ DATA_COMPGEN(0x0050d094, gSetupNetworkGameHelpHostTheHostSetsUpTheGameOptions0,
        "\x7b\xd1\xe5\xf0\xe2\xe5\xf0\x7d\x0a\x0a\xce\xef\xf0\xe5\xe4\xe5\xeb\xff\xe5\xf2\x20\xed\xe0\xf1\xf2\xf0\xee\xe9\xea\xe8\x20\xe8\xe3\xf0\xfb\x2e\x20\xcc\xee\xe6\xe5\xf2\x20\xe1\xfb\xf2\xfc\x20"
        "\xf2\xee\xeb\xfc\xea\xee\x20\xee\xe4\xe8\xed\x20\xf1\xe5\xf0\xe2\xe5\xf0\x20\xe2\x20\xee\xe4\xed\xee\xec\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe8\x2e"),
    /* {Гость}\n\n Гость ожидает, пока сервер задаст настройки игры, после чего он автоматически вступит в игру. В игре через TCP/IP и IPX может быть несколько гостей. В игре через NetBIOS - только 1. */ DATA_COMPGEN(0x0050d0ec, gSetupNetworkGameHelpGuestTheGuestWaitsForTheHostTo1,
        "\x7b\xc3\xee\xf1\xf2\xfc\x7d\x0a\x0a\x20\xc3\xee\xf1\xf2\xfc\x20\xee\xe6\xe8\xe4\xe0\xe5\xf2\x2c\x20\xef\xee\xea\xe0\x20\xf1\xe5\xf0\xe2\xe5\xf0\x20\xe7\xe0\xe4\xe0\xf1\xf2\x20\xed\xe0\xf1\xf2"
        "\xf0\xee\xe9\xea\xe8\x20\xe8\xe3\xf0\xfb\x2c\x20\xef\xee\xf1\xeb\xe5\x20\xf7\xe5\xe3\xee\x20\xee\xed\x20\xe0\xe2\xf2\xee\xec\xe0\xf2\xe8\xf7\xe5\xf1\xea\xe8\x20\xe2\xf1\xf2\xf3\xef\xe8\xf2\x20"
        "\xe2\x20\xe8\xe3\xf0\xf3\x2e\x20\xc2\x20\xe8\xe3\xf0\xe5\x20\xf7\xe5\xf0\xe5\xe7\x20\x54\x43\x50\x2f\x49\x50\x20\xe8\x20\x49\x50\x58\x20\xec\xee\xe6\xe5\xf2\x20\xe1\xfb\xf2\xfc\x20\xed\xe5\xf1"
        "\xea\xee\xeb\xfc\xea\xee\x20\xe3\xee\xf1\xf2\xe5\xe9\x2e\x20\xc2\x20\xe8\xe3\xf0\xe5\x20\xf7\xe5\xf0\xe5\xe7\x20\x4e\x65\x74\x42\x49\x4f\x53\x20\x2d\x20\xf2\xee\xeb\xfc\xea\xee\x20\x31\x2e"),
    /* {Отменить}\n\nОтменить и вернуться в главное меню. */ DATA_COMPGEN(0x0050d1ac, gSetupNetworkGameHelpCancelCancelBackToTheMainMenu2, "\x7b\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x7d\x0a\x0a\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x20\xe8\x20\xe2\xe5\xf0\xed\xf3\xf2\xfc\xf1\xff\x20\xe2\x20\xe3\xeb\xe0\xe2\xed\xee\xe5\x20\xec\xe5\xed\xfe\x2e")
};
DATA(0x004fe9a8) char* gSetupNetworkGame2Help[KB_SETUP_NETWORK_SECOND_HELP_COUNT] = {
    /* {IPX}\n\nIPX является часто используемым сетевым протоколом для Windows. По IPX могут играть до 6 человек одновременно. Протокол IPX поддерживает только версия игры, работающая под Windows 95. */ DATA_COMPGEN(0x0050d1e0, gSetupNetworkGame2HelpIPXIPXNetworkingIsTheMostCommonlyUsed0,
        "\x7b\x49\x50\x58\x7d\x0a\x0a\x49\x50\x58\x20\xff\xe2\xeb\xff\xe5\xf2\xf1\xff\x20\xf7\xe0\xf1\xf2\xee\x20\xe8\xf1\xef\xee\xeb\xfc\xe7\xf3\xe5\xec\xfb\xec\x20\xf1\xe5\xf2\xe5\xe2\xfb\xec\x20\xef"
        "\xf0\xee\xf2\xee\xea\xee\xeb\xee\xec\x20\xe4\xeb\xff\x20\x57\x69\x6e\x64\x6f\x77\x73\x2e\x20\xcf\xee\x20\x49\x50\x58\x20\xec\xee\xe3\xf3\xf2\x20\xe8\xe3\xf0\xe0\xf2\xfc\x20\xe4\xee\x20\x36\x20"
        "\xf7\xe5\xeb\xee\xe2\xe5\xea\x20\xee\xe4\xed\xee\xe2\xf0\xe5\xec\xe5\xed\xed\xee\x2e\x20\xcf\xf0\xee\xf2\xee\xea\xee\xeb\x20\x49\x50\x58\x20\xef\xee\xe4\xe4\xe5\xf0\xe6\xe8\xe2\xe0\xe5\xf2\x20"
        "\xf2\xee\xeb\xfc\xea\xee\x20\xe2\xe5\xf0\xf1\xe8\xff\x20\xe8\xe3\xf0\xfb\x2c\x20\xf0\xe0\xe1\xee\xf2\xe0\xfe\xf9\xe0\xff\x20\xef\xee\xe4\x20\x57\x69\x6e\x64\x6f\x77\x73\x20\x39\x35\x2e"),
    /* {TCP/IP}\n\nПротокол TCP/IP наиболее часто используется для соединения компьютеров через Интернет. По TCP/IP могут играть до 6 человек одновременно. Протокол TCP/IP поддерживает только версия игры, работающая под Windows. */ DATA_COMPGEN(0x0050d2a0, gSetupNetworkGame2HelpTCPIPTCPIPIsMostCommonlyUsed1,
        "\x7b\x54\x43\x50\x2f\x49\x50\x7d\x0a\x0a\xcf\xf0\xee\xf2\xee\xea\xee\xeb\x20\x54\x43\x50\x2f\x49\x50\x20\xed\xe0\xe8\xe1\xee\xeb\xe5\xe5\x20\xf7\xe0\xf1\xf2\xee\x20\xe8\xf1\xef\xee\xeb\xfc\xe7"
        "\xf3\xe5\xf2\xf1\xff\x20\xe4\xeb\xff\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xff\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xee\xe2\x20\xf7\xe5\xf0\xe5\xe7\x20\xc8\xed\xf2\xe5\xf0\xed\xe5\xf2\x2e"
        "\x20\xcf\xee\x20\x54\x43\x50\x2f\x49\x50\x20\xec\xee\xe3\xf3\xf2\x20\xe8\xe3\xf0\xe0\xf2\xfc\x20\xe4\xee\x20\x36\x20\xf7\xe5\xeb\xee\xe2\xe5\xea\x20\xee\xe4\xed\xee\xe2\xf0\xe5\xec\xe5\xed\xed"
        "\xee\x2e\x20\xcf\xf0\xee\xf2\xee\xea\xee\xeb\x20\x54\x43\x50\x2f\x49\x50\x20\xef\xee\xe4\xe4\xe5\xf0\xe6\xe8\xe2\xe0\xe5\xf2\x20\xf2\xee\xeb\xfc\xea\xee\x20\xe2\xe5\xf0\xf1\xe8\xff\x20\xe8\xe3"
        "\xf0\xfb\x2c\x20\xf0\xe0\xe1\xee\xf2\xe0\xfe\xf9\xe0\xff\x20\xef\xee\xe4\x20\x57\x69\x6e\x64\x6f\x77\x73\x2e"),
    /* {NETBios}\n\nПротокол NETBios является единственно возможным для компьютеров, работающим под DOS, но может быть использован и с Windows 95.  Этот протокол обеспечивает соединение не более двух игроков. Мы рекомендуем использовать протокол IPX. */ DATA_COMPGEN(0x0050d37c, gSetupNetworkGame2HelpNETBiosNETBiosNetworkingIsTheOnlyFormOf2,
        "\x7b\x4e\x45\x54\x42\x69\x6f\x73\x7d\x0a\x0a\xcf\xf0\xee\xf2\xee\xea\xee\xeb\x20\x4e\x45\x54\x42\x69\x6f\x73\x20\xff\xe2\xeb\xff\xe5\xf2\xf1\xff\x20\xe5\xe4\xe8\xed\xf1\xf2\xe2\xe5\xed\xed\xee"
        "\x20\xe2\xee\xe7\xec\xee\xe6\xed\xfb\xec\x20\xe4\xeb\xff\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xee\xe2\x2c\x20\xf0\xe0\xe1\xee\xf2\xe0\xfe\xf9\xe8\xec\x20\xef\xee\xe4\x20\x44\x4f\x53\x2c\x20"
        "\xed\xee\x20\xec\xee\xe6\xe5\xf2\x20\xe1\xfb\xf2\xfc\x20\xe8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xed\x20\xe8\x20\xf1\x20\x57\x69\x6e\x64\x6f\x77\x73\x20\x39\x35\x2e\x20\x20\xdd\xf2\xee\xf2\x20"
        "\xef\xf0\xee\xf2\xee\xea\xee\xeb\x20\xee\xe1\xe5\xf1\xef\xe5\xf7\xe8\xe2\xe0\xe5\xf2\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5\x20\xed\xe5\x20\xe1\xee\xeb\xe5\xe5\x20\xe4\xe2\xf3\xf5\x20\xe8"
        "\xe3\xf0\xee\xea\xee\xe2\x2e\x20\xcc\xfb\x20\xf0\xe5\xea\xee\xec\xe5\xed\xe4\xf3\xe5\xec\x20\xe8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xf2\xfc\x20\xef\xf0\xee\xf2\xee\xea\xee\xeb\x20\x49\x50\x58"
        "\x2e"),
    /* {Отмена}\n\nЗакрыть меню. */ DATA_COMPGEN(0x0050d470, gSetupNetworkGame2HelpCancelCancelBackToTheMainMenu3, "\x7b\xce\xf2\xec\xe5\xed\xe0\x7d\x0a\x0a\xc7\xe0\xea\xf0\xfb\xf2\xfc\x20\xec\xe5\xed\xfe\x2e")
};
DATA(0x004fe9b8) char* gSetupGameHelp[KB_SETUP_GAME_HELP_COUNT] = {
    /* {Обычная игра}\n\nОдиночная игра на отдельной карте. */ DATA_COMPGEN(0x0050d488, gSetupGameHelpStandardGameASinglePlayerGamePlayingOut0,
        "\x7b\xce\xe1\xfb\xf7\xed\xe0\xff\x20\xe8\xe3\xf0\xe0\x7d\x0a\x0a\xce\xe4\xe8\xed\xee\xf7\xed\xe0\xff\x20\xe8\xe3\xf0\xe0\x20\xed\xe0\x20\xee\xf2\xe4\xe5\xeb\xfc\xed\xee\xe9\x20\xea\xe0\xf0\xf2"
        "\xe5\x2e"),
    /* {Кампания}\n\nОдиночная игра на серии карт. */ DATA_COMPGEN(0x0050d4bc, gSetupGameHelpCampaignGameASinglePlayerGamePlayingThrough1, "\x7b\xca\xe0\xec\xef\xe0\xed\xe8\xff\x7d\x0a\x0a\xce\xe4\xe8\xed\xee\xf7\xed\xe0\xff\x20\xe8\xe3\xf0\xe0\x20\xed\xe0\x20\xf1\xe5\xf0\xe8\xe8\x20\xea\xe0\xf0\xf2\x2e"),
    /* {Сетевая игра}\n\nСетевая игра, где несколько игроков-людей сражаются друг против друга на одной карте. */ DATA_COMPGEN(0x0050d4e8, gSetupGameHelpMultiPlayerGameAMultiPlayerGameWith2,
        "\x7b\xd1\xe5\xf2\xe5\xe2\xe0\xff\x20\xe8\xe3\xf0\xe0\x7d\x0a\x0a\xd1\xe5\xf2\xe5\xe2\xe0\xff\x20\xe8\xe3\xf0\xe0\x2c\x20\xe3\xe4\xe5\x20\xed\xe5\xf1\xea\xee\xeb\xfc\xea\xee\x20\xe8\xe3\xf0\xee"
        "\xea\xee\xe2\x2d\xeb\xfe\xe4\xe5\xe9\x20\xf1\xf0\xe0\xe6\xe0\xfe\xf2\xf1\xff\x20\xe4\xf0\xf3\xe3\x20\xef\xf0\xee\xf2\xe8\xe2\x20\xe4\xf0\xf3\xe3\xe0\x20\xed\xe0\x20\xee\xe4\xed\xee\xe9\x20\xea"
        "\xe0\xf0\xf2\xe5\x2e"),
    /* {Отменить}\n\nОтменить и вернуться в главное меню. */ DATA_COMPGEN(0x0050d550, gSetupGameHelpCancelCancelBackToTheMainMenu3, "\x7b\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x7d\x0a\x0a\xce\xf2\xec\xe5\xed\xe8\xf2\xfc\x20\xe8\x20\xe2\xe5\xf0\xed\xf3\xf2\xfc\xf1\xff\x20\xe2\x20\xe3\xeb\xe0\xe2\xed\xee\xe5\x20\xec\xe5\xed\xfe\x2e")
};
DATA(0x004fe9c8) char* cBattleResults[KB_BATTLE_RESULT_TEXT_COUNT] = {
    /* Враг сдался! */ DATA_COMPGEN(0x0050d584, cBattleResultsTheEnemyHasSurrendered0, "\xc2\xf0\xe0\xe3\x20\xf1\xe4\xe0\xeb\xf1\xff\x21"),
    /* Враг повержен! */ DATA_COMPGEN(0x0050d594, cBattleResultsTheEnemyHasFled1, "\xc2\xf0\xe0\xe3\x20\xef\xee\xe2\xe5\xf0\xe6\xe5\xed\x21"),
    /* Великая победа! */ DATA_COMPGEN(0x0050d5a4, cBattleResultsAGloriousVictory2, "\xc2\xe5\xeb\xe8\xea\xe0\xff\x20\xef\xee\xe1\xe5\xe4\xe0\x21"),
    /* \n\nЗа мужество, проявленное в бою, %s получает %d оч. опыта. */ DATA_COMPGEN(0x0050d5b4, cBattleResultsForValorInCombatFormatReceivesFormatExperience3,
        "\x0a\x0a\xc7\xe0\x20\xec\xf3\xe6\xe5\xf1\xf2\xe2\xee\x2c\x20\xef\xf0\xee\xff\xe2\xeb\xe5\xed\xed\xee\xe5\x20\xe2\x20\xe1\xee\xfe\x2c\x20\x25\x73\x20\xef\xee\xeb\xf3\xf7\xe0\xe5\xf2\x20\x25\x64"
        "\x20\xee\xf7\x2e\x20\xee\xef\xfb\xf2\xe0\x2e"),
    /* %s сдается врагу и отступает с позором. */ DATA_COMPGEN(0x0050d5f0, cBattleResultsFormatSurrendersToTheEnemyAndDepartsIn4, "\x25\x73\x20\xf1\xe4\xe0\xe5\xf2\xf1\xff\x20\xe2\xf0\xe0\xe3\xf3\x20\xe8\x20\xee\xf2\xf1\xf2\xf3\xef\xe0\xe5\xf2\x20\xf1\x20\xef\xee\xe7\xee\xf0\xee\xec\x2e"),
    /* %s трусливо бежит с поля боя. */ DATA_COMPGEN(0x0050d618, cBattleResultsTheCowardlyFormatFleesFromBattle5, "\x25\x73\x20\xf2\xf0\xf3\xf1\xeb\xe8\xe2\xee\x20\xe1\xe5\xe6\xe8\xf2\x20\xf1\x20\xef\xee\xeb\xff\x20\xe1\xee\xff\x2e"),
    /* Ваши войска потерпели поражение и %s покидает вас. */ DATA_COMPGEN(0x0050d638, cBattleResultsYourForcesSufferABitterDefeatAndFormat6,
        "\xc2\xe0\xf8\xe8\x20\xe2\xee\xe9\xf1\xea\xe0\x20\xef\xee\xf2\xe5\xf0\xef\xe5\xeb\xe8\x20\xef\xee\xf0\xe0\xe6\xe5\xed\xe8\xe5\x20\xe8\x20\x25\x73\x20\xef\xee\xea\xe8\xe4\xe0\xe5\xf2\x20\xe2\xe0"
        "\xf1\x2e"),
    /* Ваши силы сдались врагу и отступили с позором. */ DATA_COMPGEN(0x0050d66c, cBattleResultsYourForcesSurrenderToTheEnemyAndDepart7, "\xc2\xe0\xf8\xe8\x20\xf1\xe8\xeb\xfb\x20\xf1\xe4\xe0\xeb\xe8\xf1\xfc\x20\xe2\xf0\xe0\xe3\xf3\x20\xe8\x20\xee\xf2\xf1\xf2\xf3\xef\xe8\xeb\xe8\x20\xf1\x20\xef\xee\xe7\xee\xf0\xee\xec\x2e"),
    /* Ваши трусливые войска бежали с поля боя. */ DATA_COMPGEN(0x0050d69c, cBattleResultsYourCowardlyForcesFleeFromBattle8, "\xc2\xe0\xf8\xe8\x20\xf2\xf0\xf3\xf1\xeb\xe8\xe2\xfb\xe5\x20\xe2\xee\xe9\xf1\xea\xe0\x20\xe1\xe5\xe6\xe0\xeb\xe8\x20\xf1\x20\xef\xee\xeb\xff\x20\xe1\xee\xff\x2e"),
    /* Ваши войска потерпели поражение. */ DATA_COMPGEN(0x0050d6c8, cBattleResultsYourForcesSufferABitterDefeat9, "\xc2\xe0\xf8\xe8\x20\xe2\xee\xe9\xf1\xea\xe0\x20\xef\xee\xf2\xe5\xf0\xef\xe5\xeb\xe8\x20\xef\xee\xf0\xe0\xe6\xe5\xed\xe8\xe5\x2e"),
    /* \n\nЗа мужество, проявленное в бою, %s получает %d оч. опыта, и получает %d уровень(я). */ DATA_COMPGEN(0x0050d6ec, cBattleResultsForValorInCombatFormatReceivesFormatExperience10,
        "\x0a\x0a\xc7\xe0\x20\xec\xf3\xe6\xe5\xf1\xf2\xe2\xee\x2c\x20\xef\xf0\xee\xff\xe2\xeb\xe5\xed\xed\xee\xe5\x20\xe2\x20\xe1\xee\xfe\x2c\x20\x25\x73\x20\xef\xee\xeb\xf3\xf7\xe0\xe5\xf2\x20\x25\x64"
        "\x20\xee\xf7\x2e\x20\xee\xef\xfb\xf2\xe0\x2c\x20\xe8\x20\xef\xee\xeb\xf3\xf7\xe0\xe5\xf2\x20\x25\x64\x20\xf3\xf0\xee\xe2\xe5\xed\xfc\x28\xff\x29\x2e")
};
DATA(0x004fe9f4) char* cMoraleInfo[KB_MORALE_INFO_TEXT_COUNT] = {
    /* {Высокая мораль}\n\nВысокая мораль может дать в бою вашим бойцам дополнительную атаку. */ DATA_COMPGEN(0x0050d744, cMoraleInfoGoodMoraleGoodMoraleMayGiveYourArmies0,
        "\x7b\xc2\xfb\xf1\xee\xea\xe0\xff\x20\xec\xee\xf0\xe0\xeb\xfc\x7d\x0a\x0a\xc2\xfb\xf1\xee\xea\xe0\xff\x20\xec\xee\xf0\xe0\xeb\xfc\x20\xec\xee\xe6\xe5\xf2\x20\xe4\xe0\xf2\xfc\x20\xe2\x20\xe1\xee"
        "\xfe\x20\xe2\xe0\xf8\xe8\xec\x20\xe1\xee\xe9\xf6\xe0\xec\x20\xe4\xee\xef\xee\xeb\xed\xe8\xf2\xe5\xeb\xfc\xed\xf3\xfe\x20\xe0\xf2\xe0\xea\xf3\x2e"),
    /* {Обычная мораль}\n\nС обычной моралью ваши армии никогда не получат дополнительную атаку и не будут прокляты. */ DATA_COMPGEN(0x0050d79c, cMoraleInfoNeutralMoraleNeutralMoraleMeansYourArmiesWill1,
        "\x7b\xce\xe1\xfb\xf7\xed\xe0\xff\x20\xec\xee\xf0\xe0\xeb\xfc\x7d\x0a\x0a\xd1\x20\xee\xe1\xfb\xf7\xed\xee\xe9\x20\xec\xee\xf0\xe0\xeb\xfc\xfe\x20\xe2\xe0\xf8\xe8\x20\xe0\xf0\xec\xe8\xe8\x20\xed"
        "\xe8\xea\xee\xe3\xe4\xe0\x20\xed\xe5\x20\xef\xee\xeb\xf3\xf7\xe0\xf2\x20\xe4\xee\xef\xee\xeb\xed\xe8\xf2\xe5\xeb\xfc\xed\xf3\xfe\x20\xe0\xf2\xe0\xea\xf3\x20\xe8\x20\xed\xe5\x20\xe1\xf3\xe4\xf3"
        "\xf2\x20\xef\xf0\xee\xea\xeb\xff\xf2\xfb\x2e"),
    /* {Плохая мораль}\n\nПлохая мораль может привести к потере хода в бою вашими бойцами. */ DATA_COMPGEN(0x0050d808, cMoraleInfoBadMoraleBadMoraleMayCauseYourArmies2,
        "\x7b\xcf\xeb\xee\xf5\xe0\xff\x20\xec\xee\xf0\xe0\xeb\xfc\x7d\x0a\x0a\xcf\xeb\xee\xf5\xe0\xff\x20\xec\xee\xf0\xe0\xeb\xfc\x20\xec\xee\xe6\xe5\xf2\x20\xef\xf0\xe8\xe2\xe5\xf1\xf2\xe8\x20\xea\x20"
        "\xef\xee\xf2\xe5\xf0\xe5\x20\xf5\xee\xe4\xe0\x20\xe2\x20\xe1\xee\xfe\x20\xe2\xe0\xf8\xe8\xec\xe8\x20\xe1\xee\xe9\xf6\xe0\xec\xe8\x2e"),
    /* %s\n\n\nМодификаторы морали: */ DATA_COMPGEN(0x0050d85c, cMoraleInfoFormatCurrentMoraleModifiers3, "\x25\x73\x0a\x0a\x0a\xcc\xee\xe4\xe8\xf4\xe8\xea\xe0\xf2\xee\xf0\xfb\x20\xec\xee\xf0\xe0\xeb\xe8\x3a"),
    /* \nБонус рыцаря +1 */ DATA_COMPGEN(0x0050d878, cMoraleInfoKnightBonus14, "\x0a\xc1\xee\xed\xf3\xf1\x20\xf0\xfb\xf6\xe0\xf0\xff\x20\x2b\x31"),
    /* \n%s со своей армией +1 */ DATA_COMPGEN(0x0050d88c, cMoraleInfoAllFormatTroops15, "\x0a\x25\x73\x20\xf1\xee\x20\xf1\xe2\xee\xe5\xe9\x20\xe0\xf0\xec\xe8\xe5\xe9\x20\x2b\x31"),
    /* \nВоины 3 рас -1 */ DATA_COMPGEN(0x0050d8a4, cMoraleInfoTroopsOf3Alignments16, "\x0a\xc2\xee\xe8\xed\xfb\x20\x33\x20\xf0\xe0\xf1\x20\x2d\x31"),
    /* \nВоины 4 рас -2 */ DATA_COMPGEN(0x0050d8b4, cMoraleInfoTroopsOf4Alignments27, "\x0a\xc2\xee\xe8\xed\xfb\x20\x34\x20\xf0\xe0\xf1\x20\x2d\x32"),
    /* \nМедаль отваги +1 */ DATA_COMPGEN(0x0050d8c4, cMoraleInfoMedalOfValor18, "\x0a\xcc\xe5\xe4\xe0\xeb\xfc\x20\xee\xf2\xe2\xe0\xe3\xe8\x20\x2b\x31"),
    /* \nМедаль мужества +1 */ DATA_COMPGEN(0x0050d8d8, cMoraleInfoMedalOfCourage19, "\x0a\xcc\xe5\xe4\xe0\xeb\xfc\x20\xec\xf3\xe6\xe5\xf1\xf2\xe2\xe0\x20\x2b\x31"),
    /* \nМедаль доблести +1 */ DATA_COMPGEN(0x0050d8ec, cMoraleInfoMedalOfHonor110, "\x0a\xcc\xe5\xe4\xe0\xeb\xfc\x20\xe4\xee\xe1\xeb\xe5\xf1\xf2\xe8\x20\x2b\x31"),
    /* \nМедаль почета +1 */ DATA_COMPGEN(0x0050d900, cMoraleInfoMedalOfDistinction111, "\x0a\xcc\xe5\xe4\xe0\xeb\xfc\x20\xef\xee\xf7\xe5\xf2\xe0\x20\x2b\x31"),
    /* \nСимвол неудачи -2 */ DATA_COMPGEN(0x0050d914, cMoraleInfoFizbinOfMisfortune212, "\x0a\xd1\xe8\xec\xe2\xee\xeb\x20\xed\xe5\xf3\xe4\xe0\xf7\xe8\x20\x2d\x32"),
    /* \nПосещен буй +1 */ DATA_COMPGEN(0x0050d928, cMoraleInfoBuoyVisited113, "\x0a\xcf\xee\xf1\xe5\xf9\xe5\xed\x20\xe1\xf3\xe9\x20\x2b\x31"),
    /* \nПосещен оазис +1 */ DATA_COMPGEN(0x0050d938, cMoraleInfoOasisVisited114, "\x0a\xcf\xee\xf1\xe5\xf9\xe5\xed\x20\xee\xe0\xe7\xe8\xf1\x20\x2b\x31"),
    /* \nПосещен храм +2 */ DATA_COMPGEN(0x0050d94c, cMoraleInfoTempleVisited215, "\x0a\xcf\xee\xf1\xe5\xf9\xe5\xed\x20\xf5\xf0\xe0\xec\x20\x2b\x32"),
    /* \nРасхититель гробниц -1 */ DATA_COMPGEN(0x0050d960, cMoraleInfoGraveyardRobber116, "\x0a\xd0\xe0\xf1\xf5\xe8\xf2\xe8\xf2\xe5\xeb\xfc\x20\xe3\xf0\xee\xe1\xed\xe8\xf6\x20\x2d\x31"),
    /* \nРасхититель обломков -1 */ DATA_COMPGEN(0x0050d978, cMoraleInfoShipwreckRobber117, "\x0a\xd0\xe0\xf1\xf5\xe8\xf2\xe8\xf2\xe5\xeb\xfc\x20\xee\xe1\xeb\xee\xec\xea\xee\xe2\x20\x2d\x31"),
    /* \nТрусость в бою %d */ DATA_COMPGEN(0x0050d994, cMoraleInfoBattleCowardiceFormat18, "\x0a\xd2\xf0\xf3\xf1\xee\xf1\xf2\xfc\x20\xe2\x20\xe1\xee\xfe\x20\x25\x64"),
    /* \nНет */ DATA_COMPGEN(0x0050d9a8, cMoraleInfoNone19, "\x0a\xcd\xe5\xf2"),
    /* \nВоины 5 рас -3 */ DATA_COMPGEN(0x0050d9b0, cMoraleInfoTroopsOf5Alignments320, "\x0a\xc2\xee\xe8\xed\xfb\x20\x35\x20\xf0\xe0\xf1\x20\x2d\x33"),
    /* \nВся армия одна нежить, мораль не важна. */ DATA_COMPGEN(0x0050d9c0, cMoraleInfoEntireUnitIsUndeadSoMoraleDoesNot21, "\x0a\xc2\xf1\xff\x20\xe0\xf0\xec\xe8\xff\x20\xee\xe4\xed\xe0\x20\xed\xe5\xe6\xe8\xf2\xfc\x2c\x20\xec\xee\xf0\xe0\xeb\xfc\x20\xed\xe5\x20\xe2\xe0\xe6\xed\xe0\x2e"),
    /* \nВ армии нежить -1 */ DATA_COMPGEN(0x0050d9ec, cMoraleInfoSomeUndeadInGroup122, "\x0a\xc2\x20\xe0\xf0\xec\xe8\xe8\x20\xed\xe5\xe6\xe8\xf2\xfc\x20\x2d\x31"),
    /* \nПосещена промоина +1 */ DATA_COMPGEN(0x0050da00, cMoraleInfoWateringHoleVisited123, "\x0a\xcf\xee\xf1\xe5\xf9\xe5\xed\xe0\x20\xef\xf0\xee\xec\xee\xe8\xed\xe0\x20\x2b\x31"),
    /* \nРасхититель кораблей -1 */ DATA_COMPGEN(0x0050da18, cMoraleInfoDerelictShipRobber124, "\x0a\xd0\xe0\xf1\xf5\xe8\xf2\xe8\xf2\xe5\xeb\xfc\x20\xea\xee\xf0\xe0\xe1\xeb\xe5\xe9\x20\x2d\x31"),
    /* \nКолизей варваров +2 */ DATA_COMPGEN(0x0050da34, cMoraleInfoBarbarianColiseum225, "\x0a\xca\xee\xeb\xe8\xe7\xe5\xe9\x20\xe2\xe0\xf0\xe2\xe0\xf0\xee\xe2\x20\x2b\x32"),
    /* \nТаверна +1 */ DATA_COMPGEN(0x0050da4c, cMoraleInfoTavern126, "\x0a\xd2\xe0\xe2\xe5\xf0\xed\xe0\x20\x2b\x31"),
    /* \nЛидерство 1-й ступени +1 */ DATA_COMPGEN(0x0050da58, cMoraleInfoBasicLeadership127, "\x0a\xcb\xe8\xe4\xe5\xf0\xf1\xf2\xe2\xee\x20\x31\x2d\xe9\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x20\x2b\x31"),
    /* \nЛидерство 2-й ступени +2 */ DATA_COMPGEN(0x0050da74, cMoraleInfoAdvancedLeadership228, "\x0a\xcb\xe8\xe4\xe5\xf0\xf1\xf2\xe2\xee\x20\x32\x2d\xe9\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x20\x2b\x32"),
    /* \nЛидерство 3-й ступени +3 */ DATA_COMPGEN(0x0050da90, cMoraleInfoExpertLeadership329, "\x0a\xcb\xe8\xe4\xe5\xf0\xf1\xf2\xe2\xee\x20\x33\x2d\xe9\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x20\x2b\x33"),
    /* \nБонус мачты на море +1 */ DATA_COMPGEN(0x0050daac, cMoraleInfoMastheadBonusAtSea130, "\x0a\xc1\xee\xed\xf3\xf1\x20\xec\xe0\xf7\xf2\xfb\x20\xed\xe0\x20\xec\xee\xf0\xe5\x20\x2b\x31"),
    /* \nБоевое одеяние Андурана дает максимальную мораль. */ DATA_COMPGEN(0x0050dac4, cMoraleInfoBattleGarbOfAnduranGivesYouMaximumMorale31,
        "\x0a\xc1\xee\xe5\xe2\xee\xe5\x20\xee\xe4\xe5\xff\xed\xe8\xe5\x20\xc0\xed\xe4\xf3\xf0\xe0\xed\xe0\x20\xe4\xe0\xe5\xf2\x20\xec\xe0\xea\xf1\xe8\xec\xe0\xeb\xfc\xed\xf3\xfe\x20\xec\xee\xf0\xe0\xeb"
        "\xfc\x2e")
};
DATA(0x004fea74) char* cMapSize[KB_MAP_SIZE_TEXT_COUNT] = {/* Маленькая */ DATA_COMPGEN(0x0050daf8, cMapSizeSmall0, "\xcc\xe0\xeb\xe5\xed\xfc\xea\xe0\xff"), /* Средняя */ DATA_COMPGEN(0x0050db04, cMapSizeMedium1, "\xd1\xf0\xe5\xe4\xed\xff\xff"), /* Большая */ DATA_COMPGEN(0x0050db0c, cMapSizeLarge2, "\xc1\xee\xeb\xfc\xf8\xe0\xff"), /* Огромная */ DATA_COMPGEN(0x0050db14, cMapSizeHuge3, "\xce\xe3\xf0\xee\xec\xed\xe0\xff")};
DATA(0x004fea84) char* cDifficulty[KB_DIFFICULTY_TEXT_COUNT] =
    {/* Легкая */ DATA_COMPGEN(0x0050db20, cDifficultyEasy0, "\xcb\xe5\xe3\xea\xe0\xff"), /* Обычная */ DATA_COMPGEN(0x0050db28, cDifficultyNormal1, "\xce\xe1\xfb\xf7\xed\xe0\xff"), /* Высокая */ DATA_COMPGEN(0x0050db30, cDifficultyHard2, "\xc2\xfb\xf1\xee\xea\xe0\xff"), /* Эксперт */ DATA_COMPGEN(0x0050db38, cDifficultyExpert3, "\xdd\xea\xf1\xef\xe5\xf0\xf2"), /* Невозможно! */ DATA_COMPGEN(0x0050db40, cDifficultyImpossible4, "\xcd\xe5\xe2\xee\xe7\xec\xee\xe6\xed\xee\x21")};
DATA(0x004fea98) char* cStartDifficulty[KB_START_DIFFICULTY_TEXT_COUNT] = {/* Легкая */ DATA_COMPGEN(0x0050db4c, cStartDifficultyEasy0, "\xcb\xe5\xe3\xea\xe0\xff"), /* Обычная */ DATA_COMPGEN(0x0050db54, cStartDifficultyNormal1, "\xce\xe1\xfb\xf7\xed\xe0\xff"), /* Тяжелая */ DATA_COMPGEN(0x0050db5c, cStartDifficultyHard2, "\xd2\xff\xe6\xe5\xeb\xe0\xff"), /* Эксперт */ DATA_COMPGEN(0x0050db64, cStartDifficultyExpert3, "\xdd\xea\xf1\xef\xe5\xf0\xf2")};
DATA(0x004feaa8) char* cCampaignLeaders[KB_CAMPAIGN_LEADER_TEXT_COUNT] =
    {/* Лорд Айронфист */ DATA_COMPGEN(0x0050db6c, cCampaignLeadersLordIronfist0, "\xcb\xee\xf0\xe4\x20\xc0\xe9\xf0\xee\xed\xf4\xe8\xf1\xf2"), /* Лорд Слэйер */ DATA_COMPGEN(0x0050db7c, cCampaignLeadersLordSlayer1, "\xcb\xee\xf0\xe4\x20\xd1\xeb\xfd\xe9\xe5\xf0"), /* Королева Ламанда */ DATA_COMPGEN(0x0050db88, cCampaignLeadersQueenLamanda2, "\xca\xee\xf0\xee\xeb\xe5\xe2\xe0\x20\xcb\xe0\xec\xe0\xed\xe4\xe0"), /* Лорд Аламар */ DATA_COMPGEN(0x0050db9c, cCampaignLeadersLordAlamar3, "\xcb\xee\xf0\xe4\x20\xc0\xeb\xe0\xec\xe0\xf0")};
DATA(0x004feab8) char* cWinText[KB_WIN_TEXT_COUNT] =
    {/* Дней: */ DATA_COMPGEN(0x0050dba8, cWinTextDaysSpent0, "\xc4\xed\xe5\xe9\x3a"), /* Очки: */ DATA_COMPGEN(0x0050dbb0, cWinTextBaseScore1, "\xce\xf7\xea\xe8\x3a"), /* Сложность: */ DATA_COMPGEN(0x0050dbb8, cWinTextDifficultyRating2, "\xd1\xeb\xee\xe6\xed\xee\xf1\xf2\xfc\x3a"), /* Счет: */ DATA_COMPGEN(0x0050dbc4, cWinTextFinalScore3, "\xd1\xf7\xe5\xf2\x3a"), /* Ранг: */ DATA_COMPGEN(0x0050dbcc, cWinTextRanking4, "\xd0\xe0\xed\xe3\x3a")};
DATA(0x004feacc) char* cHumanDifficulty[KB_HUMAN_DIFFICULTY_TEXT_COUNT] =
    {/* Человек\n */ DATA_COMPGEN(0x0050dbd4, cHumanDifficultyHuman0, "\xd7\xe5\xeb\xee\xe2\xe5\xea\x0a"), /* Человек\nЛегкая игра */ DATA_COMPGEN(0x0050dbe0, cHumanDifficultyHumanEasy1, "\xd7\xe5\xeb\xee\xe2\xe5\xea\x0a\xcb\xe5\xe3\xea\xe0\xff\x20\xe8\xe3\xf0\xe0"), /* Человек\nОбычная игра */ DATA_COMPGEN(0x0050dbf4, cHumanDifficultyHumanNormal2, "\xd7\xe5\xeb\xee\xe2\xe5\xea\x0a\xce\xe1\xfb\xf7\xed\xe0\xff\x20\xe8\xe3\xf0\xe0"), /* Человек\nТяжелая игра */ DATA_COMPGEN(0x0050dc0c, cHumanDifficultyHumanHard3, "\xd7\xe5\xeb\xee\xe2\xe5\xea\x0a\xd2\xff\xe6\xe5\xeb\xe0\xff\x20\xe8\xe3\xf0\xe0"), /* Человек\nЭксперт */ DATA_COMPGEN(0x0050dc24, cHumanDifficultyHumanExpert4, "\xd7\xe5\xeb\xee\xe2\xe5\xea\x0a\xdd\xea\xf1\xef\xe5\xf0\xf2")};
DATA(0x004feae0) char* cHumanInfoDifficulty[KB_HUMAN_INFO_DIFFICULTY_TEXT_COUNT] =
    {/* Чел.- */ DATA_COMPGEN(0x0050dc34, cHumanInfoDifficultyHuman0, "\xd7\xe5\xeb\x2e\x2d"), /* Чел.-Легкая игра */ DATA_COMPGEN(0x0050dc3c, cHumanInfoDifficultyHumanEasy1, "\xd7\xe5\xeb\x2e\x2d\xcb\xe5\xe3\xea\xe0\xff\x20\xe8\xe3\xf0\xe0"), /* Чел.-Обычная игра */ DATA_COMPGEN(0x0050dc50, cHumanInfoDifficultyHumanNormal2, "\xd7\xe5\xeb\x2e\x2d\xce\xe1\xfb\xf7\xed\xe0\xff\x20\xe8\xe3\xf0\xe0"), /* Чел.-Тяжелая игра */ DATA_COMPGEN(0x0050dc64, cHumanInfoDifficultyHumanHard3, "\xd7\xe5\xeb\x2e\x2d\xd2\xff\xe6\xe5\xeb\xe0\xff\x20\xe8\xe3\xf0\xe0"), /* Чел.-Эксперт */ DATA_COMPGEN(0x0050dc78, cHumanInfoDifficultyHumanExpert4, "\xd7\xe5\xeb\x2e\x2d\xdd\xea\xf1\xef\xe5\xf0\xf2")};
DATA(0x004feaf4) char* musicQualityText[KB_MUSIC_QUALITY_TEXT_COUNT] =
    {/* MIDI */ DATA_COMPGEN(0x0050dc88, musicQualityTextMIDI0, "\x4d\x49\x44\x49"), /* CD-стерео без вокала */ DATA_COMPGEN(0x0050dc90, musicQualityTextCDStereoWOOpera1, "\x43\x44\x2d\xf1\xf2\xe5\xf0\xe5\xee\x20\xe1\xe5\xe7\x20\xe2\xee\xea\xe0\xeb\xe0"), /* CD-стерео с вокалом */ DATA_COMPGEN(0x0050dca8, musicQualityTextCDStereoWithOpera2, "\x43\x44\x2d\xf1\xf2\xe5\xf0\xe5\xee\x20\xf1\x20\xe2\xee\xea\xe0\xeb\xee\xec")};
DATA(0x004feb00) char* gSpellDesc[KB_SPELL_TEXT_COUNT] = {
    /* {Огненный шар}\n\nОгромный огненный шар взрывается над выбранным участком поля боя, поражая все находящиеся поблизости отряды. */ DATA_COMPGEN(0x0050dcbc, gSpellDescFireballCausesAGiantFireballToStrikeThe0,
        "\x7b\xce\xe3\xed\xe5\xed\xed\xfb\xe9\x20\xf8\xe0\xf0\x7d\x0a\x0a\xce\xe3\xf0\xee\xec\xed\xfb\xe9\x20\xee\xe3\xed\xe5\xed\xed\xfb\xe9\x20\xf8\xe0\xf0\x20\xe2\xe7\xf0\xfb\xe2\xe0\xe5\xf2\xf1\xff"
        "\x20\xed\xe0\xe4\x20\xe2\xfb\xe1\xf0\xe0\xed\xed\xfb\xec\x20\xf3\xf7\xe0\xf1\xf2\xea\xee\xec\x20\xef\xee\xeb\xff\x20\xe1\xee\xff\x2c\x20\xef\xee\xf0\xe0\xe6\xe0\xff\x20\xe2\xf1\xe5\x20\xed\xe0"
        "\xf5\xee\xe4\xff\xf9\xe8\xe5\xf1\xff\x20\xef\xee\xe1\xeb\xe8\xe7\xee\xf1\xf2\xe8\x20\xee\xf2\xf0\xff\xe4\xfb\x2e"),
    /* {Огненный удар}\n\nУсовершенствованный вариант огненного шара. Огненный удар поражает отряды, находящиеся в радиусе не одного, а двух полей от эпицентра. */ DATA_COMPGEN(0x0050dd3c, gSpellDescFireblastAnImprovedVersionOfFireballFireblastAffects1,
        "\x7b\xce\xe3\xed\xe5\xed\xed\xfb\xe9\x20\xf3\xe4\xe0\xf0\x7d\x0a\x0a\xd3\xf1\xee\xe2\xe5\xf0\xf8\xe5\xed\xf1\xf2\xe2\xee\xe2\xe0\xed\xed\xfb\xe9\x20\xe2\xe0\xf0\xe8\xe0\xed\xf2\x20\xee\xe3\xed"
        "\xe5\xed\xed\xee\xe3\xee\x20\xf8\xe0\xf0\xe0\x2e\x20\xce\xe3\xed\xe5\xed\xed\xfb\xe9\x20\xf3\xe4\xe0\xf0\x20\xef\xee\xf0\xe0\xe6\xe0\xe5\xf2\x20\xee\xf2\xf0\xff\xe4\xfb\x2c\x20\xed\xe0\xf5\xee"
        "\xe4\xff\xf9\xe8\xe5\xf1\xff\x20\xe2\x20\xf0\xe0\xe4\xe8\xf3\xf1\xe5\x20\xed\xe5\x20\xee\xe4\xed\xee\xe3\xee\x2c\x20\xe0\x20\xe4\xe2\xf3\xf5\x20\xef\xee\xeb\xe5\xe9\x20\xee\xf2\x20\xfd\xef\xe8"
        "\xf6\xe5\xed\xf2\xf0\xe0\x2e"),
    /* {Молния}\n\nМощный электрический разряд поражает выбранный отряд противника. */ DATA_COMPGEN(0x0050ddd4, gSpellDescLightningBoltCausesABoltOfElectricalEnergy2,
        "\x7b\xcc\xee\xeb\xed\xe8\xff\x7d\x0a\x0a\xcc\xee\xf9\xed\xfb\xe9\x20\xfd\xeb\xe5\xea\xf2\xf0\xe8\xf7\xe5\xf1\xea\xe8\xe9\x20\xf0\xe0\xe7\xf0\xff\xe4\x20\xef\xee\xf0\xe0\xe6\xe0\xe5\xf2\x20\xe2"
        "\xfb\xe1\xf0\xe0\xed\xed\xfb\xe9\x20\xee\xf2\xf0\xff\xe4\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e"),
    /* {Цепь молний}\n\nЭлектрический разряд поражает выбранный отряд противника, затем ближайший к нему отряд с половинной силой, затем следующий отряд c еще вдвое меньшей силой, и так далее до тех пор, пока не уходит в землю. Будьте осторожны: это заклинание может поразить и ваши собственные отряды! */ DATA_COMPGEN(0x0050de20, gSpellDescChainLightningCausesABoltOfElectricalEnergy3,
        "\x7b\xd6\xe5\xef\xfc\x20\xec\xee\xeb\xed\xe8\xe9\x7d\x0a\x0a\xdd\xeb\xe5\xea\xf2\xf0\xe8\xf7\xe5\xf1\xea\xe8\xe9\x20\xf0\xe0\xe7\xf0\xff\xe4\x20\xef\xee\xf0\xe0\xe6\xe0\xe5\xf2\x20\xe2\xfb\xe1"
        "\xf0\xe0\xed\xed\xfb\xe9\x20\xee\xf2\xf0\xff\xe4\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2c\x20\xe7\xe0\xf2\xe5\xec\x20\xe1\xeb\xe8\xe6\xe0\xe9\xf8\xe8\xe9\x20\xea\x20\xed\xe5\xec\xf3\x20"
        "\xee\xf2\xf0\xff\xe4\x20\xf1\x20\xef\xee\xeb\xee\xe2\xe8\xed\xed\xee\xe9\x20\xf1\xe8\xeb\xee\xe9\x2c\x20\xe7\xe0\xf2\xe5\xec\x20\xf1\xeb\xe5\xe4\xf3\xfe\xf9\xe8\xe9\x20\xee\xf2\xf0\xff\xe4\x20"
        "\x63\x20\xe5\xf9\xe5\x20\xe2\xe4\xe2\xee\xe5\x20\xec\xe5\xed\xfc\xf8\xe5\xe9\x20\xf1\xe8\xeb\xee\xe9\x2c\x20\xe8\x20\xf2\xe0\xea\x20\xe4\xe0\xeb\xe5\xe5\x20\xe4\xee\x20\xf2\xe5\xf5\x20\xef\xee"
        "\xf0\x2c\x20\xef\xee\xea\xe0\x20\xed\xe5\x20\xf3\xf5\xee\xe4\xe8\xf2\x20\xe2\x20\xe7\xe5\xec\xeb\xfe\x2e\x20\xc1\xf3\xe4\xfc\xf2\xe5\x20\xee\xf1\xf2\xee\xf0\xee\xe6\xed\xfb\x3a\x20\xfd\xf2\xee"
        "\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5\x20\xec\xee\xe6\xe5\xf2\x20\xef\xee\xf0\xe0\xe7\xe8\xf2\xfc\x20\xe8\x20\xe2\xe0\xf8\xe8\x20\xf1\xee\xe1\xf1\xf2\xe2\xe5\xed\xed\xfb\xe5\x20\xee\xf2"
        "\xf0\xff\xe4\xfb\x21"),
    /* {Телепорт}\n\nМгновенно перемещает выбранный отряд в любую свободную точку на поле боя. */ DATA_COMPGEN(0x0050df48, gSpellDescTeleportTeleportsTheCreatureYouSelectToAny4,
        "\x7b\xd2\xe5\xeb\xe5\xef\xee\xf0\xf2\x7d\x0a\x0a\xcc\xe3\xed\xee\xe2\xe5\xed\xed\xee\x20\xef\xe5\xf0\xe5\xec\xe5\xf9\xe0\xe5\xf2\x20\xe2\xfb\xe1\xf0\xe0\xed\xed\xfb\xe9\x20\xee\xf2\xf0\xff\xe4"
        "\x20\xe2\x20\xeb\xfe\xe1\xf3\xfe\x20\xf1\xe2\xee\xe1\xee\xe4\xed\xf3\xfe\x20\xf2\xee\xf7\xea\xf3\x20\xed\xe0\x20\xef\xee\xeb\xe5\x20\xe1\xee\xff\x2e"),
    /* {Лечение}\n\nНейтрализует все враждебные заклинания, примененные против одного из ваших отрядов и восстанавливает по 5 единиц здоровья в расчете на каждый уровень магических способностей героя. */ DATA_COMPGEN(0x0050dfa0, gSpellDescCureRemovesAllNegativeSpellsCastUponOne5,
        "\x7b\xcb\xe5\xf7\xe5\xed\xe8\xe5\x7d\x0a\x0a\xcd\xe5\xe9\xf2\xf0\xe0\xeb\xe8\xe7\xf3\xe5\xf2\x20\xe2\xf1\xe5\x20\xe2\xf0\xe0\xe6\xe4\xe5\xe1\xed\xfb\xe5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8"
        "\xff\x2c\x20\xef\xf0\xe8\xec\xe5\xed\xe5\xed\xed\xfb\xe5\x20\xef\xf0\xee\xf2\xe8\xe2\x20\xee\xe4\xed\xee\xe3\xee\x20\xe8\xe7\x20\xe2\xe0\xf8\xe8\xf5\x20\xee\xf2\xf0\xff\xe4\xee\xe2\x20\xe8\x20"
        "\xe2\xee\xf1\xf1\xf2\xe0\xed\xe0\xe2\xeb\xe8\xe2\xe0\xe5\xf2\x20\xef\xee\x20\x35\x20\xe5\xe4\xe8\xed\xe8\xf6\x20\xe7\xe4\xee\xf0\xee\xe2\xfc\xff\x20\xe2\x20\xf0\xe0\xf1\xf7\xe5\xf2\xe5\x20\xed"
        "\xe0\x20\xea\xe0\xe6\xe4\xfb\xe9\x20\xf3\xf0\xee\xe2\xe5\xed\xfc\x20\xec\xe0\xe3\xe8\xf7\xe5\xf1\xea\xe8\xf5\x20\xf1\xef\xee\xf1\xee\xe1\xed\xee\xf1\xf2\xe5\xe9\x20\xe3\xe5\xf0\xee\xff\x2e"),
    /* {Общее лечение}\n\nНейтрализует враждебные заклинания, примененные против всех ваших отрядов и восстанавливает по 5 единиц здоровья у каждого существа за каждый уровень магических способностей героя. */ DATA_COMPGEN(0x0050e060, gSpellDescMassCureRemovesAllNegativeSpellsCastUpon6,
        "\x7b\xce\xe1\xf9\xe5\xe5\x20\xeb\xe5\xf7\xe5\xed\xe8\xe5\x7d\x0a\x0a\xcd\xe5\xe9\xf2\xf0\xe0\xeb\xe8\xe7\xf3\xe5\xf2\x20\xe2\xf0\xe0\xe6\xe4\xe5\xe1\xed\xfb\xe5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0"
        "\xed\xe8\xff\x2c\x20\xef\xf0\xe8\xec\xe5\xed\xe5\xed\xed\xfb\xe5\x20\xef\xf0\xee\xf2\xe8\xe2\x20\xe2\xf1\xe5\xf5\x20\xe2\xe0\xf8\xe8\xf5\x20\xee\xf2\xf0\xff\xe4\xee\xe2\x20\xe8\x20\xe2\xee\xf1"
        "\xf1\xf2\xe0\xed\xe0\xe2\xeb\xe8\xe2\xe0\xe5\xf2\x20\xef\xee\x20\x35\x20\xe5\xe4\xe8\xed\xe8\xf6\x20\xe7\xe4\xee\xf0\xee\xe2\xfc\xff\x20\xf3\x20\xea\xe0\xe6\xe4\xee\xe3\xee\x20\xf1\xf3\xf9\xe5"
        "\xf1\xf2\xe2\xe0\x20\xe7\xe0\x20\xea\xe0\xe6\xe4\xfb\xe9\x20\xf3\xf0\xee\xe2\xe5\xed\xfc\x20\xec\xe0\xe3\xe8\xf7\xe5\xf1\xea\xe8\xf5\x20\xf1\xef\xee\xf1\xee\xe1\xed\xee\xf1\xf2\xe5\xe9\x20\xe3"
        "\xe5\xf0\xee\xff\x2e"),
    /* {Воскрешение}\n\nДо конца сражения воскрешает воинов в отряде, которому был нанесен урон. */ DATA_COMPGEN(0x0050e128, gSpellDescResurrectResurrectsCreaturesFromADamagedOrDead7,
        "\x7b\xc2\xee\xf1\xea\xf0\xe5\xf8\xe5\xed\xe8\xe5\x7d\x0a\x0a\xc4\xee\x20\xea\xee\xed\xf6\xe0\x20\xf1\xf0\xe0\xe6\xe5\xed\xe8\xff\x20\xe2\xee\xf1\xea\xf0\xe5\xf8\xe0\xe5\xf2\x20\xe2\xee\xe8\xed"
        "\xee\xe2\x20\xe2\x20\xee\xf2\xf0\xff\xe4\xe5\x2c\x20\xea\xee\xf2\xee\xf0\xee\xec\xf3\x20\xe1\xfb\xeb\x20\xed\xe0\xed\xe5\xf1\xe5\xed\x20\xf3\xf0\xee\xed\x2e"),
    /* {Истинное воскрешение}\n\nНавсегда воскрешает воинов в отряде, которому был нанесен урон. */ DATA_COMPGEN(0x0050e180, gSpellDescResurrectTrueResurrectsCreaturesFromADamagedOr8,
        "\x7b\xc8\xf1\xf2\xe8\xed\xed\xee\xe5\x20\xe2\xee\xf1\xea\xf0\xe5\xf8\xe5\xed\xe8\xe5\x7d\x0a\x0a\xcd\xe0\xe2\xf1\xe5\xe3\xe4\xe0\x20\xe2\xee\xf1\xea\xf0\xe5\xf8\xe0\xe5\xf2\x20\xe2\xee\xe8\xed"
        "\xee\xe2\x20\xe2\x20\xee\xf2\xf0\xff\xe4\xe5\x2c\x20\xea\xee\xf2\xee\xf0\xee\xec\xf3\x20\xe1\xfb\xeb\x20\xed\xe0\xed\xe5\xf1\xe5\xed\x20\xf3\xf0\xee\xed\x2e"),
    /* {Ускорение}\n\nУвеличивает дальность передвижения любого отряда на 2 единицы. */ DATA_COMPGEN(0x0050e1d8, gSpellDescHasteIncreasesTheSpeedOfAnyCreatureBy9,
        "\x7b\xd3\xf1\xea\xee\xf0\xe5\xed\xe8\xe5\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xe4\xe0\xeb\xfc\xed\xee\xf1\xf2\xfc\x20\xef\xe5\xf0\xe5\xe4\xe2\xe8\xe6\xe5\xed\xe8\xff\x20"
        "\xeb\xfe\xe1\xee\xe3\xee\x20\xee\xf2\xf0\xff\xe4\xe0\x20\xed\xe0\x20\x32\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x2e"),
    /* {Общее ускорение}\n\nУвеличивает дальность передвижения всех ваших отрядов на 2 единицы. */ DATA_COMPGEN(0x0050e224, gSpellDescMassHasteIncreasesTheSpeedOfAllOf10,
        "\x7b\xce\xe1\xf9\xe5\xe5\x20\xf3\xf1\xea\xee\xf0\xe5\xed\xe8\xe5\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xe4\xe0\xeb\xfc\xed\xee\xf1\xf2\xfc\x20\xef\xe5\xf0\xe5\xe4\xe2\xe8"
        "\xe6\xe5\xed\xe8\xff\x20\xe2\xf1\xe5\xf5\x20\xe2\xe0\xf8\xe8\xf5\x20\xee\xf2\xf0\xff\xe4\xee\xe2\x20\xed\xe0\x20\x32\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x2e"),
    /* {Замедление}\n\nВдвое уменьшает дальность передвижения выбранного отряда противника. */ DATA_COMPGEN(0x0050e27c, gSpellDescSlowSlowsTargetToHalfMovementRate11,
        "\x7b\xc7\xe0\xec\xe5\xe4\xeb\xe5\xed\xe8\xe5\x7d\x0a\x0a\xc2\xe4\xe2\xee\xe5\x20\xf3\xec\xe5\xed\xfc\xf8\xe0\xe5\xf2\x20\xe4\xe0\xeb\xfc\xed\xee\xf1\xf2\xfc\x20\xef\xe5\xf0\xe5\xe4\xe2\xe8\xe6"
        "\xe5\xed\xe8\xff\x20\xe2\xfb\xe1\xf0\xe0\xed\xed\xee\xe3\xee\x20\xee\xf2\xf0\xff\xe4\xe0\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e"),
    /* {Общее замедление}\n\nВдвое снижает дальность перемещения всех отрядов противника. */ DATA_COMPGEN(0x0050e2d0, gSpellDescMassSlowSlowsAllEnemiesToHalfMovement12,
        "\x7b\xce\xe1\xf9\xe5\xe5\x20\xe7\xe0\xec\xe5\xe4\xeb\xe5\xed\xe8\xe5\x7d\x0a\x0a\xc2\xe4\xe2\xee\xe5\x20\xf1\xed\xe8\xe6\xe0\xe5\xf2\x20\xe4\xe0\xeb\xfc\xed\xee\xf1\xf2\xfc\x20\xef\xe5\xf0\xe5"
        "\xec\xe5\xf9\xe5\xed\xe8\xff\x20\xe2\xf1\xe5\xf5\x20\xee\xf2\xf0\xff\xe4\xee\xe2\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e"),
    /* {Ослепление}\n\nЗатуманивает взоры воинов выбранного отряда и тем самым не позволяет им перемещаться по полю боя. */ DATA_COMPGEN(0x0050e324, gSpellDescBlindCloudsTheAffectedCreaturesEyesPreventingThem13,
        "\x7b\xce\xf1\xeb\xe5\xef\xeb\xe5\xed\xe8\xe5\x7d\x0a\x0a\xc7\xe0\xf2\xf3\xec\xe0\xed\xe8\xe2\xe0\xe5\xf2\x20\xe2\xe7\xee\xf0\xfb\x20\xe2\xee\xe8\xed\xee\xe2\x20\xe2\xfb\xe1\xf0\xe0\xed\xed\xee"
        "\xe3\xee\x20\xee\xf2\xf0\xff\xe4\xe0\x20\xe8\x20\xf2\xe5\xec\x20\xf1\xe0\xec\xfb\xec\x20\xed\xe5\x20\xef\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xe8\xec\x20\xef\xe5\xf0\xe5\xec\xe5\xf9\xe0\xf2\xfc"
        "\xf1\xff\x20\xef\xee\x20\xef\xee\xeb\xfe\x20\xe1\xee\xff\x2e"),
    /* {Благословение}\n\nУвеличивает до максимума урон, наносимый выбранным отрядом. */ DATA_COMPGEN(0x0050e394, gSpellDescBlessCausesTheSelectedCreaturesToInflictMaximum14,
        "\x7b\xc1\xeb\xe0\xe3\xee\xf1\xeb\xee\xe2\xe5\xed\xe8\xe5\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xe4\xee\x20\xec\xe0\xea\xf1\xe8\xec\xf3\xec\xe0\x20\xf3\xf0\xee\xed\x2c\x20"
        "\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\xe2\xfb\xe1\xf0\xe0\xed\xed\xfb\xec\x20\xee\xf2\xf0\xff\xe4\xee\xec\x2e"),
    /* {Общее благословение}\n\nУвеличивает до максимума урон, наносимый всеми вашими отрядами. */ DATA_COMPGEN(0x0050e3e4, gSpellDescMassBlessCausesAllOfYourUnitsTo15,
        "\x7b\xce\xe1\xf9\xe5\xe5\x20\xe1\xeb\xe0\xe3\xee\xf1\xeb\xee\xe2\xe5\xed\xe8\xe5\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xe4\xee\x20\xec\xe0\xea\xf1\xe8\xec\xf3\xec\xe0\x20"
        "\xf3\xf0\xee\xed\x2c\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\xe2\xf1\xe5\xec\xe8\x20\xe2\xe0\xf8\xe8\xec\xe8\x20\xee\xf2\xf0\xff\xe4\xe0\xec\xe8\x2e"),
    /* {Каменная кожа}\n\nВолшебным образом повышает защищенность выбранного отряда. */ DATA_COMPGEN(0x0050e43c, gSpellDescStoneskinMagicallyIncreasesTheDefenseSkillOfThe16,
        "\x7b\xca\xe0\xec\xe5\xed\xed\xe0\xff\x20\xea\xee\xe6\xe0\x7d\x0a\x0a\xc2\xee\xeb\xf8\xe5\xe1\xed\xfb\xec\x20\xee\xe1\xf0\xe0\xe7\xee\xec\x20\xef\xee\xe2\xfb\xf8\xe0\xe5\xf2\x20\xe7\xe0\xf9\xe8"
        "\xf9\xe5\xed\xed\xee\xf1\xf2\xfc\x20\xe2\xfb\xe1\xf0\xe0\xed\xed\xee\xe3\xee\x20\xee\xf2\xf0\xff\xe4\xe0\x2e"),
    /* {Стальная кожа}\n\nПовышает защищенность выбранного отряда. Усовершенствованный вариант заклинания Каменная кожа. */ DATA_COMPGEN(0x0050e488, gSpellDescSteelskinIncreasesTheDefenseSkillOfTheTargeted17,
        "\x7b\xd1\xf2\xe0\xeb\xfc\xed\xe0\xff\x20\xea\xee\xe6\xe0\x7d\x0a\x0a\xcf\xee\xe2\xfb\xf8\xe0\xe5\xf2\x20\xe7\xe0\xf9\xe8\xf9\xe5\xed\xed\xee\xf1\xf2\xfc\x20\xe2\xfb\xe1\xf0\xe0\xed\xed\xee\xe3"
        "\xee\x20\xee\xf2\xf0\xff\xe4\xe0\x2e\x20\xd3\xf1\xee\xe2\xe5\xf0\xf8\xe5\xed\xf1\xf2\xe2\xee\xe2\xe0\xed\xed\xfb\xe9\x20\xe2\xe0\xf0\xe8\xe0\xed\xf2\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff"
        "\x20\xca\xe0\xec\xe5\xed\xed\xe0\xff\x20\xea\xee\xe6\xe0\x2e"),
    /* {Проклятие}\n\nУменьшает до минимума урон, причиняемый выбранным отрядом противника. */ DATA_COMPGEN(0x0050e4f8, gSpellDescCurseCausesTheSelectedCreaturesToInflictMinimum18,
        "\x7b\xcf\xf0\xee\xea\xeb\xff\xf2\xe8\xe5\x7d\x0a\x0a\xd3\xec\xe5\xed\xfc\xf8\xe0\xe5\xf2\x20\xe4\xee\x20\xec\xe8\xed\xe8\xec\xf3\xec\xe0\x20\xf3\xf0\xee\xed\x2c\x20\xef\xf0\xe8\xf7\xe8\xed\xff"
        "\xe5\xec\xfb\xe9\x20\xe2\xfb\xe1\xf0\xe0\xed\xed\xfb\xec\x20\xee\xf2\xf0\xff\xe4\xee\xec\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e"),
    /* {Общее проклятие}\n\nУменьшает до минимума урон, причиняемый всеми отрядами противника. */ DATA_COMPGEN(0x0050e54c, gSpellDescMassCurseCausesAllEnemyTroopsToInflict19,
        "\x7b\xce\xe1\xf9\xe5\xe5\x20\xef\xf0\xee\xea\xeb\xff\xf2\xe8\xe5\x7d\x0a\x0a\xd3\xec\xe5\xed\xfc\xf8\xe0\xe5\xf2\x20\xe4\xee\x20\xec\xe8\xed\xe8\xec\xf3\xec\xe0\x20\xf3\xf0\xee\xed\x2c\x20\xef"
        "\xf0\xe8\xf7\xe8\xed\xff\xe5\xec\xfb\xe9\x20\xe2\xf1\xe5\xec\xe8\x20\xee\xf2\xf0\xff\xe4\xe0\xec\xe8\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e"),
    /* {Святое слово}\n\nНаносит урон всей нежити на поле боя. */ DATA_COMPGEN(0x0050e5a4, gSpellDescHolyWordDamagesAllUndeadInTheBattle20,
        "\x7b\xd1\xe2\xff\xf2\xee\xe5\x20\xf1\xeb\xee\xe2\xee\x7d\x0a\x0a\xcd\xe0\xed\xee\xf1\xe8\xf2\x20\xf3\xf0\xee\xed\x20\xe2\xf1\xe5\xe9\x20\xed\xe5\xe6\xe8\xf2\xe8\x20\xed\xe0\x20\xef\xee\xeb\xe5"
        "\x20\xe1\xee\xff\x2e"),
    /* {Святой глас}\n\nНаносит урон всей нежити на поле боя. Усовершенствованный вариант заклинания Святое слово. */ DATA_COMPGEN(0x0050e5dc, gSpellDescHolyShoutDamagesAllUndeadInTheBattle21,
        "\x7b\xd1\xe2\xff\xf2\xee\xe9\x20\xe3\xeb\xe0\xf1\x7d\x0a\x0a\xcd\xe0\xed\xee\xf1\xe8\xf2\x20\xf3\xf0\xee\xed\x20\xe2\xf1\xe5\xe9\x20\xed\xe5\xe6\xe8\xf2\xe8\x20\xed\xe0\x20\xef\xee\xeb\xe5\x20"
        "\xe1\xee\xff\x2e\x20\xd3\xf1\xee\xe2\xe5\xf0\xf8\xe5\xed\xf1\xf2\xe2\xee\xe2\xe0\xed\xed\xfb\xe9\x20\xe2\xe0\xf0\xe8\xe0\xed\xf2\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\x20\xd1\xe2\xff\xf2"
        "\xee\xe5\x20\xf1\xeb\xee\xe2\xee\x2e"),
    /* {Антимагия}\n\nЗащищает выбранный отряд от враждебных заклинаний. */ DATA_COMPGEN(0x0050e648, gSpellDescAntiMagicPreventsHarmfulMagicAgainstTheSelected22,
        "\x7b\xc0\xed\xf2\xe8\xec\xe0\xe3\xe8\xff\x7d\x0a\x0a\xc7\xe0\xf9\xe8\xf9\xe0\xe5\xf2\x20\xe2\xfb\xe1\xf0\xe0\xed\xed\xfb\xe9\x20\xee\xf2\xf0\xff\xe4\x20\xee\xf2\x20\xe2\xf0\xe0\xe6\xe4\xe5\xe1"
        "\xed\xfb\xf5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe9\x2e"),
    /* {Снятие чар}\n\nСнимает все чары с выбранного отряда. */ DATA_COMPGEN(0x0050e688, gSpellDescDispelMagicRemovesAllMagicSpellsFromA23,
        "\x7b\xd1\xed\xff\xf2\xe8\xe5\x20\xf7\xe0\xf0\x7d\x0a\x0a\xd1\xed\xe8\xec\xe0\xe5\xf2\x20\xe2\xf1\xe5\x20\xf7\xe0\xf0\xfb\x20\xf1\x20\xe2\xfb\xe1\xf0\xe0\xed\xed\xee\xe3\xee\x20\xee\xf2\xf0\xff"
        "\xe4\xe0\x2e"),
    /* {Общее снятие чар}\n\nСнимает все чары со всех отрядов. */ DATA_COMPGEN(0x0050e6bc, gSpellDescMassDispelRemovesAllMagicSpellsFromAll24,
        "\x7b\xce\xe1\xf9\xe5\xe5\x20\xf1\xed\xff\xf2\xe8\xe5\x20\xf7\xe0\xf0\x7d\x0a\x0a\xd1\xed\xe8\xec\xe0\xe5\xf2\x20\xe2\xf1\xe5\x20\xf7\xe0\xf0\xfb\x20\xf1\xee\x20\xe2\xf1\xe5\xf5\x20\xee\xf2\xf0"
        "\xff\xe4\xee\xe2\x2e"),
    /* {Волшебная стрела}\n\nВолшебная стрела поражает выбранный отряд противника. */ DATA_COMPGEN(0x0050e6f4, gSpellDescMagicArrowCausesAMagicArrowToStrike25,
        "\x7b\xc2\xee\xeb\xf8\xe5\xe1\xed\xe0\xff\x20\xf1\xf2\xf0\xe5\xeb\xe0\x7d\x0a\x0a\xc2\xee\xeb\xf8\xe5\xe1\xed\xe0\xff\x20\xf1\xf2\xf0\xe5\xeb\xe0\x20\xef\xee\xf0\xe0\xe6\xe0\xe5\xf2\x20\xe2\xfb"
        "\xe1\xf0\xe0\xed\xed\xfb\xe9\x20\xee\xf2\xf0\xff\xe4\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e"),
    /* {Берсерк}\n\nЗаставляет выбранный отряд противника нападать на ближайший к нему отряд. */ DATA_COMPGEN(0x0050e740, gSpellDescBerzerkerCausesACreatureToAttackItsNearest26,
        "\x7b\xc1\xe5\xf0\xf1\xe5\xf0\xea\x7d\x0a\x0a\xc7\xe0\xf1\xf2\xe0\xe2\xeb\xff\xe5\xf2\x20\xe2\xfb\xe1\xf0\xe0\xed\xed\xfb\xe9\x20\xee\xf2\xf0\xff\xe4\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0"
        "\x20\xed\xe0\xef\xe0\xe4\xe0\xf2\xfc\x20\xed\xe0\x20\xe1\xeb\xe8\xe6\xe0\xe9\xf8\xe8\xe9\x20\xea\x20\xed\xe5\xec\xf3\x20\xee\xf2\xf0\xff\xe4\x2e"),
    /* {Армагеддон}\n\nУжасный катаклизм обрушивается на поле боя, нанося жестокий урон всем участникам сражения. */ DATA_COMPGEN(0x0050e798, gSpellDescArmageddonHolyTerrorStrikesTheBattlefieldCausingSevere27,
        "\x7b\xc0\xf0\xec\xe0\xe3\xe5\xe4\xe4\xee\xed\x7d\x0a\x0a\xd3\xe6\xe0\xf1\xed\xfb\xe9\x20\xea\xe0\xf2\xe0\xea\xeb\xe8\xe7\xec\x20\xee\xe1\xf0\xf3\xf8\xe8\xe2\xe0\xe5\xf2\xf1\xff\x20\xed\xe0\x20"
        "\xef\xee\xeb\xe5\x20\xe1\xee\xff\x2c\x20\xed\xe0\xed\xee\xf1\xff\x20\xe6\xe5\xf1\xf2\xee\xea\xe8\xe9\x20\xf3\xf0\xee\xed\x20\xe2\xf1\xe5\xec\x20\xf3\xf7\xe0\xf1\xf2\xed\xe8\xea\xe0\xec\x20\xf1"
        "\xf0\xe0\xe6\xe5\xed\xe8\xff\x2e"),
    /* {Буря стихий}\n\nСилы стихий обрушиваются на поле боя, нанося урон всем участникам сражения. */ DATA_COMPGEN(0x0050e804, gSpellDescElementalStormMagicalElementsPourDownOnThe28,
        "\x7b\xc1\xf3\xf0\xff\x20\xf1\xf2\xe8\xf5\xe8\xe9\x7d\x0a\x0a\xd1\xe8\xeb\xfb\x20\xf1\xf2\xe8\xf5\xe8\xe9\x20\xee\xe1\xf0\xf3\xf8\xe8\xe2\xe0\xfe\xf2\xf1\xff\x20\xed\xe0\x20\xef\xee\xeb\xe5\x20"
        "\xe1\xee\xff\x2c\x20\xed\xe0\xed\xee\xf1\xff\x20\xf3\xf0\xee\xed\x20\xe2\xf1\xe5\xec\x20\xf3\xf7\xe0\xf1\xf2\xed\xe8\xea\xe0\xec\x20\xf1\xf0\xe0\xe6\xe5\xed\xe8\xff\x2e"),
    /* {Звездопад}\n\nЗвездопад поражает выбранный участок поля боя, нанося урон всем находящимся поблизости участникам сражения. */ DATA_COMPGEN(0x0050e860, gSpellDescMeteorShowerARainOfRocksStrikesAn29,
        "\x7b\xc7\xe2\xe5\xe7\xe4\xee\xef\xe0\xe4\x7d\x0a\x0a\xc7\xe2\xe5\xe7\xe4\xee\xef\xe0\xe4\x20\xef\xee\xf0\xe0\xe6\xe0\xe5\xf2\x20\xe2\xfb\xe1\xf0\xe0\xed\xed\xfb\xe9\x20\xf3\xf7\xe0\xf1\xf2\xee"
        "\xea\x20\xef\xee\xeb\xff\x20\xe1\xee\xff\x2c\x20\xed\xe0\xed\xee\xf1\xff\x20\xf3\xf0\xee\xed\x20\xe2\xf1\xe5\xec\x20\xed\xe0\xf5\xee\xe4\xff\xf9\xe8\xec\xf1\xff\x20\xef\xee\xe1\xeb\xe8\xe7\xee"
        "\xf1\xf2\xe8\x20\xf3\xf7\xe0\xf1\xf2\xed\xe8\xea\xe0\xec\x20\xf1\xf0\xe0\xe6\xe5\xed\xe8\xff\x2e"),
    /* {Паралич}\n\nОтряд, против которого направлено это заклинание, поражает паралич, и он теряет способность передвигаться или отвечать на удары. */ DATA_COMPGEN(0x0050e8dc, gSpellDescParalyzeTheTargetedCreaturesAreParalyzedUnableTo30,
        "\x7b\xcf\xe0\xf0\xe0\xeb\xe8\xf7\x7d\x0a\x0a\xce\xf2\xf0\xff\xe4\x2c\x20\xef\xf0\xee\xf2\xe8\xe2\x20\xea\xee\xf2\xee\xf0\xee\xe3\xee\x20\xed\xe0\xef\xf0\xe0\xe2\xeb\xe5\xed\xee\x20\xfd\xf2\xee"
        "\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5\x2c\x20\xef\xee\xf0\xe0\xe6\xe0\xe5\xf2\x20\xef\xe0\xf0\xe0\xeb\xe8\xf7\x2c\x20\xe8\x20\xee\xed\x20\xf2\xe5\xf0\xff\xe5\xf2\x20\xf1\xef\xee\xf1\xee"
        "\xe1\xed\xee\xf1\xf2\xfc\x20\xef\xe5\xf0\xe5\xe4\xe2\xe8\xe3\xe0\xf2\xfc\xf1\xff\x20\xe8\xeb\xe8\x20\xee\xf2\xe2\xe5\xf7\xe0\xf2\xfc\x20\xed\xe0\x20\xf3\xe4\xe0\xf0\xfb\x2e"),
    /* {Гипноз}\n\nВыбранный отряд противника переходит под контроль вашего героя на один ход, если его суммарное здоровье не превышает магических способностей героя, умноженных на 25. */ DATA_COMPGEN(0x0050e968, gSpellDescHypnotizeBringsASingleEnemyUnitUnderYour31,
        "\x7b\xc3\xe8\xef\xed\xee\xe7\x7d\x0a\x0a\xc2\xfb\xe1\xf0\xe0\xed\xed\xfb\xe9\x20\xee\xf2\xf0\xff\xe4\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x20\xef\xe5\xf0\xe5\xf5\xee\xe4\xe8\xf2\x20\xef"
        "\xee\xe4\x20\xea\xee\xed\xf2\xf0\xee\xeb\xfc\x20\xe2\xe0\xf8\xe5\xe3\xee\x20\xe3\xe5\xf0\xee\xff\x20\xed\xe0\x20\xee\xe4\xe8\xed\x20\xf5\xee\xe4\x2c\x20\xe5\xf1\xeb\xe8\x20\xe5\xe3\xee\x20\xf1"
        "\xf3\xec\xec\xe0\xf0\xed\xee\xe5\x20\xe7\xe4\xee\xf0\xee\xe2\xfc\xe5\x20\xed\xe5\x20\xef\xf0\xe5\xe2\xfb\xf8\xe0\xe5\xf2\x20\xec\xe0\xe3\xe8\xf7\xe5\xf1\xea\xe8\xf5\x20\xf1\xef\xee\xf1\xee\xe1"
        "\xed\xee\xf1\xf2\xe5\xe9\x20\xe3\xe5\xf0\xee\xff\x2c\x20\xf3\xec\xed\xee\xe6\xe5\xed\xed\xfb\xf5\x20\xed\xe0\x20\x32\x35\x2e"),
    /* {Хладный луч}\n\nВысасывает тепло жизни из выбранного отряда противника. */ DATA_COMPGEN(0x0050ea18, gSpellDescColdRayDrainsBodyHeatFromASingle32,
        "\x7b\xd5\xeb\xe0\xe4\xed\xfb\xe9\x20\xeb\xf3\xf7\x7d\x0a\x0a\xc2\xfb\xf1\xe0\xf1\xfb\xe2\xe0\xe5\xf2\x20\xf2\xe5\xef\xeb\xee\x20\xe6\xe8\xe7\xed\xe8\x20\xe8\xe7\x20\xe2\xfb\xe1\xf0\xe0\xed\xed"
        "\xee\xe3\xee\x20\xee\xf2\xf0\xff\xe4\xe0\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e"),
    /* {Кольцо стужи}\n\nВысасывает тепло жизни из всех отрядов вокруг эпицентра заклинания, за исключением находящегося в самом эпицентре. */ DATA_COMPGEN(0x0050ea60, gSpellDescColdRingDrainsBodyHeatFromAllUnits33,
        "\x7b\xca\xee\xeb\xfc\xf6\xee\x20\xf1\xf2\xf3\xe6\xe8\x7d\x0a\x0a\xc2\xfb\xf1\xe0\xf1\xfb\xe2\xe0\xe5\xf2\x20\xf2\xe5\xef\xeb\xee\x20\xe6\xe8\xe7\xed\xe8\x20\xe8\xe7\x20\xe2\xf1\xe5\xf5\x20\xee"
        "\xf2\xf0\xff\xe4\xee\xe2\x20\xe2\xee\xea\xf0\xf3\xe3\x20\xfd\xef\xe8\xf6\xe5\xed\xf2\xf0\xe0\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\x2c\x20\xe7\xe0\x20\xe8\xf1\xea\xeb\xfe\xf7\xe5\xed\xe8"
        "\xe5\xec\x20\xed\xe0\xf5\xee\xe4\xff\xf9\xe5\xe3\xee\xf1\xff\x20\xe2\x20\xf1\xe0\xec\xee\xec\x20\xfd\xef\xe8\xf6\xe5\xed\xf2\xf0\xe5\x2e"),
    /* {Разрушительный луч}\n\nПонижает защиту выбранного отряда противника на 3 единицы. */ DATA_COMPGEN(0x0050eae4, gSpellDescDisruptingRayReducesTheDefenseRatingOfAn34,
        "\x7b\xd0\xe0\xe7\xf0\xf3\xf8\xe8\xf2\xe5\xeb\xfc\xed\xfb\xe9\x20\xeb\xf3\xf7\x7d\x0a\x0a\xcf\xee\xed\xe8\xe6\xe0\xe5\xf2\x20\xe7\xe0\xf9\xe8\xf2\xf3\x20\xe2\xfb\xe1\xf0\xe0\xed\xed\xee\xe3\xee"
        "\x20\xee\xf2\xf0\xff\xe4\xe0\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x20\xed\xe0\x20\x33\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x2e"),
    /* {Дрожь смерти}\n\nНаносит урон всем отрядам живых воинов в сражении, но не действует на нежить. */ DATA_COMPGEN(0x0050eb38, gSpellDescDeathRippleDamagesAllLivingNonUndeadUnits35,
        "\x7b\xc4\xf0\xee\xe6\xfc\x20\xf1\xec\xe5\xf0\xf2\xe8\x7d\x0a\x0a\xcd\xe0\xed\xee\xf1\xe8\xf2\x20\xf3\xf0\xee\xed\x20\xe2\xf1\xe5\xec\x20\xee\xf2\xf0\xff\xe4\xe0\xec\x20\xe6\xe8\xe2\xfb\xf5\x20"
        "\xe2\xee\xe8\xed\xee\xe2\x20\xe2\x20\xf1\xf0\xe0\xe6\xe5\xed\xe8\xe8\x2c\x20\xed\xee\x20\xed\xe5\x20\xe4\xe5\xe9\xf1\xf2\xe2\xf3\xe5\xf2\x20\xed\xe0\x20\xed\xe5\xe6\xe8\xf2\xfc\x2e"),
    /* {Волна смерти}\n\nНаносит урон всем отрядам живых воинов в сражении, но не действует на нежить. Усовершенствованный вариант заклинания Дрожь смерти. */ DATA_COMPGEN(0x0050eb98, gSpellDescDeathWaveDamagesAllLivingNonUndeadUnits36,
        "\x7b\xc2\xee\xeb\xed\xe0\x20\xf1\xec\xe5\xf0\xf2\xe8\x7d\x0a\x0a\xcd\xe0\xed\xee\xf1\xe8\xf2\x20\xf3\xf0\xee\xed\x20\xe2\xf1\xe5\xec\x20\xee\xf2\xf0\xff\xe4\xe0\xec\x20\xe6\xe8\xe2\xfb\xf5\x20"
        "\xe2\xee\xe8\xed\xee\xe2\x20\xe2\x20\xf1\xf0\xe0\xe6\xe5\xed\xe8\xe8\x2c\x20\xed\xee\x20\xed\xe5\x20\xe4\xe5\xe9\xf1\xf2\xe2\xf3\xe5\xf2\x20\xed\xe0\x20\xed\xe5\xe6\xe8\xf2\xfc\x2e\x20\xd3\xf1"
        "\xee\xe2\xe5\xf0\xf8\xe5\xed\xf1\xf2\xe2\xee\xe2\xe0\xed\xed\xfb\xe9\x20\xe2\xe0\xf0\xe8\xe0\xed\xf2\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\x20\xc4\xf0\xee\xe6\xfc\x20\xf1\xec\xe5\xf0\xf2"
        "\xe8\x2e"),
    /* {Убийца драконов}\n\nЗначительно увеличивает урон, наносимый выбранным отрядом в бою против драконов. */ DATA_COMPGEN(0x0050ec2c, gSpellDescDragonSlayerGreatlyIncreasesAUnitSAttack37,
        "\x7b\xd3\xe1\xe8\xe9\xf6\xe0\x20\xe4\xf0\xe0\xea\xee\xed\xee\xe2\x7d\x0a\x0a\xc7\xed\xe0\xf7\xe8\xf2\xe5\xeb\xfc\xed\xee\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xf3\xf0\xee\xed\x2c"
        "\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\xe2\xfb\xe1\xf0\xe0\xed\xed\xfb\xec\x20\xee\xf2\xf0\xff\xe4\xee\xec\x20\xe2\x20\xe1\xee\xfe\x20\xef\xf0\xee\xf2\xe8\xe2\x20\xe4\xf0\xe0\xea\xee\xed"
        "\xee\xe2\x2e"),
    /* {Жажда крови}\n\nУвеличивает урон, наносимый выбранным отрядом. */ DATA_COMPGEN(0x0050ec90, gSpellDescBloodLustIncreasesAUnitSAttackSkill38,
        "\x7b\xc6\xe0\xe6\xe4\xe0\x20\xea\xf0\xee\xe2\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xf3\xf0\xee\xed\x2c\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\xe2\xfb\xe1\xf0\xe0"
        "\xed\xed\xfb\xec\x20\xee\xf2\xf0\xff\xe4\xee\xec\x2e"),
    /* {Поднять мертвых}\n\nНавсегда "воскрешает" из раненных или уничтоженных отрядов нежити. */ DATA_COMPGEN(0x0050ecd0, gSpellDescAnimateDeadResurrectsCreaturesFromADamagedOr39,
        "\x7b\xcf\xee\xe4\xed\xff\xf2\xfc\x20\xec\xe5\xf0\xf2\xe2\xfb\xf5\x7d\x0a\x0a\xcd\xe0\xe2\xf1\xe5\xe3\xe4\xe0\x20\x22\xe2\xee\xf1\xea\xf0\xe5\xf8\xe0\xe5\xf2\x22\x20\xe8\xe7\x20\xf0\xe0\xed\xe5"
        "\xed\xed\xfb\xf5\x20\xe8\xeb\xe8\x20\xf3\xed\xe8\xf7\xf2\xee\xe6\xe5\xed\xed\xfb\xf5\x20\xee\xf2\xf0\xff\xe4\xee\xe2\x20\xed\xe5\xe6\xe8\xf2\xe8\x2e"),
    /* {Фантом}\n\nЗаклинание создает призрачный отряд, который является двойником существующего отряда. Призрачный отряд наносит противнику такой же урон, как и настоящий, но исчезает, если ему был нанесен хотя бы минимальный урон. */ DATA_COMPGEN(0x0050ed28, gSpellDescMirrorImageCreatesAnIllusionaryUnitThatDuplicates40,
        "\x7b\xd4\xe0\xed\xf2\xee\xec\x7d\x0a\x0a\xc7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5\x20\xf1\xee\xe7\xe4\xe0\xe5\xf2\x20\xef\xf0\xe8\xe7\xf0\xe0\xf7\xed\xfb\xe9\x20\xee\xf2\xf0\xff\xe4\x2c\x20\xea"
        "\xee\xf2\xee\xf0\xfb\xe9\x20\xff\xe2\xeb\xff\xe5\xf2\xf1\xff\x20\xe4\xe2\xee\xe9\xed\xe8\xea\xee\xec\x20\xf1\xf3\xf9\xe5\xf1\xf2\xe2\xf3\xfe\xf9\xe5\xe3\xee\x20\xee\xf2\xf0\xff\xe4\xe0\x2e\x20"
        "\xcf\xf0\xe8\xe7\xf0\xe0\xf7\xed\xfb\xe9\x20\xee\xf2\xf0\xff\xe4\x20\xed\xe0\xed\xee\xf1\xe8\xf2\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xf3\x20\xf2\xe0\xea\xee\xe9\x20\xe6\xe5\x20\xf3\xf0\xee"
        "\xed\x2c\x20\xea\xe0\xea\x20\xe8\x20\xed\xe0\xf1\xf2\xee\xff\xf9\xe8\xe9\x2c\x20\xed\xee\x20\xe8\xf1\xf7\xe5\xe7\xe0\xe5\xf2\x2c\x20\xe5\xf1\xeb\xe8\x20\xe5\xec\xf3\x20\xe1\xfb\xeb\x20\xed\xe0"
        "\xed\xe5\xf1\xe5\xed\x20\xf5\xee\xf2\xff\x20\xe1\xfb\x20\xec\xe8\xed\xe8\xec\xe0\xeb\xfc\xed\xfb\xe9\x20\xf3\xf0\xee\xed\x2e"),
    /* {Щит}\n\nВдвое уменьшает урон, получаемый выбранным отрядом от стрелковых атак противника. */ DATA_COMPGEN(0x0050ee08, gSpellDescShieldHalvesDamageReceivedFromRangedAttacksFor41,
        "\x7b\xd9\xe8\xf2\x7d\x0a\x0a\xc2\xe4\xe2\xee\xe5\x20\xf3\xec\xe5\xed\xfc\xf8\xe0\xe5\xf2\x20\xf3\xf0\xee\xed\x2c\x20\xef\xee\xeb\xf3\xf7\xe0\xe5\xec\xfb\xe9\x20\xe2\xfb\xe1\xf0\xe0\xed\xed\xfb"
        "\xec\x20\xee\xf2\xf0\xff\xe4\xee\xec\x20\xee\xf2\x20\xf1\xf2\xf0\xe5\xeb\xea\xee\xe2\xfb\xf5\x20\xe0\xf2\xe0\xea\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e"),
    /* {Общий щит}\n\nВдвое уменьшает урон, получаемый всеми отрядами от стрелковых атак противника. */ DATA_COMPGEN(0x0050ee64, gSpellDescMassShieldHalvesDamageReceivedFromRangedAttacks42,
        "\x7b\xce\xe1\xf9\xe8\xe9\x20\xf9\xe8\xf2\x7d\x0a\x0a\xc2\xe4\xe2\xee\xe5\x20\xf3\xec\xe5\xed\xfc\xf8\xe0\xe5\xf2\x20\xf3\xf0\xee\xed\x2c\x20\xef\xee\xeb\xf3\xf7\xe0\xe5\xec\xfb\xe9\x20\xe2\xf1"
        "\xe5\xec\xe8\x20\xee\xf2\xf0\xff\xe4\xe0\xec\xe8\x20\xee\xf2\x20\xf1\xf2\xf0\xe5\xeb\xea\xee\xe2\xfb\xf5\x20\xe0\xf2\xe0\xea\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e"),
    /* {Земной элементал}\n\nЗаклинание вызывает отряд элементалов земли, которые присоединяются к вашей армии. */ DATA_COMPGEN(0x0050eec0, gSpellDescSummonEarthElementalSummonsEarthElementalsToFight43,
        "\x7b\xc7\xe5\xec\xed\xee\xe9\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb\x7d\x0a\x0a\xc7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5\x20\xe2\xfb\xe7\xfb\xe2\xe0\xe5\xf2\x20\xee\xf2\xf0\xff\xe4\x20\xfd\xeb"
        "\xe5\xec\xe5\xed\xf2\xe0\xeb\xee\xe2\x20\xe7\xe5\xec\xeb\xe8\x2c\x20\xea\xee\xf2\xee\xf0\xfb\xe5\x20\xef\xf0\xe8\xf1\xee\xe5\xe4\xe8\xed\xff\xfe\xf2\xf1\xff\x20\xea\x20\xe2\xe0\xf8\xe5\xe9\x20"
        "\xe0\xf0\xec\xe8\xe8\x2e"),
    /* {Воздушный элементал}\n\nЗаклинание вызывает отряд элементалов воздуха, которые присоединяются к вашей армии. */ DATA_COMPGEN(0x0050ef28, gSpellDescSummonAirElementalSummonsAirElementalsToFight44,
        "\x7b\xc2\xee\xe7\xe4\xf3\xf8\xed\xfb\xe9\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb\x7d\x0a\x0a\xc7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5\x20\xe2\xfb\xe7\xfb\xe2\xe0\xe5\xf2\x20\xee\xf2\xf0\xff\xe4"
        "\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb\xee\xe2\x20\xe2\xee\xe7\xe4\xf3\xf5\xe0\x2c\x20\xea\xee\xf2\xee\xf0\xfb\xe5\x20\xef\xf0\xe8\xf1\xee\xe5\xe4\xe8\xed\xff\xfe\xf2\xf1\xff\x20\xea\x20\xe2"
        "\xe0\xf8\xe5\xe9\x20\xe0\xf0\xec\xe8\xe8\x2e"),
    /* {Огненный элементал}\n\nЗаклинание вызывает отряд элементалов огня, которые присоединяются к вашей армии. */ DATA_COMPGEN(0x0050ef94, gSpellDescSummonFireElementalSummonsFireElementalsToFight45,
        "\x7b\xce\xe3\xed\xe5\xed\xed\xfb\xe9\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb\x7d\x0a\x0a\xc7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5\x20\xe2\xfb\xe7\xfb\xe2\xe0\xe5\xf2\x20\xee\xf2\xf0\xff\xe4\x20"
        "\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb\xee\xe2\x20\xee\xe3\xed\xff\x2c\x20\xea\xee\xf2\xee\xf0\xfb\xe5\x20\xef\xf0\xe8\xf1\xee\xe5\xe4\xe8\xed\xff\xfe\xf2\xf1\xff\x20\xea\x20\xe2\xe0\xf8\xe5\xe9"
        "\x20\xe0\xf0\xec\xe8\xe8\x2e"),
    /* {Водный элементал}\n\nЗаклинание вызывает отряд элеманталов воздуха, которей присоединяются к вашей армии. */ DATA_COMPGEN(0x0050effc, gSpellDescSummonWaterElementalSummonsWaterElementalsToFight46,
        "\x7b\xc2\xee\xe4\xed\xfb\xe9\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb\x7d\x0a\x0a\xc7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5\x20\xe2\xfb\xe7\xfb\xe2\xe0\xe5\xf2\x20\xee\xf2\xf0\xff\xe4\x20\xfd\xeb"
        "\xe5\xec\xe0\xed\xf2\xe0\xeb\xee\xe2\x20\xe2\xee\xe7\xe4\xf3\xf5\xe0\x2c\x20\xea\xee\xf2\xee\xf0\xe5\xe9\x20\xef\xf0\xe8\xf1\xee\xe5\xe4\xe8\xed\xff\xfe\xf2\xf1\xff\x20\xea\x20\xe2\xe0\xf8\xe5"
        "\xe9\x20\xe0\xf0\xec\xe8\xe8\x2e"),
    /* {Землетрясение}\n\nНаносит ущерб крепостным стенам. */ DATA_COMPGEN(0x0050f068, gSpellDescEarthquakeDamagesCastleWalls47,
        "\x7b\xc7\xe5\xec\xeb\xe5\xf2\xf0\xff\xf1\xe5\xed\xe8\xe5\x7d\x0a\x0a\xcd\xe0\xed\xee\xf1\xe8\xf2\x20\xf3\xf9\xe5\xf0\xe1\x20\xea\xf0\xe5\xef\xee\xf1\xf2\xed\xfb\xec\x20\xf1\xf2\xe5\xed\xe0\xec"
        "\x2e"),
    /* {Показать шахты}\n\nДелает видимыми все шахты на игровой карте. */ DATA_COMPGEN(0x0050f09c, gSpellDescViewMinesCausesAllMinesAcrossTheLand48,
        "\x7b\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xf8\xe0\xf5\xf2\xfb\x7d\x0a\x0a\xc4\xe5\xeb\xe0\xe5\xf2\x20\xe2\xe8\xe4\xe8\xec\xfb\xec\xe8\x20\xe2\xf1\xe5\x20\xf8\xe0\xf5\xf2\xfb\x20\xed\xe0\x20\xe8"
        "\xe3\xf0\xee\xe2\xee\xe9\x20\xea\xe0\xf0\xf2\xe5\x2e"),
    /* {Показать ресурсы}\n\nПоказывает все ресурсы на игровой карте. */ DATA_COMPGEN(0x0050f0dc, gSpellDescViewResourcesCausesAllResourcesAcrossTheLand49,
        "\x7b\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xf0\xe5\xf1\xf3\xf0\xf1\xfb\x7d\x0a\x0a\xcf\xee\xea\xe0\xe7\xfb\xe2\xe0\xe5\xf2\x20\xe2\xf1\xe5\x20\xf0\xe5\xf1\xf3\xf0\xf1\xfb\x20\xed\xe0\x20\xe8\xe3"
        "\xf0\xee\xe2\xee\xe9\x20\xea\xe0\xf0\xf2\xe5\x2e"),
    /* {Показать артефакты}\n\nДелает видимыми все артефакты на игровой карте. */ DATA_COMPGEN(0x0050f11c, gSpellDescViewArtifactsCausesAllArtifactsAcrossTheLand50,
        "\x7b\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2\xfb\x7d\x0a\x0a\xc4\xe5\xeb\xe0\xe5\xf2\x20\xe2\xe8\xe4\xe8\xec\xfb\xec\xe8\x20\xe2\xf1\xe5\x20\xe0\xf0\xf2\xe5\xf4\xe0"
        "\xea\xf2\xfb\x20\xed\xe0\x20\xe8\xe3\xf0\xee\xe2\xee\xe9\x20\xea\xe0\xf0\xf2\xe5\x2e"),
    /* {Показать города}\n\nДелает видимыми все города и замки на игровой карте. */ DATA_COMPGEN(0x0050f164, gSpellDescViewTownsCausesAllTownsAndCastlesAcross51,
        "\x7b\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xe3\xee\xf0\xee\xe4\xe0\x7d\x0a\x0a\xc4\xe5\xeb\xe0\xe5\xf2\x20\xe2\xe8\xe4\xe8\xec\xfb\xec\xe8\x20\xe2\xf1\xe5\x20\xe3\xee\xf0\xee\xe4\xe0\x20\xe8\x20"
        "\xe7\xe0\xec\xea\xe8\x20\xed\xe0\x20\xe8\xe3\xf0\xee\xe2\xee\xe9\x20\xea\xe0\xf0\xf2\xe5\x2e"),
    /* {Показать героев}\n\nДелает видимыми всех героев на игровой карте. */ DATA_COMPGEN(0x0050f1ac, gSpellDescViewHeroesCausesAllHeroesAcrossTheLand52,
        "\x7b\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xe3\xe5\xf0\xee\xe5\xe2\x7d\x0a\x0a\xc4\xe5\xeb\xe0\xe5\xf2\x20\xe2\xe8\xe4\xe8\xec\xfb\xec\xe8\x20\xe2\xf1\xe5\xf5\x20\xe3\xe5\xf0\xee\xe5\xe2\x20\xed"
        "\xe0\x20\xe8\xe3\xf0\xee\xe2\xee\xe9\x20\xea\xe0\xf0\xf2\xe5\x2e"),
    /* {Показать все}\n\nДелает видимыми все объекты на игровой карте. */ DATA_COMPGEN(0x0050f1f0, gSpellDescViewAllCausesTheEntireLandToBecome53,
        "\x7b\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xe2\xf1\xe5\x7d\x0a\x0a\xc4\xe5\xeb\xe0\xe5\xf2\x20\xe2\xe8\xe4\xe8\xec\xfb\xec\xe8\x20\xe2\xf1\xe5\x20\xee\xe1\xfa\xe5\xea\xf2\xfb\x20\xed\xe0\x20\xe8"
        "\xe3\xf0\xee\xe2\xee\xe9\x20\xea\xe0\xf0\xf2\xe5\x2e"),
    /* {Опознать героев}\n\nПозволяет получить подробную информацию о героях противника. */ DATA_COMPGEN(0x0050f230, gSpellDescIdentifyHeroAllowsTheCasterToViewDetailed54,
        "\x7b\xce\xef\xee\xe7\xed\xe0\xf2\xfc\x20\xe3\xe5\xf0\xee\xe5\xe2\x7d\x0a\x0a\xcf\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xef\xee\xeb\xf3\xf7\xe8\xf2\xfc\x20\xef\xee\xe4\xf0\xee\xe1\xed\xf3\xfe\x20"
        "\xe8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xfe\x20\xee\x20\xe3\xe5\xf0\xee\xff\xf5\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e"),
    /* {Призвать корабль}\n\nПеремещает ваш ближайший незанятый корабль в ближайшую к вам точку побережья. Вашим считается корабль, который вы только что построили, либо тот, на котором вы плавали последним. */ DATA_COMPGEN(0x0050f280, gSpellDescSummonBoatSummonsTheNearestUnoccupiedFriendlyBoat55,
        "\x7b\xcf\xf0\xe8\xe7\xe2\xe0\xf2\xfc\x20\xea\xee\xf0\xe0\xe1\xeb\xfc\x7d\x0a\x0a\xcf\xe5\xf0\xe5\xec\xe5\xf9\xe0\xe5\xf2\x20\xe2\xe0\xf8\x20\xe1\xeb\xe8\xe6\xe0\xe9\xf8\xe8\xe9\x20\xed\xe5\xe7"
        "\xe0\xed\xff\xf2\xfb\xe9\x20\xea\xee\xf0\xe0\xe1\xeb\xfc\x20\xe2\x20\xe1\xeb\xe8\xe6\xe0\xe9\xf8\xf3\xfe\x20\xea\x20\xe2\xe0\xec\x20\xf2\xee\xf7\xea\xf3\x20\xef\xee\xe1\xe5\xf0\xe5\xe6\xfc\xff"
        "\x2e\x20\xc2\xe0\xf8\xe8\xec\x20\xf1\xf7\xe8\xf2\xe0\xe5\xf2\xf1\xff\x20\xea\xee\xf0\xe0\xe1\xeb\xfc\x2c\x20\xea\xee\xf2\xee\xf0\xfb\xe9\x20\xe2\xfb\x20\xf2\xee\xeb\xfc\xea\xee\x20\xf7\xf2\xee"
        "\x20\xef\xee\xf1\xf2\xf0\xee\xe8\xeb\xe8\x2c\x20\xeb\xe8\xe1\xee\x20\xf2\xee\xf2\x2c\x20\xed\xe0\x20\xea\xee\xf2\xee\xf0\xee\xec\x20\xe2\xfb\x20\xef\xeb\xe0\xe2\xe0\xeb\xe8\x20\xef\xee\xf1\xeb"
        "\xe5\xe4\xed\xe8\xec\x2e"),
    /* {Портал}\n\nПереносит героя в расположенную поблизости точку на карте. */ DATA_COMPGEN(0x0050f348, gSpellDescDimensionDoorAllowsTheCasterToMagicallyTransport56,
        "\x7b\xcf\xee\xf0\xf2\xe0\xeb\x7d\x0a\x0a\xcf\xe5\xf0\xe5\xed\xee\xf1\xe8\xf2\x20\xe3\xe5\xf0\xee\xff\x20\xe2\x20\xf0\xe0\xf1\xef\xee\xeb\xee\xe6\xe5\xed\xed\xf3\xfe\x20\xef\xee\xe1\xeb\xe8\xe7"
        "\xee\xf1\xf2\xe8\x20\xf2\xee\xf7\xea\xf3\x20\xed\xe0\x20\xea\xe0\xf0\xf2\xe5\x2e"),
    /* {Врата города}\n\nПереносит героя в ближайший принадлежащий игроку город или замок. */ DATA_COMPGEN(0x0050f390, gSpellDescTownGateReturnsTheCasterToAnyTown57,
        "\x7b\xc2\xf0\xe0\xf2\xe0\x20\xe3\xee\xf0\xee\xe4\xe0\x7d\x0a\x0a\xcf\xe5\xf0\xe5\xed\xee\xf1\xe8\xf2\x20\xe3\xe5\xf0\xee\xff\x20\xe2\x20\xe1\xeb\xe8\xe6\xe0\xe9\xf8\xe8\xe9\x20\xef\xf0\xe8\xed"
        "\xe0\xe4\xeb\xe5\xe6\xe0\xf9\xe8\xe9\x20\xe8\xe3\xf0\xee\xea\xf3\x20\xe3\xee\xf0\xee\xe4\x20\xe8\xeb\xe8\x20\xe7\xe0\xec\xee\xea\x2e"),
    /* {Портал города}\n\nПереносит героя в принадлежащий игроку город или замок по его выбору. */ DATA_COMPGEN(0x0050f3e4, gSpellDescTownPortalReturnsTheHeroToTheTown58,
        "\x7b\xcf\xee\xf0\xf2\xe0\xeb\x20\xe3\xee\xf0\xee\xe4\xe0\x7d\x0a\x0a\xcf\xe5\xf0\xe5\xed\xee\xf1\xe8\xf2\x20\xe3\xe5\xf0\xee\xff\x20\xe2\x20\xef\xf0\xe8\xed\xe0\xe4\xeb\xe5\xe6\xe0\xf9\xe8\xe9"
        "\x20\xe8\xe3\xf0\xee\xea\xf3\x20\xe3\xee\xf0\xee\xe4\x20\xe8\xeb\xe8\x20\xe7\xe0\xec\xee\xea\x20\xef\xee\x20\xe5\xe3\xee\x20\xe2\xfb\xe1\xee\xf0\xf3\x2e"),
    /* {Виденье}\n\nЭто заклинание позволяет предсказать вероятный исход встречи с нейтральной армией. */ DATA_COMPGEN(0x0050f43c, gSpellDescVisionsVisionsPredictsTheLikelyOutcomeOfAn59,
        "\x7b\xc2\xe8\xe4\xe5\xed\xfc\xe5\x7d\x0a\x0a\xdd\xf2\xee\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5\x20\xef\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xef\xf0\xe5\xe4\xf1\xea\xe0\xe7\xe0\xf2\xfc\x20"
        "\xe2\xe5\xf0\xee\xff\xf2\xed\xfb\xe9\x20\xe8\xf1\xf5\xee\xe4\x20\xe2\xf1\xf2\xf0\xe5\xf7\xe8\x20\xf1\x20\xed\xe5\xe9\xf2\xf0\xe0\xeb\xfc\xed\xee\xe9\x20\xe0\xf0\xec\xe8\xe5\xe9\x2e"),
    /* {Запустение}\n\nНаводняет принадлежащую игроку шахту призраками, после чего она перестает производить ресурсы. (Не доставайся же ты никому!) */ DATA_COMPGEN(0x0050f49c, gSpellDescHauntHauntsAMineYouControlWithGhosts60,
        "\x7b\xc7\xe0\xef\xf3\xf1\xf2\xe5\xed\xe8\xe5\x7d\x0a\x0a\xcd\xe0\xe2\xee\xe4\xed\xff\xe5\xf2\x20\xef\xf0\xe8\xed\xe0\xe4\xeb\xe5\xe6\xe0\xf9\xf3\xfe\x20\xe8\xe3\xf0\xee\xea\xf3\x20\xf8\xe0\xf5"
        "\xf2\xf3\x20\xef\xf0\xe8\xe7\xf0\xe0\xea\xe0\xec\xe8\x2c\x20\xef\xee\xf1\xeb\xe5\x20\xf7\xe5\xe3\xee\x20\xee\xed\xe0\x20\xef\xe5\xf0\xe5\xf1\xf2\xe0\xe5\xf2\x20\xef\xf0\xee\xe8\xe7\xe2\xee\xe4"
        "\xe8\xf2\xfc\x20\xf0\xe5\xf1\xf3\xf0\xf1\xfb\x2e\x20\x28\xcd\xe5\x20\xe4\xee\xf1\xf2\xe0\xe2\xe0\xe9\xf1\xff\x20\xe6\xe5\x20\xf2\xfb\x20\xed\xe8\xea\xee\xec\xf3\x21\x29"),
    /* {Стража земли}\n\nОтряд земных элементалов охраняет шахту от нападения армий противника. */ DATA_COMPGEN(0x0050f528, gSpellDescSetEarthGuardianSetsEarthElementalsToGuard61,
        "\x7b\xd1\xf2\xf0\xe0\xe6\xe0\x20\xe7\xe5\xec\xeb\xe8\x7d\x0a\x0a\xce\xf2\xf0\xff\xe4\x20\xe7\xe5\xec\xed\xfb\xf5\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb\xee\xe2\x20\xee\xf5\xf0\xe0\xed\xff\xe5"
        "\xf2\x20\xf8\xe0\xf5\xf2\xf3\x20\xee\xf2\x20\xed\xe0\xef\xe0\xe4\xe5\xed\xe8\xff\x20\xe0\xf0\xec\xe8\xe9\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e"),
    /* {Стража воздуха}\n\nОтряд воздушных элементалов охраняет шахту от нападения армий противника. */ DATA_COMPGEN(0x0050f580, gSpellDescSetAirGuardianSetsAirElementalsToGuard62,
        "\x7b\xd1\xf2\xf0\xe0\xe6\xe0\x20\xe2\xee\xe7\xe4\xf3\xf5\xe0\x7d\x0a\x0a\xce\xf2\xf0\xff\xe4\x20\xe2\xee\xe7\xe4\xf3\xf8\xed\xfb\xf5\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb\xee\xe2\x20\xee\xf5"
        "\xf0\xe0\xed\xff\xe5\xf2\x20\xf8\xe0\xf5\xf2\xf3\x20\xee\xf2\x20\xed\xe0\xef\xe0\xe4\xe5\xed\xe8\xff\x20\xe0\xf0\xec\xe8\xe9\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e"),
    /* {Стража огня}\n\nОтряд огненных элементалов охраняет шахту от нападения армий противника. */ DATA_COMPGEN(0x0050f5dc, gSpellDescSetFireGuardianSetsFireElementalsToGuard63,
        "\x7b\xd1\xf2\xf0\xe0\xe6\xe0\x20\xee\xe3\xed\xff\x7d\x0a\x0a\xce\xf2\xf0\xff\xe4\x20\xee\xe3\xed\xe5\xed\xed\xfb\xf5\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb\xee\xe2\x20\xee\xf5\xf0\xe0\xed\xff"
        "\xe5\xf2\x20\xf8\xe0\xf5\xf2\xf3\x20\xee\xf2\x20\xed\xe0\xef\xe0\xe4\xe5\xed\xe8\xff\x20\xe0\xf0\xec\xe8\xe9\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e"),
    /* {Стража воды}\n\nОтряд водных элементалов охраняет шахту от нападения армий противника. */ DATA_COMPGEN(0x0050f634, gSpellDescSetWaterGuardianSetsWaterElementalsToGuard64,
        "\x7b\xd1\xf2\xf0\xe0\xe6\xe0\x20\xe2\xee\xe4\xfb\x7d\x0a\x0a\xce\xf2\xf0\xff\xe4\x20\xe2\xee\xe4\xed\xfb\xf5\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb\xee\xe2\x20\xee\xf5\xf0\xe0\xed\xff\xe5\xf2"
        "\x20\xf8\xe0\xf5\xf2\xf3\x20\xee\xf2\x20\xed\xe0\xef\xe0\xe4\xe5\xed\xe8\xff\x20\xe0\xf0\xec\xe8\xe9\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e")
};
DATA(0x004fec04) char* gSpellNames[KB_SPELL_TEXT_COUNT] = {
    /* Огненный шар */ DATA_COMPGEN(0x0050f68c, gSpellNamesFireball0, "\xce\xe3\xed\xe5\xed\xed\xfb\xe9\x20\xf8\xe0\xf0"),
    /* Огненный взрыв */ DATA_COMPGEN(0x0050f69c, gSpellNamesFireblast1, "\xce\xe3\xed\xe5\xed\xed\xfb\xe9\x20\xe2\xe7\xf0\xfb\xe2"),
    /* Молния */ DATA_COMPGEN(0x0050f6ac, gSpellNamesLightningBolt2, "\xcc\xee\xeb\xed\xe8\xff"),
    /* Цепь молний */ DATA_COMPGEN(0x0050f6b4, gSpellNamesChainLightning3, "\xd6\xe5\xef\xfc\x20\xec\xee\xeb\xed\xe8\xe9"),
    /* Телепорт */ DATA_COMPGEN(0x0050f6c0, gSpellNamesTeleport4, "\xd2\xe5\xeb\xe5\xef\xee\xf0\xf2"),
    /* Лечение */ DATA_COMPGEN(0x0050f6cc, gSpellNamesCure5, "\xcb\xe5\xf7\xe5\xed\xe8\xe5"),
    /* Общее лечение */ DATA_COMPGEN(0x0050f6d4, gSpellNamesMassCure6, "\xce\xe1\xf9\xe5\xe5\x20\xeb\xe5\xf7\xe5\xed\xe8\xe5"),
    /* Воскрешение */ DATA_COMPGEN(0x0050f6e4, gSpellNamesResurrect7, "\xc2\xee\xf1\xea\xf0\xe5\xf8\xe5\xed\xe8\xe5"),
    /* Истинное воскрешение */ DATA_COMPGEN(0x0050f6f0, gSpellNamesResurrectTrue8, "\xc8\xf1\xf2\xe8\xed\xed\xee\xe5\x20\xe2\xee\xf1\xea\xf0\xe5\xf8\xe5\xed\xe8\xe5"),
    /* Ускорение */ DATA_COMPGEN(0x0050f708, gSpellNamesHaste9, "\xd3\xf1\xea\xee\xf0\xe5\xed\xe8\xe5"),
    /* Общее ускорение */ DATA_COMPGEN(0x0050f714, gSpellNamesMassHaste10, "\xce\xe1\xf9\xe5\xe5\x20\xf3\xf1\xea\xee\xf0\xe5\xed\xe8\xe5"),
    /* Замедление */ DATA_COMPGEN(0x0050f724, gSpellNamesSlow11, "\xc7\xe0\xec\xe5\xe4\xeb\xe5\xed\xe8\xe5"),
    /* Общее замедление */ DATA_COMPGEN(0x0050f730, gSpellNamesMassSlow12, "\xce\xe1\xf9\xe5\xe5\x20\xe7\xe0\xec\xe5\xe4\xeb\xe5\xed\xe8\xe5"),
    /* Ослепление */ DATA_COMPGEN(0x0050f744, gSpellNamesBlind13, "\xce\xf1\xeb\xe5\xef\xeb\xe5\xed\xe8\xe5"),
    /* Благословение */ DATA_COMPGEN(0x0050f750, gSpellNamesBless14, "\xc1\xeb\xe0\xe3\xee\xf1\xeb\xee\xe2\xe5\xed\xe8\xe5"),
    /* Общее благословение */ DATA_COMPGEN(0x0050f760, gSpellNamesMassBless15, "\xce\xe1\xf9\xe5\xe5\x20\xe1\xeb\xe0\xe3\xee\xf1\xeb\xee\xe2\xe5\xed\xe8\xe5"),
    /* Каменная кожа */ DATA_COMPGEN(0x0050f774, gSpellNamesStoneskin16, "\xca\xe0\xec\xe5\xed\xed\xe0\xff\x20\xea\xee\xe6\xe0"),
    /* Стальная кожа */ DATA_COMPGEN(0x0050f784, gSpellNamesSteelskin17, "\xd1\xf2\xe0\xeb\xfc\xed\xe0\xff\x20\xea\xee\xe6\xe0"),
    /* Проклятие */ DATA_COMPGEN(0x0050f794, gSpellNamesCurse18, "\xcf\xf0\xee\xea\xeb\xff\xf2\xe8\xe5"),
    /* Общее проклятие */ DATA_COMPGEN(0x0050f7a0, gSpellNamesMassCurse19, "\xce\xe1\xf9\xe5\xe5\x20\xef\xf0\xee\xea\xeb\xff\xf2\xe8\xe5"),
    /* Святое слово */ DATA_COMPGEN(0x0050f7b0, gSpellNamesHolyWord20, "\xd1\xe2\xff\xf2\xee\xe5\x20\xf1\xeb\xee\xe2\xee"),
    /* Святой глас */ DATA_COMPGEN(0x0050f7c0, gSpellNamesHolyShout21, "\xd1\xe2\xff\xf2\xee\xe9\x20\xe3\xeb\xe0\xf1"),
    /* Антимагия */ DATA_COMPGEN(0x0050f7cc, gSpellNamesAntiMagic22, "\xc0\xed\xf2\xe8\xec\xe0\xe3\xe8\xff"),
    /* Снятие чар */ DATA_COMPGEN(0x0050f7d8, gSpellNamesDispelMagic23, "\xd1\xed\xff\xf2\xe8\xe5\x20\xf7\xe0\xf0"),
    /* Общее снятие чар */ DATA_COMPGEN(0x0050f7e4, gSpellNamesMassDispel24, "\xce\xe1\xf9\xe5\xe5\x20\xf1\xed\xff\xf2\xe8\xe5\x20\xf7\xe0\xf0"),
    /* Волшебная стрела */ DATA_COMPGEN(0x0050f7f8, gSpellNamesMagicArrow25, "\xc2\xee\xeb\xf8\xe5\xe1\xed\xe0\xff\x20\xf1\xf2\xf0\xe5\xeb\xe0"),
    /* Берсерк */ DATA_COMPGEN(0x0050f80c, gSpellNamesBerzerker26, "\xc1\xe5\xf0\xf1\xe5\xf0\xea"),
    /* Армагеддон */ DATA_COMPGEN(0x0050f814, gSpellNamesArmageddon27, "\xc0\xf0\xec\xe0\xe3\xe5\xe4\xe4\xee\xed"),
    /* Буря стихий */ DATA_COMPGEN(0x0050f820, gSpellNamesElementalStorm28, "\xc1\xf3\xf0\xff\x20\xf1\xf2\xe8\xf5\xe8\xe9"),
    /* Звездопад */ DATA_COMPGEN(0x0050f82c, gSpellNamesMeteorShower29, "\xc7\xe2\xe5\xe7\xe4\xee\xef\xe0\xe4"),
    /* Паралич */ DATA_COMPGEN(0x0050f838, gSpellNamesParalyze30, "\xcf\xe0\xf0\xe0\xeb\xe8\xf7"),
    /* Гипноз */ DATA_COMPGEN(0x0050f840, gSpellNamesHypnotize31, "\xc3\xe8\xef\xed\xee\xe7"),
    /* Хладный луч */ DATA_COMPGEN(0x0050f848, gSpellNamesColdRay32, "\xd5\xeb\xe0\xe4\xed\xfb\xe9\x20\xeb\xf3\xf7"),
    /* Кольцо стужи */ DATA_COMPGEN(0x0050f854, gSpellNamesColdRing33, "\xca\xee\xeb\xfc\xf6\xee\x20\xf1\xf2\xf3\xe6\xe8"),
    /* Разрушительный луч */ DATA_COMPGEN(0x0050f864, gSpellNamesDisruptingRay34, "\xd0\xe0\xe7\xf0\xf3\xf8\xe8\xf2\xe5\xeb\xfc\xed\xfb\xe9\x20\xeb\xf3\xf7"),
    /* Дрожь смерти */ DATA_COMPGEN(0x0050f878, gSpellNamesDeathRipple35, "\xc4\xf0\xee\xe6\xfc\x20\xf1\xec\xe5\xf0\xf2\xe8"),
    /* Волна смерти */ DATA_COMPGEN(0x0050f888, gSpellNamesDeathWave36, "\xc2\xee\xeb\xed\xe0\x20\xf1\xec\xe5\xf0\xf2\xe8"),
    /* Убийца драконов */ DATA_COMPGEN(0x0050f898, gSpellNamesDragonSlayer37, "\xd3\xe1\xe8\xe9\xf6\xe0\x20\xe4\xf0\xe0\xea\xee\xed\xee\xe2"),
    /* Жажда крови */ DATA_COMPGEN(0x0050f8a8, gSpellNamesBloodLust38, "\xc6\xe0\xe6\xe4\xe0\x20\xea\xf0\xee\xe2\xe8"),
    /* Поднять мертвых */ DATA_COMPGEN(0x0050f8b4, gSpellNamesAnimateDead39, "\xcf\xee\xe4\xed\xff\xf2\xfc\x20\xec\xe5\xf0\xf2\xe2\xfb\xf5"),
    /* Фантом */ DATA_COMPGEN(0x0050f8c4, gSpellNamesMirrorImage40, "\xd4\xe0\xed\xf2\xee\xec"),
    /* Щит */ DATA_COMPGEN(0x0050f8cc, gSpellNamesShield41, "\xd9\xe8\xf2"),
    /* Общий щит */ DATA_COMPGEN(0x0050f8d0, gSpellNamesMassShield42, "\xce\xe1\xf9\xe8\xe9\x20\xf9\xe8\xf2"),
    /* Земной элементал */ DATA_COMPGEN(0x0050f8dc, gSpellNamesSummonEarthElemental43, "\xc7\xe5\xec\xed\xee\xe9\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb"),
    /* Воздушный элементал */ DATA_COMPGEN(0x0050f8f0, gSpellNamesSummonAirElemental44, "\xc2\xee\xe7\xe4\xf3\xf8\xed\xfb\xe9\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb"),
    /* Огненный элементал */ DATA_COMPGEN(0x0050f904, gSpellNamesSummonFireElemental45, "\xce\xe3\xed\xe5\xed\xed\xfb\xe9\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb"),
    /* Водный элементал */ DATA_COMPGEN(0x0050f918, gSpellNamesSummonWaterElemental46, "\xc2\xee\xe4\xed\xfb\xe9\x20\xfd\xeb\xe5\xec\xe5\xed\xf2\xe0\xeb"),
    /* Землетрясение */ DATA_COMPGEN(0x0050f92c, gSpellNamesEarthquake47, "\xc7\xe5\xec\xeb\xe5\xf2\xf0\xff\xf1\xe5\xed\xe8\xe5"),
    /* Показать шахты */ DATA_COMPGEN(0x0050f93c, gSpellNamesViewMines48, "\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xf8\xe0\xf5\xf2\xfb"),
    /* Показать ресурсы */ DATA_COMPGEN(0x0050f94c, gSpellNamesViewResources49, "\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xf0\xe5\xf1\xf3\xf0\xf1\xfb"),
    /* Показать артефакты */ DATA_COMPGEN(0x0050f960, gSpellNamesViewArtifacts50, "\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2\xfb"),
    /* Показать города */ DATA_COMPGEN(0x0050f974, gSpellNamesViewTowns51, "\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xe3\xee\xf0\xee\xe4\xe0"),
    /* Показать героев */ DATA_COMPGEN(0x0050f984, gSpellNamesViewHeroes52, "\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xe3\xe5\xf0\xee\xe5\xe2"),
    /* Показать все */ DATA_COMPGEN(0x0050f994, gSpellNamesViewAll53, "\xcf\xee\xea\xe0\xe7\xe0\xf2\xfc\x20\xe2\xf1\xe5"),
    /* Опознать героев */ DATA_COMPGEN(0x0050f9a4, gSpellNamesIdentifyHero54, "\xce\xef\xee\xe7\xed\xe0\xf2\xfc\x20\xe3\xe5\xf0\xee\xe5\xe2"),
    /* Призвать корабль */ DATA_COMPGEN(0x0050f9b4, gSpellNamesSummonBoat55, "\xcf\xf0\xe8\xe7\xe2\xe0\xf2\xfc\x20\xea\xee\xf0\xe0\xe1\xeb\xfc"),
    /* Портал */ DATA_COMPGEN(0x0050f9c8, gSpellNamesDimensionDoor56, "\xcf\xee\xf0\xf2\xe0\xeb"),
    /* Врата города */ DATA_COMPGEN(0x0050f9d0, gSpellNamesTownGate57, "\xc2\xf0\xe0\xf2\xe0\x20\xe3\xee\xf0\xee\xe4\xe0"),
    /* Портал города */ DATA_COMPGEN(0x0050f9e0, gSpellNamesTownPortal58, "\xcf\xee\xf0\xf2\xe0\xeb\x20\xe3\xee\xf0\xee\xe4\xe0"),
    /* Виденье */ DATA_COMPGEN(0x0050f9f0, gSpellNamesVisions59, "\xc2\xe8\xe4\xe5\xed\xfc\xe5"),
    /* Запустение */ DATA_COMPGEN(0x0050f9f8, gSpellNamesHaunt60, "\xc7\xe0\xef\xf3\xf1\xf2\xe5\xed\xe8\xe5"),
    /* Страж земли */ DATA_COMPGEN(0x0050fa04, gSpellNamesSetEarthGuardian61, "\xd1\xf2\xf0\xe0\xe6\x20\xe7\xe5\xec\xeb\xe8"),
    /* Страж воздуха */ DATA_COMPGEN(0x0050fa10, gSpellNamesSetAirGuardian62, "\xd1\xf2\xf0\xe0\xe6\x20\xe2\xee\xe7\xe4\xf3\xf5\xe0"),
    /* Страж огня */ DATA_COMPGEN(0x0050fa20, gSpellNamesSetFireGuardian63, "\xd1\xf2\xf0\xe0\xe6\x20\xee\xe3\xed\xff"),
    /* Страж воды */ DATA_COMPGEN(0x0050fa2c, gSpellNamesSetWaterGuardian64, "\xd1\xf2\xf0\xe0\xe6\x20\xe2\xee\xe4\xfb")
};
DATA(0x004fed08) char* gSecondarySkillLevels[KB_SECONDARY_SKILL_LEVEL_TEXT_COUNT] =
    {/* 1 ступени */ DATA_COMPGEN(0x0050fa38, gSecondarySkillLevelsBasic0, "\x31\x20\xf1\xf2\xf3\xef\xe5\xed\xe8"), /* 2 ступени */ DATA_COMPGEN(0x0050fa44, gSecondarySkillLevelsAdvanced1, "\x32\x20\xf1\xf2\xf3\xef\xe5\xed\xe8"), /* 3 ступени */ DATA_COMPGEN(0x0050fa50, gSecondarySkillLevelsExpert2, "\x33\x20\xf1\xf2\xf3\xef\xe5\xed\xe8")};
DATA(0x004fed14) char* gSecondarySkills[KB_SECONDARY_SKILL_TEXT_COUNT] = {
    /* Следопыт */ DATA_COMPGEN(0x0050fa5c, gSecondarySkillsPathfinding0, "\xd1\xeb\xe5\xe4\xee\xef\xfb\xf2"),
    /* Стрелок */ DATA_COMPGEN(0x0050fa68, gSecondarySkillsArchery1, "\xd1\xf2\xf0\xe5\xeb\xee\xea"),
    /* Логистика */ DATA_COMPGEN(0x0050fa70, gSecondarySkillsLogistics2, "\xcb\xee\xe3\xe8\xf1\xf2\xe8\xea\xe0"),
    /* Разведка */ DATA_COMPGEN(0x0050fa7c, gSecondarySkillsScouting3, "\xd0\xe0\xe7\xe2\xe5\xe4\xea\xe0"),
    /* Дипломатия */ DATA_COMPGEN(0x0050fa88, gSecondarySkillsDiplomacy4, "\xc4\xe8\xef\xeb\xee\xec\xe0\xf2\xe8\xff"),
    /* Навигация */ DATA_COMPGEN(0x0050fa94, gSecondarySkillsNavigation5, "\xcd\xe0\xe2\xe8\xe3\xe0\xf6\xe8\xff"),
    /* Лидерство */ DATA_COMPGEN(0x0050faa0, gSecondarySkillsLeadership6, "\xcb\xe8\xe4\xe5\xf0\xf1\xf2\xe2\xee"),
    /* Мудрость */ DATA_COMPGEN(0x0050faac, gSecondarySkillsWisdom7, "\xcc\xf3\xe4\xf0\xee\xf1\xf2\xfc"),
    /* Мистицизм */ DATA_COMPGEN(0x0050fab8, gSecondarySkillsMysticism8, "\xcc\xe8\xf1\xf2\xe8\xf6\xe8\xe7\xec"),
    /* Удача */ DATA_COMPGEN(0x0050fac4, gSecondarySkillsLuck9, "\xd3\xe4\xe0\xf7\xe0"),
    /* Баллистика */ DATA_COMPGEN(0x0050facc, gSecondarySkillsBallistics10, "\xc1\xe0\xeb\xeb\xe8\xf1\xf2\xe8\xea\xe0"),
    /* Орлиный взор */ DATA_COMPGEN(0x0050fad8, gSecondarySkillsEagleEye11, "\xce\xf0\xeb\xe8\xed\xfb\xe9\x20\xe2\xe7\xee\xf0"),
    /* Некромантия */ DATA_COMPGEN(0x0050fae8, gSecondarySkillsNecromancy12, "\xcd\xe5\xea\xf0\xee\xec\xe0\xed\xf2\xe8\xff"),
    /* Казначей */ DATA_COMPGEN(0x0050faf4, gSecondarySkillsEstates13, "\xca\xe0\xe7\xed\xe0\xf7\xe5\xe9")
};
DATA(0x004fed4c) char* gNeutralBuildingNames[KB_NEUTRAL_BUILDING_TEXT_COUNT] = {
    /* Гильдия магов */ DATA_COMPGEN(0x0050fb00, gNeutralBuildingNamesMageGuild0, "\xc3\xe8\xeb\xfc\xe4\xe8\xff\x20\xec\xe0\xe3\xee\xe2"),
    /* Гильдия воров */ DATA_COMPGEN(0x0050fb10, gNeutralBuildingNamesThievesGuild1, "\xc3\xe8\xeb\xfc\xe4\xe8\xff\x20\xe2\xee\xf0\xee\xe2"),
    /* Таверна */ DATA_COMPGEN(0x0050fb20, gNeutralBuildingNamesTavern2, "\xd2\xe0\xe2\xe5\xf0\xed\xe0"),
    /* Верфь */ DATA_COMPGEN(0x0050fb28, gNeutralBuildingNamesShipyard3, "\xc2\xe5\xf0\xf4\xfc"),
    /* Колодец */ DATA_COMPGEN(0x0050fb30, gNeutralBuildingNamesWell4, "\xca\xee\xeb\xee\xe4\xe5\xf6"),
    /* Шатер */ DATA_COMPGEN(0x0050fb38, gNeutralBuildingNamesTent5, "\xd8\xe0\xf2\xe5\xf0"),
    /* Замок */ DATA_COMPGEN(0x0050fb40, gNeutralBuildingNamesCastle6, "\xc7\xe0\xec\xee\xea"),
    /* Статуя */ DATA_COMPGEN(0x0050fb48, gNeutralBuildingNamesStatue7, "\xd1\xf2\xe0\xf2\xf3\xff"),
    /* Левая башня */ DATA_COMPGEN(0x0050fb50, gNeutralBuildingNamesLeftTurret8, "\xcb\xe5\xe2\xe0\xff\x20\xe1\xe0\xf8\xed\xff"),
    /* Правая башня */ DATA_COMPGEN(0x0050fb5c, gNeutralBuildingNamesRightTurret9, "\xcf\xf0\xe0\xe2\xe0\xff\x20\xe1\xe0\xf8\xed\xff"),
    /* Рынок */ DATA_COMPGEN(0x0050fb6c, gNeutralBuildingNamesMarketplace10, "\xd0\xfb\xed\xee\xea"),
    /*  */ DATA_COMPGEN(0x005267ec, gNeutralBuildingNamesEmpty11, ""),
    /* Ров */ DATA_COMPGEN(0x0050fb74, gNeutralBuildingNamesMoat12, "\xd0\xee\xe2"),
    /*  */ DATA_COMPGEN(0x005267f0, gNeutralBuildingNamesEmpty13, ""),
    /* Док с кораблем */ DATA_COMPGEN(0x0050fb78, gNeutralBuildingNamesDockWithBoat14, "\xc4\xee\xea\x20\xf1\x20\xea\xee\xf0\xe0\xe1\xeb\xe5\xec"),
    /* Дом капитана */ DATA_COMPGEN(0x0050fb88, gNeutralBuildingNamesCaptainSQuarters15, "\xc4\xee\xec\x20\xea\xe0\xef\xe8\xf2\xe0\xed\xe0"),
    /*  */ DATA_COMPGEN(0x005267f4, gNeutralBuildingNamesEmpty16, ""),
    /*  */ DATA_COMPGEN(0x005267f8, gNeutralBuildingNamesEmpty17, ""),
    /*  */ DATA_COMPGEN(0x005267fc, gNeutralBuildingNamesEmpty18, "")
};
DATA(0x004fed98) char* gWellExtraNames[KB_WELL_EXTRA_NAME_COUNT] = {
    /* Ферма */ DATA_COMPGEN(0x0050fb98, gWellExtraNamesFarm0, "\xd4\xe5\xf0\xec\xe0"),
    /* Свалка истории */ DATA_COMPGEN(0x0050fba0, gWellExtraNamesGarbageHeap1, "\xd1\xe2\xe0\xeb\xea\xe0\x20\xe8\xf1\xf2\xee\xf0\xe8\xe8"),
    /* Хрустальный сад */ DATA_COMPGEN(0x0050fbb0, gWellExtraNamesCrystalGarden2, "\xd5\xf0\xf3\xf1\xf2\xe0\xeb\xfc\xed\xfb\xe9\x20\xf1\xe0\xe4"),
    /* Водопад */ DATA_COMPGEN(0x0050fbc0, gWellExtraNamesWaterfall3, "\xc2\xee\xe4\xee\xef\xe0\xe4"),
    /* Фруктовый сад */ DATA_COMPGEN(0x0050fbc8, gWellExtraNamesOrchard4, "\xd4\xf0\xf3\xea\xf2\xee\xe2\xfb\xe9\x20\xf1\xe0\xe4"),
    /* Груда черепов */ DATA_COMPGEN(0x0050fbd8, gWellExtraNamesSkullPile5, "\xc3\xf0\xf3\xe4\xe0\x20\xf7\xe5\xf0\xe5\xef\xee\xe2"),
    /* Прирост воинов 1 ур. */ DATA_COMPGEN(0x0050fbe8, gWellExtraNames1stLvlGrowth6, "\xcf\xf0\xe8\xf0\xee\xf1\xf2\x20\xe2\xee\xe8\xed\xee\xe2\x20\x31\x20\xf3\xf0\x2e")
};
DATA(0x004fedb4) char* gSpecialBuildingNames[KB_SPECIAL_BUILDING_NAME_COUNT] =
    {/* Укрепления */ DATA_COMPGEN(0x0050fc00, gSpecialBuildingNamesFortifications0, "\xd3\xea\xf0\xe5\xef\xeb\xe5\xed\xe8\xff"), /* Колизей */ DATA_COMPGEN(0x0050fc0c, gSpecialBuildingNamesColiseum1, "\xca\xee\xeb\xe8\xe7\xe5\xe9"), /* Радуга */ DATA_COMPGEN(0x0050fc14, gSpecialBuildingNamesRainbow2, "\xd0\xe0\xe4\xf3\xe3\xe0"), /* Подземелье */ DATA_COMPGEN(0x0050fc1c, gSpecialBuildingNamesDungeon3, "\xcf\xee\xe4\xe7\xe5\xec\xe5\xeb\xfc\xe5"), /* Библиотека */ DATA_COMPGEN(0x0050fc28, gSpecialBuildingNamesLibrary4, "\xc1\xe8\xe1\xeb\xe8\xee\xf2\xe5\xea\xe0"), /* Шторм */ DATA_COMPGEN(0x0050fc34, gSpecialBuildingNamesStorm5, "\xd8\xf2\xee\xf0\xec"), /* Специальная */ DATA_COMPGEN(0x0050fc3c, gSpecialBuildingNamesSpecial6, "\xd1\xef\xe5\xf6\xe8\xe0\xeb\xfc\xed\xe0\xff")};
DATA(0x004fedd0) char* gDwellingNames[IDX(FACTION_COUNT)][KB_DWELLING_TYPE_COUNT] = {
    {/* Мазанка */ DATA_COMPGEN(0x0050fc48, gDwellingNamesThatchedHut0, "\xcc\xe0\xe7\xe0\xed\xea\xe0"),
     /* Стрельбище */ DATA_COMPGEN(0x0050fc50, gDwellingNamesArcheryRange1, "\xd1\xf2\xf0\xe5\xeb\xfc\xe1\xe8\xf9\xe5"),
     /* Кузница */ DATA_COMPGEN(0x0050fc5c, gDwellingNamesBlacksmith2, "\xca\xf3\xe7\xed\xe8\xf6\xe0"),
     /* Оружейная */ DATA_COMPGEN(0x0050fc64, gDwellingNamesArmory3, "\xce\xf0\xf3\xe6\xe5\xe9\xed\xe0\xff"),
     /* Ристалище */ DATA_COMPGEN(0x0050fc70, gDwellingNamesJoustingArena4, "\xd0\xe8\xf1\xf2\xe0\xeb\xe8\xf9\xe5"),
     /* Собор */ DATA_COMPGEN(0x0050fc7c, gDwellingNamesCathedral5, "\xd1\xee\xe1\xee\xf0"),
     /* Полигон */ DATA_COMPGEN(0x0050fc84, gDwellingNamesUpgArcheryRange6, "\xcf\xee\xeb\xe8\xe3\xee\xed"),
     /* Ковальня */ DATA_COMPGEN(0x0050fc8c, gDwellingNamesUpgBlacksmith7, "\xca\xee\xe2\xe0\xeb\xfc\xed\xff"),
     /* Арсенал */ DATA_COMPGEN(0x0050fc98, gDwellingNamesUpgArmory8, "\xc0\xf0\xf1\xe5\xed\xe0\xeb"),
     /* Арена */ DATA_COMPGEN(0x0050fca0, gDwellingNamesUpgJoustingArena9, "\xc0\xf0\xe5\xed\xe0"),
     /* Храм */ DATA_COMPGEN(0x0050fca8, gDwellingNamesUpgCathedral10, "\xd5\xf0\xe0\xec"),
     /*  */ DATA_COMPGEN(0x00526800, gDwellingNamesEmpty11, "")},
    {/* Хижина */ DATA_COMPGEN(0x0050fcb0, gDwellingNamesHut12, "\xd5\xe8\xe6\xe8\xed\xe0"),
     /* Халупа */ DATA_COMPGEN(0x0050fcb8, gDwellingNamesStickHut13, "\xd5\xe0\xeb\xf3\xef\xe0"),
     /* Логово */ DATA_COMPGEN(0x0050fcc0, gDwellingNamesDen14, "\xcb\xee\xe3\xee\xe2\xee"),
     /* Дом огров */ DATA_COMPGEN(0x0050fcc8, gDwellingNamesAdobe15, "\xc4\xee\xec\x20\xee\xe3\xf0\xee\xe2"),
     /* Мост */ DATA_COMPGEN(0x0050fcd4, gDwellingNamesBridge16, "\xcc\xee\xf1\xf2"),
     /* Пирамида */ DATA_COMPGEN(0x0050fcdc, gDwellingNamesPyramid17, "\xcf\xe8\xf0\xe0\xec\xe8\xe4\xe0"),
     /* Хибара */ DATA_COMPGEN(0x0050fce8, gDwellingNamesUpgStickHut18, "\xd5\xe8\xe1\xe0\xf0\xe0"),
     /*  */ DATA_COMPGEN(0x00526804, gDwellingNamesEmpty19, ""),
     /* Логово огров */ DATA_COMPGEN(0x0050fcf0, gDwellingNamesUpgAdobe20, "\xcb\xee\xe3\xee\xe2\xee\x20\xee\xe3\xf0\xee\xe2"),
     /* Царь-мост */ DATA_COMPGEN(0x0050fd00, gDwellingNamesUpgBridge21, "\xd6\xe0\xf0\xfc\x2d\xec\xee\xf1\xf2"),
     /*  */ DATA_COMPGEN(0x00526808, gDwellingNamesEmpty22, ""),
     /*  */ DATA_COMPGEN(0x0052680c, gDwellingNamesEmpty23, "")},
    {/* Древо-дом */ DATA_COMPGEN(0x0050fd0c, gDwellingNamesTreehouse24, "\xc4\xf0\xe5\xe2\xee\x2d\xe4\xee\xec"),
     /* Избушка */ DATA_COMPGEN(0x0050fd18, gDwellingNamesCottage25, "\xc8\xe7\xe1\xf3\xf8\xea\xe0"),
     /* Стрельбище */ DATA_COMPGEN(0x0050fd20, gDwellingNamesArcheryRange26, "\xd1\xf2\xf0\xe5\xeb\xfc\xe1\xe8\xf9\xe5"),
     /* Стоунхендж */ DATA_COMPGEN(0x0050fd2c, gDwellingNamesStonehenge27, "\xd1\xf2\xee\xf3\xed\xf5\xe5\xed\xe4\xe6"),
     /* Загон */ DATA_COMPGEN(0x0050fd38, gDwellingNamesFencedMeadow28, "\xc7\xe0\xe3\xee\xed"),
     /* Алая башня */ DATA_COMPGEN(0x0050fd40, gDwellingNamesRedTower29, "\xc0\xeb\xe0\xff\x20\xe1\xe0\xf8\xed\xff"),
     /* Хоромы */ DATA_COMPGEN(0x0050fd4c, gDwellingNamesUpgCottage30, "\xd5\xee\xf0\xee\xec\xfb"),
     /* Полигон */ DATA_COMPGEN(0x0050fd54, gDwellingNamesUpgArcheryRange31, "\xcf\xee\xeb\xe8\xe3\xee\xed"),
     /* Менгиры */ DATA_COMPGEN(0x0050fd5c, gDwellingNamesUpgStonehenge32, "\xcc\xe5\xed\xe3\xe8\xf0\xfb"),
     /*  */ DATA_COMPGEN(0x00526810, gDwellingNamesEmpty33, ""),
     /*  */ DATA_COMPGEN(0x00526814, gDwellingNamesEmpty34, ""),
     /*  */ DATA_COMPGEN(0x00526818, gDwellingNamesEmpty35, "")},
    {/* Пещера */ DATA_COMPGEN(0x0050fd64, gDwellingNamesCave36, "\xcf\xe5\xf9\xe5\xf0\xe0"),
     /* Крипта */ DATA_COMPGEN(0x0050fd6c, gDwellingNamesCrypt37, "\xca\xf0\xe8\xef\xf2\xe0"),
     /* Гнездо */ DATA_COMPGEN(0x0050fd74, gDwellingNamesNest38, "\xc3\xed\xe5\xe7\xe4\xee"),
     /* Лабиринт */ DATA_COMPGEN(0x0050fd7c, gDwellingNamesMaze39, "\xcb\xe0\xe1\xe8\xf0\xe8\xed\xf2"),
     /* Болото */ DATA_COMPGEN(0x0050fd88, gDwellingNamesSwamp40, "\xc1\xee\xeb\xee\xf2\xee"),
     /* Зеленая башня */ DATA_COMPGEN(0x0050fd90, gDwellingNamesGreenTower41, "\xc7\xe5\xeb\xe5\xed\xe0\xff\x20\xe1\xe0\xf8\xed\xff"),
     /*  */ DATA_COMPGEN(0x0052681c, gDwellingNamesEmpty42, ""),
     /*  */ DATA_COMPGEN(0x00526820, gDwellingNamesEmpty43, ""),
     /* Большой лабиринт */ DATA_COMPGEN(0x0050fda0, gDwellingNamesUpgMaze44, "\xc1\xee\xeb\xfc\xf8\xee\xe9\x20\xeb\xe0\xe1\xe8\xf0\xe8\xed\xf2"),
     /*  */ DATA_COMPGEN(0x00526824, gDwellingNamesEmpty45, ""),
     /* Красная башня */ DATA_COMPGEN(0x0050fdb4, gDwellingNamesRedTower46, "\xca\xf0\xe0\xf1\xed\xe0\xff\x20\xe1\xe0\xf8\xed\xff"),
     /* Черная башня */ DATA_COMPGEN(0x0050fdc4, gDwellingNamesBlackTower47, "\xd7\xe5\xf0\xed\xe0\xff\x20\xe1\xe0\xf8\xed\xff")},
    {/* Нора */ DATA_COMPGEN(0x0050fdd4, gDwellingNamesHabitat48, "\xcd\xee\xf0\xe0"),
     /* Хлев */ DATA_COMPGEN(0x0050fddc, gDwellingNamesPen49, "\xd5\xeb\xe5\xe2"),
     /* Литейный цех */ DATA_COMPGEN(0x0050fde4, gDwellingNamesFoundry50, "\xcb\xe8\xf2\xe5\xe9\xed\xfb\xe9\x20\xf6\xe5\xf5"),
     /* Гнездовье */ DATA_COMPGEN(0x0050fdf4, gDwellingNamesCliffNest51, "\xc3\xed\xe5\xe7\xe4\xee\xe2\xfc\xe5"),
     /* Башня магов */ DATA_COMPGEN(0x0050fe00, gDwellingNamesIvoryTower52, "\xc1\xe0\xf8\xed\xff\x20\xec\xe0\xe3\xee\xe2"),
     /* Небесный замок */ DATA_COMPGEN(0x0050fe0c, gDwellingNamesCloudCastle53, "\xcd\xe5\xe1\xe5\xf1\xed\xfb\xe9\x20\xe7\xe0\xec\xee\xea"),
     /*  */ DATA_COMPGEN(0x00526828, gDwellingNamesEmpty54, ""),
     /* Фабрика */ DATA_COMPGEN(0x0050fe1c, gDwellingNamesUpgFoundry55, "\xd4\xe0\xe1\xf0\xe8\xea\xe0"),
     /*  */ DATA_COMPGEN(0x0052682c, gDwellingNamesEmpty56, ""),
     /* Обитель магов */ DATA_COMPGEN(0x0050fe24, gDwellingNamesUpgIvoryTower57, "\xce\xe1\xe8\xf2\xe5\xeb\xfc\x20\xec\xe0\xe3\xee\xe2"),
     /* Небесный чертог */ DATA_COMPGEN(0x0050fe34, gDwellingNamesUpgCloudCastle58, "\xcd\xe5\xe1\xe5\xf1\xed\xfb\xe9\x20\xf7\xe5\xf0\xf2\xee\xe3"),
     /*  */ DATA_COMPGEN(0x00526830, gDwellingNamesEmpty59, "")},
    {/* Могильник */ DATA_COMPGEN(0x0050fe44, gDwellingNamesExcavation60, "\xcc\xee\xe3\xe8\xeb\xfc\xed\xe8\xea"),
     /* Кладбище */ DATA_COMPGEN(0x0050fe50, gDwellingNamesGraveyard61, "\xca\xeb\xe0\xe4\xe1\xe8\xf9\xe5"),
     /* Пирамида */ DATA_COMPGEN(0x0050fe5c, gDwellingNamesPyramid62, "\xcf\xe8\xf0\xe0\xec\xe8\xe4\xe0"),
     /* Особняк */ DATA_COMPGEN(0x0050fe68, gDwellingNamesMansion63, "\xce\xf1\xee\xe1\xed\xff\xea"),
     /* Мавзолей */ DATA_COMPGEN(0x0050fe70, gDwellingNamesMausoleum64, "\xcc\xe0\xe2\xe7\xee\xeb\xe5\xe9"),
     /* Лаборатория */ DATA_COMPGEN(0x0050fe7c, gDwellingNamesLaboratory65, "\xcb\xe0\xe1\xee\xf0\xe0\xf2\xee\xf0\xe8\xff"),
     /* Погост */ DATA_COMPGEN(0x0050fe88, gDwellingNamesUpgGraveyard66, "\xcf\xee\xe3\xee\xf1\xf2"),
     /* Великая пирамида */ DATA_COMPGEN(0x0050fe90, gDwellingNamesUpgPyramid67, "\xc2\xe5\xeb\xe8\xea\xe0\xff\x20\xef\xe8\xf0\xe0\xec\xe8\xe4\xe0"),
     /* Цитадель */ DATA_COMPGEN(0x0050fea4, gDwellingNamesUpgMansion68, "\xd6\xe8\xf2\xe0\xe4\xe5\xeb\xfc"),
     /* Некрополь */ DATA_COMPGEN(0x0050feb0, gDwellingNamesUpgMausoleum69, "\xcd\xe5\xea\xf0\xee\xef\xee\xeb\xfc"),
     /*  */ DATA_COMPGEN(0x00526834, gDwellingNamesEmpty70, ""),
     /*  */ DATA_COMPGEN(0x00526838, gDwellingNamesEmpty71, "")}
};
DATA(0x004feef0) char* cSecSkillDesc[IDX(HERO_SKILL_COUNT)][SECONDARY_SKILL_VALUE_LEVEL_COUNT] = {
    {/* {Следопыт 1 ступени}\n\nУменьшает замедление при передвижении по пересеченной местности на 25 процентов. */ DATA_COMPGEN(0x0050febc, cSecSkillDescBasicPathfindingBasicPathfindingReducesTheMovementPenalty0,
        "\x7b\xd1\xeb\xe5\xe4\xee\xef\xfb\xf2\x20\x31\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xec\xe5\xed\xfc\xf8\xe0\xe5\xf2\x20\xe7\xe0\xec\xe5\xe4\xeb\xe5\xed\xe8\xe5\x20\xef\xf0\xe8\x20\xef"
        "\xe5\xf0\xe5\xe4\xe2\xe8\xe6\xe5\xed\xe8\xe8\x20\xef\xee\x20\xef\xe5\xf0\xe5\xf1\xe5\xf7\xe5\xed\xed\xee\xe9\x20\xec\xe5\xf1\xf2\xed\xee\xf1\xf2\xe8\x20\xed\xe0\x20\x32\x35\x20\xef\xf0\xee\xf6"
        "\xe5\xed\xf2\xee\xe2\x2e"),
     /* {Следопыт 2 ступени}\n\nУменьшает замедление при передвижении по пересеченной местности на 50 процентов. */ DATA_COMPGEN(0x0050ff24, cSecSkillDescAdvancedPathfindingAdvancedPathfindingReducesTheMovementPenalty1,
         "\x7b\xd1\xeb\xe5\xe4\xee\xef\xfb\xf2\x20\x32\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xec\xe5\xed\xfc\xf8\xe0\xe5\xf2\x20\xe7\xe0\xec\xe5\xe4\xeb\xe5\xed\xe8\xe5\x20\xef\xf0\xe8\x20\xef"
         "\xe5\xf0\xe5\xe4\xe2\xe8\xe6\xe5\xed\xe8\xe8\x20\xef\xee\x20\xef\xe5\xf0\xe5\xf1\xe5\xf7\xe5\xed\xed\xee\xe9\x20\xec\xe5\xf1\xf2\xed\xee\xf1\xf2\xe8\x20\xed\xe0\x20\x35\x30\x20\xef\xf0\xee\xf6"
         "\xe5\xed\xf2\xee\xe2\x2e"),
     /* {Следопыт 3 ступени}\n\nПолностью нейтрализует замедление при передвижении по пересеченной местности. */ DATA_COMPGEN(0x0050ff8c, cSecSkillDescExpertPathfindingExpertPathfindingEliminatesTheMovementPenalty2,
         "\x7b\xd1\xeb\xe5\xe4\xee\xef\xfb\xf2\x20\x33\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xcf\xee\xeb\xed\xee\xf1\xf2\xfc\xfe\x20\xed\xe5\xe9\xf2\xf0\xe0\xeb\xe8\xe7\xf3\xe5\xf2\x20\xe7\xe0\xec"
         "\xe5\xe4\xeb\xe5\xed\xe8\xe5\x20\xef\xf0\xe8\x20\xef\xe5\xf0\xe5\xe4\xe2\xe8\xe6\xe5\xed\xe8\xe8\x20\xef\xee\x20\xef\xe5\xf0\xe5\xf1\xe5\xf7\xe5\xed\xed\xee\xe9\x20\xec\xe5\xf1\xf2\xed\xee\xf1"
         "\xf2\xe8\x2e")},
    {/* {Стрелок 1 ступени}\n\nУвеличивает на 10 процентов урон, наносимый стреляющими отрядами. */ DATA_COMPGEN(0x0050fff0, cSecSkillDescBasicArcheryBasicArcheryIncreasesTheDamageDone3,
        "\x7b\xd1\xf2\xf0\xe5\xeb\xee\xea\x20\x31\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\x20\x31\x30\x20\xef\xf0\xee\xf6\xe5\xed\xf2\xee\xe2"
        "\x20\xf3\xf0\xee\xed\x2c\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\xf1\xf2\xf0\xe5\xeb\xff\xfe\xf9\xe8\xec\xe8\x20\xee\xf2\xf0\xff\xe4\xe0\xec\xe8\x2e"),
     /* {Стрелок 2 ступени}\n\nУвеличивает на 25 процентов урон, наносимый стреляющими отрядами. */ DATA_COMPGEN(0x00510048, cSecSkillDescAdvancedArcheryAdvancedArcheryIncreasesTheDamageDone4,
         "\x7b\xd1\xf2\xf0\xe5\xeb\xee\xea\x20\x32\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\x20\x32\x35\x20\xef\xf0\xee\xf6\xe5\xed\xf2\xee\xe2"
         "\x20\xf3\xf0\xee\xed\x2c\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\xf1\xf2\xf0\xe5\xeb\xff\xfe\xf9\xe8\xec\xe8\x20\xee\xf2\xf0\xff\xe4\xe0\xec\xe8\x2e"),
     /* {Стрелок 3 ступени}\n\nУвеличивает на 50 процентов урон, наносимый стреляющими отрядами. */ DATA_COMPGEN(0x005100a0, cSecSkillDescExpertArcheryExpertArcheryIncreasesTheDamageDone5,
         "\x7b\xd1\xf2\xf0\xe5\xeb\xee\xea\x20\x33\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\x20\x35\x30\x20\xef\xf0\xee\xf6\xe5\xed\xf2\xee\xe2"
         "\x20\xf3\xf0\xee\xed\x2c\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\xf1\xf2\xf0\xe5\xeb\xff\xfe\xf9\xe8\xec\xe8\x20\xee\xf2\xf0\xff\xe4\xe0\xec\xe8\x2e")},
    {/* {Логистика 1 ступени}\n\nУвеличивает запас движения героя на 10 процентов. */ DATA_COMPGEN(0x005100f8, cSecSkillDescBasicLogisticsBasicLogisticsIncreasesYourHeroS6,
        "\x7b\xcb\xee\xe3\xe8\xf1\xf2\xe8\xea\xe0\x20\x31\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xe7\xe0\xef\xe0\xf1\x20\xe4\xe2\xe8\xe6\xe5\xed\xe8"
        "\xff\x20\xe3\xe5\xf0\xee\xff\x20\xed\xe0\x20\x31\x30\x20\xef\xf0\xee\xf6\xe5\xed\xf2\xee\xe2\x2e"),
     /* {Логистика 2 ступени}\n\nУвеличивает запас движения героя на 20 процентов. */ DATA_COMPGEN(0x00510144, cSecSkillDescAdvancedLogisticsAdvancedLogisticsIncreasesYourHeroS7,
         "\x7b\xcb\xee\xe3\xe8\xf1\xf2\xe8\xea\xe0\x20\x32\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xe7\xe0\xef\xe0\xf1\x20\xe4\xe2\xe8\xe6\xe5\xed\xe8"
         "\xff\x20\xe3\xe5\xf0\xee\xff\x20\xed\xe0\x20\x32\x30\x20\xef\xf0\xee\xf6\xe5\xed\xf2\xee\xe2\x2e"),
     /* {Логистика 3 ступени}\n\nУвеличивает запас движения героя на 30 процентов. */ DATA_COMPGEN(0x00510190, cSecSkillDescExpertLogisticsExpertLogisticsIncreasesYourHeroS8,
         "\x7b\xcb\xee\xe3\xe8\xf1\xf2\xe8\xea\xe0\x20\x33\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xe7\xe0\xef\xe0\xf1\x20\xe4\xe2\xe8\xe6\xe5\xed\xe8"
         "\xff\x20\xe3\xe5\xf0\xee\xff\x20\xed\xe0\x20\x33\x30\x20\xef\xf0\xee\xf6\xe5\xed\xf2\xee\xe2\x2e")},
    {/* {Разведка 1 ступени}\n\nУвеличивает на 1 клетку радиус обзора героя. */ DATA_COMPGEN(0x005101dc, cSecSkillDescBasicScoutingBasicScoutingIncreasesYourHeroS9,
        "\x7b\xd0\xe0\xe7\xe2\xe5\xe4\xea\xe0\x20\x31\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\x20\x31\x20\xea\xeb\xe5\xf2\xea\xf3\x20\xf0\xe0"
        "\xe4\xe8\xf3\xf1\x20\xee\xe1\xe7\xee\xf0\xe0\x20\xe3\xe5\xf0\xee\xff\x2e"),
     /* {Разведка 2 ступени}\n\nУвеличивает на 2 клетки радиус обзора героя. */ DATA_COMPGEN(0x00510220, cSecSkillDescAdvancedScoutingAdvancedScoutingIncreasesYourHeroS10,
         "\x7b\xd0\xe0\xe7\xe2\xe5\xe4\xea\xe0\x20\x32\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\x20\x32\x20\xea\xeb\xe5\xf2\xea\xe8\x20\xf0\xe0"
         "\xe4\xe8\xf3\xf1\x20\xee\xe1\xe7\xee\xf0\xe0\x20\xe3\xe5\xf0\xee\xff\x2e"),
     /* {Разведка 3 ступени}\n\nУвеличивает на 3 клетки радиус обзора героя. */ DATA_COMPGEN(0x00510264, cSecSkillDescExpertScoutingExpertScoutingIncreasesYourHeroS11,
         "\x7b\xd0\xe0\xe7\xe2\xe5\xe4\xea\xe0\x20\x33\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\x20\x33\x20\xea\xeb\xe5\xf2\xea\xe8\x20\xf0\xe0"
         "\xe4\xe8\xf3\xf1\x20\xee\xe1\xe7\xee\xf0\xe0\x20\xe3\xe5\xf0\xee\xff\x2e")},
    {/* {Дипломатия 1 ступени}\n\nПозволяет вести переговоры с отрядами монстров, более слабыми, чем ваша армия. На таком уровне дипломатии к вам может присоединиться до 1/4 отряда монстров. */ DATA_COMPGEN(0x005102a8, cSecSkillDescBasicDiplomacyBasicDiplomacyAllowsYouToNegotiate12,
        "\x7b\xc4\xe8\xef\xeb\xee\xec\xe0\xf2\xe8\xff\x20\x31\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xcf\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xe2\xe5\xf1\xf2\xe8\x20\xef\xe5\xf0\xe5\xe3\xee\xe2\xee"
        "\xf0\xfb\x20\xf1\x20\xee\xf2\xf0\xff\xe4\xe0\xec\xe8\x20\xec\xee\xed\xf1\xf2\xf0\xee\xe2\x2c\x20\xe1\xee\xeb\xe5\xe5\x20\xf1\xeb\xe0\xe1\xfb\xec\xe8\x2c\x20\xf7\xe5\xec\x20\xe2\xe0\xf8\xe0\x20"
        "\xe0\xf0\xec\xe8\xff\x2e\x20\xcd\xe0\x20\xf2\xe0\xea\xee\xec\x20\xf3\xf0\xee\xe2\xed\xe5\x20\xe4\xe8\xef\xeb\xee\xec\xe0\xf2\xe8\xe8\x20\xea\x20\xe2\xe0\xec\x20\xec\xee\xe6\xe5\xf2\x20\xef\xf0"
        "\xe8\xf1\xee\xe5\xe4\xe8\xed\xe8\xf2\xfc\xf1\xff\x20\xe4\xee\x20\x31\x2f\x34\x20\xee\xf2\xf0\xff\xe4\xe0\x20\xec\xee\xed\xf1\xf2\xf0\xee\xe2\x2e"),
     /* {Дипломатия 2 ступени}\n\nПозволяет вести переговоры с отрядами монстров, более слабыми, чем ваша армия. На таком уровне дипломатии к вам может присоединиться до 1/2 отряда монстров. */ DATA_COMPGEN(0x00510360, cSecSkillDescAdvancedDiplomacyAdvancedDiplomacyAllowsYouToNegotiate13,
         "\x7b\xc4\xe8\xef\xeb\xee\xec\xe0\xf2\xe8\xff\x20\x32\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xcf\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xe2\xe5\xf1\xf2\xe8\x20\xef\xe5\xf0\xe5\xe3\xee\xe2\xee"
         "\xf0\xfb\x20\xf1\x20\xee\xf2\xf0\xff\xe4\xe0\xec\xe8\x20\xec\xee\xed\xf1\xf2\xf0\xee\xe2\x2c\x20\xe1\xee\xeb\xe5\xe5\x20\xf1\xeb\xe0\xe1\xfb\xec\xe8\x2c\x20\xf7\xe5\xec\x20\xe2\xe0\xf8\xe0\x20"
         "\xe0\xf0\xec\xe8\xff\x2e\x20\xcd\xe0\x20\xf2\xe0\xea\xee\xec\x20\xf3\xf0\xee\xe2\xed\xe5\x20\xe4\xe8\xef\xeb\xee\xec\xe0\xf2\xe8\xe8\x20\xea\x20\xe2\xe0\xec\x20\xec\xee\xe6\xe5\xf2\x20\xef\xf0"
         "\xe8\xf1\xee\xe5\xe4\xe8\xed\xe8\xf2\xfc\xf1\xff\x20\xe4\xee\x20\x31\x2f\x32\x20\xee\xf2\xf0\xff\xe4\xe0\x20\xec\xee\xed\xf1\xf2\xf0\xee\xe2\x2e"),
     /* {Дипломатия 3 ступени}\n\nПозволяет вести переговоры с отрядами монстров, более слабыми, чем ваша армия. На таком уровне дипломатии к вам может присоединиться весь отряд монстров. */ DATA_COMPGEN(0x00510418, cSecSkillDescExpertDiplomacyExpertDiplomacyAllowsYouToNegotiate14,
         "\x7b\xc4\xe8\xef\xeb\xee\xec\xe0\xf2\xe8\xff\x20\x33\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xcf\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xe2\xe5\xf1\xf2\xe8\x20\xef\xe5\xf0\xe5\xe3\xee\xe2\xee"
         "\xf0\xfb\x20\xf1\x20\xee\xf2\xf0\xff\xe4\xe0\xec\xe8\x20\xec\xee\xed\xf1\xf2\xf0\xee\xe2\x2c\x20\xe1\xee\xeb\xe5\xe5\x20\xf1\xeb\xe0\xe1\xfb\xec\xe8\x2c\x20\xf7\xe5\xec\x20\xe2\xe0\xf8\xe0\x20"
         "\xe0\xf0\xec\xe8\xff\x2e\x20\xcd\xe0\x20\xf2\xe0\xea\xee\xec\x20\xf3\xf0\xee\xe2\xed\xe5\x20\xe4\xe8\xef\xeb\xee\xec\xe0\xf2\xe8\xe8\x20\xea\x20\xe2\xe0\xec\x20\xec\xee\xe6\xe5\xf2\x20\xef\xf0"
         "\xe8\xf1\xee\xe5\xe4\xe8\xed\xe8\xf2\xfc\xf1\xff\x20\xe2\xe5\xf1\xfc\x20\xee\xf2\xf0\xff\xe4\x20\xec\xee\xed\xf1\xf2\xf0\xee\xe2\x2e")},
    {/* {Навигация 1 ступени}\n\nУвеличивает на 1/3 запас движения героя при передвижении по воде. */ DATA_COMPGEN(0x005104cc, cSecSkillDescBasicNavigationBasicNavigationIncreasesYourHeroS15,
        "\x7b\xcd\xe0\xe2\xe8\xe3\xe0\xf6\xe8\xff\x20\x31\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\x20\x31\x2f\x33\x20\xe7\xe0\xef\xe0\xf1\x20"
        "\xe4\xe2\xe8\xe6\xe5\xed\xe8\xff\x20\xe3\xe5\xf0\xee\xff\x20\xef\xf0\xe8\x20\xef\xe5\xf0\xe5\xe4\xe2\xe8\xe6\xe5\xed\xe8\xe8\x20\xef\xee\x20\xe2\xee\xe4\xe5\x2e"),
     /* {Навигация 2 ступени}\n\nУвеличивает на 2/3 запас движения героя при передвижении по воде. */ DATA_COMPGEN(0x00510528, cSecSkillDescAdvancedNavigationAdvancedNavigationIncreasesYourHeroS16,
         "\x7b\xcd\xe0\xe2\xe8\xe3\xe0\xf6\xe8\xff\x20\x32\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\x20\x32\x2f\x33\x20\xe7\xe0\xef\xe0\xf1\x20"
         "\xe4\xe2\xe8\xe6\xe5\xed\xe8\xff\x20\xe3\xe5\xf0\xee\xff\x20\xef\xf0\xe8\x20\xef\xe5\xf0\xe5\xe4\xe2\xe8\xe6\xe5\xed\xe8\xe8\x20\xef\xee\x20\xe2\xee\xe4\xe5\x2e"),
     /* {Навигация 3 ступени}\n\nУдваивает запас движения героя при передвижении по воде. */ DATA_COMPGEN(0x00510584, cSecSkillDescExpertNavigationExpertNavigationDoublesYourHeroS17,
         "\x7b\xcd\xe0\xe2\xe8\xe3\xe0\xf6\xe8\xff\x20\x33\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xe4\xe2\xe0\xe8\xe2\xe0\xe5\xf2\x20\xe7\xe0\xef\xe0\xf1\x20\xe4\xe2\xe8\xe6\xe5\xed\xe8\xff\x20"
         "\xe3\xe5\xf0\xee\xff\x20\xef\xf0\xe8\x20\xef\xe5\xf0\xe5\xe4\xe2\xe8\xe6\xe5\xed\xe8\xe8\x20\xef\xee\x20\xe2\xee\xe4\xe5\x2e")},
    {/* {Лидерство 1 ступени}\n\nУвеличивает на 1 единицу мораль войск вашего героя. */ DATA_COMPGEN(0x005105d4, cSecSkillDescBasicLeadershipBasicLeadershipIncreasesYourHeroS18,
        "\x7b\xcb\xe8\xe4\xe5\xf0\xf1\xf2\xe2\xee\x20\x31\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\x20\x31\x20\xe5\xe4\xe8\xed\xe8\xf6\xf3\x20"
        "\xec\xee\xf0\xe0\xeb\xfc\x20\xe2\xee\xe9\xf1\xea\x20\xe2\xe0\xf8\xe5\xe3\xee\x20\xe3\xe5\xf0\xee\xff\x2e"),
     /* {Лидерство 2 ступени}\n\nУвеличивает на 2 единицы мораль войск вашего героя. */ DATA_COMPGEN(0x00510620, cSecSkillDescAdvancedLeadershipAdvancedLeadershipIncreasesYourHeroS19,
         "\x7b\xcb\xe8\xe4\xe5\xf0\xf1\xf2\xe2\xee\x20\x32\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\x20\x32\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x20"
         "\xec\xee\xf0\xe0\xeb\xfc\x20\xe2\xee\xe9\xf1\xea\x20\xe2\xe0\xf8\xe5\xe3\xee\x20\xe3\xe5\xf0\xee\xff\x2e"),
     /* {Лидерство 3 ступени}\n\nУвеличивает на 3 единицы мораль войск вашего героя. */ DATA_COMPGEN(0x0051066c, cSecSkillDescExpertLeadershipExpertLeadershipIncreasesYourHeroS20,
         "\x7b\xcb\xe8\xe4\xe5\xf0\xf1\xf2\xe2\xee\x20\x33\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\x20\x33\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x20"
         "\xec\xee\xf0\xe0\xeb\xfc\x20\xe2\xee\xe9\xf1\xea\x20\xe2\xe0\xf8\xe5\xe3\xee\x20\xe3\xe5\xf0\xee\xff\x2e")},
    {/* {Мудрость 1 ступени}\n\nПозволяет вашему герою изучать заклинания третьего уровня. */ DATA_COMPGEN(0x005106b8, cSecSkillDescBasicWisdomBasicWisdomAllowsYourHeroTo21,
        "\x7b\xcc\xf3\xe4\xf0\xee\xf1\xf2\xfc\x20\x31\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xcf\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xe2\xe0\xf8\xe5\xec\xf3\x20\xe3\xe5\xf0\xee\xfe\x20\xe8\xe7\xf3"
        "\xf7\xe0\xf2\xfc\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\x20\xf2\xf0\xe5\xf2\xfc\xe5\xe3\xee\x20\xf3\xf0\xee\xe2\xed\xff\x2e"),
     /* {Мудрость 2 ступени}\n\nПозволяет вашему герою изучать заклинания четвертого уровня. */ DATA_COMPGEN(0x0051070c, cSecSkillDescAdvancedWisdomAdvancedWisdomAllowsYourHeroTo22,
         "\x7b\xcc\xf3\xe4\xf0\xee\xf1\xf2\xfc\x20\x32\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xcf\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xe2\xe0\xf8\xe5\xec\xf3\x20\xe3\xe5\xf0\xee\xfe\x20\xe8\xe7\xf3"
         "\xf7\xe0\xf2\xfc\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\x20\xf7\xe5\xf2\xe2\xe5\xf0\xf2\xee\xe3\xee\x20\xf3\xf0\xee\xe2\xed\xff\x2e"),
     /* {Мудрость 3 ступени}\n\nПозволяет вашему герою изучать заклинания пятого уровня. */ DATA_COMPGEN(0x00510760, cSecSkillDescExpertWisdomExpertWisdomAllowsYourHeroTo23,
         "\x7b\xcc\xf3\xe4\xf0\xee\xf1\xf2\xfc\x20\x33\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xcf\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xe2\xe0\xf8\xe5\xec\xf3\x20\xe3\xe5\xf0\xee\xfe\x20\xe8\xe7\xf3"
         "\xf7\xe0\xf2\xfc\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\x20\xef\xff\xf2\xee\xe3\xee\x20\xf3\xf0\xee\xe2\xed\xff\x2e")},
    {/* {Мистицизм 1 ступени}\n\nВаш герой восстанавливает по 2 очка магии в день. */ DATA_COMPGEN(0x005107b0, cSecSkillDescBasicMysticismBasicMysticismRegeneratesTwoOfYour24,
        "\x7b\xcc\xe8\xf1\xf2\xe8\xf6\xe8\xe7\xec\x20\x31\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xc2\xe0\xf8\x20\xe3\xe5\xf0\xee\xe9\x20\xe2\xee\xf1\xf1\xf2\xe0\xed\xe0\xe2\xeb\xe8\xe2\xe0\xe5\xf2"
        "\x20\xef\xee\x20\x32\x20\xee\xf7\xea\xe0\x20\xec\xe0\xe3\xe8\xe8\x20\xe2\x20\xe4\xe5\xed\xfc\x2e"),
     /* {Мистицизм 2 ступени}\n\nВаш герой восстанавливает по 3 очка магии в день. */ DATA_COMPGEN(0x005107fc, cSecSkillDescAdvancedMysticismAdvancedMysticismRegeneratesThreeOfYour25,
         "\x7b\xcc\xe8\xf1\xf2\xe8\xf6\xe8\xe7\xec\x20\x32\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xc2\xe0\xf8\x20\xe3\xe5\xf0\xee\xe9\x20\xe2\xee\xf1\xf1\xf2\xe0\xed\xe0\xe2\xeb\xe8\xe2\xe0\xe5\xf2"
         "\x20\xef\xee\x20\x33\x20\xee\xf7\xea\xe0\x20\xec\xe0\xe3\xe8\xe8\x20\xe2\x20\xe4\xe5\xed\xfc\x2e"),
     /* {Мистицизм 3 ступени}\n\nВаш герой восстанавливает по 4 очка магии в день. */ DATA_COMPGEN(0x00510848, cSecSkillDescExpertMysticismExpertMysticismRegeneratesFourOfYour26,
         "\x7b\xcc\xe8\xf1\xf2\xe8\xf6\xe8\xe7\xec\x20\x33\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xc2\xe0\xf8\x20\xe3\xe5\xf0\xee\xe9\x20\xe2\xee\xf1\xf1\xf2\xe0\xed\xe0\xe2\xeb\xe8\xe2\xe0\xe5\xf2"
         "\x20\xef\xee\x20\x34\x20\xee\xf7\xea\xe0\x20\xec\xe0\xe3\xe8\xe8\x20\xe2\x20\xe4\xe5\xed\xfc\x2e")},
    {/* {Удача 1 ступени}\n\nУвеличивает на 1 удачу вашего героя. */ DATA_COMPGEN(0x00510894, cSecSkillDescBasicLuckBasicLuckIncreasesYourHeroS27,
        "\x7b\xd3\xe4\xe0\xf7\xe0\x20\x31\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\x20\x31\x20\xf3\xe4\xe0\xf7\xf3\x20\xe2\xe0\xf8\xe5\xe3\xee"
        "\x20\xe3\xe5\xf0\xee\xff\x2e"),
     /* {Удача 2 ступени}\n\nУвеличивает на 2 удачу вашего героя. */ DATA_COMPGEN(0x005108cc, cSecSkillDescAdvancedLuckAdvancedLuckIncreasesYourHeroS28,
         "\x7b\xd3\xe4\xe0\xf7\xe0\x20\x32\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\x20\x32\x20\xf3\xe4\xe0\xf7\xf3\x20\xe2\xe0\xf8\xe5\xe3\xee"
         "\x20\xe3\xe5\xf0\xee\xff\x2e"),
     /* {Удача 3 ступени}\n\nУвеличивает на 3 удачу вашего героя. */ DATA_COMPGEN(0x00510904, cSecSkillDescExpertLuckExpertLuckIncreasesYourHeroS29,
         "\x7b\xd3\xe4\xe0\xf7\xe0\x20\x33\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xed\xe0\x20\x33\x20\xf3\xe4\xe0\xf7\xf3\x20\xe2\xe0\xf8\xe5\xe3\xee"
         "\x20\xe3\xe5\xf0\xee\xff\x2e")},
    {/* {Баллистика 1 ступени}\n\nУвеличивает точность стрельбы  катапульты вашего героя и урон, наносимый крепостным стенам. */ DATA_COMPGEN(0x0051093c, cSecSkillDescBasicBallisticsBasicBallisticsGivesYourHeroS30,
        "\x7b\xc1\xe0\xeb\xeb\xe8\xf1\xf2\xe8\xea\xe0\x20\x31\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xd3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xf2\xee\xf7\xed\xee\xf1\xf2\xfc\x20\xf1\xf2\xf0"
        "\xe5\xeb\xfc\xe1\xfb\x20\x20\xea\xe0\xf2\xe0\xef\xf3\xeb\xfc\xf2\xfb\x20\xe2\xe0\xf8\xe5\xe3\xee\x20\xe3\xe5\xf0\xee\xff\x20\xe8\x20\xf3\xf0\xee\xed\x2c\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9"
        "\x20\xea\xf0\xe5\xef\xee\xf1\xf2\xed\xfb\xec\x20\xf1\xf2\xe5\xed\xe0\xec\x2e"),
     /* {Баллистика 2 ступени}\n\nКатапульта вашего героя делает дополнительный выстрел; при этом увеличивается точность ее стрельбы и урон, наносимый крепостным стенам. */ DATA_COMPGEN(0x005109b0, cSecSkillDescAdvancedBallisticsAdvancedBallisticsGivesYourHeroS31,
         "\x7b\xc1\xe0\xeb\xeb\xe8\xf1\xf2\xe8\xea\xe0\x20\x32\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xca\xe0\xf2\xe0\xef\xf3\xeb\xfc\xf2\xe0\x20\xe2\xe0\xf8\xe5\xe3\xee\x20\xe3\xe5\xf0\xee\xff\x20"
         "\xe4\xe5\xeb\xe0\xe5\xf2\x20\xe4\xee\xef\xee\xeb\xed\xe8\xf2\xe5\xeb\xfc\xed\xfb\xe9\x20\xe2\xfb\xf1\xf2\xf0\xe5\xeb\x3b\x20\xef\xf0\xe8\x20\xfd\xf2\xee\xec\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2"
         "\xe0\xe5\xf2\xf1\xff\x20\xf2\xee\xf7\xed\xee\xf1\xf2\xfc\x20\xe5\xe5\x20\xf1\xf2\xf0\xe5\xeb\xfc\xe1\xfb\x20\xe8\x20\xf3\xf0\xee\xed\x2c\x20\xed\xe0\xed\xee\xf1\xe8\xec\xfb\xe9\x20\xea\xf0\xe5"
         "\xef\xee\xf1\xf2\xed\xfb\xec\x20\xf1\xf2\xe5\xed\xe0\xec\x2e"),
     /* {Баллистика 3 ступени}\n\nКатапульта вашего героя делает дополнительный выстрел; при этом каждый выстрел разрушает любую стену, за исключением укрепленных стен рыцарского замка. */ DATA_COMPGEN(0x00510a50, cSecSkillDescExpertBallisticsExpertBallisticsGivesYourHeroS32,
         "\x7b\xc1\xe0\xeb\xeb\xe8\xf1\xf2\xe8\xea\xe0\x20\x33\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xca\xe0\xf2\xe0\xef\xf3\xeb\xfc\xf2\xe0\x20\xe2\xe0\xf8\xe5\xe3\xee\x20\xe3\xe5\xf0\xee\xff\x20"
         "\xe4\xe5\xeb\xe0\xe5\xf2\x20\xe4\xee\xef\xee\xeb\xed\xe8\xf2\xe5\xeb\xfc\xed\xfb\xe9\x20\xe2\xfb\xf1\xf2\xf0\xe5\xeb\x3b\x20\xef\xf0\xe8\x20\xfd\xf2\xee\xec\x20\xea\xe0\xe6\xe4\xfb\xe9\x20\xe2"
         "\xfb\xf1\xf2\xf0\xe5\xeb\x20\xf0\xe0\xe7\xf0\xf3\xf8\xe0\xe5\xf2\x20\xeb\xfe\xe1\xf3\xfe\x20\xf1\xf2\xe5\xed\xf3\x2c\x20\xe7\xe0\x20\xe8\xf1\xea\xeb\xfe\xf7\xe5\xed\xe8\xe5\xec\x20\xf3\xea\xf0"
         "\xe5\xef\xeb\xe5\xed\xed\xfb\xf5\x20\xf1\xf2\xe5\xed\x20\xf0\xfb\xf6\xe0\xf0\xf1\xea\xee\xe3\xee\x20\xe7\xe0\xec\xea\xe0\x2e")},
    {/* {Орлиный взор 1 ступени}\n\nДает вашему герою 20-процентный шанс выучить любое заклинание первого или второго уровней, примененное против него в бою. */ DATA_COMPGEN(0x00510b00, cSecSkillDescBasicEagleEyeBasicEagleEyeGivesYour33,
        "\x7b\xce\xf0\xeb\xe8\xed\xfb\xe9\x20\xe2\xe7\xee\xf0\x20\x31\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xc4\xe0\xe5\xf2\x20\xe2\xe0\xf8\xe5\xec\xf3\x20\xe3\xe5\xf0\xee\xfe\x20\x32\x30\x2d\xef"
        "\xf0\xee\xf6\xe5\xed\xf2\xed\xfb\xe9\x20\xf8\xe0\xed\xf1\x20\xe2\xfb\xf3\xf7\xe8\xf2\xfc\x20\xeb\xfe\xe1\xee\xe5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5\x20\xef\xe5\xf0\xe2\xee\xe3\xee\x20"
        "\xe8\xeb\xe8\x20\xe2\xf2\xee\xf0\xee\xe3\xee\x20\xf3\xf0\xee\xe2\xed\xe5\xe9\x2c\x20\xef\xf0\xe8\xec\xe5\xed\xe5\xed\xed\xee\xe5\x20\xef\xf0\xee\xf2\xe8\xe2\x20\xed\xe5\xe3\xee\x20\xe2\x20\xe1"
        "\xee\xfe\x2e"),
     /* {Орлиный взор 2 ступени}\n\nДает вашему герою 30-процентный шанс выучить любое заклинание третьего или более низких уровней, примененное против него в бою. */ DATA_COMPGEN(0x00510b94, cSecSkillDescAdvancedEagleEyeAdvancedEagleEyeGivesYour34,
         "\x7b\xce\xf0\xeb\xe8\xed\xfb\xe9\x20\xe2\xe7\xee\xf0\x20\x32\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xc4\xe0\xe5\xf2\x20\xe2\xe0\xf8\xe5\xec\xf3\x20\xe3\xe5\xf0\xee\xfe\x20\x33\x30\x2d\xef"
         "\xf0\xee\xf6\xe5\xed\xf2\xed\xfb\xe9\x20\xf8\xe0\xed\xf1\x20\xe2\xfb\xf3\xf7\xe8\xf2\xfc\x20\xeb\xfe\xe1\xee\xe5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5\x20\xf2\xf0\xe5\xf2\xfc\xe5\xe3\xee"
         "\x20\xe8\xeb\xe8\x20\xe1\xee\xeb\xe5\xe5\x20\xed\xe8\xe7\xea\xe8\xf5\x20\xf3\xf0\xee\xe2\xed\xe5\xe9\x2c\x20\xef\xf0\xe8\xec\xe5\xed\xe5\xed\xed\xee\xe5\x20\xef\xf0\xee\xf2\xe8\xe2\x20\xed\xe5"
         "\xe3\xee\x20\xe2\x20\xe1\xee\xfe\x2e"),
     /* {Орлиный глаз 3 ступени}\n\nДает вашему герою 40-процентный шанс выучить любое заклинание четвертого или более низких уровней, примененное против него в бою. */ DATA_COMPGEN(0x00510c30, cSecSkillDescExpertEagleEyeExpertEagleEyeGivesYour35,
         "\x7b\xce\xf0\xeb\xe8\xed\xfb\xe9\x20\xe3\xeb\xe0\xe7\x20\x33\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xc4\xe0\xe5\xf2\x20\xe2\xe0\xf8\xe5\xec\xf3\x20\xe3\xe5\xf0\xee\xfe\x20\x34\x30\x2d\xef"
         "\xf0\xee\xf6\xe5\xed\xf2\xed\xfb\xe9\x20\xf8\xe0\xed\xf1\x20\xe2\xfb\xf3\xf7\xe8\xf2\xfc\x20\xeb\xfe\xe1\xee\xe5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5\x20\xf7\xe5\xf2\xe2\xe5\xf0\xf2\xee"
         "\xe3\xee\x20\xe8\xeb\xe8\x20\xe1\xee\xeb\xe5\xe5\x20\xed\xe8\xe7\xea\xe8\xf5\x20\xf3\xf0\xee\xe2\xed\xe5\xe9\x2c\x20\xef\xf0\xe8\xec\xe5\xed\xe5\xed\xed\xee\xe5\x20\xef\xf0\xee\xf2\xe8\xe2\x20"
         "\xed\xe5\xe3\xee\x20\xe2\x20\xe1\xee\xfe\x2e")},
    {/* {Некромантия 1 ступени}\n\nВоскрешает 10 процентов существ, павших на поле боя, и превращает их в скелеты для вашей армии. */ DATA_COMPGEN(0x00510ccc, cSecSkillDescBasicNecromancyBasicNecromancyAllows10PercentOf36,
        "\x7b\xcd\xe5\xea\xf0\xee\xec\xe0\xed\xf2\xe8\xff\x20\x31\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xc2\xee\xf1\xea\xf0\xe5\xf8\xe0\xe5\xf2\x20\x31\x30\x20\xef\xf0\xee\xf6\xe5\xed\xf2\xee\xe2"
        "\x20\xf1\xf3\xf9\xe5\xf1\xf2\xe2\x2c\x20\xef\xe0\xe2\xf8\xe8\xf5\x20\xed\xe0\x20\xef\xee\xeb\xe5\x20\xe1\xee\xff\x2c\x20\xe8\x20\xef\xf0\xe5\xe2\xf0\xe0\xf9\xe0\xe5\xf2\x20\xe8\xf5\x20\xe2\x20"
        "\xf1\xea\xe5\xeb\xe5\xf2\xfb\x20\xe4\xeb\xff\x20\xe2\xe0\xf8\xe5\xe9\x20\xe0\xf0\xec\xe8\xe8\x2e"),
     /* {Некромантия 2 ступени}\n\nВоскрешает 20 процентов существ, павших на поле боя, и превращает их в скелеты для вашей армии. */ DATA_COMPGEN(0x00510d48, cSecSkillDescAdvancedNecromancyAdvancedNecromancyAllows20PercentOf37,
         "\x7b\xcd\xe5\xea\xf0\xee\xec\xe0\xed\xf2\xe8\xff\x20\x32\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xc2\xee\xf1\xea\xf0\xe5\xf8\xe0\xe5\xf2\x20\x32\x30\x20\xef\xf0\xee\xf6\xe5\xed\xf2\xee\xe2"
         "\x20\xf1\xf3\xf9\xe5\xf1\xf2\xe2\x2c\x20\xef\xe0\xe2\xf8\xe8\xf5\x20\xed\xe0\x20\xef\xee\xeb\xe5\x20\xe1\xee\xff\x2c\x20\xe8\x20\xef\xf0\xe5\xe2\xf0\xe0\xf9\xe0\xe5\xf2\x20\xe8\xf5\x20\xe2\x20"
         "\xf1\xea\xe5\xeb\xe5\xf2\xfb\x20\xe4\xeb\xff\x20\xe2\xe0\xf8\xe5\xe9\x20\xe0\xf0\xec\xe8\xe8\x2e"),
     /* {Некромантия 3 ступени}\n\nВоскрешает 30 процентов существ, павших на поле боя, и превращает их в скелеты для вашей армии. */ DATA_COMPGEN(0x00510dc4, cSecSkillDescExpertNecromancyExpertNecromancyAllows30PercentOf38,
         "\x7b\xcd\xe5\xea\xf0\xee\xec\xe0\xed\xf2\xe8\xff\x20\x33\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xc2\xee\xf1\xea\xf0\xe5\xf8\xe0\xe5\xf2\x20\x33\x30\x20\xef\xf0\xee\xf6\xe5\xed\xf2\xee\xe2"
         "\x20\xf1\xf3\xf9\xe5\xf1\xf2\xe2\x2c\x20\xef\xe0\xe2\xf8\xe8\xf5\x20\xed\xe0\x20\xef\xee\xeb\xe5\x20\xe1\xee\xff\x2c\x20\xe8\x20\xef\xf0\xe5\xe2\xf0\xe0\xf9\xe0\xe5\xf2\x20\xe8\xf5\x20\xe2\x20"
         "\xf1\xea\xe5\xeb\xe5\xf2\xfb\x20\xe4\xeb\xff\x20\xe2\xe0\xf8\xe5\xe9\x20\xe0\xf0\xec\xe8\xe8\x2e")},
    {/* {Казначей 1 ступени}\n\nВаш герой ежедневно собирает со своих владений налоги в размере 100 золотых. */ DATA_COMPGEN(0x00510e40, cSecSkillDescSmallEstatesYourHeroProduces100GoldPieces39,
        "\x7b\xca\xe0\xe7\xed\xe0\xf7\xe5\xe9\x20\x31\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xc2\xe0\xf8\x20\xe3\xe5\xf0\xee\xe9\x20\xe5\xe6\xe5\xe4\xed\xe5\xe2\xed\xee\x20\xf1\xee\xe1\xe8\xf0\xe0"
        "\xe5\xf2\x20\xf1\xee\x20\xf1\xe2\xee\xe8\xf5\x20\xe2\xeb\xe0\xe4\xe5\xed\xe8\xe9\x20\xed\xe0\xeb\xee\xe3\xe8\x20\xe2\x20\xf0\xe0\xe7\xec\xe5\xf0\xe5\x20\x31\x30\x30\x20\xe7\xee\xeb\xee\xf2\xfb"
        "\xf5\x2e"),
     /* {Казанчей 2 ступени}\n\nВаш герой ежедневно собирает со своих владений налоги в размере 250 золотых. */ DATA_COMPGEN(0x00510ea4, cSecSkillDescModerateEstatesYourHeroProduces250GoldPieces40,
         "\x7b\xca\xe0\xe7\xe0\xed\xf7\xe5\xe9\x20\x32\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\xc2\xe0\xf8\x20\xe3\xe5\xf0\xee\xe9\x20\xe5\xe6\xe5\xe4\xed\xe5\xe2\xed\xee\x20\xf1\xee\xe1\xe8\xf0\xe0"
         "\xe5\xf2\x20\xf1\xee\x20\xf1\xe2\xee\xe8\xf5\x20\xe2\xeb\xe0\xe4\xe5\xed\xe8\xe9\x20\xed\xe0\xeb\xee\xe3\xe8\x20\xe2\x20\xf0\xe0\xe7\xec\xe5\xf0\xe5\x20\x32\x35\x30\x20\xe7\xee\xeb\xee\xf2\xfb"
         "\xf5\x2e"),
     /* {Казначей 3 ступени}\n\n Ваш герой ежедневно собирает со своих владений налоги в размере 500 золотых. */ DATA_COMPGEN(0x00510f08, cSecSkillDescGrandEstatesYourHeroProduces500GoldPieces41,
         "\x7b\xca\xe0\xe7\xed\xe0\xf7\xe5\xe9\x20\x33\x20\xf1\xf2\xf3\xef\xe5\xed\xe8\x7d\x0a\x0a\x20\xc2\xe0\xf8\x20\xe3\xe5\xf0\xee\xe9\x20\xe5\xe6\xe5\xe4\xed\xe5\xe2\xed\xee\x20\xf1\xee\xe1\xe8\xf0"
         "\xe0\xe5\xf2\x20\xf1\xee\x20\xf1\xe2\xee\xe8\xf5\x20\xe2\xeb\xe0\xe4\xe5\xed\xe8\xe9\x20\xed\xe0\xeb\xee\xe3\xe8\x20\xe2\x20\xf0\xe0\xe7\xec\xe5\xf0\xe5\x20\x35\x30\x30\x20\xe7\xee\xeb\xee\xf2"
         "\xfb\xf5\x2e")}
};
DATA(0x004fef98) char* cBuildingInfoNeutral[KB_NEUTRAL_BUILDING_INFO_COUNT] = {
    /* Гильдия магов позволяет разучивать новые заклинания и восстанавливает запас очков магии. */ DATA_COMPGEN(0x00510f6c, cBuildingInfoNeutralTheMageGuildAllowsHeroesToLearnSpells0,
        "\xc3\xe8\xeb\xfc\xe4\xe8\xff\x20\xec\xe0\xe3\xee\xe2\x20\xef\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xf0\xe0\xe7\xf3\xf7\xe8\xe2\xe0\xf2\xfc\x20\xed\xee\xe2\xfb\xe5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0"
        "\xed\xe8\xff\x20\xe8\x20\xe2\xee\xf1\xf1\xf2\xe0\xed\xe0\xe2\xeb\xe8\xe2\xe0\xe5\xf2\x20\xe7\xe0\xef\xe0\xf1\x20\xee\xf7\xea\xee\xe2\x20\xec\xe0\xe3\xe8\xe8\x2e"),
    /* Гильдия воров дает информацию о врагах. Также, Гильдия воров дает разведывательную информацию о вражеских городах. Дополнительные гильдии дают дополнительную информацию. */ DATA_COMPGEN(0x00510fc8, cBuildingInfoNeutralTheThievesGuildProvidesInformationOnEnemyPlayers1,
        "\xc3\xe8\xeb\xfc\xe4\xe8\xff\x20\xe2\xee\xf0\xee\xe2\x20\xe4\xe0\xe5\xf2\x20\xe8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xfe\x20\xee\x20\xe2\xf0\xe0\xe3\xe0\xf5\x2e\x20\xd2\xe0\xea\xe6\xe5\x2c\x20\xc3"
        "\xe8\xeb\xfc\xe4\xe8\xff\x20\xe2\xee\xf0\xee\xe2\x20\xe4\xe0\xe5\xf2\x20\xf0\xe0\xe7\xe2\xe5\xe4\xfb\xe2\xe0\xf2\xe5\xeb\xfc\xed\xf3\xfe\x20\xe8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xfe\x20\xee\x20"
        "\xe2\xf0\xe0\xe6\xe5\xf1\xea\xe8\xf5\x20\xe3\xee\xf0\xee\xe4\xe0\xf5\x2e\x20\xc4\xee\xef\xee\xeb\xed\xe8\xf2\xe5\xeb\xfc\xed\xfb\xe5\x20\xe3\xe8\xeb\xfc\xe4\xe8\xe8\x20\xe4\xe0\xfe\xf2\x20\xe4"
        "\xee\xef\xee\xeb\xed\xe8\xf2\xe5\xeb\xfc\xed\xf3\xfe\x20\xe8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xfe\x2e"),
    /* Таверна увеличивает мораль бойцов, защищающих замок. */ DATA_COMPGEN(0x00511074, cBuildingInfoNeutralTheTavernIncreasesMoraleForTroopsDefendingThe2,
        "\xd2\xe0\xe2\xe5\xf0\xed\xe0\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xec\xee\xf0\xe0\xeb\xfc\x20\xe1\xee\xe9\xf6\xee\xe2\x2c\x20\xe7\xe0\xf9\xe8\xf9\xe0\xfe\xf9\xe8\xf5\x20\xe7\xe0"
        "\xec\xee\xea\x2e"),
    /* Верфь позволяет строить корабли. */ DATA_COMPGEN(0x005110ac, cBuildingInfoNeutralTheShipyardAllowsShipsToBeBuilt3, "\xc2\xe5\xf0\xf4\xfc\x20\xef\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xf1\xf2\xf0\xee\xe8\xf2\xfc\x20\xea\xee\xf0\xe0\xe1\xeb\xe8\x2e"),
    /* Колодец увеличивает прирост всех воинов на 2 в неделю. */ DATA_COMPGEN(0x005110d0, cBuildingInfoNeutralTheWellIncreasesTheGrowthRateOfAll4,
        "\xca\xee\xeb\xee\xe4\xe5\xf6\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xef\xf0\xe8\xf0\xee\xf1\xf2\x20\xe2\xf1\xe5\xf5\x20\xe2\xee\xe8\xed\xee\xe2\x20\xed\xe0\x20\x32\x20\xe2\x20\xed"
        "\xe5\xe4\xe5\xeb\xfe\x2e"),
    /* Шатер дает рабочих, которые могут возвести замок. */ DATA_COMPGEN(0x00511108, cBuildingInfoNeutralTheTentProvidesWorkersToBuildACastle5,
        "\xd8\xe0\xf2\xe5\xf0\x20\xe4\xe0\xe5\xf2\x20\xf0\xe0\xe1\xee\xf7\xe8\xf5\x2c\x20\xea\xee\xf2\xee\xf0\xfb\xe5\x20\xec\xee\xe3\xf3\xf2\x20\xe2\xee\xe7\xe2\xe5\xf1\xf2\xe8\x20\xe7\xe0\xec\xee\xea"
        "\x2e"),
    /* Замок улучшает защиту города и увеличивает доход до 1000 золотых в день. */ DATA_COMPGEN(0x0051113c, cBuildingInfoNeutralTheCastleImprovesTownDefenseAndIncreasesIncome6,
        "\xc7\xe0\xec\xee\xea\x20\xf3\xeb\xf3\xf7\xf8\xe0\xe5\xf2\x20\xe7\xe0\xf9\xe8\xf2\xf3\x20\xe3\xee\xf0\xee\xe4\xe0\x20\xe8\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xe4\xee\xf5\xee\xe4"
        "\x20\xe4\xee\x20\x31\x30\x30\x30\x20\xe7\xee\xeb\xee\xf2\xfb\xf5\x20\xe2\x20\xe4\xe5\xed\xfc\x2e"),
    /* Статуя увеличивает доход города на 250 золотых в день. */ DATA_COMPGEN(0x00511188, cBuildingInfoNeutralTheStatueIncreasesYourTownSIncomeBy7,
        "\xd1\xf2\xe0\xf2\xf3\xff\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xe4\xee\xf5\xee\xe4\x20\xe3\xee\xf0\xee\xe4\xe0\x20\xed\xe0\x20\x32\x35\x30\x20\xe7\xee\xeb\xee\xf2\xfb\xf5\x20\xe2"
        "\x20\xe4\xe5\xed\xfc\x2e"),
    /* Левая башня обеспечивает в бою дополнительную огневую мощь замку. */ DATA_COMPGEN(0x005111c0, cBuildingInfoNeutralTheLeftTurretProvidesExtraFirepowerDuringCastle8,
        "\xcb\xe5\xe2\xe0\xff\x20\xe1\xe0\xf8\xed\xff\x20\xee\xe1\xe5\xf1\xef\xe5\xf7\xe8\xe2\xe0\xe5\xf2\x20\xe2\x20\xe1\xee\xfe\x20\xe4\xee\xef\xee\xeb\xed\xe8\xf2\xe5\xeb\xfc\xed\xf3\xfe\x20\xee\xe3"
        "\xed\xe5\xe2\xf3\xfe\x20\xec\xee\xf9\xfc\x20\xe7\xe0\xec\xea\xf3\x2e"),
    /* Правая башня обеспечивает в бою дополнительную огневую мощь замку. */ DATA_COMPGEN(0x00511204, cBuildingInfoNeutralTheRightTurretProvidesExtraFirepowerDuringCastle9,
        "\xcf\xf0\xe0\xe2\xe0\xff\x20\xe1\xe0\xf8\xed\xff\x20\xee\xe1\xe5\xf1\xef\xe5\xf7\xe8\xe2\xe0\xe5\xf2\x20\xe2\x20\xe1\xee\xfe\x20\xe4\xee\xef\xee\xeb\xed\xe8\xf2\xe5\xeb\xfc\xed\xf3\xfe\x20\xee"
        "\xe3\xed\xe5\xe2\xf3\xfe\x20\xec\xee\xf9\xfc\x20\xe7\xe0\xec\xea\xf3\x2e"),
    /* Рынок можно использовать для перевода одного типа ресурсов в другой. Чем больше рынков вы контролируете, тем выгодней цена. */ DATA_COMPGEN(0x00511248, cBuildingInfoNeutralTheMarketplaceCanBeUsedToConvertOne10,
        "\xd0\xfb\xed\xee\xea\x20\xec\xee\xe6\xed\xee\x20\xe8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xf2\xfc\x20\xe4\xeb\xff\x20\xef\xe5\xf0\xe5\xe2\xee\xe4\xe0\x20\xee\xe4\xed\xee\xe3\xee\x20\xf2\xe8\xef"
        "\xe0\x20\xf0\xe5\xf1\xf3\xf0\xf1\xee\xe2\x20\xe2\x20\xe4\xf0\xf3\xe3\xee\xe9\x2e\x20\xd7\xe5\xec\x20\xe1\xee\xeb\xfc\xf8\xe5\x20\xf0\xfb\xed\xea\xee\xe2\x20\xe2\xfb\x20\xea\xee\xed\xf2\xf0\xee"
        "\xeb\xe8\xf0\xf3\xe5\xf2\xe5\x2c\x20\xf2\xe5\xec\x20\xe2\xfb\xe3\xee\xe4\xed\xe5\xe9\x20\xf6\xe5\xed\xe0\x2e"),
    /*  */ DATA_COMPGEN(0x0052683c, cBuildingInfoNeutralEmpty11, ""),
    /* Ров замедляет атаку вражеских воинов. Любой воин, вошедший в ров, окончит тут свое движение и станет более уязвимым для атаки. */ DATA_COMPGEN(0x005112c4, cBuildingInfoNeutralTheMoatSlowsAttackingUnitsAnyUnitEntering12,
        "\xd0\xee\xe2\x20\xe7\xe0\xec\xe5\xe4\xeb\xff\xe5\xf2\x20\xe0\xf2\xe0\xea\xf3\x20\xe2\xf0\xe0\xe6\xe5\xf1\xea\xe8\xf5\x20\xe2\xee\xe8\xed\xee\xe2\x2e\x20\xcb\xfe\xe1\xee\xe9\x20\xe2\xee\xe8\xed"
        "\x2c\x20\xe2\xee\xf8\xe5\xe4\xf8\xe8\xe9\x20\xe2\x20\xf0\xee\xe2\x2c\x20\xee\xea\xee\xed\xf7\xe8\xf2\x20\xf2\xf3\xf2\x20\xf1\xe2\xee\xe5\x20\xe4\xe2\xe8\xe6\xe5\xed\xe8\xe5\x20\xe8\x20\xf1\xf2"
        "\xe0\xed\xe5\xf2\x20\xe1\xee\xeb\xe5\xe5\x20\xf3\xff\xe7\xe2\xe8\xec\xfb\xec\x20\xe4\xeb\xff\x20\xe0\xf2\xe0\xea\xe8\x2e"),
    /*  */ DATA_COMPGEN(0x00526840, cBuildingInfoNeutralEmpty13, ""),
    /* Верфь позволяет строить корабли. */ DATA_COMPGEN(0x00511344, cBuildingInfoNeutralTheShipyardAllowsShipsToBeBuilt14, "\xc2\xe5\xf0\xf4\xfc\x20\xef\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xf1\xf2\xf0\xee\xe8\xf2\xfc\x20\xea\xee\xf0\xe0\xe1\xeb\xe8\x2e"),
    /* Дом капитана позволяет капитану городской стражи организовать защиту замка в отсутствии героя. */ DATA_COMPGEN(0x00511368, cBuildingInfoNeutralTheCaptainSQuartersProvidesACaptainTo15,
        "\xc4\xee\xec\x20\xea\xe0\xef\xe8\xf2\xe0\xed\xe0\x20\xef\xee\xe7\xe2\xee\xeb\xff\xe5\xf2\x20\xea\xe0\xef\xe8\xf2\xe0\xed\xf3\x20\xe3\xee\xf0\xee\xe4\xf1\xea\xee\xe9\x20\xf1\xf2\xf0\xe0\xe6\xe8"
        "\x20\xee\xf0\xe3\xe0\xed\xe8\xe7\xee\xe2\xe0\xf2\xfc\x20\xe7\xe0\xf9\xe8\xf2\xf3\x20\xe7\xe0\xec\xea\xe0\x20\xe2\x20\xee\xf2\xf1\xf3\xf2\xf1\xf2\xe2\xe8\xe8\x20\xe3\xe5\xf0\xee\xff\x2e"),
    /*  */ DATA_COMPGEN(0x00526844, cBuildingInfoNeutralEmpty16, ""),
    /*  */ DATA_COMPGEN(0x00526848, cBuildingInfoNeutralEmpty17, ""),
    /*  */ DATA_COMPGEN(0x0052684c, cBuildingInfoNeutralEmpty18, "")
};
DATA(0x004fefe4) char* gBuildingInfoSpecial[KB_SPECIAL_BUILDING_INFO_COUNT] = {
    /* Укрепления увеличивают прочность стен, увеличивая число раундов, необходимых для полного их разрушения. */ DATA_COMPGEN(0x005113c8, gBuildingInfoSpecialTheFortificationsIncreaseTheToughnessOfTheWalls0,
        "\xd3\xea\xf0\xe5\xef\xeb\xe5\xed\xe8\xff\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xfe\xf2\x20\xef\xf0\xee\xf7\xed\xee\xf1\xf2\xfc\x20\xf1\xf2\xe5\xed\x2c\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0"
        "\xff\x20\xf7\xe8\xf1\xeb\xee\x20\xf0\xe0\xf3\xed\xe4\xee\xe2\x2c\x20\xed\xe5\xee\xe1\xf5\xee\xe4\xe8\xec\xfb\xf5\x20\xe4\xeb\xff\x20\xef\xee\xeb\xed\xee\xe3\xee\x20\xe8\xf5\x20\xf0\xe0\xe7\xf0"
        "\xf3\xf8\xe5\xed\xe8\xff\x2e"),
    /* Представления, проходимые в Колизее, увеличивают мораль защитников замка на 2 единицы. */ DATA_COMPGEN(0x00511430, gBuildingInfoSpecialTheColiseumProvidesInspiringSpectaclesToDefendingTroops1,
        "\xcf\xf0\xe5\xe4\xf1\xf2\xe0\xe2\xeb\xe5\xed\xe8\xff\x2c\x20\xef\xf0\xee\xf5\xee\xe4\xe8\xec\xfb\xe5\x20\xe2\x20\xca\xee\xeb\xe8\xe7\xe5\xe5\x2c\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xfe\xf2"
        "\x20\xec\xee\xf0\xe0\xeb\xfc\x20\xe7\xe0\xf9\xe8\xf2\xed\xe8\xea\xee\xe2\x20\xe7\xe0\xec\xea\xe0\x20\xed\xe0\x20\x32\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x2e"),
    /* Радуга увеличивает удачу защитников замка на 2 единицы. */ DATA_COMPGEN(0x00511488, gBuildingInfoSpecialTheRainbowIncreasesTheLuckOfTheDefending2,
        "\xd0\xe0\xe4\xf3\xe3\xe0\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xf3\xe4\xe0\xf7\xf3\x20\xe7\xe0\xf9\xe8\xf2\xed\xe8\xea\xee\xe2\x20\xe7\xe0\xec\xea\xe0\x20\xed\xe0\x20\x32\x20\xe5"
        "\xe4\xe8\xed\xe8\xf6\xfb\x2e"),
    /* Подземелье увеличивает доход города на 500 золотых в день. */ DATA_COMPGEN(0x005114c0, gBuildingInfoSpecialTheDungeonIncreasesTheIncomeOfTheTown3,
        "\xcf\xee\xe4\xe7\xe5\xec\xe5\xeb\xfc\xe5\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xe4\xee\xf5\xee\xe4\x20\xe3\xee\xf0\xee\xe4\xe0\x20\xed\xe0\x20\x35\x30\x30\x20\xe7\xee\xeb\xee\xf2"
        "\xfb\xf5\x20\xe2\x20\xe4\xe5\xed\xfc\x2e"),
    /* Библиотека увеличивает число заклинаний, доступных в Гильдии на 1 на каждый ее этаж. */ DATA_COMPGEN(0x005114fc, gBuildingInfoSpecialTheLibraryIncreasesTheNumberOfSpellsIn4,
        "\xc1\xe8\xe1\xeb\xe8\xee\xf2\xe5\xea\xe0\x20\xf3\xe2\xe5\xeb\xe8\xf7\xe8\xe2\xe0\xe5\xf2\x20\xf7\xe8\xf1\xeb\xee\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe9\x2c\x20\xe4\xee\xf1\xf2\xf3\xef\xed"
        "\xfb\xf5\x20\xe2\x20\xc3\xe8\xeb\xfc\xe4\xe8\xe8\x20\xed\xe0\x20\x31\x20\xed\xe0\x20\xea\xe0\xe6\xe4\xfb\xe9\x20\xe5\xe5\x20\xfd\xf2\xe0\xe6\x2e"),
    /* Шторм добавляет +2 единицы к силе заклинаний защитников замка. */ DATA_COMPGEN(0x00511554, gBuildingInfoSpecialTheStormAdds2ToThePowerOf5,
        "\xd8\xf2\xee\xf0\xec\x20\xe4\xee\xe1\xe0\xe2\xeb\xff\xe5\xf2\x20\x2b\x32\x20\xe5\xe4\xe8\xed\xe8\xf6\xfb\x20\xea\x20\xf1\xe8\xeb\xe5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe9\x20\xe7\xe0\xf9"
        "\xe8\xf2\xed\xe8\xea\xee\xe2\x20\xe7\xe0\xec\xea\xe0\x2e")
};
DATA(0x004feffc) char* cDirections[KB_DIRECTION_TEXT_COUNT] = {
    /* севернее */ DATA_COMPGEN(0x00511594, cDirectionsNorthern0, "\xf1\xe5\xe2\xe5\xf0\xed\xe5\xe5"),
    /* северо-восточнее */ DATA_COMPGEN(0x005115a0, cDirectionsNortheastern1, "\xf1\xe5\xe2\xe5\xf0\xee\x2d\xe2\xee\xf1\xf2\xee\xf7\xed\xe5\xe5"),
    /* восточнее */ DATA_COMPGEN(0x005115b4, cDirectionsEastern2, "\xe2\xee\xf1\xf2\xee\xf7\xed\xe5\xe5"),
    /* юго-восточнее */ DATA_COMPGEN(0x005115c0, cDirectionsSoutheastern3, "\xfe\xe3\xee\x2d\xe2\xee\xf1\xf2\xee\xf7\xed\xe5\xe5"),
    /* южнее */ DATA_COMPGEN(0x005115d0, cDirectionsSouthern4, "\xfe\xe6\xed\xe5\xe5"),
    /* юго-восточнее */ DATA_COMPGEN(0x005115d8, cDirectionsSouthwestern5, "\xfe\xe3\xee\x2d\xe2\xee\xf1\xf2\xee\xf7\xed\xe5\xe5"),
    /* западнее */ DATA_COMPGEN(0x005115e8, cDirectionsWestern6, "\xe7\xe0\xef\xe0\xe4\xed\xe5\xe5"),
    /* северо-западнее */ DATA_COMPGEN(0x005115f4, cDirectionsNorthwestern7, "\xf1\xe5\xe2\xe5\xf0\xee\x2d\xe7\xe0\xef\xe0\xe4\xed\xe5\xe5"),
    /* в центре */ DATA_COMPGEN(0x00511604, cDirectionsCentral8, "\xe2\x20\xf6\xe5\xed\xf2\xf0\xe5")
};
DATA(0x004ff020) char* cRumourTerrainDescriptions[KB_RUMOUR_TERRAIN_DESCRIPTION_COUNT] = {
    /* Темные пучины океана */ DATA_COMPGEN(0x00511610, cRumourTerrainDescriptionsInTheMurkyDepthsOfTheOcean0, "\xd2\xe5\xec\xed\xfb\xe5\x20\xef\xf3\xf7\xe8\xed\xfb\x20\xee\xea\xe5\xe0\xed\xe0"),
    /* Зеленые равнины */ DATA_COMPGEN(0x00511628, cRumourTerrainDescriptionsOnAGrassyPlain1, "\xc7\xe5\xeb\xe5\xed\xfb\xe5\x20\xf0\xe0\xe2\xed\xe8\xed\xfb"),
    /* Глубокие снега */ DATA_COMPGEN(0x00511638, cRumourTerrainDescriptionsInTheDrivenSnow2, "\xc3\xeb\xf3\xe1\xee\xea\xe8\xe5\x20\xf1\xed\xe5\xe3\xe0"),
    /* Топкие болота */ DATA_COMPGEN(0x00511648, cRumourTerrainDescriptionsInAMurkySwamp3, "\xd2\xee\xef\xea\xe8\xe5\x20\xe1\xee\xeb\xee\xf2\xe0"),
    /* Застывшая лава */ DATA_COMPGEN(0x00511658, cRumourTerrainDescriptionsBuriedInBarrenLava4, "\xc7\xe0\xf1\xf2\xfb\xe2\xf8\xe0\xff\x20\xeb\xe0\xe2\xe0"),
    /* Бескрайние пески */ DATA_COMPGEN(0x00511668, cRumourTerrainDescriptionsUnderADesertDrift5, "\xc1\xe5\xf1\xea\xf0\xe0\xe9\xed\xe8\xe5\x20\xef\xe5\xf1\xea\xe8"),
    /* Грязь */ DATA_COMPGEN(0x0051167c, cRumourTerrainDescriptionsInTheDirt6, "\xc3\xf0\xff\xe7\xfc"),
    /* Бесплодная пустошь */ DATA_COMPGEN(0x00511684, cRumourTerrainDescriptionsInAParchedWasteland7, "\xc1\xe5\xf1\xef\xeb\xee\xe4\xed\xe0\xff\x20\xef\xf3\xf1\xf2\xee\xf8\xfc"),
    /* Побережье */ DATA_COMPGEN(0x00511698, cRumourTerrainDescriptionsBuriedOnABeach8, "\xcf\xee\xe1\xe5\xf0\xe5\xe6\xfc\xe5")
};
DATA(0x004ff044) char* gInterfaceTypeText[KB_INTERFACE_TYPE_TEXT_COUNT] = {/* Разный */ DATA_COMPGEN(0x005116a4, gInterfaceTypeTextDynamic0, "\xd0\xe0\xe7\xed\xfb\xe9"), /* 'Добрый' */ DATA_COMPGEN(0x005116ac, gInterfaceTypeTextGood1, "\x27\xc4\xee\xe1\xf0\xfb\xe9\x27"), /* 'Злой' */ DATA_COMPGEN(0x005116b8, gInterfaceTypeTextEvil2, "\x27\xc7\xeb\xee\xe9\x27")};
DATA(0x004ff050) char* cBWMouseText[KB_BW_MOUSE_TEXT_COUNT] = {/* Монохром */ DATA_COMPGEN(0x005116c0, cBWMouseTextBlackWhite0, "\xcc\xee\xed\xee\xf5\xf0\xee\xec"), /* Цветной */ DATA_COMPGEN(0x005116cc, cBWMouseTextColor1, "\xd6\xe2\xe5\xf2\xed\xee\xe9")};
DATA(0x004ff058) char* combatSpeedText[KB_COMBAT_SPEED_TEXT_COUNT] = {/* Обычная */ DATA_COMPGEN(0x005116d4, combatSpeedTextNormal0, "\xce\xe1\xfb\xf7\xed\xe0\xff"), /* Высокая */ DATA_COMPGEN(0x005116dc, combatSpeedTextFast1, "\xc2\xfb\xf1\xee\xea\xe0\xff"), /* Оч. высокая */ DATA_COMPGEN(0x005116e4, combatSpeedTextVeryFast2, "\xce\xf7\x2e\x20\xe2\xfb\xf1\xee\xea\xe0\xff")};
DATA(0x004ff064) char* combatMiniInfoText[KB_COMBAT_MINI_INFO_TEXT_COUNT] = {/* Нет */ DATA_COMPGEN(0x005116f0, combatMiniInfoTextNone0, "\xcd\xe5\xf2"), /* Только чары */ DATA_COMPGEN(0x005116f4, combatMiniInfoTextSpellsOnly1, "\xd2\xee\xeb\xfc\xea\xee\x20\xf7\xe0\xf0\xfb"), /* Полная */ DATA_COMPGEN(0x00511700, combatMiniInfoTextFull2, "\xcf\xee\xeb\xed\xe0\xff")};
DATA(0x004ff070) char* gcCommandLineHelp[KB_COMMAND_LINE_HELP_COUNT] = {
    /* \n\n\n***Command Line Help***\n */ DATA_COMPGEN(0x00511708, gcCommandLineHelpCommandLineHelp0, "\x0a\x0a\x0a\x2a\x2a\x2a\x43\x6f\x6d\x6d\x61\x6e\x64\x20\x4c\x69\x6e\x65\x20\x48\x65\x6c\x70\x2a\x2a\x2a\x0a"),
    /* \n */ DATA_COMPGEN(0x00511724, gcCommandLineHelpEmpty1, "\x0a"),
    /* /D0 - отключить цифровой звук\n */ DATA_COMPGEN(0x00511728, gcCommandLineHelpD0DisableDigitalSound2, "\x2f\x44\x30\x20\x2d\x20\xee\xf2\xea\xeb\xfe\xf7\xe8\xf2\xfc\x20\xf6\xe8\xf4\xf0\xee\xe2\xee\xe9\x20\xe7\xe2\xf3\xea\x0a"),
    /* /M0 - отключить MIDI музыку\n */ DATA_COMPGEN(0x00511748, gcCommandLineHelpM0DisableMIDIMusic3, "\x2f\x4d\x30\x20\x2d\x20\xee\xf2\xea\xeb\xfe\xf7\xe8\xf2\xfc\x20\x4d\x49\x44\x49\x20\xec\xf3\xe7\xfb\xea\xf3\x0a"),
    /* /R0 - отключить музыку\n */ DATA_COMPGEN(0x00511768, gcCommandLineHelpR0DisableRedbookMusic4, "\x2f\x52\x30\x20\x2d\x20\xee\xf2\xea\xeb\xfe\xf7\xe8\xf2\xfc\x20\xec\xf3\xe7\xfb\xea\xf3\x0a"),
    /* /I0 - пропустить интро\n */ DATA_COMPGEN(0x00511780, gcCommandLineHelpI0SkipTheIntro5, "\x2f\x49\x30\x20\x2d\x20\xef\xf0\xee\xef\xf3\xf1\xf2\xe8\xf2\xfc\x20\xe8\xed\xf2\xf0\xee\x0a"),
    /* \n */ DATA_COMPGEN(0x00511798, gcCommandLineHelpEmpty6, "\x0a"),
    /* \n */ DATA_COMPGEN(0x0051179c, gcCommandLineHelpEmpty7, "\x0a"),
    /* Пример:\n */ DATA_COMPGEN(0x005117a0, gcCommandLineHelpExample8, "\xcf\xf0\xe8\xec\xe5\xf0\x3a\x0a"),
    /* \n */ DATA_COMPGEN(0x005117ac, gcCommandLineHelpEmpty9, "\x0a"),
    /* HEROES2D /R0 /I0\n */ DATA_COMPGEN(0x005117b0, gcCommandLineHelpHEROES2DR0I010, "\x48\x45\x52\x4f\x45\x53\x32\x44\x20\x2f\x52\x30\x20\x2f\x49\x30\x0a"),
    /* \n */ DATA_COMPGEN(0x005117c4, gcCommandLineHelpEmpty11, "\x0a"),
    /* Загрузить DOS версию Героев 2.\n */ DATA_COMPGEN(0x005117c8, gcCommandLineHelpStartsTheDOSVersionOfHeroes2WithRedbook12, "\xc7\xe0\xe3\xf0\xf3\xe7\xe8\xf2\xfc\x20\x44\x4f\x53\x20\xe2\xe5\xf0\xf1\xe8\xfe\x20\xc3\xe5\xf0\xee\xe5\xe2\x20\x32\x2e\x0a"),
    /* Звук отключен и интро пропущено.\n */ DATA_COMPGEN(0x005117e8, gcCommandLineHelpSoundDisabledAndTheIntroSkipped13, "\xc7\xe2\xf3\xea\x20\xee\xf2\xea\xeb\xfe\xf7\xe5\xed\x20\xe8\x20\xe8\xed\xf2\xf0\xee\x20\xef\xf0\xee\xef\xf3\xf9\xe5\xed\xee\x2e\x0a")
};
DATA(0x004ff0a8) char* cOverviewText[KB_OVERVIEW_TEXT_COUNT] =
    {/* Герой/Параметры */ DATA_COMPGEN(0x0051180c, cOverviewTextHeroStats0, "\xc3\xe5\xf0\xee\xe9\x2f\xcf\xe0\xf0\xe0\xec\xe5\xf2\xf0\xfb"), /* Навыки */ DATA_COMPGEN(0x0051181c, cOverviewTextSkills1, "\xcd\xe0\xe2\xfb\xea\xe8"), /* Артефакты */ DATA_COMPGEN(0x00511824, cOverviewTextArtifacts2, "\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\xfb"), /* Города/Замки */ DATA_COMPGEN(0x00511830, cOverviewTextTownCastle3, "\xc3\xee\xf0\xee\xe4\xe0\x2f\xc7\xe0\xec\xea\xe8"), /* Гарнизон */ DATA_COMPGEN(0x00511840, cOverviewTextGarrison4, "\xc3\xe0\xf0\xed\xe8\xe7\xee\xed"), /* Доступно */ DATA_COMPGEN(0x0051184c, cOverviewTextAvailable5, "\xc4\xee\xf1\xf2\xf3\xef\xed\xee")};
DATA(0x004ff0c0) char* cWinComError[KB_WIN_COM_ERROR_TEXT_COUNT] = {
    /* Ошибка передачи данных при выполнении функции %s\n\nКод ошибки: %d\nЗначение ошибки: %s\n\n */ DATA_COMPGEN(0x00511858, cWinComErrorCommunicationsErrorOnFunctionFormatWin95ErrorCode0,
        "\xce\xf8\xe8\xe1\xea\xe0\x20\xef\xe5\xf0\xe5\xe4\xe0\xf7\xe8\x20\xe4\xe0\xed\xed\xfb\xf5\x20\xef\xf0\xe8\x20\xe2\xfb\xef\xee\xeb\xed\xe5\xed\xe8\xe8\x20\xf4\xf3\xed\xea\xf6\xe8\xe8\x20\x25\x73"
        "\x0a\x0a\xca\xee\xe4\x20\xee\xf8\xe8\xe1\xea\xe8\x3a\x20\x25\x64\x0a\xc7\xed\xe0\xf7\xe5\xed\xe8\xe5\x20\xee\xf8\xe8\xe1\xea\xe8\x3a\x20\x25\x73\x0a\x0a"),
    /* Предлагаемые меры устранения ошибки: */ DATA_COMPGEN(0x005118b0, cWinComErrorSuggestedSolutions1, "\xcf\xf0\xe5\xe4\xeb\xe0\xe3\xe0\xe5\xec\xfb\xe5\x20\xec\xe5\xf0\xfb\x20\xf3\xf1\xf2\xf0\xe0\xed\xe5\xed\xe8\xff\x20\xee\xf8\xe8\xe1\xea\xe8\x3a"),
    /* \n1) Убедитесь в надежности подсоединения кабелей. */ DATA_COMPGEN(0x005118d8, cWinComError1MakeSureAllCablesAreFirmlyConnected2,
        "\x0a\x31\x29\x20\xd3\xe1\xe5\xe4\xe8\xf2\xe5\xf1\xfc\x20\xe2\x20\xed\xe0\xe4\xe5\xe6\xed\xee\xf1\xf2\xe8\x20\xef\xee\xe4\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xff\x20\xea\xe0\xe1\xe5\xeb\xe5\xe9"
        "\x2e"),
    /* \n2) Перезагрузите компьютер. */ DATA_COMPGEN(0x0051190c, cWinComError2RebootComputer3, "\x0a\x32\x29\x20\xcf\xe5\xf0\xe5\xe7\xe0\xe3\xf0\xf3\xe7\xe8\xf2\xe5\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\x2e"),
    /* \n3) Убедитесь в том, что в 'CONFIG' задан правильный COM порт. (Третья кнопка на экране, где вы выбираете Хозяина или Гостя.) */ DATA_COMPGEN(0x0051192c, cWinComError3CheckToMakeSureYouHaveThe4,
        "\x0a\x33\x29\x20\xd3\xe1\xe5\xe4\xe8\xf2\xe5\xf1\xfc\x20\xe2\x20\xf2\xee\xec\x2c\x20\xf7\xf2\xee\x20\xe2\x20\x27\x43\x4f\x4e\x46\x49\x47\x27\x20\xe7\xe0\xe4\xe0\xed\x20\xef\xf0\xe0\xe2\xe8\xeb"
        "\xfc\xed\xfb\xe9\x20\x43\x4f\x4d\x20\xef\xee\xf0\xf2\x2e\x20\x28\xd2\xf0\xe5\xf2\xfc\xff\x20\xea\xed\xee\xef\xea\xe0\x20\xed\xe0\x20\xfd\xea\xf0\xe0\xed\xe5\x2c\x20\xe3\xe4\xe5\x20\xe2\xfb\x20"
        "\xe2\xfb\xe1\xe8\xf0\xe0\xe5\xf2\xe5\x20\xd5\xee\xe7\xff\xe8\xed\xe0\x20\xe8\xeb\xe8\x20\xc3\xee\xf1\xf2\xff\x2e\x29"),
    /* \n4) Попробуйте уменьшить скорость передачи данных в 'CONFIG' до 19200 или до 9600. */ DATA_COMPGEN(0x005119ac, cWinComError4ConsiderLoweringTheBAUDRateInCONFIG5,
        "\x0a\x34\x29\x20\xcf\xee\xef\xf0\xee\xe1\xf3\xe9\xf2\xe5\x20\xf3\xec\xe5\xed\xfc\xf8\xe8\xf2\xfc\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xfc\x20\xef\xe5\xf0\xe5\xe4\xe0\xf7\xe8\x20\xe4\xe0\xed\xed\xfb"
        "\xf5\x20\xe2\x20\x27\x43\x4f\x4e\x46\x49\x47\x27\x20\xe4\xee\x20\x31\x39\x32\x30\x30\x20\xe8\xeb\xe8\x20\xe4\xee\x20\x39\x36\x30\x30\x2e")
};
DATA(0x004ff0d8) char* cMiniViewText[KB_MINI_VIEW_TEXT_COUNT] =
    {/* %d воинов */ DATA_COMPGEN(0x00511a00, cMiniViewTextFormatUnits0, "\x25\x64\x20\xe2\xee\xe8\xed\xee\xe2"), /* %d воин */ DATA_COMPGEN(0x00511a0c, cMiniViewTextFormatUnit1, "\x25\x64\x20\xe2\xee\xe8\xed"), /* Атака */ DATA_COMPGEN(0x00511a14, cMiniViewTextAttack2, "\xc0\xf2\xe0\xea\xe0"), /* Защита */ DATA_COMPGEN(0x00511a1c, cMiniViewTextDefense3, "\xc7\xe0\xf9\xe8\xf2\xe0"), /* ЗД */ DATA_COMPGEN(0x00511a24, cMiniViewTextHP4, "\xc7\xc4"), /* Урон */ DATA_COMPGEN(0x00511a28, cMiniViewTextDmg5, "\xd3\xf0\xee\xed"), /* МР */ DATA_COMPGEN(0x00511a30, cMiniViewTextMrl6, "\xcc\xd0"), /* УЧ */ DATA_COMPGEN(0x00511a34, cMiniViewTextLuk7, "\xd3\xd7"), /* Выстр. */ DATA_COMPGEN(0x00511a38, cMiniViewTextShots8, "\xc2\xfb\xf1\xf2\xf0\x2e")};
DATA(0x004ff0fc) char* gFileRequestHelp[KB_FILE_REQUEST_HELP_COUNT] = {
    /* {Маленькие карты}\n\nПросмотр только маленьких карт (36 x 36). */ DATA_COMPGEN(0x00511a40, gFileRequestHelpSmallMapsViewOnlyMapsOfSizeSmall0,
        "\x7b\xcc\xe0\xeb\xe5\xed\xfc\xea\xe8\xe5\x20\xea\xe0\xf0\xf2\xfb\x7d\x0a\x0a\xcf\xf0\xee\xf1\xec\xee\xf2\xf0\x20\xf2\xee\xeb\xfc\xea\xee\x20\xec\xe0\xeb\xe5\xed\xfc\xea\xe8\xf5\x20\xea\xe0\xf0"
        "\xf2\x20\x28\x33\x36\x20\x78\x20\x33\x36\x29\x2e"),
    /* {Средние карты}\n\nПросмотр только средних карт (72 x 72). */ DATA_COMPGEN(0x00511a80, gFileRequestHelpMediumMapsViewOnlyMapsOfSizeMedium1,
        "\x7b\xd1\xf0\xe5\xe4\xed\xe8\xe5\x20\xea\xe0\xf0\xf2\xfb\x7d\x0a\x0a\xcf\xf0\xee\xf1\xec\xee\xf2\xf0\x20\xf2\xee\xeb\xfc\xea\xee\x20\xf1\xf0\xe5\xe4\xed\xe8\xf5\x20\xea\xe0\xf0\xf2\x20\x28\x37"
        "\x32\x20\x78\x20\x37\x32\x29\x2e"),
    /* {Большие карты}\n\nПросмотр только больших карт (108 x 108). */ DATA_COMPGEN(0x00511abc, gFileRequestHelpLargeMapsViewOnlyMapsOfSizeLarge2,
        "\x7b\xc1\xee\xeb\xfc\xf8\xe8\xe5\x20\xea\xe0\xf0\xf2\xfb\x7d\x0a\x0a\xcf\xf0\xee\xf1\xec\xee\xf2\xf0\x20\xf2\xee\xeb\xfc\xea\xee\x20\xe1\xee\xeb\xfc\xf8\xe8\xf5\x20\xea\xe0\xf0\xf2\x20\x28\x31"
        "\x30\x38\x20\x78\x20\x31\x30\x38\x29\x2e"),
    /* {Очень большие карты}\n\nПросмотр только очень больших карт (144 x 144). */ DATA_COMPGEN(0x00511af8, gFileRequestHelpExtraLargeMapsViewOnlyMapsOfSize3,
        "\x7b\xce\xf7\xe5\xed\xfc\x20\xe1\xee\xeb\xfc\xf8\xe8\xe5\x20\xea\xe0\xf0\xf2\xfb\x7d\x0a\x0a\xcf\xf0\xee\xf1\xec\xee\xf2\xf0\x20\xf2\xee\xeb\xfc\xea\xee\x20\xee\xf7\xe5\xed\xfc\x20\xe1\xee\xeb"
        "\xfc\xf8\xe8\xf5\x20\xea\xe0\xf0\xf2\x20\x28\x31\x34\x34\x20\x78\x20\x31\x34\x34\x29\x2e"),
    /* {Все карты}\n\nПросмотр всех карт. */ DATA_COMPGEN(0x00511b40, gFileRequestHelpAllMapsViewAllMapsRegardlessOfSize4, "\x7b\xc2\xf1\xe5\x20\xea\xe0\xf0\xf2\xfb\x7d\x0a\x0a\xcf\xf0\xee\xf1\xec\xee\xf2\xf0\x20\xe2\xf1\xe5\xf5\x20\xea\xe0\xf0\xf2\x2e"),
    /* {Ввод имени}\n\nВведите имя файла, под которым выхотите сохранить игру. */ DATA_COMPGEN(0x00511b64, gFileRequestHelpEnterNameEnterTheNameOfTheFile5,
        "\x7b\xc2\xe2\xee\xe4\x20\xe8\xec\xe5\xed\xe8\x7d\x0a\x0a\xc2\xe2\xe5\xe4\xe8\xf2\xe5\x20\xe8\xec\xff\x20\xf4\xe0\xe9\xeb\xe0\x2c\x20\xef\xee\xe4\x20\xea\xee\xf2\xee\xf0\xfb\xec\x20\xe2\xfb\xf5"
        "\xee\xf2\xe8\xf2\xe5\x20\xf1\xee\xf5\xf0\xe0\xed\xe8\xf2\xfc\x20\xe8\xe3\xf0\xf3\x2e"),
    /* {ОК}\n\nПодтверждение выбора. */ DATA_COMPGEN(0x00511bac, gFileRequestHelpOKAcceptTheChoiceMade6, "\x7b\xce\xca\x7d\x0a\x0a\xcf\xee\xe4\xf2\xe2\xe5\xf0\xe6\xe4\xe5\xed\xe8\xe5\x20\xe2\xfb\xe1\xee\xf0\xe0\x2e"),
    /* {Отмена}\n\nОтмена без подтверждения выбора. */ DATA_COMPGEN(0x00511bc8, gFileRequestHelpCancelCancelWithoutMakingAChoice7, "\x7b\xce\xf2\xec\xe5\xed\xe0\x7d\x0a\x0a\xce\xf2\xec\xe5\xed\xe0\x20\xe1\xe5\xe7\x20\xef\xee\xe4\xf2\xe2\xe5\xf0\xe6\xe4\xe5\xed\xe8\xff\x20\xe2\xfb\xe1\xee\xf0\xe0\x2e"),
    /* {Значок размера}\n\nОбозначает размер карты: маленькая (36 x 36), средняя (72 x 72), большая (108 x 108) или очень большая (144 x 144). */ DATA_COMPGEN(0x00511bf4, gFileRequestHelpSizeIconIndicatesWhetherTheMapIsSmall8,
        "\x7b\xc7\xed\xe0\xf7\xee\xea\x20\xf0\xe0\xe7\xec\xe5\xf0\xe0\x7d\x0a\x0a\xce\xe1\xee\xe7\xed\xe0\xf7\xe0\xe5\xf2\x20\xf0\xe0\xe7\xec\xe5\xf0\x20\xea\xe0\xf0\xf2\xfb\x3a\x20\xec\xe0\xeb\xe5\xed"
        "\xfc\xea\xe0\xff\x20\x28\x33\x36\x20\x78\x20\x33\x36\x29\x2c\x20\xf1\xf0\xe5\xe4\xed\xff\xff\x20\x28\x37\x32\x20\x78\x20\x37\x32\x29\x2c\x20\xe1\xee\xeb\xfc\xf8\xe0\xff\x20\x28\x31\x30\x38\x20"
        "\x78\x20\x31\x30\x38\x29\x20\xe8\xeb\xe8\x20\xee\xf7\xe5\xed\xfc\x20\xe1\xee\xeb\xfc\xf8\xe0\xff\x20\x28\x31\x34\x34\x20\x78\x20\x31\x34\x34\x29\x2e"),
    /* {Значок игроков}\n\nОбозначает количество игроков в данном сценарии. При отсутствии игроков-людей их места занимает компьютер. */ DATA_COMPGEN(0x00511c7c, gFileRequestHelpPlayersIconIndicatesHowManyPlayersTotalAre9,
        "\x7b\xc7\xed\xe0\xf7\xee\xea\x20\xe8\xe3\xf0\xee\xea\xee\xe2\x7d\x0a\x0a\xce\xe1\xee\xe7\xed\xe0\xf7\xe0\xe5\xf2\x20\xea\xee\xeb\xe8\xf7\xe5\xf1\xf2\xe2\xee\x20\xe8\xe3\xf0\xee\xea\xee\xe2\x20"
        "\xe2\x20\xe4\xe0\xed\xed\xee\xec\x20\xf1\xf6\xe5\xed\xe0\xf0\xe8\xe8\x2e\x20\xcf\xf0\xe8\x20\xee\xf2\xf1\xf3\xf2\xf1\xf2\xe2\xe8\xe8\x20\xe8\xe3\xf0\xee\xea\xee\xe2\x2d\xeb\xfe\xe4\xe5\xe9\x20"
        "\xe8\xf5\x20\xec\xe5\xf1\xf2\xe0\x20\xe7\xe0\xed\xe8\xec\xe0\xe5\xf2\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\x2e"),
    /* {Условия победы}\nПредусмотрено 6 возможных вариантов:\n{Надгробный камень} - Разгромить всех героев противника и захватить его замки.\n{Город} - Захватить определенный замок.\n{Портрет героя} - Разгромить определенного героя.\n{Медаль} - Найти определенный артефакт.\n{Рукопожатие} - Ваш альянс должен разгромить альянс противника.\n{Монеты} - Накопить нужное количество золота. */ DATA_COMPGEN(0x00511cfc, gFileRequestHelpVictoryConditionIconThereAre6PossibilitiesTombstone10,
        "\x7b\xd3\xf1\xeb\xee\xe2\xe8\xff\x20\xef\xee\xe1\xe5\xe4\xfb\x7d\x0a\xcf\xf0\xe5\xe4\xf3\xf1\xec\xee\xf2\xf0\xe5\xed\xee\x20\x36\x20\xe2\xee\xe7\xec\xee\xe6\xed\xfb\xf5\x20\xe2\xe0\xf0\xe8\xe0"
        "\xed\xf2\xee\xe2\x3a\x0a\x7b\xcd\xe0\xe4\xe3\xf0\xee\xe1\xed\xfb\xe9\x20\xea\xe0\xec\xe5\xed\xfc\x7d\x20\x2d\x20\xd0\xe0\xe7\xe3\xf0\xee\xec\xe8\xf2\xfc\x20\xe2\xf1\xe5\xf5\x20\xe3\xe5\xf0\xee"
        "\xe5\xe2\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x20\xe8\x20\xe7\xe0\xf5\xe2\xe0\xf2\xe8\xf2\xfc\x20\xe5\xe3\xee\x20\xe7\xe0\xec\xea\xe8\x2e\x0a\x7b\xc3\xee\xf0\xee\xe4\x7d\x20\x2d\x20\xc7"
        "\xe0\xf5\xe2\xe0\xf2\xe8\xf2\xfc\x20\xee\xef\xf0\xe5\xe4\xe5\xeb\xe5\xed\xed\xfb\xe9\x20\xe7\xe0\xec\xee\xea\x2e\x0a\x7b\xcf\xee\xf0\xf2\xf0\xe5\xf2\x20\xe3\xe5\xf0\xee\xff\x7d\x20\x2d\x20\xd0"
        "\xe0\xe7\xe3\xf0\xee\xec\xe8\xf2\xfc\x20\xee\xef\xf0\xe5\xe4\xe5\xeb\xe5\xed\xed\xee\xe3\xee\x20\xe3\xe5\xf0\xee\xff\x2e\x0a\x7b\xcc\xe5\xe4\xe0\xeb\xfc\x7d\x20\x2d\x20\xcd\xe0\xe9\xf2\xe8\x20"
        "\xee\xef\xf0\xe5\xe4\xe5\xeb\xe5\xed\xed\xfb\xe9\x20\xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2\x2e\x0a\x7b\xd0\xf3\xea\xee\xef\xee\xe6\xe0\xf2\xe8\xe5\x7d\x20\x2d\x20\xc2\xe0\xf8\x20\xe0\xeb\xfc\xff\xed"
        "\xf1\x20\xe4\xee\xeb\xe6\xe5\xed\x20\xf0\xe0\xe7\xe3\xf0\xee\xec\xe8\xf2\xfc\x20\xe0\xeb\xfc\xff\xed\xf1\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e\x0a\x7b\xcc\xee\xed\xe5\xf2\xfb\x7d\x20"
        "\x2d\x20\xcd\xe0\xea\xee\xef\xe8\xf2\xfc\x20\xed\xf3\xe6\xed\xee\xe5\x20\xea\xee\xeb\xe8\xf7\xe5\xf1\xf2\xe2\xee\x20\xe7\xee\xeb\xee\xf2\xe0\x2e"),
    /* {Уссловия поражения}\n\nПредусмотрено 4 возможных условия:\n{Надгробный камень} - Потеря всех ваших героев и городов.\n{Город} - Потеря определенного замка.\n{Портрет героя} - Потеря указанного героя.\n{Песочные часы} - Победа не была достигнута до указанного срока.) */ DATA_COMPGEN(0x00511e74, gFileRequestHelpLossConditionIconThereAre4PossibleLoss11,
        "\x7b\xd3\xf1\xf1\xeb\xee\xe2\xe8\xff\x20\xef\xee\xf0\xe0\xe6\xe5\xed\xe8\xff\x7d\x0a\x0a\xcf\xf0\xe5\xe4\xf3\xf1\xec\xee\xf2\xf0\xe5\xed\xee\x20\x34\x20\xe2\xee\xe7\xec\xee\xe6\xed\xfb\xf5\x20"
        "\xf3\xf1\xeb\xee\xe2\xe8\xff\x3a\x0a\x7b\xcd\xe0\xe4\xe3\xf0\xee\xe1\xed\xfb\xe9\x20\xea\xe0\xec\xe5\xed\xfc\x7d\x20\x2d\x20\xcf\xee\xf2\xe5\xf0\xff\x20\xe2\xf1\xe5\xf5\x20\xe2\xe0\xf8\xe8\xf5"
        "\x20\xe3\xe5\xf0\xee\xe5\xe2\x20\xe8\x20\xe3\xee\xf0\xee\xe4\xee\xe2\x2e\x0a\x7b\xc3\xee\xf0\xee\xe4\x7d\x20\x2d\x20\xcf\xee\xf2\xe5\xf0\xff\x20\xee\xef\xf0\xe5\xe4\xe5\xeb\xe5\xed\xed\xee\xe3"
        "\xee\x20\xe7\xe0\xec\xea\xe0\x2e\x0a\x7b\xcf\xee\xf0\xf2\xf0\xe5\xf2\x20\xe3\xe5\xf0\xee\xff\x7d\x20\x2d\x20\xcf\xee\xf2\xe5\xf0\xff\x20\xf3\xea\xe0\xe7\xe0\xed\xed\xee\xe3\xee\x20\xe3\xe5\xf0"
        "\xee\xff\x2e\x0a\x7b\xcf\xe5\xf1\xee\xf7\xed\xfb\xe5\x20\xf7\xe0\xf1\xfb\x7d\x20\x2d\x20\xcf\xee\xe1\xe5\xe4\xe0\x20\xed\xe5\x20\xe1\xfb\xeb\xe0\x20\xe4\xee\xf1\xf2\xe8\xe3\xed\xf3\xf2\xe0\x20"
        "\xe4\xee\x20\xf3\xea\xe0\xe7\xe0\xed\xed\xee\xe3\xee\x20\xf1\xf0\xee\xea\xe0\x2e\x29"),
    /* {Название}\n\nНазвание карты. */ DATA_COMPGEN(0x00511f7c, gFileRequestHelpSelectedNameTheNameOfTheCurrentlySelected12, "\x7b\xcd\xe0\xe7\xe2\xe0\xed\xe8\xe5\x7d\x0a\x0a\xcd\xe0\xe7\xe2\xe0\xed\xe8\xe5\x20\xea\xe0\xf0\xf2\xfb\x2e"),
    /* {Описание}\n\nОписание карты. */ DATA_COMPGEN(0x00511f98, gFileRequestHelpSelectedDescriptionTheDescriptionOfTheCurrentlySelected13, "\x7b\xce\xef\xe8\xf1\xe0\xed\xe8\xe5\x7d\x0a\x0a\xce\xef\xe8\xf1\xe0\xed\xe8\xe5\x20\xea\xe0\xf0\xf2\xfb\x2e"),
    /* {Трудность карты}\n\nСтепень сложности игры на этой карте. Трудность карты определяется разработчиком сценария. Более сложные карты характеризуются большим числом сильных противников, меньшим количеством ресурсов или специальными условиями, затрудняющими достижение победы. */ DATA_COMPGEN(0x00511fb4, gFileRequestHelpSelectedMapDifficultyTheMapDifficultyOfThe14,
        "\x7b\xd2\xf0\xf3\xe4\xed\xee\xf1\xf2\xfc\x20\xea\xe0\xf0\xf2\xfb\x7d\x0a\x0a\xd1\xf2\xe5\xef\xe5\xed\xfc\x20\xf1\xeb\xee\xe6\xed\xee\xf1\xf2\xe8\x20\xe8\xe3\xf0\xfb\x20\xed\xe0\x20\xfd\xf2\xee"
        "\xe9\x20\xea\xe0\xf0\xf2\xe5\x2e\x20\xd2\xf0\xf3\xe4\xed\xee\xf1\xf2\xfc\x20\xea\xe0\xf0\xf2\xfb\x20\xee\xef\xf0\xe5\xe4\xe5\xeb\xff\xe5\xf2\xf1\xff\x20\xf0\xe0\xe7\xf0\xe0\xe1\xee\xf2\xf7\xe8"
        "\xea\xee\xec\x20\xf1\xf6\xe5\xed\xe0\xf0\xe8\xff\x2e\x20\xc1\xee\xeb\xe5\xe5\x20\xf1\xeb\xee\xe6\xed\xfb\xe5\x20\xea\xe0\xf0\xf2\xfb\x20\xf5\xe0\xf0\xe0\xea\xf2\xe5\xf0\xe8\xe7\xf3\xfe\xf2\xf1"
        "\xff\x20\xe1\xee\xeb\xfc\xf8\xe8\xec\x20\xf7\xe8\xf1\xeb\xee\xec\x20\xf1\xe8\xeb\xfc\xed\xfb\xf5\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xee\xe2\x2c\x20\xec\xe5\xed\xfc\xf8\xe8\xec\x20\xea\xee"
        "\xeb\xe8\xf7\xe5\xf1\xf2\xe2\xee\xec\x20\xf0\xe5\xf1\xf3\xf0\xf1\xee\xe2\x20\xe8\xeb\xe8\x20\xf1\xef\xe5\xf6\xe8\xe0\xeb\xfc\xed\xfb\xec\xe8\x20\xf3\xf1\xeb\xee\xe2\xe8\xff\xec\xe8\x2c\x20\xe7"
        "\xe0\xf2\xf0\xf3\xe4\xed\xff\xfe\xf9\xe8\xec\xe8\x20\xe4\xee\xf1\xf2\xe8\xe6\xe5\xed\xe8\xe5\x20\xef\xee\xe1\xe5\xe4\xfb\x2e")
};
DATA(0x004ff138) char* cPersonality[KB_PERSONALITY_TEXT_COUNT] = {/* Воин */ DATA_COMPGEN(0x005120c4, cPersonalityWarrior0, "\xc2\xee\xe8\xed"), /* Строитель */ DATA_COMPGEN(0x005120cc, cPersonalityBuilder1, "\xd1\xf2\xf0\xee\xe8\xf2\xe5\xeb\xfc"), /* Исследователь */ DATA_COMPGEN(0x005120d8, cPersonalityExplorer2, "\xc8\xf1\xf1\xeb\xe5\xe4\xee\xe2\xe0\xf2\xe5\xeb\xfc"), /* Человек */ DATA_COMPGEN(0x005120e8, cPersonalityHuman3, "\xd7\xe5\xeb\xee\xe2\xe5\xea")};
DATA(0x004ff148) char* gArmySizeNames[KB_ARMY_SIZE_NAME_COUNT][KB_ARMY_SIZE_NAME_VARIANT_COUNT] = {
    {/* Мало */ DATA_COMPGEN(0x005120f0, gArmySizeNamesFew0, "\xcc\xe0\xeb\xee"), /* Мало */ DATA_COMPGEN(0x005120f8, gArmySizeNamesAFew1, "\xcc\xe0\xeb\xee"), /* мало */ DATA_COMPGEN(0x00512100, gArmySizeNamesAFew2, "\xec\xe0\xeb\xee")},
    {/* Немного */ DATA_COMPGEN(0x00512108, gArmySizeNamesSeveral3, "\xcd\xe5\xec\xed\xee\xe3\xee"), /* Немного */ DATA_COMPGEN(0x00512110, gArmySizeNamesSeveral4, "\xcd\xe5\xec\xed\xee\xe3\xee"), /* немного */ DATA_COMPGEN(0x00512118, gArmySizeNamesSeveral5, "\xed\xe5\xec\xed\xee\xe3\xee")},
    {/* Стая */ DATA_COMPGEN(0x00512120, gArmySizeNamesPack6, "\xd1\xf2\xe0\xff"), /* Стая */ DATA_COMPGEN(0x00512128, gArmySizeNamesAPackOf7, "\xd1\xf2\xe0\xff"), /* стая */ DATA_COMPGEN(0x00512130, gArmySizeNamesAPackOf8, "\xf1\xf2\xe0\xff")},
    {/* Много */ DATA_COMPGEN(0x00512138, gArmySizeNamesLots9, "\xcc\xed\xee\xe3\xee"), /* Много */ DATA_COMPGEN(0x00512140, gArmySizeNamesLotsOf10, "\xcc\xed\xee\xe3\xee"), /* много */ DATA_COMPGEN(0x00512148, gArmySizeNamesLotsOf11, "\xec\xed\xee\xe3\xee")},
    {/* Орда */ DATA_COMPGEN(0x00512150, gArmySizeNamesHorde12, "\xce\xf0\xe4\xe0"), /* Орда */ DATA_COMPGEN(0x00512158, gArmySizeNamesAHordeOf13, "\xce\xf0\xe4\xe0"), /* орда */ DATA_COMPGEN(0x00512160, gArmySizeNamesAHordeOf14, "\xee\xf0\xe4\xe0")},
    {/* Толпа */ DATA_COMPGEN(0x00512168, gArmySizeNamesThrong15, "\xd2\xee\xeb\xef\xe0"), /* Толпа */ DATA_COMPGEN(0x00512170, gArmySizeNamesAThrongOf16, "\xd2\xee\xeb\xef\xe0"), /* толпа */ DATA_COMPGEN(0x00512178, gArmySizeNamesAThrongOf17, "\xf2\xee\xeb\xef\xe0")},
    {/* Свора */ DATA_COMPGEN(0x00512180, gArmySizeNamesSwarm18, "\xd1\xe2\xee\xf0\xe0"), /* Свора */ DATA_COMPGEN(0x00512188, gArmySizeNamesASwarmOf19, "\xd1\xe2\xee\xf0\xe0"), /* свора */ DATA_COMPGEN(0x00512190, gArmySizeNamesASwarmOf20, "\xf1\xe2\xee\xf0\xe0")},
    {/* Тысячи */ DATA_COMPGEN(0x00512198, gArmySizeNamesZounds21, "\xd2\xfb\xf1\xff\xf7\xe8"), /* Тысячи... */ DATA_COMPGEN(0x005121a0, gArmySizeNamesZounds22, "\xd2\xfb\xf1\xff\xf7\xe8\x2e\x2e\x2e"), /* тысячи */ DATA_COMPGEN(0x005121ac, gArmySizeNamesZoundsOf23, "\xf2\xfb\xf1\xff\xf7\xe8")},
    {/* Легион */ DATA_COMPGEN(0x005121b4, gArmySizeNamesLegion24, "\xcb\xe5\xe3\xe8\xee\xed"), /* Легион */ DATA_COMPGEN(0x005121bc, gArmySizeNamesALegionOf25, "\xcb\xe5\xe3\xe8\xee\xed"), /* легион */ DATA_COMPGEN(0x005121c4, gArmySizeNamesALegionOf26, "\xeb\xe5\xe3\xe8\xee\xed")}
};
DATA(0x004ff1b4) char* cRandomTavernText[KB_RANDOM_TAVERN_TEXT_COUNT] = {
    /* Истина где-то рядом. */ DATA_COMPGEN(0x005121cc, cRandomTavernTextTheTruthIsOutThere0, "\xc8\xf1\xf2\xe8\xed\xe0\x20\xe3\xe4\xe5\x2d\xf2\xee\x20\xf0\xff\xe4\xee\xec\x2e"),
    /* Темная сторона сильнее. */ DATA_COMPGEN(0x005121e4, cRandomTavernTextTheDarkSideIsStronger1, "\xd2\xe5\xec\xed\xe0\xff\x20\xf1\xf2\xee\xf0\xee\xed\xe0\x20\xf1\xe8\xeb\xfc\xed\xe5\xe5\x2e"),
    /* Конец Света близок. */ DATA_COMPGEN(0x005121fc, cRandomTavernTextTheEndOfTheWorldIsNear2, "\xca\xee\xed\xe5\xf6\x20\xd1\xe2\xe5\xf2\xe0\x20\xe1\xeb\xe8\xe7\xee\xea\x2e"),
    /* Прах Лорда Слэйера захоронен в основании арены. */ DATA_COMPGEN(0x00512210, cRandomTavernTextTheBonesOfLordSlayerAreBuriedIn3, "\xcf\xf0\xe0\xf5\x20\xcb\xee\xf0\xe4\xe0\x20\xd1\xeb\xfd\xe9\xe5\xf0\xe0\x20\xe7\xe0\xf5\xee\xf0\xee\xed\xe5\xed\x20\xe2\x20\xee\xf1\xed\xee\xe2\xe0\xed\xe8\xe8\x20\xe0\xf0\xe5\xed\xfb\x2e"),
    /* Он невиновен. */ DATA_COMPGEN(0x00512240, cRandomTavernTextHeSInnocent4, "\xce\xed\x20\xed\xe5\xe2\xe8\xed\xee\xe2\xe5\xed\x2e"),
    /* Черный дракон сделает Титана в любой день недели. */ DATA_COMPGEN(0x00512250, cRandomTavernTextABlackDragonWillTakeOutATitan5,
        "\xd7\xe5\xf0\xed\xfb\xe9\x20\xe4\xf0\xe0\xea\xee\xed\x20\xf1\xe4\xe5\xeb\xe0\xe5\xf2\x20\xd2\xe8\xf2\xe0\xed\xe0\x20\xe2\x20\xeb\xfe\xe1\xee\xe9\x20\xe4\xe5\xed\xfc\x20\xed\xe5\xe4\xe5\xeb\xe8"
        "\x2e"),
    /* Он сказал ей, "Я-да-да-яда-да"... а она сказала, "Ля-ля-ля, ля-ля-ля..." */ DATA_COMPGEN(0x00512284, cRandomTavernTextHeToldHerYadaYadaYadaAndThen6,
        "\xce\xed\x20\xf1\xea\xe0\xe7\xe0\xeb\x20\xe5\xe9\x2c\x20\x22\xdf\x2d\xe4\xe0\x2d\xe4\xe0\x2d\xff\xe4\xe0\x2d\xe4\xe0\x22\x2e\x2e\x2e\x20\xe0\x20\xee\xed\xe0\x20\xf1\xea\xe0\xe7\xe0\xeb\xe0\x2c"
        "\x20\x22\xcb\xff\x2d\xeb\xff\x2d\xeb\xff\x2c\x20\xeb\xff\x2d\xeb\xff\x2d\xeb\xff\x2e\x2e\x2e\x22"),
    /* Тут бывал человек из Нунтукета... */ DATA_COMPGEN(0x005122d0, cRandomTavernTextThereOnceWasAManFromNantucket7, "\xd2\xf3\xf2\x20\xe1\xfb\xe2\xe0\xeb\x20\xf7\xe5\xeb\xee\xe2\xe5\xea\x20\xe8\xe7\x20\xcd\xf3\xed\xf2\xf3\xea\xe5\xf2\xe0\x2e\x2e\x2e")
};
DATA(0x004ff1d4) char* cRandomSignText[KB_RANDOM_SIGN_TEXT_COUNT] =
    {/* Прямо пойдешь - коня потеряешь. */ DATA_COMPGEN(0x005122f4, cRandomSignTextSeeRockCity0, "\xcf\xf0\xff\xec\xee\x20\xef\xee\xe9\xe4\xe5\xf8\xfc\x20\x2d\x20\xea\xee\xed\xff\x20\xef\xee\xf2\xe5\xf0\xff\xe5\xf8\xfc\x2e"), /* Сдается в аренду. */ DATA_COMPGEN(0x00512314, cRandomSignTextThisSpaceForRent1, "\xd1\xe4\xe0\xe5\xf2\xf1\xff\x20\xe2\x20\xe0\xf0\xe5\xed\xe4\xf3\x2e"), /* До следующего знака 50 миль. */ DATA_COMPGEN(0x00512328, cRandomSignTextNextSign50Miles2, "\xc4\xee\x20\xf1\xeb\xe5\xe4\xf3\xfe\xf9\xe5\xe3\xee\x20\xe7\xed\xe0\xea\xe0\x20\x35\x30\x20\xec\xe8\xeb\xfc\x2e"), /* Кто идет за Блинским? */ DATA_COMPGEN(0x00512348, cRandomSignTextBurmaShave3, "\xca\xf2\xee\x20\xe8\xe4\xe5\xf2\x20\xe7\xe0\x20\xc1\xeb\xe8\xed\xf1\xea\xe8\xec\x3f")};
DATA(0x004ff1e4) char* cCampaignAwards[KB_CAMPAIGN_AWARD_TEXT_COUNT] = {
    /* Альянс гномов */ DATA_COMPGEN(0x00512360, cCampaignAwardsDwarvenAlliance0, "\xc0\xeb\xfc\xff\xed\xf1\x20\xe3\xed\xee\xec\xee\xe2"),
    /* Гильдия колдуний */ DATA_COMPGEN(0x00512370, cCampaignAwardsSorceressGuild1, "\xc3\xe8\xeb\xfc\xe4\xe8\xff\x20\xea\xee\xeb\xe4\xf3\xed\xe8\xe9"),
    /* Роланд становится сильнее */ DATA_COMPGEN(0x00512384, cCampaignAwardsRolandStrengthened2, "\xd0\xee\xeb\xe0\xed\xe4\x20\xf1\xf2\xe0\xed\xee\xe2\xe8\xf2\xf1\xff\x20\xf1\xe8\xeb\xfc\xed\xe5\xe5"),
    /* Перенос войск */ DATA_COMPGEN(0x005123a0, cCampaignAwardsCarryOverForces3, "\xcf\xe5\xf0\xe5\xed\xee\xf1\x20\xe2\xee\xe9\xf1\xea"),
    /* Корлагон побежден */ DATA_COMPGEN(0x005123b0, cCampaignAwardsCorlagonDefeated4, "\xca\xee\xf0\xeb\xe0\xe3\xee\xed\x20\xef\xee\xe1\xe5\xe6\xe4\xe5\xed"),
    /* Корона всевластия */ DATA_COMPGEN(0x005123c4, cCampaignAwardsUltimateCrown5, "\xca\xee\xf0\xee\xed\xe0\x20\xe2\xf1\xe5\xe2\xeb\xe0\xf1\xf2\xe8\xff"),
    /* Гильдия некромантов */ DATA_COMPGEN(0x005123d8, cCampaignAwardsNecromancerGuild6, "\xc3\xe8\xeb\xfc\xe4\xe8\xff\x20\xed\xe5\xea\xf0\xee\xec\xe0\xed\xf2\xee\xe2"),
    /* Смерть гномам */ DATA_COMPGEN(0x005123ec, cCampaignAwardsDwarfBane7, "\xd1\xec\xe5\xf0\xf2\xfc\x20\xe3\xed\xee\xec\xe0\xec"),
    /* Союз огров */ DATA_COMPGEN(0x005123fc, cCampaignAwardsOgreAlliance8, "\xd1\xee\xfe\xe7\x20\xee\xe3\xf0\xee\xe2"),
    /* Союз драконов */ DATA_COMPGEN(0x00512408, cCampaignAwardsDragonAlliance9, "\xd1\xee\xfe\xe7\x20\xe4\xf0\xe0\xea\xee\xed\xee\xe2"),
    /* Корона всевластия */ DATA_COMPGEN(0x00512418, cCampaignAwardsUltimateCrown10, "\xca\xee\xf0\xee\xed\xe0\x20\xe2\xf1\xe5\xe2\xeb\xe0\xf1\xf2\xe8\xff"),
    /* Перенос войск */ DATA_COMPGEN(0x0051242c, cCampaignAwardsCarryOverForces11, "\xcf\xe5\xf0\xe5\xed\xee\xf1\x20\xe2\xee\xe9\xf1\xea")
};
DATA(0x004ff214) char* cCampaignName[IDX(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT] = {
    {/* Сила оружия */ DATA_COMPGEN(0x0051243c, cCampaignNameForceOfArms0, "\xd1\xe8\xeb\xe0\x20\xee\xf0\xf3\xe6\xe8\xff"),
     /* Аннексия */ DATA_COMPGEN(0x00512448, cCampaignNameAnnexation1, "\xc0\xed\xed\xe5\xea\xf1\xe8\xff"),
     /* Спасти гномов */ DATA_COMPGEN(0x00512454, cCampaignNameSaveTheDwarves2, "\xd1\xef\xe0\xf1\xf2\xe8\x20\xe3\xed\xee\xec\xee\xe2"),
     /* Копи Каратора */ DATA_COMPGEN(0x00512464, cCampaignNameCaratorMines3, "\xca\xee\xef\xe8\x20\xca\xe0\xf0\xe0\xf2\xee\xf0\xe0"),
     /* Переломный момент */ DATA_COMPGEN(0x00512474, cCampaignNameTurningPoint4, "\xcf\xe5\xf0\xe5\xeb\xee\xec\xed\xfb\xe9\x20\xec\xee\xec\xe5\xed\xf2"),
     /* Защитник */ DATA_COMPGEN(0x00512488, cCampaignNameDefender5, "\xc7\xe0\xf9\xe8\xf2\xed\xe8\xea"),
     /* Вызов брошен! */ DATA_COMPGEN(0x00512494, cCampaignNameTheGauntlet6, "\xc2\xfb\xe7\xee\xe2\x20\xe1\xf0\xee\xf8\xe5\xed\x21"),
     /* Корона */ DATA_COMPGEN(0x005124a4, cCampaignNameTheCrown7, "\xca\xee\xf0\xee\xed\xe0"),
     /* Акт отчаяния */ DATA_COMPGEN(0x005124ac, cCampaignNameCorlagonSDefense8, "\xc0\xea\xf2\x20\xee\xf2\xf7\xe0\xff\xed\xe8\xff"),
     /* Час нашей славы */ DATA_COMPGEN(0x005124bc, cCampaignNameFinalJustice9, "\xd7\xe0\xf1\x20\xed\xe0\xf8\xe5\xe9\x20\xf1\xeb\xe0\xe2\xfb"),
     /*  */ DATA_COMPGEN(0x00526850, cCampaignNameEmpty10, ""),
     /* Предательство */ DATA_COMPGEN(0x005124cc, cCampaignNameBetrayal11, "\xcf\xf0\xe5\xe4\xe0\xf2\xe5\xeb\xfc\xf1\xf2\xe2\xee")},
    {/* Первая кровь */ DATA_COMPGEN(0x005124dc, cCampaignNameFirstBlood12, "\xcf\xe5\xf0\xe2\xe0\xff\x20\xea\xf0\xee\xe2\xfc"),
     /* Войны с варварами */ DATA_COMPGEN(0x005124ec, cCampaignNameBarbarianWars13, "\xc2\xee\xe9\xed\xfb\x20\xf1\x20\xe2\xe0\xf0\xe2\xe0\xf0\xe0\xec\xe8"),
     /* Некроманты! */ DATA_COMPGEN(0x00512500, cCampaignNameNecromancers14, "\xcd\xe5\xea\xf0\xee\xec\xe0\xed\xf2\xfb\x21"),
     /* Смерть гномам */ DATA_COMPGEN(0x0051250c, cCampaignNameSlayTheDwarves15, "\xd1\xec\xe5\xf0\xf2\xfc\x20\xe3\xed\xee\xec\xe0\xec"),
     /* Переломный момент  */ DATA_COMPGEN(0x0051251c, cCampaignNameTurningPoint16, "\xcf\xe5\xf0\xe5\xeb\xee\xec\xed\xfb\xe9\x20\xec\xee\xec\xe5\xed\xf2\x20"),
     /* Крестьяне! */ DATA_COMPGEN(0x00512530, cCampaignNameRebellion17, "\xca\xf0\xe5\xf1\xf2\xfc\xff\xed\xe5\x21"),
     /* Владыка драконов */ DATA_COMPGEN(0x0051253c, cCampaignNameDragonMaster18, "\xc2\xeb\xe0\xe4\xfb\xea\xe0\x20\xe4\xf0\xe0\xea\xee\xed\xee\xe2"),
     /* Лорды провинций */ DATA_COMPGEN(0x00512550, cCampaignNameCountryLords19, "\xcb\xee\xf0\xe4\xfb\x20\xef\xf0\xee\xe2\xe8\xed\xf6\xe8\xe9"),
     /* Корона */ DATA_COMPGEN(0x00512560, cCampaignNameTheCrown20, "\xca\xee\xf0\xee\xed\xe0"),
     /* К вящей славе */ DATA_COMPGEN(0x00512568, cCampaignNameGreaterGlory21, "\xca\x20\xe2\xff\xf9\xe5\xe9\x20\xf1\xeb\xe0\xe2\xe5"),
     /* Апокалипсис */ DATA_COMPGEN(0x00512578, cCampaignNameApocalypse22, "\xc0\xef\xee\xea\xe0\xeb\xe8\xef\xf1\xe8\xf1"),
     /* Предательство! */ DATA_COMPGEN(0x00512584, cCampaignNameBetrayal23, "\xcf\xf0\xe5\xe4\xe0\xf2\xe5\xeb\xfc\xf1\xf2\xe2\xee\x21")}
};
DATA(0x004ff274) char* cCampaignDescription[IDX(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT] = {
    {/* Прежде чем поднять восстание против брата, Роланд хочет, чтобы вы одержали победу над соседними властителями. Между ними нет единства, поэтому большую часть времени они будут заняты стычками друг с другом. Победа будет вашей, когда вы захватите все города. */ DATA_COMPGEN(0x00512594, cCampaignDescriptionRolandNeedsYouToDefeatTheLordsNear0,
        "\xcf\xf0\xe5\xe6\xe4\xe5\x20\xf7\xe5\xec\x20\xef\xee\xe4\xed\xff\xf2\xfc\x20\xe2\xee\xf1\xf1\xf2\xe0\xed\xe8\xe5\x20\xef\xf0\xee\xf2\xe8\xe2\x20\xe1\xf0\xe0\xf2\xe0\x2c\x20\xd0\xee\xeb\xe0\xed"
        "\xe4\x20\xf5\xee\xf7\xe5\xf2\x2c\x20\xf7\xf2\xee\xe1\xfb\x20\xe2\xfb\x20\xee\xe4\xe5\xf0\xe6\xe0\xeb\xe8\x20\xef\xee\xe1\xe5\xe4\xf3\x20\xed\xe0\xe4\x20\xf1\xee\xf1\xe5\xe4\xed\xe8\xec\xe8\x20"
        "\xe2\xeb\xe0\xf1\xf2\xe8\xf2\xe5\xeb\xff\xec\xe8\x2e\x20\xcc\xe5\xe6\xe4\xf3\x20\xed\xe8\xec\xe8\x20\xed\xe5\xf2\x20\xe5\xe4\xe8\xed\xf1\xf2\xe2\xe0\x2c\x20\xef\xee\xfd\xf2\xee\xec\xf3\x20\xe1"
        "\xee\xeb\xfc\xf8\xf3\xfe\x20\xf7\xe0\xf1\xf2\xfc\x20\xe2\xf0\xe5\xec\xe5\xed\xe8\x20\xee\xed\xe8\x20\xe1\xf3\xe4\xf3\xf2\x20\xe7\xe0\xed\xff\xf2\xfb\x20\xf1\xf2\xfb\xf7\xea\xe0\xec\xe8\x20\xe4"
        "\xf0\xf3\xe3\x20\xf1\x20\xe4\xf0\xf3\xe3\xee\xec\x2e\x20\xcf\xee\xe1\xe5\xe4\xe0\x20\xe1\xf3\xe4\xe5\xf2\x20\xe2\xe0\xf8\xe5\xe9\x2c\x20\xea\xee\xe3\xe4\xe0\x20\xe2\xfb\x20\xe7\xe0\xf5\xe2\xe0"
        "\xf2\xe8\xf2\xe5\x20\xe2\xf1\xe5\x20\xe3\xee\xf0\xee\xe4\xe0\x2e"),
     /* Властители сопредельных земель отказываются принести клятву верности Роланду, и поэтому должны быть повержены. Богатства и власти им не занимать, поэтому будьте готовы к нелегкой борьбе. Чтобы победить, захватите все вражеские замки. */ DATA_COMPGEN(0x00512698, cCampaignDescriptionTheLocalLordsRefuseToSwearAllegianceTo1,
         "\xc2\xeb\xe0\xf1\xf2\xe8\xf2\xe5\xeb\xe8\x20\xf1\xee\xef\xf0\xe5\xe4\xe5\xeb\xfc\xed\xfb\xf5\x20\xe7\xe5\xec\xe5\xeb\xfc\x20\xee\xf2\xea\xe0\xe7\xfb\xe2\xe0\xfe\xf2\xf1\xff\x20\xef\xf0\xe8\xed"
         "\xe5\xf1\xf2\xe8\x20\xea\xeb\xff\xf2\xe2\xf3\x20\xe2\xe5\xf0\xed\xee\xf1\xf2\xe8\x20\xd0\xee\xeb\xe0\xed\xe4\xf3\x2c\x20\xe8\x20\xef\xee\xfd\xf2\xee\xec\xf3\x20\xe4\xee\xeb\xe6\xed\xfb\x20\xe1"
         "\xfb\xf2\xfc\x20\xef\xee\xe2\xe5\xf0\xe6\xe5\xed\xfb\x2e\x20\xc1\xee\xe3\xe0\xf2\xf1\xf2\xe2\xe0\x20\xe8\x20\xe2\xeb\xe0\xf1\xf2\xe8\x20\xe8\xec\x20\xed\xe5\x20\xe7\xe0\xed\xe8\xec\xe0\xf2\xfc"
         "\x2c\x20\xef\xee\xfd\xf2\xee\xec\xf3\x20\xe1\xf3\xe4\xfc\xf2\xe5\x20\xe3\xee\xf2\xee\xe2\xfb\x20\xea\x20\xed\xe5\xeb\xe5\xe3\xea\xee\xe9\x20\xe1\xee\xf0\xfc\xe1\xe5\x2e\x20\xd7\xf2\xee\xe1\xfb"
         "\x20\xef\xee\xe1\xe5\xe4\xe8\xf2\xfc\x2c\x20\xe7\xe0\xf5\xe2\xe0\xf2\xe8\xf2\xe5\x20\xe2\xf1\xe5\x20\xe2\xf0\xe0\xe6\xe5\xf1\xea\xe8\xe5\x20\xe7\xe0\xec\xea\xe8\x2e"),
     /* Вам следует защитить гномов от армий Арчибальда. Чтобы победить, захватите все города и замки противника. Следите за тем, чтобы враг не захватил все города гномов, иначе победа достанется ему. */ DATA_COMPGEN(0x00512784, cCampaignDescriptionYourTaskIsToDefendTheDwarvesAgainst2,
         "\xc2\xe0\xec\x20\xf1\xeb\xe5\xe4\xf3\xe5\xf2\x20\xe7\xe0\xf9\xe8\xf2\xe8\xf2\xfc\x20\xe3\xed\xee\xec\xee\xe2\x20\xee\xf2\x20\xe0\xf0\xec\xe8\xe9\x20\xc0\xf0\xf7\xe8\xe1\xe0\xeb\xfc\xe4\xe0\x2e"
         "\x20\xd7\xf2\xee\xe1\xfb\x20\xef\xee\xe1\xe5\xe4\xe8\xf2\xfc\x2c\x20\xe7\xe0\xf5\xe2\xe0\xf2\xe8\xf2\xe5\x20\xe2\xf1\xe5\x20\xe3\xee\xf0\xee\xe4\xe0\x20\xe8\x20\xe7\xe0\xec\xea\xe8\x20\xef\xf0"
         "\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e\x20\xd1\xeb\xe5\xe4\xe8\xf2\xe5\x20\xe7\xe0\x20\xf2\xe5\xec\x2c\x20\xf7\xf2\xee\xe1\xfb\x20\xe2\xf0\xe0\xe3\x20\xed\xe5\x20\xe7\xe0\xf5\xe2\xe0\xf2\xe8\xeb"
         "\x20\xe2\xf1\xe5\x20\xe3\xee\xf0\xee\xe4\xe0\x20\xe3\xed\xee\xec\xee\xe2\x2c\x20\xe8\xed\xe0\xf7\xe5\x20\xef\xee\xe1\xe5\xe4\xe0\x20\xe4\xee\xf1\xf2\xe0\xed\xe5\xf2\xf1\xff\x20\xe5\xec\xf3\x2e"),
     /* В борьбе за ресурсы и сокровища вам противостоят четыре противника, объединившихся в союз. Чтобы победить, захватите все вражеские замки. */ DATA_COMPGEN(0x00512848, cCampaignDescriptionYouWillFaceFourAlliedEnemiesInA3,
         "\xc2\x20\xe1\xee\xf0\xfc\xe1\xe5\x20\xe7\xe0\x20\xf0\xe5\xf1\xf3\xf0\xf1\xfb\x20\xe8\x20\xf1\xee\xea\xf0\xee\xe2\xe8\xf9\xe0\x20\xe2\xe0\xec\x20\xef\xf0\xee\xf2\xe8\xe2\xee\xf1\xf2\xee\xff\xf2"
         "\x20\xf7\xe5\xf2\xfb\xf0\xe5\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2c\x20\xee\xe1\xfa\xe5\xe4\xe8\xed\xe8\xe2\xf8\xe8\xf5\xf1\xff\x20\xe2\x20\xf1\xee\xfe\xe7\x2e\x20\xd7\xf2\xee\xe1\xfb"
         "\x20\xef\xee\xe1\xe5\xe4\xe8\xf2\xfc\x2c\x20\xe7\xe0\xf5\xe2\xe0\xf2\xe8\xf2\xe5\x20\xe2\xf1\xe5\x20\xe2\xf0\xe0\xe6\xe5\xf1\xea\xe8\xe5\x20\xe7\xe0\xec\xea\xe8\x2e"),
     /* Ваши враги заключили против вас союз. Они где-то рядом, поэтому в любой момент будьте готовы к битве. Вы победите, когда завладеете всеми четырьмя замками, находящимися в этой небольшой долине. */ DATA_COMPGEN(0x005128d4, cCampaignDescriptionYourEnemiesAreAlliedAgainstYouAndStart4,
         "\xc2\xe0\xf8\xe8\x20\xe2\xf0\xe0\xe3\xe8\x20\xe7\xe0\xea\xeb\xfe\xf7\xe8\xeb\xe8\x20\xef\xf0\xee\xf2\xe8\xe2\x20\xe2\xe0\xf1\x20\xf1\xee\xfe\xe7\x2e\x20\xce\xed\xe8\x20\xe3\xe4\xe5\x2d\xf2\xee"
         "\x20\xf0\xff\xe4\xee\xec\x2c\x20\xef\xee\xfd\xf2\xee\xec\xf3\x20\xe2\x20\xeb\xfe\xe1\xee\xe9\x20\xec\xee\xec\xe5\xed\xf2\x20\xe1\xf3\xe4\xfc\xf2\xe5\x20\xe3\xee\xf2\xee\xe2\xfb\x20\xea\x20\xe1"
         "\xe8\xf2\xe2\xe5\x2e\x20\xc2\xfb\x20\xef\xee\xe1\xe5\xe4\xe8\xf2\xe5\x2c\x20\xea\xee\xe3\xe4\xe0\x20\xe7\xe0\xe2\xeb\xe0\xe4\xe5\xe5\xf2\xe5\x20\xe2\xf1\xe5\xec\xe8\x20\xf7\xe5\xf2\xfb\xf0\xfc"
         "\xec\xff\x20\xe7\xe0\xec\xea\xe0\xec\xe8\x2c\x20\xed\xe0\xf5\xee\xe4\xff\xf9\xe8\xec\xe8\xf1\xff\x20\xe2\x20\xfd\xf2\xee\xe9\x20\xed\xe5\xe1\xee\xeb\xfc\xf8\xee\xe9\x20\xe4\xee\xeb\xe8\xed\xe5"
         "\x2e"),
     /* Гильдия колдунов славного города Норастона попросила Роланда помочь ей отбиться от союзников Арчибальда. Чтобы победить, вы должны захватить все вражеские замки. Не потеряйте Норастон, иначе вы проиграли. (Один из вражеских замков на острове в океане). */ DATA_COMPGEN(0x00512998, cCampaignDescriptionTheSorceressGuildOfNorastonHasRequestedRoland5,
         "\xc3\xe8\xeb\xfc\xe4\xe8\xff\x20\xea\xee\xeb\xe4\xf3\xed\xee\xe2\x20\xf1\xeb\xe0\xe2\xed\xee\xe3\xee\x20\xe3\xee\xf0\xee\xe4\xe0\x20\xcd\xee\xf0\xe0\xf1\xf2\xee\xed\xe0\x20\xef\xee\xef\xf0\xee"
         "\xf1\xe8\xeb\xe0\x20\xd0\xee\xeb\xe0\xed\xe4\xe0\x20\xef\xee\xec\xee\xf7\xfc\x20\xe5\xe9\x20\xee\xf2\xe1\xe8\xf2\xfc\xf1\xff\x20\xee\xf2\x20\xf1\xee\xfe\xe7\xed\xe8\xea\xee\xe2\x20\xc0\xf0\xf7"
         "\xe8\xe1\xe0\xeb\xfc\xe4\xe0\x2e\x20\xd7\xf2\xee\xe1\xfb\x20\xef\xee\xe1\xe5\xe4\xe8\xf2\xfc\x2c\x20\xe2\xfb\x20\xe4\xee\xeb\xe6\xed\xfb\x20\xe7\xe0\xf5\xe2\xe0\xf2\xe8\xf2\xfc\x20\xe2\xf1\xe5"
         "\x20\xe2\xf0\xe0\xe6\xe5\xf1\xea\xe8\xe5\x20\xe7\xe0\xec\xea\xe8\x2e\x20\xcd\xe5\x20\xef\xee\xf2\xe5\xf0\xff\xe9\xf2\xe5\x20\xcd\xee\xf0\xe0\xf1\xf2\xee\xed\x2c\x20\xe8\xed\xe0\xf7\xe5\x20\xe2"
         "\xfb\x20\xef\xf0\xee\xe8\xe3\xf0\xe0\xeb\xe8\x2e\x20\x28\xce\xe4\xe8\xed\x20\xe8\xe7\x20\xe2\xf0\xe0\xe6\xe5\xf1\xea\xe8\xf5\x20\xe7\xe0\xec\xea\xee\xe2\x20\xed\xe0\x20\xee\xf1\xf2\xf0\xee\xe2"
         "\xe5\x20\xe2\x20\xee\xea\xe5\xe0\xed\xe5\x29\x2e"),
     /* Соберите армию побольше и захватите замок противника не позднее, чем через 8 недель. Вам противостоит всего один противник, но до его замка скакать и скакать. Все войска, которые останутся у вас к концу этого сценария, примут участие в заключительной битве. */ DATA_COMPGEN(0x00512a98, cCampaignDescriptionGatherAsLargeAnArmyAsPossibleAnd6,
         "\xd1\xee\xe1\xe5\xf0\xe8\xf2\xe5\x20\xe0\xf0\xec\xe8\xfe\x20\xef\xee\xe1\xee\xeb\xfc\xf8\xe5\x20\xe8\x20\xe7\xe0\xf5\xe2\xe0\xf2\xe8\xf2\xe5\x20\xe7\xe0\xec\xee\xea\x20\xef\xf0\xee\xf2\xe8\xe2"
         "\xed\xe8\xea\xe0\x20\xed\xe5\x20\xef\xee\xe7\xe4\xed\xe5\xe5\x2c\x20\xf7\xe5\xec\x20\xf7\xe5\xf0\xe5\xe7\x20\x38\x20\xed\xe5\xe4\xe5\xeb\xfc\x2e\x20\xc2\xe0\xec\x20\xef\xf0\xee\xf2\xe8\xe2\xee"
         "\xf1\xf2\xee\xe8\xf2\x20\xe2\xf1\xe5\xe3\xee\x20\xee\xe4\xe8\xed\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\x2c\x20\xed\xee\x20\xe4\xee\x20\xe5\xe3\xee\x20\xe7\xe0\xec\xea\xe0\x20\xf1\xea\xe0\xea"
         "\xe0\xf2\xfc\x20\xe8\x20\xf1\xea\xe0\xea\xe0\xf2\xfc\x2e\x20\xc2\xf1\xe5\x20\xe2\xee\xe9\xf1\xea\xe0\x2c\x20\xea\xee\xf2\xee\xf0\xfb\xe5\x20\xee\xf1\xf2\xe0\xed\xf3\xf2\xf1\xff\x20\xf3\x20\xe2"
         "\xe0\xf1\x20\xea\x20\xea\xee\xed\xf6\xf3\x20\xfd\xf2\xee\xe3\xee\x20\xf1\xf6\xe5\xed\xe0\xf0\xe8\xff\x2c\x20\xef\xf0\xe8\xec\xf3\xf2\x20\xf3\xf7\xe0\xf1\xf2\xe8\xe5\x20\xe2\x20\xe7\xe0\xea\xeb"
         "\xfe\xf7\xe8\xf2\xe5\xeb\xfc\xed\xee\xe9\x20\xe1\xe8\xf2\xe2\xe5\x2e"),
     /* Найдите корону, прежде чем это сделают герои Арчибальда. Корона понадобится Роланду для победы в заключительной битве. */ DATA_COMPGEN(0x00512b9c, cCampaignDescriptionFindTheCrownBeforeArchibaldSHeroesFind7,
         "\xcd\xe0\xe9\xe4\xe8\xf2\xe5\x20\xea\xee\xf0\xee\xed\xf3\x2c\x20\xef\xf0\xe5\xe6\xe4\xe5\x20\xf7\xe5\xec\x20\xfd\xf2\xee\x20\xf1\xe4\xe5\xeb\xe0\xfe\xf2\x20\xe3\xe5\xf0\xee\xe8\x20\xc0\xf0\xf7"
         "\xe8\xe1\xe0\xeb\xfc\xe4\xe0\x2e\x20\xca\xee\xf0\xee\xed\xe0\x20\xef\xee\xed\xe0\xe4\xee\xe1\xe8\xf2\xf1\xff\x20\xd0\xee\xeb\xe0\xed\xe4\xf3\x20\xe4\xeb\xff\x20\xef\xee\xe1\xe5\xe4\xfb\x20\xe2"
         "\x20\xe7\xe0\xea\xeb\xfe\xf7\xe8\xf2\xe5\xeb\xfc\xed\xee\xe9\x20\xe1\xe8\xf2\xe2\xe5\x2e"),
     /* Три противника, и среди них сам лорд Корлагон, заключили союз и стоят между вами и великой победой. Роланд обосновался в замке на северо-западе, и если этот замок падет, вы проиграете. Если вы захватите Корлагона сейчас, он не будет драться против вас в последней битве. */ DATA_COMPGEN(0x00512c14, cCampaignDescriptionThreeAlliedEnemiesStandBeforeYouAndVictory8,
         "\xd2\xf0\xe8\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2c\x20\xe8\x20\xf1\xf0\xe5\xe4\xe8\x20\xed\xe8\xf5\x20\xf1\xe0\xec\x20\xeb\xee\xf0\xe4\x20\xca\xee\xf0\xeb\xe0\xe3\xee\xed\x2c\x20\xe7"
         "\xe0\xea\xeb\xfe\xf7\xe8\xeb\xe8\x20\xf1\xee\xfe\xe7\x20\xe8\x20\xf1\xf2\xee\xff\xf2\x20\xec\xe5\xe6\xe4\xf3\x20\xe2\xe0\xec\xe8\x20\xe8\x20\xe2\xe5\xeb\xe8\xea\xee\xe9\x20\xef\xee\xe1\xe5\xe4"
         "\xee\xe9\x2e\x20\xd0\xee\xeb\xe0\xed\xe4\x20\xee\xe1\xee\xf1\xed\xee\xe2\xe0\xeb\xf1\xff\x20\xe2\x20\xe7\xe0\xec\xea\xe5\x20\xed\xe0\x20\xf1\xe5\xe2\xe5\xf0\xee\x2d\xe7\xe0\xef\xe0\xe4\xe5\x2c"
         "\x20\xe8\x20\xe5\xf1\xeb\xe8\x20\xfd\xf2\xee\xf2\x20\xe7\xe0\xec\xee\xea\x20\xef\xe0\xe4\xe5\xf2\x2c\x20\xe2\xfb\x20\xef\xf0\xee\xe8\xe3\xf0\xe0\xe5\xf2\xe5\x2e\x20\xc5\xf1\xeb\xe8\x20\xe2\xfb"
         "\x20\xe7\xe0\xf5\xe2\xe0\xf2\xe8\xf2\xe5\x20\xca\xee\xf0\xeb\xe0\xe3\xee\xed\xe0\x20\xf1\xe5\xe9\xf7\xe0\xf1\x2c\x20\xee\xed\x20\xed\xe5\x20\xe1\xf3\xe4\xe5\xf2\x20\xe4\xf0\xe0\xf2\xfc\xf1\xff"
         "\x20\xef\xf0\xee\xf2\xe8\xe2\x20\xe2\xe0\xf1\x20\xe2\x20\xef\xee\xf1\xeb\xe5\xe4\xed\xe5\xe9\x20\xe1\xe8\xf2\xe2\xe5\x2e"),
     /* Итак, пробил час последнего и решительного боя. И вы, и ваши противники вооружены до зубов, и все кругом объединились против вас. Война будет закончена, когда вы захватите в плен Арчибальда! */ DATA_COMPGEN(0x00512d24, cCampaignDescriptionThisIsTheFinalBattleBothYouAnd9,
         "\xc8\xf2\xe0\xea\x2c\x20\xef\xf0\xee\xe1\xe8\xeb\x20\xf7\xe0\xf1\x20\xef\xee\xf1\xeb\xe5\xe4\xed\xe5\xe3\xee\x20\xe8\x20\xf0\xe5\xf8\xe8\xf2\xe5\xeb\xfc\xed\xee\xe3\xee\x20\xe1\xee\xff\x2e\x20"
         "\xc8\x20\xe2\xfb\x2c\x20\xe8\x20\xe2\xe0\xf8\xe8\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe8\x20\xe2\xee\xee\xf0\xf3\xe6\xe5\xed\xfb\x20\xe4\xee\x20\xe7\xf3\xe1\xee\xe2\x2c\x20\xe8\x20\xe2\xf1"
         "\xe5\x20\xea\xf0\xf3\xe3\xee\xec\x20\xee\xe1\xfa\xe5\xe4\xe8\xed\xe8\xeb\xe8\xf1\xfc\x20\xef\xf0\xee\xf2\xe8\xe2\x20\xe2\xe0\xf1\x2e\x20\xc2\xee\xe9\xed\xe0\x20\xe1\xf3\xe4\xe5\xf2\x20\xe7\xe0"
         "\xea\xee\xed\xf7\xe5\xed\xe0\x2c\x20\xea\xee\xe3\xe4\xe0\x20\xe2\xfb\x20\xe7\xe0\xf5\xe2\xe0\xf2\xe8\xf2\xe5\x20\xe2\x20\xef\xeb\xe5\xed\x20\xc0\xf0\xf7\xe8\xe1\xe0\xeb\xfc\xe4\xe0\x21"),
     /*  */ DATA_COMPGEN(0x00526854, cCampaignDescriptionEmpty10, ""),
     /* Вы сменили сюзерена, и теперь у вас три замка против одного у противника. Эта миссия будет для вас самой легкой во всей войне... Предатель! */ DATA_COMPGEN(0x00512de4, cCampaignDescriptionSwitchingSidesLeavesYouWithThreeCastlesAgainst11,
         "\xc2\xfb\x20\xf1\xec\xe5\xed\xe8\xeb\xe8\x20\xf1\xfe\xe7\xe5\xf0\xe5\xed\xe0\x2c\x20\xe8\x20\xf2\xe5\xef\xe5\xf0\xfc\x20\xf3\x20\xe2\xe0\xf1\x20\xf2\xf0\xe8\x20\xe7\xe0\xec\xea\xe0\x20\xef\xf0"
         "\xee\xf2\xe8\xe2\x20\xee\xe4\xed\xee\xe3\xee\x20\xf3\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e\x20\xdd\xf2\xe0\x20\xec\xe8\xf1\xf1\xe8\xff\x20\xe1\xf3\xe4\xe5\xf2\x20\xe4\xeb\xff\x20\xe2"
         "\xe0\xf1\x20\xf1\xe0\xec\xee\xe9\x20\xeb\xe5\xe3\xea\xee\xe9\x20\xe2\xee\x20\xe2\xf1\xe5\xe9\x20\xe2\xee\xe9\xed\xe5\x2e\x2e\x2e\x20\xcf\xf0\xe5\xe4\xe0\xf2\xe5\xeb\xfc\x21")},
    {/* Король Арчибальд требует, чтобы вы разгромили трех противников, которые обосновались в этих землях. Они не связаны между собой союзным договором, поэтому по большей части они будут тратить силы на вражду друг с другом. Вы победите, когда все их замки окажутся в ваших руках. */ DATA_COMPGEN(0x00512e70, cCampaignDescriptionKingArchibaldRequiresYouToDefeatTheThree12,
        "\xca\xee\xf0\xee\xeb\xfc\x20\xc0\xf0\xf7\xe8\xe1\xe0\xeb\xfc\xe4\x20\xf2\xf0\xe5\xe1\xf3\xe5\xf2\x2c\x20\xf7\xf2\xee\xe1\xfb\x20\xe2\xfb\x20\xf0\xe0\xe7\xe3\xf0\xee\xec\xe8\xeb\xe8\x20\xf2\xf0"
        "\xe5\xf5\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xee\xe2\x2c\x20\xea\xee\xf2\xee\xf0\xfb\xe5\x20\xee\xe1\xee\xf1\xed\xee\xe2\xe0\xeb\xe8\xf1\xfc\x20\xe2\x20\xfd\xf2\xe8\xf5\x20\xe7\xe5\xec\xeb"
        "\xff\xf5\x2e\x20\xce\xed\xe8\x20\xed\xe5\x20\xf1\xe2\xff\xe7\xe0\xed\xfb\x20\xec\xe5\xe6\xe4\xf3\x20\xf1\xee\xe1\xee\xe9\x20\xf1\xee\xfe\xe7\xed\xfb\xec\x20\xe4\xee\xe3\xee\xe2\xee\xf0\xee\xec"
        "\x2c\x20\xef\xee\xfd\xf2\xee\xec\xf3\x20\xef\xee\x20\xe1\xee\xeb\xfc\xf8\xe5\xe9\x20\xf7\xe0\xf1\xf2\xe8\x20\xee\xed\xe8\x20\xe1\xf3\xe4\xf3\xf2\x20\xf2\xf0\xe0\xf2\xe8\xf2\xfc\x20\xf1\xe8\xeb"
        "\xfb\x20\xed\xe0\x20\xe2\xf0\xe0\xe6\xe4\xf3\x20\xe4\xf0\xf3\xe3\x20\xf1\x20\xe4\xf0\xf3\xe3\xee\xec\x2e\x20\xc2\xfb\x20\xef\xee\xe1\xe5\xe4\xe8\xf2\xe5\x2c\x20\xea\xee\xe3\xe4\xe0\x20\xe2\xf1"
        "\xe5\x20\xe8\xf5\x20\xe7\xe0\xec\xea\xe8\x20\xee\xea\xe0\xe6\xf3\xf2\xf1\xff\x20\xe2\x20\xe2\xe0\xf8\xe8\xf5\x20\xf0\xf3\xea\xe0\xf5\x2e"),
     /* Вам предстоит объединить племена северных варваров, предварительно усмирив их. Как и в предыдущей миссии, противники не состоят в союзе друг с другом, но у них больше ресурсов. Победа будет вашей, когда вы захватите все вражеские замки и перебьете всех героев противника. */ DATA_COMPGEN(0x00512f84, cCampaignDescriptionYouMustUnifyTheBarbarianTribesOfThe13,
         "\xc2\xe0\xec\x20\xef\xf0\xe5\xe4\xf1\xf2\xee\xe8\xf2\x20\xee\xe1\xfa\xe5\xe4\xe8\xed\xe8\xf2\xfc\x20\xef\xeb\xe5\xec\xe5\xed\xe0\x20\xf1\xe5\xe2\xe5\xf0\xed\xfb\xf5\x20\xe2\xe0\xf0\xe2\xe0\xf0"
         "\xee\xe2\x2c\x20\xef\xf0\xe5\xe4\xe2\xe0\xf0\xe8\xf2\xe5\xeb\xfc\xed\xee\x20\xf3\xf1\xec\xe8\xf0\xe8\xe2\x20\xe8\xf5\x2e\x20\xca\xe0\xea\x20\xe8\x20\xe2\x20\xef\xf0\xe5\xe4\xfb\xe4\xf3\xf9\xe5"
         "\xe9\x20\xec\xe8\xf1\xf1\xe8\xe8\x2c\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe8\x20\xed\xe5\x20\xf1\xee\xf1\xf2\xee\xff\xf2\x20\xe2\x20\xf1\xee\xfe\xe7\xe5\x20\xe4\xf0\xf3\xe3\x20\xf1\x20\xe4"
         "\xf0\xf3\xe3\xee\xec\x2c\x20\xed\xee\x20\xf3\x20\xed\xe8\xf5\x20\xe1\xee\xeb\xfc\xf8\xe5\x20\xf0\xe5\xf1\xf3\xf0\xf1\xee\xe2\x2e\x20\xcf\xee\xe1\xe5\xe4\xe0\x20\xe1\xf3\xe4\xe5\xf2\x20\xe2\xe0"
         "\xf8\xe5\xe9\x2c\x20\xea\xee\xe3\xe4\xe0\x20\xe2\xfb\x20\xe7\xe0\xf5\xe2\xe0\xf2\xe8\xf2\xe5\x20\xe2\xf1\xe5\x20\xe2\xf0\xe0\xe6\xe5\xf1\xea\xe8\xe5\x20\xe7\xe0\xec\xea\xe8\x20\xe8\x20\xef\xe5"
         "\xf0\xe5\xe1\xfc\xe5\xf2\xe5\x20\xe2\xf1\xe5\xf5\x20\xe3\xe5\xf0\xee\xe5\xe2\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e"),
     /* Добрые волшебники захватили замок некромантов. Чтобы победить, вы должны отобрать его обратно. Помните, что хотя вы и начинаете с сильной армией, в самом начале у вас нет своего замка. Вы должны заиметь его за 7 дней, иначе все потеряно. (Ближайший замок на юго-востоке). */ DATA_COMPGEN(0x00513094, cCampaignDescriptionDoGooderWizardsHaveTakenTheNecromancersCastle14,
         "\xc4\xee\xe1\xf0\xfb\xe5\x20\xe2\xee\xeb\xf8\xe5\xe1\xed\xe8\xea\xe8\x20\xe7\xe0\xf5\xe2\xe0\xf2\xe8\xeb\xe8\x20\xe7\xe0\xec\xee\xea\x20\xed\xe5\xea\xf0\xee\xec\xe0\xed\xf2\xee\xe2\x2e\x20\xd7"
         "\xf2\xee\xe1\xfb\x20\xef\xee\xe1\xe5\xe4\xe8\xf2\xfc\x2c\x20\xe2\xfb\x20\xe4\xee\xeb\xe6\xed\xfb\x20\xee\xf2\xee\xe1\xf0\xe0\xf2\xfc\x20\xe5\xe3\xee\x20\xee\xe1\xf0\xe0\xf2\xed\xee\x2e\x20\xcf"
         "\xee\xec\xed\xe8\xf2\xe5\x2c\x20\xf7\xf2\xee\x20\xf5\xee\xf2\xff\x20\xe2\xfb\x20\xe8\x20\xed\xe0\xf7\xe8\xed\xe0\xe5\xf2\xe5\x20\xf1\x20\xf1\xe8\xeb\xfc\xed\xee\xe9\x20\xe0\xf0\xec\xe8\xe5\xe9"
         "\x2c\x20\xe2\x20\xf1\xe0\xec\xee\xec\x20\xed\xe0\xf7\xe0\xeb\xe5\x20\xf3\x20\xe2\xe0\xf1\x20\xed\xe5\xf2\x20\xf1\xe2\xee\xe5\xe3\xee\x20\xe7\xe0\xec\xea\xe0\x2e\x20\xc2\xfb\x20\xe4\xee\xeb\xe6"
         "\xed\xfb\x20\xe7\xe0\xe8\xec\xe5\xf2\xfc\x20\xe5\xe3\xee\x20\xe7\xe0\x20\x37\x20\xe4\xed\xe5\xe9\x2c\x20\xe8\xed\xe0\xf7\xe5\x20\xe2\xf1\xe5\x20\xef\xee\xf2\xe5\xf0\xff\xed\xee\x2e\x20\x28\xc1"
         "\xeb\xe8\xe6\xe0\xe9\xf8\xe8\xe9\x20\xe7\xe0\xec\xee\xea\x20\xed\xe0\x20\xfe\xe3\xee\x2d\xe2\xee\xf1\xf2\xee\xea\xe5\x29\x2e"),
     /* Гномов следует привести к покорности, прежде чем они смогут помешать планам короля Арчибальда. Под знаменами Роланда много героев, у него несколько замков, поэтому будьте готовы к нападению сразу с нескольких сторон. Вам надо захватить все города противника. */ DATA_COMPGEN(0x005131a4, cCampaignDescriptionTheDwarvesNeedConqueringBeforeTheyCanInterfere15,
         "\xc3\xed\xee\xec\xee\xe2\x20\xf1\xeb\xe5\xe4\xf3\xe5\xf2\x20\xef\xf0\xe8\xe2\xe5\xf1\xf2\xe8\x20\xea\x20\xef\xee\xea\xee\xf0\xed\xee\xf1\xf2\xe8\x2c\x20\xef\xf0\xe5\xe6\xe4\xe5\x20\xf7\xe5\xec"
         "\x20\xee\xed\xe8\x20\xf1\xec\xee\xe3\xf3\xf2\x20\xef\xee\xec\xe5\xf8\xe0\xf2\xfc\x20\xef\xeb\xe0\xed\xe0\xec\x20\xea\xee\xf0\xee\xeb\xff\x20\xc0\xf0\xf7\xe8\xe1\xe0\xeb\xfc\xe4\xe0\x2e\x20\xcf"
         "\xee\xe4\x20\xe7\xed\xe0\xec\xe5\xed\xe0\xec\xe8\x20\xd0\xee\xeb\xe0\xed\xe4\xe0\x20\xec\xed\xee\xe3\xee\x20\xe3\xe5\xf0\xee\xe5\xe2\x2c\x20\xf3\x20\xed\xe5\xe3\xee\x20\xed\xe5\xf1\xea\xee\xeb"
         "\xfc\xea\xee\x20\xe7\xe0\xec\xea\xee\xe2\x2c\x20\xef\xee\xfd\xf2\xee\xec\xf3\x20\xe1\xf3\xe4\xfc\xf2\xe5\x20\xe3\xee\xf2\xee\xe2\xfb\x20\xea\x20\xed\xe0\xef\xe0\xe4\xe5\xed\xe8\xfe\x20\xf1\xf0"
         "\xe0\xe7\xf3\x20\xf1\x20\xed\xe5\xf1\xea\xee\xeb\xfc\xea\xe8\xf5\x20\xf1\xf2\xee\xf0\xee\xed\x2e\x20\xc2\xe0\xec\x20\xed\xe0\xe4\xee\x20\xe7\xe0\xf5\xe2\xe0\xf2\xe8\xf2\xfc\x20\xe2\xf1\xe5\x20"
         "\xe3\xee\xf0\xee\xe4\xe0\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e"),
     /* Ваши противники объединились против вас и притаились неподалеку, поэтому будьте начеку. Победа будет вашей, когда вы завладеете всеми четырьмя замками, находящимися в этой небольшой долине. */ DATA_COMPGEN(0x005132a8, cCampaignDescriptionYourEnemiesAreAlliedAgainstYouAndStart16,
         "\xc2\xe0\xf8\xe8\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe8\x20\xee\xe1\xfa\xe5\xe4\xe8\xed\xe8\xeb\xe8\xf1\xfc\x20\xef\xf0\xee\xf2\xe8\xe2\x20\xe2\xe0\xf1\x20\xe8\x20\xef\xf0\xe8\xf2\xe0\xe8"
         "\xeb\xe8\xf1\xfc\x20\xed\xe5\xef\xee\xe4\xe0\xeb\xe5\xea\xf3\x2c\x20\xef\xee\xfd\xf2\xee\xec\xf3\x20\xe1\xf3\xe4\xfc\xf2\xe5\x20\xed\xe0\xf7\xe5\xea\xf3\x2e\x20\xcf\xee\xe1\xe5\xe4\xe0\x20\xe1"
         "\xf3\xe4\xe5\xf2\x20\xe2\xe0\xf8\xe5\xe9\x2c\x20\xea\xee\xe3\xe4\xe0\x20\xe2\xfb\x20\xe7\xe0\xe2\xeb\xe0\xe4\xe5\xe5\xf2\xe5\x20\xe2\xf1\xe5\xec\xe8\x20\xf7\xe5\xf2\xfb\xf0\xfc\xec\xff\x20\xe7"
         "\xe0\xec\xea\xe0\xec\xe8\x2c\x20\xed\xe0\xf5\xee\xe4\xff\xf9\xe8\xec\xe8\xf1\xff\x20\xe2\x20\xfd\xf2\xee\xe9\x20\xed\xe5\xe1\xee\xeb\xfc\xf8\xee\xe9\x20\xe4\xee\xeb\xe8\xed\xe5\x2e"),
     /* Вам предстоит подавить крестьянский бунт, во главе которого стоят агенты Роланда. Все ваши соседи объединились против вас, но на вашей стороне лорд Корлагон - опытный и сильный боец. Чтобы победить, вы должны захватить все замки противника. */ DATA_COMPGEN(0x00513368, cCampaignDescriptionYouMustPutDownAPeasantRevoltLed17,
         "\xc2\xe0\xec\x20\xef\xf0\xe5\xe4\xf1\xf2\xee\xe8\xf2\x20\xef\xee\xe4\xe0\xe2\xe8\xf2\xfc\x20\xea\xf0\xe5\xf1\xf2\xfc\xff\xed\xf1\xea\xe8\xe9\x20\xe1\xf3\xed\xf2\x2c\x20\xe2\xee\x20\xe3\xeb\xe0"
         "\xe2\xe5\x20\xea\xee\xf2\xee\xf0\xee\xe3\xee\x20\xf1\xf2\xee\xff\xf2\x20\xe0\xe3\xe5\xed\xf2\xfb\x20\xd0\xee\xeb\xe0\xed\xe4\xe0\x2e\x20\xc2\xf1\xe5\x20\xe2\xe0\xf8\xe8\x20\xf1\xee\xf1\xe5\xe4"
         "\xe8\x20\xee\xe1\xfa\xe5\xe4\xe8\xed\xe8\xeb\xe8\xf1\xfc\x20\xef\xf0\xee\xf2\xe8\xe2\x20\xe2\xe0\xf1\x2c\x20\xed\xee\x20\xed\xe0\x20\xe2\xe0\xf8\xe5\xe9\x20\xf1\xf2\xee\xf0\xee\xed\xe5\x20\xeb"
         "\xee\xf0\xe4\x20\xca\xee\xf0\xeb\xe0\xe3\xee\xed\x20\x2d\x20\xee\xef\xfb\xf2\xed\xfb\xe9\x20\xe8\x20\xf1\xe8\xeb\xfc\xed\xfb\xe9\x20\xe1\xee\xe5\xf6\x2e\x20\xd7\xf2\xee\xe1\xfb\x20\xef\xee\xe1"
         "\xe5\xe4\xe8\xf2\xfc\x2c\x20\xe2\xfb\x20\xe4\xee\xeb\xe6\xed\xfb\x20\xe7\xe0\xf5\xe2\xe0\xf2\xe8\xf2\xfc\x20\xe2\xf1\xe5\x20\xe7\xe0\xec\xea\xe8\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e"),
     /* В этой миссии вам противостоят два противника. Оба хорошо вооружены и полны решимости выставить вас со своего острова. Избегая встречи с ними, захватите Драконий город - тогда победа будет за вами. */ DATA_COMPGEN(0x0051345c, cCampaignDescriptionThereAreTwoEnemiesAlliedAgainstYouIn18,
         "\xc2\x20\xfd\xf2\xee\xe9\x20\xec\xe8\xf1\xf1\xe8\xe8\x20\xe2\xe0\xec\x20\xef\xf0\xee\xf2\xe8\xe2\xee\xf1\xf2\xee\xff\xf2\x20\xe4\xe2\xe0\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e\x20\xce"
         "\xe1\xe0\x20\xf5\xee\xf0\xee\xf8\xee\x20\xe2\xee\xee\xf0\xf3\xe6\xe5\xed\xfb\x20\xe8\x20\xef\xee\xeb\xed\xfb\x20\xf0\xe5\xf8\xe8\xec\xee\xf1\xf2\xe8\x20\xe2\xfb\xf1\xf2\xe0\xe2\xe8\xf2\xfc\x20"
         "\xe2\xe0\xf1\x20\xf1\xee\x20\xf1\xe2\xee\xe5\xe3\xee\x20\xee\xf1\xf2\xf0\xee\xe2\xe0\x2e\x20\xc8\xe7\xe1\xe5\xe3\xe0\xff\x20\xe2\xf1\xf2\xf0\xe5\xf7\xe8\x20\xf1\x20\xed\xe8\xec\xe8\x2c\x20\xe7"
         "\xe0\xf5\xe2\xe0\xf2\xe8\xf2\xe5\x20\xc4\xf0\xe0\xea\xee\xed\xe8\xe9\x20\xe3\xee\xf0\xee\xe4\x20\x2d\x20\xf2\xee\xe3\xe4\xe0\x20\xef\xee\xe1\xe5\xe4\xe0\x20\xe1\xf3\xe4\xe5\xf2\x20\xe7\xe0\x20"
         "\xe2\xe0\xec\xe8\x2e"),
     /* Вам приказано разгромить удельных властителей, которые присягнули на верность Роланду. Все вражеские замки объединились и выступают против вас. Вы начинаете игру без замка. Вам надо захватить замок за 7 дней. Победа будет вашей, когда все замки противника падут. */ DATA_COMPGEN(0x00513524, cCampaignDescriptionYourOrdersAreToConquerTheCountryLords19,
         "\xc2\xe0\xec\x20\xef\xf0\xe8\xea\xe0\xe7\xe0\xed\xee\x20\xf0\xe0\xe7\xe3\xf0\xee\xec\xe8\xf2\xfc\x20\xf3\xe4\xe5\xeb\xfc\xed\xfb\xf5\x20\xe2\xeb\xe0\xf1\xf2\xe8\xf2\xe5\xeb\xe5\xe9\x2c\x20\xea"
         "\xee\xf2\xee\xf0\xfb\xe5\x20\xef\xf0\xe8\xf1\xff\xe3\xed\xf3\xeb\xe8\x20\xed\xe0\x20\xe2\xe5\xf0\xed\xee\xf1\xf2\xfc\x20\xd0\xee\xeb\xe0\xed\xe4\xf3\x2e\x20\xc2\xf1\xe5\x20\xe2\xf0\xe0\xe6\xe5"
         "\xf1\xea\xe8\xe5\x20\xe7\xe0\xec\xea\xe8\x20\xee\xe1\xfa\xe5\xe4\xe8\xed\xe8\xeb\xe8\xf1\xfc\x20\xe8\x20\xe2\xfb\xf1\xf2\xf3\xef\xe0\xfe\xf2\x20\xef\xf0\xee\xf2\xe8\xe2\x20\xe2\xe0\xf1\x2e\x20"
         "\xc2\xfb\x20\xed\xe0\xf7\xe8\xed\xe0\xe5\xf2\xe5\x20\xe8\xe3\xf0\xf3\x20\xe1\xe5\xe7\x20\xe7\xe0\xec\xea\xe0\x2e\x20\xc2\xe0\xec\x20\xed\xe0\xe4\xee\x20\xe7\xe0\xf5\xe2\xe0\xf2\xe8\xf2\xfc\x20"
         "\xe7\xe0\xec\xee\xea\x20\xe7\xe0\x20\x37\x20\xe4\xed\xe5\xe9\x2e\x20\xcf\xee\xe1\xe5\xe4\xe0\x20\xe1\xf3\xe4\xe5\xf2\x20\xe2\xe0\xf8\xe5\xe9\x2c\x20\xea\xee\xe3\xe4\xe0\x20\xe2\xf1\xe5\x20\xe7"
         "\xe0\xec\xea\xe8\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x20\xef\xe0\xe4\xf3\xf2\x2e"),
     /* Найдите корону, пока ею не завладели герои Роланда. Корона понадобится Арчибальду для победы в заключительной битве с Роландом. */ DATA_COMPGEN(0x0051362c, cCampaignDescriptionFindTheCrownBeforeRolandSHeroesFind20,
         "\xcd\xe0\xe9\xe4\xe8\xf2\xe5\x20\xea\xee\xf0\xee\xed\xf3\x2c\x20\xef\xee\xea\xe0\x20\xe5\xfe\x20\xed\xe5\x20\xe7\xe0\xe2\xeb\xe0\xe4\xe5\xeb\xe8\x20\xe3\xe5\xf0\xee\xe8\x20\xd0\xee\xeb\xe0\xed"
         "\xe4\xe0\x2e\x20\xca\xee\xf0\xee\xed\xe0\x20\xef\xee\xed\xe0\xe4\xee\xe1\xe8\xf2\xf1\xff\x20\xc0\xf0\xf7\xe8\xe1\xe0\xeb\xfc\xe4\xf3\x20\xe4\xeb\xff\x20\xef\xee\xe1\xe5\xe4\xfb\x20\xe2\x20\xe7"
         "\xe0\xea\xeb\xfe\xf7\xe8\xf2\xe5\xeb\xfc\xed\xee\xe9\x20\xe1\xe8\xf2\xe2\xe5\x20\xf1\x20\xd0\xee\xeb\xe0\xed\xe4\xee\xec\x2e"),
     /* Соберите армию побольше и захватите замок противника не позднее, чем через 8 недель. Вам противостоит всего один противник, но до его замка скакать и скакать. Все войска, которые останутся у вас к концу этого сценария, будут с вами в заключительной битве. */ DATA_COMPGEN(0x005136ac, cCampaignDescriptionGatherAsLargeAnArmyAsPossibleAnd21,
         "\xd1\xee\xe1\xe5\xf0\xe8\xf2\xe5\x20\xe0\xf0\xec\xe8\xfe\x20\xef\xee\xe1\xee\xeb\xfc\xf8\xe5\x20\xe8\x20\xe7\xe0\xf5\xe2\xe0\xf2\xe8\xf2\xe5\x20\xe7\xe0\xec\xee\xea\x20\xef\xf0\xee\xf2\xe8\xe2"
         "\xed\xe8\xea\xe0\x20\xed\xe5\x20\xef\xee\xe7\xe4\xed\xe5\xe5\x2c\x20\xf7\xe5\xec\x20\xf7\xe5\xf0\xe5\xe7\x20\x38\x20\xed\xe5\xe4\xe5\xeb\xfc\x2e\x20\xc2\xe0\xec\x20\xef\xf0\xee\xf2\xe8\xe2\xee"
         "\xf1\xf2\xee\xe8\xf2\x20\xe2\xf1\xe5\xe3\xee\x20\xee\xe4\xe8\xed\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\x2c\x20\xed\xee\x20\xe4\xee\x20\xe5\xe3\xee\x20\xe7\xe0\xec\xea\xe0\x20\xf1\xea\xe0\xea"
         "\xe0\xf2\xfc\x20\xe8\x20\xf1\xea\xe0\xea\xe0\xf2\xfc\x2e\x20\xc2\xf1\xe5\x20\xe2\xee\xe9\xf1\xea\xe0\x2c\x20\xea\xee\xf2\xee\xf0\xfb\xe5\x20\xee\xf1\xf2\xe0\xed\xf3\xf2\xf1\xff\x20\xf3\x20\xe2"
         "\xe0\xf1\x20\xea\x20\xea\xee\xed\xf6\xf3\x20\xfd\xf2\xee\xe3\xee\x20\xf1\xf6\xe5\xed\xe0\xf0\xe8\xff\x2c\x20\xe1\xf3\xe4\xf3\xf2\x20\xf1\x20\xe2\xe0\xec\xe8\x20\xe2\x20\xe7\xe0\xea\xeb\xfe\xf7"
         "\xe8\xf2\xe5\xeb\xfc\xed\xee\xe9\x20\xe1\xe8\xf2\xe2\xe5\x2e"),
     /* Итак, пробил час последней битвы. И вы, и ваши противники вооружены до зубов, и все объединились против вас. Война закончится, когда вы захватите в плен Роланда, и смотрите, не потеряйте Арчибальда в пылу битвы! */ DATA_COMPGEN(0x005137ac, cCampaignDescriptionThisIsTheFinalBattleBothYouAnd22,
         "\xc8\xf2\xe0\xea\x2c\x20\xef\xf0\xee\xe1\xe8\xeb\x20\xf7\xe0\xf1\x20\xef\xee\xf1\xeb\xe5\xe4\xed\xe5\xe9\x20\xe1\xe8\xf2\xe2\xfb\x2e\x20\xc8\x20\xe2\xfb\x2c\x20\xe8\x20\xe2\xe0\xf8\xe8\x20\xef"
         "\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe8\x20\xe2\xee\xee\xf0\xf3\xe6\xe5\xed\xfb\x20\xe4\xee\x20\xe7\xf3\xe1\xee\xe2\x2c\x20\xe8\x20\xe2\xf1\xe5\x20\xee\xe1\xfa\xe5\xe4\xe8\xed\xe8\xeb\xe8\xf1\xfc"
         "\x20\xef\xf0\xee\xf2\xe8\xe2\x20\xe2\xe0\xf1\x2e\x20\xc2\xee\xe9\xed\xe0\x20\xe7\xe0\xea\xee\xed\xf7\xe8\xf2\xf1\xff\x2c\x20\xea\xee\xe3\xe4\xe0\x20\xe2\xfb\x20\xe7\xe0\xf5\xe2\xe0\xf2\xe8\xf2"
         "\xe5\x20\xe2\x20\xef\xeb\xe5\xed\x20\xd0\xee\xeb\xe0\xed\xe4\xe0\x2c\x20\xe8\x20\xf1\xec\xee\xf2\xf0\xe8\xf2\xe5\x2c\x20\xed\xe5\x20\xef\xee\xf2\xe5\xf0\xff\xe9\xf2\xe5\x20\xc0\xf0\xf7\xe8\xe1"
         "\xe0\xeb\xfc\xe4\xe0\x20\xe2\x20\xef\xfb\xeb\xf3\x20\xe1\xe8\xf2\xe2\xfb\x21"),
     /* Вы сменили сюзерена, и теперь у вас три замка против одного у противника. Эта миссия будет для вас самой легкой во всей войне... Предатель! */ DATA_COMPGEN(0x00513880, cCampaignDescriptionSwitchingSidesLeavesYouWithThreeCastlesAgainst23,
         "\xc2\xfb\x20\xf1\xec\xe5\xed\xe8\xeb\xe8\x20\xf1\xfe\xe7\xe5\xf0\xe5\xed\xe0\x2c\x20\xe8\x20\xf2\xe5\xef\xe5\xf0\xfc\x20\xf3\x20\xe2\xe0\xf1\x20\xf2\xf0\xe8\x20\xe7\xe0\xec\xea\xe0\x20\xef\xf0"
         "\xee\xf2\xe8\xe2\x20\xee\xe4\xed\xee\xe3\xee\x20\xf3\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e\x20\xdd\xf2\xe0\x20\xec\xe8\xf1\xf1\xe8\xff\x20\xe1\xf3\xe4\xe5\xf2\x20\xe4\xeb\xff\x20\xe2"
         "\xe0\xf1\x20\xf1\xe0\xec\xee\xe9\x20\xeb\xe5\xe3\xea\xee\xe9\x20\xe2\xee\x20\xe2\xf1\xe5\xe9\x20\xe2\xee\xe9\xed\xe5\x2e\x2e\x2e\x20\xcf\xf0\xe5\xe4\xe0\xf2\xe5\xeb\xfc\x21")}
};
DATA(0x004ff2d4) char* cOutOfMemory =
    /* \n\n\n\n\n\n\n\n\n\n\n\n\n\n%s\nГероям II требуется минимум \n%dK Расширенной  памяти (XMS) и\n480K общей памяти.\n\n */ DATA_COMPGEN(0x0051390c, cOutOfMemoryFormatHeroesIIRequiresAMinimumOfFormat0,
        "\x0a\x0a\x0a\x0a\x0a\x0a\x0a\x0a\x0a\x0a\x0a\x0a\x0a\x0a\x25\x73\x0a\xc3\xe5\xf0\xee\xff\xec\x20\x49\x49\x20\xf2\xf0\xe5\xe1\xf3\xe5\xf2\xf1\xff\x20\xec\xe8\xed\xe8\xec\xf3\xec\x20\x0a\x25\x64"
        "\x4b\x20\xd0\xe0\xf1\xf8\xe8\xf0\xe5\xed\xed\xee\xe9\x20\x20\xef\xe0\xec\xff\xf2\xe8\x20\x28\x58\x4d\x53\x29\x20\xe8\x0a\x34\x38\x30\x4b\x20\xee\xe1\xf9\xe5\xe9\x20\xef\xe0\xec\xff\xf2\xe8\x2e"
        "\x0a\x0a");
DATA(0x004ff2d8) char* cSlowVideoLevelText[KB_SLOW_VIDEO_LEVEL_TEXT_COUNT] = {/* Обычное */ DATA_COMPGEN(0x00513970, cSlowVideoLevelTextNormal0, "\xce\xe1\xfb\xf7\xed\xee\xe5"), /* Черес-\nстрочное */ DATA_COMPGEN(0x00513978, cSlowVideoLevelTextInterlaced1, "\xd7\xe5\xf0\xe5\xf1\x2d\x0a\xf1\xf2\xf0\xee\xf7\xed\xee\xe5")};
DATA(0x004ff2e0) char* gSPanelHelp[KB_SETTINGS_PANEL_HELP_COUNT] = {
    /* {ОК}\n\nЗакрыть меню. */ DATA_COMPGEN(0x00513988, gSPanelHelpOKExitThisMenu0, "\x7b\xce\xca\x7d\x0a\x0a\xc7\xe0\xea\xf0\xfb\xf2\xfc\x20\xec\xe5\xed\xfe\x2e"),
    /* {Музыка}\n\nВключить или выключить фоновую музыку. */ DATA_COMPGEN(0x0051399c, gSPanelHelpMusicToggleAmbientMusicLevelNoteWhenUsing1, "\x7b\xcc\xf3\xe7\xfb\xea\xe0\x7d\x0a\x0a\xc2\xea\xeb\xfe\xf7\xe8\xf2\xfc\x20\xe8\xeb\xe8\x20\xe2\xfb\xea\xeb\xfe\xf7\xe8\xf2\xfc\x20\xf4\xee\xed\xee\xe2\xf3\xfe\x20\xec\xf3\xe7\xfb\xea\xf3\x2e"),
    /* {Эффекты}\n\nВключить или выключить звуковые эффекты. */ DATA_COMPGEN(0x005139d0, gSPanelHelpEffectsToggleForegroundSoundsLevel2,
        "\x7b\xdd\xf4\xf4\xe5\xea\xf2\xfb\x7d\x0a\x0a\xc2\xea\xeb\xfe\xf7\xe8\xf2\xfc\x20\xe8\xeb\xe8\x20\xe2\xfb\xea\xeb\xfe\xf7\xe8\xf2\xfc\x20\xe7\xe2\xf3\xea\xee\xe2\xfb\xe5\x20\xfd\xf4\xf4\xe5\xea"
        "\xf2\xfb\x2e"),
    /* {Скорость}\n\nВыбрать скорость передвижения героев по карте. */ DATA_COMPGEN(0x00513a04, gSPanelHelpSpeedChangeTheSpeedAtWhichYourHeroes3,
        "\x7b\xd1\xea\xee\xf0\xee\xf1\xf2\xfc\x7d\x0a\x0a\xc2\xfb\xe1\xf0\xe0\xf2\xfc\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xfc\x20\xef\xe5\xf0\xe5\xe4\xe2\xe8\xe6\xe5\xed\xe8\xff\x20\xe3\xe5\xf0\xee\xe5\xe2"
        "\x20\xef\xee\x20\xea\xe0\xf0\xf2\xe5\x2e"),
    /* {Качество звука}\n\nВыбрать формат музыки. Как правило, музыка в формате MIDI не отличается качеством, но она предъявляет меньшие требования к производительности системы, чем формат Стерео CD. Формат Стерео CD дает  возможность воспроизводить оперную музыку. */ DATA_COMPGEN(0x00513a40, gSPanelHelpMusicTypeChangeTheTypeOfMusicMIDI4,
        "\x7b\xca\xe0\xf7\xe5\xf1\xf2\xe2\xee\x20\xe7\xe2\xf3\xea\xe0\x7d\x0a\x0a\xc2\xfb\xe1\xf0\xe0\xf2\xfc\x20\xf4\xee\xf0\xec\xe0\xf2\x20\xec\xf3\xe7\xfb\xea\xe8\x2e\x20\xca\xe0\xea\x20\xef\xf0\xe0"
        "\xe2\xe8\xeb\xee\x2c\x20\xec\xf3\xe7\xfb\xea\xe0\x20\xe2\x20\xf4\xee\xf0\xec\xe0\xf2\xe5\x20\x4d\x49\x44\x49\x20\xed\xe5\x20\xee\xf2\xeb\xe8\xf7\xe0\xe5\xf2\xf1\xff\x20\xea\xe0\xf7\xe5\xf1\xf2"
        "\xe2\xee\xec\x2c\x20\xed\xee\x20\xee\xed\xe0\x20\xef\xf0\xe5\xe4\xfa\xff\xe2\xeb\xff\xe5\xf2\x20\xec\xe5\xed\xfc\xf8\xe8\xe5\x20\xf2\xf0\xe5\xe1\xee\xe2\xe0\xed\xe8\xff\x20\xea\x20\xef\xf0\xee"
        "\xe8\xe7\xe2\xee\xe4\xe8\xf2\xe5\xeb\xfc\xed\xee\xf1\xf2\xe8\x20\xf1\xe8\xf1\xf2\xe5\xec\xfb\x2c\x20\xf7\xe5\xec\x20\xf4\xee\xf0\xec\xe0\xf2\x20\xd1\xf2\xe5\xf0\xe5\xee\x20\x43\x44\x2e\x20\xd4"
        "\xee\xf0\xec\xe0\xf2\x20\xd1\xf2\xe5\xf0\xe5\xee\x20\x43\x44\x20\xe4\xe0\xe5\xf2\x20\x20\xe2\xee\xe7\xec\xee\xe6\xed\xee\xf1\xf2\xfc\x20\xe2\xee\xf1\xef\xf0\xee\xe8\xe7\xe2\xee\xe4\xe8\xf2\xfc"
        "\x20\xee\xef\xe5\xf0\xed\xf3\xfe\x20\xec\xf3\xe7\xfb\xea\xf3\x2e"),
    /* {Показывать путь}\n\nВключить или выключить отображение пути героя на карте.  Если опция включена, первое нажатие по объекту на карте показывает путь к этому объекту, а по второму нажатию левой кнпоки мыши начинается движение. Если эта опция отключена, движение начинается по первому нажатию. */ DATA_COMPGEN(0x00513b44, gSPanelHelpShowPathToggleShowPathOnOffIf5,
        "\x7b\xcf\xee\xea\xe0\xe7\xfb\xe2\xe0\xf2\xfc\x20\xef\xf3\xf2\xfc\x7d\x0a\x0a\xc2\xea\xeb\xfe\xf7\xe8\xf2\xfc\x20\xe8\xeb\xe8\x20\xe2\xfb\xea\xeb\xfe\xf7\xe8\xf2\xfc\x20\xee\xf2\xee\xe1\xf0\xe0"
        "\xe6\xe5\xed\xe8\xe5\x20\xef\xf3\xf2\xe8\x20\xe3\xe5\xf0\xee\xff\x20\xed\xe0\x20\xea\xe0\xf0\xf2\xe5\x2e\x20\x20\xc5\xf1\xeb\xe8\x20\xee\xef\xf6\xe8\xff\x20\xe2\xea\xeb\xfe\xf7\xe5\xed\xe0\x2c"
        "\x20\xef\xe5\xf0\xe2\xee\xe5\x20\xed\xe0\xe6\xe0\xf2\xe8\xe5\x20\xef\xee\x20\xee\xe1\xfa\xe5\xea\xf2\xf3\x20\xed\xe0\x20\xea\xe0\xf0\xf2\xe5\x20\xef\xee\xea\xe0\xe7\xfb\xe2\xe0\xe5\xf2\x20\xef"
        "\xf3\xf2\xfc\x20\xea\x20\xfd\xf2\xee\xec\xf3\x20\xee\xe1\xfa\xe5\xea\xf2\xf3\x2c\x20\xe0\x20\xef\xee\x20\xe2\xf2\xee\xf0\xee\xec\xf3\x20\xed\xe0\xe6\xe0\xf2\xe8\xfe\x20\xeb\xe5\xe2\xee\xe9\x20"
        "\xea\xed\xef\xee\xea\xe8\x20\xec\xfb\xf8\xe8\x20\xed\xe0\xf7\xe8\xed\xe0\xe5\xf2\xf1\xff\x20\xe4\xe2\xe8\xe6\xe5\xed\xe8\xe5\x2e\x20\xc5\xf1\xeb\xe8\x20\xfd\xf2\xe0\x20\xee\xef\xf6\xe8\xff\x20"
        "\xee\xf2\xea\xeb\xfe\xf7\xe5\xed\xe0\x2c\x20\xe4\xe2\xe8\xe6\xe5\xed\xe8\xe5\x20\xed\xe0\xf7\xe8\xed\xe0\xe5\xf2\xf1\xff\x20\xef\xee\x20\xef\xe5\xf0\xe2\xee\xec\xf3\x20\xed\xe0\xe6\xe0\xf2\xe8"
        "\xfe\x2e"),
    /* {Скорость врага}\n\nВыбрать скорости перемещения героев, управляемых компьютером. При этом можно выбрать режим, в котором не будет отображаться передвижение противника. */ DATA_COMPGEN(0x00513c68, gSPanelHelpEnemySpeedSetsTheSpeedThatAI6,
        "\x7b\xd1\xea\xee\xf0\xee\xf1\xf2\xfc\x20\xe2\xf0\xe0\xe3\xe0\x7d\x0a\x0a\xc2\xfb\xe1\xf0\xe0\xf2\xfc\x20\xf1\xea\xee\xf0\xee\xf1\xf2\xe8\x20\xef\xe5\xf0\xe5\xec\xe5\xf9\xe5\xed\xe8\xff\x20\xe3"
        "\xe5\xf0\xee\xe5\xe2\x2c\x20\xf3\xef\xf0\xe0\xe2\xeb\xff\xe5\xec\xfb\xf5\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xee\xec\x2e\x20\xcf\xf0\xe8\x20\xfd\xf2\xee\xec\x20\xec\xee\xe6\xed\xee\x20\xe2"
        "\xfb\xe1\xf0\xe0\xf2\xfc\x20\xf0\xe5\xe6\xe8\xec\x2c\x20\xe2\x20\xea\xee\xf2\xee\xf0\xee\xec\x20\xed\xe5\x20\xe1\xf3\xe4\xe5\xf2\x20\xee\xf2\xee\xe1\xf0\xe0\xe6\xe0\xf2\xfc\xf1\xff\x20\xef\xe5"
        "\xf0\xe5\xe4\xe2\xe8\xe6\xe5\xed\xe8\xe5\x20\xef\xf0\xee\xf2\xe8\xe2\xed\xe8\xea\xe0\x2e"),
    /* {Интерфейс}\n\nВыбор желаемого типа интерфейса. По умолчанию задан динамический интерфейс, в котором 'злое' графическое оформление используется для трех 'злых' классов героев (варвара, чернокнижника и некроманта). */ DATA_COMPGEN(0x00513d10, gSPanelHelpInterfaceSetsWhatTypeOfInterfaceYouWant7,
        "\x7b\xc8\xed\xf2\xe5\xf0\xf4\xe5\xe9\xf1\x7d\x0a\x0a\xc2\xfb\xe1\xee\xf0\x20\xe6\xe5\xeb\xe0\xe5\xec\xee\xe3\xee\x20\xf2\xe8\xef\xe0\x20\xe8\xed\xf2\xe5\xf0\xf4\xe5\xe9\xf1\xe0\x2e\x20\xcf\xee"
        "\x20\xf3\xec\xee\xeb\xf7\xe0\xed\xe8\xfe\x20\xe7\xe0\xe4\xe0\xed\x20\xe4\xe8\xed\xe0\xec\xe8\xf7\xe5\xf1\xea\xe8\xe9\x20\xe8\xed\xf2\xe5\xf0\xf4\xe5\xe9\xf1\x2c\x20\xe2\x20\xea\xee\xf2\xee\xf0"
        "\xee\xec\x20\x27\xe7\xeb\xee\xe5\x27\x20\xe3\xf0\xe0\xf4\xe8\xf7\xe5\xf1\xea\xee\xe5\x20\xee\xf4\xee\xf0\xec\xeb\xe5\xed\xe8\xe5\x20\xe8\xf1\xef\xee\xeb\xfc\xe7\xf3\xe5\xf2\xf1\xff\x20\xe4\xeb"
        "\xff\x20\xf2\xf0\xe5\xf5\x20\x27\xe7\xeb\xfb\xf5\x27\x20\xea\xeb\xe0\xf1\xf1\xee\xe2\x20\xe3\xe5\xf0\xee\xe5\xe2\x20\x28\xe2\xe0\xf0\xe2\xe0\xf0\xe0\x2c\x20\xf7\xe5\xf0\xed\xee\xea\xed\xe8\xe6"
        "\xed\xe8\xea\xe0\x20\xe8\x20\xed\xe5\xea\xf0\xee\xec\xe0\xed\xf2\xe0\x29\x2e"),
    /* {Быстрый бой}\n\nПри включении этой опции перед каждым сражением компьютер будет делать запрос о проведении этого сражения в режиме быстрого боя. Сражение протекает автоматически, и компьютер демонстрирует вам только его результат. */ DATA_COMPGEN(0x00513de4, gSPanelHelpVideoDeterminesIfTheVideoSequencesPlayNormally8,
        "\x7b\xc1\xfb\xf1\xf2\xf0\xfb\xe9\x20\xe1\xee\xe9\x7d\x0a\x0a\xcf\xf0\xe8\x20\xe2\xea\xeb\xfe\xf7\xe5\xed\xe8\xe8\x20\xfd\xf2\xee\xe9\x20\xee\xef\xf6\xe8\xe8\x20\xef\xe5\xf0\xe5\xe4\x20\xea\xe0"
        "\xe6\xe4\xfb\xec\x20\xf1\xf0\xe0\xe6\xe5\xed\xe8\xe5\xec\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\x20\xe1\xf3\xe4\xe5\xf2\x20\xe4\xe5\xeb\xe0\xf2\xfc\x20\xe7\xe0\xef\xf0\xee\xf1\x20\xee\x20\xef"
        "\xf0\xee\xe2\xe5\xe4\xe5\xed\xe8\xe8\x20\xfd\xf2\xee\xe3\xee\x20\xf1\xf0\xe0\xe6\xe5\xed\xe8\xff\x20\xe2\x20\xf0\xe5\xe6\xe8\xec\xe5\x20\xe1\xfb\xf1\xf2\xf0\xee\xe3\xee\x20\xe1\xee\xff\x2e\x20"
        "\xd1\xf0\xe0\xe6\xe5\xed\xe8\xe5\x20\xef\xf0\xee\xf2\xe5\xea\xe0\xe5\xf2\x20\xe0\xe2\xf2\xee\xec\xe0\xf2\xe8\xf7\xe5\xf1\xea\xe8\x2c\x20\xe8\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\x20\xe4\xe5"
        "\xec\xee\xed\xf1\xf2\xf0\xe8\xf0\xf3\xe5\xf2\x20\xe2\xe0\xec\x20\xf2\xee\xeb\xfc\xea\xee\x20\xe5\xe3\xee\x20\xf0\xe5\xe7\xf3\xeb\xfc\xf2\xe0\xf2\x2e"),
    /* {Курсор}\n\nПереключение курсора с черно-белого на цветной и обратно. Цветной курсор выглядит симпатичнее, но иногда он перемещается по экрану не так плавно, как черно-белый. */ DATA_COMPGEN(0x00513ecc, gSPanelHelpMouseCursorToggleColorCursorsOnOffColor9,
        "\x7b\xca\xf3\xf0\xf1\xee\xf0\x7d\x0a\x0a\xcf\xe5\xf0\xe5\xea\xeb\xfe\xf7\xe5\xed\xe8\xe5\x20\xea\xf3\xf0\xf1\xee\xf0\xe0\x20\xf1\x20\xf7\xe5\xf0\xed\xee\x2d\xe1\xe5\xeb\xee\xe3\xee\x20\xed\xe0"
        "\x20\xf6\xe2\xe5\xf2\xed\xee\xe9\x20\xe8\x20\xee\xe1\xf0\xe0\xf2\xed\xee\x2e\x20\xd6\xe2\xe5\xf2\xed\xee\xe9\x20\xea\xf3\xf0\xf1\xee\xf0\x20\xe2\xfb\xe3\xeb\xff\xe4\xe8\xf2\x20\xf1\xe8\xec\xef"
        "\xe0\xf2\xe8\xf7\xed\xe5\xe5\x2c\x20\xed\xee\x20\xe8\xed\xee\xe3\xe4\xe0\x20\xee\xed\x20\xef\xe5\xf0\xe5\xec\xe5\xf9\xe0\xe5\xf2\xf1\xff\x20\xef\xee\x20\xfd\xea\xf0\xe0\xed\xf3\x20\xed\xe5\x20"
        "\xf2\xe0\xea\x20\xef\xeb\xe0\xe2\xed\xee\x2c\x20\xea\xe0\xea\x20\xf7\xe5\xf0\xed\xee\x2d\xe1\xe5\xeb\xfb\xe9\x2e")
};
DATA(0x004ff308) char* xBarrierColor[KB_BARRIER_COLOR_NAME_COUNT] =
    {/* Сизый */ DATA_COMPGEN(0x00513f7c, xBarrierColorAqua0, "\xd1\xe8\xe7\xfb\xe9"), /* Синий */ DATA_COMPGEN(0x00513f84, xBarrierColorBlue1, "\xd1\xe8\xed\xe8\xe9"), /* Коричневый */ DATA_COMPGEN(0x00513f8c, xBarrierColorBrown2, "\xca\xee\xf0\xe8\xf7\xed\xe5\xe2\xfb\xe9"), /* Золотой */ DATA_COMPGEN(0x00513f98, xBarrierColorGold3, "\xc7\xee\xeb\xee\xf2\xee\xe9"), /* Зеленый */ DATA_COMPGEN(0x00513fa0, xBarrierColorGreen4, "\xc7\xe5\xeb\xe5\xed\xfb\xe9"), /* Оранжевый */ DATA_COMPGEN(0x00513fa8, xBarrierColorOrange5, "\xce\xf0\xe0\xed\xe6\xe5\xe2\xfb\xe9"), /* Фиолетовый */ DATA_COMPGEN(0x00513fb4, xBarrierColorPurple6, "\xd4\xe8\xee\xeb\xe5\xf2\xee\xe2\xfb\xe9"), /* Красный */ DATA_COMPGEN(0x00513fc0, xBarrierColorRed7, "\xca\xf0\xe0\xf1\xed\xfb\xe9")};
DATA(0x004ff328) char* xGenericSiteNames[KB_GENERIC_SITE_NAME_COUNT] = {
    /* Башня алхимика */ DATA_COMPGEN(0x00513fc8, xGenericSiteNamesAlchemistSTower0, "\xc1\xe0\xf8\xed\xff\x20\xe0\xeb\xf5\xe8\xec\xe8\xea\xe0"),
    /* Арена */ DATA_COMPGEN(0x00513fd8, xGenericSiteNamesArena1, "\xc0\xf0\xe5\xed\xe0"),
    /* Лачуга волхва */ DATA_COMPGEN(0x00513fe0, xGenericSiteNamesHutOfTheMagi2, "\xcb\xe0\xf7\xf3\xe3\xe0\x20\xe2\xee\xeb\xf5\xe2\xe0"),
    /* Око волхва */ DATA_COMPGEN(0x00513ff0, xGenericSiteNamesEyeOfTheMagi3, "\xce\xea\xee\x20\xe2\xee\xeb\xf5\xe2\xe0"),
    /* Конюшни */ DATA_COMPGEN(0x00513ffc, xGenericSiteNamesStables4, "\xca\xee\xed\xfe\xf8\xed\xe8"),
    /* Русалка */ DATA_COMPGEN(0x00514004, xGenericSiteNamesMermaid5, "\xd0\xf3\xf1\xe0\xeb\xea\xe0"),
    /* Сирены */ DATA_COMPGEN(0x0051400c, xGenericSiteNamesSirens6, "\xd1\xe8\xf0\xe5\xed\xfb")
};
DATA(0x004ff344) char* xRecruitmentSiteNames[KB_RECRUITMENT_SITE_NAME_COUNT] = {
    /* Земляные холмы */ DATA_COMPGEN(0x00514014, xRecruitmentSiteNamesBarrowMounds0, "\xc7\xe5\xec\xeb\xff\xed\xfb\xe5\x20\xf5\xee\xeb\xec\xfb"),
    /* Алтарь Земли */ DATA_COMPGEN(0x00514024, xRecruitmentSiteNamesEarthSummoningAltar1, "\xc0\xeb\xf2\xe0\xf0\xfc\x20\xc7\xe5\xec\xeb\xe8"),
    /* Алтарь Воздуха */ DATA_COMPGEN(0x00514034, xRecruitmentSiteNamesAirSummoningAltar2, "\xc0\xeb\xf2\xe0\xf0\xfc\x20\xc2\xee\xe7\xe4\xf3\xf5\xe0"),
    /* Алтарь Огня */ DATA_COMPGEN(0x00514044, xRecruitmentSiteNamesFireSummoningAltar3, "\xc0\xeb\xf2\xe0\xf0\xfc\x20\xce\xe3\xed\xff"),
    /* Алтарь Воды */ DATA_COMPGEN(0x00514050, xRecruitmentSiteNamesWaterSummoningAltar4, "\xc0\xeb\xf2\xe0\xf0\xfc\x20\xc2\xee\xe4\xfb")
};
DATA(0x004ff358) SWinSetup gWinSetup[KB_WIN_SETUP_COUNT] = {
    {0, 100, /* Построить: */ DATA_COMPGEN(0x0051405c, gWinSetupBuildImprovement0, "\xcf\xee\xf1\xf2\xf0\xee\xe8\xf2\xfc\x3a")},
    {1, 100, /* Скорость */ DATA_COMPGEN(0x00514068, gWinSetupSpeed1, "\xd1\xea\xee\xf0\xee\xf1\xf2\xfc")},
    {1, 101, /* Информация\nо враге */ DATA_COMPGEN(0x00514074, gWinSetupMonsterInfo2, "\xc8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xff\x0a\xee\x20\xe2\xf0\xe0\xe3\xe5")},
    {1, 102, /* Магия\nв автобое */ DATA_COMPGEN(0x00514088, gWinSetupAutoCombatSpellCasting3, "\xcc\xe0\xe3\xe8\xff\x0a\xe2\x20\xe0\xe2\xf2\xee\xe1\xee\xe5")},
    {1, 103, /* Сетка */ DATA_COMPGEN(0x00514098, gWinSetupGrid4, "\xd1\xe5\xf2\xea\xe0")},
    {1, 104, /* Курсор\nс тенью */ DATA_COMPGEN(0x005140a0, gWinSetupShadowCursor5, "\xca\xf3\xf0\xf1\xee\xf0\x0a\xf1\x20\xf2\xe5\xed\xfc\xfe")},
    {1, 105, /* Затенение\nсетки */ DATA_COMPGEN(0x005140b0, gWinSetupShadowMovement6, "\xc7\xe0\xf2\xe5\xed\xe5\xed\xe8\xe5\x0a\xf1\xe5\xf2\xea\xe8")},
    {2, 100, /* Музыка */ DATA_COMPGEN(0x005140c0, gWinSetupMusic7, "\xcc\xf3\xe7\xfb\xea\xe0")},
    {2, 101, /* Эффекты */ DATA_COMPGEN(0x005140c8, gWinSetupEffects8, "\xdd\xf4\xf4\xe5\xea\xf2\xfb")},
    {2, 102, /* Тип музыки */ DATA_COMPGEN(0x005140d0, gWinSetupMusicType9, "\xd2\xe8\xef\x20\xec\xf3\xe7\xfb\xea\xe8")},
    {2, 103, /* Скорость */ DATA_COMPGEN(0x005140dc, gWinSetupSpeed10, "\xd1\xea\xee\xf0\xee\xf1\xf2\xfc")},
    {2, 104, /* Путь */ DATA_COMPGEN(0x005140e8, gWinSetupShowPath11, "\xcf\xf3\xf2\xfc")},
    {2, 105, /* Враг */ DATA_COMPGEN(0x005140f0, gWinSetupEnemySpeed12, "\xc2\xf0\xe0\xe3")},
    {2, 106, /* Интерфейс */ DATA_COMPGEN(0x005140f8, gWinSetupInterface13, "\xc8\xed\xf2\xe5\xf0\xf4\xe5\xe9\xf1")},
    {2, 107, /* Видео */ DATA_COMPGEN(0x00514104, gWinSetupVideo14, "\xc2\xe8\xe4\xe5\xee")},
    {2, 108, /* Тип курсора */ DATA_COMPGEN(0x0051410c, gWinSetupMouseCursor15, "\xd2\xe8\xef\x20\xea\xf3\xf0\xf1\xee\xf0\xe0")},
    {6, 300, /* Атака */ DATA_COMPGEN(0x00514118, gWinSetupAttackSkill16, "\xc0\xf2\xe0\xea\xe0")},
    {6, 301, /* Защита */ DATA_COMPGEN(0x00514120, gWinSetupDefenseSkill17, "\xc7\xe0\xf9\xe8\xf2\xe0")},
    {6, 302, /* Сила магии */ DATA_COMPGEN(0x00514128, gWinSetupSpellPower18, "\xd1\xe8\xeb\xe0\x20\xec\xe0\xe3\xe8\xe8")},
    {6, 303, /* Знания */ DATA_COMPGEN(0x00514134, gWinSetupKnowledge19, "\xc7\xed\xe0\xed\xe8\xff")},
    {7, 600, /* Сложность игры: */ DATA_COMPGEN(0x0051413c, gWinSetupGameDifficulty20, "\xd1\xeb\xee\xe6\xed\xee\xf1\xf2\xfc\x20\xe8\xe3\xf0\xfb\x3a")},
    {7, 57, /* Легкая */ DATA_COMPGEN(0x0051414c, gWinSetupEasy21, "\xcb\xe5\xe3\xea\xe0\xff")},
    {7, 58, /* Обычная */ DATA_COMPGEN(0x00514154, gWinSetupNormal22, "\xce\xe1\xfb\xf7\xed\xe0\xff")},
    {7, 59, /* Тяжелая */ DATA_COMPGEN(0x0051415c, gWinSetupHard23, "\xd2\xff\xe6\xe5\xeb\xe0\xff")},
    {7, 60, /* Эксперт */ DATA_COMPGEN(0x00514164, gWinSetupExpert24, "\xdd\xea\xf1\xef\xe5\xf0\xf2")},
    {7, 61, /* Невозможная */ DATA_COMPGEN(0x0051416c, gWinSetupImpossible25, "\xcd\xe5\xe2\xee\xe7\xec\xee\xe6\xed\xe0\xff")},
    {7, 62, /* Оппоненты: */ DATA_COMPGEN(0x00514178, gWinSetupOpponents26, "\xce\xef\xef\xee\xed\xe5\xed\xf2\xfb\x3a")},
    {7, 84, /* Класс: */ DATA_COMPGEN(0x00514184, gWinSetupClass27, "\xca\xeb\xe0\xf1\xf1\x3a")},
    {9, 41, /* Золота в день: */ DATA_COMPGEN(0x0051418c, gWinSetupGoldPerDay28, "\xc7\xee\xeb\xee\xf2\xe0\x20\xe2\x20\xe4\xe5\xed\xfc\x3a")},
    {12, 0, /* Строить корабль: */ DATA_COMPGEN(0x0051419c, gWinSetupBuildANewShip29, "\xd1\xf2\xf0\xee\xe8\xf2\xfc\x20\xea\xee\xf0\xe0\xe1\xeb\xfc\x3a")},
    {12, 1, /* Цена: */ DATA_COMPGEN(0x005141b0, gWinSetupResourceCost30, "\xd6\xe5\xed\xe0\x3a")},
    {14, 800, /* 1-й */ DATA_COMPGEN(0x005141b8, gWinSetup1st31, "\x31\x2d\xe9")},
    {14, 801, /* 2-й */ DATA_COMPGEN(0x005141bc, gWinSetup2nd32, "\x32\x2d\xe9")},
    {14, 802, /* 3-й */ DATA_COMPGEN(0x005141c0, gWinSetup3rd33, "\x33\x2d\xe9")},
    {14, 803, /* 4-й */ DATA_COMPGEN(0x005141c4, gWinSetup4th34, "\x34\x2d\xe9")},
    {14, 804, /* 5-й */ DATA_COMPGEN(0x005141c8, gWinSetup5th35, "\x35\x2d\xe9")},
    {14, 805, /* 6-й */ DATA_COMPGEN(0x005141cc, gWinSetup6th36, "\x36\x2d\xe9")},
    {14, 604, /* Городов: */ DATA_COMPGEN(0x005141d0, gWinSetupNumberOfTowns37, "\xc3\xee\xf0\xee\xe4\xee\xe2\x3a")},
    {14, 605, /* Замков: */ DATA_COMPGEN(0x005141dc, gWinSetupNumberOfCastles38, "\xc7\xe0\xec\xea\xee\xe2\x3a")},
    {14, 606, /* Героев: */ DATA_COMPGEN(0x005141e4, gWinSetupNumberOfHeroes39, "\xc3\xe5\xf0\xee\xe5\xe2\x3a")},
    {14, 607, /* Золота в казне: */ DATA_COMPGEN(0x005141ec, gWinSetupGoldInTreasury40, "\xc7\xee\xeb\xee\xf2\xe0\x20\xe2\x20\xea\xe0\xe7\xed\xe5\x3a")},
    {14, 608, /* Дерево и руда: */ DATA_COMPGEN(0x005141fc, gWinSetupWoodOre41, "\xc4\xe5\xf0\xe5\xe2\xee\x20\xe8\x20\xf0\xf3\xe4\xe0\x3a")},
    {14, 609, /* Прочие ресурсы: */ DATA_COMPGEN(0x0051420c, gWinSetupGemsCrSlfMer42, "\xcf\xf0\xee\xf7\xe8\xe5\x20\xf0\xe5\xf1\xf3\xf0\xf1\xfb\x3a")},
    {14, 610, /* Найдено обелисков: */ DATA_COMPGEN(0x0051421c, gWinSetupObelisksFound43, "\xcd\xe0\xe9\xe4\xe5\xed\xee\x20\xee\xe1\xe5\xeb\xe8\xf1\xea\xee\xe2\x3a")},
    {14, 611, /* Артефакты: */ DATA_COMPGEN(0x00514230, gWinSetupArtifacts44, "\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\xfb\x3a")},
    {14, 612, /* Общая сила армии: */ DATA_COMPGEN(0x0051423c, gWinSetupTotalArmyStrength45, "\xce\xe1\xf9\xe0\xff\x20\xf1\xe8\xeb\xe0\x20\xe0\xf0\xec\xe8\xe8\x3a")},
    {14, 613, /* Доход: */ DATA_COMPGEN(0x00514250, gWinSetupIncome46, "\xc4\xee\xf5\xee\xe4\x3a")},
    {14, 620, /* Лучший герой: */ DATA_COMPGEN(0x00514258, gWinSetupBestHero47, "\xcb\xf3\xf7\xf8\xe8\xe9\x20\xe3\xe5\xf0\xee\xe9\x3a")},
    {14, 621, /* Лучшие параметры: */ DATA_COMPGEN(0x00514268, gWinSetupBestHeroStats48, "\xcb\xf3\xf7\xf8\xe8\xe5\x20\xef\xe0\xf0\xe0\xec\xe5\xf2\xf0\xfb\x3a")},
    {14, 622, /* Характер: */ DATA_COMPGEN(0x0051427c, gWinSetupPersonality49, "\xd5\xe0\xf0\xe0\xea\xf2\xe5\xf0\x3a")},
    {14, 623, /* Лучший воин: */ DATA_COMPGEN(0x00514288, gWinSetupBestMonster50, "\xcb\xf3\xf7\xf8\xe8\xe9\x20\xe2\xee\xe8\xed\x3a")},
    {14, 0, /* Гильдия воров: достижения игроков */ DATA_COMPGEN(0x00514298, gWinSetupThievesGuildPlayerRankings51, "\xc3\xe8\xeb\xfc\xe4\xe8\xff\x20\xe2\xee\xf0\xee\xe2\x3a\x20\xe4\xee\xf1\xf2\xe8\xe6\xe5\xed\xe8\xff\x20\xe8\xe3\xf0\xee\xea\xee\xe2")},
    {17, 110, /* Доступные заклинания были записаны в книгу. */ DATA_COMPGEN(0x005142bc, gWinSetupTheAboveSpellsHaveBeenAddedToYour52, "\xc4\xee\xf1\xf2\xf3\xef\xed\xfb\xe5\x20\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff\x20\xe1\xfb\xeb\xe8\x20\xe7\xe0\xef\xe8\xf1\xe0\xed\xfb\x20\xe2\x20\xea\xed\xe8\xe3\xf3\x2e")},
    {18, 600, /* Атака: */ DATA_COMPGEN(0x005142e8, gWinSetupAttack53, "\xc0\xf2\xe0\xea\xe0\x3a")},
    {18, 601, /* Защита: */ DATA_COMPGEN(0x005142f0, gWinSetupDefense54, "\xc7\xe0\xf9\xe8\xf2\xe0\x3a")},
    {18, 602, /* Сила магии: */ DATA_COMPGEN(0x005142f8, gWinSetupSpellPower55, "\xd1\xe8\xeb\xe0\x20\xec\xe0\xe3\xe8\xe8\x3a")},
    {18, 603, /* Знания: */ DATA_COMPGEN(0x00514304, gWinSetupKnowledge56, "\xc7\xed\xe0\xed\xe8\xff\x3a")},
    {18, 604, /* Очки магии: */ DATA_COMPGEN(0x0051430c, gWinSetupSpellPoints57, "\xce\xf7\xea\xe8\x20\xec\xe0\xe3\xe8\xe8\x3a")},
    {19, 600, /* Оборона: */ DATA_COMPGEN(0x00514318, gWinSetupDefenders58, "\xce\xe1\xee\xf0\xee\xed\xe0\x3a")},
    {20, 600, /* Нанять героя */ DATA_COMPGEN(0x00514324, gWinSetupRecruitHero59, "\xcd\xe0\xed\xff\xf2\xfc\x20\xe3\xe5\xf0\xee\xff")},
    {21, 600, /* Атака */ DATA_COMPGEN(0x00514334, gWinSetupAttackSkill60, "\xc0\xf2\xe0\xea\xe0")},
    {21, 601, /* Защита */ DATA_COMPGEN(0x0051433c, gWinSetupDefenseSkill61, "\xc7\xe0\xf9\xe8\xf2\xe0")},
    {21, 602, /* Сила магии */ DATA_COMPGEN(0x00514344, gWinSetupSpellPower62, "\xd1\xe8\xeb\xe0\x20\xec\xe0\xe3\xe8\xe8")},
    {21, 603, /* Знания */ DATA_COMPGEN(0x00514350, gWinSetupKnowledge63, "\xc7\xed\xe0\xed\xe8\xff")},
    {22, 0, /* Таверна */ DATA_COMPGEN(0x00514358, gWinSetupTavern64, "\xd2\xe0\xe2\xe5\xf0\xed\xe0")},
    {23, 600, /* Сложность\nкарты */ DATA_COMPGEN(0x00514360, gWinSetupMapDifficulty65, "\xd1\xeb\xee\xe6\xed\xee\xf1\xf2\xfc\x0a\xea\xe0\xf0\xf2\xfb")},
    {23, 601, /* Сложность\nигры */ DATA_COMPGEN(0x00514370, gWinSetupGameDifficulty66, "\xd1\xeb\xee\xe6\xed\xee\xf1\xf2\xfc\x0a\xe8\xe3\xf0\xfb")},
    {23, 602, /* \nРейтинг */ DATA_COMPGEN(0x00514380, gWinSetupRating67, "\x0a\xd0\xe5\xe9\xf2\xe8\xed\xe3")},
    {23, 603, /* Размер\nкарты */ DATA_COMPGEN(0x0051438c, gWinSetupMapSize68, "\xd0\xe0\xe7\xec\xe5\xf0\x0a\xea\xe0\xf0\xf2\xfb")},
    {23, 604, /* Оппоненты */ DATA_COMPGEN(0x0051439c, gWinSetupOpponents69, "\xce\xef\xef\xee\xed\xe5\xed\xf2\xfb")},
    {23, 605, /* Классы */ DATA_COMPGEN(0x005143a8, gWinSetupClass70, "\xca\xeb\xe0\xf1\xf1\xfb")},
    {23, 606, /* Условия\nпобеды */ DATA_COMPGEN(0x005143b0, gWinSetupVictoryConditions71, "\xd3\xf1\xeb\xee\xe2\xe8\xff\x0a\xef\xee\xe1\xe5\xe4\xfb")},
    {23, 607, /* Условия\nпоражения */ DATA_COMPGEN(0x005143c0, gWinSetupLossConditions72, "\xd3\xf1\xeb\xee\xe2\xe8\xff\x0a\xef\xee\xf0\xe0\xe6\xe5\xed\xe8\xff")}
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
DATA(0x005266d8) i32 bKBDone = 0;
DATA(0x005266dc) struct _REDBOOK* hRedbookz = NULL;
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
DATA(0x00526398) i32 giRandomClouds;
DATA(0x00526110) char cOverrideDigitalDriver[GLOBAL_DRIVER_NAME_SIZE];
DATA(0x00526140) i32 giBottomViewOverrideEndTime;
DATA(0x005265b0) i8 gArmyEffected[COMBAT_SIDE_COUNT][KB_ARMY_EFFECT_COUNT];
DATA(0x005258bc) H2_ENUM_STORAGE(ResourceType, i32) giBottomViewResource;
DATA(0x00524e04) b32 gbInCampaign;
DATA(0x00526130) i32 giResExtra1;
DATA(0x00526138) i32 giResExtra2;
DATA(0x00524de4) i8 puzzlePiecesRemoved[PUZZLE_PIECE_STORAGE_SIZE];
DATA(0x00526164) i32 giSeedingValid;
DATA(0x0052613c) i32 giLimitPlayer;
DATA(0x005265a8) i32 giShowClouds;
DATA(0x005263fc) i32 bDoColorCycle;
DATA(0x00526100) inputManager* gpInputManager;
DATA(0x00525a58) i32 iMaxMapExtra;
DATA(0x00525be4) palette* gPalette;
DATA(0x005258d0) resourceManager* gpResourceManager;
DATA(0x00524a34) char gcBotViewText[GLOBAL_BOTTOM_VIEW_TEXT_SIZE];
DATA(0x005258c4) i32 bSpecialHideCursor;
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
DATA(0x005261ac) i32 giFullySeeded;
DATA(0x00524de0) icon* gBuyBuildIcons;
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
DATA(0x00524f94) i32 bFreshSave;
DATA(0x005265ac) i32 bShowIt;
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
DATA(0x00526144) b8 gbGamePosToNetPos[OLD_MAIN_MATCH_BUFFER_SIZE];
