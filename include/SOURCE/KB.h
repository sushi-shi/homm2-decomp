#ifndef HOMM2_KB_H
#define HOMM2_KB_H
#include <va.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/hero.h>
#include <SOURCE/REMOTE_TYPES.h>
// Declarations of the free functions DEFINED in KB.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
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

typedef enum GlobalTimerConstant {
    GLOBAL_TIMER_COUNT = 10,
    GLOBAL_BUTTON_REPEAT_TIMER_SLOT = 2,
    GLOBAL_MUSIC_FADE_TIMER_SLOT = 4,
    GLOBAL_POLL_SOUND_TIMER_SLOT = 5,
    GLOBAL_MOUSE_TIMER_SLOT = 6,
    GLOBAL_COLOR_CYCLE_TIMER_SLOT = 7,
    GLOBAL_COMBAT_CYCLE_TIMER_SLOT = 8
} GlobalTimerConstant;

typedef enum CombatHeroTableConstant {
    KB_COMBAT_HERO_VARIANT_COUNT = 2,
    KB_COMBAT_HERO_SPRITE_COUNT = FACTION_COUNT * KB_COMBAT_HERO_VARIANT_COUNT
} CombatHeroTableConstant;

typedef enum CampaignSide {
    CAMPAIGN_ROLAND = 0,
    CAMPAIGN_ARCHIBALD = 1,
    CAMPAIGN_SIDE_COUNT = 2
} CampaignSide;

typedef enum MonsterScoreField {
    MONSTER_SCORE_THRESHOLD = 0,
    MONSTER_SCORE_TYPE = 1,
    MONSTER_SCORE_FIELD_COUNT = 2
} MonsterScoreField;

typedef enum CampaignChoiceType {
    CAMPAIGN_CHOICE_RESOURCE = 0,
    CAMPAIGN_CHOICE_ARTIFACT = 1,
    CAMPAIGN_CHOICE_SPELL = 2,
    CAMPAIGN_CHOICE_SECONDARY_SKILL = 3,
    CAMPAIGN_CHOICE_CREATURES = 4,
    CAMPAIGN_CHOICE_PUZZLE_PIECES = 5,
    CAMPAIGN_CHOICE_EXPERIENCE = 6,
    CAMPAIGN_CHOICE_NONE = 7,
    CAMPAIGN_CHOICE_ALIGNMENT = 8,
    CAMPAIGN_CHOICE_PRIMARY_SKILL = 9,
    CAMPAIGN_CHOICE_SPELL_SCROLL = 10,
    CAMPAIGN_CHOICE_INVALID = 255
} CampaignChoiceType;

typedef enum CampaignConstant {
    CAMPAIGN_MAP_COUNT = 12,
    CAMPAIGN_REGULAR_MAP_COUNT = 11,
    CAMPAIGN_TRACK_POINT_COUNT = 13,
    CAMPAIGN_BONUS_CHOICE_COUNT = 3,
    CAMPAIGN_AWARD_COUNT = 12,
    CAMPAIGN_SWITCHING_MAP = 11,
    CAMPAIGN_SWITCHING_SCENARIO = 4,
    CAMPAIGN_NO_SCENARIO = -1,
    CAMPAIGN_ROLAND_FINAL_SCENARIO = 9,
    CAMPAIGN_ARCHIBALD_FINAL_SCENARIO = 10,
    CAMPAIGN_STATE_RESET_SIZE = 0x147,
    CAMPAIGN_SETUP_RESET_SIZE = 0x41,
    CAMPAIGN_ARMY_NAME_BUFFER_SIZE = 52,
    CAMPAIGN_ARMY_SLOT_COUNT = 5,
    CAMPAIGN_CARRYOVER_PLAYER = 3,
    CAMPAIGN_TRIPLE_ARMY_MULTIPLIER = 3,
    CAMPAIGN_EASY_SCENARIO_LIMIT = 2,
    CAMPAIGN_NORMAL_SCENARIO_LIMIT = 5,
    CAMPAIGN_HERO_COUNT = 54,
    CAMPAIGN_HERO_PRIORITY_HIGH = 100,
    CAMPAIGN_HERO_PRIORITY_NORMAL = 90,
    CAMPAIGN_EXPERIENCE_BONUS = 5000,
    CAMPAIGN_SWITCH_VICTORY_VALUE = 99,
    CAMPAIGN_ROLAND_TIME_LIMIT = 90
} CampaignConstant;

typedef enum CampaignChoiceValue {
    CAMPAIGN_CHOICE_VALUE_NONE = -1
} CampaignChoiceValue;

typedef enum CampaignChoiceAmount {
    CAMPAIGN_CHOICE_NO_AMOUNT = -1,
    CAMPAIGN_CHOICE_BASIC_SKILL = 1,
    CAMPAIGN_CHOICE_RESOURCE_BONUS = 20,
    CAMPAIGN_CHOICE_GOLD_BONUS = 2000
} CampaignChoiceAmount;

#pragma pack(push, 1)
struct SCampaignChoice {
    u8 type;
    i16 value;
    i16 amount;
};
#pragma pack(pop)
SIZE(SCampaignChoice, 5);

