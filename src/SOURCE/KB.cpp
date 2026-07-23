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
    CHOICE_NO_AMOUNT      = -1,
    CHOICE_BASIC_SKILL    = 1,
    CHOICE_RESOURCE_BONUS = 20,
    CHOICE_GOLD_BONUS     = 2000
H2_ENUM_END(CampaignChoiceAmount)

H2_ENUM_BEGIN(CheckEndGameConstants)
    END_GAME_NO_PLAYER               = -1,
    END_GAME_GRACE_DAYS              = 7,
    END_GAME_TEXT_BUFFER_SIZE        = 100,
    END_GAME_GOLD_SCALE              = 1000,
    END_GAME_ULTIMATE_ARTIFACT       = 0,
    END_GAME_PLAYER_DIALOG_ICON      = 9,
    END_GAME_REMOTE_DIALOG_TIME      = 5000,
    END_GAME_CAMPAIGN_SAVE_NAME_SIZE = 20,
    END_GAME_SCENARIO_OFFSET         = 1
H2_ENUM_END(CheckEndGameConstants)

H2_ENUM_BEGIN(CheckEndGameCampaignScenario)
    END_GAME_DWARF_SCENARIO          = 3,
    END_GAME_SIDE_SCENARIO           = 7,
    END_GAME_ROLAND_CAPTURE_SCENARIO = 9,
    END_GAME_FIRST_NO_SAVE_SCENARIO  = 10,
    END_GAME_LAST_SCENARIO           = 11
H2_ENUM_END(CheckEndGameCampaignScenario)

H2_ENUM_CLASS_BEGIN(MoraleInfoTextIndex)
    MORALE_INFO_GOOD          = 0,
    MORALE_INFO_NEUTRAL       = 1,
    MORALE_INFO_BAD           = 2,
    MORALE_INFO_HEADER        = 3,
    INFO_SAME_ALIGNMENT       = 5,
    INFO_THREE_ALIGNMENTS     = 6,
    INFO_FOUR_ALIGNMENTS      = 7,
    INFO_MEDAL_OF_VALOR       = 8,
    INFO_MEDAL_OF_COURAGE     = 9,
    INFO_MEDAL_OF_HONOR       = 10,
    INFO_MEDAL_OF_DISTINCTION = 11,
    INFO_FIZBIN               = 12,
    INFO_BUOY                 = 13,
    INFO_OASIS                = 14,
    INFO_TEMPLE               = 15,
    INFO_GRAVEYARD            = 16,
    INFO_SHIPWRECK            = 17,
    MORALE_INFO_NONE          = 19,
    INFO_FIVE_ALIGNMENTS      = 20,
    INFO_ALL_UNDEAD           = 21,
    INFO_SOME_UNDEAD          = 22,
    INFO_WATERING_HOLE        = 23,
    INFO_DERELICT_SHIP        = 24,
    INFO_COLISEUM             = 25,
    INFO_TAVERN               = 26,
    INFO_BASIC_LEADERSHIP     = 27,
    INFO_ADVANCED_LEADERSHIP  = 28,
    INFO_EXPERT_LEADERSHIP    = 29,
    MORALE_INFO_MASTHEAD      = 30,
    MORALE_INFO_BATTLE_GARB   = 31
H2_ENUM_CLASS_END(MoraleInfoTextIndex)

H2_ENUM_CLASS_BEGIN(LuckInfoTextIndex)
    LUCK_INFO_GOOD        = 0,
    LUCK_INFO_NEUTRAL     = 1,
    LUCK_INFO_BAD         = 2,
    LUCK_INFO_HEADER      = 3,
    INFO_RABBIT_FOOT      = 4,
    INFO_HORSESHOE        = 5,
    INFO_LUCKY_COIN       = 6,
    INFO_CLOVER           = 7,
    INFO_FAERIE_RING      = 8,
    INFO_FOUNTAIN         = 9,
    LUCK_INFO_NONE        = 10,
    INFO_RAINBOW          = 12,
    INFO_IDOL             = 13,
    INFO_PYRAMID          = 14,
    INFO_BASIC_SKILL      = 15,
    INFO_ADVANCED_SKILL   = 16,
    INFO_EXPERT_SKILL     = 17,
    LUCK_INFO_MASTHEAD    = 18,
    INFO_MERMAID          = 19,
    LUCK_INFO_BATTLE_GARB = 20
H2_ENUM_CLASS_END(LuckInfoTextIndex)

H2_ENUM_BEGIN(MoraleLuckInfoConstant)
    MORALE_LUCK_DESCRIPTION_SIZE = 200
H2_ENUM_END(MoraleLuckInfoConstant)

H2_ENUM_BEGIN(NetBoxLocalConstant)
    BOX_WINDOW_Y             = 0x19b,
    BOX_HEIGHT               = 0x44,
    BOX_WIDTH                = 0x27f,
    BOX_INPUT_Y              = 0x1d1,
    BOX_INPUT_HEIGHT         = 0xc,
    BOX_TEXT_LENGTH          = 150,
    BOX_LINE_COUNT           = 4,
    BOX_LINE_TEXT_LIMIT      = 120,
    BOX_TEXT_X               = 20,
    BOX_TEXT_Y               = 54,
    BOX_MAX_INPUT            = 0x5d,
    BOX_MAX_COLOR            = 6,
    BOX_DEFAULT_COLOR        = BOX_MAX_COLOR,
    BOX_COLOR_FRAME_OFFSET   = 1,
    BOX_CURSOR_DELAY         = 0x168,
    BOX_CURSOR_WIDTH_PADDING = 0x32,
    BOX_CURSOR_WIDTH_LIMIT   = 0x25a,
    BOX_MESSAGE_TIMEOUT      = 6000,
    BOX_EXIT_DELAY_STEPS     = 20,
    BOX_EXIT_DELAY           = 75,
    BOX_FIRST_LINE_ID        = 1,
    BOX_FIRST_COLOR_ID       = 0x14,
    BOX_INPUT_ID             = 5,
    BOX_THIS_PLAYER_COLOR_ID = 0x18,
    BOX_KEY_ESCAPE           = 0x1b,
    BOX_KEY_ENTER            = 10,
    BOX_KEY_BACKSPACE        = 0x7f,
    BOX_KEY_F1               = 0x3b00,
    BOX_CURSOR_GLYPH         = 0x1f,
    BOX_PACKET_BUFFER_SIZE   = 0x7f,
    BOX_FIRST_PRINTABLE      = 0x20,
    BOX_LAST_PRINTABLE       = 0x7f,
    BOX_REMOTE_MAP_CHANGE    = 0x29,
    BOX_REMOTE_SETUP         = 0x20,
    BOX_REMOTE_SAVE          = 1,
    BOX_REMOTE_CHAT          = 0xb
H2_ENUM_END(NetBoxLocalConstant)

H2_ENUM_BEGIN(PollSoundConstant)
    MOUSE_UPDATE_INTERVAL           = 13,
    COMBAT_COLOR_CYCLE_INTERVAL     = 110,
    DEFAULT_COLOR_CYCLE_INTERVAL    = 200,
    NON_PALETTED_COLOR_CYCLE_DELAY  = 300,
    SOUND_POLL_INTERVAL             = 30,
    PALETTED_VIDEO_MODE_COLOR_DEPTH = 8
H2_ENUM_END(PollSoundConstant)

H2_ENUM_BEGIN(CongratsConstant)
    CONGRATS_PALETTE_BUFFER_SIZE = 0x304,
    CONGRATS_TEXT_SIZE           = 500,
    CONGRATS_RATING_LENGTH       = 32,
    CONGRATS_SMACKER             = 2,
    CONGRATS_DIFFICULTY_SCALE    = 100
H2_ENUM_END(CongratsConstant)

H2_ENUM_BEGIN(CommandLineConstant)
    LINE_TCP_TEXT_LENGTH    = 20,
    LINE_HELP_LINE_COUNT    = 14,
    LINE_HUMAN_PLAYER_SLOTS = 4,
    LINE_TCP_MIN_PLAYERS    = 2,
    LINE_TCP_MAX_PLAYERS    = 6,
    LINE_FRAME_STEP         = 6,
    LINE_TCP_TYPE_DEFAULT   = 0,
    LINE_TCP_TYPE_L         = 1,
    LINE_TCP_CLIENT         = 0,
    LINE_TCP_HOST           = 1,
    LINE_SINGLE_PLAYER      = 1
H2_ENUM_END(CommandLineConstant)

H2_ENUM_BEGIN(InitMenuConstant)
    MENU_HOTSPOT_COUNT     = 5,
    MENU_FIRST_COMMAND     = 0x65,
    MENU_NEW_GAME          = 0x65,
    MENU_LOAD_GAME         = 0x66,
    MENU_HIGH_SCORES       = 0x67,
    MENU_CREDITS           = 0x68,
    MENU_EXIT              = 0x69,
    MENU_LAST_ACTION       = 0x6b,
    MENU_MOVIE             = 0x6b,
    MENU_FIRST_WIDGET      = 11,
    MENU_LAST_WIDGET       = 15,
    MENU_WIDGET_OFFSET     = 11,
    MENU_KEY_EXIT          = 0x10,
    MENU_KEY_HIGH_SCORES   = 0x23,
    MENU_KEY_LOAD          = 0x26,
    MENU_KEY_CREDITS       = 0x2e,
    MENU_KEY_NEW           = 0x31,
    MENU_DISABLE_MASK      = 0x200,
    MENU_CLOSE_COMMAND     = 10,
    MENU_HELP_DIALOG       = 4,
    MENU_MOVIE_SMACKER     = 0x26,
    MENU_MAIN_MUSIC        = 0x2a,
    MENU_SCREEN_WIDTH      = 640,
    MENU_SCREEN_HEIGHT     = 480,
    MENU_FRAME_STRIDE      = 4,
    MENU_HOVER_FRAME       = 3,
    MENU_IDLE_FRAME        = 1,
    MENU_ACTIVE_FRAME      = 2,
    MENU_WIDGET_FRAME_BASE = 44,
    MENU_REDRAW_LEFT       = 0,
    MENU_REDRAW_TOP        = 105,
    MENU_REDRAW_WIDTH      = 565,
    MENU_REDRAW_HEIGHT     = 375,
    MENU_HELP_NEW_GAME     = 0,
    MENU_HELP_LOAD_GAME    = 1,
    MENU_HELP_HIGH_SCORES  = 2,
    MENU_HELP_CREDITS      = 3,
    MENU_HELP_EXIT         = 4
H2_ENUM_END(InitMenuConstant)

H2_ENUM_BEGIN(RecruitHeroConstant)
    RECRUIT_HERO_VIEW_BUTTON = 2,
    RECRUIT_HERO_FADE_STEPS  = 8
H2_ENUM_END(RecruitHeroConstant)

H2_ENUM_BEGIN(BuildingRuleConstant)
    BUILDING_INFO_BUFFER_SIZE                      = 400,
    MONSTER_RARE_RESOURCE_COST                     = 1,
    MONSTER_UPGRADED_RARE_RESOURCE_COST            = 2,
    NECROMANCER_CASTLE_UPGRADE_BASE_RESOURCE_VALUE = 1000
H2_ENUM_END(BuildingRuleConstant)

H2_ENUM_BEGIN(HighScoreInputConstant)
    HIGH_SCORE_INPUT_NAME_SIZE   = 20,
    HIGH_SCORE_LAST_SHIFT_SOURCE = HIGH_SCORE_ENTRY_COUNT - 2
H2_ENUM_END(HighScoreInputConstant)

#define RETAIL_FILE const_cast<char*>("I:\\Projects\\Heroes\\Prog\\SOURCE\\KB.CPP")

inline town* GetCastleRec(i32 i) {
    return &gpGame->m_castleRecs[i];
}

inline hero* GetHeroSlot(i32 i) {
    return &gpGame->m_heroRecs[i];
}

H2_ENUM_BEGIN(NormalDialogDrawRange)
    NORMAL_DIALOG_FOREGROUND_WIDGET_LIMIT   = 0x9000,
    NORMAL_DIALOG_BACKGROUND_WIDGET_LAST_ID = -256
H2_ENUM_END(NormalDialogDrawRange)

VA(0x00496450, 0x14e)
extern "C" void PollSound(void) {
    if (gbInPollSound)
        return;
    gbInPollSound = true;
    if (KBTickCount() > glTimers[GLOBAL_MOUSE_TIMER_SLOT] && !gbPutzingWithMouseCtr) {
        glTimers[GLOBAL_MOUSE_TIMER_SLOT] = KBTickCount() + MOUSE_UPDATE_INTERVAL;
        gpMouseManager->NewUpdate(0);
    }
    if (KBTickCount() > glTimers[GLOBAL_COLOR_CYCLE_TIMER_SLOT]) {
        if (giCycleType == WINDOW_COLOR_CYCLE_COMBAT
            || giCycleType == WINDOW_COLOR_CYCLE_COMBAT_ALTERNATE)
            glTimers[GLOBAL_COLOR_CYCLE_TIMER_SLOT] =
                KBTickCount() + COMBAT_COLOR_CYCLE_INTERVAL;
        else
            glTimers[GLOBAL_COLOR_CYCLE_TIMER_SLOT] =
                KBTickCount() + DEFAULT_COLOR_CYCLE_INTERVAL;
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
    if (KBTickCount() > glTimers[GLOBAL_POLL_SOUND_TIMER_SLOT]) {
        glTimers[GLOBAL_POLL_SOUND_TIMER_SLOT] = KBTickCount() + SOUND_POLL_INTERVAL;
        if (gbForegroundApp)
            gpSoundManager->PollSound();
        PollRemote();
    }
    gbInPollSound = false;
}

VA(0x0049659e, 0x20)
void ForcePollSound(void) {
    glTimers[GLOBAL_POLL_SOUND_TIMER_SLOT] = KBTickCount() - 1;
    PollSound();
}

VA(0x004965be, 0x39e)
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

VA(0x0049695c, 0x344)
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

VA(0x00496ca0, 0x39)
void EarlyShutdown(char* caption, char* text) {
    MessageBoxA(hwndApp, text, caption, MB_ICONHAND);
    exit(0);
}

VA(0x00496cd9, 0x148)
void SetupCDRom(void) {
    i32 savedNoSound = gbNoSound;
    if (iCDRomErr == CD_ROM_DRIVE_UNAVAILABLE) {
        SetPalette(gPalette->m_data, 1);
        gpMouseManager->ShowColorPointer();
        gbNoSound = true;
        if (giTCPHostStatus)
            NormalDialog(
                DATA_COMPGEN(0x005157b0, setupCDRomUnableToAccessCDROMDrive, "Unable to access CD-ROM Drive.  Without a CD-ROM drive and a Heroes 2 Expansion "
                "CD-ROM you will only be able to play as the guest in a multi-player game."),
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
        SetPalette(gPalette->m_data, 1);
        gpMouseManager->ShowColorPointer();
        gbNoSound = true;
        if (giTCPHostStatus)
            NormalDialog(
                DATA_COMPGEN(0x0051584c, setupCDRomTheHeroes2ExpansionCDROM, "The Heroes 2 Expansion CD-ROM is not in the drive.  Without a Heroes 2 Expansion "
                "CD-ROM you will only be able to play as the guest in a multi-player game.  If you "
                "have the CD, then exit the program, put the CD in, and try again."),
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
            DATA_COMPGEN(0x00515988, setupCDRomStartupError, "Startup Error"),
            DATA_COMPGEN(0x00515934, setupCDRomUnableToChangeToTheHeroes, "Unable to change to the Heroes II directory.  Please run the installation program.")
        );
        exit(0);
    }
    if (iCDRomErr == CD_ROM_DATA_FILES_MISSING) {
        EarlyShutdown(
            DATA_COMPGEN(0x005159e8, setupCDRomStartupError2, "Startup Error"),
            DATA_COMPGEN(0x00515998, setupCDRomUnableToFindTheHeroesII, "Unable to find the Heroes II data files.  Please run the installation program.")
        );
        exit(0);
    }
    gbNoSound = savedNoSound;
}

VA(0x00496e21, 0x77)
i32 EarlySetup(void) {
    if (bEarlySetupDone)
        return 0;
    InitMainClasses();
    GetGraphicsInfo();
    ReadPrefs();
    if (!InterpretCommandLine())
        return 1;
    LogTruncate();
    LogStr(DATA_COMPGEN(0x00515a04, earlySetupES1, "ES1"));
    iCDRomErr = SetupCDDrive();
    InitVars();
    LogStr(DATA_COMPGEN(0x00515a08, earlySetupES2, "ES2"));
    return 1;
}

VA(0x00496e98, 0x16c0)
i32 oldmain(void) {
    i32 command_a;
    i32 quit_g;
    i32 mainScreenLoaded_b;
    i32 firstMainScreen_c;
    i32 savedUpdateFlags_l;
    i32 player_h;
    i32 unusedMainState_o;
    i32 unusedMenuState_d;
    i32 unusedPlayerState_c;
    i32 netPlayer_i;
    i32 gamePlayer_m;
    i32 result_i;
    i32 transmissionResult_i;
    char matchedNetPlayers_d[OLD_MAIN_MATCH_BUFFER_SIZE];
    char matchedGamePlayers_e[OLD_MAIN_MATCH_BUFFER_SIZE];
    OldMainNetBuffer netBuffer_b;

    if (bKBDone)
        return 0;
    bKBDone = 1;
    LogStr(DATA_COMPGEN(0x00515a0c, oldmainOM1, "OM1"));
    LogStr(DATA_COMPGEN(0x00515a10, oldmainOM2, "OM2"));
    command_a = -1;
    if (gpExec->InitSystem())
        ShutDown(DATA_COMPGEN(0x00515a14, oldmainInitializationFailed, "Initialization failed!"));
    LogStr(DATA_COMPGEN(0x00515a2c, oldmainOM3, "OM3"));
    KBChangeMenu(hmnuDflt);
    gPalette = gpResourceManager->GetPalette(DATA_COMPGEN(0x00515a30, oldmainKbPal, "kb.pal"));
    gpWindowManager->m_updateFlags = 1;
    smallFont = gpResourceManager->GetFont(DATA_COMPGEN(0x00515a38, oldmainSmalfontFnt, "smalfont.fnt"));
    bigFont = gpResourceManager->GetFont(DATA_COMPGEN(0x00515a48, oldmainBigfontFnt, "bigfont.fnt"));
    gpMouseManager->SetPointer(DATA_COMPGEN(0x00515a54, oldmainAdvmiceMse, "advmice.mse"), 0, MOUSE_AUTO_CURSOR_TYPE);
    gpMouseManager->SetColorMice(gConfig.gfx[IDX(giCurExe)].colorMouseCursor);
    LogStr(DATA_COMPGEN(0x00515a60, oldmainOM4, "OM4"));
    SetupCDRom();
    LogStr(DATA_COMPGEN(0x00515a64, oldmainOM5, "OM5"));
    if (gpSoundManager->Open(-1))
        ShutDown(DATA_COMPGEN(0x00515a68, oldmainUnableToInitializeSound, "Unable to initialize sound."));
    if (giDebugLevel < OLD_MAIN_DEBUG_MEMORY_CHECK_LEVEL)
        CheckMem();
    LogStr(DATA_COMPGEN(0x00515a84, oldmainOM6, "OM6"));

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
            gShingleAnim = gpResourceManager->GetIcon(DATA_COMPGEN(0x00515a88, oldmainShnganimIcn, "shnganim.icn"));
        if (gGameCommand != OLD_MAIN_EXIT)
            gpSoundManager->SwitchAmbientMusic(OLD_MAIN_MAIN_MUSIC);

        if (!mainScreenLoaded_b) {
            if (gGameCommand != OLD_MAIN_EXIT) {
                gpResourceManager->GetBackdrop(DATA_COMPGEN(0x00515a98, oldmainHeroesIcn, "heroes.icn"), gpWindowManager->m_screen, 1);
                gpWindowManager
                    ->UpdateScreenRegion(0, 0, OLD_MAIN_SCREEN_WIDTH, OLD_MAIN_SCREEN_HEIGHT);
                if (firstMainScreen_c)
                    SetPalette(gPalette->m_data, 1);
                else
                    gpWindowManager->FadeScreen(FADE_IN, OLD_MAIN_FADE_SPEED, gPalette);
                firstMainScreen_c = 0;
            }
            gpMouseManager->SetPointer(DATA_COMPGEN(0x00515aa4, oldmainAdvmiceMse2, "advmice.mse"), 0, MOUSE_AUTO_CURSOR_TYPE);
        }
        mainScreenLoaded_b = 1;
        if (gGameCommand != OLD_MAIN_EXIT)
            gpWindowManager->m_updateFlags = 1;

        if (giTCPHostStatus != -1 && gbTCPFirstTime) {
            gbTCPFirstTime = false;
            giNumHumanPlayers = 1;
            iMPBaseType = MULTIPLAYER_BASE_NETWORK;
            iMPNetProtocol = OLD_MAIN_NETWORK_PROTOCOL;
            iMPExtendedType =
                giTCPHostStatus ? REMOTE_GAME_NETWORK_HOST : REMOTE_GAME_NETWORK_GUEST;
            giSetupGameType = static_cast<u8>(giTCPType);
            RemoteMain(iMPExtendedType);
            gbWaitForRemoteReceive = iMPExtendedType == REMOTE_GAME_NETWORK_GUEST;
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
                gpInitWin = new heroWindow(0, 0, DATA_COMPGEN(0x00515ab0, oldmainStpmainBin, "stpmain.bin"));
                if (!gpInitWin)
                    MemError();
                gbInSetupDialog = true;
                gpWindowManager->DoDialog(gpInitWin, InitMenuHandler, 0);
                delete gpInitWin;
                gpInitWin = NULL;
                command_a = gpWindowManager->m_dialogResult;
                gbInSetupDialog = false;
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
                    sprintf(cPlayerNames[player_h], DATA_COMPGEN(0x00515abc, oldmainEmptyString, ""));
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
                                gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_FADE_SPEED, gPalette);
                                mainScreenLoaded_b = 0;
                                goto main_menu;
                            }
                        } else {
                            if (xIsPlayingExpansionCampaign) {
                                if (xCampaign.HandleVictory()) {
                                    xCampaign.InitMap();
                                    goto initialize_game;
                                } else {
                                    gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_FADE_SPEED, gPalette);
                                    mainScreenLoaded_b = 0;
                                    goto main_menu;
                                }
                            } else {
                                LogStr(DATA_COMPGEN(0x00515ac0, oldmainNewGame1, "New Game 1"));
                                if (!gpGame->NewGame())
                                    goto main_menu;
                                LogStr(DATA_COMPGEN(0x00515acc, oldmainNewGame2, "New Game 2"));
                            }
                        }
                        break;
                    case OLD_MAIN_SETUP_LOAD:
                        LogStr(DATA_COMPGEN(0x00515ad8, oldmainLoadGame1, "Load Game 1"));
                        if (!gpGame->PickLoadGame())
                            goto main_menu;
                        LogStr(DATA_COMPGEN(0x00515ae4, oldmainLoadGame2, "Load Game 2"));
                        break;
                }
                goto game_setup_complete;
            case OLD_MAIN_HIGH_SCORES:
                if (gpExec->AddManager(gpHighScoreManager, -1))
                    ShutDown(DATA_COMPGEN(0x00515af0, oldmainCanTAddManager, "Can't add manager!"));
                gpExec->MainLoop();
                gpExec->RemoveManager(gpHighScoreManager);
                mainScreenLoaded_b = 0;
                goto main_menu;
            case OLD_MAIN_CREDITS:
                gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_FADE_SPEED, gPalette);
                PlaySmacker(OLD_MAIN_CREDITS_FIRST_VIDEO);
                PlaySmacker(OLD_MAIN_CREDITS_SECOND_VIDEO);
                mainScreenLoaded_b = 0;
                gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_LONG_FADE_SPEED, gPalette);
                goto main_menu;
            case OLD_MAIN_EXIT:
                quit_g = 1;
                break;
        }

    game_setup_complete:
        if (giMenuCommand == -1) {
            LogStr(DATA_COMPGEN(0x00515b04, oldmainDWM1, "DWM 1"));
            if (quit_g)
                goto game_finished;
            LogStr(DATA_COMPGEN(0x00515b0c, oldmainDWM2, "DWM 2"));
            if (gbRemoteOn && giThisNetPos == 0) {
                LogStr(DATA_COMPGEN(0x00515b14, oldmainDWM3, "DWM 3"));
                memset(matchedGamePlayers_e, 0, OLD_MAIN_PLAYER_COUNT);
                memset(matchedNetPlayers_d, 0, OLD_MAIN_PLAYER_COUNT);
                for (netPlayer_i = 0; netPlayer_i < OLD_MAIN_PLAYER_COUNT; netPlayer_i++) {
                    if (gbHumanPlayer[netPlayer_i]) {
                        for (gamePlayer_m = 0; gamePlayer_m < OLD_MAIN_PLAYER_COUNT;
                             gamePlayer_m++) {
                            char* defaultName = &gpGame->m_defaultPlayerNames
                                                     [gamePlayer_m * OLD_MAIN_DEFAULT_NAME_STRIDE];
                            if (strlen(defaultName) == OLD_MAIN_DEFAULT_NAME_LENGTH
                                && !strcmp(defaultName, gsNetPlayerInfo[netPlayer_i].name)
                                && !gpGame->m_playerDead[gamePlayer_m]
                                && !matchedGamePlayers_e[gamePlayer_m]
                                && !matchedNetPlayers_d[netPlayer_i]) {
                                matchedGamePlayers_e[gamePlayer_m] = 1;
                                matchedNetPlayers_d[netPlayer_i] = 1;
                                gbGamePosToNetPos[gamePlayer_m] = static_cast<i8>(netPlayer_i);
                            }
                        }
                    }
                }
                for (gamePlayer_m = 0;
                     gamePlayer_m < OLD_MAIN_PLAYER_COUNT && matchedGamePlayers_e[gamePlayer_m];
                     gamePlayer_m++) {
                }
                for (netPlayer_i = 0; netPlayer_i < OLD_MAIN_PLAYER_COUNT; netPlayer_i++) {
                    if (!matchedNetPlayers_d[netPlayer_i]) {
                        if (!gbHumanPlayer[netPlayer_i]) {
                            gbGamePosToNetPos[netPlayer_i] = -1;
                        } else {
                            gbGamePosToNetPos[netPlayer_i] = static_cast<i8>(gamePlayer_m);
                            strcpy(
                                &gpGame->m_defaultPlayerNames
                                     [gamePlayer_m * OLD_MAIN_DEFAULT_NAME_STRIDE],
                                gsNetPlayerInfo[netPlayer_i].name
                            );
                            for (gamePlayer_m++; gamePlayer_m < OLD_MAIN_PLAYER_COUNT
                                                 && matchedGamePlayers_e[gamePlayer_m];
                                 gamePlayer_m++) {
                            }
                        }
                    }
                }

                memcpy(netBuffer_b.setup.gamePosToNetPos, gbGamePosToNetPos, OLD_MAIN_PLAYER_COUNT);
                memcpy(
                    netBuffer_b.setup.players,
                    gsNetPlayerInfo,
                    sizeof(netBuffer_b.setup.players)
                );
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
                        netBuffer_b.bytes,
                        netPlayer_i,
                        sizeof(OldMainNetSetup),
                        OLD_MAIN_NETWORK_PACKET,
                        1,
                        1,
                        REMOTE_MESSAGE_DEFAULT
                    );
                    if (!transmissionResult_i)
                        ShutDown(NULL);
                }
                for (netPlayer_i = 1; netPlayer_i < giNumHumanPlayers; netPlayer_i++) {
                    if (!gpGame->TransmitSaveGame(netPlayer_i, 0, 1))
                        ShutDown(NULL);
                }
                memset(gbThisNetHumanPlayer, 0, OLD_MAIN_PLAYER_COUNT);
                gbThisNetHumanPlayer[giThisGamePos] = 1;
                iLastDiffSendTo = -1;
                gpGame->SaveGame(gConfig.rmtRLName, 0, 0);
            }
            LogStr(DATA_COMPGEN(0x00515b1c, oldmainDWM4, "DWM 4"));
            if (gbRemoteOn && gbWaitForRemoteReceive) {
                LogStr(DATA_COMPGEN(0x00515b24, oldmainDWM5, "DWM 5"));
                giWaitType = DIALOG_WAIT_OTHER_PLAYER;
                NormalDialog(
                    DATA_COMPGEN(0x00515b2c, oldmainWaitingToReceiveGameData, "Waiting to receive game data."),
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
                sprintf(gpGame->m_saveName, DATA_COMPGEN(0x00515b4c, oldmainNEWGAME, "NEWGAME"));
                iLastDiffSendTo = -1;
                gpGame->SaveGame(gConfig.rmtSLName, 0, 0);
            }
        }

    initialize_game:
        gpWindowManager->m_updateFlags = 1;
        if (gShingleAnim)
            gpResourceManager->Dispose(gShingleAnim);
        gShingleAnim = NULL;

        if (giNumHumanPlayers > 1) {
            for (player_h = 0; player_h < giNumHumanPlayers; player_h++) {
                if (iMPBaseType != MULTIPLAYER_BASE_HOT_SEAT)
                    strcpy(cPlayerNames[NetPosToGamePos(player_h)], gsNetPlayerInfo[player_h].name);
            }
        }
        for (player_h = 0; player_h < gpGame->m_playerCount; player_h++) {
            if (!strlen(cPlayerNames[player_h])) {
                sprintf(
                    cPlayerNames[player_h],
                    DATA_COMPGEN(0x00515b54, oldmainSPlayer, "%s player"),
                    gColors[gpGame->m_players[player_h].m_color]
                );
                cPlayerNames[player_h][0] -= ' ';
            }
        }
        ComputeAdvNetControl();
        gbGameInitialized = true;
        mainScreenLoaded_b = 0;
        gpSoundManager->StopAllSamples(1);
        gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_FADE_SPEED, NULL);
        gMapX = 0;
        gMapY = 0;
        for (giCurWatchPlayer = giCurPlayer; !gbThisNetHumanPlayer[giCurWatchPlayer];
             giCurWatchPlayer = (giCurWatchPlayer + 1) % gpGame->m_playerCount) {
        }
        giCurWatchPlayerBit = static_cast<u8>(1 << giCurWatchPlayer);

        if (gbInCampaign && gpGame->m_campaignScenarioWon) {
            giEndSequence = 1;
        } else if (xIsPlayingExpansionCampaign && xCampaign.IsThisMapCompleted()) {
            giEndSequence = 1;
        } else {
            if (gpExec->AddManager(gpAdvManager, -1))
                ShutDown(DATA_COMPGEN(0x00515b60, oldmainCanTAddManager2, "Can't add manager!"));
            if (command_a == OLD_MAIN_NEW_GAME) {
                gpAdvManager->SetHeroContext(gpGame->m_players[0].NextHero(0), 0);
            }
            if (command_a == OLD_MAIN_NEW_GAME || bForceCheckTimeEvent) {
                bForceCheckTimeEvent = 0;
                gpGame->CheckForTimeEvent();
            }
            gpExec->MainLoop();
            gMapX = gpAdvManager->m_mapOriginX;
            gMapY = gpAdvManager->m_mapOriginY;
            gpExec->RemoveManager(gpAdvManager);
            gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_FADE_SPEED, gPalette);
            if (!gbGameOver)
                goto game_finished;
        }

        RemoteCleanup();
        bShowIt = 1;
        gpMouseManager->SetPointer(DATA_COMPGEN(0x00515b74, oldmainAdvmiceMse3, "advmice.mse"), 0, MOUSE_AUTO_CURSOR_TYPE);
        sprintf(
            gcWinText,
            DATA_COMPGEN(0x00515b80, oldmainMyHeroesOurFoesHaveBeen, "My heroes, our foes have been scattered, their castles broken and laid bare.  The "
            "great campaign is now complete, and I stand before you as the undisputed High "
            "King!\n\nOur victory was achieved in %d days!"),
            giCurTurn
        );

        if (giEndSequence != 1) {
            gpWindowManager->m_updateFlags = 0;
            if (!xIsExpansionMap)
                PlaySmacker(OLD_MAIN_STANDARD_VICTORY_VIDEO);
            else
                PlaySmacker(OLD_MAIN_EXPANSION_VICTORY_VIDEO);
            gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_LONG_FADE_SPEED, gPalette);
            gpResourceManager->GetBackdrop(DATA_COMPGEN(0x00515c4c, oldmainHeroesIcn2, "heroes.icn"), gpWindowManager->m_screen, 1);
            gpWindowManager
                ->UpdateScreenRegion(0, 0, OLD_MAIN_SCREEN_WIDTH, OLD_MAIN_SCREEN_HEIGHT);
            gpWindowManager->FadeScreen(FADE_IN, OLD_MAIN_FADE_SPEED, gPalette);
            gpWindowManager->m_updateFlags = 1;
            mainScreenLoaded_b = 1;
            gpSoundManager->PlayAmbientMusic(OLD_MAIN_MAIN_MUSIC, 0, -1);
        } else if (gbInCampaign) {
            result_i = gpGame->HandleCampaignWin();
            if ((gpGame->m_campaignScenario == OLD_MAIN_ARCHIBALD_FINAL_SCENARIO
                 && gpGame->m_campaignScenarioCompleted[IDX(gpGame->m_campaignType)]
                                                       [OLD_MAIN_ARCHIBALD_FINAL_SCENARIO])
                || (gpGame->m_campaignScenario == OLD_MAIN_ROLAND_FINAL_SCENARIO
                    && gpGame->m_campaignType == CAMPAIGN_ROLAND
                    && gpGame->m_campaignScenarioCompleted[IDX(CAMPAIGN_ROLAND)]
                                                          [OLD_MAIN_ROLAND_FINAL_SCENARIO])) {
                gbShowHighScore = true;
                ShowCongrats(HIGH_SCORE_CAMPAIGN);
                AddScoreToHighScore(
                    gpGame->m_campaignScore,
                    gpGame->m_campaignScore,
                    0,
                    HIGH_SCORE_CAMPAIGN,
                    const_cast<char*>(
                        gpGame->m_campaignType == CAMPAIGN_ARCHIBALD ? DATA_COMPGEN(0x00515c58, oldmainArchibald, "Archibald") : DATA_COMPGEN(0x00515c64, oldmainRoland, "Roland")
                    )
                );
            }
            if (result_i) {
                for (player_h = 0; player_h < OLD_MAIN_PLAYER_COUNT; player_h++)
                    sprintf(cPlayerNames[player_h], DATA_COMPGEN(0x00500210, oldmainEmptyString2, ""));
                gpGame->InitCampaignMap();
                gbGameOver = false;
                bForceCheckTimeEvent = 1;
                goto initialize_game;
            }
        } else if (xIsPlayingExpansionCampaign) {
            result_i = xCampaign.HandleVictory();
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
            if (result_i) {
                for (player_h = 0; player_h < OLD_MAIN_PLAYER_COUNT; player_h++)
                    sprintf(cPlayerNames[player_h], DATA_COMPGEN(0x00500288, oldmainEmptyString3, ""));
                xCampaign.InitMap();
                gbGameOver = false;
                bForceCheckTimeEvent = 1;
                goto initialize_game;
            }
        } else {
            ShowCongrats(HIGH_SCORE_STANDARD);
            if (!gbShowHighScore) {
                gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_FADE_SPEED, gPalette);
                gpResourceManager->GetBackdrop(DATA_COMPGEN(0x00515c74, oldmainSecondaryHeroesIcn, "heroes.icn"), gpWindowManager->m_screen, 1);
                gpWindowManager
                    ->UpdateScreenRegion(0, 0, OLD_MAIN_SCREEN_WIDTH, OLD_MAIN_SCREEN_HEIGHT);
                gpWindowManager->FadeScreen(FADE_IN, OLD_MAIN_FADE_SPEED, gPalette);
                gpWindowManager->m_updateFlags = 1;
                mainScreenLoaded_b = 1;
                gpSoundManager->PlayAmbientMusic(OLD_MAIN_MAIN_MUSIC, 0, -1);
            } else {
                gpSoundManager->PlayAmbientMusic(OLD_MAIN_HIGH_SCORE_MUSIC, 0, -1);
            }
        }

        gbGameOver = false;
        if (gbShowHighScore) {
            gbShowHighScore = false;
            if (gpExec->AddManager(gpHighScoreManager, -1))
                ShutDown(DATA_COMPGEN(0x00515c80, oldmainSecondaryManagerFailure, "Can't add manager!"));
            gpExec->MainLoop();
            gpExec->RemoveManager(gpHighScoreManager);
            giHighScoreRank = -1;
            gpSoundManager->SwitchAmbientMusic(OLD_MAIN_MAIN_MUSIC);
            gpResourceManager->GetBackdrop(DATA_COMPGEN(0x00515c94, oldmainTertiaryHeroesIcn, "heroes.icn"), gpWindowManager->m_screen, 1);
            gpWindowManager
                ->UpdateScreenRegion(0, 0, OLD_MAIN_SCREEN_WIDTH, OLD_MAIN_SCREEN_HEIGHT);
            gpWindowManager->FadeScreen(FADE_IN, OLD_MAIN_FADE_SPEED, gPalette);
            mainScreenLoaded_b = 1;
        }

    game_finished:
        if (gbRemoteOn)
            quit_g = 1;
    }

    ShutDown(NULL);
    return 0;
}

VA(0x00498558, 0x44)
char toupper(char c) {
    if (c >= 'a' && c <= 'z')
        return c - ' ';
    else
        return c;
}

VA(0x0049859c, 0x791)
i32 InterpretCommandLine(void) {
    i32 len;
    i32 i;
    i32 commandIndex;
    i32 valueIndex;
    i32 secondValueIndex;
    i32 helpRequested;
    gbTCPFirstTime = true;
    giTCPType = -1;
    giTCPHostStatus = -1;
    giTCPNumPlayers = -1;
    strcpy(gcTCPAddress, DATA_COMPGEN(0x00515ce8, interpretCommandLineEmptyString, ""));
    strcpy(gcTCPName, DATA_COMPGEN(0x005008e0, interpretCommandLineEmptyString2, ""));
    gbUseWaveout = false;
    giDebugLevel = 0;
    giShowIntro = 1;
    gbNoSound = false;
    gbCheatMenus = false;
    giScreenScroll = 1;
    giLimitPlayer = 0;
    gbBlackoutPlayer = true;
    helpRequested = 0;
    strcpy(gMapName, DATA_COMPGEN(0x00515cd4, interpretCommandLineChaosMp2, "Chaos.mp2"));
    strcpy(gFullMapName, DATA_COMPGEN(0x00515ce0, interpretCommandLineChaos, "Chaos"));

    len = strlen(gcCommandLine);
    for (i = 0; OD_STEER(len) > i; i++) {
        commandIndex = i + 1;
        valueIndex = commandIndex + 1;
        secondValueIndex = valueIndex + 1;
        if (gcCommandLine[i] == ' ' && commandIndex < len
            && (gcCommandLine[commandIndex] == '?' || gcCommandLine[commandIndex] == 'h'
                || gcCommandLine[commandIndex] == 'H')) {
            helpRequested = 1;
        }
        if (gcCommandLine[i] == '/' && commandIndex < len) {
            switch (toupper(gcCommandLine[commandIndex])) {
                case 'Z':
                    gbDoMemCheck = false;
                    break;
                case 'W':
                    gbUseWaveout = true;
                    if (valueIndex < len)
                        gbUseWaveout = gcCommandLine[valueIndex] - '0';
                    break;
                case 'V':
                    gConfig.slowVideo = 1;
                    WritePrefs();
                    break;
                case 'N':
                    if (secondValueIndex < len && toupper(gcCommandLine[valueIndex]) == 'W'
                        && toupper(gcCommandLine[secondValueIndex]) == 'C') {
                        gbCheatMenus = true;
                    }
                    break;
                case 'M':
                    if (valueIndex < len)
                        gbDontTryMIDI = 1 - (gcCommandLine[valueIndex] - '0');
                    break;
                case 'R':
                    if (valueIndex < len)
                        gbDontTryRedbook = 1 - (gcCommandLine[valueIndex] - '0');
                    break;
                case 'D':
                    if (valueIndex < len)
                        gbDontTryDigital = 1 - (gcCommandLine[valueIndex] - '0');
                    break;
                case 'S':
                    if (valueIndex < len)
                        gbNoSound = 1 - (gcCommandLine[valueIndex] - '0');
                    break;
                case 'I':
                    if (valueIndex < len)
                        giShowIntro = gcCommandLine[valueIndex] - '0';
                    break;
                case 'P':
                    if (valueIndex < len)
                        giDebugLevel = gcCommandLine[valueIndex] - '0';
                    break;
                case 'X':
                    xSmackFromNetwork = 1;
                    break;
                case 'T':
                    if (valueIndex < len) {
                        switch (toupper(gcCommandLine[valueIndex])) {
                            case 'T':
                                if (secondValueIndex < len
                                    && toupper(gcCommandLine[secondValueIndex]) == 'L') {
                                    giTCPType = LINE_TCP_TYPE_L;
                                } else {
                                    giTCPType = LINE_TCP_TYPE_DEFAULT;
                                }
                                break;
                            case 'S':
                                if (secondValueIndex < len
                                    && toupper(gcCommandLine[secondValueIndex]) == 'H') {
                                    giTCPHostStatus = LINE_TCP_HOST;
                                } else {
                                    giTCPHostStatus = LINE_TCP_CLIENT;
                                }
                                break;
                            case 'P': {
                                i32 count = 0;
                                if (secondValueIndex < len)
                                    count = gcCommandLine[secondValueIndex] - '0';
                                if (count >= LINE_TCP_MIN_PLAYERS
                                    && count <= LINE_TCP_MAX_PLAYERS) {
                                    giTCPNumPlayers = count;
                                }
                                break;
                            }
                            case 'A': {
                                if (secondValueIndex < len) {
                                    i32 dst = 0;
                                    i32 src = secondValueIndex;
                                    while (dst < LINE_TCP_TEXT_LENGTH && gcCommandLine[src]
                                           && gcCommandLine[src] != ' ') {
                                        gcTCPAddress[dst] = gcCommandLine[src];
                                        src++;
                                        dst++;
                                    }
                                    gcTCPAddress[dst] = 0;
                                }
                                break;
                            }
                            case 'N': {
                                if (secondValueIndex < len) {
                                    i32 dst = 0;
                                    i32 src = secondValueIndex;
                                    while (dst < LINE_TCP_TEXT_LENGTH && gcCommandLine[src]
                                           && gcCommandLine[src] != ' ') {
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
        sprintf(gText, DATA_COMPGEN(0x0050120c, interpretCommandLineEmptyString3, ""));
        for (i = 0; i < LINE_HELP_LINE_COUNT; i++)
            strcat(gText, gcCommandLineHelp[i]);
        ShutDown(gText);
    }

    sprintf(cAggPathName, DATA_COMPGEN(0x00515d00, interpretCommandLineSS, "%s%s"), DATA_COMPGEN(0x00515cf8, interpretCommandLineDATA, ".\\DATA\\"), DATA_COMPGEN(0x00515cec, interpretCommandLineHeroes2Agg, "heroes2.agg"));
    DEFAULT_AGGREGATE_NAME = cAggPathName;
    sprintf(cExpAggPathName, DATA_COMPGEN(0x00515d20, interpretCommandLineSS2, "%s%s"), DATA_COMPGEN(0x00515d18, interpretCommandLineDATA2, ".\\DATA\\"), DATA_COMPGEN(0x00515d08, interpretCommandLineHeroes2xAgg, "heroes2x.agg"));
    EXPANSION_AGGREGATE_NAME = cExpAggPathName;
    giFrameStep = LINE_FRAME_STEP;

    for (i = 0; i < LINE_HUMAN_PLAYER_SLOTS; i++) {
        if (giNumHumanPlayers > i)
            gbHumanPlayer[i] = 1;
        else
            gbHumanPlayer[i] = 0;
    }
    if (giNumHumanPlayers == LINE_SINGLE_PLAYER)
        gbBlackoutPlayer = false;

    if (giTCPHostStatus != -1) {
        if (giTCPType == -1 || giTCPNumPlayers == -1
            || (giTCPHostStatus == LINE_TCP_CLIENT && strlen(gcTCPAddress) < 1)) {
            ShutDown(DATA_COMPGEN(0x00515d28, interpretCommandLineIncompleteTCPIPCommandLineInformation, "Incomplete TCP/IP command line information"));
        }
        giShowIntro = 0;
    }
    return 1;
}

VA(0x00498d2d, 0x698)
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
                    msg.payload.widget.data.value =
                        menu * MENU_FRAME_STRIDE + MENU_HOVER_FRAME;
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
                        gpResourceManager->GetBackdrop(DATA_COMPGEN(0x00515d54, initMenuHandlerHeroesIcn, "heroes.icn"), gpWindowManager->m_screen, 1);
                        gpInitWin->DrawWindow(0);
                        gpWindowManager->UpdateScreenRegion(
                            0,
                            0,
                            MENU_SCREEN_WIDTH,
                            MENU_SCREEN_HEIGHT
                        );
                        gpSoundManager->PlayAmbientMusic(MENU_MAIN_MUSIC, 0, -1);
                        break;
                    } else {
                        gpWindowManager->m_dialogResult = msg.payload.widget.id;
                        for (idx = MENU_FIRST_WIDGET; idx <= MENU_LAST_WIDGET; idx++) {
                            msg.type = INIT_MENU_MESSAGE;
                            msg.payload.widget.id = idx;
                            msg.payload.widget.command = INIT_MENU_SET_WIDGET_COMMAND;
                            msg.payload.widget.data.value =
                                idx * MENU_FRAME_STRIDE - MENU_WIDGET_FRAME_BASE;
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
                if (IMHotSpots[idx][IDX(INIT_MENU_HOTSPOT_X)] <= msg.payload.mouse.screenX
                    && IMHotSpots[idx][IDX(INIT_MENU_HOTSPOT_Y)] <= msg.payload.mouse.screenY
                    && msg.payload.mouse.screenX
                           < IMHotSpots[idx][IDX(INIT_MENU_HOTSPOT_X)]
                                 + IMHotSpots[idx][IDX(INIT_MENU_HOTSPOT_WIDTH)]
                    && msg.payload.mouse.screenY
                           < IMHotSpots[idx][IDX(INIT_MENU_HOTSPOT_Y)]
                                 + IMHotSpots[idx][IDX(INIT_MENU_HOTSPOT_HEIGHT)]) {
                    hoverIndex = idx;
                }
            }
            if (lastIMHoverID != hoverIndex) {
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
        msg.payload.widget.command = BaseWidgetCommand(msg.payload.widget.id);
        return MESSAGE_DISPATCH_FORWARD;
    }
    CheckShingleUpdate();
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x004993c5, 0x1b)
MessageDispatchResult NullHandler(struct tag_message& msg) {
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x004993e0, 0x1a9)
MessageDispatchResult RecruitHeroHandler(tag_message& msg) {
    // Retail initializes these otherwise-unused /Od frame locals. Their suffixes preserve the
    // observed identifier-bucket order; their original purpose remains unknown.
    // NOLINTBEGIN(readability-magic-numbers)
    i16 unusedLocal0J = 2, unusedLocal1G = 3, unusedLocal2A = 8, unusedLocal3N = 9;
    // NOLINTEND(readability-magic-numbers)
    i32 shouldClose = 0;
    i32 unusedResult;
    if (msg.type == MESSAGE_WIDGET) {
        switch (msg.payload.widget.command) {
            case WIDGET_COMMAND_SELECT:
                switch (msg.payload.widget.id) {
                    case RECRUIT_HERO_VIEW_BUTTON:
                        HeroView(
                            static_cast<u8>(gpTownManager->m_recruitHero->m_id),
                            true,
                            false
                        );
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
        msg.payload.widget.command = BaseWidgetCommand(msg.payload.widget.id);
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x00499589, 0x1a7)
char* GetBuildingInfo(FactionType race, BuildingSlotType building, i32 mode) {
    char buf[BUILDING_INFO_BUFFER_SIZE];
    if (race == FACTION_NECROMANCER && building == BUILDING_SLOT_NECROMANCER_SHRINE) {
        sprintf(buf, xNecromancerShrineDesc);
    } else if (building == BUILDING_SLOT_WELL_EXTRA) {
        sprintf(
            buf,
            DATA_COMPGEN(0x00515d60, getBuildingInfoTheSIncreasesProductionOfS, "The %s increases production of %s by 8 per week."),
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
            DATA_COMPGEN(0x00515d94, getBuildingInfoTheSProducesS, "The %s produces %s."),
            GetBuildingName(race, building),
            gArmyNamesPlural[IDX(gDwellingType[IDX(race)]
                                               [IDX(building)
                                                - IDX(BUILDING_SLOT_DWELLING_FIRST)])]
        );
        return gText;
    }
    if (mode) {
        sprintf(gText, DATA_COMPGEN(0x00515da8, getBuildingInfoSS, "{%s}\n\n%s"), GetBuildingName(race, building), buf);
    } else {
        sprintf(gText, buf);
    }
    return gText;
}

VA(0x00499730, 0xa4)
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

VA(0x004997d4, 0x138)
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

VA(0x0049990c, 0x20)
char* GetMonsterName(H2_ENUM_PARAM(CreatureType, i32) monster) {
    return gArmyNames[IDX(monster)];
}

VA(0x0049992c, 0x140)
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

VA(0x00499a6c, 0x2b5)
i32 CanBuild(town* t, BuildingSlotType building) {
    i32 reqMask;
    i32 haveMask;
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
    reqMask = gHierarchyMask[IDX(t->m_type)][IDX(building) - IDX(BUILDING_SLOT_DWELLING_FIRST)];
    haveMask = t->m_buildings;
    if (haveMask & IDX(KB_DWELLING_UPGRADE_FIRST_FLAG))
        haveMask |= IDX(KB_DWELLING_FIRST_FLAG);
    if (haveMask & IDX(KB_DWELLING_UPGRADE_SECOND_FLAG))
        haveMask |= IDX(KB_DWELLING_SECOND_FLAG);
    if (haveMask & IDX(KB_DWELLING_UPGRADE_THIRD_FLAG))
        haveMask |= IDX(KB_DWELLING_THIRD_FLAG);
    if (haveMask & IDX(KB_DWELLING_UPGRADE_FOURTH_FLAG))
        haveMask |= IDX(KB_DWELLING_FOURTH_FLAG);
    if (haveMask & IDX(KB_DWELLING_UPGRADE_SIXTH_FLAG))
        haveMask |= IDX(KB_DWELLING_UPGRADE_FIFTH_FLAG);
    if (haveMask & IDX(KB_DWELLING_UPGRADE_FIFTH_FLAG))
        haveMask |= IDX(KB_DWELLING_FIFTH_FLAG);
    if ((OD_STEER(reqMask) & haveMask) == reqMask) {
        if (t->m_type == FACTION_NECROMANCER
            && building == BUILDING_SLOT_NECROMANCER_MAGE_PREREQUISITE && t->m_buildState <= 1)
            return 0;
        return 1;
    }
    return 0;
}

VA(0x00499d21, 0x9a)
i32 CanBuy(town* t, BuildingSlotType type) {
    i32 buf[KB_BUILDING_RESOURCE_COUNT];
    playerData* ptr;
    i32 idx;
    GetBuildingCost(t->m_type, type, buf, t->m_buildState);
    ptr = &gpGame->m_players[giCurPlayer];
    for (idx = 0; idx < KB_BUILDING_RESOURCE_COUNT; idx++)
        if (ptr->m_resources[idx] < buf[idx])
            return 0;
    return 1;
}

VA(0x00499dbb, 0xc6)
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

VA(0x00499e81, 0x21e)
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
        msg.payload.widget.command = BaseWidgetCommand(msg.payload.widget.id);
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x0049a09f, 0x472)
MessageDispatchResult EventWindowHandler(struct tag_message& msg) {
    i32 type;
    i32 extra;

    if (!gpSoundManager->MusicPlaying() && gpAdvManager->m_active == true)
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[IDX(gpAdvManager->m_currentTerrain)]
        );
    if (giDialogTimeout != 0 && KBTickCount() > giDialogTimeout) {
        msg.type = MESSAGE_WIDGET;
        gpWindowManager->m_dialogResult = msg.payload.widget.id;
        msg.payload.widget.id = EVENT_WINDOW_CLOSE_COMMAND;
        msg.payload.widget.command = BaseWidgetCommand(msg.payload.widget.id);
        giDialogTimeout = 0;
        return MESSAGE_DISPATCH_FORWARD;
    }
    if (msg.type == MESSAGE_WIDGET) {
        switch (msg.payload.widget.command) {
            case WIDGET_COMMAND_SELECT:
            case WIDGET_COMMAND_ALTERNATE_SELECT:
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
                                DATA_COMPGEN(0x00515db4, eventWindowHandlerExperienceExperienceAllowsYourHeroesTo, "{Experience}\n\nExperience allows your heroes to go up levels, "
                                "increasing their primary and secondary skills."),
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
                            if (extra == IDX(ARTIFACT_SPELL_SCROLL)) {
                                sprintf(gText, gArtifactDesc[extra], gSpellNames[xTheSpell]);
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
                                    gArtifactDesc[extra],
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
                                gSpellDesc[extra],
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
                                cSecSkillDesc[extra / SECONDARY_SKILL_VALUE_LEVEL_COUNT]
                                             [extra % SECONDARY_SKILL_VALUE_LEVEL_COUNT],
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
                                gStatDesc[extra],
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
                                DATA_COMPGEN(0x00515e20, eventWindowHandlerResourcesThereAreSevenResourcesIn, "{Resources}\n\nThere are seven resources in Heroes 2, used to "
                                "build and improves castles, purchase troops and recruit heroes.  "
                                "Gold is the most common, required for virtually everything.  Wood "
                                "and ore are used for most buildings.  Gems, Mercury, Sulfur and "
                                "Crystal are rare magical resources used for the most powerful "
                                "creatures and buildings."),
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
                        msg.payload.widget.command = BaseWidgetCommand(msg.payload.widget.id);
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

VA(0x0049a511, 0x1e)
MessageDispatchResult TrueFalseDialogHandler(struct tag_message& msg) {
    return EventWindowHandler(msg);
}

VA(0x0049a52f, 0x192)
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

VA(0x0049a6c1, 0x19bb)
void CheckEndGame(
    H2_ENUM_PARAM(CheckEndGameForcedResult, i32) forcedResult,
    b32 dragonCityCaptured
) {
    i32 showedDialog;
    i32 defeated;
    i32 allowNormalVictory;
    i32 lastLivingPlayer;
    i32 survivingHumans;
    i32 lastHuman_i;
    i32 netHumanCount;
    i32 player;
    i32 heroIndex;
    i32 winFlag;
    playerData* rec;
    i32 savedRemoteOn;
    i32 numAlive;
    char unusedTextA_c[END_GAME_TEXT_BUFFER_SIZE];
    char unusedTextB_c[END_GAME_TEXT_BUFFER_SIZE];
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
    hero* artifactHeroPtr;
    i32 artifactWinnerPerson;
    hero* lossHero;
    hero* winningHeroEntry;
    i32 winnerPlayer;
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
    savedRemoteOn = gbRemoteOn;
    showedDialog = 0;

    for (player = 0; player < gpGame->m_playerCount; player++) {
        if (!gpGame->m_playerDead[player]) {
            rec = &gpGame->m_players[player];
            if ((rec->m_heroCount == 0 && rec->m_townCount == 0)
                || (xIsPlayingExpansionCampaign && xCampaign.IsSpecialLossCondition(player))) {
                PlayerDead(player);
                if (giThisGamePos == player) {
                    showedDialog = 1;
                    sprintf(gText, DATA_COMPGEN(0x00515f7c, checkEndGameYouHaveBeenEliminatedFromThe, "You have been eliminated from the game!!!"));
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                } else {
                    sprintf(gText, DATA_COMPGEN(0x00515fa8, checkEndGameSHasBeenVanquished, "%s has been vanquished!"), cPlayerNames[player]);
                    NormalDialog(
                        gText,
                        1,
                        -1,
                        -1,
                        END_GAME_PLAYER_DIALOG_ICON,
                        gpGame->GetPlayerColor(static_cast<i8>(player)),
                        -1,
                        -1,
                        -1,
                        END_GAME_REMOTE_DIALOG_TIME
                    );
                }
            } else if (rec->m_townCount == 0) {
                if (rec->m_daysLeft == -1) {
                    if (gbThisNetHumanPlayer[player] && giCurPlayer == player) {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x00515fc0, checkEndGameSYouHaveLostYourLast, "%s, you have lost your last town.  If you do not conquer another "
                            "town in the next week, you will be eliminated."),
                            cPlayerNames[player]
                        );
                        NormalDialog(
                            gText,
                            1,
                            -1,
                            -1,
                            END_GAME_PLAYER_DIALOG_ICON,
                            gpGame->GetPlayerColor(static_cast<i8>(player)),
                            -1,
                            0,
                            -1,
                            0
                        );
                    }
                    rec->m_daysLeft = END_GAME_GRACE_DAYS;
                } else if (rec->m_daysLeft == 0) {
                    PlayerDead(player);
                    if (gbThisNetHumanPlayer[player] && giCurPlayer == player) {
                        if (!showedDialog) {
                            showedDialog = 1;
                            sprintf(
                                gText,
                                DATA_COMPGEN(0x00516030, checkEndGameSYourHeroesAbandonYouAnd, "%s, your heroes abandon you, and you are banished from this "
                                "land."),
                                cPlayerNames[player]
                            );
                        }
                    } else {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x00516074, checkEndGameSSHeroesHaveAbandonedHim, "%s's heroes have abandoned him, and he is banished from this "
                            "land."),
                            cPlayerNames[player]
                        );
                    }
                    NormalDialog(
                        gText,
                        1,
                        -1,
                        -1,
                        END_GAME_PLAYER_DIALOG_ICON,
                        gpGame->GetPlayerColor(static_cast<i8>(player)),
                        -1,
                        0,
                        -1,
                        0
                    );
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
    if ((gpGame->m_mapHeader.victoryCondition != MAP_VICTORY_DEFEAT_ALL
         && !gpGame->m_mapHeader.allowNormalVictory)
        || (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ARCHIBALD
            && gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET
                   == END_GAME_SIDE_SCENARIO)) {
        allowNormalVictory = 0;
    }

    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_SIDE
        && gpGame->m_mapHeader.victoryConditionValue != CAMPAIGN_SWITCH_VICTORY_VALUE
        && (!gbInCampaign || gpGame->m_campaignType != CAMPAIGN_ARCHIBALD
            || gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET
                   != END_GAME_SIDE_SCENARIO)) {
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
                defeated = 1;
            }
            if (!showedDialog && winFlag) {
                showedDialog = 1;
                sprintf(gText, DATA_COMPGEN(0x005160b8, checkEndGameTheEnemyIsBeatenYourSide, "The enemy is beaten.  Your side has triumphed!"));
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
                defeated = 1;
            }
            if (!showedDialog) {
                showedDialog = 1;
                if (winFlag) {
                    sprintf(
                        gText,
                        DATA_COMPGEN(0x005160e8, checkEndGameYouCapturedSYouAreVictorious, "You captured %s!  You are victorious."),
                        victoryTownData->m_name
                    );
                } else {
                    sprintf(
                        gText,
                        DATA_COMPGEN(0x00516110, checkEndGameTheEnemyHasCapturedSThey, "The enemy has captured %s!  They are triumphant."),
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
        if (lossTown->m_owner == TOWN_OWNER_NONE
            || !gbHumanPlayer[lossTown->m_owner]) {
            defeated = 1;
            if (!showedDialog) {
                showedDialog = 1;
                sprintf(gText, DATA_COMPGEN(0x00516144, checkEndGameSHasFallenAllIsLost, "%s has fallen!  All is lost."), lossTown->m_name);
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_ACCUMULATE_GOLD) {
        bestGold = 0;
        winnerPlayer = END_GAME_NO_PLAYER;
        for (player = 0; player < gpGame->m_playerCount; player++) {
            if ((gbHumanPlayer[player] || gpGame->m_mapHeader.computerAlsoWins)
                && gpGame->m_players[player].m_resources[IDX(RES_GOLD)]
                       >= gpGame->m_mapHeader.victoryConditionValue * END_GAME_GOLD_SCALE
                && gpGame->m_players[player].m_resources[IDX(RES_GOLD)]
                       >= bestGold) {
                bestGold = gpGame->m_players[player].m_resources[IDX(RES_GOLD)];
                winnerPlayer = player;
            }
            if (winnerPlayer != END_GAME_NO_PLAYER) {
                if (gbThisNetHumanPlayer[IDX(winnerPlayer)]) {
                    winFlag = 1;
                } else {
                    defeated = 1;
                }
                if (!showedDialog) {
                    showedDialog = 1;
                    if (winFlag) {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x00516164, checkEndGameYouHaveBuiltUpOverD, "You have built up over %d gold in your treasury.  All enemies bow "
                            "before your wealth and power."),
                            bestGold
                        );
                    } else {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x005161c4, checkEndGameTheEnemyHasBuiltUpOver, "The enemy has built up over %d gold in his treasury.  You must "
                            "bow done in defeat before his wealth and power."),
                            bestGold
                        );
                    }
                    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        }
    }

    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_HERO) {
        winningHeroEntry = GetHeroSlot(gpGame->m_mapHeader.victoryConditionValue);
        if (winningHeroEntry->m_owner < 0
            || winningHeroEntry->m_owner >= GAME_PLAYER_COUNT
            || gbHumanPlayer[winningHeroEntry->m_owner]) {
            winFlag = 1;
            if (!showedDialog) {
                showedDialog = 1;
                sprintf(
                    gText,
                    DATA_COMPGEN(0x00516234, checkEndGameYouHaveCapturedTheEnemyHero, "You have captured the enemy hero %s!  Your quest is complete."),
                    winningHeroEntry->m_name
                );
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (gpGame->m_mapHeader.lossCondition == MAP_LOSS_HERO) {
        lossHero = GetHeroSlot(gpGame->m_mapHeader.lossConditionValue);
        if (lossHero->m_owner < 0 || lossHero->m_owner >= GAME_PLAYER_COUNT
            || !gbHumanPlayer[lossHero->m_owner]) {
            defeated = 1;
            if (!showedDialog) {
                showedDialog = 1;
                sprintf(gText, DATA_COMPGEN(0x00516274, checkEndGameYouHaveLostTheHeroS, "You have lost the hero %s.  Your quest is over."), lossHero->m_name);
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (gpGame->m_mapHeader.lossCondition == MAP_LOSS_TIME) {
        if (gpGame->m_mapHeader.lossConditionValue
            < (gpGame->m_week - 1) * CALENDAR_DAYS_PER_WEEK
                  + (gpGame->m_month - 1) * CALENDAR_DAYS_PER_MONTH + gpGame->m_day) {
            defeated = 1;
            if (!showedDialog) {
                showedDialog = 1;
                sprintf(gText, DATA_COMPGEN(0x005162a4, checkEndGameYouHaveFailedToCompleteYour, "You have failed to complete your quest in time.  All is lost."));
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_FIND_ARTIFACT) {
        artifactWinnerPerson = END_GAME_NO_PLAYER;
        for (player = 0; player < gpGame->m_playerCount; player++) {
            if (!gpGame->m_playerDead[player]) {
                for (heroIndex = 0; heroIndex < gpGame->m_players[player].m_heroCount;
                     heroIndex++) {
                    artifactHeroPtr = gpGame->GetPlayerHero(player, heroIndex);
                    if (gpGame->m_mapHeader.victoryConditionValue
                        > END_GAME_ULTIMATE_ARTIFACT) {
                        if (artifactHeroPtr->HasArtifact(
                                ArtifactType(gpGame->m_mapHeader.victoryConditionValue - 1)
                            )) {
                            artifactWinnerPerson = player;
                        }
                    } else {
                        if (artifactHeroPtr->HasArtifact(ARTIFACT_ULTIMATE_BOOK)
                            || artifactHeroPtr->HasArtifact(ARTIFACT_ULTIMATE_SWORD)
                            || artifactHeroPtr->HasArtifact(ARTIFACT_ULTIMATE_CLOAK)
                            || artifactHeroPtr->HasArtifact(ARTIFACT_ULTIMATE_WAND)
                            || artifactHeroPtr->HasArtifact(ARTIFACT_ULTIMATE_SHIELD)
                            || artifactHeroPtr->HasArtifact(ARTIFACT_ULTIMATE_STAFF)
                            || artifactHeroPtr->HasArtifact(ARTIFACT_ULTIMATE_CROWN)
                            || artifactHeroPtr->HasArtifact(ARTIFACT_GOLDEN_GOOSE)) {
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
                defeated = 1;
            }
            if (!showedDialog) {
                showedDialog = 1;
                if (gpGame->m_mapHeader.victoryConditionValue
                    == END_GAME_ULTIMATE_ARTIFACT) {
                    sprintf(artifactName, DATA_COMPGEN(0x005162e4, checkEndGameUltimateArtifact, "Ultimate Artifact"));
                } else {
                    sprintf(
                        artifactName,
                        gArtifactNames[gpGame->m_mapHeader.victoryConditionValue - 1]
                    );
                }
                if (winFlag) {
                    sprintf(gText, DATA_COMPGEN(0x005162f8, checkEndGameYouHaveFoundTheSYour, "You have found the %s.  Your quest is complete."), artifactName);
                } else {
                    sprintf(
                        gText,
                        DATA_COMPGEN(0x00516328, checkEndGameTheEnemyHasFoundTheS, "The enemy has found the %s.  Your quest is a failure."),
                        artifactName
                    );
                }
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ROLAND
        && gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET
               == END_GAME_DWARF_SCENARIO) {
        hasDwarfTown = 0;
        for (player = 0; player < gpGame->m_players[0].m_townCount; player++) {
            if (gpGame->GetTown(gpGame->m_players[0].m_townIds[player])->m_type
                == FACTION_SORCERESS) {
                hasDwarfTown = 1;
            }
        }
        if (!hasDwarfTown) {
            defeated = 1;
            if (!showedDialog) {
                showedDialog = 1;
                sprintf(
                    gText,
                    DATA_COMPGEN(0x00516360, checkEndGameAllTheDwarfTownsHaveFallen, "All the dwarf towns have fallen.  This is a disastrous defeat!  You have "
                    "lost.")
                );
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ARCHIBALD
        && gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET == END_GAME_SIDE_SCENARIO
        && dragonCityCaptured) {
        winFlag = 1;
        if (!showedDialog) {
            showedDialog = 1;
            sprintf(gText, DATA_COMPGEN(0x005163b0, checkEndGameDragonCityHasFallenYouAre, "Dragon city has fallen!  You are now the Master of the Dragons."));
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
            defeated = 1;
            if (!showedDialog) {
                showedDialog = 1;
                sprintf(gText, DATA_COMPGEN(0x005163f0, checkEndGameRolandHasBeenCapturedAllIs, "Roland has been captured!  All is lost."));
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
            if (!showedDialog && winFlag) {
                showedDialog = 1;
                sprintf(gText, DATA_COMPGEN(0x00516418, checkEndGameTheEnemyIsBeatenYourSide2, "The enemy is beaten.  Your side has triumphed!"));
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
    }

    if (defeated) {
        gbGameOver = true;
        giEndSequence = 0;
    }
    if (winFlag) {
        gbGameOver = true;
        giEndSequence = 1;
    }

    if (numAlive == 1 || survivingHumans == 0
        || (survivingHumans == 1 && !gbThisNetHumanPlayer[lastHuman_i])) {
        if (survivingHumans == 1 && gbThisNetHumanPlayer[lastHuman_i]) {
            if (allowNormalVictory) {
                gbGameOver = true;
                giEndSequence = 1;
            }
        } else {
            gbGameOver = true;
            giEndSequence = 0;
        }
    }

    if (savedRemoteOn && netHumanCount == 0) {
        gbGameOver = true;
        giEndSequence = 0;
    }
    if (forcedResult == END_GAME_FORCE_VICTORY) {
        winFlag = 1;
        gbGameOver = true;
        giEndSequence = 1;
    }
    if (forcedResult == END_GAME_FORCE_DEFEAT) {
        defeated = 1;
        gbGameOver = true;
        giEndSequence = 0;
    }

    if (giEndSequence == 1 && gbGameOver) {
        winFlag = 1;
    }
    if (giEndSequence == 0 && gbGameOver) {
        defeated = 1;
    }

    if (gbInCampaign && winFlag) {
        currentDayIndex = (gpGame->m_week - 1) * CALENDAR_DAYS_PER_WEEK
                          + (gpGame->m_month - 1) * CALENDAR_DAYS_PER_MONTH
                          + gpGame->m_day;
        gpGame->m_campaignScenarioWon = 1;
        gpGame->m_campaignScenarioCompleted[IDX(gpGame->m_campaignType)]
                                                   [gpGame->m_campaignScenario] = 1;
        gpGame->m_campaignScenarioDays[IDX(gpGame->m_campaignType)]
                                      [gpGame->m_campaignScenario] =
            currentDayIndex;
        gpGame->m_campaignScore =
            gpGame->m_campaignScenarioDays[IDX(gpGame->m_campaignType)]
                                          [gpGame->m_campaignScenario]
            + gpGame->m_campaignScenarioBonus[IDX(gpGame->m_campaignType)]
                                             [gpGame->m_campaignScenario];

        carryoverHeroId = END_GAME_NO_PLAYER;
        if (gpGame->m_campaignType == CAMPAIGN_ROLAND
            && gpGame->m_campaignScenario + END_GAME_SCENARIO_OFFSET
                   == END_GAME_SIDE_SCENARIO) {
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
                    || IDX(gpGame->m_heroRecs
                               [gpGame->m_players[0].m_heroIds[campaignHeroIndex]]
                                   .m_portrait)
                           == carryoverHeroId) {
                    break;
                }
            }
            if (gpGame->m_players[0].m_heroCount == campaignHeroIndex) {
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
                DATA_COMPGEN(0x00516450, checkEndGameSC02d, "%s%c_%02d"),
                DATA_COMPGEN(0x00516448, checkEndGameWIN, "WIN_"),
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

VA(0x0049c07c, 0x95)
void QuickViewWait(void) {
    tag_message ev;
    i32 done;
    gpMouseManager->ReallyHidePointer();
    done = 0;
    while (!done) {
        PollSound();
        Process1WindowsMessage();
        ev = gpInputManager->GetEvent();
        if (ev.type == MESSAGE_RIGHT_BUTTON_UP || ev.type == MESSAGE_LEFT_BUTTON_DOWN
            || ev.type == MESSAGE_LEFT_BUTTON_UP)
            done = 1;
        else
            done = 0;
    }
    gpMouseManager->ReallyShowPointer();
}

VA(0x0049c111, 0x201)
void InitVars(void) {
    i32 i;
    i32 j;
    NULL_SAMPLE2.pSample = NULL;
    NULL_SAMPLE2.pMem = reinterpret_cast<struct _SAMPLE*>(NULL_SAMPLE2.pSample);
    gGameCommand = -1;
    gPalette = NULL;
    gbCombatSurrender = false;
    gpGame->m_viewArmyResult = 0;
    strcpy(gpGame->m_mapFilename, DATA_COMPGEN(0x0051645c, initVarsBrokenaMp2, "brokena.mp2"));
    gpGame->m_newGameInitialized = 0;
    gbInNewGameSetup = false;
    strcpy(cNetBoxLine[0], DATA_COMPGEN(0x00516468, initVarsEmptyString, ""));
    strcpy(cNetBoxLine[1], DATA_COMPGEN(0x0051646c, initVarsEmptyString2, ""));
    strcpy(cNetBoxLine[2], DATA_COMPGEN(0x00516470, initVarsEmptyString3, ""));
    strcpy(cNetBoxLine[3], DATA_COMPGEN(0x00516474, initVarsEmptyString4, ""));
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
        hmnuDflt = LoadMenuA(hInstApp, DATA_COMPGEN(0x00516478, initVarsMnuDflt, "mnuDflt"));
        hmnuCmbt = LoadMenuA(hInstApp, DATA_COMPGEN(0x00516480, initVarsMnuCmbt, "mnuCmbt"));
        hmnuAdv = LoadMenuA(hInstApp, DATA_COMPGEN(0x00516488, initVarsMnuAdvD, "mnuAdvD"));
        hmnuTown = LoadMenuA(hInstApp, DATA_COMPGEN(0x00516490, initVarsMnuTownD, "mnuTownD"));
    } else {
        hmnuDflt = LoadMenuA(hInstApp, DATA_COMPGEN(0x0051649c, initVarsMnuDflt2, "mnuDflt"));
        hmnuCmbt = LoadMenuA(hInstApp, DATA_COMPGEN(0x005164a4, initVarsMnuCmbt2, "mnuCmbt"));
        hmnuAdv = LoadMenuA(hInstApp, DATA_COMPGEN(0x005164ac, initVarsMnuAdv, "mnuAdv"));
        hmnuTown = LoadMenuA(hInstApp, DATA_COMPGEN(0x005164b4, initVarsMnuTown, "mnuTown"));
    }
}

VA(0x0049c312, 0x61b)
void game::ShowMoraleInfo(hero* h, i32 dialogType) {
    i32 mixedUndead4;
    i32 alignment_e;
    ArmyGroupAlignmentResult homogeneous3;
    i32 modifierStart;
    char description[MORALE_LUCK_DESCRIPTION_SIZE];
    i32 slot8;

    mixedUndead4 = 0;
    if (h->m_army.GetMorale(h, h->GetOccupiedTown(), NULL) > 0)
        sprintf(description, cMoraleInfo[IDX(MORALE_INFO_GOOD)]);
    else {
        if (h->m_army.GetMorale(h, h->GetOccupiedTown(), NULL) == 0)
            sprintf(description, cMoraleInfo[IDX(MORALE_INFO_NEUTRAL)]);
        else
            sprintf(description, cMoraleInfo[IDX(MORALE_INFO_BAD)]);
    }

    sprintf(gText, cMoraleInfo[IDX(MORALE_INFO_HEADER)], description);
    modifierStart = strlen(gText);
    if (h->m_army.HasAllUndead()) {
        strcat(gText, cMoraleInfo[IDX(INFO_ALL_UNDEAD)]);
    } else {
        if (h->m_army.HasSomeUndead() || h->HasArtifact(ARTIFACT_ARM_OF_MARTYR)) {
            strcat(gText, cMoraleInfo[IDX(INFO_SOME_UNDEAD)]);
            mixedUndead4 = 1;
        }

        homogeneous3 = h->m_army.IsHomogeneous(-1);
        if (mixedUndead4 && homogeneous3 > ARMY_GROUP_ALIGNMENT_NO_MODIFIER) {
            homogeneous3 = ARMY_GROUP_ALIGNMENT_NO_MODIFIER;
        }
        if (homogeneous3 > ARMY_GROUP_ALIGNMENT_NO_MODIFIER) {
            alignment_e = 0;
            for (slot8 = 0; slot8 < ARMY_GROUP_SLOT_COUNT; slot8++) {
                if (h->m_army.m_creatureTypes[slot8] != CREATURE_NONE) {
                    alignment_e =
                        IDX(gMonsterDatabase[IDX(h->m_army.m_creatureTypes[slot8])].race);
                }
            }
            sprintf(
                description,
                cMoraleInfo[IDX(INFO_SAME_ALIGNMENT)],
                gAlignmentNames[alignment_e]
            );
            strcat(gText, description);
        }
        if (homogeneous3 == ARMY_GROUP_ALIGNMENT_THREE) {
            sprintf(description, cMoraleInfo[IDX(INFO_THREE_ALIGNMENTS)]);
            strcat(gText, description);
        }
        if (homogeneous3 == ARMY_GROUP_ALIGNMENT_FOUR) {
            sprintf(description, cMoraleInfo[IDX(INFO_FOUR_ALIGNMENTS)]);
            strcat(gText, description);
        }
        if (homogeneous3 == ARMY_GROUP_ALIGNMENT_FIVE_OR_MORE) {
            sprintf(description, cMoraleInfo[IDX(INFO_FIVE_ALIGNMENTS)]);
            strcat(gText, description);
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
        if (static_cast<i32>(strlen(gText)) == modifierStart) {
            strcat(gText, cMoraleInfo[IDX(MORALE_INFO_NONE)]);
        }
    }

    NormalDialog(gText, dialogType, -1, -1, -1, 0, -1, 0, -1, 0);
}

VA(0x0049c92d, 0x371)
void game::ShowLuckInfo(hero* h, i32 dialogType) {
    char description[MORALE_LUCK_DESCRIPTION_SIZE];
    i32 luckValue;
    i32 modifierStart;

    if (gpGame->GetLuck(h, NULL, h->GetOccupiedTown()) > 0)
        sprintf(description, cLuckInfo[IDX(LUCK_INFO_GOOD)]);
    else {
        if (gpGame->GetLuck(h, NULL, h->GetOccupiedTown()) == 0)
            sprintf(description, cLuckInfo[IDX(LUCK_INFO_NEUTRAL)]);
        else
            sprintf(description, cLuckInfo[IDX(LUCK_INFO_BAD)]);
    }

    sprintf(gText, cLuckInfo[IDX(LUCK_INFO_HEADER)], description);
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
    if (static_cast<i32>(strlen(gText)) == modifierStart)
        strcat(gText, cLuckInfo[IDX(LUCK_INFO_NONE)]);

    NormalDialog(gText, dialogType, -1, -1, -1, 0, -1, 0, -1, 0);
}

VA(0x0049cc9e, 0xd7)
void ClearMapExtra(void) {
    DATA(0x005164bc) static i16 clearMapExtraSourceLineBase = 0x0cbe;
    i32 i;
    for (i = 0; OD_STEER(i) < iMaxMapExtra; i++) {
        if (ppMapExtra[i])
            H2_FREE_AT(
                ppMapExtra[i], DATA_COMPGEN(0x005164c0, clearMapExtraSourceFile, RETAIL_FILE),
                clearMapExtraSourceLineBase + 6
            );
    }
    if (ppMapExtra)
        H2_FREE_AT(ppMapExtra, DATA_COMPGEN(0x005164e8, clearMapExtraSourceFile2, RETAIL_FILE), clearMapExtraSourceLineBase + 9);
    ppMapExtra = NULL;
    if (pwSizeOfMapExtra)
        H2_FREE_AT(
            pwSizeOfMapExtra, DATA_COMPGEN(0x00516510, clearMapExtraSourceFile3, RETAIL_FILE),
            clearMapExtraSourceLineBase + 13
        );
    pwSizeOfMapExtra = NULL;
    iMaxMapExtra = 0;
}

VA(0x0049cd75, 0x9f)
i32 GetMonType(i32 score, HighScoreType campaign) {
    i32 idx;
    for (idx = IDX(CREATURE_COUNT) - 1; idx >= 0; idx--) {
        if (campaign == HIGH_SCORE_CAMPAIGN || campaign == HIGH_SCORE_EXPANSION_CAMPAIGN) {
            if (giScoreCampaignMon[idx][IDX(MONSTER_SCORE_THRESHOLD)] >= score)
                return giScoreCampaignMon[idx][IDX(MONSTER_SCORE_TYPE)];
        } else {
            if (giScoreMon[idx][IDX(MONSTER_SCORE_THRESHOLD)] <= score)
                return giScoreMon[idx][IDX(MONSTER_SCORE_TYPE)];
        }
    }
    return giScoreMon[0][IDX(MONSTER_SCORE_TYPE)];
}

VA(0x0049ce14, 0x4ac)
i32 AddScoreToHighScore(
    i32 score,
    i32 days,
    i32 scenario,
    HighScoreType highScoreType,
    char* scenarioName
) {
    i32 destination;
    HighScoreEntry entries_a[HIGH_SCORE_ENTRY_COUNT];
    i32 file_a;
    i32 entry;
    char filename_a[HIGH_SCORE_FILENAME_LENGTH];
    char playerName_c[HIGH_SCORE_INPUT_NAME_SIZE];
    i32 missingFile;

    missingFile = 0;
    if (highScoreType == HIGH_SCORE_STANDARD)
        sprintf(filename_a, DATA_COMPGEN(0x00516540, addScoreToHighScoreSSTANDARDHS, "%sSTANDARD.HS"), DATA_COMPGEN(0x00516538, addScoreToHighScoreDATA, ".\\DATA\\"));
    else
        sprintf(filename_a, DATA_COMPGEN(0x00516558, addScoreToHighScoreSCAMPAIGNHS, "%sCAMPAIGN.HS"), DATA_COMPGEN(0x00516550, addScoreToHighScoreDATA2, ".\\DATA\\"));

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

    gbShowHighScore = true;
    giHighScoreType = highScoreType;
    giHighScoreRank = HIGH_SCORE_EMPTY;
    giScore = score;
    for (entry = 0; entry < HIGH_SCORE_ENTRY_COUNT; entry++) {
        if ((entries_a[entry].score <= score && highScoreType == HIGH_SCORE_STANDARD)
            || (score <= entries_a[entry].score && highScoreType == HIGH_SCORE_CAMPAIGN)
            || (score <= entries_a[entry].score && highScoreType == HIGH_SCORE_EXPANSION_CAMPAIGN)
            || entries_a[entry].score == HIGH_SCORE_EMPTY) {
            giHighScoreRank = entry;
            break;
        }
    }

    if (entry < HIGH_SCORE_ENTRY_COUNT) {
        for (destination = HIGH_SCORE_LAST_SHIFT_SOURCE; destination >= OD_STEER(entry);
             destination--)
            entries_a[destination + 1] = entries_a[destination];

        GetDataEntry(
            DATA_COMPGEN(0x00516568, addScoreToHighScorePleaseEnterYourNameForThe, "Please enter your name for the high score list."),
            playerName_c,
            HIGH_SCORE_NAME_LENGTH,
            NULL,
            0,
            1
        );
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
        gbShowHighScore = false;
    }
    return 0;
}

VA(0x0049d2c0, 0x66)
void BVResMsg(char* s, H2_ENUM_PARAM(ResourceType, i32) res, i32 qty) {
    giBottomViewOverride = BOTTOM_VIEW_RESOURCE;
    giBottomViewOverrideEndTime = KBTickCount() + BOTTOM_VIEW_RESOURCE_MESSAGE_DURATION;
    giBottomViewResource = res;
    giBottomViewResourceQty = qty;
    strcpy(gcBottomViewText, s);
    gpAdvManager->UpdBottomView(1, 1, 1);
}

VA(0x0049d326, 0x2d)
void GOut(char* str) {
    if (gpAdvManager->m_active)
        AiPrint(str);
}

VA(0x0049d353, 0x54)
i32 NetPosToGamePos(i32 netPos) {
    for (i32 i = 0; i < GAME_PLAYER_COUNT; i++)
        if (gbGamePosToNetPos[i] == netPos)
            return i;
    return -1;
}

VA(0x0049d3a7, 0xFF)
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

// @semantic
// All 131 relocations and the control-flow topology agree. The first residual is
// the printable-key range test: retail compares the stored byte directly, while
// MSVC zero-extends the equivalent u8/u32 expression. Literal, narrowed,
// inline-helper, and bit-field spellings did not reproduce that lowering.
VA(0x0049d4a6, 0xb85)
void PopNetBox(char* text, i32 netPlayer) {
    i32 textY_d;
    i32l messageTime_b;
    heroWindow* netWindow_j;
    i32 result_a;
    i32 textWidth_b;
    i32 textX_d;
    i32 savedShowIt_a;
    i32 updateInput_a;
    i32 inputLength_a;
    char inputText_c[BOX_TEXT_LENGTH];
    i32 exitForIncomingData_i;
    i32 sendText_e;
    tag_message event_a;
    tag_message updateMessage_f;
    i32 firstLineId_a;
    i32 delay_h;
    i32 lineTextLimit_p;
    i32 done_i;
    i32 redrawLines_f;
    i32 redrawSavedShowIt_a;
    KbRemotePacket* remoteData_g;
    i32 redrawAdventure_i;
    i32 cursorState;

    if (!gbRemoteOn)
        return;

    lineTextLimit_p = BOX_LINE_TEXT_LIMIT;
    firstLineId_a = BOX_FIRST_LINE_ID;
    textX_d = BOX_TEXT_X;
    textY_d = BOX_TEXT_Y;
    messageTime_b = 0;
    if (text != NULL) {
        if (netPlayer >= 0) {
            sprintf(gText, DATA_COMPGEN(0x00516598, popNetBoxSS, "%s:  %s"), gsNetPlayerInfo[netPlayer].name, text);
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
    savedShowIt_a = bShowIt;
    bShowIt = 1;
    gbMoveShown = false;
    netWindow_j = new heroWindow(0, BOX_WINDOW_Y, DATA_COMPGEN(0x005165a0, popNetBoxNetboxBin, "netbox.bin"));
    if (netWindow_j == NULL)
        MemError();

    updateMessage_f.type = NET_BOX_UPDATE_MESSAGE;
    updateMessage_f.payload.widget.command = NET_BOX_TEXT_COMMAND;
    updateMessage_f.payload.widget.id = BOX_FIRST_LINE_ID;
    updateMessage_f.payload.widget.data.text = cNetBoxLine[0];
    netWindow_j->BroadcastMessage(updateMessage_f);
    updateMessage_f.payload.widget.id = BOX_FIRST_LINE_ID + 1;
    updateMessage_f.payload.widget.data.text = cNetBoxLine[1];
    netWindow_j->BroadcastMessage(updateMessage_f);
    updateMessage_f.payload.widget.id = BOX_FIRST_LINE_ID + 2;
    updateMessage_f.payload.widget.data.text = cNetBoxLine[2];
    netWindow_j->BroadcastMessage(updateMessage_f);
    updateMessage_f.payload.widget.id = BOX_FIRST_LINE_ID + 3;
    updateMessage_f.payload.widget.data.text = cNetBoxLine[3];
    netWindow_j->BroadcastMessage(updateMessage_f);
    updateMessage_f.payload.widget.command = NET_BOX_COLOR_COMMAND;
    updateMessage_f.payload.widget.id = BOX_FIRST_COLOR_ID;
    updateMessage_f.payload.widget.data.value = cNetBoxColor[0] + BOX_COLOR_FRAME_OFFSET;
    netWindow_j->BroadcastMessage(updateMessage_f);
    updateMessage_f.payload.widget.id = BOX_FIRST_COLOR_ID + 1;
    updateMessage_f.payload.widget.data.value = cNetBoxColor[1] + BOX_COLOR_FRAME_OFFSET;
    netWindow_j->BroadcastMessage(updateMessage_f);
    updateMessage_f.payload.widget.id = BOX_FIRST_COLOR_ID + 2;
    updateMessage_f.payload.widget.data.value = cNetBoxColor[2] + BOX_COLOR_FRAME_OFFSET;
    netWindow_j->BroadcastMessage(updateMessage_f);
    updateMessage_f.payload.widget.id = BOX_FIRST_COLOR_ID + 3;
    updateMessage_f.payload.widget.data.value = cNetBoxColor[3] + BOX_COLOR_FRAME_OFFSET;
    netWindow_j->BroadcastMessage(updateMessage_f);
    updateMessage_f.payload.widget.id = BOX_THIS_PLAYER_COLOR_ID;
    updateMessage_f.payload.widget.data.value =
        gpGame->m_players[NetPosToGamePos(giThisNetPos)].m_color + BOX_COLOR_FRAME_OFFSET;
    netWindow_j->BroadcastMessage(updateMessage_f);

    gpWindowManager->AddWindow(netWindow_j, -1, 1);
    exitForIncomingData_i = 0;
    done_i = 0;
    updateInput_a = 1;
    cursorState = 0;
    sendText_e = 0;
    redrawLines_f = 1;
    redrawAdventure_i = 0;
    strcpy(inputText_c, DATA_COMPGEN(0x005165ac, popNetBoxEmptyString, ""));
    gpInputManager->SetKeyCodeType(INPUT_KEY_CODE_ASCII);

    while (!done_i) {
        PollSound();
        remoteData_g = reinterpret_cast<KbRemotePacket*>(GetRemoteData(0));
        if (remoteData_g != NULL) {
            if (remoteData_g->type == REMOTE_MESSAGE_UNRELIABLE) {
                remoteData_g = reinterpret_cast<KbRemotePacket*>(GetRemoteData(1));
                switch (remoteData_g->command) {
                    case BOX_REMOTE_MAP_CHANGE:
                        gbLeaveNetBoxAlone = true;
                        if (gpAdvManager->m_active == 1) {
                            bShowIt = savedShowIt_a;
                            gpAdvManager->ProcessIncomingGroupMapChange(remoteData_g->payload.data);
                            bShowIt = 1;
                            redrawAdventure_i = 1;
                        }
                        gbLeaveNetBoxAlone = false;
                        updateInput_a = 1;
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
                            DATA_COMPGEN(0x005165b0, popNetBoxSS2, "%s:  %s"),
                            gsNetPlayerInfo[remoteData_g->sender].name,
                            remoteData_g->payload.data
                        );
                        AddNetBoxLine(
                            gText,
                            gpGame->m_players[NetPosToGamePos(remoteData_g->sender)].m_color
                        );
                        redrawLines_f = 1;
                        if (messageTime_b != 0)
                            messageTime_b = KBTickCount();
                        break;
                    default:
                        AddNetBoxLine(DATA_COMPGEN(0x005165b8, popNetBoxIncomingDataMustExit, "[ Incoming data, must exit... ]"), BOX_DEFAULT_COLOR);
                        redrawLines_f = 1;
                        exitForIncomingData_i = 1;
                        break;
                }
            }
        }

        Process1WindowsMessage();
        event_a = gpInputManager->GetEvent();
        switch (event_a.type) {
            case MESSAGE_KEY_DOWN:
                messageTime_b = 0;
                switch (event_a.payload.keyboard.keyCode) {
                    case BOX_KEY_ESCAPE:
                    case BOX_KEY_F1:
                        done_i = 1;
                        break;
                    case BOX_KEY_BACKSPACE:
                        if (inputLength_a > 0)
                            inputLength_a--;
                        updateInput_a = 1;
                        cursorState = 1;
                        break;
                    case BOX_KEY_ENTER:
                        sendText_e = 1;
                        break;
                    default:
                        if (event_a.payload.keyboard.keyByte
                                < static_cast<u32>(BOX_FIRST_PRINTABLE)
                            || event_a.payload.keyboard.keyByte
                                > static_cast<u32>(BOX_LAST_PRINTABLE))
                            break;
                        if (inputLength_a < BOX_MAX_INPUT
                            && event_a.payload.keyboard.keyCode != 0) {
                            inputText_c[inputLength_a] = 0;
                            textWidth_b = smallFont->LineWidth(inputText_c);
                            if (textWidth_b + BOX_CURSOR_WIDTH_PADDING
                                < BOX_CURSOR_WIDTH_LIMIT) {
                                inputText_c[inputLength_a] =
                                    static_cast<char>(event_a.payload.keyboard.keyCode);
                                inputLength_a++;
                                updateInput_a = 1;
                                cursorState = 0;
                            }
                        }
                }
        }

        if (!updateInput_a && glTimers[GLOBAL_NET_BOX_CURSOR_TIMER_SLOT] < KBTickCount()) {
            cursorState = 1 - cursorState;
            updateInput_a = 1;
        }
        if (sendText_e) {
            sendText_e = 0;
            inputText_c[inputLength_a] = 0;
            AddNetBoxLine(inputText_c, gpGame->m_players[NetPosToGamePos(giThisNetPos)].m_color);
            result_a = TransmitRemoteData(
                inputText_c,
                BOX_PACKET_BUFFER_SIZE,
                strlen(inputText_c) + 1,
                BOX_REMOTE_CHAT,
                1,
                1,
                REMOTE_MESSAGE_DEFAULT
            );
            if (!result_a)
                ShutDown(NULL);
            inputLength_a = 0;
            strcpy(inputText_c, DATA_COMPGEN(0x005165d8, popNetBoxEmptyString2, ""));
            updateInput_a = 1;
            redrawLines_f = 1;
        }

        if (redrawLines_f) {
            redrawLines_f = 0;
            updateMessage_f.type = NET_BOX_UPDATE_MESSAGE;
            updateMessage_f.payload.widget.command = NET_BOX_TEXT_COMMAND;
            updateMessage_f.payload.widget.id = BOX_FIRST_LINE_ID;
            updateMessage_f.payload.widget.data.text = cNetBoxLine[0];
            netWindow_j->BroadcastMessage(updateMessage_f);
            updateMessage_f.payload.widget.id = BOX_FIRST_LINE_ID + 1;
            updateMessage_f.payload.widget.data.text = cNetBoxLine[1];
            netWindow_j->BroadcastMessage(updateMessage_f);
            updateMessage_f.payload.widget.id = BOX_FIRST_LINE_ID + 2;
            updateMessage_f.payload.widget.data.text = cNetBoxLine[2];
            netWindow_j->BroadcastMessage(updateMessage_f);
            updateMessage_f.payload.widget.id = BOX_FIRST_LINE_ID + 3;
            updateMessage_f.payload.widget.data.text = cNetBoxLine[3];
            netWindow_j->BroadcastMessage(updateMessage_f);
            updateMessage_f.payload.widget.command = NET_BOX_COLOR_COMMAND;
            updateMessage_f.payload.widget.id = BOX_FIRST_COLOR_ID;
            updateMessage_f.payload.widget.data.value =
                cNetBoxColor[0] + BOX_COLOR_FRAME_OFFSET;
            netWindow_j->BroadcastMessage(updateMessage_f);
            updateMessage_f.payload.widget.id = BOX_FIRST_COLOR_ID + 1;
            updateMessage_f.payload.widget.data.value =
                cNetBoxColor[1] + BOX_COLOR_FRAME_OFFSET;
            netWindow_j->BroadcastMessage(updateMessage_f);
            updateMessage_f.payload.widget.id = BOX_FIRST_COLOR_ID + 2;
            updateMessage_f.payload.widget.data.value =
                cNetBoxColor[2] + BOX_COLOR_FRAME_OFFSET;
            netWindow_j->BroadcastMessage(updateMessage_f);
            updateMessage_f.payload.widget.id = BOX_FIRST_COLOR_ID + 3;
            updateMessage_f.payload.widget.data.value =
                cNetBoxColor[3] + BOX_COLOR_FRAME_OFFSET;
            netWindow_j->BroadcastMessage(updateMessage_f);
            netWindow_j->DrawWindow();
            gpWindowManager->UpdateScreenRegion(0, BOX_WINDOW_Y, BOX_WIDTH, BOX_HEIGHT);
        }

        if (updateInput_a) {
            updateInput_a = 0;
            glTimers[GLOBAL_NET_BOX_CURSOR_TIMER_SLOT] = KBTickCount() + BOX_CURSOR_DELAY;
            if (cursorState)
                inputText_c[inputLength_a] = '_';
            else
                inputText_c[inputLength_a] = BOX_CURSOR_GLYPH;
            inputText_c[inputLength_a + 1] = 0;
            updateMessage_f.type = NET_BOX_UPDATE_MESSAGE;
            updateMessage_f.payload.widget.command = NET_BOX_TEXT_COMMAND;
            updateMessage_f.payload.widget.id = BOX_INPUT_ID;
            updateMessage_f.payload.widget.data.text = inputText_c;
            netWindow_j->BroadcastMessage(updateMessage_f);
            netWindow_j->DrawWindow();
            gpWindowManager
                ->UpdateScreenRegion(0, BOX_INPUT_Y, BOX_WIDTH, BOX_INPUT_HEIGHT);
        }

        if (messageTime_b != 0 && messageTime_b + BOX_MESSAGE_TIMEOUT < KBTickCount())
            done_i = 1;
        if (exitForIncomingData_i) {
            for (delay_h = 0; delay_h < BOX_EXIT_DELAY_STEPS; delay_h++) {
                PollSound();
                DelayMilli(BOX_EXIT_DELAY);
            }
            done_i = 1;
        }
    }

    gpInputManager->SetKeyCodeType(INPUT_KEY_CODE_SCAN);
    if (redrawAdventure_i && gbMoveShown) {
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
    bShowIt = savedShowIt_a;
}

VA(0x0049e02b, 0xc7)
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

VA(0x0049e0f2, 0x214)
void ShutDown(char* msg) {
    DATA(0x005165e0) static i16 shutdownSourceLineBase = 0x0ef4;
    char buf[GLOBAL_TEXT_BUFFER_SIZE];
    if (bInShutDown)
        return;
    LogStr(DATA_COMPGEN(0x005165e4, shutDownShutdown, "Shutdown"));
    bInShutDown = 1;
    gbClosingApp = true;
    buf[0] = 0;
    gpMouseManager->SetColorMice(0);
    if (msg) {
        strcpy(buf, msg);
        SetFullScreenStatus(0);
        LogStr(buf);
        MessageBoxA(hwndApp, buf, DATA_COMPGEN(0x005165f0, shutDownUnexpectedProgramTermination, "Unexpected Program Termination"), MB_ICONHAND);
    } else {
        sprintf(buf, DATA_COMPGEN(0x00516610, shutDownBye, "Bye!"));
    }
    ShutDownSmacker();
    gpSoundManager->CDStop();
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
        H2_FREE_AT(mapExtra, DATA_COMPGEN(0x00516618, shutDownSourceFile, RETAIL_FILE), shutdownSourceLineBase + 71);
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

VA(0x0049e306, 0xa2)
void FileError(char* filename) {
    char buf1[FILE_ERROR_BUFFER_SIZE];
    i32 err;
    char buf[FILE_ERROR_BUFFER_SIZE];
    err = errno;
    sprintf(buf1, DATA_COMPGEN(0x00516640, fileErrorFileErrorS, "File Error %s"), strerror(err));
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
    sprintf(buf, DATA_COMPGEN(0x00516650, fileErrorErrorOpeningFileS, "Error opening file %s!"), filename);
    ShutDown(buf);
}

H2_ENUM_BEGIN(SmackFadeConstant)
    SMACK_FADE_FIRST_COLOR       = WINGRAPH_SYSTEM_PALETTE_SIZE,
    SMACK_FADE_COLOR_LIMIT       = WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE,
    SMACK_FADE_MATCH_COLOR_LIMIT = 36,
    SMACK_FADE_DISTANCE_SENTINEL = 999,
    SMACK_FADE_SCREEN_WIDTH      = 640,
    SMACK_FADE_SCREEN_HEIGHT     = 480,
    SMACK_FADE_BLUE_COMPONENT    = 2,
    SMACK_FADE_GREEN_COMPONENT   = 1,
    SMACK_FADE_RED_COMPONENT     = 0
H2_ENUM_END(SmackFadeConstant)

VA(0x0049e3a8, 0x255)
void SmackFade(u8* src, u8* dst) {
    DATA(0x00516668) static i16 smackFadeSourceLineBase = 0x0f61;
    u8* a;
    u8* f;
    i32 k;
    i32 h, i;
    i32 e, b;
    i32 d;
    i32 p;
    u8* j;
    i32 c, g;

    a = NULL;
    f = NULL;
    k = -1;
    a = static_cast<u8*>(H2_ALLOC_AT(
        MISC_PALETTE_BYTE_COUNT, DATA_COMPGEN(0x0051666c, smackFadeSourceFile, RETAIL_FILE),
        smackFadeSourceLineBase + 13
    ));
    f = static_cast<u8*>(H2_ALLOC_AT(
        WINGRAPH_PALETTE_SIZE, DATA_COMPGEN(0x00516694, smackFadeSourceFile2, RETAIL_FILE),
        smackFadeSourceLineBase + 14
    ));
    memset(a, 0, MISC_PALETTE_BYTE_COUNT);
    memset(f, 0, WINGRAPH_PALETTE_SIZE);
    for (h = SMACK_FADE_FIRST_COLOR; h < SMACK_FADE_COLOR_LIMIT; h++) {
        e = (OD_STEER(src[h * MISC_PALETTE_COMPONENT_BYTES + SMACK_FADE_BLUE_COMPONENT])
             + OD_STEER(src[h * MISC_PALETTE_COMPONENT_BYTES + SMACK_FADE_RED_COMPONENT])
             + src[h * MISC_PALETTE_COMPONENT_BYTES + SMACK_FADE_GREEN_COMPONENT])
            / MISC_PALETTE_COMPONENT_BYTES;
        d = SMACK_FADE_DISTANCE_SENTINEL;
        for (i = SMACK_FADE_FIRST_COLOR; i < SMACK_FADE_MATCH_COLOR_LIMIT; i++) {
            b = (OD_STEER(dst[i * MISC_PALETTE_COMPONENT_BYTES + SMACK_FADE_BLUE_COMPONENT])
                 + OD_STEER(dst[i * MISC_PALETTE_COMPONENT_BYTES + SMACK_FADE_RED_COMPONENT])
                 + dst[i * MISC_PALETTE_COMPONENT_BYTES + SMACK_FADE_GREEN_COMPONENT])
                / MISC_PALETTE_COMPONENT_BYTES;
            p = abs(e - b);
            if (OD_STEER(d) > p) {
                d = p;
                k = i;
            }
        }
        memcpy(
            a + h * MISC_PALETTE_COMPONENT_BYTES,
            dst + k * MISC_PALETTE_COMPONENT_BYTES,
            MISC_PALETTE_COMPONENT_BYTES
        );
        f[h] = (u8)k;
    }
    FadeTo(src, a, HIGH_SCORE_FADE_STEPS);
    j = gpWindowManager->m_screen->m_pixels;
    for (c = 0; c < SMACK_FADE_SCREEN_WIDTH; c++) {
        for (g = 0; g < SMACK_FADE_SCREEN_HEIGHT; g++) {
            *j = f[*j];
            j++;
        }
    }
    gpWindowManager->UpdateScreen();
    UpdatePalette(reinterpret_cast<i8*>(dst));
    H2_FREE_AT(a, DATA_COMPGEN(0x005166bc, smackFadeSourceFile3, RETAIL_FILE), smackFadeSourceLineBase + 49);
    H2_FREE_AT(f, DATA_COMPGEN(0x005166e4, smackFadeSourceFile4, RETAIL_FILE), smackFadeSourceLineBase + 50);
}

VA(0x0049e5fd, 0x303)
void ShowCongrats(HighScoreType highScoreType) {
    DATA(0x0051670c) static i16 congratsSourceLineBase = 0x0f97;
    u8 savedPalette[CONGRATS_PALETTE_BUFFER_SIZE];
    i32 baseScore;
    i32 score_e;
    char rating[CONGRATS_RATING_LENGTH];

    gpMouseManager->HideColorPointer();
    memcpy(savedPalette, gpBufferPalette->m_data, MISC_PALETTE_BYTE_COUNT);
    gpWindowManager->m_updateFlags = 0;
    congratsText = static_cast<char*>(H2_ALLOC_AT(
        CONGRATS_TEXT_SIZE, DATA_COMPGEN(0x00516710, showCongratsSourceFile, RETAIL_FILE),
        congratsSourceLineBase + 9
    ));
    baseScore = CalcBaseScore(giCurTurn);
    score_e = gpGame->m_difficultyRating * baseScore / CONGRATS_DIFFICULTY_SCALE;
    gpSoundManager->PlayAmbientMusic(MIDI_NO_TRACK, 0, MIDI_NO_TRACK);

    if (highScoreType == HIGH_SCORE_STANDARD) {
        sprintf(rating, gArmyNames[GetMonType(score_e, highScoreType)]);
    } else if (highScoreType == HIGH_SCORE_EXPANSION_CAMPAIGN) {
        sprintf(rating, gArmyNames[GetMonType(xCampaign.Days(), highScoreType)]);
    } else {
        sprintf(rating, gArmyNames[GetMonType(gpGame->m_campaignScore, highScoreType)]);
    }
    rating[0] -= 'a' - 'A';
    if (static_cast<i8>(gpGame->m_cheated))
        sprintf(rating, DATA_COMPGEN(0x00516738, showCongratsCheater, "Cheater!!!"));

    if (highScoreType == HIGH_SCORE_STANDARD) {
        sprintf(
            congratsText,
            DATA_COMPGEN(0x00516744, showCongratsCongratulationsDaysDBaseScoreD, "Congratulations!\n\nDays: %d\nBase Score: %d\nDifficulty: %d\n\nScore: "
            "%d\n\nRating:\n%s\n"),
            giCurTurn,
            baseScore,
            gpGame->m_difficultyRating,
            score_e,
            rating
        );
    } else if (highScoreType == HIGH_SCORE_EXPANSION_CAMPAIGN) {
        sprintf(
            congratsText,
            DATA_COMPGEN(0x00516798, showCongratsCongratulationsDaysDRatingS, "Congratulations!\n\nDays: %d\n\nRating:\n%s\n"),
            xCampaign.Days(),
            rating
        );
    } else {
        sprintf(
            congratsText,
            DATA_COMPGEN(0x005167c0, showCongratsCongratulationsDaysDRatingS2, "Congratulations!\n\nDays: %d\n\nRating:\n%s\n"),
            gpGame->m_campaignScore,
            rating
        );
    }

    PlaySmacker(CONGRATS_SMACKER);
    memcpy(gpBufferPalette->m_data, gPalette->m_data, MISC_PALETTE_BYTE_COUNT);
    SmackFade(reinterpret_cast<u8*>(gpBufferPalette->m_data), savedPalette);
    memcpy(gPalette->m_data, savedPalette, MISC_PALETTE_BYTE_COUNT);
    memcpy(gpBufferPalette->m_data, gPalette->m_data, MISC_PALETTE_BYTE_COUNT);
    gpMouseManager->ShowColorPointer();
    AddScoreToHighScore(
        score_e,
        giCurTurn,
        gpGame->m_difficultyRating,
        HIGH_SCORE_STANDARD,
        gpGame->m_mapHeader.name
    );
    H2_FREE_AT(congratsText, DATA_COMPGEN(0x005167e8, showCongratsSourceFile2, RETAIL_FILE), congratsSourceLineBase + 78);
    congratsText = NULL;
    gpWindowManager->m_updateFlags = 1;
    memcpy(gpBufferPalette->m_data, gPalette->m_data, MISC_PALETTE_BYTE_COUNT);
}

VA(0x0049e900, 0x99)
void CongratsWait(void) {
    i32 cmd = 0;
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
    SAMPLE_DEFAULT_WAIT_TIME      = 4000
H2_ENUM_END(SamplePlaybackConstant)

VA(0x0049e999, 0x54)
SAMPLE2 LoadPlaySample(char* name) {
    SAMPLE2 ss;
    ss.pSample = gpResourceManager->GetSample(name);
    if (ss.pSample) {
        ss.pSample->m_playbackData.channelType = SAMPLE_PLAYBACK_CHANNEL_GROUP;
        ss.pMem = gpSoundManager->MemorySample(ss.pSample);
    }
    return ss;
}

VA(0x0049e9ed, 0x8f)
void WaitEndSample(SAMPLE2 s, i32 waitTime) {
    i32l endTime;
    if (waitTime < 0)
        waitTime = SAMPLE_DEFAULT_WAIT_TIME;
    endTime = KBTickCount() + waitTime;
    if (s.pMem)
        while (gpSoundManager->DigitalReport(s.pMem, SOUND_DIGITAL_REPORT_PLAYING)
               && KBTickCount() < endTime) {
            Process1WindowsMessage();
            PollSound();
        }
    if (s.pSample)
        gpResourceManager->Dispose((resource*)s.pSample);
}

H2_ENUM_BEGIN(MemoryErrorConstant)
    MEMORY_ERROR_REQUEST_SIZE = 6400
H2_ENUM_END(MemoryErrorConstant)

VA(0x0049ea7c, 0x5d)
void MemError(void) {
    if (gbInMemError)
        return;
    gbInMemError = true;
    LogStr(DATA_COMPGEN(0x00516814, memErrorOutOfMemory, "Out of Memory"));
    sprintf(gText, cOutOfMemory, DATA_COMPGEN(0x00516824, memErrorOutOfMemory2, "Out of memory."), MEMORY_ERROR_REQUEST_SIZE);
    ShutDown(gText);
}

VA(0x0049ead9, 0x3b)
char* GetTownName(i32 i) {
    town* t = GetCastleRec(i);
    return t->m_name;
}

VA(0x0049eb14, 0x3a)
void LoadSystemwideIcons(void) {
    gBuyBuildIcons = gpResourceManager->GetIcon(DATA_COMPGEN(0x00516834, loadSystemwideIconsBuybuildIcn, "buybuild.icn"));
    gSystemIcons = gpResourceManager->GetIcon(DATA_COMPGEN(0x00516844, loadSystemwideIconsSystemIcn, "system.icn"));
}

VA(0x0049eb4e, 0x32)
void UnloadSystemwideIcons(void) {
    gpResourceManager->Dispose((resource*)gBuyBuildIcons);
    gpResourceManager->Dispose((resource*)gSystemIcons);
}

VA(0x0049eb80, 0x10)
void EarlyShutDownSystem(void) {}

VA(0x0049eb90, 0x75)
i32 GameUnsaved(void) {
    if ((gpAdvManager && gpAdvManager->m_active) || (gpCombatManager && gpCombatManager->m_active)
        || (gpTownManager && gpTownManager->m_active))
        return 1;
    else
        return 0;
}

VA(0x0049ec05, 0xa18)
i32 HandleAppSpecificMenuCommands(i32 command) {
    i32 menuChanged;
    hero* currentHeroRec;
    i32 loopIndex;
    HeroSecondarySkill secondarySkillIndex;
    HeroSkillLevel secondaryLevel;
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
            strcpy(gText, DATA_COMPGEN(0x00516850, handleAppSpecificMenuCommandsAreYouSureYouWantTo, "Are you sure you want to restart?  (Your current game will be lost)"));
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
                DATA_COMPGEN(0x00516894, handleAppSpecificMenuCommandsAreYouSureYouWantTo2, "Are you sure you want to load a new game?  (Your current game will be lost)")
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
                        static_cast<SpellType>(loopIndex), APP_MENU_SPELL_COUNT
                    );
                currentHeroRec->m_spellPoints = APP_MENU_CHEAT_SPELL_POINTS;
            }
            break;

        case APP_MENU_CHEAT_RESOURCES:
            gpGame->m_cheated = 1;
            if (gbInCampaign)
                gpGame->m_campaignCheated = 1;
            for (loopIndex = 0; loopIndex < APP_MENU_RESOURCE_COUNT; loopIndex++) {
                if (loopIndex == IDX(RES_GOLD))
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
                secondaryLevel = static_cast<HeroSkillLevel>(
                    (command - APP_MENU_SECONDARY_FIRST) % APP_MENU_SECONDARY_LEVELS
                );
                if (currentHeroRec != NULL)
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

VA(0x0049f61d, 0x310)
void UpdateSystemOptionsMenu(void) {
    i32 menuCommand;
    i32 checkedCommand;

    if (gConfig.gfx[IDX(giCurExe)].showMenu == 0)
        return;
    if (hmnuApp == NULL)
        return;
    if (OD_STEER(hmnuAdv) != hmnuApp)
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

VA(0x0049f92d, 0x99)
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

VA(0x0049f9c6, 0x2a)
void UpdateAppSpecificMenus(void* hMenu) {
    if (hmnuAdv == hMenu)
        UpdateSystemOptionsMenu();
}

VA(0x0049f9f0, 0x2d)
void EarlyResizeWindow(i32 x, i32 y, i32 w, i32 h) {
    if (gbClosingApp)
        return;
}

H2_ENUM_BEGIN(MapAreaConstant)
    MAP_AREA_ORIGIN = 16,
    MAP_AREA_LIMIT  = 448
H2_ENUM_END(MapAreaConstant)

VA(0x0049fa1d, 0x53)
i32 InMapArea(i32 x, i32 y) {
    return x >= MAP_AREA_ORIGIN && x < MAP_AREA_LIMIT && y >= MAP_AREA_ORIGIN
        && y < MAP_AREA_LIMIT;
}

H2_ENUM_BEGIN(DynamicWindowConstant)
    DYNAMIC_WINDOW_STONE      = 0,
    TILE_SIZE                 = 48,
    WINDOW_PADDING            = 48,
    CONTENT_LEFT              = 32,
    CONTENT_TOP               = 16,
    CORNER_SIZE               = 72,
    CORNER_LEFT               = 40,
    CORNER_RIGHT              = 23,
    EDGE_OFFSET               = 24,
    CORNER_FRAME_TOP_LEFT     = 0,
    CORNER_FRAME_TOP_RIGHT    = 1,
    CORNER_FRAME_BOTTOM_RIGHT = 2,
    CORNER_FRAME_BOTTOM_LEFT  = 3,
    BACKGROUND_FRAME          = 12,
    TOP_FRAME_FIRST           = 4,
    TOP_FRAME_LAST            = 5,
    RIGHT_FRAME_FIRST         = 6,
    RIGHT_FRAME_LAST          = 7,
    BOTTOM_FRAME_FIRST        = 8,
    BOTTOM_FRAME_LAST         = 9,
    LEFT_FRAME_FIRST          = 10,
    LEFT_FRAME_LAST           = 11,
H2_ENUM_END(DynamicWindowConstant)

VA(0x0049fa70, 0x6bc)
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
    i32 leftOffset;
    i32 bottomCornerPaddingNum;
    i32 numRows;
    widget* newWidgetTemp;
    i32 columnsSize;
    i32 topOffsetNum;
    i32 contentXPaddingCount;
    i32 centeredHeightCount;
    i32 centeredPadding;
    i32 topCornerPaddingCount;
    i32 bottomOffsetLocal;
    i32 rightOffset;
    i32 contentYPadding;
    i32 edge;
    i32 tileRowPos;
    i32 centeredWidthValue;
    i32 leftCornerPaddingLocal;
    i32 rightCornerPaddingValue;
    i32 stoneWidgetColorSize;
    i32 columnIndex;
    i32 bottomEdgeOffset;
    i32 tileWidth;
    i32 tileHeight;
    i32 topEdgeInset;

    tileWidth = TILE_SIZE;
    tileHeight = TILE_SIZE;
    topEdgeInset = -EDGE_OFFSET;
    bottomEdgeOffset = -EDGE_OFFSET;
    contentXPaddingCount = CONTENT_LEFT;
    contentYPadding = CONTENT_TOP;
    topCornerPaddingCount = CONTENT_TOP;
    bottomCornerPaddingNum = CONTENT_TOP;
    leftCornerPaddingLocal = CONTENT_TOP;
    rightCornerPaddingValue = CONTENT_TOP;
    centeredPadding = CONTENT_LEFT;
    stoneWidgetColorSize = CONTENT_TOP;
    newWidgetTemp = NULL;
    columnsSize = (contentWidth - 1) / TILE_SIZE + 1;
    numRows = (contentHeight - 1) / TILE_SIZE + 1;
    *windowWidth = columnsSize * TILE_SIZE + WINDOW_PADDING;
    *windowHeight = numRows * TILE_SIZE + WINDOW_PADDING;
    centeredWidthValue = columnsSize * TILE_SIZE + CONTENT_LEFT;
    centeredHeightCount = numRows * TILE_SIZE + CONTENT_LEFT;
    if (centered) {
        x += ((boundsWidth - centeredWidthValue) >> 1) - CONTENT_TOP;
        y += (boundsHeight - centeredHeightCount) >> 1;
    }
    *contentLeft = x + CONTENT_LEFT;
    *contentTop = y + CONTENT_TOP;
    *contentRight = columnsSize * TILE_SIZE + *contentLeft - 1;
    *contentBottom = numRows * TILE_SIZE + *contentTop - 1;

    if (windowType != DYNAMIC_WINDOW_STONE)
        return;
    *window = new heroWindow(
        x,
        y,
        *windowWidth,
        *windowHeight,
        WINDOW_FLAG_SAVE_BACKGROUND | WINDOW_FLAG_OWNS_WIDGETS
    );
    leftOffset = *contentLeft - x;
    topOffsetNum = *contentTop - y;
    rightOffset = *contentRight - x;
    bottomOffsetLocal = *contentBottom - y;

    for (tileRowPos = 0; OD_STEER(tileRowPos) < numRows; tileRowPos++) {
        for (columnIndex = 0; OD_STEER(columnIndex) < columnsSize; columnIndex++) {
            newWidgetTemp = new iconWidget(
                columnIndex * TILE_SIZE + leftOffset,
                tileRowPos * TILE_SIZE + topOffsetNum,
                TILE_SIZE,
                TILE_SIZE,
                DATA_COMPGEN(0x005168e0, setupDynamicWindowStonebk2Icn, "stonebk2.icn"),
                BACKGROUND_FRAME,
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (newWidgetTemp == NULL)
                MemError();
            (*window)->AddWidget(newWidgetTemp, -1);
        }
    }

    newWidgetTemp = new iconWidget(
        leftOffset - CORNER_LEFT,
        topOffsetNum - CORNER_LEFT,
        CORNER_SIZE,
        CORNER_SIZE,
        DATA_COMPGEN(0x005168f0, setupDynamicWindowStonebk2Icn2, "stonebk2.icn"),
        CORNER_FRAME_TOP_LEFT,
        ICON_DRAW_NORMAL,
        -1,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (newWidgetTemp == NULL)
        MemError();
    (*window)->AddWidget(newWidgetTemp, -1);

    newWidgetTemp = new iconWidget(
        rightOffset - CORNER_RIGHT,
        topOffsetNum - CORNER_LEFT,
        CORNER_SIZE,
        CORNER_SIZE,
        DATA_COMPGEN(0x00516900, setupDynamicWindowStonebk2Icn3, "stonebk2.icn"),
        CORNER_FRAME_TOP_RIGHT,
        ICON_DRAW_NORMAL,
        -1,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (newWidgetTemp == NULL)
        MemError();
    (*window)->AddWidget(newWidgetTemp, -1);

    newWidgetTemp = new iconWidget(
        rightOffset - CORNER_RIGHT,
        bottomOffsetLocal - CORNER_RIGHT,
        CORNER_SIZE,
        CORNER_SIZE,
        DATA_COMPGEN(0x00516910, setupDynamicWindowStonebk2Icn4, "stonebk2.icn"),
        CORNER_FRAME_BOTTOM_RIGHT,
        ICON_DRAW_NORMAL,
        -1,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (newWidgetTemp == NULL)
        MemError();
    (*window)->AddWidget(newWidgetTemp, -1);

    newWidgetTemp = new iconWidget(
        leftOffset - CORNER_LEFT,
        bottomOffsetLocal - CORNER_RIGHT,
        CORNER_SIZE,
        CORNER_SIZE,
        DATA_COMPGEN(0x00516920, setupDynamicWindowStonebk2Icn5, "stonebk2.icn"),
        CORNER_FRAME_BOTTOM_LEFT,
        ICON_DRAW_NORMAL,
        -1,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (newWidgetTemp == NULL)
        MemError();
    (*window)->AddWidget(newWidgetTemp, -1);

    for (edge = 0; columnsSize > OD_STEER(edge); edge++) {
        newWidgetTemp = new iconWidget(
            edge * TILE_SIZE + leftOffset - EDGE_OFFSET,
            topOffsetNum - CORNER_LEFT,
            CORNER_SIZE,
            CORNER_SIZE,
            DATA_COMPGEN(0x00516930, setupDynamicWindowStonebk2Icn6, "stonebk2.icn"),
            Random(TOP_FRAME_FIRST, TOP_FRAME_LAST),
            ICON_DRAW_NORMAL,
            -1,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (newWidgetTemp == NULL)
            MemError();
        (*window)->AddWidget(newWidgetTemp, -1);

        newWidgetTemp = new iconWidget(
            edge * TILE_SIZE + leftOffset - EDGE_OFFSET,
            bottomOffsetLocal - CORNER_RIGHT,
            CORNER_SIZE,
            CORNER_SIZE,
            DATA_COMPGEN(0x00516940, setupDynamicWindowStonebk2Icn7, "stonebk2.icn"),
            Random(BOTTOM_FRAME_FIRST, BOTTOM_FRAME_LAST),
            ICON_DRAW_NORMAL,
            -1,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (newWidgetTemp == NULL)
            MemError();
        (*window)->AddWidget(newWidgetTemp, -1);
    }

    for (edge = 0; OD_STEER(edge) < numRows; edge++) {
        newWidgetTemp = new iconWidget(
            leftOffset - CORNER_LEFT,
            edge * TILE_SIZE + topOffsetNum - EDGE_OFFSET,
            CORNER_SIZE,
            CORNER_SIZE,
            DATA_COMPGEN(0x00516950, setupDynamicWindowStonebk2Icn8, "stonebk2.icn"),
            Random(LEFT_FRAME_FIRST, LEFT_FRAME_LAST),
            ICON_DRAW_NORMAL,
            -1,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (newWidgetTemp == NULL)
            MemError();
        (*window)->AddWidget(newWidgetTemp, -1);

        newWidgetTemp = new iconWidget(
            rightOffset - CORNER_RIGHT,
            edge * TILE_SIZE + topOffsetNum - EDGE_OFFSET,
            CORNER_SIZE,
            CORNER_SIZE,
            DATA_COMPGEN(0x00516960, setupDynamicWindowStonebk2Icn9, "stonebk2.icn"),
            Random(RIGHT_FRAME_FIRST, RIGHT_FRAME_LAST),
            ICON_DRAW_NORMAL,
            -1,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (newWidgetTemp == NULL)
            MemError();
        (*window)->AddWidget(newWidgetTemp, -1);
    }
}

VA(0x004a012c, 0x108)
void TestDynamicWindow(i32 p1, i32 p2) {
    heroWindow* p;
    i32 q, r, s, u, v, w;
    i32 t;
    SetupDynamicWindow(
        0,
        0,
        1,
        WINGRAPH_WIDTH,
        WINGRAPH_HEIGHT,
        p1 * TILE_SIZE,
        p2 * TILE_SIZE,
        &s,
        &u,
        &v,
        &w,
        &q,
        &r,
        &p,
        DYNAMIC_WINDOW_STONE
    );
    gpWindowManager->AddWindow(p, -1, 1);
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
    gpWindowManager->RemoveWindow(p);
    delete p;
}

VA(0x004a0234, 0x91)
void HandleRemoteDeadPlayerExit(i32 pos) {
    SPlayerExit pe;
    if (giThisGamePos == pos) {
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
    PLAYER_EXIT_HOST_NET_POSITION        = 0,
    PLAYER_EXIT_FIRST_GUEST_NET_POSITION = 1,
    PLAYER_EXIT_DIRECT_PLAYER_COUNT      = 2,
    PLAYER_EXIT_MESSAGE_LENGTH           = 500,
    PLAYER_EXIT_TRANSMIT_DELAY           = 500,
    PLAYER_EXIT_MESSAGE_TIME             = 5000,
    PLAYER_EXIT_HEARTBEAT_DISABLED       = 0x0bebc1ff
H2_ENUM_END(PlayerExitLocalConstant)

VA(0x004a02c5, 0xaa)
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
    // Retail leaves the final two bytes unset. The receiver replaces continueGame and only
    // consults hostReported for eliminated packets.
    if (giThisNetPos == PLAYER_EXIT_HOST_NET_POSITION)
        destination = PLAYER_EXIT_FIRST_GUEST_NET_POSITION;
    else
        destination = PLAYER_EXIT_HOST_NET_POSITION;
    LogStr(DATA_COMPGEN(0x00516970, handleRemoteSuddenExitHRSE1, "HRSE1"));
    TransmitRemoteData(
        reinterpret_cast<char*>(&exitInfo),
        destination,
        sizeof(exitInfo),
        ADVMGR_REMOTE_COMMAND_PLAYER_EXIT,
        0,
        0,
        REMOTE_MESSAGE_RELIABLE
    );
    LogStr(DATA_COMPGEN(0x00516978, handleRemoteSuddenExitHRSE2, "HRSE2"));
    DelayMilli(PLAYER_EXIT_TRANSMIT_DELAY);
}

VA(0x004a036f, 0x62)
void DropDownToOnePlayer(void) {
    RemoteCleanup();
    giNumHumanPlayers = 1;
    for (i32 i = 0; i < REMOTE_PLAYER_COUNT; i++)
        if (giThisNetPos != i)
            gbHumanPlayer[i] = 0;
    ComputeAdvNetControl();
}

VA(0x004a03d1, 0x412)
void ReceiveHostReportsPlayerExit(i32 hostNetPosition, SPlayerExit exitInfo, i32 forwardedReport) {
    i32 showExitMessage;
    char playerExitMessage[PLAYER_EXIT_MESSAGE_LENGTH];
    i32 netPosition;

    showExitMessage = 0;
    if (!forwardedReport) {
        if (exitInfo.eliminated) {
            if (exitInfo.netPosition == giThisNetPos) {
                RemoteCleanup();
                sprintf(gText, DATA_COMPGEN(0x00516980, receiveHostReportsPlayerExitYouHaveBeenEliminatedFromThe, "You have been eliminated from the game!!!"));
                NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                gbGameOver = true;
                giEndSequence = 0;
                return;
            }

            sprintf(gText, DATA_COMPGEN(0x005169ac, receiveHostReportsPlayerExitSHasBeenVanquished, "%s has been vanquished!"), gsNetPlayerInfo[exitInfo.netPosition].name);
            NormalDialog(
                gText,
                NORMAL_DIALOG_INFO,
                -1,
                -1,
                NORMAL_DIALOG_CREST,
                gpGame->GetPlayerColor(exitInfo.gamePosition),
                -1,
                -1,
                -1,
                PLAYER_EXIT_MESSAGE_TIME
            );
        } else if (!exitInfo.continueGame) {
            gpGame->SaveGame(DATA_COMPGEN(0x005169c4, receiveHostReportsPlayerExitPLYREXIT, "PLYREXIT"), 1, 0);
            sprintf(
                gText,
                DATA_COMPGEN(0x005169d0, receiveHostReportsPlayerExitSLeftTheGameAndThe, "%s left the game, and the %s decided to terminate the entire game.  The game has "
                "been saved as 'PLYREXIT'.  Do you wish to play on alone with the computer taking "
                "over for all human players?"),
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
                    playerExitMessage,
                    DATA_COMPGEN(0x00516a90, receiveHostReportsPlayerExitHostPlayerSReportsThatPlayer, "Host player %s reports that player %s has been timed out of the game.  The "
                    "game will continue with a computer player filling in for %s."),
                    gsNetPlayerInfo[hostNetPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name
                );
            } else {
                sprintf(
                    playerExitMessage,
                    DATA_COMPGEN(0x00516b18, receiveHostReportsPlayerExitHostPlayerSReportsThatPlayer2, "Host player %s reports that player %s has exited the game.  The game will "
                    "continue with a computer player filling in for %s."),
                    gsNetPlayerInfo[hostNetPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name,
                    gsNetPlayerInfo[exitInfo.netPosition].name
                );
            }
            showExitMessage = 1;
        }
    }

    if (exitInfo.netPosition < giThisNetPos)
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

    if (showExitMessage)
        NormalDialog(
            playerExitMessage,
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

VA(0x004a07e3, 0x361)
void ReceiveRemotePlayerExit(SPlayerExit exitInfo) {
    i32 localPlayerLost;
    i32 sendReturn;
    i32 unusedPacketResult;
    i32 recipient;

    localPlayerLost = 0;
    lLastHeartbeatReceive[exitInfo.netPosition] = PLAYER_EXIT_HEARTBEAT_DISABLED;
    gpGame->SaveGame(DATA_COMPGEN(0x00516b98, receiveRemotePlayerExitPLYREXIT, "PLYREXIT"), 1, 0);

    if (exitInfo.eliminated) {
        exitInfo.continueGame = 1;
        if (exitInfo.netPosition == giThisNetPos) {
            localPlayerLost = 1;
            goto exitInfoProcessed;
        }
        sprintf(gText, DATA_COMPGEN(0x00516ba4, receiveRemotePlayerExitSHasBeenVanquished, "%s has been vanquished!"), gsNetPlayerInfo[exitInfo.netPosition].name);
        NormalDialog(
            gText,
            NORMAL_DIALOG_INFO,
            -1,
            -1,
            NORMAL_DIALOG_CREST,
            gpGame->GetPlayerColor(exitInfo.gamePosition),
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
                DATA_COMPGEN(0x00516bbc, receiveRemotePlayerExitSHasBeenTimedOutOf, "%s has been timed out of the game.  The current game has been saved as "
                "'PLYREXIT'.  Do you wish to continue playing with a computer player filling in "
                "for %s?"),
                gsNetPlayerInfo[exitInfo.netPosition].name,
                gsNetPlayerInfo[exitInfo.netPosition].name
            );
        } else {
            sprintf(
                gText,
                DATA_COMPGEN(0x00516c5c, receiveRemotePlayerExitSIsExitingTheGameThe, "%s is exiting the game.  The current game has been saved as 'PLYREXIT'.  Do you "
                "wish to continue playing with a computer player filling in for %s?"),
                gsNetPlayerInfo[exitInfo.netPosition].name,
                gsNetPlayerInfo[exitInfo.netPosition].name
            );
        }
        NormalDialog(gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE)
            exitInfo.continueGame = 1;
        else
            exitInfo.continueGame = 0;
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
        if (localPlayerLost)
            goto playerExitHandled;
        giNumHumanPlayers--;
        gbHumanPlayer[exitInfo.gamePosition] = 0;
        RemoteCleanup();
        ComputeAdvNetControl();
    } else {
        for (recipient = 0; recipient < REMOTE_PLAYER_COUNT; recipient++) {
            if ((exitInfo.netPosition == recipient && exitInfo.eliminated && !exitInfo.hostReported)
                || (exitInfo.netPosition != recipient && giNumHumanPlayers > recipient
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
        if (localPlayerLost)
            goto playerExitHandled;
        ReceiveHostReportsPlayerExit(0, exitInfo, 1);
    }

playerExitHandled:
    if (localPlayerLost) {
        sprintf(gText, DATA_COMPGEN(0x00516cf0, receiveRemotePlayerExitYouHaveBeenEliminatedFromThe, "You have been eliminated from the game!!!"));
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
    CHECK_MEMORY_TOTAL_KB             = 16000,
    CHECK_MEMORY_AVAILABLE_KB         = 8000
H2_ENUM_END(CheckMemoryConstant)

VA(0x004a0b44, 0x29)
i32 CheckMem(void) {
    giTotalHighMem = CHECK_MEMORY_TOTAL_KB;
    giHighMemBuffer = CHECK_MEMORY_AVAILABLE_KB;
    return 1;
}

VA(0x004a0b6d, 0x109)
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

VA(0x004a0c76, 0x9f)
void SetWinText(heroWindow* j, i32 id) {
    i32 a = 0;
    i32 i;
    tag_message c;
    for (i = 0; i < KB_WIN_SETUP_COUNT; i++) {
        if (gWinSetup[i].windowId == id) {
            a++;
            c.type = MESSAGE_WIDGET;
            c.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            c.payload.widget.id = gWinSetup[i].widgetId;
            c.payload.widget.data.text = gWinSetup[i].text;
            j->BroadcastMessage(c);
        }
    }
}

H2_ENUM_BEGIN(ShingleAnimationConstant)
    SHINGLE_ANIMATION_INTERVAL     = 250,
    SHINGLE_ANIMATION_FRAME_COUNT  = 39,
    SHINGLE_ANIMATION_FRAME_OFFSET = 1,
    SHINGLE_UPDATE_X               = 46,
    SHINGLE_UPDATE_Y               = 176,
    SHINGLE_UPDATE_WIDTH           = 139,
    SHINGLE_UPDATE_HEIGHT          = 187
H2_ENUM_END(ShingleAnimationConstant)

VA(0x004a0d15, 0x8a)
void CheckShingleUpdate(void) {
    if (!gShingleAnim)
        return;
    if (KBTickCount() > iNextShingleAnim) {
        iNextShingleAnim = KBTickCount() + SHINGLE_ANIMATION_INTERVAL;
        iShingleAnimFrame =
            (iShingleAnimFrame + SHINGLE_ANIMATION_FRAME_OFFSET)
            % SHINGLE_ANIMATION_FRAME_COUNT;
        gShingleAnim->DrawToBuffer(
            0, 0, iShingleAnimFrame + SHINGLE_ANIMATION_FRAME_OFFSET, ICON_DRAW_NORMAL
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
    NORMAL_DIALOG_PRIMARY_BONUS_TEXT_LENGTH        = 5,
    NORMAL_DIALOG_OR_TEXT_LENGTH                   = 3,
    NORMAL_DIALOG_RESOURCE_LABEL_HEIGHT            = 12,
    NORMAL_DIALOG_SPELL_LABEL_HEIGHT               = 24,
    NORMAL_DIALOG_RESOURCE_LABEL_WIDTH             = 100,
    NORMAL_DIALOG_RESOURCE_LABEL_HALF_WIDTH        = NORMAL_DIALOG_RESOURCE_LABEL_WIDTH / 2,
    NORMAL_DIALOG_PRIMARY_BONUS_TEXT_HEIGHT        = 16,
    NORMAL_DIALOG_OR_TEXT_WIDTH                    = 40,
    NORMAL_DIALOG_OR_TEXT_HEIGHT                   = 12,
    NORMAL_DIALOG_RESOURCE_LAYOUT_HEIGHT           = 44,
    NORMAL_DIALOG_RESOURCE_ICON_WIDTH              = 38,
    NORMAL_DIALOG_RESOURCE_ICON_HEIGHT             = 32,
    NORMAL_DIALOG_GOLD_LAYOUT_HEIGHT               = 26,
    NORMAL_DIALOG_LARGE_ICON_WIDTH                 = 76,
    NORMAL_DIALOG_ARTIFACT_ICON_HEIGHT             = 76,
    NORMAL_DIALOG_SPELL_LAYOUT_HEIGHT              = 79,
    NORMAL_DIALOG_SPELL_ICON_WIDTH                 = 70,
    NORMAL_DIALOG_SPELL_ICON_HEIGHT                = 55,
    NORMAL_DIALOG_CREST_ICON_WIDTH                 = 50,
    NORMAL_DIALOG_CREST_ICON_HEIGHT                = 55,
    NORMAL_DIALOG_EXPMRL_ICON_WIDTH                = 64,
    NORMAL_DIALOG_LUCK_BONUS_ICON_HEIGHT           = 28,
    NORMAL_DIALOG_LUCK_PENALTY_ICON_HEIGHT         = 57,
    NORMAL_DIALOG_MORALE_BONUS_ICON_HEIGHT         = 62,
    NORMAL_DIALOG_MORALE_PENALTY_ICON_HEIGHT       = 59,
    NORMAL_DIALOG_EXPERIENCE_ICON_HEIGHT           = 64,
    NORMAL_DIALOG_HERO_LAYOUT_HEIGHT               = 111,
    NORMAL_DIALOG_HERO_BACKGROUND_WIDTH            = 111,
    NORMAL_DIALOG_HERO_BACKGROUND_HEIGHT           = 105,
    NORMAL_DIALOG_SECONDARY_SKILL_LAYOUT_HEIGHT    = 81,
    NORMAL_DIALOG_SECONDARY_SKILL_BACKGROUND_WIDTH = 75,
    NORMAL_DIALOG_SECONDARY_SKILL_ICON_HEIGHT      = 65,
    NORMAL_DIALOG_PRIMARY_MONSTER_LAYOUT_HEIGHT    = 105,
    NORMAL_DIALOG_PRIMARY_MONSTER_BACKGROUND_WIDTH = 94,
    NORMAL_DIALOG_PRIMARY_MONSTER_ICON_HEIGHT      = 105,
    NORMAL_DIALOG_PRIMARY_MONSTER_OVERLAY_WIDTH    = 82,
    NORMAL_DIALOG_PRIMARY_MONSTER_OVERLAY_HEIGHT   = 93,
    NORMAL_DIALOG_CREST_OVERLAY_WIDTH              = 58,
    NORMAL_DIALOG_CREST_OVERLAY_HEIGHT             = 55,
    NORMAL_DIALOG_SECONDARY_SKILL_OVERLAY_WIDTH    = 71,
    NORMAL_DIALOG_SECONDARY_SKILL_OVERLAY_HEIGHT   = 81,
    NORMAL_DIALOG_HERO_OVERLAY_WIDTH               = 101,
    NORMAL_DIALOG_HERO_OVERLAY_HEIGHT              = 95,
    NORMAL_DIALOG_BUTTON_AREA_HEIGHT               = 39,
    NORMAL_DIALOG_RESOURCE_VERTICAL_GAP            = 14,
    NORMAL_DIALOG_ROW_CALCULATION_OFFSET           = 25,
    NORMAL_DIALOG_DEFAULT_X                        = 159,
    NORMAL_DIALOG_SINGLE_RESOURCE_CENTER_INSET     = 17,
    NORMAL_DIALOG_FIRST_RESOURCE_CENTER_X          = 104,
    NORMAL_DIALOG_SECOND_RESOURCE_RIGHT_INSET      = 87,
    NORMAL_DIALOG_RESOURCE_BOTTOM_INSET            = 48,
    NORMAL_DIALOG_SECONDARY_PAIR_SPACING           = 4,
    NORMAL_DIALOG_ICON_OVERLAY_INSET               = 6,
    NORMAL_DIALOG_SPELL_BACKGROUND_X_OFFSET        = 2,
    NORMAL_DIALOG_CREST_OVERLAY_OUTSET             = 4,
    NORMAL_DIALOG_SECONDARY_SKILL_OVERLAY_OUTSET   = 3,
    NORMAL_DIALOG_HERO_OVERLAY_INSET               = 5,
    NORMAL_DIALOG_SECONDARY_NAME_Y_OFFSET          = 72,
    NORMAL_DIALOG_SECONDARY_LEVEL_Y_OFFSET         = 24,
    NORMAL_DIALOG_PRIMARY_LABEL_Y_OFFSET           = 93,
    NORMAL_DIALOG_DEFAULT_LABEL_Y_OFFSET           = 10,
    NORMAL_DIALOG_PRIMARY_BONUS_LABEL_Y_OFFSET     = 22,
    NORMAL_DIALOG_OR_TEXT_CENTER_X_OFFSET          = 10,
    NORMAL_DIALOG_OR_TEXT_Y_OFFSET                 = 43,
    NORMAL_DIALOG_CENTER_PART_COUNT                = 2,
    NORMAL_DIALOG_PRIMARY_BACKGROUND_FRAME         = 4,
    NORMAL_DIALOG_MONSTER_BACKGROUND_FRAME         = 12,
    NORMAL_DIALOG_SECONDARY_BACKGROUND_FRAME_BASE  = 1,
    NORMAL_DIALOG_HERO_BACKGROUND_FRAME            = 4,
    NORMAL_DIALOG_ARTIFACT_FRAME_OFFSET            = 1,
    NORMAL_DIALOG_MONSTER_RACE_FRAME_OFFSET        = 4,
    NORMAL_DIALOG_CREST_OVERLAY_FRAME              = 6,
    NORMAL_DIALOG_SECONDARY_OVERLAY_FRAME          = 15
H2_ENUM_END(NormalDialogLayoutConstant)

inline i32 NormalDialogCenterOffset(i32 extent) {
    return extent / NORMAL_DIALOG_CENTER_PART_COUNT;
}

VA(0x004a0d9f, 0x17c6)
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
    DATA(0x00516d20) static i16 normalDialogSourceLineBase = 0x14a5;
    i32 panelHeight_p;
    i32 labelY_o;
    widget* borderWidget_o;
    char iconFile_h[NORMAL_DIALOG_FILENAME_LENGTH];
    char* resourceText_e[NORMAL_DIALOG_RESOURCE_COUNT];
    i32 iconHeight_d;
    i32 sizingIconHeight_l;
    i32 showPrimaryBonus_e;
    i32 resourceType_l[NORMAL_DIALOG_RESOURCE_COUNT];
    i32 lineCount_d;
    widget* iconPanel_j;
    heroWindow* savedNormalDialogWindow_o;
    i32 windowWidth_a;
    i32 savedFirstResourceType_p;
    i32 resourceImageWidth_g;
    i16 showMessage_h;
    i32 windowHeight_k;
    i32 resourceSlot_n;
    MouseCursorType savedPointerType_e;
    i32 dialogContentHeight_h;
    i32 textWidgetId_h;
    tag_message message_e;
    i32 savedSecondResourceType_f;
    i32 windowRows_j;
    i32 maxIconHeight_a;
    i32 savedFirstResourceValue_i;
    i32 resourceY_l;
    i32 resourceFrame_g;
    widget* textPanel_h;
    i32 resourceCenterX_a;
    i32 resourceValue_l[NORMAL_DIALOG_RESOURCE_COUNT];
    i32 savedSecondResourceValue_j;
    char* orText_f;
    i32 savedPointerFrame_j;
    i32 imageHeight_b;

    if (!gbRemoteOn)
        timeout = 0;
    if (timeout > NORMAL_DIALOG_TIMEOUT_MIN && timeout < NORMAL_DIALOG_TIMEOUT_MAX) {
        giDialogTimeout = KBTickCount() + timeout;
    } else {
        giDialogTimeout = timeout;
    }

    resourceCenterX_a = 0;
    resourceY_l = 0;
    resourceFrame_g = 0;
    textWidgetId_h = NORMAL_DIALOG_TEXT_WIDGET_FIRST_ID;
    resourceImageWidth_g = 0;
    iconHeight_d = 0;
    showPrimaryBonus_e = 0;
    showMessage_h = 1;

    if (firstResourceType == NORMAL_DIALOG_PRIMARY_SKILL
        && firstResourceValue >= NORMAL_DIALOG_PRIMARY_BONUS_OFFSET) {
        firstResourceValue -= NORMAL_DIALOG_PRIMARY_BONUS_OFFSET;
        showPrimaryBonus_e = 1;
    }
    if (firstResourceType >= NORMAL_DIALOG_MONSTER + 1
        && firstResourceType <= NORMAL_DIALOG_PRIMARY_SKILL - 1) {
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
        dialogContentHeight_h += NORMAL_DIALOG_BUTTON_AREA_HEIGHT;

    for (resourceSlot_n = 0; resourceSlot_n < NORMAL_DIALOG_RESOURCE_COUNT; resourceSlot_n++) {
        switch (resourceType_l[resourceSlot_n]) {
            case NORMAL_DIALOG_ARTIFACT:
                sizingIconHeight_l = NORMAL_DIALOG_ARTIFACT_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_LUCK_BONUS:
                sizingIconHeight_l = NORMAL_DIALOG_LUCK_BONUS_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_LUCK_PENALTY:
                sizingIconHeight_l = NORMAL_DIALOG_LUCK_PENALTY_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_MORALE_BONUS:
                sizingIconHeight_l = NORMAL_DIALOG_MORALE_BONUS_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_MORALE_PENALTY:
                sizingIconHeight_l = NORMAL_DIALOG_MORALE_PENALTY_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_EXPERIENCE:
                sizingIconHeight_l = resourceValue_l[resourceSlot_n] == NORMAL_DIALOG_NO_VALUE
                    ? NORMAL_DIALOG_EXPERIENCE_ICON_HEIGHT
                    : NORMAL_DIALOG_EXPERIENCE_ICON_HEIGHT
                        + NORMAL_DIALOG_RESOURCE_LABEL_HEIGHT;
                break;
            case NORMAL_DIALOG_CREST:
                sizingIconHeight_l = NORMAL_DIALOG_CREST_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_HERO:
                sizingIconHeight_l = NORMAL_DIALOG_HERO_LAYOUT_HEIGHT;
                break;
            case NORMAL_DIALOG_RESOURCE_GOLD:
                sizingIconHeight_l = NORMAL_DIALOG_GOLD_LAYOUT_HEIGHT;
                break;
            case NORMAL_DIALOG_RESOURCE_WOOD:
            case NORMAL_DIALOG_RESOURCE_MERCURY:
            case NORMAL_DIALOG_RESOURCE_ORE:
            case NORMAL_DIALOG_RESOURCE_SULFUR:
            case NORMAL_DIALOG_RESOURCE_CRYSTAL:
            case NORMAL_DIALOG_RESOURCE_GEMS:
                sizingIconHeight_l = NORMAL_DIALOG_RESOURCE_LAYOUT_HEIGHT;
                break;
            case NORMAL_DIALOG_SPELL:
                sizingIconHeight_l = NORMAL_DIALOG_SPELL_LAYOUT_HEIGHT;
                break;
            case NORMAL_DIALOG_SECONDARY_SKILL:
                sizingIconHeight_l = NORMAL_DIALOG_SECONDARY_SKILL_LAYOUT_HEIGHT;
                break;
            case NORMAL_DIALOG_MONSTER:
            case NORMAL_DIALOG_PRIMARY_SKILL:
                sizingIconHeight_l = NORMAL_DIALOG_PRIMARY_MONSTER_LAYOUT_HEIGHT;
                break;
            default:
                sizingIconHeight_l = 0;
                break;
        }
        if (maxIconHeight_a < sizingIconHeight_l)
            maxIconHeight_a = sizingIconHeight_l;
    }

    if (maxIconHeight_a)
        dialogContentHeight_h += maxIconHeight_a + NORMAL_DIALOG_RESOURCE_VERTICAL_GAP;
    windowRows_j =
        (dialogContentHeight_h - NORMAL_DIALOG_ROW_CALCULATION_OFFSET)
        / NORMAL_DIALOG_WINDOW_ROW_HEIGHT;
    if (windowRows_j > NORMAL_DIALOG_MAX_ROWS)
        windowRows_j = NORMAL_DIALOG_MAX_ROWS;
    windowWidth_a = NORMAL_DIALOG_WINDOW_WIDTH;
    windowHeight_k =
        windowRows_j * NORMAL_DIALOG_WINDOW_ROW_HEIGHT + NORMAL_DIALOG_WINDOW_BASE_HEIGHT;

    if (windowX == -1 || windowX + windowWidth_a >= NORMAL_DIALOG_SCREEN_RIGHT)
        windowX = NORMAL_DIALOG_DEFAULT_X;
    if (windowY == -1 || windowY + windowHeight_k >= NORMAL_DIALOG_SCREEN_BOTTOM) {
        windowY = NormalDialogCenterOffset(NORMAL_DIALOG_SCREEN_HEIGHT - windowHeight_k);
        if (windowY > NORMAL_DIALOG_MAX_TOP)
            windowY = NORMAL_DIALOG_MAX_TOP;
    }

    sprintf(iconFile_h, DATA_COMPGEN(0x00516d24, normalDialogEvntwinDBin, "evntwin%d.bin"), windowRows_j);
    pNormalDialogWindow = new heroWindow(windowX, windowY, iconFile_h);
    if (!pNormalDialogWindow)
        MemError();

    message_e.type = NORMAL_DIALOG_DISABLE_MESSAGE;
    message_e.payload.widget.command = NORMAL_DIALOG_DISABLE_COMMAND;
    message_e.payload.widget.data.text = reinterpret_cast<char*>(NORMAL_DIALOG_DISABLE_COMMAND);
    if (dialogType != NORMAL_DIALOG_DISABLE_SEVENTH && dialogType != NORMAL_DIALOG_DISABLE_EIGHTH) {
        message_e.payload.widget.id = NORMAL_DIALOG_BUTTON_SEVEN;
        pNormalDialogWindow->BroadcastMessage(message_e);
    }
    if (dialogType != NORMAL_DIALOG_DISABLE_SEVENTH) {
        message_e.payload.widget.id = NORMAL_DIALOG_BUTTON_EIGHT;
        pNormalDialogWindow->BroadcastMessage(message_e);
    }
    if (dialogType != NORMAL_DIALOG_WAIT_LAST && dialogType != NORMAL_DIALOG_BUTTON_PAIR) {
        message_e.payload.widget.id = NORMAL_DIALOG_BUTTON_ONE;
        pNormalDialogWindow->BroadcastMessage(message_e);
    }
    if (dialogType != NORMAL_DIALOG_WAIT_FIRST && dialogType != NORMAL_DIALOG_INFO
        && dialogType != NORMAL_DIALOG_BUTTON_PAIR) {
        message_e.payload.widget.id = NORMAL_DIALOG_BUTTON_TWO;
        pNormalDialogWindow->BroadcastMessage(message_e);
    }
    if (dialogType != NORMAL_DIALOG_CONFIRM) {
        message_e.payload.widget.id = NORMAL_DIALOG_BUTTON_FIVE;
        pNormalDialogWindow->BroadcastMessage(message_e);
        message_e.payload.widget.id = NORMAL_DIALOG_BUTTON_SIX;
        pNormalDialogWindow->BroadcastMessage(message_e);
    }

    for (resourceSlot_n = 0; resourceSlot_n < NORMAL_DIALOG_RESOURCE_COUNT; resourceSlot_n++) {
        iconPanel_j = NULL;
        textPanel_h = NULL;
        if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_NO_RESOURCE)
            break;

        resourceText_e[resourceSlot_n] = static_cast<char*>(H2_ALLOC_AT(
            NORMAL_DIALOG_TEXT_LENGTH, DATA_COMPGEN(0x00516d34, normalDialogSourceFile, RETAIL_FILE),
            normalDialogSourceLineBase + 187
        ));
        if (resourceType_l[resourceSlot_n] >= NORMAL_DIALOG_RESOURCE_FIRST
            && resourceType_l[resourceSlot_n] <= NORMAL_DIALOG_RESOURCE_LAST) {
            if (resourceValue_l[resourceSlot_n] < 1) {
                if (resourceValue_l[resourceSlot_n] == 0) {
                    strcpy(resourceText_e[resourceSlot_n], DATA_COMPGEN(0x00516d90, normalDialogEmptyString, ""));
                } else if (resourceValue_l[resourceSlot_n] < -NORMAL_DIALOG_DAILY_RESOURCE_OFFSET) {
                    sprintf(
                        resourceText_e[resourceSlot_n],
                        DATA_COMPGEN(0x00516e0c, normalDialogD, "%d"),
                        resourceValue_l[resourceSlot_n] + NORMAL_DIALOG_DAILY_RESOURCE_OFFSET
                    );
                } else {
                    sprintf(
                        resourceText_e[resourceSlot_n],
                        DATA_COMPGEN(0x00516d68, normalDialogDDay, "%d/day"),
                        -resourceValue_l[resourceSlot_n]
                    );
                }
            } else {
                sprintf(resourceText_e[resourceSlot_n], DATA_COMPGEN(0x00516d5c, normalDialogResourceValueFormat, "%d"), resourceValue_l[resourceSlot_n]);
            }
            strcpy(iconFile_h, DATA_COMPGEN(0x00516e14, normalDialogResourceIcn, "resource.icn"));
            resourceFrame_g = resourceType_l[resourceSlot_n];
        } else if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_SPELL) {
            sprintf(
                resourceText_e[resourceSlot_n],
                DATA_COMPGEN(0x00516d94, normalDialogS, "%s"),
                gSpellNames[resourceValue_l[resourceSlot_n]]
            );
            strcpy(iconFile_h, DATA_COMPGEN(0x00516d84, normalDialogSpellsIcn, "spells.icn"));
            resourceFrame_g = gsSpellInfo[resourceValue_l[resourceSlot_n]].iconIndex;
        } else if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_CREST) {
            sprintf(resourceText_e[resourceSlot_n], DATA_COMPGEN(0x00516da8, normalDialogS2, "%s"), DATA_COMPGEN(0x00516da4, normalDialogEmptyString2, ""));
            strcpy(iconFile_h, DATA_COMPGEN(0x00516d98, normalDialogBrcrestIcn, "brcrest.icn"));
            resourceFrame_g = resourceValue_l[resourceSlot_n];
        } else if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_PRIMARY_SKILL) {
            sprintf(resourceText_e[resourceSlot_n], DATA_COMPGEN(0x00516dc0, normalDialogS3, "%s"), DATA_COMPGEN(0x00516dbc, normalDialogEmptyString3, ""));
            strcpy(iconFile_h, DATA_COMPGEN(0x00516dac, normalDialogPrimskilIcn, "primskil.icn"));
            resourceFrame_g = NORMAL_DIALOG_PRIMARY_BACKGROUND_FRAME;
        } else if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_MONSTER) {
            sprintf(resourceText_e[resourceSlot_n], DATA_COMPGEN(0x00516dd0, normalDialogS4, "%s"), DATA_COMPGEN(0x00516de4, normalDialogEmptyString4, ""));
            strcpy(iconFile_h, DATA_COMPGEN(0x00516dc4, normalDialogStripIcn, "strip.icn"));
            resourceFrame_g = NORMAL_DIALOG_MONSTER_BACKGROUND_FRAME;
        } else if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_SECONDARY_SKILL) {
            sprintf(
                resourceText_e[resourceSlot_n],
                DATA_COMPGEN(0x00516de8, normalDialogS5, "%s"),
                gSecondarySkills
                    [resourceValue_l[resourceSlot_n] / SECONDARY_SKILL_VALUE_LEVEL_COUNT]
            );
            strcpy(iconFile_h, DATA_COMPGEN(0x00516dd4, normalDialogSecskillIcn, "secskill.icn"));
            resourceFrame_g =
                resourceValue_l[resourceSlot_n] / SECONDARY_SKILL_VALUE_LEVEL_COUNT
                + NORMAL_DIALOG_SECONDARY_BACKGROUND_FRAME_BASE;
        } else if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_HERO) {
            sprintf(resourceText_e[resourceSlot_n], DATA_COMPGEN(0x00516ec4, normalDialogS6, "%s"), DATA_COMPGEN(0x00516dfc, normalDialogEmptyString5, ""));
            sprintf(iconFile_h, DATA_COMPGEN(0x00516dec, normalDialogSurrendrIcn, "surrendr.icn"));
            resourceFrame_g = NORMAL_DIALOG_HERO_BACKGROUND_FRAME;
        } else if (resourceType_l[resourceSlot_n] >= NORMAL_DIALOG_EXPMRL_FIRST
                   && resourceType_l[resourceSlot_n] <= NORMAL_DIALOG_EXPMRL_LAST) {
            strcpy(resourceText_e[resourceSlot_n], DATA_COMPGEN(0x00516e10, normalDialogEmptyString6, ""));
            strcpy(iconFile_h, DATA_COMPGEN(0x00516e00, normalDialogExpmrlIcn, "expmrl.icn"));
            resourceFrame_g = resourceType_l[resourceSlot_n] - NORMAL_DIALOG_EXPMRL_FIRST;
            if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_EXPMRL_LAST
                && resourceValue_l[resourceSlot_n] != NORMAL_DIALOG_NO_VALUE) {
                sprintf(resourceText_e[resourceSlot_n], DATA_COMPGEN(0x00516d64, normalDialogD2, "%d"), resourceValue_l[resourceSlot_n]);
            }
        } else {
            strcpy(resourceText_e[resourceSlot_n], DATA_COMPGEN(0x00501260, normalDialogEmptyString7, ""));
            strcpy(iconFile_h, DATA_COMPGEN(0x00516d70, normalDialogSecondaryResourceIcn, "resource.icn"));
            resourceFrame_g = resourceType_l[resourceSlot_n];
        }

        switch (resourceType_l[resourceSlot_n]) {
            case NORMAL_DIALOG_RESOURCE_WOOD:
            case NORMAL_DIALOG_RESOURCE_MERCURY:
            case NORMAL_DIALOG_RESOURCE_ORE:
            case NORMAL_DIALOG_RESOURCE_SULFUR:
            case NORMAL_DIALOG_RESOURCE_CRYSTAL:
            case NORMAL_DIALOG_RESOURCE_GEMS:
                resourceImageWidth_g = NORMAL_DIALOG_RESOURCE_ICON_WIDTH;
                sizingIconHeight_l = NORMAL_DIALOG_RESOURCE_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_RESOURCE_GOLD:
                resourceImageWidth_g = NORMAL_DIALOG_LARGE_ICON_WIDTH;
                sizingIconHeight_l = NORMAL_DIALOG_GOLD_LAYOUT_HEIGHT;
                break;
            case NORMAL_DIALOG_ARTIFACT:
                resourceImageWidth_g = NORMAL_DIALOG_LARGE_ICON_WIDTH;
                sizingIconHeight_l = NORMAL_DIALOG_ARTIFACT_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_SPELL:
                resourceImageWidth_g = NORMAL_DIALOG_SPELL_ICON_WIDTH;
                sizingIconHeight_l = NORMAL_DIALOG_SPELL_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_CREST:
                resourceImageWidth_g = NORMAL_DIALOG_CREST_ICON_WIDTH;
                sizingIconHeight_l = NORMAL_DIALOG_CREST_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_LUCK_BONUS:
                resourceImageWidth_g = NORMAL_DIALOG_EXPMRL_ICON_WIDTH;
                sizingIconHeight_l = NORMAL_DIALOG_LUCK_BONUS_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_LUCK_PENALTY:
                resourceImageWidth_g = NORMAL_DIALOG_EXPMRL_ICON_WIDTH;
                sizingIconHeight_l = NORMAL_DIALOG_LUCK_PENALTY_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_MORALE_BONUS:
                resourceImageWidth_g = NORMAL_DIALOG_EXPMRL_ICON_WIDTH;
                sizingIconHeight_l = NORMAL_DIALOG_MORALE_BONUS_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_MORALE_PENALTY:
                resourceImageWidth_g = NORMAL_DIALOG_EXPMRL_ICON_WIDTH;
                sizingIconHeight_l = NORMAL_DIALOG_MORALE_PENALTY_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_EXPERIENCE:
                resourceImageWidth_g = NORMAL_DIALOG_EXPMRL_ICON_WIDTH;
                sizingIconHeight_l = NORMAL_DIALOG_EXPERIENCE_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_HERO:
                resourceImageWidth_g = NORMAL_DIALOG_HERO_BACKGROUND_WIDTH;
                sizingIconHeight_l = NORMAL_DIALOG_HERO_BACKGROUND_HEIGHT;
                break;
            case NORMAL_DIALOG_SECONDARY_SKILL:
                resourceImageWidth_g = NORMAL_DIALOG_SECONDARY_SKILL_BACKGROUND_WIDTH;
                sizingIconHeight_l = NORMAL_DIALOG_SECONDARY_SKILL_ICON_HEIGHT;
                break;
            case NORMAL_DIALOG_MONSTER:
            case NORMAL_DIALOG_PRIMARY_SKILL:
                resourceImageWidth_g = NORMAL_DIALOG_PRIMARY_MONSTER_BACKGROUND_WIDTH;
                sizingIconHeight_l = NORMAL_DIALOG_PRIMARY_MONSTER_ICON_HEIGHT;
                break;
        }

        imageHeight_b = sizingIconHeight_l;
        if (strlen(resourceText_e[resourceSlot_n]))
            sizingIconHeight_l += NORMAL_DIALOG_RESOURCE_LABEL_HEIGHT;

        if (resourceSlot_n == 0) {
            if (resourceType_l[1] == NORMAL_DIALOG_NO_RESOURCE)
                resourceCenterX_a = NormalDialogCenterOffset(
                                        windowWidth_a
                                        - NORMAL_DIALOG_SINGLE_RESOURCE_CENTER_INSET
                                    )
                                    + NORMAL_DIALOG_SINGLE_RESOURCE_CENTER_INSET;
            else
                resourceCenterX_a = NORMAL_DIALOG_FIRST_RESOURCE_CENTER_X;
        } else {
            resourceCenterX_a = windowWidth_a - NORMAL_DIALOG_SECOND_RESOURCE_RIGHT_INSET;
        }
        resourceY_l =
            windowHeight_k - sizingIconHeight_l - NORMAL_DIALOG_RESOURCE_BOTTOM_INSET;
        if (dialogType != NORMAL_DIALOG_QUICK_VIEW)
            resourceY_l -= NORMAL_DIALOG_BUTTON_AREA_HEIGHT;
        if (resourceType_l[0] == NORMAL_DIALOG_SECONDARY_SKILL
            && resourceType_l[1] == NORMAL_DIALOG_SECONDARY_SKILL) {
            if (resourceSlot_n == 0)
                resourceCenterX_a -= NORMAL_DIALOG_SECONDARY_PAIR_SPACING;
            else
                resourceCenterX_a += NORMAL_DIALOG_SECONDARY_PAIR_SPACING;
        }

        iconPanel_j = new iconWidget(
            resourceCenterX_a - NormalDialogCenterOffset(resourceImageWidth_g)
                + (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_SPELL)
                    * NORMAL_DIALOG_SPELL_BACKGROUND_X_OFFSET,
            resourceY_l,
            resourceImageWidth_g,
            imageHeight_b,
            iconFile_h,
            resourceFrame_g,
            ICON_DRAW_NORMAL,
            -1,
            resourceType_l[resourceSlot_n] == NORMAL_DIALOG_SPELL
                ? WIDGET_KIND_ICON_CENTERED
                : WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (!iconPanel_j)
            MemError();
        pNormalDialogWindow->AddWidget(iconPanel_j, -1);

        if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_ARTIFACT) {
            iconPanel_j = new iconWidget(
                resourceCenterX_a - NormalDialogCenterOffset(resourceImageWidth_g)
                    + NORMAL_DIALOG_ICON_OVERLAY_INSET,
                resourceY_l + NORMAL_DIALOG_ICON_OVERLAY_INSET,
                NORMAL_DIALOG_LARGE_ICON_WIDTH,
                NORMAL_DIALOG_ARTIFACT_ICON_HEIGHT,
                DATA_COMPGEN(0x00516e24, normalDialogArtifactIcn, "artifact.icn"),
                resourceValue_l[resourceSlot_n] + NORMAL_DIALOG_ARTIFACT_FRAME_OFFSET,
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (!iconPanel_j)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_j, -1);
        }
        if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_PRIMARY_SKILL) {
            iconPanel_j = new iconWidget(
                resourceCenterX_a - NormalDialogCenterOffset(resourceImageWidth_g)
                    + NORMAL_DIALOG_ICON_OVERLAY_INSET,
                resourceY_l + NORMAL_DIALOG_ICON_OVERLAY_INSET,
                NORMAL_DIALOG_PRIMARY_MONSTER_OVERLAY_WIDTH,
                NORMAL_DIALOG_PRIMARY_MONSTER_OVERLAY_HEIGHT,
                DATA_COMPGEN(0x00516e34, normalDialogPrimskilIcn2, "primskil.icn"),
                resourceValue_l[resourceSlot_n],
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (!iconPanel_j)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_j, -1);
            strcpy(resourceText_e[resourceSlot_n], gStatNames[resourceValue_l[resourceSlot_n]]);
        }
        if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_MONSTER) {
            iconPanel_j = new iconWidget(
                resourceCenterX_a - NormalDialogCenterOffset(resourceImageWidth_g)
                    + NORMAL_DIALOG_ICON_OVERLAY_INSET,
                resourceY_l + NORMAL_DIALOG_ICON_OVERLAY_INSET,
                NORMAL_DIALOG_PRIMARY_MONSTER_OVERLAY_WIDTH,
                NORMAL_DIALOG_PRIMARY_MONSTER_OVERLAY_HEIGHT,
                DATA_COMPGEN(0x00516e44, normalDialogStripIcn2, "strip.icn"),
                IDX(gMonsterDatabase[resourceValue_l[resourceSlot_n]].race)
                    + NORMAL_DIALOG_MONSTER_RACE_FRAME_OFFSET,
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (!iconPanel_j)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_j, -1);

            sprintf(gText, DATA_COMPGEN(0x00516e50, normalDialogMonh04dIcn, "monh%04d.icn"), resourceValue_l[resourceSlot_n]);
            iconPanel_j = new iconWidget(
                resourceCenterX_a - NormalDialogCenterOffset(resourceImageWidth_g)
                    + NORMAL_DIALOG_ICON_OVERLAY_INSET,
                resourceY_l + NORMAL_DIALOG_ICON_OVERLAY_INSET,
                NORMAL_DIALOG_PRIMARY_MONSTER_OVERLAY_WIDTH,
                NORMAL_DIALOG_PRIMARY_MONSTER_OVERLAY_HEIGHT,
                gText,
                0,
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (!iconPanel_j)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_j, -1);
        }
        if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_CREST) {
            iconPanel_j = new iconWidget(
                resourceCenterX_a - NormalDialogCenterOffset(resourceImageWidth_g)
                    - NORMAL_DIALOG_CREST_OVERLAY_OUTSET,
                resourceY_l - NORMAL_DIALOG_CREST_OVERLAY_OUTSET,
                NORMAL_DIALOG_CREST_OVERLAY_WIDTH,
                NORMAL_DIALOG_CREST_OVERLAY_HEIGHT,
                DATA_COMPGEN(0x00516e60, normalDialogBrcrestIcn2, "brcrest.icn"),
                NORMAL_DIALOG_CREST_OVERLAY_FRAME,
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (!iconPanel_j)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_j, -1);
        }
        if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_SECONDARY_SKILL) {
            iconPanel_j = new iconWidget(
                resourceCenterX_a - NormalDialogCenterOffset(resourceImageWidth_g)
                    - NORMAL_DIALOG_SECONDARY_SKILL_OVERLAY_OUTSET,
                resourceY_l - NORMAL_DIALOG_SECONDARY_SKILL_OVERLAY_OUTSET,
                NORMAL_DIALOG_SECONDARY_SKILL_OVERLAY_WIDTH,
                NORMAL_DIALOG_SECONDARY_SKILL_OVERLAY_HEIGHT,
                DATA_COMPGEN(0x00516e6c, normalDialogSecskillIcn2, "secskill.icn"),
                NORMAL_DIALOG_SECONDARY_OVERLAY_FRAME,
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (!iconPanel_j)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_j, -1);
        }
        if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_HERO) {
            sprintf(iconFile_h, DATA_COMPGEN(0x00516e7c, normalDialogPort04dIcn, "port%04d.icn"), resourceValue_l[resourceSlot_n]);
            iconPanel_j = new iconWidget(
                resourceCenterX_a - NormalDialogCenterOffset(resourceImageWidth_g)
                    + NORMAL_DIALOG_HERO_OVERLAY_INSET,
                resourceY_l + NORMAL_DIALOG_HERO_OVERLAY_INSET,
                NORMAL_DIALOG_HERO_OVERLAY_WIDTH,
                NORMAL_DIALOG_HERO_OVERLAY_HEIGHT,
                iconFile_h,
                0,
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (!iconPanel_j)
                MemError();
            pNormalDialogWindow->AddWidget(iconPanel_j, -1);
        }

        if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_SECONDARY_SKILL) {
            labelY_o = OD_STEER(sizingIconHeight_l) + resourceY_l
                - NORMAL_DIALOG_SECONDARY_NAME_Y_OFFSET;
            textPanel_h = new textWidget(
                resourceCenterX_a - NORMAL_DIALOG_RESOURCE_LABEL_HALF_WIDTH,
                labelY_o,
                NORMAL_DIALOG_RESOURCE_LABEL_WIDTH,
                NORMAL_DIALOG_RESOURCE_LABEL_HEIGHT,
                resourceText_e[resourceSlot_n],
                DATA_COMPGEN(0x00516e8c, normalDialogSmalfontFnt, "smalfont.fnt"),
                FONT_DRAW_DEFAULT,
                textWidgetId_h++,
                WIDGET_KIND_TEXT,
                FONT_ALIGN_CENTER
            );
            if (!textPanel_h)
                MemError();
            pNormalDialogWindow->AddWidget(textPanel_h, -1);

            resourceText_e[resourceSlot_n] = static_cast<char*>(H2_ALLOC_AT(
                NORMAL_DIALOG_TEXT_LENGTH, DATA_COMPGEN(0x00516e9c, normalDialogSourceFile2, RETAIL_FILE),
                normalDialogSourceLineBase + 431
            ));
            labelY_o = OD_STEER(sizingIconHeight_l) + resourceY_l
                - NORMAL_DIALOG_SECONDARY_LEVEL_Y_OFFSET;
            sprintf(
                resourceText_e[resourceSlot_n],
                DATA_COMPGEN(0x00509664, normalDialogS7, "%s"),
                gSecondarySkillLevels
                    [resourceValue_l[resourceSlot_n] % SECONDARY_SKILL_VALUE_LEVEL_COUNT]
            );
        } else if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_PRIMARY_SKILL) {
            labelY_o = OD_STEER(sizingIconHeight_l) + resourceY_l
                - NORMAL_DIALOG_PRIMARY_LABEL_Y_OFFSET;
        } else {
            labelY_o = OD_STEER(sizingIconHeight_l) + resourceY_l
                - NORMAL_DIALOG_DEFAULT_LABEL_Y_OFFSET;
        }

        textPanel_h = new textWidget(
            resourceCenterX_a - NORMAL_DIALOG_RESOURCE_LABEL_HALF_WIDTH,
            labelY_o,
            NORMAL_DIALOG_RESOURCE_LABEL_WIDTH,
            resourceType_l[resourceSlot_n] == NORMAL_DIALOG_SPELL
                ? NORMAL_DIALOG_SPELL_LABEL_HEIGHT
                : NORMAL_DIALOG_RESOURCE_LABEL_HEIGHT,
            resourceText_e[resourceSlot_n],
            DATA_COMPGEN(0x00516ec8, normalDialogSmalfontFnt2, "smalfont.fnt"),
            FONT_DRAW_DEFAULT,
            textWidgetId_h++,
            WIDGET_KIND_TEXT,
            FONT_ALIGN_CENTER
        );
        if (!textPanel_h)
            MemError();
        pNormalDialogWindow->AddWidget(textPanel_h, -1);

        if (resourceType_l[resourceSlot_n] == NORMAL_DIALOG_PRIMARY_SKILL && showPrimaryBonus_e) {
            char* bonusText = static_cast<char*>(H2_ALLOC_AT(
                NORMAL_DIALOG_PRIMARY_BONUS_TEXT_LENGTH, DATA_COMPGEN(0x00516ed8, normalDialogSourceFile3, RETAIL_FILE),
                normalDialogSourceLineBase + 457
            ));
            strcpy(bonusText, DATA_COMPGEN(0x00516f00, normalDialog1, "+1 "));
            textPanel_h = new textWidget(
                resourceCenterX_a - NORMAL_DIALOG_RESOURCE_LABEL_HALF_WIDTH,
                OD_STEER(sizingIconHeight_l) + resourceY_l
                    - NORMAL_DIALOG_PRIMARY_BONUS_LABEL_Y_OFFSET,
                NORMAL_DIALOG_RESOURCE_LABEL_WIDTH,
                NORMAL_DIALOG_PRIMARY_BONUS_TEXT_HEIGHT,
                bonusText,
                DATA_COMPGEN(0x00516f04, normalDialogBigfontFnt, "bigfont.fnt"),
                FONT_DRAW_DEFAULT,
                textWidgetId_h++,
                WIDGET_KIND_TEXT,
                FONT_ALIGN_CENTER
            );
            if (!textPanel_h)
                MemError();
            pNormalDialogWindow->AddWidget(textPanel_h, -1);
        }

        borderWidget_o = new border(
            resourceCenterX_a - NormalDialogCenterOffset(resourceImageWidth_g),
            resourceY_l,
            resourceImageWidth_g,
            sizingIconHeight_l,
            resourceSlot_n + NORMAL_DIALOG_RESOURCE_BORDER_FIRST_ID,
            WIDGET_KIND_TRANSPARENT,
            0,
            NULL
        );
        pNormalDialogWindow->AddWidget(borderWidget_o, -1);
    }

    message_e.type = NORMAL_DIALOG_DISABLE_MESSAGE;
    message_e.payload.widget.command = NORMAL_DIALOG_SET_TEXT_COMMAND;
    message_e.payload.widget.id = NORMAL_DIALOG_TEXT_WIDGET_ID;
    message_e.payload.widget.data.text = text;
    pNormalDialogWindow->BroadcastMessage(message_e);

    if (showOrText == NORMAL_DIALOG_SHOW_OR_TEXT) {
        orText_f = static_cast<char*>(H2_ALLOC_AT(
            NORMAL_DIALOG_OR_TEXT_LENGTH, DATA_COMPGEN(0x00516f10, normalDialogSourceFile4, RETAIL_FILE),
            normalDialogSourceLineBase + 493
        ));
        strcpy(orText_f, DATA_COMPGEN(0x00516f38, normalDialogOr, "or"));
        textPanel_h = new textWidget(
            NormalDialogCenterOffset(windowWidth_a) - NORMAL_DIALOG_OR_TEXT_CENTER_X_OFFSET,
            resourceY_l + NORMAL_DIALOG_OR_TEXT_Y_OFFSET,
            NORMAL_DIALOG_OR_TEXT_WIDTH,
            NORMAL_DIALOG_OR_TEXT_HEIGHT,
            orText_f,
            DATA_COMPGEN(0x00516f3c, normalDialogSecondarySmalfontFnt, "smalfont.fnt"),
            FONT_DRAW_DEFAULT,
            textWidgetId_h++,
            WIDGET_KIND_TEXT,
            FONT_ALIGN_CENTER
        );
        if (!textPanel_h)
            MemError();
        pNormalDialogWindow->AddWidget(textPanel_h, -1);
    }

    savedPointerType_e = gpMouseManager->m_cursorType;
    savedPointerFrame_j = gpMouseManager->m_cursorFrame;
    while (gpMouseManager->m_hideCount)
        gpMouseManager->ShowColorPointer();
    gpMouseManager->SetPointer(DATA_COMPGEN(0x00516f4c, normalDialogAdvmiceMse, "advmice.mse"), 0, MOUSE_AUTO_CURSOR_TYPE);

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
    gpMouseManager->SetPointer(DATA_COMPGEN(0x00501274, normalDialogEmptyString8, ""), savedPointerFrame_j, savedPointerType_e);
    giResType1 = savedFirstResourceType_p;
    giResExtra1 = savedFirstResourceValue_i;
    giResType2 = savedSecondResourceType_f;
    giResExtra2 = savedSecondResourceValue_j;
    pNormalDialogWindow = savedNormalDialogWindow_o;
}

VA(0x004a2565, 0x71)
void UpdateNormalDialog(char* text) {
    i16 show = 1;
    tag_message evt;
    evt.type = MESSAGE_WIDGET;
    evt.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    evt.payload.widget.id = 1;
    evt.payload.widget.data.text = text;
    pNormalDialogWindow->BroadcastMessage(evt);
    pNormalDialogWindow->DrawWindow(0, 0, NORMAL_DIALOG_FOREGROUND_WIDGET_LIMIT);
    pNormalDialogWindow->DrawWindow(
        1,
        WINDOW_ALL_WIDGETS_LOW,
        NORMAL_DIALOG_BACKGROUND_WIDGET_LAST_ID
    );
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

DATA(0x004f8c58) H2_ENUM_STORAGE(TerrainType, u8)
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
DATA(0x004f8e08) u8 giGroundShape[GROUND_TILE_IMAGE_COUNT] = {
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

// These retail palette and lookup tables are byte-exact data payloads; their entries
// are values rather than individually named constants.
// NOLINTBEGIN(readability-magic-numbers)
DATA(0x004f8fb8) u8 gColorTableTan[DIM_PALETTE_COLOR_COUNT] = {
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
DATA(0x004f90b8) u8 gColorTableGray[DIM_PALETTE_COLOR_COUNT] = {
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
DATA(0x004f91b8) u8 gColorTableYellow[DIM_PALETTE_COLOR_COUNT] = {
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
DATA(0x004f92b8) u8 gColorTableScenWin[DIM_PALETTE_COLOR_COUNT] = {
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
DATA(0x004f93b8) u8 gColorTableDarkGray[DIM_PALETTE_COLOR_COUNT] = {
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
DATA(0x004f94b8) u8 gColorTableRed[DIM_PALETTE_COLOR_COUNT] = {
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
DATA(0x004f95b8) u8 gColorTableDarkBrown[DIM_PALETTE_COLOR_COUNT] = {
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
DATA(0x004f96b8) i32 MAP_WIDTH = MAP_DIMENSION_MEDIUM;
DATA(0x004f96bc) i32 MAP_HEIGHT = MAP_DIMENSION_MEDIUM;
DATA(0x004f96c0) u8* mapExtra = NULL;
DATA(0x004f96c4) b32 gbClosingApp = false;
DATA(0x004f96c8) b32 gbForegroundApp = false;
DATA(0x004f96cc) i32 giMainVideoModeColorDepth = WINGRAPH_COLOR_DEPTH;
DATA(0x004f96d0) i32 giMainVideoModeWidth = WINGRAPH_WIDTH;
DATA(0x004f96d4) i32 giMainVideoModeHeight = WINGRAPH_HEIGHT;
DATA(0x004f96d8) u32l glMilliCounter = 0;
DATA(0x004f96e0) u8 gMapColors[RADAR_MAP_COLOR_COUNT] =
    {77, 98, 13, 104, 32, 118, 54, 206, 41, 0, 0, 0, 0, 0, 0, 0};
DATA(0x004f96f0) u8 gObjectColors[RADAR_OBJECT_COLOR_COUNT] =
    {16, 48, 98, 160, 126, 74, 110, 179, 100, 218, 12, 12, 12, 12, 12, 12};
DATA(0x004f9700) u8 gOwnerColors[RADAR_OWNER_COLOR_COUNT] = {73, 105, 190, 114, 205, 138, 10, 0};
DATA(0x004f9708) char* gTilesetFiles[IDX(TILESET_COUNT)] = {
    DATA_COMPGEN(0x00501288, gTilesetFilesEmptyString, ""),
    DATA_COMPGEN(0x005012bc, gTilesetFilesEmptyString2, ""),
    DATA_COMPGEN(0x005012d0, gTilesetFilesEmptyString3, ""),
    DATA_COMPGEN(0x00503f60, gTilesetFilesEmptyString4, ""),
    DATA_COMPGEN(0x00503f64, gTilesetFilesEmptyString5, ""),
    DATA_COMPGEN(0x00503f68, gTilesetFilesEmptyString6, ""),
    DATA_COMPGEN(0x00503f6c, gTilesetFilesEmptyString7, ""),
    DATA_COMPGEN(0x00503f70, gTilesetFilesEmptyString8, ""),
    DATA_COMPGEN(0x00503f74, gTilesetFilesEmptyString9, ""),
    DATA_COMPGEN(0x00503f78, gTilesetFilesEmptyString10, ""),
    DATA_COMPGEN(0x005001a0, gTilesetFilesObjnhaunIcn, "objnhaun.icn"),
    DATA_COMPGEN(0x005001b0, gTilesetFilesObjnartiIcn, "objnarti.icn"),
    DATA_COMPGEN(0x005001c0, gTilesetFilesMons32Icn, "mons32.icn"),
    DATA_COMPGEN(0x005001cc, gTilesetFilesArt32Icn, "art32.icn"),
    DATA_COMPGEN(0x005001d8, gTilesetFilesFlag32Icn, "flag32.icn"),
    DATA_COMPGEN(0x005001e4, gTilesetFilesRessmallIcn, "ressmall.icn"),
    DATA_COMPGEN(0x005001f4, gTilesetFilesHourglasIcn, "hourglas.icn"),
    DATA_COMPGEN(0x00500204, gTilesetFilesRouteIcn, "route.icn"),
    DATA_COMPGEN(0x00503f7c, gTilesetFilesEmptyString11, ""),
    DATA_COMPGEN(0x00500214, gTilesetFilesStonbackIcn, "stonback.icn"),
    DATA_COMPGEN(0x00500224, gTilesetFilesMinimonIcn, "minimon.icn"),
    DATA_COMPGEN(0x00500230, gTilesetFilesMiniheroIcn, "minihero.icn"),
    DATA_COMPGEN(0x00500240, gTilesetFilesMtnsnowIcn, "mtnsnow.icn"),
    DATA_COMPGEN(0x0050024c, gTilesetFilesMtnswmpIcn, "mtnswmp.icn"),
    DATA_COMPGEN(0x00500258, gTilesetFilesMtnlavaIcn, "mtnlava.icn"),
    DATA_COMPGEN(0x00500264, gTilesetFilesMtndsrtIcn, "mtndsrt.icn"),
    DATA_COMPGEN(0x00500270, gTilesetFilesMtndirtIcn, "mtndirt.icn"),
    DATA_COMPGEN(0x0050027c, gTilesetFilesMtnmultIcn, "mtnmult.icn"),
    DATA_COMPGEN(0x0050747c, gTilesetFilesEmptyString12, ""),
    DATA_COMPGEN(0x0050028c, gTilesetFilesExtraovrIcn, "extraovr.icn"),
    DATA_COMPGEN(0x0050029c, gTilesetFilesRoadIcn, "road.icn"),
    DATA_COMPGEN(0x005002a8, gTilesetFilesMtncrckIcn, "mtncrck.icn"),
    DATA_COMPGEN(0x005002b4, gTilesetFilesMtngrasIcn, "mtngras.icn"),
    DATA_COMPGEN(0x005002c0, gTilesetFilesTrejnglIcn, "trejngl.icn"),
    DATA_COMPGEN(0x005002cc, gTilesetFilesTreevilIcn, "treevil.icn"),
    DATA_COMPGEN(0x005002d8, gTilesetFilesObjntownIcn, "objntown.icn"),
    DATA_COMPGEN(0x005002e8, gTilesetFilesObjntwbaIcn, "objntwba.icn"),
    DATA_COMPGEN(0x005002f8, gTilesetFilesObjntwshIcn, "objntwsh.icn"),
    DATA_COMPGEN(0x00500308, gTilesetFilesObjntwrdIcn, "objntwrd.icn"),
    DATA_COMPGEN(0x00500318, gTilesetFilesObjnxtraIcn, "objnxtra.icn"),
    DATA_COMPGEN(0x00500328, gTilesetFilesObjnwat2Icn, "objnwat2.icn"),
    DATA_COMPGEN(0x00500338, gTilesetFilesObjnmul2Icn, "objnmul2.icn"),
    DATA_COMPGEN(0x00500348, gTilesetFilesTresnowIcn, "tresnow.icn"),
    DATA_COMPGEN(0x00500354, gTilesetFilesTrefirIcn, "trefir.icn"),
    DATA_COMPGEN(0x00500360, gTilesetFilesTrefallIcn, "trefall.icn"),
    DATA_COMPGEN(0x0050036c, gTilesetFilesStreamIcn, "stream.icn"),
    DATA_COMPGEN(0x00500378, gTilesetFilesObjnrsrcIcn, "objnrsrc.icn"),
    DATA_COMPGEN(0x00500388, gTilesetFilesDummyIcn, "dummy.icn"),
    DATA_COMPGEN(0x00500394, gTilesetFilesObjngra2Icn, "objngra2.icn"),
    DATA_COMPGEN(0x005003a4, gTilesetFilesTredeciIcn, "tredeci.icn"),
    DATA_COMPGEN(0x005003b0, gTilesetFilesObjnwatrIcn, "objnwatr.icn"),
    DATA_COMPGEN(0x005003c0, gTilesetFilesObjngrasIcn, "objngras.icn"),
    DATA_COMPGEN(0x005003d0, gTilesetFilesObjnsnowIcn, "objnsnow.icn"),
    DATA_COMPGEN(0x005003e0, gTilesetFilesObjnswmpIcn, "objnswmp.icn"),
    DATA_COMPGEN(0x005003f0, gTilesetFilesObjnlavaIcn, "objnlava.icn"),
    DATA_COMPGEN(0x00500400, gTilesetFilesObjndsrtIcn, "objndsrt.icn"),
    DATA_COMPGEN(0x00500410, gTilesetFilesObjndirtIcn, "objndirt.icn"),
    DATA_COMPGEN(0x00500420, gTilesetFilesObjncrckIcn, "objncrck.icn"),
    DATA_COMPGEN(0x00500430, gTilesetFilesObjnlav3Icn, "objnlav3.icn"),
    DATA_COMPGEN(0x00500440, gTilesetFilesObjnmultIcn, "objnmult.icn"),
    DATA_COMPGEN(0x00500450, gTilesetFilesObjnlav2Icn, "objnlav2.icn"),
    DATA_COMPGEN(0x00500460, gTilesetFilesXLoc1Icn, "x_loc1.icn"),
    DATA_COMPGEN(0x0050046c, gTilesetFilesXLoc2Icn, "x_loc2.icn"),
    DATA_COMPGEN(0x00500478, gTilesetFilesXLoc3Icn, "x_loc3.icn")
};
DATA(0x004f9808) u8 bPuzzleDraw[PUZZLE_DRAW_TABLE_COUNT] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};
DATA(0x004f9848) u8
    uDimPal[DIM_PALETTE_SET_COUNT][DIM_PALETTE_LEVEL_COUNT][DIM_PALETTE_COLOR_COUNT] = {
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
DATA(0x004fa448) u8 gColorTableLighten[DIM_PALETTE_COLOR_COUNT] = {
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
DATA(0x004fa548) u8 gColorTableNoCycle[DIM_PALETTE_COLOR_COUNT] = {
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
DATA(0x004fa648) font* smallFont = NULL;
DATA(0x004fa64c) font* bigFont = NULL;
DATA(0x004fa650) b32 gbReturnAfterComputeExtent = false;
DATA(0x004fa654) b32 gbAllowTextEntryEscape = true;
DATA(0x004fa658) WindowColorCycleMode giCycleType = WINDOW_COLOR_CYCLE_DEFAULT;
DATA(0x004fa65c) i32 giScreenScroll = 1;
DATA(0x004fa660) i32 giMenuCommand = -1;
DATA(0x004fa664) b32 gbSendMouseMoveMessages = false;
DATA(0x004fa668) b32 gbColorMice = true;
DATA(0x004fa670) u32l gTownEligibleBuildMask[TOWN_ELIGIBLE_BUILD_MASK_COUNT] = {
    TOWN_ELIGIBLE_BUILD_KNIGHT_MASK,
    TOWN_ELIGIBLE_BUILD_BARBARIAN_MASK,
    TOWN_ELIGIBLE_BUILD_SORCERESS_MASK,
    TOWN_ELIGIBLE_BUILD_WARLOCK_MASK,
    TOWN_ELIGIBLE_BUILD_WIZARD_MASK,
    TOWN_ELIGIBLE_BUILD_NECROMANCER_MASK
};
DATA(0x004fa688) u8 giMapSizes[KB_MAP_SIZE_COUNT] = {
    MAP_DIMENSION_SMALL,
    MAP_DIMENSION_MEDIUM,
    MAP_DIMENSION_LARGE,
    MAP_DIMENSION_XLARGE
};
DATA(0x004fa68c) b32 gbUseEvilInterface = false;
DATA(0x004fa690) char* cEvilTranslate[KB_INTERFACE_TYPE_COUNT][KB_INTERFACE_VARIANT_COUNT] = {
    {DATA_COMPGEN(0x00500484, cEvilTranslateAdvbordIcn, "advbord.icn"), DATA_COMPGEN(0x00500490, cEvilTranslateAdvbordeIcn, "advborde.icn")},  {DATA_COMPGEN(0x005004a0, cEvilTranslateHeroextgIcn, "heroextg.icn"), DATA_COMPGEN(0x005004b0, cEvilTranslateHeroexteIcn, "heroexte.icn")},
    {DATA_COMPGEN(0x005004c0, cEvilTranslateBuybuildIcn, "buybuild.icn"), DATA_COMPGEN(0x005004d0, cEvilTranslateBuybuileIcn, "buybuile.icn")}, {DATA_COMPGEN(0x005004e0, cEvilTranslateAdvbtnsIcn, "advbtns.icn"), DATA_COMPGEN(0x005004ec, cEvilTranslateAdvebtnsIcn, "advebtns.icn")},
    {DATA_COMPGEN(0x005004fc, cEvilTranslateHerologoIcn, "herologo.icn"), DATA_COMPGEN(0x0050050c, cEvilTranslateHerologeIcn, "herologe.icn")}, {DATA_COMPGEN(0x0050051c, cEvilTranslateSunmoonIcn, "sunmoon.icn"), DATA_COMPGEN(0x00500528, cEvilTranslateSunmooneIcn, "sunmoone.icn")},
    {DATA_COMPGEN(0x00500538, cEvilTranslateStonbackIcn, "stonback.icn"), DATA_COMPGEN(0x00500548, cEvilTranslateStonbakeIcn, "stonbake.icn")}, {DATA_COMPGEN(0x00500558, cEvilTranslateScrollIcn, "scroll.icn"), DATA_COMPGEN(0x00500564, cEvilTranslateScrolleIcn, "scrolle.icn")},
    {DATA_COMPGEN(0x00500570, cEvilTranslateLocatorsIcn, "locators.icn"), DATA_COMPGEN(0x00500580, cEvilTranslateLocatoreIcn, "locatore.icn")}, {DATA_COMPGEN(0x00500590, cEvilTranslateSystemIcn, "system.icn"), DATA_COMPGEN(0x0050059c, cEvilTranslateSystemeIcn, "systeme.icn")},
    {DATA_COMPGEN(0x005005a8, cEvilTranslateCPANBKGICN, "CPANBKG.ICN"), DATA_COMPGEN(0x005005b4, cEvilTranslateCPANBKGEICN, "CPANBKGE.ICN")},  {DATA_COMPGEN(0x005005c4, cEvilTranslateCPANELICN, "CPANEL.ICN"), DATA_COMPGEN(0x005005d0, cEvilTranslateCPANELEICN, "CPANELE.ICN")},
    {DATA_COMPGEN(0x005005dc, cEvilTranslateAPANBKGICN, "APANBKG.ICN"), DATA_COMPGEN(0x005005e8, cEvilTranslateAPANBKGEICN, "APANBKGE.ICN")},  {DATA_COMPGEN(0x005005f8, cEvilTranslateAPANELICN, "APANEL.ICN"), DATA_COMPGEN(0x00500604, cEvilTranslateAPANELEICN, "APANELE.ICN")},
    {DATA_COMPGEN(0x00500610, cEvilTranslateVIEWWRLDICN, "VIEWWRLD.ICN"), DATA_COMPGEN(0x00500620, cEvilTranslateEVIWWRLDICN, "EVIWWRLD.ICN")}, {DATA_COMPGEN(0x00500630, cEvilTranslateVIEWRSRCICN, "VIEWRSRC.ICN"), DATA_COMPGEN(0x00500640, cEvilTranslateEVIWRSRCICN, "EVIWRSRC.ICN")},
    {DATA_COMPGEN(0x00500650, cEvilTranslateVIEWRTFXICN, "VIEWRTFX.ICN"), DATA_COMPGEN(0x00500660, cEvilTranslateEVIWRTFXICN, "EVIWRTFX.ICN")}, {DATA_COMPGEN(0x00500670, cEvilTranslateVIEWTWNSICN, "VIEWTWNS.ICN"), DATA_COMPGEN(0x00500680, cEvilTranslateEVIWTWNSICN, "EVIWTWNS.ICN")},
    {DATA_COMPGEN(0x00500690, cEvilTranslateVIEWHROSICN, "VIEWHROS.ICN"), DATA_COMPGEN(0x005006a0, cEvilTranslateEVIWHROSICN, "EVIWHROS.ICN")}, {DATA_COMPGEN(0x005006b0, cEvilTranslateVIEWALLICN, "VIEW_ALL.ICN"), DATA_COMPGEN(0x005006c0, cEvilTranslateEVIWALLICN, "EVIW_ALL.ICN")},
    {DATA_COMPGEN(0x005006d0, cEvilTranslateVIEWMINEICN, "VIEWMINE.ICN"), DATA_COMPGEN(0x005006e0, cEvilTranslateEVIWMINEICN, "EVIWMINE.ICN")}, {DATA_COMPGEN(0x005006f0, cEvilTranslateVIEWDDORICN, "VIEWDDOR.ICN"), DATA_COMPGEN(0x00500700, cEvilTranslateEVIWDDORICN, "EVIWDDOR.ICN")},
    {DATA_COMPGEN(0x00500710, cEvilTranslateVIEWPUZLICN, "VIEWPUZL.ICN"), DATA_COMPGEN(0x00500720, cEvilTranslateEVIWPUZLICN, "EVIWPUZL.ICN")}, {DATA_COMPGEN(0x00500730, cEvilTranslateLGNDXTRAICN, "LGNDXTRA.ICN"), DATA_COMPGEN(0x00500740, cEvilTranslateLGNDXTREICN, "LGNDXTRE.ICN")},
    {DATA_COMPGEN(0x00500750, cEvilTranslateSPANBKGICN, "SPANBKG.ICN"), DATA_COMPGEN(0x0050075c, cEvilTranslateSPANBKGEICN, "SPANBKGE.ICN")},  {DATA_COMPGEN(0x0050076c, cEvilTranslateSPANBTNICN, "SPANBTN.ICN"), DATA_COMPGEN(0x00500778, cEvilTranslateSPANBTNEICN, "SPANBTNE.ICN")},
    {DATA_COMPGEN(0x00500788, cEvilTranslateCSPANBKGICN, "CSPANBKG.ICN"), DATA_COMPGEN(0x00500798, cEvilTranslateCSPANBKEICN, "CSPANBKE.ICN")}, {DATA_COMPGEN(0x005007a8, cEvilTranslateCSPANBTNICN, "CSPANBTN.ICN"), DATA_COMPGEN(0x005007b8, cEvilTranslateCSPANBTEICN, "CSPANBTE.ICN")},
    {DATA_COMPGEN(0x005007c8, cEvilTranslateTRADPOSTICN, "TRADPOST.ICN"), DATA_COMPGEN(0x005007d8, cEvilTranslateTRADPOSEICN, "TRADPOSE.ICN")}, {DATA_COMPGEN(0x005007e8, cEvilTranslateVIEWARMYICN, "VIEWARMY.ICN"), DATA_COMPGEN(0x005007f8, cEvilTranslateVIEWARMEICN, "VIEWARME.ICN")},
    {DATA_COMPGEN(0x00500808, cEvilTranslateWINLOSEICN, "WINLOSE.ICN"), DATA_COMPGEN(0x00500814, cEvilTranslateWINLOSEEICN, "WINLOSEE.ICN")},  {DATA_COMPGEN(0x00500824, cEvilTranslateWINCMBTBICN, "WINCMBTB.ICN"), DATA_COMPGEN(0x00500834, cEvilTranslateWINCMBBEICN, "WINCMBBE.ICN")},
    {DATA_COMPGEN(0x00500844, cEvilTranslateSURRENDRICN, "SURRENDR.ICN"), DATA_COMPGEN(0x00500854, cEvilTranslateSURRENDEICN, "SURRENDE.ICN")}, {DATA_COMPGEN(0x00500864, cEvilTranslateSURDRBKGICN, "SURDRBKG.ICN"), DATA_COMPGEN(0x00500874, cEvilTranslateSURDRBKEICN, "SURDRBKE.ICN")},
    {DATA_COMPGEN(0x00500884, cEvilTranslateVGENBKGICN, "VGENBKG.ICN"), DATA_COMPGEN(0x00500890, cEvilTranslateVGENBKGEICN, "VGENBKGE.ICN")},  {DATA_COMPGEN(0x005008a0, cEvilTranslateCampbkggICN, "campbkgg.ICN"), DATA_COMPGEN(0x005008b0, cEvilTranslateCampbkgeICN, "campbkge.ICN")},
    {DATA_COMPGEN(0x005008c0, cEvilTranslateCampxtrgICN, "campxtrg.ICN"), DATA_COMPGEN(0x005008d0, cEvilTranslateCampxtreICN, "campxtre.ICN")}
};
DATA(0x004fa7b8) char gcAnimPath[GLOBAL_AGGREGATE_PATH_SIZE] = "\\HEROES2\\ANIM\\";
DATA(0x004fa918) char gcGamePath[GLOBAL_GAME_PATH_SIZE] = ".\\GAMES\\";
DATA(0x004fa930) char gcMapPath[GLOBAL_MAP_PATH_SIZE] = ".\\MAPS\\";
DATA(0x004fa944) b32 gbPutzingWithMouseCtr = false;
DATA(0x004fa948) b32 gbDontTryRedbook = false;
DATA(0x004fa94c) b32 gbDontTryMIDI = false;
DATA(0x004fa950) b32 gbDontTryDigital = false;
DATA(0x004fa958) float gfCombatSpeedMod[KB_COMBAT_SPEED_COUNT] = {1.0f, 0.7f, 0.35f};
DATA(0x004fa964) icon* gShingleAnim = NULL;
DATA(0x004fa968) i32 iNextShingleAnim = 0;
DATA(0x004fa96c) i32 giDialogTimeout = 0;
DATA(0x004fa970) i32 giNewMonsterCycleFrame = 0;
DATA(0x004fa974) b32 gbNoCDRom = false;
DATA(0x004fa978) b32 gbLeaveNetBoxAlone = false;
DATA(0x004fa97c) b32 gbDrawWindowBackground = true;
DATA(0x004fa980) b32 gbCheatMenus = false;
DATA(0x004fa984) b32 gbUseWaveout = false;
DATA(0x004fa988) b32 gbShowAllMaps = false;
DATA(0x004fa990) char* gCombatFxNames[KB_COMBAT_FX_COUNT] = {
    DATA_COMPGEN(0x00509064, gCombatFxNamesEmptyString, ""),
    DATA_COMPGEN(0x005008e4, gCombatFxNamesMagic01Icn, "magic01.icn"),
    DATA_COMPGEN(0x005008f0, gCombatFxNamesMagic02Icn, "magic02.icn"),
    DATA_COMPGEN(0x005008fc, gCombatFxNamesMagic03Icn, "magic03.icn"),
    DATA_COMPGEN(0x00500908, gCombatFxNamesMagic04Icn, "magic04.icn"),
    DATA_COMPGEN(0x00500914, gCombatFxNamesMagic05Icn, "magic05.icn"),
    DATA_COMPGEN(0x00500920, gCombatFxNamesMagic06Icn, "magic06.icn"),
    DATA_COMPGEN(0x0050092c, gCombatFxNamesMagic07Icn, "magic07.icn"),
    DATA_COMPGEN(0x00500938, gCombatFxNamesMagic08Icn, "magic08.icn"),
    DATA_COMPGEN(0x00500944, gCombatFxNamesRainblukIcn, "rainbluk.icn"),
    DATA_COMPGEN(0x00500954, gCombatFxNamesCloudlukIcn, "cloudluk.icn"),
    DATA_COMPGEN(0x00500964, gCombatFxNamesMoralegIcn, "moraleg.icn"),
    DATA_COMPGEN(0x00500970, gCombatFxNamesMoralebIcn, "moraleb.icn"),
    DATA_COMPGEN(0x0050097c, gCombatFxNamesReddeathIcn, "reddeath.icn"),
    DATA_COMPGEN(0x0050098c, gCombatFxNamesRedfireIcn, "redfire.icn"),
    DATA_COMPGEN(0x00500998, gCombatFxNamesSparksIcn, "sparks.icn"),
    DATA_COMPGEN(0x005009a4, gCombatFxNamesElectricIcn, "electric.icn"),
    DATA_COMPGEN(0x005009b4, gCombatFxNamesPhysicalIcn, "physical.icn"),
    DATA_COMPGEN(0x005009c4, gCombatFxNamesBluefireIcn, "bluefire.icn"),
    DATA_COMPGEN(0x005009d4, gCombatFxNamesIcecloudIcn, "icecloud.icn"),
    DATA_COMPGEN(0x005009e4, gCombatFxNamesLichclodIcn, "lichclod.icn"),
    DATA_COMPGEN(0x005009f4, gCombatFxNamesBlessIcn, "bless.icn"),
    DATA_COMPGEN(0x00500a00, gCombatFxNamesBerzerkIcn, "berzerk.icn"),
    DATA_COMPGEN(0x00500a0c, gCombatFxNamesShieldIcn, "shield.icn"),
    DATA_COMPGEN(0x00500a18, gCombatFxNamesHasteIcn, "haste.icn"),
    DATA_COMPGEN(0x00500a24, gCombatFxNamesParalyzeIcn, "paralyze.icn"),
    DATA_COMPGEN(0x00500a34, gCombatFxNamesHypnotizIcn, "hypnotiz.icn"),
    DATA_COMPGEN(0x00500a44, gCombatFxNamesDragslayIcn, "dragslay.icn"),
    DATA_COMPGEN(0x00500a54, gCombatFxNamesBlindIcn, "blind.icn"),
    DATA_COMPGEN(0x00500a60, gCombatFxNamesCurseIcn, "curse.icn"),
    DATA_COMPGEN(0x00500a6c, gCombatFxNamesStonskinIcn, "stonskin.icn"),
    DATA_COMPGEN(0x00500a7c, gCombatFxNamesStelskinIcn, "stelskin.icn")
};
DATA(0x004faa10) i16 horseFrameFlip[MOVEMENT_FRAME_FLIP_COUNT] =
    {45, 46, 47, 48, 49, 50, 51, 52, 53, 179, 178, 177, 54, 175, 174, 55};
DATA(0x004faa30) i16 boatFrameFlip[MOVEMENT_FRAME_FLIP_COUNT] =
    {0, 0, 9, 9, 18, 18, 27, 27, 36, 36, 155, 155, 146, 146, 137, 137};
DATA(0x004faa50) i8 gCastleResources[CASTLE_RESOURCE_SLOT_COUNT] =
    {IDX(RES_WOOD), IDX(RES_ORE), -1, -1, 0, 0, 0, 0};
DATA(0x004faa58) i16 gCastleAmounts[CASTLE_AMOUNT_COUNT] = {20, 20, 0, 0};
DATA(0x004faa60) i32 gHeroGoldCost = HERO_RECRUITMENT_GOLD_COST;
DATA(0x004faa68) i16 gVesaMode[VESA_MODE_VALUE_COUNT] =
    {640, 480, 256, VESA_SET_MODE_FUNCTION, VESA_MODE_640_480_256, 0, 0, 0};
DATA(0x004faa78) tag_tilePoint normalDirTable[NORMAL_DIRECTION_COUNT] = {
    {0, -1, 16},
    {1, -1, 16},
    {1, 0, 16},
    {1, 1, 16},
    {0, 1, 16},
    {-1, 1, 16},
    {-1, 0, 16},
    {-1, -1, 16}
};
DATA(0x004faa98) i32
    gResourceBaseValue[RESOURCE_VALUE_COUNT] = {200, 300, 200, 300, 300, 300, 1, 0};
DATA(0x004faab8) i32
    gInitResourcesHuman[STARTING_RESOURCE_DIFFICULTY_COUNT][STARTING_RESOURCE_TYPE_COUNT] = {
        {30, 10, 30, 10, 10, 10, 10000},
        {20, 5, 20, 5, 5, 5, 7500},
        {10, 2, 10, 2, 2, 2, 5000},
        {5, 0, 5, 0, 0, 0, 2500},
        {0, 0, 0, 0, 0, 0, 0}
};
DATA(0x004fab48) i32
    gInitResourcesComputer[STARTING_RESOURCE_DIFFICULTY_COUNT][STARTING_RESOURCE_TYPE_COUNT] = {
        {20, 5, 20, 5, 5, 5, 7500},
        {20, 5, 20, 5, 5, 5, 7500},
        {30, 10, 30, 10, 10, 10, 10000},
        {30, 10, 30, 10, 10, 10, 10000},
        {30, 10, 30, 10, 10, 10, 10000}
};
DATA(0x004fabd8) i32 gMineCharacteristics[MINE_CHARACTERISTIC_COUNT] = {2, 1, 2, 1, 1, 1, 1000, 0};
DATA(0x004fabf8) i32 gSSValues[IDX(HERO_SKILL_COUNT)][SECONDARY_SKILL_VALUE_LEVEL_COUNT] = {
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
DATA(0x004faca0) H2_ENUM_STORAGE(ArtifactLevelMask, u8)
    gArtifactLevel[KB_ARTIFACT_LEVEL_COUNT] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x04, 0x04, 0x02, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x02, 0x04, 0x02, 0x04, 0x02, 0x04, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x04, 0x02, 0x02, 0x08, 0x08, 0x08, 0x08, 0x02, 0x04, 0x04, 0x08, 0x04, 0x04,
    0x08, 0x08, 0x08, 0x04, 0x04, 0x04, 0x08, 0x04, 0x04, 0x04, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x02, 0x08, 0x08, 0x08, 0x02, 0x04, 0x04, 0x08, 0x08, 0x02, 0x02, 0x02, 0x04,
    0x04, 0x02, 0x04, 0x02, 0x04, 0x02, 0x10, 0x20, 0x20, 0x20, 0x20, 0x02, 0x08, 0x02, 0x08,
    0x02, 0x02, 0x08, 0x08, 0x02, 0x02, 0x02, 0x04, 0x02, 0x02, 0x02, 0x02, 0x04, 0x00
};
DATA(0x004fad08) i32 gArtifactBaseRV[KB_ARTIFACT_BASE_VALUE_COUNT] = {
    13600, 22000, 18000, 14000, 19000, 18500, 22200, 25000, 6000,  4000, 4000,  5600,  1200,
    1200,  1200,  1200,  -1200, 2000,  1800,  1800,  2000,  1000,  3600, 5600,  4000,  5040,
    3060,  4420,  5610,  6630,  7000,  6000,  4000,  4500,  2250,  1200, 1200,  1200,  1200,
    3500,  1500,  500,   1000,  750,   500,   750,   750,   750,   750,  750,   750,   500,
    1500,  1000,  1000,  750,   750,   750,   750,   750,   750,   750,  1250,  750,   150,
    250,   3500,  1250,  250,   -1,    -1,    4000,  4000,  4000,  2000, 2000,  4000,  3800,
    7600,  3700,  7400,  0,     0,     0,     0,     0,     2500,  4800, 9000,  -3250, 36200,
    2000,  -1050, -1050, 10000, 10000, 15000, 720,   7500,  10000, 9200, 10000, 1500
};
DATA(0x004faea4) i32 gUltArtifactAvgValue = ULTIMATE_ARTIFACT_AVERAGE_VALUE;
DATA(0x004faea8) i32 giDebugLevel = 0;
DATA(0x004faeac) i8 giVisRangeTown = TOWN_VISIBILITY_RADIUS;
DATA(0x004faeb0) tag_monsterInfo gMonsterDatabase[IDX(CREATURE_COUNT)] = {
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
    {{200, 865}, 43, 5, 20, FACTION_BARBARIAN, 6, 6, 2, 3, 5, 0, DATA_COMPGEN(0x00508ac0, gMonsterDatabaseWolf, "wolf"), MONSTER_ATTRIBUTE_WIDE},
    {{300, 1065}, 36, 4, 40, FACTION_BARBARIAN, 2, 9, 5, 4, 6, 0, DATA_COMPGEN(0x00508ac8, gMonsterDatabaseOgre, "ogre"), MONSTER_FLAGS_NONE},
    {{500, 2070}, 41, 4, 60, FACTION_BARBARIAN, 4, 9, 5, 5, 7, 0, "ogre", MONSTER_FLAGS_NONE},
    {{600, 1921}, 32, 3, 40, FACTION_BARBARIAN, 4, 10, 5, 5, 7, 8, "trll", MONSTER_ATTRIBUTE_RANGED},
    {{700, 2337}, 33, 3, 40, FACTION_BARBARIAN, 5, 10, 5, 7, 9, 16, "trll", MONSTER_ATTRIBUTE_RANGED},
    {{750, 6074}, 58, 2, 80, FACTION_BARBARIAN, 5, 12, 9, 12, 24, 0, "cycl", MONSTER_ATTRIBUTE_TWO_HEX_ATTACKER},
    {{50, 129}, 26, 8, 2, FACTION_SORCERESS, 4, 4, 2, 1, 2, 0, "sprt", MONSTER_ATTRIBUTE_FLYING},
    {{200, 500}, 25, 6, 20, FACTION_SORCERESS, 2, 6, 5, 2, 4, 0, "dwrf", MONSTER_FLAGS_NONE},
    {{250, 716}, 29, 6, 20, FACTION_SORCERESS, 4, 6, 6, 2, 4, 0, "dwrf", MONSTER_FLAGS_NONE},
    {{250, 554}, 22, 4, 15, FACTION_SORCERESS, 4, 4, 3, 2, 3, 24, "elf_", MONSTER_ATTRIBUTE_RANGED},
    {{300, 658}, 22, 4, 15, FACTION_SORCERESS, 6, 5, 5, 2, 3, 24, "elf_", MONSTER_ATTRIBUTE_RANGED},
    {{350, 1290}, 37, 3, 25, FACTION_SORCERESS, 5, 7, 5, 5, 8, 8, "drui", MONSTER_ATTRIBUTE_RANGED},
    {{400, 1428}, 36, 3, 25, FACTION_SORCERESS, 6, 7, 7, 5, 8, 16, "drui", MONSTER_ATTRIBUTE_RANGED},
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
    {{150, 493}, 33, 6, 15, FACTION_WIZARD, 6, 5, 4, 2, 3, 0, DATA_COMPGEN(0x00508bc8, gMonsterDatabaseBoar, "boar"), MONSTER_ATTRIBUTE_WIDE},
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
    {{600, 1935}, 32, 2, 30, FACTION_WIZARD, 5, 11, 7, 7, 9, 12, DATA_COMPGEN(0x00501314, gMonsterDatabaseMage, "mage"), MONSTER_ATTRIBUTE_RANGED},
    {{700, 2469}, 35, 2, 35, FACTION_WIZARD, 6, 12, 8, 7, 9, 24, DATA_COMPGEN(0x00508bec, gMonsterDatabaseMage2, "mage"), MONSTER_ATTRIBUTE_RANGED},
    {{2000, 9589}, 42, 1, 150, FACTION_WIZARD, 4, 13, 10, 20, 30, 0, "titn", MONSTER_FLAGS_NONE},
    {{5000, 22933}, 79, 1, 300, FACTION_WIZARD, 6, 15, 15, 20, 30, 24, "titn", MONSTER_ATTRIBUTE_RANGED},
    {{75, 203}, 27, 8, 4, FACTION_NECROMANCER, 4, 4, 3, 2, 3, 0, "skel", MONSTER_ATTRIBUTE_UNDEAD},
    {{150, 310}, 21, 6, 15, FACTION_NECROMANCER, 2, 5, 2, 2, 3, 0, "zomb", MONSTER_ATTRIBUTE_UNDEAD},
    {{200, 506}, 25, 6, 20, FACTION_NECROMANCER, 4, 5, 2, 2, 3, 0, "zomb", MONSTER_ATTRIBUTE_UNDEAD},
    {{250, 868}, 35, 4, 25, FACTION_NECROMANCER, 4, 6, 6, 3, 4, 0, "mumy", MONSTER_ATTRIBUTE_UNDEAD},
    {{300, 1056}, 35, 4, 30, FACTION_NECROMANCER, 5, 6, 6, 3, 4, 0, "mumy", MONSTER_ATTRIBUTE_UNDEAD},
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
     DATA_COMPGEN(0x00508c60, gMonsterDatabaseLich, "lich"),
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
    {{650, 5692}, 60, 2, 50, FACTION_NEUTRAL, 6, 10, 9, 20, 30, 0, "geni", MONSTER_ATTRIBUTE_FLYING},
    {{500, 1979}, 40, 5, 35, FACTION_NEUTRAL, 4, 8, 9, 6, 10, 0, "meds", MONSTER_ATTRIBUTE_WIDE},
    {{500, 1732}, 35, 3, 50, FACTION_NEUTRAL, 3, 8, 8, 4, 5, 0, "eelm", MONSTER_FLAGS_NONE},
    {{500, 1412}, 28, 3, 35, FACTION_NEUTRAL, 6, 7, 7, 2, 8, 0, "aelm", MONSTER_FLAGS_NONE},
    {{500, 1501}, 30, 3, 40, FACTION_NEUTRAL, 5, 8, 6, 4, 6, 0, "felm", MONSTER_FLAGS_NONE},
    {{500, 1690}, 34, 3, 45, FACTION_NEUTRAL, 4, 6, 8, 3, 7, 0, "welm", MONSTER_FLAGS_NONE}
};
DATA(0x004fb568) float gfStatPower[KB_STAT_POWER_COUNT] = {
    0.5f,  0.5f,  0.5f,  0.5f,  0.52f, 0.54f, 0.56f, 0.58f, 0.6f,  0.62f, 0.64f,
    0.67f, 0.7f,  0.74f, 0.77f, 0.8f,  0.84f, 0.88f, 0.92f, 0.96f, 1.0f,  1.04f,
    1.08f, 1.12f, 1.16f, 1.2f,  1.24f, 1.28f, 1.32f, 1.36f, 1.4f,  1.44f, 1.48f,
    1.52f, 1.56f, 1.6f,  1.64f, 1.68f, 1.72f, 1.76f, 1.8f,  0.0f
};
DATA(0x004fb610) float gfBattleStat[KB_STAT_POWER_COUNT] = {
    0.2f, 0.2f,  0.2f, 0.2f,  0.2f, 0.25f, 0.3f, 0.35f, 0.4f, 0.45f, 0.5f, 0.55f, 0.6f, 0.65f,
    0.7f, 0.75f, 0.8f, 0.85f, 0.9f, 0.95f, 1.0f, 1.1f,  1.2f, 1.3f,  1.4f, 1.5f,  1.6f, 1.7f,
    1.8f, 1.9f,  2.0f, 2.1f,  2.2f, 2.3f,  2.4f, 2.5f,  2.6f, 2.7f,  2.8f, 2.9f,  3.0f, 0.0f
};
DATA(0x004fb6b8) i8 gSpellLimits[KB_SPELL_LIMIT_COUNT] = {3, 3, 2, 2, 1};
DATA(0x004fb6c0) float gfSpellCastableCombatMod[KB_SPELL_MOD_COUNT] =
    {0.27f, 0.4f, 0.48f, 0.56f, 0.64f, 0.73f, 0.81f, 0.88f, 0.93f, 0.97f, 1.0f, 0.0f};
DATA(0x004fb6f0) float gfSpellCastNumMod[KB_SPELL_MOD_COUNT] =
    {0.0f, 1.0f, 1.75f, 2.35f, 2.85f, 3.35f, 3.7f, 4.0f, 4.26f, 4.5f, 4.7f, 0.0f};
DATA(0x004fb720) float gfPhilAISpellPowerMod[KB_SPELL_MOD_COUNT] =
    {0.0f, 1.0f, 1.75f, 2.5f, 3.1f, 3.5f, 4.0f, 4.5f, 5.4f, 5.75f, 6.15f, 0.0f};
DATA(0x004fb750) float gfPhilAIDurationMod[KB_SPELL_MOD_COUNT] =
    {0.0f, 0.4f, 0.65f, 0.8f, 1.0f, 1.16f, 1.3f, 1.43f, 1.54f, 1.64f, 1.74f, 0.0f};
DATA(0x004fb780) float gfSpellTypeNumMod[KB_QUICK_COMBAT_SPELL_TYPE_COUNT] =
    {1.0f, 0.75f, 0.55f, 0.4f, 0.28f, 0.2f, 0.15f};
DATA(0x004fb79c) b32 gbDrawSavedCursor = false;
DATA(0x004fb7a0) b8 gbArrow[NORMAL_DIRECTION_COUNT][NORMAL_DIRECTION_COUNT] = {
    {8, 0, 0, 0, 8, 16, 16, 16},
    {17, 9, 1, 1, 1, 9, 17, 17},
    {18, 18, 10, 2, 2, 2, 10, 18},
    {19, 19, 19, 11, 3, 3, 3, 11},
    {12, 20, 20, 20, 12, 4, 4, 4},
    {5, 13, 21, 21, 21, 13, 5, 5},
    {6, 6, 14, 22, 22, 22, 14, 6},
    {7, 7, 7, 15, 23, 23, 23, 15}
};
DATA(0x004fb7e0) u8 giCloudType[KB_CLOUD_MASK_COUNT] = {
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
DATA(0x004fb8e0) i16 giScoreMon[IDX(CREATURE_COUNT)][IDX(MONSTER_SCORE_FIELD_COUNT)] = {
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
DATA(0x004fb9e8) i16 giScoreCampaignMon[IDX(CREATURE_COUNT)][IDX(MONSTER_SCORE_FIELD_COUNT)] = {
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
DATA(0x004fbaf0) i8 townTheme[IDX(TOWN_MUSIC_TABLE_SIZE)] = {
    IDX(TOWN_MUSIC_KNIGHT),
    IDX(TOWN_MUSIC_BARBARIAN),
    IDX(TOWN_MUSIC_WARLOCK),
    IDX(TOWN_MUSIC_WIZARD),
    IDX(TOWN_MUSIC_SORCERESS),
    IDX(TOWN_MUSIC_NECROMANCER),
    IDX(TOWN_MUSIC_NONE),
    IDX(TOWN_MUSIC_NONE)
};
DATA(0x004fbaf8) i8
    gHeroSkillBonus[IDX(FACTION_COUNT)][KB_HERO_LEVEL_BAND_COUNT][HERO_PRIMARY_STAT_COUNT] = {
        {{35, 45, 10, 10}, {25, 25, 25, 25}},
        {{55, 35, 5, 5}, {25, 25, 25, 25}},
        {{10, 10, 30, 50}, {20, 20, 30, 30}},
        {{10, 10, 50, 30}, {20, 20, 30, 30}},
        {{10, 10, 40, 40}, {20, 20, 30, 30}},
        {{15, 15, 35, 35}, {25, 25, 25, 25}}
};
DATA(0x004fbb28) b32 gbLoadingMonoIcon = false;
DATA(0x004fbb2c) i32 giMonoIconSkip = -1;
DATA(0x004fbb30) i32 giScrollX = 0;
DATA(0x004fbb34) i32 giScrollY = 0;
DATA(0x004fbb38) b32 gbNoBorder = false;
DATA(0x004fbb3c) b32 gbEnlargeScreenBlit = true;
DATA(0x004fbb40) H2_ENUM_STORAGE(ConfigExecutable, i32) giCurExe = CONFIG_EXECUTABLE_GAME;
DATA(0x004fbb44) b32 gbInDialog = false;
DATA(0x004fbb48) struct SMenuEnableStatus gsMenuEnableStatus[MENU_ENABLE_STATUS_COUNT] = {
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
DATA(0x004fbd34) b32 gbInSetupDialog = false;
DATA(0x004fbd38) b32 gbMinimized = false;
DATA(0x004fbd3c) b32 gbHeroMoving = false;
DATA(0x004fbd40) b32 gbInSmackMgr = false;
DATA(0x004fbd44) i32 glBottomRefresh = 0;
DATA(0x004fbd48) b32 gbBothMachinesWin95 = false;
DATA(0x004fbd4c) b32 gbGotFirstHeartbeat = false;
DATA(0x004fbd50) void* hmnuDflt = NULL;
DATA(0x004fbd54) void* hmnuCmbt = NULL;
DATA(0x004fbd58) void* hmnuAdv = NULL;
DATA(0x004fbd5c) void* hmnuTown = NULL;
DATA(0x004fbd60) char* cMonFilename[IDX(CREATURE_COUNT)] = {
    DATA_COMPGEN(0x00500a8c, cMonFilenamePeasantIcn, "peasant.icn"),  DATA_COMPGEN(0x00500a98, cMonFilenameArcherIcn, "archer.icn"),   DATA_COMPGEN(0x00500aa4, cMonFilenameArcher2Icn, "archer2.icn"),  DATA_COMPGEN(0x00500ab0, cMonFilenamePikemanIcn, "pikeman.icn"),  DATA_COMPGEN(0x00500abc, cMonFilenamePikeman2Icn, "pikeman2.icn"), DATA_COMPGEN(0x00500acc, cMonFilenameSwordsmnIcn, "swordsmn.icn"),
    DATA_COMPGEN(0x00500adc, cMonFilenameSwordsm2Icn, "swordsm2.icn"), DATA_COMPGEN(0x00500aec, cMonFilenameCavalryrIcn, "cavalryr.icn"), DATA_COMPGEN(0x00500afc, cMonFilenameCavalrybIcn, "cavalryb.icn"), DATA_COMPGEN(0x00500b0c, cMonFilenamePaladinIcn, "paladin.icn"),  DATA_COMPGEN(0x00500b18, cMonFilenamePaladin2Icn, "paladin2.icn"), DATA_COMPGEN(0x00500b28, cMonFilenameGoblinIcn, "goblin.icn"),
    DATA_COMPGEN(0x00500b34, cMonFilenameOrcIcn, "orc.icn"),      DATA_COMPGEN(0x00500b3c, cMonFilenameOrc2Icn, "orc2.icn"),     DATA_COMPGEN(0x00500b48, cMonFilenameWolfIcn, "wolf.icn"),     DATA_COMPGEN(0x00500b54, cMonFilenameOgreIcn, "ogre.icn"),     DATA_COMPGEN(0x00500b60, cMonFilenameOgre2Icn, "ogre2.icn"),    DATA_COMPGEN(0x00500b6c, cMonFilenameTrollIcn, "troll.icn"),
    DATA_COMPGEN(0x00500b78, cMonFilenameTroll2Icn, "troll2.icn"),   DATA_COMPGEN(0x00500b84, cMonFilenameCyclopsIcn, "cyclops.icn"),  DATA_COMPGEN(0x00500b90, cMonFilenameSpriteIcn, "sprite.icn"),   DATA_COMPGEN(0x00500b9c, cMonFilenameDwarfIcn, "dwarf.icn"),    DATA_COMPGEN(0x00500ba8, cMonFilenameDwarf2Icn, "dwarf2.icn"),   DATA_COMPGEN(0x00500bb4, cMonFilenameElfIcn, "elf.icn"),
    DATA_COMPGEN(0x00500bbc, cMonFilenameElf2Icn, "elf2.icn"),     DATA_COMPGEN(0x00500bc8, cMonFilenameDruidIcn, "druid.icn"),    DATA_COMPGEN(0x00500bd4, cMonFilenameDruid2Icn, "druid2.icn"),   DATA_COMPGEN(0x00500be0, cMonFilenameUnicornIcn, "unicorn.icn"),  DATA_COMPGEN(0x00500bec, cMonFilenamePhoenixIcn, "phoenix.icn"),  DATA_COMPGEN(0x00500bf8, cMonFilenameCentaurIcn, "centaur.icn"),
    DATA_COMPGEN(0x00500c04, cMonFilenameGargoyleIcn, "gargoyle.icn"), DATA_COMPGEN(0x00500c14, cMonFilenameGriffinIcn, "griffin.icn"),  DATA_COMPGEN(0x00500c20, cMonFilenameMinotaurIcn, "minotaur.icn"), DATA_COMPGEN(0x00500c30, cMonFilenameMinotau2Icn, "minotau2.icn"), DATA_COMPGEN(0x00500c40, cMonFilenameHydraIcn, "hydra.icn"),    DATA_COMPGEN(0x00500c4c, cMonFilenameDraggreeIcn, "draggree.icn"),
    DATA_COMPGEN(0x00500c5c, cMonFilenameDragredIcn, "dragred.icn"),  DATA_COMPGEN(0x00500c68, cMonFilenameDragblakIcn, "dragblak.icn"), DATA_COMPGEN(0x00500c78, cMonFilenameHalflingIcn, "halfling.icn"), DATA_COMPGEN(0x00500c88, cMonFilenameBoarIcn, "boar.icn"),     DATA_COMPGEN(0x00500c94, cMonFilenameGolemIcn, "golem.icn"),    DATA_COMPGEN(0x00500ca0, cMonFilenameGolem2Icn, "golem2.icn"),
    DATA_COMPGEN(0x00500cac, cMonFilenameRocIcn, "roc.icn"),      DATA_COMPGEN(0x00500cb4, cMonFilenameMage1Icn, "mage1.icn"),    DATA_COMPGEN(0x00500cc0, cMonFilenameMage2Icn, "mage2.icn"),    DATA_COMPGEN(0x00500ccc, cMonFilenameTitanbluIcn, "titanblu.icn"), DATA_COMPGEN(0x00500cdc, cMonFilenameTitanblaIcn, "titanbla.icn"), DATA_COMPGEN(0x00500cec, cMonFilenameSkeletonIcn, "skeleton.icn"),
    DATA_COMPGEN(0x00500cfc, cMonFilenameZombieIcn, "zombie.icn"),   DATA_COMPGEN(0x00500d08, cMonFilenameZombie2Icn, "zombie2.icn"),  DATA_COMPGEN(0x00500d14, cMonFilenameMummywIcn, "mummyw.icn"),   DATA_COMPGEN(0x00500d20, cMonFilenameMummy2Icn, "mummy2.icn"),   DATA_COMPGEN(0x00500d2c, cMonFilenameVampireIcn, "vampire.icn"),  DATA_COMPGEN(0x00500d38, cMonFilenameVampire2Icn, "vampire2.icn"),
    DATA_COMPGEN(0x00500d48, cMonFilenameLichIcn, "lich.icn"),     DATA_COMPGEN(0x00500d54, cMonFilenameLich2Icn, "lich2.icn"),    DATA_COMPGEN(0x00500d60, cMonFilenameDragboneIcn, "dragbone.icn"), DATA_COMPGEN(0x00500d70, cMonFilenameRogueIcn, "rogue.icn"),    DATA_COMPGEN(0x00500d7c, cMonFilenameNomadIcn, "nomad.icn"),    DATA_COMPGEN(0x00500d88, cMonFilenameGhostIcn, "ghost.icn"),
    DATA_COMPGEN(0x00500d94, cMonFilenameGenieIcn, "genie.icn"),    DATA_COMPGEN(0x00500da0, cMonFilenameMedusaIcn, "medusa.icn"),   DATA_COMPGEN(0x00500dac, cMonFilenameEelemIcn, "eelem.icn"),    DATA_COMPGEN(0x00500db8, cMonFilenameAelemIcn, "aelem.icn"),    DATA_COMPGEN(0x00500dc4, cMonFilenameFelemIcn, "felem.icn"),    DATA_COMPGEN(0x00500dd0, cMonFilenameWelemIcn, "welem.icn")
};
DATA(0x004fbe68) b32 gbProcessingCombatAction = false;
DATA(0x004fbe6c) RemoteNetworkProtocol iMPNetProtocol = REMOTE_PROTOCOL_NETBIOS;
DATA(0x004fbe70) i32 iLastDiffSendTo = DIFF_SEND_FORCE_WHOLE;
DATA(0x004fbe78) SSpellInfo gsSpellInfo[IDX(SPELL_COUNT)] = {
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
    {DATA_COMPGEN(0x005091a0, gsSpellInfoEmptyString, ""), 1, 39, 0, 30, 1, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {DATA_COMPGEN(0x00509250, gsSpellInfoEmptyString2, ""), 1, 40, 0, 30, 1, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {DATA_COMPGEN(0x005092a0, gsSpellInfoEmptyString3, ""), 2, 41, 0, 30, 2, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {DATA_COMPGEN(0x00509328, gsSpellInfoEmptyString4, ""), 3, 42, 0, 30, 2, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {DATA_COMPGEN(0x005097d4, gsSpellInfoEmptyString5, ""), 3, 43, 0, 30, 2, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {DATA_COMPGEN(0x005097d8, gsSpellInfoEmptyString6, ""), 4, 44, 0, 30, 3, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {DATA_COMPGEN(0x005097dc, gsSpellInfoEmptyString7, ""), 3, 45, 0, 30, 3, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {DATA_COMPGEN(0x005097e0, gsSpellInfoEmptyString8, ""), 2, 46, 0, 30, 5, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {DATA_COMPGEN(0x005097e4, gsSpellInfoEmptyString9, ""), 5, 47, 0, 1500, 10, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {DATA_COMPGEN(0x005097e8, gsSpellInfoEmptyString10, ""), 4, 48, 0, 30, 10, {10, 10, 10, 10, 10, 10}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {DATA_COMPGEN(0x005097ec, gsSpellInfoEmptyString11, ""), 5, 49, 0, 30, 20, {0, 0, 0, 0, 0, 0}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {DATA_COMPGEN(0x005097f0, gsSpellInfoEmptyString12, ""), 2, 50, 0, 30, 6, {0, 0, 0, 0, 0, 0}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {DATA_COMPGEN(0x0050a628, gsSpellInfoEmptyString13, ""), 2, 51, 0, 30, 8, {0, 0, 0, 0, 0, 0}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {DATA_COMPGEN(0x0050a62c, gsSpellInfoEmptyString14, ""), 4, 52, 0, 700, 15, {0, 0, 0, 0, 0, 0}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {DATA_COMPGEN(0x0050a630, gsSpellInfoEmptyString15, ""), 4, 53, 0, 700, 15, {0, 0, 0, 0, 0, 0}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {DATA_COMPGEN(0x0050a634, gsSpellInfoEmptyString16, ""), 4, 54, 0, 700, 15, {0, 0, 0, 0, 0, 0}, SPELL_INFO_ATTRIBUTE_ADVENTURE},
    {DATA_COMPGEN(0x0050a638, gsSpellInfoEmptyString17, ""), 4, 55, 0, 700, 15, {0, 0, 0, 0, 0, 0}, SPELL_INFO_ATTRIBUTE_ADVENTURE}
};
DATA(0x004fc410) char* cArmyFrameFileNames[IDX(CREATURE_COUNT)] = {
    DATA_COMPGEN(0x00500ddc, cArmyFrameFileNamesPeasFrmBin, "peas_frm.bin"), DATA_COMPGEN(0x00500dec, cArmyFrameFileNamesArchrfrmBin, "archrfrm.bin"), DATA_COMPGEN(0x00500dfc, cArmyFrameFileNamesArchrfrmBin2, "archrfrm.bin"), DATA_COMPGEN(0x00500e0c, cArmyFrameFileNamesPikmnfrmBin, "pikmnfrm.bin"), DATA_COMPGEN(0x00500e1c, cArmyFrameFileNamesPikmnfrmBin2, "pikmnfrm.bin"), DATA_COMPGEN(0x00500e2c, cArmyFrameFileNamesSwrdsfrmBin, "swrdsfrm.bin"),
    DATA_COMPGEN(0x00500e3c, cArmyFrameFileNamesSwrdsfrmBin2, "swrdsfrm.bin"), DATA_COMPGEN(0x00500e4c, cArmyFrameFileNamesCvlryfrmBin, "cvlryfrm.bin"), DATA_COMPGEN(0x00500e5c, cArmyFrameFileNamesCvlr2frmBin, "cvlr2frm.bin"), DATA_COMPGEN(0x00500e6c, cArmyFrameFileNamesPaladfrmBin, "paladfrm.bin"), DATA_COMPGEN(0x00500e7c, cArmyFrameFileNamesPaladfrmBin2, "paladfrm.bin"), DATA_COMPGEN(0x00500e8c, cArmyFrameFileNamesGoblnfrmBin, "goblnfrm.bin"),
    DATA_COMPGEN(0x00500e9c, cArmyFrameFileNamesOrcFrmBin, "orc__frm.bin"), DATA_COMPGEN(0x00500eac, cArmyFrameFileNamesOrcFrmBin2, "orc__frm.bin"), DATA_COMPGEN(0x00500ebc, cArmyFrameFileNamesWolfFrmBin, "wolf_frm.bin"), DATA_COMPGEN(0x00500ecc, cArmyFrameFileNamesOgreFrmBin, "ogre_frm.bin"), DATA_COMPGEN(0x00500edc, cArmyFrameFileNamesOgreFrmBin2, "ogre_frm.bin"), DATA_COMPGEN(0x00500eec, cArmyFrameFileNamesTrollfrmBin, "trollfrm.bin"),
    DATA_COMPGEN(0x00500efc, cArmyFrameFileNamesTrollfrmBin2, "trollfrm.bin"), DATA_COMPGEN(0x00500f0c, cArmyFrameFileNamesCyclofrmBin, "cyclofrm.bin"), DATA_COMPGEN(0x00500f1c, cArmyFrameFileNamesSpritfrmBin, "spritfrm.bin"), DATA_COMPGEN(0x00500f2c, cArmyFrameFileNamesDwarffrmBin, "dwarffrm.bin"), DATA_COMPGEN(0x00500f3c, cArmyFrameFileNamesDwarffrmBin2, "dwarffrm.bin"), DATA_COMPGEN(0x00500f4c, cArmyFrameFileNamesElfFrmBin, "elf__frm.bin"),
    DATA_COMPGEN(0x00500f5c, cArmyFrameFileNamesElfFrmBin2, "elf__frm.bin"), DATA_COMPGEN(0x00500f6c, cArmyFrameFileNamesDruidfrmBin, "druidfrm.bin"), DATA_COMPGEN(0x00500f7c, cArmyFrameFileNamesDruidfrmBin2, "druidfrm.bin"), DATA_COMPGEN(0x00500f8c, cArmyFrameFileNamesUnicofrmBin, "unicofrm.bin"), DATA_COMPGEN(0x00500f9c, cArmyFrameFileNamesPhoenfrmBin, "phoenfrm.bin"), DATA_COMPGEN(0x00500fac, cArmyFrameFileNamesCentrfrmBin, "centrfrm.bin"),
    DATA_COMPGEN(0x00500fbc, cArmyFrameFileNamesGarglfrmBin, "garglfrm.bin"), DATA_COMPGEN(0x00500fcc, cArmyFrameFileNamesGrifffrmBin, "grifffrm.bin"), DATA_COMPGEN(0x00500fdc, cArmyFrameFileNamesMinotfrmBin, "minotfrm.bin"), DATA_COMPGEN(0x00500fec, cArmyFrameFileNamesMinotfrmBin2, "minotfrm.bin"), DATA_COMPGEN(0x00500ffc, cArmyFrameFileNamesHydrafrmBin, "hydrafrm.bin"), DATA_COMPGEN(0x0050100c, cArmyFrameFileNamesDraggfrmBin, "draggfrm.bin"),
    DATA_COMPGEN(0x0050101c, cArmyFrameFileNamesDragrfrmBin, "dragrfrm.bin"), DATA_COMPGEN(0x0050102c, cArmyFrameFileNamesDragbfrmBin, "dragbfrm.bin"), DATA_COMPGEN(0x0050103c, cArmyFrameFileNamesHalflfrmBin, "halflfrm.bin"), DATA_COMPGEN(0x0050104c, cArmyFrameFileNamesBoarFrmBin, "boar_frm.bin"), DATA_COMPGEN(0x0050105c, cArmyFrameFileNamesGolemfrmBin, "golemfrm.bin"), DATA_COMPGEN(0x0050106c, cArmyFrameFileNamesGolemfrmBin2, "golemfrm.bin"),
    DATA_COMPGEN(0x0050107c, cArmyFrameFileNamesRocFrmBin, "roc__frm.bin"), DATA_COMPGEN(0x0050108c, cArmyFrameFileNamesMage1frmBin, "mage1frm.bin"), DATA_COMPGEN(0x0050109c, cArmyFrameFileNamesMage1frmBin2, "mage1frm.bin"), DATA_COMPGEN(0x005010ac, cArmyFrameFileNamesTitanfrmBin, "titanfrm.bin"), DATA_COMPGEN(0x005010bc, cArmyFrameFileNamesTita2frmBin, "tita2frm.bin"), DATA_COMPGEN(0x005010cc, cArmyFrameFileNamesSkelFrmBin, "skel_frm.bin"),
    DATA_COMPGEN(0x005010dc, cArmyFrameFileNamesZombFrmBin, "zomb_frm.bin"), DATA_COMPGEN(0x005010ec, cArmyFrameFileNamesZombFrmBin2, "zomb_frm.bin"), DATA_COMPGEN(0x005010fc, cArmyFrameFileNamesMummyfrmBin, "mummyfrm.bin"), DATA_COMPGEN(0x0050110c, cArmyFrameFileNamesMummyfrmBin2, "mummyfrm.bin"), DATA_COMPGEN(0x0050111c, cArmyFrameFileNamesVampifrmBin, "vampifrm.bin"), DATA_COMPGEN(0x0050112c, cArmyFrameFileNamesVampifrmBin2, "vampifrm.bin"),
    DATA_COMPGEN(0x0050113c, cArmyFrameFileNamesLichFrmBin, "lich_frm.bin"), DATA_COMPGEN(0x0050114c, cArmyFrameFileNamesLichFrmBin2, "lich_frm.bin"), DATA_COMPGEN(0x0050115c, cArmyFrameFileNamesDrabnfrmBin, "drabnfrm.bin"), DATA_COMPGEN(0x0050116c, cArmyFrameFileNamesRoguefrmBin, "roguefrm.bin"), DATA_COMPGEN(0x0050117c, cArmyFrameFileNamesNomadfrmBin, "nomadfrm.bin"), DATA_COMPGEN(0x0050118c, cArmyFrameFileNamesGhostfrmBin, "ghostfrm.bin"),
    DATA_COMPGEN(0x0050119c, cArmyFrameFileNamesGeniefrmBin, "geniefrm.bin"), DATA_COMPGEN(0x005011ac, cArmyFrameFileNamesMedusfrmBin, "medusfrm.bin"), DATA_COMPGEN(0x005011bc, cArmyFrameFileNamesFelemfrmBin, "felemfrm.bin"), DATA_COMPGEN(0x005011cc, cArmyFrameFileNamesFelemfrmBin2, "felemfrm.bin"), DATA_COMPGEN(0x005011dc, cArmyFrameFileNamesFelemfrmBin3, "felemfrm.bin"), DATA_COMPGEN(0x005011ec, cArmyFrameFileNamesFelemfrmBin4, "felemfrm.bin")
};
DATA(0x004fc518) u8 gcSpellInfluenceIcons[KB_SPELL_INFLUENCE_MAP_COUNT] = {
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
DATA(0x004fc528) u8 giSpellInfluenceToSpell[KB_SPELL_INFLUENCE_MAP_COUNT] = {
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
DATA(0x004fc538) u8 giNumPowFrames[KB_SPELL_EFFECT_COUNT] = {10, 10, 10, 10, 10, 10, 10, 10,
                                                             10, 8,  8,  10, 10, 10, 10, 15,
                                                             10, 10, 10, 10, 10, 16, 16, 14,
                                                             19, 22, 10, 17, 10, 12, 11, 16};
DATA(0x004fc558)
H2_ENUM_STORAGE(SpellEffectDisplayType, i32)
giSpellEffectShowType = SPELL_EFFECT_DISPLAY_EFFECT_STATUS;
DATA(0x004fc560) i8 gcColorToPlayerPos[RADAR_OWNER_COLOR_COUNT] = {0, 1, 2, 3, 4, 5, 0, 0};
DATA(0x004fc568) char* cCombatBkgNames[KB_COMBAT_BACKGROUND_COUNT] = {
    DATA_COMPGEN(0x005011fc, cCombatBkgNamesCBKGWATRIcn, "CBKGWATR.icn"), DATA_COMPGEN(0x0050abd0, cCombatBkgNamesEmptyString, ""),
    DATA_COMPGEN(0x00501210, cCombatBkgNamesCBKGGRTRIcn, "CBKGGRTR.icn"), DATA_COMPGEN(0x00501220, cCombatBkgNamesCBKGGRMTIcn, "CBKGGRMT.icn"),
    DATA_COMPGEN(0x00501230, cCombatBkgNamesCBKGSNTRIcn, "CBKGSNTR.icn"), DATA_COMPGEN(0x00501240, cCombatBkgNamesCBKGSNMTIcn, "CBKGSNMT.icn"),
    DATA_COMPGEN(0x00501250, cCombatBkgNamesCBKGSWMPIcn, "CBKGSWMP.icn"), DATA_COMPGEN(0x0050abd4, cCombatBkgNamesEmptyString2, ""),
    DATA_COMPGEN(0x00501264, cCombatBkgNamesCBKGLAVAIcn, "CBKGLAVA.icn"), DATA_COMPGEN(0x0050ad28, cCombatBkgNamesEmptyString3, ""),
    DATA_COMPGEN(0x00501278, cCombatBkgNamesCBKGDSRTIcn, "CBKGDSRT.icn"), DATA_COMPGEN(0x0050ad2c, cCombatBkgNamesEmptyString4, ""),
    DATA_COMPGEN(0x0050128c, cCombatBkgNamesCBKGDITRIcn, "CBKGDITR.icn"), DATA_COMPGEN(0x0050129c, cCombatBkgNamesCBKGDIMTIcn, "CBKGDIMT.icn"),
    DATA_COMPGEN(0x005012ac, cCombatBkgNamesCBKGCRCKIcn, "CBKGCRCK.icn"), DATA_COMPGEN(0x0050ad30, cCombatBkgNamesEmptyString5, ""),
    DATA_COMPGEN(0x005012c0, cCombatBkgNamesCBKGBEACIcn, "CBKGBEAC.icn"), DATA_COMPGEN(0x0050ad34, cCombatBkgNamesEmptyString6, ""),
    DATA_COMPGEN(0x005012d4, cCombatBkgNamesCBKGGRAVIcn, "CBKGGRAV.icn"), NULL
};
DATA(0x004fc5b8) struct SCmbtObstacle sCmbtObstacles[KB_COMBAT_OBSTACLE_COUNT] = {
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
DATA(0x004fc778) i32 gEstatesGoldLevel[IDX(HERO_SKILL_LEVEL_COUNT)] = {0, 100, 250, 500};
DATA(0x004fc788) float gfSSLogisticsMod[IDX(HERO_SKILL_LEVEL_COUNT)] = {1.0f, 1.1f, 1.2f, 1.3f};
DATA(0x004fc798) float gfSSNavigationMod[IDX(HERO_SKILL_LEVEL_COUNT)] = {1.0f, 1.33f, 1.66f, 2.0f};
DATA(0x004fc7a8) float gfSSArcheryMod[IDX(HERO_SKILL_LEVEL_COUNT)] = {1.0f, 1.1f, 1.25f, 1.5f};
DATA(0x004fc7b8) float gfSSAIArcheryMod[IDX(HERO_SKILL_LEVEL_COUNT)] = {1.0f, 1.04f, 1.1f, 1.2f};
DATA(0x004fc7c8) i8 giVisRange[IDX(HERO_SKILL_LEVEL_COUNT)] = {4, 5, 6, 7};
DATA(0x004fc7d0) u8 gStartingHeroStats[IDX(FACTION_COUNT)][HERO_STARTING_STAT_COUNT] = {
    {2, 2, 1, 1, 1},
    {3, 1, 1, 1, 1},
    {0, 0, 2, 3, 1},
    {0, 0, 3, 2, 1},
    {0, 1, 2, 2, 1},
    {1, 0, 2, 2, 1}
};
DATA(0x004fc7f0) i32
    giTerrainCost[KB_TERRAIN_TYPE_COUNT][IDX(HERO_SKILL_LEVEL_COUNT)][KB_TERRAIN_STEP_TYPE_COUNT] =
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
DATA(0x004fc930) u8 bStopOnTrigger[KB_TRIGGER_TYPE_COUNT] = {
    0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0
};
DATA(0x004fc9b0) char* gTownPrefixNames[IDX(FACTION_COUNT)] =
    {DATA_COMPGEN(0x005012e4, gTownPrefixNamesTwnk, "twnk"), DATA_COMPGEN(0x005012ec, gTownPrefixNamesTwnb, "twnb"), DATA_COMPGEN(0x005012f4, gTownPrefixNamesTwns, "twns"), DATA_COMPGEN(0x005012fc, gTownPrefixNamesTwnw, "twnw"), DATA_COMPGEN(0x00501304, gTownPrefixNamesTwnz, "twnz"), DATA_COMPGEN(0x0050130c, gTownPrefixNamesTwnn, "twnn")};
DATA(0x004fc9c8) char* gTownObjNames[KB_TOWN_OBJECT_NAME_COUNT] = {
    "mage", DATA_COMPGEN(0x0050131c, gTownObjNamesThie, "thie"), DATA_COMPGEN(0x00501324, gTownObjNamesTvrn, "tvrn"), DATA_COMPGEN(0x0050132c, gTownObjNamesDock, "dock"), DATA_COMPGEN(0x00501334, gTownObjNamesWell, "well"), DATA_COMPGEN(0x0050133c, gTownObjNamesTent, "tent"), DATA_COMPGEN(0x00501344, gTownObjNamesCstl, "cstl"), DATA_COMPGEN(0x0050134c, gTownObjNamesStat, "stat"), DATA_COMPGEN(0x00501354, gTownObjNamesLtur, "ltur"), DATA_COMPGEN(0x0050135c, gTownObjNamesRtur, "rtur"), DATA_COMPGEN(0x00501364, gTownObjNamesMark, "mark"),
    DATA_COMPGEN(0x0050136c, gTownObjNamesWel2, "wel2"), DATA_COMPGEN(0x00501374, gTownObjNamesMoat, "moat"), DATA_COMPGEN(0x0050137c, gTownObjNamesSpec, "spec"), DATA_COMPGEN(0x00501384, gTownObjNamesBoat, "boat"), DATA_COMPGEN(0x0050138c, gTownObjNamesCapt, "capt"), DATA_COMPGEN(0x00501394, gTownObjNamesExt0, "ext0"), DATA_COMPGEN(0x0050139c, gTownObjNamesExt1, "ext1"), DATA_COMPGEN(0x005013a4, gTownObjNamesExt2, "ext2"), DATA_COMPGEN(0x005013ac, gTownObjNamesDw0, "dw_0"), DATA_COMPGEN(0x005013b4, gTownObjNamesDw1, "dw_1"), DATA_COMPGEN(0x005013bc, gTownObjNamesDw2, "dw_2"),
    DATA_COMPGEN(0x005013c4, gTownObjNamesDw3, "dw_3"), DATA_COMPGEN(0x005013cc, gTownObjNamesDw4, "dw_4"), DATA_COMPGEN(0x005013d4, gTownObjNamesDw5, "dw_5"), DATA_COMPGEN(0x005013dc, gTownObjNamesUp1, "up_1"), DATA_COMPGEN(0x005013e4, gTownObjNamesUp2, "up_2"), DATA_COMPGEN(0x005013ec, gTownObjNamesUp3, "up_3"), DATA_COMPGEN(0x005013f4, gTownObjNamesUp4, "up_4"), DATA_COMPGEN(0x005013fc, gTownObjNamesUp5, "up_5"), DATA_COMPGEN(0x00501404, gTownObjNamesUp5b, "up5b"), DATA_COMPGEN(0x0050140c, gTownObjNamesExt3, "ext3")
};
DATA(0x004fca48)
H2_ENUM_STORAGE(CreatureType, i8)
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
DATA(0x004fca90) i32 gMageBuildingCosts[KB_MAGE_GUILD_LEVEL_COUNT][KB_BUILDING_RESOURCE_COUNT] = {
    {0, 0, 0, 0, 0, 0, 0},
    {5, 0, 5, 0, 0, 0, 2000},
    {5, 4, 5, 4, 4, 4, 1000},
    {5, 6, 5, 6, 6, 6, 1000},
    {5, 8, 5, 8, 8, 8, 1000},
    {5, 10, 5, 10, 10, 10, 1000}
};
DATA(0x004fcb38) i32 gSpecialBuildingCosts[IDX(FACTION_COUNT)][KB_BUILDING_RESOURCE_COUNT] = {
    {5, 0, 15, 0, 0, 0, 1500},
    {10, 0, 10, 0, 0, 0, 2000},
    {0, 0, 0, 0, 10, 0, 1500},
    {5, 0, 10, 0, 0, 0, 3000},
    {5, 5, 5, 5, 5, 5, 1500},
    {0, 10, 0, 10, 0, 0, 1000}
};
DATA(0x004fcbe0) i32 gNeutralBuildingCosts[KB_BUILDING_NEUTRAL_LIMIT][KB_BUILDING_RESOURCE_COUNT] =
    {{5, 0, 5, 0, 0, 0, 2000},
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
     {0, 0, 0, 0, 0, 0, 500}};
DATA(0x004fcda0) i32
    gMageBaseResourceValues[KB_MAGE_GUILD_LEVEL_COUNT] = {0, 4000, 6500, 8500, 10500, 15000};
DATA(0x004fcdb8) i32 gNeutralBaseResourceValues[IDX(BUILDING_SLOT_DWELLING_FIRST) + 1] = {
    5000, 300, 350, 2000, 3000, 0, 12000, 2500, 1500, 1500, 200, 1000, 500, 0, 0, 1100, 0, 0, 0, 0
};
DATA(0x004fce08) i32
    gSpecialBuildingBaseResourceValues[IDX(FACTION_COUNT)] = {1500, 1000, 1000, 4500, 3500, 1000};
DATA(0x004fce20) i32 gDwellingBaseResourceValues[IDX(FACTION_COUNT)][KB_DWELLING_TYPE_COUNT] = {
    {858, 2225, 2816, 7385, 13754, 29785, 4000, 3200, 8000, 16000, 40000, 0},
    {1802, 2615, 3414, 6967, 13212, 38141, 3500, 0, 8000, 16000, 0, 0},
    {1684, 3000, 3500, 7213, 15181, 27684, 4000, 4000, 12000, 0, 0, 0},
    {1956, 2607, 3869, 7510, 16002, 90000, 0, 0, 8500, 0, 120000, 180000},
    {1700, 3500, 2800, 9000, 11500, 85000, 0, 3500, 0, 15000, 155000, 0},
    {2200, 2100, 3800, 6000, 9500, 90000, 3000, 4900, 15000, 12000, 0, 0}
};
DATA(0x004fcf40) i32
    gDwellingCosts[IDX(FACTION_COUNT)][KB_DWELLING_TYPE_COUNT][KB_BUILDING_RESOURCE_COUNT] = {
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
DATA(0x004fd720) u32l gHierarchyMask[IDX(FACTION_COUNT)][KB_DWELLING_TYPE_COUNT] = {
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
DATA(0x004fd840) i32 giDebugBuildingToBuild = -1;
DATA(0x004fd848) u8
    giTerrainToMusicTrack[IDX(TERRAIN_COUNT)] = {16, 18, 14, 15, 11, 13, 17, 12, 16};
DATA(0x004fd858) char* cHeroTypeShortName[IDX(FACTION_COUNT)] =
    {DATA_COMPGEN(0x00501414, cHeroTypeShortNameKngt, "kngt"), DATA_COMPGEN(0x0050141c, cHeroTypeShortNameBarb, "barb"), DATA_COMPGEN(0x00501424, cHeroTypeShortNameSorc, "sorc"), DATA_COMPGEN(0x0050142c, cHeroTypeShortNameWrlk, "wrlk"), DATA_COMPGEN(0x00501434, cHeroTypeShortNameWzrd, "wzrd"), DATA_COMPGEN(0x0050143c, cHeroTypeShortNameNecr, "necr")};
DATA(0x004fd870) char cHeroTypeInitial[HERO_TYPE_INITIAL_COUNT] = {'k', 'b', 's', 'w', 'z', 'n'};
DATA(0x004fd878) i32 giDeferObjDrawX = -1;
DATA(0x004fd87c) i32 giDeferObjDrawY = -1;
DATA(0x004fd880) class heroWindow* gpInitWin = NULL;
DATA(0x004fd888) u8 iGetSSByAlignment[IDX(HERO_SKILL_COUNT)][IDX(FACTION_COUNT)] = {
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
DATA(0x004fd8e0) struct SCmbtHero sCmbtHero[KB_COMBAT_HERO_SPRITE_COUNT] = {
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
DATA(0x004fdeec) u8 iWallToHexCell[KB_CASTLE_WALL_SEGMENT_COUNT] = {9, 34, 86, 113};
DATA(0x004fdef0) u8 iTowerToHexCell[KB_CASTLE_TOWER_COUNT] = {22, 47, 73, 100};
DATA(0x004fdef8) u16 wallPos[KB_CASTLE_WALL_SEGMENT_COUNT][IDX(COORDINATE_AXIS_COUNT)] =
    {{468, 58}, {421, 128}, {417, 291}, {498, 402}};
DATA(0x004fdf08) u16 towerPos[KB_CASTLE_TOWER_COUNT][IDX(COORDINATE_AXIS_COUNT)] =
    {{441, 61}, {398, 147}, {398, 233}, {441, 316}};
DATA(0x004fdf18) u16
    doorPos[KB_CASTLE_DOOR_POSITION_COUNT][IDX(COORDINATE_AXIS_COUNT)] = {
        {393, 192}, {348, 262}
    };
DATA(0x004fdf20) float fTradingPostEfficency[KB_TRADING_POST_EFFICIENCY_COUNT] =
    {0.0f, 0.1f, 0.15f, 0.2f, 0.25f, 0.3f, 0.35f, 0.4f, 0.45f, 0.5f, 0.5f};
DATA(0x004fdf50) struct SElevationOverlay sElevationOverlay[ELEVATION_OVERLAY_COUNT] = {
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
DATA(0x004fe100) i8 captainStats[IDX(FACTION_COUNT)][HERO_PRIMARY_STAT_COUNT] =
    {{1, 1, 1, 1}, {1, 1, 1, 1}, {0, 0, 2, 2}, {0, 0, 2, 2}, {0, 0, 2, 2}, {0, 0, 2, 2}};
DATA(0x004fe118) b32 gbDrawingPuzzle = false;
DATA(0x004fe11c) i32 giWalkingFrom = -1;
DATA(0x004fe120) i32 giWalkingFrom2 = -1;
DATA(0x004fe124) i32 giWalkingTo = -1;
DATA(0x004fe128) i32 giWalkingTo2 = -1;
DATA(0x004fe12c) i32 giWalkingYMod = 0;
DATA(0x004fe130) u8 moatCell[KB_MOAT_CELL_COUNT] = {8, 21, 33, 46, 58, 72, 85, 99, 112};
DATA(0x004fe140) SCampaignChoice campaignChoices[IDX(
    CAMPAIGN_SIDE_COUNT
)][CAMPAIGN_MAP_COUNT][CAMPAIGN_BONUS_CHOICE_COUNT] = {
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
DATA(0x004fe2a8) char* congratsText = NULL;
DATA(0x004fe2b0) char* gArtifactNames[KB_ARTIFACT_LEVEL_COUNT] = {
    DATA_COMPGEN(0x00501444, gArtifactNamesUltimateBookOfKnowledge, "Ultimate Book of Knowledge"),
    DATA_COMPGEN(0x00501460, gArtifactNamesUltimateSwordOfDominion, "Ultimate Sword of Dominion"),
    DATA_COMPGEN(0x0050147c, gArtifactNamesUltimateCloakOfProtection, "Ultimate Cloak of Protection"),
    DATA_COMPGEN(0x0050149c, gArtifactNamesUltimateWandOfMagic, "Ultimate Wand of Magic"),
    DATA_COMPGEN(0x005014b4, gArtifactNamesUltimateShield, "Ultimate Shield"),
    DATA_COMPGEN(0x005014c4, gArtifactNamesUltimateStaff, "Ultimate Staff"),
    DATA_COMPGEN(0x005014d4, gArtifactNamesUltimateCrown, "Ultimate Crown"),
    DATA_COMPGEN(0x005014e4, gArtifactNamesGoldenGoose, "Golden Goose"),
    DATA_COMPGEN(0x005014f4, gArtifactNamesArcaneNecklaceOfMagic, "Arcane Necklace of Magic"),
    DATA_COMPGEN(0x00501510, gArtifactNamesCasterSBraceletOfMagic, "Caster's Bracelet of Magic"),
    DATA_COMPGEN(0x0050152c, gArtifactNamesMageSRingOfPower, "Mage's Ring of Power"),
    DATA_COMPGEN(0x00501544, gArtifactNamesWitchSBroachOfMagic, "Witch's Broach of Magic"),
    DATA_COMPGEN(0x0050155c, gArtifactNamesMedalOfValor, "Medal of Valor"),
    DATA_COMPGEN(0x0050156c, gArtifactNamesMedalOfCourage, "Medal of Courage"),
    DATA_COMPGEN(0x00501580, gArtifactNamesMedalOfHonor, "Medal of Honor"),
    DATA_COMPGEN(0x00501590, gArtifactNamesMedalOfDistinction, "Medal of Distinction"),
    DATA_COMPGEN(0x005015a8, gArtifactNamesFizbinOfMisfortune, "Fizbin of Misfortune"),
    DATA_COMPGEN(0x005015c0, gArtifactNamesThunderMaceOfDominion, "Thunder Mace of Dominion"),
    DATA_COMPGEN(0x005015dc, gArtifactNamesArmoredGauntletsOfProtection, "Armored Gauntlets of Protection"),
    DATA_COMPGEN(0x005015fc, gArtifactNamesDefenderHelmOfProtection, "Defender Helm of Protection"),
    DATA_COMPGEN(0x00501618, gArtifactNamesGiantFlailOfDominion, "Giant Flail of Dominion"),
    DATA_COMPGEN(0x00501630, gArtifactNamesBallistaOfQuickness, "Ballista of Quickness"),
    DATA_COMPGEN(0x00501648, gArtifactNamesStealthShieldOfProtection, "Stealth Shield of Protection"),
    DATA_COMPGEN(0x00501668, gArtifactNamesDragonSwordOfDominion, "Dragon Sword of Dominion"),
    DATA_COMPGEN(0x00501684, gArtifactNamesPowerAxeOfDominion, "Power Axe of Dominion"),
    DATA_COMPGEN(0x0050169c, gArtifactNamesDivineBreastplateOfProtection, "Divine Breastplate of Protection"),
    DATA_COMPGEN(0x005016c0, gArtifactNamesMinorScrollOfKnowledge, "Minor Scroll of Knowledge"),
    DATA_COMPGEN(0x005016dc, gArtifactNamesMajorScrollOfKnowledge, "Major Scroll of Knowledge"),
    DATA_COMPGEN(0x005016f8, gArtifactNamesSuperiorScrollOfKnowledge, "Superior Scroll of Knowledge"),
    DATA_COMPGEN(0x00501718, gArtifactNamesForemostScrollOfKnowledge, "Foremost Scroll of Knowledge"),
    DATA_COMPGEN(0x00501738, gArtifactNamesEndlessSackOfGold, "Endless Sack of Gold"),
    DATA_COMPGEN(0x00501750, gArtifactNamesEndlessBagOfGold, "Endless Bag of Gold"),
    DATA_COMPGEN(0x00501764, gArtifactNamesEndlessPurseOfGold, "Endless Purse of Gold"),
    DATA_COMPGEN(0x0050177c, gArtifactNamesNomadBootsOfMobility, "Nomad Boots of Mobility"),
    DATA_COMPGEN(0x00501794, gArtifactNamesTravelerSBootsOfMobility, "Traveler's Boots of Mobility"),
    DATA_COMPGEN(0x005017b4, gArtifactNamesLuckyRabbitSFoot, "Lucky Rabbit's Foot"),
    DATA_COMPGEN(0x005017c8, gArtifactNamesGoldenHorseshoe, "Golden Horseshoe"),
    DATA_COMPGEN(0x005017dc, gArtifactNamesGamblerSLuckyCoin, "Gambler's Lucky Coin"),
    DATA_COMPGEN(0x005017f4, gArtifactNamesFourLeafClover, "Four-Leaf Clover"),
    DATA_COMPGEN(0x00501808, gArtifactNamesTrueCompassOfMobility, "True Compass of Mobility"),
    DATA_COMPGEN(0x00501824, gArtifactNamesSailorSAstrolabeOfMobility, "Sailor's Astrolabe of Mobility"),
    DATA_COMPGEN(0x00501844, gArtifactNamesEvilEye, "Evil Eye"),
    DATA_COMPGEN(0x00501850, gArtifactNamesEnchantedHourglass, "Enchanted Hourglass"),
    DATA_COMPGEN(0x00501864, gArtifactNamesGoldWatch, "Gold Watch"),
    DATA_COMPGEN(0x00501870, gArtifactNamesSkullcap, "Skullcap"),
    DATA_COMPGEN(0x0050187c, gArtifactNamesIceCloak, "Ice Cloak"),
    DATA_COMPGEN(0x00501888, gArtifactNamesFireCloak, "Fire Cloak"),
    DATA_COMPGEN(0x00501894, gArtifactNamesLightningHelm, "Lightning Helm"),
    DATA_COMPGEN(0x005018a4, gArtifactNamesEvercoldIcicle, "Evercold Icicle"),
    DATA_COMPGEN(0x005018b4, gArtifactNamesEverhotLavaRock, "Everhot Lava Rock"),
    DATA_COMPGEN(0x005018c8, gArtifactNamesLightningRod, "Lightning Rod "),
    DATA_COMPGEN(0x005018d8, gArtifactNamesSnakeRing, "Snake-Ring"),
    DATA_COMPGEN(0x005018e4, gArtifactNamesAnkh, "Ankh"),
    DATA_COMPGEN(0x005018ec, gArtifactNamesBookOfElements, "Book of Elements"),
    DATA_COMPGEN(0x00501900, gArtifactNamesElementalRing, "Elemental Ring"),
    DATA_COMPGEN(0x00501910, gArtifactNamesHolyPendant, "Holy Pendant"),
    DATA_COMPGEN(0x00501920, gArtifactNamesPendantOfFreeWill, "Pendant of Free Will"),
    DATA_COMPGEN(0x00501938, gArtifactNamesPendantOfLife, "Pendant of Life"),
    DATA_COMPGEN(0x00501948, gArtifactNamesSerenityPendant, "Serenity Pendant"),
    DATA_COMPGEN(0x0050195c, gArtifactNamesSeeingEyePendant, "Seeing-eye Pendant"),
    DATA_COMPGEN(0x00501970, gArtifactNamesKineticPendant, "Kinetic Pendant"),
    DATA_COMPGEN(0x00501980, gArtifactNamesPendantOfDeath, "Pendant of Death"),
    DATA_COMPGEN(0x00501994, gArtifactNamesWandOfNegation, "Wand of Negation"),
    DATA_COMPGEN(0x005019a8, gArtifactNamesGoldenBow, "Golden Bow"),
    DATA_COMPGEN(0x005019b4, gArtifactNamesTelescope, "Telescope"),
    DATA_COMPGEN(0x005019c0, gArtifactNamesStatesmanSQuill, "Statesman's Quill"),
    DATA_COMPGEN(0x005019d4, gArtifactNamesWizardSHat, "Wizard's Hat"),
    DATA_COMPGEN(0x005019e4, gArtifactNamesPowerRing, "Power Ring"),
    DATA_COMPGEN(0x005019f0, gArtifactNamesAmmoCart, "Ammo Cart"),
    DATA_COMPGEN(0x005019fc, gArtifactNamesTaxLien, "Tax Lien"),
    DATA_COMPGEN(0x00501a08, gArtifactNamesHideousMask, "Hideous Mask"),
    DATA_COMPGEN(0x00501a18, gArtifactNamesEndlessPouchOfSulfur, "Endless Pouch of Sulfur"),
    DATA_COMPGEN(0x00501a30, gArtifactNamesEndlessVialOfMercury, "Endless Vial of Mercury"),
    DATA_COMPGEN(0x00501a48, gArtifactNamesEndlessPouchOfGems, "Endless Pouch of Gems"),
    DATA_COMPGEN(0x00501a60, gArtifactNamesEndlessCordOfWood, "Endless Cord of Wood"),
    DATA_COMPGEN(0x00501a78, gArtifactNamesEndlessCartOfOre, "Endless Cart of Ore"),
    DATA_COMPGEN(0x00501a8c, gArtifactNamesEndlessPouchOfCrystal, "Endless Pouch of Crystal"),
    DATA_COMPGEN(0x00501aa8, gArtifactNamesSpikedHelm, "Spiked Helm"),
    DATA_COMPGEN(0x00501ab4, gArtifactNamesSpikedShield, "Spiked Shield"),
    DATA_COMPGEN(0x00501ac4, gArtifactNamesWhitePearl, "White Pearl"),
    DATA_COMPGEN(0x00501ad0, gArtifactNamesBlackPearl, "Black Pearl"),
    DATA_COMPGEN(0x00501adc, gArtifactNamesMagicBook, "Magic Book"),
    DATA_COMPGEN(0x00501ae8, gArtifactNamesERRORArtifact82, "ERROR : Artifact 82"),
    DATA_COMPGEN(0x00501afc, gArtifactNamesERRORArtifact83, "ERROR : Artifact 83"),
    DATA_COMPGEN(0x00501b10, gArtifactNamesERRORArtifact84, "ERROR : Artifact 84"),
    DATA_COMPGEN(0x00501b24, gArtifactNamesERRORArtifact85, "ERROR : Artifact 85"),
    DATA_COMPGEN(0x00501b38, gArtifactNamesSpellScroll, "Spell Scroll"),
    DATA_COMPGEN(0x00501b48, gArtifactNamesArmOfTheMartyr, "Arm of the Martyr"),
    DATA_COMPGEN(0x00501b5c, gArtifactNamesBreastplateOfAnduran, "Breastplate of Anduran"),
    DATA_COMPGEN(0x00501b74, gArtifactNamesBroachOfShielding, "Broach of Shielding"),
    DATA_COMPGEN(0x00501b88, gArtifactNamesBattleGarbOfAnduran, "Battle Garb of Anduran"),
    DATA_COMPGEN(0x00501ba0, gArtifactNamesCrystalBall, "Crystal Ball"),
    DATA_COMPGEN(0x00501bb0, gArtifactNamesHeartOfFire, "Heart of Fire"),
    DATA_COMPGEN(0x00501bc0, gArtifactNamesHeartOfIce, "Heart of Ice"),
    DATA_COMPGEN(0x00501bd0, gArtifactNamesHelmetOfAnduran, "Helmet of Anduran"),
    DATA_COMPGEN(0x00501be4, gArtifactNamesHolyHammer, "Holy Hammer"),
    DATA_COMPGEN(0x00501bf0, gArtifactNamesLegendaryScepter, "Legendary Scepter"),
    DATA_COMPGEN(0x00501c04, gArtifactNamesMasthead, "Masthead"),
    DATA_COMPGEN(0x00501c10, gArtifactNamesSphereOfNegation, "Sphere of Negation"),
    DATA_COMPGEN(0x00501c24, gArtifactNamesStaffOfWizardry, "Staff of Wizardry"),
    DATA_COMPGEN(0x00501c38, gArtifactNamesSwordBreaker, "Sword Breaker"),
    DATA_COMPGEN(0x00501c48, gArtifactNamesSwordOfAnduran, "Sword of Anduran"),
    DATA_COMPGEN(0x00501c5c, gArtifactNamesSpadeOfNecromancy, "Spade of Necromancy"),
    NULL
};
DATA(0x004fe450) char* gArtifactDesc[KB_ARTIFACT_LEVEL_COUNT] = {
    DATA_COMPGEN(0x00501c70, gArtifactDescUltimateBook12KnowledgeTheUltimate, "{Ultimate Book\n(+12 Knowledge)}\n\nThe Ultimate Book of Knowledge increases your knowledge "
    "by 12."),
    DATA_COMPGEN(0x00501cd0, gArtifactDescUltimateSword12AttackTheUltimate, "{Ultimate Sword\n(+12 Attack)}\n\nThe Ultimate Sword of Dominion increases your attack skill "
    "by 12."),
    DATA_COMPGEN(0x00501d34, gArtifactDescUltimateCloak12DefenseTheUltimate, "{Ultimate Cloak\n(+12 Defense)}\n\nThe Ultimate Cloak of Protection increases your defense "
    "skill by 12."),
    DATA_COMPGEN(0x00501d9c, gArtifactDescUltimateWand12SpellPowerThe, "{Ultimate Wand\n(+12 Spell Power)}\n\nThe Ultimate Wand of Magic increases your spell power "
    "by 12."),
    DATA_COMPGEN(0x00501dfc, gArtifactDescUltimateShieldTheUltimateShieldIncreases, "{Ultimate Shield}\n\nThe Ultimate Shield increases your attack and defense skills by 6 each."),
    DATA_COMPGEN(0x00501e58, gArtifactDescUltimateStaffTheUltimateStaffIncreases, "{Ultimate Staff}\n\nThe Ultimate Staff increases your spell power and knowledge by 6 each."),
    DATA_COMPGEN(0x00501eb4, gArtifactDescUltimateCrownTheUltimateCrownIncreases, "{Ultimate Crown}\n\nThe Ultimate Crown increases each of your basic skills by 4 points."),
    DATA_COMPGEN(0x00501f0c, gArtifactDescGoldenGooseTheGoldenGooseBrings, "{Golden Goose}\n\nThe Golden Goose brings in an income of 10,000 gold per turn."),
    DATA_COMPGEN(0x00501f5c, gArtifactDescArcaneNecklace4SpellPowerThe, "{Arcane Necklace\n(+4 Spell Power)}\n\nThe Arcane Necklace of Magic increases your spell "
    "power by 4."),
    DATA_COMPGEN(0x00501fc0, gArtifactDescCasterSBracelet2SpellPower, "{Caster's Bracelet\n(+2 Spell Power)}\n\nThe Caster's Bracelet of Magic increases your spell "
    "power by 2."),
    DATA_COMPGEN(0x00502028, gArtifactDescMageSRing2SpellPower, "{Mage's Ring\n(+2 Spell Power)}\n\nThe Mage's Ring of Power increases your spell power by 2."),
    DATA_COMPGEN(0x00502084, gArtifactDescWitchesBroach3SpellPowerThe, "{Witches Broach\n(+3 Spell Power)}\n\nThe Witch's Broach of Magic increases your spell power "
    "by 3."),
    DATA_COMPGEN(0x005020e4, gArtifactDescMedalTheMedalOfValorIncreases, "{Medal}\n\nThe Medal of Valor increases your morale."),
    DATA_COMPGEN(0x00502118, gArtifactDescMedalTheMedalOfCourageIncreases, "{Medal}\n\nThe Medal of Courage increases your morale."),
    DATA_COMPGEN(0x00502150, gArtifactDescMedalTheMedalOfHonorIncreases, "{Medal}\n\nThe Medal of Honor increases your morale."),
    DATA_COMPGEN(0x00502184, gArtifactDescMedalTheMedalOfDistinctionIncreases, "{Medal}\n\nThe Medal of Distinction increases your morale."),
    DATA_COMPGEN(0x005021c0, gArtifactDescFizbinTheFizbinOfMisfortuneGreatly, "{Fizbin}\n\nThe Fizbin of Misfortune greatly decreases your morale."),
    DATA_COMPGEN(0x00502204, gArtifactDescThunderMace1AttackTheThunder, "{Thunder Mace\n(+1 Attack)}\n\nThe Thunder Mace of Dominion increases your attack skill by 1."),
    DATA_COMPGEN(0x00502260, gArtifactDescArmoredGauntlets1DefenseTheArmored, "{Armored Gauntlets\n(+1 Defense)}\n\nThe Armored Gauntlets of Protection increase your "
    "defense skill by 1."),
    DATA_COMPGEN(0x005022c8, gArtifactDescDefenderHelm1DefenseTheDefender, "{Defender Helm\n(+1 Defense)}\n\nThe Defender Helm of Protection increases your defense skill "
    "by 1."),
    DATA_COMPGEN(0x0050232c, gArtifactDescGiantFlail1AttackTheGiant, "{Giant Flail\n(+1 Attack)}\n\nThe Giant Flail of Dominion increases your attack skill by 1."),
    DATA_COMPGEN(0x00502388, gArtifactDescBallistaTheBallistaOfQuicknessLets, "{Ballista}\n\nThe Ballista of Quickness lets your catapult fire twice per combat round."),
    DATA_COMPGEN(0x005023e0, gArtifactDescStealthShield2DefenseTheStealth, "{Stealth Shield\n(+2 Defense)}\n\nThe Stealth Shield of Protection increases your defense "
    "skill by 2."),
    DATA_COMPGEN(0x00502444, gArtifactDescDragonSword3AttackTheDragon, "{Dragon Sword\n(+3 Attack)}\n\nThe Dragon Sword of Dominion increases your attack skill by 3."),
    DATA_COMPGEN(0x005024a0, gArtifactDescPowerAxe2AttackThePower, "{Power Axe\n(+2 Attack)}\n\nThe Power Axe of Dominion increases your attack skill by 2."),
    DATA_COMPGEN(0x005024f8, gArtifactDescDivineBreastplate3DefenseTheDivine, "{Divine Breastplate\n(+3 Defense)}\n\nThe Divine Breastplate of Protection increases your "
    "defense skill by 3."),
    DATA_COMPGEN(0x00502564, gArtifactDescMinorScroll2KnowledgeTheMinor, "{Minor Scroll\n(+2 Knowledge)}\n\nThe Minor Scroll of Knowledge increases your knowledge by "
    "2."),
    DATA_COMPGEN(0x005025c0, gArtifactDescMajorScroll3KnowledgeTheMajor, "{Major Scroll\n(+3 Knowledge)}\n\nThe Major Scroll of Knowledge increases your knowledge by "
    "3."),
    DATA_COMPGEN(0x0050261c, gArtifactDescSuperiorScroll4KnowledgeTheSuperior, "{Superior Scroll\n(+4 Knowledge)}\n\nThe Superior Scroll of Knowledge increases your "
    "knowledge by 4."),
    DATA_COMPGEN(0x00502680, gArtifactDescForemostScroll5KnowledgeTheForemost, "{Foremost Scroll\n(+5 Knowledge)}\n\nThe Foremost Scroll of Knowledge increases your "
    "knowledge by 5."),
    DATA_COMPGEN(0x005026e4, gArtifactDescEndlessSackTheEndlessSackOf, "{Endless Sack}\n\nThe Endless Sack of Gold provides you with 1000 gold per day."),
    DATA_COMPGEN(0x00502734, gArtifactDescEndlessBagTheEndlessBagOf, "{Endless Bag}\n\nThe Endless Bag of Gold provides you with 750 gold per day."),
    DATA_COMPGEN(0x00502780, gArtifactDescEndlessPurseTheEndlessPurseOf, "{Endless Purse}\n\nThe Endless Purse of Gold provides you with 500 gold per day."),
    DATA_COMPGEN(0x005027d0, gArtifactDescNomadBootsTheNomadBootsOf, "{Nomad Boots}\n\nThe Nomad Boots of Mobility increase your movement on land."),
    DATA_COMPGEN(0x0050281c, gArtifactDescTravelerSBootsTheTravelerS, "{Traveler's Boots}\n\nThe Traveler's Boots of Mobility increase your movement on land."),
    DATA_COMPGEN(0x00502874, gArtifactDescRabbitSFootTheLuckyRabbit, "{Rabbit's Foot}\n\nThe Lucky Rabbit's Foot increases your luck in combat."),
    DATA_COMPGEN(0x005028bc, gArtifactDescHorseshoeTheGoldenHorseshoeIncreasesYour, "{Horseshoe}\n\nThe Golden Horseshoe increases your luck in combat."),
    DATA_COMPGEN(0x00502900, gArtifactDescCoinTheGamblerSLuckyCoin, "{Coin}\n\nThe Gambler's Lucky Coin increases your luck in combat."),
    DATA_COMPGEN(0x00502940, gArtifactDescCloverTheFourLeafCloverIncreases, "{Clover}\n\nThe Four-Leaf Clover increases your luck in combat."),
    DATA_COMPGEN(0x00502980, gArtifactDescCompassTheTrueCompassOfMobility, "{Compass}\n\nThe True Compass of Mobility increases your movement on land and sea."),
    DATA_COMPGEN(0x005029d4, gArtifactDescAstrolabeTheSailorsAstrolabeOfMobility, "{Astrolabe}\n\nThe Sailors' Astrolabe of Mobility increases your movement on sea."),
    DATA_COMPGEN(0x00502a24, gArtifactDescEvilEyeTheEvilEyeReduces, "{Evil Eye}\n\nThe Evil Eye reduces the casting cost of curse spells by half."),
    DATA_COMPGEN(0x00502a70, gArtifactDescEnchantedHourglassTheEnchantedHourglassExtends, "{Enchanted hourglass}\n\nThe Enchanted Hourglass extends the duration of all your spells by 2 "
    "turns."),
    DATA_COMPGEN(0x00502ad4, gArtifactDescGoldWatchTheGoldWatchDoubles, "{Gold watch}\n\nThe Gold Watch doubles the effectiveness of your hypnotize spells."),
    DATA_COMPGEN(0x00502b28, gArtifactDescSkullcapTheSkullcapHalvesTheCasting, "{Skullcap}\n\nThe Skullcap halves the casting cost of all mind influencing spells."),
    DATA_COMPGEN(0x00502b7c, gArtifactDescIceCloakTheIceCloakHalves, "{Ice cloak}\n\nThe Ice Cloak halves all damage your troops take from cold spells."),
    DATA_COMPGEN(0x00502bcc, gArtifactDescFireCloakTheFireCloakHalves, "{Fire cloak}\n\nThe Fire Cloak halves all damage your troops take from fire spells."),
    DATA_COMPGEN(0x00502c20, gArtifactDescLightningHelmTheLightningHelmHalves, "{Lightning helm}\n\nThe Lightning Helm halves all damage your troops take from lightning "
    "spells."),
    DATA_COMPGEN(0x00502c80, gArtifactDescEvercoldIcicleTheEvercoldIcicleCauses, "{Evercold icicle}\n\nThe Evercold Icicle causes your cold spells to do 50% more damage to "
    "enemy troops."),
    DATA_COMPGEN(0x00502ce8, gArtifactDescEverhotLavaRockTheEverhotLava, "{Everhot lava rock}\n\nThe Everhot Lava Rock causes your fire spells to do 50% more damage to "
    "enemy troops."),
    DATA_COMPGEN(0x00502d54, gArtifactDescLightningRodTheLightningRodCauses, "{Lightning rod }\n\nThe Lightning Rod causes your lightning spells to do 50% more damage to "
    "enemy troops."),
    DATA_COMPGEN(0x00502dbc, gArtifactDescSnakeRingTheSnakeRingHalves, "{Snake-ring}\n\nThe Snake Ring halves the casting cost of all your bless spells."),
    DATA_COMPGEN(0x00502e0c, gArtifactDescAnkhTheAnkhDoublesTheEffectiveness, "{Ankh}\n\nThe Ankh doubles the effectiveness of all your resurrect and animate spells."),
    DATA_COMPGEN(0x00502e64, gArtifactDescBookOfElementsTheBookOf, "{Book of Elements}\n\nThe Book of Elements doubles the effectiveness of all your summoning "
    "spells."),
    DATA_COMPGEN(0x00502ec8, gArtifactDescElementalRingTheElementalRingHalves, "{Elemental ring}\n\nThe Elemental Ring halves the casting cost of all summoning spells."),
    DATA_COMPGEN(0x00502f20, gArtifactDescHolyPendantTheHolyPendantMakes, "{Holy Pendant}\n\nThe Holy Pendant makes all your troops immune to curse spells."),
    DATA_COMPGEN(0x00502f70, gArtifactDescPendantOfFreeWillThePendant, "{Pendant of free will}\n\nThe Pendant of Free Will makes all your troops immune to hypnotize "
    "spells."),
    DATA_COMPGEN(0x00502fd4, gArtifactDescPendantOfLifeThePendantOf, "{Pendant of life}\n\nThe Pendant of Life makes all your troops immune to death spells."),
    DATA_COMPGEN(0x0050302c, gArtifactDescSerenityPendantTheSerenityPendantMakes, "{Serenity pendant}\n\nThe Serenity Pendant makes all your troops immune to berserk spells."),
    DATA_COMPGEN(0x00503088, gArtifactDescSeeingEyePendantTheSeeingEye, "{Seeing-eye pendant}\n\nThe Seeing-eye Pendant makes all your troops immune to blindness "
    "spells."),
    DATA_COMPGEN(0x005030e8, gArtifactDescKineticPendantTheKineticPendantMakes, "{Kinetic pendant}\n\nThe Kinetic Pendant makes all your troops immune to paralyze spells."),
    DATA_COMPGEN(0x00503140, gArtifactDescPendantOfDeathThePendantOf, "{Pendant of death}\n\nThe Pendant of Death makes all your troops immune to holy spells."),
    DATA_COMPGEN(0x00503198, gArtifactDescWandOfNegationTheWandOf, "{Wand of negation}\n\nThe Wand of Negation protects your troops from the Dispel Magic spell."),
    DATA_COMPGEN(0x005031f4, gArtifactDescGoldenBowTheGoldenBowEliminates, "{Golden bow}\n\nThe Golden Bow eliminates the 50% penalty for your troops shooting past "
    "obstacles. (e.g. castle walls)"),
    DATA_COMPGEN(0x0050326c, gArtifactDescTelescopeTheTelescopeIncreasesTheAmount, "{Telescope}\n\nThe Telescope increases the amount of terrain your hero reveals when "
    "adventuring by 1 extra square."),
    DATA_COMPGEN(0x005032e0, gArtifactDescStatesmanSQuillTheStatesmanS, "{Statesman's quill}\n\nThe Statesman's Quill reduces the cost of surrender to 10% of the "
    "total cost of troops you have in your army."),
    DATA_COMPGEN(0x00503364, gArtifactDescWizardSHatTheWizardS, "{Wizard's hat}\n\nThe Wizard's Hat increases the duration of your spells by 10 turns!"),
    DATA_COMPGEN(0x005033b8, gArtifactDescPowerRingThePowerRingReturns, "{Power ring}\n\nThe Power Ring returns 2 extra spell points/turn to your hero."),
    DATA_COMPGEN(0x00503408, gArtifactDescAmmoCartTheAmmoCartProvides, "{Ammo cart}\n\nThe Ammo Cart provides endless ammunition for all your troops that shoot."),
    DATA_COMPGEN(0x00503460, gArtifactDescTaxLienTheTaxLienCosts, "{Tax lien}\n\nThe Tax Lien costs you 250 gold pieces/turn."),
    DATA_COMPGEN(0x0050349c, gArtifactDescHideousMaskTheHideousMaskPrevents, "{Hideous mask}\n\nThe Hideous Mask prevents all 'wandering' armies from joining your hero."),
    DATA_COMPGEN(0x005034f8, gArtifactDescEndlessPouchOfSulfurTheEndless, "{Endless pouch of sulfur}\n\nThe Endless Pouch of Sulfur provides 1 unit of sulfur per day."),
    DATA_COMPGEN(0x00503554, gArtifactDescEndlessVialOfMercuryTheEndless, "{Endless vial of mercury}\n\nThe Endless Vial of Mercury provides 1 unit of mercury per day."),
    DATA_COMPGEN(0x005035b0, gArtifactDescEndlessPouchOfGemsTheEndless, "{Endless pouch of gems}\n\nThe Endless Pouch of Gems provides 1 unit of gems per day."),
    DATA_COMPGEN(0x00503604, gArtifactDescEndlessCordOfWoodTheEndless, "{Endless cord of wood}\n\nThe Endless Cord of Wood provides 1 unit of wood per day."),
    DATA_COMPGEN(0x00503658, gArtifactDescEndlessCartOfOreTheEndless, "{Endless cart of ore}\n\nThe Endless Cart of Ore provides 1 unit of ore per day."),
    DATA_COMPGEN(0x005036a8, gArtifactDescEndlessPouchOfCrystalTheEndless, "{Endless pouch of crystal}\n\nThe Endless Pouch of Crystal provides 1 unit of crystal/day."),
    DATA_COMPGEN(0x00503704, gArtifactDescSpikedHelmTheSpikedHelmIncreases, "{Spiked helm}\n\nThe Spiked Helm increases your attack and defense skills by 1 each."),
    DATA_COMPGEN(0x00503758, gArtifactDescSpikedShieldTheSpikedShieldIncreases, "{Spiked shield}\n\nThe Spiked Shield increases your attack and defense skills by 2 each."),
    DATA_COMPGEN(0x005037b0, gArtifactDescWhitePearlTheWhitePearlIncreases, "{White pearl}\n\nThe White Pearl increases your spell power and knowledge by 1 each."),
    DATA_COMPGEN(0x00503804, gArtifactDescBlackPearlTheBlackPearlIncreases, "{Black pearl}\n\nThe Black Pearl increases your spell power and knowledge by 2 each."),
    DATA_COMPGEN(0x00503858, gArtifactDescMagicBookTheMagicBookEnables, "{Magic Book}\n\nThe Magic Book enables you to cast spells."),
    DATA_COMPGEN(0x00503894, gArtifactDescERRORArtifact82, "{ERROR}\n\nArtifact 82."),
    DATA_COMPGEN(0x005038ac, gArtifactDescERRORArtifact83, "{ERROR}\n\nArtifact 83."),
    DATA_COMPGEN(0x005038c4, gArtifactDescERRORArtifact84, "{ERROR}\n\nArtifact 84."),
    DATA_COMPGEN(0x005038dc, gArtifactDescERRORArtifact85, "{ERROR}\n\nArtifact 85."),
    DATA_COMPGEN(0x005038f4, gArtifactDescSpellScrollThisSpellScrollGives, "{Spell Scroll}\n\nThis Spell Scroll gives your hero the ability to cast the '%s' spell."),
    DATA_COMPGEN(0x0050394c, gArtifactDescArmOfTheMartyrTheArm, "{Arm of the Martyr}\n\nThe Arm of the Martyr increases your spell power by 3 but adds the "
    "undead morale penalty."),
    DATA_COMPGEN(0x005039bc, gArtifactDescBreastplateOfAnduranTheBreastplateIncreases, "{Breastplate of Anduran}\n\nThe Breastplate increases your defense by 5."),
    DATA_COMPGEN(0x00503a04, gArtifactDescBroachOfShieldingTheBroachOf, "{Broach of Shielding}\n\nThe Broach of Shielding provides 50% protection from Armageddon and "
    "Elemental Storm, but decreases spell power by 2."),
    DATA_COMPGEN(0x00503a90, gArtifactDescBattleGarbTheBattleGarbOf, "{Battle Garb}\n\nThe Battle Garb of Anduran combines the powers of the three Anduran "
    "artifacts.  It provides maximum luck and morale for your troops and gives you the Town Portal "
    "spell."),
    DATA_COMPGEN(0x00503b48, gArtifactDescCrystalBallTheCrystalBallLets, "{Crystal Ball}\n\nThe Crystal Ball lets you get more specific information about monsters, "
    "enemy heroes, and castles nearby the hero who holds it."),
    DATA_COMPGEN(0x00503bd8, gArtifactDescHeartOfFireTheHeartOf, "{Heart of Fire}\n\nThe Heart of Fire provides 50% protection from fire, but doubles the "
    "damage taken from cold."),
    DATA_COMPGEN(0x00503c48, gArtifactDescHeartOfIceTheHeartOf, "{Heart of Ice}\n\nThe Heart of Ice provides 50% protection from cold, but doubles the damage "
    "taken from fire."),
    DATA_COMPGEN(0x00503cb4, gArtifactDescHelmetOfAnduranTheHelmetIncreases, "{Helmet of Anduran}\n\nThe Helmet increases your spell power by 5."),
    DATA_COMPGEN(0x00503cf8, gArtifactDescHolyHammerTheHolyHammerIncreases, "{Holy Hammer}\n\nThe Holy Hammer increases your attack skill by 5."),
    DATA_COMPGEN(0x00503d3c, gArtifactDescLegendaryScepterTheLegendaryScepterAdds, "{Legendary Scepter}\n\nThe Legendary Scepter adds 2 points to all attributes."),
    DATA_COMPGEN(0x00503d88, gArtifactDescMastheadTheMastheadBoostsYourLuck, "{Masthead}\n\nThe Masthead boosts your luck and morale by 1 each in sea combat."),
    DATA_COMPGEN(0x00503dd8, gArtifactDescSphereOfNegationTheSphereOf, "{Sphere of Negation}\n\nThe Sphere of Negation disables all spell casting, for both sides, in "
    "combat."),
    DATA_COMPGEN(0x00503e3c, gArtifactDescStaffOfWizardryTheStaffOf, "{Staff of Wizardry}\n\nThe Staff of Wizardry boosts your spell power by 5."),
    DATA_COMPGEN(0x00503e88, gArtifactDescSwordBreakerTheSwordBreakerIncreases, "{Sword Breaker}\n\nThe Sword Breaker increases your defense by 4 and attack by 1."),
    DATA_COMPGEN(0x00503ed8, gArtifactDescSwordOfAnduranTheSwordIncreases, "{Sword of Anduran}\n\nThe Sword increases your attack skill by 5."),
    DATA_COMPGEN(0x00503f18, gArtifactDescSpadeOfNecromancyTheSpadeGives, "{Spade of Necromancy}\n\nThe Spade gives you increased necromancy skill."),
    NULL
};
DATA(0x004fe5f0) char* gArtifactEvent[KB_ARTIFACT_LEVEL_COUNT] = {
    DATA_COMPGEN(0x0050ad38, gArtifactEventEmptyString, ""),
    DATA_COMPGEN(0x0050ad3c, gArtifactEventEmptyString2, ""),
    DATA_COMPGEN(0x0050b0d8, gArtifactEventEmptyString3, ""),
    DATA_COMPGEN(0x0050b0dc, gArtifactEventEmptyString4, ""),
    DATA_COMPGEN(0x0050b0e0, gArtifactEventEmptyString5, ""),
    DATA_COMPGEN(0x0050b0e4, gArtifactEventEmptyString6, ""),
    DATA_COMPGEN(0x0050b0e8, gArtifactEventEmptyString7, ""),
    DATA_COMPGEN(0x0050c3b4, gArtifactEventEmptyString8, ""),
    DATA_COMPGEN(0x00503f80, gArtifactEventAfterRescuingASorceressFromA, "After rescuing a Sorceress from a cursed tomb, she rewards your heroism with an exquisite "
    "jeweled necklace."),
    DATA_COMPGEN(0x00503fec, gArtifactEventWhileSearchingThroughTheRubbleOf, "While searching through the rubble of a caved-in mine, you free a group of trapped Dwarves.  "
    "Grateful, the leader gives you a golden bracelet."),
    DATA_COMPGEN(0x0050407c, gArtifactEventACryOfPainLeadsYou, "A cry of pain leads you to a Centaur, caught in a trap.  Upon setting the creature free, he "
    "hands you a small pouch.  Emptying the contents, you find a dazzling jeweled ring."),
    DATA_COMPGEN(0x0050412c, gArtifactEventAlongsideTheRemainsOfABurnt, "Alongside the remains of a burnt witch lies a beautiful broach, intricately designed.  "
    "Approaching the corpse with caution, you add the broach to your inventory."),
    DATA_COMPGEN(0x005041d0, gArtifactEventFreeingAVirtuousMaidenFromThe, "Freeing a virtuous maiden from the clutches of an evil overlord, you are granted a Medal of "
    "Valor by the King's herald."),
    DATA_COMPGEN(0x00504248, gArtifactEventAfterSavingAYoungBoyFrom, "After saving a young boy from a vicious pack of Wolves, you return him to his father's manor. "
    " The grateful nobleman awards you with a Medal of Courage."),
    DATA_COMPGEN(0x005042e4, gArtifactEventAfterFreeingAPrincessOfA, "After freeing a princess of a neighboring kingdom from the evil clutches of despicable "
    "slavers, she awards you with a Medal of Honor."),
    DATA_COMPGEN(0x0050436c, gArtifactEventRiddingTheCountrysideOfTheHideous, "Ridding the countryside of the hideous Minotaur who made a sport of eating noblemen's "
    "Knights, you are honored with the Medal of Distinction."),
    DATA_COMPGEN(0x005043fc, gArtifactEventYouStumbleUponAMedalLying, "You stumble upon a medal lying alongside the empty road.  Adding the medal to your inventory, "
    "you become aware that you have acquired the undesirable Fizbin of Misfortune, greatly "
    "decreasing your army's morale."),
    DATA_COMPGEN(0x005044d0, gArtifactEventDuringASuddenStormABolt, "During a sudden storm, a bolt of lightning strikes a tree, splitting it.  Inside the tree you "
    "find a mysterious mace."),
    DATA_COMPGEN(0x00504548, gArtifactEventYouEncounterTheInfamousBlackKnight, "You encounter the infamous Black Knight!  After a grueling duel ending in a draw, the Knight, "
    "out of respect, offers you a pair of armored gauntlets."),
    DATA_COMPGEN(0x005045e0, gArtifactEventAGlintOfGoldenLightCatches, "A glint of golden light catches your eye.  Upon further investigation, you find a golden helm "
    "hidden under a bush."),
    DATA_COMPGEN(0x00504654, gArtifactEventAClumsyGiantHasKilledHimself, "A clumsy Giant has killed himself with his own flail.  Knowing your superior skill with this "
    "weapon, you confidently remove the spectacular flail from the fallen Giant."),
    DATA_COMPGEN(0x00504700, gArtifactEventWalkingThroughTheRuinsOfAn, "Walking through the ruins of an ancient walled city, you find the instrument of the city's "
    "destruction, an elaborately crafted ballista."),
    DATA_COMPGEN(0x0050478c, gArtifactEventAStoneStatueOfAWarrior, "A stone statue of a warrior holds a silver shield.  As you remove the shield, the statue "
    "crumbles into dust."),
    DATA_COMPGEN(0x005047fc, gArtifactEventAsYouAreWalkingAlongA, "As you are walking along a narrow path, a nearby bush suddenly bursts into flames.  Before "
    "your eyes the flames become the image of a beautiful woman.  She holds out a magnificent "
    "sword to you."),
    DATA_COMPGEN(0x005048c0, gArtifactEventYouSeeASilverAxeEmbedded, "You see a silver axe embedded deeply in the ground.  After several unsuccessful attempts by "
    "your army to remove the axe, you tightly grip the handle of the axe and effortlessly pull it "
    "free."),
    DATA_COMPGEN(0x00504980, gArtifactEventAGangOfRoguesIsSifting, "A gang of Rogues is sifting through the possessions of dead warriors.  Scaring off the "
    "scavengers, you note the Rogues had overlooked a beautiful breastplate."),
    DATA_COMPGEN(0x00504a20, gArtifactEventBeforeYouAppearsALevitatingGlass, "Before you appears a levitating glass case with a scroll, perched upon a bed of crimson "
    "velvet.  At your touch, the lid opens and the scroll floats into your awaiting hands."),
    DATA_COMPGEN(0x00504ad0, gArtifactEventVisitingALocalWisemanYouExplain, "Visiting a local wiseman, you explain the intent of your journey.  He reaches into a sack and "
    "withdraws a yellowed scroll and hands it to you."),
    DATA_COMPGEN(0x00504b60, gArtifactEventYouComeAcrossTheRemainsOf, "You come across the remains of an ancient Druid.  Bones, yellowed with age, peer from the "
    "ragged folds of her robe.  Searching the robe, you discover a scroll hidden in the folds."),
    DATA_COMPGEN(0x00504c14, gArtifactEventMangledBonesYellowedWithAgePeer, "Mangled bones, yellowed with age, peer from the ragged folds of a dead Druid's robe.  "
    "Searching the robe, you discover a scroll hidden within."),
    DATA_COMPGEN(0x00504ca4, gArtifactEventALittleLeprechaunDancesGleefullyAround, "A little leprechaun dances gleefully around a magic sack.  Seeing you approach, he stops in "
    "mid-stride.  The little man screams and stamps his foot ferociously, vanishing into thin air. "
    " Remembering the old leprechaun saying 'Finders Keepers', you grab the sack and leave."),
    DATA_COMPGEN(0x00504db8, gArtifactEventANoblewomanSeparatedFromHerTraveling, "A noblewoman, separated from her traveling companions, asks for your help.  After escorting "
    "her home, she rewards you with a bag filled with gold."),
    DATA_COMPGEN(0x00504e4c, gArtifactEventInYourTravelsYouFindA, "In your travels, you find a leather purse filled with gold that once belonged to a great "
    "warrior king who had the ability to transform any inanimate object into gold."),
    DATA_COMPGEN(0x00504ef4, gArtifactEventANomadTraderSeeksProtectionFrom, "A Nomad trader seeks protection from a tribe of Goblins.  For your assistance, he gives you a "
    "finely crafted pair of boots made from the softest leather.  Looking closely, you see "
    "fascinating ancient carvings engraved on the leather."),
    DATA_COMPGEN(0x00504fe0, gArtifactEventDiscoveringAPairOfBeautifullyBeaded, "Discovering a pair of beautifully beaded boots made from the finest and softest leather, you "
    "thank the anonymous donor and add the boots to your inventory."),
    DATA_COMPGEN(0x0050507c, gArtifactEventATravelingMerchantOffersYouA, "A traveling merchant offers you a rabbit's foot, made of gleaming silver fur, for safe "
    "passage.  The merchant explains the charm will increase your luck in combat."),
    DATA_COMPGEN(0x00505120, gArtifactEventAnEnsnaredUnicornWhinniesInFright, "An ensnared Unicorn whinnies in fright.  Murmuring soothing words, you set her free.  "
    "Snorting and stamping her front hoof once, she gallops off.  Looking down you see a golden "
    "horseshoe."),
    DATA_COMPGEN(0x005051dc, gArtifactEventYouHaveCapturedAMischievousImp, "You have captured a mischievous imp who has been terrorizing the region.  In exchange for his "
    "release, he rewards you with a magical coin."),
    DATA_COMPGEN(0x00505268, gArtifactEventInTheMiddleOfAPatch, "In the middle of a patch of dead and dry vegetation, to your surprise you find a healthy "
    "green four-leaf clover."),
    DATA_COMPGEN(0x005052dc, gArtifactEventAnOldManClaimingToBe, "An old man claiming to be an inventor asks you to try his latest invention.  He then hands "
    "you a compass."),
    DATA_COMPGEN(0x00505348, gArtifactEventAnOldSeaCaptainIsBeing, "An old sea captain is being tortured by Ogres.  You save him, and in return he rewards you "
    "with a wondrous instrument to measure the distance of a star."),
    DATA_COMPGEN(0x005053e4, gArtifactEventWhileVenturingIntoADecrepitHut, "While venturing into a decrepit hut you find the Skeleton of a long dead witch.  "
    "Investigation of the remains reveals a glass eye rolling around inside an empty skull."),
    DATA_COMPGEN(0x0050548c, gArtifactEventASurpriseTurnInTheLandscape, "A surprise turn in the landscape finds you in the midst of a grisly scene:  Vultures picking "
    "at the aftermath of a terrible battle.  Your cursory search of the remains turns up an "
    "enchanted hourglass."),
    DATA_COMPGEN(0x00505558, gArtifactEventInRewardForHelpingHisCart, "In reward for helping his cart out of a ditch, a traveling potion salesman gives you a "
    "\"magic\" gold watch.  Unbeknownst to him, the watch really is magical."),
    DATA_COMPGEN(0x005055f8, gArtifactEventABriefStopAtAnImprobable, "A brief stop at an improbable rural inn yields an exchange of money, tales, and accidentally, "
    "luggage.  You find a magical skullcap in your new backpack."),
    DATA_COMPGEN(0x00505694, gArtifactEventRespondingToThePanickedCriesOf, "Responding to the panicked cries of a damsel in distress, you discover a young woman fleeing "
    "from a hungry bear.  You slay the beast in the nick of time, and the grateful Sorceress "
    "weaves a magic cloak from the bear's hide."),
    DATA_COMPGEN(0x00505774, gArtifactEventYouVeComeUponAFight, "You've come upon a fight between a Necromancer and a Paladin.  The Necromancer blasts the "
    "Paladin with a fire bolt, bringing him to his knees.  Acting quickly, you slay the evil one "
    "before the final blow.  The grateful Paladin gives you the fire cloak that saved him."),
    DATA_COMPGEN(0x00505880, gArtifactEventATravelingTinkerInNeedOf, "A traveling tinker in need of supplies offers you a helm with a thunderbolt design on its top "
    "in exchange for food and water.  Curious, you accept, and later find out that the helm is "
    "magical."),
    DATA_COMPGEN(0x00505944, gArtifactEventAnIcicleWithstandingTheFullHeat, "An icicle withstanding the full heat of the noonday sun attracts your attention.  Intrigued, "
    "you break it off, and find that it does not melt in your hand."),
    DATA_COMPGEN(0x005059e0, gArtifactEventYourWanderingsBringYouIntoContact, "Your wanderings bring you into contact with a tribe of ape-like beings using a magical lava "
    "rock that never cools to light their fires.  You take pity on them and teach them to make "
    "fire with sticks.  Believing you to be a god, the apes give you their rock."),
    DATA_COMPGEN(0x00505ae4, gArtifactEventWhileWaitingOutAStormA, "While waiting out a storm, a lighting bolt strikes a nearby cottage's lightning rod, which "
    "melts and falls to the ground.  The tip of the rod, however, survives intact and makes your "
    "hair stand on end when you touch it.  Hmm..."),
    DATA_COMPGEN(0x00505bc8, gArtifactEventYouVeFoundAnOddlyShaped, "You've found an oddly shaped ring on the finger of a long dead traveler.  The ring looks like "
    "a snake biting its own tail."),
    DATA_COMPGEN(0x00505c44, gArtifactEventAFierceWindstormRevealsTheEntrance, "A fierce windstorm reveals the entrance to a buried tomb.  Your investigation reveals that "
    "the tomb has already been looted, but the thieves overlooked an ankh on a silver chain in the "
    "dark."),
    DATA_COMPGEN(0x00505d04, gArtifactEventYouComeAcrossAConjurerWho, "You come across a conjurer who begs to accompany you and your army awhile for safety.  You "
    "agree, and he offers as payment a copy of the book of the elements."),
    DATA_COMPGEN(0x00505da4, gArtifactEventWhilePausingToRestYouNotice, "While pausing to rest, you notice a bobcat climbing a short tree to get at a crow's nest.  On "
    "impulse, you climb the tree yourself and scare off the cat.  When you look in the nest, you "
    "find a collection of shiny stones and a ring."),
    DATA_COMPGEN(0x00505e8c, gArtifactEventInYourWanderingsYouComeAcross, "In your wanderings you come across a hermit living in a small, tidy hut.  Impressed with your "
    "mission, he takes time out from his meditations to bless and give you a charm against curses."),
    DATA_COMPGEN(0x00505f48, gArtifactEventRespondingToCriesForHelpYou, "Responding to cries for help, you find river Sprites making a sport of dunking an old man.  "
    "Feeling vengeful, you rescue the man and drag a Sprite onto dry land for awhile.  The Sprite, "
    "uncomfortable in the air, gives you a magic pendant to let him go."),
    DATA_COMPGEN(0x00506048, gArtifactEventABriefRoadsideEncounterWithA, "A brief roadside encounter with a small caravan and a game of knucklebones wins a magic "
    "pendant.  Its former owner says that it protects from Necromancers' death spells."),
    DATA_COMPGEN(0x005060f4, gArtifactEventTheSoundsOfCombatDrawYou, "The sounds of combat draw you to the scene of a fight between an old Barbarian and an "
    "eight-headed Hydra.  Your timely intervention swings the battle in favor of the man, and he "
    "rewards you with a pendant he used to use to calm his mind for battle."),
    DATA_COMPGEN(0x005061f0, gArtifactEventYouComeUponAVeryOld, "You come upon a very old woman, long blind from cataracts and dying alone.  You tend to her "
    "final needs and promise a proper burial.  Grateful, she gives you a magic pendant emblazoned "
    "with a stylized eye.  It lets you see with your eyes closed."),
    DATA_COMPGEN(0x005062e8, gArtifactEventYouComeAcrossAGolemWearing, "You come across a golem wearing a glowing pendant and blocking your way.   Acting on a hunch, "
    "you cut the pendant from its neck.  Deprived of its power source, the golem breaks down, "
    "leaving you with the magical pendant."),
    DATA_COMPGEN(0x005063c8, gArtifactEventAQuickAndDeadlyBattleWith, "A quick and deadly battle with a Necromancer wins you his magical pendant.  Later, a Wizard "
    "tells you that the pendant protects undead under your control from holy word spells."),
    DATA_COMPGEN(0x0050647c, gArtifactEventYouMeetAnOldWizardFriend, "You meet an old Wizard friend of yours traveling in the opposite direction.  He presents  you "
    "with a gift:  A wand that prevents the use of the dispel magic spell on your allies."),
    DATA_COMPGEN(0x00506530, gArtifactEventAChanceMeetingWithAFamous, "A chance meeting with a famous Archer finds you in a game of knucklebones pitting his bow "
    "against your horse.  You win."),
    DATA_COMPGEN(0x005065a8, gArtifactEventAMerchantFromFarAwayLands, "A merchant from far away lands trades you a new invention of his people for traveling "
    "supplies.  It makes distant objects appear closer, and he calls it...\n\na telescope."),
    DATA_COMPGEN(0x00506654, gArtifactEventYouPauseToHelpADiplomat, "You pause to help a diplomat with a broken axle fix his problem.  In gratitude, he gives you "
    "a writing quill with magical properties which he says will \"help people see things your "
    "way\"."),
    DATA_COMPGEN(0x00506710, gArtifactEventYouSeeAWizardFleeingFrom, "You see a Wizard fleeing from a Griffin and riding like the wind.  The Wizard opens a portal "
    "and rides through, getting his hat knocked off by the edge of the gate.  The Griffin follows; "
    "the gate closes.  You pick the hat up, dust it off, and put it on."),
    DATA_COMPGEN(0x00506810, gArtifactEventYouFindASmallTreeThat, "You find a small tree that closely resembles the great Warlock Carnauth with a ring around "
    "one of its twigs.  Scraps of clothing and rotting leather lead you to suspect that it IS "
    "Carnauth, transformed.  Since you can't help him, you take the magic ring."),
    DATA_COMPGEN(0x00506910, gArtifactEventAnAmmunitionCartInTheMiddle, "An ammunition cart in the middle of an old battlefield catches your eye.  Inspection shows it "
    "to be in good working order, so  you take it along."),
    DATA_COMPGEN(0x005069a4, gArtifactEventYourBigSpendingHabitsHaveEarned, "Your big spending habits have earned you a massive tax bill that you can't hope to pay.  The "
    "tax man takes pity and agrees to only take 250 gold a day from your account for life.  Check "
    "here if you want one dollar to go to the presidential campaign election fund."),
    DATA_COMPGEN(0x00506aac, gArtifactEventYourLootingOfTheGraveOf, "Your looting of the grave of Sinfilas Gardolad, the famous shapeshifting Warlock, unearths "
    "his fabled mask.  Trembling, you put it on and it twists your visage into an awful grimace!  "
    "Oh no!  It's actually the hideous mask of Gromluck Greene, and you are stuck with it."),
    DATA_COMPGEN(0x00506bbc, gArtifactEventYouVisitAnAlchemistWhoUpon, "You visit an alchemist who, upon seeing your army, is swayed by the righteousness of your "
    "cause.  The newly loyal subject gives you his endless pouch of sulfur to help with the war "
    "effort."),
    DATA_COMPGEN(0x00506c7c, gArtifactEventABriefStopAtAHastily, "A brief stop at a hastily abandoned Wizard's tower turns up a magical vial of mercury that "
    "always has a little left on the bottom.  Recognizing a treasure when you see one, you cap it "
    "and slip it in your pocket."),
    DATA_COMPGEN(0x00506d50, gArtifactEventAShortRainstormBringsForthA, "A short rainstorm brings forth a rainbow...and you can see the end of it.  Riding quickly, "
    "you seize the pot of gold you find there.  The leprechaun who owns it, unable to stop you "
    "from taking it, offers an endless pouch of gems for the return of his gold.  You accept."),
    DATA_COMPGEN(0x00506e60, gArtifactEventPausingToRestAndLightA, "Pausing to rest and light a cook fire, you pull wood out of a nearby pile of dead wood.  As "
    "you keep pulling wood from the pile, you notice that it doesn't shrink.  You realize to your "
    "delight that the wood is enchanted, so you take it along."),
    DATA_COMPGEN(0x00506f54, gArtifactEventYouVeFoundAGoblinWeapon, "You've found a Goblin weapon smithy making weapons for use against humans.  With a tremendous "
    "yell you and your army descend upon their camp and drive them away.  A search finds a magic "
    "ore cart that never runs out of iron."),
    DATA_COMPGEN(0x00507034, gArtifactEventTakingShelterFromAStormIn, "Taking shelter from a storm in a small cave,  you notice a small patch of crystal in one "
    "corner.  Curious, you break a piece off and notice that the original crystal grows the lost "
    "piece back.  You decide to stuff the entire patch into a pouch and take it with you."),
    DATA_COMPGEN(0x00507140, gArtifactEventYourArmyIsAmbushedByA, "Your army is ambushed by a small tribe of wild (and none too bright) Orcs.  You fend them off "
    "easily and the survivors flee in all directions.  One of the Orcs was wearing a polished "
    "spiked helm.  Figuring it will make a good souvenir, you take it."),
    DATA_COMPGEN(0x0050723c, gArtifactEventYouComeUponABridgeSpanning, "You come upon a bridge spanning a dry gully.  Before you can cross, a Troll steps out from "
    "under the bridge and demands payment before it will permit you to pass.  You refuse, and the "
    "Troll charges, forcing you to slay it.  You take its spiked shield as a trophy."),
    DATA_COMPGEN(0x00507344, gArtifactEventAWalkAcrossADrySaltwater, "A walk across a dry saltwater lake bed yields an unlikely prize:  A white pearl amidst "
    "shattered shells and debris."),
    DATA_COMPGEN(0x005073b8, gArtifactEventRumorsOfAGriffinOfUnusual, "Rumors of a Griffin of unusual size preying upon the countryside lead you to its cave lair.  "
    "A quick, brutal fight dispatches the beast, and a search of its foul nest turns up a huge "
    "black pearl."),
    DATA_COMPGEN(0x0050ca44, gArtifactEventEmptyString9, ""),
    DATA_COMPGEN(0x00507480, gArtifactEventERRORArtifactEvent82, "ERROR : Artifact event 82."),
    DATA_COMPGEN(0x0050749c, gArtifactEventERRORArtifactEvent83, "ERROR : Artifact event 83."),
    DATA_COMPGEN(0x005074b8, gArtifactEventERRORArtifactEvent84, "ERROR : Artifact event 84."),
    DATA_COMPGEN(0x005074d4, gArtifactEventERRORArtifactEvent85, "ERROR : Artifact event 85."),
    DATA_COMPGEN(0x005074f0, gArtifactEventYouFindAnElaborateContainerWhich, "You find an elaborate container which houses an old vellum scroll. The runes on the container "
    "are very old, and the artistry with which it was put together is stunning. As you pull the "
    "scroll out, you feel imbued with magical power."),
    DATA_COMPGEN(0x005075dc, gArtifactEventOneOfTheLessIntelligentMembers, "One of the less intelligent members of your party picks up an arm off of the ground.  Despite "
    "its missing a body, it is still moving.  Your troops find the dismembered arm repulsive, but "
    "you cannot bring yourself to drop it: it seems to hold some sort of magical power that "
    "influences your decision making."),
    DATA_COMPGEN(0x00507710, gArtifactEventYouComeUponASignIt, "You come upon a sign.  It reads: \"Here lies the body of Anduran.  Bow and swear fealty, and "
    "you shall be rewarded.\"  You decide to do as it says.  As you stand up, you feel a coldness "
    "against your skin.  Looking down, you find that you are suddenly wearing a gleaming, ornate "
    "breastplate."),
    DATA_COMPGEN(0x00507834, gArtifactEventAKindlySorceressThinksThatYour, "A kindly Sorceress thinks that your army's defenses could use a magical boost.  She offers to "
    "enchant the Broach that you wear on your cloak, and you accept."),
    DATA_COMPGEN(0x005078d4, gArtifactEventOutOfPityForAPoor, "Out of pity for a poor peasant, you purchase a chest of old junk they are hawking for too "
    "much gold. Later, as you search through it, you find it contains the 3 pieces of the "
    "legendary battle garb of Anduran!"),
    DATA_COMPGEN(0x005079a8, gArtifactEventYouComeUponACaravanOf, "You come upon a caravan of gypsies who are feasting and fortifying their bodies with mead.  "
    "They call you forward and say \"If you prove that you can dance the Rama-Buta, we will reward "
    "you.\"  You don't know it, but try anyway.  They laugh hysterically, but admire your bravery, "
    "giving you a Crystal Ball."),
    DATA_COMPGEN(0x00507adc, gArtifactEventYouEnterARecentlyBurnedGlade, "You enter a recently burned glade and come upon a Fire Elemental sitting atop a rock.  It "
    "looks up, its flaming face contorted in a look of severe pain.  It then tosses a glowing "
    "object at you.  You put up your hands to block it, but it passes right through them and sears "
    "itself into your chest."),
    DATA_COMPGEN(0x00507c08, gArtifactEventSuddenlyABitingColdnessEngulfsYour, "Suddenly, a biting coldness engulfs your body.  You seize up, falling from your horse.  The "
    "pain subsides, but you still feel as if your chest is frozen.  As you pick yourself up off of "
    "the ground, you hear hearty laughter.  You turn around just in time to see a Frost Giant run "
    "off into the woods and disappear."),
    DATA_COMPGEN(0x00507d44, gArtifactEventYouSpyAGleamingObjectPoking, "You spy a gleaming object poking up out of the ground.   You send a member of your party over "
    "to investigate.  He comes back with a golden helmet in his hands.  You realize that it must "
    "be the helmet of the legendary Anduran, the only man who was known to wear solid gold armor."),
    DATA_COMPGEN(0x00507e5c, gArtifactEventYouComeUponABattleWhere, "You come upon a battle where a Paladin has been mortally wounded by a group of Zombies.  He "
    "asks you to take his hammer and finish what he started.  As you pick it up, it begins to hum, "
    "and then everything becomes a blur.  The Zombies lie dead, the hammer dripping with blood.  "
    "You strap it to your belt."),
    DATA_COMPGEN(0x00507f90, gArtifactEventUponCrestingASmallHillYou, "Upon cresting a small hill, you come upon a ridiculous looking sight.  A Sprite is attempting "
    "to carry a Scepter that is almost as big as it is.  Trying not to laugh, you ask, \"Need "
    "help?\"  The Sprite glares at you and answers: \"You think this is funny?  Fine.  You can "
    "carry it.  I much prefer flying anyway.\""),
    DATA_COMPGEN(0x005080c8, gArtifactEventAnOldSeamanTellsYouA, "An old seaman tells you a tale of an enchanted masthead that he used in his youth to rally "
    "his crew during times of trouble.  He then hands you a faded map that shows where he hid it.  "
    "After much exploring, you find it stashed underneath a nearby dock."),
    DATA_COMPGEN(0x005081c8, gArtifactEventYouStopToHelpAPeasant, "You stop to help a Peasant catch a runaway mare.  To show his gratitude, he hands you a tiny "
    "sphere.  As soon as you grasp it, you feel the magical energy drain from your limbs..."),
    DATA_COMPGEN(0x0050827c, gArtifactEventWhileOutScaringUpGameYour, "While out scaring up game, your troops find a mysterious staff levitating about three feet "
    "off of the ground.  They hand it to you, and you notice an inscription.  It reads: \"Brains "
    "best brawn and magic beats might.  Heed my words, and you'll win every fight.\""),
    DATA_COMPGEN(0x00508384, gArtifactEventAFormerCaptainOfTheGuard, "A former Captain of the Guard admires your quest and gives you the enchanted Sword Breaker "
    "that he relied on during his tour of duty."),
    DATA_COMPGEN(0x0050840c, gArtifactEventATrollStopsYouAndSays, "A Troll stops you and says: \"Pay me 5,000 gold, or the Sword of Anduran will slay you where "
    "you stand.\"  You refuse.  The troll grabs the sword hanging from its belt, screams in pain, "
    "and runs away.  Picking up the fabled sword, you give thanks that half-witted Trolls tend to "
    "grab the wrong end of sharp objects."),
    DATA_COMPGEN(0x00508548, gArtifactEventADirtyShovelHasBeenThrust, "A dirty shovel has been thrust into a dirt mound nearby. Upon investigation, you discover it "
    "to be the enchanted shovel of the Gravediggers, long thought lost by mortals."),
    NULL
};
DATA(0x004fe790) char* gStatNames[HERO_PRIMARY_STAT_COUNT] =
    {DATA_COMPGEN(0x005085f4, gStatNamesAttackSkill, "Attack Skill"), DATA_COMPGEN(0x00508604, gStatNamesDefenseSkill, "Defense Skill"), DATA_COMPGEN(0x00508614, gStatNamesSpellPower, "Spell Power"), DATA_COMPGEN(0x00508620, gStatNamesKnowledge, "Knowledge")};
DATA(0x004fe7a0) char* gStatDesc[HERO_PRIMARY_STAT_COUNT] = {
    DATA_COMPGEN(0x0050862c, gStatDescAttackSkillYourAttackSkillIs, "{Attack Skill}\n\nYour attack skill is a bonus added to each creature's attack skill."),
    DATA_COMPGEN(0x00508680, gStatDescDefenseSkillYourDefenseSkillIs, "{Defense Skill}\n\nYour defense skill is a bonus added to each creature's defense skill."),
    DATA_COMPGEN(0x005086d8, gStatDescSpellPowerYourSpellPowerDetermines, "{Spell Power}\n\nYour spell power determines the length or power of a spell."),
    DATA_COMPGEN(0x00508724, gStatDescKnowledgeYourKnowledgeDeterminesHowMany, "{Knowledge}\n\nYour knowledge determines how many spell points your hero may have.  Under "
    "normal cirumstances, a hero is limited to 10 spell points per level of knowledge.")
};
DATA(0x004fe7b0) char* gAlignmentNames[KB_ALIGNMENT_NAME_COUNT] =
    {DATA_COMPGEN(0x005087d0, gAlignmentNamesKnight, "Knight"), DATA_COMPGEN(0x005087d8, gAlignmentNamesBarbarian, "Barbarian"), DATA_COMPGEN(0x005087e4, gAlignmentNamesSorceress, "Sorceress"), DATA_COMPGEN(0x005087f0, gAlignmentNamesWarlock, "Warlock"), DATA_COMPGEN(0x005087f8, gAlignmentNamesWizard, "Wizard"), DATA_COMPGEN(0x00508800, gAlignmentNamesNecromancer, "Necromancer"), DATA_COMPGEN(0x0050880c, gAlignmentNamesMultiple, "Multiple"), DATA_COMPGEN(0x00508818, gAlignmentNamesRandom, "Random")};
DATA(0x004fe7d0) char* gArmyShortNames[IDX(CREATURE_COUNT)] = {
    DATA_COMPGEN(0x00508820, gArmyShortNamesPeasn, "peasn"), DATA_COMPGEN(0x00508828, gArmyShortNamesArchr, "archr"), DATA_COMPGEN(0x00508830, gArmyShortNamesArch2, "arch2"), DATA_COMPGEN(0x00508838, gArmyShortNamesPikmn, "pikmn"), DATA_COMPGEN(0x00508840, gArmyShortNamesPikm2, "pikm2"), DATA_COMPGEN(0x00508848, gArmyShortNamesSwman, "swman"), DATA_COMPGEN(0x00508850, gArmyShortNamesSwma2, "swma2"), DATA_COMPGEN(0x00508858, gArmyShortNamesCvlry, "cvlry"), DATA_COMPGEN(0x00508860, gArmyShortNamesCvlr2, "cvlr2"), DATA_COMPGEN(0x00508868, gArmyShortNamesPaldn, "paldn"),
    DATA_COMPGEN(0x00508870, gArmyShortNamesPald2, "pald2"), DATA_COMPGEN(0x00508878, gArmyShortNamesGobln, "gobln"), DATA_COMPGEN(0x00508880, gArmyShortNamesOrc, "orc__"), DATA_COMPGEN(0x00508888, gArmyShortNamesOrc2, "orc_2"), DATA_COMPGEN(0x00508890, gArmyShortNamesWolf, "Wolf_"), DATA_COMPGEN(0x00508898, gArmyShortNamesOgre, "Ogre_"), DATA_COMPGEN(0x005088a0, gArmyShortNamesOgre2, "Ogre2"), DATA_COMPGEN(0x005088a8, gArmyShortNamesTroll, "Troll"), DATA_COMPGEN(0x005088b0, gArmyShortNamesTrol2, "trol2"), DATA_COMPGEN(0x005088b8, gArmyShortNamesCyclp, "cyclp"),
    DATA_COMPGEN(0x005088c0, gArmyShortNamesSprit, "sprit"), DATA_COMPGEN(0x005088c8, gArmyShortNamesDwarf, "Dwarf"), DATA_COMPGEN(0x005088d0, gArmyShortNamesDwar2, "dwar2"), DATA_COMPGEN(0x005088d8, gArmyShortNamesElf, "elf__"), DATA_COMPGEN(0x005088e0, gArmyShortNamesElf2, "elf_2"), DATA_COMPGEN(0x005088e8, gArmyShortNamesDruid, "druid"), DATA_COMPGEN(0x005088f0, gArmyShortNamesDrui2, "drui2"), DATA_COMPGEN(0x005088f8, gArmyShortNamesUncrn, "uncrn"), DATA_COMPGEN(0x00508900, gArmyShortNamesPhoen, "phoen"), DATA_COMPGEN(0x00508908, gArmyShortNamesCentr, "centr"),
    DATA_COMPGEN(0x00508910, gArmyShortNamesGargl, "gargl"), DATA_COMPGEN(0x00508918, gArmyShortNamesGriff, "griff"), DATA_COMPGEN(0x00508920, gArmyShortNamesMintr, "mintr"), DATA_COMPGEN(0x00508928, gArmyShortNamesMint2, "mint2"), DATA_COMPGEN(0x00508930, gArmyShortNamesHydra, "Hydra"), DATA_COMPGEN(0x00508938, gArmyShortNamesDragn, "dragn"), DATA_COMPGEN(0x00508940, gArmyShortNamesDrag2, "drag2"), DATA_COMPGEN(0x00508948, gArmyShortNamesDrag3, "drag3"), DATA_COMPGEN(0x00508950, gArmyShortNamesHlflg, "hlflg"), DATA_COMPGEN(0x00508958, gArmyShortNamesBoar, "Boar_"),
    DATA_COMPGEN(0x00508960, gArmyShortNamesIrong, "irong"), DATA_COMPGEN(0x00508968, gArmyShortNamesIron2, "iron2"), DATA_COMPGEN(0x00508970, gArmyShortNamesRoc, "roc__"), DATA_COMPGEN(0x00508978, gArmyShortNamesArchm, "archm"), DATA_COMPGEN(0x00508980, gArmyShortNamesArch22, "arch2"), DATA_COMPGEN(0x00508988, gArmyShortNamesTitan, "titan"), DATA_COMPGEN(0x00508990, gArmyShortNamesTita2, "tita2"), DATA_COMPGEN(0x00508998, gArmyShortNamesSkel, "skel_"), DATA_COMPGEN(0x005089a0, gArmyShortNamesZomb, "zomb_"), DATA_COMPGEN(0x005089a8, gArmyShortNamesZomb2, "zomb2"),
    DATA_COMPGEN(0x005089b0, gArmyShortNamesMummy, "Mummy"), DATA_COMPGEN(0x005089b8, gArmyShortNamesMumm2, "mumm2"), DATA_COMPGEN(0x005089c0, gArmyShortNamesVampr, "vampr"), DATA_COMPGEN(0x005089c8, gArmyShortNamesVamp2, "vamp2"), DATA_COMPGEN(0x005089d0, gArmyShortNamesLich, "lich_"), DATA_COMPGEN(0x005089d8, gArmyShortNamesLich2, "lich2"), DATA_COMPGEN(0x005089e0, gArmyShortNamesBoned, "boned"), DATA_COMPGEN(0x005089e8, gArmyShortNamesRogue, "Rogue"), DATA_COMPGEN(0x005089f0, gArmyShortNamesNomad, "Nomad"), DATA_COMPGEN(0x005089f8, gArmyShortNamesGhost, "Ghost"),
    DATA_COMPGEN(0x00508a00, gArmyShortNamesGenie, "Genie"), DATA_COMPGEN(0x00508a08, gArmyShortNamesMedus, "medus"), DATA_COMPGEN(0x00508a10, gArmyShortNamesEleme, "eleme"), DATA_COMPGEN(0x00508a18, gArmyShortNamesElema, "elema"), DATA_COMPGEN(0x00508a20, gArmyShortNamesElemf, "elemf"), DATA_COMPGEN(0x00508a28, gArmyShortNamesElemw, "elemw")
};
DATA(0x004fe8d8) char* gArmyNames[IDX(CREATURE_COUNT)] = {
    DATA_COMPGEN(0x00508a30, gArmyNamesPeasant, "peasant"),
    DATA_COMPGEN(0x00508a38, gArmyNamesArcher, "archer"),
    DATA_COMPGEN(0x00508a40, gArmyNamesRanger, "ranger"),
    DATA_COMPGEN(0x00508a48, gArmyNamesPikeman, "pikeman"),
    DATA_COMPGEN(0x00508a50, gArmyNamesVeteranPikeman, "veteran pikeman"),
    DATA_COMPGEN(0x00508a60, gArmyNamesSwordsman, "swordsman"),
    DATA_COMPGEN(0x00508a6c, gArmyNamesMasterSwordsman, "master swordsman"),
    DATA_COMPGEN(0x00508a80, gArmyNamesCavalry, "cavalry"),
    DATA_COMPGEN(0x00508a88, gArmyNamesChampion, "champion"),
    DATA_COMPGEN(0x00508a94, gArmyNamesPaladin, "paladin"),
    DATA_COMPGEN(0x00508a9c, gArmyNamesCrusader, "crusader"),
    DATA_COMPGEN(0x00508aa8, gArmyNamesGoblin, "goblin"),
    DATA_COMPGEN(0x00508ab0, gArmyNamesOrc, "orc"),
    DATA_COMPGEN(0x00508ab4, gArmyNamesOrcChief, "orc chief"),
    "wolf",
    "ogre",
    DATA_COMPGEN(0x00508ad0, gArmyNamesOgreLord, "ogre lord"),
    DATA_COMPGEN(0x00508adc, gArmyNamesTroll, "troll"),
    DATA_COMPGEN(0x00508ae4, gArmyNamesWarTroll, "war troll"),
    DATA_COMPGEN(0x00508af0, gArmyNamesCyclops, "cyclops"),
    DATA_COMPGEN(0x00508af8, gArmyNamesSprite, "sprite"),
    DATA_COMPGEN(0x00508b00, gArmyNamesDwarf, "dwarf"),
    DATA_COMPGEN(0x00508b08, gArmyNamesBattleDwarf, "battle dwarf"),
    DATA_COMPGEN(0x00508b18, gArmyNamesElf, "elf"),
    DATA_COMPGEN(0x00508b1c, gArmyNamesGrandElf, "grand elf"),
    DATA_COMPGEN(0x00508b28, gArmyNamesDruid, "druid"),
    DATA_COMPGEN(0x00508b30, gArmyNamesGreaterDruid, "greater druid"),
    DATA_COMPGEN(0x00508b40, gArmyNamesUnicorn, "unicorn"),
    DATA_COMPGEN(0x00508b48, gArmyNamesPhoenix, "phoenix"),
    DATA_COMPGEN(0x00508b50, gArmyNamesCentaur, "centaur"),
    DATA_COMPGEN(0x00508b58, gArmyNamesGargoyle, "gargoyle"),
    DATA_COMPGEN(0x00508b64, gArmyNamesGriffin, "griffin"),
    DATA_COMPGEN(0x00508b6c, gArmyNamesMinotaur, "minotaur"),
    DATA_COMPGEN(0x00508b78, gArmyNamesMinotaurKing, "minotaur king"),
    DATA_COMPGEN(0x00508b88, gArmyNamesHydra, "hydra"),
    DATA_COMPGEN(0x00508b90, gArmyNamesGreenDragon, "green dragon"),
    DATA_COMPGEN(0x00508ba0, gArmyNamesRedDragon, "red dragon"),
    DATA_COMPGEN(0x00508bac, gArmyNamesBlackDragon, "black dragon"),
    DATA_COMPGEN(0x00508bbc, gArmyNamesHalfling, "halfling"),
    "boar",
    DATA_COMPGEN(0x00508bd0, gArmyNamesIronGolem, "iron golem"),
    DATA_COMPGEN(0x00508bdc, gArmyNamesSteelGolem, "steel golem"),
    DATA_COMPGEN(0x00508be8, gArmyNamesRoc, "roc"),
    "mage",
    DATA_COMPGEN(0x00508bf4, gArmyNamesArchmage, "archmage"),
    DATA_COMPGEN(0x00508c00, gArmyNamesGiant, "giant"),
    DATA_COMPGEN(0x00508c08, gArmyNamesTitan, "titan"),
    DATA_COMPGEN(0x00508c10, gArmyNamesSkeleton, "skeleton"),
    DATA_COMPGEN(0x00508c1c, gArmyNamesZombie, "zombie"),
    DATA_COMPGEN(0x00508c24, gArmyNamesMutantZombie, "mutant zombie"),
    DATA_COMPGEN(0x00508c34, gArmyNamesMummy, "mummy"),
    DATA_COMPGEN(0x00508c3c, gArmyNamesRoyalMummy, "royal mummy"),
    DATA_COMPGEN(0x00508c48, gArmyNamesVampire, "vampire"),
    DATA_COMPGEN(0x00508c50, gArmyNamesVampireLord, "vampire lord"),
    "lich",
    DATA_COMPGEN(0x00508c68, gArmyNamesPowerLich, "power lich"),
    DATA_COMPGEN(0x00508c74, gArmyNamesBoneDragon, "bone dragon"),
    DATA_COMPGEN(0x00508c80, gArmyNamesRogue, "rogue"),
    DATA_COMPGEN(0x00508c88, gArmyNamesNomad, "nomad"),
    DATA_COMPGEN(0x00508c90, gArmyNamesGhost, "ghost"),
    DATA_COMPGEN(0x00508c98, gArmyNamesGenie, "genie"),
    DATA_COMPGEN(0x00508ca0, gArmyNamesMedusa, "medusa"),
    DATA_COMPGEN(0x00508ca8, gArmyNamesEarthElemental, "earth elemental"),
    DATA_COMPGEN(0x00508cb8, gArmyNamesAirElemental, "air elemental"),
    DATA_COMPGEN(0x00508cc8, gArmyNamesFireElemental, "fire elemental"),
    DATA_COMPGEN(0x00508cd8, gArmyNamesWaterElemental, "water elemental")
};
DATA(0x004fe9e0) char* gArmyNamesPlural[IDX(CREATURE_COUNT)] = {
    DATA_COMPGEN(0x00508ce8, gArmyNamesPluralPeasants, "peasants"),
    DATA_COMPGEN(0x00508cf4, gArmyNamesPluralArchers, "archers"),
    DATA_COMPGEN(0x00508cfc, gArmyNamesPluralRangers, "rangers"),
    DATA_COMPGEN(0x00508d04, gArmyNamesPluralPikemen, "pikemen"),
    DATA_COMPGEN(0x00508d0c, gArmyNamesPluralVeteranPikemen, "veteran pikemen"),
    DATA_COMPGEN(0x00508d1c, gArmyNamesPluralSwordsmen, "swordsmen"),
    DATA_COMPGEN(0x00508d28, gArmyNamesPluralMasterSwordsmen, "master swordsmen"),
    DATA_COMPGEN(0x00508d3c, gArmyNamesPluralCavalries, "cavalries"),
    DATA_COMPGEN(0x00508d48, gArmyNamesPluralChampions, "champions"),
    DATA_COMPGEN(0x00508d54, gArmyNamesPluralPaladins, "paladins"),
    DATA_COMPGEN(0x00508d60, gArmyNamesPluralCrusaders, "crusaders"),
    DATA_COMPGEN(0x00508d6c, gArmyNamesPluralGoblins, "goblins"),
    DATA_COMPGEN(0x00508d74, gArmyNamesPluralOrcs, "orcs"),
    DATA_COMPGEN(0x00508d7c, gArmyNamesPluralOrcChiefs, "orc chiefs"),
    DATA_COMPGEN(0x00508d88, gArmyNamesPluralWolves, "wolves"),
    DATA_COMPGEN(0x00508d90, gArmyNamesPluralOgres, "ogres"),
    DATA_COMPGEN(0x00508d98, gArmyNamesPluralOgreLords, "ogre lords"),
    DATA_COMPGEN(0x00508da4, gArmyNamesPluralTrolls, "trolls"),
    DATA_COMPGEN(0x00508dac, gArmyNamesPluralWarTroll, "war troll"),
    DATA_COMPGEN(0x00508db8, gArmyNamesPluralCyclopes, "cyclopes"),
    DATA_COMPGEN(0x00508dc4, gArmyNamesPluralSprites, "sprites"),
    DATA_COMPGEN(0x00508dcc, gArmyNamesPluralDwarves, "dwarves"),
    DATA_COMPGEN(0x00508dd4, gArmyNamesPluralBattleDwarves, "battle dwarves"),
    DATA_COMPGEN(0x00508de4, gArmyNamesPluralElves, "elves"),
    DATA_COMPGEN(0x00508dec, gArmyNamesPluralGrandElves, "grand elves"),
    DATA_COMPGEN(0x00508df8, gArmyNamesPluralDruids, "druids"),
    DATA_COMPGEN(0x00508e00, gArmyNamesPluralGreaterDruids, "greater druids"),
    DATA_COMPGEN(0x00508e10, gArmyNamesPluralUnicorns, "unicorns"),
    DATA_COMPGEN(0x00508e1c, gArmyNamesPluralPhoenix, "phoenix"),
    DATA_COMPGEN(0x00508e24, gArmyNamesPluralCentaurs, "centaurs"),
    DATA_COMPGEN(0x00508e30, gArmyNamesPluralGargoyles, "gargoyles"),
    DATA_COMPGEN(0x00508e3c, gArmyNamesPluralGriffins, "griffins"),
    DATA_COMPGEN(0x00508e48, gArmyNamesPluralMinotaurs, "minotaurs"),
    DATA_COMPGEN(0x00508e54, gArmyNamesPluralMinotaurKings, "minotaur kings"),
    DATA_COMPGEN(0x00508e64, gArmyNamesPluralHydras, "hydras"),
    DATA_COMPGEN(0x00508e6c, gArmyNamesPluralGreenDragons, "green dragons"),
    DATA_COMPGEN(0x00508e7c, gArmyNamesPluralRedDragons, "red dragons"),
    DATA_COMPGEN(0x00508e88, gArmyNamesPluralBlackDragons, "black dragons"),
    DATA_COMPGEN(0x00508e98, gArmyNamesPluralHalflings, "halflings"),
    DATA_COMPGEN(0x00508ea4, gArmyNamesPluralBoars, "boars"),
    DATA_COMPGEN(0x00508eac, gArmyNamesPluralIronGolems, "iron golems"),
    DATA_COMPGEN(0x00508eb8, gArmyNamesPluralSteelGolems, "steel golems"),
    DATA_COMPGEN(0x00508ec8, gArmyNamesPluralRocs, "rocs"),
    DATA_COMPGEN(0x00508ed0, gArmyNamesPluralMagi, "magi"),
    DATA_COMPGEN(0x00508ed8, gArmyNamesPluralArchmagi, "archmagi"),
    DATA_COMPGEN(0x00508ee4, gArmyNamesPluralGiants, "giants"),
    DATA_COMPGEN(0x00508eec, gArmyNamesPluralTitans, "titans"),
    DATA_COMPGEN(0x00508ef4, gArmyNamesPluralSkeletons, "skeletons"),
    DATA_COMPGEN(0x00508f00, gArmyNamesPluralZombies, "zombies"),
    DATA_COMPGEN(0x00508f08, gArmyNamesPluralMutantZombies, "mutant zombies"),
    DATA_COMPGEN(0x00508f18, gArmyNamesPluralMummies, "mummies"),
    DATA_COMPGEN(0x00508f20, gArmyNamesPluralRoyalMummies, "royal mummies"),
    DATA_COMPGEN(0x00508f30, gArmyNamesPluralVampires, "vampires"),
    DATA_COMPGEN(0x00508f3c, gArmyNamesPluralVampireLords, "vampire lords"),
    DATA_COMPGEN(0x00508f4c, gArmyNamesPluralLiches, "liches"),
    DATA_COMPGEN(0x00508f54, gArmyNamesPluralPowerLiches, "power liches"),
    DATA_COMPGEN(0x00508f64, gArmyNamesPluralBoneDragons, "bone dragons"),
    DATA_COMPGEN(0x00508f74, gArmyNamesPluralRogues, "rogues"),
    DATA_COMPGEN(0x00508f7c, gArmyNamesPluralNomads, "nomads"),
    DATA_COMPGEN(0x00508f84, gArmyNamesPluralGhosts, "ghosts"),
    DATA_COMPGEN(0x00508f8c, gArmyNamesPluralGenies, "genies"),
    DATA_COMPGEN(0x00508f94, gArmyNamesPluralMedusas, "medusas"),
    DATA_COMPGEN(0x00508f9c, gArmyNamesPluralEarthElementals, "earth elementals"),
    DATA_COMPGEN(0x00508fb0, gArmyNamesPluralAirElementals, "air elementals"),
    DATA_COMPGEN(0x00508fc0, gArmyNamesPluralFireElementals, "fire elementals"),
    DATA_COMPGEN(0x00508fd0, gArmyNamesPluralWaterElementals, "water elementals")
};
DATA(0x004feae8) char* gTerrainNames[KB_TERRAIN_TYPE_COUNT] =
    {DATA_COMPGEN(0x00508fe4, gTerrainNamesOcean, "Ocean"), DATA_COMPGEN(0x00508fec, gTerrainNamesGrass, "Grass"), DATA_COMPGEN(0x00508ff4, gTerrainNamesSnow, "Snow"), DATA_COMPGEN(0x00508ffc, gTerrainNamesSwamp, "Swamp"), DATA_COMPGEN(0x00509004, gTerrainNamesLava, "Lava"), DATA_COMPGEN(0x0050900c, gTerrainNamesDesert, "Desert"), DATA_COMPGEN(0x00509014, gTerrainNamesDirt, "Dirt"), DATA_COMPGEN(0x0050901c, gTerrainNamesWasteland, "Wasteland"), DATA_COMPGEN(0x00509028, gTerrainNamesBeach, "Beach"), NULL};
DATA(0x004feb10) char* gResourceNames[RESOURCE_VALUE_COUNT] =
    {DATA_COMPGEN(0x00509030, gResourceNamesWood, "Wood"), DATA_COMPGEN(0x00509038, gResourceNamesMercury, "Mercury"), DATA_COMPGEN(0x00509040, gResourceNamesOre, "Ore"), DATA_COMPGEN(0x00509044, gResourceNamesSulfur, "Sulfur"), DATA_COMPGEN(0x0050904c, gResourceNamesCrystal, "Crystal"), DATA_COMPGEN(0x00509054, gResourceNamesGems, "Gems"), DATA_COMPGEN(0x0050905c, gResourceNamesGold, "Gold"), NULL};
DATA(0x004feb30) char* gQuickViewText[KB_QUICK_VIEW_TEXT_COUNT] = {
    DATA_COMPGEN(0x0050cb94, gQuickViewTextEmptyString, ""),
    DATA_COMPGEN(0x00509068, gQuickViewTextAlchemistLab, "Alchemist Lab"),
    DATA_COMPGEN(0x00509078, gQuickViewTextSign, "Sign"),
    DATA_COMPGEN(0x00509080, gQuickViewTextBuoy, "Buoy"),
    DATA_COMPGEN(0x00509088, gQuickViewTextSkeleton, "Skeleton"),
    DATA_COMPGEN(0x00509094, gQuickViewTextDaemonCave, "Daemon Cave"),
    DATA_COMPGEN(0x005090a0, gQuickViewTextTreasureChest, "Treasure Chest"),
    DATA_COMPGEN(0x005090b0, gQuickViewTextFaerieRing, "Faerie Ring"),
    DATA_COMPGEN(0x005090bc, gQuickViewTextCampfire, "Campfire"),
    DATA_COMPGEN(0x005090c8, gQuickViewTextFountain, "Fountain"),
    DATA_COMPGEN(0x005090d4, gQuickViewTextGazebo, "Gazebo"),
    DATA_COMPGEN(0x005090dc, gQuickViewTextAncientLamp, "Ancient Lamp"),
    DATA_COMPGEN(0x005090ec, gQuickViewTextGraveyard, "Graveyard"),
    DATA_COMPGEN(0x005090f8, gQuickViewTextArcherSHouse, "Archer's House"),
    DATA_COMPGEN(0x00509108, gQuickViewTextGoblinHut, "Goblin Hut"),
    DATA_COMPGEN(0x00509114, gQuickViewTextDwarfCottage, "Dwarf Cottage"),
    DATA_COMPGEN(0x00509124, gQuickViewTextPeasantHut, "Peasant Hut"),
    DATA_COMPGEN(0x00509130, gQuickViewTextLogCabin, "Log Cabin"),
    DATA_COMPGEN(0x0050913c, gQuickViewTextRoad, "Road"),
    DATA_COMPGEN(0x00509144, gQuickViewTextEvent, "Event"),
    DATA_COMPGEN(0x0050914c, gQuickViewTextDragonCity, "Dragon City"),
    DATA_COMPGEN(0x00509158, gQuickViewTextLighthouse, "Lighthouse"),
    DATA_COMPGEN(0x00509164, gQuickViewTextWaterWheel, "Water Wheel"),
    DATA_COMPGEN(0x00509170, gQuickViewTextMine, "Mine"),
    DATA_COMPGEN(0x00509178, gQuickViewTextArmyCamp, "Army Camp"),
    DATA_COMPGEN(0x00509184, gQuickViewTextObelisk, "Obelisk"),
    DATA_COMPGEN(0x0050918c, gQuickViewTextOasis, "Oasis"),
    DATA_COMPGEN(0x00509194, gQuickViewTextResource, "Resource"),
    DATA_COMPGEN(0x0050ce1c, gQuickViewTextEmptyString2, ""),
    DATA_COMPGEN(0x005091a4, gQuickViewTextSawmill, "Sawmill"),
    DATA_COMPGEN(0x005091ac, gQuickViewTextOracle, "Oracle"),
    DATA_COMPGEN(0x005091b4, gQuickViewTextShrineOfTheFirstCircle, "Shrine of the First Circle"),
    DATA_COMPGEN(0x005091d0, gQuickViewTextShipwreck, "Shipwreck"),
    DATA_COMPGEN(0x005091dc, gQuickViewTextSeaChest, "Sea Chest"),
    DATA_COMPGEN(0x005091e8, gQuickViewTextDesertTent, "Desert Tent"),
    DATA_COMPGEN(0x005091f4, gQuickViewTextTown, "Town"),
    DATA_COMPGEN(0x005091fc, gQuickViewTextStoneLiths, "Stone Liths"),
    DATA_COMPGEN(0x00509208, gQuickViewTextWagonCamp, "Wagon Camp"),
    DATA_COMPGEN(0x00509214, gQuickViewTextWell, "Well"),
    DATA_COMPGEN(0x0050921c, gQuickViewTextWhirlpool, "Whirlpool"),
    DATA_COMPGEN(0x00509228, gQuickViewTextWindmill, "Windmill"),
    DATA_COMPGEN(0x00509234, gQuickViewTextArtifact, "Artifact"),
    DATA_COMPGEN(0x00509240, gQuickViewTextHero, "Hero"),
    DATA_COMPGEN(0x00509248, gQuickViewTextBoat, "Boat"),
    DATA_COMPGEN(0x00510ce4, gQuickViewTextEmptyString3, ""),
    DATA_COMPGEN(0x00509254, gQuickViewTextRandomArtifact, "Random Artifact"),
    DATA_COMPGEN(0x00509264, gQuickViewTextRandomResource, "Random Resource"),
    DATA_COMPGEN(0x00509274, gQuickViewTextRandomMonster, "Random Monster"),
    DATA_COMPGEN(0x00509284, gQuickViewTextRandomTown, "Random Town"),
    DATA_COMPGEN(0x00509290, gQuickViewTextRandomCastle, "Random Castle"),
    DATA_COMPGEN(0x00510cf0, gQuickViewTextEmptyString4, ""),
    DATA_COMPGEN(0x005092a4, gQuickViewTextRandomMonsterWeak, "Random Monster - weak"),
    DATA_COMPGEN(0x005092bc, gQuickViewTextRandomMonsterMedium, "Random Monster - medium"),
    DATA_COMPGEN(0x005092d4, gQuickViewTextRandomMonsterStrong, "Random Monster - strong"),
    DATA_COMPGEN(0x005092ec, gQuickViewTextRandomMonsterVeryStrong, "Random Monster - very strong"),
    DATA_COMPGEN(0x0050930c, gQuickViewTextRandomHero, "Random Hero"),
    DATA_COMPGEN(0x00509318, gQuickViewTextNothingSpecial, "Nothing Special"),
    DATA_COMPGEN(0x00510d18, gQuickViewTextEmptyString5, ""),
    DATA_COMPGEN(0x0050932c, gQuickViewTextWatchTower, "Watch Tower"),
    DATA_COMPGEN(0x00509338, gQuickViewTextTreeHouse, "Tree House"),
    DATA_COMPGEN(0x00509344, gQuickViewTextTreeCity, "Tree City"),
    DATA_COMPGEN(0x00509350, gQuickViewTextRuins, "Ruins"),
    DATA_COMPGEN(0x00509358, gQuickViewTextFort, "Fort"),
    DATA_COMPGEN(0x00509360, gQuickViewTextTradingPost, "Trading Post"),
    DATA_COMPGEN(0x00509370, gQuickViewTextAbandonedMine, "Abandoned Mine"),
    DATA_COMPGEN(0x00509380, gQuickViewTextDwarfCabin, "Dwarf Cabin"),
    DATA_COMPGEN(0x0050938c, gQuickViewTextStandingStones, "Standing Stones"),
    DATA_COMPGEN(0x0050939c, gQuickViewTextIdol, "Idol"),
    DATA_COMPGEN(0x005093a4, gQuickViewTextTreeOfKnowledge, "Tree of Knowledge"),
    DATA_COMPGEN(0x005093b8, gQuickViewTextWitchDoctorSHut, "Witch Doctor's Hut"),
    DATA_COMPGEN(0x005093cc, gQuickViewTextTemple, "Temple"),
    DATA_COMPGEN(0x005093d4, gQuickViewTextHillFort, "Hill Fort"),
    DATA_COMPGEN(0x005093e0, gQuickViewTextHalflingHole, "Halfling Hole"),
    DATA_COMPGEN(0x005093f0, gQuickViewTextMercenaryCamp, "Mercenary Camp"),
    DATA_COMPGEN(0x00509400, gQuickViewTextShrineOfTheSecondCircle, "Shrine of the Second Circle"),
    DATA_COMPGEN(0x0050941c, gQuickViewTextShrineOfTheThirdCircle, "Shrine of the Third Circle"),
    DATA_COMPGEN(0x00509438, gQuickViewTextPyramid, "Pyramid"),
    DATA_COMPGEN(0x00509440, gQuickViewTextCityOfTheDead, "City of the Dead"),
    DATA_COMPGEN(0x00509454, gQuickViewTextExcavation, "Excavation"),
    DATA_COMPGEN(0x00509460, gQuickViewTextSphinx, "Sphinx"),
    DATA_COMPGEN(0x00509468, gQuickViewTextWagon, "Wagon"),
    DATA_COMPGEN(0x00509470, gQuickViewTextTarPit, "Tar Pit"),
    DATA_COMPGEN(0x00509478, gQuickViewTextArtesianSpring, "Artesian Spring"),
    DATA_COMPGEN(0x00509488, gQuickViewTextTrollBridge, "Troll Bridge"),
    DATA_COMPGEN(0x00509498, gQuickViewTextWateringHole, "Watering Hole"),
    DATA_COMPGEN(0x005094a8, gQuickViewTextWitchSHut, "Witch's Hut"),
    DATA_COMPGEN(0x005094b4, gQuickViewTextXanadu, "Xanadu"),
    DATA_COMPGEN(0x005094bc, gQuickViewTextCave, "Cave"),
    DATA_COMPGEN(0x005094c4, gQuickViewTextLeanTo, "Lean-To"),
    DATA_COMPGEN(0x005094cc, gQuickViewTextMagellanSMaps, "Magellan's Maps"),
    DATA_COMPGEN(0x005094dc, gQuickViewTextFlotsam, "Flotsam"),
    DATA_COMPGEN(0x005094e4, gQuickViewTextDerelictShip, "Derelict Ship"),
    DATA_COMPGEN(0x005094f4, gQuickViewTextShipwreckSurvivor, "Shipwreck Survivor"),
    DATA_COMPGEN(0x00509508, gQuickViewTextBottle, "Bottle"),
    DATA_COMPGEN(0x00509510, gQuickViewTextMagicWell, "Magic Well"),
    DATA_COMPGEN(0x0050951c, gQuickViewTextMagicGarden, "Magic Garden"),
    DATA_COMPGEN(0x0050952c, gQuickViewTextObservationTower, "Observation Tower"),
    DATA_COMPGEN(0x00509540, gQuickViewTextFreemanSFoundry, "Freeman's Foundry"),
    DATA_COMPGEN(0x00509554, gQuickViewTextStream, "Stream"),
    DATA_COMPGEN(0x0050955c, gQuickViewTextTrees, "Trees"),
    DATA_COMPGEN(0x00509564, gQuickViewTextMountains, "Mountains"),
    DATA_COMPGEN(0x00509570, gQuickViewTextVolcano, "Volcano"),
    DATA_COMPGEN(0x00509578, gQuickViewTextFlowers, "Flowers"),
    DATA_COMPGEN(0x00509580, gQuickViewTextRock, "Rock"),
    DATA_COMPGEN(0x00509588, gQuickViewTextLake, "Lake"),
    DATA_COMPGEN(0x00509590, gQuickViewTextMandrake, "Mandrake"),
    DATA_COMPGEN(0x0050959c, gQuickViewTextDeadTree, "Dead Tree"),
    DATA_COMPGEN(0x005095a8, gQuickViewTextStump, "Stump"),
    DATA_COMPGEN(0x005095b0, gQuickViewTextCrater, "Crater"),
    DATA_COMPGEN(0x005095b8, gQuickViewTextCactus, "Cactus"),
    DATA_COMPGEN(0x005095c0, gQuickViewTextMound, "Mound"),
    DATA_COMPGEN(0x005095c8, gQuickViewTextDune, "Dune"),
    DATA_COMPGEN(0x005095d0, gQuickViewTextLavaPool, "Lava Pool"),
    DATA_COMPGEN(0x005095dc, gQuickViewTextShrub, "Shrub"),
    DATA_COMPGEN(0x005095e4, gQuickViewTextHole, "Hole"),
    DATA_COMPGEN(0x005095ec, gQuickViewTextOutcropping, "Outcropping"),
    DATA_COMPGEN(0x005095f8, gQuickViewTextRandomArtifactTreasure, "Random Artifact - Treasure"),
    DATA_COMPGEN(0x00509614, gQuickViewTextRandomArtifactMinor, "Random Artifact - Minor"),
    DATA_COMPGEN(0x0050962c, gQuickViewTextRandomArtifactMajor, "Random Artifact - Major"),
    DATA_COMPGEN(0x00509644, gQuickViewTextSBarrier, "%s Barrier"),
    DATA_COMPGEN(0x00509650, gQuickViewTextSTravellerSTent, "%s Traveller's Tent"),
    DATA_COMPGEN(0x00509668, gQuickViewTextS, "%s"),
    DATA_COMPGEN(0x00516d80, gQuickViewTextSecondaryStringFormat, "%s"),
    DATA_COMPGEN(0x0050966c, gQuickViewTextJail, "Jail")
};
DATA(0x004fed20) char* gEventText[KB_EVENT_TEXT_TABLE_COUNT] = {
    DATA_COMPGEN(0x00509674, gEventTextAlchemistYouHaveTakenControlOf, "{Alchemist}\n\nYou have taken control of the local Alchemist shop. It will provide you with "
    "one unit of Mercury per day."),
    DATA_COMPGEN(0x005096ec, gEventTextSignpostASignpostReadsSIs, "{Signpost}\n\nA signpost reads:}\n\n%s is near."),
    DATA_COMPGEN(0x00509718, gEventTextBuoyYourMenSpotANavigational2, "{Buoy}\n\nYour men spot a navigational buoy, confirming that you are on course."),
    DATA_COMPGEN(0x00509768, gEventTextBuoyYourMenSpotANavigational, "{Buoy}\n\nYour men spot a navigational buoy, confirming that you are on course and increasing "
    "their morale."),
    DATA_COMPGEN(0x00510d1c, gEventTextEmptyString, ""),
    DATA_COMPGEN(0x00510d20, gEventTextEmptyString2, ""),
    DATA_COMPGEN(0x00510e64, gEventTextEmptyString3, ""),
    DATA_COMPGEN(0x00510ea4, gEventTextEmptyString4, ""),
    DATA_COMPGEN(0x00510ec0, gEventTextEmptyString5, ""),
    DATA_COMPGEN(0x00510ec4, gEventTextEmptyString6, ""),
    DATA_COMPGEN(0x00510f48, gEventTextEmptyString7, ""),
    DATA_COMPGEN(0x00510f4c, gEventTextEmptyString8, ""),
    DATA_COMPGEN(0x005097f4, gEventTextFaerieRingYouEnterTheFaerie, "{Faerie Ring}\n\nYou enter the faerie ring, but nothing happens."),
    DATA_COMPGEN(0x00509834, gEventTextFaerieRingUponEnteringTheMystical, "{Faerie Ring}\n\nUpon entering the mystical faerie ring, your army gains luck for its next "
    "battle."),
    DATA_COMPGEN(0x00509898, gEventTextCampfireRansackingAnEnemyCampYou, "{Campfire}\n\nRansacking an enemy camp, you discover a hidden cache of treasures."),
    DATA_COMPGEN(0x005098e8, gEventTextFountainYouDrinkFromTheEnchanted, "{Fountain}\n\nYou drink from the enchanted fountain, but nothing happens."),
    DATA_COMPGEN(0x00509930, gEventTextFountainAsYouDrinkTheSweet, "{Fountain}\n\nAs you drink the sweet water, you gain luck for your next battle."),
    DATA_COMPGEN(0x00509980, gEventTextGazeboAnOldKnightAppearsOn, "{Gazebo}\n\nAn old Knight appears on the steps of the gazebo. \"I am sorry, my liege, I have "
    "taught you all I can.\""),
    DATA_COMPGEN(0x005099f4, gEventTextGazeboAnOldKnightAppearsOn2, "{Gazebo}\n\nAn old Knight appears on the steps of the gazebo. \"My liege, I will teach you "
    "all that I know to aid you in your travels.\""),
    DATA_COMPGEN(0x00509a7c, gEventTextGenieLampYouStumbleUponA, "{Genie Lamp}\n\nYou stumble upon a dented and tarnished lamp lodged deep in the earth. Do you "
    "wish to rub the lamp?"),
    DATA_COMPGEN(0x00509af0, gEventTextGraveyardYouTentativelyApproachTheBurial, "{Graveyard}\n\nYou tentatively approach the burial ground of ancient warriors.  Do you want "
    "to search the graves?"),
    DATA_COMPGEN(0x00509b60, gEventTextUponDefeatingTheZombiesYouSpend, "Upon defeating the Zombies you spend several hours searching the graves and find nothing.  "
    "Such a despicable act reduces your army's morale."),
    DATA_COMPGEN(0x00509bf0, gEventTextUponDefeatingTheZomiesYouSearch, "Upon defeating the zomies you search the graves and find something!"),
    DATA_COMPGEN(0x00509c34, gEventTextArcherSHouseAGroupOf, "{Archer's House}\n\nA group of Archers with a desire for greater glory wish to join you. Do "
    "you accept? "),
    DATA_COMPGEN(0x00509c9c, gEventTextYouAreUnableToRecruitAt, "You are unable to recruit at this time, your ranks are full."),
    DATA_COMPGEN(0x00509cdc, gEventTextArcherSHouseAsYouApproach, "{Archer's House}\n\nAs you approach the dwelling, you notice that there is no one here."),
    DATA_COMPGEN(0x00509d34, gEventTextGoblinHutAGroupOfGoblins, "{Goblin Hut}\n\nA group of Goblins with a desire for greater glory wish to join you. Do you "
    "accept?"),
    DATA_COMPGEN(0x00509d98, gEventTextYouAreUnableToRecruitAt2, "You are unable to recruit at this time, your ranks are full."),
    DATA_COMPGEN(0x00509dd8, gEventTextGoblinHutAsYouApproachThe, "{Goblin Hut}\n\nAs you approach the dwelling, you notice that there is no one here."),
    DATA_COMPGEN(0x00509e2c, gEventTextPeasantHutAGroupOfPeasants, "{Peasant Hut}\n\nA group of Peasants with a desire for greater glory wish to join you. Do you "
    "accept? "),
    DATA_COMPGEN(0x00509e94, gEventTextYouAreUnableToRecruitAt3, "You are unable to recruit at this time, your ranks are full."),
    DATA_COMPGEN(0x00509ed4, gEventTextPeasantHutAsYouApproachThe, "{Peasant Hut}\n\nAs you approach the dwelling, you notice that there is no one here."),
    DATA_COMPGEN(0x00509f28, gEventTextDwarfCottageAGroupOfDwarves, "{Dwarf Cottage}\n\nA group of Dwarves with a desire for greater glory wish to join you. Do "
    "you accept? "),
    DATA_COMPGEN(0x00509f90, gEventTextYouAreUnableToRecruitAt4, "You are unable to recruit at this time, your ranks are full."),
    DATA_COMPGEN(0x00509fd0, gEventTextDwarfCottageAsYouApproachThe, "{Dwarf Cottage}\n\nAs you approach the dwelling, you notice that there is no one here."),
    DATA_COMPGEN(0x0050a028, gEventTextThatchedHutAGroupOfPeasants, "{Thatched Hut}\n\nA group of Peasants with a desire for greater glory wish to join you. Do "
    "you accept? "),
    DATA_COMPGEN(0x0050a090, gEventTextYouAreUnableToRecruitAt5, "You are unable to recruit at this time, your ranks are full."),
    DATA_COMPGEN(0x0050a0d0, gEventTextThatchedHutAsYouApproachThe, "{Thatched Hut}\n\nAs you approach the Peasant dwelling you notice that there is no one here."),
    DATA_COMPGEN(0x0050a12c, gEventTextTreeHouseAGroupOfSprites, "{Tree House}\n\nA group of Sprites with a desire for greater glory wish to join you. Do you "
    "accept? "),
    DATA_COMPGEN(0x0050a190, gEventTextYouAreUnableToRecruitAt6, "You are unable to recruit at this time, your ranks are full."),
    DATA_COMPGEN(0x0050a1d0, gEventTextTreeHouseAsYouApproachThe, "{Tree House}\n\nAs you approach the Sprites' tree house you notice that there is no one here."),
    DATA_COMPGEN(0x0050a22c, gEventTextHalflingHoleAGroupOfHalflings, "{Halfling Hole}\n\nA group of Halflings with a desire for greater glory wish to join you. Do "
    "you accept? "),
    DATA_COMPGEN(0x0050a294, gEventTextYouAreUnableToRecruitAt7, "You are unable to recruit at this time, your ranks are full."),
    DATA_COMPGEN(0x0050a2d4, gEventTextHalflingHoleAsYouApproachThe, "{Halfling Hole}\n\nAs you approach the habitat you notice that there is no one here."),
    DATA_COMPGEN(0x0050a328, gEventTextWatchTowerAGroupOfOrcs, "{Watch Tower}\n\nA group of Orcs with a desire for greater glory wish to join you. Do you "
    "accept? "),
    DATA_COMPGEN(0x0050a38c, gEventTextYouAreUnableToRecruitAt8, "You are unable to recruit at this time, your ranks are full."),
    DATA_COMPGEN(0x0050a3cc, gEventTextWatchTowerAsYouApproachThe, "{Watch Tower}\n\nAs you approach the Orcs' watch tower you notice that there is no one here."),
    DATA_COMPGEN(0x0050a428, gEventTextSnowCaveAGroupOfCentaurs, "{Snow Cave}\n\nA group of Centaurs with a desire for greater glory wish to join you. Do you "
    "accept? "),
    DATA_COMPGEN(0x0050a48c, gEventTextYouAreUnableToRecruitAt9, "You are unable to recruit at this time, your ranks are full."),
    DATA_COMPGEN(0x0050a4cc, gEventTextCaveAsYouApproachTheCentaurs, "{Cave}\n\nAs you approach the Centaurs' cave you notice that there is no one here."),
    DATA_COMPGEN(0x0050a520, gEventTextExcavationAGroupOfSkeletonsWith, "{Excavation}\n\nA group of Skeletons with a desire for greater glory wish to join you. Do you "
    "accept? "),
    DATA_COMPGEN(0x0050a588, gEventTextYouAreUnableToRecruitAt10, "You are unable to recruit at this time, your ranks are full."),
    DATA_COMPGEN(0x0050a5c8, gEventTextExcavationAsYouApproachTheSkeletons, "{Excavation}\n\nAs you approach the Skeletons' excavation you notice that there is no one "
    "here."),
    DATA_COMPGEN(0x00510f50, gEventTextEmptyString9, ""),
    DATA_COMPGEN(0x00510f88, gEventTextEmptyString10, ""),
    DATA_COMPGEN(0x00510f8c, gEventTextEmptyString11, ""),
    DATA_COMPGEN(0x00510f9c, gEventTextEmptyString12, ""),
    DATA_COMPGEN(0x00510ff4, gEventTextEmptyString13, ""),
    DATA_COMPGEN(0x0050a63c, gEventTextLighthouseTheLighthouseIsNowUnder, "{Lighthouse}\n\nThe lighthouse is now under your control, and all of your ships will now move "
    "further each turn."),
    DATA_COMPGEN(0x0050a6ac, gEventTextWaterWheelTheKeeperOfThe, "{Water Wheel}\n\nThe keeper of the mill announces: \"Milord, I am sorry, there is no gold "
    "currently available.  Please try again next week.\""),
    DATA_COMPGEN(0x0050a738, gEventTextWaterWheelTheKeeperOfThe2, "{Water Wheel}\n\nThe keeper of the mill announces: \"Milord, I have been working very hard to "
    "provide you with this gold, come back next week for more.\""),
    DATA_COMPGEN(0x0050a7d0, gEventTextOreMineYouGainControlOf, "{Ore Mine}\n\nYou gain control of an ore mine. It will provide you with two units of ore per "
    "day."),
    DATA_COMPGEN(0x0050a830, gEventTextSulfurMineYouGainControlOf, "{Sulfur Mine}\n\nYou gain control of a sulfur mine. It will provide you with one unit of "
    "sulfur per day."),
    DATA_COMPGEN(0x0050a898, gEventTextCrystalMineYouGainControlOf, "{Crystal Mine}\n\nYou gain control of a crystal mine. It will provide you with one unit of "
    "crystal per day."),
    DATA_COMPGEN(0x0050a904, gEventTextGemMineYouGainControlOf, "{Gem Mine}\n\nYou gain control of a gem mine. It will provide you with one unit of gems per "
    "day."),
    DATA_COMPGEN(0x0050a964, gEventTextGoldMineYouGainControlOf, "{Gold Mine}\n\nYou gain control of a gold mine. It will provide you with 1000 gold per day."),
    DATA_COMPGEN(0x0050a9c0, gEventTextFollowersAGroupOfSWith, "{Followers}\n\nA group of %s with a desire for greater glory wish to join you. Do you "
    "accept? "),
    DATA_COMPGEN(0x0050aa20, gEventTextInsultedByYourRefusalOfTheir, "Insulted by your refusal of their offer, the monsters attack!"),
    DATA_COMPGEN(0x0050aa60, gEventTextObeliskYouComeUponAnObelisk, "{Obelisk}\n\nYou come upon an obelisk made from a type of stone you have never seen before.  "
    "Staring at it intensely, the smooth surface suddenly changes to an inscription.  The "
    "inscription is a piece of a lost ancient map.  Quickly you copy down the piece and the "
    "inscription vanishes as abruptly as it had appeared."),
    DATA_COMPGEN(0x0050ab9c, gEventTextObeliskYouHaveAlreadyBeenTo, "{Obelisk}\n\nYou have already been to this obelisk."),
    DATA_COMPGEN(0x00511008, gEventTextEmptyString14, ""),
    DATA_COMPGEN(0x00511034, gEventTextEmptyString15, ""),
    DATA_COMPGEN(0x0050abd8, gEventTextYouFindASmallQuantityOf, "You find a small quantity of %s."),
    DATA_COMPGEN(0x0050abfc, gEventTextSawmillYouGainControlOfA, "{Sawmill}\n\nYou gain control of a sawmill. It will provide you with two units of wood per "
    "day."),
    DATA_COMPGEN(0x0050ac5c, gEventTextOracleNestledAmongTheTreesSits, "{Oracle}\n\nNestled among the trees sits a blind seer. After explaining the intent of your "
    "journey, the seer activates his crystal ball, allowing you to see the strengths and "
    "weaknesses of your opponents."),
    DATA_COMPGEN(0x005110b8, gEventTextEmptyString16, ""),
    DATA_COMPGEN(0x005110bc, gEventTextEmptyString17, ""),
    DATA_COMPGEN(0x005125f8, gEventTextEmptyString18, ""),
    DATA_COMPGEN(0x00512678, gEventTextEmptyString19, ""),
    DATA_COMPGEN(0x0051270c, gEventTextEmptyString20, ""),
    DATA_COMPGEN(0x00512710, gEventTextEmptyString21, ""),
    DATA_COMPGEN(0x0050ad40, gEventTextDesertTentAGroupOfTattered2, "{Desert Tent}\n\nA group of tattered tents, billowing in the sandy wind, beckons you.  The "
    "tents are unoccupied.  Perhaps more Nomads will be here later."),
    DATA_COMPGEN(0x0050add8, gEventTextDesertTentAGroupOfTattered, "{Desert Tent}\n\nA group of tattered tents, billowing in the sandy wind, beckons you.  Do you "
    "wish to have any Nomads join you during your travels?"),
    DATA_COMPGEN(0x0050ae6c, gEventTextWagonAColorfulRoguesWagonStands, "{Wagon}\n\nA colorful Rogues' wagon stands empty here.  Perhaps more Rogues will be here "
    "later."),
    DATA_COMPGEN(0x0050aecc, gEventTextWagonDistantSoundsOfMusicAnd, "{Wagon}\n\nDistant sounds of music and laughter draw you to a colorful wagon housing Rogues.  "
    "Do you wish to have any Rogues join your army?"),
    DATA_COMPGEN(0x0050af58, gEventTextWhirlpoolAWhirlpoolEngulfsYourShip, "{Whirlpool}\n\nA whirlpool engulfs your ship.  Some of your army has fallen overboard."),
    DATA_COMPGEN(0x0050afb0, gEventTextWindmillTheKeeperOfTheMill, "{Windmill}\n\nThe keeper of the mill announces: \"Milord, I am sorry, there are no resources "
    "currently available. Please try again next week.\""),
    DATA_COMPGEN(0x0050b03c, gEventTextWindmillTheKeeperOfTheMill2, "{Windmill}\n\nThe keeper of the mill announces: \"Milord, I have been working very hard to "
    "provide you with these resources, come back next week for more.\""),
    DATA_COMPGEN(0x00512714, gEventTextEmptyString22, ""),
    DATA_COMPGEN(0x005138c4, gEventTextEmptyString23, ""),
    DATA_COMPGEN(0x0051415c, gEventTextEmptyString24, ""),
    DATA_COMPGEN(0x00516f58, gEventTextSecondaryEmptyString, ""),
    DATA_COMPGEN(0x00500178, gEventTextSecondaryEmptyString2, ""),
    DATA_COMPGEN(0x0050b0ec, gEventTextSkeletonYouComeUponTheRemains2, "{Skeleton}\n\nYou come upon the remains of an unfortunate adventurer.  Searching through the "
    "tattered clothing, you find nothing."),
    DATA_COMPGEN(0x0050b16c, gEventTextSkeletonYouComeUponTheRemains, "{Skeleton}\n\nYou come upon the remains of an unfortunate adventurer.  Searching through the "
    "tattered clothing, you find "),
    NULL
};
DATA(0x004feea0) char* gCPanelHelp[KB_CONTROL_PANEL_HELP_COUNT] = {
    DATA_COMPGEN(0x0050b1e4, gCPanelHelpStartASingleOrMultiPlayer, "Start a single or multi-player game."),
    DATA_COMPGEN(0x0050b20c, gCPanelHelpLoadAPreviouslySavedGame, "Load a previously saved game."),
    DATA_COMPGEN(0x0050b22c, gCPanelHelpSaveTheCurrentGame, "Save the current game."),
    DATA_COMPGEN(0x0050b244, gCPanelHelpQuitOutOfHeroesOfMight, "Quit out of Heroes of Might and Magic II."),
    DATA_COMPGEN(0x0050b270, gCPanelHelpExitThisMenuWithoutDoingAnything, "Exit this menu without doing anything."),
    NULL
};
DATA(0x004feeb8) char* gCSPanelHelp[KB_COMBAT_SPELL_PANEL_HELP_COUNT] = {
    DATA_COMPGEN(0x0050b298, gCSPanelHelpOKExitThisMenu, "{OK}\n\nExit this menu."),
    DATA_COMPGEN(0x0050b2b0, gCSPanelHelpSpeedSetTheSpeedOfCombat, "{Speed}\n\nSet the speed of combat actions and animations."),
    DATA_COMPGEN(0x0050b2ec, gCSPanelHelpMonsterInfoToggleTheMonsterInfo, "{Monster Info}\n\nToggle the monster info window, which shows information on the active and "
    "targeted monsters."),
    DATA_COMPGEN(0x0050b35c, gCSPanelHelpAutoCombatSpellCastingToggleWhether, "{Auto Combat\nSpell Casting}\n\nToggle whether or not the computer will cast spells for you "
    "when auto combat is on.  (Note: This does not affect spell casting for computer players in "
    "any way, nor does it affect quick combat.)"),
    DATA_COMPGEN(0x0050b43c, gCSPanelHelpGridToggleTheHexGridOn, "{Grid}\n\nToggle the hex grid on or off.  The hex grid always underlies movement, even if "
    "turned off.  This switch only determines if the grid is visible."),
    DATA_COMPGEN(0x0050b4d8, gCSPanelHelpShadowMovementToggleOnOrOff, "{Shadow Movement}\n\nToggle on or off a shadowing system showing where your creatures can "
    "move and attack.  When this is turned on, there is a slight performance cost, most noticeable "
    "on 486 computers."),
    DATA_COMPGEN(0x0050b5a0, gCSPanelHelpShadowCursorToggleOnOrOff, "{Shadow Cursor}\n\nToggle on or off a shadowing system showing where the hex grid that the "
    "mouse cursor is currently over is shadowed.  When this is turned on, there is a slight "
    "performance cost, most noticeable on 486 computers."),
    NULL
};
DATA(0x004feed8) char* gAPanelHelp[KB_ADVENTURE_PANEL_HELP_COUNT] = {
    DATA_COMPGEN(0x0050b684, gAPanelHelpViewTheEntireWorld, "View the entire world."),
    DATA_COMPGEN(0x0050b69c, gAPanelHelpViewTheObeliskPuzzle, "View the obelisk puzzle."),
    DATA_COMPGEN(0x0050b6b8, gAPanelHelpViewInformationOnTheScenarioYou, "View information on the scenario you are currently playing."),
    DATA_COMPGEN(0x0050b6f4, gAPanelHelpDigForTheUltimateArtifact, "Dig for the Ultimate Artifact."),
    DATA_COMPGEN(0x0050b714, gAPanelHelpExitThisMenuWithoutDoingAnything, "Exit this menu without doing anything."),
    NULL
};
DATA(0x004feef0) char* gInitMenuHelp[KB_INIT_MENU_HELP_COUNT] = {
    DATA_COMPGEN(0x0050b73c, gInitMenuHelpNewGameStartASingleOr, "{New Game}\n\nStart a single or multi-player game."),
    DATA_COMPGEN(0x0050b770, gInitMenuHelpLoadGameLoadAPreviouslySaved, "{Load Game}\n\nLoad a previously saved game."),
    DATA_COMPGEN(0x0050b79c, gInitMenuHelpHighScoresViewTheHighScore, "{High Scores}\n\nView the high score screen."),
    DATA_COMPGEN(0x0050b7c8, gInitMenuHelpCreditsViewTheCreditsScreen, "{Credits}\n\nView the credits screen."),
    DATA_COMPGEN(0x0050b7ec, gInitMenuHelpQuitQuitHeroesOfMightAnd, "{Quit}\n\nQuit Heroes of Might and Magic and return to the operating system."),
    NULL
};
DATA(0x004fef08) char* gAdvMenuHelp[KB_ADVENTURE_MENU_HELP_COUNT] = {
    DATA_COMPGEN(0x0050b838, gAdvMenuHelpNextHeroSelectTheNextHero, "{Next Hero}\n\nSelect the next Hero."),
    DATA_COMPGEN(0x0050b85c, gAdvMenuHelpContinueMovementContinueTheHeroS, "{Continue Movement}\n\nContinue the Hero's movement along the current path."),
    DATA_COMPGEN(0x0050b8a8, gAdvMenuHelpKingdomSummaryViewASummaryOf, "{Kingdom Summary}\n\nView a summary of your kingdom."),
    DATA_COMPGEN(0x0050b8dc, gAdvMenuHelpEndTurnEndYourTurnAnd, "{End Turn}\n\nEnd your turn and let the computer take its turn."),
    DATA_COMPGEN(0x0050b91c, gAdvMenuHelpAdventureOptionsBringUpTheAdventure, "{Adventure Options}\n\nBring up the adventure options menu."),
    DATA_COMPGEN(0x0050b958, gAdvMenuHelpFileOptionsBringUpTheFile, "{File Options}\n\nBring up the file options menu, allowing you to load, save etc."),
    DATA_COMPGEN(0x0050b9a8, gAdvMenuHelpSystemOptionsBringUpTheSystem, "{System Options}\n\nBring up the system options menu, allowing you to customize your game."),
    DATA_COMPGEN(0x0050ba04, gAdvMenuHelpCastSpellCastAnAdventureSpell, "{Cast Spell}\n\nCast an adventure spell.")
};
DATA(0x004fef28) char* gLuckText[KB_LUCK_TEXT_COUNT] =
    {DATA_COMPGEN(0x0050ba2c, gLuckTextCursed, "Cursed"), DATA_COMPGEN(0x0050ba34, gLuckTextAwful, "Awful"), DATA_COMPGEN(0x0050ba3c, gLuckTextBad, "Bad"), DATA_COMPGEN(0x0050ba40, gLuckTextNormal, "Normal"), DATA_COMPGEN(0x0050ba48, gLuckTextGood, "Good"), DATA_COMPGEN(0x0050ba50, gLuckTextGreat, "Great"), DATA_COMPGEN(0x0050ba58, gLuckTextIrish, "Irish"), NULL};
DATA(0x004fef48) char* gMoraleText[KB_MORALE_TEXT_COUNT] =
    {DATA_COMPGEN(0x0050ba60, gMoraleTextTreason, "Treason"), DATA_COMPGEN(0x0050ba68, gMoraleTextAwful, "Awful"), DATA_COMPGEN(0x0050ba70, gMoraleTextPoor, "Poor"), DATA_COMPGEN(0x0050ba78, gMoraleTextNormal, "Normal"), DATA_COMPGEN(0x0050ba80, gMoraleTextGood, "Good"), DATA_COMPGEN(0x0050ba88, gMoraleTextGreat, "Great"), DATA_COMPGEN(0x0050ba90, gMoraleTextBlood, "Blood!"), NULL};
DATA(0x004fef68) char* onOffText[KB_ON_OFF_TEXT_COUNT] = {
    DATA_COMPGEN(0x0050ba98, onOffTextOff, "Off"),
    DATA_COMPGEN(0x0050ba9c, onOffTextOn, "On"),
    DATA_COMPGEN(0x0050baa0, onOffTextOnVolume9, "On\nVolume 9"),
    DATA_COMPGEN(0x0050baac, onOffTextOnVolume8, "On\nVolume 8"),
    DATA_COMPGEN(0x0050bab8, onOffTextOnVolume7, "On\nVolume 7"),
    DATA_COMPGEN(0x0050bac4, onOffTextOnVolume6, "On\nVolume 6"),
    DATA_COMPGEN(0x0050bad0, onOffTextOnVolume5, "On\nVolume 5"),
    DATA_COMPGEN(0x0050badc, onOffTextOnVolume4, "On\nVolume 4"),
    DATA_COMPGEN(0x0050bae8, onOffTextOnVolume3, "On\nVolume 3"),
    DATA_COMPGEN(0x0050baf4, onOffTextOnVolume2, "On\nVolume 2"),
    DATA_COMPGEN(0x0050bb00, onOffTextOnVolume1, "On\nVolume 1"),
    NULL
};
DATA(0x004fef98) char* walkSpeedText[KB_WALK_SPEED_TEXT_COUNT] =
    {DATA_COMPGEN(0x0050bb0c, walkSpeedTextWalk, "Walk"), DATA_COMPGEN(0x0050bb14, walkSpeedTextTrot, "Trot"), DATA_COMPGEN(0x0050bb1c, walkSpeedTextCanter, "Canter"), DATA_COMPGEN(0x0050bb24, walkSpeedTextGallop, "Gallop"), DATA_COMPGEN(0x0050bb2c, walkSpeedTextJump, "Jump"), NULL};
DATA(0x004fefb0) char* gColors[IDX(FACTION_COUNT)] =
    {DATA_COMPGEN(0x0050bb34, gColorsBlue, "blue"), DATA_COMPGEN(0x0050bb3c, gColorsGreen, "green"), DATA_COMPGEN(0x0050bb44, gColorsRed, "red"), DATA_COMPGEN(0x0050bb48, gColorsYellow, "yellow"), DATA_COMPGEN(0x0050bb50, gColorsOrange, "orange"), DATA_COMPGEN(0x0050bb58, gColorsPurple, "purple")};
DATA(0x004fefc8) char* gMonthNames[KB_MONTH_NAME_COUNT] = {
    DATA_COMPGEN(0x0050bb60, gMonthNamesGrasshopper, "Grasshopper"),
    DATA_COMPGEN(0x0050bb6c, gMonthNamesAnt, "Ant"),
    DATA_COMPGEN(0x0050bb70, gMonthNamesDragonfly, "Dragonfly"),
    DATA_COMPGEN(0x0050bb7c, gMonthNamesSpider, "Spider"),
    DATA_COMPGEN(0x0050bb84, gMonthNamesButterfly, "Butterfly"),
    DATA_COMPGEN(0x0050bb90, gMonthNamesBumblebee, "Bumblebee"),
    DATA_COMPGEN(0x0050bb9c, gMonthNamesLocust, "Locust"),
    DATA_COMPGEN(0x0050bba4, gMonthNamesEarthworm, "Earthworm"),
    DATA_COMPGEN(0x0050bbb0, gMonthNamesHornet, "Hornet"),
    DATA_COMPGEN(0x0050bbb8, gMonthNamesBeetle, "Beetle")
};
DATA(0x004feff0) char* gWeekNames[KB_WEEK_NAME_COUNT] = {
    DATA_COMPGEN(0x0050bbc0, gWeekNamesSquirrel, "Squirrel"),
    DATA_COMPGEN(0x0050bbcc, gWeekNamesRabbit, "Rabbit"),
    DATA_COMPGEN(0x0050bbd4, gWeekNamesGopher, "Gopher"),
    DATA_COMPGEN(0x0050bbdc, gWeekNamesBadger, "Badger"),
    DATA_COMPGEN(0x0050bbe4, gWeekNamesRat, "Rat"),
    DATA_COMPGEN(0x0050bbe8, gWeekNamesEagle, "Eagle"),
    DATA_COMPGEN(0x0050bbf0, gWeekNamesWeasel, "Weasel"),
    DATA_COMPGEN(0x0050bbf8, gWeekNamesRaven, "Raven"),
    DATA_COMPGEN(0x0050bc00, gWeekNamesMongoose, "Mongoose"),
    DATA_COMPGEN(0x0050bc0c, gWeekNamesDog, "Dog"),
    DATA_COMPGEN(0x0050bc10, gWeekNamesAardvark, "Aardvark"),
    DATA_COMPGEN(0x0050bc1c, gWeekNamesLizard, "Lizard"),
    DATA_COMPGEN(0x0050bc24, gWeekNamesTortoise, "Tortoise"),
    DATA_COMPGEN(0x0050bc30, gWeekNamesHedgehog, "Hedgehog"),
    DATA_COMPGEN(0x0050bc3c, gWeekNamesCondor, "Condor"),
    NULL
};
DATA(0x004ff030) char* cHeroScreen[KB_HERO_SCREEN_TEXT_COUNT] = {
    DATA_COMPGEN(0x0050bc44, cHeroScreenKingdomOverview, "Kingdom Overview"),
    DATA_COMPGEN(0x0050bc58, cHeroScreenViewSInfo, "View %s Info"),
    DATA_COMPGEN(0x0050bc68, cHeroScreenAdditionalHeroCharacteristics, "Additional hero characteristics"),
    DATA_COMPGEN(0x0050bc88, cHeroScreenViewGoodMoraleInfo, "View Good Morale Info"),
    DATA_COMPGEN(0x0050bca0, cHeroScreenViewNeutralMoraleInfo, "View Neutral Morale Info"),
    DATA_COMPGEN(0x0050bcbc, cHeroScreenViewBadMoraleInfo, "View Bad Morale Info"),
    DATA_COMPGEN(0x0050bcd4, cHeroScreenViewGoodLuckInfo, "View Good Luck Info"),
    DATA_COMPGEN(0x0050bce8, cHeroScreenViewNeutralLuckInfo, "View Neutral Luck Info"),
    DATA_COMPGEN(0x0050bd00, cHeroScreenViewBadLuckInfo, "View Bad Luck Info"),
    DATA_COMPGEN(0x0050bd14, cHeroScreenViewExperienceInfo, "View Experience Info"),
    DATA_COMPGEN(0x0050bd2c, cHeroScreenSelectS, "Select %s"),
    DATA_COMPGEN(0x0050bd38, cHeroScreenEmpty, "Empty"),
    DATA_COMPGEN(0x0050bd40, cHeroScreenMoveS, "Move %s"),
    DATA_COMPGEN(0x0050bd48, cHeroScreenExchangeSWithS, "Exchange %s with %s"),
    DATA_COMPGEN(0x0050bd5c, cHeroScreenViewSpells, "View Spells"),
    DATA_COMPGEN(0x0050bd68, cHeroScreenViewSInfo2, "View %s Info"),
    DATA_COMPGEN(0x0050bd78, cHeroScreenDismissSTheS, "Dismiss %s the %s"),
    DATA_COMPGEN(0x0050bd8c, cHeroScreenExitHeroScreen, "Exit Hero Screen"),
    DATA_COMPGEN(0x0050bda0, cHeroScreenHeroScreen, "Hero Screen"),
    DATA_COMPGEN(0x0050bdac, cHeroScreenCombineS, "Combine %s"),
    DATA_COMPGEN(0x0050bdb8, cHeroScreenSplitS, "Split %s"),
    DATA_COMPGEN(0x0050bdc4, cHeroScreenViewSSInfo, "View %s %s Info"),
    DATA_COMPGEN(0x0050bdd4, cHeroScreenViewSpellPointsInfo, "View Spell Points Info"),
    DATA_COMPGEN(0x0050bdec, cHeroScreenSetArmyCombatFormationToSpread, "Set army combat formation to 'Spread'"),
    DATA_COMPGEN(0x0050be14, cHeroScreenSetArmyCombatFormationToGrouped, "Set army combat formation to 'Grouped'"),
    NULL
};
DATA(0x004ff098) char* cCastleInfo[KB_CASTLE_INFO_TEXT_COUNT] = {
    DATA_COMPGEN(0x0050be3c, cCastleInfoBuildMageGuild, "Build Mage Guild"),
    DATA_COMPGEN(0x0050be50, cCastleInfoMageGuildIsAtHighestLevel, "Mage Guild is at highest level."),
    DATA_COMPGEN(0x0050be70, cCastleInfoCannotAffordNextLevel, "Cannot afford next level."),
    DATA_COMPGEN(0x0050be8c, cCastleInfoAddAnotherLevelToMageGuild, "Add another level to Mage Guild"),
    DATA_COMPGEN(0x0050beac, cCastleInfoSIsAlreadyBuilt, "%s is already built"),
    DATA_COMPGEN(0x0050bec0, cCastleInfoCannotBuildS, "Cannot build %s"),
    DATA_COMPGEN(0x0050bed0, cCastleInfoCannotAffordS, "Cannot afford %s"),
    DATA_COMPGEN(0x0050bee4, cCastleInfoBuildS, "Build %s"),
    DATA_COMPGEN(0x0050bef0, cCastleInfoCannotAffordAHero, "Cannot afford a Hero."),
    DATA_COMPGEN(0x0050bf08, cCastleInfoCannotRecruitYouAlreadyHaveD, "Cannot recruit - you already have %d Heroes."),
    DATA_COMPGEN(0x0050bf38, cCastleInfoCannotRecruitYouAlreadyHaveA, "Cannot recruit - you already have a Hero in this town."),
    DATA_COMPGEN(0x0050bf70, cCastleInfoRecruitSTheS, "Recruit %s the %s"),
    DATA_COMPGEN(0x0050bf84, cCastleInfoExitCastle, "Exit Castle"),
    DATA_COMPGEN(0x0050bf90, cCastleInfoCastleOptions, "Castle Options"),
    DATA_COMPGEN(0x0050bfa0, cCastleInfoSetGarrisonCombatFormationToGrouped, "Set garrison combat formation to 'Grouped'"),
    DATA_COMPGEN(0x0050bfcc, cCastleInfoSetGarrisonCombatFormationToSpread, "Set garrison combat formation to 'Spread'")
};
DATA(0x004ff0d8) char* cLuckInfo[KB_LUCK_INFO_TEXT_COUNT] = {
    DATA_COMPGEN(0x0050bff8, cLuckInfoGoodLuckGoodLuckSometimesLets, "{Good Luck}\n\nGood luck sometimes lets your armies get lucky attacks (double strength) in "
    "combat."),
    DATA_COMPGEN(0x0050c05c, cLuckInfoNeutralLuckNeutralLuckMeansYour, "{Neutral Luck}\n\nNeutral luck means your armies will never get lucky or unlucky attacks on "
    "the enemy."),
    DATA_COMPGEN(0x0050c0c4, cLuckInfoBadLuckBadLuckSometimesFalls, "{Bad Luck}\n\nBad luck sometimes falls on your armies in combat, causing their attacks to "
    "only do half damage."),
    DATA_COMPGEN(0x0050c134, cLuckInfoSCurrentLuckModifiers, "%s\n\n\nCurrent Luck Modifiers:"),
    DATA_COMPGEN(0x0050c154, cLuckInfoLuckyRabbitSFoot1, "\nLucky Rabbit's Foot +1"),
    DATA_COMPGEN(0x0050c16c, cLuckInfoGoldenHorseshoe1, "\nGolden Horseshoe +1"),
    DATA_COMPGEN(0x0050c184, cLuckInfoGamblerSLuckyCoin1, "\nGambler's Lucky Coin +1"),
    DATA_COMPGEN(0x0050c1a0, cLuckInfoFourLeafClover1, "\nFour-Leaf Clover +1"),
    DATA_COMPGEN(0x0050c1b8, cLuckInfoFaerieRingVisited1, "\nFaerie ring visited +1"),
    DATA_COMPGEN(0x0050c1d0, cLuckInfoFountainVisited1, "\nFountain visited +1"),
    DATA_COMPGEN(0x0050c1e8, cLuckInfoNone, "\nnone"),
    DATA_COMPGEN(0x0050c1f0, cLuckInfoTombRobber1, "\nTomb robber -1"),
    DATA_COMPGEN(0x0050c200, cLuckInfoSorceressRainbow2, "\nSorceress Rainbow +2"),
    DATA_COMPGEN(0x0050c218, cLuckInfoIdolVisited1, "\nIdol visited +1"),
    DATA_COMPGEN(0x0050c22c, cLuckInfoPyramidRaided2, "\nPyramid raided -2"),
    DATA_COMPGEN(0x0050c240, cLuckInfoBasicLuck1, "\nBasic Luck +1"),
    DATA_COMPGEN(0x0050c250, cLuckInfoAdvancedLuck2, "\nAdvanced Luck +2"),
    DATA_COMPGEN(0x0050c264, cLuckInfoExpertLuck3, "\nExpert Luck +3"),
    DATA_COMPGEN(0x0050c274, cLuckInfoMastheadBonusAtSea1, "\nMasthead bonus at sea +1"),
    DATA_COMPGEN(0x0050c290, cLuckInfoMermaidVisited1, "\nMermaid visited +1"),
    DATA_COMPGEN(0x0050c2a4, cLuckInfoBattleGarbOfAnduranGivesYou, "\nBattle Garb of Anduran gives you maximum luck."),
    NULL
};
DATA(0x004ff130) char* IQnames[KB_IQ_NAME_COUNT] =
    {DATA_COMPGEN(0x0050c2d4, iQnamesNone, "None"), DATA_COMPGEN(0x0050c2dc, iQnamesDumb, "Dumb"), DATA_COMPGEN(0x0050c2e4, iQnamesAverage, "Average"), DATA_COMPGEN(0x0050c2ec, iQnamesSmart, "Smart"), DATA_COMPGEN(0x0050c2f4, iQnamesGenius, "Genius"), NULL};
DATA(0x004ff148) char* cSpellHelp[KB_SPELL_HELP_TEXT_COUNT] = {
    DATA_COMPGEN(0x0050c2fc, cSpellHelpViewPreviousPage, "View previous page"),
    DATA_COMPGEN(0x0050c310, cSpellHelpViewNextPage, "View next page"),
    DATA_COMPGEN(0x0050c320, cSpellHelpViewAdventureSpells, "View adventure Spells"),
    DATA_COMPGEN(0x0050c338, cSpellHelpViewCombatSpells, "View Combat Spells"),
    DATA_COMPGEN(0x0050c34c, cSpellHelpCloseSpellbook, "Close Spellbook"),
    DATA_COMPGEN(0x0050c35c, cSpellHelpViewSpells, "View Spells"),
    DATA_COMPGEN(0x0050c368, cSpellHelpSelectSpell, "Select Spell"),
    DATA_COMPGEN(0x0050c378, cSpellHelpViewCombatSpells2, "View Combat Spells"),
    DATA_COMPGEN(0x0050c38c, cSpellHelpYourHeroHasDSpellPoints, "Your hero has %d spell points remaining"),
    NULL
};
DATA(0x004ff170) char* speedText[KB_SPEED_TEXT_COUNT] = {
    DATA_COMPGEN(0x0050017c, speedTextEmptyString, ""),
    DATA_COMPGEN(0x0050c3b8, speedTextCrawling, "Crawling"),
    DATA_COMPGEN(0x0050c3c4, speedTextVerySlow, "Very slow"),
    DATA_COMPGEN(0x0050c3d0, speedTextSlow, "Slow"),
    DATA_COMPGEN(0x0050c3d8, speedTextAverage, "Average"),
    DATA_COMPGEN(0x0050c3e0, speedTextFast, "Fast"),
    DATA_COMPGEN(0x0050c3e8, speedTextVeryFast, "Very Fast"),
    DATA_COMPGEN(0x0050c3f4, speedTextUltraFast, "Ultra Fast"),
    DATA_COMPGEN(0x0050c400, speedTextBlazing, "Blazing"),
    DATA_COMPGEN(0x0050c408, speedTextInstant, "Instant")
};
DATA(0x004ff198) char* cArmyDetail[KB_ARMY_DETAIL_TEXT_COUNT] = {
    DATA_COMPGEN(0x0050c410, cArmyDetailAttackSkill, "Attack Skill: "),
    DATA_COMPGEN(0x0050c420, cArmyDetailDefenseSkill, "Defense Skill: "),
    DATA_COMPGEN(0x0050c430, cArmyDetailShotsLeft, "Shots left: "),
    DATA_COMPGEN(0x0050c440, cArmyDetailDamage, "Damage: "),
    DATA_COMPGEN(0x0050c44c, cArmyDetailHitPoints, "Hit Points: "),
    DATA_COMPGEN(0x0050c45c, cArmyDetailSpeed, "Speed: "),
    DATA_COMPGEN(0x0050c464, cArmyDetailMorale, "Morale: "),
    DATA_COMPGEN(0x0050c470, cArmyDetailLuck, "Luck: "),
    DATA_COMPGEN(0x0050c478, cArmyDetailShots, "Shots: "),
    NULL
};
DATA(0x004ff1c0) char* cWellDetail[KB_WELL_DETAIL_TEXT_COUNT] = {
    DATA_COMPGEN(0x0050c480, cWellDetailAttack, "Attack: "),
    DATA_COMPGEN(0x0050c48c, cWellDetailDefense, "Defense: "),
    DATA_COMPGEN(0x0050c498, cWellDetailShots, "Shots: "),
    DATA_COMPGEN(0x0050c4a0, cWellDetailDamg, "Damg: "),
    DATA_COMPGEN(0x0050c4a8, cWellDetailHP, "HP: "),
    DATA_COMPGEN(0x0050c4b0, cWellDetailSpeed, "Speed: "),
    DATA_COMPGEN(0x0050c4b8, cWellDetailAvailable, "Available: "),
    DATA_COMPGEN(0x0050c4c4, cWellDetailSpeedS, "\n\nSpeed:\n%s"),
    DATA_COMPGEN(0x0050c4d0, cWellDetailGrowthDWeek, "\n\nGrowth\n + %d / week"),
    NULL
};
DATA(0x004ff1e8) char* cKingdomOverview[KB_KINGDOM_OVERVIEW_TEXT_COUNT] = {
    DATA_COMPGEN(0x0050c4e8, cKingdomOverviewKingdomOverviewMonthDWeekD, "Kingdom Overview     Month %d, Week %d, Day %d"),
    DATA_COMPGEN(0x0050c518, cKingdomOverviewYouOwnDragonCity, "You own Dragon City."),
    DATA_COMPGEN(0x0050c530, cKingdomOverviewYouOwnTheLighthouse, "You own the Lighthouse."),
    NULL
};
DATA(0x004ff1f8) char* cNewTurn[KB_NEW_TURN_TEXT_COUNT] = {
    DATA_COMPGEN(0x0050c548, cNewTurnSYouOnlyHaveDDays, "%s, you only have %d days left to capture a town, or you will be banished from this land."),
    DATA_COMPGEN(0x0050c5a4, cNewTurnSThisIsYourLastDay, "%s, this is your last day to capture a town, or you will be banished from this land."),
    DATA_COMPGEN(0x0050c5fc, cNewTurnAstrologersProclaimMonthOfTheS2, "Astrologers proclaim month of the %s.\n\nAll dwellings increase population."),
    DATA_COMPGEN(0x0050c648, cNewTurnAstrologersProclaimMonthOfTheS, "Astrologers proclaim month of the %s.\n\n%s population doubles!\n\nAll dwellings increase "
    "population."),
    DATA_COMPGEN(0x0050c6ac, cNewTurnAstrologersProclaimMonthOfThePLAGUE, "Astrologers proclaim month of the PLAGUE!\n\nAll populations are halved."),
    DATA_COMPGEN(0x0050c6f4, cNewTurnAstrologersProclaimWeekOfTheS2, "Astrologers proclaim week of the %s.\n\nAll dwellings increase population."),
    DATA_COMPGEN(0x0050c740, cNewTurnAstrologersProclaimWeekOfTheS, "Astrologers proclaim week of the %s.\n\n%s growth +5.\n\nAll dwellings increase population."),
    NULL
};
DATA(0x004ff218) char* cViewGeneralLabels[KB_VIEW_GENERAL_LABEL_COUNT] = {
    DATA_COMPGEN(0x0050c798, cViewGeneralLabelsAttack, "Attack: "),
    DATA_COMPGEN(0x0050c7a4, cViewGeneralLabelsDefense, "Defense: "),
    DATA_COMPGEN(0x0050c7b0, cViewGeneralLabelsSpellPower, "Spell Power: "),
    DATA_COMPGEN(0x0050c7c0, cViewGeneralLabelsKnowledge, "Knowledge: "),
    DATA_COMPGEN(0x0050c7cc, cViewGeneralLabelsMorale, "Morale: "),
    DATA_COMPGEN(0x0050c7d8, cViewGeneralLabelsLuck, "Luck: "),
    DATA_COMPGEN(0x0050c7e0, cViewGeneralLabelsSpellPoints, "Spell Points: "),
    NULL
};
DATA(0x004ff238) char* cViewGeneralHelp[KB_VIEW_GENERAL_HELP_COUNT] = {
    DATA_COMPGEN(0x0050c7f0, cViewGeneralHelpStopCatapult, "Stop Catapult"),
    DATA_COMPGEN(0x0050c800, cViewGeneralHelpCastSpell, "Cast Spell"),
    DATA_COMPGEN(0x0050c80c, cViewGeneralHelpRetreat, "Retreat"),
    DATA_COMPGEN(0x0050c814, cViewGeneralHelpSurrender, "Surrender"),
    DATA_COMPGEN(0x0050c820, cViewGeneralHelpCancel, "Cancel"),
    DATA_COMPGEN(0x0050c828, cViewGeneralHelpHeroSOptions, "Hero's Options"),
    DATA_COMPGEN(0x0050c838, cViewGeneralHelpCaptainSOptions, "Captain's Options"),
    NULL
};
DATA(0x004ff258) char* cViewGeneralLongHelp[KB_VIEW_GENERAL_LONG_HELP_COUNT] = {
    DATA_COMPGEN(0x0050c84c, cViewGeneralLongHelpCastSpellCastAMagicalSpell, "{Cast Spell}\n\nCast a magical spell.  You may only cast one spell per combat round.  The "
    "round is reset when every creature has had a turn."),
    DATA_COMPGEN(0x0050c8d8, cViewGeneralLongHelpRetreatRetreatYourHeroAbandoningYour, "{Retreat}\n\nRetreat your hero, abandoning your creatures.  Your hero will be available for "
    "you to recruit again, however, the hero will have only a novice hero's forces."),
    DATA_COMPGEN(0x0050c984, cViewGeneralLongHelpSurrenderSurrenderingCostsGoldHoweverIf, "{Surrender}\n\nSurrendering costs gold.  However, if you pay the ransom, the hero and all of "
    "his or her surviving creatures will be available to recruit again."),
    DATA_COMPGEN(0x0050ca24, cViewGeneralLongHelpCancelReturnToTheBattle, "{Cancel}\n\nReturn to the battle.")
};
DATA(0x004ff268) char* cCombatMessage[KB_COMBAT_MESSAGE_COUNT] = {
    DATA_COMPGEN(0x00500180, cCombatMessageEmptyString, ""),
    DATA_COMPGEN(0x0050ca48, cCombatMessageMoveSHere, "Move %s here."),
    DATA_COMPGEN(0x0050ca58, cCombatMessageFlySHere, "Fly %s here."),
    DATA_COMPGEN(0x0050ca68, cCombatMessageAttackS, "Attack %s"),
    DATA_COMPGEN(0x0050ca74, cCombatMessageShootSDShotSLeft, "Shoot %s(%d shot(s) left)"),
    DATA_COMPGEN(0x0050ca90, cCombatMessageHeroSOptions, "Hero's Options"),
    DATA_COMPGEN(0x0050caa0, cCombatMessageViewOpposingHero, "View Opposing Hero"),
    DATA_COMPGEN(0x0050cab4, cCombatMessageViewSInfo, "View %s info."),
    DATA_COMPGEN(0x0050cac4, cCombatMessageNoShotsLeft, "No shots left!"),
    DATA_COMPGEN(0x0050cad4, cCombatMessageCaptainSOptions, "Captain's Options"),
    DATA_COMPGEN(0x0050cae8, cCombatMessageViewOpposingCaptain, "View Opposing Captain"),
    DATA_COMPGEN(0x0050cb00, cCombatMessageViewBallistaInfo, "View Ballista Info")
};
DATA(0x004ff298) char* cHeroLevel[KB_HERO_LEVEL_TEXT_COUNT] =
    {DATA_COMPGEN(0x0050cb14, cHeroLevelSHasGained, "%s has gained"), DATA_COMPGEN(0x0050cb24, cHeroLevelALevel, " a level.\n"), DATA_COMPGEN(0x0050cb30, cHeroLevelDLevels, " %d levels.\n"), NULL};
DATA(0x004ff2a8) char* cCombatHelp[KB_COMBAT_HELP_COUNT] = {
    DATA_COMPGEN(0x0050cb40, cCombatHelpWaitForOtherUnitsToGo, "Wait for other units to go."),
    DATA_COMPGEN(0x0050cb5c, cCombatHelpSkipThisUnit, "Skip this unit"),
    DATA_COMPGEN(0x0050cb6c, cCombatHelpAutoCombat, "Auto combat"),
    DATA_COMPGEN(0x0050cb78, cCombatHelpCustomizeSystemOptions, "Customize system options."),
    DATA_COMPGEN(0x00500184, cCombatHelpEmptyString, ""),
    NULL
};
DATA(0x004ff2c0) char* cLongCombatHelp[KB_LONG_COMBAT_HELP_COUNT] = {
    DATA_COMPGEN(0x0050cb98, cLongCombatHelpWaitHaveTheCurrentCreatureWait, "{Wait}\n\nHave the current creature wait to take its turn until all the other creatures have "
    "gone.  The creature does not lose its turn."),
    DATA_COMPGEN(0x0050cc20, cLongCombatHelpSkipSkipsTheCurrentCreatureThe, "{Skip}\n\nSkips the current creature.  The current creature loses its turn and does not get "
    "to go again until the next round."),
    DATA_COMPGEN(0x0050cc9c, cLongCombatHelpAutoCombatAllowsTheComputerTo, "{Auto Combat}\n\nAllows the computer to fight out the battle for you."),
    DATA_COMPGEN(0x0050cce0, cLongCombatHelpSystemOptionsAllowsYouToCustomize, "{System Options}\n\nAllows you to customize the combat screen."),
    DATA_COMPGEN(0x0050cd20, cLongCombatHelpMessageBarShowsTheResultsOf, "{Message Bar}\n\nShows the results of individual monster's actions."),
    NULL
};
DATA(0x004ff2d8) char* cTownCommand[KB_TOWN_COMMAND_COUNT] = {
    DATA_COMPGEN(0x0050cd64, cTownCommandRedistributeSArmy, "Redistribute %s army"),
    DATA_COMPGEN(0x0050cd7c, cTownCommandCannotCombineHeroSLastArmy, "Cannot combine hero's last army"),
    DATA_COMPGEN(0x0050cd9c, cTownCommandCombineSArmies, "Combine %s armies"),
    DATA_COMPGEN(0x0050cdb0, cTownCommandRedistributeSArmy2, "Redistribute %s army"),
    DATA_COMPGEN(0x0050cdc8, cTownCommandViewS, "View %s"),
    DATA_COMPGEN(0x0050cdd0, cTownCommandCannotMoveLastArmyToGarrison, "Cannot move last army to garrison."),
    DATA_COMPGEN(0x0050cdf4, cTownCommandMoveS, "Move %s"),
    DATA_COMPGEN(0x0050cdfc, cTownCommandExchangeSWithS, "Exchange %s with %s"),
    DATA_COMPGEN(0x0050ce10, cTownCommandExitTown, "Exit town"),
    DATA_COMPGEN(0x00500188, cTownCommandEmptyString, ""),
    DATA_COMPGEN(0x0050ce20, cTownCommandKingdomOverview, "Kingdom Overview"),
    DATA_COMPGEN(0x0050ce34, cTownCommandEmpty, "Empty"),
    DATA_COMPGEN(0x0050ce3c, cTownCommandSelectS, "Select %s"),
    DATA_COMPGEN(0x0050ce48, cTownCommandViewHero, "View Hero"),
    DATA_COMPGEN(0x0050ce54, cTownCommandMageGuild, "Mage Guild"),
    DATA_COMPGEN(0x0050ce60, cTownCommandThievesGuild, "Thieves' Guild"),
    DATA_COMPGEN(0x0050ce70, cTownCommandTavern, "Tavern"),
    DATA_COMPGEN(0x0050ce78, cTownCommandDock, "Dock"),
    DATA_COMPGEN(0x0050ce80, cTownCommandWell, "Well"),
    DATA_COMPGEN(0x0050ce88, cTownCommandTent, "Tent"),
    DATA_COMPGEN(0x0050ce90, cTownCommandCastle, "Castle"),
    DATA_COMPGEN(0x0050ce98, cTownCommandRecruitS, "Recruit %s"),
    DATA_COMPGEN(0x0050cea4, cTownCommandStatue, "Statue"),
    DATA_COMPGEN(0x0050ceac, cTownCommandLeftTurret, "Left Turret"),
    DATA_COMPGEN(0x0050ceb8, cTownCommandRightTurret, "Right Turret"),
    DATA_COMPGEN(0x0050cec8, cTownCommandMoat, "Moat"),
    DATA_COMPGEN(0x0050ced0, cTownCommandMarketplace, "Marketplace"),
    DATA_COMPGEN(0x0050cedc, cTownCommandCaptainSQuarters, "Captain's Quarters")
};
DATA(0x004ff348) char* gHeroDefaultNames[KB_HERO_DEFAULT_NAME_COUNT] = {
    DATA_COMPGEN(0x0050cef0, gHeroDefaultNamesLordKilburn, "Lord Kilburn"), DATA_COMPGEN(0x0050cf00, gHeroDefaultNamesSirGallant, "Sir Gallant"), DATA_COMPGEN(0x0050cf0c, gHeroDefaultNamesEctor, "Ector"),    DATA_COMPGEN(0x0050cf14, gHeroDefaultNamesGwenneth, "Gwenneth"), DATA_COMPGEN(0x0050cf20, gHeroDefaultNamesTyro, "Tyro"),    DATA_COMPGEN(0x0050cf28, gHeroDefaultNamesAmbrose, "Ambrose"),   DATA_COMPGEN(0x0050cf30, gHeroDefaultNamesRuby, "Ruby"),
    DATA_COMPGEN(0x0050cf38, gHeroDefaultNamesMaximus, "Maximus"),      DATA_COMPGEN(0x0050cf40, gHeroDefaultNamesDimitri, "Dimitri"),     DATA_COMPGEN(0x0050cf48, gHeroDefaultNamesThundax, "Thundax"),  DATA_COMPGEN(0x0050cf50, gHeroDefaultNamesFineous, "Fineous"),  DATA_COMPGEN(0x0050cf58, gHeroDefaultNamesJojosh, "Jojosh"),  DATA_COMPGEN(0x0050cf60, gHeroDefaultNamesCragHack, "Crag Hack"), DATA_COMPGEN(0x0050cf6c, gHeroDefaultNamesJezebel, "Jezebel"),
    DATA_COMPGEN(0x0050cf74, gHeroDefaultNamesJaclyn, "Jaclyn"),       DATA_COMPGEN(0x0050cf7c, gHeroDefaultNamesErgon, "Ergon"),       DATA_COMPGEN(0x0050cf84, gHeroDefaultNamesTsabu, "Tsabu"),    DATA_COMPGEN(0x0050cf8c, gHeroDefaultNamesAtlas, "Atlas"),    DATA_COMPGEN(0x0050cf94, gHeroDefaultNamesAstra, "Astra"),   DATA_COMPGEN(0x0050cf9c, gHeroDefaultNamesNatasha, "Natasha"),   DATA_COMPGEN(0x0050cfa4, gHeroDefaultNamesTroyan, "Troyan"),
    DATA_COMPGEN(0x0050cfac, gHeroDefaultNamesVatawna, "Vatawna"),      DATA_COMPGEN(0x0050cfb4, gHeroDefaultNamesRebecca, "Rebecca"),     DATA_COMPGEN(0x0050cfbc, gHeroDefaultNamesGem, "Gem"),      DATA_COMPGEN(0x0050cfc0, gHeroDefaultNamesAriel, "Ariel"),    DATA_COMPGEN(0x0050cfc8, gHeroDefaultNamesCarlawn, "Carlawn"), DATA_COMPGEN(0x0050cfd0, gHeroDefaultNamesLuna, "Luna"),      DATA_COMPGEN(0x0050cfd8, gHeroDefaultNamesArie, "Arie"),
    DATA_COMPGEN(0x0050cfe0, gHeroDefaultNamesAlamar, "Alamar"),       DATA_COMPGEN(0x0050cfe8, gHeroDefaultNamesVesper, "Vesper"),      DATA_COMPGEN(0x0050cff0, gHeroDefaultNamesCrodo, "Crodo"),    DATA_COMPGEN(0x0050cff8, gHeroDefaultNamesBarok, "Barok"),    DATA_COMPGEN(0x0050d000, gHeroDefaultNamesKastore, "Kastore"), DATA_COMPGEN(0x0050d008, gHeroDefaultNamesAgar, "Agar"),      DATA_COMPGEN(0x0050d010, gHeroDefaultNamesFalagar, "Falagar"),
    DATA_COMPGEN(0x0050d018, gHeroDefaultNamesWrathmont, "Wrathmont"),    DATA_COMPGEN(0x0050d024, gHeroDefaultNamesMyra, "Myra"),        DATA_COMPGEN(0x0050d02c, gHeroDefaultNamesFlint, "Flint"),    DATA_COMPGEN(0x0050d034, gHeroDefaultNamesDawn, "Dawn"),     DATA_COMPGEN(0x0050d03c, gHeroDefaultNamesHalon, "Halon"),   DATA_COMPGEN(0x0050d044, gHeroDefaultNamesMyrini, "Myrini"),    DATA_COMPGEN(0x0050d04c, gHeroDefaultNamesWilfrey, "Wilfrey"),
    DATA_COMPGEN(0x0050d054, gHeroDefaultNamesSarakin, "Sarakin"),      DATA_COMPGEN(0x0050d05c, gHeroDefaultNamesKalindra, "Kalindra"),    DATA_COMPGEN(0x0050d068, gHeroDefaultNamesMandigal, "Mandigal"), DATA_COMPGEN(0x0050d074, gHeroDefaultNamesZom, "Zom"),      DATA_COMPGEN(0x0050d078, gHeroDefaultNamesDarlana, "Darlana"), DATA_COMPGEN(0x0050d080, gHeroDefaultNamesZam, "Zam"),       DATA_COMPGEN(0x0050d084, gHeroDefaultNamesRanloo, "Ranloo"),
    DATA_COMPGEN(0x0050d08c, gHeroDefaultNamesCharity, "Charity"),      DATA_COMPGEN(0x0050d094, gHeroDefaultNamesRialdo, "Rialdo"),      DATA_COMPGEN(0x0050d09c, gHeroDefaultNamesRoxana, "Roxana"),   DATA_COMPGEN(0x0050d0a4, gHeroDefaultNamesSandro, "Sandro"),   DATA_COMPGEN(0x0050d0ac, gHeroDefaultNamesCelia, "Celia")
};
DATA(0x004ff420) char* gNewGameHelp[KB_NEW_GAME_HELP_COUNT] = {
    DATA_COMPGEN(0x0050d0b4, gNewGameHelpGameDifficultyThisLetsYouChange, "{Game Difficulty}\n\nThis lets you change the starting difficulty at which you will play.  "
    "Higher difficulty levels start you off with fewer resources, and at the higher settings, give "
    "extra resources to the computer."),
    DATA_COMPGEN(0x0050d18c, gNewGameHelpHandicapThisLetsYouChangeThe, "{Handicap}\n\nThis lets you change the handicap of a particular player.  Only humans may be "
    "handicapped.  Handicapped players start with fewer resources and earn 15 or 30% fewer "
    "resources per turn for mild and severe handicaps, respectively."),
    DATA_COMPGEN(0x0050d27c, gNewGameHelpOpponentsThisLetsYouChangePlayer, "{Opponents}\n\nThis lets you change player starting positions and colors.  A particular color "
    "will always start in a particular location.  Some positions may only be played by a computer "
    "player or only by a human player."),
    DATA_COMPGEN(0x0050d358, gNewGameHelpClassThisLetsYouChangeThe, "{Class}\n\nThis lets you change the class of a player.  Classes are not always changeable.  "
    "Depending on the scenario, a player may receive additional towns and/or heroes not of their "
    "primary alignment."),
    DATA_COMPGEN(0x0050d424, gNewGameHelpScenarioClickHereToSelectWhich, "{Scenario}\n\nClick here to select which scenario to play."),
    DATA_COMPGEN(0x0050d460, gNewGameHelpDifficultyRatingTheDifficultyRatingReflects, "{Difficulty Rating}\n\nThe difficulty rating reflects a combination of various settings for "
    "your game.  This number will be applied to your final score."),
    DATA_COMPGEN(0x0050d4f8, gNewGameHelpOKClickToAcceptTheseSettings, "{OK}\n\nClick to accept these settings and start a new game."),
    DATA_COMPGEN(0x0050d534, gNewGameHelpCancelClickToReturnToThe, "{Cancel}\n\nClick to return to the main menu.")
};
DATA(0x004ff440) char* gSetupBaudHelp[KB_SETUP_BAUD_HELP_COUNT] = {
    DATA_COMPGEN(0x0050d560, gSetupBaudHelp2400BaudUseA2400Baud, "{2400 Baud}\n\nUse a 2400 baud connection speed. \n\nNote: For a 14400 baud modem, use the "
    "19200 baud speed.  For a 28800 baud modem, use the 38400 baud speed."),
    DATA_COMPGEN(0x0050d5fc, gSetupBaudHelp9600BaudUseA9600Baud, "{9600 Baud}\n\nUse a 9600 baud connection speed. \n\nNote: For a 14400 baud modem, use the "
    "19200 baud speed.  For a 28800 baud modem, use the 38400 baud speed."),
    DATA_COMPGEN(0x0050d698, gSetupBaudHelp19200BaudUseA19200Baud, "{19200 Baud}\n\nUse a 19200 baud connection speed.\n\nNote: For a 14400 baud modem, use the "
    "19200 baud speed.  For a 28800 baud modem, use the 38400 baud speed."),
    DATA_COMPGEN(0x0050d738, gSetupBaudHelp38400BaudUseA38400Baud, "{38400 Baud}\n\nUse a 38400 baud connection speed.\n\nNote: For a 14400 baud modem, use the "
    "19200 baud speed.  For a 28800 baud modem, use the 38400 baud speed."),
    DATA_COMPGEN(0x0050d7d8, gSetupBaudHelpCancelCancelBackToTheMain, "{Cancel}\n\nCancel back to the main menu."),
    NULL
};
DATA(0x004ff458) char* gSetupComPortHelp[KB_SETUP_COM_PORT_HELP_COUNT] = {
    DATA_COMPGEN(0x0050d800, gSetupComPortHelpCOM1UseCOMPort1, "{COM 1}\n\nUse COM Port 1 for the modem connection."),
    DATA_COMPGEN(0x0050d834, gSetupComPortHelpCOM2UseCOMPort2, "{COM 2}\n\nUse COM Port 2 for the modem connection."),
    DATA_COMPGEN(0x0050d868, gSetupComPortHelpCOM3UseCOMPort3, "{COM 3}\n\nUse COM Port 3 for the modem connection."),
    DATA_COMPGEN(0x0050d89c, gSetupComPortHelpCOM4UseCOMPort4, "{COM 4}\n\nUse COM Port 4 for the modem connection."),
    DATA_COMPGEN(0x0050d8d0, gSetupComPortHelpCancelCancelBackToTheMain, "{Cancel}\n\nCancel back to the main menu."),
    NULL
};
DATA(0x004ff470) char* gSetupDCBaudHelp[KB_SETUP_DC_BAUD_HELP_COUNT] = {
    DATA_COMPGEN(0x0050d8f8, gSetupDCBaudHelp2400BaudUseA2400Baud, "{2400 Baud}\n\nUse a 2400 baud connection speed. \n\nNote: In general, computers with the "
    "older UART 8250 chip should use 19200 baud, and computers with the newer UART 16550 chip "
    "should use 38400 baud.  When in doubt, try slower speeds first, and if they work, then try "
    "faster speeds.  Most computers made in 1994 or later have a UART 16550 chip."),
    DATA_COMPGEN(0x0050da50, gSetupDCBaudHelp9600BaudUseA9600Baud, "{9600 Baud}\n\nUse a 9600 baud connection speed. \n\nNote: In general, computers with the "
    "older UART 8250 chip should use 19200 baud, and computers with the newer UART 16550 chip "
    "should use 38400 baud.  When in doubt, try slower speeds first, and if they work, then try "
    "faster speeds.  Most computers made in 1994 or later have a UART 16550 chip."),
    DATA_COMPGEN(0x0050dba8, gSetupDCBaudHelp19200BaudUseA19200Baud, "{19200 Baud}\n\nUse a 19200 baud connection speed.\n\nNote: In general, computers with the "
    "older UART 8250 chip should use 19200 baud, and computers with the newer UART 16550 chip "
    "should use 38400 baud.  When in doubt, try slower speeds first, and if they work, then try "
    "faster speeds.  Most computers made in 1994 or later have a UART 16550 chip."),
    DATA_COMPGEN(0x0050dd00, gSetupDCBaudHelp38400BaudUseA38400Baud, "{38400 Baud}\n\nUse a 38400 baud connection speed.\n\nNote: In general, computers with the "
    "older UART 8250 chip should use 19200 baud, and computers with the newer UART 16550 chip "
    "should use 38400 baud.  When in doubt, try slower speeds first, and if they work, then try "
    "faster speeds.  Most computers made in 1994 or later have a UART 16550 chip."),
    DATA_COMPGEN(0x0050de58, gSetupDCBaudHelpCancelCancelBackToTheMain, "{Cancel}\n\nCancel back to the main menu."),
    NULL
};
DATA(0x004ff488) char* gSetupDCComPortHelp[KB_SETUP_DC_COM_PORT_HELP_COUNT] = {
    DATA_COMPGEN(0x0050de80, gSetupDCComPortHelpCOM1UseCOMPort1, "{COM 1}\n\nUse COM Port 1 for the direct connection."),
    DATA_COMPGEN(0x0050deb4, gSetupDCComPortHelpCOM2UseCOMPort2, "{COM 2}\n\nUse COM Port 2 for the direct connection."),
    DATA_COMPGEN(0x0050dee8, gSetupDCComPortHelpCOM3UseCOMPort3, "{COM 3}\n\nUse COM Port 3 for the direct connection."),
    DATA_COMPGEN(0x0050df1c, gSetupDCComPortHelpCom4UseCOMPort4, "{Com 4}\n\nUse COM Port 4 for the direct connection."),
    DATA_COMPGEN(0x0050df50, gSetupDCComPortHelpCancelCancelBackToTheMain, "{Cancel}\n\nCancel back to the main menu."),
    NULL
};
DATA(0x004ff4a0) char* gSetupHotSeatGameHelp[KB_SETUP_HOT_SEAT_HELP_COUNT] = {
    DATA_COMPGEN(0x0050df78, gSetupHotSeatGameHelp2PlayersPlayWith2Human, "{2 Players}\n\nPlay with 2 human players, and optionally, up to 4 additional computer "
    "players."),
    DATA_COMPGEN(0x0050dfd8, gSetupHotSeatGameHelp3PlayersPlayWith3Human, "{3 Players}\n\nPlay with 3 human players, and optionally, up to 3 additional computer "
    "players."),
    DATA_COMPGEN(0x0050e038, gSetupHotSeatGameHelp4PlayersPlayWith4Human, "{4 Players}\n\nPlay with 4 human players, and optionally, up to 2 additional computer "
    "players."),
    DATA_COMPGEN(0x0050e098, gSetupHotSeatGameHelp5PlayersPlayWith5Human, "{5 Players}\n\nPlay with 5 human players, and optionally 1 computer player."),
    DATA_COMPGEN(0x0050e0e4, gSetupHotSeatGameHelp6PlayersPlayWith6Human, "{6 Players}\n\nPlay with 6 human players."),
    DATA_COMPGEN(0x0050e10c, gSetupHotSeatGameHelpCancelCancelBackToTheMain, "{Cancel}\n\nCancel back to the main menu.")
};
DATA(0x004ff4b8) char* gSetupModemGameHelp[KB_SETUP_MODEM_HELP_COUNT] = {
    DATA_COMPGEN(0x0050e134, gSetupModemGameHelpHostTheHostSetsUpThe, "{Host}\n\nThe host sets up the game options, chooses the number to dial, and places the call."),
    DATA_COMPGEN(0x0050e190, gSetupModemGameHelpGuestTheGuestWaitsForThe, "{Guest}\n\nThe guest waits for the host to call and set up the game."),
    DATA_COMPGEN(0x0050e1d4, gSetupModemGameHelpConfigChangeYourModemConfiguration, "{Config}\n\nChange your modem configuration."),
    DATA_COMPGEN(0x0050e200, gSetupModemGameHelpCancelCancelBackToTheMain, "{Cancel}\n\nCancel back to the main menu.")
};
DATA(0x004ff4c8) char* gSetupDCGameHelp[KB_SETUP_DIRECT_CONNECT_HELP_COUNT] = {
    DATA_COMPGEN(0x0050e228, gSetupDCGameHelpHostTheHostSetsUpThe, "{Host}\n\nThe host sets up the game options."),
    DATA_COMPGEN(0x0050e254, gSetupDCGameHelpGuestTheGuestWaitsForThe, "{Guest}\n\nThe guest waits for the host to set up the game."),
    DATA_COMPGEN(0x0050e290, gSetupDCGameHelpConfigChangeYourDirectConnectPort, "{Config}\n\nChange your direct connect port configuration."),
    DATA_COMPGEN(0x0050e2cc, gSetupDCGameHelpCancelCancelBackToTheMain, "{Cancel}\n\nCancel back to the main menu.")
};
DATA(0x004ff4d8) char* gSetupMultiPlayerGameHelp[KB_SETUP_MULTIPLAYER_HELP_COUNT] = {
    DATA_COMPGEN(0x0050e2f4, gSetupMultiPlayerGameHelpHotSeatPlayAHotSeat, "{Hot Seat}\n\nPlay a Hot Seat game, where 2 to 4 players play around the same computer, "
    "switching into the 'Hot Seat' when it is their turn."),
    DATA_COMPGEN(0x0050e380, gSetupMultiPlayerGameHelpNetworkPlayANetworkGameWhere, "{Network}\n\nPlay a network game, where 2 players use their own computers connected through a "
    "LAN (Local Area Network)."),
    DATA_COMPGEN(0x0050e3f8, gSetupMultiPlayerGameHelpModemPlayAModemGameWhere, "{Modem}\n\nPlay a modem game, where 2 players use their own computers connected over the "
    "phone lines using modems."),
    DATA_COMPGEN(0x0050e46c, gSetupMultiPlayerGameHelpDirectConnectPlayADirectConnect, "{Direct Connect}\n\nPlay a direct connect game, where 2 players use their own computers "
    "directly connected through their serial port by a null modem."),
    DATA_COMPGEN(0x0050e500, gSetupMultiPlayerGameHelpCancelCancelBackToTheMain, "{Cancel}\n\nCancel back to the main menu."),
    NULL
};
DATA(0x004ff4f0) char* gSetupNetworkGameHelp[KB_SETUP_NETWORK_HELP_COUNT] = {
    DATA_COMPGEN(0x0050e528, gSetupNetworkGameHelpHostTheHostSetsUpThe, "{Host}\n\nThe host sets up the game options.  There can only be one host per network game."),
    DATA_COMPGEN(0x0050e584, gSetupNetworkGameHelpGuestTheGuestWaitsForThe, "{Guest}\n\nThe guest waits for the host to set up the game, then is automatically added in.  "
    "There can be multiple guests for TCP/IP and IPX games, but only 1 guest for NetBIOS games."),
    DATA_COMPGEN(0x0050e63c, gSetupNetworkGameHelpCancelCancelBackToTheMain, "{Cancel}\n\nCancel back to the main menu."),
    NULL
};
DATA(0x004ff500) char* gSetupNetworkGame2Help[KB_SETUP_NETWORK_SECOND_HELP_COUNT] = {
    DATA_COMPGEN(0x0050e664, gSetupNetworkGame2HelpIPXIPXNetworkingIsTheMost, "{IPX}\n\nIPX networking is the most commonly used form of network in Windows 95.  Up to 6 "
    "players can play using IPX.  Only the Windows 95 version of Heroes II supports IPX "
    "networking."),
    DATA_COMPGEN(0x0050e71c, gSetupNetworkGame2HelpTCPIPTCPIPIsMost, "{TCP/IP}\n\nTCP/IP is most commonly used as a way of connecting machines over the internet.  "
    "Up to 6 players can play using IPX.  To connect over the internet, you must have an account "
    "with an ISP (Independent Service Provider).  Only the Windows 95 version of Heroes II "
    "supports TCP/IP networking."),
    DATA_COMPGEN(0x0050e848, gSetupNetworkGame2HelpNETBiosNETBiosNetworkingIsTheOnly, "{NETBios}\n\nNETBios networking is the only form of networking available for DOS machines, "
    "and is also available for Windows 95.  Only two players may connect with NetBIOS.  If all "
    "machines you wish to connect are using the Windows 95 version of Heroes II, then IPX "
    "networking is recommended."),
    DATA_COMPGEN(0x0050e96c, gSetupNetworkGame2HelpCancelCancelBackToTheMain, "{Cancel}\n\nCancel back to the main menu.")
};
DATA(0x004ff510) char* gSetupGameHelp[KB_SETUP_GAME_HELP_COUNT] = {
    DATA_COMPGEN(0x0050e994, gSetupGameHelpStandardGameASinglePlayerGame, "{Standard Game}\n\nA single player game playing out a single map."),
    DATA_COMPGEN(0x0050e9d4, gSetupGameHelpCampaignGameASinglePlayerGame, "{Campaign Game}\n\nA single player game playing through a series of maps."),
    DATA_COMPGEN(0x0050ea1c, gSetupGameHelpMultiPlayerGameAMultiPlayer, "{Multi-Player Game}\n\nA multi-player game, with several human players competing against each "
    "other on a single map."),
    DATA_COMPGEN(0x0050ea90, gSetupGameHelpCancelCancelBackToTheMain, "{Cancel}\n\nCancel back to the main menu.")
};
DATA(0x004ff520) char* cBattleResults[KB_BATTLE_RESULT_TEXT_COUNT] = {
    DATA_COMPGEN(0x0050eab8, cBattleResultsTheEnemyHasSurrendered, "The enemy has surrendered!"),
    DATA_COMPGEN(0x0050ead4, cBattleResultsTheEnemyHasFled, "The enemy has fled!"),
    DATA_COMPGEN(0x0050eae8, cBattleResultsAGloriousVictory, "A glorious victory!"),
    DATA_COMPGEN(0x0050eafc, cBattleResultsForValorInCombatSReceives, "\n\nFor valor in combat, %s receives %d experience"),
    DATA_COMPGEN(0x0050eb30, cBattleResultsSSurrendersToTheEnemyAnd, "%s surrenders to the enemy, and departs in shame."),
    DATA_COMPGEN(0x0050eb64, cBattleResultsTheCowardlySFleesFromBattle, "The cowardly %s flees from battle."),
    DATA_COMPGEN(0x0050eb88, cBattleResultsYourForcesSufferABitterDefeat, "Your forces suffer a bitter defeat, and %s abandons your cause."),
    DATA_COMPGEN(0x0050ebc8, cBattleResultsYourForcesSurrenderToTheEnemy, "Your forces surrender to the enemy, and depart in shame."),
    DATA_COMPGEN(0x0050ec04, cBattleResultsYourCowardlyForcesFleeFromBattle, "Your cowardly forces flee from battle."),
    DATA_COMPGEN(0x0050ec2c, cBattleResultsYourForcesSufferABitterDefeat2, "Your forces suffer a bitter defeat."),
    DATA_COMPGEN(0x0050ec50, cBattleResultsForValorInCombatSReceives2, "\n\nFor valor in combat, %s receives %d experience, and gains %d level(s)."),
    NULL
};
DATA(0x004ff550) char* cMoraleInfo[KB_MORALE_INFO_TEXT_COUNT] = {
    DATA_COMPGEN(0x0050ec9c, cMoraleInfoGoodMoraleGoodMoraleMayGive, "{Good Morale}\n\nGood morale may give your armies extra attacks in combat."),
    DATA_COMPGEN(0x0050ece8, cMoraleInfoNeutralMoraleNeutralMoraleMeansYour, "{Neutral Morale}\n\nNeutral morale means your armies will never be blessed with extra attacks "
    "or freeze in combat."),
    DATA_COMPGEN(0x0050ed5c, cMoraleInfoBadMoraleBadMoraleMayCause, "{Bad Morale}\n\nBad morale may cause your armies to freeze in combat."),
    DATA_COMPGEN(0x0050eda0, cMoraleInfoSCurrentMoraleModifiers, "%s\n\n\nCurrent Morale Modifiers:"),
    DATA_COMPGEN(0x0050edc0, cMoraleInfoKnightBonus1, "\nKnight bonus +1"),
    DATA_COMPGEN(0x0050edd4, cMoraleInfoAllSTroops1, "\nAll %s troops +1"),
    DATA_COMPGEN(0x0050ede8, cMoraleInfoTroopsOf3Alignments1, "\nTroops of 3 alignments -1"),
    DATA_COMPGEN(0x0050ee04, cMoraleInfoTroopsOf4Alignments2, "\nTroops of 4 alignments -2"),
    DATA_COMPGEN(0x0050ee20, cMoraleInfoMedalOfValor1, "\nMedal of Valor +1"),
    DATA_COMPGEN(0x0050ee34, cMoraleInfoMedalOfCourage1, "\nMedal of Courage +1"),
    DATA_COMPGEN(0x0050ee4c, cMoraleInfoMedalOfHonor1, "\nMedal of Honor +1"),
    DATA_COMPGEN(0x0050ee60, cMoraleInfoMedalOfDistinction1, "\nMedal of Distinction +1"),
    DATA_COMPGEN(0x0050ee7c, cMoraleInfoFizbinOfMisfortune2, "\nFizbin of Misfortune -2"),
    DATA_COMPGEN(0x0050ee98, cMoraleInfoBuoyVisited1, "\nBuoy visited +1"),
    DATA_COMPGEN(0x0050eeac, cMoraleInfoOasisVisited1, "\nOasis visited +1"),
    DATA_COMPGEN(0x0050eec0, cMoraleInfoTempleVisited2, "\nTemple visited +2"),
    DATA_COMPGEN(0x0050eed4, cMoraleInfoGraveyardRobber1, "\nGraveyard robber -1"),
    DATA_COMPGEN(0x0050eeec, cMoraleInfoShipwreckRobber1, "\nShipwreck robber -1"),
    DATA_COMPGEN(0x0050ef04, cMoraleInfoBattleCowardiceD, "\nBattle cowardice %d"),
    DATA_COMPGEN(0x0050ef1c, cMoraleInfoNone, "\nnone"),
    DATA_COMPGEN(0x0050ef24, cMoraleInfoTroopsOf5Alignments3, "\nTroops of 5 alignments -3"),
    DATA_COMPGEN(0x0050ef40, cMoraleInfoEntireUnitIsUndeadSoMorale, "\nEntire unit is undead, so morale does not apply."),
    DATA_COMPGEN(0x0050ef74, cMoraleInfoSomeUndeadInGroup1, "\nSome undead in group -1"),
    DATA_COMPGEN(0x0050ef90, cMoraleInfoWateringHoleVisited1, "\nWatering hole visited +1"),
    DATA_COMPGEN(0x0050efac, cMoraleInfoDerelictShipRobber1, "\nDerelict ship robber -1"),
    DATA_COMPGEN(0x0050efc8, cMoraleInfoBarbarianColiseum2, "\nBarbarian Coliseum +2"),
    DATA_COMPGEN(0x0050efe0, cMoraleInfoTavern1, "\nTavern +1"),
    DATA_COMPGEN(0x0050efec, cMoraleInfoBasicLeadership1, "\nBasic Leadership +1"),
    DATA_COMPGEN(0x0050f004, cMoraleInfoAdvancedLeadership2, "\nAdvanced Leadership +2"),
    DATA_COMPGEN(0x0050f01c, cMoraleInfoExpertLeadership3, "\nExpert Leadership +3"),
    DATA_COMPGEN(0x0050f034, cMoraleInfoMastheadBonusAtSea1, "\nMasthead bonus at sea +1"),
    DATA_COMPGEN(0x0050f050, cMoraleInfoBattleGarbOfAnduranGivesYou, "\nBattle Garb of Anduran gives you maximum morale.")
};
DATA(0x004ff5d0) char* cMapSize[KB_MAP_SIZE_TEXT_COUNT] = {DATA_COMPGEN(0x0050f084, cMapSizeSmall, "Small"), DATA_COMPGEN(0x0050f08c, cMapSizeMedium, "Medium"), DATA_COMPGEN(0x0050f094, cMapSizeLarge, "Large"), DATA_COMPGEN(0x0050f09c, cMapSizeHuge, "Huge")};
DATA(0x004ff5e0) char* cDifficulty[KB_DIFFICULTY_TEXT_COUNT] =
    {DATA_COMPGEN(0x0050f0a4, cDifficultyEasy, "Easy"), DATA_COMPGEN(0x0050f0ac, cDifficultyNormal, "Normal"), DATA_COMPGEN(0x0050f0b4, cDifficultyHard, "Hard"), DATA_COMPGEN(0x0050f0bc, cDifficultyExpert, "Expert"), DATA_COMPGEN(0x0050f0c4, cDifficultyImpossible, "Impossible"), NULL};
DATA(0x004ff5f8) char* cStartDifficulty[KB_START_DIFFICULTY_TEXT_COUNT] =
    {DATA_COMPGEN(0x0050f0d0, cStartDifficultyEasy, "Easy"), DATA_COMPGEN(0x0050f0d8, cStartDifficultyNormal, "Normal"), DATA_COMPGEN(0x0050f0e0, cStartDifficultyHard, "Hard"), DATA_COMPGEN(0x0050f0e8, cStartDifficultyExpert, "Expert")};
DATA(0x004ff608) char* cCampaignLeaders[KB_CAMPAIGN_LEADER_TEXT_COUNT] =
    {DATA_COMPGEN(0x0050f0f0, cCampaignLeadersLordIronfist, "Lord Ironfist"), DATA_COMPGEN(0x0050f100, cCampaignLeadersLordSlayer, "Lord Slayer"), DATA_COMPGEN(0x0050f10c, cCampaignLeadersQueenLamanda, "Queen Lamanda"), DATA_COMPGEN(0x0050f11c, cCampaignLeadersLordAlamar, "Lord Alamar")};
DATA(0x004ff618) char* cWinText[KB_WIN_TEXT_COUNT] =
    {DATA_COMPGEN(0x0050f128, cWinTextDaysSpent, "Days Spent:"), DATA_COMPGEN(0x0050f134, cWinTextBaseScore, "Base Score:"), DATA_COMPGEN(0x0050f140, cWinTextDifficultyRating, "Difficulty Rating:"), DATA_COMPGEN(0x0050f154, cWinTextFinalScore, "Final Score:"), DATA_COMPGEN(0x0050f164, cWinTextRanking, "Ranking:"), NULL};
DATA(0x004ff630) char* cHumanDifficulty[KB_HUMAN_DIFFICULTY_TEXT_COUNT] =
    {DATA_COMPGEN(0x0050f170, cHumanDifficultyHuman, "Human\n"), DATA_COMPGEN(0x0050f178, cHumanDifficultyHumanEasy, "Human\nEasy"), DATA_COMPGEN(0x0050f184, cHumanDifficultyHumanNormal, "Human\nNormal"), DATA_COMPGEN(0x0050f194, cHumanDifficultyHumanHard, "Human\nHard"), DATA_COMPGEN(0x0050f1a0, cHumanDifficultyHumanExpert, "Human\nExpert"), NULL};
DATA(0x004ff648) char* cHumanInfoDifficulty[KB_HUMAN_INFO_DIFFICULTY_TEXT_COUNT] =
    {DATA_COMPGEN(0x0050f1b0, cHumanInfoDifficultyHuman, "Human-"), DATA_COMPGEN(0x0050f1b8, cHumanInfoDifficultyHumanEasy, "Human-Easy"), DATA_COMPGEN(0x0050f1c4, cHumanInfoDifficultyHumanNormal, "Human-Normal"), DATA_COMPGEN(0x0050f1d4, cHumanInfoDifficultyHumanHard, "Human-Hard"), DATA_COMPGEN(0x0050f1e0, cHumanInfoDifficultyHumanExpert, "Human-Expert"), NULL};
DATA(0x004ff660) char* musicQualityText[KB_MUSIC_QUALITY_TEXT_COUNT] =
    {DATA_COMPGEN(0x0050f1f0, musicQualityTextMIDI, "MIDI"), DATA_COMPGEN(0x0050f1f8, musicQualityTextCDStereoWOOpera, "CD Stereo w/o Opera"), DATA_COMPGEN(0x0050f20c, musicQualityTextCDStereoWithOpera, "CD Stereo with Opera"), NULL};
DATA(0x004ff670) char* gSpellDesc[KB_SPELL_TEXT_COUNT] = {
    DATA_COMPGEN(0x0050f224, gSpellDescFireballCausesAGiantFireballTo, "{Fireball}\n\nCauses a giant fireball to strike the selected area, damaging all nearby "
    "creatures."),
    DATA_COMPGEN(0x0050f284, gSpellDescFireblastAnImprovedVersionOfFireball, "{Fireblast}\n\nAn improved version of fireball, fireblast affects two hexes around the center "
    "point of the spell, rather than one."),
    DATA_COMPGEN(0x0050f308, gSpellDescLightningBoltCausesABoltOf, "{Lightning Bolt}\n\nCauses a bolt of electrical energy to strike the selected creature."),
    DATA_COMPGEN(0x0050f360, gSpellDescChainLightningCausesABoltOf, "{Chain Lightning}\n\nCauses a bolt of electrical energy to strike a selected creature, then "
    "strike the nearest creature with half damage, then strike the NEXT nearest creature with half "
    "again damage, and so on, until it becomes too weak to be harmful.  Warning:  This spell can "
    "hit your own creatures!"),
    DATA_COMPGEN(0x0050f48c, gSpellDescTeleportTeleportsTheCreatureYouSelect, "{Teleport}\n\nTeleports the creature you select to any open position on the battlefield."),
    DATA_COMPGEN(0x0050f4e4, gSpellDescCureRemovesAllNegativeSpellsCast, "{Cure}\n\nRemoves all negative spells cast upon one of your units, and restores up to 5 HP "
    "per level of spell power."),
    DATA_COMPGEN(0x0050f558, gSpellDescMassCureRemovesAllNegativeSpells, "{Mass Cure}\n\nRemoves all negative spells cast upon your forces, and restores up to 5 HP per "
    "level of spell power, per creature."),
    DATA_COMPGEN(0x0050f5d8, gSpellDescResurrectResurrectsCreaturesFromADamaged, "{Resurrect}\n\nResurrects creatures from a damaged or dead unit until end of combat."),
    DATA_COMPGEN(0x0050f62c, gSpellDescResurrectTrueResurrectsCreaturesFromA, "{Resurrect True}\n\nResurrects creatures from a damaged or dead unit permanently."),
    DATA_COMPGEN(0x0050f67c, gSpellDescHasteIncreasesTheSpeedOfAny, "{Haste}\n\nIncreases the speed of any creature by two."),
    DATA_COMPGEN(0x0050f6b4, gSpellDescMassHasteIncreasesTheSpeedOf, "{Mass Haste}\n\nIncreases the speed of all of your creatures by two."),
    DATA_COMPGEN(0x0050f6f8, gSpellDescSlowSlowsTargetToHalfMovement, "{Slow}\n\nSlows target to half movement rate."),
    DATA_COMPGEN(0x0050f724, gSpellDescMassSlowSlowsAllEnemiesTo, "{Mass Slow}\n\nSlows all enemies to half movement rate."),
    DATA_COMPGEN(0x0050f75c, gSpellDescBlindCloudsTheAffectedCreaturesEyes, "{Blind }\n\nClouds the affected creatures' eyes, preventing them from moving."),
    DATA_COMPGEN(0x0050f7a8, gSpellDescBlessCausesTheSelectedCreaturesTo, "{Bless}\n\nCauses the selected creatures to inflict maximum damage."),
    DATA_COMPGEN(0x0050f7ec, gSpellDescMassBlessCausesAllOfYour, "{Mass Bless}\n\nCauses all of your units to inflict maximum damage."),
    DATA_COMPGEN(0x0050f830, gSpellDescStoneskinMagicallyIncreasesTheDefenseSkill, "{Stoneskin}\n\nMagically increases the defense skill of the selected creatures."),
    DATA_COMPGEN(0x0050f880, gSpellDescSteelskinIncreasesTheDefenseSkillOf, "{Steelskin}\n\nIncreases the defense skill of the targeted creatures.  This is an improved "
    "version of Stoneskin."),
    DATA_COMPGEN(0x0050f8f0, gSpellDescCurseCausesTheSelectedCreaturesTo, "{Curse}\n\nCauses the selected creatures to inflict minimum damage."),
    DATA_COMPGEN(0x0050f934, gSpellDescMassCurseCausesAllEnemyTroops, "{Mass Curse}\n\nCauses all enemy troops to inflict minimum damage."),
    DATA_COMPGEN(0x0050f978, gSpellDescHolyWordDamagesAllUndeadIn, "{Holy Word}\n\nDamages all undead in the battle."),
    DATA_COMPGEN(0x0050f9a8, gSpellDescHolyShoutDamagesAllUndeadIn, "{Holy Shout}\n\nDamages all undead in the battle.  This is an improved version of Holy Word."),
    DATA_COMPGEN(0x0050fa04, gSpellDescAntiMagicPreventsHarmfulMagicAgainst, "{Anti-Magic}\n\nPrevents harmful magic against the selected creatures."),
    DATA_COMPGEN(0x0050fa4c, gSpellDescDispelMagicRemovesAllMagicSpells, "{Dispel Magic}\n\nRemoves all magic spells from a single target."),
    DATA_COMPGEN(0x0050fa8c, gSpellDescMassDispelRemovesAllMagicSpells, "{Mass Dispel}\n\nRemoves all magic spells from all creatures."),
    DATA_COMPGEN(0x0050fac8, gSpellDescMagicArrowCausesAMagicArrow, "{Magic Arrow}\n\nCauses a magic arrow to strike the selected target."),
    DATA_COMPGEN(0x0050fb0c, gSpellDescBerzerkerCausesACreatureToAttack, "{Berzerker}\n\nCauses a creature to attack its nearest neighbor."),
    DATA_COMPGEN(0x0050fb4c, gSpellDescArmageddonHolyTerrorStrikesTheBattlefield, "{Armageddon}\n\nHoly terror strikes the battlefield, causing severe damage to all creatures."),
    DATA_COMPGEN(0x0050fba8, gSpellDescElementalStormMagicalElementsPourDown, "{Elemental Storm}\n\nMagical elements pour down on the battlefield, damaging all creatures."),
    DATA_COMPGEN(0x0050fc04, gSpellDescMeteorShowerARainOfRocks, "{Meteor Shower}\n\nA rain of rocks strikes an area of the battlefield, damaging all nearby "
    "creatures."),
    DATA_COMPGEN(0x0050fc68, gSpellDescParalyzeTheTargetedCreaturesAreParalyzed, "{Paralyze}\n\nThe targeted creatures are paralyzed, unable to move or retaliate."),
    DATA_COMPGEN(0x0050fcb8, gSpellDescHypnotizeBringsASingleEnemyUnit, "{Hypnotize}\n\nBrings a single enemy unit under your control for one combat round if its hits "
    "are less than 25 times the caster's spell power."),
    DATA_COMPGEN(0x0050fd48, gSpellDescColdRayDrainsBodyHeatFrom, "{Cold Ray}\n\nDrains body heat from a single enemy unit."),
    DATA_COMPGEN(0x0050fd80, gSpellDescColdRingDrainsBodyHeatFrom, "{Cold Ring}\n\nDrains body heat from all units surrounding the center point, but not "
    "including the center point."),
    DATA_COMPGEN(0x0050fdf0, gSpellDescDisruptingRayReducesTheDefenseRating, "{Disrupting Ray}\n\nReduces the defense rating of an enemy unit by three."),
    DATA_COMPGEN(0x0050fe38, gSpellDescDeathRippleDamagesAllLivingNon, "{Death Ripple}\n\nDamages all living (non-undead) units in the battle."),
    DATA_COMPGEN(0x0050fe80, gSpellDescDeathWaveDamagesAllLivingNon, "{Death Wave}\n\nDamages all living (non-undead) units in the battle.  This spell is an "
    "improved version of Death Ripple."),
    DATA_COMPGEN(0x0050fef8, gSpellDescDragonSlayerGreatlyIncreasesAUnit, "{Dragon Slayer}\n\nGreatly increases a unit's attack skill vs. Dragons."),
    DATA_COMPGEN(0x0050ff40, gSpellDescBloodLustIncreasesAUnitS, "{Blood Lust}\n\nIncreases a unit's attack skill."),
    DATA_COMPGEN(0x0050ff70, gSpellDescAnimateDeadResurrectsCreaturesFromA, "{Animate Dead}\n\n'Resurrects' creatures from a damaged or dead undead unit permanently."),
    DATA_COMPGEN(0x0050ffc8, gSpellDescMirrorImageCreatesAnIllusionaryUnit, "{Mirror Image}\n\nCreates an illusionary unit that duplicates one of your existing units.  "
    "This illusionary unit does the same damages as the original, but will vanish if it takes any "
    "damage."),
    DATA_COMPGEN(0x00510088, gSpellDescShieldHalvesDamageReceivedFromRanged, "{Shield}\n\nHalves damage received from ranged attacks for a single unit."),
    DATA_COMPGEN(0x005100d0, gSpellDescMassShieldHalvesDamageReceivedFrom, "{Mass Shield}\n\nHalves damage received from ranged attacks for all of your units."),
    DATA_COMPGEN(0x00510124, gSpellDescSummonEarthElementalSummonsEarthElementals, "{Summon Earth Elemental}\n\nSummons Earth Elementals to fight for your army."),
    DATA_COMPGEN(0x00510170, gSpellDescSummonAirElementalSummonsAirElementals, "{Summon Air Elemental}\n\nSummons Air Elementals to fight for your army."),
    DATA_COMPGEN(0x005101b8, gSpellDescSummonFireElementalSummonsFireElementals, "{Summon Fire Elemental}\n\nSummons Fire Elementals to fight for your army."),
    DATA_COMPGEN(0x00510204, gSpellDescSummonWaterElementalSummonsWaterElementals, "{Summon Water Elemental}\n\nSummons Water Elementals to fight for your army."),
    DATA_COMPGEN(0x00510250, gSpellDescEarthquakeDamagesCastleWalls, "{Earthquake}\n\nDamages castle walls."),
    DATA_COMPGEN(0x00510274, gSpellDescViewMinesCausesAllMinesAcross, "{View Mines}\n\nCauses all mines across the land to become visible."),
    DATA_COMPGEN(0x005102b8, gSpellDescViewResourcesCausesAllResourcesAcross, "{View Resources}\n\nCauses all resources across the land to become visible."),
    DATA_COMPGEN(0x00510304, gSpellDescViewArtifactsCausesAllArtifactsAcross, "{View Artifacts}\n\nCauses all artifacts across the land to become visible."),
    DATA_COMPGEN(0x00510350, gSpellDescViewTownsCausesAllTownsAnd, "{View Towns}\n\nCauses all towns and castles across the land to become visible."),
    DATA_COMPGEN(0x005103a0, gSpellDescViewHeroesCausesAllHeroesAcross, "{View Heroes}\n\nCauses all Heroes across the land to become visible."),
    DATA_COMPGEN(0x005103e4, gSpellDescViewAllCausesTheEntireLand, "{View All}\n\nCauses the entire land to become visible."),
    DATA_COMPGEN(0x0051041c, gSpellDescIdentifyHeroAllowsTheCasterTo, "{Identify Hero}\n\nAllows the caster to view detailed information on enemy Heroes."),
    DATA_COMPGEN(0x00510470, gSpellDescSummonBoatSummonsTheNearestUnoccupied, "{Summon Boat}\n\nSummons the nearest unoccupied, friendly boat to an adjacent shore location. "
    " A friendly boat is one which you just built or were the most recent player to occupy."),
    DATA_COMPGEN(0x00510524, gSpellDescDimensionDoorAllowsTheCasterTo, "{Dimension Door}\n\nAllows the caster to magically transport to a nearby location."),
    DATA_COMPGEN(0x00510578, gSpellDescTownGateReturnsTheCasterTo, "{Town Gate}\n\nReturns the caster to any town or castle currently owned."),
    DATA_COMPGEN(0x005105c0, gSpellDescTownPortalReturnsTheHeroTo, "{Town Portal}\n\nReturns the hero to the town or castle of choice, provided it is controlled "
    "by you."),
    DATA_COMPGEN(0x00510624, gSpellDescVisionsVisionsPredictsTheLikelyOutcome, "{Visions}\n\n Visions predicts the likely outcome of an encounter with a neutral army camp."),
    DATA_COMPGEN(0x00510680, gSpellDescHauntHauntsAMineYouControl, "{Haunt}\n\nHaunts a mine you control with Ghosts.  This mine stops producing resources.  (If "
    "I can't keep it, nobody will!)"),
    DATA_COMPGEN(0x005106fc, gSpellDescSetEarthGuardianSetsEarthElementals, "{Set Earth Guardian}\n\nSets Earth Elementals to guard a mine against enemy armies."),
    DATA_COMPGEN(0x00510750, gSpellDescSetAirGuardianSetsAirElementals, "{Set Air Guardian}\n\nSets Air Elementals to guard a mine against enemy armies."),
    DATA_COMPGEN(0x005107a0, gSpellDescSetFireGuardianSetsFireElementals, "{Set Fire Guardian}\n\nSets Fire Elementals to guard a mine against enemy armies."),
    DATA_COMPGEN(0x005107f0, gSpellDescSetWaterGuardianSetsWaterElementals, "{Set Water Guardian}\n\nSets Water Elementals to guard a mine against enemy armies."),
    NULL
};
DATA(0x004ff778) char* gSpellNames[KB_SPELL_TEXT_COUNT] = {
    DATA_COMPGEN(0x00510844, gSpellNamesFireball, "Fireball"),
    DATA_COMPGEN(0x00510850, gSpellNamesFireblast, "Fireblast"),
    DATA_COMPGEN(0x0051085c, gSpellNamesLightningBolt, "Lightning Bolt"),
    DATA_COMPGEN(0x0051086c, gSpellNamesChainLightning, "Chain Lightning"),
    DATA_COMPGEN(0x0051087c, gSpellNamesTeleport, "Teleport"),
    DATA_COMPGEN(0x00510888, gSpellNamesCure, "Cure"),
    DATA_COMPGEN(0x00510890, gSpellNamesMassCure, "Mass Cure"),
    DATA_COMPGEN(0x0051089c, gSpellNamesResurrect, "Resurrect"),
    DATA_COMPGEN(0x005108a8, gSpellNamesResurrectTrue, "Resurrect True"),
    DATA_COMPGEN(0x005108b8, gSpellNamesHaste, "Haste"),
    DATA_COMPGEN(0x005108c0, gSpellNamesMassHaste, "Mass Haste"),
    DATA_COMPGEN(0x005108cc, gSpellNamesSlow, "Slow"),
    DATA_COMPGEN(0x005108d4, gSpellNamesMassSlow, "Mass Slow"),
    DATA_COMPGEN(0x005108e0, gSpellNamesBlind, "Blind "),
    DATA_COMPGEN(0x005108e8, gSpellNamesBless, "Bless"),
    DATA_COMPGEN(0x005108f0, gSpellNamesMassBless, "Mass Bless"),
    DATA_COMPGEN(0x005108fc, gSpellNamesStoneskin, "Stoneskin"),
    DATA_COMPGEN(0x00510908, gSpellNamesSteelskin, "Steelskin"),
    DATA_COMPGEN(0x00510914, gSpellNamesCurse, "Curse"),
    DATA_COMPGEN(0x0051091c, gSpellNamesMassCurse, "Mass Curse"),
    DATA_COMPGEN(0x00510928, gSpellNamesHolyWord, "Holy Word"),
    DATA_COMPGEN(0x00510934, gSpellNamesHolyShout, "Holy Shout"),
    DATA_COMPGEN(0x00510940, gSpellNamesAntiMagic, "Anti-Magic"),
    DATA_COMPGEN(0x0051094c, gSpellNamesDispelMagic, "Dispel Magic"),
    DATA_COMPGEN(0x0051095c, gSpellNamesMassDispel, "Mass Dispel"),
    DATA_COMPGEN(0x00510968, gSpellNamesMagicArrow, "Magic Arrow"),
    DATA_COMPGEN(0x00510974, gSpellNamesBerzerker, "Berzerker"),
    DATA_COMPGEN(0x00510980, gSpellNamesArmageddon, "Armageddon"),
    DATA_COMPGEN(0x0051098c, gSpellNamesElementalStorm, "Elemental Storm"),
    DATA_COMPGEN(0x0051099c, gSpellNamesMeteorShower, "Meteor Shower"),
    DATA_COMPGEN(0x005109ac, gSpellNamesParalyze, "Paralyze"),
    DATA_COMPGEN(0x005109b8, gSpellNamesHypnotize, "Hypnotize"),
    DATA_COMPGEN(0x005109c4, gSpellNamesColdRay, "Cold Ray"),
    DATA_COMPGEN(0x005109d0, gSpellNamesColdRing, "Cold Ring"),
    DATA_COMPGEN(0x005109dc, gSpellNamesDisruptingRay, "Disrupting Ray"),
    DATA_COMPGEN(0x005109ec, gSpellNamesDeathRipple, "Death Ripple"),
    DATA_COMPGEN(0x005109fc, gSpellNamesDeathWave, "Death Wave"),
    DATA_COMPGEN(0x00510a08, gSpellNamesDragonSlayer, "Dragon Slayer"),
    DATA_COMPGEN(0x00510a18, gSpellNamesBloodLust, "Blood Lust"),
    DATA_COMPGEN(0x00510a24, gSpellNamesAnimateDead, "Animate Dead"),
    DATA_COMPGEN(0x00510a34, gSpellNamesMirrorImage, "Mirror Image"),
    DATA_COMPGEN(0x00510a44, gSpellNamesShield, "Shield"),
    DATA_COMPGEN(0x00510a4c, gSpellNamesMassShield, "Mass Shield"),
    DATA_COMPGEN(0x00510a58, gSpellNamesSummonEarthElemental, "Summon Earth Elemental"),
    DATA_COMPGEN(0x00510a70, gSpellNamesSummonAirElemental, "Summon Air Elemental"),
    DATA_COMPGEN(0x00510a88, gSpellNamesSummonFireElemental, "Summon Fire Elemental"),
    DATA_COMPGEN(0x00510aa0, gSpellNamesSummonWaterElemental, "Summon Water Elemental"),
    DATA_COMPGEN(0x00510ab8, gSpellNamesEarthquake, "Earthquake"),
    DATA_COMPGEN(0x00510ac4, gSpellNamesViewMines, "View Mines"),
    DATA_COMPGEN(0x00510ad0, gSpellNamesViewResources, "View Resources"),
    DATA_COMPGEN(0x00510ae0, gSpellNamesViewArtifacts, "View Artifacts"),
    DATA_COMPGEN(0x00510af0, gSpellNamesViewTowns, "View Towns"),
    DATA_COMPGEN(0x00510afc, gSpellNamesViewHeroes, "View Heroes"),
    DATA_COMPGEN(0x00510b08, gSpellNamesViewAll, "View All"),
    DATA_COMPGEN(0x00510b14, gSpellNamesIdentifyHero, "Identify Hero"),
    DATA_COMPGEN(0x00510b24, gSpellNamesSummonBoat, "Summon Boat"),
    DATA_COMPGEN(0x00510b30, gSpellNamesDimensionDoor, "Dimension Door"),
    DATA_COMPGEN(0x00510b40, gSpellNamesTownGate, "Town Gate"),
    DATA_COMPGEN(0x00510b4c, gSpellNamesTownPortal, "Town Portal"),
    DATA_COMPGEN(0x00510b58, gSpellNamesVisions, "Visions"),
    DATA_COMPGEN(0x00510b60, gSpellNamesHaunt, "Haunt"),
    DATA_COMPGEN(0x00510b68, gSpellNamesSetEarthGuardian, "Set Earth Guardian"),
    DATA_COMPGEN(0x00510b7c, gSpellNamesSetAirGuardian, "Set Air Guardian"),
    DATA_COMPGEN(0x00510b90, gSpellNamesSetFireGuardian, "Set Fire Guardian"),
    DATA_COMPGEN(0x00510ba4, gSpellNamesSetWaterGuardian, "Set Water Guardian"),
    NULL
};
DATA(0x004ff880) char* gSecondarySkillLevels[KB_SECONDARY_SKILL_LEVEL_TEXT_COUNT] =
    {DATA_COMPGEN(0x00510bb8, gSecondarySkillLevelsBasic, "Basic"), DATA_COMPGEN(0x00510bc0, gSecondarySkillLevelsAdvanced, "Advanced"), DATA_COMPGEN(0x00510bcc, gSecondarySkillLevelsExpert, "Expert"), NULL};
DATA(0x004ff890) char* gSecondarySkills[KB_SECONDARY_SKILL_TEXT_COUNT] = {
    DATA_COMPGEN(0x00510bd4, gSecondarySkillsPathfinding, "Pathfinding"),
    DATA_COMPGEN(0x00510be0, gSecondarySkillsArchery, "Archery"),
    DATA_COMPGEN(0x00510be8, gSecondarySkillsLogistics, "Logistics"),
    DATA_COMPGEN(0x00510bf4, gSecondarySkillsScouting, "Scouting"),
    DATA_COMPGEN(0x00510c00, gSecondarySkillsDiplomacy, "Diplomacy"),
    DATA_COMPGEN(0x00510c0c, gSecondarySkillsNavigation, "Navigation"),
    DATA_COMPGEN(0x00510c18, gSecondarySkillsLeadership, "Leadership"),
    DATA_COMPGEN(0x00510c24, gSecondarySkillsWisdom, "Wisdom"),
    DATA_COMPGEN(0x00510c2c, gSecondarySkillsMysticism, "Mysticism"),
    DATA_COMPGEN(0x00510c38, gSecondarySkillsLuck, "Luck"),
    DATA_COMPGEN(0x00510c40, gSecondarySkillsBallistics, "Ballistics"),
    DATA_COMPGEN(0x00510c4c, gSecondarySkillsEagleEye, "Eagle Eye"),
    DATA_COMPGEN(0x00510c58, gSecondarySkillsNecromancy, "Necromancy"),
    DATA_COMPGEN(0x00510c64, gSecondarySkillsEstates, "Estates")
};
DATA(0x004ff8c8) char* gNeutralBuildingNames[KB_NEUTRAL_BUILDING_TEXT_COUNT] = {
    DATA_COMPGEN(0x00510c6c, gNeutralBuildingNamesMageGuild, "Mage Guild"),
    DATA_COMPGEN(0x00510c78, gNeutralBuildingNamesThievesGuild, "Thieves' Guild"),
    DATA_COMPGEN(0x00510c88, gNeutralBuildingNamesTavern, "Tavern"),
    DATA_COMPGEN(0x00510c90, gNeutralBuildingNamesShipyard, "Shipyard"),
    DATA_COMPGEN(0x00510c9c, gNeutralBuildingNamesWell, "Well"),
    DATA_COMPGEN(0x00510ca4, gNeutralBuildingNamesTent, "Tent"),
    DATA_COMPGEN(0x00510cac, gNeutralBuildingNamesCastle, "Castle"),
    DATA_COMPGEN(0x00510cb4, gNeutralBuildingNamesStatue, "Statue"),
    DATA_COMPGEN(0x00510cbc, gNeutralBuildingNamesLeftTurret, "Left Turret"),
    DATA_COMPGEN(0x00510cc8, gNeutralBuildingNamesRightTurret, "Right Turret"),
    DATA_COMPGEN(0x00510cd8, gNeutralBuildingNamesMarketplace, "Marketplace"),
    DATA_COMPGEN(0x0050018c, gNeutralBuildingNamesEmptyString, ""),
    DATA_COMPGEN(0x00510ce8, gNeutralBuildingNamesMoat, "Moat"),
    DATA_COMPGEN(0x00500190, gNeutralBuildingNamesEmptyString2, ""),
    DATA_COMPGEN(0x00510cf4, gNeutralBuildingNamesDockWithBoat, "Dock with boat"),
    DATA_COMPGEN(0x00510d04, gNeutralBuildingNamesCaptainSQuarters, "Captain's Quarters"),
    DATA_COMPGEN(0x00500194, gNeutralBuildingNamesEmptyString3, ""),
    DATA_COMPGEN(0x00500198, gNeutralBuildingNamesEmptyString4, ""),
    DATA_COMPGEN(0x0050019c, gNeutralBuildingNamesEmptyString5, ""),
    NULL
};
DATA(0x004ff918) char* gWellExtraNames[KB_WELL_EXTRA_NAME_COUNT] = {
    DATA_COMPGEN(0x00510d24, gWellExtraNamesFarm, "Farm"),
    DATA_COMPGEN(0x00510d2c, gWellExtraNamesGarbageHeap, "Garbage Heap"),
    DATA_COMPGEN(0x00510d3c, gWellExtraNamesCrystalGarden, "Crystal Garden"),
    DATA_COMPGEN(0x00510d4c, gWellExtraNamesWaterfall, "Waterfall"),
    DATA_COMPGEN(0x00510d58, gWellExtraNamesOrchard, "Orchard"),
    DATA_COMPGEN(0x00510d60, gWellExtraNamesSkullPile, "Skull Pile"),
    DATA_COMPGEN(0x00510d6c, gWellExtraNames1stLvlGrowth, "1st Lvl Growth"),
    NULL
};
DATA(0x004ff938) char* gSpecialBuildingNames[KB_SPECIAL_BUILDING_NAME_COUNT] =
    {DATA_COMPGEN(0x00510d7c, gSpecialBuildingNamesFortifications, "Fortifications"), DATA_COMPGEN(0x00510d8c, gSpecialBuildingNamesColiseum, "Coliseum"), DATA_COMPGEN(0x00510d98, gSpecialBuildingNamesRainbow, "Rainbow"), DATA_COMPGEN(0x00510da0, gSpecialBuildingNamesDungeon, "Dungeon"), DATA_COMPGEN(0x00510da8, gSpecialBuildingNamesLibrary, "Library"), DATA_COMPGEN(0x00510db0, gSpecialBuildingNamesStorm, "Storm"), DATA_COMPGEN(0x00510db8, gSpecialBuildingNamesSpecial, "Special"), NULL};
DATA(0x004ff958) char* gDwellingNames[IDX(FACTION_COUNT)][KB_DWELLING_TYPE_COUNT] = {
    {DATA_COMPGEN(0x00510dc0, gDwellingNamesThatchedHut, "Thatched Hut"),
     DATA_COMPGEN(0x00510dd0, gDwellingNamesArcheryRange, "Archery Range"),
     DATA_COMPGEN(0x00510de0, gDwellingNamesBlacksmith, "Blacksmith"),
     DATA_COMPGEN(0x00510dec, gDwellingNamesArmory, "Armory"),
     DATA_COMPGEN(0x00510df4, gDwellingNamesJoustingArena, "Jousting Arena"),
     DATA_COMPGEN(0x00510e04, gDwellingNamesCathedral, "Cathedral"),
     DATA_COMPGEN(0x00510e10, gDwellingNamesUpgArcheryRange, "Upg. Archery Range"),
     DATA_COMPGEN(0x00510e24, gDwellingNamesUpgBlacksmith, "Upg. Blacksmith"),
     DATA_COMPGEN(0x00510e34, gDwellingNamesUpgArmory, "Upg. Armory"),
     DATA_COMPGEN(0x00510e40, gDwellingNamesUpgJoustingArena, "Upg. Jousting Arena"),
     DATA_COMPGEN(0x00510e54, gDwellingNamesUpgCathedral, "Upg. Cathedral"),
     DATA_COMPGEN(0x00515c6c, gDwellingNamesEmptyString, "")},
    {DATA_COMPGEN(0x00510e68, gDwellingNamesHut, "Hut"),
     DATA_COMPGEN(0x00510e6c, gDwellingNamesStickHut, "Stick Hut"),
     DATA_COMPGEN(0x00510e78, gDwellingNamesDen, "Den"),
     DATA_COMPGEN(0x00510e7c, gDwellingNamesAdobe, "Adobe"),
     DATA_COMPGEN(0x00510e84, gDwellingNamesBridge, "Bridge"),
     DATA_COMPGEN(0x00510e8c, gDwellingNamesPyramid, "Pyramid"),
     DATA_COMPGEN(0x00510e94, gDwellingNamesUpgStickHut, "Upg. Stick Hut"),
     DATA_COMPGEN(0x00515c70, gDwellingNamesEmptyString2, ""),
     DATA_COMPGEN(0x00510ea8, gDwellingNamesUpgAdobe, "Upg. Adobe"),
     DATA_COMPGEN(0x00510eb4, gDwellingNamesUpgBridge, "Upg. Bridge"),
     DATA_COMPGEN(0x00515ccc, gDwellingNamesEmptyString3, ""),
     DATA_COMPGEN(0x00515cd0, gDwellingNamesEmptyString4, "")},
    {DATA_COMPGEN(0x00510ec8, gDwellingNamesTreehouse, "Treehouse"),
     DATA_COMPGEN(0x00510ed4, gDwellingNamesCottage, "Cottage"),
     DATA_COMPGEN(0x00510edc, gDwellingNamesArcheryRange2, "Archery Range"),
     DATA_COMPGEN(0x00510eec, gDwellingNamesStonehenge, "Stonehenge"),
     DATA_COMPGEN(0x00510ef8, gDwellingNamesFencedMeadow, "Fenced Meadow"),
     DATA_COMPGEN(0x00510f08, gDwellingNamesRedTower, "Red Tower"),
     DATA_COMPGEN(0x00510f14, gDwellingNamesUpgCottage, "Upg. Cottage"),
     DATA_COMPGEN(0x00510f24, gDwellingNamesUpgArcheryRange2, "Upg. Archery Range"),
     DATA_COMPGEN(0x00510f38, gDwellingNamesUpgStonehenge, "Upg. Stonehenge"),
     DATA_COMPGEN(0x00516d60, gDwellingNamesEmptyString5, ""),
     "",
     ""},
    {DATA_COMPGEN(0x00510f54, gDwellingNamesCave, "Cave"),
     DATA_COMPGEN(0x00510f5c, gDwellingNamesCrypt, "Crypt"),
     DATA_COMPGEN(0x00510f64, gDwellingNamesNest, "Nest"),
     DATA_COMPGEN(0x00510f6c, gDwellingNamesMaze, "Maze"),
     DATA_COMPGEN(0x00510f74, gDwellingNamesSwamp, "Swamp"),
     DATA_COMPGEN(0x00510f7c, gDwellingNamesGreenTower, "Green Tower"),
     "",
     "",
     DATA_COMPGEN(0x00510f90, gDwellingNamesUpgMaze, "Upg. Maze"),
     "",
     DATA_COMPGEN(0x00510fa0, gDwellingNamesRedTower2, "Red Tower"),
     DATA_COMPGEN(0x00510fac, gDwellingNamesBlackTower, "Black Tower")},
    {DATA_COMPGEN(0x00510fb8, gDwellingNamesHabitat, "Habitat"),
     DATA_COMPGEN(0x00510fc0, gDwellingNamesPen, "Pen"),
     DATA_COMPGEN(0x00510fc4, gDwellingNamesFoundry, "Foundry"),
     DATA_COMPGEN(0x00510fcc, gDwellingNamesCliffNest, "Cliff Nest"),
     DATA_COMPGEN(0x00510fd8, gDwellingNamesIvoryTower, "Ivory Tower"),
     DATA_COMPGEN(0x00510fe4, gDwellingNamesCloudCastle, "Cloud Castle"),
     "",
     DATA_COMPGEN(0x00510ff8, gDwellingNamesUpgFoundry, "Upg. Foundry"),
     "",
     DATA_COMPGEN(0x0051100c, gDwellingNamesUpgIvoryTower, "Upg. Ivory Tower"),
     DATA_COMPGEN(0x00511020, gDwellingNamesUpgCloudCastle, "Upg. Cloud Castle"),
     ""},
    {DATA_COMPGEN(0x00511038, gDwellingNamesExcavation, "Excavation"),
     DATA_COMPGEN(0x00511044, gDwellingNamesGraveyard, "Graveyard"),
     DATA_COMPGEN(0x00511050, gDwellingNamesPyramid2, "Pyramid"),
     DATA_COMPGEN(0x00511058, gDwellingNamesMansion, "Mansion"),
     DATA_COMPGEN(0x00511060, gDwellingNamesMausoleum, "Mausoleum"),
     DATA_COMPGEN(0x0051106c, gDwellingNamesLaboratory, "Laboratory"),
     DATA_COMPGEN(0x00511078, gDwellingNamesUpgGraveyard, "Upg. Graveyard"),
     DATA_COMPGEN(0x00511088, gDwellingNamesUpgPyramid, "Upg. Pyramid"),
     DATA_COMPGEN(0x00511098, gDwellingNamesUpgMansion, "Upg. Mansion"),
     DATA_COMPGEN(0x005110a8, gDwellingNamesUpgMausoleum, "Upg. Mausoleum"),
     "",
     ""}
};
DATA(0x004ffa78) char* cSecSkillDesc[IDX(HERO_SKILL_COUNT)][SECONDARY_SKILL_VALUE_LEVEL_COUNT] = {
    {DATA_COMPGEN(0x005110c0, cSecSkillDescBasicPathfindingBasicPathfindingReducesThe, "{Basic Pathfinding}\n\nBasic Pathfinding reduces the movement penalty for rough "
     "terrain by 25 percent."),
     DATA_COMPGEN(0x00511128, cSecSkillDescAdvancedPathfindingAdvancedPathfindingReducesThe, "{Advanced Pathfinding}\n\nAdvanced Pathfinding reduces the movement penalty for "
     "rough terrain by 50 percent."),
     DATA_COMPGEN(0x00511194, cSecSkillDescExpertPathfindingExpertPathfindingEliminatesThe, "{Expert Pathfinding}\n\nExpert Pathfinding eliminates the movement penalty for rough "
     "terrain.")},
    {DATA_COMPGEN(0x005111f0, cSecSkillDescBasicArcheryBasicArcheryIncreasesThe, "{Basic Archery}\n\nBasic Archery increases the damage done by range attacking "
     "creatures by 10 percent."),
     DATA_COMPGEN(0x00511258, cSecSkillDescAdvancedArcheryAdvancedArcheryIncreasesThe, "{Advanced Archery}\n\nAdvanced Archery increases the damage done by range attacking "
     "creatures by 25 percent."),
     DATA_COMPGEN(0x005112c4, cSecSkillDescExpertArcheryExpertArcheryIncreasesThe, "{Expert Archery}\n\nExpert Archery increases the damage done by range attacking "
     "creatures by 50 percent.")},
    {DATA_COMPGEN(0x0051132c, cSecSkillDescBasicLogisticsBasicLogisticsIncreasesYour, "{Basic Logistics}\n\nBasic Logistics increases your hero's movement points by 10 "
     "percent."),
     DATA_COMPGEN(0x00511384, cSecSkillDescAdvancedLogisticsAdvancedLogisticsIncreasesYour, "{Advanced Logistics}\n\nAdvanced Logistics increases your hero's movement points by "
     "20 percent."),
     DATA_COMPGEN(0x005113e4, cSecSkillDescExpertLogisticsExpertLogisticsIncreasesYour, "{Expert Logistics}\n\nExpert Logistics increases your hero's movement points by 30 "
     "percent.")},
    {DATA_COMPGEN(0x00511440, cSecSkillDescBasicScoutingBasicScoutingIncreasesYour, "{Basic Scouting}\n\nBasic Scouting increases your hero's viewable area by 1 square."),
     DATA_COMPGEN(0x00511494, cSecSkillDescAdvancedScoutingAdvancedScoutingIncreasesYour, "{Advanced Scouting}\n\nAdvanced Scouting increases your hero's viewable area by 2 "
     "squares."),
     DATA_COMPGEN(0x005114f0, cSecSkillDescExpertScoutingExpertScoutingIncreasesYour, "{Expert Scouting}\n\nExpert Scouting increases your hero's viewable area by 3 "
     "squares.")},
    {DATA_COMPGEN(0x00511548, cSecSkillDescBasicDiplomacyBasicDiplomacyAllowsYou, "{Basic Diplomacy}\n\nBasic Diplomacy allows you to negotiate with monsters who are "
     "weaker than your group.  Approximately 1/4 of the creatures may offer to join you."),
     DATA_COMPGEN(0x005115ec, cSecSkillDescAdvancedDiplomacyAdvancedDiplomacyAllowsYou, "{Advanced Diplomacy}\n\nAdvanced Diplomacy allows you to negotiate with monsters who "
     "are weaker than your group.  Approximately 1/2 of the creatures may offer to join "
     "you."),
     DATA_COMPGEN(0x00511698, cSecSkillDescExpertDiplomacyExpertDiplomacyAllowsYou, "{Expert Diplomacy}\n\nExpert Diplomacy allows you to negotiate with monsters who are "
     "weaker than your group.  All of the creatures may offer to join you.")},
    {DATA_COMPGEN(0x00511730, cSecSkillDescBasicNavigationBasicNavigationIncreasesYour, "{Basic Navigation}\n\nBasic Navigation increases your hero's movement points over "
     "water by 1/3."),
     DATA_COMPGEN(0x00511790, cSecSkillDescAdvancedNavigationAdvancedNavigationIncreasesYour, "{Advanced Navigation}\n\nAdvanced Navigation increases your hero's movement points "
     "over water by 2/3."),
     DATA_COMPGEN(0x005117f4, cSecSkillDescExpertNavigationExpertNavigationDoublesYour, "{Expert Navigation}\n\nExpert Navigation doubles your hero's movement points over "
     "water.")},
    {DATA_COMPGEN(0x0051184c, cSecSkillDescBasicLeadershipBasicLeadershipIncreasesYour, "{Basic Leadership}\n\nBasic Leadership increases your hero's troops' morale by 1."),
     DATA_COMPGEN(0x0051189c, cSecSkillDescAdvancedLeadershipAdvancedLeadershipIncreasesYour, "{Advanced Leadership}\n\nAdvanced Leadership increases your hero's troops' morale by "
     "2."),
     DATA_COMPGEN(0x005118f4, cSecSkillDescExpertLeadershipExpertLeadershipIncreasesYour, "{Expert Leadership}\n\nExpert Leadership increases your hero's troops' morale by 3.")},
    {DATA_COMPGEN(0x00511948, cSecSkillDescBasicWisdomBasicWisdomAllowsYour, "{Basic Wisdom}\n\nBasic Wisdom allows your hero to learn third level spells."),
     DATA_COMPGEN(0x00511994, cSecSkillDescAdvancedWisdomAdvancedWisdomAllowsYour, "{Advanced Wisdom}\n\nAdvanced Wisdom allows your hero to learn fourth level spells."),
     DATA_COMPGEN(0x005119e8, cSecSkillDescExpertWisdomExpertWisdomAllowsYour, "{Expert Wisdom}\n\nExpert Wisdom allows your hero to learn fifth level spells.")},
    {DATA_COMPGEN(0x00511a38, cSecSkillDescBasicMysticismBasicMysticismRegeneratesTwo, "{Basic Mysticism}\n\nBasic Mysticism regenerates two of your hero's spell points per "
     "day."),
     DATA_COMPGEN(0x00511a90, cSecSkillDescAdvancedMysticismAdvancedMysticismRegeneratesThree, "{Advanced Mysticism}\n\nAdvanced Mysticism regenerates three of your hero's spell "
     "points per day."),
     DATA_COMPGEN(0x00511af0, cSecSkillDescExpertMysticismExpertMysticismRegeneratesFour, "{Expert Mysticism}\n\nExpert Mysticism regenerates four of your hero's spell points "
     "per day.")},
    {DATA_COMPGEN(0x00511b4c, cSecSkillDescBasicLuckBasicLuckIncreasesYour, "{Basic Luck}\n\nBasic Luck increases your hero's luck by 1."),
     DATA_COMPGEN(0x00511b88, cSecSkillDescAdvancedLuckAdvancedLuckIncreasesYour, "{Advanced Luck}\n\nAdvanced Luck increases your hero's luck by 2."),
     DATA_COMPGEN(0x00511bc8, cSecSkillDescExpertLuckExpertLuckIncreasesYour, "{Expert Luck}\n\nExpert Luck increases your hero's luck by 3.")},
    {DATA_COMPGEN(0x00511c04, cSecSkillDescBasicBallisticsBasicBallisticsGivesYour, "{Basic Ballistics}\n\nBasic Ballistics gives your hero's catapult shots a greater "
     "chance to hit and do damage to castle walls."),
     DATA_COMPGEN(0x00511c84, cSecSkillDescAdvancedBallisticsAdvancedBallisticsGivesYour, "{Advanced Ballistics}\n\nAdvanced Ballistics gives your hero's catapult an extra "
     "shot, and each shot has a greater chance to hit and do damage to castle walls."),
     DATA_COMPGEN(0x00511d24, cSecSkillDescExpertBallisticsExpertBallisticsGivesYour, "{Expert Ballistics}\n\nExpert Ballistics gives your hero's catapult an extra shot, "
     "and each shot automatically destroys any wall, except a fortified wall in a Knight "
     "town.")},
    {DATA_COMPGEN(0x00511dd0, cSecSkillDescBasicEagleEyeBasicEagleEye, "{Basic Eagle Eye}\n\nBasic Eagle Eye gives your hero a 20 percent chance to learn "
     "any given 1st or 2nd level enemy spell used against him in a combat."),
     DATA_COMPGEN(0x00511e68, cSecSkillDescAdvancedEagleEyeAdvancedEagleEye, "{Advanced Eagle Eye}\n\nAdvanced Eagle Eye gives your hero a 30 percent chance to "
     "learn any given 3rd level spell (or below) used against him in combat."),
     DATA_COMPGEN(0x00511f00, cSecSkillDescExpertEagleEyeExpertEagleEye, "{Expert Eagle Eye}\n\nExpert Eagle Eye gives your hero a 40 percent chance to learn "
     "any given 4th level spell (or below) used against him in combat.")},
    {DATA_COMPGEN(0x00511f94, cSecSkillDescBasicNecromancyBasicNecromancyAllows10, "{Basic Necromancy}\n\nBasic Necromancy allows 10 percent of the creatures killed in "
     "combat to be brought back from the dead as Skeletons."),
     DATA_COMPGEN(0x0051201c, cSecSkillDescAdvancedNecromancyAdvancedNecromancyAllows20, "{Advanced Necromancy}\n\nAdvanced Necromancy allows 20 percent of the creatures "
     "killed in combat to be brought back from the dead as Skeletons."),
     DATA_COMPGEN(0x005120ac, cSecSkillDescExpertNecromancyExpertNecromancyAllows30, "{Expert Necromancy}\n\nExpert Necromancy allows 30 percent of the creatures killed "
     "in combat to be brought back from the dead as Skeletons.")},
    {DATA_COMPGEN(0x00512138, cSecSkillDescSmallEstatesYourHeroProduces100, "{Small Estates}\n\nYour hero produces 100 gold pieces per turn as tax revenue from "
     "estates."),
     DATA_COMPGEN(0x00512194, cSecSkillDescModerateEstatesYourHeroProduces250, "{Moderate Estates}\n\nYour hero produces 250 gold pieces per turn as tax revenue "
     "from estates."),
     DATA_COMPGEN(0x005121f4, cSecSkillDescGrandEstatesYourHeroProduces500, "{Grand Estates}\n\nYour hero produces 500 gold pieces per turn as tax revenue from "
     "estates.")}
};
DATA(0x004ffb20) char* cBuildingInfoNeutral[KB_NEUTRAL_BUILDING_INFO_COUNT] = {
    DATA_COMPGEN(0x00512250, cBuildingInfoNeutralTheMageGuildAllowsHeroesTo, "The Mage Guild allows heroes to learn spells and replenish their spell points."),
    DATA_COMPGEN(0x005122a0, cBuildingInfoNeutralTheThievesGuildProvidesInformationOn, "The Thieves' Guild provides information on enemy players.  Thieves' Guilds can also provide "
    "scouting information on enemy towns.  Additional Guilds provide more information."),
    DATA_COMPGEN(0x00512350, cBuildingInfoNeutralTheTavernIncreasesMoraleForTroops, "The Tavern increases morale for troops defending the castle."),
    DATA_COMPGEN(0x00512390, cBuildingInfoNeutralTheShipyardAllowsShipsToBe, "The Shipyard allows ships to be built."),
    DATA_COMPGEN(0x005123b8, cBuildingInfoNeutralTheWellIncreasesTheGrowthRate, "The Well increases the growth rate of all dwellings by 2 creatures per week."),
    DATA_COMPGEN(0x00512408, cBuildingInfoNeutralTheTentProvidesWorkersToBuild, "The Tent provides workers to build a castle, provided the materials and the gold are "
    "available."),
    DATA_COMPGEN(0x00512468, cBuildingInfoNeutralTheCastleImprovesTownDefenseAnd, "The Castle improves town defense and increases income to 1000 gold per day."),
    DATA_COMPGEN(0x005124b4, cBuildingInfoNeutralTheStatueIncreasesYourTownS, "The Statue increases your town's income by 250 per day."),
    DATA_COMPGEN(0x005124ec, cBuildingInfoNeutralTheLeftTurretProvidesExtraFirepower, "The Left Turret provides extra firepower during castle combat."),
    DATA_COMPGEN(0x0051252c, cBuildingInfoNeutralTheRightTurretProvidesExtraFirepower, "The Right Turret provides extra firepower during castle combat."),
    DATA_COMPGEN(0x0051256c, cBuildingInfoNeutralTheMarketplaceCanBeUsedTo, "The Marketplace can be used to convert one type of resource into another.  The more "
    "marketplaces you control, the better the exchange rate."),
    "",
    DATA_COMPGEN(0x005125fc, cBuildingInfoNeutralTheMoatSlowsAttackingUnitsAny, "The Moat slows attacking units.  Any unit entering the moat must end its turn there and "
    "becomes more vulnerable to attack."),
    "",
    DATA_COMPGEN(0x0051267c, cBuildingInfoNeutralTheShipyardAllowsShipsToBe2, "The Shipyard allows ships to be built."),
    DATA_COMPGEN(0x005126a4, cBuildingInfoNeutralTheCaptainSQuartersProvidesA, "The Captain's Quarters provides a captain to assist in the castle's defense when no hero is "
    "present."),
    "",
    "",
    "",
    NULL
};
DATA(0x004ffb70) char* gBuildingInfoSpecial[KB_SPECIAL_BUILDING_INFO_COUNT] = {
    DATA_COMPGEN(0x00512718, gBuildingInfoSpecialTheFortificationsIncreaseTheToughnessOf, "The Fortifications increase the toughness of the walls, increasing the number of turns it "
    "takes to knock them down."),
    DATA_COMPGEN(0x0051278c, gBuildingInfoSpecialTheColiseumProvidesInspiringSpectaclesTo, "The Coliseum provides inspiring spectacles to defending troops, raising their morale by two "
    "during combat."),
    DATA_COMPGEN(0x005127f8, gBuildingInfoSpecialTheRainbowIncreasesTheLuckOf, "The Rainbow increases the luck of the defending units by two."),
    DATA_COMPGEN(0x00512838, gBuildingInfoSpecialTheDungeonIncreasesTheIncomeOf, "The Dungeon increases the income of the town by 500 / day."),
    DATA_COMPGEN(0x00512874, gBuildingInfoSpecialTheLibraryIncreasesTheNumberOf, "The Library increases the number of spells in the Guild by one for each level of the guild."),
    DATA_COMPGEN(0x005128d0, gBuildingInfoSpecialTheStormAdds2ToThe, "The Storm adds +2 to the power of spells of a defending spell caster.")
};
DATA(0x004ffb88) char* cDirections[KB_DIRECTION_TEXT_COUNT] = {
    DATA_COMPGEN(0x00512918, cDirectionsNorthern, "northern"),
    DATA_COMPGEN(0x00512924, cDirectionsNortheastern, "northeastern"),
    DATA_COMPGEN(0x00512934, cDirectionsEastern, "eastern"),
    DATA_COMPGEN(0x0051293c, cDirectionsSoutheastern, "southeastern"),
    DATA_COMPGEN(0x0051294c, cDirectionsSouthern, "southern"),
    DATA_COMPGEN(0x00512958, cDirectionsSouthwestern, "southwestern"),
    DATA_COMPGEN(0x00512968, cDirectionsWestern, "western"),
    DATA_COMPGEN(0x00512970, cDirectionsNorthwestern, "northwestern"),
    DATA_COMPGEN(0x00512980, cDirectionsCentral, "central"),
    NULL
};
DATA(0x004ffbb0) char* cRumourTerrainDescriptions[KB_RUMOUR_TERRAIN_DESCRIPTION_COUNT] = {
    DATA_COMPGEN(0x00512988, cRumourTerrainDescriptionsInTheMurkyDepthsOfThe, "in the murky depths of the ocean."),
    DATA_COMPGEN(0x005129ac, cRumourTerrainDescriptionsOnAGrassyPlain, "on a grassy plain."),
    DATA_COMPGEN(0x005129c0, cRumourTerrainDescriptionsInTheDrivenSnow, "in the driven snow."),
    DATA_COMPGEN(0x005129d4, cRumourTerrainDescriptionsInAMurkySwamp, "in a murky swamp."),
    DATA_COMPGEN(0x005129e8, cRumourTerrainDescriptionsBuriedInBarrenLava, "buried in barren lava."),
    DATA_COMPGEN(0x00512a00, cRumourTerrainDescriptionsUnderADesertDrift, "under a desert drift."),
    DATA_COMPGEN(0x00512a18, cRumourTerrainDescriptionsInTheDirt, "in the dirt."),
    DATA_COMPGEN(0x00512a28, cRumourTerrainDescriptionsInAParchedWasteland, "in a parched wasteland."),
    DATA_COMPGEN(0x00512a40, cRumourTerrainDescriptionsBuriedOnABeach, "buried on a beach."),
    NULL
};
DATA(0x004ffbd8) char* gInterfaceTypeText[KB_INTERFACE_TYPE_TEXT_COUNT] =
    {DATA_COMPGEN(0x00512a54, gInterfaceTypeTextDynamic, "Dynamic"), DATA_COMPGEN(0x00512a5c, gInterfaceTypeTextGood, "Good"), DATA_COMPGEN(0x00512a64, gInterfaceTypeTextEvil, "Evil"), NULL};
DATA(0x004ffbe8) char* cBWMouseText[KB_BW_MOUSE_TEXT_COUNT] = {DATA_COMPGEN(0x00512a6c, cBWMouseTextBlackWhite, "Black & White"), DATA_COMPGEN(0x00512a7c, cBWMouseTextColor, "Color")};
DATA(0x004ffbf0) char* combatSpeedText[KB_COMBAT_SPEED_TEXT_COUNT] =
    {DATA_COMPGEN(0x00512a84, combatSpeedTextNormal, "Normal"), DATA_COMPGEN(0x00512a8c, combatSpeedTextFast, "Fast"), DATA_COMPGEN(0x00512a94, combatSpeedTextVeryFast, "Very Fast"), NULL};
DATA(0x004ffc00) char* combatMiniInfoText[KB_COMBAT_MINI_INFO_TEXT_COUNT] =
    {DATA_COMPGEN(0x00512aa0, combatMiniInfoTextNone, "None"), DATA_COMPGEN(0x00512aa8, combatMiniInfoTextSpellsOnly, "Spells Only"), DATA_COMPGEN(0x00512ab4, combatMiniInfoTextFull, "Full"), NULL};
DATA(0x004ffc10) char* gcCommandLineHelp[KB_COMMAND_LINE_HELP_COUNT] = {
    DATA_COMPGEN(0x00512abc, gcCommandLineHelpCommandLineHelp, "\n\n\n***Command Line Help***\n"),
    DATA_COMPGEN(0x00512ad8, gcCommandLineHelpEmptyString, "\n"),
    DATA_COMPGEN(0x00512adc, gcCommandLineHelpD0DisableDigitalSound, "/D0 - disable digital sound\n"),
    DATA_COMPGEN(0x00512afc, gcCommandLineHelpM0DisableMIDIMusic, "/M0 - disable MIDI music\n"),
    DATA_COMPGEN(0x00512b18, gcCommandLineHelpR0DisableRedbookMusic, "/R0 - disable Redbook music\n"),
    DATA_COMPGEN(0x00512b38, gcCommandLineHelpI0SkipTheIntro, "/I0 - skip the intro\n"),
    DATA_COMPGEN(0x00512b50, gcCommandLineHelpEmptyString2, "\n"),
    DATA_COMPGEN(0x00512b54, gcCommandLineHelpEmptyString3, "\n"),
    DATA_COMPGEN(0x00512b58, gcCommandLineHelpExample, "Example:\n"),
    DATA_COMPGEN(0x00512b64, gcCommandLineHelpEmptyString4, "\n"),
    DATA_COMPGEN(0x00512b68, gcCommandLineHelpHEROES2DR0I0, "HEROES2D /R0 /I0\n"),
    DATA_COMPGEN(0x00512b7c, gcCommandLineHelpEmptyString5, "\n"),
    DATA_COMPGEN(0x00512b80, gcCommandLineHelpStartsTheDOSVersionOfHeroes2, "Starts the DOS version of Heroes2 with redbook\n"),
    DATA_COMPGEN(0x00512bb0, gcCommandLineHelpSoundDisabledAndTheIntroSkipped, "sound disabled and the intro skipped.\n")
};
DATA(0x004ffc48) char* cOverviewText[KB_OVERVIEW_TEXT_COUNT] =
    {DATA_COMPGEN(0x00512bd8, cOverviewTextHeroStats, "Hero/Stats"), DATA_COMPGEN(0x00512be4, cOverviewTextSkills, "Skills"), DATA_COMPGEN(0x00512bec, cOverviewTextArtifacts, "Artifacts"), DATA_COMPGEN(0x00512bf8, cOverviewTextTownCastle, "Town/Castle"), DATA_COMPGEN(0x00512c04, cOverviewTextGarrison, "Garrison"), DATA_COMPGEN(0x00512c10, cOverviewTextAvailable, "Available")};
DATA(0x004ffc60) char* cWinComError[KB_WIN_COM_ERROR_TEXT_COUNT] = {
    DATA_COMPGEN(0x00512c1c, cWinComErrorCommunicationsErrorOnFunctionSWin95, "Communications error on function '%s'\n\nWin95 Error Code: %d\nWin95 Error Meaning: %s\n\n"),
    DATA_COMPGEN(0x00512c74, cWinComErrorSuggestedSolutions, "Suggested solutions:"),
    DATA_COMPGEN(0x00512c8c, cWinComError1MakeSureAllCablesAre, "\n  1) Make sure all cables are firmly connected."),
    DATA_COMPGEN(0x00512cc0, cWinComError2RebootComputer, "\n  2) Reboot computer."),
    DATA_COMPGEN(0x00512cd8, cWinComError3CheckToMakeSureYou, "\n  3) Check to make sure you have the correct COM port setting in 'CONFIG'. (The 3rd button "
    "down on the screen where you choose Host or Guest.)"),
    DATA_COMPGEN(0x00512d68, cWinComError4ConsiderLoweringTheBAUDRate, "\n  4) Consider lowering the BAUD rate in 'CONFIG' to 19200 or 9600.")
};
DATA(0x004ffc78) char* cMiniViewText[KB_MINI_VIEW_TEXT_COUNT] =
    {DATA_COMPGEN(0x00512dac, cMiniViewTextDUnits, "%d Units"), DATA_COMPGEN(0x00512db8, cMiniViewTextDUnit, "%d Unit"), DATA_COMPGEN(0x00512dc0, cMiniViewTextAttack, "Attack"), DATA_COMPGEN(0x00512dc8, cMiniViewTextDefense, "Defense"), DATA_COMPGEN(0x00512dd0, cMiniViewTextHP, "HP "), DATA_COMPGEN(0x00512dd4, cMiniViewTextDmg, "Dmg"), DATA_COMPGEN(0x00512dd8, cMiniViewTextMrl, "Mrl"), DATA_COMPGEN(0x00512ddc, cMiniViewTextLuk, "Luk"), DATA_COMPGEN(0x00512de0, cMiniViewTextShots, "Shots"), NULL};
DATA(0x004ffca0) char* gFileRequestHelp[KB_FILE_REQUEST_HELP_COUNT] = {
    DATA_COMPGEN(0x00512de8, gFileRequestHelpSmallMapsViewOnlyMapsOf, "{Small Maps}\n\nView only maps of size small (36 x 36)."),
    DATA_COMPGEN(0x00512e20, gFileRequestHelpMediumMapsViewOnlyMapsOf, "{Medium Maps}\n\nView only maps of size medium (72 x 72)."),
    DATA_COMPGEN(0x00512e58, gFileRequestHelpLargeMapsViewOnlyMapsOf, "{Large Maps}\n\nView only maps of size large (108 x 108)."),
    DATA_COMPGEN(0x00512e90, gFileRequestHelpExtraLargeMapsViewOnlyMaps, "{Extra Large Maps}\n\nView only maps of size extra large (144 x 144)."),
    DATA_COMPGEN(0x00512ed4, gFileRequestHelpAllMapsViewAllMapsRegardless, "{All Maps}\n\nView all maps, regardless of size."),
    DATA_COMPGEN(0x00512f04, gFileRequestHelpEnterNameEnterTheNameOf, "{Enter Name}\n\nEnter the name of the file you wish to save."),
    DATA_COMPGEN(0x00512f40, gFileRequestHelpOKAcceptTheChoiceMade, "{OK}\n\nAccept the choice made."),
    DATA_COMPGEN(0x00512f60, gFileRequestHelpCancelCancelWithoutMakingAChoice, "{Cancel}\n\nCancel without making a choice."),
    DATA_COMPGEN(0x00512f8c, gFileRequestHelpSizeIconIndicatesWhetherTheMap, "{Size Icon}\n\nIndicates whether the map is small (36 x 36), medium (72 x 72), large (108 x "
    "108), or extra large (144 x 144)."),
    DATA_COMPGEN(0x00513008, gFileRequestHelpPlayersIconIndicatesHowManyPlayers, "{Players Icon}\n\nIndicates how many players total are in the scenario.  Any positions not "
    "occupied by humans will be occupied by computer players."),
    DATA_COMPGEN(0x0051309c, gFileRequestHelpVictoryConditionIconThereAre6, "{Victory Condition Icon}\n\nThere are 6 possibilities:\n\n{Tombstone} - Defeat all enemy "
    "heroes and towns.\n\n{Town} - Capture a specific town.\n\n{Hero Face} - Defeat a specific "
    "hero.\n\n{Medal} - Find a specific artifact.\n\n{Handshake} - Your side defeats the opposing "
    "side.\n\n{Coins} - Accumulate a large amount of gold."),
    DATA_COMPGEN(0x005131d4, gFileRequestHelpLossConditionIconThereAre4, "{Loss Condition Icon}\n\nThere are 4 possible loss conditions, as indicated by the following "
    "icons:\n\n{Tombstone} - Lose all your heroes and towns.\n\n{Town} - Lose a specific "
    "town.\n\n{Hero Face} - Lose a specific hero.\n\n{Hourglass} - Run out of time. (Fail to win "
    "by a certain point.)"),
    DATA_COMPGEN(0x005132ec, gFileRequestHelpSelectedNameTheNameOfThe, "{Selected Name}\n\nThe name of the currently selected map."),
    DATA_COMPGEN(0x00513328, gFileRequestHelpSelectedDescriptionTheDescriptionOfThe, "{Selected Description}\n\nThe description of the currently selected map."),
    DATA_COMPGEN(0x00513370, gFileRequestHelpSelectedMapDifficultyTheMapDifficulty, "{Selected Map Difficulty}\n\nThe map difficulty of the currently selected map.  The map "
    "difficulty is determined by the scenario designer.  More difficult maps might include more or "
    "stronger enemies, fewer resources, or other special conditions making things tougher for the "
    "human player."),
    NULL
};
DATA(0x004ffce0) char* cPersonality[KB_PERSONALITY_TEXT_COUNT] =
    {DATA_COMPGEN(0x00513490, cPersonalityWarrior, "Warrior"), DATA_COMPGEN(0x00513498, cPersonalityBuilder, "Builder"), DATA_COMPGEN(0x005134a0, cPersonalityExplorer, "Explorer"), DATA_COMPGEN(0x005134ac, cPersonalityHuman, "Human")};
DATA(0x004ffcf0) char* gArmySizeNames[KB_ARMY_SIZE_NAME_COUNT][KB_ARMY_SIZE_NAME_VARIANT_COUNT] = {
    {DATA_COMPGEN(0x005134b4, gArmySizeNamesFew, "Few"), DATA_COMPGEN(0x005134b8, gArmySizeNamesAFew, "A few"), DATA_COMPGEN(0x005134c0, gArmySizeNamesAFew2, "a few")},
    {DATA_COMPGEN(0x005134c8, gArmySizeNamesSeveral, "Several"), DATA_COMPGEN(0x005134d0, gArmySizeNamesSeveral2, "Several"), DATA_COMPGEN(0x005134d8, gArmySizeNamesSeveral3, "several")},
    {DATA_COMPGEN(0x005134e0, gArmySizeNamesPack, "Pack"), DATA_COMPGEN(0x005134e8, gArmySizeNamesAPackOf, "A pack of"), DATA_COMPGEN(0x005134f4, gArmySizeNamesAPackOf2, "a pack of")},
    {DATA_COMPGEN(0x00513500, gArmySizeNamesLots, "Lots"), DATA_COMPGEN(0x00513508, gArmySizeNamesLotsOf, "Lots of"), DATA_COMPGEN(0x00513510, gArmySizeNamesLotsOf2, "lots of")},
    {DATA_COMPGEN(0x00513518, gArmySizeNamesHorde, "Horde"), DATA_COMPGEN(0x00513520, gArmySizeNamesAHordeOf, "A Horde of"), DATA_COMPGEN(0x0051352c, gArmySizeNamesAHordeOf2, "a horde of")},
    {DATA_COMPGEN(0x00513538, gArmySizeNamesThrong, "Throng"), DATA_COMPGEN(0x00513540, gArmySizeNamesAThrongOf, "A Throng of"), DATA_COMPGEN(0x0051354c, gArmySizeNamesAThrongOf2, "a throng of")},
    {DATA_COMPGEN(0x00513558, gArmySizeNamesSwarm, "Swarm"), DATA_COMPGEN(0x00513560, gArmySizeNamesASwarmOf, "A Swarm of"), DATA_COMPGEN(0x0051356c, gArmySizeNamesASwarmOf2, "a swarm of")},
    {DATA_COMPGEN(0x00513578, gArmySizeNamesZounds, "Zounds"), DATA_COMPGEN(0x00513580, gArmySizeNamesZounds2, "Zounds..."), DATA_COMPGEN(0x0051358c, gArmySizeNamesZoundsOf, "zounds of")},
    {DATA_COMPGEN(0x00513598, gArmySizeNamesLegion, "Legion"), DATA_COMPGEN(0x005135a0, gArmySizeNamesALegionOf, "A Legion of"), DATA_COMPGEN(0x005135ac, gArmySizeNamesALegionOf2, "a legion of")}
};
DATA(0x004ffd60) char* cRandomTavernText[KB_RANDOM_TAVERN_TEXT_COUNT] = {
    DATA_COMPGEN(0x005135b8, cRandomTavernTextTheTruthIsOutThere, "The truth is out there."),
    DATA_COMPGEN(0x005135d0, cRandomTavernTextTheDarkSideIsStronger, "The dark side is stronger."),
    DATA_COMPGEN(0x005135ec, cRandomTavernTextTheEndOfTheWorldIs, "The end of the world is near."),
    DATA_COMPGEN(0x0051360c, cRandomTavernTextTheBonesOfLordSlayerAre, "The bones of Lord Slayer are buried in the foundation of the arena."),
    DATA_COMPGEN(0x00513650, cRandomTavernTextHeSInnocent, "He's innocent."),
    DATA_COMPGEN(0x00513660, cRandomTavernTextABlackDragonWillTakeOut, "A Black Dragon will take out a Titan any day of the week."),
    DATA_COMPGEN(0x0051369c, cRandomTavernTextHeToldHerYadaYadaYada, "He told her \"Yada yada yada\"...  and then she said \"Blah, blah, blah...\""),
    DATA_COMPGEN(0x005136e8, cRandomTavernTextThereOnceWasAManFrom, "There once was a man from Nantucket...")
};
DATA(0x004ffd80) char* cRandomSignText[KB_RANDOM_SIGN_TEXT_COUNT] =
    {DATA_COMPGEN(0x00513710, cRandomSignTextSeeRockCity, "See Rock City"), DATA_COMPGEN(0x00513720, cRandomSignTextThisSpaceForRent, "This space for rent"), DATA_COMPGEN(0x00513734, cRandomSignTextNextSign50Miles, "Next sign 50 miles"), DATA_COMPGEN(0x00513748, cRandomSignTextBurmaShave, "Burma shave")};
DATA(0x004ffd90) char* cCampaignAwards[KB_CAMPAIGN_AWARD_TEXT_COUNT] = {
    DATA_COMPGEN(0x00513754, cCampaignAwardsDwarvenAlliance, "Dwarven alliance"),
    DATA_COMPGEN(0x00513768, cCampaignAwardsSorceressGuild, "Sorceress guild"),
    DATA_COMPGEN(0x00513778, cCampaignAwardsRolandStrengthened, "Roland strengthened"),
    DATA_COMPGEN(0x0051378c, cCampaignAwardsCarryOverForces, "Carry over forces"),
    DATA_COMPGEN(0x005137a0, cCampaignAwardsCorlagonDefeated, "Corlagon defeated"),
    DATA_COMPGEN(0x005137b4, cCampaignAwardsUltimateCrown, "Ultimate crown"),
    DATA_COMPGEN(0x005137c4, cCampaignAwardsNecromancerGuild, "Necromancer guild"),
    DATA_COMPGEN(0x005137d8, cCampaignAwardsDwarfBane, "DwarfBane"),
    DATA_COMPGEN(0x005137e4, cCampaignAwardsOgreAlliance, "Ogre alliance"),
    DATA_COMPGEN(0x005137f4, cCampaignAwardsDragonAlliance, "Dragon alliance"),
    DATA_COMPGEN(0x00513804, cCampaignAwardsUltimateCrown2, "Ultimate crown"),
    DATA_COMPGEN(0x00513814, cCampaignAwardsCarryOverForces2, "Carry over forces")
};
DATA(0x004ffdc0) char* cCampaignName[IDX(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT] = {
    {DATA_COMPGEN(0x00513828, cCampaignNameForceOfArms, "Force of Arms"),
     DATA_COMPGEN(0x00513838, cCampaignNameAnnexation, "Annexation"),
     DATA_COMPGEN(0x00513844, cCampaignNameSaveTheDwarves, "Save the Dwarves"),
     DATA_COMPGEN(0x00513858, cCampaignNameCaratorMines, "Carator Mines"),
     DATA_COMPGEN(0x00513868, cCampaignNameTurningPoint, "Turning Point"),
     DATA_COMPGEN(0x00513878, cCampaignNameDefender, "Defender"),
     DATA_COMPGEN(0x00513884, cCampaignNameTheGauntlet, "The Gauntlet"),
     DATA_COMPGEN(0x00513894, cCampaignNameTheCrown, "The Crown"),
     DATA_COMPGEN(0x005138a0, cCampaignNameCorlagonSDefense, "Corlagon's Defense"),
     DATA_COMPGEN(0x005138b4, cCampaignNameFinalJustice, "Final Justice"),
     "",
     DATA_COMPGEN(0x005138c8, cCampaignNameBetrayal, "Betrayal")},
    {DATA_COMPGEN(0x005138d4, cCampaignNameFirstBlood, "First Blood"),
     DATA_COMPGEN(0x005138e0, cCampaignNameBarbarianWars, "Barbarian Wars"),
     DATA_COMPGEN(0x005138f0, cCampaignNameNecromancers, "Necromancers"),
     DATA_COMPGEN(0x00513900, cCampaignNameSlayTheDwarves, "Slay the Dwarves"),
     DATA_COMPGEN(0x00513914, cCampaignNameTurningPoint2, "Turning Point"),
     DATA_COMPGEN(0x00513924, cCampaignNameRebellion, "Rebellion"),
     DATA_COMPGEN(0x00513930, cCampaignNameDragonMaster, "Dragon Master"),
     DATA_COMPGEN(0x00513940, cCampaignNameCountryLords, "Country Lords"),
     DATA_COMPGEN(0x00513950, cCampaignNameTheCrown2, "The Crown"),
     DATA_COMPGEN(0x0051395c, cCampaignNameGreaterGlory, "Greater Glory"),
     DATA_COMPGEN(0x0051396c, cCampaignNameApocalypse, "Apocalypse"),
     DATA_COMPGEN(0x00513978, cCampaignNameBetrayal2, "Betrayal")}
};
DATA(0x004ffe20) char* cCampaignDescription[IDX(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT] = {
    {DATA_COMPGEN(0x00513984, cCampaignDescriptionRolandNeedsYouToDefeatThe, "Roland needs you to defeat the lords near his castle to begin his war of "
     "rebellion against his brother.  They are not allied with each other, so "
     "they will spend most of their time fighting with one another.  Victory is "
     "yours when you have defeated all of their castles and heroes."),
     DATA_COMPGEN(0x00513aa0, cCampaignDescriptionTheLocalLordsRefuseToSwear, "The local lords refuse to swear allegiance to Roland, and must be subdued.  "
     "They are wealthy and powerful, so be prepared for a tough fight.  Capture "
     "all enemy castles to win."),
     DATA_COMPGEN(0x00513b50, cCampaignDescriptionYourTaskIsToDefendThe, "Your task is to defend the Dwarves against Archibald's forces.  Capture all "
     "of the enemy towns and castles to win, and be sure not to lose all of the "
     "dwarf towns at once, or the enemy will have won."),
     DATA_COMPGEN(0x00513c18, cCampaignDescriptionYouWillFaceFourAlliedEnemies, "You will face four allied enemies in a straightforward fight for resources "
     "and treasure.  Capture all of the enemy castles for victory."),
     DATA_COMPGEN(0x00513ca0, cCampaignDescriptionYourEnemiesAreAlliedAgainstYou, "Your enemies are allied against you and start close by, so be ready to come "
     "out fighting.  You will need to own all four castles in this small valley "
     "to win."),
     DATA_COMPGEN(0x00513d40, cCampaignDescriptionTheSorceressGuildOfNorastonHas, "The Sorceress' guild of Noraston has requested Roland's aid against an "
     "attack from Archibald's allies.  Capture all of the enemy castles to win, "
     "and don't lose Noraston, or you'll lose the scenario.  (Hint:  There is an "
     "enemy castle on an island in the ocean.)"),
     DATA_COMPGEN(0x00513e48, cCampaignDescriptionGatherAsLargeAnArmyAs, "Gather as large an army as possible and capture the enemy castle within 8 "
     "weeks.  You are opposed by only one enemy, but must travel a long way to get "
     "to the enemy castle.  Any troops you have in your army at the end of this "
     "scenario will be with you in the final battle."),
     DATA_COMPGEN(0x00513f58, cCampaignDescriptionFindTheCrownBeforeArchibaldS, "Find the Crown before Archibald's heroes find it.  Roland will need the "
     "Crown for the final battle."),
     DATA_COMPGEN(0x00513fbc, cCampaignDescriptionThreeAlliedEnemiesStandBeforeYou, "Three allied enemies stand before you and victory, including Lord Corlagon.  "
     "Roland is in a castle to the northwest, and you will lose if he falls to the "
     "enemy.  Remember that capturing Lord Corlagon will ensure that he will not "
     "fight against you in the final scenario."),
     DATA_COMPGEN(0x005140cc, cCampaignDescriptionThisIsTheFinalBattleBoth, "This is the final battle.  Both you and your enemy are armed to the teeth, "
     "and all are allied against you.  Capture Archibald to end the war!"),
     "",
     DATA_COMPGEN(0x00514160, cCampaignDescriptionSwitchingSidesLeavesYouWithThree, "Switching sides leaves you with three castles against the enemy's one.  This "
     "battle will be the easiest one you will face for the rest of the war...traitor.")},
    {DATA_COMPGEN(0x00514200, cCampaignDescriptionKingArchibaldRequiresYouToDefeat, "King Archibald requires you to defeat the three enemies in this region.  "
     "They are not allied with one another, so they will spend most of their "
     "energy fighting amongst themselves.  You will win when you own all of the "
     "enemy castles and there are no more heroes left to fight."),
     DATA_COMPGEN(0x00514314, cCampaignDescriptionYouMustUnifyTheBarbarianTribes, "You must unify the barbarian tribes of the north by conquering them.  As in "
     "the previous mission, the enemy is not allied against you, but they have "
     "more resources at their disposal.  You will win when you own all of the "
     "enemy castles and there are no more heroes left to fight."),
     DATA_COMPGEN(0x0051442c, cCampaignDescriptionDoGooderWizardsHaveTakenThe, "Do-gooder wizards have taken the Necromancers' castle.  You must retake it "
     "to achieve victory.  Remember that while you start with a powerful army, "
     "you have no castle and must take one within 7 days, or lose this battle.  "
     "(Hint: The nearest castle is to the southeast.)"),
     DATA_COMPGEN(0x0051453c, cCampaignDescriptionTheDwarvesNeedConqueringBeforeThey, "The Dwarves need conquering before they can interfere in King Archibald's "
     "plans.  Roland's forces have more than one hero and many towns to start "
     "with, so be ready for attack from multiple directions.  You must capture "
     "all of the enemy towns and castles to claim victory."),
     DATA_COMPGEN(0x0051464c, cCampaignDescriptionYourEnemiesAreAlliedAgainstYou2, "Your enemies are allied against you and start close by, so be ready to come "
     "out fighting.  You will need to own all four castles in this small valley "
     "to win."),
     DATA_COMPGEN(0x005146ec, cCampaignDescriptionYouMustPutDownAPeasant, "You must put down a peasant revolt led by Roland's forces.  All are allied "
     "against you, but you have Lord Corlagon, an experienced hero, to help you.  "
     "Capture all enemy castles to win."),
     DATA_COMPGEN(0x005147a8, cCampaignDescriptionThereAreTwoEnemiesAlliedAgainst, "There are two enemies allied against you in this mission.  Both are well "
     "armed and seek to evict you from their island.  Avoid them and capture "
     "Dragon City to win."),
     DATA_COMPGEN(0x0051484c, cCampaignDescriptionYourOrdersAreToConquerThe, "Your orders are to conquer the country lords that have sworn to serve "
     "Roland.  All of the enemy castles are unified against you.  Since you start "
     "without a castle, you must hurry to capture one before the end of the week.  "
     "Capture all enemy castles for victory."),
     DATA_COMPGEN(0x00514954, cCampaignDescriptionFindTheCrownBeforeRolandS, "Find the Crown before Roland's heroes find it.  Archibald will need the "
     "Crown for the final battle with Roland."),
     DATA_COMPGEN(0x005149c4, cCampaignDescriptionGatherAsLargeAnArmyAs2, "Gather as large an army as possible and capture the enemy castle within 8 "
     "weeks.  You are opposed by only one enemy, but must travel a long way to get "
     "to the enemy castle.  Any troops you have in your army at the end of this "
     "scenario will be with you in the next."),
     DATA_COMPGEN(0x00514acc, cCampaignDescriptionThisIsTheFinalBattleBoth2, "This is the final battle.  Both you and your enemy are armed to the teeth, "
     "and all are allied against you.  Capture Roland to win the war, and be sure "
     "not to lose Archibald in the fight!"),
     DATA_COMPGEN(0x00514b88, cCampaignDescriptionSwitchingSidesLeavesYouWithThree2, "Switching sides leaves you with three castles against the enemy's one.  This "
     "battle will be the easiest one you will face for the rest of the war...traitor.")}
};
DATA(0x004ffe80) char* cOutOfMemory =
    DATA_COMPGEN(0x00514c28, cOutOfMemorySHeroesIIRequiresAMinimum, "\n\n\n\n\n\n\n\n\n\n\n\n\n\n%s\nHeroes II requires a minimum of\n%dK of Extended Memory (XMS) "
    "and\n480K of Conventional Memory\n\n");
DATA(0x004ffe88) char* cSlowVideoLevelText[KB_SLOW_VIDEO_LEVEL_TEXT_COUNT] = {
    DATA_COMPGEN(0x00514c98, cSlowVideoLevelTextNormal, "Normal"),
    DATA_COMPGEN(0x00514ca0, cSlowVideoLevelTextInterlaced, "Interlaced")
};
DATA(0x004ffe90) char* gSPanelHelp[KB_SETTINGS_PANEL_HELP_COUNT] = {
    DATA_COMPGEN(0x00514cac, gSPanelHelpOKExitThisMenu, "{OK}\n\nExit this menu."),
    DATA_COMPGEN(0x00514cc4, gSPanelHelpMusicToggleAmbientMusicLevelNote, "{Music}\n\nToggle ambient music level.\n\n(Note: When using CD Stereo music in DOS, the music "
    "may only be turned on or off - the level has no effect.)"),
    DATA_COMPGEN(0x00514d58, gSPanelHelpEffectsToggleForegroundSoundsLevel, "{Effects}\n\nToggle foreground sounds level."),
    DATA_COMPGEN(0x00514d84, gSPanelHelpSpeedChangeTheSpeedAtWhich, "{Speed}\n\nChange the speed at which your heroes move on the main screen."),
    DATA_COMPGEN(0x00514dcc, gSPanelHelpMusicTypeChangeTheTypeOf, "{Music Type}\n\nChange the type of music.  MIDI music does not usually sound as good, but "
    "hurts performance less than CD Stereo.  If you use CD Stereo, you can select whether or not "
    "to have the operatic town themes play or not.\n\n(Note: Some older computers do not handle CD "
    "Stereo music well.  If you have a computer that leaves the music playing when you quit or "
    "crashes when you switch music tracks, try MIDI music.)"),
    DATA_COMPGEN(0x00514f70, gSPanelHelpShowPathToggleShowPathOn, "{Show Path}\n\nToggle 'Show Path' on/off.  If 'Show Path' is on, your first click on a map "
    "location will show the path to get there, your second will start you moving. If this option "
    "is off, one click starts you moving immediately."),
    DATA_COMPGEN(0x00515058, gSPanelHelpEnemySpeedSetsTheSpeedThat, "{Enemy Speed}\n\nSets the speed that A.I. heroes move at.  You can also elect not to view "
    "A.I. movement at all."),
    DATA_COMPGEN(0x005150c8, gSPanelHelpInterfaceSetsWhatTypeOfInterface, "{Interface}\n\nSets what type of interface you want to use.  The default selection is a "
    "dynamic interface, which uses Evil graphics for the 3 evil heroes (Barbarian, Warlock, and "
    "Necromancer)."),
    DATA_COMPGEN(0x00515188, gSPanelHelpVideoDeterminesIfTheVideoSequences, "{Video}\n\nDetermines if the video sequences play normally or in interlaced mode.  Interlaced "
    "mode runs better on slower machines, or machines with double-speed CD drives."),
    DATA_COMPGEN(0x00515234, gSPanelHelpMouseCursorToggleColorCursorsOn, "{Mouse Cursor}\n\nToggle color cursors on/off.  Color cursors look nicer, but sometimes don't "
    "move as smoothly as black and white ones.")
};
DATA(0x004ffeb8) char* xBarrierColor[KB_BARRIER_COLOR_NAME_COUNT] =
    {DATA_COMPGEN(0x005152bc, xBarrierColorAqua, "aqua"), DATA_COMPGEN(0x005152c4, xBarrierColorBlue, "blue"), DATA_COMPGEN(0x005152cc, xBarrierColorBrown, "brown"), DATA_COMPGEN(0x005152d4, xBarrierColorGold, "gold"), DATA_COMPGEN(0x005152dc, xBarrierColorGreen, "green"), DATA_COMPGEN(0x005152e4, xBarrierColorOrange, "orange"), DATA_COMPGEN(0x005152ec, xBarrierColorPurple, "purple"), DATA_COMPGEN(0x005152f4, xBarrierColorRed, "red")};
DATA(0x004ffed8) char* xGenericSiteNames[KB_GENERIC_SITE_NAME_COUNT] = {
    DATA_COMPGEN(0x005152f8, xGenericSiteNamesAlchemistSTower, "Alchemist's Tower"),
    DATA_COMPGEN(0x0051530c, xGenericSiteNamesArena, "Arena"),
    DATA_COMPGEN(0x00515314, xGenericSiteNamesHutOfTheMagi, "Hut of the Magi"),
    DATA_COMPGEN(0x00515324, xGenericSiteNamesEyeOfTheMagi, "Eye of the Magi"),
    DATA_COMPGEN(0x00515334, xGenericSiteNamesStables, "Stables"),
    DATA_COMPGEN(0x0051533c, xGenericSiteNamesMermaid, "Mermaid"),
    DATA_COMPGEN(0x00515344, xGenericSiteNamesSirens, "Sirens"),
    NULL
};
DATA(0x004ffef8) char* xRecruitmentSiteNames[KB_RECRUITMENT_SITE_NAME_COUNT] = {
    DATA_COMPGEN(0x0051534c, xRecruitmentSiteNamesBarrowMounds, "Barrow Mounds"),
    DATA_COMPGEN(0x0051535c, xRecruitmentSiteNamesEarthSummoningAltar, "Earth Summoning Altar"),
    DATA_COMPGEN(0x00515374, xRecruitmentSiteNamesAirSummoningAltar, "Air Summoning Altar"),
    DATA_COMPGEN(0x00515388, xRecruitmentSiteNamesFireSummoningAltar, "Fire Summoning Altar"),
    DATA_COMPGEN(0x005153a0, xRecruitmentSiteNamesWaterSummoningAltar, "Water Summoning Altar"),
    NULL
};
DATA(0x004fff10) SWinSetup gWinSetup[KB_WIN_SETUP_COUNT] = {
    {0, 100, DATA_COMPGEN(0x005153b8, gWinSetupBuildImprovement, "Build improvement:")},
    {1, 100, DATA_COMPGEN(0x005153cc, gWinSetupSpeed, "Speed")},
    {1, 101, DATA_COMPGEN(0x005153d4, gWinSetupMonsterInfo, "Monster Info")},
    {1, 102, DATA_COMPGEN(0x005153e4, gWinSetupAutoCombatSpellCasting, "Auto Combat\nSpell Casting")},
    {1, 103, DATA_COMPGEN(0x00515400, gWinSetupGrid, "Grid")},
    {1, 104, DATA_COMPGEN(0x00515408, gWinSetupShadowCursor, "Shadow\nCursor")},
    {1, 105, DATA_COMPGEN(0x00515418, gWinSetupShadowMovement, "Shadow\nMovement")},
    {2, 100, DATA_COMPGEN(0x00515428, gWinSetupMusic, "Music")},
    {2, 101, DATA_COMPGEN(0x00515430, gWinSetupEffects, "Effects")},
    {2, 102, DATA_COMPGEN(0x00515438, gWinSetupMusicType, "Music Type")},
    {2, 103, DATA_COMPGEN(0x00515444, gWinSetupSpeed2, "Speed")},
    {2, 104, DATA_COMPGEN(0x0051544c, gWinSetupShowPath, "Show Path")},
    {2, 105, DATA_COMPGEN(0x00515458, gWinSetupEnemySpeed, "Enemy Speed")},
    {2, 106, DATA_COMPGEN(0x00515464, gWinSetupInterface, "Interface")},
    {2, 107, DATA_COMPGEN(0x00515470, gWinSetupVideo, "Video")},
    {2, 108, DATA_COMPGEN(0x00515478, gWinSetupMouseCursor, "Mouse Cursor")},
    {6, 300, DATA_COMPGEN(0x00515488, gWinSetupAttackSkill, "Attack Skill")},
    {6, 301, DATA_COMPGEN(0x00515498, gWinSetupDefenseSkill, "Defense Skill")},
    {6, 302, DATA_COMPGEN(0x005154a8, gWinSetupSpellPower, "Spell Power")},
    {6, 303, DATA_COMPGEN(0x005154b4, gWinSetupKnowledge, "Knowledge")},
    {7, 600, DATA_COMPGEN(0x005154c0, gWinSetupGameDifficulty2, "Game Difficulty:")},
    {7, 57, DATA_COMPGEN(0x005154d4, gWinSetupEasy, "Easy")},
    {7, 58, DATA_COMPGEN(0x005154dc, gWinSetupNormal, "Normal")},
    {7, 59, DATA_COMPGEN(0x005154e4, gWinSetupHard, "Hard")},
    {7, 60, DATA_COMPGEN(0x005154ec, gWinSetupExpert, "Expert")},
    {7, 61, DATA_COMPGEN(0x005154f4, gWinSetupImpossible, "Impossible")},
    {7, 62, DATA_COMPGEN(0x00515500, gWinSetupOpponents2, "Opponents:")},
    {7, 84, DATA_COMPGEN(0x0051550c, gWinSetupClass2, "Class:")},
    {9, 41, DATA_COMPGEN(0x00515514, gWinSetupGoldPerDay, "Gold Per Day:")},
    {12, 0, DATA_COMPGEN(0x00515524, gWinSetupBuildANewShip, "Build a new ship:")},
    {12, 1, DATA_COMPGEN(0x00515538, gWinSetupResourceCost, "Resource cost:")},
    {14, 800, DATA_COMPGEN(0x00515548, gWinSetup1st, "1st")},
    {14, 801, DATA_COMPGEN(0x0051554c, gWinSetup2nd, "2nd")},
    {14, 802, DATA_COMPGEN(0x00515550, gWinSetup3rd, "3rd")},
    {14, 803, DATA_COMPGEN(0x00515554, gWinSetup4th, "4th")},
    {14, 804, DATA_COMPGEN(0x00515558, gWinSetup5th, "5th")},
    {14, 805, DATA_COMPGEN(0x0051555c, gWinSetup6th, "6th")},
    {14, 604, DATA_COMPGEN(0x00515560, gWinSetupNumberOfTowns, "Number of Towns:")},
    {14, 605, DATA_COMPGEN(0x00515574, gWinSetupNumberOfCastles, "Number of Castles:")},
    {14, 606, DATA_COMPGEN(0x00515588, gWinSetupNumberOfHeroes, "Number of Heroes:")},
    {14, 607, DATA_COMPGEN(0x0051559c, gWinSetupGoldInTreasury, "Gold in Treasury:")},
    {14, 608, DATA_COMPGEN(0x005155b0, gWinSetupWoodOre, "Wood & Ore:")},
    {14, 609, DATA_COMPGEN(0x005155bc, gWinSetupGemsCrSlfMer, "Gems, Cr, Slf & Mer:")},
    {14, 610, DATA_COMPGEN(0x005155d4, gWinSetupObelisksFound, "Obelisks Found:")},
    {14, 611, DATA_COMPGEN(0x005155e4, gWinSetupArtifacts, "Artifacts:")},
    {14, 612, DATA_COMPGEN(0x005155f0, gWinSetupTotalArmyStrength, "Total Army Strength:")},
    {14, 613, DATA_COMPGEN(0x00515608, gWinSetupIncome, "Income:")},
    {14, 620, DATA_COMPGEN(0x00515610, gWinSetupBestHero, "Best Hero:")},
    {14, 621, DATA_COMPGEN(0x0051561c, gWinSetupBestHeroStats, "Best Hero Stats:")},
    {14, 622, DATA_COMPGEN(0x00515630, gWinSetupPersonality, "Personality:")},
    {14, 623, DATA_COMPGEN(0x00515640, gWinSetupBestMonster, "Best Monster:")},
    {14, 0, DATA_COMPGEN(0x00515650, gWinSetupThievesGuildPlayerRankings, "Thieves' Guild: Player Rankings")},
    {17, 110, DATA_COMPGEN(0x00515670, gWinSetupTheAboveSpellsHaveBeenAdded, "The above spells have been added to your book.")},
    {18, 600, DATA_COMPGEN(0x005156a0, gWinSetupAttack, "Attack:")},
    {18, 601, DATA_COMPGEN(0x005156a8, gWinSetupDefense, "Defense:")},
    {18, 602, DATA_COMPGEN(0x005156b4, gWinSetupSpellPower3, "Spell Power:")},
    {18, 603, DATA_COMPGEN(0x005156c4, gWinSetupKnowledge3, "Knowledge:")},
    {18, 604, DATA_COMPGEN(0x005156d0, gWinSetupSpellPoints, "Spell Points:")},
    {19, 600, DATA_COMPGEN(0x005156e0, gWinSetupDefenders, "Defenders:")},
    {20, 600, DATA_COMPGEN(0x005156ec, gWinSetupRecruitHero, "Recruit Hero")},
    {21, 600, DATA_COMPGEN(0x005156fc, gWinSetupAttackSkill2, "Attack Skill")},
    {21, 601, DATA_COMPGEN(0x0051570c, gWinSetupDefenseSkill2, "Defense Skill")},
    {21, 602, DATA_COMPGEN(0x0051571c, gWinSetupSpellPower2, "Spell Power")},
    {21, 603, DATA_COMPGEN(0x00515728, gWinSetupKnowledge2, "Knowledge")},
    {22, 0, DATA_COMPGEN(0x00515734, gWinSetupTavern, "Tavern")},
    {23, 600, DATA_COMPGEN(0x0051573c, gWinSetupMapDifficulty, "Map\nDifficulty")},
    {23, 601, DATA_COMPGEN(0x0051574c, gWinSetupGameDifficulty, "Game\nDifficulty")},
    {23, 602, DATA_COMPGEN(0x0051575c, gWinSetupRating, "\nRating")},
    {23, 603, DATA_COMPGEN(0x00515764, gWinSetupMapSize, "\nMap Size")},
    {23, 604, DATA_COMPGEN(0x00515770, gWinSetupOpponents, "Opponents")},
    {23, 605, DATA_COMPGEN(0x0051577c, gWinSetupClass, "Class")},
    {23, 606, DATA_COMPGEN(0x00515784, gWinSetupVictoryConditions, "Victory\nConditions")},
    {23, 607, DATA_COMPGEN(0x00515798, gWinSetupLossConditions, "Loss\nConditions")}
};
DATA(0x00500110) b32 gbHeroWindShowing = false;
DATA(0x00500114) b32 gbFullCombatScreenDrawn = true;
DATA(0x00500118) b32 gbLimitedCombatUpdatePalette = false;
DATA(0x0050011c) b32 gbFirstTimeThrough = false;
DATA(0x00500120) b32 gbSkipIntro = false;
DATA(0x00500124) b32 gbDoMemCheck = true;
DATA(0x00500128) b32 gbAllBlack = false;
DATA(0x0050012c) b32 gbInCombat = false;
DATA(0x00500130) b32 gbDirectConnect = false;
DATA(0x00500134) i32 giForceSwitchMusic = -1;
DATA(0x00500138) b32 gbComputeExtent = false;
DATA(0x0050013c) b32 gbSaveBiggestExtent = false;
DATA(0x00500140) b32 gbLimitToExtent = false;
DATA(0x00500144) b32 gbCurrArmyDrawn = true;
DATA(0x00500148) H2_ENUM_STORAGE(AdventureDisposeLevel, i32) gAdvDisposeLevel = ADV_DISPOSE_NONE;
DATA(0x0050014c) b32 gbRemoteOn = false;
DATA(0x00500150) b32 gbGameInitialized = false;
DATA(0x00500154) i32 giHighScoreRank = -1;
DATA(0x00500158) H2_ENUM_STORAGE(HighScoreType, i32) giHighScoreType = HIGH_SCORE_STANDARD;
DATA(0x0050015c) b32 gbShowHighScore = false;
DATA(0x00500160) b32 gbLowMemory = false;
DATA(0x00500164) i32 giHighMemBuffer = CHECK_MEMORY_INITIAL_AVAILABLE_KB;
DATA(0x00500168) void* gLowPage = NULL;
DATA(0x0050016c) b32 gbLowPageGrabbed = false;
DATA(0x00500170) i8 xSmackFromNetwork = 0;
DATA(0x00500174) b32 gbInPollSound = false;
DATA(0x005157a8) H2_ENUM_STORAGE(CDRomSetupResult, i32) iCDRomErr = CD_ROM_READY;
DATA(0x005157ac) i32 bEarlySetupDone = 0;
DATA(0x005159f8) i32 bKBDone = 0;
DATA(0x005159fc) struct _REDBOOK* hRedbookz = NULL;
DATA(0x00515a00) i32 bForceCheckTimeEvent = 0;
DATA(0x00515ca0) u16 IMHotSpots[KB_INIT_MENU_HOTSPOT_COUNT][IDX(INIT_MENU_HOTSPOT_FIELD_COUNT)] = {
    {481, 185, 83, 96},
    {194, 179, 82, 79},
    {412, 105, 75, 76},
    {303, 137, 75, 44},
    {0, 389, 86, 90}
};
// NOLINTEND(readability-magic-numbers)

DATA(0x00515cc8) i32 lastIMHoverID = -1;
DATA(0x00515f78) i32 bInCheckEndGame = 0;
DATA(0x005165dc) i32 bInShutDown = 0;
DATA(0x00516810) b32 gbInMemError = false;
DATA(0x00516d1c) i32 iShingleAnimFrame = 0;
DATA(0x00528598) b32 gbHumanPlayer[IDX(GAME_PLAYER_COUNT)];
DATA(0x005285b0) b32 gbHitEvent;
DATA(0x005285b4) i32 giMaxExtentX;
DATA(0x005285b8) i32 giMaxExtentY;
DATA(0x005285bc) i32 giRandomClouds;
DATA(0x005285c0) char cOverrideDigitalDriver[GLOBAL_DRIVER_NAME_SIZE];
DATA(0x005285d0) i32 giBottomViewOverrideEndTime;
DATA(0x005285d8) i8 gArmyEffected[COMBAT_SIDE_COUNT][KB_ARMY_EFFECT_COUNT];
DATA(0x00528600) H2_ENUM_STORAGE(ResourceType, i32) giBottomViewResource;
DATA(0x00528604) b32 gbInCampaign;
DATA(0x00528608) i32 giResExtra1;
DATA(0x0052860c) i32 giResExtra2;
DATA(0x00528610) i8 puzzlePiecesRemoved[PUZZLE_PIECE_STORAGE_SIZE];
DATA(0x00528618) i32 giSeedingValid;
DATA(0x0052861c) i32 giLimitPlayer;
DATA(0x00528620) i32 giShowClouds;
DATA(0x00528624) i32 bDoColorCycle;
DATA(0x00528628) inputManager* gpInputManager;
DATA(0x00528630) SAMPLE2 NULL_SAMPLE2;
DATA(0x00528638) i32 iMaxMapExtra;
DATA(0x0052863c) palette* gPalette;
DATA(0x00528640) resourceManager* gpResourceManager;
DATA(0x00528648) char gcBotViewText[GLOBAL_BOTTOM_VIEW_TEXT_SIZE];
DATA(0x005286e0) i32 bSpecialHideCursor;
DATA(0x005286e4) searchArray* gpSearchArray;
DATA(0x005286e8) i32 giResType1;
DATA(0x005286ec) b32 gbBlackoutPlayer;
DATA(0x005286f0) i32 giResType2;
DATA(0x005286f8) char cNetBoxLine[BOX_LINE_COUNT][NET_BOX_LINE_SIZE];
DATA(0x00528928) i32 gIndex;
DATA(0x0052892c) i32 giWeekTypeExtra;
DATA(0x00528930) philAI* gpPhilAI;
DATA(0x00528934) i32 giTCPType;
DATA(0x00528938) H2_ENUM_STORAGE(CombatEffectType, i32) gCurLoadedSpellEffect;
DATA(0x0052893c) class mouseManager* gpMouseManager;
DATA(0x00528940) i32 giCurTempMobility;
DATA(0x00528944) OverviewReturnAction giOverviewReturnAction;
DATA(0x00528948) char cOverrideMIDIDriver[GLOBAL_DRIVER_NAME_SIZE];
DATA(0x00528958) icon* gSystemIcons;
DATA(0x0052895c) b8 gbCombatSurrender;
DATA(0x00528960) heroWindow* pNormalDialogWindow;
DATA(0x00528964) i32 giTCPHostStatus;
DATA(0x00528968) char gMapName[GLOBAL_SHORT_MAP_NAME_SIZE];
DATA(0x00528978) i32 giMinExtentX;
DATA(0x0052897c) i32 giMinExtentY;
DATA(0x00528980) MultiplayerBaseType iMPBaseType;
DATA(0x00528984) b32 gbTCPFirstTime;
DATA(0x00528988) i16* pwSizeOfMapExtra;
DATA(0x0052898c) i32 giHeroScreenSrcIndex;
DATA(0x00528990) H2_ENUM_STORAGE(CalendarPeriodType, i32) giWeekType;
DATA(0x00528998) char gText[GLOBAL_TEXT_BUFFER_SIZE];
DATA(0x00528c98) b32 gbInNewGameSetup;
DATA(0x00528c9c) class palette* gpBufferPalette;
DATA(0x00528ca0) H2_ENUM_STORAGE(CampaignSide, i32) gbCampaignSideChoice;
DATA(0x00528ca4) char cNetBoxColor[BOX_LINE_COUNT];
DATA(0x00528ca8) i32 giMonthTypeExtra;
DATA(0x00528cac) H2_ENUM_STORAGE(RemoteGameMode, i32) iMPExtendedType;
DATA(0x00528cb0) i8 gcColorToSetupPos[RADAR_OWNER_COLOR_COUNT];
DATA(0x00528cb8) char gFullMapName[GLOBAL_MAP_NAME_SIZE];
DATA(0x00528cd0) char gcTCPName[GLOBAL_TCP_TEXT_SIZE];
DATA(0x00528ce8) i32 giShowIntro;
DATA(0x00528cf0) i32 glTimers[GLOBAL_TIMER_COUNT];
DATA(0x00528d18) i32 giScore;
DATA(0x00528d1c) armyGroup* gpMonGroup;
DATA(0x00528d20) configStruct gConfig;
DATA(0x00528ec0) char gcRegAppPath[GLOBAL_AGGREGATE_PATH_SIZE];
DATA(0x00529020) u32l gTimeMark;
DATA(0x00529024) char* EXPANSION_AGGREGATE_NAME;
DATA(0x00529028) char cPlayerNames[X_GLOBAL_PLAYER_COUNT][GLOBAL_PLAYER_NAME_SIZE];
DATA(0x005290a8) game* gpGame;
DATA(0x005290ac) b8 gbRetreatWin;
DATA(0x005290b0) DialogWaitType giWaitType;
DATA(0x005290b4) class icon* gCurLoadedSpellIcon;
DATA(0x005290b8) u8 bSaveMusicPosition[KB_MUSIC_TRACK_COUNT];
DATA(0x005290f4) H2_ENUM_STORAGE(BottomViewMode, i32) giBottomViewOverride;
DATA(0x005290f8) char gcTCPAddress[GLOBAL_TCP_TEXT_SIZE];
DATA(0x00529110) u8 giSetupGameType;
DATA(0x00529118) char gLastFilename[GLOBAL_AGGREGATE_PATH_SIZE];
DATA(0x00529278) i32 giFullySeeded;
DATA(0x0052927c) icon* gBuyBuildIcons;
DATA(0x00529280) b32 gbNoSound;
DATA(0x00529288) i32 iCombatControlNetPos[COMBAT_SIDE_COUNT];
DATA(0x00529290) char cExpAggPathName[GLOBAL_AGGREGATE_PATH_SIZE];
DATA(0x005293f0) b32 gbMoveShown;
DATA(0x005293f4) void** ppMapExtra;
DATA(0x005293f8) char gcBottomViewText[GLOBAL_BOTTOM_VIEW_MESSAGE_SIZE];
DATA(0x00529454) i32 giThisNetPos;
DATA(0x00529458) b8 gbSetupGamePosToRealGamePos[RADAR_OWNER_COLOR_COUNT];
DATA(0x00529460) char gcRegCDRomPath[GLOBAL_AGGREGATE_PATH_SIZE];
DATA(0x005295c0) class heroWindow* heroWin;
DATA(0x005295c4) i32 giOverviewReturnActionExtra;
DATA(0x005295c8) H2_ENUM_STORAGE(CombatSide, i32) giCurGeneral;
DATA(0x005295cc) i32 giThisGamePos;
DATA(0x005295d0) i32 giNumHumanPlayers;
DATA(0x005295d4) b32 gbIconClipOn;
DATA(0x005295d8) b32 gbRemoteGameOpen;
DATA(0x005295dc) combatManager* gpCombatManager;
DATA(0x005295e0) i32 giTCPNumPlayers;
DATA(0x005295e4) executive* gpExec;
DATA(0x005295e8) void* hMainWindow;
DATA(0x005295ec) i32 giCurWindowsStyleFlags;
DATA(0x005295f0) i32 gGameCommand;
DATA(0x005295f4) H2_ENUM_STORAGE(CalendarPeriodType, i32) giMonthType;
DATA(0x005295f8) char* DEFAULT_AGGREGATE_NAME;
DATA(0x005295fc) i32 gCurSpellEffectFrame;
DATA(0x00529600) b8 gbThisNetHumanPlayer[RADAR_OWNER_COLOR_COUNT];
DATA(0x00529608) char cAggPathName[GLOBAL_AGGREGATE_PATH_SIZE];
DATA(0x00529768) class highScoreManager* gpHighScoreManager;
DATA(0x0052976c) b32 gbFunctionComplete;
DATA(0x00529770) b32 gbIAmGreatest;
DATA(0x00529774) b32 gbTextEntryEscaped;
DATA(0x00529778) i32 giTotalHighMem;
DATA(0x0052977c) i32 gMapX;
DATA(0x00529780) i32 gMapY;
DATA(0x00529788) char gcWinText[GLOBAL_WINDOW_TEXT_SIZE];
DATA(0x005298b4) i32 bFreshSave;
DATA(0x005298b8) i32 bShowIt;
DATA(0x005298bc) i32 gLowPageScreenSelector;
DATA(0x005298c0) class heroWindowManager* gpWindowManager;
DATA(0x005298c4) i32 giCurWatchPlayer;
DATA(0x005298c8) char gcCommandLine[GLOBAL_COMMAND_LINE_SIZE];
DATA(0x00529908) i32 giBottomViewResourceQty;
DATA(0x0052990c) soundManager* gpSoundManager;
DATA(0x00529910) b32 gbThisNetGotAdventureControl;
DATA(0x00529914) i32 giMapChangeCtr;
DATA(0x00529918) SMapChange sMapChangeQueue[CURSOR_MAP_CHANGE_QUEUE_COUNT];
DATA(0x0052a184) b32 gbWaitForRemoteReceive;
DATA(0x0052a188) u8 bMusicIsLooping[KB_MUSIC_TRACK_COUNT];
DATA(0x0052a1c4) townManager* gpTownManager;
DATA(0x0052a1c8) advManager* gpAdvManager;
DATA(0x0052a1d0) b8 gbGamePosToNetPos[OLD_MAIN_MATCH_BUFFER_SIZE];

#undef RETAIL_FILE
