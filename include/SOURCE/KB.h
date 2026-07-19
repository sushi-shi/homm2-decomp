#ifndef HOMM2_KB_H
#define HOMM2_KB_H

#include <va.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/hero.h>
#include <SOURCE/REMOTE_TYPES.h>

struct SAMPLE2;
struct SSpellInfo;
struct SWinSetup;
class advManager;
class armyGroup;
class combatManager;
struct configStruct;
class executive;
class font;
class game;
class hero;
class heroWindow;
class heroWindowManager;
class icon;
class inputManager;
class mouseManager;
class palette;
class philAI;
class resourceManager;
class searchArray;
class soundManager;
struct tag_message;
struct tag_monsterInfo;
struct tag_tilePoint;
struct mapEventExtra;
class town;
class townManager;
#include <SOURCE/KB_TYPES.h>
#include <BASE/message.h>
#include <BASE/WINMGR.h>

H2_ENUM_BEGIN(GlobalTimerConstant)
    GLOBAL_TIMER_COUNT              = 10,
    GLOBAL_BUTTON_REPEAT_TIMER_SLOT = 2,
    GLOBAL_MUSIC_FADE_TIMER_SLOT    = 4,
    GLOBAL_POLL_SOUND_TIMER_SLOT    = 5,
    GLOBAL_MOUSE_TIMER_SLOT         = 6,
    GLOBAL_COLOR_CYCLE_TIMER_SLOT   = 7,
    GLOBAL_COMBAT_CYCLE_TIMER_SLOT  = 8
H2_ENUM_END(GlobalTimerConstant)

H2_ENUM_BEGIN(CombatHeroTableConstant)
    KB_COMBAT_HERO_VARIANT_COUNT = 2,
    KB_COMBAT_HERO_SPRITE_COUNT  = IDX(FACTION_COUNT) * KB_COMBAT_HERO_VARIANT_COUNT
H2_ENUM_END(CombatHeroTableConstant)

H2_ENUM_CLASS_BEGIN(CampaignSide)
    CAMPAIGN_ROLAND     = 0,
    CAMPAIGN_ARCHIBALD  = 1,
    CAMPAIGN_SIDE_COUNT = 2
H2_ENUM_CLASS_END(CampaignSide)

H2_ENUM_CLASS_BEGIN(MonsterScoreField)
    MONSTER_SCORE_THRESHOLD   = 0,
    MONSTER_SCORE_TYPE        = 1,
    MONSTER_SCORE_FIELD_COUNT = 2
H2_ENUM_CLASS_END(MonsterScoreField)

H2_ENUM_CLASS_BEGIN_T(CampaignChoiceType, u8)
    CAMPAIGN_CHOICE_RESOURCE        = 0,
    CAMPAIGN_CHOICE_ARTIFACT        = 1,
    CAMPAIGN_CHOICE_SPELL           = 2,
    CAMPAIGN_CHOICE_SECONDARY_SKILL = 3,
    CAMPAIGN_CHOICE_CREATURES       = 4,
    CAMPAIGN_CHOICE_PUZZLE_PIECES   = 5,
    CAMPAIGN_CHOICE_EXPERIENCE      = 6,
    CAMPAIGN_CHOICE_NONE            = 7,
    CAMPAIGN_CHOICE_ALIGNMENT       = 8,
    CAMPAIGN_CHOICE_PRIMARY_SKILL   = 9,
    CAMPAIGN_CHOICE_SPELL_SCROLL    = 10,
    CAMPAIGN_CHOICE_INVALID         = 255
H2_ENUM_CLASS_END_T(CampaignChoiceType, u8)