extern SCampaignChoice campaignChoices[CAMPAIGN_SIDE_COUNT][CAMPAIGN_MAP_COUNT]
                                      [CAMPAIGN_BONUS_CHOICE_COUNT];
extern char* cCampaignName[CAMPAIGN_SIDE_COUNT][CAMPAIGN_MAP_COUNT];
extern char* cCampaignDescription[CAMPAIGN_SIDE_COUNT][CAMPAIGN_MAP_COUNT];

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

typedef enum PlayerExitConstant {
    PLAYER_EXIT_DIALOG_INFO = 1,
    PLAYER_EXIT_DIALOG_CONFIRM = 2,
    PLAYER_EXIT_CONFIRM_OK = 0x7805,
    PLAYER_EXIT_NETWORK_SLOTS = 6,
    PLAYER_EXIT_SHIFT_SLOTS = 5,
    PLAYER_EXIT_MESSAGE_LENGTH = 500,
    PLAYER_EXIT_MESSAGE_TIME = 5000,
    PLAYER_EXIT_HEARTBEAT_DISABLED = 0x0bebc1ff,
    PLAYER_EXIT_PACKET_TYPE = 7,
    PLAYER_EXIT_PACKET_COMMAND = 0x21
} PlayerExitConstant;

typedef enum EventWindowConstant {
    EVENT_WINDOW_RESOURCE_FLAG = 0x200,
    EVENT_WINDOW_CLOSE_COMMAND = 10,
    EVENT_WINDOW_FIRST_RESOURCE_WIDGET = 0x1e14,
    EVENT_WINDOW_SECOND_RESOURCE_WIDGET = 0x1e15,
    EVENT_WINDOW_FIRST_BUTTON = 0x7800,
    EVENT_WINDOW_SECOND_BUTTON = 0x7801,
    EVENT_WINDOW_THIRD_BUTTON = 0x7802,
    EVENT_WINDOW_FOURTH_BUTTON = 0x7803,
    EVENT_WINDOW_IGNORED_BUTTON = 0x7804,
    EVENT_WINDOW_FIFTH_BUTTON = 0x7805,
    EVENT_WINDOW_SIXTH_BUTTON = 0x7806,
    EVENT_WINDOW_SEVENTH_BUTTON = 0x7807,
    EVENT_WINDOW_EIGHTH_BUTTON = 0x7808,
    EVENT_WINDOW_CONTINUE = 1,
    EVENT_WINDOW_CLOSE = 2,
    EVENT_WINDOW_LUCK = 10,
    EVENT_WINDOW_BAD_LUCK = 11,
    EVENT_WINDOW_MORALE = 12,
    EVENT_WINDOW_BAD_MORALE = 13,
    EVENT_WINDOW_EXPERIENCE = 14
} EventWindowConstant;

typedef enum PlayerDeadConstant {
    PLAYER_DEAD_MAP_CHANGE = 0xa,
    PLAYER_DEAD_MAP_CHANGE_UNUSED = -999
} PlayerDeadConstant;

typedef enum KbBuildingConstant {
    KB_BUILDING_NEUTRAL_LIMIT = 16,
    KB_BUILDING_RESOURCE_COUNT = 7,
    KB_MAGE_GUILD_MAX_LEVEL = 5,
    KB_MAGE_GUILD_LEVEL_COUNT = KB_MAGE_GUILD_MAX_LEVEL + 1,
    KB_DWELLING_TYPE_COUNT = BUILDING_SLOT_DWELLING_LAST - BUILDING_SLOT_DWELLING_FIRST + 1
} KbBuildingConstant;

typedef enum KbDwellingFlag {
    KB_DWELLING_FIRST_FLAG = 0x00100000,
    KB_DWELLING_SECOND_FLAG = 0x00200000,
    KB_DWELLING_THIRD_FLAG = 0x00400000,
    KB_DWELLING_FOURTH_FLAG = 0x00800000,
    KB_DWELLING_FIFTH_FLAG = 0x01000000,
    KB_DWELLING_UPGRADE_FIRST_FLAG = 0x02000000,
    KB_DWELLING_UPGRADE_SECOND_FLAG = 0x04000000,
    KB_DWELLING_UPGRADE_THIRD_FLAG = 0x08000000,
    KB_DWELLING_UPGRADE_FOURTH_FLAG = 0x10000000,
    KB_DWELLING_UPGRADE_FIFTH_FLAG = 0x20000000,
    KB_DWELLING_UPGRADE_SIXTH_FLAG = 0x40000000
} KbDwellingFlag;

typedef enum KbMonsterResourceType {
    KB_MONSTER_NEEDS_CRYSTAL = 19,
    KB_MONSTER_NEEDS_MERCURY = 28,
    KB_MONSTER_NEEDS_SULFUR_FIRST = 35,
    KB_MONSTER_NEEDS_SULFUR_SECOND = 36,
    KB_MONSTER_NEEDS_TWO_SULFUR = 37,
    KB_MONSTER_NEEDS_GEMS = 45,
    KB_MONSTER_NEEDS_TWO_GEMS = 46,
    KB_MONSTER_NEEDS_GEMS_EXPANSION = 60
} KbMonsterResourceType;

