#include <Ints.h>
#include <vector>
#include <SOURCE/PHILAI.h>
#include <SOURCE/philAI.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/GAME.h>
#include <BASE/Icon2b.h>
#include <BASE/Misc.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/FINDPATH.h>
#include <SOURCE/HERO.h>
#include <SOURCE/KB.h>
#include <SOURCE/REMOTE.h>
#include <PLATFORM/File.h>
#include <PLATFORM/Platform.h>
#include <PLATFORM/Strings.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <SOURCE/game.h>
#include <SOURCE/playerData.h>
#include <SOURCE/town.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/advManager.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/army.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/Campaign.h>
#include <SOURCE/townManager.h>
#include <SOURCE/TOWNMGR.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/EVENTS.h>
#include <IRONFIST/artifacts.h>
#include <IRONFIST/creatures.h>
#include <IRONFIST/prefs.h>
#include <IRONFIST/heroes.h>
#include <IRONFIST/hooks.h>
#include <IRONFIST/save_xml.h>
#include <IRONFIST/townconsts.h>
#include <EDITOR/mapcell.h>
#include <EDITOR/fullMap.h>
#include <BASE/soundManager.h>
#include <BASE/resourceManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/heroWindow.h>
#include <BASE/font.h>
#include <BASE/iconWidget.h>
#include <BASE/BITS.h>
#include <BASE/Bzip.h>
#include <BASE/INPUTMGR.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
#include <SOURCE/ARMY.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/Localization.h>
#include <SOURCE/SaveNames.h>
#include <BASE/Utf8.h>

#include <string>

#define GAME_SCORE_EXTRA_LARGE_DAY_SCALE 0.6
#define GAME_SCORE_LARGE_DAY_SCALE                                                 \
    0.8
#define GAME_SCORE_MEDIUM_DAY_SCALE 1.0
#define GAME_SCORE_SMALL_DAY_SCALE 1.4
#define GAME_VIEW_ARMY_FRAME_DELAY_SCALE 1.35
#define GAME_ULTIMATE_ARTIFACT_TWO_THIRDS                                          \
    0.66

typedef enum MapTilesetConstant {
    WAGON_CAMP_ACTIVE_FRAME = 129
} MapTilesetConstant;

typedef enum ExpansionCampaignSaveConstant {
    CAMPAIGN_SAVE_PREFIX_SIZE = 0x4f
} ExpansionCampaignSaveConstant;


typedef enum GameSaveFormatConstant {
    SAVE_PATH_CAPACITY                 = 452,
    SAVE_LEGACY_SCRATCH_SIZE           = 100,
    SAVE_LEGACY_CLEAR_SIZE             = 40,
    SAVE_LEGACY_SERIALIZED_SIZE        = 36,
    SAVE_STANDARD_FILENAME_SIZE        = 14,
    STANDARD_FILENAME_BASENAME_SIZE    = 8,
    SAVE_CURRENT_PLAYER_SCRATCH_SIZE   = 4,
    SAVE_PLAYER_FLAGS_SCRATCH_SIZE     = 8,
    SAVE_SPARE_SLOT_COUNT              = 6,
    LOAD_CURRENT_PLAYER_SCRATCH_SIZE   = 4,
    SAVE_TRUNCATED_SCALAR_SIZE         = sizeof(i8),
    SAVE_EVENT_HEADER_SIZE             = sizeof(u16) * 2,
    SAVE_EXPANSION_CAMPAIGN_FORMAT_TAG = 2
} GameSaveFormatConstant;

typedef enum GameHeroSelectionConstant {
    HERO_SELECTION_RETRY_LIMIT            = 2000,
    HERO_SELECTION_REUSE_RETRY_LIMIT      = 1500,
    HERO_SELECTION_FACTION_RETRY_LIMIT    = 100,
    HERO_SELECTION_EXPERIENCE_RETRY_LIMIT = 40,
    HERO_SELECTION_MINIMUM_EXPERIENCE     = 1000,
    HERO_SELECTION_CAMPAIGN_RETRY_LIMIT   = 500
} GameHeroSelectionConstant;

typedef enum GameInitialStateConstant {
    INITIAL_DIFFICULTY_RATING   = 1,
    INITIAL_PLAYER_COUNT        = 4,
    INITIAL_CALENDAR_VALUE      = 1,
    INITIAL_MAP_CHANGE_SEQUENCE = 1,
    INITIAL_RECORD_TYPE_STRIDE  = H2EnumIndex(GAME_HERO_COUNT) / H2EnumIndex(FACTION_COUNT),
    BOAT_HERO_NONE              = -1,
    BOAT_SLOT_EMPTY             = -1
} GameInitialStateConstant;

typedef enum NeutralTownReinforcementConstant {
    REINFORCEMENT_CASTLE_CHANCE        = 80,
    REINFORCEMENT_CHANCE               = 40,
    REINFORCEMENT_ROLL_PERCENT_MAX     = 100,
    REINFORCEMENT_ROLL_MIN             = 1,
    REINFORCEMENT_ROLL_MAX             = 15,
    REINFORCEMENT_TIER_ONE_THRESHOLD   = 5,
    REINFORCEMENT_TIER_TWO_THRESHOLD   = 10,
    REINFORCEMENT_TIER_THREE_THRESHOLD = 13,
    REINFORCEMENT_TIER_FOUR_THRESHOLD  = 15,
    REINFORCEMENT_TIER_ONE_KEY         = 10,
    REINFORCEMENT_TIER_TWO_KEY         = 20,
    REINFORCEMENT_TIER_THREE_KEY       = 30,
    REINFORCEMENT_TIER_FOUR_KEY        = 40,
    REINFORCEMENT_TIER_FIVE_KEY        = 50,
    REINFORCEMENT_TIER_ONE_COUNT_MIN   = 8,
    REINFORCEMENT_TIER_ONE_COUNT_MAX   = 15,
    REINFORCEMENT_TIER_TWO_COUNT_MIN   = 5,
    REINFORCEMENT_TIER_TWO_COUNT_MAX   = 7,
    REINFORCEMENT_TIER_THREE_COUNT_MIN = 3,
    REINFORCEMENT_TIER_THREE_COUNT_MAX = 5,
    REINFORCEMENT_TIER_FOUR_COUNT_MIN  = 1,
    REINFORCEMENT_TIER_FOUR_COUNT_MAX  = 3,
    REINFORCEMENT_TIER_FIVE_COUNT      = 1,
    REINFORCEMENT_TURN_ROLL_DIVISOR    = 10,
    REINFORCEMENT_TURN_COUNT_DIVISOR   = 20
} NeutralTownReinforcementConstant;

typedef enum NewMapConstant {
    VICTORY_SIDE_SEARCH_DONE                   = 99,
    STARTING_HERO_TOWN_PASS_COUNT              = 2,
    STARTING_HERO_ALLOW_NON_CASTLE_PASS        = 1,
    NECROMANCER_SHRINE_BUILD_MASK              = 1 << H2EnumIndex(BUILDING_SLOT_NECROMANCER_SHRINE),
    CAMPAIGN_SCENARIO_NUMBER_OFFSET            = 1,
    CAMPAIGN_ROLAND_ULTIMATE_CROWN_SCENARIO    = 8,
    CAMPAIGN_ARCHIBALD_ULTIMATE_CROWN_SCENARIO = 9,
    ULTIMATE_DISTANCE_ROLL_MIN                 = 1,
    ULTIMATE_DISTANCE_COMMON_ROLL_MAX          = 20,
    ULTIMATE_DISTANCE_BONUS_ROLL_MAX           = 30,
    ULTIMATE_ARTIFACT_BORDER_MARGIN            = 9,
    ULTIMATE_HUMAN_DISTANCE_RETRY_LIMIT        = 200,
    ULTIMATE_SEARCH_REGION_RETRY_LIMIT         = 400
} NewMapConstant;

typedef enum GameDailyEconomyConstant {
    DAILY_GOLD_MINE_INCOME          = 1000,
    DAILY_GOLD_TOWN_INCOME          = 1000,
    DAILY_GOLD_VILLAGE_INCOME       = 250,
    DAILY_GOLD_STATUE_INCOME        = 250,
    DAILY_GOLD_DUNGEON_INCOME       = 500,
    DAILY_GOLD_ENDLESS_SACK_INCOME  = 1000,
    DAILY_GOLD_ENDLESS_BAG_INCOME   = 750,
    DAILY_GOLD_ENDLESS_PURSE_INCOME = 500,
    DAILY_GOLD_GOLDEN_GOOSE_INCOME  = 10000,
    DAILY_GOLD_TAX_LIEN_INCOME      = -250,
    DAILY_MINE_BULK_RESOURCE_INCOME = 2,
    DAILY_CAMPAIGN_WOOD_BONUS       = 2,
    DAILY_RESOURCE_BONUS_FIRST_DAY  = 1,
    DAILY_RESOURCE_BONUS_LAST_DAY   = H2EnumIndex(RES_GOLD),
    GAME_WEEKS_PER_MONTH            = 4,
    POWER_RING_DAILY_MANA_BONUS     = 2
} GameDailyEconomyConstant;

typedef enum WeeklyRuntimeConstant {
    WEEK_NAME_LAST                = KB_WEEK_NAME_COUNT - 1,
    CREATURE_WEEK_LAST            = H2EnumIndex(CREATURE_BONE_DRAGON),
    SPECIAL_WEEK_ROLL_MAX         = 4,
    CASTLE_GROWTH_SPECIAL_BONUS   = 2,
    CASTLE_GROWTH_WELL_BONUS      = 8,
    NEUTRAL_CASTLE_GROWTH_DIVISOR = 2,
    CREATURE_WEEK_GROWTH_BONUS    = 5,
    CREATURE_MONTH_MULTIPLIER     = 2
} WeeklyRuntimeConstant;

typedef enum RandomMapConstant {
    RANDOM_TOWN_SCRATCH_WIDTH    = 2,
    RANDOM_MINE_RETRY_LIMIT      = 30,
    RANDOM_MINE_FOOTPRINT_WIDTH  = 2,
    RANDOM_MINE_RESOURCE_COUNT   = H2EnumIndex(MINE_TYPE_GOLD) + 1,
    RANDOM_HERO_SECOND_SELECTION = 2,
    RANDOM_HERO_AVERAGE_DIVISOR  = 2
} RandomMapConstant;

typedef enum GameMapSetupConstant {
    COMPUTER_SCREEN_WIDGET_FIRST          = 1,
    COMPUTER_SCREEN_WIDGET_LAST           = 6,
    HEROES_LOGO_X                         = 480,
    HEROES_LOGO_Y                         = 16,
    HEROES_LOGO_WIDTH                     = 144,
    HEROES_LOGO_HEIGHT                    = 144,
    GAME_SCREEN_WIDTH                     = 640,
    GAME_SCREEN_HEIGHT                    = 480,
    ALCHEMIST_LATE_OVERLAY_OFFSET         = 2,
    DEFAULT_DWELLING_ROLL_CAPACITY        = 12,
    DEFAULT_DWELLING_ROLL_BUCKET_COUNT    = 10,
    TOWN_UPGRADE_BUILDING_FIRST           = H2EnumIndex(BUILDING_SLOT_UPGRADE_FIRST),
    TOWN_UPGRADE_BUILDING_LAST            = H2EnumIndex(BUILDING_SLOT_SPECIAL_THIRTY),
    TOWN_DWELLING_BUILDING_FIRST          = H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST),
    TOWN_DWELLING_BUILDING_LAST           = H2EnumIndex(BUILDING_SLOT_DWELLING_LAST),
    TOWN_UPGRADE_TO_DWELLING_OFFSET       = 5,
    MAP_HERO_PROCESS_PASS_COUNT           = 3,
    MAP_HERO_ASSIGNMENT_PASS              = 0,
    MAP_HERO_CLASS_PASS                   = 1,
    MAP_HERO_PLACEMENT_PASS               = 2,
    MAP_HERO_FRAME_STRIDE                 = H2EnumIndex(FACTION_NEUTRAL) + 1,
    MAP_HERO_RANDOM_FACTION_FRAME         = H2EnumIndex(FACTION_NEUTRAL),
    MAP_HEROES_PER_FACTION                = GAME_HERO_COUNT / H2EnumIndex(FACTION_COUNT),
    MAP_HERO_CLASS_SCAN_RETRY_LIMIT       = 1000,
    MAP_HERO_SCOUTING_SKILL_INDEX         = H2EnumIndex(HERO_SKILL_SCOUTING),
    HERO_CONSISTENCY_PLAYABLE_FACTION_MAX = H2EnumIndex(FACTION_NECROMANCER),
    HERO_CONSISTENCY_POOL_THRESHOLD       = 40
} GameMapSetupConstant;

typedef enum GamePlayerTurnConstant {
    GAME_AI_MUSIC_TRACK            = 28,
    ENVIRONMENT_ORIGIN_TILE_OFFSET = 7
} GamePlayerTurnConstant;

typedef enum GameScoreConstant {
    SCORE_BASE              = 200,
    SCORE_FIRST_TIER        = 60,
    SCORE_SECOND_TIER       = 120,
    SCORE_THIRD_TIER        = 360,
    SCORE_MINIMUM           = 20,
    RATING_EASY_BONUS       = 50,
    RATING_NORMAL_BONUS     = 80,
    RATING_HARD_BONUS       = 100,
    RATING_EXPERT_BONUS     = 120,
    RATING_IMPOSSIBLE_BONUS = 140,
    MAP_RATING_EASY_BONUS   = 0,
    MAP_RATING_NORMAL_BONUS = 20,
    MAP_RATING_HARD_BONUS   = 40,
    MAP_RATING_EXPERT_BONUS = 80
} GameScoreConstant;

typedef enum GameJoinConstant {
    JOIN_HEADER_SIZE = 2,
    JOIN_BUFFER_SIZE = 700000
} GameJoinConstant;

typedef enum RemoteSaveConstant {
    TRANSMIT_FILENAME_CAPACITY       = SAVE_PATH_CAPACITY,
    RECEIVE_FILENAME_CAPACITY        = SAVE_PATH_CAPACITY,
    REMOTE_LOOPING_SOUND_COUNT       = 4,
    REMOTE_PACKET_TRACKING_CAPACITY  = 5000,
    REMOTE_HEADER_CAPACITY           = 256,
    REMOTE_BUFFER_EXTRA              = 2000,
    REMOTE_PACKET_PAYLOAD_SIZE       = 200,
    REMOTE_PACKET_BATCH_SIZE         = 100,
    REMOTE_PACKET_INDEX_SIZE         = sizeof(i16),
    REMOTE_SAVE_HEADER_SIZE          = sizeof(i32) * 4,
    REMOTE_DECODE_BUFFER_SIZE        = JOIN_BUFFER_SIZE,
    REMOTE_RECEIVE_TIMEOUT           = 90000,
    REMOTE_RECEIVE_DIALOG_BUTTONS    = 2,
    REMOTE_MAP_CHANGE_UNWIND_LIMIT   = 999,
    REMOTE_SAVE_HEADER_FILE_SIZE     = 0,
    REMOTE_SAVE_HEADER_FILE_CRC      = 1,
    REMOTE_SAVE_HEADER_TRANSMIT_CRC  = 2,
    REMOTE_SAVE_HEADER_PLAYER        = 3,
    REMOTE_SAVE_INIT_COMMAND         = 1,
    REMOTE_SAVE_INIT_RESPONSE        = 2,
    REMOTE_SAVE_DATA_COMMAND         = 3,
    REMOTE_SAVE_ACK_REQUEST_COMMAND  = 4,
    REMOTE_SAVE_ACK_RESPONSE_COMMAND = 5,
    REMOTE_SAVE_FINISH_COMMAND       = 6
} RemoteSaveConstant;

typedef enum NewTurnConstant {
    NEW_TURN_MUSIC_FILENAME_CAPACITY = 16,
    NEW_TURN_LOWER_NAME_CAPACITY     = 52,
    NEW_TURN_BOTTOM_VIEW_DURATION    = 3000,
    NEW_TURN_DIALOG_TYPE             = 9,
    NEW_MONTH_MUSIC_TRACK            = 21,
    NEW_WEEK_MUSIC_TRACK             = 20,
    NEW_MONTH_NORMAL_TEXT            = 2,
    NEW_MONTH_CREATURE_TEXT          = 3,
    NEW_MONTH_PLAGUE_TEXT            = 4,
    NEW_WEEK_NORMAL_TEXT             = 5,
    NEW_WEEK_CREATURE_TEXT           = 6
} NewTurnConstant;

typedef enum DiffRuntimeConstant {
    DIFF_WORD_SHIFT       = 16,
    DIFF_WORD_HEADER_SIZE = 3,
    DIFF_BYTE_SHIFT       = 8,
    DIFF_BYTE_HEADER_SIZE = 2,
    DIFF_BUFFER_EXTRA     = 5000,
    DIFF_MAX_SHORT_MATCH  = 3,
    DIFF_COPY_FLAG_SHIFT  = 7
} DiffRuntimeConstant;

#define SCORE_SECOND_TIER_FACTOR 0.5
#define SCORE_SECOND_TIER_BASE_DEDUCTION 30.0
#define SCORE_THIRD_TIER_FACTOR 0.25
#define SCORE_THIRD_TIER_BASE_DEDUCTION 60.0
#define SCORE_FINAL_TIER_FACTOR 0.125

typedef enum GameDiffEncoding {
    COMMAND_SHIFT      = 7,
    LEN_WORD_FLAG      = 0x40,
    LEN_BYTE_FLAG      = 0x20,
    LEN_WORD_HIGH_MASK = 0x3f,
    LEN_HIGH_MASK      = 0x2f0000,
    LEN_LOW_MASK       = 0xffff,
    LEN_SHORT_MASK     = 0x1f,
    LEN_MID_MASK       = 0x1f00,
    LEN_BYTE_MASK      = 0xff,
    LEN_BYTE_MAX       = 0x1f,
    LEN_WORD_MAX       = 0x1fff
} GameDiffEncoding;

typedef enum GameSaveSentinel {
    SAVED_TOWN_OFF_MAP = 0xff
} GameSaveSentinel;

typedef enum GameMonthlyConstant {
    ROLL_MIN                      = 1,
    ROLL_MAX                      = 10,
    NORMAL_ROLL_MAX               = 5,
    CREATURE_ROLL_MAX             = 9,
    NORMAL_NAME_MIN               = 0,
    NORMAL_NAME_MAX               = 9,
    CREATURE_LIST_MIN             = 0,
    CREATURE_LIST_MAX             = 11,
    WELL_BUILDING                 = 0x10,
    WELL_GROWTH                   = 2,
    FIRST_DWELLING_BONUS_BUILDING = 0x800,
    FIRST_DWELLING_GROWTH         = 8,
    MONSTER_TRIGGER               = 0x98,
    MONSTER_SPAWN_MIN             = 0,
    MONSTER_SPAWN_MAX             = 360,
    MONSTER_SPAWN_ROLL            = 10
} GameMonthlyConstant;

typedef enum GameRandomArtifactConstant {
    ARTIFACT_BASE_TABLE_SIZE      = H2EnumIndex(ARTIFACT_MAGIC_BOOK) + 1,
    ARTIFACT_FIRST                = H2EnumIndex(ARTIFACT_ULTIMATE_BOOK),
    ARTIFACT_BASE_LAST            = H2EnumIndex(ARTIFACT_MAGIC_BOOK),
    ARTIFACT_EXPANSION_LAST       = H2EnumIndex(ARTIFACT_SPADE_NECROMANCY),
    ARTIFACT_UNIQUE_RETRIES       = 100,
    ARTIFACT_CURSED_ROLL_MAX      = 100,
    ARTIFACT_CURSED_REJECT_CHANCE = 30,
    ARTIFACT_ID_OFFSET            = 1
} GameRandomArtifactConstant;

typedef enum LayerScanConstant {
    LAYER_SCAN_CAPACITY = 5
} LayerScanConstant;

typedef enum RandomMapObjectFootprintConstant {
    CASTLE_METADATA_X_RADIUS      = 2,
    CASTLE_METADATA_TOP_OFFSET    = 3,
    CASTLE_METADATA_BOTTOM_OFFSET = 1,
    CASTLE_BOAT_X_OFFSET          = 1,
    CASTLE_BOAT_Y_OFFSET          = 2,
    MINE_METADATA_LEFT_OFFSET     = 2,
    MINE_METADATA_RIGHT_OFFSET    = 1,
    MINE_METADATA_TOP_OFFSET      = 1
} RandomMapObjectFootprintConstant;

typedef enum RandomMineConstant {
    ABANDONED_MINE_GUARDIAN_COUNT_MIN = 30,
    ABANDONED_MINE_GUARDIAN_COUNT_MAX = 60,
    WINDMILL_RESOURCE_AMOUNT_MIN      = 1,
    WINDMILL_RESOURCE_AMOUNT_MAX      = 5
} RandomMineConstant;

inline MineType RandomMineType(MineType first, MineType last) {
    return static_cast<MineType>(Random(H2EnumIndex(first), H2EnumIndex(last)));
}

typedef enum ArtifactGuardianConstant {
    ARTIFACT_GUARDIAN_CHOICE_COUNT = 10,
    MINOR_GUARDIAN_CHOICE_FIRST    = 0,
    MINOR_GUARDIAN_CHOICE_COUNT    = 4,
    MAJOR_GUARDIAN_CHOICE_FIRST    = 4,
    MAJOR_GUARDIAN_CHOICE_COUNT    = 6
} ArtifactGuardianConstant;

typedef enum ArtifactEventGenerationConstant {
    ARTIFACT_EVENT_UNCONDITIONAL_CUTOFF = 60,
    ARTIFACT_EVENT_GUARD_CUTOFF         = 80,
    ARTIFACT_EVENT_WISDOM_BUCKET        = 1,
    ARTIFACT_EVENT_LEADERSHIP_BUCKET    = 2
} ArtifactEventGenerationConstant;

typedef enum WitchHutConstant {
    WITCH_HUT_SKILL_FIRST = H2EnumIndex(HERO_SKILL_PATHFINDING),
    WITCH_HUT_SKILL_LAST  = H2EnumIndex(HERO_SKILL_ESTATES)
} WitchHutConstant;

typedef enum SkeletonEventConstant {
    SKELETON_DESERT_FRAME      = 84,
    SKELETON_ROLL_MIN          = 0,
    SKELETON_ROLL_MAX          = 9,
    SKELETON_ARTIFACT_ROLL_MAX = 2
} SkeletonEventConstant;

typedef enum EventGenerationRollConstant {
    EVENT_ROLL_MIN        = 0,
    EVENT_BINARY_ROLL_MAX = 1,
    EVENT_ROLL_MAX        = 100,
    EVENT_BUCKET_ROLL_MAX = 99,
    EVENT_BUCKET_COUNT    = 10
} EventGenerationRollConstant;

typedef enum PackedResourceGenerationConstant {
    WAGON_EMPTY_CUTOFF    = 40,
    WAGON_ARTIFACT_CUTOFF = 50,
    WAGON_AMOUNT_MIN      = 2,
    WAGON_AMOUNT_MAX      = 5,
    LEAN_TO_AMOUNT_MIN    = 1,
    LEAN_TO_AMOUNT_MAX    = 4,
    CAMPFIRE_AMOUNT_MIN   = 4,
    CAMPFIRE_AMOUNT_MAX   = 6
} PackedResourceGenerationConstant;

typedef enum TreasureChestGenerationConstant {
    SEA_CHEST_EMPTY_CUTOFF   = 20,
    SEA_CHEST_GOLD_CUTOFF    = 90,
    LAND_CHEST_SMALL_CUTOFF  = 32,
    LAND_CHEST_MEDIUM_CUTOFF = 64,
    LAND_CHEST_LARGE_CUTOFF  = 95
} TreasureChestGenerationConstant;

typedef enum AncientLampGenerationConstant {
    ANCIENT_LAMP_ROLL_MIN     = 0,
    ANCIENT_LAMP_ROLL_MAX     = 2,
    ANCIENT_LAMP_COUNT_OFFSET = 2
} AncientLampGenerationConstant;

typedef enum ShipwreckSurvivorGenerationConstant {
    SHIPWRECK_SURVIVOR_TREASURE_CUTOFF = 60,
    SHIPWRECK_SURVIVOR_MINOR_CUTOFF    = 80
} ShipwreckSurvivorGenerationConstant;

typedef enum MonsterGuardGenerationConstant {
    MONSTER_GUARD_ROLL_MIN = 0,
    MONSTER_GUARD_ROLL_MAX = 100,
    MONSTER_GUARD_CUTOFF   = 20
} MonsterGuardGenerationConstant;

typedef enum ResourceGenerationConstant {
    RESOURCE_BULK_AMOUNT_MIN   = 5,
    RESOURCE_BULK_AMOUNT_MAX   = 10,
    RESOURCE_SCARCE_AMOUNT_MIN = 3,
    RESOURCE_SCARCE_AMOUNT_MAX = 6
} ResourceGenerationConstant;

typedef enum GameVisibilityConstant {
    EARLY_TURN_LAST        = 20,
    MIDDLE_TURN_LAST       = 40,
    EARLY_AI_BONUS         = 1,
    MIDDLE_AI_BONUS        = 2,
    LATE_AI_BONUS          = 3,
    SMALL_RADIUS_LIMIT     = 5,
    SMALL_RADIUS_THRESHOLD = 2,
    LARGE_RADIUS_THRESHOLD = 3,
    RADIAL_RADIUS_LIMIT    = 10
} GameVisibilityConstant;

typedef enum GameLuckConstant {
    NEUTRAL       = 0,
    MINIMUM       = -3,
    MAXIMUM       = 3,
    RAINBOW_BONUS = 2
} GameLuckConstant;

typedef enum GameTimeEventConstant {
    EVENT_DAYS_PER_WEEK    = 7,
    EVENT_DAYS_PER_MONTH   = 28,
    EVENT_RESOURCE_COUNT   = 7,
    EVENT_RESOURCE_PENALTY = 100000
} GameTimeEventConstant;

typedef enum GameCompressionTestConstant {
    TEST_RANDOM_SIZE_MIN     = 20000,
    TEST_RANDOM_SIZE_MAX     = 100000,
    TEST_RANDOM_BUFFER_EXTRA = 5000,
    TEST_FILE_BUFFER_EXTRA   = 2000,
    TEST_FILENAME_SIZE       = 32,
    TEST_MESSAGE_CAPACITY    = 40,
    COMPRESS_TEST_ITERATIONS = 100
} GameCompressionTestConstant;

typedef enum GameRumourConstant {
    RUMOUR_SCRATCH_CAPACITY        = 100,
    RUMOUR_CATEGORY_ORDER_CAPACITY = 8
} GameRumourConstant;

b32 gbGameOver = false;

typedef enum GameTuningConstant {
    RANDOM_SCAN_RETRY_LIMIT          = 10000,
    EXPERIENCE_HERO_PRESENCE_BONUS   = 500,
    MINE_FLAG_OVERWRITE_LIMIT        = 0x30,
    RANDOM_MONSTER_SPRITE_FIRST      = 0x43,
    RANDOM_MONSTER_SPRITE_LAST       = 0x46,
    RANDOM_MONSTER_SPRITE_TO_TRIGGER =
        0x70,
    BANK_GUARDIAN_FLAG               = 0x100,
    TOWN_RECORD_TYPE_MASK            = 0x7f
} GameTuningConstant;

typedef enum GamePasswordConstant {
    PASSWORD_INDEX_COUNT = X_GLOBAL_PASSWORD_STRING_INDEX_COUNT,
    PASSWORD_INDEX_MASK  = PASSWORD_INDEX_COUNT - 1,
    PASSWORD_COLOR_SHIFT = 3
} GamePasswordConstant;

typedef enum GameLoadMapConstant {
    LOAD_MAP_COORDINATE_SCRATCH_SIZE = 4,
    LOAD_MAP_RECORD_SCRATCH_SIZE     = 8
} GameLoadMapConstant;

typedef enum GameOwnershipConstant {
    TOWN_FLAG_FRAME_STRIDE       = 2,
    TOWN_FLAG_RIGHT_FRAME        = 1,
    TOWN_NEW_OWNER_TURN_COUNT    = 2,
    MINE_FLAG_COMMON_OFFSET      = 14,
    MINE_FLAG_MERCURY_OFFSET     = 21,
    MINE_FLAG_WOOD_OFFSET        = 28,
    MINE_FLAG_ALCHEMIST_OFFSET   = 35,
    MINE_FLAG_LIGHTHOUSE_OFFSET  = 42,
    MINE_ALCHEMIST_FLAG_Y_OFFSET = 3
} GameOwnershipConstant;

typedef enum GameViewSpellsConstant {
    VIEW_SPELLS_WINDOW_X               = 86,
    VIEW_SPELLS_WINDOW_Y               = 87,
    VIEW_SPELL_PREVIOUS_ID             = 2,
    VIEW_SPELL_NEXT_ID                 = 3,
    VIEW_SPELL_COMBAT_TAB_ID           = 4,
    VIEW_SPELL_ADVENTURE_TAB_ID        = 5,
    VIEW_SPELL_MANA_LABEL_ID           = 6,
    VIEW_SPELL_MANA_HUNDREDS_ID        = 7,
    VIEW_SPELL_MANA_TENS_ID            = 8,
    VIEW_SPELL_MANA_ONES_ID            = 9,
    VIEW_SPELL_CLOSE_ID                = 10,
    VIEW_SPELL_PAGE_SIZE               = 12,
    VIEW_SPELL_TEXT_ID_BASE            = 30,
    VIEW_SPELL_ICON_ID_0               = 100,
    VIEW_SPELL_ICON_ID_1               = 101,
    VIEW_SPELL_ICON_ID_2               = 102,
    VIEW_SPELL_ICON_ID_3               = 103,
    VIEW_SPELL_ICON_ID_4               = 104,
    VIEW_SPELL_ICON_ID_5               = 105,
    VIEW_SPELL_ICON_ID_6               = 106,
    VIEW_SPELL_ICON_ID_7               = 107,
    VIEW_SPELL_ICON_ID_8               = 108,
    VIEW_SPELL_ICON_ID_9               = 109,
    VIEW_SPELL_ICON_ID_10              = 110,
    VIEW_SPELL_ICON_ID_11              = 111,
    VIEW_SPELL_ICON_ID_BASE            = VIEW_SPELL_ICON_ID_0,
    VIEW_SPELL_AVAILABLE_COLOR         = 1,
    VIEW_SPELL_UNAVAILABLE_COLOR       = 3,
    VIEW_SPELL_NAME_WIDTH              = 78,
    VIEW_SPELL_MANA_MAX                = 999,
    VIEW_SPELL_MANA_HUNDREDS_THRESHOLD = 99,
    VIEW_SPELL_MANA_TENS_THRESHOLD     = 9,
    VIEW_SPELL_MANA_HUNDREDS_DIVISOR   = 100,
    VIEW_SPELL_MANA_TENS_DIVISOR       = 10,
    VIEW_SPELL_MANA_DIGIT_BASE         = 10,
    VIEW_SPELL_HELP_PREVIOUS           = 0,
    VIEW_SPELL_HELP_NEXT               = 1,
    VIEW_SPELL_HELP_COMBAT             = 2,
    VIEW_SPELL_HELP_ADVENTURE          = 3,
    VIEW_SPELL_HELP_CLOSE              = 4,
    VIEW_SPELL_HELP_OTHER              = 5,
    VIEW_SPELL_HELP_MANA               = 8
} GameViewSpellsConstant;

typedef enum GameViewArmyConstant {
    VIEW_ARMY_UNUSED_BASE_X           = 86,
    VIEW_ARMY_UNUSED_QUICK_BASE_Y     = 164,
    VIEW_ARMY_BLANK_WIDGET_ID         = 1,
    VIEW_ARMY_COUNT_WIDGET_ID         = 2,
    VIEW_ARMY_TITLE_WIDGET_ID         = 3,
    VIEW_ARMY_DETAIL_WIDGET_ID        = 4,
    VIEW_ARMY_MONSTER_WIDGET_ID       = 5,
    VIEW_ARMY_UPGRADE_ACTION_ID       = 500,
    VIEW_ARMY_CLOSE_ID                = 10,
    VIEW_ARMY_WINDOW_X                = 19,
    VIEW_ARMY_WINDOW_Y                = 75,
    VIEW_ARMY_MONSTER_BASE_X          = 167,
    VIEW_ARMY_MONSTER_BASE_Y          = 138,
    VIEW_ARMY_LOW_MEMORY_MONSTER_X    = 126,
    VIEW_ARMY_LOW_MEMORY_MONSTER_Y    = 93,
    VIEW_ARMY_MONSTER_WIDGET_WIDTH    = 86,
    VIEW_ARMY_MONSTER_WIDGET_HEIGHT   = 149,
    VIEW_ARMY_MONSTER_WIDGET_Z_ORDER  = 5,
    VIEW_ARMY_ICON_CENTER_DIVISOR     = 2,
    VIEW_ARMY_FILENAME_SIZE           = 16,
    VIEW_ARMY_NAME_SIZE               = 32,
    VIEW_ARMY_ASCII_CASE_OFFSET       = 32,
    VIEW_ARMY_DETAIL_BUFFER_SIZE      = 550,
    VIEW_ARMY_TEXT_NEUTRAL_OFFSET     = 3,
    VIEW_ARMY_COUNT_TEXT_SIZE         = 12,
    VIEW_ARMY_SPELL_BASE_Y            = 169,
    VIEW_ARMY_SPELL_CENTER_X          = 420,
    VIEW_ARMY_QUICK_SPELL_Y_OFFSET    = 12,
    VIEW_ARMY_SPELL_SPACING_BASE      = 44,
    VIEW_ARMY_SPELL_X_BIAS            = 10,
    VIEW_ARMY_SPELL_VISIBLE_LIMIT     = 6,
    VIEW_ARMY_SPELL_WIDGET_Y_OFFSET   = 14,
    VIEW_ARMY_SPELL_WIDGET_ID_BASE    = 200,
    VIEW_ARMY_ANIMATION_INITIAL_DELAY = 90,
    VIEW_ARMY_HANDLER_FRAME_DELAY     = 5,
    VIEW_ARMY_UPGRADE_COST_MULTIPLIER = 2,
    VIEW_ARMY_DWELLING_UPGRADE_OFFSET =
        H2EnumIndex(BUILDING_SLOT_UPGRADE_FIRST) - H2EnumIndex(BUILDING_SLOT_DWELLING_SECOND)
} GameViewArmyConstant;

typedef enum GameArmyDetailText {
    ARMY_DETAIL_ATTACK        = 0,
    ARMY_DETAIL_DEFENSE       = 1,
    ARMY_DETAIL_SHOTS_LEFT    = 2,
    ARMY_DETAIL_DAMAGE        = 3,
    ARMY_DETAIL_HIT_POINTS    = 4,
    ARMY_DETAIL_SPEED         = 5,
    ARMY_DETAIL_MORALE        = 6,
    ARMY_DETAIL_LUCK          = 7,
    ARMY_DETAIL_SHOTS_OUTSIDE = 8
} GameArmyDetailText;

#define GAME_HANDICAP_MODERATE_RESOURCE_FACTOR 0.85
#define GAME_HANDICAP_SEVERE_RESOURCE_FACTOR 0.7
#define GAME_HANDICAP_MODERATE_DAILY_PENALTY 0.15
#define GAME_HANDICAP_SEVERE_DAILY_PENALTY 0.30
#define GAME_DIFFICULTY_EASY_GOLD_FACTOR 0.75
#define GAME_DIFFICULTY_NORMAL_GOLD_FACTOR 1.0
#define GAME_DIFFICULTY_HARD_GOLD_FACTOR 1.29
#define GAME_DIFFICULTY_EXPERT_GOLD_FACTOR 1.45
#define GAME_DIFFICULTY_IMPOSSIBLE_GOLD_FACTOR 1.6
#define WEEKLY_HARD_GROWTH_FACTOR 1.20
#define WEEKLY_EXPERT_GROWTH_FACTOR 1.32
#define WEEKLY_IMPOSSIBLE_GROWTH_FACTOR 1.44

typedef enum ViewArmyControlId {
    VIEW_ARMY_QUICK_VIEW_ID = 0x7800,
    VIEW_ARMY_UPGRADE_ID    = 0x7803
} ViewArmyControlId;

#define WORLDMAP (&m_worldMap)

inline town* GetCastle(i32 idx) {
    return &gpGame->m_castleRecs[idx];
}

typedef enum PlayerDataSerializationConstant {
    PLAYER_SAVE_SCRATCH_SIZE       = 52,
    PLAYER_SAVE_SCRATCH_CLEAR_SIZE = 48,
    PLAYER_SAVE_RESERVED_SIZE      = 42,
    PLAYER_SAVE_CHEATED_FLAG_SIZE  = 1
} PlayerDataSerializationConstant;

void playerData::Write(i32 file) {
    char unused[PLAYER_SAVE_SCRATCH_SIZE];

    platform::FileWrite(file, &m_color, sizeof(m_color));
    platform::FileWrite(file, &m_heroCount, sizeof(m_heroCount));
    platform::FileWrite(file, &m_currentHero, sizeof(m_currentHero));
    platform::FileWrite(file, &m_heroLocatorPage, sizeof(m_heroLocatorPage));
    platform::FileWrite(file, m_heroIds, sizeof(m_heroIds));
    platform::FileWrite(file, m_availableHeroIds, sizeof(m_availableHeroIds));
    memset(unused, 0, PLAYER_SAVE_SCRATCH_CLEAR_SIZE);
    platform::FileWrite(file, unused, PLAYER_SAVE_RESERVED_SIZE);
    platform::FileWrite(file, &gpGame->m_cheated, PLAYER_SAVE_CHEATED_FLAG_SIZE);
    platform::FileWrite(file, &m_cheatValue, sizeof(m_cheatValue));
    platform::FileWrite(file, &m_aiDifficulty, sizeof(m_aiDifficulty));
    platform::FileWrite(file, &m_minimumHeroCount, sizeof(m_minimumHeroCount));
    platform::FileWrite(file, &m_evilInterface, sizeof(m_evilInterface));
    platform::FileWrite(file, &m_ultimateArtifactHintChance, sizeof(m_ultimateArtifactHintChance));
    platform::FileWrite(file, &m_ultimateArtifactHintX, sizeof(m_ultimateArtifactHintX));
    platform::FileWrite(file, &m_ultimateArtifactHintY, sizeof(m_ultimateArtifactHintY));
    platform::FileWrite(file, &m_daysLeft, sizeof(m_daysLeft));
    platform::FileWrite(file, &m_townCount, sizeof(m_townCount));
    platform::FileWrite(file, &m_currentTown, sizeof(m_currentTown));
    platform::FileWrite(file, &m_townLocatorPage, sizeof(m_townLocatorPage));
    platform::FileWrite(file, m_townIds, sizeof(m_townIds));
    platform::FileWrite(file, m_resources, sizeof(m_resources));
    platform::FileWrite(file, m_aiData.m_income, sizeof(m_aiData.m_income));
    platform::FileWrite(file, &m_barrierTents, sizeof(m_barrierTents));
    platform::FileWrite(file, &m_barrierTents, sizeof(m_barrierTents));
    platform::FileWrite(file, m_unknownad, sizeof(m_unknownad));
}

void playerData::Read(i32 file) {
    char unused[PLAYER_SAVE_SCRATCH_SIZE];

    platform::FileRead(file, &m_color, sizeof(m_color));
    platform::FileRead(file, &m_heroCount, sizeof(m_heroCount));
    platform::FileRead(file, &m_currentHero, sizeof(m_currentHero));
    platform::FileRead(file, &m_heroLocatorPage, sizeof(m_heroLocatorPage));
    platform::FileRead(file, m_heroIds, sizeof(m_heroIds));
    platform::FileRead(file, m_availableHeroIds, sizeof(m_availableHeroIds));
    platform::FileRead(file, unused, PLAYER_SAVE_RESERVED_SIZE);
    platform::FileRead(file, &gpGame->m_cheated, PLAYER_SAVE_CHEATED_FLAG_SIZE);
    platform::FileRead(file, &m_cheatValue, sizeof(m_cheatValue));
    platform::FileRead(file, &m_aiDifficulty, sizeof(m_aiDifficulty));
    platform::FileRead(file, &m_minimumHeroCount, sizeof(m_minimumHeroCount));
    platform::FileRead(file, &m_evilInterface, sizeof(m_evilInterface));
    platform::FileRead(file, &m_ultimateArtifactHintChance, sizeof(m_ultimateArtifactHintChance));
    platform::FileRead(file, &m_ultimateArtifactHintX, sizeof(m_ultimateArtifactHintX));
    platform::FileRead(file, &m_ultimateArtifactHintY, sizeof(m_ultimateArtifactHintY));
    platform::FileRead(file, &m_daysLeft, sizeof(m_daysLeft));
    platform::FileRead(file, &m_townCount, sizeof(m_townCount));
    platform::FileRead(file, &m_currentTown, sizeof(m_currentTown));
    platform::FileRead(file, &m_townLocatorPage, sizeof(m_townLocatorPage));
    platform::FileRead(file, m_townIds, sizeof(m_townIds));
    platform::FileRead(file, m_resources, sizeof(m_resources));
    platform::FileRead(file, m_aiData.m_income, sizeof(m_aiData.m_income));
    platform::FileRead(file, &m_barrierTents, sizeof(m_barrierTents));
    platform::FileRead(file, &m_barrierTents, sizeof(m_barrierTents));
    platform::FileRead(file, m_unknownad, sizeof(m_unknownad));
}

i32 playerData::NextHero(i32) {
    i32 curHero = -1;
    i32 i;

    if (gpCurPlayer->m_currentHero != -1) {
        for (i = 0; i < gpCurPlayer->m_heroCount; i++) {
            if (gpCurPlayer->m_currentHero == gpCurPlayer->m_heroIds[i])
                curHero = i;
        }
    }

    for (i = curHero + 1; i < gpCurPlayer->m_heroCount; i++) {
        if (gpGame->IsMobile(gpCurPlayer->m_heroIds[i]))
            return m_heroIds[i];
    }
    for (i = 0; i < curHero + 1; i++) {
        if (gpGame->IsMobile(gpCurPlayer->m_heroIds[i]))
            return m_heroIds[i];
    }
    return -1;
}

i32 playerData::HasMobileHero(void) {
    i32 i;
    for (i = 0; i < m_heroCount; i++) {
        if (gpGame->IsMobile(m_heroIds[i]))
            return 1;
    }
    return 0;
}

i32 GetNumObelisks(i32 color) {
    i32 count = 0;
    i32 idx;
    for (idx = 0; idx < GAME_BOAT_COUNT; idx++) {
        if (gpGame->m_obeliskVisitors[idx] & (1 << color))
            count++;
    }
    return count;
}

i32 playerData::BuildingsOwned(FactionType townType, BuildingSlotType buildingIndex, i32 buildState) {
    i32 count = 0;
    i32 i;
    for (i = 0; i < m_townCount; i++) {
        town* ownedTown = &gpGame->m_castleRecs[m_townIds[i]];
        if (buildingIndex < BUILDING_SLOT_DWELLING_FIRST || ownedTown->m_type == townType) {
            if (buildingIndex == BUILDING_SLOT_MAGE_GUILD) {
                if (ownedTown->m_buildings & H2EnumIndex(TOWN_BUILDING_MAGE_GUILD)) {
                    if (ownedTown->m_buildState == buildState)
                        count++;
                }
            } else {
                if (ownedTown->m_buildings & (1 << H2EnumIndex(buildingIndex)))
                    count++;
            }
        }
    }
    return count;
}

i32 playerData::NumOfGivenArtifact(ArtifactType artifact) {
    i32 count = 0;
    i32 i;
    i32 jj;
    for (i = 0; i < m_heroCount; i++) {
        for (jj = 0; jj < HERO_ARTIFACT_SLOT_COUNT; jj++) {
            if (gpGame->m_heroRecs[m_heroIds[i]].m_artifacts[jj] == artifact)
                count++;
        }
    }
    return count;
}

i32 game::MineTypesOwned(i32 owner, MineType mineType) {
    i32 num = 0;
    i32 n;
    for (n = 0; n < GAME_MINE_COUNT; n++) {
        if (m_mines[n].owner == owner && m_mines[n].resourceType == mineType)
            num++;
    }
    return num;
}

typedef enum UltimateArtifactHintConstant {
    MINIMUM_PUZZLE_PIECES     = 8,
    HINT_CHANCE_PER_PIECE     = 4,
    HINT_CHANCE_MINIMUM       = 1,
    HINT_CHANCE_MAXIMUM       = 100,
    HINT_COORDINATE_UNKNOWN   = -1,
    HINT_OFFSET_CENTER        = 3,
    HINT_OFFSET_ROLL_MAXIMUM  = 2,
    HINT_LOCATION_RETRY_LIMIT = 200
} UltimateArtifactHintConstant;

void ComputeUALoc(i32 playerIndex) {
    i32 result = gpGame->SetupPuzzlePieces(playerIndex, 1);
    if (result < MINIMUM_PUZZLE_PIECES
        || gpGame->m_ultimateArtifactId == ARTIFACT_NONE) {
        gpGame->m_players[playerIndex].m_ultimateArtifactHintChance = 0;
        gpGame->m_players[playerIndex].m_ultimateArtifactHintX = HINT_COORDINATE_UNKNOWN;
        gpGame->m_players[playerIndex].m_ultimateArtifactHintY = HINT_COORDINATE_UNKNOWN;
    } else {
        i32 chance =
            (result - MINIMUM_PUZZLE_PIECES) * HINT_CHANCE_PER_PIECE;
        if (chance > HINT_CHANCE_MAXIMUM)
            chance = HINT_CHANCE_MAXIMUM;
        if (chance < HINT_CHANCE_MINIMUM)
            chance = HINT_CHANCE_MINIMUM;
        gpGame->m_players[playerIndex].m_ultimateArtifactHintChance = static_cast<i8>(chance);

        if (Random(HINT_CHANCE_MINIMUM, HINT_CHANCE_MAXIMUM)
            <= gpGame->m_players[playerIndex].m_ultimateArtifactHintChance) {
            gpGame->m_players[playerIndex].m_ultimateArtifactHintX = gpGame->m_ultimateArtifactX;
            gpGame->m_players[playerIndex].m_ultimateArtifactHintY = gpGame->m_ultimateArtifactY;
        } else {
            i32 x = HINT_COORDINATE_UNKNOWN;
            i32 y = HINT_COORDINATE_UNKNOWN;
            i32 heading = 0;
            i32 triesCount = 0;
            while (
                !(x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT
                  && gpGame->m_worldMap.GetCell(x, y)->m_triggerType == MAP_OBJECT_NONE
                  && gpGame->m_worldMap.GetCell(x, y)->m_objectIndex == MAPCELL_SPRITE_NONE
                  && gpGame->m_worldMap.GetCell(x, y)->m_overlayIndex == MAPCELL_SPRITE_NONE
                  && giGroundToTerrain[gpGame->m_worldMap.GetCell(x, y)->m_terrainImageIndex]
                         != TERRAIN_WATER)
            ) {
                triesCount++;
                heading = 0;
                while (heading == 0)
                    heading = HINT_OFFSET_CENTER - Random(0, HINT_OFFSET_ROLL_MAXIMUM)
                                - Random(0, HINT_OFFSET_ROLL_MAXIMUM)
                                - Random(0, HINT_OFFSET_ROLL_MAXIMUM);
                x = gpGame->m_ultimateArtifactX + heading;
                heading = 0;
                while (heading == 0)
                    heading = HINT_OFFSET_CENTER - Random(0, HINT_OFFSET_ROLL_MAXIMUM)
                                - Random(0, HINT_OFFSET_ROLL_MAXIMUM)
                                - Random(0, HINT_OFFSET_ROLL_MAXIMUM);
                y = gpGame->m_ultimateArtifactY + heading;
                if (triesCount >= HINT_LOCATION_RETRY_LIMIT) {
                    x = gpGame->m_ultimateArtifactX;
                    y = gpGame->m_ultimateArtifactY;
                    goto saveLocation;
                }
            }
        saveLocation:
            gpGame->m_players[playerIndex].m_ultimateArtifactHintX = static_cast<i8>(x);
            gpGame->m_players[playerIndex].m_ultimateArtifactHintY = static_cast<i8>(y);
        }
    }
}

typedef enum PuzzleSetupConstant {
    PUZZLE_INTERPOLATION_TERM_COUNT = 2,
    PUZZLE_INTERFACE_SEED_STRIDE    = 3,
    PUZZLE_RANDOM_STEP_MAXIMUM      = 5,
    PUZZLE_FALLBACK_RETRY_LIMIT     = 100
} PuzzleSetupConstant;

i32 game::SetupPuzzlePieces(i32 player, i32 justCount) {
    i32 pieceCount = GetNumObelisks(player);
    i32 unvisitedObelisks = PUZZLE_PIECE_COUNT - m_obeliskCount;
    float fraction = GetNumObelisks(player) / static_cast<double>(m_obeliskCount);
    float interp =
        (fraction * fraction + fraction)
        / static_cast<float>(PUZZLE_INTERPOLATION_TERM_COUNT)
        ;
    pieceCount = static_cast<i32>(pieceCount + unvisitedObelisks * interp);

    if (GetNumObelisks(player) == m_obeliskCount)
        pieceCount = PUZZLE_PIECE_COUNT;
    pieceCount += m_players[player].m_cheatValue;
    if (pieceCount > PUZZLE_PIECE_COUNT)
        pieceCount = PUZZLE_PIECE_COUNT;
    if (justCount)
        return pieceCount;

    memset(puzzlePiecesRemoved, 0, PUZZLE_PIECE_STORAGE_SIZE);
    SRand(
        m_players[player].m_color
        + m_players[player].m_evilInterface * PUZZLE_INTERFACE_SEED_STRIDE
    );
    i32 targetPiece;
    i32 tries;
    i32 fallbackNum;
    i32 i;
    for (i = 0; i < pieceCount; i++) {
        for (targetPiece = 0;
             targetPiece < PUZZLE_PIECE_COUNT;
             targetPiece += SRandom(1, PUZZLE_RANDOM_STEP_MAXIMUM)) {
            if (!H2BitTest(puzzlePiecesRemoved, targetPiece))
                break;
        }

        for (tries = 0; tries < PUZZLE_FALLBACK_RETRY_LIMIT; tries++) {
            fallbackNum = SRandom(0, PUZZLE_PIECE_COUNT - 1);
            if (!H2BitTest(puzzlePiecesRemoved, fallbackNum))
                break;
        }
        if (tries >= PUZZLE_FALLBACK_RETRY_LIMIT) {
            for (fallbackNum = 0; fallbackNum < PUZZLE_PIECE_COUNT; fallbackNum++) {
                if (!H2BitTest(puzzlePiecesRemoved, fallbackNum))
                    break;
            }
        }
        if (fallbackNum >= PUZZLE_PIECE_COUNT)
            fallbackNum = 0;
        if (targetPiece < PUZZLE_PIECE_COUNT)
            H2BitSet(puzzlePiecesRemoved, targetPiece);
        else
            H2BitSet(puzzlePiecesRemoved, fallbackNum);
    }
    return pieceCount;
}

i32 game::IsMobile(i32 heroId) {
    if (heroId == -1)
        return 0;
    hero* mobileHero = &m_heroRecs[heroId];
    mapCell* cell = gpAdvManager->GetCell(mobileHero->m_x, mobileHero->m_y);
    return mobileHero->m_remainingMobility
           >= CalcTerrainCost(
               giGroundToTerrain[cell->m_terrainImageIndex],
               1,
               mobileHero->m_remainingMobility,
               H2EnumIndex(mobileHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_PATHFINDING)]),
               cell->m_isRoad,
               0
           );
}

fullMap* game::GetWorldMapData(void) {
    return &m_worldMap;
}

i32 game::CreateBoat(i32 x, i32 y, i32 notify) {
    i32 boatIdx = Scan(m_boatSlots, 0, GAME_BOAT_COUNT);
    if (boatIdx != -1) {
        if (notify == 0)
            SendMapChange(MAP_CHANGE_BUILD_BOAT, 0, x, y, MAP_CHANGE_CURRENT_PLAYER, 0, 0);
        m_boatSlots[boatIdx] = static_cast<i8>(boatIdx);
        boatRecord* boat = &m_boats[boatIdx];
        boat->id = static_cast<i8>(boatIdx);
        boat->x = static_cast<i8>(x);
        boat->y = static_cast<i8>(y);
        boat->direction = MAP_DIRECTION_EAST;
        boat->owner = static_cast<i8>(giCurPlayer);
        mapCell* square = WORLDMAP->GetCell(x, y);
        boat->savedTriggerType = square->m_triggerType;
        boat->savedEventData = static_cast<u8>(square->m_objectMetadata);
        square->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT;
        square->m_objectMetadata = boatIdx;
    }
    return boatIdx;
}

i32 game::Scan(i8* array, i32 start, i32 length) {
    i32 i;
    for (i = start; i < start + length; i++) {
        if (array[i] == -1)
            return i;
    }
    return -1;
}

i32 game::RandomScan(i8* array, i32 start, i32 range, i32 unused, i8 target) {
    i32 idx = target;
    i32 i;
    for (i = 0; i < RANDOM_SCAN_RETRY_LIMIT; i++) {
        idx = start + Random(0, range - 1);
        if (array[idx] == target)
            return idx;
    }
    return -1;
}

i32 game::GetNewHeroId(i32, FactionType heroClass, i32 requireExperienced) {
    i32 r = -1;
    i32 previousHero;
    i32 heroIdx = -1;
    i32 attempts = 0;
    i32 oldHeroId;
    while (attempts < HERO_SELECTION_RETRY_LIMIT) {
        attempts++;
        heroIdx = Random(0, H2EnumIndex(GAME_HERO_COUNT) - 1);
        if (m_availableHeroes[heroIdx] != HERO_AVAILABILITY_UNAVAILABLE
            && m_availableHeroes[heroIdx] != WEEKLY_AVAILABLE_HERO)
            continue;
        if (m_availableHeroes[heroIdx] == WEEKLY_AVAILABLE_HERO
            && attempts < HERO_SELECTION_REUSE_RETRY_LIMIT)
            continue;
        if (heroClass >= FACTION_KNIGHT && heroClass <= FACTION_NECROMANCER
            && attempts < HERO_SELECTION_FACTION_RETRY_LIMIT
            && m_heroRecs[heroIdx].m_cursorType != heroClass)
            continue;
        if (requireExperienced && attempts < HERO_SELECTION_EXPERIENCE_RETRY_LIMIT
            && m_heroRecs[heroIdx].m_experience < HERO_SELECTION_MINIMUM_EXPERIENCE
            && (m_heroRecs[heroIdx].m_artifacts[0] == ARTIFACT_NONE
                || m_heroRecs[heroIdx].m_artifacts[0] == ARTIFACT_MAGIC_BOOK)
            && (m_heroRecs[heroIdx].m_artifacts[1] == ARTIFACT_NONE
                || m_heroRecs[heroIdx].m_artifacts[1] == ARTIFACT_MAGIC_BOOK))
            continue;
        if (gbInCampaign && attempts < HERO_SELECTION_CAMPAIGN_RETRY_LIMIT
            && m_heroRecs[heroIdx].m_portrait >= CAMPAIGN_HERO_ROLAND
            && m_heroRecs[heroIdx].m_portrait <= CAMPAIGN_HERO_BRAX)
            continue;
        break;
    }
    return heroIdx;
}

i32 game::GetTownId(i32 col, i32 row) {
    i32 i;
    for (i = 0; i < GAME_TOWN_COUNT; i++) {
        if (m_castleRecs[i].m_x == col && m_castleRecs[i].m_y == row)
            return i;
    }
    return -1;
}

i32 game::GetMineId(i32 col, i32 row) {
    i32 i;
    for (i = 0; i < GAME_MINE_COUNT; i++) {
        if (m_mines[i].x == col && m_mines[i].y == row)
            return i;
    }
    return -1;
}

void GenerateStandardFileName(char* source, char* destination) {
    char* ext = FindLastToken(source, '.');
    if (ext == NULL) {
        strcpy(destination, source);
        return;
    }

    *ext = '\0';
    i32 indexOut = 0;
    i32 length = strlen(source);
    i32 i;
    char chr;
    for (i = 0; i < length; i++) {
        chr = source[i];
        if (chr >= 'a' && chr <= 'z')
            chr -= 'a' - 'A';
        if ((chr >= 'A' && chr <= 'Z') || (chr >= '0' && chr <= '9') || chr == '_') {
            destination[indexOut] = chr;
            indexOut++;
        }
        if (indexOut >= STANDARD_FILENAME_BASENAME_SIZE)
            i = 999;
    }
    *ext = '.';
    strcpy(destination + indexOut, ext);
}

i32 game::SaveGame(char* filename, i32 generateName, i8) {
    return Ironfist_SaveGame(filename, generateName);
}

void game::SetupOrigData(void) {
    ClearMapExtra();
    gbIAmGreatest = false;
    m_difficultyRating = INITIAL_DIFFICULTY_RATING;
    giMonthType = CALENDAR_PERIOD_NORMAL;
    giMonthTypeExtra = 0;
    giWeekType = CALENDAR_PERIOD_NORMAL;
    giWeekTypeExtra = 0;
    m_cheated = 0;
    gpAdvManager->PurgeMapChangeQueue();
    giMapChangeCtr = INITIAL_MAP_CHANGE_SEQUENCE;
    strcpy(m_saveName, save_names::NewGame);
    m_playerCount = INITIAL_PLAYER_COUNT;
    m_deadPlayerCount = 0;
    memset(m_playerDead, 0, sizeof(m_playerDead));
    m_month = INITIAL_CALENDAR_VALUE;
    m_week = INITIAL_CALENDAR_VALUE;
    m_day = INITIAL_CALENDAR_VALUE;
    giCurTurn = INITIAL_CALENDAR_VALUE;

    i32 i;
    i32 j;
    for (i = 0; i < GAME_PLAYER_COUNT; i++) {
        strcpy(
            m_defaultPlayerNames + i * GAME_DEFAULT_PLAYER_NAME_SIZE,
            ""
        );
        if (i < giNumHumanPlayers) {
            if (i == 0 || iMPBaseType == MULTIPLAYER_BASE_HOT_SEAT)
                gbThisNetHumanPlayer[i] = 1;
            else
                gbThisNetHumanPlayer[i] = 0;
            gbHumanPlayer[i] = 1;
        } else {
            gbThisNetHumanPlayer[i] = 0;
            gbHumanPlayer[i] = 0;
        }
        memset(&m_players[i], 0, sizeof(m_players[i]));
        m_players[i].m_color = static_cast<i8>(i);
        m_players[i].m_heroCount = 0;
        m_players[i].m_townCount = 0;
        m_players[i].m_daysLeft = -1;
        m_players[i].m_cheatValue = 0;
        memset(m_players[i].m_availableHeroIds, -1, sizeof(m_players[i].m_availableHeroIds));
        memset(m_players[i].m_heroIds, -1, sizeof(m_players[i].m_heroIds));
        memset(m_players[i].m_townIds, -1, sizeof(m_players[i].m_townIds));
    }

    m_obeliskCount = 0;
    gpAdvManager->m_heroContextLocked = 0;
    memset(m_availableHeroes, HERO_AVAILABILITY_UNAVAILABLE, sizeof(m_availableHeroes));
    for (i = 0; i < GAME_HERO_COUNT; i++) {
        memset(&m_heroRecs[i], 0, sizeof(m_heroRecs[i]));
        memset(m_heroRecs[i].m_spells, 0, sizeof(m_heroRecs[i].m_spells));
        memset(m_heroRecs[i].m_artifacts, H2EnumIndex(ARTIFACT_NONE), sizeof(m_heroRecs[i].m_artifacts));
        m_heroRecs[i].m_patrolY = HERO_DESTINATION_NONE;
        m_heroRecs[i].m_patrolX = HERO_DESTINATION_NONE;
        m_heroRecs[i].m_id = static_cast<i8>(i);
        m_heroRecs[i].m_portrait = static_cast<u8>(i);
        m_heroRecs[i].m_owner = HERO_OWNER_NONE;
        m_heroRecs[i].m_direction = MAP_DIRECTION_EAST;
        utf8::Copy(
            m_heroRecs[i].m_name,
            sizeof(m_heroRecs[i].m_name),
            gHeroDefaultNames[i]
        );
        m_heroRecs[i].m_cursorType = static_cast<FactionType>(i / INITIAL_RECORD_TYPE_STRIDE);
        for (j = 0; j < HERO_STARTING_STAT_COUNT; j++)
            m_heroRecs[i].m_primaryStats[j] =
                gStartingHeroStats[H2EnumIndex(m_heroRecs[i].m_cursorType)][j];
        for (j = 0; j < ARMY_GROUP_SLOT_COUNT; j++)
            m_heroRecs[i].m_army.m_creatureTypes[j] = CREATURE_NONE;
        m_heroRecs[i].m_destinationY = HERO_DESTINATION_NONE;
        m_heroRecs[i].m_destinationX = HERO_DESTINATION_NONE;
        m_heroRecs[i].m_level = HERO_INITIAL_LEVEL;
        m_heroRecs[i].m_spellPoints =
            m_heroRecs[i].Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE;
        m_heroRecs[i].m_secondarySkillCount = 0;
        for (j = 0; j < H2EnumIndex(HERO_SKILL_COUNT); j++) {
            m_heroRecs[i].m_secondarySkills[j] = HERO_SKILL_LEVEL_NONE;
            m_heroRecs[i].m_secondarySkillOrder[j] = 0;
        }
        if (m_heroRecs[i].m_cursorType == FACTION_KNIGHT) {
            m_heroRecs[i].GiveSS(HERO_SKILL_LEADERSHIP, HERO_SKILL_LEVEL_BASIC);
            m_heroRecs[i].GiveSS(HERO_SKILL_BALLISTICS, HERO_SKILL_LEVEL_BASIC);
        }
        if (m_heroRecs[i].m_cursorType == FACTION_SORCERESS) {
            m_heroRecs[i].m_artifacts[0] = ARTIFACT_MAGIC_BOOK;
            m_heroRecs[i].GiveSS(HERO_SKILL_NAVIGATION, HERO_SKILL_LEVEL_ADVANCED);
            m_heroRecs[i].GiveSS(HERO_SKILL_WISDOM, HERO_SKILL_LEVEL_BASIC);
        }
        if (m_heroRecs[i].m_cursorType == FACTION_BARBARIAN)
            m_heroRecs[i].GiveSS(HERO_SKILL_PATHFINDING, HERO_SKILL_LEVEL_ADVANCED);
        if (m_heroRecs[i].m_cursorType == FACTION_WARLOCK) {
            m_heroRecs[i].m_artifacts[0] = ARTIFACT_MAGIC_BOOK;
            m_heroRecs[i].GiveSS(HERO_SKILL_SCOUTING, HERO_SKILL_LEVEL_ADVANCED);
            m_heroRecs[i].GiveSS(HERO_SKILL_WISDOM, HERO_SKILL_LEVEL_BASIC);
        }
        if (m_heroRecs[i].m_cursorType == FACTION_WIZARD) {
            m_heroRecs[i].m_artifacts[0] = ARTIFACT_MAGIC_BOOK;
            m_heroRecs[i].GiveSS(HERO_SKILL_WISDOM, HERO_SKILL_LEVEL_ADVANCED);
        }
        if (m_heroRecs[i].m_cursorType == FACTION_NECROMANCER) {
            m_heroRecs[i].m_artifacts[0] = ARTIFACT_MAGIC_BOOK;
            m_heroRecs[i].GiveSS(HERO_SKILL_WISDOM, HERO_SKILL_LEVEL_BASIC);
            m_heroRecs[i].GiveSS(HERO_SKILL_NECROMANCY, HERO_SKILL_LEVEL_BASIC);
        }
    }

    memset(m_castleOwners, TOWN_OWNER_NONE, sizeof(m_castleOwners));
    for (i = 0; i < GAME_TOWN_COUNT; i++) {
        memset(&m_castleRecs[i], 0, sizeof(m_castleRecs[i]));
        m_castleRecs[i].m_onMap = 0;
        m_castleRecs[i].m_id = static_cast<u8>(i);
        m_castleRecs[i].m_owner = TOWN_OWNER_NONE;
        m_castleRecs[i].m_type = static_cast<FactionType>(i / INITIAL_RECORD_TYPE_STRIDE);
        m_castleRecs[i].m_occupyingHeroId = TOWN_OCCUPYING_HERO_NONE;
        for (j = 0; j < ARMY_GROUP_SLOT_COUNT; j++)
            m_castleRecs[i].m_army.m_creatureTypes[j] = CREATURE_NONE;
    }
    for (i = 0; i < GAME_MINE_COUNT; i++)
        memset(&m_mines[i], -1, sizeof(m_mines[i]));
    memset(m_mineOwners, -1, sizeof(m_mineOwners));
    for (i = 0; i < GAME_BOAT_COUNT; i++) {
        memset(&m_boats[i], 0, sizeof(m_boats[i]));
        m_boats[i].id = static_cast<i8>(i);
        m_boats[i].heroId = BOAT_HERO_NONE;
    }
    memset(m_dailyEventFlags, 0, sizeof(m_dailyEventFlags));
    memset(m_boatSlots, BOAT_SLOT_EMPTY, sizeof(m_boatSlots));
    m_ultimateArtifactY = HINT_COORDINATE_UNKNOWN;
    m_ultimateArtifactX = HINT_COORDINATE_UNKNOWN;
    memset(m_obeliskVisitors, 0, sizeof(m_obeliskVisitors));
    strcpy(gpGame->m_saveName, save_names::NewGame);
    giCurPlayer = 0;
    gpCurPlayer = &gpGame->m_players[giCurPlayer];
    giCurPlayerBit = static_cast<u8>(1 << giCurPlayer);
    giCurWatchPlayer = giCurPlayer;
    while (!gbThisNetHumanPlayer[giCurWatchPlayer])
        giCurWatchPlayer = (giCurWatchPlayer + 1) % m_playerCount;
    giCurWatchPlayerBit = static_cast<u8>(1 << giCurWatchPlayer);
    gpAdvManager->CheckSetEvilInterface(0, -1);
    bShowIt = gbThisNetHumanPlayer[giCurPlayer];
}

void game::LoadGame(char* filename, i32 loadFromFile, i32) {
    char workData[SAVE_LEGACY_CLEAR_SIZE];
    i32 oldFlag;
    char isHuman[SAVE_PLAYER_FLAGS_SCRATCH_SIZE];
    i32 saveVal;
    i32 rows;
    char pathBuf[SAVE_PATH_CAPACITY];
    i8 expTag;
    i32 fd;
    char junkBuf[SAVE_LEGACY_CLEAR_SIZE];
    i32 ndx;
    char plBuf[LOAD_CURRENT_PLAYER_SCRATCH_SIZE];
    char chunkTag[LOAD_CURRENT_PLAYER_SCRATCH_SIZE];
    i32 numHumans;
    i32 wide;

    LogStr("LG1");
    if (Ironfist_LoadGame(filename, !loadFromFile))
        return;
    if (loadFromFile) {
        SetupOrigData();
        return;
    }
    LogStr("LG2");
    numHumans = 0;
    gbGameOver = false;
    m_gameLoaded = 1;

    if (loadFromFile || platform::CompareIgnoringCase(filename, "RMT", sizeof("RMT") - 1) == 0)
        sprintf(pathBuf, "%s%s", ".\\DATA\\", filename);
    else
        sprintf(pathBuf, "%s%s", gcGamePath, filename);

    fd = platform::FileOpen(pathBuf, platform::FileMode::Read);
    if (fd == -1)
        FileError(pathBuf);
    ClearMapExtra();

    expTag = 0;
    platform::FileRead(fd, &wide, sizeof(wide));
    if (wide == -1) {
        expTag = 1;
        platform::FileRead(fd, &wide, sizeof(wide));
    }
    platform::FileRead(fd, &rows, sizeof(rows));
    SetMapSize(wide, rows);
    platform::FileRead(fd, &m_mapHeader, sizeof(m_mapHeader));
    platform::FileRead(fd, m_setupPlayerColor, CAMPAIGN_SETUP_RESET_SIZE);
    platform::FileRead(fd, &gbIAmGreatest, SAVE_TRUNCATED_SCALAR_SIZE);
    platform::FileRead(fd, this, sizeof(m_difficultyRating));
    platform::FileRead(fd, &giMonthType, SAVE_TRUNCATED_SCALAR_SIZE);
    platform::FileRead(fd, &giMonthTypeExtra, SAVE_TRUNCATED_SCALAR_SIZE);
    platform::FileRead(fd, &giWeekType, SAVE_TRUNCATED_SCALAR_SIZE);
    platform::FileRead(fd, &giWeekTypeExtra, SAVE_TRUNCATED_SCALAR_SIZE);
    platform::FileRead(fd, cPlayerNames, sizeof(cPlayerNames));
    {
        const char* provenanceFields[GAME_PLAYER_COUNT + 2] = {
            m_mapHeader.name,
            m_mapHeader.description,
        };
        for (ndx = 0; ndx < GAME_PLAYER_COUNT; ++ndx) {
            provenanceFields[ndx + 2] = cPlayerNames[ndx];
        }
        localization::SetCurrentFileTextEncoding(
            localization::DetectTextEncoding(
                provenanceFields,
                sizeof(provenanceFields) / sizeof(provenanceFields[0]),
                localization::DefaultFileTextEncoding()
            )
        );
        platform::Host().Log(
            platform::LogLevel::Info,
            (std::string("localization: save-text=")
             + localization::TextEncodingName(
                 localization::CurrentFileTextEncoding()
             )).c_str()
        );
    }
    for (auto& playerName : cPlayerNames) {
        const std::string decodedName = localization::DecodeExternalText(playerName);
        utf8::Copy(playerName, sizeof(playerName), decodedName.c_str());
    }

    platform::FileRead(fd, workData, SAVE_LEGACY_SERIALIZED_SIZE);
    platform::FileRead(fd, &gbInCampaign, sizeof(gbInCampaign));
    if (gbInCampaign == 1) {
        platform::FileRead(fd, &m_campaignType, CAMPAIGN_STATE_RESET_SIZE);
    } else if (gbInCampaign == SAVE_EXPANSION_CAMPAIGN_FORMAT_TAG) {
        xIsPlayingExpansionCampaign = 1;
        gbInCampaign = false;
        platform::FileRead(fd, &xCampaign, CAMPAIGN_SAVE_PREFIX_SIZE);
    }
    if (expTag)
        platform::FileRead(fd, &xIsExpansionMap, sizeof(xIsExpansionMap));

    gpAdvManager->PurgeMapChangeQueue();
    platform::FileRead(fd, &giMapChangeCtr, sizeof(giMapChangeCtr));
    platform::FileRead(fd, workData, SAVE_STANDARD_FILENAME_SIZE);
    if (platform::CompareIgnoringCase(filename, "RMT", sizeof("RMT") - 1) != 0)
        sprintf(gpGame->m_saveName, filename);
    platform::FileRead(fd, &m_playerCount, sizeof(m_playerCount));

    platform::FileRead(fd, plBuf, sizeof(plBuf[0]));
    giCurPlayer = plBuf[0];
    platform::FileRead(fd, &m_deadPlayerCount, sizeof(m_deadPlayerCount));
    platform::FileRead(fd, m_playerDead, sizeof(m_playerDead));

    platform::FileRead(fd, isHuman, GAME_PLAYER_COUNT);
    for (ndx = 0; ndx < GAME_PLAYER_COUNT; ndx++) {
        if (isHuman[ndx] && numHumans < giNumHumanPlayers) {
            numHumans++;
            gbHumanPlayer[ndx] = 1;
        } else {
            gbHumanPlayer[ndx] = 0;
        }
    }
    for (ndx = 0; ndx < GAME_PLAYER_COUNT; ndx++) {
        if (gbHumanPlayer[ndx]) {
            if (!gbRemoteOn || ndx == giThisGamePos)
                gbThisNetHumanPlayer[ndx] = 1;
            else
                gbThisNetHumanPlayer[ndx] = 0;
        } else {
            gbThisNetHumanPlayer[ndx] = 0;
        }
    }

    platform::FileRead(fd, &m_day, sizeof(m_day));
    platform::FileRead(fd, &m_week, sizeof(m_week));
    platform::FileRead(fd, &m_month, sizeof(m_month));
    giCurTurn = m_day + (m_week - 1) * EVENT_DAYS_PER_WEEK
                + (m_month - 1) * EVENT_DAYS_PER_MONTH;
    for (ndx = 0; ndx < GAME_PLAYER_COUNT; ndx++)
        m_players[ndx].Read(fd);

    platform::FileRead(fd, &m_obeliskCount, sizeof(m_obeliskCount));
    for (ndx = 0; ndx < GAME_HERO_COUNT; ndx++)
        m_heroRecs[ndx].Read(fd, expTag);
    platform::FileRead(fd, m_availableHeroes, sizeof(m_availableHeroes));
    platform::FileRead(fd, m_castleRecs, sizeof(m_castleRecs));
    for (town& castle : m_castleRecs) {
        const std::string decodedName = localization::DecodeExternalText(castle.m_name);
        utf8::Copy(castle.m_name, sizeof(castle.m_name), decodedName.c_str());
    }
    platform::FileRead(fd, m_castleOwners, sizeof(m_castleOwners));
    platform::FileRead(fd, m_dailyEventFlags, sizeof(m_dailyEventFlags));
    platform::FileRead(fd, m_mines, sizeof(m_mines));
    platform::FileRead(fd, m_mineOwners, sizeof(m_mineOwners));
    if (expTag)
        platform::FileRead(fd, m_randomArtifacts, H2EnumIndex(ARTIFACT_COUNT));
    else
        platform::FileRead(fd, m_randomArtifacts, ARTIFACT_BASE_TABLE_SIZE);
    platform::FileRead(fd, m_boats, sizeof(m_boats));
    platform::FileRead(fd, m_boatSlots, sizeof(m_boatSlots));
    platform::FileRead(fd, m_obeliskVisitors, sizeof(m_obeliskVisitors));
    platform::FileRead(fd, &m_ultimateArtifactX, sizeof(m_ultimateArtifactX));
    platform::FileRead(fd, &m_ultimateArtifactY, sizeof(m_ultimateArtifactY));
    platform::FileRead(fd, &m_ultimateArtifactId, sizeof(m_ultimateArtifactId));
    platform::FileRead(fd, m_rumour, sizeof(m_rumour));
    {
        const std::string decodedRumour = localization::DecodeExternalText(m_rumour);
        utf8::Copy(m_rumour, sizeof(m_rumour), decodedRumour.c_str());
    }
    platform::FileRead(fd, m_defaultPlayerNames, sizeof(m_defaultPlayerNames));
    platform::FileRead(fd, &m_rumourEventCount, SAVE_EVENT_HEADER_SIZE);
    platform::FileRead(
        fd,
        m_rumourEventIndices,
        m_rumourEventCount * sizeof(m_rumourEventIndices[0])
    );
    platform::FileRead(fd, &m_timeEventCount, SAVE_EVENT_HEADER_SIZE);
    platform::FileRead(fd, m_timeEventIndices, m_timeEventCount * sizeof(m_timeEventIndices[0]));
    platform::FileRead(fd, &m_mapEventCount, SAVE_EVENT_HEADER_SIZE);
    platform::FileRead(fd, m_mapEventIndices, m_mapEventCount * sizeof(m_mapEventIndices[0]));

    platform::FileRead(fd, chunkTag, sizeof(i32));
    platform::FileRead(fd, &iMaxMapExtra, sizeof(iMaxMapExtra));
    platform::FileRead(fd, chunkTag, sizeof(i32));
    ppMapExtra = reinterpret_cast<void**>(
        H2_ALLOC(iMaxMapExtra * sizeof(*ppMapExtra))
    );
    pwSizeOfMapExtra = reinterpret_cast<i16*>(
        H2_ALLOC(iMaxMapExtra * sizeof(*pwSizeOfMapExtra))
    );
    memset(ppMapExtra, 0, iMaxMapExtra * sizeof(*ppMapExtra));
    memset(pwSizeOfMapExtra, 0, iMaxMapExtra * sizeof(*pwSizeOfMapExtra));
    for (ndx = 1; ndx < iMaxMapExtra; ndx++) {
        platform::FileRead(fd, chunkTag, sizeof(i32));
        platform::FileRead(fd, pwSizeOfMapExtra + ndx, sizeof(pwSizeOfMapExtra[ndx]));
        ppMapExtra[ndx] = H2_ALLOC(pwSizeOfMapExtra[ndx]);
        platform::FileRead(fd, ppMapExtra[ndx], pwSizeOfMapExtra[ndx]);
    }
    platform::FileRead(fd, chunkTag, sizeof(i32));
    platform::FileRead(fd, mapExtra, MAP_WIDTH * MAP_HEIGHT);
    platform::FileRead(fd, chunkTag, sizeof(i32));
    m_worldMap.Read(fd, 0);
    platform::FileRead(fd, chunkTag, sizeof(i32));
    platform::FileClose(fd);

    gpAdvManager->m_heroContextLocked = 0;
    gpCurPlayer = &gpGame->m_players[giCurPlayer];
    giCurPlayerBit = static_cast<u8>(1 << giCurPlayer);
    giCurWatchPlayer = giCurPlayer;
    while (!gbThisNetHumanPlayer[giCurWatchPlayer])
        giCurWatchPlayer = (giCurWatchPlayer + 1) % m_playerCount;
    giCurWatchPlayerBit = static_cast<u8>(1 << giCurWatchPlayer);
    bShowIt = gbThisNetHumanPlayer[giCurPlayer];
    SetupAdjacentMons();
    LogStr("LG3");
    gpAdvManager->CheckSetEvilInterface(0, -1);
}

void game::GiveTroopsToNeutralTown(i32 townId) {
    town* neutralTown = &m_castleRecs[townId];
    if ((neutralTown->m_x <= 0 && neutralTown->m_y <= 0) || neutralTown->m_owner >= 0)
        return;

    i32 roll = Random(REINFORCEMENT_ROLL_MIN, REINFORCEMENT_ROLL_MAX);
    const i32 turnBonus = giCurTurn / REINFORCEMENT_TURN_ROLL_DIVISOR;
    if (turnBonus != 0)
        roll += Random(0, turnBonus);

    i32 tier;
    i32 creatureCount;
    if (roll <= REINFORCEMENT_TIER_ONE_THRESHOLD) {
        tier = 0;
        creatureCount = Random(REINFORCEMENT_TIER_ONE_COUNT_MIN, REINFORCEMENT_TIER_ONE_COUNT_MAX);
    } else if (roll <= REINFORCEMENT_TIER_TWO_THRESHOLD) {
        tier = 1;
        creatureCount = Random(REINFORCEMENT_TIER_TWO_COUNT_MIN, REINFORCEMENT_TIER_TWO_COUNT_MAX);
    } else if (roll <= REINFORCEMENT_TIER_THREE_THRESHOLD) {
        tier = 2;
        creatureCount = Random(REINFORCEMENT_TIER_THREE_COUNT_MIN, REINFORCEMENT_TIER_THREE_COUNT_MAX);
    } else if (roll <= REINFORCEMENT_TIER_FOUR_THRESHOLD) {
        tier = 3;
        creatureCount = Random(REINFORCEMENT_TIER_FOUR_COUNT_MIN, REINFORCEMENT_TIER_FOUR_COUNT_MAX);
    } else {
        tier = 4;
        creatureCount = REINFORCEMENT_TIER_FIVE_COUNT;
    }

    creatureCount += giCurTurn / REINFORCEMENT_TURN_COUNT_DIVISOR;
    const CreatureType creature =
        neutralTownCreatureTypes[H2EnumIndex(neutralTown->m_type)][tier];
    if (H2EnumIndex(creature) != CREATURE_INVALID_ID)
        GiveArmy(&neutralTown->m_army, creature, creatureCount, ARMY_GROUP_EMPTY_SLOT);
}

void game::GiveTroopsToNeutralTowns(void) {
    i32 i;
    for (i = 0; i < GAME_TOWN_COUNT; i++) {
        GiveTroopsToNeutralTown(i);
        if (m_castleRecs[i].m_buildings & H2EnumIndex(TOWN_BUILDING_CASTLE)) {
            if (Random(0, REINFORCEMENT_ROLL_PERCENT_MAX) < REINFORCEMENT_CASTLE_CHANCE)
                GiveTroopsToNeutralTown(i);
        } else {
            if (Random(0, REINFORCEMENT_ROLL_PERCENT_MAX) < REINFORCEMENT_CHANCE)
                GiveTroopsToNeutralTown(i);
        }
    }
}

void game::NewMap(char* filename) {
    FactionType sideClass;
    char* heroName;
    HeroPortrait curPic;
    FactionType specClass;
    i32 awardHero;
    i32 padNum;
    i32 selectedTown;
    i32 heroIndex;
    i32 humanPos;
    FactionType startClass;
    i32 nTown;
    i32 player;
    i32 yPos;
    i32 ultimateTries;
    i32 xPos;
    FactionType race;
    char* dotPos;
    i32 junkVal;
    i32 iPass;
    i32 ultimateDistance;
    i32 townIndex;

    if (!gbInCampaign && !xIsPlayingExpansionCampaign) {
        write_pref(
            xIsExpansionMap ? "Last Map expansion" : "Last Map",
            std::string(filename)
        );
    }
    for (player = 0; player < GAME_PLAYER_COUNT; player++) {
        if (m_setupPlayerRace[player] == FACTION_RANDOM) {
            m_setupPlayerRace[player] =
                FACTIONS_ACTUAL[Random(0, FACTIONS_ACTUAL.size() - 1)];
        }
    }
    Ironfist_NewMap(filename);

    dotPos = FindLastToken(gMapName, '.');
    if (dotPos != NULL && StrEqNoCase(dotPos + 1, "MX2"))
        xIsExpansionMap = 1;
    if (xIsExpansionMap)
        gTownEligibleBuildMask[H2EnumIndex(FACTION_NECROMANCER)] |= NECROMANCER_SHRINE_BUILD_MASK;
    else
        gTownEligibleBuildMask[H2EnumIndex(FACTION_NECROMANCER)] &= ~NECROMANCER_SHRINE_BUILD_MASK;

    gbInNewGameSetup = true;
    giCurPlayer = 0;
    gpCurPlayer = &gpGame->m_players[giCurPlayer];
    giCurPlayerBit = static_cast<u8>(1 << giCurPlayer);
    giCurWatchPlayerBit = giCurPlayerBit;
    giCurWatchPlayer = giCurPlayer;
    race = static_cast<FactionType>(Random(0, GAME_PLAYER_COUNT - 1));
    humanPos = giNumHumanPlayers;

    for (player = 0; player < GAME_PLAYER_COUNT; player++) {
        if (player >= gpGame->m_mapHeader.playerCount) {
            gbSetupGamePosToRealGamePos[player] = -1;
        } else {
            if (m_setupPlayerNetworkId[player] == GAME_COMPUTER_PLAYER)
                gbSetupGamePosToRealGamePos[player] = static_cast<i8>(humanPos++);
            else
                gbSetupGamePosToRealGamePos[player] = m_setupPlayerNetworkId[player];
        }
    }
    for (player = 0; player < GAME_PLAYER_COUNT; player++) {
        m_players[player].m_color = -1;
        gcColorToPlayerPos[player] = -1;
        gcColorToSetupPos[player] = -1;
        if (gpGame->m_setupPlayerRace[player] == FACTION_RANDOM)
            gpGame->m_setupPlayerRace[player] = race;
        race = (race + 1) % GAME_PLAYER_COUNT;
    }
    for (player = 0; player < m_playerCount; player++)
        gcColorToSetupPos[m_setupPlayerColor[player]] = static_cast<i8>(player);
    for (player = 0; player < m_playerCount; player++)
        m_players[gbSetupGamePosToRealGamePos[player]].m_color = m_setupPlayerColor[player];
    for (player = 0; player < m_playerCount; player++)
        gcColorToPlayerPos[m_players[player].m_color] = static_cast<i8>(player);
    for (player = 0; player < m_playerCount; player++) {
        m_players[player].m_townCount = 0;
        m_players[player].m_townLocatorPage = 0;
        m_players[player].m_currentTown = -1;
        m_players[player].m_heroCount = 0;
        m_players[player].m_heroLocatorPage = 0;
        m_players[player].m_currentHero = -1;
    }

    RandomizeHeroPool();
    strcpy(gMapName, filename);
    LoadMap(gMapName);
    InitRandomArtifacts();
    ProcessRandomObjects();
    ProcessMapExtra();
    SetupTowns();
    InitializePasswords();
    for (player = 0; player < GAME_PLAYER_COUNT; player++)
        m_players[player].m_barrierTents = 0;
    RandomizeEvents();
    ProcessOnMapHeroes();
    m_deadPlayerCount = 0;
    for (player = m_playerCount; player < GAME_PLAYER_COUNT; player++)
        m_playerDead[player] = 1;

    if (m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_SIDE
        || m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_HERO) {
        m_mapHeader.computerAlsoWins = 1;
        m_mapHeader.allowNormalVictory = 0;
    }
    if (m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_SIDE) {
        nTown = 0;
        for (player = 0; player < GAME_PLAYER_COUNT; player++) {
            if (m_mapHeader.playerEnabled[player] != 0)
                nTown++;
            if (nTown == m_mapHeader.victoryConditionValue + 1) {
                m_mapHeader.victorySideThreshold = static_cast<u16>(player);
                player = VICTORY_SIDE_SEARCH_DONE;
            }
        }
    }
    if (m_mapHeader.victoryCondition == MAP_VICTORY_FIND_ARTIFACT)
        m_mapHeader.computerAlsoWins = 1;

    for (player = 0; player < m_playerCount; player++) {
        m_players[player].m_ultimateArtifactHintChance = 0;
        m_players[player].m_ultimateArtifactHintX = -1;
        m_players[player].m_ultimateArtifactHintY = -1;
        heroIndex = 0;
        selectedTown = -1;
        if (m_mapHeader.unknown25 == 0 && m_players[player].m_townCount > 0) {
            for (iPass = 0; iPass < STARTING_HERO_TOWN_PASS_COUNT; iPass++) {
                for (nTown = 0; nTown < m_players[player].m_townCount; nTown++) {
                    if (selectedTown == -1
                        && m_castleRecs[(m_players + player)->m_townIds[nTown]]
                                   .m_occupyingHeroId
                               == -1
                        && ((m_castleRecs[(m_players + player)->m_townIds[nTown]].m_buildings
                             & H2EnumIndex(TOWN_BUILDING_CASTLE))
                                != 0
                            || iPass == STARTING_HERO_ALLOW_NON_CASTLE_PASS))
                        selectedTown = nTown;
                }
            }
        }
        if (selectedTown != -1) {
            m_players[player].m_heroIds[m_players[player].m_heroCount] =
                static_cast<i8>(GetNewHeroId(
                    player,
                    m_castleRecs[m_players[player].m_townIds[selectedTown]].m_type,
                    0
                ));
            m_availableHeroes[m_players[player].m_heroIds[m_players[player].m_heroCount]] =
                static_cast<i8>(player);
            m_heroRecs[m_players[player].m_heroIds[m_players[player].m_heroCount]].m_owner =
                static_cast<i8>(player);
            m_heroRecs[m_players[player].m_heroIds[m_players[player].m_heroCount]].m_x =
                m_castleRecs[m_players[player].m_townIds[selectedTown]].m_x;
            m_heroRecs[m_players[player].m_heroIds[m_players[player].m_heroCount]].m_y =
                m_castleRecs[m_players[player].m_townIds[selectedTown]].m_y;
            m_castleRecs[m_players[player].m_townIds[selectedTown]].m_occupyingHeroId =
                m_players[player].m_heroIds[m_players[player].m_heroCount];
            SetVisibility(
                m_heroRecs[m_players[player].m_heroIds[m_players[player].m_heroCount]].m_x,
                m_heroRecs[m_players[player].m_heroIds[m_players[player].m_heroCount]].m_y,
                player,
                giVisRange[H2EnumIndex(m_heroRecs[m_players[player].m_heroIds[0]]
                                   .m_secondarySkills[H2EnumIndex(HERO_SKILL_SCOUTING)])]
            );
            m_players[player].m_heroCount++;
        }
    }

    for (player = 0; player < m_playerCount; player++) {
        if (player == 0 && gbInCampaign
            && (m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_SORCERESS_GUILD)] != 0
                || m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_NECROMANCER_GUILD)] != 0)) {
            specClass = m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_SORCERESS_GUILD)]
                                  ? FACTION_SORCERESS
                                  : FACTION_NECROMANCER;
            for (awardHero = 0; awardHero < GAME_HERO_COUNT; awardHero++) {
                if (m_heroRecs[awardHero].m_cursorType == specClass
                    && m_availableHeroes[awardHero] == -1)
                    break;
            }
            if (awardHero < GAME_HERO_COUNT) {
                if (m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_SORCERESS_GUILD)] != 0) {
                    m_heroRecs[awardHero].m_experience += CAMPAIGN_EXPERIENCE_BONUS;
                    m_heroRecs[awardHero].CheckLevel();
                    utf8::Copy(
                        m_heroRecs[awardHero].m_name,
                        sizeof(m_heroRecs[awardHero].m_name),
                        localization::Tr("campaign.hero.sister_eliza")
                    );
                    m_heroRecs[awardHero].m_portrait = CAMPAIGN_HERO_ELIZA;
                } else {
                    m_heroRecs[awardHero].m_experience += CAMPAIGN_EXPERIENCE_BONUS;
                    m_heroRecs[awardHero].CheckLevel();
                    utf8::Copy(
                        m_heroRecs[awardHero].m_name,
                        sizeof(m_heroRecs[awardHero].m_name),
                        localization::Tr("campaign.hero.brother_brax")
                    );
                    m_heroRecs[awardHero].m_portrait = CAMPAIGN_HERO_BRAX;
                }
                m_players[player].m_availableHeroIds[0] = static_cast<char>(awardHero);
                m_availableHeroes[m_players[player].m_availableHeroIds[0]] =
                    WEEKLY_AVAILABLE_HERO;
                startClass = m_heroRecs[awardHero].m_cursorType;
                goto secondHero;
            }
        }
        {
            if (xIsPlayingExpansionCampaign && player == 0) {
                specClass = FACTION_ANY;
                if (xCampaign.HasAward(AWARD_WAYWARD_SON)) {
                    specClass = FACTION_WIZARD;
                    heroName = xCampaign.JosephName();
                    curPic = EXPANSION_HERO_JOSEPH_PORTRAIT;
                } else if (xCampaign.HasAward(AWARD_UNCLE_IVAN)) {
                    specClass = FACTION_BARBARIAN;
                    heroName = xCampaign.IvanName();
                    curPic = EXPANSION_HERO_IVAN_PORTRAIT;
                }
                if (specClass != FACTION_ANY) {
                    for (awardHero = 0; awardHero < GAME_HERO_COUNT; awardHero++) {
                        if (m_heroRecs[awardHero].m_cursorType == specClass
                            && m_availableHeroes[awardHero] == -1)
                            break;
                    }
                    if (awardHero < GAME_HERO_COUNT) {
                        m_heroRecs[awardHero].m_experience = CAMPAIGN_EXPERIENCE_BONUS;
                        m_heroRecs[awardHero].CheckLevel();
                        utf8::Copy(
                            m_heroRecs[awardHero].m_name,
                            sizeof(m_heroRecs[awardHero].m_name),
                            heroName
                        );
                        m_heroRecs[awardHero].m_portrait = curPic;
                        m_players[player].m_availableHeroIds[0] =
                            static_cast<char>(awardHero);
                        m_availableHeroes[m_players[player].m_availableHeroIds[0]] =
                            WEEKLY_AVAILABLE_HERO;
                        startClass = m_heroRecs[awardHero].m_cursorType;
                        goto secondHero;
                    }
                }
            }
            startClass = static_cast<FactionType>(Random(0, H2EnumIndex(FACTION_COUNT) - 1));
            if (m_setupPlayerRace[gcColorToSetupPos[m_players[player].m_color]]
                < FACTION_COUNT)
                startClass = m_setupPlayerRace[gcColorToSetupPos[m_players[player].m_color]];
            m_players[player].m_availableHeroIds[0] =
                static_cast<char>(GetNewHeroId(player, startClass, 0));
            m_availableHeroes[m_players[player].m_availableHeroIds[0]] = WEEKLY_AVAILABLE_HERO;
        }
    secondHero:
        startClass = (startClass + Random(1, H2EnumIndex(FACTION_COUNT) - 1)) % H2EnumIndex(FACTION_COUNT);
        m_players[player].m_availableHeroIds[1] =
            static_cast<char>(GetNewHeroId(player, startClass, 0));
        m_availableHeroes[m_players[player].m_availableHeroIds[1]] = WEEKLY_AVAILABLE_HERO;
    }

    for (player = 0; player < m_playerCount; player++) {
        for (nTown = 0; nTown < m_players[player].m_heroCount;
             nTown++) {
            xPos = m_heroRecs[m_players[player].m_heroIds[nTown]].m_x;
            yPos = m_heroRecs[m_players[player].m_heroIds[nTown]].m_y;
            m_heroRecs[m_players[player].m_heroIds[nTown]].m_locationType =
                m_worldMap.GetCell(xPos, yPos)->m_triggerType;
            m_heroRecs[m_players[player].m_heroIds[nTown]].m_occupiedTown =
                m_worldMap.GetCell(xPos, yPos)->m_objectMetadata;
            m_worldMap.GetCell(xPos, yPos)->m_triggerType =
                MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION;
            m_worldMap.GetCell(xPos, yPos)->m_objectMetadata =
                m_players[player].m_heroIds[nTown];
        }
        if (m_players[player].m_heroCount > 0)
            m_players[player].m_currentHero = m_players[player].m_heroIds[0];
        else if (m_players[player].m_townCount > 0)
            m_players[player].m_currentTown = m_players[player].m_townIds[0];
    }

    player = -1;
    nTown = -1;
    ultimateTries = 0;
    ultimateDistance =
        Random(ULTIMATE_DISTANCE_ROLL_MIN, ULTIMATE_DISTANCE_COMMON_ROLL_MAX)
        + Random(ULTIMATE_DISTANCE_ROLL_MIN, ULTIMATE_DISTANCE_COMMON_ROLL_MAX)
        + Random(ULTIMATE_DISTANCE_ROLL_MIN, ULTIMATE_DISTANCE_BONUS_ROLL_MAX);
    while (player < ULTIMATE_ARTIFACT_BORDER_MARGIN
           || nTown < ULTIMATE_ARTIFACT_BORDER_MARGIN
           || player > MAP_WIDTH - ULTIMATE_ARTIFACT_BORDER_MARGIN - 1
           || nTown > MAP_HEIGHT - ULTIMATE_ARTIFACT_BORDER_MARGIN - 1
           || m_worldMap.GetCell(player, nTown)->m_objectIndex != MAPCELL_SPRITE_NONE
           || m_worldMap.GetCell(player, nTown)->m_overlayIndex != MAPCELL_SPRITE_NONE
           || giGroundToTerrain[m_worldMap.GetCell(player, nTown)->m_terrainImageIndex]
                  == TERRAIN_WATER
           || (giNumHumanPlayers == 1
               && ultimateTries < ULTIMATE_HUMAN_DISTANCE_RETRY_LIMIT
               && ultimateDistance
                      >= abs(player - m_heroRecs[m_players[0].m_heroIds[0]].m_x)
                             + abs(nTown - m_heroRecs[m_players[0].m_heroIds[0]].m_y))) {
        if (ultimateTries < ULTIMATE_SEARCH_REGION_RETRY_LIMIT && giUABaseX > 0) {
            player = giUABaseX + (giUARadius != 0 ? Random(-giUARadius, giUARadius) : 0);
            nTown = giUABaseY + (giUARadius != 0 ? Random(-giUARadius, giUARadius) : 0);
        } else {
            player = Random(
                ULTIMATE_ARTIFACT_BORDER_MARGIN,
                MAP_WIDTH - ULTIMATE_ARTIFACT_BORDER_MARGIN - 1
            );
            nTown = Random(
                ULTIMATE_ARTIFACT_BORDER_MARGIN,
                MAP_HEIGHT - ULTIMATE_ARTIFACT_BORDER_MARGIN - 1
            );
        }
        ultimateDistance =
            Random(ULTIMATE_DISTANCE_ROLL_MIN, ULTIMATE_DISTANCE_COMMON_ROLL_MAX)
            + Random(ULTIMATE_DISTANCE_ROLL_MIN, ULTIMATE_DISTANCE_COMMON_ROLL_MAX)
            + Random(ULTIMATE_DISTANCE_ROLL_MIN, ULTIMATE_DISTANCE_BONUS_ROLL_MAX);
        ultimateTries++;
    }
    m_ultimateArtifactX = static_cast<i8>(player);
    m_ultimateArtifactY = static_cast<i8>(nTown);
    m_ultimateArtifactId =
        static_cast<ArtifactType>(Random(H2EnumIndex(ARTIFACT_ULTIMATE_BOOK), H2EnumIndex(ARTIFACT_GOLDEN_GOOSE)));
    if (gbInCampaign
        && ((m_campaignType == CAMPAIGN_ROLAND
             && m_campaignScenario + CAMPAIGN_SCENARIO_NUMBER_OFFSET
                    == CAMPAIGN_ROLAND_ULTIMATE_CROWN_SCENARIO)
            || (m_campaignType == CAMPAIGN_ARCHIBALD
                && m_campaignScenario + CAMPAIGN_SCENARIO_NUMBER_OFFSET
                       == CAMPAIGN_ARCHIBALD_ULTIMATE_CROWN_SCENARIO)))
        m_ultimateArtifactId = ARTIFACT_ULTIMATE_CROWN;
    for (player = 0; player < m_playerCount; player++) {
        if (gbHumanPlayer[player]) {
            m_players[player].m_aiDifficulty = PLAYER_PERSONALITY_HUMAN;
            memcpy(
                m_players[player].m_resources,
                gInitResourcesHuman[H2EnumIndex(m_difficulty)],
                sizeof(m_players[player].m_resources)
            );
            if (m_playerHandicap[player] != PLAYER_HANDICAP_NONE) {
                for (nTown = 0; nTown < H2EnumIndex(RES_COUNT); nTown++) {
                    (m_players + player)->m_resources[nTown] = static_cast<i32>(
                        (m_players + player)->m_resources[nTown]
                        * (m_playerHandicap[player] == PLAYER_HANDICAP_MODERATE
                               ? GAME_HANDICAP_MODERATE_RESOURCE_FACTOR
                               : GAME_HANDICAP_SEVERE_RESOURCE_FACTOR)
                    );
                }
            }
        } else {
            m_players[player].m_aiDifficulty = static_cast<PlayerPersonality>(Random(
                H2EnumIndex(PLAYER_PERSONALITY_COMPUTER_FIRST),
                H2EnumIndex(PLAYER_PERSONALITY_COMPUTER_LAST)
            ));
            memcpy(
                m_players[player].m_resources,
                gInitResourcesComputer[H2EnumIndex(m_difficulty)],
                sizeof(m_players[player].m_resources)
            );
        }
    }
    SetupAdjacentMons();
    if (m_mapHeader.lossCondition == MAP_LOSS_HERO) {
        xPos = m_mapHeader.lossConditionValue;
        yPos = m_mapHeader.lossTownY;
        m_mapHeader.lossConditionValue = 0;
        if (m_worldMap.GetCell(xPos, yPos)->m_triggerType
            == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MERMAID))
            m_mapHeader.lossConditionValue =
                m_worldMap.GetCell(xPos, yPos)->m_objectMetadata;
        else {
            if (m_worldMap.GetCell(xPos, yPos - 1)->m_triggerType
                == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MERMAID))
                m_mapHeader.lossConditionValue =
                    m_worldMap.GetCell(xPos, yPos - 1)->m_objectMetadata;
            else
                m_mapHeader.lossCondition = MAP_LOSS_STANDARD;
        }
    }
    if (m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_HERO) {
        xPos = m_mapHeader.victoryConditionValue;
        yPos = m_mapHeader.victoryTownY;
        m_mapHeader.victoryConditionValue = 0;
        if (m_worldMap.GetCell(xPos, yPos)->m_triggerType
            == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MERMAID))
            m_mapHeader.victoryConditionValue =
                m_worldMap.GetCell(xPos, yPos)->m_objectMetadata;
        else {
            if (m_worldMap.GetCell(xPos, yPos - 1)->m_triggerType
                == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MERMAID))
                m_mapHeader.victoryConditionValue =
                    m_worldMap.GetCell(xPos, yPos - 1)->m_objectMetadata;
            else
                m_mapHeader.victoryCondition = MAP_VICTORY_DEFEAT_ALL;
        }
    }
    for (player = 0; player < m_playerCount; player++) {
        sideClass = FACTION_KNIGHT;
        if (m_setupPlayerRace[gcColorToSetupPos[m_players[player].m_color]] >= FACTION_KNIGHT
            && m_setupPlayerRace[gcColorToSetupPos[m_players[player].m_color]]
                   < FACTION_COUNT) {
            sideClass = m_setupPlayerRace[gcColorToSetupPos[m_players[player].m_color]];
        } else {
            if (!!m_players[player].m_townCount) {
                sideClass = gpGame->m_castleRecs[m_players[player].m_townIds[0]].m_type;
            } else if (!!m_players[player].m_heroCount) {
                sideClass = gpGame->m_heroRecs[m_players[player].m_heroIds[0]].m_cursorType;
            }
        }
        if (sideClass == FACTION_BARBARIAN || sideClass == FACTION_WARLOCK
            || sideClass == FACTION_NECROMANCER)
            m_players[player].m_evilInterface = 1;
        else
            m_players[player].m_evilInterface = 0;
        if (gbInCampaign && player == 0) {
            if (m_campaignType == CAMPAIGN_ARCHIBALD)
                m_players[player].m_evilInterface = 1;
            else
                m_players[player].m_evilInterface = 0;
        }
        for (nTown = 0; nTown < gpGame->m_players[player].m_townCount; nTown++)
            GetCastle(gpGame->m_players[player].m_townIds[nTown])->GiveSpells(NULL);
        gpGame->m_players[player].m_minimumHeroCount = gpGame->m_players[player].m_heroCount;
    }
    gpPhilAI->GetGameAIVars();
    gbInNewGameSetup = false;
    SetupNewRumour();
    gpAdvManager->CheckSetEvilInterface(0, -1);
    // Towns with no water for a dock never offer one.
    for (townIndex = 0; townIndex < GAME_TOWN_COUNT; townIndex++) {
        if (!GetTown(townIndex)->CanBuildDock())
            Ironfist_DisallowBuilding(townIndex, H2EnumIndex(BUILDING_SLOT_DOCK));
    }
    return;
}

inline town* GetCastleSlot(game* instance, i32 index) {
    return &instance->m_castleRecs[index];
}

void game::RandomizeEvents(void) {
    i32 valid;
    u32 extraIndex27;
    i32 row;
    i32 xPos;
    ArtifactType value;
    i32 shrineId8 = 1;
    i32 bottleId = 1;
    i32 jailId8 = 1;
    i32 sphinxId = 1;
    i32 tentId0 = 1;
    i32 hutId27 = 1;
    i32 eyeId13 = 1;
    i32 signId = 1;
    i32 j4;
    mapCell* cell2;
    i32 yPos;
    i32 randomValue5;
    i32 column3;
    mapCellExtra* extra9;
    mapEventExtra* eventData4;
    i32 mineId6;
    EventExtra* mapEvent0;
    town* townRec;
    mapCell* townEntrance2;
    CreatureType artifactGuardianChoices1[ARTIFACT_GUARDIAN_CHOICE_COUNT];
    i32 lowerCount;
    i32 upperCount5;
    TilesetId lowerTilesets5[LAYER_SCAN_CAPACITY];
    TilesetId upperTilesets0[LAYER_SCAN_CAPACITY];
    i32 upperIndexes8[LAYER_SCAN_CAPACITY];
    i32 lowerIndexes28[LAYER_SCAN_CAPACITY];

    m_mapEventCount = 0;
    memset(m_mapEventIndices, 0, sizeof(m_mapEventIndices));

    for (yPos = 0; yPos < MAP_HEIGHT; yPos++) {
        for (xPos = 0; xPos < MAP_WIDTH; xPos++) {
            cell2 = m_worldMap.GetCell(xPos, yPos);
            switch (cell2->m_triggerType) {
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WITCH_HUT:
                    cell2->m_objectMetadata = H2EnumIndex(HERO_SKILL_NECROMANCY);
                    while (cell2->m_objectMetadata == H2EnumIndex(HERO_SKILL_NECROMANCY)
                           || cell2->m_objectMetadata == H2EnumIndex(HERO_SKILL_LEADERSHIP)) {
                        cell2->m_objectMetadata =
                            Random(WITCH_HUT_SKILL_FIRST, WITCH_HUT_SKILL_LAST);
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT:
                    cell2->m_objectTileset = TILESET_NONE;
                    cell2->m_objectIndex = MAPCELL_SPRITE_NONE;
                    cell2->m_objectMetadata = 0;
                    cell2->m_triggerType = 0;
                    CreateBoat(xPos, yPos, 1);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SPHINX:
                    eventData4 =
                        reinterpret_cast<mapEventExtra*>(ppMapExtra[cell2->m_objectMetadata]);
                    if (strlen(eventData4->riddle) > 1 && eventData4->answerCount >= 1)
                        eventData4->active = 1;
                    else
                        eventData4->active = 0;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MAP_EVENT:
                    m_mapEventIndices[m_mapEventCount] = cell2->m_objectMetadata;
                    mapEvent0 = reinterpret_cast<EventExtra*>(ppMapExtra[cell2->m_objectMetadata]);
                    mapEvent0->x = static_cast<i16>(xPos);
                    mapEvent0->y = static_cast<i16>(yPos);
                    mapEvent0->active = 1;
                    cell2->m_objectMetadata = 0;
                    cell2->m_triggerType = 0;
                    cell2->m_objectIndex = MAPCELL_SPRITE_NONE;
                    cell2->m_objectTileset = TILESET_NONE;
                    m_mapEventCount++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_GAZEBO:
                    cell2->m_objectMetadata = bottleId++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_FORT:
                    cell2->m_objectMetadata = jailId8++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WITCH_DOCTOR_HUT:
                    cell2->m_objectMetadata = sphinxId;
                    sphinxId++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MERCENARY_CAMP:
                    cell2->m_objectMetadata = tentId0++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_STANDING_STONES:
                    if (xPos > 0
                        && m_worldMap.GetCell(xPos - 1, yPos)->m_triggerType
                               == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_STANDING_STONES))
                        cell2->m_objectMetadata =
                            m_worldMap.GetCell(xPos - 1, yPos)->m_objectMetadata;
                    else
                        cell2->m_objectMetadata = hutId27++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_XANADU:
                    cell2->m_objectMetadata = signId++;
                    break;
                case MAP_OBJECT_WHIRLPOOL:
                    cell2->m_triggerType |= MAP_TRIGGER_ACTION_FLAG;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_OBELISK:
                    cell2->m_objectMetadata = shrineId8++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_FLOTSAM:
                    cell2->m_objectMetadata =
                        Random(H2EnumIndex(FLOTSAM_EMPTY), H2EnumIndex(FLOTSAM_LARGE_TREASURE));
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SKELETON:
                    if (!HasObjectTilesetIndex(
                            xPos,
                            yPos,
                            TILESET_OBJNDSRT,
                            SKELETON_DESERT_FRAME
                        ))
                        cell2->m_triggerType &= MAP_TRIGGER_TYPE_MASK;
                    else
                        cell2->m_objectMetadata = Random(SKELETON_ROLL_MIN, SKELETON_ROLL_MAX)
                                                          <= SKELETON_ARTIFACT_ROLL_MAX
                                                      ? GetRandomArtifactId(
                                                            ARTIFACT_LEVEL_RANDOM,
                                                            true
                                                        ) + SKELETON_ARTIFACT_OFFSET
                                                      : SKELETON_EMPTY;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WAGON:
                    randomValue5 = Random(EVENT_ROLL_MIN, EVENT_ROLL_MAX);
                    if (randomValue5 < WAGON_EMPTY_CUTOFF)
                        cell2->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
                    else if (randomValue5 < WAGON_ARTIFACT_CUTOFF)
                        cell2->m_objectMetadata =
                            GetRandomArtifactId(
                                ARTIFACT_LEVEL_MINOR | ARTIFACT_LEVEL_TREASURE,
                                true
                            )
                            | WAGON_ARTIFACT_FLAG;
                    else
                        cell2->m_objectMetadata =
                            Random(H2EnumIndex(RES_WOOD), H2EnumIndex(RES_GEMS))
                            + (Random(WAGON_AMOUNT_MIN, WAGON_AMOUNT_MAX)
                               << CAMPFIRE_AMOUNT_SHIFT)
                            + MAP_EVENT_RESOURCE_OFFSET;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_LEAN_TO:
                    cell2->m_objectMetadata =
                        Random(H2EnumIndex(RES_WOOD), H2EnumIndex(RES_GEMS))
                        + (Random(LEAN_TO_AMOUNT_MIN, LEAN_TO_AMOUNT_MAX)
                           << CAMPFIRE_AMOUNT_SHIFT)
                        + MAP_EVENT_RESOURCE_OFFSET;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DAEMON_CAVE:
                    switch (Random(EVENT_ROLL_MIN, EVENT_BUCKET_ROLL_MAX) % EVENT_BUCKET_COUNT) {
                        case 0:
                        case 1:
                        case 2:
                            cell2->m_objectMetadata = DAEMON_REWARD_EXPERIENCE;
                            break;
                        case 3:
                            cell2->m_objectMetadata = DAEMON_REWARD_ARTIFACT;
                            break;
                        case 4:
                        case 5:
                        case 6:
                            cell2->m_objectMetadata = DAEMON_REWARD_EXPERIENCE_GOLD;
                            break;
                        case 7:
                        case 8:
                        case 9:
                            cell2->m_objectMetadata = DAEMON_REWARD_RANSOM;
                            break;
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TREASURE_CHEST:
                    if (giGroundToTerrain[cell2->m_terrainImageIndex] == TERRAIN_WATER) {
                        cell2->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SEA_CHEST;
                        randomValue5 = Random(EVENT_ROLL_MIN, EVENT_ROLL_MAX);
                        if (randomValue5 < SEA_CHEST_EMPTY_CUTOFF)
                            cell2->m_objectMetadata = SEA_CHEST_OUTCOME_EMPTY;
                        else if (randomValue5 < SEA_CHEST_GOLD_CUTOFF)
                            cell2->m_objectMetadata = SEA_CHEST_OUTCOME_GOLD;
                        else
                            cell2->m_objectMetadata =
                                GetRandomArtifactId(ARTIFACT_LEVEL_TREASURE, true)
                                | CHEST_ARTIFACT_FLAG;
                    } else {
                        randomValue5 = Random(EVENT_ROLL_MIN, EVENT_ROLL_MAX);
                        if (randomValue5 < LAND_CHEST_SMALL_CUTOFF)
                            cell2->m_objectMetadata = CHEST_REWARD_SMALL;
                        else if (randomValue5 < LAND_CHEST_MEDIUM_CUTOFF)
                            cell2->m_objectMetadata = CHEST_REWARD_MEDIUM;
                        else if (randomValue5 < LAND_CHEST_LARGE_CUTOFF)
                            cell2->m_objectMetadata = CHEST_REWARD_LARGE;
                        else
                            cell2->m_objectMetadata =
                                GetRandomArtifactId(ARTIFACT_LEVEL_TREASURE, true)
                                | CHEST_ARTIFACT_FLAG;
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CAMPFIRE:
                    cell2->m_objectMetadata =
                        Random(CAMPFIRE_AMOUNT_MIN, CAMPFIRE_AMOUNT_MAX) << CAMPFIRE_AMOUNT_SHIFT;
                    cell2->m_objectMetadata |= Random(H2EnumIndex(RES_WOOD), H2EnumIndex(RES_GEMS));
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ANCIENT_LAMP:
                    cell2->m_objectMetadata =
                        Random(ANCIENT_LAMP_ROLL_MIN, ANCIENT_LAMP_ROLL_MAX)
                        + ANCIENT_LAMP_COUNT_OFFSET;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHIPWRECK_SURVIVOR:
                    randomValue5 = Random(EVENT_ROLL_MIN, EVENT_ROLL_MAX);
                    if (randomValue5 < SHIPWRECK_SURVIVOR_TREASURE_CUTOFF)
                        cell2->m_objectMetadata =
                            GetRandomArtifactId(ARTIFACT_LEVEL_TREASURE, true);
                    else if (randomValue5 < SHIPWRECK_SURVIVOR_MINOR_CUTOFF)
                        cell2->m_objectMetadata = GetRandomArtifactId(ARTIFACT_LEVEL_MINOR, true);
                    else
                        cell2->m_objectMetadata = GetRandomArtifactId(ARTIFACT_LEVEL_MAJOR, true);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_GRAVEYARD:
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHIPWRECK:
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DERELICT_SHIP:
                    switch (Random(EVENT_ROLL_MIN, EVENT_BUCKET_ROLL_MAX) % EVENT_BUCKET_COUNT) {
                        case 0:
                        case 1:
                        case 2:
                            cell2->m_objectMetadata = H2EnumIndex(EVENT_LEVEL_SMALL);
                            break;
                        case 3:
                        case 4:
                        case 5:
                            cell2->m_objectMetadata = H2EnumIndex(EVENT_LEVEL_MEDIUM);
                            break;
                        case 6:
                        case 7:
                        case 8:
                            cell2->m_objectMetadata = H2EnumIndex(EVENT_LEVEL_LARGE);
                            break;
                        case 9:
                            cell2->m_objectMetadata = H2EnumIndex(EVENT_LEVEL_HUGE);
                            break;
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARCHER_HOUSE:
                    cell2->m_objectMetadata = Random(10, 25);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_GOBLIN_HUT:
                    cell2->m_objectMetadata = Random(15, 40);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DWARF_COTTAGE:
                    cell2->m_objectMetadata = Random(0, 20) + 1;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_PEASANT_HUT:
                    cell2->m_objectMetadata = Random(0, 40) + 1;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_LOG_CABIN:
                    cell2->m_objectMetadata = Random(20, 50);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WATER_WHEEL:
                    cell2->m_objectMetadata = MAP_EVENT_DATA_AVAILABLE;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTESIAN_SPRING:
                    cell2->m_objectMetadata = MAP_EVENT_DATA_AVAILABLE;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MAGIC_GARDEN:
                    cell2->m_objectMetadata =
                        Random(EVENT_ROLL_MIN, EVENT_BINARY_ROLL_MAX) == EVENT_ROLL_MIN
                            ? H2EnumIndex(RES_GEMS) + MAP_EVENT_RESOURCE_OFFSET
                            : H2EnumIndex(RES_GOLD) + MAP_EVENT_RESOURCE_OFFSET;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TREE_OF_KNOWLEDGE:
                    cell2->m_objectMetadata =
                        eyeId13++
                        | (Random(TREE_KNOWLEDGE_FREE, TREE_KNOWLEDGE_GEMS)
                           << TREE_KNOWLEDGE_MODE_SHIFT);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER:
                    if (cell2->m_objectMetadata == MAP_EVENT_DATA_EMPTY) {
                        cell2->m_objectMetadata = GetRandomNumTroops(static_cast<CreatureType>(cell2->m_objectIndex));
                        if (cell2->m_objectIndex != H2EnumIndex(CREATURE_GHOST)
                            && cell2->m_objectIndex != H2EnumIndex(CREATURE_EARTH_ELEMENTAL)
                            && cell2->m_objectIndex != H2EnumIndex(CREATURE_AIR_ELEMENTAL)
                            && cell2->m_objectIndex != H2EnumIndex(CREATURE_FIRE_ELEMENTAL)
                            && cell2->m_objectIndex != H2EnumIndex(CREATURE_WATER_ELEMENTAL)
                            && Random(MONSTER_GUARD_ROLL_MIN, MONSTER_GUARD_ROLL_MAX)
                                   < MONSTER_GUARD_CUTOFF)
                            cell2->m_objectMetadata |= H2EnumIndex(MAP_MONSTER_GUARD_FLAG);
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RESOURCE:
                    cell2->m_objectMetadata = cell2->m_objectIndex / 2;
                    switch (cell2->m_objectMetadata) {
                        case H2EnumIndex(RES_WOOD):
                        case H2EnumIndex(RES_ORE):
                            cell2->m_objectMetadata =
                                Random(RESOURCE_BULK_AMOUNT_MIN, RESOURCE_BULK_AMOUNT_MAX);
                            break;
                        case H2EnumIndex(RES_GOLD):
                            cell2->m_objectMetadata =
                                Random(RESOURCE_BULK_AMOUNT_MIN, RESOURCE_BULK_AMOUNT_MAX);
                            break;
                        default:
                            cell2->m_objectMetadata =
                                Random(RESOURCE_SCARCE_AMOUNT_MIN, RESOURCE_SCARCE_AMOUNT_MAX);
                            break;
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHRINE_FIRST_CIRCLE:
                    cell2->m_objectMetadata =
                        Random(H2EnumIndex(SPELL_FIREBALL), H2EnumIndex(SPELL_COUNT) - 1) + MAP_EVENT_SPELL_OFFSET;
                    while (gsSpellInfo[cell2->m_objectMetadata - MAP_EVENT_SPELL_OFFSET].level
                           != SPELL_LEVEL_FIRST)
                        cell2->m_objectMetadata = Random(H2EnumIndex(SPELL_FIREBALL), H2EnumIndex(SPELL_COUNT) - 1)
                                                  + MAP_EVENT_SPELL_OFFSET;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHRINE_SECOND_CIRCLE:
                    cell2->m_objectMetadata =
                        Random(H2EnumIndex(SPELL_FIREBALL), H2EnumIndex(SPELL_COUNT) - 1) + MAP_EVENT_SPELL_OFFSET;
                    while (gsSpellInfo[cell2->m_objectMetadata - MAP_EVENT_SPELL_OFFSET].level
                           != SPELL_LEVEL_SECOND)
                        cell2->m_objectMetadata = Random(H2EnumIndex(SPELL_FIREBALL), H2EnumIndex(SPELL_COUNT) - 1)
                                                  + MAP_EVENT_SPELL_OFFSET;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHRINE_THIRD_CIRCLE:
                    cell2->m_objectMetadata =
                        Random(H2EnumIndex(SPELL_FIREBALL), H2EnumIndex(SPELL_COUNT) - 1) + MAP_EVENT_SPELL_OFFSET;
                    while (gsSpellInfo[cell2->m_objectMetadata - MAP_EVENT_SPELL_OFFSET].level
                           != SPELL_LEVEL_THIRD)
                        cell2->m_objectMetadata = Random(H2EnumIndex(SPELL_FIREBALL), H2EnumIndex(SPELL_COUNT) - 1)
                                                  + MAP_EVENT_SPELL_OFFSET;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_PYRAMID:
                    cell2->m_objectMetadata =
                        Random(H2EnumIndex(SPELL_FIREBALL), H2EnumIndex(SPELL_COUNT) - 1) + MAP_EVENT_SPELL_OFFSET;
                    while (gsSpellInfo[cell2->m_objectMetadata - MAP_EVENT_SPELL_OFFSET].level
                           != SPELL_LEVEL_FIFTH)
                        cell2->m_objectMetadata = Random(H2EnumIndex(SPELL_FIREBALL), H2EnumIndex(SPELL_COUNT) - 1)
                                                  + MAP_EVENT_SPELL_OFFSET;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TREE_HOUSE:
                    cell2->m_objectMetadata = Random(15, 25);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SIRENS:
                    cell2->m_objectMetadata = Random(10, 20);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WATCH_TOWER:
                    cell2->m_objectMetadata = Random(7, 10);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RUINS:
                    cell2->m_objectMetadata = Random(3, 5);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TREE_CITY:
                    cell2->m_objectMetadata = Random(20, 40);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HALFLING_HOLE:
                    cell2->m_objectMetadata = Random(20, 40);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TROLL_BRIDGE:
                    cell2->m_objectMetadata = Random(4, 6) | BANK_GUARDIAN_FLAG;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CITY_OF_DEAD:
                    cell2->m_objectMetadata = Random(4, 6) | BANK_GUARDIAN_FLAG;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DRAGON_CITY:
                    cell2->m_objectMetadata = BANK_GUARDIAN_FLAG | 2;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CAVE:
                    cell2->m_objectMetadata = Random(10, 20);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_EXCAVATION:
                    cell2->m_objectMetadata = Random(10, 25);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DESERT_TENT:
                    cell2->m_objectMetadata = Random(10, 20);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WAGON_CAMP:
                    if (!HasObjectTilesetIndex(
                            xPos,
                            yPos,
                            TILESET_OBJNMUL2,
                            WAGON_CAMP_ACTIVE_FRAME
                        )) {
                        cell2->m_triggerType &= MAP_TRIGGER_TYPE_MASK;
                        break;
                    }
                    cell2->m_objectMetadata = Random(30, 50);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTIFACT:
                    randomValue5 = Random(EVENT_ROLL_MIN, EVENT_BUCKET_ROLL_MAX);
                    value = static_cast<ArtifactType>(cell2->m_objectIndex / 2);
                    if (value == ARTIFACT_SPELL_SCROLL)
                        break;
                    if (randomValue5 < ARTIFACT_EVENT_UNCONDITIONAL_CUTOFF) {
                        if (randomValue5 % EVENT_BUCKET_COUNT
                            == ARTIFACT_EVENT_WISDOM_BUCKET)
                            cell2->m_objectMetadata = ARTIFACT_EVENT_MODE_WISDOM;
                        else if (randomValue5 % EVENT_BUCKET_COUNT
                                 == ARTIFACT_EVENT_LEADERSHIP_BUCKET)
                            cell2->m_objectMetadata = ARTIFACT_EVENT_MODE_LEADERSHIP;
                        else
                            cell2->m_objectMetadata = ARTIFACT_EVENT_MODE_PICKUP;
                    } else if (randomValue5 < ARTIFACT_EVENT_GUARD_CUTOFF) {
                        if (gArtifactLevel[H2EnumIndex(value)] == ARTIFACT_LEVEL_TREASURE)
                            cell2->m_objectMetadata = ARTIFACT_EVENT_MODE_GOLD;
                        else if (gArtifactLevel[H2EnumIndex(value)] == ARTIFACT_LEVEL_MINOR)
                            cell2->m_objectMetadata =
                                (Random(H2EnumIndex(RES_WOOD), H2EnumIndex(RES_GEMS))
                                 << ARTIFACT_EVENT_RESOURCE_SHIFT)
                                | ARTIFACT_EVENT_MODE_RESOURCE_3;
                        else if (gArtifactLevel[H2EnumIndex(value)] == ARTIFACT_LEVEL_MAJOR)
                            cell2->m_objectMetadata =
                                (Random(H2EnumIndex(RES_WOOD), H2EnumIndex(RES_GEMS))
                                 << ARTIFACT_EVENT_RESOURCE_SHIFT)
                                | ARTIFACT_EVENT_MODE_RESOURCE_5;
                    } else {
                        artifactGuardianChoices1[0] = CREATURE_PALADIN;
                        artifactGuardianChoices1[1] = CREATURE_CRUSADER;
                        artifactGuardianChoices1[2] = CREATURE_CYCLOPS;
                        artifactGuardianChoices1[3] = CREATURE_GENIE;
                        artifactGuardianChoices1[4] = CREATURE_GREEN_DRAGON;
                        artifactGuardianChoices1[5] = CREATURE_RED_DRAGON;
                        artifactGuardianChoices1[6] = CREATURE_BLACK_DRAGON;
                        artifactGuardianChoices1[7] = CREATURE_BONE_DRAGON;
                        artifactGuardianChoices1[8] = CREATURE_GIANT;
                        artifactGuardianChoices1[9] = CREATURE_TITAN;
                        cell2->m_objectMetadata = ARTIFACT_EVENT_GUARDED_FLAG;
                        if (gArtifactLevel[H2EnumIndex(value)] == ARTIFACT_LEVEL_TREASURE)
                            cell2->m_objectMetadata |= H2EnumIndex(CREATURE_ROGUE);
                        else if (gArtifactLevel[H2EnumIndex(value)] == ARTIFACT_LEVEL_MINOR)
                            cell2->m_objectMetadata |= H2EnumIndex(artifactGuardianChoices1[Random(
                                EVENT_ROLL_MIN,
                                MINOR_GUARDIAN_CHOICE_COUNT - 1
                            ) + MINOR_GUARDIAN_CHOICE_FIRST]);
                        else
                            cell2->m_objectMetadata |= H2EnumIndex(artifactGuardianChoices1[Random(
                                EVENT_ROLL_MIN,
                                MAJOR_GUARDIAN_CHOICE_COUNT - 1
                            ) + MAJOR_GUARDIAN_CHOICE_FIRST]);
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE:
                    mineId6 = GetTownId(xPos, yPos);
                    for (row = yPos - CASTLE_METADATA_TOP_OFFSET;
                         row <= yPos + CASTLE_METADATA_BOTTOM_OFFSET;
                         row++) {
                        for (column3 = xPos - CASTLE_METADATA_X_RADIUS;
                             column3 <= xPos + CASTLE_METADATA_X_RADIUS;
                             column3++) {
                            if (m_worldMap.GetCell(column3, row)->m_objectMetadata == 0)
                                m_worldMap.GetCell(column3, row)->m_objectMetadata = mineId6;
                        }
                    }
                    townRec = GetCastleSlot(this, mineId6);
                    townRec->m_boatY = -1;
                    townRec->m_boatX = -1;
                    if (yPos <= MAP_HEIGHT - CASTLE_BOAT_Y_OFFSET - 1) {
                        townEntrance2 = gpAdvManager->GetCell(
                            xPos - CASTLE_BOAT_X_OFFSET,
                            yPos + CASTLE_BOAT_Y_OFFSET
                        );
                        if (giGroundToTerrain[townEntrance2->m_terrainImageIndex]
                            == TERRAIN_WATER) {
                            townRec->m_boatX = static_cast<i8>(xPos - CASTLE_BOAT_X_OFFSET);
                            townRec->m_boatY = static_cast<i8>(yPos + CASTLE_BOAT_Y_OFFSET);
                        } else {
                            townEntrance2 = gpAdvManager->GetCell(
                                xPos + CASTLE_BOAT_X_OFFSET,
                                yPos + CASTLE_BOAT_Y_OFFSET
                            );
                            if (giGroundToTerrain[townEntrance2->m_terrainImageIndex]
                                == TERRAIN_WATER) {
                                townRec->m_boatX =
                                    static_cast<i8>(xPos + CASTLE_BOAT_X_OFFSET);
                                townRec->m_boatY =
                                    static_cast<i8>(yPos + CASTLE_BOAT_Y_OFFSET);
                            }
                        }
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_LIGHTHOUSE: {
                    m_worldMap.GetCell(xPos, yPos)->m_objectMetadata = GetMineId(xPos, yPos);
                    break;
                }
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ABANDONED_MINE:
                    mineId6 = GetMineId(xPos, yPos);
                    m_mines[mineId6].guardianType = CREATURE_GHOST;
                    m_mines[mineId6].guardianCount = static_cast<u8>(Random(
                        ABANDONED_MINE_GUARDIAN_COUNT_MIN,
                        ABANDONED_MINE_GUARDIAN_COUNT_MAX
                    ));
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ALCHEMIST_LAB:
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MINE:
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SAWMILL:
                    mineId6 = GetMineId(xPos, yPos);
                    for (row = yPos - MINE_METADATA_TOP_OFFSET; row <= yPos;
                         row++) {
                        for (column3 = xPos - MINE_METADATA_LEFT_OFFSET;
                             column3 <= xPos + MINE_METADATA_RIGHT_OFFSET;
                             column3++) {
                            if (column3 == xPos - MINE_METADATA_LEFT_OFFSET
                                && cell2->m_triggerType
                                       != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ALCHEMIST_LAB))
                                continue;
                            if (m_worldMap.GetCell(column3, row)->m_objectMetadata == 0
                                || ((m_worldMap.GetCell(column3, row)->m_triggerType
                                     & MAP_TRIGGER_TYPE_MASK)
                                    == (cell2->m_triggerType & MAP_TRIGGER_TYPE_MASK)))
                                m_worldMap.GetCell(column3, row)->m_objectMetadata = mineId6;
                        }
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WINDMILL:
                    cell2->m_objectMetadata =
                        Random(WINDMILL_RESOURCE_AMOUNT_MIN, WINDMILL_RESOURCE_AMOUNT_MAX);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BARRIER:
                    RandomizeBarrier(cell2);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TRAVELER_TENT:
                    RandomizePassword(cell2);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_EXPANSION_OBJECT:
                    WeeklyGenericSite(cell2);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_EXPANSION_DWELLING:
                    WeeklyRecruitSite(cell2);
                    break;
            }
        }
    }

    for (yPos = 0; yPos < MAP_HEIGHT; yPos++) {
        for (xPos = 0; xPos < MAP_WIDTH; xPos++) {
            cell2 = m_worldMap.GetCell(xPos, yPos);
            if (cell2->m_objectIndex != MAPCELL_SPRITE_NONE && cell2->m_objectLayerBit1) {
                valid = 1;
                extraIndex27 = cell2->m_extraIndex;
                while (extraIndex27 != 0) {
                    extra9 = m_worldMap.Extra(extraIndex27);
                    if (extra9->objectIndex != MAPCELL_SPRITE_NONE
                        && !extra9->objectLayerBit1)
                        valid = 0;
                    extraIndex27 = extra9->nextIndex;
                }
                if (valid)
                    cell2->m_flags |= H2EnumIndex(MAP_CELL_OBJECT_SHADOW_ONLY);
            }
        }
    }

    for (yPos = 0; yPos < MAP_HEIGHT; yPos++) {
        for (xPos = 0; xPos < MAP_WIDTH; xPos++) {
            cell2 = m_worldMap.GetCell(xPos, yPos);
            if ((cell2->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_ROCK
                && cell2->m_objectTileset == TILESET_X_LOC2)
                cell2->m_flags |= H2EnumIndex(MAP_CELL_OCCUPIED);
            if (cell2->m_objectIndex != MAPCELL_SPRITE_NONE
                && !(cell2->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
                && !(cell2->m_flags & H2EnumIndex(MAP_CELL_OBJECT_SHADOW_ONLY))
                && cell2->m_overlayIndex != MAPCELL_SPRITE_NONE)
                cell2->m_flags |= H2EnumIndex(MAP_CELL_OCCUPIED);
            upperCount5 = 0;
            lowerCount = 0;
            if (!(cell2->m_flags & H2EnumIndex(MAP_CELL_OCCUPIED)) && yPos < MAP_HEIGHT - 1
                && cell2->m_objectIndex != MAPCELL_SPRITE_NONE
                && !(cell2->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
                && !(cell2->m_flags & H2EnumIndex(MAP_CELL_OBJECT_SHADOW_ONLY))) {
                mapCell* below0;
                if (m_worldMap.GetCell(xPos, yPos + 1)->m_objectIndex != MAPCELL_SPRITE_NONE
                    && !(
                        m_worldMap.GetCell(xPos, yPos + 1)->m_triggerType
                        & MAP_TRIGGER_ACTION_FLAG
                    )
                    && !(
                        m_worldMap.GetCell(xPos, yPos + 1)->m_flags
                        & H2EnumIndex(MAP_CELL_OBJECT_SHADOW_ONLY)
                    )) {
                    if (!cell2->m_objectLayerBit1) {
                        upperTilesets0[upperCount5] = cell2->m_objectTileset;
                        upperIndexes8[upperCount5] = cell2->m_objectIndex;
                        upperCount5++;
                    }
                    if (cell2->m_extraIndex != 0)
                        extra9 = m_worldMap.Extra(cell2->m_extraIndex);
                    else
                        extra9 = NULL;
                    while (upperCount5 < LAYER_SCAN_CAPACITY && extra9 != NULL) {
                        if (extra9->objectIndex != MAPCELL_SPRITE_NONE
                            && !extra9->objectLayerBit1) {
                            upperTilesets0[upperCount5] = extra9->objectTileset;
                            upperIndexes8[upperCount5] = extra9->objectIndex;
                            upperCount5++;
                        }
                        if (extra9->nextIndex != 0)
                            extra9 = m_worldMap.Extra(extra9->nextIndex);
                        else
                            extra9 = NULL;
                    }
                    below0 = m_worldMap.GetCell(xPos, yPos + 1);
                    if (!below0->m_objectLayerBit1) {
                        lowerTilesets5[lowerCount] = below0->m_objectTileset;
                        lowerIndexes28[lowerCount] = below0->m_objectIndex;
                        lowerCount++;
                    }
                    if (below0->m_extraIndex != 0)
                        extra9 = m_worldMap.Extra(below0->m_extraIndex);
                    else
                        extra9 = NULL;
                    while (lowerCount < LAYER_SCAN_CAPACITY && extra9 != NULL) {
                        if (extra9->objectIndex != MAPCELL_SPRITE_NONE
                            && !extra9->objectLayerBit1) {
                            lowerTilesets5[lowerCount] = extra9->objectTileset;
                            lowerIndexes28[lowerCount] = extra9->objectIndex;
                            lowerCount++;
                        }
                        if (extra9->nextIndex != 0)
                            extra9 = m_worldMap.Extra(extra9->nextIndex);
                        else
                            extra9 = NULL;
                    }
                    for (randomValue5 = 0; randomValue5 < upperCount5; randomValue5++) {
                        for (j4 = 0; j4 < lowerCount; j4++) {
                            if (upperTilesets0[randomValue5] == lowerTilesets5[j4]
                                || (upperTilesets0[randomValue5] >= TILESET_OBJNTOWN
                                    && upperTilesets0[randomValue5] <= TILESET_OBJNTWRD
                                    && lowerTilesets5[j4] >= TILESET_OBJNTOWN
                                    && lowerTilesets5[j4] <= TILESET_OBJNTWRD))
                                cell2->m_flags |= H2EnumIndex(MAP_CELL_OCCUPIED);
                        }
                    }
                }
            }
            if (yPos < MAP_HEIGHT - 1) {
                if (m_worldMap.GetCell(xPos, yPos + 1)->m_triggerType
                        == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)
                    || m_worldMap.GetCell(xPos, yPos + 1)->m_triggerType
                           == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_TOWN)
                    || m_worldMap.GetCell(xPos, yPos + 1)->m_triggerType
                           == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_CASTLE))
                    cell2->m_flags |= H2EnumIndex(MAP_CELL_OCCUPIED);
            }
            if (cell2->m_objectIndex != MAPCELL_SPRITE_NONE
                && !(cell2->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
                && !(cell2->m_flags & H2EnumIndex(MAP_CELL_OBJECT_SHADOW_ONLY))
                && (yPos == MAP_HEIGHT - 1
                    || (m_worldMap.GetCell(xPos, yPos + 1)->m_flags & 4)))
                cell2->m_flags |= H2EnumIndex(MAP_CELL_OCCUPIED);
        }
    }
}

void game::InitializePasswords(void) {
    char flag;
    i32 i;
    i32 j;
    for (i = 0; i < PASSWORD_INDEX_COUNT; i++) {
        flag = 0;
        while (flag == 0) {
            xPasswordStringsIndex[i] = Random(0, X_GLOBAL_PASSWORD_STRING_COUNT - 1);
            flag = 1;
            for (j = 0; j < i; j++) {
                if (xPasswordStringsIndex[i] == xPasswordStringsIndex[j])
                    flag = 0;
            }
        }
    }
}

void game::RandomizeBarrier(mapCell* cell) {
    i32 index = cell->m_objectMetadata;
    index &= PASSWORD_INDEX_MASK;
    i32 p = xPasswordStringsIndex[index];
    i32 color = (p << PASSWORD_COLOR_SHIFT) | index;
    cell->m_objectMetadata = color;
}

void game::RandomizePassword(mapCell* cell) {
    RandomizeBarrier(cell);
}

i32 game::LoadMap(char* filename) {
    char x[LOAD_MAP_COORDINATE_SCRATCH_SIZE];
    char y[LOAD_MAP_COORDINATE_SCRATCH_SIZE];
    char type[LOAD_MAP_RECORD_SCRATCH_SIZE];
    char junk[LOAD_MAP_RECORD_SCRATCH_SIZE];
    i32 i;
    i32 handle;

    sprintf(gText, "%s%s", gcMapPath, filename);
    handle = platform::FileOpen(gText, platform::FileMode::Read);
    if (handle == -1)
        FileError(gText);
    platform::FileRead(handle, &m_mapHeader, sizeof(m_mapHeader));
    localization::SetCurrentFileTextEncoding(
        GetMapHeaderTextEncoding(&m_mapHeader)
    );
    platform::Host().Log(
        platform::LogLevel::Info,
        (std::string("localization: map-text=")
         + localization::TextEncodingName(localization::CurrentFileTextEncoding())
         + ", file=" + filename).c_str()
    );
    m_worldMap.Read(handle, 1);
    SetMapSize(m_worldMap.width, m_worldMap.height);

    for (i = 0; i < GAME_TOWN_COUNT; i++) {
        platform::FileRead(handle, x, sizeof(x[0]));
        platform::FileRead(handle, y, sizeof(y[0]));
        platform::FileRead(handle, type, sizeof(type[0]));
        if (static_cast<u8>(x[0]) != SAVED_TOWN_OFF_MAP) {
            m_castleRecs[i].m_onMap = 1;
            m_castleRecs[i].m_x = static_cast<u8>(x[0]);
            m_castleRecs[i].m_y = static_cast<u8>(y[0]);
            m_castleRecs[i].m_type =
                static_cast<FactionType>(type[0] & TOWN_RECORD_TYPE_MASK);
            if (type[0] < 0)
                m_castleRecs[i].m_buildings |= H2EnumIndex(TOWN_BUILDING_CASTLE);
            else
                m_castleRecs[i].m_buildings |= H2EnumIndex(TOWN_BUILDING_TENT);
        }
    }

    for (i = 0; i < GAME_MINE_COUNT; i++) {
        if (m_mapHeader.magic == MAP_HEADER_MAGIC_BASE_GAME && i >= GAME_TOWN_COUNT) {
            x[0] = -1;
            y[0] = -1;
            type[0] = -1;
        } else {
            platform::FileRead(handle, x, sizeof(x[0]));
            platform::FileRead(handle, y, sizeof(y[0]));
            platform::FileRead(handle, type, sizeof(type[0]));
        }
        if (static_cast<u8>(x[0]) != SAVED_TOWN_OFF_MAP) {
            m_mines[i].guardianType = CREATURE_NONE;
            m_mines[i].x = static_cast<u8>(x[0]);
            m_mines[i].y = static_cast<u8>(y[0]);
            m_mines[i].resourceType = static_cast<MineType>(type[0]);
        }
    }

    m_mapHeader.magic = MAP_HEADER_MAGIC_EXPANSION_GAME;
    platform::FileRead(handle, &m_obeliskCount, sizeof(m_obeliskCount));
    platform::FileRead(
        handle,
        m_rumourEventIndices,
        m_mapHeader.rumourCount * sizeof(m_rumourEventIndices[0])
    );
    m_rumourEventCount = m_mapHeader.rumourCount;
    platform::FileRead(
        handle,
        m_timeEventIndices,
        m_mapHeader.timeEventCount * sizeof(m_timeEventIndices[0])
    );
    m_timeEventCount = m_mapHeader.timeEventCount;
    platform::FileRead(handle, &iMaxMapExtra, sizeof(iMaxMapExtra));
    ppMapExtra = reinterpret_cast<void**>(
        H2_ALLOC(iMaxMapExtra * sizeof(ppMapExtra[0]))
    );
    pwSizeOfMapExtra = reinterpret_cast<i16*>(
        H2_ALLOC(iMaxMapExtra * sizeof(pwSizeOfMapExtra[0]))
    );
    memset(ppMapExtra, 0, iMaxMapExtra * sizeof(ppMapExtra[0]));
    memset(pwSizeOfMapExtra, 0, iMaxMapExtra * sizeof(pwSizeOfMapExtra[0]));
    for (i = 1; i < iMaxMapExtra; i++) {
        platform::FileRead(handle, pwSizeOfMapExtra + i, sizeof(pwSizeOfMapExtra[0]));
        ppMapExtra[i] = H2_ALLOC(pwSizeOfMapExtra[i]);
        platform::FileRead(handle, ppMapExtra[i], pwSizeOfMapExtra[i]);
    }
    platform::FileRead(handle, junk, sizeof(u16));
    platform::FileClose(handle);
    return 0;
}

void game::ClaimTown(i32 townId, i32 player, i32 suppressVisibility) {
    i32 i;
    town* townRec4;
    mapCell* cellPtr8;

    if (!gbInNewGameSetup)
        SendMapChange(
            MAP_CHANGE_CLAIM_TOWN,
            static_cast<i8>(townId),
            0,
            0,
            player,
            0,
            0
        );
    townRec4 = &m_castleRecs[townId];
    if (townRec4->m_owner == player)
        return;
    townRec4->m_formation = 0;
    if (m_castleOwners[townId] != -1)
        GetCastle(townId)->Deallocate();
    for (i = 0; i < ARMY_GROUP_SLOT_COUNT; i++) {
        townRec4->m_army.m_creatureTypes[i] = CREATURE_NONE;
        townRec4->m_army.m_creatureCounts[i] = 0;
    }
    if (m_castleRecs[townId].m_owner == -1)
        m_castleRecs[townId].m_turnsOwned = TOWN_NEW_OWNER_TURN_COUNT;
    else
        m_castleRecs[townId].m_turnsOwned = 0;
    m_castleRecs[townId].m_owner = static_cast<i8>(player);
    m_castleOwners[townId] = static_cast<i8>(player);
    m_players[player].m_townIds[m_players[player].m_townCount] = static_cast<i8>(townId);
    m_players[player].m_townCount++;

    cellPtr8 = m_worldMap.GetCell(m_castleRecs[townId].m_x - 1, m_castleRecs[townId].m_y);
    m_worldMap.ChangeTilesetIndex(
        cellPtr8,
        m_castleRecs[townId].m_x - 1,
        m_castleRecs[townId].m_y,
        TILESET_FLAG32,
        GetPlayerColor(static_cast<i8>(player)) * TOWN_FLAG_FRAME_STRIDE,
        1,
        -1
    );
    cellPtr8 = m_worldMap.GetCell(m_castleRecs[townId].m_x + 1, m_castleRecs[townId].m_y);
    m_worldMap.ChangeTilesetIndex(
        cellPtr8,
        m_castleRecs[townId].m_x + 1,
        m_castleRecs[townId].m_y,
        TILESET_FLAG32,
        GetPlayerColor(static_cast<i8>(player)) * TOWN_FLAG_FRAME_STRIDE
            + TOWN_FLAG_RIGHT_FRAME,
        1,
        -1
    );
    if (suppressVisibility != 0) {
        Ironfist_CastleConquered(townId, player);
        return;
    }
    SetVisibility(m_castleRecs[townId].m_x, m_castleRecs[townId].m_y, player, giVisRangeTown);
    CheckEndGame(END_GAME_FORCE_NONE, false);
    Ironfist_CastleConquered(townId, player);
}

void game::ClaimMine(i32 mineId, i32 player) {
    i32 x;
    i32 y;
    i32 flag1;
    mapCell* cell;

    SendMapChange(
        MAP_CHANGE_CLAIM_MINE,
        static_cast<i8>(mineId),
        0,
        0,
        player,
        0,
        0
    );
    m_mines[mineId].owner = static_cast<i8>(player);
    m_mineOwners[mineId] = static_cast<i8>(player);
    switch (m_mines[mineId].resourceType) {
        case MINE_TYPE_ALCHEMIST_LAB:
            flag1 = MINE_FLAG_ALCHEMIST_OFFSET;
            break;
        case MINE_TYPE_LIGHTHOUSE:
            flag1 = MINE_FLAG_LIGHTHOUSE_OFFSET;
            break;
        case MINE_TYPE_WOOD:
            flag1 = MINE_FLAG_WOOD_OFFSET;
            break;
        case MINE_TYPE_MERCURY:
            flag1 = MINE_FLAG_MERCURY_OFFSET;
            break;
        default:
            flag1 = MINE_FLAG_COMMON_OFFSET;
            break;
    }
    switch (m_mines[mineId].resourceType) {
        case MINE_TYPE_MERCURY:
            x = m_mines[mineId].x;
            y = m_mines[mineId].y - 1;
            break;
        case MINE_TYPE_WOOD:
            x = m_mines[mineId].x + 1;
            y = m_mines[mineId].y - 1;
            break;
        case MINE_TYPE_ALCHEMIST_LAB:
            x = m_mines[mineId].x - 1;
            y = m_mines[mineId].y - MINE_ALCHEMIST_FLAG_Y_OFFSET;
            break;
        case MINE_TYPE_LIGHTHOUSE:
            x = m_mines[mineId].x;
            y = m_mines[mineId].y;
            break;
        default:
            x = m_mines[mineId].x;
            y = m_mines[mineId].y;
            break;
    }
    cell = m_worldMap.GetCell(x, y);
    if (player == -1) {
        m_worldMap.ChangeTilesetIndex(
            cell, x, y, TILESET_FLAG32, MAPCELL_SPRITE_NONE, 1, -1
        );
    } else {
        m_worldMap.ChangeTilesetIndex(
            cell,
            x,
            y,
            TILESET_FLAG32,
            flag1 + GetPlayerColor(static_cast<i8>(player)),
            1,
            -1
        );
        if (m_mines[mineId].resourceType == MINE_TYPE_MERCURY) {
            ConvertFlagToLateOverlay(x, y);
        } else if (y > 0
                   && (m_mines[mineId].resourceType == MINE_TYPE_ORE
                       || m_mines[mineId].resourceType == MINE_TYPE_GOLD
                       || m_mines[mineId].resourceType == MINE_TYPE_GEMS
                       || m_mines[mineId].resourceType == MINE_TYPE_SULFUR
                       || m_mines[mineId].resourceType == MINE_TYPE_CRYSTAL)
                   && HasLateOverlay(x, y - 1)) {
            ConvertFlagToLateOverlay(x, y);
        }
    }
}

SpellType
game::ViewSpells(
    hero* spellHero, HeroSpellType spellType, MessageDispatchHandler callback, i32 readOnly
) {
    tag_message message;

    viewSpellsHero = spellHero;
    m_viewSpell = SPELL_NONE;
    if (spellHero->GetNumSpells(spellType) == 0) {
        NormalDialog(const_cast<char*>(localization::Tr("spell.none_to_cast")), 1, -1, -1, -1, 0, -1, 0, -1, 0);
    } else {
        m_viewSpellsCallback = callback;
        m_viewSpellsReadOnly = static_cast<i8>(readOnly);
        m_viewSpellsHero = spellHero;
        if (spellType == SPELL_TYPE_ALL)
            m_viewSpellsType = SPELL_TYPE_ADVENTURE;
        else
            m_viewSpellsType = spellType;
        m_viewSpellsTop[H2EnumIndex(SPELL_TYPE_COMBAT)] = 0;
        m_viewSpellsCount[H2EnumIndex(SPELL_TYPE_COMBAT)] =
            spellHero->GetNumSpells(SPELL_TYPE_COMBAT);
        m_viewSpellsTop[H2EnumIndex(SPELL_TYPE_ADVENTURE)] = 0;
        m_viewSpellsCount[H2EnumIndex(SPELL_TYPE_ADVENTURE)] =
            spellHero->GetNumSpells(SPELL_TYPE_ADVENTURE);
        m_viewSpellsWindow = new heroWindow(
            VIEW_SPELLS_WINDOW_X, VIEW_SPELLS_WINDOW_Y, const_cast<char*>("spellwin.bin")
        );
        if (m_viewSpellsWindow == NULL)
            MemError();
        if (spellType != SPELL_TYPE_ALL) {
            message.type = MESSAGE_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.id = static_cast<i16>(
                VIEW_SPELL_COMBAT_TAB_ID
                + static_cast<i32>(spellType != SPELL_TYPE_COMBAT)
            );
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
            m_viewSpellsWindow->BroadcastMessage(message);
        }
        UpdateSpellWidgets();
        gpWindowManager->DoDialog(m_viewSpellsWindow, ViewSpellsHandler, 0);
        delete m_viewSpellsWindow;
    }
    return m_viewSpell;
}

void game::UpdateSpellWidgets(void) {
    i32 spellPoints0;
    i32 lines;
    tag_message message;
    i32 i;
    SpellType spell1;

    message.type = MESSAGE_WIDGET;
    spellPoints0 = m_viewSpellsHero->m_spellPoints;
    if (spellPoints0 > VIEW_SPELL_MANA_MAX)
        spellPoints0 = VIEW_SPELL_MANA_MAX;
    memset(&message, 0, sizeof(message));

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = static_cast<BaseWidgetCommand>(
        H2EnumIndex(WIDGET_COMMAND_SET_FLAGS)
        + static_cast<i32>(spellPoints0 <= VIEW_SPELL_MANA_HUNDREDS_THRESHOLD)
    );
    message.payload.widget.id = VIEW_SPELL_MANA_HUNDREDS_ID;
    message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
    m_viewSpellsWindow->BroadcastMessage(message);

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = static_cast<BaseWidgetCommand>(
        H2EnumIndex(WIDGET_COMMAND_SET_FLAGS)
        + static_cast<i32>(spellPoints0 <= VIEW_SPELL_MANA_TENS_THRESHOLD)
    );
    message.payload.widget.id = VIEW_SPELL_MANA_TENS_ID;
    message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
    m_viewSpellsWindow->BroadcastMessage(message);

    sprintf(
        gText,
        "%d",
        (spellPoints0 / VIEW_SPELL_MANA_HUNDREDS_DIVISOR) % VIEW_SPELL_MANA_DIGIT_BASE
    );
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = VIEW_SPELL_MANA_HUNDREDS_ID;
    message.payload.widget.data.text = gText;
    m_viewSpellsWindow->BroadcastMessage(message);

    sprintf(
        gText,
        "%d",
        (spellPoints0 / VIEW_SPELL_MANA_TENS_DIVISOR) % VIEW_SPELL_MANA_DIGIT_BASE
    );
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = VIEW_SPELL_MANA_TENS_ID;
    message.payload.widget.data.text = gText;
    m_viewSpellsWindow->BroadcastMessage(message);

    sprintf(gText, "%d", spellPoints0 % VIEW_SPELL_MANA_DIGIT_BASE);
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = VIEW_SPELL_MANA_ONES_ID;
    message.payload.widget.data.text = gText;
    m_viewSpellsWindow->BroadcastMessage(message);

    for (i = 0; i < VIEW_SPELL_PAGE_SIZE; i++) {
        if (m_viewSpellsTop[H2EnumIndex(m_viewSpellsType)] + i
            >= m_viewSpellsCount[H2EnumIndex(m_viewSpellsType)]) {
            message.type = MESSAGE_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.id = i + VIEW_SPELL_ICON_ID_BASE;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
            m_viewSpellsWindow->BroadcastMessage(message);

            message.type = MESSAGE_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.id = i + VIEW_SPELL_TEXT_ID_BASE;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
            m_viewSpellsWindow->BroadcastMessage(message);
        } else {
            message.type = MESSAGE_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.id = i + VIEW_SPELL_TEXT_ID_BASE;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED);
            m_viewSpellsWindow->BroadcastMessage(message);

            spell1 = m_viewSpellsHero->GetNthSpell(
                m_viewSpellsType,
                m_viewSpellsTop[H2EnumIndex(m_viewSpellsType)] + i + 1
            );
            message.type = MESSAGE_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_SET_FILL_COLOR;
            message.payload.widget.id = i + VIEW_SPELL_TEXT_ID_BASE;
            if (GetManaCost(spell1, m_viewSpellsHero) > m_viewSpellsHero->m_spellPoints)
                message.payload.widget.data.value = VIEW_SPELL_UNAVAILABLE_COLOR;
            else
                message.payload.widget.data.value = VIEW_SPELL_AVAILABLE_COLOR;
            m_viewSpellsWindow->BroadcastMessage(message);

            lines = smallFont->LineLength(gSpellNames[H2EnumIndex(spell1)], VIEW_SPELL_NAME_WIDTH);
            if (lines == 1) {
                sprintf(
                    gText,
                    "%s\n[%d]",
                    gSpellNames[H2EnumIndex(spell1)],
                    GetManaCost(spell1, m_viewSpellsHero)
                );
            } else {
                sprintf(
                    gText,
                    "%s [%d]",
                    gSpellNames[H2EnumIndex(spell1)],
                    GetManaCost(spell1, m_viewSpellsHero)
                );
            }
            message.type = MESSAGE_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            message.payload.widget.id = i + VIEW_SPELL_TEXT_ID_BASE;
            message.payload.widget.data.text = gText;
            m_viewSpellsWindow->BroadcastMessage(message);

            message.type = MESSAGE_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.id = i + VIEW_SPELL_TEXT_ID_BASE;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
            m_viewSpellsWindow->BroadcastMessage(message);

            message.type = MESSAGE_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.id = i + VIEW_SPELL_ICON_ID_BASE;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED);
            m_viewSpellsWindow->BroadcastMessage(message);

            message.type = MESSAGE_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message.payload.widget.id = i + VIEW_SPELL_ICON_ID_BASE;
            message.payload.widget.data.value = gsSpellInfo[H2EnumIndex(spell1)].iconIndex;
            m_viewSpellsWindow->BroadcastMessage(message);

            message.type = MESSAGE_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.id = i + VIEW_SPELL_ICON_ID_BASE;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
            m_viewSpellsWindow->BroadcastMessage(message);
        }
    }
}

MessageDispatchResult ViewSpellsHandler(tag_message& msg) {
    SpellType spell;

    if (msg.type == MESSAGE_MOUSE_MOVE) {
        gpWindowManager->ConvertToHover(msg);
        if (gpWindowManager->m_lastHoverId == msg.payload.hover.id) {
            return MESSAGE_DISPATCH_CONSUME;
        } else {
            return gpGame->m_viewSpellsCallback(msg);
        }
    }
    if (msg.type == MESSAGE_WIDGET) {
        switch (msg.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                if (msg.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT
                    || ((H2EnumIndex((msg.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) != 0)
                    break;
                {
                    switch (msg.payload.widget.id) {
                        case VIEW_SPELL_MANA_LABEL_ID:
                        case VIEW_SPELL_MANA_HUNDREDS_ID:
                        case VIEW_SPELL_MANA_TENS_ID:
                        case VIEW_SPELL_MANA_ONES_ID:
                            sprintf(
                                gText,
                                cSpellHelp[VIEW_SPELL_HELP_MANA],
                                viewSpellsHero->m_spellPoints
                            );
                            NormalDialog(
                                gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0
                            );
                            break;
                        case VIEW_SPELL_PREVIOUS_ID:
                            if (gpGame->m_viewSpellsTop[H2EnumIndex(gpGame->m_viewSpellsType)] == 0) {
                                gpGame->UpdateSpellWidgets();
                                gpGame->m_viewSpellsWindow->MoveWindow(0, 0);
                                break;
                            }
                            gpGame->m_viewSpellsTop[H2EnumIndex(gpGame->m_viewSpellsType)] -=
                                VIEW_SPELL_PAGE_SIZE;
                            if (gpGame->m_viewSpellsTop[H2EnumIndex(gpGame->m_viewSpellsType)] < 0)
                                gpGame->m_viewSpellsTop[H2EnumIndex(gpGame->m_viewSpellsType)] = 0;
                            gpGame->UpdateSpellWidgets();
                            gpGame->m_viewSpellsWindow->MoveWindow(0, 0);
                            break;
                        case VIEW_SPELL_NEXT_ID:
                            if (gpGame->m_viewSpellsTop[H2EnumIndex(gpGame->m_viewSpellsType)]
                                    + VIEW_SPELL_PAGE_SIZE
                                < gpGame->m_viewSpellsCount[H2EnumIndex(gpGame->m_viewSpellsType)])
                                gpGame->m_viewSpellsTop[H2EnumIndex(gpGame->m_viewSpellsType)] +=
                                    VIEW_SPELL_PAGE_SIZE;
                            gpGame->UpdateSpellWidgets();
                            gpGame->m_viewSpellsWindow->MoveWindow(0, 0);
                            break;
                        case VIEW_SPELL_COMBAT_TAB_ID:
                            gpGame->m_viewSpellsType = SPELL_TYPE_ADVENTURE;
                            gpGame->UpdateSpellWidgets();
                            gpGame->m_viewSpellsWindow->MoveWindow(0, 0);
                            break;
                        case VIEW_SPELL_ADVENTURE_TAB_ID:
                            gpGame->m_viewSpellsType = SPELL_TYPE_COMBAT;
                            gpGame->UpdateSpellWidgets();
                            gpGame->m_viewSpellsWindow->MoveWindow(0, 0);
                            break;
                        case EVENT_WINDOW_FIRST_BUTTON:
                            msg.payload.widget.id = VIEW_SPELL_CLOSE_ID;
                            break;
                    }
                }
                break;
            case WIDGET_COMMAND_SELECT:
            case WIDGET_COMMAND_ALTERNATE_SELECT:
                if (msg.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT
                    || ((H2EnumIndex((msg.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) != 0) {
                    switch (msg.payload.widget.id) {
                        case VIEW_SPELL_ICON_ID_0:
                        case VIEW_SPELL_ICON_ID_1:
                        case VIEW_SPELL_ICON_ID_2:
                        case VIEW_SPELL_ICON_ID_3:
                        case VIEW_SPELL_ICON_ID_4:
                        case VIEW_SPELL_ICON_ID_5:
                        case VIEW_SPELL_ICON_ID_6:
                        case VIEW_SPELL_ICON_ID_7:
                        case VIEW_SPELL_ICON_ID_8:
                        case VIEW_SPELL_ICON_ID_9:
                        case VIEW_SPELL_ICON_ID_10:
                        case VIEW_SPELL_ICON_ID_11:
                            spell = gpGame->m_viewSpellsHero->GetNthSpell(
                                gpGame->m_viewSpellsType,
                                gpGame->m_viewSpellsTop[H2EnumIndex(gpGame->m_viewSpellsType)]
                                    + (msg.payload.widget.id - VIEW_SPELL_ICON_ID_BASE) + 1
                            );
                            NormalDialog(
                                gSpellDesc[H2EnumIndex(spell)],
                                NORMAL_DIALOG_QUICK_VIEW,
                                -1,
                                -1,
                                NORMAL_DIALOG_SPELL,
                                H2EnumIndex(spell),
                                -1,
                                0,
                                -1,
                                0
                            );
                            break;
                        case VIEW_SPELL_PREVIOUS_ID:
                            NormalDialog(
                                cSpellHelp[VIEW_SPELL_HELP_PREVIOUS],
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
                        case VIEW_SPELL_NEXT_ID:
                            NormalDialog(
                                cSpellHelp[VIEW_SPELL_HELP_NEXT],
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
                        case VIEW_SPELL_COMBAT_TAB_ID:
                            NormalDialog(
                                cSpellHelp[VIEW_SPELL_HELP_COMBAT],
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
                        case VIEW_SPELL_ADVENTURE_TAB_ID:
                            NormalDialog(
                                cSpellHelp[VIEW_SPELL_HELP_ADVENTURE],
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
                        case VIEW_SPELL_MANA_LABEL_ID:
                        case VIEW_SPELL_MANA_HUNDREDS_ID:
                        case VIEW_SPELL_MANA_TENS_ID:
                        case VIEW_SPELL_MANA_ONES_ID:
                            sprintf(
                                gText,
                                cSpellHelp[VIEW_SPELL_HELP_MANA],
                                viewSpellsHero->m_spellPoints
                            );
                            NormalDialog(
                                gText, NORMAL_DIALOG_QUICK_VIEW, -1, -1, -1, 0, -1, 0, -1, 0
                            );
                            break;
                    }
                } else {
                    switch (msg.payload.widget.id) {
                        case VIEW_SPELL_ICON_ID_0:
                        case VIEW_SPELL_ICON_ID_1:
                        case VIEW_SPELL_ICON_ID_2:
                        case VIEW_SPELL_ICON_ID_3:
                        case VIEW_SPELL_ICON_ID_4:
                        case VIEW_SPELL_ICON_ID_5:
                        case VIEW_SPELL_ICON_ID_6:
                        case VIEW_SPELL_ICON_ID_7:
                        case VIEW_SPELL_ICON_ID_8:
                        case VIEW_SPELL_ICON_ID_9:
                        case VIEW_SPELL_ICON_ID_10:
                        case VIEW_SPELL_ICON_ID_11:
                            spell = gpGame->m_viewSpellsHero->GetNthSpell(
                                gpGame->m_viewSpellsType,
                                gpGame->m_viewSpellsTop[H2EnumIndex(gpGame->m_viewSpellsType)]
                                    + (msg.payload.widget.id - VIEW_SPELL_ICON_ID_BASE) + 1
                            );
                            if (gpGame->m_viewSpellsReadOnly) {
                                NormalDialog(
                                    gSpellDesc[H2EnumIndex(spell)],
                                    NORMAL_DIALOG_INFO,
                                    -1,
                                    -1,
                                    NORMAL_DIALOG_SPELL,
                                    H2EnumIndex(spell),
                                    -1,
                                    0,
                                    -1,
                                    0
                                );
                                return MESSAGE_DISPATCH_CONSUME;
                            }
                            if (GetManaCost(spell, viewSpellsHero)
                                > viewSpellsHero->m_spellPoints) {
                                sprintf(
                                    gText,
                                    localization::Tr("spell.mana.insufficient")
                                         ,
                                    GetManaCost(spell, viewSpellsHero),
                                    viewSpellsHero->m_spellPoints
                                );
                                NormalDialog(
                                    gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0
                                );
                                return MESSAGE_DISPATCH_CONTINUE;
                            }
                            gpGame->m_viewSpell = spell;
                            msg.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
                            return MESSAGE_DISPATCH_FORWARD;
                    }
                }
                break;
            default:
                break;
        }

        if (msg.payload.widget.id == VIEW_SPELL_CLOSE_ID) {
            msg.payload.widget.command = BaseWidgetCommand(msg.payload.widget.id);
            return MESSAGE_DISPATCH_FORWARD;
        }
    }
    return MESSAGE_DISPATCH_CONSUME;
}

MessageDispatchResult ViewSpecialHandler(tag_message& msg) {
    if (msg.type == MESSAGE_MOUSE_MOVE) {
        if (msg.payload.hover.id == gpWindowManager->m_lastHoverId)
            return MESSAGE_DISPATCH_CONSUME;
        gpWindowManager->m_lastHoverId = msg.payload.hover.id;
        switch (msg.payload.hover.id) {
            case VIEW_SPELL_PREVIOUS_ID:
                strcpy(gText, cSpellHelp[VIEW_SPELL_HELP_PREVIOUS]);
                break;
            case VIEW_SPELL_NEXT_ID:
                strcpy(gText, cSpellHelp[VIEW_SPELL_HELP_NEXT]);
                break;
            case VIEW_SPELL_COMBAT_TAB_ID:
                strcpy(gText, cSpellHelp[VIEW_SPELL_HELP_COMBAT]);
                break;
            case VIEW_SPELL_ADVENTURE_TAB_ID:
                strcpy(gText, cSpellHelp[VIEW_SPELL_HELP_ADVENTURE]);
                break;
            case EVENT_WINDOW_FIRST_BUTTON:
                strcpy(gText, cSpellHelp[VIEW_SPELL_HELP_CLOSE]);
                break;
            case VIEW_SPELL_MANA_LABEL_ID:
            case VIEW_SPELL_MANA_HUNDREDS_ID:
            case VIEW_SPELL_MANA_TENS_ID:
            case VIEW_SPELL_MANA_ONES_ID:
                sprintf(
                    gText,
                    cSpellHelp[VIEW_SPELL_HELP_MANA],
                    viewSpellsHero->m_spellPoints
                );
                break;
            default:
                strcpy(gText, cSpellHelp[VIEW_SPELL_HELP_OTHER]);
                break;
        }
        HeroMessageUpdate(gText);
        return MESSAGE_DISPATCH_CONSUME;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

void game::ViewArmy(
    i32 x,
    i32 y,
    CreatureType monsterType,
    i32 numTroops,
    town* castle,
    i32 disableUpgrade,
    ArmyFacing facing,
    i32 quickView,
    hero* theHero,
    class army* theArmy,
    armyGroup* theGroup,
    i32 groupIndex
) {
    i16 titleMessage14;
    i16 detailMessage0;
    tag_monsterInfo* monster;
    iconWidget* monsterWidget9;
    char* details0;
    i16 blankWidget;
    i32 modifier14;
    i16 quickBaseY;
    tag_monsterInfo* armyMonster2;
    char filename5[VIEW_ARMY_FILENAME_SIZE];
    i16 baseX8;
    i32 loopIndex;
    i32 iconFrame8;
    i16 numWidget5;
    i32 morale;
    i32 luck4;
    char armyName0[VIEW_ARMY_NAME_SIZE];
    icon* monsterIcon5;
    tag_message message;
    i16 frame;

    baseX8 = VIEW_ARMY_UNUSED_BASE_X;
    quickBaseY = VIEW_ARMY_UNUSED_QUICK_BASE_Y;
    blankWidget = VIEW_ARMY_BLANK_WIDGET_ID;
    numWidget5 = VIEW_ARMY_COUNT_WIDGET_ID;
    titleMessage14 = VIEW_ARMY_TITLE_WIDGET_ID;
    detailMessage0 = VIEW_ARMY_DETAIL_WIDGET_ID;
    frame = VIEW_ARMY_MONSTER_WIDGET_ID;
    message.type = MESSAGE_WIDGET;

    iViewArmyFrame = 0;
    iViewArmyType = monsterType;
    iViewArmyNumTroops = numTroops;
    gbAllowUpgrade = false;

    if (castle && (gpAdvManager->m_active == 1 || gpTownManager->m_active == 1)) {
        for (loopIndex = H2EnumIndex(BUILDING_SLOT_DWELLING_SECOND);
             loopIndex <= H2EnumIndex(BUILDING_SLOT_DWELLING_SIXTH);
             loopIndex++) {
            if (gDwellingType[H2EnumIndex(castle->m_type)]
                             [loopIndex - H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)]
                    == monsterType
                && (castle->m_buildings
                    & (1 << (loopIndex + VIEW_ARMY_DWELLING_UPGRADE_OFFSET)))) {
                gbAllowUpgrade = true;
                iViewArmyUpgradeToType = NextCreatureType(monsterType);
            }
        }
        if ((monsterType == CREATURE_GREEN_DRAGON || monsterType == CREATURE_RED_DRAGON)
            && (castle->m_buildings & H2EnumIndex(KB_DWELLING_UPGRADE_SIXTH_FLAG))) {
            gbAllowUpgrade = true;
            iViewArmyUpgradeToType = CREATURE_BLACK_DRAGON;
        }
    }

    monster = &gMonsterDatabase[H2EnumIndex(monsterType)];
    if (theArmy)
        armyMonster2 = &theArmy->m_monster;
    else
        armyMonster2 = &gMonsterDatabase[H2EnumIndex(monsterType)];

    x = VIEW_ARMY_WINDOW_X;
    y = VIEW_ARMY_WINDOW_Y;
    m_viewArmyWindow = new heroWindow(x, y, const_cast<char*>("armywin.bin"));
    if (!m_viewArmyWindow)
        MemError();

    if (facing == ARMY_FACING_RIGHT)
        viewArmyFacingWIPXMod = -1;
    else
        viewArmyFacingWIPXMod = 1;
    gpResourceManager
        ->PointToFile(gpResourceManager->MakeId(cArmyFrameFileNames[H2EnumIndex(monsterType)], 1));
    gpResourceManager->ReadBlock(
        reinterpret_cast<i8*>(&sViewArmyMonFrameInfo),
        sizeof(sViewArmyMonFrameInfo)
    );
    ModifyFrameInfo(&sViewArmyMonFrameInfo, monsterType);
    BuildTempWalkSeq(&sViewArmyMonFrameInfo, 0, 1);

    viewArmyBaseX = VIEW_ARMY_MONSTER_BASE_X;
    strcpy(filename5, cMonFilename[H2EnumIndex(monsterType)]);

    monsterIcon5 = gpResourceManager->GetIcon(filename5);
    iconFrame8 = sViewArmyMonFrameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK)][0];
    viewArmyBaseX += viewArmyFacingWIPXMod
                     * (GetIconEntry(monsterIcon5, iconFrame8)->w
                        / VIEW_ARMY_ICON_CENTER_DIVISOR);
    viewArmyBaseX += GetIconEntry(monsterIcon5, iconFrame8)->x * viewArmyFacingWIPXMod
                     + viewArmyFacingWIPXMod * sViewArmyMonFrameInfo.walkXOffsets[0];
    viewArmyBaseY = VIEW_ARMY_MONSTER_BASE_Y;
    viewArmyBaseY +=
        GetIconEntry(monsterIcon5, iconFrame8)->h / VIEW_ARMY_ICON_CENTER_DIVISOR;
    monsterWidget9 = new iconWidget(
        static_cast<i16>(viewArmyBaseX),
        static_cast<i16>(viewArmyBaseY),
        VIEW_ARMY_MONSTER_WIDGET_WIDTH,
        VIEW_ARMY_MONSTER_WIDGET_HEIGHT,
        filename5,
        sViewArmyMonFrameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK)][0],
        facing == ARMY_FACING_LEFT ? ICON_DRAW_FLIPPED : ICON_DRAW_NORMAL,
        VIEW_ARMY_MONSTER_WIDGET_Z_ORDER,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (!monsterWidget9)
        MemError();
    m_viewArmyWindow->AddWidget(monsterWidget9, -1);
    gpResourceManager->Dispose(monsterIcon5);

    utf8::Copy(
        armyName0,
        sizeof(armyName0),
        gArmyNames[H2EnumIndex(monsterType)]
    );
    utf8::UppercaseFirst(armyName0);
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = VIEW_ARMY_TITLE_WIDGET_ID;
    message.payload.widget.data.text = armyName0;
    m_viewArmyWindow->BroadcastMessage(message);

    details0 = static_cast<char*>(H2_ALLOC(VIEW_ARMY_DETAIL_BUFFER_SIZE));
    if (theGroup)
        morale = theGroup->GetMorale(theHero, castle, NULL);
    else
        morale = 0;
    if ((H2EnumIndex((monster->flags.all) & (MONSTER_FLAGS_NO_MORALE))))
        morale = 0;

    sprintf(
        details0,
        ""
    );
    modifier14 = 0;
    sprintf(
        gText,
        "%s%d",
        cArmyDetail[ARMY_DETAIL_ATTACK],
        static_cast<i32>(monster->attack)
    );
    strcat(details0, gText);
    if (theHero)
        modifier14 += theHero->Stats(HERO_PRIMARY_ATTACK);
    if (theArmy)
        modifier14 = theArmy->m_monster.attack - monster->attack;
    if (modifier14) {
        sprintf(gText, " (%d)", monster->attack + modifier14);
        strcat(details0, gText);
    }

    modifier14 = 0;
    sprintf(
        gText,
        "\n%s%d",
        cArmyDetail[ARMY_DETAIL_DEFENSE],
        static_cast<i32>(monster->defense)
    );
    strcat(details0, gText);
    if (theHero)
        modifier14 += theHero->Stats(HERO_PRIMARY_DEFENSE);
    if (theArmy)
        modifier14 = theArmy->m_monster.defense - monster->defense;
    if (modifier14) {
        sprintf(gText, " (%d)", monster->defense + modifier14);
        strcat(details0, gText);
    }

    if ((H2EnumIndex((monster->flags.all) & (MONSTER_FLAGS_SHOOTER)))) {
        i32 shots8 = armyMonster2->shots;
        if (shots8 > 0) {
            if (gpCombatManager->m_active == 1)
                sprintf(gText, "\n%s%d", cArmyDetail[ARMY_DETAIL_SHOTS_LEFT], shots8);
            else
                sprintf(gText, "\n%s%d", cArmyDetail[ARMY_DETAIL_SHOTS_OUTSIDE], shots8);
            strcat(details0, gText);
        }
    }

    sprintf(
        gText,
        "\n%s%d",
        cArmyDetail[ARMY_DETAIL_DAMAGE],
        static_cast<i32>(monster->damageMin)
    );
    strcat(details0, gText);
    if (monster->damageMin != monster->damageMax) {
        sprintf(gText, "-%d", static_cast<i32>(monster->damageMax));
        strcat(details0, gText);
    }
    sprintf(
        gText,
        "\n%s%d",
        cArmyDetail[ARMY_DETAIL_HIT_POINTS],
        static_cast<u32>(monster->hitPoints)
    );
    strcat(details0, gText);
    if (gpCombatManager->m_active == 1) {
        sprintf(
            gText,
            localization::Tr("army.hit_points_left"),
            static_cast<u32>(monster->hitPoints) - theArmy->m_hitPointsLost
        );
        strcat(details0, gText);
    }
    sprintf(gText, "\n%s%s", cArmyDetail[ARMY_DETAIL_SPEED], speedText[armyMonster2->speed]);
    strcat(details0, gText);
    sprintf(
        gText,
        "\n%s%s",
        cArmyDetail[ARMY_DETAIL_MORALE],
        gMoraleText[morale + VIEW_ARMY_TEXT_NEUTRAL_OFFSET]
    );
    strcat(details0, gText);
    luck4 = GetLuck(theHero, theArmy, castle);
    sprintf(
        gText,
        "\n%s%s",
        cArmyDetail[ARMY_DETAIL_LUCK],
        gLuckText[luck4 + VIEW_ARMY_TEXT_NEUTRAL_OFFSET]
    );
    strcat(details0, gText);

    message.payload.widget.id = VIEW_ARMY_DETAIL_WIDGET_ID;
    message.payload.widget.data.text = details0;
    m_viewArmyWindow->BroadcastMessage(message);
    if (!gbAllowUpgrade) {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        message.payload.widget.id = VIEW_ARMY_UPGRADE_ACTION_ID;
        m_viewArmyWindow->BroadcastMessage(message);
    }
    if (disableUpgrade) {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        message.payload.widget.id = VIEW_ARMY_UPGRADE_ID;
        m_viewArmyWindow->BroadcastMessage(message);
    }
    if (quickView) {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        message.payload.widget.id = VIEW_ARMY_QUICK_VIEW_ID;
        m_viewArmyWindow->BroadcastMessage(message);
    }
    if (numTroops < 1) {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        message.payload.widget.id = VIEW_ARMY_BLANK_WIDGET_ID;
        m_viewArmyWindow->BroadcastMessage(message);
        message.payload.widget.id = VIEW_ARMY_COUNT_WIDGET_ID;
        m_viewArmyWindow->BroadcastMessage(message);
    } else {
        char countText[VIEW_ARMY_COUNT_TEXT_SIZE];
        sprintf(countText, "%d", numTroops);
        message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        message.payload.widget.id = VIEW_ARMY_COUNT_WIDGET_ID;
        message.payload.widget.data.text = countText;
        m_viewArmyWindow->BroadcastMessage(message);
    }

    if (theArmy) {
        iconWidget* spellWidget0;
        i32 spellY10 = VIEW_ARMY_SPELL_BASE_Y;
        i32 spellCenterX27 = VIEW_ARMY_SPELL_CENTER_X;
        if (quickView)
            spellY10 += VIEW_ARMY_QUICK_SPELL_Y_OFFSET;
        i32 spacing5 = VIEW_ARMY_SPELL_SPACING_BASE - theArmy->m_spellCount;
        i32 spellX2 =
            spellCenterX27 + (VIEW_ARMY_SPELL_X_BIAS - theArmy->m_spellCount)
            - (theArmy->m_spellCount * spacing5) / VIEW_ARMY_ICON_CENTER_DIVISOR;
        H2SteppedEnumStorage<ArmySpellInfluence, i32> spellIndex8 =
            ARMY_SPELL_INFLUENCE_NONE;
        for (loopIndex = 0;
             loopIndex < (theArmy->m_spellCount > VIEW_ARMY_SPELL_VISIBLE_LIMIT
                               ? VIEW_ARMY_SPELL_VISIBLE_LIMIT
                               : theArmy->m_spellCount);
             loopIndex++) {
            spellIndex8++;
            for (; spellIndex8 < ARMY_SPELL_INFLUENCE_COUNT; spellIndex8++) {
                if (theArmy->m_spellInfluence[H2EnumIndex(spellIndex8)])
                    break;
            }
            spellWidget0 = new iconWidget(
                static_cast<i16>(spellX2 + loopIndex * spacing5),
                static_cast<i16>(spellY10 + VIEW_ARMY_SPELL_WIDGET_Y_OFFSET),
                0,
                0,
                const_cast<char*>("spellinl.icn"),
                static_cast<i16>(H2EnumIndex(spellIndex8)),
                ICON_DRAW_NORMAL,
                static_cast<i16>(loopIndex + VIEW_ARMY_SPELL_WIDGET_ID_BASE),
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (!spellWidget0)
                MemError();
            m_viewArmyWindow->AddWidget(spellWidget0, -1);
        }
    }

    glTimers[0] = platform::Ticks() + VIEW_ARMY_ANIMATION_INITIAL_DELAY;
    m_viewArmyResult = 0;
    if (quickView) {
        gpWindowManager->AddWindow(m_viewArmyWindow, -1, 1);
        QuickViewWait();
        gpWindowManager->RemoveWindow(m_viewArmyWindow);
    } else {
        gpWindowManager->DoDialog(m_viewArmyWindow, ViewArmyHandler, 0);
        if (gbDismissArmy && theGroup) {
            theGroup->m_troopTypes[groupIndex] = CREATURE_NONE;
            theGroup->m_troopCounts[groupIndex] = 0;
        }
        if (gbUpgradeArmy && theGroup)
            theGroup->m_troopTypes[groupIndex] = iViewArmyUpgradeToType;
    }
    H2_FREE(details0);
    delete m_viewArmyWindow;
}

MessageDispatchResult ViewArmyHandler(tag_message& msg) {
    i32 resourceCost;
    i16 frameDelay6;
    i16 frameOffset;
    i32 goldCost;
    ResourceType resourceType7;

    gbDismissArmy = false;
    gbUpgradeArmy = false;
    frameDelay6 = VIEW_ARMY_HANDLER_FRAME_DELAY;

    if (msg.type == MESSAGE_WIDGET) {
        switch (msg.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                switch (msg.payload.widget.id) {
                    case EVENT_WINDOW_FIRST_BUTTON:
                    case EVENT_WINDOW_SECOND_BUTTON:
                        gpWindowManager->m_dialogResult = msg.payload.widget.id;
                        msg.payload.widget.id = VIEW_ARMY_CLOSE_ID;
                        msg.payload.widget.command = BaseWidgetCommand(VIEW_ARMY_CLOSE_ID);
                        return MESSAGE_DISPATCH_FORWARD;
                    case EVENT_WINDOW_FOURTH_BUTTON:
                        NormalDialog(
                            const_cast<char*>(localization::Tr("army.confirm.dismiss")),
                            NORMAL_DIALOG_CONFIRM,
                            -1,
                            -1,
                            -1,
                            0,
                            -1,
                            0,
                            -1,
                            0
                        );
                        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE) {
                            gbDismissArmy = true;
                            msg.payload.widget.id = VIEW_ARMY_CLOSE_ID;
                            msg.payload.widget.command = BaseWidgetCommand(VIEW_ARMY_CLOSE_ID);
                            return MESSAGE_DISPATCH_FORWARD;
                        }
                        break;
                    case VIEW_ARMY_UPGRADE_ACTION_ID:
                        goldCost = iViewArmyNumTroops * VIEW_ARMY_UPGRADE_COST_MULTIPLIER
                                   * (gMonsterDatabase[H2EnumIndex(iViewArmyUpgradeToType)].cost
                                      - gMonsterDatabase[H2EnumIndex(iViewArmyType)].cost);
                        if (iViewArmyUpgradeToType == CREATURE_BLACK_DRAGON) {
                            resourceType7 = RES_SULFUR;
                            resourceCost =
                                iViewArmyNumTroops * VIEW_ARMY_UPGRADE_COST_MULTIPLIER;
                        } else if (iViewArmyUpgradeToType == CREATURE_TITAN) {
                            resourceType7 = RES_GEMS;
                            resourceCost =
                                iViewArmyNumTroops * VIEW_ARMY_UPGRADE_COST_MULTIPLIER;
                        } else {
                            resourceType7 = RES_NONE;
                            resourceCost = 0;
                        }
                        if (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] >= goldCost
                            && (resourceType7 == RES_NONE
                                || gpCurPlayer->m_resources[H2EnumIndex(resourceType7)] >= resourceCost)) {
                            NormalDialog(
                                const_cast<char*>(
                                    localization::Tr("army.upgrade.confirm")
                                ),
                                NORMAL_DIALOG_CONFIRM,
                                -1,
                                -1,
                                H2EnumIndex(RES_GOLD),
                                goldCost,
                                H2EnumIndex(resourceType7),
                                resourceCost,
                                -1,
                                0
                            );
                            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE) {
                                gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] -= goldCost;
                                if (resourceType7 != RES_NONE)
                                    gpCurPlayer->m_resources[H2EnumIndex(resourceType7)] -= resourceCost;
                                gbUpgradeArmy = true;
                                msg.payload.widget.id = VIEW_ARMY_CLOSE_ID;
                                msg.payload.widget.command =
                                    BaseWidgetCommand(VIEW_ARMY_CLOSE_ID);
                                return MESSAGE_DISPATCH_FORWARD;
                            }
                        } else {
                            NormalDialog(
                                const_cast<char*>(localization::Tr("army.upgrade.cannot_afford")),
                                NORMAL_DIALOG_INFO,
                                -1,
                                -1,
                                H2EnumIndex(RES_GOLD),
                                goldCost,
                                H2EnumIndex(resourceType7),
                                resourceCost,
                                -1,
                                0
                            );
                        }
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    if (glTimers[0] < platform::Ticks()) {
        msg.type = MESSAGE_WIDGET;
        msg.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        msg.payload.widget.id = VIEW_ARMY_MONSTER_WIDGET_ID;
        iViewArmyFrame = (iViewArmyFrame + 1)
                         % sViewArmyMonFrameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)];
        msg.payload.widget.data.value =
            sViewArmyMonFrameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK)][iViewArmyFrame];
        gpGame->m_viewArmyWindow->BroadcastMessage(msg);
        msg.payload.widget.command = WIDGET_COMMAND_SET_X;
        msg.payload.widget.data.value =
            viewArmyBaseX
            + viewArmyFacingWIPXMod * sViewArmyMonFrameInfo.walkXOffsets[iViewArmyFrame];
        gpGame->m_viewArmyWindow->BroadcastMessage(msg);
        gpGame->m_viewArmyWindow->DrawWindow(1, 0, WINDOW_DRAW_ID_LIMIT);
        glTimers[0] = static_cast<i32>(
            platform::Ticks()
            + sViewArmyMonFrameInfo.walkDuration * GAME_VIEW_ARMY_FRAME_DELAY_SCALE
                  / sViewArmyMonFrameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)]
        );
    }
    return MESSAGE_DISPATCH_CONSUME;
}

i32 game::GetRandomNumTroops(CreatureType monsterType) {
    return Random(
        gMonRandBound[H2EnumIndex(monsterType)][0],
        gMonRandBound[H2EnumIndex(monsterType)][1]
    );
}

void game::TurnOnAIMusic(void) {
    gpSoundManager->StopAllSamples(1);
    gpSoundManager->SwitchAmbientMusic(GAME_AI_MUSIC_TRACK);
    gSoundBackendsReady = 0;
}

void game::TurnOffAIMusic(void) {
    gSoundBackendsReady = 1;
}

void game::NextPlayer(void) {
    i32 remotePlayer;
    i32 index;
    i32 humansAlive;

    m_heroRecs[gpCurPlayer->m_availableHeroIds[0]].m_eventFlags = HeroEventFlag(
        static_cast<i32>(m_heroRecs[gpCurPlayer->m_availableHeroIds[0]].m_eventFlags)
        & ~H2EnumIndex(HERO_EVENT_WEEKLY_VISIT)
    );
    m_heroRecs[gpCurPlayer->m_availableHeroIds[1]].m_eventFlags = HeroEventFlag(
        static_cast<i32>(m_heroRecs[gpCurPlayer->m_availableHeroIds[1]].m_eventFlags)
        & ~H2EnumIndex(HERO_EVENT_WEEKLY_VISIT)
    );
    iCurHourGlassPhase = 0;

    if (gbThisNetHumanPlayer[giCurPlayer] && gConfig.autosave) {
        humansAlive = 0;
        for (index = 0; index < GAME_PLAYER_COUNT; index++) {
            if (m_playerDead[index] == 0 && gbHumanPlayer[index])
                humansAlive++;
        }
        SaveGame(
            const_cast<char*>(save_names::Autosave),
            1,
            0
        );
    }

    gpAdvManager->m_identifyHeroActive = 0;
    if (gpGame->m_players[giCurPlayer].m_daysLeft > 0)
        gpGame->m_players[giCurPlayer].m_daysLeft--;
    CheckEndGame(END_GAME_FORCE_NONE, false);
    gpAdvManager->DeactivateCurrTown();
    gpAdvManager->DeactivateCurrHero();

    do {
        giCurPlayer++;
        if (giCurPlayer >= m_playerCount) {
            giCurPlayer = 0;
            PerDay();
        }
    } while (gpGame->m_playerDead[giCurPlayer]);

    gpCurPlayer = &gpGame->m_players[giCurPlayer];
    giCurPlayerBit = static_cast<u8>(1 << giCurPlayer);
    for (index = 0; index < m_players[giCurPlayer].m_heroCount; index++) {
        hero* currentHero = &m_heroRecs[m_players[giCurPlayer].m_heroIds[index]];
        currentHero->m_mobility = currentHero->CalcMobility();
        currentHero->m_remainingMobility = currentHero->m_mobility;
    }

    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        gpMouseManager->SetPointer(1);
        gpAdvManager->HideRoute(1, 0, 1);
        gpAdvManager->CheckDimNextHeroBut();
        TurnOnAIMusic();
        platform::SetDialogMenusEnabled(0);
        giBottomViewOverride = BOTTOM_VIEW_OVERRIDE_DISABLED;
        ShowComputerScreen();
        bShowIt = 0;
        if (gbRemoteOn && gbHumanPlayer[giCurPlayer]) {
            gbThisNetGotAdventureControl = false;
            remotePlayer = gbGamePosToNetPos[giCurPlayer];
            if (!gpGame->TransmitSaveGame(remotePlayer, 0, 0))
                ShutDown(NULL);
        }
        if (giBottomViewOverride == BOTTOM_VIEW_OVERRIDE_DISABLED)
            giBottomViewOverride = BOTTOM_VIEW_NONE;
    } else {
        platform::SetDialogMenusEnabled(1);
        gpInputManager->Flush();
        gbAllBlack = true;
        gpAdvManager->CheckSetEvilInterface(1, giCurPlayer);
        gbAllBlack = false;
        if (gbBlackoutPlayer && giNumHumanPlayers > 1) {
            sprintf(gText, localization::Tr("player.turn"), cPlayerNames[giCurPlayer]);
            WaitForPlayer(gText, giCurPlayer);
        }
        if (gbThisNetHumanPlayer[giCurPlayer])
            CancelComputerScreen();
        giCurWatchPlayerBit = giCurPlayerBit;
        giCurWatchPlayer = giCurPlayer;
    }

    if (gbThisNetHumanPlayer[giCurPlayer] && gbRemoteOn && m_day != 1 && giForceSwitchMusic == -1) {
        gpSoundManager->SwitchAmbientMusic(WAIT_AMBIENT_MUSIC);
        giForceSwitchMusic = platform::Ticks();
        gSoundBackendsReady = 0;
    }
    if (m_day == 1 && giCurTurn != 1)
        gSoundBackendsReady = 0;

    DoNewTurn();
    CheckEndGame(END_GAME_FORCE_NONE, false);
    if (gbThisNetHumanPlayer[giCurPlayer] && gSoundBackendsReady == 0
        && giForceSwitchMusic == -1) {
        gSoundBackendsReady = 1;
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[H2EnumIndex(gpAdvManager->m_currentTerrain)]
        );
        gpAdvManager->SetEnvironmentOrigin(
            gpAdvManager->m_mapOriginX + ENVIRONMENT_ORIGIN_TILE_OFFSET,
            gpAdvManager->m_mapOriginY + ENVIRONMENT_ORIGIN_TILE_OFFSET,
            1
        );
    }
    if (gbThisNetHumanPlayer[giCurPlayer])
        gpAdvManager->ForceNewHover();
    Ironfist_HeroPoolRegainMobility();
}

i32 game::ComputeDailyGold(i32 player) {
    i32 heroIndex;
    i32 dailyGold = 0;
    i32 index;

    for (index = 0; index < GAME_MINE_COUNT; index++) {
        if (m_mines[index].owner == player) {
            if (m_mines[index].resourceType == MINE_TYPE_GOLD)
                dailyGold += DAILY_GOLD_MINE_INCOME;
            if (m_mines[index].resourceType == MINE_TYPE_ALCHEMIST_LAB)
                dailyGold += DAILY_GOLD_MINE_INCOME;
        }
    }

    for (index = 0; index < GAME_TOWN_COUNT; index++) {
        if (m_castleRecs[index].m_owner == player) {
            dailyGold += (m_castleRecs[index].m_buildings & (1 << H2EnumIndex(BUILDING_SLOT_UPGRADE_CASTLE)))
                        ? DAILY_GOLD_VILLAGE_INCOME
                        : DAILY_GOLD_TOWN_INCOME;
            if (m_castleRecs[index].m_buildings & (1 << H2EnumIndex(BUILDING_SLOT_SPECIAL_SEVEN)))
                dailyGold += DAILY_GOLD_STATUE_INCOME;
            if (m_castleRecs[index].m_type == FACTION_WARLOCK
                && (m_castleRecs[index].m_buildings & (1 << H2EnumIndex(BUILDING_SLOT_SPECIAL))))
                dailyGold += DAILY_GOLD_DUNGEON_INCOME;
        }
    }

    dailyGold += m_players[player].NumOfGivenArtifact(ARTIFACT_ENDLESS_SACK_GOLD)
            * DAILY_GOLD_ENDLESS_SACK_INCOME;
    dailyGold += m_players[player].NumOfGivenArtifact(ARTIFACT_ENDLESS_BAG_GOLD)
            * DAILY_GOLD_ENDLESS_BAG_INCOME;
    dailyGold += m_players[player].NumOfGivenArtifact(ARTIFACT_ENDLESS_PURSE_GOLD)
            * DAILY_GOLD_ENDLESS_PURSE_INCOME;
    dailyGold += m_players[player].NumOfGivenArtifact(ARTIFACT_GOLDEN_GOOSE)
            * DAILY_GOLD_GOLDEN_GOOSE_INCOME;
    dailyGold += m_players[player].NumOfGivenArtifact(ARTIFACT_TAX_LIEN)
            * DAILY_GOLD_TAX_LIEN_INCOME;

    for (heroIndex = 0; heroIndex < m_players[player].m_heroCount; heroIndex++) {
        dailyGold += gEstatesGoldLevel
            [H2EnumIndex(gpGame->m_heroRecs[m_players[player].m_heroIds[heroIndex]]
                     .m_secondarySkills[H2EnumIndex(HERO_SKILL_ESTATES)])];
    }

    if (!gbHumanPlayer[player]) {
        if (gpGame->m_difficulty == DIFFICULTY_EASY)
            dailyGold = static_cast<i32>(dailyGold * GAME_DIFFICULTY_EASY_GOLD_FACTOR);
        if (gpGame->m_difficulty == DIFFICULTY_NORMAL)
            dailyGold = static_cast<i32>(dailyGold * GAME_DIFFICULTY_NORMAL_GOLD_FACTOR);
        if (gpGame->m_difficulty == DIFFICULTY_HARD)
            dailyGold = static_cast<i32>(dailyGold * GAME_DIFFICULTY_HARD_GOLD_FACTOR);
        if (gpGame->m_difficulty == DIFFICULTY_EXPERT)
            dailyGold = static_cast<i32>(dailyGold * GAME_DIFFICULTY_EXPERT_GOLD_FACTOR);
        if (gpGame->m_difficulty == DIFFICULTY_IMPOSSIBLE)
            dailyGold = static_cast<i32>(dailyGold * GAME_DIFFICULTY_IMPOSSIBLE_GOLD_FACTOR);
    }

    if (m_playerHandicap[player] == PLAYER_HANDICAP_MODERATE)
        dailyGold = static_cast<i32>(dailyGold * GAME_HANDICAP_MODERATE_RESOURCE_FACTOR);
    else if (m_playerHandicap[player] == PLAYER_HANDICAP_SEVERE)
        dailyGold = static_cast<i32>(dailyGold * GAME_HANDICAP_SEVERE_RESOURCE_FACTOR);
    return dailyGold;
}

void game::PerDay(void) {
    i32 maxSpellPoints9;
    i32 player;
    H2SteppedEnumStorage<ResourceType, i32> resource8;
    i32 income8;
    i32 dailyIncome0;
    MineType resourceType1;
    hero* currentHero7;
    i32 restoredSpellPoints14;
    hero* townHero6;
    town* currentTown1;

    for (player = 0; player < gpGame->m_playerCount; player++) {
        for (resource8 = RES_WOOD; resource8 < RES_COUNT; resource8++) {
            gpGame->m_players[player].m_aiData.m_income[H2EnumIndex(resource8)] =
                -m_players[player].m_resources[H2EnumIndex(resource8)];
        }
    }

    memset(m_dailyEventFlags, 0, sizeof(m_dailyEventFlags));

    for (player = 0; player < GAME_MINE_COUNT; player++) {
        if (m_mines[player].owner != -1) {
            resourceType1 = m_mines[player].resourceType;
            dailyIncome0 = 0;
            if (resourceType1 == MINE_TYPE_ORE)
                dailyIncome0 = DAILY_MINE_BULK_RESOURCE_INCOME;
            else if (resourceType1 == MINE_TYPE_WOOD)
                dailyIncome0 = DAILY_MINE_BULK_RESOURCE_INCOME;
            else if (resourceType1 != MINE_TYPE_GOLD)
                dailyIncome0 = 1;

            if (resourceType1 != MINE_TYPE_GOLD && resourceType1 <= MINE_TYPE_GOLD)
                m_players[m_mines[player].owner].m_resources[H2EnumIndex(resourceType1)] += dailyIncome0;
        }
    }

    for (player = 0; player < GAME_TOWN_COUNT; player++)
        m_castleRecs[player].m_turnsOwned++;

    for (player = 0; player < m_playerCount; player++) {
        m_players[player].m_resources[H2EnumIndex(RES_SULFUR)] +=
            m_players[player].NumOfGivenArtifact(ARTIFACT_ENDLESS_POUCH_SULFUR);
        m_players[player].m_resources[H2EnumIndex(RES_MERCURY)] +=
            m_players[player].NumOfGivenArtifact(ARTIFACT_ENDLESS_VIAL_MERCURY);
        m_players[player].m_resources[H2EnumIndex(RES_GEMS)] +=
            m_players[player].NumOfGivenArtifact(ARTIFACT_ENDLESS_POUCH_GEMS);
        m_players[player].m_resources[H2EnumIndex(RES_WOOD)] +=
            m_players[player].NumOfGivenArtifact(ARTIFACT_ENDLESS_CORD_WOOD);
        m_players[player].m_resources[H2EnumIndex(RES_ORE)] +=
            m_players[player].NumOfGivenArtifact(ARTIFACT_ENDLESS_CART_ORE);
        m_players[player].m_resources[H2EnumIndex(RES_CRYSTAL)] +=
            m_players[player].NumOfGivenArtifact(ARTIFACT_ENDLESS_POUCH_CRYSTAL);
        m_players[player].m_resources[H2EnumIndex(RES_GOLD)] += ComputeDailyGold(player);
    }

    if (xIsPlayingExpansionCampaign && xCampaign.HasAward(AWARD_WOOD_BONUS))
        m_players[0].m_resources[H2EnumIndex(RES_WOOD)] += DAILY_CAMPAIGN_WOOD_BONUS;

    for (player = 0; player < m_playerCount; player++) {
        if (!gbHumanPlayer[player]) {
            if (gpGame->m_difficulty >= DIFFICULTY_HARD) {
                m_players[player].m_resources[H2EnumIndex(RES_WOOD)]++;
                m_players[player].m_resources[H2EnumIndex(RES_ORE)]++;
            }
            if (gpGame->m_difficulty >= DIFFICULTY_EXPERT
                && m_day >= DAILY_RESOURCE_BONUS_FIRST_DAY
                && m_day <= DAILY_RESOURCE_BONUS_LAST_DAY)
                m_players[player].m_resources[m_day - 1] += 1;
            if (gpGame->m_difficulty >= DIFFICULTY_IMPOSSIBLE
                && m_day >= DAILY_RESOURCE_BONUS_FIRST_DAY
                && m_day <= DAILY_RESOURCE_BONUS_LAST_DAY)
                m_players[player].m_resources[m_day - 1] += 1;
            if (gpGame->m_players[player].m_aiDifficulty == PLAYER_PERSONALITY_BUILDER
                && m_day >= DAILY_RESOURCE_BONUS_FIRST_DAY
                && m_day <= DAILY_RESOURCE_BONUS_LAST_DAY)
                m_players[player].m_resources[m_day - 1] += 1;
        }
    }

    m_day++;
    giCurTurn = m_day + (m_week - 1) * EVENT_DAYS_PER_WEEK
                + (m_month - 1) * EVENT_DAYS_PER_MONTH;
    if (!gbGameOver) {
        if (m_day > EVENT_DAYS_PER_WEEK) {
            m_day = 1;
            PerWeek();
        }
        if (m_week > GAME_WEEKS_PER_MONTH) {
            m_week = 1;
            PerMonth();
        }
    }

    for (player = 0; player < GAME_HERO_COUNT; player++)
        m_heroRecs[player].m_eventFlags = HeroEventFlag(
            static_cast<i32>(m_heroRecs[player].m_eventFlags) & ~H2EnumIndex(WEEKLY_HERO_RESERVED_FLAG)
        );

    for (player = 0; player < gpGame->m_playerCount; player++) {
        for (resource8 = RES_WOOD; resource8 < RES_GOLD; resource8++) {
            if (m_playerHandicap[player] != PLAYER_HANDICAP_NONE) {
                m_players[player].m_resources[H2EnumIndex(resource8)] -= static_cast<i32>(
                    (gpGame->m_players[player].m_aiData.m_income[H2EnumIndex(resource8)]
                     + m_players[player].m_resources[H2EnumIndex(resource8)])
                    * (m_playerHandicap[player] == PLAYER_HANDICAP_MODERATE ? GAME_HANDICAP_MODERATE_DAILY_PENALTY : GAME_HANDICAP_SEVERE_DAILY_PENALTY)
                );
            }
        }
    }

    for (player = 0; player < gpGame->m_playerCount; player++) {
        for (resource8 = RES_WOOD; resource8 < RES_COUNT; resource8++) {
            gpGame->m_players[player].m_aiData.m_income[H2EnumIndex(resource8)] +=
                m_players[player].m_resources[H2EnumIndex(resource8)];
        }
    }

    for (player = 0; player < GAME_HERO_COUNT; player++) {
        currentHero7 = &m_heroRecs[player];
        restoredSpellPoints14 = currentHero7->m_spellPoints;
        maxSpellPoints9 =
            currentHero7->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE;
        restoredSpellPoints14 +=
            H2EnumIndex(currentHero7->m_secondarySkills[H2EnumIndex(HERO_SKILL_MYSTICISM)]) + 1;
        if (currentHero7->HasArtifact(ARTIFACT_POWER_RING))
            restoredSpellPoints14 += POWER_RING_DAILY_MANA_BONUS;
        if (restoredSpellPoints14 > maxSpellPoints9)
            restoredSpellPoints14 = maxSpellPoints9;
        if (restoredSpellPoints14 > currentHero7->m_spellPoints)
            currentHero7->m_spellPoints = static_cast<i16>(restoredSpellPoints14);
        if ((H2EnumIndex((currentHero7->m_eventFlags) & (HERO_EVENT_MAGIC_WELL))))
            ((currentHero7->m_eventFlags) &= ~(HERO_EVENT_MAGIC_WELL));
    }

    for (player = 0; player < GAME_TOWN_COUNT; player++) {
        currentTown1 = GetTown(player);
        if (!(currentTown1->m_buildings & (1 << H2EnumIndex(BUILDING_SLOT_MAGE_GUILD))))
            continue;
        if (currentTown1->m_occupyingHeroId != -1) {
            townHero6 = GetHero(currentTown1->m_occupyingHeroId);
            maxSpellPoints9 =
                townHero6->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE;
            if (maxSpellPoints9 > townHero6->m_spellPoints)
                townHero6->m_spellPoints = static_cast<i16>(maxSpellPoints9);
        }
    }
    Ironfist_NewDay();
}

void game::PerWeek(void) {
    FactionType heroClass = FACTION_KNIGHT;
    i32 mapY7;
    i32 mapX10;
    FactionType desiredClass;
    i32 outerIndex;
    i32 innerIndex;
    i32 monsterCount;
    i32 growth2;
    town* castle5;
    hero* weeklyHero;
    i32 monsterIncrease8;

    giWeekType = CALENDAR_PERIOD_NORMAL;
    giWeekTypeExtra = Random(0, WEEK_NAME_LAST);
    if (m_week != GAME_WEEKS_PER_MONTH) {
        outerIndex = Random(1, SPECIAL_WEEK_ROLL_MAX);
        if (outerIndex == 1) {
            giWeekType = CALENDAR_PERIOD_CREATURE;
            // Any randomizable creature can name the week, Ironfist's included.
            giWeekTypeExtra = H2EnumIndex(
                CREATURES_RANDOMIZABLE[Random(0, CREATURES_RANDOMIZABLE.size() - 1)]
            );
        }
    }

    for (outerIndex = 0; outerIndex < GAME_TOWN_COUNT; outerIndex++) {
        castle5 = GetTown(outerIndex);
        for (innerIndex = WEEKLY_FIRST_DWELLING; innerIndex <= WEEKLY_LAST_DWELLING;
             innerIndex++) {
            if (castle5->m_buildings & (1 << innerIndex)) {
                growth2 = gMonsterDatabase[H2EnumIndex(gDwellingType[H2EnumIndex(castle5->m_type)]
                                                             [innerIndex - WEEKLY_FIRST_DWELLING])]
                               .growth;
                if (castle5->m_buildings & (1 << H2EnumIndex(BUILDING_SLOT_SPECIAL_FOUR)))
                    growth2 += CASTLE_GROWTH_SPECIAL_BONUS;
                if (innerIndex == WEEKLY_FIRST_DWELLING
                    && (castle5->m_buildings & (1 << H2EnumIndex(BUILDING_SLOT_WELL_EXTRA))))
                    growth2 += CASTLE_GROWTH_WELL_BONUS;
                if (castle5->m_owner == -1)
                    growth2 /= NEUTRAL_CASTLE_GROWTH_DIVISOR;
                if (castle5->m_owner >= 0
                    && castle5->m_garrison[innerIndex - WEEKLY_FIRST_DWELLING] == 0
                    && !gbHumanPlayer[castle5->m_owner]) {
                    if (gpGame->m_difficulty == DIFFICULTY_HARD)
                        growth2 = static_cast<i32>(growth2 * WEEKLY_HARD_GROWTH_FACTOR);
                    if (gpGame->m_difficulty == DIFFICULTY_EXPERT)
                        growth2 = static_cast<i32>(growth2 * WEEKLY_EXPERT_GROWTH_FACTOR);
                    if (gpGame->m_difficulty == DIFFICULTY_IMPOSSIBLE)
                        growth2 = static_cast<i32>(growth2 * WEEKLY_IMPOSSIBLE_GROWTH_FACTOR);
                }
                if (giWeekType == CALENDAR_PERIOD_CREATURE
                    && H2EnumIndex(gDwellingType[H2EnumIndex(castle5->m_type)]
                                        [innerIndex - WEEKLY_FIRST_DWELLING])
                           == giWeekTypeExtra)
                    growth2 += CREATURE_WEEK_GROWTH_BONUS;
                castle5->m_garrison[innerIndex - WEEKLY_FIRST_DWELLING] += growth2;
            }
        }
    }

    for (outerIndex = 0; outerIndex < GAME_PLAYER_COUNT; outerIndex++) {
        for (innerIndex = 0; innerIndex < AVAILABLE_HERO_SLOTS; innerIndex++) {
            if (innerIndex == 1) {
                heroClass =
                    m_heroRecs[gpGame->m_players[outerIndex].m_availableHeroIds[0]].m_cursorType;
            }
            heroClass =
                static_cast<FactionType>((Random(1, H2EnumIndex(FACTION_COUNT) - 1) + H2EnumIndex(heroClass)) % H2EnumIndex(FACTION_COUNT));
            desiredClass = heroClass;
            if (innerIndex == 0
                && m_setupPlayerRace[gcColorToSetupPos[m_players[outerIndex].m_color]]
                       < FACTION_COUNT) {
                desiredClass =
                    m_setupPlayerRace[gcColorToSetupPos[m_players[outerIndex].m_color]];
            }

            if (gpGame->m_availableHeroes[gpGame->m_players[outerIndex].m_availableHeroIds[innerIndex]]
                == WEEKLY_AVAILABLE_HERO) {
                if ((H2EnumIndex((gpGame
                            ->m_heroRecs[gpGame->m_players[outerIndex].m_availableHeroIds[innerIndex]]
                            .m_eventFlags) & (WEEKLY_HERO_RESERVED_FLAG))))
                    continue;
            }
            {
                if (gpGame->m_availableHeroes[gpGame->m_players[outerIndex].m_availableHeroIds[innerIndex]]
                    == WEEKLY_AVAILABLE_HERO)
                    gpGame->m_availableHeroes[gpGame->m_players[outerIndex].m_availableHeroIds[innerIndex]] = -1;
                if (innerIndex == 1 && !gbHumanPlayer[outerIndex])
                    desiredClass = FACTION_ANY;
                gpGame->m_players[outerIndex].m_availableHeroIds[innerIndex] =
                    static_cast<i8>(gpGame->GetNewHeroId(
                        outerIndex,
                        desiredClass,
                        !gbHumanPlayer[outerIndex] && gpGame->m_difficulty > DIFFICULTY_EASY
                    ));
                m_availableHeroes[gpGame->m_players[outerIndex].m_availableHeroIds[innerIndex]] = WEEKLY_AVAILABLE_HERO;
            }
        }
    }

    for (mapY7 = 0; mapY7 < MAP_HEIGHT; mapY7++) {
        for (mapX10 = 0; mapX10 < MAP_WIDTH; mapX10++) {
            switch (WORLDMAP->GetCell(mapX10, mapY7)->m_triggerType) {
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER: {
                    monsterCount = WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata
                                     & H2EnumIndex(MAP_MONSTER_COUNT_MASK);
                    monsterIncrease8 = monsterCount / EVENT_DAYS_PER_WEEK;
                    if (Random(1, EVENT_DAYS_PER_WEEK)
                        <= static_cast<i32>(monsterCount % EVENT_DAYS_PER_WEEK))
                        monsterIncrease8++;
                    monsterCount += monsterIncrease8;
                    if (monsterCount > WEEKLY_MONSTER_LIMIT)
                        monsterCount = WEEKLY_MONSTER_LIMIT;
                    WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata =
                        (WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata
                         & H2EnumIndex(MAP_MONSTER_GUARD_FLAG))
                        | monsterCount;
                    break;
                }
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTESIAN_SPRING:
                    WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata = 1;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WATER_WHEEL:
                    if (WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata
                        != WEEKLY_WATER_WHEEL_EMPTY)
                        WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata = 2;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MAGIC_GARDEN:
                    WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata = Random(0, 1) ? 7 : 6;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WINDMILL:
                    WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata = Random(1, 5);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARCHER_HOUSE:
                    if (WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata += Random(2, 4);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_GOBLIN_HUT:
                    if (WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata += Random(3, 6);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DWARF_COTTAGE:
                    if (WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata += Random(2, 4);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_PEASANT_HUT:
                    if (WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata += Random(5, 10);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_LOG_CABIN:
                    if (WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata += Random(5, 10);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DESERT_TENT:
                    if (WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata += Random(1, 3);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WAGON_CAMP:
                    if (WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata += Random(3, 6);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TREE_HOUSE:
                    if (WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata += Random(4, 8);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SIRENS:
                    if (WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata += Random(3, 6);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WATCH_TOWER:
                    if (WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata += Random(1, 4);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RUINS:
                    if (WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata += Random(1, 3);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TREE_CITY:
                    if (WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata
                        < WEEKLY_MONSTER_POPULATION_LIMIT)
                        WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata += Random(10, 20);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CAVE:
                    if (WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata += Random(3, 6);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_EXCAVATION:
                    if (WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata += Random(4, 8);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HALFLING_HOLE:
                    if (WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata += Random(5, 10);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TROLL_BRIDGE:
                    if (!(WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata
                          & WEEKLY_DWELLING_NO_GROWTH_FLAG)
                        && WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata
                               < WEEKLY_DRAGON_CITY_LIMIT)
                        WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata += Random(1, 3);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CITY_OF_DEAD:
                    if (!(WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata
                          & WEEKLY_DWELLING_NO_GROWTH_FLAG)
                        && WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata
                               < WEEKLY_DRAGON_CITY_LIMIT)
                        WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata += Random(1, 3);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DRAGON_CITY:
                    if (!(WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata
                          & WEEKLY_DWELLING_NO_GROWTH_FLAG)
                        && WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata
                               < WEEKLY_DRAGON_CITY_LIMIT)
                        WORLDMAP->GetCell(mapX10, mapY7)->m_objectMetadata += 1;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_EXPANSION_DWELLING:
                    WeeklyRecruitSite(WORLDMAP->GetCell(mapX10, mapY7));
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_EXPANSION_OBJECT:
                    WeeklyGenericSite(WORLDMAP->GetCell(mapX10, mapY7));
                    break;
                default:
                    break;
            }
        }
    }

    for (outerIndex = 0; outerIndex < GAME_HERO_COUNT; outerIndex++) {
        weeklyHero = &m_heroRecs[outerIndex];
        if ((H2EnumIndex((weeklyHero->m_eventFlags) & (WEEKLY_HERO_VISIT_FLAG))))
            ((weeklyHero->m_eventFlags) &= ~(WEEKLY_HERO_VISIT_FLAG));
    }

    m_week++;
    SetupNewRumour();
    GiveTroopsToNeutralTowns();
    Ironfist_UndoWellGrowth();
}

void game::WeeklyRecruitSite(mapCell* cell) {
    i32 type = cell->m_objectMetadata;
    type &= WEEKLY_RECRUIT_TYPE_MASK;
    i32 recruitCount = cell->m_objectMetadata;
    recruitCount >>= WEEKLY_RECRUIT_COUNT_SHIFT;
    i32 value;

    switch (type) {
        case 0:
            recruitCount += Random(WEEKLY_RECRUIT_MIN_GROWTH, WEEKLY_RECRUIT_MAX_GROWTH);
            break;
        case 1:
            recruitCount += Random(WEEKLY_RECRUIT_MIN_GROWTH, WEEKLY_RECRUIT_MAX_GROWTH);
            break;
        case 2:
            recruitCount += Random(WEEKLY_RECRUIT_MIN_GROWTH, WEEKLY_RECRUIT_MAX_GROWTH);
            break;
        case 3:
            recruitCount += Random(WEEKLY_RECRUIT_MIN_GROWTH, WEEKLY_RECRUIT_MAX_GROWTH);
            break;
        case 4:
            recruitCount += Random(WEEKLY_RECRUIT_MIN_GROWTH, WEEKLY_RECRUIT_MAX_GROWTH);
            break;
    }

    if (recruitCount > WEEKLY_RECRUIT_LIMIT)
        recruitCount = WEEKLY_RECRUIT_LIMIT;
    value = (recruitCount << WEEKLY_RECRUIT_COUNT_SHIFT) | type;
    cell->m_objectMetadata = value;
}

void game::WeeklyGenericSite(mapCell* cell) {
    i32 type = cell->m_objectMetadata;
    type &= WEEKLY_SITE_TYPE_MASK;
    switch (type) {
        case 4:
            cell->m_objectMetadata = type;
            break;
    }
}

void game::PerMonth(void) {
    mapCell* spot;
    i32 x;
    i32 i;
    i32 y;
    i32 growth;
    town* twn;
    i32 j;

    m_month++;
    i = Random(ROLL_MIN, ROLL_MAX);
    if (i <= NORMAL_ROLL_MAX) {
        giMonthType = CALENDAR_PERIOD_NORMAL;
        giMonthTypeExtra = Random(NORMAL_NAME_MIN, NORMAL_NAME_MAX);
    } else if (i <= CREATURE_ROLL_MAX) {
        giMonthType = CALENDAR_PERIOD_CREATURE;
        giMonthTypeExtra = giMonType[Random(CREATURE_LIST_MIN, CREATURE_LIST_MAX)];
    } else {
        giMonthType = CALENDAR_PERIOD_PLAGUE;
    }

    for (i = 0; i < GAME_TOWN_COUNT; i++) {
        for (j = WEEKLY_FIRST_DWELLING; j <= WEEKLY_LAST_DWELLING; j++) {
            twn = GetTown(i);
            if (twn->m_buildings & (1 << j)) {
                growth = gMonsterDatabase[H2EnumIndex(gDwellingType[H2EnumIndex(twn->m_type)]
                                                           [j - WEEKLY_FIRST_DWELLING])]
                              .growth;
                if (twn->m_buildings & WELL_BUILDING)
                    growth += WELL_GROWTH;
                if (j == WEEKLY_FIRST_DWELLING
                    && (twn->m_buildings & FIRST_DWELLING_BONUS_BUILDING))
                    growth += FIRST_DWELLING_GROWTH;

                if (giMonthType == CALENDAR_PERIOD_CREATURE
                    && H2EnumIndex(gDwellingType[H2EnumIndex(twn->m_type)][j - WEEKLY_FIRST_DWELLING])
                           == giMonthTypeExtra)
                    twn->m_garrison[j - WEEKLY_FIRST_DWELLING] *= CREATURE_MONTH_MULTIPLIER;

                if (giMonthType == CALENDAR_PERIOD_PLAGUE && !IsWellDisabled()) {
                    twn->m_garrison[j - WEEKLY_FIRST_DWELLING] -= growth;
                    if (twn->m_garrison[j - WEEKLY_FIRST_DWELLING] < 0)
                        twn->m_garrison[j - WEEKLY_FIRST_DWELLING] = 0;
                    twn->m_garrison[j - WEEKLY_FIRST_DWELLING] =
                        twn->m_garrison[j - WEEKLY_FIRST_DWELLING] >> 1;
                }
            }
        }
    }

    if (giMonthType == CALENDAR_PERIOD_CREATURE) {
        for (x = 0; x < MAP_WIDTH; x++) {
            for (y = 0; y < MAP_HEIGHT; y++) {
                spot = gpAdvManager->GetCell(x, y);
                if (spot->m_triggerType == MAP_OBJECT_NONE && !spot->m_objectLayerBit1
                    && !spot->m_objectLayerBit0
                    && giGroundToTerrain[spot->m_terrainImageIndex] != TERRAIN_WATER) {
                    if (Random(MONSTER_SPAWN_MIN, MONSTER_SPAWN_MAX)
                        == MONSTER_SPAWN_ROLL) {
                        spot->m_triggerType = MONSTER_TRIGGER;
                        spot->m_objectTileset = TILESET_MONS32;
                        spot->m_objectIndex = static_cast<u8>(giMonthTypeExtra);
                        spot->m_objectMetadata =
                            GetRandomNumTroops(static_cast<CreatureType>(giMonthTypeExtra))
                            + GetRandomNumTroops(static_cast<CreatureType>(giMonthTypeExtra));
                        if (Random(MONSTER_GUARD_ROLL_MIN, MONSTER_GUARD_ROLL_MAX)
                            < MONSTER_GUARD_CUTOFF)
                            spot->m_objectMetadata |= H2EnumIndex(MAP_MONSTER_GUARD_FLAG);
                    }
                }
            }
        }
    }

    // With the Well disabled the plague math no longer adds up; the month
    // becomes a benign one instead.
    if (IsWellDisabled() && giMonthType == CALENDAR_PERIOD_PLAGUE) {
        giMonthType = CALENDAR_PERIOD_NORMAL;
        giMonthTypeExtra = Random(NORMAL_NAME_MIN, NORMAL_NAME_MAX);
    }

    gpAdvManager->CompleteDraw(0);
}

void game::ConvertObject(
    i32 left,
    i32 top,
    i32 right,
    i32 bottom,
    TilesetId oldTileset,
    i32 oldFirstIndex,
    i32 oldLastIndex,
    TilesetId newTileset,
    i32 newFirstIndex,
    MapObjectType oldTrigger,
    MapObjectType newTrigger
) {
    i32 x;
    i32 y;
    mapCell* cell;
    mapCellExtra* ext;

    for (x = left; x <= right; x++) {
        for (y = top; y <= bottom; y++) {
            if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
                continue;
            cell = WORLDMAP->GetCell(x, y);
            if (cell->m_objectIndex != static_cast<u8>(-1)
                && cell->m_objectTileset == oldTileset
                && cell->m_objectIndex >= oldFirstIndex
                && cell->m_objectIndex <= oldLastIndex) {
                cell->m_objectTileset = newTileset;
                cell->m_objectIndex =
                    static_cast<u8>(cell->m_objectIndex - oldFirstIndex + newFirstIndex);
            }
            if ((cell->m_triggerType & MAP_TRIGGER_TYPE_MASK) == oldTrigger)
                cell->m_triggerType =
                    (cell->m_triggerType & MAP_TRIGGER_ACTION_FLAG) | newTrigger;

            if (cell->m_extraIndex != 0
                && WORLDMAP->Extra(cell->m_extraIndex)->objectIndex != static_cast<u8>(-1))
                ext = WORLDMAP->Extra(cell->m_extraIndex);
            else
                ext = NULL;
            while (ext != NULL) {
                if (ext->objectTileset == oldTileset
                    && ext->objectIndex >= oldFirstIndex
                    && ext->objectIndex <= oldLastIndex) {
                    ext->objectTileset = newTileset;
                    ext->objectIndex =
                        static_cast<u8>(ext->objectIndex - oldFirstIndex + newFirstIndex);
                }
                if (ext->nextIndex != 0
                    && WORLDMAP->Extra(ext->nextIndex)->objectIndex != static_cast<u8>(-1))
                    ext = WORLDMAP->Extra(ext->nextIndex);
                else
                    ext = NULL;
            }

            if (cell->m_overlayIndex != static_cast<u8>(-1)
                && cell->m_overlayTileset == oldTileset
                && cell->m_overlayIndex >= oldFirstIndex
                && cell->m_overlayIndex <= oldLastIndex) {
                cell->m_overlayTileset = newTileset;
                cell->m_overlayIndex =
                    static_cast<u8>(cell->m_overlayIndex - oldFirstIndex + newFirstIndex);
            }
            if (cell->m_extraIndex != 0
                && WORLDMAP->Extra(cell->m_extraIndex)->overlayIndex != static_cast<u8>(-1))
                ext = WORLDMAP->Extra(cell->m_extraIndex);
            else
                ext = NULL;
            while (ext != NULL) {
                if (ext->overlayTileset == oldTileset
                    && ext->overlayIndex >= oldFirstIndex
                    && ext->overlayIndex <= oldLastIndex) {
                    ext->overlayTileset = newTileset;
                    ext->overlayIndex =
                        static_cast<u8>(ext->overlayIndex - oldFirstIndex + newFirstIndex);
                }
                if (ext->nextIndex != 0
                    && WORLDMAP->Extra(ext->nextIndex)->overlayIndex != static_cast<u8>(-1))
                    ext = WORLDMAP->Extra(ext->nextIndex);
                else
                    ext = NULL;
            }
        }
    }
}

void game::RandomizeTown(i32 x, i32 y, i32) {
    i32 unused[RANDOM_TOWN_SCRATCH_WIDTH];
    i32 townId = GetTownId(x, y);
    town* castle = GetTown(townId);
    mapTownExtra* townExtra =
        reinterpret_cast<mapTownExtra*>(ppMapExtra[WORLDMAP->GetCell(x, y)->m_objectMetadata]);
    FactionType race;

    if (townExtra->color == RANDOM_TOWN_UNOWNED_COLOR)
        race = FACTIONS_ACTUAL[Random(0, FACTIONS_ACTUAL.size() - 1)];
    else
        race = m_setupPlayerRace[gcColorToSetupPos[townExtra->color]];

    castle->m_turnsOwned = RANDOM_TOWN_AGE;
    const i32 spriteRace =
        race == FACTION_CYBORG ? IRONFIST_CYBORG_SPRITE_SLOT : H2EnumIndex(race);
    ConvertObject(
        x + RANDOM_TOWN_LEFT,
        y + RANDOM_TOWN_TOP,
        x + RANDOM_TOWN_RIGHT,
        y + RANDOM_TOWN_BOTTOM,
        RANDOM_TOWN_SOURCE_TILESET,
        RANDOM_TOWN_OBJECT_SOURCE_FIRST,
        RANDOM_TOWN_OBJECT_SOURCE_LAST,
        RANDOM_TOWN_OBJECT_TILESET,
        spriteRace << RANDOM_TOWN_RACE_FRAME_SHIFT,
        MAP_OBJECT_RANDOM_TOWN,
        MAP_OBJECT_CASTLE
    );
    ConvertObject(
        x + RANDOM_TOWN_LEFT,
        y + RANDOM_TOWN_TOP,
        x + RANDOM_TOWN_RIGHT,
        y + RANDOM_TOWN_BOTTOM,
        RANDOM_TOWN_SOURCE_TILESET,
        RANDOM_TOWN_OVERLAY_SOURCE_FIRST,
        RANDOM_TOWN_OVERLAY_SOURCE_LAST,
        RANDOM_TOWN_OVERLAY_TILESET,
        spriteRace << RANDOM_TOWN_RACE_FRAME_SHIFT,
        MAP_OBJECT_RANDOM_TOWN,
        MAP_OBJECT_CASTLE
    );
    ConvertObject(
        x + RANDOM_TOWN_LEFT,
        y + RANDOM_TOWN_TOP,
        x + RANDOM_TOWN_RIGHT,
        y + RANDOM_TOWN_BOTTOM,
        RANDOM_TOWN_SOURCE_TILESET,
        RANDOM_TOWN_OBJECT_SOURCE_FIRST,
        RANDOM_TOWN_OBJECT_SOURCE_LAST,
        RANDOM_TOWN_OBJECT_TILESET,
        spriteRace << RANDOM_TOWN_RACE_FRAME_SHIFT,
        MAP_OBJECT_RANDOM_CASTLE,
        MAP_OBJECT_CASTLE
    );
    ConvertObject(
        x + RANDOM_TOWN_LEFT,
        y + RANDOM_TOWN_TOP,
        x + RANDOM_TOWN_RIGHT,
        y + RANDOM_TOWN_BOTTOM,
        RANDOM_TOWN_SOURCE_TILESET,
        RANDOM_TOWN_OVERLAY_SOURCE_FIRST,
        RANDOM_TOWN_OVERLAY_SOURCE_LAST,
        RANDOM_TOWN_OVERLAY_TILESET,
        spriteRace << RANDOM_TOWN_RACE_FRAME_SHIFT,
        MAP_OBJECT_RANDOM_CASTLE,
        MAP_OBJECT_CASTLE
    );
    m_castleRecs[townId].m_type = race;
}

void game::RandomizeMine(i32 x, i32 y) {
    u8 objFrame;
    i32 mineIdx;
    i32 iRow;
    i32 count;
    i32 iCol;
    MapObjectType trigger;
    MineType resType;
    H2EnumStorage<TerrainType, i32> terrain;
    u8 mineFrame;

    terrain = giGroundToTerrain[WORLDMAP->GetCell(x, y)->m_terrainImageIndex];
    for (count = 0; count < RANDOM_MINE_RETRY_LIMIT; count++) {
        switch (terrain) {
            case TERRAIN_GRASS:
            case TERRAIN_DIRT:
                resType = static_cast<MineType>(Random(H2EnumIndex(MINE_TYPE_MERCURY), H2EnumIndex(MINE_TYPE_GOLD)));
                if (resType == MINE_TYPE_MERCURY)
                    resType = MINE_TYPE_WOOD;
                break;
            case TERRAIN_SNOW:
                resType = static_cast<MineType>(Random(H2EnumIndex(MINE_TYPE_ORE), H2EnumIndex(MINE_TYPE_GOLD)));
                break;
            case TERRAIN_SWAMP:
                resType = static_cast<MineType>(Random(H2EnumIndex(MINE_TYPE_WOOD), H2EnumIndex(MINE_TYPE_GOLD)));
                break;
            case TERRAIN_LAVA:
                resType = MINE_TYPE_MERCURY;
                break;
            default:
                resType = static_cast<MineType>(Random(H2EnumIndex(MINE_TYPE_MERCURY), H2EnumIndex(MINE_TYPE_GOLD)));
                break;
        }
        if (RandMineQty[H2EnumIndex(resType)] == 0)
            count = RANDOM_MINE_RETRY_LIMIT;
    }
    RandMineQty[H2EnumIndex(resType)]++;

    switch (resType) {
        case MINE_TYPE_WOOD:
            mineFrame = 5;
            break;
        case MINE_TYPE_MERCURY:
            mineFrame = 25;
            break;
        default:
            switch (terrain) {
                case TERRAIN_GRASS:
                    mineFrame = 15;
                    break;
                case TERRAIN_SNOW:
                    mineFrame = 19;
                    break;
                default:
                    mineFrame = 9;
                    break;
            }
            break;
    }

    switch (resType) {
        case MINE_TYPE_WOOD:
            objFrame = 7;
            break;
        case MINE_TYPE_MERCURY:
            switch (terrain) {
                case TERRAIN_SWAMP:
                    objFrame = 43;
                    break;
                case TERRAIN_LAVA:
                    objFrame = 35;
                    break;
                default:
                    objFrame = 27;
                    break;
            }
            break;
        default:
            switch (terrain) {
                case TERRAIN_GRASS:
                    objFrame = 17;
                    break;
                case TERRAIN_SNOW:
                    objFrame = 21;
                    break;
                case TERRAIN_SWAMP:
                    objFrame = 23;
                    break;
                case TERRAIN_DESERT:
                    objFrame = 13;
                    break;
                default:
                    objFrame = 11;
                    break;
            }
            break;
    }

    WORLDMAP->GetCell(x, y)->m_objectIndex = objFrame;
    WORLDMAP->GetCell(x + 1, y)->m_objectIndex = objFrame + 1;
    WORLDMAP->GetCell(x, y - 1)->m_overlayIndex = mineFrame;
    WORLDMAP->GetCell(x + 1, y - 1)->m_overlayIndex = mineFrame + 1;

    if (resType == MINE_TYPE_MERCURY) {
        WORLDMAP->GetCell(x + 1, y)->m_objType |= 1;
        trigger = MAP_OBJECT_ALCHEMIST_LAB;
    } else if (resType == MINE_TYPE_WOOD) {
        trigger = MAP_OBJECT_SAWMILL;
    } else {
        m_worldMap.ChangeTilesetIndex(
            WORLDMAP->GetCell(x + 1, y),
            x + 1,
            y,
            TILESET_EXTRAOVR,
            H2EnumIndex(resType) - H2EnumIndex(MINE_TYPE_ORE),
            0,
            -1
        );
        trigger = MAP_OBJECT_MINE;
    }

    mineIdx = GetMineId(x, y);
    for (iRow = 0; iRow < RANDOM_MINE_FOOTPRINT_WIDTH; iRow++) {
        for (iCol = 0; iCol < RANDOM_MINE_FOOTPRINT_WIDTH; iCol++) {
            if ((WORLDMAP->GetCell(x + iCol, y - iRow)->m_triggerType
                 & MAP_TRIGGER_TYPE_MASK)
                    > MAP_OBJECT_NONE
                && H2EnumIndex(WORLDMAP->GetCell(x + iCol, y - iRow)->m_triggerType
                       & MAP_TRIGGER_TYPE_MASK)
                       <= MINE_FLAG_OVERWRITE_LIMIT) {

            } else {
                WORLDMAP->GetCell(x + iCol, y - iRow)->m_objectMetadata = mineIdx;
                WORLDMAP->GetCell(x + iCol, y - iRow)->m_triggerType = trigger;
            }
        }
    }
    WORLDMAP->GetCell(x, y)->m_triggerType |= MAP_TRIGGER_ACTION_FLAG;
    m_mines[mineIdx].resourceType = resType;
}

void game::InitRandomArtifacts(void) {
    ResetGeneratedArtifacts();
    for (i32 x = 0; x < MAP_WIDTH; ++x) {
        for (i32 y = 0; y < MAP_HEIGHT; ++y) {
            mapCell* tile = m_worldMap.GetCell(x, y);
            if (tile->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTIFACT)) {
                // Each artifact has two tiles; convert tile id to artifact id.
                GenerateArtifact(static_cast<u8>(tile->m_objectIndex) / 2);
            }
        }
    }
}

i32 game::GetRandomArtifactId(
    ArtifactLevelMask levelMask,
    b32 allowCursed
) {
    i32 winConditionArtifact = -1;
    if (m_mapHeader.victoryCondition == MAP_VICTORY_FIND_ARTIFACT) {
        winConditionArtifact = m_mapHeader.victoryConditionValue - 1;
    }

    // Cursed artifacts are allowed 70% of the time, every time.
    const bool cursedAllowed = (allowCursed && Random(0, 100) >= 30);

    std::vector<int> choices;
    const i32 numArtifacts =
        xIsExpansionMap ? KB_ARTIFACT_TABLE_CAPACITY : H2EnumIndex(ARTIFACT_MAGIC_BOOK) + 1;

    for (i32 i = 0; i < numArtifacts; ++i) {
        if ((GetArtifactLevel(i) & H2EnumIndex(levelMask)) == 0) {
            continue;
        }
        if (!IsArtifactGenerationAllowed(i)) {
            continue;
        }
        if (!cursedAllowed && IsCursedItem(i)) {
            continue;
        }
        if (i == winConditionArtifact) {
            continue;
        }

        choices.push_back(i);
    }

    // Ran out of artifacts, so reset the used ones and try again.
    if (choices.empty()) {
        ResetGeneratedArtifacts(H2EnumIndex(levelMask));
        return GetRandomArtifactId(levelMask, allowCursed);
    }

    const i32 artifactId = choices[Random(0, choices.size() - 1)];
    GenerateArtifact(artifactId);
    return artifactId;
}

i32 IsCursedItem(ArtifactType item) {
    if (item == ARTIFACT_FIZBIN_OF_MISFORTUNE || item == ARTIFACT_HIDEOUS_MASK
        || item == ARTIFACT_TAX_LIEN || item == ARTIFACT_ARM_OF_MARTYR
        || item == ARTIFACT_BROACH_SHIELDING || item == ARTIFACT_HEART_FIRE
        || item == ARTIFACT_HEART_ICE)
        return 1;
    return 0;
}

void game::RandomizeHeroPool(void) {
    for (i32 idx = 0; idx < H2EnumIndex(GAME_HERO_COUNT); idx++) {
        m_heroRecs[idx].m_experience = Random(0, 50) + 40;
        SetRandomHeroArmies(idx, 0);
        m_heroRecs[idx].m_remainingMobility = m_heroRecs[idx].CalcMobility();
        m_heroRecs[idx].m_mobility = m_heroRecs[idx].m_remainingMobility;
        m_heroRecs[idx].m_randomSeed = static_cast<u8>(Random(1, 255));
        m_heroRecs[idx].m_enabled = 1;
        switch (m_heroRecs[idx].m_cursorType) {
            case FACTION_SORCERESS:
                m_heroRecs[idx].AddSpell(
                    SPELL_BLESS, m_heroRecs[idx].Stats(HERO_PRIMARY_KNOWLEDGE)
                );
                break;
            case FACTION_WARLOCK:
                m_heroRecs[idx].AddSpell(
                    SPELL_CURSE, m_heroRecs[idx].Stats(HERO_PRIMARY_KNOWLEDGE)
                );
                break;
            case FACTION_NECROMANCER:
                m_heroRecs[idx].AddSpell(
                    SPELL_HASTE, m_heroRecs[idx].Stats(HERO_PRIMARY_KNOWLEDGE)
                );
                break;
            case FACTION_WIZARD:
                m_heroRecs[idx].AddSpell(
                    SPELL_STONE_SKIN, m_heroRecs[idx].Stats(HERO_PRIMARY_KNOWLEDGE)
                );
                break;
            default:
                break;
        }
    }
}

void game::SetRandomHeroArmies(i32 heroId, i32 strongArmy) {
    armyGroup* heroArmy = &m_heroRecs[heroId].m_army;
    randomHeroCreatureInfo* creatureFaction =
        randomHeroArmyBounds[H2EnumIndex(m_heroRecs[heroId].m_cursorType)];
    bool hasTier[IRONFIST_HERO_ARMY_TIER_COUNT];

    hasTier[0] = true;
    hasTier[1] = Random(0, 99) < 87;

    for (i32 slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        heroArmy->m_creatureTypes[slot] = CREATURE_NONE;
        heroArmy->m_quantities[slot] = 0;
    }
    for (i32 tier = 0; tier < IRONFIST_HERO_ARMY_TIER_COUNT; ++tier) {
        if (hasTier[tier]) {
            double randomLowerBound = creatureFaction[tier].lowQuantity;
            double randomUpperBound = creatureFaction[tier].highQuantity + 0.9;
            if (strongArmy) {
                // The average of the bounds, shifting odds toward higher counts.
                randomLowerBound = (randomUpperBound + randomLowerBound) / 2;
            }
            const i32 numCreatures = Random(
                static_cast<i32>(randomLowerBound), static_cast<i32>(randomUpperBound)
            );
            GiveArmy(heroArmy, creatureFaction[tier].creatureType, numCreatures, tier);
        }
    }
}

void game::ProcessRandomObjects(void) {
    i32 artifactId;
    i32 minValue;
    i32 mineIndex2;
    i32 x;
    i32 y;
    i32 maxValue;
    mapCell* cell;
    MapObjectType randomObjectType8;
    i32 randomType8;

    giUABaseX = -1;
    giUABaseY = -1;
    giUARadius = 0;
    for (mineIndex2 = 0; mineIndex2 < RANDOM_MINE_RESOURCE_COUNT; mineIndex2++)
        RandMineQty[mineIndex2] = 0;

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            cell = WORLDMAP->GetCell(x, y);
            switch (cell->m_triggerType) {
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT:
                    giUABaseX = static_cast<i16>(x);
                    giUABaseY = static_cast<i16>(y);
                    giUARadius = static_cast<i16>(cell->m_objectMetadata);
                    cell->m_triggerType = MAP_OBJECT_NONE;
                    cell->m_objectTileset = TILESET_NONE;
                    cell->m_objectIndex = -1;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_TOWN:
                    RandomizeTown(x, y, 0);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_CASTLE:
                    RandomizeTown(x, y, 1);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MONSTER:
                    minValue = 80;
                    maxValue = 2000;
                    goto randomMonster;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MONSTER_WEAK:
                    minValue = 0;
                    maxValue = 400;
                    goto randomMonster;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MONSTER_MEDIUM:
                    minValue = 400;
                    maxValue = 1000;
                    goto randomMonster;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MONSTER_STRONG:
                    minValue = 1000;
                    maxValue = 2500;
                    goto randomMonster;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG:
                    minValue = 2500;
                    maxValue = 100000;
                    goto randomMonster;
                randomMonster:
                    if (cell->m_objectTileset == TILESET_MONS32
                        && cell->m_objectIndex >= RANDOM_MONSTER_SPRITE_FIRST
                        && cell->m_objectIndex <= RANDOM_MONSTER_SPRITE_LAST) {
                        randomObjectType8 = static_cast<MapObjectType>(
                            cell->m_objectIndex + RANDOM_MONSTER_SPRITE_TO_TRIGGER
                        );
                        switch (randomObjectType8) {
                            case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MONSTER_WEAK:
                                minValue = 0;
                                maxValue = 400;
                                goto monsterBoundsReady;
                            case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MONSTER_MEDIUM:
                                minValue = 400;
                                maxValue = 1000;
                                goto monsterBoundsReady;
                            case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MONSTER_STRONG:
                                minValue = 1000;
                                maxValue = 2500;
                                goto monsterBoundsReady;
                            case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG:
                                minValue = 2500;
                                maxValue = 100000;
                                goto monsterBoundsReady;
                        }
                    }
                monsterBoundsReady:
                    cell->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER;
                    do {
                        cell->m_objectIndex = static_cast<u8>(H2EnumIndex(
                            CREATURES_RANDOMIZABLE[
                                Random(0, CREATURES_RANDOMIZABLE.size() - 1)
                            ]
                        ));
                    } while (gMonsterDatabase[cell->m_objectIndex].randomValue <= minValue
                             || gMonsterDatabase[cell->m_objectIndex].randomValue >= maxValue);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_RESOURCE:
                    cell->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RESOURCE;
                    randomType8 = Random(0, 6);
                    ConvertObject(
                        x - 1,
                        y,
                        x - 1,
                        y,
                        TILESET_OBJNRSRC,
                        16,
                        16,
                        TILESET_OBJNRSRC,
                        randomType8 * 2,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    ConvertObject(
                        x,
                        y,
                        x,
                        y,
                        TILESET_OBJNRSRC,
                        17,
                        17,
                        TILESET_OBJNRSRC,
                        randomType8 * 2 + 1,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    switch (randomType8) {
                        case 0:
                        case 2:
                            cell->m_objectMetadata = Random(8, 16);
                            break;
                        case 6:
                            cell->m_objectMetadata = Random(5, 10);
                            break;
                        default:
                            cell->m_objectMetadata = Random(3, 7);
                            break;
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_ARTIFACT:
                    artifactId = GetRandomArtifactId(ARTIFACT_LEVEL_RANDOM, false);
                    cell->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTIFACT;
                    ConvertObject(
                        x - 1,
                        y,
                        x - 1,
                        y,
                        TILESET_OBJNARTI,
                        162,
                        162,
                        TILESET_OBJNARTI,
                        artifactId * 2,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    ConvertObject(
                        x,
                        y,
                        x,
                        y,
                        TILESET_OBJNARTI,
                        163,
                        163,
                        TILESET_OBJNARTI,
                        artifactId * 2 + 1,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_TREASURE_ARTIFACT:
                    artifactId = GetRandomArtifactId(ARTIFACT_LEVEL_TREASURE, false);
                    cell->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTIFACT;
                    ConvertObject(
                        x - 1,
                        y,
                        x - 1,
                        y,
                        TILESET_OBJNARTI,
                        166,
                        166,
                        TILESET_OBJNARTI,
                        artifactId * 2,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    ConvertObject(
                        x,
                        y,
                        x,
                        y,
                        TILESET_OBJNARTI,
                        167,
                        167,
                        TILESET_OBJNARTI,
                        artifactId * 2 + 1,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MINOR_ARTIFACT:
                    artifactId = GetRandomArtifactId(ARTIFACT_LEVEL_MINOR, false);
                    cell->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTIFACT;
                    ConvertObject(
                        x - 1,
                        y,
                        x - 1,
                        y,
                        TILESET_OBJNARTI,
                        168,
                        168,
                        TILESET_OBJNARTI,
                        artifactId * 2,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    ConvertObject(
                        x,
                        y,
                        x,
                        y,
                        TILESET_OBJNARTI,
                        169,
                        169,
                        TILESET_OBJNARTI,
                        artifactId * 2 + 1,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MAJOR_ARTIFACT:
                    artifactId = GetRandomArtifactId(ARTIFACT_LEVEL_MAJOR, false);
                    cell->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTIFACT;
                    ConvertObject(
                        x - 1,
                        y,
                        x - 1,
                        y,
                        TILESET_OBJNARTI,
                        170,
                        170,
                        TILESET_OBJNARTI,
                        artifactId * 2,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    ConvertObject(
                        x,
                        y,
                        x,
                        y,
                        TILESET_OBJNARTI,
                        171,
                        171,
                        TILESET_OBJNARTI,
                        artifactId * 2 + 1,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_EYE_OF_MAGI:
                    RandomizeMine(x, y);
                    break;
            }
        }
    }
}

void game::SetVisibility(i32 x, i32 y, i32 player, i32 radius) {
    i32 i;
    i32 cutoff;
    i32 j;
    u8 mask = static_cast<u8>(1 << player);
    i32 vis;
    i32 distance;

    if (!gbHumanPlayer[player]) {
        if (giCurTurn > MIDDLE_TURN_LAST) {
            radius += LATE_AI_BONUS;
        } else {
            if (giCurTurn > EARLY_TURN_LAST)
                radius += MIDDLE_AI_BONUS;
            else
                radius += EARLY_AI_BONUS;
        }
    }

    if (radius >= SMALL_RADIUS_LIMIT)
        cutoff = LARGE_RADIUS_THRESHOLD;
    else
        cutoff = SMALL_RADIUS_THRESHOLD;

    if (radius >= RADIAL_RADIUS_LIMIT) {
        for (j = 0; j < MAP_HEIGHT; j++) {
            for (i = 0; i < MAP_WIDTH; i++) {
                distance = static_cast<i32>(
                    sqrt(static_cast<double>((y - j) * (y - j) + (x - i) * (x - i)))
                );
                if (distance < radius) {
                    MAP_EXTRA_AT_WFIRST(i, j) |= mask;
                }
            }
        }
    } else {
        for (j = y - radius; j <= y + radius; j++) {
            for (i = x - radius; i <= x + radius; i++) {
                vis = radius - abs(y - j) + radius - abs(x - i);
                if (vis >= cutoff && i >= 0 && j >= 0 && i < MAP_WIDTH
                    && j < MAP_HEIGHT) {
                    MAP_EXTRA_AT_WFIRST(i, j) |= mask;
                }
            }
        }
    }
    Ironfist_SetVisibilityShared(x, y, player, radius);
}

void game::MakeAllWaterVisible(i32 player) {
    char mask = static_cast<char>(1 << player);
    i32 x;
    i32 y;
    for (x = 0; x < MAP_WIDTH; x++) {
        for (y = 0; y < MAP_HEIGHT; y++) {
            fullMap* map = WORLDMAP;
            if (giGroundToTerrain[map->Column(x)[y * map->width].m_terrainImageIndex]
                == TERRAIN_WATER)
                MAP_EXTRA_AT_WFIRST(x, y) |= mask;
        }
    }
    Ironfist_AllWaterVisibleShared(player);
}

void game::GiveArmy(
    armyGroup* group,
    CreatureType type,
    i32 count,
    i32 slot
) {
    i32 swap;
    i32 i;
    if (slot >= 0) {
        i = slot;
        group->m_creatureTypes[i] = type;
        group->m_creatureCounts[i] = 0;
    } else {
        for (i = 0; i < ARMY_GROUP_SLOT_COUNT; i++) {
            if (group->m_creatureTypes[i] == type)
                break;
        }
        if (i >= ARMY_GROUP_SLOT_COUNT) {
            for (i = 0; i < ARMY_GROUP_SLOT_COUNT; i++) {
                if (group->m_creatureTypes[i] < CREATURE_VALID_BEGIN) {
                    group->m_creatureCounts[i] = 0;
                    break;
                }
            }
        }
        if (i >= ARMY_GROUP_SLOT_COUNT)
            return;
    }
    group->m_creatureTypes[i] = type;
    group->m_creatureCounts[i] += count;
}

i32 game::ExperienceValueOfStack(armyGroup* group, hero* h) {
    i32 exp = 0;
    i32 i;
    for (i = 0; i < ARMY_GROUP_SLOT_COUNT; i++) {
        if (group->m_quantities[i] > 0) {
            exp += group->m_quantities[i]
                * gMonsterDatabase[H2EnumIndex(group->m_creatureTypes[i])].hitPoints;
        }
    }
    if (h != NULL)
        exp += EXPERIENCE_HERO_PRESENCE_BONUS;
    return exp;
}

i32 game::GetLuck(hero* h, class army* stack, town* castle) {
    i32 luck;
    if (h == NULL)
        return Ironfist_CalcLuck(h, stack, castle, NEUTRAL);
    luck = NEUTRAL;
    if (h->HasArtifact(ARTIFACT_RABBIT_FOOT))
        luck++;
    if (h->HasArtifact(ARTIFACT_GOLDEN_HORSESHOE))
        luck++;
    if (h->HasArtifact(ARTIFACT_GAMBLERS_COIN))
        luck++;
    if (h->HasArtifact(ARTIFACT_FOUR_LEAF_CLOVER))
        luck++;
    if (h->HasArtifact(ARTIFACT_MASTHEAD) && (H2EnumIndex((h->m_eventFlags) & (HERO_EVENT_EMBARKED)))) {
        luck++;
    }
    luck += h->m_luck;
    luck += H2EnumIndex(h->m_secondarySkills[H2EnumIndex(HERO_SKILL_LUCK)]);
    if (castle != NULL && castle->m_type == FACTION_SORCERESS
        && (castle->m_buildings & H2EnumIndex(TOWN_BUILDING_RAINBOW))) {
        luck += RAINBOW_BONUS;
    }
    if (luck < MINIMUM)
        luck = MINIMUM;
    if (luck > MAXIMUM)
        luck = MAXIMUM;
    if (h->HasArtifact(ARTIFACT_BATTLE_GARB))
        luck = MAXIMUM;
    return Ironfist_CalcLuck(h, stack, castle, luck);
}

void game::SetupAdjacentMons(void) {
    i32 col2;
    i32 row13;
    u8 mask = H2EnumIndex(MAP_EXTRA_ADJACENT_CLEAR_MASK);
    i32 x;
    i32 y;

    for (x = 0; x < MAP_WIDTH; x++) {
        for (y = 0; y < MAP_HEIGHT; y++) {
            if (gpAdvManager->FindAdjacentMonster(x, y, &col2, &row13, -1, -1))
                MAP_EXTRA_AT(x, y) |= H2EnumIndex(MAP_EXTRA_ADJACENT_MONSTER);
            else
                MAP_EXTRA_AT(x, y) &= H2EnumIndex(mask);
        }
    }
}

void game::CancelComputerScreen(void) {
    TurnOffAIMusic();
    bShowIt = 1;
    i32 i;
    for (i = COMPUTER_SCREEN_WIDGET_FIRST; i <= COMPUTER_SCREEN_WIDGET_LAST; i++) {
        gpWindowManager->BroadcastMessage(
            MESSAGE_WIDGET,
            WIDGET_COMMAND_CLEAR_FLAGS,
            i,
            H2EnumIndex(WIDGET_FLAG_UPDATE | WIDGET_FLAG_DIMMED)
        );
    }
}

void game::ShowComputerScreen(void) {
    if (gConfig.blackoutComputer) {
        i32 saved = gbThisNetHumanPlayer[giCurPlayer];
        gbThisNetHumanPlayer[giCurPlayer] = 1;
        i32 i;
        for (i = COMPUTER_SCREEN_WIDGET_FIRST; i <= COMPUTER_SCREEN_WIDGET_LAST; i++)
            gpWindowManager->BroadcastMessage(
                MESSAGE_WIDGET,
                WIDGET_COMMAND_SET_FLAGS,
                i,
                H2EnumIndex(WIDGET_FLAG_UPDATE | WIDGET_FLAG_DIMMED)
            );
        gbAllBlack = true;
        gpAdvManager->CompleteDraw(1);
        gpAdvManager->UpdateHeroLocators(1, 1);
        gpAdvManager->UpdateTownLocators(1, 1);
        gpAdvManager->UpdBottomView(1, 1, 1);
        gpAdvManager->UpdateScreen(0, 1);
        gbAllBlack = false;
        gbThisNetHumanPlayer[giCurPlayer] = static_cast<i8>(saved);
    }
    ShowHeroesLogo();
}

void game::ShowHeroesLogo(void) {
    if (gpAdvManager->m_openState == 0) {
        gpAdvManager->m_openState = 1;
        icon* theIcon = gpResourceManager->GetIcon("herologo.icn");
        IconToBitmap(
            theIcon,
            gpWindowManager->m_screen,
            HEROES_LOGO_X,
            HEROES_LOGO_Y,
            0,
            ICON_DRAW_NO_CLIP,
            0,
            0,
            GAME_SCREEN_WIDTH,
            GAME_SCREEN_HEIGHT,
            0
        );
        gpWindowManager->UpdateScreenRegion(
            HEROES_LOGO_X,
            HEROES_LOGO_Y,
            HEROES_LOGO_WIDTH,
            HEROES_LOGO_HEIGHT
        );
        gpResourceManager->Dispose(static_cast<resource*>(theIcon));
    }
}

void game::WaitForPlayer(char* text, i32 player) {
    if (gbBlackoutPlayer && giNumHumanPlayers > 1 && !gbRemoteOn) {
        gpMouseManager->SetPointer(0);
        gbAllBlack = true;
        giBottomViewOverrideEndTime = platform::Ticks() + WAIT_BOTTOM_VIEW_TIMEOUT;
        if (gbThisNetHumanPlayer[giCurPlayer])
            giBottomViewOverride = BOTTOM_VIEW_NEW_TURN;
        else
            giBottomViewOverride = BOTTOM_VIEW_NONE;
        gSoundBackendsReady = 1;
        gpSoundManager->SwitchAmbientMusic(WAIT_AMBIENT_MUSIC);
        gpAdvManager->CompleteDraw(1);
        gpAdvManager->UpdateHeroLocators(1, 1);
        gpAdvManager->UpdateTownLocators(1, 1);
        gpAdvManager->UpdateScreen(0, 1);
        ShowHeroesLogo();
        gbAllBlack = false;
        NormalDialog(
            text,
            1,
            -1,
            -1,
            WAIT_DIALOG_TYPE,
            gpGame->m_players[player].m_color,
            -1,
            0,
            -1,
            0
        );
        gpSoundManager->SwitchAmbientMusic(-1);
    }
}

i32 game::HasLateOverlay(i32 col, i32 row) {
    mapCell* cell = WORLDMAP->GetCell(col, row);
    if (cell->m_drawOverlayOnTop)
        return 1;
    mapCellExtra* extra;

    if (cell->m_extraIndex)
        extra = WORLDMAP->Extra(cell->m_extraIndex);
    else
        extra = NULL;
    while (extra) {
        if (extra->drawOverlayOnTop)
            return 1;
        if (extra->nextIndex)
            extra = WORLDMAP->Extra(extra->nextIndex);
        else
            extra = NULL;
    }
    return 0;
}

void game::ConvertFlagToLateOverlay(i32 col, i32 row) {
    mapCell* cell = WORLDMAP->GetCell(col, row);
    if (cell->m_overlayTileset == TILESET_FLAG32)
        cell->m_drawOverlayOnTop = 1;
    mapCellExtra* extra;

    if (cell->m_extraIndex)
        extra = WORLDMAP->Extra(cell->m_extraIndex);
    else
        extra = NULL;
    while (extra) {
        if (extra->overlayTileset == TILESET_FLAG32)
            extra->drawOverlayOnTop = 1;
        if (extra->nextIndex)
            extra = WORLDMAP->Extra(extra->nextIndex);
        else
            extra = NULL;
    }
}

i32 game::HasObjectTilesetIndex(
    i32 col,
    i32 row,
    TilesetId tileset,
    i32 index
) {
    mapCell* cell = WORLDMAP->GetCell(col, row);
    if (cell->m_objectTileset == tileset && cell->m_objectIndex == index)
        return 1;
    mapCellExtra* extra;

    if (cell->m_extraIndex)
        extra = WORLDMAP->Extra(cell->m_extraIndex);
    else
        extra = NULL;
    while (extra) {
        if (extra->objectTileset == tileset && extra->objectIndex == index)
            return 1;
        if (extra->nextIndex)
            extra = WORLDMAP->Extra(extra->nextIndex);
        else
            extra = NULL;
    }
    return 0;
}

void game::ConvertAllToLateOverlay(i32 col, i32 row) {
    mapCell* cell = WORLDMAP->GetCell(col, row);
    if (cell->m_overlayIndex != MAPCELL_SPRITE_NONE)
        cell->m_drawOverlayOnTop = 1;
    mapCellExtra* extra;

    if (cell->m_extraIndex)
        extra = WORLDMAP->Extra(cell->m_extraIndex);
    else
        extra = NULL;
    while (extra) {
        if (extra->overlayIndex != MAPCELL_SPRITE_NONE)
            extra->drawOverlayOnTop = 1;
        if (extra->nextIndex)
            extra = WORLDMAP->Extra(extra->nextIndex);
        else
            extra = NULL;
    }
}

void game::ProcessMapExtra(void) {
    i32 unused;
    i32 cost;
    mapCell* cell10;
    i32 townId;
    i32 row16;
    i32 col6;

    for (row16 = 0; row16 < MAP_HEIGHT; row16++) {
        for (col6 = 0; col6 < MAP_WIDTH; col6++) {
            cell10 = WORLDMAP->GetCell(col6, row16);
            switch (cell10->m_triggerType) {
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE:
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_TOWN:
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_CASTLE:
                    townId = GetTownId(col6, row16);
                    m_castleRecs[townId].m_extraIndex = cell10->m_objectMetadata;
                    cell10->m_objectMetadata = townId;
                    break;
            }
        }
    }

    for (row16 = 0; row16 < MAP_HEIGHT; row16++) {
        for (col6 = 0; col6 < MAP_WIDTH; col6++) {
            cell10 = WORLDMAP->GetCell(col6, row16);
            if (cell10->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MINE) && row16 > 0
                && HasLateOverlay(col6, row16 - 1)) {
                ConvertFlagToLateOverlay(col6, row16);
            }
            if (cell10->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ALCHEMIST_LAB)) {
                if (row16 > 0)
                    ConvertFlagToLateOverlay(col6, row16 - 1);
                if (row16 > 1)
                    ConvertFlagToLateOverlay(col6, row16 - ALCHEMIST_LATE_OVERLAY_OFFSET);
            }
        }
    }

    for (row16 = 0; row16 < MAP_HEIGHT; row16++) {
        for (col6 = 0; col6 < MAP_WIDTH; col6++) {
            if (HasLateOverlay(col6, row16))
                ConvertAllToLateOverlay(col6, row16);
        }
    }
}

void game::SetupTowns(void) {
    u32 extraIndex27;
    town* castle8;
    i32 townIndex1;
    i32 slot12;
    i32 unused;
    mapTownExtra* extra0;
    i32 attempts17;
    i8 usedSpells0[H2EnumIndex(SPELL_COUNT)];
    i32 owner12;
    i32 combatSpells27;
    i32 roll8;
    SpellType spell1;
    i32 spellLevel;
    i32 spellIndex3;
    i32 spellsPerLevel12[TOWN_MAGE_GUILD_LEVEL_COUNT];
    char defaultDwellingRoll15[DEFAULT_DWELLING_ROLL_CAPACITY];
    i32 dwellingCount1;
    i32 spellValue;

    for (townIndex1 = 0; townIndex1 < GAME_TOWN_COUNT; townIndex1++) {
        if (!m_castleRecs[townIndex1].m_onMap)
            continue;
        castle8 = GetTown(townIndex1);

        extraIndex27 = castle8->m_extraIndex;
        extra0 = reinterpret_cast<mapTownExtra*>(ppMapExtra[extraIndex27]);
        if (extra0->color == -1)
            owner12 = -1;
        else
            owner12 = gcColorToPlayerPos[extra0->color];
        ClaimTown(townIndex1, owner12, 0);
        castle8->m_originalOwner = static_cast<i8>(owner12);

        if (extra0->hasCustomArmy) {
            for (slot12 = 0; slot12 < ARMY_GROUP_SLOT_COUNT; slot12++) {
                castle8->m_army.m_troopCounts[slot12] = extra0->troopCounts[slot12];
                if (static_cast<i16>(castle8->m_army.m_troopCounts[slot12]) > 0)
                    castle8->m_army.m_troopTypes[slot12] = extra0->troopTypes[slot12];
                else
                    castle8->m_army.m_troopTypes[slot12] = CREATURE_NONE;
            }
        } else {
            for (slot12 = 0; slot12 < ARMY_GROUP_SLOT_COUNT; slot12++) {
                castle8->m_army.m_troopCounts[slot12] = 0;
                castle8->m_army.m_troopTypes[slot12] = CREATURE_NONE;
            }
            GiveTroopsToNeutralTown(townIndex1);
            GiveTroopsToNeutralTown(townIndex1);
            GiveTroopsToNeutralTown(townIndex1);
            GiveTroopsToNeutralTown(townIndex1);
        }

        if (extra0->hasCustomBuildings) {
            castle8->m_buildings =
                (castle8->m_buildings & (H2EnumIndex(TOWN_BUILDING_CASTLE) | H2EnumIndex(TOWN_BUILDING_TENT)))
                | (extra0->buildings & gTownEligibleBuildMask[H2EnumIndex(castle8->m_type)]);
            castle8->m_buildState = extra0->mageGuildLevel;
        } else {
            defaultDwellingRoll15[0] = 1;
            defaultDwellingRoll15[1] = 1;
            defaultDwellingRoll15[2] = 1;
            defaultDwellingRoll15[3] = 2;
            defaultDwellingRoll15[4] = 1;
            defaultDwellingRoll15[5] = 1;
            defaultDwellingRoll15[6] = 1;
            defaultDwellingRoll15[7] = 2;
            defaultDwellingRoll15[8] = 1;
            defaultDwellingRoll15[9] = 2;
            dwellingCount1 =
                defaultDwellingRoll15[Random(0, 99) / DEFAULT_DWELLING_ROLL_BUCKET_COUNT];
            castle8->m_buildings |= H2EnumIndex(TOWN_BUILDING_DWELLING_1);
            if (!gbHumanPlayer[castle8->m_owner] && dwellingCount1 == 1 && Random(1, 10) < 4)
                dwellingCount1++;
            if (--dwellingCount1 != 0) {
                castle8->m_buildings |= H2EnumIndex(TOWN_BUILDING_DWELLING_2);
                dwellingCount1--;
            }
            castle8->m_buildState = 0;
        }

        for (slot12 = TOWN_UPGRADE_BUILDING_FIRST; slot12 <= TOWN_UPGRADE_BUILDING_LAST;
             slot12++) {
            if (castle8->m_buildings & (1 << slot12)) {
                if (slot12 == TOWN_UPGRADE_BUILDING_LAST)
                    castle8->m_buildings &=
                        ~(H2EnumIndex(TOWN_BUILDING_DWELLING_6)
                          | H2EnumIndex(TOWN_BUILDING_UPGRADED_DWELLING_6));
                else
                    castle8->m_buildings &=
                        -1 - (1 << (slot12 - TOWN_UPGRADE_TO_DWELLING_OFFSET));
            }
        }
        for (slot12 = TOWN_DWELLING_BUILDING_FIRST;
             slot12 <= TOWN_DWELLING_BUILDING_LAST;
             slot12++) {
            if (castle8->m_buildings & (1 << slot12)) {
                castle8->m_garrison[slot12 - TOWN_DWELLING_BUILDING_FIRST] =
                    gMonsterDatabase[H2EnumIndex(gDwellingType[H2EnumIndex(castle8->m_type)]
                                      [slot12 - TOWN_DWELLING_BUILDING_FIRST])]
                        .growth;
            }
        }
        if (castle8->m_buildings & H2EnumIndex(TOWN_BUILDING_MAGE_GUILD)) {
            for (slot12 = 1; slot12 <= castle8->m_buildState; slot12++) {
                if (castle8->m_type == FACTION_CYBORG)
                    castle8->m_spellCounts[slot12] = ironfistCyborgSpellLimits[slot12 - 1];
                else
                    castle8->m_spellCounts[slot12] = gSpellLimits[slot12 - 1];
                if (castle8->m_type == FACTION_WIZARD
                    && (castle8->m_buildings & (1 << H2EnumIndex(BUILDING_SLOT_SPECIAL))))
                    castle8->m_spellCounts[slot12]++;
            }
        }
        if (extra0->hasShrine)
            castle8->m_buildings |= H2EnumIndex(TOWN_BUILDING_CAPTAIN_QUARTERS);
        castle8->m_mayNotUpgradeToCastle = extra0->unknown28;
        const std::string townName = localization::DecodeExternalText(extra0->name);
        utf8::Copy(castle8->m_name, sizeof(castle8->m_name), townName.c_str());

        memset(usedSpells0, 0, H2EnumIndex(SPELL_COUNT));
        for (spellLevel = 0; spellLevel < TOWN_MAGE_GUILD_LEVEL_COUNT; spellLevel++) {
            spellsPerLevel12[spellLevel] = 0;
            for (spellIndex3 = 0; spellIndex3 < TOWN_MAGE_GUILD_SPELLS_PER_LEVEL; spellIndex3++)
                castle8->m_spellSlots[spellLevel * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + spellIndex3] =
                    SPELL_NONE;
        }

        if (castle8->m_type == FACTION_CYBORG) {
            castle8->m_spells[0][0] = SPELL_SHADOW_MARK;
            castle8->m_spells[0][1] = SPELL_MARKSMAN_PIERCE;
            castle8->m_spells[1][0] = SPELL_PLASMA_CONE;
            castle8->m_spells[1][1] = SPELL_FORCE_SHIELD;
            castle8->m_spells[2][0] = SPELL_MASS_FORCE_SHIELD;
            castle8->m_spells[3][0] = SPELL_FIRE_BOMB;
            castle8->m_spells[4][0] = SPELL_IMPLOSION_GRENADE;
            goto cyborgSpellsSelected;
        }

        if (castle8->m_type == FACTION_NECROMANCER && castle8->m_owner != -1
            && !gbHumanPlayer[castle8->m_owner]) {
            if (Random(0, 100) < 50)
                spellIndex3 = H2EnumIndex(SPELL_DEATH_RIPPLE);
            else
                spellIndex3 = H2EnumIndex(SPELL_DEATH_WAVE);
            spellLevel = H2EnumIndex(gsSpellInfo[spellIndex3].level) - 1;
            castle8->m_spells[spellLevel][spellsPerLevel12[spellLevel]] = SpellType(spellIndex3);
            spellsPerLevel12[spellLevel]++;
        }

        roll8 = Random(0, 100);
        if (roll8 < 25)
            spellIndex3 = H2EnumIndex(SPELL_DISPEL);
        else if (roll8 < 50)
            spellIndex3 = H2EnumIndex(SPELL_MASS_DISPEL);
        else if (roll8 < 75)
            spellIndex3 = H2EnumIndex(SPELL_ANTI_MAGIC);
        else
            spellIndex3 = H2EnumIndex(SPELL_CURE);
        spellLevel = H2EnumIndex(gsSpellInfo[spellIndex3].level) - 1;
        castle8->m_spells[spellLevel][spellsPerLevel12[spellLevel]] = SpellType(spellIndex3);
        spellsPerLevel12[spellLevel]++;

        roll8 = Random(0, 100);
        if (roll8 < 20)
            spellIndex3 = H2EnumIndex(SPELL_MAGIC_ARROW);
        else if (roll8 < 40)
            spellIndex3 = H2EnumIndex(SPELL_LIGHTNING_BOLT);
        else if (roll8 < 60)
            spellIndex3 = H2EnumIndex(SPELL_FIREBALL);
        else if (roll8 < 80)
            spellIndex3 = H2EnumIndex(SPELL_COLD_RAY);
        else
            spellIndex3 = H2EnumIndex(SPELL_COLD_RING);
        spellLevel = H2EnumIndex(gsSpellInfo[spellIndex3].level) - 1;
        castle8->m_spells[spellLevel][spellsPerLevel12[spellLevel]] = SpellType(spellIndex3);
        spellsPerLevel12[spellLevel]++;

        for (spellLevel = 0; spellLevel < TOWN_MAGE_GUILD_LEVEL_COUNT; spellLevel++) {
            combatSpells27 = 0;
            for (spellIndex3 = 0; spellIndex3 < TOWN_MAGE_GUILD_SPELLS_PER_LEVEL; spellIndex3++) {
                if (castle8->m_spellSlots[spellLevel * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + spellIndex3]
                    != SPELL_NONE) {
                    usedSpells0[H2EnumIndex(castle8->m_spellSlots
                            [spellLevel * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + spellIndex3])] = 1;
                    continue;
                }
                {
                    attempts17 = 0;
                    do {
                        spell1 =
                            SpellType(Random(H2EnumIndex(SPELL_FIREBALL), H2EnumIndex(SPELL_SET_WATER_GUARDIAN)));
                        while (H2EnumIndex(gsSpellInfo[H2EnumIndex(spell1)].level) - 1 != spellLevel)
                            spell1 = SpellType(
                                Random(H2EnumIndex(SPELL_FIREBALL), H2EnumIndex(SPELL_SET_WATER_GUARDIAN))
                            );
                        if (castle8->m_owner != -1 && !gbHumanPlayer[castle8->m_owner])
                            spellValue =
                                gsSpellInfo[H2EnumIndex(spell1)].aiValue
                                    * ((H2EnumIndex((gsSpellInfo[H2EnumIndex(spell1)].attributes) & (SPELL_INFO_ATTRIBUTE_POWER)))
                                           ? 4
                                           : 1)
                                + 50;
                        else
                            spellValue = 1500;
                        if (spell1 == SPELL_DIMENSION_DOOR)
                            spellValue = 1500;
                    } while (
                        (combatSpells27 == 1
                         && (H2EnumIndex((gsSpellInfo[H2EnumIndex(spell1)].attributes) & (SPELL_INFO_ATTRIBUTE_ADVENTURE))))
                        || Random(0, 10)
                               > gsSpellInfo[H2EnumIndex(spell1)].raceChance[H2EnumIndex(castle8->m_type)]
                        || attempts17++ > 500 || usedSpells0[H2EnumIndex(spell1)]
                        || Random(1, 1500) > spellValue
                    );
                    if ((H2EnumIndex((gsSpellInfo[H2EnumIndex(spell1)].attributes) & (SPELL_INFO_ATTRIBUTE_ADVENTURE))))
                        combatSpells27++;
                    castle8
                        ->m_spellSlots[spellLevel * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + spellIndex3] =
                        spell1;
                    usedSpells0[H2EnumIndex(spell1)] = 1;
                }
            }
        }

    cyborgSpellsSelected:
        H2_FREE(ppMapExtra[extraIndex27]);
        ppMapExtra[extraIndex27] = NULL;
    }
}

void game::ProcessOnMapHeroes(void) {
    struct HeroData {
        FactionType faction;
        bool randomizable;
        bool exists;
    };

    std::vector<HeroData> heroesAvailable;
    for (i32 i = 0; i < IRONFIST_TOTAL_AVAILABLE_HEROES; i++) {
        HeroData data;
        data.exists = false;
        if (i < H2EnumIndex(GAME_HERO_COUNT)) {
            data.randomizable = true;
            data.faction = static_cast<FactionType>(i / MAP_HEROES_PER_FACTION);
        } else if (i < IRONFIST_FIRST_CYBORG_HERO_ID) {
            data.randomizable = false;
            data.faction = FACTION_KNIGHT;
        } else {
            data.randomizable = true;
            data.faction = FACTION_CYBORG;
        }
        heroesAvailable.push_back(data);
    }

    char heroExists[H2EnumIndex(GAME_HERO_COUNT)];
    memset(heroExists, 0, H2EnumIndex(GAME_HERO_COUNT));
    for (i32 y = 0; y < MAP_HEIGHT; ++y) {
        for (i32 x = 0; x < MAP_WIDTH; ++x) {
            mapCell* loc = m_worldMap.GetCell(x, y);
            i32 locationType = H2EnumIndex(loc->m_triggerType & H2EnumIndex(MAP_TRIGGER_TYPE_MASK));
            if (locationType == H2EnumIndex(MAP_OBJECT_HERO)
                || loc->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_JAIL)) {
                const bool isJail = locationType == H2EnumIndex(MAP_OBJECT_JAIL);
                const i32 extraIdx = loc->m_objectMetadata;
                mapHeroExtra* mapExtraHero =
                    reinterpret_cast<mapHeroExtra*>(ppMapExtra[extraIdx]);
                // handle non-default heroes
                if (!mapExtraHero->hasCustomHero
                    || mapExtraHero->heroId >= H2EnumIndex(GAME_HERO_COUNT)
                    || heroExists[mapExtraHero->heroId]) {
                    mapExtraHero->hasAssignedHero = 0;
                } else {
                    // handle default 54 heroes
                    heroExists[mapExtraHero->heroId] = true;
                    mapExtraHero->hasAssignedHero = 1;
                }

                FactionType faction;
                if (isJail) {
                    mapExtraHero->owner = -1;
                    faction = mapExtraHero->heroClass;
                } else {
                    mapExtraHero->owner =
                        gcColorToPlayerPos[GetHeroOverlayColor(loc->m_objectIndex)];
                    faction = GetHeroOverlayFaction(loc->m_objectIndex);
                    if (faction == FACTION_NEUTRAL) {
                        faction = m_setupPlayerRace
                            [gcColorToSetupPos[m_players[mapExtraHero->owner].m_color]];
                    }
                }

                i32 heroIndex;
                if (mapExtraHero->hasAssignedHero) {
                    heroIndex = mapExtraHero->heroId;
                    m_heroRecs[heroIndex].m_cursorType = static_cast<HeroCursorType>(faction);
                } else {
                    // find a free hero with the wanted faction
                    std::vector<i32> neededFactionHeroes;
                    for (i32 i = 0; i < static_cast<i32>(heroesAvailable.size()); i++) {
                        HeroData* hro = &heroesAvailable[i];
                        if (hro->randomizable && !hro->exists && hro->faction == faction)
                            neededFactionHeroes.push_back(i);
                    }

                    i32 randomHeroIdx;
                    if (neededFactionHeroes.size()) {
                        randomHeroIdx =
                            neededFactionHeroes[Random(0, neededFactionHeroes.size() - 1)];
                        if (!mapExtraHero->hasCustomHero) // keep an editor-placed portrait
                            mapExtraHero->heroId = static_cast<i8>(randomHeroIdx);
                    } else {
                        // none from this faction available: any free hero
                        std::vector<i32> allFreeHeroes;
                        for (i32 i = 0; i < static_cast<i32>(heroesAvailable.size()); i++) {
                            HeroData* hro = &heroesAvailable[i];
                            if (hro->randomizable && !hro->exists)
                                allFreeHeroes.push_back(i);
                        }
                        randomHeroIdx = allFreeHeroes[Random(0, allFreeHeroes.size() - 1)];
                        mapExtraHero->heroId = static_cast<i8>(randomHeroIdx);
                    }

                    // Campaign and Cyborg heroes live past the retail pool:
                    // take over a random free retail slot and keep the
                    // extended portrait.
                    i32 extendedHeroId = mapExtraHero->heroId;
                    if (randomHeroIdx >= H2EnumIndex(GAME_HERO_COUNT)) {
                        std::vector<i32> freeHeroes;
                        for (i32 i = 0; i < H2EnumIndex(GAME_HERO_COUNT); i++) {
                            if (!heroExists[i])
                                freeHeroes.push_back(i);
                        }

                        randomHeroIdx = freeHeroes[Random(0, freeHeroes.size() - 1)];
                        mapExtraHero->hasCustomHero = true;
                    }

                    m_heroRecs[randomHeroIdx].m_cursorType = static_cast<HeroCursorType>(faction);
                    if (mapExtraHero->hasCustomHero
                        && extendedHeroId >= H2EnumIndex(GAME_HERO_COUNT)) {
                        m_heroRecs[randomHeroIdx].m_portrait =
                            static_cast<HeroPortrait>(extendedHeroId);
                    }
                    if (extendedHeroId < static_cast<i32>(heroNames.size())
                        && !heroNames[extendedHeroId].empty()) {
                        utf8::Copy(
                            m_heroRecs[randomHeroIdx].m_name,
                            sizeof(m_heroRecs[randomHeroIdx].m_name),
                            heroNames[extendedHeroId].c_str()
                        );
                    }
                    if (extendedHeroId < static_cast<i32>(heroesAvailable.size()))
                        heroesAvailable[extendedHeroId].exists = true;
                    mapExtraHero->heroId = static_cast<i8>(randomHeroIdx);
                    heroIndex = randomHeroIdx;
                }

                heroExists[heroIndex] = true;
                heroesAvailable[heroIndex].exists = true;

                hero* randomHero = &m_heroRecs[heroIndex];

                if (!isJail && mapExtraHero->hasPatrol) {
                    randomHero->m_patrolX = static_cast<i8>(x);
                    randomHero->m_patrolY = static_cast<i8>(y);
                    randomHero->m_patrolRadius = mapExtraHero->patrolRadius;
                }

                if (mapExtraHero->hasCustomArmy) {
                    for (i32 slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
                        randomHero->m_army.m_quantities[slot] =
                            static_cast<i16>(mapExtraHero->troopCounts[slot]);
                        if (randomHero->m_army.m_quantities[slot] <= 0) {
                            randomHero->m_army.m_creatureTypes[slot] = CREATURE_NONE;
                        } else {
                            randomHero->m_army.m_creatureTypes[slot] =
                                mapExtraHero->troopTypes[slot];
                        }
                    }
                } else {
                    SetRandomHeroArmies(heroIndex, 0);
                }

                for (i32 i = 0; i < EVENT_RECORD_HERO_ARTIFACT_COUNT; ++i) {
                    if (mapExtraHero->artifacts[i] >= 0) {
                        GiveArtifact(
                            randomHero, static_cast<ArtifactType>(mapExtraHero->artifacts[i]),
                            1, -1
                        );
                    }
                }
                if (mapExtraHero->hasCustomName) {
                    const std::string heroName =
                        localization::DecodeExternalText(mapExtraHero->name);
                    utf8::Copy(randomHero->m_name, sizeof(randomHero->m_name), heroName.c_str());
                }
                randomHero->m_experience = 0;
                gpAdvManager->GiveExperience(randomHero, mapExtraHero->experience, 1);
                randomHero->CheckLevel();
                randomHero->m_x = x;
                randomHero->m_y = y;

                if (isJail) {
                    randomHero->m_owner = -1;
                    m_availableHeroes[heroIndex] = HERO_AVAILABILITY_JAILED;
                    loc->m_objectMetadata = heroIndex;
                } else {
                    randomHero->m_owner = mapExtraHero->owner;
                    m_availableHeroes[heroIndex] = randomHero->m_owner;
                    m_players[randomHero->m_owner]
                        .m_heroIds[m_players[randomHero->m_owner].m_heroCount++] =
                        randomHero->m_id;
                    if (y > 0
                        && m_worldMap.GetCell(x, y - 1)->m_triggerType
                               == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)) {
                        --randomHero->m_patrolY;
                        --randomHero->m_y;
                        m_castleRecs[GetTownId(x, y - 1)].m_occupyingHeroId = randomHero->m_id;
                    }
                    loc->m_objectTileset = TILESET_NONE;
                    loc->m_objectIndex = static_cast<u8>(-1);
                    loc->m_objectMetadata = 0;
                    loc->m_triggerType = 0;
                }

                if (mapExtraHero->hasCustomSkills) {
                    for (i32 i = 0; i < H2EnumIndex(HERO_SKILL_COUNT); ++i) {
                        randomHero->m_secondarySkills[i] = HERO_SKILL_LEVEL_NONE;
                        randomHero->m_secondarySkillOrder[i] = 0;
                    }
                    randomHero->m_secondarySkillCount = 0;
                    for (i32 i = 0; i < EVENT_RECORD_SKILL_CAPACITY; ++i) {
                        if (mapExtraHero->skillTypes[i] != -1)
                            randomHero->GiveSS(
                                static_cast<HeroSecondarySkill>(mapExtraHero->skillTypes[i]),
                                static_cast<HeroSkillLevel>(mapExtraHero->skillLevels[i])
                            );
                    }
                } else if (faction == FACTION_CYBORG) {
                    // every Cyborg gets Cybernetics (Wisdom) and a magic book
                    if (!randomHero->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
                        GiveArtifact(randomHero, ARTIFACT_MAGIC_BOOK, 1, -1);
                    }
                    for (i32 i = 0; i < H2EnumIndex(HERO_SKILL_COUNT); ++i) {
                        randomHero->m_secondarySkills[i] = HERO_SKILL_LEVEL_NONE;
                        randomHero->m_secondarySkillOrder[i] = 0;
                    }
                    randomHero->m_secondarySkillCount = 0;
                    randomHero->GiveSS(HERO_SKILL_WISDOM, HERO_SKILL_LEVEL_ADVANCED);
                }

                if (!isJail) {
                    SetVisibility(
                        randomHero->m_x, randomHero->m_y, randomHero->m_owner,
                        giVisRange[H2EnumIndex(
                            randomHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_SCOUTING)]
                        )]
                    );
                }

                H2_FREE(ppMapExtra[extraIdx]);
                ppMapExtra[extraIdx] = NULL;
            }
        }
    }
}

void game::CheckHeroConsistency(void) {
    i32 all = 0;
    i32 y8;
    i32 c;
    hero* boardHro;
    i32 slot;
    i32 player;
    mapCell* cell;
    town* townOccupied;
    i32 sane;

    for (player = 0; player < m_playerCount; player++) {
        if (m_playerDead[player] != 0)
            continue;
        all += m_players[player].m_heroCount;
        for (slot = 0; slot < m_players[player].m_heroCount; slot++) {
            if (m_heroRecs[m_players[player].m_heroIds[slot]].m_owner != player)
                sane = 0;
        }
    }

    for (player = 0; player < m_playerCount; player++) {
        if (m_playerDead[player] == 0) {
            for (slot = 0; slot < AVAILABLE_HERO_SLOTS; slot++) {
                if ((m_availableHeroes[m_players[player].m_availableHeroIds[slot]] >= 0
                     && m_availableHeroes[m_players[player].m_availableHeroIds[slot]]
                            <= HERO_CONSISTENCY_PLAYABLE_FACTION_MAX)
                    || (all < HERO_CONSISTENCY_POOL_THRESHOLD
                        && m_availableHeroes[m_players[player].m_availableHeroIds[slot]] == -1)) {
                    m_players[player].m_availableHeroIds[slot] =
                        static_cast<i8>(GetNewHeroId(player, FACTION_ANY, 0));
                    m_availableHeroes[m_players[player].m_availableHeroIds[slot]] =
                        WEEKLY_AVAILABLE_HERO;
                }
            }
        }
    }

    for (c = 0; c < MAP_WIDTH; c++) {
        for (y8 = 0; y8 < MAP_HEIGHT; y8++) {
            cell = gpAdvManager->GetCell(c, y8);
            if (cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MERMAID)) {
                if (cell->m_objectMetadata >= 0 && cell->m_objectMetadata < GAME_HERO_COUNT) {
                    boardHro = GetHero(cell->m_objectMetadata);
                    if (boardHro->m_x != c || boardHro->m_y != y8) {
                        cell->m_triggerType = 0;
                        cell->m_objectMetadata = 0;
                    }
                    if (boardHro->m_owner < 0 || boardHro->m_owner >= GAME_PLAYER_COUNT) {
                        if (boardHro->m_locationType
                            == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)) {
                            townOccupied = gpGame->GetTown(boardHro->m_occupiedTown);
                            townOccupied->m_occupyingHeroId = -1;
                        }
                        if (boardHro->m_x == c && boardHro->m_y == y8) {
                            RestoreCell(
                                boardHro->m_x,
                                boardHro->m_y,
                                boardHro->m_locationType,
                                boardHro->m_occupiedTown,
                                NULL,
                                1
                            );
                        } else {
                            cell->m_triggerType = 0;
                            cell->m_objectMetadata = 0;
                        }
                    }
                } else {
                    cell->m_triggerType = 0;
                }
            }
        }
    }

    for (player = 0; player < GAME_HERO_COUNT; player++) {
        for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; slot++) {
            if (m_heroRecs[player].m_army.m_troopTypes[slot] == CREATURE_NONE
                || m_heroRecs[player].m_army.m_creatureCounts[slot] < 0)
                m_heroRecs[player].m_army.m_creatureCounts[slot] = 0;
        }
    }
    for (player = 0; player < GAME_TOWN_COUNT; player++) {
        for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; slot++) {
            if (m_castleRecs[player].m_army.m_troopTypes[slot] == CREATURE_NONE
                || m_castleRecs[player].m_army.m_creatureCounts[slot] < 0)
                m_castleRecs[player].m_army.m_creatureCounts[slot] = 0;
        }
    }
}

#define done done9
#define fileData fileData5
#define fileSize fileSize15
#define result result9
#define fileCrc fileCrc18
#define acknowledged acknowledged7
#define reply reply36
#define transmitCrc transmitCrc9
#define packetsInBatch packetsInBatch7
#define header header2
#define batch batch9
#define file file7
#define oldTrack oldTrack6
#define packetCount packetCount4
#define packet packet0
#define filename filename18
#define samplesReady samplesReady3
#define success success7

i32 game::TransmitSaveGame(i32 remotePlayer, i32 player, i32 useCurrentSave) {
    char filename[TRANSMIT_FILENAME_CAPACITY];
    u32 transmitCrc;
    i32 packetsInBatch;
    i32 unused1d0;
    u32 fileCrc;
    i32 oldTrack;
    i32 batchCount;
    i32 chunkSize;
    i32 result;
    i32 packetCount;
    char* reply;
    i32 success;
    i32 fileSize;
    i32 packet;
    u8* transmitData;
    i32 file;
    i32 unused208;
    i32 done;
    i32 unused21c;
    bool samplesReady;
    u8* fileData;
    char* acknowledged;
    i32* header;
    i32 unused8;
    i32 batch;
    i32 unused9;

    gpAdvManager->TrimLoopingSounds(REMOTE_LOOPING_SOUND_COUNT);
    header = NULL;
    reply = NULL;
    transmitData = NULL;
    fileData = NULL;
    success = 0;
    result = 0;
    acknowledged = NULL;
    oldTrack = -1;

    samplesReady = gSoundBackendsReady;
    gSoundBackendsReady = 1;
    oldTrack = gpSoundManager->m_musicTrack;
    gpSoundManager->SwitchAmbientMusic(-1);
    gSoundBackendsReady = samplesReady;

    LogStr(const_cast<char*>("Transmit Game Start"));
    if (gpAdvManager->m_active == 1)
        BVResMsg(const_cast<char*>(localization::Tr("network.data.sending")), RES_NONE, 0);
    AiPrint(const_cast<char*>("Transmit Start - Compressing"));

    acknowledged = static_cast<char*>(H2_ALLOC(REMOTE_PACKET_TRACKING_CAPACITY));
    memset(acknowledged, 0, REMOTE_PACKET_TRACKING_CAPACITY);
    SaveGame(gConfig.rmtSCName, 0, 0);
    if (!gbUseDiffCompression)
        useCurrentSave = 1;
    CreateDiffFile(
        gConfig.rmtSLName,
        gConfig.rmtSCName,
        gConfig.rmtSDName,
        remotePlayer,
        useCurrentSave
    );
    sprintf(filename, "%s%s", ".\\DATA\\", gConfig.rmtSDName);
    fileSize = FileSize(filename);
    LogInt(
        const_cast<char*>("PostDiffFileSize"),
        fileSize,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );

    header = static_cast<i32*>(H2_ALLOC(REMOTE_HEADER_CAPACITY));
    if (gbUseRegularCompression)
        transmitData = static_cast<u8*>(H2_ALLOC(fileSize + REMOTE_BUFFER_EXTRA));
    fileData = static_cast<u8*>(H2_ALLOC(fileSize + REMOTE_BUFFER_EXTRA));

    file = platform::FileOpen(filename, platform::FileMode::Read);
    if (file == -1)
        FileError(filename);
    if (file == -1) {
        goto transmitCleanup;
    }
    {
        platform::FileRead(file, fileData, fileSize);
        platform::FileClose(file);
        fileCrc = calc_crc_long(fileData, fileSize);
        if (gbUseRegularCompression)
            fileSize = EncodeData(
                reinterpret_cast<char*>(transmitData),
                reinterpret_cast<char*>(fileData),
                fileSize
            );
        else
            transmitData = fileData;

        AiPrint(const_cast<char*>("Transmit Start - Sending"));
        if (gbUseRegularCompression)
            transmitCrc = calc_crc_long(transmitData, fileSize);
        else
            transmitCrc = fileCrc;
        LogInt(
            const_cast<char*>("Send"),
            fileSize,
            transmitCrc,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE
        );

        header[REMOTE_SAVE_HEADER_FILE_SIZE] = fileSize;
        header[REMOTE_SAVE_HEADER_FILE_CRC] = fileCrc;
        header[REMOTE_SAVE_HEADER_TRANSMIT_CRC] = transmitCrc;
        header[REMOTE_SAVE_HEADER_PLAYER] = player;
        result = TransmitAndWait(
            reinterpret_cast<char*>(header),
            remotePlayer,
            REMOTE_SAVE_HEADER_SIZE,
            REMOTE_SAVE_INIT_COMMAND,
            REMOTE_SAVE_INIT_RESPONSE,
            &reply
        );
        if (!result)
            ShutDown(NULL);

        packetCount = (fileSize - 1) / REMOTE_PACKET_PAYLOAD_SIZE + 1;
        batchCount = (packetCount - 1) / REMOTE_PACKET_BATCH_SIZE + 1;
        for (batch = 0; batch < batchCount; batch++) {
            if (batch + 1 == batchCount)
                packetsInBatch = packetCount - batch * REMOTE_PACKET_BATCH_SIZE;
            else
                packetsInBatch = REMOTE_PACKET_BATCH_SIZE;

            done = 0;
            while (!done) {
                for (packet = batch * REMOTE_PACKET_BATCH_SIZE;
                     packet < batch * REMOTE_PACKET_BATCH_SIZE + packetsInBatch;
                     packet++) {
                    PollSound();
                    CheckDoMain(0, 1);
                    if (!acknowledged[packet]) {
                        if (packet + 1 == packetCount)
                            chunkSize = fileSize - packet * REMOTE_PACKET_PAYLOAD_SIZE;
                        else
                            chunkSize = REMOTE_PACKET_PAYLOAD_SIZE;
                        *reinterpret_cast<i16*>(header) = static_cast<i16>(packet);
                        memcpy(
                            reinterpret_cast<char*>(header) + REMOTE_PACKET_INDEX_SIZE,
                            transmitData + packet * REMOTE_PACKET_PAYLOAD_SIZE,
                            chunkSize
                        );
                        result = TransmitRemoteData(
                            reinterpret_cast<char*>(header),
                            remotePlayer,
                            chunkSize + REMOTE_PACKET_INDEX_SIZE,
                            REMOTE_SAVE_DATA_COMMAND,
                            0,
                            1,
                            REMOTE_MESSAGE_DEFAULT
                        );
                        if (!result)
                            ShutDown(NULL);
                    }
                }
                LogStr(const_cast<char*>("PreWait"));
                *reinterpret_cast<i16*>(header) =
                    static_cast<i16>(batch * REMOTE_PACKET_BATCH_SIZE);
                result = TransmitAndWait(
                    reinterpret_cast<char*>(header),
                    remotePlayer,
                    REMOTE_PACKET_INDEX_SIZE,
                    REMOTE_SAVE_ACK_REQUEST_COMMAND,
                    REMOTE_SAVE_ACK_RESPONSE_COMMAND,
                    &reply
                );
                LogStr(const_cast<char*>("PostWait"));
                if (!result)
                    ShutDown(NULL);
                for (packet = 0; packet < packetsInBatch; packet++) {
                    if (reinterpret_cast<RemoteMessage*>(reply)->payload[packet] > 0)
                        *(acknowledged + packet + batch * REMOTE_PACKET_BATCH_SIZE) = 1;
                }
                done = 1;
                for (packet = batch * REMOTE_PACKET_BATCH_SIZE;
                     packet < batch * REMOTE_PACKET_BATCH_SIZE + packetsInBatch;
                     packet++) {
                    if (!acknowledged[packet])
                        done = 0;
                }
            }
        }
        result = TransmitRemoteData(
            NULL,
            remotePlayer,
            0,
            REMOTE_SAVE_FINISH_COMMAND,
            1,
            1,
            REMOTE_MESSAGE_DEFAULT
        );
        if (!result)
            ShutDown(NULL);
        success = 1;
    }

transmitCleanup:
    if (header)
        H2_FREE(header);
    if (transmitData)
        H2_FREE(transmitData);
    if (fileData && fileData != transmitData)
        H2_FREE(fileData);
    if (acknowledged)
        H2_FREE(acknowledged);

    AiPrint(const_cast<char*>("Transmit End"));
    if (gpAdvManager->m_active == 1) {
        giBottomViewOverride = BOTTOM_VIEW_NONE;
        gpAdvManager->UpdBottomView(1, 1, 1);
    }
    if (oldTrack != -1) {
        samplesReady = gSoundBackendsReady;
        gSoundBackendsReady = 1;
        gpSoundManager->SwitchAmbientMusic(oldTrack);
        gSoundBackendsReady = samplesReady;
    }
    return success;
}

#undef done
#undef fileData
#undef fileSize
#undef result
#undef fileCrc
#undef acknowledged
#undef reply
#undef transmitCrc
#undef packetsInBatch
#undef header
#undef batch
#undef file
#undef oldTrack
#undef packetCount
#undef packet
#undef filename
#undef samplesReady
#undef success

#define packetStart packetStart0
#define lastPacketTime lastPacketTime9
#define result result9
#define packet packet15
#define ackBuffer ackBuffer6
#define finished finished4
#define computedCrc computedCrc2
#define oldTrack oldTrack6
#define index index27
#define incomingData incomingData7
#define file file6
#define filename filename18
#define samplesReady samplesReady0
#define success success7

i32 game::ReceiveSaveGame(
    i32 dataSize,
    i32 expectedCrc,
    i32 expectedTransmitCrc,
    i32 remotePlayer
) {
    char filename[RECEIVE_FILENAME_CAPACITY];
    i32 receivedCrc;
    i32 finished;
    i32 oldTrack;
    i32 result;
    char* received;
    RemoteMessage* packet;
    i32 computedCrc;
    i32 success;
    i32 index;
    u8* incomingData;
    i32 file;
    i32 packetStart;
    u8* ackBuffer;
    u8* decodedData;
    bool samplesReady;
    i32 unused2080;
    i32l lastPacketTime;

    LogInt(
        const_cast<char*>("FW1"),
        remotePlayer,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );
    LogStr(const_cast<char*>("RSG1"));
    AiPrint(const_cast<char*>("Receive Start - Getting Data"));
    gpAdvManager->TrimLoopingSounds(REMOTE_LOOPING_SOUND_COUNT);

    ackBuffer = NULL;
    incomingData = NULL;
    decodedData = NULL;
    packet = NULL;
    file = 0;
    finished = 0;
    unused2080 = 0;
    received = NULL;
    success = 0;
    oldTrack = -1;

    gpAdvManager->UnwindMapChangeQueue(REMOTE_MAP_CHANGE_UNWIND_LIMIT, 0);
    if (gpAdvManager->m_active == 1)
        BVResMsg(const_cast<char*>(localization::Tr("network.data.receiving")), RES_NONE, 0);

    samplesReady = gSoundBackendsReady;
    oldTrack = gpSoundManager->m_musicTrack;
    gSoundBackendsReady = 1;
    gpSoundManager->SwitchAmbientMusic(-1);
    gSoundBackendsReady = samplesReady;

    LogStr(const_cast<char*>("Begin Transmit Init Confirm"));
    result = TransmitRemoteData(
        NULL,
        remotePlayer,
        0,
        REMOTE_SAVE_INIT_RESPONSE,
        1,
        1,
        REMOTE_MESSAGE_DEFAULT
    );
    LogStr(const_cast<char*>("End Transmit Init Confirm"));
    if (!result)
        ShutDown(NULL);

    received = static_cast<char*>(H2_ALLOC(REMOTE_PACKET_TRACKING_CAPACITY));
    memset(received, 0, REMOTE_PACKET_TRACKING_CAPACITY);
    if (gbUseRegularCompression)
        decodedData = static_cast<u8*>(H2_ALLOC(REMOTE_DECODE_BUFFER_SIZE));
    ackBuffer = static_cast<u8*>(H2_ALLOC(REMOTE_HEADER_CAPACITY));
    incomingData = static_cast<u8*>(H2_ALLOC(dataSize + REMOTE_BUFFER_EXTRA));

    lastPacketTime = platform::Ticks();
    LogInt(
        const_cast<char*>("FW2"),
        remotePlayer,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );
    while (!finished) {
        PollSound();
        CheckDoMain(0, 1);
        if (lastPacketTime + REMOTE_RECEIVE_TIMEOUT < platform::Ticks()) {
            NormalDialog(
                const_cast<char*>(localization::Tr("network.receive.retry")),
                REMOTE_RECEIVE_DIALOG_BUTTONS,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE)
                lastPacketTime = platform::Ticks();
            else
                ShutDown(NULL);
        }

        packet = reinterpret_cast<RemoteMessage*>(GetRemoteData(1));
        if (packet
            && (packet->type == REMOTE_MESSAGE_RELIABLE
                || packet->type == REMOTE_MESSAGE_UNRELIABLE)) {
            lastPacketTime = platform::Ticks();
            switch (packet->command) {
                case REMOTE_SAVE_DATA_COMMAND:
                    packetStart = *reinterpret_cast<i16*>(packet->payload);
                    received[packetStart] = 1;
                    memcpy(
                        incomingData + packetStart * REMOTE_PACKET_PAYLOAD_SIZE,
                        packet->payload + REMOTE_PACKET_INDEX_SIZE,
                        packet->payloadSize - REMOTE_PACKET_INDEX_SIZE
                    );
                    break;
                case REMOTE_SAVE_ACK_REQUEST_COMMAND:
                    packetStart = *reinterpret_cast<i16*>(packet->payload);
                    for (index = packetStart; index < packetStart + REMOTE_PACKET_BATCH_SIZE;
                         index++)
                        *(ackBuffer + index - packetStart) = received[index];
                    LogInt(
                        const_cast<char*>("FW3"),
                        remotePlayer,
                        LOG_UNUSED_VALUE,
                        LOG_UNUSED_VALUE,
                        LOG_UNUSED_VALUE,
                        LOG_UNUSED_VALUE,
                        LOG_UNUSED_VALUE,
                        LOG_UNUSED_VALUE
                    );
                    result = TransmitRemoteData(
                        reinterpret_cast<char*>(ackBuffer),
                        remotePlayer,
                        REMOTE_PACKET_PAYLOAD_SIZE,
                        REMOTE_SAVE_ACK_RESPONSE_COMMAND,
                        1,
                        1,
                        REMOTE_MESSAGE_DEFAULT
                    );
                    if (!result)
                        ShutDown(NULL);
                    break;
                case REMOTE_SAVE_FINISH_COMMAND:
                    finished = 1;
                    break;
            }
        }
    }

    AiPrint(const_cast<char*>("Receive Start - Decompressing Data"));
    receivedCrc = calc_crc_long(incomingData, dataSize);
    LogInt(
        const_cast<char*>("Receive"),
        dataSize,
        receivedCrc,
        expectedTransmitCrc,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );
    if (gbUseRegularCompression) {
        dataSize = DecodeData(
            reinterpret_cast<char*>(decodedData),
            reinterpret_cast<char*>(incomingData),
            dataSize
        );
        computedCrc = calc_crc_long(decodedData, dataSize);
    } else {
        decodedData = incomingData;
        computedCrc = receivedCrc;
    }
    LogInt(
        const_cast<char*>("Receive"),
        dataSize,
        computedCrc,
        expectedCrc,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );

    sprintf(filename, "%s%s", ".\\DATA\\", gConfig.rmtRDName);
    file = platform::FileOpen(filename, platform::FileMode::Write);
    if (file == -1)
        FileError(filename);
    platform::FileWrite(file, decodedData, dataSize);
    platform::FileClose(file);
    success = 1;

    if (received)
        H2_FREE(received);
    if (ackBuffer)
        H2_FREE(ackBuffer);
    if (incomingData)
        H2_FREE(incomingData);
    if (decodedData && decodedData != incomingData)
        H2_FREE(decodedData);

    CreateJoinFile(gConfig.rmtRLName, gConfig.rmtRDName, gConfig.rmtRCName);
    AiPrint(const_cast<char*>("Receive End"));
    if (gpAdvManager->m_active == 1) {
        giBottomViewOverride = BOTTOM_VIEW_NONE;
        gpAdvManager->UpdBottomView(1, 1, 1);
    }
    if (oldTrack != -1) {
        samplesReady = gSoundBackendsReady;
        gSoundBackendsReady = 1;
        gpSoundManager->SwitchAmbientMusic(oldTrack);
        gSoundBackendsReady = samplesReady;
    }
    return success;
}

#undef packetStart
#undef lastPacketTime
#undef result
#undef packet
#undef ackBuffer
#undef finished
#undef computedCrc
#undef oldTrack
#undef index
#undef incomingData
#undef file
#undef filename
#undef samplesReady
#undef success

void game::DoNewTurn(void) {
    char musicFile18[NEW_TURN_MUSIC_FILENAME_CAPACITY];
    char lowerName19[NEW_TURN_LOWER_NAME_CAPACITY];
    i32 musicTrack2;

    CheckForTimeEvent();
    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        CheckEndGame(END_GAME_FORCE_NONE, false);
        return;
    }
    giBottomViewOverrideEndTime = platform::Ticks() + NEW_TURN_BOTTOM_VIEW_DURATION;
    giBottomViewOverride = BOTTOM_VIEW_NEW_TURN;
    gpAdvManager->UpdBottomView(1, 1, 1);
    gpAdvManager->SetInitialMapOrigin();
    gpAdvManager->CompleteDraw(0);
    gpAdvManager->UpdateScreen(0, 0);
    CheckEndGame(END_GAME_FORCE_NONE, false);

    if (gpCurPlayer->m_daysLeft >= 0) {
        if (gpCurPlayer->m_daysLeft == 1) {
            sprintf(gText, cNewTurn[1], cPlayerNames[giCurPlayer]);
        } else {
            sprintf(gText, cNewTurn[0], cPlayerNames[giCurPlayer], gpCurPlayer->m_daysLeft);
        }
        NormalDialog(
            gText,
            1,
            -1,
            -1,
            NEW_TURN_DIALOG_TYPE,
            gpGame->GetPlayerColor(static_cast<i8>(giCurPlayer)),
            -1,
            0,
            -1,
            0
        );
    }

    if (gpCurPlayer->m_heroCount > 0) {
        gpAdvManager->SetHeroContext(gpCurPlayer->NextHero(0), 0);
    } else if (gpCurPlayer->m_townCount > 0) {
        gpAdvManager->SetTownContext(gpCurPlayer->m_townIds[0]);
    }
    gpAdvManager->CheckDimNextHeroBut();

    if (m_day == 1 && (m_month != 1 || m_week != 1 || m_day != 1)) {
        if (gbThisNetHumanPlayer[giCurPlayer])
            gSoundBackendsReady = 1;
        if (giWeekType != CALENDAR_PERIOD_NONE) {
            musicTrack2 = -1;
            if (m_week == 1) {
                musicTrack2 = NEW_MONTH_MUSIC_TRACK;
                strcpy(musicFile18, "newmonth.82m");
                if (giMonthType == CALENDAR_PERIOD_NORMAL) {
                    sprintf(
                        gText,
                        cNewTurn[NEW_MONTH_NORMAL_TEXT],
                        gMonthNames[giMonthTypeExtra]
                    );
                } else if (giMonthType == CALENDAR_PERIOD_CREATURE) {
                    utf8::Copy(
                        lowerName19,
                        sizeof(lowerName19),
                        gArmyNamesPlural[giMonthTypeExtra]
                    );
                    utf8::LowercaseFirst(lowerName19);
                    sprintf(
                        gText,
                        cNewTurn[NEW_MONTH_CREATURE_TEXT],
                        gArmyNamesPlural[giMonthTypeExtra],
                        lowerName19
                    );
                } else {
                    sprintf(gText, cNewTurn[NEW_MONTH_PLAGUE_TEXT]);
                }
            } else {
                musicTrack2 = NEW_WEEK_MUSIC_TRACK;
                strcpy(musicFile18, "newweek.82m");
                if (giWeekType == CALENDAR_PERIOD_NORMAL) {
                    sprintf(gText, cNewTurn[NEW_WEEK_NORMAL_TEXT], gWeekNames[giWeekTypeExtra]);
                } else {
                    utf8::Copy(
                        lowerName19,
                        sizeof(lowerName19),
                        gArmyNamesPlural[giWeekTypeExtra]
                    );
                    utf8::LowercaseFirst(lowerName19);
                    sprintf(
                        gText,
                        cNewTurn[NEW_WEEK_CREATURE_TEXT],
                        gArmyNamesPlural[giWeekTypeExtra],
                        lowerName19
                    );
                }
            }
            gpSoundManager->PlayAmbientMusic(musicTrack2);
            gpMouseManager->SetPointer(0);
            NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            gpSoundManager->SwitchAmbientMusic(
                giTerrainToMusicTrack[H2EnumIndex(gpAdvManager->m_currentTerrain)]
            );
        }
    }
    gpSoundManager->SwitchAmbientMusic(
        giTerrainToMusicTrack[H2EnumIndex(gpAdvManager->m_currentTerrain)]
    );
    gpAdvManager->SetEnvironmentOrigin(
        gpAdvManager->m_mapOriginX + ENVIRONMENT_ORIGIN_TILE_OFFSET,
        gpAdvManager->m_mapOriginY + ENVIRONMENT_ORIGIN_TILE_OFFSET,
        1
    );
}

i32 game::GetBoatsBuilt(void) {
    i32 count = 0;
    i32 i;
    for (i = 0; i < GAME_BOAT_COUNT; i++) {
        if (m_boatSlots[i] != -1)
            count++;
    }
    return count;
}

i32 game::GetNumThievesGuilds(i32 color) {
    i32 num = 0;
    i32 i;
    for (i = 0; i < m_players[color].m_townCount; i++) {
        if (gpGame->m_castleRecs[m_players[color].m_townIds[i]].m_buildings
            & H2EnumIndex(TOWN_BUILDING_THIEVES_GUILD))
            num++;
    }
    return num;
}

i32 game::CalcDifficultyRating(void) {
    i32 notused;
    i32 rating = 0;
    if (m_difficulty == DIFFICULTY_EASY)
        rating += RATING_EASY_BONUS;
    else if (m_difficulty == DIFFICULTY_NORMAL)
        rating += RATING_NORMAL_BONUS;
    else if (m_difficulty == DIFFICULTY_HARD)
        rating += RATING_HARD_BONUS;
    else if (m_difficulty == DIFFICULTY_EXPERT)
        rating += RATING_EXPERT_BONUS;
    else if (m_difficulty == DIFFICULTY_IMPOSSIBLE)
        rating += RATING_IMPOSSIBLE_BONUS;
    if (m_mapHeader.difficulty == DIFFICULTY_EASY)
        rating += MAP_RATING_EASY_BONUS;
    else if (m_mapHeader.difficulty == DIFFICULTY_NORMAL)
        rating += MAP_RATING_NORMAL_BONUS;
    else if (m_mapHeader.difficulty == DIFFICULTY_HARD)
        rating += MAP_RATING_HARD_BONUS;
    else if (m_mapHeader.difficulty == DIFFICULTY_EXPERT)
        rating += MAP_RATING_EXPERT_BONUS;
    return rating;
}

i32 CalcBaseScore(i32 days) {
    i32 score = SCORE_BASE;

    if (gpGame->m_mapHeader.width == MAP_DIMENSION_XLARGE)
        days = static_cast<i32>(days * GAME_SCORE_EXTRA_LARGE_DAY_SCALE);
    else if (gpGame->m_mapHeader.width == MAP_DIMENSION_LARGE)
        days = static_cast<i32>(days * GAME_SCORE_LARGE_DAY_SCALE);
    else if (gpGame->m_mapHeader.width == MAP_DIMENSION_MEDIUM)
        days = static_cast<i32>(days * GAME_SCORE_MEDIUM_DAY_SCALE);
    else if (gpGame->m_mapHeader.width == MAP_DIMENSION_SMALL)
        days = static_cast<i32>(days * GAME_SCORE_SMALL_DAY_SCALE);

    if (days <= SCORE_FIRST_TIER) {
        score -= days;
        goto clampScore;
    } else
        score -= SCORE_FIRST_TIER;
    if (days <= SCORE_SECOND_TIER) {
        score = static_cast<i32>(
            score - (days - SCORE_FIRST_TIER) * SCORE_SECOND_TIER_FACTOR
        );
        goto clampScore;
    } else
        score = static_cast<i32>(score - SCORE_SECOND_TIER_BASE_DEDUCTION);
    if (days <= SCORE_THIRD_TIER) {
        score = static_cast<i32>(
            score - (days - SCORE_SECOND_TIER) * SCORE_THIRD_TIER_FACTOR
        );
        goto clampScore;
    } else
        score = static_cast<i32>(score - SCORE_THIRD_TIER_BASE_DEDUCTION);
    score = static_cast<i32>(
        score - (days - SCORE_THIRD_TIER) * SCORE_FINAL_TIER_FACTOR
    );

clampScore:
    if (score < SCORE_MINIMUM)
        score = SCORE_MINIMUM;
    return score;
}

void game::RestoreCell(
    i32 x,
    i32 y,
    MapObjectType objectType,
    i32 barrier,
    mapCell* passedCell,
    i32 p6
) {
    mapCell* cell;
    if (passedCell)
        cell = passedCell;
    else
        cell = gpAdvManager->GetCell(x, y);
    if (y > 0 && objectType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)
        && gpAdvManager->GetCell(x, y - 1)->m_triggerType != MAP_OBJECT_CASTLE) {
        cell->m_triggerType = 0;
        cell->m_objectMetadata = 0;
        return;
    }
    cell->m_triggerType = objectType;
    cell->m_objectMetadata = barrier;
}

void game::SetMapSize(i32 w, i32 h) {
    if (MAP_HEIGHT == h && MAP_WIDTH == w && bMapInitialized)
        goto mapSized;
    {
        bMapInitialized = 1;
        MAP_WIDTH = w;
        MAP_HEIGHT = h;
        gpSearchArray->Init();
    }
mapSized:
    if (mapExtra)
        H2_FREE(mapExtra);
    mapExtra = static_cast<u8*>(H2_ALLOC(MAP_HEIGHT * MAP_WIDTH));
    memset(mapExtra, 0, MAP_HEIGHT * MAP_WIDTH);
}

void WriteDiffHeaderInfo(u8 cmd, i32 len, u8* buf, i32* pos) {
    u8 flags = 0;
    flags |= cmd << COMMAND_SHIFT;
    if (len > LEN_WORD_MAX) {
        flags |= LEN_WORD_FLAG;
        flags |= (len & LEN_HIGH_MASK) >> DIFF_WORD_SHIFT;
        u16 word = static_cast<u16>(len & LEN_LOW_MASK);
        buf[*pos] = flags;
        *reinterpret_cast<u16*>(buf + *pos + 1) = word;
        *pos += DIFF_WORD_HEADER_SIZE;
    } else if (len > LEN_BYTE_MAX) {
        flags |= LEN_BYTE_FLAG;
        flags |= (len & LEN_MID_MASK) >> DIFF_BYTE_SHIFT;
        u8 lo = len & LEN_BYTE_MASK;
        buf[*pos] = flags;
        buf[*pos + 1] = lo;
        *pos += DIFF_BYTE_HEADER_SIZE;
    } else {
        flags |= static_cast<u8>(len);
        buf[*pos] = flags;
        (*pos)++;
    }
}

i32 GetSkipCopyLen(u8* buf, i32* pos) {
    u8 b = buf[*pos];
    i32 len;
    if (b & LEN_WORD_FLAG) {
        len = b & LEN_WORD_HIGH_MASK;
        len <<= DIFF_WORD_SHIFT;
        len |= *reinterpret_cast<u16*>(buf + *pos + 1);
        *pos += DIFF_WORD_HEADER_SIZE;
    } else if (b & LEN_BYTE_FLAG) {
        len = b & LEN_SHORT_MASK;
        len <<= DIFF_BYTE_SHIFT;
        len |= buf[*pos + 1];
        *pos += DIFF_BYTE_HEADER_SIZE;
    } else {
        len = b & LEN_SHORT_MASK;
        (*pos)++;
    }
    return len;
}

void CreateDiffFile(
    char* oldName,
    char* joinName,
    char* diffName,
    i32 remotePlayer,
    i32 forceWhole
) {
    i32 joinSize;
    u8* fullData;
    i32 unusedVal;
    i32 inFd;
    i32l timeIn;
    i32 matchLen;
    u8* prevData;
    i32 diffTotal;
    i32 readFile;
    i32 length;
    u8* diffOut;
    i32 oldSize;
    i32 fullSend;
    i32 destFile;
    i32 position;

    timeIn = platform::Ticks();
    prevData = NULL;
    fullData = NULL;
    diffOut = NULL;
    oldSize = 0;
    joinSize = 0;
    diffTotal = 0;
    fullSend = 0;

    if (forceWhole || (iLastDiffSendTo != -1 && iLastDiffSendTo != remotePlayer))
        fullSend = 1;
    iLastDiffSendTo = remotePlayer;

    sprintf(gText, "%s%s", ".\\DATA\\", joinName);
    joinSize = FileSize(gText);
    fullData = static_cast<u8*>(H2_ALLOC(joinSize));
    sprintf(gText, "%s%s", ".\\DATA\\", joinName);
    readFile = platform::FileOpen(gText, platform::FileMode::Read);
    if (readFile == -1)
        FileError(gText);
    platform::FileRead(readFile, fullData, joinSize);
    platform::FileClose(readFile);
    LogInt(
        const_cast<char*>("Orig Join CRC"),
        calc_crc_long(fullData, joinSize),
        joinSize,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );

    if (!forceWhole) {
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        oldSize = FileSize(gText);
        prevData = static_cast<u8*>(H2_ALLOC(oldSize));
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        inFd = platform::FileOpen(gText, platform::FileMode::Read);
        if (inFd == -1)
            FileError(gText);
        platform::FileRead(inFd, prevData, oldSize);
        platform::FileClose(inFd);
    }

    diffOut = static_cast<u8*>(H2_ALLOC((oldSize > joinSize ? oldSize : joinSize) + DIFF_BUFFER_EXTRA));
    if (fullSend) {
        diffOut[0] = 0;
        diffOut[1] = 0;
        memcpy(diffOut + JOIN_HEADER_SIZE, fullData, joinSize);
        diffTotal = joinSize + JOIN_HEADER_SIZE;
    } else {
        diffOut[0] = 1;
        diffOut[1] = 0;
        diffTotal = JOIN_HEADER_SIZE;
        position = 0;
        length = 0;
        matchLen = length;
        while (1) {
            if (position + length >= oldSize || position + length >= joinSize) {
                length = oldSize - position;
                WriteDiffHeaderInfo(1, length, diffOut, &diffTotal);
                memcpy(diffOut + diffTotal, fullData + position, length);
                diffTotal += length;
                position += length;
                length = 0;
                goto Finish;
            }
            if (*(prevData + position + length) == *(fullData + position + length)) {
                matchLen = 1;
                while (position + length + matchLen < oldSize
                       && position + length + matchLen < joinSize
                       && *(prevData + position + length + matchLen)
                              == *(fullData + position + length + matchLen))
                    matchLen++;
                if (matchLen <= DIFF_MAX_SHORT_MATCH) {
                    length += matchLen;
                    matchLen = 0;
                } else {
                    if (length != 0) {
                        WriteDiffHeaderInfo(1, length, diffOut, &diffTotal);
                        memcpy(diffOut + diffTotal, fullData + position, length);
                        diffTotal += length;
                        position += length;
                        length = 0;
                    }
                    WriteDiffHeaderInfo(0, matchLen, diffOut, &diffTotal);
                    position += matchLen;
                    matchLen = 0;
                }
            } else {
                while (position + length < oldSize && position + length < joinSize
                       && *(prevData + position + length)
                              != *(fullData + position + length))
                    length++;
            }
        }
    }

Finish:
    sprintf(gText, "%s%s", ".\\DATA\\", diffName);
    destFile = platform::FileOpen(gText, platform::FileMode::Write);
    if (destFile == -1)
        FileError(gText);
    platform::FileWrite(destFile, diffOut, diffTotal);
    platform::FileClose(destFile);

    sprintf(gText, "%s%s", ".\\DATA\\", oldName);
    destFile = platform::FileOpen(gText, platform::FileMode::Write);
    if (destFile == -1)
        FileError(gText);
    platform::FileWrite(destFile, fullData, joinSize);
    platform::FileClose(destFile);

    if (prevData != NULL)
        H2_FREE(prevData);
    if (fullData != NULL)
        H2_FREE(fullData);
    if (diffOut != NULL)
        H2_FREE(diffOut);
    return;
}

void CreateJoinFile(char* oldName, char* diffName, char* joinName) {
    u8* oldBuf = NULL;
    u8 copyFlag;
    u8* diffData = NULL;
    u8* outData = NULL;
    i32 outSize = 0;
    i32 diffLength;
    i32 diffFile;
    i32 copyLength;
    i32 oldSize;
    i32 position;
    i32 joinFile;

    sprintf(gText, "%s%s", ".\\DATA\\", diffName);
    diffLength = FileSize(gText);
    diffData = static_cast<u8*>(H2_ALLOC(diffLength));
    sprintf(gText, "%s%s", ".\\DATA\\", diffName);
    diffFile = platform::FileOpen(gText, platform::FileMode::Read);
    if (diffFile == -1)
        FileError(gText);
    platform::FileRead(diffFile, diffData, diffLength);
    platform::FileClose(diffFile);

    outData = static_cast<u8*>(H2_ALLOC(JOIN_BUFFER_SIZE));
    if (diffData[0] == 0) {
        memcpy(outData, diffData + JOIN_HEADER_SIZE, diffLength - JOIN_HEADER_SIZE);
        outSize = diffLength - JOIN_HEADER_SIZE;
    } else {
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        oldSize = FileSize(gText);
        oldBuf = static_cast<u8*>(H2_ALLOC(oldSize));
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        diffFile = platform::FileOpen(gText, platform::FileMode::Read);
        if (diffFile == -1)
            FileError(gText);
        platform::FileRead(diffFile, oldBuf, oldSize);
        platform::FileClose(diffFile);
        memcpy(outData, oldBuf, oldSize);

        position = JOIN_HEADER_SIZE;
        while (position < diffLength) {
            copyFlag = diffData[position] >> DIFF_COPY_FLAG_SHIFT;
            copyLength = GetSkipCopyLen(diffData, &position);
            if (copyFlag) {
                memcpy(outData + outSize, diffData + position, copyLength);
                outSize += copyLength;
                position += copyLength;
            } else {
                outSize += copyLength;
            }
        }
    }

    sprintf(gText, "%s%s", ".\\DATA\\", joinName);
    joinFile = platform::FileOpen(gText, platform::FileMode::Write);
    if (joinFile == -1)
        FileError(gText);
    platform::FileWrite(joinFile, outData, outSize);
    platform::FileClose(joinFile);
    LogInt(
        const_cast<char*>("New Join CRC"),
        calc_crc_long(outData, outSize),
        outSize,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );

    sprintf(gText, "%s%s", ".\\DATA\\", oldName);
    joinFile = platform::FileOpen(gText, platform::FileMode::Write);
    if (joinFile == -1)
        FileError(gText);
    platform::FileWrite(joinFile, outData, outSize);
    platform::FileClose(joinFile);

    if (oldBuf)
        H2_FREE(oldBuf);
    if (diffData)
        H2_FREE(diffData);
    if (outData)
        H2_FREE(outData);
}

i32 game::HeroIDToHeroPos(playerData* pd, i32 heroId) {
    i32 i;
    for (i = 0; i < pd->m_heroCount; i++) {
        if (pd->m_heroIds[i] == heroId)
            return i;
    }
    return -1;
}

i32 game::TownIDToTownPos(playerData* pd, i32 townId) {
    i32 i;
    for (i = 0; i < pd->m_townCount; i++) {
        if (pd->m_townIds[i] == townId)
            return i;
    }
    return -1;
}

void game::SetupNewRumour(void) {
    i32 roll;
    i32 attempts8;
    rumourEventExtra* event0;
    i32 eventIndex;
    i32 selectionRoll7;
    i32l categoryStats[GAME_PLAYER_COUNT];
    i32 direction;
    i8 categoryOrder[RUMOUR_CATEGORY_ORDER_CAPACITY];
    if (m_rumourEventCount != 0 && Random(0, 9) < static_cast<i32>(m_rumourEventCount)) {
        attempts8 = 0;
        while (attempts8++ < 200) {
            if (m_rumourEventCount > 1)
                eventIndex = Random(0, m_rumourEventCount - 1);
            else
                eventIndex = 0;
            event0 =
                reinterpret_cast<rumourEventExtra*>(ppMapExtra[m_rumourEventIndices[eventIndex]]);
            if (strlen(event0->text) > 2 && event0->text[0] != '@') {
                const std::string rumour = localization::DecodeExternalText(event0->text);
                utf8::Copy(m_rumour, sizeof(m_rumour), rumour.c_str());
                event0->text[0] = '@';
                return;
            }
        }
    }

    if (Random(0, 100) < 30) {
        strcpy(m_rumour, cRandomTavernText[(giCurTurn / 7) % 8]);
    } else {
        roll = Random(0, 100);
        if (roll < 80 && giCurTurn > 1) {
            attempts8 = 0;
            while (attempts8++ < 200) {
                selectionRoll7 = Random(
                    H2EnumIndex(THIEVES_CATEGORY_OBELISKS), H2EnumIndex(THIEVES_CATEGORY_INCOME)
                );
                GetCategoryStats(
                    static_cast<TownThievesGuildCategory>(selectionRoll7),
                    categoryStats,
                    categoryOrder
                );
                SortStats(categoryStats, categoryOrder);
                if (categoryStats[0] != categoryStats[1]) {
                    if (selectionRoll7 == H2EnumIndex(THIEVES_CATEGORY_OBELISKS))
                        sprintf(
                            m_rumour,
                            localization::Tr("rumor.leader.obelisks"),
                            cPlayerNames[categoryOrder[0]]
                        );
                    else if (selectionRoll7 == H2EnumIndex(THIEVES_CATEGORY_ARTIFACTS))
                        sprintf(
                            m_rumour,
                            localization::Tr("rumor.leader.artifacts"),
                            cPlayerNames[categoryOrder[0]]
                        );
                    else if (selectionRoll7 == H2EnumIndex(THIEVES_CATEGORY_ARMY_STRENGTH))
                        sprintf(
                            m_rumour,
                            localization::Tr("rumor.leader.army"),
                            cPlayerNames[categoryOrder[0]]
                        );
                    else
                        sprintf(
                            m_rumour,
                            localization::Tr("rumor.leader.income"),
                            cPlayerNames[categoryOrder[0]]
                        );
                    return;
                }
            }
            goto ultimateRumour;
        } else {
        ultimateRumour:
            selectionRoll7 = Random(0, 100);
            if (selectionRoll7 < 33) {
                if (!(m_ultimateArtifactX >= H2EnumIndex(m_mapHeader.width) * 0.33
                      || m_ultimateArtifactX >= H2EnumIndex(m_mapHeader.height) * 0.33)) {
                    direction = 7;
                } else if (!(m_ultimateArtifactX >= H2EnumIndex(m_mapHeader.width) * 0.33
                             || m_ultimateArtifactX <= H2EnumIndex(m_mapHeader.height)
                                    * GAME_ULTIMATE_ARTIFACT_TWO_THIRDS)) {
                    direction = 5;
                } else if (!(m_ultimateArtifactX >= H2EnumIndex(m_mapHeader.width) * 0.33)) {
                    direction = 6;
                } else if (!(m_ultimateArtifactX <= H2EnumIndex(m_mapHeader.width)
                                 * GAME_ULTIMATE_ARTIFACT_TWO_THIRDS
                             || m_ultimateArtifactX >= H2EnumIndex(m_mapHeader.height) * 0.33)) {
                    direction = 1;
                } else if (!(m_ultimateArtifactX <= H2EnumIndex(m_mapHeader.width)
                                 * GAME_ULTIMATE_ARTIFACT_TWO_THIRDS
                             || m_ultimateArtifactX <= H2EnumIndex(m_mapHeader.height)
                                    * GAME_ULTIMATE_ARTIFACT_TWO_THIRDS)) {
                    direction = 3;
                } else if (!(m_ultimateArtifactX <= H2EnumIndex(m_mapHeader.width)
                                 * GAME_ULTIMATE_ARTIFACT_TWO_THIRDS)) {
                    direction = 2;
                } else if (!(m_ultimateArtifactX >= H2EnumIndex(m_mapHeader.height) * 0.33)) {
                    direction = 0;
                } else if (!(m_ultimateArtifactX <= H2EnumIndex(m_mapHeader.height)
                                 * GAME_ULTIMATE_ARTIFACT_TWO_THIRDS)) {
                    direction = 4;
                } else {
                    direction = 8;
                }
                sprintf(
                    m_rumour,
                    localization::Tr("rumor.ultimate_artifact.region"),
                    cDirections[direction]
                );
            } else if (selectionRoll7 < 66) {
                sprintf(
                    m_rumour,
                    localization::Tr("rumor.ultimate_artifact.terrain"),
                    cRumourTerrainDescriptions
                        [H2EnumIndex(giGroundToTerrain
                                 [gpAdvManager
                                      ->GetCell(m_ultimateArtifactX, m_ultimateArtifactY)
                                      ->m_terrainImageIndex])]
                );
            } else if (m_ultimateArtifactId != ARTIFACT_NONE) {
                sprintf(
                    m_rumour,
                    localization::Tr("rumor.ultimate_artifact.identity"),
                    gArtifactNames[H2EnumIndex(m_ultimateArtifactId)]
                );
            } else {
                strcpy(m_rumour, cRandomTavernText[(giCurTurn / 7) % 8]);
            }
        }
    }
}

EventExtra* GetMapEvent(i32 x, i32 y) {
    EventExtra* ev;
    i32 i;
    for (i = 0; i < gpGame->m_mapEventCount; i++) {
        ev = reinterpret_cast<EventExtra*>(ppMapExtra[gpGame->m_mapEventIndices[i]]);
        if (ev->x == x && ev->y == y && ev->active != 0
            && ev->players[gpGame->m_players[static_cast<i8>(giCurPlayer)].m_color] != 0)
            return ev;
    }
    return NULL;
}

void game::CheckForTimeEvent(void) {
    timeEventExtra* event0;
    i32 dayNumber4;
    i32 eventIndex5;
    i32 resourceIndex;
    i32 primaryAmount2;
    i32 primaryType9;
    i32 secondaryType4;
    i32 secondaryAmount;
    i32 resourceAmount;

    dayNumber4 = m_day + (m_week - 1) * EVENT_DAYS_PER_WEEK
                 + (m_month - 1) * EVENT_DAYS_PER_MONTH;
    for (eventIndex5 = 0; eventIndex5 < m_timeEventCount; eventIndex5++) {
        event0 = static_cast<timeEventExtra*>(ppMapExtra[m_timeEventIndices[eventIndex5]]);
        if (((gbHumanPlayer[giCurPlayer] && event0->appliesToHuman)
             || (!gbHumanPlayer[giCurPlayer] && event0->appliesToComputer))
            && event0->players[GetPlayerColor(static_cast<i8>(giCurPlayer))]
            && (event0->firstDay == dayNumber4
                || (event0->repeatInterval != 0 && dayNumber4 > event0->firstDay
                    && (dayNumber4 - event0->firstDay) % event0->repeatInterval == 0))) {
            primaryType9 = -1;
            primaryAmount2 = 0;
            secondaryType4 = -1;
            secondaryAmount = 0;
            for (resourceIndex = 0; resourceIndex < EVENT_RESOURCE_COUNT;
                 resourceIndex++) {
                resourceAmount = event0->resources[resourceIndex];
                if (-resourceAmount
                    > gpGame->m_players[giCurPlayer].m_resources[resourceIndex]) {
                    resourceAmount = -gpGame->m_players[giCurPlayer].m_resources[resourceIndex];
                }
                gpGame->m_players[giCurPlayer].m_resources[resourceIndex] +=
                    event0->resources[resourceIndex];
                if (gpGame->m_players[giCurPlayer].m_resources[resourceIndex] < 0)
                    gpGame->m_players[giCurPlayer].m_resources[resourceIndex] = 0;
                if (resourceAmount != 0) {
                    if (primaryType9 != -1) {
                        secondaryType4 = primaryType9;
                        secondaryAmount = primaryAmount2;
                    }
                    primaryType9 = resourceIndex;
                    primaryAmount2 = resourceAmount;
                }
            }
            if (primaryType9 >= 0 && primaryType9 <= EVENT_RESOURCE_COUNT - 1
                && primaryAmount2 < 0) {
                primaryAmount2 -= EVENT_RESOURCE_PENALTY;
            }
            if (secondaryType4 >= 0 && secondaryType4 <= EVENT_RESOURCE_COUNT - 1
                && secondaryAmount < 0) {
                secondaryAmount -= EVENT_RESOURCE_PENALTY;
            }
            if (gbThisNetHumanPlayer[giCurPlayer]) {
                const std::string eventMessage =
                    localization::DecodeExternalText(event0->message);
                NormalDialog(
                    const_cast<char*>(eventMessage.c_str()),
                    1,
                    -1,
                    -1,
                    primaryType9,
                    primaryAmount2,
                    secondaryType4,
                    secondaryAmount,
                    -1,
                    0
                );
            }
        }
    }
}

void CheckValidAvailableHeroes(void) {
    i32 candidatePlayer0;
    i32 heroIndex5;
    i32 availableSlot13;
    i32 heroPlayer26;

    for (heroPlayer26 = 0; heroPlayer26 < gpGame->m_playerCount; heroPlayer26++) {
        for (heroIndex5 = 0; heroIndex5 < gpGame->m_players[heroPlayer26].m_heroCount;
             heroIndex5++) {
            for (candidatePlayer0 = 0; candidatePlayer0 < gpGame->m_playerCount;
                 candidatePlayer0++) {
                for (availableSlot13 = 0; availableSlot13 < AVAILABLE_HERO_SLOTS;
                     availableSlot13++) {
                    if (gpGame->m_players[candidatePlayer0].m_availableHeroIds[availableSlot13]
                        == gpGame->m_players[heroPlayer26].m_heroIds[heroIndex5]) {
                        gpGame->m_players[candidatePlayer0].m_availableHeroIds[availableSlot13] =
                            static_cast<i8>(gpGame->GetNewHeroId(heroPlayer26, FACTION_ANY, 0));
                    }
                }
            }
        }
    }
}

i32 CalcFileCRC(char* file) {
    i32l len = FileSize(file);
    char* blk = static_cast<char*>(H2_ALLOC(len));
    i32 fp = platform::FileOpen(file, platform::FileMode::Read);
    if (fp == -1)
        FileError(file);
    platform::FileRead(fp, blk, len);
    i32 checksum = calc_crc_long(reinterpret_cast<u8*>(blk), len);
    platform::FileClose(fp);
    H2_FREE(blk);
    return checksum;
}

void CompressTest2(void) {
    i32l plainSize;
    char* unpackedData;
    i32l compSize;
    i32 srcCrc;
    i32 dataSz;
    i32 index;
    i32 unpackedCrc;
    char* fromData;
    char* encoded;
    i32 srcCrcCheck;

    dataSz = Random(TEST_RANDOM_SIZE_MIN, TEST_RANDOM_SIZE_MAX);
    fromData =
        static_cast<char*>(
            H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
        );
    encoded =
        static_cast<char*>(
            H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
        );
    unpackedData =
        static_cast<char*>(
            H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
        );
    for (index = 0; index < dataSz; index++)
        fromData[index] = static_cast<char>(Random(0, 255));
    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
    compSize = EncodeData(encoded, fromData, dataSz);
    plainSize = DecodeData(unpackedData, encoded, compSize);
    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
    H2_FREE(fromData);
    H2_FREE(encoded);
    H2_FREE(unpackedData);
}

void CompressTest(void) {
    char* fromData;
    char* encoded;
    i32 srcCrcCheck;
    i32 unpackedCrc;
    i32 hFile;
    i32l fileSize;
    char diffName[TEST_FILENAME_SIZE];
    i32 srcCrc;
    char* unpackedData;
    i32l compSize;
    i32l plainSize;

    LogStr(const_cast<char*>("C1"));
    strcpy(diffName, "c:\\TEMP\\Z.DIF");
    fileSize = FileSize(diffName);
    fromData = static_cast<char*>(
        H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
    );
    encoded = static_cast<char*>(
        H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
    );
    unpackedData = static_cast<char*>(
        H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
    );
    LogStr(const_cast<char*>("C2"));
    hFile = platform::FileOpen(diffName, platform::FileMode::Read);
    if (hFile == -1)
        FileError(diffName);
    platform::FileRead(hFile, fromData, fileSize);
    LogStr(const_cast<char*>("C3"));
    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
    LogStr(const_cast<char*>("C4"));
    platform::FileClose(hFile);
    LogStr(const_cast<char*>("C5"));
    compSize = EncodeData(encoded, fromData, fileSize);
    LogStr(const_cast<char*>("C6"));
    plainSize = DecodeData(unpackedData, encoded, compSize);
    LogStr(const_cast<char*>("C7"));
    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
    H2_FREE(fromData);
    H2_FREE(encoded);
    H2_FREE(unpackedData);
    LogStr(const_cast<char*>("C8"));
}

void CompressTest3(void) {
    char buffer[TEST_MESSAGE_CAPACITY];
    i32 i;
    for (i = 0; i < COMPRESS_TEST_ITERATIONS; i++) {
        sprintf(buffer, "Test # %d", i);
        AiPrint(buffer);
        CompressTest2();
    }
}

i32 game::CountShrines(i32 player) {
    town* castle;
    i32 count;
    mapCell* cell;
    i32 row15;
    i32 col5;
    hero* occupier;

    if (xIsExpansionMap == 0)
        return 0;
    count = 0;
    for (row15 = 0; row15 < MAP_HEIGHT; row15++) {
        for (col5 = 0; col5 < MAP_WIDTH; col5++) {
            cell = WORLDMAP->GetCell(col5, row15);
            castle = NULL;
            if (cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)) {
                castle = GetCastle(cell->m_objectMetadata);
            } else if (cell->m_triggerType
                       == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION)) {
                occupier = gpGame->GetHero(cell->m_objectMetadata);
                if (occupier->m_locationType
                    == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE))
                    castle = GetCastle(occupier->m_occupiedTown);
            }
            if (castle != NULL && castle->m_owner == player
                && (castle->m_buildings & H2EnumIndex(TOWN_BUILDING_TAVERN))
                && castle->m_type == FACTION_NECROMANCER)
                count++;
        }
    }
    return count;
}

i8 giMonType[] = {
    H2EnumIndex(CREATURE_PEASANT),
    H2EnumIndex(CREATURE_TROLL),
    H2EnumIndex(CREATURE_DWARF),
    H2EnumIndex(CREATURE_ROC),
    H2EnumIndex(CREATURE_OGRE),
    H2EnumIndex(CREATURE_DRUID),
    H2EnumIndex(CREATURE_VAMPIRE),
    H2EnumIndex(CREATURE_WOLF),
    H2EnumIndex(CREATURE_CENTAUR),
    H2EnumIndex(CREATURE_GARGOYLE),
    H2EnumIndex(CREATURE_UNICORN),
    H2EnumIndex(CREATURE_LICH)
};
char bMapInitialized = 0;
i32 iViewArmyNumTroops;
i8 gbNGHeroType[GAME_PLAYER_COUNT];
SMonFrameInfo sViewArmyMonFrameInfo;
i16 giUABaseX;
i16 giUABaseY;
i32 giEndSequence;
b32 gbDismissArmy;
i8 gbNGHuman[GAME_PLAYER_COUNT];
i32 iViewArmyFrame;
b32 gbAllowUpgrade;
H2EnumStorage<CreatureType, i32> iViewArmyType;
class hero* viewSpellsHero;
b32 gbUpgradeArmy;
i16 RandMineQty[AI_RANDOM_MINE_TYPE_COUNT];
char gcCurMapName[GAME_CURRENT_MAP_NAME_SIZE];
i8 gbNGDifficulty[GAME_PLAYER_COUNT];
H2EnumStorage<CreatureType, i32> iViewArmyUpgradeToType;
i32 viewArmyBaseX;
i32 viewArmyBaseY;
i8 gbNGColor[GAME_PLAYER_COUNT];
i16 giUARadius;
i8 gbNGPlayerPos[GAME_PLAYER_COUNT];
i32 viewArmyFacingWIPXMod;