H2_ENUM_BEGIN(CampaignConstant)
    CAMPAIGN_MAP_COUNT                = 12,
    CAMPAIGN_REGULAR_MAP_COUNT        = 11,
    CAMPAIGN_TRACK_POINT_COUNT        = 13,
    CAMPAIGN_BONUS_CHOICE_COUNT       = 3,
    CAMPAIGN_AWARD_COUNT              = 12,
    CAMPAIGN_SWITCHING_MAP            = 11,
    CAMPAIGN_SWITCHING_SCENARIO       = 4,
    CAMPAIGN_NO_SCENARIO              = -1,
    CAMPAIGN_ROLAND_FINAL_SCENARIO    = 9,
    CAMPAIGN_ARCHIBALD_FINAL_SCENARIO = 10,
    CAMPAIGN_STATE_RESET_SIZE         = 0x147,
    CAMPAIGN_SETUP_RESET_SIZE         = 0x41,
    CAMPAIGN_ARMY_NAME_BUFFER_SIZE    = 52,
    CAMPAIGN_ARMY_SLOT_COUNT          = 5,
    CAMPAIGN_CARRYOVER_PLAYER         = 3,
    CAMPAIGN_TRIPLE_ARMY_MULTIPLIER   = 3,
    CAMPAIGN_EASY_SCENARIO_LIMIT      = 2,
    CAMPAIGN_NORMAL_SCENARIO_LIMIT    = 5,
    CAMPAIGN_HERO_COUNT               = 54,
    CAMPAIGN_HERO_PRIORITY_HIGH       = 100,
    CAMPAIGN_HERO_PRIORITY_NORMAL     = 90,
    CAMPAIGN_EXPERIENCE_BONUS         = 5000,
    CAMPAIGN_SWITCH_VICTORY_VALUE     = 99,
    CAMPAIGN_ROLAND_TIME_LIMIT        = 90
H2_ENUM_END(CampaignConstant)

#pragma pack(push, 1)
struct SCampaignChoice {
    CampaignChoiceType type;
    i16 value;
    i16 amount;
};
#pragma pack(pop)
SIZE(SCampaignChoice, 5);