typedef enum NormalDialogResourceType {
    NORMAL_DIALOG_NO_RESOURCE = -1,
    NORMAL_DIALOG_RESOURCE_FIRST = 0,
    NORMAL_DIALOG_RESOURCE_LAST = 6,
    NORMAL_DIALOG_ARTIFACT = 7,
    NORMAL_DIALOG_SPELL = 8,
    NORMAL_DIALOG_CREST = 9,
    NORMAL_DIALOG_EXPMRL_FIRST = 10,
    NORMAL_DIALOG_EXPMRL_LAST = 14,
    NORMAL_DIALOG_HERO = 15,
    NORMAL_DIALOG_SECONDARY_SKILL = 17,
    NORMAL_DIALOG_MONSTER = 18,
    NORMAL_DIALOG_PRIMARY_SKILL = 25
} NormalDialogResourceType;

typedef enum NormalDialogConstant {
    NORMAL_DIALOG_INFO = 1,
    NORMAL_DIALOG_CONFIRM = 2,
    NORMAL_DIALOG_BUTTON_PAIR = 3,
    NORMAL_DIALOG_QUICK_VIEW = 4,
    NORMAL_DIALOG_WAIT_FIRST = 5,
    NORMAL_DIALOG_WAIT_LAST = 6,
    NORMAL_DIALOG_DISABLE_SEVENTH = 7,
    NORMAL_DIALOG_DISABLE_EIGHTH = 8,
    NORMAL_DIALOG_RESOURCE_COUNT = 2,
    NORMAL_DIALOG_PRIMARY_BONUS_OFFSET = 100,
    NORMAL_DIALOG_DAILY_RESOURCE_OFFSET = 100000,
    NORMAL_DIALOG_SHOW_OR_TEXT = 1,
    NORMAL_DIALOG_NO_VALUE = -1,
    NORMAL_DIALOG_TEXT_LENGTH = 80,
    NORMAL_DIALOG_FILENAME_LENGTH = 16,
    NORMAL_DIALOG_WINDOW_WIDTH = 306,
    NORMAL_DIALOG_WINDOW_BASE_HEIGHT = 180,
    NORMAL_DIALOG_WINDOW_ROW_HEIGHT = 45,
    NORMAL_DIALOG_MAX_ROWS = 6,
    NORMAL_DIALOG_TEXT_LINE_WIDTH = 244,
    NORMAL_DIALOG_TEXT_LINE_HEIGHT = 16,
    NORMAL_DIALOG_SCREEN_RIGHT = 639,
    NORMAL_DIALOG_SCREEN_BOTTOM = 479,
    NORMAL_DIALOG_SCREEN_HEIGHT = 480,
    NORMAL_DIALOG_MAX_TOP = 28,
    NORMAL_DIALOG_WIDGET_COLOR = 0x10,
    NORMAL_DIALOG_TEXT_WIDGET_FIRST_ID = 100,
    NORMAL_DIALOG_RESOURCE_BORDER_FIRST_ID = 0x1e14,
    NORMAL_DIALOG_TIMEOUT_MIN = 1,
    NORMAL_DIALOG_TIMEOUT_MAX = 20000,
    NORMAL_DIALOG_DISABLE_MESSAGE = 0x200,
    NORMAL_DIALOG_DISABLE_COMMAND = 6,
    NORMAL_DIALOG_SET_TEXT_COMMAND = 3,
    NORMAL_DIALOG_TEXT_WIDGET_ID = 1,
    NORMAL_DIALOG_BUTTON_ONE = 0x7801,
    NORMAL_DIALOG_BUTTON_TWO = 0x7802,
    NORMAL_DIALOG_BUTTON_FIVE = 0x7805,
    NORMAL_DIALOG_BUTTON_SIX = 0x7806,
    NORMAL_DIALOG_BUTTON_SEVEN = 0x7807,
    NORMAL_DIALOG_BUTTON_EIGHT = 0x7808,
    NORMAL_DIALOG_WIDGET_FLAGS = 0x200,
    NORMAL_DIALOG_POINTER_ID = -999
} NormalDialogConstant;

typedef enum CheckEndGameConstants {
    CHECK_END_GAME_NO_PLAYER = -1,
    CHECK_END_GAME_EMPTY_ARMY = -1,
    CHECK_END_GAME_PLAYER_COUNT = 6,
    CHECK_END_GAME_HERO_COUNT = 54,
    CHECK_END_GAME_ARMY_SLOTS = 5,
    CHECK_END_GAME_GOLD_RESOURCE = 6,
    CHECK_END_GAME_GRACE_DAYS = 7,
    CHECK_END_GAME_DAYS_PER_WEEK = 7,
    CHECK_END_GAME_DAYS_PER_MONTH = 28,
    CHECK_END_GAME_TEXT_BUFFER_SIZE = 100,
    CHECK_END_GAME_GOLD_SCALE = 1000,
    CHECK_END_GAME_ULTIMATE_ARTIFACT = 0,
    CHECK_END_GAME_LAST_ULTIMATE_PART = 7,
    CHECK_END_GAME_SIDE_SPECIAL_VALUE = 99,
    CHECK_END_GAME_PLAYER_DIALOG_ICON = 9,
    CHECK_END_GAME_REMOTE_DIALOG_TIME = 5000,
    CHECK_END_GAME_DWARF_TOWN = 2,
    CHECK_END_GAME_ROLAND_HERO = 54
} CheckEndGameConstants;

typedef enum CheckEndGameCampaignConstants {
    CHECK_END_GAME_ROLAND_CAMPAIGN = 0,
    CHECK_END_GAME_ARCHIBALD_CAMPAIGN = 1,
    CHECK_END_GAME_SCENARIO_OFFSET = 1,
    CHECK_END_GAME_DWARF_SCENARIO = 3,
    CHECK_END_GAME_SIDE_SCENARIO = 7,
    CHECK_END_GAME_ROLAND_CAPTURE_SCENARIO = 9,
    CHECK_END_GAME_FIRST_NO_SAVE_SCENARIO = 10,
    CHECK_END_GAME_LAST_SCENARIO = 11,
    CHECK_END_GAME_ROLAND_COLOR = 0,
    CHECK_END_GAME_ALLY_COLOR = 3
} CheckEndGameCampaignConstants;

typedef enum CheckEndGameForcedResult {
    CHECK_END_GAME_FORCE_NONE = 0,
    CHECK_END_GAME_FORCE_VICTORY = 1,
    CHECK_END_GAME_FORCE_DEFEAT = 2
} CheckEndGameForcedResult;

#pragma pack(push, 1)
struct HighScoreEntry {
    char playerName[17];
    char scenarioName[41];
    i32 score;
    i32 days;
    i32 scenario;
    char cheated;
    char reserved[29];
};
#pragma pack(pop)
SIZE(HighScoreEntry, 100);

typedef enum HighScoreConstant {
    HIGH_SCORE_ENTRY_COUNT = 10,
    HIGH_SCORE_NAME_LENGTH = 16,
    HIGH_SCORE_STANDARD = 1,
    HIGH_SCORE_CAMPAIGN = 0,
    HIGH_SCORE_EXPANSION_CAMPAIGN = 2,
    HIGH_SCORE_EMPTY = -1,
    HIGH_SCORE_FILE_READ_FLAGS = 0x8000,
    HIGH_SCORE_FILE_WRITE_FLAGS = 0x8301,
    HIGH_SCORE_FILE_PERMISSIONS = 0x80
} HighScoreConstant;

typedef enum MoraleInfoTextIndex {
    MORALE_INFO_GOOD = 0,
    MORALE_INFO_NEUTRAL = 1,
    MORALE_INFO_BAD = 2,
    MORALE_INFO_HEADER = 3,
    MORALE_INFO_SAME_ALIGNMENT = 5,
    MORALE_INFO_THREE_ALIGNMENTS = 6,
    MORALE_INFO_FOUR_ALIGNMENTS = 7,
    MORALE_INFO_MEDAL_OF_VALOR = 8,
    MORALE_INFO_MEDAL_OF_COURAGE = 9,
    MORALE_INFO_MEDAL_OF_HONOR = 10,
    MORALE_INFO_MEDAL_OF_DISTINCTION = 11,
    MORALE_INFO_FIZBIN = 12,
    MORALE_INFO_BUOY = 13,
    MORALE_INFO_OASIS = 14,
    MORALE_INFO_TEMPLE = 15,
    MORALE_INFO_GRAVEYARD = 16,
    MORALE_INFO_SHIPWRECK = 17,
    MORALE_INFO_NONE = 19,
    MORALE_INFO_FIVE_ALIGNMENTS = 20,
    MORALE_INFO_ALL_UNDEAD = 21,
    MORALE_INFO_SOME_UNDEAD = 22,
    MORALE_INFO_WATERING_HOLE = 23,
    MORALE_INFO_DERELICT_SHIP = 24,
    MORALE_INFO_COLISEUM = 25,
    MORALE_INFO_TAVERN = 26,
    MORALE_INFO_BASIC_LEADERSHIP = 27,
    MORALE_INFO_ADVANCED_LEADERSHIP = 28,
    MORALE_INFO_EXPERT_LEADERSHIP = 29,
    MORALE_INFO_MASTHEAD = 30,
    MORALE_INFO_BATTLE_GARB = 31
} MoraleInfoTextIndex;

typedef enum LuckInfoTextIndex {
    LUCK_INFO_GOOD = 0,
    LUCK_INFO_NEUTRAL = 1,
    LUCK_INFO_BAD = 2,
    LUCK_INFO_HEADER = 3,
    LUCK_INFO_RABBIT_FOOT = 4,
    LUCK_INFO_HORSESHOE = 5,
    LUCK_INFO_LUCKY_COIN = 6,
    LUCK_INFO_CLOVER = 7,
    LUCK_INFO_FAERIE_RING = 8,
    LUCK_INFO_FOUNTAIN = 9,
    LUCK_INFO_NONE = 10,
    LUCK_INFO_RAINBOW = 12,
    LUCK_INFO_IDOL = 13,
    LUCK_INFO_PYRAMID = 14,
    LUCK_INFO_BASIC_SKILL = 15,
    LUCK_INFO_ADVANCED_SKILL = 16,
    LUCK_INFO_EXPERT_SKILL = 17,
    LUCK_INFO_MASTHEAD = 18,
    LUCK_INFO_MERMAID = 19,
    LUCK_INFO_BATTLE_GARB = 20
} LuckInfoTextIndex;