extern SCampaignChoice campaignChoices[IDX(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT]
                                      [CAMPAIGN_BONUS_CHOICE_COUNT];
extern char* cCampaignName[IDX(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT];
extern char* cCampaignDescription[IDX(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT];

struct SPlayerExit {
    i8 netPosition;
    i8 gamePosition;
    i8 updateNetworkControl;
    i8 timedOut;
    i8 eliminated;
    i8 hostReported;
    i8 continueGame;
};
SIZE(SPlayerExit, 7);

H2_ENUM_BEGIN(PlayerExitConstant)
    PLAYER_EXIT_DIALOG_INFO        = 1,
    PLAYER_EXIT_DIALOG_CONFIRM     = 2,
    PLAYER_EXIT_CONFIRM_OK         = 0x7805,
    PLAYER_EXIT_NETWORK_SLOTS      = 6,
    PLAYER_EXIT_SHIFT_SLOTS        = 5,
    PLAYER_EXIT_MESSAGE_LENGTH     = 500,
    PLAYER_EXIT_MESSAGE_TIME       = 5000,
    PLAYER_EXIT_HEARTBEAT_DISABLED = 0x0bebc1ff,
    PLAYER_EXIT_PACKET_TYPE        = 7,
    PLAYER_EXIT_PACKET_COMMAND     = 0x21
H2_ENUM_END(PlayerExitConstant)

H2_ENUM_BEGIN(EventWindowConstant)
    EVENT_WINDOW_RESOURCE_FLAG          = 0x200,
    EVENT_WINDOW_CLOSE_COMMAND          = 10,
    EVENT_WINDOW_FIRST_RESOURCE_WIDGET  = 0x1e14,
    EVENT_WINDOW_SECOND_RESOURCE_WIDGET = 0x1e15,
    EVENT_WINDOW_FIRST_BUTTON           = 0x7800,
    EVENT_WINDOW_SECOND_BUTTON          = 0x7801,
    EVENT_WINDOW_THIRD_BUTTON           = 0x7802,
    EVENT_WINDOW_FOURTH_BUTTON          = 0x7803,
    EVENT_WINDOW_IGNORED_BUTTON         = 0x7804,
    EVENT_WINDOW_FIFTH_BUTTON           = 0x7805,
    EVENT_WINDOW_SIXTH_BUTTON           = 0x7806,
    EVENT_WINDOW_SEVENTH_BUTTON         = 0x7807,
    EVENT_WINDOW_EIGHTH_BUTTON          = 0x7808,
    EVENT_WINDOW_CONTINUE               = 1,
    EVENT_WINDOW_CLOSE                  = 2,
    EVENT_WINDOW_LUCK                   = 10,
    EVENT_WINDOW_BAD_LUCK               = 11,
    EVENT_WINDOW_MORALE                 = 12,
    EVENT_WINDOW_BAD_MORALE             = 13,
    EVENT_WINDOW_EXPERIENCE             = 14
H2_ENUM_END(EventWindowConstant)

H2_ENUM_BEGIN(KbBuildingConstant)
    KB_BUILDING_NEUTRAL_LIMIT  = 16,
    KB_BUILDING_RESOURCE_COUNT = 7,
    KB_MAGE_GUILD_MAX_LEVEL    = 5,
    KB_MAGE_GUILD_LEVEL_COUNT  = KB_MAGE_GUILD_MAX_LEVEL + 1,
    KB_DWELLING_TYPE_COUNT     =
        IDX(BUILDING_SLOT_DWELLING_LAST) - IDX(BUILDING_SLOT_DWELLING_FIRST) + 1
H2_ENUM_END(KbBuildingConstant)

H2_ENUM_CLASS_BEGIN(KbDwellingFlag)
    KB_DWELLING_FIRST_FLAG          = 0x00100000,
    KB_DWELLING_SECOND_FLAG         = 0x00200000,
    KB_DWELLING_THIRD_FLAG          = 0x00400000,
    KB_DWELLING_FOURTH_FLAG         = 0x00800000,
    KB_DWELLING_FIFTH_FLAG          = 0x01000000,
    KB_DWELLING_UPGRADE_FIRST_FLAG  = 0x02000000,
    KB_DWELLING_UPGRADE_SECOND_FLAG = 0x04000000,
    KB_DWELLING_UPGRADE_THIRD_FLAG  = 0x08000000,
    KB_DWELLING_UPGRADE_FOURTH_FLAG = 0x10000000,
    KB_DWELLING_UPGRADE_FIFTH_FLAG  = 0x20000000,
    KB_DWELLING_UPGRADE_SIXTH_FLAG  = 0x40000000
H2_ENUM_CLASS_END(KbDwellingFlag)

H2_ENUM_BEGIN(NormalDialogResourceType)
    NORMAL_DIALOG_NO_RESOURCE     = -1,
    NORMAL_DIALOG_RESOURCE_FIRST  = 0,
    NORMAL_DIALOG_RESOURCE_LAST   = 6,
    NORMAL_DIALOG_ARTIFACT        = 7,
    NORMAL_DIALOG_SPELL           = 8,
    NORMAL_DIALOG_CREST           = 9,
    NORMAL_DIALOG_EXPMRL_FIRST    = 10,
    NORMAL_DIALOG_EXPMRL_LAST     = 14,
    NORMAL_DIALOG_HERO            = 15,
    NORMAL_DIALOG_SECONDARY_SKILL = 17,
    NORMAL_DIALOG_MONSTER         = 18,
    NORMAL_DIALOG_PRIMARY_SKILL   = 25
H2_ENUM_END(NormalDialogResourceType)

H2_ENUM_BEGIN(NormalDialogConstant)
    NORMAL_DIALOG_INFO                     = 1,
    NORMAL_DIALOG_CONFIRM                  = 2,
    NORMAL_DIALOG_BUTTON_PAIR              = 3,
    NORMAL_DIALOG_QUICK_VIEW               = 4,
    NORMAL_DIALOG_WAIT_FIRST               = 5,
    NORMAL_DIALOG_WAIT_LAST                = 6,
    NORMAL_DIALOG_DISABLE_SEVENTH          = 7,
    NORMAL_DIALOG_DISABLE_EIGHTH           = 8,
    NORMAL_DIALOG_RESOURCE_COUNT           = 2,
    NORMAL_DIALOG_PRIMARY_BONUS_OFFSET     = 100,
    NORMAL_DIALOG_DAILY_RESOURCE_OFFSET    = 100000,
    NORMAL_DIALOG_SHOW_OR_TEXT             = 1,
    NORMAL_DIALOG_NO_VALUE                 = -1,
    NORMAL_DIALOG_TEXT_LENGTH              = 80,
    NORMAL_DIALOG_FILENAME_LENGTH          = 16,
    NORMAL_DIALOG_WINDOW_WIDTH             = 306,
    NORMAL_DIALOG_WINDOW_BASE_HEIGHT       = 180,
    NORMAL_DIALOG_WINDOW_ROW_HEIGHT        = 45,
    NORMAL_DIALOG_MAX_ROWS                 = 6,
    NORMAL_DIALOG_TEXT_LINE_WIDTH          = 244,
    NORMAL_DIALOG_TEXT_LINE_HEIGHT         = 16,
    NORMAL_DIALOG_SCREEN_RIGHT             = 639,
    NORMAL_DIALOG_SCREEN_BOTTOM            = 479,
    NORMAL_DIALOG_SCREEN_HEIGHT            = 480,
    NORMAL_DIALOG_MAX_TOP                  = 28,
    NORMAL_DIALOG_WIDGET_COLOR             = 0x10,
    NORMAL_DIALOG_TEXT_WIDGET_FIRST_ID     = 100,
    NORMAL_DIALOG_RESOURCE_BORDER_FIRST_ID = 0x1e14,
    NORMAL_DIALOG_TIMEOUT_MIN              = 1,
    NORMAL_DIALOG_TIMEOUT_MAX              = 20000,
    NORMAL_DIALOG_TEXT_WIDGET_ID           = 1,
    NORMAL_DIALOG_BUTTON_ONE               = 0x7801,
    NORMAL_DIALOG_BUTTON_TWO               = 0x7802,
    NORMAL_DIALOG_BUTTON_FIVE              = 0x7805,
    NORMAL_DIALOG_BUTTON_SIX               = 0x7806,
    NORMAL_DIALOG_BUTTON_SEVEN             = 0x7807,
    NORMAL_DIALOG_BUTTON_EIGHT             = 0x7808,
    NORMAL_DIALOG_WIDGET_FLAGS             = 0x200
H2_ENUM_END(NormalDialogConstant)

H2_ENUM_BEGIN(HighScoreConstant)
    HIGH_SCORE_ENTRY_COUNT        = 10,
    HIGH_SCORE_NAME_LENGTH        = 16,
    HIGH_SCORE_PLAYER_NAME_SIZE   = HIGH_SCORE_NAME_LENGTH + 1,
    HIGH_SCORE_SCENARIO_NAME_SIZE = 41,
    HIGH_SCORE_RESERVED_SIZE      = 29,
    HIGH_SCORE_STANDARD           = 1,
    HIGH_SCORE_CAMPAIGN           = 0,
    HIGH_SCORE_EXPANSION_CAMPAIGN = 2,
    HIGH_SCORE_EMPTY              = -1,
    HIGH_SCORE_FILE_READ_FLAGS    = 0x8000,
    HIGH_SCORE_FILE_WRITE_FLAGS   = 0x8301,
    HIGH_SCORE_FILE_PERMISSIONS   = 0x80
H2_ENUM_END(HighScoreConstant)

#pragma pack(push, 1)
struct HighScoreEntry {
    char playerName[HIGH_SCORE_PLAYER_NAME_SIZE];
    char scenarioName[HIGH_SCORE_SCENARIO_NAME_SIZE];
    i32 score;
    i32 days;
    i32 scenario;
    char cheated;
    char reserved[HIGH_SCORE_RESERVED_SIZE];
};
#pragma pack(pop)
SIZE(HighScoreEntry, 100);

H2_ENUM_BEGIN(AppMenuCommand)
    APP_MENU_NONE            = 0,
    APP_MENU_VIEW_WORLD      = 0x9c4c,
    APP_MENU_VIEW_PUZZLE     = 0x9c4d,
    APP_MENU_CAST_SPELL      = 0x9c4e,
    APP_MENU_SEARCH          = 0x9c4f,
    APP_MENU_MUSIC_FIRST     = 0x9c50,
    APP_MENU_MUSIC_LAST      = 0x9c5a,
    APP_MENU_SOUND_FIRST     = 0x9c5c,
    APP_MENU_SOUND_LAST      = 0x9c66,
    APP_MENU_SPEED_FIRST     = 0x9c68,
    APP_MENU_SPEED_LAST      = 0x9c6c,
    APP_MENU_UNKNOWN_9C6D    = 0x9c6d,
    APP_MENU_TOGGLE_ROUTE    = 0x9c6e,
    APP_MENU_TOGGLE_BLACKOUT = 0x9c6f,
    APP_MENU_RESTART_0       = 0x9ca6,
    APP_MENU_RESTART_1       = 0x9ca8,
    APP_MENU_RESTART_2       = 0x9ca9,
    APP_MENU_RESTART_3       = 0x9caa,
    APP_MENU_RESTART_4       = 0x9cab,
    APP_MENU_UNKNOWN_9CAD    = 0x9cad,
    APP_MENU_RESTART_5       = 0x9cae,
    APP_MENU_RESTART_6       = 0x9caf,
    APP_MENU_RESTART_7       = 0x9cb0,
    APP_MENU_RESTART_8       = 0x9cb2,
    APP_MENU_RESTART_9       = 0x9cb3,
    APP_MENU_RESTART_10      = 0x9cb5,
    APP_MENU_RESTART_11      = 0x9cb6,
    APP_MENU_RESTART_12      = 0x9cb8,
    APP_MENU_RESTART_13      = 0x9cb9,
    APP_MENU_LOAD_0          = 0x9cbb,
    APP_MENU_LOAD_1          = 0x9cbc,
    APP_MENU_LOAD_2          = 0x9cbf,
    APP_MENU_LOAD_3          = 0x9cc0,
    APP_MENU_LOAD_4          = 0x9cc1,
    APP_MENU_LOAD_5          = 0x9cc3,
    APP_MENU_LOAD_6          = 0x9cc4,
    APP_MENU_LOAD_7          = 0x9cc6,
    APP_MENU_LOAD_8          = 0x9cc7,
    APP_MENU_LOAD_9          = 0x9cc9,
    APP_MENU_LOAD_10         = 0x9cca,
    APP_MENU_SAVE            = 0x9ccb,
    APP_MENU_EXIT            = 0x9ccc,
    APP_MENU_CHEAT_REVEAL    = 0x9ccd,
    APP_MENU_CHEAT_MOVEMENT  = 0x9cce,
    APP_MENU_CHEAT_SPELLS    = 0x9ccf,
    APP_MENU_CHEAT_RESOURCES = 0x9cd0
H2_ENUM_END(AppMenuCommand)

H2_ENUM_BEGIN(OldMainConstant)
    OLD_MAIN_PLAYER_COUNT                     = 6,
    OLD_MAIN_MATCH_BUFFER_SIZE                = 8,
    OLD_MAIN_PLAYER_NAME_LENGTH               = 21,
    OLD_MAIN_DEFAULT_NAME_LENGTH              = 3,
    OLD_MAIN_DEFAULT_NAME_STRIDE              = 4,
    OLD_MAIN_SCREEN_WIDTH                     = 640,
    OLD_MAIN_SCREEN_HEIGHT                    = 480,
    OLD_MAIN_MAIN_MUSIC                       = 42,
    OLD_MAIN_HIGH_SCORE_MUSIC                 = 43,
    OLD_MAIN_FADE_SPEED                       = 8,
    OLD_MAIN_LONG_FADE_SPEED                  = 0x80,
    OLD_MAIN_DEBUG_MEMORY_CHECK_LEVEL         = 9,
    OLD_MAIN_INTRO_PRIMARY_VIDEO              = 0x42,
    OLD_MAIN_INTRO_FALLBACK_VIDEO             = 1,
    OLD_MAIN_INTRO_SECONDARY_VIDEO            = 0x41,
    OLD_MAIN_CREDITS_FIRST_VIDEO              = 0x48,
    OLD_MAIN_CREDITS_SECOND_VIDEO             = 0x24,
    OLD_MAIN_STANDARD_VICTORY_VIDEO           = 3,
    OLD_MAIN_EXPANSION_VICTORY_VIDEO          = 0x40,
    OLD_MAIN_NEW_GAME                         = 0x65,
    OLD_MAIN_LOAD_GAME                        = 0x66,
    OLD_MAIN_HIGH_SCORES                      = 0x67,
    OLD_MAIN_CREDITS                          = 0x68,
    OLD_MAIN_EXIT                             = 0x69,
    OLD_MAIN_SETUP_NEW                        = 0,
    OLD_MAIN_SETUP_LOAD                       = 1,
    OLD_MAIN_REMOTE_HOST                      = 1,
    OLD_MAIN_REMOTE_CLIENT                    = 2,
    OLD_MAIN_REGULAR_COMPRESSION_MEMORY_LIMIT = 5999,
    OLD_MAIN_NET_BUFFER_SIZE                  = 256,
    OLD_MAIN_NETWORK_PACKET                   = 0x20,
    OLD_MAIN_ARCHIBALD_FINAL_SCENARIO         = 10,
    OLD_MAIN_ROLAND_FINAL_SCENARIO            = 9,
    OLD_MAIN_ROLAND_CAMPAIGN                  = 0,
    OLD_MAIN_DIALOG_WAIT                      = 6,
    OLD_MAIN_WAIT_DIRECT_CONNECT              = 7,
    OLD_MAIN_WAIT_DP_FIRST_GUEST              = 8,
    OLD_MAIN_WAIT_DP_EXTRA_GUESTS             = 9,
    OLD_MAIN_WAIT_DP_HOST                     = 0xa,
    OLD_MAIN_WAIT_WS_FIRST_GUEST              = 0xb,
    OLD_MAIN_WAIT_WS_EXTRA_GUESTS             = 0xc,
    OLD_MAIN_WAIT_WS_HOST                     = 0xd,
    OLD_MAIN_REMOTE_PREFIX_RESERVED_SIZE      = 4,
    OLD_MAIN_REMOTE_BODY_RESERVED_SIZE        = 2,
    OLD_MAIN_REMOTE_PAYLOAD_HEAD_SIZE         = 1
H2_ENUM_END(OldMainConstant)

#pragma pack(push, 1)
struct OldMainNetSetup {
    i8 gamePosToNetPos[OLD_MAIN_PLAYER_COUNT];
    i8 useRegularCompression;
    i8 useDiffCompression;
    SNetPlayerInfo players[OLD_MAIN_PLAYER_COUNT];
};
#pragma pack(pop)
SIZE(OldMainNetSetup, 0xd4);

union OldMainNetBuffer {
    OldMainNetSetup setup;
    char bytes[OLD_MAIN_NET_BUFFER_SIZE];
};
SIZE(OldMainNetBuffer, OLD_MAIN_NET_BUFFER_SIZE);

#pragma pack(push, 1)
struct KbRemotePacket {
    i8 sender;
    char reserved1[OLD_MAIN_REMOTE_PREFIX_RESERVED_SIZE];
    i8 group;
    i8 command;
    char reserved2[OLD_MAIN_REMOTE_BODY_RESERVED_SIZE];
    union {
        OldMainNetSetup setup;
        struct {
            i32 saveId;
            i32 saveOffset;
            i32 saveSize;
        } save;
        char data[OLD_MAIN_REMOTE_PAYLOAD_HEAD_SIZE];
    } payload;
};
#pragma pack(pop)
SIZE(KbRemotePacket, 0xdd);

H2_ENUM_BEGIN(AppMenuConstant)
    APP_MENU_CHECKED             = 8,
    APP_MENU_UNCHECKED           = 0,
    APP_MENU_CONFIRM_DIALOG      = 2,
    APP_MENU_CONFIRM_OK          = 0x7805,
    APP_MENU_REVEAL_SIZE         = 0x1e,
    APP_MENU_REVEAL_RADIUS       = 0xb4,
    APP_MENU_MAX_SPELLS          = 0x41,
    APP_MENU_SPELL_COUNT         = 10,
    APP_MENU_RESOURCE_COUNT      = 7,
    APP_MENU_RESOURCE_BONUS      = 10,
    APP_MENU_GOLD_BONUS          = 1000,
    APP_MENU_MOVEMENT_BONUS      = 299999,
    APP_MENU_CLOSE_MESSAGE       = 0x10,
    APP_MENU_ARMY_FIRST          = 41000,
    APP_MENU_ARMY_LAST           = 41066,
    APP_MENU_SECONDARY_FIRST     = 42000,
    APP_MENU_SECONDARY_LAST      = 42056,
    APP_MENU_SECONDARY_LEVELS    = 4,
    APP_MENU_BUILDING_FIRST      = 43000,
    APP_MENU_BUILDING_LAST       = 43101,
    APP_MENU_COMBAT_FIRST        = 44000,
    APP_MENU_COMBAT_LAST         = 44200,
    APP_MENU_COMBAT_HEX_COUNT    = 117,
    APP_MENU_FORMATION_HEX_COUNT = 15
H2_ENUM_END(AppMenuConstant)

H2_ENUM_BEGIN(NetBoxConstant)
    NET_BOX_LINE_SIZE = 140
H2_ENUM_END(NetBoxConstant)

extern "C" void PollSound(void);
void ForcePollSound(void);
void InitMainClasses(void);
void DeleteMainClasses(void);
void EarlyShutdown(char* caption, char* text);
void SetupCDRom(void);
i32 EarlySetup(void);
i32 oldmain(void);
char toupper(char c);
i32 InterpretCommandLine(void);
i32 InitMenuHandler(struct tag_message&);
i32 NullHandler(struct tag_message& msg);
i32 RecruitHeroHandler(tag_message& msg);
char* GetBuildingInfo(i32 race, i32 building, i32 mode);
char* GetBuildingName(FactionType race, BuildingSlotType building);
void GetBuildingCost(FactionType race, BuildingSlotType building, i32* const dest, i32 mageLevel);
char* GetMonsterName(i32 m);
void GetMonsterCost(CreatureType monster, i32* const cost);
i32 CanBuild(town* t, BuildingSlotType building);
i32 CanBuy(town* t, BuildingSlotType type);
i32 GetBuildingBaseResourceValue(FactionType race, BuildingSlotType building, i32 level);
i32 WaitHandler(tag_message& msg);
i32 EventWindowHandler(struct tag_message&);
i32 TrueFalseDialogHandler(struct tag_message& msg);
void PlayerDead(i32 player);
void CheckEndGame(i32, i32);
void QuickViewWait(void);
void InitVars(void);
void ClearMapExtra(void);
i32 GetMonType(i32 score, i32 campaign);
i32 AddScoreToHighScore(i32, i32, i32, i32, char*);
void BVResMsg(char* s, i32 res, i32 qty);
void GOut(char* str);
i32 NetPosToGamePos(i32 netPos);
i32 WaitForOtherPlayer(void);
void PopNetBox(char*, i32);
void AddNetBoxLine(char* str, char color);
void ShutDown(char* msg);
void FileError(char* filename);
void SmackFade(u8* src, u8* dst);
void ShowCongrats(i32);
void CongratsWait(void);
SAMPLE2 LoadPlaySample(char* name);
void WaitEndSample(SAMPLE2 s, i32 waitTime);
void MemError(void);
char* GetTownName(i32 i);
void LoadSystemwideIcons(void);
void UnloadSystemwideIcons(void);
void EarlyShutDownSystem(void);
i32 GameUnsaved(void);
i32 HandleAppSpecificMenuCommands(i32);
void UpdateSystemOptionsMenu(void);
void CleanUpMenus(void);
void UpdateAppSpecificMenus(void* hMenu);
void EarlyResizeWindow(i32 x, i32 y, i32 w, i32 h);
i32 InMapArea(i32 x, i32 y);
void SetupDynamicWindow(
    i32,
    i32,
    i32,
    i32,
    i32,
    i32,
    i32,
    i32*,
    i32*,
    i32*,
    i32*,
    i32*,
    i32*,
    class heroWindow**,
    i32
);
void TestDynamicWindow(i32 p1, i32 p2);
void HandleRemoteDeadPlayerExit(i32 pos);
void HandleRemoteSuddenExit(void);
void DropDownToOnePlayer(void);
void ReceiveHostReportsPlayerExit(i32, struct SPlayerExit, i32);
void ReceiveRemotePlayerExit(struct SPlayerExit);
i32 CheckMem(void);
i32 GetManaCost(SpellType spell, hero* h);
void SetWinText(heroWindow* j, i32 id);
void CheckShingleUpdate(void);
void NormalDialog(char*, i32, i32, i32, i32, i32, i32, i32, i32, i32);
void UpdateNormalDialog(char* text);

extern i32 bDoColorCycle;
extern b32 gbDrawWindowBackground;
extern i32 bEarlySetupDone;
extern font* bigFont;
extern i32 bInShutDown;
extern i32 bShowIt;
extern i32 bSpecialHideCursor;
extern char* cBuildingInfoNeutral[];
extern char cNetBoxColor[];
extern char cNetBoxLine[][NET_BOX_LINE_SIZE];
extern char* cOutOfMemory;
extern char* gArmyNames[IDX(CREATURE_COUNT)];
extern char* gArmyNamesPlural[IDX(CREATURE_COUNT)];
extern char* cMonFilename[IDX(CREATURE_COUNT)];
extern char* cArmyFrameFileNames[IDX(CREATURE_COUNT)];
extern char* gArmyShortNames[IDX(CREATURE_COUNT)];
extern i32 gArtifactBaseRV[];
extern b32 gbAllBlack;
extern b32 gbCheatMenus;
extern b32 gbClosingApp;
extern b8 gbCombatSurrender;
extern i8 captainStats[IDX(FACTION_COUNT)][HERO_PRIMARY_STAT_COUNT];
extern b32 gbDrawSavedCursor;
extern b32 gbForegroundApp;
extern b32 gbFunctionComplete;
extern b32 gbGameInitialized;
extern b8 gbGamePosToNetPos[OLD_MAIN_MATCH_BUFFER_SIZE];
extern b32 gbHeroMoving;
extern b32 gbHumanPlayer[];
extern b32 gbLoadingMonoIcon;
extern u8 gColorTableYellow[];
extern u8 gColorTableScenWin[];
extern u8 gColorTableDarkGray[];
extern b32 gbInMemError;
extern b32 gbInNewGameSetup;
extern b32 gbInPollSound;
extern b32 gbNoCDRom;
extern b32 gbNoSound;
extern b32 gbPutzingWithMouseCtr;
extern b32 gbRemoteOn;
extern b8 gbRetreatWin;
extern b32 gbTextEntryEscaped;
extern b32 gbThisNetGotAdventureControl;
extern b8 gbThisNetHumanPlayer[];
extern u8 bStopOnTrigger[];
extern char* gBuildingInfoSpecial[];
extern icon* gBuyBuildIcons;
extern char gcBottomViewText[];
extern configStruct gConfig;
extern SMenuEnableStatus gsMenuEnableStatus[MENU_ENABLE_STATUS_COUNT];
extern i32 gDwellingBaseResourceValues[][KB_DWELLING_TYPE_COUNT];
extern i32 gDwellingCosts[][KB_DWELLING_TYPE_COUNT][KB_BUILDING_RESOURCE_COUNT];
extern char* gDwellingNames[][KB_DWELLING_TYPE_COUNT];
extern i8 gDwellingType[][KB_DWELLING_TYPE_COUNT];
extern i32 gGameCommand;
extern i32 gHeroGoldCost;
extern u32l gHierarchyMask[][KB_DWELLING_TYPE_COUNT];
extern i32 giBottomViewOverride;
extern i32 giBottomViewOverrideEndTime;
extern i32 giBottomViewResource;
extern i32 giBottomViewResourceQty;
extern WindowColorCycleMode giCycleType;
extern i32 giDebugLevel;
extern i32 giDialogTimeout;
extern u8 giGroundToTerrain[];
extern i32 giHighMemBuffer;
extern i32 giMainVideoModeColorDepth;
extern i32 giNumHumanPlayers;
extern i16 giScoreCampaignMon[][IDX(MONSTER_SCORE_FIELD_COUNT)];
extern i16 giScoreMon[][IDX(MONSTER_SCORE_FIELD_COUNT)];
extern i32 giTCPHostStatus;
extern i32 giThisGamePos;
extern i32 giThisNetPos;
extern i32 giTotalHighMem;
extern i32 giWaitType;
extern i32 glTimers[GLOBAL_TIMER_COUNT];
extern i32 gMageBaseResourceValues[];
extern i32 gMageBuildingCosts[][KB_BUILDING_RESOURCE_COUNT];
extern tag_monsterInfo gMonsterDatabase[IDX(CREATURE_COUNT)];
extern SCmbtHero sCmbtHero[KB_COMBAT_HERO_SPRITE_COUNT];
extern i32 gNeutralBaseResourceValues[];
extern i32 gNeutralBuildingCosts[][KB_BUILDING_RESOURCE_COUNT];
extern char* gNeutralBuildingNames[];
extern advManager* gpAdvManager;
extern palette* gPalette;
extern combatManager* gpCombatManager;
extern executive* gpExec;
extern game* gpGame;
extern inputManager* gpInputManager;
extern armyGroup* gpMonGroup;
extern class mouseManager* gpMouseManager;
extern philAI* gpPhilAI;
extern resourceManager* gpResourceManager;
extern searchArray* gpSearchArray;
extern soundManager* gpSoundManager;
extern townManager* gpTownManager;
extern class heroWindowManager* gpWindowManager;
extern i32 gResourceBaseValue[];
extern icon* gShingleAnim;
extern i32 gSpecialBuildingBaseResourceValues[];
extern i32 gSpecialBuildingCosts[][KB_BUILDING_RESOURCE_COUNT];
extern char* gSpecialBuildingNames[];
extern SSpellInfo gsSpellInfo[IDX(SPELL_COUNT)];
extern icon* gSystemIcons;
extern char gText[];
extern char* gWellExtraNames[];
extern SWinSetup gWinSetup[];
extern void* hmnuAdv;
extern void* hmnuCmbt;
extern void* hmnuDflt;
extern void* hmnuTown;
extern i32 iCDRomErr;
extern i32 iMaxMapExtra;
extern i32 iNextShingleAnim;
extern i32 iShingleAnimFrame;
extern i32 MAP_HEIGHT;
extern i32 MAP_WIDTH;
H2_ENUM_CLASS_BEGIN(MapExtraFlag)
    MAP_EXTRA_ADJACENT_MONSTER    = 0x80,
    MAP_EXTRA_ADJACENT_CLEAR_MASK = 0x7f
H2_ENUM_CLASS_END(MapExtraFlag)
extern u8* mapExtra;
extern tag_tilePoint normalDirTable[];
extern SAMPLE2 NULL_SAMPLE2;
inline SAMPLE2& GetNullSample(void) {
    return NULL_SAMPLE2;
}
extern heroWindow* pNormalDialogWindow;
extern void** ppMapExtra;
extern i16* pwSizeOfMapExtra;
extern font* smallFont;
extern u8 iGetSSByAlignment[IDX(HERO_SKILL_COUNT)][IDX(FACTION_COUNT)];

#endif