typedef enum NetBoxConstant {
    NET_BOX_WINDOW_Y = 0x19b,
    NET_BOX_HEIGHT = 0x44,
    NET_BOX_WIDTH = 0x27f,
    NET_BOX_INPUT_Y = 0x1d1,
    NET_BOX_INPUT_HEIGHT = 0xc,
    NET_BOX_TEXT_LENGTH = 150,
    NET_BOX_LINE_COUNT = 4,
    NET_BOX_LINE_TEXT_LIMIT = 120,
    NET_BOX_TEXT_X = 20,
    NET_BOX_TEXT_Y = 54,
    NET_BOX_MAX_INPUT = 0x5d,
    NET_BOX_DEFAULT_COLOR = 6,
    NET_BOX_CURSOR_DELAY = 0x168,
    NET_BOX_CURSOR_WIDTH_PADDING = 0x32,
    NET_BOX_CURSOR_WIDTH_LIMIT = 0x25a,
    NET_BOX_MESSAGE_TIMEOUT = 6000,
    NET_BOX_EXIT_DELAY_STEPS = 20,
    NET_BOX_EXIT_DELAY = 75,
    NET_BOX_UPDATE_MESSAGE = 0x200,
    NET_BOX_TEXT_COMMAND = 3,
    NET_BOX_COLOR_COMMAND = 4,
    NET_BOX_FIRST_LINE_ID = 1,
    NET_BOX_FIRST_COLOR_ID = 0x14,
    NET_BOX_SECOND_COLOR_ID = 0x15,
    NET_BOX_THIRD_COLOR_ID = 0x16,
    NET_BOX_FOURTH_COLOR_ID = 0x17,
    NET_BOX_INPUT_ID = 5,
    NET_BOX_THIS_PLAYER_COLOR_ID = 0x18,
    NET_BOX_KEY_ESCAPE = 0x1b,
    NET_BOX_KEY_ENTER = 10,
    NET_BOX_KEY_BACKSPACE = 0x7f,
    NET_BOX_KEY_F1 = 0x3b00,
    NET_BOX_CURSOR_GLYPH = 0x1f,
    NET_BOX_PACKET_BUFFER_SIZE = 0x7f,
    NET_BOX_FIRST_PRINTABLE = 0x20,
    NET_BOX_LAST_PRINTABLE = 0x7f,
    NET_BOX_REMOTE_MAP_CHANGE = 0x29,
    NET_BOX_REMOTE_SETUP = 0x20,
    NET_BOX_REMOTE_SAVE = 1,
    NET_BOX_REMOTE_CHAT = 0xb,
    NET_BOX_REMOTE_GROUP = 3,
    NET_BOX_REMOTE_CONTROL = 2
} NetBoxConstant;

typedef enum CongratsConstant {
    CONGRATS_PALETTE_SIZE = 0x300,
    CONGRATS_PALETTE_BUFFER_SIZE = 0x304,
    CONGRATS_TEXT_SIZE = 500,
    CONGRATS_RATING_LENGTH = 32,
    CONGRATS_CAMPAIGN = 0,
    CONGRATS_STANDARD = 1,
    CONGRATS_EXPANSION_CAMPAIGN = 2,
    CONGRATS_MUSIC_SILENT = -1,
    CONGRATS_SMACKER = 2,
    CONGRATS_DIFFICULTY_SCALE = 100
} CongratsConstant;

typedef enum KbSourceLine {
} KbSourceLine;

typedef enum AppMenuCommand {
    APP_MENU_NONE = 0,
    APP_MENU_VIEW_WORLD = 0x9c4c,
    APP_MENU_VIEW_PUZZLE = 0x9c4d,
    APP_MENU_CAST_SPELL = 0x9c4e,
    APP_MENU_SEARCH = 0x9c4f,
    APP_MENU_MUSIC_FIRST = 0x9c50,
    APP_MENU_MUSIC_LAST = 0x9c5a,
    APP_MENU_SOUND_FIRST = 0x9c5c,
    APP_MENU_SOUND_LAST = 0x9c66,
    APP_MENU_SPEED_FIRST = 0x9c68,
    APP_MENU_SPEED_LAST = 0x9c6c,
    APP_MENU_UNKNOWN_9C6D = 0x9c6d,
    APP_MENU_TOGGLE_ROUTE = 0x9c6e,
    APP_MENU_TOGGLE_BLACKOUT = 0x9c6f,
    APP_MENU_RESTART_0 = 0x9ca6,
    APP_MENU_RESTART_1 = 0x9ca8,
    APP_MENU_RESTART_2 = 0x9ca9,
    APP_MENU_RESTART_3 = 0x9caa,
    APP_MENU_RESTART_4 = 0x9cab,
    APP_MENU_UNKNOWN_9CAD = 0x9cad,
    APP_MENU_RESTART_5 = 0x9cae,
    APP_MENU_RESTART_6 = 0x9caf,
    APP_MENU_RESTART_7 = 0x9cb0,
    APP_MENU_RESTART_8 = 0x9cb2,
    APP_MENU_RESTART_9 = 0x9cb3,
    APP_MENU_RESTART_10 = 0x9cb5,
    APP_MENU_RESTART_11 = 0x9cb6,
    APP_MENU_RESTART_12 = 0x9cb8,
    APP_MENU_RESTART_13 = 0x9cb9,
    APP_MENU_LOAD_0 = 0x9cbb,
    APP_MENU_LOAD_1 = 0x9cbc,
    APP_MENU_LOAD_2 = 0x9cbf,
    APP_MENU_LOAD_3 = 0x9cc0,
    APP_MENU_LOAD_4 = 0x9cc1,
    APP_MENU_LOAD_5 = 0x9cc3,
    APP_MENU_LOAD_6 = 0x9cc4,
    APP_MENU_LOAD_7 = 0x9cc6,
    APP_MENU_LOAD_8 = 0x9cc7,
    APP_MENU_LOAD_9 = 0x9cc9,
    APP_MENU_LOAD_10 = 0x9cca,
    APP_MENU_SAVE = 0x9ccb,
    APP_MENU_EXIT = 0x9ccc,
    APP_MENU_CHEAT_REVEAL = 0x9ccd,
    APP_MENU_CHEAT_MOVEMENT = 0x9cce,
    APP_MENU_CHEAT_SPELLS = 0x9ccf,
    APP_MENU_CHEAT_RESOURCES = 0x9cd0
} AppMenuCommand;

typedef enum OldMainConstant {
    OLD_MAIN_PLAYER_COUNT = 6,
    OLD_MAIN_MATCH_BUFFER_SIZE = 8,
    OLD_MAIN_PLAYER_NAME_LENGTH = 21,
    OLD_MAIN_DEFAULT_NAME_LENGTH = 3,
    OLD_MAIN_DEFAULT_NAME_STRIDE = 4,
    OLD_MAIN_SCREEN_WIDTH = 640,
    OLD_MAIN_SCREEN_HEIGHT = 480,
    OLD_MAIN_MAIN_MUSIC = 42,
    OLD_MAIN_HIGH_SCORE_MUSIC = 43,
    OLD_MAIN_FADE_SPEED = 8,
    OLD_MAIN_LONG_FADE_SPEED = 0x80,
    OLD_MAIN_DEBUG_MEMORY_CHECK_LEVEL = 9,
    OLD_MAIN_INTRO_PRIMARY_VIDEO = 0x42,
    OLD_MAIN_INTRO_FALLBACK_VIDEO = 1,
    OLD_MAIN_INTRO_SECONDARY_VIDEO = 0x41,
    OLD_MAIN_CREDITS_FIRST_VIDEO = 0x48,
    OLD_MAIN_CREDITS_SECOND_VIDEO = 0x24,
    OLD_MAIN_STANDARD_VICTORY_VIDEO = 3,
    OLD_MAIN_EXPANSION_VICTORY_VIDEO = 0x40,
    OLD_MAIN_NEW_GAME = 0x65,
    OLD_MAIN_LOAD_GAME = 0x66,
    OLD_MAIN_HIGH_SCORES = 0x67,
    OLD_MAIN_CREDITS = 0x68,
    OLD_MAIN_EXIT = 0x69,
    OLD_MAIN_SETUP_NEW = 0,
    OLD_MAIN_SETUP_LOAD = 1,
    OLD_MAIN_NETWORK_PROTOCOL = 2,
    OLD_MAIN_REMOTE_HOST = 1,
    OLD_MAIN_REMOTE_CLIENT = 2,
    OLD_MAIN_REGULAR_COMPRESSION_MEMORY_LIMIT = 5999,
    OLD_MAIN_NET_BUFFER_SIZE = 256,
    OLD_MAIN_NETWORK_PACKET = 0x20,
    OLD_MAIN_ARCHIBALD_FINAL_SCENARIO = 10,
    OLD_MAIN_ROLAND_FINAL_SCENARIO = 9,
    OLD_MAIN_ROLAND_CAMPAIGN = 0,
    OLD_MAIN_DIALOG_WAIT = 6,
    OLD_MAIN_WAIT_DIRECT_CONNECT = 7,
    OLD_MAIN_WAIT_DP_FIRST_GUEST = 8,
    OLD_MAIN_WAIT_DP_EXTRA_GUESTS = 9,
    OLD_MAIN_WAIT_DP_HOST = 0xa,
    OLD_MAIN_WAIT_WS_FIRST_GUEST = 0xb,
    OLD_MAIN_WAIT_WS_EXTRA_GUESTS = 0xc,
    OLD_MAIN_WAIT_WS_HOST = 0xd,
    OLD_MAIN_POINTER_TYPE = -999
} OldMainConstant;

typedef enum CommandLineConstant {
    COMMAND_LINE_TCP_TEXT_LENGTH = 20,
    COMMAND_LINE_HELP_LINE_COUNT = 14,
    COMMAND_LINE_HUMAN_PLAYER_SLOTS = 4,
    COMMAND_LINE_TCP_MIN_PLAYERS = 2,
    COMMAND_LINE_TCP_MAX_PLAYERS = 6,
    COMMAND_LINE_FRAME_STEP = 6,
    COMMAND_LINE_TCP_TYPE_DEFAULT = 0,
    COMMAND_LINE_TCP_TYPE_L = 1,
    COMMAND_LINE_TCP_CLIENT = 0,
    COMMAND_LINE_TCP_HOST = 1,
    COMMAND_LINE_SINGLE_PLAYER = 1
} CommandLineConstant;

typedef enum InitMenuConstant {
    INIT_MENU_HOTSPOT_COUNT = 5,
    INIT_MENU_FIRST_COMMAND = 0x65,
    INIT_MENU_NEW_GAME = 0x65,
    INIT_MENU_LOAD_GAME = 0x66,
    INIT_MENU_HIGH_SCORES = 0x67,
    INIT_MENU_CREDITS = 0x68,
    INIT_MENU_EXIT = 0x69,
    INIT_MENU_LAST_ACTION = 0x6b,
    INIT_MENU_MOVIE = 0x6b,
    INIT_MENU_FIRST_WIDGET = 11,
    INIT_MENU_LAST_WIDGET = 15,
    INIT_MENU_WIDGET_OFFSET = 11,
    INIT_MENU_KEY_EXIT = 0x10,
    INIT_MENU_KEY_HIGH_SCORES = 0x23,
    INIT_MENU_KEY_LOAD = 0x26,
    INIT_MENU_KEY_CREDITS = 0x2e,
    INIT_MENU_KEY_NEW = 0x31,
    INIT_MENU_MESSAGE = 0x200,
    INIT_MENU_MOUSE_MOVE = 4,
    INIT_MENU_KEY_PRESS = 1,
    INIT_MENU_HOVER_COMMAND = 0xc,
    INIT_MENU_CLICK_COMMAND = 0xd,
    INIT_MENU_HELP_COMMAND = 0xe,
    INIT_MENU_SET_WIDGET_COMMAND = 4,
    INIT_MENU_DISABLE_MASK = 0x200,
    INIT_MENU_CLOSE_COMMAND = 10,
    INIT_MENU_HANDLER_CLOSE = 2,
    INIT_MENU_HANDLER_CONTINUE = 1,
    INIT_MENU_HANDLER_IGNORE = 0,
    INIT_MENU_HELP_DIALOG = 4,
    INIT_MENU_MOVIE_SMACKER = 0x26,
    INIT_MENU_MAIN_MUSIC = 0x2a,
    INIT_MENU_SCREEN_WIDTH = 640,
    INIT_MENU_SCREEN_HEIGHT = 480,
    INIT_MENU_FRAME_STRIDE = 4,
    INIT_MENU_HOVER_FRAME = 3,
    INIT_MENU_IDLE_FRAME = 1,
    INIT_MENU_ACTIVE_FRAME = 2,
    INIT_MENU_WIDGET_FRAME_BASE = 44,
    INIT_MENU_REDRAW_LEFT = 0,
    INIT_MENU_REDRAW_TOP = 105,
    INIT_MENU_REDRAW_WIDTH = 565,
    INIT_MENU_REDRAW_HEIGHT = 375
} InitMenuConstant;

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
    char reserved1[4];
    i8 group;
    i8 command;
    char reserved2[2];
    union {
        OldMainNetSetup setup;
        struct {
            i32 saveId;
            i32 saveOffset;
            i32 saveSize;
        } save;
        char data[1];
    } payload;
};
#pragma pack(pop)
SIZE(KbRemotePacket, 0xdd);

typedef enum AppMenuConstant {
    APP_MENU_CHECKED = 8,
    APP_MENU_UNCHECKED = 0,
    APP_MENU_CONFIRM_DIALOG = 2,
    APP_MENU_CONFIRM_OK = 0x7805,
    APP_MENU_REVEAL_SIZE = 0x1e,
    APP_MENU_REVEAL_RADIUS = 0xb4,
    APP_MENU_MAX_SPELLS = 0x41,
    APP_MENU_SPELL_COUNT = 10,
    APP_MENU_RESOURCE_COUNT = 7,
    APP_MENU_RESOURCE_BONUS = 10,
    APP_MENU_GOLD_BONUS = 1000,
    APP_MENU_MOVEMENT_BONUS = 299999,
    APP_MENU_CLOSE_MESSAGE = 0x10,
    APP_MENU_ARMY_FIRST = 41000,
    APP_MENU_ARMY_LAST = 41066,
    APP_MENU_SECONDARY_FIRST = 42000,
    APP_MENU_SECONDARY_LAST = 42056,
    APP_MENU_SECONDARY_LEVELS = 4,
    APP_MENU_BUILDING_FIRST = 43000,
    APP_MENU_BUILDING_LAST = 43101,
    APP_MENU_COMBAT_FIRST = 44000,
    APP_MENU_COMBAT_LAST = 44200,
    APP_MENU_COMBAT_HEX_COUNT = 117,
    APP_MENU_FORMATION_HEX_COUNT = 15
} AppMenuConstant;

typedef enum DynamicWindowConstant {
    DYNAMIC_TILE_SIZE = 48,
    DYNAMIC_WINDOW_PADDING = 48,
    DYNAMIC_CONTENT_LEFT = 32,
    DYNAMIC_CONTENT_TOP = 16,
    DYNAMIC_CORNER_SIZE = 72,
    DYNAMIC_CORNER_LEFT = 40,
    DYNAMIC_CORNER_RIGHT = 23,
    DYNAMIC_EDGE_OFFSET = 24,
    DYNAMIC_WINDOW_FLAGS = 0x4002,
    DYNAMIC_BACKGROUND_FRAME = 12,
    DYNAMIC_TOP_FRAME_FIRST = 4,
    DYNAMIC_TOP_FRAME_LAST = 5,
    DYNAMIC_RIGHT_FRAME_FIRST = 6,
    DYNAMIC_RIGHT_FRAME_LAST = 7,
    DYNAMIC_BOTTOM_FRAME_FIRST = 8,
    DYNAMIC_BOTTOM_FRAME_LAST = 9,
    DYNAMIC_LEFT_FRAME_FIRST = 10,
    DYNAMIC_LEFT_FRAME_LAST = 11,
    DYNAMIC_WIDGET_COLOR = 0x10
} DynamicWindowConstant;

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
char* GetBuildingName(i32 race, i32 building);
void GetBuildingCost(i32 race, i32 building, i32* const dest, i32 mageLevel);
char* GetMonsterName(i32 m);
void GetMonsterCost(i32 monster, i32* const cost);
i32 CanBuild(town* t, i32 building);
i32 CanBuy(town* t, i32 type);
i32 GetBuildingBaseResourceValue(i32 race, i32 building, i32 level);
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
i32 GetManaCost(i32 spell, hero* h);
void SetWinText(heroWindow* j, i32 id);
void CheckShingleUpdate(void);
void NormalDialog(char*, i32, i32, i32, i32, i32, i32, i32, i32, i32);
void UpdateNormalDialog(char* text);

// --- globals owned by this TU (moved from _globals.h; CodeView-attributed) ---
extern i32 bDoColorCycle;
extern i32 gbDrawWindowBackground;
extern i32 bEarlySetupDone;
extern font* bigFont;
extern i32 bInShutDown;
extern i32 bShowIt;
extern i32 bSpecialHideCursor;
extern char* cBuildingInfoNeutral[];
extern char cNetBoxColor[];
extern char cNetBoxLine[][140];
extern char* cOutOfMemory;
extern char* gArmyNames[CREATURE_COUNT];
extern char* gArmyNamesPlural[CREATURE_COUNT];
extern char* cMonFilename[CREATURE_COUNT];
extern char* cArmyFrameFileNames[CREATURE_COUNT];
extern char* gArmyShortNames[CREATURE_COUNT];
extern i32 gArtifactBaseRV[];
extern i32 gbAllBlack;
extern i32 gbCheatMenus;
extern i32 gbClosingApp;
extern i8 gbCombatSurrender;
extern i8 captainStats[FACTION_COUNT][HERO_PRIMARY_STAT_COUNT];
extern i32 gbDrawSavedCursor;
extern i32 gbForegroundApp;
extern i32 gbFunctionComplete;
extern i32 gbGameInitialized;
extern i8 gbGamePosToNetPos[OLD_MAIN_MATCH_BUFFER_SIZE];
extern i32 gbHeroMoving;
extern i32 gbHumanPlayer[];
extern i32 gbLoadingMonoIcon;
extern u8 gColorTableYellow[];
extern u8 gColorTableScenWin[];
extern u8 gColorTableDarkGray[];
extern i32 gbInMemError;
extern i32 gbInNewGameSetup;
extern i32 gbInPollSound;
extern i32 gbNoCDRom;
extern i32 gbNoSound;
extern i32 gbPutzingWithMouseCtr;
extern i32 gbRemoteOn;
extern i8 gbRetreatWin;
extern i32 gbTextEntryEscaped;
extern i32 gbThisNetGotAdventureControl;
extern i8 gbThisNetHumanPlayer[];
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
extern i32 giCycleType;
extern i32 giDebugLevel;
extern i32 giDialogTimeout;
extern u8 giGroundToTerrain[];
extern i32 giHighMemBuffer;
extern i32 giMainVideoModeColorDepth;
extern i32 giNumHumanPlayers;
extern i16 giScoreCampaignMon[][MONSTER_SCORE_FIELD_COUNT];
extern i16 giScoreMon[][MONSTER_SCORE_FIELD_COUNT];
extern i32 giTCPHostStatus;
extern i32 giThisGamePos;
extern i32 giThisNetPos;
extern i32 giTotalHighMem;
extern i32 giWaitType;
extern i32 glTimers[GLOBAL_TIMER_COUNT];
extern i32 gMageBaseResourceValues[];
extern i32 gMageBuildingCosts[][KB_BUILDING_RESOURCE_COUNT];
extern tag_monsterInfo gMonsterDatabase[CREATURE_COUNT];
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
extern SSpellInfo gsSpellInfo[SPELL_COUNT];
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
extern u8 iGetSSByAlignment[HERO_SKILL_COUNT][FACTION_COUNT];

#endif // HOMM2_KB_H
